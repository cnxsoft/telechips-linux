/*
 * ek08009_lcd.c -- support for LG PHILIPS LB080WV6 LCD
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
#include <mach/TCC_LCD_DriverCtrl.h>
#include <mach/TCC_LCD_Interface.h>
#if defined(CONFIG_ARCH_TCC892X)
#include <mach/reg_physical.h>
#endif
//#include <linux/51boardpcb.h>
#if 1
#define dprintk(msg...)	printk(msg)
#else
#define dprintk(msg...)	do{}while(0)
#endif
/*The following Function from tcc_backlight.c file */


static struct mutex panel_lock;
extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);
static int kr080pa2s_panel_init(struct lcd_panel *panel)
{

	dprintk("%s : \n", __func__ );
	return 0;
}

static int kr080pa2s_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;
	dprintk("%s : %d %d  \n", __func__, on, panel->bl_level);

	mutex_lock(&panel_lock);

	panel->state = on;

	if (on) {
		gpio_set_value(pdata->power_on, 1);
		udelay(100);

		gpio_set_value(pdata->reset, 1);
		msleep(20);

		lcdc_initialize(panel, lcd_num);
		LCDC_IO_Set(lcd_num, panel->bus_width);
		/*Delay Open The Backlight when Come back from suspend or resume By JimKuang*/
		if(panel->bl_level)		
			{
			msleep(80); 	
		#if defined(CONFIG_ARCH_TCC892X)
			if (system_rev == 0x1005 || system_rev == 0x1007)
				tcc_gpio_config(pdata->bl_on, GPIO_FN(11));
			else if (system_rev == 0x1006)
				tcc_gpio_config(pdata->bl_on, GPIO_FN(7));
			else
				tcc_gpio_config(pdata->bl_on, GPIO_FN(9));
		#else
			tcc_gpio_config(pdata->bl_on, GPIO_FN(2));
		#endif
		}else{
			msleep(80);
		}
		
	}
	else 
	{
		msleep(10);
		gpio_set_value(pdata->reset, 0);
		gpio_set_value(pdata->power_on, 0);

		LCDC_IO_Disable(lcd_num, panel->bus_width);
	}
	mutex_unlock(&panel_lock);
	return 0;
}

static int kr080pa2s_set_backlight_level(struct lcd_panel *panel, int level)
{

	struct lcd_platform_data *pdata = panel->dev->platform_data;	
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

static struct lcd_panel kr080pa2s_panel = {
	.name		= "kr080pa2s",
	.manufacturer	= "xingyuan",
	.id			= PANEL_ID_KR080PA2S,
	.xres		= 1024,
	.yres		= 768,
	.width		= 103,
	.height		= 62,
	.bpp		= 24,
	.clk_freq	= 650000,
	.clk_div	= 1,
	.bus_width	= 24,
	.lpw		= 5,
	.lpc		= 1024,
	.lswc		= 160,
	.lewc		= 160,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 2,
	.flc1		= 768,
	.fswc1		= 20,
	.fewc1		= 15,
	.fpw2		= 2,
	.flc2		= 768,
	.fswc2		= 20,
	.fewc2		= 15,
	.sync_invert	= IV_INVERT| IH_INVERT,
	.init		= kr080pa2s_panel_init,
	.set_power	= kr080pa2s_set_power,
	.set_backlight_level = kr080pa2s_set_backlight_level,
};

static int kr080pa2s_probe(struct platform_device *pdev)
{
	dprintk("###%s###\r\n",__func__);
	mutex_init(&panel_lock);
	kr080pa2s_panel.state = 1;
	kr080pa2s_panel.dev = &pdev->dev;
	tccfb_register_panel(&kr080pa2s_panel);
	return 0;
}

static int kr080pa2s_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver kr080pa2s_lcd = {
	.probe	= kr080pa2s_probe,
	.remove	= kr080pa2s_remove,
	.driver	= {
		.name	= "kr080pa2s_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int kr080pa2s_init(void)
{
	printk("~ %s ~ \n", __func__);
	return platform_driver_register(&kr080pa2s_lcd);
}

static __exit void kr080pa2s_exit(void)
{
	return platform_driver_unregister(&kr080pa2s_lcd);
}

subsys_initcall(kr080pa2s_init);
module_exit(kr080pa2s_exit);

MODULE_DESCRIPTION("kr080pa2s LCD driver");
MODULE_LICENSE("GPL");
