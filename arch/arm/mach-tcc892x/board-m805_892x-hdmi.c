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

#include "board-m805_892x.h"

tcc_hdmi_power_s hdmipwr;

#if defined(CONFIG_REGULATOR)
static struct regulator *vdd_hdmi_osc = NULL;
static struct regulator *vdd_hdmi_pll = NULL;
#endif

int m805_892x_hdmi_power(struct device *dev, tcc_hdmi_power_s pwr)
{
	unsigned hdmi_5p, hdmi_en;

	hdmipwr = pwr;
	
 	if (machine_is_m805_892x()) 
	{
		printk("%s  m805s_892x pwr:%d system_rev:0x%x \n", __func__, pwr, system_rev);

		switch(pwr)
		{
			case TCC_HDMI_PWR_INIT:
				
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
				
				if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
					gpio_request(TCC_GPE(26), "hdmi_on");					
					gpio_direction_output(TCC_GPE(26), 1);
				} else {
					gpio_request(TCC_GPF(15), "hdmi_on");
					gpio_direction_output(TCC_GPF(15), 1);
				}
				break;
				
			case TCC_HDMI_PWR_ON:	
#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_pll)
 					regulator_enable(vdd_hdmi_pll);
				if (vdd_hdmi_osc)
 					regulator_enable(vdd_hdmi_osc);
#endif
				if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
					gpio_set_value(TCC_GPE(26), 1);
				else
					gpio_set_value(TCC_GPF(15), 1);
				break;

			case TCC_HDMI_PWR_OFF:
#if defined(CONFIG_REGULATOR)
				if (vdd_hdmi_osc)
					regulator_disable(vdd_hdmi_osc);
				if (vdd_hdmi_pll)
					regulator_disable(vdd_hdmi_pll);
#endif
				if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
					gpio_set_value(TCC_GPE(26), 0);
				else
					gpio_set_value(TCC_GPF(15), 0);
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
	.set_power	= m805_892x_hdmi_power,
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

static int __init m805_892x_init_hdmi(void)
{

	printk("%s (built %s %s)\n", __func__, __DATE__, __TIME__);

	platform_device_register(&tcc_hdmi_device);

	platform_device_register(&tcc_audio_device);

	platform_device_register(&tcc_cec_device);

	platform_device_register(&tcc_hpd_device);

	return 0;
}

#if defined(CONFIG_TCC_STB_MODE)
device_initcall(m805_892x_init_hdmi);
#else
late_initcall(m805_892x_init_hdmi);
#endif /* CONFIG_TCC_STB_MODE */
