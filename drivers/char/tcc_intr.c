/*
 * linux/drivers/char/tcc_intr.c 
 *
 * Author:  <linux@telechips.com>
 * Created: 31th March, 2009 
 * Description: User-level interrupt Driver 
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/poll.h>
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include <mach/bsp.h>
#include <mach/irqs.h>
#include <mach/tca_intr.h>

#define TCC_INTR_VERSION	"2.0"
#define TCC_INTR_DEV_NAME	"tcc_intr"
#define TCC_INTR_DEV_BROADCAST	"tcc-intr-bc"

static irqreturn_t bc_handler(int irq, void *dev_id)
{
	tcc_intr_data_t *tcc_intr = dev_id;

	spin_lock_irq(&(tcc_intr->lock));
	tcc_intr->count++;
	spin_unlock_irq(&(tcc_intr->lock));

	wake_up_interruptible(&(tcc_intr->wq));

	return IRQ_HANDLED;
}

static unsigned int intr_poll(struct file *filp, poll_table *wait)
{
	tcc_intr_data_t *tcc_intr = (tcc_intr_data_t *)filp->private_data;

	if (tcc_intr == NULL)
	{
		return -EFAULT;
	}
	if (tcc_intr->count > 0) {
		spin_lock_irq(&(tcc_intr->lock));
		tcc_intr->count--;
		spin_unlock_irq(&(tcc_intr->lock));
		return (POLLIN | POLLRDNORM);
	}
	
	poll_wait(filp, &(tcc_intr->wq), wait);

	if (tcc_intr->count > 0) {
		spin_lock_irq(&(tcc_intr->lock));
		tcc_intr->count--;
		spin_unlock_irq(&(tcc_intr->lock));
		return (POLLIN | POLLRDNORM);
	} else {
		return 0;
	}
}

static int intr_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned long data;
	tcc_intr_data_t *tcc_intr = (tcc_intr_data_t *)filp->private_data;

	switch (cmd) {
		case IOCTL_INTR_SET:
			if (copy_from_user((void *)&data, (const void *)arg, sizeof(data)))
				return -EFAULT;
			if (data == 0 || data == 1) io_interrupt(tcc_intr, data);
			else return -EFAULT;
			break;
		case IOCTL_INTR_GET:
			data = io_interrupt(tcc_intr, 2);
			if (copy_to_user((void *)arg, (const void *)&data, sizeof(data)))
				return -EFAULT;
			break;
		default:
			printk("tcc_intr(%d): unrecognized ioctl (0x%x)\n", MINOR(inode->i_rdev), cmd);
			return -EINVAL;
			break;
	}

	return 0;
}

static int intr_release(struct inode *inode, struct file *filp)
{
	tcc_intr_data_t *tcc_intr = (tcc_intr_data_t *)filp->private_data;

	free_irq(tcc_intr->irq, tcc_intr);
	kfree(tcc_intr);

	return 0;
}

static int intr_bc_open(struct inode *inode, struct file *filp)
{
	int ret = 0;
	tcc_intr_data_t *bc_data = NULL;

	bc_data = (tcc_intr_data_t *)kmalloc(sizeof(tcc_intr_data_t), GFP_KERNEL);
	if (bc_data == NULL) {
		ret = -ENOMEM;
		goto error;
	}
	memset(bc_data, 0, sizeof(tcc_intr_data_t));

	spin_lock_init(&(bc_data->lock));
	init_waitqueue_head(&(bc_data->wq));

	init_bc_interrupt(bc_data);
	ret = request_irq(bc_data->irq, bc_handler, IRQF_DISABLED, TCC_INTR_DEV_BROADCAST, bc_data);
	if (ret) {
		//ZzaU :: exception process in case this abnormally stop!!for class concept in android!!
		printk("FAILED to aquire irq\n");
		//ret = -EFAULT;	goto error;
	}

	filp->private_data = (void *)bc_data;
	
	return 0;
error:
	if (bc_data) 
		kfree(bc_data);
	return ret;
}

struct file_operations intr_bc_fops =
{
	.owner		= THIS_MODULE,
	.poll		= intr_poll,
	.unlocked_ioctl		= intr_ioctl,
	.open		= intr_bc_open,
	.release	= intr_release,
};

static int tcc_intr_open(struct inode *inode, struct file *filp)
{
	switch (MINOR(inode->i_rdev)) {
		case 3: filp->f_op = &intr_bc_fops; break;
		default : return -ENXIO;
	}

	if (filp->f_op && filp->f_op->open)
		return filp->f_op->open(inode, filp);

	return 0;
}

struct file_operations tcc_intr_fops =
{
	.owner		= THIS_MODULE,
	.open		= tcc_intr_open,
};

static struct class *intr_class;

static int __init tcc_intr_init(void)
{
	int res;

	res = register_chrdev(TCC_INTR_DEV_MAJOR, TCC_INTR_DEV_NAME, &tcc_intr_fops);
	if (res < 0)
		return res;

	intr_class = class_create(THIS_MODULE, TCC_INTR_DEV_NAME);

	device_create(intr_class, NULL, MKDEV(TCC_INTR_DEV_MAJOR, 3), NULL, TCC_INTR_DEV_BROADCAST);
	printk("tcc_intr: init (ver %s)\n", TCC_INTR_VERSION);

    return 0;
}

static void __exit tcc_intr_exit(void)
{
	unregister_chrdev(TCC_INTR_DEV_MAJOR, TCC_INTR_DEV_NAME);
    printk("tcc_intr: exit\n");
}


module_init(tcc_intr_init);
module_exit(tcc_intr_exit);

MODULE_AUTHOR("Telechips Inc. SYS4-3 linux@telechips.com");
MODULE_DESCRIPTION("Telechips user level interrut driver");
MODULE_LICENSE("GPL");
