/*
 * act8810.c  --  Voltage and current regulation for the ACT8810
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
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/slab.h>

#include <linux/regulator/act8810.h>

#if defined(DEBUG)
#define dbg(msg...) printk("ACT8810: " msg)
#else
#define dbg(msg...)
#endif

#define ACT8810_MIN_UV   800000
#define ACT8810_MAX_UV  4400000


/********************************************************************
	Define Types
********************************************************************/
struct act8810_voltage_t {
	int uV;
	u8  val;
};

struct act8810_data {
	struct i2c_client *client;
	unsigned int min_uV;
	unsigned int max_uV;
	struct regulator_dev *rdev[0];
};


/********************************************************************
	I2C Command & Values
********************************************************************/
/* Registers */
#define ACT8810_DCDC1_REG                0x13
#define ACT8810_DCDC2_REG                0x23
#define ACT8810_DCDC3_REG                0x33
#define ACT8810_LDO1_REG                 0x28
#define ACT8810_LDO2_REG                 0x36
#define ACT8810_LDO3_REG                 0x40


static struct act8810_voltage_t dcdc_voltages[] = {
	{  800000, 0x07 }, {  825000, 0x08 }, {  850000, 0x09 }, {  875000, 0x0A },
	{  900000, 0x0B }, {  925000, 0x0C }, {  950000, 0x0D }, {  975000, 0x0E },
	{ 1000000, 0x0F }, { 1025000, 0x10 }, { 1050000, 0x11 }, { 1075000, 0x12 }, // 16
	{ 1100000, 0x13 }, { 1125000, 0x14 }, { 1150000, 0x15 }, { 1175000, 0x16 },
	{ 1200000, 0x17 }, { 1225000, 0x18 }, { 1250000, 0x19 }, { 1275000, 0x1A },
	{ 1300000, 0x1B }, { 1325000, 0x1C }, { 1350000, 0x1D }, { 1375000, 0x1E },
	{ 1400000, 0x1F }, { 1425000, 0x20 }, { 1450000, 0x21 }, { 1475000, 0x22 }, // 32
	{ 1500000, 0x23 }, { 1525000, 0x24 }, { 1550000, 0x25 }, { 1575000, 0x26 },
	{ 1600000, 0x27 }, { 1625000, 0x28 }, { 1650000, 0x29 }, { 1675000, 0x2A },
	{ 1700000, 0x2B }, { 1725000, 0x2C }, { 1750000, 0x2D }, { 1775000, 0x2E },
	{ 1800000, 0x2F }, { 1825000, 0x30 }, { 1850000, 0x31 }, { 1875000, 0x32 }, // 48
	{ 1900000, 0x33 }, { 1925000, 0x34 }, { 1950000, 0x35 }, { 1975000, 0x36 },
	{ 2000000, 0x37 }, { 2025000, 0x38 }, { 2050000, 0x39 }, { 2075000, 0x3A },
	{ 2100000, 0x3B }, { 2125000, 0x3C }, { 2150000, 0x3D }, { 2175000, 0x3E },
	{ 2200000, 0x3F }, 
}; 
#define NUM_DCDC	ARRAY_SIZE(dcdc_voltages)
 
 
/********************************************************************
	Functions
********************************************************************/
static int act8810_dcdc_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV)
{
	struct act8810_data* act8810 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;
	int i;

	switch (id) {
		case ACT8810_ID_DCDC1:
			reg = ACT8810_DCDC1_REG;
			break;
		case ACT8810_ID_DCDC2:
			reg = ACT8810_DCDC2_REG;
			break;
		case ACT8810_ID_DCDC3:
			reg = ACT8810_DCDC3_REG;
			break;
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

	//dev_dbg(&client->dev, "changing voltage reg:0x%x to %dmV\n", reg, min_uV / 1000);
	dbg("changing voltage reg:0x%x to %dmV\n", reg, dcdc_voltages[i].uV / 1000);

	return i2c_smbus_write_byte_data(act8810->client, reg, value);
}

static int act8810_dcdc_get_voltage(struct regulator_dev *rdev)
{
	struct act8810_data* act8810 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;

	switch (id) {
		case ACT8810_ID_DCDC1:
			reg = ACT8810_DCDC1_REG;
			break;
		case ACT8810_ID_DCDC2:
			reg = ACT8810_DCDC2_REG;
			break;
		case ACT8810_ID_DCDC3:
			reg = ACT8810_DCDC3_REG;
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static struct regulator_ops act8810_dcdc_ops = {
	.set_voltage = act8810_dcdc_set_voltage,
	.get_voltage = act8810_dcdc_get_voltage,
};

#if 0
static int act8810_ldo_set_voltage(struct regulator_dev *rdev, int min_uV, int max_uV)
{
	struct act8810_data* act8810 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, old_value, value;
	int i;

	switch (id) {
		case ACT8810_ID_LDO1:
			reg = ACT8810_LDO1_REG;
			break;
		case ACT8810_ID_LDO2:
			reg = ACT8810_LDO2_REG;
			break;
		case ACT8810_ID_LDO3:
			reg = ACT8810_LDO3_REG;
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

	old_value = i2c_smbus_read_byte_data(act8810->client, reg);

	if (id == ACT8810_ID_LDO2 || id == ACT8810_ID_LDO4)
		value << 4;

	//dev_dbg(&client->dev, "changing voltage reg:0x%x to %dmV\n", reg, min_uV / 1000);
	dbg("changing voltage reg:0x%x to %dmV\n", reg, ldo_voltages[i].uV / 1000);

	return i2c_smbus_write_byte_data(act8810->client, reg, value);
}

static int act8810_ldo_get_voltage(struct regulator_dev *rdev)
{
	struct act8810_data* act8810 = rdev_get_drvdata(rdev);
	int id = rdev_get_id(rdev);
	u8 reg, value;

	switch (id) {
//		case ACT8810_ID_LDO1:
//			reg = ACT8810_LDO1_REG;
//			break;
		case ACT8810_ID_LDO2:
			reg = ACT8810_LDO23_REG;
			break;
		case ACT8810_ID_LDO3:
			reg = ACT8810_LDO23_REG;
			break;
//		case ACT8810_ID_LDO4:
//			reg = ACT8810_LDO4_REG;
//			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static struct regulator_ops act8810_ldo_ops = {
	.set_voltage = act8810_ldo_set_voltage,
	.get_voltage = act8810_ldo_get_voltage,
};
#endif

static struct regulator_desc act8810_reg[] = {
	{
		.name = "dcdc1",
		.id = ACT8810_ID_DCDC1,
		.ops = &act8810_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "dcdc2",
		.id = ACT8810_ID_DCDC2,
		.ops = &act8810_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "dcdc3",
		.id = ACT8810_ID_DCDC3,
		.ops = &act8810_dcdc_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_DCDC + 1,
		.owner = THIS_MODULE,
	},
#if 0
	{
		.name = "ldo1",
		.id = ACT8810_ID_LDO1,
		.ops = &act8810_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo2",
		.id = ACT8810_ID_LDO2,
		.ops = &act8810_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
	{
		.name = "ldo3",
		.id = ACT8810_ID_LDO3,
		.ops = &act8810_ldo_ops,
		.type = REGULATOR_VOLTAGE,
		.n_voltages = NUM_LDO + 1,
		.owner = THIS_MODULE,
	},
#endif
};
#define NUM_OUPUT	ARRAY_SIZE(act8810_reg)

static int act8810_pmic_probe(struct i2c_client *client, const struct i2c_device_id *i2c_id)
{
	struct regulator_dev **rdev;
	struct act8810_platform_data *pdata = client->dev.platform_data;
	struct act8810_data *act8810;
	int i, id, ret = -ENOMEM;
	unsigned int power_status;

	act8810 = kzalloc(sizeof(struct act8810_data) +
			sizeof(struct regulator_dev *) * (NUM_OUPUT + 1),
			GFP_KERNEL);
	if (!act8810)
		goto out;

	act8810->client = client;

	act8810->min_uV = ACT8810_MIN_UV;
	act8810->max_uV = ACT8810_MAX_UV;

	rdev = act8810->rdev;

	i2c_set_clientdata(client, rdev);

#if 0
	power_status = i2c_smbus_read_byte_data(client, 0x00);
	if ((power_status&0xFF) == 0xFF) {
		ret = -ENODEV;
		goto err_nodev;
	} else {
		printk("######## %s: %x ########\n", __func__, power_status);
	}
#endif

	for (i = 0; i < pdata->num_subdevs && i <= NUM_OUPUT; i++) {
		id = pdata->subdevs[i].id;
		if (!pdata->subdevs[i].platform_data)
			continue;
		if (id >= ACT8810_ID_MAX) {
			dev_err(&client->dev, "invalid regulator id %d\n", id);
			goto err;
		}
		rdev[i] = regulator_register(&act8810_reg[id], &client->dev,
					     pdata->subdevs[i].platform_data,
					     act8810);
		if (IS_ERR(rdev[i])) {
			ret = PTR_ERR(rdev[i]);
			dev_err(&client->dev, "failed to register %s\n",
				act8810_reg[id].name);
			goto err;
		}
	}

#if 0
	if (client->irq) {
		/* long time key press time set to 1S, auto power off function disable */
		i2c_smbus_write_byte_data(client, ACT8810_PEK_REG, 0x45);

		/* clear irq status */
		i2c_smbus_write_byte_data(client, ACT8810_IRQ_STS1_REG, 0xFF);
		i2c_smbus_write_byte_data(client, ACT8810_IRQ_STS2_REG, 0xFF);
		i2c_smbus_write_byte_data(client, ACT8810_IRQ_STS3_REG, 0xFF);
		i2c_smbus_write_byte_data(client, ACT8810_IRQ_STS4_REG, 0xFF);

		if (pdata->init_irq) {
			pdata->init_irq();
			dbg("irq_init\n");
		}

		if (request_irq(client->irq, act8810_interrupt, IRQF_DISABLED, "act8810_irq", act8810)) {
			dev_err(&client->dev, "could not allocate IRQ_NO(%d) !\n", client->irq);
			ret = -EBUSY;
			goto err;
		}
	}
#endif

	dev_info(&client->dev, "ACT8810 regulator driver loaded\n");
	return 0;

err:
	while (--i >= 0)
		regulator_unregister(rdev[i]);
err_nodev:
	kfree(act8810);
out:
	return ret;
}

static int act8810_pmic_remove(struct i2c_client *client)
{
	struct regulator_dev **rdev = i2c_get_clientdata(client);
	struct act8810_data* act8810 = rdev_get_drvdata(rdev[0]);
	int i;

	for (i = 0; i <= NUM_OUPUT; i++)
		if (rdev[i])
			regulator_unregister(rdev[i]);
	kfree(rdev);
	i2c_set_clientdata(client, NULL);

	return 0;
}

static const struct i2c_device_id act8810_id[] = {
	{ "act8810", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, act8810_id);

static struct i2c_driver act8810_pmic_driver = {
	.probe = act8810_pmic_probe,
	.remove = act8810_pmic_remove,
	.driver		= {
		.name	= "act8810",
	},
	.id_table	= act8810_id,
};

static int __init act8810_pmic_init(void)
{
	return i2c_add_driver(&act8810_pmic_driver);
}
subsys_initcall(act8810_pmic_init);

static void __exit act8810_pmic_exit(void)
{
	i2c_del_driver(&act8810_pmic_driver);
}
module_exit(act8810_pmic_exit);

/* Module information */
MODULE_DESCRIPTION("ACT8810 regulator driver");
MODULE_AUTHOR("Telechips");
MODULE_LICENSE("GPL");
