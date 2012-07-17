/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               CAMERA    API    M O D U L E

                        EDIT HISTORY FOR MODULE

when        who       what, where, why
--------    ---       -------------------------------------------------------
10/xx/08  Telechips   Created file.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include <linux/delay.h>
#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include "sensor_if.h"
#include "cam.h"
#include "cam_reg.h"
#include "tcc_cam_i2c.h"

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_SR130PC10

#if (0)
#if defined(CONFIG_MACH_TCC9200S) 
#define CAM_PWR_ENABLE()		(HwGPIOD->GPEN |= Hw10);	(HwGPIOD->GPDAT |= Hw10);
#define CAM_PWR_DISABLE()		(HwGPIOD->GPEN |= Hw10);	(HwGPIOD->GPDAT &= ~Hw10);
#define CAM_PWRDN_ENABLE()		(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT |= Hw21);
#define CAM_PWRDN_DISABLE()		(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT &= ~Hw21);
#define CAM_RESET_HIGH()		(HwGPIOE->GPEN |= Hw2);		(HwGPIOE->GPDAT |= Hw2);
#define CAM_RESET_LOW()			(HwGPIOE->GPEN |= Hw2);		(HwGPIOE->GPDAT &= ~Hw2);
#elif defined(CONFIG_MACH_TCC9200) || defined(CONFIG_MACH_TCC9201) || defined(CONFIG_MACH_TCC8900)
#define CAM_PWR_ENABLE()
#define CAM_PWR_DISABLE()
#define CAM_PWRDN_ENABLE()		//(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT |= Hw21);
#define CAM_PWRDN_DISABLE()		//(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT &= ~Hw21);
#define CAM_RESET_HIGH()		(HwGPIOE->GPEN |= Hw26);	(HwGPIOE->GPDAT |= Hw26);
#define CAM_RESET_LOW()			(HwGPIOE->GPEN |= Hw26);	(HwGPIOE->GPDAT &= ~Hw26);
#else
#define CAM_PWR_ENABLE()
#define CAM_PWR_DISABLE()
#define CAM_PWRDN_ENABLE()		(HwGPFEN |= Hw1); 	(HwGPFDAT |= Hw1);
#define CAM_PWRDN_DISABLE()		(HwGPFEN |= Hw1); 	(HwGPFDAT &= ~Hw1);
#define CAM_RESET_HIGH()		(HwGPEEN |= Hw9); 	(HwGPEDAT |= Hw9);
#define CAM_RESET_LOW()			(HwGPEEN |= Hw9); 	(HwGPEDAT &= ~Hw9);
#endif
#endif

/* Array of image sizes supported by MT9D111.  These must be ordered from 
 * smallest image size to largest.
 */
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
struct capture_size sensor_sizes_sr130pc10[] = {
	//{ 1600, 1200 },	/* UXGA */
	{ 1280,  1024 },	/* SXGA */
	{ 1024,  768 },	/* XGA */
	{  800,  600 },	/* SVGA */
	{  640,  480 },	/* VGA */
	{  320,  240 },	/* QVGA */
	{  176,  144 },	/* QCIF */
};
#else
struct capture_size sensor_sizes[] = {
	//{ 1600, 1200 },	/* UXGA */
	{ 1280,  1024 },	/* SXGA */
	{ 1024,  768 },	/* XGA */
	{  800,  600 },	/* SVGA */
	{  640,  480 },	/* VGA */
	{  320,  240 },	/* QVGA */
	{  176,  144 },	/* QCIF */
};
#endif

/* register initialization tables for sensor */
/* common sensor register initialization for all image sizes, pixel formats, 
 * and frame rates
 */

static struct sensor_reg sensor_initialize_sr130pc10[] = {

// Initialize_Begin
	{0x01, 0xF1},	//sleep on
	{0x08, 0x0F},	//Hi-Z on
	{0x01, 0xF0},	//sleep off
	{0x03, 0x00},	//Dummy 750us
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x0E, 0x03},	//PLL X 2.0
	{0x0E, 0x73},	//PLL X 2.0 enable
	{0x03, 0x00},	//Dummy 750us
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x0E, 0x00},	//PLL off
	{0x01, 0xF1}, 
	{0x08, 0x00}, 
	{0x01, 0xF1}, 
	{0x01, 0xF3}, 
	{0x01, 0xF1}, 
	{0x03, 0x20}, 
	{0x10, 0x1C}, 
	{0x03, 0x22}, 
	{0x10, 0x6B}, 
	//PAGE0
	{0x03, 0x00}, 
	{0x10, 0x0C}, 
	{0x11, 0x90}, 
	{0x12, 0x04}, 
	{0x03, 0x00}, 
	{0x0B, 0xAA}, 
	{0x0C, 0x55}, 
	{0x0D, 0xAA}, 
	//window
	{0x20, 0x00}, 
	{0x21, 0x2A}, 
	{0x22, 0x00}, 
	{0x23, 0x0C}, 
	{0x24, 0x03}, 
	{0x25, 0xC0}, 
	{0x26, 0x05}, 
	{0x27, 0x00}, 
	{0x29, 0x13}, 
	{0x38, 0x02}, 
	{0x39, 0x0A}, 
	{0x3A, 0x01}, 
	{0x3B, 0x55},	//pre3 height_l
	{0x3C, 0x05}, 
	{0x3D, 0x00}, 
	{0x40, 0x01}, 
	{0x41, 0x18}, 
	{0x42, 0x00}, 
	{0x43, 0x50}, 
	{0x45, 0x04}, 
	{0x46, 0x28}, 
	{0x47, 0xC8}, 
	//BLC
	{0x80, 0x08}, 
	{0x81, 0x97}, 
	{0x82, 0x90}, 
	{0x83, 0x30}, 
	{0x84, 0xCC}, 
	{0x85, 0x00}, 
	{0x86, 0xD4}, 
	{0x03, 0x00}, 
	{0x90, 0x0C}, 
	{0x91, 0x0C}, 
	{0x92, 0x97}, 
	{0x93, 0x8F}, 
	{0x94, 0x95}, 
	{0x95, 0x90}, 
	{0x98, 0x38}, 
	//Dark
	{0xA0, 0x01},	//20100309  Blue sign[6] offset[5:0]
	{0xA2, 0x01},	//20100309  Gb
	{0xA4, 0x01},	//20100309  Red
	{0xA6, 0x01},	//20100309  Gr
	{0xA8, 0x00},	//Blue
	{0xAA, 0x00},	//Gb
	{0xAC, 0x00},	//Red
	{0xAE, 0x00},	//Gr
	{0x99, 0x43},	//Red
	{0x9A, 0x43},	//Gr
	{0x9B, 0x43},	//Gb
	{0x9C, 0x43},	//Blue
	{0x03, 0x02},	//
	{0x18, 0x4C},	//10825  X2c->X4c
	{0x19, 0x00},	//10825 X00 :: pclk delay X00->Xc0
	{0x1A, 0x39},	//10825
	{0x1C, 0x3C},	//10825
	{0x1D, 0x01},	//10825
	{0x1E, 0x30},	//10825
	{0x1F, 0x10},	//10825
	{0x20, 0x77},	//10825
	{0x21, 0xED},	//10825  adc bias incresment
	{0x22, 0xA7},	//10825
	{0x23, 0x30},	//10825 X32->X30 sunspot anhyunjoo
	{0x24, 0x47},	//10825
	{0x27, 0x34},	//10825
	{0x2B, 0x80},	//10825
	{0x2E, 0x11},	//10825
	{0x2F, 0xA1},	//10825
	{0x30, 0x00},	//10825
	{0x31, 0x99},	//10825
	{0x32, 0x00},	//10825
	{0x33, 0x00},	//10825
	{0x34, 0x22},	//10825
	{0x36, 0x01},	//10825 preview2 mode support
	{0x37, 0x01},	//10825 preview2 mode support
	{0x3D, 0x03},	//10825
	{0x3E, 0x0D},	//10825 preview2 mode support Xb->Xd
	{0x49, 0xD1},	//10825
	{0x50, 0x28},	//10825
	{0x52, 0x01},	//10825 horizon line noise
	{0x53, 0x81},	//10825 preview2 mode suppot
	{0x54, 0x3C},	//10825
	{0x55, 0x1C},	//10825
	{0x56, 0x11},	//10825
	{0x5D, 0xA2},	//10825
	{0x5E, 0x5A},	//10825
	{0x60, 0x6C},	//10825[[
	{0x61, 0x77},	//
	{0x62, 0x6C},	//
	{0x63, 0x75},	//
	{0x64, 0x6C},	//
	{0x65, 0x75},	//
	{0x67, 0x0C},	//
	{0x68, 0x0C},	//
	{0x69, 0x0C},	//
	{0x72, 0x6D},	//
	{0x73, 0x74},	//
	{0x74, 0x6D},	//
	{0x75, 0x74},	//
	{0x76, 0x6D},	//
	{0x77, 0x74},	//
	{0x7C, 0x6B},	//
	{0x7D, 0x84},	//
	{0x80, 0x01},	//
	{0x81, 0x64},	//
	{0x82, 0x1E},	//
	{0x83, 0x2C},	//
	{0x84, 0x63},	//
	{0x85, 0x65},	//
	{0x86, 0x63},	//
	{0x87, 0x65},	//
	{0x92, 0x41},	//
	{0x93, 0x4E},	//
	{0x94, 0x63},	//
	{0x95, 0x65},	//
	{0x96, 0x63},	//
	{0x97, 0x65},	//
	{0xA0, 0x02},	//
	{0xA1, 0x61},	//
	{0xA2, 0x02},	//
	{0xA3, 0x61},	//
	{0xA4, 0x61},	//
	{0xA5, 0x02},	//
	{0xA6, 0x61},	//
	{0xA7, 0x02},	//
	{0xA8, 0x6C},	//
	{0xA9, 0x70},	//
	{0xAA, 0x6C},	//
	{0xAB, 0x70},	//
	{0xAC, 0x1C},	//
	{0xAD, 0x22},	//
	{0xAE, 0x1C},	//
	{0xAF, 0x22},	//
	{0xB0, 0x77},	//
	{0xB1, 0x80},	//
	{0xB2, 0x77},	//
	{0xB3, 0x80},	//
	{0xB4, 0x78},	//
	{0xB5, 0x7F},	//
	{0xB6, 0x78},	//
	{0xB7, 0x7F},	//
	{0xB8, 0x78},	//
	{0xB9, 0x7E},	//
	{0xBA, 0x78},	//
	{0xBB, 0x7E},	//
	{0xBC, 0x79},	//
	{0xBD, 0x7D},	//
	{0xBE, 0x79},	//
	{0xBF, 0x7D},	//
	{0xC4, 0x2E},	//
	{0xC5, 0x3F},	//
	{0xC6, 0x50},	//
	{0xC7, 0x60},	//
	{0xC8, 0x2F},	//
	{0xC9, 0x3E},	//
	{0xCA, 0x2F},	//
	{0xCB, 0x3E},	//
	{0xCC, 0x51},	//
	{0xCD, 0x5F},	//
	{0xCE, 0x51}, 
	{0xCF, 0x5F}, 
	{0xD0, 0x0A}, 
	{0xD1, 0x09}, 
	{0xD2, 0x20}, 
	{0xD3, 0x00}, 
	{0xD4, 0x0C}, 
	{0xD5, 0x0C}, 
	{0xD6, 0x97}, 
	{0xD7, 0x8F}, 
	{0xE0, 0xE1}, 
	{0xE1, 0xE1}, 
	{0xE2, 0xE1}, 
	{0xE3, 0xE1}, 
	{0xE4, 0xE1}, 
	{0xE5, 0x01}, 
	{0xE9, 0x00}, 
	{0xEA, 0x64}, 
	// PAGE10
	{0x03, 0x10}, 
	{0x10, 0x01}, 
	{0x12, 0x10}, 
	{0x13, 0x02},	//contrast effet enable : X02
	{0x34, 0x00},	//hidden 10->00 100209
	{0x37, 0x01},	//yc2d power save
	{0x3F, 0x04},	//100825
	{0x40, 0x80},	//Y offset
	{0x48, 0x86},	//Contrast (Y = constrast * (Y - 128) + 128)
	{0x53, 0x00},	//dif_offset option
	{0x55, 0x30},	//dif_offset option  diff_offset max
	{0x60, 0x83},	//out color sat en[7] | auto color decrement en[1] | manual color sat en[0]
	{0x61, 0x00},	//sat ctrl2
	{0x62, 0xB0},	//blue saturation_C0
	{0x63, 0xB0},	//red saturation_B0
	{0x64, 0xFF},	//auto decresment on AG th
	{0x65, 0xFF},	//auto decresment on DG th
	{0x66, 0xE4},	//Outdoor saturation step 137fps apply out th
	{0x67, 0x42},	//Outdoor saturation B/R
	{0x03, 0x11},	//
	{0x10, 0x3F},	//B[6]:Blue En  Dlpf on[4:0] Sky over off : X7f->3f
	{0x11, 0x20},	// Uniform Full GbGr/OV-Nr
	{0x12, 0x80},	//Blue MaxOpt  blue sky max filter optoin rate : 0 Xc0->80
	{0x13, 0xB8},	//dark2[7] | dark2 maxfilter ratio[6:4] | dark3[3] | dark3 maxfilter ratio[2:0]
	{0x30, 0xBA},	//Outdoor2 H th
	{0x31, 0x10},	//Outdoor2 L th
	{0x32, 0x40},	//Outdoor2 gain ratio
	{0x33, 0x1A},	//Outdoor2 H lum
	{0x34, 0x15},	//Outdoor2 M lum
	{0x35, 0x1D},	//Outdoor2 L lum
	{0x36, 0xB0},	//Outdoor1 H th
	{0x37, 0x18},	//Outdoor1 L th
	{0x38, 0x40},	//Outdoor1 gain ratio  X80->40
	{0x39, 0x1A},	//Outdoor1 H lum       X28->1e
	{0x3A, 0x15},	//Outdoor1 M lum       X10->15
	{0x3B, 0x1D},	//Outdoor1 L lum       X08->20
	{0x3C, 0x4A},	//indoor H th
	{0x3D, 0x16},	//indoor L th
	{0x3E, 0x44},	//indoor gain ratio    X44  6a
	{0x3F, 0x13},	//indoor H lum         X12  18
	{0x40, 0x19},	//indoor M lum	       X18  1c
	{0x41, 0x18},	//indoor L lum         X18  3e
	{0x42, 0x98},	//dark1 H th
	{0x43, 0x28},	//dark1 L th
	{0x44, 0x65},	//dark1 gain ratio
	{0x45, 0x16},	//dark1 H lum         X38->X28
	{0x46, 0x30},	//dark1 M lum         X27->X17
	{0x47, 0x34},	//dark1 L lum         X20->X1a
	{0x48, 0x90},	//dark2 H th
	{0x49, 0x2A},	//dark2 L th
	{0x4A, 0x65},	//dark2 gain ratio
	{0x4B, 0x18},	//dark2 H lum
	{0x4C, 0x31},	//dark2 M lum
	{0x4D, 0x36},	//dark2 L lum
	{0x4E, 0x80},	//dark3 H th
	{0x4F, 0x30}, 
	{0x50, 0x65}, 
	{0x51, 0x19}, 
	{0x52, 0x31}, 
	{0x53, 0x36}, 
	{0x5A, 0x3F}, 
	{0x5B, 0x00}, 
	{0x5C, 0x9F}, 
	{0x60, 0x3F}, 
	{0x62, 0x0F}, 
	{0x63, 0x25}, 
	{0x64, 0x10}, 
	{0x65, 0x0C}, 
	{0x66, 0x0C},	//Indoor GbGr  rate H 100% M 25% L 100%
	{0x67, 0x00},	//dark GbGr    rate H/M/L  100%
	{0x70, 0x0C},	// Abberation On/Off B[1]: Outdoor B[0]: Indoor 07>>c
	{0x75, 0xA0},	// Outdoor2 Abberation Luminance lvl
	{0x7D, 0xB4},	// Indoor Abberation Luminance   lvl
	{0x96, 0x08},	//indoor/Dark1 edgeoffset1
	{0x97, 0x14},	//indoor/Dark1 center G value
	{0x98, 0xF5},	//slope indoor :: left/right graph polarity, slope
	{0x99, 0x2A},	//indoor uncertain ratio control
	{0x9A, 0x20},	//Edgeoffset_dark
	{0x03, 0x12},	//
	{0x20, 0x17},	//Yc2d ctrl1 Preview Yc2d    off[X16] on[X17]
	{0x21, 0x0F},	//Yc2d ctrl2 Preview Cfilter off[X0e] on[X0f]
	{0x22, 0x06},	//Yc2d ctrl3 C filter mask outdoor1 | Indoor
	{0x23, 0x00},	//
	{0x24, 0x59},	//
	{0x25, 0x00},	//2010.8.25 bit[5] off -> sharpness pre block off
	{0x2A, 0x01},	//
	{0x2E, 0x00},	//2010.8.25
	{0x30, 0x35},	//Texture region(most detail)
	{0x31, 0xA0},	//STD uniform1 most blur region
	{0x32, 0xB0},	//STD uniform2      2nd blur
	{0x33, 0xC0},	//STD uniform3      3rd blur
	{0x34, 0xD0},	//STD normal noise1 4th blur
	{0x35, 0xE0},	//STD normal noise2 5th blur
	{0x36, 0xFF},	//STD normal noise3 6th blur
	{0x3B, 0x06},	//
	{0x3C, 0x06},	//
	{0x40, 0x83},	//Outdoor2 H th
	{0x41, 0x20},	//Outdoor2 L th
	{0x42, 0x0A},	//Outdoor2 H luminance
	{0x43, 0x13},	//Outdoor2 M luminance
	{0x44, 0x10},	//Outdoor2 l luminance
	{0x45, 0x40},	//Outdoor2 ratio
	{0x46, 0x83},	//Outdoor1 H th
	{0x47, 0x20},	//Outdoor1 L th
	{0x48, 0x0A},	//Outdoor1 H luminance
	{0x49, 0x13},	//Outdoor1 M luminance
	{0x4A, 0x10},	//Outdoor1 L luminance
	{0x4B, 0x40},	//Outdoor1 ratio
	{0x4C, 0x80},	//Indoor H th
	{0x4D, 0x48},	//Indoor L th
	{0x4E, 0x11},	//indoor H lum
	{0x4F, 0x12},	//indoor M lum
	{0x50, 0x10},	//indoor L lum
	{0x51, 0x45},	//indoor ratio  X10 -> X45
	{0x52, 0xA8},	//dark1 H th
	{0x53, 0x30},	//dark1 L th
	{0x54, 0x28},	//dark1 H lum
	{0x55, 0x3E},	//dark1 M lum
	{0x56, 0x67},	//dark1 L lum
	{0x57, 0x70},	//dark1 ratio
	{0x58, 0xA0},	//dark2 H th
	{0x59, 0xA0},	//dark2 L th
	{0x5A, 0x28},	//dark2 H lum
	{0x5B, 0x3F},	//dark2 M lum
	{0x5C, 0x68},	//dark2 L lum
	{0x5D, 0x70},	//dark2 ratio
	{0x5E, 0xA0},	//dark3 H th
	{0x5F, 0x40},	//dark3 L th
	{0x60, 0x29},	//dark3 H lum
	{0x61, 0x3F},	//dark3 M lum
	{0x62, 0x69},	//dark3 L lum
	{0x63, 0x70},	//dark3 ratio
	{0x70, 0x10},	//
	{0x71, 0x0A},	//
	{0x72, 0x10},	//
	{0x73, 0x0A},	//
	{0x74, 0x18},	//
	{0x75, 0x12},	//
	{0x90, 0x5D},	//Preview DPC off[X5c] on[X5d]
	{0xAD, 0x07},	//10825
	{0xAE, 0x07},	//10825
	{0xAF, 0x07},	//10825
	{0xC5, 0x23},	//BlueRange   2010.8.25    X40->23
	{0xC6, 0x20},	//GreenRange  2010.8.25    X3b->20
	{0xD0, 0x08},	//2010.8.25
	{0xDB, 0x38},	//20110307 18->38
	{0xD9, 0x04},	//strong_edge detect ratio
	{0x03, 0x13},	//
	{0x10, 0xCB},	//
	{0x11, 0x7B},	//
	{0x12, 0x0E},	//07
	{0x14, 0x00},	//
	{0x15, 0x11},	//added option 1.3M Ãß°¡
	{0x20, 0x15},	//
	{0x21, 0x13},	//
	{0x22, 0x33},	//
	{0x23, 0x04},	//
	{0x24, 0x09},	//
	{0x25, 0x0A},	//
	{0x26, 0x18},	// w_gain 20->18
	{0x27, 0x30},	//
	{0x29, 0x12},	//
	{0x2A, 0x50},	//
	{0x2B, 0x06},	//
	{0x2C, 0x06},	//
	{0x25, 0x08},	//
	{0x2D, 0x0C},	//
	{0x2E, 0x12},	//
	{0x2F, 0x12},	//
	{0x50, 0x08},	//out2  hi nega
	{0x53, 0x06},	//      hi pos
	{0x51, 0x07},	//      mi nega
	{0x54, 0x06},	//      mi pos
	{0x52, 0x07},	//      lo nega
	{0x55, 0x05},	//      lo pos
	{0x56, 0x08},	//out1   hi nega
	{0x59, 0x06},	//       hi pos
	{0x57, 0x07},	//       mi nega
	{0x5A, 0x06},	//       mi pos
	{0x58, 0x07},	//       lo nega
	{0x5B, 0x05},	//       lo pos
	{0x5C, 0x08},	//0a
	{0x5D, 0x0B},	//
	{0x5E, 0x0A},	//
	{0x5F, 0x07},	//08
	{0x60, 0x09},	//
	{0x61, 0x08},	//
	{0x62, 0x08},	//
	{0x63, 0x08},	//
	{0x64, 0x08},	//
	{0x65, 0x06},	//
	{0x66, 0x06},	//
	{0x67, 0x06},	//
	{0x68, 0x07},	//
	{0x69, 0x07},	//
	{0x6A, 0x07},	//
	{0x6B, 0x05},	//
	{0x6C, 0x05},	//
	{0x6D, 0x05},	//
	{0x6E, 0x07},	//
	{0x6F, 0x07},	//
	{0x70, 0x07},	//
	{0x71, 0x05},	//
	{0x72, 0x05},	//
	{0x73, 0x05},	//
	{0x80, 0xC1},	//fd
	{0x81, 0x1F},	//
	{0x82, 0x11},	//05
	{0x83, 0x11},	//41
	{0x84, 0x86},	//Lpf gaussian coef out2[7:6]out1[5:4]in[3:2]dark[1:0] //1.3M Ãß°¡
	{0x90, 0x05},	//
	{0x91, 0x05},	//
	{0x92, 0x33},	//
	{0x93, 0x30},	//
	{0x94, 0x03},	//
	{0x95, 0x14},	//
	{0x97, 0x30},	//
	{0x99, 0x30},	//
	{0xA0, 0x04},	//
	{0xA1, 0x05},	//
	{0xA2, 0x04},	//
	{0xA3, 0x05},	//
	{0xA4, 0x07},	//
	{0xA5, 0x08},	//
	{0xA6, 0x07},	//
	{0xA7, 0x08},	//
	{0xA8, 0x07},	//
	{0xA9, 0x08},	//
	{0xAA, 0x07},	//
	{0xAB, 0x08},	//
	{0xB0, 0x0D},	//out2   H Ne
	{0xB3, 0x0C},	//       H Po
	{0xB1, 0x0B},	//       M Ne
	{0xB4, 0x09},	//       M Po
	{0xB2, 0x0B},	//       L Ne
	{0xB5, 0x09},	//       L Po
	{0xB6, 0x0D},	//out1   H Ne   Xd->18
	{0xB9, 0x0C},	//       H Po   X8->16
	{0xB7, 0x0B},	//       M Ne   Xd->15
	{0xBA, 0x09},	//       M Po   X8->17
	{0xB8, 0x0B},	//       L Ne   Xd->13
	{0xBB, 0x09},	//       L Po   X8->11
	{0xBC, 0x20},	//25
	{0xBD, 0x2A},	//
	{0xBE, 0x2B},	//27
	{0xBF, 0x1E},	//25
	{0xC0, 0x28},	//2a
	{0xC1, 0x2A},	//27
	{0xC2, 0x1E},	//
	{0xC3, 0x24},	//
	{0xC4, 0x20},	//
	{0xC5, 0x1E},	//
	{0xC6, 0x24},	//
	{0xC7, 0x20},	//
	{0xC8, 0x18},	//
	{0xC9, 0x20},	//
	{0xCA, 0x1E},	//
	{0xCB, 0x18},	//
	{0xCC, 0x20},	//
	{0xCD, 0x1E},	//
	{0xCE, 0x18},	//
	{0xCF, 0x20},	//
	{0xD0, 0x1E},	//
	{0xD1, 0x18},	//
	{0xD2, 0x20},	//
	{0xD3, 0x1E},	//
	{0x03, 0x14},	//
	{0x10, 0x11},	//
	{0x20, 0x80},	// x center gg
	{0x21, 0x80},	// y center gg
	{0x22, 0x80},	// x center rr
	{0x23, 0x80},	// y center rr
	{0x24, 0x80},	// x center bb
	{0x25, 0x80},	// y center bb
	{0x28, 0x60},	//
	{0x29, 0x60},	//
	{0x40, 0x61},	//20110307 shading
	{0x41, 0x43},	//
	{0x42, 0x3D},	//
	{0x43, 0x43},	//
	{0x03, 0x15},	//
	{0x10, 0x21},	//
	{0x14, 0x3E},	//CMCOFSGH_3D
	{0x15, 0x2E},	//CMCOFSGM_30
	{0x16, 0x28},	//CMCOFSGL
	{0x17, 0x2F},	//CMC SIGN
	{0x30, 0xAD},	//
	{0x31, 0x32},	//
	{0x32, 0x05},	//
	{0x33, 0x1C},	//
	{0x34, 0xA9},	//
	{0x35, 0x0D},	//
	{0x36, 0x01},	//
	{0x37, 0x36},	//
	{0x38, 0xB7},	//
	{0x40, 0x8C},	//
	{0x41, 0x08},	//
	{0x42, 0x04},	//
	{0x43, 0x02},	//
	{0x44, 0x08},	//
	{0x45, 0x8A},	//
	{0x46, 0x94},	//
	{0x47, 0x8C},	//
	{0x48, 0x20},	//
	{0x50, 0x01},	//
	{0x51, 0x00},	//
	{0x52, 0x81},	//
	{0x53, 0x11},	//
	{0x54, 0x00},	//
	{0x55, 0x91},	//
	{0x56, 0x00},	//
	{0x57, 0x90},	//
	{0x58, 0x10},	//
	{0x80, 0x00},	//
	{0x85, 0x80},	//
	{0x03, 0x16},	//
	{0x10, 0x31},	//GMA_CTL
	{0x18, 0x7E},	//AG_ON
	{0x19, 0x7D},	//AG_OFF
	{0x1A, 0x0E},	//TIME_ON
	{0x1B, 0x01},	//TIME_OFF
	{0x1C, 0xDC},	//OUT_ON
	{0x1D, 0xFE},	//OUT_OFF
	{0x30, 0x00},	//
	{0x31, 0x0B},	//
	{0x32, 0x1D},	//
	{0x33, 0x36},	//
	{0x34, 0x58},	//
	{0x35, 0x7A},	//
	{0x36, 0x93},	//
	{0x37, 0xA7},	//
	{0x38, 0xB6},	//
	{0x39, 0xC3},	//
	{0x3A, 0xCC},	//
	{0x3B, 0xD6},	//
	{0x3C, 0xDF},	//
	{0x3D, 0xE7},	//
	{0x3E, 0xED},	//
	{0x3F, 0xF2},	//
	{0x40, 0xF7},	//
	{0x41, 0xFB},	//
	{0x42, 0xFF},	//
	{0x50, 0x00},	//
	{0x51, 0x03},	//
	{0x52, 0x19},	//
	{0x53, 0x34},	//
	{0x54, 0x58},	//
	{0x55, 0x75},	//
	{0x56, 0x8D},	//
	{0x57, 0xA1},	//
	{0x58, 0xB2},	//
	{0x59, 0xBE},	//
	{0x5A, 0xC9},	//
	{0x5B, 0xD2},	//
	{0x5C, 0xDB},	//
	{0x5D, 0xE3},	//
	{0x5E, 0xEB},	//
	{0x5F, 0xF0},	//
	{0x60, 0xF5},	//
	{0x61, 0xF7},	//
	{0x62, 0xF8},	//
	{0x70, 0x07},	//
	{0x71, 0x18},	//
	{0x72, 0x21},	//
	{0x73, 0x38},	//
	{0x74, 0x52},	//
	{0x75, 0x6C},	//
	{0x76, 0x85},	//
	{0x77, 0x9A},	//
	{0x78, 0xAD},	//
	{0x79, 0xBD},	//
	{0x7A, 0xCB},	//
	{0x7B, 0xD6},	//
	{0x7C, 0xE0},	//
	{0x7D, 0xE8},	//
	{0x7E, 0xEF},	//
	{0x7F, 0xF4},	//
	{0x80, 0xF9},	//
	{0x81, 0xFC},	//
	{0x82, 0xFF},	//
	{0x03, 0x17},	//
	{0x10, 0x05},	//en_cnt_check[4] | en_histogra,[0]
	{0x11, 0x0A},	//
	{0x12, 0x60},	//X40
	{0xC4, 0x4B},	//FLK200
	{0xC5, 0x3E},	//FLK240
	{0x03, 0x18},	//
	{0x10, 0x00},	//scaler off
	{0x81, 0xE4}, 
	{0x82, 0xE4}, 
	{0x83, 0x4E}, 
	{0x84, 0x4E}, 
	//PAGE2
	{0x03, 0x20}, 
	{0x10, 0x1C}, 
	{0x11, 0x1C}, 
	{0x1A, 0x04}, 
	{0x20, 0x01}, 
	{0x21, 0x18}, 
	{0x22, 0x00}, 
	{0x27, 0x2A}, 
	{0x28, 0xBF},	//
	{0x29, 0x0D},	//100825  2M :: X0d
	{0x2A, 0xF0},	//
	{0x2B, 0xF4},	//(2/120 flicker disable)
	{0x30, 0xF8},	//Speed_Hunting Option_f8 //X78 ->Xf8 20110221
	{0x37, 0x22},	//20110225 hunting X28->X24->X22
	{0x3E, 0x20},	//20110225 hunting
	{0x39, 0x22},	//
	{0x3A, 0xDE},	//
	{0x3B, 0x22},	//
	{0x3C, 0xDE},	//
	{0x2F, 0x00},	//
	{0x43, 0xC0},	//
	{0x56, 0x28},	//
	{0x57, 0x78},	//
	{0x58, 0x20},	//
	{0x59, 0x60},	//
	{0x5E, 0x9F},	// hsync inc size 1.3mega X9f
	{0x5F, 0x77},	// vsync inc size 1.3mega 960 X77
	{0x60, 0x55},	//
	{0x61, 0x55},	//
	{0x62, 0x55},	//
	{0x63, 0x55},	//
	{0x64, 0x56},	//55
	{0x65, 0x95},	//55
	{0x66, 0x57},	//
	{0x67, 0xD5},	//
	{0x68, 0x5F},	//
	{0x69, 0xF5},	//
	{0x6A, 0x7F},	//
	{0x6B, 0xFD},	//
	{0x6C, 0x7F},	//
	{0x6D, 0xFD},	//
	{0x6E, 0x55},	//
	{0x6F, 0x55},	//
	{0x70, 0x36},	//30
	{0x71, 0x83},	//00_3*4=12
	{0x76, 0x22},	//21 //X11->X22 20110221
	{0x77, 0x81},	//81
	{0x78, 0x22},	//Y-TH1
	{0x79, 0x27},	//Y-TH2  24 //X26->X27 20110221
	{0x7A, 0x22}, 
	{0x7B, 0x22}, 
	{0x7D, 0x22}, 
	{0x7E, 0x22}, 
	//Pll 2
	{0x83, 0x01}, 
	{0x84, 0x5F}, 
	{0x85, 0x90}, 
	{0x86, 0x01}, 
	{0x87, 0x90}, 
	{0x88, 0x05}, 
	{0x89, 0x7E}, 
	{0x8A, 0x40}, 
	{0x8B, 0x75}, 
	{0x8C, 0x30}, 
	{0x8D, 0x61}, 
	{0x8E, 0xA8}, 
	{0x91, 0x02}, 
	{0x92, 0xFC}, 
	{0x93, 0xD8},	//
	{0x94, 0x02},	//
	{0x95, 0x7A}, 
	{0x96, 0xC4}, 
	{0x98, 0x9E}, 
	{0x99, 0x46}, 
	{0x9B, 0x9A}, 
	{0x9A, 0x28}, 
	{0x9C, 0x16}, 
	{0x9D, 0xA8}, 
	{0x9E, 0x01}, 
	{0x9F, 0x90}, 
	{0xA0, 0x00}, 
	{0xA1, 0xC3}, 
	{0xA2, 0x50}, 
	//Exp12
	{0xA3, 0x00},	//Antibandtime/256=120*256=30720fps
	{0xA4, 0x30},	//
	{0xA5, 0x01},	//Capture ExpmaxH
	{0xA6, 0x6E},	//Capture ExpmaxM
	{0xA7, 0x36},	//Capture ExpmaxL
	{0xA8, 0x1D},	//Capture Exp100H
	{0xA9, 0x4C},	//Capture Exp100L
	{0xAA, 0x18},	//Capture Exp120H //PLL 2x ³·Ãè´Ù°í °¡Á¤.
	{0xAB, 0x6A},	//Capture Exp120L
	{0xAC, 0x00},	//Capture Exp12000H
	{0xAD, 0x18},	//Capture Exp12000L
	{0xAE, 0x23},	//Capture Outtime th2
	{0xB0, 0x10},	//AG
	{0xB1, 0x10},	//AGMIN
	{0xB2, 0x9F},	//AGMAX
	{0xB3, 0x1A},	//AGLVL
	{0xB4, 0x1A},	//AGTH1
	{0xB5, 0x44},	//AGTH2
	{0xB6, 0x2F}, 
	{0xB7, 0x28}, 
	{0xB8, 0x25}, 
	{0xB9, 0x22}, 
	{0xBA, 0x21}, 
	{0xBB, 0x20}, 
	{0xBC, 0x38}, 
	{0xBD, 0x37}, 
	{0xBE, 0x36}, 
	{0xBF, 0x36}, 
	{0xC0, 0x35}, 
	{0xC1, 0x35}, 
	{0xC2, 0x30}, 
	{0xC3, 0x30}, 
	{0xC4, 0x30}, 
	{0xC5, 0x08}, 
	{0xC6, 0x48}, 
	{0xCC, 0x48}, 
	{0xCA, 0xE0}, 
	{0xC7, 0x18}, 
	{0xC8, 0xA0}, 
	{0xC9, 0x80}, 
	{0x03, 0x22},	//
	{0x10, 0x6B},	//
	{0x11, 0x2E},	//must be X2e
	{0x18, 0x00},	//uniform_on  X01
	{0x1A, 0x54},	//r_max
	{0x1B, 0x2E},	//r_min
	{0x1C, 0x44},	//b_max
	{0x1D, 0x2E},	//b_min
	{0x1E, 0x04},	//time_th
	{0x1F, 0x60},	//time_th
	{0x19, 0x41},	//Low temp off X40 -> Low temp on X41
	{0x20, 0x28},	//AWB unstable speed  fast X28 slow X0d
	{0x21, 0x00},	//skin color on
	{0x22, 0x00},	//
	{0x7A, 0xD6},	//
	{0x23, 0x40},	//
	{0x24, 0x07},	//
	{0x25, 0x3C},	//
	{0x26, 0x00},	//
	{0x27, 0x0B},	//6b
	{0x28, 0x66},	//
	{0x29, 0x00},	// method 1 //10
	{0x2A, 0x46},	//LG yangsan X0a->X46
	{0x2B, 0x00},	//
	{0x2C, 0x00},	//
	{0x2D, 0x7A},	//
	{0x2E, 0xBF},	//LG yangsan ff->bf
	{0x2F, 0x00},	//lowtemp crcb method X00 old
	{0x30, 0x80},	//
	{0x31, 0x80},	//
	{0x34, 0x18},	//
	{0x35, 0x0B},	//
	{0x36, 0x2A},	//
	{0x37, 0x3A},	//
	{0x38, 0x11},	//12
	{0x39, 0x66},	//
	{0x46, 0x02},	//
	{0x4D, 0xD4},	//awb stable y th
	{0x4E, 0x0A},	//
	{0x42, 0x22},	//csum2
	{0x50, 0x22},	//csum4
	{0x41, 0x42},	//cdiff
	{0x40, 0xF6},	//yth	//Xf2->Xf6
	{0x45, 0x44},	//csum2
	{0x51, 0x34},	//csum4
	{0x44, 0x44},	//cdiff
	{0x43, 0xF2},	//yth
	{0x49, 0x34},	//csum2
	{0x52, 0x23},	//csum4
	{0x48, 0x53},	//cdiff
	{0x47, 0xF3},	//yth
	{0x4C, 0x34},	//csum2
	{0x53, 0x23},	//csum4
	{0x4B, 0x63},	//cdiff  X43
	{0x4A, 0xE6},	//yth   X84->Xe6
	{0x54, 0x00},	//
	{0x55, 0xA5},	//sign
	{0x56, 0x11},	//ofs
	{0x57, 0x55},	//ofs
	{0x5A, 0xE0},	//
	{0x5B, 0xF0},	//
	{0x5C, 0x60},	//
	{0x5D, 0x14},	//
	{0x5E, 0x40},	//
	{0x5F, 0x00},	//
	{0x60, 0xF8},	//
	{0x61, 0x64},	//
	{0x62, 0x10},	//
	{0x63, 0xF8},	//
	{0x64, 0x20},	//
	{0x65, 0x22},	//D1/D3 valid color th
	{0x66, 0x44},	//D2/D4 valid color th
	{0x67, 0x14},	//
	{0x68, 0x14},	//
	{0x69, 0x14},	//
	{0x6A, 0x14},	//
	{0x6B, 0x00},	//
	{0x6C, 0x00},	//
	{0x6D, 0x40},	//
	{0x6E, 0x50},	//
	{0x6D, 0x40},	//
	{0x6E, 0x50},	//
	{0x6F, 0x30},	//
	{0x70, 0x40},	//
	{0x71, 0x50},	//
	{0x72, 0x30},	//
	{0x73, 0x88},	//
	{0x74, 0x40},	//
	{0x75, 0x50},	//
	{0x76, 0x30},	//
	{0x77, 0x88},	//
	{0xE1, 0x22},	//
	{0x80, 0x38},	//r gain
	{0x81, 0x20},	//g gain
	{0x82, 0x36},	//b gain
	{0x83, 0x54},	//Normal R max
	{0x84, 0x1F},	//Normal R min
	{0x85, 0x53},	//Normal B max
	{0x86, 0x1F},	//Normal B mim
	{0x87, 0x4E},	//middle R max        //
	{0x88, 0x36},	//middle R min  39_ÀÜµðGain l
	{0x89, 0x36},	//middle B max  30_ÀÜµðGain l
	{0x8A, 0x25},	//middle B min
	{0x8B, 0x3D},	//bottom R max //42
	{0x8C, 0x34},	//bottom R min
	{0x8D, 0x34},	//bottom B max  //X34
	{0x8E, 0x2E},	//bottom B min //2a
	{0x8F, 0x53},	//p1
	{0x90, 0x51},	//p2
	{0x91, 0x4E},	//p3
	{0x92, 0x48},	//p4
	{0x93, 0x42},	//p5
	{0x94, 0x3A},	//p6
	{0x95, 0x32},	//p7
	{0x96, 0x25},	//p8
	{0x97, 0x19},	//p9
	{0x98, 0x17},	//p10
	{0x99, 0x15},	//p11
	{0x9A, 0x13},	//p12
	{0x9B, 0x89},	//56 band12   X77->ff  ÇÑµµ
	{0x9C, 0x88},	//76 band34   X77->ff
	{0x9D, 0x4F},	//R_bandth 1
	{0x9E, 0x30},	//R_bandth 2
	{0x9F, 0x2C},	//R_bandth 3
	{0xA0, 0x25},	//R delta 1
	{0xA1, 0x32},	//B delta 1
	{0xA2, 0x32},	//R delta 23
	{0xA3, 0x21},	//B delta 2
	{0xA4, 0x10},	//R_TH1	1920fps  Xa->X10
	{0xA5, 0x35},	//R_TH2 529fps -> 578fps X3a->X35
	{0xA6, 0x70},	//R_TH3
	{0xA7, 0x00},	//low temp R min
	{0xA8, 0x18},	//low temp R max
	{0xAD, 0x38},	//
	{0xAE, 0x30},	//
	{0xAF, 0x32},	//
	{0xB0, 0x2C},	//
	{0xB1, 0x00},	//lowtemp y control X28->X00
	{0xB2, 0x48},	//
	{0xB3, 0x40},	//
	{0xB5, 0x7F},	//
	{0xB6, 0x00},	//
	{0xB7, 0x91},	//low Cb,Cr target offset X09->0
	{0xB8, 0x2C},	//Low temp off
	{0xB9, 0x29},	//Low temp On
	{0xBC, 0x00},	//Low gain cb/cb retarget point
	{0xBD, 0x36},	//Low gain 80/80 point
	{0x03, 0x24},	//Resol control
	{0x60, 0x4F},	//edge even frame | 16bit resol | white edge cnt | scene resol enable
	{0x61, 0x04},	//even frame update
	{0x64, 0x35},	//edge th1 H
	{0x65, 0x00},	//edge th1 L
	{0x66, 0x26},	//edge th2 H
	{0x67, 0x00},	//edge th2 L
	{0x03, 0x13},	//
	{0x74, 0x02},	//det slope en | gausian filter
	{0x75, 0x09},	//	//1D negative gain det
	{0x76, 0x09},	//	//1//D postive  gain det
	{0x77, 0x0C},	//	//1//D hclp2 det
	{0xF8, 0x0D},	//sharp2d resl coef_L => coef_h_en | slope en
	{0xF9, 0x30},	//	//2//D negative gain
	{0xFA, 0x30},	//2//D postive  gain
	{0xFB, 0x1E},	//	//2D hclp2
	{0xFC, 0x37},	//	//2D Hi  frequence Gaussian coef
	{0xFD, 0x20},	//	//2D Hi  frequence th
	{0xFE, 0x08}, 
	{0x03, 0x12}, 
	{0x7B, 0x10}, 
	{0x7C, 0x04}, 
	{0x7D, 0x04}, 
	{0x03, 0x20}, 
	{0x10, 0x9C}, 
	{0x03, 0x22}, 
	{0x10, 0xEB}, 
	{0x03, 0x00}, 
	{0x0E, 0x03}, 
	{0x0E, 0x73}, 
	{0x03, 0x00}, 
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00},	//
	{0x03, 0x00}, 
	{0x03, 0x00}, 
	{0x03, 0x00}, 
	{0x03, 0x00}, 
	{0x01, 0xF0}, 
	//p100	//Delay 100ms
	{SR130PC10_REG_TERM, 0x64},
	
	{0xFF, 0xFF},	



// Intialize_End

    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};


static struct sensor_reg sensor_set_preview_sr130pc10[] = {

    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_set_capture_sr130pc10[] = {

    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

 struct sensor_reg* sensor_reg_common_sr130pc10[3] =
{
	//sensor_initialize_sr130pc10_max27fps,
	sensor_initialize_sr130pc10,
	sensor_set_preview_sr130pc10,
	sensor_set_capture_sr130pc10
};

static struct sensor_reg sensor_brightness_0_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_brightness_1_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_brightness_2_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_brightness_3_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_brightness_4_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_brightness_sr130pc10[5] =
{
	sensor_brightness_0_sr130pc10,
	sensor_brightness_1_sr130pc10,
	sensor_brightness_2_sr130pc10,
	sensor_brightness_3_sr130pc10,
	sensor_brightness_4_sr130pc10
};


static struct sensor_reg sensor_awb_auto_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_awb_daylight_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_awb_incandescent_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_awb_fluorescent_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_awb_cloudy_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_awb_sunset_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_awb_sr130pc10[6] =
{
	sensor_awb_auto_sr130pc10,
	sensor_awb_daylight_sr130pc10,
	sensor_awb_incandescent_sr130pc10,
	sensor_awb_fluorescent_sr130pc10,
	sensor_awb_cloudy_sr130pc10,
	sensor_awb_sunset_sr130pc10
	
};


static struct sensor_reg sensor_iso_auto_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_iso_100_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_iso_200_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_iso_400_sr130pc10[] = {
	{SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_iso_sr130pc10[4] =
{
	sensor_iso_auto_sr130pc10,
	sensor_iso_100_sr130pc10,
	sensor_iso_200_sr130pc10,
	sensor_iso_400_sr130pc10
};


static struct sensor_reg sensor_effect_normal_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_effect_gray_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_effect_negative_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_effect_sepia_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_effect_sharpness_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_effect_sketch_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_effect_sr130pc10[6] =
{
	sensor_effect_normal_sr130pc10,
	sensor_effect_gray_sr130pc10,
	sensor_effect_negative_sr130pc10,
	sensor_effect_sepia_sr130pc10,
	sensor_effect_sharpness_sr130pc10,
	sensor_effect_sketch_sr130pc10,
};


static struct sensor_reg sensor_reg_flipnone_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_reg_hflip_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_reg_vflip_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_reg_hvflip_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_flip_sr130pc10[4] =
{
	sensor_reg_flipnone_sr130pc10,
	sensor_reg_hflip_sr130pc10,
	sensor_reg_vflip_sr130pc10,
	sensor_reg_hvflip_sr130pc10,
};


static struct sensor_reg sensor_secne_auto_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_secne_night_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_secne_landscape_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_secne_portrait_sr130pc10[] = {
	{SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_secne_sport_sr130pc10[] = {
	{SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_scene_sr130pc10[5] =
{
	sensor_secne_auto_sr130pc10,
	sensor_secne_night_sr130pc10,
	sensor_secne_landscape_sr130pc10,
	sensor_secne_portrait_sr130pc10,
	sensor_secne_sport_sr130pc10
};

static struct sensor_reg sensor_me_mtrix_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_me_center_weighted_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_me_spot_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_metering_exposure_sr130pc10[3] =
{
	sensor_me_mtrix_sr130pc10,
	sensor_me_center_weighted_sr130pc10,
	sensor_me_spot_sr130pc10
};

static struct sensor_reg sensor_af_single_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

static struct sensor_reg sensor_af_manual_sr130pc10[] = {
    {SR130PC10_REG_TERM, SR130PC10_VAL_TERM}
};

struct sensor_reg* sensor_reg_af_sr130pc10[2] =
{
	sensor_af_single_sr130pc10,
	sensor_af_manual_sr130pc10
};

static int write_regs_sr130pc10(const struct sensor_reg reglist[])
{
	int err;
	int err_cnt = 0;	
	int sleep_cnt = 100;
	unsigned char data[132];
	unsigned char bytes;
	const struct sensor_reg *next = reglist;
	
	while (!((next->reg == SR130PC10_REG_TERM) && (next->val == SR130PC10_VAL_TERM)))
	{
		
		if(next->reg == SR130PC10_REG_TERM && next->val != SR130PC10_VAL_TERM)
		{
			//mdelay(next->val);
			msleep(next->val);
			sleep_cnt = 100;
			//printk("Sensor init Delay[%d]!!!! \n", next->val);
			next++;
		}
		else
		{
			/*sleep_cnt--;
			if(sleep_cnt == 0)
			{
				msleep(10);
				sleep_cnt = 100;
			}*/
			
			bytes = 0;
			data[bytes++] = (unsigned char)(next->reg) & 0xff; 	
			data[bytes]   = (unsigned char)(next->val) & 0xff;
			
	        err = DDI_I2C_Write(data, 1, 1);
			if (err)
			{
				err_cnt++;
				if(err_cnt >= 3)
				{
					printk("ERROR: Sensor I2C !!!! \n"); 
					return err;
				}
			}
			else
			{
				err_cnt = 0;
				next++;
			}
		}
	}

	return 0;
}

static int sensor_open_sr130pc10(bool bChangeCamera)
{
	if(bChangeCamera){
		CIF_Open();
		sensor_delay(40);
		
		sensor_reset_low();
		sensor_powerdown_disable();
		msleep(10);

		sensor_reset_high();
		msleep(10);
	}
	else{
		sensor_power_disable();
		sensor_delay(10);
		
		sensor_power_enable();
		sensor_delay(10);

		sensor_powerdown_enable();
		sensor_delay(10);

		sensor_reset_low();
		sensor_delay(10);

		CIF_Open();
		sensor_delay(40);

		sensor_reset_high();
		sensor_delay(15);
	}

	return write_regs_sr130pc10(sensor_reg_common_sr130pc10[0]);
}

static int sensor_close_sr130pc10(void)
{
	CIF_ONOFF(OFF);

	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_disable();

	CIF_Close();
	msleep(5);

	//printk("sr130pc10 close!\n");

	return 0;
}

static int sensor_powerdown_sr130pc10(bool bChangeCamera)
{		

	if(bChangeCamera){
		CIF_Open();
		msleep(10);
		sensor_reset_low();
		msleep(10);
		sensor_powerdown_enable();
		msleep(10);

		//printk("sr130pc10 change camera powerdown! \n");
	}
	else{
		sensor_reset_low();
		sensor_power_disable();
		sensor_powerdown_disable();	

		//printk("sr130pc10 first/last camera powerdown! \n");
	}

	return 0;
}

static int sensor_preview_sr130pc10(void)
{
	return write_regs_sr130pc10(sensor_reg_common_sr130pc10[1]);
}

static int sensor_capture_sr130pc10(void)
{
	return write_regs_sr130pc10(sensor_reg_common_sr130pc10[2]);
}

static int sensor_capturecfg_sr130pc10(int width, int height)
{
	return 0;
}

static int sensor_zoom_sr130pc10(int val)
{
	return 0;
}

static int sensor_autofocus_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_af_sr130pc10[val]);
}

static int sensor_effect_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_effect_sr130pc10[val]);
}

static int sensor_flip_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_flip_sr130pc10[val]);
}

static int sensor_iso_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_iso_sr130pc10[val]);
}

static int sensor_me_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_metering_exposure_sr130pc10[val]);
}

static int sensor_wb_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_awb_sr130pc10[val]);
}

static int sensor_bright_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_brightness_sr130pc10[val]);
}

static int sensor_scene_sr130pc10(int val)
{
	return write_regs_sr130pc10(sensor_reg_scene_sr130pc10[val]);
}

static int sensor_check_esd_sr130pc10(int val)
{
	return 0;
}

static int sensor_check_luma_sr130pc10(int val)
{
	return 0;
}

/**********************************************************
*    Function  
**********************************************************/
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void sensor_info_init_sr130pc10(TCC_SENSOR_INFO_TYPE *sensor_info)
{
	sensor_info->i2c_addr 				= 0x40;
	sensor_info->reg_term 				= 0x0000;
	sensor_info->val_term 				= 0x0000;
	#if defined(CONFIG_ARCH_TCC92XX)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 720000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#elif defined(CONFIG_ARCH_TCC93XX)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 660000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else  // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 1120000;
	sensor_info->s_clock_source 			= PCDIRECTPLL1;
	#endif  // CONFIG_USE_ISP
	#elif defined(CONFIG_ARCH_TCC88XX)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 720000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else  // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 1250000;
	sensor_info->s_clock_source 			= PCDIRECTPLL1;
	#endif  // CONFIG_USE_ISP
	#elif defined(CONFIG_ARCH_TCC892X)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 1250000;
	sensor_info->s_clock_source 			= PCDIRECTPLL1;
	#endif
	sensor_info->preview_w 				= 640;
	sensor_info->preview_h 				= 480;
	sensor_info->preview_zoom_offset_x 	= 8;
	sensor_info->preview_zoom_offset_y 	= 6;
	sensor_info->capture_w 				= 1280;
	sensor_info->capture_h 				= 960;
	sensor_info->capture_zoom_offset_x 	= 20;
	sensor_info->capture_zoom_offset_y 	= 16;
	sensor_info->max_zoom_step 			= 15;
	sensor_info->cam_capchg_width 		= 640;
	sensor_info->framerate				= 15;
	sensor_info->p_clock_pol 				= POSITIVE_EDGE;
	sensor_info->v_sync_pol 				= ACT_HIGH;
	sensor_info->h_sync_pol                 = ACT_HIGH;
	#if defined(CONFIG_ARCH_TCC892X)
	sensor_info->de_pol                 = ACT_LOW;
	#endif	
	sensor_info->format 					= M420_ZERO;
	sensor_info->capture_skip_frame 		= 1;
	sensor_info->sensor_sizes 			= sensor_sizes_sr130pc10;
}

void sensor_init_fnc_sr130pc10(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_sr130pc10;
	sensor_func->Close 					= sensor_close_sr130pc10;
	sensor_func->PowerDown				= sensor_powerdown_sr130pc10;

	sensor_func->Set_Preview 			= sensor_preview_sr130pc10;
	sensor_func->Set_Capture 			= sensor_capture_sr130pc10;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_sr130pc10;

	sensor_func->Set_Zoom 				= sensor_zoom_sr130pc10;
	sensor_func->Set_AF 				= sensor_autofocus_sr130pc10;
	sensor_func->Set_Effect 				= sensor_effect_sr130pc10;
	sensor_func->Set_Flip 				= sensor_flip_sr130pc10;
	sensor_func->Set_ISO 				= sensor_iso_sr130pc10;
	sensor_func->Set_ME 				= sensor_me_sr130pc10;
	sensor_func->Set_WB 				= sensor_wb_sr130pc10;
	sensor_func->Set_Bright 				= sensor_bright_sr130pc10;
	sensor_func->Set_Scene 				= sensor_scene_sr130pc10;

	sensor_func->Check_ESD 				= sensor_check_esd_sr130pc10;
	sensor_func->Check_Luma 			= sensor_check_luma_sr130pc10;
}
#else
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_sr130pc10;
	sensor_func->Close 					= sensor_close_sr130pc10;
	sensor_func->PowerDown				= sensor_powerdown_sr130pc10;

	sensor_func->Set_Preview 			= sensor_preview_sr130pc10;
	sensor_func->Set_Capture 			= sensor_capture_sr130pc10;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_sr130pc10;

	sensor_func->Set_Zoom 				= sensor_zoom_sr130pc10;
	sensor_func->Set_AF 				= sensor_autofocus_sr130pc10;
	sensor_func->Set_Effect 				= sensor_effect_sr130pc10;
	sensor_func->Set_Flip 				= sensor_flip_sr130pc10;
	sensor_func->Set_ISO 				= sensor_iso_sr130pc10;
	sensor_func->Set_ME 				= sensor_me_sr130pc10;
	sensor_func->Set_WB 				= sensor_wb_sr130pc10;
	sensor_func->Set_Bright 				= sensor_bright_sr130pc10;
	sensor_func->Set_Scene 				= sensor_scene_sr130pc10;

	sensor_func->Check_ESD 				= sensor_check_esd_sr130pc10;
	sensor_func->Check_Luma 			= sensor_check_luma_sr130pc10;
}
#endif
#endif

