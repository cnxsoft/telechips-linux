/*
 * linux/include/asm-arm/arch-tcc88xx/uncompress.h
 *
 * Author: <source@mvista.com>
 * Modified: <linux@telechips.com>
 * Modified: August, 2010
 * Description: Serial port stubs for kernel decompress status messages
 *
 * 2004 (c) MontaVista Software, Inc.
 * Copyright (C) 2010 Telechips
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/types.h>
#include <linux/serial_reg.h>

unsigned int system_rev;

#define ID_MASK			0x7fff

static void putc(int c)
{
	volatile u8 * uart = 0;
	int shift = 2;

	/*
	 * Now, xmit each character
	 */
	while (!(uart[UART_LSR << shift] & UART_LSR_THRE))
		barrier();
	uart[UART_TX << shift] = c;
}

static inline void flush(void)
{
}

/*
 * nothing to do
 */
#define arch_decomp_setup()
#define arch_decomp_wdog()
