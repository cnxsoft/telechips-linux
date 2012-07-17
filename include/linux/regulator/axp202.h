/*
 * axp202.h  --  Voltage regulation for the KrossPower AXP202
 *
 * Copyright (C) 2012 by Telechips
 *
 * This program is free software
 *
 */

#ifndef REGULATOR_AXP202
#define REGULATOR_AXP202

#include <linux/regulator/machine.h>

enum {
	AXP202_ID_DCDC2 = 0,
	AXP202_ID_DCDC3,
	AXP202_ID_LDO2,
	AXP202_ID_LDO3,
	AXP202_ID_LDO4,
	AXP202_ID_LDO5,
	AXP202_ID_MAX
};

/**
 * AXP202_subdev_data - regulator data
 * @id: regulator Id (either AXP202_ID_DCDC2 or AXP202_ID_DCDC3 or ,,,)
 * @name: regulator cute name (example for AXP202_ID_DCDC2: "vcc_core")
 * @platform_data: regulator init data (constraints, supplies, ...)
 */
struct axp202_subdev_data {
	int                         id;
	char                        *name;
	struct regulator_init_data  *platform_data;
};

/**
 * axp202_platform_data - platform data for axp202
 * @num_subdevs: number of regulators used (may be 1 or 2 or ,,)
 * @subdevs: regulator used
 *           At most, there will be a regulator for AXP202_ID_DCDC2 and one for AXP202_ID_LDO5 voltages.
 * @init_irq: main chip irq initialize setting.
 */
struct axp202_platform_data {
	int num_subdevs;
	struct axp202_subdev_data *subdevs;
	int (*init_port)(int irq_num);
};

enum {
	AXP202_CHG_CURR_300mA = 0,
	AXP202_CHG_CURR_400mA,
	AXP202_CHG_CURR_500mA,
	AXP202_CHG_CURR_600mA,
	AXP202_CHG_CURR_700mA,
	AXP202_CHG_CURR_800mA,
	AXP202_CHG_CURR_900mA,
	AXP202_CHG_CURR_1000mA,
	AXP202_CHG_CURR_1100mA,
	AXP202_CHG_CURR_1200mA,
	AXP202_CHG_CURR_1300mA,
	AXP202_CHG_CURR_1400mA,
	AXP202_CHG_CURR_1500mA,
	AXP202_CHG_CURR_1600mA,
	AXP202_CHG_CURR_1700mA,
	AXP202_CHG_CURR_1800mA,
	AXP202_CHG_CURR_MAX
};

#define AXP202_CHG_OK			0x02
#define AXP202_CHG_ING			0x01
#define AXP202_CHG_NONE			0x00

extern int axp202_battery_voltage(void);
extern int axp202_acin_detect(void);
extern int axp202_vbus_voltage(void);
extern void axp202_power_off(void);
extern void axp202_charge_current(unsigned char val);
extern int axp202_charge_status(void);
#endif
