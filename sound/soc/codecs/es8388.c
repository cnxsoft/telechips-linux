/*
 * es8388.c -- es8388 ALSA SoC audio driver
 *
 * Copyright 2005 Openedhand Ltd.
 *
 * Author: Richard Purdie <richard@openedhand.com>
 *
 * Based on es8388.c
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
#include <linux/slab.h>
#include <linux/gpio.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#ifdef CONFIG_HHBF_FAST_REBOOT
#include <asm/reboot.h>
#endif

#include <mach/bsp.h>
#include <asm/io.h>
#include <asm/mach-types.h>


#include "es8388.h"
#include "../tcc/tcc-pcm.h"

#define AUDIO_NAME "ES8388"
#define ES8388_VERSION "v1.0"


extern int tcc_hp_hw_mute(int flag);
extern int tcc_spk_hw_mute(int flag);
extern int tcc_hp_is_valid(void);


unsigned int system_mute;
unsigned int system_mute_state;


/* codec private data */
struct es8388_priv {
	enum snd_soc_control_type control_type;
	u16 reg_cache[ES8388_CACHEREGNUM];
	unsigned int sysclk;
	int sysclk_type;
	int playback_fs;
	bool deemph;
};



/*
 * Debug
 */

#define ES8388_DEBUG 0

#if ES8388_DEBUG
#define alsa_dbg(format, arg...) \
	printk("== ES8388 == " format, ## arg)
#else
#define alsa_dbg(format, arg...) do {} while (0)
#endif


#define err(format, arg...) \
	printk(KERN_ERR AUDIO_NAME ": " format "\n" , ## arg)
#define info(format, arg...) \
	printk(KERN_INFO AUDIO_NAME ": " format "\n" , ## arg)
#define warn(format, arg...) \
	printk(KERN_WARNING AUDIO_NAME ": " format "\n" , ## arg)


/*
 * es8388 register cache
 * We can't read the es8388 register space when we
 * are using 2 wire for device control, so we cache them instead.
 */
static const u16 es8388_reg[] = {
	0x06, 0x1C, 0xC3, 0xFC,  /*  0 *////0x0100 0x0180
	0xC0, 0x00, 0x00, 0x7C,  /*  4 */
	0x80, 0x00, 0x00, 0x06,  /*  8 */
	0x00, 0x06, 0x30, 0x30,  /* 12 */
	0xC0, 0xC0, 0x38, 0xB0,  /* 16 */
	0x32, 0x06, 0x00, 0x00,  /* 20 */
	0x06, 0x32, 0xC0, 0xC0,  /* 24 */
	0x08, 0x06, 0x1F, 0xF7,  /* 28 */
	0xFD, 0xFF, 0x1F, 0xF7,  /* 32 */
	0xFD, 0xFF, 0x00, 0x38,  /* 36 */
	0x38, 0x38, 0x38, 0x38,  /* 40 */
	0x38, 0x00, 0x00, 0x00,  /* 44 */
	0x00, 0x00, 0x00, 0x00,  /* 48 */
	0x00, 0x00, 0x00, 0x00,  /* 52 */
};


static void dac_start_event(struct snd_soc_dapm_widget *w,
    struct snd_kcontrol *kcontrol, int event)
{
    struct snd_soc_codec *codec = w->codec;

    alsa_dbg("%s()  event[%x]\n\n", __func__, event);

    switch(event) {
        case SND_SOC_DAPM_POST_PMU:
            snd_soc_update_bits(codec, ES8388_CHIPPOWER, 0x51, 0x00);
            break;
        case SND_SOC_DAPM_PRE_PMD:
            snd_soc_update_bits(codec, ES8388_CHIPPOWER, 0x51, 0x51);
#ifdef CONFIG_COBY_MID7025
             tcc_hp_hw_mute(true);
             tcc_spk_hw_mute(true);
#endif
            break;
    }
}

static void adc_start_event(struct snd_soc_dapm_widget *w,
    struct snd_kcontrol *kcontrol, int event)
{
    struct snd_soc_codec *codec = w->codec;

    alsa_dbg("%s()  event[%x]\n", __func__, event);

    switch(event) {
        case SND_SOC_DAPM_POST_PMU:
            snd_soc_update_bits(codec, ES8388_CHIPPOWER, 0xA2, 0x00);
            break;
        case SND_SOC_DAPM_PRE_PMD:
            snd_soc_update_bits(codec, ES8388_CHIPPOWER, 0xA2, 0xA2);
            break;
    }
}

/* DAC output mono/stereo */
static const char *es8388_dac_mono[] = {"Stereo", "Mono"};

static const struct snd_kcontrol_new es8388_snd_controls[] = {
    SOC_DOUBLE_R("Capture Volume", ES8388_LADC_VOL, ES8388_RADC_VOL, 0, 63, 0),
    //SOC_DOUBLE_R("Capture ZC Switch", ES8388_LINVOL, ES8388_RINVOL, 6, 1, 0),

    //SOC_DOUBLE_R("Capture Switch", ES8388_LINVOL, ES8388_RINVOL, 7, 1, 1),
    //SOC_SINGLE("Capture Mic Switch", ES8388_ADCIN, 7, 1, 1),

    SOC_SINGLE("OUTPUT Mono Switch", ES8388_DACCONTROL7, 5, 1, 0),
    SOC_SINGLE("LOUT2 Switch", ES8388_DACPOWER, 3, 1, 0),
    SOC_SINGLE("ROUT2 Switch", ES8388_DACPOWER, 2, 1, 0),

    //SOC_DOUBLE_R("Playback Switch", ES8388_LDAC, ES8388_RDAC,6, 1, 1),
    SOC_DOUBLE_R("Playback Volume", ES8388_LDAC_VOL, ES8388_RDAC_VOL, 0, 192, 0),
};


/*
 * DAPM Controls
 */

/* Channel Input Mixer */
static const char *es8388_line_texts[] = {
	"Line 1", "Line 2", "Differential"};

static const unsigned int es8388_line_values[] = {
	0, 1, 3};

static const struct soc_enum es8388_lline_enum =
    SOC_VALUE_ENUM_SINGLE(ES8388_ADCCONTROL3, 6, 0xC0,
                ARRAY_SIZE(es8388_line_texts),
                es8388_line_texts,
                es8388_line_values);
static const struct snd_kcontrol_new es8388_left_line_controls =
    SOC_DAPM_VALUE_ENUM("Route", es8388_lline_enum);

static const struct soc_enum es8388_rline_enum =
    SOC_VALUE_ENUM_SINGLE(ES8388_ADCCONTROL3, 4, 0x30,
                ARRAY_SIZE(es8388_line_texts),
                es8388_line_texts,
                es8388_line_values);
static const struct snd_kcontrol_new es8388_right_line_controls =
    SOC_DAPM_VALUE_ENUM("Route", es8388_lline_enum);


/* Left Mixer */
static const struct snd_kcontrol_new es8388_left_mixer_controls[] = {
    SOC_DAPM_SINGLE("Left Playback Switch", ES8388_DACCONTROL17, 7, 1, 0),      // 39 
    SOC_DAPM_SINGLE("Left Bypass Switch"  , ES8388_DACCONTROL17, 7, 1, 0),      // 39
};

/* Right Mixer */
static const struct snd_kcontrol_new es8388_right_mixer_controls[] = {
    SOC_DAPM_SINGLE("Right Playback Switch", ES8388_DACCONTROL20, 7, 1, 0),     // 42
    SOC_DAPM_SINGLE("Right Bypass Switch"  , ES8388_DACCONTROL20, 7, 1, 0),     // 42
};


/* Differential Mux */
static const char *es8388_diff_sel[] = {"Line 1", "Line 2"};
static const struct soc_enum left_diffmux =
	SOC_ENUM_SINGLE(ES8388_ADCCONTROL2, 2, 2, es8388_diff_sel);
static const struct snd_kcontrol_new es8388_left_diffmux_controls =
	SOC_DAPM_ENUM("Route", left_diffmux);

static const struct soc_enum right_diffmux =
	SOC_ENUM_SINGLE(ES8388_ADCCONTROL3, 7, 2, es8388_diff_sel);
static const struct snd_kcontrol_new es8388_right_diffmux_controls =
	SOC_DAPM_ENUM("Route", right_diffmux);


/* Mono ADC Mux */
static const char *es8388_mono_mux[] = {"Stereo", "Mono (Left)",
	"Mono (Right)", "NONE"};
static const struct soc_enum monomux =
	SOC_ENUM_SINGLE(ES8388_ADCCONTROL3, 3, 4, es8388_mono_mux);
static const struct snd_kcontrol_new es8388_monomux_controls =
	SOC_DAPM_ENUM("Route", monomux);


static const struct snd_soc_dapm_widget es8388_dapm_widgets[] = {
    // DAC Part
    SND_SOC_DAPM_MIXER("Left Mixer", SND_SOC_NOPM, 0, 0,
        &es8388_left_mixer_controls[0],
        ARRAY_SIZE(es8388_left_mixer_controls)),
    SND_SOC_DAPM_MIXER("Right Mixer", SND_SOC_NOPM, 0, 0,
        &es8388_right_mixer_controls[0],
        ARRAY_SIZE(es8388_right_mixer_controls)),

	SND_SOC_DAPM_MUX("Left Line Mux", SND_SOC_NOPM, 0, 0,
		&es8388_left_line_controls),
    SND_SOC_DAPM_MUX("Right Line Mux", SND_SOC_NOPM, 0, 0,
        &es8388_right_line_controls),

    SND_SOC_DAPM_DAC("Left DAC"  , "Left Playback" , ES8388_DACPOWER, 7, 1),
    SND_SOC_DAPM_DAC("Right DAC" , "Right Playback", ES8388_DACPOWER, 6, 1),
    SND_SOC_DAPM_PGA("Left Out 1" , ES8388_DACPOWER, 5, 0, NULL, 0),
    //SND_SOC_DAPM_PGA("Right Out 1", ES8388_DACPOWER, 4, 0, NULL, 0),
    SND_SOC_DAPM_PGA_E("Right Out 1", ES8388_DACPOWER, 4, 0, NULL, 0, dac_start_event, SND_SOC_DAPM_POST_PMU|SND_SOC_DAPM_PRE_PMD),
    SND_SOC_DAPM_PGA("Left Out 2" , ES8388_DACPOWER, 3, 0, NULL, 0),
    SND_SOC_DAPM_PGA("Right Out 2", ES8388_DACPOWER, 2, 0, NULL, 0),


	SND_SOC_DAPM_OUTPUT("LOUT1"),
	SND_SOC_DAPM_OUTPUT("ROUT1"),
    //SND_SOC_DAPM_OUTPUT("LOUT2"),
    //SND_SOC_DAPM_OUTPUT("ROUT2"),
	SND_SOC_DAPM_OUTPUT("VREF"),


#if 1
    //--------------------------------------------
    // ADC Part
    //--------------------------------------------
	SND_SOC_DAPM_MUX("Differential Left Mux", SND_SOC_NOPM, 0, 0,
		&es8388_left_diffmux_controls),
	SND_SOC_DAPM_MUX("Differential Right Mux", SND_SOC_NOPM, 0, 0,
		&es8388_right_diffmux_controls),

	SND_SOC_DAPM_MUX("Left ADC Mux", SND_SOC_NOPM, 0, 0,
		&es8388_monomux_controls),
	SND_SOC_DAPM_MUX("Right ADC Mux", SND_SOC_NOPM, 0, 0,
		&es8388_monomux_controls),


    SND_SOC_DAPM_PGA("Left Analog Input" , ES8388_ADCPOWER, 7, 1, NULL, 0),
    SND_SOC_DAPM_PGA("Right Analog Input", ES8388_ADCPOWER, 6, 1, NULL, 0),
    SND_SOC_DAPM_ADC("Left ADC" , "Left Capture" , ES8388_ADCPOWER, 5, 1),
    SND_SOC_DAPM_ADC("Right ADC", "Right Capture", ES8388_ADCPOWER, 4, 1),

    SND_SOC_DAPM_MICBIAS_E("Mic Bias", ES8388_ADCPOWER, 3, 1, adc_start_event, SND_SOC_DAPM_POST_PMU|SND_SOC_DAPM_PRE_PMD),
#endif

    SND_SOC_DAPM_INPUT("MICIN"),
    SND_SOC_DAPM_INPUT("LINPUT1"),
    SND_SOC_DAPM_INPUT("LINPUT2"),
    SND_SOC_DAPM_INPUT("RINPUT1"),
    SND_SOC_DAPM_INPUT("RINPUT2"),
};


static const struct snd_soc_dapm_route es8388_intercon[] = {
    /* left mixer */
    {"Left Mixer", "Left Playback Switch"   , "Left DAC"},

    /* right mixer */
    {"Right Mixer", "Right Playback Switch" , "Right DAC"},

    /* left out 1 */
    {"Left Out 1" , NULL, "Left Mixer"},
    {"LOUT1"      , NULL, "Left Out 1"},

    /* right out 1 */
    {"Right Out 1", NULL, "Right Mixer"},
    {"ROUT1"      , NULL, "Right Out 1"},


#if 1
    /* Differential Mux */
    {"Differential Left Mux" , "Line 1", "LINPUT1"},
    {"Differential Right Mux", "Line 1", "RINPUT1"},
    {"Differential Left Mux" , "Line 2", "LINPUT2"},
    {"Differential Right Mux", "Line 2", "RINPUT2"},

    /* Left Line Mux */
    {"Left Line Mux", "Line 1"      , "LINPUT1"},
    {"Left Line Mux", "Line 2"      , "LINPUT2"},
    {"Left Line Mux", "Differential", "Differential Left Mux"},

    /* Right Line Mux */
    {"Right Line Mux", "Line 1"      , "RINPUT1"},
    {"Right Line Mux", "Line 2"      , "RINPUT2"},
    {"Right Line Mux", "Differential", "Differential Right Mux"},

    /* Left ADC Mux */
    {"Left ADC Mux", "Stereo"      , "Left Line Mux"},
//    {"Left ADC Mux", "Mono (Left)" , "Left Line Mux"},

    /* Right ADC Mux */
    {"Right ADC Mux", "Stereo"      , "Right Line Mux"},
//    {"Right ADC Mux", "Mono (Right)", "Right Line Mux"},

    /* ADC */
    {"Left ADC" , NULL, "Left ADC Mux"},
    {"Right ADC", NULL, "Right ADC Mux"},


    {"Left ADC" , NULL, "Mic Bias"},
    {"Right ADC", NULL, "Mic Bias"},

    {"Mic Bias", NULL, "MICIN"},
#endif
};


struct _coeff_div {
	u32 mclk;
	u32 rate;
	u16 fs;
	u8 sr:5;
	u8 single_double:1;
	u8 blckdiv:4;
};

/* codec hifi mclk clock divider coefficients */
#if 0
static const struct _coeff_div coeff_div[] = {
	/* 8k */
	{12000000,  8000, 1500, 0x1B, 0, 0xa},
    
	/* 11.025k */
	{12000000, 11025, 1088, 0x19, 0, 0xa},
    
    /* 12k */
	{12000000, 12000, 1000, 0x18, 0, 0xa},
    
	/* 16k */
	{12000000, 16000,  750, 0x17, 0, 0x6},
    
	/* 22.05k */
	{12000000, 22050,  544, 0x16, 0, 0x6},
    
    /* 24k */
	{12000000, 24000,  500, 0x15, 0, 0x6},
	
	/* 32k */
	{12000000, 32000,  375, 0x14, 0, 0x6},
    
	/* 44.1k */
	{11289600, 44100,  256, 0x02, 0, 0x4}, /* add for hdmi, zyy 2010.6.19 */
	{12000000, 44100,  272, 0x13, 0, 0x4},
    
	/* 48k */
                  
	{12000000, 48000,  250, 0x12, 0, 0x4},
	{12288000, 48000,  256, 0x02, 0, 0x4}, /* add for hdmi */
    
	/* 88.2k */
	{12000000, 88200,  136, 0x11, 1, 0x2},
    
	/* 96k */
	{12000000, 96000,  125, 0x10, 1, 0x2},
};
#else
static const struct _coeff_div coeff_div[] = {
	{11289600, 44100,  256, 0x02, 0, 0x4}, /* add for hdmi, zyy 2010.6.19 */
};
#endif

static inline int get_coeff(int mclk, int rate)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
        if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk) {
            alsa_dbg(KERN_ERR "es8388:************************** can get coeff for mclk %d @ rate %d\n",mclk, rate);
            return i;
        }
    }

    printk(KERN_ERR "es8388: could not get coeff for mclk %d @ rate %d\n", mclk, rate);
    return -EINVAL;
}


static unsigned int es8388_read_reg_cache(struct snd_soc_codec *codec,
				     unsigned int reg)
{
	u16 *cache = codec->reg_cache;
	if (reg >= codec->driver->reg_cache_size)
		return -1;
	return cache[reg];
}


static int es8388_set_dai_sysclk(struct snd_soc_dai *codec_dai,
		int clk_id, unsigned int freq, int dir)
{
    struct snd_soc_codec *codec = codec_dai->codec;
    struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);
    alsa_dbg("%s----%d\n",__FUNCTION__,__LINE__);
    switch (freq) {
        case 11289600:
        case 12000000:
        case 12288000:
        case 16934400:
        case 18432000:
            es8388->sysclk = freq;
            return 0;
    }
    return -EINVAL;
}

static int es8388_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
    struct snd_soc_codec *codec = codec_dai->codec;
    u8 iface = 0;
    u8 adciface = 0;
    u8 daciface = 0;
    alsa_dbg("%s----%d, fmt[%02x]\n",__FUNCTION__,__LINE__,fmt);

    iface    = snd_soc_read(codec, ES8388_IFACE);
    adciface = snd_soc_read(codec, ES8388_ADC_IFACE);
    daciface = snd_soc_read(codec, ES8388_DAC_IFACE);

    /* set master/slave audio interface */
    switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
        case SND_SOC_DAIFMT_CBM_CFM:    // MASTER MODE
            iface |= 0x80;
            break;
        case SND_SOC_DAIFMT_CBS_CFS:    // SLAVE MODE
            iface &= 0x7F;
            break;
        default:
            return -EINVAL;
    }

    /* interface format */
    switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
        case SND_SOC_DAIFMT_I2S:
            adciface &= 0xFC;
            //daciface &= 0xF9;  //updated by david-everest,5-25           
            daciface &= 0xF9;
            break;
        case SND_SOC_DAIFMT_RIGHT_J:
            break;
        case SND_SOC_DAIFMT_LEFT_J:
            break;
        case SND_SOC_DAIFMT_DSP_A:
            break;
        case SND_SOC_DAIFMT_DSP_B:
            break;
        default:
            return -EINVAL;
    }

    /* clock inversion */
    switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
        case SND_SOC_DAIFMT_NB_NF:
            iface    &= 0xDF;
            adciface &= 0xDF;
            //daciface &= 0xDF;    //UPDATED BY david-everest,5-25        
            daciface &= 0xBF;
            break;
        case SND_SOC_DAIFMT_IB_IF:
            iface    |= 0x20;
            //adciface &= 0xDF;    //UPDATED BY david-everest,5-25
            adciface |= 0x20;
            //daciface &= 0xDF;   //UPDATED BY david-everest,5-25
            daciface |= 0x40;
            break;
        case SND_SOC_DAIFMT_IB_NF:
            iface    |= 0x20;
           // adciface |= 0x40;  //UPDATED BY david-everest,5-25
            adciface &= 0xDF;
            //daciface |= 0x40;  //UPDATED BY david-everest,5-25
            daciface &= 0xBF;
            break;
        case SND_SOC_DAIFMT_NB_IF:
            iface    &= 0xDF;
            adciface |= 0x20;
            //daciface |= 0x20;  //UPDATED BY david-everest,5-25
            daciface |= 0x40;
            break;
        default:
            return -EINVAL;
    }

    snd_soc_write(codec, ES8388_IFACE    , iface);
    snd_soc_write(codec, ES8388_ADC_IFACE, adciface);
    snd_soc_write(codec, ES8388_DAC_IFACE, daciface);

    return 0;
}

static int es8388_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params,
		struct snd_soc_dai *dai)
{
    struct snd_soc_codec *codec =  dai->codec;
	u16 iface;

    alsa_dbg("es8388_pcm_hw_params()----%d, sampling rate[%d]\n", __LINE__, params_rate(params));

    if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
        alsa_dbg("es8388_pcm_hw_params()   playback stream \n\n");

        iface = snd_soc_read(codec, ES8388_DAC_IFACE) & 0xC7;

        /* bit size */
        switch (params_format(params)) {
        case SNDRV_PCM_FORMAT_S16_LE:
            iface |= 0x0018;
            break;
        case SNDRV_PCM_FORMAT_S20_3LE:
            iface |= 0x0008;
            break;
        case SNDRV_PCM_FORMAT_S24_LE:
            break;
        case SNDRV_PCM_FORMAT_S32_LE:
            iface |= 0x0020;
            break;
        }
        /* set iface & srate */
        snd_soc_write(codec, ES8388_DAC_IFACE, iface);
    }
    else {
        alsa_dbg("es8388_pcm_hw_params()   capture stream \n\n");

        iface = snd_soc_read(codec, ES8388_ADC_IFACE) & 0xE3;

        /* bit size */
        switch (params_format(params)) {
        case SNDRV_PCM_FORMAT_S16_LE:
            iface |= 0x000C;
            break;
        case SNDRV_PCM_FORMAT_S20_3LE:
            iface |= 0x0004;
            break;
        case SNDRV_PCM_FORMAT_S24_LE:
            break;
        case SNDRV_PCM_FORMAT_S32_LE:
            iface |= 0x0010;
            break;
        }
        /* set iface */
        snd_soc_write(codec, ES8388_ADC_IFACE, iface);
    }

	return 0;
}

static int es8388_mute(struct snd_soc_dai *codec_dai, int mute)
{
    struct snd_soc_codec *codec = codec_dai->codec;
    unsigned char val = 0;

    alsa_dbg("%s----%d, %d\n",__FUNCTION__,__LINE__,mute);

    val = snd_soc_read(codec, ES8388_DAC_MUTE);
    if (mute){
        val |= 0x04;
    }
    else{
        val &= ~0x04;
    }

    snd_soc_write(codec, ES8388_DAC_MUTE, val);

    return 0;
}

#define ES8388_USED_POWER_CONTROL 1

static int es8388_set_bias_level(struct snd_soc_codec *codec,
		enum snd_soc_bias_level level)
{
    int i;
	u16 *cache = codec->reg_cache;

    alsa_dbg("%s----%d, %s : %d\n",__FUNCTION__,__LINE__,
        level == 0 ? "BIAS_OFF" :
        level == 1 ? "BIAS_STANDBY" :
        level == 2 ? "PREPARE" : "BIAS_ON", level);

    switch(level)
    {
        case SND_SOC_BIAS_ON:
#ifdef CONFIG_COBY_MID7025
    		 tcc_hp_hw_mute(false);
    		 tcc_spk_hw_mute(false);
#else
            if(tcc_hp_is_valid()) {      /* Headphone */
                alsa_dbg("headphone\n");
                tcc_hp_hw_mute(false);
                tcc_spk_hw_mute(true);
            }
            else {                          /* Speaker */
                alsa_dbg("speaker\n");
                tcc_hp_hw_mute(true);
                tcc_spk_hw_mute(false);
            }
#endif

#if ES8388_USED_POWER_CONTROL
            // check Power status of ADC and DAC DLL...
            if (cache[ES8388_CHIPPOWER] & 0x0C) {   // ADC and DAC DLL power down...
                snd_soc_write(codec, ES8388_CHIPPOWER, cache[ES8388_CHIPPOWER] &0xF3);
            }
#endif

            break;

        case SND_SOC_BIAS_PREPARE:
            if(codec->dapm.bias_level == SND_SOC_BIAS_ON) {
                tcc_hp_hw_mute(true);
                tcc_spk_hw_mute(true);
            }
            else {
#if ES8388_USED_POWER_CONTROL
            //-------------------------------------------
            //- Resume from standby mode Sequence
            
                // Power up ADC / Analog Input / Micbias for Record
                snd_soc_write(codec, ES8388_ADCPOWER, 0x00);
                // Power up DAC and enable LOUT/ROUT
                snd_soc_write(codec, ES8388_DACPOWER , 0x30);
                // UnMute ADC and DAC
                snd_soc_write(codec, ES8388_ADCCONTROL7, 0x30);
                snd_soc_write(codec, ES8388_DACCONTROL3, 0x32);
                // Power up DEM and STM
                snd_soc_write(codec, ES8388_CHIPPOWER , 0x00);                
#else
            	/* updated by David-everest,5-25
                // Chip Power on
                snd_soc_write(codec, ES8388_CHIPPOWER, 0xF3);
                // VMID control
                snd_soc_write(codec, ES8388_CONTROL1 , 0x06);
                // ADC/DAC DLL power on
                snd_soc_write(codec, ES8388_CONTROL2 , 0xF3);
                */
                snd_soc_write(codec, ES8388_ADCPOWER, 0x00);
                snd_soc_write(codec, ES8388_DACPOWER , 0x30);
                snd_soc_write(codec, ES8388_CHIPPOWER , 0x00);
#endif
            }
            break;

        case SND_SOC_BIAS_STANDBY:
            if (codec->dapm.bias_level == SND_SOC_BIAS_OFF) {
                for(i = 0;i < ES8388_CACHEREGNUM;i++) {
                    codec->write(codec, i, cache[i]);
                }
            }
            
            tcc_hp_hw_mute(true);
            tcc_spk_hw_mute(true);
#if ES8388_USED_POWER_CONTROL
            //-------------------------------------------
            //- Power Down Sequence (To Standby Mode)

            // Mute ADC and DAC
            snd_soc_write(codec, ES8388_ADCCONTROL7, 0x34);
            snd_soc_write(codec, ES8388_DACCONTROL3, 0x36);
            // Power down DEM and STM
            snd_soc_write(codec, ES8388_CHIPPOWER, 0xF3);
            // Power Down ADC / Analog Input / Micbias for Record
            snd_soc_write(codec, ES8388_ADCPOWER, 0xFC);
            // Power down DAC and disable LOUT/ROUT
            snd_soc_write(codec, ES8388_DACPOWER, 0xC0);
#else
            /*
            // ADC/DAC DLL power on
            snd_soc_write(codec, ES8388_CONTROL2 , 0xFF);
            // Chip Power off
            snd_soc_write(codec, ES8388_CHIPPOWER, 0xF3);
            */
            snd_soc_write(codec, ES8388_ADCPOWER, 0x00);
            snd_soc_write(codec, ES8388_DACPOWER , 0x30);
            snd_soc_write(codec, ES8388_CHIPPOWER , 0x00);
#endif
            break;

        case SND_SOC_BIAS_OFF:
            tcc_hp_hw_mute(true);
            tcc_spk_hw_mute(true);
#if ES8388_USED_POWER_CONTROL
            snd_soc_write(codec, ES8388_DACCONTROL7, 0x06);
            snd_soc_write(codec, ES8388_DACCONTROL3, 0x36);
            // Power Down ADC / Analog Input / Micbias for Record
            snd_soc_write(codec, ES8388_ADCPOWER, 0xFC);
            // Power down DAC and disable LOUT/ROUT
            snd_soc_write(codec, ES8388_DACPOWER, 0xC0);
            // Power down ADC and DAC DLL
            snd_soc_write(codec, ES8388_CHIPPOWER, 0xFF);
#else
            /*
            // ADC/DAC DLL power off
            snd_soc_write(codec, ES8388_CONTROL2 , 0xFF);
            // Chip Control
            snd_soc_write(codec, ES8388_CONTROL1 , 0x00);
            // Chip Power off 
            snd_soc_write(codec, ES8388_CHIPPOWER, 0xFF);
            */
            snd_soc_write(codec, ES8388_ADCPOWER, 0xff);
            snd_soc_write(codec, ES8388_DACPOWER , 0xC0);
            snd_soc_write(codec, ES8388_CHIPPOWER , 0xC3);             
#endif
            break;
    }
	codec->dapm.bias_level = level;

	return 0;
}


#define ES8388_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |\
                    SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_44100 | \
                    SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

#define ES8388_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
                    SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops es8388_ops = {
    .hw_params    = es8388_pcm_hw_params,
    .set_fmt      = es8388_set_dai_fmt,
    .set_sysclk   = es8388_set_dai_sysclk,
    .digital_mute = es8388_mute,
};


static struct snd_soc_dai_driver es8388_dai = {
    .name = "ES8388",
    .playback = {
        .stream_name = "Playback",
        .channels_min = 2,
        .channels_max = 2,
        .rates = ES8388_RATES,
        .formats = ES8388_FORMATS,},
    .capture = {
        .stream_name = "Capture",
        .channels_min = 2,
        .channels_max = 2,
        .rates = ES8388_RATES,
        .formats = ES8388_FORMATS,},
    .ops = &es8388_ops,	
};

static int es8388_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
    // Power off
	es8388_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int es8388_resume(struct snd_soc_codec *codec)
{
	int i;
	u8 data[2];
	u16 *cache = codec->reg_cache;

	/* Sync reg_cache with the hardware */
	for (i = 0; i < ARRAY_SIZE(es8388_reg); i++) {
		data[0] = i;
		data[1] = cache[i] & 0xFF;
		codec->hw_write(codec->control_data, data, 2);
	}
	es8388_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	es8388_set_bias_level(codec, codec->dapm.suspend_bias_level);

	return 0;
}


/**********************************************************************************************************/
static int es8388_register_init(struct snd_soc_codec *codec)
{
	es8388_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

    snd_soc_write(codec, ES8388_MASTERMODE  , 0x00);    // SLAVE MODE, MCLK not divide
    snd_soc_write(codec, ES8388_CHIPPOWER   , 0xf3);    // Power down: ADC DEM, DAC DSM/DEM, ADC/DAC state machine, ADC/DAC ananlog reference
    snd_soc_write(codec, ES8388_DACCONTROL21, 0x80);    // DACLRC and ADCLRC same, ADC/DAC DLL power up, Enable MCLK input from PAD.

    snd_soc_write(codec, ES8388_CONTROL1   , 0x05);     // VMIDSEL (500 kohme divider enabled)
    snd_soc_write(codec, ES8388_CONTROL2 , 0x40);   // 
    
    snd_soc_write(codec, ES8388_DACPOWER   , 0x3C);     // DAC R/L Power on, OUT1 enable, OUT2 enable
    snd_soc_write(codec, ES8388_ADCPOWER   , 0x00);     // 
    snd_soc_write(codec, ES8388_ANAVOLMANAG, 0x7C);     // 

    //-----------------------------------------------------------------------------------------------------------------
    snd_soc_write(codec, ES8388_ADCCONTROL1, 0x99);     // MIC PGA gain: +24dB
    snd_soc_write(codec, ES8388_ADCCONTROL2, 0xf0);     // LINSEL(L-R differential), RINGSEL(L-R differential)
    snd_soc_write(codec, ES8388_ADCCONTROL3, 0x82);     // Input Select: LIN2/RIN2
    snd_soc_write(codec, ES8388_ADCCONTROL4, 0x4C);     // Left data = left ADC, right data = right ADC, 24 bits I2S
    snd_soc_write(codec, ES8388_ADCCONTROL5, 0x02);     // 256fs
    snd_soc_write(codec, ES8388_ADCCONTROL6, 0x30);     // Disable High pass filter

    snd_soc_write(codec, ES8388_LADC_VOL, 0x00);        // 0dB
    snd_soc_write(codec, ES8388_RADC_VOL, 0x00);        // 0dB

    //snd_soc_write(codec, ES8388_ADCCONTROL10, 0x3A);    // ALC stereo, Max gain(17.5dB), Min gain(0dB)
    snd_soc_write(codec, ES8388_ADCCONTROL10, 0xda);    // ALC stereo, Max gain(17.5dB), Min gain(0dB)  //ca
    snd_soc_write(codec, ES8388_ADCCONTROL11, 0xa0);    // ALCLVL(-1.5dB), ALCHLD(0ms)                  //a0
    snd_soc_write(codec, ES8388_ADCCONTROL12, 0x05);    // ALCDCY(1.64ms/363us), ALCATK(1664us/363.2us)
    snd_soc_write(codec, ES8388_ADCCONTROL13, 0x06);    // ALCMODE(ALC mode), ALCZC(disable), TIME_OUT(disable), WIN_SIZE(96 samples)
    snd_soc_write(codec, ES8388_ADCCONTROL14, 0xfb);    // NGTH(XXX), NGG(mute ADC output), NGAT(enable)


    //----------------------------------------------------------------------------------------------------------------
    snd_soc_write(codec, ES8388_DACCONTROL1, 0x18);     // I2S 16bits 
    snd_soc_write(codec, ES8388_DACCONTROL2, 0x02);     // 256fs

    snd_soc_write(codec, ES8388_LDAC_VOL, 0x00);    // left DAC volume
    snd_soc_write(codec, ES8388_RDAC_VOL, 0x00);    // right DAC volume

    snd_soc_write(codec, ES8388_DACCONTROL3, 0xE0);     // DAC unmute

    snd_soc_write(codec, ES8388_DACCONTROL17, 0xb8);    // left DAC to left mixer enable, 
    snd_soc_write(codec, ES8388_DACCONTROL18, 0x38);    // ???
    snd_soc_write(codec, ES8388_DACCONTROL19, 0x38);    // ???
    snd_soc_write(codec, ES8388_DACCONTROL20, 0xb8);    // right DAC to right mixer enable,

    snd_soc_write(codec, ES8388_CHIPPOWER, 0x00);   // ALL Block POWER ON
    //snd_soc_write(codec, ES8388_CONTROL2 , 0x72);   // updated by david-everest,5-25
    //mdelay(100);

    snd_soc_write(codec, ES8388_LOUT1_VOL, 0x1e);   // 
    snd_soc_write(codec, ES8388_ROUT1_VOL, 0x1e);   // 
    snd_soc_write(codec, ES8388_LOUT2_VOL, 0x1e);   // Enable LOUT2
    snd_soc_write(codec, ES8388_ROUT2_VOL, 0x1e);   // Enable ROUT2

    codec->dapm.bias_level = SND_SOC_BIAS_STANDBY;

	return 0;
}


static int es8388_probe(struct snd_soc_codec *codec)
{
	struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);
    int ret = 0;


    alsa_dbg("%s() \n", __func__);

	ret = snd_soc_codec_set_cache_io(codec, 8, 8, es8388->control_type);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}

#if 0   // B090183, _2011_12_19_
	ret = es8388_reset(codec);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to issue reset: %d\n", ret);
		return ret;
	}
#endif

    /* update register of es8388 */
    es8388_register_init(codec);

    snd_soc_add_controls(codec, es8388_snd_controls,
                 ARRAY_SIZE(es8388_snd_controls));

	info("Audio Codec Driver %s", ES8388_VERSION);

    return ret;
}


/* power down chip */
static int es8388_remove(struct snd_soc_codec *codec)
{
	es8388_set_bias_level(codec, SND_SOC_BIAS_OFF);
    snd_soc_dapm_free(&codec->dapm);

    return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_es8388 = {
	.probe = 	es8388_probe,
	.remove = 	es8388_remove,
	.suspend = 	es8388_suspend,
	.resume =	es8388_resume,

	.set_bias_level = es8388_set_bias_level,
	.reg_cache_size = ARRAY_SIZE(es8388_reg),
	.reg_word_size = sizeof(u16),
	.reg_cache_default = es8388_reg,
	.dapm_widgets = es8388_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(es8388_dapm_widgets),
	.dapm_routes = es8388_intercon,
	.num_dapm_routes = ARRAY_SIZE(es8388_intercon),

};



#if defined (CONFIG_I2C) || defined (CONFIG_I2C_MODULE)
static int es8388_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct es8388_priv *es8388;
	int ret;

    alsa_dbg("%s() \n", __func__);

	es8388 = kzalloc(sizeof(struct es8388_priv), GFP_KERNEL);
	if (es8388 == NULL)
		return -ENOMEM;

	i2c_set_clientdata(i2c, es8388);
	es8388->control_type = SND_SOC_I2C;

	ret =  snd_soc_register_codec(&i2c->dev,
			&soc_codec_dev_es8388, &es8388_dai, 1);
	if (ret < 0)
		kfree(es8388);
	return ret;
}

static int es8388_i2c_remove(struct i2c_client *client)
{
	snd_soc_unregister_codec(&client->dev);
	kfree(i2c_get_clientdata(client));
	return 0;
}


static const struct i2c_device_id es8388_i2c_id[] = {
	{ "es8388", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, es8388_i2c_id);

static struct i2c_driver es8388_i2c_driver = {
	.driver = {
		.name = "es8388 I2C Codec",
		.owner = THIS_MODULE,
	},
	.probe    = es8388_i2c_probe,
	.remove   = es8388_i2c_remove,
	.id_table = es8388_i2c_id,
};


static int __init es8388_modinit(void)
{
	int ret;

    alsa_dbg("%s() \n", __func__);
	ret = i2c_add_driver(&es8388_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register ES8388 I2C driver: %d\n",
		       ret);
	}

	return 0;
}
module_init(es8388_modinit);

static void __exit es8388_exit(void)
{
    alsa_dbg("%s() \n", __func__);
	i2c_del_driver(&es8388_i2c_driver);
}
module_exit(es8388_exit);
#endif


MODULE_DESCRIPTION("ASoC es8388 driver");
MODULE_AUTHOR("Liam Girdwood");
MODULE_LICENSE("GPL");
