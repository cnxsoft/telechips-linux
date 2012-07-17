/*
 * include/asm-arm/arch-tcc88xx/entry-macro.S
 *
 * Author : <linux@telechips.com>
 * Created: August 30, 2010
 * Description: Low-level IRQ helper macros for TCC-based platforms
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <mach/hardware.h>
#include <mach/irqs.h>

    .macro  disable_fiq
    .endm 

    .macro  get_irqnr_preamble, base, tmp
    .endm

    .macro  arch_ret_to_user, tmp1, tmp2
    .endm

    /* tcc892x dependent code */
    .macro  get_irqnr_and_base, irqnr, irqstat, base, tmp 
        ldr \base, =0xF5100000     @ load base address of IRQ registers 

        /* A intr */
        ldr \irqstat, [\base, #0x50]    /* MIRQ0 */
        cmp \irqstat, #0
        bne 1001f /* jump to check A-intr */

        /* B intr */
        ldr \irqstat, [\base, #0x54]    /* MIRQ1 */
        cmp \irqstat, #0
        beq 1002f /* jump to exit macro. cannot find the intr number. (something wrong) */

        /* get B-intr num */
        mov \irqnr, #0                      @@ start here B
1102:	ands \tmp, \irqstat, #1
		moveq \irqstat, \irqstat, LSR #1
		addeq \irqnr, \irqnr, #1
		beq	1102b
		add \irqnr, \irqnr, #32
		b	1002f


        /* get A-intr num */
1001:	mov \irqnr, #0                      @@ start here A
1101:	ands \tmp, \irqstat, #1
		moveq \irqstat, \irqstat, LSR #1
		addeq \irqnr, \irqnr, #1
		beq	1101b
		@@ work out which irq (if any) we got

        @@ ADD Register #2
	

1002: /* exit */
        @@ exit here, Z flag unset if IRQ
    .endm

    .macro irq_prio_table
    .endm
