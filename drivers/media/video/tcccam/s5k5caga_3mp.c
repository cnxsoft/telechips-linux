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

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_S5K5CAGA

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern int CameraID;
#endif

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
struct capture_size sensor_sizes_s5k5caga[] = {
	{ 2048, 1536 },
	{ 1600, 1200 },	/* UXGA */
	{ 1280,  960 },	/* SXGA */
	{ 1024,  768 },	/* XGA */
	{  800,  600 },	/* SVGA */
	{  640,  480 },	/* VGA */
	{  320,  240 },	/* QVGA */
	{  176,  144 },	/* QCIF */
};
#else
struct capture_size sensor_sizes[] = {
	{ 2048, 1536 },
	{ 1600, 1200 },	/* UXGA */
	{ 1280,  960 },	/* SXGA */
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


static struct sensor_reg sensor_initialize_s5k5caga[] = {
	{0xFCFC, 0xD000, WORD_LEN},	

	//********************************//
	////************************************/																															//
	////Ver 0.1 Nnalog Settings for 5CA EVT0 30FPS in Binning, including latest TnP for EVT1              //
	////20091310 ded analog settings for 430LSB, long exposure mode only. Settings are for 32MHz Sys. CLK //
	////20091102 ded all calibration data, final settings for STW EVT1 module, SCLK 32MHz, PCLK 60 MHz.   //
	////20091104 anged the shading alpha &Near off                                                        //
	////20091104 anged awbb_GridEnable from 0001h to 0002h	//awbb_GridEnable                             //
	////aetarget4, gamma change for improving contrast                                                    //
	////20100113 w TnP updated//

	{0xFCFC, 0xD000, WORD_LEN},		//Reset                                  //
	{0x0010, 0x0001, WORD_LEN},	   //Clear host interrupt so main will wait //
	{0x1030, 0x0000, WORD_LEN},	   //ARM go                                 //
	{0x0014, 0x0001, WORD_LEN},	   //Wait100mSec                            //

	//p100	//Delay 100ms	
	{S5K5CAGA_REG_TERM, 0x0064, WORD_LEN}, 

	// Start T&P part
	// DO NOT DELETE T&P SECTION COMMENTS! They are required to debug T&P related issues.
	// svn://transrdsrv/svn/svnroot/System/Software/tcevb/SDK+FW/ISP_5CA/Firmware
	// Rev: 33047-33047
	// Signature:
	// md5 6254dfca66adca40485075c9b910ba19 .btp
	// md5 cfb4ae7a2d8ae0084b859906a6b2a12c .htp
	// md5 eef3c9ff422110ae87ecc792a436d69e .RegsMap.h
	// md5 e18c15b6de4bae04b1e75aef7c2317ea .RegsMap.bin
	// md5 506b4144bd48cdb79cbecdda4f7176ba .base.RegsMap.h
	// md5 fd8f92f13566c1a788746b23691c5f5f .base.RegsMap.bin
	//

	{0x0028, 0x7000, WORD_LEN},	
	{0x002A, 0x2CF8, WORD_LEN},	
	{0x0F12, 0xB510, WORD_LEN},	
	{0x0F12, 0x4827, WORD_LEN},	
	{0x0F12, 0x21C0, WORD_LEN},	
	{0x0F12, 0x8041, WORD_LEN},	
	{0x0F12, 0x4825, WORD_LEN},	
	{0x0F12, 0x4A26, WORD_LEN},	
	{0x0F12, 0x3020, WORD_LEN},	
	{0x0F12, 0x8382, WORD_LEN},	
	{0x0F12, 0x1D12, WORD_LEN},	
	{0x0F12, 0x83C2, WORD_LEN},	
	{0x0F12, 0x4822, WORD_LEN},	
	{0x0F12, 0x3040, WORD_LEN},	
	{0x0F12, 0x8041, WORD_LEN},	
	{0x0F12, 0x4821, WORD_LEN},	
	{0x0F12, 0x4922, WORD_LEN},	
	{0x0F12, 0x3060, WORD_LEN},	
	{0x0F12, 0x8381, WORD_LEN},	
	{0x0F12, 0x1D09, WORD_LEN},	
	{0x0F12, 0x83C1, WORD_LEN},	
	{0x0F12, 0x4821, WORD_LEN},	
	{0x0F12, 0x491D, WORD_LEN},	
	{0x0F12, 0x8802, WORD_LEN},	
	{0x0F12, 0x3980, WORD_LEN},	
	{0x0F12, 0x804A, WORD_LEN},	
	{0x0F12, 0x8842, WORD_LEN},	
	{0x0F12, 0x808A, WORD_LEN},	
	{0x0F12, 0x8882, WORD_LEN},	
	{0x0F12, 0x80CA, WORD_LEN},	
	{0x0F12, 0x88C2, WORD_LEN},	
	{0x0F12, 0x810A, WORD_LEN},	
	{0x0F12, 0x8902, WORD_LEN},	
	{0x0F12, 0x491C, WORD_LEN},	
	{0x0F12, 0x80CA, WORD_LEN},	
	{0x0F12, 0x8942, WORD_LEN},	
	{0x0F12, 0x814A, WORD_LEN},	
	{0x0F12, 0x8982, WORD_LEN},	
	{0x0F12, 0x830A, WORD_LEN},	
	{0x0F12, 0x89C2, WORD_LEN},	
	{0x0F12, 0x834A, WORD_LEN},	
	{0x0F12, 0x8A00, WORD_LEN},	
	{0x0F12, 0x4918, WORD_LEN},	
	{0x0F12, 0x8188, WORD_LEN},	
	{0x0F12, 0x4918, WORD_LEN},	
	{0x0F12, 0x4819, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xFA0E, WORD_LEN},	
	{0x0F12, 0x4918, WORD_LEN},	
	{0x0F12, 0x4819, WORD_LEN},	
	{0x0F12, 0x6341, WORD_LEN},	
	{0x0F12, 0x4919, WORD_LEN},	
	{0x0F12, 0x4819, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xFA07, WORD_LEN},	
	{0x0F12, 0x4816, WORD_LEN},	
	{0x0F12, 0x4918, WORD_LEN},	
	{0x0F12, 0x3840, WORD_LEN},	
	{0x0F12, 0x62C1, WORD_LEN},	
	{0x0F12, 0x4918, WORD_LEN},	
	{0x0F12, 0x3880, WORD_LEN},	
	{0x0F12, 0x63C1, WORD_LEN},	
	{0x0F12, 0x4917, WORD_LEN},	
	{0x0F12, 0x6301, WORD_LEN},	
	{0x0F12, 0x4917, WORD_LEN},	
	{0x0F12, 0x3040, WORD_LEN},	
	{0x0F12, 0x6181, WORD_LEN},	
	{0x0F12, 0x4917, WORD_LEN},	
	{0x0F12, 0x4817, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF9F7, WORD_LEN},	
	{0x0F12, 0x4917, WORD_LEN},	
	{0x0F12, 0x4817, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF9F3, WORD_LEN},	
	{0x0F12, 0x4917, WORD_LEN},	
	{0x0F12, 0x4817, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF9EF, WORD_LEN},	
	{0x0F12, 0xBC10, WORD_LEN},	
	{0x0F12, 0xBC08, WORD_LEN},	
	{0x0F12, 0x4718, WORD_LEN},	
	{0x0F12, 0x1100, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x267C, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x2CE8, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x3274, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0xF400, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0xF520, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x2DF1, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x89A9, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x2E43, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x0140, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x2E7D, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0xB4F7, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x2F07, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x2F2B, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x2FD1, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x2FE5, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x2FB9, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x013D, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},	
	{0x0F12, 0x306B, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x5823, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x30B9, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0xD789, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0xB570, WORD_LEN},	
	{0x0F12, 0x6804, WORD_LEN},	
	{0x0F12, 0x6845, WORD_LEN},	
	{0x0F12, 0x6881, WORD_LEN},	
	{0x0F12, 0x6840, WORD_LEN},	
	{0x0F12, 0x2900, WORD_LEN},	
	{0x0F12, 0x6880, WORD_LEN},	
	{0x0F12, 0xD007, WORD_LEN},	
	{0x0F12, 0x49C3, WORD_LEN},	
	{0x0F12, 0x8949, WORD_LEN},	
	{0x0F12, 0x084A, WORD_LEN},	
	{0x0F12, 0x1880, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF9BA, WORD_LEN},	
	{0x0F12, 0x80A0, WORD_LEN},	
	{0x0F12, 0xE000, WORD_LEN},	
	{0x0F12, 0x80A0, WORD_LEN},	
	{0x0F12, 0x88A0, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD010, WORD_LEN},	
	{0x0F12, 0x68A9, WORD_LEN},	
	{0x0F12, 0x6828, WORD_LEN},	
	{0x0F12, 0x084A, WORD_LEN},	
	{0x0F12, 0x1880, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF9AE, WORD_LEN},	
	{0x0F12, 0x8020, WORD_LEN},	
	{0x0F12, 0x1D2D, WORD_LEN},	
	{0x0F12, 0xCD03, WORD_LEN},	
	{0x0F12, 0x084A, WORD_LEN},	
	{0x0F12, 0x1880, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF9A7, WORD_LEN},	
	{0x0F12, 0x8060, WORD_LEN},	
	{0x0F12, 0xBC70, WORD_LEN},	
	{0x0F12, 0xBC08, WORD_LEN},	
	{0x0F12, 0x4718, WORD_LEN},	
	{0x0F12, 0x2000, WORD_LEN},	
	{0x0F12, 0x8060, WORD_LEN},	
	{0x0F12, 0x8020, WORD_LEN},	
	{0x0F12, 0xE7F8, WORD_LEN},	
	{0x0F12, 0xB510, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF9A2, WORD_LEN},	
	{0x0F12, 0x48B2, WORD_LEN},	
	{0x0F12, 0x8A40, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD00C, WORD_LEN},	
	{0x0F12, 0x48B1, WORD_LEN},	
	{0x0F12, 0x49B2, WORD_LEN},	
	{0x0F12, 0x8800, WORD_LEN},	
	{0x0F12, 0x4AB2, WORD_LEN},	
	{0x0F12, 0x2805, WORD_LEN},	
	{0x0F12, 0xD003, WORD_LEN},	
	{0x0F12, 0x4BB1, WORD_LEN},	
	{0x0F12, 0x795B, WORD_LEN},	
	{0x0F12, 0x2B00, WORD_LEN},	
	{0x0F12, 0xD005, WORD_LEN},	
	{0x0F12, 0x2001, WORD_LEN},	
	{0x0F12, 0x8008, WORD_LEN},	
	{0x0F12, 0x8010, WORD_LEN},	
	{0x0F12, 0xBC10, WORD_LEN},	
	{0x0F12, 0xBC08, WORD_LEN},	
	{0x0F12, 0x4718, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD1FA, WORD_LEN},	
	{0x0F12, 0x2000, WORD_LEN},	
	{0x0F12, 0x8008, WORD_LEN},	
	{0x0F12, 0x8010, WORD_LEN},	
	{0x0F12, 0xE7F6, WORD_LEN},	
	{0x0F12, 0xB5F8, WORD_LEN},	
	{0x0F12, 0x2407, WORD_LEN},	
	{0x0F12, 0x2C06, WORD_LEN},	
	{0x0F12, 0xD035, WORD_LEN},	
	{0x0F12, 0x2C07, WORD_LEN},	
	{0x0F12, 0xD033, WORD_LEN},	
	{0x0F12, 0x48A3, WORD_LEN},	
	{0x0F12, 0x8BC1, WORD_LEN},	
	{0x0F12, 0x2900, WORD_LEN},	
	{0x0F12, 0xD02A, WORD_LEN},	
	{0x0F12, 0x00A2, WORD_LEN},	
	{0x0F12, 0x1815, WORD_LEN},	
	{0x0F12, 0x4AA4, WORD_LEN},	
	{0x0F12, 0x6DEE, WORD_LEN},	
	{0x0F12, 0x8A92, WORD_LEN},	
	{0x0F12, 0x4296, WORD_LEN},	
	{0x0F12, 0xD923, WORD_LEN},	
	{0x0F12, 0x0028, WORD_LEN},	
	{0x0F12, 0x3080, WORD_LEN},	
	{0x0F12, 0x0007, WORD_LEN},	
	{0x0F12, 0x69C0, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF96B, WORD_LEN},	
	{0x0F12, 0x1C71, WORD_LEN},	
	{0x0F12, 0x0280, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF967, WORD_LEN},	
	{0x0F12, 0x0006, WORD_LEN},	
	{0x0F12, 0x4898, WORD_LEN},	
	{0x0F12, 0x0061, WORD_LEN},	
	{0x0F12, 0x1808, WORD_LEN},	
	{0x0F12, 0x8D80, WORD_LEN},	
	{0x0F12, 0x0A01, WORD_LEN},	
	{0x0F12, 0x0600, WORD_LEN},	
	{0x0F12, 0x0E00, WORD_LEN},	
	{0x0F12, 0x1A08, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF96A, WORD_LEN},	
	{0x0F12, 0x0002, WORD_LEN},	
	{0x0F12, 0x6DE9, WORD_LEN},	
	{0x0F12, 0x6FE8, WORD_LEN},	
	{0x0F12, 0x1A08, WORD_LEN},	
	{0x0F12, 0x4351, WORD_LEN},	
	{0x0F12, 0x0300, WORD_LEN},	
	{0x0F12, 0x1C49, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF953, WORD_LEN},	
	{0x0F12, 0x0401, WORD_LEN},	
	{0x0F12, 0x0430, WORD_LEN},	
	{0x0F12, 0x0C00, WORD_LEN},	
	{0x0F12, 0x4301, WORD_LEN},	
	{0x0F12, 0x61F9, WORD_LEN},	
	{0x0F12, 0xE004, WORD_LEN},	
	{0x0F12, 0x00A2, WORD_LEN},	
	{0x0F12, 0x4990, WORD_LEN},	
	{0x0F12, 0x1810, WORD_LEN},	
	{0x0F12, 0x3080, WORD_LEN},	
	{0x0F12, 0x61C1, WORD_LEN},	
	{0x0F12, 0x1E64, WORD_LEN},	
	{0x0F12, 0xD2C5, WORD_LEN},	
	{0x0F12, 0x2006, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF959, WORD_LEN},	
	{0x0F12, 0x2007, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF956, WORD_LEN},	
	{0x0F12, 0xBCF8, WORD_LEN},	
	{0x0F12, 0xBC08, WORD_LEN},	
	{0x0F12, 0x4718, WORD_LEN},	
	{0x0F12, 0xB510, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF958, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD00A, WORD_LEN},	
	{0x0F12, 0x4881, WORD_LEN},	
	{0x0F12, 0x8B81, WORD_LEN},	
	{0x0F12, 0x0089, WORD_LEN},	
	{0x0F12, 0x1808, WORD_LEN},	
	{0x0F12, 0x6DC1, WORD_LEN},	
	{0x0F12, 0x4883, WORD_LEN},	
	{0x0F12, 0x8A80, WORD_LEN},	
	{0x0F12, 0x4281, WORD_LEN},	
	{0x0F12, 0xD901, WORD_LEN},	
	{0x0F12, 0x2001, WORD_LEN},	
	{0x0F12, 0xE7A1, WORD_LEN},	
	{0x0F12, 0x2000, WORD_LEN},	
	{0x0F12, 0xE79F, WORD_LEN},	
	{0x0F12, 0xB5F8, WORD_LEN},	
	{0x0F12, 0x0004, WORD_LEN},	
	{0x0F12, 0x4F80, WORD_LEN},	
	{0x0F12, 0x227D, WORD_LEN},	
	{0x0F12, 0x8938, WORD_LEN},	
	{0x0F12, 0x0152, WORD_LEN},	
	{0x0F12, 0x4342, WORD_LEN},	
	{0x0F12, 0x487E, WORD_LEN},	
	{0x0F12, 0x9000, WORD_LEN},	
	{0x0F12, 0x8A01, WORD_LEN},	
	{0x0F12, 0x0848, WORD_LEN},	
	{0x0F12, 0x1810, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF91D, WORD_LEN},	
	{0x0F12, 0x210F, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF940, WORD_LEN},	
	{0x0F12, 0x497A, WORD_LEN},	
	{0x0F12, 0x8C49, WORD_LEN},	
	{0x0F12, 0x090E, WORD_LEN},	
	{0x0F12, 0x0136, WORD_LEN},	
	{0x0F12, 0x4306, WORD_LEN},	
	{0x0F12, 0x4979, WORD_LEN},	
	{0x0F12, 0x2C00, WORD_LEN},	
	{0x0F12, 0xD003, WORD_LEN},	
	{0x0F12, 0x2001, WORD_LEN},	
	{0x0F12, 0x0240, WORD_LEN},	
	{0x0F12, 0x4330, WORD_LEN},	
	{0x0F12, 0x8108, WORD_LEN},	
	{0x0F12, 0x4876, WORD_LEN},	
	{0x0F12, 0x2C00, WORD_LEN},	
	{0x0F12, 0x8D00, WORD_LEN},	
	{0x0F12, 0xD001, WORD_LEN},	
	{0x0F12, 0x2501, WORD_LEN},	
	{0x0F12, 0xE000, WORD_LEN},	
	{0x0F12, 0x2500, WORD_LEN},	
	{0x0F12, 0x4972, WORD_LEN},	
	{0x0F12, 0x4328, WORD_LEN},	
	{0x0F12, 0x8008, WORD_LEN},	
	{0x0F12, 0x207D, WORD_LEN},	
	{0x0F12, 0x00C0, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF92E, WORD_LEN},	
	{0x0F12, 0x2C00, WORD_LEN},	
	{0x0F12, 0x496E, WORD_LEN},	
	{0x0F12, 0x0328, WORD_LEN},	
	{0x0F12, 0x4330, WORD_LEN},	
	{0x0F12, 0x8108, WORD_LEN},	
	{0x0F12, 0x88F8, WORD_LEN},	
	{0x0F12, 0x2C00, WORD_LEN},	
	{0x0F12, 0x01AA, WORD_LEN},	
	{0x0F12, 0x4310, WORD_LEN},	
	{0x0F12, 0x8088, WORD_LEN},	
	{0x0F12, 0x9800, WORD_LEN},	
	{0x0F12, 0x8A01, WORD_LEN},	
	{0x0F12, 0x486A, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8F1, WORD_LEN},	
	{0x0F12, 0x496A, WORD_LEN},	
	{0x0F12, 0x8809, WORD_LEN},	
	{0x0F12, 0x4348, WORD_LEN},	
	{0x0F12, 0x0400, WORD_LEN},	
	{0x0F12, 0x0C00, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF918, WORD_LEN},	
	{0x0F12, 0x0020, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF91D, WORD_LEN},	
	{0x0F12, 0x4866, WORD_LEN},	
	{0x0F12, 0x7004, WORD_LEN},	
	{0x0F12, 0xE7A3, WORD_LEN},	
	{0x0F12, 0xB510, WORD_LEN},	
	{0x0F12, 0x0004, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF91E, WORD_LEN},	
	{0x0F12, 0x6020, WORD_LEN},	
	{0x0F12, 0x4963, WORD_LEN},	
	{0x0F12, 0x8B49, WORD_LEN},	
	{0x0F12, 0x0789, WORD_LEN},	
	{0x0F12, 0xD001, WORD_LEN},	
	{0x0F12, 0x0040, WORD_LEN},	
	{0x0F12, 0x6020, WORD_LEN},	
	{0x0F12, 0xE74C, WORD_LEN},	
	{0x0F12, 0xB510, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF91B, WORD_LEN},	
	{0x0F12, 0x485F, WORD_LEN},	
	{0x0F12, 0x8880, WORD_LEN},	
	{0x0F12, 0x0601, WORD_LEN},	
	{0x0F12, 0x4854, WORD_LEN},	
	{0x0F12, 0x1609, WORD_LEN},	
	{0x0F12, 0x8141, WORD_LEN},	
	{0x0F12, 0xE742, WORD_LEN},	
	{0x0F12, 0xB5F8, WORD_LEN},	
	{0x0F12, 0x000F, WORD_LEN},	
	{0x0F12, 0x4C55, WORD_LEN},	
	{0x0F12, 0x3420, WORD_LEN},	
	{0x0F12, 0x2500, WORD_LEN},	
	{0x0F12, 0x5765, WORD_LEN},	
	{0x0F12, 0x0039, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF913, WORD_LEN},	
	{0x0F12, 0x9000, WORD_LEN},	
	{0x0F12, 0x2600, WORD_LEN},	
	{0x0F12, 0x57A6, WORD_LEN},	
	{0x0F12, 0x4C4C, WORD_LEN},	
	{0x0F12, 0x42AE, WORD_LEN},	
	{0x0F12, 0xD01B, WORD_LEN},	
	{0x0F12, 0x4D54, WORD_LEN},	
	{0x0F12, 0x8AE8, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD013, WORD_LEN},	
	{0x0F12, 0x484D, WORD_LEN},	
	{0x0F12, 0x8A01, WORD_LEN},	
	{0x0F12, 0x8B80, WORD_LEN},	
	{0x0F12, 0x4378, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8B5, WORD_LEN},	
	{0x0F12, 0x89A9, WORD_LEN},	
	{0x0F12, 0x1A41, WORD_LEN},	
	{0x0F12, 0x484E, WORD_LEN},	
	{0x0F12, 0x3820, WORD_LEN},	
	{0x0F12, 0x8AC0, WORD_LEN},	
	{0x0F12, 0x4348, WORD_LEN},	
	{0x0F12, 0x17C1, WORD_LEN},	
	{0x0F12, 0x0D89, WORD_LEN},	
	{0x0F12, 0x1808, WORD_LEN},	
	{0x0F12, 0x1280, WORD_LEN},	
	{0x0F12, 0x8961, WORD_LEN},	
	{0x0F12, 0x1A08, WORD_LEN},	
	{0x0F12, 0x8160, WORD_LEN},	
	{0x0F12, 0xE003, WORD_LEN},	
	{0x0F12, 0x88A8, WORD_LEN},	
	{0x0F12, 0x0600, WORD_LEN},	
	{0x0F12, 0x1600, WORD_LEN},	
	{0x0F12, 0x8160, WORD_LEN},	
	{0x0F12, 0x200A, WORD_LEN},	
	{0x0F12, 0x5E20, WORD_LEN},	
	{0x0F12, 0x42B0, WORD_LEN},	
	{0x0F12, 0xD011, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8AB, WORD_LEN},	
	{0x0F12, 0x1D40, WORD_LEN},	
	{0x0F12, 0x00C3, WORD_LEN},	
	{0x0F12, 0x1A18, WORD_LEN},	
	{0x0F12, 0x214B, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF897, WORD_LEN},	
	{0x0F12, 0x211F, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8BA, WORD_LEN},	
	{0x0F12, 0x210A, WORD_LEN},	
	{0x0F12, 0x5E61, WORD_LEN},	
	{0x0F12, 0x0FC9, WORD_LEN},	
	{0x0F12, 0x0149, WORD_LEN},	
	{0x0F12, 0x4301, WORD_LEN},	
	{0x0F12, 0x483D, WORD_LEN},	
	{0x0F12, 0x81C1, WORD_LEN},	
	{0x0F12, 0x9800, WORD_LEN},	
	{0x0F12, 0xE74A, WORD_LEN},	
	{0x0F12, 0xB5F1, WORD_LEN},	
	{0x0F12, 0xB082, WORD_LEN},	
	{0x0F12, 0x2500, WORD_LEN},	
	{0x0F12, 0x483A, WORD_LEN},	
	{0x0F12, 0x9001, WORD_LEN},	
	{0x0F12, 0x2400, WORD_LEN},	
	{0x0F12, 0x2028, WORD_LEN},	
	{0x0F12, 0x4368, WORD_LEN},	
	{0x0F12, 0x4A39, WORD_LEN},	
	{0x0F12, 0x4925, WORD_LEN},	
	{0x0F12, 0x1887, WORD_LEN},	
	{0x0F12, 0x1840, WORD_LEN},	
	{0x0F12, 0x9000, WORD_LEN},	
	{0x0F12, 0x9800, WORD_LEN},	
	{0x0F12, 0x0066, WORD_LEN},	
	{0x0F12, 0x9A01, WORD_LEN},	
	{0x0F12, 0x1980, WORD_LEN},	
	{0x0F12, 0x218C, WORD_LEN},	
	{0x0F12, 0x5A09, WORD_LEN},	
	{0x0F12, 0x8A80, WORD_LEN},	
	{0x0F12, 0x8812, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8CA, WORD_LEN},	
	{0x0F12, 0x53B8, WORD_LEN},	
	{0x0F12, 0x1C64, WORD_LEN},	
	{0x0F12, 0x2C14, WORD_LEN},	
	{0x0F12, 0xDBF1, WORD_LEN},	
	{0x0F12, 0x1C6D, WORD_LEN},	
	{0x0F12, 0x2D03, WORD_LEN},	
	{0x0F12, 0xDBE6, WORD_LEN},	
	{0x0F12, 0x9802, WORD_LEN},	
	{0x0F12, 0x6800, WORD_LEN},	
	{0x0F12, 0x0600, WORD_LEN},	
	{0x0F12, 0x0E00, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8C5, WORD_LEN},	
	{0x0F12, 0xBCFE, WORD_LEN},	
	{0x0F12, 0xBC08, WORD_LEN},	
	{0x0F12, 0x4718, WORD_LEN},	
	{0x0F12, 0xB570, WORD_LEN},	
	{0x0F12, 0x6805, WORD_LEN},	
	{0x0F12, 0x2404, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8C5, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD103, WORD_LEN},	
	{0x0F12, 0xF000, WORD_LEN},	
	{0x0F12, 0xF8C9, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x2400, WORD_LEN},	
	{0x0F12, 0x3540, WORD_LEN},	
	{0x0F12, 0x88E8, WORD_LEN},	
	{0x0F12, 0x0500, WORD_LEN},	
	{0x0F12, 0xD403, WORD_LEN},	
	{0x0F12, 0x4822, WORD_LEN},	
	{0x0F12, 0x89C0, WORD_LEN},	
	{0x0F12, 0x2800, WORD_LEN},	
	{0x0F12, 0xD002, WORD_LEN},	
	{0x0F12, 0x2008, WORD_LEN},	
	{0x0F12, 0x4304, WORD_LEN},	
	{0x0F12, 0xE001, WORD_LEN},	
	{0x0F12, 0x2010, WORD_LEN},	
	{0x0F12, 0x4304, WORD_LEN},	
	{0x0F12, 0x481F, WORD_LEN},	
	{0x0F12, 0x8B80, WORD_LEN},	
	{0x0F12, 0x0700, WORD_LEN},	
	{0x0F12, 0x0F81, WORD_LEN},	
	{0x0F12, 0x2001, WORD_LEN},	
	{0x0F12, 0x2900, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x4304, WORD_LEN},	
	{0x0F12, 0x491C, WORD_LEN},	
	{0x0F12, 0x8B0A, WORD_LEN},	
	{0x0F12, 0x42A2, WORD_LEN},	
	{0x0F12, 0xD004, WORD_LEN},	
	{0x0F12, 0x0762, WORD_LEN},	
	{0x0F12, 0xD502, WORD_LEN},	
	{0x0F12, 0x4A19, WORD_LEN},	
	{0x0F12, 0x3220, WORD_LEN},	
	{0x0F12, 0x8110, WORD_LEN},	
	{0x0F12, 0x830C, WORD_LEN},	
	{0x0F12, 0xE691, WORD_LEN},	
	{0x0F12, 0x0C3C, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x3274, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x26E8, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x6100, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x6500, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x1A7C, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x1120, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0xFFFF, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x3374, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x1D6C, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x167C, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0xF400, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x2C2C, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x40A0, WORD_LEN},	
	{0x0F12, 0x00DD, WORD_LEN},	
	{0x0F12, 0xF520, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x2C29, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x1A54, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x1564, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0xF2A0, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x2440, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x05A0, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x2894, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0x1224, WORD_LEN},	
	{0x0F12, 0x7000, WORD_LEN},	
	{0x0F12, 0xB000, WORD_LEN},	
	{0x0F12, 0xD000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0x1A3F, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xF004, WORD_LEN},	
	{0x0F12, 0xE51F, WORD_LEN},	
	{0x0F12, 0x1F48, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0x24BD, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0x36DD, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xB4CF, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xB5D7, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0x36ED, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xF53F, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xF5D9, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0x013D, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xF5C9, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xFAA9, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0x3723, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0x5823, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xD771, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x4778, WORD_LEN},	
	{0x0F12, 0x46C0, WORD_LEN},	
	{0x0F12, 0xC000, WORD_LEN},	
	{0x0F12, 0xE59F, WORD_LEN},	
	{0x0F12, 0xFF1C, WORD_LEN},	
	{0x0F12, 0xE12F, WORD_LEN},	
	{0x0F12, 0xD75B, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	{0x0F12, 0x8117, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
	//
	// Parameters Defined in T&P:
	// Mon_SARR_usGammaLutRGBInterpolate          120 700005A0 ARRAY
	// Mon_SARR_usGammaLutRGBInterpolate[0][0]      2 700005A0 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][1]      2 700005A2 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][2]      2 700005A4 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][3]      2 700005A6 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][4]      2 700005A8 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][5]      2 700005AA SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][6]      2 700005AC SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][7]      2 700005AE SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][8]      2 700005B0 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][9]      2 700005B2 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][10]     2 700005B4 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][11]     2 700005B6 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][12]     2 700005B8 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][13]     2 700005BA SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][14]     2 700005BC SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][15]     2 700005BE SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][16]     2 700005C0 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][17]     2 700005C2 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][18]     2 700005C4 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[0][19]     2 700005C6 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][0]      2 700005C8 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][1]      2 700005CA SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][2]      2 700005CC SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][3]      2 700005CE SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][4]      2 700005D0 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][5]      2 700005D2 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][6]      2 700005D4 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][7]      2 700005D6 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][8]      2 700005D8 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][9]      2 700005DA SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][10]     2 700005DC SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][11]     2 700005DE SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][12]     2 700005E0 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][13]     2 700005E2 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][14]     2 700005E4 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][15]     2 700005E6 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][16]     2 700005E8 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][17]     2 700005EA SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][18]     2 700005EC SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[1][19]     2 700005EE SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][0]      2 700005F0 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][1]      2 700005F2 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][2]      2 700005F4 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][3]      2 700005F6 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][4]      2 700005F8 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][5]      2 700005FA SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][6]      2 700005FC SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][7]      2 700005FE SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][8]      2 70000600 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][9]      2 70000602 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][10]     2 70000604 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][11]     2 70000606 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][12]     2 70000608 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][13]     2 7000060A SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][14]     2 7000060C SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][15]     2 7000060E SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][16]     2 70000610 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][17]     2 70000612 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][18]     2 70000614 SHORT
	// Mon_SARR_usGammaLutRGBInterpolate[2][19]     2 70000616 SHORT
	// TnP_SvnVersion                               2 7000326C SHORT
	// Tune_TP                                    268 70003274 STRUCT
	// Tune_TP_IO_DrivingCurrent_D0_D4_cs10         2 70003274 SHORT
	// Tune_TP_IO_DrivingCurrent_D9_D5_cs10         2 70003276 SHORT
	// Tune_TP_IO_DrivingCurrent_GPIO_cd10          2 70003278 SHORT
	// Tune_TP_IO_DrivingCurrent_CLKs_cd10          2 7000327A SHORT
	// Tune_TP_atop_dblr_reg_1                      2 7000327C SHORT
	// Tune_TP_atop_dblr_reg_3                      2 7000327E SHORT
	// Tune_TP_atop_ramp_reg_1                      2 70003280 SHORT
	// Tune_TP_atop_ramp_reg_2                      2 70003282 SHORT
	// Tune_TP_atop_rmp_offset_sig                  2 70003284 SHORT
	// Tune_TP_bEnablePrePostGammaAfControl         2 70003286 SHORT
	// Tune_TP_seti                               240 70003288 STRUCT
	// SARR_usDualGammaLutRGBIndoor               120 70003288 ARRAY
	// SARR_usDualGammaLutRGBIndoor[0][0]           2 70003288 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][1]           2 7000328A SHORT
	// SARR_usDualGammaLutRGBIndoor[0][2]           2 7000328C SHORT
	// SARR_usDualGammaLutRGBIndoor[0][3]           2 7000328E SHORT
	// SARR_usDualGammaLutRGBIndoor[0][4]           2 70003290 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][5]           2 70003292 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][6]           2 70003294 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][7]           2 70003296 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][8]           2 70003298 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][9]           2 7000329A SHORT
	// SARR_usDualGammaLutRGBIndoor[0][10]          2 7000329C SHORT
	// SARR_usDualGammaLutRGBIndoor[0][11]          2 7000329E SHORT
	// SARR_usDualGammaLutRGBIndoor[0][12]          2 700032A0 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][13]          2 700032A2 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][14]          2 700032A4 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][15]          2 700032A6 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][16]          2 700032A8 SHORT
	// SARR_usDualGammaLutRGBIndoor[0][17]          2 700032AA SHORT
	// SARR_usDualGammaLutRGBIndoor[0][18]          2 700032AC SHORT
	// SARR_usDualGammaLutRGBIndoor[0][19]          2 700032AE SHORT
	// SARR_usDualGammaLutRGBIndoor[1][0]           2 700032B0 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][1]           2 700032B2 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][2]           2 700032B4 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][3]           2 700032B6 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][4]           2 700032B8 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][5]           2 700032BA SHORT
	// SARR_usDualGammaLutRGBIndoor[1][6]           2 700032BC SHORT
	// SARR_usDualGammaLutRGBIndoor[1][7]           2 700032BE SHORT
	// SARR_usDualGammaLutRGBIndoor[1][8]           2 700032C0 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][9]           2 700032C2 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][10]          2 700032C4 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][11]          2 700032C6 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][12]          2 700032C8 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][13]          2 700032CA SHORT
	// SARR_usDualGammaLutRGBIndoor[1][14]          2 700032CC SHORT
	// SARR_usDualGammaLutRGBIndoor[1][15]          2 700032CE SHORT
	// SARR_usDualGammaLutRGBIndoor[1][16]          2 700032D0 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][17]          2 700032D2 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][18]          2 700032D4 SHORT
	// SARR_usDualGammaLutRGBIndoor[1][19]          2 700032D6 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][0]           2 700032D8 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][1]           2 700032DA SHORT
	// SARR_usDualGammaLutRGBIndoor[2][2]           2 700032DC SHORT
	// SARR_usDualGammaLutRGBIndoor[2][3]           2 700032DE SHORT
	// SARR_usDualGammaLutRGBIndoor[2][4]           2 700032E0 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][5]           2 700032E2 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][6]           2 700032E4 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][7]           2 700032E6 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][8]           2 700032E8 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][9]           2 700032EA SHORT
	// SARR_usDualGammaLutRGBIndoor[2][10]          2 700032EC SHORT
	// SARR_usDualGammaLutRGBIndoor[2][11]          2 700032EE SHORT
	// SARR_usDualGammaLutRGBIndoor[2][12]          2 700032F0 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][13]          2 700032F2 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][14]          2 700032F4 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][15]          2 700032F6 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][16]          2 700032F8 SHORT
	// SARR_usDualGammaLutRGBIndoor[2][17]          2 700032FA SHORT
	// SARR_usDualGammaLutRGBIndoor[2][18]          2 700032FC SHORT
	// SARR_usDualGammaLutRGBIndoor[2][19]          2 700032FE SHORT
	// SARR_usDualGammaLutRGBOutdoor              120 70003300 ARRAY
	// SARR_usDualGammaLutRGBOutdoor[0][0]          2 70003300 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][1]          2 70003302 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][2]          2 70003304 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][3]          2 70003306 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][4]          2 70003308 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][5]          2 7000330A SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][6]          2 7000330C SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][7]          2 7000330E SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][8]          2 70003310 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][9]          2 70003312 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][10]         2 70003314 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][11]         2 70003316 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][12]         2 70003318 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][13]         2 7000331A SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][14]         2 7000331C SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][15]         2 7000331E SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][16]         2 70003320 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][17]         2 70003322 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][18]         2 70003324 SHORT
	// SARR_usDualGammaLutRGBOutdoor[0][19]         2 70003326 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][0]          2 70003328 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][1]          2 7000332A SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][2]          2 7000332C SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][3]          2 7000332E SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][4]          2 70003330 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][5]          2 70003332 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][6]          2 70003334 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][7]          2 70003336 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][8]          2 70003338 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][9]          2 7000333A SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][10]         2 7000333C SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][11]         2 7000333E SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][12]         2 70003340 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][13]         2 70003342 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][14]         2 70003344 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][15]         2 70003346 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][16]         2 70003348 SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][17]         2 7000334A SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][18]         2 7000334C SHORT
	// SARR_usDualGammaLutRGBOutdoor[1][19]         2 7000334E SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][0]          2 70003350 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][1]          2 70003352 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][2]          2 70003354 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][3]          2 70003356 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][4]          2 70003358 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][5]          2 7000335A SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][6]          2 7000335C SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][7]          2 7000335E SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][8]          2 70003360 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][9]          2 70003362 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][10]         2 70003364 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][11]         2 70003366 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][12]         2 70003368 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][13]         2 7000336A SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][14]         2 7000336C SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][15]         2 7000336E SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][16]         2 70003370 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][17]         2 70003372 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][18]         2 70003374 SHORT
	// SARR_usDualGammaLutRGBOutdoor[2][19]         2 70003376 SHORT
	// Tune_TP_reserved                             2 70003378 SHORT
	// Tune_TP_atop_dbus_reg                        2 7000337A SHORT
	// Tune_TP_dblr_base_freq_mhz                   2 7000337C SHORT
	// Tune_TP_GL_sen_sOfs                          2 7000337E SHORT
	//
	// End T&P part

	//========================================================                      
	// CIs/APs/An setting        - 400LsB  sYsCLK 32MHz                             
	//========================================================                      
	// This regis are for FACTORY ONLY. If you change it without prior notification,
	// YOU are REsIBLE for the FAILURE that will happen in the future.              
	//========================================================                      


	{0x002A, 0x157A, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},	
	{0x002A, 0x1578, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},	
	{0x002A, 0x1576, WORD_LEN},	
	{0x0F12, 0x0020, WORD_LEN},	
	{0x002A, 0x1574, WORD_LEN},	
	{0x0F12, 0x0006, WORD_LEN},	
	{0x002A, 0x156E, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},		// slope calibration tolerance in units of 1/256	
	{0x002A, 0x1568, WORD_LEN},	
	{0x0F12, 0x00FC, WORD_LEN},	
	      
	//ADC control 
	{0x002A, 0x155A, WORD_LEN},	
	{0x0F12, 0x01CC, WORD_LEN},		//ADC sAT of 450mV for 10bit default in EVT1                         	
	{0x002A, 0x157E, WORD_LEN},	                                                                        
	{0x0F12, 0x0C80, WORD_LEN},		// 3200 Max. Reset ramp DCLK counts (default 2048 0x800)             
	{0x0F12, 0x0578, WORD_LEN},		// 1400 Max. Reset ramp DCLK counts for x3.5                         
	{0x002A, 0x157C, WORD_LEN},	                                                                        
	{0x0F12, 0x0190, WORD_LEN},		// 400 Reset ramp for x1 in DCLK counts                              
	{0x002A, 0x1570, WORD_LEN},	                                                                        
	{0x0F12, 0x00A0, WORD_LEN},		// 160 LsB                                                           
	{0x0F12, 0x0010, WORD_LEN},		// reset threshold                                                   
	{0x002A, 0x12C4, WORD_LEN},	                                                                        
	{0x0F12, 0x006A, WORD_LEN},		// 106 additional timing columns.                                    
	{0x002A, 0x12C8, WORD_LEN},	                                                                        
	{0x0F12, 0x08AC, WORD_LEN},		// 2220 ADC columns in normal mode including Hold & Latch            
	{0x0F12, 0x0050, WORD_LEN},		// 80 addition of ADC columns in Y-ave mode (default 244 0x74)
	      
	{0x002A, 0x1696, WORD_LEN},		// based on APs guidelines                        
	{0x0F12, 0x0000, WORD_LEN},	   // based on APs guidelines                        
	{0x0F12, 0x0000, WORD_LEN},	   // default. 1492 used for ADC dark characteristics
	{0x0F12, 0x00C6, WORD_LEN},	   // default. 1492 used for ADC dark characteristics
	{0x0F12, 0x00C6, WORD_LEN},	                                                                                                       
	                                                                   
	{0x002A, 0x1690, WORD_LEN},	   // when set double sampling is activated - requires different set of pointers                                                 
	{0x0F12, 0x0001, WORD_LEN},	                                                     
	                                                                   
	{0x002A, 0x12B0, WORD_LEN},	   // comp and pixel bias control 0xF40E - default for EVT1                                                                      
	{0x0F12, 0x0055, WORD_LEN},	   // comp and pixel bias control 0xF40E for binning mode                                                                        
	{0x0F12, 0x005A, WORD_LEN},	                                                     
	                                                                   
	{0x002A, 0x337A, WORD_LEN},	   // [7] - is used for rest-only mode (EVT0 value is 0xD and HW 0x6)                                                            
	{0x0F12, 0x0006, WORD_LEN},	
	{0x0F12, 0x0068, WORD_LEN},	
	{0x002A, 0x169E, WORD_LEN},	
	{0x0F12, 0x0007, WORD_LEN},	
	{0x002A, 0x0BF6, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	
		           
	                 
	{0x002A, 0x327C, WORD_LEN},	
	{0x0F12, 0x1000, WORD_LEN},	
	{0x0F12, 0x6998, WORD_LEN},	
	{0x0F12, 0x0078, WORD_LEN},	
	{0x0F12, 0x04FE, WORD_LEN},	
	{0x0F12, 0x8800, WORD_LEN},	
	                 
	{0x002A, 0x3274, WORD_LEN},	
	{0x0F12, 0x0155, WORD_LEN},		//set IO driving current 2mA for Gs500
	{0x0F12, 0x0155, WORD_LEN},	   //set IO driving current              
	{0x0F12, 0x1555, WORD_LEN},	   //set IO driving current              
	{0x0F12, 0x0555, WORD_LEN},	   //set IO driving current              

	{0x0028, 0x7000, WORD_LEN},	
	{0x002A, 0x0572, WORD_LEN},	
	{0x0F12, 0x0007, WORD_LEN},		//#skl_usConfigStbySettings // Enable T&P code after HW stby + skip ZI part on HW wakeup.
	                  
	{0x0028, 0x7000, WORD_LEN},		
	{0x002A, 0x12D2, WORD_LEN},		   
	{0x0F12, 0x0003, WORD_LEN},		//senHal_pContSenModesRegsArray[0][0]2 700012D2     
	{0x0F12, 0x0003, WORD_LEN},	   //senHal_pContSenModesRegsArray[0][1]2 700012D4  
	{0x0F12, 0x0003, WORD_LEN},	   //senHal_pContSenModesRegsArray[0][2]2 700012D6  
	{0x0F12, 0x0003, WORD_LEN},	   //senHal_pContSenModesRegsArray[0][3]2 700012D8  
	{0x0F12, 0x0884, WORD_LEN},	   //senHal_pContSenModesRegsArray[1][0]2 700012DA  
	{0x0F12, 0x08CF, WORD_LEN},	   //senHal_pContSenModesRegsArray[1][1]2 700012DC  
	{0x0F12, 0x0500, WORD_LEN},	   //senHal_pContSenModesRegsArray[1][2]2 700012DE  
	{0x0F12, 0x054B, WORD_LEN},	   //senHal_pContSenModesRegsArray[1][3]2 700012E0  
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[2][0]2 700012E2  
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[2][1]2 700012E4  
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[2][2]2 700012E6  
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[2][3]2 700012E8  
	{0x0F12, 0x0885, WORD_LEN},	   //senHal_pContSenModesRegsArray[3][0]2 700012EA  
	{0x0F12, 0x0467, WORD_LEN},	   //senHal_pContSenModesRegsArray[3][1]2 700012EC  
	{0x0F12, 0x0501, WORD_LEN},	   //senHal_pContSenModesRegsArray[3][2]2 700012EE  
	{0x0F12, 0x02A5, WORD_LEN},	   //senHal_pContSenModesRegsArray[3][3]2 700012F0  
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[4][0]2 700012F2  
	{0x0F12, 0x046A, WORD_LEN},	   //senHal_pContSenModesRegsArray[4][1]2 700012F4  
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[4][2]2 700012F6  
	{0x0F12, 0x02A8, WORD_LEN},	   //senHal_pContSenModesRegsArray[4][3]2 700012F8  
	{0x0F12, 0x0885, WORD_LEN},	   //senHal_pContSenModesRegsArray[5][0]2 700012FA  
	{0x0F12, 0x08D0, WORD_LEN},	   //senHal_pContSenModesRegsArray[5][1]2 700012FC  
	{0x0F12, 0x0501, WORD_LEN},	   //senHal_pContSenModesRegsArray[5][2]2 700012FE  
	{0x0F12, 0x054C, WORD_LEN},	   //senHal_pContSenModesRegsArray[5][3]2 70001300  
	{0x0F12, 0x0006, WORD_LEN},	   //senHal_pContSenModesRegsArray[6][0]2 70001302  
	{0x0F12, 0x0020, WORD_LEN},	   //senHal_pContSenModesRegsArray[6][1]2 70001304  
	{0x0F12, 0x0006, WORD_LEN},	   //senHal_pContSenModesRegsArray[6][2]2 70001306  
	{0x0F12, 0x0020, WORD_LEN},	   //senHal_pContSenModesRegsArray[6][3]2 70001308  
	{0x0F12, 0x0881, WORD_LEN},	   //senHal_pContSenModesRegsArray[7][0]2 7000130A  
	{0x0F12, 0x0463, WORD_LEN},	   //senHal_pContSenModesRegsArray[7][1]2 7000130C  
	{0x0F12, 0x04FD, WORD_LEN},	   //senHal_pContSenModesRegsArray[7][2]2 7000130E  
	{0x0F12, 0x02A1, WORD_LEN},	   //senHal_pContSenModesRegsArray[7][3]2 70001310  
	{0x0F12, 0x0006, WORD_LEN},	   //senHal_pContSenModesRegsArray[8][0]2 70001312  
	{0x0F12, 0x0489, WORD_LEN},	   //senHal_pContSenModesRegsArray[8][1]2 70001314  
	{0x0F12, 0x0006, WORD_LEN},	   //senHal_pContSenModesRegsArray[8][2]2 70001316  
	{0x0F12, 0x02C7, WORD_LEN},	   //senHal_pContSenModesRegsArray[8][3]2 70001318  
	{0x0F12, 0x0881, WORD_LEN},	   //senHal_pContSenModesRegsArray[9][0]2 7000131A  
	{0x0F12, 0x08CC, WORD_LEN},	   //senHal_pContSenModesRegsArray[9][1]2 7000131C  
	{0x0F12, 0x04FD, WORD_LEN},	   //senHal_pContSenModesRegsArray[9][2]2 7000131E  
	{0x0F12, 0x0548, WORD_LEN},	   //senHal_pContSenModesRegsArray[9][3]2 70001320  
	{0x0F12, 0x03A2, WORD_LEN},	   //senHal_pContSenModesRegsArray[10][0] 2 70001322
	{0x0F12, 0x01D3, WORD_LEN},	   //senHal_pContSenModesRegsArray[10][1] 2 70001324
	{0x0F12, 0x01E0, WORD_LEN},	   //senHal_pContSenModesRegsArray[10][2] 2 70001326
	{0x0F12, 0x00F2, WORD_LEN},	   //senHal_pContSenModesRegsArray[10][3] 2 70001328
	{0x0F12, 0x03F2, WORD_LEN},	   //senHal_pContSenModesRegsArray[11][0] 2 7000132A
	{0x0F12, 0x0223, WORD_LEN},	   //senHal_pContSenModesRegsArray[11][1] 2 7000132C
	{0x0F12, 0x0230, WORD_LEN},	   //senHal_pContSenModesRegsArray[11][2] 2 7000132E
	{0x0F12, 0x0142, WORD_LEN},	   //senHal_pContSenModesRegsArray[11][3] 2 70001330
	{0x0F12, 0x03A2, WORD_LEN},	   //senHal_pContSenModesRegsArray[12][0] 2 70001332
	{0x0F12, 0x063C, WORD_LEN},	   //senHal_pContSenModesRegsArray[12][1] 2 70001334
	{0x0F12, 0x01E0, WORD_LEN},	   //senHal_pContSenModesRegsArray[12][2] 2 70001336
	{0x0F12, 0x0399, WORD_LEN},	   //senHal_pContSenModesRegsArray[12][3] 2 70001338
	{0x0F12, 0x03F2, WORD_LEN},	   //senHal_pContSenModesRegsArray[13][0] 2 7000133A
	{0x0F12, 0x068C, WORD_LEN},	   //senHal_pContSenModesRegsArray[13][1] 2 7000133C
	{0x0F12, 0x0230, WORD_LEN},	   //senHal_pContSenModesRegsArray[13][2] 2 7000133E
	{0x0F12, 0x03E9, WORD_LEN},	   //senHal_pContSenModesRegsArray[13][3] 2 70001340
	{0x0F12, 0x0002, WORD_LEN},	   //senHal_pContSenModesRegsArray[14][0] 2 70001342
	{0x0F12, 0x0002, WORD_LEN},	   //senHal_pContSenModesRegsArray[14][1] 2 70001344
	{0x0F12, 0x0002, WORD_LEN},	   //senHal_pContSenModesRegsArray[14][2] 2 70001346
	{0x0F12, 0x0002, WORD_LEN},	   //senHal_pContSenModesRegsArray[14][3] 2 70001348
	{0x0F12, 0x003C, WORD_LEN},	   //senHal_pContSenModesRegsArray[15][0] 2 7000134A
	{0x0F12, 0x003C, WORD_LEN},	   //senHal_pContSenModesRegsArray[15][1] 2 7000134C
	{0x0F12, 0x003C, WORD_LEN},	   //senHal_pContSenModesRegsArray[15][2] 2 7000134E
	{0x0F12, 0x003C, WORD_LEN},	   //senHal_pContSenModesRegsArray[15][3] 2 70001350
	{0x0F12, 0x01D3, WORD_LEN},	   //senHal_pContSenModesRegsArray[16][0] 2 70001352
	{0x0F12, 0x01D3, WORD_LEN},	   //senHal_pContSenModesRegsArray[16][1] 2 70001354
	{0x0F12, 0x00F2, WORD_LEN},	   //senHal_pContSenModesRegsArray[16][2] 2 70001356
	{0x0F12, 0x00F2, WORD_LEN},	   //senHal_pContSenModesRegsArray[16][3] 2 70001358
	{0x0F12, 0x020B, WORD_LEN},	   //senHal_pContSenModesRegsArray[17][0] 2 7000135A
	{0x0F12, 0x024A, WORD_LEN},	   //senHal_pContSenModesRegsArray[17][1] 2 7000135C
	{0x0F12, 0x012A, WORD_LEN},	   //senHal_pContSenModesRegsArray[17][2] 2 7000135E
	{0x0F12, 0x0169, WORD_LEN},	   //senHal_pContSenModesRegsArray[17][3] 2 70001360
	{0x0F12, 0x0002, WORD_LEN},	   //senHal_pContSenModesRegsArray[18][0] 2 70001362
	{0x0F12, 0x046B, WORD_LEN},	   //senHal_pContSenModesRegsArray[18][1] 2 70001364
	{0x0F12, 0x0002, WORD_LEN},	   //senHal_pContSenModesRegsArray[18][2] 2 70001366
	{0x0F12, 0x02A9, WORD_LEN},	   //senHal_pContSenModesRegsArray[18][3] 2 70001368
	{0x0F12, 0x0419, WORD_LEN},	   //senHal_pContSenModesRegsArray[19][0] 2 7000136A
	{0x0F12, 0x04A5, WORD_LEN},	   //senHal_pContSenModesRegsArray[19][1] 2 7000136C
	{0x0F12, 0x0257, WORD_LEN},	   //senHal_pContSenModesRegsArray[19][2] 2 7000136E
	{0x0F12, 0x02E3, WORD_LEN},	   //senHal_pContSenModesRegsArray[19][3] 2 70001370
	{0x0F12, 0x0630, WORD_LEN},	   //senHal_pContSenModesRegsArray[20][0] 2 70001372
	{0x0F12, 0x063C, WORD_LEN},	   //senHal_pContSenModesRegsArray[20][1] 2 70001374
	{0x0F12, 0x038D, WORD_LEN},	   //senHal_pContSenModesRegsArray[20][2] 2 70001376
	{0x0F12, 0x0399, WORD_LEN},	   //senHal_pContSenModesRegsArray[20][3] 2 70001378
	{0x0F12, 0x0668, WORD_LEN},	   //senHal_pContSenModesRegsArray[21][0] 2 7000137A
	{0x0F12, 0x06B3, WORD_LEN},	   //senHal_pContSenModesRegsArray[21][1] 2 7000137C
	{0x0F12, 0x03C5, WORD_LEN},	   //senHal_pContSenModesRegsArray[21][2] 2 7000137E
	{0x0F12, 0x0410, WORD_LEN},	   //senHal_pContSenModesRegsArray[21][3] 2 70001380
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[22][0] 2 70001382
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[22][1] 2 70001384
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[22][2] 2 70001386
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[22][3] 2 70001388
	{0x0F12, 0x03A2, WORD_LEN},	   //senHal_pContSenModesRegsArray[23][0] 2 7000138A
	{0x0F12, 0x01D3, WORD_LEN},	   //senHal_pContSenModesRegsArray[23][1] 2 7000138C
	{0x0F12, 0x01E0, WORD_LEN},	   //senHal_pContSenModesRegsArray[23][2] 2 7000138E
	{0x0F12, 0x00F2, WORD_LEN},	   //senHal_pContSenModesRegsArray[23][3] 2 70001390
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[24][0] 2 70001392
	{0x0F12, 0x0461, WORD_LEN},	   //senHal_pContSenModesRegsArray[24][1] 2 70001394
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[24][2] 2 70001396
	{0x0F12, 0x029F, WORD_LEN},	   //senHal_pContSenModesRegsArray[24][3] 2 70001398
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[25][0] 2 7000139A
	{0x0F12, 0x063C, WORD_LEN},	   //senHal_pContSenModesRegsArray[25][1] 2 7000139C
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[25][2] 2 7000139E
	{0x0F12, 0x0399, WORD_LEN},	   //senHal_pContSenModesRegsArray[25][3] 2 700013A0
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[26][0] 2 700013A2
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[26][1] 2 700013A4
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[26][2] 2 700013A6
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[26][3] 2 700013A8
	{0x0F12, 0x01D0, WORD_LEN},	   //senHal_pContSenModesRegsArray[27][0] 2 700013AA
	{0x0F12, 0x01D0, WORD_LEN},	   //senHal_pContSenModesRegsArray[27][1] 2 700013AC
	{0x0F12, 0x00EF, WORD_LEN},	   //senHal_pContSenModesRegsArray[27][2] 2 700013AE
	{0x0F12, 0x00EF, WORD_LEN},	   //senHal_pContSenModesRegsArray[27][3] 2 700013B0
	{0x0F12, 0x020C, WORD_LEN},	   //senHal_pContSenModesRegsArray[28][0] 2 700013B2
	{0x0F12, 0x024B, WORD_LEN},	   //senHal_pContSenModesRegsArray[28][1] 2 700013B4
	{0x0F12, 0x012B, WORD_LEN},	   //senHal_pContSenModesRegsArray[28][2] 2 700013B6
	{0x0F12, 0x016A, WORD_LEN},	   //senHal_pContSenModesRegsArray[28][3] 2 700013B8
	{0x0F12, 0x039F, WORD_LEN},	   //senHal_pContSenModesRegsArray[29][0] 2 700013BA
	{0x0F12, 0x045E, WORD_LEN},	   //senHal_pContSenModesRegsArray[29][1] 2 700013BC
	{0x0F12, 0x01DD, WORD_LEN},	   //senHal_pContSenModesRegsArray[29][2] 2 700013BE
	{0x0F12, 0x029C, WORD_LEN},	   //senHal_pContSenModesRegsArray[29][3] 2 700013C0
	{0x0F12, 0x041A, WORD_LEN},	   //senHal_pContSenModesRegsArray[30][0] 2 700013C2
	{0x0F12, 0x04A6, WORD_LEN},	   //senHal_pContSenModesRegsArray[30][1] 2 700013C4
	{0x0F12, 0x0258, WORD_LEN},	   //senHal_pContSenModesRegsArray[30][2] 2 700013C6
	{0x0F12, 0x02E4, WORD_LEN},	   //senHal_pContSenModesRegsArray[30][3] 2 700013C8
	{0x0F12, 0x062D, WORD_LEN},	   //senHal_pContSenModesRegsArray[31][0] 2 700013CA
	{0x0F12, 0x0639, WORD_LEN},	   //senHal_pContSenModesRegsArray[31][1] 2 700013CC
	{0x0F12, 0x038A, WORD_LEN},	   //senHal_pContSenModesRegsArray[31][2] 2 700013CE
	{0x0F12, 0x0396, WORD_LEN},	   //senHal_pContSenModesRegsArray[31][3] 2 700013D0
	{0x0F12, 0x0669, WORD_LEN},	   //senHal_pContSenModesRegsArray[32][0] 2 700013D2
	{0x0F12, 0x06B4, WORD_LEN},	   //senHal_pContSenModesRegsArray[32][1] 2 700013D4
	{0x0F12, 0x03C6, WORD_LEN},	   //senHal_pContSenModesRegsArray[32][2] 2 700013D6
	{0x0F12, 0x0411, WORD_LEN},	   //senHal_pContSenModesRegsArray[32][3] 2 700013D8
	{0x0F12, 0x087C, WORD_LEN},	   //senHal_pContSenModesRegsArray[33][0] 2 700013DA
	{0x0F12, 0x08C7, WORD_LEN},	   //senHal_pContSenModesRegsArray[33][1] 2 700013DC
	{0x0F12, 0x04F8, WORD_LEN},	   //senHal_pContSenModesRegsArray[33][2] 2 700013DE
	{0x0F12, 0x0543, WORD_LEN},	   //senHal_pContSenModesRegsArray[33][3] 2 700013E0
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[34][0] 2 700013E2
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[34][1] 2 700013E4
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[34][2] 2 700013E6
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[34][3] 2 700013E8
	{0x0F12, 0x01D0, WORD_LEN},	   //senHal_pContSenModesRegsArray[35][0] 2 700013EA
	{0x0F12, 0x01D0, WORD_LEN},	   //senHal_pContSenModesRegsArray[35][1] 2 700013EC
	{0x0F12, 0x00EF, WORD_LEN},	   //senHal_pContSenModesRegsArray[35][2] 2 700013EE
	{0x0F12, 0x00EF, WORD_LEN},	   //senHal_pContSenModesRegsArray[35][3] 2 700013F0
	{0x0F12, 0x020F, WORD_LEN},	   //senHal_pContSenModesRegsArray[36][0] 2 700013F2
	{0x0F12, 0x024E, WORD_LEN},	   //senHal_pContSenModesRegsArray[36][1] 2 700013F4
	{0x0F12, 0x012E, WORD_LEN},	   //senHal_pContSenModesRegsArray[36][2] 2 700013F6
	{0x0F12, 0x016D, WORD_LEN},	   //senHal_pContSenModesRegsArray[36][3] 2 700013F8
	{0x0F12, 0x039F, WORD_LEN},	   //senHal_pContSenModesRegsArray[37][0] 2 700013FA
	{0x0F12, 0x045E, WORD_LEN},	   //senHal_pContSenModesRegsArray[37][1] 2 700013FC
	{0x0F12, 0x01DD, WORD_LEN},	   //senHal_pContSenModesRegsArray[37][2] 2 700013FE
	{0x0F12, 0x029C, WORD_LEN},	   //senHal_pContSenModesRegsArray[37][3] 2 70001400
	{0x0F12, 0x041D, WORD_LEN},	   //senHal_pContSenModesRegsArray[38][0] 2 70001402
	{0x0F12, 0x04A9, WORD_LEN},	   //senHal_pContSenModesRegsArray[38][1] 2 70001404
	{0x0F12, 0x025B, WORD_LEN},	   //senHal_pContSenModesRegsArray[38][2] 2 70001406
	{0x0F12, 0x02E7, WORD_LEN},	   //senHal_pContSenModesRegsArray[38][3] 2 70001408
	{0x0F12, 0x062D, WORD_LEN},	   //senHal_pContSenModesRegsArray[39][0] 2 7000140A
	{0x0F12, 0x0639, WORD_LEN},	   //senHal_pContSenModesRegsArray[39][1] 2 7000140C
	{0x0F12, 0x038A, WORD_LEN},	   //senHal_pContSenModesRegsArray[39][2] 2 7000140E
	{0x0F12, 0x0396, WORD_LEN},	   //senHal_pContSenModesRegsArray[39][3] 2 70001410
	{0x0F12, 0x066C, WORD_LEN},	   //senHal_pContSenModesRegsArray[40][0] 2 70001412
	{0x0F12, 0x06B7, WORD_LEN},	   //senHal_pContSenModesRegsArray[40][1] 2 70001414
	{0x0F12, 0x03C9, WORD_LEN},	   //senHal_pContSenModesRegsArray[40][2] 2 70001416
	{0x0F12, 0x0414, WORD_LEN},	   //senHal_pContSenModesRegsArray[40][3] 2 70001418
	{0x0F12, 0x087C, WORD_LEN},	   //senHal_pContSenModesRegsArray[41][0] 2 7000141A
	{0x0F12, 0x08C7, WORD_LEN},	   //senHal_pContSenModesRegsArray[41][1] 2 7000141C
	{0x0F12, 0x04F8, WORD_LEN},	   //senHal_pContSenModesRegsArray[41][2] 2 7000141E
	{0x0F12, 0x0543, WORD_LEN},	   //senHal_pContSenModesRegsArray[41][3] 2 70001420
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[42][0] 2 70001422
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[42][1] 2 70001424
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[42][2] 2 70001426
	{0x0F12, 0x0040, WORD_LEN},	   //senHal_pContSenModesRegsArray[42][3] 2 70001428
	{0x0F12, 0x01D0, WORD_LEN},	   //senHal_pContSenModesRegsArray[43][0] 2 7000142A
	{0x0F12, 0x01D0, WORD_LEN},	   //senHal_pContSenModesRegsArray[43][1] 2 7000142C
	{0x0F12, 0x00EF, WORD_LEN},	   //senHal_pContSenModesRegsArray[43][2] 2 7000142E
	{0x0F12, 0x00EF, WORD_LEN},	   //senHal_pContSenModesRegsArray[43][3] 2 70001430
	{0x0F12, 0x020F, WORD_LEN},	   //senHal_pContSenModesRegsArray[44][0] 2 70001432
	{0x0F12, 0x024E, WORD_LEN},	   //senHal_pContSenModesRegsArray[44][1] 2 70001434
	{0x0F12, 0x012E, WORD_LEN},	   //senHal_pContSenModesRegsArray[44][2] 2 70001436
	{0x0F12, 0x016D, WORD_LEN},	   //senHal_pContSenModesRegsArray[44][3] 2 70001438
	{0x0F12, 0x039F, WORD_LEN},	   //senHal_pContSenModesRegsArray[45][0] 2 7000143A
	{0x0F12, 0x045E, WORD_LEN},	   //senHal_pContSenModesRegsArray[45][1] 2 7000143C
	{0x0F12, 0x01DD, WORD_LEN},	   //senHal_pContSenModesRegsArray[45][2] 2 7000143E
	{0x0F12, 0x029C, WORD_LEN},	   //senHal_pContSenModesRegsArray[45][3] 2 70001440
	{0x0F12, 0x041D, WORD_LEN},	   //senHal_pContSenModesRegsArray[46][0] 2 70001442
	{0x0F12, 0x04A9, WORD_LEN},	   //senHal_pContSenModesRegsArray[46][1] 2 70001444
	{0x0F12, 0x025B, WORD_LEN},	   //senHal_pContSenModesRegsArray[46][2] 2 70001446
	{0x0F12, 0x02E7, WORD_LEN},	   //senHal_pContSenModesRegsArray[46][3] 2 70001448
	{0x0F12, 0x062D, WORD_LEN},	   //senHal_pContSenModesRegsArray[47][0] 2 7000144A
	{0x0F12, 0x0639, WORD_LEN},	   //senHal_pContSenModesRegsArray[47][1] 2 7000144C
	{0x0F12, 0x038A, WORD_LEN},	   //senHal_pContSenModesRegsArray[47][2] 2 7000144E
	{0x0F12, 0x0396, WORD_LEN},	   //senHal_pContSenModesRegsArray[47][3] 2 70001450
	{0x0F12, 0x066C, WORD_LEN},	   //senHal_pContSenModesRegsArray[48][0] 2 70001452
	{0x0F12, 0x06B7, WORD_LEN},	   //senHal_pContSenModesRegsArray[48][1] 2 70001454
	{0x0F12, 0x03C9, WORD_LEN},	   //senHal_pContSenModesRegsArray[48][2] 2 70001456
	{0x0F12, 0x0414, WORD_LEN},	   //senHal_pContSenModesRegsArray[48][3] 2 70001458
	{0x0F12, 0x087C, WORD_LEN},	   //senHal_pContSenModesRegsArray[49][0] 2 7000145A
	{0x0F12, 0x08C7, WORD_LEN},	   //senHal_pContSenModesRegsArray[49][1] 2 7000145C
	{0x0F12, 0x04F8, WORD_LEN},	   //senHal_pContSenModesRegsArray[49][2] 2 7000145E
	{0x0F12, 0x0543, WORD_LEN},	   //senHal_pContSenModesRegsArray[49][3] 2 70001460
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[50][0] 2 70001462
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[50][1] 2 70001464
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[50][2] 2 70001466
	{0x0F12, 0x003D, WORD_LEN},	   //senHal_pContSenModesRegsArray[50][3] 2 70001468
	{0x0F12, 0x01D2, WORD_LEN},	   //senHal_pContSenModesRegsArray[51][0] 2 7000146A
	{0x0F12, 0x01D2, WORD_LEN},	   //senHal_pContSenModesRegsArray[51][1] 2 7000146C
	{0x0F12, 0x00F1, WORD_LEN},	   //senHal_pContSenModesRegsArray[51][2] 2 7000146E
	{0x0F12, 0x00F1, WORD_LEN},	   //senHal_pContSenModesRegsArray[51][3] 2 70001470
	{0x0F12, 0x020C, WORD_LEN},	   //senHal_pContSenModesRegsArray[52][0] 2 70001472
	{0x0F12, 0x024B, WORD_LEN},	   //senHal_pContSenModesRegsArray[52][1] 2 70001474
	{0x0F12, 0x012B, WORD_LEN},	   //senHal_pContSenModesRegsArray[52][2] 2 70001476
	{0x0F12, 0x016A, WORD_LEN},	   //senHal_pContSenModesRegsArray[52][3] 2 70001478
	{0x0F12, 0x03A1, WORD_LEN},	   //senHal_pContSenModesRegsArray[53][0] 2 7000147A
	{0x0F12, 0x0460, WORD_LEN},	   //senHal_pContSenModesRegsArray[53][1] 2 7000147C
	{0x0F12, 0x01DF, WORD_LEN},	   //senHal_pContSenModesRegsArray[53][2] 2 7000147E
	{0x0F12, 0x029E, WORD_LEN},	   //senHal_pContSenModesRegsArray[53][3] 2 70001480
	{0x0F12, 0x041A, WORD_LEN},	   //senHal_pContSenModesRegsArray[54][0] 2 70001482
	{0x0F12, 0x04A6, WORD_LEN},	   //senHal_pContSenModesRegsArray[54][1] 2 70001484
	{0x0F12, 0x0258, WORD_LEN},	   //senHal_pContSenModesRegsArray[54][2] 2 70001486
	{0x0F12, 0x02E4, WORD_LEN},	   //senHal_pContSenModesRegsArray[54][3] 2 70001488
	{0x0F12, 0x062F, WORD_LEN},	   //senHal_pContSenModesRegsArray[55][0] 2 7000148A
	{0x0F12, 0x063B, WORD_LEN},	   //senHal_pContSenModesRegsArray[55][1] 2 7000148C
	{0x0F12, 0x038C, WORD_LEN},	   //senHal_pContSenModesRegsArray[55][2] 2 7000148E
	{0x0F12, 0x0398, WORD_LEN},	   //senHal_pContSenModesRegsArray[55][3] 2 70001490
	{0x0F12, 0x0669, WORD_LEN},	   //senHal_pContSenModesRegsArray[56][0] 2 70001492
	{0x0F12, 0x06B4, WORD_LEN},	   //senHal_pContSenModesRegsArray[56][1] 2 70001494
	{0x0F12, 0x03C6, WORD_LEN},	   //senHal_pContSenModesRegsArray[56][2] 2 70001496
	{0x0F12, 0x0411, WORD_LEN},	   //senHal_pContSenModesRegsArray[56][3] 2 70001498
	{0x0F12, 0x087E, WORD_LEN},	   //senHal_pContSenModesRegsArray[57][0] 2 7000149A
	{0x0F12, 0x08C9, WORD_LEN},	   //senHal_pContSenModesRegsArray[57][1] 2 7000149C
	{0x0F12, 0x04FA, WORD_LEN},	   //senHal_pContSenModesRegsArray[57][2] 2 7000149E
	{0x0F12, 0x0545, WORD_LEN},	   //senHal_pContSenModesRegsArray[57][3] 2 700014A0
	{0x0F12, 0x03A2, WORD_LEN},	   //senHal_pContSenModesRegsArray[58][0] 2 700014A2
	{0x0F12, 0x01D3, WORD_LEN},	   //senHal_pContSenModesRegsArray[58][1] 2 700014A4
	{0x0F12, 0x01E0, WORD_LEN},	   //senHal_pContSenModesRegsArray[58][2] 2 700014A6
	{0x0F12, 0x00F2, WORD_LEN},	   //senHal_pContSenModesRegsArray[58][3] 2 700014A8
	{0x0F12, 0x03AF, WORD_LEN},	   //senHal_pContSenModesRegsArray[59][0] 2 700014AA
	{0x0F12, 0x01E0, WORD_LEN},	   //senHal_pContSenModesRegsArray[59][1] 2 700014AC
	{0x0F12, 0x01ED, WORD_LEN},	   //senHal_pContSenModesRegsArray[59][2] 2 700014AE
	{0x0F12, 0x00FF, WORD_LEN},	   //senHal_pContSenModesRegsArray[59][3] 2 700014B0
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[60][0] 2 700014B2
	{0x0F12, 0x0461, WORD_LEN},	   //senHal_pContSenModesRegsArray[60][1] 2 700014B4
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[60][2] 2 700014B6
	{0x0F12, 0x029F, WORD_LEN},	   //senHal_pContSenModesRegsArray[60][3] 2 700014B8
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[61][0] 2 700014BA
	{0x0F12, 0x046E, WORD_LEN},	   //senHal_pContSenModesRegsArray[61][1] 2 700014BC
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[61][2] 2 700014BE
	{0x0F12, 0x02AC, WORD_LEN},	   //senHal_pContSenModesRegsArray[61][3] 2 700014C0
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[62][0] 2 700014C2
	{0x0F12, 0x063C, WORD_LEN},	   //senHal_pContSenModesRegsArray[62][1] 2 700014C4
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[62][2] 2 700014C6
	{0x0F12, 0x0399, WORD_LEN},	   //senHal_pContSenModesRegsArray[62][3] 2 700014C8
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[63][0] 2 700014CA
	{0x0F12, 0x0649, WORD_LEN},	   //senHal_pContSenModesRegsArray[63][1] 2 700014CC
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[63][2] 2 700014CE
	{0x0F12, 0x03A6, WORD_LEN},	   //senHal_pContSenModesRegsArray[63][3] 2 700014D0
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[64][0] 2 700014D2
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[64][1] 2 700014D4
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[64][2] 2 700014D6
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[64][3] 2 700014D8
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[65][0] 2 700014DA
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[65][1] 2 700014DC
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[65][2] 2 700014DE
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[65][3] 2 700014E0
	{0x0F12, 0x03AA, WORD_LEN},	   //senHal_pContSenModesRegsArray[66][0] 2 700014E2
	{0x0F12, 0x01DB, WORD_LEN},	   //senHal_pContSenModesRegsArray[66][1] 2 700014E4
	{0x0F12, 0x01E8, WORD_LEN},	   //senHal_pContSenModesRegsArray[66][2] 2 700014E6
	{0x0F12, 0x00FA, WORD_LEN},	   //senHal_pContSenModesRegsArray[66][3] 2 700014E8
	{0x0F12, 0x03B7, WORD_LEN},	   //senHal_pContSenModesRegsArray[67][0] 2 700014EA
	{0x0F12, 0x01E8, WORD_LEN},	   //senHal_pContSenModesRegsArray[67][1] 2 700014EC
	{0x0F12, 0x01F5, WORD_LEN},	   //senHal_pContSenModesRegsArray[67][2] 2 700014EE
	{0x0F12, 0x0107, WORD_LEN},	   //senHal_pContSenModesRegsArray[67][3] 2 700014F0
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[68][0] 2 700014F2
	{0x0F12, 0x0469, WORD_LEN},	   //senHal_pContSenModesRegsArray[68][1] 2 700014F4
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[68][2] 2 700014F6
	{0x0F12, 0x02A7, WORD_LEN},	   //senHal_pContSenModesRegsArray[68][3] 2 700014F8
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[69][0] 2 700014FA
	{0x0F12, 0x0476, WORD_LEN},	   //senHal_pContSenModesRegsArray[69][1] 2 700014FC
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[69][2] 2 700014FE
	{0x0F12, 0x02B4, WORD_LEN},	   //senHal_pContSenModesRegsArray[69][3] 2 70001500
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[70][0] 2 70001502
	{0x0F12, 0x0644, WORD_LEN},	   //senHal_pContSenModesRegsArray[70][1] 2 70001504
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[70][2] 2 70001506
	{0x0F12, 0x03A1, WORD_LEN},	   //senHal_pContSenModesRegsArray[70][3] 2 70001508
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[71][0] 2 7000150A
	{0x0F12, 0x0651, WORD_LEN},	   //senHal_pContSenModesRegsArray[71][1] 2 7000150C
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[71][2] 2 7000150E
	{0x0F12, 0x03AE, WORD_LEN},	   //senHal_pContSenModesRegsArray[71][3] 2 70001510
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[72][0] 2 70001512
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[72][1] 2 70001514
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[72][2] 2 70001516
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[72][3] 2 70001518
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[73][0] 2 7000151A
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[73][1] 2 7000151C
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[73][2] 2 7000151E
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[73][3] 2 70001520
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[74][0] 2 70001522
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[74][1] 2 70001524
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[74][2] 2 70001526
	{0x0F12, 0x0001, WORD_LEN},	   //senHal_pContSenModesRegsArray[74][3] 2 70001528
	{0x0F12, 0x000F, WORD_LEN},	   //senHal_pContSenModesRegsArray[75][0] 2 7000152A
	{0x0F12, 0x000F, WORD_LEN},	   //senHal_pContSenModesRegsArray[75][1] 2 7000152C
	{0x0F12, 0x000F, WORD_LEN},	   //senHal_pContSenModesRegsArray[75][2] 2 7000152E
	{0x0F12, 0x000F, WORD_LEN},	   //senHal_pContSenModesRegsArray[75][3] 2 70001530
	{0x0F12, 0x05AD, WORD_LEN},	   //senHal_pContSenModesRegsArray[76][0] 2 70001532
	{0x0F12, 0x03DE, WORD_LEN},	   //senHal_pContSenModesRegsArray[76][1] 2 70001534
	{0x0F12, 0x030A, WORD_LEN},	   //senHal_pContSenModesRegsArray[76][2] 2 70001536
	{0x0F12, 0x021C, WORD_LEN},	   //senHal_pContSenModesRegsArray[76][3] 2 70001538
	{0x0F12, 0x062F, WORD_LEN},	   //senHal_pContSenModesRegsArray[77][0] 2 7000153A
	{0x0F12, 0x0460, WORD_LEN},	   //senHal_pContSenModesRegsArray[77][1] 2 7000153C
	{0x0F12, 0x038C, WORD_LEN},	   //senHal_pContSenModesRegsArray[77][2] 2 7000153E
	{0x0F12, 0x029E, WORD_LEN},	   //senHal_pContSenModesRegsArray[77][3] 2 70001540
	{0x0F12, 0x07FC, WORD_LEN},	   //senHal_pContSenModesRegsArray[78][0] 2 70001542
	{0x0F12, 0x0847, WORD_LEN},	   //senHal_pContSenModesRegsArray[78][1] 2 70001544
	{0x0F12, 0x0478, WORD_LEN},	   //senHal_pContSenModesRegsArray[78][2] 2 70001546
	{0x0F12, 0x04C3, WORD_LEN},	   //senHal_pContSenModesRegsArray[78][3] 2 70001548
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[79][0] 2 7000154A
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[79][1] 2 7000154C
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[79][2] 2 7000154E
	{0x0F12, 0x0000, WORD_LEN},	   //senHal_pContSenModesRegsArray[79][3] 2 70001550


	//============================================================ 
	// AF Interface setting
	//============================================================ 
	{0x002A, 0x01D4, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	 //REG_TC_IPRM_AuxGpios : 0 - no Flash
	{0x002A, 0x01DE, WORD_LEN},	
	{0x0F12, 0x0003, WORD_LEN},	 //REG_TC_IPRM_CM_Init_AfModeType : 3 - AFD_VCM_I2C
	{0x0F12, 0x0000, WORD_LEN},	 //REG_TC_IPRM_CM_Init_PwmConfig1 : 0 - no PWM
	{0x002A, 0x01E4, WORD_LEN},	
	{0x0F12, 0x0041, WORD_LEN},	 //REG_TC_IPRM_CM_Init_GpioConfig1 : 4 -  GPIO4 
	{0x002A, 0x01E8, WORD_LEN},	
	{0x0F12, 0x2A0C, WORD_LEN},	 //REG_TC_IPRM_CM_Init_Mi2cBits : MSCL - GPIO1 MSDA - GPIO2 Device ID (0C)
	{0x0F12, 0x0190, WORD_LEN},	 //REG_TC_IPRM_CM_Init_Mi2cRateKhz : MI2C Speed - 400KHz

	//============================================================ 
	// AF Parameter setting
	//============================================================ 
	// AF Window Settings
	{0x002A, 0x025A, WORD_LEN},	
	{0x0F12, 0x0100, WORD_LEN},	 //#REG_TC_AF_FstWinStartX
	{0x0F12, 0x0110, WORD_LEN},	 //#REG_TC_AF_FstWinStartY
	{0x0F12, 0x0200, WORD_LEN},	 //#REG_TC_AF_FstWinSizeX
	{0x0F12, 0x0238, WORD_LEN},	 //#REG_TC_AF_FstWinSizeY
	{0x0F12, 0x018C, WORD_LEN},	 //#REG_TC_AF_ScndWinStartX
	{0x0F12, 0x01C0, WORD_LEN},	 //#REG_TC_AF_ScndWinStartY
	{0x0F12, 0x00E6, WORD_LEN},	 //#REG_TC_AF_ScndWinSizeX
	{0x0F12, 0x0132, WORD_LEN},	 //#REG_TC_AF_ScndWinSizeY
	{0x0F12, 0x0001, WORD_LEN},	 //#REG_TC_AF_WinSizesUpdated

	// AF Setot Settings 
	{0x002A, 0x0586, WORD_LEN},	
	{0x0F12, 0x00FF, WORD_LEN},	 //#skl_af_StatOvlpExpFactor

	// AF Scene Settings 
	{0x002A, 0x115E, WORD_LEN},	
	{0x0F12, 0x0003, WORD_LEN},	 //#af_scene_usSaturatedScene

	// AF Fine Search Settings 
	{0x002A, 0x10D4, WORD_LEN},	
	{0x0F12, 0x1000, WORD_LEN},	 //FineSearch Disable //#af_search_usSingleAfFlags
	{0x002A, 0x10DE, WORD_LEN},	
	{0x0F12, 0x0004, WORD_LEN},	 //#af_search_usFinePeakCount
	{0x002A, 0x106C, WORD_LEN},	
	{0x0F12, 0x0202, WORD_LEN},	 //#af_pos_usFineStepNumSize

	// AF Peak Threshold Setting
	{0x002A, 0x10CA, WORD_LEN},	 //#af_search_usPeakThr
	{0x0F12, 0x00C0, WORD_LEN},	 

	// AF Default Position 
	{0x002A, 0x1060, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	 //#af_pos_usHomePos
	{0x0F12, 0xB300, WORD_LEN},	 //#af_pos_usLowConfPos

	// AF LowConfThr Setting
	{0x002A, 0x10F4, WORD_LEN},	 //LowEdgeBoth GRAD
	{0x0F12, 0x0280, WORD_LEN},	 
	{0x002A, 0x1100, WORD_LEN},	 //LowLight HPF
	{0x0F12, 0x03A0, WORD_LEN},	  
	{0x0F12, 0x0320, WORD_LEN},	 

	{0x002A, 0x1134, WORD_LEN},	
	{0x0F12, 0x0030, WORD_LEN},	//af_stat_usMinStatVal

	// AF low Br Th
	{0x002A, 0x1154, WORD_LEN},	 // normBrThr
	{0x0F12, 0x0060, WORD_LEN},	

	// AF Policy
	{0x002A, 0x10E2, WORD_LEN},	
	{0x0F12, 0x0000, WORD_LEN},	 //#af_search_usCapturePolicy: Focus_Priority, 0002 : Shutter_Priority_Fixed, 0001 : Shutter_Priority_Last_BFP 0000: Shutter_Priority_Current
	{0x002A, 0x1072, WORD_LEN},	
	{0x0F12, 0x003C, WORD_LEN},	 //#af_pos_usCaptureFixedPo// 0x0008

	// AF Lens Position Table Settings 
	{0x002A, 0x1074, WORD_LEN},	
	{0x0F12, 0x0013, WORD_LEN},	 //#af_pos_usTableLastInd// 0~20 16 Steps 091222
	{0x0F12, 0x002C, WORD_LEN},	 //#af_pos_usTable_0_// af_pos_usTable          
	{0x0F12, 0x0030, WORD_LEN},	 //#af_pos_usTable_1_                           
	{0x0F12, 0x0033, WORD_LEN},	 //#af_pos_usTable_2_                           
	{0x0F12, 0x0036, WORD_LEN},	 //#af_pos_usTable_3_                           
	{0x0F12, 0x0039, WORD_LEN},	 //#af_pos_usTable_4_                           
	{0x0F12, 0x003D, WORD_LEN},	 //#af_pos_usTable_5_                           
	{0x0F12, 0x0041, WORD_LEN},	 //#af_pos_usTable_6_                           
	{0x0F12, 0x0045, WORD_LEN},	 //#af_pos_usTable_7_                           
	{0x0F12, 0x0049, WORD_LEN},	 //#af_pos_usTable_8_                           
	{0x0F12, 0x004F, WORD_LEN},	 //#af_pos_usTable_9_                           
	{0x0F12, 0x0055, WORD_LEN},	 //#af_pos_usTable_10_                          
	{0x0F12, 0x005D, WORD_LEN},	 //#af_pos_usTable_11_                          
	{0x0F12, 0x0065, WORD_LEN},	 //#af_pos_usTable_12_                          
	{0x0F12, 0x006D, WORD_LEN},	 //#af_pos_usTable_13_                          
	{0x0F12, 0x0077, WORD_LEN},	 //#af_pos_usTable_14_                          
	{0x0F12, 0x0083, WORD_LEN},	 //#af_pos_usTable_15_                          
	{0x0F12, 0x008F, WORD_LEN},	 //#af_pos_usTable_16_  
	{0x0F12, 0x009B, WORD_LEN},	 //#af_pos_usTable_17_                          
	{0x0F12, 0x00A7, WORD_LEN},	 //#af_pos_usTable_18_  
	{0x0F12, 0x00B3, WORD_LEN},	 //#af_pos_usTable_19_  

	{0x002A, 0x0252, WORD_LEN},	
	{0x0F12, 0x0003, WORD_LEN},	 //init 

	{0x002A, 0x12B8, WORD_LEN},	   //disable CINTR 0                                 
	{0x0F12, 0x1000, WORD_LEN},	        

	//============================================================
	// ISP-FE Setting
	//============================================================                    
	{0x002A, 0x158A, WORD_LEN},	   
	{0x0F12, 0xEAF0, WORD_LEN},	   
	{0x002A, 0x15C6, WORD_LEN},	   
	{0x0F12, 0x0020, WORD_LEN},	   
	{0x0F12, 0x0060, WORD_LEN},	   
	{0x002A, 0x15BC, WORD_LEN},	   
	{0x0F12, 0x0200, WORD_LEN},	   
	                    
	{0x002A, 0x1608, WORD_LEN},	   
	{0x0F12, 0x0100, WORD_LEN},	   
	{0x0F12, 0x0100, WORD_LEN},	   
	{0x0F12, 0x0100, WORD_LEN},	   
	{0x0F12, 0x0100, WORD_LEN},	   
	                    
	{0x002A, 0x0F70, WORD_LEN},	   
	{0x0F12, 0x0040, WORD_LEN},		 //36 //TVAR_ae_BrAve  //ae Target//
	{0x002A, 0x0530, WORD_LEN},	   	                                                    
	{0x0F12, 0x3415, WORD_LEN},	   //3A98 //3A98////lt_uMaxExp1	32 30ms  9~10ea// 15fps  // 
	{0x002A, 0x0534, WORD_LEN},	                                                               
	{0x0F12, 0x682A, WORD_LEN},	   //68b0 //7EF4////lt_uMaxExp2	67 65ms	18~20ea // 7.5fps //
	{0x002A, 0x167C, WORD_LEN},	                                                               
	{0x0F12, 0x8235, WORD_LEN},	   //8340 //9C40//MaxExp3  83 80ms  24~25ea //                 
	{0x002A, 0x1680, WORD_LEN},	                                                               
	{0x0F12, 0xc350, WORD_LEN},	    //F424//MaxExp4   125ms  38ea //                           
	        
	{0x002A, 0x0538, WORD_LEN},	                                                               
	{0x0F12, 0x3415, WORD_LEN},	   // 15fps //                                                 
	{0x002A, 0x053C, WORD_LEN},	                                                               
	{0x0F12, 0x682A, WORD_LEN},	   // 7.5fps //                                                
	{0x002A, 0x1684, WORD_LEN},	                                                               
	{0x0F12, 0x8235, WORD_LEN},	   //CapMaxExp3 //                                             
	{0x002A, 0x1688, WORD_LEN},	                                                               
	{0x0F12, 0xC350, WORD_LEN},	   //CapMaxExp4 //                                             

	//Shutter tunpoint//        //gain * 256 = value//
	{0x002A, 0x0540, WORD_LEN},	                                                               
	{0x0F12, 0x01B3, WORD_LEN},	    //0170//0150//lt_uMaxAnGain1_700lux//                                              
	{0x0F12, 0x01B3, WORD_LEN},	   //0200//0400//lt_uMaxAnGain2_400lux//                              
	{0x002A, 0x168C, WORD_LEN},	                                                               
	{0x0F12, 0x02A0, WORD_LEN},	   //0300//MaxAnGain3_200lux//                                       
	{0x0F12, 0x0710, WORD_LEN},	   //MaxAnGain4 //                                             
	//Shutter tunend//
	        
	{0x002A, 0x0544, WORD_LEN},	                                                               
	{0x0F12, 0x0100, WORD_LEN},	   	                                                    
	{0x0F12, 0x8000, WORD_LEN},	   //Max Gain 8 //                                             
	        
	        
	{0x002A, 0x1694, WORD_LEN},	                                                               
	{0x0F12, 0x0001, WORD_LEN},	   //expand forbidde zone //                                   

	{0x002A, 0x021A, WORD_LEN},	                                                               
	{0x0F12, 0x0000, WORD_LEN},	  //MBR off// 	                                                    


	//==============================================//
	//AFC                                          //
	//==============================================//
	{0x002A, 0x04d2, WORD_LEN},	
	{0x0F12, 0x065f, WORD_LEN},	     //065f : Manual AFC on   067f : Manual AFC off //
	{0x002A, 0x04ba, WORD_LEN},	     
	{0x0F12, 0x0001, WORD_LEN},	     // 0002: 60hz  0001 : 50hz //
	{0x0F12, 0x0001, WORD_LEN},	     // afc update command //
	                         
	                    
	                    
	{0x002A, 0x06CE, WORD_LEN},	   
	{0x0F12, 0x0100, WORD_LEN},		//TVAR_ash_GAsalpha[0] // 	   
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[1] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[2] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[3] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[4] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[5] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[6] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[7] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[8] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[9] // 
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[10] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[11] //
	{0x0F12, 0x00e0, WORD_LEN},	   //TVAR_ash_GAsalpha[12] //
	{0x0F12, 0x00f8, WORD_LEN},	   //TVAR_ash_GAsalpha[13] //
	{0x0F12, 0x00f8, WORD_LEN},	   //TVAR_ash_GAsalpha[14] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[15] //
	{0x0F12, 0x00e8, WORD_LEN},	   //TVAR_ash_GAsalpha[16] //
	{0x0F12, 0x00f8, WORD_LEN},	   //TVAR_ash_GAsalpha[17] //
	{0x0F12, 0x00f8, WORD_LEN},	   //TVAR_ash_GAsalpha[18] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[19] //
	{0x0F12, 0x00f0, WORD_LEN},	   //TVAR_ash_GAsalpha[20] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[21] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[22] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[23] //
	{0x0F12, 0x00f0, WORD_LEN},	   //TVAR_ash_GAsalpha[24] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[25] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[26] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAsalpha[27] //
	                    
	{0x0F12, 0x00f8, WORD_LEN},		//TVAR_ash_GAS OutdoorAlpha[0] //   
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAS OutdoorAlpha[1] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAS OutdoorAlpha[2] //
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_GAS OutdoorAlpha[3] //
	                    
	                    
	{0x0F12, 0x0036, WORD_LEN},		//ash_GASBeta[0] // 	   
	{0x0F12, 0x001F, WORD_LEN},		//ash_GASBeta[1] //    
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[2] // 
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GASBeta[3] // 
	{0x0F12, 0x0036, WORD_LEN},	   //ash_GASBeta[4] // 
	{0x0F12, 0x001F, WORD_LEN},	   //ash_GASBeta[5] // 
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[6] // 
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GASBeta[7] // 
	{0x0F12, 0x0036, WORD_LEN},	   //ash_GASBeta[8] // 
	{0x0F12, 0x001F, WORD_LEN},	   //ash_GASBeta[9] // 
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[10] //
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GASBeta[11] //
	{0x0F12, 0x0010, WORD_LEN},	   //ash_GASBeta[12] //
	{0x0F12, 0x001F, WORD_LEN},	   //ash_GASBeta[13] //
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[14] //
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GASBeta[15] //
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[16] //
	{0x0F12, 0x001F, WORD_LEN},	   //ash_GASBeta[17] //
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[18] //
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GASBeta[19] //
	{0x0F12, 0x0036, WORD_LEN},	   //ash_GASBeta[20] //
	{0x0F12, 0x001F, WORD_LEN},	   //ash_GASBeta[21] //
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[22] //
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GASBeta[23] //
	{0x0F12, 0x0036, WORD_LEN},	   //ash_GASBeta[24] //
	{0x0F12, 0x001F, WORD_LEN},	   //ash_GASBeta[25] //
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GASBeta[26] //
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GASBeta[27] //
	                    
	{0x0F12, 0x0036, WORD_LEN},		//ash_GAS OutdoorBeta[0] //   
	{0x0F12, 0x001F, WORD_LEN},	   //ash_GAS OutdoorBeta[1] //
	{0x0F12, 0x0020, WORD_LEN},	   //ash_GAS OutdoorBeta[2] //
	{0x0F12, 0x0000, WORD_LEN},	   //ash_GAS OutdoorBeta[3] //
	                    
	{0x002A, 0x075A, WORD_LEN},		//ash_bParabolicEstimation//   
	{0x0F12, 0x0000, WORD_LEN},	   //ash_uParabolicCenterX   //
	{0x0F12, 0x0400, WORD_LEN},	   //ash_uParabolicCenterY   //
	{0x0F12, 0x0300, WORD_LEN},	   //ash_uParabolicscalingA  //
	{0x0F12, 0x0010, WORD_LEN},	   //ash_uParabolicscalingB  //
	{0x0F12, 0x0011, WORD_LEN},	   
	                    
	{0x002A, 0x347C, WORD_LEN},	   
	{0x0F12, 0x01D9, WORD_LEN},		//TVAR_ash_pGAS[0] //     
	{0x0F12, 0x019D, WORD_LEN},	   //TVAR_ash_pGAS[1] //  
	{0x0F12, 0x015C, WORD_LEN},	   //TVAR_ash_pGAS[2] //  
	{0x0F12, 0x0125, WORD_LEN},	   //TVAR_ash_pGAS[3] //  
	{0x0F12, 0x00FE, WORD_LEN},	   //TVAR_ash_pGAS[4] //  
	{0x0F12, 0x00E5, WORD_LEN},	   //TVAR_ash_pGAS[5] //  
	{0x0F12, 0x00DA, WORD_LEN},	   //TVAR_ash_pGAS[6] //  
	{0x0F12, 0x00E5, WORD_LEN},	   //TVAR_ash_pGAS[7] //  
	{0x0F12, 0x0100, WORD_LEN},	   //TVAR_ash_pGAS[8] //  
	{0x0F12, 0x012D, WORD_LEN},	   //TVAR_ash_pGAS[9] //  
	{0x0F12, 0x016B, WORD_LEN},	   //TVAR_ash_pGAS[10] // 
	{0x0F12, 0x01B3, WORD_LEN},	   //TVAR_ash_pGAS[11] // 
	{0x0F12, 0x01F2, WORD_LEN},	   //TVAR_ash_pGAS[12] // 
	{0x0F12, 0x01A7, WORD_LEN},	   //TVAR_ash_pGAS[13] // 
	{0x0F12, 0x0165, WORD_LEN},	   //TVAR_ash_pGAS[14] // 
	{0x0F12, 0x011E, WORD_LEN},	   //TVAR_ash_pGAS[15] // 
	{0x0F12, 0x00E3, WORD_LEN},	   //TVAR_ash_pGAS[16] // 
	{0x0F12, 0x00B6, WORD_LEN},	   //TVAR_ash_pGAS[17] // 
	{0x0F12, 0x009C, WORD_LEN},	   //TVAR_ash_pGAS[18] // 
	{0x0F12, 0x0092, WORD_LEN},	   //TVAR_ash_pGAS[19] // 
	{0x0F12, 0x009D, WORD_LEN},	   //TVAR_ash_pGAS[20] // 
	{0x0F12, 0x00BB, WORD_LEN},	   //TVAR_ash_pGAS[21] // 
	{0x0F12, 0x00F0, WORD_LEN},	   //TVAR_ash_pGAS[22] // 
	{0x0F12, 0x0133, WORD_LEN},	   //TVAR_ash_pGAS[23] // 
	{0x0F12, 0x0182, WORD_LEN},	   //TVAR_ash_pGAS[24] // 
	{0x0F12, 0x01CD, WORD_LEN},	   //TVAR_ash_pGAS[25] // 
	{0x0F12, 0x0170, WORD_LEN},	   //TVAR_ash_pGAS[26] // 
	{0x0F12, 0x012A, WORD_LEN},	   //TVAR_ash_pGAS[27] // 
	{0x0F12, 0x00DC, WORD_LEN},	   //TVAR_ash_pGAS[28] // 
	{0x0F12, 0x009A, WORD_LEN},	   //TVAR_ash_pGAS[29] // 
	{0x0F12, 0x006E, WORD_LEN},	   //TVAR_ash_pGAS[30] // 
	{0x0F12, 0x0053, WORD_LEN},	   //TVAR_ash_pGAS[31] // 
	{0x0F12, 0x004A, WORD_LEN},	   //TVAR_ash_pGAS[32] // 
	{0x0F12, 0x0055, WORD_LEN},	   //TVAR_ash_pGAS[33] // 
	{0x0F12, 0x0076, WORD_LEN},	   //TVAR_ash_pGAS[34] // 
	{0x0F12, 0x00AC, WORD_LEN},	   //TVAR_ash_pGAS[35] // 
	{0x0F12, 0x00F5, WORD_LEN},	   //TVAR_ash_pGAS[36] // 
	{0x0F12, 0x0147, WORD_LEN},	   //TVAR_ash_pGAS[37] // 
	{0x0F12, 0x0196, WORD_LEN},	   //TVAR_ash_pGAS[38] // 
	{0x0F12, 0x014C, WORD_LEN},	   //TVAR_ash_pGAS[39] // 
	{0x0F12, 0x0102, WORD_LEN},	   //TVAR_ash_pGAS[40] // 
	{0x0F12, 0x00B1, WORD_LEN},	   //TVAR_ash_pGAS[41] // 
	{0x0F12, 0x006F, WORD_LEN},	   //TVAR_ash_pGAS[42] // 
	{0x0F12, 0x0041, WORD_LEN},	   //TVAR_ash_pGAS[43] // 
	{0x0F12, 0x0027, WORD_LEN},	   //TVAR_ash_pGAS[44] // 
	{0x0F12, 0x001F, WORD_LEN},	   //TVAR_ash_pGAS[45] // 
	{0x0F12, 0x002A, WORD_LEN},	   //TVAR_ash_pGAS[46] // 
	{0x0F12, 0x004B, WORD_LEN},	   //TVAR_ash_pGAS[47] // 
	{0x0F12, 0x0083, WORD_LEN},	   //TVAR_ash_pGAS[48] // 
	{0x0F12, 0x00CE, WORD_LEN},	   //TVAR_ash_pGAS[49] // 
	{0x0F12, 0x0128, WORD_LEN},	   //TVAR_ash_pGAS[50] // 
	{0x0F12, 0x0177, WORD_LEN},	   //TVAR_ash_pGAS[51] // 
	{0x0F12, 0x0133, WORD_LEN},	   //TVAR_ash_pGAS[52] // 
	{0x0F12, 0x00E6, WORD_LEN},	   //TVAR_ash_pGAS[53] // 
	{0x0F12, 0x0094, WORD_LEN},	   //TVAR_ash_pGAS[54] // 
	{0x0F12, 0x0052, WORD_LEN},	   //TVAR_ash_pGAS[55] // 
	{0x0F12, 0x0025, WORD_LEN},	   //TVAR_ash_pGAS[56] // 
	{0x0F12, 0x000C, WORD_LEN},	   //TVAR_ash_pGAS[57] // 
	{0x0F12, 0x0004, WORD_LEN},	   //TVAR_ash_pGAS[58] // 
	{0x0F12, 0x0010, WORD_LEN},	   //TVAR_ash_pGAS[59] // 
	{0x0F12, 0x0030, WORD_LEN},	   //TVAR_ash_pGAS[60] // 
	{0x0F12, 0x0069, WORD_LEN},	   //TVAR_ash_pGAS[61] // 
	{0x0F12, 0x00B6, WORD_LEN},	   //TVAR_ash_pGAS[62] // 
	{0x0F12, 0x0112, WORD_LEN},	   //TVAR_ash_pGAS[63] // 
	{0x0F12, 0x0168, WORD_LEN},	   //TVAR_ash_pGAS[64] // 
	{0x0F12, 0x012F, WORD_LEN},	   //TVAR_ash_pGAS[65] // 
	{0x0F12, 0x00E3, WORD_LEN},	   //TVAR_ash_pGAS[66] // 
	{0x0F12, 0x008E, WORD_LEN},	   //TVAR_ash_pGAS[67] // 
	{0x0F12, 0x004C, WORD_LEN},	   //TVAR_ash_pGAS[68] // 
	{0x0F12, 0x0020, WORD_LEN},	   //TVAR_ash_pGAS[69] // 
	{0x0F12, 0x0007, WORD_LEN},	   //TVAR_ash_pGAS[70] // 
	{0x0F12, 0x0000, WORD_LEN},	   //TVAR_ash_pGAS[71] // 
	{0x0F12, 0x000B, WORD_LEN},	   //TVAR_ash_pGAS[72] // 
	{0x0F12, 0x002D, WORD_LEN},	   //TVAR_ash_pGAS[73] // 
	{0x0F12, 0x0065, WORD_LEN},	   //TVAR_ash_pGAS[74] // 
	{0x0F12, 0x00B4, WORD_LEN},	   //TVAR_ash_pGAS[75] // 
	{0x0F12, 0x0114, WORD_LEN},	   //TVAR_ash_pGAS[76] // 
	{0x0F12, 0x016B, WORD_LEN},	   //TVAR_ash_pGAS[77] // 
	{0x0F12, 0x0138, WORD_LEN},	   //TVAR_ash_pGAS[78] // 
	{0x0F12, 0x00EB, WORD_LEN},	   //TVAR_ash_pGAS[79] // 
	{0x0F12, 0x0099, WORD_LEN},	   //TVAR_ash_pGAS[80] // 
	{0x0F12, 0x0058, WORD_LEN},	   //TVAR_ash_pGAS[81] // 
	{0x0F12, 0x002B, WORD_LEN},	   //TVAR_ash_pGAS[82] // 
	{0x0F12, 0x0012, WORD_LEN},	   //TVAR_ash_pGAS[83] // 
	{0x0F12, 0x000B, WORD_LEN},	   //TVAR_ash_pGAS[84] // 
	{0x0F12, 0x0017, WORD_LEN},	   //TVAR_ash_pGAS[85] // 
	{0x0F12, 0x0039, WORD_LEN},	   //TVAR_ash_pGAS[86] // 
	{0x0F12, 0x0074, WORD_LEN},	   //TVAR_ash_pGAS[87] // 
	{0x0F12, 0x00C2, WORD_LEN},	   //TVAR_ash_pGAS[88] // 
	{0x0F12, 0x0121, WORD_LEN},	   //TVAR_ash_pGAS[89] // 
	{0x0F12, 0x0177, WORD_LEN},	   //TVAR_ash_pGAS[90] // 
	{0x0F12, 0x0158, WORD_LEN},	   //TVAR_ash_pGAS[91] // 
	{0x0F12, 0x010C, WORD_LEN},	   //TVAR_ash_pGAS[92] // 
	{0x0F12, 0x00BC, WORD_LEN},	   //TVAR_ash_pGAS[93] // 
	{0x0F12, 0x007B, WORD_LEN},	   //TVAR_ash_pGAS[94] // 
	{0x0F12, 0x004E, WORD_LEN},	   //TVAR_ash_pGAS[95] // 
	{0x0F12, 0x0034, WORD_LEN},	   //TVAR_ash_pGAS[96] // 
	{0x0F12, 0x002D, WORD_LEN},	   //TVAR_ash_pGAS[97] // 
	{0x0F12, 0x003B, WORD_LEN},	   //TVAR_ash_pGAS[98] // 
	{0x0F12, 0x005E, WORD_LEN},	   //TVAR_ash_pGAS[99] // 
	{0x0F12, 0x0099, WORD_LEN},	   //TVAR_ash_pGAS[100] //
	{0x0F12, 0x00E7, WORD_LEN},	   //TVAR_ash_pGAS[101] //
	{0x0F12, 0x0145, WORD_LEN},	   //TVAR_ash_pGAS[102] //
	{0x0F12, 0x0197, WORD_LEN},	   //TVAR_ash_pGAS[103] //
	{0x0F12, 0x017E, WORD_LEN},	   //TVAR_ash_pGAS[104] //
	{0x0F12, 0x0138, WORD_LEN},	   //TVAR_ash_pGAS[105] //
	{0x0F12, 0x00EB, WORD_LEN},	   //TVAR_ash_pGAS[106] //
	{0x0F12, 0x00AC, WORD_LEN},	   //TVAR_ash_pGAS[107] //
	{0x0F12, 0x0080, WORD_LEN},	   //TVAR_ash_pGAS[108] //
	{0x0F12, 0x0067, WORD_LEN},	   //TVAR_ash_pGAS[109] //
	{0x0F12, 0x0061, WORD_LEN},	   //TVAR_ash_pGAS[110] //
	{0x0F12, 0x006E, WORD_LEN},	   //TVAR_ash_pGAS[111] //
	{0x0F12, 0x0091, WORD_LEN},	   //TVAR_ash_pGAS[112] //
	{0x0F12, 0x00CA, WORD_LEN},	   //TVAR_ash_pGAS[113] //
	{0x0F12, 0x0116, WORD_LEN},	   //TVAR_ash_pGAS[114] //
	{0x0F12, 0x016E, WORD_LEN},	   //TVAR_ash_pGAS[115] //
	{0x0F12, 0x01C0, WORD_LEN},	   //TVAR_ash_pGAS[116] //
	{0x0F12, 0x01BB, WORD_LEN},	   //TVAR_ash_pGAS[117] //
	{0x0F12, 0x0177, WORD_LEN},	   //TVAR_ash_pGAS[118] //
	{0x0F12, 0x0131, WORD_LEN},	   //TVAR_ash_pGAS[119] //
	{0x0F12, 0x00F7, WORD_LEN},	   //TVAR_ash_pGAS[120] //
	{0x0F12, 0x00CE, WORD_LEN},	   //TVAR_ash_pGAS[121] //
	{0x0F12, 0x00B6, WORD_LEN},	   //TVAR_ash_pGAS[122] //
	{0x0F12, 0x00AF, WORD_LEN},	   //TVAR_ash_pGAS[123] //
	{0x0F12, 0x00BD, WORD_LEN},	   //TVAR_ash_pGAS[124] //
	{0x0F12, 0x00DF, WORD_LEN},	   //TVAR_ash_pGAS[125] //
	{0x0F12, 0x0113, WORD_LEN},	   //TVAR_ash_pGAS[126] //
	{0x0F12, 0x0156, WORD_LEN},	   //TVAR_ash_pGAS[127] //
	{0x0F12, 0x01AA, WORD_LEN},	   //TVAR_ash_pGAS[128] //
	{0x0F12, 0x01F7, WORD_LEN},	   //TVAR_ash_pGAS[129] //
	{0x0F12, 0x01EF, WORD_LEN},	   //TVAR_ash_pGAS[130] //
	{0x0F12, 0x01B2, WORD_LEN},	   //TVAR_ash_pGAS[131] //
	{0x0F12, 0x0170, WORD_LEN},	   //TVAR_ash_pGAS[132] //
	{0x0F12, 0x013E, WORD_LEN},	   //TVAR_ash_pGAS[133] //
	{0x0F12, 0x0119, WORD_LEN},	   //TVAR_ash_pGAS[134] //
	{0x0F12, 0x0103, WORD_LEN},	   //TVAR_ash_pGAS[135] //
	{0x0F12, 0x00FF, WORD_LEN},	   //TVAR_ash_pGAS[136] //
	{0x0F12, 0x010D, WORD_LEN},	   //TVAR_ash_pGAS[137] //
	{0x0F12, 0x012B, WORD_LEN},	   //TVAR_ash_pGAS[138] //
	{0x0F12, 0x0158, WORD_LEN},	   //TVAR_ash_pGAS[139] //
	{0x0F12, 0x0194, WORD_LEN},	   //TVAR_ash_pGAS[140] //
	{0x0F12, 0x01DA, WORD_LEN},	   //TVAR_ash_pGAS[141] //
	{0x0F12, 0x0207, WORD_LEN},	   //TVAR_ash_pGAS[142] //
	{0x0F12, 0x0198, WORD_LEN},	   //TVAR_ash_pGAS[143] //
	{0x0F12, 0x0156, WORD_LEN},	   //TVAR_ash_pGAS[144] //
	{0x0F12, 0x011D, WORD_LEN},	   //TVAR_ash_pGAS[145] //
	{0x0F12, 0x00EF, WORD_LEN},	   //TVAR_ash_pGAS[146] //
	{0x0F12, 0x00C9, WORD_LEN},	   //TVAR_ash_pGAS[147] //
	{0x0F12, 0x00B2, WORD_LEN},	   //TVAR_ash_pGAS[148] //
	{0x0F12, 0x00A9, WORD_LEN},	   //TVAR_ash_pGAS[149] //
	{0x0F12, 0x00B1, WORD_LEN},	   //TVAR_ash_pGAS[150] //
	{0x0F12, 0x00C5, WORD_LEN},	   //TVAR_ash_pGAS[151] //
	{0x0F12, 0x00E7, WORD_LEN},	   //TVAR_ash_pGAS[152] //
	{0x0F12, 0x0113, WORD_LEN},	   //TVAR_ash_pGAS[153] //
	{0x0F12, 0x0152, WORD_LEN},	   //TVAR_ash_pGAS[154] //
	{0x0F12, 0x018C, WORD_LEN},	   //TVAR_ash_pGAS[155] //
	{0x0F12, 0x016E, WORD_LEN},	   //TVAR_ash_pGAS[156] //
	{0x0F12, 0x0127, WORD_LEN},	   //TVAR_ash_pGAS[157] //
	{0x0F12, 0x00E8, WORD_LEN},	   //TVAR_ash_pGAS[158] //
	{0x0F12, 0x00B6, WORD_LEN},	   //TVAR_ash_pGAS[159] //
	{0x0F12, 0x0090, WORD_LEN},	   //TVAR_ash_pGAS[160] //
	{0x0F12, 0x0078, WORD_LEN},	   //TVAR_ash_pGAS[161] //
	{0x0F12, 0x0070, WORD_LEN},	   //TVAR_ash_pGAS[162] //
	{0x0F12, 0x0078, WORD_LEN},	   //TVAR_ash_pGAS[163] //
	{0x0F12, 0x008F, WORD_LEN},	   //TVAR_ash_pGAS[164] //
	{0x0F12, 0x00B5, WORD_LEN},	   //TVAR_ash_pGAS[165] //
	{0x0F12, 0x00E7, WORD_LEN},	   //TVAR_ash_pGAS[166] //
	{0x0F12, 0x0126, WORD_LEN},	   //TVAR_ash_pGAS[167] //
	{0x0F12, 0x016C, WORD_LEN},	   //TVAR_ash_pGAS[168] //
	{0x0F12, 0x013D, WORD_LEN},	   //TVAR_ash_pGAS[169] //
	{0x0F12, 0x00F6, WORD_LEN},	   //TVAR_ash_pGAS[170] //
	{0x0F12, 0x00B5, WORD_LEN},	   //TVAR_ash_pGAS[171] //
	{0x0F12, 0x0080, WORD_LEN},	   //TVAR_ash_pGAS[172] //
	{0x0F12, 0x0058, WORD_LEN},	   //TVAR_ash_pGAS[173] //
	{0x0F12, 0x0040, WORD_LEN},	   //TVAR_ash_pGAS[174] //
	{0x0F12, 0x0038, WORD_LEN},	   //TVAR_ash_pGAS[175] //
	{0x0F12, 0x0042, WORD_LEN},	   //TVAR_ash_pGAS[176] //
	{0x0F12, 0x005B, WORD_LEN},	   //TVAR_ash_pGAS[177] //
	{0x0F12, 0x0083, WORD_LEN},	   //TVAR_ash_pGAS[178] //
	{0x0F12, 0x00B9, WORD_LEN},	   //TVAR_ash_pGAS[179] //
	{0x0F12, 0x00F9, WORD_LEN},	   //TVAR_ash_pGAS[180] //
	{0x0F12, 0x0141, WORD_LEN},	   //TVAR_ash_pGAS[181] //
	{0x0F12, 0x011E, WORD_LEN},	   //TVAR_ash_pGAS[182] //
	{0x0F12, 0x00D7, WORD_LEN},	   //TVAR_ash_pGAS[183] //
	{0x0F12, 0x0094, WORD_LEN},	   //TVAR_ash_pGAS[184] //
	{0x0F12, 0x005E, WORD_LEN},	   //TVAR_ash_pGAS[185] //
	{0x0F12, 0x0035, WORD_LEN},	   //TVAR_ash_pGAS[186] //
	{0x0F12, 0x001E, WORD_LEN},	   //TVAR_ash_pGAS[187] //
	{0x0F12, 0x0017, WORD_LEN},	   //TVAR_ash_pGAS[188] //
	{0x0F12, 0x0021, WORD_LEN},	   //TVAR_ash_pGAS[189] //
	{0x0F12, 0x003C, WORD_LEN},	   //TVAR_ash_pGAS[190] //
	{0x0F12, 0x0066, WORD_LEN},	   //TVAR_ash_pGAS[191] //
	{0x0F12, 0x009F, WORD_LEN},	   //TVAR_ash_pGAS[192] //
	{0x0F12, 0x00E2, WORD_LEN},	   //TVAR_ash_pGAS[193] //
	{0x0F12, 0x012A, WORD_LEN},	   //TVAR_ash_pGAS[194] //
	{0x0F12, 0x010A, WORD_LEN},	   //TVAR_ash_pGAS[195] //
	{0x0F12, 0x00C0, WORD_LEN},	   //TVAR_ash_pGAS[196] //
	{0x0F12, 0x007B, WORD_LEN},	   //TVAR_ash_pGAS[197] //
	{0x0F12, 0x0045, WORD_LEN},	   //TVAR_ash_pGAS[198] //
	{0x0F12, 0x001F, WORD_LEN},	   //TVAR_ash_pGAS[199] //
	{0x0F12, 0x0008, WORD_LEN},	   //TVAR_ash_pGAS[200] //
	{0x0F12, 0x0002, WORD_LEN},	   //TVAR_ash_pGAS[201] //
	{0x0F12, 0x000C, WORD_LEN},	   //TVAR_ash_pGAS[202] //
	{0x0F12, 0x0027, WORD_LEN},	   //TVAR_ash_pGAS[203] //
	{0x0F12, 0x0052, WORD_LEN},	   //TVAR_ash_pGAS[204] //
	{0x0F12, 0x008C, WORD_LEN},	   //TVAR_ash_pGAS[205] //
	{0x0F12, 0x00D2, WORD_LEN},	   //TVAR_ash_pGAS[206] //
	{0x0F12, 0x011E, WORD_LEN},	   //TVAR_ash_pGAS[207] //
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_ash_pGAS[208] //
	{0x0F12, 0x00BC, WORD_LEN},	   //TVAR_ash_pGAS[209] //
	{0x0F12, 0x0077, WORD_LEN},	   //TVAR_ash_pGAS[210] //
	{0x0F12, 0x0040, WORD_LEN},	   //TVAR_ash_pGAS[211] //
	{0x0F12, 0x001A, WORD_LEN},	   //TVAR_ash_pGAS[212] //
	{0x0F12, 0x0005, WORD_LEN},	   //TVAR_ash_pGAS[213] //
	{0x0F12, 0x0000, WORD_LEN},	   //TVAR_ash_pGAS[214] //
	{0x0F12, 0x000A, WORD_LEN},	   //TVAR_ash_pGAS[215] //
	{0x0F12, 0x0026, WORD_LEN},	   //TVAR_ash_pGAS[216] //
	{0x0F12, 0x0052, WORD_LEN},	   //TVAR_ash_pGAS[217] //
	{0x0F12, 0x008D, WORD_LEN},	   //TVAR_ash_pGAS[218] //
	{0x0F12, 0x00D5, WORD_LEN},	   //TVAR_ash_pGAS[219] //
	{0x0F12, 0x0121, WORD_LEN},	   //TVAR_ash_pGAS[220] //
	{0x0F12, 0x010A, WORD_LEN},	   //TVAR_ash_pGAS[221] //
	{0x0F12, 0x00C1, WORD_LEN},	   //TVAR_ash_pGAS[222] //
	{0x0F12, 0x007E, WORD_LEN},	   //TVAR_ash_pGAS[223] //
	{0x0F12, 0x0049, WORD_LEN},	   //TVAR_ash_pGAS[224] //
	{0x0F12, 0x0023, WORD_LEN},	   //TVAR_ash_pGAS[225] //
	{0x0F12, 0x000E, WORD_LEN},	   //TVAR_ash_pGAS[226] //
	{0x0F12, 0x0009, WORD_LEN},	   //TVAR_ash_pGAS[227] //
	{0x0F12, 0x0015, WORD_LEN},	   //TVAR_ash_pGAS[228] //
	{0x0F12, 0x0031, WORD_LEN},	   //TVAR_ash_pGAS[229] //
	{0x0F12, 0x005D, WORD_LEN},	   //TVAR_ash_pGAS[230] //
	{0x0F12, 0x0097, WORD_LEN},	   //TVAR_ash_pGAS[231] //
	{0x0F12, 0x00DF, WORD_LEN},	   //TVAR_ash_pGAS[232] //
	{0x0F12, 0x0129, WORD_LEN},	   //TVAR_ash_pGAS[233] //
	{0x0F12, 0x0121, WORD_LEN},	   //TVAR_ash_pGAS[234] //
	{0x0F12, 0x00DA, WORD_LEN},	   //TVAR_ash_pGAS[235] //
	{0x0F12, 0x009A, WORD_LEN},	   //TVAR_ash_pGAS[236] //
	{0x0F12, 0x0065, WORD_LEN},	   //TVAR_ash_pGAS[237] //
	{0x0F12, 0x0040, WORD_LEN},	   //TVAR_ash_pGAS[238] //
	{0x0F12, 0x002B, WORD_LEN},	   //TVAR_ash_pGAS[239] //
	{0x0F12, 0x0026, WORD_LEN},	   //TVAR_ash_pGAS[240] //
	{0x0F12, 0x0031, WORD_LEN},	   //TVAR_ash_pGAS[241] //
	{0x0F12, 0x004E, WORD_LEN},	   //TVAR_ash_pGAS[242] //
	{0x0F12, 0x007C, WORD_LEN},	   //TVAR_ash_pGAS[243] //
	{0x0F12, 0x00B6, WORD_LEN},	   //TVAR_ash_pGAS[244] //
	{0x0F12, 0x00FB, WORD_LEN},	   //TVAR_ash_pGAS[245] //
	{0x0F12, 0x0143, WORD_LEN},	   //TVAR_ash_pGAS[246] //
	{0x0F12, 0x0140, WORD_LEN},	   //TVAR_ash_pGAS[247] //
	{0x0F12, 0x00FB, WORD_LEN},	   //TVAR_ash_pGAS[248] //
	{0x0F12, 0x00BD, WORD_LEN},	   //TVAR_ash_pGAS[249] //
	{0x0F12, 0x008C, WORD_LEN},	   //TVAR_ash_pGAS[250] //
	{0x0F12, 0x0068, WORD_LEN},	   //TVAR_ash_pGAS[251] //
	{0x0F12, 0x0054, WORD_LEN},	   //TVAR_ash_pGAS[252] //
	{0x0F12, 0x004F, WORD_LEN},	   //TVAR_ash_pGAS[253] //
	{0x0F12, 0x005B, WORD_LEN},	   //TVAR_ash_pGAS[254] //
	{0x0F12, 0x0077, WORD_LEN},	   //TVAR_ash_pGAS[255] //
	{0x0F12, 0x00A2, WORD_LEN},	   //TVAR_ash_pGAS[256] //
	{0x0F12, 0x00DA, WORD_LEN},	   //TVAR_ash_pGAS[257] //
	{0x0F12, 0x011C, WORD_LEN},	   //TVAR_ash_pGAS[258] //
	{0x0F12, 0x0163, WORD_LEN},	   //TVAR_ash_pGAS[259] //
	{0x0F12, 0x0173, WORD_LEN},	   //TVAR_ash_pGAS[260] //
	{0x0F12, 0x012D, WORD_LEN},	   //TVAR_ash_pGAS[261] //
	{0x0F12, 0x00F5, WORD_LEN},	   //TVAR_ash_pGAS[262] //
	{0x0F12, 0x00C6, WORD_LEN},	   //TVAR_ash_pGAS[263] //
	{0x0F12, 0x00A5, WORD_LEN},	   //TVAR_ash_pGAS[264] //
	{0x0F12, 0x0092, WORD_LEN},	   //TVAR_ash_pGAS[265] //
	{0x0F12, 0x008F, WORD_LEN},	   //TVAR_ash_pGAS[266] //
	{0x0F12, 0x009B, WORD_LEN},	   //TVAR_ash_pGAS[267] //
	{0x0F12, 0x00B5, WORD_LEN},	   //TVAR_ash_pGAS[268] //
	{0x0F12, 0x00DC, WORD_LEN},	   //TVAR_ash_pGAS[269] //
	{0x0F12, 0x010C, WORD_LEN},	   //TVAR_ash_pGAS[270] //
	{0x0F12, 0x014C, WORD_LEN},	   //TVAR_ash_pGAS[271] //
	{0x0F12, 0x0197, WORD_LEN},	   //TVAR_ash_pGAS[272] //
	{0x0F12, 0x019F, WORD_LEN},	   //TVAR_ash_pGAS[273] //
	{0x0F12, 0x015F, WORD_LEN},	   //TVAR_ash_pGAS[274] //
	{0x0F12, 0x0128, WORD_LEN},	   //TVAR_ash_pGAS[275] //
	{0x0F12, 0x00FF, WORD_LEN},	   //TVAR_ash_pGAS[276] //
	{0x0F12, 0x00E1, WORD_LEN},	   //TVAR_ash_pGAS[277] //
	{0x0F12, 0x00D0, WORD_LEN},	   //TVAR_ash_pGAS[278] //
	{0x0F12, 0x00CF, WORD_LEN},	   //TVAR_ash_pGAS[279] //
	{0x0F12, 0x00DA, WORD_LEN},	   //TVAR_ash_pGAS[280] //
	{0x0F12, 0x00F4, WORD_LEN},	   //TVAR_ash_pGAS[281] //
	{0x0F12, 0x0116, WORD_LEN},	   //TVAR_ash_pGAS[282] //
	{0x0F12, 0x0142, WORD_LEN},	   //TVAR_ash_pGAS[283] //
	{0x0F12, 0x017A, WORD_LEN},	   //TVAR_ash_pGAS[284] //
	{0x0F12, 0x01A8, WORD_LEN},	   //TVAR_ash_pGAS[285] //
	{0x0F12, 0x0191, WORD_LEN},	   //TVAR_ash_pGAS[286] //
	{0x0F12, 0x0152, WORD_LEN},	   //TVAR_ash_pGAS[287] //
	{0x0F12, 0x0118, WORD_LEN},	   //TVAR_ash_pGAS[288] //
	{0x0F12, 0x00EB, WORD_LEN},	   //TVAR_ash_pGAS[289] //
	{0x0F12, 0x00C8, WORD_LEN},	   //TVAR_ash_pGAS[290] //
	{0x0F12, 0x00B4, WORD_LEN},	   //TVAR_ash_pGAS[291] //
	{0x0F12, 0x00AF, WORD_LEN},	   //TVAR_ash_pGAS[292] //
	{0x0F12, 0x00BB, WORD_LEN},	   //TVAR_ash_pGAS[293] //
	{0x0F12, 0x00D5, WORD_LEN},	   //TVAR_ash_pGAS[294] //
	{0x0F12, 0x00FE, WORD_LEN},	   //TVAR_ash_pGAS[295] //
	{0x0F12, 0x012E, WORD_LEN},	   //TVAR_ash_pGAS[296] //
	{0x0F12, 0x016E, WORD_LEN},	   //TVAR_ash_pGAS[297] //
	{0x0F12, 0x01AC, WORD_LEN},	   //TVAR_ash_pGAS[298] //
	{0x0F12, 0x0166, WORD_LEN},	   //TVAR_ash_pGAS[299] //
	{0x0F12, 0x0121, WORD_LEN},	   //TVAR_ash_pGAS[300] //
	{0x0F12, 0x00E3, WORD_LEN},	   //TVAR_ash_pGAS[301] //
	{0x0F12, 0x00B3, WORD_LEN},	   //TVAR_ash_pGAS[302] //
	{0x0F12, 0x008E, WORD_LEN},	   //TVAR_ash_pGAS[303] //
	{0x0F12, 0x0079, WORD_LEN},	   //TVAR_ash_pGAS[304] //
	{0x0F12, 0x0074, WORD_LEN},	   //TVAR_ash_pGAS[305] //
	{0x0F12, 0x0081, WORD_LEN},	   //TVAR_ash_pGAS[306] //
	{0x0F12, 0x009D, WORD_LEN},	   //TVAR_ash_pGAS[307] //
	{0x0F12, 0x00C7, WORD_LEN},	   //TVAR_ash_pGAS[308] //
	{0x0F12, 0x00FF, WORD_LEN},	   //TVAR_ash_pGAS[309] //
	{0x0F12, 0x013F, WORD_LEN},	   //TVAR_ash_pGAS[310] //
	{0x0F12, 0x0188, WORD_LEN},	   //TVAR_ash_pGAS[311] //
	{0x0F12, 0x0134, WORD_LEN},	   //TVAR_ash_pGAS[312] //
	{0x0F12, 0x00F1, WORD_LEN},	   //TVAR_ash_pGAS[313] //
	{0x0F12, 0x00B1, WORD_LEN},	   //TVAR_ash_pGAS[314] //
	{0x0F12, 0x007C, WORD_LEN},	   //TVAR_ash_pGAS[315] //
	{0x0F12, 0x0057, WORD_LEN},	   //TVAR_ash_pGAS[316] //
	{0x0F12, 0x0041, WORD_LEN},	   //TVAR_ash_pGAS[317] //
	{0x0F12, 0x003C, WORD_LEN},	   //TVAR_ash_pGAS[318] //
	{0x0F12, 0x0048, WORD_LEN},	   //TVAR_ash_pGAS[319] //
	{0x0F12, 0x0065, WORD_LEN},	   //TVAR_ash_pGAS[320] //
	{0x0F12, 0x0091, WORD_LEN},	   //TVAR_ash_pGAS[321] //
	{0x0F12, 0x00CA, WORD_LEN},	   //TVAR_ash_pGAS[322] //
	{0x0F12, 0x010C, WORD_LEN},	   //TVAR_ash_pGAS[323] //
	{0x0F12, 0x0157, WORD_LEN},	   //TVAR_ash_pGAS[324] //
	{0x0F12, 0x0118, WORD_LEN},	   //TVAR_ash_pGAS[325] //
	{0x0F12, 0x00D2, WORD_LEN},	   //TVAR_ash_pGAS[326] //
	{0x0F12, 0x0091, WORD_LEN},	   //TVAR_ash_pGAS[327] //
	{0x0F12, 0x005C, WORD_LEN},	   //TVAR_ash_pGAS[328] //
	{0x0F12, 0x0035, WORD_LEN},	   //TVAR_ash_pGAS[329] //
	{0x0F12, 0x001E, WORD_LEN},	   //TVAR_ash_pGAS[330] //
	{0x0F12, 0x0019, WORD_LEN},	   //TVAR_ash_pGAS[331] //
	{0x0F12, 0x0025, WORD_LEN},	   //TVAR_ash_pGAS[332] //
	{0x0F12, 0x0043, WORD_LEN},	   //TVAR_ash_pGAS[333] //
	{0x0F12, 0x0070, WORD_LEN},	   //TVAR_ash_pGAS[334] //
	{0x0F12, 0x00A9, WORD_LEN},	   //TVAR_ash_pGAS[335] //
	{0x0F12, 0x00EE, WORD_LEN},	   //TVAR_ash_pGAS[336] //
	{0x0F12, 0x0136, WORD_LEN},	   //TVAR_ash_pGAS[337] //
	{0x0F12, 0x0105, WORD_LEN},	   //TVAR_ash_pGAS[338] //
	{0x0F12, 0x00BD, WORD_LEN},	   //TVAR_ash_pGAS[339] //
	{0x0F12, 0x007B, WORD_LEN},	   //TVAR_ash_pGAS[340] //
	{0x0F12, 0x0046, WORD_LEN},	   //TVAR_ash_pGAS[341] //
	{0x0F12, 0x001F, WORD_LEN},	   //TVAR_ash_pGAS[342] //
	{0x0F12, 0x0009, WORD_LEN},	   //TVAR_ash_pGAS[343] //
	{0x0F12, 0x0003, WORD_LEN},	   //TVAR_ash_pGAS[344] //
	{0x0F12, 0x000E, WORD_LEN},	   //TVAR_ash_pGAS[345] //
	{0x0F12, 0x002B, WORD_LEN},	   //TVAR_ash_pGAS[346] //
	{0x0F12, 0x0057, WORD_LEN},	   //TVAR_ash_pGAS[347] //
	{0x0F12, 0x0091, WORD_LEN},	   //TVAR_ash_pGAS[348] //
	{0x0F12, 0x00D7, WORD_LEN},	   //TVAR_ash_pGAS[349] //
	{0x0F12, 0x0121, WORD_LEN},	   //TVAR_ash_pGAS[350] //
	{0x0F12, 0x0105, WORD_LEN},	   //TVAR_ash_pGAS[351] //
	{0x0F12, 0x00BC, WORD_LEN},	   //TVAR_ash_pGAS[352] //
	{0x0F12, 0x0078, WORD_LEN},	   //TVAR_ash_pGAS[353] //
	{0x0F12, 0x0043, WORD_LEN},	   //TVAR_ash_pGAS[354] //
	{0x0F12, 0x001D, WORD_LEN},	   //TVAR_ash_pGAS[355] //
	{0x0F12, 0x0006, WORD_LEN},	   //TVAR_ash_pGAS[356] //
	{0x0F12, 0x0000, WORD_LEN},	   //TVAR_ash_pGAS[357] //
	{0x0F12, 0x000B, WORD_LEN},	   //TVAR_ash_pGAS[358] //
	{0x0F12, 0x0026, WORD_LEN},	   //TVAR_ash_pGAS[359] //
	{0x0F12, 0x0052, WORD_LEN},	   //TVAR_ash_pGAS[360] //
	{0x0F12, 0x008C, WORD_LEN},	   //TVAR_ash_pGAS[361] //
	{0x0F12, 0x00D3, WORD_LEN},	   //TVAR_ash_pGAS[362] //
	{0x0F12, 0x011E, WORD_LEN},	   //TVAR_ash_pGAS[363] //
	{0x0F12, 0x010D, WORD_LEN},	   //TVAR_ash_pGAS[364] //
	{0x0F12, 0x00C5, WORD_LEN},	   //TVAR_ash_pGAS[365] //
	{0x0F12, 0x0083, WORD_LEN},	   //TVAR_ash_pGAS[366] //
	{0x0F12, 0x004E, WORD_LEN},	   //TVAR_ash_pGAS[367] //
	{0x0F12, 0x0027, WORD_LEN},	   //TVAR_ash_pGAS[368] //
	{0x0F12, 0x000F, WORD_LEN},	   //TVAR_ash_pGAS[369] //
	{0x0F12, 0x0008, WORD_LEN},	   //TVAR_ash_pGAS[370] //
	{0x0F12, 0x0012, WORD_LEN},	   //TVAR_ash_pGAS[371] //
	{0x0F12, 0x002E, WORD_LEN},	   //TVAR_ash_pGAS[372] //
	{0x0F12, 0x0059, WORD_LEN},	   //TVAR_ash_pGAS[373] //
	{0x0F12, 0x0091, WORD_LEN},	   //TVAR_ash_pGAS[374] //
	{0x0F12, 0x00D6, WORD_LEN},	   //TVAR_ash_pGAS[375] //
	{0x0F12, 0x0120, WORD_LEN},	   //TVAR_ash_pGAS[376] //
	{0x0F12, 0x012A, WORD_LEN},	   //TVAR_ash_pGAS[377] //
	{0x0F12, 0x00E2, WORD_LEN},	   //TVAR_ash_pGAS[378] //
	{0x0F12, 0x00A2, WORD_LEN},	   //TVAR_ash_pGAS[379] //
	{0x0F12, 0x006D, WORD_LEN},	   //TVAR_ash_pGAS[380] //
	{0x0F12, 0x0045, WORD_LEN},	   //TVAR_ash_pGAS[381] //
	{0x0F12, 0x002D, WORD_LEN},	   //TVAR_ash_pGAS[382] //
	{0x0F12, 0x0025, WORD_LEN},	   //TVAR_ash_pGAS[383] //
	{0x0F12, 0x002E, WORD_LEN},	   //TVAR_ash_pGAS[384] //
	{0x0F12, 0x0049, WORD_LEN},	   //TVAR_ash_pGAS[385] //
	{0x0F12, 0x0073, WORD_LEN},	   //TVAR_ash_pGAS[386] //
	{0x0F12, 0x00AA, WORD_LEN},	   //TVAR_ash_pGAS[387] //
	{0x0F12, 0x00EC, WORD_LEN},	   //TVAR_ash_pGAS[388] //
	{0x0F12, 0x0135, WORD_LEN},	   //TVAR_ash_pGAS[389] //
	{0x0F12, 0x0149, WORD_LEN},	   //TVAR_ash_pGAS[390] //
	{0x0F12, 0x0105, WORD_LEN},	   //TVAR_ash_pGAS[391] //
	{0x0F12, 0x00C9, WORD_LEN},	   //TVAR_ash_pGAS[392] //
	{0x0F12, 0x0096, WORD_LEN},	   //TVAR_ash_pGAS[393] //
	{0x0F12, 0x006F, WORD_LEN},	   //TVAR_ash_pGAS[394] //
	{0x0F12, 0x0056, WORD_LEN},	   //TVAR_ash_pGAS[395] //
	{0x0F12, 0x004E, WORD_LEN},	   //TVAR_ash_pGAS[396] //
	{0x0F12, 0x0056, WORD_LEN},	   //TVAR_ash_pGAS[397] //
	{0x0F12, 0x006F, WORD_LEN},	   //TVAR_ash_pGAS[398] //
	{0x0F12, 0x0096, WORD_LEN},	   //TVAR_ash_pGAS[399] //
	{0x0F12, 0x00CA, WORD_LEN},	   //TVAR_ash_pGAS[400] //
	{0x0F12, 0x0109, WORD_LEN},	   //TVAR_ash_pGAS[401] //
	{0x0F12, 0x0153, WORD_LEN},	   //TVAR_ash_pGAS[402] //
	{0x0F12, 0x0181, WORD_LEN},	   //TVAR_ash_pGAS[403] //
	{0x0F12, 0x013B, WORD_LEN},	   //TVAR_ash_pGAS[404] //
	{0x0F12, 0x0102, WORD_LEN},	   //TVAR_ash_pGAS[405] //
	{0x0F12, 0x00D2, WORD_LEN},	   //TVAR_ash_pGAS[406] //
	{0x0F12, 0x00AE, WORD_LEN},	   //TVAR_ash_pGAS[407] //
	{0x0F12, 0x0096, WORD_LEN},	   //TVAR_ash_pGAS[408] //
	{0x0F12, 0x008D, WORD_LEN},	   //TVAR_ash_pGAS[409] //
	{0x0F12, 0x0094, WORD_LEN},	   //TVAR_ash_pGAS[410] //
	{0x0F12, 0x00AA, WORD_LEN},	   //TVAR_ash_pGAS[411] //
	{0x0F12, 0x00CC, WORD_LEN},	   //TVAR_ash_pGAS[412] //
	{0x0F12, 0x00F8, WORD_LEN},	   //TVAR_ash_pGAS[413] //
	{0x0F12, 0x0135, WORD_LEN},	   //TVAR_ash_pGAS[414] //
	{0x0F12, 0x0183, WORD_LEN},	   //TVAR_ash_pGAS[415] //
	{0x0F12, 0x01B0, WORD_LEN},	   //TVAR_ash_pGAS[416] //
	{0x0F12, 0x016F, WORD_LEN},	   //TVAR_ash_pGAS[417] //
	{0x0F12, 0x0139, WORD_LEN},	   //TVAR_ash_pGAS[418] //
	{0x0F12, 0x010E, WORD_LEN},	   //TVAR_ash_pGAS[419] //
	{0x0F12, 0x00ED, WORD_LEN},	   //TVAR_ash_pGAS[420] //
	{0x0F12, 0x00D6, WORD_LEN},	   //TVAR_ash_pGAS[421] //
	{0x0F12, 0x00CD, WORD_LEN},	   //TVAR_ash_pGAS[422] //
	{0x0F12, 0x00D3, WORD_LEN},	   //TVAR_ash_pGAS[423] //
	{0x0F12, 0x00E8, WORD_LEN},	   //TVAR_ash_pGAS[424] //
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_ash_pGAS[425] //
	{0x0F12, 0x012D, WORD_LEN},	   //TVAR_ash_pGAS[426] //
	{0x0F12, 0x0163, WORD_LEN},	   //TVAR_ash_pGAS[427] //
	{0x0F12, 0x0195, WORD_LEN},	   //TVAR_ash_pGAS[428] //
	{0x0F12, 0x0146, WORD_LEN},	   //TVAR_ash_pGAS[429] //
	{0x0F12, 0x011B, WORD_LEN},	   //TVAR_ash_pGAS[430] //
	{0x0F12, 0x00EC, WORD_LEN},	   //TVAR_ash_pGAS[431] //
	{0x0F12, 0x00C6, WORD_LEN},	   //TVAR_ash_pGAS[432] //
	{0x0F12, 0x00AD, WORD_LEN},	   //TVAR_ash_pGAS[433] //
	{0x0F12, 0x009F, WORD_LEN},	   //TVAR_ash_pGAS[434] //
	{0x0F12, 0x009C, WORD_LEN},	   //TVAR_ash_pGAS[435] //
	{0x0F12, 0x00A6, WORD_LEN},	   //TVAR_ash_pGAS[436] //
	{0x0F12, 0x00BC, WORD_LEN},	   //TVAR_ash_pGAS[437] //
	{0x0F12, 0x00DF, WORD_LEN},	   //TVAR_ash_pGAS[438] //
	{0x0F12, 0x010B, WORD_LEN},	   //TVAR_ash_pGAS[439] //
	{0x0F12, 0x0146, WORD_LEN},	   //TVAR_ash_pGAS[440] //
	{0x0F12, 0x0176, WORD_LEN},	   //TVAR_ash_pGAS[441] //
	{0x0F12, 0x0120, WORD_LEN},	   //TVAR_ash_pGAS[442] //
	{0x0F12, 0x00EC, WORD_LEN},	   //TVAR_ash_pGAS[443] //
	{0x0F12, 0x00BA, WORD_LEN},	   //TVAR_ash_pGAS[444] //
	{0x0F12, 0x0093, WORD_LEN},	   //TVAR_ash_pGAS[445] //
	{0x0F12, 0x007A, WORD_LEN},	   //TVAR_ash_pGAS[446] //
	{0x0F12, 0x006C, WORD_LEN},	   //TVAR_ash_pGAS[447] //
	{0x0F12, 0x0069, WORD_LEN},	   //TVAR_ash_pGAS[448] //
	{0x0F12, 0x0073, WORD_LEN},	   //TVAR_ash_pGAS[449] //
	{0x0F12, 0x008B, WORD_LEN},	   //TVAR_ash_pGAS[450] //
	{0x0F12, 0x00AD, WORD_LEN},	   //TVAR_ash_pGAS[451] //
	{0x0F12, 0x00DD, WORD_LEN},	   //TVAR_ash_pGAS[452] //
	{0x0F12, 0x0118, WORD_LEN},	   //TVAR_ash_pGAS[453] //
	{0x0F12, 0x0156, WORD_LEN},	   //TVAR_ash_pGAS[454] //
	{0x0F12, 0x00F0, WORD_LEN},	   //TVAR_ash_pGAS[455] //
	{0x0F12, 0x00BF, WORD_LEN},	   //TVAR_ash_pGAS[456] //
	{0x0F12, 0x0089, WORD_LEN},	   //TVAR_ash_pGAS[457] //
	{0x0F12, 0x0062, WORD_LEN},	   //TVAR_ash_pGAS[458] //
	{0x0F12, 0x0047, WORD_LEN},	   //TVAR_ash_pGAS[459] //
	{0x0F12, 0x003A, WORD_LEN},	   //TVAR_ash_pGAS[460] //
	{0x0F12, 0x0038, WORD_LEN},	   //TVAR_ash_pGAS[461] //
	{0x0F12, 0x0041, WORD_LEN},	   //TVAR_ash_pGAS[462] //
	{0x0F12, 0x0058, WORD_LEN},	   //TVAR_ash_pGAS[463] //
	{0x0F12, 0x007C, WORD_LEN},	   //TVAR_ash_pGAS[464] //
	{0x0F12, 0x00AC, WORD_LEN},	   //TVAR_ash_pGAS[465] //
	{0x0F12, 0x00E7, WORD_LEN},	   //TVAR_ash_pGAS[466] //
	{0x0F12, 0x0123, WORD_LEN},	   //TVAR_ash_pGAS[467] //
	{0x0F12, 0x00D5, WORD_LEN},	   //TVAR_ash_pGAS[468] //
	{0x0F12, 0x00A1, WORD_LEN},	   //TVAR_ash_pGAS[469] //
	{0x0F12, 0x006C, WORD_LEN},	   //TVAR_ash_pGAS[470] //
	{0x0F12, 0x0044, WORD_LEN},	   //TVAR_ash_pGAS[471] //
	{0x0F12, 0x0029, WORD_LEN},	   //TVAR_ash_pGAS[472] //
	{0x0F12, 0x001B, WORD_LEN},	   //TVAR_ash_pGAS[473] //
	{0x0F12, 0x0018, WORD_LEN},	   //TVAR_ash_pGAS[474] //
	{0x0F12, 0x0022, WORD_LEN},	   //TVAR_ash_pGAS[475] //
	{0x0F12, 0x0039, WORD_LEN},	   //TVAR_ash_pGAS[476] //
	{0x0F12, 0x005D, WORD_LEN},	   //TVAR_ash_pGAS[477] //
	{0x0F12, 0x008E, WORD_LEN},	   //TVAR_ash_pGAS[478] //
	{0x0F12, 0x00CA, WORD_LEN},	   //TVAR_ash_pGAS[479] //
	{0x0F12, 0x0105, WORD_LEN},	   //TVAR_ash_pGAS[480] //
	{0x0F12, 0x00C1, WORD_LEN},	   //TVAR_ash_pGAS[481] //
	{0x0F12, 0x008C, WORD_LEN},	   //TVAR_ash_pGAS[482] //
	{0x0F12, 0x0057, WORD_LEN},		//TVAR_ash_pGAS[483] //                                                                        
	{0x0F12, 0x002F, WORD_LEN},		//TVAR_ash_pGAS[484] //                                                                        
	{0x0F12, 0x0014, WORD_LEN},		//TVAR_ash_pGAS[485] //                                                                        
	{0x0F12, 0x0006, WORD_LEN},		//TVAR_ash_pGAS[486] //                                                                        
	{0x0F12, 0x0003, WORD_LEN},		//TVAR_ash_pGAS[487] //                                                                        
	{0x0F12, 0x000B, WORD_LEN},		//TVAR_ash_pGAS[488] //                                                                        
	{0x0F12, 0x0022, WORD_LEN},		//TVAR_ash_pGAS[489] //                                                                        
	{0x0F12, 0x0046, WORD_LEN},		//TVAR_ash_pGAS[490] //                                                                        
	{0x0F12, 0x0076, WORD_LEN},		//TVAR_ash_pGAS[491] //                                                                        
	{0x0F12, 0x00B4, WORD_LEN},		//TVAR_ash_pGAS[492] //                                                                        
	{0x0F12, 0x00F0, WORD_LEN},		//TVAR_ash_pGAS[493] //                                                                        
	{0x0F12, 0x00C1, WORD_LEN},		//TVAR_ash_pGAS[494] //                                                                        
	{0x0F12, 0x008B, WORD_LEN},		//TVAR_ash_pGAS[495] //                                                                        
	{0x0F12, 0x0055, WORD_LEN},		//TVAR_ash_pGAS[496] //                                                                        
	{0x0F12, 0x002C, WORD_LEN},		//TVAR_ash_pGAS[497] //                                                                        
	{0x0F12, 0x0011, WORD_LEN},		//TVAR_ash_pGAS[498] //                                                                        
	{0x0F12, 0x0003, WORD_LEN},		//TVAR_ash_pGAS[499] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//TVAR_ash_pGAS[500] //                                                                        
	{0x0F12, 0x0007, WORD_LEN},		//TVAR_ash_pGAS[501] //                                                                        
	{0x0F12, 0x001D, WORD_LEN},		//TVAR_ash_pGAS[502] //                                                                        
	{0x0F12, 0x0040, WORD_LEN},		//TVAR_ash_pGAS[503] //                                                                        
	{0x0F12, 0x0070, WORD_LEN},		//TVAR_ash_pGAS[504] //                                                                        
	{0x0F12, 0x00AF, WORD_LEN},		//TVAR_ash_pGAS[505] //                                                                        
	{0x0F12, 0x00EC, WORD_LEN},		//TVAR_ash_pGAS[506] //                                                                        
	{0x0F12, 0x00C6, WORD_LEN},		//TVAR_ash_pGAS[507] //                                                                        
	{0x0F12, 0x0092, WORD_LEN},		//TVAR_ash_pGAS[508] //                                                                        
	{0x0F12, 0x005D, WORD_LEN},		//TVAR_ash_pGAS[509] //                                                                        
	{0x0F12, 0x0035, WORD_LEN},		//TVAR_ash_pGAS[510] //                                                                        
	{0x0F12, 0x0019, WORD_LEN},		//TVAR_ash_pGAS[511] //                                                                        
	{0x0F12, 0x000A, WORD_LEN},		//TVAR_ash_pGAS[512] //                                                                        
	{0x0F12, 0x0006, WORD_LEN},		//TVAR_ash_pGAS[513] //                                                                        
	{0x0F12, 0x000E, WORD_LEN},		//TVAR_ash_pGAS[514] //                                                                        
	{0x0F12, 0x0024, WORD_LEN},		//TVAR_ash_pGAS[515] //                                                                        
	{0x0F12, 0x0046, WORD_LEN},		//TVAR_ash_pGAS[516] //                                                                        
	{0x0F12, 0x0074, WORD_LEN},		//TVAR_ash_pGAS[517] //                                                                        
	{0x0F12, 0x00B0, WORD_LEN},		//TVAR_ash_pGAS[518] //                                                                        
	{0x0F12, 0x00ED, WORD_LEN},		//TVAR_ash_pGAS[519] //                                                                        
	{0x0F12, 0x00E0, WORD_LEN},		//TVAR_ash_pGAS[520] //                                                                        
	{0x0F12, 0x00AA, WORD_LEN},		//TVAR_ash_pGAS[521] //                                                                        
	{0x0F12, 0x0078, WORD_LEN},		//TVAR_ash_pGAS[522] //                                                                        
	{0x0F12, 0x0050, WORD_LEN},		//TVAR_ash_pGAS[523] //                                                                        
	{0x0F12, 0x0034, WORD_LEN},		//TVAR_ash_pGAS[524] //                                                                        
	{0x0F12, 0x0024, WORD_LEN},		//TVAR_ash_pGAS[525] //                                                                        
	{0x0F12, 0x001F, WORD_LEN},		//TVAR_ash_pGAS[526] //                                                                        
	{0x0F12, 0x0026, WORD_LEN},		//TVAR_ash_pGAS[527] //                                                                        
	{0x0F12, 0x003A, WORD_LEN},		//TVAR_ash_pGAS[528] //                                                                        
	{0x0F12, 0x005D, WORD_LEN},		//TVAR_ash_pGAS[529] //                                                                        
	{0x0F12, 0x008B, WORD_LEN},		//TVAR_ash_pGAS[530] //                                                                        
	{0x0F12, 0x00C4, WORD_LEN},		//TVAR_ash_pGAS[531] //                                                                        
	{0x0F12, 0x00FE, WORD_LEN},		//TVAR_ash_pGAS[532] //                                                                        
	{0x0F12, 0x00FE, WORD_LEN},		//TVAR_ash_pGAS[533] //                                                                        
	{0x0F12, 0x00CB, WORD_LEN},		//TVAR_ash_pGAS[534] //                                                                        
	{0x0F12, 0x0099, WORD_LEN},		//TVAR_ash_pGAS[535] //                                                                        
	{0x0F12, 0x0074, WORD_LEN},		//TVAR_ash_pGAS[536] //                                                                        
	{0x0F12, 0x005A, WORD_LEN},		//TVAR_ash_pGAS[537] //                                                                        
	{0x0F12, 0x004B, WORD_LEN},		//TVAR_ash_pGAS[538] //                                                                        
	{0x0F12, 0x0046, WORD_LEN},		//TVAR_ash_pGAS[539] //                                                                        
	{0x0F12, 0x004A, WORD_LEN},		//TVAR_ash_pGAS[540] //                                                                        
	{0x0F12, 0x005C, WORD_LEN},		//TVAR_ash_pGAS[541] //                                                                        
	{0x0F12, 0x007A, WORD_LEN},		//TVAR_ash_pGAS[542] //                                                                        
	{0x0F12, 0x00A7, WORD_LEN},		//TVAR_ash_pGAS[543] //                                                                        
	{0x0F12, 0x00E0, WORD_LEN},		//TVAR_ash_pGAS[544] //                                                                        
	{0x0F12, 0x0119, WORD_LEN},		//TVAR_ash_pGAS[545] //                                                                        
	{0x0F12, 0x0131, WORD_LEN},		//TVAR_ash_pGAS[546] //                                                                        
	{0x0F12, 0x00FD, WORD_LEN},		//TVAR_ash_pGAS[547] //                                                                        
	{0x0F12, 0x00CF, WORD_LEN},		//TVAR_ash_pGAS[548] //                                                                        
	{0x0F12, 0x00AC, WORD_LEN},		//TVAR_ash_pGAS[549] //                                                                        
	{0x0F12, 0x0092, WORD_LEN},		//TVAR_ash_pGAS[550] //                                                                        
	{0x0F12, 0x0084, WORD_LEN},		//TVAR_ash_pGAS[551] //                                                                        
	{0x0F12, 0x007F, WORD_LEN},		//TVAR_ash_pGAS[552] //                                                                        
	{0x0F12, 0x0085, WORD_LEN},		//TVAR_ash_pGAS[553] //                                                                        
	{0x0F12, 0x0094, WORD_LEN},		//TVAR_ash_pGAS[554] //                                                                        
	{0x0F12, 0x00AE, WORD_LEN},		//TVAR_ash_pGAS[555] //                                                                        
	{0x0F12, 0x00CF, WORD_LEN},		//TVAR_ash_pGAS[556] //                                                                        
	{0x0F12, 0x0107, WORD_LEN},		//TVAR_ash_pGAS[557] //                                                                        
	{0x0F12, 0x0148, WORD_LEN},		//TVAR_ash_pGAS[558] //                                                                        
	{0x0F12, 0x015F, WORD_LEN},		//TVAR_ash_pGAS[559] //                                                                        
	{0x0F12, 0x012F, WORD_LEN},		//TVAR_ash_pGAS[560] //                                                                        
	{0x0F12, 0x0102, WORD_LEN},		//TVAR_ash_pGAS[561] //                                                                        
	{0x0F12, 0x00E3, WORD_LEN},		//TVAR_ash_pGAS[562] //                                                                        
	{0x0F12, 0x00CA, WORD_LEN},		//TVAR_ash_pGAS[563] //                                                                        
	{0x0F12, 0x00BD, WORD_LEN},		//TVAR_ash_pGAS[564] //                                                                        
	{0x0F12, 0x00B8, WORD_LEN},		//TVAR_ash_pGAS[565] //                                                                        
	{0x0F12, 0x00BC, WORD_LEN},		//TVAR_ash_pGAS[566] //                                                                        
	{0x0F12, 0x00CB, WORD_LEN},		//TVAR_ash_pGAS[567] //                                                                        
	{0x0F12, 0x00E4, WORD_LEN},		//TVAR_ash_pGAS[568] //                                                                        
	{0x0F12, 0x0104, WORD_LEN},		//TVAR_ash_pGAS[569] //                                                                        
	{0x0F12, 0x012E, WORD_LEN},		//TVAR_ash_pGAS[570] //                                                                        
	{0x0F12, 0x0148, WORD_LEN},		//TVAR_ash_pGAS[571] //                                                                        
	                     
	{0x002A, 0x074E, WORD_LEN},		    
	{0x0F12, 0x0001, WORD_LEN},		//ash_bLumaMode //         	                                                                        
	{0x002A, 0x0D30, WORD_LEN},	                               
	{0x0F12, 0x02A8, WORD_LEN},		//awbb_GLocu //        
	{0x0F12, 0x0347, WORD_LEN},		//awbb_GLocuSB //      
	                                                
	{0x002A, 0x06B8, WORD_LEN},	                               
	{0x0F12, 0x00C0, WORD_LEN},		//TVAR_ash_AwbashCord[0] //                                                                        
	{0x0F12, 0x00E0, WORD_LEN},		//TVAR_ash_AwbashCord[1] //                                                                        
	{0x0F12, 0x0120, WORD_LEN},		//TVAR_ash_AwbashCord[2] //                                                                        
	{0x0F12, 0x0124, WORD_LEN},		//TVAR_ash_AwbashCord[3] //                                                                        
	{0x0F12, 0x0156, WORD_LEN},		//TVAR_ash_AwbashCord[4] //                                                                        
	{0x0F12, 0x017F, WORD_LEN},		//TVAR_ash_AwbashCord[5] //                                                                        
	{0x0F12, 0x018F, WORD_LEN},		//TVAR_ash_AwbashCord[6] //                                                                        
	                                                
	                                                
	{0x002A, 0x0664, WORD_LEN},	                               
	{0x0F12, 0x013E, WORD_LEN},		//seti_uContrastCenter //                                                                          
	                                                
	                                                
	{0x002A, 0x06C6, WORD_LEN},	                               
	{0x0F12, 0x010B, WORD_LEN},		//ash_CGrasalphaS[0] //
	{0x0F12, 0x0103, WORD_LEN},		//ash_CGrasalphaS[1] //
	{0x0F12, 0x00FC, WORD_LEN},		//ash_CGrasalphaS[2] //
	{0x0F12, 0x010C, WORD_LEN},		//ash_CGrasalphaS[3] //
	                     
	{0x002A, 0x0C48, WORD_LEN},	    
	{0x0F12, 0x03C8, WORD_LEN},	 //03C9	//awbb_IndoorGrZones_m_BGrid[0] // 		                                                                        
	{0x0F12, 0x03DE, WORD_LEN},	 //03DE	//awbb_IndoorGrZones_m_BGrid[1] //                                                                         
	{0x0F12, 0x0372, WORD_LEN},	 //0372	//awbb_IndoorGrZones_m_BGrid[2] //                                                                         
	{0x0F12, 0x03EA, WORD_LEN},	 //03EA	//awbb_IndoorGrZones_m_BGrid[3] //                                                                         
	{0x0F12, 0x0336, WORD_LEN},	 //0336	//awbb_IndoorGrZones_m_BGrid[4] //                                                                         
	{0x0F12, 0x03DE, WORD_LEN},	 //03DE	//awbb_IndoorGrZones_m_BGrid[5] //                                                                         
	{0x0F12, 0x0302, WORD_LEN},	 //0302	//awbb_IndoorGrZones_m_BGrid[6] //                                                                         
	{0x0F12, 0x03A2, WORD_LEN},	 //03A2	//awbb_IndoorGrZones_m_BGrid[7] //                                                                         
	{0x0F12, 0x02C8, WORD_LEN},	 //02c8	//awbb_IndoorGrZones_m_BGrid[8] //                                                                         
	{0x0F12, 0x0368, WORD_LEN},	 //0368	//awbb_IndoorGrZones_m_BGrid[9] //                                                                         
	{0x0F12, 0x0292, WORD_LEN},	 //0292	//awbb_IndoorGrZones_m_BGrid[10] //                                                                        
	{0x0F12, 0x033A, WORD_LEN},	 //033A	//awbb_IndoorGrZones_m_BGrid[11] //                                                                        
	{0x0F12, 0x0276, WORD_LEN},	 //0262	//awbb_IndoorGrZones_m_BGrid[12] //                                                                        
	{0x0F12, 0x0306, WORD_LEN},	 //0306	//awbb_IndoorGrZones_m_BGrid[13] //                                                                        
	{0x0F12, 0x025A, WORD_LEN},	 //0250	//awbb_IndoorGrZones_m_BGrid[14] //                                                                        
	{0x0F12, 0x02C2, WORD_LEN},	 //02C2	//awbb_IndoorGrZones_m_BGrid[15] //                                                                        
	{0x0F12, 0x0246, WORD_LEN},	 //023A	//awbb_IndoorGrZones_m_BGrid[16] //                                                                        
	{0x0F12, 0x02A2, WORD_LEN},	 //02A2	//awbb_IndoorGrZones_m_BGrid[17] //                                                                        
	{0x0F12, 0x0232, WORD_LEN},	 //0228	//awbb_IndoorGrZones_m_BGrid[18] //                                                                        
	{0x0F12, 0x0298, WORD_LEN},	 //0298	//awbb_IndoorGrZones_m_BGrid[19] //                                                                        
	{0x0F12, 0x021E, WORD_LEN},	 //0210	//awbb_IndoorGrZones_m_BGrid[20] //                                                                        
	{0x0F12, 0x027E, WORD_LEN},	 //029C	//awbb_IndoorGrZones_m_BGrid[21] //                                                                        
	{0x0F12, 0x0208, WORD_LEN},	 //01FE	//awbb_IndoorGrZones_m_BGrid[22] //                                                                        
	{0x0F12, 0x026C, WORD_LEN},	 //0292	//awbb_IndoorGrZones_m_BGrid[23] //                                                                        
	{0x0F12, 0x01EE, WORD_LEN},	 //01EE	//awbb_IndoorGrZones_m_BGrid[24] //                                                                        
	{0x0F12, 0x025C, WORD_LEN},	 //0278	//awbb_IndoorGrZones_m_BGrid[25] //                                                                        
	{0x0F12, 0x01F0, WORD_LEN},	 //01F2	//awbb_IndoorGrZones_m_BGrid[26] //                                                                        
	{0x0F12, 0x0248, WORD_LEN},	 //0268	//awbb_IndoorGrZones_m_BGrid[27] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},	 //0200	//awbb_IndoorGrZones_m_BGrid[28] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},	 //0246	//awbb_IndoorGrZones_m_BGrid[29] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[30] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[31] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[32] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[33] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[34] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[35] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[36] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[37] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[38] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_IndoorGrZones_m_BGrid[39] //                                                                        
	                                                        
	{0x0F12, 0x0005, WORD_LEN},	   //awbb_IndoorGrZones_m_Gridstep //  

	{0x002A, 0x0C9C, WORD_LEN},	
	{0x0F12, 0x000E, WORD_LEN},	
	{0x002A, 0x0CA0, WORD_LEN},	                                       
	{0x0F12, 0x0108, WORD_LEN},		//awbb_IndoorGrZones_m_Boffs //
	                                                        
	{0x002A, 0x0CE0, WORD_LEN},	                                       
	{0x0F12, 0x03D4, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[0] //                                                                          
	{0x0F12, 0x043E, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[1] //                                                                          
	{0x0F12, 0x035C, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[2] //                                                                          
	{0x0F12, 0x0438, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[3] //                                                                          
	{0x0F12, 0x02F0, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[4] //                                                                          
	{0x0F12, 0x042D, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[5] //                                                                          
	{0x0F12, 0x029A, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[6] //                                                                          
	{0x0F12, 0x03EF, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[7] //                                                                          
	{0x0F12, 0x025E, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[8] //                                                                          
	{0x0F12, 0x0395, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[9] //                                                                          
	{0x0F12, 0x022E, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[10] //                                                                         
	{0x0F12, 0x0346, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[11] //                                                                         
	{0x0F12, 0x0200, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[12] //                                                                         
	{0x0F12, 0x02F6, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[13] //                                                                         
	{0x0F12, 0x01CE, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[14] //                                                                         
	{0x0F12, 0x02C8, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[15] //                                                                         
	{0x0F12, 0x01BB, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[16] //                                                                         
	{0x0F12, 0x0287, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[17] //                                                                         
	{0x0F12, 0x01E2, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[18] //                                                                         
	{0x0F12, 0x0239, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[19] //                                                                         
	{0x0F12, 0x0000, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[20] //                                                                         
	{0x0F12, 0x0000, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[21] //                                                                         
	{0x0F12, 0x0000, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[22] //                                                                         
	{0x0F12, 0x0000, WORD_LEN},		//awbb_LowBrGrZones_m_BGrid[23] //                                                                         
	                     
	{0x0F12, 0x0006, WORD_LEN},		//awbb_LowBrGrZones_m_Gridstep //	                                                                        
	{0x002A, 0x0D18, WORD_LEN},	                                     
	{0x0F12, 0x00AE, WORD_LEN},		//awbb_LowBrGrZones_m_Boff //

	{0x002A, 0x0CA4, WORD_LEN},	    
	{0x0F12, 0x026E, WORD_LEN},		//0294//0286//02C2//awbb_OutdoorGrZones_m_BGrid[0] // 		                                                                        
	{0x0F12, 0x02A4, WORD_LEN},		//02CB//02BD//02E0//awbb_OutdoorGrZones_m_BGrid[1] //                                                                         
	{0x0F12, 0x0262, WORD_LEN},		//027A//026C//0278//awbb_OutdoorGrZones_m_BGrid[2] //                                                                         
	{0x0F12, 0x02A8, WORD_LEN},		//02D7//02C9//02BC//awbb_OutdoorGrZones_m_BGrid[3] //                                                                         
	{0x0F12, 0x0256, WORD_LEN},		//0266//0258//025A//awbb_OutdoorGrZones_m_BGrid[4] //                                                                         
	{0x0F12, 0x02AE, WORD_LEN},		//02BF//02B1//02A2//awbb_OutdoorGrZones_m_BGrid[5] //                                                                         
	{0x0F12, 0x0248, WORD_LEN},		//0252//0244//024A//awbb_OutdoorGrZones_m_BGrid[6] //                                                                         
	{0x0F12, 0x02A4, WORD_LEN},		//02A8//029A//0288//awbb_OutdoorGrZones_m_BGrid[7] //                                                                         
	{0x0F12, 0x023E, WORD_LEN},		//023E//0230//0240//awbb_OutdoorGrZones_m_BGrid[8] //                                                                         
	{0x0F12, 0x029A, WORD_LEN},		//028F//0281//0278//awbb_OutdoorGrZones_m_BGrid[9] //                                                                         
	{0x0F12, 0x023A, WORD_LEN},		//0239//022B//023E//awbb_OutdoorGrZones_m_BGrid[10] //                                                                        
	{0x0F12, 0x0290, WORD_LEN},		//027A//026C//0254//awbb_OutdoorGrZones_m_BGrid[11] //                                                                        
	{0x0F12, 0x023A, WORD_LEN},		//024A//023C//0000//awbb_OutdoorGrZones_m_BGrid[12] //                                                                        
	{0x0F12, 0x027E, WORD_LEN},		//0260//0252//0000//awbb_OutdoorGrZones_m_BGrid[13] //                                                                        
	{0x0F12, 0x0244, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[14] //                                                                        
	{0x0F12, 0x0266, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[15] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[16] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[17] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[18] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[19] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[20] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[21] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[22] //                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//0000//0000//0000//awbb_OutdoorGrZones_m_BGrid[23] //                                                                        

	{0x0F12, 0x0004, WORD_LEN},		//awbb_OutdoorGrZones_m_Gridstep //       
	{0x002A, 0x0CD8, WORD_LEN},	                                                          
	{0x0F12, 0x0008, WORD_LEN},	               
	{0x002A, 0x0CDC, WORD_LEN},	                                                          
	{0x0F12, 0x0204, WORD_LEN},		//awbb_OutdoorGrZones_m_Boff //                   
	{0x002A, 0x0D1C, WORD_LEN},	                                                          
	{0x0F12, 0x037C, WORD_LEN},		//awbb_CrclLowT_R_c //                            
	{0x002A, 0x0D20, WORD_LEN},	                                                          
	{0x0F12, 0x0157, WORD_LEN},		//awbb_CrclLowT_B_c //                            
	{0x002A, 0x0D24, WORD_LEN},	                                                          
	{0x0F12, 0x3EB8, WORD_LEN},		//awbb_CrclLowT_Rad_c // 

	{0x002A, 0x0D2C, WORD_LEN},	    
	{0x0F12, 0x013D, WORD_LEN},		//awbb_IntcR //	                                                                        
	{0x0F12, 0x011E, WORD_LEN},		//awbb_IntcB //                                                                        
	{0x002A, 0x0D46, WORD_LEN},	    
	{0x0F12, 0x04C0, WORD_LEN},		//0554//055D//0396//04A2//awbb_MvEq_RBthresh //	                                                                        
	                     
	                     
	                     
	{0x002A, 0x0D28, WORD_LEN},	    //wp outdoor
	{0x0F12, 0x0270, WORD_LEN},		                                                                        
	{0x0F12, 0x0240, WORD_LEN},		                                                                        
	                                                                    
	                                        
	{0x002A, 0x0D5C, WORD_LEN},	    
	{0x0F12, 0x7FFF, WORD_LEN},		                                                                        
	{0x0F12, 0x0050, WORD_LEN},		                                                                        
	                     
	{0x002A, 0x2316, WORD_LEN},	    
	{0x0F12, 0x0006, WORD_LEN},		                                                                        
	                     
	{0x002A, 0x0E44, WORD_LEN},	    
	{0x0F12, 0x0525, WORD_LEN},		                                                                        
	{0x0F12, 0x0400, WORD_LEN},		                                                                        
	{0x0F12, 0x078C, WORD_LEN},		                                                                        
	                     
	{0x002A, 0x0E36, WORD_LEN},	    
	{0x0F12, 0x0028, WORD_LEN},		 //R OFFSET                                                                       
	{0x0F12, 0xFFD8, WORD_LEN},		 //B OFFSET                                                                       
	{0x0F12, 0x0000, WORD_LEN},		 //G OFFSET                                                                       

	{0x002A, 0x0DD4, WORD_LEN},	    
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[0] //        	                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[1] //    
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[2] //    
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[3] //    
	{0x0F12, 0xFFF0, WORD_LEN},		//awbb_GridCorr_R[4] //    
	{0x0F12, 0x0080, WORD_LEN},		//awbb_GridCorr_R[5] //    
	               
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[6] //    
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[7] //    
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[8] //    
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[9] //    
	{0x0F12, 0xFFF0, WORD_LEN},		//awbb_GridCorr_R[10] //   
	{0x0F12, 0x0080, WORD_LEN},		//awbb_GridCorr_R[11] //   
	               
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[12] //   
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[13] //   
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[14] //   
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_R[15] //   
	{0x0F12, 0xFFF0, WORD_LEN},		//awbb_GridCorr_R[16] //   
	{0x0F12, 0x0080, WORD_LEN},		//awbb_GridCorr_R[17] //   
	               
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[0] ////                                                                        
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[1] //                                                                          
	{0x0F12, 0x0032, WORD_LEN},		//awbb_GridCorr_B[2] //                                                                          
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[3] //                                                                          
	{0x0F12, 0x0020, WORD_LEN},		//awbb_GridCorr_B[4] //                                                                          
	{0x0F12, 0xFFC0, WORD_LEN},		//awbb_GridCorr_B[5] //                                                                          
	           
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[6] //                                                                          
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[7] //                                                                          
	{0x0F12, 0x0032, WORD_LEN},		//awbb_GridCorr_B[8] //                                                                          
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[9] //                                                                          
	{0x0F12, 0x0020, WORD_LEN},		//awbb_GridCorr_B[10] //                                                                         
	{0x0F12, 0xFFC0, WORD_LEN},		//awbb_GridCorr_B[11] //                                                                         
	           
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[12] //                                                                         
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[13] //                                                                         
	{0x0F12, 0x0032, WORD_LEN},		//awbb_GridCorr_B[14] //                                                                         
	{0x0F12, 0x0000, WORD_LEN},		//awbb_GridCorr_B[15] //                                                                         
	{0x0F12, 0x0020, WORD_LEN},		//awbb_GridCorr_B[16] //   
	{0x0F12, 0xFFC0, WORD_LEN},		//awbb_GridCorr_B[17] //                 
	                
	{0x0F12, 0x02D9, WORD_LEN},		//awbb_GridConst_1[0] //                                                                     
	{0x0F12, 0x0357, WORD_LEN},		//awbb_GridConst_1[1] //                                                                     
	{0x0F12, 0x03D1, WORD_LEN},		//awbb_GridConst_1[2] //                                                                     
	                
	                
	{0x0F12, 0x0DF6, WORD_LEN},		//0E4F//0DE9//0DE9//awbb_GridConst_2[0] //                                                   
	{0x0F12, 0x0ED8, WORD_LEN},		//0EDD//0EDD//0EDD//awbb_GridConst_2[1] //                                                   
	{0x0F12, 0x0F51, WORD_LEN},		//0F42//0F42//0F42//awbb_GridConst_2[2] //                                                   
	{0x0F12, 0x0F5C, WORD_LEN},		//0F4E//0F4E//0F54//awbb_GridConst_2[3] //                                                   
	{0x0F12, 0x0F8F, WORD_LEN},		//0F99//0F99//0FAE//awbb_GridConst_2[4] //                                                   
	{0x0F12, 0x1006, WORD_LEN},		//1006//1006//1011//awbb_GridConst_2[5] //                                                   
	                
	{0x0F12, 0x00AC, WORD_LEN},		//00BA//awbb_GridCoeff_R_1                                                                   
	{0x0F12, 0x00BD, WORD_LEN},		//00AF//awbb_GridCoeff_B_1                                                                   
	{0x0F12, 0x0049, WORD_LEN},		//0049//awbb_GridCoeff_R_2                                                                   
	{0x0F12, 0x00F5, WORD_LEN},		//00F5//awbb_GridCoeff_B_2                                                                   
	                
	{0x002A, 0x0E4A, WORD_LEN},	                         
	{0x0F12, 0x0002, WORD_LEN},		//awbb_GridEnable//                                                                          
	                
	{0x002A, 0x051A, WORD_LEN},	                         
	{0x0F12, 0x010E, WORD_LEN},		//lt_uLimitHigh//
	{0x0F12, 0x00F5, WORD_LEN},		//lt_uLimitLow// 
	                
	                
	{0x002A, 0x0F76, WORD_LEN},	                         
	{0x0F12, 0x0007, WORD_LEN},		//ae_statmode BLC off : 0x0F, on : 0x0D//  illumType On : 07 , Off : 0F
	                
	{0x002A, 0x1034, WORD_LEN},	                         
	{0x0F12, 0x00C0, WORD_LEN},		//saRR_IllumType[0] //                                                                       
	{0x0F12, 0x00E0, WORD_LEN},		//saRR_IllumType[1] //                                                                       
	{0x0F12, 0x0104, WORD_LEN},		//saRR_IllumType[2] //                                                                       
	{0x0F12, 0x0129, WORD_LEN},		//saRR_IllumType[3] //                                                                       
	{0x0F12, 0x0156, WORD_LEN},		//saRR_IllumType[4] //                                                                       
	{0x0F12, 0x017F, WORD_LEN},		//saRR_IllumType[5] //                                                                       
	{0x0F12, 0x018F, WORD_LEN},		//saRR_IllumType[6] //                                                                       
	                
	                
	{0x0F12, 0x0120, WORD_LEN},		//saRR_IllumTypeF[0] //                                                                      
	{0x0F12, 0x0120, WORD_LEN},		//saRR_IllumTypeF[1] //                                                                      
	{0x0F12, 0x0120, WORD_LEN},		//saRR_IllumTypeF[2] //                                                                      
	{0x0F12, 0x0100, WORD_LEN},		//saRR_IllumTypeF[3] //                                                                      
	{0x0F12, 0x0100, WORD_LEN},		//saRR_IllumTypeF[4] //                                                                      
	{0x0F12, 0x0100, WORD_LEN},		//saRR_IllumTypeF[5] //                                                                      
	{0x0F12, 0x0100, WORD_LEN},		//saRR_IllumTypeF[6] //                                                                      
	                     
	                     
	                     
	{0x002A, 0x3288, WORD_LEN},		//saRR_usDualGammaLutRGBIndoor  //               	                                                                        
	{0x0F12, 0x0000, WORD_LEN},	 //	saRR_usDualGammaLutRGBIndoor[0] //[0] //                                                 
	{0x0F12, 0x0008, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[1] //                                                       
	{0x0F12, 0x0013, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[2] //                                                       
	{0x0F12, 0x002C, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[3] //                                                       
	{0x0F12, 0x0062, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[4] //                                                       
	{0x0F12, 0x00CD, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[5] //                                                       
	{0x0F12, 0x0129, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[6] //                                                       
	{0x0F12, 0x0151, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[7] //                                                       
	{0x0F12, 0x0174, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[8] //                                                       
	{0x0F12, 0x01AA, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[9] //                                                       
	{0x0F12, 0x01D7, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[10] //                                                      
	{0x0F12, 0x01FE, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[11] //                                                      
	{0x0F12, 0x0221, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[12] //                                                      
	{0x0F12, 0x025D, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[13] //                                                      
	{0x0F12, 0x0291, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[14] //                                                      
	{0x0F12, 0x02EB, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[15] //                                                      
	{0x0F12, 0x033A, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[16] //                                                      
	{0x0F12, 0x0380, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[17] //                                                      
	{0x0F12, 0x03C2, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[18] //                                                      
	{0x0F12, 0x03FF, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[0] //[19] //                                                      
	{0x0F12, 0x0000, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[0] //                                                       
	{0x0F12, 0x0008, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[1] //                                                       
	{0x0F12, 0x0013, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[2] //                                                       
	{0x0F12, 0x002C, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[3] //                                                       
	{0x0F12, 0x0062, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[4] //                                                       
	{0x0F12, 0x00CD, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[5] //                                                       
	{0x0F12, 0x0129, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[6] //                                                       
	{0x0F12, 0x0151, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[7] //                                                       
	{0x0F12, 0x0174, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[8] //                                                       
	{0x0F12, 0x01AA, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[9] //                                                       
	{0x0F12, 0x01D7, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[10] //                                                      
	{0x0F12, 0x01FE, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[11] //                                                      
	{0x0F12, 0x0221, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[12] //                                                      
	{0x0F12, 0x025D, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[13] //                                                      
	{0x0F12, 0x0291, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[14] //                                                      
	{0x0F12, 0x02EB, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[15] //                                                      
	{0x0F12, 0x033A, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[16] //                                                      
	{0x0F12, 0x0380, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[17] //                                                      
	{0x0F12, 0x03C2, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[18] //                                                      
	{0x0F12, 0x03FF, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[1] //[19] //                                                      
	{0x0F12, 0x0000, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[0] //                                                       
	{0x0F12, 0x0008, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[1] //                                                       
	{0x0F12, 0x0013, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[2] //                                                       
	{0x0F12, 0x002C, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[3] //                                                       
	{0x0F12, 0x0062, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[4] //                                                       
	{0x0F12, 0x00CD, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[5] //                                                       
	{0x0F12, 0x0129, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[6] //                                                       
	{0x0F12, 0x0151, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[7] //                                                       
	{0x0F12, 0x0174, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[8] //                                                       
	{0x0F12, 0x01AA, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[9] //                                                       
	{0x0F12, 0x01D7, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[10] //                                                      
	{0x0F12, 0x01FE, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[11] //                                                      
	{0x0F12, 0x0221, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[12] //                                                      
	{0x0F12, 0x025D, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[13] //                                                      
	{0x0F12, 0x0291, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[14] //                                                      
	{0x0F12, 0x02EB, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[15] //                                                      
	{0x0F12, 0x033A, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[16] //                                                      
	{0x0F12, 0x0380, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[17] //                                                      
	{0x0F12, 0x03C2, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[18] //                                                      
	{0x0F12, 0x03FF, WORD_LEN},	 //  saRR_usDualGammaLutRGBIndoor[2] //[19] //                                                      
	                                            
	                                            
	{0x0F12, 0x0000, WORD_LEN},		//			saRR_usDualGammaLutRGBOutdoor[0] //[0] //
	{0x0F12, 0x0008, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[1] //                                                
	{0x0F12, 0x0013, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[2] //                                                
	{0x0F12, 0x002C, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[3] //                                                
	{0x0F12, 0x0062, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[4] //                                                
	{0x0F12, 0x00CD, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[5] //                                                
	{0x0F12, 0x0129, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[6] //                                                
	{0x0F12, 0x0151, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[7] //                                                
	{0x0F12, 0x0174, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[8] //                                                
	{0x0F12, 0x01AA, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[9] //                                                
	{0x0F12, 0x01D7, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[10] //                                               
	{0x0F12, 0x01FE, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[11] //                                               
	{0x0F12, 0x0221, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[12] //                                               
	{0x0F12, 0x025D, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[13] //                                               
	{0x0F12, 0x0291, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[14] //                                               
	{0x0F12, 0x02EB, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[15] //                                               
	{0x0F12, 0x033A, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[16] //                                               
	{0x0F12, 0x0380, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[17] //                                               
	{0x0F12, 0x03C2, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[18] //                                               
	{0x0F12, 0x03FF, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[0] //[19] //                                               
	{0x0F12, 0x0000, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[0] //                                                
	{0x0F12, 0x0008, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[1] //                                                
	{0x0F12, 0x0013, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[2] //                                                
	{0x0F12, 0x002C, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[3] //                                                
	{0x0F12, 0x0062, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[4] //                                                
	{0x0F12, 0x00CD, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[5] //                                                
	{0x0F12, 0x0129, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[6] //                                                
	{0x0F12, 0x0151, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[7] //                                                
	{0x0F12, 0x0174, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[8] //                                                
	{0x0F12, 0x01AA, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[9] //                                                
	{0x0F12, 0x01D7, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[10] //                                               
	{0x0F12, 0x01FE, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[11] //                                               
	{0x0F12, 0x0221, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[12] //                                               
	{0x0F12, 0x025D, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[13] //                                               
	{0x0F12, 0x0291, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[14] //                                               
	{0x0F12, 0x02EB, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[15] //                                               
	{0x0F12, 0x033A, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[16] //                                               
	{0x0F12, 0x0380, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[17] //                                               
	{0x0F12, 0x03C2, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[18] //                                               
	{0x0F12, 0x03FF, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[1] //[19] //                                               
	{0x0F12, 0x0000, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[0] //                                                
	{0x0F12, 0x0008, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[1] //                                                
	{0x0F12, 0x0013, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[2] //                                                
	{0x0F12, 0x002C, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[3] //                                                
	{0x0F12, 0x0062, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[4] //                                                
	{0x0F12, 0x00CD, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[5] //                                                
	{0x0F12, 0x0129, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[6] //                                                
	{0x0F12, 0x0151, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[7] //                                                
	{0x0F12, 0x0174, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[8] //                                                
	{0x0F12, 0x01AA, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[9] //                                                
	{0x0F12, 0x01D7, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[10] //                                               
	{0x0F12, 0x01FE, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[11] //                                               
	{0x0F12, 0x0221, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[12] //                                               
	{0x0F12, 0x025D, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[13] //                                               
	{0x0F12, 0x0291, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[14] //                                               
	{0x0F12, 0x02EB, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[15] //                                               
	{0x0F12, 0x033A, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[16] //                                               
	{0x0F12, 0x0380, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[17] //                                               
	{0x0F12, 0x03C2, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[18] //                                               
	{0x0F12, 0x03FF, WORD_LEN},		//  saRR_usDualGammaLutRGBOutdoor[2] //[19] //                                               
	                                            
	                                            
	{0x002A, 0x06A6, WORD_LEN},	                           
	{0x0F12, 0x00C0, WORD_LEN},		//saRR_AwbCcmCord[0] //                      
	{0x0F12, 0x00E0, WORD_LEN},		//saRR_AwbCcmCord[1] //                      
	{0x0F12, 0x0110, WORD_LEN},		//saRR_AwbCcmCord[2] //                      
	{0x0F12, 0x0139, WORD_LEN},		//saRR_AwbCcmCord[3] //                      
	{0x0F12, 0x0166, WORD_LEN},		//saRR_AwbCcmCord[4] //                      
	{0x0F12, 0x019F, WORD_LEN},		//saRR_AwbCcmCord[5] //                      
	                     
	{0x002A, 0x33A4, WORD_LEN},	    
	{0x0F12, 0x0181, WORD_LEN},		  //TVAR_wbt_pBaseCcmS[0] //                                                           
	{0x0F12, 0xFF88, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[1] //                                                        
	{0x0F12, 0xFF90, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[2] //                                                        
	{0x0F12, 0xFE6B, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[3] //                                                        
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[4] //                                                        
	{0x0F12, 0xFF0B, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[5] //                                                        
	{0x0F12, 0xFFDD, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[6] //                                                        
	{0x0F12, 0xFFEE, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[7] //                                                        
	{0x0F12, 0x01CB, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[8] //                                                        
	{0x0F12, 0x0187, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[9] //                                                        
	{0x0F12, 0x00A6, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[10] //                                                       
	{0x0F12, 0xFEBE, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[11] //                                                       
	{0x0F12, 0x021C, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[12] //                                                       
	{0x0F12, 0xFF5F, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[13] //                                                       
	{0x0F12, 0x0175, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[14] //                                                       
	{0x0F12, 0xFEE7, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[15] //                                                       
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[16] //                                                       
	{0x0F12, 0x00F3, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[17] //                                                       
	     
	{0x0F12, 0x0181, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[18] //                                                       
	{0x0F12, 0xFF88, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[19] //                                                       
	{0x0F12, 0xFF90, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[20] //                                                       
	{0x0F12, 0xFE6B, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[21] //                                                       
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[22] //                                                       
	{0x0F12, 0xFF0B, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[23] //                                                       
	{0x0F12, 0xFFDD, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[24] //                                                       
	{0x0F12, 0xFFEE, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[25] //                                                       
	{0x0F12, 0x01CB, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[26] //                                                       
	{0x0F12, 0x0187, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[27] //                                                       
	{0x0F12, 0x00A6, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[28] //                                                       
	{0x0F12, 0xFEBE, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[29] //                                                       
	{0x0F12, 0x021C, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[30] //                                                       
	{0x0F12, 0xFF5F, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[31] //                                                       
	{0x0F12, 0x0175, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[32] //                                                       
	{0x0F12, 0xFEE7, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[33] //                                                       
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[34] //                                                       
	{0x0F12, 0x00F3, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[35] //                                                       
	    
	{0x0F12, 0x0181, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[36] //                                                       
	{0x0F12, 0xFF88, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[37] //                                                       
	{0x0F12, 0xFF90, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[38] //                                                       
	{0x0F12, 0xFE6B, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[39] //                                                       
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[40] //                                                       
	{0x0F12, 0xFF0B, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[41] //                                                       
	{0x0F12, 0xFFDD, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[42] //                                                       
	{0x0F12, 0xFFEE, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[43] //                                                       
	{0x0F12, 0x01CB, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[44] //                                                       
	{0x0F12, 0x0187, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[45] //                                                       
	{0x0F12, 0x00A6, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[46] //                                                       
	{0x0F12, 0xFEBE, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[47] //                                                       
	{0x0F12, 0x021C, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[48] //                                                       
	{0x0F12, 0xFF5F, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[49] //                                                       
	{0x0F12, 0x0175, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[50] //                                                       
	{0x0F12, 0xFEE7, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[51] //                                                       
	{0x0F12, 0x0106, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[52] //                                                       
	{0x0F12, 0x00F3, WORD_LEN},	   //TVAR_wbt_pBaseCcmS[53] //                                                                        
	   
	{0x0F12, 0x01FD, WORD_LEN},	  //01FA   //TVAR_wbt_pBaseCcmS[54] //                                                       
	{0x0F12, 0xFFAB, WORD_LEN},	  //FF9B   //TVAR_wbt_pBaseCcmS[55] //                                                       
	{0x0F12, 0xFFED, WORD_LEN},	  //FFFF   //TVAR_wbt_pBaseCcmS[56] //                                                       
	{0x0F12, 0xFEB5, WORD_LEN},	  //FE9F   //TVAR_wbt_pBaseCcmS[57] //                                                       
	{0x0F12, 0x0112, WORD_LEN},	  //010F   //TVAR_wbt_pBaseCcmS[58] //                                                       
	{0x0F12, 0xFEDC, WORD_LEN},	  //FEF5   //TVAR_wbt_pBaseCcmS[59] //                                                       
	{0x0F12, 0xFFD2, WORD_LEN},	  //FFD2   //TVAR_wbt_pBaseCcmS[60] //                                                       
	{0x0F12, 0x0015, WORD_LEN},	  //0015   //TVAR_wbt_pBaseCcmS[61] //                                                       
	{0x0F12, 0x01A1, WORD_LEN},	  //01A1   //TVAR_wbt_pBaseCcmS[62] //                                                       
	{0x0F12, 0x0111, WORD_LEN},	  //0111   //TVAR_wbt_pBaseCcmS[63] //                                                       
	{0x0F12, 0x009D, WORD_LEN},	  //009D   //TVAR_wbt_pBaseCcmS[64] //                                                       
	{0x0F12, 0xFECB, WORD_LEN},	  //FECB   //TVAR_wbt_pBaseCcmS[65] //                                                       
	{0x0F12, 0x01FC, WORD_LEN},	  //01FC   //TVAR_wbt_pBaseCcmS[66] //                                                       
	{0x0F12, 0xFF99, WORD_LEN},	  //FF99   //TVAR_wbt_pBaseCcmS[67] //                                                       
	{0x0F12, 0x01A9, WORD_LEN},	  //01A9   //TVAR_wbt_pBaseCcmS[68] //                                                       
	{0x0F12, 0xFF26, WORD_LEN},	  //FF26   //TVAR_wbt_pBaseCcmS[69] //                                                       
	{0x0F12, 0x012B, WORD_LEN},	  //012B   //TVAR_wbt_pBaseCcmS[70] //                                                       
	{0x0F12, 0x00DF, WORD_LEN},	  //00DF   //TVAR_wbt_pBaseCcmS[71] //    
	      
	{0x0F12, 0x01E3, WORD_LEN},	  //01E2   //TVAR_wbt_pBaseCcmS[72] //                                                 
	{0x0F12, 0xFFA4, WORD_LEN},	  //FF9A   //TVAR_wbt_pBaseCcmS[73] //                                                 
	{0x0F12, 0xFFDB, WORD_LEN},	  //FFE7   //TVAR_wbt_pBaseCcmS[74] //                                                 
	{0x0F12, 0xFEB5, WORD_LEN},	  //FE9F   //TVAR_wbt_pBaseCcmS[75] //                                                 
	{0x0F12, 0x0112, WORD_LEN},	  //010F   //TVAR_wbt_pBaseCcmS[76] //                                                 
	{0x0F12, 0xFEDC, WORD_LEN},	  //FEF5   //TVAR_wbt_pBaseCcmS[77] //                                                 
	{0x0F12, 0xFFD2, WORD_LEN},	  //FFD2   //TVAR_wbt_pBaseCcmS[78] //                                                 
	{0x0F12, 0xFFFE, WORD_LEN},	  //FFFE   //TVAR_wbt_pBaseCcmS[79] //                                                 
	{0x0F12, 0x01B7, WORD_LEN},	  //01B7   //TVAR_wbt_pBaseCcmS[80] //                                                 
	{0x0F12, 0x00E8, WORD_LEN},	  //00E8   //TVAR_wbt_pBaseCcmS[81] //                                                 
	{0x0F12, 0x0095, WORD_LEN},	  //0095   //TVAR_wbt_pBaseCcmS[82] //                                                 
	{0x0F12, 0xFF0D, WORD_LEN},	  //FF0D   //TVAR_wbt_pBaseCcmS[83] //                                                 
	{0x0F12, 0x0182, WORD_LEN},	  //0182   //TVAR_wbt_pBaseCcmS[84] //                                                 
	{0x0F12, 0xFF29, WORD_LEN},	  //FF29   //TVAR_wbt_pBaseCcmS[85] //                                                 
	{0x0F12, 0x0146, WORD_LEN},	  //0146   //TVAR_wbt_pBaseCcmS[86] //                                                 
	{0x0F12, 0xFF26, WORD_LEN},	  //FF26   //TVAR_wbt_pBaseCcmS[87] //                                                 
	{0x0F12, 0x012B, WORD_LEN},	  //012B   //TVAR_wbt_pBaseCcmS[88] //                                                 
	{0x0F12, 0x00DF, WORD_LEN},	  //00DF   //TVAR_wbt_pBaseCcmS[89] //                                                                         

	{0x0F12, 0x01E3, WORD_LEN},	  //01E2   //TVAR_wbt_pBaseCcmS[90] //                                                 
	{0x0F12, 0xFFA4, WORD_LEN},	  //FF9A   //TVAR_wbt_pBaseCcmS[91] //                                                 
	{0x0F12, 0xFFDB, WORD_LEN},	  //FFE7   //TVAR_wbt_pBaseCcmS[92] //                                                 
	{0x0F12, 0xFEB5, WORD_LEN},	  //FE9F   //TVAR_wbt_pBaseCcmS[93] //                                                 
	{0x0F12, 0x0112, WORD_LEN},	  //010F   //TVAR_wbt_pBaseCcmS[94] //                                                 
	{0x0F12, 0xFEDC, WORD_LEN},	  //FEF5   //TVAR_wbt_pBaseCcmS[95] //                                                 
	{0x0F12, 0xFFD2, WORD_LEN},	  //FFD2   //TVAR_wbt_pBaseCcmS[96] //                                                 
	{0x0F12, 0xFFFE, WORD_LEN},	  //FFFE   //TVAR_wbt_pBaseCcmS[97] //                                                 
	{0x0F12, 0x01B7, WORD_LEN},	  //01B7   //TVAR_wbt_pBaseCcmS[98] //                                                 
	{0x0F12, 0x00E8, WORD_LEN},	  //00E8   //TVAR_wbt_pBaseCcmS[99] //                                                 
	{0x0F12, 0x0095, WORD_LEN},	  //0095   //TVAR_wbt_pBaseCcmS[100] //                                                
	{0x0F12, 0xFF0D, WORD_LEN},	  //FF0D   //TVAR_wbt_pBaseCcmS[101] //                                                
	{0x0F12, 0x0182, WORD_LEN},	  //0182   //TVAR_wbt_pBaseCcmS[102] //                                                
	{0x0F12, 0xFF29, WORD_LEN},	  //FF29   //TVAR_wbt_pBaseCcmS[103] //                                                
	{0x0F12, 0x0146, WORD_LEN},	  //0146   //TVAR_wbt_pBaseCcmS[104] //                                                
	{0x0F12, 0xFF26, WORD_LEN},	  //FF26   //TVAR_wbt_pBaseCcmS[105] //                                                
	{0x0F12, 0x012B, WORD_LEN},	  //012B   //TVAR_wbt_pBaseCcmS[106] //                                                
	{0x0F12, 0x00DF, WORD_LEN},	  //00DF   //TVAR_wbt_pBaseCcmS[107] //                                                
	                                       
	{0x002A, 0x3380, WORD_LEN},	  //12시최적     
	{0x0F12, 0x020E, WORD_LEN},	  //0223   //0223  //01F3  //01F3  //TVAR_wbt_pOutdoorCcm[0] //  
	{0x0F12, 0xFF88, WORD_LEN},	  //FF7C   //FF7C  //FFA4  //FFA4  //TVAR_wbt_pOutdoorCcm[1] //  
	{0x0F12, 0xFFCE, WORD_LEN},	  //FFC5   //FFC5  //FFE4  //FFE4  //TVAR_wbt_pOutdoorCcm[2] //  
	{0x0F12, 0xFE3D, WORD_LEN},	  //FE3D   //FE3D  //FE3D  //FE23  //TVAR_wbt_pOutdoorCcm[3] //  
	{0x0F12, 0x0158, WORD_LEN},	  //0158   //0158  //0158  //017D  //TVAR_wbt_pOutdoorCcm[4] //  
	{0x0F12, 0xFF03, WORD_LEN},	  //FF03   //FF03  //FF03  //FEF9  //TVAR_wbt_pOutdoorCcm[5] //  
	{0x0F12, 0xFF99, WORD_LEN},	  //FF9F   //FF9F  //FF9F  //FF9F  //TVAR_wbt_pOutdoorCcm[6] //  
	{0x0F12, 0x0018, WORD_LEN},	  //0011   //0011  //0011  //0011  //TVAR_wbt_pOutdoorCcm[7] //  
	{0x0F12, 0x0235, WORD_LEN},	  //0237   //0237  //0237  //0237  //TVAR_wbt_pOutdoorCcm[8] //  
	{0x0F12, 0x0101, WORD_LEN},	  //00EB   //00D1  //012A  //0143  //TVAR_wbt_pOutdoorCcm[9] //  
	{0x0F12, 0x0116, WORD_LEN},	  //012A   //0125  //00CA  //00F6  //TVAR_wbt_pOutdoorCcm[10] // 
	{0x0F12, 0xFF00, WORD_LEN},	  //FF02   //FEF5  //FEF6  //FEB1  //TVAR_wbt_pOutdoorCcm[11] // 
	{0x0F12, 0x01C5, WORD_LEN},	  //01C5   //01C5  //01C5  //01C5  //TVAR_wbt_pOutdoorCcm[12] // 
	{0x0F12, 0xFF80, WORD_LEN},	  //FF80   //FF80  //FF80  //FF80  //TVAR_wbt_pOutdoorCcm[13] // 
	{0x0F12, 0x019D, WORD_LEN},	  //019D   //019D  //019D  //019D  //TVAR_wbt_pOutdoorCcm[14] // 
	{0x0F12, 0xFE7A, WORD_LEN},	  //FE7A   //FE7A  //FE7A  //FE7A  //TVAR_wbt_pOutdoorCcm[15] // 
	{0x0F12, 0x0179, WORD_LEN},	  //0179   //0179  //0179  //0179  //TVAR_wbt_pOutdoorCcm[16] // 
	{0x0F12, 0x0179, WORD_LEN},	  //0179   //0179  //0179  //0179  //TVAR_wbt_pOutdoorCcm[17] // 
	                   
	                        
	                        
	{0x002A, 0x0764, WORD_LEN},	       
	{0x0F12, 0x0049, WORD_LEN},		//afit_uNoiseIndInDoor[0] //                                                                         
	{0x0F12, 0x005F, WORD_LEN},		//afit_uNoiseIndInDoor[1] //                                                                         
	{0x0F12, 0x00CB, WORD_LEN},		//afit_uNoiseIndInDoor[2] // 203//                                                                   
	{0x0F12, 0x01E0, WORD_LEN},		//afit_uNoiseIndInDoor[3] // Indoor_NB below 1500 _Noise index 300-400d //                           
	{0x0F12, 0x0220, WORD_LEN},		//afit_uNoiseIndInDoor[4] // DNP NB 4600 _ Noisenidex :560d-230h //                                  
	                        
	                        
	{0x002A, 0x07C4, WORD_LEN},	       
	{0x0F12, 0x0014, WORD_LEN},		//700007C4 //TVAR_afit_pBaseValS[0] // AFIT16_BRIGHTNESS                                                                                  
	{0x0F12, 0x0000, WORD_LEN},		//700007C6 //TVAR_afit_pBaseValS[1] // AFIT16_CONTRAST                                                                                    
	{0x0F12, 0x0014, WORD_LEN},		//700007C8 //TVAR_afit_pBaseValS[2] // AFIT16_SATURATION                                                                                  
	{0x0F12, 0x0000, WORD_LEN},		//700007CA //TVAR_afit_pBaseValS[3] // AFIT16_SHARP_BLUR                                                                                  
	{0x0F12, 0x0000, WORD_LEN},		//700007CC //TVAR_afit_pBaseValS[4] // AFIT16_GLAMOUR                                                                                     
	{0x0F12, 0x00C1, WORD_LEN},		//700007CE //TVAR_afit_pBaseValS[5] // AFIT16_sddd8a_edge_high                                                                            
	{0x0F12, 0x03FF, WORD_LEN},		//700007D0 //TVAR_afit_pBaseValS[6] // AFIT16_Demosaicing_iSatVal                                                                         
	{0x0F12, 0x009C, WORD_LEN},		//700007D2 //TVAR_afit_pBaseValS[7] // AFIT16_Sharpening_iReduceEdgeThresh                                                                
	{0x0F12, 0x0251, WORD_LEN},		//700007D4 //TVAR_afit_pBaseValS[8] // AFIT16_demsharpmix1_iRGBOffset                                                                     
	{0x0F12, 0x03FF, WORD_LEN},		//700007D6 //TVAR_afit_pBaseValS[9] // AFIT16_demsharpmix1_iDemClamp                                                                      
	{0x0F12, 0x000C, WORD_LEN},		//700007D8 //TVAR_afit_pBaseValS[10] //AFIT16_demsharpmix1_iLowThreshold                                                                  
	{0x0F12, 0x0010, WORD_LEN},		//700007DA //TVAR_afit_pBaseValS[11] //AFIT16_demsharpmix1_iHighThreshold                                                                 
	{0x0F12, 0x012C, WORD_LEN},		//700007DC //TVAR_afit_pBaseValS[12] //AFIT16_demsharpmix1_iLowBright                                                                     
	{0x0F12, 0x03E8, WORD_LEN},		//700007DE //TVAR_afit_pBaseValS[13] //AFIT16_demsharpmix1_iHighBright                                                                    
	{0x0F12, 0x0046, WORD_LEN},		//700007E0 //TVAR_afit_pBaseValS[14] //AFIT16_demsharpmix1_iLowSat                                                                        
	{0x0F12, 0x005A, WORD_LEN},		//700007E2 //TVAR_afit_pBaseValS[15] //AFIT16_demsharpmix1_iHighSat                                                                       
	{0x0F12, 0x0070, WORD_LEN},		//700007E4 //TVAR_afit_pBaseValS[16] //AFIT16_demsharpmix1_iTune                                                                          
	{0x0F12, 0x0000, WORD_LEN},		//700007E6 //TVAR_afit_pBaseValS[17] //AFIT16_demsharpmix1_iHystThLow                                                                     
	{0x0F12, 0x0000, WORD_LEN},		//700007E8 //TVAR_afit_pBaseValS[18] //AFIT16_demsharpmix1_iHystThHigh                                                                    
	{0x0F12, 0x01AA, WORD_LEN},		//700007EA //TVAR_afit_pBaseValS[19] //AFIT16_demsharpmix1_iHystCenter                                                                    
	{0x0F12, 0x003C, WORD_LEN},		//700007EC //TVAR_afit_pBaseValS[20] //AFIT16_YUV422_DENOISE_iUVLowThresh                                                                 
	{0x0F12, 0x003C, WORD_LEN},		//700007EE //TVAR_afit_pBaseValS[21] //AFIT16_YUV422_DENOISE_iUVHighThresh                                                                
	{0x0F12, 0x0000, WORD_LEN},		//700007F0 //TVAR_afit_pBaseValS[22] //AFIT16_YUV422_DENOISE_iYLowThresh                                                                  
	{0x0F12, 0x0000, WORD_LEN},		//700007F2 //TVAR_afit_pBaseValS[23] //AFIT16_YUV422_DENOISE_iYHighThresh                                                                 
	{0x0F12, 0x003E, WORD_LEN},		//700007F4 //TVAR_afit_pBaseValS[24] //AFIT16_Sharpening_iLowSharpClamp                                                                   
	{0x0F12, 0x0008, WORD_LEN},		//700007F6 //TVAR_afit_pBaseValS[25] //AFIT16_Sharpening_iHighSharpClamp                                                                  
	{0x0F12, 0x003C, WORD_LEN},		//700007F8 //TVAR_afit_pBaseValS[26] //AFIT16_Sharpening_iLowSharpClamp_Bin                                                               
	{0x0F12, 0x001E, WORD_LEN},		//700007FA //TVAR_afit_pBaseValS[27] //AFIT16_Sharpening_iHighSharpClamp_Bin                                                              
	{0x0F12, 0x003C, WORD_LEN},		//700007FC //TVAR_afit_pBaseValS[28] //AFIT16_Sharpening_iLowSharpClamp_sBin                                                              
	{0x0F12, 0x001E, WORD_LEN},		//700007FE //TVAR_afit_pBaseValS[29] //AFIT16_Sharpening_iHighSharpClamp_sBin                                                             
	{0x0F12, 0x0A24, WORD_LEN},		//70000800 //TVAR_afit_pBaseValS[30] //AFIT8_sddd8a_edge_low [7:0],   AFIT8_sddd8a_repl_thresh [15:8]                                     
	{0x0F12, 0x1701, WORD_LEN},		//70000802 //TVAR_afit_pBaseValS[31] //AFIT8_sddd8a_repl_force [7:0],  AFIT8_sddd8a_sat_level [15:8]                                      
	{0x0F12, 0x0229, WORD_LEN},		//70000804 //TVAR_afit_pBaseValS[32] //AFIT8_sddd8a_sat_thr[7:0],  AFIT8_sddd8a_sat_mpl [15:8]                                            
	{0x0F12, 0x1403, WORD_LEN},		//70000806 //TVAR_afit_pBaseValS[33] //AFIT8_sddd8a_sat_noise[7:0],  AFIT8_sddd8a_iMaxSlopeAllowed [15:8]                                 
	{0x0F12, 0x0000, WORD_LEN},		//70000808 //TVAR_afit_pBaseValS[34] //AFIT8_sddd8a_iHotThreshHigh[7:0],  AFIT8_sddd8a_iHotThreshLow [15:8]                               
	{0x0F12, 0x0000, WORD_LEN},		//7000080A //TVAR_afit_pBaseValS[35] //AFIT8_sddd8a_iColdThreshHigh[7:0],  AFIT8_sddd8a_iColdThreshLow [15:8]                             
	{0x0F12, 0x0000, WORD_LEN},		//7000080C //TVAR_afit_pBaseValS[36] //AFIT8_sddd8a_AddNoisePower1[7:0],  AFIT8_sddd8a_AddNoisePower2 [15:8]                              
	{0x0F12, 0x00FF, WORD_LEN},		//7000080E //TVAR_afit_pBaseValS[37] //AFIT8_sddd8a_iSatSat[7:0],   AFIT8_sddd8a_iRadialTune [15:8]                                       
	{0x0F12, 0x045A, WORD_LEN},		//70000810 //TVAR_afit_pBaseValS[38] //AFIT8_sddd8a_iRadialLimit [7:0],   AFIT8_sddd8a_iRadialPower [15:8]                                
	{0x0F12, 0x1414, WORD_LEN},		//70000812 //TVAR_afit_pBaseValS[39] //AFIT8_sddd8a_iLowMaxSlopeAllowed [7:0],  AFIT8_sddd8a_iHighMaxSlopeAllowed [15:8]                  
	{0x0F12, 0x0301, WORD_LEN},		//70000814 //TVAR_afit_pBaseValS[40] //AFIT8_sddd8a_iLowSlopeThresh[7:0],   AFIT8_sddd8a_iHighSlopeThresh [15:8]                          
	{0x0F12, 0xFF07, WORD_LEN},		//70000816 //TVAR_afit_pBaseValS[41] //AFIT8_sddd8a_iSquaresRounding [7:0],   AFIT8_Demosaicing_iCentGrad [15:8]                          
	{0x0F12, 0x081E, WORD_LEN},		//70000818 //TVAR_afit_pBaseValS[42] //AFIT8_Demosaicing_iMonochrom [7:0],   AFIT8_Demosaicing_iDecisionThresh [15:8]                     
	{0x0F12, 0x0A14, WORD_LEN},		//7000081A //TVAR_afit_pBaseValS[43] //AFIT8_Demosaicing_iDesatThresh [7:0],   AFIT8_Demosaicing_iEnhThresh [15:8]                        
	{0x0F12, 0x0F0F, WORD_LEN},		//7000081C //TVAR_afit_pBaseValS[44] //AFIT8_Demosaicing_iGRDenoiseVal [7:0],   AFIT8_Demosaicing_iGBDenoiseVal [15:8]                    
	{0x0F12, 0x0A00, WORD_LEN},		//7000081E //TVAR_afit_pBaseValS[45] //AFIT8_Demosaicing_iNearGrayDesat[7:0],   AFIT8_Demosaicing_iDFD_ReduceCoeff [15:8]                 
	{0x0F12, 0x0032, WORD_LEN},		//70000820 //TVAR_afit_pBaseValS[46] //AFIT8_Sharpening_iMSharpen [7:0],   AFIT8_Sharpening_iMShThresh [15:8]                             
	{0x0F12, 0x000E, WORD_LEN},		//70000822 //TVAR_afit_pBaseValS[47] //AFIT8_Sharpening_iWSharpen [7:0],   AFIT8_Sharpening_iWShThresh [15:8]                             
	{0x0F12, 0x0002, WORD_LEN},		//70000824 //TVAR_afit_pBaseValS[48] //AFIT8_Sharpening_nSharpWidth [7:0],   AFIT8_Sharpening_iReduceNegative [15:8]                      
	{0x0F12, 0x00FF, WORD_LEN},		//70000826 //TVAR_afit_pBaseValS[49] //AFIT8_Sharpening_iShDespeckle [7:0],  AFIT8_demsharpmix1_iRGBMultiplier [15:8]                     
	{0x0F12, 0x1102, WORD_LEN},		//70000828 //TVAR_afit_pBaseValS[50] //AFIT8_demsharpmix1_iFilterPower [7:0],  AFIT8_demsharpmix1_iBCoeff [15:8]                          
	{0x0F12, 0x001B, WORD_LEN},		//7000082A //TVAR_afit_pBaseValS[51] //AFIT8_demsharpmix1_iGCoeff [7:0],   AFIT8_demsharpmix1_iWideMult [15:8]                            
	{0x0F12, 0x0900, WORD_LEN},		//7000082C //TVAR_afit_pBaseValS[52] //AFIT8_demsharpmix1_iNarrMult [7:0],   AFIT8_demsharpmix1_iHystFalloff [15:8]                       
	{0x0F12, 0x0600, WORD_LEN},		//7000082E //TVAR_afit_pBaseValS[53] //AFIT8_demsharpmix1_iHystMinMult [7:0],   AFIT8_demsharpmix1_iHystWidth [15:8]                      
	{0x0F12, 0x0504, WORD_LEN},		//70000830 //TVAR_afit_pBaseValS[54] //AFIT8_demsharpmix1_iHystFallLow [7:0],   AFIT8_demsharpmix1_iHystFallHigh [15:8]                   
	{0x0F12, 0x0306, WORD_LEN},		//70000832 //TVAR_afit_pBaseValS[55] //AFIT8_demsharpmix1_iHystTune [7:0],  * AFIT8_YUV422_DENOISE_iUVSupport [15:8]                      
	{0x0F12, 0x4603, WORD_LEN},		//70000834 //TVAR_afit_pBaseValS[56] //AFIT8_YUV422_DENOISE_iYSupport [7:0],   AFIT8_byr_cgras_iShadingPower [15:8]                       
	{0x0F12, 0x0480, WORD_LEN},		//70000836 //TVAR_afit_pBaseValS[57] //AFIT8_RGBGamma2_iLinearity [7:0],  AFIT8_RGBGamma2_iDarkReduce [15:8]                              
	{0x0F12, 0x003C, WORD_LEN},		//70000838 //TVAR_afit_pBaseValS[58] //AFIT8_ccm_oscar_iSaturation[7:0],   AFIT8_RGB2YUV_iYOffset [15:8]                                  
	{0x0F12, 0x0080, WORD_LEN},		//7000083A //TVAR_afit_pBaseValS[59] //AFIT8_RGB2YUV_iRGBGain [7:0],   AFIT8_RGB2YUV_iSaturation [15:8]                                   
	{0x0F12, 0x0101, WORD_LEN},		//7000083C //TVAR_afit_pBaseValS[60] //AFIT8_sddd8a_iClustThresh_H [7:0],  AFIT8_sddd8a_iClustThresh_C [15:8]                             
	{0x0F12, 0x0707, WORD_LEN},		//7000083E //TVAR_afit_pBaseValS[61] //AFIT8_sddd8a_iClustMulT_H [7:0],   AFIT8_sddd8a_iClustMulT_C [15:8]                                
	{0x0F12, 0x4601, WORD_LEN},		//70000840 //TVAR_afit_pBaseValS[62] //AFIT8_sddd8a_nClustLevel_H [7:0],   AFIT8_sddd8a_DispTH_Low [15:8]                                 
	{0x0F12, 0x4944, WORD_LEN},		//70000842 //TVAR_afit_pBaseValS[63] //AFIT8_sddd8a_DispTH_High [7:0],   AFIT8_sddd8a_iDenThreshLow [15:8]                                
	{0x0F12, 0x5044, WORD_LEN},		//70000844 //TVAR_afit_pBaseValS[64] //AFIT8_sddd8a_iDenThreshHigh[7:0],   AFIT8_Demosaicing_iEdgeDesat [15:8]                            
	{0x0F12, 0x0500, WORD_LEN},		//70000846 //TVAR_afit_pBaseValS[65] //AFIT8_Demosaicing_iEdgeDesatThrLow [7:0],   AFIT8_Demosaicing_iEdgeDesatThrHigh [15:8]             
	{0x0F12, 0x0003, WORD_LEN},		//70000848 //TVAR_afit_pBaseValS[66] //AFIT8_Demosaicing_iEdgeDesatLimit[7:0],  AFIT8_Demosaicing_iDemSharpenLow [15:8]                   
	{0x0F12, 0x5400, WORD_LEN},		//7000084A //TVAR_afit_pBaseValS[67] //AFIT8_Demosaicing_iDemSharpenHigh[7:0],   AFIT8_Demosaicing_iDemSharpThresh [15:8]                 
	{0x0F12, 0x0714, WORD_LEN},		//7000084C //TVAR_afit_pBaseValS[68] //AFIT8_Demosaicing_iDemShLowLimit [7:0],   AFIT8_Demosaicing_iDespeckleForDemsharp [15:8]           
	{0x0F12, 0x32FF, WORD_LEN},		//7000084E //TVAR_afit_pBaseValS[69] //AFIT8_Demosaicing_iDemBlurLow[7:0],   AFIT8_Demosaicing_iDemBlurHigh [15:8]                        
	{0x0F12, 0x5A04, WORD_LEN},		//70000850 //TVAR_afit_pBaseValS[70] //AFIT8_Demosaicing_iDemBlurRange[7:0],   AFIT8_Sharpening_iLowSharpPower [15:8]                     
	{0x0F12, 0x201E, WORD_LEN},		//70000852 //TVAR_afit_pBaseValS[71] //AFIT8_Sharpening_iHighSharpPower[7:0],   AFIT8_Sharpening_iLowShDenoise [15:8]                     
	{0x0F12, 0x4012, WORD_LEN},		//70000854 //TVAR_afit_pBaseValS[72] //AFIT8_Sharpening_iHighShDenoise [7:0],   AFIT8_Sharpening_iReduceEdgeMinMult [15:8]                
	{0x0F12, 0x0204, WORD_LEN},		//70000856 //TVAR_afit_pBaseValS[73] //AFIT8_Sharpening_iReduceEdgeSlope [7:0],  AFIT8_demsharpmix1_iWideFiltReduce [15:8]                
	{0x0F12, 0x1403, WORD_LEN},		//70000858 //TVAR_afit_pBaseValS[74] //AFIT8_demsharpmix1_iNarrFiltReduce [7:0],  AFIT8_sddd8a_iClustThresh_H_Bin [15:8]                  
	{0x0F12, 0x0114, WORD_LEN},		//7000085A //TVAR_afit_pBaseValS[75] //AFIT8_sddd8a_iClustThresh_C_Bin [7:0],   AFIT8_sddd8a_iClustMulT_H_Bin [15:8]                      
	{0x0F12, 0x0101, WORD_LEN},		//7000085C //TVAR_afit_pBaseValS[76] //AFIT8_sddd8a_iClustMulT_C_Bin [7:0],   AFIT8_sddd8a_nClustLevel_H_Bin [15:8]                       
	{0x0F12, 0x4446, WORD_LEN},		//7000085E //TVAR_afit_pBaseValS[77] //AFIT8_sddd8a_DispTH_Low_Bin [7:0],   AFIT8_sddd8a_DispTH_High_Bin [15:8]                           
	{0x0F12, 0x646E, WORD_LEN},		//70000860 //TVAR_afit_pBaseValS[78] //AFIT8_sddd8a_iDenThreshLow_Bin [7:0],   AFIT8_sddd8a_iDenThreshHigh_Bin [15:8]                     
	{0x0F12, 0x0028, WORD_LEN},		//70000862 //TVAR_afit_pBaseValS[79] //AFIT8_Demosaicing_iEdgeDesat_Bin[7:0],   AFIT8_Demosaicing_iEdgeDesatThrLow_Bin [15:8]             
	{0x0F12, 0x030A, WORD_LEN},		//70000864 //TVAR_afit_pBaseValS[80] //AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin [7:0],  AFIT8_Demosaicing_iEdgeDesatLimit_Bin [15:8]       
	{0x0F12, 0x0000, WORD_LEN},		//70000866 //TVAR_afit_pBaseValS[81] //AFIT8_Demosaicing_iDemSharpenLow_Bin [7:0],  AFIT8_Demosaicing_iDemSharpenHigh_Bin [15:8]          
	{0x0F12, 0x141E, WORD_LEN},		//70000868 //TVAR_afit_pBaseValS[82] //AFIT8_Demosaicing_iDemSharpThresh_Bin [7:0],  AFIT8_Demosaicing_iDemShLowLimit_Bin [15:8]          
	{0x0F12, 0xFF07, WORD_LEN},		//7000086A //TVAR_afit_pBaseValS[83] //AFIT8_Demosaicing_iDespeckleForDemsharp_Bin [7:0],  AFIT8_Demosaicing_iDemBlurLow_Bin [15:8]       
	{0x0F12, 0x0432, WORD_LEN},		//7000086C //TVAR_afit_pBaseValS[84] //AFIT8_Demosaicing_iDemBlurHigh_Bin [7:0],  AFIT8_Demosaicing_iDemBlurRange_Bin [15:8]              
	{0x0F12, 0x0000, WORD_LEN},		//7000086E //TVAR_afit_pBaseValS[85] //AFIT8_Sharpening_iLowSharpPower_Bin [7:0],  AFIT8_Sharpening_iHighSharpPower_Bin [15:8]            
	{0x0F12, 0x0F0F, WORD_LEN},		//70000870 //TVAR_afit_pBaseValS[86] //AFIT8_Sharpening_iLowShDenoise_Bin [7:0],  AFIT8_Sharpening_iHighShDenoise_Bin [15:8]              
	{0x0F12, 0x0440, WORD_LEN},		//70000872 //TVAR_afit_pBaseValS[87] //AFIT8_Sharpening_iReduceEdgeMinMult_Bin [7:0],  AFIT8_Sharpening_iReduceEdgeSlope_Bin [15:8]       
	{0x0F12, 0x0302, WORD_LEN},		//70000874 //TVAR_afit_pBaseValS[88] //AFIT8_demsharpmix1_iWideFiltReduce_Bin [7:0],  AFIT8_demsharpmix1_iNarrFiltReduce_Bin [15:8]       
	{0x0F12, 0x1414, WORD_LEN},		//70000876 //TVAR_afit_pBaseValS[89] //AFIT8_sddd8a_iClustThresh_H_sBin[7:0],   AFIT8_sddd8a_iClustThresh_C_sBin [15:8]                   
	{0x0F12, 0x0101, WORD_LEN},		//70000878 //TVAR_afit_pBaseValS[90] //AFIT8_sddd8a_iClustMulT_H_sBin [7:0],   AFIT8_sddd8a_iClustMulT_C_sBin [15:8]                      
	{0x0F12, 0x4601, WORD_LEN},		//7000087A //TVAR_afit_pBaseValS[91] //AFIT8_sddd8a_nClustLevel_H_sBin [7:0],   AFIT8_sddd8a_DispTH_Low_sBin [15:8]                       
	{0x0F12, 0x6E44, WORD_LEN},		//7000087C //TVAR_afit_pBaseValS[92] //AFIT8_sddd8a_DispTH_High_sBin [7:0],   AFIT8_sddd8a_iDenThreshLow_sBin [15:8]                      
	{0x0F12, 0x2864, WORD_LEN},		//7000087E //TVAR_afit_pBaseValS[93] //AFIT8_sddd8a_iDenThreshHigh_sBin[7:0],   AFIT8_Demosaicing_iEdgeDesat_sBin [15:8]                  
	{0x0F12, 0x0A00, WORD_LEN},		//70000880 //TVAR_afit_pBaseValS[94] //AFIT8_Demosaicing_iEdgeDesatThrLow_sBin [7:0],  AFIT8_Demosaicing_iEdgeDesatThrHigh_sBin [15:8]    
	{0x0F12, 0x0003, WORD_LEN},		//70000882 //TVAR_afit_pBaseValS[95] //AFIT8_Demosaicing_iEdgeDesatLimit_sBin [7:0],  AFIT8_Demosaicing_iDemSharpenLow_sBin [15:8]        
	{0x0F12, 0x1E00, WORD_LEN},		//70000884 //TVAR_afit_pBaseValS[96] //AFIT8_Demosaicing_iDemSharpenHigh_sBin [7:0],  AFIT8_Demosaicing_iDemSharpThresh_sBin [15:8]       
	{0x0F12, 0x0714, WORD_LEN},		//70000886 //TVAR_afit_pBaseValS[97] //AFIT8_Demosaicing_iDemShLowLimit_sBin [7:0],  AFIT8_Demosaicing_iDespeckleForDemsharp_sBin [15:8]  
	{0x0F12, 0x32FF, WORD_LEN},		//70000888 //TVAR_afit_pBaseValS[98] //AFIT8_Demosaicing_iDemBlurLow_sBin [7:0],  AFIT8_Demosaicing_iDemBlurHigh_sBin [15:8]              
	{0x0F12, 0x0004, WORD_LEN},		//7000088A //TVAR_afit_pBaseValS[99] //AFIT8_Demosaicing_iDemBlurRange_sBin [7:0],  AFIT8_Sharpening_iLowSharpPower_sBin [15:8]           
	{0x0F12, 0x0F00, WORD_LEN},		//7000088C //TVAR_afit_pBaseValS[100] /AFIT8_Sharpening_iHighSharpPower_sBin [7:0],  AFIT8_Sharpening_iLowShDenoise_sBin [15:8]          /
	{0x0F12, 0x400F, WORD_LEN},		//7000088E //TVAR_afit_pBaseValS[101] /AFIT8_Sharpening_iHighShDenoise_sBin [7:0],  AFIT8_Sharpening_iReduceEdgeMinMult_sBin [15:8]      /
	{0x0F12, 0x0204, WORD_LEN},		//70000890 //TVAR_afit_pBaseValS[102] /AFIT8_Sharpening_iReduceEdgeSlope_sBin [7:0],  AFIT8_demsharpmix1_iWideFiltReduce_sBin [15:8]     /
	{0x0F12, 0x0003, WORD_LEN},		//70000892 //TVAR_afit_pBaseValS[103] /AFIT8_demsharpmix1_iNarrFiltReduce_sBin [7:0]                                                     /
	{0x0F12, 0x0000, WORD_LEN},		//70000894 //TVAR_afit_pBaseValS[104] /AFIT16_BRIGHTNESS                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000896 //TVAR_afit_pBaseValS[105] /AFIT16_CONTRAST                                                                                   /
	{0x0F12, 0x0014, WORD_LEN},		//70000898 //TVAR_afit_pBaseValS[106] /AFIT16_SATURATION                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//7000089A //TVAR_afit_pBaseValS[107] /AFIT16_SHARP_BLUR                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//7000089C //TVAR_afit_pBaseValS[108] /AFIT16_GLAMOUR                                                                                    /
	{0x0F12, 0x00C1, WORD_LEN},		//7000089E //TVAR_afit_pBaseValS[109] /AFIT16_sddd8a_edge_high                                                                           /
	{0x0F12, 0x03FF, WORD_LEN},		//700008A0 //TVAR_afit_pBaseValS[110] /AFIT16_Demosaicing_iSatVal                                                                        /
	{0x0F12, 0x009C, WORD_LEN},		//700008A2 //TVAR_afit_pBaseValS[111] /AFIT16_Sharpening_iReduceEdgeThresh                                                               /
	{0x0F12, 0x0251, WORD_LEN},		//700008A4 //TVAR_afit_pBaseValS[112] /AFIT16_demsharpmix1_iRGBOffset                                                                    /
	{0x0F12, 0x03FF, WORD_LEN},		//700008A6 //TVAR_afit_pBaseValS[113] /AFIT16_demsharpmix1_iDemClamp                                                                     /
	{0x0F12, 0x000C, WORD_LEN},		//700008A8 //TVAR_afit_pBaseValS[114] /AFIT16_demsharpmix1_iLowThreshold                                                                 /
	{0x0F12, 0x0010, WORD_LEN},		//700008AA //TVAR_afit_pBaseValS[115] /AFIT16_demsharpmix1_iHighThreshold                                                                /
	{0x0F12, 0x012C, WORD_LEN},		//700008AC //TVAR_afit_pBaseValS[116] /AFIT16_demsharpmix1_iLowBright                                                                    /
	{0x0F12, 0x03E8, WORD_LEN},		//700008AE //TVAR_afit_pBaseValS[117] /AFIT16_demsharpmix1_iHighBright                                                                   /
	{0x0F12, 0x0046, WORD_LEN},		//700008B0 //TVAR_afit_pBaseValS[118] /AFIT16_demsharpmix1_iLowSat                                                                       /
	{0x0F12, 0x005A, WORD_LEN},		//700008B2 //TVAR_afit_pBaseValS[119] /AFIT16_demsharpmix1_iHighSat                                                                      /
	{0x0F12, 0x0070, WORD_LEN},		//700008B4 //TVAR_afit_pBaseValS[120] /AFIT16_demsharpmix1_iTune                                                                         /
	{0x0F12, 0x0000, WORD_LEN},		//700008B6 //TVAR_afit_pBaseValS[121] /AFIT16_demsharpmix1_iHystThLow                                                                    /
	{0x0F12, 0x0000, WORD_LEN},		//700008B8 //TVAR_afit_pBaseValS[122] /AFIT16_demsharpmix1_iHystThHigh                                                                   /
	{0x0F12, 0x01AE, WORD_LEN},		//700008BA //TVAR_afit_pBaseValS[123] /AFIT16_demsharpmix1_iHystCenter                                                                   /
	{0x0F12, 0x001E, WORD_LEN},		//700008BC //TVAR_afit_pBaseValS[124] /AFIT16_YUV422_DENOISE_iUVLowThresh                                                                /
	{0x0F12, 0x001E, WORD_LEN},		//700008BE //TVAR_afit_pBaseValS[125] /AFIT16_YUV422_DENOISE_iUVHighThresh                                                               /
	{0x0F12, 0x0000, WORD_LEN},		//700008C0 //TVAR_afit_pBaseValS[126] /AFIT16_YUV422_DENOISE_iYLowThresh                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//700008C2 //TVAR_afit_pBaseValS[127] /AFIT16_YUV422_DENOISE_iYHighThresh                                                                /
	{0x0F12, 0x003E, WORD_LEN},		//700008C4 //TVAR_afit_pBaseValS[128] /AFIT16_Sharpening_iLowSharpClamp                                                                  /
	{0x0F12, 0x0008, WORD_LEN},		//700008C6 //TVAR_afit_pBaseValS[129] /AFIT16_Sharpening_iHighSharpClamp                                                                 /
	{0x0F12, 0x003C, WORD_LEN},		//700008C8 //TVAR_afit_pBaseValS[130] /AFIT16_Sharpening_iLowSharpClamp_Bin                                                              /
	{0x0F12, 0x001E, WORD_LEN},		//700008CA //TVAR_afit_pBaseValS[131] /AFIT16_Sharpening_iHighSharpClamp_Bin                                                             /
	{0x0F12, 0x003C, WORD_LEN},		//700008CC //TVAR_afit_pBaseValS[132] /AFIT16_Sharpening_iLowSharpClamp_sBin                                                             /
	{0x0F12, 0x001E, WORD_LEN},		//700008CE //TVAR_afit_pBaseValS[133] /AFIT16_Sharpening_iHighSharpClamp_sBin                                                            /
	{0x0F12, 0x0A24, WORD_LEN},		//700008D0 //TVAR_afit_pBaseValS[134] /AFIT8_sddd8a_edge_low [7:0],   AFIT8_sddd8a_repl_thresh [15:8]                                    /
	{0x0F12, 0x1701, WORD_LEN},		//700008D2 //TVAR_afit_pBaseValS[135] /AFIT8_sddd8a_repl_force [7:0],  AFIT8_sddd8a_sat_level [15:8]                                     /
	{0x0F12, 0x0229, WORD_LEN},		//700008D4 //TVAR_afit_pBaseValS[136] /AFIT8_sddd8a_sat_thr[7:0],  AFIT8_sddd8a_sat_mpl [15:8]                                           /
	{0x0F12, 0x1403, WORD_LEN},		//700008D6 //TVAR_afit_pBaseValS[137] /AFIT8_sddd8a_sat_noise[7:0],  AFIT8_sddd8a_iMaxSlopeAllowed [15:8]                                /
	{0x0F12, 0x0000, WORD_LEN},		//700008D8 //TVAR_afit_pBaseValS[138] /AFIT8_sddd8a_iHotThreshHigh[7:0],  AFIT8_sddd8a_iHotThreshLow [15:8]                              /
	{0x0F12, 0x0000, WORD_LEN},		//700008DA //TVAR_afit_pBaseValS[139] /AFIT8_sddd8a_iColdThreshHigh[7:0],  AFIT8_sddd8a_iColdThreshLow [15:8]                            /
	{0x0F12, 0x0000, WORD_LEN},		//700008DC //TVAR_afit_pBaseValS[140] /AFIT8_sddd8a_AddNoisePower1[7:0],  AFIT8_sddd8a_AddNoisePower2 [15:8]                             /
	{0x0F12, 0x00FF, WORD_LEN},		//700008DE //TVAR_afit_pBaseValS[141] /AFIT8_sddd8a_iSatSat[7:0],   AFIT8_sddd8a_iRadialTune [15:8]                                      /
	{0x0F12, 0x045A, WORD_LEN},		//700008E0 //TVAR_afit_pBaseValS[142] /AFIT8_sddd8a_iRadialLimit [7:0],   AFIT8_sddd8a_iRadialPower [15:8]                               /
	{0x0F12, 0x1414, WORD_LEN},		//700008E2 //TVAR_afit_pBaseValS[143] /AFIT8_sddd8a_iLowMaxSlopeAllowed [7:0],  AFIT8_sddd8a_iHighMaxSlopeAllowed [15:8]                 /
	{0x0F12, 0x0301, WORD_LEN},		//700008E4 //TVAR_afit_pBaseValS[144] /AFIT8_sddd8a_iLowSlopeThresh[7:0],   AFIT8_sddd8a_iHighSlopeThresh [15:8]                         /
	{0x0F12, 0xFF07, WORD_LEN},		//700008E6 //TVAR_afit_pBaseValS[145] /AFIT8_sddd8a_iSquaresRounding [7:0],   AFIT8_Demosaicing_iCentGrad [15:8]                         /
	{0x0F12, 0x081E, WORD_LEN},		//700008E8 //TVAR_afit_pBaseValS[146] /AFIT8_Demosaicing_iMonochrom [7:0],   AFIT8_Demosaicing_iDecisionThresh [15:8]                    /
	{0x0F12, 0x0A14, WORD_LEN},		//700008EA //TVAR_afit_pBaseValS[147] /AFIT8_Demosaicing_iDesatThresh [7:0],   AFIT8_Demosaicing_iEnhThresh [15:8]                       /
	{0x0F12, 0x0F0F, WORD_LEN},		//700008EC //TVAR_afit_pBaseValS[148] /AFIT8_Demosaicing_iGRDenoiseVal [7:0],   AFIT8_Demosaicing_iGBDenoiseVal [15:8]                   /
	{0x0F12, 0x0A00, WORD_LEN},		//700008EE //TVAR_afit_pBaseValS[149] /AFIT8_Demosaicing_iNearGrayDesat[7:0],   AFIT8_Demosaicing_iDFD_ReduceCoeff [15:8]                /
	{0x0F12, 0x0032, WORD_LEN},		//700008F0 //TVAR_afit_pBaseValS[150] /AFIT8_Sharpening_iMSharpen [7:0],   AFIT8_Sharpening_iMShThresh [15:8]                            /
	{0x0F12, 0x000E, WORD_LEN},		//700008F2 //TVAR_afit_pBaseValS[151] /AFIT8_Sharpening_iWSharpen [7:0],   AFIT8_Sharpening_iWShThresh [15:8]                            /
	{0x0F12, 0x0002, WORD_LEN},		//700008F4 //TVAR_afit_pBaseValS[152] /AFIT8_Sharpening_nSharpWidth [7:0],   AFIT8_Sharpening_iReduceNegative [15:8]                     /
	{0x0F12, 0x00FF, WORD_LEN},		//700008F6 //TVAR_afit_pBaseValS[153] /AFIT8_Sharpening_iShDespeckle [7:0],  AFIT8_demsharpmix1_iRGBMultiplier [15:8]                    /
	{0x0F12, 0x1102, WORD_LEN},		//700008F8 //TVAR_afit_pBaseValS[154] /AFIT8_demsharpmix1_iFilterPower [7:0],  AFIT8_demsharpmix1_iBCoeff [15:8]                         /
	{0x0F12, 0x001B, WORD_LEN},		//700008FA //TVAR_afit_pBaseValS[155] /AFIT8_demsharpmix1_iGCoeff [7:0],   AFIT8_demsharpmix1_iWideMult [15:8]                           /
	{0x0F12, 0x0900, WORD_LEN},		//700008FC //TVAR_afit_pBaseValS[156] /AFIT8_demsharpmix1_iNarrMult [7:0],   AFIT8_demsharpmix1_iHystFalloff [15:8]                      /
	{0x0F12, 0x0600, WORD_LEN},		//700008FE //TVAR_afit_pBaseValS[157] /AFIT8_demsharpmix1_iHystMinMult [7:0],   AFIT8_demsharpmix1_iHystWidth [15:8]                     /
	{0x0F12, 0x0504, WORD_LEN},		//70000900 //TVAR_afit_pBaseValS[158] /AFIT8_demsharpmix1_iHystFallLow [7:0],   AFIT8_demsharpmix1_iHystFallHigh [15:8]                  /
	{0x0F12, 0x0306, WORD_LEN},		//70000902 //TVAR_afit_pBaseValS[159] /AFIT8_demsharpmix1_iHystTune [7:0],  * AFIT8_YUV422_DENOISE_iUVSupport [15:8]                     /
	{0x0F12, 0x4603, WORD_LEN},		//70000904 //TVAR_afit_pBaseValS[160] /AFIT8_YUV422_DENOISE_iYSupport [7:0],   AFIT8_byr_cgras_iShadingPower [15:8]                      /
	{0x0F12, 0x0480, WORD_LEN},		//70000906 //TVAR_afit_pBaseValS[161] /AFIT8_RGBGamma2_iLinearity [7:0],  AFIT8_RGBGamma2_iDarkReduce [15:8]                             /
	{0x0F12, 0x0046, WORD_LEN},		//70000908 //TVAR_afit_pBaseValS[162] /AFIT8_ccm_oscar_iSaturation[7:0],   AFIT8_RGB2YUV_iYOffset [15:8]                                 /
	{0x0F12, 0x0080, WORD_LEN},		//7000090A //TVAR_afit_pBaseValS[163] /AFIT8_RGB2YUV_iRGBGain [7:0],   AFIT8_RGB2YUV_iSaturation [15:8]                                  /
	{0x0F12, 0x0101, WORD_LEN},		//7000090C //TVAR_afit_pBaseValS[164] /AFIT8_sddd8a_iClustThresh_H [7:0],  AFIT8_sddd8a_iClustThresh_C [15:8]                            /
	{0x0F12, 0x0707, WORD_LEN},		//7000090E //TVAR_afit_pBaseValS[165] /AFIT8_sddd8a_iClustMulT_H [7:0],   AFIT8_sddd8a_iClustMulT_C [15:8]                               /
	{0x0F12, 0x1E01, WORD_LEN},		//70000910 //TVAR_afit_pBaseValS[166] /AFIT8_sddd8a_nClustLevel_H [7:0],   AFIT8_sddd8a_DispTH_Low [15:8]                                /
	{0x0F12, 0x491E, WORD_LEN},		//70000912 //TVAR_afit_pBaseValS[167] /AFIT8_sddd8a_DispTH_High [7:0],   AFIT8_sddd8a_iDenThreshLow [15:8]                               /
	{0x0F12, 0x5044, WORD_LEN},		//70000914 //TVAR_afit_pBaseValS[168] /AFIT8_sddd8a_iDenThreshHigh[7:0],   AFIT8_Demosaicing_iEdgeDesat [15:8]                           /
	{0x0F12, 0x0500, WORD_LEN},		//70000916 //TVAR_afit_pBaseValS[169] /AFIT8_Demosaicing_iEdgeDesatThrLow [7:0],   AFIT8_Demosaicing_iEdgeDesatThrHigh [15:8]            /
	{0x0F12, 0x0004, WORD_LEN},		//70000918 //TVAR_afit_pBaseValS[170] /AFIT8_Demosaicing_iEdgeDesatLimit[7:0],  AFIT8_Demosaicing_iDemSharpenLow [15:8]                  /
	{0x0F12, 0x3C0A, WORD_LEN},		//7000091A //TVAR_afit_pBaseValS[171] /AFIT8_Demosaicing_iDemSharpenHigh[7:0],   AFIT8_Demosaicing_iDemSharpThresh [15:8]                /
	{0x0F12, 0x0714, WORD_LEN},		//7000091C //TVAR_afit_pBaseValS[172] /AFIT8_Demosaicing_iDemShLowLimit [7:0],   AFIT8_Demosaicing_iDespeckleForDemsharp [15:8]          /
	{0x0F12, 0x3214, WORD_LEN},		//7000091E //TVAR_afit_pBaseValS[173] /AFIT8_Demosaicing_iDemBlurLow[7:0],   AFIT8_Demosaicing_iDemBlurHigh [15:8]                       /
	{0x0F12, 0x5A03, WORD_LEN},		//70000920 //TVAR_afit_pBaseValS[174] /AFIT8_Demosaicing_iDemBlurRange[7:0],   AFIT8_Sharpening_iLowSharpPower [15:8]                    /
	{0x0F12, 0x121E, WORD_LEN},		//70000922 //TVAR_afit_pBaseValS[175] /AFIT8_Sharpening_iHighSharpPower[7:0],   AFIT8_Sharpening_iLowShDenoise [15:8]                    /
	{0x0F12, 0x4012, WORD_LEN},		//70000924 //TVAR_afit_pBaseValS[176] /AFIT8_Sharpening_iHighShDenoise [7:0],   AFIT8_Sharpening_iReduceEdgeMinMult [15:8]               /
	{0x0F12, 0x0604, WORD_LEN},		//70000926 //TVAR_afit_pBaseValS[177] /AFIT8_Sharpening_iReduceEdgeSlope [7:0],  AFIT8_demsharpmix1_iWideFiltReduce [15:8]               /
	{0x0F12, 0x1E06, WORD_LEN},		//70000928 //TVAR_afit_pBaseValS[178] /AFIT8_demsharpmix1_iNarrFiltReduce [7:0],  AFIT8_sddd8a_iClustThresh_H_Bin [15:8]                 /
	{0x0F12, 0x011E, WORD_LEN},		//7000092A //TVAR_afit_pBaseValS[179] /AFIT8_sddd8a_iClustThresh_C_Bin [7:0],   AFIT8_sddd8a_iClustMulT_H_Bin [15:8]                     /
	{0x0F12, 0x0101, WORD_LEN},		//7000092C //TVAR_afit_pBaseValS[180] /AFIT8_sddd8a_iClustMulT_C_Bin [7:0],   AFIT8_sddd8a_nClustLevel_H_Bin [15:8]                      /
	{0x0F12, 0x3A3C, WORD_LEN},		//7000092E //TVAR_afit_pBaseValS[181] /AFIT8_sddd8a_DispTH_Low_Bin [7:0],   AFIT8_sddd8a_DispTH_High_Bin [15:8]                          /
	{0x0F12, 0x585A, WORD_LEN},		//70000930 //TVAR_afit_pBaseValS[182] /AFIT8_sddd8a_iDenThreshLow_Bin [7:0],   AFIT8_sddd8a_iDenThreshHigh_Bin [15:8]                    /
	{0x0F12, 0x0028, WORD_LEN},		//70000932 //TVAR_afit_pBaseValS[183] /AFIT8_Demosaicing_iEdgeDesat_Bin[7:0],   AFIT8_Demosaicing_iEdgeDesatThrLow_Bin [15:8]            /
	{0x0F12, 0x030A, WORD_LEN},		//70000934 //TVAR_afit_pBaseValS[184] /AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin [7:0],  AFIT8_Demosaicing_iEdgeDesatLimit_Bin [15:8]      /
	{0x0F12, 0x0000, WORD_LEN},		//70000936 //TVAR_afit_pBaseValS[185] /AFIT8_Demosaicing_iDemSharpenLow_Bin [7:0],  AFIT8_Demosaicing_iDemSharpenHigh_Bin [15:8]         /
	{0x0F12, 0x141E, WORD_LEN},		//70000938 //TVAR_afit_pBaseValS[186] /AFIT8_Demosaicing_iDemSharpThresh_Bin [7:0],  AFIT8_Demosaicing_iDemShLowLimit_Bin [15:8]         /
	{0x0F12, 0xFF07, WORD_LEN},		//7000093A //TVAR_afit_pBaseValS[187] /AFIT8_Demosaicing_iDespeckleForDemsharp_Bin [7:0],  AFIT8_Demosaicing_iDemBlurLow_Bin [15:8]      /
	{0x0F12, 0x0432, WORD_LEN},		//7000093C //TVAR_afit_pBaseValS[188] /AFIT8_Demosaicing_iDemBlurHigh_Bin [7:0],  AFIT8_Demosaicing_iDemBlurRange_Bin [15:8]             /
	{0x0F12, 0x0000, WORD_LEN},		//7000093E //TVAR_afit_pBaseValS[189] /AFIT8_Sharpening_iLowSharpPower_Bin [7:0],  AFIT8_Sharpening_iHighSharpPower_Bin [15:8]           /
	{0x0F12, 0x0F0F, WORD_LEN},		//70000940 //TVAR_afit_pBaseValS[190] /AFIT8_Sharpening_iLowShDenoise_Bin [7:0],  AFIT8_Sharpening_iHighShDenoise_Bin [15:8]             /
	{0x0F12, 0x0440, WORD_LEN},		//70000942 //TVAR_afit_pBaseValS[191] /AFIT8_Sharpening_iReduceEdgeMinMult_Bin [7:0],  AFIT8_Sharpening_iReduceEdgeSlope_Bin [15:8]      /
	{0x0F12, 0x0302, WORD_LEN},		//70000944 //TVAR_afit_pBaseValS[192] /AFIT8_demsharpmix1_iWideFiltReduce_Bin [7:0],  AFIT8_demsharpmix1_iNarrFiltReduce_Bin [15:8]      /
	{0x0F12, 0x1E1E, WORD_LEN},		//70000946 //TVAR_afit_pBaseValS[193] /AFIT8_sddd8a_iClustThresh_H_sBin[7:0],   AFIT8_sddd8a_iClustThresh_C_sBin [15:8]                  /
	{0x0F12, 0x0101, WORD_LEN},		//70000948 //TVAR_afit_pBaseValS[194] /AFIT8_sddd8a_iClustMulT_H_sBin [7:0],   AFIT8_sddd8a_iClustMulT_C_sBin [15:8]                     /
	{0x0F12, 0x3C01, WORD_LEN},		//7000094A //TVAR_afit_pBaseValS[195] /AFIT8_sddd8a_nClustLevel_H_sBin [7:0],   AFIT8_sddd8a_DispTH_Low_sBin [15:8]                      /
	{0x0F12, 0x5A3A, WORD_LEN},		//7000094C //TVAR_afit_pBaseValS[196] /AFIT8_sddd8a_DispTH_High_sBin [7:0],   AFIT8_sddd8a_iDenThreshLow_sBin [15:8]                     /
	{0x0F12, 0x2858, WORD_LEN},		//7000094E //TVAR_afit_pBaseValS[197] /AFIT8_sddd8a_iDenThreshHigh_sBin[7:0],   AFIT8_Demosaicing_iEdgeDesat_sBin [15:8]                 /
	{0x0F12, 0x0A00, WORD_LEN},		//70000950 //TVAR_afit_pBaseValS[198] /AFIT8_Demosaicing_iEdgeDesatThrLow_sBin [7:0],  AFIT8_Demosaicing_iEdgeDesatThrHigh_sBin [15:8]   /
	{0x0F12, 0x0003, WORD_LEN},		//70000952 //TVAR_afit_pBaseValS[199] /AFIT8_Demosaicing_iEdgeDesatLimit_sBin [7:0],  AFIT8_Demosaicing_iDemSharpenLow_sBin [15:8]       /
	{0x0F12, 0x1E00, WORD_LEN},		//70000954 //TVAR_afit_pBaseValS[200] /AFIT8_Demosaicing_iDemSharpenHigh_sBin [7:0],  AFIT8_Demosaicing_iDemSharpThresh_sBin [15:8]      /
	{0x0F12, 0x0714, WORD_LEN},		//70000956 //TVAR_afit_pBaseValS[201] /AFIT8_Demosaicing_iDemShLowLimit_sBin [7:0],  AFIT8_Demosaicing_iDespeckleForDemsharp_sBin [15:8] /
	{0x0F12, 0x32FF, WORD_LEN},		//70000958 //TVAR_afit_pBaseValS[202] /AFIT8_Demosaicing_iDemBlurLow_sBin [7:0],  AFIT8_Demosaicing_iDemBlurHigh_sBin [15:8]             /
	{0x0F12, 0x0004, WORD_LEN},		//7000095A //TVAR_afit_pBaseValS[203] /AFIT8_Demosaicing_iDemBlurRange_sBin [7:0],  AFIT8_Sharpening_iLowSharpPower_sBin [15:8]          /
	{0x0F12, 0x0F00, WORD_LEN},		//7000095C //TVAR_afit_pBaseValS[204] /AFIT8_Sharpening_iHighSharpPower_sBin [7:0],  AFIT8_Sharpening_iLowShDenoise_sBin [15:8]          /
	{0x0F12, 0x400F, WORD_LEN},		//7000095E //TVAR_afit_pBaseValS[205] /AFIT8_Sharpening_iHighShDenoise_sBin [7:0],  AFIT8_Sharpening_iReduceEdgeMinMult_sBin [15:8]      /
	{0x0F12, 0x0204, WORD_LEN},		//70000960 //TVAR_afit_pBaseValS[206] /AFIT8_Sharpening_iReduceEdgeSlope_sBin [7:0],  AFIT8_demsharpmix1_iWideFiltReduce_sBin [15:8]     /
	{0x0F12, 0x0003, WORD_LEN},		//70000962 //TVAR_afit_pBaseValS[207] /AFIT8_demsharpmix1_iNarrFiltReduce_sBin [7:0]                                                     /
	{0x0F12, 0x0000, WORD_LEN},		//70000964 //TVAR_afit_pBaseValS[208] /AFIT16_BRIGHTNESS                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000966 //TVAR_afit_pBaseValS[209] /AFIT16_CONTRAST                                                                                   /
	{0x0F12, 0x0000, WORD_LEN},		//70000968 //TVAR_afit_pBaseValS[210] /AFIT16_SATURATION                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//7000096A //TVAR_afit_pBaseValS[211] /AFIT16_SHARP_BLUR                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//7000096C //TVAR_afit_pBaseValS[212] /AFIT16_GLAMOUR                                                                                    /
	{0x0F12, 0x00C1, WORD_LEN},		//7000096E //TVAR_afit_pBaseValS[213] /AFIT16_sddd8a_edge_high                                                                           /
	{0x0F12, 0x03FF, WORD_LEN},		//70000970 //TVAR_afit_pBaseValS[214] /AFIT16_Demosaicing_iSatVal                                                                        /
	{0x0F12, 0x009C, WORD_LEN},		//70000972 //TVAR_afit_pBaseValS[215] /AFIT16_Sharpening_iReduceEdgeThresh                                                               /
	{0x0F12, 0x0251, WORD_LEN},		//70000974 //TVAR_afit_pBaseValS[216] /AFIT16_demsharpmix1_iRGBOffset                                                                    /
	{0x0F12, 0x03FF, WORD_LEN},		//70000976 //TVAR_afit_pBaseValS[217] /AFIT16_demsharpmix1_iDemClamp                                                                     /
	{0x0F12, 0x000C, WORD_LEN},		//70000978 //TVAR_afit_pBaseValS[218] /AFIT16_demsharpmix1_iLowThreshold                                                                 /
	{0x0F12, 0x0010, WORD_LEN},		//7000097A //TVAR_afit_pBaseValS[219] /AFIT16_demsharpmix1_iHighThreshold                                                                /
	{0x0F12, 0x012C, WORD_LEN},		//7000097C //TVAR_afit_pBaseValS[220] /AFIT16_demsharpmix1_iLowBright                                                                    /
	{0x0F12, 0x03E8, WORD_LEN},		//7000097E //TVAR_afit_pBaseValS[221] /AFIT16_demsharpmix1_iHighBright                                                                   /
	{0x0F12, 0x0046, WORD_LEN},		//70000980 //TVAR_afit_pBaseValS[222] /AFIT16_demsharpmix1_iLowSat                                                                       /
	{0x0F12, 0x005A, WORD_LEN},		//70000982 //TVAR_afit_pBaseValS[223] /AFIT16_demsharpmix1_iHighSat                                                                      /
	{0x0F12, 0x0070, WORD_LEN},		//70000984 //TVAR_afit_pBaseValS[224] /AFIT16_demsharpmix1_iTune                                                                         /
	{0x0F12, 0x0000, WORD_LEN},		//70000986 //TVAR_afit_pBaseValS[225] /AFIT16_demsharpmix1_iHystThLow                                                                    /
	{0x0F12, 0x0000, WORD_LEN},		//70000988 //TVAR_afit_pBaseValS[226] /AFIT16_demsharpmix1_iHystThHigh                                                                   /
	{0x0F12, 0x0226, WORD_LEN},		//7000098A //TVAR_afit_pBaseValS[227] /AFIT16_demsharpmix1_iHystCenter                                                                   /
	{0x0F12, 0x001E, WORD_LEN},		//7000098C //TVAR_afit_pBaseValS[228] /AFIT16_YUV422_DENOISE_iUVLowThresh                                                                /
	{0x0F12, 0x001E, WORD_LEN},		//7000098E //TVAR_afit_pBaseValS[229] /AFIT16_YUV422_DENOISE_iUVHighThresh                                                               /
	{0x0F12, 0x0000, WORD_LEN},		//70000990 //TVAR_afit_pBaseValS[230] /AFIT16_YUV422_DENOISE_iYLowThresh                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000992 //TVAR_afit_pBaseValS[231] /AFIT16_YUV422_DENOISE_iYHighThresh                                                                /
	{0x0F12, 0x004E, WORD_LEN},		//70000994 //TVAR_afit_pBaseValS[232] /AFIT16_Sharpening_iLowSharpClamp                                                                  /
	{0x0F12, 0x0000, WORD_LEN},		//70000996 //TVAR_afit_pBaseValS[233] /AFIT16_Sharpening_iHighSharpClamp                                                                 /
	{0x0F12, 0x003C, WORD_LEN},		//70000998 //TVAR_afit_pBaseValS[234] /AFIT16_Sharpening_iLowSharpClamp_Bin                                                              /
	{0x0F12, 0x001E, WORD_LEN},		//7000099A //TVAR_afit_pBaseValS[235] /AFIT16_Sharpening_iHighSharpClamp_Bin                                                             /
	{0x0F12, 0x003C, WORD_LEN},		//7000099C //TVAR_afit_pBaseValS[236] /AFIT16_Sharpening_iLowSharpClamp_sBin                                                             /
	{0x0F12, 0x001E, WORD_LEN},		//7000099E //TVAR_afit_pBaseValS[237] /AFIT16_Sharpening_iHighSharpClamp_sBin                                                            /
	{0x0F12, 0x0A24, WORD_LEN},		//700009A0 //TVAR_afit_pBaseValS[238] /AFIT8_sddd8a_edge_low [7:0],   AFIT8_sddd8a_repl_thresh [15:8]                                    /
	{0x0F12, 0x1701, WORD_LEN},		//700009A2 //TVAR_afit_pBaseValS[239] /AFIT8_sddd8a_repl_force [7:0],  AFIT8_sddd8a_sat_level [15:8]                                     /
	{0x0F12, 0x0229, WORD_LEN},		//700009A4 //TVAR_afit_pBaseValS[240] /AFIT8_sddd8a_sat_thr[7:0],  AFIT8_sddd8a_sat_mpl [15:8]                                           /
	{0x0F12, 0x1403, WORD_LEN},		//700009A6 //TVAR_afit_pBaseValS[241] /AFIT8_sddd8a_sat_noise[7:0],  AFIT8_sddd8a_iMaxSlopeAllowed [15:8]                                /
	{0x0F12, 0x0000, WORD_LEN},		//700009A8 //TVAR_afit_pBaseValS[242] /AFIT8_sddd8a_iHotThreshHigh[7:0],  AFIT8_sddd8a_iHotThreshLow [15:8]                              /
	{0x0F12, 0x0000, WORD_LEN},		//700009AA //TVAR_afit_pBaseValS[243] /AFIT8_sddd8a_iColdThreshHigh[7:0],  AFIT8_sddd8a_iColdThreshLow [15:8]                            /
	{0x0F12, 0x0906, WORD_LEN},		//700009AC //TVAR_afit_pBaseValS[244] /AFIT8_sddd8a_AddNoisePower1[7:0],  AFIT8_sddd8a_AddNoisePower2 [15:8]                             /
	{0x0F12, 0x00FF, WORD_LEN},		//700009AE //TVAR_afit_pBaseValS[245] /AFIT8_sddd8a_iSatSat[7:0],   AFIT8_sddd8a_iRadialTune [15:8]                                      /
	{0x0F12, 0x045A, WORD_LEN},		//700009B0 //TVAR_afit_pBaseValS[246] /AFIT8_sddd8a_iRadialLimit [7:0],   AFIT8_sddd8a_iRadialPower [15:8]                               /
	{0x0F12, 0x1414, WORD_LEN},		//700009B2 //TVAR_afit_pBaseValS[247] /AFIT8_sddd8a_iLowMaxSlopeAllowed [7:0],  AFIT8_sddd8a_iHighMaxSlopeAllowed [15:8]                 /
	{0x0F12, 0x0301, WORD_LEN},		//700009B4 //TVAR_afit_pBaseValS[248] /AFIT8_sddd8a_iLowSlopeThresh[7:0],   AFIT8_sddd8a_iHighSlopeThresh [15:8]                         /
	{0x0F12, 0xFF07, WORD_LEN},		//700009B6 //TVAR_afit_pBaseValS[249] /AFIT8_sddd8a_iSquaresRounding [7:0],   AFIT8_Demosaicing_iCentGrad [15:8]                         /
	{0x0F12, 0x081E, WORD_LEN},		//700009B8 //TVAR_afit_pBaseValS[250] /AFIT8_Demosaicing_iMonochrom [7:0],   AFIT8_Demosaicing_iDecisionThresh [15:8]                    /
	{0x0F12, 0x0A14, WORD_LEN},		//700009BA //TVAR_afit_pBaseValS[251] /AFIT8_Demosaicing_iDesatThresh [7:0],   AFIT8_Demosaicing_iEnhThresh [15:8]                       /
	{0x0F12, 0x0F0F, WORD_LEN},		//700009BC //TVAR_afit_pBaseValS[252] /AFIT8_Demosaicing_iGRDenoiseVal [7:0],   AFIT8_Demosaicing_iGBDenoiseVal [15:8]                   /
	{0x0F12, 0x0A00, WORD_LEN},		//700009BE //TVAR_afit_pBaseValS[253] /AFIT8_Demosaicing_iNearGrayDesat[7:0],   AFIT8_Demosaicing_iDFD_ReduceCoeff [15:8]                /
	{0x0F12, 0x0090, WORD_LEN},		//700009C0 //TVAR_afit_pBaseValS[254] /AFIT8_Sharpening_iMSharpen [7:0],   AFIT8_Sharpening_iMShThresh [15:8]                            /
	{0x0F12, 0x000A, WORD_LEN},		//700009C2 //TVAR_afit_pBaseValS[255] /AFIT8_Sharpening_iWSharpen [7:0],   AFIT8_Sharpening_iWShThresh [15:8]                            /
	{0x0F12, 0x0002, WORD_LEN},		//700009C4 //TVAR_afit_pBaseValS[256] /AFIT8_Sharpening_nSharpWidth [7:0],   AFIT8_Sharpening_iReduceNegative [15:8]                     /
	{0x0F12, 0x00FF, WORD_LEN},		//700009C6 //TVAR_afit_pBaseValS[257] /AFIT8_Sharpening_iShDespeckle [7:0],  AFIT8_demsharpmix1_iRGBMultiplier [15:8]                    /
	{0x0F12, 0x1102, WORD_LEN},		//700009C8 //TVAR_afit_pBaseValS[258] /AFIT8_demsharpmix1_iFilterPower [7:0],  AFIT8_demsharpmix1_iBCoeff [15:8]                         /
	{0x0F12, 0x001B, WORD_LEN},		//700009CA //TVAR_afit_pBaseValS[259] /AFIT8_demsharpmix1_iGCoeff [7:0],   AFIT8_demsharpmix1_iWideMult [15:8]                           /
	{0x0F12, 0x0900, WORD_LEN},		//700009CC //TVAR_afit_pBaseValS[260] /AFIT8_demsharpmix1_iNarrMult [7:0],   AFIT8_demsharpmix1_iHystFalloff [15:8]                      /
	{0x0F12, 0x0600, WORD_LEN},		//700009CE //TVAR_afit_pBaseValS[261] /AFIT8_demsharpmix1_iHystMinMult [7:0],   AFIT8_demsharpmix1_iHystWidth [15:8]                     /
	{0x0F12, 0x0504, WORD_LEN},		//700009D0 //TVAR_afit_pBaseValS[262] /AFIT8_demsharpmix1_iHystFallLow [7:0],   AFIT8_demsharpmix1_iHystFallHigh [15:8]                  /
	{0x0F12, 0x0306, WORD_LEN},		//700009D2 //TVAR_afit_pBaseValS[263] /AFIT8_demsharpmix1_iHystTune [7:0],  * AFIT8_YUV422_DENOISE_iUVSupport [15:8]                     /
	{0x0F12, 0x4602, WORD_LEN},		//700009D4 //TVAR_afit_pBaseValS[264] /AFIT8_YUV422_DENOISE_iYSupport [7:0],   AFIT8_byr_cgras_iShadingPower [15:8]                      /
	{0x0F12, 0x0880, WORD_LEN},		//700009D6 //TVAR_afit_pBaseValS[265] /AFIT8_RGBGamma2_iLinearity [7:0],  AFIT8_RGBGamma2_iDarkReduce [15:8]                             /
	{0x0F12, 0x0080, WORD_LEN},		//700009D8 //TVAR_afit_pBaseValS[266] /AFIT8_ccm_oscar_iSaturation[7:0],   AFIT8_RGB2YUV_iYOffset [15:8]                                 /
	{0x0F12, 0x0080, WORD_LEN},		//700009DA //TVAR_afit_pBaseValS[267] /AFIT8_RGB2YUV_iRGBGain [7:0],   AFIT8_RGB2YUV_iSaturation [15:8]                                  /
	{0x0F12, 0x0101, WORD_LEN},		//700009DC //TVAR_afit_pBaseValS[268] /AFIT8_sddd8a_iClustThresh_H [7:0],  AFIT8_sddd8a_iClustThresh_C [15:8]                            /
	{0x0F12, 0x0707, WORD_LEN},		//700009DE //TVAR_afit_pBaseValS[269] /AFIT8_sddd8a_iClustMulT_H [7:0],   AFIT8_sddd8a_iClustMulT_C [15:8]                               /
	{0x0F12, 0x1E01, WORD_LEN},		//700009E0 //TVAR_afit_pBaseValS[270] /AFIT8_sddd8a_nClustLevel_H [7:0],   AFIT8_sddd8a_DispTH_Low [15:8]                                /
	{0x0F12, 0x3C1E, WORD_LEN},		//700009E2 //TVAR_afit_pBaseValS[271] /AFIT8_sddd8a_DispTH_High [7:0],   AFIT8_sddd8a_iDenThreshLow [15:8]                               /
	{0x0F12, 0x5028, WORD_LEN},		//700009E4 //TVAR_afit_pBaseValS[272] /AFIT8_sddd8a_iDenThreshHigh[7:0],   AFIT8_Demosaicing_iEdgeDesat [15:8]                           /
	{0x0F12, 0x0500, WORD_LEN},		//700009E6 //TVAR_afit_pBaseValS[273] /AFIT8_Demosaicing_iEdgeDesatThrLow [7:0],   AFIT8_Demosaicing_iEdgeDesatThrHigh [15:8]            /
	{0x0F12, 0x1A04, WORD_LEN},		//700009E8 //TVAR_afit_pBaseValS[274] /AFIT8_Demosaicing_iEdgeDesatLimit[7:0],  AFIT8_Demosaicing_iDemSharpenLow [15:8]                  /
	{0x0F12, 0x280A, WORD_LEN},		//700009EA //TVAR_afit_pBaseValS[275] /AFIT8_Demosaicing_iDemSharpenHigh[7:0],   AFIT8_Demosaicing_iDemSharpThresh [15:8]                /
	{0x0F12, 0x080C, WORD_LEN},		//700009EC //TVAR_afit_pBaseValS[276] /AFIT8_Demosaicing_iDemShLowLimit [7:0],   AFIT8_Demosaicing_iDespeckleForDemsharp [15:8]          /
	{0x0F12, 0x1414, WORD_LEN},		//700009EE //TVAR_afit_pBaseValS[277] /AFIT8_Demosaicing_iDemBlurLow[7:0],   AFIT8_Demosaicing_iDemBlurHigh [15:8]                       /
	{0x0F12, 0x6A03, WORD_LEN},		//700009F0 //TVAR_afit_pBaseValS[278] /AFIT8_Demosaicing_iDemBlurRange[7:0],   AFIT8_Sharpening_iLowSharpPower [15:8]                    /
	{0x0F12, 0x121E, WORD_LEN},		//700009F2 //TVAR_afit_pBaseValS[279] /AFIT8_Sharpening_iHighSharpPower[7:0],   AFIT8_Sharpening_iLowShDenoise [15:8]                    /
	{0x0F12, 0x4012, WORD_LEN},		//700009F4 //TVAR_afit_pBaseValS[280] /AFIT8_Sharpening_iHighShDenoise [7:0],   AFIT8_Sharpening_iReduceEdgeMinMult [15:8]               /
	{0x0F12, 0x0604, WORD_LEN},		//700009F6 //TVAR_afit_pBaseValS[281] /AFIT8_Sharpening_iReduceEdgeSlope [7:0],  AFIT8_demsharpmix1_iWideFiltReduce [15:8]               /
	{0x0F12, 0x2806, WORD_LEN},		//700009F8 //TVAR_afit_pBaseValS[282] /AFIT8_demsharpmix1_iNarrFiltReduce [7:0],  AFIT8_sddd8a_iClustThresh_H_Bin [15:8]                 /
	{0x0F12, 0x0128, WORD_LEN},		//700009FA //TVAR_afit_pBaseValS[283] /AFIT8_sddd8a_iClustThresh_C_Bin [7:0],   AFIT8_sddd8a_iClustMulT_H_Bin [15:8]                     /
	{0x0F12, 0x0101, WORD_LEN},		//700009FC //TVAR_afit_pBaseValS[284] /AFIT8_sddd8a_iClustMulT_C_Bin [7:0],   AFIT8_sddd8a_nClustLevel_H_Bin [15:8]                      /
	{0x0F12, 0x2224, WORD_LEN},		//700009FE //TVAR_afit_pBaseValS[285] /AFIT8_sddd8a_DispTH_Low_Bin [7:0],   AFIT8_sddd8a_DispTH_High_Bin [15:8]                          /
	{0x0F12, 0x3236, WORD_LEN},		//70000A00 //TVAR_afit_pBaseValS[286] /AFIT8_sddd8a_iDenThreshLow_Bin [7:0],   AFIT8_sddd8a_iDenThreshHigh_Bin [15:8]                    /
	{0x0F12, 0x0028, WORD_LEN},		//70000A02 //TVAR_afit_pBaseValS[287] /AFIT8_Demosaicing_iEdgeDesat_Bin[7:0],   AFIT8_Demosaicing_iEdgeDesatThrLow_Bin [15:8]            /
	{0x0F12, 0x030A, WORD_LEN},		//70000A04 //TVAR_afit_pBaseValS[288] /AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin [7:0],  AFIT8_Demosaicing_iEdgeDesatLimit_Bin [15:8]      /
	{0x0F12, 0x0410, WORD_LEN},		//70000A06 //TVAR_afit_pBaseValS[289] /AFIT8_Demosaicing_iDemSharpenLow_Bin [7:0],  AFIT8_Demosaicing_iDemSharpenHigh_Bin [15:8]         /
	{0x0F12, 0x141E, WORD_LEN},		//70000A08 //TVAR_afit_pBaseValS[290] /AFIT8_Demosaicing_iDemSharpThresh_Bin [7:0],  AFIT8_Demosaicing_iDemShLowLimit_Bin [15:8]         /
	{0x0F12, 0xFF07, WORD_LEN},		//70000A0A //TVAR_afit_pBaseValS[291] /AFIT8_Demosaicing_iDespeckleForDemsharp_Bin [7:0],  AFIT8_Demosaicing_iDemBlurLow_Bin [15:8]      /
	{0x0F12, 0x0432, WORD_LEN},		//70000A0C //TVAR_afit_pBaseValS[292] /AFIT8_Demosaicing_iDemBlurHigh_Bin [7:0],  AFIT8_Demosaicing_iDemBlurRange_Bin [15:8]             /
	{0x0F12, 0x4050, WORD_LEN},		//70000A0E //TVAR_afit_pBaseValS[293] /AFIT8_Sharpening_iLowSharpPower_Bin [7:0],  AFIT8_Sharpening_iHighSharpPower_Bin [15:8]           /
	{0x0F12, 0x0F0F, WORD_LEN},		//70000A10 //TVAR_afit_pBaseValS[294] /AFIT8_Sharpening_iLowShDenoise_Bin [7:0],  AFIT8_Sharpening_iHighShDenoise_Bin [15:8]             /
	{0x0F12, 0x0440, WORD_LEN},		//70000A12 //TVAR_afit_pBaseValS[295] /AFIT8_Sharpening_iReduceEdgeMinMult_Bin [7:0],  AFIT8_Sharpening_iReduceEdgeSlope_Bin [15:8]      /
	{0x0F12, 0x0302, WORD_LEN},		//70000A14 //TVAR_afit_pBaseValS[296] /AFIT8_demsharpmix1_iWideFiltReduce_Bin [7:0],  AFIT8_demsharpmix1_iNarrFiltReduce_Bin [15:8]      /
	{0x0F12, 0x2828, WORD_LEN},		//70000A16 //TVAR_afit_pBaseValS[297] /AFIT8_sddd8a_iClustThresh_H_sBin[7:0],   AFIT8_sddd8a_iClustThresh_C_sBin [15:8]                  /
	{0x0F12, 0x0101, WORD_LEN},		//70000A18 //TVAR_afit_pBaseValS[298] /AFIT8_sddd8a_iClustMulT_H_sBin [7:0],   AFIT8_sddd8a_iClustMulT_C_sBin [15:8]                     /
	{0x0F12, 0x2401, WORD_LEN},		//70000A1A //TVAR_afit_pBaseValS[299] /AFIT8_sddd8a_nClustLevel_H_sBin [7:0],   AFIT8_sddd8a_DispTH_Low_sBin [15:8]                      /
	{0x0F12, 0x3622, WORD_LEN},		//70000A1C //TVAR_afit_pBaseValS[300] /AFIT8_sddd8a_DispTH_High_sBin [7:0],   AFIT8_sddd8a_iDenThreshLow_sBin [15:8]                     /
	{0x0F12, 0x2832, WORD_LEN},		//70000A1E //TVAR_afit_pBaseValS[301] /AFIT8_sddd8a_iDenThreshHigh_sBin[7:0],   AFIT8_Demosaicing_iEdgeDesat_sBin [15:8]                 /
	{0x0F12, 0x0A00, WORD_LEN},		//70000A20 //TVAR_afit_pBaseValS[302] /AFIT8_Demosaicing_iEdgeDesatThrLow_sBin [7:0],  AFIT8_Demosaicing_iEdgeDesatThrHigh_sBin [15:8]   /
	{0x0F12, 0x1003, WORD_LEN},		//70000A22 //TVAR_afit_pBaseValS[303] /AFIT8_Demosaicing_iEdgeDesatLimit_sBin [7:0],  AFIT8_Demosaicing_iDemSharpenLow_sBin [15:8]       /
	{0x0F12, 0x1E04, WORD_LEN},		//70000A24 //TVAR_afit_pBaseValS[304] /AFIT8_Demosaicing_iDemSharpenHigh_sBin [7:0],  AFIT8_Demosaicing_iDemSharpThresh_sBin [15:8]      /
	{0x0F12, 0x0714, WORD_LEN},		//70000A26 //TVAR_afit_pBaseValS[305] /AFIT8_Demosaicing_iDemShLowLimit_sBin [7:0],  AFIT8_Demosaicing_iDespeckleForDemsharp_sBin [15:8] /
	{0x0F12, 0x32FF, WORD_LEN},		//70000A28 //TVAR_afit_pBaseValS[306] /AFIT8_Demosaicing_iDemBlurLow_sBin [7:0],  AFIT8_Demosaicing_iDemBlurHigh_sBin [15:8]             /
	{0x0F12, 0x5004, WORD_LEN},		//70000A2A //TVAR_afit_pBaseValS[307] /AFIT8_Demosaicing_iDemBlurRange_sBin [7:0],  AFIT8_Sharpening_iLowSharpPower_sBin [15:8]          /
	{0x0F12, 0x0F40, WORD_LEN},		//70000A2C //TVAR_afit_pBaseValS[308] /AFIT8_Sharpening_iHighSharpPower_sBin [7:0],  AFIT8_Sharpening_iLowShDenoise_sBin [15:8]          /
	{0x0F12, 0x400F, WORD_LEN},		//70000A2E //TVAR_afit_pBaseValS[309] /AFIT8_Sharpening_iHighShDenoise_sBin [7:0],  AFIT8_Sharpening_iReduceEdgeMinMult_sBin [15:8]      /
	{0x0F12, 0x0204, WORD_LEN},		//70000A30 //TVAR_afit_pBaseValS[310] /AFIT8_Sharpening_iReduceEdgeSlope_sBin [7:0],  AFIT8_demsharpmix1_iWideFiltReduce_sBin [15:8]     /
	{0x0F12, 0x0003, WORD_LEN},		//70000A32 //TVAR_afit_pBaseValS[311] /AFIT8_demsharpmix1_iNarrFiltReduce_sBin [7:0]                                                     /
	{0x0F12, 0x0000, WORD_LEN},		//70000A34 //TVAR_afit_pBaseValS[312] /AFIT16_BRIGHTNESS                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000A36 //TVAR_afit_pBaseValS[313] /AFIT16_CONTRAST                                                                                   /
	{0x0F12, 0x0000, WORD_LEN},		//70000A38 //TVAR_afit_pBaseValS[314] /AFIT16_SATURATION                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000A3A //TVAR_afit_pBaseValS[315] /AFIT16_SHARP_BLUR                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000A3C //TVAR_afit_pBaseValS[316] /AFIT16_GLAMOUR                                                                                    /
	{0x0F12, 0x00C1, WORD_LEN},		//70000A3E //TVAR_afit_pBaseValS[317] /AFIT16_sddd8a_edge_high                                                                           /
	{0x0F12, 0x03FF, WORD_LEN},		//70000A40 //TVAR_afit_pBaseValS[318] /AFIT16_Demosaicing_iSatVal                                                                        /
	{0x0F12, 0x009C, WORD_LEN},		//70000A42 //TVAR_afit_pBaseValS[319] /AFIT16_Sharpening_iReduceEdgeThresh                                                               /
	{0x0F12, 0x0251, WORD_LEN},		//70000A44 //TVAR_afit_pBaseValS[320] /AFIT16_demsharpmix1_iRGBOffset                                                                    /
	{0x0F12, 0x03FF, WORD_LEN},		//70000A46 //TVAR_afit_pBaseValS[321] /AFIT16_demsharpmix1_iDemClamp                                                                     /
	{0x0F12, 0x000C, WORD_LEN},		//70000A48 //TVAR_afit_pBaseValS[322] /AFIT16_demsharpmix1_iLowThreshold                                                                 /
	{0x0F12, 0x0010, WORD_LEN},		//70000A4A //TVAR_afit_pBaseValS[323] /AFIT16_demsharpmix1_iHighThreshold                                                                /
	{0x0F12, 0x00C8, WORD_LEN},		//70000A4C //TVAR_afit_pBaseValS[324] /AFIT16_demsharpmix1_iLowBright                                                                    /
	{0x0F12, 0x03E8, WORD_LEN},		//70000A4E //TVAR_afit_pBaseValS[325] /AFIT16_demsharpmix1_iHighBright                                                                   /
	{0x0F12, 0x0046, WORD_LEN},		//70000A50 //TVAR_afit_pBaseValS[326] /AFIT16_demsharpmix1_iLowSat                                                                       /
	{0x0F12, 0x0050, WORD_LEN},		//70000A52 //TVAR_afit_pBaseValS[327] /AFIT16_demsharpmix1_iHighSat                                                                      /
	{0x0F12, 0x0070, WORD_LEN},		//70000A54 //TVAR_afit_pBaseValS[328] /AFIT16_demsharpmix1_iTune                                                                         /
	{0x0F12, 0x0000, WORD_LEN},		//70000A56 //TVAR_afit_pBaseValS[329] /AFIT16_demsharpmix1_iHystThLow                                                                    /
	{0x0F12, 0x0000, WORD_LEN},		//70000A58 //TVAR_afit_pBaseValS[330] /AFIT16_demsharpmix1_iHystThHigh                                                                   /
	{0x0F12, 0x0226, WORD_LEN},		//70000A5A //TVAR_afit_pBaseValS[331] /AFIT16_demsharpmix1_iHystCenter                                                                   /
	{0x0F12, 0x0014, WORD_LEN},		//70000A5C //TVAR_afit_pBaseValS[332] /AFIT16_YUV422_DENOISE_iUVLowThresh                                                                /
	{0x0F12, 0x0014, WORD_LEN},		//70000A5E //TVAR_afit_pBaseValS[333] /AFIT16_YUV422_DENOISE_iUVHighThresh                                                               /
	{0x0F12, 0x0000, WORD_LEN},		//70000A60 //TVAR_afit_pBaseValS[334] /AFIT16_YUV422_DENOISE_iYLowThresh                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000A62 //TVAR_afit_pBaseValS[335] /AFIT16_YUV422_DENOISE_iYHighThresh                                                                /
	{0x0F12, 0x004E, WORD_LEN},		//70000A64 //TVAR_afit_pBaseValS[336] /AFIT16_Sharpening_iLowSharpClamp                                                                  /
	{0x0F12, 0x0000, WORD_LEN},		//70000A66 //TVAR_afit_pBaseValS[337] /AFIT16_Sharpening_iHighSharpClamp                                                                 /
	{0x0F12, 0x002D, WORD_LEN},		//70000A68 //TVAR_afit_pBaseValS[338] /AFIT16_Sharpening_iLowSharpClamp_Bin                                                              /
	{0x0F12, 0x0019, WORD_LEN},		//70000A6A //TVAR_afit_pBaseValS[339] /AFIT16_Sharpening_iHighSharpClamp_Bin                                                             /
	{0x0F12, 0x002D, WORD_LEN},		//70000A6C //TVAR_afit_pBaseValS[340] /AFIT16_Sharpening_iLowSharpClamp_sBin                                                             /
	{0x0F12, 0x0019, WORD_LEN},		//70000A6E //TVAR_afit_pBaseValS[341] /AFIT16_Sharpening_iHighSharpClamp_sBin                                                            /
	{0x0F12, 0x0A24, WORD_LEN},		//70000A70 //TVAR_afit_pBaseValS[342] /AFIT8_sddd8a_edge_low [7:0],   AFIT8_sddd8a_repl_thresh [15:8]                                    /
	{0x0F12, 0x1701, WORD_LEN},		//70000A72 //TVAR_afit_pBaseValS[343] /AFIT8_sddd8a_repl_force [7:0],  AFIT8_sddd8a_sat_level [15:8]                                     /
	{0x0F12, 0x0229, WORD_LEN},		//70000A74 //TVAR_afit_pBaseValS[344] /AFIT8_sddd8a_sat_thr[7:0],  AFIT8_sddd8a_sat_mpl [15:8]                                           /
	{0x0F12, 0x1403, WORD_LEN},		//70000A76 //TVAR_afit_pBaseValS[345] /AFIT8_sddd8a_sat_noise[7:0],  AFIT8_sddd8a_iMaxSlopeAllowed [15:8]                                /
	{0x0F12, 0x0000, WORD_LEN},		//70000A78 //TVAR_afit_pBaseValS[346] /AFIT8_sddd8a_iHotThreshHigh[7:0],  AFIT8_sddd8a_iHotThreshLow [15:8]                              /
	{0x0F12, 0x0000, WORD_LEN},		//70000A7A //TVAR_afit_pBaseValS[347] /AFIT8_sddd8a_iColdThreshHigh[7:0],  AFIT8_sddd8a_iColdThreshLow [15:8]                            /
	{0x0F12, 0x0906, WORD_LEN},		//70000A7C //TVAR_afit_pBaseValS[348] /AFIT8_sddd8a_AddNoisePower1[7:0],  AFIT8_sddd8a_AddNoisePower2 [15:8]                             /
	{0x0F12, 0x00FF, WORD_LEN},		//70000A7E //TVAR_afit_pBaseValS[349] /AFIT8_sddd8a_iSatSat[7:0],   AFIT8_sddd8a_iRadialTune [15:8]                                      /
	{0x0F12, 0x045A, WORD_LEN},		//70000A80 //TVAR_afit_pBaseValS[350] /AFIT8_sddd8a_iRadialLimit [7:0],   AFIT8_sddd8a_iRadialPower [15:8]                               /
	{0x0F12, 0x1414, WORD_LEN},		//70000A82 //TVAR_afit_pBaseValS[351] /AFIT8_sddd8a_iLowMaxSlopeAllowed [7:0],  AFIT8_sddd8a_iHighMaxSlopeAllowed [15:8]                 /
	{0x0F12, 0x0301, WORD_LEN},		//70000A84 //TVAR_afit_pBaseValS[352] /AFIT8_sddd8a_iLowSlopeThresh[7:0],   AFIT8_sddd8a_iHighSlopeThresh [15:8]                         /
	{0x0F12, 0xFF07, WORD_LEN},		//70000A86 //TVAR_afit_pBaseValS[353] /AFIT8_sddd8a_iSquaresRounding [7:0],   AFIT8_Demosaicing_iCentGrad [15:8]                         /
	{0x0F12, 0x081E, WORD_LEN},		//70000A88 //TVAR_afit_pBaseValS[354] /AFIT8_Demosaicing_iMonochrom [7:0],   AFIT8_Demosaicing_iDecisionThresh [15:8]                    /
	{0x0F12, 0x0A14, WORD_LEN},		//70000A8A //TVAR_afit_pBaseValS[355] /AFIT8_Demosaicing_iDesatThresh [7:0],   AFIT8_Demosaicing_iEnhThresh [15:8]                       /
	{0x0F12, 0x0F0F, WORD_LEN},		//70000A8C //TVAR_afit_pBaseValS[356] /AFIT8_Demosaicing_iGRDenoiseVal [7:0],   AFIT8_Demosaicing_iGBDenoiseVal [15:8]                   /
	{0x0F12, 0x0A00, WORD_LEN},		//70000A8E //TVAR_afit_pBaseValS[357] /AFIT8_Demosaicing_iNearGrayDesat[7:0],   AFIT8_Demosaicing_iDFD_ReduceCoeff [15:8]                /
	{0x0F12, 0x0090, WORD_LEN},		//70000A90 //TVAR_afit_pBaseValS[358] /AFIT8_Sharpening_iMSharpen [7:0],   AFIT8_Sharpening_iMShThresh [15:8]                            /
	{0x0F12, 0x000A, WORD_LEN},		//70000A92 //TVAR_afit_pBaseValS[359] /AFIT8_Sharpening_iWSharpen [7:0],   AFIT8_Sharpening_iWShThresh [15:8]                            /
	{0x0F12, 0x0001, WORD_LEN},		//70000A94 //TVAR_afit_pBaseValS[360] /AFIT8_Sharpening_nSharpWidth [7:0],   AFIT8_Sharpening_iReduceNegative [15:8]                     /
	{0x0F12, 0x00FF, WORD_LEN},		//70000A96 //TVAR_afit_pBaseValS[361] /AFIT8_Sharpening_iShDespeckle [7:0],  AFIT8_demsharpmix1_iRGBMultiplier [15:8]                    /
	{0x0F12, 0x1002, WORD_LEN},		//70000A98 //TVAR_afit_pBaseValS[362] /AFIT8_demsharpmix1_iFilterPower [7:0],  AFIT8_demsharpmix1_iBCoeff [15:8]                         /
	{0x0F12, 0x001E, WORD_LEN},		//70000A9A //TVAR_afit_pBaseValS[363] /AFIT8_demsharpmix1_iGCoeff [7:0],   AFIT8_demsharpmix1_iWideMult [15:8]                           /
	{0x0F12, 0x0900, WORD_LEN},		//70000A9C //TVAR_afit_pBaseValS[364] /AFIT8_demsharpmix1_iNarrMult [7:0],   AFIT8_demsharpmix1_iHystFalloff [15:8]                      /
	{0x0F12, 0x0600, WORD_LEN},		//70000A9E //TVAR_afit_pBaseValS[365] /AFIT8_demsharpmix1_iHystMinMult [7:0],   AFIT8_demsharpmix1_iHystWidth [15:8]                     /
	{0x0F12, 0x0504, WORD_LEN},		//70000AA0 //TVAR_afit_pBaseValS[366] /AFIT8_demsharpmix1_iHystFallLow [7:0],   AFIT8_demsharpmix1_iHystFallHigh [15:8]                  /
	{0x0F12, 0x0307, WORD_LEN},		//70000AA2 //TVAR_afit_pBaseValS[367] /AFIT8_demsharpmix1_iHystTune [7:0],  * AFIT8_YUV422_DENOISE_iUVSupport [15:8]                     /
	{0x0F12, 0x5002, WORD_LEN},		//70000AA4 //TVAR_afit_pBaseValS[368] /AFIT8_YUV422_DENOISE_iYSupport [7:0],   AFIT8_byr_cgras_iShadingPower [15:8]                      /
	{0x0F12, 0x0080, WORD_LEN},		//70000AA6 //TVAR_afit_pBaseValS[369] /AFIT8_RGBGamma2_iLinearity [7:0],  AFIT8_RGBGamma2_iDarkReduce [15:8]                             /
	{0x0F12, 0x0080, WORD_LEN},		//70000AA8 //TVAR_afit_pBaseValS[370] /AFIT8_ccm_oscar_iSaturation[7:0],   AFIT8_RGB2YUV_iYOffset [15:8]                                 /
	{0x0F12, 0x0080, WORD_LEN},		//70000AAA //TVAR_afit_pBaseValS[371] /AFIT8_RGB2YUV_iRGBGain [7:0],   AFIT8_RGB2YUV_iSaturation [15:8]                                  /
	{0x0F12, 0x0101, WORD_LEN},		//70000AAC //TVAR_afit_pBaseValS[372] /AFIT8_sddd8a_iClustThresh_H [7:0],  AFIT8_sddd8a_iClustThresh_C [15:8]                            /
	{0x0F12, 0x0707, WORD_LEN},		//70000AAE //TVAR_afit_pBaseValS[373] /AFIT8_sddd8a_iClustMulT_H [7:0],   AFIT8_sddd8a_iClustMulT_C [15:8]                               /
	{0x0F12, 0x1E01, WORD_LEN},		//70000AB0 //TVAR_afit_pBaseValS[374] /AFIT8_sddd8a_nClustLevel_H [7:0],   AFIT8_sddd8a_DispTH_Low [15:8]                                /
	{0x0F12, 0x2A1E, WORD_LEN},		//70000AB2 //TVAR_afit_pBaseValS[375] /AFIT8_sddd8a_DispTH_High [7:0],   AFIT8_sddd8a_iDenThreshLow [15:8]                               /
	{0x0F12, 0x5020, WORD_LEN},		//70000AB4 //TVAR_afit_pBaseValS[376] /AFIT8_sddd8a_iDenThreshHigh[7:0],   AFIT8_Demosaicing_iEdgeDesat [15:8]                           /
	{0x0F12, 0x0500, WORD_LEN},		//70000AB6 //TVAR_afit_pBaseValS[377] /AFIT8_Demosaicing_iEdgeDesatThrLow [7:0],   AFIT8_Demosaicing_iEdgeDesatThrHigh [15:8]            /
	{0x0F12, 0x1A04, WORD_LEN},		//70000AB8 //TVAR_afit_pBaseValS[378] /AFIT8_Demosaicing_iEdgeDesatLimit[7:0],  AFIT8_Demosaicing_iDemSharpenLow [15:8]                  /
	{0x0F12, 0x280A, WORD_LEN},		//70000ABA //TVAR_afit_pBaseValS[379] /AFIT8_Demosaicing_iDemSharpenHigh[7:0],   AFIT8_Demosaicing_iDemSharpThresh [15:8]                /
	{0x0F12, 0x080C, WORD_LEN},		//70000ABC //TVAR_afit_pBaseValS[380] /AFIT8_Demosaicing_iDemShLowLimit [7:0],   AFIT8_Demosaicing_iDespeckleForDemsharp [15:8]          /
	{0x0F12, 0x1414, WORD_LEN},		//70000ABE //TVAR_afit_pBaseValS[381] /AFIT8_Demosaicing_iDemBlurLow[7:0],   AFIT8_Demosaicing_iDemBlurHigh [15:8]                       /
	{0x0F12, 0x6A03, WORD_LEN},		//70000AC0 //TVAR_afit_pBaseValS[382] /AFIT8_Demosaicing_iDemBlurRange[7:0],   AFIT8_Sharpening_iLowSharpPower [15:8]                    /
	{0x0F12, 0x121E, WORD_LEN},		//70000AC2 //TVAR_afit_pBaseValS[383] /AFIT8_Sharpening_iHighSharpPower[7:0],   AFIT8_Sharpening_iLowShDenoise [15:8]                    /
	{0x0F12, 0x4012, WORD_LEN},		//70000AC4 //TVAR_afit_pBaseValS[384] /AFIT8_Sharpening_iHighShDenoise [7:0],   AFIT8_Sharpening_iReduceEdgeMinMult [15:8]               /
	{0x0F12, 0x0604, WORD_LEN},		//70000AC6 //TVAR_afit_pBaseValS[385] /AFIT8_Sharpening_iReduceEdgeSlope [7:0],  AFIT8_demsharpmix1_iWideFiltReduce [15:8]               /
	{0x0F12, 0x3C06, WORD_LEN},		//70000AC8 //TVAR_afit_pBaseValS[386] /AFIT8_demsharpmix1_iNarrFiltReduce [7:0],  AFIT8_sddd8a_iClustThresh_H_Bin [15:8]                 /
	{0x0F12, 0x013C, WORD_LEN},		//70000ACA //TVAR_afit_pBaseValS[387] /AFIT8_sddd8a_iClustThresh_C_Bin [7:0],   AFIT8_sddd8a_iClustMulT_H_Bin [15:8]                     /
	{0x0F12, 0x0101, WORD_LEN},		//70000ACC //TVAR_afit_pBaseValS[388] /AFIT8_sddd8a_iClustMulT_C_Bin [7:0],   AFIT8_sddd8a_nClustLevel_H_Bin [15:8]                      /
	{0x0F12, 0x1C1E, WORD_LEN},		//70000ACE //TVAR_afit_pBaseValS[389] /AFIT8_sddd8a_DispTH_Low_Bin [7:0],   AFIT8_sddd8a_DispTH_High_Bin [15:8]                          /
	{0x0F12, 0x1E22, WORD_LEN},		//70000AD0 //TVAR_afit_pBaseValS[390] /AFIT8_sddd8a_iDenThreshLow_Bin [7:0],   AFIT8_sddd8a_iDenThreshHigh_Bin [15:8]                    /
	{0x0F12, 0x0028, WORD_LEN},		//70000AD2 //TVAR_afit_pBaseValS[391] /AFIT8_Demosaicing_iEdgeDesat_Bin[7:0],   AFIT8_Demosaicing_iEdgeDesatThrLow_Bin [15:8]            /
	{0x0F12, 0x030A, WORD_LEN},		//70000AD4 //TVAR_afit_pBaseValS[392] /AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin [7:0],  AFIT8_Demosaicing_iEdgeDesatLimit_Bin [15:8]      /
	{0x0F12, 0x0214, WORD_LEN},		//70000AD6 //TVAR_afit_pBaseValS[393] /AFIT8_Demosaicing_iDemSharpenLow_Bin [7:0],  AFIT8_Demosaicing_iDemSharpenHigh_Bin [15:8]         /
	{0x0F12, 0x0E14, WORD_LEN},		//70000AD8 //TVAR_afit_pBaseValS[394] /AFIT8_Demosaicing_iDemSharpThresh_Bin [7:0],  AFIT8_Demosaicing_iDemShLowLimit_Bin [15:8]         /
	{0x0F12, 0xFF06, WORD_LEN},		//70000ADA //TVAR_afit_pBaseValS[395] /AFIT8_Demosaicing_iDespeckleForDemsharp_Bin [7:0],  AFIT8_Demosaicing_iDemBlurLow_Bin [15:8]      /
	{0x0F12, 0x0432, WORD_LEN},		//70000ADC //TVAR_afit_pBaseValS[396] /AFIT8_Demosaicing_iDemBlurHigh_Bin [7:0],  AFIT8_Demosaicing_iDemBlurRange_Bin [15:8]             /
	{0x0F12, 0x4052, WORD_LEN},		//70000ADE //TVAR_afit_pBaseValS[397] /AFIT8_Sharpening_iLowSharpPower_Bin [7:0],  AFIT8_Sharpening_iHighSharpPower_Bin [15:8]           /
	{0x0F12, 0x150C, WORD_LEN},		//70000AE0 //TVAR_afit_pBaseValS[398] /AFIT8_Sharpening_iLowShDenoise_Bin [7:0],  AFIT8_Sharpening_iHighShDenoise_Bin [15:8]             /
	{0x0F12, 0x0440, WORD_LEN},		//70000AE2 //TVAR_afit_pBaseValS[399] /AFIT8_Sharpening_iReduceEdgeMinMult_Bin [7:0],  AFIT8_Sharpening_iReduceEdgeSlope_Bin [15:8]      /
	{0x0F12, 0x0302, WORD_LEN},		//70000AE4 //TVAR_afit_pBaseValS[400] /AFIT8_demsharpmix1_iWideFiltReduce_Bin [7:0],  AFIT8_demsharpmix1_iNarrFiltReduce_Bin [15:8]      /
	{0x0F12, 0x3C3C, WORD_LEN},		//70000AE6 //TVAR_afit_pBaseValS[401] /AFIT8_sddd8a_iClustThresh_H_sBin[7:0],   AFIT8_sddd8a_iClustThresh_C_sBin [15:8]                  /
	{0x0F12, 0x0101, WORD_LEN},		//70000AE8 //TVAR_afit_pBaseValS[402] /AFIT8_sddd8a_iClustMulT_H_sBin [7:0],   AFIT8_sddd8a_iClustMulT_C_sBin [15:8]                     /
	{0x0F12, 0x1E01, WORD_LEN},		//70000AEA //TVAR_afit_pBaseValS[403] /AFIT8_sddd8a_nClustLevel_H_sBin [7:0],   AFIT8_sddd8a_DispTH_Low_sBin [15:8]                      /
	{0x0F12, 0x221C, WORD_LEN},		//70000AEC //TVAR_afit_pBaseValS[404] /AFIT8_sddd8a_DispTH_High_sBin [7:0],   AFIT8_sddd8a_iDenThreshLow_sBin [15:8]                     /
	{0x0F12, 0x281E, WORD_LEN},		//70000AEE //TVAR_afit_pBaseValS[405] /AFIT8_sddd8a_iDenThreshHigh_sBin[7:0],   AFIT8_Demosaicing_iEdgeDesat_sBin [15:8]                 /
	{0x0F12, 0x0A00, WORD_LEN},		//70000AF0 //TVAR_afit_pBaseValS[406] /AFIT8_Demosaicing_iEdgeDesatThrLow_sBin [7:0],  AFIT8_Demosaicing_iEdgeDesatThrHigh_sBin [15:8]   /
	{0x0F12, 0x1403, WORD_LEN},		//70000AF2 //TVAR_afit_pBaseValS[407] /AFIT8_Demosaicing_iEdgeDesatLimit_sBin [7:0],  AFIT8_Demosaicing_iDemSharpenLow_sBin [15:8]       /
	{0x0F12, 0x1402, WORD_LEN},		//70000AF4 //TVAR_afit_pBaseValS[408] /AFIT8_Demosaicing_iDemSharpenHigh_sBin [7:0],  AFIT8_Demosaicing_iDemSharpThresh_sBin [15:8]      /
	{0x0F12, 0x060E, WORD_LEN},		//70000AF6 //TVAR_afit_pBaseValS[409] /AFIT8_Demosaicing_iDemShLowLimit_sBin [7:0],  AFIT8_Demosaicing_iDespeckleForDemsharp_sBin [15:8] /
	{0x0F12, 0x32FF, WORD_LEN},		//70000AF8 //TVAR_afit_pBaseValS[410] /AFIT8_Demosaicing_iDemBlurLow_sBin [7:0],  AFIT8_Demosaicing_iDemBlurHigh_sBin [15:8]             /
	{0x0F12, 0x5204, WORD_LEN},		//70000AFA //TVAR_afit_pBaseValS[411] /AFIT8_Demosaicing_iDemBlurRange_sBin [7:0],  AFIT8_Sharpening_iLowSharpPower_sBin [15:8]          /
	{0x0F12, 0x0C40, WORD_LEN},		//70000AFC //TVAR_afit_pBaseValS[412] /AFIT8_Sharpening_iHighSharpPower_sBin [7:0],  AFIT8_Sharpening_iLowShDenoise_sBin [15:8]          /
	{0x0F12, 0x4015, WORD_LEN},		//70000AFE //TVAR_afit_pBaseValS[413] /AFIT8_Sharpening_iHighShDenoise_sBin [7:0],  AFIT8_Sharpening_iReduceEdgeMinMult_sBin [15:8]      /
	{0x0F12, 0x0204, WORD_LEN},		//70000B00 //TVAR_afit_pBaseValS[414] /AFIT8_Sharpening_iReduceEdgeSlope_sBin [7:0],  AFIT8_demsharpmix1_iWideFiltReduce_sBin [15:8]     /
	{0x0F12, 0x0003, WORD_LEN},		//70000B02 //TVAR_afit_pBaseValS[415] /AFIT8_demsharpmix1_iNarrFiltReduce_sBin [7:0]                                                     /
	{0x0F12, 0x0000, WORD_LEN},		//70000B04 //TVAR_afit_pBaseValS[416] /AFIT16_BRIGHTNESS                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000B06 //TVAR_afit_pBaseValS[417] /AFIT16_CONTRAST                                                                                   /
	{0x0F12, 0x0000, WORD_LEN},		//70000B08 //TVAR_afit_pBaseValS[418] /AFIT16_SATURATION                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000B0A //TVAR_afit_pBaseValS[419] /AFIT16_SHARP_BLUR                                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000B0C //TVAR_afit_pBaseValS[420] /AFIT16_GLAMOUR                                                                                    /
	{0x0F12, 0x00C1, WORD_LEN},		//70000B0E //TVAR_afit_pBaseValS[421] /AFIT16_sddd8a_edge_high                                                                           /
	{0x0F12, 0x03FF, WORD_LEN},		//70000B10 //TVAR_afit_pBaseValS[422] /AFIT16_Demosaicing_iSatVal                                                                        /
	{0x0F12, 0x009C, WORD_LEN},		//70000B12 //TVAR_afit_pBaseValS[423] /AFIT16_Sharpening_iReduceEdgeThresh                                                               /
	{0x0F12, 0x0251, WORD_LEN},		//70000B14 //TVAR_afit_pBaseValS[424] /AFIT16_demsharpmix1_iRGBOffset                                                                    /
	{0x0F12, 0x03FF, WORD_LEN},		//70000B16 //TVAR_afit_pBaseValS[425] /AFIT16_demsharpmix1_iDemClamp                                                                     /
	{0x0F12, 0x000C, WORD_LEN},		//70000B18 //TVAR_afit_pBaseValS[426] /AFIT16_demsharpmix1_iLowThreshold                                                                 /
	{0x0F12, 0x0010, WORD_LEN},		//70000B1A //TVAR_afit_pBaseValS[427] /AFIT16_demsharpmix1_iHighThreshold                                                                /
	{0x0F12, 0x0032, WORD_LEN},		//70000B1C //TVAR_afit_pBaseValS[428] /AFIT16_demsharpmix1_iLowBright                                                                    /
	{0x0F12, 0x028A, WORD_LEN},		//70000B1E //TVAR_afit_pBaseValS[429] /AFIT16_demsharpmix1_iHighBright                                                                   /
	{0x0F12, 0x0032, WORD_LEN},		//70000B20 //TVAR_afit_pBaseValS[430] /AFIT16_demsharpmix1_iLowSat                                                                       /
	{0x0F12, 0x01F4, WORD_LEN},		//70000B22 //TVAR_afit_pBaseValS[431] /AFIT16_demsharpmix1_iHighSat                                                                      /
	{0x0F12, 0x0070, WORD_LEN},		//70000B24 //TVAR_afit_pBaseValS[432] /AFIT16_demsharpmix1_iTune                                                                         /
	{0x0F12, 0x0000, WORD_LEN},		//70000B26 //TVAR_afit_pBaseValS[433] /AFIT16_demsharpmix1_iHystThLow                                                                    /
	{0x0F12, 0x0000, WORD_LEN},		//70000B28 //TVAR_afit_pBaseValS[434] /AFIT16_demsharpmix1_iHystThHigh                                                                   /
	{0x0F12, 0x01AA, WORD_LEN},		//70000B2A //TVAR_afit_pBaseValS[435] /AFIT16_demsharpmix1_iHystCenter                                                                   /
	{0x0F12, 0x003C, WORD_LEN},		//70000B2C //TVAR_afit_pBaseValS[436] /AFIT16_YUV422_DENOISE_iUVLowThresh                                                                /
	{0x0F12, 0x0050, WORD_LEN},		//70000B2E //TVAR_afit_pBaseValS[437] /AFIT16_YUV422_DENOISE_iUVHighThresh                                                               /
	{0x0F12, 0x0000, WORD_LEN},		//70000B30 //TVAR_afit_pBaseValS[438] /AFIT16_YUV422_DENOISE_iYLowThresh                                                                 /
	{0x0F12, 0x0000, WORD_LEN},		//70000B32 //TVAR_afit_pBaseValS[439] /AFIT16_YUV422_DENOISE_iYHighThresh                                                                /
	{0x0F12, 0x0044, WORD_LEN},		//70000B34 //TVAR_afit_pBaseValS[440] /AFIT16_Sharpening_iLowSharpClamp                                                                  /
	{0x0F12, 0x0014, WORD_LEN},		//70000B36 //TVAR_afit_pBaseValS[441] /AFIT16_Sharpening_iHighSharpClamp                                                                 /
	{0x0F12, 0x0046, WORD_LEN},		//70000B38 //TVAR_afit_pBaseValS[442] /AFIT16_Sharpening_iLowSharpClamp_Bin                                                              /
	{0x0F12, 0x0019, WORD_LEN},		//70000B3A //TVAR_afit_pBaseValS[443] /AFIT16_Sharpening_iHighSharpClamp_Bin                                                             /
	{0x0F12, 0x0046, WORD_LEN},		//70000B3C //TVAR_afit_pBaseValS[444] /AFIT16_Sharpening_iLowSharpClamp_sBin                                                             /
	{0x0F12, 0x0019, WORD_LEN},		//70000B3E //TVAR_afit_pBaseValS[445] /AFIT16_Sharpening_iHighSharpClamp_sBin                                                            /
	{0x0F12, 0x0A24, WORD_LEN},		//70000B40 //TVAR_afit_pBaseValS[446] /AFIT8_sddd8a_edge_low [7:0],   AFIT8_sddd8a_repl_thresh [15:8]                                    /
	{0x0F12, 0x1701, WORD_LEN},		//70000B42 //TVAR_afit_pBaseValS[447] /AFIT8_sddd8a_repl_force [7:0],  AFIT8_sddd8a_sat_level [15:8]                                     /
	{0x0F12, 0x0229, WORD_LEN},		//70000B44 //TVAR_afit_pBaseValS[448] /AFIT8_sddd8a_sat_thr[7:0],  AFIT8_sddd8a_sat_mpl [15:8]                                           /
	{0x0F12, 0x0503, WORD_LEN},		//70000B46 //TVAR_afit_pBaseValS[449] /AFIT8_sddd8a_sat_noise[7:0],  AFIT8_sddd8a_iMaxSlopeAllowed [15:8]                                /
	{0x0F12, 0x080F, WORD_LEN},		//70000B48 //TVAR_afit_pBaseValS[450] /AFIT8_sddd8a_iHotThreshHigh[7:0],  AFIT8_sddd8a_iHotThreshLow [15:8]                              /
	{0x0F12, 0x0808, WORD_LEN},		//70000B4A //TVAR_afit_pBaseValS[451] /AFIT8_sddd8a_iColdThreshHigh[7:0],  AFIT8_sddd8a_iColdThreshLow [15:8]                            /
	{0x0F12, 0x0000, WORD_LEN},		//70000B4C //TVAR_afit_pBaseValS[452] /AFIT8_sddd8a_AddNoisePower1[7:0],  AFIT8_sddd8a_AddNoisePower2 [15:8]                             /
	{0x0F12, 0x00FF, WORD_LEN},		//70000B4E //TVAR_afit_pBaseValS[453] /AFIT8_sddd8a_iSatSat[7:0],   AFIT8_sddd8a_iRadialTune [15:8]                                      /
	{0x0F12, 0x012D, WORD_LEN},		//70000B50 //TVAR_afit_pBaseValS[454] /AFIT8_sddd8a_iRadialLimit [7:0],   AFIT8_sddd8a_iRadialPower [15:8]                               /
	{0x0F12, 0x1414, WORD_LEN},		//70000B52 //TVAR_afit_pBaseValS[455] /AFIT8_sddd8a_iLowMaxSlopeAllowed [7:0],  AFIT8_sddd8a_iHighMaxSlopeAllowed [15:8]                 /
	{0x0F12, 0x0301, WORD_LEN},		//70000B54 //TVAR_afit_pBaseValS[456] /AFIT8_sddd8a_iLowSlopeThresh[7:0],   AFIT8_sddd8a_iHighSlopeThresh [15:8]                         /
	{0x0F12, 0xFF07, WORD_LEN},		//70000B56 //TVAR_afit_pBaseValS[457] /AFIT8_sddd8a_iSquaresRounding [7:0],   AFIT8_Demosaicing_iCentGrad [15:8]                         /
	{0x0F12, 0x061E, WORD_LEN},		//70000B58 //TVAR_afit_pBaseValS[458] /AFIT8_Demosaicing_iMonochrom [7:0],   AFIT8_Demosaicing_iDecisionThresh [15:8]                    /
	{0x0F12, 0x0A1E, WORD_LEN},		//70000B5A //TVAR_afit_pBaseValS[459] /AFIT8_Demosaicing_iDesatThresh [7:0],   AFIT8_Demosaicing_iEnhThresh [15:8]                       /
	{0x0F12, 0x0606, WORD_LEN},		//70000B5C //TVAR_afit_pBaseValS[460] /AFIT8_Demosaicing_iGRDenoiseVal [7:0],   AFIT8_Demosaicing_iGBDenoiseVal [15:8]                   /
	{0x0F12, 0x0A03, WORD_LEN},		//70000B5E //TVAR_afit_pBaseValS[461] /AFIT8_Demosaicing_iNearGrayDesat[7:0],   AFIT8_Demosaicing_iDFD_ReduceCoeff [15:8]                /
	{0x0F12, 0x378B, WORD_LEN},		//70000B60 //TVAR_afit_pBaseValS[462] /AFIT8_Sharpening_iMSharpen [7:0],   AFIT8_Sharpening_iMShThresh [15:8]                            /
	{0x0F12, 0x1028, WORD_LEN},		//70000B62 //TVAR_afit_pBaseValS[463] /AFIT8_Sharpening_iWSharpen [7:0],   AFIT8_Sharpening_iWShThresh [15:8]                            /
	{0x0F12, 0x0001, WORD_LEN},		//70000B64 //TVAR_afit_pBaseValS[464] /AFIT8_Sharpening_nSharpWidth [7:0],   AFIT8_Sharpening_iReduceNegative [15:8]                     /
	{0x0F12, 0x00FF, WORD_LEN},		//70000B66 //TVAR_afit_pBaseValS[465] /AFIT8_Sharpening_iShDespeckle [7:0],  AFIT8_demsharpmix1_iRGBMultiplier [15:8]                    /
	{0x0F12, 0x1002, WORD_LEN},		//70000B68 //TVAR_afit_pBaseValS[466] /AFIT8_demsharpmix1_iFilterPower [7:0],  AFIT8_demsharpmix1_iBCoeff [15:8]                         /
	{0x0F12, 0x001E, WORD_LEN},		//70000B6A //TVAR_afit_pBaseValS[467] /AFIT8_demsharpmix1_iGCoeff [7:0],   AFIT8_demsharpmix1_iWideMult [15:8]                           /
	{0x0F12, 0x0900, WORD_LEN},		//70000B6C //TVAR_afit_pBaseValS[468] /AFIT8_demsharpmix1_iNarrMult [7:0],   AFIT8_demsharpmix1_iHystFalloff [15:8]                      /
	{0x0F12, 0x0600, WORD_LEN},		//70000B6E //TVAR_afit_pBaseValS[469] /AFIT8_demsharpmix1_iHystMinMult [7:0],   AFIT8_demsharpmix1_iHystWidth [15:8]                     /
	{0x0F12, 0x0504, WORD_LEN},		//70000B70 //TVAR_afit_pBaseValS[470] /AFIT8_demsharpmix1_iHystFallLow [7:0],   AFIT8_demsharpmix1_iHystFallHigh [15:8]                  /
	{0x0F12, 0x0307, WORD_LEN},		//70000B72 //TVAR_afit_pBaseValS[471] /AFIT8_demsharpmix1_iHystTune [7:0],  * AFIT8_YUV422_DENOISE_iUVSupport [15:8]                     /
	{0x0F12, 0x5001, WORD_LEN},		//70000B74 //TVAR_afit_pBaseValS[472] /AFIT8_YUV422_DENOISE_iYSupport [7:0],   AFIT8_byr_cgras_iShadingPower [15:8]                      /
	{0x0F12, 0x0080, WORD_LEN},		//70000B76 //TVAR_afit_pBaseValS[473] /AFIT8_RGBGamma2_iLinearity [7:0],  AFIT8_RGBGamma2_iDarkReduce [15:8]                             /
	{0x0F12, 0x0080, WORD_LEN},		//70000B78 //TVAR_afit_pBaseValS[474] /AFIT8_ccm_oscar_iSaturation[7:0],   AFIT8_RGB2YUV_iYOffset [15:8]                                 /
	{0x0F12, 0x0080, WORD_LEN},		//70000B7A //TVAR_afit_pBaseValS[475] /AFIT8_RGB2YUV_iRGBGain [7:0],   AFIT8_RGB2YUV_iSaturation [15:8]                                  /
	{0x0F12, 0x5050, WORD_LEN},		//70000B7C //TVAR_afit_pBaseValS[476] /AFIT8_sddd8a_iClustThresh_H [7:0],  AFIT8_sddd8a_iClustThresh_C [15:8]                            /
	{0x0F12, 0x0101, WORD_LEN},		//70000B7E //TVAR_afit_pBaseValS[477] /AFIT8_sddd8a_iClustMulT_H [7:0],   AFIT8_sddd8a_iClustMulT_C [15:8]                               /
	{0x0F12, 0x3201, WORD_LEN},		//70000B80 //TVAR_afit_pBaseValS[478] /AFIT8_sddd8a_nClustLevel_H [7:0],   AFIT8_sddd8a_DispTH_Low [15:8]                                /
	{0x0F12, 0x1832, WORD_LEN},		//70000B82 //TVAR_afit_pBaseValS[479] /AFIT8_sddd8a_DispTH_High [7:0],   AFIT8_sddd8a_iDenThreshLow [15:8]                               /        iden 12
	{0x0F12, 0x210C, WORD_LEN},		//70000B84 //TVAR_afit_pBaseValS[480] /AFIT8_sddd8a_iDenThreshHigh[7:0],   AFIT8_Demosaicing_iEdgeDesat [15:8]                           /
	{0x0F12, 0x0A00, WORD_LEN},		//70000B86 //TVAR_afit_pBaseValS[481] /AFIT8_Demosaicing_iEdgeDesatThrLow [7:0],   AFIT8_Demosaicing_iEdgeDesatThrHigh [15:8]            /
	{0x0F12, 0x1E04, WORD_LEN},		//70000B88 //TVAR_afit_pBaseValS[482] /AFIT8_Demosaicing_iEdgeDesatLimit[7:0],  AFIT8_Demosaicing_iDemSharpenLow [15:8]                  /    1E demhsharplow
	{0x0F12, 0x0A08, WORD_LEN},		//70000B8A //TVAR_afit_pBaseValS[483] /AFIT8_Demosaicing_iDemSharpenHigh[7:0],   AFIT8_Demosaicing_iDemSharpThresh [15:8]                /
	{0x0F12, 0x070C, WORD_LEN},		//70000B8C //TVAR_afit_pBaseValS[484] /AFIT8_Demosaicing_iDemShLowLimit [7:0],   AFIT8_Demosaicing_iDespeckleForDemsharp [15:8]          /    c8 clamp
	{0x0F12, 0x3264, WORD_LEN},		//70000B8E //TVAR_afit_pBaseValS[485] /AFIT8_Demosaicing_iDemBlurLow[7:0],   AFIT8_Demosaicing_iDemBlurHigh [15:8]                       /
	{0x0F12, 0x5A02, WORD_LEN},		//70000B90 //TVAR_afit_pBaseValS[486] /AFIT8_Demosaicing_iDemBlurRange[7:0],   AFIT8_Sharpening_iLowSharpPower [15:8]                    /
	{0x0F12, 0x1040, WORD_LEN},		//70000B92 //TVAR_afit_pBaseValS[487] /AFIT8_Sharpening_iHighSharpPower[7:0],   AFIT8_Sharpening_iLowShDenoise [15:8]                    /
	{0x0F12, 0x4012, WORD_LEN},		//70000B94 //TVAR_afit_pBaseValS[488] /AFIT8_Sharpening_iHighShDenoise [7:0],   AFIT8_Sharpening_iReduceEdgeMinMult [15:8]               /
	{0x0F12, 0x0604, WORD_LEN},		//70000B96 //TVAR_afit_pBaseValS[489] /AFIT8_Sharpening_iReduceEdgeSlope [7:0],  AFIT8_demsharpmix1_iWideFiltReduce [15:8]               /
	{0x0F12, 0x4606, WORD_LEN},		//70000B98 //TVAR_afit_pBaseValS[490] /AFIT8_demsharpmix1_iNarrFiltReduce [7:0],  AFIT8_sddd8a_iClustThresh_H_Bin [15:8]                 /
	{0x0F12, 0x0146, WORD_LEN},		//70000B9A //TVAR_afit_pBaseValS[491] /AFIT8_sddd8a_iClustThresh_C_Bin [7:0],   AFIT8_sddd8a_iClustMulT_H_Bin [15:8]                     /
	{0x0F12, 0x0101, WORD_LEN},		//70000B9C //TVAR_afit_pBaseValS[492] /AFIT8_sddd8a_iClustMulT_C_Bin [7:0],   AFIT8_sddd8a_nClustLevel_H_Bin [15:8]                      /
	{0x0F12, 0x1C18, WORD_LEN},		//70000B9E //TVAR_afit_pBaseValS[493] /AFIT8_sddd8a_DispTH_Low_Bin [7:0],   AFIT8_sddd8a_DispTH_High_Bin [15:8]                          /
	{0x0F12, 0x1819, WORD_LEN},		//70000BA0 //TVAR_afit_pBaseValS[494] /AFIT8_sddd8a_iDenThreshLow_Bin [7:0],   AFIT8_sddd8a_iDenThreshHigh_Bin [15:8]                    /
	{0x0F12, 0x0028, WORD_LEN},		//70000BA2 //TVAR_afit_pBaseValS[495] /AFIT8_Demosaicing_iEdgeDesat_Bin[7:0],   AFIT8_Demosaicing_iEdgeDesatThrLow_Bin [15:8]            /
	{0x0F12, 0x030A, WORD_LEN},		//70000BA4 //TVAR_afit_pBaseValS[496] /AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin [7:0],  AFIT8_Demosaicing_iEdgeDesatLimit_Bin [15:8]      /
	{0x0F12, 0x0514, WORD_LEN},		//70000BA6 //TVAR_afit_pBaseValS[497] /AFIT8_Demosaicing_iDemSharpenLow_Bin [7:0],  AFIT8_Demosaicing_iDemSharpenHigh_Bin [15:8]         /
	{0x0F12, 0x0C14, WORD_LEN},		//70000BA8 //TVAR_afit_pBaseValS[498] /AFIT8_Demosaicing_iDemSharpThresh_Bin [7:0],  AFIT8_Demosaicing_iDemShLowLimit_Bin [15:8]         /
	{0x0F12, 0xFF05, WORD_LEN},		//70000BAA //TVAR_afit_pBaseValS[499] /AFIT8_Demosaicing_iDespeckleForDemsharp_Bin [7:0],  AFIT8_Demosaicing_iDemBlurLow_Bin [15:8]      /
	{0x0F12, 0x0432, WORD_LEN},		//70000BAC //TVAR_afit_pBaseValS[500] /AFIT8_Demosaicing_iDemBlurHigh_Bin [7:0],  AFIT8_Demosaicing_iDemBlurRange_Bin [15:8]             /
	{0x0F12, 0x4052, WORD_LEN},		//70000BAE //TVAR_afit_pBaseValS[501] /AFIT8_Sharpening_iLowSharpPower_Bin [7:0],  AFIT8_Sharpening_iHighSharpPower_Bin [15:8]           /
	{0x0F12, 0x1514, WORD_LEN},		//70000BB0 //TVAR_afit_pBaseValS[502] /AFIT8_Sharpening_iLowShDenoise_Bin [7:0],  AFIT8_Sharpening_iHighShDenoise_Bin [15:8]             /
	{0x0F12, 0x0440, WORD_LEN},		//70000BB2 //TVAR_afit_pBaseValS[503] /AFIT8_Sharpening_iReduceEdgeMinMult_Bin [7:0],  AFIT8_Sharpening_iReduceEdgeSlope_Bin [15:8]      /
	{0x0F12, 0x0302, WORD_LEN},		//70000BB4 //TVAR_afit_pBaseValS[504] /AFIT8_demsharpmix1_iWideFiltReduce_Bin [7:0],  AFIT8_demsharpmix1_iNarrFiltReduce_Bin [15:8]      /
	{0x0F12, 0x4646, WORD_LEN},		//70000BB6 //TVAR_afit_pBaseValS[505] /AFIT8_sddd8a_iClustThresh_H_sBin[7:0],   AFIT8_sddd8a_iClustThresh_C_sBin [15:8]                  /
	{0x0F12, 0x0101, WORD_LEN},		//70000BB8 //TVAR_afit_pBaseValS[506] /AFIT8_sddd8a_iClustMulT_H_sBin [7:0],   AFIT8_sddd8a_iClustMulT_C_sBin [15:8]                     /
	{0x0F12, 0x1801, WORD_LEN},		//70000BBA //TVAR_afit_pBaseValS[507] /AFIT8_sddd8a_nClustLevel_H_sBin [7:0],   AFIT8_sddd8a_DispTH_Low_sBin [15:8]                      /
	{0x0F12, 0x191C, WORD_LEN},		//70000BBC //TVAR_afit_pBaseValS[508] /AFIT8_sddd8a_DispTH_High_sBin [7:0],   AFIT8_sddd8a_iDenThreshLow_sBin [15:8]                     /
	{0x0F12, 0x2818, WORD_LEN},		//70000BBE //TVAR_afit_pBaseValS[509] /AFIT8_sddd8a_iDenThreshHigh_sBin[7:0],   AFIT8_Demosaicing_iEdgeDesat_sBin [15:8]                 /
	{0x0F12, 0x0A00, WORD_LEN},		//70000BC0 //TVAR_afit_pBaseValS[510] /AFIT8_Demosaicing_iEdgeDesatThrLow_sBin [7:0],  AFIT8_Demosaicing_iEdgeDesatThrHigh_sBin [15:8]   /
	{0x0F12, 0x1403, WORD_LEN},		//70000BC2 //TVAR_afit_pBaseValS[511] /AFIT8_Demosaicing_iEdgeDesatLimit_sBin [7:0],  AFIT8_Demosaicing_iDemSharpenLow_sBin [15:8]       /
	{0x0F12, 0x1405, WORD_LEN},		//70000BC4 //TVAR_afit_pBaseValS[512] /AFIT8_Demosaicing_iDemSharpenHigh_sBin [7:0],  AFIT8_Demosaicing_iDemSharpThresh_sBin [15:8]      /
	{0x0F12, 0x050C, WORD_LEN},		//70000BC6 //TVAR_afit_pBaseValS[513] /AFIT8_Demosaicing_iDemShLowLimit_sBin [7:0],  AFIT8_Demosaicing_iDespeckleForDemsharp_sBin [15:8] /
	{0x0F12, 0x32FF, WORD_LEN},		//70000BC8 //TVAR_afit_pBaseValS[514] /AFIT8_Demosaicing_iDemBlurLow_sBin [7:0],  AFIT8_Demosaicing_iDemBlurHigh_sBin [15:8]             /
	{0x0F12, 0x5204, WORD_LEN},		//70000BCA //TVAR_afit_pBaseValS[515] /AFIT8_Demosaicing_iDemBlurRange_sBin [7:0],  AFIT8_Sharpening_iLowSharpPower_sBin [15:8]          /
	{0x0F12, 0x1440, WORD_LEN},		//70000BCC //TVAR_afit_pBaseValS[516] /AFIT8_Sharpening_iHighSharpPower_sBin [7:0],  AFIT8_Sharpening_iLowShDenoise_sBin [15:8]          /
	{0x0F12, 0x4015, WORD_LEN},		//70000BCE //TVAR_afit_pBaseValS[517] /AFIT8_Sharpening_iHighShDenoise_sBin [7:0],  AFIT8_Sharpening_iReduceEdgeMinMult_sBin [15:8]      /
	{0x0F12, 0x0204, WORD_LEN},		//70000BD0 //TVAR_afit_pBaseValS[518] /AFIT8_Sharpening_iReduceEdgeSlope_sBin [7:0],  AFIT8_demsharpmix1_iWideFiltReduce_sBin [15:8]     /
	{0x0F12, 0x0003, WORD_LEN},		//70000BD2 //TVAR_afit_pBaseValS[519] /AFIT8_demsharpmix1_iNarrFiltReduce_sBin [7:0]                                                     /
	                        
	{0x0F12, 0x7DFA, WORD_LEN},		//afit_pConstBaseValS[0] //                                                                          
	{0x0F12, 0xFFBD, WORD_LEN},		//afit_pConstBaseValS[1] //                                                                          
	{0x0F12, 0x26FE, WORD_LEN},		//afit_pConstBaseValS[2] //                                                                          
	{0x0F12, 0xF7BC, WORD_LEN},		//afit_pConstBaseValS[3] //                                                                          
	{0x0F12, 0x7E06, WORD_LEN},		//afit_pConstBaseValS[4] //                                                                          
	{0x0F12, 0x00D3, WORD_LEN},		//afit_pConstBaseValS[5] //                                                                          

	                     
	{0x002A, 0x2CE8, WORD_LEN},	    
	{0x0F12, 0x0007, WORD_LEN},		// Modify LSB to control AWBB_YThreshLow //    	                                                                        
	{0x0F12, 0x00E2, WORD_LEN},		//                                         
	{0x0F12, 0x0005, WORD_LEN},		// Modify LSB to control AWBB_YThreshLowBrLow//                                                                        
	{0x0F12, 0x00E2, WORD_LEN},		//                                         
	                                                                    
	                                                                                  
	{0x002A, 0x0F7E, WORD_LEN},	    //ae weight    //                                
	{0x0F12, 0x0000, WORD_LEN},	  //0000  //0000
	{0x0F12, 0x0000, WORD_LEN},	  //0000  //0000
	{0x0F12, 0x0000, WORD_LEN},	  //0000  //0000
	{0x0F12, 0x0000, WORD_LEN},	  //0000  //0000                     
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101                     
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101
	{0x0F12, 0x0201, WORD_LEN},	  //0201  //0201
	{0x0F12, 0x0102, WORD_LEN},	  //0102  //0102
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101                     
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101                     
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0101                     
	{0x0F12, 0x0201, WORD_LEN},	  //0101  //0101
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0102, WORD_LEN},	  //0101  //0101                     
	{0x0F12, 0x0201, WORD_LEN},	  //0101  //0201
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0202, WORD_LEN},	  //0202  //0202
	{0x0F12, 0x0102, WORD_LEN},	  //0101  //0102                     
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0201
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0202
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0202
	{0x0F12, 0x0101, WORD_LEN},	  //0101  //0102
	                     
	                            
	{0x002A, 0x01CC, WORD_LEN},	    
	{0x0F12, 0x5DC0, WORD_LEN},		//REG_TC_IPRM_InClockLSBs//input clock=24MHz                        	                                                                        
	{0x0F12, 0x0000, WORD_LEN},	    //REG_TC_IPRM_InClockMSBs                                           
	{0x002A, 0x01EE, WORD_LEN},	                                                                        
	{0x0F12, 0x0003, WORD_LEN},	 //#REG_TC_IPRM_UseNPviClocks		// Number of PLL setting
	//s0F120003 //#REG_TC_IPRM_UseNMiPiClocks		// Number of PLL setting                                     
	    
	{0x002A, 0x01F6, WORD_LEN},	                                                                        
	{0x0F12, 0x1F40, WORD_LEN},	    //REG_TC_IPRM_OpClk4KHz_0                   	2   700001F6         
	{0x0F12, 0x32A8, WORD_LEN},		//3A88    //REG_TC_IPRM_MinOutRate4KHz_0              	2   700001F8                                                                        
	{0x0F12, 0x32E8, WORD_LEN},		//3AA8    //REG_TC_IPRM_MaxOutRate4KHz_0                        
	                     
	{0x0F12, 0x1F40, WORD_LEN},		 //REG_TC_IPRM_OpClk4KHz_1                   	2   700001FC    
	{0x0F12, 0x2ea0, WORD_LEN},	    //REG_TC_IPRM_MinOutRate4KHz_1              	2   700001FE 
	{0x0F12, 0x2f00, WORD_LEN},	    //REG_TC_IPRM_MaxOutRate4KHz_1              	2   70000200 
	         
	{0x0F12, 0x0BB8, WORD_LEN},	    //REG_TC_IPRM_OpClk4KHz_2                   	2   70000202 
	{0x0F12, 0x05DC, WORD_LEN},	    //REG_TC_IPRM_MinOutRate4KHz_2              	2   70000204 
	{0x0F12, 0x1770, WORD_LEN},	    //REG_TC_IPRM_MaxOutRate4KHz_2              	2   70000206 
	         
	{0x002A, 0x0208, WORD_LEN},	                                                                
	{0x0F12, 0x0001, WORD_LEN},	      //REG_TC_IPRM_InitParamsUpdated                           

	//sffff0064
	//p100
	{S5K5CAGA_REG_TERM, 0x0064, WORD_LEN}, 

	////sffffffff  //sleep(100)                     

	   
	{0x0028, 0x7000, WORD_LEN},		    
	{0x002A, 0x026C, WORD_LEN},	   //Normal preview 15fps
	{0x0F12, 0x0280, WORD_LEN},		//REG_0TC_PCFG_usWidth                      	2   7000026C			//                            
	{0x0F12, 0x01E0, WORD_LEN},	   //REG_0TC_PCFG_usHeight                     	2   7000026E    //                                         
	{0x0F12, 0x0005, WORD_LEN},	   //REG_0TC_PCFG_Format                       	2   70000270    //                                         
	{0x0F12, 0x32E8, WORD_LEN},		//3AA8//REG_0TC_PCFG_usMaxOut4KHzRate             	2   70000272  //                              
	{0x0F12, 0x32a8, WORD_LEN},		//3A88//REG_0TC_PCFG_usMinOut4KHzRate             	2   70000274  //                              
	{0x0F12, 0x0100, WORD_LEN},	   //REG_0TC_PCFG_OutClkPerPix88               	2   70000276    //                                         
	{0x0F12, 0x0800, WORD_LEN},	   //REG_0TC_PCFG_uMaxBpp88                    	2   70000278    //                                         
	{0x0F12, 0x0050, WORD_LEN},	   //REG_0TC_PCFG_PVIMask                      	2   7000027A    //92  (1) PCLK inversion  (4)1b_C first (5) UV First                                       
	{0x0F12, 0x0010, WORD_LEN},	   //REG_0TC_PCFG_OIFMask                      	2   7000027C    //                                         
	{0x0F12, 0x01E0, WORD_LEN},	   //REG_0TC_PCFG_usJpegPacketSize             	2   7000027E    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_usJpegTotalPackets           	2   70000280    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_uClockInd                    	2   70000282    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_usFrTimeType                 	2   70000284    //                                         
	{0x0F12, 0x0001, WORD_LEN},	   //REG_0TC_PCFG_FrRateQualityType            	2   70000286    //                                         
	{0x0F12, 0x0535, WORD_LEN},	   //REG_0TC_PCFG_usMaxFrTimeMsecMult10        	2   70000288    //                                         
	{0x0F12, 0x029A, WORD_LEN},	   //REG_0TC_PCFG_usMinFrTimeMsecMult10        	2   7000028A    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_bSmearOutput                 	2   7000028C    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_sSaturation                  	2   7000028E    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_sSharpBlur                   	2   70000290    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_sColorTemp                   	2   70000292    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_uDeviceGammaIndex            	2   70000294    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_uPrevMirror                  	2   70000296    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_uCaptureMirror               	2   70000298    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_PCFG_uRotation                    	2   7000029A    // 
	                    
	                    //comcoder frame fix 15                                        
	{0x0F12, 0x0400, WORD_LEN},	   //REG_1TC_PCFG_usWidth                      	2   7000029C    //                                         
	{0x0F12, 0x0300, WORD_LEN},	   //REG_1TC_PCFG_usHeight                     	2   7000029E    //                                         
	{0x0F12, 0x0005, WORD_LEN},	   //REG_1TC_PCFG_Format                       	2   700002A0    //                                         
	{0x0F12, 0x32E8, WORD_LEN},	   //REG_1TC_PCFG_usMaxOut4KHzRate             	2   700002A2    //                                         
	{0x0F12, 0x32a8, WORD_LEN},	   //REG_1TC_PCFG_usMinOut4KHzRate             	2   700002A4    //                                         
	{0x0F12, 0x0100, WORD_LEN},	   //REG_1TC_PCFG_OutClkPerPix88               	2   700002A6    //                                         
	{0x0F12, 0x0800, WORD_LEN},	   //REG_1TC_PCFG_uMaxBpp88                    	2   700002A8    //                                         
	{0x0F12, 0x0050, WORD_LEN},	   //REG_1TC_PCFG_PVIMask                      	2   700002AA    //                                         
	{0x0F12, 0x0010, WORD_LEN},	   //REG_1TC_PCFG_OIFMask                      	2   700002AC    //                                         
	{0x0F12, 0x01E0, WORD_LEN},	   //REG_1TC_PCFG_usJpegPacketSize             	2   700002AE    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_usJpegTotalPackets           	2   700002B0    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_uClockInd                    	2   700002B2    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_usFrTimeType                 	2   700002B4    //                                         
	{0x0F12, 0x0001, WORD_LEN},	   //REG_1TC_PCFG_FrRateQualityType            	2   700002B6    //                                         
	{0x0F12, 0x029a, WORD_LEN},	   //REG_1TC_PCFG_usMaxFrTimeMsecMult10        	2   700002B8    //                                         
	{0x0F12, 0x029a, WORD_LEN},	   //REG_1TC_PCFG_usMinFrTimeMsecMult10        	2   700002BA    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_bSmearOutput                 	2   700002BC    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_sSaturation                  	2   700002BE    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_sSharpBlur                   	2   700002C0    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_sColorTemp                   	2   700002C2    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_uDeviceGammaIndex            	2   700002C4    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_uPrevMirror                  	2   700002C6    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_uCaptureMirror               	2   700002C8    //                                         
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_PCFG_uRotation                    	2   700002CA    // 
	                                    
	                         
	{0x002A, 0x035C, WORD_LEN},	   //Normal capture //    
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_uCaptureMode                 	2   7000035C						// 
	{0x0F12, 0x0800, WORD_LEN},	   //REG_0TC_CCFG_usWidth                      	2   7000035E          //                                   
	{0x0F12, 0x0600, WORD_LEN},	   //REG_0TC_CCFG_usHeight                     	2   70000360          //                                   
	{0x0F12, 0x0005, WORD_LEN},	   //REG_0TC_CCFG_Format                       	2   70000362          //                                   
	{0x0F12, 0x32E8, WORD_LEN},		//2f00//3AA8//REG_0TC_CCFG_usMaxOut4KHzRate             	2   70000364  //                      
	{0x0F12, 0x32a8, WORD_LEN},		//2ea0//3A88//REG_0TC_CCFG_usMinOut4KHzRate             	2   70000366  //                      
	{0x0F12, 0x0100, WORD_LEN},	   //REG_0TC_CCFG_OutClkPerPix88               	2   70000368          //                                   
	{0x0F12, 0x0800, WORD_LEN},	   //REG_0TC_CCFG_uMaxBpp88                    	2   7000036A          //                                   
	{0x0F12, 0x0050, WORD_LEN},	   //REG_0TC_CCFG_PVIMask                      	2   7000036C          //                                   
	{0x0F12, 0x0010, WORD_LEN},	   //REG_0TC_CCFG_OIFMask                      	2   7000036E          //                                   
	{0x0F12, 0x01E0, WORD_LEN},	   //REG_0TC_CCFG_usJpegPacketSize             	2   70000370          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_usJpegTotalPackets           	2   70000372          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_uClockInd                    	2   70000374          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_usFrTimeType                 	2   70000376          //                                   
	{0x0F12, 0x0002, WORD_LEN},	   //REG_0TC_CCFG_FrRateQualityType            	2   70000378          //                                   
	{0x0F12, 0x0535, WORD_LEN},	   //REG_0TC_CCFG_usMaxFrTimeMsecMult10        	2   7000037A          //                                   
	{0x0F12, 0x0535, WORD_LEN},	   //REG_0TC_CCFG_usMinFrTimeMsecMult10        	2   7000037C          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_bSmearOutput                 	2   7000037E          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_sSaturation                  	2   70000380          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_sSharpBlur                   	2   70000382          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_sColorTemp                   	2   70000384          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_0TC_CCFG_uDeviceGammaIndex            	2   70000386          //                                   
	             
	                    // Low_lux capture//    
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_uCaptureMode                 	2   70000388						// 
	{0x0F12, 0x0800, WORD_LEN},	   //REG_1TC_CCFG_usWidth                      	2   7000038A          //                                   
	{0x0F12, 0x0600, WORD_LEN},	   //REG_1TC_CCFG_usHeight                     	2   7000038C          //                                   
	{0x0F12, 0x0005, WORD_LEN},	   //REG_1TC_CCFG_Format                       	2   7000038E          //                                   
	{0x0F12, 0x32E8, WORD_LEN},	   //REG_1TC_CCFG_usMaxOut4KHzRate             	2   70000390          //                                   
	{0x0F12, 0x32a8, WORD_LEN},	   //REG_1TC_CCFG_usMinOut4KHzRate             	2   70000392          //                                   
	{0x0F12, 0x0100, WORD_LEN},	   //REG_1TC_CCFG_OutClkPerPix88               	2   70000394          //                                   
	{0x0F12, 0x0800, WORD_LEN},	   //REG_1TC_CCFG_uMaxBpp88                    	2   70000396          //                                   
	{0x0F12, 0x0050, WORD_LEN},	   //REG_1TC_CCFG_PVIMask                      	2   70000398          //                                   
	{0x0F12, 0x0010, WORD_LEN},	   //REG_1TC_CCFG_OIFMask                      	2   7000039A          //                                   
	{0x0F12, 0x01E0, WORD_LEN},	   //REG_1TC_CCFG_usJpegPacketSize             	2   7000039C          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_usJpegTotalPackets           	2   7000039E          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_uClockInd                    	2   700003A0          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_usFrTimeType                 	2   700003A2          //                                   
	{0x0F12, 0x0002, WORD_LEN},	   //REG_1TC_CCFG_FrRateQualityType            	2   700003A4          //                                   
	{0x0F12, 0x0535, WORD_LEN},	   //REG_1TC_CCFG_usMaxFrTimeMsecMult10        	2   700003A6          //                                   
	{0x0F12, 0x0535, WORD_LEN},	   //REG_1TC_CCFG_usMinFrTimeMsecMult10        	2   700003A8          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_bSmearOutput                 	2   700003AA          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_sSaturation                  	2   700003AC          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_sSharpBlur                   	2   700003AE          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_sColorTemp                   	2   700003B0          //                                   
	{0x0F12, 0x0000, WORD_LEN},	   //REG_1TC_CCFG_uDeviceGammaIndex            	2   700003B2          //                    
	                      
	{0x002A, 0x0208, WORD_LEN},	        
	{0x0F12, 0x0001, WORD_LEN},	      //REG_TC_IPRM_InitParamsUpdated//                                                                       
	                         
	{0x002A, 0x023C, WORD_LEN},	   
	{0x0F12, 0x0000, WORD_LEN},		//32MHz Sys Clock//	
	{0x002A, 0x0244, WORD_LEN},		                   
	{0x0F12, 0x0000, WORD_LEN},	       
	{0x002A, 0x0240, WORD_LEN},	 
	{0x0F12, 0x0001, WORD_LEN},	 
	{0x002A, 0x0230, WORD_LEN},	 
	{0x0F12, 0x0001, WORD_LEN},	 
	{0x002A, 0x023E, WORD_LEN},	 
	{0x0F12, 0x0001, WORD_LEN},	 
	{0x002A, 0x0246, WORD_LEN},	 
	{0x0F12, 0x0001, WORD_LEN},	 
	{0x002A, 0x0220, WORD_LEN},	 
	{0x0F12, 0x0001, WORD_LEN},	 
	{0x0F12, 0x0001, WORD_LEN},	 
	                  
	{0x1000, 0x0001, WORD_LEN},		// Set host interrupt so main start run//                                                  

	//p100	//Delay 100ms	
	{S5K5CAGA_REG_TERM, 0x0064, WORD_LEN}, 

	                     
	{0x002A, 0x0DA0, WORD_LEN},	    
	{0x0F12, 0x0005, WORD_LEN},	    
	                     
	{0x002A, 0x0D88, WORD_LEN},	    
	{0x0F12, 0x0038, WORD_LEN},	    
	{0x0F12, 0x0074, WORD_LEN},	    
	{0x0F12, 0xFFF1, WORD_LEN},	    
	{0x0F12, 0x00BF, WORD_LEN},	    
	{0x0F12, 0xFF9B, WORD_LEN},	    
	{0x0F12, 0x00DB, WORD_LEN},	    
	{0x0F12, 0xFF56, WORD_LEN},	    
	{0x0F12, 0x00F0, WORD_LEN},	    
	{0x0F12, 0xFEFF, WORD_LEN},	    
	{0x0F12, 0x010F, WORD_LEN},	    
	{0x0F12, 0x0E74, WORD_LEN},	    
	{0x002A, 0x0DA8, WORD_LEN},	    
	{0x0F12, 0x0BB8, WORD_LEN},	    //NB 3000
	{0x002A, 0x0DA4, WORD_LEN},	    
	{0x0F12, 0x274E, WORD_LEN},	    

	{0x002A, 0x0DCA, WORD_LEN},	
	{0x0F12, 0x0030, WORD_LEN},	

	{0x002A, 0x3286, WORD_LEN},	
	{0x0F12, 0x0001, WORD_LEN},	 //Pre/Post gamma on(원복)       

	{0x002A, 0x032C, WORD_LEN},	
	{0x0F12, 0xAAAA, WORD_LEN},	  //ESD Check             


	{0x002A, 0x032E, WORD_LEN},	
	{0x0F12, 0xFFFE, WORD_LEN},	    //HighLux over this NB
	{0x0F12, 0x0000, WORD_LEN},	   

	{0x0F12, 0x001C, WORD_LEN},	    //LowLux under this NB
	{0x0F12, 0x0000, WORD_LEN},	 
  
	{0xFFFF, 0xFFFF, WORD_LEN},

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_set_preview_s5k5caga[] = {
	{0xFCFC, 0xD000, WORD_LEN},
	{0x0028, 0x7000, WORD_LEN},

	{0x002A, 0x07E6, WORD_LEN},
	{0x0F12, 0x0000, WORD_LEN},	//700007E6 //TVAR_afit_pBaseValS[17] //AFIT16_demsharpmix1_iHystThLow
	{0x0F12, 0x0000, WORD_LEN},	//700007E8 //TVAR_afit_pBaseValS[18] //AFIT16_demsharpmix1_iHystThHigh

	{0x002A, 0x08B6, WORD_LEN},
	{0x0F12, 0x0000, WORD_LEN},	//700008B6 //TVAR_afit_pBaseValS[121] /AFIT16_demsharpmix1_iHystThLow
	{0x0F12, 0x0000, WORD_LEN},	//700008B8 //TVAR_afit_pBaseValS[122] /AFIT16_demsharpmix1_iHystThHigh

	{0x002A, 0x0986, WORD_LEN},
	{0x0F12, 0x0000, WORD_LEN},	//70000986 //TVAR_afit_pBaseValS[225] /AFIT16_demsharpmix1_iHystThLow
	{0x0F12, 0x0000, WORD_LEN},	//70000988 //TVAR_afit_pBaseValS[226] /AFIT16_demsharpmix1_iHystThHigh
    
	{0x002A, 0x0A56, WORD_LEN},
	{0x0F12, 0x0000, WORD_LEN},	//70000A56 //TVAR_afit_pBaseValS[329] /AFIT16_demsharpmix1_iHystThLow
	{0x0F12, 0x0000, WORD_LEN},	//70000A58 //TVAR_afit_pBaseValS[330] /AFIT16_demsharpmix1_iHystThHigh

	{0x002A, 0x0838, WORD_LEN},
	{0x0F12, 0x003C, WORD_LEN},	//TVAR_afit_pBaseValS[58] //AFIT8_ccm_oscar_iSaturation[7:0],   AFIT8_RGB2YUV_iYOffset [15:8]

	{0x002A, 0x0908, WORD_LEN},
	{0x0F12, 0x0046, WORD_LEN},	//TVAR_afit_pBaseValS[162] /AFIT8_ccm_oscar_iSaturation[7:0],   AFIT8_RGB2YUV_iYOffset [15:8]

	//HIGHLUX RECOVER Value
	{0x002a, 0x0B2C, WORD_LEN},
	{0x0F12, 0x003C, WORD_LEN},	//70000B2C //TVAR_afit_pBaseValS[436] /AFIT16_YUV422_DENOISE_iUVLowThresh
	{0x0F12, 0x0050, WORD_LEN},	//70000B2E //TVAR_afit_pBaseValS[437] /AFIT16_YUV422_DENOISE_iUVHighThresh

	{0x002a, 0x0B50, WORD_LEN},
	{0x0F12, 0x012D, WORD_LEN},	//70000B50 //TVAR_afit_pBaseValS[454] /AFIT8_sddd8a_iRadialLimit [7:0],   AFIT8_sddd8a_iRadialPower [15:8] 

	{0x002a, 0x0B84, WORD_LEN},
	{0x0F12, 0x210C, WORD_LEN},	//70000B84 //TVAR_afit_pBaseValS[480] /AFIT8_sddd8a_iDenThreshHigh[7:0],   AFIT8_Demosaicing_iEdgeDesat [15:8]


	{0x002A, 0x023C, WORD_LEN}, //Normal Preview//
	{0x0F12, 0x0000, WORD_LEN}, //config 0 //
	{0x002A, 0x0240, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	{0x002A, 0x0230, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	{0x002A, 0x023E, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	{0x002A, 0x0220, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	{0x002A, 0x0222, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	
	{0xFFFF, 0xFFFF, WORD_LEN},	


    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_set_capture_s5k5caga[] = {

	{0xFCFC, 0xD000, WORD_LEN},
	{0x0028, 0x7000, WORD_LEN},

	{0x002A, 0x07E6, WORD_LEN},
	{0x0F12, 0x0020, WORD_LEN},	//TVAR_afit_pBaseValS[17] //AFIT16_demsharpmix1_iHystThLow                                                                     
	{0x0F12, 0x0020, WORD_LEN},	//TVAR_afit_pBaseValS[18] //AFIT16_demsharpmix1_iHystThHigh

	{0x002A, 0x08B6, WORD_LEN},
	{0x0F12, 0x000A, WORD_LEN},	//700008B6 //TVAR_afit_pBaseValS[121] /AFIT16_demsharpmix1_iHystThLow
	{0x0F12, 0x0005, WORD_LEN},	//700008B8 //TVAR_afit_pBaseValS[122] /AFIT16_demsharpmix1_iHystThHigh

	{0x002A, 0x0986, WORD_LEN},
	{0x0F12, 0x0005, WORD_LEN},	//70000986 //TVAR_afit_pBaseValS[225] /AFIT16_demsharpmix1_iHystThLow
	{0x0F12, 0x0005, WORD_LEN},	//70000988 //TVAR_afit_pBaseValS[226] /AFIT16_demsharpmix1_iHystThHigh
    
	{0x002A, 0x0A56, WORD_LEN},
	{0x0F12, 0x0005, WORD_LEN},	//70000A56 //TVAR_afit_pBaseValS[329] /AFIT16_demsharpmix1_iHystThLow
	{0x0F12, 0x0005, WORD_LEN},	//70000A58 //TVAR_afit_pBaseValS[330] /AFIT16_demsharpmix1_iHystThHigh

	{0x002A, 0x0244, WORD_LEN}, //Normal capture//
	{0x0F12, 0x0000, WORD_LEN}, //config 0 //
	{0x002A, 0x0230, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	{0x002A, 0x0246, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	{0x002A, 0x0224, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},
	{0x0F12, 0x0001, WORD_LEN},

	//p160	//Delay 160ms
	{S5K5CAGA_REG_TERM, 0x00A0, WORD_LEN},

	{0xFFFF, 0xFFFF, WORD_LEN},	

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

 struct sensor_reg* sensor_reg_common_s5k5caga[3] =
{
	//sensor_initialize_s5k5caga_max30fps,
	sensor_initialize_s5k5caga,
	sensor_set_preview_s5k5caga,
	sensor_set_capture_s5k5caga
};

static struct sensor_reg sensor_brightness_m1_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_m2_s5k5caga[] = {
	
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_m3_s5k5caga[] = {
	
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_m4_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_m5_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_0_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_p1_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_p2_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_p3_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_p4_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_p5_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_brightness_s5k5caga[5] =
{
	sensor_brightness_m5_s5k5caga,
	sensor_brightness_m4_s5k5caga,
	sensor_brightness_m3_s5k5caga,
	sensor_brightness_m2_s5k5caga,
	sensor_brightness_m1_s5k5caga,
	sensor_brightness_0_s5k5caga,
	sensor_brightness_p1_s5k5caga,
	sensor_brightness_p2_s5k5caga,
	sensor_brightness_p3_s5k5caga,
	sensor_brightness_p4_s5k5caga,
	sensor_brightness_p5_s5k5caga
};


static struct sensor_reg sensor_awb_auto_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_daylight_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_incandescent_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_fluorescent_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_cloudy_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_sunset_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_awb_s5k5caga[6] =
{
	sensor_awb_auto_s5k5caga,
	sensor_awb_daylight_s5k5caga,
	sensor_awb_incandescent_s5k5caga,
	sensor_awb_fluorescent_s5k5caga,
	sensor_awb_cloudy_s5k5caga,
	sensor_awb_sunset_s5k5caga
	
};


static struct sensor_reg sensor_iso_auto_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_iso_100_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_iso_200_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_iso_400_s5k5caga[] = {
	{S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_iso_s5k5caga[4] =
{
	sensor_iso_auto_s5k5caga,
	sensor_iso_100_s5k5caga,
	sensor_iso_200_s5k5caga,
	sensor_iso_400_s5k5caga
};


static struct sensor_reg sensor_effect_normal_s5k5caga[] = {
	
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_gray_s5k5caga[] = {
	
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_negative_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_sepia_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_sharpness_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_sketch_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_effect_s5k5caga[6] =
{
	sensor_effect_normal_s5k5caga,
	sensor_effect_gray_s5k5caga,
	sensor_effect_negative_s5k5caga,
	sensor_effect_sepia_s5k5caga,
	sensor_effect_sharpness_s5k5caga,
	sensor_effect_sketch_s5k5caga,
};


static struct sensor_reg sensor_reg_flipnone_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_reg_hflip_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_reg_vflip_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_reg_hvflip_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_flip_s5k5caga[4] =
{
	sensor_reg_flipnone_s5k5caga,
	sensor_reg_hflip_s5k5caga,
	sensor_reg_vflip_s5k5caga,
	sensor_reg_hvflip_s5k5caga,
};


static struct sensor_reg sensor_secne_auto_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_night_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_landscape_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_portrait_s5k5caga[] = {
	{S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_sport_s5k5caga[] = {
	{S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_scene_s5k5caga[5] =
{
	sensor_secne_auto_s5k5caga,
	sensor_secne_night_s5k5caga,
	sensor_secne_landscape_s5k5caga,
	sensor_secne_portrait_s5k5caga,
	sensor_secne_sport_s5k5caga
};

static struct sensor_reg sensor_me_mtrix_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_me_center_weighted_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_me_spot_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_metering_exposure_s5k5caga[3] =
{
	sensor_me_mtrix_s5k5caga,
	sensor_me_center_weighted_s5k5caga,
	sensor_me_spot_s5k5caga
};

static struct sensor_reg sensor_af_single_s5k5caga[] = {

    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_af_manual_s5k5caga[] = {
    {S5K5CAGA_REG_TERM, S5K5CAGA_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_af_s5k5caga[2] =
{
	sensor_af_single_s5k5caga,
	sensor_af_manual_s5k5caga
};

static int write_regs_s5k5caga(const struct sensor_reg reglist[])
{
	int err, cnt;
	int err_cnt = 0;	
	int sleep_cnt = 100;
	unsigned char data[2000];
	unsigned short bytes;
	int burst_cnt;
	const struct sensor_reg *next = reglist;

	while (!((next->reg == S5K5CAGA_REG_TERM) && (next->val == S5K5CAGA_VAL_TERM)))
	{
		
		if(next->reg == S5K5CAGA_REG_TERM && next->val != S5K5CAGA_VAL_TERM)
		{
			//mdelay(next->val);
			msleep(next->val);
			sleep_cnt = 100;
			printk("Sensor init Delay[%d]!!!! \n", next->val);
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
			#ifdef ENABLE_BURST_MODE
				if(next->reg == 0x0F12){				
					bytes = 0;
					burst_cnt =0;
					data[bytes]= next->reg>>8;		bytes++;		
					data[bytes]= (u8)next->reg&0xff; 	bytes++;

					data[bytes]= next->val>>8;		bytes++;		
					data[bytes]= (u8)next->val&0xff; 	bytes++;
					burst_cnt += 4;

					while(1){
						next++;

						if(next->reg == 0x0F12){
							data[bytes]= next->val>>8;		bytes++;		
							data[bytes]= (u8)next->val&0xff;		bytes++;
							burst_cnt += 2;
						}
						else{
							//printk("burst_cnt[%d]\n", burst_cnt);
							next--;
							break;
						}
					}					
				
					err = DDI_I2C_Write(data, 2, bytes-2);
							
				}
				else{					
					bytes = 0;
					data[bytes]= next->reg>>8;		bytes++;		
					data[bytes]= (u8)next->reg&0xff; 	bytes++;

					data[bytes]= next->val>>8;		bytes++;		
					data[bytes]= (u8)next->val&0xff; 	bytes++;
					
					err = DDI_I2C_Write(data, 2, bytes-2);					
				}		
			#else
				if(next->width == WORD_LEN){
					bytes = 0;
					data[bytes]= next->reg>>8;		bytes++;		
					data[bytes]= (u8)next->reg&0xff; 	bytes++;

					data[bytes]= next->val>>8;		bytes++;		
				data[bytes]= (u8)next->val&0xff; 	bytes++;

				err = DDI_I2C_Write(data, 2, bytes-2);			
			}
			else{
				bytes = 0;
				data[bytes]= next->reg>>8;		bytes++;		
				data[bytes]= (u8)next->reg&0xff; 	bytes++;

					//data[bytes]= next->val;		bytes++;		
					data[bytes]= (u8)next->val&0xff; 	bytes++;

					err = DDI_I2C_Write(data, 2, bytes-2);				
				}
			#endif
			
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

static int sensor_open_s5k5caga(bool bChangeCamera)
{

	unsigned char data_r[2];
	unsigned short data_read;
	unsigned short input_reg = 0x0000;
	
	if(bChangeCamera){
		CIF_Open();
		sensor_delay(40);
		
		sensor_reset_low();
		
		sensor_powerdown_disable();
		msleep(10);

		sensor_reset_high();
		sensor_delay(10);
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

	#if 0
		DDI_I2C_Read(input_reg, 2, data_r, 2);
		data_read = data_r[1];
		data_read = (data_read<<8)&0xff00 | data_r[0];

		printk("data_read = 0x%x \n", data_read);
	#endif

	return write_regs_s5k5caga(sensor_reg_common_s5k5caga[0]);
}

static int sensor_close_s5k5caga(void)
{
	unsigned char data_r[2];
	unsigned short data_read;
	unsigned short input_reg = 0x0000;
	
	DDI_I2C_Read(input_reg, 2, data_r, 2);
	data_read = data_r[1];
	data_read = (data_read<<8)&0xff00 | data_r[0];

	//printk("close! data_read = 0x%x \n", data_read);

	CIF_ONOFF(OFF);

	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_disable();

	CIF_Close();
	msleep(5);

	return 0;
}

static int sensor_powerdown_s5k5caga(bool bChangeCamera)
{
	unsigned char data_r[2];
	unsigned short data_read;
	unsigned short input_reg = 0x0000;

 
	if(bChangeCamera){

		sensor_powerdown_disable();
		sensor_delay(10);

		sensor_reset_low();
		sensor_delay(10);

		CIF_Open();
		sensor_delay(40);

		sensor_reset_high();
		sensor_delay(15);
	
		DDI_I2C_Read(input_reg, 2, data_r, 2);
		data_read = data_r[1];
		data_read = (data_read<<8)&0xff00 | data_r[0];

		printk("powerdown! data_read = 0x%x \n", data_read);

		sensor_powerdown_enable();
		msleep(10);

	
		//printk("s5k5caga change camera powerdown! \n");
	}
	else{		
		sensor_reset_low();
		sensor_power_disable();
		sensor_powerdown_disable();
			
		//printk("s5k5caga first/last camera powerdown! \n");
	}

	return 0;
}

static int sensor_preview_s5k5caga(void)
{
	return write_regs_s5k5caga(sensor_reg_common_s5k5caga[1]);
}

static int sensor_capture_s5k5caga(void)
{
	return write_regs_s5k5caga(sensor_reg_common_s5k5caga[2]);
}

static int sensor_capturecfg_s5k5caga(int width, int height)
{
	return 0;
}

static int sensor_zoom_s5k5caga(int val)
{
	return 0;
}

static int sensor_autofocus_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_af_s5k5caga[val]);
}

static int sensor_effect_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_effect_s5k5caga[val]);
}

static int sensor_flip_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_flip_s5k5caga[val]);
}

static int sensor_iso_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_iso_s5k5caga[val]);
}

static int sensor_me_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_metering_exposure_s5k5caga[val]);
}

static int sensor_wb_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_awb_s5k5caga[val]);
}

static int sensor_bright_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_brightness_s5k5caga[val]);
}

static int sensor_scene_s5k5caga(int val)
{
	return write_regs_s5k5caga(sensor_reg_scene_s5k5caga[val]);
}

static int sensor_check_esd_s5k5caga(int val)
{
	return 0;
}

static int sensor_check_luma_s5k5caga(int val)
{
	return 0;
}

/**********************************************************
*    Function  
**********************************************************/
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void sensor_info_init_s5k5caga(TCC_SENSOR_INFO_TYPE *sensor_info)
{
	sensor_info->i2c_addr 				= 0x78;
	sensor_info->reg_term 				= 0x0000;
	sensor_info->val_term 				= 0x0000;
	#if defined(CONFIG_ARCH_TCC92XX)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 1440000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#elif defined(CONFIG_ARCH_TCC93XX)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 1680000;
	sensor_info->s_clock_source 			= PCDIRECTPLL1;
	#else // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 1680000;
	sensor_info->s_clock_source 			= PCDIRECTPLL1;
	#endif// CONFIG_USE_ISP
	#elif defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 1440000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else  // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 1440000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#endif  // CONFIG_USE_ISP
	#endif
	sensor_info->preview_w 				= 640;
	sensor_info->preview_h 				= 480;
	sensor_info->preview_zoom_offset_x 	= 10;
	sensor_info->preview_zoom_offset_y 	= 8;
	sensor_info->capture_w 				= 2048;
	sensor_info->capture_h 				= 1536;
	sensor_info->capture_zoom_offset_x 	= 32;
	sensor_info->capture_zoom_offset_y 	= 24;
	sensor_info->max_zoom_step 			= 15;
	sensor_info->cam_capchg_width 		= 640;
	sensor_info->framerate				= 15;
	sensor_info->p_clock_pol 				= POSITIVE_EDGE;//NEGATIVE_EDGE;//
	sensor_info->v_sync_pol 				= ACT_HIGH;
	sensor_info->h_sync_pol                 = ACT_HIGH;
	#if defined(CONFIG_ARCH_TCC892X)
	sensor_info->de_pol                 = ACT_LOW;
	#endif	
	sensor_info->format 					= M420_ZERO;
	sensor_info->capture_skip_frame 		= 1;
	sensor_info->sensor_sizes 			= sensor_sizes_s5k5caga;
}

void sensor_init_fnc_s5k5caga(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_s5k5caga;
	sensor_func->Close 					= sensor_close_s5k5caga;
	sensor_func->PowerDown				= sensor_powerdown_s5k5caga;

	sensor_func->Set_Preview 			= sensor_preview_s5k5caga;
	sensor_func->Set_Capture 			= sensor_capture_s5k5caga;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_s5k5caga;

	sensor_func->Set_Zoom 				= sensor_zoom_s5k5caga;
	sensor_func->Set_AF 				= sensor_autofocus_s5k5caga;
	sensor_func->Set_Effect 				= sensor_effect_s5k5caga;
	sensor_func->Set_Flip 				= sensor_flip_s5k5caga;
	sensor_func->Set_ISO 				= sensor_iso_s5k5caga;
	sensor_func->Set_ME 				= sensor_me_s5k5caga;
	sensor_func->Set_WB 				= sensor_wb_s5k5caga;
	sensor_func->Set_Bright 				= sensor_bright_s5k5caga;
	sensor_func->Set_Scene 				= sensor_scene_s5k5caga;

	sensor_func->Check_ESD 				= sensor_check_esd_s5k5caga;
	sensor_func->Check_Luma 			= sensor_check_luma_s5k5caga;
}
#else
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_s5k5caga;
	sensor_func->Close 					= sensor_close_s5k5caga;
	sensor_func->PowerDown				= sensor_powerdown_s5k5caga;

	sensor_func->Set_Preview 			= sensor_preview_s5k5caga;
	sensor_func->Set_Capture 			= sensor_capture_s5k5caga;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_s5k5caga;

	sensor_func->Set_Zoom 				= sensor_zoom_s5k5caga;
	sensor_func->Set_AF 				= sensor_autofocus_s5k5caga;
	sensor_func->Set_Effect 				= sensor_effect_s5k5caga;
	sensor_func->Set_Flip 				= sensor_flip_s5k5caga;
	sensor_func->Set_ISO 				= sensor_iso_s5k5caga;
	sensor_func->Set_ME 				= sensor_me_s5k5caga;
	sensor_func->Set_WB 				= sensor_wb_s5k5caga;
	sensor_func->Set_Bright 				= sensor_bright_s5k5caga;
	sensor_func->Set_Scene 				= sensor_scene_s5k5caga;

	sensor_func->Check_ESD 				= sensor_check_esd_s5k5caga;
	sensor_func->Check_Luma 			= sensor_check_luma_s5k5caga;
}
#endif
#endif

