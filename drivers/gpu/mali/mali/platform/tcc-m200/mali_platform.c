/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009-2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/**
 * @file mali_platform.c
 * Platform specific Mali driver functions for a default platform
 */
#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_platform.h"

#include <linux/clk.h>
#include <linux/cpufreq.h>

static struct clk *mali_clk = NULL;

extern struct tcc_freq_table_t gtMaliClockLimitTable;
typedef enum	{
	MALI_CLK_NONE=0,
	MALI_CLK_ENABLED,
	MALI_CLK_DISABLED
} t_mali_clk_type;
static t_mali_clk_type mali_clk_enable = MALI_CLK_NONE;

_mali_osk_errcode_t mali_platform_init(void)
{
	if(mali_clk_enable != MALI_CLK_ENABLED)
	{
//		printk("mali_platform_init() clk_enable\n");
		if (mali_clk == NULL)
			mali_clk = clk_get(NULL, "mali_clk");	
		clk_enable(mali_clk);
		tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable, TCC_FREQ_LIMIT_MALI, 1);
		mali_clk_enable = MALI_CLK_ENABLED;
	}

    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_deinit(void)
{
	if(mali_clk_enable == MALI_CLK_ENABLED)
	{
//		printk("mali_platform_deinit() clk_disable\n");
		if (mali_clk == NULL)
			mali_clk = clk_get(NULL, "mali_clk");	
		tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable, TCC_FREQ_LIMIT_MALI, 0);
		clk_disable(mali_clk);
		mali_clk_enable = MALI_CLK_DISABLED;

	}

    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_powerdown(u32 cores)
{
	if(mali_clk_enable == MALI_CLK_ENABLED)
	{
//		printk("mali_platform_powerdown() clk_disable\n");
		if (mali_clk == NULL)
			mali_clk = clk_get(NULL, "mali_clk");	
		tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable, TCC_FREQ_LIMIT_MALI, 0);
		clk_disable(mali_clk);
		mali_clk_enable = MALI_CLK_DISABLED;

	}
    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_powerup(u32 cores)
{
	if(mali_clk_enable != MALI_CLK_ENABLED)
	{
//		printk("mali_platform_powerup() clk_enable\n");
		if (mali_clk == NULL)
			mali_clk = clk_get(NULL, "mali_clk");	
		clk_enable(mali_clk);
		tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable, TCC_FREQ_LIMIT_MALI, 1);
		mali_clk_enable = MALI_CLK_ENABLED;

	}
    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_power_mode_change(mali_power_mode power_mode)
{
	if(power_mode == MALI_POWER_MODE_ON)
	{
		if(mali_clk_enable != MALI_CLK_ENABLED)
		{
//			printk("mali_platform_power_mode_change() clk_enable\n");
			if (mali_clk == NULL)
				mali_clk = clk_get(NULL, "mali_clk");	
			clk_enable(mali_clk);
			tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable, TCC_FREQ_LIMIT_MALI, 1);
			mali_clk_enable = MALI_CLK_ENABLED;
		}
	}

	else
	{
		if( mali_clk_enable == MALI_CLK_ENABLED)
		{
//			printk("mali_platform_power_mode_change() clk_disable\n");
			if (mali_clk == NULL)
				mali_clk = clk_get(NULL, "mali_clk");	
			tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable, TCC_FREQ_LIMIT_MALI, 0);
			clk_disable(mali_clk);
			mali_clk_enable = MALI_CLK_DISABLED;
		}

	}
    MALI_SUCCESS;
}

void mali_gpu_utilization_handler(u32 utilization)
{
}

void set_mali_parent_power_domain(void* dev)
{
}
