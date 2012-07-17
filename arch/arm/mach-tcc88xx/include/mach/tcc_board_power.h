/* linux/arch/arm/mach-tcc88xx/include/mach/tcc_board_power.h
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

#ifndef __TCC_POWER_H
#define __TCC_POWER_H

#include <linux/types.h>
#include <linux/device.h>

typedef enum{
	TCC_V5P_POWER,
	TCC_POWER_MAX
}tcc_power_s;

#define TCC_POWER_INIT 	0
#define TCC_POWER_ON	1
#define TCC_POWER_OFF 	2

extern int tcc_power_control(int power, char control);

#endif
