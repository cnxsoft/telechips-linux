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

/* Debugging stuff */
static int debug = 1;
#define dprintk(msg...)	if (debug) { printk( "fb92_I: " msg); }

static int screen_debug = 0;
#define sprintk(msg...)	if (screen_debug) { printk( "tcc92fb scr: " msg); }

extern unsigned int tcc_output_fb_get_disable(void);
extern void TCC_LCDC_Change_CtrlReg(PLCDC SrcLCDC, PLCDC DestLCDC);
extern void TCC_LCDC_Change_ChannelReg(PLCDC_CHANNEL SrcLCDC_CH, PLCDC_CHANNEL DestLCDC_CH);

 
struct lcd_struct {
	spinlock_t lock;
	wait_queue_head_t waitq;
	char state;
};
static struct lcd_struct lcdc_struct;


char Fb_Lcdc_num = 1;
static volatile PLCDC pLCDC;
static volatile PLCDC_CHANNEL pLCDC_FB_CH;


static struct clk *lcdc0_clk;
static struct clk *lcdc1_clk;
static struct clk *ddi_cache;
#ifdef CONFIG_LCD_CPU_INTERFACE
static struct clk *lcd_si;
#endif//CONFIG_LCD_CPU_INTERFACE
 
void tca92xxfb_clock_init(void)
{
	lcdc0_clk = clk_get(0, "lcdc0");
	lcdc1_clk = clk_get(0, "lcdc1");

	ddi_cache = clk_get(0, "ddi_cache");

	#ifdef CONFIG_LCD_CPU_INTERFACE
	lcd_si = clk_get(0, "lcdsi");
	#endif//

	BUG_ON(IS_ERR(lcdc0_clk));
	BUG_ON(IS_ERR(lcdc1_clk));
	BUG_ON(IS_ERR(ddi_cache));
	#ifdef CONFIG_LCD_CPU_INTERFACE
	BUG_ON(IS_ERR(lcd_si));
	#endif//
 }

void tca92xxfb_clock_delete(void)
{
	clk_put(lcdc0_clk);
	clk_put(lcdc1_clk);
	clk_put(ddi_cache);

	#ifdef CONFIG_LCD_CPU_INTERFACE
	clk_put(lcd_si);
	#endif//
}

#if defined(CONFIG_CPU_FREQ)
extern struct tcc_freq_table_t stFBClockLimitTable;
#endif//
static int  tca92xxfb_clock_set(int cmd)
{
	int ret = 0;
    switch (cmd) {
	    case PWR_CMD_OFF:
			#if CONFIG_CPU_FREQ
			tcc_cpufreq_set_limit_table(&stFBClockLimitTable, TCC_FREQ_LIMIT_FB, 0);
			#endif			
			clk_disable(ddi_cache);
			clk_disable(lcdc0_clk);
			clk_disable(lcdc1_clk);

			#ifdef CONFIG_LCD_CPU_INTERFACE
			clk_disable(lcd_si);
			#endif// 
			break;
			
	    case PWR_CMD_ON:
			clk_enable(lcdc0_clk);
			clk_enable(lcdc1_clk);
			clk_enable(ddi_cache);

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



char TCC_FB_LCDC_NumSet(char NumLcdc, char OnOff)
{
	volatile int ttt= 0;
	unsigned int regl;
	struct lcd_panel *panel;

	PLCDC pLCDC0, pLCDC1;
	PLCDC_CHANNEL pLCDC0_FB_CH, pLCDC1_FB_CH;

	PCKC	pCKC = (PCKC)tcc_p2v(HwCKC_BASE);

	pLCDC0 = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	pLCDC0_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(pLCDC0->LI0C);

	pLCDC1 = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	pLCDC1_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(pLCDC1->LI2C);

	dprintk("%s set_num:%d before_num:%d OnOff:%d \n",__func__, NumLcdc, Fb_Lcdc_num, OnOff);
	panel = tccfb_get_panel();

	if(NumLcdc != Fb_Lcdc_num)
	{
		if(NumLcdc)
		{
			regl = readl(&pCKC->PCLK_LCD0);
			writel(regl | (0x10000000), &pCKC->PCLK_LCD1); // EN: 0
		}
		else
		{
			regl = readl(&pCKC->PCLK_LCD1);
			writel(regl | (0x10000000), &pCKC->PCLK_LCD0); // EN: 0
		}

		if(NumLcdc)
		{
			pLCDC0->LCTRL &= (~HwLCTRL_LEN);
			pLCDC0->LI0C &= (~HwLIC_IEN);

			pLCDC1->LCTRL &= (~HwLCTRL_LEN);
			pLCDC1->LI0C &= (~HwLIC_IEN);
			pLCDC1->LI1C &= (~HwLIC_IEN);
			pLCDC1->LI2C &= (~HwLIC_IEN);		

			msleep(7);
			
			while((ISSET(pLCDC0->LSTATUS, HwLSTATUS_BUSY))|| (ISSET(pLCDC1->LSTATUS, HwLSTATUS_BUSY)))
					ttt++;								

			if(ttt)
				printk("~~~~~~~~~~~~~  %d ~~~~~~~~~~~~ \n", ttt);
			
			TCC_LCDC_Change_ChannelReg(pLCDC0_FB_CH, pLCDC1_FB_CH);
			TCC_LCDC_Change_CtrlReg(pLCDC0, pLCDC1);

		}
		else
		{
			pLCDC1->LCTRL &= (~HwLCTRL_LEN);
			pLCDC1->LI0C &= (~HwLIC_IEN);
			pLCDC1->LI1C &= (~HwLIC_IEN);
			pLCDC1->LI2C &= (~HwLIC_IEN);

			pLCDC0->LCTRL &= (~HwLCTRL_LEN);
			pLCDC0->LI0C &= (~HwLIC_IEN);

			msleep(7);
			
			while((ISSET(pLCDC0->LSTATUS, HwLSTATUS_BUSY))|| (ISSET(pLCDC1->LSTATUS, HwLSTATUS_BUSY)))
					ttt++;								

			if(ttt)
				printk("~~~~~~~~~~~~~  %d ~~~~~~~~~~~~~~~ \n", ttt);

			TCC_LCDC_Change_ChannelReg(pLCDC1_FB_CH, pLCDC0_FB_CH);
			TCC_LCDC_Change_CtrlReg(pLCDC1, pLCDC0);

		}	


		if(OnOff)
		{
			if(NumLcdc)			{
				pLCDC1_FB_CH->LIC |= HwLIC_IEN;
				pLCDC1->LCTRL |= HwLCTRL_LEN;
			}
			else			{
				pLCDC0_FB_CH->LIC |= HwLIC_IEN;
				pLCDC0->LCTRL |= HwLCTRL_LEN;
			}
		}


		if(NumLcdc)
		{
			pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
			pLCDC_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(pLCDC->LI2C);
			Fb_Lcdc_num = 1;
		}
		else
		{
			pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
			pLCDC_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(pLCDC->LI0C);
			Fb_Lcdc_num = 0;
		}
		Fb_Lcdc_num = NumLcdc;

		LCDC_IO_Set(Fb_Lcdc_num, panel->bpp);

		return 1;
	}
	
	return 0;
}


/* tccfb_pan_display
 *
 * pandisplay (set) the controller from the given framebuffer
 * information
*/
int tca_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	return 0;
}
EXPORT_SYMBOL(tca_fb_pan_display);

void tca_default_fb_info_set(struct tccfb_info *fbi)
{
	unsigned int base_addr = 0, tmp_value = 0, bpp_value = 0;

	sprintk("%s:fb_node: %d !\n", __FUNCTION__, fbi->fb->node);

	base_addr = fbi->map_dma +fbi->fb->var.xres * fbi->fb->var.yoffset * (fbi->fb->var.bits_per_pixel/8);
	if(fbi->fb->var.yoffset > fbi->fb->var.yres)	{
		base_addr = PAGE_ALIGN(base_addr);
	}


	if(fbi->fb->var.bits_per_pixel == 32){
		bpp_value = 0x000C; //RGB888
	}
	else if(fbi->fb->var.bits_per_pixel == 16)		{
		bpp_value = 0x000A; //RGB565
	}
	else	{
		printk("%s:fb%d Not Supported BPP!\n", __FUNCTION__, fbi->fb->node);
		return;
	}

	tmp_value = pLCDC->LDS; // get LCD size 

	// default framebuffer 
	tmp_value = pLCDC_FB_CH->LIC;
	pLCDC_FB_CH->LIC = (tmp_value & 0xFFFFFFE0) | (bpp_value & 0x0000001F);
	pLCDC_FB_CH->LIO = fbi->fb->var.xres * (fbi->fb->var.bits_per_pixel/8);
	pLCDC_FB_CH->LIS	= (fbi->fb->var.yres<< 16) | (fbi->fb->var.xres); //Size
	pLCDC_FB_CH->LIBA0	= base_addr;


}


void tca_fb_vsync_activate(struct fb_var_screeninfo *var, struct tccfb_info *fbi)
{
	unsigned int base_addr = 0;
	unsigned int bpp = fbi->fb->var.bits_per_pixel / 8;	/* bytes per pixel */


 	base_addr = fbi->map_dma +fbi->fb->var.xres * bpp * var->yoffset;
	if(var->yoffset > var->yres)	{
		base_addr = PAGE_ALIGN(base_addr);
	}


	#ifdef CONFIG_FB_TCC_USE_VSYNC_INTERRUPT
	{
		int ret;
 
			pLCDC->LSTATUS = 0xFFFFFFFF; 
			tca_lcdc_interrupt_onoff(TRUE, Fb_Lcdc_num);
			lcdc_struct.state = 0;

			ret = wait_event_interruptible_timeout(lcdc_struct.waitq, lcdc_struct.state == 1, msecs_to_jiffies(50));
			if(!ret)	{
			 	printk("  [%d]: tcc_setup_interrupt timed_out!! \n", ret);
			}

			#ifdef CONFIG_USE_EXT_INTERRUPT
			else
			{
				if(!(pLCDC->LSTATUS & HwLSTATUS_RU))
				{
					tca_lcdc_interrupt_onoff(TRUE, Fb_Lcdc_num);
					lcdc_struct.state = 0;
					ret = wait_event_interruptible_timeout(lcdc_struct.waitq, lcdc_struct.state == 1, msecs_to_jiffies(50));
					if(!ret)	{
					 	printk("  [%d]: tcc_setup_interrupt timed_out!! \n", ret);
					}
				}
			}
			#endif//
 	}
	#endif //CONFIG_FB_TCC_USE_VSYNC_INTERRUPT

}

void tcc_disable_internal_display(void)
{
	if(pLCDC_FB_CH){
		/* Disable : CH2 of LCDC1 */		
		if(pLCDC_FB_CH->LIC & Hw28){
			BITCSET(pLCDC_FB_CH->LIC, 1<<28, 0<<28);
		#ifndef CONFIG_ARCH_TCC92XX
			pLCDC_FB_CH->LIC |=  HwLCT_RU;    //  image update enable
		#endif//	CONFIG_ARCH_TCC92XX
		}
	}
}

/* tccfb_activate_var
 * activate (set) the controller from the given framebuffer
 * information
*/
void tca_fb_activate_var(struct tccfb_info *fbi,  struct fb_var_screeninfo *var)
{
	unsigned int imgch, bpp_value,tmp_value, base_addr;
	unsigned int regl, lcd_width, lcd_height, img_width, img_height;

	#define IMG_AOPT          	2
	
	unsigned int alpha_type = 0, alpha_blending_en = 0;
	unsigned int chromaR, chromaG, chromaB, chroma_en;

	imgch = fbi->fb->node;
	if(fbi->fb->var.bits_per_pixel == 32)
	{
		chroma_en = 0;
		alpha_type = 1;
		alpha_blending_en = 1;
		bpp_value = 0x000C; //RGB888
	}
	else if(fbi->fb->var.bits_per_pixel == 16)
	{
		chroma_en = 1;
		alpha_type = 0;
		alpha_blending_en = 0;
		bpp_value = 0x000A; //RGB565
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


	sprintk("%s: fb%d Baddr:0x%x Naddr:0x%x!\n", __FUNCTION__, fbi->fb->node, base_addr, pLCDC_FB_CH->LIBA0);


	regl = pLCDC->LDS; // get LCD size 

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
			/* If LCD CH1 is enabled, Do not update anythings on Ch2 */
			if(tcc_output_fb_get_disable()){
				BITCSET(pLCDC_FB_CH->LIC, 1<<28, 0<<28);
			#ifndef CONFIG_ARCH_TCC92XX
				pLCDC_FB_CH->LIC |=  HwLCT_RU;    //  image update enable
			#endif//	CONFIG_ARCH_TCC92XX
				break;
			}else{
				BITCSET(pLCDC_FB_CH->LIC, 1<<28, 1<<28);
			}
			
			// default framebuffer 
			tmp_value = pLCDC_FB_CH->LIC;
			pLCDC_FB_CH->LIC = (tmp_value & 0xFFFFFFE0) | (bpp_value & 0x0000001F);
			pLCDC_FB_CH->LIO = fbi->fb->var.xres * (fbi->fb->var.bits_per_pixel/8);
			pLCDC_FB_CH->LIS	= (img_height<< 16) | (img_width); //Size
			pLCDC_FB_CH->LIP	= 0; // position
	 		pLCDC_FB_CH->LIBA0	= base_addr;				
	 

			// overlay ¼Ó¼º
			BITCSET (pLCDC->LI2C,	0x1<< 30,  (alpha_blending_en)	<< 30); // alpha enable
			BITCSET (pLCDC->LI2C, 0x3<< 25,	(IMG_AOPT)	<< 25); // alpha opt
			BITCSET (pLCDC->LI2C, 0x1<< 24,	(alpha_type)  << 24); // alpha select  

			BITCSET (pLCDC->LI2C, 0x1<< 29,	(chroma_en)  << 29); // chroma-keying
			
			BITCSET (pLCDC->LI2KR, 0xff <<  0, (chromaR)  <<  0); // key
			BITCSET (pLCDC->LI2KR, 0xff << 16, (0xF8)	<< 16); // keymask
			BITCSET (pLCDC->LI2KG, 0xff <<  0, (chromaG)  <<  0); // key
			BITCSET (pLCDC->LI2KG, 0xff << 16, (0xFC)	<< 16); // keymask
			BITCSET (pLCDC->LI2KB, 0xff <<  0, (chromaB)  <<  0); // key
			BITCSET (pLCDC->LI2KB, 0xff << 16, (0xF8)	<< 16); // keymask

			tmp_value = pLCDC->LI2C;
			pLCDC->LI2C = (tmp_value & 0xFFFFFFE0) | (bpp_value & 0x0000001F);
			pLCDC->LI2O = fbi->fb->var.xres * (fbi->fb->var.bits_per_pixel/8);

			#ifndef CONFIG_ARCH_TCC92XX
			pLCDC_FB_CH->LIC |=  HwLCT_RU;    //  image update enable
			#endif//	CONFIG_ARCH_TCC92XX

			#ifdef CONFIG_FB_TCC_USE_VSYNC_INTERRUPT
			tca_fb_vsync_activate(var, fbi);
			#else
			msleep(16);
			#endif//
			
			break;

		case 1:
			tmp_value = pLCDC->LI1C;
			pLCDC->LI1C = (tmp_value & 0xFFFFFFE0) | (bpp_value & 0x0000001F);
			pLCDC->LI1O = fbi->fb->var.xres * (fbi->fb->var.bits_per_pixel/8);
			pLCDC->LI1S	= (img_height<< 16) | (img_width); //Size
			pLCDC->LI1BA0 = base_addr;
			#ifdef CONFIG_ARCH_TCC92XX
			pLCDC->LI1C |= (HwLIC_IEN);
			#else
			pLCDC->LI1C |= (HwLIC_IEN | HwLCT_RU);
			#endif//
			break;

		case 2:
			tmp_value = pLCDC->LI0C;
			pLCDC->LI0C = (tmp_value & 0xFFFFFFE0) | (bpp_value & 0x0000001F);
			pLCDC->LI0O = fbi->fb->var.xres * (fbi->fb->var.bits_per_pixel/8);
			pLCDC->LI0S	= (img_height<< 16) | (img_width); //Size
			pLCDC->LI0BA0 = base_addr;
			#ifdef CONFIG_ARCH_TCC92XX
			pLCDC->LI0C |= (HwLIC_IEN);
			#else
			pLCDC->LI0C |= (HwLIC_IEN | HwLCT_RU);
			#endif//			
			break;
	}

	return;
	
}
EXPORT_SYMBOL(tca_fb_activate_var);



#ifdef CONFIG_HAS_EARLYSUSPEND

void tca_fb_earlier_suspend(struct early_suspend *h)
{
	printk("%s: START Fb_Lcdc_num:%d \n", __FUNCTION__, Fb_Lcdc_num);
}
EXPORT_SYMBOL(tca_fb_earlier_suspend);


static volatile LCDC_CHANNEL LCDC_FB_BackUp;
static volatile lcdc_gamma_params LCDC_Gamma_BackUp;

extern unsigned int tcc_LCDC_set_gamma(lcdc_gamma_params *gamma);
extern unsigned int tcc_LCDC_get_gamma(lcdc_gamma_params *gamma);



void tca_fb_early_suspend(struct early_suspend *h)
{

	LCDC_FB_BackUp = *pLCDC_FB_CH;

	//gamma setting backup
	LCDC_Gamma_BackUp.lcdc_num = Fb_Lcdc_num;
	tcc_LCDC_get_gamma(&LCDC_Gamma_BackUp);
	#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
	tca_lcdc_interrupt_onoff(FALSE, Fb_Lcdc_num);
	#endif
	
 	tcc_LCDC_Wait_signal_disable(0);

	tcc_LCDC_Wait_signal_disable(1);

	tca92xxfb_clock_set(PWR_CMD_OFF);

}
EXPORT_SYMBOL(tca_fb_early_suspend);


void tca_fb_late_resume(struct early_suspend *h)
{

	tca92xxfb_clock_set(PWR_CMD_ON);

	tcc_onthefly_init();
	tcc_ddi_cache_setting();

	 *pLCDC_FB_CH = LCDC_FB_BackUp;
	 
	//gamma setting restore
	tcc_LCDC_set_gamma(&LCDC_Gamma_BackUp);

	#ifdef CONFIG_ARCH_TCC92XX
	pLCDC_FB_CH->LIC |= (HwLIC_IEN);    //  image enable
	#else
	pLCDC_FB_CH->LIC |= (HwLIC_IEN | HwLCT_RU);    //  image enable
	#endif//	CONFIG_ARCH_TCC92XX
	
	printk(" end \n ");
}
EXPORT_SYMBOL(tca_fb_late_resume);


void tca_fb_later_resume(struct early_suspend *h)
{
	printk("%s: START Fb_Lcdc_num:%d \n", __FUNCTION__, Fb_Lcdc_num);

	// lcd module on
//  	tca_bkl_powerup();
}
EXPORT_SYMBOL(tca_fb_later_resume);
#endif



/* suspend and resume support for the lcd controller */
int tca_fb_suspend(struct platform_device *dev, pm_message_t state)
{
	printk("%s:  \n", __FUNCTION__);
	return 0;
}
EXPORT_SYMBOL(tca_fb_suspend);


int tca_fb_resume(struct platform_device *dev)
{
	printk("%s:  \n", __FUNCTION__);
	return 0;
}
EXPORT_SYMBOL(tca_fb_resume);


static irqreturn_t tcc_lcd_handler(int irq, void *dev_id)
{
	unsigned int LCDCstatus = pLCDC->LSTATUS;

 	sprintk("%s lcdc_struct.state:%d STATUS:0x%x 0x%x \n",__func__, lcdc_struct.state, LCDCstatus,  pLCDC->LSTATUS);


//	if(LCDCstatus & HwLSTATUS_VSF)
	{
		#ifndef CONFIG_USE_EXT_INTERRUPT		
		pLCDC->LSTATUS = 0xFFFFFFFF;
		#endif//
		
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

	if(SetClear)
	{
		#ifdef CONFIG_USE_EXT_INTERRUPT
		   if (machine_is_tcc8900()) {
			    BITCLR(HwGPIOF->GPFN3, Hw8-Hw4);
			    BITCLR(HwGPIOF->GPEN, Hw25);
				
				BITCSET(HwEINTSEL->EINTSEL1, HwEINTSEL1_EINT6_MASK, HwEINTSEL1_EINT6(SEL_GPIOF25));
			}
           
			if(machine_is_m57te())
			{
			    if( system_rev >= 0x0130) {
    			    	BITCLR(HwGPIOF->GPFN1, 0x00F00000 );
    			    	BITCLR(HwGPIOF->GPEN, Hw13);
    				BITCSET(HwEINTSEL->EINTSEL1, HwEINTSEL1_EINT6_MASK, HwEINTSEL1_EINT6(SEL_GPIOF13));	
		    	    } 
			    else {
    			    	BITCLR(HwGPIOA->GPFN0, 0xF0000000 );
	    			BITCLR(HwGPIOA->GPEN, Hw7);
    				BITCSET(HwEINTSEL->EINTSEL1, HwEINTSEL1_EINT6_MASK, HwEINTSEL1_EINT6(SEL_GPIOA7));	
   		            }
			}
			#if defined(CONFIG_MACH_M801)
			if(machine_is_m801())
			{
				BITCSET(HwGPIOF->GPFN1, HwPORTCFG_GPFN6_MASK ,HwPORTCFG_GPFN6(0));
				BITCSET(HwGPIOF->GPEN, Hw13 ,0);
				
				BITCSET(HwEINTSEL->EINTSEL1, HwEINTSEL1_EINT6_MASK, HwEINTSEL1_EINT6(SEL_GPIOF13));
			}
			#endif//

			BITSET(HwPIC->POL0, HwINT0_EI6);
			BITCLR(HwPIC->MODE0, HwINT0_EI6);
			BITCLR(HwPIC->MODEA0, HwINT0_EI6);
			BITSET(HwPIC->SEL0, HwINT0_EI6);	
			BITCLR(HwPIC->INTMSK0, HwINT0_EI6);	

			if (request_irq(IRQ_EI6, tcc_lcd_handler,	IRQF_SHARED,
							"TCC_LCD",	tcc_lcd_handler))	{
				printk(KERN_ERR "tcc_lcd_interrupt_reg request_irq ERROR!\n");
			}
		#else
		
			tca_lcdc_interrupt_onoff(0, Fb_Lcdc_num);
			if(!Fb_Lcdc_num){
				ret	= request_irq(IRQ_LCD0, tcc_lcd_handler,	IRQF_SHARED,
							"TCC_LCD",tcc_lcd_handler);
			}
			else {
				
				ret	= request_irq(IRQ_LCD1, tcc_lcd_handler,	IRQF_SHARED,
						"TCC_LCD",	tcc_lcd_handler);
			}
		#endif//CONFIG_USE_EXT_INTERRUPT
	}
	else
	{
		if(!Fb_Lcdc_num)
			free_irq(IRQ_LCD0, tcc_lcd_handler);
		else
			free_irq(IRQ_LCD1, tcc_lcd_handler);
	}
	
	return ret;
}
EXPORT_SYMBOL(tcc_lcd_interrupt_reg);


int tca_fb_init(void)
{
	struct lcd_panel *lcd_info;
	pmap_t pmap_fb_video;

	printk(KERN_INFO " tcc92xx %s (built %s %s)\n", __func__, __DATE__, __TIME__);

	pmap_get_info("fb_video", &pmap_fb_video);

#ifdef TCC_LCDC1_USE
	pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
#ifdef CONFIG_ARCH_TCC92XX
	pLCDC_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(pLCDC->LI2C);
#else
	pLCDC_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(HwLCDC1_CH_BASE(2));
#endif//
	Fb_Lcdc_num = 1;
#else
	pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
#ifdef CONFIG_ARCH_TCC92XX
	pLCDC_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(HwLCDC0_CH_BASE(0));
#else
	pLCDC_FB_CH = (volatile PLCDC_CHANNEL)tcc_p2v(HwLCDC0_CH_BASE(2));
#endif//
	Fb_Lcdc_num = 0;
#endif//	TCC_LCDC1_USE

	tca92xxfb_clock_init();
	tca92xxfb_clock_set(PWR_CMD_ON);

	tcc_onthefly_init();
	tcc_ddi_cache_setting();

	TCC_OUTPUT_LCDC_Init();

	init_waitqueue_head(&lcdc_struct.waitq);
	lcdc_struct.state = 1;

	printk(" %s LCDC:%d  end \n", __func__, Fb_Lcdc_num);

	return 0;
}
EXPORT_SYMBOL(tca_fb_init);


void tca_fb_cleanup(void)
{
	dprintk(" %s LCDC:%d \n", __func__, Fb_Lcdc_num);
	tca92xxfb_clock_delete();
}
EXPORT_SYMBOL(tca_fb_cleanup);


