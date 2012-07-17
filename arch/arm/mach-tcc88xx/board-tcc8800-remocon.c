/* linux/arch/arm/mach-tcc93xx/board-tcc9300st-remocon.c
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
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/mmc/host.h>
#include <linux/gpio.h>

#include <mach/mmc.h>

#include <mach/gpio.h>
#include <mach/bsp.h>
#include <asm/mach-types.h>

#include "devices.h"

void tcc_remocon_reset(void)
{
	;
}

void tcc_remocon_set_io(void)
{
	PGPIO pGpioA = (volatile PGPIO)tcc_p2v(HwGPIOA_BASE);

	BITSET(pGpioA->GPAIS, Hw5);
}

