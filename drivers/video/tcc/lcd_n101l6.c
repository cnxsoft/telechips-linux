
/*
 * n101l6_lcd.c -- support for CPT CLAA104XA01CW LCD
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
static char lcd_pwr_state;
static unsigned int lcd_bl_level;
extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);

static struct clk *lvds_clk;

static int n101l6_panel_init(struct lcd_panel *panel)
{
	printk("%s : %d\n", __func__, 0);
	return 0;
}

static int n101l6_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
	PDDICONFIG	pDDICfg;
	unsigned int P, M, S, VSEL;

	struct lcd_platform_data *pdata = panel->dev->platform_data;
//	printk("%s : %d %d  \n", __func__, on, panel->bl_level);

	mutex_lock(&panel_lock);
	
	panel->state = on;

	pDDICfg = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if (on) {

		gpio_set_value( (GPIO_PORTF|13) , 1);	// LVDS_Module_PWR	
		gpio_set_value(pdata->power_on, 1);		
		gpio_set_value(pdata->display_on, 1);	// LVDS LED_PWM
		gpio_set_value(pdata->reset, 1);		// LVDS LED_EN
		
		// LVDS power on
		clk_enable(lvds_clk);	
		
		lcdc_initialize(panel, lcd_num);
	
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
		M = 10; P = 10; S = 0; VSEL = 0;
		BITCSET(pDDICfg->LVDS_CTRL, Hw21- Hw4, (VSEL<<4)|(S<<5)|(M<<8)|(P<<15)); //LCDC1
	    BITSET(pDDICfg->LVDS_CTRL, Hw1);    // reset
	    #endif//
		
		// LVDS enable
		BITSET(pDDICfg->LVDS_CTRL, Hw2);	// enable
	}
	
	else 
	{
		#ifdef CONFIG_ARCH_TCC88XX	
		BITCLR(pDDICfg->LVDS_CTRL, Hw1);	// reset
		#endif//
		
		BITCLR(pDDICfg->LVDS_CTRL, Hw2);	// disable
		
		clk_disable(lvds_clk);	

		gpio_set_value(pdata->reset, 0);		
		gpio_set_value(pdata->display_on, 0);
		gpio_set_value(pdata->power_on, 0);
		gpio_set_value( (GPIO_PORTF|13) , 0);	// LVDS_Module_PWR	
	}
	
	mutex_unlock(&panel_lock);

	if(on)
		panel->set_backlight_level(panel , panel->bl_level);
	
	return 0;
}

static int n101l6_set_backlight_level(struct lcd_panel *panel, int level)
{

	#define MAX_BL_LEVEL	255	
	volatile PTIMER pTIMER;
	
	struct lcd_platform_data *pdata = panel->dev->platform_data;
	panel->bl_level = level;

	mutex_lock(&panel_lock);


	#define MAX_BACKLIGTH 255
	if (level == 0) {
		tca_tco_pwm_ctrl(0, pdata->display_on, MAX_BACKLIGTH, level);
		gpio_set_value(pdata->bl_on, 0);

	}
	else 
	{
		if(panel->state)
		{
			#if defined(CONFIG_ARCH_TCC892X)
			if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
				tca_tco_pwm_ctrl(0, pdata->display_on, ((MAX_BACKLIGTH<< 2) | 0x3), (MAX_BL_LEVEL - ((MAX_BL_LEVEL - level)) <<2) | (0x3 << 2 ));
			else
				tca_tco_pwm_ctrl(1, pdata->display_on, ((MAX_BACKLIGTH<< 2) | 0x3), (MAX_BL_LEVEL - ((MAX_BL_LEVEL - level)) <<2) | (0x3 << 2 ));
			#else
				tca_tco_pwm_ctrl(0, pdata->display_on, ((MAX_BACKLIGTH<< 2) | 0x3), (MAX_BL_LEVEL - ((MAX_BL_LEVEL - level)) <<2) | (0x3 << 2 ));		
			#endif//

			gpio_set_value(pdata->bl_on, 1);

		}
	}

	mutex_unlock(&panel_lock);
	return 0;
}
static struct lcd_panel n101l6_panel = {
	.name		= "N101L6",
	.manufacturer	= "CHI_MEI",
	.id			= PANEL_ID_N101L6,			// PANEL_ID_N101l6 is announced in the tcclcd.h (in /tcc88xx)	
	.xres		= 1024,		
	.yres		= 600,	
	.width		= 222,		
	.height		= 125,		
	.bpp		= 32,	
	.clk_freq	= 439700,	   
	.clk_div	= 2,	
	.bus_width	= 24,	
	.lpw		= 0,
	.lpc		= 1024,		
	.lswc		= 0,		
	.lewc		= 157,		
	.vdb		= 0,
	.vdf		= 0,	
	.fpw1		= 0,
	.flc1		= 600,		
	.fswc1		= 0,
	.fewc1		= 16,		
	.fpw2		= 0,
	.flc2		= 600,	
	.fswc2		= 0,
	.fewc2		= 16,		
	
	.init		= n101l6_panel_init,
	.set_power	= n101l6_set_power,
	.set_backlight_level = n101l6_set_backlight_level,
};


static int n101l6_probe(struct platform_device *pdev)
{
	struct lcd_platform_data *pdata = pdev->dev.platform_data;
	printk("%s : %s\n", __func__,  pdev->name);

	
	mutex_init(&panel_lock);

	gpio_request(pdata->power_on, "lvds_on");
	gpio_request(pdata->bl_on, "lvds_bl");
	gpio_request(pdata->display_on, "lvds_display");
	gpio_request(pdata->reset, "lvds_reset");

	gpio_direction_output(pdata->power_on, 1);
	gpio_direction_output(pdata->bl_on, 1);
	gpio_direction_output(pdata->display_on, 1);
	gpio_direction_output(pdata->reset, 1);

	n101l6_panel.dev = &pdev->dev;
	n101l6_panel.state = 1;
	
	lvds_clk = clk_get(0, "lvds");
	BUG_ON(lvds_clk == NULL);
	clk_enable(lvds_clk);	
	
	tccfb_register_panel(&n101l6_panel);
	return 0;
}

static int n101l6_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver n101l6_lcd = {
	.probe	= n101l6_probe,
	.remove	= n101l6_remove,
	.driver	= {
		.name	= "n101l6_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int n101l6_init(void)
{
	printk("~ %s ~ \n", __func__);
	return platform_driver_register(&n101l6_lcd);
}

static __exit void n101l6_exit(void)
{
	return platform_driver_unregister(&n101l6_lcd);
}

subsys_initcall(n101l6_init);
module_exit(n101l6_exit);

MODULE_DESCRIPTION("N101L6 LCD driver");
MODULE_LICENSE("GPL");




