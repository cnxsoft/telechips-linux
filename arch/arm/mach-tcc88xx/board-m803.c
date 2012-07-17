/* linux/arch/arm/mach-tcc88xx/board-m803.c
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
#include <linux/usb/android_composite.h>
#include <linux/spi/spi.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/axp192.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>

#include <plat/serial.h>
#include <mach/tca_serial.h>
#include <mach/gpio.h>
#include <mach/bsp.h>
#include <mach/tca_i2c.h>
#include <plat/tcc_ts.h>
#include <plat/nand.h>

#include "devices.h"
#include "board-m803.h"

void __cpu_early_init(void);

extern void __init tcc_init_irq(void);
extern void __init tcc_map_common_io(void);

static struct spi_board_info m803_spi0_board_info[] = {
	{
		.modalias = "spidev",
		.bus_num = 0,					// spi channel 0
		.chip_select = 0,
		/* you can modify the following member variables */
		.max_speed_hz = 2*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
};
#if 0
static struct spi_board_info m801_88_spi1_board_info[] = {
	{
		.modalias = "spidev",
		.bus_num = 1,					// spi channel 1
		.chip_select = 0,
		/* you can modify the following member variables */
		.max_speed_hz = 2*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
};
#endif

/*----------------------------------------------------------------------
 * Device	  : ADC touchscreen resource
 * Description: tcc8800_touchscreen_resource
 *----------------------------------------------------------------------*/
#if defined(CONFIG_TOUCHSCREEN_TCCTS) || defined(CONFIG_TOUCHSCREEN_TCCTS_MODULE)
static struct resource m801_touch_resources[] = {
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

static struct tcc_adc_ts_platform_data m801_touchscreen_pdata = {
#ifdef CONFIG_TOUCHSCREEN_CALIBRATION
	.min_x = 1,
	.max_x = 4095,
	.min_y = 1,
	.max_y = 4095,
#else
	.min_x = 350,
	.max_x = 3650,
	.min_y = 420,
	.max_y = 3680,
#endif

#ifdef CONFIG_PORTRAIT_LCD
	.swap_xy_pos = 1,
	.reverse_y_pos = 1,
#endif
	.reverse_x_pos = 1,
};

static struct platform_device m801_touchscreen_device = {
	.name		= "tcc-ts",
	.id		= -1,
	.resource	= m801_touch_resources,
	.num_resources	= ARRAY_SIZE(m801_touch_resources),
	.dev = {
		.platform_data = &m801_touchscreen_pdata
	},
};
#endif

static struct akm8975_platform_data akm8975_data = {
    .gpio_DRDY = 0,
};

#if defined(CONFIG_REGULATOR_AXP192)
static struct regulator_consumer_supply axp192_consumer_a = {
	.supply = "vdd_coreA",
};

static struct regulator_init_data axp192_dcdc2_info = {
	.constraints = {
		.name = "vdd_coreA range",
		.min_uV =  950000,
		.max_uV = 1700000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies     = &axp192_consumer_a,
};

static struct regulator_consumer_supply axp192_consumer_hdmi_osc = {
	.supply = "vdd_hdmi_osc",
};

static struct regulator_init_data axp192_ldo4_info = {
	.constraints = {
		.name = "vdd_hdmi_osc",
		.min_uV = 1800000,
		.max_uV = 3300000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies     = &axp192_consumer_hdmi_osc,
};

static struct axp192_subdev_data axp192_subdev[] = {
	{
		.name = "vdd_coreA",
		.id   = AXP192_ID_DCDC2,
		.platform_data = &axp192_dcdc2_info,
	},
	{
		.name = "vdd_hdmi_osc",
		.id   = AXP192_ID_LDO4,
		.platform_data = &axp192_ldo4_info,
	}
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
//	.v3_gain     = AXP192_GAIN_R24_3k32, /* 730..1550 mV */
	.init_irq    = axp192_irq_init,
};
#endif

#if defined(CONFIG_I2C_TCC_CORE0)
static struct i2c_board_info __initdata i2c_devices0[] = {
    {
        I2C_BOARD_INFO("akm8975", 0x0F),
        .irq           = COMPASS_IRQ,
        .platform_data = &akm8975_data,
    },
#if defined(CONFIG_REGULATOR_AXP192)
	{
		I2C_BOARD_INFO("axp192", 0x34),
		.irq           = PMIC_IRQ,
		.platform_data = &axp192_info,
	},
#endif
};
#endif

#if 0
static struct tcc_adc_ts_platform_data m803_touchscreen_pdata = {
	.min_x = 1,
	.max_x = 4095,
	.min_y = 1,
	.max_y = 4095,
	.reverse_x_pos = 1,
};
#endif

#if defined(CONFIG_I2C_TCC_CORE1)
static struct i2c_board_info __initdata i2c_devices1[] = {
//	#if defined(CONFIG_TOUCHSCREEN_TCC_UOR6153)
	{
		I2C_BOARD_INFO("tcc-ts-uor6153", 0x48 ),
//		.irq		  = TOUCH_IRQ,
		.platform_data = NULL,
//		.platform_data = &m803_touchscreen_pdata,
	},
//	#endif
};
#endif

#if defined(CONFIG_I2C_TCC_SMU)
static struct i2c_board_info __initdata i2c_devices_smu[] = {
};
#endif

#if defined(CONFIG_I2C_TCC)
static struct tcc_i2c_platform_data m803_core0_platform_data = {
    .core_clk_rate      = 4*1000*1000,    /* core clock rate: 4MHz */
    .core_clk_name      = "i2c0",
    .smu_i2c_flag       = 0,
    .i2c_ch_clk_rate[0] = 400,      /* SCL clock rate : 400kHz */
    .i2c_ch_clk_rate[1] = 400,      /* SCL clock rate : 400kHz */
};

static struct tcc_i2c_platform_data m803_core1_platform_data = {
    .core_clk_rate  = 4*1000*1000,        /* core clock rate: 4MHz */
    .core_clk_name  = "i2c1",
    .smu_i2c_flag   = 0,
    .i2c_ch_clk_rate[0] = 100,      /* SCL clock rate : 100kHz */
    .i2c_ch_clk_rate[1] = 100,      /* SCL clock rate : 100kHz */
};

static struct tcc_i2c_platform_data m803_smu_platform_data = {
    .core_clk_name  = "smu",
    .smu_i2c_flag   = 1,
};
#endif

extern void __init m803_init_gpio(void);
extern void __init m803_init_camera(void);

static void __init tcc8800_init_irq(void)
{
	tcc_init_irq();
}

static void m803_nand_init(void)
{
	unsigned int gpio_wp = GPIO_NAND_WP;
	unsigned int gpio_rdy = GPIO_NAND_RDY;

	tcc_gpio_config(gpio_wp, GPIO_FN(0));
	tcc_gpio_config(gpio_rdy, GPIO_FN(0));

	gpio_request(gpio_wp, "nand_wp");
	gpio_direction_output(gpio_wp, 1);

	gpio_request(gpio_rdy, "nand_rdy");
	gpio_direction_input(gpio_rdy);
}

static int m803_nand_ready(void)
{
	return !gpio_get_value(GPIO_NAND_RDY);
}

static struct tcc_nand_platform_data tcc_nand_platdata = {
	.parts		= NULL,
	.nr_parts	= 0,
	.gpio_wp	= GPIO_NAND_WP,
	.init		= m803_nand_init,
	.ready		= m803_nand_ready,
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


/*----------------------------------------------------------------------
 * Device     : USB Android Gadget
 * Description:
 *----------------------------------------------------------------------*/
static struct usb_mass_storage_platform_data mass_storage_pdata = {
#ifdef CONFIG_SCSI
	.nluns = 4, // for iNand
#else
	.nluns = 3,
#endif
	.vendor = "Telechips, Inc.",
	.product = "M803",
	.release = 0x0100,
};

static struct platform_device usb_mass_storage_device = {
	.name = "usb_mass_storage",
	.id = -1,
	.dev = {
		.platform_data = &mass_storage_pdata,
	},
};
#if CONFIG_TCC_UART2_DMA
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

#if CONFIG_TCC_UART3_DMA
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

#if CONFIG_TCC_BT_DEV
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

#ifdef CONFIG_USB_ANDROID_RNDIS
static struct usb_ether_platform_data rndis_pdata = {
	/* ethaddr is filled by board_serialno_setup */
	.vendorID	= 0x18d1,
	.vendorDescr	= "Telechips, Inc.",
};

static struct platform_device rndis_device = {
	.name	= "rndis",
	.id	= -1,
	.dev	= {
		.platform_data = &rndis_pdata,
	},
};
#endif

static char *usb_functions_ums[] = {
	"usb_mass_storage",
};

static char *usb_functions_ums_adb[] = {
	"usb_mass_storage",
	"adb",
};

static char *usb_functions_rndis[] = {
	"rndis",
};

static char *usb_functions_rndis_adb[] = {
	"rndis",
	"adb",
};

static char *usb_functions_all[] = {
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis",
#endif
	"usb_mass_storage",
	"adb",
#ifdef CONFIG_USB_ANDROID_ACM
	"acm",
#endif
};

static struct android_usb_product usb_products[] = {
	{
		.product_id	= 0xb058, /* Telechips UMS PID */
		.num_functions	= ARRAY_SIZE(usb_functions_ums),
		.functions	= usb_functions_ums,
	},
	{
		.product_id	= 0xdeed,
		.num_functions	= ARRAY_SIZE(usb_functions_ums_adb),
		.functions	= usb_functions_ums_adb,
	},
	{
		.product_id	= 0x0002,
		.num_functions	= ARRAY_SIZE(usb_functions_rndis),
		.functions	= usb_functions_rndis,
	},
	{
		.product_id	= 0x0003,
		.num_functions	= ARRAY_SIZE(usb_functions_rndis_adb),
		.functions	= usb_functions_rndis_adb,
	},
};

static struct android_usb_platform_data android_usb_pdata = {
	.vendor_id	= 0x18D1,
	.product_id	= 0x0001,
	.version	= 0x0100,
	.product_name		= "M803",
	.manufacturer_name	= "Telechips, Inc.",
	.num_products	= ARRAY_SIZE(usb_products),
	.products	= usb_products,
	.num_functions	= ARRAY_SIZE(usb_functions_all),
	.functions	= usb_functions_all,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id		= -1,
	.dev		= {
		.platform_data = &android_usb_pdata,
	},
};

static struct platform_device tcc_earjack_detect_device = {
	.name	= "switch-gpio-earjack-detect",
	.id		= -1,
};

#if defined(CONFIG_TCC_WATCHDOG)
static struct platform_device tccwdt_device = {
	.name	= "tcc-wdt",
	.id		= -1,
};
#endif

static struct platform_device *m803_devices[] __initdata = {
	&tcc8800_uart0_device,
#if defined(CONFIG_TCC_BT_DEV)
	&tcc8800_uart1_device,
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
#if defined(CONFIG_TOUCHSCREEN_TCCTS)
	&m801_touchscreen_device,
#endif
#if 1
#if defined(CONFIG_I2C_TCC_CORE0)
    &tcc8800_i2c_core0_device,
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
    &tcc8800_i2c_core1_device,
#endif
#if defined(CONFIG_TCC_DWC_OTG) || defined(CONFIG_TCC_DWC_OTG_MODULE)	
	&tcc8800_dwc_otg_device,	
#endif
#if defined(CONFIG_USB_EHCI_TCC)  || defined(CONFIG_USB_EHCI_HCD_MODULE)
	&ehci_hs_device,
	&ehci_fs_device,
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
#endif
	&usb_mass_storage_device,
	&android_usb_device,
	&tcc_earjack_detect_device,
#if defined(CONFIG_GPS_JGR_SC3_S)
	&tcc8800_uart3_device,
#endif
#if defined(CONFIG_TCC_WATCHDOG)
	&tccwdt_device,
#endif
};

static int __init board_serialno_setup(char *serialno)
{
	android_usb_pdata.serial_number = serialno;
	return 1;
}
__setup("androidboot.serialno=", board_serialno_setup);

static void __init tcc8800_init_machine(void)
{
	__cpu_early_init();

	m803_init_gpio();
	m803_init_camera();

#if defined(CONFIG_SPI_TCCXXXX_MASTER)
	spi_register_board_info(m803_spi0_board_info, ARRAY_SIZE(m803_spi0_board_info));
	//spi_register_board_info(tcc8800_spi1_board_info, ARRAY_SIZE(tcc8800_spi1_board_info)); //jhlim
#endif

#if defined(CONFIG_I2C_TCC_CORE0)
	i2c_register_board_info(0, i2c_devices0, ARRAY_SIZE(i2c_devices0));
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
	i2c_register_board_info(1, i2c_devices1, ARRAY_SIZE(i2c_devices1));
#endif
#if defined(CONFIG_I2C_TCC_SMU)
	i2c_register_board_info(2, i2c_devices_smu, ARRAY_SIZE(i2c_devices_smu);
#endif

#if defined(CONFIG_SERIAL_TCC_DMA) && defined(CONFIG_TCC_BT_DEV)
	/* BT: use UART1 and TX DMA */
	platform_device_add_data(&tcc8800_uart1_device, &uart1_data_bt, sizeof(struct tcc_uart_platform_data));
#endif



#if CONFIG_TCC_UART2_DMA
	platform_device_add_data(&tcc8800_uart2_device, &uart2_data, sizeof(struct tcc_uart_platform_data));
#endif

#if CONFIG_TCC_UART3_DMA
	platform_device_add_data(&tcc8800_uart3_device, &uart3_data, sizeof(struct tcc_uart_platform_data));
#endif


#if defined(CONFIG_SENSORS_AK8975)
    /* Input mode */
    tcc_gpio_config(TCC_GPF(8), GPIO_FN(0)|GPIO_PULL_DISABLE);  // GPIOF[8]: input mode, disable pull-up/down
    tcc_gpio_direction_input(TCC_GPF(8));
    tcc_gpio_config_ext_intr(INT_EI1, EXTINT_GPIOF_08);
#endif

#if defined(CONFIG_I2C_TCC_CORE0)
    platform_device_add_data(&tcc8800_i2c_core0_device, &m803_core0_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
    platform_device_add_data(&tcc8800_i2c_core1_device, &m803_core1_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
#if defined(CONFIG_I2C_TCC_SMU)
    platform_device_add_data(&tcc8800_i2c_smu_device, &m803_smu_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
    
	tcc_add_nand_device();

	platform_add_devices(m803_devices, ARRAY_SIZE(m803_devices));
}


static void __init tcc8800_map_io(void)
{
	tcc_map_common_io();
}

extern struct sys_timer tcc_timer;

MACHINE_START(M803, "m803")
	/* Maintainer: Telechips Linux BSP Team <linux@telechips.com> */
	.phys_io        = 0xf0000000,
	.io_pg_offst    = ((0xf0000000) >> 18) & 0xfffc,
	.boot_params    = PHYS_OFFSET + 0x00000100,
	.map_io         = tcc8800_map_io,
	.init_irq       = tcc8800_init_irq,
	.init_machine   = tcc8800_init_machine,
	.timer          = &tcc_timer,
MACHINE_END
