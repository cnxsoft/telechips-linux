/*
 * rn5t614.c  --  Voltage and current regulation for the RN5T614
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

#include <linux/regulator/rn5t614.h>

#if defined(DEBUG)
#define dbg(msg...) printk("RN5T614: " msg)
#else
#define dbg(msg...)
#endif

#define RN5T614_MIN_UV   700000
#define RN5T614_MAX_UV  3500000


/********************************************************************
	Define Types
********************************************************************/
struct rn5t614_voltage_t {
	int uV;
	u8  val;
};

struct rn5t614_data {
	struct i2c_client *client;
	unsigned int min_uV;
	unsigned int max_uV;
	struct work_struct work;
	struct regulator_dev *rdev[0];
};


/********************************************************************
	I2C Command & Values
********************************************************************/
/* Registers */
#define RN5T614_PCCNT_REG		0x00
#define RN5T614_PCST_REG		0x01
#define RN5T614_VDCTRL_REG		0x02
#define RN5T614_LDOON_REG		0x03
#define RN5T614_LDO2DAC_REG		0x04
#define RN5T614_LDO3DAC_REG		0x05
#define RN5T614_LDO4DAC_REG		0x06
#define RN5T614_LDO5DAC_REG		0x07
#define RN5T614_LDO6DAC_REG		0x08
#define RN5T614_LDO7DAC_REG		0x09
#define RN5T614_LDO8DAC_REG		0x0A
#define RN5T614_DDCTL1_REG		0x10
#define RN5T614_DDCTL2_REG		0x11
#define RN5T614_RAMP1CTL_REG		0x12
#define RN5T614_RAMP2CTL_REG		0x13
#define RN5T614_DD1DAC_REG		0x14
#define RN5T614_DD2DAC_REG		0x15
#define RN5T614_CHGSTART_REG		0x20
#define RN5T614_FET1CNT_REG		0x21
#define RN5T614_FET2CNT_REG		0x22
#define RN5T614_TEST_REG		0x23
#define RN5T614_CMPSET_REG		0x24
#define RN5T614_SUSPEND_REG		0x25
#define RN5T614_CHGSTATE_REG		0x26
#define RN5T614_CHGEN1_REG		0x28
#define RN5T614_CHGIR1_REG		0x29
#define RN5T614_CHGMON1_REG		0x2A
#define RN5T614_CHGEN2_REG		0x2C
#define RN5T614_CHGIR2_REG		0x2D

/* Interrupt values */
#define RN5T614_IR_VBOV			0x80
#define RN5T614_IR_VCOV			0x40
#define RN5T614_IR_NOBATT		0x20
#define RN5T614_IR_VBTERR		0x04
#define RN5T614_IR_DIEOT		0x02
#define RN5T614_IR_ADPDET		0x01

#define RN5T614_IR_TIMEOUT		0x10
#define RN5T614_IR_CHGCMP		0x08
#define RN5T614_IR_STRC			0x04
#define RN5T614_IR_STCR			0x01

/* Canging control 1 values */

/* DCDC voltage level */
static struct rn5t614_voltage_t dcdc_voltages[] = {
	{  900000, 0x00 }, {  912500, 0x01 }, {  925000, 0x02 }, {  937500, 0x03 },	// 4
	{  950000, 0x04 }, {  962500, 0x05 }, {  975000, 0x06 }, {  987500, 0x07 },	// 8
	{ 1000000, 0x08 }, { 1012500, 0x09 }, { 1025000, 0x0A }, { 1037500, 0x0B },	// 12
	{ 1050000, 0x0C }, { 1062500, 0x0D }, { 1075000, 0x0E }, { 1087500, 0x0F },	// 16
	{ 1100000, 0x10 }, { 1112500, 0x11 }, { 1125000, 0x12 }, { 1137500, 0x13 },	// 20
	{ 1150000, 0x14 }, { 1162500, 0x15 }, { 1175000, 0x16 }, { 1187500, 0x17 },	// 24
	{ 1200000, 0x18 }, { 1212500, 0x19 }, { 1225000, 0x1A }, { 1237500, 0x1B },	// 28
	{ 1250000, 0x1C }, { 1262500, 0x1D }, { 1275000, 0x1E }, { 1287500, 0x1F },	// 32
	{ 1300000, 0x20 }, { 1312500, 0x21 }, { 1325000, 0x22 }, { 1337500, 0x23 },	// 36
	{ 1350000, 0x24 }, { 1362500, 0x25 }, { 1375000, 0x26 }, { 1387500, 0x27 },	// 40
	{ 1400000, 0x28 }, { 1412500, 0x29 }, { 1425000, 0x2A }, { 1437500, 0x2B },	// 44
	{ 1450000, 0x2C }, { 1462500, 0x2D }, { 1475000, 0x2E }, { 1487500, 0x2F },	// 48
	{ 1500000, 0x30 }, { 1512500, 0x31 }, { 1525000, 0x32 }, { 1537500, 0x33 },	// 52
	{ 1550000, 0x34 }, { 1562500, 0x35 }, { 1575000, 0x36 }, { 1587500, 0x37 },	// 56
	{ 1600000, 0x38 }, { 1612500, 0x39 }, { 1625000, 0x3A }, { 1637500, 0x3B },	// 60
	{ 1650000, 0x3C }, { 1662500, 0x3D }, { 1675000, 0x3E }, { 1687500, 0x3F },	// 64
	{ 1700000, 0x40 }, { 1712500, 0x41 }, { 1725000, 0x42 }, { 1737500, 0x43 },	// 68
	{ 1750000, 0x44 }, { 1762500, 0x45 }, { 1775000, 0x46 }, { 1787500, 0x47 },	// 72
	{ 1800000, 0x48 },
};
#define NUM_DCDC ARRAY_SIZE(dcdc_voltages)

static struct rn5t614_voltage_t ldo_voltages[8][8] = {
	{ {       0, 0x00 }, {       0, 0x00 }, {       0, 0x00 }, {       0, 0x00 },
	  {       0, 0x00 }, {       0, 0x00 }, {       0, 0x00 }, {       0, 0x00 } },	// LDO1 - Not Used
	{ {  900000, 0x00 }, { 1000000, 0x01 }, { 1100000, 0x02 }, { 1200000, 0x03 },
	  { 1300000, 0x04 }, {       0, 0x00 }, {       0, 0x00 }, {       0, 0x00 } },	// LDO2
	{ {  900000, 0x00 }, { 1000000, 0x01 }, { 1100000, 0x02 }, { 1200000, 0x03 },
	  { 1300000, 0x04 }, {       0, 0x00 }, {       0, 0x00 }, {       0, 0x00 } },	// LDO3
	{ { 1800000, 0x00 }, { 2500000, 0x01 }, { 2600000, 0x02 }, { 2800000, 0x03 },
	  { 2850000, 0x04 }, { 3000000, 0x05 }, { 3300000, 0x06 }, {       0, 0x00 } },	// LDO4
	{ { 1800000, 0x00 }, { 2500000, 0x01 }, { 2600000, 0x02 }, { 2800000, 0x03 },
	  { 2850000, 0x04 }, { 3000000, 0x05 }, { 3300000, 0x06 }, {       0, 0x00 } },	// LDO5
	{ { 1200000, 0x00 }, { 1800000, 0x01 }, { 2500000, 0x02 }, { 2600000, 0x03 },
	  { 2800000, 0x04 }, { 2850000, 0x05 }, { 3000000, 0x06 }, { 3300000, 0x07 } },	// LDO6
	{ { 1200000, 0x00 }, { 1800000, 0x01 }, { 2500000, 0x02 }, { 2600000, 0x03 },
	  { 2800000, 0x04 }, { 2850000, 0x05 }, { 3000000, 0x06 }, { 3300000, 0x07 } },	// LDO7
	{ {       0, 0x00 }, { 1800000, 0x01 }, { 2500000, 0x02 }, { 2600000, 0x03 },
	  { 2800000, 0x04 }, { 2850000, 0x05 }, { 3000000, 0x06 }, { 3300000, 0x07 } },	// LDO8
};
#define NUM_LDO  8


/********************************************************************
	Variables
********************************************************************/
static struct workqueue_struct *rn5t614_wq;
static struct i2c_client *rn5t614_i2c_client = NULL;
static int rn5t614_acin_det = 0;
static int rn5t614_charge_sts = 0;
static unsigned int rn5t614_suspend_status = 0;


/********************************************************************
	Functions (Global)
********************************************************************/
int rn5t614_battery_voltage(void)
{
	//signed long data[2];
	int ret = 4200;

	if (rn5t614_i2c_client) {
		// TODO: read battery level
	}

//	dbg("%s: %dmV\n", __func__, ret);
	return ret;
}

extern int g_ac_plugin;
int rn5t614_acin_detect(void)
{
	unsigned char temp;

	if(rn5t614_suspend_status) {
		dbg("%s(%d)\n", __func__, g_ac_plugin);
		return g_ac_plugin;
	}

	temp = i2c_smbus_read_byte_data(rn5t614_i2c_client, RN5T614_CHGMON1_REG);
	return temp & 0x01;
}

void rn5t614_power_off(void)
{
    i2c_smbus_write_byte_data(rn5t614_i2c_client, RN5T614_SUSPEND_REG, 0x00);
}

void rn5t614_charge_current(unsigned char val)
{
    //unsigned char temp;

//    temp = RN5T614_CHG_EN|RN5T614_CHG_VOL_4_20V|RN5T614_CHG_OFF_CUR_10PER|(val&0x0f);
    //i2c_smbus_write_byte_data(rn5t614_i2c_client, RN5T614_FET2CNT_REG, temp);
}

/* Test API */
int rn5t614_charge_status(void)
{
	unsigned char temp;

	if(rn5t614_suspend_status) {
		dbg("%s\n", __func__);
		return 0;
	}

	temp = i2c_smbus_read_byte_data(rn5t614_i2c_client, RN5T614_CHGSTATE_REG);
	//TODO: 

	if((temp & 0xf) == 0x05)
	    return 0x02;
	else 	
	    return 0;
}

EXPORT_SYMBOL(rn5t614_battery_voltage);
EXPORT_SYMBOL(rn5t614_acin_detect);
EXPORT_SYMBOL(rn5t614_power_off);
EXPORT_SYMBOL(rn5t614_charge_current);
EXPORT_SYMBOL(rn5t614_charge_status);


/*******************************************
	Functions (Internal)
*******************************************/
static void rn5t614_work_func(struct work_struct *work)
{
	struct rn5t614_data* rn5t614 = container_of(work, struct rn5t614_data, work);
	unsigned char data[3];
	dbg("%s\n", __func__);

	data[0] = (unsigned char)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_CHGIR1_REG);
	data[1] = (unsigned char)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_CHGIR2_REG);
	data[2] = 0;	//(unsigned char)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_CHGMON1_REG);

	if (data[0] & 0xFF)
		i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR1_REG, data[0]);
	if (data[1] & 0xFF)
		i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR2_REG, data[1]);

	if (data[0]&RN5T614_IR_VBOV) {
		dbg("Battery over voltage(VVAT > 4.6V)\n");
	}
	if (data[0]&RN5T614_IR_VCOV) {
		dbg("Adapter over voltage(VVCHG > 6.2V)\n");
	}
	if (data[0]&RN5T614_IR_NOBATT) {
		dbg("No Battery detect\n");
	}
	if (data[0]&RN5T614_IR_VBTERR) {
		dbg("Battery abnormal temperature\n");
	}
	if (data[0]&RN5T614_IR_DIEOT) {
		dbg("Die abnormal temperature by SW1 or SW2 in charger\n");
	}
	if (data[0]&RN5T614_IR_ADPDET) {
		if (data[2] & RN5T614_IR_ADPDET)
			rn5t614_acin_det = 1;
		else
			rn5t614_acin_det = 0;
		dbg("Adapter insert & remove (sts:%d)\n", rn5t614_acin_det);
	}
	if (data[0]&RN5T614_IR_TIMEOUT) {
		dbg("Timer time out\n");
		rn5t614_charge_sts = RN5T614_CHG_TIMEOUT;
	}
	if (data[1]&RN5T614_IR_CHGCMP) {
		dbg("Charge complete\n");
		rn5t614_charge_sts = RN5T614_CHG_OK;
	}
	if (data[1]&RN5T614_IR_STRC) {
		dbg("Shift to Rapid-Charge state\n");
	}
	if (data[1]&RN5T614_IR_STCR) {
		dbg("Shift to Charge-Ready state\n");
	}

	i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR1_REG, ~data[0]);
	i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR2_REG, ~data[1]);
	enable_irq(rn5t614->client->irq);
}

static irqreturn_t rn5t614_interrupt(int irqno, void *param)
{
	struct rn5t614_data *rn5t614 = (struct rn5t614_data *)param;
	dbg("%s\n", __func__);

	disable_irq_nosync(rn5t614->client->irq);
	queue_work(rn5t614_wq, &rn5t614->work);
	return IRQ_HANDLED;
}

static int rn5t614_dcdc_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value = 0;
	int i, ret;

	if(rn5t614_suspend_status)
		return -EBUSY;

	switch (id) {
		case RN5T614_ID_DCDC1:
			reg = RN5T614_DD1DAC_REG;
			break;
		case RN5T614_ID_DCDC2:
			reg = RN5T614_DD2DAC_REG;
			break;
//		case RN5T614_ID_DCDC3:
//			break;
		default:
			return -EINVAL;
	}

	for (i = 0; i < NUM_DCDC; i++) {
		if (dcdc_voltages[i].uV >= min_uV) {
			value = dcdc_voltages[i].val;
			break;
		}
	}

	if (i == NUM_DCDC)
		return -EINVAL;


	dbg("%s: reg:0x%x value:%dmV\n", __func__, reg, dcdc_voltages[i].uV/1000);
	ret = i2c_smbus_write_byte_data(rn5t614->client, reg, value);
	udelay(50);

	if(ret != 0)
		printk("%s(reg:%x, val:%x) - fail(%d)\n", __func__, reg, value, ret);

	return ret;
}

static int rn5t614_dcdc_get_voltage(struct regulator_dev *rdev)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg;
	int ret, i, voltage = 0;

	switch (id) {
		case RN5T614_ID_DCDC1:
			reg = RN5T614_DD1DAC_REG;
			break;
		case RN5T614_ID_DCDC2:
			reg = RN5T614_DD2DAC_REG;
			break;
//		case RN5T614_ID_DCDC3:
//			break;
		default:
			return -EINVAL;
	}

	ret = i2c_smbus_read_byte_data(rn5t614->client, reg);
	if (ret < 0)
		return -EINVAL;

	ret &= 0xFF;
	for (i = 0; i < NUM_DCDC; i++) {
		if (dcdc_voltages[i].val == ret) {
			voltage = dcdc_voltages[i].uV;
			break;
		}
	}

	if (i == NUM_DCDC)
		return -EINVAL;

	dbg("%s: reg:0x%x value:%dmV\n", __func__, reg, voltage/1000);
	return voltage;
}

static int rn5t614_dcdc_enable(struct regulator_dev *rdev)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg = 0x0, value, old_value, bit;

	dbg("%s: id:%d\n", __func__, id);
	switch (id) {
		case RN5T614_ID_DCDC1:
		case RN5T614_ID_DCDC2:
		case RN5T614_ID_DCDC3:
			reg = RN5T614_DDCTL1_REG;
			bit = id - RN5T614_ID_DCDC1;
			break;
		default:
			return -EINVAL;
	}

	old_value = (u8)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_DDCTL1_REG);
	value = old_value | (1 << bit);

	return i2c_smbus_write_byte_data(rn5t614->client, RN5T614_DDCTL1_REG, value);
}

static int rn5t614_dcdc_disable(struct regulator_dev *rdev)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg = 0x0, value, old_value, bit;

	dbg("%s: id:%d\n", __func__, id);
	switch (id) {
		case RN5T614_ID_DCDC1:
		case RN5T614_ID_DCDC2:
		case RN5T614_ID_DCDC3:
			reg = RN5T614_DDCTL1_REG;
			bit = id - RN5T614_ID_DCDC1;
			break;
		default:
			return -EINVAL;
	}

	old_value = (u8)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_DDCTL1_REG);
	value = old_value & ~(1 << bit);

	return i2c_smbus_write_byte_data(rn5t614->client, RN5T614_DDCTL1_REG, value);
}

static struct regulator_ops rn5t614_dcdc_ops = {
	.set_voltage = rn5t614_dcdc_set_voltage,
	.get_voltage = rn5t614_dcdc_get_voltage,
	.enable      = rn5t614_dcdc_enable,
	.disable     = rn5t614_dcdc_disable,
};

static int rn5t614_ldo_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;
	int i;

	switch (id) {
		case RN5T614_ID_LDO2:
			reg = RN5T614_LDO2DAC_REG;
			break;
		case RN5T614_ID_LDO3:
			reg = RN5T614_LDO3DAC_REG;
			break;
		case RN5T614_ID_LDO4:
			reg = RN5T614_LDO4DAC_REG;
			break;
		case RN5T614_ID_LDO5:
			reg = RN5T614_LDO5DAC_REG;
			break;
		case RN5T614_ID_LDO6:
			reg = RN5T614_LDO6DAC_REG;
			break;
		case RN5T614_ID_LDO7:
			reg = RN5T614_LDO7DAC_REG;
			break;
		case RN5T614_ID_LDO8:
			reg = RN5T614_LDO8DAC_REG;
			break;
		default:
			return -EINVAL;
	}

	for (i = 0; i < NUM_LDO; i++) {
		if (ldo_voltages[id-RN5T614_ID_LDO1][i].uV && (ldo_voltages[id-RN5T614_ID_LDO1][i].uV >= min_uV)) {
			value = ldo_voltages[id-RN5T614_ID_LDO1][i].val;
			break;
		}
	}

	if (i == NUM_LDO)
		return -EINVAL;

	dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, ldo_voltages[id-RN5T614_ID_LDO1][i].uV/1000);
	return i2c_smbus_write_byte_data(rn5t614->client, reg, value);
}

static int rn5t614_ldo_get_voltage(struct regulator_dev *rdev)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg;
	int ret, i, voltage = 0;

	switch (id) {
		case RN5T614_ID_LDO2:
			reg = RN5T614_LDO2DAC_REG;
			break;
		case RN5T614_ID_LDO3:
			reg = RN5T614_LDO3DAC_REG;
			break;
		case RN5T614_ID_LDO4:
			reg = RN5T614_LDO4DAC_REG;
			break;
		case RN5T614_ID_LDO5:
			reg = RN5T614_LDO5DAC_REG;
			break;
		case RN5T614_ID_LDO6:
			reg = RN5T614_LDO6DAC_REG;
			break;
		case RN5T614_ID_LDO7:
			reg = RN5T614_LDO7DAC_REG;
			break;
		case RN5T614_ID_LDO8:
			reg = RN5T614_LDO8DAC_REG;
			break;
		default:
			return -EINVAL;
	}

	ret = i2c_smbus_read_byte_data(rn5t614->client, reg);
	if (ret < 0)
		return -EINVAL;

	ret &= 0xFF;
	for (i = 0; i < NUM_LDO; i++) {
		if (ldo_voltages[id-RN5T614_ID_LDO1][i].uV && (ldo_voltages[id-RN5T614_ID_LDO1][i].val == ret)) {
			voltage = ldo_voltages[id-RN5T614_ID_LDO1][i].uV;
			break;
		}
	}

	if (i == NUM_LDO)
		return -EINVAL;

	dbg("%s: reg:0x%x value: %dmV\n", __func__, reg, voltage/1000);
	return voltage;
}

static int rn5t614_ldo_enable(struct regulator_dev *rdev)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 value, old_value, bit;

	dbg("%s: id:%d\n", __func__, id);
	switch (id) {
//		case RN5T614_ID_LDO1:
		case RN5T614_ID_LDO2:
		case RN5T614_ID_LDO3:
		case RN5T614_ID_LDO4:
		case RN5T614_ID_LDO5:
		case RN5T614_ID_LDO6:
		case RN5T614_ID_LDO7:
//		case RN5T614_ID_LDO8:
			bit = id - RN5T614_ID_LDO1;
			break;
		default:
			return -EINVAL;
	}

	old_value = (u8)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_LDOON_REG);
	value = old_value | (1 << bit);

	return i2c_smbus_write_byte_data(rn5t614->client, RN5T614_LDOON_REG, value);
}

static int rn5t614_ldo_disable(struct regulator_dev *rdev)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 value, old_value, bit;

	dbg("%s: id:%d\n", __func__, id);
	switch (id) {
//		case RN5T614_ID_LDO1:
		case RN5T614_ID_LDO2:
		case RN5T614_ID_LDO3:
		case RN5T614_ID_LDO4:
		case RN5T614_ID_LDO5:
		case RN5T614_ID_LDO6:
		case RN5T614_ID_LDO7:
//		case RN5T614_ID_LDO8:
			bit = id - RN5T614_ID_LDO1;
			break;
		default:
			return -EINVAL;
	}

	old_value = (u8)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_LDOON_REG);
	value = old_value & ~(1 << bit);

	return i2c_smbus_write_byte_data(rn5t614->client, RN5T614_LDOON_REG, value);
}

static int rn5t614_ldo_is_enabled(struct regulator_dev *rdev)
{
	struct rn5t614_data* rn5t614 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 old_value, bit;

	dbg("%s: id:%d\n", __func__, id);
	switch (id) {
//		case RN5T614_ID_LDO1:
		case RN5T614_ID_LDO2:
		case RN5T614_ID_LDO3:
		case RN5T614_ID_LDO4:
		case RN5T614_ID_LDO5:
		case RN5T614_ID_LDO6:
		case RN5T614_ID_LDO7:
//		case RN5T614_ID_LDO8:
			bit = id - RN5T614_ID_LDO1;
			break;
		default:
			return -EINVAL;
	}

	old_value = (u8)i2c_smbus_read_byte_data(rn5t614->client, RN5T614_LDOON_REG);
	if (old_value & (1 << bit))
		;
	else
		;

	return 0;
}

static struct regulator_ops rn5t614_ldo_ops = {
	.set_voltage = rn5t614_ldo_set_voltage,
	.get_voltage = rn5t614_ldo_get_voltage,
	.enable      = rn5t614_ldo_enable,
	.disable     = rn5t614_ldo_disable,
	.is_enabled  = rn5t614_ldo_is_enabled,
};

static struct regulator_desc rn5t614_reg[] = {
	{
		.name = "dcdc1",
		.id = RN5T614_ID_DCDC1,
		.ops = &rn5t614_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "dcdc2",
		.id = RN5T614_ID_DCDC2,
		.ops = &rn5t614_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "dcdc3",
		.id = RN5T614_ID_DCDC3,
		.ops = &rn5t614_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo1",
		.id = RN5T614_ID_LDO1,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo2",
		.id = RN5T614_ID_LDO2,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo3",
		.id = RN5T614_ID_LDO3,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo4",
		.id = RN5T614_ID_LDO4,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo5",
		.id = RN5T614_ID_LDO5,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo6",
		.id = RN5T614_ID_LDO6,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo7",
		.id = RN5T614_ID_LDO7,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo8",
		.id = RN5T614_ID_LDO8,
		.ops = &rn5t614_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
};
#define NUM_OUPUT	ARRAY_SIZE(rn5t614_reg)

static int rn5t614_pmic_probe(struct i2c_client *client, const struct i2c_device_id *i2c_id)
{
	struct regulator_dev **rdev;
	struct rn5t614_platform_data *pdata = client->dev.platform_data;
	struct rn5t614_data *rn5t614;
	int i, id, ret = -ENOMEM;
	unsigned int power_status;

	rn5t614 = kzalloc(sizeof(struct rn5t614_data) +
			sizeof(struct regulator_dev *) * (NUM_OUPUT + 1),
			GFP_KERNEL);
	if (!rn5t614)
		goto out;

	rn5t614->client = client;
	rn5t614_i2c_client = client;

	rn5t614->min_uV = RN5T614_MIN_UV;
	rn5t614->max_uV = RN5T614_MAX_UV;

	rdev = rn5t614->rdev;

	i2c_set_clientdata(client, rdev);

	// gpio & external interrupt ports configuration
	if (pdata->init_port) {
		pdata->init_port(client->irq);
	}

	power_status = i2c_smbus_read_byte_data(client, 0x00);
	if ((power_status&0xFF) == 0xFF) {
		ret = -ENODEV;
		goto err_nodev;
	} else {
		printk("######## %s: %x ########\n", __func__, power_status);
	}

	INIT_WORK(&rn5t614->work, rn5t614_work_func);

	for (i = 0; i < pdata->num_subdevs && i <= NUM_OUPUT; i++) {
		id = pdata->subdevs[i].id;
		if (!pdata->subdevs[i].platform_data) {
			rdev[i] = NULL;
			continue;
		}
		if (id >= RN5T614_ID_MAX) {
			dev_err(&client->dev, "invalid regulator id %d\n", id);
			goto err;
		}
		rdev[i] = regulator_register(&rn5t614_reg[id], &client->dev,
						 pdata->subdevs[i].platform_data,
						 rn5t614);
		if (IS_ERR(rdev[i])) {
			ret = PTR_ERR(rdev[i]);
			dev_err(&client->dev, "failed to register %s\n",
				rn5t614_reg[id].name);
			goto err;
		}
	}

	if (client->irq) {
		/* irq enable */
		// TODO:

		/* clear irq status */
		// TODO:
		i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR1_REG, 0x00);
		i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR2_REG, 0x00);


		if (request_irq(client->irq, rn5t614_interrupt, IRQ_TYPE_EDGE_FALLING|IRQF_DISABLED, "rn5t614_irq", rn5t614)) {
			dev_err(&client->dev, "could not allocate IRQ_NO(%d) !\n", client->irq);
			ret = -EBUSY;
			goto err;
		}
	}

	dev_info(&client->dev, "RN5T614 regulator driver loaded\n");
	return 0;

err:
	while (--i >= 0)
		regulator_unregister(rdev[i]);
err_nodev:
	kfree(rn5t614);
out:
	rn5t614_i2c_client = NULL;
	return ret;
}

static int rn5t614_pmic_remove(struct i2c_client *client)
{
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct rn5t614_data* rn5t614 = NULL;
	int i;

	for (i=0 ; (rdev[i] != NULL) && (i<RN5T614_ID_MAX) ; i++)
		rn5t614 = rdev_get_drvdata(rdev[i]);

	for (i = 0; i <= NUM_OUPUT; i++)
		if (rdev[i])
			regulator_unregister(rdev[i]);
	kfree(rdev);
	i2c_set_clientdata(client, NULL);
	rn5t614_i2c_client = NULL;

	return 0;
}

static int rn5t614_pmic_suspend(struct i2c_client *client, pm_message_t mesg)
{
	int i, ret = 0;
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct rn5t614_data* rn5t614 = NULL;

	for (i=0 ; (rdev[i] != NULL) && (i<RN5T614_ID_MAX) ; i++)
		rn5t614 = rdev_get_drvdata(rdev[i]);

	if (client->irq)
		disable_irq(client->irq);

	/* clear irq status */
	// TODO:

	if (rn5t614) {
		ret = cancel_work_sync(&rn5t614->work);
		flush_workqueue(rn5t614_wq);
	}

	if (ret && client->irq)
		enable_irq(client->irq);

	rn5t614_suspend_status = 1;

	return 0;
}

static int rn5t614_pmic_resume(struct i2c_client *client)
{
	int i;
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct rn5t614_data* rn5t614 = NULL;

	for (i=0 ; (rdev[i] != NULL) && (i<RN5T614_ID_MAX) ; i++)
		rn5t614 = rdev_get_drvdata(rdev[i]);

	if (client->irq)
		enable_irq(client->irq);
	/* clear irq status */
	// TODO:
	i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR1_REG, 0x00);
	i2c_smbus_write_byte_data(rn5t614->client, RN5T614_CHGIR2_REG, 0x00);


	rn5t614_suspend_status = 0;
	return 0;
}

static const struct i2c_device_id rn5t614_id[] = {
	{ "rn5t614", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rn5t614_id);

static struct i2c_driver rn5t614_pmic_driver = {
	.probe   = rn5t614_pmic_probe,
	.remove	 = rn5t614_pmic_remove,
	.suspend = rn5t614_pmic_suspend,
	.resume	 = rn5t614_pmic_resume,
	.driver	 = {
	.name    = "rn5t614",
	},
	.id_table   = rn5t614_id,
};

static int __init rn5t614_pmic_init(void)
{
	rn5t614_wq = create_singlethread_workqueue("rn5t614_wq");
	if (!rn5t614_wq)
		return -ENOMEM;

	return i2c_add_driver(&rn5t614_pmic_driver);
}
subsys_initcall(rn5t614_pmic_init);

static void __exit rn5t614_pmic_exit(void)
{
	i2c_del_driver(&rn5t614_pmic_driver);
	if (rn5t614_wq)
		destroy_workqueue(rn5t614_wq);}
module_exit(rn5t614_pmic_exit);

/* Module information */
MODULE_DESCRIPTION("RN5T614 regulator driver");
MODULE_AUTHOR("Telechips");
MODULE_LICENSE("GPL");
