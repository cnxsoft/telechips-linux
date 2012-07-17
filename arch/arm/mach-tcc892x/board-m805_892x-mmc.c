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
#include "board-m805_892x.h"

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

#if defined(CONFIG_M805S_8923_0XA)
#define TFCD_EXT_INT		EXTINT_GPIOE_13
#define TFCD_GPIO_PORT		TCC_GPE(13)
#elif defined(CONFIG_M805S_8925_0XX)
#define TFCD_EXT_INT		EXTINT_GPIOE_28
#define TFCD_GPIO_PORT		TCC_GPE(28)
#else
#define TFCD_EXT_INT		EXTINT_GPIOD_17
#define TFCD_GPIO_PORT		TCC_GPD(17)
#endif


typedef enum {
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
	TCC_MMC_TYPE_EMMC,
	#endif
	TCC_MMC_TYPE_SD,
//	TCC_MMC_TYPE_WIFI,
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

#if defined(CONFIG_M805S_8925_0XX)
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
	[TCC_MMC_TYPE_SD] = {
		.data0	= TCC_GPF(26),
		.data1	= TCC_GPF(25),
		.data2	= TCC_GPF(24),
		.data3	= TCC_GPF(23),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPF(27),
		.clk	= TCC_GPF(28),
		.func	= GPIO_FN(3),
		.width	= TCC_MMC_BUS_WIDTH_4,

		.cd 	= TFCD_GPIO_PORT,
		.pwr	= TCC_MMC_PORT_NULL,
	},
};
#else
static struct mmc_port_config mmc_ports[] = {
	[TCC_MMC_TYPE_SD] = {
		.data0  = TCC_GPC(2),
		.data1  = TCC_GPC(3),
		.data2  = TCC_GPC(4),
		.data3  = TCC_GPC(5),
		.data4  = TCC_MMC_PORT_NULL,
		.data5  = TCC_MMC_PORT_NULL,
		.data6  = TCC_MMC_PORT_NULL,
		.data7  = TCC_MMC_PORT_NULL,
		.cmd    = TCC_GPC(1),
		.clk    = TCC_GPC(0),
		.func   = GPIO_FN(3),
		.width  = TCC_MMC_BUS_WIDTH_4,

		.cd     = TFCD_GPIO_PORT,
		.pwr    = TCC_MMC_PORT_NULL,
	},
};
#endif

static int tccUsedSDportNum = TCC_MMC_TYPE_MAX;
static int TCC_SDMMC_DRIVE_STRENGTH = GPIO_CD(1);

int m805_892x_mmc_init(struct device *dev, int id)
{
	BUG_ON(id >= tccUsedSDportNum);

	if(mmc_ports[id].pwr != TCC_MMC_PORT_NULL)
		gpio_request(mmc_ports[id].pwr, "sd_power");

	tcc_gpio_config(mmc_ports[id].data0, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
	tcc_gpio_config(mmc_ports[id].data1, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
	tcc_gpio_config(mmc_ports[id].data2, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
	tcc_gpio_config(mmc_ports[id].data3, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);

	if(mmc_ports[id].width == TCC_MMC_BUS_WIDTH_8)
	{
		tcc_gpio_config(mmc_ports[id].data4, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
		tcc_gpio_config(mmc_ports[id].data5, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
		tcc_gpio_config(mmc_ports[id].data6, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
		tcc_gpio_config(mmc_ports[id].data7, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
	}

	tcc_gpio_config(mmc_ports[id].cmd, mmc_ports[id].func | TCC_SDMMC_DRIVE_STRENGTH);
	tcc_gpio_config(mmc_ports[id].clk, mmc_ports[id].func | GPIO_CD(3));

	if(mmc_ports[id].cd != TCC_MMC_PORT_NULL)
	{
		tcc_gpio_config(mmc_ports[id].cd, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(mmc_ports[id].cd, "sd_cd");

		gpio_direction_input(mmc_ports[id].cd);
	}

	return 0;
}

int m805_892x_mmc_card_detect(struct device *dev, int id)
{
	BUG_ON(id >= TCC_MMC_TYPE_MAX);

	if(mmc_ports[id].cd == TCC_MMC_PORT_NULL)
		return 1;

	return gpio_get_value(mmc_ports[id].cd) ? 0 : 1;	
}

int m805_892x_mmc_suspend(struct device *dev, int id)
{
	return 0;
}

int m805_892x_mmc_resume(struct device *dev, int id)
{
 	return 0;
}

int m805_892x_mmc_set_power(struct device *dev, int id, int on)
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

int m805_892x_mmc_set_bus_width(struct device *dev, int id, int width)
{
	return 0;
}

int m805_892x_mmc_cd_int_config(struct device *dev, int id, unsigned int cd_irq)
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

struct tcc_mmc_platform_data tcc8920_mmc_platform_data[] = {
#if defined(CONFIG_M805S_8925_0XX)
	#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)		// [0]:eMMC,   [1]:SD,   [2]:WiFi
	[TCC_MMC_TYPE_EMMC] = {
		.slot	= 4,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_8_BIT_DATA*/
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD0 Slot
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= m805_892x_mmc_init,
		.card_detect = m805_892x_mmc_card_detect,
		.cd_int_config = m805_892x_mmc_cd_int_config,
		.suspend = m805_892x_mmc_suspend,
		.resume = m805_892x_mmc_resume,
		.set_power = m805_892x_mmc_set_power,
		.set_bus_width = m805_892x_mmc_set_bus_width,

		.cd_int_num = -1,
		//.cd_irq_num = INT_EI5,
		.cd_ext_irq = -1,
		.peri_name = PERI_SDMMC0,
		.io_name = RB_SDMMC0CONTROLLER,
		.pic = HwINT1_SD0,
	},
	#endif
	[TCC_MMC_TYPE_SD] = {
		.slot	= 6,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/* MMC_CAP_8_BIT_DATA */
			| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000, /* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= m805_892x_mmc_init,
		.card_detect = m805_892x_mmc_card_detect,
		.cd_int_config = m805_892x_mmc_cd_int_config,
		.suspend = m805_892x_mmc_suspend,
		.resume = m805_892x_mmc_resume,
		.set_power = m805_892x_mmc_set_power,
		.set_bus_width = m805_892x_mmc_set_bus_width,
	
		.cd_int_num = HwINT0_EI4,
		.cd_irq_num = INT_EI4,
		.cd_ext_irq = TFCD_EXT_INT,
		.peri_name = PERI_SDMMC2,
		.io_name = RB_SDMMC2CONTROLLER,
		.pic = HwINT1_SD2,
	},
#else
	[TCC_MMC_TYPE_SD] = {
		.slot	= 0,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/* MMC_CAP_8_BIT_DATA */
			| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= m805_892x_mmc_init,
		.card_detect = m805_892x_mmc_card_detect,
		.cd_int_config = m805_892x_mmc_cd_int_config,
		.suspend = m805_892x_mmc_suspend,
		.resume	= m805_892x_mmc_resume,
		.set_power = m805_892x_mmc_set_power,
		.set_bus_width = m805_892x_mmc_set_bus_width,

		.cd_int_num = HwINT0_EI4,
		.cd_irq_num = INT_EI4,
		.cd_ext_irq = TFCD_EXT_INT,
		.peri_name = PERI_SDMMC0,
		.io_name = RB_SDMMC0CONTROLLER,
		.pic = HwINT1_SD0,
	},
#endif
};

static int __init m805_892x_init_mmc(void)
{
	if (!machine_is_m805_892x())
		return 0;

	tccUsedSDportNum = TCC_MMC_TYPE_MAX;

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
device_initcall(m805_892x_init_mmc);
#endif

