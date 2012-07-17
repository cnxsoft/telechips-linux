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

tcc_hdmi_power_s hdmipwr;

int tcc_hdmi_power(struct device *dev, tcc_hdmi_power_s pwr)
{
	hdmipwr = pwr;
	
	if (machine_is_tcc8800st())
	{
		printk("%s  tcc8800st  pwr:%d system_rev:0x%x \n", __func__, pwr, system_rev);

		switch(pwr)
		{
			case TCC_HDMI_PWR_INIT:
				gpio_request(TCC_GPE(23), "hdmi_on");
				gpio_direction_output(TCC_GPE(23), 1);
				break;

			case TCC_HDMI_PWR_ON:
				gpio_set_value(TCC_GPE(23), 1);
				break;

			case TCC_HDMI_PWR_OFF:
				gpio_set_value(TCC_GPE(23), 0);	
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

	printk("%s (built %s %s)\n", __func__, __DATE__, __TIME__);

	platform_device_register(&tcc_hdmi_device);

	platform_device_register(&tcc_audio_device);

	platform_device_register(&tcc_cec_device);

	platform_device_register(&tcc_hpd_device);

	return 0;
}

device_initcall(tcc8800_init_hdmi);


