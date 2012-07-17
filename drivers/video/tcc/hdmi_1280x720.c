/*
 * hdmi_1280x720.c -- support for Hdmi 1280x720 LCD
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

static struct mutex panel_lock;

extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);

static int hdmi1280x720_panel_init(struct lcd_panel *panel)
{
	return 0;
}

static int hdmi1280x720_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;

#if 0
	printk("%s\n", __func__);
	
	mutex_lock(&panel_lock);
	if (on) {
		gpio_set_value(pdata->power_on, 1);
		gpio_set_value(pdata->reset, 1);
		msleep(10);
		lcdc_initialize(panel);
		LCDC_IO_Set(1, panel->bus_width);
		msleep(16);

		gpio_set_value(pdata->display_on, 1);
	}
	else 
	{
		gpio_set_value(pdata->display_on, 0);
		msleep(10);
		gpio_set_value(pdata->reset, 0);
		gpio_set_value(pdata->power_on, 0);

		LCDC_IO_Disable(0, panel->bus_width);
	}
	mutex_unlock(&panel_lock);
#endif /* 0 */

	return 0;
}

static int hdmi1280x720_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;

#if 0
	mutex_lock(&panel_lock);
	if (level == 0) {
		gpio_set_value(pdata->bl_on, 0);
	} else {
		gpio_set_value(pdata->bl_on, 1);
	}
	mutex_unlock(&panel_lock);
#endif /* 0 */


	return 0;
}

static struct lcd_panel hdmi1280x720_panel = {
	.name		= "HDMI1280x720",
	.manufacturer	= "Telechips",
	.id		= PANEL_ID_HDMI,
	.xres		= 1280,
	.yres		= 720,
	.width		= 103,
	.height		= 62,
	.bpp		= 32,
	.clk_freq	= 245000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 2,
	.lpc		= 1280,
	.lswc		= 12,
	.lewc		= 7,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 0,
	.flc1		= 720,
	.fswc1		= 6,
	.fewc1		= 4,
	.fpw2		= 0,
	.flc2		= 720,
	.fswc2		= 6,
	.fewc2		= 4,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= hdmi1280x720_panel_init,
	.set_power	= hdmi1280x720_set_power,
	.set_backlight_level = hdmi1280x720_set_backlight_level,
};

static int hdmi1280x720_probe(struct platform_device *pdev)
{

	printk("%s\n", __func__);
	
	mutex_init(&panel_lock);

	hdmi1280x720_panel.dev = &pdev->dev;
	tccfb_register_panel(&hdmi1280x720_panel);
	return 0;
}

static int hdmi1280x720_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver hdmi1280x720_lcd = {
	.probe	= hdmi1280x720_probe,
	.remove	= hdmi1280x720_remove,
	.driver	= {
		.name	= "hdmi1280x720_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int hdmi1280x720_init(void)
{
	printk("%s\n", __func__);
	return platform_driver_register(&hdmi1280x720_lcd);
}

static __exit void hdmi1280x720_exit(void)
{
	platform_driver_unregister(&hdmi1280x720_lcd);
}

subsys_initcall(hdmi1280x720_init);
module_exit(hdmi1280x720_exit);

MODULE_DESCRIPTION("HDMI 1280x720 LCD driver");
MODULE_LICENSE("GPL");
