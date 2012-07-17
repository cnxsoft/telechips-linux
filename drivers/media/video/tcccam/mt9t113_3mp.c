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

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_MT9T113

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern int CameraID;
#endif

unsigned char bEnableAutoFocus = true;

/* Array of image sizes supported by MT9T113.  These must be ordered from 
 * smallest image size to largest.
 */
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
struct capture_size sensor_sizes_mt9t113[] = {
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

static struct sensor_reg sensor_initialize_mt9t113[] = {

#if 1	// 24.75 Mhz Setting
//[Initialize]
//[MT9T113 (SOC3140) Register Wizard Defaults]
//{MT9T113_REG_TERM. 0x0100, WORD_LEN},
{ 0x0010, 0x0110	, WORD_LEN},	//PLL Dividers = 272
{ 0x0012, 0x0070	, WORD_LEN},	//PLL P Dividers = 112
{ 0x002A, 0x7475	, WORD_LEN},	//PLL P Dividers 4-5-6 = 29813
{ 0x0018, 0x402E	, WORD_LEN},	//Standby:Default = 16430
//POLL_REG=0x0018,0x4000,==1,DELAY=10,TIMEOUT=100   // Wait for FW initialization complete
{MT9T113_REG_TERM, 0x0100, WORD_LEN},//Delay = 30
{ 0x0022, 0x0140	, WORD_LEN},	//Reference clock count for 20 us = 320
{ 0x001E, 0x0400	, WORD_LEN},	//Pad Slew Rate = 1911
{ 0x3B84, 0x0188	, WORD_LEN},	//I2C Master Clock Divider = 392

//LOAD the FW patches and recommended settings here!

{ 0x0018, 0x4028	, WORD_LEN},	//Out of Standby = 16424
{ 0x098E, 0x4800	, WORD_LEN},	//Row Start (A)
{ 0x0990, 0x0010	, WORD_LEN},	//      = 16
{ 0x098E, 0x4802	, WORD_LEN},	//Column Start (A)
{ 0x0990, 0x0010	, WORD_LEN},	//      = 16
{ 0x098E, 0x4804	, WORD_LEN},	//Row End (A)
{ 0x0990, 0x062D	, WORD_LEN},	//      = 1581
{ 0x098E, 0x4806	, WORD_LEN},	//Column End (A)
{ 0x0990, 0x082D	, WORD_LEN},	//      = 2093
{ 0x098E, 0x4808	, WORD_LEN},	//Base Frame Lines (A)
{ 0x0990, 0x0359	, WORD_LEN},	//      = 857
{ 0x098E, 0x480A	, WORD_LEN},	//Line Length (A)
{ 0x0990, 0x0B24	, WORD_LEN},	//      = 2852
{ 0x098E, 0x480C	, WORD_LEN},	//Fine Correction (A)
{ 0x0990, 0x0399	, WORD_LEN},	//      = 921
{ 0x098E, 0x480E	, WORD_LEN},	//Row Speed (A)
{ 0x0990, 0x0111	, WORD_LEN},	//      = 273
{ 0x098E, 0x4810	, WORD_LEN},	//Read Mode (A)
{ 0x0990, 0x046C	, WORD_LEN},	//      = 1132
{ 0x098E, 0x4812	, WORD_LEN},	//Fine IT Min (A)
{ 0x0990, 0x0510	, WORD_LEN},	//      = 1296
{ 0x098E, 0x4814	, WORD_LEN},	//Fine IT Max Margin (A)
{ 0x0990, 0x01BA	, WORD_LEN},	//      = 442
{ 0x098E, 0x482D	, WORD_LEN},	//Row Start (B)
{ 0x0990, 0x0018	, WORD_LEN},	//      = 24
{ 0x098E, 0x482F	, WORD_LEN},	//Column Start (B)
{ 0x0990, 0x0018	, WORD_LEN},	//      = 24
{ 0x098E, 0x4831	, WORD_LEN},	//Row End (B)
{ 0x0990, 0x0627	, WORD_LEN},	//      = 1575
{ 0x098E, 0x4833	, WORD_LEN},	//Column End (B)
{ 0x0990, 0x0827	, WORD_LEN},	//      = 2087
{ 0x098E, 0x4835	, WORD_LEN},	//Base Frame Lines (B)
{ 0x0990, 0x065D	, WORD_LEN},	//      = 1629
{ 0x098E, 0x4837	, WORD_LEN},	//Line Length (B)
{ 0x0990, 0x15AE	, WORD_LEN},	//      = 5550
{ 0x098E, 0x4839	, WORD_LEN},	//Fine Correction (B)
{ 0x0990, 0x019F	, WORD_LEN},	//      = 415
{ 0x098E, 0x483B	, WORD_LEN},	//Row Speed (B)
{ 0x0990, 0x0111	, WORD_LEN},	//      = 273
{ 0x098E, 0x483D	, WORD_LEN},	//Read Mode (B)
{ 0x0990, 0x0024	, WORD_LEN},	//      = 36
{ 0x098E, 0x483F	, WORD_LEN},	//Fine IT Min (B)
{ 0x0990, 0x0266	, WORD_LEN},	//      = 614
{ 0x098E, 0x4841	, WORD_LEN},	//Fine IT Max Margin (B)
{ 0x0990, 0x010A	, WORD_LEN},	//      = 266
{ 0x098E, 0xB81A	, WORD_LEN},	//fd_zone_height
{ 0x0990, 0x05		, BYTE_LEN},//      = 5
{ 0x098E, 0x481A	, WORD_LEN},	//fd_period_50Hz (A)
{ 0x0990, 0x00E7	, WORD_LEN},	//      = 231
{ 0x098E, 0x481C	, WORD_LEN},	//fd_period_60Hz (A)
{ 0x0990, 0x00C1	, WORD_LEN},	//      = 193
{ 0x098E, 0xC81E	, WORD_LEN},	//fd_search_f1_50hz (A)
{ 0x0990, 0x25		, BYTE_LEN},//      = 37
{ 0x098E, 0xC81F	, WORD_LEN},	//fd_search_f2_50hz (A)
{ 0x0990, 0x27		, BYTE_LEN},//      = 39
{ 0x098E, 0xC820	, WORD_LEN},	//fd_search_f1_60hz (A)
{ 0x0990, 0x2D		, BYTE_LEN},//      = 45
{ 0x098E, 0xC821	, WORD_LEN},	//fd_search_f2_60hz (A)
{ 0x0990, 0x2F		, BYTE_LEN},//      = 47
{ 0x098E, 0x4847	, WORD_LEN},	//fd_period_50Hz (B)
{ 0x0990, 0x0077	, WORD_LEN},	//      = 119
{ 0x098E, 0x4849	, WORD_LEN},	//fd_period_60Hz (B)
{ 0x0990, 0x0063	, WORD_LEN},	//      = 99
{ 0x098E, 0xC84B	, WORD_LEN},	//fd_search_f1_50hz (B)
{ 0x0990, 0x12		, BYTE_LEN},//      = 18
{ 0x098E, 0xC84C	, WORD_LEN},	//fd_search_f2_50hz (B)
{ 0x0990, 0x14		, BYTE_LEN},//      = 20
{ 0x098E, 0xC84D	, WORD_LEN},	//fd_search_f1_60hz (B)
{ 0x0990, 0x16		, BYTE_LEN},//      = 22
{ 0x098E, 0xC84E	, WORD_LEN},	//fd_search_f2_60hz (B)
{ 0x0990, 0x18		, BYTE_LEN},//      = 24
{ 0x098E, 0x6800	, WORD_LEN},	//Output Width (A)
{ 0x0990, 0x0280	, WORD_LEN},	//      = 640
{ 0x098E, 0x6802	, WORD_LEN},	//Output Height (A)
{ 0x0990, 0x01E0	, WORD_LEN},	//      = 480
{ 0x098E, 0x6804	, WORD_LEN},	//FOV Width (A)
{ 0x0990, 0x0400	, WORD_LEN},	//      = 1024
{ 0x098E, 0x6806	, WORD_LEN},	//FOV Height (A)
{ 0x0990, 0x0300	, WORD_LEN},	//      = 768
{ 0x098E, 0xE892	, WORD_LEN},	//JPEG Mode (A)
{ 0x0990, 0x00		, BYTE_LEN},//      = 0
{ 0x098E, 0x6C00	, WORD_LEN},	//Output Width (B)
{ 0x0990, 0x0800	, WORD_LEN},	//      = 2048
{ 0x098E, 0x6C02	, WORD_LEN},	//Output Height (B)
{ 0x0990, 0x0600	, WORD_LEN},	//      = 1536
{ 0x098E, 0x6C04	, WORD_LEN},	//FOV Width (B)
{ 0x0990, 0x0800	, WORD_LEN},	//      = 2048
{ 0x098E, 0x6C06	, WORD_LEN},	//FOV Height (B)
{ 0x0990, 0x0600	, WORD_LEN},	//      = 1536
{ 0x098E, 0xEC92	, WORD_LEN},	//JPEG Mode (B)
{ 0x0990, 0x00		, BYTE_LEN},//      = 0
{ 0x098E, 0x8400	, WORD_LEN},	//Refresh Sequencer Mode
{ 0x0990, 0x06		, BYTE_LEN},//      = 6
#endif

#if 0
{ 0x098E, 0xC84B	, WORD_LEN}, //fd_search_f1_50hz (B)
{ 0x0990, 0x12	, BYTE_LEN}, //      = 18
{ 0x098E, 0xC84C	, WORD_LEN}, //fd_search_f2_50hz (B)
{ 0x0990, 0x14	, BYTE_LEN}, //      = 20
{ 0x098E, 0xC84D	, WORD_LEN}, //fd_search_f1_60hz (B)
{ 0x0990, 0x16	, BYTE_LEN}, //      = 22
{ 0x098E, 0xC84E	, WORD_LEN}, //fd_search_f2_60hz (B)
{ 0x0990, 0x18	, BYTE_LEN}, //      = 24
{ 0x098E, 0x6800	, WORD_LEN}, //Output Width (A)
{ 0x0990, 0x0320	, WORD_LEN}, //      = 800
{ 0x098E, 0x6802	, WORD_LEN}, //Output Height (A)
{ 0x0990, 0x01E0	, WORD_LEN}, //      = 480
{ 0x098E, 0x6804	, WORD_LEN}, //FOV Width (A)
{ 0x0990, 0x0400	, WORD_LEN}, //      = 1024->400
{ 0x098E, 0x6806	, WORD_LEN}, //FOV Height (A)
{ 0x0990, 0x0300	, WORD_LEN}, //      = 768->300
{ 0x098E, 0xE892	, WORD_LEN}, //JPEG Mode (A)
{ 0x0990, 0x00	, BYTE_LEN}, //      = 0
{ 0x098E, 0x6C00	, WORD_LEN}, //Output Width (B)
{ 0x0990, 0x0800	, WORD_LEN}, //      = 2048
{ 0x098E, 0x6C02	, WORD_LEN}, //Output Height (B)
{ 0x0990, 0x0600	, WORD_LEN}, //      = 1536
{ 0x098E, 0x6C04	, WORD_LEN}, //FOV Width (B)
{ 0x0990, 0x0800	, WORD_LEN}, //      = 2048
{ 0x098E, 0x6C06	, WORD_LEN}, //FOV Height (B)
{ 0x0990, 0x0600	, WORD_LEN}, //      = 1536
{ 0x098E, 0xEC92	, WORD_LEN}, //JPEG Mode (B)
{ 0x0990, 0x00	, BYTE_LEN}, //      = 0
{ 0x098E, 0x8400	, WORD_LEN}, //Refresh Sequencer Mode
{ 0x0990, 0x06	, BYTE_LEN}, //      = 6
#endif
{0x3210, 0x01B8 	, WORD_LEN}, // COLOR_PIPELINE_CONTROL
{0x3640, 0x0390 	, WORD_LEN}, // P_G1_P0Q0
{0x3642, 0x7E4D 	, WORD_LEN}, // P_G1_P0Q1
{0x3644, 0x2391 	, WORD_LEN}, // P_G1_P0Q2
{0x3646, 0x934E 	, WORD_LEN}, // P_G1_P0Q3
{0x3648, 0xB370 	, WORD_LEN}, // P_G1_P0Q4
{0x364A, 0x04F0 	, WORD_LEN}, // P_R_P0Q0
{0x364C, 0x9A0E 	, WORD_LEN}, // P_R_P0Q1
{0x364E, 0x5C50 	, WORD_LEN}, // P_R_P0Q2
{0x3650, 0x3C0F 	, WORD_LEN}, // P_R_P0Q3
{0x3652, 0x63EC 	, WORD_LEN}, // P_R_P0Q4
{0x3654, 0x04D0 	, WORD_LEN}, // P_B_P0Q0
{0x3656, 0x5B4E 	, WORD_LEN}, // P_B_P0Q1
{0x3658, 0x45B0 	, WORD_LEN}, // P_B_P0Q2
{0x365A, 0xC78F 	, WORD_LEN}, // P_B_P0Q3
{0x365C, 0x770E 	, WORD_LEN}, // P_B_P0Q4
{0x365E, 0x0530 	, WORD_LEN}, // P_G2_P0Q0
{0x3660, 0xD48E 	, WORD_LEN}, // P_G2_P0Q1
{0x3662, 0x3711 	, WORD_LEN}, // P_G2_P0Q2
{0x3664, 0x496E 	, WORD_LEN}, // P_G2_P0Q3
{0x3666, 0xF830 	, WORD_LEN}, // P_G2_P0Q4
{0x3680, 0xA00D 	, WORD_LEN}, // P_G1_P1Q0
{0x3682, 0xBF6E 	, WORD_LEN}, // P_G1_P1Q1
{0x3684, 0x826E 	, WORD_LEN}, // P_G1_P1Q2
{0x3686, 0x1A2F 	, WORD_LEN}, // P_G1_P1Q3
{0x3688, 0x11CD 	, WORD_LEN}, // P_G1_P1Q4
{0x368A, 0xA1ED 	, WORD_LEN}, // P_R_P1Q0
{0x368C, 0x258E 	, WORD_LEN}, // P_R_P1Q1
{0x368E, 0x374D 	, WORD_LEN}, // P_R_P1Q2
{0x3690, 0xB56F 	, WORD_LEN}, // P_R_P1Q3
{0x3692, 0xAA6F 	, WORD_LEN}, // P_R_P1Q4
{0x3694, 0x140E 	, WORD_LEN}, // P_B_P1Q0
{0x3696, 0x1E0F 	, WORD_LEN}, // P_B_P1Q1
{0x3698, 0x0910 	, WORD_LEN}, // P_B_P1Q2
{0x369A, 0x9910 	, WORD_LEN}, // P_B_P1Q3
{0x369C, 0x8172 	, WORD_LEN}, // P_B_P1Q4
{0x369E, 0x2D6D 	, WORD_LEN}, // P_G2_P1Q0
{0x36A0, 0x8CCF 	, WORD_LEN}, // P_G2_P1Q1
{0x36A2, 0x30F0 	, WORD_LEN}, // P_G2_P1Q2
{0x36A4, 0x4E0F 	, WORD_LEN}, // P_G2_P1Q3
{0x36A6, 0xFEB1 	, WORD_LEN}, // P_G2_P1Q4
{0x36C0, 0x4F91 	, WORD_LEN}, // P_G1_P2Q0
{0x36C2, 0x368F 	, WORD_LEN}, // P_G1_P2Q1
{0x36C4, 0xDB92 	, WORD_LEN}, // P_G1_P2Q2
{0x36C6, 0x8F92 	, WORD_LEN}, // P_G1_P2Q3
{0x36C8, 0x7ED4 	, WORD_LEN}, // P_G1_P2Q4
{0x36CA, 0x5131 	, WORD_LEN}, // P_R_P2Q0
{0x36CC, 0xDE2F 	, WORD_LEN}, // P_R_P2Q1
{0x36CE, 0xB411 	, WORD_LEN}, // P_R_P2Q2
{0x36D0, 0xB751 	, WORD_LEN}, // P_R_P2Q3
{0x36D2, 0x2C14 	, WORD_LEN}, // P_R_P2Q4
{0x36D4, 0x42B1 	, WORD_LEN}, // P_B_P2Q0
{0x36D6, 0x0A10 	, WORD_LEN}, // P_B_P2Q1
{0x36D8, 0xE312 	, WORD_LEN}, // P_B_P2Q2
{0x36DA, 0x8952 	, WORD_LEN}, // P_B_P2Q3
{0x36DC, 0x1195 	, WORD_LEN}, // P_B_P2Q4
{0x36DE, 0x4411 	, WORD_LEN}, // P_G2_P2Q0
{0x36E0, 0x9E4F 	, WORD_LEN}, // P_G2_P2Q1
{0x36E2, 0x8F92 	, WORD_LEN}, // P_G2_P2Q2
{0x36E4, 0xA292 	, WORD_LEN}, // P_G2_P2Q3
{0x36E6, 0x6F94 	, WORD_LEN}, // P_G2_P2Q4
{0x3700, 0x246E 	, WORD_LEN}, // P_G1_P3Q0
{0x3702, 0x2B70 	, WORD_LEN}, // P_G1_P3Q1
{0x3704, 0xE031 	, WORD_LEN}, // P_G1_P3Q2
{0x3706, 0xBD91 	, WORD_LEN}, // P_G1_P3Q3
{0x3708, 0x0D74 	, WORD_LEN}, // P_G1_P3Q4
{0x370A, 0x558F 	, WORD_LEN}, // P_R_P3Q0
{0x370C, 0x92CD 	, WORD_LEN}, // P_R_P3Q1
{0x370E, 0xF112 	, WORD_LEN}, // P_R_P3Q2
{0x3710, 0x5690 	, WORD_LEN}, // P_R_P3Q3
{0x3712, 0x3654 	, WORD_LEN}, // P_R_P3Q4
{0x3714, 0x4B4C 	, WORD_LEN}, // P_B_P3Q0
{0x3716, 0x8790 	, WORD_LEN}, // P_B_P3Q1
{0x3718, 0xC072 	, WORD_LEN}, // P_B_P3Q2
{0x371A, 0x0271 	, WORD_LEN}, // P_B_P3Q3
{0x371C, 0x4CF4 	, WORD_LEN}, // P_B_P3Q4
{0x371E, 0x566D 	, WORD_LEN}, // P_G2_P3Q0
{0x3720, 0x3F4F 	, WORD_LEN}, // P_G2_P3Q1
{0x3722, 0xD412 	, WORD_LEN}, // P_G2_P3Q2
{0x3724, 0x5D4F 	, WORD_LEN}, // P_G2_P3Q3
{0x3726, 0x1CB4 	, WORD_LEN}, // P_G2_P3Q4
{0x3740, 0xE8B2 	, WORD_LEN}, // P_G1_P4Q0
{0x3742, 0x9DD2 	, WORD_LEN}, // P_G1_P4Q1
{0x3744, 0x64F5 	, WORD_LEN}, // P_G1_P4Q2
{0x3746, 0x2D14 	, WORD_LEN}, // P_G1_P4Q3
{0x3748, 0xF997 	, WORD_LEN}, // P_G1_P4Q4
{0x374A, 0xE392 	, WORD_LEN}, // P_R_P4Q0
{0x374C, 0x32B0 	, WORD_LEN}, // P_R_P4Q1
{0x374E, 0x5515 	, WORD_LEN}, // P_R_P4Q2
{0x3750, 0x0BF2 	, WORD_LEN}, // P_R_P4Q3
{0x3752, 0xDDB7 	, WORD_LEN}, // P_R_P4Q4
{0x3754, 0xF8D2 	, WORD_LEN}, // P_B_P4Q0
{0x3756, 0xFE52 	, WORD_LEN}, // P_B_P4Q1
{0x3758, 0x0816 	, WORD_LEN}, // P_B_P4Q2
{0x375A, 0x08D5 	, WORD_LEN}, // P_B_P4Q3
{0x375C, 0x8B38 	, WORD_LEN}, // P_B_P4Q4
{0x375E, 0xB772 	, WORD_LEN}, // P_G2_P4Q0
{0x3760, 0x3E4E 	, WORD_LEN}, // P_G2_P4Q1
{0x3762, 0x1755 	, WORD_LEN}, // P_G2_P4Q2
{0x3764, 0x1CF4 	, WORD_LEN}, // P_G2_P4Q3
{0x3766, 0xCEB7 	, WORD_LEN}, // P_G2_P4Q4
{0x3782, 0x02D0 	, WORD_LEN}, // CENTER_ROW
{0x3784, 0x03A0 	, WORD_LEN}, // CENTER_COLUMN
{0x098E, 0x6848 	, WORD_LEN}, // MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
{0x0990, 0x003F 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0x6865 	, WORD_LEN}, // MCU_ADDRESS [PRI_A_CONFIG_STAT_ALGO_ENTER]
{0x0990, 0x801F 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0x6867 	, WORD_LEN}, // MCU_ADDRESS [PRI_A_CONFIG_STAT_ALGO_RUN]
{0x0990, 0x12F7 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0x6881 	, WORD_LEN}, // MCU_ADDRESS [PRI_A_CONFIG_SYSCTRL_ALGO_ENTER]
{0x0990, 0x0002 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0x6883 	, WORD_LEN}, // MCU_ADDRESS [PRI_A_CONFIG_SYSCTRL_ALGO_RUN]
{0x0990, 0x000B 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0x8400 	, WORD_LEN}, // MCU_ADDRESS [SEQ_CMD]
{0x0990, 0x0006 	, WORD_LEN}, // MCU_DATA_0
//[Exposure  0] //AE(Brightness target)=55
{0x098E, 0xE826 	, WORD_LEN}, // MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
{0x0990, 0x0037 	, WORD_LEN}, // MCU_DATA_0
 //[Sharp 0]  //Aperature Correction Gain=3.5
{0x326C, 0x170B 	, WORD_LEN}, // APERTURE_PARAMETERS_2D
 //[Contrast 1.0]     //contras =1.25
{0x098E, 0xBC09 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_0]
{0x0990, 0x0000 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC0A 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_1]
{0x0990, 0x0020 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC0B 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_2]
{0x0990, 0x005D 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC0C 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_3]
{0x0990, 0x008A 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC0D 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_4]
{0x0990, 0x00A9 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC0E 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_5]
{0x0990, 0x00BA 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC0F 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_6]
{0x0990, 0x00C6 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC10 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_7]
{0x0990, 0x00CF 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC11 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_8]
{0x0990, 0x00D7 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC12 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_9]
{0x0990, 0x00DD 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC13 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_10]
{0x0990, 0x00E2 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC14 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_11]
{0x0990, 0x00E7 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC15 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_12]
{0x0990, 0x00EC 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC16 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_13]
{0x0990, 0x00EF 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC17 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_14]
{0x0990, 0x00F3 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC18 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_15]
{0x0990, 0x00F6 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC19 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_16]
{0x0990, 0x00F9 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC1A 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_17]
{0x0990, 0x00FC 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0xBC1B 	, WORD_LEN}, // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_18]
{0x0990, 0x00FF 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0x8400 	, WORD_LEN}, // MCU_ADDRESS [SEQ_CMD]
{0x0990, 0x0005 	, WORD_LEN}, // MCU_DATA_0
//[Saturatioin 0]
{ 0x098E,0xE876 , WORD_LEN},
{ 0x0990,0x0080 , WORD_LEN},
 // Load internal VCM
{0x098E, 0x4403 	, WORD_LEN}, // MCU_ADDRESS [AFM_ALGO]
{0x0990, 0x8008 	, WORD_LEN}, // MCU_DATA_0
{0x098E, 0x8400 	, WORD_LEN}, // MCU_ADDRESS [SEQ_CMD]
{0x0990, 0x0005 	, WORD_LEN}, // MCU_DATA_0
//full scan mode
{0x098E, 0x3003 	, WORD_LEN}, // MCU_ADDRESS [AF_ALGO]
{0x0990, 0x0002 	, WORD_LEN}, // MCU_DATA_0
//total step=10
{0x098E, 0xB00B 	, WORD_LEN}, // MCU_ADDRESS [AF_STEP_SIZE]
{0x0990, 0x000A 	, WORD_LEN}, // MCU_DATA_0

{MT9T113_REG_TERM, 0x0064, WORD_LEN}, //DELAY= 100	 // 100ms
{ 0x098E, 0xEC09        , WORD_LEN},// MCU_ADDRESS [PRI_B_NUM_OF_FRAMES_RUN]
{ 0x0990, 0x0000        , WORD_LEN},// MCU_DATA_0
{ 0x098E, 0x8400        , WORD_LEN},// MCU_ADDRESS [SEQ_CMD]
{ 0x0990, 0x0002        , WORD_LEN},// MCU_DATA_0

{MT9T113_REG_TERM, 0x0064, WORD_LEN}, //DELAY= 100	 // 100ms
{ 0x098E, 0xEC09        , WORD_LEN},    // MCU_ADDRESS [PRI_B_NUM_OF_FRAMES_RUN]
{ 0x0990, 0x0005        , WORD_LEN},// MCU_DATA_0       // 0x0005
{ 0x098E, 0x8400        , WORD_LEN},    // MCU_ADDRESS [SEQ_CMD]
{ 0x0990, 0x0001        , WORD_LEN},    // MCU_DATA_0   //0x0001

{MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};


static struct sensor_reg sensor_set_preview_mt9t113[] = {
{ 0x098E, 0xEC09	, WORD_LEN}, 	// MCU_ADDRESS [PRI_B_NUM_OF_FRAMES_RUN]
{ 0x0990, 0x0005 	, WORD_LEN},// MCU_DATA_0	// 0x0005
{ 0x098E, 0x8400 	, WORD_LEN}, 	// MCU_ADDRESS [SEQ_CMD]
{ 0x0990, 0x0001 	, WORD_LEN}, 	// MCU_DATA_0	//0x0001
{MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_set_capture_mt9t113[] = {
{ 0x098E, 0xEC09 	, WORD_LEN},// MCU_ADDRESS [PRI_B_NUM_OF_FRAMES_RUN]
{ 0x0990, 0x0000 	, WORD_LEN},// MCU_DATA_0
{ 0x098E, 0x8400 	, WORD_LEN},// MCU_ADDRESS [SEQ_CMD]
{ 0x0990, 0x0002 	, WORD_LEN},// MCU_DATA_0
{MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

 struct sensor_reg* sensor_reg_common_mt9t113[3] =
{
	sensor_initialize_mt9t113,
	sensor_set_preview_mt9t113,
	sensor_set_capture_mt9t113	
};

static struct sensor_reg sensor_brightness_0_mt9t113[] = {
	{ 0x098E, 0xE826 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{ 0x0990, 0x002D 	, WORD_LEN},// MCU_DATA_0
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_1_mt9t113[] = {
	{ 0x098E, 0xE826 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{ 0x0990, 0x0037 	, WORD_LEN},// MCU_DATA_0
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_2_mt9t113[] = {
	{ 0x098E, 0xE826 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{ 0x0990, 0x0041 	, WORD_LEN},// MCU_DATA_0
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_3_mt9t113[] = {
	{ 0x098E, 0xE826 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{ 0x0990, 0x004B 	, WORD_LEN},// MCU_DATA_0
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_brightness_4_mt9t113[] = {
	{ 0x098E, 0xE826 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{ 0x0990, 0x0055 	, WORD_LEN},// MCU_DATA_0
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_brightness_mt9t113[5] =
{
	sensor_brightness_0_mt9t113,
	sensor_brightness_1_mt9t113,
	sensor_brightness_2_mt9t113,
	sensor_brightness_3_mt9t113,
	sensor_brightness_4_mt9t113
};


static struct sensor_reg sensor_awb_auto_mt9t113[] = {
	{ 0x098E, 0x6848 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x003F 	, WORD_LEN},// MCU_DATA_0
	{ 0x098E, 0x6865 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_STAT_ALGO_ENTER]	
	{ 0x0990, 0x801F 	, WORD_LEN},// MCU_DATA_0
	{ 0x098E, 0x6867 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_STAT_ALGO_RUN]
	{ 0x0990, 0x12F7 	, WORD_LEN},// MCU_DATA_0
	{ 0x098E, 0x6881 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_SYSCTRL_ALGO_ENTER]	
	{ 0x0990, 0x0002 	, WORD_LEN},// MCU_DATA_0
	{ 0x098E, 0x6883 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_SYSCTRL_ALGO_RUN]
	{ 0x0990, 0x000B 	, WORD_LEN},// MCU_DATA_0
	{ 0x098E, 0x8400 	, WORD_LEN},// MCU_ADDRESS [SEQ_CMD]
	{ 0x0990, 0x0006 	, WORD_LEN},// MCU_DATA_0	
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_daylight_mt9t113[] = {

	{ 0x098E, 0x6848 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0000 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0x8400 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0006 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3B 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0041 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3C 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0058 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]

  {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};


//  WB-CWF = incandescent
static struct sensor_reg sensor_awb_incandescent_mt9t113[] = {
	{ 0x098E, 0x6848 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0000 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0x8400 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0006 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3B 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0048 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3C 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0036 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

//  WB-A28 = fluorescent
static struct sensor_reg sensor_awb_fluorescent_mt9t113[] = {
	{ 0x098E, 0x6848 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0000 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0x8400 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0006 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3B 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x005F 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3C 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x002E 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]

   {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_cloudy_mt9t113[] = {
	{ 0x098E, 0x6848 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0000 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0x8400 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0006 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3B 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0070 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x098E, 0xAC3C 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
	{ 0x0990, 0x0029 	, WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]

  {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_awb_sunset_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_awb_mt9t113[6] =
{
	sensor_awb_auto_mt9t113,
	sensor_awb_daylight_mt9t113,
	sensor_awb_incandescent_mt9t113,
	sensor_awb_fluorescent_mt9t113,
	sensor_awb_cloudy_mt9t113,
	sensor_awb_sunset_mt9t113
	
};


static struct sensor_reg sensor_iso_auto_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_iso_100_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_iso_200_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_iso_400_mt9t113[] = {
	{MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_iso_mt9t113[4] =
{
	sensor_iso_auto_mt9t113,
	sensor_iso_100_mt9t113,
	sensor_iso_200_mt9t113,
	sensor_iso_400_mt9t113
};


static struct sensor_reg sensor_effect_normal_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_gray_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_negative_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_sepia_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_sharpness_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_effect_sketch_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_effect_mt9t113[6] =
{
	sensor_effect_normal_mt9t113,
	sensor_effect_gray_mt9t113,
	sensor_effect_negative_mt9t113,
	sensor_effect_sepia_mt9t113,
	sensor_effect_sharpness_mt9t113,
	sensor_effect_sketch_mt9t113,
};


static struct sensor_reg sensor_reg_flipnone_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_reg_hflip_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_reg_vflip_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_reg_hvflip_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_flip_mt9t113[4] =
{
	sensor_reg_flipnone_mt9t113,
	sensor_reg_hflip_mt9t113,
	sensor_reg_vflip_mt9t113,
	sensor_reg_hvflip_mt9t113,
};


static struct sensor_reg sensor_secne_auto_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_night_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_landscape_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_portrait_mt9t113[] = {
	{MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_secne_sport_mt9t113[] = {
	{MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_scene_mt9t113[5] =
{
	sensor_secne_auto_mt9t113,
	sensor_secne_night_mt9t113,
	sensor_secne_landscape_mt9t113,
	sensor_secne_portrait_mt9t113,
	sensor_secne_sport_mt9t113
};

static struct sensor_reg sensor_me_mtrix_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_me_center_weighted_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_me_spot_mt9t113[] = {
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_metering_exposure_mt9t113[3] =
{
	sensor_me_mtrix_mt9t113,
	sensor_me_center_weighted_mt9t113,
	sensor_me_spot_mt9t113
};

/////////////////////////////////////////////////////////////
// when focus photo catpure
// 1st 
// sensor_af_single_mt9t113() : normal focus
// or
// sensor_af_closeness_mt9t113() : closeness focus   (e.g.) when name card be captured
//
// 2nd    
// polling 0x0004 read. polling time     (e.g.) 10ms (as your select)
//{ 0x098E, 0x3000    , WORD_LEN},   // polling the AF states
//{ 0x0990, read_vlaue    , WORD_LEN},   // read_value == 0x004
//
// 3rd 
// if(read == 0x004) OK?
// true : Capture
// false : go to 2nd (polling)
////////////////////////////////////////////////////////////////

static struct sensor_reg sensor_af_single_mt9t113[] = {
	//[AF - Trigger]
#if 1
//;Full_Scan_Trigger
// Load internal VCM
    { 0x098E, 0x4403 	, WORD_LEN},// MCU_ADDRESS [AFM_ALGO]
    { 0x0990, 0x8008 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0x8400 	, WORD_LEN},// MCU_ADDRESS [SEQ_CMD]
    { 0x0990, 0x0005 	, WORD_LEN},// MCU_DATA_0

//full scan mode
    { 0x098E, 0x3003 	, WORD_LEN},// MCU_ADDRESS [AF_ALGO]
    { 0x0990, 0x0002 	, WORD_LEN},// MCU_DATA_0

//total step=10
    { 0x098E, 0xB00B 	, WORD_LEN},// MCU_ADDRESS [AF_STEP_SIZE]
    { 0x0990, 0x000A 	, WORD_LEN},// MCU_DATA_0

//[Infinite AF]//set Manual focus range=0~100 (00~64)
    { 0x098E, 0xB00C 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_0]
    { 0x0990, 0x0000 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB00D 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_1]
    { 0x0990, 0x000A 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB00E 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_2]
    { 0x0990, 0x0014 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB00F 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_3]
    { 0x0990, 0x001E 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB010 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_4]
    { 0x0990, 0x0028 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB011 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_5]
    { 0x0990, 0x0032 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB012 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_6]
    { 0x0990, 0x003C 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB013 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_7]
    { 0x0990, 0x0046 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB014 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_8]
    { 0x0990, 0x0050 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB015 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_9]
    { 0x0990, 0x005A 	, WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB016 	, WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_10]
    { 0x0990, 0x0064 	, WORD_LEN},// MCU_DATA_0
#endif
// 2ND_SCAN setting info
	{ 0x098E, 0xB002	, WORD_LEN},
	{ 0x0990, 0x0003	, WORD_LEN},
//[Trigger]
    { 0x098E, 0xB008 	, WORD_LEN},// MCU_ADDRESS [AF_PROGRESS]
    { 0x0990, 0x0001 	, WORD_LEN},// MCU_DATA_0

    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_af_closeness_mt9t113[] = {
        //[AF - Trigger]
#if 1
//;Full_Scan_Trigger
// Load internal VCM
    { 0x098E, 0x4403        , WORD_LEN},// MCU_ADDRESS [AFM_ALGO]
    { 0x0990, 0x8008        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0x8400        , WORD_LEN},// MCU_ADDRESS [SEQ_CMD]
    { 0x0990, 0x0005        , WORD_LEN},// MCU_DATA_0

//full scan mode
    { 0x098E, 0x3003        , WORD_LEN},// MCU_ADDRESS [AF_ALGO]
    { 0x0990, 0x0002        , WORD_LEN},// MCU_DATA_0

//total step=10
    { 0x098E, 0xB00B        , WORD_LEN},// MCU_ADDRESS [AF_STEP_SIZE]
    { 0x0990, 0x000A        , WORD_LEN},// MCU_DATA_0

//[Marco AF]//set Manual focus range=0~100 (00~64)
    { 0x098E, 0xB00C        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_0]
    { 0x0990, 0x0064        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB00D        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_1]
    { 0x0990, 0x0072        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB00E        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_2]
    { 0x0990, 0x0080        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB00F        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_3]
    { 0x0990, 0x008E        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB010        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_4]
    { 0x0990, 0x009C        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB011        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_5]
    { 0x0990, 0x00AA        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB012        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_6]
    { 0x0990, 0x00B8        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB013        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_7]
    { 0x0990, 0x00C6        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB014        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_8]
    { 0x0990, 0x00D4        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB015        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_9]
    { 0x0990, 0x00E2        , WORD_LEN},// MCU_DATA_0
    { 0x098E, 0xB016        , WORD_LEN},// MCU_ADDRESS [AF_POSITIONS_10]
    { 0x0990, 0x00F0        , WORD_LEN},// MCU_DATA_0
#endif
// 2ND_SCAN setting info
	{ 0x098E, 0xB002		, WORD_LEN},
	{ 0x0990, 0x0003		, WORD_LEN},
//[Trigger]
    { 0x098E, 0xB008        , WORD_LEN},// MCU_ADDRESS [AF_PROGRESS]
    { 0x0990, 0x0001        , WORD_LEN},// MCU_DATA_0

    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

static struct sensor_reg sensor_af_manual_mt9t113[] = {

    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};

struct sensor_reg* sensor_reg_af_mt9t113[2] =
{
	sensor_af_single_mt9t113,
    sensor_af_closeness_mt9t113
//	sensor_af_manual_mt9t113
};

static struct sensor_reg sensor_sw_standby_mt9t113[] = {
	
    {MT9T113_REG_TERM, MT9T113_VAL_TERM, WORD_LEN}
};


static struct sensor_reg sensor_exposure_L2_mt9t113[] = {
	{0x098E, 0xE826 ,WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{0x0990, 0x004B ,WORD_LEN},// MCU_DATA_0

    {MT9T113_REG_TERM, MT9T113_VAL_TERM}
};

static struct sensor_reg sensor_exposure_L1_mt9t113[] = {
    {0x098E, 0xE826 ,WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{0x0990, 0x003C ,WORD_LEN},// MCU_DATA_0

    {MT9T113_REG_TERM, MT9T113_VAL_TERM}
};

//default
static struct sensor_reg sensor_exposure_L0_mt9t113[] = {
    {0x098E, 0xE826 ,WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{0x0990, 0x002D ,WORD_LEN},// MCU_DATA_0

    {MT9T113_REG_TERM, MT9T113_VAL_TERM}
};

static struct sensor_reg sensor_exposure_LN1_mt9t113[] = {
    {0x098E, 0xE826 ,WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{0x0990, 0x001E ,WORD_LEN},// MCU_DATA_0
	
    {MT9T113_REG_TERM, MT9T113_VAL_TERM}
};

static struct sensor_reg sensor_exposure_LN2_mt9t113[] = {
    {0x098E, 0xE826 ,WORD_LEN},// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_BASE_TARGET]
	{0x0990, 0x000F ,WORD_LEN},// MCU_DATA_0

    {MT9T113_REG_TERM, MT9T113_VAL_TERM}
};

struct sensor_reg* sensor_reg_exposure_mt9t113[5] =
{	
	sensor_exposure_L2_mt9t113,
	sensor_exposure_L1_mt9t113,
	sensor_exposure_L0_mt9t113,
	sensor_exposure_LN1_mt9t113,
	sensor_exposure_LN2_mt9t113	
};


static int write_regs_mt9t113(const struct sensor_reg reglist[])
{
	int err;
	int err_cnt = 0;	
	int sleep_cnt = 100;
	unsigned char data[132];
	unsigned char bytes;
	const struct sensor_reg *next = reglist;

	while (!((next->reg == MT9T113_REG_TERM) && (next->val == MT9T113_VAL_TERM)))
	{
		
		if(next->reg == MT9T113_REG_TERM && next->val != MT9T113_VAL_TERM)
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

#if 0
static int write_serial_regs_mt9t113(const struct sensor_serial_reg reglist[])
{
	int err;
	int err_cnt = 0;	
	int sleep_cnt = 100;
	unsigned char data[132];
	unsigned char bytes;
	const struct sensor_serial_reg *next = reglist;
	
	while (!((next->reg == MT9T113_REG_TERM) && (next->width== MS_DELAY)))
	{
		
		if(next->reg != MT9T113_REG_TERM && next->width== MS_DELAY)
		{
			//mdelay(next->val);
			msleep(next->reg);
			sleep_cnt = 100;
			printk("Sensor init Delay[%d]!!!! \n", next->reg);
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

			if(next->width == WORD_LEN){			
				bytes = 0;
				data[bytes]= next->reg>>8;		bytes++;		
				data[bytes]= (u8)next->reg&0xff; 	bytes++;

				//data[bytes]= next->val>>8;		bytes++;		
				//data[bytes]= (u8)next->val&0xff; 	bytes++;

				err = DDI_I2C_Write(data, 2, bytes-2);
			}
			else{
				bytes = 0;
				//data[bytes]= next->reg>>8;		bytes++;		
				data[bytes]= (u8)next->reg&0xff; 	bytes++;

				//data[bytes]= next->val;		bytes++;		
				//data[bytes]= (u8)next->val&0xff; 	bytes++;

				err = DDI_I2C_Write(data, 1, 0);				
			}
			
			
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
#endif

static int sensor_open_mt9t113(bool bChangeCamera)
{

	unsigned char data_r[2];
	unsigned short data_read;
	unsigned short input_reg = 0x0000;
	
	if(bChangeCamera){		// for Dual Camera.
		CIF_Open();
		sensor_delay(40);
		
		sensor_reset_low();
		
		sensor_powerdown_disable();
		msleep(10);

		sensor_reset_high();
		sensor_delay(10);
	}
	else{					// for Single Camera 
		sensor_power_disable();
		sensor_delay(100);
		
		sensor_power_enable();
		sensor_delay(100);

		sensor_powerdown_disable();
		sensor_delay(100);

		sensor_reset_low();
		sensor_delay(800);

		CIF_Open();
		sensor_delay(500);

		sensor_reset_high();
		sensor_delay(500);	
	}

	#if 0
		DDI_I2C_Read(input_reg, 2, data_r, 2);
		data_read = data_r[1];
		data_read = (data_read<<8)&0xff00 | data_r[0];

		printk("data_read = 0x%x \n", data_read);
	#endif

	return write_regs_mt9t113(sensor_reg_common_mt9t113[0]);
}

static int sensor_close_mt9t113(void)
{
	unsigned char data_r[2];
	unsigned short data_read;
	unsigned short input_reg = 0x0000;
	
	DDI_I2C_Read(input_reg, 2, data_r, 2);
	data_read = data_r[1];
	data_read = (data_read<<8)&0xff00 | data_r[0];

	//printk("close! data_read = 0x%x \n", data_read);

	//write_regs_mt9t113(sensor_sw_standby_mt9t113);
	msleep(10);	

	
	CIF_ONOFF(OFF);

	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_enable();

	CIF_Close();
	msleep(5);

	return 0;
}

static int sensor_powerdown_mt9t113(bool bChangeCamera)
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

		//write_regs_mt9t113(sensor_sw_standby_mt9t113);
		msleep(10);	

		sensor_powerdown_enable();
		msleep(10);

	
		//printk("mt9t113 change camera powerdown! \n");
	}
	else{		
		sensor_reset_low();
		sensor_power_disable();
		sensor_powerdown_enable();
			
		//printk("mt9t113 first/last camera powerdown! \n");
	}

	return 0;
}

static int sensor_preview_mt9t113(void)
{
	return write_regs_mt9t113(sensor_reg_common_mt9t113[1]);
}

static int sensor_capture_mt9t113(void)  
{
	return write_regs_mt9t113(sensor_reg_common_mt9t113[2]);
//	return write_regs_mt9t113(sensor_reg_af_mt9t113[0]);
}

static int sensor_capturecfg_mt9t113(int width, int height)
{
	return 0;
}

static int sensor_zoom_mt9t113(int val)
{
	return 0;
}

// val == 0 : normal focus
// val == 1 : closeness focus
static int sensor_autofocus_mt9t113(int val)  
{
	if(bEnableAutoFocus){
		printk("sensor AutoFocus \n");
		return write_regs_mt9t113(sensor_reg_af_mt9t113[0]);
	}
	else{
		printk("sensor Closeness Focus \n");
		return write_regs_mt9t113(sensor_reg_af_mt9t113[1]);
	}
}

static int sensor_effect_mt9t113(int val)
{		
	return write_regs_mt9t113(sensor_reg_effect_mt9t113[val]);
}

static int sensor_flip_mt9t113(int val)
{
	return write_regs_mt9t113(sensor_reg_flip_mt9t113[val]);
}

static int sensor_iso_mt9t113(int val)
{
	return write_regs_mt9t113(sensor_reg_iso_mt9t113[val]);
}

static int sensor_me_mt9t113(int val)
{
	return write_regs_mt9t113(sensor_reg_metering_exposure_mt9t113[val]);
}

static int sensor_wb_mt9t113(int val)
{
	return write_regs_mt9t113(sensor_reg_awb_mt9t113[val]);
}

static int sensor_bright_mt9t113(int val)
{
	return write_regs_mt9t113(sensor_reg_brightness_mt9t113[val]);
}

static int sensor_scene_mt9t113(int val)
{
	return write_regs_mt9t113(sensor_reg_scene_mt9t113[val]);
}

static int sensor_focus_mode_mt9t113(int val)
{
	printk("sensor focus mode[%d] \n", val);
	if(val == 0)
		bEnableAutoFocus = true;
	else
		bEnableAutoFocus = false;
	
	return 0;
}
static int sensor_check_esd_mt9t113(int val)
{
	return 0;
}

static int sensor_exposure_mt9t113(int val)
{
	printk("sensor exposure value[%d] \n", val);

	switch(val){
		case 20: val = 0; break;
		case 10: val = 1; break;
		case 0: val = 2; break;
		case -10: val = 3; break;
		case -20: val = 4; break;
		default: val = 2; break;
	}
	
	return write_regs_mt9t113(sensor_reg_exposure_mt9t113[val]);
}

static int sensor_check_luma_mt9t113(int val)
{
	return 0;
}

/**********************************************************
*    Function  
**********************************************************/
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void sensor_info_init_mt9t113(TCC_SENSOR_INFO_TYPE *sensor_info)
{
	sensor_info->i2c_addr 				= 0x7A;
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
	sensor_info->m_clock 					= 247500;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 1440000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else  // CONFIG_USE_ISP
	sensor_info->m_clock 					= 247500;
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
	sensor_info->cam_capchg_width 		= 2048;
	sensor_info->framerate				= 15;
	sensor_info->p_clock_pol 				= POSITIVE_EDGE;//NEGATIVE_EDGE;//
	sensor_info->v_sync_pol 				= ACT_HIGH;
	sensor_info->h_sync_pol                 = ACT_HIGH;
	#if defined(CONFIG_ARCH_TCC892X)
	sensor_info->de_pol                 = ACT_LOW;
	#endif	
	sensor_info->format 					= M420_ZERO;
	sensor_info->capture_skip_frame 		= 1;
	sensor_info->sensor_sizes 			= sensor_sizes_mt9t113;
}

void sensor_init_fnc_mt9t113(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_mt9t113;
	sensor_func->Close 					= sensor_close_mt9t113;
	sensor_func->PowerDown				= sensor_powerdown_mt9t113;

	sensor_func->Set_Preview 			= sensor_preview_mt9t113;
	sensor_func->Set_Capture 			= sensor_capture_mt9t113;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_mt9t113;

	sensor_func->Set_Zoom 				= sensor_zoom_mt9t113;
	sensor_func->Set_AF 				= sensor_autofocus_mt9t113;
	sensor_func->Set_Effect 				= sensor_effect_mt9t113;
	sensor_func->Set_Flip 				= sensor_flip_mt9t113;
	sensor_func->Set_ISO 				= sensor_iso_mt9t113;
	sensor_func->Set_ME 				= sensor_me_mt9t113;
	sensor_func->Set_WB 				= sensor_wb_mt9t113;
	sensor_func->Set_Bright 				= sensor_bright_mt9t113;
	sensor_func->Set_Scene 				= sensor_scene_mt9t113;
	sensor_func->Set_Exposure			= sensor_exposure_mt9t113;
	sensor_func->Set_FocusMode			= sensor_focus_mode_mt9t113;

	sensor_func->Check_ESD 				= sensor_check_esd_mt9t113;
	sensor_func->Check_Luma 			= sensor_check_luma_mt9t113;
}
#else
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_mt9t113;
	sensor_func->Close 					= sensor_close_mt9t113;
	sensor_func->PowerDown				= sensor_powerdown_mt9t113;

	sensor_func->Set_Preview 			= sensor_preview_mt9t113;
	sensor_func->Set_Capture 			= sensor_capture_mt9t113;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_mt9t113;

	sensor_func->Set_Zoom 				= sensor_zoom_mt9t113;
	sensor_func->Set_AF 				= sensor_autofocus_mt9t113;
	sensor_func->Set_Effect 				= sensor_effect_mt9t113;
	sensor_func->Set_Flip 				= sensor_flip_mt9t113;
	sensor_func->Set_ISO 				= sensor_iso_mt9t113;
	sensor_func->Set_ME 				= sensor_me_mt9t113;
	sensor_func->Set_WB 				= sensor_wb_mt9t113;
	sensor_func->Set_Bright 				= sensor_bright_mt9t113;
	sensor_func->Set_Scene 				= sensor_scene_mt9t113;
	sensor_func->Set_Exposure			= sensor_exposure_mt9t113;
	sensor_func->Set_FocusMode			= sensor_focus_mode_mt9t113;

	sensor_func->Check_ESD 				= sensor_check_esd_mt9t113;
	sensor_func->Check_Luma 			= sensor_check_luma_mt9t113;
}
#endif
#endif

