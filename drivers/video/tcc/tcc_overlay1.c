/*
 * drivers/video/Tcc_overlay1.c
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
#include <linux/mutex.h>

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

#define dprintk(msg...)	if (debug) { printk( "tcc_overlay1: " msg); }

#define PLATFORM_DEVICE

#define DEVICE_NAME			"overlay1"
#ifdef PLATFORM_DEVICE
#define DEV_MINOR	203
#else
#define MAJOR_ID	203
#define MINOR_ID	1
#endif

//#define OVERLAY_CNT_DEFAULT 2

static struct clk *overlay1_lcdc_clk;

static overlay_config_t overlay1_cfg;
static struct v4l2_format overlay1_fmt;

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
static PLCDC		pLCDC1, pLCDC0;
static volatile PLCDC_CHANNEL pLCDC1_CH1;
#else
static VIOC_WMIX *pWMIXBase1;
static VIOC_WMIX *pWMIXBaseExtend;
static VIOC_RDMA *pRDMABase5;
#endif//

static unsigned char start_en = 0;
static unsigned char wait_restart = 0;
static unsigned char pos_reset = 0;

unsigned char tcc_overlay1_use = 0;

//#define VC_OVERLAY_PROFILE // CAM_28pin, GPIO_D24
#ifdef VC_OVERLAY_PROFILE
static unsigned char toggle_int = 0;
#endif

static DEFINE_MUTEX(overlay1_mutex);


extern OUTPUT_SELECT_MODE	Output_SelectMode;
extern void tccxxx_overlay_common_enable(void);
#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
extern void tccxxx_overlay_common_disable(int channel, VIOC_RDMA *pRdmaBase);
#else
extern void tccxxx_overlay_common_disable(int channel);
#endif
extern void tccxxx_overlay_check_priority(void);
unsigned char tccxxx_overlay1_use(void)
{
	return tcc_overlay1_use;
}
EXPORT_SYMBOL(tccxxx_overlay1_use);

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int tccxxx_overlay1_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR  "overlay1: this address is not allowed \n");
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


DECLARE_WAIT_QUEUE_HEAD(overlay1_wait);

static unsigned int tccxxx_overlay1_poll(struct file *file, struct poll_table_struct *wait)
{
	dprintk(" tccxxx_overlay1_poll wait[%d][%d]!!!\n", (unsigned)wait, (unsigned)&overlay1_wait);
	poll_wait(file, &(overlay1_wait), wait);	
	dprintk(" tccxxx_overlay1_poll finish[%d][%d]!!!\n", (unsigned)wait, (unsigned)&overlay1_wait);	
	return POLLIN;
}

static int tccxxx_overlay1_get_pos(overlay_config_t * arg )
{
	overlay_config_t pos;

	pos.sx 		= overlay1_cfg.sx;
	pos.sy 		= overlay1_cfg.sy;
	pos.width	= overlay1_cfg.width;
	pos.height	= overlay1_cfg.height;
	dprintk(" Overlay -> Get Position :: (%d,%d) | (%d,%d) \n", overlay1_cfg.sx, overlay1_cfg.sy, overlay1_cfg.width, overlay1_cfg.height);
	
	if(copy_to_user((overlay_config_t *)arg, &pos, sizeof(overlay_config_t)))
		return -EFAULT;

	return 0;
}

static int tccxxx_overlay1_get_screenInfo(overlay_config_t * arg )
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

void tccxxx_overlay1_fmt_set(unsigned int fmt)
{
	dprintk(" Overlay -> S_FMT :: format = 0x%x(RGB565-0x%x, YUV420-0x%x, YUV420inter-0x%x) \n", fmt, V4L2_PIX_FMT_RGB565,V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_NV12);
	#if  defined (CONFIG_TCC_LCDC_CONTROLLER)
	if(fmt == V4L2_PIX_FMT_RGB565)
	{
		BITCSET (pLCDC1->LI1C, 0x1f<< 0,	(IMG_RGB565_FMT) <<  0); // format
		BITCSET (pLCDC1->LI1O, 0x0000FFFF,	overlay1_cfg.width * 2); // format
		BITCSET (pLCDC1->LI1C, 0x1<<  8,	(0)  <<  8); // y2r converter enable
	}
	else if(fmt == V4L2_PIX_FMT_NV12)
	{
		BITCSET (pLCDC1->LI1C, 0x1f<< 0,	(IMG_RGBYUV420I_FMT) <<  0); // format
		BITCSET (pLCDC1->LI1O, 0xFFFFFFFF,	((overlay1_cfg.width)<<16) | (overlay1_cfg.width)); // format 			
		BITCSET (pLCDC1->LI1C, 0x1<<  8,	(IMG_Y2R)  <<  8); // y2r converter enable
	}
	else if(fmt == V4L2_PIX_FMT_YUV422P)
	{
		BITCSET (pLCDC1->LI1C, 0x1f<< 0,	(IMG_RGBYUV422SQ_FMT) <<  0); // format
		BITCSET (pLCDC1->LI1O, 0x0000FFFF,	overlay1_cfg.width * 2); // format
		BITCSET (pLCDC1->LI1C, 0x1<<  8,	(IMG_Y2R)  <<  8); // y2r converter enable
	}
	else
	{
		BITCSET (pLCDC1->LI1C, 0x1f<< 0,	(IMG_RGBYUV420_FMT) <<	0); // format
		BITCSET (pLCDC1->LI1O, 0xFFFFFFFF,	((overlay1_cfg.width/2)<<16) | (overlay1_cfg.width)); // format				
		BITCSET (pLCDC1->LI1C, 0x1<<  8,	(IMG_Y2R)  <<  8); // y2r converter enable
	}
	#else

	if(fmt == V4L2_PIX_FMT_RGB565)
	{
		VIOC_RDMA_SetImageFormat(pRDMABase5, VIOC_IMG_FMT_RGB565);
		VIOC_RDMA_SetImageOffset(pRDMABase5, VIOC_IMG_FMT_RGB565, overlay1_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase5, 0);
	}
	else if(fmt == V4L2_PIX_FMT_NV12)
	{
		VIOC_RDMA_SetImageFormat(pRDMABase5, VIOC_IMG_FMT_YUV420IL0);
		VIOC_RDMA_SetImageOffset(pRDMABase5, VIOC_IMG_FMT_YUV420IL0, overlay1_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase5, 1);
	}
	else if(fmt == V4L2_PIX_FMT_YUV422P)
	{
		VIOC_RDMA_SetImageFormat(pRDMABase5, VIOC_IMG_FMT_YUYV);
		VIOC_RDMA_SetImageOffset(pRDMABase5, VIOC_IMG_FMT_YUYV, overlay1_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase5, 1);
	}
	else
	{
		VIOC_RDMA_SetImageFormat(pRDMABase5, VIOC_IMG_FMT_YUV420SEP);
		VIOC_RDMA_SetImageOffset(pRDMABase5, VIOC_IMG_FMT_YUV420SEP, overlay1_cfg.width);
		VIOC_RDMA_SetImageY2REnable(pRDMABase5, 1);	
	}	
	#endif//
}

int tccxxx_overlay1_q_buffer(unsigned int* arg )
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
			BITCSET (pLCDC1->LI1P, 0xffff<< 16, (overlay1_cfg.sy)  << 16); // position y
			BITCSET (pLCDC1->LI1P, 0xffff<<  0, (overlay1_cfg.sx)  <<  0); // position x
			
			// size
			BITCSET (pLCDC1->LI1S, 0xffff<< 16, (overlay1_cfg.height) << 16); // height
			BITCSET (pLCDC1->LI1S, 0xffff<<  0, (overlay1_cfg.width) <<	0); // width
		#else
			VIOC_WMIX_SetPosition(pWMIXBase1, 2,  overlay1_cfg.sx, overlay1_cfg.sy);
			VIOC_RDMA_SetImageSize(pRDMABase5, overlay1_cfg.width, overlay1_cfg.height);
		#endif//
		
		tccxxx_overlay1_fmt_set(overlay1_fmt.fmt.pix.pixelformat);		
	}

	// image address
	curU_phyAddr = GET_ADDR_YUV42X_spU(curY_phyAddr, overlay1_cfg.width, overlay1_cfg.height); 
	curV_phyAddr = GET_ADDR_YUV420_spV(curU_phyAddr, overlay1_cfg.width, overlay1_cfg.height);

#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITCSET (pLCDC1->LI1BA0, 0xFFFFFFFF,  curY_phyAddr); // address0
		BITCSET (pLCDC1->LI1BA1, 0xFFFFFFFF,  curU_phyAddr); // address1	
		BITCSET (pLCDC1->LI1BA2, 0xFFFFFFFF,  curV_phyAddr); // address2
#else
		VIOC_RDMA_SetImageBase(pRDMABase5, curY_phyAddr, curU_phyAddr,curV_phyAddr );
#endif//
	
	if(!start_en)
	{
		tccxxx_overlay_common_enable();
		tccxxx_overlay1_fmt_set(overlay1_fmt.fmt.pix.pixelformat);
		
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITCLR (pLCDC1->LI1C, HwLIC_INTL);				 // not interlace format
		BITSET(pLCDC1->LCTRL, Hw0);	
#else
		VIOC_RDMA_SetImageIntl(pRDMABase5 , 0);
		VIOC_RDMA_SetImageEnable(pRDMABase5);
#endif//
		start_en = 1;
	}
	
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		if(!(pLCDC1->LI1C & Hw28))
			BITCSET (pLCDC1->LI1C, 0x1<<28, 	(1)  << 28); // Enable Image
#else
	VIOC_RDMA_GetImageEnable(pRDMABase5, &enable);
	if(!enable)
		VIOC_RDMA_SetImageEnable(pRDMABase5);
#endif//

	tccxxx_overlay_check_priority();
	
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		#if !defined(CONFIG_ARCH_TCC92XX)
			BITCSET (pLCDC1->LI1C, HwLCT_RU, HwLCT_RU); //Image update
		#endif
#else
//	VIOC_WMIX_SetUpdate(pWMIXBase1);
	VIOC_RDMA_SetImageUpdate(pRDMABase5);
#endif

	return 0;
}


static int tccxxx_overlay1_disable(void)
{
	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITSCLR (pLCDC1->LI1C, 0x1<<28, 	(1)  << 28); // Disable Image

		#if !defined(CONFIG_ARCH_TCC92XX)
		BITCSET (pLCDC1->LI1C, HwLCT_RU, HwLCT_RU); //Image update
		#endif
	#else
		VIOC_RDMA_SetImageDisable(pRDMABase5);
	#endif//

	wait_restart = 1;

	return 0;
}


static int tccxxx_overlay1_set_pos(overlay_config_t * arg )
{
	struct lcd_panel *panel = tccfb_get_panel();
	unsigned int screen_width, screen_height;

	overlay_config_t pos;

	
	if(copy_from_user(&pos, (overlay_config_t *)arg, sizeof(overlay_config_t)))
		return -EFAULT;

	if(!start_en)
	{
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITSCLR (pLCDC1->LI1C, 0x1<<28, 	(1)  << 28); // Disable Image
		#else
			VIOC_RDMA_SetImageDisable(pRDMABase5);
		#endif//
		wait_restart = 1;
	}

	overlay1_cfg.sx		=	pos.sx; 	
	overlay1_cfg.sy		=	pos.sy; 	
	overlay1_cfg.width	=	pos.width;

	if(overlay1_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB565 || overlay1_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUV422P)
		overlay1_cfg.width	=	((overlay1_cfg.width+3) >> 2)<<2;
	else
		overlay1_cfg.width	=	((overlay1_cfg.width+15) >> 4)<<4;
	
	overlay1_cfg.height	=	pos.height;

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

	if(overlay1_cfg.sx + overlay1_cfg.width > screen_width)
	{
		if(overlay1_cfg.width > screen_width)
		{
			overlay1_cfg.sx = 0;
			overlay1_cfg.width = screen_width;
		}
		else
		{		
			overlay1_cfg.sx = (screen_width - overlay1_cfg.width)/2;			
		}
	}

	if(overlay1_cfg.sy + overlay1_cfg.height > screen_height)
	{
		if(overlay1_cfg.height > screen_height)
		{
			overlay1_cfg.sy = 0;
			overlay1_cfg.height = screen_height;
		}
		else
		{		
			overlay1_cfg.sy = (screen_height - overlay1_cfg.height)/2;			
		}
	}
	
	dprintk(" Overlay -> Set Position adjust :: (%d,%d) | (%d,%d) \n", overlay1_cfg.sx, overlay1_cfg.sy, overlay1_cfg.width, overlay1_cfg.height);

	//in case position reset in streamming.
	if(start_en)
	{
		pos_reset = 1;
		return 0;
	}

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
	// position
	BITCSET (pLCDC1->LI1P, 0xffff<< 16, (overlay1_cfg.sy)  << 16); // position y
	BITCSET (pLCDC1->LI1P, 0xffff<<  0, (overlay1_cfg.sx)  <<  0); // position x

	// size
	BITCSET (pLCDC1->LI1S, 0xffff<< 16, (overlay1_cfg.height) << 16); // height
	BITCSET (pLCDC1->LI1S, 0xffff<<  0, (overlay1_cfg.width) <<  0); // width
	#else
		VIOC_WMIX_SetPosition(pWMIXBase1, 1,  overlay1_cfg.sx, overlay1_cfg.sy);
		VIOC_RDMA_SetImageSize(pRDMABase5, overlay1_cfg.width, overlay1_cfg.height);
	#endif//

	tccxxx_overlay1_fmt_set(overlay1_fmt.fmt.pix.pixelformat);

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		#if !defined(CONFIG_ARCH_TCC92XX)
		BITCSET (pLCDC1->LI1C, HwLCT_RU, HwLCT_RU); //Image update
		#endif
	#else
		VIOC_WMIX_SetUpdate(pWMIXBase1);
		VIOC_RDMA_SetImageUpdate(pRDMABase5);
	#endif//
	
	return 0;
}

static int tccxxx_overlay1_set_configure(overlay_config_t* arg)
{
	unsigned int screen_width, screen_height;
	struct lcd_panel *panel = tccfb_get_panel();
	overlay_config_t config;

	if(copy_from_user(&config, (overlay_config_t *)arg, sizeof(overlay_config_t)))
		return -EFAULT;

	overlay1_fmt.fmt.pix.width 	 	= config.width;
	overlay1_fmt.fmt.pix.height 		= config.height ;
	overlay1_fmt.fmt.pix.pixelformat	= config.format;
	dprintk(" Overlay -> S_FMT :: size(%d,%d), format = 0x%x(RGB565-0x%x, YUV420-0x%x, YUV420inter-0x%x) \n", overlay1_fmt.fmt.pix.width, overlay1_fmt.fmt.pix.height, overlay1_fmt.fmt.pix.pixelformat, V4L2_PIX_FMT_RGB565,V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_NV12);

	overlay1_cfg.sx		=	config.sx;	
	overlay1_cfg.sy		=	config.sy;		
	overlay1_cfg.width	=	overlay1_fmt.fmt.pix.width;	
	if(overlay1_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB565 || overlay1_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUV422P)
		overlay1_cfg.width	=	((overlay1_cfg.width+3) >> 2)<<2;
	else
		overlay1_cfg.width	=	((overlay1_cfg.width+15) >> 4)<<4;
	overlay1_cfg.height	=	overlay1_fmt.fmt.pix.height;

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

	if(overlay1_cfg.sx + overlay1_cfg.width > screen_width)
	{
		if(overlay1_cfg.width > screen_width)
		{
			overlay1_cfg.sx = 0;
			overlay1_cfg.width = screen_width;
		}
		else
		{		
			overlay1_cfg.sx = (screen_width - overlay1_cfg.width)/2;			
		}
	}

	if(overlay1_cfg.sy + overlay1_cfg.height > screen_height)
	{
		if(overlay1_cfg.height > screen_height)
		{
			overlay1_cfg.sy = 0;
			overlay1_cfg.height = screen_height;
		}
		else
		{		
			overlay1_cfg.sy = (screen_height - overlay1_cfg.height)/2;			
		}
	}

	dprintk(" Overlay -> S_FMT :: Real => size(%d,%d ~ %d,%d) \n", overlay1_cfg.sx, overlay1_cfg.sy, overlay1_cfg.width, overlay1_cfg.height);

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
	// position
	BITCSET (pLCDC1->LI1P, 0xffff<< 16, (overlay1_cfg.sy)  << 16); // position y
	BITCSET (pLCDC1->LI1P, 0xffff<<  0, (overlay1_cfg.sx)  <<  0); // position x

	// size
	BITCSET (pLCDC1->LI1S, 0xffff<< 16, (overlay1_cfg.height) << 16); // height
	BITCSET (pLCDC1->LI1S, 0xffff<<  0, (overlay1_cfg.width) <<  0); // width
	#else
		VIOC_WMIX_SetPosition(pWMIXBase1, 2,  overlay1_cfg.sx, overlay1_cfg.sy);
		VIOC_RDMA_SetImageSize(pRDMABase5, overlay1_cfg.width, overlay1_cfg.height);
	#endif//
	tccxxx_overlay1_fmt_set(overlay1_fmt.fmt.pix.pixelformat);

	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		BITCLR(pLCDC1->LI1C, HwLIC_SRC);
		#if !defined(CONFIG_ARCH_TCC92XX)
		BITCSET (pLCDC1->LI1C, HwLCT_RU, HwLCT_RU); //Image update
		#endif
	#endif//
	return 0;
}


static int tccxxx_overlay1_set_chroma(overlay_chroma_t* arg)
{
	overlay_chroma_t *config;

	config = (overlay_chroma_t*)arg;
	if(copy_from_user(config, (unsigned int *)arg, sizeof(*config)))
		return -EFAULT;
	
#if defined(CONFIG_TCC_LCDC_CONTROLLER)	
	BITCSET (pLCDC1->LI1C, 0x1<< 29, (config->enable) << 29); // chroma-keying
	
	BITCSET (pLCDC1->LI1KR, 0x00FF00FF, config->chromaR); // key
	BITCSET (pLCDC1->LI1KG, 0x00FF00FF, config->chromaG); // key
	BITCSET (pLCDC1->LI1KB, 0x00FF00FF, config->chromaB); // key

	#if !defined(CONFIG_ARCH_TCC92XX)
	BITCSET (pLCDC1->LI1C, 	HwLCT_RU, 	HwLCT_RU); //Image update
	#endif//	
#else
	VIOC_WMIX_SetChromaKey(pWMIXBase1, 2, config->enable, (config->chromaR & 0xFF), (config->chromaG & 0xFF), (config->chromaB & 0xFF), 
									((config->chromaR >> 16) & 0xFF), ((config->chromaG >> 16) & 0xFF), ((config->chromaB >> 16) & 0xFF));
#endif//

	//printk("lcdc 1  0x%x 0x%x 0x%x 0x%x \n", pLCDC1->LI1C, pLCDC1->LI1KR, pLCDC1->LI1KG, pLCDC1->LI1KB);

	#if !defined(CONFIG_ARCH_TCC92XX)
	if(Output_SelectMode != OUTPUT_SELECT_NONE)
	{
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)	
		pLCDC0	= (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

		BITCSET (pLCDC0->LI1C, 0x1<< 29, (config->enable) << 29); // chroma-keying
		BITCSET (pLCDC0->LI1KR, 0xFFFFFFFF, config->chromaR); // key
		BITCSET (pLCDC0->LI1KG, 0xFFFFFFFF, config->chromaG); // key
		BITCSET (pLCDC0->LI1KB, 0xFFFFFFFF, config->chromaB); // key

		BITCSET (pLCDC0->LI1C, 	HwLCT_RU, 	HwLCT_RU); //Image update
		#else
		VIOC_WMIX_SetChromaKey(pWMIXBaseExtend, 2, config->enable, (config->chromaR & 0xFF), (config->chromaG & 0xFF), (config->chromaB & 0xFF), 
										((config->chromaR >> 16) & 0xFF), ((config->chromaG >> 16) & 0xFF), ((config->chromaB >> 16) & 0xFF));
		#endif//
		
		//printk("lcdc 0   0x%x 0x%x 0x%x 0x%x \n", pLCDC0->LI1C, pLCDC0->LI1KR, pLCDC0->LI1KG, pLCDC0->LI1KB);
	}
	#endif//	CONFIG_ARCH_TCC93XX

	return 0;
}



static int overlay1_forbid;
static long tccxxx_overlay1_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int intArg;

	if((Output_SelectMode != OUTPUT_SELECT_NONE) &&(cmd !=OVERLAY_SET_CROMA))
		return 0;
	//if(overlay1_forbid && (cmd != OVERLAY_FORBID))
	//	return 0;

	switch(cmd)
	{
		case OVERLAY_FORBID:
			if(copy_from_user(&intArg, (int *)arg, sizeof(int)))
				return -EFAULT;
			overlay1_forbid = intArg;
			return 0;

		case OVERLAY_GET_POSITION:
			return tccxxx_overlay1_get_pos((overlay_config_t*)arg);

		case OVERLAY_GET_SCREEN_INFO:
			return tccxxx_overlay1_get_screenInfo((overlay_config_t*)arg);

		case OVERLAY_SET_POSITION:
			return tccxxx_overlay1_set_pos((overlay_config_t*)arg);

		case OVERLAY_QUEUE_BUFFER:
			return tccxxx_overlay1_q_buffer((unsigned int*)arg);

		case OVERLAY_SET_CONFIGURE:
			return tccxxx_overlay1_set_configure((overlay_config_t*)arg);

		case OVERLAY_SET_DISABLE:
			return tccxxx_overlay1_disable();

		case OVERLAY_SET_CROMA:
			return tccxxx_overlay1_set_chroma((overlay_chroma_t*)arg);

		default:
			dprintk(" Unsupported IOCTL!!!\n");      
			break;			
		}

		return 0;
	}

static int tccxxx_overlay1_release(struct inode *inode, struct file *file)
{
	mutex_lock(&overlay1_mutex);
	{
		start_en = 0;
		wait_restart = 0;
		tcc_overlay1_use--;
		dprintk(" ===========> tccxxx_overlay1_release num:%d \n", tcc_overlay1_use);

#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
		tccxxx_overlay_common_disable(1, pRDMABase5);
#else
		tccxxx_overlay_common_disable(1);
#endif
		clk_disable(overlay1_lcdc_clk);
	}
	mutex_unlock(&overlay1_mutex);

	return 0;
}

static int tccxxx_overlay1_open(struct inode *inode, struct file *file)
{
	mutex_lock(&overlay1_mutex);
	{
		tcc_overlay1_use++;
		clk_enable(overlay1_lcdc_clk);

		if(tcc_overlay1_use > 1)
		{
			start_en = 0;
			wait_restart = 0;
			tcc_overlay1_use--;
			dprintk(" ===========> forced close num:%d \n", tcc_overlay1_use);

#if  defined (CONFIG_TCC_VIOC_CONTROLLER)
			tccxxx_overlay_common_disable(1, pRDMABase5);
#else
			tccxxx_overlay_common_disable(1);
#endif
			clk_disable(overlay1_lcdc_clk);
		}
	}
	mutex_unlock(&overlay1_mutex);

	dprintk(" ===========> tccxxx_overlay1_open num:%d \n", tcc_overlay1_use);

	return 0;	
}

static struct file_operations tcc_overlay1_fops = 
{
	.owner          = THIS_MODULE,
	.poll           = tccxxx_overlay1_poll,
	.unlocked_ioctl = tccxxx_overlay1_ioctl,
	.mmap           = tccxxx_overlay1_mmap,
	.open           = tccxxx_overlay1_open,
	.release        = tccxxx_overlay1_release,
};

#ifdef PLATFORM_DEVICE
static struct miscdevice overlay1_misc_device =
{
    DEV_MINOR,
    DEVICE_NAME,
    &tcc_overlay1_fops,
};

extern unsigned int tca_get_lcd_lcdc_num(viod);
static int __init tcc_overlay1_probe(struct platform_device *pdev)
{
	unsigned int lcd_lcdc_num;
	lcd_lcdc_num = tca_get_lcd_lcdc_num();
	
	overlay1_lcdc_clk = clk_get(0, "lcdc1");
	BUG_ON(overlay1_lcdc_clk == NULL);
	#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		pLCDC1	= (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);	

		#ifdef CONFIG_ARCH_TCC92XX
		pLCDC1_CH1 = (volatile PLCDC_CHANNEL)tcc_p2v(pLCDC1->LI1C);
		#else
		pLCDC1_CH1 = (volatile PLCDC_CHANNEL)tcc_p2v(HwLCDC1_CH_BASE(1));
		#endif//

	#else
		if(lcd_lcdc_num)
		{
			pWMIXBase1 = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX1);
			pWMIXBaseExtend = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX0);
			pRDMABase5 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA05);
		}
		else
		{
			pWMIXBase1 = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX0);
			pWMIXBaseExtend = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX1);
			pRDMABase5 = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA01);
		}
	#endif//
    if (misc_register(&overlay1_misc_device))
    {
        dprintk(KERN_WARNING "OVERLAY: Couldn't register device %d.\n", DEV_MINOR);
        return -EBUSY;
    }

	return 0;
}

static int tcc_overlay1_remove(struct platform_device *pdev)
{
    misc_deregister(&overlay1_misc_device);

	return 0;
}

#ifdef CONFIG_PM
#if defined(CONFIG_TCC_LCDC_CONTROLLER)
static volatile LCDC_CHANNEL LCDC1_CH1_BackUp;
#else
static VIOC_RDMA RDMA_BackUp;
#endif//
static int tcc_overlay1_suspend(struct platform_device *pdev, pm_message_t state)
{
	if(tcc_overlay1_use != 0)
	{	
		printk("tcc_overlay1_suspend %d opened\n", tcc_overlay1_use);
		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		LCDC1_CH1_BackUp = *pLCDC1_CH1;
		#else
		RDMA_BackUp = *pRDMABase5;
		#endif//
		clk_disable(overlay1_lcdc_clk);
	}
	
	return 0;
}

static int tcc_overlay1_resume(struct platform_device *pdev)
{
	if(tcc_overlay1_use != 0)
	{	
		printk("tcc_overlay1_resume %d opened\n", tcc_overlay1_use);
		
		clk_enable(overlay1_lcdc_clk);

		#if defined(CONFIG_TCC_LCDC_CONTROLLER)
		*pLCDC1_CH1 = LCDC1_CH1_BackUp;
		#else
		 *pRDMABase5 = RDMA_BackUp;
		#endif//		
	}
	
	return 0;
}

#else //CONFIG_PM
#define tcc_overlay1_suspend NULL
#define tcc_overlay1_resume NULL
#endif //CONFIG_PM

static struct platform_device tcc_overlay1_device = {
	.name	= "tcc_overlay1",
	.dev	= {
		.release 	= NULL,
	},
	.id	= 0,
};

static struct platform_driver tcc_overlay1_driver = {
	.driver         = {
	     .name   = "tcc_overlay1",
	     .owner  = THIS_MODULE,
	},
	.probe          = tcc_overlay1_probe,
	.remove         = tcc_overlay1_remove,
	.suspend        = tcc_overlay1_suspend,
	.resume         = tcc_overlay1_resume,
};
#endif


static void __exit
tccxxx_overlay1_cleanup(void)
{
#ifdef PLATFORM_DEVICE
	platform_driver_unregister(&tcc_overlay1_driver);
	platform_device_unregister(&tcc_overlay1_device);
#else
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
#endif

	dprintk(" ===========> tccxxx_overlay1_cleanup \n");
	
	return;
}

static char banner[] __initdata = KERN_INFO "TCC Overlay driver initializing\n";

#ifndef PLATFORM_DEVICE
static struct class *overlay1_class;
#endif

static int __init 
tccxxx_overlay1_init(void)
{
	printk(banner);

#ifdef PLATFORM_DEVICE
	platform_device_register(&tcc_overlay1_device);
	platform_driver_register(&tcc_overlay1_driver);
#else
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_overlay1_fops);

	overlay1_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(overlay1_class,NULL,MKDEV(MAJOR_ID,MINOR_ID),NULL,DEVICE_NAME);

	overlay1_lcdc_clk = clk_get(0, "lcdc1");
	BUG_ON(overlay1_lcdc_clk == NULL);	
	pLCDC1	= (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);	
#endif

	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCC Video for Linux overlay1 driver");
MODULE_LICENSE("GPL");


module_init(tccxxx_overlay1_init);
module_exit(tccxxx_overlay1_cleanup);

