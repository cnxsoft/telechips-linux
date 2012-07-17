/* linux/arch/arm/mach-tcc892x/board-m805_892x-pmic.c
 *
 * Copyright (C) 2012 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <mach/gpio.h>
#include <mach/bsp.h>
#include <asm/mach-types.h>
#include "devices.h"
#include "board-tcc8920.h"

#if defined(CONFIG_REGULATOR_AXP192)
#include <linux/regulator/axp192.h>

static struct regulator_init_data axp192_dcdc1_info = {
	.constraints = {
		.name = "vdd_dcdc1 range",
		.min_uV =  700000,
		.max_uV = 3500000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE|REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data axp192_dcdc2_info = {
	.constraints = {
		.name = "vdd_dcdc2 range",
		.min_uV =  700000,
		.max_uV = 2275000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data axp192_dcdc3_info = {
	.constraints = {
		.name = "vdd_dcdc3 range",
		.min_uV =  700000,
		.max_uV = 3500000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data axp192_ldo2_info = {
	.constraints = {
		.name = "vdd_ldo2 range",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data axp192_ldo3_info = {
	.constraints = {
		.name = "vdd_ldo3 range",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data axp192_ldo4_info = {
	.constraints = {
		.name = "vdd_ldo4 range",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct axp192_subdev_data axp192_subdev[] = {
	{
		.id   = AXP192_ID_DCDC1,
		.platform_data = &axp192_dcdc1_info,
	},
	{
		.id   = AXP192_ID_DCDC2,
		.platform_data = &axp192_dcdc2_info,
	},
	{
		.id   = AXP192_ID_DCDC3,
		.platform_data = &axp192_dcdc3_info,
	},
	{
		.id   = AXP192_ID_LDO2,
		.platform_data = &axp192_ldo2_info,
	},
	{
		.id   = AXP192_ID_LDO3,
		.platform_data = &axp192_ldo3_info,
	},
	{
		.id   = AXP192_ID_LDO4,
		.platform_data = &axp192_ldo4_info,
	},
};

static int axp192_irq_init(void)
{
	if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
		tcc_gpio_config(TCC_GPE(27), GPIO_FN(0)|GPIO_PULL_DISABLE);  // GPIOE[31]: input mode, disable pull-up/down
		tcc_gpio_config_ext_intr(PMIC_IRQ, EXTINT_GPIOE_27);

		gpio_request(TCC_GPE(27), "PMIC_IRQ");
		gpio_direction_input(TCC_GPE(27));
	}
	else {
		tcc_gpio_config(TCC_GPD(9), GPIO_FN(0)|GPIO_PULL_DISABLE);  // GPIOE[31]: input mode, disable pull-up/down
		tcc_gpio_config_ext_intr(PMIC_IRQ, EXTINT_GPIOD_09);

		gpio_request(TCC_GPD(9), "PMIC_IRQ");
		gpio_direction_input(TCC_GPD(9));
	}

	return 0;
}

static struct axp192_platform_data axp192_info = {
	.num_subdevs = ARRAY_SIZE(axp192_subdev),
	.subdevs     = axp192_subdev,
	.init_irq    = axp192_irq_init,
};
#endif

#if defined(CONFIG_REGULATOR_RN5T614)
#include <linux/regulator/rn5t614.h>

static struct regulator_init_data rn5t614_dcdc1_info = {
	.constraints = {
		.name = "vdd_dcdc1 range",
		.min_uV =  950000,
		.max_uV = 1500000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_dcdc2_info = {
	.constraints = {
		.name = "vdd_dcdc2 range",
		.min_uV =  950000,
		.max_uV = 1500000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_dcdc3_info = {
	.constraints = {
		.name = "vdd_dcdc3 range",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_ldo2_info = {
	.constraints = {
		.name = "vdd_ldo2 range",
		.min_uV =  900000,
		.max_uV = 1300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_ldo3_info = {
	.constraints = {
		.name = "vdd_ldo3 range",
		.min_uV =  900000,
		.max_uV = 1300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_ldo4_info = {
	.constraints = {
		.name = "vdd_ldo4 range",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_ldo5_info = {
	.constraints = {
		.name = "vdd_ldo5 range",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_ldo6_info = {
	.constraints = {
		.name = "vdd_ldo6 range",
		.min_uV = 1200000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_ldo7_info = {
	.constraints = {
		.name = "vdd_ldo7 range",
		.min_uV = 1200000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct regulator_init_data rn5t614_ldo8_info = {
	.constraints = {
		.name = "vdd_ldo8 range",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 0,
};

static struct rn5t614_subdev_data rn5t614_subdev[] = {
	{
		.id   = RN5T614_ID_DCDC1,
		.platform_data = &rn5t614_dcdc1_info,
	},
	{
		.id   = RN5T614_ID_DCDC2,
		.platform_data = &rn5t614_dcdc2_info,
	},
	{
		.id   = RN5T614_ID_DCDC3,
		.platform_data = &rn5t614_dcdc3_info,
	},
	{
		.id   = RN5T614_ID_LDO2,
		.platform_data = &rn5t614_ldo2_info,
	},
	{
		.id   = RN5T614_ID_LDO3,
		.platform_data = &rn5t614_ldo3_info,
	},
	{
		.id   = RN5T614_ID_LDO4,
		.platform_data = &rn5t614_ldo4_info,
	},
	{
		.id   = RN5T614_ID_LDO5,
		.platform_data = &rn5t614_ldo5_info,
	},
	{
		.id   = RN5T614_ID_LDO6,
		.platform_data = &rn5t614_ldo6_info,
	},
	{
		.id   = RN5T614_ID_LDO7,
		.platform_data = &rn5t614_ldo7_info,
	},
	{
		.id   = RN5T614_ID_LDO8,
		.platform_data = &rn5t614_ldo8_info,
	},
};

static int rn5t614_port_init(int irq_num)
{
	if (irq_num) {
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			tcc_gpio_config(TCC_GPE(27), GPIO_FN(0)|GPIO_PULL_DISABLE);  // GPIOE[31]: input mode, disable pull-up/down
			tcc_gpio_config_ext_intr(irq_num, EXTINT_GPIOE_27);
		
			gpio_request(TCC_GPE(27), "PMIC_IRQ");
			gpio_direction_input(TCC_GPE(27));
		}
		else {
			tcc_gpio_config(TCC_GPF(24), GPIO_FN(0)|GPIO_PULL_DISABLE);
			tcc_gpio_config_ext_intr(irq_num, EXTINT_GPIOF_24);

			gpio_request(TCC_GPF(24), "PMIC_IRQ");
			gpio_direction_input(TCC_GPF(24));
		}
	}

	return 0;
}

static struct rn5t614_platform_data rn5t614_info = {
	.num_subdevs   = ARRAY_SIZE(rn5t614_subdev),
	.subdevs       = rn5t614_subdev,
	.init_port     = rn5t614_port_init,
};
#endif

static struct i2c_board_info __initdata i2c_pmic_devices[] = {
#if defined(CONFIG_REGULATOR_AXP192)
	{
		I2C_BOARD_INFO("axp192", 0x34),
		.irq           = PMIC_IRQ,
		.platform_data = &axp192_info,
	},
#endif
#if defined(CONFIG_REGULATOR_RN5T614)
	{
		I2C_BOARD_INFO("rn5t614", 0x32),
		.irq           = PMIC_IRQ,
		.platform_data = &rn5t614_info,
	},
#endif
};

static struct regulator_consumer_supply consumer_core = {
	.supply = "vdd_core",
};

static struct regulator_consumer_supply consumer_wifi = {
	.supply = "vdd_wifi",
};

static struct regulator_consumer_supply consumer_dxb = {
	.supply = "vdd_dxb",
};

static struct regulator_consumer_supply consumer_cam_iod = {
	.supply = "vdd_cam",
};

static struct regulator_consumer_supply consumer_lvds_12d = {
	.supply = "vdd_lvds_12",
};

static struct regulator_consumer_supply consumer_hdmi_pll = {
	.supply = "vdd_hdmi_pll",
};

static struct regulator_consumer_supply consumer_hdmi_osc = {
	.supply = "vdd_hdmi_osc",
};

static struct regulator_consumer_supply consumer_iod0 = {
	.supply = "vdd_iod0",
};

static struct regulator_consumer_supply consumer_iod1 = {
	.supply = "vdd_iod1",
};

void __init m805_892x_init_pmic(void)
{
	if (!machine_is_m805_892x())
		return;

#if defined(CONFIG_REGULATOR_AXP192)
	axp192_dcdc2_info.num_consumer_supplies = 1;
	axp192_dcdc2_info.consumer_supplies = &consumer_core;
	axp192_ldo2_info.num_consumer_supplies = 1;
	axp192_ldo2_info.consumer_supplies = &consumer_iod0;
	axp192_ldo3_info.num_consumer_supplies = 1;
	axp192_ldo3_info.consumer_supplies = &consumer_wifi;
	axp192_ldo4_info.num_consumer_supplies = 1;
	axp192_ldo4_info.consumer_supplies = &consumer_hdmi_osc;
#endif

#if defined(CONFIG_REGULATOR_RN5T614)
	rn5t614_dcdc1_info.num_consumer_supplies = 1;
	rn5t614_dcdc1_info.consumer_supplies = &consumer_core;

	if (system_rev == 0x2002) {
		rn5t614_ldo4_info.num_consumer_supplies = 1;
		rn5t614_ldo4_info.consumer_supplies = &consumer_iod0;
		rn5t614_ldo5_info.num_consumer_supplies = 1;
		rn5t614_ldo5_info.consumer_supplies = &consumer_wifi;
		rn5t614_ldo6_info.num_consumer_supplies = 1;
		rn5t614_ldo6_info.consumer_supplies = &consumer_iod1;
		rn5t614_ldo7_info.num_consumer_supplies = 1;
		rn5t614_ldo7_info.consumer_supplies = &consumer_dxb;
		rn5t614_ldo8_info.num_consumer_supplies = 1;
		rn5t614_ldo8_info.consumer_supplies = &consumer_cam_iod;
	}
	else if (system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
		rn5t614_ldo4_info.num_consumer_supplies = 1;
		rn5t614_ldo4_info.consumer_supplies = &consumer_iod0;
		rn5t614_ldo5_info.num_consumer_supplies = 1;
		rn5t614_ldo5_info.consumer_supplies = &consumer_wifi;
		rn5t614_ldo6_info.num_consumer_supplies = 1;
		rn5t614_ldo6_info.consumer_supplies = &consumer_lvds_12d;
		rn5t614_ldo7_info.num_consumer_supplies = 1;
		rn5t614_ldo7_info.consumer_supplies = &consumer_hdmi_pll;
		rn5t614_ldo8_info.num_consumer_supplies = 1;
		rn5t614_ldo8_info.consumer_supplies = &consumer_cam_iod;
	}
#endif

	i2c_register_board_info(0, i2c_pmic_devices, ARRAY_SIZE(i2c_pmic_devices));
}

//device_initcall(m805_892x_init_pmic);
