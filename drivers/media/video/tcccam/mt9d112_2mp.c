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
#include <asm/mach-types.h>

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_MT9D112

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
struct capture_size sensor_sizes_mt9d112[] = {
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
static struct sensor_reg sensor_initialize_mt9d112[] = {
	{0x3216, 0x0008},
	{0x341E, 0x8F09},
#if 0 //27.5fps
	{0x341C, 0x0250},
#else
	{0x341C, 0x0120},
#endif
	{0x341E, 0x8F09},
	{0x341E, 0x8F08},
	{0x301A, 0x0ACC},
	{0x3202, 0x0008},
	{0x33F4, 0x031D},
	{MT9D112_REG_TERM, 0x0064}, 	//delay 200ms //20110901 ysseung   modify to delay time.  200ms -> 100ms.

	{0x3044, 0x0540}, //Reserved = 0x540
	{0x3216, 0x02CF}, //Internal Clock Control = 0x2CF
	{0x321C, 0x0402}, //OF Control Status = 0x402
	{0x3212, 0x0001}, //Factory Bypass = 0x1

#if 0 //27.5 fps
	{0x338C, 0x2703},	//Output Width (A)
	{0x3390, 0x0320},	//      = 800
	{0x338C, 0x2705},	//Output Height (A)
	{0x3390, 0x0258},	//      = 600
	{0x338C, 0x2707},	//Output Width (B)
	{0x3390, 0x0640},	//      = 1600
	{0x338C, 0x2709},	//Output Height (B)
	{0x3390, 0x04B0},	//      = 1200
	{0x338C, 0x270D},	//Row Start (A)
	{0x3390, 0x0000},	//      = 0
	{0x338C, 0x270F},	//Column Start (A)
	{0x3390, 0x0000},	//      = 0
	{0x338C, 0x2711},	//Row End (A)
	{0x3390, 0x04BD},	//      = 1213
	{0x338C, 0x2713},	//Column End (A)
	{0x3390, 0x064D},	//      = 1613
	{0x338C, 0x2715},	//Extra Delay (A)
	{0x3390, 0x07CD},	//      = 1644
	{0x338C, 0x2717},	//Row Speed (A)
	{0x3390, 0x2111},	//      = 8465
	{0x338C, 0x2719},	//Read Mode (A)
	{0x3390, 0x046C},	//      = 1132
	{0x338C, 0x271B},	//sensor_sample_time_pck (A)
	{0x3390, 0x024F},	//      = 591
	{0x338C, 0x271D},	//sensor_fine_correction (A)
	{0x3390, 0x0102},	//      = 258
	{0x338C, 0x271F},	//sensor_fine_IT_min (A)
	{0x3390, 0x0279},	//      = 633
	{0x338C, 0x2721},	//sensor_fine_IT_max_margin (A)
	{0x3390, 0x0155},	//      = 341
	{0x338C, 0x2723},	//Frame Lines (A)
	{0x3390, 0x02B9},	//      = 659
	{0x338C, 0x2725},	//Line Length (A)
	{0x3390, 0x0824},	//      = 2084
	{0x338C, 0x2727},	//sensor_dac_id_4_5 (A)
	{0x3390, 0x2020},	//      = 8224
	{0x338C, 0x2729},	//sensor_dac_id_6_7 (A)
	{0x3390, 0x2020},	//      = 8224
	{0x338C, 0x272B},	//sensor_dac_id_8_9 (A)
	{0x3390, 0x1020},	//      = 4128
	{0x338C, 0x272D},	//sensor_dac_id_10_11 (A)
	{0x3390, 0x2007},	//      = 8199
	{0x338C, 0x272F},	//Row Start (B)
	{0x3390, 0x0004},	//      = 4
	{0x338C, 0x2731},	//Column Start (B)
	{0x3390, 0x0004},	//      = 4
	{0x338C, 0x2733},	//Row End (B)
	{0x3390, 0x04BB},	//      = 1211
	{0x338C, 0x2735},	//Column End (B)
	{0x3390, 0x064B},	//      = 1611
	{0x338C, 0x2737},	//Extra Delay (B)
	{0x3390, 0x04F3},	//      = 1490
	{0x338C, 0x2739},	//Row Speed (B)
	{0x3390, 0x2111},	//      = 8465
	{0x338C, 0x273B},	//Read Mode (B)
	{0x3390, 0x0024},	//      = 36
	{0x338C, 0x273D},	//sensor_sample_time_pck (B)
	{0x3390, 0x0120},	//      = 288
	{0x338C, 0x273F},	//sensor_fine_correction (B)
	{0x3390, 0x00A4},	//      = 164
	{0x338C, 0x2741},	//sensor_fine_IT_min (B)
	{0x3390, 0x0169},	//      = 361
	{0x338C, 0x2743},	//sensor_fine_IT_max_margin (B)
	{0x3390, 0x00A4},	//      = 164
	{0x338C, 0x2745},	//Frame Lines (B)
	{0x3390, 0x051E},	//      = 2478
	{0x338C, 0x2747},	//Line Length (B)
	{0x3390, 0x0824},	//      = 2084
	{0x338C, 0x2751},	//Crop_X0 (A)
	{0x3390, 0x0000},	//      = 0
	{0x338C, 0x2753},	//Crop_X1 (A)
	{0x3390, 0x0320},	//      = 800
	{0x338C, 0x2755},	//Crop_Y0 (A)
	{0x3390, 0x0000},	//      = 0
	{0x338C, 0x2757},	//Crop_Y1 (A)
	{0x3390, 0x0258},	//      = 600
	{0x338C, 0x275F},	//Crop_X0 (B)
	{0x3390, 0x0000},	//      = 0
	{0x338C, 0x2761},	//Crop_X1 (B)
	{0x3390, 0x0640},	//      = 1600
	{0x338C, 0x2763},	//Crop_Y0 (B)
	{0x3390, 0x0000},	//      = 0
	{0x338C, 0x2765},	//Crop_Y1 (B)
	{0x3390, 0x04B0},	//      = 1200
	{0x338C, 0x222E},	//R9 Step
	{0x3390, 0x00A0},	//      = 151
	{0x338C, 0xA408},	//search_f1_50
	{0x3390, 0x0026},	//      = 36
	{0x338C, 0xA409},	//search_f2_50
	{0x3390, 0x0029},	//      = 39
	{0x338C, 0xA40A},	//search_f1_60
	{0x3390, 0x002E},	//      = 43
	{0x338C, 0xA40B},	//search_f2_60
	{0x3390, 0x0031},	//      = 46
	{0x338C, 0x2411},	//R9_Step_60 (A)
	{0x3390, 0x00A0},	//      = 151
	{0x338C, 0x2413},	//R9_Step_50 (A)
	{0x3390, 0x00C0},	//      = 181
	{0x338C, 0x2415},	//R9_Step_60 (B)
	{0x3390, 0x00A0},	//      = 151
	{0x338C, 0x2417},	//R9_Step_50 (B)
	{0x3390, 0x00C0},	//      = 181
	{0x338C, 0xA40D},	//Stat_min
	{0x3390, 0x0002},	//      = 2
	{0x338C, 0xA410},	//Min_amplitude
	{0x3390, 0x0001},	//      = 1
	{0x338C, 0xA103},	//Refresh Sequencer Mode
	{0x3390, 0x0006},	//      = 6
	//delay
	{MT9D112_REG_TERM, 0x0010},

	{0x338C, 0xA103},	//Refresh Sequencer
	{0x3390, 0x0005},	//      = 5
	//delay
	{MT9D112_REG_TERM, 0x0010},
//	{MT9D112_REG_TERM, MT9D112_VAL_TERM},
#else
	{0x338C, 0x2703},  //Output Width (A)
	{0x3390, 0x0320},  //      = 800
	{0x338C, 0x2705},  //Output Height (A)
	{0x3390, 0x0258},  //      = 600
	{0x338C, 0x2707}, //Output Width (B)
	{0x3390, 0x0640}, //      = 1600
	{0x338C, 0x2709}, //Output Height (B)
	{0x3390, 0x04B2}, //      = 1202
	{0x338C, 0x270D}, //Row Start (A)
	{0x3390, 0x0000}, //      = 0
	{0x338C, 0x270F}, //Column Start (A)
	{0x3390, 0x0000}, //      = 0
	{0x338C, 0x2711}, //Row End (A)
	{0x3390, 0x04BD}, //      = 1213
	{0x338C, 0x2713}, //Column End (A)
	{0x3390, 0x064D}, //      = 1613
	{0x338C, 0x2715}, //Extra Delay (A)
	{0x3390, 0x030B}, //      = 779
	{0x338C, 0x2717}, //Row Speed (A)
	{0x3390, 0x2111}, //      = 8465
	{0x338C, 0x2719}, //Read Mode (A)
	{0x3390, 0x046C}, //      = 1132
	{0x338C, 0x271B}, //sensor_sample_time_pck (A)
	{0x3390, 0x024F}, //      = 591
	{0x338C, 0x271D}, //sensor_fine_correction (A)
	{0x3390, 0x0102}, //      = 258
	{0x338C, 0x271F}, //sensor_fine_IT_min (A)
	{0x3390, 0x0279}, //      = 633
	{0x338C, 0x2721}, //sensor_fine_IT_max_margin (A)
	{0x3390, 0x0155}, //      = 341
	{0x338C, 0x2723}, //Frame Lines (A)
	{0x3390, 0x02EA}, //      = 746
	{0x338C, 0x2725}, //Line Length (A)
	{0x3390, 0x0824}, //      = 2084
	{0x338C, 0x2727}, //sensor_dac_id_4_5 (A)
	{0x3390, 0x2020}, //      = 8224
	{0x338C, 0x2729}, //sensor_dac_id_6_7 (A)
	{0x3390, 0x2020}, //      = 8224
	{0x338C, 0x272B}, //sensor_dac_id_8_9 (A)
	{0x3390, 0x1020}, //      = 4128
	{0x338C, 0x272D}, //sensor_dac_id_10_11 (A)
	{0x3390, 0x2007}, //      = 8199
	{0x338C, 0x272F}, //Row Start (B)
	{0x3390, 0x0004}, //      = 4
	{0x338C, 0x2731}, //Column Start (B)
	{0x3390, 0x0004}, //      = 4
	{0x338C, 0x2733}, //Row End (B)
	{0x3390, 0x04BD}, //      = 1211 + ?
	{0x338C, 0x2735}, //Column End (B)
	{0x3390, 0x064B}, //      = 1611
	{0x338C, 0x2737}, //Extra Delay (B)
	{0x3390, 0x04C6}, //      = 1222
	{0x338C, 0x2739}, //Row Speed (B)
	{0x3390, 0x2111}, //      = 8465
	{0x338C, 0x273B}, //Read Mode (B)
	{0x3390, 0x0024}, //      = 36
	{0x338C, 0x273D}, //sensor_sample_time_pck (B)
	{0x3390, 0x0120}, //      = 288
	{0x338C, 0x273F}, //sensor_fine_correction (B)
	{0x3390, 0x00A4}, //      = 164
	{0x338C, 0x2741}, //sensor_fine_IT_min (B)
	{0x3390, 0x0169}, //      = 330 + ?
	{0x338C, 0x2743}, //sensor_fine_IT_max_margin (B)
	{0x3390, 0x00A4}, //      = 164
	{0x338C, 0x2745}, //Frame Lines (B)
	{0x3390, 0x0573}, //      = 1395
	{0x338C, 0x2747}, //Line Length (B)
	{0x3390, 0x0824}, //      = 2084
	{0x338C, 0x2751}, //Crop_X0 (A)
	{0x3390, 0x0000}, //      = 0
	{0x338C, 0x2753}, //Crop_X1 (A)
	{0x3390, 0x0320}, //      = 800
	{0x338C, 0x2755}, //Crop_Y0 (A)
	{0x3390, 0x0000}, //      = 0
	{0x338C, 0x2757}, //Crop_Y1 (A)
	{0x3390, 0x0258}, //      = 600
	{0x338C, 0x275F}, //Crop_X0 (B)
	{0x3390, 0x0000}, //      = 0
	{0x338C, 0x2761}, //Crop_X1 (B)
	{0x3390, 0x0640}, //      = 1600
	{0x338C, 0x2763}, //Crop_Y0 (B)
	{0x3390, 0x0000}, //      = 0
	{0x338C, 0x2765}, //Crop_Y1 (B)
	{0x3390, 0x04B2}, //      = 1202
	{0x338C, 0x222E}, //R9 Step
	{0x3390, 0x0062}, //      = 98
	{0x338C, 0xA408}, //search_f1_50
	{0x3390, 0x0011}, //      = 17
	{0x338C, 0xA409}, //search_f2_50
	{0x3390, 0x0014}, //      = 20
	{0x338C, 0xA40A}, //search_f1_60
	{0x3390, 0x0015}, //      = 21
	{0x338C, 0xA40B}, //search_f2_60
	{0x3390, 0x0018}, //      = 24
	{0x338C, 0x2411}, //R9_Step_60_A
	{0x3390, 0x0062}, //      = 98
	{0x338C, 0x2413}, //R9_Step_50_A
	{0x3390, 0x0076}, //      = 118
	{0x338C, 0x2415}, //R9_Step_60_B
	{0x3390, 0x0062}, //      = 98
	{0x338C, 0x2417}, //R9_Step_50_B
	{0x3390, 0x0076}, //      = 118
	{0x338C, 0xA40D}, //Stat_min
	{0x3390, 0x0002}, //      = 2
	{0x338C, 0xA410}, //Min_amplitude
	{0x3390, 0x0001}, //      = 1
	{0x338C, 0xA404}, // MCU_ADDRESS
	{0x3390, 0x0000}, // FD_MODE
	{0x338C, 0xA20C}, // MCU_ADDRESS
	{0x3390, 0x000C}, // AE_MAX_INDEX
#endif

	{0x338C, 0xA215}, // MCU_ADDRESS
	{0x3390, 0x0008}, // AE_INDEX_TH23
	{0x338C, 0xA206}, // MCU_ADDRESS
	{0x3390, 0x0046}, // AE_TARGET
	{0x338C, 0xA207}, // MCU_ADDRESS
	{0x3390, 0x000A}, // AE_GATE
	{0x338C, 0xA202}, // MCU_ADDRESS
	{0x3390, 0x0033}, // AE_WINDOW_POS
	{0x338C, 0xA203}, // MCU_ADDRESS
	{0x3390, 0x0099}, // AE_WINDOW_SIZE
	{0x338C, 0xA217}, // MCU_ADDRESS
	{0x3390, 0x00BF}, // AE_WEIGHTS

	// Sequence
	{0x338C, 0xA13E}, // MCU_ADDRESS
	{0x3390, 0x0004}, // MCU_DATA_0
	{0x338C, 0xA13F}, // MCU_ADDRESS
	{0x3390, 0x000E}, // MCU_DATA_0
	{0x338C, 0xA140}, // MCU_ADDRESS
	{0x3390, 0x0004}, // MCU_DATA_0
	{0x338C, 0xA141}, // MCU_ADDRESS
	{0x3390, 0x0004}, // MCU_DATA_0
	{0x338C, 0xA142}, // MCU_ADDRESS
	{0x3390, 0x0032}, // MCU_DATA_0
	{0x338C, 0xA143}, // MCU_ADDRESS
	{0x3390, 0x000F}, // MCU_DATA_0
	{0x338C, 0xA144}, // MCU_ADDRESS
	{0x3390, 0x0032}, // MCU_DATA_0
	{0x338C, 0xA145}, // MCU_ADDRESS
	{0x3390, 0x0032}, // MCU_DATA_0
	{0x338C, 0xA146}, // MCU_ADDRESS
	{0x3390, 0x0005}, // MCU_DATA_0
	{0x338C, 0xA147}, // MCU_ADDRESS
	{0x3390, 0x003A}, // MCU_DATA_0
	{0x338C, 0xA115}, // MCU_ADDRESS
	{0x3390, 0x006F}, // MCU_DATA_0
	{0x338C, 0xA118}, // MCU_ADDRESS
	{0x3390, 0x0080}, // SEQ_LLSAT1
	{0x338C, 0xA119}, // MCU_ADDRESS
	{0x3390, 0x0060}, // SEQ_LLSAT2
	{0x338C, 0xA11C}, // MCU_ADDRESS
	{0x3390, 0x0003}, // SEQ_LLAPCORR1 : 4 => 3 20080108 edge 3 <- 4
	{0x338C, 0xA11D}, // MCU_ADDRESS
	{0x3390, 0x0002}, // SEQ_LLAPCORR2
	{0x338C, 0xA12C}, // MCU_ADDRESS
	{0x3390, 0x0003}, // SEQ_PREVIEW_1_AF
	{0x338C, 0xA130}, // MCU_ADDRESS
	{0x3390, 0x0000}, // SEQ_PREVIEW_2_AE
	{0x338C, 0xA132}, // MCU_ADDRESS
	{0x3390, 0x0000}, // SEQ_PREVIEW_2_AWB
	{0x338C, 0xA134}, // MCU_ADDRESS
	{0x3390, 0x0000}, // SEQ_PREVIEW_2_HG
	{0x338C, 0xA136}, // MCU_ADDRESS
	{0x3390, 0x0040}, // MCU_DATA_0
	{0x326C, 0x1511}, // APERTURE_PARAMETERS

	// LSC 
	{0x34CE, 0x81A8}, 	// LENS_CORRECTION_CONTROL
	{0x34D0, 0x6432}, 	// ZONE_BOUNDS_X1_X2
	{0x34D2, 0x3296}, 	// ZONE_BOUNDS_X0_X3
	{0x34D4, 0x9664}, 	// ZONE_BOUNDS_X4_X5
	{0x34D6, 0x5028}, 	// ZONE_BOUNDS_Y1_Y2
	{0x34D8, 0x2878}, 	// ZONE_BOUNDS_Y0_Y3
	{0x34DA, 0x7850}, 	// ZONE_BOUNDS_Y4_Y5
	{0x34DC, 0x0000}, 	// CENTER_OFFSET
	{0x34DE, 0x0113}, 	// FX_RED
	{0x34E6, 0x00BB}, 	// FY_RED
	{0x34EE, 0x0955}, 	// DF_DX_RED
	{0x34F6, 0x0B48}, 	// DF_DY_RED
	{0x3500, 0x2249}, 	// SECOND_DERIV_ZONE_0_RED
	{0x3508, 0x1E07}, 	// SECOND_DERIV_ZONE_1_RED
	{0x3510, 0x1C33}, 	// SECOND_DERIV_ZONE_2_RED
	{0x3518, 0x1D54}, 	// SECOND_DERIV_ZONE_3_RED
	{0x3520, 0x2952}, 	// SECOND_DERIV_ZONE_4_RED
	{0x3528, 0x2451}, 	// SECOND_DERIV_ZONE_5_RED
	{0x3530, 0x0D34}, 	// SECOND_DERIV_ZONE_6_RED
	{0x3538, 0x27B2}, 	// SECOND_DERIV_ZONE_7_RED
	{0x354C, 0x0045}, 	// K_FACTOR_IN_K_FX_FY_R_TL
	{0x3544, 0x0415}, 	// K_FACTOR_IN_K_FX_FY_R_TR
	{0x355C, 0x00A5}, 	// K_FACTOR_IN_K_FX_FY_R_BL
	{0x3554, 0x0449}, 	// K_FACTOR_IN_K_FX_FY_R_BR
	{0x34E0, 0x010C}, 	// FX_GREEN
	{0x34E8, 0x0095}, 	// FY_GREEN
	{0x34F0, 0x0B02}, 	// DF_DX_GREEN
	{0x34F8, 0x0BB5}, 	// DF_DY_GREEN
	{0x3502, 0x3C22}, 	// SECOND_DERIV_ZONE_0_GREEN
	{0x350A, 0x0B03}, 	// SECOND_DERIV_ZONE_1_GREEN
	{0x3512, 0x1646}, 	// SECOND_DERIV_ZONE_2_GREEN
	{0x351A, 0x193F}, 	// SECOND_DERIV_ZONE_3_GREEN
	{0x3522, 0x2142}, 	// SECOND_DERIV_ZONE_4_GREEN
	{0x352A, 0x1A21}, 	// SECOND_DERIV_ZONE_5_GREEN
	{0x3532, 0x092A}, 	// SECOND_DERIV_ZONE_6_GREEN
	{0x353A, 0x3E71}, 	// SECOND_DERIV_ZONE_7_GREEN
	{0x354E, 0x0439}, 	// K_FACTOR_IN_K_FX_FY_G1_TL
	{0x3546, 0x00F3}, 	// K_FACTOR_IN_K_FX_FY_G1_TR
	{0x355E, 0x0025}, 	// K_FACTOR_IN_K_FX_FY_G1_BL
	{0x3556, 0x0173}, 	// K_FACTOR_IN_K_FX_FY_G1_BR
	{0x34E4, 0x00D9}, 	// FX_BLUE
	{0x34EC, 0x006A}, 	// FY_BLUE
	{0x34F4, 0x0B40}, 	// DF_DX_BLUE
	{0x34FC, 0x0C75}, 	// DF_DY_BLUE
	{0x3506, 0x3A30}, 	// SECOND_DERIV_ZONE_0_BLUE
	{0x350E, 0x08FD}, 	// SECOND_DERIV_ZONE_1_BLUE
	{0x3516, 0x152F}, 	// SECOND_DERIV_ZONE_2_BLUE
	{0x351E, 0x1236}, 	// SECOND_DERIV_ZONE_3_BLUE
	{0x3526, 0x1432}, 	// SECOND_DERIV_ZONE_4_BLUE
	{0x352E, 0x1011}, 	// SECOND_DERIV_ZONE_5_BLUE
	{0x3536, 0x153B}, 	// SECOND_DERIV_ZONE_6_BLUE
	{0x353E, 0x3116}, 	// SECOND_DERIV_ZONE_7_BLUE
	{0x3552, 0x00A0}, 	// K_FACTOR_IN_K_FX_FY_B_TL
	{0x354A, 0x02BB}, 	// K_FACTOR_IN_K_FX_FY_B_TR
	{0x3562, 0x0140}, 	// K_FACTOR_IN_K_FX_FY_B_BL
	{0x355A, 0x0238}, 	// K_FACTOR_IN_K_FX_FY_B_BR
	{0x34E2, 0x0111}, 	// FX_GREEN2
	{0x34EA, 0x0085}, 	// FY_GREEN2
	{0x34F2, 0x094C}, 	// DF_DX_GREEN2
	{0x34FA, 0x0C02}, 	// DF_DY_GREEN2
	{0x3504, 0x364A}, 	// SECOND_DERIV_ZONE_0_GREEN2
	{0x350C, 0x0C08}, 	// SECOND_DERIV_ZONE_1_GREEN2
	{0x3514, 0x1837}, 	// SECOND_DERIV_ZONE_2_GREEN2
	{0x351C, 0x1A46}, 	// SECOND_DERIV_ZONE_3_GREEN2
	{0x3524, 0x1846}, 	// SECOND_DERIV_ZONE_4_GREEN2
	{0x352C, 0x1837}, 	// SECOND_DERIV_ZONE_5_GREEN2
	{0x3534, 0x1127}, 	// SECOND_DERIV_ZONE_6_GREEN2
	{0x353C, 0x3A1D}, 	// SECOND_DERIV_ZONE_7_GREEN2
	{0x3550, 0x0122}, 	// K_FACTOR_IN_K_FX_FY_G2_TL
	{0x3548, 0x0054}, 	// K_FACTOR_IN_K_FX_FY_G2_TR
	{0x3560, 0x0099}, 	// K_FACTOR_IN_K_FX_FY_G2_BL
	{0x3558, 0x0013}, 	// K_FACTOR_IN_K_FX_FY_G2_BR
	{0x3540, 0x0001}, 	// X2_FACTORS
	{0x3542, 0x0000}, 	// GLOBAL_OFFSET_FXY_FUNCTION
	{0x3210, 0x01FC}, 	// COLOR_PIPELINE_CONTROL

	// Gamma (Programmed)
	{0x338C, 0xA76D}, 	// MCU_ADDRESS [MODE_GAM_CONT_A]
	{0x3390, 0x0042}, 	// MCU_DATA_0
	{0x338C, 0xA76E}, 	// MCU_ADDRESS [MODE_GAM_CONT_B]
	{0x3390, 0x0042}, 	// MCU_DATA_0
	{0x338C, 0xAB04}, 	// MCU_ADDRESS [HG_MAX_DLEVEL]
	{0x3390, 0x0018}, 	// MCU_DATA_0
	{0x338C, 0xAB05}, 	// MCU_ADDRESS [HG_PERCENT]
	{0x3390, 0x0002}, 	// MCU_DATA_0

	// AWB & CCM 
	{0x338C, 0x2306}, 	// MCU_ADDRESS [AWB_CCM_L_0]
	{0x3390, 0x02E1}, 	// MCU_DATA_0
	{0x338C, 0x2308}, 	// MCU_ADDRESS [AWB_CCM_L_1]
	{0x3390, 0xFE81}, 	// MCU_DATA_0
	{0x338C, 0x230A}, 	// MCU_ADDRESS [AWB_CCM_L_2]
	{0x3390, 0xFFC9}, 	// MCU_DATA_0
	{0x338C, 0x230C}, 	// MCU_ADDRESS [AWB_CCM_L_3]
	{0x3390, 0xFF64}, 	// MCU_DATA_0
	{0x338C, 0x230E}, 	// MCU_ADDRESS [AWB_CCM_L_4]
	{0x3390, 0x02A9}, 	// MCU_DATA_0
	{0x338C, 0x2310}, 	// MCU_ADDRESS [AWB_CCM_L_5]
	{0x3390, 0xFF41}, 	// MCU_DATA_0
	{0x338C, 0x2312}, 	// MCU_ADDRESS [AWB_CCM_L_6]
	{0x3390, 0xFF77}, 	// MCU_DATA_0
	{0x338C, 0x2314}, 	// MCU_ADDRESS [AWB_CCM_L_7]
	{0x3390, 0xFE15}, 	// MCU_DATA_0
	{0x338C, 0x2316}, 	// MCU_ADDRESS [AWB_CCM_L_8]
	{0x3390, 0x03D7}, 	// MCU_DATA_0
	{0x338C, 0x2318}, 	// MCU_ADDRESS [AWB_CCM_L_9]
	{0x3390, 0x0024}, 	// MCU_DATA_0
	{0x338C, 0x231A}, 	// MCU_ADDRESS [AWB_CCM_L_10]
	{0x3390, 0x003C}, 	// MCU_DATA_0
	{0x338C, 0x231C}, 	// MCU_ADDRESS [AWB_CCM_RL_0]
	{0x3390, 0x0072}, 	// MCU_DATA_0
	{0x338C, 0x231E}, 	// MCU_ADDRESS [AWB_CCM_RL_1]
	{0x3390, 0xFF68}, 	// MCU_DATA_0
	{0x338C, 0x2320}, 	// MCU_ADDRESS [AWB_CCM_RL_2]
	{0x3390, 0x002A}, 	// MCU_DATA_0
	{0x338C, 0x2322}, 	// MCU_ADDRESS [AWB_CCM_RL_3]
	{0x3390, 0x0000}, 	// MCU_DATA_0
	{0x338C, 0x2324}, 	// MCU_ADDRESS [AWB_CCM_RL_4]
	{0x3390, 0x005D}, 	// MCU_DATA_0
	{0x338C, 0x2326}, 	// MCU_ADDRESS [AWB_CCM_RL_5]
	{0x3390, 0xFFBA}, 	// MCU_DATA_0
	{0x338C, 0x2328}, 	// MCU_ADDRESS [AWB_CCM_RL_6]
	{0x3390, 0x005D},		// 94}, 	// MCU_DATA_0
	{0x338C, 0x232A}, 	// MCU_ADDRESS [AWB_CCM_RL_7]
	{0x3390, 0x0104},		//0C0}, 	// MCU_DATA_0
	{0x338C, 0x232C}, 	// MCU_ADDRESS [AWB_CCM_RL_8]
	{0x3390, 0xFE68},		//FE75}, 	// MCU_DATA_0
	{0x338C, 0x232E}, 	// MCU_ADDRESS [AWB_CCM_RL_9]
	{0x3390, 0x001C}, 	// MCU_DATA_0
	{0x338C, 0x2330}, 	// MCU_ADDRESS [AWB_CCM_RL_10]
	{0x3390, 0xFFEC}, 	// MCU_DATA_0
	{0x338C, 0xA348}, 	// MCU_ADDRESS [AWB_GAIN_BUFFER_SPEED]
	{0x3390, 0x0008}, 	// MCU_DATA_0
	{0x338C, 0xA349}, 	// MCU_ADDRESS [AWB_JUMP_DIVISOR]
	{0x3390, 0x0002}, 	// MCU_DATA_0
	{0x338C, 0xA34A}, 	// MCU_ADDRESS [AWB_GAIN_MIN]
	{0x3390, 0x0060}, 	// MCU_DATA_0
	{0x338C, 0xA34B}, 	// MCU_ADDRESS [AWB_GAIN_MAX]
	{0x3390, 0x008C}, 	// MCU_DATA_0
	{0x338C, 0xA34F}, 	// MCU_ADDRESS [AWB_CCM_POSITION_MIN]
	{0x3390, 0x0018}, 	// MCU_DATA_0
	{0x338C, 0xA350}, 	// MCU_ADDRESS [AWB_CCM_POSITION_MAX]
	{0x3390, 0x0076}, 	// MCU_DATA_0
	{0x338C, 0xA352}, 	// MCU_ADDRESS [AWB_SATURATION]
	{0x3390, 0x0080}, 	// MCU_DATA_0
	{0x338C, 0xA353}, 	// MCU_ADDRESS [AWB_MODE]
	{0x3390, 0x0002}, 	// MCU_DATA_0
	{0x338C, 0xA35B}, 	// MCU_ADDRESS [AWB_STEADY_BGAIN_OUT_MIN]
	{0x3390, 0x0078}, 	// MCU_DATA_0
	{0x338C, 0xA35C}, 	// MCU_ADDRESS [AWB_STEADY_BGAIN_OUT_MAX]
	{0x3390, 0x0086}, 	// MCU_DATA_0
	{0x338C, 0xA35D}, 	// MCU_ADDRESS [AWB_STEADY_BGAIN_IN_MIN]
	{0x3390, 0x007E}, 	// MCU_DATA_0
	{0x338C, 0xA35E}, 	// MCU_ADDRESS [AWB_STEADY_BGAIN_IN_MAX]
	{0x3390, 0x0082}, 	// MCU_DATA_0
	{0x338C, 0x235F}, 	// MCU_ADDRESS [AWB_CNT_PXL_TH]
	{0x3390, 0x0040}, 	// MCU_DATA_0
	{0x338C, 0xA361}, 	// MCU_ADDRESS [AWB_TG_MIN0]
	{0x3390, 0x00DB}, 	// MCU_DATA_0
	{0x338C, 0xA362}, 	// MCU_ADDRESS [AWB_TG_MAX0]
	{0x3390, 0x00E7}, 	// MCU_DATA_0
	{0x338C, 0xA302}, 	// MCU_ADDRESS [AWB_WINDOW_POS]
	{0x3390, 0x0000}, 	// MCU_DATA_0
	{0x338C, 0xA303}, 	// MCU_ADDRESS [AWB_WINDOW_SIZE]
	{0x3390, 0x00EF}, 	// MCU_DATA_0
	{0x338C, 0xA364}, 	// MCU_ADDRESS [AWB_KR_L]
	{0x3390, 0x0098}, 	// MCU_DATA_0
	{0x338C, 0xA366}, 	// MCU_ADDRESS [AWB_KB_L]
	{0x3390, 0x0088}, 	// MCU_DATA_0
	{0x338C, 0xA367}, 	// MCU_ADDRESS [AWB_KR_R]
	{0x3390, 0x0072}, 	// MCU_DATA_0
	{0x338C, 0xA369}, 	// MCU_ADDRESS [AWB_KB_R]
	{0x3390, 0x007A}, 	// MCU_DATA_0
	{0x338C, 0xA36B}, 	// MCU_ADDRESS [AWB_EDGETH_MIN]
	{0x3390, 0x0004}, 	// MCU_DATA_0

	// Refresh
	{0x338C, 0xA103}, // MCU_ADDRESS
	{0x3390, 0x0006}, // MCU_DATA_0

	//{MT9D112_REG_TERM, 0x0064}, 	//delay 200ms //20110901 ysseung   remove to delay time.
	//DELAY=200 ms    // OR Wait until state = 3
	//{0xFFFF, 100},
	//{0xFFFF, 100},

	{0x338C, 0xA103}, // MCU_ADDRESS
	{0x3390, 0x0005}, // MCU_DATA_0

	//{MT9D112_REG_TERM, 0x0064}, 	//delay 500ms //20110901 ysseung   remove to delay time.
	//DELAY=500 ms    // OR Wait until state = 3
	//{0xFFFF, 100},
	//{0xFFFF, 100},
	//{0xFFFF, 100},
	//{0xFFFF, 100},
	//{0xFFFF, 100},

	// AF Driver - new 08.03 0~100%
	{0x338C, 0x0400}, // MCU_ADDRESS
	{0x3390, 0x3C3C}, // MCU_DATA_0
	{0x3392, 0x3C3C}, // MCU_DATA_1
	{0x3394, 0x3C3C}, // MCU_DATA_2
	{0x3396, 0xFE10}, // MCU_DATA_3
	{0x3398, 0x50EC}, // MCU_DATA_4
	{0x339A, 0x0CFD}, // MCU_DATA_5
	{0x339C, 0x0300}, // MCU_DATA_6
	{0x339E, 0xFE03}, // MCU_DATA_7
	{0x338C, 0x0410}, // MCU_ADDRESS
	{0x3390, 0x00C6}, // MCU_DATA_0
	{0x3392, 0x81E7}, // MCU_DATA_1
	{0x3394, 0x02FE}, // MCU_DATA_2
	{0x3396, 0x0300}, // MCU_DATA_3
	{0x3398, 0xEE00}, // MCU_DATA_4
	{0x339A, 0xEE00}, // MCU_DATA_5
	{0x339C, 0xAD00}, // MCU_DATA_6
	{0x339E, 0xFE03}, // MCU_DATA_7
	{0x338C, 0x0420}, // MCU_ADDRESS
	{0x3390, 0x00C6}, // MCU_DATA_0
	{0x3392, 0x04E7}, // MCU_DATA_1
	{0x3394, 0x02CC}, // MCU_DATA_2
	{0x3396, 0x0400}, // MCU_DATA_3
	{0x3398, 0xFD03}, // MCU_DATA_4
	{0x339A, 0x02CC}, // MCU_DATA_5
	{0x339C, 0x0485}, // MCU_DATA_6
	{0x339E, 0xFD03}, // MCU_DATA_7
	{0x338C, 0x0430}, // MCU_ADDRESS
	{0x3390, 0x04FE}, // MCU_DATA_0
	{0x3392, 0x0300}, // MCU_DATA_1
	{0x3394, 0x1AEE}, // MCU_DATA_2
	{0x3396, 0x0018}, // MCU_DATA_3
	{0x3398, 0xEC04}, // MCU_DATA_4
	{0x339A, 0xFD03}, // MCU_DATA_5
	{0x339C, 0x061A}, // MCU_DATA_6
	{0x339E, 0xEE00}, // MCU_DATA_7
	{0x338C, 0x0440}, // MCU_ADDRESS
	{0x3390, 0x18EC}, // MCU_DATA_0
	{0x3392, 0x06FD}, // MCU_DATA_1
	{0x3394, 0x0308}, // MCU_DATA_2
	{0x3396, 0xCC03}, // MCU_DATA_3
	{0x3398, 0x02ED}, // MCU_DATA_4
	{0x339A, 0x00CC}, // MCU_DATA_5
	{0x339C, 0x0001}, // MCU_DATA_6
	{0x339E, 0xFD10}, // MCU_DATA_7
	{0x338C, 0x0450}, // MCU_ADDRESS
	{0x3390, 0x7EC6}, // MCU_DATA_0
	{0x3392, 0xD130}, // MCU_DATA_1
	{0x3394, 0xED0A}, // MCU_DATA_2
	{0x3396, 0xCC00}, // MCU_DATA_3
	{0x3398, 0x32ED}, // MCU_DATA_4
	{0x339A, 0x085F}, // MCU_DATA_5
	{0x339C, 0xED06}, // MCU_DATA_6
	{0x339E, 0xED04}, // MCU_DATA_7
	{0x338C, 0x0460}, // MCU_ADDRESS
	{0x3390, 0xED02}, // MCU_DATA_0
	{0x3392, 0xED00}, // MCU_DATA_1
	{0x3394, 0x5CBD}, // MCU_DATA_2
	{0x3396, 0x0545}, // MCU_DATA_3
	{0x3398, 0xCE10}, // MCU_DATA_4
	{0x339A, 0x801C}, // MCU_DATA_5
	{0x339C, 0x3601}, // MCU_DATA_6
	{0x339E, 0x1C35}, // MCU_DATA_7
	{0x338C, 0x0470}, // MCU_ADDRESS
	{0x3390, 0x011D}, // MCU_DATA_0
	{0x3392, 0x3501}, // MCU_DATA_1
	{0x3394, 0xCE10}, // MCU_DATA_2
	{0x3396, 0x001D}, // MCU_DATA_3
	{0x3398, 0x7101}, // MCU_DATA_4
	{0x339A, 0xC6FF}, // MCU_DATA_5
	{0x339C, 0xBD04}, // MCU_DATA_6
	{0x339E, 0x8530}, // MCU_DATA_7
	{0x338C, 0x0480}, // MCU_ADDRESS
	{0x3390, 0xC60C}, // MCU_DATA_0
	{0x3392, 0x3A35}, // MCU_DATA_1
	{0x3394, 0x3937}, // MCU_DATA_2
	{0x3396, 0x308F}, // MCU_DATA_3
	{0x3398, 0xC3FF}, // MCU_DATA_4
	{0x339A, 0xED8F}, // MCU_DATA_5
	{0x339C, 0x35FE}, // MCU_DATA_6
	{0x339E, 0x0300}, // MCU_DATA_7
	{0x338C, 0x0490}, // MCU_ADDRESS
	{0x3390, 0xE603}, // MCU_DATA_0
	{0x3392, 0x30E1}, // MCU_DATA_1
	{0x3394, 0x1322}, // MCU_DATA_2
	{0x3396, 0x035F}, // MCU_DATA_3
	{0x3398, 0x2002}, // MCU_DATA_4
	{0x339A, 0xC601}, // MCU_DATA_5
	{0x339C, 0xE712}, // MCU_DATA_6
	{0x339E, 0xE613}, // MCU_DATA_7
	{0x338C, 0x04A0}, // MCU_ADDRESS
	{0x3390, 0x4FC3}, // MCU_DATA_0
	{0x3392, 0x0002}, // MCU_DATA_1
	{0x3394, 0xED10}, // MCU_DATA_2
	{0x3396, 0xE613}, // MCU_DATA_3
	{0x3398, 0xC1FF}, // MCU_DATA_4
	{0x339A, 0x2614}, // MCU_DATA_5
	{0x339C, 0x5F4F}, // MCU_DATA_6
	{0x339E, 0xED0A}, // MCU_DATA_7
	{0x338C, 0x04B0}, // MCU_ADDRESS
	{0x3390, 0xED08}, // MCU_DATA_0
	{0x3392, 0xED06}, // MCU_DATA_1
	{0x3394, 0xED04}, // MCU_DATA_2
	{0x3396, 0xED02}, // MCU_DATA_3
	{0x3398, 0xED00}, // MCU_DATA_4
	{0x339A, 0x5CBD}, // MCU_DATA_5
	{0x339C, 0x0545}, // MCU_DATA_6
	{0x339E, 0x2040}, // MCU_DATA_7
	{0x338C, 0x04C0}, // MCU_ADDRESS
	{0x3390, 0x6D13}, // MCU_DATA_0
	{0x3392, 0x2617}, // MCU_DATA_1
	{0x3394, 0xCC01}, // MCU_DATA_2
	{0x3396, 0x03ED}, // MCU_DATA_3
	{0x3398, 0x0A5F}, // MCU_DATA_4
	{0x339A, 0x4FED}, // MCU_DATA_5
	{0x339C, 0x08ED}, // MCU_DATA_6
	{0x339E, 0x06ED}, // MCU_DATA_7
	{0x338C, 0x04D0}, // MCU_ADDRESS
	{0x3390, 0x04ED}, // MCU_DATA_0
	{0x3392, 0x02ED}, // MCU_DATA_1
	{0x3394, 0x005C}, // MCU_DATA_2
	{0x3396, 0xBD05}, // MCU_DATA_3
	{0x3398, 0x4520}, // MCU_DATA_4
	{0x339A, 0x25CC}, // MCU_DATA_5
	{0x339C, 0x0103}, // MCU_DATA_6
	{0x339E, 0xA310}, // MCU_DATA_7
	{0x338C, 0x04E0}, // MCU_ADDRESS
	{0x3390, 0xED0A}, // MCU_DATA_0
	{0x3392, 0xEC10}, // MCU_DATA_1
	{0x3394, 0xED08}, // MCU_DATA_2
	{0x3396, 0x5F4F}, // MCU_DATA_3
	{0x3398, 0xED06}, // MCU_DATA_4
	{0x339A, 0xED04}, // MCU_DATA_5
	{0x339C, 0xED02}, // MCU_DATA_6
	{0x339E, 0xED00}, // MCU_DATA_7
	{0x338C, 0x04F0}, // MCU_ADDRESS
	{0x3390, 0x5CBD}, // MCU_DATA_0
	{0x3392, 0x0545}, // MCU_DATA_1
	{0x3394, 0xCE10}, // MCU_DATA_2
	{0x3396, 0x001D}, // MCU_DATA_3
	{0x3398, 0x7101}, // MCU_DATA_4
	{0x339A, 0xCE10}, // MCU_DATA_5
	{0x339C, 0x801D}, // MCU_DATA_6
	{0x339E, 0x3601}, // MCU_DATA_7
	{0x338C, 0x0500}, // MCU_ADDRESS
	{0x3390, 0x30E6}, // MCU_DATA_0
	{0x3392, 0x134F}, // MCU_DATA_1
	{0x3394, 0x175F}, // MCU_DATA_2
	{0x3396, 0xED0E}, // MCU_DATA_3
	{0x3398, 0x18FE}, // MCU_DATA_4
	{0x339A, 0x0300}, // MCU_DATA_5
	{0x339C, 0x18E6}, // MCU_DATA_6
	{0x339E, 0x034F}, // MCU_DATA_7
	{0x338C, 0x0510}, // MCU_ADDRESS
	{0x3390, 0xED0C}, // MCU_DATA_0
	{0x3392, 0xE30E}, // MCU_DATA_1
	{0x3394, 0xCDEE}, // MCU_DATA_2
	{0x3396, 0x0BEE}, // MCU_DATA_3
	{0x3398, 0x04AD}, // MCU_DATA_4
	{0x339A, 0x0030}, // MCU_DATA_5
	{0x339C, 0x6D12}, // MCU_DATA_6
	{0x339E, 0x2604}, // MCU_DATA_7
	{0x338C, 0x0520}, // MCU_ADDRESS
	{0x3390, 0xC602}, // MCU_DATA_0
	{0x3392, 0x2002}, // MCU_DATA_1
	{0x3394, 0xC606}, // MCU_DATA_2
	{0x3396, 0xFE03}, // MCU_DATA_3
	{0x3398, 0x00E7}, // MCU_DATA_4
	{0x339A, 0x05FE}, // MCU_DATA_5
	{0x339C, 0x0300}, // MCU_DATA_6
	{0x339E, 0x3C18}, // MCU_DATA_7
	{0x338C, 0x0530}, // MCU_ADDRESS
	{0x3390, 0x38E6}, // MCU_DATA_0
	{0x3392, 0x03E7}, // MCU_DATA_1
	{0x3394, 0x0418}, // MCU_DATA_2
	{0x3396, 0xFE03}, // MCU_DATA_3
	{0x3398, 0x0030}, // MCU_DATA_4
	{0x339A, 0xE613}, // MCU_DATA_5
	{0x339C, 0x18E7}, // MCU_DATA_6
	{0x339E, 0x0330}, // MCU_DATA_7
	{0x338C, 0x0540}, // MCU_ADDRESS
	{0x3390, 0xC614}, // MCU_DATA_0
	{0x3392, 0x3A35}, // MCU_DATA_1
	{0x3394, 0x3937}, // MCU_DATA_2
	{0x3396, 0x3C3C}, // MCU_DATA_3
	{0x3398, 0x3C30}, // MCU_DATA_4
	{0x339A, 0x6F05}, // MCU_DATA_5
	{0x339C, 0xA605}, // MCU_DATA_6
	{0x339E, 0xC601}, // MCU_DATA_7
	{0x338C, 0x0550}, // MCU_ADDRESS
	{0x3390, 0xBD05}, // MCU_DATA_0
	{0x3392, 0xCC30}, // MCU_DATA_1
	{0x3394, 0xE406}, // MCU_DATA_2
	{0x3396, 0x2765}, // MCU_DATA_3
	{0x3398, 0xCC10}, // MCU_DATA_4
	{0x339A, 0x80ED}, // MCU_DATA_5
	{0x339C, 0x03E6}, // MCU_DATA_6
	{0x339E, 0x0586}, // MCU_DATA_7
	{0x338C, 0x0560}, // MCU_ADDRESS
	{0x3390, 0x033D}, // MCU_DATA_0
	{0x3392, 0x05E3}, // MCU_DATA_1
	{0x3394, 0x03ED}, // MCU_DATA_2
	{0x3396, 0x031A}, // MCU_DATA_3
	{0x3398, 0xEE03}, // MCU_DATA_4
	{0x339A, 0xEC13}, // MCU_DATA_5
	{0x339C, 0x18ED}, // MCU_DATA_6
	{0x339E, 0x001A}, // MCU_DATA_7
	{0x338C, 0x0570}, // MCU_ADDRESS
	{0x3390, 0xEE03}, // MCU_DATA_0
	{0x3392, 0xEC11}, // MCU_DATA_1
	{0x3394, 0x18ED}, // MCU_DATA_2
	{0x3396, 0x021A}, // MCU_DATA_3
	{0x3398, 0xEE03}, // MCU_DATA_4
	{0x339A, 0xEC0F}, // MCU_DATA_5
	{0x339C, 0x18ED}, // MCU_DATA_6
	{0x339E, 0x041A}, // MCU_DATA_7
	{0x338C, 0x0580}, // MCU_ADDRESS
	{0x3390, 0xEE03}, // MCU_DATA_0
	{0x3392, 0xEC0D}, // MCU_DATA_1
	{0x3394, 0x18ED}, // MCU_DATA_2
	{0x3396, 0x061A}, // MCU_DATA_3
	{0x3398, 0xEE03}, // MCU_DATA_4
	{0x339A, 0xEC0B}, // MCU_DATA_5
	{0x339C, 0x18ED}, // MCU_DATA_6
	{0x339E, 0x081A}, // MCU_DATA_7
	{0x338C, 0x0590}, // MCU_ADDRESS
	{0x3390, 0xEE03}, // MCU_DATA_0
	{0x3392, 0xEC09}, // MCU_DATA_1
	{0x3394, 0x18ED}, // MCU_DATA_2
	{0x3396, 0x0AA6}, // MCU_DATA_3
	{0x3398, 0x0544}, // MCU_DATA_4
	{0x339A, 0x8B04}, // MCU_DATA_5
	{0x339C, 0xC601}, // MCU_DATA_6
	{0x339E, 0xBD05}, // MCU_DATA_7
	{0x338C, 0x05A0}, // MCU_ADDRESS
	{0x3390, 0xCC30}, // MCU_DATA_0
	{0x3392, 0xA605}, // MCU_DATA_1
	{0x3394, 0x44A7}, // MCU_DATA_2
	{0x3396, 0x0286}, // MCU_DATA_3
	{0x3398, 0x01ED}, // MCU_DATA_4
	{0x339A, 0x00A6}, // MCU_DATA_5
	{0x339C, 0x02E6}, // MCU_DATA_6
	{0x339E, 0x00BD}, // MCU_DATA_7
	{0x338C, 0x05B0}, // MCU_ADDRESS
	{0x3390, 0x05CC}, // MCU_DATA_0
	{0x3392, 0x30E7}, // MCU_DATA_1
	{0x3394, 0x02EB}, // MCU_DATA_2
	{0x3396, 0x01FA}, // MCU_DATA_3
	{0x3398, 0x10B0}, // MCU_DATA_4
	{0x339A, 0xF710}, // MCU_DATA_5
	{0x339C, 0xB0E6}, // MCU_DATA_6
	{0x339E, 0x05CB}, // MCU_DATA_7
	{0x338C, 0x05C0}, // MCU_ADDRESS
	{0x3390, 0x02E7}, // MCU_DATA_0
	{0x3392, 0x05C1}, // MCU_DATA_1
	{0x3394, 0x0825}, // MCU_DATA_2
	{0x3396, 0x8538}, // MCU_DATA_3
	{0x3398, 0x3838}, // MCU_DATA_4
	{0x339A, 0x3139}, // MCU_DATA_5
	{0x339C, 0x4D27}, // MCU_DATA_6
	{0x339E, 0x0458}, // MCU_DATA_7
	{0x338C, 0x05D0}, // MCU_ADDRESS
	{0x3390, 0x4A26}, // MCU_DATA_0
	{0x3392, 0xFC39}, // MCU_DATA_1
	{0x338C, 0x2003}, // MCU_ADDRESS
	{0x3390, 0x0400}, // MON_ARG1
	{0x338C, 0xA002}, // MCU_ADDRESS
	{0x3390, 0x0001}, // MON_CMD
	{0x338C, 0xA102}, // MCU_ADDRESS
	{0x3390, 0x001F}, // SEQ_MODE
	{0x338C, 0x1070}, // MCU_ADDRESS
	{0x3390, 0x0302}, // MCU_DATA_0
	{0x338C, 0x1078}, // MCU_ADDRESS
	{0x3390, 0x0CFC}, // MCU_DATA_0
	{0x338C, 0xA505}, // MCU_ADDRESS
	{0x3390, 0x0000}, // AF_MODE_EX
	{0x338C, 0xA506}, // MCU_ADDRESS
	{0x3390, 0x0010}, // AF_NUM_STEPS
	{0x338C, 0xA514}, // MCU_ADDRESS
	{0x3390, 0x0000}, // AF_POSITION_0
	{0x338C, 0xA515}, // MCU_ADDRESS
	{0x3390, 0x0011}, // AF_POSITION_1
	{0x338C, 0xA516}, // MCU_ADDRESS
	{0x3390, 0x0022}, // AF_POSITION_2
	{0x338C, 0xA517}, // MCU_ADDRESS
	{0x3390, 0x0033}, // AF_POSITION_3
	{0x338C, 0xA518}, // MCU_ADDRESS
	{0x3390, 0x0044}, // AF_POSITION_4
	{0x338C, 0xA519}, // MCU_ADDRESS
	{0x3390, 0x0055}, // AF_POSITION_5
	{0x338C, 0xA51A}, // MCU_ADDRESS
	{0x3390, 0x0066}, // AF_POSITION_6
	{0x338C, 0xA51B}, // MCU_ADDRESS
	{0x3390, 0x0077}, // AF_POSITION_7
	{0x338C, 0xA51C}, // MCU_ADDRESS
	{0x3390, 0x0088}, // AF_POSITION_8
	{0x338C, 0xA51D}, // MCU_ADDRESS
	{0x3390, 0x0099}, // AF_POSITION_9
	{0x338C, 0xA51E}, // MCU_ADDRESS
	{0x3390, 0x00AA}, // AF_POSITION_10
	{0x338C, 0xA51F}, // MCU_ADDRESS
	{0x3390, 0x00BB}, // AF_POSITION_11
	{0x338C, 0xA520}, // MCU_ADDRESS
	{0x3390, 0x00CC}, // AF_POSITION_12
	{0x338C, 0xA521}, // MCU_ADDRESS
	{0x3390, 0x00DD}, // AF_POSITION_13
	{0x338C, 0xA522}, // MCU_ADDRESS
	{0x3390, 0x00EE}, // AF_POSITION_14
	{0x338C, 0xA523}, // MCU_ADDRESS
	{0x3390, 0x00FE}, // AF_POSITION_15
	{0x338C, 0xA505}, // MCU_ADDRESS
	{0x3390, 0x0020}, // AF_MODE_EX
	{0x338C, 0xA508}, // MCU_ADDRESS
	{0x3390, 0x0004}, // AF_NUM_STEPS_2
	{0x338C, 0xA509}, // MCU_ADDRESS
	{0x3390, 0x0006}, // AF_STEP_SIZE

	{0x338C, 0x1078},  // MCU_ADDRESS
	{0x3390, 0x0CFC},  // MCU_DATA_0
	{0x338C, 0x1070}, // MCU_ADDRESS
	{0x3390, 0x0302}, // GPIO_DATA
	{0x338C, 0x1074}, // MCU_ADDRESS
	{0x3390, 0x0002}, // GPIO_OUTPUT_SET
	{0x338C, 0x107E}, // MCU_ADDRESS
	{0x3390, 0x0002}, // GPIO_DIR_OUT

	{0x338C, 0xA603}, // MCU_ADDRESS  
	{0x3390, 0x0000}, // AF_STEP_SIZE

    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_set_preview_mt9d112[] = {
	{0x338C, 0xA120},
	{0x3390, 0x0000},
	{0x338C, 0xA103},
	{0x3390, 0x0001},
	//{0x3290, 0x0003},
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_set_capture_mt9d112[] = {
	{0x338C, 0xA120},
	{0x3390, 0x0002},
	{0x338C, 0xA103},
	{0x3390, 0x0002},
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

 struct sensor_reg* sensor_reg_common_mt9d112[3] =
{
	sensor_initialize_mt9d112,
	sensor_set_preview_mt9d112,
	sensor_set_capture_mt9d112
};

static struct sensor_reg sensor_brightness_0_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_brightness_1_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_brightness_2_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_brightness_3_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_brightness_4_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_brightness_mt9d112[5] =
{
	sensor_brightness_0_mt9d112,
	sensor_brightness_1_mt9d112,
	sensor_brightness_2_mt9d112,
	sensor_brightness_3_mt9d112,
	sensor_brightness_4_mt9d112
};

static struct sensor_reg sensor_awb_auto_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_awb_daylight_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_awb_incandescent_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_awb_fluorescent_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_awb_cloudy_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_awb_sunset_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_awb_mt9d112[6] =
{
	sensor_awb_auto_mt9d112,
	sensor_awb_daylight_mt9d112,
	sensor_awb_incandescent_mt9d112,
	sensor_awb_fluorescent_mt9d112,
	sensor_awb_cloudy_mt9d112,
	sensor_awb_sunset_mt9d112
	
};

static struct sensor_reg sensor_iso_auto_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_iso_100_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_iso_200_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_iso_400_mt9d112[] = {
	{MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_iso_mt9d112[4] =
{
	sensor_iso_auto_mt9d112,
	sensor_iso_100_mt9d112,
	sensor_iso_200_mt9d112,
	sensor_iso_400_mt9d112
};

static struct sensor_reg sensor_effect_normal_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_effect_gray_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_effect_negative_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_effect_sepia_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_effect_sharpness_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_effect_sketch_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_effect_mt9d112[6] =
{
	sensor_effect_normal_mt9d112,
	sensor_effect_gray_mt9d112,
	sensor_effect_negative_mt9d112,
	sensor_effect_sepia_mt9d112,
	sensor_effect_sharpness_mt9d112,
	sensor_effect_sketch_mt9d112,
};

static struct sensor_reg sensor_reg_flipnone_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_reg_hflip_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_reg_vflip_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_reg_hvflip_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_flip_mt9d112[4] =
{
	sensor_reg_flipnone_mt9d112,
	sensor_reg_hflip_mt9d112,
	sensor_reg_vflip_mt9d112,
	sensor_reg_hvflip_mt9d112,
};

static struct sensor_reg sensor_secne_auto_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_secne_night_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_secne_landscape_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_secne_portrait_mt9d112[] = {
	{MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_secne_sport_mt9d112[] = {
	{MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_scene_mt9d112[5] =
{
	sensor_secne_auto_mt9d112,
	sensor_secne_night_mt9d112,
	sensor_secne_landscape_mt9d112,
	sensor_secne_portrait_mt9d112,
	sensor_secne_sport_mt9d112
};

static struct sensor_reg sensor_me_mtrix_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_me_center_weighted_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_me_spot_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_metering_exposure_mt9d112[3] =
{
	sensor_me_mtrix_mt9d112,
	sensor_me_center_weighted_mt9d112,
	sensor_me_spot_mt9d112
};

static struct sensor_reg sensor_af_single_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

static struct sensor_reg sensor_af_manual_mt9d112[] = {
    {MT9D112_REG_TERM, MT9D112_VAL_TERM}
};

struct sensor_reg* sensor_reg_af_mt9d112[2] =
{
	sensor_af_single_mt9d112,
	sensor_af_manual_mt9d112
};

static int write_regs_mt9d112(const struct sensor_reg reglist[])
{
	int err;
	int err_cnt = 0;	
	int sleep_cnt = 100;
	unsigned char data[132];
	unsigned char bytes;
	const struct sensor_reg *next = reglist;
	
	while (!((next->reg == MT9D112_REG_TERM) && (next->val == MT9D112_VAL_TERM)))
	{
		
		if(next->reg == MT9D112_REG_TERM && next->val != MT9D112_VAL_TERM)
		{
			//mdelay(next->val);
			msleep(next->val);
			sleep_cnt = 100;
			printk("Sensor init Delay[%d]!!!! \n", next->val);
			next++;
		}
		else
		{
			sleep_cnt--;
			if(sleep_cnt == 0)
			{
				msleep(10);
				sleep_cnt = 100;
			}
			
			bytes = 0;
			data[bytes]= next->reg>>8;		bytes++;		
			data[bytes]= (u8)next->reg&0xff; 	bytes++;

			data[bytes]= next->val>>8;		bytes++;		
			data[bytes]= (u8)next->val&0xff; 	bytes++;

			err = DDI_I2C_Write(data, 2, bytes-2);
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

static int sensor_open_mt9d112(bool bChangeCamera)
{
	sensor_power_disable();
	msleep(10);
	
	sensor_power_enable();
	msleep(10);

	sensor_powerdown_disable();
	msleep(10);

	sensor_reset_low();
	msleep(10);

	CIF_Open();
	msleep(50);

	sensor_reset_high();
	msleep(10);

	return write_regs_mt9d112(sensor_reg_common_mt9d112[0]);
}

static int sensor_close_mt9d112(void)
{
	CIF_ONOFF(OFF);

	#if (1) //20110315 ysseung   supported to sensor stand-by mode.
	sensor_powerdown_enable();
	msleep(10);

	sensor_reset_low();
	msleep(10);
	#else
	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_disable();
	#endif

	CIF_Close();
	msleep(5);

	return 0;
}

static int sensor_powerdown_mt9d112(bool bChangeCamera)
{
	#if (1) //20110315 ysseung   supported to sensor stand-by mode.
	sensor_powerdown_enable();
	msleep(60);

	sensor_reset_low();
	msleep(20);
	#else
	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_disable();
	#endif

	return 0;
}

static int sensor_preview_mt9d112(void)
{
	return write_regs_mt9d112(sensor_reg_common_mt9d112[1]);
}

static int sensor_capture_mt9d112(void)
{
	return write_regs_mt9d112(sensor_reg_common_mt9d112[2]);
}

static int sensor_capturecfg_mt9d112(int width, int height)
{
	return 0;
}

static int sensor_zoom_mt9d112(int val)
{
	return 0;
}

static int sensor_autofocus_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_af_mt9d112[val]);
}

static int sensor_effect_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_effect_mt9d112[val]);
}

static int sensor_flip_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_flip_mt9d112[val]);
}

static int sensor_iso_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_iso_mt9d112[val]);
}

static int sensor_me_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_metering_exposure_mt9d112[val]);
}

static int sensor_wb_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_awb_mt9d112[val]);
}

static int sensor_bright_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_brightness_mt9d112[val]);
}

static int sensor_scene_mt9d112(int val)
{
	return write_regs_mt9d112(sensor_reg_scene_mt9d112[val]);
}

static int sensor_check_esd_mt9d112(int val)
{
	return 0;
}

static int sensor_check_luma_mt9d112(int val)
{
	return 0;
}

/**********************************************************
*    Function  
**********************************************************/
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void sensor_info_init_mt9d112(TCC_SENSOR_INFO_TYPE *sensor_info)
{
	if(machine_is_m803())
		sensor_info->i2c_addr 				= 0x78;
	else
		sensor_info->i2c_addr 				= 0x7A;
	
	sensor_info->reg_term 				= 0x0000;
	sensor_info->val_term 				= 0x0000;
	#if defined(CONFIG_ARCH_TCC92XX)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 540000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#elif defined(CONFIG_ARCH_TCC93XX)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 540000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 540000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#endif // CONFIG_USE_ISP
	#elif defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 540000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 540000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#endif // CONFIG_USE_ISP
	#endif
	sensor_info->preview_w 				= 800;
	sensor_info->preview_h 				= 600;
	sensor_info->preview_zoom_offset_x 	= 12;
	sensor_info->preview_zoom_offset_y 	= 9;
	sensor_info->capture_w 				= 1600;
	sensor_info->capture_h 				= 1200;
	sensor_info->capture_zoom_offset_x 	= 24;
	sensor_info->capture_zoom_offset_y 	= 18;
	sensor_info->max_zoom_step 			= 15;
	sensor_info->cam_capchg_width 		= 800;
	sensor_info->framerate				= 15;
	sensor_info->p_clock_pol 				= POSITIVE_EDGE;
	sensor_info->v_sync_pol 				= ACT_HIGH;
	sensor_info->h_sync_pol                 = ACT_HIGH;
	#if defined(CONFIG_ARCH_TCC892X)
	sensor_info->de_pol                 = ACT_LOW;
	#endif	
	sensor_info->format 					= M420_ZERO;
	sensor_info->capture_skip_frame 		= 1;
	sensor_info->sensor_sizes 			= sensor_sizes_mt9d112;
}

void sensor_init_fnc_mt9d112(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_mt9d112;
	sensor_func->Close 					= sensor_close_mt9d112;
	sensor_func->PowerDown				= sensor_powerdown_mt9d112;

	sensor_func->Set_Preview 			= sensor_preview_mt9d112;
	sensor_func->Set_Capture 			= sensor_capture_mt9d112;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_mt9d112;

	sensor_func->Set_Zoom 				= sensor_zoom_mt9d112;
	sensor_func->Set_AF 				= sensor_autofocus_mt9d112;
	sensor_func->Set_Effect 				= sensor_effect_mt9d112;
	sensor_func->Set_Flip 				= sensor_flip_mt9d112;
	sensor_func->Set_ISO 				= sensor_iso_mt9d112;
	sensor_func->Set_ME 				= sensor_me_mt9d112;
	sensor_func->Set_WB 				= sensor_wb_mt9d112;
	sensor_func->Set_Bright 				= sensor_bright_mt9d112;
	sensor_func->Set_Scene 				= sensor_scene_mt9d112;

	sensor_func->Check_ESD 				= sensor_check_esd_mt9d112;
	sensor_func->Check_Luma 			= sensor_check_luma_mt9d112;
}
#else
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_mt9d112;
	sensor_func->Close 					= sensor_close_mt9d112;
	sensor_func->PowerDown				= sensor_powerdown_mt9d112;

	sensor_func->Set_Preview 			= sensor_preview_mt9d112;
	sensor_func->Set_Capture 			= sensor_capture_mt9d112;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_mt9d112;

	sensor_func->Set_Zoom 				= sensor_zoom_mt9d112;
	sensor_func->Set_AF 				= sensor_autofocus_mt9d112;
	sensor_func->Set_Effect 				= sensor_effect_mt9d112;
	sensor_func->Set_Flip 				= sensor_flip_mt9d112;
	sensor_func->Set_ISO 				= sensor_iso_mt9d112;
	sensor_func->Set_ME 				= sensor_me_mt9d112;
	sensor_func->Set_WB 				= sensor_wb_mt9d112;
	sensor_func->Set_Bright 				= sensor_bright_mt9d112;
	sensor_func->Set_Scene 				= sensor_scene_mt9d112;

	sensor_func->Check_ESD 				= sensor_check_esd_mt9d112;
	sensor_func->Check_Luma 			= sensor_check_luma_mt9d112;
}
#endif
#endif

