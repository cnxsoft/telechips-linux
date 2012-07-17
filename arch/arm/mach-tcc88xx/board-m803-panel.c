/* linux/arch/arm/mach-tcc88xx/board-m803-panel.c
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <asm/setup.h>
#include <asm/mach-types.h>

#include <mach/gpio.h>
#include <mach/tcc_fb.h>
#include "devices.h"
#include "board-m803.h"

#define DEFAULT_BACKLIGHT_BRIGHTNESS	102

static struct lcd_platform_data lcd_pdata = {
	.power_on	= GPIO_LCD_ON,
	.display_on	= GPIO_LCD_DISPLAY,
	.bl_on		= GPIO_LCD_BL,
	.reset		= GPIO_LCD_RESET,
};

#if 0
static struct lcd_platform_data lvds_pdata = {
	.power_on	= GPIO_LVDS_EN,
	.display_on	= GPIO_LVDSIVT_ON,
	.bl_on		= GPIO_LVDS_LP_CTRL1,
	.reset		= GPIO_LVDS_LP_CTRL2,
};
#else
static struct lcd_platform_data lvds_pdata = {
	.power_on	= TCC_GPG(2),
	.display_on	= TCC_GPA(4),
	.bl_on		= TCC_GPC(29),
	.reset		= TCC_GPC(24),
};
#endif

#ifdef CONFIG_LCD_TD070RDH 
static struct platform_device tm070rdh113_lcd = {
	.name	= "tm070rdh11_lcd",
	.dev	= {
		.platform_data	= &lcd_pdata,
	},
};
#endif//CONFIG_LCD_TD070RDH 

#ifdef CONFIG_LCD_AT070TN93
static struct platform_device at070tn93_lcd = {
	.name	= "at070tn93_lcd",
	.dev	= {
		.platform_data	= &lcd_pdata,
	},
};
#endif//CONFIG_LCD_AT070TN93

#ifdef CONFIG_LCD_N101L6
static struct platform_device n101l6_lcd = {
	.name	= "n101l6_lcd",
	.dev	= {
		.platform_data	= &lvds_pdata,
	},
};
#endif

#ifdef CONFIG_LCD_CLAA102NA0DCW
static struct platform_device claa102na0dcw_lcd= {
	.name	= "claa102na0dcw_lcd",
	.dev	= {
		.platform_data	= &lvds_pdata,
	},
};
#endif


static void m803_brightness_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	struct lcd_panel *lcd_panel = tccfb_get_panel();

	if (lcd_panel->set_backlight_level)
		lcd_panel->set_backlight_level(lcd_panel, value);
}

static struct led_classdev m803_backlight_led = {
	.name		= "lcd-backlight",
	.brightness	= DEFAULT_BACKLIGHT_BRIGHTNESS,
	.brightness_set	= m803_brightness_set,
};

#define M803_GPIO_LCD_RESET		TCC_GPC(24)
#define M803_GPIO_LCD_ON			TCC_GPG(2)
#define M803_GPIO_LCD_BL			TCC_GPA(4)
#define M803_GPIO_LCD_DISPLAY		TCC_GPC(29)
//efine M803_GPIO_LCD_DISPLAY

static int m803_backlight_probe(struct platform_device *pdev)
{
	printk("%s\n", __func__);
	tcc_gpio_config(M803_GPIO_LCD_ON, GPIO_FN(0)); 	//gpio G 2
	tcc_gpio_config(M803_GPIO_LCD_BL, GPIO_FN(0));	//gpio C 29
	tcc_gpio_config(M803_GPIO_LCD_RESET, GPIO_FN(0));	//gpio c 24
	tcc_gpio_config(M803_GPIO_LCD_DISPLAY, GPIO_FN(0));
	

	gpio_request(M803_GPIO_LCD_ON,		"lcd_on");
	gpio_request(M803_GPIO_LCD_BL,	 	"lcd_bl");
	gpio_request(M803_GPIO_LCD_RESET, 	"lcd_reset");

	
	return led_classdev_register(&pdev->dev, &m803_backlight_led);
}

static int m803_backlight_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&m803_backlight_led);
	return 0;
}

static struct platform_driver m803_backlight_driver = {
	.probe	= m803_backlight_probe,
	.remove	= m803_backlight_remove,
	.driver	= {
		.name	= "m803-backlight",
		.owner	= THIS_MODULE,
	},
};

static struct platform_device m803_backlight = {
	.name = "m803-backlight",
};

int __init m803_init_panel(void)
{
	int ret;
	if (!machine_is_m803())
		return 0;
	printk("%s LCD panel type %d\n", __func__, tcc_panel_id);

	switch (tcc_panel_id) {
		
		#ifdef CONFIG_LCD_TD070RDH 
		case PANEL_ID_TD070RDH11:
			platform_device_register(&tm070rdh113_lcd);
			break;

		#endif//
	
		#ifdef CONFIG_LCD_AT070TN93
		case PANEL_ID_AT070TN93:
			platform_device_register(&at070tn93_lcd);
			break;
		#endif
	
#ifdef CONFIG_LCD_N101L6
	case PANEL_ID_N101L6:
		gpio_set_value(TCC_GPF(13), 1);
		platform_device_register(&n101l6_lcd);
		break;
#endif//

#ifdef CONFIG_LCD_CLAA102NA0DCW         
	case PANEL_ID_CLAA102NA0DCW:
		gpio_set_value(TCC_GPF(13), 1);
		platform_device_register(&claa102na0dcw_lcd);
		break;
#endif//

		default:
			pr_err("Not supported LCD panel type %d\n", tcc_panel_id);
			return -EINVAL;
	}

	ret = platform_device_register(&tcc_lcd_device);
	if (ret)
		return ret;

	platform_device_register(&m803_backlight);
	ret = platform_driver_register(&m803_backlight_driver);
	if (ret)
		return ret;

	return 0;
}
device_initcall(m803_init_panel);
