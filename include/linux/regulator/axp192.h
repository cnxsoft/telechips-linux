/*
 * axp192.h  --  Voltage regulation for the KrossPower AXP192
 *
 * Copyright (C) 2011 by Telechips
 *
 * This program is free software
 *
 */

#ifndef REGULATOR_AXP192
#define REGULATOR_AXP192

#include <linux/regulator/machine.h>

enum {
	AXP192_ID_DCDC1 = 0,
	AXP192_ID_DCDC2,
	AXP192_ID_DCDC3,
#if 0
	AXP192_ID_LDO1,
#endif
	AXP192_ID_LDO2,
	AXP192_ID_LDO3,
	AXP192_ID_LDO4,
	AXP192_ID_MAX
};

/**
 * AXP192_subdev_data - regulator data
 * @id: regulator Id (either AXP192_V3 or AXP192_V6)
 * @name: regulator cute name (example for V3: "vcc_core")
 * @platform_data: regulator init data (constraints, supplies, ...)
 */
struct axp192_subdev_data {
	int                         id;
	char                        *name;
	struct regulator_init_data  *platform_data;
};

/**
 * axp192_platform_data - platform data for axp192
 * @num_subdevs: number of regulators used (may be 1 or 2)
 * @subdevs: regulator used
 *           At most, there will be a regulator for V3 and one for V6 voltages.
 * @init_irq: main chip irq initialize setting.
 */
struct axp192_platform_data {
	int num_subdevs;
	struct axp192_subdev_data *subdevs;
	int (*init_irq)(int irq_num);
};

enum {
	AXP192_CHG_CURR_100mA = 0,
	AXP192_CHG_CURR_190mA,
	AXP192_CHG_CURR_280mA,
	AXP192_CHG_CURR_360mA,
	AXP192_CHG_CURR_450mA,
	AXP192_CHG_CURR_550mA,
	AXP192_CHG_CURR_630mA,
	AXP192_CHG_CURR_700mA,
	AXP192_CHG_CURR_780mA,
	AXP192_CHG_CURR_880mA,
	AXP192_CHG_CURR_960mA,
	AXP192_CHG_CURR_1000mA,
	AXP192_CHG_CURR_1080mA,
	AXP192_CHG_CURR_1160mA,
	AXP192_CHG_CURR_MAX
};

#define AXP192_CHG_OK			0x02
#define AXP192_CHG_ING			0x01
#define AXP192_CHG_NONE			0x00

extern int axp192_battery_voltage(void);
extern int axp192_acin_detect(void);
extern void axp192_power_off(void);
extern void axp192_charge_current(unsigned char val);
extern int axp192_charge_status(void);
#endif
