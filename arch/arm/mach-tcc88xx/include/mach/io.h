/*
 * linux/include/asm-arm/arch-tcc88xx/io.h
 *
 * Based on:    linux/include/asm-arm/arch-sa1100/io.h
 * Author : <linux@telechips.com>
 * Description: IO definitions for TCC88xx processors and boards
 *
 * Copyright (C) 1997-1999 Russell King
 * Copyright (C) 2008-2010 Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __ASM_ARM_ARCH_IO_H__
#define __ASM_ARM_ARCH_IO_H__

#include <mach/hardware.h>

#define IO_SPACE_LIMIT 0xffffffff

/*
 * We don't actually have real ISA nor PCI buses, but there is so many
 * drivers out there that might just work if we fake them...
 */
#define __io(a)		((void __iomem *)(PCIO_BASE + (a)))
#define __mem_pci(a)	(a)

/*
 * ----------------------------------------------------------------------------
 * I/O mapping
 * ----------------------------------------------------------------------------
 */
#define PCIO_BASE	0

#define IO_PHYS		0xF0000000
#define IO_OFFSET	0x00000000	/* Virtual IO = 0xf0000000 */
#define IO_SIZE		0x100000
#define IO_VIRT		(IO_PHYS - IO_OFFSET)
#define IO_ADDRESS(pa)	((pa) - IO_OFFSET)
#define io_p2v(pa)	((pa) - IO_OFFSET)
#define io_v2p(va)	((va) + IO_OFFSET)

/* Physical value to Virtual Address */
#define tcc_p2v(pa)         io_p2v(pa)

#define tcc_readb(a)		(*(volatile unsigned char  *)IO_ADDRESS(a))
#define tcc_readw(a)		(*(volatile unsigned short *)IO_ADDRESS(a))
#define tcc_readl(a)		(*(volatile unsigned int   *)IO_ADDRESS(a))

#define tcc_writeb(v,a)		(*(volatile unsigned char  *)IO_ADDRESS(a) = (v))
#define tcc_writew(v,a)		(*(volatile unsigned short *)IO_ADDRESS(a) = (v))
#define tcc_writel(v,a)		(*(volatile unsigned int   *)IO_ADDRESS(a) = (v))

#endif	/*__ASM_ARM_ARCH_IO_H__*/
