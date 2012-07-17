#ifndef _CLOCK_INFO_H_
#define _CLOCK_INFO_H_

#include "globals.h"
#include "ddr.h"

/*////////////////////////////////////////////////////////////////////////////////////////////////
//
// TELECHIPS Clock Header
//
////////////////////////////////////////////////////////////////////////////////////////////////*/


/*////////////////////////////////////////////////////////////////////////////////////////////////
//
// PLL Settings......
// PLL table for XIN=12MHz
// P, M, S, VSEL
//
////////////////////////////////////////////////////////////////////////////////////////////////*/

//defines
typedef struct {
	unsigned			uFpll;
	unsigned			P;
	unsigned			M;
	unsigned			S;
	unsigned			VSEL;
} sfPLL;

#define PLLFREQ(P, M, S)		(( 120000 * (M) )  / (P) ) >> (S) // 100Hz Unit..
#define FPLL_t(P, M, S, VSEL) 	PLLFREQ(P,M,S), P, M, S, VSEL




static sfPLL	pIO_CKC_PLL045[]	=
{
	{FPLL_t(3, 280,  4,  0)},		//  70 MHz
	{FPLL_t(3, 264,  3,  0)},		// 132 MHz
	{FPLL_t(3, 292,  3,  0)},		// 146 MHz
	{FPLL_t(3, 360,  3,  1)},		// 180 MHz
	{FPLL_t(3, 380,  3,  1)},		// 190 MHz
	{FPLL_t(3, 384,  3,  1)},		// 192 MHz
	{FPLL_t(3, 408,  3,  1)},		// 204 MHz
	{FPLL_t(3, 432,  3,  2)},		// 216 MHz
	{FPLL_t(3, 444,  3,  1)},		// 222 MHz
	{FPLL_t(3, 460,  3,  1)},		// 230 MHz
	{FPLL_t(3, 260,  2,  0)},		// 260 MHz
	{FPLL_t(3, 290,  2,  0)},		// 290 MHz
	{FPLL_t(3, 304,  2,  0)},		// 304 MHz
	{FPLL_t(3, 312,  2,  0)},		// 312 MHz
	{FPLL_t(3, 324,  2,  0)},		// 324 MHz
	{FPLL_t(3, 380,  2,  1)},		// 380 MHz
	{FPLL_t(3, 400,  2,  1)},		// 400 MHz
	{FPLL_t(3, 432,  2,  1)},		// 432 MHz
	{FPLL_t(3, 468,  2,  1)},		// 468 MHz
	{FPLL_t(3, 480,  2,  1)},		// 480 MHz
	{FPLL_t(3, 486,  2,  1)},		// 486 MHz
	{FPLL_t(3, 500,  2,  1)},		// 500 MHz
	{FPLL_t(3, 264,  1,  0)},		// 528 MHz
	{FPLL_t(3, 270,  1,  0)},		// 540 MHz
	{FPLL_t(3, 297,  1,  0)},		// 594 MHz
	{FPLL_t(3, 300,  1,  0)},		// 600 MHz
	{FPLL_t(3, 312,  1,  0)},		// 624 MHz
	{FPLL_t(3, 324,  1,  0)},		// 648 MHz
	{FPLL_t(3, 330,  1,  0)},		// 660 MHz
	{FPLL_t(3, 336,  1,  0)},		// 672 MHz
	{FPLL_t(3, 348,  1,  0)},		// 696 MHz
	{FPLL_t(3, 350,  1,  0)},		// 700 MHz
	{FPLL_t(3, 360,  1,  1)},		// 720 MHz
	{FPLL_t(3, 380,  1,  1)},		// 760 MHz
	{FPLL_t(3, 384,  1,  1)},		// 768 MHz
	{FPLL_t(3, 400,  1,  1)},		// 800 MHz
	{FPLL_t(3, 410,  1,  1)},		// 820 MHz
	{FPLL_t(3, 420,  1,  1)},		// 840 MHz
	{FPLL_t(3, 430,  1,  1)},		// 860 MHz
	{FPLL_t(3, 440,  1,  1)},		// 880 MHz
	{FPLL_t(3, 450,  1,  1)},		// 900 MHz
	{FPLL_t(3, 470,  1,  1)},		// 940 MHz
	{FPLL_t(3, 490,  1,  1)},		// 980 MHz
	{FPLL_t(3, 498,  1,  1)},		// 996 MHz
	{FPLL_t(3, 251,  0,  0)},		// 1004 MHz
	{FPLL_t(3, 253,  0,  0)},		// 1012 MHz
	{FPLL_t(3, 255,  0,  0)},		// 1020 MHz
	{FPLL_t(3, 257,  0,  0)},		// 1028 MHz
	{FPLL_t(3, 259,  0,  0)},		// 1036 MHz
	{FPLL_t(3, 261,  0,  0)},		// 1044 MHz
	{FPLL_t(3, 263,  0,  0)},		// 1052 MHz
	{FPLL_t(3, 265,  0,  0)},		// 1060 MHz
	{FPLL_t(3, 267,  0,  0)},		// 1068 MHz
	{FPLL_t(3, 269,  0,  0)},		// 1076 MHz
	{FPLL_t(3, 271,  0,  0)},		// 1084 MHz
	{FPLL_t(3, 273,  0,  0)},		// 1092 MHz
	{FPLL_t(3, 275,  0,  0)},		// 1100 MHz
	{FPLL_t(3, 277,  0,  0)},		// 1108 MHz
	{FPLL_t(3, 279,  0,  0)},		// 1116 MHz
	{FPLL_t(3, 300,  0,  0)},		// 1200 MHz
};


//you can add or modify below array what you want.

static sfPLL	pIO_CKC_PLL123[]	=
{
	{FPLL_t(3, 140,  3,  1)},		// 70MHz
	{FPLL_t(3, 146,  2,  1)},		// 146MHz
	{FPLL_t(3, 102,  1,  0)},		// 204MHz
	{FPLL_t(3, 115,  1,  0)},		// 230 MHz


	{FPLL_t(3, 130,  1,  1)},		// 260 MHz
	{FPLL_t(3, 145,  1,  1)},		// 290 MHz
	{FPLL_t(3, 152,  1,  1)},		// 304 MHz
	{FPLL_t(3, 156,  1,  1)},		// 312 MHz
	{FPLL_t(3, 95,   0,  0)},		// 380 MHz
	{FPLL_t(3, 100,  0,  0)},		// 400 MHz
	{FPLL_t(3, 108,  0,  0)},		// 432 MHz
	{FPLL_t(3, 117,  0,  1)},		// 468 MHz
	{FPLL_t(3, 120,  0,  1)},		// 480 MHz
	{FPLL_t(3, 125,  0,  1)},		// 500 MHz
	{FPLL_t(3, 132,  0,  1)},		// 528 MHz
	{FPLL_t(3, 135,  0,  1)},		// 540 MHz
	{FPLL_t(3, 145,  0,  1)},		// 580 MHz
	{FPLL_t(3, 150,  0,  1)},		// 600 MHz
	{FPLL_t(3, 156,  0,  1)},		// 624 MHz
	{FPLL_t(3, 165,  0,  1)},		// 660 MHz
	{FPLL_t(3, 175,  0,  1)},		// 700 MHz
	{FPLL_t(3, 180,  0,  1)},		// 720 MHz
	{FPLL_t(3, 200,  0,  1)},		// 800 MHz
	{FPLL_t(3, 225,  0,  1)},		// 900 MHz
	{FPLL_t(3, 250,  0,  1)},		//1000 MHz
};

#define NUM_PLL045 				(sizeof(pIO_CKC_PLL045)/sizeof(sfPLL))
#define NUM_PLL123 				(sizeof(pIO_CKC_PLL123)/sizeof(sfPLL))


/*////////////////////////////////////////////////////////////////////////////////////////////////
//
// CORE clocks
//
////////////////////////////////////////////////////////////////////////////////////////////////*/

#define FCORE_STEP_NUM 74

static unsigned int FcoreStepValue[FCORE_STEP_NUM] = {
	11000000, 10600000, 10040000, 9000000, 8600000,
	8000000, 7200000, 6000000, 5062500, 4860000,
	4725000, 4556250, 4387500, 4252500, 4050000,
	3948750, 3780000, 3712500, 3645000, 3510000,
	3375000, 3240000, 3037500, 2970000, 2835000,
	2700000, 2632500, 2430000, 2362500, 2227500,
	2160000, 2025000, 1890000, 1822500, 1750000,
	1687500, 1620000, 1518750, 1485000, 1417500,
	1350000, 1215000, 1125000, 1080000, 1012500,
	960000,  945000,  907500,  892500,  840000,
	810000,  787500,  765000,  730000,  712500,
	660000,  637500,  607500,  577500,  547500,
	495000,  450000,  405000,  382500,  365000,
	330000,  270000,  247500,  240000,  225000,
	202500,  182500,  135000,  120000,
};

static unsigned int FcorePllValue[FCORE_STEP_NUM] = {
	11000000, 10600000, 10040000, 9000000, 8600000,
	8000000, 7200000, 6000000, 5400000,     4860000,
	5400000, 4860000, 5400000, 4860000,     4320000,
	4860000, 4320000, 5400000, 4860000,     4320000,
	5400000, 3240000, 3240000, 4320000,     3240000,
	5400000, 3240000, 3240000, 5400000,		3240000,
	2160000, 2160000, 2160000, 3240000,     2160000,
	5400000, 2160000, 4860000, 2160000,		3240000,
	2160000, 2160000, 1800000, 2160000,     3240000,
	1920000, 2160000, 1320000, 2040000,     1920000,
	2160000, 1800000, 2040000, 1460000,     1900000,
	1320000, 2040000, 3240000, 1320000,     1460000,
	1320000, 5400000, 2160000, 2040000,		1460000,
	1320000, 4320000, 1320000, 1920000,		1800000,
	3240000, 1460000, 2160000, 1920000,
};


static unsigned int FcoreDividerValue[FCORE_STEP_NUM] = {
	16,		16,		16,	    16,     16,
	16,		16,		16,		15,		16,
	14,     15,     13,     14,     15,
	13,     14,     11,		12,		13,
	10,     16,     15,     11,     14,
	8,		13,     12,		7,		11,
	16,     15,     14,     9,		13,
	5,		12,		5,		11,     7,
	10,     9,		10,     8,		5,
	8,		7,		11,		7,		7,
	6,      7,      6,      8,      6,
	8,      5,      3,		7,		6,
	6,      5,      4,		3,		4,
	4,		1,		3,		2,		2,
	1,		2,		1,		1,
};


/*////////////////////////////////////////////////////////////////////////////////////////////////
//
// MEMORY clocks
//
////////////////////////////////////////////////////////////////////////////////////////////////*/

typedef struct {
	unsigned int        pll1config_reg;
	unsigned int        clkctrl2_reg;
} MEMCLK;




#if defined(DRAM_DDR3)
#define FMBUS_TOTAL_NUM 37
static MEMCLK pIO_CKC_PLL4[FMBUS_TOTAL_NUM]	=
{
	{0x0200FC03,0x00200010 + DIRECTPLL4},//L"FMBUS_126Mhz",
	{0x02010403,0x00200010 + DIRECTPLL4},//L"FMBUS_130Mhz",
	{0x02011403,0x00200010 + DIRECTPLL4},//L"FMBUS_138Mhz",
	{0x02011A03,0x00200010 + DIRECTPLL4},//L"FMBUS_141Mhz",
	{0x02012203,0x00200010 + DIRECTPLL4},//L"FMBUS_145Mhz",

	{0x02013003,0x00200010 + DIRECTPLL4},//L"FMBUS_152Mhz",
	{0x02014003,0x00200010 + DIRECTPLL4},//L"FMBUS_160Mhz",
	{0x02015403,0x00200010 + DIRECTPLL4},//L"FMBUS_170Mhz",
	{0x42016803,0x00200010 + DIRECTPLL4},//L"FMBUS_180Mhz",
	{0x42017C03,0x00200010 + DIRECTPLL4},//L"FMBUS_190Mhz",

	{0x42019003,0x00200010 + DIRECTPLL4},//L"FMBUS_200Mhz",
	{0x4201A403,0x00200010 + DIRECTPLL4},//L"FMBUS_210Mhz",
	{0x4201B803,0x00200010 + DIRECTPLL4},//L"FMBUS_220Mhz",
	{0x4201CC03,0x00200010 + DIRECTPLL4},//L"FMBUS_230Mhz",
	{0x4201E003,0x00200010 + DIRECTPLL4},//L"FMBUS_240Mhz",

	{0x4201F403,0x00200010 + DIRECTPLL4},//L"FMBUS_250Mhz",
	{0x01010403,0x00200010 + DIRECTPLL4},//L"FMBUS_260Mhz",
	{0x01010E03,0x00200010 + DIRECTPLL4},//L"FMBUS_270Mhz",
	{0x01011803,0x00200010 + DIRECTPLL4},//L"FMBUS_280Mhz",
	{0x01012203,0x00200010 + DIRECTPLL4},//L"FMBUS_290Mhz",

	{0x01012C03,0x00200010 + DIRECTPLL4},//L"FMBUS_300Mhz",
	{0x01013803,0x00200010 + DIRECTPLL4},//L"FMBUS_312Mhz",
	{0x01014003,0x00200010 + DIRECTPLL4},//L"FMBUS_320Mhz",
	{0x01014A03,0x00200010 + DIRECTPLL4},//L"FMBUS_330Mhz",
	{0x41016803,0x00200010 + DIRECTPLL4},//L"FMBUS_360Mhz",

	{0x41017203,0x00200010 + DIRECTPLL4},//L"FMBUS_370Mhz",
	{0x41017C03,0x00200010 + DIRECTPLL4},//L"FMBUS_380Mhz",
	{0x41018603,0x00200010 + DIRECTPLL4},//L"FMBUS_390Mhz",
	{0x41019003,0x00200010 + DIRECTPLL4},//L"FMBUS_400Mhz",
	{0x41019A03,0x00200010 + DIRECTPLL4},//L"FMBUS_410Mhz",

	{0x4101A403,0x00200010 + DIRECTPLL4},//L"FMBUS_420Mhz",
	{0x4101AE03,0x00200010 + DIRECTPLL4},//L"FMBUS_430Mhz",
	{0x4101B803,0x00200010 + DIRECTPLL4},//L"FMBUS_440Mhz",
	{0x4101C203,0x00200010 + DIRECTPLL4},//L"FMBUS_450Mhz",
	{0x4101D603,0x00200010 + DIRECTPLL4},//L"FMBUS_470Mhz",

	{0x4101EA03,0x00200010 + DIRECTPLL4},//L"FMBUS_490Mhz",
	{0x0000FF03,0x00200010 + DIRECTPLL4},//L"FMBUS_510Mhz",
};


#elif defined(DRAM_DDR2)
#define FMBUS_TOTAL_NUM 26
static MEMCLK pIO_CKC_PLL4[FMBUS_TOTAL_NUM]	=
{
	{0x0200FC03,0x00200010 + DIRECTPLL4},//L"FMBUS_126Mhz",
	{0x02010403,0x00200010 + DIRECTPLL4},//L"FMBUS_130Mhz",
	{0x02011403,0x00200010 + DIRECTPLL4},//L"FMBUS_138Mhz",
	{0x02011A03,0x00200010 + DIRECTPLL4},//L"FMBUS_141Mhz",
	{0x02012203,0x00200010 + DIRECTPLL4},//L"FMBUS_145Mhz",

	{0x02013003,0x00200010 + DIRECTPLL4},//L"FMBUS_152Mhz",
	{0x02014003,0x00200010 + DIRECTPLL4},//L"FMBUS_160Mhz",
	{0x02015403,0x00200010 + DIRECTPLL4},//L"FMBUS_170Mhz",
	{0x42016803,0x00200010 + DIRECTPLL4},//L"FMBUS_180Mhz",
	{0x42017C03,0x00200010 + DIRECTPLL4},//L"FMBUS_190Mhz",

	{0x42019003,0x00200010 + DIRECTPLL4},//L"FMBUS_200Mhz",
	{0x4201A403,0x00200010 + DIRECTPLL4},//L"FMBUS_210Mhz",
	{0x4201B803,0x00200010 + DIRECTPLL4},//L"FMBUS_220Mhz",
	{0x4201CC03,0x00200010 + DIRECTPLL4},//L"FMBUS_230Mhz",
	{0x4201E003,0x00200010 + DIRECTPLL4},//L"FMBUS_240Mhz",

	{0x4201F403,0x00200010 + DIRECTPLL4},//L"FMBUS_250Mhz",
	{0x01010403,0x00200010 + DIRECTPLL4},//L"FMBUS_260Mhz",
	{0x01010E03,0x00200010 + DIRECTPLL4},//L"FMBUS_270Mhz",
	{0x01011803,0x00200010 + DIRECTPLL4},//L"FMBUS_280Mhz",
	{0x01012203,0x00200010 + DIRECTPLL4},//L"FMBUS_290Mhz",

	{0x01012C03,0x00200010 + DIRECTPLL4},//L"FMBUS_300Mhz",
	{0x01013803,0x00200010 + DIRECTPLL4},//L"FMBUS_312Mhz",
	{0x01014003,0x00200010 + DIRECTPLL4},//L"FMBUS_320Mhz",
	{0x01014A03,0x00200010 + DIRECTPLL4},//L"FMBUS_330Mhz",
	{0x41016803,0x00200010 + DIRECTPLL4},//L"FMBUS_360Mhz",

	{0x41019003,0x00200010 + DIRECTPLL4},//L"FMBUS_400Mhz",
};
#endif

static unsigned int FmbusItemValue[FMBUS_TOTAL_NUM] = {
	1260000,
	1300000,
	1380000,
	1410000,
	1450000,

	1520000,
	1600000,
	1700000,
	1800000,
	1900000,

	2000000,
	2100000,
	2200000,
	2300000,
	2400000,

	2500000,
	2600000,
	2700000,
	2800000,
	2900000,

	3000000,
	3120000,
	3200000,
	3300000,
	3600000,

#if defined(DRAM_DDR3)
	3700000,
	3800000,
	3900000,
#endif
	4000000,
#if defined(DRAM_DDR3)
	4100000,
	4200000,
	4300000,
	4400000,
	4500000,
	4700000,
	4900000,
	5100000,
#endif
};

/*////////////////////////////////////////////////////////////////////////////////////////////////
//
// BUS(except CORE,MEM) clocks
//
////////////////////////////////////////////////////////////////////////////////////////////////*/

#define FBUS_STEP_NUM 33
static unsigned int FbusStepValue[FBUS_STEP_NUM] = {
	3120000, // PLL1
	2700000, // PLL3
	2500000, // PLL2

	2080000, // PLL1
	1800000, // PLL3
	1666666, // PLL2

	1560000, // PLL1
	1350000, // PLL3
	1250000, // PLL2

	1080000, // PLL3
	1040000, // PLL1
	1000000, // PLL2

	900000,  // PLL3
	833333,  // PLL2
	780000,  // PLL1

	714285,  // PLL2
	693333,  // PLL1
	625000,  // PLL2

	567272,  // PLL1
	540000,  // PLL3
	500000,  // PLL2

	480000,  // PLL1
	450000,  // PLL3
	416000,  // PLL1

	390000,  // PLL1
	385714,  // PLL3
	360000,  // PLL3

	333333,  // PLL2
	312500,  // PLL2

	60000, //25. XIN
	30000, //26. XIN
	10000, //27. XIN
	0, //28.

};

static unsigned int FbusSrcValue[FBUS_STEP_NUM] = {
	DIRECTPLL1, DIRECTPLL3, DIRECTPLL2,
	DIRECTPLL1, DIRECTPLL3, DIRECTPLL2,
	DIRECTPLL1, DIRECTPLL3, DIRECTPLL2,
	DIRECTPLL3, DIRECTPLL1, DIRECTPLL2,
	DIRECTPLL3, DIRECTPLL2, DIRECTPLL1,
	DIRECTPLL2, DIRECTPLL1, DIRECTPLL2,
	DIRECTPLL1, DIRECTPLL3, DIRECTPLL2,
	DIRECTPLL1, DIRECTPLL3, DIRECTPLL1,
	DIRECTPLL1, DIRECTPLL3, DIRECTPLL3,
	DIRECTPLL2, DIRECTPLL2,
	DIRECTXIN,  DIRECTXIN,
	DIRECTXIN,  DIRECTXIN,

};


/*////////////////////////////////////////////////////////////////////////////////////////////////
//
// String Tables...
//
////////////////////////////////////////////////////////////////////////////////////////////////*/

////////////IOBUS
#define IOBUS_TOTAL_NUM	35
static char IobusItemString[IOBUS_TOTAL_NUM][15] = {
	"USB11H",
	"USB20OTG",
	"IDE",
	"DMA",
	"SDMMC",
	"SATA",
	"MSTICK",
	"I2C",
	"NFC",
	"EXCON0",
	"EXCON1",
	"UART0",
	"UART1",
	"UART2",
	"UART3",
	"UART4",
	"UART5",
	"GPSB0",
	"GPSB1",
	"GPSB2",
	"GPSB3",
	"GPSB4",
	"GPSB5",
	"DAICDIF",
	"ECC",
	"SPDIF",
	"RTC",
	"TSADC",
	"GPS",
	"Reserved",
	"CAN",
	"ADMA", // 31
	"MPEFEC",
	"TSIF",
	"SRAM",
};

static unsigned int IobusItemValue[IOBUS_TOTAL_NUM] = {
	0,1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,
	31,32,33,34
};


////////////DDI
#define DDI_TOTAL_NUM	11
static char DdiItemString[DDI_TOTAL_NUM][15] = {
	"DDIPWDN_CIF",
	"DDIPWDN_VIQE",
	"DDIPWDN_LCDC0",
	"DDIPWDN_LCDC1",
	"DDIPWDN_LCDSI",
	"DDIPWDN_MSCL0",
	"DDIPWDN_MSCL1",
	"DDIPWDN_DDIC",
	"DDIPWDN_HDMI",

};

static unsigned int DdiItemValue[DDI_TOTAL_NUM] = {
	DDIPWDN_RESERVED,
	DDIPWDN_VIQE,
	DDIPWDN_LCDC0,
	DDIPWDN_LCDC1,
	DDIPWDN_LCDSI0,
	DDIPWDN_MSCL0,
	DDIPWDN_MSCL1,
	DDIPWDN_DDIC,
	DDIPWDN_HDMI,
	DDIPWDN_LCDSI1,
	DDIPWDN_STEPMAX,
};

////////////ETC
#define ETC_TOTAL_NUM 6
static char EtcItemString[ETC_TOTAL_NUM][20] = {
	"ETC_USBPHYOFF",
	"ETC_USBPHYON",
	"ETC_3DGPUOFF",
	"ETC_3DGPUON",
	"ETC_OVERLAYMIXEROFF",
	"ETC_OVERLAYMIXERON ",
};

static unsigned int EtcItemValue[ETC_TOTAL_NUM] = {
	ETC_USBPHYOFF,
	ETC_USBPHYON,
	ETC_3DGPUOFF,
	ETC_3DGPUON,
	ETC_OVERLAYMIXEROFF,
	ETC_OVERLAYMIXERON
};



#define CLOCKINFO_TOTAL_NUM 14
static unsigned int ClockinfoItemValue[CLOCKINFO_TOTAL_NUM] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
};

static char ClockinfoItemString[CLOCKINFO_TOTAL_NUM][6] = {
    "PLL0",
    "PLL1",
    "PLL2",
    "PLL3",
    "PLL4",
    "PLL5",
    "CPU",
    "BUS",
    "DDI",
    "GRP",
    "IOBUS",
    "VBUS",
    "VPU",
    "SMU",
};

#define PERI_TOTAL_NUM	56
static unsigned int PeriItemValue[PERI_TOTAL_NUM] = {
	0,1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,
	31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,
    51,52,53,54,55,
};
static char PeriItemString[PERI_TOTAL_NUM][32] = {
    "PERI_TCX",
    "PERI_TCT",
    "PERI_TCZ",
    "PERI_LCD0",
    "PERI_LCD1",
    "PERI_LCDSI0",
    "PERI_LCDSI1",
    "PERI_HDMIA",
    "PERI_DSI",
    "PERI_RESERVED0",
    "PERI_HDMI",
    "PERI_USB11H",
    "PERI_SDMMC0",
    "PERI_MSTICK",
    "PERI_I2C0",
    "PERI_UART0",
    "PERI_UART1",
    "PERI_UART2",
    "PERI_UART3",
    "PERI_UART4",
    "PERI_UART5",
    "PERI_GPSB0",
    "PERI_GPSB1",
    "PERI_GPSB2",
    "PERI_GPSB3",
    "PERI_GPSB4",
    "PERI_GPSB5",
    "PERI_ADC",
    "PERI_SPDIF",
    "PERI_EHI0",
    "PERI_EHI1",
    "PERI_AUD",       //Not used
    "PERI_PDM",
    "PERI_SDMMC1",
    "PERI_SDMMC2",
    "PERI_SDMMC3",
    "PERI_DAI0",
    "PERI_DAI1",      //7.1 ch audio
    "PERI_DAI2",
    "PERI_I2C1",
    "PERI_PCIE",
    "PERI_SATAH0",
    "PERI_SATAH1",
    "PERI_SATAD",
    "PERI_USB20H",
    "PERI_GMAC",
    "PERI_ISPJ",
    "PERI_ISPS",
    "PERI_CIFMC",
    "PERI_CIFSC",
    "PERI_FILTER",
    "PERI_RESERVED1",
    "PERI_RESERVED2",
    "PERI_RESERVED3",
    "PERI_OUT0",
    "PERI_OUT1",
};



#endif

