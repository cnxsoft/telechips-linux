/* linux/arch/arm/mach-tcc88xx/board-mobis-panel.c
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
#include "board-mobis.h"

#define DEFAULT_BACKLIGHT_BRIGHTNESS	102

static struct lcd_platform_data lcd_pdata = {
	.power_on	= GPIO_LCD_ON,
	.display_on	= GPIO_LCD_DISPLAY,
	.bl_on		= GPIO_LCD_BL,
	.reset		= GPIO_LCD_RESET,
};


#ifdef CONFIG_LCD_LMS480KF01
static struct platform_device lms480kf01_lcd = {
	.name	= "lms480kf01_lcd",
	.dev	= {
		.platform_data	= &lcd_pdata,
	},
};
#endif

#ifdef CONFIG_LCD_AT070TN93
static struct platform_device at070tn93_lcd = {
	.name	= "at070tn93_lcd",
	.dev	= {
		.platform_data	= &lcd_pdata,
	},
};
#endif

#ifdef CONFIG_LCD_TW8816
static struct platform_device tw8816_lcd = {
	.name	= "tw8816_lcd",
	.dev	= {
		.platform_data	= &lcd_pdata,
	},
};
#endif

static void mobis_brightness_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	struct lcd_panel *lcd_panel = tccfb_get_panel();

	if (lcd_panel->set_backlight_level)
		lcd_panel->set_backlight_level(lcd_panel, value);
}

static struct led_classdev mobis_backlight_led = {
	.name		= "lcd-backlight",
	.brightness	= DEFAULT_BACKLIGHT_BRIGHTNESS,
	.brightness_set	= mobis_brightness_set,
};

static int mobis_backlight_probe(struct platform_device *pdev)
{

	gpio_request(GPIO_LCD_ON, "lcd_on");
	gpio_request(GPIO_LCD_BL, "lcd_bl");
	gpio_request(GPIO_LCD_DISPLAY, "lcd_display");
	gpio_request(GPIO_LCD_RESET, "lcd_reset");

	gpio_direction_output(GPIO_LCD_ON, 1);
	gpio_direction_output(GPIO_LCD_BL, 1);
	gpio_direction_output(GPIO_LCD_DISPLAY, 1);
	gpio_direction_output(GPIO_LCD_RESET, 1);

	return led_classdev_register(&pdev->dev, &mobis_backlight_led);
}

static int mobis_backlight_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&mobis_backlight_led);
	return 0;
}

static struct platform_driver mobis_backlight_driver = {
	.probe	= mobis_backlight_probe,
	.remove	= mobis_backlight_remove,
	.driver	= {
		.name	= "mobis-backlight",
		.owner	= THIS_MODULE,
	},
};

static struct platform_device mobis_backlight = {
	.name = "mobis-backlight",
};

int __init mobis_init_panel(void)
{
	int ret;
	if (!machine_is_mobis())
		return 0;

	printk("%s supported LCD panel type %d\n",__func__,  tcc_panel_id);

	switch (tcc_panel_id) {


#ifdef CONFIG_LCD_LMS480KF01
	case PANEL_ID_LMS480KF01:
		platform_device_register(&lms480kf01_lcd);
		break;
#endif

#ifdef CONFIG_LCD_AT070TN93
	case PANEL_ID_AT070TN93:
		platform_device_register(&at070tn93_lcd);
		break;
#endif
#ifdef CONFIG_LCD_TW8816 
	case PANEL_ID_TW8816:
		platform_device_register(&tw8816_lcd);
		break;
#endif//


	default:
		pr_err("Not supported LCD panel type %d\n", tcc_panel_id);
		return -EINVAL;
	}

	ret = platform_device_register(&tcc_lcd_device);
	if (ret)
		return ret;

	platform_device_register(&mobis_backlight);
	ret = platform_driver_register(&mobis_backlight_driver);
	if (ret)
		return ret;

	return 0;
}


device_initcall(mobis_init_panel);
