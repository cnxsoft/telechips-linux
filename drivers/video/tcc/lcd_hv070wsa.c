/*
 * lcd_hv070wsa.c
 *
 * Copyright (C) 2009, 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <linux/err.h>

#include <mach/tcc_fb.h>
#include <mach/gpio.h>
#include <mach/tca_lcdc.h>
#include <mach/TCC_LCD_Interface.h>
#include <mach/globals.h>
#include <mach/reg_physical.h>
#include <mach/tca_ckc.h>
#include <mach/tca_tco.h>
#include <asm/mach-types.h>
#include <asm/io.h>

static struct mutex panel_lock;

static unsigned int lcd_bl_level;
extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);
static struct clk *lvds_clk;
static int hv070wsa_panel_init(struct lcd_panel *panel)
{
	printk("%s : %d\n", __func__, 0);
	return 0;
}

static int hv070wsa_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
	PDDICONFIG	pDDICfg;
	unsigned int P, M, S, VSEL;
	struct lcd_platform_data *pdata = panel->dev->platform_data;

	printk("%s : %d %d  \n", __func__, on, lcd_bl_level);

	if(!on && panel->bl_level)
		panel->set_backlight_level(panel , 0);

	mutex_lock(&panel_lock);
	panel->state = on;

	pDDICfg = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	if (on) {
		gpio_set_value(pdata->power_on, 1);

		// LVDS power on
		clk_enable(lvds_clk);	
		
		lcdc_initialize(panel, lcd_num);

#if defined(CONFIG_ARCH_TCC892X)
			
		// LVDS reset	
		pDDICfg->LVDS_CTRL.bREG.RST =1;
		pDDICfg->LVDS_CTRL.bREG.RST =0;		

		#if 0
		BITCSET(pDDICfg->LVDS_TXO_SEL0.nREG, 0xFFFFFFFF, 0x15141312);
		BITCSET(pDDICfg->LVDS_TXO_SEL1.nREG, 0xFFFFFFFF, 0x0B0A1716);
		BITCSET(pDDICfg->LVDS_TXO_SEL2.nREG, 0xFFFFFFFF, 0x0F0E0D0C);
		BITCSET(pDDICfg->LVDS_TXO_SEL3.nREG, 0xFFFFFFFF, 0x05040302);
		BITCSET(pDDICfg->LVDS_TXO_SEL4.nREG, 0xFFFFFFFF, 0x1A190706);
		BITCSET(pDDICfg->LVDS_TXO_SEL5.nREG, 0xFFFFFFFF, 0x1F1E1F18);
		BITCSET(pDDICfg->LVDS_TXO_SEL6.nREG, 0xFFFFFFFF, 0x1F1E1F1E);
		BITCSET(pDDICfg->LVDS_TXO_SEL7.nREG, 0xFFFFFFFF, 0x1F1E1F1E);
		BITCSET(pDDICfg->LVDS_TXO_SEL8.nREG, 0xFFFFFFFF, 0x001E1F1E);
		#else
		BITCSET(pDDICfg->LVDS_TXO_SEL0.nREG, 0xFFFFFFFF, 0x13121110);
		BITCSET(pDDICfg->LVDS_TXO_SEL1.nREG, 0xFFFFFFFF, 0x09081514);
		BITCSET(pDDICfg->LVDS_TXO_SEL2.nREG, 0xFFFFFFFF, 0x0D0C0B0A);
		BITCSET(pDDICfg->LVDS_TXO_SEL3.nREG, 0xFFFFFFFF, 0x03020100);
		
		BITCSET(pDDICfg->LVDS_TXO_SEL4.nREG, 0xFFFFFFFF, 0x1A190504);
		BITCSET(pDDICfg->LVDS_TXO_SEL5.nREG, 0xFFFFFFFF, 0x0E171618);
		BITCSET(pDDICfg->LVDS_TXO_SEL6.nREG, 0xFFFFFFFF, 0x1F07060F);
		BITCSET(pDDICfg->LVDS_TXO_SEL7.nREG, 0xFFFFFFFF, 0x1F1E1F1E);
		BITCSET(pDDICfg->LVDS_TXO_SEL8.nREG, 0xFFFFFFFF, 0x001E1F1E);
		#endif//
		
		M = 10; P = 10; S = 0; VSEL = 0;		
		BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x00FFFFF0, (VSEL<<4)|(S<<5)|(M<<8)|(P<<15)); //LCDC1

		// LVDS Select LCDC 1
		if(pdata->lcdc_num ==1)
			BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x3 << 30 , 0x1 << 30);
		else
			BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x3 << 30 , 0x0 << 30);

	    	pDDICfg->LVDS_CTRL.bREG.RST = 1;	//  reset
	  	pDDICfg->LVDS_CTRL.bREG.EN = 1;   // enable
#else
		// LVDS reset
		BITSET(pDDICfg->LVDS_CTRL, Hw1);	// reset
		BITCLR(pDDICfg->LVDS_CTRL, Hw1);	// normal
	
		pDDICfg->LVDS_TXO_SEL0 = 0x15141312; // SEL_03, SEL_02, SEL_01, SEL_00,
		pDDICfg->LVDS_TXO_SEL1 = 0x0B0A1716; // SEL_07, SEL_06, SEL_05, SEL_04,
		pDDICfg->LVDS_TXO_SEL2 = 0x0F0E0D0C; // SEL_11, SEL_10, SEL_09, SEL_08,
		pDDICfg->LVDS_TXO_SEL3 = 0x05040302; // SEL_15, SEL_14, SEL_13, SEL_12,
		pDDICfg->LVDS_TXO_SEL4 = 0x1A190706; // SEL_19, SEL_18, SEL_17, SEL_16,
		pDDICfg->LVDS_TXO_SEL5 = 0x1F1E1F18; // 						SEL_20,
		pDDICfg->LVDS_TXO_SEL6 = 0x1F1E1F1E;
		pDDICfg->LVDS_TXO_SEL7 = 0x1F1E1F1E;
		pDDICfg->LVDS_TXO_SEL8 = 0x1F1E1F1E;
		
		// LVDS Select
	//	BITCLR(pDDICfg->LVDS_CTRL, Hw0); //LCDC0
		BITSET(pDDICfg->LVDS_CTRL, Hw0); //LCDC1
	
	
		#ifdef CONFIG_ARCH_TCC88XX
		{

			M = 10; P = 10; S = 0; VSEL = 0;
			BITCSET(pDDICfg->LVDS_CTRL, Hw21- Hw4, (VSEL<<4)|(S<<5)|(M<<8)|(P<<15)); //LCDC1
		}

		BITSET(pDDICfg->LVDS_CTRL, Hw1);	// reset
		#endif//
		
		// LVDS enable
		BITSET(pDDICfg->LVDS_CTRL, Hw2);	// enable
#endif		
	
	}
	else 
	{
		#if defined(CONFIG_ARCH_TCC892X)
			pDDICfg->LVDS_CTRL.bREG.RST = 1;		// reset		
			pDDICfg->LVDS_CTRL.bREG.EN = 0;		// reset		
		#else	
			#if defined(CONFIG_ARCH_TCC88XX)		
				BITCLR(pDDICfg->LVDS_CTRL, Hw1);	// reset			
			#endif
			BITCLR(pDDICfg->LVDS_CTRL, Hw2);	// disable
		#endif
		
		clk_disable(lvds_clk);	
		gpio_set_value(pdata->power_on, 0);

	}
	mutex_unlock(&panel_lock);
	
	if(on && panel->bl_level)
		panel->set_backlight_level(panel , panel->bl_level);	

	return 0;
}

static int hv070wsa_set_backlight_level(struct lcd_panel *panel, int level)
{
    #define MAX_BACKLIGTH 255
	struct lcd_platform_data *pdata = panel->dev->platform_data;

	//printk("%s state:%d level:%d  gpio:0x%x \n " , __func__, panel->state, level, pdata->bl_on);

	mutex_lock(&panel_lock);

	panel->bl_level = level;
	
	if (level == 0) {
		gpio_set_value(pdata->bl_on, 0);
	} else {
		if(panel->state) {
			tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);
		}
	}
	mutex_unlock(&panel_lock);
	return 0;
}

struct lcd_panel hv070wsa_panel = {
	.name		= "HV070WSA",
	.manufacturer	= "BOE",
	.id		= PANEL_ID_HV070WSA,
	.xres		= 1024,
	.yres		= 600,
	.width		= 153,
	.height		= 90,
	.bpp		= 24,
	.clk_freq	= 512000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 0,
	.lpc		= 1024,
	.lswc		= 0,
	.lewc		= 323,	
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 0,
	.flc1		= 600,
	.fswc1		= 0,
	.fewc1		= 34,	
	.fpw2		= 0,
	.flc2		= 600,
	.fswc2		= 0,
	.fewc2		= 34,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= hv070wsa_panel_init,
	.set_power	= hv070wsa_set_power,
	.set_backlight_level = hv070wsa_set_backlight_level,
};

static int hv070wsa_probe(struct platform_device *pdev)
{
	struct lcd_platform_data *pdata = pdev->dev.platform_data;

	printk("%s : %s GPIO: power_on:0x%x bl_on:0x%x reset:0x%x  \n", __func__,  pdev->name, pdata->power_on, pdata->bl_on, pdata->reset);

	mutex_init(&panel_lock);
	
	hv070wsa_panel.state = 1;

	gpio_request(pdata->power_on, "lcd_on");
	gpio_request(pdata->bl_on, "lcd_bl");
	gpio_request(pdata->reset, "lcd_reset");

	gpio_direction_output(pdata->power_on, 1);
	gpio_direction_output(pdata->bl_on, 1);
	gpio_direction_output(pdata->reset, 1);
	
	hv070wsa_panel.dev = &pdev->dev;

	#if defined(CONFIG_ARCH_TCC892X)
		lvds_clk = clk_get(0, "lvds_phy"); //8920
		if(IS_ERR(lvds_clk))
		lvds_clk = NULL;
	#else
		lvds_clk = clk_get(0, "lvds");
		BUG_ON(lvds_clk == NULL);
	#endif
	clk_enable(lvds_clk);	
	
	tccfb_register_panel(&hv070wsa_panel);
	return 0;
}

static int hv070wsa_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver hv070wsa_lcd = {
	.probe	= hv070wsa_probe,
	.remove	= hv070wsa_remove,
	.driver	= {
		.name	= "hv070wsa_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int hv070wsa_init(void)
{
	printk("~ %s ~ \n", __func__);
	return platform_driver_register(&hv070wsa_lcd);
}

static __exit void hv070wsa_exit(void)
{
	platform_driver_unregister(&hv070wsa_lcd);
}

subsys_initcall(hv070wsa_init);
module_exit(hv070wsa_exit);

MODULE_DESCRIPTION("CLAA070MP01 LCD driver");
MODULE_LICENSE("GPL");
