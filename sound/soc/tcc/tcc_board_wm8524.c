/*
 * linux/sound/soc/tcc/tcc_board_wm8524.c
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

#include <linux/gpio.h>
#include <linux/clk.h>
#include <asm/mach-types.h>

#include "tcc/tca_tcchwcontrol.h"
#include "../codecs/wm8524.h"

#include "tcc-pcm.h"
#include "tcc-i2s.h"

#define TCC_HP        0
#define TCC_HEADSET   1
#define TCC_HP_OFF    2
#define TCC_SPK_ON    0
#define TCC_SPK_OFF   1

#undef alsa_dbg
#if 0
#define alsa_dbg(fmt,arg...)    printk("== alsa-debug == "fmt,##arg)
#else
#define alsa_dbg(fmt,arg...)
#endif

// /* audio clock in Hz - rounded from 12.235MHz */
//#define TCC83X_AUDIO_CLOCK 12288000

static int tcc_jack_func;
static int tcc_spk_func;

static void tcc_ext_control(struct snd_soc_codec *codec)
{
    int spk = 0, mic = 0, line = 0, hp = 0, hs = 0;
    struct snd_soc_dapm_context *dapm = &codec->dapm;

	/* set up jack connection */
	switch (tcc_jack_func) {
	case TCC_HP:
		hp = 1;
		snd_soc_dapm_disable_pin(dapm,    "Ext Spk");
		snd_soc_dapm_enable_pin(dapm, "Headphone Jack");
		snd_soc_dapm_disable_pin(dapm, "Headset Jack");
		break;
	case TCC_HEADSET:
		hs = 1;
		mic = 1;
		snd_soc_dapm_disable_pin(dapm,    "Ext Spk");
		snd_soc_dapm_disable_pin(dapm, "Headphone Jack");
		snd_soc_dapm_enable_pin(dapm, "Headset Jack");		
		break;
	}

	if (tcc_spk_func == TCC_SPK_ON)
	{
		spk = 1;
		snd_soc_dapm_enable_pin(dapm, "Ext Spk");		
	}


	snd_soc_dapm_enable_pin(dapm, "Headphone Jack");
	snd_soc_dapm_enable_pin(dapm, "Headphone Jack");

	/* signal a DAPM event */
	snd_soc_dapm_sync(dapm);
}

static int tcc_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;

	/* check the jack status at stream startup */
	tcc_ext_control(codec);
	return 0;
}

/* we need to unmute the HP at shutdown as the mute burns power on tcc83x */
static void tcc_shutdown(struct snd_pcm_substream *substream)
{
}

static int tcc_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
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
    default:
		clk = 11289600;
		break;
	}
 
	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
        printk("tcc_hw_params()  codec_dai: set_fmt error[%d]\n", ret);
		return ret;
    }

	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
        printk("tcc_hw_params()  cpu_dai: set_fmt error[%d]\n", ret);
		return ret;
    }

	/* set the codec system clock for DAC and ADC */
	ret = snd_soc_dai_set_sysclk(codec_dai, WM8524_SYSCLK, clk,
		SND_SOC_CLOCK_IN);
	if (ret < 0) {
        printk("tcc_hw_params()  codec_dai: sysclk error[%d]\n", ret);
		return ret;
    }

	/* set the I2S system clock as input (unused) */
   	ret = snd_soc_dai_set_sysclk(cpu_dai, 0, 0, SND_SOC_CLOCK_IN);
	if (ret < 0) {
        printk("tcc_hw_params()  cpu_dai: sysclk error[%d]\n", ret);
		return ret;
    }
 
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

/* tcc machine dapm widgets */
static const struct snd_soc_dapm_widget wm8524_dapm_widgets[] = {
SND_SOC_DAPM_HP("Headphone Jack", NULL),
SND_SOC_DAPM_SPK("Ext Spk", tcc_amp_event),
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
};

static const char *jack_function[] = {"Headphone", "Headset", "Off"};
static const char *spk_function[] = {"On", "Off"};
static const struct soc_enum tcc_enum[] = {
	SOC_ENUM_SINGLE_EXT(3, jack_function),
	SOC_ENUM_SINGLE_EXT(2, spk_function),
};

static const struct snd_kcontrol_new wm8524_tcc_controls[] = {
	SOC_ENUM_EXT("Jack Function", tcc_enum[0], tcc_get_jack,
		tcc_set_jack),
	SOC_ENUM_EXT("Speaker Function", tcc_enum[1], tcc_get_spk,
		tcc_set_spk),
};

/*
 * Logic for a wm8524 as connected on a Sharp SL-C7x0 Device
 */
static int tcc_wm8524_init(struct snd_soc_pcm_runtime *rtd)
{
    int ret;
    struct snd_soc_codec *codec = rtd->codec;
    struct snd_soc_dapm_context *dapm = &codec->dapm;

    /* Add tcc specific controls */
    ret = snd_soc_add_controls(codec, wm8524_tcc_controls,
                            ARRAY_SIZE(wm8524_tcc_controls));
    if (ret)
        return ret;

    /* Add tcc specific widgets */
    snd_soc_dapm_new_controls(dapm, wm8524_dapm_widgets,
                ARRAY_SIZE(wm8524_dapm_widgets));

    /* Set up Telechips specific audio path audio_map */
    snd_soc_dapm_add_routes(dapm, audio_map, ARRAY_SIZE(audio_map));

    snd_soc_dapm_sync(dapm);
    return 0;
}

#if !defined(CONFIG_SND_TCC_DAI2SPDIF)
static int tcc_iec958_dummy_init(struct snd_soc_pcm_runtime *rtd)
{
    return 0;
}
#endif

/* tcc digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link tcc_dai[] = {
    {
        .name = "WM8524",
        .stream_name = "WM8524",
        .platform_name  = "tcc-pcm-audio",
        .cpu_dai_name   = "tcc-dai-i2s",

        .codec_name = "tcc-wm8524",
        .codec_dai_name = "wm8524-hifi",
        .init = &tcc_wm8524_init,
        .ops = &tcc_ops,
    },
#if !defined(CONFIG_SND_TCC_DAI2SPDIF)
    {
        .name = "TCC",
        .stream_name = "IEC958",
        .platform_name  = "tcc-pcm-audio",
        .cpu_dai_name   = "tcc-dai-spdif",

        .codec_name     = "tcc-iec958",
        .codec_dai_name = "IEC958",
        .init = &tcc_iec958_dummy_init,
        .ops = &tcc_ops,
    },
#endif
};

/* tcc audio machine driver */
static struct snd_soc_card tcc_soc_card = {
    .driver_name 	= "TCC Audio",
    .long_name		= "Telechips Board",
    .dai_link		= tcc_dai,
    .num_links		= ARRAY_SIZE(tcc_dai),
};

static struct platform_device *tcc_snd_device;

static int __init tcc_init_wm8524(void)
{
	volatile PGPIO pGpio = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	int ret;

    if( !(machine_is_tcc8800st() || machine_is_tcc9300st() || machine_is_tcc8920st()) ) {
        alsa_dbg("\n\n\n\n%s() do not execution....\n\n", __func__);
        return 0;
    }

    alsa_dbg("TCC Board probe [%s]\n", __FUNCTION__);

	#if defined(CONFIG_ARCH_TCC93XX)
		if(machine_is_tcc9300st()) {
	        gpio_request(TCC_GPF(4), "AIF_MODE");
	        gpio_request(TCC_GPF(3), "MUTE_ANG");
	        gpio_request(TCC_GPF(2), "MUTE_CTL");
	 
	        BITCLR(pGpio->GPFFN0, Hw20-Hw8);	    /* GPIOF[4:2]: AIFMode, MUTE_ANG, MUTE_CTL */
	        gpio_direction_output(TCC_GPF(4), 1);   /* AIFMode: 0(24-bit LJ), 1(24bit I2S), Z(24bit RJ) */
	        gpio_direction_output(TCC_GPF(3), 0);   /* MUT_ANG: h/w mute */
	        gpio_direction_output(TCC_GPF(2), 0);   /* MUTE_CTL of WM8524 */

	        tcc_soc_card.name = "tcc9300st";
	    }
	#elif defined(CONFIG_ARCH_TCC88XX)
	    if(machine_is_tcc8800st()) {
	        gpio_request(TCC_GPD(20), "AIF_MODE");
	        gpio_request(TCC_GPD(19), "MUTE_ANG");
	        gpio_request(TCC_GPD(18), "MUTE_CTL");

	        BITCLR(pGpio->GPDFN2, Hw20-Hw8);	    /* GPIOD[20:18]: AIFMode, MUTE_ANG, MUTE_CTL */
	        gpio_direction_output(TCC_GPD(20), 1);   /* AIFMode: 0(24-bit LJ), 1(24bit I2S), Z(24bit RJ) */
	        gpio_direction_output(TCC_GPD(19), 0);   /* MUT_ANG: h/w mute */
	        gpio_direction_output(TCC_GPD(18), 0);   /* MUTE_CTL of WM8524 */

	        tcc_soc_card.name = "tcc8800st";
	    }
	#elif defined(CONFIG_ARCH_TCC892X)
		if(machine_is_tcc8920st()) {
	        gpio_request(TCC_GPF(24), "AIF_MODE");
	        gpio_request(TCC_GPB(7), "MUTE_ANG");
	        gpio_request(TCC_GPB(6), "MUTE_CTL");
	 
	        //BITCLR(pGpio->GPFFN0, Hw20-Hw8);	    /* GPIOF[4:2]: AIFMode, MUTE_ANG, MUTE_CTL */
	        gpio_direction_output(TCC_GPF(24), 1);  /* AIFMode: 0(24-bit LJ), 1(24bit I2S), Z(24bit RJ) */
	        gpio_direction_output(TCC_GPB(7), 0);   /* MUT_ANG: h/w mute */
	        gpio_direction_output(TCC_GPB(6), 0);   /* MUTE_CTL of WM8524 */

	        tcc_soc_card.name = "tcc8920st";
		}
    #endif

    tca_tcc_initport();

    tcc_snd_device = platform_device_alloc("soc-audio", -1);
    if (!tcc_snd_device)
        return -ENOMEM;

	platform_set_drvdata(tcc_snd_device, &tcc_soc_card);

	ret = platform_device_add(tcc_snd_device);
	if (ret) {
        printk(KERN_ERR "Unable to add platform device\n");\
		platform_device_put(tcc_snd_device);
	}

	return ret;
}

static void __exit tcc_exit_wm8524(void)
{
	platform_device_unregister(tcc_snd_device);
}

module_init(tcc_init_wm8524);
module_exit(tcc_exit_wm8524);

/* Module information */
MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("ALSA SoC TCCxx Board(WM8524)");
MODULE_LICENSE("GPL");
