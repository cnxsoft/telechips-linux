/* linux/arch/arm/mach-tcc88xx/cpu_early_init.S
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

	.text

	.global __cpu_early_init
__cpu_early_init:
	/* Enable L2 cache */
	mrc	p15, 0, r0, c1, c0, 1	@ read auxiliary control reg
	orr	r0, r0, #(1<<1)
	mcr	p15, 0, r0, c1, c0, 1	@ write auxiliary control reg
	bx	lr
