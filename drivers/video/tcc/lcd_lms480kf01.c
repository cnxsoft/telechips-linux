/*
 * lms480kf01_lcd.c -- support for Samsung LMS480KF01 LCD
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
#include <asm/mach-types.h>

#include <mach/tcc_fb.h>
#include <mach/gpio.h>
#include <mach/tca_lcdc.h>
#include <mach/TCC_LCD_Interface.h>
#include <linux/delay.h>
static struct mutex panel_lock;

extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);

static int lms480kf01_panel_init(struct lcd_panel *panel)
{
	return 0;
}

static int lms480kf01_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;
	mutex_lock(&panel_lock);
	if (on) {
		gpio_set_value(pdata->power_on, 1);
		gpio_set_value(pdata->reset, 1);
		msleep(2);
		LCDC_IO_Set(1, panel->bus_width);
		lcdc_initialize(panel, lcd_num);
		msleep(16);
		gpio_set_value(pdata->display_on, 1);
		msleep(16);

	}
	else 
	{
		gpio_set_value(pdata->display_on, 0);
		gpio_set_value(pdata->reset, 0);
		LCDC_IO_Disable(1, panel->bus_width);
		gpio_set_value(pdata->power_on, 0);
	}
	panel->state = on;

	mutex_unlock(&panel_lock);

	if(on)
		panel->set_backlight_level(panel , panel->bl_level);

	return 0;
}

static int lms480kf01_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;

	panel->bl_level = level;
	if((panel->state == 0) && (level != 0))
		return 0;
	mutex_lock(&panel_lock);
	if (level == 0) {
		gpio_set_value(pdata->bl_on, 0);
		gpio_set_value(pdata->display_on, 0);
	} else {
		gpio_set_value(pdata->display_on, 1);
		gpio_set_value(pdata->bl_on, 1);
	}
	mutex_unlock(&panel_lock);

	return 0;
}

static struct lcd_panel lms480kf01_panel = {
	.name		= "LMS480KF01",
	.manufacturer	= "Samsung",
	.id		= PANEL_ID_LMS480KF01,
	.xres		= 800,
	.yres		= 480,
	.width		= 103,
	.height		= 62,
	.bpp		= 24,
	.clk_freq	= 264000,	
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 2,
	.lpc		= 800,
	.lswc		= 12,
	.lewc		= 7,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 0,
	.flc1		= 480,
	.fswc1		= 6,
	.fewc1		= 36,
	.fpw2		= 0,
	.flc2		= 480,
	.fswc2		= 6,
	.fewc2		= 36,	

	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= lms480kf01_panel_init,
	.set_power	= lms480kf01_set_power,
	.set_backlight_level = lms480kf01_set_backlight_level,
};

static int lms480kf01_probe(struct platform_device *pdev)
{
	mutex_init(&panel_lock);
	lms480kf01_panel.state = 1;
	lms480kf01_panel.dev = &pdev->dev;
	tccfb_register_panel(&lms480kf01_panel);
	return 0;
}

static int lms480kf01_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver lms480kf01_lcd = {
	.probe	= lms480kf01_probe,
	.remove	= lms480kf01_remove,
	.driver	= {
		.name	= "lms480kf01_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int lms480kf01_init(void)
{
	return platform_driver_register(&lms480kf01_lcd);
}

static __exit void lms480kf01_exit(void)
{
	platform_driver_unregister(&lms480kf01_lcd);
}

subsys_initcall(lms480kf01_init);
module_exit(lms480kf01_exit);

MODULE_DESCRIPTION("LMS480KF01 LCD driver");
MODULE_LICENSE("GPL");
