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

//---------------------------
// CPSR Mode Bit Definition
//---------------------------
#define Mode_USR		0x10
#define Mode_FIQ		0x11
#define Mode_IRQ		0x12
#define Mode_SVC		0x13
#define Mode_ABT		0x17
#define Mode_UND		0x1B
#define Mode_SYS		0x1F
#define Mode_MASK		0x1F
#define NOINT			0xC0
#define I_Bit			0x80
#define F_Bit			0x40
	.text

//-------------------------------------------------------------------------------
//
//	FUNCTION :
//
//	DESCRIPTION :
//
//-------------------------------------------------------------------------------
ENTRY(IO_ARM_SetCPSR)
	//EXPORT	IO_ARM_SetCPSR
	//EXPORT	IO_ARM_SetINT
//IO_ARM_SetCPSR
//IO_ARM_SetINT
	msr	cpsr_cf, r0
	bx	lr				@mov	pc, lr

//-------------------------------------------------------------------------------
//
//	FUNCTION :
//
//	DESCRIPTION :
//
//-------------------------------------------------------------------------------
ENTRY(IO_ARM_GetCPSR)
	//EXPORT	IO_ARM_GetCPSR
	//EXPORT	IO_ARM_GetINT
//IO_ARM_GetCPSR
//IO_ARM_GetINT
	mrs	r0, cpsr
	bx	lr				@mov	pc, lr


//-------------------------------------------------------------------------------
//
//	FUNCTION :
//
//	DESCRIPTION :
//
//-------------------------------------------------------------------------------
ENTRY(IO_ARM_ChangeStackSRAM)

	mov		r0, r13
//	ldr		r13, =0xF0803FA0  			@	0x10000000 ~ 0x10003FFF SRAM(16KB)    
//	ldr		r13, =0xF0A03FA0  			@	0x10000000 ~ 0x10003FFF SRAM(16KB)    
	ldr		r13, =0xF0A00800 //;SHUTDOWN_STACK_ADDR+SHUTDOWN_STACK_SIZE
	mov		pc, lr


//-------------------------------------------------------------------------------
//
//	FUNCTION :
//
//	DESCRIPTION :
//
//-------------------------------------------------------------------------------  
ENTRY(IO_ARM_RestoreStackSRAM)

    mov		r13, r0    
    mov		pc, lr


//-------------------------------------------------------------------------------  
//	r0 : shut-down func phy address
//  r1 : mmu & cpu_reg backup area vir address
//  r2 : restore func vir address
//------------------------------------------------------------------------------- 
ENTRY(save_cpu_reg)

	//; shut-down func address
	mov	r12, r0

	//; save svc_reg (r4-r12,lr)
	stmdb   sp!, {r4-r12}
	stmdb   sp!, {lr}

	//; mark shut down mode
	ldr     r3, =0x18C818C8
	str     r3, [r1, #0x00]

	//; save restore func address
	str     r2, [r1, #0x04]
	mov		r3, r1

	//; save SCTLR
	mrc		p15, 0, r2, c1, c0, 0
	ldr		r0, =0x853AC780 // Should Be Zero Mask for System Control Register
	bic		r2, r2, r0
	ldr		r0, =0x00C50078 // Should Be One Mask for System Control Register
	orr		r2, r2, r0
	str		r2, [r3, #0x08]

	//; save TTBR0
	mrc     p15, 0, r2, c2, c0, 0
	str		r2, [r3, #0x0C]

	//; save TTBR1
	mrc		p15, 0, r2, c2, c0, 1
	str		r2, [r3, #0x10]

	//; save TTBCR
	mrc     p15, 0, r2, c2, c0, 2
	str		r2, [r3, #0x14]

	//; save DACR
	mrc		p15, 0, r2, c3, c0, 0
	str		r2, [r3, #0x18]

	//; save DFSR
	mrc p15, 0, r2, c5, c0, 0
	str r2, [r3, #0x1C]

	//; save DFAR
	mrc p15, 0, r2, c6, c0, 0
	str r2, [r3, #0x20]

	//; save IFSR
	mrc p15, 0, r2, c5, c0, 1
	str r2, [r3, #0x24]

	//; save IFAR
	mrc p15, 0, r2, c6, c0, 2
	str r2, [r3, #0x28]

	//; save DAFSR
	mrc p15, 0, r2, c5, c1, 0
	str r2, [r3, #0x2C]

	//; save IAFSR
	mrc p15, 0, r2, c5, c1, 1
	str r2, [r3, #0x30]

	//; save PMRRR
	mrc p15, 0, r2, c10, c2, 0
	str r2, [r3, #0x34]

	//; save NMRRR
	mrc p15, 0, r2, c10, c2, 1
	str r2, [r3, #0x38]

	//; save svc_reg (spsr)
	str		sp, [r3, #0x3C]
	mrs		r2, spsr
	str		r2, [r3, #0x40]

	add 	r3, r1, #0x00000044

	//; save fiq_reg
	mov		r1, #Mode_FIQ | NOINT		// Enter FIQ mode, no interrupts
	msr		cpsr, r1
	mrs		r2, spsr					// Status Register
	stmia	r3!, {r2, r8-r12, sp, lr}		// Store FIQ mode registers [SleepState_FIQ_SPSR~SleepState_FIQ_LR]

	//; save abort_reg
	mov		r1, #Mode_ABT | NOINT		// Enter ABT mode, no interrupts
	msr		cpsr, r1
	mrs		r0, spsr					// Status Register
	stmia	r3!, {r0, sp, lr}			// Store ABT mode Registers [SleepState_ABT_SPSR~SleepState_ABT_LR]

	//; save irq_reg
	mov		r1, #Mode_IRQ | NOINT		// Enter IRQ mode, no interrupts
	msr		cpsr, r1
	mrs		r0, spsr					// Status Register
	stmia	r3!, {r0, sp, lr}			// Store the IRQ Mode Registers [SleepState_IRQ_SPSR~SleepState_IRQ_LR]
	
	//; save undef_reg
	mov		r1, #Mode_UND | NOINT		// Enter UND mode, no interrupts
	msr		cpsr, r1
	mrs		r0, spsr					// Status Register
	stmia	r3!, {r0, sp, lr}			// Store the UND mode Registers [SleepState_UND_SPSR~SleepState_UND_LR]

	//; save usr(sys)_reg
	mov		r1, #Mode_SYS | NOINT		// Enter SYS mode, no interrupts
	msr		cpsr, r1
	stmia	r3!, {sp, lr}				// Store the SYS mode Registers [SleepState_SYS_SP, SleepState_SYS_LR]

	//; Return to SVC mode
	mov		r1, #Mode_SVC | NOINT		// Back to SVC mode, no interrupts
	msr		cpsr, r1
	
	ldr		r2, [sp]
	str		r2, [r3], #4	

	//; invalidate entire tlb & btb & prefetch buffer.
	mov		r0, #0x0
	dsb     sy
	mcr     p15,0x0,r0,c8,c7,0x0 //; p15,0,r3,c8,c7,0 (itlb+dtlb flush all)
	mcr     p15,0x0,r0,c7,c5,0x6 //; p15,0,r3,c7,c5,6 (invalidate btb)
	mcr     p15, 0, r0, c7, c5, 4 //; flush prefetch buffer
	dsb     sy
	isb     sy

	//; clean & invalidate cache(L1, L2)
	bl      v7_flush_kern_cache_all

	//; Data synchronization barrier operation
	mov r0, #0
	mcr	p15, 0, r0, c7, c10, 4

	//; mmu off
	ldr	sp, =0x10000800 //;SHUTDOWN_STACK_PHY+SHUTDOWN_STACK_SIZE
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #(1<<12) //ICache
	bic r0, r0, #(1<<2)  //DCache
	bic r0, r0, #(1<<0)  //MMU

	//; ret_address
	mov r1, r12
	ldr pc, =0xF0700100 //; MMU_SWITCH_EXEC_ADDR

//---------------------------------------------------------------------
ENTRY(SRAM_Boot)

	ldr	pc,	=0x10000004

	//; Mask IRQ/FIQ
	mrs	r0, cpsr
	orr	r0, r0, #0xC0
	msr	cpsr_cf, r0

	//; Set sp(stack point)
	ldr r13, =0x10000800 //;SHUTDOWN_STACK_PHY+SHUTDOWN_STACK_SIZE

	//; CONFIG1.BBEN : Backup RAM Boot Enable //;*((volatile unsigned long *)0xB0503014) |= Hw12;
	ldr r0, =0xF0404000
	bic r1, r1, #0x1000
	str r1, [r0, #0x14]

	//; I/O restore suspend_wakeup
	mov lr, pc
	ldr pc, =0x10000800 //IO_RESTORE_FUNC_ADDR, io_restore()

	//; SDRAM Init - Jump InitRoutine_Start for SDRAM Initialize
	mov lr, pc
	ldr pc, =0x10000A00 //SDRAM_INIT_FUNC_PHY, sdram_init()

	//; Check whether Shutdown is enabled or not.
	//; cpu_reg/mmu data backup address
	ldr r12, =0x10001850 //; REG_MMU_DATA_PHY
	ldr r1, [r12]
	ldr r2, =0x18C818C8
	cmp r1, r2
	ldr r1, =0x0
	str r1, [r12]

resume_err_loop:
	bne resume_err_loop

	//; DCache Invalidate..
	dmb					@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	ands	r3, r0, #0x7000000		@ extract loc from clidr
	mov	r3, r3, lsr #23			@ left align loc bit field
	beq	finished			@ if loc is 0, then no need to clean
	mov	r10, #0				@ start clean at cache level 0
loop1:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	skip				@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
loop2:
	mov	r9, r4				@ create working copy of max way size
loop3:
	orr	r11, r10, r9, lsl r5	@ factor way and cache number into r11
	orr	r11, r11, r7, lsl r2	@ factor index number into r11
	mcr	p15, 0, r11, c7, c6, 2		@ invalidate by set/way
	subs	r9, r9, #1			@ decrement the way
	bge	loop3
	subs	r7, r7, #1			@ decrement the index
	bge	loop2
skip:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	loop1
finished:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb
	isb

	//; ICache Invalidate..
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0		@ I+BTB cache invalidate

	//; disable mmu & cache
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x0005		// Disable MMU and Data Cache
	mcr p15, 0, r0, c1, c0, 0

	//; invalidate tlb
	mov r0, #0
	mcr p15, 0, r0, c8, c7, 0

	//; restore DACR
	ldr r0, [r12, #0x18]
	mcr p15, 0, r0, c3, c0, 0

	//; restore TTBCR
	ldr r0, [r12, #0x14]
	mcr p15, 0, r0, c2, c0, 2

	//; restore TTBR1
	ldr r0, [r12, #0x10]
	mcr p15, 0, r0, c2, c0, 1

	//; restore TTBR0
	ldr r0, [r12, #0x0C]
	mcr p15, 0, r0, c2, c0, 0

	//; DFSR
	ldr r0, [r12, #0x1C]
	mcr p15, 0, r0, c5, c0, 0

	//; DFAR
	ldr r0, [r12, #0x20]
	mcr p15, 0, r0, c6, c0, 0

	//; IFSR
	ldr r0, [r12, #0x24]
	mcr p15, 0, r0, c5, c0, 1

	//; IFAR
	ldr r0, [r12, #0x28]
	mcr p15, 0, r0, c6, c0, 2

	//; DAFSR
	ldr r0, [r12, #0x2C]
	mcr p15, 0, r0, c5, c1, 0

	//; IAFSR
	ldr r0, [r12, #0x30]
	mcr p15, 0, r0, c5, c1, 1

	//; PMRRR
	ldr r0, [r12, #0x34]
	mcr p15, 0, r0, c10, c2, 0

	//; NMRRR
	ldr r0, [r12, #0x38]
	mcr p15, 0, r0, c10, c2, 1

	//; restore SCTLR
	ldr r0, [r12, #0x08]	// System Control Register

	//; Return Address
	ldr r1, [r12, #0x04]

	//; mmu on routine, jump VA=PA area
	ldr r8,=0x10001800 //; MMU_SWITCH_FUNC_PHY
	ldr r7,=0xF0700100 //; MMU_SWITCH_EXEC_ADDR
	ldr r9,	[r8, #0x0]
	str r9,	[r7, #0x0]
	ldr r9,	[r8, #0x4]
	str r9,	[r7, #0x4]
	ldr r9,	[r8, #0x8]
	str r9,	[r7, #0x8]
	ldr r9,	[r8, #0xc]
	str r9,	[r7, #0xc]
	mov pc, r7

//---------------------------------------------------------------------
// r0 : sys_ctrl_reg
// r1 : ret_address)
//---------------------------------------------------------------------
ENTRY(tcc_mmu_switch)
	mcr p15, 0, r0, c1, c0, 0
	nop
	mov pc, r1
	nop

//---------------------------------------------------------------------
ENTRY(resore_cpu_reg)

	//; cpu_reg/mmu data backup address
	ldr r3, =0xF0A01850 //; REG_MMU_DATA_ADDR

	//; restore fiq_reg
	mov r1, #Mode_FIQ | NOINT
	msr cpsr, r1
	ldr r0,	[r3, #0x44]
	msr spsr, r0
	ldr r8,	[r3, #0x48]
	ldr r9,	[r3, #0x4C]
	ldr r10,[r3, #0x50]
	ldr r11,[r3, #0x54]
	ldr r12,[r3, #0x58]
	ldr sp,	[r3, #0x5C]
	ldr lr,	[r3, #0x60]

	//; restore abort_reg
	mov r1, #Mode_ABT | I_Bit
	msr cpsr, r1
	ldr r0,	[r3, #0x64]
	msr spsr, r0
	ldr sp,	[r3, #0x68]
	ldr lr,	[r3, #0x6C]

	//; restore irq_reg
	mov r1, #Mode_IRQ | I_Bit
	msr cpsr, r1
	ldr r0,	[r3, #0x70]
	msr spsr, r0
	ldr sp,	[r3, #0x74]
	ldr lr,	[r3, #0x78]

	//; restore undef_reg
	mov r1, #Mode_UND | I_Bit
	msr cpsr, r1
	ldr r0,	[r3, #0x7C]
	msr spsr, r0
	ldr sp,	[r3, #0x80]
	ldr lr,	[r3, #0x84]

	//; restore usr(sys)_reg
	mov r1, #Mode_SYS | I_Bit
	msr cpsr, r1
	ldr sp,	[r3, #0x88]
	ldr lr,	[r3, #0x8C]

	//; restore svc_reg
	mov r1, #Mode_SVC | I_Bit
	msr cpsr, r1
	ldr r0, [r3, #0x40]
	msr spsr, r0
	ldr sp, [r3, #0x3C]
	ldr lr, [sp], #4
	ldmia sp!, {r4-r12}

	mov pc, lr  

