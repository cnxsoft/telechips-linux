/*
 * arch/arm/mach-tcc892x/tcc_clocktbl_ddr3.h
 *
 * TCC892x cpufreq driver
 *
 * Copyright (C) 2011 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

//#define VIDEO_USING_WVGA_LCD //ZzaU :: Below clock table is just for 1280x800 LCD. Enable this feature if WVGA LCD will be used.

static struct tcc_freq_table_t gtClockLimitTable[] = {
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO */
#if (0)
	{ 343750, 155150, 300000, 184000,  97470, 137750, 137750,  97470, 124320 },	// Core 1.00V
	{ 401250, 189010, 322885, 224140, 118735, 167800, 167800, 100000, 151445 },	// Core 1.05V
	{ 468750, 222860, 380710, 264290, 140000, 197860, 197860, 100000, 178570 },	// Core 1.10V
	{ 546875, 267425, 456860, 317140, 168000, 237425, 237425, 100000, 214285 },	// Core 1.15V
	{ 625000, 312000, 533000, 370000, 196000, 277000, 277000, 100000, 250000 },	// Core 1.20V
	{ 718750, 349440, 533000, 414400, 219520, 310240, 310240, 100000, 280000 },	// Core 1.25V
	{ 812500, 386880, 600000, 458800, 243040, 343480, 343480, 100000, 310000 },	// Core 1.30V
#else
#if defined(CONFIG_MACH_TCC8920ST)
	{ 343750,      0, 300000,      0,  97470,      0,      0,  97470,      0 },	// Core 1.00V
	{ 401250,      0, 322885,      0, 118735,      0,      0, 100000,      0 },	// Core 1.05V
	{ 468750,      0, 380710,      0, 140000,      0,      0, 100000,      0 },	// Core 1.10V
	{ 546875,      0, 456860,      0, 168000,      0,      0, 100000,      0 },	// Core 1.15V
	{ 625000,      0, 533000,      0, 196000,      0,      0, 100000,      0 },	// Core 1.20V
	#if !defined(CONFIG_STB_BOARD_DONGLE)
	{ 718750,      0, 533000,      0, 219520,      0,      0, 100000,      0 },	// Core 1.25V
	{ 812500,      0, 600000,      0, 243040,      0,      0, 100000,      0 },	// Core 1.30V
	#if !defined(CONFIG_STB_BOARD_HDB892S) && !defined(CONFIG_STB_BOARD_HDB892F) && !defined(CONFIG_STB_BOARD_ISDBT_MODULE)
	{ 937500,      0, 600000,      0, 243040,      0,      0, 100000,      0 },	// Core 1.40V
	#endif
	#endif
#else
	{ 343750,      0, 300000,      0,  97470,      0,      0,  97470,      0 },	// Core 1.00V
	{ 401250,      0, 322885,      0, 118735,      0,      0, 100000,      0 },	// Core 1.05V
	{ 468750,      0, 380710,      0, 140000,      0,      0, 100000,      0 },	// Core 1.10V
	{ 546875,      0, 456860,      0, 168000,      0,      0, 100000,      0 },	// Core 1.15V
	{ 625000,      0, 533000,      0, 196000,      0,      0, 100000,      0 },	// Core 1.20V
	{ 718750,      0, 533000,      0, 219520,      0,      0, 100000,      0 },	// Core 1.25V
	{ 812500,      0, 600000,      0, 243040,      0,      0, 100000,      0 },	// Core 1.30V
	#if defined(CONFIG_M805S_8925_0XX)
	{ 900000,      0, 600000,      0, 243040,      0,      0, 100000,      0 },	// Core 1.40V
	#else
	{ 937500,      0, 600000,      0, 243040,      0,      0, 100000,      0 },	// Core 1.40V
	#endif
#endif
#if defined(CONFIG_CPU_HIGHSPEED)
	{ 996000,      0, 600000,      0, 243040,      0,      0, 100000,      0 },	// Core 1.50V
#endif
#endif
};

//sync with gtJpegClockLimitTable  default 5M / 720p 
const struct tcc_freq_table_t gtCameraClockLimitTable[] =
{
	{      0, 267425, 380710,      0,      0,      0,      0,      0,      0 },     
	{      0, 267425, 456860,      0,      0,      0,      0,      0,      0 },     
	{      0, 267425, 456860,      0,      0,      0,      0,      0,      0 },     
//	{      0, 386880, 533000,      0,      0,      0,      0,      0,      0 },	// Core 1.30V
//	{      0, 386880, 533000,      0,      0,      0,      0,      0,      0 },	// Core 1.30V
};

const struct tcc_freq_table_t gtISPCameraClockLimitTable[] =
{
	{      0, 222860, 380710,      0,      0,      0,      0,      0,      0 },	// Core 1.10V
	{      0, 222860, 533000,      0,      0,      0,      0,      0,      0 },	// Core 1.20V
	{      0, 386880, 533000,      0,      0,      0,      0,      0,      0 },	// Core 1.30V
};

const struct tcc_freq_table_t gtVpuNormalClockLimitTable[] =
{
	// PLL: 594 / 500 / 432 / 672
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO */	
#ifdef VIDEO_USING_WVGA_LCD
	{      0, 155150, 456860, 317140,  97470, 137750, 137750,      0,      0 },	// Core 1.00V  (137/137)
	{      0, 189010, 456860, 317140, 118735, 167800, 167800,      0,      0 },	// Core 1.05V  (166/166)
	{      0, 267425, 456860, 317140, 168000, 237425, 237425,      0,      0 },	// Core 1.15V  (224/224)
#else
	{ 343750, 189010, 322885,      0, 118735, 167800, 167800,      0,      0 },	// Core 1.05V  (166/166)
	{ 401250, 222860, 380710,      0, 140000, 197860, 197860,      0,      0 },	// Core 1.10V  (182/182)
	#if defined(CONFIG_STB_BOARD_DONGLE)
	{ 401250, 312000, 533000,      0, 196000, 277000, 277000,      0,      0 },	// Core 1.20V  (336/336)
	#else
	{ 401250, 386880, 600000,      0, 243040, 343480, 343480,      0,      0 },	// Core 1.30V  (336/336)
	#endif
#endif
};

const struct tcc_freq_table_t gtVpu_480p_ClockLimitTable[] =
{
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO */	
#ifdef VIDEO_USING_WVGA_LCD
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * ~ 10Mbps,  Core 1.00V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 },	//   10~20Mbps, Core 1.00V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 },	//   20~30Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 },	//   30Mbps ~,  Core 1.10V
#else
	{ 343750,      0,      0,      0, 118735,      0,      0,      0,      0 },	// * ~ 10Mbps,  Core 1.05V
	{ 401250,      0,      0,      0, 140000,      0,      0,      0,      0 },	//   10~20Mbps, Core 1.10V
	{ 468750,      0,      0,      0, 168000,      0,      0,      0,      0 },	//   20~30Mbps, Core 1.15V
	{ 546875,      0,      0,      0, 168000,      0,      0,      0,      0 },	//   30Mbps ~,  Core 1.15V
#endif
};

const struct tcc_freq_table_t gtVpu_720p_ClockLimitTable[] =
{
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO */	
#ifdef VIDEO_USING_WVGA_LCD
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 }, //  ~ 10Mbps,  Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // *10~20Mbps, Core 1.05V
	{ 401250,      0,      0,      0,      0,      0,      0,      0,      0 }, //  20~30Mbps, Core 1.05V
	{ 468750, 312000,      0,      0, 196000, 277000, 277000,      0,      0 },	//  30Mbps ~,  Core 1.20V (250/250)
#else
	{ 401250,      0,      0,      0, 168000,      0,      0,      0,      0 }, //  ~ 10Mbps,  Core 1.15V
	{ 468750,      0,      0,      0, 168000,      0,      0,      0,      0 }, // *10~20Mbps, Core 1.15V
	#if defined(CONFIG_STB_BOARD_DONGLE)
	{ 625000,      0,      0,      0, 196000, 237425, 237425,      0,      0 }, //  20~30Mbps, Core 1.20V
	#else
	{ 625000,      0,      0,      0, 219520, 237425, 237425,      0,      0 }, //  20~30Mbps, Core 1.20V
	{ 812500, 386880, 533000,      0, 243040, 343480, 343480,      0,      0 },	//  30Mbps ~,  Core 1.30V
	#endif
#endif
};

const struct tcc_freq_table_t gtVpu_1080p_ClockLimitTable[] =
{
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO */	
#ifdef VIDEO_USING_WVGA_LCD
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // 	~ 10Mbps,  Core 1.15V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // * 10~20Mbps, Core 1.15V
	{ 468750, 312000,      0,      0, 196000, 277000, 277000,      0,      0 }, //   20~30Mbps, Core 1.20V (250/250)
	{ 625000, 386880, 600000,      0, 243040, 343480, 343480,      0,      0 }, //   30Mbps ~,  Core 1.30V (343/343)
#else
	#if defined(CONFIG_STB_BOARD_DONGLE)
	{ 625000,      0,      0, 370000,      0,      0,      0,      0,      0 }, // * 10~20Mbps, Core 1.20V
	#else
	{ 468750,      0,      0, 414400,      0,      0,      0,      0,      0 }, // 	~ 10Mbps,  Core 1.30V
	{ 625000,      0,      0, 414400,      0,      0,      0,      0,      0 }, // * 10~20Mbps, Core 1.30V
	{ 718750,      0,      0, 458800,      0,      0,      0,      0,      0 }, //   20~30Mbps, Core 1.30V
	{ 812500,      0,      0, 458800,      0,      0,      0,      0,      0 }, //   30Mbps ~,  Core 1.30V
	#endif
#endif
};

const struct tcc_freq_table_t gtJpegClockLimitTable[]= {
	{      0,      0, 380710,      0, 140000, 197860, 197860,      0,      0 },	// Core 1.10V
	{      0,      0, 533000,      0, 196000, 277000, 277000,      0,      0 },	// Core 1.20V
	{      0,      0, 533000,      0, 243040, 297000, 297000,      0,      0 },	// Core 1.30V
};

const struct tcc_freq_table_t gtJpegMaxClockLimitTable = {
	#if defined(CONFIG_STB_BOARD_DONGLE)
	       0,      0, 533000,      0, 196000, 277000, 277000,      0,      0	// Core 1.20V
	#else
	       0,      0, 600000,      0, 243040, 343480, 343480,      0,      0	// Core 1.30V
	#endif
};

const struct tcc_freq_table_t gtMultiVpuClockLimitTable = {
	#if defined(CONFIG_STB_BOARD_DONGLE)
	       0,      0, 533000,      0, 196000, 277000, 277000,      0,      0	// Core 1.20V
	#else
	       0,      0, 600000,      0, 243040, 296000, 296000,      0,      0	// Core 1.30V
	#endif
};

const struct tcc_freq_table_t gtHdmiClockLimitTable = {
#ifdef VIDEO_USING_WVGA_LCD
	  625000, 386880, 600000,      0, 243040, 296000, 296000,      0,      0	// Core 1.30V
#else
#if defined(CONFIG_STB_BOARD_DONGLE)
	  625000, 312000, 533000,      0, 196000, 277000, 277000,      0,      0	// Core 1.20V
#else
	  625000, 386880, 600000,      0, 243040,      0,      0,      0,      0	// Core 1.30V
#endif /* CONFIG_STB_BOARD_DONGLE */
#endif
};

const struct tcc_freq_table_t gtMaliClockLimitTable[] = {
#if defined(CONFIG_GPU_BUS_SCALING)
	{      0,      0, 300000, 184000,      0,      0,      0,      0,      0 },	// Core 1.00V
	{      0,      0, 322885, 224140,      0,      0,      0,      0,      0 },	// Core 1.05V
	{      0,      0, 380710, 264290,      0,      0,      0,      0,      0 },	// Core 1.10V
	{      0,      0, 456860, 317140,      0,      0,      0,      0,      0 },	// Core 1.15V
	{      0,      0, 533000, 370000,      0,      0,      0,      0,      0 },	// Core 1.20V
	{      0,      0, 563450, 414400,      0,      0,      0,      0,      0 },	// Core 1.25V
	{      0,      0, 600000, 458800,      0,      0,      0,      0,      0 },	// Core 1.30V
#else
	{      0,      0, 533000, 370000,      0,      0,      0,      0,      0 },	// Core 1.20V
#endif
};

const struct tcc_freq_table_t stFBClockLimitTable = {
	       0, 155150, 300000,      0,  97470,      0,      0,      0,      0	// Core 1.00V
};

const struct tcc_freq_table_t gtOverlayClockLimitTable = {
	       0,      0,      0,      0, 196000,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t gtTvClockLimitTable = {
	       0, 312000, 533000,      0,      0,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t gtBtClockLimitTable = {
	#if defined(CONFIG_STB_BOARD_DONGLE)
	       0,      0, 533000,      0, 196000,      0,      0,      0,      0	// Core 1.20V
	#else
	       0,      0, 600000,      0, 196000,      0,      0,      0,      0	// Core 1.20V
	#endif
};

const struct tcc_freq_table_t gtUSBClockLimitTable[] = {
	{      0,      0,      0,      0,  97470,      0,      0,      0,      0 },	// Core 1.00V	// Idle
	{      0,      0, 533000,      0, 196000,      0,      0,      0,      0 },	// Core 1.20V	// Actived
};

const struct tcc_freq_table_t gtAppClockLimitTable = {
	  812500,      0, 533000,      0,      0,      0,      0,      0,      0	// Core 1.30V
};

const struct tcc_freq_table_t gtRemoconClockLimitTable = {
	       0,      0,      0,      0, 196000,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t gtHSIOClockLimitTable = {
	       0,      0, 533000,      0,      0,      0,      0,      0, 250000	// Core 1.20V
};

const struct tcc_freq_table_t gtHSIONormalClockLimitTable = {
	       0,      0,      0,      0,      0,      0,      0,      0, 124320	// Core 1.00V
};

const struct tcc_freq_table_t gtVoipClockLimitTable[] = {
	{ 625000,      0, 533000,      0, 196000,      0,      0, 100000,      0 },	// Core 1.20V
	#if !defined(CONFIG_STB_BOARD_DONGLE)
	{ 812500,      0, 600000,      0, 243040,      0,      0, 100000,      0 },	// Core 1.30V
	#endif
};

EXPORT_SYMBOL(gtCameraClockLimitTable);
EXPORT_SYMBOL(gtISPCameraClockLimitTable);
EXPORT_SYMBOL(gtVpuNormalClockLimitTable);
EXPORT_SYMBOL(gtVpu_480p_ClockLimitTable);
EXPORT_SYMBOL(gtVpu_720p_ClockLimitTable);
EXPORT_SYMBOL(gtVpu_1080p_ClockLimitTable);
EXPORT_SYMBOL(gtJpegClockLimitTable);
EXPORT_SYMBOL(gtJpegMaxClockLimitTable);
EXPORT_SYMBOL(gtMultiVpuClockLimitTable);
EXPORT_SYMBOL(gtHdmiClockLimitTable);
EXPORT_SYMBOL(gtMaliClockLimitTable);
EXPORT_SYMBOL(stFBClockLimitTable);
EXPORT_SYMBOL(gtTvClockLimitTable);
EXPORT_SYMBOL(gtBtClockLimitTable);
EXPORT_SYMBOL(gtAppClockLimitTable);
EXPORT_SYMBOL(gtUSBClockLimitTable);
EXPORT_SYMBOL(gtRemoconClockLimitTable);
EXPORT_SYMBOL(gtHSIOClockLimitTable);
EXPORT_SYMBOL(gtHSIONormalClockLimitTable);
EXPORT_SYMBOL(gtVoipClockLimitTable);
