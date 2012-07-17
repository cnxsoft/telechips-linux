#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/mmc/host.h>
#include <linux/gpio.h>

#include <mach/mmc.h>

#include <mach/gpio.h>
#include <mach/irqs.h>
//#include <mach/bsp.h>
#include <mach/bsp.h>
#include <asm/mach-types.h>

#include "devices.h"
#include "board-tcc8920.h"

#if defined(CONFIG_MMC_TCC_SDHC)
extern void tcc_init_sdhc_devices(void);

struct tcc_mmc_platform_data tcc8920_mmc_platform_data[];

typedef enum {
	TCC_MMC_BUS_WIDTH_4 = 4,
	TCC_MMC_BUS_WIDTH_8 = 8
} tcc_mmc_bus_width_type;

#define TCC_MMC_PORT_NULL	0x0FFFFFFF

// PIC 0
#define HwINT0_EI4					Hw7						// R/W, External Interrupt 4 enable
#define HwINT0_EI5					Hw8						// R/W, External Interrupt 5 enable

// PIC 1
#define HwINT1_SD0					Hw12					// R/W, SD/MMC 0 interrupt enable
#define HwINT1_SD1					Hw13					// R/W, SD/MMC 1 interrupt enable
#define HwINT1_SD2	 				Hw1 					// R/W, SD/MMC 2 Interrupt enable
#define HwINT1_SD3		 			Hw0 					// R/W, SD/MMC 3 Interrupt enable


#if defined(CONFIG_MMC_TCC_SD30_TEST)	// [0] : SD3.0,   [1] : WiFi
#define TCC_MMC_SDIO_WIFI_USED

typedef enum {
	TCC_MMC_TYPE_SD3_0,
	TCC_MMC_TYPE_WIFI,
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

static int TCC_MMC_SD30_ON = TCC_GPC(29);

static struct mmc_port_config mmc_ports[] = {
	[TCC_MMC_TYPE_SD3_0] = {
		.data0	= TCC_GPC(2),
		.data1	= TCC_GPC(3),
		.data2	= TCC_GPC(4),
		.data3	= TCC_GPC(5),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPC(1),
		.clk	= TCC_GPC(0),
		.func	= GPIO_FN(3),
		.width	= TCC_MMC_BUS_WIDTH_4,

		.cd	= TCC_GPC(27),
		.pwr	= TCC_GPC(10),	//High:3.3V, Low:1.8V
	},
	[TCC_MMC_TYPE_WIFI] = {
		.data0	= TCC_GPB(20),
		.data1	= TCC_GPB(21),
		.data2	= TCC_GPB(22),
		.data3	= TCC_GPB(23),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPB(19),
		.clk	= TCC_GPB(28),
		.func	= GPIO_FN(3),
		.width	= TCC_MMC_BUS_WIDTH_4,

		.cd	= TCC_GPB(13),
		.pwr	= GPIO_SD1_ON,
	},
};

#else	//#if defined(CONFIG_MMC_TCC_SD30_TEST)

#define TCC_MMC_SDIO_WIFI_USED

typedef enum {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
	TCC_MMC_TYPE_EMMC,
	#endif
	TCC_MMC_TYPE_SD,
	TCC_MMC_TYPE_WIFI,
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

static struct mmc_port_config mmc_ports[] = {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
	[TCC_MMC_TYPE_EMMC] = {
		.data0	= TCC_GPD(18),
		.data1	= TCC_GPD(17),
		.data2	= TCC_GPD(16),
		.data3	= TCC_GPD(15),
		.data4	= TCC_GPD(14),
		.data5	= TCC_GPD(13),
		.data6	= TCC_GPD(12),
		.data7	= TCC_GPD(11),
		.cmd	= TCC_GPD(19),
		.clk	= TCC_GPD(20),
		.func	= GPIO_FN(2),
		.width	= TCC_MMC_BUS_WIDTH_8,
		.strength = GPIO_CD(1),

		.cd	= TCC_MMC_PORT_NULL,	//TCC_GPB(14),
		.pwr	= TCC_MMC_PORT_NULL,	// do not used!
	},
	#endif
	[TCC_MMC_TYPE_SD] = {
		.data0	= TCC_GPF(19),
		.data1	= TCC_GPF(20),
		.data2	= TCC_GPF(21),
		.data3	= TCC_GPF(22),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPF(18),
		.clk	= TCC_GPF(17),
		.func	= GPIO_FN(2),
		.width	= TCC_MMC_BUS_WIDTH_4,
		.strength = GPIO_CD(1),

		.cd	= TCC_GPB(12),
		.pwr	= TCC_MMC_PORT_NULL,	// do not used!
	},
	[TCC_MMC_TYPE_WIFI] = {
		.data0	= TCC_GPB(20),
		.data1	= TCC_GPB(21),
		.data2	= TCC_GPB(22),
		.data3	= TCC_GPB(23),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPB(19),
		.clk	= TCC_GPB(28),
		.func	= GPIO_FN(3),
		.width	= TCC_MMC_BUS_WIDTH_4,
		.strength = GPIO_CD(1),

		.cd	= TCC_GPB(13),
		.pwr	= GPIO_SD1_ON,	// for Power Control
	},
};
#endif	//#if defined(CONFIG_MMC_TCC_SD30_TEST)

static int tccUsedSDportNum = TCC_MMC_TYPE_MAX;

int tcc8920_mmc_init(struct device *dev, int id)
{
	struct mmc_port_config *mmc_port;
	int mmc_gpio_config;

	BUG_ON(id >= tccUsedSDportNum);

	mmc_port = &mmc_ports[id];

	// 1. Power Up
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
	if(id == TCC_MMC_TYPE_EMMC)
	{
		gpio_request(GPIO_SD0_ON, "eMMC_Power");
		gpio_direction_output(GPIO_SD0_ON, 1);
	}
	#endif

	if(mmc_port->pwr != TCC_MMC_PORT_NULL)
	{
		gpio_request(mmc_port->pwr, "WiFi_Power");
		gpio_direction_output(mmc_port->pwr, 0);
		msleep(100);
		gpio_direction_output(mmc_port->pwr, 1);
	}

	#if defined(CONFIG_BCM4330_SUB_BOARD)
	if(id == TCC_MMC_TYPE_WIFI) {
	    gpio_request(GPIO_SDWF_RST, "bcm4330_on");
	    gpio_direction_output(GPIO_SDWF_RST, 0);
	    msleep(300); // FIXME:
	    gpio_direction_output(GPIO_SDWF_RST, 1);
	}
	#endif

	// 2. GPIO Function
	mmc_gpio_config = mmc_port->func | mmc_port->strength;
	tcc_gpio_config(mmc_port->data0, mmc_gpio_config);
	tcc_gpio_config(mmc_port->data1, mmc_gpio_config);
	tcc_gpio_config(mmc_port->data2, mmc_gpio_config);
	tcc_gpio_config(mmc_port->data3, mmc_gpio_config);

	if(mmc_port->width == TCC_MMC_BUS_WIDTH_8)
	{
		tcc_gpio_config(mmc_port->data4, mmc_gpio_config);
		tcc_gpio_config(mmc_port->data5, mmc_gpio_config);
		tcc_gpio_config(mmc_port->data6, mmc_gpio_config);
		tcc_gpio_config(mmc_port->data7, mmc_gpio_config);
	}

	tcc_gpio_config(mmc_port->cmd, mmc_gpio_config);
	tcc_gpio_config(mmc_port->clk, mmc_port->func | GPIO_CD(3));

	// 3. Card Detect
	if(mmc_port->cd != TCC_MMC_PORT_NULL)
	{
		tcc_gpio_config(mmc_port->cd, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(mmc_port->cd, "sd_cd");

		gpio_direction_input(mmc_port->cd);
	}

	return 0;
}

int tcc8920_mmc_card_detect(struct device *dev, int id)
{
	BUG_ON(id >= tccUsedSDportNum);

#if defined(CONFIG_BCM4330_SUB_BOARD)
	if(id == TCC_MMC_TYPE_WIFI)
		return 1;
#endif

	if(mmc_ports[id].cd == TCC_MMC_PORT_NULL)
		return 1;

	return gpio_get_value(mmc_ports[id].cd) ? 0 : 1;	
}

int tcc8920_mmc_suspend(struct device *dev, int id)
{
	if(mmc_ports[id].pwr != TCC_MMC_PORT_NULL)
		gpio_direction_output(mmc_ports[id].pwr, 0);

	return 0;
}

int tcc8920_mmc_resume(struct device *dev, int id)
{
	if(mmc_ports[id].pwr != TCC_MMC_PORT_NULL)
		gpio_direction_output(mmc_ports[id].pwr, 1);

 	return 0;
}

int tcc8920_mmc_set_power(struct device *dev, int id, int on)
{
	if (on) {
		/* power */
		if(mmc_ports[id].pwr != TCC_MMC_PORT_NULL)
		{
			gpio_direction_output(mmc_ports[id].pwr, 1);

			mdelay(1);
		}
	} else {

		//mdelay(10);
	}

	return 0;
}

int tcc8920_mmc_set_bus_width(struct device *dev, int id, int width)
{
	return 0;
}

int tcc8920_mmc_cd_int_config(struct device *dev, int id, unsigned int cd_irq)
{
	if(tcc8920_mmc_platform_data[id].cd_int_num > 0)
	{
		tcc_gpio_config_ext_intr(tcc8920_mmc_platform_data[id].cd_irq_num, tcc8920_mmc_platform_data[id].cd_ext_irq);
	}
	else
	{
		return -1;
	}	

	return 0;
}

//Start : Wakeup for SD Insert->Remove in suspend. - 120109, hjbae
int tcc892x_sd_card_detect(void)
{
	return gpio_get_value(mmc_ports[TCC_MMC_TYPE_SD].cd) ? 0 : 1;
}
//End

#if defined(CONFIG_MMC_TCC_SD30_TEST)
struct tcc_mmc_platform_data tcc8920_mmc_platform_data[] = {		// [0]:SD3.0,   [1]:WiFi
	[TCC_MMC_TYPE_SD3_0] = {
		.slot	= 0,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/* MMC_CAP_8_BIT_DATA */
			| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,		// used on CPU Board
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8920_mmc_init,
		.card_detect = tcc8920_mmc_card_detect,
		.cd_int_config = tcc8920_mmc_cd_int_config,
		.suspend = tcc8920_mmc_suspend,
		.resume	= tcc8920_mmc_resume,
		.set_power = tcc8920_mmc_set_power,
		.set_bus_width = tcc8920_mmc_set_bus_width,

		.cd_int_num = HwINT0_EI4,
		.cd_irq_num = INT_EI4,
		.cd_ext_irq = EXTINT_GPIOC_27,
		.peri_name = PERI_SDMMC0,
		.io_name = RB_SDMMC0CONTROLLER,
		.pic = HwINT1_SD0,
	},
	[TCC_MMC_TYPE_WIFI] = {
		.slot	= 3,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD1 Slot
		.f_min	= 100000,
//		.f_max	= 48000000,	/* support highspeed mode */
		.f_max	= 24000000,	// Only Atheros WiFi(AR6102)
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8920_mmc_init,
		.card_detect = tcc8920_mmc_card_detect,
		.cd_int_config = tcc8920_mmc_cd_int_config,
		.suspend = tcc8920_mmc_suspend,
		.resume	= tcc8920_mmc_resume,
		.set_power = tcc8920_mmc_set_power,
		.set_bus_width = tcc8920_mmc_set_bus_width,

		.cd_int_num = HwINT0_EI5,
		.cd_irq_num = INT_EI5,
		.cd_ext_irq = EXTINT_GPIOD_14,
		.peri_name = PERI_SDMMC3,
		.io_name = RB_SDMMC3CONTROLLER,
		.pic = HwINT1_SD3,
	},
};

#else	//#if defined(CONFIG_MMC_TCC_SD30_TEST)

struct tcc_mmc_platform_data tcc8920_mmc_platform_data[] = {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)		// [0]:eMMC,   [1]:SD,   [2]:WiFi
	[TCC_MMC_TYPE_EMMC] = {
		.slot	= 4,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			| MMC_CAP_8_BIT_DATA
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD0 Slot
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8920_mmc_init,
		.card_detect = tcc8920_mmc_card_detect,
		.cd_int_config = tcc8920_mmc_cd_int_config,
		.suspend = tcc8920_mmc_suspend,
		.resume	= tcc8920_mmc_resume,
		.set_power = tcc8920_mmc_set_power,
		.set_bus_width = tcc8920_mmc_set_bus_width,

		.cd_int_num = -1,
		//.cd_irq_num = INT_EI5,
		.cd_ext_irq = -1,
		.peri_name = PERI_SDMMC0,
		.io_name = RB_SDMMC0CONTROLLER,
		.pic = HwINT1_SD0,
	},
	#endif
	[TCC_MMC_TYPE_SD] = {
		.slot	= 5,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/* MMC_CAP_8_BIT_DATA */
			| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8920_mmc_init,
		.card_detect = tcc8920_mmc_card_detect,
		.cd_int_config = tcc8920_mmc_cd_int_config,
		.suspend = tcc8920_mmc_suspend,
		.resume	= tcc8920_mmc_resume,
		.set_power = tcc8920_mmc_set_power,
		.set_bus_width = tcc8920_mmc_set_bus_width,

		.cd_int_num = HwINT0_EI4,
		.cd_irq_num = INT_EI4,
		.cd_ext_irq = EXTINT_GPIOB_12,
		.peri_name = PERI_SDMMC1,
		.io_name = RB_SDMMC1CONTROLLER,
		.pic = HwINT1_SD1,
	},
	[TCC_MMC_TYPE_WIFI] = {
		.slot	= 3,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD1 Slot
		.f_min	= 100000,
//		.f_max	= 48000000,	/* support highspeed mode */
		.f_max	= 24000000,	// Only Atheros WiFi(AR6102)
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8920_mmc_init,
		.card_detect = tcc8920_mmc_card_detect,
		.cd_int_config = tcc8920_mmc_cd_int_config,
		.suspend = tcc8920_mmc_suspend,
		.resume	= tcc8920_mmc_resume,
		.set_power = tcc8920_mmc_set_power,
		.set_bus_width = tcc8920_mmc_set_bus_width,

		.cd_int_num = HwINT0_EI5,
		.cd_irq_num = INT_EI5,
		.cd_ext_irq = EXTINT_GPIOB_13,
		.peri_name = PERI_SDMMC3,
		.io_name = RB_SDMMC3CONTROLLER,
		.pic = HwINT1_SD3,
	},

	#if 0	//for Example
	[x] = {
		.slot	= 2,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_8_BIT_DATA */
			| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8920_mmc_init,
		.card_detect = tcc8920_mmc_card_detect,
		.suspend = tcc8920_mmc_suspend,
		.resume	= tcc8920_mmc_resume,
		.set_power = tcc8920_mmc_set_power,
		.set_bus_width = tcc8920_mmc_set_bus_width,

		.cd_int_num = -1,
		.peri_name = PERI_SDMMC2,
		.io_name = RB_SDMMC2CONTROLLER,
		.pic = HwINT1_SD2,
	},
	#endif
};
#endif	//#if defined(CONFIG_MMC_TCC_SD30_TEST)		// [0]:SD3.0,   [1]:WiFi

static void tcc8920_mmc_port_setup(void)
{
	// 0x1000 : TCC8920_D2_08X4_SV01 - DDR2 512MB(32Bit)
	// 0x1001 : TCC8920_D2_08X4_SV01 - DDR2 1024MB(32Bit)
	// 0x1002 : TCC8920_D3_08X4_SV01 - DDR3 512MB(32Bit)
	// 0x1003 : TCC8920_D3_08X4_SV01 - DDR3 1024MB(32Bit)
	// 0x1004 : TCC8920_D3_08X4_SV01 - DDR3 512MB(16Bit)
	// 0x1005 : TCC8920_D3_16X4_2CS_SV01 - DDR3 1024MB (32Bit)
	// 0x1006 : TCC8925_D3_08X4_2CS_SV01 - DDR3 1024MB (16Bit)
	// 0x1007 : TCC8920_D3_08X4_SV60 - DDR3 1024MB(32Bit)
	// 0x1008 : TCC8923_D3_08X4_SV01 - DDR3 1024MB(32Bit)
	if(system_rev < 0x1005)		// for TCC8920 EVM Board
	{
		// for SD3.0 Test
		#if defined(CONFIG_MMC_TCC_SD30_TEST)
		mmc_ports[TCC_MMC_TYPE_SD3_0].data0 = TCC_GPD(18);
		mmc_ports[TCC_MMC_TYPE_SD3_0].data1 = TCC_GPD(17);
		mmc_ports[TCC_MMC_TYPE_SD3_0].data2 = TCC_GPD(16);
		mmc_ports[TCC_MMC_TYPE_SD3_0].data3 = TCC_GPD(15);
		mmc_ports[TCC_MMC_TYPE_SD3_0].cmd = TCC_GPD(19);
		mmc_ports[TCC_MMC_TYPE_SD3_0].clk = TCC_GPD(20);
		mmc_ports[TCC_MMC_TYPE_SD3_0].func = GPIO_FN(2);
		mmc_ports[TCC_MMC_TYPE_SD3_0].cd = TCC_GPF(9);

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].slot	= 4;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].caps	|= (MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].cd_ext_irq = EXTINT_GPIOF_09;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].peri_name = PERI_SDMMC0;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].io_name = RB_SDMMC0CONTROLLER;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].pic = HwINT1_SD0;

		#else

		// for eMMC
		#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
		mmc_ports[TCC_MMC_TYPE_EMMC].data0 = TCC_GPF(19);
		mmc_ports[TCC_MMC_TYPE_EMMC].data1 = TCC_GPF(20);
		mmc_ports[TCC_MMC_TYPE_EMMC].data2 = TCC_GPF(21);
		mmc_ports[TCC_MMC_TYPE_EMMC].data3 = TCC_GPF(22);
		mmc_ports[TCC_MMC_TYPE_EMMC].data4 = TCC_GPF(23);
		mmc_ports[TCC_MMC_TYPE_EMMC].data5 = TCC_GPF(24);
		mmc_ports[TCC_MMC_TYPE_EMMC].data6 = TCC_GPF(25);
		mmc_ports[TCC_MMC_TYPE_EMMC].data7 = TCC_GPF(26);
		mmc_ports[TCC_MMC_TYPE_EMMC].cmd = TCC_GPF(18);
		mmc_ports[TCC_MMC_TYPE_EMMC].clk = TCC_GPF(17);
		//mmc_ports[TCC_MMC_TYPE_EMMC].cd = TCC_MMC_PORT_NULL,	//TCC_GPD(12);
		//mmc_ports[TCC_MMC_TYPE_EMMC].func = GPIO_FN(2);

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_EMMC].slot = 5;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_EMMC].caps |= (MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_EMMC].peri_name = PERI_SDMMC1;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_EMMC].io_name = RB_SDMMC1CONTROLLER;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_EMMC].pic = HwINT1_SD1;
		#endif

		// for SDHC
		mmc_ports[TCC_MMC_TYPE_SD].data0 = TCC_GPB(2);
		mmc_ports[TCC_MMC_TYPE_SD].data1 = TCC_GPB(3);
		mmc_ports[TCC_MMC_TYPE_SD].data2 = TCC_GPB(4);
		mmc_ports[TCC_MMC_TYPE_SD].data3 = TCC_GPB(5);
		mmc_ports[TCC_MMC_TYPE_SD].cmd = TCC_GPB(1);
		mmc_ports[TCC_MMC_TYPE_SD].clk = TCC_GPB(0);
		mmc_ports[TCC_MMC_TYPE_SD].cd = TCC_GPD(13);
		mmc_ports[TCC_MMC_TYPE_SD].func = GPIO_FN(3);

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].slot = 2;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].caps &= ~(MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].cd_ext_irq = EXTINT_GPIOD_13;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].peri_name = PERI_SDMMC2;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].io_name = RB_SDMMC2CONTROLLER;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].pic = HwINT1_SD2;
		#endif

		// for WiFi
		#if defined(TCC_MMC_SDIO_WIFI_USED)
		mmc_ports[TCC_MMC_TYPE_WIFI].cd = TCC_GPD(14);

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_WIFI].cd_ext_irq = EXTINT_GPIOD_14;
		#endif
	}
	else if(system_rev == 0x1006)
	{
		#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
		tccUsedSDportNum = 2;
		#else
		tccUsedSDportNum = 1;
		#endif

		// for eMMC
		#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
		//mmc_ports[TCC_MMC_TYPE_EMMC].cd = TCC_MMC_PORT_NULL,	//TCC_GPE(26),
		#endif

		// for SDHC
		mmc_ports[TCC_MMC_TYPE_SD].data0 = TCC_GPF(26);
		mmc_ports[TCC_MMC_TYPE_SD].data1 = TCC_GPF(25);
		mmc_ports[TCC_MMC_TYPE_SD].data2 = TCC_GPF(24);
		mmc_ports[TCC_MMC_TYPE_SD].data3 = TCC_GPF(23);
		mmc_ports[TCC_MMC_TYPE_SD].cmd = TCC_GPF(27);
		mmc_ports[TCC_MMC_TYPE_SD].clk = TCC_GPF(28);
		mmc_ports[TCC_MMC_TYPE_SD].cd = TCC_GPE(28);
		mmc_ports[TCC_MMC_TYPE_SD].func = GPIO_FN(3);

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].slot = 6;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].caps &= ~(MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].cd_ext_irq = EXTINT_GPIOE_28;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].peri_name = PERI_SDMMC2;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].io_name = RB_SDMMC2CONTROLLER;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].pic = HwINT1_SD2;
	}
	else if(system_rev == 0x1008)
	{
		#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)	// eMMC(SD0) + SDHC(SD2)
		tccUsedSDportNum = 2;

		// for eMMC
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_EMMC].caps |= (MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);

		mmc_ports[TCC_MMC_TYPE_EMMC].strength = GPIO_CD(0);

		// for SDHC - WiFi
		mmc_ports[TCC_MMC_TYPE_SD].cd = TCC_GPE(0);
		mmc_ports[TCC_MMC_TYPE_SD].pwr = GPIO_SD1_ON;

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].f_max	= 24000000,	/* support highspeed mode */
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].caps &= ~(MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].cd_ext_irq = EXTINT_GPIOE_00;

		#else

		#if 1	// SDHC(SD0) + WiFi(SD2)
		// for SDHC
		mmc_ports[TCC_MMC_TYPE_SD].data0 = TCC_GPD(18);
		mmc_ports[TCC_MMC_TYPE_SD].data1 = TCC_GPD(17);
		mmc_ports[TCC_MMC_TYPE_SD].data2 = TCC_GPD(16);
		mmc_ports[TCC_MMC_TYPE_SD].data3 = TCC_GPD(15);
		mmc_ports[TCC_MMC_TYPE_SD].cmd = TCC_GPD(19);
		mmc_ports[TCC_MMC_TYPE_SD].clk = TCC_GPD(20);
		mmc_ports[TCC_MMC_TYPE_SD].func = GPIO_FN(2);
		mmc_ports[TCC_MMC_TYPE_SD].cd = TCC_GPE(1);
		mmc_ports[TCC_MMC_TYPE_SD].pwr	= TCC_GPC(20);

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].slot	= 4;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].caps	|= (MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].cd_ext_irq = EXTINT_GPIOE_01;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].peri_name = PERI_SDMMC0;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].io_name = RB_SDMMC0CONTROLLER;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_SD].pic = HwINT1_SD0;

		// for WiFi
		mmc_ports[TCC_MMC_TYPE_WIFI].data0 = TCC_GPF(19);
		mmc_ports[TCC_MMC_TYPE_WIFI].data1 = TCC_GPF(20);
		mmc_ports[TCC_MMC_TYPE_WIFI].data2 = TCC_GPF(21);
		mmc_ports[TCC_MMC_TYPE_WIFI].data3 = TCC_GPF(22);
		mmc_ports[TCC_MMC_TYPE_WIFI].cmd = TCC_GPF(18);
		mmc_ports[TCC_MMC_TYPE_WIFI].clk = TCC_GPF(17);
		mmc_ports[TCC_MMC_TYPE_WIFI].cd = TCC_GPE(0);
		mmc_ports[TCC_MMC_TYPE_WIFI].func = GPIO_FN(2);

		tcc8920_mmc_platform_data[TCC_MMC_TYPE_WIFI].slot = 5;
		//tcc8920_mmc_platform_data[TCC_MMC_TYPE_WIFI].caps |= (MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED);
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_WIFI].cd_ext_irq = EXTINT_GPIOE_00;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_WIFI].peri_name = PERI_SDMMC1;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_WIFI].io_name = RB_SDMMC1CONTROLLER;
		tcc8920_mmc_platform_data[TCC_MMC_TYPE_WIFI].pic = HwINT1_SD1;

		#else	// Only SDHC(SD2) (SD&WiFi)

		tccUsedSDportNum = 1;
		#endif
		#endif
	}
}

static int __init tcc8920_init_mmc(void)
{
	if (!machine_is_tcc8920())
		return 0;

	tccUsedSDportNum = TCC_MMC_TYPE_MAX;

	tcc8920_mmc_port_setup();
	tcc_init_sdhc_devices();

	printk("%s(%d)\n",__func__, tccUsedSDportNum);

#if defined(CONFIG_MMC_TCC_SDHC0)
	if (tccUsedSDportNum > 0)
	{
		tcc_sdhc0_device.dev.platform_data = &tcc8920_mmc_platform_data[0];
		platform_device_register(&tcc_sdhc0_device);
	}
#endif
#if defined(CONFIG_MMC_TCC_SDHC1)
	if (tccUsedSDportNum > 1)
	{
		tcc_sdhc1_device.dev.platform_data = &tcc8920_mmc_platform_data[1];
		platform_device_register(&tcc_sdhc1_device);
	}
#endif
#if defined(CONFIG_MMC_TCC_SDHC2)
	if (tccUsedSDportNum > 2)
	{
		tcc_sdhc2_device.dev.platform_data = &tcc8920_mmc_platform_data[2];
		platform_device_register(&tcc_sdhc2_device);
	}
#endif
#if defined(CONFIG_MMC_TCC_SDHC3)
	if (tccUsedSDportNum > 3)
	{
		tcc_sdhc3_device.dev.platform_data = &tcc8920_mmc_platform_data[3];
		platform_device_register(&tcc_sdhc3_device);
	}
#endif

	return 0;
}
device_initcall(tcc8920_init_mmc);
#endif

