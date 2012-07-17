/*
 * drivers/char/Tcc83xx_scaler.c
 *
 * Copyright (C) 2004 Texas Instruments, Inc. 
 *
 * Video-for-Linux (Version 2) graphic capture driver for
 * the OMAP H2 and H3 camera controller.
 *
 * Adapted from omap24xx driver written by Andy Lowe (source@mvista.com)
 * Copyright (C) 2003-2004 MontaVista Software, Inc.
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * History:
 *   27/03/05   Vladimir Barinov - Added support for power management
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include <mach/bsp.h>
#if defined(CONFIG_ARCH_TCC892X)
#include <mach/irqs.h>
#include <mach/vioc_ireq.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_config.h>
#include <mach/vioc_plugin_tcc892x.h>
#include <mach/vioc_scaler.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_api.h>
#endif // CONFIG_ARCH_TCC892X
#include <linux/poll.h>
#include "tcc_wmixer.h"
#include <mach/tcc_wmixer_ioctrl.h>


#define WMIXER_DEBUG 		0
#define dprintk(msg...) 	if(WMIXER_DEBUG) { printk("WMIXER: " msg); }

volatile PVIOC_RDMA 	pWMIX_rdma_base;
volatile PVIOC_WMIX 	pWMIX_wmix_base;
volatile PVIOC_WDMA 	pWMIX_wdma_base;

typedef struct _INTERRUPT_DATA_T {
	// wait for poll  
	wait_queue_head_t 	poll_wq;
	spinlock_t 			poll_lock;
	unsigned int 		poll_count;

	// wait for ioctl command
	wait_queue_head_t 	cmd_wq;
	spinlock_t 			cmd_lock;
	unsigned int 		cmd_count;

	struct mutex 		io_mutex;
	unsigned char 		block_operating;
	unsigned char 		block_waiting;
	unsigned char 		irq_reged;
	unsigned int  		dev_opened;
} INTERRUPT_DATA_T;
static INTERRUPT_DATA_T wmixer_data;

#define DEVICE_NAME			"wmixer"
#define MAJOR_ID			210
#define MINOR_ID			1

static struct clk *wmixer_clk;

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int tccxxx_wmixer_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0) {
		printk(KERN_ERR	"wmixer: this address is not allowed. \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff , vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}

	vma->vm_ops		=  NULL;
	vma->vm_flags 	|= VM_IO;
	vma->vm_flags 	|= VM_RESERVED;

	return 0;
}

char tccxxx_wmixer_ctrl(WMIXER_INFO_TYPE *wmix_info)
{
	int ret = 0;

	dprintk("%s \n", __func__);
	dprintk("Src  : addr:0x%x 0x%x 0x%x  fmt:%d \n", wmix_info->src_y_addr, wmix_info->src_u_addr, wmix_info->src_v_addr, wmix_info->src_fmt);
	dprintk("Dest: addr:0x%x 0x%x 0x%x  fmt:%d \n", wmix_info->dst_y_addr, wmix_info->dst_u_addr, wmix_info->dst_v_addr, wmix_info->dst_fmt);
	dprintk("Size : W:%d  H:%d \n", wmix_info->img_width, wmix_info->img_height);


	spin_lock_irq(&(wmixer_data.cmd_lock));

	// set to RDMA
	VIOC_RDMA_SetImageFormat(pWMIX_rdma_base, wmix_info->src_fmt);
	VIOC_RDMA_SetImageSize(pWMIX_rdma_base, wmix_info->img_width, wmix_info->img_height);
	VIOC_RDMA_SetImageOffset(pWMIX_rdma_base, wmix_info->src_fmt, wmix_info->img_width);
	VIOC_RDMA_SetImageBase(pWMIX_rdma_base, wmix_info->src_y_addr, wmix_info->src_u_addr,  wmix_info->src_v_addr);


	// set to WMIX
	VIOC_WMIX_SetSize(pWMIX_wmix_base, wmix_info->img_width, wmix_info->img_height);
	VIOC_WMIX_SetUpdate(pWMIX_wmix_base);
	VIOC_RDMA_SetImageEnable(pWMIX_rdma_base); // Soc guide info.

	// set to WRMA
	VIOC_WDMA_SetImageFormat(pWMIX_wdma_base, wmix_info->dst_fmt);
	VIOC_WDMA_SetImageSize(pWMIX_wdma_base, wmix_info->img_width, wmix_info->img_height);
	VIOC_WDMA_SetImageOffset(pWMIX_wdma_base, wmix_info->dst_fmt, wmix_info->img_width);

	VIOC_WDMA_SetImageBase(pWMIX_wdma_base, wmix_info->dst_y_addr, wmix_info->dst_u_addr, wmix_info->dst_v_addr);
	VIOC_WDMA_SetImageEnable(pWMIX_wdma_base, 0/*OFF*/);
	pWMIX_wdma_base->uIRQSTS.nREG = 0xFFFFFFFF; // wdma status register all clear.

	spin_unlock_irq(&(wmixer_data.cmd_lock));

	if(wmix_info->rsp_type == WMIXER_POLLING) {
		ret = wait_event_interruptible_timeout(wmixer_data.poll_wq, wmixer_data.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0) {
			 wmixer_data.block_operating = 0;
			printk("wmixer time out: %d, Line: %d. \n", ret, __LINE__);
		}
	}

	return ret;
}

static unsigned int tccxxx_wmixer_poll(struct file *filp, poll_table *wait)
{
	int ret = 0;
	INTERRUPT_DATA_T *wmix_data = (INTERRUPT_DATA_T *)filp->private_data;

	if(wmix_data == NULL) 	return -EFAULT;

	poll_wait(filp, &(wmix_data->poll_wq), wait);

	spin_lock_irq(&(wmix_data->poll_lock));

	if(wmix_data->block_operating == 0) 	ret = (POLLIN|POLLRDNORM);

	spin_unlock_irq(&(wmix_data->poll_lock));

	return ret;
}

static irqreturn_t tccxxx_wmixer_handler(int irq, void *client_data)
{  	
	INTERRUPT_DATA_T *wmix_data = client_data;
	
	if(pWMIX_wdma_base->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFR_MASK) {
		dprintk("WDMA Interrupt is VIOC_WDMA_IREQ_EOFR_MASK. \n");
		pWMIX_wdma_base->uIRQSTS.nREG = 0xFFFFFFFF;   // wdma status register all clear.
	}
		dprintk("WDMA Interrupt is VIOC_WDMA_IREQ_EOFR_MASK. \n");

	if(wmix_data->block_operating >= 1) 	
		wmix_data->block_operating = 0;
		
	wake_up_interruptible(&(wmix_data->poll_wq));

	if(wmix_data->block_waiting) 	
		wake_up_interruptible(&wmix_data->cmd_wq);

	return IRQ_HANDLED;
}

long tccxxx_wmixer_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	WMIXER_INFO_TYPE wmix_info;
	INTERRUPT_DATA_T *wmix_data = (INTERRUPT_DATA_T *)file->private_data;
	dprintk("wmixer:  cmd(0x%x), block_operating(0x%x), block_waiting(0x%x), cmd_count(0x%x), poll_count(0x%x). \n", 	\
					cmd, wmix_data->block_operating, wmix_data->block_waiting, wmix_data->cmd_count, wmix_data->poll_count);

	switch(cmd) {
		case TCC_WMIXER_IOCTRL:
			mutex_lock(&wmix_data->io_mutex);
			if(wmix_data->block_operating) {
				wmix_data->block_waiting = 1;
				ret = wait_event_interruptible_timeout(wmix_data->cmd_wq, wmix_data->block_operating == 0, msecs_to_jiffies(200));
				if(ret <= 0) {
					wmix_data->block_operating = 0;
					printk("[%d]: wmixer 0 timed_out block_operation:%d!! cmd_count:%d \n", ret, wmix_data->block_waiting, wmix_data->cmd_count);
				}
				ret = 0;
			}

			if(copy_from_user(&wmix_info, (WMIXER_INFO_TYPE *)arg, sizeof(WMIXER_INFO_TYPE))) {
				printk(KERN_ALERT "Not Supported copy_from_user(%d). \n", cmd);
				ret = -EFAULT;
			}
			
			if(ret >= 0) {
				if(wmix_data->block_operating >= 1) {
					printk("scaler + :: block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n", 	\
								wmix_data->block_operating, wmix_data->block_waiting, wmix_data->cmd_count, wmix_data->poll_count);
				}

				wmix_data->block_waiting = 0;
				wmix_data->block_operating = 1;
				ret = tccxxx_wmixer_ctrl(&wmix_info);
				if(ret < 0) 	wmix_data->block_operating = 0;
			}
			mutex_unlock(&wmix_data->io_mutex);
			return ret;

		default:
			printk(KERN_ALERT "not supported WMIXER IOCTL(0x%x). \n", cmd);
			break;			
	}

	return 0;
}
EXPORT_SYMBOL(tccxxx_wmixer_ioctl);

int tccxxx_wmixer_release(struct inode *inode, struct file *filp)
{
	int ret = 0;
	dprintk("wmixer_release IN:  %d'th, block(%d/%d), cmd(%d), irq(%d). \n", wmixer_data.dev_opened, 			\
			wmixer_data.block_operating, wmixer_data.block_waiting, wmixer_data.cmd_count, wmixer_data.irq_reged);

	if(wmixer_data.dev_opened > 0) 	wmixer_data.dev_opened--;
	if(wmixer_data.dev_opened == 0) {
		if(wmixer_data.block_operating) {
			ret = wait_event_interruptible_timeout(wmixer_data.cmd_wq, wmixer_data.block_operating == 0, msecs_to_jiffies(200));
		}

		if(ret <= 0) {
 			printk("[%d]: wmixer timed_out block_operation: %d, cmd_count: %d. \n", ret, wmixer_data.block_waiting, wmixer_data.cmd_count);
		}

		if(wmixer_data.irq_reged) {
			free_irq(INT_VIOC_WD4, &wmixer_data);
			wmixer_data.irq_reged = 0;
		}

		VIOC_WMIX_SetSWReset(VIOC_WMIX4, VIOC_WMIX_RDMA_14, VIOC_WMIX_WDMA_04);

		wmixer_data.block_operating = wmixer_data.block_waiting = 0;
		wmixer_data.poll_count = wmixer_data.cmd_count = 0;
	}

	clk_disable(wmixer_clk);

	dprintk("wmixer_release OUT:  %d'th. \n", wmixer_data.dev_opened);
	return 0;
}
EXPORT_SYMBOL(tccxxx_wmixer_release);

int tccxxx_wmixer_open(struct inode *inode, struct file *filp)
{	
	int ret = 0;
	dprintk("wmixer_open IN:  %d'th, block(%d/%d), cmd(%d), irq(%d). \n", wmixer_data.dev_opened, 				\
			wmixer_data.block_operating, wmixer_data.block_waiting, wmixer_data.cmd_count, wmixer_data.irq_reged);

	clk_enable(wmixer_clk);

	if(!wmixer_data.irq_reged) {
		// set to RDMA
		pWMIX_rdma_base = (volatile PVIOC_RDMA)tcc_p2v((unsigned int)HwVIOC_RDMA14);

		// set to WMIX
		pWMIX_wmix_base = (volatile PVIOC_WMIX)tcc_p2v((unsigned int)HwVIOC_WMIX4);

		// set to WDMA
		pWMIX_wdma_base = (volatile PVIOC_WDMA)tcc_p2v((unsigned int)HwVIOC_WDMA04);

		VIOC_WMIX_SetSWReset(VIOC_WMIX4, VIOC_WMIX_RDMA_14, VIOC_WMIX_WDMA_04);
		VIOC_WDMA_SetIreqMask(pWMIX_wdma_base, VIOC_WDMA_IREQ_EOFR_MASK, 0x0);

		ret = request_irq(INT_VIOC_WD4, tccxxx_wmixer_handler, IRQF_SHARED, "wmixer", &wmixer_data);
		
		if(ret) {
			clk_disable(wmixer_clk);
			printk("failed to aquire wmixer request_irq. \n");
			return -EFAULT;
		}

		wmixer_data.irq_reged = 1;
	}

	wmixer_data.dev_opened++;
	filp->private_data = &wmixer_data;
	
	dprintk("wmixer_open OUT:  %d'th. \n", wmixer_data.dev_opened);
	return ret;	
}
EXPORT_SYMBOL(tccxxx_wmixer_open);


static struct file_operations tcc_wmixer_fops = {
	.owner 				= THIS_MODULE,
	.unlocked_ioctl 	= tccxxx_wmixer_ioctl,
	.mmap 				= tccxxx_wmixer_mmap,
	.open 				= tccxxx_wmixer_open,
	.release 			= tccxxx_wmixer_release,
	.poll 				= tccxxx_wmixer_poll,
};

void __exit
tccxxx_wmixer_cleanup(void)
{
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
	clk_put(wmixer_clk);
	return;
}

static char banner[] __initdata = KERN_INFO "TCC WMIXER Driver Initializing. \n";
static struct class *wmixer_class;

int __init 
tccxxx_wmixer_init(void)
{
	printk(banner);
	
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_wmixer_fops);
	wmixer_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(wmixer_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);

	memset(&wmixer_data, 0, sizeof(INTERRUPT_DATA_T));

	spin_lock_init(&(wmixer_data.poll_lock));
	spin_lock_init(&(wmixer_data.cmd_lock));

	mutex_init(&(wmixer_data.io_mutex));
	
	init_waitqueue_head(&(wmixer_data.poll_wq));
	init_waitqueue_head(&(wmixer_data.cmd_wq));

	wmixer_clk = clk_get(NULL, "ddi");
	BUG_ON(wmixer_clk == NULL);

	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCC WMIXER Driver");
MODULE_LICENSE("GPL");


arch_initcall(tccxxx_wmixer_init);
module_exit(tccxxx_wmixer_cleanup);


