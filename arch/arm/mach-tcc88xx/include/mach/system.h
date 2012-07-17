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

static inline void arch_idle(void)
{
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
#if defined(CONFIG_REGULATOR)
	struct regulator *vdd_core;
#endif
	volatile PPMU pPMU = (volatile PPMU)(tcc_p2v(HwPMU_BASE));
	volatile PIOBUSCFG pIOBUSCFG = (volatile PIOBUSCFG)(tcc_p2v(HwIOBUSCFG_BASE));

#if defined(CONFIG_REGULATOR)
	vdd_core = regulator_get(NULL, "vdd_coreA");
	if (IS_ERR(vdd_core))
		vdd_core = NULL;
	else
	{
		regulator_set_voltage(vdd_core, 1200000, 1200000);
		regulator_put(vdd_core);
	}
#endif

	pPMU->CONFIG1 &= 0xFFFFF8FF;	/* remap to internal ROM */

	pPMU->PWROFF = 0x00924820;	/* PMU is not initialized with WATCHDOG */

	pIOBUSCFG->HCLKEN0 = 0xffffffff;
	pIOBUSCFG->HCLKEN1 = 0xffffffff;

	while (1) {
		pPMU->WATCHDOG = (Hw31 + 0x1);
	}
}
#endif
