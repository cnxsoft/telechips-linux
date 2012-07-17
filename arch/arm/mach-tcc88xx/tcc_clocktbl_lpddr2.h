/*
 * arch/arm/mach-tcc88xx/tcc_clocktbl_lpddr2.h
 *
 * TCC88xx cpufreq driver
 *
 * Copyright (C) 2010 Telechips, Inc.
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

static struct tcc_freq_table_t gtClockLimitTable[] = {
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO /   CAM */
//	{ 100000,      0, 110000,      0,  72000,      0,      0,  50000,      0,      0 },
//	{ 144000,      0, 120000,      0,  99000,      0,      0,  50000,      0,      0 },
	{ 200000,      0, 160000,      0, 108000,      0,      0,  50000,      0,      0 },	// Core 0.95V
	{ 300000,      0, 160000,      0, 112000,      0,      0,  75000,      0,      0 },
	{ 400000,      0, 160000,      0, 125000,      0,      0, 100000,      0,      0 },	// Core 1.00V
	{ 500000,      0, 200000,      0, 148000,      0,      0, 100000,      0,      0 },	// Core 1.05V
	{ 600000,      0, 240000,      0, 148000,      0,      0, 100000,      0,      0 },	// Core 1.10V
	{ 700000,      0, 280000,      0, 172000,      0,      0, 100000,      0,      0 },	// Core 1.15V
	{ 800000,      0, 320000,      0, 198000,      0,      0, 100000,      0,      0 },	// Core 1.20V
	{ 900000,      0, 320000,      0, 220000,      0,      0, 100000,      0,      0 },	// Core 1.25V
	{ 996000,      0, 320000,      0, 250000,      0,      0, 100000,      0,      0 },	// Core 1.32V
#if defined(CONFIG_CPU_HIGHSPEED)
	{1200000,      0, 320000,      0, 250000,      0,      0, 100000,      0,      0 },	// Core 1.45V
#endif
};

//sync with gtJpegClockLimitTable
const struct tcc_freq_table_t gtCameraClockLimitTable[] =
{
	{      0, 147000, 280000,      0,      0,      0,      0,      0,      0, 330000 },	// Core 1.20V
	{      0, 221000, 280000,      0,      0,      0,      0,      0,      0, 330000 },	// Core 1.20V
#if 1
	{      0, 297000, 320000,      0,      0,      0,      0,      0,      0, 330000 },	// Core 1.20V	// do not verify for Full HD
#else
	{      0, 297000, 320000,      0,      0,      0,      0,      0,      0, 330000 },	// Core 1.32V	// do not verify for Full HD
#endif
};

const struct tcc_freq_table_t gtISPCameraClockLimitTable[] =
{
	{      0, 147000, 200000,      0,      0,      0,      0,      0,      0, 330000 },	// Core 1.20V
	{      0, 221000, 240000,      0,      0,      0,      0,      0,      0, 330000 },	// Core 1.20V
#if 1
	{      0, 297000, 320000,      0,      0,      0,      0,      0,      0, 330000 },	// Core 1.20V	// do not verify for Full HD
#else
	{      0, 297000, 320000,      0,      0,      0,      0,      0,      0, 330000 },	// core 1.32V	// do not verify for Full HD
#endif
};

const struct tcc_freq_table_t gtVpuNormalClockLimitTable[] =
{
	{      0, 147000, 160000,      0,      0, 150000, 145000,      0,      0,      0 },	// Core 1.00V	// D1
	{      0, 221000, 240000,      0,      0, 225000, 218000,      0,      0,      0 },	// Core 1.10V	// HD
	{      0, 297000, 320000,      0,      0, 336000, 297000,      0,      0,      0 },	// Core 1.32V	// Not recommanded value
};

const struct tcc_freq_table_t gtVpu_480p_ClockLimitTable[] =
{
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO /   CAM */
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
};

const struct tcc_freq_table_t gtVpu_720p_ClockLimitTable[] =
{
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO /   CAM */
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
};

const struct tcc_freq_table_t gtVpu_1080p_ClockLimitTable[] =
{
	/*  CPU /   DDI /   MEM /   GPU /    IO /  VBUS /  VCOD /   SMU /  HSIO /   CAM */
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
	{      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },	// * 10Mbps, Core 1.05V
};

const struct tcc_freq_table_t gtJpegClockLimitTable[]= {
	{      0, 147000, 160000,      0,      0, 150000, 145000,      0,      0,      0 },	// Core 1.00V	// D1
	{      0, 221000, 240000,      0,      0, 225000, 218000,      0,      0,      0 },	// Core 1.10V	// HD
	{      0, 297000, 320000,      0,      0, 336000, 297000,      0,      0,      0 },	// Core 1.32V	// Not recommanded value
};

const struct tcc_freq_table_t gtJpegMaxClockLimitTable = {
	       0, 297000, 320000,      0,      0, 336000, 297000,      0,      0,      0	// Core 1.32V	// Not recommanded value
};

const struct tcc_freq_table_t gtHdmiClockLimitTable = {
	       0, 336000, 320000,      0, 198000,      0,      0,      0,      0,      0	// Core 1.32V
};

const struct tcc_freq_table_t gtMaliClockLimitTable = {
	       0,      0, 320000, 322000,      0,      0,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t stFBClockLimitTable = {
	       0, 110000, 125000,      0,  98000,      0,      0,      0,      0,      0	// Core 1.00V
};

const struct tcc_freq_table_t gtOverlayClockLimitTable = {
	       0,      0,      0,      0, 148000,      0,      0,      0,      0,      0	// Core 1.20V
};


const struct tcc_freq_table_t stPowerResumeClockLimitTable = {
	       0,      0, 320000,      0, 198000,      0,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t gtTvClockLimitTable = {
	       0, 297000, 320000,      0,      0,      0,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t gtEthernetClockLimitTable = {
	       0, 297000, 320000,      0, 198000,      0,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t gtBtClockLimitTable = {
	       0,      0, 320000,      0, 198000,      0,      0,      0,      0,      0	// Core 1.32V
};

const struct tcc_freq_table_t gtUSBClockLimitTable[] = {
	{      0,      0,      0,      0,  98000,      0,      0,      0,      0,      0 }, // Core 1.00V	// Idle
	{      0,      0, 320000,      0, 198000,      0,      0,      0,      0,      0 }, // Core 1.20V	// Actived
};

const struct tcc_freq_table_t gtAppClockLimitTable = {
	  996000,      0, 320000,      0,      0,      0,      0,      0,      0,      0	// Core 1.32V
};

const struct tcc_freq_table_t gtRemoconClockLimitTable = {
	       0,      0,      0,      0, 198000,      0,      0,      0,      0,      0	// Core 1.20V
};

const struct tcc_freq_table_t gtHSIOClockLimitTable = {
#if 0
	       0,      0, 320000,      0,      0,      0,      0,      0, 243000,      0	// Core 1.20V
#else
	       0,      0, 320000,      0,      0,      0,      0,      0, 273000,      0	// Core 1.25V
#endif
};

const struct tcc_freq_table_t gtHSIONormalClockLimitTable = {
	       0,      0,      0,      0,      0,      0,      0,      0, 122000,      0	// Core 1.00V
};

const struct tcc_freq_table_t gtVoipClockLimitTable[] = {
//	{      0,      0, 160000,      0, 125000,      0,      0,      0,      0,      0 },	// Core 1.00V
//	{      0,      0, 200000,      0, 148000,      0,      0,      0,      0,      0 },	// Core 1.05V
	{      0,      0, 240000,      0, 148000,      0,      0,      0,      0,      0 },	// Core 1.10V
	{      0,      0, 320000,      0, 250000,      0,      0,      0,      0,      0 },	// Core 1.32V
};

EXPORT_SYMBOL(gtCameraClockLimitTable);
EXPORT_SYMBOL(gtISPCameraClockLimitTable);
EXPORT_SYMBOL(gtVpuNormalClockLimitTable);
EXPORT_SYMBOL(gtVpu_480p_ClockLimitTable);
EXPORT_SYMBOL(gtVpu_720p_ClockLimitTable);
EXPORT_SYMBOL(gtVpu_1080p_ClockLimitTable);
EXPORT_SYMBOL(gtJpegClockLimitTable);
EXPORT_SYMBOL(gtJpegMaxClockLimitTable);
EXPORT_SYMBOL(gtHdmiClockLimitTable);
EXPORT_SYMBOL(gtMaliClockLimitTable);
EXPORT_SYMBOL(stFBClockLimitTable);
EXPORT_SYMBOL(gtTvClockLimitTable);
EXPORT_SYMBOL(gtEthernetClockLimitTable);
EXPORT_SYMBOL(gtBtClockLimitTable);
EXPORT_SYMBOL(gtAppClockLimitTable);
EXPORT_SYMBOL(gtUSBClockLimitTable);
EXPORT_SYMBOL(gtRemoconClockLimitTable);
EXPORT_SYMBOL(gtHSIOClockLimitTable);
EXPORT_SYMBOL(gtHSIONormalClockLimitTable);
EXPORT_SYMBOL(gtVoipClockLimitTable);
