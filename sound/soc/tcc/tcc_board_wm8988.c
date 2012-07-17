/*
 * linux/sound/soc/tcc/tcc_board.c
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
#include <linux/clk.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <asm/mach-types.h>

#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include "tcc/tca_tcchwcontrol.h"
#include "../codecs/wm8988.h"
#include "tcc-pcm.h"
#include "tcc-i2s.h"

#define TCC_HP        0
#define TCC_SPK       1

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

/* default: Speaker output */
static int tcc_jack_func = TCC_SPK;
static int tcc_spk_func;



static void spk_mute(void)
{
#if defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88() || machine_is_m803())
		gpio_set_value(TCC_GPG(6), 0);
#elif defined(CONFIG_ARCH_TCC892X)
	if(machine_is_m805_892x())
	{
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			gpio_set_value(TCC_GPE(18), 0);
		} else {	
			#if defined(CONFIG_M805S_8923_0XA)
			gpio_set_value(TCC_GPG(11), 0);
			#else
			gpio_set_value(TCC_GPF(27), 0);
			#endif
		}
	}
#endif

}

static void spk_un_mute(void)
{
#if defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88() || machine_is_m803())
		gpio_set_value(TCC_GPG(6), 1);
#elif defined(CONFIG_ARCH_TCC892X)
	if(machine_is_m805_892x())
	{
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			gpio_set_value(TCC_GPE(18), 1);
		} else {
			#if defined(CONFIG_M805S_8923_0XA)
			gpio_set_value(TCC_GPG(11), 1);
			#else
			gpio_set_value(TCC_GPF(27), 1);
			#endif
		}
	}
#endif
}

static void hp_mute(void)
{
#if defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88() || machine_is_m803())
		gpio_set_value(TCC_GPD(11), 0);
#elif defined(CONFIG_ARCH_TCC892X)
	if(machine_is_m805_892x())
	{
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			gpio_set_value(TCC_GPE(17), 0);
		else
			gpio_set_value(TCC_GPG(5), 0);
	}
#endif
}

static void hp_un_mute(void)
{
#if defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88() || machine_is_m803())
		gpio_set_value(TCC_GPD(11), 1);
#elif defined(CONFIG_ARCH_TCC892X)
	if(machine_is_m805_892x())
	{
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			gpio_set_value(TCC_GPE(17), 1);
		else
			gpio_set_value(TCC_GPG(5), 1);
	}
#endif
}

int tcc_hp_is_valid(void)
{
#if defined(CONFIG_ARCH_TCC88XX)
    if(machine_is_m801_88() || machine_is_m803()) {
        // gpio_get_value is ==> 0: disconnect, 1: connect
        return gpio_get_value(TCC_GPD(10));
    }
#elif defined(CONFIG_ARCH_TCC892X)
	if(machine_is_m805_892x())
	{
		// gpio_get_value is ==> 0: disconnect, 1: connect
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			return gpio_get_value(TCC_GPE(16));
		else
			return gpio_get_value(TCC_GPE(5));
	}
#endif

    return 0;
}

int tcc_hp_hw_mute(int flag)
{
    if(flag)
        hp_mute();
    else
        hp_un_mute();
}

int tcc_spk_hw_mute(int flag)
{
    if(flag)
        spk_mute();
    else
        spk_un_mute();
}


static void tcc_ext_control(struct snd_soc_codec *codec)
{
	int spk = 0;
    struct snd_soc_dapm_context *dapm = &codec->dapm;

    alsa_dbg("%s() tcc_jack_func=%d, bias_level[%d]\n", __func__, tcc_jack_func, dapm->bias_level);

	/* set up jack connection */
    if(dapm->bias_level == SND_SOC_BIAS_ON) {
    	switch (tcc_jack_func) {
    	case TCC_HP:
            tcc_hp_hw_mute(false);
            tcc_spk_hw_mute(true);
    		break;
    	case TCC_SPK:
            tcc_hp_hw_mute(true);
            tcc_spk_hw_mute(false);
    		break;
    	}
    }
	if (tcc_spk_func == TCC_SPK_ON)
	{
		spk = 1;
		snd_soc_dapm_enable_pin(dapm, "Ext Spk");		
	}

	/* signal a DAPM event */
	snd_soc_dapm_sync(dapm);
}

static int tcc_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;

    alsa_dbg("%s()\n", __func__);

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

    alsa_dbg("%s()\n", __func__);

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
	ret = snd_soc_dai_set_sysclk(codec_dai, WM8988_SYSCLK, clk,
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

    alsa_dbg("%s() tcc_jack_func=%d, ucontrol->value.integer.value[0]=%d\n", __func__, tcc_jack_func, ucontrol->value.integer.value[0]);

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

static int tcc_hp_event(struct snd_soc_dapm_widget *w,
    struct snd_kcontrol *k, int event)
{
    alsa_dbg("%s() in... event[%d]\n", __func__, event);
    return 0;
}
static int tcc_amp_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int event)
{
	return 0;
}

static int tcc_mic_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int event)
{
	return 0;
}

/* tcc machine dapm widgets */
static const struct snd_soc_dapm_widget tcc_wm8988_dapm_widgets[] = {
    SND_SOC_DAPM_HP("Headphone Jack", tcc_hp_event),
    SND_SOC_DAPM_MIC("Mic Jack", tcc_mic_event),
    SND_SOC_DAPM_SPK("Ext Spk", tcc_amp_event),
    SND_SOC_DAPM_LINE("Line Jack", NULL),
    SND_SOC_DAPM_HP("Headset Jack", NULL),
};

/* tcc machine audio map (connections to the codec pins) */
static const struct snd_soc_dapm_route tcc_audio_map[] = {

	/* mic is connected to MICIN (via right channel of headphone jack) */
	{"MICIN", NULL, "Mic Jack"},

	/* Same as the above but no mic bias for line signals */
	{"MICIN", NULL, "Line Jack"},

	/* headset Jack  - in = micin, out = LHPOUT*/
	{"Headset Jack", NULL, "LOUT1"},
    {"Headset Jack", NULL, "ROUT1"},

	/* headphone connected to LHPOUT1, RHPOUT1 */
	{"Headphone Jack", NULL, "LOUT1"},
	{"Headphone Jack", NULL, "ROUT1"},

	/* speaker connected to LOUT, ROUT */
	{"Ext Spk", NULL, "ROUT1"},
	{"Ext Spk", NULL, "LOUT1"},

};

static const char *jack_function[] = {"HeadPhone", "Speaker"};
static const char *spk_function[]  = {"On", "Off"};

static const struct soc_enum tcc_enum[] = {
	SOC_ENUM_SINGLE_EXT(2, jack_function),
	SOC_ENUM_SINGLE_EXT(2, spk_function),
};

static const struct snd_kcontrol_new wm8988_tcc_controls[] = {
	SOC_ENUM_EXT("Jack Function"   , tcc_enum[0], tcc_get_jack, tcc_set_jack),
	SOC_ENUM_EXT("Speaker Function", tcc_enum[1], tcc_get_spk , tcc_set_spk),
};

/*
 * Logic for a wm8988 as connected on a Sharp SL-C7x0 Device
 */
static int tcc_wm8988_init(struct snd_soc_pcm_runtime *rtd)
{
	int ret;
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

    alsa_dbg("%s() in...\n", __func__);

    snd_soc_dapm_enable_pin(dapm, "MICIN");

	/* Add tcc specific controls */
	ret = snd_soc_add_controls(codec, wm8988_tcc_controls,
				ARRAY_SIZE(wm8988_tcc_controls));
	if (ret)
		return ret;

	/* Add tcc specific widgets */
	snd_soc_dapm_new_controls(dapm, tcc_wm8988_dapm_widgets,
				  ARRAY_SIZE(tcc_wm8988_dapm_widgets));

	/* Set up Telechips specific audio path telechips audio_map */
	snd_soc_dapm_add_routes(dapm, tcc_audio_map, ARRAY_SIZE(tcc_audio_map));

	snd_soc_dapm_sync(dapm);

    alsa_dbg("%s() call snd_soc_jack_new()\n", __func__);
	return 0;
}

extern struct snd_soc_platform tcc_soc_platform;

static int tcc_iec958_dummy_init(struct snd_soc_codec *codec)
{
    return 0;
}

/* tcc digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link tcc_dai[] = {
	{
		.name = "WM8988",
		.stream_name = "WM8988",
        .platform_name  = "tcc-pcm-audio",
        .cpu_dai_name   = "tcc-dai-i2s",
        .codec_name = "wm8988 I2C Codec.0-001a",
        .codec_dai_name = "WM8988",
		.init = tcc_wm8988_init,
		.ops = &tcc_ops,
	},
    {
        .name = "TCC",
        .stream_name = "IEC958",
        .platform_name  = "tcc-pcm-audio",
        .cpu_dai_name   = "tcc-dai-spdif",

        .codec_name     = "tcc-iec958",
        .codec_dai_name = "IEC958",
        .init = tcc_iec958_dummy_init,
        .ops = &tcc_ops,
    },
};


/* tcc audio machine driver */
static struct snd_soc_card tcc_soc_card = {
	.driver_name = "TCC Audio",
	.long_name   = "Telechips Board",
	.dai_link    = tcc_dai,
	.num_links   = ARRAY_SIZE(tcc_dai),
};


/* 
 * FIXME: This is a temporary bodge to avoid cross-tree merge issues. 
 * New drivers should register the wm8988 I2C device in the machine 
 * setup code (under arch/arm for ARM systems). 
 */
static int wm8988_i2c_register(void)
{
    struct i2c_board_info info;
    struct i2c_adapter *adapter;
    struct i2c_client *client;

    alsa_dbg("%s()\n", __func__);

    memset(&info, 0, sizeof(struct i2c_board_info));
    info.addr = 0x1a;
    strlcpy(info.type, "wm8988", I2C_NAME_SIZE);

    adapter = i2c_get_adapter(0);
    if (!adapter) 
    {
        printk(KERN_ERR "can't get i2c adapter 0\n");
        return -ENODEV;
    }

    client = i2c_new_device(adapter, &info);
    i2c_put_adapter(adapter);
    if (!client) 
    {
        printk(KERN_ERR "can't add i2c device at 0x%x\n", (unsigned int)info.addr);
        return -ENODEV;
    }
    return 0;
}



static struct platform_device *tcc_snd_device;

static int __init tcc_init_wm8988(void)
{

	int ret;

    if( !(machine_is_m57te() || machine_is_m801() || machine_is_m801_88() || machine_is_m803() || machine_is_m805_892x() ) ) {
        alsa_dbg("\n\n\n\n%s() do not execution....\n\n", __func__);
        return 0;
    }

#if defined(CONFIG_ARCH_TCC88XX)
    alsa_dbg("TCC Board probe [%s]\n", __FUNCTION__);

    /* h/w mute control */
    if(machine_is_m801_88() || machine_is_m803()) {
        tcc_gpio_config(TCC_GPG(6), GPIO_FN(0));
        tcc_gpio_config(TCC_GPD(11), GPIO_FN(0));
        gpio_request(TCC_GPG(6), "SPK_MUTE_CTL");
        gpio_request(TCC_GPD(11), "HP_MUTE_CTL");
        
        gpio_direction_output(TCC_GPG(6), 0);    // Speaker mute
        gpio_direction_output(TCC_GPD(11), 1);   // HeadPhone mute
        tcc_hp_hw_mute(false);
        tcc_spk_hw_mute(false);

        tcc_soc_card.name = "M801";
    }

#elif defined(CONFIG_ARCH_TCC892X)
	alsa_dbg("TCC Board probe [%s]\n", __FUNCTION__);

	/* h/w mute control */
	if(machine_is_m805_892x())
	{
		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			tcc_gpio_config(TCC_GPE(18), GPIO_FN(0));
			gpio_request(TCC_GPE(18), "SPK_MUTE_CTL");
			gpio_direction_output(TCC_GPE(18), 0);	 // Speaker mute
			
			tcc_gpio_config(TCC_GPE(17), GPIO_FN(0));
			gpio_request(TCC_GPE(17), "HP_MUTE_CTL");
			gpio_direction_output(TCC_GPE(17), 1);	 // HeadPhone mute
		}
		else {
			#if defined(CONFIG_M805S_8923_0XA)
			tcc_gpio_config(TCC_GPG(11), GPIO_FN(0));
			gpio_request(TCC_GPG(11), "SPK_MUTE_CTL");
			gpio_direction_output(TCC_GPG(11), 0);	 // Speaker mute
			#else
			tcc_gpio_config(TCC_GPF(27), GPIO_FN(0));
			gpio_request(TCC_GPF(27), "SPK_MUTE_CTL");
			gpio_direction_output(TCC_GPF(27), 0);	 // Speaker mute
			#endif

			tcc_gpio_config(TCC_GPG(5), GPIO_FN(0));
			gpio_request(TCC_GPG(5), "HP_MUTE_CTL");
			gpio_direction_output(TCC_GPG(5), 1);	 // HeadPhone mute
		}

		tcc_hp_hw_mute(false);
		tcc_spk_hw_mute(false);

        tcc_soc_card.name = "M805";
	}
#else
    alsa_dbg("TCC Board probe [%s]\n", __FUNCTION__);
	return 0;
#endif

    tca_tcc_initport();

    ret = wm8988_i2c_register();

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

static void __exit tcc_exit_wm8988(void)
{
    printk("%s() \n", __func__);

	platform_device_unregister(tcc_snd_device);
}

module_init(tcc_init_wm8988);
module_exit(tcc_exit_wm8988);

/* Module information */
MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("ALSA SoC TCCxx Board (WM8988)");
MODULE_LICENSE("GPL");
