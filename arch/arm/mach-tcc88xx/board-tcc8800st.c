/* linux/arch/arm/mach-tcc88xx/board-tcc8800st.c
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
#include <mach/tcc_fb.h>
#include <plat/tcc_ts.h>

#include <plat/nand.h>

#include "devices.h"
#include "board-tcc8800st.h"

#if defined(CONFIG_TCC_OUTPUT_STARTER)
#include <linux/i2c/hdmi_phy.h>
#include <linux/i2c/hdmi_edid.h>
#endif

#if defined(CONFIG_FB_TCC_COMPONENT)
#include <linux/i2c/cs4954.h>
#endif

#if defined(CONFIG_TCC_OUTPUT_STARTER)
#define HDMI_PHY_I2C_SLAVE_ID 		(0x70>>1)
#define HDMI_EDID_I2C_SLAVE_ID 		(0xA0>>1)
#define HDMI_EDID_SEG_I2C_SLAVE_ID	(0x60>>1)
#endif // defined(CONFIG_TCC_OUTPUT_STARTER)

#if defined(CONFIG_FB_TCC_COMPONENT)
#define CS4954_I2C_SLAVE_ID 		(0x08>>1)
#define THS8200_I2C_SLAVE_ID 		(0x40>>1)
#endif // defined(CONFIG_FB_TCC_COMPONENT)

void __cpu_early_init(void);

extern void __init tcc_init_irq(void);
extern void __init tcc_map_common_io(void);

static struct spi_board_info tcc8800st_spi0_board_info[] = {
	{
		.modalias = "spidev",
		.bus_num = 0,					// spi channel 0
		.chip_select = 0,
		/* you can modify the following member variables */
		.max_speed_hz = 2*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
};

static struct spi_board_info tcc8800st_spi1_board_info[] = {
	{
		.modalias = "spidev",
		.bus_num = 1,					// spi channel 1
		.chip_select = 0,
		/* you can modify the following member variables */
		.max_speed_hz = 2*1000*1000,	// default 2Mhz
		.mode = 0						// default 0, you can choose [SPI_CPOL|SPI_CPHA|SPI_CS_HIGH|SPI_LSB_FIRST]
	},
};

#if defined(CONFIG_TCC_OUTPUT_STARTER)
static struct tcc_hdmi_phy_i2c_platform_data  hdmi_phy_i2c_data = {
};
static struct tcc_hdmi_phy_i2c_platform_data  hdmi_edid_i2c_data = {
};
static struct tcc_hdmi_phy_i2c_platform_data  hdmi_edid_seg_i2c_data = {
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
#if defined(CONFIG_TCC_OUTPUT_STARTER)
    {
		I2C_BOARD_INFO("tcc-hdmi-edid", HDMI_EDID_I2C_SLAVE_ID),
		.platform_data = &hdmi_edid_i2c_data,
	},	
    {
		I2C_BOARD_INFO("tcc-hdmi-edid-seg", HDMI_EDID_SEG_I2C_SLAVE_ID),
		.platform_data = &hdmi_edid_seg_i2c_data,
	},	
#endif
};
#endif

#if defined(CONFIG_I2C_TCC_CORE1)
static struct i2c_board_info __initdata i2c_devices1[] = {
	#if defined(CONFIG_FB_TCC_COMPONENT)
	{
		I2C_BOARD_INFO("component-cs4954", CS4954_I2C_SLAVE_ID),
		.platform_data = &cs4954_i2c_data,
	},
	{
		I2C_BOARD_INFO("component-ths8200", THS8200_I2C_SLAVE_ID),
		.platform_data = &ths8200_i2c_data,
	},
	#endif
};
#endif

#if defined(CONFIG_I2C_TCC_SMU)
static struct i2c_board_info __initdata i2c_devices_smu[] = {
#if defined(CONFIG_TCC_OUTPUT_STARTER)
	{
		I2C_BOARD_INFO("tcc-hdmi-phy", HDMI_PHY_I2C_SLAVE_ID),
		.platform_data = &hdmi_phy_i2c_data,
	},
#endif
};
#endif

static struct tcc_i2c_platform_data tcc8800st_core0_platform_data = {
    .core_clk_rate      = 4*1000*1000,    /* core clock rate: 4MHz */
    .core_clk_name      = "i2c0",
    .smu_i2c_flag       = 0,
//20110706 koo : I2C clk change 400KHz => 100KHz for atsc module interface
    .i2c_ch_clk_rate[0] = 100,      /* SCL clock rate : 100kHz */
    .i2c_ch_clk_rate[1] = 100,      /* SCL clock rate : 100kHz */
};

static struct tcc_i2c_platform_data tcc8800st_core1_platform_data = {
    .core_clk_rate      = 4*1000*1000,    /* core clock rate: 4MHz */
    .core_clk_name      = "i2c1",
    .smu_i2c_flag       = 0,
    .i2c_ch_clk_rate[0] = 100,      /* SCL clock rate : 100kHz */
    .i2c_ch_clk_rate[1] = 100,      /* SCL clock rate : 100kHz */
};

static struct tcc_i2c_platform_data tcc8800st_smu_platform_data = {
    .core_clk_name  = "smu",
    .smu_i2c_flag   = 1,
};

extern void __init tcc8800st_init_gpio(void);

static void __init tcc8800_init_irq(void)
{
	tcc_init_irq();
}

static void tcc8800st_nand_init(void)
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

static int tcc8800st_nand_ready(void)
{
	return !gpio_get_value(GPIO_NAND_RDY);
}

static struct tcc_nand_platform_data tcc_nand_platdata = {
	.parts		= NULL,
	.nr_parts	= 0,
	.gpio_wp	= GPIO_NAND_WP,
	.init		= tcc8800st_nand_init,
	.ready		= tcc8800st_nand_ready,
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
	.product = "TCC8800ST",
	.release = 0x0100,
};

static struct platform_device usb_mass_storage_device = {
	.name = "usb_mass_storage",
	.id = -1,
	.dev = {
		.platform_data = &mass_storage_pdata,
	},
};

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
	.product_name		= "TCC8800ST",
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

#if defined(CONFIG_TCC_WATCHDOG)
static struct platform_device tccwdt_device = {
	.name	= "tcc-wdt",
	.id		= -1,
};
#endif

static struct tcc_uart_platform_data uart1_data_bt = {
    .bt_use         = 1,

    .tx_dma_use     = 0,
    .tx_dma_buf_size= SERIAL_TX_DMA_BUF_SIZE,
    .tx_dma_base    = HwGDMA1_BASE,
    .tx_dma_ch      = SERIAL_TX_DMA_CH_NUM,
    .tx_dma_intr    = INT_DMA1_CH0,
    .tx_dma_mode    = SERIAL_TX_DMA_MODE,

    .rx_dma_use     = 1,
    .rx_dma_buf_size= SERIAL_RX_DMA_BUF_SIZE,
    .rx_dma_base    = HwGDMA1_BASE,
    .rx_dma_ch      = SERIAL_RX_DMA_CH_NUM,
    .rx_dma_intr    = 0,
    .rx_dma_mode    = SERIAL_RX_DMA_MODE,
};

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
	.id 	= -1,
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
			tcc_touchscreen_pdata.reverse_y_pos = 1;		
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


static struct platform_device *tcc8800st_devices[] __initdata = {
	&tcc8800_uart0_device,
	&tcc8800_uart1_device,
#if defined(CONFIG_GPS_JGR_SC3_S)
	&tcc8800_uart5_device,
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

	&usb_mass_storage_device,
	&android_usb_device,
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

	tcc8800st_init_gpio();

#if defined(CONFIG_SPI_TCCXXXX_MASTER)
	spi_register_board_info(tcc8800st_spi0_board_info, ARRAY_SIZE(tcc8800st_spi0_board_info));
	//spi_register_board_info(tcc8800_spi1_board_info, ARRAY_SIZE(tcc8800_spi1_board_info)); //jhlim
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

#if defined(CONFIG_SERIAL_TCC_DMA) || defined(CONFIG_BT)
	/* BT: use UART1 and TX DMA */
	platform_device_add_data(&tcc8800_uart1_device, &uart1_data_bt, sizeof(struct tcc_uart_platform_data));
#endif

#if defined(CONFIG_TOUCHSCREEN_TCCTS) || defined(CONFIG_TOUCHSCREEN_TCCTS_MODULE)
	tcc_add_touchscreen_device();
#endif

#if defined(CONFIG_I2C_TCC_CORE0)
    platform_device_add_data(&tcc8800_i2c_core0_device, &tcc8800st_core0_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
    platform_device_add_data(&tcc8800_i2c_core1_device, &tcc8800st_core1_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif
#if defined(CONFIG_I2C_TCC_SMU)
    platform_device_add_data(&tcc8800_i2c_smu_device, &tcc8800st_smu_platform_data, sizeof(struct tcc_i2c_platform_data));
#endif

	tcc_add_nand_device();

	platform_add_devices(tcc8800st_devices, ARRAY_SIZE(tcc8800st_devices));
}


static void __init tcc8800_map_io(void)
{
	tcc_map_common_io();
}

extern struct sys_timer tcc_timer;

MACHINE_START(TCC8800ST, "tcc8800st")
	/* Maintainer: Telechips Linux BSP Team <linux@telechips.com> */
	.phys_io        = 0xf0000000,
	.io_pg_offst    = ((0xf0000000) >> 18) & 0xfffc,
	.boot_params    = PHYS_OFFSET + 0x00000100,
	.map_io         = tcc8800_map_io,
	.init_irq       = tcc8800_init_irq,
	.init_machine   = tcc8800_init_machine,
	.timer          = &tcc_timer,
MACHINE_END
