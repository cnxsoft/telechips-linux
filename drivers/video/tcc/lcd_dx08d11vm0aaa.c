/*
 * dx08d11vm0aaa_lcd.c -- support for Hitachi DX08D11VM0AAA LCD
 *
 * Copyright (C) 2009, 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <asm/mach-types.h>

#include <mach/tcc_fb.h>
#include <mach/gpio.h>
#include <mach/tca_lcdc.h>
#include <mach/TCC_LCD_DriverCtrl.h>
#include <mach/TCC_LCD_Interface.h>
#include <linux/delay.h>
#include <mach/dev_gpsb.h>

#define TCC_SPI_DIRECTCONTROL
#define WAIT_FLAG	0xFFFF
#define END_FLAG	0xFFFE

extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);


static const struct i2c_device_id dx08d11vm0aaa_id[] = {
	{ "dx08d11vm0aaa_bl", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, dx08d11vm0aaa_id);

#include <mach/dev_gpsb.h>
static DEV_LCD_SPIO_Config_type lcd_spio = { 
	.clock = 60000,
	.data_length = 24,
	#if defined(CONFIG_ARCH_TCC88XX)
	.port = LCD_GPSB_PORT10,
	#else
	.port = LCD_GPSB_PORT9,
	#endif//
	.mode = 0
	| DRV_GPSB_MODE_OpMod_SPI 			// 0 : spi
	| DRV_GPSB_MODE_MasterMode			// 0 : Master mode enable	
	| DRV_GPSB_MODE_BitWidth(23)		// 31 : Bit width selection, 32bits = 31+1
	| DRV_GPSB_MODE_SCKPolCtrl_High		// 16
	| DRV_GPSB_MODE_EnableOperation		// 3
	| DRV_GPSB_MODE_RcvRisingEdge		// 17
	| DRV_GPSB_MODE_TransRisingEdge		// 18
	| DRV_GPSB_MODE_ClearRcvFifoCount	// 1 : Assert Rx FIFO Counter Clear
//	| DRV_GPSB_MODE_SCKPolCtrl_High		// 1 : SCKI inverted. for SPI timing 1, 2	
	,
};

unsigned short dx08d11vm0aaa_on_table[][2]=
{
		{0x002, 0x0000}, 
		{0x003, 0x0000},
		{0x004, 0x0000},
		{0x010, 0x0210}, 
		{0x020, 0x0000},
		{0x021, 0x2831},
		{0x022, 0x003E},
		{0x023, 0x7400},
		{0x024, 0x7400},
		{0x025, 0x6A06},
		{0x026, 0x7400},
		{0x027, 0x1C06},
		{0x028, 0x1C28},
		{0x029, 0x1C4A},
		{0x02A, 0x0666},
		{0x100, 0x0533},
		{0x101, 0x0003},
		{0x102, 0x3700},
#if 1		
		{0x300, 0x7777}, 
		{0x301, 0x0717},
		{0x302, 0xC114},
		{0x303, 0x0071}, 
		{0x304, 0x7110}, 
		{0x305, 0x01D4},
#else
		{0x300, 0x5757}, //0x7777
		{0x301, 0x0537}, //0x0717
		{0x302, 0xC114},
		{0x303, 0x0053}, //0x0071
		{0x304, 0x5130}, //0x7110
		{0x305, 0xC317}, //0x01D4
#endif
		{0x402, 0x0000},
		{0x501, 0xFFFF},
		{0x502, 0xFFFF},
		{0x503, 0xFFFF},
		{0x504, 0x00FF},
		{0x002, 0x0200},
		{WAIT_FLAG, 2},//wait 10ms
		{0x001, 0x0001},
		{WAIT_FLAG, 1},//wait 2ms
		{0x002, 0x8210},
		{WAIT_FLAG, 4},//wait 20ms
		{0x002, 0x8310},
		{WAIT_FLAG, 4},//wait 20ms
		{0x002, 0x0710},
		{WAIT_FLAG, 4},//wait 20ms
		{0x002, 0x1730},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001, 0x0012},
		{0x001, 0x0032},
		{0x023, 0x0000},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001, 0x0033},
		{0x023, 0x7400},
		{END_FLAG, 0x0}
};

unsigned short dx08d11vm0aaa_off_table[][2]=
{
		{0x203, 0x0000},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001 ,0x032},
		{0x023 ,0x7400},
		{WAIT_FLAG, 4},//wait 20ms
		{0x023, 0x0000},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001 ,0x0012},
		{WAIT_FLAG, 4},//wait 20ms
		{0x001 ,0x0000},
		{WAIT_FLAG, 1},//wait 1ms
		{0x002 ,0x0730},
		{0x100 ,0x0000},
		{0x002 ,0x0000},
		{WAIT_FLAG, 1},//wait 1ms
		{0x002 ,0x0001},
		{WAIT_FLAG, 1},//wait 1ms
		{END_FLAG, 0x0}
};


struct bl_info {
	unsigned gpio_start;
	uint16_t reg_output;
	uint16_t reg_direction;

	struct i2c_client *client;
	struct gpio_chip gpio_chip;
};


static void lcd_spi_write(unsigned int Reg, unsigned int Data)
{
	volatile unsigned int gReg = 0x100000;
	volatile unsigned int gWriteData = 0x120000;
	unsigned int value;
	
	value = gReg  | Reg;
	DEV_LCD_SPIO_TxRxData(&value, 0, 1);

	value = gWriteData | Data;
	DEV_LCD_SPIO_TxRxData(&value, 0, 1);
}

static int DX08D11VM0AAA_Module_On(void)
{
	int i = 0;

	DEV_LCD_SPIO_Config(&lcd_spio);
	while(dx08d11vm0aaa_on_table[i][0] != END_FLAG)
	{
		if(dx08d11vm0aaa_on_table[i][0] == WAIT_FLAG)
			msleep(dx08d11vm0aaa_on_table[i][1]);
		else
			lcd_spi_write(dx08d11vm0aaa_on_table[i][0], dx08d11vm0aaa_on_table[i][1]);

		i++;			
	}
	return 1;
}


void DX08D11VM0AAA_Module_Off(void)
{
        int i = 0;
		DEV_LCD_SPIO_Config(&lcd_spio);

        while(dx08d11vm0aaa_off_table[i][0] != END_FLAG)
        {
                if(dx08d11vm0aaa_off_table[i][0] == WAIT_FLAG)
                        msleep(dx08d11vm0aaa_off_table[i][1]);
                else
                        lcd_spi_write(dx08d11vm0aaa_off_table[i][0], dx08d11vm0aaa_off_table[i][1]);

                i++;
        }
}

static int dx08d11vm0aaa_bl_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct bl_info *info;

	info = kzalloc(sizeof(struct bl_info), GFP_KERNEL);
	if (info == NULL) {
		printk("dx08d11vm0aaa_bl: no memory for i2c\n");
		return -ENOMEM;
	}

	info->client = client;
	i2c_set_clientdata(client, info);

	return 0;
}

static int dx08d11vm0aaa_bl_remove(struct i2c_client *client)
{
	struct bl_info *info = i2c_get_clientdata(client);

	kfree(info);

	return 0;
}

static int dx08d11vm0aaa_panel_init(struct lcd_panel *panel)
{
	return 0;
}

static int dx08d11vm0aaa_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;

	printk("dx08d11vm0aaa_set_power: %d\n", on);
		
	if (on) {
		gpio_set_value(pdata->power_on, 1);
//		gpio_set_value(pdata->reset, 1);
		udelay(10);

		DX08D11VM0AAA_Module_On();
		msleep(16);

		lcdc_initialize(panel, lcd_num);
		LCDC_IO_Set(1, panel->bus_width);
		msleep(16);

		gpio_set_value(pdata->display_on, 1);

	} else {
		gpio_set_value(pdata->display_on, 0);
		udelay(10);

		LCDC_IO_Disable(1, panel->bus_width);
		DX08D11VM0AAA_Module_Off();

//		gpio_set_value(pdata->reset, 0);
		gpio_set_value(pdata->power_on, 0);


	}
	return 0;
}

typedef struct{
	char reg;
	char value;
} BD6083GUL_CTRL_TYPE;

BD6083GUL_CTRL_TYPE BD6083_init[] = {
	{0x00, 0x01}, {0x01, 0x0E}, {0x03, 0x63},
	{0x09, 0x00},{0x02, 0x01}
};
BD6083GUL_CTRL_TYPE BD6083_NormaleToALCD[] = {
	{0x01, 0x0F}, {0x09, 0x87},	{0x0A, 0x01},
	{0x0B, 0x01}, {0x0D, 0x13},	{0x0E, 0x63}, {0x01, 0x0F}
};
BD6083GUL_CTRL_TYPE BD6083_ALCToNormal[] = {
	{0x01, 0x0E},	{0x09, 0x00},{0x02, 0x01}
};
BD6083GUL_CTRL_TYPE BD6083_NormalToDiming[] = { {0x03, 0x04} };
BD6083GUL_CTRL_TYPE BD6083_DimingToSleep[] = { {0x02, 0x00} };
BD6083GUL_CTRL_TYPE BD6083_SleepToNomal[] = { {0x06, 0x63}, {0x02, 0x01} };
BD6083GUL_CTRL_TYPE BD6083_DimingToNomal[] = { {0x03, 0x63} };

void bl_write(struct i2c_client *client, BD6083GUL_CTRL_TYPE *ctrl, unsigned size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (ctrl[i].reg == 0xFF)
			udelay(ctrl[i].value);
		else
			i2c_smbus_write_byte_data(client, ctrl[i].reg, ctrl[i].value);
	}
}

static char BD6083_state = 1;
static int dx08d11vm0aaa_set_backlight_level(struct lcd_panel *panel, int level)
{

	struct platform_device *pdev = container_of(panel->dev, struct platform_device, dev);
	struct i2c_client *i2c = dev_get_drvdata(&pdev->dev);

	if (level) {
		if (!BD6083_state) {
			bl_write(i2c, BD6083_init, ARRAY_SIZE(BD6083_init));
		} else {
			BD6083_NormalToDiming[0].value = level >> 1;
			bl_write(i2c, BD6083_NormalToDiming, ARRAY_SIZE(BD6083_NormalToDiming));
		}
	} else {
		BD6083_state = 0;
		bl_write(i2c, BD6083_DimingToSleep, ARRAY_SIZE(BD6083_DimingToSleep));
	}
	return 0;
}

static struct lcd_panel dx08d11vm0aaa_panel = {
	.name		= "DX08D11VM0AAA",
	.manufacturer	= "Hitachi",
	.xres		= 480,
	.yres		= 800,
	.width		= 45,
	.height		= 76,
	.bpp		= 32,
	.clk_freq	= 230000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 4,
	.lpc		= 480,
	.lswc		= 8,
	.lewc		= 5,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 4,
	.flc1		= 800,
	.fswc1		= 2,
	.fewc1		= 11,
	.fpw2		= 4,
	.flc2		= 800,
	.fswc2		= 2,
	.fewc2		= 11,
	.sync_invert	= ID_INVERT | IV_INVERT | IH_INVERT | IP_INVERT,
	.init		= dx08d11vm0aaa_panel_init,
	.set_power	= dx08d11vm0aaa_set_power,
	.set_backlight_level = dx08d11vm0aaa_set_backlight_level,
};

static int dx08d11vm0aaa_probe(struct platform_device *pdev)
{
	struct i2c_adapter *adap = i2c_get_adapter(1);
	struct i2c_client *i2c;
	struct i2c_board_info dx08d11vm0aaa_bl = {
		I2C_BOARD_INFO("dx08d11vm0aaa_bl", 0xec >> 1),
		.platform_data = &dx08d11vm0aaa_panel,
	};

	#ifdef CONFIG_ARCH_TCC88XX
	adap = i2c_get_adapter(1);
	#else	
	adap = i2c_get_adapter(0);
	#endif//
	/* TODO: add channel configuration */
	i2c = i2c_new_device(adap , &dx08d11vm0aaa_bl);
	if (!i2c)
		return -EINVAL;


	dev_set_drvdata(&pdev->dev, i2c);

	dx08d11vm0aaa_panel.dev = &pdev->dev;

	tccfb_register_panel(&dx08d11vm0aaa_panel);
	return 0;
}

static int dx08d11vm0aaa_remove(struct platform_device *spi)
{
	return 0;
}

static struct i2c_driver dx08d11vm0aaa_bl = {
	.probe		= dx08d11vm0aaa_bl_probe,
	.remove		= dx08d11vm0aaa_bl_remove,
	.driver = {
		.name	= "dx08d11vm0aaa_bl",
		.owner	= THIS_MODULE,
	},
	.id_table	= dx08d11vm0aaa_id,
};

static struct platform_driver dx08d11vm0aaa_lcd = {
	.probe		= dx08d11vm0aaa_probe,
	.remove		= dx08d11vm0aaa_remove,
	.driver = {
		.name	= "dx08d11vm0aaa_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int dx08d11vm0aaa_init(void)
{
	i2c_add_driver(&dx08d11vm0aaa_bl);

	return platform_driver_register(&dx08d11vm0aaa_lcd);
}

static __exit void dx08d11vm0aaa_exit(void)
{
	i2c_del_driver(&dx08d11vm0aaa_bl);

	return platform_driver_unregister(&dx08d11vm0aaa_lcd);
}

subsys_initcall(dx08d11vm0aaa_init);
module_exit(dx08d11vm0aaa_exit);

MODULE_DESCRIPTION("DX08D11VM0AAA LCD driver");
MODULE_LICENSE("GPL");
