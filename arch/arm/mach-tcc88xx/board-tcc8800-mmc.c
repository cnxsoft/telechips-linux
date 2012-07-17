#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/mmc/host.h>
#include <linux/gpio.h>

#include <mach/mmc.h>

#include <mach/gpio.h>
//#include <mach/bsp.h>
#include <mach/bsp.h>
#include <asm/mach-types.h>

#include "devices.h"
#include "board-tcc8800.h"

#if defined(CONFIG_MMC_TCC_SDHC)
typedef enum {
#if defined(CONFIG_MMC_TCC_PORT3)
	TCC_MMC_TYPE_EMMC,
#endif
	TCC_MMC_TYPE_SD,
#if defined(CONFIG_MMC_TCC_PORT7)
	TCC_MMC_TYPE_WIFI,
#endif
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

static struct mmc_port_config mmc_ports[] = {
#if defined(CONFIG_MMC_TCC_PORT3)
	[TCC_MMC_TYPE_EMMC] = {
		.data0	= TCC_GPF(0),
		.data1	= TCC_GPF(1),
		.data2	= TCC_GPF(2),
		.data3	= TCC_GPF(3),
		.data4	= TCC_GPF(4),
		.data5	= TCC_GPF(5),
		.data6	= TCC_GPF(6),
		.data7	= TCC_GPF(7),
		.cmd	= TCC_GPF(8),
		.clk	= TCC_GPF(9),
		.cd = TCC_GPF(10),

		.pwr = GPIO_SD0_ON,
	},
#endif
	[TCC_MMC_TYPE_SD] = {
		.data0	= TCC_GPF(18),
		.data1	= TCC_GPF(19),
		.data2	= TCC_GPF(20),
		.data3	= TCC_GPF(21),
		.data4	= 0,
		.data5	= 0,
		.data6	= 0,
		.data7	= 0,
		.cmd	= TCC_GPF(22),
		.clk	= TCC_GPF(23),
		.cd = TCC_GPF(13),

		.pwr = GPIO_SD2_ON,
	},
#if defined(CONFIG_MMC_TCC_PORT7)
	[TCC_MMC_TYPE_WIFI] = {
		.data0	= TCC_GPE(28),
		.data1	= TCC_GPE(29),
		.data2	= TCC_GPE(30),
		.data3	= TCC_GPE(31),
		.data4	= 0,
		.data5	= 0,
		.data6	= 0,
		.data7	= 0,
		.cmd	= TCC_GPE(26),
		.clk	= TCC_GPE(27),
		.cd = TCC_GPE(25),
#if defined(CONFIG_WIFI_SUB_BOARD)
		.pwr = GPIO_SDWF_RST,
#else
		.pwr = GPIO_SD1_ON,
#endif
	},
#endif
};

int tcc8800_mmc_init(struct device *dev, int id)
{
	BUG_ON(id >= TCC_MMC_TYPE_MAX);

	gpio_request(mmc_ports[id].pwr, "sd_power");

#if defined(CONFIG_MMC_TCC_PORT7)
	if(id == TCC_MMC_TYPE_WIFI)
	{
		gpio_request(GPIO_SD1_ON, "wifi_pre_power");
		gpio_direction_output(GPIO_SD1_ON, 0);
		msleep(100);
		gpio_direction_output(GPIO_SD1_ON, 1);
        
#if defined(CONFIG_WIFI_SUB_BOARD)
		gpio_request(TCC_GPG(11),"wifi_rst");

		gpio_direction_output(TCC_GPG(11), 0);
		msleep(100);
		gpio_direction_output(TCC_GPG(11), 1);
#endif
	}
#endif

	if(id == TCC_MMC_TYPE_SD)
	{
		gpio_request(mmc_ports[id].data0, "sd_d0");
		gpio_request(mmc_ports[id].data1, "sd_d1");
		gpio_request(mmc_ports[id].data2, "sd_d2");
		gpio_request(mmc_ports[id].data3, "sd_d3");

		gpio_request(mmc_ports[id].cmd, "sd_cmd");
		gpio_request(mmc_ports[id].clk, "sd_clk");
	}

	tcc_gpio_config(mmc_ports[id].data0, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data1, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data2, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data3, GPIO_FN(2) | GPIO_CD(1));

#if defined(CONFIG_MMC_TCC_PORT3)
	if(id == TCC_MMC_TYPE_EMMC)
	{
		tcc_gpio_config(mmc_ports[id].data4, GPIO_FN(2) | GPIO_CD(1));
		tcc_gpio_config(mmc_ports[id].data5, GPIO_FN(2) | GPIO_CD(1));
		tcc_gpio_config(mmc_ports[id].data6, GPIO_FN(2) | GPIO_CD(1));
		tcc_gpio_config(mmc_ports[id].data7, GPIO_FN(2) | GPIO_CD(1));
	}				
#endif	
	
	tcc_gpio_config(mmc_ports[id].cmd, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].clk, GPIO_FN(2) | GPIO_CD(3));

	tcc_gpio_config(mmc_ports[id].cd, GPIO_FN(0));
	gpio_request(mmc_ports[id].cd, "sd_cd");
	
	gpio_direction_input(mmc_ports[id].cd);

	return 0;
}

int tcc8800_mmc_card_detect(struct device *dev, int id)
{
	BUG_ON(id >= TCC_MMC_TYPE_MAX);

#if defined(CONFIG_WIFI_SUB_BOARD)
	if(id == TCC_MMC_TYPE_WIFI)
		return 1;
#endif

#if defined(CONFIG_MMC_TCC_PORT3)
	if(id == TCC_MMC_TYPE_EMMC)
		return 1;
#endif

	return gpio_get_value(mmc_ports[id].cd) ? 0 : 1;	

}

int tcc8800_mmc_suspend(struct device *dev, int id)
{
#if defined(CONFIG_MMC_TCC_PORT7)
	if(id == TCC_MMC_TYPE_WIFI) {
		gpio_direction_output(GPIO_SD1_ON, 0);
	} 
#endif

	return 0;
}

int tcc8800_mmc_resume(struct device *dev, int id)
{
#if defined(CONFIG_MMC_TCC_PORT7)
	if (id == TCC_MMC_TYPE_WIFI) {
		gpio_direction_output(GPIO_SD1_ON, 1);
	}
#endif

 	return 0;
}

int tcc8800_mmc_set_power(struct device *dev, int id, int on)
{
	if (on) {
		/* power */
		gpio_direction_output(mmc_ports[id].pwr, 1);
		mdelay(1);
	} else {

#if (0) //Bruce_temp.. sd/mmc power always turned on
		// gpio_direction_output(mmc_ports[id].pwr, 0);

		if (id == TCC_MMC_TYPE_SD)
		{
			/* SD2 - GPIO mode */
			tcc_gpio_config(mmc_ports[id].data0, GPIO_FN(0));
			tcc_gpio_config(mmc_ports[id].data1, GPIO_FN(0));
			tcc_gpio_config(mmc_ports[id].data2, GPIO_FN(0));
			tcc_gpio_config(mmc_ports[id].data3, GPIO_FN(0));

			tcc_gpio_config(mmc_ports[id].cmd, GPIO_FN(0));
			tcc_gpio_config(mmc_ports[id].clk, GPIO_FN(0));

			/* output mode */
			gpio_direction_output(mmc_ports[id].data0, 1);
			gpio_direction_output(mmc_ports[id].data1, 1);
			gpio_direction_output(mmc_ports[id].data2, 1);
			gpio_direction_output(mmc_ports[id].data3, 1);

			gpio_direction_output(mmc_ports[id].cmd, 1);
			gpio_direction_output(mmc_ports[id].clk, 1);
		}
#endif

		//mdelay(10);
	}

	return 0;
}

int tcc8800_mmc_set_bus_width(struct device *dev, int id, int width)
{
	return 0;
}

int tcc8800_mmc_cd_int_config(struct device *dev, int id, unsigned int cd_irq)
{
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);	

#if defined(CONFIG_MMC_TCC_PORT3)
	if(id == TCC_MMC_TYPE_EMMC)
	{
		BITCSET(pPIC->EI37SEL, 0x00000020, 0x00000020);
		#if 1	// TCC88xx EVM Board Revision
		// TCC8801F(0615/0621/0623) & TCC8803(0614/0622/0624)
		if(system_rev == 0x0614 || system_rev == 0x0615 || system_rev == 0x0621 || system_rev == 0x0622 || system_rev == 0x0623 || system_rev == 0x0624)
			BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT5_MASK, HwEINTSEL1_EINT5(SEL_GPIOA13));
		else
		#endif
			BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT5_MASK, HwEINTSEL1_EINT5(SEL_GPIOF10));
	}
	else
#elif defined(CONFIG_MMC_TCC_PORT7)
	if(id == TCC_MMC_TYPE_WIFI)
	{
		BITCSET(pPIC->EI37SEL, 0x00000020, 0x00000020);
		BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT5_MASK, HwEINTSEL1_EINT5(SEL_GPIOE25));		
	}
	else
#endif
	if(id == TCC_MMC_TYPE_SD)
	{
		BITCSET(pPIC->EI37SEL, 0x00000010, 0x00000010);
		#if 1	// TCC88xx EVM Board Revision
		// TCC8801F(0615/0621/0623) & TCC8803(0614/0622/0624)
		if(system_rev == 0x0614 || system_rev == 0x0615 || system_rev == 0x0621 || system_rev == 0x0622 || system_rev == 0x0623 || system_rev == 0x0624)
			BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT4_MASK, HwEINTSEL1_EINT4(SEL_GPIOA15));
		else
		#endif
			BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT4_MASK, HwEINTSEL1_EINT4(SEL_GPIOF13));
	}
	else
	{
		return -1;
	}	

	pPIC->SEL1		|= cd_irq;
	pPIC->INTMSK1	|= cd_irq;
	pPIC->MODE1 	&= (~(cd_irq)); // edge trigger
	pPIC->MODEA1	|= (cd_irq);	//both edge
	pPIC->IEN1		|= cd_irq;		
	pPIC->CLR1		|= cd_irq;	

	return 0;
}


struct tcc_mmc_platform_data tcc8800_mmc_platform_data[] = {
	#if defined(CONFIG_MMC_TCC_PORT3)
	[TCC_MMC_TYPE_EMMC] = {
		.slot	= 1,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			| MMC_CAP_8_BIT_DATA
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD0 Slot
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8800_mmc_init,
		.card_detect = tcc8800_mmc_card_detect,
		.cd_int_config = tcc8800_mmc_cd_int_config,
		.suspend = tcc8800_mmc_suspend,
		.resume	= tcc8800_mmc_resume,
		.set_power = tcc8800_mmc_set_power,
		.set_bus_width = tcc8800_mmc_set_bus_width,

		.cd_int_num = HwINT1_EI5,
		.cd_irq_num = INT_EI5,
		.peri_name = PERI_SDMMC3,
		.io_name = RB_SDMMC3CONTROLLER,
		.pic = HwINT1_SD3,
	},
	#endif
	[TCC_MMC_TYPE_SD] = {
		.slot	= 5,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/* MMC_CAP_8_BIT_DATA */
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD2 Slot
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8800_mmc_init,
		.card_detect = tcc8800_mmc_card_detect,
		.cd_int_config = tcc8800_mmc_cd_int_config,
		.suspend = tcc8800_mmc_suspend,
		.resume	= tcc8800_mmc_resume,
		.set_power = tcc8800_mmc_set_power,
		.set_bus_width = tcc8800_mmc_set_bus_width,

		.cd_int_num = HwINT1_EI4,
		.cd_irq_num = INT_EI4,
		.peri_name = PERI_SDMMC1,
		.io_name = RB_SDMMC1CONTROLLER,
		.pic = HwINT1_SD1,
	},
	#if defined(CONFIG_MMC_TCC_PORT7)
	[TCC_MMC_TYPE_WIFI] = {
		.slot	= 1,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD1 Slot
		.f_min	= 100000,
//		.f_max	= 48000000,	/* support highspeed mode */
		.f_max	= 24000000,	// Only Atheros WiFi(AR6102)
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8800_mmc_init,
		.card_detect = tcc8800_mmc_card_detect,
		.cd_int_config = tcc8800_mmc_cd_int_config,
		.suspend = tcc8800_mmc_suspend,
		.resume	= tcc8800_mmc_resume,
		.set_power = tcc8800_mmc_set_power,
		.set_bus_width = tcc8800_mmc_set_bus_width,

		.cd_int_num = HwINT1_EI5,
		.cd_irq_num = INT_EI5,
		.peri_name = PERI_SDMMC3,
		.io_name = RB_SDMMC3CONTROLLER,
		.pic = HwINT1_SD3,
	},
	#endif
	[2] = {
		.slot	= 2,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_8_BIT_DATA */
			  | MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
//		.cd_int_num =INT_EI9,				
		.init	= tcc8800_mmc_init,
		.card_detect = tcc8800_mmc_card_detect,
		.suspend = tcc8800_mmc_suspend,
		.resume	= tcc8800_mmc_resume,
		.set_power = tcc8800_mmc_set_power,
		.set_bus_width = tcc8800_mmc_set_bus_width,

		.peri_name = PERI_SDMMC2,
		.io_name = RB_SDMMC2CONTROLLER,
		.pic = HwINT1_SD2,		
	},	
	[3] = {
		.slot	= 2,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_8_BIT_DATA */
			  | MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
//		.cd_int_num =INT_EI9,				
		.init	= tcc8800_mmc_init,
		.card_detect = tcc8800_mmc_card_detect,
		.suspend = tcc8800_mmc_suspend,
		.resume	= tcc8800_mmc_resume,
		.set_power = tcc8800_mmc_set_power,
		.set_bus_width = tcc8800_mmc_set_bus_width,

		.peri_name = PERI_SDMMC0,
		.io_name = RB_SDMMC0CONTROLLER,
		.pic = HwINT1_SD0,	
	},	
};

static int __init tcc8800_init_mmc(void)
{
	if (!machine_is_tcc8800())
		return 0;

	printk("%s\n",__func__);

	#if 1	// TCC88xx EVM Board Revision
	// TCC8803(0614/0622/0624)
	if(system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624)
	{
		// Change Card Detect
		mmc_ports[TCC_MMC_TYPE_SD].cd = TCC_GPA(15);
		#if defined(CONFIG_MMC_TCC_PORT3)
		mmc_ports[TCC_MMC_TYPE_EMMC].cd = TCC_GPA(13);
		#endif

		// Change Power
		mmc_ports[TCC_MMC_TYPE_SD].pwr = TCC_GPF(13);
		#if defined(CONFIG_MMC_TCC_PORT3)
		mmc_ports[TCC_MMC_TYPE_EMMC].pwr = TCC_GPF(10);
		#endif

		// Support High-Speed mode
		tcc8800_mmc_platform_data[TCC_MMC_TYPE_SD].caps |= MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED;
		#if defined(CONFIG_MMC_TCC_PORT3)
		tcc8800_mmc_platform_data[TCC_MMC_TYPE_EMMC].caps |= MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED;
		#endif
	}
	// TCC8801F(0615/0621/0623)
	else if(system_rev == 0x0615 || system_rev == 0x0621 || system_rev == 0x0623)
	{
		// Change Card Detect
		mmc_ports[TCC_MMC_TYPE_SD].cd = TCC_GPA(15);
		#if defined(CONFIG_MMC_TCC_PORT3)
		mmc_ports[TCC_MMC_TYPE_EMMC].cd = TCC_GPA(13);
		#endif
	}
	#endif

#if defined(CONFIG_MMC_TCC_SDHC0)
	tcc_sdhc0_device.dev.platform_data = &tcc8800_mmc_platform_data[0];
	platform_device_register(&tcc_sdhc0_device);
#endif
#if defined(CONFIG_MMC_TCC_SDHC1)
	tcc_sdhc1_device.dev.platform_data = &tcc8800_mmc_platform_data[1];
	platform_device_register(&tcc_sdhc1_device);
#endif
#if defined(CONFIG_MMC_TCC_SDHC2)
	tcc_sdhc2_device.dev.platform_data = &tcc8800_mmc_platform_data[2];
	platform_device_register(&tcc_sdhc2_device);
#endif
#if defined(CONFIG_MMC_TCC_SDHC3)
	tcc_sdhc3_device.dev.platform_data = &tcc8800_mmc_platform_data[3];
	platform_device_register(&tcc_sdhc3_device);
#endif

	return 0;
}
device_initcall(tcc8800_init_mmc);
#endif

