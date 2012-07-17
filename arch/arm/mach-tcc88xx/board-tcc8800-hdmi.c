#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mmc/host.h>
#include <linux/gpio.h>

#include <asm/mach-types.h>
#include <asm/io.h>

#include <mach/gpio.h>
#include <mach/bsp.h>
#include <mach/tcc_board_hdmi.h>

#include <mach/tcc_board_power.h>
#include <linux/platform_device.h>

#include <linux/err.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/consumer.h>


#if defined(CONFIG_ARCH_TCC88XX)
#include "board-tcc8800.h"
#endif//
tcc_hdmi_power_s hdmipwr;

#if defined(CONFIG_REGULATOR)
static struct regulator *vdd_hdmi_osc = NULL;
#endif

int tcc_hdmi_power(struct device *dev, tcc_hdmi_power_s pwr)
{
	hdmipwr = pwr;
	
 	if (machine_is_tcc8800()) {
		unsigned hdmi_5p, hdmi_en;
		printk("%s  tcc88xx  pwr:%d system_rev:0x%x \n", __func__, pwr, system_rev);
		if(system_rev ==0x0500)
		{
			hdmi_5p = TCC_GPEXT4(0);
			hdmi_en = TCC_GPEXT4(3);
		}
		else
		{
			hdmi_5p = GPIO_V_5P0_EN;
			hdmi_en = GPIO_HDMI_EN;
		}
			

		switch(pwr)
		{
			case TCC_HDMI_PWR_INIT:
				tcc_power_control(TCC_V5P_POWER, TCC_POWER_INIT);
//				gpio_request(hdmi_5p, "hdmi_lvds_on");
//				gpio_direction_output(hdmi_5p, 1);

				gpio_request(hdmi_en, "hdmi_on");
				gpio_direction_output(hdmi_en, 1);
				break;

			case TCC_HDMI_PWR_ON:		
				tcc_power_control(TCC_V5P_POWER, TCC_POWER_ON);
//				gpio_set_value(hdmi_5p, 1);	
				gpio_set_value(hdmi_en, 1);
				break;

			case TCC_HDMI_PWR_OFF:
				tcc_power_control(TCC_V5P_POWER, TCC_POWER_OFF);
//				gpio_set_value(hdmi_5p, 0);
				gpio_set_value(hdmi_en, 0);	
				break;
		}		
	}
	else if (machine_is_m801_88() || machine_is_m803())
	{
		printk("%s  tcc88xx  pwr:%d \n", __func__, pwr);
		switch(pwr)
		{
			case TCC_HDMI_PWR_INIT:
#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_osc == NULL)
				{
					vdd_hdmi_osc = regulator_get(NULL, "vdd_hdmi_osc");
					if (IS_ERR(vdd_hdmi_osc)) {
						printk("Failed to obtain vdd_hdmi_osc\n");
						vdd_hdmi_osc = NULL;
					}
				}
#endif
				gpio_request(TCC_GPG(3), "hdmi_on");						
				gpio_direction_output(TCC_GPG(3), 1);
				break;
				
			case TCC_HDMI_PWR_ON:	
#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_osc)
					regulator_enable(vdd_hdmi_osc);
#endif
				gpio_set_value(TCC_GPG(3), 1);
				break;
			case TCC_HDMI_PWR_OFF:
#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_osc)
					regulator_disable(vdd_hdmi_osc);
#endif
				gpio_set_value(TCC_GPG(3), 0);
				break;
		}		
	}
	else
	{
		printk("ERROR : can not find HDMI POWER SETTING ");
	}	
	return 0;

}


static struct tcc_hdmi_platform_data hdmi_pdata = {
	.set_power	= tcc_hdmi_power,
};


struct platform_device tcc_hdmi_device = {
	.name	= "tcc_hdmi",
	.dev	= {
		.platform_data	= &hdmi_pdata,
	},
};

struct platform_device tcc_audio_device = {
	.name	= "tcc_hdmi_audio",
};


struct platform_device tcc_cec_device = {
	.name	= "tcc_hdmi_cec",
};

static struct tcc_hpd_platform_data hpd_pdata = {
	.hpd_port = TCC_GPD(25),
};

struct platform_device tcc_hpd_device = {
	.name	= "tcc_hdmi_hpd",
	.dev = {
		.platform_data = &hpd_pdata,
	}, 
};

static int __init tcc8800_init_hdmi(void)
{

	volatile PGPION pGPIOD = (volatile PGPION)tcc_p2v(HwGPIOD_BASE);
	printk("%s (built %s %s)\n", __func__, __DATE__, __TIME__);

	platform_device_register(&tcc_hdmi_device);

	platform_device_register(&tcc_audio_device);

	platform_device_register(&tcc_cec_device);

	if (machine_is_tcc8800())
	{
		BITCSET(pGPIOD->GPPD1, Hw19|Hw18, 0);
	}
	else
	{
		BITCSET(pGPIOD->GPPD1, Hw19|Hw18, Hw19);
	}
	platform_device_register(&tcc_hpd_device);

	return 0;
}

#if defined(CONFIG_TCC_STB_MODE)
device_initcall(tcc8800_init_hdmi);
#else
late_initcall(tcc8800_init_hdmi);
#endif /* CONFIG_TCC_STB_MODE */
