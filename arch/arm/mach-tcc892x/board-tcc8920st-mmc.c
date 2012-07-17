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
#include "board-tcc8920st.h"

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

#if defined(CONFIG_STB_BOARD_DONGLE)
#define TCC_MMC_SDIO_WIFI_USED

#if defined(CONFIG_HDMI_DONGLE_WIFI_REALTEK) || defined(CONFIG_HDMI_DONGLE_WIFI_BROADCOM)
#define WIFI_SDMMC_PORT		3
#else
#define WIFI_SDMMC_PORT		4
#endif
#define WIFI_PERI_SDMMC		PERI_SDMMC0
#define WIFI_RB_SDMMC		RB_SDMMC0CONTROLLER
#define WIFI_HwINT1_SD		HwINT1_SD0

#define EMMC_SDMMC_PORT		4
#define EMMC_PERI_SDMMC		PERI_SDMMC0
#define EMMC_RB_SDMMC		RB_SDMMC0CONTROLLER
#define EMMC_HwINT1_SD		HwINT1_SD0

typedef enum {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
	TCC_MMC_TYPE_EMMC,
	#endif
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
			.data4	= TCC_MMC_PORT_NULL,
			.data5	= TCC_MMC_PORT_NULL,
			.data6	= TCC_MMC_PORT_NULL,
			.data7	= TCC_MMC_PORT_NULL,
			.cmd	= TCC_GPD(19),
			.clk	= TCC_GPD(20),
			.func	= GPIO_FN(2),
			.width	= TCC_MMC_BUS_WIDTH_4,

			.cd		= TCC_MMC_PORT_NULL,
			.pwr	= TCC_MMC_PORT_NULL,
		},
	#endif
	
	#if defined(CONFIG_HDMI_DONGLE_WIFI_REALTEK) || defined(CONFIG_HDMI_DONGLE_WIFI_BROADCOM)
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

			.cd	= TCC_MMC_PORT_NULL,
			.pwr	= TCC_GPD(21),
		},
	#else
		[TCC_MMC_TYPE_WIFI] = {
			.data0	= TCC_GPD(18),
			.data1	= TCC_GPD(17),
			.data2	= TCC_GPD(16),
			.data3	= TCC_GPD(15),
			.data4	= TCC_MMC_PORT_NULL,
			.data5	= TCC_MMC_PORT_NULL,
			.data6	= TCC_MMC_PORT_NULL,
			.data7	= TCC_MMC_PORT_NULL,
			.cmd	= TCC_GPD(19),
			.clk	= TCC_GPD(20),
			.func	= GPIO_FN(2),
			.width	= TCC_MMC_BUS_WIDTH_4,

			.cd	= TCC_MMC_PORT_NULL,
			.pwr	= TCC_GPD(21),
		},
	#endif
};

#elif defined(CONFIG_STB_BOARD_ISDBT_MODULE)
#define TCC_MMC_SD_CARD_USED

typedef enum {
	TCC_MMC_TYPE_SD,
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

#define TFCD_EXT_INT		EXTINT_GPIOD_14
#define TFCD_GPIO_PORT		TCC_GPD(14)

#define TFCD_SDMMC_PORT		4
#define TFCD_PERI_SDMMC		PERI_SDMMC0
#define TFCD_RB_SDMMC		RB_SDMMC0CONTROLLER
#define TFCD_HwINT1_SD		HwINT1_SD0

static struct mmc_port_config mmc_ports[] = {
	[TCC_MMC_TYPE_SD] = {
		.data0	= TCC_GPD(18),
		.data1	= TCC_GPD(17),
		.data2	= TCC_GPD(16),
		.data3	= TCC_GPD(15),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPD(19),
		.clk	= TCC_GPD(20),
		.func	= GPIO_FN(2),
		.width	= TCC_MMC_BUS_WIDTH_4,

		.cd	= TFCD_GPIO_PORT,
		.pwr	= TCC_MMC_PORT_NULL,
	},
};


#elif defined(CONFIG_STB_BOARD_HDB892S) || defined(CONFIG_STB_BOARD_HDB892F)
#define TCC_MMC_SD_CARD_USED

typedef enum {
	TCC_MMC_TYPE_SD,
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

#if defined(CONFIG_STB_BOARD_HDB892S)
#define TFCD_EXT_INT		EXTINT_GPIOD_12
#define TFCD_GPIO_PORT		TCC_GPD(12)

#define TFCD_SDMMC_PORT		5
#define TFCD_PERI_SDMMC		PERI_SDMMC1
#define TFCD_RB_SDMMC		RB_SDMMC1CONTROLLER
#define TFCD_HwINT1_SD		HwINT1_SD1

static struct mmc_port_config mmc_ports[] = {
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

		.cd	= TFCD_GPIO_PORT,
		.pwr	= TCC_MMC_PORT_NULL,
	},
};

#else	//if defined(CONFIG_STB_BOARD_HDB892F)
#define TFCD_EXT_INT		EXTINT_GPIOF_02
#define TFCD_GPIO_PORT		TCC_GPF(2)

#define TFCD_SDMMC_PORT		4
#define TFCD_PERI_SDMMC		PERI_SDMMC0
#define TFCD_RB_SDMMC		RB_SDMMC0CONTROLLER
#define TFCD_HwINT1_SD		HwINT1_SD0

static struct mmc_port_config mmc_ports[] = {
	[TCC_MMC_TYPE_SD] = {
		.data0	= TCC_GPD(18),
		.data1	= TCC_GPD(17),
		.data2	= TCC_GPD(16),
		.data3	= TCC_GPD(15),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPD(19),
		.clk	= TCC_GPD(20),
		.func	= GPIO_FN(2),
		.width	= TCC_MMC_BUS_WIDTH_4,

		.cd	= TFCD_GPIO_PORT,
		.pwr	= TCC_MMC_PORT_NULL,
	},
};
#endif

#else	// TCC892x STB
#define TCC_MMC_SD_CARD_USED
//#define TCC_MMC_SDIO_WIFI_USED

typedef enum {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
	TCC_MMC_TYPE_EMMC,
	#endif
	#if defined(TCC_MMC_SD_CARD_USED)
	TCC_MMC_TYPE_SD,
	#endif
	#if defined(TCC_MMC_SDIO_WIFI_USED)
	TCC_MMC_TYPE_WIFI,
	#endif
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

#define TFCD_EXT_INT		EXTINT_GPIOD_12
#define TFCD_GPIO_PORT		TCC_GPD(12)

#define TFCD_SDMMC_PORT		5
#define TFCD_PERI_SDMMC		PERI_SDMMC1
#define TFCD_RB_SDMMC		RB_SDMMC1CONTROLLER
#define TFCD_HwINT1_SD		HwINT1_SD1

#define WIFI_SDMMC_PORT		6
#define WIFI_PERI_SDMMC		PERI_SDMMC2
#define WIFI_RB_SDMMC		RB_SDMMC2CONTROLLER
#define WIFI_HwINT1_SD		HwINT1_SD2

#define EMMC_SDMMC_PORT		5
#define EMMC_PERI_SDMMC		PERI_SDMMC1
#define EMMC_RB_SDMMC		RB_SDMMC1CONTROLLER
#define EMMC_HwINT1_SD		HwINT1_SD1

static struct mmc_port_config mmc_ports[] = {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
		[TCC_MMC_TYPE_EMMC] = {
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

		.cd		= TCC_MMC_PORT_NULL,
		.pwr	= TCC_MMC_PORT_NULL,
		},
	#endif

	#if defined(TCC_MMC_SD_CARD_USED)
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

		.cd	= TFCD_GPIO_PORT,
		.pwr	= TCC_MMC_PORT_NULL,
	},
	#endif
	#if defined(TCC_MMC_SDIO_WIFI_USED)
	[TCC_MMC_TYPE_WIFI] = {
		.data0	= TCC_GPB(2),
		.data1	= TCC_GPB(3),
		.data2	= TCC_GPB(4),
		.data3	= TCC_GPB(5),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPB(1),
		.clk	= TCC_GPB(0),
		.func	= GPIO_FN(3),
		.width	= TCC_MMC_BUS_WIDTH_4,

		.cd	= TCC_MMC_PORT_NULL,
		.pwr	= TCC_GPF(10),
	},
	#endif
};
#endif

#if defined(TCC_MMC_SD_CARD_USED)
#define CONFIG_TCC_SD_PORT_RESTORE	//for SD Power-off in the STB
#endif

static int tccUsedSDportNum = TCC_MMC_TYPE_MAX;

int tcc8920_mmc_init(struct device *dev, int id)
{
	BUG_ON(id >= TCC_MMC_TYPE_MAX);

	if(mmc_ports[id].pwr != TCC_MMC_PORT_NULL)
	{
		gpio_request(mmc_ports[id].pwr, "sd_power");

		#if defined(TCC_MMC_SDIO_WIFI_USED)
		if(id == TCC_MMC_TYPE_WIFI)
		{
			//gpio_request(mmc_ports[id].pwr, "wifi_pre_power");
			gpio_direction_output(mmc_ports[id].pwr, 0);
			msleep(100);
			gpio_direction_output(mmc_ports[id].pwr, 1);

		}
		#endif
	}

	#if defined(CONFIG_TCC_SD_PORT_RESTORE)
	if(id == TCC_MMC_TYPE_SD)
	{
		gpio_request(mmc_ports[id].data0, "sd_d0");
		gpio_request(mmc_ports[id].data1, "sd_d1");
		gpio_request(mmc_ports[id].data2, "sd_d2");
		gpio_request(mmc_ports[id].data3, "sd_d3");

		gpio_request(mmc_ports[id].cmd, "sd_cmd");
		gpio_request(mmc_ports[id].clk, "sd_clk");
	}
	#endif

	tcc_gpio_config(mmc_ports[id].data0, mmc_ports[id].func | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data1, mmc_ports[id].func | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data2, mmc_ports[id].func | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data3, mmc_ports[id].func | GPIO_CD(1));

	if(mmc_ports[id].width == TCC_MMC_BUS_WIDTH_8)
	{
		tcc_gpio_config(mmc_ports[id].data4, mmc_ports[id].func | GPIO_CD(1));
		tcc_gpio_config(mmc_ports[id].data5, mmc_ports[id].func | GPIO_CD(1));
		tcc_gpio_config(mmc_ports[id].data6, mmc_ports[id].func | GPIO_CD(1));
		tcc_gpio_config(mmc_ports[id].data7, mmc_ports[id].func | GPIO_CD(1));
	}

	tcc_gpio_config(mmc_ports[id].cmd, mmc_ports[id].func | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].clk, mmc_ports[id].func | GPIO_CD(3));

	if(mmc_ports[id].cd != TCC_MMC_PORT_NULL)
	{
		tcc_gpio_config(mmc_ports[id].cd, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(mmc_ports[id].cd, "sd_cd");

		gpio_direction_input(mmc_ports[id].cd);
	}

	return 0;
}

int tcc8920_mmc_card_detect(struct device *dev, int id)
{
	BUG_ON(id >= TCC_MMC_TYPE_MAX);

	if(mmc_ports[id].cd == TCC_MMC_PORT_NULL)
		return 1;

	return gpio_get_value(mmc_ports[id].cd) ? 0 : 1;	
}

int tcc8920_mmc_suspend(struct device *dev, int id)
{
	#if defined(TCC_MMC_SDIO_WIFI_USED)
	if(id == TCC_MMC_TYPE_WIFI) {
		if(mmc_ports[id].pwr != TCC_MMC_PORT_NULL)
			gpio_direction_output(mmc_ports[id].pwr, 0);
	}
	#endif

	#if defined(CONFIG_TCC_SD_PORT_RESTORE)
	if (id == TCC_MMC_TYPE_SD)
	{
		/* GPIO mode */
		tcc_gpio_config(mmc_ports[id].data0, GPIO_FN(0));
		tcc_gpio_config(mmc_ports[id].data1, GPIO_FN(0));
		tcc_gpio_config(mmc_ports[id].data2, GPIO_FN(0));
		tcc_gpio_config(mmc_ports[id].data3, GPIO_FN(0));

		tcc_gpio_config(mmc_ports[id].cmd, GPIO_FN(0));
		tcc_gpio_config(mmc_ports[id].clk, GPIO_FN(0));

		/* output mode - 1:high, 0:low */
		gpio_direction_output(mmc_ports[id].data0, 0);
		gpio_direction_output(mmc_ports[id].data1, 0);
		gpio_direction_output(mmc_ports[id].data2, 0);
		gpio_direction_output(mmc_ports[id].data3, 0);

		gpio_direction_output(mmc_ports[id].cmd, 0);
		gpio_direction_output(mmc_ports[id].clk, 0);
	}
	#endif

	return 0;
}

int tcc8920_mmc_resume(struct device *dev, int id)
{
	#if defined(TCC_MMC_SDIO_WIFI_USED)
	if (id == TCC_MMC_TYPE_WIFI) {
		if(mmc_ports[id].pwr != TCC_MMC_PORT_NULL)
			gpio_direction_output(mmc_ports[id].pwr, 1);
	}
	#endif

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
	#if defined(TCC_MMC_SD_CARD_USED)
	return gpio_get_value(mmc_ports[TCC_MMC_TYPE_SD].cd) ? 0 : 1;
	#else
	return 0;
	#endif
}
//End

struct tcc_mmc_platform_data tcc8920_mmc_platform_data[] = {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)		// [0]:eMMC,   [1]:SD,   [2]:WiFi
	[TCC_MMC_TYPE_EMMC] = {
		.slot	= EMMC_SDMMC_PORT,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_8_BIT_DATA*/
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD0 Slot
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8920_mmc_init,
		.card_detect = tcc8920_mmc_card_detect,
		.cd_int_config = tcc8920_mmc_cd_int_config,
		.suspend = tcc8920_mmc_suspend,
		.resume = tcc8920_mmc_resume,
		.set_power = tcc8920_mmc_set_power,
		.set_bus_width = tcc8920_mmc_set_bus_width,

		.cd_int_num = -1,
		.cd_ext_irq = -1,
		.peri_name = EMMC_PERI_SDMMC,
		.io_name = EMMC_RB_SDMMC,
		.pic = EMMC_HwINT1_SD,
	},
	#endif
	#if defined(TCC_MMC_SD_CARD_USED)
	[TCC_MMC_TYPE_SD] = {
		.slot	= TFCD_SDMMC_PORT,
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
		.cd_ext_irq = TFCD_EXT_INT,
		.peri_name = TFCD_PERI_SDMMC,
		.io_name = TFCD_RB_SDMMC,
		.pic = TFCD_HwINT1_SD,
	},
	#endif
	#if defined(TCC_MMC_SDIO_WIFI_USED)
	[TCC_MMC_TYPE_WIFI] = {
		.slot	= WIFI_SDMMC_PORT,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,
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

		.cd_int_num = -1,
		.peri_name = WIFI_PERI_SDMMC,
		.io_name = WIFI_RB_SDMMC,
		.pic = WIFI_HwINT1_SD,
	},
	#endif

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

static int __init tcc8920_init_mmc(void)
{
	if (!machine_is_tcc8920st())
		return 0;

	printk("%s\n",__func__);

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

