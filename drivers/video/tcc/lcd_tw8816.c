/*
 * tw8816_lcd.c -- support for Hitachi DX08D11VM0AAA LCD
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
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <asm/mach-types.h>

#include <mach/tcc_fb.h>
#include <mach/gpio.h>
#include <mach/tca_lcdc.h>
#include <mach/TCC_LCD_DriverCtrl.h>
#include <mach/TCC_LCD_Interface.h>
#include <linux/delay.h>
#include <mach/dev_gpsb.h>

#include "tca_tw8816.h"

extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);

struct tcc_i2c_chip_info {
	unsigned gpio_start;
	uint16_t reg_output;
	uint16_t reg_direction;

	struct i2c_client *client;
	struct gpio_chip gpio_chip;
};


static const struct i2c_device_id tw8816_id[] = {
	{ "tw8816_pane_i2c", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tw8816_id);


void tw8816_i2c_write(struct i2c_client *client, unsigned reg, unsigned value)
{
	i2c_smbus_write_byte_data(client, reg, value);
}


static int tw8816_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct tcc_i2c_chip_info *info;

	info = kzalloc(sizeof(struct tcc_i2c_chip_info), GFP_KERNEL);
	if (info == NULL) {
		printk("tw8816_bl: no memory for i2c\n");
		return -ENOMEM;
	}

	info->client = client;
	i2c_set_clientdata(client, info);

	return 0;
}

static int tw8816_i2c_remove(struct i2c_client *client)
{
	struct bl_info *info = i2c_get_clientdata(client);

	kfree(info);

	return 0;
}

static int tw8816_panel_init(struct lcd_panel *panel)
{
	return 0;
}

static int tw8816_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
#if 0

	unsigned int reg, value;
	struct lcd_platform_data *pdata = panel->dev->platform_data;
	struct platform_device *pdev = container_of(panel->dev, struct platform_device, dev);
	struct i2c_client *i2c = dev_get_drvdata(&pdev->dev);


	printk("tw8816_set_power: %d\n", on);
		
	if (on) {
//		gpio_set_value(pdata->power_on, 1);
//		gpio_set_value(pdata->reset, 1);
//		udelay(10);

	//set register and value
	//	tw8816_i2c_write(i2c, reg, value);

			
		msleep(16);

		lcdc_initialize(panel, lcd_num);
		LCDC_IO_Set(1, panel->bus_width);
		msleep(16);

		gpio_set_value(pdata->display_on, 1);

	} else {

		gpio_set_value(pdata->display_on, 0);
		udelay(10);

		LCDC_IO_Disable(0, panel->bus_width);
	//set register and value
	//	tw8816_i2c_write(i2c, reg, value);


//		gpio_set_value(pdata->reset, 0);
		gpio_set_value(pdata->power_on, 0);

	}
#endif//

	return 0;
}

static char BD6083_state = 1;
static int tw8816_set_backlight_level(struct lcd_panel *panel, int level)
{

	return 0;
}

static struct lcd_panel tw8816_panel = {
	.name		= "TW8816",
	.manufacturer	= "Techwell",
	.id			= 	PANEL_ID_TW8816,
	
	.xres		= 800,
	.yres		= 480,
	.width		= 76,
	.height		= 48,
	.bpp			= 32,
	.clk_freq		= 292320,
	.clk_div		= 2,
	.bus_width	= 24,
	.lpw			= 47,
	.lpc			= 800,
	.lswc		= 87,
	.lewc		= 39,
	.vdb			= 0,
	.vdf			= 0,
	.fpw1		= 3,
	.flc1			= 480,
	.fswc1		= 32,
	.fewc1		= 13,
	.fpw2		= 3,
	.flc2			= 480,
	.fswc2		= 32,
	.fewc2		= 13,
	.sync_invert	= IV_INVERT | IH_INVERT | IP_INVERT,
	.init		= tw8816_panel_init,
	.set_power	= tw8816_set_power,
	.set_backlight_level = tw8816_set_backlight_level,
};

static int tw8816_probe(struct platform_device *pdev)
{
	struct i2c_adapter *adap = i2c_get_adapter(1);
	struct i2c_client *i2c;
	struct i2c_board_info tw8816_panel_i2c_info = {
		I2C_BOARD_INFO("tw8816_panel_i2c", TW8816_I2C_ADDR >> 1),
		.platform_data = &tw8816_panel,
	};

	#ifdef CONFIG_ARCH_TCC88XX
	adap = i2c_get_adapter(1);
	#else	
	adap = i2c_get_adapter(0);
	#endif//
	/* TODO: add channel configuration */
	i2c = i2c_new_device(adap , &tw8816_panel_i2c_info);
	if (!i2c)
		return -EINVAL;


	dev_set_drvdata(&pdev->dev, i2c);

	tw8816_panel.dev = &pdev->dev;

	tccfb_register_panel(&tw8816_panel);
	return 0;
}

static int tw8816_remove(struct platform_device *spi)
{
	return 0;
}

static struct i2c_driver tw8816_panel_i2c = {
	.probe		= tw8816_i2c_probe,
	.remove		= tw8816_i2c_remove,
	.driver = {
		.name	= "tw8816_panel_i2c",
		.owner	= THIS_MODULE,
	},
	.id_table	= tw8816_id,
};

static struct platform_driver tw8816_lcd = {
	.probe		= tw8816_probe,
	.remove		= tw8816_remove,
	.driver = {
		.name	= "tw8816_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int tw8816_init(void)
{
	i2c_add_driver(&tw8816_panel_i2c);

	return platform_driver_register(&tw8816_lcd);
}

static __exit void tw8816_exit(void)
{
	i2c_del_driver(&tw8816_panel_i2c);

	return platform_driver_unregister(&tw8816_lcd);
}

subsys_initcall(tw8816_init);
module_exit(tw8816_exit);

MODULE_DESCRIPTION("TW8816 LCD driver");
MODULE_LICENSE("GPL");
