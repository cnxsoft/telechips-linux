/* linux/arch/arm/mach-tcc88xx/board-m801_88-panel.c
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
#include "board-m801_88.h"

#define DEFAULT_BACKLIGHT_BRIGHTNESS	102

static struct lcd_platform_data lcd_pdata = {
	.power_on	= GPIO_LCD_ON,
	.display_on	= GPIO_LCD_DISPLAY,
	.bl_on		= GPIO_LCD_BL,
	.reset		= GPIO_LCD_RESET,
};


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

static void m801_88_brightness_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	struct lcd_panel *lcd_panel = tccfb_get_panel();

	if (lcd_panel->set_backlight_level)
		lcd_panel->set_backlight_level(lcd_panel, value);
}

static struct led_classdev m801_88_backlight_led = {
	.name		= "lcd-backlight",
	.brightness	= DEFAULT_BACKLIGHT_BRIGHTNESS,
	.brightness_set	= m801_88_brightness_set,
};

#define M801_88_GPIO_LCD_RESET		TCC_GPC(29)
#define M801_88_GPIO_LCD_ON			TCC_GPG(2)
#define M801_88_GPIO_LCD_BL			TCC_GPA(4)
static int m801_88_backlight_probe(struct platform_device *pdev)
{
	tcc_gpio_config(M801_88_GPIO_LCD_ON, GPIO_FN(0)); 	//gpio_d 21
	tcc_gpio_config(M801_88_GPIO_LCD_BL, GPIO_FN(0));	//gpio a 4
	tcc_gpio_config(M801_88_GPIO_LCD_RESET, GPIO_FN(0));	//gpio c 28

	gpio_request(M801_88_GPIO_LCD_ON,		"lcd_on");
	gpio_request(M801_88_GPIO_LCD_BL,	 	"lcd_bl");
	gpio_request(M801_88_GPIO_LCD_RESET, 	"lcd_reset");

	
	return led_classdev_register(&pdev->dev, &m801_88_backlight_led);
}

static int m801_88_backlight_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&m801_88_backlight_led);
	return 0;
}

static struct platform_driver m801_88_backlight_driver = {
	.probe	= m801_88_backlight_probe,
	.remove	= m801_88_backlight_remove,
	.driver	= {
		.name	= "m801_88-backlight",
		.owner	= THIS_MODULE,
	},
};

static struct platform_device m801_88_backlight = {
	.name = "m801_88-backlight",
};

int __init m801_88_init_panel(void)
{
	int ret;
	if (!machine_is_m801_88())
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
	
		default:
			pr_err("Not supported LCD panel type %d\n", tcc_panel_id);
			return -EINVAL;
	}

	ret = platform_device_register(&tcc_lcd_device);
	if (ret)
		return ret;

	platform_device_register(&m801_88_backlight);
	ret = platform_driver_register(&m801_88_backlight_driver);
	if (ret)
		return ret;

	return 0;
}
device_initcall(m801_88_init_panel);
