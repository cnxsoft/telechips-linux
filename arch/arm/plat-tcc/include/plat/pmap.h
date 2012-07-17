/* linux/arch/arm/plat-tcc/include/mach/pmap.h
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

#ifndef __PMAP_H
#define __PMAP_H

#define TCC_PMAP_NAME_LEN	16

typedef struct {
	char name[TCC_PMAP_NAME_LEN];
	__u32 base;
	__u32 size;
} pmap_t;

int pmap_get_info(const char *name, pmap_t *mem);

#endif
