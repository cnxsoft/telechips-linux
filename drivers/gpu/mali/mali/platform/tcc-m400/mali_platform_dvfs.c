/*
 * Copyright (C) 2010 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_platform.h"
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <asm/io.h>
#include "mali_device_pause_resume.h"
#include <linux/workqueue.h>
#include <linux/cpufreq.h>

extern struct tcc_freq_table_t gtMaliClockLimitTable[];
#define MALI_DVFS_STEPS 7 //ARRAY_SIZE(gtMaliClockLimitTable)

#define GPU_UP_THRESHOLD	((int)((85*255)/100))
#define GPU_DOWN_THRESHOLD	((int)((75*255)/100))

static struct workqueue_struct *mali_dvfs_wq = 0;
static unsigned int maliDvfsCurrentStep;
static u32 mali_dvfs_utilization = 255;

static void mali_dvfs_work_handler(struct work_struct *w);
static DECLARE_WORK(mali_dvfs_work, mali_dvfs_work_handler);

__inline static unsigned int decideNextStatus(unsigned int utilization)
{
    unsigned int level;
	level = maliDvfsCurrentStep;

    if(utilization > GPU_UP_THRESHOLD) {
	    level++;
		if (level >= (MALI_DVFS_STEPS-1))
			level = MALI_DVFS_STEPS-1;
	}
    else if(utilization < GPU_DOWN_THRESHOLD) {
		if (level > 0)
			level--;
	}

	return level;
}

static void mali_dvfs_work_handler(struct work_struct *w)
{
	unsigned int nextStep = 0;
	unsigned int currStep = 0;
	mali_bool boostup = MALI_FALSE;

	/*decide next step*/
	currStep = maliDvfsCurrentStep;
	nextStep = decideNextStatus(mali_dvfs_utilization);

	/*if next status is same with current status, don't change anything*/
	if(currStep != nextStep) {
		/*check if boost up or not*/
		if(nextStep > currStep)
			boostup = 1;

		/*change mali dvfs status*/
	    if(boostup) {
			tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable[nextStep], TCC_FREQ_LIMIT_MALI, 1);
		}
	    else {
			tcc_cpufreq_set_limit_table(&gtMaliClockLimitTable[nextStep], TCC_FREQ_LIMIT_MALI, 1);
		}

	    maliDvfsCurrentStep = nextStep;
	}
}

mali_bool init_mali_dvfs_staus(int step)
{
	if (!mali_dvfs_wq)
		mali_dvfs_wq = create_singlethread_workqueue("mali_dvfs");
	maliDvfsCurrentStep = step;
	return MALI_TRUE;
}

void deinit_mali_dvfs_staus(void)
{
	if (mali_dvfs_wq)
		destroy_workqueue(mali_dvfs_wq);
	mali_dvfs_wq = NULL;
}

mali_bool mali_dvfs_handler(u32 utilization)
{
	mali_dvfs_utilization = utilization;
	if (mali_dvfs_wq)
		queue_work_on(0, mali_dvfs_wq,&mali_dvfs_work);
	return MALI_TRUE;
}
