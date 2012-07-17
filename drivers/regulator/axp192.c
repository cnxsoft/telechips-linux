/*
 * axp192.c  --  Voltage and current regulation for the AXP192
 *
 * Copyright (C) 2011 by Telechips
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

#include <linux/regulator/axp192.h>

#if defined(DEBUG)
#define dbg(msg...) printk("AXP192: " msg)
#else
#define dbg(msg...)
#endif

#define AXP192_MIN_UV   700000
#define AXP192_MAX_UV  3500000


/********************************************************************
	Define Types
********************************************************************/
struct axp192_voltage_t {
	int uV;
	u8  val;
};

struct axp192_data {
	struct i2c_client *client;
	unsigned int min_uV;
	unsigned int max_uV;
	struct work_struct work;
#if defined(CONFIG_REGULATOR_AXP192_PEK)
	struct input_dev *input_dev;
	struct timer_list timer;
#endif
	struct regulator_dev *rdev[0];
};


/********************************************************************
	I2C Command & Values
********************************************************************/
/* Registers */
#define AXP192_POWER_STS_REG            0x00
#define AXP192_MODE_CHARGING_STS_REG    0x01
#define AXP192_OTG_VBUS_STS_REG         0x04
#define AXP192_EXTEN_DCDC2_CTRL_REG     0x10
#define AXP192_DCDC13_LDO23_CTRL_REG    0x12
#define AXP192_DCDC2_REG                0x23
#define AXP192_DCDC1_REG                0x26
#define AXP192_DCDC3_REG                0x27
#define AXP192_LDO23_REG                0x28
#define AXP192_VBUS_IPSOUT_REG          0x30
#define AXP192_POWER_OFF_VOL_REG        0x31
#define AXP192_POWER_OFF_REG            0x32
#define AXP192_CHARGING_CTRL1_REG       0x33
#define AXP192_CHARGING_CTRL2_REG       0x34
#define AXP192_BACKUP_BATT_CHG_REG      0x35
#define AXP192_PEK_REG                  0x36

#define AXP192_ADC_ENABLE1_REG          0x82
#define AXP192_ADC_ENABLE2_REG          0x83
#define AXP192_ADC_TSPIN_REG            0x84

#define AXP192_IRQ_EN1_REG              0x40
#define AXP192_IRQ_EN2_REG              0x41
#define AXP192_IRQ_EN3_REG              0x42
#define AXP192_IRQ_EN4_REG              0x43
#define AXP192_IRQ_STS1_REG             0x44
#define AXP192_IRQ_STS2_REG             0x45
#define AXP192_IRQ_STS3_REG             0x46
#define AXP192_IRQ_STS4_REG             0x47

#define AXP192_ADC_ACIN_VOL_H_REG       0x56
#define AXP192_ADC_ACIN_VOL_L_REG       0x57
#define AXP192_ADC_VBUS_VOL_H_REG       0x5A
#define AXP192_ADC_VBUS_VOL_L_REG       0x5B
#define AXP192_ADC_BATT_VOL_H_REG       0x78
#define AXP192_ADC_BATT_VOL_L_REG       0x79
/* BAT Charge */
#define AXP192_BATT_CHARGE_CURRENT_H_REG 0x7A
#define AXP192_BATT_CHARGE_CURRENT_L_REG 0x7B
#define AXP192_BATT_DISCHARGE_CURRENT_H_REG 0x7C
#define AXP192_BATT_DISCHARGE_CURRENT_L_REG 0x7D

#define AXP192_LDO4_FUNC_SET_REG        0x90
#define AXP192_LDO4_VOLTAGE_REG         0x91


/* Power off voltage values */
#define AXP192_PWROFF_VOL_2_6V          0x00
#define AXP192_PWROFF_VOL_2_7V          0x01
#define AXP192_PWROFF_VOL_2_8V          0x02
#define AXP192_PWROFF_VOL_2_9V          0x03
#define AXP192_PWROFF_VOL_3_0V          0x04
#define AXP192_PWROFF_VOL_3_1V          0x05
#define AXP192_PWROFF_VOL_3_2V          0x06
#define AXP192_PWROFF_VOL_3_3V          0x07
#define AXP192_PWROFF_VOL_MASK          0x07

/* Power off values */
#define AXP192_PWROFF_EN                0x80
#define AXP192_PWROFF_BATT_MON_EN       0x40
#define AXP192_PWROFF_BATT_MON_DIS      0x00
#define AXP192_PWROFF_CHGLED_HIGHZ      0x00
#define AXP192_PWROFF_CHGLED_1Hz        0x10
#define AXP192_PWROFF_CHGLED_4Hz        0x20
#define AXP192_PWROFF_CHGLED_OUTLOW     0x30
#define AXP192_PWROFF_CHGLED_MASK       0x30
#define AXP192_PWROFF_CHGLED_CTRL_CHG   0x00
#define AXP192_PWROFF_CHGLED_CTRL_REG   0x08
#define AXP192_PWROFF_N_OE_DLY_0_5S     0x00
#define AXP192_PWROFF_N_OE_DLY_1_0S     0x01
#define AXP192_PWROFF_N_OE_DLY_2_0S     0x02
#define AXP192_PWROFF_N_OE_DLY_3_0S     0x03
#define AXP192_PWROFF_N_OE_DLY_MASK     0x03
#define AXP192_PWROFF_VALUES ( 0 \
		| AXP192_PWROFF_BATT_MON_EN \
		| AXP192_PWROFF_CHGLED_HIGHZ \
		| AXP192_PWROFF_CHGLED_CTRL_CHG \
		| AXP192_PWROFF_N_OE_DLY_0_5S \
		)

/* Changing control 1 values */
#define AXP192_CHG_EN                   0x80
#define AXP192_CHG_VOL_4_10V            0x00
#define AXP192_CHG_VOL_4_15V            0x20
#define AXP192_CHG_VOL_4_20V            0x40
#define AXP192_CHG_VOL_4_36V            0x60
#define AXP192_CHG_VOL_MASK             0x60
#define AXP192_CHG_OFF_CUR_10PER        0x00
#define AXP192_CHG_OFF_CUR_15PER        0x10
#define AXP192_CHG_OFF_CUR_MASK         0x10

/* PEK press key parameter set vaules */
#define AXP192_PEK_PWRON_128mS          0x00
#define AXP192_PEK_PWRON_256mS          0x40
#define AXP192_PEK_PWRON_512mS          0x80
#define AXP192_PEK_PWRON_1S             0xC0
#define AXP192_PEK_PWRON_MASK           0xC0
#define AXP192_PEK_LONGKEY_1_0S         0x00
#define AXP192_PEK_LONGKEY_1_5S         0x10
#define AXP192_PEK_LONGKEY_2_0S         0x20
#define AXP192_PEK_LONGKEY_2_5S         0x30
#define AXP192_PEK_LONGKEY_MASK         0x30
#define AXP192_PEK_LONGKEY_PWROFF_EN    0x08
#define AXP192_PEK_LONGKEY_PWROFF_DIS   0x00
#define AXP192_PEK_PWRON_DLY_32mS       0x00
#define AXP192_PEK_PWRON_DLY_64mS       0x04
#define AXP192_PEK_PWRON_DLY_MASK       0x04
#define AXP192_PEK_PWROFF_TIME_4S       0x00
#define AXP192_PEK_PWROFF_TIME_6S       0x01
#define AXP192_PEK_PWROFF_TIME_8S       0x02
#define AXP192_PEK_PWROFF_TIME_12S      0x03
#define AXP192_PEK_PWROFF_TIME_MASK     0x03
#define AXP192_PEK_VALUES ( 0 \
		| AXP192_PEK_PWRON_1S \
		| AXP192_PEK_LONGKEY_1_0S \
		| AXP192_PEK_LONGKEY_PWROFF_DIS \
		| AXP192_PEK_PWRON_DLY_64mS \
		| AXP192_PEK_PWROFF_TIME_12S \
		)

/* ADC enable values*/
#define AXP192_ADC_1_BATT_VOL           0x80
#define AXP192_ADC_1_BATT_CUR           0x40
#define AXP192_ADC_1_ACIN_VOL           0x20
#define AXP192_ADC_1_ACIN_CUR           0x10
#define AXP192_ADC_1_VBUS_VOL           0x08
#define AXP192_ADC_1_VBUS_CUR           0x04
#define AXP192_ADC_1_APS_VOL            0x02
#define AXP192_ADC_1_TS_PIN             0x01

#define AXP192_ADC_2_TEMP_MON           0x80
#define AXP192_ADC_2_GPIO0              0x08
#define AXP192_ADC_2_GPIO1              0x04
#define AXP192_ADC_2_GPIO2              0x02
#define AXP192_ADC_2_GPIO3              0x01

#define AXP192_ADC_TS_SAMPLE_25         0x00
#define AXP192_ADC_TS_SAMPLE_50         0x40
#define AXP192_ADC_TS_SAMPLE_100        0x80
#define AXP192_ADC_TS_SAMPLE_200        0xC0
#define AXP192_ADC_TS_SAMPLE_MASK       0xC0
#define AXP192_ADC_TS_OUT_CUR_20uA      0x00
#define AXP192_ADC_TS_OUT_CUR_40uA      0x10
#define AXP192_ADC_TS_OUT_CUR_60uA      0x20
#define AXP192_ADC_TS_OUT_CUR_80uA      0x30
#define AXP192_ADC_TS_OUT_CUR_MASK      0x30
#define AXP192_ADC_TS_PIN_TEMP_MON      0x00
#define AXP192_ADC_TS_PIN_EXTERN_ADC    0x04
#define AXP192_ADC_TS_PIN_OUT_DIS       0x00
#define AXP192_ADC_TS_PIN_OUT_CHG       0x01
#define AXP192_ADC_TS_PIN_OUT_SAVE_ENG  0x20
#define AXP192_ADC_TS_PIN_OUT_ALWAYS    0x30
#define AXP192_ADC_TS_PIN_OUT_MASK      0x30

#define AXP192_ADC1 ( 0 \
		| AXP192_ADC_1_BATT_VOL \
		| AXP192_ADC_1_BATT_CUR \
		)
#define AXP192_ADC2 ( 0 \
		)
#define AXP192_ADC_TS ( 0 \
		| AXP192_ADC_TS_SAMPLE_25 \
		| AXP192_ADC_TS_OUT_CUR_20uA \
		| AXP192_ADC_TS_PIN_EXTERN_ADC \
		| AXP192_ADC_TS_PIN_OUT_DIS \
		)

/* Interrupt values */
#define AXP192_IRQ_1_ACIN_OVER          0x80
#define AXP192_IRQ_1_ACIN_INSERT        0x40
#define AXP192_IRQ_1_ACIN_REMOVE        0x20
#define AXP192_IRQ_1_VBUS_OVER          0x10
#define AXP192_IRQ_1_VBUS_INSERT        0x08
#define AXP192_IRQ_1_VBUS_REMOVE        0x04
#define AXP192_IRQ_1_VBUS_LOWER_VALID   0x02

#define AXP192_IRQ_2_BATT_INSERT        0x80
#define AXP192_IRQ_2_BATT_REMOVE        0x40
#define AXP192_IRQ_2_BATT_ACT           0x20
#define AXP192_IRQ_2_BATT_QUIT_ACT      0x10
#define AXP192_IRQ_2_CHARGING           0x08
#define AXP192_IRQ_2_CHARGE_FINISH      0x04
#define AXP192_IRQ_2_BATT_OVER          0x02
#define AXP192_IRQ_2_BATT_UNDER         0x01

#define AXP192_IRQ_3_INTERNAL_OVER      0x80
#define AXP192_IRQ_3_NOT_ENOUGH_CURR    0x40
#define AXP192_IRQ_3_DCDC1_UNDER        0x20
#define AXP192_IRQ_3_DCDC2_UNDER        0x10
#define AXP192_IRQ_3_DCDC3_UNDER        0x08
#define AXP192_IRQ_3_SHORT_KEY          0x02
#define AXP192_IRQ_3_LONG_KEY           0x01

#define AXP192_IRQ_4_POWER_ON           0x80
#define AXP192_IRQ_4_POWER_OFF          0x40
#define AXP192_IRQ_4_VBUS_VALID         0x20
#define AXP192_IRQ_4_VBUS_INVLAID       0x10
#define AXP192_IRQ_4_VBUS_SESSION_AB    0x08
#define AXP192_IRQ_4_VBUS_SESSION_END   0x04
#define AXP192_IRQ_4_UNDER_VOLTAGE      0x01

#define AXP192_IRQ1 ( 0 \
		| AXP192_IRQ_1_ACIN_INSERT \
		| AXP192_IRQ_1_ACIN_REMOVE \
		)
#define AXP192_IRQ2 ( 0 \
		| AXP192_IRQ_2_CHARGE_FINISH \
		| AXP192_IRQ_2_CHARGING \
		)
#define AXP192_IRQ3 ( 0 \
		| AXP192_IRQ_3_SHORT_KEY \
		| AXP192_IRQ_3_LONG_KEY \
		)
#define AXP192_IRQ4 ( 0 \
		)


/* DCDC voltage level */
static struct axp192_voltage_t dcdc_voltages[] = {
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

static struct axp192_voltage_t ldo_voltages[] = {
	{ 1800000, 0x00 }, { 1900000, 0x01 }, { 2000000, 0x02 }, { 2100000, 0x03 },
	{ 2200000, 0x04 }, { 2300000, 0x05 }, { 2400000, 0x06 }, { 2500000, 0x07 },
	{ 2600000, 0x08 }, { 2700000, 0x09 }, { 2800000, 0x0A }, { 2900000, 0x0B },
	{ 3000000, 0x0C }, { 3100000, 0x0D }, { 3200000, 0x0E }, { 3300000, 0x0F }, // 16
};
#define NUM_LDO	 ARRAY_SIZE(ldo_voltages)


/********************************************************************
	Variables
********************************************************************/
static struct workqueue_struct *axp192_wq;
static struct i2c_client *axp192_i2c_client = NULL;
static int axp192_acin_det = 0;
static int axp192_charge_sts = 0;
static unsigned int axp192_suspend_status = 0;

/********************************************************************
	Functions (Global)
********************************************************************/
int axp192_battery_voltage(void)
{
	signed long data[2];
	int ret = 4200;

	if (axp192_i2c_client) {
		data[0] = i2c_smbus_read_byte_data(axp192_i2c_client, AXP192_ADC_BATT_VOL_H_REG);
		data[1] = i2c_smbus_read_byte_data(axp192_i2c_client, AXP192_ADC_BATT_VOL_L_REG);
		ret = ((data[0]<<4)|(data[1]&0xF))&0xFFF;
	}

//	dbg("%s: %dmV\n", __func__, ret);
	return ret;
}

int axp192_acin_detect(void)
{
	signed long data;

	if (axp192_i2c_client && !(axp192_i2c_client->irq)) {
		data = i2c_smbus_read_byte_data(axp192_i2c_client, AXP192_POWER_STS_REG);
		if (data & 0x80)
			axp192_acin_det = 1;
		else
			axp192_acin_det = 0;
	}

//	dbg("%s: %d\n", __func__, axp192_acin_det);
	return axp192_acin_det;
}

void axp192_power_off(void)
{
	i2c_smbus_write_byte_data(axp192_i2c_client, AXP192_POWER_OFF_REG, 0xC6);
}

void axp192_charge_current(unsigned char curr)
{
	unsigned char value;

	if (curr >= AXP192_CHG_CURR_MAX)
		return;

	value = AXP192_CHG_EN|AXP192_CHG_VOL_4_20V|AXP192_CHG_OFF_CUR_10PER|(curr&0x0f);
	i2c_smbus_write_byte_data(axp192_i2c_client, AXP192_CHARGING_CTRL1_REG, value);
}

/* Test API */
int axp192_charge_status(void)
{
	return axp192_charge_sts;
}

int axp192_get_batt_discharge_current(void)
{
	signed long dischg[2];
	int discharge_cur = 0;
	if (axp192_i2c_client) {
		dischg[0] = i2c_smbus_read_byte_data(axp192_i2c_client, AXP192_BATT_DISCHARGE_CURRENT_H_REG);
		dischg[1] = i2c_smbus_read_byte_data(axp192_i2c_client, AXP192_BATT_DISCHARGE_CURRENT_L_REG);
		discharge_cur = ((dischg[0] << 5) | (dischg[1] & 0x1f))*5/10;
//		dbg("BATT Discharge Current %d\n", discharge_cur);
	}
	return discharge_cur;	

}

int axp192_get_batt_charge_current(void)
{
	signed long chg[2];
	int charge_cur = 0;
	if (axp192_i2c_client) {
		chg[0] = i2c_smbus_read_byte_data(axp192_i2c_client, AXP192_BATT_CHARGE_CURRENT_H_REG);
		chg[1] = i2c_smbus_read_byte_data(axp192_i2c_client, AXP192_BATT_CHARGE_CURRENT_L_REG);
		charge_cur = ((chg[0] << 5) | (chg[1] & 0x0f))*5/10;
//		dbg("BATT Charge Current %d\n", charge_cur);
	}
	return charge_cur;	
}
EXPORT_SYMBOL(axp192_battery_voltage);
EXPORT_SYMBOL(axp192_acin_detect);
EXPORT_SYMBOL(axp192_power_off);
EXPORT_SYMBOL(axp192_charge_current);
EXPORT_SYMBOL(axp192_charge_status);
EXPORT_SYMBOL(axp192_get_batt_discharge_current);
EXPORT_SYMBOL(axp192_get_batt_charge_current);

/*******************************************
	Functions (Internal)
*******************************************/
static void axp192_work_func(struct work_struct *work)
{
	struct axp192_data* axp192 = container_of(work, struct axp192_data, work);
	signed long data[4];
	
	dbg("%s\n", __func__);

	data[0] =0;
	data[1] =0;
	data[2] =0;
	data[3] =0;
	
	data[0] = i2c_smbus_read_byte_data(axp192->client, AXP192_IRQ_STS1_REG);
	i2c_smbus_write_byte_data(axp192->client, AXP192_IRQ_STS1_REG, 0xFF);
	data[1] = i2c_smbus_read_byte_data(axp192->client, AXP192_IRQ_STS2_REG);
	i2c_smbus_write_byte_data(axp192->client, AXP192_IRQ_STS2_REG, 0xFF);
	data[2] = i2c_smbus_read_byte_data(axp192->client, AXP192_IRQ_STS3_REG);
	i2c_smbus_write_byte_data(axp192->client, AXP192_IRQ_STS3_REG, 0xFF);
	data[3] = i2c_smbus_read_byte_data(axp192->client, AXP192_IRQ_STS4_REG);
	i2c_smbus_write_byte_data(axp192->client, AXP192_IRQ_STS4_REG, 0xFF);

	if (data[0]&AXP192_IRQ_1_VBUS_LOWER_VALID) {
		dbg("VBUS valid, but lower then V(hold)IRQ status\n");
	}
	if (data[0]&AXP192_IRQ_1_VBUS_REMOVE) {
		dbg("VBUS remove IRQ status\n");
	}
	if (data[0]&AXP192_IRQ_1_VBUS_INSERT) {
		dbg("VBUS insert IRQ status\n");
	}
	if (data[0]&AXP192_IRQ_1_VBUS_OVER) {
		dbg("VBUS over voltage IRQ status\n");
	}
	if (data[0]&AXP192_IRQ_1_ACIN_REMOVE) {
		axp192_acin_det = 0;
		axp192_charge_sts = AXP192_CHG_NONE;
		dbg("ACIN remove IRQ status\n");
	}
	if (data[0]&AXP192_IRQ_1_ACIN_INSERT) {
		axp192_acin_det = 1;
		dbg("ACIN insert IRQ status\n");
	}
	if (data[0]&AXP192_IRQ_1_ACIN_OVER) {
		dbg("ACIN over voltage IRQ status\n");
	}
	if (data[1]&AXP192_IRQ_2_BATT_UNDER) {
		dbg("Battery under temperature IRQ status\n");
	}
	if (data[1]&AXP192_IRQ_2_BATT_OVER) {
		dbg("Battery over temperature IRQ status\n");
	}
	if (data[1]&AXP192_IRQ_2_CHARGE_FINISH) {
		dbg("Charge finish IRQ status\n");
		axp192_charge_sts = AXP192_CHG_OK;
	}
	if (data[1]&AXP192_IRQ_2_CHARGING) {
		dbg("Charging IRQ status\n");
		axp192_charge_sts = AXP192_CHG_ING;
	}
	if (data[1]&AXP192_IRQ_2_BATT_QUIT_ACT) {
		dbg("Quit battery active mode IRQ status\n");
	}
	if (data[1]&AXP192_IRQ_2_BATT_ACT) {
		dbg("Battery active mode IRQ status\n");
	}
	if (data[1]&AXP192_IRQ_2_BATT_REMOVE) {
		dbg("Battery remove IRQ status\n");
	}
	if (data[1]&AXP192_IRQ_2_BATT_INSERT) {
		dbg("Battery insert IRQ status\n");
	}
	if (data[2]&AXP192_IRQ_3_LONG_KEY) {
#if defined(CONFIG_REGULATOR_AXP192_PEK)
		del_timer(&axp192->timer);
		input_report_key(axp192->input_dev, KEY_POWER, 1);
		//input_event(axp192->input_dev, EV_KEY, KEY_POWER, 1);
		input_event(axp192->input_dev, EV_SYN, 0, 0);
		axp192->timer.expires = jiffies + msecs_to_jiffies(1000);
		add_timer(&axp192->timer);
#endif
		dbg("Long time key press IRQ status\n");
	}
	if (data[2]&AXP192_IRQ_3_SHORT_KEY) {
#if defined(CONFIG_REGULATOR_AXP192_PEK)
		del_timer(&axp192->timer);
		input_report_key(axp192->input_dev, KEY_POWER, 1);
		//input_event(axp192->input_dev, EV_KEY, KEY_POWER, 1);
		input_event(axp192->input_dev, EV_SYN, 0, 0);
		axp192->timer.expires = jiffies + msecs_to_jiffies(100);
		add_timer(&axp192->timer);
#endif
		dbg("Short time key press IRQ status\n");
	}
	if (data[2]&AXP192_IRQ_3_DCDC3_UNDER) {
		dbg("DC-DC3 under voltage IRQ status\n");
	}
	if (data[2]&AXP192_IRQ_3_DCDC2_UNDER) {
		dbg("DC-DC2 under voltage IRQ status\n");
	}
	if (data[2]&AXP192_IRQ_3_DCDC1_UNDER) {
		dbg("DC-DC1 under voltage IRQ status\n");
	}
	if (data[2]&AXP192_IRQ_3_NOT_ENOUGH_CURR) {
		dbg("Charge current not enough IRQ status\n");
	}
	if (data[2]&AXP192_IRQ_3_INTERNAL_OVER) {
		dbg("AXP192 internal over temperature IRQ status\n");
	}
	if (data[3]&AXP192_IRQ_4_UNDER_VOLTAGE) {
		dbg("APS under voltage IRQ status\n");
	}
	if (data[3]&AXP192_IRQ_4_VBUS_SESSION_END) {
		dbg("VBUS Session End IRQ status\n");
	}
	if (data[3]&AXP192_IRQ_4_VBUS_SESSION_AB) {
		dbg("VBUS Session A/B IRQ status\n");
	}
	if (data[3]&AXP192_IRQ_4_VBUS_INVLAID) {
		dbg("VBUS invalid IRQ status\n");
	}
	if (data[3]&AXP192_IRQ_4_VBUS_VALID) {
		dbg("VBUS valid IRQ status\n");
	}
	if (data[3]&AXP192_IRQ_4_POWER_OFF) {
		dbg("Power off by N_OE IRQ status\n");
	}
	if (data[3]&AXP192_IRQ_4_POWER_ON) {
		dbg("Power on by N_OE IRQ status\n");
	}

	enable_irq(axp192->client->irq);
}

static irqreturn_t axp192_interrupt(int irqno, void *param)
{
	struct axp192_data *axp192 = (struct axp192_data *)param;
	dbg("%s\n", __func__);

	disable_irq_nosync(axp192->client->irq);
	queue_work(axp192_wq, &axp192->work);
	return IRQ_HANDLED;
}

#if defined(CONFIG_REGULATOR_AXP192_PEK)
static void axp192_timer_func(unsigned long data)
{
	struct axp192_data *axp192 = (struct axp192_data *)data;
	dbg("%s\n", __func__);

	input_report_key(axp192->input_dev, KEY_POWER, 0);
	//input_event(axp192->input_dev, EV_KEY, KEY_POWER, 0);
	input_event(axp192->input_dev, EV_SYN, 0, 0);
}
#endif

static int axp192_dcdc_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV, unsigned *selector)
{
	struct axp192_data* axp192 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value = 0;
	int i, max_num = 0, ret;

	if(axp192_suspend_status)
		return -EBUSY;

	switch (id) {
#if !defined(CONFIG_REGULATOR_AXP192_DCDC12_CONTROLLING_TOGETHER)
		case AXP192_ID_DCDC1:
			reg = AXP192_DCDC1_REG;
			max_num = NUM_DCDC;
			break;
#endif
		case AXP192_ID_DCDC2:
			reg = AXP192_DCDC2_REG;
			max_num = NUM_DCDC2;
			break;
		case AXP192_ID_DCDC3:
			reg = AXP192_DCDC3_REG;
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

#if defined(CONFIG_REGULATOR_AXP192_DCDC12_CONTROLLING_TOGETHER)
	if (id == AXP192_ID_DCDC2) {
		ret = i2c_smbus_read_byte_data(axp192->client, reg);
		if (ret < 0)
			return -EINVAL;

		if (value >= (ret &= 0xFF)) {
			ret = i2c_smbus_write_byte_data(axp192->client, AXP192_DCDC1_REG, value);
			ret = i2c_smbus_write_byte_data(axp192->client, reg, value);
		}
		else {
			ret = i2c_smbus_write_byte_data(axp192->client, reg, value);
			ret = i2c_smbus_write_byte_data(axp192->client, AXP192_DCDC1_REG, value);
		}
			
	} else
#endif
	ret = i2c_smbus_write_byte_data(axp192->client, reg, value);
	udelay(50);

	return ret;
}

static int axp192_dcdc_get_voltage(struct regulator_dev *rdev)
{
	struct axp192_data* axp192 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg;
	int ret, i, max_num, voltage = 0;

	switch (id) {
#if !defined(CONFIG_REGULATOR_AXP192_DCDC12_CONTROLLING_TOGETHER)
		case AXP192_ID_DCDC1:
			reg = AXP192_DCDC1_REG;
			max_num = NUM_DCDC;
			break;
#endif
		case AXP192_ID_DCDC2:
			reg = AXP192_DCDC2_REG;
			max_num = NUM_DCDC2;
			break;
		case AXP192_ID_DCDC3:
			reg = AXP192_DCDC3_REG;
			max_num = NUM_DCDC;
			break;
		default:
			return -EINVAL;
	}

	ret = i2c_smbus_read_byte_data(axp192->client, reg);
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

static struct regulator_ops axp192_dcdc_ops = {
	.set_voltage = axp192_dcdc_set_voltage,
	.get_voltage = axp192_dcdc_get_voltage,
};

static int axp192_ldo_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV, unsigned *selector)
{
	struct axp192_data* axp192 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, old_value, value;
	int i, shift;

	switch (id) {
		case AXP192_ID_LDO2:
			reg = AXP192_LDO23_REG;
			old_value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			shift = 4;
			break;
		case AXP192_ID_LDO3:
			reg = AXP192_LDO23_REG;
			old_value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			shift = 0;
			break;
		case AXP192_ID_LDO4:
			reg = AXP192_LDO4_VOLTAGE_REG;
			old_value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			shift = 4;
			break;
		default:
			return -EINVAL;
	}

	for (i = 0; i < NUM_LDO; i++) {
		if (ldo_voltages[i].uV >= min_uV) {
			value = ldo_voltages[i].val;
			break;
		}
	}

	if (i == NUM_LDO)
		return -EINVAL;

	value = (old_value&(0xFF&(~(0xF<<shift)))) | (value<<shift);

	dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, ldo_voltages[i].uV/1000);
	return i2c_smbus_write_byte_data(axp192->client, reg, value);
}

static int axp192_ldo_get_voltage(struct regulator_dev *rdev)
{
	struct axp192_data* axp192 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg;
	int ret, i, shift, voltage = 0;

	switch (id) {
		case AXP192_ID_LDO2:
			reg = AXP192_LDO23_REG;
			shift = 4;
			break;
		case AXP192_ID_LDO3:
			reg = AXP192_LDO23_REG;
			shift = 0;
			break;
		case AXP192_ID_LDO4:
			reg = AXP192_LDO4_VOLTAGE_REG;
			shift = 4;
			break;
		default:
			return -EINVAL;
	}

	ret = i2c_smbus_read_byte_data(axp192->client, reg);
	if (ret < 0)
		return -EINVAL;

	ret &= 0xFF;
	for (i = 0; i < NUM_LDO; i++) {
		if (ldo_voltages[i].val == ret) {
			voltage = ldo_voltages[i].uV;
			break;
		}
	}

	if (i == NUM_LDO)
		return -EINVAL;

	dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, voltage/1000);
	return voltage;
}

static int axp192_ldo_enable(struct regulator_dev *rdev)
{
	struct axp192_data* axp192 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;

	switch (id) {
		case AXP192_ID_LDO2:
			reg = AXP192_DCDC13_LDO23_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			value |= 0x04;
			break;
		case AXP192_ID_LDO3:
			reg = AXP192_DCDC13_LDO23_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			value |= 0x08;
			break;
		case AXP192_ID_LDO4:
			reg = AXP192_LDO4_FUNC_SET_REG;
			value = 0x02;	// low noise LDO
			break;
		default:
			return -EINVAL;
	}

	dbg("%s: id:%d\n", __func__, id);
	return i2c_smbus_write_byte_data(axp192->client, reg, value);
}

static int axp192_ldo_disable(struct regulator_dev *rdev)
{
	struct axp192_data* axp192 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;

	switch (id) {
		case AXP192_ID_LDO2:
			reg = AXP192_DCDC13_LDO23_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			value &= ~0x04;
			break;
		case AXP192_ID_LDO3:
			reg = AXP192_DCDC13_LDO23_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			value &= ~0x08;
			break;
		case AXP192_ID_LDO4:
			reg = AXP192_LDO4_FUNC_SET_REG;
			value = 0x05;	// Low output
			break;
		default:
			return -EINVAL;
	}

	dbg("%s: id:%d\n", __func__, id);
	return i2c_smbus_write_byte_data(axp192->client, reg, value);
}

static int axp192_ldo_is_enabled(struct regulator_dev *rdev)
{
	struct axp192_data* axp192 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;
	int rst = 0;

	switch (id) {
		case AXP192_ID_LDO2:
			reg = AXP192_DCDC13_LDO23_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			if (value & 0x04)
				rst = 1;
			break;
		case AXP192_ID_LDO3:
			reg = AXP192_DCDC13_LDO23_CTRL_REG;
			value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			if (value & 0x08)
				rst = 1;
			break;
		case AXP192_ID_LDO4:
			reg = AXP192_LDO4_FUNC_SET_REG;
			value = (u8)i2c_smbus_read_byte_data(axp192->client, reg);
			if ((value&0x07) == 0x02)
				rst = 1;
			break;
		default:
			return -EINVAL;
	}

	dbg("%s: id:%d, rst:%d\n", __func__, id, rst);

	return rst;
}

static struct regulator_ops axp192_ldo_ops = {
	.set_voltage = axp192_ldo_set_voltage,
	.get_voltage = axp192_ldo_get_voltage,
	.enable	  = axp192_ldo_enable,
	.disable	 = axp192_ldo_disable,
	.is_enabled  = axp192_ldo_is_enabled,
};

static struct regulator_desc axp192_reg[] = {
	{
		.name = "dcdc1",
		.id = AXP192_ID_DCDC1,
		.ops = &axp192_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "dcdc2",
		.id = AXP192_ID_DCDC2,
		.ops = &axp192_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC2 + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "dcdc3",
		.id = AXP192_ID_DCDC3,
		.ops = &axp192_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
#if 0
	{
		.name = "ldo1",
		.id = AXP192_ID_LDO1,
		.ops = &axp192_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
#endif
	{
		.name = "ldo2",
		.id = AXP192_ID_LDO2,
		.ops = &axp192_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo3",
		.id = AXP192_ID_LDO3,
		.ops = &axp192_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo4",
		.id = AXP192_ID_LDO4,
		.ops = &axp192_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
};
#define NUM_OUPUT	ARRAY_SIZE(axp192_reg)

static int axp192_pmic_probe(struct i2c_client *client, const struct i2c_device_id *i2c_id)
{
	struct regulator_dev **rdev;
	struct axp192_platform_data *pdata = client->dev.platform_data;
	struct axp192_data *axp192;
	int i, id, ret = -ENOMEM;
	unsigned int power_status;

	axp192 = kzalloc(sizeof(struct axp192_data) +
			sizeof(struct regulator_dev *) * (NUM_OUPUT + 1),
			GFP_KERNEL);
	if (!axp192)
		goto out;

	axp192->client = client;
	axp192_i2c_client = client;

	axp192->min_uV = AXP192_MIN_UV;
	axp192->max_uV = AXP192_MAX_UV;

	rdev = axp192->rdev;

	i2c_set_clientdata(client, rdev);

	power_status = i2c_smbus_read_byte_data(client, AXP192_POWER_STS_REG);
	if ((power_status&0xFF) == 0xFF) {
		ret = -ENODEV;
		goto err_nodev;
	} else {
		printk("######## %s: %x ########\n", __func__, power_status);
	}

	if (power_status & 0x80)
		axp192_acin_det = 1;
	else
		axp192_acin_det = 0;


	INIT_WORK(&axp192->work, axp192_work_func);

	for (i = 0; i < pdata->num_subdevs && i <= NUM_OUPUT; i++) {
		id = pdata->subdevs[i].id;
		if (!pdata->subdevs[i].platform_data) {
			rdev[i] = NULL;
			continue;
		}
		if (id >= AXP192_ID_MAX) {
			dev_err(&client->dev, "invalid regulator id %d\n", id);
			goto err;
		}
		rdev[i] = regulator_register(&axp192_reg[id], &client->dev,
						 pdata->subdevs[i].platform_data,
						 axp192);
		if (IS_ERR(rdev[i])) {
			ret = PTR_ERR(rdev[i]);
			dev_err(&client->dev, "failed to register %s\n",
				axp192_reg[id].name);
			goto err;
		}
	}

	/* AXP192 Init. Setting */
	i2c_smbus_write_byte_data(client, AXP192_POWER_OFF_VOL_REG, AXP192_PWROFF_VOL_2_6V);
	i2c_smbus_write_byte_data(client, AXP192_POWER_OFF_REG, AXP192_PWROFF_VALUES);
	i2c_smbus_write_byte_data(client, AXP192_PEK_REG, AXP192_PEK_VALUES);
	i2c_smbus_write_byte_data(client, AXP192_ADC_ENABLE1_REG, AXP192_ADC1);
	i2c_smbus_write_byte_data(client, AXP192_ADC_ENABLE2_REG, AXP192_ADC2);
	i2c_smbus_write_byte_data(client, AXP192_ADC_TSPIN_REG, AXP192_ADC_TS);

	axp192_charge_current(AXP192_CHG_CURR_780mA);

	if (client->irq) {
		/* irq enable */
		i2c_smbus_write_byte_data(client, AXP192_IRQ_EN1_REG, AXP192_IRQ1);
		i2c_smbus_write_byte_data(client, AXP192_IRQ_EN2_REG, AXP192_IRQ2);
		i2c_smbus_write_byte_data(client, AXP192_IRQ_EN3_REG, AXP192_IRQ3);
		i2c_smbus_write_byte_data(client, AXP192_IRQ_EN4_REG, AXP192_IRQ4);

		/* clear irq status */
		i2c_smbus_write_byte_data(client, AXP192_IRQ_STS1_REG, 0xFF);
		i2c_smbus_write_byte_data(client, AXP192_IRQ_STS2_REG, 0xFF);
		i2c_smbus_write_byte_data(client, AXP192_IRQ_STS3_REG, 0xFF);
		i2c_smbus_write_byte_data(client, AXP192_IRQ_STS4_REG, 0xFF);

		if (pdata->init_irq) {
			pdata->init_irq(client->irq);
		}

		if (request_irq(client->irq, axp192_interrupt, IRQ_TYPE_EDGE_FALLING|IRQF_DISABLED, "axp192_irq", axp192)) {
			dev_err(&client->dev, "could not allocate IRQ_NO(%d) !\n", client->irq);
			ret = -EBUSY;
			goto err;
		}
	}

#if defined(CONFIG_REGULATOR_AXP192_PEK)
	// register input device for power key.
	axp192->input_dev = input_allocate_device();
	if (axp192->input_dev == NULL) {
		ret = -ENOMEM;
		dev_err(&client->dev, "%s: Failed to allocate input device\n", __func__);
		goto err_input_dev_alloc_failed;
	}

	axp192->input_dev->evbit[0] = BIT(EV_KEY);
	axp192->input_dev->name = "axp192 power-key";
	set_bit(KEY_POWER & KEY_MAX, axp192->input_dev->keybit);
	ret = input_register_device(axp192->input_dev);
	if (ret) {
		dev_err(&client->dev, "%s: Unable to register %s input device\n", __func__, axp192->input_dev->name);
		goto err_input_register_device_failed;
	}

	init_timer(&axp192->timer);
	axp192->timer.data = (unsigned long)axp192;
	axp192->timer.function = axp192_timer_func;
#endif

	dev_info(&client->dev, "AXP192 regulator driver loaded\n");
	return 0;

#if defined(CONFIG_REGULATOR_AXP192_PEK)
err_input_register_device_failed:
	input_free_device(axp192->input_dev);
err_input_dev_alloc_failed:
#endif
err:
	while (--i >= 0)
		regulator_unregister(rdev[i]);
err_nodev:
	kfree(axp192);
out:
	axp192_i2c_client = NULL;
	return ret;
}

static int axp192_pmic_remove(struct i2c_client *client)
{
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct axp192_data* axp192 = NULL;
	int i;

	for (i=0 ; (rdev[i] != NULL) && (i<AXP192_ID_MAX) ; i++)
		axp192 = rdev_get_drvdata(rdev[i]);

#if defined(CONFIG_REGULATOR_AXP192_PEK)
	if (axp192)
		del_timer(&axp192->timer);
#endif

	for (i = 0; i <= NUM_OUPUT; i++)
		if (rdev[i])
			regulator_unregister(rdev[i]);
	kfree(rdev);
	i2c_set_clientdata(client, NULL);
	axp192_i2c_client = NULL;

	return 0;
}

static int axp192_pmic_suspend(struct i2c_client *client, pm_message_t mesg)
{
	int i, ret = 0;
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct axp192_data* axp192 = NULL;

	for (i=0 ; (rdev[i] != NULL) && (i<AXP192_ID_MAX) ; i++)
		axp192 = rdev_get_drvdata(rdev[i]);

	if (client->irq)
		disable_irq(client->irq);

	/* clear irq status */
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS1_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS2_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS3_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS4_REG, 0xFF);

	if (axp192) {
		ret = cancel_work_sync(&axp192->work);
		flush_workqueue(axp192_wq);
	}

	if (ret && client->irq)
		enable_irq(client->irq);

	axp192_suspend_status = 1;

	return 0;
}

static int axp192_pmic_resume(struct i2c_client *client)
{
	int i;
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct axp192_data* axp192 = NULL;

	for (i=0 ; (rdev[i] != NULL) && (i<AXP192_ID_MAX) ; i++)
		axp192 = rdev_get_drvdata(rdev[i]);

	#if defined(CONFIG_REGULATOR_AXP192_PEK)
	if (axp192)
		queue_work(axp192_wq, &axp192->work);
	#else
	if (client->irq)
		enable_irq(client->irq);
	/* clear irq status */
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS1_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS2_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS3_REG, 0xFF);
	i2c_smbus_write_byte_data(client, AXP192_IRQ_STS4_REG, 0xFF);
	#endif

#if 0
	temp = i2c_smbus_read_byte_data(client, AXP192_POWER_STS_REG);
	if (temp & 0x80)
		axp192_acin_det = 1;
	else
		axp192_acin_det = 0;
#endif 
	axp192_suspend_status = 0;
	return 0;
}

static const struct i2c_device_id axp192_id[] = {
	{ "axp192", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, axp192_id);

static struct i2c_driver axp192_pmic_driver = {
	.probe   = axp192_pmic_probe,
	.remove  = axp192_pmic_remove,
	.suspend = axp192_pmic_suspend,
	.resume  = axp192_pmic_resume,
	.driver  = {
		.name   = "axp192",
	},
	.id_table   = axp192_id,
};

static int __init axp192_pmic_init(void)
{
	axp192_wq = create_singlethread_workqueue("axp192_wq");
	if (!axp192_wq)
		return -ENOMEM;

	return i2c_add_driver(&axp192_pmic_driver);
}
subsys_initcall(axp192_pmic_init);

static void __exit axp192_pmic_exit(void)
{
	i2c_del_driver(&axp192_pmic_driver);
	if (axp192_wq)
		destroy_workqueue(axp192_wq);
}
module_exit(axp192_pmic_exit);

/* Module information */
MODULE_DESCRIPTION("AXP192 regulator driver");
MODULE_AUTHOR("Telechips");
MODULE_LICENSE("GPL");
