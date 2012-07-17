/*
 * linux/drivers/char/tcc_intr.c 
 *
 * Author:  <linux@telechips.com>
 * Created: 31th March, 2009 
 * Description: Telechips Linux User-level interrupt Driver 
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/poll.h>
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include <mach/bsp.h>
#include <mach/irqs.h>
#include <mach/tca_intr.h>

/*
 *	BroadCasting DXB Interface.
 */
void init_bc_interrupt(tcc_intr_data_t *tcc_intr)
{	
	PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	
	tcc_intr->irq = INT_EI3;
	tcc_intr->irq_bit = (1 << INT_EI3);
	tcc_intr->irq_regs = (volatile PPIC)tcc_p2v(HwPIC_BASE);

	/* DXB0_IRQ - GPIO_B15 - ExINT3 */	
	BITCLR(pGPIO->GPBFN0.nREG, Hw32-Hw28);	// gpio
	BITCLR(pGPIO->GPBEN.nREG, Hw15);		// input mode

	BITCSET(pGPIO->EINTSEL0.nREG, Hw30-Hw24, (158<<24));
	BITSET(tcc_intr->irq_regs->EI37SEL, Hw3);

	/* Int trigger setting */
	BITCLR(tcc_intr->irq_regs->MODE0.nREG, tcc_intr->irq_bit);	// edge-triggered
	BITCLR(tcc_intr->irq_regs->MODEA0.nREG, tcc_intr->irq_bit);	// single-edge
	BITCLR(tcc_intr->irq_regs->POL0.nREG, tcc_intr->irq_bit);	// active-high
}

///////////////////////////////////////////////////////////////////////////
//
long io_interrupt(tcc_intr_data_t *tcc_intr, long flag)
{
	/* NOTE: irq_regs->XXX0, XXX1 */
	long ret = 0;
	if (flag == 0) {
		BITSET(tcc_intr->irq_regs->CLR0.nREG, tcc_intr->irq_bit);			// clear intr
		BITCLR(tcc_intr->irq_regs->INTMSK0.nREG, tcc_intr->irq_bit); 		// disable intr
	} else if (flag == 1) {
		BITSET(tcc_intr->irq_regs->CLR0.nREG, tcc_intr->irq_bit);
		BITSET(tcc_intr->irq_regs->INTMSK0.nREG, tcc_intr->irq_bit); 		// enable intr
	} else if (flag == 2) {
		ret = (tcc_intr->irq_regs->INTMSK0.nREG & tcc_intr->irq_bit)?1:0;	// get int-mask status
	} else {
		ret = -1;
	}
	return ret;
}

EXPORT_SYMBOL(init_bc_interrupt);
EXPORT_SYMBOL(io_interrupt);

