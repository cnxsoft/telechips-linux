/*
 * linux/drivers/video/tcc_component.c
 *
 * Author:  <linux@telechips.com>
 * Created: March 18, 2010
 * Description: TCC Component TV-Out Driver
 *
 * Copyright (C) 20010-2011 Telechips 
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
#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include "tcc_component.h"
#include <mach/tcc_component_ioctl.h>
#include <mach/tccfb_ioctrl.h>
#include <mach/tca_fb_output.h>
#include <mach/tca_lcdc.h>
#include <mach/gpio.h>
#include "tcc_component_cs4954.h"
#include "tcc_component_ths8200.h"

/*****************************************************************************

 VARIABLES

******************************************************************************/

extern char fb_power_state;

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tcc_component: " msg); }

#define TCC_LCDC1_USE

static int				Component_LCDC_Num = -1;
static int				Component_Mode = -1;
static PLCDC			pComponent_HwLCDC = NULL;
static PLCDC_CHANNEL	pComponent_HwLCDC_CH = NULL;

static int				Component_FID = 0;

#define DEVICE_NAME		"component"
#define MAJOR_ID		206
#define MINOR_ID		1

#if defined (CONFIG_MACH_TCC9300ST)
#define COMPONENT_DETECT_GPIO		TCC_GPE(28)
#define COMPONENT_DETECT_EINTSEL	SEL_GPIOE28
#define COMPONENT_DETECT_EINTNUM	4
#define COMPONENT_DETECT_EINT		HwINT0_EI4
#elif defined (CONFIG_MACH_TCC8800ST)
#define COMPONENT_DETECT_GPIO		TCC_GPF(26)
#define COMPONENT_DETECT_EINTSEL	SEL_GPIOF26
#define COMPONENT_DETECT_EINTNUM	6
#define COMPONENT_DETECT_EINT		HwINT1_EI6
#else
#define COMPONENT_DETECT_GPIO		NULL
#define COMPONENT_DETECT_EINTSEL	NULL
#define COMPONENT_DETECT_EINTNUM	NULL
#define COMPONENT_DETECT_EINT		NULL
#endif

static struct clk *component_lcdc0_clk;
static struct clk *component_lcdc1_clk;

static char component_start = 0;
static char component_plugout = 0;
static char component_plugout_count = 0;
static char component_ext_interrupt = 0;

#if defined(CONFIG_CPU_FREQ_TCC92X) || defined (CONFIG_CPU_FREQ_TCC93XX)
extern struct tcc_freq_table_t gtTvClockLimitTable;
#endif

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
#define COMPONENT_EXCLUSIVE_UI_SCALER0		0
#define COMPONENT_EXCLUSIVE_UI_SCALER1		1
#define COMPONENT_EXCLUSIVE_UI_DVD_MAX_WD	720
#define COMPONENT_EXCLUSIVE_UI_DVD_MAX_HT	576
static unsigned int component_exclusive_ui_idx = 0;
static unsigned int component_exclusive_ui_onthefly = FALSE;
static exclusive_ui_params component_exclusive_ui_param;
#endif

/*****************************************************************************

 FUNCTIONS

******************************************************************************/

extern char TCC_FB_LCDC_NumSet(char NumLcdc, char OnOff);
extern void tca_lcdc_port_setting(char Nlcdc);
extern void LCDC_IO_Set(char lcdctrl_num, unsigned bit_per_pixel);

extern void tca_vsync_video_display_enable(void);
extern void tca_vsync_video_display_disable(void);
#if defined(TCC_VIDEO_DISPLAY_DEINTERLACE_MODE)
extern void tcc_vsync_viqe_deinitialize(void);
#endif /* TCC_VIDEO_DISPLAY_DEINTERLACE_MODE */
#if defined(TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
extern void tcc_vsync_set_firstFrameFlag(int firstFrameFlag);
#endif /* TCC_VIDEO_DISPLAY_BY_VSYNC_INT */


/*****************************************************************************
 Function Name : tcc_component_ext_handler()
******************************************************************************/
static irqreturn_t tcc_component_ext_handler(int irq, void *dev_id)
{
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwVPIC_BASE);
	
	dprintk("%s, component_plugout_count=%d\n", __func__, component_plugout_count);

	component_plugout_count++;
	if(component_plugout_count > 10)
	{
		component_plugout_count = 0;
		component_plugout = 1;

	#if defined(CONFIG_MACH_TCC9300ST)
        BITCLR(pHwPIC->INTMSK0, COMPONENT_DETECT_EINT);
        BITCLR(pHwPIC->IEN0, COMPONENT_DETECT_EINT);
		BITSET(pHwPIC->CLR0, COMPONENT_DETECT_EINT);
	#else
        BITCLR(pHwPIC->INTMSK1, COMPONENT_DETECT_EINT);
        BITCLR(pHwPIC->IEN1, COMPONENT_DETECT_EINT);
		BITSET(pHwPIC->CLR1, COMPONENT_DETECT_EINT);
	#endif
	}
	else
	{
	#if defined(CONFIG_MACH_TCC9300ST)
		BITSET(pHwPIC->CLR0, COMPONENT_DETECT_EINT);
	#else
		BITSET(pHwPIC->CLR1, COMPONENT_DETECT_EINT);
	#endif
	}

	return IRQ_HANDLED;
}

/*****************************************************************************
 Function Name : tcc_component_ext_interrupt_sel()
******************************************************************************/
void tcc_component_ext_interrupt_sel(char ext_int_num, char ext_int_sel)
{
#if defined(CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST)
	char shift_bit;
	#if defined(CONFIG_MACH_TCC9300ST)
	PGPIOINT pHwGPIOINT = (volatile PGPIOINT)tcc_p2v(HwEINTSEL_BASE);
	#else
	PGPIO pHwGPIOINT = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	#endif
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwVPIC_BASE);
	
	#if defined(CONFIG_MACH_TCC8800ST)
		if(ext_int_num >= 3)
			pHwPIC->EI37SEL = (pHwPIC->EI37SEL | (0x01<<ext_int_num));
	#endif

	if(ext_int_num < 4)
	{
		shift_bit = 8*(ext_int_num-0);
		BITCSET(pHwGPIOINT->EINTSEL0, 0x7F<<shift_bit, ext_int_sel<<shift_bit);
	}
	else if(ext_int_num < 8)
	{
		shift_bit = 8*(ext_int_num-4);
		BITCSET(pHwGPIOINT->EINTSEL1, 0x7F<<shift_bit, ext_int_sel<<shift_bit);
	}
	else if(ext_int_num < 12)
	{
		shift_bit = 8*(ext_int_num-8);
		BITCSET(pHwGPIOINT->EINTSEL2, 0x7F<<shift_bit, ext_int_sel<<shift_bit);
	}
	#if defined(CONFIG_MACH_TCC9300ST)
	else
	{
		shift_bit = 8*(ext_int_num-12);
		BITCSET(pHwGPIOINT->EINTSEL3, 0x7F<<shift_bit, ext_int_sel<<shift_bit);
	}
	#endif
#endif
}

/*****************************************************************************
 Function Name : tcc_component_set_ext_interrupt()
******************************************************************************/
void tcc_component_ext_interrupt_set(char onoff)
{
#if defined(CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST)
	int ret, irq_num;
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwVPIC_BASE);
	
	component_plugout_count = 0;

	#if defined(CONFIG_MACH_TCC9300ST)
		irq_num = IRQ_EI5;
	#else
		irq_num = INT_EI6;
	#endif
	
	if(onoff)
	{
		if(component_ext_interrupt == 1)
			return;

		tcc_component_ext_interrupt_sel(COMPONENT_DETECT_EINTNUM, COMPONENT_DETECT_EINTSEL);

	#if defined(CONFIG_MACH_TCC9300ST)
		BITCLR(pHwPIC->POL0, COMPONENT_DETECT_EINT);
		BITCLR(pHwPIC->MODE0, COMPONENT_DETECT_EINT);
		BITCLR(pHwPIC->MODEA0, COMPONENT_DETECT_EINT);
		BITSET(pHwPIC->SEL0, COMPONENT_DETECT_EINT);
	#else
		BITCLR(pHwPIC->POL1, COMPONENT_DETECT_EINT);
		BITCLR(pHwPIC->MODE1, COMPONENT_DETECT_EINT);
		BITCLR(pHwPIC->MODEA1, COMPONENT_DETECT_EINT);
		BITSET(pHwPIC->SEL1, COMPONENT_DETECT_EINT);
	#endif

		if(ret = request_irq(irq_num, tcc_component_ext_handler, IRQF_SHARED, "TCC_COMPONENT_EXT", tcc_component_ext_handler))	
		{
			printk("%s, ret=%d, irq_num=%d, request_irq ERROR!\n", __func__, ret, irq_num);
		}

	#if defined(CONFIG_MACH_TCC9300ST)
        BITSET(pHwPIC->CLR0, COMPONENT_DETECT_EINT);
		BITSET(pHwPIC->INTMSK0, COMPONENT_DETECT_EINT);	
        BITSET(pHwPIC->IEN0, COMPONENT_DETECT_EINT);
	#else
        BITSET(pHwPIC->CLR1, COMPONENT_DETECT_EINT);
		BITSET(pHwPIC->INTMSK1, COMPONENT_DETECT_EINT);	
        BITSET(pHwPIC->IEN1, COMPONENT_DETECT_EINT);
	#endif

		component_ext_interrupt = 1;
	}
	else
	{
		if(component_ext_interrupt == 0)
			return;

		free_irq(irq_num, tcc_component_ext_handler);
		
	#if defined(CONFIG_MACH_TCC9300ST)
        BITCLR(pHwPIC->INTMSK0, COMPONENT_DETECT_EINT);
        BITCLR(pHwPIC->IEN0, COMPONENT_DETECT_EINT);
        BITSET(pHwPIC->CLR0, COMPONENT_DETECT_EINT);
	#else
        BITCLR(pHwPIC->INTMSK1, COMPONENT_DETECT_EINT);
        BITCLR(pHwPIC->IEN1, COMPONENT_DETECT_EINT);
        BITSET(pHwPIC->CLR1, COMPONENT_DETECT_EINT);
	#endif

		component_ext_interrupt = 0;
	}

	dprintk("%s, onoff=%d\n", __func__, onoff);
#endif
}

/*****************************************************************************
 Function Name : tcc_component_detect()
******************************************************************************/
int tcc_component_detect(void)
{
	int detect = true;

	#if defined (CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST)
		#if defined(CONFIG_TCC_OUTPUT_AUTO_DETECTION)
			if(component_plugout)
			{
				if(component_ext_interrupt == 1)
					tcc_component_ext_interrupt_set(FALSE);

				detect = false;
			}
			else
			{
				if(component_ext_interrupt)
				{
				
					detect = true;
				}
				else
				{
					detect = gpio_get_value(COMPONENT_DETECT_GPIO)? false : true;

					if(detect)
					{
						if(component_ext_interrupt == 0)
							tcc_component_ext_interrupt_set(TRUE);
			 		}

					dprintk("%s, detect=%d\n", __func__, detect);
				}
			}
		#endif
	#endif

	return detect;
}

/*****************************************************************************
 Function Name : tcc_component_lcdc_handler()
******************************************************************************/
static irqreturn_t tcc_component_lcdc_handler(int irq, void *dev_id)
{
	PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	
#if defined(CONFIG_ARCH_TCC93XX)
	#if defined(CONFIG_MACH_TCC9300ST)
		BITCLR(pGPIO->GPCFN3, Hw4-Hw0);
		BITSET(pGPIO->GPCEN, Hw24);

		if(Component_FID)		
			BITCLR(pGPIO->GPCDAT, Hw24);
		else
			BITSET(pGPIO->GPCDAT, Hw24);
	#endif
#endif

	Component_FID = !Component_FID;

	//dprintk("%s - Component_FID:%s\n", __func__, Component_FID? "high":"low");

	return IRQ_HANDLED;
}

/*****************************************************************************
 Function Name : tcc_component_check_handler()
******************************************************************************/
int tcc_component_lcdc_handler_check(char lcdc_num)
{
	//dprintk("%s\n", __func__);
	
	if((component_start == 1) && (Component_Mode == COMPONENT_MODE_1080I))
	{
		if(lcdc_num == Component_LCDC_Num)
			return 1;
	}

	return 0;
}

/*****************************************************************************
 Function Name : tcc_component_lcdc_interrupt_set()
******************************************************************************/
void tcc_component_lcdc_interrupt_set(char onoff)
{
	int ret;

	dprintk("%s, onoff=%d\n", __func__, onoff);
	
	if(onoff)
	{
		tca_lcdc_interrupt_onoff(TRUE, Component_LCDC_Num);
		if(Component_LCDC_Num)
			ret = request_irq(IRQ_LCD1, tcc_component_lcdc_handler, IRQF_SHARED, "TCC_COMPONENT_LCD1", tcc_component_lcdc_handler);
		else
			ret = request_irq(IRQ_LCD0, tcc_component_lcdc_handler, IRQF_SHARED, "TCC_COMPONENT_LCD0", tcc_component_lcdc_handler);
	}
	else
	{
		tca_lcdc_interrupt_onoff(FALSE, Component_LCDC_Num);
		if(Component_LCDC_Num)
			free_irq(IRQ_LCD1, tcc_component_lcdc_handler);
		else
			free_irq(IRQ_LCD0, tcc_component_lcdc_handler);
	}
}

/*****************************************************************************
 Function Name : tcc_component_set_lcdc()
******************************************************************************/
int tcc_component_set_lcdc(int lcdc_num)
{
	dprintk("%s - new_lcdc:%d, before_lcdc:%d\n", __func__, lcdc_num, Component_LCDC_Num);

	if(lcdc_num == Component_LCDC_Num)
		return FALSE;
	
	if(lcdc_num)
	{
		Component_LCDC_Num = 1;
		pComponent_HwLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
		pComponent_HwLCDC_CH = (volatile PLCDC_CHANNEL)tcc_p2v(pComponent_HwLCDC->LI2C);
	}
	else
	{
		Component_LCDC_Num = 0;
		pComponent_HwLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
		pComponent_HwLCDC_CH = (volatile PLCDC_CHANNEL)tcc_p2v(pComponent_HwLCDC->LI0C);
	}
	
	return TRUE;
}

/*****************************************************************************
 Function Name : tcc_component_get_spec()
******************************************************************************/
void tcc_component_get_spec(COMPONENT_MODE_TYPE mode, COMPONENT_SPEC_TYPE *spec)
{
	switch(mode)
	{
		case COMPONENT_MODE_NTSC_M:
		case COMPONENT_MODE_NTSC_M_J:
		case COMPONENT_MODE_PAL_M:
		case COMPONENT_MODE_NTSC_443:
		case COMPONENT_MODE_PSEUDO_PAL:
			spec->component_clk = 270000;
			spec->component_bus_width = 8;
			spec->component_lcd_width = 720;
			spec->component_lcd_height = 480;
			spec->component_LPW = 128 - 1; 					// line pulse width
			spec->component_LPC = 720 * 2 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 116 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 32 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay

			spec->component_FPW1 = 6 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 480 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 30 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 9 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 6 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 480 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 31 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 8 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;
				
		case COMPONENT_MODE_NTSC_N:
		case COMPONENT_MODE_NTSC_N_J:
		case COMPONENT_MODE_PAL_N:
		case COMPONENT_MODE_PAL_B:
		case COMPONENT_MODE_PAL_G:
		case COMPONENT_MODE_PAL_H:
		case COMPONENT_MODE_PAL_I:
		case COMPONENT_MODE_PSEUDO_NTSC:
			spec->component_clk = 270000;
			spec->component_bus_width = 8;
			spec->component_lcd_width = 720;
			spec->component_lcd_height = 576;
			spec->component_LPW = 128 - 1; 					// line pulse width
			spec->component_LPC = 720 * 2 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 136 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 24 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay

			spec->component_FPW1 = 5 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 576 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 39 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 5 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 5 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 576 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 40 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 4 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;

		case COMPONENT_MODE_720P:
			spec->component_clk = 742500;
			spec->component_bus_width = 24;
			spec->component_lcd_width = 1280;
			spec->component_lcd_height = 720;
			spec->component_LPW = 24 - 1; 					// line pulse width
			spec->component_LPC = 1280 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 325 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 21 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay
				
			spec->component_FPW1 = 3 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 720 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 26 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 1 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 3 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 720 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 26 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 1 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;

		case COMPONENT_MODE_1080I:
			spec->component_clk = 742500;
			spec->component_bus_width = 24;
			spec->component_lcd_width = 1920;
			spec->component_lcd_height = 1080;
			spec->component_LPW = 24 - 1; 					// line pulse width
			spec->component_LPC = 1920 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->component_LSWC = 254 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->component_LEWC = 2 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->component_VDB = 0; 						// Back porch Vsync delay
			spec->component_VDF = 0; 						// front porch of Vsync delay
				
			spec->component_FPW1 = 5*2 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC1 = 540*2 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC1 = 15*2 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC1 = 2.5*2 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->component_FPW2 = 5*2 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->component_FLC2 = 540*2 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->component_FSWC2 = 15.5*2 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->component_FEWC2 = 2*2 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;
	}
}

/*****************************************************************************
 Function Name : tcc_component_set_lcd2tv()
******************************************************************************/
void tcc_component_set_lcd2tv(COMPONENT_MODE_TYPE mode)
{
	COMPONENT_SPEC_TYPE spec;
	unsigned int lcd_ctrl = 0;
	unsigned int lcd_peri = 0;
	PDDICONFIG pDDICfg = (PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	dprintk("%s\n", __func__);
	
	if(Component_LCDC_Num)
	{
		BITSET(pDDICfg->SWRESET, Hw3);	// LCDC1
		BITCLR(pDDICfg->SWRESET, Hw3);	// LCDC1
		lcd_peri = PERI_LCD1;
	}
	else
	{
		BITSET(pDDICfg->SWRESET, Hw2);	// LCDC0
		BITCLR(pDDICfg->SWRESET, Hw2);	// LCDC0
		lcd_peri = PERI_LCD0;
	}
		
	tcc_component_get_spec(mode, &spec);
	
	LCDC_IO_Set(Component_LCDC_Num, spec.component_bus_width);

	#if (defined(CONFIG_CPU_FREQ_TCC92X) || defined (CONFIG_CPU_FREQ_TCC93XX)) && defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtTvClockLimitTable, TCC_FREQ_LIMIT_TV, 1);
    
	#endif

	#if defined(CONFIG_ARCH_TCC88XX)
		tca_ckc_setperi(lcd_peri,ENABLE,spec.component_clk,PCDIRECTPLL0);
	#else
		tca_ckc_setperi(lcd_peri,ENABLE,spec.component_clk,PCDIRECTPLL2);
	#endif

	#if defined(CONFIG_ARCH_TCC92XX) && defined(CONFIG_TCC_OUTPUT_STARTER)
	{
		int wait_count = 0;

		BITCLR(pComponent_HwLCDC->LCTRL, HwLCTRL_LEN);
		while(ISSET(pComponent_HwLCDC->LSTATUS, HwLSTATUS_BUSY))
		{
			msleep(1);
			wait_count++;
			if(wait_count > 100)
			{
				break;
			}
		}
	}
	#endif

	dprintk("PLL2 Clock: %d KHz, LCDC0 Clock: %d KHz, LCDC1 Clock: %d KHz\n", tca_ckc_getpll(DIRECTPLL2)/10, tca_ckc_getperi(PERI_LCD0)/10, tca_ckc_getperi(PERI_LCD1)/10);

	BITCSET(pComponent_HwLCDC->LCLKDIV, 0x00FF0000, 0<<16);
	BITCSET(pComponent_HwLCDC->LCLKDIV, 0x000000FF, 0);

	BITCSET(pComponent_HwLCDC->LHTIME1, 0x0003FFFF, spec.component_LPC);
	BITCSET(pComponent_HwLCDC->LHTIME1, 0x01FF0000, (spec.component_LPW << 16));
	BITCSET(pComponent_HwLCDC->LHTIME2, 0x000001FF, spec.component_LEWC);
	BITCSET(pComponent_HwLCDC->LHTIME2, 0x01FF0000, (spec.component_LSWC << 16));
	
	BITCSET(pComponent_HwLCDC->LVTIME1, 0x00003FFF, spec.component_FLC1);
	BITCSET(pComponent_HwLCDC->LVTIME1, 0x003F0000, (spec.component_FPW1 << 16));
	BITCSET(pComponent_HwLCDC->LVTIME1, 0x03C00000, (spec.component_VDF << 22));
	BITCSET(pComponent_HwLCDC->LVTIME1, 0xF8000000, (spec.component_VDB << 27));
	
	BITCSET(pComponent_HwLCDC->LVTIME2, 0x01FF0000, (spec.component_FSWC1 << 16));
	BITCSET(pComponent_HwLCDC->LVTIME2, 0x000001FF, spec.component_FEWC1);
	
	BITCSET(pComponent_HwLCDC->LVTIME3, 0x00003FFF, spec.component_FLC2);
	BITCSET(pComponent_HwLCDC->LVTIME3, 0x003F0000, (spec.component_FPW2 << 16));
	
	BITCSET(pComponent_HwLCDC->LVTIME4, 0x01FF0000, (spec.component_FSWC2 << 16));
	BITCSET(pComponent_HwLCDC->LVTIME4, 0x000001FF, spec.component_FEWC2);
	
	BITCSET(pComponent_HwLCDC->LDS, 0x0000FFFF, spec.component_lcd_width);
	BITCSET(pComponent_HwLCDC->LDS, 0xFFFF0000, (spec.component_lcd_height << 16));

	switch(mode)
	{
		case COMPONENT_MODE_NTSC_M:
		case COMPONENT_MODE_NTSC_M_J:
		case COMPONENT_MODE_PAL_M:
		case COMPONENT_MODE_NTSC_443:
		case COMPONENT_MODE_PSEUDO_PAL:
		case COMPONENT_MODE_NTSC_N:
		case COMPONENT_MODE_NTSC_N_J:
		case COMPONENT_MODE_PAL_N:
		case COMPONENT_MODE_PAL_B:
		case COMPONENT_MODE_PAL_G:
		case COMPONENT_MODE_PAL_H:
		case COMPONENT_MODE_PAL_I:
		case COMPONENT_MODE_PSEUDO_NTSC:
			BITCSET(lcd_ctrl, Hw29|Hw28, Hw29|Hw28);				// R2YMD - RGB to YCbCr Conversion Option
			BITCSET(lcd_ctrl, Hw23, Hw23);							// CKG - Clock Gating Enable for Timing Generator
			BITCSET(lcd_ctrl, Hw15, Hw15);							// ID - Inverted Data Enable
			BITCSET(lcd_ctrl, Hw14, Hw14);							// IV - Inverted Vertical Sync
			BITCSET(lcd_ctrl, Hw13, Hw13);							// IH - Inverted Horizontal Sync
			BITCSET(lcd_ctrl, Hw12, Hw12);							// IP - Inverted Pixel Clock
			BITCSET(lcd_ctrl, Hw10, Hw10);							// R2Y - RGB to YCbCr Converter Enable for OUPUT
			BITCSET(lcd_ctrl, Hw7, Hw7);							// TV - TV mode
			BITCSET(lcd_ctrl, Hw3|Hw2|Hw1, Hw3|Hw1);				// OVP
			BITCSET(lcd_ctrl, Hw0, Hw0);							// LEN - LCD Controller Enable
			#ifdef TCC_COMPONENT_CCIR656
			BITCSET(lcd_ctrl, Hw24, Hw24);							// 656 - CCIR 656 Mode
			BITCSET(lcd_ctrl, Hw19|Hw18|Hw17|Hw16, Hw19);			// PXDW - Pixel Data Width
			BITCSET(lcd_ctrl, Hw9, Hw9);							// DP - Double Pixel Data
			#else
			BITCSET(lcd_ctrl, Hw19|Hw18|Hw17|Hw16, Hw18|Hw17);		// PXDW - Pixel Data Width
			#endif
			break;

		case COMPONENT_MODE_720P:
			BITCSET(lcd_ctrl, Hw29|Hw28, Hw29|Hw28);				// R2YMD - RGB to YCbCr Conversion Option
			BITCSET(lcd_ctrl, Hw23, Hw23);							// CKG - Clock Gating Enable for Timing Generator
			#if defined(CONFIG_MACH_TCC9300ST)
//			BITCSET(lcd_ctrl, Hw15, Hw15);							// ID - Inverted Data Enable
			BITCSET(lcd_ctrl, Hw14, Hw14);							// IV - Inverted Vertical Sync
			BITCSET(lcd_ctrl, Hw13, Hw13);							// IH - Inverted Horizontal Sync
//			BITCSET(lcd_ctrl, Hw12, Hw12);							// IP - Inverted Pixel Clock
			#else
			BITCSET(lcd_ctrl, Hw15, Hw15);							// ID - Inverted Data Enable
			BITCSET(lcd_ctrl, Hw14, Hw14);							// IV - Inverted Vertical Sync
//			BITCSET(lcd_ctrl, Hw13, Hw13);							// IH - Inverted Horizontal Sync
			BITCSET(lcd_ctrl, Hw12, Hw12);							// IP - Inverted Pixel Clock
			#endif
			BITCSET(lcd_ctrl, Hw8, Hw8);							// NI - Non-interlace Mode
			BITCSET(lcd_ctrl, Hw3|Hw2|Hw1, Hw3|Hw1);				// OVP
			BITCSET(lcd_ctrl, Hw0, Hw0);							// LEN - LCD Controller Enable
			#ifdef TCC_COMPONENT_16BPP_YCBCR
			BITCSET(lcd_ctrl, Hw10, Hw10);							// R2Y - RGB to YCbCr Converter Enable for OUPUT
			BITCSET(lcd_ctrl, Hw19|Hw18|Hw17|Hw16, Hw19);			// PXDW - Pixel Data Width
			#else
			BITCSET(lcd_ctrl, Hw19|Hw18|Hw17|Hw16, Hw19|Hw18);		// PXDW - Pixel Data Width
			#endif
			break;

		case COMPONENT_MODE_1080I:
			BITCSET(lcd_ctrl, Hw29|Hw28, Hw29|Hw28);				// R2YMD - RGB to YCbCr Conversion Option
			BITCSET(lcd_ctrl, Hw23, Hw23);							// CKG - Clock Gating Enable for Timing Generator
//			BITCSET(lcd_ctrl, Hw15, Hw15);							// ID - Inverted Data Enable
//			BITCSET(lcd_ctrl, Hw14, Hw14);							// IV - Inverted Vertical Sync
			BITCSET(lcd_ctrl, Hw13, Hw13);							// IH - Inverted Horizontal Sync
			BITCSET(lcd_ctrl, Hw12, Hw12);							// IP - Inverted Pixel Clock
			BITCSET(lcd_ctrl, Hw7, Hw7);							// TV
			BITCSET(lcd_ctrl, Hw3|Hw2|Hw1, Hw3|Hw1);				// OVP
			BITCSET(lcd_ctrl, Hw0, Hw0);							// LEN - LCD Controller Enable
			#ifdef TCC_COMPONENT_16BPP_YCBCR
			BITCSET(lcd_ctrl, Hw10, Hw10);							// R2Y - RGB to YCbCr Converter Enable for OUPUT
			BITCSET(lcd_ctrl, Hw19|Hw18|Hw17|Hw16, Hw19);			// PXDW - Pixel Data Width
			#else
			BITCSET(lcd_ctrl, Hw19|Hw18|Hw17|Hw16, Hw19|Hw18);		// PXDW - Pixel Data Width
			#endif
			break;

		default:
			break;
	}

	BITCSET(pComponent_HwLCDC->LCTRL, 0xFFFFFFFF, lcd_ctrl);
}

/*****************************************************************************
 Function Name : tcc_component_get_lcdsize()
******************************************************************************/
void tcc_component_get_lcdsize(unsigned int *width, unsigned int *height)
{
	unsigned int lcdsize;
		
	lcdsize = pComponent_HwLCDC->LDS;

	*width = lcdsize & 0x0000FFFF;
	*height = lcdsize >>16;
}

/*****************************************************************************
 Function Name : tcc_component_change_formattype()
******************************************************************************/
COMPONENT_LCDC_IMG_FMT_TYPE tcc_component_change_formattype(TCC_COMPONENT_FORMAT_TYPE g2d_format)
{
	COMPONENT_LCDC_IMG_FMT_TYPE LCDC_fmt = COMPONENT_LCDC_IMG_FMT_MAX;

	switch(g2d_format)
	{
		case GE_YUV444_sp:
			break;

		case GE_YUV440_sp:
			break;
				
		case GE_YUV422_sp: 
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_YUV422SP;
			break;
				
		case GE_YUV420_sp:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_YUV420SP;
			break;

		case GE_YUV411_sp:
		case GE_YUV410_sp:
			break;

		case GE_YUV422_in:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_YUV422ITL0;
			break;

		case GE_YUV420_in:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_YUV420ITL0;
			break;

		case GE_YUV444_sq:
			break;
		case GE_YUV422_sq:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_YUV422SQ;
			break;

		case GE_RGB332:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_RGB332;
			break;
				
		case GE_RGB444:
		case GE_ARGB4444:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_RGB444;
			break;

		case GE_RGB454:
		case GE_ARGB3454:
			break;

		case GE_RGB555:
		case GE_ARGB1555:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_RGB555;
			break;
		case GE_RGB565:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_RGB565;
			break;
			
		case GE_RGB666:
		case GE_ARGB4666:
		case GE_ARGB6666:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_RGB666;
			break;

		case GE_RGB888:
		case GE_ARGB4888:
		case GE_ARGB8888:
			LCDC_fmt = COMPONENT_LCDC_IMG_FMT_RGB888;
			break;
	}

	return LCDC_fmt;
}

/*****************************************************************************
 Function Name: tcc_component_set_imagebase()
 Parameters
		: nType		: IMG_CH0, IMG_CH1, IMG_CH2
		: nBase0	: Base Address for Y Channel or RGB Channel
					: Valid for 3 Channel
		: nBase1	: Base Address for Cb(U)
					: Valid for IMG_CH0
		: nBase2	: Base Address for Cb(U) Channel
					: Valid for IMG_CH0
******************************************************************************/
void tcc_component_set_imagebase(unsigned int type, unsigned int base0, unsigned int base1, unsigned int base2)
{
	switch (type) 
	{
		case	IMG_CH0 :
			BITCSET(pComponent_HwLCDC->LI0BA0, 0xFFFFFFFF, base0);
			BITCSET(pComponent_HwLCDC->LI0BA1, 0xFFFFFFFF, base1);
			BITCSET(pComponent_HwLCDC->LI0BA2, 0xFFFFFFFF, base2);			
			break;

		case	IMG_CH1 :
			BITCSET(pComponent_HwLCDC->LI1BA0, 0xFFFFFFFF, base0);
			break;

		case	IMG_CH2 :
			BITCSET(pComponent_HwLCDC->LI2BA0, 0xFFFFFFFF, base0);
			break;

		default	:
			break;
	}
}

/*****************************************************************************
 Function Name : tcc_component_get_offset()
******************************************************************************/
void tcc_component_get_offset(COMPONENT_LCDC_IMG_FMT_TYPE fmt, unsigned int width, unsigned int *offset0, unsigned int *offset1)
{
	switch(fmt)
	{
		case COMPONENT_LCDC_IMG_FMT_1BPP:
		case COMPONENT_LCDC_IMG_FMT_2BPP:
		case COMPONENT_LCDC_IMG_FMT_4BPP:
		case COMPONENT_LCDC_IMG_FMT_8BPP:
		case COMPONENT_LCDC_IMG_FMT_RGB332:
			*offset0 = width;
			*offset1 = 0;
			break;
			
		case COMPONENT_LCDC_IMG_FMT_RGB444:
		case COMPONENT_LCDC_IMG_FMT_RGB565:
		case COMPONENT_LCDC_IMG_FMT_RGB555:
			*offset0 = width * 2;
			*offset1 = 0;
			break;

		case COMPONENT_LCDC_IMG_FMT_RGB888:
		case COMPONENT_LCDC_IMG_FMT_RGB666:
			*offset0 = width * 4;
			*offset1 = 0;
			break;

		case COMPONENT_LCDC_IMG_FMT_YUV420SP:
		case COMPONENT_LCDC_IMG_FMT_YUV422SP:
			*offset0 = width;
			*offset1 = width/2;
			break;
		case COMPONENT_LCDC_IMG_FMT_YUV422SQ:
			*offset0 = width * 2;
			*offset1 = 0;
			break;

		case COMPONENT_LCDC_IMG_FMT_YUV420ITL0:
		case COMPONENT_LCDC_IMG_FMT_YUV420ITL1:
		case COMPONENT_LCDC_IMG_FMT_YUV422ITL0:
		case COMPONENT_LCDC_IMG_FMT_YUV422ITL1:
			*offset0 = width;
			*offset1 = width;
			break;
	}
}

/*****************************************************************************
 Function Name: tcc_component_set_imageinfo()
 Parameters
		: nType		: IMG_CH0, IMG_CH1, IMG_CH2
		: nHPos		: Horizontal Position
		: nVPos		: Vertical Position
		: nOffset0	: Horizontal Pixel Offset for Y Channel or RGB Channel
		: nOffset1	: Horizontal Pixel Offset for UV Channel
					: Valid for IMG_CH0
		: nHScale	: Horizontal Scale Factor
		: nVScale	: Vertical Scale Factor
		: nFormat	: Endian / YUV / BPP Setting
					: [3:0]	: BPP Information
					: [6:4]	: YUV Format Information
					: [7]		: Endian Information
******************************************************************************/
unsigned int tcc_component_set_imageinfo (unsigned int flag, unsigned int type, unsigned int width, unsigned int height, unsigned int hpos, unsigned int vpos, unsigned int hoffset0, unsigned int hoffset1, unsigned int hscale, unsigned int vscale)
{
	unsigned int uiCH0Position = 0;
	
	switch (type) 
	{
		case IMG_CH0 :
			if(flag & SET_IMAGE_SIZE)
			{
				BITCSET(pComponent_HwLCDC->LI0S, 0xFFFF0000, (height << 16));
				BITCSET(pComponent_HwLCDC->LI0S, 0x0000FFFF, width);
			}

			if(flag & READ_IMAGE_POSITION)
			{
				uiCH0Position = (unsigned int)pComponent_HwLCDC->LI0P;
			}

			if(flag & SET_IMAGE_POSITION)
			{
				BITCSET(pComponent_HwLCDC->LI0P, 0x0000FFFF, hpos);
				BITCSET(pComponent_HwLCDC->LI0P, 0xFFFF0000, (vpos << 16));
			}	

			if(flag & SET_IMAGE_OFFSET)
			{
				BITCSET(pComponent_HwLCDC->LI0O, 0x0000FFFF, hoffset0);
				BITCSET(pComponent_HwLCDC->LI0O, 0xFFFF0000, (hoffset1 << 16));
			}	

			if(flag & SET_IMAGE_SCALE)
			{
				BITCSET(pComponent_HwLCDC->LI0SR, HwLISCALE_X, hscale);
				BITCSET(pComponent_HwLCDC->LI0SR, HwLISCALE_Y, (vscale << 4));
			}
			break;
			
		case IMG_CH1 :
			if(flag & SET_IMAGE_SIZE)
			{
				BITCSET(pComponent_HwLCDC->LI1S, 0xFFFF0000, (height << 16));
				BITCSET(pComponent_HwLCDC->LI1S, 0x0000FFFF, width);
			}	

			if(flag & SET_IMAGE_POSITION)
			{
				BITCSET(pComponent_HwLCDC->LI1P, 0x0000FFFF, hpos);
				BITCSET(pComponent_HwLCDC->LI1P, 0xFFFF0000, (vpos << 16));
			}	

			if(flag & SET_IMAGE_OFFSET)
			{
				BITCSET(pComponent_HwLCDC->LI1O, 0x0000FFFF, hoffset0);
			}	

			if(flag & SET_IMAGE_SCALE)
			{
				BITCSET(pComponent_HwLCDC->LI1SR, 0x0000000F, hscale);
				BITCSET(pComponent_HwLCDC->LI1SR, 0x000000F0, (vscale << 4));
			}	
			break;
			
		case IMG_CH2 :
			if(flag & SET_IMAGE_SIZE)
			{
				BITCSET(pComponent_HwLCDC->LI2S, 0xFFFF0000, (height << 16));
				BITCSET(pComponent_HwLCDC->LI2S, 0x0000FFFF, width);
			}

			if(flag & SET_IMAGE_POSITION)
			{
				BITCSET(pComponent_HwLCDC->LI2P, 0x0000FFFF, hpos);
				BITCSET(pComponent_HwLCDC->LI2P, 0xFFFF0000, (vpos << 16));
			}

			if(flag & SET_IMAGE_OFFSET)
			{
				BITCSET(pComponent_HwLCDC->LI2O, 0x0000FFFF, hoffset0);
			}

			if(flag & SET_IMAGE_SCALE)
			{
				BITCSET(pComponent_HwLCDC->LI2SR, 0x0000000F, hscale);
				BITCSET(pComponent_HwLCDC->LI2SR, 0x000000F0, (vscale << 4));
			}
			break;

		default	:
			break;
	}
	return uiCH0Position;
}

/*****************************************************************************
 Function Name : tcc_component_set_imagectrl()
******************************************************************************/
void tcc_component_set_imagectrl(unsigned int flag, unsigned int type, COMPONENT_LCDC_IMG_CTRL_TYPE *imgctrl)
{
	unsigned int IMG_Ctrl_reg;
	
	switch (type) 
	{
		case IMG_CH0:
			IMG_Ctrl_reg = pComponent_HwLCDC->LI0C;
			break;

		case IMG_CH1:
			IMG_Ctrl_reg = pComponent_HwLCDC->LI1C;
			break;

		case IMG_CH2:
			IMG_Ctrl_reg = pComponent_HwLCDC->LI2C;
			break;

		default:
			return;
	}

	if(flag & SET_IMAGE_INTL)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_INTL, (imgctrl->INTL<<31));
	}
	if(flag & SET_IMAGE_AEN)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_AEN, (imgctrl->AEN<<30));
	}
	if(flag & SET_IMAGE_CEN)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_CEN, (imgctrl->CEN<<29));
	}
	if(flag & SET_IMAGE_IEN)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_IEN, (imgctrl->IEN<<28));
	}
	if(flag & SET_IMAGE_SRC)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_SRC, (imgctrl->SRC<<27));
	}			
	if(flag & SET_IMAGE_AOPT)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_AOPT, (imgctrl->AOPT<<25));
	}
	if(flag & SET_IMAGE_ASEL)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_ASEL, (imgctrl->ASEL<<24));
	}
	if(flag & SET_IMAGE_UPD)
	{
		#if defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
		BITCSET(IMG_Ctrl_reg, HwLCT_RU, (imgctrl->UPD<<16));
		#endif
	}
	if(flag & SET_IMAGE_PD)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_PD, (imgctrl->PD<<15));
	}
	if(flag & SET_IMAGE_Y2RMD)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_Y2RMD, (imgctrl->Y2RMD<<9));
	}
	if(flag & SET_IMAGE_Y2R)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_Y2R, (imgctrl->Y2R<<8));
	}
	if(flag & SET_IMAGE_BR)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_BR, (imgctrl->BR<<7));
	}
	if(flag & SET_IMAGE_FMT)
	{
		BITCSET(IMG_Ctrl_reg, HwLIC_FMT, (imgctrl->FMT));
	}

	switch (type) 
	{
		case IMG_CH0:
			pComponent_HwLCDC->LI0C = IMG_Ctrl_reg;
			break;

		case IMG_CH1:
			pComponent_HwLCDC->LI1C = IMG_Ctrl_reg;
			break;

		case IMG_CH2:
			pComponent_HwLCDC->LI2C = IMG_Ctrl_reg;
			break;

		default:
			return;
	}
}

/*****************************************************************************
 Function Name : tcc_component_update()
******************************************************************************/
void tcc_component_update(struct tcc_lcdc_image_update *update)
{
	COMPONENT_LCDC_IMG_CTRL_TYPE ImgCtrl;
	unsigned int Y_offset, UV_offset;
	unsigned int img_width = 0, img_height = 0, win_offset_x = 0, win_offset_y = 0;
	unsigned int lcd_w = 0, lcd_h =0;
	unsigned int lcd_ctrl_flag;
	
	dprintk("%s  \n", __func__);

	tcc_component_get_lcdsize(&lcd_w, &lcd_h);

	//printk("lcd_w : %d, lcd_h %d \n", lcd_w, lcd_h);
	//printk("update->addr0 : %x, update->addr1 : %x, update->addr2  : %x \n",update->addr0, update->addr1, update->addr2 );

	// Set LCD Image channel 0 
	memset((void*)&ImgCtrl, NULL, sizeof(COMPONENT_LCDC_IMG_CTRL_TYPE));

	if(Component_Mode == COMPONENT_MODE_720P)
 		ImgCtrl.INTL = FALSE;
	else
	{
	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
 		if(component_exclusive_ui_param.interlace && !TCC_OUTPUT_EXCLUSIVE_UI_GetBypass())
			ImgCtrl.INTL = FALSE;
		else
			ImgCtrl.INTL = TRUE;
	#else
		ImgCtrl.INTL = TRUE;
	#endif
	}

	ImgCtrl.IEN = update->enable;
	ImgCtrl.FMT = update->fmt;

	if(ImgCtrl.FMT >= TCC_LCDC_IMG_FMT_YUV420SP)
		ImgCtrl.Y2R = TRUE;
	else
		ImgCtrl.Y2R = FALSE;

	ImgCtrl.UPD = TRUE;
	
	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		/* Get the parameters for exclusive ui */
		TCC_OUTPUT_EXCLUSIVE_UI_GetParam(&component_exclusive_ui_param);

		if(component_exclusive_ui_param.enable && component_exclusive_ui_onthefly)
		{
			ImgCtrl.SRC = TRUE;
		}
		else
		{
			ImgCtrl.SRC = FALSE;
			tcc_component_set_imagebase(IMG_CH0, (unsigned int)update->addr0, (unsigned int)update->addr1, (unsigned int)update->addr2);
		}
		tcc_component_get_offset(ImgCtrl.FMT, update->Frame_width, &Y_offset, &UV_offset);
		component_exclusive_ui_onthefly = FALSE;
	#else
		tcc_component_set_imagebase(IMG_CH0, (unsigned int)update->addr0, (unsigned int)update->addr1, (unsigned int)update->addr2);
		tcc_component_get_offset(ImgCtrl.FMT, update->Frame_width, &Y_offset, &UV_offset);
	#endif
	
	if(update->Image_width > lcd_w)
	{
		win_offset_x = 0;
		img_width = lcd_w;
	}
	else
	{
		img_width = update->Image_width;
		win_offset_x = (lcd_w - img_width)/2;
	}

	if(update->Image_height > lcd_h)
	{
		win_offset_y = 0;
		img_height = lcd_h;
	}
	else
	{
		img_height = update->Image_height;
		win_offset_y = (lcd_h - img_height)/2; 
	}
		
	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		win_offset_x = update->offset_x;
		win_offset_y =  update->offset_y; 
	#else
		if(update->offset_x)
			win_offset_x = update->offset_x;

		if(update->offset_y)
			win_offset_y =  update->offset_y; 
	#endif
	
	if(Component_Mode == COMPONENT_MODE_1080I)
		win_offset_y = win_offset_y/2;

	dprintk("%s, img_widht:%d, img_height:%d, win_offset_x:%d, win_offset_y:%d\n", __func__, img_width, img_height, win_offset_x, win_offset_y);
	
	lcd_ctrl_flag = SET_IMAGE_INTL |
					SET_IMAGE_UPD |
					SET_IMAGE_IEN |
					SET_IMAGE_SRC |
					SET_IMAGE_Y2R |
					SET_IMAGE_FMT |
					SET_IMAGE_CEN |
					SET_IMAGE_AEN |
					SET_IMAGE_ASEL;

	tcc_component_set_imageinfo(SET_IMAGE_ALL, IMG_CH0, img_width, img_height, win_offset_x, win_offset_y, Y_offset, UV_offset, 0, 0);
	tcc_component_set_imagectrl(lcd_ctrl_flag, IMG_CH0, &ImgCtrl);

	BITCSET(pComponent_HwLCDC->LCTRL, HwLCTRL_LEN, HwLCTRL_LEN);
}

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
/*****************************************************************************
 Function Name : tcc_component_process()
******************************************************************************/
void tcc_component_process(struct tcc_lcdc_image_update *update, char force_update)
{
	COMPONENT_LCDC_IMG_CTRL_TYPE ImgCtrl;
	unsigned int Y_offset, UV_offset;
	unsigned int img_width = 0, img_height = 0, win_offset_x = 0, win_offset_y = 0;
	unsigned int lcd_w = 0, lcd_h =0;
	unsigned int lcd_ctrl_flag, mixer_interrupt_use = 0;
	exclusive_ui_ar_params aspect_ratio;
	PLCDC pLCDC;
	
	if(Component_LCDC_Num)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
		
	tcc_component_get_lcdsize(&lcd_w, &lcd_h);

	img_width = update->Image_width;
	img_height = update->Image_height;

	dprintk( "%s, img_w=%d, img_h=%d, lcd_w=%d, lcd_h=%d, plane_w=%d, plane_ht=%d\n", __func__,
			img_width, img_height, lcd_w, lcd_h, component_exclusive_ui_param.plane_width, component_exclusive_ui_param.plane_height);

	/* Aspect Ratio Conversion for DVD Contents */
	//if((img_width <= COMPONENT_EXCLUSIVE_UI_DVD_MAX_WD) && (img_height <= COMPONENT_EXCLUSIVE_UI_DVD_MAX_HT))
	{
		memset((void *)&aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		TCC_OUTPUT_EXCLUSIVE_UI_CfgAR(Component_LCDC_Num, img_width, img_height, &aspect_ratio);
	}

	/* Get the parameters for exclusive ui */
	TCC_OUTPUT_EXCLUSIVE_UI_GetParam(&component_exclusive_ui_param);

	if(component_exclusive_ui_param.enable)
	{
		/* Clear the on_the_flay for video */
		component_exclusive_ui_onthefly = FALSE;

		/* Clear update flag and backup an information of the last video */
		TCC_OUTPUT_EXCLUSIVE_UI_SetImage(update);

		/*-------------------------------------*/
		/*           INTERLACE VIDEO           */
		/* Update video data with exclusive ui */
		/*-------------------------------------*/
		if(component_exclusive_ui_param.interlace)
		{
			/* Interlace BD Contents with 1920x1080 graphic plane */
			if(!TCC_OUTPUT_EXCLUSIVE_UI_ViqeMtoM(Component_LCDC_Num, img_width, img_height))
			{
				if((img_width == lcd_w) && (img_height == lcd_h))
				{
					/* 1920x1080 COMPONENT Output */
				}
				else
				{
					/* 1280x720 COMPONENT Output */
				}
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(Component_LCDC_Num, FALSE);
				
				/* VIQE and M2M Scaler with On_the_fly */
				/* Check the output format */
				if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
					lcd_h = lcd_h >> 1;
			
				if(component_exclusive_ui_param.clear == TRUE)
				{
					/* M2M Scaler without On_the_fly */
					if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(update, Component_LCDC_Num) < 0)
						component_exclusive_ui_onthefly = FALSE;
					else
						component_exclusive_ui_onthefly = TRUE;

					/* Update Screen */
					tcc_component_update(update);
				}
				else
				{
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(update, COMPONENT_EXCLUSIVE_UI_SCALER1, component_exclusive_ui_param.plane_width, component_exclusive_ui_param.plane_height, component_exclusive_ui_idx, 0);

					if(TCC_OUTPUT_EXCLUSIVE_UI_GetUpdate())
					{
						dprintk("E\n");
					}
					else
					{
						/* Overlay Mixer */
						TCC_OUTPUT_EXCLUSIVE_UI_Mixer(update, component_exclusive_ui_idx, 0);
						/* Update overlay mixer image */
						TCC_OUTPUT_EXCLUSIVE_UI_Restore_Mixer(update, Component_LCDC_Num, lcd_w, lcd_h, component_exclusive_ui_idx);
						/* Set the interrupt flag */
						mixer_interrupt_use = 1;
	 				}
				}
			}
		}
		else
		/*-------------------------------------*/
		/*          PROGRESSIVE VIDEO          */
		/* Update video data with exclusive ui */
		/*-------------------------------------*/
		{
			if( (img_width == component_exclusive_ui_param.plane_width) && (img_width == lcd_w) && (img_width == TCC_FB_OUT_MAX_WIDTH) &&
				(img_height == component_exclusive_ui_param.plane_height) && (img_height == lcd_h) && (img_height == TCC_FB_OUT_MAX_HEIGHT) )
			{
				/* 1920x1080 COMPONENT Output */
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(Component_LCDC_Num, FALSE);

				if(component_exclusive_ui_param.clear == FALSE)
				{
					/* M2M Scaler without On_the_fly */
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(update, COMPONENT_EXCLUSIVE_UI_SCALER0, component_exclusive_ui_param.plane_width, component_exclusive_ui_param.plane_height, component_exclusive_ui_idx, 0);
					/* Overlay Mixer */
					TCC_OUTPUT_EXCLUSIVE_UI_Mixer(update, component_exclusive_ui_idx, 0);
 				}
				
			#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
				/* M2M Scaler without On_the_fly */
				TCC_OUTPUT_EXCLUSIVE_UI_Scaler(update, COMPONENT_EXCLUSIVE_UI_SCALER0, lcd_w, lcd_h, component_exclusive_ui_idx, 1);
				//hdmi_exclusive_ui_onthefly = FALSE;
			#else
				/* M2M Scaler with On_the_fly */
				if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(update, Component_LCDC_Num) < 0)
					component_exclusive_ui_onthefly = FALSE;
				else
					component_exclusive_ui_onthefly = TRUE;
			#endif
			}

			#if 0 //shkim - on_the_fly
			/* Set the on_the_flay for android UI */
			if(component_exclusive_ui_onthefly == FALSE)
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(Component_LCDC_Num, 1, TRUE);
			else
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(Component_LCDC_Num, 1, FALSE);
			#endif
		}

		component_exclusive_ui_idx = !component_exclusive_ui_idx;
	}
	
	if(force_update &&  !mixer_interrupt_use)
	{
		tcc_component_update(update);
	}
}

void tcc_component_display_image(exclusive_ui_update UpdateImage)
{
	dprintk("%s\n", __func__);
			
	/* M2M Scaler with On_the_fly */
	if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(&UpdateImage.image, UpdateImage.lcdc) < 0)
		component_exclusive_ui_onthefly = FALSE;
	else
		component_exclusive_ui_onthefly = TRUE;

	/* Update Screen */
	tcc_component_update(&UpdateImage.image);
}
#endif

/*****************************************************************************
 Function Name : tcc_component_end()
******************************************************************************/
void tcc_component_end(void)
{
	dprintk("%s, LCDC_Num = %d\n", __func__, Component_LCDC_Num);

	//tcc_component_set_interrupt(FALSE, Component_LCDC_Num);

	#ifdef TCC_COMPONENT_IC_CS4954
		//cs4954_reset();
	#endif

	#ifdef TCC_COMPONENT_IC_THS8200
		ths8200_reset();
	#endif

	#if (defined(CONFIG_CPU_FREQ_TCC92X) || defined (CONFIG_CPU_FREQ_TCC93XX)) && defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtTvClockLimitTable, TCC_FREQ_LIMIT_TV, 0);
	#endif

	/* Clear the start flag */
	component_start = 0;
	
#if 0
	if(Component_Mode == COMPONENT_MODE_1080I)
	{
		Component_FID = 0;
		tcc_component_lcdc_interrupt_set(FALSE);
	}
#endif /* 0 */
	
	if(component_plugout)
		component_plugout = 0;
}

/*****************************************************************************
 Function Name : tcc_component_start()
******************************************************************************/
void tcc_component_start(TCC_COMPONENT_MODE_TYPE mode)
{
	PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);

	dprintk("%s, mode=%d\n", __func__, mode);

	switch(mode)
	{
		case COMPONENT_NTST_M:
			Component_Mode = COMPONENT_MODE_NTSC_M;
			break;
		case COMPONENT_PAL:
			Component_Mode = COMPONENT_MODE_PAL_B;
			break;
		case COMPONENT_720P:
			Component_Mode = COMPONENT_MODE_720P;
			break;
		case COMPONENT_1080I:
			Component_Mode = COMPONENT_MODE_1080I;
			break;
	}

	#ifdef CONFIG_ARCH_TCC92XX
		//tca_lcdc_port_setting(Component_LCDC_Num);
	#endif

	tcc_component_set_lcd2tv(Component_Mode);

	if(Component_Mode == COMPONENT_MODE_720P || Component_Mode == COMPONENT_MODE_1080I)
	{
		/* Driver Strength Control - LCD_CLK(GPIO_C25) */
		#if defined(CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST) || defined(CONFIG_ARCH_TCC88XX)
			BITCSET(pGPIO->GPCCD1, Hw19|Hw18, Hw19|Hw18);
		#endif

		if(Component_Mode == COMPONENT_MODE_1080I)
			TCC_OUTPUT_LCDC_CtrlLayer(TCC_OUTPUT_COMPONENT, 1, TCC_LCDC_IMG_FMT_RGB565);
		else
			TCC_OUTPUT_LCDC_CtrlLayer(TCC_OUTPUT_COMPONENT, 0, TCC_LCDC_IMG_FMT_RGB565);

		/* FID port of THS8200 */
		#if defined(CONFIG_ARCH_TCC92XX)
			#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200) || defined(CONFIG_MACH_TCC9201)
			BITSET(pGPIO->GPCDAT, Hw28);
			#else
			BITSET(pGPIO->GPCDAT, Hw30);
			#endif
		#elif defined(CONFIG_ARCH_TCC93XX) 			
			#if defined(CONFIG_MACH_TCC9300ST)
			BITCLR(pGPIO->GPCFN3, Hw4-Hw0);
			BITSET(pGPIO->GPCEN, Hw24);
			BITSET(pGPIO->GPCDAT, Hw24);
			#elif defined(CONFIG_MACH_TCC9300CM)
			BITSET(pGPIO->GPCDAT, Hw28);
			#else
			BITSET(pGPIO->GPEDAT, Hw1);
			#endif
		#elif defined(CONFIG_ARCH_TCC88XX)					
			#if defined(CONFIG_MACH_TCC8800ST)
			BITCLR(pGPIO->GPCFN3, Hw4-Hw0);
			BITSET(pGPIO->GPCEN, Hw24);
			BITSET(pGPIO->GPCDAT, Hw24);			
			#else
			BITCLR(pGPIO->GPCFN3, Hw24-Hw20);
			BITSET(pGPIO->GPCEN, Hw29);
			BITSET(pGPIO->GPCDAT, Hw29);			
			#endif			
		#endif
	}

	#ifdef TCC_COMPONENT_IC_CS4954
		cs4954_enable(Component_Mode);
	#endif

	#ifdef TCC_COMPONENT_IC_THS8200
		ths8200_enable(Component_Mode);
	#endif

#if 0
	if(Component_Mode == COMPONENT_MODE_1080I)
	{
		Component_FID = 0;
		tcc_component_lcdc_interrupt_set(TRUE);
	}
#endif /* 0 */

	/* Set the start flag */
	component_start = 1;
}

/*****************************************************************************
 Function Name : tcc_component_clock_onoff()
******************************************************************************/
void tcc_component_clock_onoff(char OnOff)
{
	dprintk("%s, Onoff = %d \n", __func__, OnOff);

	if(OnOff)
	{
		if(Component_LCDC_Num)
			clk_enable(component_lcdc1_clk);
		else
			clk_enable(component_lcdc0_clk);
	}
	else
	{
		if(Component_LCDC_Num)
			clk_disable(component_lcdc1_clk);
		else		
			clk_disable(component_lcdc0_clk);
	}

}

/*****************************************************************************
 Function Name : tcc_component_ioctl()
******************************************************************************/
static long tcc_component_ioctl(struct file *file, unsigned int cmd, void *arg)
{
	TCC_COMPONENT_START_TYPE start;
	TCC_COMPONENT_MODE_TYPE type;
	struct tcc_lcdc_image_update update;			
	
	dprintk("component_ioctl IOCTRL[%d] \n", cmd);      

	switch(cmd)
	{
		case TCC_COMPONENT_IOCTL_START:
			copy_from_user(&start,arg,sizeof(start));
			#if !defined(CONFIG_ARCH_TCC93XX) && !defined(CONFIG_ARCH_TCC88XX) && !defined(CONFIG_TCC_OUTPUT_STARTER)
				if(start.lcdc == COMPONENT_LCDC_0)
				{
					TCC_FB_LCDC_NumSet(1, FALSE);
				}
				else
				{
					TCC_FB_LCDC_NumSet(0, FALSE);
				}
			#endif
			
			TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_COMPONENT, start.lcdc, TRUE);
			
			if(start.lcdc != Component_LCDC_Num)
			{
				tcc_component_set_lcdc(start.lcdc);
				tcc_component_clock_onoff(TRUE);
			}
			tcc_component_start(start.mode);
			
#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
			tca_vsync_video_display_enable();
#endif
			
			break;

		case TCC_COMPONENT_IOCTL_UPDATE:
			copy_from_user(&update,arg,sizeof(update));
			tcc_component_update(&update);									
			break;
			
		case TCC_COMPONENT_IOCTL_END:
			#if !defined(CONFIG_ARCH_TCC93XX) && !defined(CONFIG_ARCH_TCC88XX) && !defined(CONFIG_TCC_OUTPUT_STARTER)
				if(fb_power_state)	
					TCC_FB_LCDC_NumSet(1, TRUE);
			#endif
			TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_COMPONENT, Component_LCDC_Num, FALSE);
			tcc_component_end();

#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
			tca_vsync_video_display_disable();
			tcc_vsync_set_firstFrameFlag(1);
#endif
#if defined(TCC_VIDEO_DISPLAY_DEINTERLACE_MODE)
			tcc_vsync_viqe_deinitialize();
#endif /* TCC_VIDEO_DISPLAY_DEINTERLACE_MODE */
			break;

		case TCC_COMPONENT_IOCTL_PROCESS:
			#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
				copy_from_user(&update,arg,sizeof(update));
				tcc_component_process(&update, 0);
				copy_to_user(arg,&update,sizeof(update));
			#endif
			break;

		default:
			printk("%d, Unsupported IOCTL!!!\n", cmd);      
			break;			
	}

	return 0;
}

/*****************************************************************************
 Function Name : tcc_component_open()
******************************************************************************/
static int tcc_component_open(struct inode *inode, struct file *filp)
{	
	dprintk("%s  \n", __func__);

	if(Component_Mode != COMPONENT_MODE_1080I)
		tcc_component_clock_onoff(1);

	return 0;
}

/*****************************************************************************
 Function Name : tcc_component_release()
******************************************************************************/
static int tcc_component_release(struct inode *inode, struct file *file)
{
	dprintk("%s  \n", __func__);

	//LCD off
	#if defined(CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST) || defined(CONFIG_ARCH_TCC88XX)
	if(pComponent_HwLCDC)
		pComponent_HwLCDC->LI0C = 0 | HwLCT_RU;
	#endif

	if(Component_Mode != COMPONENT_MODE_1080I)
		tcc_component_clock_onoff(0);
	
	return 0;
}

static struct file_operations tcc_component_fops = 
{
	.owner          = THIS_MODULE,
	.unlocked_ioctl = tcc_component_ioctl,
	.open           = tcc_component_open,
	.release        = tcc_component_release,	
};

static struct class *tcc_component_class;

/*****************************************************************************
 Function Name : tcc_component_init()
******************************************************************************/
int __init tcc_component_init(void)
{
	printk("%s  \n", __func__);

	component_lcdc0_clk = clk_get(0, "lcdc0");
	BUG_ON(component_lcdc0_clk == NULL);
	component_lcdc1_clk = clk_get(0, "lcdc1");
	BUG_ON(component_lcdc1_clk == NULL);

	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_component_fops);
	tcc_component_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(tcc_component_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);
	
	#if defined (CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST)
		#if defined(CONFIG_TCC_OUTPUT_AUTO_DETECTION)
			tcc_gpio_config(COMPONENT_DETECT_GPIO, GPIO_FN(0));
			gpio_request(COMPONENT_DETECT_GPIO, NULL);
			gpio_direction_input(COMPONENT_DETECT_GPIO);
		#endif
	#endif

	return 0;
}

/*****************************************************************************
 Function Name : tcc_component_cleanup()
******************************************************************************/
void __exit tcc_component_cleanup(void)
{
	printk("%s LCDC:%d \n", __func__, Component_LCDC_Num);

	unregister_chrdev(MAJOR_ID, DEVICE_NAME);

	if(component_lcdc0_clk =! NULL)
		clk_put(component_lcdc0_clk);
	if(component_lcdc1_clk =! NULL)
		clk_put(component_lcdc1_clk);

	return;
}

module_init(tcc_component_init);
module_exit(tcc_component_cleanup);

MODULE_AUTHOR("Telechips");
MODULE_DESCRIPTION("Telechips COMPONENT Out driver");
MODULE_LICENSE("GPL");

