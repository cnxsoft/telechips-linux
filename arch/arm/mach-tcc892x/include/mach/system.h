/*
 * linux/arch/arm/mach-tcc88xx/include/mach/system.h
 *
 * Author: <linux@telechips.com>
 * Created: August 30, 2010
 * Description: LINUX SYSTEM FUNCTIONS
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H
#include <linux/clk.h>

#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <linux/regulator/consumer.h>

#include <mach/bsp.h>

#define nop_delay(x) for(cnt=0 ; cnt<x ; cnt++){ \
					__asm__ __volatile__ ("nop\n"); }

static inline void arch_idle(void)
{
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
	volatile PPMU pPMU = (volatile PPMU)(tcc_p2v(HwPMU_BASE));
	volatile PIOBUSCFG pIOBUSCFG = (volatile PIOBUSCFG)(tcc_p2v(HwIOBUSCFG_BASE));
	volatile unsigned int cnt = 0;

	/* remap to internal ROM */
	pPMU->PMU_CONFIG.bREG.REMAP = 0;

	/* PMU is not initialized with WATCHDOG */
	pPMU->PMU_ISOL.nREG = 0x00000000;
	nop_delay(100000);
	pPMU->PWRUP_MBUS.bREG.DATA = 0;
	nop_delay(100000);
	pPMU->PWRUP_VBUS.bREG.DATA = 0;
	nop_delay(100000);
	pPMU->PWRUP_GBUS.bREG.DATA = 0;
	nop_delay(100000);
	pPMU->PWRUP_DBUS.bREG.DATA = 0;
	nop_delay(100000);
	pPMU->PWRUP_HSBUS.bREG.DATA = 0;
	nop_delay(100000);

	pPMU->PMU_SYSRST.bREG.VB = 1;
	pPMU->PMU_SYSRST.bREG.GB = 1;
	pPMU->PMU_SYSRST.bREG.DB = 1;
	pPMU->PMU_SYSRST.bREG.HSB = 1;
	nop_delay(100000);

	pIOBUSCFG->HCLKEN0.nREG = 0xFFFFFFFF;	// clock enable
	pIOBUSCFG->HCLKEN1.nREG = 0xFFFFFFFF;	// clock enable

	while (1) {
		pPMU->PMU_WDTCTRL.nREG = (Hw31 + 0x1);
	}
}
#endif
