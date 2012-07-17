/*
 * rn5t614.h  --  Voltage regulation for the RICOH RN5T614
 *
 * Copyright (C) 2011 by Telechips
 *
 * This program is free software
 *
 */

#ifndef REGULATOR_RN5T614
#define REGULATOR_RN5T614

#include <linux/regulator/machine.h>

enum {
	RN5T614_ID_DCDC1 = 0,
	RN5T614_ID_DCDC2,
	RN5T614_ID_DCDC3,
	RN5T614_ID_LDO1,
	RN5T614_ID_LDO2,
	RN5T614_ID_LDO3,
	RN5T614_ID_LDO4,
	RN5T614_ID_LDO5,
	RN5T614_ID_LDO6,
	RN5T614_ID_LDO7,
	RN5T614_ID_LDO8,
	RN5T614_ID_MAX
};

/**
 * RN5T614_subdev_data - regulator data
 * @id: regulator Id
 * @name: regulator cute name (example: "vcc_core")
 * @platform_data: regulator init data (constraints, supplies, ...)
 */
struct rn5t614_subdev_data {
	int				id;
	char				*name;
	struct regulator_init_data	*platform_data;
};

/**
 * rn5t614_platform_data - platform data for rn5t614
 * @num_subdevs: number of regulators used (may be 1 or 2)
 * @subdevs: regulator used
 * @init_irq: main chip irq initialize setting.
 * sub_addr: 0x64 (0110010b)
 */
struct rn5t614_platform_data {
	int num_subdevs;
	struct rn5t614_subdev_data *subdevs;
	int (*init_port)(int irq_num);
};


#define RN5T614_CHG_TIMEOUT		0x03
#define RN5T614_CHG_OK			0x02
#define RN5T614_CHG_ING			0x01
#define RN5T614_CHG_NONE		0x00

#endif
