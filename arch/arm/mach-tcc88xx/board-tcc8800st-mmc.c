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
#include "board-tcc8800st.h"

#if defined(CONFIG_MMC_TCC_SDHC)
typedef enum {
	TCC_MMC_TYPE_SD,
#if defined(CONFIG_MMC_TCC_PORT7)
	TCC_MMC_TYPE_WIFI,
#endif
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

static struct mmc_port_config mmc_ports[] = {
	[0] = {
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

		.pwr	= 0,
	},
#if defined(CONFIG_MMC_TCC_PORT7)
	[1] = {
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
		.cd		= 0,
#if defined(CONFIG_WIFI_SUB_BOARD)
		.pwr = GPIO_SDWF_RST,
#else
		.pwr	= 0,
#endif
	},
#endif
};

static int tcc8800st_mmc_init(struct device *dev, int id)
{
	BUG_ON(id >= TCC_MMC_TYPE_MAX);

	gpio_request(mmc_ports[id].pwr, "sd_power");

	if(id == TCC_MMC_TYPE_SD)
	{
		gpio_request(mmc_ports[id].data0, "sd_d0");
		gpio_request(mmc_ports[id].data1, "sd_d1");
		gpio_request(mmc_ports[id].data2, "sd_d2");
		gpio_request(mmc_ports[id].data3, "sd_d3");

		gpio_request(mmc_ports[id].cmd, "sd_cmd");
		gpio_request(mmc_ports[id].clk, "sd_clk");
	}

#if defined(CONFIG_WIFI_SUB_BOARD)
	if(id == TCC_MMC_TYPE_WIFI)
	{
		gpio_request(GPIO_SD1_ON, "wifi_pre_power");
		gpio_direction_output(GPIO_SD1_ON, 1);
        
                gpio_request(TCC_GPG(11),"wifi_rst");

                gpio_direction_output(TCC_GPG(11), 0);
                msleep(100);
                gpio_direction_output(TCC_GPG(11), 1);
        }
#endif

	tcc_gpio_config(mmc_ports[id].data0, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data1, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data2, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].data3, GPIO_FN(2) | GPIO_CD(1));

	tcc_gpio_config(mmc_ports[id].cmd, GPIO_FN(2) | GPIO_CD(1));
	tcc_gpio_config(mmc_ports[id].clk, GPIO_FN(2) | GPIO_CD(3));

	tcc_gpio_config(mmc_ports[id].cd, GPIO_FN(0));
	gpio_request(mmc_ports[id].cd, "sd_cd");
	
	gpio_direction_input(mmc_ports[id].cd);

	return 0;
}

static int tcc8800st_mmc_card_detect(struct device *dev, int id)
{
	BUG_ON(id >= TCC_MMC_TYPE_MAX);

#if defined(CONFIG_WIFI_SUB_BOARD)
	if(id == TCC_MMC_TYPE_WIFI)
		return 1;
#endif

	return gpio_get_value(mmc_ports[id].cd) ? 0 : 1;	

}

static int tcc8800st_mmc_suspend(struct device *dev, int id)
{
#if defined(CONFIG_MMC_TCC_PORT7)
	if(id == TCC_MMC_TYPE_WIFI) {
		gpio_direction_output(GPIO_SD1_ON, 0);
	}
#endif

	return 0;
}

static int tcc8800st_mmc_resume(struct device *dev, int id)
{
#if defined(CONFIG_MMC_TCC_PORT7)
	if (id == TCC_MMC_TYPE_WIFI) {
		gpio_direction_output(GPIO_SD1_ON, 1);
	}
#endif

 	return 0;
}

static int tcc8800st_mmc_set_power(struct device *dev, int id, int on)
{
	if (on)
	{
		/* power */
		gpio_direction_output(mmc_ports[id].pwr, 1);
		mdelay(1);
	}
	else
	{
#if 0	// sd/mmc power always turned on
		//gpio_direction_output(mmc_ports[id].pwr, 0);

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

static int tcc8800st_mmc_set_bus_width(struct device *dev, int id, int width)
{
	return 0;
}

static int tcc8800st_mmc_cd_int_config(struct device *dev, int id, unsigned int cd_irq)
{
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);	

	if(id == TCC_MMC_TYPE_SD)
	{
		BITCSET(pPIC->EI37SEL, 0x00000010, 0x00000010);		
		BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT4_MASK, HwEINTSEL1_EINT4(SEL_GPIOF13));
	}			
#if defined(CONFIG_MMC_TCC_PORT7)
	else if(id == TCC_MMC_TYPE_WIFI)
	{
		BITCSET(pPIC->EI37SEL, 0x00000020, 0x00000020);
		BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT5_MASK, HwEINTSEL1_EINT5(SEL_GPIOE25));		
	}
#endif
	else
	{
	}	

	pPIC->SEL1		|= cd_irq;
	pPIC->INTMSK1	|= cd_irq;
	pPIC->MODE1 	&= (~(cd_irq)); // edge trigger
	pPIC->MODEA1	|= (cd_irq);	//both edge
	pPIC->IEN1		|= cd_irq;		
	pPIC->CLR1		|= cd_irq;	

	return 0;
}


static struct tcc_mmc_platform_data tcc8800st_mmc_platform_data[] = {
	[0] = {
		.slot	= 5,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/* MMC_CAP_8_BIT_DATA */
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD2 Slot
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8800st_mmc_init,
		.card_detect = tcc8800st_mmc_card_detect,
		.cd_int_config = tcc8800st_mmc_cd_int_config,
		.suspend = tcc8800st_mmc_suspend,
		.resume	= tcc8800st_mmc_resume,
		.set_power = tcc8800st_mmc_set_power,
		.set_bus_width = tcc8800st_mmc_set_bus_width,

		.cd_int_num = HwINT1_EI4,
		.cd_irq_num = INT_EI4,		
		.peri_name = PERI_SDMMC1,
		.io_name = RB_SDMMC1CONTROLLER,
		.pic = HwINT1_SD1, 
	},
	[1] = {
		.slot	= 1,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
	#if defined(CONFIG_MMC_TCC_PORT7)
			/*| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED*/,		// SD1 Slot
	#endif
		.f_min	= 100000,
//		.f_max	= 48000000,	/* support highspeed mode */
		.f_max	= 24000000,	// Only Atheros WiFi(AR6102)
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= tcc8800st_mmc_init,
		.card_detect = tcc8800st_mmc_card_detect,
		.cd_int_config = tcc8800st_mmc_cd_int_config,		
		.suspend = tcc8800st_mmc_suspend,
		.resume	= tcc8800st_mmc_resume,
		.set_power = tcc8800st_mmc_set_power,
		.set_bus_width = tcc8800st_mmc_set_bus_width,

		.cd_int_num = HwINT1_EI5,
		.cd_irq_num = INT_EI5,				
		.peri_name = PERI_SDMMC3,
		.io_name = RB_SDMMC3CONTROLLER,
		.pic = HwINT1_SD3,		
	},
};

static int __init tcc8800st_init_mmc(void)
{
	if (!machine_is_tcc8800st())
		return 0;

	printk("%s\n",__func__);

#if defined(CONFIG_MMC_TCC_SDHC0)
	tcc_sdhc0_device.dev.platform_data = &tcc8800st_mmc_platform_data[0];
	platform_device_register(&tcc_sdhc0_device);
#endif
#if defined(CONFIG_MMC_TCC_SDHC1)
	tcc_sdhc1_device.dev.platform_data = &tcc8800st_mmc_platform_data[1];
	platform_device_register(&tcc_sdhc1_device);
#endif

	return 0;
}
device_initcall(tcc8800st_init_mmc);
#endif

