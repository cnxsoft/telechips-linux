/*
 * drivers/video/Tcc_overlay.c
 *
 * Copyright (C) 2004 Telechips, Inc. 
 *
 * Video-for-Linux (Version 2) graphic capture driver
 *
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/videodev2.h>
#include <linux/miscdevice.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include <linux/poll.h>

#include <mach/bsp.h>

#include <mach/tcc_fb.h>
#include <mach/tcc_overlay_ioctl.h>
#include <mach/tccfb_ioctrl.h>
#ifdef CONFIG_TCC_VIOC_CONTROLLER
#include <mach/vioc_outcfg.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_disp.h>
#include <mach/vioc_global.h>
#endif//

#if 0
static int debug	   = 1;
#else
static int debug	   = 0;
#endif

#define dprintk(msg...)	if (debug) { printk( "tcc_overlay: " msg); }

#define PLATFORM_DEVICE

#define DEVICE_NAME			"overlay"
#ifdef PLATFORM_DEVICE
#define DEV_MINOR	202
#else
#define MAJOR_ID	202
#define MINOR_ID	1
#endif

//#define OVERLAY_CNT_DEFAULT 2

static struct clk *overlay_lcdc_clk;

static overlay_config_t overlay_cfg;
static struct v4l2_format overlay_fmt;

			
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
/* LCD Overlay Setting*/
#define IMG_INTL	       	0
#define IMG_AEN           	0
#define IMG_CEN           	0
#define IMG_IEN           	1
#define IMG_AOPT          	2
#define IMG_ASEL          	0
#define IMG_PD            	0
#define IMG_Y2RMD         	1
#define IMG_Y2R          	1
#define IMG_BR            	0

#define IMG_RGB565_FMT           	10	//RGB888
#define IMG_RGB888_FMT           	12	//RGB888
#define IMG_RGBYUV420_FMT         24	//RGB888
#define IMG_RGBYUV422SP_FMT         25	//RGB888
#define IMG_RGBYUV422SQ_FMT         26	//RGB888
#define IMG_RGBYUV420I_FMT         28	//YUV420_INTERLEAVED

#define IMG_POSITION_Y    	0
#define IMG_POSITION_X    	0
#define IMG_HEIGHT	       	LCD_HEIGHT
#define IMG_WIDTH         		LCD_WIDTH
#define IMG_AVAL0         		95
#define IMG_AVAL1         		95
static PLCDC		pLCDC1;
static volatile PLCDC_CHANNEL pLCDC1_CH0;
#else
static VIOC_WMIX *pWMIXBase1;
static VIOC_RDMA *pRDMABase6;
#endif//


static unsigned char start_en = 0;
static unsigned char wait_restart = 0;
static unsigned char pos_reset = 0;

static unsigned char overlay_en_count = 0;
unsigned char tcc_overlay_use = 0;

//#define VC_OVERLAY_PROFILE // CAM_28pin, GPIO_D24
#ifdef VC_OVERLAY_PROFILE
static unsigned char toggle_int = 0;
#endif

extern struct display_platform_data tcc_display_data;
extern OUTPUT_SELECT_MODE	Output_SelectMode;

void tccxxx_overlay_check_priority(void);
unsigned char tccxxx_overlay_use(void)
{
	return tcc_overlay_use;
}
EXPORT_SYMBOL(tccxxx_overlay_use);

void tccxxx_overlay_start(void)
{
	tccxxx_overlay_check_priority();

	if(!start_en){
		dprintk("call start en \n");		
		start_en = 1;
	}
}
EXPORT_SYMBOL(tccxxx_overlay_start);

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int tccxxx_overlay_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR  "overlay: this address is not allowed \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff , vma->vm_end - vma->vm_start, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	vma->vm_ops	= NULL;
	vma->vm_flags 	|= VM_IO;
	vma->vm_flags 	|= VM_RESERVED;
	
	return 0;
}


DECLARE_WAIT_QUEUE_HEAD(overlay_wait);

static unsigned int tccxxx_overlay_poll(struct file *file, struct poll_table_struct *wait)
{
	dprintk(" tccxxx_overlay_poll wait[%d][%d]!!!\n", (unsigned)wait, (unsigned)&overlay_wait);
	poll_wait(file, &(overlay_wait), wait);	
	dprintk(" tccxxx_overlay_poll finish[%d][%d]!!!\n", (unsigned)wait, (unsigned)&overlay_wait);	
	return POLLIN;
}

static int tccxxx_overlay_get_pos(overlay_config_t * arg )
{
	overlay_config_t pos;

	pos.sx 		= overlay_cfg.sx;
	pos.sy 		= overlay_cfg.sy;
	pos.width	= overlay_cfg.width;
	pos.height	= overlay_cfg.height;
	dprintk(" Overlay -> Get Position :: (%d,%d) | (%d,%d) \n", overlay_cfg.sx, overlay_cfg.sy, overlay_cfg.width, overlay_cfg.height);
	
	if(copy_to_user((overlay_config_t *)arg, &pos, sizeof(overlay_config_t)))
		return -EFAULT;

	return 0;
}

static int tccxxx_overlay_get_screenInfo(overlay_config_t * arg )
{
	struct lcd_panel *panel;
	unsigned int screen_width, screen_height;

	overlay_config_t screen_info;

	panel = tccfb_get_panel();
    screen_width      = panel->xres;
    screen_height     = panel->yres;

#if defined(CONFIG_TCC_HDMI_UI_SIZE_1280_720)
    if(tcc_display_data.resolution == 1)
    {
        screen_width      = 720;
        screen_height     = 576;
    }
    else if(tcc_display_data.resolution == 2)
    {
        screen_width      = 800;
        screen_height     = 480;
    }
#endif	

	screen_info.sx 		= 0;
	screen_info.sy 		= 0;
	screen_info.width	= screen_width;
	screen_info.height	= screen_height;
	
	dprintk(" Overlay -> Get ScreenInfo :: (%d,%d) | (%d,%d) \n", screen_info.sx, screen_info.sy, screen_info.width, screen_info.height);
	
	if(copy_to_user((overlay_config_t *)arg, &screen_info, sizeof(overlay_config_t)))
		return -EFAULT;

	return 0;
}

void tccxxx_overlay_fmt_set(unsigned int fmt)
{
	dprintk(" Overlay -> S_FMT :: format = 0x%x(RGB565-0x%x, YUV420-0x%x, YUV420inter-0x%x) \n", fmt, V4L2_PIX_FMT_RGB565,V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_NV12);
	#if  defined (CONFIG_TCC_LCDC_CONTROLLER)
	if(fmt == V4L2_PIX_FMT_RGB565)
	{
		BITCSET (pLCDC1->LI0C, 0x1f<< 0,	(IMG_RGB565_FMT) <<  0); // format
		BITCSET (pLCDC1->LI0O, 0x0000FFFF,	overlay_cfg.width * 2); // format
		BITCSET (pLCDC1->LI0C, 0x1<<  8,	(0)  <<  8); // y2r converter enable
	}
	else if(fmt == V4L2_PIX_FMT_NV12)
	{
		BITCSET (pLCDC1->LI0C, 0x1f<< 0,	(IMG_RGBYUV420I_FMT) <<  0); // format
		BITCSET (pLCDC1->LI0O, 0xFFFFFFFF,	((overlay_cfg.width)<<16) | (overlay_cfg.width)); // format 			
		BITCSET (pLCDC1->LI0C, 0x1<<  8,	(IMG_Y2R)  <<  8); // y2r converter enable
	}
	else if(fmt == V4L2_PIX_FMT_YUV422P)
	{
		BITCSET (pLCDC1->LI0C, 0x1f<< 0,	(IMG_RGBYUV422SQ_FMT) <<  0); // format
		BITCSET (pLCDC1->LI0O, 0x0000FFFF,	overlay_cfg.width * 2); // format
		BITCSET (pLCDC1->LI0C, 0x1<<  8,	(IMG_Y2R)  <<  8); // y2r converter enable
	}
	else
	{
		BITCSET (pLCDC1->LI0C, 0x1f<< 0,	(IMG_RGBYUV420_FMT) <<	0); // format
		BITCSET (pLCDC1->LI0O, 0xFFFFFFFF,	((overlay_cfg.width/2)<<16) | (overlay_cfg.width)); // format				
		BITCSET (pLCDC1->LI0C, 0x1<<  8,	(IMG_Y2R)  <<  8); // y2r converter enable
	}
	#else

	if(fmt == V4L2_PIX_FMT_RGB565)
	{
		VIOC_RDMA_SetImageFormat(pRDMABase6, VIOC_IMG_FMT_RGB565);
		VIOC_RDMA_SetImageOffset(pRDMABase6, VIOC_IMG_FMT_RGB565, overlay_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase6, 0);
	}
	else if(fmt == V4L2_PIX_FMT_NV12)
	{
		VIOC_RDMA_SetImageFormat(pRDMABase6, VIOC_IMG_FMT_YUV420IL0);
		VIOC_RDMA_SetImageOffset(pRDMABase6, VIOC_IMG_FMT_YUV420IL0, overlay_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase6, 1);
	}
	else if(fmt == V4L2_PIX_FMT_YUV422P)
	{
		VIOC_RDMA_SetImageFormat(pRDMABase6, VIOC_IMG_FMT_YUYV);
		VIOC_RDMA_SetImageOffset(pRDMABase6, VIOC_IMG_FMT_YUYV, overlay_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase6, 1);
	}
	else
	{
		VIOC_RDMA_SetImageFormat(pRDMABase6, VIOC_IMG_FMT_YUV420SEP);
		VIOC_RDMA_SetImageOffset(pRDMABase6, VIOC_IMG_FMT_YUV420SEP, overlay_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase6, 1);	
	}	
	#endif//
}

void tccxxx_overlay_check_priority(void)
{
	unsigned int ch0_region, ch1_region;

#if  defined (CONFIG_TCC_LCDC_CONTROLLER)
	PLCDC pLCD;

#if !defined(CONFIG_ARCH_TCC92XX)	
	if(Output_SelectMode != OUTPUT_SELECT_NONE)
	{	
		pLCD = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	}
	else
#endif
	{
		pLCD = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	}
	
	if((pLCD->LI1C & Hw28) && (pLCD->LI0C & Hw28))
	{
		ch0_region = (pLCD->LI0S&0xFFFF) * ((pLCD->LI0S >> 16)&0xFFFF);
		ch1_region = (pLCD->LI1S&0xFFFF) * ((pLCD->LI1S >> 16)&0xFFFF);
		
		if(ch0_region < ch1_region)
		{
			// 2 > 0 > 1			
			dprintk(" CH_priority :%d: (2 > 0 > 1) %d x %d < %d x %d \n", Output_SelectMode, 
						(pLCD->LI0S&0xFFFF), ((pLCD->LI0S >> 16)&0xFFFF), (pLCD->LI1S&0xFFFF), ((pLCD->LI1S >> 16)&0xFFFF));
			BITCSET (pLCD->LCTRL, HwLCTRL_OVP, 0x3<<1);
		}
		else
		{
			// 2 > 1 > 0			
			dprintk(" CH_priority :: (2 > 1 > 0) %d x %d < %d x %d \n", Output_SelectMode, 
						(pLCD->LI0S&0xFFFF), ((pLCD->LI0S >> 16)&0xFFFF), (pLCD->LI1S&0xFFFF), ((pLCD->LI1S >> 16)&0xFFFF));
			BITCSET (pLCD->LCTRL, HwLCTRL_OVP, 0x5<<1);
		}
	}
	else
	{
		BITCSET (pLCD->LCTRL, HwLCTRL_OVP, 0x5<<1);
	}
#else
	unsigned int ch1_en, ch0_en;
	VIOC_WMIX* pWMIXBase;
	VIOC_RDMA* pRDMABase0, *pRDMABase1 ;


		
	if(Output_SelectMode != OUTPUT_SELECT_NONE)
	{	
		// LCD channel
		pRDMABase0 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA05);
		pRDMABase1 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA06);
		pWMIXBase = (VIOC_WMIX *) tcc_p2v(HwVIOC_WMIX1);
	}
	else
	{
		// External display channel
		pRDMABase0 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA01);
		pRDMABase1 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA02);
		pWMIXBase = (VIOC_WMIX *) tcc_p2v(HwVIOC_WMIX0);
	}

	VIOC_RDMA_GetImageEnable(pRDMABase0, &ch0_en);
	VIOC_RDMA_GetImageEnable(pRDMABase1, &ch1_en);
	
	if(ch0_en && ch1_en)
	{
		unsigned int sw0, sh0, sw1, sh1;
		
		VIOC_RDMA_GetImageSize(pRDMABase0, &sw0, &sh0);
		VIOC_RDMA_GetImageSize(pRDMABase1, &sw1, &sh1);

		ch0_region = sw0 * sh0;
		ch1_region = sw1 * sh1;
		
		if(ch0_region < ch1_region)
		{
			// 2 > 0 > 1			
			dprintk(" CH_priority :%d: (2 > 0 > 1) %d x %d < %d x %d \n", Output_SelectMode, sw0, sh0, sw1, sh1);
			VIOC_WMIX_SetOverlayPriority(pWMIXBase, 25);
		}
		else
		{
			// 2 > 1 > 0			
			dprintk(" CH_priority : %d : (2 > 1 > 0) %d x %d < %d x %d \n", Output_SelectMode, sw0, sh0, sw1, sh1);
			VIOC_WMIX_SetOverlayPriority(pWMIXBase, 24);
		}
	}
	else
	{
			VIOC_WMIX_SetOverlayPriority(pWMIXBase, 24);
	}
#endif//

	return;
}

void tccxxx_overlay_common_enable(void)
{
#if  defined (CONFIG_TCC_LCDC_CONTROLLER)
	unsigned int reg = 0, alpha_type = 0;	
	unsigned int chroma_en;
	unsigned int alpha_blending_en;
	unsigned int chromaR, chromaG, chromaB, alpha_value;

	if(!overlay_en_count)
	{
		reg = pLCDC1->LI2C;
		reg= (reg & 0x1F);//RGB888
		
		if(reg == 0xC) // RGB888
		{
			chroma_en = 0;
			alpha_value = 200;
			alpha_type = 1;
			alpha_blending_en = 1;//1;
			chromaR = chromaG = chromaB = 0x00;
		}
		else
		{
			chroma_en = 1;
			alpha_value = 200;
			alpha_type = 0;
			alpha_blending_en = 0;//1;
			chromaR = chromaG = chromaB = 0x00;
		}
		
		// overlay 속성
		BITCSET (pLCDC1->LI2C,	0x1<< 30,  (alpha_blending_en)	<< 30); // alpha enable
		BITCSET (pLCDC1->LI2C, 0x1<< 29,	(chroma_en)  << 29); // chroma-keying
		
		BITCSET (pLCDC1->LI2KR, 0xff <<  0, (chromaR)  <<  0); // key
		BITCSET (pLCDC1->LI2KR, 0xff << 16, (0xF8)	<< 16); // keymask
		BITCSET (pLCDC1->LI2KG, 0xff <<  0, (chromaG)  <<  0); // key
		BITCSET (pLCDC1->LI2KG, 0xff << 16, (0xFC)	<< 16); // keymask
		BITCSET (pLCDC1->LI2KB, 0xff <<  0, (chromaB)  <<  0); // key
		BITCSET (pLCDC1->LI2KB, 0xff << 16, (0xF8)	<< 16); // keymask
		
		// alpha_value 0~255  -- 0~100% 까지 
		BITCSET (pLCDC1->LI2A, 0xffff <<16,  (alpha_value)<< 16); // alpha1
		BITCSET (pLCDC1->LI2A, 0xffff << 0,  (alpha_value)<<  0); // alpha0
		BITCSET (pLCDC1->LI2C, 0x3<< 25,	(IMG_AOPT)	<< 25); // alpha opt
		BITCSET (pLCDC1->LI2C, 0x1<< 24,	(alpha_type)  << 24); // alpha select  

#if !defined(CONFIG_ARCH_TCC92XX)
		BITCSET (pLCDC1->LI2C, HwLCT_RU, HwLCT_RU); //Image update 
#endif
	}

#else

#endif//

	if(overlay_en_count < 2) //max overlay is 2.
		overlay_en_count++;
	dprintk("Enable :: overlay_en_count = %d \n", overlay_en_count);
}
EXPORT_SYMBOL(tccxxx_overlay_common_enable);

#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
void tccxxx_overlay_common_disable(int channel, VIOC_RDMA *pRdmaBase)
#else
void tccxxx_overlay_common_disable(int channel)
#endif
{
#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
	VIOC_RDMA* pRDMABase = pRdmaBase;
	unsigned int enable;
#endif//

	dprintk("overlay disable ch:%d output mode:%d \n", channel, Output_SelectMode);

	if(channel == 0)
	{
#if !defined(CONFIG_ARCH_TCC92XX)	
		if(Output_SelectMode != OUTPUT_SELECT_NONE)
		{			
			#if  defined (CONFIG_TCC_LCDC_CONTROLLER)
			PLCDC pLCDC0 = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

			if(pLCDC0->LI0C & Hw28)
			{
				BITCSET (pLCDC0->LI0C, 0x1<< 28, (0)<<28); // lcdc channel enable
				BITCSET (pLCDC0->LI0C, HwLCT_RU, HwLCT_RU); //Image update
			}
			#else
			VIOC_RDMA_SetImageDisable(pRDMABase);
			#endif//
		}
#endif

	#if  defined (CONFIG_TCC_LCDC_CONTROLLER)
		if(!(pLCDC1->LI0C & Hw28))
			return;
	#else
		VIOC_RDMA_GetImageEnable(pRDMABase, &enable);
		if(!enable)
			return;
	#endif//
	}
	else if(channel == 1)
	{
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		if(!(pLCDC1->LI1C & Hw28))
			return;
		#else
		VIOC_RDMA_GetImageEnable(pRDMABase, &enable);
		if(!enable)
			return;
		#endif//
	}
	else
	{
		//to do
	}

	if(overlay_en_count > 0)
		overlay_en_count--;


	if((!overlay_en_count)
		#if defined(CONFIG_ARCH_TCC92XX)
		&& (Output_SelectMode == OUTPUT_SELECT_NONE)
		#endif
	)
	{
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
			#if !defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
				BITCSET (pLCDC1->LI2C,  0x1<< 30,  (0)  << 30); // alpha enable
				BITCSET (pLCDC1->LI2C, 0x1<< 29,    (0)  << 29); // chroma-keying

				#if !defined(CONFIG_ARCH_TCC92XX)
				BITCSET (pLCDC1->LI2C, HwLCT_RU, HwLCT_RU); //Image update 
				#endif
			#endif
		#else
		VIOC_WMIX *pWMIXBase;
		unsigned int nKeyEn, nKeyR, nKeyG, nKeyB, nKeyMaskR, nKeyMaskG, nKeyMaskB;
		pWMIXBase = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX1);
		VIOC_WMIX_GetChromaKey(pWMIXBase, 0, &nKeyEn, &nKeyR, &nKeyG, &nKeyB, &nKeyMaskR, &nKeyMaskG, &nKeyMaskB);
		nKeyEn = 0;
		VIOC_WMIX_SetChromaKey(pWMIXBase, 0, nKeyEn, nKeyR, nKeyG, nKeyB, nKeyMaskR, nKeyMaskG, nKeyMaskB);
		
		#endif//
	}

	#if defined(CONFIG_ARCH_TCC92XX)
	if(Output_SelectMode == OUTPUT_SELECT_NONE)
	#endif
	{
		if(channel == 0)
		{
			#if defined(CONFIG_TCC_LCDC_CONTROLLER)
				BITCSET (pLCDC1->LI0C, 0x1<< 28,	(0)  << 28); // lcdc channel enable
				#if !defined(CONFIG_ARCH_TCC92XX)
				BITCSET (pLCDC1->LI0C, HwLCT_RU, HwLCT_RU); //Image update
				#endif
			#else
				VIOC_RDMA_SetImageDisable(pRDMABase);
			#endif//
		}
		else if(channel == 1)
		{
			#if defined(CONFIG_TCC_LCDC_CONTROLLER)
				BITCSET (pLCDC1->LI1C, 0x1<< 28,	(0)  << 28); // lcdc channel enable
				#if !defined(CONFIG_ARCH_TCC92XX)
				BITCSET (pLCDC1->LI1C, HwLCT_RU, HwLCT_RU); //Image update
				#endif
			#else
				VIOC_RDMA_SetImageDisable(pRDMABase);
			#endif//
		}
		else
		{
			
		}
	}
	
	dprintk("Disable :: overlay_en_count = %d \n", overlay_en_count);
}
EXPORT_SYMBOL(tccxxx_overlay_common_disable);

int tccxxx_overlay_q_buffer(unsigned int* arg )
{
	unsigned int curY_phyAddr, curU_phyAddr, curV_phyAddr;
#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
	unsigned int enable;
#endif//

	if(copy_from_user(&curY_phyAddr, (unsigned int *)arg, sizeof(unsigned int)))
		return -EFAULT;

//	dprintk(" Overlay -> Q_Buffer :: buffer = 0x%x pos_reset:%d start_en:%d\n", curY_phyAddr, pos_reset, start_en);

#ifdef VC_OVERLAY_PROFILE
	if(toggle_int)
	{
		(HwGPIOD->GPEN |= Hw24);	(HwGPIOD->GPDAT |= Hw24);
		toggle_int = 0;
	}
	else
	{
		(HwGPIOD->GPEN |= Hw24);	(HwGPIOD->GPDAT &= ~Hw24);
		toggle_int = 1;
	}
#endif

	//in case position reset in streamming.
	if(pos_reset)
	{
		pos_reset = 0;
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
			// position
			BITCSET (pLCDC1->LI0P, 0xffff<< 16, (overlay_cfg.sy)  << 16); // position y
			BITCSET (pLCDC1->LI0P, 0xffff<<  0, (overlay_cfg.sx)  <<  0); // position x
			
			// size
			BITCSET (pLCDC1->LI0S, 0xffff<< 16, (overlay_cfg.height) << 16); // height
			BITCSET (pLCDC1->LI0S, 0xffff<<  0, (overlay_cfg.width) <<	0); // width
		#else
			VIOC_WMIX_SetPosition(pWMIXBase1, 2,  overlay_cfg.sx, overlay_cfg.sy);
			VIOC_RDMA_SetImageSize(pRDMABase6, overlay_cfg.width, overlay_cfg.height);
		#endif//
		tccxxx_overlay_fmt_set(overlay_fmt.fmt.pix.pixelformat);		
	}

	// image address
	curU_phyAddr = GET_ADDR_YUV42X_spU(curY_phyAddr, overlay_cfg.width, overlay_cfg.height); 
	curV_phyAddr = GET_ADDR_YUV420_spV(curU_phyAddr, overlay_cfg.width, overlay_cfg.height);

#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITCSET (pLCDC1->LI0BA0, 0xFFFFFFFF,  curY_phyAddr); // address0
		BITCSET (pLCDC1->LI0BA1, 0xFFFFFFFF,  curU_phyAddr); // address1	
		BITCSET (pLCDC1->LI0BA2, 0xFFFFFFFF,  curV_phyAddr); // address2
#else
		VIOC_RDMA_SetImageBase(pRDMABase6, curY_phyAddr, curU_phyAddr,curV_phyAddr );
#endif//
	
	if(!start_en)
	{
		tccxxx_overlay_common_enable();
		tccxxx_overlay_fmt_set(overlay_fmt.fmt.pix.pixelformat);
		
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITCLR (pLCDC1->LI0C, HwLIC_INTL);				 // not interlace format
		BITSET(pLCDC1->LCTRL, Hw0);	
#else
		VIOC_RDMA_SetImageIntl(pRDMABase6 , 0);
		VIOC_RDMA_SetImageEnable(pRDMABase6 );
#endif//
		start_en = 1;
	}
	
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
	if(!(pLCDC1->LI0C & Hw28)){
		BITCSET (pLCDC1->LI0C, 0x1<<28, 	(1)  << 28); // Enable Image
	}
#else
	VIOC_RDMA_GetImageEnable(pRDMABase6, &enable);
	if(!enable)
		VIOC_RDMA_SetImageEnable(pRDMABase6);
#endif//

	tccxxx_overlay_check_priority();
	
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
	#if !defined(CONFIG_ARCH_TCC92XX)
	BITCSET (pLCDC1->LI0C, HwLCT_RU, HwLCT_RU); //Image update
	#endif
#else
//	VIOC_WMIX_SetUpdate(pWMIXBase1);
	VIOC_RDMA_SetImageUpdate(pRDMABase6);
#endif
	return 0;
}


static int tccxxx_overlay_disable(void)
{
	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITSCLR (pLCDC1->LI0C, 0x1<<28, 	(1)  << 28); // Disable Image
		#if !defined(CONFIG_ARCH_TCC92XX)
		BITCSET (pLCDC1->LI0C, HwLCT_RU, HwLCT_RU); //Image update
		#endif
	#else
		VIOC_RDMA_SetImageDisable(pRDMABase6);
	#endif//

	wait_restart = 1;

	return 0;
}


static int tccxxx_overlay_set_pos(overlay_config_t * arg )
{
	struct lcd_panel *panel = tccfb_get_panel();
	unsigned int screen_width, screen_height;

	overlay_config_t pos;

	
	if(copy_from_user(&pos, (overlay_config_t *)arg, sizeof(overlay_config_t)))
		return -EFAULT;

	if(!start_en)
	{
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITSCLR (pLCDC1->LI0C, 0x1<<28, 	(1)  << 28); // Disable Image
		#else
		VIOC_RDMA_SetImageDisable(pRDMABase6);
		#endif//
		wait_restart = 1;
	}

	overlay_cfg.sx		=	pos.sx; 	
	overlay_cfg.sy		=	pos.sy; 	
	overlay_cfg.width	=	pos.width;

	if(overlay_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB565 || overlay_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUV422P)
		overlay_cfg.width	=	((overlay_cfg.width+3) >> 2)<<2;
	else
		overlay_cfg.width	=	((overlay_cfg.width+15) >> 4)<<4;
	
	overlay_cfg.height	=	pos.height;

    screen_width      = panel->xres;
    screen_height     = panel->yres;
#if defined(CONFIG_TCC_HDMI_UI_SIZE_1280_720)
    if(tcc_display_data.resolution == 1)
    {
        screen_width      = 720;
        screen_height     = 576;
    }
    else if(tcc_display_data.resolution == 2)
    {
        screen_width      = 800;
        screen_height     = 480;
    }
#endif

	if(overlay_cfg.sx + overlay_cfg.width > screen_width)
	{
		if(overlay_cfg.width > screen_width)
		{
			overlay_cfg.sx = 0;
			overlay_cfg.width = screen_width;
		}
		else
		{		
			overlay_cfg.sx = (screen_width - overlay_cfg.width)/2;			
		}
	}

	if(overlay_cfg.sy + overlay_cfg.height > screen_height)
	{
		if(overlay_cfg.height > screen_height)
		{
			overlay_cfg.sy = 0;
			overlay_cfg.height = screen_height;
		}
		else
		{		
			overlay_cfg.sy = (screen_height - overlay_cfg.height)/2;			
		}
	}
	
	dprintk(" Overlay -> Set Position adjust :: (%d,%d) | (%d,%d) \n", overlay_cfg.sx, overlay_cfg.sy, overlay_cfg.width, overlay_cfg.height);

	//in case position reset in streamming.
	if(start_en)
	{
		pos_reset = 1;
		return 0;
	}

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		// position
		BITCSET (pLCDC1->LI0P, 0xffff<< 16, (overlay_cfg.sy)  << 16); // position y
		BITCSET (pLCDC1->LI0P, 0xffff<<  0, (overlay_cfg.sx)  <<  0); // position x
		
		// size
		BITCSET (pLCDC1->LI0S, 0xffff<< 16, (overlay_cfg.height) << 16); // height
		BITCSET (pLCDC1->LI0S, 0xffff<<  0, (overlay_cfg.width) <<	0); // width
	#else
		VIOC_WMIX_SetPosition(pWMIXBase1, 2,  overlay_cfg.sx, overlay_cfg.sy);
		VIOC_RDMA_SetImageSize(pRDMABase6, overlay_cfg.width, overlay_cfg.height);
	#endif//

	tccxxx_overlay_fmt_set(overlay_fmt.fmt.pix.pixelformat);

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		#if !defined(CONFIG_ARCH_TCC92XX)
			BITCSET (pLCDC1->LI0C, HwLCT_RU, HwLCT_RU); //Image update
		#endif
	#else
		VIOC_WMIX_SetUpdate(pWMIXBase1);
		VIOC_RDMA_SetImageUpdate(pRDMABase6);
	#endif//

	return 0;
}

static int tccxxx_overlay_set_configure(overlay_config_t* arg)
{
	unsigned int screen_width, screen_height;
	struct lcd_panel *panel = tccfb_get_panel();
	overlay_config_t config;

	if(copy_from_user(&config, (overlay_config_t *)arg, sizeof(overlay_config_t)))
		return -EFAULT;

	overlay_fmt.fmt.pix.width 	 	= config.width;
	overlay_fmt.fmt.pix.height 		= config.height ;
	overlay_fmt.fmt.pix.pixelformat	= config.format;
	dprintk(" Overlay -> S_FMT :: size(%d,%d), format = 0x%x(RGB565-0x%x, YUV420-0x%x, YUV420inter-0x%x) \n", overlay_fmt.fmt.pix.width, overlay_fmt.fmt.pix.height, overlay_fmt.fmt.pix.pixelformat, V4L2_PIX_FMT_RGB565,V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_NV12);

	overlay_cfg.sx		=	config.sx;	
	overlay_cfg.sy		=	config.sy;		
	overlay_cfg.width	=	overlay_fmt.fmt.pix.width;	
	if(overlay_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB565 || overlay_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUV422P)
		overlay_cfg.width	=	((overlay_cfg.width+3) >> 2)<<2;
	else
		overlay_cfg.width	=	((overlay_cfg.width+15) >> 4)<<4;
	overlay_cfg.height	=	overlay_fmt.fmt.pix.height;

    screen_width      = panel->xres;
    screen_height     = panel->yres;
#if defined(CONFIG_TCC_HDMI_UI_SIZE_1280_720)
    if(tcc_display_data.resolution == 1)
    {
        screen_width      = 720;
        screen_height     = 576;
    }
    else if(tcc_display_data.resolution == 2)
    {
        screen_width      = 800;
        screen_height     = 480;
    }
#endif

	if(overlay_cfg.sx + overlay_cfg.width > screen_width)
	{
		if(overlay_cfg.width > screen_width)
		{
			overlay_cfg.sx = 0;
			overlay_cfg.width = screen_width;
		}
		else
		{		
			overlay_cfg.sx = (screen_width - overlay_cfg.width)/2;			
		}
	}

	if(overlay_cfg.sy + overlay_cfg.height > screen_height)
	{
		if(overlay_cfg.height > screen_height)
		{
			overlay_cfg.sy = 0;
			overlay_cfg.height = screen_height;
		}
		else
		{		
			overlay_cfg.sy = (screen_height - overlay_cfg.height)/2;			
		}
	}

	dprintk(" Overlay -> S_FMT :: Real => size(%d,%d ~ %d,%d) \n", overlay_cfg.sx, overlay_cfg.sy, overlay_cfg.width, overlay_cfg.height);

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		// position
		BITCSET (pLCDC1->LI0P, 0xffff<< 16, (overlay_cfg.sy)  << 16); // position y
		BITCSET (pLCDC1->LI0P, 0xffff<<  0, (overlay_cfg.sx)  <<  0); // position x
		
		// size
		BITCSET (pLCDC1->LI0S, 0xffff<< 16, (overlay_cfg.height) << 16); // height
		BITCSET (pLCDC1->LI0S, 0xffff<<  0, (overlay_cfg.width) <<	0); // width
	#else
		VIOC_WMIX_SetPosition(pWMIXBase1, 2,  overlay_cfg.sx, overlay_cfg.sy);
		VIOC_RDMA_SetImageSize(pRDMABase6, overlay_cfg.width, overlay_cfg.height);
	#endif//
	
	tccxxx_overlay_fmt_set(overlay_fmt.fmt.pix.pixelformat);

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITCLR(pLCDC1->LI0C, HwLIC_SRC);
		#if !defined(CONFIG_ARCH_TCC92XX)
		BITCSET (pLCDC1->LI0C, HwLCT_RU, HwLCT_RU); //Image update
		#endif
	#endif//

	return 0;
}

static int overlay_forbid;
static long tccxxx_overlay_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int intArg;

#if 0
	if(Output_SelectMode != OUTPUT_SELECT_NONE)
		return 0;
	//if(overlay_forbid && (cmd != OVERLAY_FORBID))
	//	return 0;
#endif//

	switch(cmd)
	{
		case OVERLAY_FORBID:
			if(copy_from_user(&intArg, (int *)arg, sizeof(int)))
				return -EFAULT;
			overlay_forbid = intArg;
			return 0;

		case OVERLAY_GET_POSITION:
			return tccxxx_overlay_get_pos((overlay_config_t*)arg);

		case OVERLAY_GET_SCREEN_INFO:
			return tccxxx_overlay_get_screenInfo((overlay_config_t*)arg);

		case OVERLAY_SET_POSITION:
			return tccxxx_overlay_set_pos((overlay_config_t*)arg);

		case OVERLAY_QUEUE_BUFFER:
			return tccxxx_overlay_q_buffer((unsigned int*)arg);

		case OVERLAY_SET_CONFIGURE:
			return tccxxx_overlay_set_configure((overlay_config_t*)arg);

		case OVERLAY_SET_DISABLE:
			return tccxxx_overlay_disable();


		default:
			dprintk(" Unsupported IOCTL(%d)!!!\n", cmd);      
			break;			
		}

		return 0;
	}

static int tccxxx_overlay_release(struct inode *inode, struct file *file)
{
	start_en = 0;
	wait_restart = 0;
	tcc_overlay_use--;
	dprintk(" ===========> tccxxx_overlay_release num:%d \n", tcc_overlay_use);

#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
	tccxxx_overlay_common_disable(0, pRDMABase6);
#else
	tccxxx_overlay_common_disable(0);
#endif

	clk_disable(overlay_lcdc_clk);

	return 0;
}

static int tccxxx_overlay_open(struct inode *inode, struct file *file)
{
	tcc_overlay_use++;
	clk_enable(overlay_lcdc_clk);

	if(tcc_overlay_use > 1)
	{
		start_en = 0;
		wait_restart = 0;
		tcc_overlay_use--;
		dprintk(" ===========> forced close num:%d \n", tcc_overlay_use);

#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
		tccxxx_overlay_common_disable(0, pRDMABase6);
#else
		tccxxx_overlay_common_disable(0);
#endif
		clk_disable(overlay_lcdc_clk);
	}

	dprintk(" ===========> tccxxx_overlay_open num:%d \n", tcc_overlay_use);

	return 0;	
}

static struct file_operations tcc_overlay_fops = 
{
	.owner          = THIS_MODULE,
	.poll           = tccxxx_overlay_poll,
	.unlocked_ioctl = tccxxx_overlay_ioctl,
	.mmap           = tccxxx_overlay_mmap,
	.open           = tccxxx_overlay_open,
	.release        = tccxxx_overlay_release,
};

#ifdef PLATFORM_DEVICE
static struct miscdevice overlay_misc_device =
{
    DEV_MINOR,
    DEVICE_NAME,
    &tcc_overlay_fops,
};

extern unsigned int tca_get_lcd_lcdc_num(viod);
static int __init tcc_overlay_probe(struct platform_device *pdev)
{
	unsigned int lcd_lcdc_num;
	lcd_lcdc_num = tca_get_lcd_lcdc_num();

	overlay_lcdc_clk = clk_get(0, "lcdc1");
	BUG_ON(overlay_lcdc_clk == NULL);

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		pLCDC1 = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
		#ifdef CONFIG_ARCH_TCC92XX
		pLCDC1_CH0 = (volatile PLCDC_CHANNEL)tcc_p2v(pLCDC1->LI0C);
		#else
		pLCDC1_CH0 = (volatile PLCDC_CHANNEL)tcc_p2v(HwLCDC1_CH_BASE(0));
		#endif//

	#else
		if(lcd_lcdc_num)
		{
			pWMIXBase1 = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX1);
			pRDMABase6 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA06);
		}
		else
		{
			pWMIXBase1 = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX0);
			pRDMABase6 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA02);
		}
	#endif//

    if (misc_register(&overlay_misc_device))
    {
        dprintk(KERN_WARNING "OVERLAY: Couldn't register device %d.\n", DEV_MINOR);
        return -EBUSY;
    }

	return 0;
}

static int tcc_overlay_remove(struct platform_device *pdev)
{
    misc_deregister(&overlay_misc_device);

	return 0;
}

#ifdef CONFIG_PM
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
static volatile LCDC_CHANNEL LCDC1_CH0_BackUp;
#else
static VIOC_RDMA RDMA_BackUp;
#endif//
static int tcc_overlay_suspend(struct platform_device *pdev, pm_message_t state)
{
	if(tcc_overlay_use != 0)
	{	
		printk("tcc_overlay_suspend %d opened\n", tcc_overlay_use);
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		LCDC1_CH0_BackUp = *pLCDC1_CH0;
		#else
		RDMA_BackUp = *pRDMABase6;
		#endif//
		clk_disable(overlay_lcdc_clk);
	}
	
	return 0;
}

static int tcc_overlay_resume(struct platform_device *pdev)
{
	if(tcc_overlay_use != 0)
	{	
		printk("tcc_overlay_resume %d opened\n", tcc_overlay_use);
		
		clk_enable(overlay_lcdc_clk);

		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		*pLCDC1_CH0 = LCDC1_CH0_BackUp;
		#else
		 *pRDMABase6 = RDMA_BackUp;
		#endif//		
	}
	
	return 0;
}

#else //CONFIG_PM
#define tcc_overlay_suspend NULL
#define tcc_overlay_resume NULL
#endif //CONFIG_PM

static struct platform_device tcc_overlay_device = {
	.name	= "tcc_overlay",
	.dev	= {
		.release 	= NULL,
	},
	.id	= 0,
};

static struct platform_driver tcc_overlay_driver = {
	.driver         = {
	     .name   = "tcc_overlay",
	     .owner  = THIS_MODULE,
	},
	.probe          = tcc_overlay_probe,
	.remove         = tcc_overlay_remove,
	.suspend        = tcc_overlay_suspend,
	.resume         = tcc_overlay_resume,
};
#endif


static void __exit
tccxxx_overlay_cleanup(void)
{
#ifdef PLATFORM_DEVICE
	platform_driver_unregister(&tcc_overlay_driver);
	platform_device_unregister(&tcc_overlay_device);
#else
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
#endif

	dprintk(" ===========> tccxxx_overlay_cleanup \n");
	
	return;
}

static char banner[] __initdata = KERN_INFO "TCC Overlay driver initializing\n";

#ifndef PLATFORM_DEVICE
static struct class *overlay_class;
#endif

static int __init 
tccxxx_overlay_init(void)
{
	printk(banner);

#ifdef PLATFORM_DEVICE
	platform_device_register(&tcc_overlay_device);
	platform_driver_register(&tcc_overlay_driver);
#else
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_overlay_fops);

	overlay_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(overlay_class,NULL,MKDEV(MAJOR_ID,MINOR_ID),NULL,DEVICE_NAME);

	overlay_lcdc_clk = clk_get(0, "lcdc1");
	BUG_ON(overlay_lcdc_clk == NULL);
	pLCDC1	= (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);	
#endif

	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCC Video for Linux overlay driver");
MODULE_LICENSE("GPL");


module_init(tccxxx_overlay_init);
module_exit(tccxxx_overlay_cleanup);

