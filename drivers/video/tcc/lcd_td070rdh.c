/*
 * tm070rdh11_lcd.c -- support for TPO TD070RDH11 LCD
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
#include <linux/mutex.h>

#include <mach/hardware.h>
#include <mach/bsp.h>
#include <asm/io.h>
#include <asm/mach-types.h>

#include <mach/tcc_fb.h>
#include <mach/gpio.h>
#include <mach/tca_lcdc.h>
#include <mach/tca_tco.h>
#include <mach/TCC_LCD_Interface.h>


static struct mutex panel_lock;
extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);



static int tm070rdh11_panel_init(struct lcd_panel *panel)
{
	printk("%s : %d\n", __func__, 0);
	return 0;
}

static int tm070rdh11_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;

	printk("%s : %d %d  \n", __func__, on, panel->bl_level);

	mutex_lock(&panel_lock);
	panel->state = on;

	if (on) {

		
		gpio_set_value(pdata->power_on, 1);
		udelay(100);

		gpio_set_value(pdata->reset, 1);
		mdelay(20);

		lcdc_initialize(panel, lcd_num);
		LCDC_IO_Set(1, panel->bus_width);

		if(panel->bl_level)		{
			msleep(80); 	
			tcc_gpio_config(pdata->bl_on, GPIO_FN(2));
		}
		else		{
			msleep(80);
		}
		
	}
	else 
	{
		msleep(10);
		gpio_set_value(pdata->reset, 0);

		gpio_set_value(pdata->power_on, 0);

		LCDC_IO_Disable(1, panel->bus_width);
	}
	mutex_unlock(&panel_lock);

	return 0;
}

static int tm070rdh11_set_backlight_level(struct lcd_panel *panel, int level)
{

	#define MAX_BL_LEVEL	255	
	volatile PTIMER pTIMER;

	struct lcd_platform_data *pdata = panel->dev->platform_data;

//	printk("%s : %d\n", __func__, level);
	
	mutex_lock(&panel_lock);
	panel->bl_level = level;


	#define MAX_BACKLIGTH 255
 	if (level == 0) {
		tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);
	}
	else 
	{
		if(panel->state)
		{
			#if defined(CONFIG_ARCH_TCC892X)
			if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 ||system_rev == 0x1008 || system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
				tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);
			else
				tca_tco_pwm_ctrl(1, pdata->bl_on, MAX_BACKLIGTH, level);
			#else
				tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);		
			#endif//
		}
	}

	mutex_unlock(&panel_lock);

	return 0;
}

static struct lcd_panel tm070rdh11_panel = {
	.name		= "TD070RDH11",
	.manufacturer	= "OPTOELECTRONICS",
	.id		= PANEL_ID_TD070RDH11,
	.xres		= 800,
	.yres		= 480,
	.width		= 154,
	.height		= 85,
	.bpp		= 24,
	.clk_freq	= 300000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 47,
	.lpc		= 800,
	.lswc		= 39,
	.lewc		= 39,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 2,
	.flc1		= 480,
	.fswc1		= 28,
	.fewc1		= 12,
	.fpw2		= 2,
	.flc2		= 480,
	.fswc2		= 28,
	.fewc2		= 12,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= tm070rdh11_panel_init,
	.set_power	= tm070rdh11_set_power,
	.set_backlight_level = tm070rdh11_set_backlight_level,
};

static int tm070rdh11_probe(struct platform_device *pdev)
{
	printk("%s : %d\n", __func__, 0);

	mutex_init(&panel_lock);
	
	tm070rdh11_panel.state =1;
	tm070rdh11_panel.dev = &pdev->dev;
	
	tccfb_register_panel(&tm070rdh11_panel);
	return 0;
}

static int tm070rdh11_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver tm070rdh11_lcd = {
	.probe	= tm070rdh11_probe,
	.remove	= tm070rdh11_remove,
	.driver	= {
		.name	= "tm070rdh11_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int tm070rdh11_init(void)
{
	printk("~ %s ~ \n", __func__);
	return platform_driver_register(&tm070rdh11_lcd);
}

static __exit void tm070rdh11_exit(void)
{
	platform_driver_unregister(&tm070rdh11_lcd);
}

subsys_initcall(tm070rdh11_init);
module_exit(tm070rdh11_exit);

MODULE_DESCRIPTION("TM070RDH11 LCD driver");
MODULE_LICENSE("GPL");
