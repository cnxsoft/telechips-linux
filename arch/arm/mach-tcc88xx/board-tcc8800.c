/* linux/arch/arm/mach-tcc88xx/board-tcc8900.c
 *
 * Copyright (C) 2010 Telechips, Inc.
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
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/i2c/pca953x.h>
#include <linux/akm8975.h>
//#include <linux/usb/android_composite.h>      // Temp, Telechips B090183
#include <linux/spi/spi.h>
#include <linux/regulator/axp192.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>

#include <mach/gpio.h>
#include <mach/bsp.h>
#include <plat/serial.h>
#include <mach/tca_serial.h>
#include <mach/tca_i2c.h>
#include <mach/ohci.h>
#include <mach/tcc_fb.h>
#include <plat/tcc_ts.h>

#include <plat/nand.h>

#include "devices.h"
#include "board-tcc8800.h"

#if defined(CONFIG_RADIO_RDA5870)
#define RADIO_I2C_SLAVE_ID			(0x20>>1)
#endif

#if defined(CONFIG_FB_TCC_COMPONENT)
#include <linux/i2c/cs4954.h>
#endif

#if defined(CONFIG_FB_TCC_COMPONENT)
#define CS4954_I2C_SLAVE_ID 		(0x08>>1)
#define THS8200_I2C_SLAVE_ID 		(0x40>>1)

#endif // defined(CONFIG_FB_TCC_COMPONENT)


void __cpu_early_init(void);

extern void __init tcc_init_irq(void);
extern void __init tcc_map_common_io(void);
extern void __init tcc_reserve_sdram(void);


static struct spi_board_info tcc8800_spi0_board_info[] = {
	{
		.modalias = "spidev",
		.bus_num = 0,					// spi channel 0
		.chip_select = 0,
		/* you can modify the following member variables */
		.max_speed_hz = 2*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
};

static struct spi_board_info tcc8800_spi1_board_info[] = {
	{
		.modalias = "spidev",
		.bus_num = 1,					// spi channel 1
		.chip_select = 0,
		/* you can modify the following member variables */
		.max_speed_hz = 2*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
};

#if defined(CONFIG_SENSORS_AK8975)
static struct akm8975_platform_data akm8975_data = {
    .gpio_DRDY = 0,
};
#endif

#if defined(CONFIG_REGULATOR_AXP192)
static struct regulator_consumer_supply axp192_consumer_a = {
	.supply = "vdd_coreA",
};

static struct regulator_init_data axp192_dcdc2_info = {
	.constraints = {
		.name = "vdd_coreA range",
		.min_uV = 1050000,
		.max_uV = 1700000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies     = &axp192_consumer_a,
};

static struct regulator_consumer_supply axp192_consumer_b = {
	.supply = "vdd_coreB",
};

static struct regulator_init_data axp192_dcdc1_info = {
	.constraints = {
		.name = "vdd_coreB range",
		.min_uV = 1200000,
		.max_uV = 1700000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies     = &axp192_consumer_b,
};

static struct regulator_consumer_supply axp192_consumer_sata33 = {
	.supply = "vdd_sata33",
};

static struct regulator_init_data axp192_ldo4_info = {
	.constraints = {
		.name = "vdd_sata33",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies     = &axp192_consumer_sata33,
};

static struct axp192_subdev_data axp192_subdev[] = {
	{
		.name = "vdd_coreA",
		.id   = AXP192_ID_DCDC2,
		.platform_data = &axp192_dcdc2_info,
	},
#if 0
	{
		.name = "vdd_coreB",
		.id   = AXP192_ID_DCDC1,
		.platform_data = &axp192_dcdc1_info,
	},
#endif
	{
		.name = "vdd_sata33",
		.id   = AXP192_ID_LDO4,
		.platform_data = &axp192_ldo4_info,
	},
};

static int axp192_irq_init(int irq_num)
{
	tcc_gpio_config(TCC_GPF(24), GPIO_FN(0)|GPIO_PULL_DISABLE);
	tcc_gpio_config_ext_intr(irq_num, EXTINT_GPIOF_24);

	gpio_request(TCC_GPF(24), "PMIC_IRQ");
	gpio_direction_input(TCC_GPF(24));

	return 0;
}

static struct axp192_platform_data axp192_info = {
	.num_subdevs = 2,
	.subdevs     = axp192_subdev,
	.init_irq    = axp192_irq_init,
};
#endif

#if defined(CONFIG_FB_TCC_COMPONENT)
static struct cs4954_i2c_platform_data  cs4954_i2c_data = {
};
static struct cs4954_i2c_platform_data  ths8200_i2c_data = {
};
#endif

#if defined(CONFIG_I2C_TCC_CORE0)
static struct i2c_board_info __initdata i2c_devices0[] = {
	#if defined(CONFIG_RADIO_RDA5870)
	{
		I2C_BOARD_INFO("RDA5870E-FM", RADIO_I2C_SLAVE_ID),
		.platform_data = NULL,
	},
	#endif
    #if defined(CONFIG_SENSORS_AK8975)
    {
        I2C_BOARD_INFO("akm8975", 0x0F),
        .irq           = IRQ_EI1,
        .platform_data = &akm8975_data,
    },
    #endif
	#if defined(CONFIG_REGULATOR_AXP192)
	{
		I2C_BOARD_INFO("axp192", 0x34),
		.irq           = PMIC_IRQ,
		.platform_data = &axp192_info,
	},
	#endif

	#if defined(CONFIG_FB_TCC_COMPONENT)
	{
		I2C_BOARD_INFO("component-cs4954", CS4954_I2C_SLAVE_ID),
		.platform_data = &cs4954_i2c_data,
	},
	{
		I2C_BOARD_INFO("component-ths8200", THS8200_I2C_SLAVE_ID),
		.platform_data = &ths8200_i2c_data,
	}, //CONFIG_FB_TCC_COMPONENT
	#endif

};
#endif

#if defined(CONFIG_I2C_TCC_CORE1)
static struct i2c_board_info __initdata i2c_devices1[] = {
#if defined(CONFIG_TOUCHSCREEN_TCC_AK4183)
	{
		I2C_BOARD_INFO("tcc-ts-ak4183", 0x48),
		.platform_data = NULL,
	},
#endif
#if defined(CONFIG_TOUCHSCREEN_TCC_SITRONIX)
	{
		I2C_BOARD_INFO("tcc-ts-sitronix", 0x55),
		.platform_data = NULL,
	},
#endif
};	
#endif

#if defined(CONFIG_I2C_TCC_SMU)
static struct i2c_board_info __initdata i2c_devices_smu[] = {
};	
#endif

static struct tcc_i2c_platform_data tcc8800_core0_platform_data = {
    .core_clk_rate      = 4*1000*1000,    /* core clock rate: 4MHz */
    .core_clk_name      = "i2c0",
    .smu_i2c_flag       = 0,
    .i2c_ch_clk_rate[0] = 400,      /* SCL clock rate : 100kHz */
    .i2c_ch_clk_rate[1] = 400,      /* SCL clock rate : 100kHz */
};

static struct tcc_i2c_platform_data tcc8800_core1_platform_data = {
    .core_clk_rate      = 4*1000*1000,    /* core clock rate: 4MHz */
    .core_clk_name      = "i2c1",
    .smu_i2c_flag       = 0,
    .i2c_ch_clk_rate[0] = 100,      /* SCL clock rate : 100kHz */
    .i2c_ch_clk_rate[1] = 100,      /* SCL clock rate : 100kHz */
};

static struct tcc_i2c_platform_data tcc8800_smu_platform_data = {
    .core_clk_name  = "smu",
    .smu_i2c_flag   = 1,
};

extern void __init tcc8800_init_gpio(void);
extern void __init tcc8800_init_camera(void);

static void __init tcc8800_init_irq(void)
{
	tcc_init_irq();
}

static int gMultiCS = 0;

static void tcc8800_nand_init(void)
{
	unsigned int gpio_wp = GPIO_NAND_WP;
	unsigned int gpio_rdy0 = GPIO_NAND_RDY0;	
		
	tcc_gpio_config(gpio_wp, GPIO_FN(0));
	tcc_gpio_config(gpio_rdy0, GPIO_FN(0));	
	
	gpio_request(gpio_wp, "nand_wp");
	gpio_direction_output(gpio_wp, 1);

	gpio_request(gpio_rdy0, "nand_rdy0");
	gpio_direction_input(gpio_rdy0);
}

static int tcc8800_nand_ready(void)
{
	if ( gMultiCS == TRUE )
		return ! ( ( gpio_get_value(GPIO_NAND_RDY0) ) && ( gpio_get_value(GPIO_NAND_RDY1) ) );
	else
		return !gpio_get_value(GPIO_NAND_RDY0);	
}

static void tcc8800_SetFlags( int bMultiCS )
{
	gMultiCS = bMultiCS;

	if( gMultiCS == TRUE )
	{
		unsigned int gpio_rdy1 = GPIO_NAND_RDY1;

		tcc_gpio_config(gpio_rdy1, GPIO_FN(0));
		gpio_request(gpio_rdy1, "nand_rdy1");
		gpio_direction_input(gpio_rdy1);
	}
}

static struct tcc_nand_platform_data tcc_nand_platdata = {
	.parts		= NULL,
	.nr_parts	= 0,
	.gpio_wp	= GPIO_NAND_WP,
	.init		= tcc8800_nand_init,
	.ready		= tcc8800_nand_ready,
	.SetFlags	= tcc8800_SetFlags,
};

static struct platform_device tcc_nand_device = {
	.name		= "tcc_mtd",
	.id			= -1,
	.dev		= {
		.platform_data = &tcc_nand_platdata,
	},
};

static void tcc_add_nand_device(void)
{
	tcc_get_partition_info(&tcc_nand_platdata.parts, &tcc_nand_platdata.nr_parts);
	platform_device_register(&tcc_nand_device);
}



#if defined(CONFIG_TCC_WATCHDOG)
static struct platform_device tccwdt_device = {
	.name	= "tcc-wdt",
	.id		= -1,
};
#endif

#if defined(CONFIG_TCC_UART2_DMA)
static struct tcc_uart_platform_data uart2_data = {
    .tx_dma_use     = 0,
    .tx_dma_buf_size= SERIAL_TX_DMA_BUF_SIZE,
    .tx_dma_base    = HwGDMA2_BASE,
    .tx_dma_ch      = SERIAL_TX_DMA_CH_NUM,
    .tx_dma_intr    = INT_DMA2_CH0,
    .tx_dma_mode    = SERIAL_TX_DMA_MODE,

    .rx_dma_use     = 1,
    .rx_dma_buf_size= SERIAL_RX_DMA_BUF_SIZE,
    .rx_dma_base    = HwGDMA2_BASE,
    .rx_dma_ch      = SERIAL_RX_DMA_CH_NUM-2,
    .rx_dma_intr    = 0,
    .rx_dma_mode    = SERIAL_RX_DMA_MODE,
};
#endif

#if defined(CONFIG_TCC_UART3_DMA)
static struct tcc_uart_platform_data uart3_data = {
    .tx_dma_use     = 0,
    .tx_dma_buf_size= SERIAL_TX_DMA_BUF_SIZE,
    .tx_dma_base    = HwGDMA2_BASE,
    .tx_dma_ch      = SERIAL_TX_DMA_CH_NUM+1,
    .tx_dma_intr    = INT_DMA2_CH1,
    .tx_dma_mode    = SERIAL_TX_DMA_MODE,

    .rx_dma_use     = 1,
    .rx_dma_buf_size= SERIAL_RX_DMA_BUF_SIZE,
    .rx_dma_base    = HwGDMA2_BASE,
    .rx_dma_ch      = SERIAL_RX_DMA_CH_NUM-1,
    .rx_dma_intr    = 0,
    .rx_dma_mode    = SERIAL_RX_DMA_MODE,
};
#endif

#if defined(CONFIG_TCC_BT_DEV)
static struct tcc_uart_platform_data uart1_data_bt = {
    .bt_use         = 1,

    .tx_dma_use     = 0,
    .tx_dma_buf_size= SERIAL_TX_DMA_BUF_SIZE,
    .tx_dma_base    = HwGDMA2_BASE,
    .tx_dma_ch      = SERIAL_TX_DMA_CH_NUM+2,
    .tx_dma_intr    = INT_DMA2_CH2,
    .tx_dma_mode    = SERIAL_TX_DMA_MODE,

    .rx_dma_use     = 1,
    .rx_dma_buf_size= SERIAL_RX_DMA_BUF_SIZE,
    .rx_dma_base    = HwGDMA2_BASE,
    .rx_dma_ch      = SERIAL_RX_DMA_CH_NUM,
    .rx_dma_intr    = 0,
    .rx_dma_mode    = SERIAL_RX_DMA_MODE,
};
#endif

#if defined(CONFIG_TCC_BT_DEV)
static struct platform_device tcc_bluetooth_device = {	
	.name = "tcc_bluetooth_rfkill",	
	.id = -1,
};

static void tcc_add_bluetooth_device(void)
{
	platform_device_register(&tcc_bluetooth_device);
}
#endif


/*----------------------------------------------------------------------
 * Device	  : ADC touchscreen resource
 * Description: tcc8800_touchscreen_resource
 *----------------------------------------------------------------------*/
#if defined(CONFIG_TOUCHSCREEN_TCCTS) || defined(CONFIG_TOUCHSCREEN_TCCTS_MODULE)
static struct resource tcc8800_touch_resources[] = {
	[0] = {
		.start	= TCC_TSADC_BASE,
		.end	= TCC_TSADC_BASE + 0x20,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_TSADC,
		.end   = INT_TSADC,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = INT_EI2,
		.end   = INT_EI2,
		.flags = IORESOURCE_IRQ,
	},
};

static struct tcc_adc_ts_platform_data tcc_touchscreen_pdata = {
#ifdef CONFIG_TOUCHSCREEN_CALIBRATION
	.min_x = 1,
	.max_x = 4095,
	.min_y = 1,
	.max_y = 4095,
#else
	.min_x = 110,
	.max_x = 3990,
	.min_y = 250,
	.max_y = 3800,
#endif
};

static struct platform_device tcc8800_touchscreen_device = {
	.name		= "tcc-ts",
	.id		= -1,
	.resource	= tcc8800_touch_resources,
	.num_resources	= ARRAY_SIZE(tcc8800_touch_resources),
	.dev = {
		.platform_data = &tcc_touchscreen_pdata
	},
};

static void tcc_add_touchscreen_device(void)
{
	if (system_rev == 0x500 ) {
		if(tcc_panel_id == PANEL_ID_AT070TN93 || tcc_panel_id == PANEL_ID_TD070RDH11)
			tcc_touchscreen_pdata.reverse_x_pos = 1;
		else
			tcc_touchscreen_pdata.reverse_y_pos = 1;
	} else {
		if(tcc_panel_id == PANEL_ID_AT070TN93 || tcc_panel_id == PANEL_ID_TD070RDH11)
#if defined(CONFIG_PORTRAIT_LCD)
			tcc_touchscreen_pdata.swap_xy_pos = 1;
#else
			tcc_touchscreen_pdata.reverse_y_pos = 1;		
#endif
		else if(tcc_panel_id == PANEL_ID_LMS350DF01)
		{
			tcc_touchscreen_pdata.swap_xy_pos = 1;
			tcc_touchscreen_pdata.reverse_x_pos = 1;
			tcc_touchscreen_pdata.reverse_y_pos = 1;
		}	
		else
			tcc_touchscreen_pdata.reverse_x_pos = 1;
	}
	platform_device_register(&tcc8800_touchscreen_device);
}
#endif

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
static u64 tcc8800_device_ohci_dmamask = 0xffffffffUL;
 
static struct resource tcc8800_ohci_resources[] = {
	[0] = {
		.start = tcc_p2v(HwUSBHOST_BASE),
		.end   = tcc_p2v(HwUSBHOST_BASE) + 0x5C,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_UH11,
		.end   = INT_UH11,
		.flags = IORESOURCE_IRQ,
	}
};

static struct platform_device tcc8800_ohci_device = {
	.name			= "tcc-ohci",
	.id				= 0,
	.num_resources  = ARRAY_SIZE(tcc8800_ohci_resources),
	.resource       = tcc8800_ohci_resources,
	.dev			= {
		.dma_mask 			= &tcc8800_device_ohci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
};

static int tcc8800_ohci_init(struct device *dev)
{
	return 0;
}

static struct tccohci_platform_data tcc8800_ohci_platform_data = {
	.port_mode	= USBOHCI_PPM_PERPORT,
	.init			= tcc8800_ohci_init,
};

void __init tcc_set_ohci_info(struct tccohci_platform_data *info)
{
	tcc8800_ohci_device.dev.platform_data = info;
}

void __init tcc8800_init_usbhost(void)
{
	tcc_set_ohci_info(&tcc8800_ohci_platform_data);
}
#endif /* CONFIG_USB_OHCI_HCD */

static struct platform_device *tcc8800_devices[] __initdata = {
	&tcc8800_uart0_device,
	&tcc8800_uart1_device,
#if defined(CONFIG_GPS_JGR_SC3_S)
	&tcc8800_uart3_device,
#endif

#if defined(CONFIG_TCC_ECID_SUPPORT)
	&tcc_cpu_id_device,
#endif

	&tcc8800_adc_device,
#if defined(CONFIG_BATTERY_TCC)
	&tcc_battery_device,
#endif
#if defined(CONFIG_RTC_DRV_TCC8800)
	&tcc8800_rtc_device,
#endif
#if defined(CONFIG_INPUT_TCC_REMOTE)
	&tcc8800_remote_device,
#endif
#if defined(CONFIG_I2C_TCC_CORE0)
    &tcc8800_i2c_core0_device,
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
    &tcc8800_i2c_core1_device,
#endif
#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	&tcc8800_ohci_device,
#endif
#if defined(CONFIG_TCC_DWC_OTG)  || defined(CONFIG_TCC_DWC_OTG_MODULE)
	&tcc8800_dwc_otg_device,	
#endif	
#if defined(CONFIG_USB_EHCI_TCC)  || defined(CONFIG_USB_EHCI_HCD_MODULE)
	&ehci_hs_device,
	&ehci_fs_device,
#endif
#if defined(CONFIG_SATA_AHCI)
	&tcc_ahci_device,
#endif
#if defined(CONFIG_I2C_TCC_SMU)
    &tcc8800_i2c_smu_device,
#endif
#if defined(CONFIG_SPI_TCCXXXX_MASTER)
	&tcc8800_spi0_device,
#endif
#if defined(CONFIG_SPI_TCCXXXX_TSIF_SLAVE)
	&tcc_tsif_device,
	&tcc_tsif_ex_device,
#endif
#if defined(CONFIG_TCC_GMAC) || defined(CONFIG_TCC_GMAC_MODULE)
	&tcc_gmac_device,
#endif

//	&usb_mass_storage_device,   // temp, Telechips B090183
//	&android_usb_device,        // temp, Telechips B090183
#if defined(CONFIG_TCC_WATCHDOG)
	&tccwdt_device,
#endif
};

static int __init board_serialno_setup(char *serialno)
{
//	android_usb_pdata.serial_number = serialno;     // temp, Telechips B090183

	return 1;
}
__setup("androidboot.serialno=", board_serialno_setup);

char* atheros_wifi_mac;

static int __init board_wifimac_setup(char *wifimac)
{
	atheros_wifi_mac = wifimac;

	return 1;
}
__setup("androidboot.wifimac=", board_wifimac_setup);

EXPORT_SYMBOL(atheros_wifi_mac);

static int __init board_btaddr_setup(char *btaddr)
{
	return 1;
}
__setup("androidboot.btaddr=", board_btaddr_setup);

static void __init tcc8800_init_machine(void)
{
	__cpu_early_init();

	tcc8800_init_gpio();
	tcc8800_init_camera();
	
#if defined(CONFIG_SPI_TCCXXXX_MASTER)
	spi_register_board_info(tcc8800_spi0_board_info, ARRAY_SIZE(tcc8800_spi0_board_info));
	//spi_register_board_info(tcc8800_spi1_board_info, ARRAY_SIZE(tcc8800_spi1_board_info)); //jhlim
#endif

#if defined(CONFIG_SENSORS_AK8975)
    /* Input mode */
    tcc_gpio_config(TCC_GPF(26), GPIO_FN(0)|GPIO_PULL_DISABLE);  // GPIOF[26]: input mode, disable pull-up/down
    tcc_gpio_direction_input(TCC_GPF(26));
    tcc_gpio_config_ext_intr(INT_EI1, EXTINT_GPIOF_26);
#endif

#if defined(CONFIG_I2C_TCC_CORE0)
	i2c_register_board_info(0, i2c_devices0, ARRAY_SIZE(i2c_devices0));
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
	i2c_register_board_info(1, i2c_devices1, ARRAY_SIZE(i2c_devices1));
#endif
#if defined(CONFIG_I2C_TCC_SMU)
	i2c_register_board_info(2, i2c_devices_smu, ARRAY_SIZE(i2c_devices_smu));
#endif

#if CONFIG_TCC_BT_DEV
	/* BT: use UART1 and TX DMA */
	platform_device_add_data(&tcc8800_uart1_device, &uart1_data_bt, sizeof(struct tcc_uart_platform_data));
#endif

#if defined(CONFIG_TCC_BT_DEV)
	tcc_add_bluetooth_device();
#endif

#if CONFIG_TCC_UART2_DMA
	platform_device_add_data(&tcc8800_uart2_device, &uart2_data, sizeof(struct tcc_uart_platform_data));
#endif

#if CONFIG_TCC_UART3_DMA
	platform_device_add_data(&tcc8800_uart3_device, &uart3_data, sizeof(struct tcc_uart_platform_data));
#endif

#if defined(CONFIG_TOUCHSCREEN_TCCTS) || defined(CONFIG_TOUCHSCREEN_TCCTS_MODULE)
	tcc_add_touchscreen_device();
#endif

#if defined(CONFIG_I2C_TCC_CORE0)
    platform_device_add_data(&tcc8800_i2c_core0_device, &tcc8800_core0_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
	platform_device_add_data(&tcc8800_i2c_core1_device, &tcc8800_core1_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
#if defined(CONFIG_I2C_TCC_SMU)
	platform_device_add_data(&tcc8800_i2c_smu_device, &tcc8800_smu_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
#if defined( CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	platform_device_add_data(&tcc8800_ohci_device, &tcc8800_ohci_platform_data, sizeof(struct tccohci_platform_data));
#endif

	tcc_add_nand_device();

	platform_add_devices(tcc8800_devices, ARRAY_SIZE(tcc8800_devices));
}


static void __init tcc8800_map_io(void)
{
	tcc_map_common_io();
}

static void __init tcc8800_mem_reserve(void)
{
    tcc_reserve_sdram();
}

extern struct sys_timer tcc_timer;

MACHINE_START(TCC8800, "tcc8800")
	/* Maintainer: Telechips Linux BSP Team <linux@telechips.com> */
//	.phys_io        = 0xf0000000,
//	.io_pg_offst    = ((0xf0000000) >> 18) & 0xfffc,
	.boot_params    = PHYS_OFFSET + 0x00000100,
	.reserve        = tcc8800_mem_reserve,
	.map_io         = tcc8800_map_io,
	.init_irq       = tcc8800_init_irq,
	.init_machine   = tcc8800_init_machine,
	.timer          = &tcc_timer,
MACHINE_END
