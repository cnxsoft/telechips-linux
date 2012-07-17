/*
 * linux/arch/arm/mach-tcc92x/board-tcc8900-gpio.c
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
#include <linux/module.h>
#include <mach/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c/pca953x.h>
#include "board-tcc8800.h"
#include <asm/mach-types.h>

static struct board_gpio_irq_config tcc8800_gpio_irqs[] = {
	{ -1, -1 },
};

static struct pca953x_platform_data pca9539_data1 = {
	.gpio_base	= GPIO_PORTEXT1,
};

static struct pca953x_platform_data pca9539_data2 = {
	.gpio_base	= GPIO_PORTEXT2,
};

static struct pca953x_platform_data pca9539_data3 = {
	.gpio_base	= GPIO_PORTEXT3,
};

static struct pca953x_platform_data pca9539_data4 = {
	.gpio_base	= GPIO_PORTEXT4,
};

static struct pca953x_platform_data pca9539_data5 = {
	.gpio_base	= GPIO_PORTEXT5,
};


/* I2C core0 channel0 devices */
static struct i2c_board_info __initdata i2c_devices_expender[] = {
	{
		I2C_BOARD_INFO("pca9539", 0x74),
		.platform_data = &pca9539_data1,
	},
	{
		I2C_BOARD_INFO("pca9539", 0x77),
		.platform_data = &pca9539_data2,
	},
	{
		I2C_BOARD_INFO("pca9539", 0x75),
		.platform_data = &pca9539_data3,
	},
	{
		I2C_BOARD_INFO("pca9538", 0x70),
		.platform_data = &pca9539_data4,
	},
	{	// SV60 Power Board
		I2C_BOARD_INFO("pca9539", 0x76),
		.platform_data = &pca9539_data5,
	},
};

void __init tcc8800_init_gpio(void)
{
	if (!machine_is_tcc8800())
		return;

	i2c_register_board_info(0, i2c_devices_expender, ARRAY_SIZE(i2c_devices_expender));

	board_gpio_irqs = tcc8800_gpio_irqs;
	printk(KERN_INFO "TCC8800 GPIO initialized\n");
	return;
}
