/* linux/arch/arm/plat-tcc/include/mach/tcc_ts.h
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

#ifndef __TCC_TS_H
#define __TCC_TS_H

struct tcc_adc_ts_platform_data {
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int swap_xy_pos;
	int reverse_x_pos;
	int reverse_y_pos;
};

#endif
