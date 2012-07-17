//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file hdmi.c
 * @brief This file implements HDMI device driver.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/uaccess.h>

#include "../hdmi/regs-hdmi.h"

#include <mach/cec.h>
#include <mach/gpio.h>

#include "regs-cec.h"

#include "tcc_cec_interface.h"

#define CEC_DEBUG 0

#if CEC_DEBUG
#define DPRINTK(args...)    printk(args)
#else
#define DPRINTK(args...)
#endif


#define VERSION   "1.0" /* Driver version number */
#define CEC_MINOR 242 /* Major 10, Minor 242, /dev/cec */

#define CEC_MESSAGE_BROADCAST_MASK    0x0F
#define CEC_MESSAGE_BROADCAST         0x0F

#define CEC_FILTER_THRESHOLD          0x15


#if defined(CONFIG_ARCH_TCC88XX)
#define CEC_GPIO_COMMON		TCC_GPD(24)
#elif defined(CONFIG_ARCH_TCC93XX)
#define CEC_GPIO_COMMON		TCC_GPA(6)
#elif defined(CONFIG_ARCH_TCC92XX)
#define CEC_GPIO_IN				TCC_GPA(7)
#define CEC_GPIO_OUT			TCC_GPA(6)
#elif defined(CONFIG_ARCH_TCC892X)
#define CEC_GPIO_COMMON		TCC_GPHDMI(0)
#else
#error : not define HDMI CEC GPIO
#endif /* CONFIG_ARCH_TCC88XX */



/**
 * @enum cec_state
 * Defines all possible states of CEC software state machine
 */
enum cec_state {
    STATE_RX,
    STATE_TX,
    STATE_DONE,
    STATE_ERROR
};

/**
 * @struct cec_rx_struct
 * Holds CEC Rx state and data
 */
struct cec_rx_struct {
    spinlock_t lock;
    wait_queue_head_t waitq;
    atomic_t state;
    u8 *buffer;
    unsigned int size;
	unsigned int flag;
};

/**
 * @struct cec_tx_struct
 * Holds CEC Tx state and data
 */
struct cec_tx_struct {
    wait_queue_head_t waitq;
    atomic_t state;
};

static struct clk *ddi_clk;
static struct clk *hdmi_cec_clk;

static struct cec_rx_struct cec_rx_struct;
static struct cec_tx_struct cec_tx_struct;

static int cec_open(struct inode *inode, struct file *file);
static int cec_release(struct inode *inode, struct file *file);
static ssize_t cec_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
static ssize_t cec_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos);
static int cec_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static unsigned int cec_poll(struct file *file, poll_table *wait);

static irqreturn_t cec_irq_handler(int irq, void *dev_id);

static void cec_start(void);
static void cec_stop(void);

inline static void cec_set_divider(void);
inline static void cec_enable_interrupts(void);
inline static void cec_disable_interrupts(void);
inline static void cec_enable_rx(void);
inline static void cec_mask_rx_interrupts(void);
inline static void cec_unmask_rx_interrupts(void);
inline static void cec_mask_tx_interrupts(void);
inline static void cec_unmask_tx_interrupts(void);
inline static void cec_set_rx_state(enum cec_state state);
inline static void cec_set_tx_state(enum cec_state state);

static const struct file_operations cec_fops =
{
    .owner          = THIS_MODULE,
    .open           = cec_open,
    .release        = cec_release,
    .read           = cec_read,
    .write          = cec_write,
    .unlocked_ioctl = cec_ioctl,
    .poll           = cec_poll,
};

static struct miscdevice cec_misc_device =
{
    .minor = CEC_MINOR,
    .name  = "CEC",
    .fops  = &cec_fops,
};


int cec_open(struct inode *inode, struct file *file)
{
	DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

	clk_enable(hdmi_cec_clk);

    return 0;
}

int cec_release(struct inode *inode, struct file *file)
{
    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

	clk_disable(hdmi_cec_clk);

    return 0;
}

ssize_t cec_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
    ssize_t retval;

	if( cec_rx_struct.flag != 1)
		return 0;

    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

#if 0
    if (wait_event_interruptible(cec_rx_struct.waitq, atomic_read(&cec_rx_struct.state) == STATE_DONE)) {
        return -ERESTARTSYS;
    }
#endif /* 0 */

    spin_lock_irq(&cec_rx_struct.lock);

    if (cec_rx_struct.size > count) {
        spin_unlock_irq(&cec_rx_struct.lock);
        return -1;
    }

    if (copy_to_user(buffer, cec_rx_struct.buffer, cec_rx_struct.size)) {
        spin_unlock_irq(&cec_rx_struct.lock);
        printk(KERN_ERR "CEC: copy_to_user() failed!\n");
        return -EFAULT;
    }

    retval = cec_rx_struct.size;
    cec_set_rx_state(STATE_RX);
    spin_unlock_irq(&cec_rx_struct.lock);

	cec_rx_struct.flag = 0;

    return retval;
}

ssize_t cec_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    char *data;
    unsigned char reg;
    int i = 0;

    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    /* check data size */
    if (count > CEC_TX_BUFF_SIZE || count == 0)
        return -1;

    data = kmalloc(count, GFP_KERNEL);
    if (!data)
    {
        printk(KERN_ERR "CEC: kmalloc() failed!\n");
        return -1;
    }

    if (copy_from_user(data, buffer, count))
    {
        printk(KERN_ERR "CEC: copy_from_user() failed!\n");
        kfree(data);
        return -EFAULT;
    }

    /* copy packet to hardware buffer */
    while (i < count) {
        writeb(data[i], CEC_TX_BUFF0 + (i*4));
        i++;
    }

    /* set number of bytes to transfer */
    writeb(count, CEC_TX_BYTES);

    cec_set_tx_state(STATE_TX);

    /* start transfer */
    reg = readb(CEC_TX_CTRL);
    reg |= CEC_TX_CTRL_START;

    /* if message is broadcast message - set corresponding bit */
    if ((data[0] & CEC_MESSAGE_BROADCAST_MASK) == CEC_MESSAGE_BROADCAST)
        reg |= CEC_TX_CTRL_BCAST;
    else
        reg &= ~CEC_TX_CTRL_BCAST;

    /* set number of retransmissions */
    reg |= 0x50;

    writeb(reg, CEC_TX_CTRL);

    kfree(data);

    /* wait for interrupt */
    if (wait_event_interruptible(cec_tx_struct.waitq, atomic_read(&cec_tx_struct.state) != STATE_TX)) {
        return -ERESTARTSYS;
    }

    if (atomic_read(&cec_tx_struct.state) == STATE_ERROR) {
        return -1;
    }

    return count;
}

void cec_start(void)
{
    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

#if defined(CONFIG_ARCH_TCC88XX)
	tcc_gpio_config(CEC_GPIO_COMMON, GPIO_FN(5));
	gpio_direction_output(CEC_GPIO_COMMON, 0);
#elif defined(CONFIG_ARCH_TCC93XX)
	tcc_gpio_config(CEC_GPIO_COMMON, GPIO_FN(1));
	gpio_direction_output(CEC_GPIO_COMMON, 0);
#elif defined(CONFIG_ARCH_TCC92XX)
	tcc_gpio_config(CEC_GPIO_IN, GPIO_FN(1));
	tcc_gpio_config(CEC_GPIO_OUT, GPIO_FN(1));

	gpio_direction_output(CEC_GPIO_IN, 0);
	gpio_direction_output(CEC_GPIO_OUT, 1);	
#elif defined(CONFIG_ARCH_TCC892X)
	tcc_gpio_config(CEC_GPIO_COMMON, GPIO_FN(1));
	gpio_direction_output(CEC_GPIO_COMMON, 0);
#else
#error : not define HDMI CEC GPIO
#endif /* CONFIG_ARCH_TCC88XX */

    writeb(CEC_RX_CTRL_RESET, CEC_RX_CTRL); // reset CEC Rx
    writeb(CEC_TX_CTRL_RESET, CEC_TX_CTRL); // reset CEC Tx

    cec_set_divider();

    writeb(CEC_FILTER_THRESHOLD, CEC_RX_FILTER_TH); // setup filter

    cec_enable_interrupts();

    cec_unmask_tx_interrupts();

    cec_set_rx_state(STATE_RX);
    cec_unmask_rx_interrupts();
    cec_enable_rx();

	cec_rx_struct.flag = 0;
}

void cec_stop(void)
{
    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    cec_mask_tx_interrupts();
    cec_mask_rx_interrupts();
    cec_disable_interrupts();

#if defined(CONFIG_ARCH_TCC92XX)
	tcc_gpio_config(CEC_GPIO_IN, GPIO_FN(0));
	tcc_gpio_config(CEC_GPIO_OUT, GPIO_FN(0));
#else
	tcc_gpio_config(CEC_GPIO_COMMON, GPIO_FN(0));
#endif /* CONFIG_ARCH_TCC88XX */

}

int cec_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    unsigned int laddr;
	unsigned int uiData;

    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    switch (cmd) {
		case CEC_IOC_START:
			cec_start();
			break;
		case CEC_IOC_STOP:
			cec_stop();
			break;
        case CEC_IOC_SETLADDR:
            DPRINTK(KERN_INFO "CEC: ioctl(CEC_IOC_SETLADDR)\n");
            if (get_user(laddr, (unsigned int __user *) arg))
                return -EFAULT;
            DPRINTK(KERN_INFO "CEC: logical address = 0x%02x\n", laddr);
            writeb(laddr & 0x0F, CEC_LOGIC_ADDR);
            break;
		case CEC_IOC_SENDDATA:
			DPRINTK(KERN_INFO "CEC: ioctl(CEC_IOC_SENDDATA)\n");
            if (get_user(uiData, (unsigned int __user *) arg))
                return -EFAULT;
			DPRINTK(KERN_INFO "CEC: SendData = 0x%02x\n", uiData);
			TccCECInterface_SendData( 0, uiData);
			break;
		case CEC_IOC_RECVDATA:
			break;
        default:
            return -EINVAL;
    }

    return 0;
}

unsigned int cec_poll(struct file *file, poll_table *wait)
{
//    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    poll_wait(file, &cec_rx_struct.waitq, wait);

    if (atomic_read(&cec_rx_struct.state) == STATE_DONE)
        return POLLIN | POLLRDNORM;

    return 0;
}

/**
 * @brief CEC interrupt handler
 *
 * Handles interrupt requests from CEC hardware. \n
 * Action depends on current state of CEC hardware.
 */
irqreturn_t cec_irq_handler(int irq, void *dev_id)
{
    u8 flag;
    u32 status;
#if (CEC_DEBUG)
	u32 tx_stat_23=0;
#endif
    /* read flag register */
    flag = readb(HDMI_SS_INTC_FLAG);

    /* is this our interrupt? */
    if (!(flag & (1<<HDMI_IRQ_CEC))) {
        return IRQ_NONE;
    }

    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    status = readb(CEC_STATUS_0);
    status |= readb(CEC_STATUS_1) << 8;
    status |= readb(CEC_STATUS_2) << 16;
    status |= readb(CEC_STATUS_3) << 24;

#if (CEC_DEBUG)
	tx_stat_23 = readb(CEC_TX_STAT0);
	tx_stat_23 = readb(CEC_TX_STAT1) << 8;

//	DPRINTK(KERN_INFO "CEC: status = 0x%x!\n", status);
	DPRINTK(KERN_INFO "CEC: status = 0x%x! [0x%x] \n", status, tx_stat_23);
#endif

    if (status & CEC_STATUS_TX_DONE) {
        if (status & CEC_STATUS_TX_ERROR) {
            DPRINTK(KERN_INFO "CEC: CEC_STATUS_TX_ERROR!\n");
			writeb(CEC_TX_CTRL_RESET, CEC_TX_CTRL); // reset CEC Tx

            cec_set_tx_state(STATE_ERROR);
        } else {
            DPRINTK(KERN_INFO "CEC: CEC_STATUS_TX_DONE!\n");
            cec_set_tx_state(STATE_DONE);
        }
        /* clear interrupt pending bit */
        writeb(CEC_IRQ_TX_DONE | CEC_IRQ_TX_ERROR, CEC_IRQ_CLEAR);
        wake_up_interruptible(&cec_tx_struct.waitq);
    }

    if (status & CEC_STATUS_RX_DONE) {
        if (status & CEC_STATUS_RX_ERROR) {
            DPRINTK(KERN_INFO "CEC: CEC_STATUS_RX_ERROR!\n");
            writeb(CEC_RX_CTRL_RESET, CEC_RX_CTRL); // reset CEC Rx
        } else {
            unsigned int size, i = 0;

            DPRINTK(KERN_INFO "CEC: CEC_STATUS_RX_DONE!\n");

            /* copy data from internal buffer */
            size = status >> 24;

            spin_lock(&cec_rx_struct.lock);

            while (i < size) {
                cec_rx_struct.buffer[i] = readb(CEC_RX_BUFF0 + (i*4));
                i++;
            }

    if (size > 0) {
        DPRINTK(KERN_INFO "fsize: %d ", size);
        DPRINTK(KERN_INFO "frame: ");
        for (i = 0; i < size; i++) {
            DPRINTK(KERN_INFO "0x%02x ", cec_rx_struct.buffer[i]);
        }
        DPRINTK(KERN_INFO "End ~\n");
    }

			TccCECInterface_ParseMessage(cec_rx_struct.buffer, size);
			
            cec_rx_struct.size = size;
            cec_set_rx_state(STATE_DONE);
			cec_rx_struct.flag = 1;

            spin_unlock(&cec_rx_struct.lock);

            cec_enable_rx();
        }
        /* clear interrupt pending bit */
        writeb(CEC_IRQ_RX_DONE | CEC_IRQ_RX_ERROR, CEC_IRQ_CLEAR);
        wake_up_interruptible(&cec_rx_struct.waitq);
    }

    return IRQ_HANDLED;
}

static int cec_probe(struct platform_device *pdev)
{
    u8 *buffer;


#if defined(CONFIG_ARCH_TCC92XX)
	gpio_request(CEC_GPIO_IN, "CEC_IN");
	gpio_request(CEC_GPIO_OUT, "CEC_OUT");

	gpio_direction_output(CEC_GPIO_IN, 0);
	gpio_direction_output(CEC_GPIO_OUT, 1);	
#else
	gpio_request(CEC_GPIO_COMMON, "CEC");
	gpio_direction_output(CEC_GPIO_COMMON, 1);

#endif /* CONFIG_ARCH_TCC88XX */


	
    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    if (!machine_is_hdmidp())
        return -ENODEV;

	hdmi_cec_clk = clk_get(0, "hdmi");
	ddi_clk = clk_get(0, "ddi");
	
	clk_enable(hdmi_cec_clk);

	printk(KERN_INFO "CEC Driver ver. %s (built %s %s)\n", VERSION, __DATE__, __TIME__);

	if (misc_register(&cec_misc_device))
	{
	    printk(KERN_WARNING "CEC: Couldn't register device 10, %d.\n", CEC_MINOR);
	    return -EBUSY;
	}

    cec_disable_interrupts();

	if (request_irq(IRQ_HDMI, cec_irq_handler, IRQF_SHARED, "cec", cec_irq_handler))
	{
	    printk(KERN_WARNING "CEC: IRQ %d is not free.\n", IRQ_HDMI);
	    misc_deregister(&cec_misc_device);
	    return -EIO;
	}

	init_waitqueue_head(&cec_rx_struct.waitq);
	spin_lock_init(&cec_rx_struct.lock);
	init_waitqueue_head(&cec_tx_struct.waitq);

	buffer = kmalloc(CEC_TX_BUFF_SIZE, GFP_KERNEL);
	if (!buffer)
	{
	    printk(KERN_ERR "CEC: kmalloc() failed!\n");
	    misc_deregister(&cec_misc_device);
	    return -EIO;
	}

	cec_rx_struct.buffer = buffer;
	cec_rx_struct.size   = 0;

	TccCECInterface_Init();

	clk_disable(hdmi_cec_clk);

    return 0;
}

static int cec_remove(struct platform_device *pdev)
{
    DPRINTK(KERN_INFO "%s\n", __FUNCTION__);

    free_irq(IRQ_HDMI, cec_irq_handler);
    misc_deregister(&cec_misc_device);

    kfree(cec_rx_struct.buffer);
}

/**
 * Set CEC divider value.
 */
void cec_set_divider(void)
{
    /**
     * (CEC_DIVISOR) * (clock cycle time) = 0.05ms \n
     * for 27MHz clock divisor is 0x0545
     */
	#if (0)
    writeb(0x05, CEC_DIVISOR_1);
    writeb(0x45, CEC_DIVISOR_0);
	#else
	
	unsigned int ddibus_clk;
	unsigned int div = 0;
	ddibus_clk = clk_get_rate(ddi_clk) / 100;
	//    ddibus_clk = tca_ckc_getfbusctrl(CLKCTRL1);

    ddibus_clk = ddibus_clk/10000;
    div = ddibus_clk * 50 - 1;

    DPRINTK("cec src clk = %dmhz\n", ddibus_clk);
    DPRINTK("cec divisor = %d\n", div);

	writeb((unsigned char)((div>>24)&0xff), CEC_DIVISOR_3);
	writeb((unsigned char)((div>>16)&0xff), CEC_DIVISOR_2);
	writeb((unsigned char)((div>> 8)&0xff), CEC_DIVISOR_1);
	writeb((unsigned char)((div>> 0)&0xff), CEC_DIVISOR_0);
	#endif
}

/**
 * Enable CEC interrupts
 */
void cec_enable_interrupts(void)
{
    unsigned char reg;
    reg = readb(HDMI_SS_INTC_CON);
    writeb(reg | (1<<HDMI_IRQ_CEC) | (1<<HDMI_IRQ_GLOBAL), HDMI_SS_INTC_CON);
}

/**
 * Disable CEC interrupts
 */
void cec_disable_interrupts(void)
{
    unsigned char reg;
    reg = readb(HDMI_SS_INTC_CON);
    writeb(reg & ~(1<<HDMI_IRQ_CEC), HDMI_SS_INTC_CON);
}

/**
 * Enable CEC Rx engine
 */
void cec_enable_rx(void)
{
    unsigned char reg;
    reg = readb(CEC_RX_CTRL);
    reg |= CEC_RX_CTRL_ENABLE;
    writeb(reg, CEC_RX_CTRL);
}

/**
 * Mask CEC Rx interrupts
 */
void cec_mask_rx_interrupts(void)
{
    unsigned char reg;
    reg = readb(CEC_IRQ_MASK);
    reg |= CEC_IRQ_RX_DONE;
    reg |= CEC_IRQ_RX_ERROR;
    writeb(reg, CEC_IRQ_MASK);
}

/**
 * Unmask CEC Rx interrupts
 */
void cec_unmask_rx_interrupts(void)
{
    unsigned char reg;
    reg = readb(CEC_IRQ_MASK);
    reg &= ~CEC_IRQ_RX_DONE;
    reg &= ~CEC_IRQ_RX_ERROR;
    writeb(reg, CEC_IRQ_MASK);
}

/**
 * Mask CEC Tx interrupts
 */
void cec_mask_tx_interrupts(void)
{
    unsigned char reg;
    reg = readb(CEC_IRQ_MASK);
    reg |= CEC_IRQ_TX_DONE;
    reg |= CEC_IRQ_TX_ERROR;
    writeb(reg, CEC_IRQ_MASK);
}

/**
 * Unmask CEC Tx interrupts
 */
void cec_unmask_tx_interrupts(void)
{
    unsigned char reg;
    reg = readb(CEC_IRQ_MASK);
    reg &= ~CEC_IRQ_TX_DONE;
    reg &= ~CEC_IRQ_TX_ERROR;
    writeb(reg, CEC_IRQ_MASK);
}

/**
 * Change CEC Tx state to state
 * @param state [in] new CEC Tx state.
 */
void cec_set_tx_state(enum cec_state state)
{
    atomic_set(&cec_tx_struct.state, state);
}

/**
 * Change CEC Rx state to @c state.
 * @param state [in] new CEC Rx state.
 */
void cec_set_rx_state(enum cec_state state)
{
    atomic_set(&cec_rx_struct.state, state);
}


static struct platform_driver tcc_hdmi_cec = {
	.probe	= cec_probe,
	.remove	= cec_remove,
	.driver	= {
		.name	= "tcc_hdmi_cec",
		.owner	= THIS_MODULE,
	},
};


static __init int cec_init(void)
{
	return platform_driver_register(&tcc_hdmi_cec);
}

static __exit void cec_exit(void)
{
	platform_driver_unregister(&tcc_hdmi_cec);
}


module_init(cec_init);
module_exit(cec_exit);

MODULE_AUTHOR("Telechips Inc. <linux@telechips.com>");
MODULE_LICENSE("GPL");


