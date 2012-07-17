/*
 * linux/sound/soc/tcc/tcc_board_wm8994.c
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
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
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
#include "../codecs/wm8994.h"

#include "tcc-pcm.h"
#include "tcc-i2s.h"

#define TCC_HP        0
#define TCC_MIC       1
#define TCC_LINE      2
#define TCC_HEADSET   3
#define TCC_HP_OFF    4

#define TCC_CODEC_ROUTE_0   0
#define TCC_CODEC_ROUTE_1   1
#define TCC_CODEC_ROUTE_2   2
#define TCC_CODEC_ROUTE_3   3
#define TCC_CODEC_ROUTE_4   4
#define TCC_CODEC_ROUTE_5   5
#define TCC_CODEC_ROUTE_6   6
#define TCC_CODEC_ROUTE_7   7
#define TCC_CODEC_ROUTE_8   8
#define TCC_CODEC_ROUTE_9   9
#define TCC_CODEC_ROUTE_10  10

// /* audio clock in Hz - rounded from 12.235MHz */
//#define TCC83X_AUDIO_CLOCK 12288000


/********************************************************************************
 * WM8994 Audio Path Route
 ********************************************************************************/
static int tcc_jack_func = TCC_HP_OFF;
static int tcc_spk_func  = TCC_CODEC_ROUTE_0;

/**********************************************************************
 * Ear Mode
 *      - AIF2 -> DAC -> HPOUT1 , MIC -> ADC -> AIF2
 **********************************************************************/
static void tcc_codec_jack_hp(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() \n", __func__);

// Power up Init
    snd_soc_write(codec, 0x1C, 0x012D);     // HP Mute
    snd_soc_write(codec, 0x1D, 0x012D);     // HP Mute

// Power - UP sequence
    snd_soc_write(codec, 0x39, 0x006C);     // AntiPOP(2)       VMID_RAMP(11) VMID_BUF_ENA(1) START_BIAS_ENA(1)
    mdelay(5);
    snd_soc_write(codec, 0x01, 0x0013);     // PowerManagement1     MICBIAS1_ENA(1) VMID_SEL(01) BIAS_ENA(1)
    msleep(50);
    snd_soc_write(codec, 0x4C, 0x9F25);     // Charge Pump(1)
    mdelay(5);
    snd_soc_write(codec, 0x01, 0x0313);     // PowerManagement1     HPOUT1L_ENA(1) HPOUT1R_ENA(1) MICBIAS1_ENA(1) VMID_SEL(01) BIAS_ENA(1)
    mdelay(5);
    snd_soc_write(codec, 0x60, 0x0022);     // Analogue HP      HPOUT1L_DLY(1) HOPUT1R_DLY(1)
    msleep(300);
    snd_soc_write(codec, 0x54, 0x0003);     // DC Servo(1)      DCS_ENA_CHAN_1(1) DCS_ENA_CHAN_0(0)
    msleep(10);

    snd_soc_write(codec, 0x60, 0x00EE);     // Analogue HP      
// Digital Path Enables and Unmutes
    snd_soc_write(codec, 0x05, 0x3003);     // PowerManagement5     Enable DAC1 (Left), Enable DAC1 (Right), Enable AIF2 Timeslot 0 DAC1 (Left) Path, Enable AIF2 Timeslot 0 DAC1 (Right) Path
    snd_soc_write(codec, 0x520, 0x0000);    // Unmute the AIF2 Timeslot 0 DAC path
    snd_soc_write(codec, 0x601, 0x0005);    // Enable the AIF2 (Left) to DAC 1 (Left) mixer path
    snd_soc_write(codec, 0x602, 0x0005);    // Enable the AIF2 (Right) to DAC 1 (Right) mixer path
    snd_soc_write(codec, 0x610, 0x00C0);    // Unmute DAC 1 (Left)
    snd_soc_write(codec, 0x611, 0x01C0);    // Unmute DAC 1 (Right)
// Clocking
    snd_soc_write(codec, 0x200, 0x0001);    // Enable AIF1 clock, AIF1 clock = MCLK1
    snd_soc_write(codec, 0x204, 0x0001);    // Enable AIF2 clock, AIF2 clock = MCLK1
    snd_soc_write(codec, 0x208, 0x000E);    // Enable the DSP processing core for AIF1, Enable the DSP processing core for AIF2, Enable the core clock
    snd_soc_write(codec, 0x210, 0x0008);    // Set the AIF1 sample rate to 8 kHz, AIF1CLK = 1408 Fs
    snd_soc_write(codec, 0x211, 0x0073);    // Set the AIF2 sample rate to 44.1 kHz, AIF2CLK = 256 Fs
// AIF2 Interface
    snd_soc_write(codec, 0x310, 0x4010);    // AIF2 Interface = I2S, 16bits
// AIF1 Interface
    snd_soc_write(codec, 0x300, 0x4050);    // AIF1 Interface = I2S, 24bits
// GPIO setting
    snd_soc_write(codec, 0x700, 0xA101);    // GPIO1/ADCLRCLK1  Input, Pull-Down, Button detect input
    snd_soc_write(codec, 0x701, 0x8100);    // GPIO2/MCLK2      Input, Input De-bounce, MCLK2
    snd_soc_write(codec, 0x702, 0x8100);    // GPIO3/BCLK2      Input, Input De-bounce, BCLK2
    snd_soc_write(codec, 0x703, 0x8100);    // GPIO4/LRCLK2     Input, Input De-bounce, LRCLK2
    snd_soc_write(codec, 0x704, 0x8100);    // GPIO5/DACDAT2    Input, Input De-bounce, DACDAT2
    snd_soc_write(codec, 0x705, 0xA101);    // GPIO6/ADCLRCLK   Input, Pull-Down, Button detect input
    snd_soc_write(codec, 0x706, 0x0100);    // GPIO7/ADCDAT2    Output, Input De-bounce, ADCDAT2

// Analogue Input Configuration
    snd_soc_write(codec, 0x02, 0x6240);     // Enable Left Input Mixer (MIXINL), Enable IN1L PGA
    snd_soc_write(codec, 0x18, 0x011F);     // Unmute IN1L PGA, +30dB
    snd_soc_write(codec, 0x28, 0x0030);     // Connect IN1LN to IN1L PGA inverting input, Connect IN1LP to IN1L PGA input
    snd_soc_write(codec, 0x29, 0x0020);     // Unmute IN1L PGA output to Left Input Mixer (MIXINL) path
// Analogue Output Configuration
    snd_soc_write(codec, 0x03, 0x0030);     // PowerManagement3 MIXOUTL_ENA(1) MIXOUTR_ENA(1)
    snd_soc_write(codec, 0x2D, 0x0001);     // MIXOUTL      Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path
    snd_soc_write(codec, 0x2E, 0x0001);     // MIXOUTR      Select DAC1 (Right) to Left Headphone Output PGA (HPOUT1RVOL) path
    mdelay(5);
    snd_soc_write(codec, 0x1C, 0x0073);     // HP Un-Mute, 0x3F(+6dB), 0x3C(+3dB), 0x39(0dB), 0x36(-3dB), 0x33(-6dB)
    snd_soc_write(codec, 0x1D, 0x0173);     // HP Un-Mute, 0x3F(+6dB), 0x3C(+3dB), 0x39(0dB), 0x36(-3dB), 0x33(-6dB)
    mdelay(5);
//    snd_soc_write(codec, 0x39, 0x0000);     // WR

}


/**********************************************************************
 * Music Mode (Spekaer)
 *      - AIF2 -> DAC -> SPK, MIC -> ADC -> AIF2
 **********************************************************************/
static void tcc_codec_route_0(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() Play the Music (AIF2 -> Speaker)\n", __func__);

// Power up Init
    snd_soc_write(codec, 0x39, 0x006C);     // WR Soft fast start, Start-Up bias current generator, Start-Up bias
    mdelay(5);
    snd_soc_write(codec, 0x01, 0x0013);     // 2 x 40kohm divider(for normal operation), Bias enable, Enable Microphone Bias 1
    mdelay(50);
    snd_soc_write(codec, 0x01, 0x1013);     // SpeakerMixerLeft Enable

// Analogue Input Configuration
    snd_soc_write(codec, 0x02, 0x6240);     // Enable Left Input Mixer (MIXINL), Enable IN1L PGA
    snd_soc_write(codec, 0x18, 0x011F);     // Unmute IN1L PGA, +30dB
    snd_soc_write(codec, 0x28, 0x0030);     // Connect IN1LN to IN1L PGA inverting input, Connect IN1LP to IN1L PGA input
    snd_soc_write(codec, 0x29, 0x0020);     // Unmute IN1L PGA output to Left Input Mixer (MIXINL) path
// Analogue Output Configuration
    snd_soc_write(codec, 0x03, 0x0300);     // Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL
    snd_soc_write(codec, 0x22, 0x0000);     // Speaker output, Left Speaker Mixer Volume = 0dB
    snd_soc_write(codec, 0x23, 0x0000);     // Speaker output mode = Class D, Right Speaker Mixer Volume = 0dB
    snd_soc_write(codec, 0x25, 0x0168);     // SPK BOOST
    snd_soc_write(codec, 0x36, 0x0003);     // Unmute DAC1 (Left) to Left Speaker Mixer (SPKMIXL) path, Unmute DAC1 (Right) to Right Speaker Mixer (SPKMIXR) path
// Digital Path Enables and Unmutes
    snd_soc_write(codec, 0x04, 0x2002);     // Enable ADC (Left), Enable AIF2 ADC (Left) Path
    snd_soc_write(codec, 0x05, 0x3303);     // Enable DAC1 (Left), Enable DAC1 (Right), Enable AIF1 Timeslot 0 DAC1 (Left) Path, Enable AIF1 Timeslot 0 DAC1 (Right) Path, Enable AIF2 DAC1 (Left) Path, Enable AIF2 DAC1 (Right) Path
    snd_soc_write(codec, 0x520, 0x0000);    // Unmute the AIF2 DAC path
    snd_soc_write(codec, 0x601, 0x0004);    // Enable the AIF2 (Left) to DAC 1 (Left) mixer path
    snd_soc_write(codec, 0x602, 0x0004);    // Enable the AIF2 (Right) to DAC 1 (Right) mixer path
    snd_soc_write(codec, 0x603, 0x000C);    // Set ADC (Left) to DAC 2 sidetone path volume to 0dB
    snd_soc_write(codec, 0x604, 0x0010);    // Enable the ADC (Left) to DAC 2 (Left) mixer path
    snd_soc_write(codec, 0x610, 0x00C0);    // Unmute DAC 1 (Left)
    snd_soc_write(codec, 0x611, 0x01C0);    // Unmute DAC 1 (Right)
    snd_soc_write(codec, 0x612, 0x01C0);    // Unmute DAC 2 (Left)
    snd_soc_write(codec, 0x613, 0x01C0);    // Unmute DAC 2 (Right)
    snd_soc_write(codec, 0x620, 0x0000);    // ADC OSR Set 0
    snd_soc_write(codec, 0x621, 0x01C0);    // Sidetone HPF Enable LADC Select
// Clocking setting
    snd_soc_write(codec, 0x200, 0x0001);    // Enable AIF1 clock, AIF1 clock = MCLK1
    snd_soc_write(codec, 0x204, 0x0001);    // Enable AIF2 clock, AIF2 clock = MCLK1
    snd_soc_write(codec, 0x208, 0x000E);    // Enable the DSP processing core for AIF1, Enable the DSP processing core for AIF2, Enable the core clock
    snd_soc_write(codec, 0x210, 0x0008);    // Set the AIF1 sample rate to 8 kHz, AIF1CLK = 1408 Fs
    snd_soc_write(codec, 0x211, 0x0073);    // Set the AIF2 sample rate to 44.1 kHz, AIF2CLK = 256 Fs
// AIF2 Interface
    snd_soc_write(codec, 0x310, 0x4010);    // AIF2 Interface = I2S, 16bits
// AIF1 Interface
    snd_soc_write(codec, 0x300, 0x4050);    // AIF1 Interface = I2S, 24bits
// GPIO setting
    snd_soc_write(codec, 0x700, 0xA101);    // GPIO1/ADCLRCLK1  Input, Pull-Down, Button detect input
    snd_soc_write(codec, 0x701, 0x8100);    // GPIO2/MCLK2      Input, Input De-bounce, MCLK2
    snd_soc_write(codec, 0x702, 0x8100);    // GPIO3/BCLK2      Input, Input De-bounce, BCLK2
    snd_soc_write(codec, 0x703, 0x8100);    // GPIO4/LRCLK2     Input, Input De-bounce, LRCLK2
    snd_soc_write(codec, 0x704, 0x8100);    // GPIO5/DACDAT2    Input, Input De-bounce, DACDAT2
    snd_soc_write(codec, 0x705, 0xA101);    // GPIO6/ADCLRCLK   Input, Pull-Down, Button detect input
    snd_soc_write(codec, 0x706, 0x0100);    // GPIO7/ADCDAT2    Output, Input De-bounce, ADCDAT2

    mdelay(5);
    snd_soc_write(codec, 0x22, 0x0000);     // Speaker Out unmute, Speaker output mode = Class D

}

/**********************************************************************
 * Call Mode (Speaker)
 *      - AIF1-> DAC -> SPKOUT , MIC -> ADC -> AIF1
 **********************************************************************/
static void tcc_codec_route_1(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() Call (AIF1 -> Speaker)\n", __func__);

// Power up Init.
    snd_soc_write(codec, 0x039, 0x006C);    // WR
    mdelay(5);
    snd_soc_write(codec, 0x001, 0x0013);    // WR
    msleep(50);
    snd_soc_write(codec, 0x001, 0x1013);    // WR

// Analogue Input Configuration
    snd_soc_write(codec, 0x002, 0x6240);    // Enable Left Input Mixer (MIXINL), Enable IN1L PGA
    snd_soc_write(codec, 0x018, 0x011F);    // Unmute IN1L PGA, +30dB
    snd_soc_write(codec, 0x028, 0x0030);    // Connect IN1LN to IN1L PGA inverting input, Connect IN1LP to IN1L PGA input
    snd_soc_write(codec, 0x029, 0x0020);    // Unmute IN1L PGA output to Left Input Mixer (MIXINL) path
// Analogue Output Configuration
    snd_soc_write(codec, 0x003, 0x0300);    // Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL
    snd_soc_write(codec, 0x022, 0x0000);    // Speaker output, Left Speaker Mixer Volume = 0dB
    snd_soc_write(codec, 0x023, 0x0103);    // Speaker output mode = Class AB, Right Speaker Mixer Volume = 0dB
    snd_soc_write(codec, 0x025, 0x0168);    // SPK BOOST
    snd_soc_write(codec, 0x036, 0x0003);    // Unmute DAC1 (Left) to Left Speaker Mixer (SPKMIXL) path, Unmute DAC1 (Right) to Right Speaker Mixer (SPKMIXR) path
// Digital Path Enables and Unmutes
    snd_soc_write(codec, 0x004, 0x0202);    // Enable ADC (Left), Enable AIF1 ADC (Left) Path
    snd_soc_write(codec, 0x005, 0x0303);    // Enable DAC1 (Left), Enable DAC1 (Right), Enable AIF1 Timeslot 0 DAC1 (Left) Path, Enable AIF1 Timeslot1 DAC1 (Right) Path
    snd_soc_write(codec, 0x420, 0x0000);    // Unmute the AIF1 Timeslot 0 DAC path
    snd_soc_write(codec, 0x601, 0x0001);    // Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path
    snd_soc_write(codec, 0x602, 0x0001);    // Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path
    snd_soc_write(codec, 0x606, 0x0002);    // Enable the ADC (Left) to AIF1 Timeslot 0 (Left) mixer path
    snd_soc_write(codec, 0x610, 0x00C0);    // Unmute DAC 1 (Left)
    snd_soc_write(codec, 0x611, 0x01C0);    // Unmute DAC 1 (Right)
// Clocking
    snd_soc_write(codec, 0x200, 0x0001);    // Enable AIF1 Clock, AIF1 Clock Source = MCLK1
    snd_soc_write(codec, 0x204, 0x0001);    // Enable AIF2 Clock, AIF2 Clock Source = MCLK1
    snd_soc_write(codec, 0x208, 0x000F);    // Enable the DSP processing core for AIF1, Enable the DSP processing core for AIF2, Enable the core Clock(SYSCLK=AIF2CLK)
    snd_soc_write(codec, 0x210, 0x0008);    // Set the AIF1 sample rate to 8kHz, AIF1CLK = 1408 Fs
    snd_soc_write(codec, 0x211, 0x0073);    // Set the AIF2 sample rate to 44.1kHz, AIF2CLK = 256 Fs
// AIF1 Interface
    snd_soc_write(codec, 0x300, 0x4050);    // AIF1 Interface = I2S, 24bits
// AIF2 Interface
    snd_soc_write(codec, 0x310, 0x4010);    // AIF2 Interface = I2S, 16bits
// GPIO setting
    snd_soc_write(codec, 0x700, 0xA101);    // GPIO1
    snd_soc_write(codec, 0x701, 0x8100);    // GPIO2 - Set to MCLK2 input
    snd_soc_write(codec, 0x702, 0x8100);    // GPIO3 - Set to BCLK2 input
    snd_soc_write(codec, 0x703, 0x8100);    // GPIO4 - Set to DACLRCLK2 input
    snd_soc_write(codec, 0x704, 0x8100);    // GPIO5 - Set to DACDAT2 input
    snd_soc_write(codec, 0x705, 0xA101);    // GPIO6 - Set to GPIO2
    snd_soc_write(codec, 0x706, 0x0100);    // GPIO7 - Set to ADCDAT2 output

    mdelay(5);
// SPK Unmute
    snd_soc_write(codec, 0x022, 0x0000);    // Speaker output mode = Class D
}

/**********************************************************************
 * Call Mode (Earpiece)
 *      - AIF1-> DAC -> HPOUT2 , MIC -> ADC -> AIF1
 **********************************************************************/
static void tcc_codec_route_2(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() Call (AIF1 -> Earpiece)\n", __func__);

// Power up Init.
    snd_soc_write(codec, 0x039, 0x006C);    // WR
    mdelay(5);
    snd_soc_write(codec, 0x001, 0x0033);    // WR
    msleep(50);
    snd_soc_write(codec, 0x001, 0x0833);    // Enable bias generator, Enable VMID, Enable MICBIAS 1, Enable HPOUT2 (Earpiece)

// Analogue Input Configuration
    snd_soc_write(codec, 0x002, 0x6240);    // Enable Left Input Mixer (MIXINL), Enable IN1L PGA
    snd_soc_write(codec, 0x018, 0x0117);    // Unmute IN1L PGA, +30dB
    snd_soc_write(codec, 0x028, 0x0030);    // Connect IN1LN to IN1L PGA inverting input, Connect IN1LP to IN1L PGA input
    snd_soc_write(codec, 0x029, 0x0020);    // Unmute IN1L PGA output to Left Input Mixer (MIXINL) path
// Analogue Output Configuration
    snd_soc_write(codec, 0x003, 0x00F0);    // Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR), Enable Left Output Mixer Volume Control, Enable Right Output Mixer Volume Control
    snd_soc_write(codec, 0x02D, 0x0001);    // Unmute Left DAC1 to Left Output Mixer (MIXOUTL) path
    snd_soc_write(codec, 0x02E, 0x0001);    // Unmute Right DAC1 to Right Output Mixer (MIXOUTR) path
//    snd_soc_write(codec, 0x020, 0x0179);    // Unmute Left Mixer Output PGA (MIXOUTLVOL), Volume 0dB  <= default value (Reset value)
//    snd_soc_write(codec, 0x021, 0x0179);    // Unmute Right Mixer Output PGA (MIXOUTRVOL), Volume 0dB <= default value (Reset value)
    snd_soc_write(codec, 0x033, 0x0018);    // Unmute Left Output Mixer to HPOUT2 (Earpiece) path, Unmute Right Output Mixer to HPOUT2 (Earpiece) path
// Digital Path Enables and Unmutes
    snd_soc_write(codec, 0x004, 0x0202);    // Enable ADC (Left), Enable AIF1 ADC (Left) Path
    snd_soc_write(codec, 0x005, 0x0303);    // Enable DAC1 (Left), Enable DAC1 (Right), Enable AIF1 Timeslot 0 DAC1 (Left) Path, Enable AIF1 Timeslot0 DAC1 (Right) Path
    snd_soc_write(codec, 0x420, 0x0000);    // Unmute the AIF1 Timeslot 0 DAC path
    snd_soc_write(codec, 0x601, 0x0001);    // Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path
    snd_soc_write(codec, 0x602, 0x0001);    // Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path
    snd_soc_write(codec, 0x606, 0x0002);    // Enable the ADC (Left) to AIF1 Timeslot 0 (Left) mixer path
    snd_soc_write(codec, 0x610, 0x00C0);    // Unmute DAC 1 (Left)
    snd_soc_write(codec, 0x611, 0x01C0);    // Unmute DAC 1 (Right)
// Clocking
    snd_soc_write(codec, 0x200, 0x0001);    // Enable AIF1 Clock, AIF1 Clock Source = MCLK1
    snd_soc_write(codec, 0x204, 0x0001);    // Enable AIF2 Clock, AIF2 Clock Source = MCLK1
    snd_soc_write(codec, 0x208, 0x000F);    // Enable the DSP processing core for AIF1, Enable the DSP processing core for AIF2, Enable the core Clock(SYSCLK=AIF2CLK)
    snd_soc_write(codec, 0x210, 0x0008);    // Set the AIF1 sample rate to 8kHz, AIF1CLK = 1408 Fs
    snd_soc_write(codec, 0x211, 0x0073);    // Set the AIF2 sample rate to 44.1kHz, AIF2CLK = 256 Fs
// AIF1 Interface
    snd_soc_write(codec, 0x300, 0x4050);    // AIF1 Interface = I2S, 24bits
// AIF2 Interface
    snd_soc_write(codec, 0x310, 0x4010);    // AIF2 Interface = I2S, 16bits
// GPIO setting
    snd_soc_write(codec, 0x700, 0xA101);    // GPIO1
    snd_soc_write(codec, 0x701, 0x8100);    // GPIO2 - Set to MCLK2 input
    snd_soc_write(codec, 0x702, 0x8100);    // GPIO3 - Set to BCLK2 input
    snd_soc_write(codec, 0x703, 0x8100);    // GPIO4 - Set to DACLRCLK2 input
    snd_soc_write(codec, 0x704, 0x8100);    // GPIO5 - Set to DACDAT2 input
    snd_soc_write(codec, 0x705, 0xA101);    // GPIO6 - Set to GPIO2
    snd_soc_write(codec, 0x706, 0x0100);    // GPIO7 - Set to ADCDAT2 output

    mdelay(5);
// MIXOUTLVOL Unmute, +6dB
    snd_soc_write(codec, 0x020, 0x017A);    // Unmute Left Mixer Output PGA (MIXOUTLVOL), Volume +1dB
    snd_soc_write(codec, 0x021, 0x017A);    // Unmute Right Mixer Output PGA (MIXOUTRVOL), Volume +1dB
    snd_soc_write(codec, 0x01F, 0x0000);    // Unmute HPOUT2
}

static void tcc_codec_route_3(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() \n", __func__);
}

static void tcc_codec_route_5(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() \n", __func__);
}

static void tcc_codec_route_6(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() \n", __func__);
}

static void tcc_codec_route_7(struct snd_soc_codec *codec)
{
    printk("[ALSA Debug] %s() \n", __func__);
}

static void tcc_ext_control(struct snd_soc_codec *codec)
{
	int spk = 0, mic = 0, line = 0, hp = 0, hs = 0;

	/* set up jack connection */
	switch (tcc_jack_func) {
    	case TCC_HP:
    		hp = 1;
            tcc_codec_jack_hp(codec);
    		/* set = unmute headphone */
    //		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
    //		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
            return;
    	case TCC_MIC:
    		mic = 1;
    		/* reset = mute headphone */
    //		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
    //		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
            return;
    	case TCC_LINE:
    		line = 1;
    //		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
    //		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
            return;
    	case TCC_HEADSET:
    		hs = 1;
    		mic = 1;
    //		reset_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_L);
    //		set_scoop_gpio(&corgiscoop_device.dev, CORGI_SCP_MUTE_R);
            return;
        case TCC_HP_OFF:
            break;
	}


	switch (tcc_spk_func)
    {
        case TCC_CODEC_ROUTE_0:
            tcc_codec_route_0(codec);
            break;
        case TCC_CODEC_ROUTE_1:
            tcc_codec_route_1(codec);
    		spk = 1;
            break;
        case TCC_CODEC_ROUTE_2:
            tcc_codec_route_2(codec);
            break;
        default:
            tcc_codec_route_0(codec);
            break;
    }

#if 0   // For debug, B090183
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
#endif

}

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

static const char *jack_function[] = {"Headphone", "Mic", "Line", "Headset", "Off"};
static const char *spk_function[]  = {"ROUT0", "ROUT1", "ROUT2", "ROUT3"};
static const struct soc_enum tcc_enum[] = {
	SOC_ENUM_SINGLE_EXT(5, jack_function),
	SOC_ENUM_SINGLE_EXT(4, spk_function),
};

static const struct snd_kcontrol_new wm8994_tcc_controls[] = {
	SOC_ENUM_EXT("Jack Function", tcc_enum[0], tcc_get_jack,
		tcc_set_jack),
	SOC_ENUM_EXT("Speaker Function", tcc_enum[1], tcc_get_spk,
		tcc_set_spk),
};



/********************************************************************************
 * snd_soc_ops definition for TCC92xx
 ********************************************************************************/
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
	ret = snd_soc_dai_set_sysclk(codec_dai, WM8994_SYSCLK_MCLK1, clk,
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


/********************************************************************************
 * 
 ********************************************************************************/
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
static const struct snd_soc_dapm_widget wm8994_dapm_widgets[] = {
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


/*
 * Logic for a wm8994 as connected on a TCC92xx Device
 */
static int tcc_wm8994_init(struct snd_soc_codec *codec)
{
	int i, err;

	//snd_soc_dapm_nc_pin(codec, "LLINEIN");
	//snd_soc_dapm_nc_pin(codec, "RLINEIN");
	//snd_soc_dapm_enable_pin(codec, "MICIN");

printk("[Debug] %s()\n", __func__);

	/* Add tcc specific controls */
	for (i = 0; i < ARRAY_SIZE(wm8994_tcc_controls); i++) {
		err = snd_ctl_add(codec->card,
			snd_soc_cnew(&wm8994_tcc_controls[i],codec, NULL));
		if (err < 0)
			return err;
	}

	/* Add tcc specific widgets */
	snd_soc_dapm_new_controls(codec, wm8994_dapm_widgets,
				  ARRAY_SIZE(wm8994_dapm_widgets));

	/* Set up Telechips specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	snd_soc_dapm_sync(codec);
	return 0;
}


extern struct snd_soc_platform tcc_soc_platform;

/* tcc digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link tcc_dai = {
	.name = "WM8994",
	.stream_name = "WM8994 AIF2",
	.cpu_dai = &tcc_i2s_dai,
	.codec_dai = &wm8994_dai[1],
	.init = tcc_wm8994_init,
	.ops = &tcc_ops,
};

static int tcc_board_probe(struct platform_device *pdev)
{
    GPIO *pStrGPIOBaseReg = (GPIO *)tcc_p2v(HwGPIO_BASE);
    printk("TCC Board probe [%s]\n", __FUNCTION__);

//	tca_ckc_pclk_enable(PERI_DAI, 1);       // B090183, enable after clock control implement
	tca_ckc_setiobus(RB_DAICDIFCONTROLLER, 1);
//	tca_ckc_pclk_enable(PERI_AUD, 1);       // B090183, enable after clock control implement
	tca_ckc_setiobus(RB_ADMACONTROLLER, 1);

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
static struct wm8994_setup_data tcc_wm8994_setup = {
    .i2c_bus = 1,
	.i2c_address = 0x1a,
};

/* tcc audio subsystem */
static struct snd_soc_device tcc_snd_devdata = {
	.card = &snd_soc_machine_tcc,
	.codec_dev = &soc_codec_dev_wm8994,
	.codec_data = &tcc_wm8994_setup,
};

static struct platform_device *tcc_snd_device;

/* temporary i2c device creation until this can be moved into the machine
 * support file
 */
static struct i2c_board_info i2c_device[] = {
    { I2C_BOARD_INFO("WM8994", 0x1a), }
};

static int __init tcc_init(void)
{

	int ret;

    printk("tcc_init(): in... \n");

    i2c_register_board_info(1, i2c_device, ARRAY_SIZE(i2c_device));

	tcc_snd_device = platform_device_alloc("soc-audio", -1);
	if (!tcc_snd_device)
		return -ENOMEM;

	platform_set_drvdata(tcc_snd_device, &tcc_snd_devdata);
	tcc_snd_devdata.dev = &tcc_snd_device->dev;
	ret = platform_device_add(tcc_snd_device);

	if (ret)
		platform_device_put(tcc_snd_device);

    printk("\ntcc_init(): done... \n\n");

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
