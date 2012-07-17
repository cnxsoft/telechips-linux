/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2007-2009 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef __ARCH_CONFIG_H__
#define __ARCH_CONFIG_H__

/*
 * IRQ Number
 * 3DPP 	24
 * 3DGP		25
 * 3DMMU	26
 */
#define _IRQ_3DPP_	24
#define _IRQ_3DGP_	25
#define _IRQ_3DMMU_	26


/* Note: IRQ auto detection (setting irq to -1) only works if the IRQ is not shared with any other hardware resource */

//static struct mali_arch_resource arch_configuration [] =
static _mali_osk_resource_t arch_configuration [] =
{
	{
		.type = PMU,
		.description = "Mali-200 Dummy PMU",
		.base = 0,
		.irq = -1,
		.mmu_id = 0
	},
	{
		.type = MALIGP2,
		.description = "MALI GP2",
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC88XX)
		.base = 0xf0002000,
#elif defined(CONFIG_ARCH_TCC93XX)
		.base = 0xb0702000,
#endif		
		.irq = _IRQ_3DGP_,
		.mmu_id = 1
	},
	{
		.type = MMU,
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC88XX)
		.base = 0xf0003000,
#elif defined(CONFIG_ARCH_TCC93XX)
		.base = 0xb0703000,
#endif		
		.irq = _IRQ_3DMMU_, /*105*/
		.description = "Mali MMU",
		.mmu_id = 1
	},
	{
		.type = MALI200,
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC88XX)
		.base = 0xf0000000,
#elif defined(CONFIG_ARCH_TCC93XX)
		.base = 0xb0700000,
#endif		
		.irq = _IRQ_3DPP_, /*106*/
		.description = "Mali 200 (GX525)",
		.mmu_id = 1
	},
#if USING_OS_MEMORY
	{
		.type = OS_MEMORY,
		.description = "OS Memory",
		.size = CONFIG_MALI_MEMORY_SIZE * 1024UL * 1024UL,
		.alloc_order = 0, /* highest preference for this memory */
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE | _MALI_PP_READABLE | _MALI_PP_WRITEABLE |_MALI_GP_READABLE | _MALI_GP_WRITEABLE
	},
#endif
};

#endif /* __ARCH_CONFIG_H__ */
