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

#include <mach/irqs.h>
#include <mach/vioc_ireq.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_config.h>
#include <mach/vioc_plugin_tcc892x.h>
#include <mach/vioc_scaler.h>
#include <mach/vioc_api.h>


#include <linux/poll.h>


#include "tcc_scaler.h"
#include <mach/tcc_scaler_ctrl.h>
#include <mach/tccfb_address.h>

#if 0
static int debug	   = 1;
#else
static int debug	   = 0;
#endif


volatile PVIOC_SC 		pM2MSCALER0;
volatile PVIOC_RDMA 	SC2_pRDMABase;
volatile PVIOC_WMIX 	SC2_pWIXBase;
volatile PVIOC_WDMA 	SC2_pWDMABase;

#define dprintk(msg...) 	if(debug) { printk("tcc_scaler2: " msg); }

typedef struct _intr_data_t {
	//wait for Poll!!  
	wait_queue_head_t poll_wq;
	spinlock_t poll_lock;
	unsigned int poll_count;

	//wait for Ioctl command!!
	wait_queue_head_t cmd_wq;
	spinlock_t cmd_lock;
	unsigned int cmd_count;

	struct mutex io_mutex;
	unsigned char block_operating;
	unsigned char block_waiting;
	unsigned char irq_reged;
	unsigned int  dev_opened;
} intr_data_t;
static intr_data_t SC2_data;

#define DEVICE_NAME			"scaler2"
#define MAJOR_ID			202
#define MINOR_ID			1

static struct clk *scaler2_clk;


extern unsigned int scaler_ended;

extern void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
									unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U,unsigned int* V);

char M2M_Scaler2_Ctrl_Detail(SCALER_TYPE *scale_img)
{
	int ret = 0;
	unsigned int pSrcBase0 = 0, pSrcBase1 = 0, pSrcBase2 = 0;
	unsigned int pDstBase0 = 0, pDstBase1 = 0, pDstBase2 = 0;
	unsigned int crop_width;

	VIOC_SCALER_INFO_Type pScalerInfo;


	pSrcBase0 = (unsigned int)scale_img->src_Yaddr;
	pSrcBase1 = (unsigned int)scale_img->src_Uaddr;
	pSrcBase2 = (unsigned int)scale_img->src_Vaddr;

	// address limitation!!
	crop_width 				= scale_img->src_winRight - scale_img->src_winLeft;
	scale_img->src_winLeft 	= (scale_img->src_winLeft>>3)<<3; 
	scale_img->src_winRight = scale_img->src_winLeft + crop_width;
	tccxxx_GetAddress(scale_img->src_fmt, (unsigned int)scale_img->src_Yaddr,
								scale_img->src_ImgWidth, scale_img->src_ImgHeight,
								scale_img->src_winLeft, scale_img->src_winTop,
								&pSrcBase0, &pSrcBase1, &pSrcBase2);

	pDstBase0 = (unsigned int)scale_img->dest_Yaddr;
	pDstBase1 = (unsigned int)scale_img->dest_Uaddr;
	pDstBase2 = (unsigned int)scale_img->dest_Vaddr;

	// address limitation!!
	crop_width 				 = scale_img->dest_winRight - scale_img->dest_winLeft;
	scale_img->dest_winLeft  = (scale_img->dest_winLeft>>3)<<3; 
	scale_img->dest_winRight = scale_img->dest_winLeft + crop_width;
	tccxxx_GetAddress(scale_img->dest_fmt, (unsigned int)scale_img->dest_Yaddr, 
								scale_img->dest_ImgWidth, scale_img->dest_ImgHeight, 
								scale_img->dest_winLeft, scale_img->dest_winTop,
								&pDstBase0, &pDstBase1, &pDstBase2);

	spin_lock_irq(&(SC2_data.cmd_lock));


	// set to VRDMA
	if((scale_img->src_fmt == SC_IMG_FMT_ARGB6666) || (scale_img->src_fmt == SC_IMG_FMT_ARGB8888))	{
		VIOC_RDMA_SetImageAlphaSelect(SC2_pRDMABase, 1);
		VIOC_RDMA_SetImageAlphaEnable(SC2_pRDMABase, 1);
	}
	else	{
		VIOC_RDMA_SetImageAlphaSelect(SC2_pRDMABase, 0);
		VIOC_RDMA_SetImageAlphaEnable(SC2_pRDMABase, 0);
	}
	

	VIOC_RDMA_SetImageFormat(SC2_pRDMABase, scale_img->src_fmt);
	VIOC_RDMA_SetImageSize(SC2_pRDMABase,scale_img->src_winRight- scale_img->src_winLeft, scale_img->src_winBottom - scale_img->src_winTop);
	VIOC_RDMA_SetImageOffset(SC2_pRDMABase, scale_img->src_fmt, scale_img->src_ImgWidth);
	VIOC_RDMA_SetImageBase(SC2_pRDMABase, pSrcBase0, pSrcBase1, pSrcBase2);
	//VIOC_RDMA_SetImageEnable(SC2_pRDMABase);

	// set to VIOC Scaler2
	pScalerInfo.BYPASS 			= FALSE /* 0 */;
	pScalerInfo.DST_WIDTH 		= (scale_img->dest_winRight - scale_img->dest_winLeft);
	pScalerInfo.DST_HEIGHT 		= (scale_img->dest_winBottom - scale_img->dest_winTop);
	pScalerInfo.OUTPUT_POS_X 	= 0;
	pScalerInfo.OUTPUT_POS_Y 	= 0;
	pScalerInfo.OUTPUT_WIDTH 	= pScalerInfo.DST_WIDTH;
	pScalerInfo.OUTPUT_HEIGHT 	= pScalerInfo.DST_HEIGHT;

	VIOC_API_SCALER_SetConfig(VIOC_SC0, &pScalerInfo);
	VIOC_API_SCALER_SetPlugIn(VIOC_SC0, VIOC_SC_RDMA_17);
	VIOC_API_SCALER_SetUpdate(VIOC_SC0);
	VIOC_RDMA_SetImageEnable(SC2_pRDMABase); // SoC guide info.

	// set to WMIX30  
	VIOC_CONFIG_WMIXPath(WMIX60, 1 /* mixer  */);
	VIOC_WMIX_SetSize(SC2_pWIXBase, pScalerInfo.DST_WIDTH, pScalerInfo.DST_HEIGHT);	
	VIOC_WMIX_SetUpdate(SC2_pWIXBase);

	// set to VWRMA
	VIOC_WDMA_SetImageFormat(SC2_pWDMABase, scale_img->dest_fmt);
	VIOC_WDMA_SetImageSize(SC2_pWDMABase, pScalerInfo.DST_WIDTH, pScalerInfo.DST_HEIGHT);
	VIOC_WDMA_SetImageOffset(SC2_pWDMABase, scale_img->dest_fmt, scale_img->dest_ImgWidth);
	VIOC_WDMA_SetImageBase(SC2_pWDMABase, pDstBase0, pDstBase1, pDstBase2);
	VIOC_WDMA_SetImageEnable(SC2_pWDMABase, 0 /* OFF */);	
	SC2_pWDMABase->uIRQSTS.nREG = 0xFFFFFFFF; // wdma status register all clear.

	spin_unlock_irq(&(SC2_data.cmd_lock));

	if(scale_img->responsetype  == SCALER_POLLING)	{
		ret = wait_event_interruptible_timeout(SC2_data.poll_wq,  SC2_data.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0) {
			 SC2_data.block_operating = 0;
			printk("Scaler  time out :%d Line :%d \n", __LINE__, ret);
		}		
	}
	else if(scale_img->responsetype  == SCALER_NOWAIT)	{
		if(scale_img->viqe_onthefly & 0x2)
			 SC2_data.block_operating = 0;
	}

	return ret;
}

char M2M_Scaler2_Ctrl_Divide(SCALER_TYPE *scale_img)
{
	int ret = 0;
	unsigned int pSrcBaseY = 0, pSrcBaseU = 0, pSrcBaseV = 0;
	unsigned int pDstBase0 = 0, pDstBase1 = 0, pDstBaseU = 0, pDstBaseV = 0;
	unsigned int img_wd, img_ht;

	VIOC_SCALER_INFO_Type pScalerInfo;
	PVIOC_WDMA pWDMABase1 = (PVIOC_WDMA)tcc_p2v(HwVIOC_WDMA08);
	PVIOC_WDMA pWDMABase0 = (PVIOC_WDMA)SC2_pWDMABase;

	pSrcBaseY = (unsigned int)scale_img->src_Yaddr;
	pSrcBaseU = (unsigned int)scale_img->src_Uaddr;
	pSrcBaseV = (unsigned int)scale_img->src_Vaddr;

	pDstBase0 = (unsigned int)scale_img->dest_Yaddr;
	pDstBase1 = (unsigned int)scale_img->dest_Yaddr;
	pDstBaseU = (unsigned int)scale_img->dest_Uaddr;
	pDstBaseV = (unsigned int)scale_img->dest_Vaddr;

	spin_lock_irq(&(SC2_data.cmd_lock));

	// set to VRDMA switch path
	//VIOC_CONFIG_RDMA12PathCtrl(0 /* RDMA12 */);

	// set to VRDMA
	VIOC_RDMA_SetImageAlphaSelect(SC2_pRDMABase, 1);
	VIOC_RDMA_SetImageAlphaEnable(SC2_pRDMABase, 1);
	VIOC_RDMA_SetImageFormat(SC2_pRDMABase, scale_img->src_fmt);
	VIOC_RDMA_SetImageSize(SC2_pRDMABase, scale_img->src_ImgWidth, scale_img->src_ImgHeight);
	VIOC_RDMA_SetImageOffset(SC2_pRDMABase, scale_img->src_fmt, scale_img->src_ImgWidth);
	VIOC_RDMA_SetImageBase(SC2_pRDMABase, pSrcBaseY, pSrcBaseU, pSrcBaseV);
	//VIOC_RDMA_SetImageEnable(SC2_pRDMABase);

	img_wd = (scale_img->dest_winRight - scale_img->dest_winLeft);
	img_ht = (scale_img->dest_winBottom - scale_img->dest_winTop);

	// set to VIOC Scaler2
	pScalerInfo.BYPASS 			= FALSE /* 0 */;
	pScalerInfo.SRC_WIDTH 		= scale_img->src_ImgWidth;
	pScalerInfo.SRC_HEIGHT 		= scale_img->src_ImgHeight;
	pScalerInfo.DST_WIDTH 		= img_wd;
	pScalerInfo.DST_HEIGHT 		= img_ht;
	pScalerInfo.OUTPUT_POS_X 	= 0;
	pScalerInfo.OUTPUT_POS_Y 	= 0;
	pScalerInfo.OUTPUT_WIDTH 	= img_wd;
	pScalerInfo.OUTPUT_HEIGHT 	= img_ht;

	VIOC_API_SCALER_SetConfig(VIOC_SC0, &pScalerInfo);
	VIOC_API_SCALER_SetPlugIn(VIOC_SC0, VIOC_SC_RDMA_17);
	VIOC_API_SCALER_SetUpdate(VIOC_SC0);
	VIOC_RDMA_SetImageEnable(SC2_pRDMABase); // SoC guide info.

	/* Side-By-Side */
	if(scale_img->divide_path == 0x01)
	{
		img_wd = scale_img->dest_ImgWidth/2;
		img_ht = scale_img->dest_ImgHeight;

		pDstBase0 = scale_img->dest_Yaddr;
		pDstBase1 = pDstBase0 + img_wd*4;
	}
	/* Top-N-Bottom */
	else if(scale_img->divide_path == 0x02)
	{
		img_wd = scale_img->dest_ImgWidth;
		img_ht = scale_img->dest_ImgHeight/2;

		pDstBase0 = scale_img->dest_Yaddr;
		pDstBase1 = pDstBase0 + img_wd*img_ht*4;
	}

	VIOC_CONFIG_WMIXPath(WMIX60, 1 /* mixing */);
	VIOC_WMIX_SetSize(SC2_pWIXBase, img_wd, img_ht);
	VIOC_WMIX_SetPosition(SC2_pWIXBase, 0, scale_img->dest_winLeft, scale_img->dest_winTop);
	VIOC_WMIX_SetPosition(SC2_pWIXBase, 1, scale_img->dest_winLeft, scale_img->dest_winTop);
	VIOC_WMIX_SetUpdate(SC2_pWIXBase);

	// set to VWRMA
 	VIOC_WDMA_SetImageFormat(pWDMABase0, scale_img->dest_fmt);
	VIOC_WDMA_SetImageSize(pWDMABase0, img_wd, img_ht);
	VIOC_WDMA_SetImageOffset(pWDMABase0, scale_img->dest_fmt, scale_img->dest_ImgWidth);
	VIOC_WDMA_SetImageBase(pWDMABase0, pDstBase0, pDstBaseU, pDstBaseV);
	VIOC_WDMA_SetImageEnable(pWDMABase0, 0 /* OFF */);
	pWDMABase0->uIRQSTS.nREG = 0xFFFFFFFF; // wdma status register all clear.

	VIOC_WDMA_SetImageFormat(pWDMABase1, scale_img->dest_fmt);
	VIOC_WDMA_SetImageSize(pWDMABase1, img_wd, img_ht);
	VIOC_WDMA_SetImageOffset(pWDMABase1, scale_img->dest_fmt, scale_img->dest_ImgWidth);
	VIOC_WDMA_SetImageBase(pWDMABase1, pDstBase1, pDstBaseU, pDstBaseV);
	VIOC_WDMA_SetImageEnable(pWDMABase1, 0 /* OFF */);
	pWDMABase1->uIRQSTS.nREG = 0xFFFFFFFF; // wdma status register all clear.

	spin_unlock_irq(&(SC2_data.cmd_lock));

	if(scale_img->responsetype  == SCALER_POLLING)	{
		ret = wait_event_interruptible_timeout(SC2_data.poll_wq,  SC2_data.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0) {
			 SC2_data.block_operating = 0;
			printk("Scaler  time out :%d Line :%d \n", __LINE__, ret);
		}		
	}
	else if(scale_img->responsetype  == SCALER_NOWAIT)	{
		if(scale_img->viqe_onthefly & 0x2)
			 SC2_data.block_operating = 0;
	}

	return ret;
}

static unsigned int tccxxx_scaler2_poll(struct file *filp, poll_table *wait)
{
	int ret = 0;
	intr_data_t *msc_data = (intr_data_t *)filp->private_data;

	if (msc_data == NULL)
		return -EFAULT;
	
	poll_wait(filp, &(msc_data->poll_wq), wait);

	spin_lock_irq(&(msc_data->poll_lock));

	if (msc_data->block_operating == 0) 	{
		ret =  (POLLIN | POLLRDNORM);
	}

	spin_unlock_irq(&(msc_data->poll_lock));

	return ret;
}

// interrupt handler
static irqreturn_t tccxxx_scaler2_handler(int irq, void *client_data/*, struct pt_regs *reg*/)
{  	
	unsigned long IFlag;
	intr_data_t *msc_data = (intr_data_t *)client_data;
	
	if(SC2_pWDMABase->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFR_MASK) {
		dprintk("WDMA Interrupt is VIOC_WDMA_IREQ_EOFR_MASK. \n");
		SC2_pWDMABase->uIRQSTS.nREG = 0xFFFFFFFF;   // wdma status register all clear.
	}

	if(msc_data->block_operating >= 1) {
		msc_data->block_operating = 0;
	}
		
	wake_up_interruptible(&(msc_data->poll_wq));

	if(msc_data->block_waiting)
		wake_up_interruptible(&msc_data->cmd_wq);

	return IRQ_HANDLED;
}


extern void tccxxx_convert_image_format(SCALER_TYPE *pScalerInfo);
long tccxxx_scaler2_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	SCALER_TYPE scaler_v;
	intr_data_t *msc_data = (intr_data_t *)file->private_data;

	dprintk("scaler2  ::cmd:0x%x block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n", 	\
					cmd, msc_data->block_operating, msc_data->block_waiting, msc_data->cmd_count, msc_data->poll_count);

	switch(cmd) {
		case TCC_SCALER_IOCTRL:
		case TCC_SCALER_IOCTRL_KERENL:
			mutex_lock(&msc_data->io_mutex);
			if(msc_data->block_operating) {
				msc_data->block_waiting = 1;
				ret = wait_event_interruptible_timeout(msc_data->cmd_wq, msc_data->block_operating == 0, msecs_to_jiffies(200));
				if(ret <= 0) {
					msc_data->block_operating = 0;
					printk("[%d]: scaler 2 timed_out block_operation:%d!! cmd_count:%d \n", ret, msc_data->block_waiting, msc_data->cmd_count);
				}
				ret = 0;
			}

			if(cmd == TCC_SCALER_IOCTRL_KERENL)	{
				memcpy(&scaler_v, (SCALER_TYPE *)arg, sizeof(scaler_v));
			} else {
				if(copy_from_user(&scaler_v, (SCALER_TYPE *)arg, sizeof(scaler_v))) {
					printk(KERN_ALERT "Not Supported copy_from_user(%d). \n", cmd);
					ret = -EFAULT;
				}
			}
			
			if(ret >= 0) {
				if(msc_data->block_operating >= 1) {
					printk("scaler + :: block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n", 	\
								msc_data->block_operating, msc_data->block_waiting, msc_data->cmd_count, msc_data->poll_count);
				}

				tccxxx_convert_image_format(&scaler_v);

				msc_data->block_waiting = 0;
				msc_data->block_operating = 1;

				if(scaler_v.divide_path)
					ret = M2M_Scaler2_Ctrl_Divide(&scaler_v);
				else
					ret = M2M_Scaler2_Ctrl_Detail(&scaler_v);

				if(ret < 0) 	
					msc_data->block_operating = 0;
			}
			mutex_unlock(&msc_data->io_mutex);
			return ret;


		default:
			printk(KERN_ALERT "Not Supported SCALER_IOCTL(%d)\n", cmd);
			break;			
	}

	return 0;
}
EXPORT_SYMBOL(tccxxx_scaler2_ioctl);

int tccxxx_scaler2_release(struct inode *inode, struct file *filp)
{
	int ret = 0;
	dprintk("scaler2_release In!! %d'th, block(%d/%d), cmd(%d), irq(%d)  \n", SC2_data.dev_opened, SC2_data.block_operating, \
																SC2_data.block_waiting, SC2_data.cmd_count, SC2_data.irq_reged);

	if(SC2_data.dev_opened > 0) 	
		SC2_data.dev_opened--;

	if(SC2_data.dev_opened == 0) {
		if(SC2_data.block_operating) {
			ret = wait_event_interruptible_timeout(SC2_data.cmd_wq, SC2_data.block_operating == 0, msecs_to_jiffies(200));

			if(ret <= 0) {
	 			printk("[%d]: scaler2 timed_out block_operation:%d!! cmd_count:%d \n", ret, SC2_data.block_waiting, SC2_data.cmd_count);
			}
		}

		if(SC2_data.irq_reged) {
			free_irq(INT_VIOC_WD7, &SC2_data);
			SC2_data.irq_reged = 0;
		}
		
		VIOC_CONFIG_PlugOut(VIOC_SC0);
		VIOC_SC_SetSWReset(VIOC_SC0, 17/*VIOC_SC_RDMA_17*/, 7/*VIOC_SC_WDMA_07*/);

		SC2_data.block_operating = SC2_data.block_waiting = 0;
		SC2_data.poll_count = SC2_data.cmd_count = 0;
	}

	clk_disable(scaler2_clk);
	dprintk("scaler2_release Out!! %d'th \n", SC2_data.dev_opened);
	return 0;
}
EXPORT_SYMBOL(tccxxx_scaler2_release);

int tccxxx_scaler2_open(struct inode *inode, struct file *filp)
{	
	int ret = 0;
	dprintk("scaler2_open In!! %d'th, block(%d/%d), cmd(%d), irq(%d) \n", SC2_data.dev_opened, SC2_data.block_operating, \
															SC2_data.block_waiting, SC2_data.cmd_count, SC2_data.irq_reged);

	clk_enable(scaler2_clk);

	if(!SC2_data.irq_reged) {
		// set to VRDMA 17
		SC2_pRDMABase = (volatile PVIOC_RDMA)tcc_p2v((unsigned int)HwVIOC_RDMA17);

		// set to WMIX6
		SC2_pWIXBase = (volatile PVIOC_WMIX)tcc_p2v((unsigned int)HwVIOC_WMIX6);

		// set to VWDMA
		SC2_pWDMABase = (volatile PVIOC_WDMA)tcc_p2v((unsigned int)HwVIOC_WDMA07);

		VIOC_SC_SetSWReset(VIOC_SC0, 17/*VIOC_SC_RDMA_17*/, 7/*VIOC_SC_WDMA_07*/);

		VIOC_WDMA_SetIreqMask(SC2_pWDMABase, VIOC_WDMA_IREQ_EOFR_MASK, 0x0);
		ret = request_irq(INT_VIOC_WD7, tccxxx_scaler2_handler, IRQF_SHARED, "scaler2", &SC2_data);
		
		if(ret) {
			clk_disable(scaler2_clk);
			printk("FAILED to aquire scaler2-irq. \n");
			return -EFAULT;
		}

		SC2_data.irq_reged = 1;
	}

	SC2_data.dev_opened++;
	filp->private_data = &SC2_data;
	
	dprintk("scaler2_open Out!! %d'th \n", SC2_data.dev_opened);
	return ret;	
}
EXPORT_SYMBOL(tccxxx_scaler2_open);


static struct file_operations tcc_scaler2_fops = 
{
	.owner 			= THIS_MODULE,
	.unlocked_ioctl 	= tccxxx_scaler2_ioctl,
	.open 			= tccxxx_scaler2_open,
	.release 			= tccxxx_scaler2_release,
	.poll 			= tccxxx_scaler2_poll,
};

void __exit
tccxxx_scaler2_cleanup(void)
{
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
	clk_put(scaler2_clk);
	return;
}

static char banner[] __initdata = KERN_INFO "TCC Scaler-2 Driver Initializing. \n";
static struct class *scaler2_class;

int __init 
tccxxx_scaler2_init(void)
{
	printk(banner);
	
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_scaler2_fops);
	scaler2_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(scaler2_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);

	memset(&SC2_data, 0, sizeof(intr_data_t));

	spin_lock_init(&(SC2_data.poll_lock));
	spin_lock_init(&(SC2_data.cmd_lock));

	mutex_init(&(SC2_data.io_mutex));
	
	init_waitqueue_head(&(SC2_data.poll_wq));
	init_waitqueue_head(&(SC2_data.cmd_wq));

	scaler2_clk = clk_get(NULL, "ddi");
	BUG_ON(scaler2_clk == NULL);

	pM2MSCALER0 = (volatile PVIOC_SC)tcc_p2v((unsigned int)HwVIOC_SC0);
	
	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCC Scaler-2 Driver");
MODULE_LICENSE("GPL");


arch_initcall(tccxxx_scaler2_init);
module_exit(tccxxx_scaler2_cleanup);


