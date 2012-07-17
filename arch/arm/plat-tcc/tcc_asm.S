/*
 * linux/arch/arm/plat-tcc/tcc_asm.S
 *
 * Author:  <linux@telechips.com>
 * Created: April 21, 2008
 * Description: LINUX POWER MANAGEMENT FUNCTIONS
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/linkage.h>
//#include <asm/hardware.h>
#include <mach/hardware.h>
#include <asm/ptrace.h>

	.text

/*
 * IO_ARM_SaveREG
 * --------------
 *
 * Save ARM registers, MMU data and MODE data for suspend mode.
 *
 * input parameters:
 *	r0: SRAM_ADDR_STANDBY
 *	r1: mTCC83X_REG->uMMU
 *	r2: mTCC83X_REG->uCPSR
 */
ENTRY(IO_ARM_SaveREG)
	@@ address of sdram_self_refresh() (r0)
	mov r12, r0

	@@ Save MMU to mTCC83X_REG->uMMU (r1)
	mrc p15, 0, r3, c1, c0, 0
	str r3, [r1, #0x0]				@ MMU control data

	mrc p15, 0, r3, c2, c0, 0
	str r3, [r1, #0x4]				@ TTB

	mrc p15, 0, r3, c3, c0, 0
	str r3, [r1, #0x8]				@ DAC

	mrc p15, 0, r3, c5, c0, 0
	str r3, [r1, #0xc]				@ DFS

	mrc p15, 0, r3, c5, c0, 1
	str r3, [r1, #0x10]				@ IFS

	mrc p15, 0, r3, c6, c0, 0
	str r3, [r1, #0x14]				@ DFA
            
	@@ Save CPSR to mTCC83X_REG->uCPSR (r2)
	mrs	r3, CPSR
	mov r0, r2
	str	r3, [r0], #4				@ save CPSR

	@@ Save ARM registers
	stmia	r0!, {r4-r14}

	@@ Save MODE registers
	mov	r2, r3
	orr	r2, r2, #SYSTEM_MODE
	msr	CPSR_cf, r2
	stmia r0!, {r13-r14}			@ save System mode registers

	bic	r2, r2, #MODE_MASK
	orr	r2, r2, #UND_MODE
	msr	CPSR_cf, r2
	stmia	r0!, {r13-r14}			@ save Undef mode registers

	bic	r2, r2, #MODE_MASK
	orr	r2, r2, #ABT_MODE
	msr	CPSR_cf, r2
	stmia	r0!, {r13-r14}			@ save Abort mode registers

	bic	r2, r2, #MODE_MASK
	orr	r2, r2, #IRQ_MODE
	msr	CPSR_cf, r2
	stmia	r0!, {r13-r14}			@ save IRQ mode registers

	bic	r2, r2, #MODE_MASK
	orr	r2, r2, #FIQ_MODE
	msr	CPSR_cf, r2
	stmia	r0!, {r8-r14}			@ save FIQ mode registers


	@@ back-to SVC mode
	msr	CPSR_cf, r3


	@@ jump to power-off code
	mov	r0, #0xFF
	mov	pc, r12

