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
 * 3DPPMMU	16
 * 3DGPMMU	17
 * 3DL2		19
 * 3DPP		24
 * 3DGP		25
 * 3DPMU		26
 */
#define _IRQ_3DPPMMU_	16
#define _IRQ_3DGPMMU_	17
#define _IRQ_3DL2_	19
#define _IRQ_3DPP_	24
#define _IRQ_3DGP_	25
#define _IRQ_3DPMU_	26

/* Note: IRQ auto detection (setting irq to -1) only works if the IRQ is not shared with any other hardware resource */
static _mali_osk_resource_t arch_configuration [] =
{

    	{
                .type = PMU,
                .description = "Mali-400 PMU",
                .base = 0x70002000,
                //.irq = _IRQ_3DPMU_,		
                //.mmu_id = 0
        },
	{
		.type = MALI400GP,
		.description = "Mali-400 GP",
		.base = 0x70000000,
		.irq = _IRQ_3DGP_,
		.mmu_id = 1
	},
	{
		.type = MALI400PP,
		.base = 0x70008000,
		.irq = _IRQ_3DPP_,
		.description = "Mali-400 PP",
		.mmu_id = 2
	},
	{
		.type = MMU,
		.base = 0x70003000,
		.irq = _IRQ_3DGPMMU_,
		.description = "Mali-400 MMU for GP",
		.mmu_id = 1
	},
	{
		.type = MMU,
		.base = 0x70004000,
		.irq = _IRQ_3DPPMMU_,
		.description = "Mali-400 MMU for PP",
		.mmu_id = 2
	},
#if USING_OS_MEMORY
	{		
		.type = OS_MEMORY,
		.description = "OS Memory",
		.size = CONFIG_MALI_MEMORY_SIZE * 1024UL * 1024UL,
		.alloc_order = 0, /* highest preference for this memory */
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE | _MALI_PP_READABLE | _MALI_PP_WRITEABLE |_MALI_GP_READABLE | _MALI_GP_WRITEABLE
	},
#endif /*USING_OS_MEMORY*/
#if 0
	{
		.type = MEM_VALIDATION,
		.description = "Framebuffer",
		.base = 0xe0000000,
		.size = 0x01000000,
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE | _MALI_PP_WRITEABLE | _MALI_PP_READABLE
	},
#endif	
	{
		.type = MALI400L2,
		.base = 0x70001000,
		.description = "Mali-400 L2 cache"
	},
	

};
#endif /* __ARCH_CONFIG_H__ */
