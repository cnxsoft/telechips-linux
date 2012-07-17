/*
 * linux/drivers/video/tcc/tccfb_interface.c
 *
 * Based on:    Based on s3c2410fb.c, sa1100fb.c and others
 * Author:  <linux@telechips.com>
 * Created: June 10, 2008
 * Description: TCC LCD Controller Frame Buffer Driver
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
#include <linux/cpufreq.h>
#include <linux/irq.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#include <asm/tlbflush.h>
#include <asm/cacheflush.h>
#include <asm/mach-types.h>

#include <mach/globals.h>
#include <mach/reg_physical.h>
#include <mach/tca_ckc.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <plat/pmap.h>
#include <mach/tcc_fb.h>
#include "tccfb.h"
#include <mach/tccfb_ioctrl.h>
#include <mach/TCC_LCD_Interface.h>
#include <mach/tcc_scaler_ctrl.h>

#include <mach/tcc_composite_ioctl.h>
#include <mach/tcc_component_ioctl.h>

#include <mach/tca_lcdc.h>
#include <mach/tca_fb_hdmi.h>
#include <mach/tca_fb_output.h>

#include <linux/tcc_pwm.h>

#include <mach/timex.h>

#include <mach/vioc_outcfg.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_disp.h>
#include <mach/vioc_config.h>

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "VIOC_I: " msg); }

static int screen_debug = 0;
#define sprintk(msg...)	if (screen_debug) { printk( "VIOC scr: " msg); }


struct lcd_struct {
	spinlock_t lock;
	wait_queue_head_t waitq;
	char state;
};
static struct lcd_struct lcdc_struct;


char Fb_Lcdc_num = 0;

static VIOC_DISP * pDISPBase;
static VIOC_WMIX* pWMIXBase;
static VIOC_RDMA *pRDMABase;

static struct clk *lcdc0_clk;
static struct clk *lcdc1_clk;

#ifdef CONFIG_LCD_CPU_INTERFACE
static struct clk *lcd_si;
#endif//CONFIG_LCD_CPU_INTERFACE

void tcafb_clock_init(void)
{
	lcdc0_clk = clk_get(0, "lcdc0");
	if (IS_ERR(lcdc0_clk))
		lcdc0_clk = NULL;

	lcdc1_clk = clk_get(0, "lcdc1");


	#ifdef CONFIG_LCD_CPU_INTERFACE
	lcd_si = clk_get(0, "lcdsi");
	#endif//

	BUG_ON(lcdc0_clk == NULL);
	BUG_ON(lcdc1_clk == NULL);
	#ifdef CONFIG_LCD_CPU_INTERFACE
	BUG_ON(lcd_si == NULL);
	#endif//

	
}

void tca92xxfb_clock_delete(void)
{
	clk_put(lcdc0_clk);
	clk_put(lcdc1_clk);

	#ifdef CONFIG_LCD_CPU_INTERFACE
	clk_put(lcd_si);
	#endif//
}

#if defined(CONFIG_CPU_FREQ)
extern struct tcc_freq_table_t stFBClockLimitTable;
#endif//
static int  tcafb_clock_set(int cmd)
{
	int ret = 0;
    switch (cmd) {
	    case PWR_CMD_OFF:
			#if CONFIG_CPU_FREQ
			tcc_cpufreq_set_limit_table(&stFBClockLimitTable, TCC_FREQ_LIMIT_FB, 0);
			#endif			
			clk_disable(lcdc0_clk);
			clk_disable(lcdc1_clk);

			#ifdef CONFIG_LCD_CPU_INTERFACE
			clk_disable(lcd_si);
			#endif//

			break;
			
	    case PWR_CMD_ON:
			clk_enable(lcdc0_clk);
			clk_enable(lcdc1_clk);

			#ifdef CONFIG_LCD_CPU_INTERFACE
			clk_enable(lcd_si);
			#endif//

			#if CONFIG_CPU_FREQ
			tcc_cpufreq_set_limit_table(&stFBClockLimitTable, TCC_FREQ_LIMIT_FB, 1);
			#endif			

			break;

	    default:
			ret = -EINVAL;
	        break;
    }

    return ret;
	
}


/* tccfb_pan_display
 *
 * pandisplay (set) the controller from the given framebuffer
 * information
*/
int tca_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	int ret = 1;

	#ifdef CONFIG_FB_TCC_USE_VSYNC_INTERRUPT
	if(lcdc_struct.state == 0)
		ret = wait_event_interruptible_timeout(lcdc_struct.waitq, lcdc_struct.state == 1, msecs_to_jiffies(50));

	if(!ret)	{
	 	printk("  [%d]: tcc_setup_interrupt timed_out!! \n", ret);
	}
	#else
		msleep(16);
	#endif //CONFIG_FB_TCC_USE_VSYNC_INTERRUPT
	return 0;
}
EXPORT_SYMBOL(tca_fb_pan_display);

void tca_fb_vsync_activate(struct fb_var_screeninfo *var, struct tccfb_info *fbi)
{
	#ifdef CONFIG_FB_TCC_USE_VSYNC_INTERRUPT
	pDISPBase->uLSTATUS.nREG = 0xFFFFFFFF;
	lcdc_struct.state = 0;
	tca_lcdc_interrupt_onoff(TRUE, Fb_Lcdc_num);

	#endif//
}


/* tccfb_activate_var
 * activate (set) the controller from the given framebuffer
 * information
*/
void tca_fb_activate_var(struct tccfb_info *fbi,  struct fb_var_screeninfo *var)
{
	unsigned int imgch, fmt ,  base_addr;
	unsigned int regl, lcd_width, lcd_height, img_width, img_height;

	#define IMG_AOPT       	2
	
	unsigned int alpha_type = 0, alpha_blending_en = 0;
	unsigned int chromaR, chromaG, chromaB, chroma_en;


	imgch = fbi->fb->node; //FB_driver
	if(fbi->fb->var.bits_per_pixel == 32) 	{
		chroma_en = 0;
		alpha_type = 1;
		alpha_blending_en = 1;
		fmt = TCC_LCDC_IMG_FMT_RGB888;
	}
	else if(fbi->fb->var.bits_per_pixel == 16)	{
		chroma_en = 1;
		alpha_type = 0;
		alpha_blending_en = 0;
		fmt = TCC_LCDC_IMG_FMT_RGB565; 
	}
	else	{
		printk("%s:fb%d Not Supported BPP!\n", __FUNCTION__, fbi->fb->node);
		return;
	}

	chromaR = chromaG = chromaB = 0;

	sprintk("%s: fb%d Supported BPP!\n", __FUNCTION__, fbi->fb->node);

 	base_addr = fbi->map_dma + fbi->fb->var.xres * var->yoffset * (fbi->fb->var.bits_per_pixel/8);
	if(fbi->fb->var.yoffset > fbi->fb->var.yres)	{
		base_addr = PAGE_ALIGN(base_addr);
	}

	sprintk("%s: fb%d Baddr:0x%x Naddr:0x%x!\n", __FUNCTION__, fbi->fb->node, base_addr, pRDMABase->nBASE0);

	regl = pDISPBase->uLSIZE.nREG;

	lcd_width = (regl & 0xFFFF);
	lcd_height = ((regl>>16) & 0xFFFF);
	img_width = fbi->fb->var.xres;
	img_height = fbi->fb->var.yres;

	if(img_width > lcd_width)	
		img_width = lcd_width;
	
	if(img_height > lcd_height)	
		img_height = lcd_height;

	
	/* write new registers */
	switch(imgch)
	{
		case 0:
			// default framebuffer 
			VIOC_WMIX_SetPosition(pWMIXBase, imgch, 0, 0);

			VIOC_RDMA_SetImageFormat(pRDMABase, fmt );	//fmt
			VIOC_RDMA_SetImageOffset (pRDMABase, fmt, img_width  );	//offset	
			VIOC_RDMA_SetImageSize (pRDMABase,  img_width, img_height  );	//size	
			VIOC_RDMA_SetImageBase (pRDMABase , base_addr, 0 , 0 );
			VIOC_RDMA_SetImageEnable(pRDMABase);

			VIOC_RDMA_SetImageAlphaSelect(pRDMABase, alpha_type);
			VIOC_RDMA_SetImageAlphaEnable(pRDMABase, alpha_blending_en);

			//overlay setting
			VIOC_WMIX_SetChromaKey(pWMIXBase, imgch, chroma_en, chromaR, chromaG, chromaB, 0xF8, 0xFC, 0xF8);			

			VIOC_WMIX_SetUpdate(pWMIXBase);	
			VIOC_RDMA_SetImageUpdate(pRDMABase);

			#ifdef CONFIG_FB_TCC_USE_VSYNC_INTERRUPT
			tca_fb_vsync_activate(var, fbi);
			#endif//
			
			break;

		case 1:
			break;

		case 2:
			break;
	}
	sprintk("%s: fb  end\n", __FUNCTION__);

	return;
	
}
EXPORT_SYMBOL(tca_fb_activate_var);



#ifdef CONFIG_HAS_EARLYSUSPEND

void tca_fb_earlier_suspend(struct early_suspend *h)
{
	printk("%s: START Fb_Lcdc_num:%d \n", __FUNCTION__, Fb_Lcdc_num);
}
EXPORT_SYMBOL(tca_fb_earlier_suspend);



static volatile VIOC_RDMA pRDMA_BackUp;

void tca_fb_early_suspend(struct early_suspend *h)
{
	if((system_rev != 0x2004) && (system_rev != 0x2005))
	{
		pRDMA_BackUp = *pRDMABase;

		#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
		tca_lcdc_interrupt_onoff(FALSE, Fb_Lcdc_num);
		#endif
		
		if (Fb_Lcdc_num)
			disable_irq(INT_VIOC_DEV1);
		else
			disable_irq(INT_VIOC_DEV0);

		VIOC_DISP_TurnOff(pDISPBase);
		VIOC_RDMA_SetImageDisable(pRDMABase);

		tcafb_clock_set(PWR_CMD_OFF);
	}
	else
	{
		VIOC_RDMA *pRDMABase_video;
			
		if(Fb_Lcdc_num)	{
			pRDMABase_video = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA06);
		}
		else	{
			pRDMABase_video = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA02);
		}
		VIOC_RDMA_SetImageSize(pRDMABase_video, 0, 0);
		VIOC_RDMA_SetImageUpdate(pRDMABase_video);

		VIOC_RDMA_SetImageSize(pRDMABase, 0, 0);
		VIOC_RDMA_SetImageUpdate(pRDMABase);
	}
	
}
EXPORT_SYMBOL(tca_fb_early_suspend);


void tca_fb_late_resume(struct early_suspend *h)
{
	if((system_rev != 0x2004) && (system_rev != 0x2005))
	{
		tcafb_clock_set(PWR_CMD_ON);	

		*pRDMABase = pRDMA_BackUp;
		VIOC_RDMA_SetImageSize(pRDMABase, 0, 0);
		VIOC_RDMA_SetImageEnable(pRDMABase);

		VIOC_WMIX_SetUpdate(pWMIXBase);	

		if (Fb_Lcdc_num)
			enable_irq(INT_VIOC_DEV1);
		else
			enable_irq(INT_VIOC_DEV0);
	}
}
EXPORT_SYMBOL(tca_fb_late_resume);


void tca_fb_later_resume(struct early_suspend *h)
{
	printk("%s: START Fb_Lcdc_num:%d \n", __FUNCTION__, Fb_Lcdc_num);

}
EXPORT_SYMBOL(tca_fb_later_resume);
#endif



/* suspend and resume support for the lcd controller */
int tca_fb_suspend(struct platform_device *dev, pm_message_t state)
{
	printk("%s:  \n", __FUNCTION__);
	if((system_rev == 0x2004) || (system_rev == 0x2005))
	{
		pRDMA_BackUp = *pRDMABase;

		#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
		tca_lcdc_interrupt_onoff(FALSE, Fb_Lcdc_num);
		#endif
		
		if (Fb_Lcdc_num)
			disable_irq(INT_VIOC_DEV1);
		else
			disable_irq(INT_VIOC_DEV0);

		VIOC_DISP_TurnOff(pDISPBase);
		VIOC_RDMA_SetImageDisable(pRDMABase);

		tcafb_clock_set(PWR_CMD_OFF);
	}
	
	return 0;
}
EXPORT_SYMBOL(tca_fb_suspend);


int tca_fb_resume(struct platform_device *dev)
{
	printk("%s:  \n", __FUNCTION__);

	if((system_rev == 0x2004) || (system_rev == 0x2005))
	{
		tcafb_clock_set(PWR_CMD_ON);	

		*pRDMABase = pRDMA_BackUp;
		VIOC_RDMA_SetImageSize(pRDMABase, 0, 0);

		VIOC_RDMA_SetImageEnable(pRDMABase);

		VIOC_WMIX_SetUpdate(pWMIXBase);	

		if (Fb_Lcdc_num)
			enable_irq(INT_VIOC_DEV1);
		else
			enable_irq(INT_VIOC_DEV0);	
	}	
	
	return 0;
}
EXPORT_SYMBOL(tca_fb_resume);


irqreturn_t tcc_lcd_handler(int irq, void *dev_id)
{
	unsigned int VOICstatus = pDISPBase->uLSTATUS.nREG;
	
 	sprintk("%s lcdc_struct.state:%d STATUS:0x%x 0x%x \n",__func__, 	lcdc_struct.state, VOICstatus, (unsigned int)pDISPBase->uLSTATUS.nREG);
	
	if(VOICstatus & VIOC_DISP_IREQ_RU_MASK)
	{
		pDISPBase->uLSTATUS.nREG = 0xFFFFFFFF;
		
		#ifndef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
		tca_lcdc_interrupt_onoff(FALSE, Fb_Lcdc_num);
		#endif

		if(lcdc_struct.state == 0)		{
			lcdc_struct.state = 1;
			wake_up_interruptible(&lcdc_struct.waitq);
		}
	}

	return IRQ_HANDLED;
}


int tcc_lcd_interrupt_reg(char SetClear)
{
	int ret = 0;
	dprintk("%s SetClear:%d lcdc_num:%d \n",__func__, SetClear, Fb_Lcdc_num);

	if(SetClear)	{

			if(Fb_Lcdc_num)	{
				ret	= request_irq(INT_VIOC_DEV1, tcc_lcd_handler,	IRQ_TYPE_EDGE_FALLING|IRQF_SHARED,
									"TCC_LCD",	tcc_lcd_handler);
			}
			else {
				ret	= request_irq(INT_VIOC_DEV0, tcc_lcd_handler,	IRQ_TYPE_EDGE_FALLING|IRQF_SHARED,
						"TCC_LCD",	tcc_lcd_handler);	
			}		

			tca_lcdc_interrupt_onoff(1, Fb_Lcdc_num);
	}
	else 	{
		if(Fb_Lcdc_num)
			free_irq(INT_VIOC_DEV1, tcc_lcd_handler);
		else
			free_irq(INT_VIOC_DEV0, tcc_lcd_handler);
	}
	return ret;
}
EXPORT_SYMBOL(tcc_lcd_interrupt_reg);

extern unsigned int tca_get_lcd_lcdc_num(viod);
int tca_fb_init(void)
{
	pmap_t pmap_fb_video;

	printk(KERN_INFO " tcc892X %s (built %s %s)\n", __func__, __DATE__, __TIME__);

	pmap_get_info("fb_video", &pmap_fb_video);

	Fb_Lcdc_num = tca_get_lcd_lcdc_num();

	if(Fb_Lcdc_num){
		VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_MRGB, VIOC_OUTCFG_DISP1);
		VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_HDMI, VIOC_OUTCFG_DISP0);

		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);
		pRDMABase = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA04);
	} else {
		VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_MRGB, VIOC_OUTCFG_DISP0);
		VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_HDMI, VIOC_OUTCFG_DISP1);

		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP0);
		pRDMABase = (VIOC_RDMA *)tcc_p2v(HwVIOC_RDMA00);
	}

	tcafb_clock_init();
	tcafb_clock_set(PWR_CMD_ON);

	TCC_OUTPUT_LCDC_Init();
	init_waitqueue_head(&lcdc_struct.waitq);
	lcdc_struct.state = 1;

	printk(" %s LCDC:%d  pRDMABase:%p  pWMIXBase:%p pDISPBase:%p end \n", __func__, Fb_Lcdc_num, pRDMABase, pWMIXBase, pDISPBase);

	return 0;
}
EXPORT_SYMBOL(tca_fb_init);


void tca_fb_exit(void)
{
	dprintk(" %s LCDC:%d \n", __func__, Fb_Lcdc_num);
	tca92xxfb_clock_delete();
}
EXPORT_SYMBOL(tca_fb_exit);


