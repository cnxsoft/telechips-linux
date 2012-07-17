/*
 * wm8524.c  --  WM8524 ALSA SoC Audio driver
 *
 * Copyright 2010 Telechips
 *
 * Author: 
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <asm/mach-types.h>

#include "wm8524.h"

#define WM8524_VERSION "0.01"

#if 0
#define alsa_dbg(f, a...)  printk("== alsa-debug == " f, ##a)
#else
#define alsa_dbg(f, a...)
#endif

/* codec private data */
struct wm8524_priv {
	unsigned int sysclk;
};

static int wm8524_hw_params(struct snd_pcm_substream *substream,
                            struct snd_pcm_hw_params *params, 
                            struct snd_soc_dai *dai)
{
    return 0;
}

static int wm8524_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;

    alsa_dbg("%s: mute[%d]\n", __func__, mute);
	if (mute)
	{
		if(machine_is_tcc9300st()) {
		    gpio_set_value(TCC_GPF(3), 1);      // MUTE
		}
		else if(machine_is_tcc8800st()) {
		    gpio_set_value(TCC_GPD(19), 1);     // MUTE
		}
		else if(machine_is_tcc8920st()) {
		    gpio_set_value(TCC_GPB(7), 1);     // MUTE
		}
	}
	else
	{
		if(machine_is_tcc9300st()) {
		    gpio_set_value(TCC_GPF(3), 0);      // Un-MUTE
		}
		else if(machine_is_tcc8800st()) {
		    gpio_set_value(TCC_GPD(19), 0);     // Un-MUTE
		}
		else if(machine_is_tcc8920st()) {
		    gpio_set_value(TCC_GPB(7), 0);     // Un-MUTE
		}
	}

	return 0;
}

static int wm8524_set_dai_sysclk(struct snd_soc_dai *codec_dai,
                int clk_id, unsigned int freq, int dir)
{
    return 0;
}

static int wm8524_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
    return 0;
}

static int wm8524_pcm_prepare(struct snd_pcm_substream *substream,
			      struct snd_soc_dai *dai)
{
	/* set active */
    alsa_dbg("%s: \n", __func__);

	if(machine_is_tcc9300st()) {
        gpio_set_value(TCC_GPF(2), 1);      // Standby --> Enable mode
	}
	else if(machine_is_tcc8800st()) {
        gpio_set_value(TCC_GPD(18), 1);      // Standby --> Enable mode
	}
	else if(machine_is_tcc8920st()) {
        gpio_set_value(TCC_GPB(6), 1);      // Standby --> Enable mode
	}
	
	return 0;
}

static void wm8524_shutdown(struct snd_pcm_substream *substream,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;

	/* deactivate */
	if (!codec->active) {
		udelay(50);
	}
}

static void wm8524_hw_free(struct snd_pcm_substream *substream)
{
    alsa_dbg("%s \n", __func__);
}


#if defined(CONFIG_SND_SOC_TCC_MULTICHANNEL)
#define WM8524_RATES (SNDRV_PCM_RATE_8000_192000)
#else
#define WM8524_RATES (SNDRV_PCM_RATE_8000_96000)
#endif
#define WM8524_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)

static struct snd_soc_dai_ops wm8524_dai_ops = {
	.prepare		= wm8524_pcm_prepare,
	.hw_params		= wm8524_hw_params,
	.shutdown		= wm8524_shutdown,
	.digital_mute	= wm8524_mute,
	.set_sysclk		= wm8524_set_dai_sysclk,
	.set_fmt		= wm8524_set_dai_fmt,
	.hw_free		= wm8524_hw_free,
};

static struct snd_soc_dai_driver wm8524_dai = {
	.name = "wm8524-hifi",
	.playback = {
	.stream_name = "Playback",
	.channels_min = 1,
#if defined(CONFIG_SND_SOC_TCC_MULTICHANNEL)
	.channels_max = 8,
#else
	.channels_max = 2,
#endif
	.rates = WM8524_RATES,
	.formats = WM8524_FORMATS,},
	.ops = &wm8524_dai_ops,
};

static int wm8524_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	#ifdef _WM8524_REG_DEBUG_	/* Telechips */
		printk("%s(): in...\n", __func__);
	#endif

	if(machine_is_tcc8920st())
	{
		gpio_set_value(TCC_GPF(24), 0);	//AIFMODE
		gpio_set_value(TCC_GPB( 6), 0);	//MUTE_CTL
		gpio_set_value(TCC_GPB( 7), 0);	//MUTE_ANG

		tcc_gpio_config(TCC_GPG( 6), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//MCLK
		tcc_gpio_config(TCC_GPG( 7), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//BCLK
		tcc_gpio_config(TCC_GPG( 8), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//LRCK
		tcc_gpio_config(TCC_GPG( 9), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//DAO
		tcc_gpio_config(TCC_GPG(10), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//DAI

		gpio_set_value(TCC_GPG( 6), 0);	//MCLK
		gpio_set_value(TCC_GPG( 7), 0);	//BCLK
		gpio_set_value(TCC_GPG( 8), 0);	//LRCK
		gpio_set_value(TCC_GPG( 9), 0);	//DAO
		gpio_set_value(TCC_GPG(10), 0);	//DAI
	}

	return 0;
}

static int wm8524_resume(struct snd_soc_codec *codec)
{
	#ifdef _WM8524_REG_DEBUG_	/* Telechips */
		printk("%s(): out..\n", __func__);
	#endif
	
	if(machine_is_tcc8920st())
	{
		tcc_gpio_config(TCC_GPG( 6), GPIO_FN(1));	//MCLK
		tcc_gpio_config(TCC_GPG( 7), GPIO_FN(1));	//BCLK
		tcc_gpio_config(TCC_GPG( 8), GPIO_FN(1));	//LRCK
		tcc_gpio_config(TCC_GPG( 9), GPIO_FN(1));	//DAO
		tcc_gpio_config(TCC_GPG(10), GPIO_FN(1));	//DAI

		gpio_set_value(TCC_GPF(24), 1);	//AIFMODE
		gpio_set_value(TCC_GPB( 6), 1);	//MUTE_CTL
		gpio_set_value(TCC_GPB( 7), 1);	//MUTE_ANG
	}
	
	return 0;
}

static int wm8524_probe(struct snd_soc_codec *codec)
{
	struct wm8524_priv *wm8524;
	int ret = 0;

	alsa_dbg("WM8524 Simple DAC %s", WM8524_VERSION);

	wm8524 = kzalloc(sizeof(struct wm8524_priv), GFP_KERNEL);
	if (wm8524 == NULL) {
		kfree(codec);
		return -ENOMEM;
	}

	snd_soc_codec_set_drvdata(codec, wm8524);

	return ret;
}

/* power down chip */
static int wm8524_remove(struct snd_soc_codec *codec)
{
	struct wm8524_priv *wm8524 = snd_soc_codec_get_drvdata(codec);

	kfree(wm8524);

	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_wm8524 = {
	.probe = 	wm8524_probe,
	.remove = 	wm8524_remove,
	.suspend = 	wm8524_suspend,
	.resume =	wm8524_resume,
};

static __devinit int wm8524_codec_probe(struct platform_device *pdev)
{
	int ret;

	ret =  snd_soc_register_codec(&pdev->dev, &soc_codec_dev_wm8524, &wm8524_dai, 1);

	if (ret) {
		alsa_dbg("Failed to register codec\n");
		return -1;
	}
	
	return ret;
}

static int __devexit wm8524_codec_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct platform_driver wm8524_codec_driver = {
	.driver = {
		.name = "tcc-wm8524",
		.owner = THIS_MODULE,
	},
	.probe = wm8524_codec_probe,
	.remove = __devexit_p(wm8524_codec_remove),
};

static int __init wm8524_modinit(void)
{
	return platform_driver_register(&wm8524_codec_driver);
}
module_init(wm8524_modinit);

static void __exit wm8524_exit(void)
{
	platform_driver_unregister(&wm8524_codec_driver);
}
module_exit(wm8524_exit);

MODULE_DESCRIPTION("ASoC WM8524 driver");
MODULE_AUTHOR("Richard Purdie");
MODULE_LICENSE("GPL");

