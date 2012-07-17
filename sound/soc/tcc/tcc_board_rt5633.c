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
#include "../codecs/rt5633.h"
#include "tcc-pcm.h"
#include "tcc-i2s.h"

#define TCC_HP        0
#define TCC_SPK       1

#define TCC_SPK_ON    0
#define TCC_SPK_OFF   1

#undef alsa_dbg
#if 1
#define alsa_dbg(fmt,arg...)    printk("<=tcc_board_rt5633== alsa-debug == "fmt,##arg)
#else
#define alsa_dbg(fmt,arg...)
#endif

// /* audio clock in Hz - rounded from 12.235MHz */
//#define TCC83X_AUDIO_CLOCK 12288000

/* default: Speaker output */
static int tcc_jack_func = TCC_SPK;
static int tcc_spk_func;

static void spk_mute()
{
#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_m57te())
#ifdef CONFIG_COBY_MID7025
		gpio_set_value(TCC_GPD(5), 0);
#else	
		gpio_set_value(TCC_GPA(11), 0);
#endif
	else if(machine_is_m801())
		gpio_set_value(TCC_GPD(5), 0);
#elif defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88())
		gpio_set_value(TCC_GPG(6), 0);
#endif

}

static void spk_un_mute()
{
#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_m57te())
#ifdef CONFIG_COBY_MID7025
{
	 gpio_set_value(TCC_GPE(2), 1);
	 gpio_set_value(TCC_GPD(5), 1);
}
#else	
		gpio_set_value(TCC_GPA(11), 1);
#endif
	else if(machine_is_m801())
		gpio_set_value(TCC_GPD(5), 1);
#elif defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88())
		gpio_set_value(TCC_GPG(6), 1);
#endif
}

static void hp_mute()
{
#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_m57te())
#ifdef CONFIG_COBY_MID7025
		gpio_set_value(TCC_GPD(7), 0);
#else	
		gpio_set_value(TCC_GPD(7), 0);
#endif
	else if(machine_is_m801())
		gpio_set_value(TCC_GPD(7), 0);
#elif defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88())
		gpio_set_value(TCC_GPD(11), 0);
#endif
#if defined(CONFIG_MACH_M805_892X)
	if(machine_is_m805_892x())
	{
		if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			gpio_set_value(TCC_GPE(17), 0);
		else
			gpio_set_value(TCC_GPG(5), 0);
        printk("@@@@@@@@@@@@@@@@@ %s() \n", __func__);
	}
#endif
}

static void hp_un_mute()
{
#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_m57te())
#ifdef CONFIG_COBY_MID7025
		gpio_set_value(TCC_GPD(7), 1);
#else	
		gpio_set_value(TCC_GPD(7), 1);
#endif
	else if(machine_is_m801())
		gpio_set_value(TCC_GPD(7), 1);
#elif defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_m801_88())
		gpio_set_value(TCC_GPD(11), 1);
#endif
#if defined(CONFIG_MACH_M805_892X)
	if(machine_is_m805_892x())
	{
		if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			gpio_set_value(TCC_GPE(17), 1);
		else
			gpio_set_value(TCC_GPG(5), 1);
        printk("@@@@@@@@@@@@@@@@@ %s() \n", __func__);
	}
#endif

}

int tcc_hp_is_valid(void)
{

#if defined(CONFIG_ARCH_TCC92XX)
    if(machine_is_m57te() || machine_is_m801()) {
        // gpio_get_value is ==> 1: disconnect, 0: connect
        return gpio_get_value(TCC_GPA(12));
    }
#elif defined(CONFIG_ARCH_TCC88XX)
    if(machine_is_m801_88()) {
        // gpio_get_value is ==> 0: disconnect, 1: connect
        return gpio_get_value(TCC_GPD(10));
    }
#endif
#if defined(CONFIG_MACH_M805_892X)
	if(machine_is_m805_892x())
        // gpio_get_value is ==> 0: disconnect, 1: connect
        return gpio_get_value(TCC_GPE(5));
#endif

 alsa_dbg("%s() null\n", __func__);
    return 0;
}

int tcc_hp_hw_mute(int flag)
{
	 alsa_dbg("%s() flag = %d\n", __func__, flag);
    if(flag)
        hp_mute();
    else
        hp_un_mute();
    if(!flag)
	tcc_jack_func = TCC_HP;
}

int tcc_spk_hw_mute(int flag)
{
	 alsa_dbg("%s() flag = %d\n", __func__, flag);
    if(flag)
        spk_mute();
    else
        spk_un_mute();
    if(!flag)
	tcc_jack_func = TCC_SPK;
}


static void tcc_ext_control(struct snd_soc_codec *codec)
{
	int spk = 0;
    struct snd_soc_dapm_context *dapm = &codec->dapm;

    alsa_dbg("%s() tcc_jack_func=%d\n", __func__, tcc_jack_func);

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
	 alsa_dbg("%s() null\n", __func__);
//	 struct snd_soc_pcm_runtime *rtd = substream->private_data;
//	 struct snd_soc_codec *codec = rtd->codec;
//	 tcc_ext_control(codec);
//	 spk_mute();   //add by jf.s
//	 hp_mute();   //add by jf.s
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
    default:
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
	ret = snd_soc_dai_set_sysclk(codec_dai, 0, clk,
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
	 alsa_dbg("%s() tcc_jack_func = %d\n", __func__, tcc_jack_func);
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
	 alsa_dbg("%s() tcc_jack_func = %d\n", __func__, tcc_jack_func);
	ucontrol->value.integer.value[0] = tcc_spk_func;
	return 0;
}

static int tcc_set_spk(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec =  snd_kcontrol_chip(kcontrol);
	 alsa_dbg("%s()\n", __func__);
	if (tcc_spk_func == ucontrol->value.integer.value[0])
		return 0;

	tcc_spk_func = ucontrol->value.integer.value[0];
	tcc_ext_control(codec);
	return 1;
}

static int tcc_hp_event(struct snd_soc_dapm_widget *w,
    struct snd_kcontrol *k, int event)
{
    alsa_dbg("%s() in... event[%d] null\n", __func__, event);
    return 0;
}
static int tcc_amp_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int event)
{
	 alsa_dbg("%s() null\n", __func__);
	return 0;
}

static int tcc_mic_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int event)
{
	 alsa_dbg("%s() null\n", __func__);
	return 0;
}

/* tcc machine dapm widgets */
static const struct snd_soc_dapm_widget tcc_rt5633_dapm_widgets[] = {
    SND_SOC_DAPM_HP("Headphone Jack", tcc_hp_event),
    SND_SOC_DAPM_MIC("Mic Jack", tcc_mic_event),
    SND_SOC_DAPM_SPK("Ext Spk", tcc_amp_event),
    SND_SOC_DAPM_LINE("Line Jack", NULL),
    SND_SOC_DAPM_HP("Headset Jack", NULL),
};

/* tcc machine audio map (connections to the codec pins) */
static const struct snd_soc_dapm_route tcc_audio_map[] = {

	/* mic is connected to MICIN (via right channel of headphone jack) */
	{"MIC1", NULL, "Mic Jack"},

	/* Same as the above but no mic bias for line signals */
	{"MIC1", NULL, "Line Jack"},

	/* headset Jack  - in = micin, out = HPOR,HPOL*/
	{"Headset Jack", NULL, "HPOR"},
    {"Headset Jack", NULL, "HPOL"},

	/* headphone connected to HPOR, HPOL */
	{"Headphone Jack", NULL, "HPOL"},
	{"Headphone Jack", NULL, "HPOR"},

	/* speaker connected to SPOL, SPOR */
	{"Ext Spk", NULL, "SPOR"},
	{"Ext Spk", NULL, "SPOL"},

};

static const char *jack_function[] = {"HeadPhone", "Speaker"};
static const char *spk_function[]  = {"On", "Off"};

static const struct soc_enum tcc_enum[] = {
	SOC_ENUM_SINGLE_EXT(2, jack_function),
	SOC_ENUM_SINGLE_EXT(2, spk_function),
};

static const struct snd_kcontrol_new rt5633_tcc_controls[] = {
	SOC_ENUM_EXT("Jack Function"   , tcc_enum[0], tcc_get_jack, tcc_set_jack),
	SOC_ENUM_EXT("Speaker Function", tcc_enum[1], tcc_get_spk , tcc_set_spk),
};

/*void power_off_speaker(void)
{
	BITCSET(HwGPIOG->GPEN,Hw6,Hw6);
	BITCLR(HwGPIOG->GPDAT, Hw6);   //close the Double codecs power 
}
EXPORT_SYMBOL(power_off_speaker);

void power_on_speaker(void)
{
	BITCSET(HwGPIOG->GPEN,Hw6,Hw6);
	BITCSET(HwGPIOG->GPDAT,Hw6,Hw6);   //close the Double codecs power 
}
EXPORT_SYMBOL(power_on_speaker);
*/
/*
 * Logic for a rt5633 as connected on a Sharp SL-C7x0 Device
 */
static int tcc_rt5633_init(struct snd_soc_pcm_runtime *rtd)
{
	int i, ret;
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

    alsa_dbg("%s() in...\n", __func__);
//	gpio_set_value(TCC_GPE(2), 1);
	
#if 1
    snd_soc_dapm_enable_pin(dapm, "MICIN");

	/* Add tcc specific controls */
	ret = snd_soc_add_controls(codec, rt5633_tcc_controls,
				ARRAY_SIZE(rt5633_tcc_controls));
	if (ret)
		return ret;


	/* Add tcc specific widgets */
	snd_soc_dapm_new_controls(dapm, tcc_rt5633_dapm_widgets,
				  ARRAY_SIZE(tcc_rt5633_dapm_widgets));

	/* Set up Telechips specific audio path telechips audio_map */
	snd_soc_dapm_add_routes(dapm, tcc_audio_map, ARRAY_SIZE(tcc_audio_map));

	snd_soc_dapm_sync(dapm);
#endif
    alsa_dbg("%s() call snd_soc_jack_new()\n", __func__);
	return 0;
}

static int tcc_iec958_dummy_init(struct snd_soc_codec *codec)
{
	 alsa_dbg("%s() null\n", __func__);
    return 0;
}

/* tcc digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link tcc_dai[] = {
	{
		.name = "rt5633",
		.stream_name = "rt5633",
		.platform_name = "tcc-pcm-audio",
		.cpu_dai_name  = "tcc-dai-i2s",

        .codec_name = "rt5633 I2C Codec.0-001c",
        .codec_dai_name = "rt5633-hifi",
        .init = tcc_rt5633_init,
        .ops = &tcc_ops,
	},
    {
        .name = "TCC",
        .stream_name = "IEC958",
        .platform_name = "tcc-pcm-audio",
        .cpu_dai_name   = "tcc-dai-spdif",
        
        .codec_name = "tcc-iec958",
        .codec_dai_name = "IEC958",
        .init = tcc_iec958_dummy_init,
        .ops = &tcc_ops,
    },
};



/* tcc audio subsystem */
static struct snd_soc_card tcc_soc_card = {
	.driver_name = "TCC Audio",
    .long_name = "Telechips Board",
    .dai_link = tcc_dai,
    .num_links = ARRAY_SIZE(tcc_dai),
};


/* 
 * FIXME: This is a temporary bodge to avoid cross-tree merge issues. 
 * New drivers should register the rt5633 I2C device in the machine 
 * setup code (under arch/arm for ARM systems). 
 */
static int rt5633_i2c_register(void)
{
    struct i2c_board_info info;
    struct i2c_adapter *adapter;
    struct i2c_client *client;

    alsa_dbg("%s()\n", __func__);

    memset(&info, 0, sizeof(struct i2c_board_info));
    info.addr = 0x1c;
    strlcpy(info.type, "rt5633", I2C_NAME_SIZE);

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

static int __init tcc_init_rt5633(void)
{

	int ret;

    tcc_soc_card.name = "TCC892x EVM";
    alsa_dbg("TCC Board probe [%s]\n", __FUNCTION__);
#if defined(CONFIG_MACH_M805_892X)
    alsa_dbg("=======TCC Board probe [%s]\n", __FUNCTION__);

    /* h/w mute control */
    if(machine_is_m805_892x()) {
		if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			tcc_gpio_config(TCC_GPE(17), GPIO_FN(0));
			gpio_request(TCC_GPE(17), "HP_MUTE_CTL");
			gpio_direction_output(TCC_GPE(17), 0);   // HeadPhone mute
		}
		else {
			tcc_gpio_config(TCC_GPG(5), GPIO_FN(0));
			gpio_request(TCC_GPG(5), "HP_MUTE_CTL");
			gpio_direction_output(TCC_GPG(5), 0);   // HeadPhone mute
		}
        tcc_hp_hw_mute(false);
        tcc_spk_hw_mute(false);
    }

#else
    alsa_dbg("TCC Board probe [%s]\n", __FUNCTION__);
#endif

    tca_tcc_initport();

    ret = rt5633_i2c_register();

	tcc_snd_device = platform_device_alloc("soc-audio", -1);
	if (!tcc_snd_device)
		return -ENOMEM;

	platform_set_drvdata(tcc_snd_device, &tcc_soc_card);

	ret = platform_device_add(tcc_snd_device);
	if (ret)
		platform_device_put(tcc_snd_device);

    alsa_dbg("=======end func: tcc_init_rt5633\n");
	return ret;
}

static void __exit tcc_exit_rt5633(void)
{
	platform_device_unregister(tcc_snd_device);
}

module_init(tcc_init_rt5633);
module_exit(tcc_exit_rt5633);

/* Module information */
MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("ALSA SoC TCCxx Board (RT5633)");
MODULE_LICENSE("GPL");
