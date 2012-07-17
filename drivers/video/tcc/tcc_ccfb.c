/*
 * drivers/video/tcc_ccfb.c
 *
 * Copyright (C) 2011 Telechips, Inc. 
 *
 * This file used for DTV subtitle display for TCC93xx/TCC88xx. Not for TCC92xx. 
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
#include <mach/tcc_ccfb_ioctl.h>
#include <mach/tcc_fb.h>
#include <mach/tccfb_ioctrl.h>
#ifdef CONFIG_TCC_VIOC_CONTROLLER
#include <mach/vioc_outcfg.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_disp.h>
#include <mach/vioc_global.h>
#endif//

/****************************************************************************
DEFINITION
****************************************************************************/
#define dprintk(msg...)	//printk(msg)

#define DEVICE_NAME		"ccfb"
#define DEV_MINOR		204
#define MAX_LCDC_NUM	2

#define MUTEX_LOCK		//mutex_lock(&g_ccfb_mutex);
#define MUTEX_UNLOCK	//mutex_lunock(&g_ccfb_mutex);

/****************************************************************************
DEFINITION OF TYPE
****************************************************************************/
typedef enum
{
	CCFB_STATE_CLOSED,
	CCFB_STATE_OPENED,
	CCFB_STATE_RUNNING,
	CCFB_STATE_PAUSE,

	CCFB_STATE_MAX
}ccfb_state_t;

typedef struct
{
	ccfb_state_t			cur_state;
	int32_t				act_lcdc_idx;
	VIOC_RDMA			*pCurLcdc;
	VIOC_WMIX			*pCurWMix;
	struct clk 			*pLcdcClk[MAX_LCDC_NUM];	/* TCCxx machine has two LCD controller */
}ccfb_dev_config_t;


/****************************************************************************
DEFINITION OF EXTERNAL VARIABLES
****************************************************************************/


/****************************************************************************
DEFINITION OF STATIC VARIABLES
****************************************************************************/
static ccfb_dev_config_t	g_dev_cfg;
static DEFINE_MUTEX(g_ccfb_mutex);
static ccfb_config_t g_ccfg_cfg;
static unsigned int g_ovp_value = 0;


/****************************************************************************
DEFINITION OF EXTERNAL FUNCTIONS
****************************************************************************/
extern int range_is_allowed(unsigned long pfn, unsigned long size);


/****************************************************************************
DEFINITION OF LOCAL FUNCTIONS
****************************************************************************/
static ccfb_dev_config_t* get_ccfb_dev(void)
{
	return &g_dev_cfg;
}

static void init_ccfb_dev(void)
{
	ccfb_dev_config_t *dev = get_ccfb_dev();
	
	dev->cur_state = CCFB_STATE_CLOSED;
	dev->act_lcdc_idx = -1;
	dev->pCurLcdc = NULL;
	dev->pCurWMix = NULL;
	dev->pLcdcClk[0] = NULL;
	dev->pLcdcClk[1] = NULL;
}

static int tccxxx_ccfb_mmap(struct file *file, struct vm_area_struct *vma)
{
	dprintk("==> %s\n", __func__);
	
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR  "ccfb: this address is not allowed \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff, 
						vma->vm_end - vma->vm_start, 
						vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	vma->vm_ops	= NULL;
	vma->vm_flags 	|= VM_IO;
	vma->vm_flags 	|= VM_RESERVED;
	
	return 0;
}

static int tccxxx_ccfb_act_clock(ccfb_dev_config_t *dev, int lcdc_num)
{
	char *pDevName[2]={"lcdc0", "lcdc1"};

	dprintk("==> %s (%d)\n", __func__, lcdc_num);

	if((dev == NULL) && (lcdc_num < 0) && (lcdc_num >= 2))
	{
		printk(KERN_ERR "Invalid parameters for lcdc(%d, %p)\n", lcdc_num, dev);
		return -EINVAL;
	}

	dev->pLcdcClk[lcdc_num] = clk_get(0, pDevName[lcdc_num]);
	if (IS_ERR(dev->pLcdcClk[lcdc_num])){
		printk(KERN_ERR "%s clock get fail.\n", pDevName[lcdc_num]);
		return -EIO;
	}
	clk_enable(dev->pLcdcClk[lcdc_num]);	

	dev->act_lcdc_idx = lcdc_num;

	if(lcdc_num == 0){
		dev->pCurLcdc = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA01);
		dev->pCurWMix = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX0);
	}else{
		dev->pCurLcdc = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA05);
		dev->pCurWMix = (VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX1);
	}
	
	return 0;
}

#if 0
static int tccxxx_ccfb_deact_clock(ccfb_dev_config_t *dev)
{
	int i;

	dprintk("==> %s\n", __func__);

	for(i = 0 ; i<MAX_LCDC_NUM ; i++)
	{
		if(dev->pLcdcClk[i] != NULL){
			clk_disable(dev->pLcdcClk[i]);
			dev->pLcdcClk[i] = NULL;
		}
	}
	
	return 0;
}
#endif    /* End of 0 */

static int tccxxx_ccfb_lcdc_enable(ccfb_dev_config_t *dev)
{
	dprintk("==> %s\n", __func__);
	
	if(dev->pCurLcdc){
		VIOC_RDMA_SetImageEnable	(dev->pCurLcdc);
	}
	
	return 0;
}

static int tccxxx_ccfb_lcdc_disable(ccfb_dev_config_t *dev)
{
	dprintk("==> %s\n", __func__);
	
	if(dev->pCurLcdc){
		VIOC_RDMA_SetImageDisable	(dev->pCurLcdc);
	}
	
	return 0;
}

static int tccxxx_ccfb_get_config(ccfb_dev_config_t *dev, void *arg)
{
	ccfb_config_t cfg;
	struct lcd_panel *panel = tccfb_get_panel();

	cfg.res.disp_w = panel->xres;
	cfg.res.disp_h = panel->yres;

	//printk("==> %s - %dx%d (%d)\n", __func__, panel->xres, panel->yres, tcc_display_data.resolution);
	
	if(copy_to_user((ccfb_config_t *)arg, &cfg, sizeof(ccfb_config_t)))
		return -EFAULT;
	
	return 0;
}

static int tccxxx_ccfb_set_config(ccfb_dev_config_t *dev, ccfb_config_t *arg)
{
	int ret = -ENODEV;
	ccfb_config_t cfg;

	dprintk("==> %s\n", __func__);
	
	if(copy_from_user((void*)&cfg, (void *)arg, sizeof(ccfb_config_t)))
		return -EFAULT;	

	ret = tccxxx_ccfb_act_clock(dev, cfg.curLcdc);
	if(ret == 0)
	{
		memcpy(&g_ccfg_cfg, &cfg, sizeof(ccfb_config_t));

		VIOC_WMIX_GetOverlayPriority(dev->pCurWMix, &g_ovp_value);
		VIOC_WMIX_SetOverlayPriority(dev->pCurWMix, 1);
		
		// position (full screen update)
		VIOC_WMIX_SetPosition(dev->pCurWMix, 1, cfg.res.disp_x, cfg.res.disp_y);

		// size
		VIOC_RDMA_SetImageSize(dev->pCurLcdc, cfg.res.disp_w, cfg.res.disp_h);

		// scale
		VIOC_RDMA_SetImageScale(dev->pCurLcdc, cfg.res.disp_m, cfg.res.disp_m);

		if(cfg.res.disp_fmt == CCFB_IMG_FMT_RGB565)	{
			VIOC_RDMA_SetImageFormat(dev->pCurLcdc, VIOC_IMG_FMT_RGB565);
			VIOC_RDMA_SetImageOffset(dev->pCurLcdc, VIOC_IMG_FMT_RGB565, cfg.res.mem_w);

			// Chroma-keying set
			if(cfg.res.chroma_en) {
				VIOC_WMIX_SetChromaKey(dev->pCurWMix, 1, cfg.res.chroma_en, cfg.res.chroma_R, cfg.res.chroma_G, cfg.res.chroma_B, 0xF8, 0xFC, 0xF8);
			}
		} else {
			// ARGB 32bit format		
			VIOC_RDMA_SetImageFormat(dev->pCurLcdc, VIOC_IMG_FMT_ARGB8888);
			VIOC_RDMA_SetImageOffset(dev->pCurLcdc, VIOC_IMG_FMT_ARGB8888, cfg.res.mem_w);

			// Y2R disable
			VIOC_RDMA_SetImageY2REnable(dev->pCurLcdc, 0);

			// Alpha enable
			VIOC_RDMA_SetImageAlphaSelect(dev->pCurLcdc, 1);	// ASEL set
			VIOC_RDMA_SetImageAlphaEnable(dev->pCurLcdc, 1);	// AEN set
		}

		// Ch disable
		VIOC_RDMA_SetImageDisable	(dev->pCurLcdc);
	}	
	
	return ret;
}

static int tccxxx_ccfb_disp_update(ccfb_dev_config_t *dev, unsigned int* arg)
{
	unsigned int cur_addr;

	dprintk("==> %s\n", __func__);
	
	if(copy_from_user((void*)&cur_addr, (void *)arg, sizeof(unsigned int)))
	{
		printk(KERN_ERR "error\n");
		return -EFAULT;
	}
	
	dprintk("updated address : 0x%08x\n", cur_addr);
#if 0
	if(dev->pUiLcdc->LIC & Hw28){
		printk("==>>> [%s] WARNING : UI is enabled.\n", __func__);
		BITCSET(dev->pUiLcdc->LIC, 0x0<<28, 0x0<<28);
	#if !defined(CONFIG_ARCH_TCC92XX)
		BITCSET (dev->pUiLcdc->LIC, HwLCT_RU, HwLCT_RU);
	#endif
	}

	// position (full screen update)
	//VIOC_WMIX_SetPosition(dev->pCurWMix, 1, g_ccfg_cfg.res.disp_x, g_ccfg_cfg.res.disp_y);
	VIOC_WMIX_SetPosition(dev->pCurWMix, 1, 10, 10);

	// size
	VIOC_RDMA_SetImageSize(dev->pCurLcdc, g_ccfg_cfg.res.disp_w, g_ccfg_cfg.res.disp_h);

	// scale
	VIOC_RDMA_SetImageScale(dev->pCurLcdc, 0, 0);

	// ARGB 32bit format		
	VIOC_RDMA_SetImageFormat(dev->pCurLcdc, VIOC_IMG_FMT_ARGB8888);
	VIOC_RDMA_SetImageOffset(dev->pCurLcdc, VIOC_IMG_FMT_ARGB8888, g_ccfg_cfg.res.mem_w);

	// Y2R disable
	VIOC_RDMA_SetImageY2REnable(dev->pCurLcdc, 0);

	// Chroma-keying disable
	//BITCSET (dev->pCurLcdc->LIC, 0x1<< 29, 0 << 29);

	// Alpha enable
	VIOC_RDMA_SetImageAlphaSelect(dev->pCurLcdc, 1);	// ASEL set
	VIOC_RDMA_SetImageAlphaEnable(dev->pCurLcdc, 1);	// AEN set

	// Ch disable
	VIOC_RDMA_SetImageEnable(dev->pCurLcdc);
#endif /* 0 */

	VIOC_WMIX_SetUpdate(dev->pCurWMix);

	VIOC_RDMA_SetImageBase(dev->pCurLcdc, cur_addr, 0, 0);
	VIOC_RDMA_SetImageEnable(dev->pCurLcdc);
	
	return 0;
}

static long tccxxx_ccfb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = -EPERM;
	ccfb_dev_config_t	*dev = get_ccfb_dev();

	dprintk("==> %s\n", __func__);
	
	switch(cmd)
	{
		case CCFB_GET_CONFIG:
			MUTEX_LOCK
			ret = tccxxx_ccfb_get_config(dev, (ccfb_config_t *)arg);
			MUTEX_UNLOCK
			break;
			
		case CCFB_SET_CONFIG:
			if(dev->cur_state != CCFB_STATE_CLOSED){
				MUTEX_LOCK
				ret = tccxxx_ccfb_set_config(dev, (ccfb_config_t *)arg);
				MUTEX_UNLOCK
				if(ret == 0) dev->cur_state = CCFB_STATE_PAUSE;
			}		
			break;
			
		case CCFB_DISP_UPDATE:
			if(dev->cur_state == CCFB_STATE_RUNNING){
				MUTEX_LOCK
				ret = tccxxx_ccfb_disp_update(dev, (unsigned int *)arg);
				MUTEX_UNLOCK
			}
			break;
		
		case CCFB_DISP_ENABLE:
			if(dev->cur_state == CCFB_STATE_PAUSE){
				MUTEX_LOCK
				/* enable setting is moved to update routine */
				//ret = tccxxx_ccfb_lcdc_enable(dev);
				dev->cur_state = CCFB_STATE_RUNNING;
				MUTEX_UNLOCK
			}
			break;
			
		case CCFB_DISP_DISABLE:
			if(dev->cur_state == CCFB_STATE_RUNNING){
				MUTEX_LOCK
				ret = tccxxx_ccfb_lcdc_disable(dev);
				dev->cur_state = CCFB_STATE_PAUSE;
				MUTEX_UNLOCK
			}
			break;

		default:
			printk("%s - Unsupported IOCTL!!!(0x%X)\n", __func__, cmd);
			ret = -EINVAL;
			break;
	}

	return ret;
}

static int tccxxx_ccfb_release(struct inode *inode, struct file *file)
{
	ccfb_dev_config_t *dev = get_ccfb_dev();

	dprintk("==> %s\n", __func__);

	if(dev->cur_state == CCFB_STATE_CLOSED)
	{
		printk("WARNING : ccfb already closed.\n");
		return 0;
	}

	if((dev->cur_state == CCFB_STATE_RUNNING)||(dev->cur_state == CCFB_STATE_PAUSE))
	{
		dprintk("==> [%s] ccfb is still running... ccfb close trying...\n", __func__);
		tccxxx_ccfb_lcdc_disable(dev);
		//tccxxx_ccfb_deact_clock(dev);
	}

	if(dev->pCurWMix != NULL){
		if(g_ccfg_cfg.res.chroma_en) {
			VIOC_WMIX_SetChromaKey(dev->pCurWMix, 1, 0, 0, 0, 0, 0xF8, 0xFC, 0xF8);
		}

		VIOC_WMIX_SetOverlayPriority(dev->pCurWMix, g_ovp_value);
	}
		
	memset(&g_ccfg_cfg, 0x0, sizeof(ccfb_config_t));

	dev->cur_state = CCFB_STATE_CLOSED;
	dev->act_lcdc_idx = -1;
	dev->pCurLcdc = NULL;
	//dev->pUiLcdc = NULL;
	dev->pLcdcClk[0] = NULL;
	dev->pLcdcClk[1] = NULL;
	
	return 0;
}

static int tccxxx_ccfb_open(struct inode *inode, struct file *file)
{
	ccfb_dev_config_t *dev = get_ccfb_dev();

	dprintk("==> %s\n", __func__);
	
	if(dev->cur_state != CCFB_STATE_CLOSED)
	{
		printk("WARNING : ccfb already opened.\n");
		return 0;
	}

	memset(&g_ccfg_cfg, 0x0, sizeof(ccfb_config_t));

	dev->cur_state = CCFB_STATE_OPENED;
	dev->act_lcdc_idx = -1;
	dev->pCurLcdc = NULL;
	//dev->pUiLcdc = NULL;
	dev->pLcdcClk[0] = NULL;
	dev->pLcdcClk[1] = NULL;
	
	return 0;
}

static struct file_operations tcc_ccfb_fops = 
{
	.owner          = THIS_MODULE,
	.unlocked_ioctl = tccxxx_ccfb_ioctl,
	.mmap           = tccxxx_ccfb_mmap,
	.open           = tccxxx_ccfb_open,
	.release        = tccxxx_ccfb_release,
};

static struct miscdevice ccfb_misc_device =
{
	DEV_MINOR,
	DEVICE_NAME,
	&tcc_ccfb_fops,
};

static int __init tcc_ccfb_probe(struct platform_device *pdev)
{
	dprintk("==> %s\n", __func__);

	init_ccfb_dev();
	
	if (misc_register(&ccfb_misc_device))
	{
		dprintk(KERN_WARNING "ccfb: Couldn't register device %d.\n", DEV_MINOR);
		return -EINVAL;
	}

	return 0;
}

static int tcc_ccfb_remove(struct platform_device *pdev)
{
	dprintk("==> %s\n", __func__);
	
	misc_deregister(&ccfb_misc_device);
	return 0;
}

#ifdef CONFIG_PM
//static volatile LCDC_CHANNEL active_lcdc_backup;
static int tcc_ccfb_suspend(struct platform_device *pdev, pm_message_t state)
{
#if 0
	ccfb_dev_config_t	*dev = get_ccfb_dev();

	dprintk("==> %s\n", __func__);

	dev->cur_state = CCFB_STATE_PAUSE;
	msleep_interruptible(50);
	
	active_lcdc_backup = *(dev->pCurLcdc);
	tccxxx_ccfb_deact_clock(dev);
#endif    /* End of 0 */

	return 0;
}

static int tcc_ccfb_resume(struct platform_device *pdev)
{
#if 0
	ccfb_dev_config_t	*dev = get_ccfb_dev();

	dprintk("==> %s\n", __func__);

	tccxxx_ccfb_act_clock(dev, dev->act_lcdc_idx);
	*(dev->pCurLcdc) = active_lcdc_backup;
	dev->cur_state = CCFB_STATE_RUNNING;
#endif    /* End of 0 */
		
	return 0;
}
#else
#define tcc_ccfb_suspend NULL
#define tcc_ccfb_resume NULL
#endif

static struct platform_device tcc_ccfb_device = {
	.name	= "tcc_ccfb",
	.dev	= {
		.release 	= NULL,
	},
	.id	= 0,
};

static struct platform_driver tcc_ccfb_driver = {
	.driver         = {
	     .name   = "tcc_ccfb",
	     .owner  = THIS_MODULE,
	},
	.probe		= tcc_ccfb_probe,
	.remove         = tcc_ccfb_remove,
	.suspend        = tcc_ccfb_suspend,
	.resume         = tcc_ccfb_resume,
};

static int __init tccxxx_ccfb_init(void)
{
	dprintk("==> %s\n", __func__);
	
	platform_device_register(&tcc_ccfb_device);
	platform_driver_register(&tcc_ccfb_driver);

	return 0;
}
module_init(tccxxx_ccfb_init);

static void __exit tccxxx_ccfb_exit(void)
{
	dprintk("==> %s\n", __func__);
	
	platform_driver_unregister(&tcc_ccfb_driver);
	platform_device_unregister(&tcc_ccfb_device);
	
	return;
}
module_exit(tccxxx_ccfb_exit);


MODULE_AUTHOR("Telechips Inc.");
MODULE_DESCRIPTION("Telechips ccfb driver");
MODULE_LICENSE("GPL");
