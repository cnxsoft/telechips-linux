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
//#include "board-m801_88.h"

#if defined(CONFIG_MMC_TCC_SDHC)
typedef enum {
	TCC_MMC_TYPE_SD = 0,
	TCC_MMC_TYPE_WIFI,
	TCC_MMC_TYPE_MAX
} tcc_mmc_type;

#define TCC_MMC_PORT_NULL	0x0FFFFFFF

static struct mmc_port_config mmc_ports[] = {
	[TCC_MMC_TYPE_SD] = {
		.data0	= TCC_GPF(21),
		.data1	= TCC_GPF(20),
		.data2	= TCC_GPF(19),
		.data3	= TCC_GPF(18),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPF(22),
		.clk	= TCC_GPF(23),
		.cd	= TCC_GPF(17),

		.pwr = TCC_MMC_PORT_NULL,
	},
	[TCC_MMC_TYPE_WIFI] = {
		.data0	= TCC_GPE(28),
		.data1	= TCC_GPE(29),
		.data2	= TCC_GPE(30),
		.data3	= TCC_GPE(31),
		.data4	= TCC_MMC_PORT_NULL,
		.data5	= TCC_MMC_PORT_NULL,
		.data6	= TCC_MMC_PORT_NULL,
		.data7	= TCC_MMC_PORT_NULL,
		.cmd	= TCC_GPE(26),
		.clk	= TCC_GPE(27),
		.cd	= TCC_MMC_PORT_NULL,

		.pwr = TCC_MMC_PORT_NULL,
	},
};

int m801_88_mmc_init(struct device *dev, int id)
{
	BUG_ON(id > TCC_MMC_TYPE_MAX);

	tcc_gpio_config(mmc_ports[id].data0, GPIO_FN(2) | GPIO_CD(0));
	tcc_gpio_config(mmc_ports[id].data1, GPIO_FN(2) | GPIO_CD(0));
	tcc_gpio_config(mmc_ports[id].data2, GPIO_FN(2) | GPIO_CD(0));
	tcc_gpio_config(mmc_ports[id].data3, GPIO_FN(2) | GPIO_CD(0));
	tcc_gpio_config(mmc_ports[id].cmd, GPIO_FN(2));
	tcc_gpio_config(mmc_ports[id].clk, GPIO_FN(2) | GPIO_CD(3));

	if(id == TCC_MMC_TYPE_WIFI)
	{
		if(system_rev == 0x0005 || system_rev == 0x0006)
		{
			gpio_request(TCC_GPG(11), "wifi_pwr");
			gpio_direction_output(TCC_GPG(11), 1);
		}
		else{
			gpio_request(TCC_GPG(11),"wifi_rst");
			gpio_request(TCC_GPG(10),"wifi_on");

			gpio_direction_output(TCC_GPG(11), 1);
			gpio_direction_output(TCC_GPG(10), 1);
			gpio_direction_output(TCC_GPG(11), 0);
			msleep(10);
			gpio_direction_output(TCC_GPG(11), 1);
		}
	}

	if(id == TCC_MMC_TYPE_SD){
		tcc_gpio_config(mmc_ports[id].cd, GPIO_FN(0));
		gpio_request(mmc_ports[id].cd, "sd_cd");

		gpio_direction_input(mmc_ports[id].cd);
	}

	return 0;
}

int m801_88_mmc_card_detect(struct device *dev, int id)
{
	BUG_ON(id > TCC_MMC_TYPE_MAX);

	if(id == TCC_MMC_TYPE_WIFI)
		return 1;

	return gpio_get_value(mmc_ports[id].cd) ? 0 : 1;
}

int m801_88_mmc_suspend(struct device *dev, int id)
{
	if(system_rev == 0x0005 || system_rev == 0x0006)
	{
		gpio_direction_output(TCC_GPG(11), 0);
	}
	else{
		if(id == TCC_MMC_TYPE_WIFI) 
			gpio_direction_output(TCC_GPG(10),0);
	}


	return 0;
}

int m801_88_mmc_resume(struct device *dev, int id)
{
	if(system_rev ==0x0005 || system_rev == 0x0006)
	{
		gpio_direction_output(TCC_GPG(11), 1);
	}
	else{
		if(id == TCC_MMC_TYPE_WIFI) 
			gpio_direction_output(TCC_GPG(10),1);
	}

 	return 0;
}

int m801_88_mmc_set_power(struct device *dev, int id, int on)
{
	//if (on) {
	//} else {
	//}

	return 0;
}

int m801_88_mmc_set_bus_width(struct device *dev, int id, int width)
{
	return 0;
}

int m801_88_mmc_cd_int_config(struct device *dev, int id, unsigned int cd_irq)
{
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);

	if(id == TCC_MMC_TYPE_SD)
	{
		BITCSET(pPIC->EI37SEL, 0x00000010, 0x00000010);

		if(system_rev == 0x0004 || system_rev == 0x0005 || system_rev == 0x0006)  // system_rev : 0x0004 = M805 V0.4A
			BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT4_MASK, HwEINTSEL1_EINT4(SEL_GPIOE25));
		else
			BITCSET(pGPIO->EINTSEL1, HwEINTSEL1_EINT4_MASK, HwEINTSEL1_EINT4(SEL_GPIOF17));
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

struct tcc_mmc_platform_data m801_88_mmc_platform_data[] = {
	[TCC_MMC_TYPE_SD] = {
		.slot	= 5,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/* MMC_CAP_8_BIT_DATA */
			| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= m801_88_mmc_init,
		.card_detect = m801_88_mmc_card_detect,
		.cd_int_config = m801_88_mmc_cd_int_config,
		.suspend = m801_88_mmc_suspend,
		.resume	= m801_88_mmc_resume,
		.set_power = m801_88_mmc_set_power,
		.set_bus_width = m801_88_mmc_set_bus_width,
		.cd_int_num = HwINT1_EI4,
		.cd_irq_num = INT_EI4,
		.peri_name = PERI_SDMMC1,
		.io_name = RB_SDMMC1CONTROLLER,
		.pic = HwINT1_SD1,
	},
	[TCC_MMC_TYPE_WIFI] = {
		.slot	= 1,
		.caps	= MMC_CAP_SDIO_IRQ | MMC_CAP_4_BIT_DATA
			/*| MMC_CAP_8_BIT_DATA */
			| MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED,
		.f_min	= 100000,
		.f_max	= 48000000,	/* support highspeed mode */
		.ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34,
		.init	= m801_88_mmc_init,
		.card_detect = m801_88_mmc_card_detect,
		.cd_int_config = m801_88_mmc_cd_int_config,
		.suspend = m801_88_mmc_suspend,
		.resume	= m801_88_mmc_resume,
		.set_power = m801_88_mmc_set_power,
		.set_bus_width = m801_88_mmc_set_bus_width,
		.cd_int_num =-1,
		.peri_name = PERI_SDMMC3,
		.io_name = RB_SDMMC3CONTROLLER,
		.pic = HwINT1_SD3,
	},
};

static int __init m801_88_init_mmc(void)
{
	if (!machine_is_m801_88())
		return 0;

	printk("%s\n",__func__);

	// system_rev : 0x0004 = M805 V0.4A
	if(system_rev == 0x0004 || system_rev == 0x0005 || system_rev == 0x0006)
	{
		// Change Card Detect
		mmc_ports[TCC_MMC_TYPE_SD].cd = TCC_GPE(25);
	}

#if defined(CONFIG_MMC_TCC_SDHC0)
	tcc_sdhc0_device.dev.platform_data = &m801_88_mmc_platform_data[0];
	platform_device_register(&tcc_sdhc0_device);
#endif
#if defined(CONFIG_MMC_TCC_SDHC1)
	tcc_sdhc1_device.dev.platform_data = &m801_88_mmc_platform_data[1];
	platform_device_register(&tcc_sdhc1_device);
#endif
#if defined(CONFIG_MMC_TCC_SDHC2)
	tcc_sdhc2_device.dev.platform_data = &m801_88_mmc_platform_data[2];
	platform_device_register(&tcc_sdhc2_device);
#endif
#if defined(CONFIG_MMC_TCC_SDHC3)
	tcc_sdhc3_device.dev.platform_data = &m801_88_mmc_platform_data[3];
	platform_device_register(&tcc_sdhc3_device);
#endif

	return 0;
}
device_initcall(m801_88_init_mmc);
#endif

