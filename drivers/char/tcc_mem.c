/*
 * drivers/char/tcc_mem.c
 *
 * TCC MEM driver
 *
 * Copyright (C) 2009 Telechips, Inc.
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/init.h>
#include <linux/ptrace.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/syscalls.h>
#include <linux/version.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
#include <asm/pgtable.h>
#endif
#include <mach/globals.h>
#include <mach/bsp.h>

#include <asm/mach-types.h>
#include <plat/pmap.h>

#define DEV_MAJOR	213
#define DEV_MINOR	1
#define DEV_NAME	"tmem"


#define dprintk(msg...)	if (0) { printk( "tMEM: " msg); }

#if defined(CONFIG_ARCH_TCC88XX)
#define VPU_BASE		0xF0700000
#define CLK_BASE        0xF0400000
#define DXB_BASE1       0x30000000
#define DXB_BASE2       0xF08C0000
#define AV_BASE         0xF0538000
#define AV_BASE2        0xF0308000
#elif defined(CONFIG_ARCH_TCC93XX)
#define VPU_BASE		0xB0900000
#define CLK_BASE        0xB0500000
#define DXB_BASE1       0xE0000000
#define DXB_BASE2       0x00000000
#define AV_BASE         0xB010C000
#define AV_BASE2        0xB010A000
#elif defined(CONFIG_ARCH_TCC892X)
#define VPU_BASE		0x75000000
#define JPEG_BASE		0x75300000
#define CLK_BASE        0x74000000
#define DXB_BASE1       0x30000000 // have to modify
#define DXB_BASE2       0xF08C0000 // have to modify
#define AV_BASE         0x76067000
#define AV_BASE2        0x74200000
#else
#define VPU_BASE		0xF0700000
#define CLK_BASE        0xF0400000
#define DXB_BASE1       0xE0000000
#define DXB_BASE2       0x00000000
#define AV_BASE         0xE0000000
#define AV_BASE2        0xF0102000
#endif

static pmap_t pmap_total;

struct allow_region {
	unsigned long	start;
	unsigned long	len;
};

struct allow_region AllowRegion[] = {
	{ VPU_BASE, 0x21000}, 	//VPU Register..	
	{ CLK_BASE, 0x1000}, 	//to check vpu-running.
	{ DXB_BASE1,0x1000}, 	//for broadcasting.
	{ DXB_BASE2,0x1000}, 	//for broadcasting.
#ifdef CONFIG_ARCH_TCC892X
	{ JPEG_BASE,0x2000}, 	//JPEG(in VPU) Register..	
#endif

	{ AV_BASE,  0x1000},  	//for AV_algorithm.
	{ AV_BASE2, 0x1000}  	  //for AV_algorithm.

};

#define SIZE_TABLE_MAX (sizeof(AllowRegion)/sizeof(struct allow_region))

//extern  void drop_pagecache(void);
extern char tcc_hdmi_open_num;
int tccxxx_sync_player(int sync)
{
	if(sync)
	{
		sys_sync();
//		drop_pagecache();
	}
#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_ARCH_TCC88XX)
	else
	{
		if(machine_is_tcc8800() || machine_is_m801_88() || machine_is_m803())
		{
			if(tcc_hdmi_open_num > 0)
				return 1;
		}
	}
#endif

	return 0;
}
EXPORT_SYMBOL(tccxxx_sync_player);

int tccxxx_get_pmap(const char *name, pmap_t *mem)
{
	if(name == NULL)
		return -1;
	
	pmap_get_info(name, mem);

	return 0;
}
EXPORT_SYMBOL(tccxxx_get_pmap);

static int sync_once = 1;
static int tmem_open(struct inode *inode, struct file *filp)
{
	dprintk("tmem_open \n");
	if(sync_once){
		tccxxx_sync_player(0);
		tccxxx_get_pmap(NULL, NULL);
		sync_once = 0;
	}

	return 0;
}

static int tmem_release(struct inode *inode, struct file *filp)
{
	dprintk("tmem_release  \n");

	return 0;
}

static void mmap_mem_open(struct vm_area_struct *vma)
{
	/* do nothing */
}

static void mmap_mem_close(struct vm_area_struct *vma)
{
	/* do nothing */
}

static struct vm_operations_struct tmem_mmap_ops = {
	.open = mmap_mem_open,
	.close = mmap_mem_close,
};

static inline int uncached_access(struct file *file, unsigned long addr)
{
	if (file->f_flags & O_SYNC)
		return 1;
	return addr >= __pa(high_memory);
}

#if !(LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
static pgprot_t phys_mem_access_prot(struct file *file, unsigned long pfn,
				     unsigned long size, pgprot_t vma_prot)
{
	unsigned long offset = pfn << PAGE_SHIFT;

	if (uncached_access(file, offset))
		return pgprot_noncached(vma_prot);
	return vma_prot;
}
#endif

int range_is_allowed(unsigned long pfn, unsigned long size)
{
	int i;
	unsigned long request_start, request_end;

	request_start = pfn << PAGE_SHIFT;
	request_end = request_start + size;
	dprintk("Req: 0x%x - 0x%x, 0x%x \n", request_start, request_end, size);

//to check reserved physical memory.
	if((pmap_total.base <= request_start) && ((pmap_total.base+pmap_total.size) >= request_end))
	{
		dprintk("Allowed Reserved Physical mem : 0x%x <= 0x%x && 0x%x >= 0x%x \n", pmap_total.base, request_start, (pmap_total.base+pmap_total.size), request_end);
		return 1;
	}

	for(i=0; i<SIZE_TABLE_MAX; i++)
	{
		if((AllowRegion[i].start <= request_start) && ((AllowRegion[i].start+AllowRegion[i].len) >= request_end))
		{
			dprintk("Allowed : 0x%x <= 0x%x && 0x%x >= 0x%x \n", AllowRegion[i].start, request_start, (AllowRegion[i].start+AllowRegion[i].len), request_end);
			return 1;
		}
	}

	printk("Can't allow to mmap : size %d, 0x%x ~ 0x%x \n", size, request_start, request_end);
	
	return -1;
}
EXPORT_SYMBOL(range_is_allowed);

static int tmem_mmap(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;

	if(range_is_allowed(vma->vm_pgoff, size) < 0){
		printk(KERN_ERR	 "tmem: this address is not allowed \n");
		return -EPERM;
	}

	vma->vm_page_prot = phys_mem_access_prot(file, vma->vm_pgoff, size, vma->vm_page_prot);
	vma->vm_ops = &tmem_mmap_ops;

	if (remap_pfn_range(vma,
			    vma->vm_start,
			    vma->vm_pgoff,
			    size,
			    vma->vm_page_prot)) {
		printk(KERN_ERR	 "tmem: remap_pfn_range failed \n");
		return -EAGAIN;
	}
	return 0;
}

static struct file_operations tmem_fops = {
	.open		= tmem_open,
	.release	= tmem_release,
	.mmap		= tmem_mmap,
};

static void __exit tmem_cleanup(void)
{
	unregister_chrdev(DEV_MAJOR,DEV_NAME);
}

static struct class *tmem_class;

static int __init tmem_init(void)
{
	pmap_get_info("total", &pmap_total);

	if (register_chrdev(DEV_MAJOR, DEV_NAME, &tmem_fops))
		printk(KERN_ERR "unable to get major %d for tMEM device\n", DEV_MAJOR);
	tmem_class = class_create(THIS_MODULE, DEV_NAME);
	device_create(tmem_class, NULL, MKDEV(DEV_MAJOR, DEV_MINOR), NULL, DEV_NAME);
	return 0;
}

module_init(tmem_init)
module_exit(tmem_cleanup)
