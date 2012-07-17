/*
 * linux/drivers/video/tcc_composite.c
 *
 * Author:  <linux@telechips.com>
 * Created: March 18, 2010
 * Description: TCC Composite TV-Out Driver
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
#include <asm/mach-types.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include "../tccfb.h"
#include "tcc_composite.h"
#include "tcc_composite_internal.h"
#include <mach/tcc_composite_ioctl.h>

#include <mach/tccfb_ioctrl.h>
#include <mach/tca_fb_output.h>
#include <mach/gpio.h>

#include <mach/vioc_outcfg.h>
#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_disp.h>
#include <mach/vioc_global.h>
#include <mach/vioc_config.h>
#include <mach/vioc_scaler.h>

extern void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
									unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U,unsigned int* V);


/*****************************************************************************

 VARIABLES

******************************************************************************/

extern char fb_power_state;

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tcc_composite: " msg); }

#define TCC_LCDC1_USE

static int				Composite_LCDC_Num = -1;

#define DEVICE_NAME		"composite"
#define MAJOR_ID		205
#define MINOR_ID		1

#if defined (CONFIG_MACH_TCC9300ST)
#define COMPOSITE_DETECT_GPIO		TCC_GPE(30)
#define COMPOSITE_DETECT_EINTSEL	SEL_GPIOE30
#define COMPOSITE_DETECT_EINTNUM	5
#define COMPOSITE_DETECT_EINT		HwINT0_EI5
#elif defined (CONFIG_MACH_TCC8800ST)
#define COMPOSITE_DETECT_GPIO		TCC_GPF(27)
#define COMPOSITE_DETECT_EINTSEL	SEL_GPIOF27
#define COMPOSITE_DETECT_EINTNUM	7
#define COMPOSITE_DETECT_EINT		HwINT1_EI7
#elif defined (CONFIG_MACH_TCC8920ST)
#define COMPOSITE_DETECT_GPIO		TCC_GPF(1)
#define COMPOSITE_DETECT_EINTSEL	EXTINT_GPIOF_01
#define COMPOSITE_DETECT_EINTNUM	INT_EI7
#define COMPOSITE_DETECT_EINT		1<<INT_EI7
#else
#define COMPOSITE_DETECT_GPIO		NULL
#define COMPOSITE_DETECT_EINTSEL	NULL
#define COMPOSITE_DETECT_EINTNUM	NULL
#define COMPOSITE_DETECT_EINT		NULL
#endif

static struct clk *composite_lcdc0_clk;
static struct clk *composite_lcdc1_clk;
static struct clk *composite_dac_clk;
static struct clk *composite_ntscpal_clk;

static char tcc_composite_mode = COMPOSITE_MAX_M;

static char composite_plugout = 0;
static char composite_plugout_count = 0;
static char composite_ext_interrupt = 0;

#if defined(CONFIG_CPU_FREQ_TCC92X) || defined (CONFIG_CPU_FREQ_TCC93XX)
extern struct tcc_freq_table_t gtTvClockLimitTable;
#endif

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
#define COMPOSITE_EXCLUSIVE_UI_SCALER0		0
#define COMPOSITE_EXCLUSIVE_UI_SCALER1		1
#define COMPOSITE_EXCLUSIVE_UI_DVD_MAX_WD	720
#define COMPOSITE_EXCLUSIVE_UI_DVD_MAX_HT	576
static unsigned int composite_exclusive_ui_idx = 0;
static unsigned int composite_exclusive_ui_onthefly = FALSE;
static exclusive_ui_params composite_exclusive_ui_param;
#endif

/*****************************************************************************

 FUNCTIONS

******************************************************************************/

extern char TCC_FB_LCDC_NumSet(char NumLcdc, char OnOff);
extern void tca_vsync_video_display_enable(void);
extern void tca_vsync_video_display_disable(void);
#if defined(TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
extern void tcc_vsync_set_firstFrameFlag(int firstFrameFlag);
extern int tcc_vsync_get_isVsyncRunning(void);
#endif /* TCC_VIDEO_DISPLAY_BY_VSYNC_INT */

extern unsigned int tca_get_lcd_lcdc_num(viod);
extern unsigned int tca_get_output_lcdc_num(viod);


/*****************************************************************************
 Function Name : tcc_composite_ext_handler()
******************************************************************************/
static irqreturn_t tcc_composite_ext_handler(int irq, void *dev_id)
{
	#if defined(CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST)
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwVPIC_BASE);
	#elif defined(CONFIG_MACH_TCC8920ST)
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	#endif
	
	dprintk("%s, composite_plugout_count=%d\n", __func__, composite_plugout_count);

	composite_plugout_count++;
	if(composite_plugout_count > 10)
	{
		composite_plugout_count = 0;
		composite_plugout = 1;

	#if defined(CONFIG_MACH_TCC9300ST)
        BITCLR(pHwPIC->INTMSK0, COMPOSITE_DETECT_EINT);
        BITCLR(pHwPIC->IEN0, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->CLR0, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8800ST)
        BITCLR(pHwPIC->INTMSK1, COMPOSITE_DETECT_EINT);
        BITCLR(pHwPIC->IEN1, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->CLR1, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8920ST)
        BITCLR(pHwPIC->INTMSK0.nREG, COMPOSITE_DETECT_EINT);
        BITCLR(pHwPIC->IEN0.nREG, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->CLR0.nREG, COMPOSITE_DETECT_EINT);
	#endif
	}
	else
	{
	#if defined(CONFIG_MACH_TCC9300ST)
		BITSET(pHwPIC->CLR0, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8800ST)
		BITSET(pHwPIC->CLR1, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8920ST)
		BITSET(pHwPIC->CLR0.nREG, COMPOSITE_DETECT_EINT);
	#endif
	}

	return IRQ_HANDLED;
}

/*****************************************************************************
 Function Name : tcc_composite_ext_interrupt_sel()
******************************************************************************/
void tcc_composite_ext_interrupt_sel(char ext_int_num, char ext_int_sel)
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
#elif defined(CONFIG_MACH_TCC8920ST)
    tcc_gpio_config_ext_intr(ext_int_num, ext_int_sel);
#endif
}

/*****************************************************************************
 Function Name : tcc_composite_ext_interrupt_set()
******************************************************************************/
void tcc_composite_ext_interrupt_set(char onoff)
{
#if defined(CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST) || defined(CONFIG_MACH_TCC8920ST)
	int ret, irq_num;
	#if defined(CONFIG_MACH_TCC8920ST)
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	#else
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwVPIC_BASE);
	#endif
	
	composite_plugout_count = 0;

	#if defined(CONFIG_MACH_TCC9300ST)
		irq_num = IRQ_EI5;
	#else
		irq_num = INT_EI7;
	#endif
	
	if(onoff)
	{
		if(composite_ext_interrupt == 1)
			return;

		tcc_composite_ext_interrupt_sel(COMPOSITE_DETECT_EINTNUM, COMPOSITE_DETECT_EINTSEL);

	#if defined(CONFIG_MACH_TCC9300ST)
		BITCLR(pHwPIC->POL0, COMPOSITE_DETECT_EINT);
		BITCLR(pHwPIC->MODE0, COMPOSITE_DETECT_EINT);
		BITCLR(pHwPIC->MODEA0, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->SEL0, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8800ST)
		BITCLR(pHwPIC->POL1, COMPOSITE_DETECT_EINT);
		BITCLR(pHwPIC->MODE1, COMPOSITE_DETECT_EINT);
		BITCLR(pHwPIC->MODEA1, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->SEL1, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8920ST)
		BITCLR(pHwPIC->POL0.nREG, COMPOSITE_DETECT_EINT);
		BITCLR(pHwPIC->MODE0.nREG, COMPOSITE_DETECT_EINT);
		BITCLR(pHwPIC->MODEA0.nREG, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->SEL0.nREG, COMPOSITE_DETECT_EINT);
	#endif

		if(ret = request_irq(irq_num, tcc_composite_ext_handler, IRQF_SHARED, "TCC_COMPOSITE_EXT", tcc_composite_ext_handler))	
		{
			printk("%s, ret=%d, irq_num=%d, request_irq ERROR!\n", __func__, ret, irq_num);
		}

 	#if defined(CONFIG_MACH_TCC9300ST)
		BITSET(pHwPIC->CLR0, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->INTMSK0, COMPOSITE_DETECT_EINT);	
        BITSET(pHwPIC->IEN0, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8800ST)
		BITSET(pHwPIC->CLR1, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->INTMSK1, COMPOSITE_DETECT_EINT);	
        BITSET(pHwPIC->IEN1, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8920ST)
        BITSET(pHwPIC->CLR0.nREG, COMPOSITE_DETECT_EINT);
		BITSET(pHwPIC->INTMSK0.nREG, COMPOSITE_DETECT_EINT);	
        BITSET(pHwPIC->IEN0.nREG, COMPOSITE_DETECT_EINT);
	#endif

		composite_ext_interrupt = 1;
	}
	else
	{
		if(composite_ext_interrupt == 0)
			return;

		free_irq(irq_num, tcc_composite_ext_handler);
		
 	#if defined(CONFIG_MACH_TCC9300ST)
        BITCLR(pHwPIC->INTMSK0, COMPOSITE_DETECT_EINT);
        BITCLR(pHwPIC->IEN0, COMPOSITE_DETECT_EINT);
        BITSET(pHwPIC->CLR0, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8800ST)
        BITCLR(pHwPIC->INTMSK1, COMPOSITE_DETECT_EINT);
        BITCLR(pHwPIC->IEN1, COMPOSITE_DETECT_EINT);
        BITSET(pHwPIC->CLR1, COMPOSITE_DETECT_EINT);
	#elif defined(CONFIG_MACH_TCC8920ST)
        BITCLR(pHwPIC->INTMSK0.nREG, COMPOSITE_DETECT_EINT);
        BITCLR(pHwPIC->IEN0.nREG, COMPOSITE_DETECT_EINT);
        BITSET(pHwPIC->CLR0.nREG, COMPOSITE_DETECT_EINT);
	#endif

		composite_ext_interrupt = 0;
	}

	dprintk("%s, onoff=%d\n", __func__, onoff);
 #endif
}

/*****************************************************************************
 Function Name : tcc_composite_detect()
******************************************************************************/
int tcc_composite_detect(void)
{
	int detect = true;

	#if defined (CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST) || defined(CONFIG_MACH_TCC8920ST)
		#if defined(CONFIG_TCC_OUTPUT_AUTO_DETECTION) && defined(TCC_OUTPUT_AUTO_ALL)
			if(composite_plugout)
			{
				if(composite_ext_interrupt == 1)
					tcc_composite_ext_interrupt_set(FALSE);

				detect = false;
			}
			else
			{
				if(composite_ext_interrupt)
				{
					detect = true;
				}
				else
				{
					detect = gpio_get_value(COMPOSITE_DETECT_GPIO)? false : true;

					if(detect)
					{
						if(composite_ext_interrupt == 0)
							tcc_composite_ext_interrupt_set(TRUE);
			 		}

					dprintk("%s, detect=%d\n", __func__, detect);
				}
			}
		#elif (defined(CONFIG_TCC_OUTPUT_AUTO_DETECTION) && defined(TCC_OUTPUT_AUTO_HDMI_CVBS)) || (defined(CONFIG_TCC_OUTPUT_ATTACH) && defined(CONFIG_OUTPUT_ATTACH_HDMI_CVBS))
			/* Check the HDMI detection */
			#if defined(CONFIG_MACH_TCC9300ST)
				if(gpio_get_value(TCC_GPA(14)))
			#elif defined(CONFIG_MACH_TCC8800ST)
				if(gpio_get_value(TCC_GPD(25)))
			#elif defined(CONFIG_MACH_TCC8920ST)
				if(gpio_get_value(TCC_GPHDMI(1)))
			#endif
				{
					detect = false;
				}
 		#elif defined(CONFIG_TCC_OUTPUT_ATTACH) && defined(CONFIG_OUTPUT_ATTACH_DUAL_AUTO)
			detect = false;
		#endif
	#endif

	return detect;
}

/*****************************************************************************
 Function Name : tcc_composite_set_lcdc()
******************************************************************************/
int tcc_composite_set_lcdc(int lcdc_num)
{
	dprintk("%s  new_mode:%d , before_mode:%d \n",__func__, lcdc_num, Composite_LCDC_Num);

	if(lcdc_num == Composite_LCDC_Num)
		return FALSE;
	
	if(lcdc_num)
	{
		Composite_LCDC_Num = 1;
	}
	else
	{
		Composite_LCDC_Num = 0;
	}
	
	return TRUE;
}

/*****************************************************************************
 Function Name : tcc_composite_connect_lcdc()
******************************************************************************/
int tcc_composite_connect_lcdc(int lcdc_num)
{
	PDDICONFIG pHwDDICFG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	
	if(lcdc_num)
	{
		pHwDDICFG->NTSCPAL_EN.nREG |= Hw0;	// LCDC1	- default
	}
	else
	{
		pHwDDICFG->NTSCPAL_EN.nREG &= ~Hw0;	// LCDC0
	}
}

/*****************************************************************************
 Function Name : tcc_composite_get_spec()
******************************************************************************/
void tcc_composite_get_spec(COMPOSITE_MODE_TYPE mode, COMPOSITE_SPEC_TYPE *spec)
{
	switch(mode)
	{
		case NTSC_M:
		case NTSC_M_J:
		case PAL_M:
		case NTSC_443:
		case PSEUDO_PAL:
			spec->composite_clk = 270000;
			spec->composite_bus_width = 8;
			spec->composite_lcd_width = 720;
			spec->composite_lcd_height = 480;
		#ifdef TCC_COMPOSITE_CCIR656
			spec->composite_LPW = 224 - 1; 					// line pulse width
			spec->composite_LPC = 720 * 2 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->composite_LSWC = 20 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->composite_LEWC = 32 - 1;					// line end wait clock (the number of dummy pixel clock - 1)
		#else
			spec->composite_LPW = 212 - 1; 					// line pulse width
			spec->composite_LPC = 720 * 2 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->composite_LSWC = 32 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->composite_LEWC = 32 - 1;					// line end wait clock (the number of dummy pixel clock - 1)
		#endif

			spec->composite_VDB = 0; 						// Back porch Vsync delay
			spec->composite_VDF = 0; 						// front porch of Vsync delay

			spec->composite_FPW1 = 1 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC1 = 480 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC1 = 37 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC1 = 7 - 1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->composite_FPW2 = 1 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC2 = 480 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC2 = 38 - 1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC2 = 6 - 1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;

		case NTSC_N:
		case NTSC_N_J:
		case PAL_N:
		case PAL_B:
		case PAL_G:
		case PAL_H:
		case PAL_I:
		case PSEUDO_NTSC:		
			spec->composite_clk = 270000;
			spec->composite_bus_width = 8;
			spec->composite_lcd_width = 720;
			spec->composite_lcd_height = 576;
			spec->composite_LPW = 128 - 1; 					// line pulse width
			spec->composite_LPC = 720 * 2 - 1; 				// line pulse count (active horizontal pixel - 1)
			spec->composite_LSWC = 138 - 1;					// line start wait clock (the number of dummy pixel clock - 1)
			spec->composite_LEWC = 22 - 1;					// line end wait clock (the number of dummy pixel clock - 1)

			spec->composite_VDB = 0; 						// Back porch Vsync delay
			spec->composite_VDF = 0; 						// front porch of Vsync delay
				
			spec->composite_FPW1 = 1 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC1 = 576 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC1 = 43-1;					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC1 = 5-1;					// frame start wait cycle is the number of lines to insert at the begining each frame
			spec->composite_FPW2 = 1 - 1;					// TFT/TV : Frame pulse width is the pulse width of frmae clock
			spec->composite_FLC2 = 576 - 1;					// frmae line count is the number of lines in each frmae on the screen
			spec->composite_FSWC2 = 44-1;//4 					// frmae start wait cycle is the number of lines to insert at the end each frame
			spec->composite_FEWC2 = 4-1; 					// frame start wait cycle is the number of lines to insert at the begining each frame
			break;
	}
}

/*****************************************************************************
 Function Name : tcc_composite_set_lcd2tv()
******************************************************************************/
void tcc_composite_set_lcd2tv(COMPOSITE_MODE_TYPE type)
{
	COMPOSITE_SPEC_TYPE spec;
	stLTIMING				CompositeTiming;
	stLCDCTR				LcdCtrlParam;
	PVIOC_DISP				pDISPBase;
	PVIOC_WMIX				pWIXBase;
	PVIOC_RDMA				pRDMA;
	PDDICONFIG 				pDDICfg = (PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	unsigned int			width, height;

	unsigned int lcd_ctrl = 0;
	unsigned int lcd_peri = 0;

	#define LCDC_CLK_DIV 1
	
	tcc_composite_get_spec(type, &spec);
	
	#if (defined(CONFIG_CPU_FREQ_TCC92X) || defined (CONFIG_CPU_FREQ_TCC93XX)) && defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtTvClockLimitTable, TCC_FREQ_LIMIT_TV, 1);
	#endif

	if(Composite_LCDC_Num)
		lcd_peri = PERI_LCD1;
	else
		lcd_peri = PERI_LCD0;

	VIOC_DISP_SWReset(Composite_LCDC_Num);

	BITSET(pDDICfg->PWDN.nREG, Hw1);		// PWDN - TVE
	BITCLR(pDDICfg->SWRESET.nREG, Hw1);		// SWRESET - TVE
	BITSET(pDDICfg->SWRESET.nREG, Hw1);		// SWRESET - TVE	
	BITSET(pDDICfg->NTSCPAL_EN.nREG, Hw0);	// NTSCPAL_EN	
	
	if(Composite_LCDC_Num)	
	{
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);
		pWIXBase =(VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX1); 
	}
	else
	{
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP0);
		pWIXBase =(VIOC_WMIX *)tcc_p2v(HwVIOC_WMIX0); 
	}

	tca_ckc_setperi(lcd_peri, ENABLE, 270000);

	width = spec.composite_lcd_width;
	height = spec.composite_lcd_height;
	
	CompositeTiming.lpw = spec.composite_LPW;
	CompositeTiming.lpc = spec.composite_LPC + 1;
	CompositeTiming.lswc = spec.composite_LSWC + 1;
	CompositeTiming.lewc = spec.composite_LEWC + 1;
	
	CompositeTiming.vdb = spec.composite_VDB;
	CompositeTiming.vdf = spec.composite_VDF;
	CompositeTiming.fpw = spec.composite_FPW1;
	CompositeTiming.flc = spec.composite_FLC1;
	CompositeTiming.fswc = spec.composite_FSWC1;
	CompositeTiming.fewc = spec.composite_FEWC1;
	CompositeTiming.fpw2 = spec.composite_FPW2;
	CompositeTiming.flc2 = spec.composite_FLC2;
	CompositeTiming.fswc2 = spec.composite_FSWC2;
	CompositeTiming.fewc2 = spec.composite_FEWC2;
	
	VIOC_DISP_SetTimingParam(pDISPBase, &CompositeTiming);
 
	memset(&LcdCtrlParam, NULL, sizeof(LcdCtrlParam));

	LcdCtrlParam.r2ymd = 0;
	LcdCtrlParam.ckg = 1;
	LcdCtrlParam.id= 0;
	LcdCtrlParam.iv = 0;
	LcdCtrlParam.ih = 1;
	LcdCtrlParam.ip = 1;
	LcdCtrlParam.clen = 1;
	LcdCtrlParam.r2y = 1;
	LcdCtrlParam.pxdw = 6;
	LcdCtrlParam.dp = 0;
	LcdCtrlParam.ni = 0;
	LcdCtrlParam.tv = 1;
	LcdCtrlParam.opt = 0;
	LcdCtrlParam.stn = 0;
	LcdCtrlParam.evsel = 0;
	LcdCtrlParam.ovp = 0;

	if(Composite_LCDC_Num)	
		LcdCtrlParam.advi = 1;
	else
		LcdCtrlParam.advi = 0;

	VIOC_DISP_SetControlConfigure(pDISPBase, &LcdCtrlParam);

	VIOC_DISP_SetSize(pDISPBase, width, height);
	VIOC_DISP_SetBGColor(pDISPBase, 0, 0 , 0);

	//VIOC_DISP_TurnOn(pDISPBase);

	VIOC_WMIX_SetOverlayPriority(pWIXBase, 0);
	VIOC_WMIX_SetBGColor(pWIXBase, 0x00, 0x00, 0x00, 0xff);
	VIOC_WMIX_SetSize(pWIXBase, width, height);
	VIOC_WMIX_SetPosition(pWIXBase, 0, 0, 0);
	VIOC_WMIX_SetChromaKey(pWIXBase, 0, 0, 0, 0, 0, 0xF8, 0xFC, 0xF8);
	VIOC_WMIX_SetUpdate(pWIXBase);

	if(Composite_LCDC_Num)	
	{
		VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_SDVENC, VIOC_OUTCFG_DISP1);
	}
	else
	{
		VIOC_OUTCFG_SetOutConfig(VIOC_OUTCFG_SDVENC, VIOC_OUTCFG_DISP0);
	}
}

/*****************************************************************************
 Function Name : tcc_composite_get_lcdsize()
******************************************************************************/
void tcc_composite_get_lcdsize(unsigned int *width, unsigned int *height)
{
	unsigned int lcdsize;

	*width = lcdsize & 0x0000FFFF;
	*height = lcdsize >>16;
}

/*****************************************************************************
 Function Name : tcc_composite_change_formattype()
******************************************************************************/
COMPOSITE_LCDC_IMG_FMT_TYPE tcc_composite_change_formattype(TCC_COMPOSITE_FORMAT_TYPE g2d_format)
{
	COMPOSITE_LCDC_IMG_FMT_TYPE LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_MAX;

	switch(g2d_format)
	{
		case GE_YUV444_sp:
			break;

		case GE_YUV440_sp:
			break;
				
		case GE_YUV422_sp: 
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_YUV422SP;
			break;
				
		case GE_YUV420_sp:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_YUV420SP;
			break;

		case GE_YUV411_sp:
		case GE_YUV410_sp:
			break;

		case GE_YUV422_in:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_YUV422ITL0;
			break;

		case GE_YUV420_in:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_YUV420ITL0;
			break;

		case GE_YUV444_sq:
			break;
		case GE_YUV422_sq:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_YUV422SQ;
			break;

		case GE_RGB332:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_RGB332;
			break;
				
		case GE_RGB444:
		case GE_ARGB4444:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_RGB444;
			break;

		case GE_RGB454:
		case GE_ARGB3454:
			break;

		case GE_RGB555:
		case GE_ARGB1555:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_RGB555;
			break;
		case GE_RGB565:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_RGB565;
			break;
			
		case GE_RGB666:
		case GE_ARGB4666:
		case GE_ARGB6666:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_RGB666;
			break;

		case GE_RGB888:
		case GE_ARGB4888:
		case GE_ARGB8888:
			LCDC_fmt = COMPOSITE_LCDC_IMG_FMT_RGB888;
			break;
	}

	return LCDC_fmt;
}

/*****************************************************************************
 Function Name: tcc_composite_set_imagebase()
 Parameters
		: nType		: IMG_CH0, IMG_CH1, IMG_CH2
		: nBase0	: Base Address for Y Channel or RGB Channel
					: Valid for 3 Channel
		: nBase1	: Base Address for Cb(U)
					: Valid for IMG_CH0
		: nBase2	: Base Address for Cb(U) Channel
					: Valid for IMG_CH0
******************************************************************************/
void tcc_composite_set_imagebase(unsigned int type, unsigned int base0, unsigned int base1, unsigned int base2)
{
	switch (type) 
	{
		case	IMG_CH0 :
			break;

		case	IMG_CH1 :
			break;

		case	IMG_CH2 :
			break;

		default	:
			break;
	}
}

/*****************************************************************************
 Function Name : tcc_composite_get_offset()
******************************************************************************/
void tcc_composite_get_offset(COMPOSITE_LCDC_IMG_FMT_TYPE fmt, unsigned int width, unsigned int *offset0, unsigned int *offset1)
{
	switch(fmt)
	{
		case COMPOSITE_LCDC_IMG_FMT_1BPP:
		case COMPOSITE_LCDC_IMG_FMT_2BPP:
		case COMPOSITE_LCDC_IMG_FMT_4BPP:
		case COMPOSITE_LCDC_IMG_FMT_8BPP:
		case COMPOSITE_LCDC_IMG_FMT_RGB332:
			*offset0 = width;
			*offset1 = 0;
			break;
			
		case COMPOSITE_LCDC_IMG_FMT_RGB444:
		case COMPOSITE_LCDC_IMG_FMT_RGB565:
		case COMPOSITE_LCDC_IMG_FMT_RGB555:
			*offset0 = width * 2;
			*offset1 = 0;
			break;

		case COMPOSITE_LCDC_IMG_FMT_RGB888:
		case COMPOSITE_LCDC_IMG_FMT_RGB666:
			*offset0 = width * 4;
			*offset1 = 0;
			break;

		case COMPOSITE_LCDC_IMG_FMT_YUV420SP:
		case COMPOSITE_LCDC_IMG_FMT_YUV422SP:
			*offset0 = width;
			*offset1 = width/2;
			break;

		case COMPOSITE_LCDC_IMG_FMT_YUV422SQ:
			*offset0 = width * 2;
			*offset1 = 0;
			break;

		case COMPOSITE_LCDC_IMG_FMT_YUV420ITL0:
		case COMPOSITE_LCDC_IMG_FMT_YUV420ITL1:
		case COMPOSITE_LCDC_IMG_FMT_YUV422ITL0:
		case COMPOSITE_LCDC_IMG_FMT_YUV422ITL1:
			*offset0 = width;
			*offset1 = width;
			break;
	}
}

/*****************************************************************************
 Function Name: tcc_composite_set_imageinfo()
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
unsigned int tcc_composite_set_imageinfo (unsigned int flag, unsigned int type, unsigned int width, unsigned int height, unsigned int hpos, unsigned int vpos, unsigned int hoffset0, unsigned int hoffset1, unsigned int hscale, unsigned int vscale)
{
	unsigned int uiCH0Position = 0;
	
	switch (type) 
	{
		case IMG_CH0 :
			break;
			
		case IMG_CH1 :
			break;
			
		case IMG_CH2 :
			break;

		default	:
			break;
	}
	return uiCH0Position;
}

/*****************************************************************************
 Function Name : tcc_composite_set_imagectrl()
******************************************************************************/
void tcc_composite_set_imagectrl(unsigned int flag, unsigned int type, COMPOSITE_LCDC_IMG_CTRL_TYPE *imgctrl)
{
	unsigned int IMG_Ctrl_reg;
	
	switch (type) 
	{
		case IMG_CH0:
			break;

		case IMG_CH1:
			break;

		case IMG_CH2:
			break;

		default:
			return;
	}

	switch (type) 
	{
		case IMG_CH0:
			break;

		case IMG_CH1:
			break;

		case IMG_CH2:
			break;

		default:
			return;
	}
}


static int onthefly_using;
/*****************************************************************************
 Function Name : tcc_composite_update()
******************************************************************************/
void tcc_composite_update(struct tcc_lcdc_image_update *ImageInfo)
{
	VIOC_DISP * pDISPBase;
	VIOC_WMIX * pWMIXBase;
	VIOC_RDMA * pRDMABase;
	unsigned int lcd_width = 0, lcd_height = 0, lcd_h_pos = 0, lcd_w_pos = 0, scale_x = 0, scale_y = 0;

	VIOC_SC *pSC;
	pSC = (VIOC_SC *)tcc_p2v(HwVIOC_SC1);

	
	dprintk("%s enable:%d, layer:%d, fmt:%d, Fw:%d, Fh:%d, Iw:%d, Ih:%d, fmt:%d onthefly:%d\n", __func__, ImageInfo->enable, ImageInfo->Lcdc_layer,
			ImageInfo->fmt,ImageInfo->Frame_width, ImageInfo->Frame_height, ImageInfo->Image_width, ImageInfo->Image_height, ImageInfo->fmt, ImageInfo->on_the_fly);
	
	if((ImageInfo->Lcdc_layer >= 3) || (ImageInfo->fmt >TCC_LCDC_IMG_FMT_MAX))
		return;

	if(Composite_LCDC_Num)
	{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);		

		switch(ImageInfo->Lcdc_layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA06);
				break;
		}
	}
	else
	{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);
		
		switch(ImageInfo->Lcdc_layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA02);
				break;
		}		
	}

	VIOC_DISP_GetSize(pDISPBase, &lcd_width, &lcd_height);
	
	if((!lcd_width) || (!lcd_height))
		return;
	
	if(!ImageInfo->enable)	{
		VIOC_RDMA_SetImageDisable(pRDMABase);		

		if(ISSET(onthefly_using, 1<<ImageInfo->Lcdc_layer))
			{
			VIOC_CONFIG_PlugOut(VIOC_SC1);
			BITCLR(onthefly_using, 1 << ImageInfo->Lcdc_layer);
		}		
		return;
	}	

	if(ImageInfo->on_the_fly)
	{
		unsigned int RDMA_NUM;
		RDMA_NUM = Composite_LCDC_Num ? (ImageInfo->Lcdc_layer + VIOC_SC_RDMA_04) : ImageInfo->Lcdc_layer;

#if defined(CONFIG_MACH_TCC8920ST)
		if(!onthefly_using) {
			dprintk(" %s  scaler 1 is plug in RDMA %d \n",__func__, RDMA_NUM);
			BITSET(onthefly_using, 1 << ImageInfo->Lcdc_layer);
			VIOC_CONFIG_PlugIn (VIOC_SC1, RDMA_NUM);			
		}

		if(ImageInfo->Frame_width==ImageInfo->Image_width || ImageInfo->Frame_height==ImageInfo->Image_height) {
			VIOC_SC_SetBypass (pSC, ON);
			dprintk(" %s  scaler 1 is plug in SetBypass ON \n",__func__);
		}else {
			VIOC_SC_SetBypass (pSC, OFF);
			dprintk(" %s  scaler 1 is plug in SetBypass OFF \n",__func__);
		}
#else
		if(!onthefly_using) {
			dprintk(" %s  scaler 1 is plug in RDMA %d \n",__func__, RDMA_NUM);
			BITSET(onthefly_using, 1 << ImageInfo->Lcdc_layer);
			VIOC_CONFIG_PlugIn (VIOC_SC1, RDMA_NUM);			
			VIOC_SC_SetBypass (pSC, OFF);
		}
#endif
		VIOC_SC_SetSrcSize(pSC, ImageInfo->Frame_width, ImageInfo->Frame_height);
		VIOC_SC_SetDstSize (pSC, ImageInfo->Image_width, ImageInfo->Image_height);			// set destination size in scaler
		VIOC_SC_SetOutSize (pSC, ImageInfo->Image_width, ImageInfo->Image_height);			// set output size in scaer
	}
	else
	{
		if(ISSET(onthefly_using, 1<<ImageInfo->Lcdc_layer))
		{
			dprintk(" %s  scaler 1 is plug  \n",__func__);
			VIOC_RDMA_SetImageDisable(pRDMABase);
			VIOC_CONFIG_PlugOut(VIOC_SC1);
			BITCLR(onthefly_using, 1 << ImageInfo->Lcdc_layer);
		}
	}

	dprintk("%s lcdc:%d, pRDMA:0x%08x, pWMIX:0x%08x, pDISP:0x%08x, addr0:0x%08x\n", __func__, Composite_LCDC_Num, pRDMABase, pWMIXBase, pDISPBase, ImageInfo->addr0);
		
	if(machine_is_tcc8920st()) {
		VIOC_RDMA_SetImageUVIEnable(pRDMABase, TRUE);
	}
		
	if(ImageInfo->fmt >= TCC_LCDC_IMG_FMT_UYVY && ImageInfo->fmt <= TCC_LCDC_IMG_FMT_YUV422ITL1)
	{
		VIOC_RDMA_SetImageY2REnable(pRDMABase, TRUE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase, 0); /* Y2RMode Default 0 (Studio Color) */
	}

	VIOC_RDMA_SetImageOffset(pRDMABase, ImageInfo->fmt, ImageInfo->Frame_width);
	VIOC_RDMA_SetImageFormat(pRDMABase, ImageInfo->fmt);

	scale_x = 0;
	scale_y = 0;
	lcd_h_pos = 0;
	lcd_w_pos = 0;

	if(lcd_width > ImageInfo->Image_width)
		lcd_w_pos = (lcd_width - ImageInfo->Image_width)/2;
	
	if(lcd_height > ImageInfo->Image_height)
		lcd_h_pos = (lcd_height - ImageInfo->Image_height)/2;

	dprintk("%s lcd_width:%d, lcd_height:%d, lcd_w_pos:%d, lcd_h_pos:%d\n\n", __func__, lcd_width, lcd_height, lcd_w_pos, lcd_h_pos);
	
	// position
	//pLCDC_channel->LIP = ((lcd_h_pos << 16) | (lcd_w_pos));
	VIOC_WMIX_SetPosition(pWMIXBase, ImageInfo->Lcdc_layer, lcd_w_pos, lcd_h_pos);

	// scale
	//pLCDC_channel->LISR =((scale_y<<4) | scale_x);
	VIOC_RDMA_SetImageScale(pRDMABase, scale_x, scale_y);
	
	if( !( ((ImageInfo->crop_left == 0) && (ImageInfo->crop_right == ImageInfo->Frame_width)) &&  ((ImageInfo->crop_top == 0) && (ImageInfo->crop_bottom == ImageInfo->Frame_height)))  )
	{
		dprintk(" Image Crop left=[%d], right=[%d], top=[%d], bottom=[%d] \n", ImageInfo->crop_left, ImageInfo->crop_right, ImageInfo->crop_top, ImageInfo->crop_bottom);
		int addr_Y = (unsigned int)ImageInfo->addr0;
		int addr_U = (unsigned int)ImageInfo->addr1;
		int addr_V = (unsigned int)ImageInfo->addr2;
		
		tccxxx_GetAddress(ImageInfo->fmt, (unsigned int)ImageInfo->addr0, ImageInfo->Frame_width, ImageInfo->Frame_height, 		
								ImageInfo->crop_left, ImageInfo->crop_top, &addr_Y, &addr_U, &addr_V);		

		VIOC_RDMA_SetImageSize(pRDMABase, ImageInfo->crop_right - ImageInfo->crop_left, ImageInfo->crop_bottom - ImageInfo->crop_top);
		VIOC_RDMA_SetImageBase(pRDMABase, addr_Y, addr_U, addr_V);
	}
	else
	{	
		dprintk(" don't Image Crop left=[%d], right=[%d], top=[%d], bottom=[%d] \n", ImageInfo->crop_left, ImageInfo->crop_right, ImageInfo->crop_top, ImageInfo->crop_bottom);
		VIOC_RDMA_SetImageSize(pRDMABase, ImageInfo->Frame_width, ImageInfo->Frame_height);
		VIOC_RDMA_SetImageBase(pRDMABase, ImageInfo->addr0, ImageInfo->addr1, ImageInfo->addr2);		
	}
		
	// position
	//if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI)) //--
#if 0//defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT) || defined(TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
	if(ImageInfo->deinterlace_mode == 1 && ImageInfo->output_toMemory == 0)
	{
		VIOC_RDMA_SetImageY2REnable(pRDMABase, FALSE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase, 0x02); /* Y2RMode Default 0 (Studio Color) */
		VIOC_RDMA_SetImageIntl(pRDMABase, 1);
		VIOC_RDMA_SetImageBfield(pRDMABase, ImageInfo->odd_first_flag);
		VIOC_WMIX_SetPosition(pWMIXBase, ImageInfo->Lcdc_layer,  ImageInfo->offset_x, (ImageInfo->offset_y/2) );
	}
	else
#endif		
	{
		VIOC_RDMA_SetImageIntl(pRDMABase, 0);
		VIOC_WMIX_SetPosition(pWMIXBase, ImageInfo->Lcdc_layer, ImageInfo->offset_x, ImageInfo->offset_y);
	}

	VIOC_RDMA_SetImageEnable(pRDMABase);

	if(onthefly_using)
		VIOC_SC_SetUpdate (pSC);

	VIOC_WMIX_SetUpdate(pWMIXBase);
}

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
/*****************************************************************************
 Function Name : tcc_composite_process()
******************************************************************************/
void tcc_composite_process(struct tcc_lcdc_image_update *update, char force_update)
{
	COMPOSITE_LCDC_IMG_CTRL_TYPE ImgCtrl;
	unsigned int Y_offset, UV_offset;
	unsigned int img_width = 0, img_height = 0, win_offset_x = 0, win_offset_y = 0;
	unsigned int lcd_w = 0, lcd_h =0;
	unsigned int lcd_ctrl_flag, mixer_interrupt_use = 0;
	exclusive_ui_ar_params aspect_ratio;
	PLCDC pLCDC;
	
	if(Composite_LCDC_Num)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
		
	tcc_composite_get_lcdsize(&lcd_w, &lcd_h);

	img_width = update->Image_width;
	img_height = update->Image_height;

	dprintk("%s, img_w=%d, img_h=%d, lcd_w=%d, lcd_h=%d, plane_w=%d, plane_ht=%d\n", __func__,
			img_width, img_height, lcd_w, lcd_h, composite_exclusive_ui_param.plane_width, composite_exclusive_ui_param.plane_height);

	/* Aspect Ratio Conversion for DVD Contents */
	//if((img_width <= COMPOSITE_EXCLUSIVE_UI_DVD_MAX_WD) && (img_height <= COMPOSITE_EXCLUSIVE_UI_DVD_MAX_HT))
	{
		memset((void *)&aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		TCC_OUTPUT_EXCLUSIVE_UI_CfgAR(Composite_LCDC_Num, img_width, img_height, &aspect_ratio);
	}

	/* Get the parameters for exclusive ui */
	TCC_OUTPUT_EXCLUSIVE_UI_GetParam(&composite_exclusive_ui_param);

	if(composite_exclusive_ui_param.enable)
	{
		/* Clear the on_the_flay for video */
		composite_exclusive_ui_onthefly = FALSE;

		/* Clear update flag and backup an information of the last video */
		TCC_OUTPUT_EXCLUSIVE_UI_SetImage(update);

		/*-------------------------------------*/
		/*           INTERLACE VIDEO           */
		/* Update video data with exclusive ui */
		/*-------------------------------------*/
		if(composite_exclusive_ui_param.interlace)
		{
			/* Interlace BD Contents with 1920x1080 graphic plane */
			if(!TCC_OUTPUT_EXCLUSIVE_UI_ViqeMtoM(Composite_LCDC_Num, img_width, img_height))
			{
				if((img_width == lcd_w) && (img_height == lcd_h))
				{
					/* 1920x1080 COMPOSITE Output */
				}
				else
				{
					/* 1280x720 COMPOSITE Output */
				}
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(Composite_LCDC_Num, FALSE);
				
				/* VIQE and M2M Scaler with On_the_fly */
				/* Check the output format */
				if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
					lcd_h = lcd_h >> 1;
			
				if(composite_exclusive_ui_param.clear == TRUE)
				{
					/* M2M Scaler without On_the_fly */
					if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(update, Composite_LCDC_Num) < 0)
						composite_exclusive_ui_onthefly = FALSE;
					else
						composite_exclusive_ui_onthefly = TRUE;

					/* Update Screen */
					tcc_composite_update(update);
				}
				else
				{
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(update, COMPOSITE_EXCLUSIVE_UI_SCALER1, composite_exclusive_ui_param.plane_width, composite_exclusive_ui_param.plane_height, composite_exclusive_ui_idx, 0);

					if(TCC_OUTPUT_EXCLUSIVE_UI_GetUpdate())
					{
						dprintk("E\n");
					}
					else
					{
						/* Overlay Mixer */
						TCC_OUTPUT_EXCLUSIVE_UI_Mixer(update, composite_exclusive_ui_idx, 0);
						/* Update overlay mixer image */
						TCC_OUTPUT_EXCLUSIVE_UI_Restore_Mixer(update, Composite_LCDC_Num, lcd_w, lcd_h, composite_exclusive_ui_idx);
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
			if( (img_width == composite_exclusive_ui_param.plane_width) && (img_width == lcd_w) && (img_width == TCC_FB_OUT_MAX_WIDTH) &&
				(img_height == composite_exclusive_ui_param.plane_height) && (img_height == lcd_h) && (img_height == TCC_FB_OUT_MAX_HEIGHT) )
			{
				/* 1920x1080 COMPOSITE Output */
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(Composite_LCDC_Num, FALSE);

				if(composite_exclusive_ui_param.clear == FALSE)
				{
					/* M2M Scaler without On_the_fly */
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(update, COMPOSITE_EXCLUSIVE_UI_SCALER0, composite_exclusive_ui_param.plane_width, composite_exclusive_ui_param.plane_height, composite_exclusive_ui_idx, 0);
					/* Overlay Mixer */
					TCC_OUTPUT_EXCLUSIVE_UI_Mixer(update, composite_exclusive_ui_idx, 0);
 				}
				
			#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
				/* M2M Scaler without On_the_fly */
				TCC_OUTPUT_EXCLUSIVE_UI_Scaler(update, COMPOSITE_EXCLUSIVE_UI_SCALER0, lcd_w, lcd_h, composite_exclusive_ui_idx, 1);
				//hdmi_exclusive_ui_onthefly = FALSE;
			#else
				/* M2M Scaler with On_the_fly */
				if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(update, Composite_LCDC_Num) < 0)
					composite_exclusive_ui_onthefly = FALSE;
				else
					composite_exclusive_ui_onthefly = TRUE;
			#endif
			}

			#if 0 //shkim - on_the_fly
			/* Set the on_the_flay for android UI */
			if(composite_exclusive_ui_onthefly == FALSE)
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(Composite_LCDC_Num, 1, TRUE);
			else
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(Composite_LCDC_Num, 1, FALSE);
			#endif
		}

		composite_exclusive_ui_idx = !composite_exclusive_ui_idx;
	}
	
	if(force_update &&  !mixer_interrupt_use)
	{
		tcc_composite_update(update);
	}
}

void tcc_composite_display_image(exclusive_ui_update UpdateImage)
{
	dprintk("%s\n", __func__);
			
	/* M2M Scaler with On_the_fly */
	if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(&UpdateImage.image, UpdateImage.lcdc) < 0)
		composite_exclusive_ui_onthefly = FALSE;
	else
		composite_exclusive_ui_onthefly = TRUE;

	/* Update Screen */
	tcc_composite_update(&UpdateImage.image);
}
#endif

/*****************************************************************************
 Function Name : tcc_composite_get_mode()
******************************************************************************/
TCC_COMPOSITE_MODE_TYPE tcc_composite_get_mode(void)
{
	return tcc_composite_mode;
}

/*****************************************************************************
 Function Name : tcc_composite_enabled()
******************************************************************************/
int tcc_composite_enabled(void)
{
	volatile PPMU 		pHwPMU = (volatile PPMU)tcc_p2v(HwPMU_BASE);
	volatile PNTSCPAL 	pHwTVE = (volatile PNTSCPAL)tcc_p2v(HwTVE_BASE);
	volatile PNTSCPAL_ENCODER_CTRL 	pHwTVE_VEN = (PNTSCPAL_ENCODER_CTRL)HwNTSCPAL_ENC_CTRL_BASE;

	int iEnabled = 0;
	
	if(pHwTVE_VEN->VENCON.nREG & HwTVEVENCON_EN_EN)
	{
		iEnabled = 1;
	}

	return iEnabled;
}

/*****************************************************************************
 Function Name : tcc_composite_end()
******************************************************************************/
void tcc_composite_end(void)
{
	dprintk("%s, LCDC_Num = %d \n", __func__, Composite_LCDC_Num);

	internal_tve_enable(0, 0);

	#if (defined(CONFIG_CPU_FREQ_TCC92X) || defined (CONFIG_CPU_FREQ_TCC93XX)) && defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtTvClockLimitTable, TCC_FREQ_LIMIT_TV, 0);
	#endif

	if(composite_plugout)
		composite_plugout = 0;
}

/*****************************************************************************
 Function Name : tcc_composite_start()
******************************************************************************/
void tcc_composite_start(TCC_COMPOSITE_MODE_TYPE mode)
{
	COMPOSITE_MODE_TYPE composite_mode;
	PVIOC_DISP				pDISPBase;
	
	pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);

	dprintk("%s mode=%d, lcdc_num=%d\n", __func__, mode, Composite_LCDC_Num);

	tcc_composite_mode = mode;

	if(mode == COMPOSITE_NTST_M)
		composite_mode = NTSC_M;
	else
		composite_mode = PAL_B;

	tcc_composite_connect_lcdc(Composite_LCDC_Num);
	tcc_composite_set_lcd2tv(composite_mode);

	internal_tve_enable(composite_mode, 1);
}

/*****************************************************************************
 Function Name : tcc_composite_clock_onoff()
******************************************************************************/
void tcc_composite_clock_onoff(char OnOff)
{
	dprintk("%s, ch = %d OnOff = %d \n", __func__, Composite_LCDC_Num, OnOff);

	if(OnOff)
	{
		if(Composite_LCDC_Num)
			clk_enable(composite_lcdc1_clk);
		else
			clk_enable(composite_lcdc0_clk);
		
		clk_enable(composite_dac_clk);
		clk_enable(composite_ntscpal_clk);
	}
	else
	{
		if(Composite_LCDC_Num)
			clk_disable(composite_lcdc1_clk);
		else		
			clk_disable(composite_lcdc0_clk);

		clk_disable(composite_dac_clk);
		clk_disable(composite_ntscpal_clk);
	}
}

/*****************************************************************************
 Function Name : tcc_composite_ioctl()
******************************************************************************/
static long tcc_composite_ioctl(struct file *file, unsigned int cmd, void *arg)
{
	TCC_COMPOSITE_START_TYPE start;
	TCC_COMPOSITE_MODE_TYPE type;
	struct tcc_lcdc_image_update update;			
	
	dprintk("composite_ioctl IOCTRL[%d], lcdc_num=%d\n", cmd, Composite_LCDC_Num);

	switch(cmd)
	{
		case TCC_COMPOSITE_IOCTL_START:
			copy_from_user(&start,arg,sizeof(start));

			TCC_OUTPUT_FB_DetachOutput(1);
			
			#if !defined(CONFIG_MACH_TCC8920ST)
			start.lcdc = tca_get_output_lcdc_num();
			#endif /* CONFIG_MACH_TCC8920ST */
			
			TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_COMPOSITE, start.lcdc, TRUE);

			if(start.lcdc != Composite_LCDC_Num)
			{
				tcc_composite_set_lcdc(start.lcdc);
				tcc_composite_clock_onoff(TRUE);
			}
			tcc_composite_start(start.mode);

			TCC_OUTPUT_UPDATE_OnOff(1, TCC_OUTPUT_COMPOSITE);

#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
			if( tcc_vsync_get_isVsyncRunning() )
				tca_vsync_video_display_enable();
#endif
			break;

		case TCC_COMPOSITE_IOCTL_UPDATE:
			copy_from_user(&update,arg,sizeof(update));
			tcc_composite_update(&update);									
			break;
			
		case TCC_COMPOSITE_IOCTL_END:
			tcc_composite_end();

			TCC_OUTPUT_LCDC_OnOff(TCC_OUTPUT_COMPOSITE, Composite_LCDC_Num, FALSE);			
						
#ifdef TCC_VIDEO_DISPLAY_BY_VSYNC_INT
			if( tcc_vsync_get_isVsyncRunning() )
				tca_vsync_video_display_disable();
			tcc_vsync_set_firstFrameFlag(1);
#endif
			break;

		case TCC_COMPOSITE_IOCTL_PROCESS:
			#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
				copy_from_user(&update,arg,sizeof(update));
				tcc_composite_process(&update, 0);
				copy_to_user(arg,&update,sizeof(update));
			#endif
			break;

		case TCC_COMPOSITE_IOCTL_ATTACH:
			#if defined(CONFIG_TCC_OUTPUT_ATTACH)
				copy_from_user(&start,arg,sizeof(start));
				tcc_composite_mode = start.mode;

				TCC_OUTPUT_FB_DetachOutput(0);
				TCC_OUTPUT_FB_AttachOutput(start.lcdc, TCC_OUTPUT_COMPOSITE, tcc_composite_mode, 0);
			#endif
			break;

		case TCC_COMPOSITE_IOCTL_DETACH:
			#if defined(CONFIG_TCC_OUTPUT_ATTACH)
				TCC_OUTPUT_FB_DetachOutput(0);
			#endif
			break;
			
		default:
			printk(" Unsupported IOCTL!!!\n");      
			break;			
	}

	return 0;
}

/*****************************************************************************
 Function Name : tcc_composite_attach()
******************************************************************************/
void tcc_composite_attach(char lcdc_num, char mode, char onoff)
{
	char composite_mode;

	dprintk("%s, lcdc_num=%d, onoff=%d, mode=%d\n", __func__, lcdc_num, onoff, mode);

	if(onoff)
	{
		if(mode >= COMPOSITE_MAX_M)
			composite_mode = COMPOSITE_NTST_M;
		else
			composite_mode = mode;

		tcc_composite_set_lcdc(lcdc_num);
		tcc_composite_clock_onoff(TRUE);
		tcc_composite_start(composite_mode);
	}
	else
	{
		tcc_composite_end();
	}
}

/*****************************************************************************
 Function Name : tcc_composite_open()
******************************************************************************/
static int tcc_composite_open(struct inode *inode, struct file *filp)
{	
	dprintk("%s  \n", __func__);

	tcc_composite_clock_onoff(1);

	return 0;
}

/*****************************************************************************
 Function Name : tcc_composite_release()
******************************************************************************/
static int tcc_composite_release(struct inode *inode, struct file *file)
{
	dprintk(" %s  \n", __func__);

	tcc_composite_clock_onoff(0);

	return 0;
}

static struct file_operations tcc_composite_fops = 
{
	.owner          = THIS_MODULE,
	.unlocked_ioctl = tcc_composite_ioctl,
	.open           = tcc_composite_open,
	.release        = tcc_composite_release,	
};

static struct class *tcc_composite_class;

/*****************************************************************************
 Function Name : tcc_composite_init()
******************************************************************************/
int __init tcc_composite_init(void)
{
	printk("%s  \n", __func__);

	composite_lcdc0_clk = clk_get(0, "lcdc0");
	BUG_ON(composite_lcdc0_clk == NULL);
	composite_lcdc1_clk = clk_get(0, "lcdc1");
	BUG_ON(composite_lcdc1_clk == NULL);
	composite_dac_clk = clk_get(0, "vdac_phy");
	BUG_ON(composite_dac_clk == NULL);
	composite_ntscpal_clk = clk_get(0, "ntscpal");
	BUG_ON(composite_ntscpal_clk == NULL);

	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_composite_fops);
	tcc_composite_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(tcc_composite_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);

	tcc_composite_clock_onoff(1);
	internal_tve_enable(0, 0);
	tcc_composite_clock_onoff(0);

	#if defined (CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC8800ST)
		#if defined(CONFIG_TCC_OUTPUT_AUTO_DETECTION) && defined(TCC_OUTPUT_AUTO_ALL)
			tcc_gpio_config(COMPOSITE_DETECT_GPIO, GPIO_FN(0));
			gpio_request(COMPOSITE_DETECT_GPIO, NULL);
			gpio_direction_input(COMPOSITE_DETECT_GPIO);
		#endif
	#endif

	return 0;
}

/*****************************************************************************
 Function Name : tcc_composite_cleanup()
******************************************************************************/
void __exit tcc_composite_cleanup(void)
{
	printk("%s LCDC:%d \n", __func__, Composite_LCDC_Num);

	unregister_chrdev(MAJOR_ID, DEVICE_NAME);

	if(composite_lcdc0_clk =! NULL)
		clk_put(composite_lcdc0_clk);
	if(composite_lcdc1_clk =! NULL)
		clk_put(composite_lcdc1_clk);
	if(composite_dac_clk =! NULL)
		clk_put(composite_dac_clk);
	if(composite_ntscpal_clk =! NULL)
		clk_put(composite_ntscpal_clk);

	return;
}

module_init(tcc_composite_init);
module_exit(tcc_composite_cleanup);

MODULE_AUTHOR("Telechps");
MODULE_DESCRIPTION("Telechips COMPOSITE Out driver");
MODULE_LICENSE("GPL");


