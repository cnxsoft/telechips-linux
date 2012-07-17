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

//=================================================================
// __cpu_early_init
//=================================================================
	.global __cpu_early_init
__cpu_early_init:
	//Bruce, CortexA-5 does not have internal L2 Cache.
	/* Enable L2 cache */
	//mrc	p15, 0, r0, c1, c0, 1	@ read auxiliary control reg
	//orr	r0, r0, #(1<<1)
	//mcr	p15, 0, r0, c1, c0, 1	@ write auxiliary control reg

	//Bruce, for using VFP of the Cortex-A5,
	// 1. Non-Secure mode should be permitted to access to CP10 and CP11.
	// 2. Enable Advenced SIMD extention instructions that are not VFP instructions ard undefined.
	// 3. Enable D16-D31 of the VFP register file.
	MRC p15, 0, r0, c1, c1, 2
	ORR r0, r0, #1<<10
	ORR r0, r0, #1<<11
	MCR p15, 0, r0, c1, c1, 2

	LDR r0, =(0xF << 20)
	MCR p15, 0, r0, c1, c0, 2

	//MOV r0, #0x40000000
	//VMSR FPEXC, r0

#if defined(CONFIG_DRAM_DDR3)
	//Bruce, to prevent LCD Flickering
	ldr r0, =0xF4800400
	ldr r1, =0x00000007
	str r1, [r0, #0x60]
	ldr r1, =0x00000045 //IO[0], DDI Primary[2], DDI Secondary[6]
	str r1, [r0, #0x64]
#endif

	bx	lr

