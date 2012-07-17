/*
 * td043mgeb1_lcd.c -- support for TPO TD043MGEB1 LCD
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
#include <linux/mutex.h>

#include <mach/hardware.h>
#include <mach/bsp.h>
#include <asm/io.h>
#include <asm/mach-types.h>

#include <mach/tcc_fb.h>
#include <mach/gpio.h>
#include <mach/tca_lcdc.h>
#include <mach/TCC_LCD_Interface.h>
#include <linux/delay.h>

#include <mach/dev_gpsb.h>

static struct mutex panel_lock;

extern void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num);


#define WAIT_FLAG	0xFF
#define END_FLAG	0xFE
unsigned char td043mgeb1_on_table[][2]=
{
	{0x02, 0x07}, 
	{0x03, 0x5F},
	{WAIT_FLAG, 1},//wait 5ms
	{0x04, 0x17},
	{0x05, 0x20}, 
	{0x06, 0x08}, 
	{0x07, 0x20}, 
	{0x08, 0x20}, 
	{0x09, 0x20}, 
	{0x0A, 0x20}, 
	{0x0B, 0x20}, 
	{0x0C, 0x20}, 
	{0x0D, 0x20}, 
	{0x0E, 0x10}, 
	{0x0F, 0x10}, 
	{0x10, 0x10}, 
	{0x11, 0x15}, 
	{0x12, 0xAA}, 
	{0x13, 0xFF}, 
	{0x14, 0x86}, 
	{0x15, 0x8d}, 
	{0x16, 0xd4}, 
	{0x17, 0xfb}, 
	{0x18, 0x2e}, 
	{0x19, 0x5a}, 
	{0x1A, 0x89}, 
	{0x1B, 0xfe}, 
	{0x1C, 0x5a}, 
	{0x1D, 0x9b}, 
	{0x1E, 0xc5}, 
	{0x1F, 0xFF}, 
	{0x20, 0xF0}, 
	{0x21, 0xF0}, 
	{0x22, 0x08}, 
	{WAIT_FLAG, 4},//wait 20ms
	{END_FLAG, 0x0}
};


unsigned char td043mgeb1_off_table[][2]=
{
	{0x03, 0x00},
	{END_FLAG, 0x0}
};

void GPSB_Port_disable(void)
{
	BITCSET(HwGPIOC->GPFN3, HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN7_MASK | HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN4(0)| HwPORTCFG_GPFN6(0)|HwPORTCFG_GPFN7(0));

	BITCSET(HwGPIOC->GPEN, Hw28|Hw30|Hw31, Hw28|Hw30|Hw31);
	BITCLR(HwGPIOC->GPDAT, Hw28|Hw30|Hw31);
}


void M57TE_SPI_Set_Data(unsigned int Reg, unsigned int Data)
{
	unsigned int value;

	value = (Reg << 10) | Data;
	DEV_LCD_SPIO_TxRxData(&value, 0, 1);
	ndelay(20);
}


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
	| DRV_GPSB_MODE_BitWidth(15)		// 31 : Bit width selection, 32bits = 31+1
	| DRV_GPSB_MODE_SCKPolCtrl_High		// 16
	| DRV_GPSB_MODE_EnableOperation		// 3
	| DRV_GPSB_MODE_RcvRisingEdge		// 17
	| DRV_GPSB_MODE_TransRisingEdge		// 18
	| DRV_GPSB_MODE_ClearRcvFifoCount	// 1 : Assert Rx FIFO Counter Clear
//	| DRV_GPSB_MODE_SCKPolCtrl_High		// 1 : SCKI inverted. for SPI timing 1, 2	
				,
};

void Module_device_init(void)
{
	int i = 0;


	DEV_LCD_SPIO_Config(&lcd_spio);

	while(td043mgeb1_on_table[i][0] != END_FLAG)
	{
		if(td043mgeb1_on_table[i][0] == WAIT_FLAG)
			msleep(td043mgeb1_on_table[i][1]);
		else
			M57TE_SPI_Set_Data(td043mgeb1_on_table[i][0], td043mgeb1_on_table[i][1]);

		i++;			
	}
	GPSB_Port_disable();

}



void Module_device_Off(void)
{
	int i = 0;

	DEV_LCD_SPIO_Config(&lcd_spio);

	while(td043mgeb1_on_table[i][0] != END_FLAG)
	{
		if(td043mgeb1_on_table[i][0] == WAIT_FLAG)
			msleep(td043mgeb1_off_table[i][1]);
		else
			M57TE_SPI_Set_Data(td043mgeb1_off_table[i][0], td043mgeb1_off_table[i][1]);

		i++;			
	}
	GPSB_Port_disable();

}

static int td043mgeb1_panel_init(struct lcd_panel *panel)
{
	printk("%s : %d\n", __func__, 0);
	return 0;
}

static int td043mgeb1_set_power(struct lcd_panel *panel, int on, unsigned int lcd_num)
{
	struct lcd_platform_data *pdata = panel->dev->platform_data;
	printk("%s : %d \n", __func__, on);

	mutex_lock(&panel_lock);
	panel->state = on;
	
	if (on) {
		gpio_set_value(pdata->power_on, 0);

		gpio_set_value(pdata->reset, 1);
		udelay(10);
		gpio_set_value(pdata->power_on, 1);
		udelay(100);
		gpio_set_value(pdata->display_on, 1);
		msleep(1);
		LCDC_IO_Set(1, panel->bus_width);
		lcdc_initialize(panel, lcd_num);
	
		Module_device_init();		
		msleep(16);
		
	}
	else 
	{
		Module_device_Off();		

		gpio_set_value(pdata->display_on, 0);
		msleep(10);
		gpio_set_value(pdata->reset, 0);
		if (!machine_is_m57te())
			gpio_set_value(pdata->power_on, 0);

		LCDC_IO_Disable(1, panel->bus_width);
	}

	mutex_unlock(&panel_lock);
	
	if(on)
		panel->set_backlight_level(panel , panel->bl_level);

	return 0;
}

static int td043mgeb1_set_backlight_level(struct lcd_panel *panel, int level)
{

	#define MAX_BL_LEVEL	255	
	volatile PTIMER pTIMER;

	struct lcd_platform_data *pdata = panel->dev->platform_data;

//	printk("%s : %d\n", __func__, level);
	
	panel->bl_level = level;

	if((panel->state == 0) && (level != 0))
		return 0;
	mutex_lock(&panel_lock);

	if (level == 0) {
		tcc_gpio_config(pdata->bl_on, GPIO_FN(0));
		gpio_set_value(pdata->bl_on, 0);
	} else {
		if(panel->state)
		{
			tcc_gpio_config(pdata->bl_on, GPIO_FN(0));
			gpio_set_value(pdata->bl_on, 1);
		}
	}

	mutex_unlock(&panel_lock);

	return 0;
}

static struct lcd_panel td043mgeb1_panel = {
	.name		= "TD043MGEB1",
	.manufacturer	= "TPO",
	.id		= PANEL_ID_TD043MGEB1,
	.xres		= 800,
	.yres		= 480,
	.width		= 92,
	.height		= 56,
	.bpp		= 24,
	.clk_freq	= 340000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 0,
	.lpc		= 800,
	.lswc		= 215,
	.lewc		= 37,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 0,
	.flc1		= 480,
	.fswc1		= 34,
	.fewc1		= 9,
	.fpw2		= 0,
	.flc2		= 480,
	.fswc2		= 34,
	.fewc2		= 9,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= td043mgeb1_panel_init,
	.set_power	= td043mgeb1_set_power,
	.set_backlight_level = td043mgeb1_set_backlight_level,
};

static int td043mgeb1_probe(struct platform_device *pdev)
{
	printk("%s : %d\n", __func__, 0);

	mutex_init(&panel_lock);
	td043mgeb1_panel.state = 1;
	td043mgeb1_panel.dev = &pdev->dev;
	
	tccfb_register_panel(&td043mgeb1_panel);
	return 0;
}

static int td043mgeb1_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver td043mgeb1_lcd = {
	.probe	= td043mgeb1_probe,
	.remove	= td043mgeb1_remove,
	.driver	= {
		.name	= "td043mgeb1_lcd",
		.owner	= THIS_MODULE,
	},
};

static __init int td043mgeb1_init(void)
{
	return platform_driver_register(&td043mgeb1_lcd);
}

static __exit void td043mgeb1_exit(void)
{
	platform_driver_unregister(&td043mgeb1_lcd);
}

subsys_initcall(td043mgeb1_init);
module_exit(td043mgeb1_exit);

MODULE_DESCRIPTION("TD043MGEB1 LCD driver");
MODULE_LICENSE("GPL");
