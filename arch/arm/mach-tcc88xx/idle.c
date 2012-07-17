/*
 * linux/arch/arm/mach-tcc88xx/idle.c
 *
 * Author: <linux@telechips.com>
 * Created: August 30, 2010
 * Description: TCC88xx idle function
 *
 * Copyright (C) 2010 Telechips
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
#include <asm/atomic.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/bsp.h>

volatile static PCKC pCKC = (volatile PCKC)tcc_p2v(HwCKC_BASE);
//volatile static unsigned int bCLK0CTRL = 0;
volatile unsigned int idle_expired;
extern unsigned int g_org_tcc_clk; /* backup cpu clock value */

inline void tcc_idle(void)
{
	idle_expired = 1;

	//bCLK0CTRL = pCKC->CLK0CTRL;
	pCKC->CLK0CTRL |= (Hw20|0xFF00);
	local_irq_enable();
	while (idle_expired);
	//pCKC->CLK0CTRL = bCLK0CTRL;
	local_irq_disable();
}

inline void arch_idle_off(void)
{
    if (g_org_tcc_clk && (g_org_tcc_clk != pCKC->CLK0CTRL)) {
        pCKC->CLK0CTRL = g_org_tcc_clk;
    }
	idle_expired = 0;
}

/* end of file */
