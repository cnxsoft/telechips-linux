/*
 * linux/arch/arm/mach-tcc92x/board-tcc8800st-gpio.c
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
#include "board-tcc8800st.h"
#include <asm/mach-types.h>

static struct board_gpio_irq_config tcc8800st_gpio_irqs[] = {
	{ -1, -1 },
};

void __init tcc8800st_init_gpio(void)
{
	if (!machine_is_tcc8800st())
		return;

	board_gpio_irqs = tcc8800st_gpio_irqs;
	printk(KERN_INFO "TCC8800ST GPIO initialized\n");
	return;
}
