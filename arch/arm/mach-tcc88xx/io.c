/*
 * linux/arch/arm/mach-tcc88xx/io.c
 *
 * Author:  <linux@telechips.com>
 * Created: August 30, 2010
 * Description: TCC88xx mapping code
 *
 * Copyright (C) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bootmem.h>
#include <linux/init.h>
#include <linux/memblock.h>

#include <asm/tlb.h>
#include <asm/mach/map.h>
#include <asm/io.h>

#include <mach/bsp.h>
#include <plat/pmap.h>
#include <mach/tca_ckc.h>

#if defined(__TODO__)
extern void IO_UTIL_ReadECID(void);
#endif

/*
 * The machine specific code may provide the extra mapping besides the
 * default mapping provided here.
 */
static struct map_desc tcc8800_io_desc[] __initdata = {
    {
        .virtual    = 0xF0000000,
        .pfn        = __phys_to_pfn(0xF0000000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0100000,
        .pfn        = __phys_to_pfn(0xF0100000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0200000,
        .pfn        = __phys_to_pfn(0xF0200000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0300000,
        .pfn        = __phys_to_pfn(0xF0300000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0400000,
        .pfn        = __phys_to_pfn(0xF0400000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0500000,
        .pfn        = __phys_to_pfn(0xF0500000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0600000,
        .pfn        = __phys_to_pfn(0xF0600000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0700000,
        .pfn        = __phys_to_pfn(0xF0700000),
        .length     = 0x100000,
        .type       = MT_MEMORY_TCC //Bruce, 101207, mmu on/off를 위해서, Video FIFO를 Excute area로 사용하며, 이를 위해서 Cache 속성을 바꿈.
    },
    {
        .virtual    = 0xF0800000,
        .pfn        = __phys_to_pfn(0xF0800000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0900000,
        .pfn        = __phys_to_pfn(0xF0900000),
        .length     = 0x100000,
        .type       = MT_DEVICE
    },
    {
        .virtual    = 0xF0A00000, //0xEFF00000,
        .pfn        = __phys_to_pfn(0x10000000),
        .length     = 0x100000,
        .type       = MT_MEMORY_TCC
    },
    {
        .virtual    = 0xF0B00000, //0xEFE00000,
        .pfn        = __phys_to_pfn(0x30000000),
        .length     = 0x100000,
        .type       = MT_MEMORY_TCC
    },
};

void __init tcc_reserve_sdram(void)
{
	pmap_t pmap;
#if 0
	if (pmap_get_info("total", &pmap)) {
		if (reserve_bootmem(pmap.base, pmap.size, BOOTMEM_EXCLUSIVE) == 0) {
			printk(KERN_DEBUG "Total reserved memory: base=0x%x, size=0x%x\n", pmap.base, pmap.size);
		} else {
			printk(KERN_ERR "Can't reserve memory (base=0x%x, size=0x%x)\n", pmap.base, pmap.size);
		}
	}
#else
	if (pmap_get_info("total", &pmap)) {
		if (memblock_remove(pmap.base, pmap.size) == 0) {
			printk(KERN_INFO "Total reserved memory: base=0x%x, size=0x%x\n", pmap.base, pmap.size);
		} else {
			printk(KERN_ERR "Can't reserve memory (base=0x%x, size=0x%x)\n", pmap.base, pmap.size);
		}
	}
#endif
}

/*
 *  Maps common IO regions for tcc88xx.
 */
void __init tcc_map_common_io(void)
{
	iotable_init(tcc8800_io_desc, ARRAY_SIZE(tcc8800_io_desc));

	/* Normally devicemaps_init() would flush caches and tlb after
	 * mdesc->map_io(), but we must also do it here because of the CPU
	 * revision check below.
	 */
	local_flush_tlb_all();
	flush_cache_all();
#if defined(__TODO__)
	IO_UTIL_ReadECID();
#endif

	//tcc_reserve_sdram();

	// XXX
	tca_ckc_init();
}
