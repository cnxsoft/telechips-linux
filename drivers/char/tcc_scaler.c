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
#include <mach/vioc_api.h>
#include <mach/tccfb_address.h>
#endif // CONFIG_ARCH_TCC892X

#include <linux/poll.h>


#include "tcc_scaler.h"
#include <mach/tcc_scaler_ctrl.h>

#if 0
static int debug	   = 1;
#else
static int debug	   = 0;
#endif

#if defined(CONFIG_ARCH_TCC892X)
volatile PVIOC_SC 		pM2MSCALER;
volatile PVIOC_RDMA 	pRDMABase;
volatile PVIOC_RDMA 	pRDMA1Base;
volatile PVIOC_WMIX 	pWIXBase;
volatile PVIOC_WDMA 	pWDMABase;
#else // CONFIG_ARCH_TCC892X
volatile PM2MSCALER pM2MSCALER;
#endif // CONFIG_ARCH_TCC892X

#define dprintk(msg...)	if (debug) { printk( "tcc_scaler: " msg); }

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
static intr_data_t sc_data;

#define DEVICE_NAME			"scaler"
#define MAJOR_ID			200
#define MINOR_ID			1

static struct clk *m2m1_clk;
#if !defined(CONFIG_ARCH_TCC892X)
static struct clk *m2m0_ddi_cache;
#endif // CONFIG_ARCH_TCC892X


#if defined(CONFIG_ARCH_TCC892X)
extern void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
									unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U,unsigned int* V);
#endif

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int tccxxx_scaler_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR	 "scaler: this address is not allowed \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff , vma->vm_end - vma->vm_start, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	vma->vm_ops		= NULL;
	vma->vm_flags 	|= VM_IO;
	vma->vm_flags 	|= VM_RESERVED;
	
	return 0;
}

extern unsigned int scaler_ended;
static unsigned int check_status_intr;
char M2M_Scaler_Ctrl_Detail(SCALER_TYPE *scale_img)
{
	int ret = 0;
	unsigned int pSrcBase0 = 0, pSrcBase1 = 0, pSrcBase2 = 0;
	unsigned int pDstBase0 = 0, pDstBase1 = 0, pDstBase2 = 0;
	unsigned int SrcHsize, SrcVsize;
	unsigned int DstHsize, DstVsize;
	unsigned int SrcImgType, DstImgType;
	unsigned int ScaleRatV, ScaleRatH;
	unsigned int SrcOff_Hsize, DstOff_Hsize;
	int src_bpp, dst_bpp;
	unsigned char interleaved_mode = 0; //0:Cr/Cb, 1:Cb/Cr
	unsigned char interleaved_src = 0;
	unsigned char interleaved_dst = 0;
	unsigned int crop_width;


	#if defined(CONFIG_ARCH_TCC892X)
	VIOC_SCALER_INFO_Type pScalerInfo;

	dprintk("TCC892X VIOC Scaler: M2M_Scaler_Ctrl_Detail(). \n");

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

	spin_lock_irq(&(sc_data.cmd_lock));

	// set to VRDMA switch path
	VIOC_CONFIG_RDMA12PathCtrl(0 /* RDMA12 */);

	// set to VRDMA
	#if defined(CONFIG_MACH_TCC8920ST)
		VIOC_RDMA_SetImageAlphaSelect(pRDMABase, 1);
		VIOC_RDMA_SetImageAlphaEnable(pRDMABase, 1);
	#else
		VIOC_RDMA_SetImageAlphaEnable(pRDMABase, 1);
	#endif

	VIOC_RDMA_SetImageFormat(pRDMABase, scale_img->src_fmt);
	VIOC_RDMA_SetImageSize(pRDMABase,scale_img->src_winRight- scale_img->src_winLeft, scale_img->src_winBottom - scale_img->src_winTop);
	VIOC_RDMA_SetImageOffset(pRDMABase, scale_img->src_fmt, scale_img->src_ImgWidth);
	VIOC_RDMA_SetImageBase(pRDMABase, pSrcBase0, pSrcBase1, pSrcBase2);

	// set to VIOC Scaler2
	pScalerInfo.BYPASS 			= FALSE /* 0 */;
	pScalerInfo.DST_WIDTH 		= (scale_img->dest_winRight - scale_img->dest_winLeft);
	pScalerInfo.DST_HEIGHT 		= (scale_img->dest_winBottom - scale_img->dest_winTop);
	pScalerInfo.OUTPUT_POS_X 	= 0;
	pScalerInfo.OUTPUT_POS_Y 	= 0;
	pScalerInfo.OUTPUT_WIDTH 	= pScalerInfo.DST_WIDTH;
	pScalerInfo.OUTPUT_HEIGHT 	= pScalerInfo.DST_HEIGHT;
	
	VIOC_API_SCALER_SetConfig(VIOC_SC1, &pScalerInfo);
	VIOC_API_SCALER_SetPlugIn(VIOC_SC1, VIOC_SC_RDMA_12);
	VIOC_API_SCALER_SetUpdate(VIOC_SC1);
	VIOC_RDMA_SetImageEnable(pRDMABase); // SoC guide info.

	// set to WMIX30  
	VIOC_CONFIG_WMIXPath(WMIX30, 1 /* mixer-pass */);
	VIOC_WMIX_SetSize(pWIXBase, pScalerInfo.DST_WIDTH, pScalerInfo.DST_HEIGHT);	
	VIOC_WMIX_SetUpdate(pWIXBase);

	// set to VWRMA
	VIOC_WDMA_SetImageFormat(pWDMABase, scale_img->dest_fmt);
	VIOC_WDMA_SetImageSize(pWDMABase, pScalerInfo.DST_WIDTH, pScalerInfo.DST_HEIGHT);
	VIOC_WDMA_SetImageOffset(pWDMABase, scale_img->dest_fmt, scale_img->dest_ImgWidth);
	VIOC_WDMA_SetImageBase(pWDMABase, pDstBase0, pDstBase1, pDstBase2);
	VIOC_WDMA_SetImageEnable(pWDMABase, 0 /* OFF */);
	pWDMABase->uIRQSTS.nREG = 0xFFFFFFFF; // wdma status register all clear.

	spin_unlock_irq(&(sc_data.cmd_lock));
	
	#else // CONFIG_ARCH_TCC892X

	if((scale_img->src_ImgWidth == 0) || (scale_img->src_ImgHeight == 0) ||
		(scale_img->dest_ImgWidth == 0) || (scale_img->dest_ImgHeight == 0))
	{
		printk("Operation size is abnormal. (%d x %d) -> (%d x %d) \n", scale_img->src_ImgWidth, scale_img->src_ImgHeight, scale_img->dest_ImgWidth, scale_img->dest_ImgHeight);
		return -1;
	}
	
	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(!(scale_img->viqe_onthefly & 0x02) && !(scale_img->viqe_onthefly & 0x01))
			M2M_Scaler_SW_Reset(M2M_SCALER0);
	#else
		//실제 사용할때 reset 하도록 수정  ..pjj
		if(!(scale_img->viqe_onthefly & 0x02))
			M2M_Scaler_SW_Reset(M2M_SCALER0);
	#endif

	dprintk(" M2M_Scaler_Ctrl_Detail response[%d] \n", scale_img->responsetype);

	if(scale_img->src_fmt == SCALER_YUV422_inter || scale_img->src_fmt == SCALER_YUV420_inter)
		interleaved_src = 1;

	if(scale_img->dest_fmt == SCALER_YUV422_inter || scale_img->dest_fmt == SCALER_YUV420_inter)
		interleaved_dst = 1;
	
	if(!scaler_image_converter(scale_img->src_fmt, &SrcImgType, &src_bpp))
	{
		dprintk("source format error \n");
		return -1;
	}
	if(!scaler_image_converter(scale_img->dest_fmt, &DstImgType, &dst_bpp))
	{
		 dprintk("destination format error \n");		
		 return -1;
	}

	pSrcBase0 = (unsigned int)scale_img->src_Yaddr;
	pSrcBase1 = (unsigned int)scale_img->src_Uaddr;
	pSrcBase2 = (unsigned int)scale_img->src_Vaddr;

	// address limitation!!
	{
		crop_width	 = scale_img->src_winRight - scale_img->src_winLeft;
		scale_img->src_winLeft	= (scale_img->src_winLeft>>3)<<3; 
		scale_img->src_winRight = scale_img->src_winLeft + crop_width;
	}
	scale_get_address(scale_img->src_fmt, (unsigned int)scale_img->src_Yaddr, 
						scale_img->src_ImgWidth, scale_img->src_ImgHeight, 
						scale_img->src_winLeft, scale_img->src_winTop,
						&pSrcBase0, &pSrcBase1, &pSrcBase2);
	
	
	pDstBase0 = (unsigned int)scale_img->dest_Yaddr;
	pDstBase1 = (unsigned int)scale_img->dest_Uaddr;
	pDstBase2 = (unsigned int)scale_img->dest_Vaddr;

	// address limitation!!
	{
		crop_width	 = scale_img->dest_winRight - scale_img->dest_winLeft;
		scale_img->dest_winLeft	= (scale_img->dest_winLeft>>3)<<3; 
		scale_img->dest_winRight = scale_img->dest_winLeft + crop_width;
	}
	
	scale_get_address(scale_img->dest_fmt, (unsigned int)scale_img->dest_Yaddr, 
						scale_img->dest_ImgWidth, scale_img->dest_ImgHeight, 
						scale_img->dest_winLeft, scale_img->dest_winTop,
						&pDstBase0, &pDstBase1, &pDstBase2);

	// m2m scaler setting
	SrcOff_Hsize = scale_img->src_ImgWidth;
	DstOff_Hsize = scale_img->dest_ImgWidth;
	SrcHsize = scale_img->src_winRight - scale_img->src_winLeft;
	SrcVsize = scale_img->src_winBottom - scale_img->src_winTop;
	DstHsize = scale_img->dest_winRight - scale_img->dest_winLeft;
	DstVsize = scale_img->dest_winBottom - scale_img->dest_winTop;
	
	ScaleRatH = (SrcHsize * 256)/DstHsize;
	ScaleRatV = (SrcVsize * 256)/DstVsize;

	dprintk("src fmt[%x] width[%d] height[%d] \n", SrcImgType, SrcHsize, SrcVsize);
	dprintk("src Address [0x%x] [0x%x] [0x%x] \n", pSrcBase0, pSrcBase1, pSrcBase2);
	
	dprintk("dest fmt[%x] width[%d] height[%d] \n", DstImgType, DstHsize, DstVsize);
	dprintk("dest Address [0x%x] [0x%x] [0x%x] \n", pDstBase0, pDstBase1, pDstBase2);
					
	spin_lock_irq(&(sc_data.cmd_lock));

	MEM_SCALER_SRC_ADDR(pSrcBase0, pSrcBase1, pSrcBase2);
	MEM_SCALER_SRC_SIZE(SrcHsize, SrcVsize);
	MEM_SCALER_SRC_CONFIG(MSC_CFG_ALL, interleaved_mode, interleaved_src, SrcImgType);

	if ((SrcImgType == HwMSC_SRC_CFG_YUV422_SEP) || (SrcImgType == HwMSC_SRC_CFG_YUV420_SEP))
	{
		if(interleaved_src)
			MEM_SCALER_SRC_OFFSET_YC(SrcOff_Hsize, SrcOff_Hsize);
		else
			MEM_SCALER_SRC_OFFSET_YC(SrcOff_Hsize, (SrcOff_Hsize/2));
	}
	else
	{
		MEM_SCALER_SRC_OFFSET_YC((SrcOff_Hsize*2), 0);
	}

	//interlaced frame process ex) MPEG2
	if(scale_img->interlaced)
	{
		unsigned int validSrcVSize = (( SrcVsize/2) >>4) <<4 ;
		
		MEM_SCALER_SRC_SIZE(SrcHsize,validSrcVSize);
		
		if(interleaved_src)
			MEM_SCALER_SRC_OFFSET_YC((unsigned int)SrcOff_Hsize*2, SrcOff_Hsize*2);
		else
			MEM_SCALER_SRC_OFFSET_YC((unsigned int)SrcOff_Hsize*2, SrcOff_Hsize);
		
		ScaleRatV = (validSrcVSize * 256)/DstVsize;
	}
	
	if(DstImgType >= HwMSC_SRC_CFG_RGB565)
		pDstBase1 = pDstBase2 = 0;
	
	MEM_SCALER_DST_ADDR(pDstBase0, pDstBase1, pDstBase2);
	MEM_SCALER_RATIO(ScaleRatH, ScaleRatV);
	MEM_SCALER_DST_SIZE(DstHsize, DstVsize);

	if ((DstImgType == HwMSC_SRC_CFG_YUV422_SEP) || (DstImgType == HwMSC_SRC_CFG_YUV420_SEP))
	{
		if(interleaved_dst)
			MEM_SCALER_DST_OFFSET_YC(DstOff_Hsize, DstOff_Hsize);
		else
		MEM_SCALER_DST_OFFSET_YC(DstOff_Hsize, (DstOff_Hsize/2));
		MEM_SCALER_DST_CONFIG(MSC_CFG_ALL, 0, 0, 0, interleaved_mode, 0, interleaved_dst, DstImgType);
	}
	else
	{
		MEM_SCALER_DST_OFFSET_YC((DstOff_Hsize*2), 0);

		if(scale_img->viqe_onthefly & 0x2)
		{
			unsigned int WAIT_CYCLE = 1;
			if (ScaleRatH > 128)      
				WAIT_CYCLE = 1;
			else if (ScaleRatH > 64)      
				WAIT_CYCLE = 3;
			else if (ScaleRatH > 32)      
				WAIT_CYCLE = 7;
			else 
				WAIT_CYCLE = 15;		
			MEM_SCALER_DST_CONFIG(MSC_CFG_ALL, 0, WAIT_CYCLE, 1, 0, 1, 0, DstImgType);
		}
		else
		{
			MEM_SCALER_DST_CONFIG(MSC_CFG_ALL, 0, 1, 1, 0, 0, 0, DstImgType);
		}
	}

//	printk("0xF0210000 : %8x, %8x, %8x, %8x \n",pM2MSCALER->SRCBASEY, pM2MSCALER->SRCBASEU, pM2MSCALER->SRCBASEV, pM2MSCALER->SRCSIZE);
//	printk("0xF0210010 : %8x, %8x, --------, -------- \n",pM2MSCALER->SRCOFF, pM2MSCALER->SRCCFG);
//	printk("0xF0210020 : %8x, %8x, %8x, %8x \n",pM2MSCALER->DSTBASEY, pM2MSCALER->DSTBASEU, pM2MSCALER->DSTBASEV, pM2MSCALER->DSTSIZE);
//	printk("0xF0210030 : %8x, %8x, --------, -------- \n",pM2MSCALER->DSTOFF, pM2MSCALER->DSTCFG);	
//	printk("0xF0210040 : %8x, %8x, %8x, --------\n",pM2MSCALER->MSCINF, pM2MSCALER->MSCCTR, pM2MSCALER->MSCSTR);
//	printk("0xF0210050 : %8x, %8x, %8x, %8x \n",pM2MSCALER->DSTSLINE, pM2MSCALER->DSTCLINE, pM2MSCALER->CLIP0, pM2MSCALER->CLIP1);	
//	printk("0xF0210060 : %8x, --------, --------, --------\n\n",pM2MSCALER->VSTROBE);

//	scaler_ended = check_status_intr = 0;

	if(scale_img->viqe_onthefly& 0x01)
	{
		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x80000000);
		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x00000000);

		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x01000003);
	}
	else if(scale_img->viqe_onthefly & 0x02)
	{
		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x00000021);
	}
	else
	{
		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x80000000);
		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x00000000);
		BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x00000003);
	}
	spin_unlock_irq(&(sc_data.cmd_lock));
	#endif // CONFIG_ARCH_TCC892X

	if(scale_img->responsetype  == SCALER_POLLING)	{
		ret = wait_event_interruptible_timeout(sc_data.poll_wq,  sc_data.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0) {
			 sc_data.block_operating = 0;
			printk("Scaler  time out :%d Line :%d \n", __LINE__, ret);
		}		
	}
	else if(scale_img->responsetype  == SCALER_NOWAIT)	{
		if(scale_img->viqe_onthefly & 0x2)
			 sc_data.block_operating = 0;
	}

	return ret;
}


#if defined(CONFIG_ARCH_TCC892X)
char Alphablending_Ctrl_Func(SCALER_ALPHABLENDING_TYPE* apb_info)
{
	char ret = 0;

	spin_lock_irq(&(sc_data.cmd_lock));

	// set to VRDMA12
	VIOC_CONFIG_RDMA12PathCtrl(0); // enable rdma path.
	VIOC_RDMA_SetImageAlphaEnable(pRDMABase, 1);
	VIOC_RDMA_SetImageAlphaSelect(pRDMABase, 1);
	VIOC_RDMA_SetImageFormat(pRDMABase, apb_info->src0_fmt);
	VIOC_RDMA_SetImageSize(pRDMABase, apb_info->src0_width, apb_info->src0_height);
	VIOC_RDMA_SetImageOffset(pRDMABase, apb_info->src0_fmt, apb_info->src0_width);
	VIOC_RDMA_SetImageBase(pRDMABase, apb_info->src0_Yaddr, apb_info->src0_Uaddr, apb_info->src0_Vaddr);

	// set to VRDMA13
	VIOC_RDMA_SetImageAlphaEnable(pRDMA1Base, 1);
	VIOC_RDMA_SetImageAlphaSelect(pRDMA1Base, 1);
	VIOC_RDMA_SetImageFormat(pRDMA1Base, apb_info->src1_fmt);
	VIOC_RDMA_SetImageSize(pRDMA1Base, apb_info->src1_width, apb_info->src1_height);
	VIOC_RDMA_SetImageOffset(pRDMA1Base, apb_info->src1_fmt, apb_info->src1_width);
	VIOC_RDMA_SetImageBase(pRDMA1Base, apb_info->src1_Yaddr, apb_info->src1_Uaddr, apb_info->src1_Vaddr);
	VIOC_RDMA_SetImageEnable(pRDMABase); // SoC guide info.
	VIOC_RDMA_SetImageEnable(pRDMA1Base);

	// set to WMIX
	VIOC_WMIX_SetSize(pWIXBase, apb_info->dst_width, apb_info->dst_height);
	// set to layer0 of WMIX.
	VIOC_API_WMIX_SetOverlayAlphaValueControl(pWIXBase, apb_info->src0_layer, apb_info->region, apb_info->src0_acon0, apb_info->src0_acon1);
	VIOC_API_WMIX_SetOverlayAlphaColorControl(pWIXBase, apb_info->src0_layer, apb_info->region, apb_info->src0_ccon0, apb_info->src0_ccon1);
	VIOC_API_WMIX_SetOverlayAlphaROPMode(pWIXBase, apb_info->src0_layer, apb_info->src0_rop_mode);
	VIOC_API_WMIX_SetOverlayAlphaSelection(pWIXBase, apb_info->src0_layer, apb_info->src0_asel);
	VIOC_API_WMIX_SetOverlayAlphaValue(pWIXBase, apb_info->src0_layer, apb_info->src0_alpha0, apb_info->src0_alpha1);
	// set to layer1 of WMIX.
	//VIOC_API_WMIX_SetOverlayAlphaValueControl(pWIXBase, apb_info->src1_layer, apb_info->region, apb_info->src1_acon0, apb_info->src1_acon1);
	//VIOC_API_WMIX_SetOverlayAlphaColorControl(pWIXBase, apb_info->src1_layer, apb_info->region, apb_info->src1_ccon0, apb_info->src1_ccon1);
	//VIOC_API_WMIX_SetOverlayAlphaROPMode(pWIXBase, apb_info->src1_layer, apb_info->src1_rop_mode);
	//VIOC_API_WMIX_SetOverlayAlphaSelection(pWIXBase, apb_info->src1_layer, apb_info->src1_asel);
	//VIOC_API_WMIX_SetOverlayAlphaValue(pWIXBase, apb_info->src1_layer, apb_info->src1_alpha0, apb_info->src1_alpha1);
	// update WMIX.
	VIOC_WMIX_SetUpdate(pWIXBase);

	// set to VWRMA
	VIOC_WDMA_SetImageFormat(pWDMABase, apb_info->dst_fmt);
	VIOC_WDMA_SetImageSize(pWDMABase, apb_info->dst_width, apb_info->dst_height);
	VIOC_WDMA_SetImageOffset(pWDMABase, apb_info->dst_fmt, apb_info->dst_width);
	VIOC_WDMA_SetImageBase(pWDMABase, apb_info->dst_Yaddr, apb_info->dst_Uaddr, apb_info->dst_Vaddr);
	VIOC_WDMA_SetImageEnable(pWDMABase, 0 /* OFF */);
	pWDMABase->uIRQSTS.nREG = 0xFFFFFFFF; // wdma status register all clear.

	spin_unlock_irq(&(sc_data.cmd_lock));

	if(apb_info->rsp_type == SCALER_POLLING) {
		ret = wait_event_interruptible_timeout(sc_data.poll_wq,  sc_data.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0) {
			 sc_data.block_operating = 0;
			printk("Scaler  time out :%d Line :%d \n", __LINE__, ret);
		}
	}

	return ret;
}
#endif // CONFIG_ARCH_TCC892X


static unsigned int tccxxx_scaler_poll(struct file *filp, poll_table *wait)
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
static irqreturn_t tccxxx_scaler_handler(int irq, void *client_data/*, struct pt_regs *reg*/)
{  	
	unsigned long IFlag;
	intr_data_t *msc_data = client_data;
	
	#if defined(CONFIG_ARCH_TCC892X)
	if(pWDMABase->uIRQSTS.nREG & VIOC_WDMA_IREQ_EOFR_MASK) {
		dprintk("WDMA Interrupt is VIOC_WDMA_IREQ_EOFR_MASK. \n");
		pWDMABase->uIRQSTS.nREG = 0xFFFFFFFF;   // wdma status register all clear.
	}
	#else // CONFIG_ARCH_TCC892X
	IFlag = pM2MSCALER->MSCSTR;

	if (IFlag & HwMSC_STATUS_IRDY)	{
		scaler_ended  =1;
	}
	else if (IFlag & HwMSC_STATUS_IBUSY)	{
		scaler_ended  = 2;
		printk("interrupt:: scaler busy!!!\n");
	}
	else 	{
		scaler_ended  = 3;
		printk("unknown interrupt:: 0x%x!!!\n", IFlag);
	}

	BITCSET(pM2MSCALER->MSCCTR, 0xffffffff, 0x00000000);
	pM2MSCALER->MSCSTR = IFlag; // clear interrupt flag
	#endif // CONFIG_ARCH_TCC892X
	dprintk("tccxxx_scaler_handler :0  block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n",
			msc_data->block_operating, msc_data->block_waiting, msc_data->cmd_count, msc_data->poll_count);		

	if(msc_data->block_operating >= 1) {
		msc_data->block_operating = 0;
	}
		
	wake_up_interruptible(&(msc_data->poll_wq));

	if(msc_data->block_waiting)
		wake_up_interruptible(&msc_data->cmd_wq);

	return IRQ_HANDLED;
}


#if defined(CONFIG_ARCH_TCC892X)
void tccxxx_convert_image_format(SCALER_TYPE *pScalerInfo)
{
	dprintk("before: src_fmt=%d, dst_fmt=%d. \n", pScalerInfo->src_fmt, pScalerInfo->dest_fmt);
	switch((unsigned char)pScalerInfo->src_fmt) {
		case SCALER_YUV422_sq0:
			pScalerInfo->src_fmt = SC_IMG_FMT_YCbCr422_SEQ_YUYV;
			break;
		case SCALER_YUV422_sq1:
			pScalerInfo->src_fmt = SC_IMG_FMT_YCbCr422_SEQ_UYVY;
			break;
		case SCALER_YUV422_sp:
			pScalerInfo->src_fmt = SC_IMG_FMT_YCbCr422_SEP;
			break;
		case SCALER_YUV420_sp:
			pScalerInfo->src_fmt = SC_IMG_FMT_YCbCr420_SEP;
			break;
		case SCALER_YUV422_inter:
			pScalerInfo->src_fmt = SC_IMG_FMT_YCbCr422_INT_TYPE0;
			break;
		case SCALER_YUV420_inter:
			pScalerInfo->src_fmt = SC_IMG_FMT_YCbCr420_INT_TYPE0;
			break;
		case SCALER_RGB565:
			pScalerInfo->src_fmt = SC_IMG_FMT_RGB565;
			break;
		case SCALER_RGB555:
			//pScalerInfo->src_fmt = SC_IMG_FMT_RGB555;
			//break;
		case SCALER_RGB454:
			pScalerInfo->src_fmt = SC_IMG_FMT_RGB555;
			break;
		case SCALER_RGB444:
			pScalerInfo->src_fmt = SC_IMG_FMT_RGB444;
			break;
		case SCALER_ARGB8888:
			pScalerInfo->src_fmt = SC_IMG_FMT_ARGB8888;
			break;
	}

	switch((unsigned char)pScalerInfo->dest_fmt) {
		case SCALER_YUV422_sq0:
			pScalerInfo->dest_fmt = SC_IMG_FMT_YCbCr422_SEQ_YUYV;
			break;
		case SCALER_YUV422_sq1:
			pScalerInfo->dest_fmt = SC_IMG_FMT_YCbCr422_SEQ_UYVY;
			break;
		case SCALER_YUV422_sp:
			pScalerInfo->dest_fmt = SC_IMG_FMT_YCbCr422_SEP;
			break;
		case SCALER_YUV420_sp:
			pScalerInfo->dest_fmt = SC_IMG_FMT_YCbCr420_SEP;
			break;
		case SCALER_YUV422_inter:
			pScalerInfo->dest_fmt = SC_IMG_FMT_YCbCr422_INT_TYPE0;
			break;
		case SCALER_YUV420_inter:
			pScalerInfo->dest_fmt = SC_IMG_FMT_YCbCr420_INT_TYPE0;
			break;
		case SCALER_RGB565:
			pScalerInfo->dest_fmt = SC_IMG_FMT_RGB565;
			break;
		case SCALER_RGB555:
			//pScalerInfo->dest_fmt = SC_IMG_FMT_RGB555;
			//break;
		case SCALER_RGB454:
			pScalerInfo->dest_fmt = SC_IMG_FMT_RGB555;
			break;
		case SCALER_RGB444:
			pScalerInfo->dest_fmt = SC_IMG_FMT_RGB444;
			break;
		case SCALER_ARGB8888:
			pScalerInfo->dest_fmt = SC_IMG_FMT_ARGB8888;
			break;
	}
	dprintk("after: src_fmt=%d, dst_fmt=%d. \n", pScalerInfo->src_fmt, pScalerInfo->dest_fmt);
}
#endif

long tccxxx_scaler_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	SCALER_TYPE scaler_v;
	#if defined(CONFIG_ARCH_TCC892X)
	SCALER_PLUGIN_Type scaler_plugin;
	VIOC_SCALER_INFO_Type pScalerInfo;
	SCALER_ALPHABLENDING_TYPE alpha_blending;
	#endif
	intr_data_t *msc_data = (intr_data_t *)file->private_data;

	dprintk("scaler  ::cmd:0x%x block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n",
			cmd, msc_data->block_operating, msc_data->block_waiting, msc_data->cmd_count, msc_data->poll_count);		
	
	switch(cmd)
	{
		case TCC_SCALER_IOCTRL:			
		case TCC_SCALER_IOCTRL_KERENL:
			mutex_lock(&msc_data->io_mutex);
			if(msc_data->block_operating) {
				msc_data->block_waiting = 1;
				ret = wait_event_interruptible_timeout(msc_data->cmd_wq, msc_data->block_operating == 0, msecs_to_jiffies(200));
				if(ret <= 0) {
					msc_data->block_operating = 0;
					printk("[%d]: scaler 0 timed_out block_operation:%d!! cmd_count:%d \n", ret, msc_data->block_waiting, msc_data->cmd_count);
				}
				ret = 0;
			}

			if(cmd == TCC_SCALER_IOCTRL_KERENL)	{
				memcpy(&scaler_v,(SCALER_TYPE*)arg,sizeof(scaler_v));
			} else {
				if(copy_from_user(&scaler_v,(SCALER_TYPE*)arg,sizeof(scaler_v))) {
					printk(KERN_ALERT "Not Supported copy_from_user(%d)\n", cmd);
					ret = -EFAULT;
				}
			}
			
			if(ret >= 0) {
				if(msc_data->block_operating >= 1) {
					printk("scaler + :: block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n", \
							msc_data->block_operating, msc_data->block_waiting, msc_data->cmd_count, msc_data->poll_count);
				}

				#if defined(CONFIG_ARCH_TCC892X)
				tccxxx_convert_image_format(&scaler_v);
				#endif // CONFIG_ARCH_TCC892X

				msc_data->block_waiting = 0;
				msc_data->block_operating = 1;
				ret = M2M_Scaler_Ctrl_Detail(&scaler_v);
				if(ret < 0) 	msc_data->block_operating = 0;
			}
			mutex_unlock(&msc_data->io_mutex);
			return ret;

		#if defined(CONFIG_ARCH_TCC892X)
		case TCC_SCALER_VIOC_PLUGIN:
			mutex_lock(&msc_data->io_mutex);

			if(copy_from_user(&scaler_plugin,(SCALER_PLUGIN_Type *)arg, sizeof(SCALER_PLUGIN_Type))) {
				printk(KERN_ALERT "Not Supported copy_from_user(%d)\n", cmd);
				ret = -EFAULT;
			}

			// set to scaler & plug in.
			pScalerInfo.BYPASS 			= scaler_plugin.bypass_mode;
			pScalerInfo.SRC_WIDTH 		= scaler_plugin.src_width;
			pScalerInfo.SRC_HEIGHT 		= scaler_plugin.src_height;
			pScalerInfo.DST_WIDTH 		= scaler_plugin.dst_width;
			pScalerInfo.DST_HEIGHT 		= scaler_plugin.dst_height;
			pScalerInfo.OUTPUT_POS_X 	= scaler_plugin.dst_win_left;
			pScalerInfo.OUTPUT_POS_Y 	= scaler_plugin.dst_win_top;
			pScalerInfo.OUTPUT_WIDTH 	= (scaler_plugin.dst_width  - scaler_plugin.dst_win_left);
			pScalerInfo.OUTPUT_HEIGHT 	= (scaler_plugin.dst_height - scaler_plugin.dst_win_top);

			VIOC_SC_SetSWReset(scaler_plugin.scaler_no, 0xFF/*RDMA*/, 0xFF/*WDMA*/);
			VIOC_API_SCALER_SetConfig(scaler_plugin.scaler_no, &pScalerInfo);
			ret = VIOC_API_SCALER_SetPlugIn(scaler_plugin.scaler_no, scaler_plugin.plugin_path);
			VIOC_API_SCALER_SetUpdate(scaler_plugin.scaler_no);
			
			mutex_unlock(&msc_data->io_mutex);
			return ret;

		case TCC_SCALER_VIOC_PLUGOUT:
			VIOC_RDMA_SetImageIntl(pRDMABase, 0);
			ret = VIOC_API_SCALER_SetPlugOut((unsigned int)arg);
			return ret;

		case TCC_SCALER_VIOC_ALPHA_INIT:
			pRDMA1Base = (volatile PVIOC_RDMA)tcc_p2v((unsigned int)HwVIOC_RDMA13);
			VIOC_SC_SetSWReset(100, 13/*RDMA13*/, 100); // only rdma13 reset.
			return ret;

		case TCC_SCALER_VIOC_ALPHA_RUN:
			mutex_lock(&msc_data->io_mutex);

			if(msc_data->block_operating) {
				msc_data->block_waiting = 1;
				ret = wait_event_interruptible_timeout(msc_data->cmd_wq, msc_data->block_operating == 0, msecs_to_jiffies(200));
				if(ret <= 0) {
					msc_data->block_operating = 0;
					printk("[%d]: scaler 0 timed_out block_operation:%d!! cmd_count:%d \n", ret, msc_data->block_waiting, msc_data->cmd_count);
				}
				ret = 0;
			}

			if(copy_from_user(&alpha_blending,(SCALER_ALPHABLENDING_TYPE *)arg, sizeof(SCALER_ALPHABLENDING_TYPE))) {
				printk(KERN_ALERT "Not Supported copy_from_user(%d)\n", cmd);
				ret = -EFAULT;
			}

			if(ret >= 0) {
				if(msc_data->block_operating >= 1) {
					printk("scaler + :: block_operating(%d) - block_waiting(%d) - cmd_count(%d) - poll_count(%d)!!!\n", \
							msc_data->block_operating, msc_data->block_waiting, msc_data->cmd_count, msc_data->poll_count);
				}

				msc_data->block_waiting = 0;
				msc_data->block_operating = 1;
				ret = Alphablending_Ctrl_Func(&alpha_blending);
				if(ret < 0) 	msc_data->block_operating = 0;
			}

			mutex_unlock(&msc_data->io_mutex);
			return ret;
		#endif // CONFIG_ARCH_TCC892X

		default:
			printk(KERN_ALERT "Not Supported SCALER_IOCTL(%d)\n", cmd);
			break;			
	}

	return 0;
}
EXPORT_SYMBOL(tccxxx_scaler_ioctl);

int tccxxx_scaler_release(struct inode *inode, struct file *filp)
{
	int ret = 0;
	dprintk("scaler_release In!! %d'th, block(%d/%d), cmd(%d), irq(%d)  \n", sc_data.dev_opened, sc_data.block_operating, sc_data.block_waiting, sc_data.cmd_count, sc_data.irq_reged);

	if(sc_data.dev_opened > 0)
		sc_data.dev_opened--;

	if(sc_data.dev_opened == 0)
	{
		if(sc_data.block_operating)
		{
			ret = wait_event_interruptible_timeout(sc_data.cmd_wq, sc_data.block_operating == 0, msecs_to_jiffies(200));

			if(ret <= 0)
	 			printk("[%d]: scaler 0 timed_out block_operation:%d!! cmd_count:%d \n", ret, sc_data.block_waiting, sc_data.cmd_count);
		}

		#if defined(CONFIG_ARCH_TCC892X)
		if(sc_data.irq_reged) {
			free_irq(INT_VIOC_WD3, &sc_data);
			sc_data.irq_reged = 0;
		}
		VIOC_CONFIG_PlugOut(VIOC_SC1);
		VIOC_SC_SetSWReset(VIOC_SC1, 12/*RDMA12*/, 3/*WDMA03*/);
		#else // CONFIG_ARCH_TCC892X
		if(sc_data.irq_reged)	{
			free_irq(INT_SC0, &sc_data);	
			sc_data.irq_reged = 0;
		}

		M2M_Scaler_SW_Reset(M2M_SCALER0);
		#endif // CONFIG_ARCH_TCC892X

		sc_data.block_operating = sc_data.block_waiting = 0;
		sc_data.poll_count = sc_data.cmd_count = 0;
	}

	clk_disable(m2m1_clk);
	#if !defined(CONFIG_ARCH_TCC892X)
	clk_disable(m2m0_ddi_cache);
	#endif // CONFIG_ARCH_TCC892X

	dprintk("scaler_release Out!! %d'th \n", sc_data.dev_opened);

	return 0;
}
EXPORT_SYMBOL(tccxxx_scaler_release);

int tccxxx_scaler_open(struct inode *inode, struct file *filp)
{	
	int ret = 0;

	dprintk("scaler_open In!! %d'th, block(%d/%d), cmd(%d), irq(%d) \n", sc_data.dev_opened, sc_data.block_operating, sc_data.block_waiting, sc_data.cmd_count, sc_data.irq_reged);

	clk_enable(m2m1_clk);
	#if !defined(CONFIG_ARCH_TCC892X)
	clk_enable(m2m0_ddi_cache);
	#endif // CONFIG_ARCH_TCC892X

	if(!sc_data.irq_reged) {
		#if defined(CONFIG_ARCH_TCC892X)
		// set to VRDMA
		pRDMABase = (volatile PVIOC_RDMA)tcc_p2v((unsigned int)HwVIOC_RDMA12);

		// set to WMIX3
		pWIXBase = (volatile PVIOC_WMIX)tcc_p2v((unsigned int)HwVIOC_WMIX3);

		// set to VWDMA
		pWDMABase = (volatile PVIOC_WDMA)tcc_p2v((unsigned int)HwVIOC_WDMA03);

		VIOC_SC_SetSWReset(VIOC_SC1, 12/*RDMA12*/, 3/*WDMA03*/);
		VIOC_WDMA_SetIreqMask(pWDMABase, VIOC_WDMA_IREQ_EOFR_MASK, 0x0);
		ret = request_irq(INT_VIOC_WD3, tccxxx_scaler_handler, IRQF_SHARED, "scaler", &sc_data);
		#else // CONFIG_ARCH_TCC892X
		M2M_Scaler_SW_Reset(M2M_SCALER0);
		MEMSCL_SET_INTERRUPT_INIT(M2M_SCALER0);
		MEMSCL_SET_INTERRUPT(M2M_SCALER0, SET_M2MSCL_INT_ENABLE);
		ret = request_irq(INT_SC0, tccxxx_scaler_handler, IRQF_DISABLED, "scaler", &sc_data);
		#endif // CONFIG_ARCH_TCC892X

		
		if (ret) {
			clk_disable(m2m1_clk);
			#if !defined(CONFIG_ARCH_TCC892X)
			clk_disable(m2m0_ddi_cache);
			#endif // CONFIG_ARCH_TCC892X
			printk("FAILED to aquire scaler0-irq\n");
			return -EFAULT;
		}

		sc_data.irq_reged = 1;
	}

	sc_data.dev_opened++;
	filp->private_data = &sc_data;
	
	dprintk("scaler_open Out!! %d'th \n", sc_data.dev_opened);
	
	return ret;	
}
EXPORT_SYMBOL(tccxxx_scaler_open);


static struct file_operations tcc_scaler_fops = 
{
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= tccxxx_scaler_ioctl,
	.mmap			= tccxxx_scaler_mmap,
	.open			= tccxxx_scaler_open,
	.release		= tccxxx_scaler_release,
	.poll			= tccxxx_scaler_poll,
};

void __exit
tccxxx_scaler_cleanup(void)
{
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
	clk_put(m2m1_clk);

	return;
}

static char banner[] __initdata = KERN_INFO "TCC scaler driver initializing\n";

static struct class *scaler_class;

int __init 
tccxxx_scaler_init(void)
{
	printk(banner);
	
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_scaler_fops);
	scaler_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(scaler_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);

	memset(&sc_data, 0, sizeof(intr_data_t));

	spin_lock_init(&(sc_data.poll_lock));
	spin_lock_init(&(sc_data.cmd_lock));

	mutex_init(&(sc_data.io_mutex));
	
	init_waitqueue_head(&(sc_data.poll_wq));
	init_waitqueue_head(&(sc_data.cmd_wq));

	#if defined(CONFIG_ARCH_TCC892X)
	m2m1_clk = clk_get(NULL, "ddi");
	BUG_ON(m2m1_clk == NULL);

	pM2MSCALER = (volatile PVIOC_SC)tcc_p2v((unsigned int)HwVIOC_SC1);
	#else // CONFIG_ARCH_TCC892X
	m2m1_clk = clk_get(NULL, "m2m0");
	BUG_ON(m2m1_clk == NULL);

	m2m0_ddi_cache = clk_get(NULL, "ddi_cache");
	BUG_ON(m2m0_ddi_cache == NULL);

	pM2MSCALER = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	#endif // CONFIG_ARCH_TCC892X
	
	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCC scaler driver");
MODULE_LICENSE("GPL");


arch_initcall(tccxxx_scaler_init);
module_exit(tccxxx_scaler_cleanup);


