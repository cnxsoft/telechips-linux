/*
 * drivers/char/tcc_vpu.c
 *
 * TCC VPU driver
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
#include <mach/globals.h>
#include <mach/tca_ckc.h>
#include <mach/bsp.h>

#include <mach/tcc_vpu_ioctl.h>
#include <mach/tcc_fb.h>
#include <linux/delay.h>
#include <linux/cpufreq.h>
#include <asm/mach-types.h>

#if 0
static int debug	   = 1;
#else
static int debug	   = 0;
#endif

static OPENED_gINFO opened_info;

#define dprintk(msg...)	if (debug) { printk( "TCC_VPU: " msg); }

#define DEV_MAJOR	196
#define DEV_MINOR	1
#define DEV_NAME	"vpu"

#define BUS_FOR_NORMAL  0
#define BUS_FOR_VIDEO   1

static struct clk *vcore_clk;
static struct clk *vcache_clk; // for pwdn and vBus.
static struct clk *vcodec_clk; // for pwdn and vBus.
static struct clk *overlay_clk;
static struct clk *lcdc0_clk;
static struct clk *lcdc1_clk;

static int curr_type = 0;

#if defined(CONFIG_DRAM_MDDR)
	#if defined(CONFIG_HIGH_PERFORMANCE)
		#define CPU_1ST_CLOCK 600000
		#define CPU_2ND_CLOCK 500000
		#define CPU_3RD_CLOCK 425250
		
		#define MEM_1ST_CLOCK 290000
		#define MEM_2ND_CLOCK 240000
		#define MEM_3RD_CLOCK 198000
	#else
		#define CPU_1ST_CLOCK 500000
		#define CPU_2ND_CLOCK 425250
		#define CPU_3RD_CLOCK 364500
		
		#define MEM_1ST_CLOCK 240000
		#define MEM_2ND_CLOCK 198000
		#define MEM_3RD_CLOCK 166000
	#endif
#elif defined(CONFIG_DRAM_DDR2) || defined(CONFIG_DRAM_DDR3) || defined(CONFIG_DRAM_LPDDR2)
		#define CPU_1ST_CLOCK 720000
		#define CPU_2ND_CLOCK 600000
		#define CPU_3RD_CLOCK 425250
		
		#define MEM_1ST_CLOCK 330000
		#define MEM_2ND_CLOCK 290000
		#define MEM_3RD_CLOCK 198000
#endif

static unsigned int gtVpuSizeTable[] =
{
//	176 * 144 , //QCIF,	
//	352 * 288 , //CIF,	
//	400 * 240 , //WQVGA,
	
//	640 * 480 , //VGA,	
	720 * 480 , //D1,	
//	800 * 600 , //SVGA,	
	1280 * 720, //HD720P,
	1920 * 1080, //HD1080P,
};


#define SIZE_TABLE_MAX 				(sizeof(gtVpuSizeTable)/sizeof(unsigned int))

#ifdef CONFIG_CPU_FREQ
extern struct tcc_freq_table_t gtVpuNormalClockLimitTable[];
#endif

static int Get_Index_sizeTable(unsigned int Image_Size)
{
	int i;
	
	//exception process in case of error-size in parser.!!
	if(Image_Size < 64*64)
		Image_Size = 1920*1080;

	for(i=0; i<SIZE_TABLE_MAX; i++)
	{
		if( gtVpuSizeTable[i] >= Image_Size)
		{
			return i;
		}
	}
	
	return (SIZE_TABLE_MAX -1);
}

#ifdef CONFIG_VIDEO_TCCXX_CAMERA
extern void cif_set_frameskip(unsigned char skip_count, unsigned char locked);
extern int tccxxx_isalive_camera(void);
#endif

extern int tccxxx_sync_player(int sync);
static int cache_droped = 0;

#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_ARCH_TCC88XX)
void vpu_BusPrioritySetting(int mode)
{
	if(machine_is_tcc8800() || machine_is_m801_88() || machine_is_m803())
	{
		volatile unsigned int *pBUSPriorityRead = (volatile unsigned int *)tcc_p2v(HwBUSPRIORITY_READ);
		volatile unsigned int *pBUSPriorityWrite = (volatile unsigned int *)tcc_p2v(HwBUSPRIORITY_WRITE);
		
		if(mode == BUS_FOR_VIDEO)
		{
			*pBUSPriorityRead = HwBUSPRIORITY_IOBUS		| HwBUSPRIORITY_4; 	//IOBUS		: 4
			*pBUSPriorityRead = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_3; 	//GBUS	 	: 3
			*pBUSPriorityRead = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_1; 	//CPUBUS	: 1
			*pBUSPriorityRead = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
			*pBUSPriorityRead = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_2; 	//VBUS		: 2
			*pBUSPriorityRead = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_5; 	//HSIOBUS	: 5
			*pBUSPriorityRead = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_6; 	//CAMBUS	: 6
			
			*pBUSPriorityWrite = HwBUSPRIORITY_IOBUS	| HwBUSPRIORITY_4; 	//IOBUS		: 4
			*pBUSPriorityWrite = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_3; 	//GBUS	 	: 3
			*pBUSPriorityWrite = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_1; 	//CPUBUS	: 1
			*pBUSPriorityWrite = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
			*pBUSPriorityWrite = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_2; 	//VBUS		: 2
			*pBUSPriorityWrite = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_5; 	//HSIOBUS	: 5
			*pBUSPriorityWrite = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_6; 	//CAMBUS	: 6
		}
		else
		{
			*pBUSPriorityRead = HwBUSPRIORITY_IOBUS		| HwBUSPRIORITY_0; 	//IOBUS		: 0
			*pBUSPriorityRead = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_0; 	//GBUS	 	: 0
			*pBUSPriorityRead = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_0; 	//CPUBUS	: 0
			*pBUSPriorityRead = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
			*pBUSPriorityRead = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_0; 	//VBUS		: 0
			*pBUSPriorityRead = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_0; 	//HSIOBUS	: 0
			*pBUSPriorityRead = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_0; 	//CAMBUS	: 0
			
			*pBUSPriorityWrite = HwBUSPRIORITY_IOBUS	| HwBUSPRIORITY_0; 	//IOBUS		: 0
			*pBUSPriorityWrite = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_0; 	//GBUS	 	: 0
			*pBUSPriorityWrite = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_0; 	//CPUBUS	: 0
			*pBUSPriorityWrite = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
			*pBUSPriorityWrite = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_0; 	//VBUS		: 0
			*pBUSPriorityWrite = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_0; 	//HSIOBUS	: 0
			*pBUSPriorityWrite = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_0; 	//CAMBUS	: 0
		}

	}
}
#endif

#ifdef CONFIG_CPU_FREQ
int vpu_setClock(CONTENTS_INFO info)
{
	struct lcd_panel *panel = tccfb_get_panel();
	int index, img_size;
	int lcd_width = panel->xres, lcd_height = panel->yres;
	struct tcc_freq_table_t clk_table;

	img_size = info.width * info.height;
	index = Get_Index_sizeTable(img_size);
	if((index < (SIZE_TABLE_MAX-1)) && info.framerate > 30)
		index += 1;

	if(!cache_droped)
	{
		tccxxx_sync_player(1);
		cache_droped = 1;
	}

	if(info.isSWCodec)
		index = SIZE_TABLE_MAX - 1;
	
	memcpy(&clk_table, &gtVpuNormalClockLimitTable[index], sizeof(struct tcc_freq_table_t));
	
	printk("Clk_idx[%d] :: res = %d x %d, fps: %d, mbps: %d \n", index, info.width, info.height, info.framerate, info.bitrate);

	curr_type = info.type;

#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc8800() || machine_is_m801_88() || machine_is_m803())
	{
		if(info.type != VPU_ENC)
	    {
	        vpu_BusPrioritySetting(BUS_FOR_VIDEO);
	    }
	}
#endif

	if(info.isSWCodec)
		clk_table.cpu_freq = CPU_1ST_CLOCK;

	if(info.type != VPU_ENC)
	{
#if defined(CONFIG_ARCH_TCC92XX)
		if(machine_is_tcc9201() || machine_is_tcc9200s())
		{
			if(info.bitrate >= 7)
			{
				clk_table.cpu_freq = CPU_1ST_CLOCK;
				clk_table.mem_freq = MEM_1ST_CLOCK;
			}
			else if (info.bitrate >= 5)
			{
				clk_table.cpu_freq = CPU_2ND_CLOCK;
				clk_table.mem_freq = MEM_2ND_CLOCK;
			}
			else
			{
				/*NONE*/
			}
		}
		else
#endif
		{
			if(info.bitrate > 25)
			{
				if(clk_table.cpu_freq < CPU_1ST_CLOCK)
					clk_table.cpu_freq = CPU_1ST_CLOCK;
				
				if(clk_table.mem_freq < MEM_1ST_CLOCK)
					clk_table.mem_freq = MEM_1ST_CLOCK;
			}
			else if(info.bitrate > 15)
			{
				if(clk_table.cpu_freq < CPU_2ND_CLOCK)
					clk_table.cpu_freq = CPU_2ND_CLOCK;
				
				if(clk_table.mem_freq < MEM_2ND_CLOCK)
					clk_table.mem_freq = MEM_2ND_CLOCK;
			}
			else if (info.bitrate > 10)
			{ 
				if(clk_table.cpu_freq < CPU_3RD_CLOCK)
					clk_table.cpu_freq = CPU_3RD_CLOCK;
				
				if(clk_table.mem_freq < MEM_3RD_CLOCK)
					clk_table.mem_freq = MEM_3RD_CLOCK;
			}
			else
			{
				/*NONE*/
			}
		}
	}

	if(info.type == VPU_ENC)
	{
	#if defined(CONFIG_HIGH_PERFORMANCE)
		clk_table.vcod_freq = 1920000;
	#endif	
	
	#ifdef CONFIG_VIDEO_TCCXX_CAMERA
		cif_set_frameskip(20, 1); //Pause frame-update to prevent overflowing cif fifo buffer while memory clock is change. 
		tcc_cpufreq_set_limit_table(&clk_table, TCC_FREQ_LIMIT_VPU, 1);
		cif_set_frameskip(0, 0);
	#else
		tcc_cpufreq_set_limit_table(&clk_table, TCC_FREQ_LIMIT_VPU, 1);
	#endif
	}
	else
		tcc_cpufreq_set_limit_table(&clk_table, TCC_FREQ_LIMIT_VPU, 1);

	return 0;
}
#endif

int vpu_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	CONTENTS_INFO info;
	OPENED_sINFO open_info;
	
	switch(cmd)
	{
		case VPU_SET_CLK:
#ifdef CONFIG_CPU_FREQ
			if(copy_from_user(&info,(CONTENTS_INFO*)arg,sizeof(info)))
				return -EFAULT;
			return vpu_setClock(info);
#else
			return 0;
#endif


		case VPU_FORCE_CLK_CLOSE:
			if(opened_info.count != 0)
			{
				clk_disable(vcache_clk);
				clk_disable(vcodec_clk);
				clk_disable(vcore_clk);
				clk_disable(overlay_clk);
				
				opened_info.count--;
			}			
			return 0;

		case VPU_GET_OPEN_INFO:
			if (copy_to_user((unsigned int*)arg, &opened_info, sizeof(OPENED_gINFO)))
				return -EFAULT;
			else
				return 0;

		case VPU_SET_OPEN_INFO:
			if(copy_from_user(&open_info,(OPENED_sINFO*)arg,sizeof(OPENED_sINFO)))
				return -EFAULT;

			if(open_info.type == OPEN_VIDEO)
				opened_info.vid_opened = open_info.opened_cnt;
			else
				opened_info.dmb_opened = open_info.opened_cnt;
			return 0;
			
		default:
			printk(" Unsupported IOCTL!!!\n");      
			break;			
	}

	return 0;
}


static int vpu_open(struct inode *inode, struct file *filp)
{
	clk_enable(vcore_clk);
	clk_enable(vcache_clk);
	clk_enable(vcodec_clk);
	clk_enable(overlay_clk);

	clk_enable(lcdc0_clk);
	clk_enable(lcdc1_clk);

	opened_info.count++;
	dprintk("open :: vpu_opened %d \n", opened_info.count);

	return 0;
}

static unsigned int total_cnt = 0;
static int vpu_release(struct inode *inode, struct file *filp)
{
	PLCDC pLCDC1;

#ifdef CONFIG_VIDEO_TCCXX_CAMERA
	if(!tccxxx_isalive_camera())
#endif
	{	
		volatile PCKC pCKC;

		pCKC = (volatile PCKC)tcc_p2v(HwCKC_BASE);
		
		//Disable overlay-ch0 in vpu-close because of performance-dead.!!
		if(pCKC->PCLK_LCD1 & Hw28){
			pLCDC1 = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
			if(pLCDC1->LI0C & Hw28)
			{
				BITCSET (pLCDC1->LI0C, 0x1<<28, (0)<<28);	
#if !defined(CONFIG_ARCH_TCC92XX)
				BITCSET (pLCDC1->LI0C, HwLCT_RU, HwLCT_RU); //Image update 
#endif			
				mdelay(16);
			}
		}
	}

	if(opened_info.count != 0)
	{
		clk_disable(lcdc0_clk);
		clk_disable(lcdc1_clk);
		
		clk_disable(vcache_clk);
		clk_disable(vcodec_clk);
		clk_disable(vcore_clk);
		clk_disable(overlay_clk);
		
		opened_info.count--;
	}
	else
	{
		printk("closk for vpu already is closed \n");
	}

	dprintk("VPU CLOCK ::rel initialized to 0 \n");

#ifdef CONFIG_CPU_FREQ
	if(opened_info.count == 0)
		tcc_cpufreq_set_limit_table(&gtVpuNormalClockLimitTable[0], TCC_FREQ_LIMIT_VPU, 0);
#endif

#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc8800() || machine_is_m801_88() || machine_is_m803())
	{
		if(curr_type != VPU_ENC)
	    {
	        vpu_BusPrioritySetting(BUS_FOR_NORMAL);
	    }
	}
#endif

	total_cnt++;

	printk("release :: vpu_opened %d , total = %d \n", opened_info.count, total_cnt);

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

static struct vm_operations_struct vpu_mmap_ops = {
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

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int vpu_mmap(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;

	if(range_is_allowed(vma->vm_pgoff, size) < 0){
		printk(KERN_ERR  "vpu: this address is not allowed \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	vma->vm_ops = &vpu_mmap_ops;

	if (remap_pfn_range(vma,
			    vma->vm_start,
			    vma->vm_pgoff,
			    size,
			    vma->vm_page_prot)) {
		printk(KERN_ERR	 "vpu: remap_pfn_range failed\n");
		return -EAGAIN;
	}
	return 0;
}

static struct file_operations vpu_fops = {
	.open		= vpu_open,
	.release	= vpu_release,
	.mmap		= vpu_mmap,
	.ioctl		= vpu_ioctl,
};

static void __exit vpu_cleanup(void)
{
	clk_put(vcore_clk);
	clk_put(vcache_clk);
	clk_put(vcodec_clk);
	clk_put(overlay_clk);

	unregister_chrdev(DEV_MAJOR, DEV_NAME);
}

static struct class *vpu_class;

static int __init vpu_init(void)
{
	total_cnt = 0;

	cache_droped = 0;
	memset(&opened_info, 0x00, sizeof(opened_info));
	
	vcore_clk = clk_get(NULL, "vcore");
	BUG_ON(vcore_clk == NULL);

	vcache_clk = clk_get(NULL, "vcache");
	BUG_ON(vcache_clk == NULL);
	
	vcodec_clk = clk_get(NULL, "vcodec");
	BUG_ON(vcodec_clk == NULL);

	overlay_clk = clk_get(NULL, "overlay");
	BUG_ON(overlay_clk == NULL);

	lcdc0_clk = clk_get(0, "lcdc0");
	BUG_ON(lcdc0_clk == NULL);

	lcdc1_clk = clk_get(0, "lcdc1");
	BUG_ON(lcdc1_clk == NULL);

	if (register_chrdev(DEV_MAJOR, DEV_NAME, &vpu_fops))
		printk(KERN_ERR "unable to get major %d for VPU device\n", DEV_MAJOR);
	vpu_class = class_create(THIS_MODULE, DEV_NAME);
	device_create(vpu_class, NULL, MKDEV(DEV_MAJOR, DEV_MINOR), NULL, DEV_NAME);
	return 0;
}

module_init(vpu_init)
module_exit(vpu_cleanup)
