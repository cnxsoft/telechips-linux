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
#if defined(CONFIG_REGULATOR)
#include <linux/regulator/machine.h>
#include <linux/regulator/consumer.h>
#endif 
#include "board-tcc8920.h"

tcc_hdmi_power_s hdmipwr;

#if defined(CONFIG_REGULATOR)
static struct regulator *vdd_hdmi_osc = NULL;
static struct regulator *vdd_hdmi_pll = NULL;
#endif

int tcc_hdmi_power(struct device *dev, tcc_hdmi_power_s pwr)
{
	unsigned hdmi_5p, hdmi_en;

	hdmipwr = pwr;
	
 	if (machine_is_tcc8920()) 
	{
		printk("%s  tcc88xx  pwr:%d system_rev:0x%x \n", __func__, pwr, system_rev);

		hdmi_5p = GPIO_V_5P0_EN;
		hdmi_en = GPIO_HDMI_EN;
		switch(pwr)
		{

			case TCC_HDMI_PWR_INIT:
				tcc_power_control(TCC_V5P_POWER, TCC_POWER_INIT);
//				gpio_request(hdmi_5p, "hdmi_lvds_on");
//				gpio_direction_output(hdmi_5p, 1);

				gpio_request(hdmi_en, "hdmi_on");
				gpio_direction_output(hdmi_en, 1);

#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_osc == NULL) {
					vdd_hdmi_osc = regulator_get(NULL, "vdd_hdmi_osc");
					if (IS_ERR(vdd_hdmi_osc)) {
						printk("Failed to obtain vdd_hdmi_osc\n");
						vdd_hdmi_osc = NULL;
					}
				}
				if (vdd_hdmi_pll == NULL) {
					vdd_hdmi_pll = regulator_get(NULL, "vdd_hdmi_pll");
					if (IS_ERR(vdd_hdmi_pll)) {
						printk("Failed to obtain vdd_hdmi_osc\n");
						vdd_hdmi_pll = NULL;
					}
				}
#endif
				break;

			case TCC_HDMI_PWR_ON:
				tcc_power_control(TCC_V5P_POWER, TCC_POWER_ON);
//				gpio_set_value(hdmi_5p, 1);
				gpio_set_value(hdmi_en, 1);

#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_pll)
 					regulator_enable(vdd_hdmi_pll);
				if (vdd_hdmi_osc)
 					regulator_enable(vdd_hdmi_osc);
#endif
				break;

			case TCC_HDMI_PWR_OFF:
#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_osc)
					regulator_disable(vdd_hdmi_osc);
				if (vdd_hdmi_pll)
					regulator_disable(vdd_hdmi_pll);
#endif
				tcc_power_control(TCC_V5P_POWER, TCC_POWER_OFF);
//				gpio_set_value(hdmi_5p, 0);
				gpio_set_value(hdmi_en, 0);
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
	.hpd_port = TCC_GPHDMI(1),
};

struct platform_device tcc_hpd_device = {
	.name	= "tcc_hdmi_hpd",
	.dev = {
		.platform_data = &hpd_pdata,
	}, 
};

static int __init tcc8920_init_hdmi(void)
{

	printk("%s (built %s %s)\n", __func__, __DATE__, __TIME__);

	platform_device_register(&tcc_hdmi_device);

	platform_device_register(&tcc_audio_device);

	platform_device_register(&tcc_cec_device);

	platform_device_register(&tcc_hpd_device);

	return 0;
}

#if defined(CONFIG_TCC_STB_MODE)
device_initcall(tcc8920_init_hdmi);
#else
late_initcall(tcc8920_init_hdmi);
#endif /* CONFIG_TCC_STB_MODE */
