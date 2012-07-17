/*
 * linux/sound/soc/tcc/tcc_board_wm8750.c
 *
 * Author:  <linux@telechips.com>
 * Created: Nov 30, 2007
 * Description: SoC audio for TCCxx
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>

//#include <asm/plat-tcc/tcc_ckc.h>
#include "tcc/tca_tcchwcontrol.h"
#include "../codecs/wm8750.h"

#include "tcc-pcm.h"
#include "tcc-i2s.h"

#define TCC_HP        0
#define TCC_MIC       1
#define TCC_LINE      2
#define TCC_HEADSET   3
#define TCC_HP_OFF    4
#define TCC_SPK_ON    0
#define TCC_SPK_OFF   1

// /* audio clock in Hz - rounded from 12.235MHz */
//#define TCC83X_AUDIO_CLOCK 12288000

static int tcc_jack_func;
static int tcc_spk_func;

static void tcc_ext_control(struct snd_soc_codec *codec)
{
	int spk = 0, mic = 0, line = 0, hp = 0, hs = 0;

	/* set up jack connection */
	switch (tcc_jack_func) {
	case TCC_HP:
		hp = 1;
		/* set = unmute headphone */
//		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
//		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
		break;
	case TCC_MIC:
		mic = 1;
		/* reset = mute headphone */
//		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
//		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
		break;
	case TCC_LINE:
		line = 1;
//		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
//		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
		break;
	case TCC_HEADSET:
		hs = 1;
		mic = 1;
//		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
//		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
		break;
	}

	if (tcc_spk_func == TCC_SPK_ON)
		spk = 1;

	/* set the enpoints to their new connetion states */
	//snd_soc_dapm_set_endpoint(codec, "Ext Spk", spk);
    snd_soc_dapm_enable_pin(codec, "Ext Spk");
	//snd_soc_dapm_set_endpoint(codec, "Mic Jack", mic);
	snd_soc_dapm_enable_pin(codec, "Mic Jack");
	//snd_soc_dapm_set_endpoint(codec, "Line Jack", line);
	snd_soc_dapm_enable_pin(codec, "Line Jack");
	//snd_soc_dapm_set_endpoint(codec, "Headphone Jack", hp);
	snd_soc_dapm_enable_pin(codec, "Headphone Jack");
	//snd_soc_dapm_set_endpoint(codec, "Headset Jack", hs);
    snd_soc_dapm_enable_pin(codec, "Headphone Jack");

	/* signal a DAPM event */
	//snd_soc_dapm_sync_endpoints(codec);
	snd_soc_dapm_sync(codec);
}

static int tcc_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->socdev->codec;

	/* check the jack status at stream startup */
	tcc_ext_control(codec);
	return 0;
}

/* we need to unmute the HP at shutdown as the mute burns power on tcc83x */
static void tcc_shutdown(struct snd_pcm_substream *substream)
{
	//struct snd_soc_pcm_runtime *rtd = substream->private_data;
	//struct snd_soc_codec *codec = rtd->socdev->codec;

	/* set = unmute headphone */
//	set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
//	set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
	//return 0;
}

static int tcc_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	unsigned int clk = 0;
	int ret = 0;

	switch (params_rate(params)) {
	case 8000:
	case 16000:
	case 48000:
	case 96000:
		clk = 12288000;
		break;
	case 11025:
	case 22050:
    case 32000:
	case 44100:
		clk = 11289600;
		break;
	}
 
	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* set the codec system clock for DAC and ADC */
	ret = snd_soc_dai_set_sysclk(codec_dai, WM8750_SYSCLK, clk,
		SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	/* set the I2S system clock as input (unused) */
   	ret = snd_soc_dai_set_sysclk(cpu_dai, 0, 0, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;
 
	return 0;
}

static struct snd_soc_ops tcc_ops = {
	.startup = tcc_startup,
	.hw_params = tcc_hw_params,
	.shutdown = tcc_shutdown,
};

static int tcc_get_jack(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = tcc_jack_func;
	return 0;
}

static int tcc_set_jack(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);

	if (tcc_jack_func == ucontrol->value.integer.value[0])
		return 0;

	tcc_jack_func = ucontrol->value.integer.value[0];
	tcc_ext_control(codec);
	return 1;
}

static int tcc_get_spk(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = tcc_spk_func;
	return 0;
}

static int tcc_set_spk(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec =  snd_kcontrol_chip(kcontrol);

	if (tcc_spk_func == ucontrol->value.integer.value[0])
		return 0;

	tcc_spk_func = ucontrol->value.integer.value[0];
	tcc_ext_control(codec);
	return 1;
}

static int tcc_amp_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int event)
{
#if 0
	if (SND_SOC_DAPM_EVENT_ON(event))
		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_APM_ON);
	else
		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_APM_ON);
#endif

	return 0;
}

static int tcc_mic_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int event)
{
#if 0
	if (SND_SOC_DAPM_EVENT_ON(event))
//		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MIC_BIAS);
	else
//		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MIC_BIAS);
#endif

	return 0;
}

/* tcc machine dapm widgets */
static const struct snd_soc_dapm_widget wm8750_dapm_widgets[] = {
SND_SOC_DAPM_HP("Headphone Jack", NULL),
SND_SOC_DAPM_MIC("Mic Jack", tcc_mic_event),
SND_SOC_DAPM_SPK("Ext Spk", tcc_amp_event),
SND_SOC_DAPM_LINE("Line Jack", NULL),
SND_SOC_DAPM_HP("Headset Jack", NULL),
};

/* tcc machine audio map (connections to the codec pins) */
static const struct snd_soc_dapm_route audio_map[] = {

	/* headset Jack  - in = micin, out = LHPOUT*/
	{"Headset Jack", NULL, "LHPOUT"},

	/* headphone connected to LHPOUT1, RHPOUT1 */
	{"Headphone Jack", NULL, "LHPOUT"},
	{"Headphone Jack", NULL, "RHPOUT"},

	/* speaker connected to LOUT, ROUT */
	{"Ext Spk", NULL, "ROUT"},
	{"Ext Spk", NULL, "LOUT"},

	/* mic is connected to MICIN (via right channel of headphone jack) */
	{"MICIN", NULL, "Mic Jack"},

	/* Same as the above but no mic bias for line signals */
	{"MICIN", NULL, "Line Jack"},
};

static const char *jack_function[] = {"Headphone", "Mic", "Line", "Headset",
	"Off"};
static const char *spk_function[] = {"On", "Off"};
static const struct soc_enum tcc_enum[] = {
	SOC_ENUM_SINGLE_EXT(5, jack_function),
	SOC_ENUM_SINGLE_EXT(2, spk_function),
};

static const struct snd_kcontrol_new wm8750_tcc_controls[] = {
	SOC_ENUM_EXT("Jack Function", tcc_enum[0], tcc_get_jack,
		tcc_set_jack),
	SOC_ENUM_EXT("Speaker Function", tcc_enum[1], tcc_get_spk,
		tcc_set_spk),
};

/*
 * Logic for a wm8750 as connected on a Sharp SL-C7x0 Device
 */
static int tcc_wm8750_init(struct snd_soc_codec *codec)
{
	int i, err;

	//snd_soc_dapm_nc_pin(codec, "LLINEIN");
	//snd_soc_dapm_nc_pin(codec, "RLINEIN");
	snd_soc_dapm_enable_pin(codec, "MICIN");

	/* Add tcc specific controls */
	for (i = 0; i < ARRAY_SIZE(wm8750_tcc_controls); i++) {
		err = snd_ctl_add(codec->card,
			snd_soc_cnew(&wm8750_tcc_controls[i],codec, NULL));
		if (err < 0)
			return err;
	}

	/* Add tcc specific widgets */
	snd_soc_dapm_new_controls(codec, wm8750_dapm_widgets,
				  ARRAY_SIZE(wm8750_dapm_widgets));

	/* Set up Telechips specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	snd_soc_dapm_sync(codec);
	return 0;
}


extern struct snd_soc_platform tcc_soc_platform;

/* tcc digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link tcc_dai = {
	.name = "WM8750",
	.stream_name = "WM8750",
	.cpu_dai = &tcc_i2s_dai,
	.codec_dai = &wm8750_dai,
	.init = tcc_wm8750_init,
	.ops = &tcc_ops,
};

static int tcc_board_probe(struct platform_device *pdev)
{
    GPIO *pStrGPIOBaseReg = (GPIO *)tcc_p2v(HwGPIO_BASE);

	tca_ckc_pclk_enable(PERI_DAI, 1);
	tca_ckc_setiobus(RB_DAICDIFCONTROLLER, 1);
	tca_ckc_pclk_enable(PERI_AUD, 1);
	tca_ckc_setiobus(RB_ADMACONTROLLER, 1);

    printk("TCC Board probe [%s]\n", __FUNCTION__);
    tca_tcc_initport();
   // tca_ckc_com_setperibus(RB_DAI, 1);
//    tca_ckc_setiobus(RB_DAICDIFCONTROLLER, ENABLE);
    return 0;
}

/* tcc audio machine driver */
static struct snd_soc_card snd_soc_machine_tcc = {
	.platform = &tcc_soc_platform,
	.name = "tccx_board",
    .probe = tcc_board_probe,
	.dai_link = &tcc_dai,
	.num_links = 1,
};


/* tcc audio private data */
static struct wm8750_setup_data tcc_wm8750_setup = {
//    .i2c_bus = 0,
#if defined (CONFIG_I2C) || defined (CONFIG_I2C_MODULE)
	.i2c_address = 0x1a,  
#else
	.i2c_address = 0x34,		
#endif
};

/* tcc audio subsystem */
static struct snd_soc_device tcc_snd_devdata = {
	.card = &snd_soc_machine_tcc,
	.codec_dev = &soc_codec_dev_wm8750,
	.codec_data = &tcc_wm8750_setup,
};

static struct platform_device *tcc_snd_device;

static int __init tcc_init(void)
{

	int ret;

	tcc_snd_device = platform_device_alloc("soc-audio", -1);
	if (!tcc_snd_device)
		return -ENOMEM;

	platform_set_drvdata(tcc_snd_device, &tcc_snd_devdata);
	tcc_snd_devdata.dev = &tcc_snd_device->dev;
	ret = platform_device_add(tcc_snd_device);

	if (ret)
		platform_device_put(tcc_snd_device);

	return ret;
}

static void __exit tcc_exit(void)
{
	platform_device_unregister(tcc_snd_device);
}

module_init(tcc_init);
module_exit(tcc_exit);

/* Module information */
MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("ALSA SoC TCCxx Board");
MODULE_LICENSE("GPL");
