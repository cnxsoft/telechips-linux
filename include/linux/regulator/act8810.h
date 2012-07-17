/*
 * act8810.h  --  Eight Channel ActivePath Power Management IC
 *
 * Copyright (C) 2011 by Telechips
 *
 * This program is free software
 *
 */

#ifndef REGULATOR_ACT8810
#define REGULATOR_ACT8810

#include <linux/regulator/machine.h>

enum {
	ACT8810_ID_DCDC1 = 0,
	ACT8810_ID_DCDC2,
	ACT8810_ID_DCDC3,
	ACT8810_ID_LDO1,
	ACT8810_ID_LDO2,
	ACT8810_ID_LDO3,
	ACT8810_ID_MAX
};

/**
 * ACT8810_subdev_data - regulator data
 * @id: regulator Id
 * @name: regulator cute name (example for V3: "vcc_core")
 * @platform_data: regulator init data (constraints, supplies, ...)
 */
struct act8810_subdev_data {
	int                         id;
	char                        *name;
	struct regulator_init_data  *platform_data;
};

/**
 * act8810_platform_data - platform data for act8810
 * @num_subdevs: number of regulators used (may be 1 or 2)
 * @subdevs: regulator used
 *           At most, there will be a regulator for V3 and one for V6 voltages.
 * @init_irq: main chip irq initialize setting.
 */
struct act8810_platform_data {
	int num_subdevs;
	struct act8810_subdev_data *subdevs;
};

#endif
