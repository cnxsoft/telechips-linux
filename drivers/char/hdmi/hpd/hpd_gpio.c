//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file  hpd.c
 * @brief This file contains an implementation of HPD device driver.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/poll.h>

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/uaccess.h>

#include <mach/tcc_board_hdmi.h>
#include <mach/hpd.h>
#include <mach/gpio.h>

#include "../hdmi/regs-hdmi.h"

#define HPD_DEBUG 		0
#define HPD_DEBUG_GPIO 	0
#if HPD_DEBUG
#define DPRINTK(args...)    printk("hpd-gpio:" args)
#else
#define DPRINTK(args...)
#endif

static struct clk *hdmi_hpd_clk;


#define VERSION         "1.2" /* Driver version number */
#define HPD_MINOR       243 /* Major 10, Minor 243, /dev/hpd */

#define HPD_LO          0
#define HPD_HI          1


struct hpd_struct {
	spinlock_t lock;
	wait_queue_head_t waitq;
	atomic_t state;

};

static struct hpd_struct hpd_struct;

static int hpd_open(struct inode *inode, struct file *file);
static int hpd_release(struct inode *inode, struct file *file);
static ssize_t hpd_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
static int hpd_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static unsigned int hpd_poll(struct file *file, poll_table *wait);
static irqreturn_t hpd_irq_handler(int irq, void *dev_id);
static int hpd_start(void);
static int hpd_stop(void);

static int last_hpd_state;
static unsigned hpd_port;

static const struct file_operations hpd_fops =
{
    .owner   = THIS_MODULE,
    .open    = hpd_open,
    .release = hpd_release,
    .read    = hpd_read,
    .unlocked_ioctl = hpd_ioctl,
//    .poll    = hpd_poll,
};

static struct miscdevice hpd_misc_device =
{
    HPD_MINOR,
    "hpd",
    &hpd_fops,
};


int hpd_open(struct inode *inode, struct file *file)
{
	DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

	clk_enable(hdmi_hpd_clk);

    return 0;
}

int hpd_release(struct inode *inode, struct file *file)
{

	DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

	clk_disable(hdmi_hpd_clk);

    return 0;
}

ssize_t hpd_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
    ssize_t retval = 0;


    spin_lock_irq(&hpd_struct.lock);

	if(gpio_get_value(hpd_port) && (last_hpd_state  == 1))
		atomic_set(&hpd_struct.state, 1);
	else
		atomic_set(&hpd_struct.state, 0);

	//printk(KERN_INFO "%s register :0x%x hpd_port:0x%x\n", __FUNCTION__, atomic_read(&hpd_struct.state), hpd_port);

	retval = put_user(atomic_read(&hpd_struct.state), (unsigned int __user *) buffer);

	atomic_set(&hpd_struct.state, -1);

    spin_unlock_irq(&hpd_struct.lock);

    return retval;
}

unsigned int hpd_poll(struct file *file, poll_table *wait)
{
    DPRINTK(KERN_INFO "%s \n", __FUNCTION__);

 // poll_wait(file, &hpd_struct.waitq, wait);

    if (atomic_read(&hpd_struct.state) != -1)
        return POLLIN | POLLRDNORM;

    return 0;
}

int hpd_start(void)
{
	unsigned char reg;

	DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

	/* disable HPD interrupts */
	reg = readb(HDMI_SS_INTC_CON);
	reg &= ~(1<<HDMI_IRQ_HPD_PLUG);
	reg &= ~(1<<HDMI_IRQ_HPD_UNPLUG);
	writeb(reg, HDMI_SS_INTC_CON);
	
	last_hpd_state  = 1;

    return 0;
}

int hpd_stop(void)
{
	DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    return 0;
}

int hpd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    switch (cmd) {
		case HPD_IOC_START:
			hpd_start();
			break;
		case HPD_IOC_STOP:
			hpd_stop();
			break;
        default:
            return -EINVAL;	
	}

	return 0;
}

/**
 * @brief HPD interrupt handler
 *
 * Handles interrupt requests from HPD hardware. \n
 * Handler changes value of internal variable and notifies waiting thread.
 */
static irqreturn_t hpd_irq_handler(int irq, void *dev_id)
{
     return IRQ_HANDLED;
}


static int hpd_probe(struct platform_device *pdev)
{
	unsigned int reg;
	struct tcc_hpd_platform_data *hpd_dev;
	hpd_dev = pdev->dev.platform_data;

    if (!machine_is_hdmidp())
        return -ENODEV;

    printk(KERN_INFO "HPD Driver ver. %s (built %s %s)\n", VERSION, __DATE__, __TIME__);

	hdmi_hpd_clk = clk_get(0, "hdmi");
	
	clk_enable(hdmi_hpd_clk);
	hpd_port = hpd_dev->hpd_port;

	gpio_request(hpd_port, "hpd");

	#if defined(CONFIG_MACH_M805_892X)
	tcc_gpio_config(hpd_port, GPIO_FN(0)|GPIO_PULLDOWN);
	#else
	tcc_gpio_config(hpd_port, GPIO_FN(0));
	#endif

	gpio_direction_input(hpd_port);

    if (misc_register(&hpd_misc_device))    {
        printk(KERN_WARNING "HPD: Couldn't register device 10, %d.\n", HPD_MINOR);
        return -EBUSY;
    }
    spin_lock_init(&hpd_struct.lock);
	
    /* disable HPD interrupts */
    reg = readb(HDMI_SS_INTC_CON);
    reg &= ~(1<<HDMI_IRQ_HPD_PLUG);
    reg &= ~(1<<HDMI_IRQ_HPD_UNPLUG);
    writeb(reg, HDMI_SS_INTC_CON);
    atomic_set(&hpd_struct.state, -1);

	clk_disable(hdmi_hpd_clk);

    return 0;
}

static int hpd_remove(struct platform_device *pdev)
{
    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    misc_deregister(&hpd_misc_device);
	return 0;
}


/* suspend and resume support for the lcd controller */
static int hpd_suspend(struct platform_device *dev, pm_message_t state)
{
	last_hpd_state  = 0;
	return 0;
}

static int hpd_resume(struct platform_device *dev)
{
	return 0;
}


static struct platform_driver tcc_hdmi_hpd = {
	.probe	= hpd_probe,
	.remove	= hpd_remove,
	.suspend	= hpd_suspend,
	.resume	= hpd_resume,	
	.driver	= {
		.name	= "tcc_hdmi_hpd",
		.owner	= THIS_MODULE,
	},
};
static __init int hpd_init(void)
{
	return platform_driver_register(&tcc_hdmi_hpd);
}

static __exit void hpd_exit(void)
{
	platform_driver_unregister(&tcc_hdmi_hpd);
}


module_init(hpd_init);
module_exit(hpd_exit);
MODULE_AUTHOR("Telechips Inc. <linux@telechips.com>");
MODULE_LICENSE("GPL");

