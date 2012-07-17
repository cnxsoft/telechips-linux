/*
 * linux/arch/arm/mach-tcc88xx/include/mach/clock.h
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __TCC88XX_CLOCK_H
#define __TCC88XX_CLOCK_H

/* Flags */
#define CLK_FIXED_RATE		(1 << 0)
#define CLK_ALWAYS_ENABLED	(1 << 1)
#define CLK_AUTO_OFF		(1 << 2)
#define CLK_RATE_PROPAGATES	(1 << 3)

struct clk {
	struct list_head list;
	unsigned int id;
	const char *name;
	struct clk *parent;
	struct device *dev;
	unsigned long rate;			// requested rate
	unsigned long real_rate;	// setted rate
	unsigned int flags;
	int usecount;
	unsigned long (*get_rate)(struct clk *);
	int (*set_rate)(struct clk *, unsigned long);
	int (*enable)(struct clk *);
	void (*disable)(struct clk *);
	unsigned int pwdn_idx;
	int (*pwdn)(unsigned int, unsigned int);
	unsigned int swreset_idx;
	int (*swreset)(unsigned int, unsigned int);
	unsigned int clock_idx;
	int (*clock)(unsigned int, unsigned int);
};

#endif
