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

#include "rt5633.h"
#define VERSION_rt5633 "alsa 1.0.23 version 0.3"
static const u16 rt5633_reg[0x80];
static unsigned int BLCK_FREQ=32;	//32fs,bitclk is 32 fs
module_param(BLCK_FREQ, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(BLCK_FREQ, "relationship between bclk and fs");
	
#define VIRTUAL_POWER_CONTROL 0x90


//******************************************************************************************************
//thread to check speaker or hp and enable related EQ
//******************************************************************************************************
#define RT5633_SPK_TIMER	0	//if enable this, MUST enable RT5633_EQ_FUNC_ENA and RT5633_EQ_FUNC_SEL==RT5633_EQ_FOR_MANUAL first!

#if (RT5633_SPK_TIMER == 1)

static struct timer_list spk_timer;
struct work_struct  spk_work;
static bool last_is_spk = false;	// need modify.

#endif

//*******************************************************************************************************
//ALC setting
//*******************************************************************************************************
#define RT5633_ALC_FUNC_ENA 	0

#if RT5633_ALC_FUNC_ENA

#define	RT5633_ALC_DISABLE	0
#define	RT5633_ALC_FOR_DAC	1
#define	RT5633_ALC_FOR_ADC	2
#define	RT5633_ALC_FOR_MANUAL	3

#define RT5633_ALC_FUNC_SEL RT5633_ALC_FOR_ADC

#endif

//*******************************************************************************************************
//EQ setting
//*******************************************************************************************************
#define RT5633_EQ_FUNC_ENA  	1

#if RT5633_EQ_FUNC_ENA

#define	RT5633_EQ_DISABLE 	0
#define	RT5633_EQ_FOR_DAC 	1
#define	RT5633_EQ_FOR_ADC 	2
#define	RT5633_EQ_FOR_MANUAL 	3

#define RT5633_EQ_FUNC_SEL RT5633_EQ_FOR_MANUAL

#endif
//********************************************************************************************************
static unsigned int reg90;
static int rt5633_write(struct snd_soc_codec *codec, unsigned int reg, unsigned int val);
static unsigned int rt5633_read(struct snd_soc_codec *codec, unsigned int reg);
#define rt5633_reset(c) rt5633_write(c, RT5633_RESET, 0)
//#define rt5633_write_mask(c, reg, val, mask) snd_soc_update_bits(c, reg, mask, val)
static int rt5633_reg_init(struct snd_soc_codec *codec);

struct rt5633_priv {
	enum snd_soc_control_type control_type;
	int master;
	int sysclk;
};

int rt5633_write_mask(struct snd_soc_codec *codec, unsigned short reg,
    			 unsigned int value, unsigned int mask)
{
//  	printk(KERN_INFO "%s reg=0x%x, mask=0x%x, val=0x%x\n", __func__, reg, mask, value); 
    return snd_soc_update_bits(codec, reg, mask, value);
}

//static struct snd_soc_device *rt5633_socdev;
//static struct snd_soc_codec *rt5633_codec;
/*
 * read rt5633 register cache
 */
static inline unsigned int rt5633_read_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg)
{
	u16 *cache = codec->reg_cache;
	if (reg < 1 || reg > (ARRAY_SIZE(rt5633_reg) + 1))
		return -1;
	return cache[reg];
}


/*
 * write rt5633 register cache
 */

static inline void rt5633_write_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int value)
{
	u16 *cache = codec->reg_cache;
	if (reg < 0 || reg > 0x7e)
		return;
	cache[reg] = value;
}


static int rt5633_write(struct snd_soc_codec *codec, unsigned int reg, unsigned int val)
{
	u8 data[3];
	
	if (reg > 0x7e) {
		if (reg == 0x90) 
			reg90 = val;

		return 0;
	}
	
	data[0] = reg;
	data[1] = (0xff00 & val) >> 8;
	data[2] = (0xff & val);
	
	if (codec->hw_write(codec->control_data, data, 3) == 3) {
		rt5633_write_reg_cache(codec, reg, val);
//		printk(KERN_INFO "%s reg=0x%x, val=0x%x\n", __func__, reg, val);
		return 0;	
	} else {
		printk(KERN_ERR "%s failed\n", __func__);
		return -EIO;	
	}
}

static unsigned int rt5633_read(struct snd_soc_codec *codec, unsigned int reg)
{
	u8 data[2] = {0};
	unsigned int value = 0x0;
	
	if (reg > 0x7e) {
		if (reg == 0x90)
		     return reg90;
	}
	
	data[0] = reg;
	if (codec->hw_write(codec->control_data, data, 1) == 1) {
		i2c_master_recv(codec->control_data, data, 2);
		value = (data[0] << 8) | data[1];

//  	printk(KERN_INFO "%s reg=0x%x, val=0x%x\n", __func__, reg, value); 

		return value;	
	} else {
		printk(KERN_ERR "%s failed\n", __func__);
		return -EIO;	
	}		
}


struct rt5633_init_reg{
	u8 reg;
	u16 val;
};

static int rt5633_write_index(struct snd_soc_codec *codec, unsigned int index,unsigned int value)
{
    unsigned char RetVal = 0;   

    RetVal = rt5633_write(codec,RT5633_PRI_REG_ADD,index);
    
    if(RetVal != 0)
      return RetVal;
    
    RetVal = rt5633_write(codec,RT5633_PRI_REG_DATA,value);
    return RetVal;    
}

unsigned int rt5633_read_index(struct snd_soc_codec *codec, unsigned int reg)
{
	unsigned int value = 0x0;
	rt5633_write(codec,RT5633_PRI_REG_ADD,reg);
	value=rt5633_read(codec,RT5633_PRI_REG_DATA);	
	
	return value;
}

void rt5633_write_index_mask(struct snd_soc_codec *codec, unsigned int reg,unsigned int value,unsigned int mask)
{
	unsigned  int CodecData;

	if(!mask)
		return; 

	if(mask!=0xffff)
	 {
		CodecData=rt5633_read_index(codec,reg);		
		CodecData&=~mask;
		CodecData|=(value&mask);
		rt5633_write_index(codec,reg,CodecData);
	 }		
	else
	{
		rt5633_write_index(codec,reg,value);
	}
}


static struct rt5633_init_reg init_list[] = {
	{RT5633_SPK_OUT_VOL		, 0xe000},//speaker output volume is 0db by default,
	{RT5633_SPK_HP_MIXER_CTRL	, 0x0020},//HP from HP_VOL	
	{RT5633_HP_OUT_VOL 		, 0xc0c0},//HP output volume is 0 db by default
	{RT5633_AUXOUT_VOL		, 0x0010},//Auxout volume is 0db by default
	{RT5633_REC_MIXER_CTRL		, 0x7d7d},//ADC Record Mixer Control
	{RT5633_ADC_CTRL		, 0x000a},
	{RT5633_MIC_CTRL_2		, 0x7700},//boost 40db
	{RT5633_HPMIXER_CTRL		, 0x3e3e},//"HP Mixer Control"
//	{RT5633_AUXMIXER_CTRL		, 0x3e3e},//"AUX Mixer Control"
	{RT5633_SPKMIXER_CTRL		, 0x08fc},//"SPK Mixer Control"
	{RT5633_SPK_AMP_CTRL		, 0x0000},
//	{RT5633_GEN_PUR_CTRL_1		, 0x8C00}, //set spkratio to auto  
	{RT5633_ZC_SM_CTRL_1		, 0x0001},	//Disable Zero Cross
	{RT5633_ZC_SM_CTRL_2		, 0x3000},	//Disable Zero cross
	{RT5633_MIC_CTRL_1       	, 0x8808}, //set mic1 to differnetial mode	
	{RT5633_DEPOP_CTRL_2		, 0xB000},
	{RT5633_PRI_REG_ADD		    , 0x0056},
	{RT5633_PRI_REG_DATA		, 0x303f},	
	{RT5633_ALC_CTRL_1		    , 0x0808},
	{RT5633_ALC_CTRL_2		    , 0x0003},
	{RT5633_ALC_CTRL_3		    , 0xe081},
#if 0	//internal hp detect
	{RT5633_ZC_SM_CTRL_1		, 0x04b0},	
	{RT5633_ZC_SM_CTRL_2		, 0x3000},
	{RT5633_JACK_DET_CTRL           , 0x6980},		
#endif	
};

#define RT5633_INIT_REG_LEN 	ARRAY_SIZE(init_list)

//*******************************************************************************************************************
//ALC setting function
//*******************************************************************************************************************
static void rt5633_alc_enable(struct snd_soc_codec *codec,unsigned int EnableALC)
{
	if(EnableALC)
	{
		rt5633_write(codec, 0x64,0x0a0f);
		rt5633_write(codec, 0x65,0x0005);
		rt5633_write(codec, 0x66,0xe080);
		
	}
	else
	{
		rt5633_write_mask(codec, 0x66,0x0000,0x4000); //disable ALC		
	}		
}
//*******************************************************************************************************************
//EQ parameter
static enum
{
	NORMAL=0,
	CLUB,
	DANCE,
	LIVE,	
	POP,
	ROCK,
	OPPO,
	TREBLE,
	BASS,
	RECORD,	
	HFREQ,
	SPK_FR,	
};

typedef struct  _HW_EQ_PRESET
{
	u16 	HwEqType;
	u16 	EqValue[22];
	u16  HwEQCtrl;

}HW_EQ_PRESET;


HW_EQ_PRESET HwEq_Preset[]={
/*       0x0    0x1    0x2     0x3   0x4    0x5	   0x6	  0x7	0x8     0x9    0xa    0xb    0xc    0x0d   0x0e    0x0f	   0x10  0x11  0x12   0x13    0x14  0x15    0x70*/
{NORMAL,{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x0000},			
{CLUB  ,{0x1C10,0x0000,0xC1CC,0x1E5D,0x0699,0xCD48,0x188D,0x0699,0xC3B6,0x1CD0,0x0699,0x0436,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x000E},
{DANCE ,{0x1F2C,0x095B,0xC071,0x1F95,0x0616,0xC96E,0x1B11,0xFC91,0xDCF2,0x1194,0xFAF2,0x0436,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x000F},
{LIVE  ,{0x1EB5,0xFCB6,0xC24A,0x1DF8,0x0E7C,0xC883,0x1C10,0x0699,0xDA41,0x1561,0x0295,0x0436,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x000F},
{POP   ,{0x1EB5,0xFCB6,0xC1D4,0x1E5D,0x0E23,0xD92E,0x16E6,0xFCB6,0x0000,0x0969,0xF988,0x0436,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x000F},
{ROCK  ,{0x1EB5,0xFCB6,0xC071,0x1F95,0x0424,0xC30A,0x1D27,0xF900,0x0C5D,0x0FC7,0x0E23,0x0436,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x000F},
{OPPO  ,{0x0000,0x0000,0xCA4A,0x17F8,0x0FEC,0xCA4A,0x17F8,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x000F},
{TREBLE,{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x188D,0x1699,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x0010},
{BASS  ,{0x1A43,0x0C00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x0001},
{RECORD,{0x1E3C,0xF405,0xC1E0,0x1E39,0x2298,0xDF29,0x0701,0x1D18,0xF34B,0x0CA9,0xF805,0xF9CC,0xF405,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x001F},
{HFREQ, {0x1C10,0x0000,0xC883,0x1C10,0x0000,0xD588,0x1C10,0x0000,0xE904,0x1C10,0x0000,0xFBCA,0x0699,0x05E9,0x1A97,0x1D2B,0x0000,0x0003,0x0007,0x0388,0x15FF,0xFE43},0x0050},
{SPK_FR,{0x1CD4,0xF405,0xe0bf,0x0f9e,0xfa19,0x07CA,0x12AF,0xF805,0xE904,0x1C10,0x0000,0x1C8B,0x0000,0xc5e1,0x1afb,0x1d46,0x0000,0x0000,0x0003,0x0000,0x0000,0x0000},0x0090},
	
};
//*******************************************************************************************************************
#define EQ_DEFAULT_PRESET BASS
//*******************************************************************************************************************
static void rt5633_update_eqmode(struct snd_soc_codec *codec, int mode)
{
	u16 HwEqIndex=0;

		if(mode==RECORD)
		{	 
			rt5633_write_mask(codec, 0x3a,0x1800,0x1800); //enable ADC power
		}
		else
		{	 
			rt5633_write_mask(codec, 0x3a,0x0700,0x0700);//enable DAC power
		}		

	if(mode==NORMAL)
	{
		/*clear EQ parameter*/
		for(HwEqIndex=0;HwEqIndex<=0x15;HwEqIndex++)
		{
			rt5633_write_index(codec, HwEqIndex, HwEq_Preset[mode].EqValue[HwEqIndex]);
		}
		
		rt5633_write(codec, 0x70,0x0);		/*disable EQ block*/
	}
	else
	{		
		rt5633_write(codec, 0x70,HwEq_Preset[mode].HwEQCtrl);

		/*Fill EQ parameter*/
		for(HwEqIndex=0;HwEqIndex<=0x15;HwEqIndex++)
		{
			rt5633_write_index(codec, HwEqIndex, HwEq_Preset[mode].EqValue[HwEqIndex]);
		}		
		
		if(mode==RECORD)
		{	//for ADC update EQ 
			rt5633_write_mask(codec, 0x6e,0xa000,0xa000);
		}
		else
		{	//for DAC update EQ 
			rt5633_write_mask(codec, 0x6e,0x2000,0xa000);
		}	
	}
}
//*******************************************************************************************************************

#if (RT5633_SPK_TIMER == 1)

static void spk_work_handler(struct work_struct *work)
{
	struct snd_soc_device *socdev = rt5633_socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	bool is_spk = (rt5633_read(codec, 0x4a)) & 0x10;	//detect rt5633 reg4a[4], 1'b:SPK, 0'b:HP ;
	
	if(last_is_spk != is_spk)
		printk("%s---%s is in use.last is %s in use\n", __FUNCTION__,is_spk?"speaker":"headphone",last_is_spk?"speaker":"headphone");
		
	if(is_spk && !last_is_spk){

#if (RT5633_EQ_FUNC_SEL==RT5633_EQ_FOR_MANUAL) 		

		rt5633_update_eqmode(codec, NORMAL);		// SPK is in use, enable EQ mode of NORMAL.
		
#endif		
		
	}else if(!is_spk && last_is_spk){

#if (RT5633_EQ_FUNC_SEL==RT5633_EQ_FOR_MANUAL)
		
		rt5633_update_eqmode(codec,HFREQ);		

#endif
		
	}
	last_is_spk = is_spk;
}

/* timer to judge SPK or HP in use, and handle EQ issues accordingly. */
void spk_timer_callback(unsigned long data )
{	
	int ret = 0;

	schedule_work(&spk_work);

  ret = mod_timer(&spk_timer, jiffies + msecs_to_jiffies(1000));
  if (ret) printk("Error in mod_timer\n");
}

#endif


static int rt5633_reg_init(struct snd_soc_codec *codec)
{
	int i;
	for (i = 0; i < RT5633_INIT_REG_LEN; i ++) {
		rt5633_write(codec, init_list[i].reg, init_list[i].val);	
	}	
	
	return 0;
}

static const char *rt5633_spo_source_sel[] = {"VMID", "HPMIX", "SPKMIX", "AUXMIX"};
static const char *rt5633_input_mode_source_sel[] = {"Single-end", "Differential"}; 
static const char *rt5633_auxout_mode_source_sel[] = {"Differential", "Stereo"}; 
static const char *rt5633_mic_boost[] = {"Bypass", "+20db", "+24db", "+30db",	
			"+35db", "+40db", "+44db", "+50db", "+52db"};
static const char *rt5633_spor_source_sel[] = {"RN", "RP", "LN", "VMID"};

static const struct soc_enum rt5633_enum[] = {
SOC_ENUM_SINGLE(RT5633_SPKMIXER_CTRL, 10, 4, rt5633_spo_source_sel), 	 /*0*/
SOC_ENUM_SINGLE(RT5633_MIC_CTRL_1, 15, 2,  rt5633_input_mode_source_sel),	/*1*/
SOC_ENUM_SINGLE(RT5633_MIC_CTRL_1, 7, 2,  rt5633_input_mode_source_sel),	/*2*/
SOC_ENUM_SINGLE(RT5633_SPK_OUT_VOL, 12, 2, rt5633_input_mode_source_sel),	/*3*/
SOC_ENUM_SINGLE(RT5633_MIC_CTRL_2, 12, 8, rt5633_mic_boost),			/*4*/
SOC_ENUM_SINGLE(RT5633_MIC_CTRL_2, 8, 8, rt5633_mic_boost),			/*5*/
SOC_ENUM_SINGLE(RT5633_SPK_OUT_VOL, 13, 4, rt5633_spor_source_sel), /*6*/
SOC_ENUM_SINGLE(RT5633_AUXOUT_VOL, 14, 2, rt5633_auxout_mode_source_sel), /*7*/
};

static const struct snd_kcontrol_new rt5633_snd_controls[] = {
SOC_ENUM("MIC1 Mode Control",  rt5633_enum[1]),   
SOC_ENUM("MIC1 Boost", rt5633_enum[4]),

SOC_ENUM("MIC2 Mode Control", rt5633_enum[2]),
SOC_ENUM("MIC2 Boost", rt5633_enum[5]),
SOC_ENUM("Classab Mode Control", rt5633_enum[3]),
SOC_ENUM("SPKR Out Control", rt5633_enum[6]),
SOC_ENUM("AUXOUT Control", rt5633_enum[7]),

SOC_DOUBLE("Line1 Capture Volume", RT5633_LINE_IN_1_VOL, 8, 0, 31, 1),
SOC_DOUBLE("Line2 Capture Volume", RT5633_LINE_IN_2_VOL, 8, 0, 31, 1),

SOC_SINGLE("MIC1 Playback Volume", RT5633_MIC_CTRL_1, 8, 31, 1),
SOC_SINGLE("MIC2 Playback Volume", RT5633_MIC_CTRL_1, 0, 31, 1),

SOC_SINGLE("AXOL Playback Switch", RT5633_AUXOUT_VOL, 15, 1, 1),
SOC_SINGLE("AXOR Playback Switch", RT5633_AUXOUT_VOL, 7, 1, 1),
SOC_DOUBLE("AUX Playback Volume", RT5633_AUXOUT_VOL, 8, 0, 31, 1),
SOC_SINGLE("SPK Playback Switch", RT5633_SPK_OUT_VOL, 15, 1, 1),
SOC_DOUBLE("SPK Playback Volume", RT5633_SPK_OUT_VOL, 5, 0, 31, 1),
SOC_SINGLE("HPL Playback Switch", RT5633_HP_OUT_VOL, 15, 1, 1),
SOC_SINGLE("HPR Playback Switch", RT5633_HP_OUT_VOL, 7, 1, 1),
SOC_DOUBLE("HP Playback Volume", RT5633_HP_OUT_VOL, 8, 0, 31, 1),
};

//HP power on depop
static void hp_depop_mode2(struct snd_soc_codec *codec)
{
        rt5633_write_mask(codec,RT5633_PWR_MANAG_ADD3,PWR_MAIN_BIAS|PWR_VREF,PWR_VREF|PWR_MAIN_BIAS);
		rt5633_write_mask(codec,RT5633_DEPOP_CTRL_1,PW_SOFT_GEN,PW_SOFT_GEN);
		rt5633_write_mask(codec,RT5633_PWR_MANAG_ADD3,PWR_HP_AMP,PWR_HP_AMP);
		rt5633_write_mask(codec,RT5633_DEPOP_CTRL_1,EN_DEPOP_2,EN_DEPOP_2);
		schedule_timeout_uninterruptible(msecs_to_jiffies(300));
        rt5633_write_mask(codec,RT5633_PWR_MANAG_ADD3,PWR_HP_DIS_DEPOP|PWR_HP_AMP_DRI,PWR_HP_DIS_DEPOP|PWR_HP_AMP_DRI);
		
        rt5633_write_mask(codec,RT5633_PWR_MANAG_ADD4,PWR_HP_L_VOL|PWR_HP_R_VOL,PWR_HP_L_VOL|PWR_HP_R_VOL);
        
         
        rt5633_write_mask(codec,RT5633_DEPOP_CTRL_1,0,EN_DEPOP_2);
}

//HP mute/unmute depop
static void hp_mute_unmute_depop(struct snd_soc_codec *codec,unsigned int Mute)
{
	if(Mute)
	{
		rt5633_write_mask(codec,RT5633_DEPOP_CTRL_1,PW_SOFT_GEN|EN_SOFT_FOR_S_M_DEPOP|EN_HP_R_M_UM_DEPOP|EN_HP_L_M_UM_DEPOP
												   ,PW_SOFT_GEN|EN_SOFT_FOR_S_M_DEPOP|EN_HP_R_M_UM_DEPOP|EN_HP_L_M_UM_DEPOP);
		rt5633_write_mask(codec, RT5633_HP_OUT_VOL, 0x8080, 0x8080);				
		mdelay(80);
		rt5633_write_mask(codec,RT5633_DEPOP_CTRL_1,0,PW_SOFT_GEN|EN_SOFT_FOR_S_M_DEPOP|EN_HP_R_M_UM_DEPOP|EN_HP_L_M_UM_DEPOP);
	}
	else
	{
		rt5633_write_mask(codec,RT5633_DEPOP_CTRL_1,PW_SOFT_GEN|EN_SOFT_FOR_S_M_DEPOP|EN_HP_R_M_UM_DEPOP|EN_HP_L_M_UM_DEPOP
												   ,PW_SOFT_GEN|EN_SOFT_FOR_S_M_DEPOP|EN_HP_R_M_UM_DEPOP|EN_HP_L_M_UM_DEPOP);
		rt5633_write_mask(codec, RT5633_HP_OUT_VOL, 0x0000, 0x8080);
		mdelay(80);
		rt5633_write_mask(codec,RT5633_DEPOP_CTRL_1,0,PW_SOFT_GEN|EN_SOFT_FOR_S_M_DEPOP|EN_HP_R_M_UM_DEPOP|EN_HP_L_M_UM_DEPOP);
	}

}


static const struct snd_kcontrol_new rt5633_recmixl_mixer_controls[] = {
SOC_DAPM_SINGLE("HPMIXL Capture Switch", RT5633_REC_MIXER_CTRL, 14, 1, 1),
SOC_DAPM_SINGLE("AUXMIXL Capture Switch", RT5633_REC_MIXER_CTRL, 13, 1, 1),
SOC_DAPM_SINGLE("SPKMIX Capture Switch", RT5633_REC_MIXER_CTRL, 12, 1, 1),
SOC_DAPM_SINGLE("LINE1L Capture Switch", RT5633_REC_MIXER_CTRL, 11, 1, 1),
SOC_DAPM_SINGLE("LINE2L Capture Switch", RT5633_REC_MIXER_CTRL, 10, 1, 1),
SOC_DAPM_SINGLE("MIC1 Capture Switch", RT5633_REC_MIXER_CTRL, 9, 1, 1),
SOC_DAPM_SINGLE("MIC2 Capture Switch", RT5633_REC_MIXER_CTRL, 8, 1, 1),
};

static const struct snd_kcontrol_new rt5633_recmixr_mixer_controls[] = {
SOC_DAPM_SINGLE("HPMIXR Capture Switch", RT5633_REC_MIXER_CTRL, 6, 1, 1),
SOC_DAPM_SINGLE("AUXMIXR Capture Switch", RT5633_REC_MIXER_CTRL, 5, 1, 1),
SOC_DAPM_SINGLE("SPKMIX Capture Switch", RT5633_REC_MIXER_CTRL, 4, 1, 1),
SOC_DAPM_SINGLE("LINE1R Capture Switch", RT5633_REC_MIXER_CTRL, 3, 1, 1),
SOC_DAPM_SINGLE("LINE2R Capture Switch", RT5633_REC_MIXER_CTRL, 2, 1, 1),
SOC_DAPM_SINGLE("MIC1 Capture Switch", RT5633_REC_MIXER_CTRL, 1, 1, 1),
SOC_DAPM_SINGLE("MIC2 Capture Switch", RT5633_REC_MIXER_CTRL, 0, 1, 1),
};


static const struct snd_kcontrol_new rt5633_hp_mixl_mixer_controls[] = {
SOC_DAPM_SINGLE("RECMIXL Playback Switch", RT5633_HPMIXER_CTRL, 13, 1, 1),
SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_HPMIXER_CTRL, 12, 1, 1),	
SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_HPMIXER_CTRL, 11, 1, 1),	
SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_HPMIXER_CTRL, 10, 1, 1),	
SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_HPMIXER_CTRL, 9, 1, 1),	
SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_HPMIXER_CTRL, 8, 1, 1),


};

static const struct snd_kcontrol_new rt5633_hp_mixr_mixer_controls[] = {
SOC_DAPM_SINGLE("RECMIXR Playback Switch", RT5633_HPMIXER_CTRL, 5, 1, 1),
SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_HPMIXER_CTRL, 4, 1, 1),	
SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_HPMIXER_CTRL, 3, 1, 1),	
SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_HPMIXER_CTRL, 2, 1, 1),	
SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_HPMIXER_CTRL, 1, 1, 1),	
SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_HPMIXER_CTRL, 0, 1, 1),
};

static const struct snd_kcontrol_new rt5633_auxmixl_mixer_controls[] = {
SOC_DAPM_SINGLE("RECMIXL Playback Switch", RT5633_AUXMIXER_CTRL, 13, 1, 1),
SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_AUXMIXER_CTRL, 12, 1, 1),
SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_AUXMIXER_CTRL, 11, 1, 1),
SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_AUXMIXER_CTRL, 10, 1, 1),
SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_AUXMIXER_CTRL, 9, 1, 1),
SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_AUXMIXER_CTRL, 8, 1, 1),

};

static const struct snd_kcontrol_new rt5633_auxmixr_mixer_controls[] = {
SOC_DAPM_SINGLE("RECMIXR Playback Switch", RT5633_AUXMIXER_CTRL, 5, 1, 1),
SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_AUXMIXER_CTRL, 4, 1, 1),
SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_AUXMIXER_CTRL, 3, 1, 1),
SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_AUXMIXER_CTRL, 2, 1, 1),
SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_AUXMIXER_CTRL, 1, 1, 1),
SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_AUXMIXER_CTRL, 0, 1, 1),
};

static const struct snd_kcontrol_new rt5633_spkmixr_mixer_controls[]  = {
SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_SPKMIXER_CTRL, 7, 1, 1),	
SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_SPKMIXER_CTRL, 6, 1, 1),
SOC_DAPM_SINGLE("LINE1L Playback Switch", RT5633_SPKMIXER_CTRL, 5, 1, 1),
SOC_DAPM_SINGLE("LINE1R Playback Switch", RT5633_SPKMIXER_CTRL, 4, 1, 1),
SOC_DAPM_SINGLE("LINE2L Playback Switch", RT5633_SPKMIXER_CTRL, 3, 1, 1),
SOC_DAPM_SINGLE("LINE2R Playback Switch", RT5633_SPKMIXER_CTRL, 2, 1, 1),
SOC_DAPM_SINGLE("DACL Playback Switch", RT5633_SPKMIXER_CTRL, 1, 1, 1),
SOC_DAPM_SINGLE("DACR Playback Switch", RT5633_SPKMIXER_CTRL, 0, 1, 1),
};



static const struct snd_kcontrol_new rt5633_spo_mux_control = 
SOC_DAPM_ENUM("Route", rt5633_enum[0]);




static int spk_event(struct snd_soc_dapm_widget *w, 
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	unsigned int l, r;
	
	l = (rt5633_read(codec, RT5633_PWR_MANAG_ADD4) & (0x01 << 15)) >> 15;
	r = (rt5633_read(codec, RT5633_PWR_MANAG_ADD4) & (0x01 << 14)) >> 14;
	
	switch (event) {
	case SND_SOC_DAPM_PRE_PMD:
		if (l && r) 
		{
			rt5633_write_mask(codec, RT5633_SPK_OUT_VOL, 0x8000, 0x8000);
           			
			rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD1, 0x0000, 0x2020);		
		}
		
		break;
	case SND_SOC_DAPM_POST_PMU:		
		if (l && r) 
		{
			rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD1, 0x2020, 0x2020);
			rt5633_write_mask(codec, RT5633_SPK_OUT_VOL, 0x0000, 0x8000);
			 rt5633_write(codec, RT5633_DAC_DIG_VOL, 0x1010);
			 rt5633_write_index(codec, 0X45, 0X4100);
		}
		break;
	default:
		return -EINVAL;	
	}
	
	return 0;
}


static int open_hp_end_widgets(struct snd_soc_codec *codec)
{

	hp_mute_unmute_depop(codec,0);

	return 0;
}

static int close_hp_end_widgets(struct snd_soc_codec *codec)
{
	hp_mute_unmute_depop(codec,1);

	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD3, 0x0000, 0x000f); 

	return 0;
}

static int hp_event(struct snd_soc_dapm_widget *w, 
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	unsigned int l, r;
	static unsigned int hp_out_enable=0;
	
	l = (rt5633_read(codec, RT5633_PWR_MANAG_ADD4) & (0x01 << 11)) >> 11;
	r = (rt5633_read(codec, RT5633_PWR_MANAG_ADD4) & (0x01 << 10)) >> 10;
	
	switch (event) {
	case SND_SOC_DAPM_PRE_PMD:

		if ((l && r)&&(hp_out_enable))  
		{
			close_hp_end_widgets(codec);
			hp_out_enable=0;
		}

	break;

	case SND_SOC_DAPM_POST_PMU:

		if ((l && r)&&(!hp_out_enable))
		{
		 	hp_depop_mode2(codec);
			open_hp_end_widgets(codec);
			hp_out_enable=1;
			rt5633_write(codec, RT5633_DAC_DIG_VOL, 0x0000);
		}

	break;

	default:
		return -EINVAL;	
	}
	
	return 0;
}

static int auxout_event(struct snd_soc_dapm_widget *w, 
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	unsigned int l, r;
	static unsigned int aux_out_enable=0;
	
	l = (rt5633_read(codec, RT5633_PWR_MANAG_ADD4) & (0x01 << 9)) >> 9;
	r = (rt5633_read(codec, RT5633_PWR_MANAG_ADD4) & (0x01 << 8)) >> 8;
	
	switch (event) {
	case SND_SOC_DAPM_PRE_PMD:

		if ((l && r)&&(aux_out_enable))  
		{
			rt5633_write_mask(codec, RT5633_AUXOUT_VOL, 0x8080, 0x8080);
			aux_out_enable=0;
		}

	break;

	case SND_SOC_DAPM_POST_PMU:

		if ((l && r)&&(!aux_out_enable))
		{
			rt5633_write_mask(codec, RT5633_AUXOUT_VOL, 0x0000, 0x8080);
			aux_out_enable=1;
		}

	break;

	default:
		return -EINVAL;	
	}
	
	return 0;
}


static int dac_event(struct snd_soc_dapm_widget *w, 
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	static unsigned int dac_enable=0;
	
	switch (event) {
		
	case SND_SOC_DAPM_PRE_PMD:
	
		printk("dac_event --SND_SOC_DAPM_PRE_PMD\n");	
		if (dac_enable)  
		{			

#if RT5633_EQ_FUNC_ENA

	#if (RT5633_EQ_FUNC_SEL==RT5633_EQ_FOR_DAC)

		rt5633_update_eqmode(codec,NORMAL);	//disable EQ

	#endif

#endif

#if RT5633_ALC_FUNC_ENA

	#if (RT5633_ALC_FUNC_SEL==RT5633_ALC_FOR_DAC)

		rt5633_alc_enable(codec,0);		//disable ALC

	#endif

#endif

			dac_enable=0;
		}
		break;
		
	case SND_SOC_DAPM_POST_PMU:
	
		printk("dac_event --SND_SOC_DAPM_POST_PMU\n");	
		if(!dac_enable)
		{

#if RT5633_EQ_FUNC_ENA

	#if (RT5633_EQ_FUNC_SEL==RT5633_EQ_FOR_DAC)
	
		rt5633_update_eqmode(codec,EQ_DEFAULT_PRESET); //enable EQ preset

	#endif

#endif

#if RT5633_ALC_FUNC_ENA

	#if (RT5633_ALC_FUNC_SEL==RT5633_ALC_FOR_DAC)

		rt5633_alc_enable(codec,1);		//enable ALC

	#endif

#endif
			dac_enable=1;
		}
		break;
	default:
		return 0;	
	}
	
	return 0;
}


static int adc_event(struct snd_soc_dapm_widget *w, 
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	static unsigned int adc_enable=0;
	
	switch (event) {
	case SND_SOC_DAPM_PRE_PMD:
		printk("adc_event --SND_SOC_DAPM_PRE_PMD\n");	
		if (adc_enable)  
		{			

#if RT5633_EQ_FUNC_ENA

	#if (RT5633_EQ_FUNC_SEL==RT5633_EQ_FOR_ADC)

		rt5633_update_eqmode(codec,NORMAL); //disable EQ
	
	#endif

#endif


#if RT5633_ALC_FUNC_ENA

	#if (RT5633_ALC_FUNC_SEL==RT5633_ALC_FOR_ADC)

		rt5633_alc_enable(codec,0); //disable ALC

	#endif

#endif
			adc_enable=0;
		}
		break;
	case SND_SOC_DAPM_POST_PMU:
		printk("adc_event --SND_SOC_DAPM_POST_PMU\n");	
		if(!adc_enable)
		{

#if RT5633_EQ_FUNC_ENA

	#if (RT5633_EQ_FUNC_SEL==RT5633_EQ_FOR_ADC)

		rt5633_update_eqmode(codec,EQ_DEFAULT_PRESET); //enable EQ preset
		
	#endif

#endif

#if RT5633_ALC_FUNC_ENA

	#if (RT5633_ALC_FUNC_SEL==RT5633_ALC_FOR_ADC)

		rt5633_alc_enable(codec,1);	//enable ALC

	#endif

#endif
			adc_enable=1;
		}
		break;
	default:
		return 0;
	}
	
	return 0;
}


static const struct snd_soc_dapm_widget rt5633_dapm_widgets[] = {
	
SND_SOC_DAPM_INPUT("MIC1"),
SND_SOC_DAPM_INPUT("MIC2"),
SND_SOC_DAPM_INPUT("LINE1L"),
SND_SOC_DAPM_INPUT("LINE2L"),
SND_SOC_DAPM_INPUT("LINE1R"),
SND_SOC_DAPM_INPUT("LINE2R"),


SND_SOC_DAPM_PGA("Mic1 Boost", RT5633_PWR_MANAG_ADD2, 5, 0, NULL, 0),  
SND_SOC_DAPM_PGA("Mic2 Boost", RT5633_PWR_MANAG_ADD2, 4, 0, NULL, 0), 

SND_SOC_DAPM_PGA("LINE1L Inp Vol", RT5633_PWR_MANAG_ADD2, 9, 0, NULL, 0),
SND_SOC_DAPM_PGA("LINE1R Inp Vol", RT5633_PWR_MANAG_ADD2, 8, 0, NULL, 0),
SND_SOC_DAPM_PGA("LINE2L Inp Vol", RT5633_PWR_MANAG_ADD2, 7, 0, NULL, 0),
SND_SOC_DAPM_PGA("LINE2R Inp Vol", RT5633_PWR_MANAG_ADD2, 6, 0, NULL, 0),



SND_SOC_DAPM_MIXER("RECMIXL Mixer", RT5633_PWR_MANAG_ADD2, 11, 0, 
	&rt5633_recmixl_mixer_controls[0], ARRAY_SIZE(rt5633_recmixl_mixer_controls)),
SND_SOC_DAPM_MIXER("RECMIXR Mixer", RT5633_PWR_MANAG_ADD2, 10, 0, 
	&rt5633_recmixr_mixer_controls[0], ARRAY_SIZE(rt5633_recmixr_mixer_controls)),


SND_SOC_DAPM_ADC_E("Left ADC","Left ADC HIFI Capture", RT5633_PWR_MANAG_ADD1,12, 0,
		adc_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
SND_SOC_DAPM_ADC_E("Right ADC","Right ADC HIFI Capture", RT5633_PWR_MANAG_ADD1,11, 0,
		adc_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),		

SND_SOC_DAPM_DAC_E("Left DAC", "Left DAC HIFI Playback", RT5633_PWR_MANAG_ADD1, 10, 0,
		dac_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
SND_SOC_DAPM_DAC_E("Right DAC", "Right DAC HIFI Playback", RT5633_PWR_MANAG_ADD1, 9, 0,
		dac_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),

SND_SOC_DAPM_MIXER("HPMIXL Mixer", RT5633_PWR_MANAG_ADD2, 15, 0, 
	&rt5633_hp_mixl_mixer_controls[0], ARRAY_SIZE(rt5633_hp_mixl_mixer_controls)),
SND_SOC_DAPM_MIXER("HPMIXR Mixer", RT5633_PWR_MANAG_ADD2, 14, 0, 
	&rt5633_hp_mixr_mixer_controls[0], ARRAY_SIZE(rt5633_hp_mixr_mixer_controls)),
SND_SOC_DAPM_MIXER("AUXMIXL Mixer", RT5633_PWR_MANAG_ADD2, 13, 0, 
	&rt5633_auxmixl_mixer_controls[0], ARRAY_SIZE(rt5633_auxmixl_mixer_controls)),
SND_SOC_DAPM_MIXER("AUXMIXR Mixer", RT5633_PWR_MANAG_ADD2, 12, 0, 
	&rt5633_auxmixr_mixer_controls[0], ARRAY_SIZE(rt5633_auxmixr_mixer_controls)),
SND_SOC_DAPM_MIXER("SPXMIX Mixer", RT5633_PWR_MANAG_ADD2, 0, 0, 
	&rt5633_spkmixr_mixer_controls[0], ARRAY_SIZE(rt5633_spkmixr_mixer_controls)),

SND_SOC_DAPM_PGA_E("Left SPK Vol", RT5633_PWR_MANAG_ADD4, 15, 0, NULL, 0,
		spk_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
SND_SOC_DAPM_PGA_E("Right SPK Vol", RT5633_PWR_MANAG_ADD4, 14, 0, NULL, 0,
		spk_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),

SND_SOC_DAPM_PGA_E("Left HP Vol", RT5633_PWR_MANAG_ADD4, 11, 0, NULL, 0,
		hp_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
SND_SOC_DAPM_PGA_E("Right HP Vol", RT5633_PWR_MANAG_ADD4, 10, 0, NULL, 0,
		hp_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
SND_SOC_DAPM_PGA_E("Left AUX Out Vol", RT5633_PWR_MANAG_ADD4, 9, 0, NULL, 0,
		auxout_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
SND_SOC_DAPM_PGA_E("Right AUX Out Vol", RT5633_PWR_MANAG_ADD4, 8, 0, NULL, 0,
		auxout_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),

SND_SOC_DAPM_MUX("SPKO Mux", SND_SOC_NOPM, 0, 0, &rt5633_spo_mux_control),

SND_SOC_DAPM_MICBIAS("Mic Bias1", RT5633_PWR_MANAG_ADD2, 3, 0),
SND_SOC_DAPM_MICBIAS("Mic Bias2", RT5633_PWR_MANAG_ADD2, 2, 0),

SND_SOC_DAPM_OUTPUT("AUXOUTL"),
SND_SOC_DAPM_OUTPUT("AUXOUTR"),
SND_SOC_DAPM_OUTPUT("SPOL"),
SND_SOC_DAPM_OUTPUT("SPOR"),
SND_SOC_DAPM_OUTPUT("HPOL"),
SND_SOC_DAPM_OUTPUT("HPOR"),

};


static const struct snd_soc_dapm_route audio_map[] = {

	{"Mic1 Boost", NULL, "MIC1"},
	{"Mic2 Boost", NULL, "MIC2"},
	
	
	{"LINE1L Inp Vol", NULL, "LINE1L"},
	{"LINE1R Inp Vol", NULL, "LINE1R"},

	{"LINE2L Inp Vol", NULL, "LINE2L"},
	{"LINE2R Inp Vol", NULL, "LINE2R"},


	{"RECMIXL Mixer", "HPMIXL Capture Switch", "HPMIXL Mixer"},
	{"RECMIXL Mixer", "AUXMIXL Capture Switch", "AUXMIXL Mixer"},
	{"RECMIXL Mixer", "SPKMIX Capture Switch", "SPXMIX Mixer"},
	{"RECMIXL Mixer", "LINE1L Capture Switch", "LINE1L Inp Vol"},
	{"RECMIXL Mixer", "LINE2L Capture Switch", "LINE2L Inp Vol"},
	{"RECMIXL Mixer", "MIC1 Capture Switch", "Mic1 Boost"},
	{"RECMIXL Mixer", "MIC2 Capture Switch", "Mic2 Boost"},

	{"RECMIXR Mixer", "HPMIXR Capture Switch", "HPMIXR Mixer"},
	{"RECMIXR Mixer", "AUXMIXR Capture Switch", "AUXMIXR Mixer"},
	{"RECMIXR Mixer", "SPKMIX Capture Switch", "SPXMIX Mixer"},
	{"RECMIXR Mixer", "LINE1R Capture Switch", "LINE1R Inp Vol"},
	{"RECMIXR Mixer", "LINE2R Capture Switch", "LINE2R Inp Vol"},
	{"RECMIXR Mixer", "MIC1 Capture Switch", "Mic1 Boost"},
	{"RECMIXR Mixer", "MIC2 Capture Switch", "Mic2 Boost"},

	{"Left ADC", NULL, "RECMIXL Mixer"},
	{"Right ADC", NULL, "RECMIXR Mixer"},

	{"HPMIXL Mixer", "RECMIXL Playback Switch", "RECMIXL Mixer"},
	{"HPMIXL Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"HPMIXL Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"HPMIXL Mixer", "LINE1 Playback Switch", "LINE1L Inp Vol"},
	{"HPMIXL Mixer", "LINE2 Playback Switch", "LINE2L Inp Vol"},
	{"HPMIXL Mixer", "DAC Playback Switch", "Left DAC"},

	{"HPMIXR Mixer", "RECMIXR Playback Switch", "RECMIXR Mixer"},
	{"HPMIXR Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"HPMIXR Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"HPMIXR Mixer", "LINE1 Playback Switch", "LINE1R Inp Vol"},
	{"HPMIXR Mixer", "LINE2 Playback Switch", "LINE2R Inp Vol"},
	{"HPMIXR Mixer", "DAC Playback Switch", "Right DAC"},

	{"AUXMIXL Mixer", "RECMIXL Playback Switch", "RECMIXL Mixer"},
	{"AUXMIXL Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"AUXMIXL Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"AUXMIXL Mixer", "LINE1 Playback Switch", "LINE1L Inp Vol"},
	{"AUXMIXL Mixer", "LINE2 Playback Switch", "LINE2L Inp Vol"},
	{"AUXMIXL Mixer", "DAC Playback Switch", "Left DAC"},

	{"AUXMIXR Mixer", "RECMIXR Playback Switch", "RECMIXR Mixer"},
	{"AUXMIXR Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"AUXMIXR Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"AUXMIXR Mixer", "LINE1 Playback Switch", "LINE1R Inp Vol"},
	{"AUXMIXR Mixer", "LINE2 Playback Switch", "LINE2R Inp Vol"},
	{"AUXMIXR Mixer", "DAC Playback Switch", "Right DAC"},

	{"SPXMIX Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"SPXMIX Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"SPXMIX Mixer", "DACL Playback Switch", "Left DAC"},
	{"SPXMIX Mixer", "DACR Playback Switch", "Right DAC"},
	{"SPXMIX Mixer", "LINE1L Playback Switch", "LINE1L Inp Vol"},
	{"SPXMIX Mixer", "LINE1R Playback Switch", "LINE1R Inp Vol"},
	{"SPXMIX Mixer", "LINE2L Playback Switch", "LINE2L Inp Vol"},
	{"SPXMIX Mixer", "LINE2R Playback Switch", "LINE2R Inp Vol"},

	{"SPKO Mux", "HPMIX", "HPMIXL Mixer"},
	{"SPKO Mux", "SPKMIX", "SPXMIX Mixer"},	
	{"SPKO Mux", "AUXMIX", "AUXMIXL Mixer"},	

	{"Left SPK Vol",  NULL, "SPKO Mux"},
	{"Right SPK Vol",  NULL, "SPKO Mux"},
	
	{"Right HP Vol",  NULL, "HPMIXR Mixer"},
	{"Left HP Vol",  NULL, "HPMIXL Mixer"},
	
	{"Left AUX Out Vol",  NULL, "AUXMIXL Mixer"},
	{"Right AUX Out Vol",  NULL, "AUXMIXR Mixer"},
	
	{"AUXOUTL", NULL, "Left AUX Out Vol"},
	{"AUXOUTR", NULL, "Right AUX Out Vol"},
	{"SPOL", NULL, "Left SPK Vol"},
	{"SPOR", NULL, "Right SPK Vol"},
	{"HPOL", NULL, "Left HP Vol"},
	{"HPOR", NULL, "Right HP Vol"},


};

#if 0
static int rt5633_add_widgets(struct snd_soc_codec *codec)
{
	snd_soc_dapm_new_controls(codec, rt5633_dapm_widgets,
						ARRAY_SIZE(rt5633_dapm_widgets));
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));
//	snd_soc_dapm_new_widgets(codec);
	return 0;
}
#endif

struct _coeff_div{
	unsigned int mclk;       //pllout or MCLK
	unsigned int bclk;       //master mode
	unsigned int rate;
	unsigned int reg_val;
};
/*PLL divisors*/
struct _pll_div {
	u32 pll_in;
	u32 pll_out;
	u16 regvalue;
};

static const struct _pll_div codec_master_pll_div[] = {
	
	{  2048000,  8192000,	0x0ea0},		
	{  3686400,  8192000,	0x4e27},	
	{ 12000000,  8192000,	0x456b},   
	{ 13000000,  8192000,	0x495f},
	{ 13100000,	 8192000,	0x0320},	
	{  2048000,  11289600,	0xf637},
	{  3686400,  11289600,	0x2f22},	
	{ 12000000,  11289600,	0x3e2f},   
	{ 13000000,  11289600,	0x4d5b},
	{ 13100000,	 11289600,	0x363b},	
	{  2048000,  16384000,	0x1ea0},
	{  3686400,  16384000,	0x9e27},	
	{ 12000000,  16384000,	0x452b},   
	{ 13000000,  16384000,	0x542f},
	{ 13100000,	 16384000,	0x03a0},	
	{  2048000,  16934400,	0xe625},
	{  3686400,  16934400,	0x9126},	
	{ 12000000,  16934400,	0x4d2c},   
	{ 13000000,  16934400,	0x742f},
	{ 13100000,	 16934400,	0x3c27},			
	{  2048000,  22579200,	0x2aa0},
	{  3686400,  22579200,	0x2f20},	
	{ 12000000,  22579200,	0x7e2f},   
	{ 13000000,  22579200,	0x742f},
	{ 13100000,	 22579200,	0x3c27},		
	{  2048000,  24576000,	0x2ea0},
	{  3686400,  24576000,	0xee27},	
	{ 12000000,  24576000,	0x2915},   
	{ 13000000,  24576000,	0x772e},
	{ 13100000,	 24576000,	0x0d20},	
	{ 26000000,  24576000,	0x2027},
	{ 26000000,  22579200,	0x392f},
	{ 24576000,  22579200,	0x0921},
	{ 24576000,  24576000,	0x02a0},
};

static const struct _pll_div codec_slave_pll_div[] = {
	
	{  1024000,  16384000,  0x3ea0},	
	{  1411200,  22579200,	0x3ea0},
	{  1536000,	 24576000,	0x3ea0},	
	{  2048000,  16384000,  0x1ea0},	
	{  2822400,  22579200,	0x1ea0},
	{  3072000,	 24576000,	0x1ea0},
	{	705600,  11289600, 	0x3ea0},
	{	705600,   8467200, 	0x3ab0},
			
};

struct _coeff_div coeff_div[] = {

	//sysclk is 256fs
	{ 2048000,  8000 * 32,  8000, 0x1000},
	{ 2048000,  8000 * 64,  8000, 0x0000},
	{ 2822400, 11025 * 32, 11025, 0x1000},
	{ 2822400, 11025 * 64, 11025, 0x0000},
	{ 4096000, 16000 * 32, 16000, 0x1000},
	{ 4096000, 16000 * 64, 16000, 0x0000},
	{ 5644800, 22050 * 32, 22050, 0x1000},
	{ 5644800, 22050 * 64, 22050, 0x0000},
	{ 8192000, 32000 * 32, 32000, 0x1000},
	{ 8192000, 32000 * 64, 32000, 0x0000},
	{11289600, 44100 * 32, 44100, 0x1000},
	{11289600, 44100 * 64, 44100, 0x0000},
	{12288000, 48000 * 32, 48000, 0x1000},
	{12288000, 48000 * 64, 48000, 0x0000},
	//sysclk is 512fs
	{ 4096000,  8000 * 32,  8000, 0x3000},
	{ 4096000,  8000 * 64,  8000, 0x2000},
	{ 5644800, 11025 * 32, 11025, 0x3000},
	{ 5644800, 11025 * 64, 11025, 0x2000},
	{ 8192000, 16000 * 32, 16000, 0x3000},
	{ 8192000, 16000 * 64, 16000, 0x2000},
	{11289600, 22050 * 32, 22050, 0x3000},
	{11289600, 22050 * 64, 22050, 0x2000},
	{16384000, 32000 * 32, 32000, 0x3000},
	{16384000, 32000 * 64, 32000, 0x2000},
	{22579200, 44100 * 32, 44100, 0x3000},
	{22579200, 44100 * 64, 44100, 0x2000},
	{24576000, 48000 * 32, 48000, 0x3000},
	{24576000, 48000 * 64, 48000, 0x2000},
	//SYSCLK is 22.5792Mhz or 24.576Mhz(8k to 48k)	
	{24576000, 48000 * 32, 48000, 0x3000},
	{24576000, 48000 * 64, 48000, 0x2000},
	{22579200, 44100 * 32, 44100, 0x3000},
	{22579200, 44100 * 64, 44100, 0x2000},
	{24576000, 32000 * 32, 32000, 0x1080},
	{24576000, 32000 * 64, 32000, 0x0080},
	{22579200, 22050 * 32, 22050, 0x5000},
	{22579200, 22050 * 64, 22050, 0x4000},	
	{24576000, 16000 * 32, 16000, 0x3080},
	{24576000, 16000 * 64, 16000, 0x2080},	
	{22579200, 11025 * 32, 11025, 0x7000},
	{22579200, 11025 * 64, 11025, 0x6000},	
	{24576000, 	8000 * 32, 	8000, 0x7080},
	{24576000, 	8000 * 64, 	8000, 0x6080},	

};


static int get_coeff(int mclk, int rate, int timesofbclk)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if ((coeff_div[i].mclk == mclk) 
				&& (coeff_div[i].rate == rate)
				&& ((coeff_div[i].bclk / coeff_div[i].rate) == timesofbclk))
				return i;
	}

		return -1;
}

static int get_coeff_in_slave_mode(int mclk, int rate)
{
	return get_coeff(mclk, rate, BLCK_FREQ);
}

static int get_coeff_in_master_mode(int mclk, int rate)
{
	return get_coeff(mclk, rate ,BLCK_FREQ);	
}

static int reg_addr_list[] = 
{
0X00, 0X02, 0X03, 0X04, 0X06, 0X08, 0X0A, 0X0C, 0X0E, 0X10, 0X12, 0X14, 0X16, 0X18, 0X1A, 
0X1C, 0X1E, 0X22, 0X34, 0X38, 0X3A, 0X3B, 0X3C, 0X3E, 0X40, 0X42, 0X44, 0X48, 0X4A, 0X4C, 
0X4D, 0X52, 0X54, 0X56, 0X5A, 0X5C, 0X5D, 0X64, 0X65, 0X66, 0X68, 0X6A, 0X6C, 0X6E, 0X70, 
0x7A, 0x7C, 0x7E, 
};


static int rt5633_reg_dump(struct snd_soc_codec *codec)
{
	int i, val;
	for (i = 0; i < ARRAY_SIZE(reg_addr_list); i ++) {
		val = rt5633_read(codec, reg_addr_list[i]);	
      	printk(KERN_INFO "%s reg=0x%x, val=0x%x\n", __func__, reg_addr_list[i], val); 
	}	
	
	return 0;
}

static int rt5633_hifi_pcm_params(struct snd_pcm_substream *substream, 
			struct snd_pcm_hw_params *params,
			struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct rt5633_priv *rt5633 = snd_soc_codec_get_drvdata(codec);
	unsigned int iface = 0;
	int rate = params_rate(params);
	int coeff = 0;

	printk(KERN_DEBUG "enter %s\n", __func__);	
	if (!rt5633->master)
		coeff = get_coeff_in_slave_mode(rt5633->sysclk, rate);
	else
		coeff = get_coeff_in_master_mode(rt5633->sysclk, rate);
	if (coeff < 0) {
		printk(KERN_ERR "%s get_coeff err!\n", __func__);
	//	return -EINVAL;
	}
	switch (params_format(params))
	{
		case SNDRV_PCM_FORMAT_S16_LE:
			break;
		case SNDRV_PCM_FORMAT_S20_3LE:
			iface |= 0x0004;
			break;
		case SNDRV_PCM_FORMAT_S24_LE:
			iface |= 0x0008;
			break;
		case SNDRV_PCM_FORMAT_S8:
			iface |= 0x000c;
			break;
		default:
			return -EINVAL;
	}
	
	rt5633_write_mask(codec, RT5633_SDP_CTRL, iface, SDP_I2S_DL_MASK);
	rt5633_write(codec, RT5633_STEREO_AD_DA_CLK_CTRL, coeff_div[coeff].reg_val);
	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD1, 0x81C0, 0x81C0);


//    rt5633_reg_dump(codec);

	return 0;
}

static int rt5633_hifi_codec_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct rt5633_priv *rt5633 =  snd_soc_codec_get_drvdata(codec);
	u16 iface = 0;
	
	printk(KERN_DEBUG "enter %s\n", __func__);	
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		rt5633->master = 1;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		iface |= (0x0001 << 15);
		rt5633->master = 0;
		break;
	default:
		return -EINVAL;
	}
	
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= (0x0001);
		break;
	case SND_SOC_DAIFMT_DSP_A:
		iface |= (0x0002);
		break;
	case SND_SOC_DAIFMT_DSP_B:
		iface  |= (0x0003);
		break;
	default:
		return -EINVAL;
	}
	
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= (0x0001 << 7);
		break;
	default:
		return -EINVAL;	
	}
	
	rt5633_write(codec, RT5633_SDP_CTRL, iface);
	return 0;
}
static int rt5633_hifi_codec_set_dai_sysclk(struct snd_soc_dai *codec_dai, 
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct rt5633_priv *rt5633 = snd_soc_codec_get_drvdata(codec);
	
	printk(KERN_DEBUG "enter %s\n", __func__);	
	if ((freq >= (256 * 8000)) && (freq <= (512 * 48000))) {
		rt5633->sysclk = freq;
		return 0;	
	}
	
	printk(KERN_ERR "unsupported sysclk freq %u for audio i2s\n", freq);
	return 0;
}

static int rt5633_codec_set_dai_pll(struct snd_soc_dai *codec_dai, 
		int pll_id,int source, unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = codec_dai->codec;	
	struct rt5633_priv *rt5633 = snd_soc_codec_get_drvdata(codec);
	int i;
	int ret = -EINVAL;
	
	
	printk(KERN_DEBUG "enter %s:   freq_in[%d]  freq_out[%d]\n", __func__, freq_in, freq_out);	
	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD2, 0, PWR_PLL);

	if (!freq_in || !freq_out)
		return 0;
		
	if (rt5633->master) {
		for (i = 0; i < ARRAY_SIZE(codec_master_pll_div); i ++) {
			if ((freq_in == codec_master_pll_div[i].pll_in) && (freq_out == codec_master_pll_div[i].pll_out)) {
				rt5633_write(codec, RT5633_PLL_CTRL, codec_master_pll_div[i].regvalue);
				rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD2, PWR_PLL, PWR_PLL);
				schedule_timeout_uninterruptible(msecs_to_jiffies(20));
				rt5633_write(codec, RT5633_GBL_CLK_CTRL, 0x0000);	
				ret = 0;
			}
		}	
	} else {
		for (i = 0; i < ARRAY_SIZE(codec_slave_pll_div); i ++) {
			if ((freq_in == codec_slave_pll_div[i].pll_in) && (freq_out == codec_slave_pll_div[i].pll_out))  {
				rt5633_write(codec, RT5633_PLL_CTRL, codec_slave_pll_div[i].regvalue);
				rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD2, PWR_PLL, PWR_PLL);
				schedule_timeout_uninterruptible(msecs_to_jiffies(20));
				rt5633_write(codec, RT5633_GBL_CLK_CTRL, 0x6000);
				ret = 0;
			}
		}
	}
	
	return 0;
}

static void rt5633_hifi_shutdown(struct snd_pcm_substream *substream,
		struct snd_soc_dai *codec_dai)
{

	
}

#define RT5633_STEREO_RATES		(SNDRV_PCM_RATE_8000_48000)

#define RT5633_FORMAT					(SNDRV_PCM_FMTBIT_S16_LE |\
			SNDRV_PCM_FMTBIT_S20_3LE |\
			SNDRV_PCM_FMTBIT_S24_LE |\
			SNDRV_PCM_FMTBIT_S8)
			
static struct snd_soc_dai_ops rt5633_ops = {
	.hw_params = rt5633_hifi_pcm_params,
	.set_fmt = rt5633_hifi_codec_set_dai_fmt,
	.set_sysclk = rt5633_hifi_codec_set_dai_sysclk,
	.set_pll = rt5633_codec_set_dai_pll,
	.shutdown = rt5633_hifi_shutdown,	
};

			
static struct snd_soc_dai_driver rt5633_dai[] = { 
	{
		.name = "rt5633-hifi",
		.id = 1,
		.playback = {
			.stream_name = "HIFI Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5633_STEREO_RATES,
			.formats = RT5633_FORMAT,	
		}	,
		.capture = {
			.stream_name = "HIFI Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5633_STEREO_RATES,	
			.formats = RT5633_FORMAT,	
		},

		.ops =&rt5633_ops,
	},
		
	{
		.name = "RT5633 Reserved",
		.id = 2,
	}
};

#if 0
static u32 StrtoInt(const char *str)
{
	u32 i,CodecValue=0;
		
	printk("strtoin=%s \n",str);
	if(!str)
	{
		printk("%s(): NULL pointer input\n", __FUNCTION__);
		return -1;
	}
	for(i=0; *str; str++)
	{
		if((*str>='A' && *str<='F')||(*str>='a' && *str<='f')||(*str>='0' && *str<='9'))
		{
			 CodecValue*=0x10;
			if(*str>='A' && *str<='F')
				CodecValue += *str-'A'+10;
			else if(*str>='a' && *str<='f')
				CodecValue += *str-'a'+10;
			else if(*str>='0' && *str<='9')
				CodecValue += *str-'0';				
		}
		else
				return CodecValue;
	}
	return CodecValue;
}


enum
{
	WRITE_REG=0,
	READ_REG,
	WRITE_INDEX,
	READ_INDEX,
	BAD_FORMAT,	
};

static u32 last_command=0;
static u32 read_codec_reg=0;
static u32 read_codec_value=0;

static ssize_t rt5633_codec_reg_show(struct device *dev,struct device_attribute *attr ,char *buf)
{
	if(last_command==READ_REG)
	{
		return sprintf(buf,"%04x\n",read_codec_value);
	}
	if(last_command==READ_INDEX)
	{
		return sprintf(buf,"%04x\n",read_codec_value);
	}	
	
	return sprintf(buf,"read fail\n");
}


static ssize_t rt5633_codec_reg_store(struct device *dev,struct device_attribute *attr, const char *buf,
 size_t count)
{
	const char * p=buf;
	u32 reg=0, val=0;
	printk("store buf=%s \n",buf);	

	if(!strncmp(buf, "readr", strlen("readr")))
	{
		p+=strlen("readr");
		read_codec_reg=(u32)StrtoInt(p);
		read_codec_value=rt5633_read(rt5633_codec, read_codec_reg);
		last_command=READ_REG;
		printk("%s(): get 0x%04x=0x%04x\n", __FUNCTION__, read_codec_reg, val);
	}
	else if(!strncmp(buf, "writer", strlen("writer")))
	{
		p+=strlen("writer");
		reg=(u32)StrtoInt(p);
		p=strchr(buf, '=');
		if(p)
		{
			++ p;
			val=(u32)StrtoInt(p);
			rt5633_write(rt5633_codec, reg, val);
			last_command=WRITE_REG;
			printk("%s(): set 0x%04x=0x%04x\n", __FUNCTION__, reg, val);
		}
		else
		{
			last_command=BAD_FORMAT;
			printk("%s(): Bad string format input!\n", __FUNCTION__);
		}	
	}
	else if(!strncmp(buf, "writei", strlen("writei")))
	{
		p+=strlen("writei");
		reg=(u32)StrtoInt(p);
		p=strchr(buf, '=');
		if(p)
		{
			++ p;
			val=(u32)StrtoInt(p);
			rt5633_write(rt5633_codec, 0x6a, reg);
			rt5633_write(rt5633_codec, 0x6c, val);
			last_command=WRITE_INDEX;
			printk("%s(): set 0x%04x=0x%04x\n", __FUNCTION__, reg, val);
		}
		else
		{
			last_command = BAD_FORMAT;
			printk("%s(): Bad string format input!\n", __FUNCTION__);
		}	
	}
	else if(!strncmp(buf, "readi", strlen("readi")))
	{
		p+=strlen("readi");
		read_codec_reg=(u32)StrtoInt(p);
		rt5633_write(rt5633_codec, 0x6a, read_codec_reg);	
		read_codec_value=rt5633_read(rt5633_codec, 0x6c);
		last_command=READ_INDEX;
		printk("%s(): get 0x%04x=0x%04x\n", __FUNCTION__, read_codec_reg, val);
	}
	else
	{
		last_command = BAD_FORMAT;
		printk("%s(): Bad string format input!\n", __FUNCTION__);
	}	
	
	return count;
} 

static DEVICE_ATTR(rt_codec_reg, 0666, rt5633_codec_reg_show, rt5633_codec_reg_store);
#endif

static int rt5633_set_bias_level(struct snd_soc_codec *codec, enum snd_soc_bias_level level)
{
	printk(KERN_DEBUG "enter %s\n", __func__);

	switch (level) {
	case SND_SOC_BIAS_ON:
	case SND_SOC_BIAS_PREPARE:
		rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD3,PWR_VREF|PWR_MAIN_BIAS, PWR_VREF|PWR_MAIN_BIAS);
		rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD2,0x0008, 0x0008);		
		break;
	case SND_SOC_BIAS_STANDBY:
		if (codec->dapm.bias_level == SND_SOC_BIAS_OFF) {
        	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD3,PWR_VREF|PWR_MAIN_BIAS, PWR_VREF|PWR_MAIN_BIAS);
        	//schedule_timeout_uninterruptible(msecs_to_jiffies(110));
			msleep(80);
        	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD3,PWR_DIS_FAST_VREF,PWR_DIS_FAST_VREF);

			codec->cache_only = false;
			snd_soc_cache_sync(codec);
		}
		break;
	case SND_SOC_BIAS_OFF:
		rt5633_write_mask(codec, RT5633_SPK_OUT_VOL, 0x8000, 0x8000);	//mute speaker volume
		rt5633_write_mask(codec, RT5633_HP_OUT_VOL, 0x8080, 0x8080);	//mute hp volume
		rt5633_write(codec, RT5633_PWR_MANAG_ADD1, 0x0000);
		rt5633_write(codec, RT5633_PWR_MANAG_ADD2, 0x0000);
		rt5633_write(codec, RT5633_PWR_MANAG_ADD3, 0x0000);
		rt5633_write(codec, RT5633_PWR_MANAG_ADD4, 0x0000);
		break;
	}
	
	codec->dapm.bias_level = level;
	return 0;
}


static int rt5633_probe(struct snd_soc_codec *codec) 
{
	struct rt5633_priv *rt5633 = snd_soc_codec_get_drvdata(codec);
	int ret = 0;
	
	printk(KERN_DEBUG "enter %s\n", __func__);	
	
	ret = snd_soc_codec_set_cache_io(codec, 8, 16, SND_SOC_I2C);
	if (ret != 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}

	//rt5633_register_init(codec);
	
	snd_soc_add_controls(codec, rt5633_snd_controls,
				ARRAY_SIZE(rt5633_snd_controls));


	rt5633_reset(codec);

	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD3,PWR_VREF|PWR_MAIN_BIAS, PWR_VREF|PWR_MAIN_BIAS);
	schedule_timeout_uninterruptible(msecs_to_jiffies(110));
	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD3,PWR_DIS_FAST_VREF,PWR_DIS_FAST_VREF);

	rt5633_reg_init(codec);


	codec->dapm.bias_level = SND_SOC_BIAS_STANDBY;
	return ret;
}

static int rt5633_remove(struct snd_soc_codec *codec)
{
	rt5633_set_bias_level(codec, SND_SOC_BIAS_OFF);
	snd_soc_dapm_free(&codec->dapm);

	return 0;
}

static int rt5633_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	printk(KERN_DEBUG "enter %s\n", __func__);	
	rt5633_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int rt5633_resume(struct snd_soc_codec *codec)
{
	printk(KERN_DEBUG "enter %s\n", __func__);	

	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD3,PWR_VREF|PWR_MAIN_BIAS, PWR_VREF|PWR_MAIN_BIAS);
	schedule_timeout_uninterruptible(msecs_to_jiffies(110));
	rt5633_reg_init(codec);
	rt5633_write_mask(codec, RT5633_PWR_MANAG_ADD1, 0x81C0, 0x81C0);

#if (RT5633_SPK_TIMER == 1)
		last_is_spk = !last_is_spk;	//wired~, update eqmode right here by spk_timer.
#endif	
	
	if (codec->dapm.suspend_bias_level == SND_SOC_BIAS_ON) {
		rt5633_set_bias_level(codec, SND_SOC_BIAS_PREPARE);
		codec->dapm.bias_level = SND_SOC_BIAS_ON;
//		schedule_delayed_work(&codec->delayed_work, msecs_to_jiffies(100));	
	}

	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_rt5633 = 
{
	.probe   = rt5633_probe,
	.remove  = rt5633_remove,
	.suspend = rt5633_suspend,
	.resume  = rt5633_resume,
	
	.set_bias_level = rt5633_set_bias_level,
	.reg_cache_size = ARRAY_SIZE(rt5633_reg),
	.reg_word_size  = sizeof(u16),
	.reg_cache_default = rt5633_reg,
	.dapm_widgets      = rt5633_dapm_widgets,
	.num_dapm_widgets  = ARRAY_SIZE(rt5633_dapm_widgets),
	.dapm_routes       = audio_map,
	.num_dapm_routes   = ARRAY_SIZE(audio_map),
};


static int rt5633_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct rt5633_priv *rt5633;
	int ret;

	printk(KERN_DEBUG "enter %s\n", __func__);	

	rt5633 = kzalloc(sizeof(struct rt5633_priv), GFP_KERNEL);
	if (rt5633 == NULL)
		return -ENOMEM;

	i2c_set_clientdata(i2c, rt5633);
	rt5633->control_type = SND_SOC_I2C;

	ret = snd_soc_register_codec(&i2c->dev,
			&soc_codec_dev_rt5633, &rt5633_dai, ARRAY_SIZE(rt5633_dai));
	if (ret < 0) {
		kfree(rt5633);
		printk("failed to initialise rt5633!\n");
	}

	return ret;
}

static int rt5633_i2c_remove(struct i2c_client *client)
{
	snd_soc_unregister_codec(&client->dev);
	kfree(i2c_get_clientdata(client));
	return 0;
}

static void rt5633_i2c_shutdown(struct i2c_client *client)
{
}

static const struct i2c_device_id rt5633_i2c_id[] = {
	{"rt5633", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, rt5633_i2c_id);

struct i2c_driver rt5633_i2c_driver = {
	.driver = {
		.name = "rt5633 I2C Codec",
		.owner = THIS_MODULE,
	},
	.probe = rt5633_i2c_probe,
	.remove = rt5633_i2c_remove,
	.shutdown = rt5633_i2c_shutdown,	
	.id_table = rt5633_i2c_id,
};


static int rt5633_add_i2c_device(struct platform_device *pdev, 
		const struct rt5633_setup_data *setup)
{
#if 1	//0
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client;
#endif
	int ret;
	
	printk(KERN_DEBUG "enter %s\n", __func__);	
	ret = i2c_add_driver(&rt5633_i2c_driver);
	if (ret != 0) {
		dev_err(&pdev->dev, "can't add i2c driver\n");
		return ret;	
	}	

#if 1	//0
	memset(&info, '\0', sizeof(struct i2c_board_info));
	info.addr = setup->i2c_address;
	info.platform_data = pdev;
	strlcpy(info.type, "rt5633", I2C_NAME_SIZE);
	
	adapter = i2c_get_adapter(setup->i2c_bus);
	if (!adapter) {
		dev_err(&pdev->dev, "can't get i2c adapter %d\n", 
				setup->i2c_bus);
		goto err_driver;	
	}
	
	client = i2c_new_device(adapter, &info);
	i2c_put_adapter(adapter);
	if (!client) {
		dev_err(&pdev->dev, "can't add i2c device at 0x%x\n",
				(unsigned int)info.addr);
		goto err_driver;
	}
#endif

	return 0;
#if 1	//0
err_driver:
	i2c_del_driver(&rt5633_i2c_driver);
	return -ENODEV;
#endif
}


static int __init rt5633_modinit(void)
{
	int ret;
	
	printk(KERN_DEBUG "enter %s\n", __func__);	
	ret = i2c_add_driver(&rt5633_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register RT5633 I2C driver: %d\n", ret);
	}
	return ret;
}

static void __exit rt5633_modexit(void) 
{
	printk(KERN_DEBUG "enter %s\n", __func__);	
	i2c_del_driver(&rt5633_i2c_driver);
}

module_init(rt5633_modinit);
module_exit(rt5633_modexit);
MODULE_LICENSE("GPL");
