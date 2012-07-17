/*
 * linux/drivers/sound/soc/tcc/tcc_board_tcc250.c
 *
 * Author:  <linux@telechips.com>
 * Created:     Nov 30, 2007
 * Description: SoC audio for TCC250
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
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/driver.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <asm/mach-types.h>
#include <asm/hardware/scoop.h>
#include <asm/arch/hardware.h>
#ifdef CONFIG_ARCH_TCC83X
#include <asm/arch/tcc83x_virt.h>
#elif CONFIG_ARCH_TCC79X
#include <asm/arch/tcc79x.h>
#endif

#include <asm/plat-tcc/tcc_ckc.h>
#include "../codecs/tcc250.h"
#include "tcc-pcm.h"
#include "tcc-i2s.h"


static int tcc_startup(struct snd_pcm_substream *substream)
{
	return 0;
}

/* we need to unmute the HP at shutdown as the mute burns power on tcc */
static void tcc_shutdown(struct snd_pcm_substream *substream)
{
}

static int tcc_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_cpu_dai *cpu_dai = rtd->dai->cpu_dai;
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
	ret = codec_dai->dai_ops.set_fmt(codec_dai, SND_SOC_DAIFMT_I2S
                                     | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* set cpu DAI configuration */
	ret = cpu_dai->dai_ops.set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S
                                   | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* set the codec system clock for DAC and ADC */
	ret = codec_dai->dai_ops.set_sysclk(codec_dai, 0, clk, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	/* set the I2S system clock as input (unused) */
    ret = cpu_dai->dai_ops.set_sysclk(cpu_dai, 0, 0, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;
 
	return 0;
}

static struct snd_soc_ops tcc_ops = {
	.startup = tcc_startup,
	.hw_params = tcc_hw_params,
	.shutdown = tcc_shutdown,
};


/*
 * Logic for a tcc250 as connected on a Sharp SL-C7x0 Device
 */
static int tcc_tcc250_init(struct snd_soc_codec *codec)
{
	snd_soc_dapm_sync_endpoints(codec);
	return 0;
}


extern struct snd_soc_platform tcc_soc_platform;

/* tcc digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link tcc_dai = {
	.name = "TCC250",
	.stream_name = "TCC250",
	.cpu_dai = &tcc_i2s_dai,
	.codec_dai = &tcc250_dai,
	.init = tcc_tcc250_init,
	.ops = &tcc_ops,
};

static int tcc250_board_probe(struct platform_device *pdev)
{
    printk("TCC250 Board probe [%s]\n", __FUNCTION__);
    tca_tcc_initport();
    tca_ckc_com_setperibus(RB_DAI, 1);
    return 0;
}

/* tcc audio machine driver */
static struct snd_soc_card snd_soc_machine_tcc = {
	.platform = &tcc_soc_platform,
	.name = "tcc_board_tcc250",
    .probe = tcc250_board_probe,
	.dai_link = &tcc_dai,
	.num_links = 1,
};


/* tcc audio private data */
static struct tcc250_setup_data tcc_tcc250_setup = {
#if defined (CONFIG_I2C) || defined (CONFIG_I2C_MODULE)
	.i2c_address = 0x49,
#else
	.i2c_address = 0x92,		
#endif
};

/* tcc audio subsystem */
static struct snd_soc_device tcc_snd_devdata = {
	.card = &snd_soc_machine_tcc,
	.codec_dev = &soc_codec_dev_tcc250,
	.codec_data = &tcc_tcc250_setup,
};

static struct platform_device *tcc_snd_device;

static int __init tcc250_init(void)
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

static void __exit tcc250_exit(void)
{
	platform_device_unregister(tcc_snd_device);
}

module_init(tcc250_init);
module_exit(tcc250_exit);

/* Module information */
MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("ALSA SoC TCC250 Board");
MODULE_LICENSE("GPL");

