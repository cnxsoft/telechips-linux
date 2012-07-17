/*
 * axp202.c  --  Voltage and current regulation for the AXP202
 *
 * Copyright (C) 2012 by Telechips
 *
 * This program is free software;
 *
 */
#include <linux/module.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <asm/mach-types.h>

#include <linux/regulator/axp202.h>

#if defined(DEBUG)
#define dbg(msg...) printk("AXP202: " msg)
#else
#define dbg(msg...)
#endif

#define AXP202_MIN_UV   700000
#define AXP202_MAX_UV  3500000


/********************************************************************
	Define Types
********************************************************************/
struct axp202_voltage_t {
	int uV;
	u8  val;
};

struct axp202_data {
	struct i2c_client *client;
	unsigned int min_uV;
	unsigned int max_uV;
	struct work_struct work;
#if defined(CONFIG_REGULATOR_AXP202_PEK)
	struct input_dev *input_dev;
	struct timer_list timer;
#endif
	struct regulator_dev *rdev[0];
};


/********************************************************************
	I2C Command & Values
********************************************************************/
/* Power Control */
#define AXP202_POWER_STS_REG            0x00
#define AXP202_MODE_CHARGE_STS_REG      0x01
#define AXP202_OTG_VBUS_STS_REG         0x02
#define AXP202_POWER_OUT_CTRL_REG       0x12
#define AXP202_DCDC2_REG                0x23
#define AXP202_DCDC2_LDO3_DVS_REG       0x25
#define AXP202_DCDC3_REG                0x27
#define AXP202_LDO24_REG                0x28
#define AXP202_LDO3_REG                 0x29
#define AXP202_VBUS_IPSOUT_REG          0x30
#define AXP202_POWER_OFF_REG            0x31
#define AXP202_SHDN_BATT_CHGLED_REG     0x32
#define AXP202_CHARGING_CTRL1_REG       0x33
#define AXP202_CHARGING_CTRL2_REG       0x34
#define AXP202_BACKUP_BATT_CHG_REG      0x35
#define AXP202_PEK_REG                  0x36
#define AXP202_DCDC_WORK_FREQ_REG       0x37
#define AXP202_BATT_CHG_UNDER_TEMP_REG  0x38
#define AXP202_BATT_CHG_OVER_TEMP_REG   0x39
#define AXP202_IPSOUT_V_WARNING_1_REG   0x3A
#define AXP202_IPSOUT_V_WARNING_2_REG   0x3B
#define AXP202_BATT_DCHG_UNDER_TEMP_REG 0x3C
#define AXP202_BATT_DCHG_OVER_TEMP_REG  0x3D
#define AXP202_DCDC_WORK_MODE_REG       0x80
#define AXP202_ADC_ENABLE1_REG          0x82
#define AXP202_ADC_ENABLE2_REG          0x83
#define AXP202_ADC_TSPIN_REG            0x84
#define AXP202_ADC_INPUT_RANGE_REG      0x85
#define AXP202_TIMER_CTRL               0x8A
#define AXP202_VBUS_DET_SRP_FUNC_REG    0x8B
#define AXP202_OVER_TEMP_SHDN_REG       0x8F

/* GPIO control */
#define AXP202_LDO5_FUNC_SET_REG        0x90
#define AXP202_LDO5_REG                 0x91
#define AXP202_GPIO1_FUNC_REG           0x92
#define AXP202_GPIO2_FUNC_REG           0x93
#define AXP202_GPIO012_STS_REG          0x94
#define AXP202_GPIO3_FUNC_REG           0x95

/* Interrupt control */
#define AXP202_IRQ_EN1_REG              0x40
#define AXP202_IRQ_EN2_REG              0x41
#define AXP202_IRQ_EN3_REG              0x42
#define AXP202_IRQ_EN4_REG              0x43
#define AXP202_IRQ_EN5_REG              0x44
#define AXP202_IRQ_STS1_REG             0x48
#define AXP202_IRQ_STS2_REG             0x49
#define AXP202_IRQ_STS3_REG             0x4A
#define AXP202_IRQ_STS4_REG             0x4B
#define AXP202_IRQ_STS5_REG             0x4C

/* ADC data */
#define AXP202_ADC_ACIN_VOL_H_REG       0x56
#define AXP202_ADC_ACIN_VOL_L_REG       0x57
#define AXP202_ADC_ACIN_CUR_H_REG       0x58
#define AXP202_ADC_ACIN_CUR_L_REG       0x59
#define AXP202_ADC_VBUS_VOL_H_REG       0x5A
#define AXP202_ADC_VBUS_VOL_L_REG       0x5B
#define AXP202_ADC_VBUS_CUR_H_REG       0x5C
#define AXP202_ADC_VBUS_CUR_L_REG       0x5D
#define AXP202_ADC_BATT_VOL_H_REG       0x78
#define AXP202_ADC_BATT_VOL_L_REG       0x79

/* BAT Charge */
#define AXP202_BATT_CHARGE_CURRENT_H_REG 0x7A
#define AXP202_BATT_CHARGE_CURRENT_L_REG 0x7B
#define AXP202_BATT_DISCHARGE_CURRENT_H_REG 0x7C
#define AXP202_BATT_DISCHARGE_CURRENT_L_REG 0x7D

/* ADC enable values */
#define AXP202_ADC_1_BATT_VOL           0x80
#define AXP202_ADC_1_BATT_CUR           0x40
#define AXP202_ADC_1_ACIN_VOL           0x20
#define AXP202_ADC_1_ACIN_CUR           0x10
#define AXP202_ADC_1_VBUS_VOL           0x08
#define AXP202_ADC_1_VBUS_CUR           0x04
#define AXP202_ADC_1_APS_VOL            0x02
#define AXP202_ADC_1_TS_PIN             0x01

#define AXP202_ADC_2_TEMP_MON           0x80
#define AXP202_ADC_2_GPIO0              0x08
#define AXP202_ADC_2_GPIO1              0x04


/* Interrupt values */
#define AXP202_IRQ_1_ACIN_OVER          0x80
#define AXP202_IRQ_1_ACIN_INSERT        0x40
#define AXP202_IRQ_1_ACIN_REMOVE        0x20
#define AXP202_IRQ_1_VBUS_OVER          0x10
#define AXP202_IRQ_1_VBUS_INSERT        0x08
#define AXP202_IRQ_1_VBUS_REMOVE        0x04
#define AXP202_IRQ_1_VBUS_LOWER_VALID   0x02

#define AXP202_IRQ_2_BATT_INSERT        0x80
#define AXP202_IRQ_2_BATT_REMOVE        0x40
#define AXP202_IRQ_2_BATT_ACT           0x20
#define AXP202_IRQ_2_BATT_QUIT_ACT      0x10
#define AXP202_IRQ_2_CHARGING           0x08
#define AXP202_IRQ_2_CHARGE_FINISH      0x04
#define AXP202_IRQ_2_BATT_OVER          0x02
#define AXP202_IRQ_2_BATT_UNDER         0x01

#define AXP202_IRQ_3_INTERNAL_OVER      0x80
#define AXP202_IRQ_3_NOT_ENOUGH_CURR    0x40
#define AXP202_IRQ_3_DCDC2_UNDER        0x10
#define AXP202_IRQ_3_DCDC3_UNDER        0x08
#define AXP202_IRQ_3_LDO3_UNDER         0x04
#define AXP202_IRQ_3_SHORT_KEY          0x02
#define AXP202_IRQ_3_LONG_KEY           0x01

#define AXP202_IRQ_4_N_OE_ON            0x80
#define AXP202_IRQ_4_N_OE_OFF           0x40
#define AXP202_IRQ_4_VBUS_VALID         0x20
#define AXP202_IRQ_4_VBUS_INVLAID       0x10
#define AXP202_IRQ_4_VBUS_SESSION_AB    0x08
#define AXP202_IRQ_4_VBUS_SESSION_END   0x04
#define AXP202_IRQ_4_APS_UNDER_LEV1     0x02
#define AXP202_IRQ_4_APS_UNDER_LEV2     0x01

#define AXP202_IRQ_5_TIMEOUT            0x80
#define AXP202_IRQ_5_PEK_RISING         0x40
#define AXP202_IRQ_5_PEK_FALLING        0x20
#define AXP202_IRQ_5_GPIO3_INPUT        0x08
#define AXP202_IRQ_5_GPIO2_INPUT        0x04
#define AXP202_IRQ_5_GPIO1_INPUT        0x02
#define AXP202_IRQ_5_GPIO0_INPUT        0x01


/* initial setting values */
#define AXP202_ADC1 ( 0 \
		| AXP202_ADC_1_BATT_VOL \
		| AXP202_ADC_1_BATT_CUR \
		| AXP202_ADC_1_ACIN_VOL \
		| AXP202_ADC_1_ACIN_CUR \
		| AXP202_ADC_1_APS_VOL \
		| AXP202_ADC_1_TS_PIN \
		)
#define AXP202_ADC2 ( 0 \
		)

#define AXP202_IRQ1 ( 0 \
		| AXP202_IRQ_1_ACIN_OVER \
		| AXP202_IRQ_1_ACIN_INSERT \
		| AXP202_IRQ_1_ACIN_REMOVE \
		)
#define AXP202_IRQ2 ( 0 \
		| AXP202_IRQ_2_BATT_ACT \
		| AXP202_IRQ_2_BATT_QUIT_ACT \
		| AXP202_IRQ_2_CHARGING \
		| AXP202_IRQ_2_CHARGE_FINISH \
		| AXP202_IRQ_2_BATT_OVER \
		| AXP202_IRQ_2_BATT_UNDER \
		)
#define AXP202_IRQ3 ( 0 \
		| AXP202_IRQ_3_INTERNAL_OVER \
		| AXP202_IRQ_3_NOT_ENOUGH_CURR \
		| AXP202_IRQ_3_DCDC2_UNDER \
		| AXP202_IRQ_3_DCDC3_UNDER \
		| AXP202_IRQ_3_LDO3_UNDER \
		| AXP202_IRQ_3_SHORT_KEY \
		| AXP202_IRQ_3_LONG_KEY \
		)
#define AXP202_IRQ4 ( 0 \
		)
#define AXP202_IRQ5 ( 0 \
		| AXP202_IRQ_5_TIMEOUT \
		)

/* Canging control 1 values */
#define AXP202_CHG_EN               0x80
#define AXP202_CHG_VOL_4_10V        0x00
#define AXP202_CHG_VOL_4_15V        0x20
#define AXP202_CHG_VOL_4_20V        0x40
#define AXP202_CHG_VOL_4_36V        0x60
#define AXP202_CHG_VOL_MASK         0x60
#define AXP202_CHG_OFF_CUR_10PER    0x00
#define AXP202_CHG_OFF_CUR_15PER    0x10
#define AXP202_CHG_OFF_CUR_MASK     0x10

/* DCDC voltage level */
static struct axp202_voltage_t dcdc_voltages[] = {
	{  700000, 0x00 }, {  725000, 0x01 }, {  750000, 0x02 }, {  775000, 0x03 },
	{  800000, 0x04 }, {  825000, 0x05 }, {  850000, 0x06 }, {  875000, 0x07 },
	{  900000, 0x08 }, {  925000, 0x09 }, {  950000, 0x0A }, {  975000, 0x0B },
	{ 1000000, 0x0C }, { 1025000, 0x0D }, { 1050000, 0x0E }, { 1075000, 0x0F }, // 16
	{ 1100000, 0x10 }, { 1125000, 0x11 }, { 1150000, 0x12 }, { 1175000, 0x13 },
	{ 1200000, 0x14 }, { 1225000, 0x15 }, { 1250000, 0x16 }, { 1275000, 0x17 },
	{ 1300000, 0x18 }, { 1325000, 0x19 }, { 1350000, 0x1A }, { 1375000, 0x1B },
	{ 1400000, 0x1C }, { 1425000, 0x1D }, { 1450000, 0x1E }, { 1475000, 0x1F }, // 32
	{ 1500000, 0x20 }, { 1525000, 0x21 }, { 1550000, 0x22 }, { 1575000, 0x23 },
	{ 1600000, 0x24 }, { 1625000, 0x25 }, { 1650000, 0x26 }, { 1675000, 0x27 },
	{ 1700000, 0x28 }, { 1725000, 0x29 }, { 1750000, 0x2A }, { 1775000, 0x2B },
	{ 1800000, 0x2C }, { 1825000, 0x2D }, { 1850000, 0x2E }, { 1875000, 0x2F }, // 48
	{ 1900000, 0x30 }, { 1925000, 0x31 }, { 1950000, 0x32 }, { 1975000, 0x33 },
	{ 2000000, 0x34 }, { 2025000, 0x35 }, { 2050000, 0x36 }, { 2075000, 0x37 },
	{ 2100000, 0x38 }, { 2125000, 0x39 }, { 2150000, 0x3A }, { 2175000, 0x3B },
	{ 2200000, 0x3C }, { 2225000, 0x3D }, { 2250000, 0x3E }, { 2275000, 0x3F }, // 64
	{ 2300000, 0x40 }, { 2325000, 0x41 }, { 2350000, 0x42 }, { 2375000, 0x43 },
	{ 2400000, 0x44 }, { 2425000, 0x45 }, { 2450000, 0x46 }, { 2475000, 0x47 },
	{ 2500000, 0x48 }, { 2525000, 0x49 }, { 2550000, 0x4A }, { 2575000, 0x4B },
	{ 2600000, 0x4C }, { 2625000, 0x4D }, { 2650000, 0x4E }, { 2675000, 0x4F }, // 80
	{ 2700000, 0x50 }, { 2725000, 0x51 }, { 2750000, 0x52 }, { 2775000, 0x53 },
	{ 2800000, 0x54 }, { 2825000, 0x55 }, { 2850000, 0x56 }, { 2875000, 0x57 },
	{ 2900000, 0x58 }, { 2925000, 0x59 }, { 2950000, 0x5A }, { 2975000, 0x5B },
	{ 3000000, 0x5C }, { 3025000, 0x5D }, { 3050000, 0x5E }, { 3075000, 0x5F }, // 96
	{ 3100000, 0x60 }, { 3125000, 0x61 }, { 3150000, 0x62 }, { 3175000, 0x63 },
	{ 3200000, 0x64 }, { 3225000, 0x65 }, { 3250000, 0x66 }, { 3275000, 0x67 },
	{ 3300000, 0x68 }, { 3325000, 0x69 }, { 3350000, 0x6A }, { 3375000, 0x6B },
	{ 3400000, 0x6C }, { 3425000, 0x6D }, { 3450000, 0x6E }, { 3475000, 0x6F }, // 112
	{ 3500000, 0x70 },
}; 
#define NUM_DCDC	ARRAY_SIZE(dcdc_voltages)
#define NUM_DCDC2	64	/* 700mA~2275mA */

static struct axp202_voltage_t ldo2_voltages[] = {
	{ 1800000, 0x00 }, { 1900000, 0x01 }, { 2000000, 0x02 }, { 2100000, 0x03 },
	{ 2200000, 0x04 }, { 2300000, 0x05 }, { 2400000, 0x06 }, { 2500000, 0x07 },
	{ 2600000, 0x08 }, { 2700000, 0x09 }, { 2800000, 0x0A }, { 2900000, 0x0B },
	{ 3000000, 0x0C }, { 3100000, 0x0D }, { 3200000, 0x0E }, { 3300000, 0x0F }, // 16
};
#define NUM_LDO2	ARRAY_SIZE(ldo2_voltages)

#define NUM_LDO3	64	/* 700mA~2275mA */

static struct axp202_voltage_t ldo4_voltages[] = {
	{  700000, 0x00 }, { 1300000, 0x01 }, { 1400000, 0x02 }, { 1500000, 0x03 },
	{ 1600000, 0x04 }, { 1700000, 0x05 }, { 1800000, 0x06 }, { 1900000, 0x07 },
	{ 2000000, 0x08 }, { 2500000, 0x09 }, { 2700000, 0x0A }, { 2800000, 0x0B },
	{ 3000000, 0x0C }, { 3100000, 0x0D }, { 3200000, 0x0E }, { 3300000, 0x0F }, // 16
};
#define NUM_LDO4	ARRAY_SIZE(ldo4_voltages)

#define NUM_LDO5	ARRAY_SIZE(ldo2_voltages)
/********************************************************************
	Variables
********************************************************************/
static struct workqueue_struct *axp202_wq;
static struct i2c_client *axp202_i2c_client = NULL;
static int axp202_acin_det = 0;
static int axp202_charge_sts = 0;
static unsigned int axp202_acin_voltage_read_count = 20;
static unsigned int axp202_suspend_status = 0;

/********************************************************************
	Functions (Global)
********************************************************************/
int axp202_battery_voltage(void)
{
	signed long data[2];
	int ret = 4200;

	if (axp202_i2c_client) {
		data[0] = i2c_smbus_read_byte_data(axp202_i2c_client, AXP202_ADC_BATT_VOL_H_REG);
		data[1] = i2c_smbus_read_byte_data(axp202_i2c_client, AXP202_ADC_BATT_VOL_L_REG);
		ret = ((data[0]<<4)|(data[1]&0xF))&0xFFF;
	}

//	dbg("%s: %dmV\n", __func__, ret);
	return ret;
}

int axp202_acin_detect(void)
{
	signed long data[2];

	if (axp202_acin_voltage_read_count) {
		if (axp202_i2c_client) {
			data[0] = i2c_smbus_read_byte_data(axp202_i2c_client, AXP202_POWER_STS_REG);
			if (data[0]&0x80)	// ACIN detect.
				axp202_acin_det = 1;
			else
				axp202_acin_det = 0;
		}
		axp202_acin_voltage_read_count--;
	}

//	dbg("%s: %d\n", __func__, axp202_acin_det);
	return axp202_acin_det;
}

void axp202_power_off(void)
{
	i2c_smbus_write_byte_data(axp202_i2c_client, AXP202_SHDN_BATT_CHGLED_REG, 0xC6);
}

void axp202_charge_current(unsigned char curr)
{
	unsigned char value;

	if (curr >= AXP202_CHG_CURR_MAX)
		return;

	dbg("AXP202 charge current: %dmA\n", curr*100+300);

	if (axp202_i2c_client) {
		value = AXP202_CHG_EN|AXP202_CHG_VOL_4_20V|AXP202_CHG_OFF_CUR_10PER|(curr&0x0f);
		i2c_smbus_write_byte_data(axp202_i2c_client, AXP202_CHARGING_CTRL1_REG, value);
	}
}

int axp202_charge_status(void)
{
	return axp202_charge_sts;
}

int axp202_get_batt_discharge_current(void)
{
	signed long dischg[2];
	int discharge_cur = 0;
	if (axp202_i2c_client) {
		dischg[0] = i2c_smbus_read_byte_data(axp202_i2c_client, AXP202_BATT_DISCHARGE_CURRENT_H_REG);
		dischg[1] = i2c_smbus_read_byte_data(axp202_i2c_client, AXP202_BATT_DISCHARGE_CURRENT_L_REG);
		discharge_cur = ((dischg[0] << 5) | (dischg[1] & 0x1f))*5/10;
//		dbg("BATT Discharge Current %d\n", discharge_cur);
	}
	return discharge_cur;	

}

int axp202_get_batt_charge_current(void)
{
	signed long chg[2];
	int charge_cur = 0;
	if (axp202_i2c_client) {
		chg[0] = i2c_smbus_read_byte_data(axp202_i2c_client, AXP202_BATT_CHARGE_CURRENT_H_REG);
		chg[1] = i2c_smbus_read_byte_data(axp202_i2c_client, AXP202_BATT_CHARGE_CURRENT_L_REG);
		charge_cur = ((chg[0] << 4) | (chg[1] & 0x0f))*5/10;
//		dbg("BATT Charge Current %d\n", charge_cur);
	}
	return charge_cur;	
}
EXPORT_SYMBOL(axp202_battery_voltage);
EXPORT_SYMBOL(axp202_acin_detect);
EXPORT_SYMBOL(axp202_power_off);
EXPORT_SYMBOL(axp202_charge_current);
EXPORT_SYMBOL(axp202_charge_status);
EXPORT_SYMBOL(axp202_get_batt_discharge_current);
EXPORT_SYMBOL(axp202_get_batt_charge_current);

/*******************************************
	Functions (Internal)
*******************************************/
static void axp202_work_func(struct work_struct *work)
{
	struct axp202_data* axp202 = container_of(work, struct axp202_data, work);
	signed long data[5];
	
	dbg("%s\n", __func__);

	data[0] =0;
	data[1] =0;
	data[2] =0;
	data[3] =0;
	data[4] =0;
	
	data[0] = i2c_smbus_read_byte_data(axp202->client, AXP202_IRQ_STS1_REG);
	data[1] = i2c_smbus_read_byte_data(axp202->client, AXP202_IRQ_STS2_REG);
	data[2] = i2c_smbus_read_byte_data(axp202->client, AXP202_IRQ_STS3_REG);
	data[3] = i2c_smbus_read_byte_data(axp202->client, AXP202_IRQ_STS4_REG);
	data[4] = i2c_smbus_read_byte_data(axp202->client, AXP202_IRQ_STS5_REG);

	i2c_smbus_write_byte_data(axp202->client, AXP202_IRQ_STS1_REG, 0xFF);
	i2c_smbus_write_byte_data(axp202->client, AXP202_IRQ_STS2_REG, 0xFF);
	i2c_smbus_write_byte_data(axp202->client, AXP202_IRQ_STS3_REG, 0xFF);
	i2c_smbus_write_byte_data(axp202->client, AXP202_IRQ_STS4_REG, 0xFF);
	i2c_smbus_write_byte_data(axp202->client, AXP202_IRQ_STS5_REG, 0xFF);

	if (data[0]&AXP202_IRQ_1_ACIN_OVER) {
		dbg("ACIN over-voltage, IRQ status\n");
	}
	if (data[0]&AXP202_IRQ_1_ACIN_INSERT) {
		dbg("ACIN connected, IRQ status\n");
		axp202_acin_det = 1;
	}
	if (data[0]&AXP202_IRQ_1_ACIN_REMOVE) {
		dbg("ACIN removed, IRQ status\n");
		axp202_acin_det = 0;
		axp202_charge_sts = AXP202_CHG_NONE;
	}
	if (data[0]&AXP202_IRQ_1_VBUS_OVER) {
		dbg("VBUS over-voltage, IRQ status\n");
	}
	if (data[0]&AXP202_IRQ_1_VBUS_INSERT) {
		dbg("VBUS connected, IRQ status\n");
	}
	if (data[0]&AXP202_IRQ_1_VBUS_REMOVE) {
		dbg("VBUS removed, IRQ status\n");
	}
	if (data[0]&AXP202_IRQ_1_VBUS_LOWER_VALID) {
		dbg("VBUS is available, but lower then Vhold, IRQ status\n");
	}
	if (data[1]&AXP202_IRQ_2_BATT_INSERT) {
		dbg("Battery connected, IRQ status\n");
	}
	if (data[1]&AXP202_IRQ_2_BATT_REMOVE) {
		dbg("Battery removed, IRQ status\n");
	}
	if (data[1]&AXP202_IRQ_2_BATT_ACT) {
		dbg("Battery activate mode, IRQ status\n");
	}
	if (data[1]&AXP202_IRQ_2_BATT_QUIT_ACT) {
		dbg("Exit battery activate mode, IRQ status\n");
	}
	if (data[1]&AXP202_IRQ_2_CHARGING) {
		dbg("Be charging, IRQ status\n");
		axp202_charge_sts = AXP202_CHG_ING;
	}
	if (data[1]&AXP202_IRQ_2_CHARGE_FINISH) {
		dbg("Carge finished, IRQ status\n");
		axp202_charge_sts = AXP202_CHG_OK;
	}
	if (data[1]&AXP202_IRQ_2_BATT_OVER) {
		dbg("Battery over-temperature, IRQ status\n");
	}
	if (data[1]&AXP202_IRQ_2_BATT_UNDER) {
		dbg("Battery low-temperature, IRQ status\n");
	}
	if (data[2]&AXP202_IRQ_3_INTERNAL_OVER) {
		dbg("AXP202 internal over-temperature, IRQ status\n");
	}
	if (data[2]&AXP202_IRQ_3_NOT_ENOUGH_CURR) {
		dbg("Charge current is lower than the set current, IRQ status\n");
	}
	if (data[2]&AXP202_IRQ_3_DCDC2_UNDER) {
		dbg("DC-DC2 output voltage is lower than the set value, IRQ status\n");
	}
	if (data[2]&AXP202_IRQ_3_DCDC3_UNDER) {
		dbg("DC-DC3 output voltage is lower than the set value, IRQ status\n");
	}
	if (data[2]&AXP202_IRQ_3_LDO3_UNDER) {
		dbg("LDO2 output voltage is lower than the set value, IRQ status\n");
	}
	if (data[2]&AXP202_IRQ_3_SHORT_KEY) {
#if defined(CONFIG_REGULATOR_AXP202_PEK)
        del_timer(&axp202->timer);
        //input_report_key(axp202->input_dev, KEY_POWER, 1);//CONFIG_COBY_MID9125
		input_event(axp202->input_dev, EV_KEY, KEY_POWER, 1);
		input_event(axp202->input_dev, EV_SYN, 0, 0);
		axp202->timer.expires = jiffies + msecs_to_jiffies(100);
		add_timer(&axp202->timer);
#endif
		dbg("PEK short press, IRQ status\n");
	}
	if (data[2]&AXP202_IRQ_3_LONG_KEY) {
#if defined(CONFIG_REGULATOR_AXP202_PEK)
        del_timer(&axp202->timer);
        //input_report_key(axp202->input_dev, KEY_POWER, 1);//CONFIG_COBY_MID9125
		input_event(axp202->input_dev, EV_KEY, KEY_POWER, 1);
		input_event(axp202->input_dev, EV_SYN, 0, 0);
		axp202->timer.expires = jiffies + msecs_to_jiffies(1000);
		add_timer(&axp202->timer);
#endif
		dbg("PEK long press, IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_N_OE_ON) {
		dbg("N_OE startup, IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_N_OE_OFF) {
		dbg("N_OE shutdown, IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_VBUS_VALID) {
		dbg("VBUS valid, IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_VBUS_INVLAID) {
		dbg("VBUS invalid, IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_VBUS_SESSION_AB) {
		dbg("VBUS Session A/B, IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_VBUS_SESSION_END) {
		dbg("VBUS Session End IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_APS_UNDER_LEV1) {
		dbg("APS low-voltage(LEVEL1), IRQ status\n");
	}
	if (data[3]&AXP202_IRQ_4_APS_UNDER_LEV2) {
		dbg("APS low-voltage(LEVEL2), IRQ status\n");
	}
	if (data[4]&AXP202_IRQ_5_TIMEOUT) {
		dbg("Timer timeout, IRQ status\n");
	}
	if (data[4]&AXP202_IRQ_5_PEK_RISING) {
		dbg("PEK press rising edge, IRQ status\n");
	}
	if (data[4]&AXP202_IRQ_5_PEK_FALLING) {
		dbg("PEK press falling edge, IRQ status\n");
	}
	if (data[4]&AXP202_IRQ_5_GPIO3_INPUT) {
		dbg("GPIO3 input edge trigger, IRQ status\n");
	}
	if (data[4]&AXP202_IRQ_5_GPIO2_INPUT) {
		dbg("GPIO2 input edge trigger, IRQ status\n");
	}
	if (data[4]&AXP202_IRQ_5_GPIO1_INPUT) {
		dbg("GPIO1 input edge trigger or ADC input, IRQ status\n");
	}
	if (data[4]&AXP202_IRQ_5_GPIO0_INPUT) {
		dbg("GPIO0 input edge trigger, IRQ status\n");
	}

	enable_irq(axp202->client->irq);
}

static irqreturn_t axp202_interrupt(int irqno, void *param)
{
	struct axp202_data *axp202 = (struct axp202_data *)param;
	dbg("%s\n", __func__);

	disable_irq_nosync(axp202->client->irq);
	queue_work(axp202_wq, &axp202->work);
	return IRQ_HANDLED;
}

#if defined(CONFIG_REGULATOR_AXP202_PEK)
static void axp202_timer_func(unsigned long data)
{
	struct axp202_data *axp202 = (struct axp202_data *)data;
	dbg("%s\n", __func__);

    //input_report_key(axp202->input_dev, KEY_POWER, 0);//CONFIG_COBY_MID9125
	input_event(axp202->input_dev, EV_KEY, KEY_POWER, 0);
	input_event(axp202->input_dev, EV_SYN, 0, 0);
}
#endif

static int axp202_dcdc_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV, unsigned *selector)
{
	struct axp202_data* axp202 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value = 0;
	int i, max_num = 0, ret;

	if(axp202_suspend_status)
		return -EBUSY;

	switch (id) {
		case AXP202_ID_DCDC2:
			reg = AXP202_DCDC2_REG;
			max_num = NUM_DCDC2;
			break;
		case AXP202_ID_DCDC3:
			reg = AXP202_DCDC3_REG;
			max_num = NUM_DCDC;
			break;
		default:
			return -EINVAL;
	}

	for (i = 0; i < max_num; i++) {
		if (dcdc_voltages[i].uV >= min_uV) {
			value = dcdc_voltages[i].val;
			break;
		}
	}

	if (i == max_num)
		return -EINVAL;

	dbg("%s: reg:0x%x value:%dmV\n", __func__, reg, dcdc_voltages[i].uV/1000);

	ret = i2c_smbus_write_byte_data(axp202->client, reg, value);
	udelay(50);

	return ret;
}

static int axp202_dcdc_get_voltage(struct regulator_dev *rdev)
{
	struct axp202_data* axp202 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg;
	int ret, i, max_num, voltage = 0;

	switch (id) {
		case AXP202_ID_DCDC2:
			reg = AXP202_DCDC2_REG;
			max_num = NUM_DCDC2;
			break;
		case AXP202_ID_DCDC3:
			reg = AXP202_DCDC3_REG;
			max_num = NUM_DCDC;
			break;
		default:
			return -EINVAL;
	}

	ret = i2c_smbus_read_byte_data(axp202->client, reg);
	if (ret < 0)
		return -EINVAL;

	ret &= 0xFF;
	for (i = 0; i < max_num; i++) {
		if (dcdc_voltages[i].val == ret) {
			voltage = dcdc_voltages[i].uV;
			break;
		}
	}

	if (i == max_num)
		return -EINVAL;

	dbg("%s: reg:0x%x value:%dmV\n", __func__, reg, voltage/1000);
	return voltage;
}

static struct regulator_ops axp202_dcdc_ops = {
	.set_voltage = axp202_dcdc_set_voltage,
	.get_voltage = axp202_dcdc_get_voltage,
};

static int axp202_ldo_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV, unsigned *selector)
{
	struct axp202_data* axp202 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, old_value, value;
	int i;

	switch (id) {
		case AXP202_ID_LDO2:
			reg = AXP202_LDO24_REG;
			old_value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			for (i = 0; i < NUM_LDO2; i++) {
				if (ldo2_voltages[i].uV >= min_uV) {
					value = ldo2_voltages[i].val;
					break;
				}
			}
			if (i == NUM_LDO2)
				return -EINVAL;
			value = (old_value&(0xFF&(~(0xF<<4)))) | (value<<4);
			dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, ldo2_voltages[i].uV/1000);
			break;
		case AXP202_ID_LDO3:
			reg = AXP202_LDO3_REG;
			for (i = 0; i < NUM_LDO3; i++) {
				if (dcdc_voltages[i].uV >= min_uV) {
					value = dcdc_voltages[i].val;
					break;
				}
			}
			if (i == NUM_LDO3)
				return -EINVAL;
			dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, dcdc_voltages[i].uV/1000);
			break;
		case AXP202_ID_LDO4:
			reg = AXP202_LDO24_REG;
			old_value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			for (i = 0; i < NUM_LDO4; i++) {
				if (ldo4_voltages[i].uV >= min_uV) {
					value = ldo4_voltages[i].val;
					break;
				}
			}
			if (i == NUM_LDO4)
				return -EINVAL;
			value = (old_value&(0xFF&(~(0xF)))) | value;
			dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, ldo4_voltages[i].uV/1000);
			break;
		case AXP202_ID_LDO5:
			reg = AXP202_LDO5_REG;
			old_value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			for (i = 0; i < NUM_LDO5; i++) {
				if (ldo2_voltages[i].uV >= min_uV) {
					value = ldo2_voltages[i].val;
					break;
				}
			}
			if (i == NUM_LDO5)
				return -EINVAL;
			value = (old_value&(0xFF&(~(0xF<<4)))) | (value<<4);
			dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, ldo2_voltages[i].uV/1000);
			break;
		default:
			return -EINVAL;
	}

	return i2c_smbus_write_byte_data(axp202->client, reg, value);
}

static int axp202_ldo_get_voltage(struct regulator_dev *rdev)
{
	struct axp202_data* axp202 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg;
	int ret, i, voltage = 0;

	switch (id) {
		case AXP202_ID_LDO2:
			reg = AXP202_LDO24_REG;
			ret = i2c_smbus_read_byte_data(axp202->client, reg);
			if (ret < 0)
				return -EINVAL;

			ret = (ret>>4)&0xF;
			for (i = 0; i < NUM_LDO2; i++) {
				if (ldo2_voltages[i].val == ret) {
					voltage = ldo2_voltages[i].uV;
					break;
				}
			}

			if (i == NUM_LDO2)
				return -EINVAL;
			break;
		case AXP202_ID_LDO3:
			reg = AXP202_LDO3_REG;
			ret = i2c_smbus_read_byte_data(axp202->client, reg);
			if (ret < 0)
				return -EINVAL;
			ret &= 0x7F;
			for (i = 0; i < NUM_LDO3; i++) {
				if (dcdc_voltages[i].val == ret) {
					voltage = dcdc_voltages[i].uV;
					break;
				}
			}
			if (i == NUM_LDO3)
				return -EINVAL;
			break;
		case AXP202_ID_LDO4:
			reg = AXP202_LDO24_REG;
			ret = i2c_smbus_read_byte_data(axp202->client, reg);
			if (ret < 0)
				return -EINVAL;
			ret &= 0xF;
			for (i = 0; i < NUM_LDO4; i++) {
				if (ldo4_voltages[i].val == ret) {
					voltage = ldo4_voltages[i].uV;
					break;
				}
			}
			if (i == NUM_LDO4)
				return -EINVAL;
			break;
		case AXP202_ID_LDO5:
			reg = AXP202_LDO5_REG;
			ret = i2c_smbus_read_byte_data(axp202->client, reg);
			if (ret < 0)
				return -EINVAL;
			ret &= (ret>>4)&0xF;
			for (i = 0; i < NUM_LDO5; i++) {
				if (ldo2_voltages[i].val == ret) {
					voltage = ldo2_voltages[i].uV;
					break;
				}
			}
			if (i == NUM_LDO5)
				return -EINVAL;
			break;
		default:
			return -EINVAL;
	}

	dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, voltage/1000);
	return voltage;
}

static int axp202_ldo_enable(struct regulator_dev *rdev)
{
	struct axp202_data* axp202 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;

	switch (id) {
		case AXP202_ID_LDO2:
			reg = AXP202_POWER_OUT_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value |= 0x04;
			break;
		case AXP202_ID_LDO3:
			reg = AXP202_POWER_OUT_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value |= 0x40;
			break;
		case AXP202_ID_LDO4:
			reg = AXP202_POWER_OUT_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value |= 0x08;
			break;
		case AXP202_ID_LDO5:
			reg = AXP202_LDO5_FUNC_SET_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value &= ~0x07;
			value |= 0x03;
			break;
		default:
			return -EINVAL;
	}

	dbg("%s: id:%d\n", __func__, id);
	return i2c_smbus_write_byte_data(axp202->client, reg, value);
}

static int axp202_ldo_disable(struct regulator_dev *rdev)
{
	struct axp202_data* axp202 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;

	switch (id) {
		case AXP202_ID_LDO2:
			reg = AXP202_POWER_OUT_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value &= ~0x04;
			break;
		case AXP202_ID_LDO3:
			reg = AXP202_POWER_OUT_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value &= ~0x40;
			break;
		case AXP202_ID_LDO4:
			reg = AXP202_POWER_OUT_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value &= ~0x08;
			break;
		case AXP202_ID_LDO5:
			reg = AXP202_LDO5_FUNC_SET_REG;
			value = (u8)i2c_smbus_read_byte_data(axp202->client, reg);
			value &= ~0x07;
			break;
		default:
			return -EINVAL;
	}

	dbg("%s: id:%d\n", __func__, id);
	return i2c_smbus_write_byte_data(axp202->client, reg, value);
}

static int axp202_ldo_is_enabled(struct regulator_dev *rdev)
{
	return 0;
}

static struct regulator_ops axp202_ldo_ops = {
	.set_voltage = axp202_ldo_set_voltage,
	.get_voltage = axp202_ldo_get_voltage,
	.enable	  = axp202_ldo_enable,
	.disable	 = axp202_ldo_disable,
	.is_enabled  = axp202_ldo_is_enabled,
};

static struct regulator_desc axp202_reg[] = {
	{
		.name = "dcdc2",
		.id = AXP202_ID_DCDC2,
		.ops = &axp202_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC2 + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "dcdc3",
		.id = AXP202_ID_DCDC3,
		.ops = &axp202_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo2",
		.id = AXP202_ID_LDO2,
		.ops = &axp202_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO2 + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo3",
		.id = AXP202_ID_LDO3,
		.ops = &axp202_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO3 + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo4",
		.id = AXP202_ID_LDO4,
		.ops = &axp202_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO4 + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo5",
		.id = AXP202_ID_LDO5,
		.ops = &axp202_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO5 + 1,
		.owner = THIS_MODULE,
	},
};
#define NUM_OUPUT	ARRAY_SIZE(axp202_reg)

static int axp202_pmic_probe(struct i2c_client *client, const struct i2c_device_id *i2c_id)
{
	struct regulator_dev **rdev;
	struct axp202_platform_data *pdata = client->dev.platform_data;
	struct axp202_data *axp202;
	int i, id, ret = -ENOMEM;
	unsigned int power_status;

	axp202 = kzalloc(sizeof(struct axp202_data) +
			sizeof(struct regulator_dev *) * (NUM_OUPUT + 1),
			GFP_KERNEL);
	if (!axp202)
		goto out;

	axp202->client = client;
	axp202_i2c_client = client;

	axp202->min_uV = AXP202_MIN_UV;
	axp202->max_uV = AXP202_MAX_UV;

	rdev = axp202->rdev;

	i2c_set_clientdata(client, rdev);

	power_status = i2c_smbus_read_byte_data(client, 0x00);
	if ((power_status&0xFF) == 0xFF) {
		ret = -ENODEV;
		goto err_nodev;
	} else {
		printk("######## %s: %x ########\n", __func__, power_status);
	}

	INIT_WORK(&axp202->work, axp202_work_func);

	for (i = 0; i < pdata->num_subdevs && i <= NUM_OUPUT; i++) {
		id = pdata->subdevs[i].id;
		if (!pdata->subdevs[i].platform_data) {
			rdev[i] = NULL;
			continue;
		}
		if (id >= AXP202_ID_MAX) {
			dev_err(&client->dev, "invalid regulator id %d\n", id);
			goto err;
		}
		rdev[i] = regulator_register(&axp202_reg[id], &client->dev,
						 pdata->subdevs[i].platform_data,
						 axp202);
		if (IS_ERR(rdev[i])) {
			ret = PTR_ERR(rdev[i]);
			dev_err(&client->dev, "failed to register %s\n",
				axp202_reg[id].name);
			goto err;
		}
	}

	i2c_smbus_write_byte_data(client, AXP202_ADC_ENABLE1_REG, AXP202_ADC1);
	i2c_smbus_write_byte_data(client, AXP202_ADC_ENABLE2_REG, AXP202_ADC2);
	i2c_smbus_write_byte_data(client, AXP202_ADC_TSPIN_REG, 0x36);

	/* GPIO0 set to Low output for using LDO5 */
	i2c_smbus_write_byte_data(client, AXP202_LDO5_FUNC_SET_REG, 0x00);	// Low output
	i2c_smbus_write_byte_data(client, AXP202_LDO5_REG, 0xD5);			// 3.3V

	/* long time key press time set to 1S, auto power off function disable */
	i2c_smbus_write_byte_data(client, AXP202_PEK_REG, 0xC5);	// start-up:2s, long-press:1s, autooff:disable

	power_status = i2c_smbus_read_byte_data(client, AXP202_POWER_OFF_REG);
	i2c_smbus_write_byte_data(client, AXP202_POWER_OFF_REG, (power_status&0xFC)|0x00 );		// requested by H/W

	if (pdata->init_port) {
		pdata->init_port(client->irq);
	}

	if (client->irq) {
		/* irq enable */
		i2c_smbus_write_byte_data(client, AXP202_IRQ_EN1_REG, AXP202_IRQ1);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_EN2_REG, AXP202_IRQ2);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_EN3_REG, AXP202_IRQ3);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_EN4_REG, AXP202_IRQ4);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_EN5_REG, AXP202_IRQ5);

		/* clear irq status */
		i2c_smbus_write_byte_data(client, AXP202_IRQ_STS1_REG, 0xFF);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_STS2_REG, 0xFF);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_STS3_REG, 0xFF);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_STS4_REG, 0xFF);
		i2c_smbus_write_byte_data(client, AXP202_IRQ_STS5_REG, 0xFF);

		if (request_irq(client->irq, axp202_interrupt, IRQ_TYPE_EDGE_FALLING|IRQF_DISABLED, "axp202_irq", axp202)) {
			dev_err(&client->dev, "could not allocate IRQ_NO(%d) !\n", client->irq);
			ret = -EBUSY;
			goto err;
		}
	}

#if defined(CONFIG_REGULATOR_AXP202_PEK)
	// register input device for power key.
	axp202->input_dev = input_allocate_device();
	if (axp202->input_dev == NULL) {
		ret = -ENOMEM;
		dev_err(&client->dev, "%s: Failed to allocate input device\n", __func__);
		goto err_input_dev_alloc_failed;
	}

	axp202->input_dev->evbit[0] = BIT(EV_KEY);
	axp202->input_dev->name = "axp202 power-key";
	set_bit(KEY_POWER & KEY_MAX, axp202->input_dev->keybit);
	ret = input_register_device(axp202->input_dev);
	if (ret) {
		dev_err(&client->dev, "%s: Unable to register %s input device\n", __func__, axp202->input_dev->name);
		goto err_input_register_device_failed;
	}

	init_timer(&axp202->timer);
	axp202->timer.data = (unsigned long)axp202;
	axp202->timer.function = axp202_timer_func;
#endif

	dev_info(&client->dev, "AXP202 regulator driver loaded\n");
	return 0;

#if defined(CONFIG_REGULATOR_AXP202_PEK)
err_input_register_device_failed:
	input_free_device(axp202->input_dev);
err_input_dev_alloc_failed:
#endif
err:
	while (--i >= 0)
		regulator_unregister(rdev[i]);
err_nodev:
	kfree(axp202);
out:
	axp202_i2c_client = NULL;
	return ret;
}

static int axp202_pmic_remove(struct i2c_client *client)
{
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct axp202_data* axp202 = NULL;
	int i;

	for (i=0 ; (rdev[i] != NULL) && (i<AXP202_ID_MAX) ; i++)
		axp202 = rdev_get_drvdata(rdev[i]);

#if defined(CONFIG_REGULATOR_AXP202_PEK)
	if (axp202)
		del_timer(&axp202->timer);
#endif

	for (i = 0; i <= NUM_OUPUT; i++)
		if (rdev[i])
			regulator_unregister(rdev[i]);
	kfree(rdev);
	i2c_set_clientdata(client, NULL);
	axp202_i2c_client = NULL;

	return 0;
}

static int axp202_pmic_suspend(struct i2c_client *client, pm_message_t mesg)
{
	int i, ret = 0;
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct axp202_data* axp202 = NULL;

	for (i=0 ; (rdev[i] != NULL) && (i<AXP202_ID_MAX) ; i++)
		axp202 = rdev_get_drvdata(rdev[i]);

	if (client->irq)
		disable_irq(client->irq);

	/* clear irq status */
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS1_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS2_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS3_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS4_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS5_REG, 0xFF);

	if (axp202) {
		ret = cancel_work_sync(&axp202->work);
		flush_workqueue(axp202_wq);
	}

	if (ret && client->irq)
		enable_irq(client->irq);

	axp202_suspend_status = 1;

	return 0;
}

static int axp202_pmic_resume(struct i2c_client *client)
{
	int i;
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct axp202_data* axp202 = NULL;

	for (i=0 ; (rdev[i] != NULL) && (i<AXP202_ID_MAX) ; i++)
		axp202 = rdev_get_drvdata(rdev[i]);

	#if defined(CONFIG_REGULATOR_AXP202_PEK)
	if (axp202)
		queue_work(axp202_wq, &axp202->work);
	#else
	if (client->irq)
		enable_irq(client->irq);
	/* clear irq status */
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS1_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS2_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS3_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS4_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP202_IRQ_STS5_REG, 0xFF);
	#endif

	axp202_suspend_status = 0;
	return 0;
}

static const struct i2c_device_id axp202_id[] = {
	{ "axp202", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, axp202_id);

static struct i2c_driver axp202_pmic_driver = {
	.probe	  = axp202_pmic_probe,
	.remove	 = axp202_pmic_remove,
	.suspend	= axp202_pmic_suspend,
	.resume	 = axp202_pmic_resume,
	.driver	 = {
		.name   = "axp202",
	},
	.id_table   = axp202_id,
};

static int __init axp202_pmic_init(void)
{
	axp202_wq = create_singlethread_workqueue("axp202_wq");
	if (!axp202_wq)
		return -ENOMEM;

	return i2c_add_driver(&axp202_pmic_driver);
}
subsys_initcall(axp202_pmic_init);

static void __exit axp202_pmic_exit(void)
{
	i2c_del_driver(&axp202_pmic_driver);
	if (axp202_wq)
		destroy_workqueue(axp202_wq);}
module_exit(axp202_pmic_exit);

/* Module information */
MODULE_DESCRIPTION("AXP202 regulator driver");
MODULE_AUTHOR("Telechips");
MODULE_LICENSE("GPL");
