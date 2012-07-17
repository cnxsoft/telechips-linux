/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : CAMERA INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCC92XX
*    SOURCE        : isx006_5mp.c
*
*    START DATE    : 2010. 4. 1.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3-2 TEAM
*                : TELECHIPS, INC.
* 
************************************************************************/


#if defined(CONFIG_ARCH_TCC92XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif
#include "tcc_cam_i2c.h"
#include <asm/delay.h>
#include <linux/delay.h>
#include "sensor_if.h"
#include "cam.h"

#include <linux/jiffies.h>
extern unsigned long volatile __jiffy_data jiffies;

static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "isx600_5mp: " msg); }

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_ISX006

/* Array of image sizes supported by ISX600.  These must be ordered from 
 * smallest image size to largest.
 */
struct capture_size sensor_sizes[] = {
	{ 2560, 1920 }, // CAM_5M	
	{ 2048, 1536 },	//CAM_QXGA
	{ 1600, 1200 },	//CAM_UXGA
	{ 1280,  960 },	//CAM_SXGA
	{ 1024,  768 },	//CAM_XGA
	{  800,  600 },	//CAM_SVGA
	{  800,  480 },	//CAM_Wallpaper
	{  640,  480 },	//CAM_VGA
	{  320,  240 },	//CAM_QVGA	
};

#if defined(CONFIG_MACH_TCC9200S_M340EVB)
#define MAIN_SENSOR_PWRDN_ENABLE()		(HwGPIOD->GPEN |= Hw11);	(HwGPIOD->GPDAT |= Hw11);  // ISP Core Power Enable
#define MAIN_SENSOR_PWRDN_DISABLE()		(HwGPIOD->GPEN |= Hw11);	(HwGPIOD->GPDAT &= ~Hw11); //
#define SUB_SENSOR_PWRDN_ENABLE() 		(HwGPIOA->GPEN |= Hw13);	(HwGPIOA->GPDAT |= Hw13);
#define SUB_SENSOR_PWRDN_DISABLE() 		(HwGPIOA->GPEN |= Hw13);	(HwGPIOA->GPDAT &= ~Hw13);

#define SENSOR_RESET_HIGH()				(HwGPIOE->GPEN |= Hw3);		(HwGPIOE->GPDAT |= Hw3);
#define SENSOR_RESET_LOW()				(HwGPIOE->GPEN |= Hw3);		(HwGPIOE->GPDAT &= ~Hw3);

//put the power using pmic.
#define SENSOR_CORE_PWR_ON()			sensor_pwr_control(CAM_CORE, 1);			
#define SENSOR_CORE_PWR_OFF()			sensor_pwr_control(CAM_CORE, 0);
#define SENSOR_IO_PWR_ON()			 	sensor_pwr_control(CAM_IO, 1);
#define SENSOR_IO_PWR_OFF()		 		sensor_pwr_control(CAM_IO, 0);
#define SENSOR_AF_PWR_ON()		 		sensor_pwr_control(CAM_ETC, 1);  //AF
#define SENSOR_AF_PWR_OFF()		 		sensor_pwr_control(CAM_ETC, 0);  //AF
#else // We need to implement gpio for TCC Demo-board.
#define MAIN_SENSOR_PWRDN_ENABLE()		
#define MAIN_SENSOR_PWRDN_DISABLE()		
#define SUB_SENSOR_PWRDN_ENABLE() 		
#define SUB_SENSOR_PWRDN_DISABLE() 		

#define SENSOR_RESET_HIGH()				
#define SENSOR_RESET_LOW()				

//put the power using pmic.
#define SENSOR_CORE_PWR_ON()			
#define SENSOR_CORE_PWR_OFF()			
#define SENSOR_IO_PWR_ON()			 	
#define SENSOR_IO_PWR_OFF()		 		
#define SENSOR_AF_PWR_ON()		 		
#define SENSOR_AF_PWR_OFF()		 		
#endif

#define WRITE_RETRY_COUNT 3 //  Write 실패시 재시도 횟수 


/* register initialization tables for sensor */
/* common sensor register initialization for all image sizes, pixel formats, 
 * and frame rates
 */
static struct sensor_reg sensor_initialize[] = {
#if 1
// ISX006-1 (FW 3013)
//   Parallel    15fps
//        PLL   432MHz
//        DCK   108MHz
//
// inifile
// size address data
//
	{0x000b, 0x00,	8},  		// PLL_CKSEL         
	{0x02C7, 0x00,	8},			// SRCCK_DIV         
	{0x02B8, 0x00,	8},			// VIF_CLKCONFIG1    ( YUV     : 72MHz )
	{0x02B9, 0x40,	8},			// VIF_CLKCONFIG2    ( Preview : 108MHz )  
	{0x02BA, 0x50,	8}, 		// VIF_CLKCONFIG3    
	{0x02BB, 0x50,	8}, 		// VIF_CLKCONFIG4    
	{0x02BC, 0x10,	8}, 		// VIF_CLKCONFIG5    
	{0x02BD, 0x50,	8}, 		// VIF_CLKCONFIG6    
	{0x02BE, 0x00,	8}, 		// VIF_CLKCONFIG7    
	{0x02C0, 0x00,	8}, 		// VIF_CLKCONFIG9    ( Interleave : 72MHz )
	{0x02C1, 0x00,	8}, 		// VIF_CLKCONFIG10   
	{0x4C10, 0x00,	8}, 		// VIF_CLKCONFIG13   
	{0x033E, 0x01,	8},			// HD_PLL_CKSEL      
	{0x0348, 0x00,	8}, 		// VIF_CLKCONFIG_HD1 
	{0x0349, 0x00,	8}, 		// VIF_CLKCONFIG_HD2 
	{0x034A, 0x01,	8},			// VIF_CLKCONFIG_HD3 
	{0x034B, 0x01,	8},			// VIF_CLKCONFIG_HD4 
	{0x4044, 0x21C0,16},		// 1SECCLK_ALL
	{0x4046, 0x2A30,16},		// 1SECCLK_HD_1_1
	{0x4048, 0x32A0,16},		// 1SECCLK_HD_1_2
	{0x402B, 0x99,	8},			// FPS_SENSALL
	{0x402C, 0x012F,16},		// FPS_SENS_HD_1_1
	{0x4050, 0x025D,16},		// FPS_SENS_HD_1_2
	{0x4060, 0x21C0,16},		// PIXEL_CLK
	{0x4062, 0x2A30,16},		// PIXEL_CLK_HD_1_1
	{0x4064, 0x32A0,16},		// PIXEL_CLK_HD_1_2
	{0x403E, 0x22E3,16},		// FLSHT50
	{0x4074, 0x242D,16},		// FLSHT50_HD_1_1
	{0x4078, 0x253A,16},		// FLSHT50_HD_1_2
	{0x4040, 0x21D6,16},		// FLSHT60
	{0x4076, 0x2320,16},		// FLSHT60_HD_1_1
	{0x407A, 0x242D,16},		// FLSHT60_HD_1_2
	{0x02A2, 0x0000,16},		// VADJ_SENS_1_1
	{0x0364, 0x0027,16},		// VADJ_SENS_HD_1_1
	{0x02A4, 0x0000,16},		// VADJ_SENS_1_2
	{0x0366, 0x0000,16},		// VADJ_SENS_HD_1_2
	{0x02A6, 0x0000,16},		// VADJ_SENS_1_4
	{0x02A8, 0x0000,16},		// VADJ_SENS_1_8
	{0x4C14, 0x0440,16},		// FLC_OPD_HEIGHT_NORMAL_1_1
	{0x4C1C, 0x0420,16},		// FLC_OPD_HEIGHT_HD_1_1
	{0x4C16, 0x03C0,16},		// FLC_OPD_HEIGHT_NORMAL_1_2
	{0x4C1E, 0x02C0,16},		// FLC_OPD_HEIGHT_HD_1_2
	{0x4C18, 0x01E0,16},		// FLC_OPD_HEIGHT_NORMAL_1_4
	{0x4C1A, 0x00E0,16},		// FLC_OPD_HEIGHT_NORMAL_1_8
	{0x02C4, 0x010A,16},		// GPIO_FUNCSEL      : AF PWM & UART enable
	{0x4C86, 0x01,	8},			// VIF_DATA_MODE     : USESRAM=1, ASYNCHS=0, RGBCCENTOR=0
	{0x001C, 0x00,	8},			// FMTTYPE_MONI 
	{0x0022, 0x0500,16},		// HSIZE_MONI  1280
	{0x0028, 0x02D0,16},		// HSIZE_MONI  720
//	{0x0022, 0x0320, 16}, 	// HSIZE_MONI - 800
//	{0x0028, 0x0258, 16}, 	// VSIZE_MONI - 600	
	{0x0380, 0x01,	8},			// SENSMODE_MONI
	{0x0383, 0x02,	8},			// FPSTYE_MONI
	{0x0009, 0x16,	8},			// INCK_SET  24MHz

	{REG_TERM, 0x20 , 0}, //delay 32ms


#if 1
	// USERCTRL FOR AE & GAMMA
	{0x0105, 0x80,8},   //SCENE1 GAMMA SELECT FOR KNOT DATA COPY 
	{0x0387, 0x01,8},   //MAIN GAMMA G-KNOT DATA COPY TO R-KNOT AND B-KNOT 

	///////////////////////////////////
	//    CAT.:ADJ                 ////
	///////////////////////////////////
	{0x4A04, 0x0FD1,16},   //5800K NORMALIZED COEFFICIENT R ADJUST THIS USING THE PRE-WHITE BALANCE FUNCTION OF THE ADJUSTMENT MODE. 1.0 = 1000H. WHEN 0 IS SET, IT IS REGARDED AS 1.
	{0x4A06, 0x0EF8,16},   //5800K NORMALIZED COEFFICIENT B ADJUST THIS USING THE PRE-WHITE BALANCE FUNCTION OF THE ADJUSTMENT MODE. 1.0 = 1000H. WHEN 0 IS SET, IT IS REGARDED AS 1.
	{0x4A08, 0x0134,16},   //3200K R GAIN ADJUST THIS USING THE PRE-WHITE BALANCE FUNCTION OF THE ADJUSTMENT MODE. 1.0 = 100H. WHEN 0 IS SET, IT IS REGARDED AS 1. WHEN THE VALUE LARGER THAN FFFH IS SET, IT IS REGARDED AS FFFH.
	{0x4A0A, 0x0248,16},   //3200K B GAIN ADJUST THIS USING THE PRE-WHITE BALANCE FUNCTION OF THE ADJUSTMENT MODE. 1.0 = 100H. WHEN 0 IS SET, IT IS REGARDED AS 1. WHEN THE VALUE LARGER THAN FFFH IS SET, IT IS REGARDED AS FFFH.
	{0x4A0C, 0x00,8},   //ADJUSTMENT MODES
	{0x4A0D, 0x00,8},   //ADJUSTMENT SUB-MODES
	{0x4A0E, 0x05,8},   //PRE-WHITE BARALCE OPD ERROR RETRY FRAME NUMBER THIS IS THE NUMBER OF FRAMES WHICH WAIT IGNORING THE OPDERR ACKNOWLEDGEMENT WHEN THE ADJUSTMENT INSTRUCTION IS EARLIER THAN THE OPD CONVERGENCE AFTER THE CHANGE OF THE LIGHT SOURCE.
	{0x4A10, 0x0A83,16},   //AWB PRE-WHITE BALANCE ADJUSTMENT C14 R/G COORDINATE SETTING
	{0x4A12, 0x1C2D,16},   //AWB PRE-WHITE BALANCE ADJUSTMENT C14 B/G COORDINATE SETTING
	{0x4A14, 0x00,8},   //REFERENCE SENSITIVITY RATIO OF SENSOR (R/G)
	{0x4A15, 0x00,8},   //REFERENCE SENSITIVITY RATIO OF SENSOR (B/G)
	{0x4A16, 0x0000,16},   //OFFSET VALUE FOR FINE ADJUSTMENT (R COMPONENT)
	{0x4A18, 0x0000,16},   //OFFSET VALUE FOR FINE ADJUSTMENT (B COMPONENT)
	{0x4A1A, 0x0400,16},   //SHD SETTING, KNOT MINIMUM VALUE, R
	{0x4A1C, 0x0400,16},   //SHD SETTING, KNOT MINIMUM VALUE, GR
	{0x4A1E, 0x0400,16},   //SHD SETTING, KNOT MINIMUM VALUE, GB
	{0x4A20, 0x0400,16},   //SHD SETTING, KNOT MINIMUM VALUE, B
	{0x4A22, 0x0000,16},   //SHD, NORMR OFFSET, FOR R2
	{0x4A24, 0x0013,16},   //SHD, PRER OFFSET, FOR R2
	{0x4A26, 0x0000,16},   //SHD, NORMR OFFSET, FOR RB
	{0x4A28, 0xFFF1,16},   //SHD, PRER OFFSET, FOR RB
	{0x4A2A, 0x0000,16},   //SHD, NORMB OFFSET, FOR RB
	{0x4A2C, 0xFFF2,16},   //SHD, PREB OFFSET, FOR RB
 
	///////////////////////////////////
	//CAT.:CXC_DATA(PRE-SHADING)////
	///////////////////////////////////
	{0x9000, 0x8282,16},	//CXC_VALID_CODE
	{0x9002, 0x02,8},
	{0x9003, 0x12,8},
	{0x9004, 0x08,8},
	{0x9005, 0x48,8},
	{0x9006, 0x00,8},
	{0x9007, 0x20,8},
	{0x9008, 0x01,8},
	{0x9009, 0x00,8},
	{0x900A, 0x05,8},
	{0x900B, 0x02,8},
	{0x900C, 0x16,8},
	{0x900D, 0x00,8},
	{0x900E, 0x40,8},
	{0x900F, 0x00,8},
	{0x9010, 0x60,8},
	{0x9011, 0x81,8},
	{0x9012, 0x80,8},
	{0x9013, 0x05,8},
	{0x9014, 0x02,8},
	{0x9015, 0x16,8},
	{0x9016, 0x08,8},
	{0x9017, 0x48,8},
	{0x9018, 0x20,8},
	{0x9019, 0x20,8},
	{0x901A, 0x01,8},
	{0x901B, 0x80,8},
	{0x901C, 0x04,8},
	{0x901D, 0x00,8},
	{0x901E, 0x14,8},
	{0x901F, 0x08,8},
	{0x9020, 0x58,8},
	{0x9021, 0x00,8},
	{0x9022, 0x00,8},
	{0x9023, 0x01,8},
	{0x9024, 0x80,8},
	{0x9025, 0x05,8},
	{0x9026, 0x02,8},
	{0x9027, 0x16,8},
	{0x9028, 0x08,8},
	{0x9029, 0x58,8},
	{0x902A, 0x20,8},
	{0x902B, 0xE0,8},
	{0x902C, 0x80,8},
	{0x902D, 0x80,8},
	{0x902E, 0x03,8},
	{0x902F, 0x02,8},
	{0x9030, 0x10,8},
	{0x9031, 0x08,8},
	{0x9032, 0x40,8},
	{0x9033, 0x00,8},
	{0x9034, 0xE0,8},
	{0x9035, 0x40,8},
	{0x9036, 0x80,8},
	{0x9037, 0x04,8},
	{0x9038, 0x03,8},
	{0x9039, 0x12,8},
	{0x903A, 0x08,8},
	{0x903B, 0x58,8},
	{0x903C, 0x20,8},
	{0x903D, 0x60,8},
	{0x903E, 0x81,8},
	{0x903F, 0x00,8},
	{0x9040, 0x04,8},
	{0x9041, 0x02,8},
	{0x9042, 0x10,8},
	{0x9043, 0x08,8},
	{0x9044, 0x28,8},
	{0x9045, 0x30,8},
	{0x9046, 0xA0,8},
	{0x9047, 0x00,8},
	{0x9048, 0x01,8},
	{0x9049, 0x02,8},
	{0x904A, 0x02,8},
	{0x904B, 0x0C,8},
	{0x904C, 0x0C,8},
	{0x904D, 0x30,8},
	{0x904E, 0x10,8},
	{0x904F, 0x20,8},
	{0x9050, 0x41,8},
	{0x9051, 0x80,8},
	{0x9052, 0x04,8},
	{0x9053, 0x04,8},
	{0x9054, 0x0A,8},
	{0x9055, 0x10,8},
	{0x9056, 0x28,8},
	{0x9057, 0x30,8},
	{0x9058, 0x80,8},
	{0x9059, 0xC0,8},
	{0x905A, 0x80,8},
	{0x905B, 0x01,8},
	{0x905C, 0x01,8},
	{0x905D, 0x00,8},
	{0x905E, 0x0C,8},
	{0x905F, 0x10,8},
	{0x9060, 0x20,8},
	{0x9061, 0xA0,8},
	{0x9062, 0xC0,8},
	{0x9063, 0x00,8},
	{0x9064, 0x03,8},
	{0x9065, 0x03,8},
	{0x9066, 0x0C,8},
	{0x9067, 0x0C,8},
	{0x9068, 0x18,8},
	{0x9069, 0x30,8},
	{0x906A, 0x60,8},
	{0x906B, 0x00,8},
	{0x906C, 0x81,8},
	{0x906D, 0x00,8},
	{0x906E, 0x04,8},
	{0x906F, 0x00,8},
	{0x9070, 0x08,8},
	{0x9071, 0x00,8},
	{0x9072, 0x20,8},
	{0x9073, 0x00,8},
	{0x9074, 0x80,8},
	{0x9075, 0x00,8},
	{0x9076, 0x01,8},
	{0x9077, 0x04,8},
	{0x9078, 0x0C,8},
	{0x9079, 0x10,8},
	{0x907A, 0x30,8},
	{0x907B, 0x10,8},
	{0x907C, 0x60,8},
	{0x907D, 0x40,8},
	{0x907E, 0x80,8},
	{0x907F, 0x01,8},
	{0x9080, 0x01,8},
	{0x9081, 0x00,8},
	{0x9082, 0x08,8},
	{0x9083, 0x00,8},
	{0x9084, 0x30,8},
	{0x9085, 0x20,8},
	{0x9086, 0x40,8},
	{0x9087, 0x00,8},
	{0x9088, 0x00,8},
	{0x9089, 0x01,8},
	{0x908A, 0x04,8},
	{0x908B, 0x00,8},
	{0x908C, 0x20,8},
	{0x908D, 0x00,8},
	{0x908E, 0x80,8},
	{0x908F, 0xC0,8},
	{0x9090, 0x80,8},
	{0x9091, 0x01,8},
	{0x9092, 0x03,8},
	{0x9093, 0x06,8},
	{0x9094, 0x08,8},
	{0x9095, 0x00,8},
	{0x9096, 0x30,8},
	{0x9097, 0x00,8},
	{0x9098, 0xC0,8},
	{0x9099, 0x00,8},
	{0x909A, 0x00,8},
	{0x909B, 0x03,8},
	{0x909C, 0xFE,8},
	{0x909D, 0x0B,8},
	{0x909E, 0x10,8},
	{0x909F, 0x00,8},
	{0x90A0, 0x80,8},
	{0x90A1, 0x00,8},
	{0x90A2, 0x00,8},
	{0x90A3, 0x02,8},
	{0x90A4, 0x04,8},
	{0x90A5, 0x0C,8},
	{0x90A6, 0x10,8},
	{0x90A7, 0x30,8},
	{0x90A8, 0x40,8},
	{0x90A9, 0x40,8},
	{0x90AA, 0x00,8},
	{0x90AB, 0x81,8},
	{0x90AC, 0x00,8},
	{0x90AD, 0x05,8},
	{0x90AE, 0x02,8},
	{0x90AF, 0x10,8},
	{0x90B0, 0x08,8},
	{0x90B1, 0x20,8},
	{0x90B2, 0x40,8},
	{0x90B3, 0x80,8},
	{0x90B4, 0x00,8},
	{0x90B5, 0x03,8},
	{0x90B6, 0x02,8},
	{0x90B7, 0x0C,8},
	{0x90B8, 0x08,8},
	{0x90B9, 0x28,8},
	{0x90BA, 0x20,8},
	{0x90BB, 0xA0,8},
	{0x90BC, 0x80,8},
	{0x90BD, 0x80,8},
	{0x90BE, 0x02,8},
	{0x90BF, 0x02,8},
	{0x90C0, 0x06,8},
	{0x90C1, 0x08,8},
	{0x90C2, 0x10,8},
	{0x90C3, 0x40,8},
	{0x90C4, 0x60,8},
	{0x90C5, 0x80,8},
	{0x90C6, 0x00,8},
	{0x90C7, 0x03,8},
	{0x90C8, 0x03,8},
	{0x90C9, 0x0E,8},
	{0x90CA, 0x0C,8},
	{0x90CB, 0x38,8},
	{0x90CC, 0x10,8},
	{0x90CD, 0x80,8},
	{0x90CE, 0x40,8},
	{0x90CF, 0x00,8},
	{0x90D0, 0x02,8},
	{0x90D1, 0x04,8},
	{0x90D2, 0x0C,8},
	{0x90D3, 0x18,8},
	{0x90D4, 0x30,8},
	{0x90D5, 0x40,8},
	{0x90D6, 0xA0,8},
	{0x90D7, 0x00,8},
	{0x90D8, 0x81,8},
	{0x90D9, 0x02,8},
	{0x90DA, 0x05,8},
	{0x90DB, 0x0E,8},
	{0x90DC, 0x08,8},
	{0x90DD, 0x30,8},
	{0x90DE, 0x20,8},
	{0x90DF, 0xC0,8},
	{0x90E0, 0x40,8},
	{0x90E1, 0x00,8},
	{0x90E2, 0x02,8},
	{0x90E3, 0x01,8},
	{0x90E4, 0x08,8},
	{0x90E5, 0x10,8},
	{0x90E6, 0x30,8},
	{0x90E7, 0x60,8},
	{0x90E8, 0xC0,8},
	{0x90E9, 0x00,8},
	{0x90EA, 0x81,8},
	{0x90EB, 0x02,8},
	{0x90EC, 0x04,8},
	{0x90ED, 0x0A,8},
	{0x90EE, 0x14,8},
	{0x90EF, 0x38,8},
	{0x90F0, 0x20,8},
	{0x90F1, 0xC0,8},
	{0x90F2, 0x80,8},
	{0x90F3, 0x00,8},
	{0x90F4, 0x03,8},
	{0x90F5, 0x02,8},
	{0x90F6, 0xEE,8},
	{0x90F7, 0x0B,8},
	{0x90F8, 0xB8,8},
	{0x90F9, 0x2F,8},
	{0x90FA, 0xA0,8},
	{0x90FB, 0x3E,8},
	{0x90FC, 0x80,8},
	{0x90FD, 0xFA,8},
	{0x90FE, 0x01,8},
	{0x90FF, 0xEE,8},
	{0x9100, 0x03,8},
	{0x9101, 0xB0,8},
	{0x9102, 0x1F,8},
	{0x9103, 0xC0,8},
	{0x9104, 0x3E,8},
	{0x9105, 0x00,8},
	{0x9106, 0xFA,8},
	{0x9107, 0x00,8},
	{0x9108, 0xE8,8},
	{0x9109, 0x0B,8},
	{0x910A, 0xB8,8},
	{0x910B, 0x2F,8},
	{0x910C, 0xE0,8},
	{0x910D, 0xBE,8},
	{0x910E, 0x80,8},
	{0x910F, 0xFA,8},
	{0x9110, 0x00,8},
	{0x9111, 0xEA,8},
	{0x9112, 0x07,8},
	{0x9113, 0xB8,8},
	{0x9114, 0x0F,8},
	{0x9115, 0xC0,8},
	{0x9116, 0x7E,8},
	{0x9117, 0x00,8},
	{0x9118, 0xFB,8},
	{0x9119, 0x00,8},
	{0x911A, 0xE8,8},
	{0x911B, 0x03,8},
	{0x911C, 0xA0,8},
	{0x911D, 0x3F,8},
	{0x911E, 0xE0,8},
	{0x911F, 0xFE,8},
	{0x9120, 0x80,8},
	{0x9121, 0xFB,8},
	{0x9122, 0x03,8},
	{0x9123, 0xEA,8},
	{0x9124, 0x03,8},
	{0x9125, 0xA8,8},
	{0x9126, 0x1F,8},
	{0x9127, 0x00,8},
	{0x9128, 0xBF,8},
	{0x9129, 0xFF,8},
	{0x912A, 0xFB,8},
	{0x912B, 0xFF,8},
	{0x912C, 0xED,8},
	{0x912D, 0x03,8},
	{0x912E, 0xB0,8},
	{0x912F, 0x0F,8},
	{0x9130, 0xC0,8},
	{0x9131, 0xFE,8},
	{0x9132, 0x80,8},
	{0x9133, 0xFA,8},
	{0x9134, 0x03,8},
	{0x9135, 0xEA,8},
	{0x9136, 0x03,8},
	{0x9137, 0xC0,8},
	{0x9138, 0xFF,8},
	{0x9139, 0x1F,8},
	{0x913A, 0xFF,8},
	{0x913B, 0x7F,8},
	{0x913C, 0xFD,8},
	{0x913D, 0xFF,8},
	{0x913E, 0xEF,8},
	{0x913F, 0xFF,8},
	{0x9140, 0xCF,8},
	{0x9141, 0xEF,8},
	{0x9142, 0x7F,8},
	{0x9143, 0xBF,8},
	{0x9144, 0xFF,8},
	{0x9145, 0xFD,8},
	{0x9146, 0x02,8},
	{0x9147, 0xF8,8},
	{0x9148, 0x0B,8},
	{0x9149, 0xE0,8},
	{0x914A, 0x0F,8},
	{0x914B, 0x20,8},
	{0x914C, 0x3F,8},
	{0x914D, 0x00,8},
	{0x914E, 0xFC,8},
	{0x914F, 0xFF,8},
	{0x9150, 0xEB,8},
	{0x9151, 0xFB,8},
	{0x9152, 0xE7,8},
	{0x9153, 0xEF,8},
	{0x9154, 0x1F,8},
	{0x9155, 0x3F,8},
	{0x9156, 0x80,8},
	{0x9157, 0xFC,8},
	{0x9158, 0x00,8},
	{0x9159, 0xF2,8},
	{0x915A, 0x0F,8},
	{0x915B, 0xD0,8},
	{0x915C, 0x3F,8},
	{0x915D, 0x40,8},
	{0x915E, 0x3F,8},
	{0x915F, 0x80,8},
	{0x9160, 0xFD,8},
	{0x9161, 0xFF,8},
	{0x9162, 0xF9,8},
	{0x9163, 0xFF,8},
	{0x9164, 0xDF,8},
	{0x9165, 0xFF,8},
	{0x9166, 0x9F,8},
	{0x9167, 0xFF,8},
	{0x9168, 0xFF,8},
	{0x9169, 0xFD,8},
	{0x916A, 0x00,8},
	{0x916B, 0xFC,8},
	{0x916C, 0x03,8},
	{0x916D, 0xF0,8},
	{0x916E, 0x5F,8},
	{0x916F, 0xA0,8},
	{0x9170, 0x7F,8},
	{0x9171, 0x81,8},
	{0x9172, 0xFE,8},
	{0x9173, 0x02,8},
	{0x9174, 0xFC,8},
	{0x9175, 0x07,8},
	{0x9176, 0xF8,8},
	{0x9177, 0x2F,8},
	{0x9178, 0x20,8},
	{0x9179, 0xC0,8},
	{0x917A, 0x7F,8},
	{0x917B, 0x00,8},
	{0x917C, 0x00,8},
	{0x917D, 0xFA,8},
	{0x917E, 0x0B,8},
	{0x917F, 0x00,8},
	{0x9180, 0x20,8},
	{0x9181, 0x00,8},
	{0x9182, 0xC0,8},
	{0x9183, 0x01,8},
	{0x9184, 0x00,8},
	{0x9185, 0x07,8},
	{0x9186, 0x00,8},
	{0x9187, 0x14,8},
	{0x9188, 0x08,8},
	{0x9189, 0x20,8},
	{0x918A, 0x40,8},
	{0x918B, 0x80,8},
	{0x918C, 0x00,8},
	{0x918D, 0x02,8},
	{0x918E, 0x02,8},
	{0x918F, 0x02,8},
	{0x9190, 0x04,8},
	{0x9191, 0x28,8},
	{0x9192, 0x10,8},
	{0x9193, 0x60,8},
	{0x9194, 0x40,8},
	{0x9195, 0x80,8},
	{0x9196, 0x01,8},
	{0x9197, 0x07,8},
	{0x9198, 0x06,8},
	{0x9199, 0x1C,8},
	{0x919A, 0x18,8},
	{0x919B, 0x40,8},
	{0x919C, 0x80,8},
	{0x919D, 0x00,8},
	{0x919E, 0x81,8},
	{0x919F, 0x02,8},
	{0x91A0, 0x02,8},
	{0x91A1, 0x0C,8},
	{0x91A2, 0x08,8},
	{0x91A3, 0x20,8},
	{0x91A4, 0x30,8},
	{0x91A5, 0x80,8},
	{0x91A6, 0x00,8},
	{0x91A7, 0x01,8},
	{0x91A8, 0x02,8},
	{0x91A9, 0x04,8},
	{0x91AA, 0x08,8},
	{0x91AB, 0x1C,8},
	{0x91AC, 0x10,8},
	{0x91AD, 0x70,8},
	{0x91AE, 0x40,8},
	{0x91AF, 0x00,8},
	{0x91B0, 0x02,8},
	{0x91B1, 0x03,8},
	{0x91B2, 0x06,8},
	{0x91B3, 0x10,8},
	{0x91B4, 0x0C,8},
	{0x91B5, 0x28,8},
	{0x91B6, 0x40,8},
	{0x91B7, 0x00,8},
	{0x91B8, 0xC1,8},
	{0x91B9, 0x80,8},
	{0x91BA, 0x03,8},
	{0x91BB, 0x03,8},
	{0x91BC, 0x04,8},
	{0x91BD, 0x0C,8},
	{0x91BE, 0x10,8},
	{0x91BF, 0x60,8},
	{0x91C0, 0xA0,8},
	{0x91C1, 0x80,8},
	{0x91C2, 0x81,8},
	{0x91C3, 0x02,8},
	{0x91C4, 0x06,8},
	{0x91C5, 0x10,8},
	{0x91C6, 0x14,8},
	{0x91C7, 0x48,8},
	{0x91C8, 0x50,8},
	{0x91C9, 0xE0,8},
	{0x91CA, 0x40,8},
	{0x91CB, 0x01,8},
	{0x91CC, 0x05,8},
	{0x91CD, 0x05,8},
	{0x91CE, 0x0A,8},
	{0x91CF, 0x10,8},
	{0x91D0, 0x38,8},
	{0x91D1, 0x40,8},
	{0x91D2, 0xE0,8},
	{0x91D3, 0x80,8},
	{0x91D4, 0x81,8},
	{0x91D5, 0x02,8},
	{0x91D6, 0x06,8},
	{0x91D7, 0x0A,8},
	{0x91D8, 0x18,8},
	{0x91D9, 0x40,8},
	{0x91DA, 0x50,8},
	{0x91DB, 0x20,8},
	{0x91DC, 0x41,8},
	{0x91DD, 0x81,8},
	{0x91DE, 0x03,8},
	{0x91DF, 0x05,8},
	{0x91E0, 0x14,8},
	{0x91E1, 0x14,8},
	{0x91E2, 0x28,8},
	{0x91E3, 0x40,8},
	{0x91E4, 0xE0,8},
	{0x91E5, 0x00,8},
	{0x91E6, 0x81,8},
	{0x91E7, 0x03,8},

	/////////////////////////////////////
	////CAT.:SHD_DATA////
	/////////////////////////////////////
	{0xA000, 0x9191,16},
	{0xA002, 0x0A,8},
	{0xA003, 0x82,8},
	{0xA004, 0x13,8},
	{0xA005, 0x85,8},
	{0xA006, 0xFA,8},
	{0xA007, 0xB4,8},
	{0xA008, 0x1F,8},
	{0xA009, 0x32,8},
	{0xA00A, 0x85,8},
	{0xA00B, 0xE7,8},
	{0xA00C, 0x48,8},
	{0xA00D, 0xDE,8},
	{0xA00E, 0x21,8},
	{0xA00F, 0x12,8},
	{0xA010, 0x78,8},
	{0xA011, 0x82,8},
	{0xA012, 0x94,8},
	{0xA013, 0x9F,8},
	{0xA014, 0x33,8},
	{0xA015, 0x41,8},
	{0xA016, 0xC8,8},
	{0xA017, 0x50,8},
	{0xA018, 0x04,8},
	{0xA019, 0xCA,8},
	{0xA01A, 0x53,8},
	{0xA01B, 0x84,8},
	{0xA01C, 0xEC,8},
	{0xA01D, 0x24,8},
	{0xA01E, 0xA0,8},
	{0xA01F, 0x32,8},
	{0xA020, 0x79,8},
	{0xA021, 0x67,8},
	{0xA022, 0x47,8},
	{0xA023, 0xC9,8},
	{0xA024, 0x11,8},
	{0xA025, 0x11,8},
	{0xA026, 0x71,8},
	{0xA027, 0x36,8},
	{0xA028, 0x74,8},
	{0xA029, 0x9C,8},
	{0xA02A, 0x0E,8},
	{0xA02B, 0x6D,8},
	{0xA02C, 0xC7,8},
	{0xA02D, 0x47,8},
	{0xA02E, 0xFD,8},
	{0xA02F, 0x69,8},
	{0xA030, 0x13,8},
	{0xA031, 0x82,8},
	{0xA032, 0x00,8},
	{0xA033, 0x25,8},
	{0xA034, 0x1E,8},
	{0xA035, 0x1B,8},
	{0xA036, 0x25,8},
	{0xA037, 0x67,8},
	{0xA038, 0x42,8},
	{0xA039, 0xA6,8},
	{0xA03A, 0x31,8},
	{0xA03B, 0x0F,8},
	{0xA03C, 0x64,8},
	{0xA03D, 0x90,8},
	{0xA03E, 0x43,8},
	{0xA03F, 0x18,8},
	{0xA040, 0xDC,8},
	{0xA041, 0x38,8},
	{0xA042, 0xC6,8},
	{0xA043, 0x38,8},
	{0xA044, 0xA3,8},
	{0xA045, 0x39,8},
	{0xA046, 0x0F,8},
	{0xA047, 0x71,8},
	{0xA048, 0x38,8},
	{0xA049, 0x74,8},
	{0xA04A, 0x9D,8},
	{0xA04B, 0x1F,8},
	{0xA04C, 0xDD,8},
	{0xA04D, 0x86,8},
	{0xA04E, 0x3F,8},
	{0xA04F, 0x9D,8},
	{0xA050, 0xB1,8},
	{0xA051, 0x0E,8},
	{0xA052, 0x5D,8},
	{0xA053, 0x34,8},
	{0xA054, 0x53,8},
	{0xA055, 0x95,8},
	{0xA056, 0xB7,8},
	{0xA057, 0x2C,8},
	{0xA058, 0x45,8},
	{0xA059, 0x2C,8},
	{0xA05A, 0x53,8},
	{0xA05B, 0x79,8},
	{0xA05C, 0x4B,8},
	{0xA05D, 0x5C,8},
	{0xA05E, 0x32,8},
	{0xA05F, 0x73,8},
	{0xA060, 0x19,8},
	{0xA061, 0xEB,8},
	{0xA062, 0xA8,8},
	{0xA063, 0x26,8},
	{0xA064, 0x3F,8},
	{0xA065, 0x97,8},
	{0xA066, 0x49,8},
	{0xA067, 0x0E,8},
	{0xA068, 0x5E,8},
	{0xA069, 0x3A,8},
	{0xA06A, 0x73,8},
	{0xA06B, 0x94,8},
	{0xA06C, 0xAD,8},
	{0xA06D, 0xA0,8},
	{0xA06E, 0x44,8},
	{0xA06F, 0x26,8},
	{0xA070, 0x1A,8},
	{0xA071, 0x01,8},
	{0xA072, 0x89,8},
	{0xA073, 0x49,8},
	{0xA074, 0x60,8},
	{0xA075, 0x42,8},
	{0xA076, 0x14,8},
	{0xA077, 0xAD,8},
	{0xA078, 0xC8,8},
	{0xA079, 0x85,8},
	{0xA07A, 0x33,8},
	{0xA07B, 0x8D,8},
	{0xA07C, 0x21,8},
	{0xA07D, 0xCE,8},
	{0xA07E, 0x60,8},
	{0xA07F, 0x54,8},
	{0xA080, 0xF3,8},
	{0xA081, 0x95,8},
	{0xA082, 0xBC,8},
	{0xA083, 0xB0,8},
	{0xA084, 0xA4,8},
	{0xA085, 0x26,8},
	{0xA086, 0x0E,8},
	{0xA087, 0x79,8},
	{0xA088, 0x88,8},
	{0xA089, 0x40,8},
	{0xA08A, 0x04,8},
	{0xA08B, 0xC2,8},
	{0xA08C, 0x10,8},
	{0xA08D, 0x87,8},
	{0xA08E, 0xA4,8},
	{0xA08F, 0xA4,8},
	{0xA090, 0x26,8},
	{0xA091, 0x5B,8},
	{0xA092, 0xD1,8},
	{0xA093, 0x8B,8},
	{0xA094, 0x5E,8},
	{0xA095, 0x50,8},
	{0xA096, 0xC3,8},
	{0xA097, 0x97,8},
	{0xA098, 0xD3,8},
	{0xA099, 0x64,8},
	{0xA09A, 0x65,8},
	{0xA09B, 0x2E,8},
	{0xA09C, 0x25,8},
	{0xA09D, 0x71,8},
	{0xA09E, 0x09,8},
	{0xA09F, 0x42,8},
	{0xA0A0, 0x12,8},
	{0xA0A1, 0xD2,8},
	{0xA0A2, 0x8F,8},
	{0xA0A3, 0x7E,8},
	{0xA0A4, 0x18,8},
	{0xA0A5, 0x04,8},
	{0xA0A6, 0x21,8},
	{0xA0A7, 0x23,8},
	{0xA0A8, 0x71,8},
	{0xA0A9, 0x09,8},
	{0xA0AA, 0x55,8},
	{0xA0AB, 0xE4,8},
	{0xA0AC, 0x32,8},
	{0xA0AD, 0x97,8},
	{0xA0AE, 0xD0,8},
	{0xA0AF, 0x24,8},
	{0xA0B0, 0x06,8},
	{0xA0B1, 0x37,8},
	{0xA0B2, 0x67,8},
	{0xA0B3, 0x39,8},
	{0xA0B4, 0x4C,8},
	{0xA0B5, 0x4D,8},
	{0xA0B6, 0x82,8},
	{0xA0B7, 0x72,8},
	{0xA0B8, 0x91,8},
	{0xA0B9, 0x8D,8},
	{0xA0BA, 0x2C,8},
	{0xA0BB, 0xA4,8},
	{0xA0BC, 0x21,8},
	{0xA0BD, 0x16,8},
	{0xA0BE, 0xC9,8},
	{0xA0BF, 0x88,8},
	{0xA0C0, 0x4C,8},
	{0xA0C1, 0x84,8},
	{0xA0C2, 0x12,8},
	{0xA0C3, 0x96,8},
	{0xA0C4, 0xC3,8},
	{0xA0C5, 0xF8,8},
	{0xA0C6, 0x65,8},
	{0xA0C7, 0x36,8},
	{0xA0C8, 0xA2,8},
	{0xA0C9, 0x01,8},
	{0xA0CA, 0x8F,8},
	{0xA0CB, 0x61,8},
	{0xA0CC, 0x6C,8},
	{0xA0CD, 0x93,8},
	{0xA0CE, 0x95,8},
	{0xA0CF, 0xB9,8},
	{0xA0D0, 0xE0,8},
	{0xA0D1, 0xC4,8},
	{0xA0D2, 0x28,8},
	{0xA0D3, 0x2E,8},
	{0xA0D4, 0xC9,8},
	{0xA0D5, 0xC9,8},
	{0xA0D6, 0x4D,8},
	{0xA0D7, 0x8A,8},
	{0xA0D8, 0x62,8},
	{0xA0D9, 0x95,8},
	{0xA0DA, 0xB9,8},
	{0xA0DB, 0x04,8},
	{0xA0DC, 0xC6,8},
	{0xA0DD, 0x36,8},
	{0xA0DE, 0x98,8},
	{0xA0DF, 0xD9,8},
	{0xA0E0, 0x8E,8},
	{0xA0E1, 0x6F,8},
	{0xA0E2, 0x18,8},
	{0xA0E3, 0x64,8},
	{0xA0E4, 0x9A,8},
	{0xA0E5, 0xF4,8},
	{0xA0E6, 0x0C,8},
	{0xA0E7, 0xC6,8},
	{0xA0E8, 0x36,8},
	{0xA0E9, 0x6B,8},
	{0xA0EA, 0x89,8},
	{0xA0EB, 0xCC,8},
	{0xA0EC, 0x57,8},
	{0xA0ED, 0xFE,8},
	{0xA0EE, 0xB2,8},
	{0xA0EF, 0x16,8},
	{0xA0F0, 0xC8,8},
	{0xA0F1, 0x08,8},
	{0xA0F2, 0x26,8},
	{0xA0F3, 0x37,8},
	{0xA0F4, 0xA5,8},
	{0xA0F5, 0x71,8},
	{0xA0F6, 0xCF,8},
	{0xA0F7, 0x6E,8},
	{0xA0F8, 0x1C,8},
	{0xA0F9, 0xC4,8},
	{0xA0FA, 0x1E,8},
	{0xA0FB, 0x23,8},
	{0xA0FC, 0x51,8},
	{0xA0FD, 0x27,8},
	{0xA0FE, 0x45,8},
	{0xA0FF, 0xB4,8},
	{0xA100, 0x01,8},
	{0xA101, 0x50,8},
	{0xA102, 0x68,8},
	{0xA103, 0xCA,8},
	{0xA104, 0xB3,8},
	{0xA105, 0x19,8},
	{0xA106, 0xED,8},
	{0xA107, 0x80,8},
	{0xA108, 0x46,8},
	{0xA109, 0x3C,8},
	{0xA10A, 0xB5,8},
	{0xA10B, 0x21,8},
	{0xA10C, 0xD0,8},
	{0xA10D, 0x75,8},
	{0xA10E, 0x66,8},
	{0xA10F, 0xC4,8},
	{0xA110, 0x9E,8},
	{0xA111, 0x26,8},
	{0xA112, 0xCD,8},
	{0xA113, 0x67,8},
	{0xA114, 0x4A,8},
	{0xA115, 0xEC,8},
	{0xA116, 0x51,8},
	{0xA117, 0x52,8},
	{0xA118, 0x73,8},
	{0xA119, 0x4C,8},
	{0xA11A, 0x54,8},
	{0xA11B, 0x9B,8},
	{0xA11C, 0x03,8},
	{0xA11D, 0xB9,8},
	{0xA11E, 0x86,8},
	{0xA11F, 0x3F,8},
	{0xA120, 0xB5,8},
	{0xA121, 0x29,8},
	{0xA122, 0x10,8},
	{0xA123, 0x74,8},
	{0xA124, 0x5A,8},
	{0xA125, 0x14,8},
	{0xA126, 0x1F,8},
	{0xA127, 0x2B,8},
	{0xA128, 0xD9,8},
	{0xA129, 0x87,8},
	{0xA12A, 0x4B,8},
	{0xA12B, 0xFC,8},
	{0xA12C, 0x69,8},
	{0xA12D, 0xD0,8},
	{0xA12E, 0x80,8},
	{0xA12F, 0x32,8},
	{0xA130, 0xF4,8},
	{0xA131, 0x1E,8},
	{0xA132, 0x01,8},
	{0xA133, 0x65,8},
	{0xA134, 0x27,8},
	{0xA135, 0x3D,8},
	{0xA136, 0xDA,8},
	{0xA137, 0x29,8},
	{0xA138, 0xCF,8},
	{0xA139, 0x77,8},
	{0xA13A, 0xD2,8},
	{0xA13B, 0xA3,8},
	{0xA13C, 0x9F,8},
	{0xA13D, 0x00,8},
	{0xA13E, 0x45,8},
	{0xA13F, 0xA8,8},
	{0xA140, 0x42,8},
	{0xA141, 0x08,8},
	{0xA142, 0x3A,8},
	{0xA143, 0x10,8},
	{0xA144, 0x7F,8},
	{0xA145, 0x28,8},
	{0xA146, 0x04,8},
	{0xA147, 0x9F,8},
	{0xA148, 0x02,8},
	{0xA149, 0x49,8},
	{0xA14A, 0x67,8},
	{0xA14B, 0x3C,8},
	{0xA14C, 0xC0,8},
	{0xA14D, 0x71,8},
	{0xA14E, 0xCE,8},
	{0xA14F, 0x6F,8},
	{0xA150, 0x94,8},
	{0xA151, 0x43,8},
	{0xA152, 0x1C,8},
	{0xA153, 0xE7,8},
	{0xA154, 0x68,8},
	{0xA155, 0x67,8},
	{0xA156, 0x3C,8},
	{0xA157, 0xFB,8},
	{0xA158, 0x11,8},
	{0xA159, 0x90,8},
	{0xA15A, 0x81,8},
	{0xA15B, 0x16,8},
	{0xA15C, 0xC4,8},
	{0xA15D, 0x9C,8},
	{0xA15E, 0xF0,8},
	{0xA15F, 0xE0,8},
	{0xA160, 0x26,8},
	{0xA161, 0x39,8},
	{0xA162, 0x9A,8},
	{0xA163, 0x41,8},
	{0xA164, 0x0D,8},
	{0xA165, 0x62,8},
	{0xA166, 0x26,8},
	{0xA167, 0x13,8},
	{0xA168, 0x98,8},
	{0xA169, 0xC4,8},
	{0xA16A, 0x28,8},
	{0xA16B, 0x66,8},
	{0xA16C, 0x32,8},
	{0xA16D, 0x9F,8},
	{0xA16E, 0x39,8},
	{0xA16F, 0xCD,8},
	{0xA170, 0x6F,8},
	{0xA171, 0x8E,8},
	{0xA172, 0x23,8},
	{0xA173, 0x1D,8},
	{0xA174, 0xED,8},
	{0xA175, 0x88,8},
	{0xA176, 0x66,8},
	{0xA177, 0x36,8},
	{0xA178, 0x8C,8},
	{0xA179, 0xE1,8},
	{0xA17A, 0x4C,8},
	{0xA17B, 0x5A,8},
	{0xA17C, 0xEC,8},
	{0xA17D, 0xE2,8},
	{0xA17E, 0x14,8},
	{0xA17F, 0xAB,8},
	{0xA180, 0x24,8},
	{0xA181, 0xC5,8},
	{0xA182, 0x29,8},
	{0xA183, 0x51,8},
	{0xA184, 0xA9,8},
	{0xA185, 0x8A,8},
	{0xA186, 0x5B,8},
	{0xA187, 0xE8,8},
	{0xA188, 0x12,8},
	{0xA189, 0x19,8},
	{0xA18A, 0xCD,8},
	{0xA18B, 0x88,8},
	{0xA18C, 0xA6,8},
	{0xA18D, 0x35,8},
	{0xA18E, 0x80,8},
	{0xA18F, 0x99,8},
	{0xA190, 0xCC,8},
	{0xA191, 0x59,8},
	{0xA192, 0xEC,8},
	{0xA193, 0xD2,8},
	{0xA194, 0x93,8},
	{0xA195, 0xA3,8},
	{0xA196, 0x90,8},
	{0xA197, 0x24,8},
	{0xA198, 0x25,8},
	{0xA199, 0x1A,8},
	{0xA19A, 0xD9,8},
	{0xA19B, 0x48,8},
	{0xA19C, 0x49,8},
	{0xA19D, 0x4E,8},
	{0xA19E, 0x22,8},
	{0xA19F, 0x14,8},
	{0xA1A0, 0xA2,8},
	{0xA1A1, 0xB0,8},
	{0xA1A2, 0x25,8},
	{0xA1A3, 0x2E,8},
	{0xA1A4, 0x83,8},
	{0xA1A5, 0x51,8},
	{0xA1A6, 0x4C,8},
	{0xA1A7, 0x5B,8},
	{0xA1A8, 0xFA,8},
	{0xA1A9, 0x12,8},
	{0xA1AA, 0x15,8},
	{0xA1AB, 0xAE,8},
	{0xA1AC, 0x90,8},
	{0xA1AD, 0x84,8},
	{0xA1AE, 0x25,8},
	{0xA1AF, 0x0A,8},
	{0xA1B0, 0x71,8},
	{0xA1B1, 0x88,8},
	{0xA1B2, 0x40,8},
	{0xA1B3, 0x04,8},
	{0xA1B4, 0xC2,8},
	{0xA1B5, 0x10,8},
	{0xA1B6, 0x86,8},
	{0xA1B7, 0x98,8},
	{0xA1B8, 0xE4,8},
	{0xA1B9, 0x24,8},
	{0xA1BA, 0x54,8},
	{0xA1BB, 0xC1,8},
	{0xA1BC, 0x8A,8},
	{0xA1BD, 0x5B,8},
	{0xA1BE, 0xEC,8},
	{0xA1BF, 0xB2,8},
	{0xA1C0, 0x16,8},
	{0xA1C1, 0xBB,8},
	{0xA1C2, 0x38,8},
	{0xA1C3, 0xA5,8},
	{0xA1C4, 0x2A,8},
	{0xA1C5, 0x20,8},
	{0xA1C6, 0x29,8},
	{0xA1C7, 0x49,8},
	{0xA1C8, 0x41,8},
	{0xA1C9, 0x10,8},
	{0xA1CA, 0xD2,8},
	{0xA1CB, 0x8F,8},
	{0xA1CC, 0x7E,8},
	{0xA1CD, 0x18,8},
	{0xA1CE, 0xC4,8},
	{0xA1CF, 0x20,8},
	{0xA1D0, 0x20,8},
	{0xA1D1, 0x01,8},
	{0xA1D2, 0x89,8},
	{0xA1D3, 0x53,8},
	{0xA1D4, 0xA2,8},
	{0xA1D5, 0x82,8},
	{0xA1D6, 0x16,8},
	{0xA1D7, 0xB8,8},
	{0xA1D8, 0xDC,8},
	{0xA1D9, 0x05,8},
	{0xA1DA, 0x30,8},
	{0xA1DB, 0x5C,8},
	{0xA1DC, 0x09,8},
	{0xA1DD, 0xCB,8},
	{0xA1DE, 0x4B,8},
	{0xA1DF, 0x64,8},
	{0xA1E0, 0x22,8},
	{0xA1E1, 0x91,8},
	{0xA1E2, 0x8A,8},
	{0xA1E3, 0x20,8},
	{0xA1E4, 0x24,8},
	{0xA1E5, 0x21,8},
	{0xA1E6, 0x13,8},
	{0xA1E7, 0x99,8},
	{0xA1E8, 0x48,8},
	{0xA1E9, 0x4B,8},
	{0xA1EA, 0x5E,8},
	{0xA1EB, 0x72,8},
	{0xA1EC, 0x95,8},
	{0xA1ED, 0xAF,8},
	{0xA1EE, 0xB8,8},
	{0xA1EF, 0x65,8},
	{0xA1F0, 0x2F,8},
	{0xA1F1, 0x8F,8},
	{0xA1F2, 0xB9,8},
	{0xA1F3, 0xCC,8},
	{0xA1F4, 0x5D,8},
	{0xA1F5, 0xFA,8},
	{0xA1F6, 0xE2,8},
	{0xA1F7, 0x94,8},
	{0xA1F8, 0xA9,8},
	{0xA1F9, 0xC0,8},
	{0xA1FA, 0x84,8},
	{0xA1FB, 0x26,8},
	{0xA1FC, 0x27,8},
	{0xA1FD, 0x51,8},
	{0xA1FE, 0xC9,8},
	{0xA1FF, 0x4B,8},
	{0xA200, 0x64,8},
	{0xA201, 0xB2,8},
	{0xA202, 0x14,8},
	{0xA203, 0xA9,8},
	{0xA204, 0xC0,8},
	{0xA205, 0x85,8},
	{0xA206, 0x2F,8},
	{0xA207, 0x83,8},
	{0xA208, 0x91,8},
	{0xA209, 0x4C,8},
	{0xA20A, 0x6B,8},
	{0xA20B, 0x68,8},
	{0xA20C, 0x93,8},
	{0xA20D, 0x99,8},
	{0xA20E, 0xCF,8},
	{0xA20F, 0xE4,8},
	{0xA210, 0xE5,8},
	{0xA211, 0x2F,8},
	{0xA212, 0x62,8},
	{0xA213, 0x39,8},
	{0xA214, 0x4B,8},
	{0xA215, 0x55,8},
	{0xA216, 0xB6,8},
	{0xA217, 0xF2,8},
	{0xA218, 0x15,8},
	{0xA219, 0xB4,8},
	{0xA21A, 0xC4,8},
	{0xA21B, 0xE5,8},
	{0xA21C, 0x2F,8},
	{0xA21D, 0x8F,8},
	{0xA21E, 0x01,8},
	{0xA21F, 0x0D,8},
	{0xA220, 0x68,8},
	{0xA221, 0x66,8},
	{0xA222, 0xB3,8},
	{0xA223, 0x9D,8},
	{0xA224, 0xF3,8},
	{0xA225, 0x1C,8},
	{0xA226, 0x47,8},
	{0xA227, 0x3A,8},
	{0xA228, 0xA8,8},
	{0xA229, 0x89,8},
	{0xA22A, 0x0D,8},
	{0xA22B, 0x65,8},
	{0xA22C, 0x3E,8},
	{0xA22D, 0xE3,8},
	{0xA22E, 0x18,8},
	{0xA22F, 0xCD,8},
	{0xA230, 0x44,8},
	{0xA231, 0x06,8},
	{0xA232, 0x34,8},
	{0xA233, 0xA1,8},
	{0xA234, 0x99,8},
	{0xA235, 0xCD,8},
	{0xA236, 0x6F,8},
	{0xA237, 0xAC,8},
	{0xA238, 0x03,8},
	{0xA239, 0x9D,8},
	{0xA23A, 0xF4,8},
	{0xA23B, 0x94,8},
	{0xA23C, 0x47,8},
	{0xA23D, 0x3E,8},
	{0xA23E, 0xDF,8},
	{0xA23F, 0x59,8},
	{0xA240, 0xCF,8},
	{0xA241, 0x6F,8},
	{0xA242, 0x98,8},
	{0xA243, 0x73,8},
	{0xA244, 0x9A,8},
	{0xA245, 0xDA,8},
	{0xA246, 0x84,8},
	{0xA247, 0xE6,8},
	{0xA248, 0x35,8},
	{0xA249, 0xA5,8},
	{0xA24A, 0xB1,8},
	{0xA24B, 0x0D,8},
	{0xA24C, 0x6F,8},
	{0xA24D, 0xA0,8},
	{0xA24E, 0x93,8},
	{0xA24F, 0x1D,8},
	{0xA250, 0xF9,8},
	{0xA251, 0x70,8},
	{0xA252, 0xA7,8},
	{0xA253, 0x3E,8},

	{0xA254, 0x36,8},	//SHD2
	{0xA255, 0x3A,8},
	{0xA256, 0x52,8},
	{0xA257, 0x8C,8},
	{0xA258, 0x2E,8},
	{0xA259, 0x54,8},
	{0xA25A, 0x21,8},
	{0xA25B, 0x0B,8},
	{0xA25C, 0xD1,8},
	{0xA25D, 0xC8,8},
	{0xA25E, 0x49,8},
	{0xA25F, 0x40,8},
	{0xA260, 0x0A,8},
	{0xA261, 0x12,8},
	{0xA262, 0x8D,8},
	{0xA263, 0x1C,8},
	{0xA264, 0x74,8},
	{0xA265, 0x9F,8},
	{0xA266, 0xF8,8},
	{0xA267, 0xD4,8},
	{0xA268, 0x27,8},
	{0xA269, 0x42,8},
	{0xA26A, 0x3B,8},
	{0xA26B, 0x4A,8},
	{0xA26C, 0x12,8},
	{0xA26D, 0x83,8},
	{0xA26E, 0xDA,8},
	{0xA26F, 0x23,8},
	{0xA270, 0x1C,8},
	{0xA271, 0xD3,8},
	{0xA272, 0x64,8},
	{0xA273, 0xC6,8},
	{0xA274, 0x34,8},
	{0xA275, 0xC3,8},
	{0xA276, 0xA1,8},
	{0xA277, 0xCF,8},
	{0xA278, 0x84,8},
	{0xA279, 0xAE,8},
	{0xA27A, 0x53,8},
	{0xA27B, 0x9B,8},
	{0xA27C, 0xBF,8},
	{0xA27D, 0x68,8},
	{0xA27E, 0x05,8},
	{0xA27F, 0x2A,8},
	{0xA280, 0x5A,8},
	{0xA281, 0xF9,8},
	{0xA282, 0x4B,8},
	{0xA283, 0x6D,8},
	{0xA284, 0xAA,8},
	{0xA285, 0xB3,8},
	{0xA286, 0x9A,8},
	{0xA287, 0xC1,8},
	{0xA288, 0x28,8},
	{0xA289, 0x05,8},
	{0xA28A, 0x25,8},
	{0xA28B, 0x1A,8},
	{0xA28C, 0x39,8},
	{0xA28D, 0x89,8},
	{0xA28E, 0x52,8},
	{0xA28F, 0x06,8},
	{0xA290, 0x73,8},
	{0xA291, 0x9A,8},
	{0xA292, 0xC8,8},
	{0xA293, 0x94,8},
	{0xA294, 0x85,8},
	{0xA295, 0x25,8},
	{0xA296, 0x0D,8},
	{0xA297, 0x09,8},
	{0xA298, 0xC8,8},
	{0xA299, 0x42,8},
	{0xA29A, 0x56,8},
	{0xA29B, 0x62,8},
	{0xA29C, 0x16,8},
	{0xA29D, 0xC7,8},
	{0xA29E, 0x34,8},
	{0xA29F, 0x26,8},
	{0xA2A0, 0x2C,8},
	{0xA2A1, 0x26,8},
	{0xA2A2, 0x41,8},
	{0xA2A3, 0x88,8},
	{0xA2A4, 0x3F,8},
	{0xA2A5, 0x0E,8},
	{0xA2A6, 0x62,8},
	{0xA2A7, 0x12,8},
	{0xA2A8, 0xB0,8},
	{0xA2A9, 0x24,8},
	{0xA2AA, 0xA6,8},
	{0xA2AB, 0x33,8},
	{0xA2AC, 0x72,8},
	{0xA2AD, 0xB1,8},
	{0xA2AE, 0x89,8},
	{0xA2AF, 0x45,8},
	{0xA2B0, 0x16,8},
	{0xA2B1, 0x52,8},
	{0xA2B2, 0x91,8},
	{0xA2B3, 0x9B,8},
	{0xA2B4, 0xC8,8},
	{0xA2B5, 0x25,8},
	{0xA2B6, 0x33,8},
	{0xA2B7, 0xC1,8},
	{0xA2B8, 0xD9,8},
	{0xA2B9, 0x0C,8},
	{0xA2BA, 0x58,8},
	{0xA2BB, 0x74,8},
	{0xA2BC, 0x02,8},
	{0xA2BD, 0x13,8},
	{0xA2BE, 0x9D,8},
	{0xA2BF, 0x84,8},
	{0xA2C0, 0x85,8},
	{0xA2C1, 0x33,8},
	{0xA2C2, 0xBE,8},
	{0xA2C3, 0x59,8},
	{0xA2C4, 0xCF,8},
	{0xA2C5, 0x72,8},
	{0xA2C6, 0x36,8},
	{0xA2C7, 0xB3,8},
	{0xA2C8, 0x17,8},
	{0xA2C9, 0xB6,8},
	{0xA2CA, 0xEC,8},
	{0xA2CB, 0xE5,8},
	{0xA2CC, 0x33,8},
	{0xA2CD, 0xD1,8},
	{0xA2CE, 0x81,8},
	{0xA2CF, 0xCF,8},
	{0xA2D0, 0x88,8},
	{0xA2D1, 0x12,8},
	{0xA2D2, 0x14,8},
	{0xA2D3, 0x9E,8},
	{0xA2D4, 0xE3,8},
	{0xA2D5, 0xF8,8},
	{0xA2D6, 0xE6,8},
	{0xA2D7, 0x38,8},
	{0xA2D8, 0xE6,8},
	{0xA2D9, 0xA1,8},
	{0xA2DA, 0x50,8},
	{0xA2DB, 0x8B,8},
	{0xA2DC, 0x56,8},
	{0xA2DD, 0x74,8},
	{0xA2DE, 0xA2,8},
	{0xA2DF, 0x02,8},
	{0xA2E0, 0xA1,8},
	{0xA2E1, 0x07,8},
	{0xA2E2, 0x3C,8},
	{0xA2E3, 0xE8,8},
	{0xA2E4, 0x69,8},
	{0xA2E5, 0x90,8},
	{0xA2E6, 0x8D,8},
	{0xA2E7, 0x6E,8},
	{0xA2E8, 0x04,8},
	{0xA2E9, 0x80,8},
	{0xA2EA, 0x00,8},
	{0xA2EB, 0x00,8},
	{0xA2EC, 0x00,8},
	{0xA2ED, 0x00,8},

	{0xA2EE, 0x95,8},	//SHD3
	{0xA2EF, 0x52,8},
	{0xA2F0, 0x55,8},
	{0xA2F1, 0xA5,8},
	{0xA2F2, 0xEE,8},
	{0xA2F3, 0x24,8},
	{0xA2F4, 0xA7,8},
	{0xA2F5, 0x39,8},
	{0xA2F6, 0x65,8},
	{0xA2F7, 0xEA,8},
	{0xA2F8, 0x56,8},
	{0xA2F9, 0x9E,8},
	{0xA2FA, 0x2A,8},
	{0xA2FB, 0x95,8},
	{0xA2FC, 0xA5,8},
	{0xA2FD, 0xD2,8},
	{0xA2FE, 0xC4,8},
	{0xA2FF, 0x24,8},
	{0xA300, 0x22,8},
	{0xA301, 0x25,8},
	{0xA302, 0xA9,8},
	{0xA303, 0x4D,8},
	{0xA304, 0xA0,8},
	{0xA305, 0x92,8},
	{0xA306, 0x15,8},
	{0xA307, 0x99,8},
	{0xA308, 0x7C,8},
	{0xA309, 0x74,8},
	{0xA30A, 0xA0,8},
	{0xA30B, 0xF1,8},
	{0xA30C, 0x48,8},
	{0xA30D, 0x67,8},
	{0xA30E, 0x3C,8},
	{0xA30F, 0x09,8},
	{0xA310, 0x32,8},
	{0xA311, 0xD2,8},
	{0xA312, 0x9B,8},
	{0xA313, 0x44,8},
	{0xA314, 0x64,8},
	{0xA315, 0x1F,8},
	{0xA316, 0xD7,8},
	{0xA317, 0xF0,8},
	{0xA318, 0xA5,8},
	{0xA319, 0x2D,8},
	{0xA31A, 0x7C,8},
	{0xA31B, 0x71,8},
	{0xA31C, 0xCD,8},
	{0xA31D, 0x7D,8},
	{0xA31E, 0x46,8},
	{0xA31F, 0xA4,8},
	{0xA320, 0x1E,8},
	{0xA321, 0xDA,8},
	{0xA322, 0x98,8},
	{0xA323, 0xC5,8},
	{0xA324, 0x26,8},
	{0xA325, 0x24,8},
	{0xA326, 0xA9,8},
	{0xA327, 0x49,8},
	{0xA328, 0x59,8},
	{0xA329, 0x68,8},
	{0xA32A, 0x93,8},
	{0xA32B, 0x1E,8},
	{0xA32C, 0xE4,8},
	{0xA32D, 0x30,8},
	{0xA32E, 0xA6,8},
	{0xA32F, 0x27,8},
	{0xA330, 0x11,8},
	{0xA331, 0x11,8},
	{0xA332, 0x08,8},
	{0xA333, 0x44,8},
	{0xA334, 0x78,8},
	{0xA335, 0xD2,8},
	{0xA336, 0x98,8},
	{0xA337, 0xE4,8},
	{0xA338, 0x08,8},
	{0xA339, 0xA7,8},
	{0xA33A, 0x30,8},
	{0xA33B, 0x34,8},
	{0xA33C, 0x59,8},
	{0xA33D, 0x48,8},
	{0xA33E, 0x3F,8},
	{0xA33F, 0x14,8},
	{0xA340, 0x62,8},
	{0xA341, 0x13,8},
	{0xA342, 0xC3,8},
	{0xA343, 0x08,8},
	{0xA344, 0xE7,8},
	{0xA345, 0x3A,8},
	{0xA346, 0x9D,8},
	{0xA347, 0x61,8},
	{0xA348, 0xCA,8},
	{0xA349, 0x47,8},
	{0xA34A, 0x20,8},
	{0xA34B, 0xF2,8},
	{0xA34C, 0x91,8},
	{0xA34D, 0xA6,8},
	{0xA34E, 0x7C,8},
	{0xA34F, 0x06,8},
	{0xA350, 0x3B,8},
	{0xA351, 0x05,8},
	{0xA352, 0x9A,8},
	{0xA353, 0x4E,8},
	{0xA354, 0x61,8},
	{0xA355, 0xA2,8},
	{0xA356, 0x32,8},
	{0xA357, 0x14,8},
	{0xA358, 0xA9,8},
	{0xA359, 0x1C,8},
	{0xA35A, 0xC6,8},
	{0xA35B, 0x3A,8},
	{0xA35C, 0x06,8},
	{0xA35D, 0xD2,8},
	{0xA35E, 0x51,8},
	{0xA35F, 0x84,8},
	{0xA360, 0xA8,8},
	{0xA361, 0x93,8},
	{0xA362, 0x1A,8},
	{0xA363, 0xCB,8},
	{0xA364, 0xA8,8},
	{0xA365, 0x46,8},
	{0xA366, 0x3B,8},
	{0xA367, 0x1B,8},
	{0xA368, 0x1A,8},
	{0xA369, 0xD2,8},
	{0xA36A, 0x9F,8},
	{0xA36B, 0xBC,8},
	{0xA36C, 0xD4,8},
	{0xA36D, 0xA2,8},
	{0xA36E, 0x06,8},
	{0xA36F, 0x09,8},
	{0xA370, 0xC8,8},
	{0xA371, 0x41,8},
	{0xA372, 0x34,8},
	{0xA373, 0x62,8},
	{0xA374, 0xD3,8},
	{0xA375, 0xA3,8},
	{0xA376, 0x10,8},
	{0xA377, 0x35,8},
	{0xA378, 0xA8,8},
	{0xA379, 0x2C,8},
	{0xA37A, 0xDD,8},
	{0xA37B, 0x88,8},
	{0xA37C, 0x45,8},
	{0xA37D, 0x35,8},
	{0xA37E, 0x12,8},
	{0xA37F, 0xD3,8},
	{0xA380, 0xA4,8},
	{0xA381, 0x34,8},
	{0xA382, 0x55,8},
	{0xA383, 0x21,8},
	{0xA384, 0x0E,8},
	{0xA385, 0x1D,8},
	{0xA386, 0x48,8},
	{0xA387, 0x3D,8},
	{0xA388, 0x15,8},
	{0xA389, 0xE2,8},
	{0xA38A, 0xD0,8},
	{0xA38B, 0x81,8},
	{0xA38C, 0xD4,8},
	{0xA38D, 0xC3,8},
	{0xA38E, 0x1E,8},
	{0xA38F, 0xF8,8},
	{0xA390, 0x44,8},
	{0xA391, 0xC8,8},
	{0xA392, 0x44,8},
	{0xA393, 0x20,8},
	{0xA394, 0xAA,8},
	{0xA395, 0x10,8},
	{0xA396, 0x82,8},
	{0xA397, 0xCC,8},
	{0xA398, 0xF3,8},
	{0xA399, 0x9C,8},
	{0xA39A, 0xE7,8},
	{0xA39B, 0x4C,8},
	{0xA39C, 0x67,8},
	{0xA39D, 0x3D,8},
	{0xA39E, 0x0E,8},
	{0xA39F, 0xFA,8},
	{0xA3A0, 0x90,8},
	{0xA3A1, 0x78,8},
	{0xA3A2, 0x98,8},
	{0xA3A3, 0xB3,8},
	{0xA3A4, 0x1A,8},
	{0xA3A5, 0xCA,8},
	{0xA3A6, 0x30,8},
	{0xA3A7, 0x66,8},
	{0xA3A8, 0x32,8},
	{0xA3A9, 0xAA,8},
	{0xA3AA, 0x79,8},
	{0xA3AB, 0x8E,8},
	{0xA3AC, 0x79,8},
	{0xA3AD, 0x6A,8},
	{0xA3AE, 0xD3,8},
	{0xA3AF, 0x19,8},
	{0xA3B0, 0xBA,8},
	{0xA3B1, 0x58,8},
	{0xA3B2, 0xE5,8},
	{0xA3B3, 0x29,8},
	{0xA3B4, 0x57,8},
	{0xA3B5, 0xB1,8},
	{0xA3B6, 0x0B,8},
	{0xA3B7, 0x67,8},
	{0xA3B8, 0x60,8},
	{0xA3B9, 0xD3,8},
	{0xA3BA, 0x98,8},
	{0xA3BB, 0xB9,8},
	{0xA3BC, 0x14,8},
	{0xA3BD, 0x25,8},
	{0xA3BE, 0x25,8},
	{0xA3BF, 0x1D,8},
	{0xA3C0, 0x41,8},
	{0xA3C1, 0x89,8},
	{0xA3C2, 0x51,8},
	{0xA3C3, 0xEC,8},
	{0xA3C4, 0xF2,8},
	{0xA3C5, 0x98,8},
	{0xA3C6, 0xBC,8},
	{0xA3C7, 0x6C,8},
	{0xA3C8, 0x25,8},
	{0xA3C9, 0x25,8},
	{0xA3CA, 0x0C,8},
	{0xA3CB, 0x11,8},
	{0xA3CC, 0x08,8},
	{0xA3CD, 0x43,8},
	{0xA3CE, 0x52,8},
	{0xA3CF, 0xC2,8},
	{0xA3D0, 0x95,8},
	{0xA3D1, 0xBD,8},
	{0xA3D2, 0xE4,8},
	{0xA3D3, 0x25,8},
	{0xA3D4, 0x2B,8},
	{0xA3D5, 0x25,8},
	{0xA3D6, 0x39,8},
	{0xA3D7, 0x48,8},
	{0xA3D8, 0x3F,8},
	{0xA3D9, 0x0A,8},
	{0xA3DA, 0x22,8},
	{0xA3DB, 0x12,8},
	{0xA3DC, 0xAB,8},
	{0xA3DD, 0xD8,8},
	{0xA3DE, 0xE5,8},
	{0xA3DF, 0x30,8},
	{0xA3E0, 0x68,8},
	{0xA3E1, 0xB1,8},
	{0xA3E2, 0x09,8},
	{0xA3E3, 0x45,8},
	{0xA3E4, 0x12,8},
	{0xA3E5, 0x42,8},
	{0xA3E6, 0x11,8},
	{0xA3E7, 0x98,8},
	{0xA3E8, 0x80,8},
	{0xA3E9, 0x85,8},
	{0xA3EA, 0x2F,8},
	{0xA3EB, 0x9F,8},
	{0xA3EC, 0x21,8},
	{0xA3ED, 0x8C,8},
	{0xA3EE, 0x55,8},
	{0xA3EF, 0x68,8},
	{0xA3F0, 0x92,8},
	{0xA3F1, 0x12,8},
	{0xA3F2, 0x99,8},
	{0xA3F3, 0x48,8},
	{0xA3F4, 0xA5,8},
	{0xA3F5, 0x2F,8},
	{0xA3F6, 0x95,8},
	{0xA3F7, 0xF1,8},
	{0xA3F8, 0xCD,8},
	{0xA3F9, 0x69,8},
	{0xA3FA, 0x0A,8},
	{0xA3FB, 0xA3,8},
	{0xA3FC, 0x16,8},
	{0xA3FD, 0xAD,8},
	{0xA3FE, 0x98,8},
	{0xA3FF, 0x85,8},
	{0xA400, 0x2F,8},
	{0xA401, 0x9E,8},
	{0xA402, 0xA1,8},
	{0xA403, 0x4D,8},
	{0xA404, 0x7B,8},
	{0xA405, 0xAE,8},
	{0xA406, 0x43,8},
	{0xA407, 0x1B,8},
	{0xA408, 0xCF,8},
	{0xA409, 0x60,8},
	{0xA40A, 0xC6,8},
	{0xA40B, 0x33,8},
	{0xA40C, 0xB0,8},
	{0xA40D, 0x99,8},
	{0xA40E, 0xCE,8},
	{0xA40F, 0x7A,8},
	{0xA410, 0xEC,8},
	{0xA411, 0x13,8},
	{0xA412, 0x9F,8},
	{0xA413, 0xE7,8},
	{0xA414, 0xD4,8},
	{0xA415, 0x06,8},
	{0xA416, 0x36,8},
	{0xA417, 0xB6,8},
	{0xA418, 0x79,8},
	{0xA419, 0x4E,8},
	{0xA41A, 0x7C,8},
	{0xA41B, 0xF8,8},
	{0xA41C, 0x83,8},
	{0xA41D, 0x5E,8},
	{0xA41E, 0x00,8},
	{0xA41F, 0x0C,8},
	{0xA420, 0x40,8},
	{0xA421, 0x00,8},

	{0x0068, 0x5F,8}, //SHD CONTROL SWITCHING FOR CAPTURE.(FOR LEDOFF) 

	// AE
	{0x400E, 0x2400,16},   //FREE FRAME PHOTOMETRY MODE CONVERGENCE LUMINANCE SETTING THIS IS THE ADJUSTMENT VALUE OF THE CONVERGENCE LUMINANCE WHEN USING THE FREE FRAME. CONVERGENCE IS PERFORMED IN SUCH A WAY THAT THE EVALUATION VALUE AND SETTING VALUE ARE MADE EQUAL.
	{0x4012, 0xFF0F,16},   //MOVIE AE SHUTTER SCALE (LIMITTED 1 FRAME)
	{0x4014, 0x0F07,16},   //MOVIE AE AGC MAX

	{0x4036, 0x0500,16},   //ISO SENSITIVITY ADJUSTMENT COEFFICIENT SET 090804 
	{0x403A, 0x000A,16},   //CMOS?T???B?J?P?M?C?Y???X?????? CMOS?T???B?J?????X???H???????????L???????????????????O????????
	{0x4042, 0x16,8},   //
	{0x4043, 0x16,8},   //
	{0x4027, 0x14,8},    // SPOT_SIDEWEIGHT  %
	{0x404D, 0xA0,8},   //????????????H??RATIO??L
	{0x404E, 0x02,8},   //????????????H??WEIGHT??L
	{0x4080, 0x64,8},   //AUTO LOW ??????A
	{0x4081, 0x20,8},   //AUTO LOW ??????B
	{0x4082, 0x35,8},   //?????D????????V  3X5?????D??
	{0x4083, 0x26,8},   //?????D????????H
	{0x4084, 0x05,8},   //?????????????
	{0x4085, 0x04,8},   //?????D?????D???I?? TYPE1??TYPE2??NEWTYPE3??TYPE4?????X 090804
	{0x4093, 0x03,8},   //EV?????G??
	{0x4098, 0x01,8},   //???W?I????ON????ES1??DEFAULT??0??????
	{0x4099, 0x10,8},   //???W?I????RATIO
	{0x409A, 0x02,8},   //???W?I????WEIGHT
	{0x409B, 0x64,8},   //???W?I?????????A
	{0x409C, 0x20,8},   //???W?I?????????B
	{0x409D, 0x08,8},   //HIGHJUDGE A
	{0x409E, 0x06,8},   //HIGHJUDGE B

	// AE FRAME ?J?E?S??
	{0x42BD, 0x04,8},    //?????D???00?G???D???W?? TYPE4
	{0x42BE, 0x05,8},    //?????D???01?G???D???W?? TYPE4
	{0x42BF, 0x06,8},    //?????D???02?G???D???W?? TYPE4
	{0x42C0, 0x07,8},    //?????D???03?G???D???W?? TYPE4
	{0x42C1, 0x08,8},    //?????D???04?G???D???W?? TYPE4
	{0x42C2, 0x07,8},    //?????D???05?G???D???W?? TYPE4
	{0x42C3, 0x06,8},    //?????D???06?G???D???W?? TYPE4
	{0x42C4, 0x05,8},    //?????D???07?G???D???W?? TYPE4
	{0x42C5, 0x04,8},    //?????D???08?G???D???W?? TYPE4
	{0x42C6, 0x09,8},    //?????D???09?G???D???W?? TYPE4
	{0x42C7, 0x0D,8},    //?????D???10?G???D???W?? TYPE4
	{0x42C8, 0x0E,8},    //?????D???11?G???D???W?? TYPE4
	{0x42C9, 0x10,8},    //?????D???12?G???D???W?? TYPE4
	{0x42CA, 0x11,8},    //?????D???13?G???D???W?? TYPE4
	{0x42CB, 0x10,8},    //?????D???14?G???D???W?? TYPE4
	{0x42CC, 0x0E,8},    //?????D???15?G???D???W?? TYPE4
	{0x42CD, 0x0D,8},    //?????D???16?G???D???W?? TYPE4
	{0x42CE, 0x09,8},    //?????D???17?G???D???W?? TYPE4
	{0x42CF, 0x0C,8},    //?????D???18?G???D???W?? TYPE4
	{0x42D0, 0x13,8},    //?????D???19?G???D???W?? TYPE4
	{0x42D1, 0x1F,8},    //?????D???20?G???D???W?? TYPE4
	{0x42D2, 0x27,8},    //?????D???21?G???D???W?? TYPE4
	{0x42D3, 0x33,8},    //?????D???22?G???D???W?? TYPE4
	{0x42D4, 0x27,8},    //?????D???23?G???D???W?? TYPE4
	{0x42D5, 0x1F,8},    //?????D???24?G???D???W?? TYPE4
	{0x42D6, 0x13,8},    //?????D???25?G???D???W?? TYPE4
	{0x42D7, 0x0C,8},    //?????D???26?G???D???W?? TYPE4
	{0x42D8, 0x0D,8},    //?????D???27?G???D???W?? TYPE4
	{0x42D9, 0x1F,8},    //?????D???28?G???D???W?? TYPE4
	{0x42DA, 0x47,8},    //?????D???29?G???D???W?? TYPE4
	{0x42DB, 0x4B,8},    //?????D???30?G???D???W?? TYPE4
	{0x42DC, 0x4B,8},    //?????D???31?G???D???W?? TYPE4
	{0x42DD, 0x4E,8},    //?????D???32?G???D???W?? TYPE4
	{0x42DE, 0x47,8},    //?????D???33?G???D???W?? TYPE4
	{0x42DF, 0x1F,8},    //?????D???34?G???D???W?? TYPE4
	{0x42E0, 0x0D,8},    //?????D???35?G???D???W?? TYPE4
	{0x42E1, 0x0E,8},    //?????D???36?G???D???W?? TYPE4
	{0x42E2, 0x2A,8},    //?????D???37?G???D???W?? TYPE4
	{0x42E3, 0x47,8},    //?????D???38?G???D???W?? TYPE4
	{0x42E4, 0x47,8},    //?????D???39?G???D???W?? TYPE4
	{0x42E5, 0x58,8},    //?????D???40?G???D???W?? TYPE4
	{0x42E6, 0x47,8},    //?????D???41?G???D???W?? TYPE4
	{0x42E7, 0x47,8},    //?????D???42?G???D???W?? TYPE4
	{0x42E8, 0x2A,8},    //?????D???43?G???D???W?? TYPE4
	{0x42E9, 0x0E,8},    //?????D???44?G???D???W?? TYPE4
	{0x42EA, 0x0D,8},    //?????D???45?G???D???W?? TYPE4
	{0x42EB, 0x1F,8},    //?????D???46?G???D???W?? TYPE4
	{0x42EC, 0x3B,8},    //?????D???47?G???D???W?? TYPE4
	{0x42ED, 0x43,8},    //?????D???48?G???D???W?? TYPE4
	{0x42EE, 0x4B,8},    //?????D???49?G???D???W?? TYPE4
	{0x42EF, 0x43,8},    //?????D???50?G???D???W?? TYPE4
	{0x42F0, 0x3B,8},    //?????D???51?G???D???W?? TYPE4
	{0x42F1, 0x1F,8},    //?????D???52?G???D???W?? TYPE4
	{0x42F2, 0x0D,8},    //?????D???53?G???D???W?? TYPE4
	{0x42F3, 0x0B,8},    //?????D???54?G???D???W?? TYPE4
	{0x42F4, 0x11,8},    //?????D???55?G???D???W?? TYPE4
	{0x42F5, 0x14,8},    //?????D???56?G???D???W?? TYPE4
	{0x42F6, 0x18,8},    //?????D???57?G???D???W?? TYPE4
	{0x42F7, 0x1C,8},    //?????D???58?G???D???W?? TYPE4
	{0x42F8, 0x18,8},    //?????D???59?G???D???W?? TYPE4
	{0x42F9, 0x14,8},    //?????D???60?G???D???W?? TYPE4
	{0x42FA, 0x11,8},    //?????D???61?G???D???W?? TYPE4
	{0x42FB, 0x0B,8},    //?????D???62?G???D???W?? TYPE4

	/////////////////////////////////////////
	//// CAT. : GAMMA1         ////////
	/////////////////////////////////////////
	//GAMMA G
	{0x5236, 0x0000,16},    //RGB-GAMMA G-KNOT0(TABLE0)
	{0x5238, 0x0004,16},   //RGB-GAMMA G-KNOT1(TABLE0)
	{0x523A, 0x0009,16},   //RGB-GAMMA G-KNOT2(TABLE0)
	{0x523C, 0x0013,16},   //RGB-GAMMA G-KNOT3(TABLE0)
	{0x523E, 0x001D,16},   //RGB-GAMMA G-KNOT4(TABLE0)
	{0x5240, 0x0026,16},   //RGB-GAMMA G-KNOT5(TABLE0)
	{0x5242, 0x002E,16},   //RGB-GAMMA G-KNOT6(TABLE0)
	{0x5244, 0x0034,16},   //RGB-GAMMA G-KNOT7(TABLE0)
	{0x5246, 0x003B,16},   //RGB-GAMMA G-KNOT8(TABLE0)
	{0x5248, 0x0041,16},   //RGB-GAMMA G-KNOT9(TABLE0)
	{0x524A, 0x0048,16},   //RGB-GAMMA G-KNOT10(TABLE0)
	{0x524C, 0x004D,16},   //RGB-GAMMA G-KNOT11(TABLE0)
	{0x524E, 0x0053,16},   //RGB-GAMMA G-KNOT12(TABLE0)
	{0x5250, 0x0058,16},   //RGB-GAMMA G-KNOT13(TABLE0)
	{0x5252, 0x005D,16},   //RGB-GAMMA G-KNOT14(TABLE0)
	{0x5254, 0x0062,16},   //RGB-GAMMA G-KNOT15(TABLE0)
	{0x5256, 0x0066,16},   //RGB-GAMMA G-KNOT16(TABLE0)
	{0x5258, 0x006A,16},   //RGB-GAMMA G-KNOT17(TABLE0)
	{0x525A, 0x004D,16},   //RGB-GAMMA G-KNOT18(TABLE0)
	{0x525C, 0x0084,16},   //RGB-GAMMA G-KNOT19(TABLE0)
	{0x525E, 0x00AC,16},   //RGB-GAMMA G-KNOT20(TABLE0)
	{0x5260, 0x00CC,16},   //RGB-GAMMA G-KNOT21(TABLE0)
	{0x5262, 0x00E2,16},   //RGB-GAMMA G-KNOT22(TABLE0)
	{0x5264, 0x00EE,16},   //RGB-GAMMA G-KNOT23(TABLE0)
	{0x5266, 0x00F7,16},    //RGB-GAMMA G-KNOT24(TABLE0)
	{0x5268, 0x00FF,16},   //RGB-GAMMA G-KNOT25(TABLE0)
	{0x526A, 0x0104,16},   //RGB-GAMMA G-KNOT26(TABLE0)
	{0x52A2, 0x0210,16},    //LOW-RANGE SETTING/OFFSET SETTING -R (TABLE0
	{0x52A4, 0x4005,16},    //LOW-RANGE SETTING -R (TABLE0)              
	{0x52A6, 0x0210,16},    //LOW-RANGE SETTING/OFFSET SETTING -G (TABLE0
	{0x52A8, 0x4005,16},    //LOW-RANGE SETTING -G (TABLE0)              
	{0x52AA, 0x0210,16},    //LOW-RANGE SETTING/OFFSET SETTING -B (TABLE0
	{0x52AC, 0x4005,16},    //LOW-RANGE SETTING -B (TABLE0)

	// EVREF             
	{0x52AE, 0x2100,16},   //CONVERGENCE LUMINANCE SETTING (TABLE0) 090827

	//////////////////////////////////////////////////
	//// CAT. : GAMMA2 FOR WDR HIGH ////
	/////////////////////////////////////////////////
	{0x5636, 0x0000,16},   //GAMMA G-KNOT0(TABLE2)
	{0x5638, 0x0006,16},   //GAMMA G-KNOT1(TABLE2)
	{0x563A, 0x0011,16},   //GAMMA G-KNOT2(TABLE2)
	{0x563C, 0x001B,16},   //GAMMA G-KNOT3(TABLE2)
	{0x563E, 0x0025,16},   //GAMMA G-KNOT4(TABLE2)
	{0x5640, 0x0030,16},   //GAMMA G-KNOT5(TABLE2)
	{0x5642, 0x0038,16},   //GAMMA G-KNOT6(TABLE2)
	{0x5644, 0x0040,16},   //GAMMA G-KNOT7(TABLE2)
	{0x5646, 0x0048,16},   //GAMMA G-KNOT8(TABLE2)
	{0x5648, 0x0050,16},   //GAMMA G-KNOT9(TABLE2)
	{0x564A, 0x0057,16},   //GAMMA G-KNOT10(TABLE2)
	{0x564C, 0x005E,16},   //GAMMA G-KNOT11(TABLE2)
	{0x564E, 0x0064,16},   //GAMMA G-KNOT12(TABLE2)
	{0x5650, 0x0069,16},   //GAMMA G-KNOT13(TABLE2)
	{0x5652, 0x0070,16},   //GAMMA G-KNOT14(TABLE2)
	{0x5654, 0x0074,16},   //GAMMA G-KNOT15(TABLE2)
	{0x5656, 0x007A,16},   //GAMMA G-KNOT16(TABLE2)
	{0x5658, 0x007E,16},   //GAMMA G-KNOT17(TABLE2)
	{0x565A, 0x0058,16},   //GAMMA G-KNOT18(TABLE2)
	{0x565C, 0x00A0,16},   //GAMMA G-KNOT19(TABLE2)
	{0x565E, 0x00C4,16},   //GAMMA G-KNOT20(TABLE2)
	{0x5660, 0x00D6,16},   //GAMMA G-KNOT21(TABLE2)
	{0x5662, 0x00E4,16},   //GAMMA G-KNOT22(TABLE2)
	{0x5664, 0x00EE,16},   //GAMMA G-KNOT23(TABLE2)
	{0x5666, 0x00F7,16},   //GAMMA G-KNOT24(TABLE2)
	{0x5668, 0x0100,16},   //GAMMA G-KNOT25(TABLE2)
	{0x566A, 0x0102,16},   //GAMMA G-KNOT26(TABLE2)
	{0x56A2, 0x0211,16},   //LOW-RANGE SETTING/OFFSET SETTING -R (TABLE2)
	{0x56A4, 0x4005,16},   //LOW-RANGE SETTING -R (TABLE2)
	{0x56A6, 0x0211,16},   //LOW-RANGE SETTING/OFFSET SETTING -G (TABLE2)
	{0x56A8, 0x4005,16},   //LOW-RANGE SETTING -G (TABLE2)
	{0x56AA, 0x0211,16},   //LOW-RANGE SETTING/OFFSET SETTING -B (TABLE2)
	{0x56AC, 0x4005,16},   //LOW-RANGE SETTING -B (TABLE2)
	{0x56AE, 0x1800,16},   //CONVERGENCE LUMINANCE SETTING FOR WDR-HIGH (TABLE2)

	// AWB FRM 
	{0x4600, 0xAA,8},    // INFRM_LEFT00 : 
	{0x4601, 0xAA,8},    // INFRM_LEFT01 : 
	{0x4602, 0xAA,8},    // INFRM_LEFT02 : 
	{0x4603, 0xAA,8},    // INFRM_LEFT03 : 
	{0x4604, 0xAA,8},    // INFRM_LEFT04 : 
	{0x4605, 0xAA,8},    // INFRM_LEFT05 : 
	{0x4606, 0xAA,8},    // INFRM_LEFT06 : 
	{0x4607, 0xAA,8},    // INFRM_LEFT07 : 
	{0x4608, 0xAA,8},    // INFRM_LEFT08 : 
	{0x4609, 0xAE,8},    // INFRM_LEFT09 : 
	{0x460A, 0xA0,8},    // INFRM_LEFT10 : 
	{0x460B, 0x8C,8},    // INFRM_LEFT11 : 
	{0x460C, 0x72,8},    // INFRM_LEFT12 : 
	{0x460D, 0x64,8},    // INFRM_LEFT13 : 
	{0x460E, 0x5A,8},    // INFRM_LEFT14 : 
	{0x460F, 0x52,8},    // INFRM_LEFT15 : 
	{0x4610, 0x4B,8},    // INFRM_LEFT16 : 
	{0x4611, 0x46,8},    // INFRM_LEFT17 : 
	{0x4612, 0x40,8},    // INFRM_LEFT18 : 
	{0x4613, 0x3A,8},    // INFRM_LEFT19 : 
	{0x4614, 0x36,8},    // INFRM_LEFT20 : 
	{0x4615, 0x34,8},    // INFRM_LEFT21 : 
	{0x4616, 0x33,8},    // INFRM_LEFT22 : 
	{0x4617, 0x32,8},    // INFRM_LEFT23 : 
	{0x4618, 0x31,8},    // INFRM_LEFT24 : 
	{0x4619, 0x30,8},    // INFRM_LEFT25 : 
	{0x461A, 0x2F,8},    // INFRM_LEFT26 : 
	{0x461B, 0x2E,8},    // INFRM_LEFT27 : 
	{0x461C, 0x2D,8},    // INFRM_LEFT28 : 
	{0x461D, 0x2C,8},    // INFRM_LEFT29 : 
	{0x461E, 0x2B,8},    // INFRM_LEFT30 : 
	{0x461F, 0x29,8},    // INFRM_LEFT31 : 
	{0x4620, 0x27,8},    // INFRM_LEFT32 : 
	{0x4621, 0x25,8},    // INFRM_LEFT33 : 
	{0x4622, 0x23,8},    // INFRM_LEFT34 : 
	{0x4623, 0x22,8},    // INFRM_LEFT35 : 
	{0x4624, 0x21,8},    // INFRM_LEFT36 : 
	{0x4625, 0x20,8},    // INFRM_LEFT37 : 
	{0x4626, 0xAF,8},    // INFRM_RIGHT00 : 
	{0x4627, 0xAF,8},    // INFRM_RIGHT01 : 
	{0x4628, 0xAF,8},    // INFRM_RIGHT02 : 
	{0x4629, 0xAF,8},    // INFRM_RIGHT03 : 
	{0x462A, 0xAF,8},    // INFRM_RIGHT04 : 
	{0x462B, 0xAF,8},    // INFRM_RIGHT05 : 
	{0x462C, 0xAF,8},    // INFRM_RIGHT06 : 
	{0x462D, 0xAF,8},    // INFRM_RIGHT07 : 
	{0x462E, 0xAF,8},    // INFRM_RIGHT08 : 
	{0x462F, 0xAE,8},    // INFRM_RIGHT09 : 
	{0x4630, 0xB2,8},    // INFRM_RIGHT10 : 
	{0x4631, 0xB4,8},    // INFRM_RIGHT11 : 
	{0x4632, 0xB6,8},    // INFRM_RIGHT12 : 
	{0x4633, 0xB4,8},    // INFRM_RIGHT13 : 
	{0x4634, 0x9B,8},    // INFRM_RIGHT14 : 
	{0x4635, 0x82,8},    // INFRM_RIGHT15 : 
	{0x4636, 0x78,8},    // INFRM_RIGHT16 : 
	{0x4637, 0x72,8},    // INFRM_RIGHT17 : 
	{0x4638, 0x6C,8},    // INFRM_RIGHT18 : 
	{0x4639, 0x67,8},    // INFRM_RIGHT19 : 
	{0x463A, 0x63,8},    // INFRM_RIGHT20 : 
	{0x463B, 0x5E,8},    // INFRM_RIGHT21 : 
	{0x463C, 0x58,8},    // INFRM_RIGHT22 : 
	{0x463D, 0x53,8},    // INFRM_RIGHT23 : 
	{0x463E, 0x4E,8},    // INFRM_RIGHT24 : 
	{0x463F, 0x4A,8},    // INFRM_RIGHT25 : 
	{0x4640, 0x45,8},    // INFRM_RIGHT26 : 
	{0x4641, 0x40,8},    // INFRM_RIGHT27 : 
	{0x4642, 0x3D,8},    // INFRM_RIGHT28 : 
	{0x4643, 0x3A,8},    // INFRM_RIGHT29 : 
	{0x4644, 0x38,8},    // INFRM_RIGHT30 : 
	{0x4645, 0x36,8},    // INFRM_RIGHT31 : 
	{0x4646, 0x35,8},    // INFRM_RIGHT32 : 
	{0x4647, 0x34,8},    // INFRM_RIGHT33 : 
	{0x4648, 0x39,8},    // INFRM_RIGHT34 : 
	{0x4649, 0x38,8},    // INFRM_RIGHT35 : 
	{0x464A, 0x37,8},    // INFRM_RIGHT36 : 
	{0x464B, 0x36,8},    // INFRM_RIGHT37 : 
	{0x464C, 0x21,8},    // INFRM_TOP : 
	{0x464D, 0x09,8},    // INFRM_BOTM : 
	{0x464E, 0x19,8},    // INFRM_FLTOP : 
	{0x464F, 0x10,8},    // INFRM_FLBOTM : 
	{0x4650, 0x91,8},    // INAIM_LEFT00 : 
	{0x4651, 0x91,8},    // INAIM_LEFT01 : 
	{0x4652, 0x91,8},    // INAIM_LEFT02 : 
	{0x4653, 0x91,8},    // INAIM_LEFT03 : 
	{0x4654, 0x91,8},    // INAIM_LEFT04 : 
	{0x4655, 0x91,8},    // INAIM_LEFT05 : 
	{0x4656, 0x91,8},    // INAIM_LEFT06 : 
	{0x4657, 0x91,8},    // INAIM_LEFT07 : 
	{0x4658, 0x91,8},    // INAIM_LEFT08 : 
	{0x4659, 0x91,8},    // INAIM_LEFT09 : 
	{0x465A, 0x91,8},    // INAIM_LEFT10 : 
	{0x465B, 0x91,8},    // INAIM_LEFT11 : 
	{0x465C, 0x91,8},    // INAIM_LEFT12 : 
	{0x465D, 0x78,8},    // INAIM_LEFT13 : 
	{0x465E, 0x68,8},    // INAIM_LEFT14 : 
	{0x465F, 0x6E,8},    // INAIM_LEFT15 : 
	{0x4660, 0x53,8},    // INAIM_LEFT16 : 
	{0x4661, 0x48,8},    // INAIM_LEFT17 : 
	{0x4662, 0x41,8},    // INAIM_LEFT18 : 
	{0x4663, 0x3D,8},    // INAIM_LEFT19 : 
	{0x4664, 0x3A,8},    // INAIM_LEFT20 : 
	{0x4665, 0x37,8},    // INAIM_LEFT21 : 
	{0x4666, 0x35,8},    // INAIM_LEFT22 : 
	{0x4667, 0x33,8},    // INAIM_LEFT23 : 
	{0x4668, 0x32,8},    // INAIM_LEFT24 : 
	{0x4669, 0x31,8},    // INAIM_LEFT25 : 
	{0x466A, 0x30,8},    // INAIM_LEFT26 : 
	{0x466B, 0x2E,8},    // INAIM_LEFT27 : 
	{0x466C, 0x2E,8},    // INAIM_LEFT28 : 
	{0x466D, 0x2D,8},    // INAIM_LEFT29 : 
	{0x466E, 0x2C,8},    // INAIM_LEFT30 : 
	{0x466F, 0x2A,8},    // INAIM_LEFT31 : 
	{0x4670, 0x28,8},    // INAIM_LEFT32 : 
	{0x4671, 0x26,8},    // INAIM_LEFT33 : 
	{0x4672, 0x29,8},    // INAIM_LEFT34 : 
	{0x4673, 0x28,8},    // INAIM_LEFT35 : 
	{0x4674, 0x29,8},    // INAIM_LEFT36 : 
	{0x4675, 0x26,8},    // INAIM_LEFT37 : 
	{0x4676, 0xFF,8},    // INAIM_RIGHT00 : 
	{0x4677, 0xFF,8},    // INAIM_RIGHT01 : 
	{0x4678, 0xFF,8},    // INAIM_RIGHT02 : 
	{0x4679, 0xFF,8},    // INAIM_RIGHT03 : 
	{0x467A, 0xFF,8},    // INAIM_RIGHT04 : 
	{0x467B, 0xFF,8},    // INAIM_RIGHT05 : 
	{0x467C, 0xFF,8},    // INAIM_RIGHT06 : 
	{0x467D, 0xFF,8},    // INAIM_RIGHT07 : 
	{0x467E, 0xFF,8},    // INAIM_RIGHT08 : 
	{0x467F, 0xFF,8},    // INAIM_RIGHT09 : 
	{0x4680, 0xD9,8},    // INAIM_RIGHT10 : 
	{0x4681, 0xB7,8},    // INAIM_RIGHT11 : 
	{0x4682, 0x96,8},    // INAIM_RIGHT12 : 
	{0x4683, 0x83,8},    // INAIM_RIGHT13 : 
	{0x4684, 0x7A,8},    // INAIM_RIGHT14 : 
	{0x4685, 0x6E,8},    // INAIM_RIGHT15 : 
	{0x4686, 0x6E,8},    // INAIM_RIGHT16 : 
	{0x4687, 0x6A,8},    // INAIM_RIGHT17 : 
	{0x4688, 0x64,8},    // INAIM_RIGHT18 : 
	{0x4689, 0x5F,8},    // INAIM_RIGHT19 : 
	{0x468A, 0x5B,8},    // INAIM_RIGHT20 : 
	{0x468B, 0x56,8},    // INAIM_RIGHT21 : 
	{0x468C, 0x53,8},    // INAIM_RIGHT22 : 
	{0x468D, 0x4E,8},    // INAIM_RIGHT23 : 
	{0x468E, 0x4A,8},    // INAIM_RIGHT24 : 
	{0x468F, 0x46,8},    // INAIM_RIGHT25 : 
	{0x4690, 0x43,8},    // INAIM_RIGHT26 : 
	{0x4691, 0x40,8},    // INAIM_RIGHT27 : 
	{0x4692, 0x3D,8},    // INAIM_RIGHT28 : 
	{0x4693, 0x3A,8},    // INAIM_RIGHT29 : 
	{0x4694, 0x37,8},    // INAIM_RIGHT30 : 
	{0x4695, 0x35,8},    // INAIM_RIGHT31 : 
	{0x4696, 0x34,8},    // INAIM_RIGHT32 : 
	{0x4697, 0x32,8},    // INAIM_RIGHT33 : 
	{0x4698, 0x31,8},    // INAIM_RIGHT34 : 
	{0x4699, 0x30,8},    // INAIM_RIGHT35 : 
	{0x469A, 0x2F,8},    // INAIM_RIGHT36 : 
	{0x469B, 0x2E,8},    // INAIM_RIGHT37 : 
	{0x469C, 0x21,8},    // INAIM_TOP : 
	{0x469D, 0x0F,8},    // INAIM_BOTM : 
	{0x469E, 0x18,8},    // INAIM_FLTOP : 
	{0x469F, 0x11,8},    // INAIM_FLBOTM : 
	{0x46A0, 0xFF,8},    // OUTFRM_LEFT00 : 
	{0x46A1, 0xFF,8},    // OUTFRM_LEFT01 : 
	{0x46A2, 0xFF,8},    // OUTFRM_LEFT02 : 
	{0x46A3, 0xFF,8},    // OUTFRM_LEFT03 : 
	{0x46A4, 0xFF,8},    // OUTFRM_LEFT04 : 
	{0x46A5, 0xFF,8},    // OUTFRM_LEFT05 : 
	{0x46A6, 0xFF,8},    // OUTFRM_LEFT06 : 
	{0x46A7, 0xFF,8},    // OUTFRM_LEFT07 : 
	{0x46A8, 0xFF,8},    // OUTFRM_LEFT08 : 
	{0x46A9, 0xFD,8},    // OUTFRM_LEFT09 : 
	{0x46AA, 0xCB,8},    // OUTFRM_LEFT10 : 
	{0x46AB, 0xA9,8},    // OUTFRM_LEFT11 : 
	{0x46AC, 0x90,8},    // OUTFRM_LEFT12 : 
	{0x46AD, 0x7D,8},    // OUTFRM_LEFT13 : 
	{0x46AE, 0x70,8},    // OUTFRM_LEFT14 : 
	{0x46AF, 0x65,8},    // OUTFRM_LEFT15 : 
	{0x46B0, 0x5C,8},    // OUTFRM_LEFT16 : 
	{0x46B1, 0x55,8},    // OUTFRM_LEFT17 : 
	{0x46B2, 0x4F,8},    // OUTFRM_LEFT18 : 
	{0x46B3, 0x32,8},    // OUTFRM_LEFT19 : 
	{0x46B4, 0x30,8},    // OUTFRM_LEFT20 : 
	{0x46B5, 0x2F,8},    // OUTFRM_LEFT21 : 
	{0x46B6, 0x2D,8},    // OUTFRM_LEFT22 : 
	{0x46B7, 0x2B,8},    // OUTFRM_LEFT23 : 
	{0x46B8, 0x29,8},    // OUTFRM_LEFT24 : 
	{0x46B9, 0x27,8},    // OUTFRM_LEFT25 : 
	{0x46BA, 0x25,8},    // OUTFRM_LEFT26 : 
	{0x46BB, 0x23,8},    // OUTFRM_LEFT27 : 
	{0x46BC, 0x21,8},    // OUTFRM_LEFT28 : 
	{0x46BD, 0x1F,8},    // OUTFRM_LEFT29 : 
	{0x46BE, 0x1D,8},    // OUTFRM_LEFT30 : 
	{0x46BF, 0x1B,8},    // OUTFRM_LEFT31 : 
	{0x46C0, 0x1A,8},    // OUTFRM_LEFT32 : 
	{0x46C1, 0x2A,8},    // OUTFRM_LEFT33 : 
	{0x46C2, 0x29,8},    // OUTFRM_LEFT34 : 
	{0x46C3, 0x28,8},    // OUTFRM_LEFT35 : 
	{0x46C4, 0x27,8},    // OUTFRM_LEFT36 : 
	{0x46C5, 0x26,8},    // OUTFRM_LEFT37 : 
	{0x46C6, 0xFF,8},    // OUTFRM_RIGHT00 : 
	{0x46C7, 0xFF,8},    // OUTFRM_RIGHT01 : 
	{0x46C8, 0xFF,8},    // OUTFRM_RIGHT02 : 
	{0x46C9, 0xFF,8},    // OUTFRM_RIGHT03 : 
	{0x46CA, 0xFF,8},    // OUTFRM_RIGHT04 : 
	{0x46CB, 0xFF,8},    // OUTFRM_RIGHT05 : 
	{0x46CC, 0xFF,8},    // OUTFRM_RIGHT06 : 
	{0x46CD, 0xFF,8},    // OUTFRM_RIGHT07 : 
	{0x46CE, 0xFF,8},    // OUTFRM_RIGHT08 : 
	{0x46CF, 0xFF,8},    // OUTFRM_RIGHT09 : 
	{0x46D0, 0xD3,8},    // OUTFRM_RIGHT10 : 
	{0x46D1, 0xB1,8},    // OUTFRM_RIGHT11 : 
	{0x46D2, 0x98,8},    // OUTFRM_RIGHT12 : 
	{0x46D3, 0x85,8},    // OUTFRM_RIGHT13 : 
	{0x46D4, 0x78,8},    // OUTFRM_RIGHT14 : 
	{0x46D5, 0x6D,8},    // OUTFRM_RIGHT15 : 
	{0x46D6, 0x64,8},    // OUTFRM_RIGHT16 : 
	{0x46D7, 0x5D,8},    // OUTFRM_RIGHT17 : 
	{0x46D8, 0x57,8},    // OUTFRM_RIGHT18 : 
	{0x46D9, 0x63,8},    // OUTFRM_RIGHT19 : 
	{0x46DA, 0x5E,8},    // OUTFRM_RIGHT20 : 
	{0x46DB, 0x5A,8},    // OUTFRM_RIGHT21 : 
	{0x46DC, 0x56,8},    // OUTFRM_RIGHT22 : 
	{0x46DD, 0x52,8},    // OUTFRM_RIGHT23 : 
	{0x46DE, 0x50,8},    // OUTFRM_RIGHT24 : 
	{0x46DF, 0x4E,8},    // OUTFRM_RIGHT25 : 
	{0x46E0, 0x4C,8},    // OUTFRM_RIGHT26 : 
	{0x46E1, 0x4A,8},    // OUTFRM_RIGHT27 : 
	{0x46E2, 0x48,8},    // OUTFRM_RIGHT28 : 
	{0x46E3, 0x46,8},    // OUTFRM_RIGHT29 : 
	{0x46E4, 0x44,8},    // OUTFRM_RIGHT30 : 
	{0x46E5, 0x43,8},    // OUTFRM_RIGHT31 : 
	{0x46E6, 0x42,8},    // OUTFRM_RIGHT32 : 
	{0x46E7, 0x32,8},    // OUTFRM_RIGHT33 : 
	{0x46E8, 0x31,8},    // OUTFRM_RIGHT34 : 
	{0x46E9, 0x30,8},    // OUTFRM_RIGHT35 : 
	{0x46EA, 0x2F,8},    // OUTFRM_RIGHT36 : 
	{0x46EB, 0x2E,8},    // OUTFRM_RIGHT37 : 
	{0x46EC, 0x1E,8},    // OUTFRM_TOP : 
	{0x46ED, 0x14,8},    // OUTFRM_BOTM : 
	{0x46EE, 0x19,8},    // OUTFRM_FLTOP : 
	{0x46EF, 0x10,8},    // OUTFRM_FLBOTM : 
	{0x46F0, 0xFF,8},    // OUTAIM_LEFT00 : 
	{0x46F1, 0xFF,8},    // OUTAIM_LEFT01 : 
	{0x46F2, 0xFF,8},    // OUTAIM_LEFT02 : 
	{0x46F3, 0xFF,8},    // OUTAIM_LEFT03 : 
	{0x46F4, 0xFF,8},    // OUTAIM_LEFT04 : 
	{0x46F5, 0xFF,8},    // OUTAIM_LEFT05 : 
	{0x46F6, 0xFF,8},    // OUTAIM_LEFT06 : 
	{0x46F7, 0xFF,8},    // OUTAIM_LEFT07 : 
	{0x46F8, 0xFF,8},    // OUTAIM_LEFT08 : 
	{0x46F9, 0xFF,8},    // OUTAIM_LEFT09 : 
	{0x46FA, 0xCD,8},    // OUTAIM_LEFT10 : 
	{0x46FB, 0xAB,8},    // OUTAIM_LEFT11 : 
	{0x46FC, 0x92,8},    // OUTAIM_LEFT12 : 
	{0x46FD, 0x7F,8},    // OUTAIM_LEFT13 : 
	{0x46FE, 0x72,8},    // OUTAIM_LEFT14 : 
	{0x46FF, 0x67,8},    // OUTAIM_LEFT15 : 
	{0x4700, 0x5E,8},    // OUTAIM_LEFT16 : 
	{0x4701, 0x57,8},    // OUTAIM_LEFT17 : 
	{0x4702, 0x51,8},    // OUTAIM_LEFT18 : 
	{0x4703, 0x4E,8},    // OUTAIM_LEFT19 : 
	{0x4704, 0x49,8},    // OUTAIM_LEFT20 : 
	{0x4705, 0x45,8},    // OUTAIM_LEFT21 : 
	{0x4706, 0x40,8},    // OUTAIM_LEFT22 : 
	{0x4707, 0x3E,8},    // OUTAIM_LEFT23 : 
	{0x4708, 0x3B,8},    // OUTAIM_LEFT24 : 
	{0x4709, 0x38,8},    // OUTAIM_LEFT25 : 
	{0x470A, 0x37,8},    // OUTAIM_LEFT26 : 
	{0x470B, 0x35,8},    // OUTAIM_LEFT27 : 
	{0x470C, 0x33,8},    // OUTAIM_LEFT28 : 
	{0x470D, 0x32,8},    // OUTAIM_LEFT29 : 
	{0x470E, 0x30,8},    // OUTAIM_LEFT30 : 
	{0x470F, 0x2E,8},    // OUTAIM_LEFT31 : 
	{0x4710, 0x2D,8},    // OUTAIM_LEFT32 : 
	{0x4711, 0x2C,8},    // OUTAIM_LEFT33 : 
	{0x4712, 0x2B,8},    // OUTAIM_LEFT34 : 
	{0x4713, 0x2A,8},    // OUTAIM_LEFT35 : 
	{0x4714, 0x29,8},    // OUTAIM_LEFT36 : 
	{0x4715, 0x28,8},    // OUTAIM_LEFT37 : 
	{0x4716, 0xFF,8},    // OUTAIM_RIGHT00 : 
	{0x4717, 0xFF,8},    // OUTAIM_RIGHT01 : 
	{0x4718, 0xFF,8},    // OUTAIM_RIGHT02 : 
	{0x4719, 0xFF,8},    // OUTAIM_RIGHT03 : 
	{0x471A, 0xFF,8},    // OUTAIM_RIGHT04 : 
	{0x471B, 0xFF,8},    // OUTAIM_RIGHT05 : 
	{0x471C, 0xFF,8},    // OUTAIM_RIGHT06 : 
	{0x471D, 0xFF,8},    // OUTAIM_RIGHT07 : 
	{0x471E, 0xFF,8},    // OUTAIM_RIGHT08 : 
	{0x471F, 0xFF,8},    // OUTAIM_RIGHT09 : 
	{0x4720, 0xD3,8},    // OUTAIM_RIGHT10 : 
	{0x4721, 0xB1,8},    // OUTAIM_RIGHT11 : 
	{0x4722, 0x98,8},    // OUTAIM_RIGHT12 : 
	{0x4723, 0x85,8},    // OUTAIM_RIGHT13 : 
	{0x4724, 0x78,8},    // OUTAIM_RIGHT14 : 
	{0x4725, 0x6D,8},    // OUTAIM_RIGHT15 : 
	{0x4726, 0x64,8},    // OUTAIM_RIGHT16 : 
	{0x4727, 0x5D,8},    // OUTAIM_RIGHT17 : 
	{0x4728, 0x57,8},    // OUTAIM_RIGHT18 : 
	{0x4729, 0x52,8},    // OUTAIM_RIGHT19 : 
	{0x472A, 0x4D,8},    // OUTAIM_RIGHT20 : 
	{0x472B, 0x49,8},    // OUTAIM_RIGHT21 : 
	{0x472C, 0x44,8},    // OUTAIM_RIGHT22 : 
	{0x472D, 0x42,8},    // OUTAIM_RIGHT23 : 
	{0x472E, 0x3F,8},    // OUTAIM_RIGHT24 : 
	{0x472F, 0x3C,8},    // OUTAIM_RIGHT25 : 
	{0x4730, 0x3B,8},    // OUTAIM_RIGHT26 : 
	{0x4731, 0x39,8},    // OUTAIM_RIGHT27 : 
	{0x4732, 0x37,8},    // OUTAIM_RIGHT28 : 
	{0x4733, 0x36,8},    // OUTAIM_RIGHT29 : 
	{0x4734, 0x34,8},    // OUTAIM_RIGHT30 : 
	{0x4735, 0x34,8},    // OUTAIM_RIGHT31 : 
	{0x4736, 0x33,8},    // OUTAIM_RIGHT32 : 
	{0x4737, 0x32,8},    // OUTAIM_RIGHT33 : 
	{0x4738, 0x31,8},    // OUTAIM_RIGHT34 : 
	{0x4739, 0x30,8},    // OUTAIM_RIGHT35 : 
	{0x473A, 0x2F,8},    // OUTAIM_RIGHT36 : 
	{0x473B, 0x2E,8},    // OUTAIM_RIGHT37 : 
	{0x473C, 0x1D,8},    // OUTAIM_TOP : 
	{0x473D, 0x16,8},    // OUTAIM_BOTM : 
	{0x473E, 0x19,8},    // OUTAIM_FLTOP : 
	{0x473F, 0x16,8},    // OUTAIM_FLBOTM : 

	// AWB
	{0x4400, 0x00,8},   //AWB MODE
	{0x4C0A, 0x0120,16},   //ADD 0105 LJH WIDTH
	{0x4C0C, 0x0114,16},   //ADD 0105 LJH HEIGTH
	{0x4401, 0x02,8},   //INDOOR/OUTDOOR DECISION MODE
	{0x4402, 0x01,8},   //REDNESS SHIFT ON
	{0x4403, 0x01,8},   //BLACK BODY SHIFT ON FLAG WITH OUTSIDE OUTDOOR FRAME
	{0x4404, 0x00,8},   //TRIGGER-TYPE PUSHLOCKON FLAG
	{0x4405, 0x01,8},   //ATW CYCLE
	{0x4406, 0x0A,8},   //HIGH-SPEED OPERATION MASK COUNTER
	{0x4408, 0x0100,16},   //G GAIN
	{0x440A, 0x1000,16},   //INITIAL VALUE OF ERROR AMOUNT FOR DETERMINING COMMENCEMENT OF AWB STARTUP
	{0x440C, 0x16,8},   //INDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x440D, 0x16,8},   //INDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x440E, 0x16,8},   //INDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x440F, 0x16,8},   //INDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4410, 0x04,8},   //INDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4411, 0x04,8},   //INDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4412, 0x04,8},   //INDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4413, 0x04,8},   //INDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4414, 0x20,8},   //OUTDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4415, 0x20,8},   //OUTDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4416, 0x20,8},   //OUTDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4417, 0x20,8},   //OUTDOOR DEAD BAND (HIGH) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4418, 0x04,8},   //OUTDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4419, 0x04,8},   //OUTDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x441A, 0x04,8},   //OUTDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x441B, 0x04,8},   //OUTDOOR DEAD BAND (MID) EXPANSION COEFFICIENT [??] DIRECTION
	{0x441C, 0x01,8},   //CONVERGENCE END DEAD BAND (LOW) EXPANSION COEFFICIENT [??] DIRECTION
	{0x441D, 0x01,8},   //CONVERGENCE END DEAD BAND (LOW) EXPANSION COEFFICIENT [??] DIRECTION
	{0x441E, 0x01,8},   //CONVERGENCE END DEAD BAND (LOW) EXPANSION COEFFICIENT [??] DIRECTION
	{0x441F, 0x01,8},   //CONVERGENCE END DEAD BAND (LOW) EXPANSION COEFFICIENT [??] DIRECTION
	{0x4420, 0x00,8},   //AIM WEIGHT (FOR HIGH-SPEED OPERATION)
	{0x4421, 0x04,8},   //GAIN STEP (FOR HIGH-SPEED OPERATION)
	{0x4422, 0x10,8},   //OPERATION DELAY (FOR ATW MODE)
	{0x4423, 0x01,8},   //AIM WEIGHT (FOR ATW MODE)
	{0x4424, 0x02,8},   //GAIN STEP (FOR ATW MODE)
	{0x4425, 0x02,8},   //GAIN STEP (ALL PULL-IN MODE)
	{0x4426, 0xFFFF,16},   //SOFT LIMIT (WITH HIGH-SPEED OPERATION)
	{0x4428, 0xFFFF,16},   //SOFT LIMIT (WITH NORMAL OPERATION)
	{0x442A, 0x0D2D,16},   //FIXED COORDINATES FOR START-UP (INDOOR) R/G
	{0x442C, 0x13F6,16},   //FIXED COORDINATE FOR START-UP (INDOOR) B/G
	{0x442E, 0x0BBC,16},   //FIXED COORDINATE FOR START-UP (OUTDOOR) R/G
	{0x4430, 0x1775,16},   //FIXED COORDINATE FOR START-UP (OUTDOOR) B/G
	{0x4432, 0x1000,16},   //PUSHLOCK COORDINATE R/G
	{0x4434, 0x1000,16},   //PUSHLOCK COORDINATE B/G   
	{0x444A, 0x7D00,16},   //INDOOR DECISION THRESHOLD VALUE
	{0x444C, 0x80E8,16},   //OUTDOOR DECISION THRESHOLD VALUE
	{0x444E, 0x0A,8},   //NUMBER OF TIMES TO PICK UP CHATTERING FOR INDOOR/OUTDOOR DECISION
	{0x444F, 0x00,8},   //RATIO REDNESS SHIFT COEFFICIENT
	{0x4450, 0x0000,16},   //REDNESS SHIFT DISTANCE LIMIT
	{0x4452, 0x1E,8},   //SETTING FOR NUMBER OF FRAMES TO ABANDON IN INDOOR ?? OUTDOOR SWITCHING
	{0x4453, 0x7B,8},   //
	{0x4454, 0x0000,16},   //FIXED SHIFT PARAMETER R FOR ATW (INDOOR)
	{0x4456, 0x0000,16},   //FIXED SHIFT PARAMETER B FOR ATW (INDOOR)
	{0x4458, 0x0000,16},   //FIXED SHIFT PARAMETER R FOR ATW (OUTDOOR)
	{0x445A, 0x0000,16},   //FIXED SHIFT PARAMETER B FOR ATW (OUTDOOR)
	{0x445C, 0x0000,16},   //SHIFT PARAMETER R FOR STROBE
	{0x445E, 0x0000,16},   //SHIFT PARAMETER B FOR STROBE
	{0x4460, 0x02C0,16},   //HIGH LUMINANCE UPPER LIMIT AT INTEGRATION LEVEL BY LUMINANCE
	{0x4462, 0x0020,16},   //MEDIUM LUMINANCE LOWER LIMIT AT INTEGRATION LEVEL BY LUMINANCE
	{0x4464, 0x0230,16},   //PEAK CLIP VALUE R WHEN CALCULATING DETECTION VALUE FOR AWB
	{0x4466, 0x03C2,16},   //PEAK CLIP VALUE G WHEN CALCULATING DETECTION VALUE FOR AWB
	{0x4468, 0x0230,16},   //PEAK CLIP VALUE B WHEN CALCULATING DETECTION VALUE FOR AWB
	{0x446A, 0x20,8},   //RATIO RE-CALCULATION RANGE
	{0x446B, 0x05,8},   //NUMBER DECIDED AS INSIDE WINDOW 
	{0x446C, 0x00,8},   //AIM ESTIMATION SYSTEM
	{0x446D, 0x20,8},   //USER SPECIFIED RANGE
	{0x446E, 0x0000,16},   //INDOOR/OUTDOOR DECISION BRATIO THRESHOLD VALUE
	{0x4470, 0x05,8},   //SPECIFYING THE RATIO NUMBER INSIDE THE NEW USER USE DECISION RANGE.
	{0x4471, 0x08,8},   //WEIGHT COEFFICIENT WHEN CALCULATING RATIO OF ATW 
	                                          
	//////////////////////////////////////////////////////////
	//// AF DRIVER  START (FOR SEHF MODULE)   ////
	//////////////////////////////////////////////////////////
	{0xC000, 0xB5F7,16},
	{0xC002, 0x4D0E,16},
	{0xC004, 0x4C0E,16},
	{0xC006, 0x4A0F,16},  
	{0xC008, 0x4F0F,16},
	{0xC00A, 0x4B10,16},  
	{0xC00C, 0x2100,16},
	{0xC00E, 0x2A00,16},  
	{0xC010, 0xD904,16},
	{0xC012, 0x5C6E,16},  
	{0xC014, 0x5466,16},
	{0xC016, 0x3101,16},  
	{0xC018, 0x4291,16},
	{0xC01A, 0xD3FA,16},  
	{0xC01C, 0x2100,16},
	{0xC01E, 0x2B00,16},  
	{0xC020, 0xD904,16},
	{0xC022, 0x2200,16},  
	{0xC024, 0x547A,16},
	{0xC026, 0x3101,16},  
	{0xC028, 0x4299,16},
	{0xC02A, 0xD3FB,16},  
	{0xC02C, 0x4908,16},
	{0xC02E, 0x6088,16},  
	{0xC030, 0x9901,16},
	{0xC032, 0x4808,16},  
	{0xC034, 0x6008,16},
	{0xC036, 0xB003,16},  
	{0xC038, 0x2001,16},
	{0xC03A, 0xBDF0,16},  
	{0xC03C, 0x0D20,16},
	{0xC03E, 0x0018,16},  
	{0xC040, 0x0D20,16},
	{0xC042, 0x0018,16},  
	{0xC044, 0x0004,16},
	{0xC046, 0x0000,16},  
	{0xC048, 0x0D24,16},
	{0xC04A, 0x0018,16},  
	{0xC04C, 0x000C,16},
	{0xC04E, 0x0000,16},  
	{0xC050, 0x0D24,16},
	{0xC052, 0x0018,16},  
	{0xC054, 0x0D20,16},
	{0xC056, 0x0018,16},  
	{0xC058, 0xB580,16},
	{0xC05A, 0x781F,16},  
	{0xC05C, 0x2F00,16},
	{0xC05E, 0xD102,16},  
	{0xC060, 0xF000,16},
	{0xC062, 0xF8B8,16},  
	{0xC064, 0xBD80,16},
	{0xC066, 0xF000,16},  
	{0xC068, 0xF801,16},
	{0xC06A, 0xE7FB,16},  
	{0xC06C, 0xB5FF,16},
	{0xC06E, 0xB083,16},  
	{0xC070, 0x1C0C,16},
	{0xC072, 0x1C1F,16},  
	{0xC074, 0x2500,16},
	{0xC076, 0x2308,16},  
	{0xC078, 0x5EF8,16},
	{0xC07A, 0x43EB,16},  
	{0xC07C, 0x4E54,16},
	{0xC07E, 0x4298,16},  
	{0xC080, 0xD128,16},
	{0xC082, 0x80F2,16},  
	{0xC084, 0x1AA0,16},
	{0xC086, 0xD502,16},  
	{0xC088, 0x4240,16},
	{0xC08A, 0x2101,16},  
	{0xC08C, 0xE000,16},
	{0xC08E, 0x2100,16},  
	{0xC090, 0x7031,16},
	{0xC092, 0x0400,16},  
	{0xC094, 0x0C00,16},
	{0xC096, 0x8070,16},  
	{0xC098, 0x80B0,16},
	{0xC09A, 0xD11B,16},  
	{0xC09C, 0x0120,16},
	{0xC09E, 0x0400,16},  
	{0xC0A0, 0x0C00,16},
	{0xC0A2, 0xAB00,16},  
	{0xC0A4, 0x8018,16},
	{0xC0A6, 0xF000,16},  
	{0xC0A8, 0xF8EE,16},
	{0xC0AA, 0xAB00,16},  
	{0xC0AC, 0x8018,16},
	{0xC0AE, 0x68B0,16},  
	{0xC0B0, 0x466A,16},
	{0xC0B2, 0x2102,16},  
	{0xC0B4, 0x6803,16},
	{0xC0B6, 0x200C,16},  
	{0xC0B8, 0xF67F,16},
	{0xC0BA, 0xFBAF,16},  
	{0xC0BC, 0x2801,16},
	{0xC0BE, 0xD004,16},  
	{0xC0C0, 0x2000,16},
	{0xC0C2, 0x43C0,16},  
	{0xC0C4, 0x8138,16},
	{0xC0C6, 0x2000,16},  
	{0xC0C8, 0xE07F,16},
	{0xC0CA, 0x9803,16},  
	{0xC0CC, 0x8004,16},
	{0xC0CE, 0x2001,16},  
	{0xC0D0, 0x8138,16},
	{0xC0D2, 0xE07A,16},  
	{0xC0D4, 0x7878,16},
	{0xC0D6, 0x4E3E,16},  
	{0xC0D8, 0x9002,16},
	{0xC0DA, 0x78B9,16},  
	{0xC0DC, 0x0040,16},
	{0xC0DE, 0x1A40,16},  
	{0xC0E0, 0x78F9,16},
	{0xC0E2, 0x1A40,16},  
	{0xC0E4, 0x88B1,16},
	{0xC0E6, 0x0049,16},  
	{0xC0E8, 0xF67F,16},
	{0xC0EA, 0xFB9C,16},  
	{0xC0EC, 0x0404,16},
	{0xC0EE, 0x0C24,16},  
	{0xC0F0, 0x8A38,16},
	{0xC0F2, 0x42A0,16},  
	{0xC0F4, 0xDA02,16},
	{0xC0F6, 0x2101,16},  
	{0xC0F8, 0x1C04,16},
	{0xC0FA, 0xE000,16},  
	{0xC0FC, 0x2100,16},
	{0xC0FE, 0x9101,16},  
	{0xC100, 0x9802,16},
	{0xC102, 0x2600,16},  
	{0xC104, 0x2800,16},
	{0xC106, 0xDD0B,16},  
	{0xC108, 0x1C30,16},
	{0xC10A, 0x1C21,16},  
	{0xC10C, 0x1C3A,16},
	{0xC10E, 0xF000,16},  
	{0xC110, 0xF89F,16},
	{0xC112, 0x1940,16},  
	{0xC114, 0x0405,16},
	{0xC116, 0x0C2D,16},  
	{0xC118, 0x7878,16},
	{0xC11A, 0x3601,16},  
	{0xC11C, 0x42B0,16},
	{0xC11E, 0xDCF3,16},  
	{0xC120, 0x4E2B,16},
	{0xC122, 0x88B0,16},  
	{0xC124, 0x1B40,16},
	{0xC126, 0x80B0,16},  
	{0xC128, 0x2308,16},
	{0xC12A, 0x5EF8,16},  
	{0xC12C, 0x2301,16},
	{0xC12E, 0x42D8,16},  
	{0xC130, 0xD113,16},
	{0xC132, 0x9901,16},  
	{0xC134, 0x2900,16},
	{0xC136, 0xD102,16},  
	{0xC138, 0x2101,16},
	{0xC13A, 0x8139,16},  
	{0xC13C, 0xE00D,16},
	{0xC13E, 0x8871,16},  
	{0xC140, 0x1C28,16},
	{0xC142, 0xF67F,16},  
	{0xC144, 0xFAB9,16},
	{0xC146, 0x8138,16},  
	{0xC148, 0x8871,16},
	{0xC14A, 0x1C28,16},  
	{0xC14C, 0xF67F,16},
	{0xC14E, 0xFAB4,16},  
	{0xC150, 0x2900,16},
	{0xC152, 0xD002,16},  
	{0xC154, 0x8938,16},
	{0xC156, 0x3001,16},  
	{0xC158, 0x8138,16},
	{0xC15A, 0x7878,16},  
	{0xC15C, 0x2500,16},
	{0xC15E, 0x2800,16},  
	{0xC160, 0xDD32,16},
	{0xC162, 0x1C28,16},  
	{0xC164, 0x1C21,16},
	{0xC166, 0x1C3A,16},  
	{0xC168, 0xF000,16},
	{0xC16A, 0xF872,16},  
	{0xC16C, 0x9901,16},
	{0xC16E, 0x2900,16},  
	{0xC170, 0xD107,16},
	{0xC172, 0x88B1,16},  
	{0xC174, 0x2900,16},
	{0xC176, 0xDD04,16},  
	{0xC178, 0x3001,16},
	{0xC17A, 0x0400,16},  
	{0xC17C, 0x0C00,16},
	{0xC17E, 0x3901,16},  
	{0xC180, 0x80B1,16},
	{0xC182, 0x7831,16},  
	{0xC184, 0x2900,16},
	{0xC186, 0xD102,16},  
	{0xC188, 0x88F1,16},
	{0xC18A, 0x1808,16},  
	{0xC18C, 0xE001,16},
	{0xC18E, 0x88F1,16},  
	{0xC190, 0x1A08,16},
	{0xC192, 0x80F0,16},  
	{0xC194, 0x88F0,16},
	{0xC196, 0xAB00,16},  
	{0xC198, 0x0100,16},
	{0xC19A, 0x0400,16},  
	{0xC19C, 0x0C00,16},
	{0xC19E, 0x8018,16},  
	{0xC1A0, 0xF000,16},
	{0xC1A2, 0xF871,16},  
	{0xC1A4, 0xAB00,16},
	{0xC1A6, 0x8018,16},  
	{0xC1A8, 0x68B0,16},
	{0xC1AA, 0x466A,16},  
	{0xC1AC, 0x2102,16},
	{0xC1AE, 0x6803,16},  
	{0xC1B0, 0x200C,16},
	{0xC1B2, 0xF67F,16},  
	{0xC1B4, 0xFB32,16},
	{0xC1B6, 0x2801,16},  
	{0xC1B8, 0xD182,16},
	{0xC1BA, 0x88F0,16},  
	{0xC1BC, 0x9903,16},
	{0xC1BE, 0x8008,16},  
	{0xC1C0, 0x7878,16},
	{0xC1C2, 0x3501,16},  
	{0xC1C4, 0x42A8,16},
	{0xC1C6, 0xDCCC,16},  
	{0xC1C8, 0x2001,16},
	{0xC1CA, 0xB007,16},  
	{0xC1CC, 0xBDF0,16},
	{0xC1CE, 0x0000,16},  
	{0xC1D0, 0x0D24,16},
	{0xC1D2, 0x0018,16},  
	{0xC1D4, 0xB5F8,16},
	{0xC1D6, 0x1C1C,16},  
	{0xC1D8, 0x1C15,16},
	{0xC1DA, 0x1C06,16},  
	{0xC1DC, 0x1C0F,16},
	{0xC1DE, 0x0138,16},  
	{0xC1E0, 0x0400,16},
	{0xC1E2, 0x0C00,16},  
	{0xC1E4, 0xAB00,16},
	{0xC1E6, 0x8018,16},  
	{0xC1E8, 0xF000,16},
	{0xC1EA, 0xF84D,16},  
	{0xC1EC, 0xAB00,16},
	{0xC1EE, 0x8018,16},  
	{0xC1F0, 0x4809,16},
	{0xC1F2, 0x466A,16},  
	{0xC1F4, 0x6880,16},
	{0xC1F6, 0x2102,16},  
	{0xC1F8, 0x6803,16},
	{0xC1FA, 0x200C,16},  
	{0xC1FC, 0xF67F,16},
	{0xC1FE, 0xFB0D,16},  
	{0xC200, 0x2801,16},
	{0xC202, 0xD001,16},  
	{0xC204, 0x2000,16},
	{0xC206, 0xE006,16},  
	{0xC208, 0x1C38,16},
	{0xC20A, 0x1C29,16},  
	{0xC20C, 0x1C22,16},
	{0xC20E, 0x8037,16},  
	{0xC210, 0xF000,16},
	{0xC212, 0xF804,16},  
	{0xC214, 0x2001,16},
	{0xC216, 0xBDF8,16},  
	{0xC218, 0x0D24,16},
	{0xC21A, 0x0018,16},  
	{0xC21C, 0xB580,16},
	{0xC21E, 0x1C17,16},  
	{0xC220, 0x1A40,16},
	{0xC222, 0xD500,16},  
	{0xC224, 0x4240,16},
	{0xC226, 0x89BA,16},  
	{0xC228, 0x0901,16},
	{0xC22A, 0x4351,16},  
	{0xC22C, 0x0700,16},
	{0xC22E, 0x0F00,16},  
	{0xC230, 0xD000,16},
	{0xC232, 0x1851,16},  
	{0xC234, 0x89F8,16},
	{0xC236, 0x1841,16},  
	{0xC238, 0x6978,16},
	{0xC23A, 0x1809,16},  
	{0xC23C, 0xF67F,16},
	{0xC23E, 0xFA3C,16},  
	{0xC240, 0x4B02,16},
	{0xC242, 0x4298,16},  
	{0xC244, 0xD900,16},
	{0xC246, 0x1C18,16},  
	{0xC248, 0x8138,16},
	{0xC24A, 0xBD80,16},  
	{0xC24C, 0x7FFF,16},
	{0xC24E, 0x0000,16},  
	{0xC250, 0xB580,16},
	{0xC252, 0x7893,16},  
	{0xC254, 0x4283,16},
	{0xC256, 0xDD05,16},  
	{0xC258, 0x3001,16},
	{0xC25A, 0x4348,16},  
	{0xC25C, 0x1C0A,16},
	{0xC25E, 0x1C01,16},  
	{0xC260, 0x1C58,16},
	{0xC262, 0xE009,16},  
	{0xC264, 0x7853,16},
	{0xC266, 0x78D2,16},  
	{0xC268, 0x1A9F,16},
	{0xC26A, 0x4287,16},  
	{0xC26C, 0xDC09,16},
	{0xC26E, 0x1A18,16},  
	{0xC270, 0x4348,16},
	{0xC272, 0x1C0B,16},  
	{0xC274, 0x1C01,16},
	{0xC276, 0x1C50,16},  
	{0xC278, 0xF67F,16},
	{0xC27A, 0xFAD4,16},  
	{0xC27C, 0x0400,16},
	{0xC27E, 0x0C00,16},  
	{0xC280, 0xBD80,16},
	{0xC282, 0x1C08,16},  
	{0xC284, 0xE7FC,16},
	{0xC286, 0x0201,16},  
	{0xC288, 0x1200,16},
	{0xC28A, 0x4308,16},  
	{0xC28C, 0x0400,16},
	{0xC28E, 0x0C00,16},  
	{0xC290, 0x46F7,16},
	{0xC292, 0x0000,16},  
	{0xC294, 0x0AE5,16},
	{0xC296, 0x0018,16},  

	{0x485C, 0x0298,16}, 	//AF_DRV_SIZE                                                 
	{0x485E, 0x57E5,16}, 	//AF_DRV_CHKSUM_CODE                                                 
	// AF DRIVER END

	//AF.INI	SEHF_CHOO_START
	{0x482C, 0x0032,16},			// CLIMB DETECT??????L
	{0x4857, 0x0A,8},			// IIR DETECT????
	{0x490C, 0x0200,16},			// LEVEL DEPEND
	{0x490E, 0x0200,16},			// LEVEL DEPEND
	{0x4910, 0x0200,16},			// LEVEL DEPEND
	{0x4924, 0x0010,16},			// LEVEL DEPEND
	{0x4926, 0x0010,16},			// LEVEL DEPEND
	{0x4928, 0x0010,16},			// LEVEL DEPEND
	{0x493C, 0x0200,16},			// LEVEL DEPEND
	{0x493E, 0x0200,16},			// LEVEL DEPEND
	{0x4940, 0x0200,16},			// LEVEL DEPEND
	{0x4954, 0x0010,16},			// LEVEL DEPEND
	{0x4956, 0x0010,16},			// LEVEL DEPEND
	{0x4958, 0x0010,16},			// LEVEL DEPEND
	{0x4822, 0x0002,16},			// MOVING AMOUNT PER WOBBLE STEP
	{0x4824, 0x0008,16},			// MOVING AMOUNT PER CLIMB STEP

	{0x4882, 0x0000,16},	//AF_FOCUS_OFFSET    
	{0x4972, 0x02,8},    	//AF_SECOND_WND_CHK 
	{0x4856, 0x14,8},	//AF_DROPRATE_ON_PEAK_DETECT_FIR       
	             
	{0x4874, 0x0708,16}, 	//AF_DRV_MOVINGTIME     
	{0x4804, 0x00,8},   	//AF_SEARCH_DIR          
	{0x4816, 0x01,8},   	//AF_DIRECTBACK_F        
	{0x4846, 0x01,8},   	//AF_SEARCH_SECOND_DIR   
	{0x4847, 0x01,8},   	//AF_DIRECTBACK_SECOND_F
	{0x4864, 0x0000,16},	//AF_OVERSRCH_AREA_LOW   
	{0x4866, 0x0000,16},	//AF_OVERSRCH_AREA_HIGH  
	{0x486A, 0x00BC,16},   //AF_LENSPOS_ON_AFNG
	  
	{0x4968, 0x00,8},	//STOP DECISION SETTING DURING WOBBLE/CLIMB
	{0x4808, 0x001A,16},   //MINIMUM AF-OPD_TH VALUE FOR IIR
	{0x480A, 0x0096,16},   //MAXIMUM AF-OPD_TH VALUE FOR IIR
	{0x480E, 0x0032,16},   //MINIMUM AF-OPD_TH VALUE FOR FIR
	{0x4810, 0x0032,16},   //MAXIMUM AF-OPD_TH VALUE FOR FIR
	{0x481C, 0x00,8},	//MEAN OFF
	 
	{0x4888, 0x01,8}, // AF_SEND_PARTITION_F
	{0x4889, 0x10,8}, // AF_SENDNUM_ALL
	{0x488A, 0x00,8}, // AF_SENDNUM_UP
	{0x488B, 0x00,8}, // AF_SENDNUM_DOWN
	{0x488C, 0x0008,16},   // AF_SENDAMOUNT_ADDLIMIT
	 
	{0x4860, 0x00BC,16}, 	//AF_ADJ_INF
	{0x4862, 0x02BC,16}, 	//AF_ADJ_MACRO
	{0x4844, 0x0020,16},   //FINE SEARCH RANGE OFFSET
	{0x486C, 0x0020,16},   //AF_DRV_AMOUNT_TONEAR_F
	{0x486E, 0x0008,16},   //AF_DRV_AMOUNT_TONEAR_S
	{0x4870, 0x0020,16},    //AF_DRV_AMOUNT_TOFAR_F
	{0x4872, 0x0008,16},   //AF_DRV_AMOUNT_TOFAR_S
	{0x4876, 0x00BC,16},   //AF_AREA_LOW_TYPE1
	{0x4878, 0x02BC,16},   //AF_AREA_HIGH_TYPE1
	{0x487A, 0x00BC,16},	// TYPE2 AREA LOW P-AF-014
	{0x487C, 0x02BC,16},	// TYPE2 AREA HIGH P-AF-014
	{0x487E, 0x00BC,16},	// TYPE3 AREA LOW
	{0x4880, 0x02BC,16},	// TYPE3 AREA HIGH  		
	//SEHF_CHOO_END

	{0x489A, 0x5EC2,16},   //SCLTH1 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x489C, 0x68F6,16},   //SCLTH2 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x489E, 0x77A6,16},   //SCLTH3 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48A0, 0x0026,16},   //VALUE1 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48A2, 0x0006,16},   //VALUE2 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48A4, 0x001E,16},   //VALUE3 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48A6, 0x5EC2,16},   //SCLTH1 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HST FIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48A8, 0x68F6,16},   //SCLTH2 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HST FIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48AA, 0x77A6,16},   //SCLTH3 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HST FIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48AC, 0x0002,16},   //VALUE1 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48AE, 0x0005,16},   //VALUE2 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HST FIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48B0, 0x0004,16},   //VALUE3 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HST FIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48B2, 0x5EC2,16},   //SCLTH1 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48B4, 0x68F6,16},   //SCLTH2 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48B6, 0x77A6,16},   //SCLTH3 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48B8, 0x0005,16},   //VALUE1 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48BA, 0x0009,16},   //?VALUE2 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48BC, 0x000B,16},   //VALUE3 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48BE, 0x5EC2,16},   //SCLTH1 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48C0, 0x68F6,16},   //SCLTH2 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48C2, 0x77A6,16},   //SCLTH3 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48C4, 0x0014,16},   //VALUE1 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48C6, 0x0014,16},   //?VALUE2 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48C8, 0x0026,16},   //VALUE3 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF FIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48CA, 0x5EC2,16},   //SCLTH1 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48CC, 0x68F6,16},   //SCLTH2 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48CE, 0x77A6,16},   //SCLTH3 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48D0, 0x0026,16},   //VALUE1 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48D2, 0x0006,16},   //VALUE2 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48D4, 0x001E,16},   //VALUE3 SETTING: SPECIFIES THE LUMINANCE SIDE LOW LEVEL THRESHOLD VALUE FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48D6, 0x5EC2,16},   //SCLTH1 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48D8, 0x68F6,16},   //SCLTH2 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48DA, 0x77A6,16},   //SCLTH3 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48DC, 0x0002,16},   //VALUE1 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48DE, 0x0006,16},   //VALUE2 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48E0, 0x0003,16},   //VALUE3 SETTING: SETS THE MEDIUM-LUMINANCE AREA GRADIENT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE. A NEGATIVE OR POSITIVE GRADIENT CAN BE SPECIFIED.
	{0x48E2, 0x5EC2,16},   //SCLTH1 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48E4, 0x68F6,16},   //SCLTH2 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48E6, 0x77A6,16},   //SCLTH3 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48E8, 0x0006,16},   //VALUE1 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48EA, 0x0009,16},   //VALUE2 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48EC, 0x000C,16},   //VALUE3 SETTING: SETS THE MINIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48EE, 0x5EC2,16},   //SCLTH1 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48F0, 0x68F6,16},   //SCLTH2 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48F2, 0x77A6,16},   //SCLTH3 SETTING: SETS THE MAXIMUM CORING AMOUNT FOR HPF IIR CORING PROCESSING LUMINANCE DEPENDENCE.
	{0x48F4, 0x001E,16},   //VALUE1??????: HPF IIR?R?A?????O?????P?X?????????????A?R?A?????O???????L?????????????B
	{0x48F6, 0x0016,16},   //VALUE2??????: HPF IIR?R?A?????O?????P?X?????????????A?R?A?????O???????L?????????????B
	{0x48F8, 0x0026,16},   //VALUE3??????: HPF IIR?R?A?????O?????P?X?????????????A?R?A?????O???????L?????????????B

	//SHD GAIN?A??ON 8/5
	{0x4F25, 0x01,8},   //???X?F????OFF SET 8/5
	{0x59BF, 0x40,8},
	{0x59C0, 0x40,8},
	{0x59C1, 0x40,8},
	{0x59C2, 0x64,8},
	{0x59C3, 0x64,8},
	{0x59C4, 0x64,8},
	{0x59C5, 0x08,8},
	{0x59C6, 0x08,8},
	{0x59C7, 0x08,8}, //CS_SLP_C_C 7/6
	{0x59C8, 0x1E,8},
	{0x59C9, 0x1E,8},
	{0x59CA, 0x1E,8}, //CS_CBHLEV_C 7/6
	{0x59CB, 0x1E,8},
	{0x59CC, 0x1E,8},
	{0x59CD, 0x1E,8}, //CS_CRHLEV_C 7/6
	{0x59CE, 0x20,8},
	{0x59CF, 0x20,8},
	{0x59D0, 0x20,8},
	{0x59D1, 0x20,8},
	{0x59D2, 0x20,8},
	{0x59D3, 0x20,8},
	{0x59D4, 0x0000,16},
	{0x59D6, 0x0000,16},
	{0x59D8, 0x0000,16}, //CS_CBLLEV_C 7/6
	{0x59DA, 0xFFE2,16},
	{0x59DC, 0xFFE2,16},
	{0x59DE, 0xFFE2,16}, //CS_CRLLEV_C 7/6
	{0x59E0, 0xFFE0,16},
	{0x59E2, 0xFFE0,16},
	{0x59E4, 0xFFE0,16},
	{0x59E6, 0xFFE0,16},
	{0x59E8, 0xFFE0,16},
	{0x59EA, 0xFFE0,16},
	{0x59EC, 0xF9,8},
	{0x59ED, 0xF9,8},
	{0x59EE, 0xF9,8},
	{0x59EF, 0xFE,8},
	{0x59F0, 0xFE,8},
	{0x59F1, 0xFE,8},
	{0x59F2, 0xF9,8},
	{0x59F3, 0xF9,8},
	{0x59F4, 0xF9,8},

	// USER_CTRL
	{0x0060, 0x00,8}, // UIBRIGHTNESS[7:0] 
	{0x0061, 0x80,8}, // UICONTRAST[7:0] 
	{0x0062, 0x20,8}, // UISHARPNESS[7:0] 
	{0x0080, 0x00,8},	//EV_SEL  OFF

	// MAX FRAME
	{0x01BD, 0x01,8},   // MAX FRAME TYPE1 NORMAL AE MODE 33MS
	{0x01BE, 0x02,8},   // MAX FRAME TYPE2 LONG AE MODE  66MS FOR 1/2S
	{0x01BF, 0x08,8},   // MAX FRAME TYPE3 LONG NIGHT MODE   250MS FOR 1/2S
	{0x01C0, 0x0F,8},   // MAX FRAME TYPE4 1SEC SHUTTER FOR DEBUG  500MS FOR 1S

	// ISO
	{0x01E5, 0x00,8},   // ISO_TYPE1 - AUTO
	{0x01E6, 0x04,8},   // ISO_TYPE2 - ISO 50
	{0x01E7, 0x0A,8},   // ISO_TYPE3 - ISO 200
	{0x01E8, 0x00,8},   // ISO_TYPE4

	// AE   INCK = 24MHZ, PLL = 432MHZ
	{0x4A00, 0x0561,16},   //MIN SHT START FROM 5LINE @CAPTURE
	{0x4A02, 0x0000,16},   //MIN AGC
	{0x4016, 0x2465,16},   //LONG TIME AE 1ST SHUTTER POINT
	{0x4018, 0x03CA,16},   //LONG TIME AE 1ST GAIN UP POINT   FOR EVREF=2380H
	{0x401A, 0x0549,16},   //LONG TIME AE 2ND SHUTTER POINT
	{0x401C, 0x03FD,16},   //LONG TIME AE 2ND GAIN UP POINT
	{0x401E, 0x025F,16},   //LONG TIME AE MAX SHUTTER POINT
	{0x4020, 0x02FE,16},   //LONG TIME AE MAX GAIN UP POINT 16.2DB

	{0x405C, 0x09B8,16},// CLKSHTMIN_FDON_MACRO_RAW10
	{0x405E, 0x08C6,16},// CLKSHTMIN_FDOFF_MACRO_RAW10

	// EV
	{0x4087, 0x05,8},	// EVSEL_GAIN_P1_3  EV +0.5
	{0x4088, 0x0E,8},	// EVSEL_GAIN_P2_3
	{0x4089, 0x0B,8},	// EVSEL_GAIN_P3_3  EV +1
	{0x408A, 0x0F,8},	// EVSEL_GAIN_P4_3  EV +1.5
	{0x408B, 0x21,8},	// EVSEL_GAIN_P5_3
	{0x408C, 0x13,8},	// EVSEL_GAIN_P6_3  EV +2
	{0x408D, 0x06,8},	// EVSEL_GAIN_M1_3 EV -0.5
	{0x408E, 0x0E,8},	// EVSEL_GAIN_M2_3
	{0x408F, 0x0C,8},	// EVSEL_GAIN_M3_3  EV -1
	{0x4090, 0x13,8},	// EVSEL_GAIN_M4_3  EV -1.5
	{0x4091, 0x21,8},	// EVSEL_GAIN_M5_3
	{0x4092, 0x18,8},	// EVSEL_GAIN_M6_3  EV -2

	// MWB
	{0x4436, 0x0AC1,16},   //USER#0 FIXED COORDINATE R/G MWB_DAYLIGHT
	{0x4438, 0x1C01,16},   //USER#0 FIXED COORDINATE B/G
	{0x443A, 0x09A0,16},   //USER#1 FIXED COORDINATE R/G MWB_SHADE XXX
	{0x443C, 0x21FC,16},   //USER#1 FIXED COORDINATE B/G 
	{0x443E, 0x088D,16},   //USER#2 FIXED COORDINATE R/G MWB_CLOUDY 
	{0x4440, 0x20D0,16},   //USER#2 FIXED COORDINATE B/G          
	{0x4442, 0x0CE4,16},   //USER#3 FIXED COORDINATE R/G MWB_FLUOR      
	{0x4444, 0x12AE,16},   //USER#3 FIXED COORDINATE B/G            
	{0x4446, 0x112E,16},   //USER#4 FIXED COORDINATE R/G MWB_INCAND  
	{0x4448, 0x0F39,16},   //USER#4 FIXED COORDINATE B/G       

	{0x0100, 0x00,8},   //SCENE SELECTION
	// NORMAL
	{0x0102, 0x20,8},    // AWB_SN1 :  
	{0x0103, 0x01,8},    // AE_SN1 : 
	{0x0104, 0x4D,8},    // AE_SUB_SN1 : 
	{0x0105, 0x40,8},    // PICT1_SN1 : 
	{0x0106, 0x00,8},    // RESERVE_0106 : 
	// PORTRAIT
	{0x0107, 0x20,8},    // AWB_SN2 : 
	{0x0108, 0x01,8},    // AE_SN2 : 
	{0x0109, 0x4D,8},    // AE_SUB_SN2 : 
	{0x010A, 0x40,8},    // PICT1_SN2 : 
	{0x010B, 0x00,8},    // RESERVE_010B : 
	// LANDSCAPE
	{0x010C, 0x20,8},    // AWB_SN3 : 
	{0x010D, 0x01,8},    // AE_SN3 : 
	{0x010E, 0x4F,8},    // AE_SUB_SN3 : MULTI
	{0x010F, 0x54,8},    // PICT1_SN3 : 
	{0x0110, 0x00,8},    // RESERVE_0110 : 
	// SPORT
	{0x0111, 0x20,8},    // AWB_SN4 : 
	{0x0112, 0x00,8},    // AE_SN4 : NORMAL AE
	{0x0113, 0x0D,8},    // AE_SUB_SN4 :  33MS
	{0x0114, 0x40,8},    // PICT1_SN4 : 
	{0x0115, 0x00,8},    // RESERVE_0115 : 
	//PARTY/INDOOR
	{0x0116, 0x20,8},    // AWB_SN5 : 
	{0x0117, 0x41,8},    // AE_SN5 : - ISO 200
	{0x0118, 0x4D,8},    // AE_SUB_SN5 : 
	{0x0119, 0x54,8},    // PICT1_SN5 : 
	{0x011A, 0x00,8},    // RESERVE_011A : 
	// BEACH/SNOW
	{0x011B, 0x20,8},    // AWB_SN6 : 
	{0x011C, 0x21,8},    // AE_SN6 :  - ISO 50
	{0x011D, 0x4D,8},    // AE_SUB_SN6 : 
	{0x011E, 0x54,8},    // PICT1_SN6 : 
	{0x011F, 0x00,8},    // RESERVE_011F : 
	// SUNSET
	{0x0120, 0x24,8},    // AWB_SN7 : DAYLIGHT
	{0x0121, 0x01,8},    // AE_SN7 : 
	{0x0122, 0x4D,8},    // AE_SUB_SN7 : 
	{0x0123, 0x40,8},    // PICT1_SN7 : 
	{0x0124, 0x00,8},    // RESERVE_0124 : 
	// DAWN
	{0x0125, 0x27,8},    // AWB_SN8 : CWF
	{0x0126, 0x01,8},    // AE_SN8 : 
	{0x0127, 0x4D,8},    // AE_SUB_SN8 : 
	{0x0128, 0x40,8},    // PICT1_SN8 : 
	{0x0129, 0x00,8},    // RESERVE_0129 : 
	// FALL
	{0x012A, 0x20,8},    // AWB_SN9 : 
	{0x012B, 0x01,8},    // AE_SN9 : 
	{0x012C, 0x4D,8},    // AE_SUB_SN9 : 
	{0x012D, 0x68,8},    // PICT1_SN9 : 
	{0x012E, 0x00,8},    // RESERVE_012E : 
	// FIRE/NIGHTSHOT
	{0x012F, 0x20,8},    // AWB_SN10 : 
	{0x0130, 0x01,8},    // AE_SN10 : 
	{0x0131, 0x4D,8},    // AE_SUB_SN10 : 
	{0x0132, 0x40,8},    // PICT1_SN10 : 
	{0x0133, 0x00,8},    // RESERVE_0133 : 
	// TEXT /CANDLE/BACKLIGHT
	{0x0134, 0x20,8},    // AWB_SN11 : 
	{0x0135, 0x01,8},    // AE_SN11 : 
	{0x0136, 0x4D,8},    // AE_SUB_SN11 : 
	{0x0137, 0x40,8},    // PICT1_SN11 : 
	{0x0138, 0x00,8},    // RESERVE_0138 : 
	// MOVIE
	{0x0139, 0x20,8},    // AWB_SN12 : 
	{0x013A, 0x01,8},    // AE_SN12 : 
	{0x013B, 0x03,8},    // AE_SUB_SN12 : 
	{0x013C, 0x40,8},    // PICT1_SN12 : 
	{0x013D, 0x00,8},    // RESERVE_013D : 

	{0x01AE, 0x00,8},    // UIHUE_TYPE1 : 
	{0x01AF, 0xF5,8},    // UIHUE_TYPE2 :  //  1 - SCENE_LANDSCAPE
	{0x01B0, 0xF5,8},    // UIHUE_TYPE3 :  //  2 - SCENE_FALL

	{0x01B1, 0x80,8},    // UISATURATION_TYPE1 : 
	{0x01B2, 0x9F,8},    // UISATURATION_TYPE2 :  // 1 - SCENE_LANDSCAPE
	{0x01B3, 0xA3,8},    // UISATURATION_TYPE3 :  // 2 - SCENE_FALL

	{0x01D3, 0x00,8},   //AF_SN1
	{0x01D4, 0x00,8},   //AF_SN2
	{0x01D5, 0x00,8},   //AF_SN3
	{0x01D6, 0x00,8},   //AF_SN4
	{0x01D7, 0x00,8},   //AF_SN5
	{0x01D8, 0x00,8},   //AF_SN6
	{0x01D9, 0x00,8},   //AF_SN7
	{0x01DA, 0x00,8},   //AF_SN8
	{0x01DB, 0x00,8},   //AF_SN9
	{0x01DC, 0x00,8},   //AF_SN10
	{0x01DD, 0x00,8},   //AF_SN11
	{0x01DE, 0x00,8},   //AF_SN12
	{0x01F8, 0x55,8},   //SCENE1-4 ?? SETTING FOR CAPTURE. 
	{0x01F9, 0x55,8},   //SCENE5-8 ?? SETTING FOR CAPTURE.
	{0x01FA, 0x55,8},   //SCENE9-12 ?? SETTING FOR CAPTURE. 
	{0x01FB, 0x55,8},   //SCENE1-4 ?? SETTING FOR MOVIE. 
	{0x01FC, 0x55,8},   //SCENE5-8 ?? SETTING FOR MOVIE. 
	{0x01FD, 0x55,8},   //SCENE9-12 ?? SETTING FOR MOVIE. 
	{0x02D7, 0x11,8},   //LM MODE SELECTION SCENE 1&2 0:AUTO  1 TO 7 POS1 TO 7
	{0x02D8, 0x11,8},   //LM MODE SELECTION SCENE 3&4 0:AUTO  1 TO 7 POS1 TO 7
	{0x02D9, 0x11,8},   //LM MODE SELECTION SCENE 5&6 0:AUTO  1 TO 7 POS1 TO 7
	{0x02DA, 0x01,8},   //LM MODE SELECTION SCENE 7&8 0:AUTO  1 TO 7 POS1 TO 7
	{0x02DB, 0x00,8},   //LM MODE SELECTION SCENE 9&10 0:AUTO  1 TO 7 POS1 TO 7
	{0x02DC, 0x10,8},   //LM MODE SELECTION SCENE 11&12 0:AUTO  1 TO 7 POS1 TO 7

	{0x4E5D, 0x00,8},   
	{0x4E5E, 0x0320,16},
	{0x4E60, 0x7640,16}, 
	{0x4E62, 0x1CE7,16},   
	{0x4E60, 0x7640,16},   
	{0x4E64, 0xF3DB,16},   
	{0x4E66, 0xDAF6,16},
	{0x4E68, 0xF325,16},   
	{0x4E6A, 0x2516,16},
	{0x4E6C, 0xDAD6,16},   
	{0x4E70, 0x7640,16},   
	{0x4E74, 0xF3DB,16},   
	{0x4E78, 0xF325,16},   
	{0x4E7C, 0xDAD6,16},   
	{0x4E62, 0x1CE7,16},
	{0x4E66, 0xDAF6,16},
	{0x4E6A, 0x2516,16},
	{0x4E6E, 0xDAD6,16},
	{0x4E72, 0x1CE7,16},
	{0x4E76, 0xDAF6,16},
	{0x4E7A, 0x2516,16},
	{0x4E7E, 0xDAD6,16},

	{0x4F24, 0x3E,8},   //MAIN_CONFIG1
	{0x4F25, 0x01,8},   //MAIN_CONFIG2

	//LMT	POS2-POS7}??POS1???W????????(090810)
	{0x5000, 0xFFCC,16},   //GRG LINEAR MATRIX COEFFICIENT POS1
	{0x5002, 0xFFFC,16},   //GRB LINEAR MATRIX COEFFICIENT POS1
	{0x5004, 0xFFF0,16},   //GGR LINEAR MATRIX COEFFICIENT POS1
	{0x5006, 0xFFF0,16},   //GGB LINEAR MATRIX COEFFICIENT POS1
	{0x5008, 0xFFFB,16},   //GBR LINEAR MATRIX COEFFICIENT POS1
	{0x500A, 0xFFCF,16},   //GBG LINEAR MATRIX COEFFICIENT POS1

	{0x5054, 0x31,8},   //POS1 R/G COORDINATE SETTING
	{0x5055, 0x1E,8},   //POS1 B/G COORDINATE SETTING
	{0x5056, 0x39,8},   //POS2 R/G COORDINATE SETTING
	{0x5057, 0x19,8},   //POS2 B/G COORDINATE SETTING
	{0x5058, 0x42,8},   //POS3 R/G COORDINATE SETTING
	{0x5059, 0x15,8},   //POS3 B/G COORDINATE SETTING
	{0x505A, 0x60,8},   //POS4 R/G COORDINATE SETTING
	{0x505B, 0x0F,8},   //POS4 B/G COORDINATE SETTING
	{0x505C, 0x95,8},   //POS5 R/G COORDINATE SETTING
	{0x505D, 0x0C,8},   //POS5 B/G COORDINATE SETTING
	{0x505E, 0x3D,8},   //POS6 R/G COORDINATE SETTING
	{0x505F, 0x17,8},   //POS6 B/G COORDINATE SETTING
	{0x5060, 0x4E,8},   //POS7 R/G COORDINATE SETTING
	{0x5061, 0x12,8},   //POS7 B/G COORDINATE SETTING
	{0x5092, 0xFFFA,16},   //BYHUE REFERENCE VALUE (1ST QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x5094, 0xFFEE,16},   //RYHUE REFERENCE VALUE (1ST QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x5096, 0xFFFA,16},   //BYHUE REFERENCE VALUE (2ND QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x5098, 0xFFEE,16},   //RYHUE REFERENCE VALUE (2ND QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x509A, 0xFFFA,16},   //BYHUE REFERENCE VALUE (3RD QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x509C, 0xFFEE,16},   //RYHUE REFERENCE VALUE (3RD QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x509E, 0x0000,16},   //BYHUE REFERENCE VALUE (4TH QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A0, 0xFFEE,16},   //RYHUE REFERENCE VALUE (4TH QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A2, 0x5C,8},   //RYGAIN REFERENCE VALUE (1ST QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A3, 0x50,8},   //BYGAIN REFERENCE VALUE (1ST QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A4, 0x5C,8},   //RYGAIN REFERENCE VALUE (2ND QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A5, 0x50,8},   //BYGAIN REFERENCE VALUE (2ND QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A6, 0x5C,8},   //RYGAIN REFERENCE VALUE (3RD QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A7, 0x47,8},   //BYGAIN REFERENCE VALUE (3RD QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A8, 0x5C,8},   //RYGAIN REFERENCE VALUE (4TH QUADRANT), FOR AUTOMATIC CONTROL, POS1
	{0x50A9, 0x61,8},   //6DBYGAIN REFERENCE VALUE (4TH QUADRANT), FOR AUTOMATIC CONTROL, POS1                           

	{0x50C2, 0x12,8},   //HUEGAIN POS1 COORDINATE FOR AUTOMATIC CONTROL (HIGH COLOR TEMPERATURE SIDE?J X256 IS THE COORDINATE.
	{0x50C3, 0x10,8},   //HUEGAIN POS2 COORDINATE FOR AUTOMATIC CONTROL (LOW COLOR TEMPERATURE SIDE?J X256 IS THE COORDINATE.
	{0x01B1, 0x80,8},   //COLOR DEPTH ADJUSTMENT VALUE [US1.7] (RANGE: 0 TO 2X(FFH))

	//PICT
	{0x4E0B, 0x01,8},	//HUE GAIN MODE SELECTION 0: AUTO (COLORING CONTROL) 1: FIXED AT SETTING VALUE FOR POS1 2: FIXED AT SETTING VALUE FOR POS2
	{0x4E0C, 0x0320,16},	//9 CHROMA SUPPRESS STARTGAIN 8DB                                                                                                  
	{0x4E0E, 0x09C4,16},	//15.6 CHROMA SUPPRESS ENDGAIN 18DB                                                                                                 
	{0x4E10, 0x00D0,16},	//85% , 147A CHRAMA SUPPRESS ENDLEVEL 85%(OFF) 7/6   
	{0x4E16, 0x00,8},   	//LINEAR MATRIX/APCON SELECTION
	{0x4E17, 0x84,8},   	//CXC MIXING RATIO SETTING                                                    
	{0x59BD, 0x40,8},	//LMT_WEIGHT A U1.8 7/20 LEE   
	{0x59BE, 0x80,8},	//LMT_WEIGHT B      7/20 LEE   
	{0x02D7, 0x11,8}, 	// MATRIX TYPE1    

	//PICT_SHD
	{0x4E40, 0x154A,16},   //HIGH THRESHOLD FOR R2 AT THE B/G COORDINATES AND HIGH COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E42, 0x14B4,16},   //LOW THRESHOLD FOR R2 AT THE B/G COORDINATES AND HIGH COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E44, 0x114E,16},   //HIGH THRESHOLD FOR R2 AT THE B/G COORDINATES AND LOW COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E46, 0x1004,16},   //LOW THRESHOLD FOR R2 AT THE B/G COORDINATES AND LOW COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E48, 0x1300,16},   //R/G COORDINATES HIGH THRESHOLD VALUE FOR SHADING COLOR CORRECTION FOR R2.
	{0x4E4A, 0x1280,16},   //R/G COORDINATES LOW THRESHOLD VALUE FOR SHADING COLOR CORRECTION FOR R2.
	{0x4E4C, 0x6683,16},   //HIGH LUMINANCE SIDE THRESHOLD VALUE FOR SHADING GAIN CORRECTION.
	{0x4E4E, 0x5AB4,16},   //LOW LUMINANCE SIDE THRESHOLD VALUE FOR SHADING GAIN CORRECTION.   
	{0x4E50, 0x0FA0,16},   //HIGH THRESHOLD FOR RB AT THE B/G COORDINATES AND HIGH COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E52, 0x0E74,16},   //LOW THRESHOLD FOR RB AT THE B/G COORDINATES AND HIGH COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E54, 0x0A28,16},   //HIGH THRESHOLD FOR RB AT THE B/G COORDINATES AND LOW COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E56, 0x08FC,16},   //LOW THRESHOLD FOR RB AT THE B/G COORDINATES AND LOW COLOR TEMPERATURE SIDE FOR SHADING COLOR CORRECTION.
	{0x4E58, 0x1B58,16},   //HIGH THRESHOLD FOR RB AT THE R/G COORDINATES FOR SHADING COLOR CORRECTION.
	{0x4E5A, 0x1A2C,16},   //LOW THRESHOLD FOR RB AT THE R/G COORDINATES FOR SHADING COLOR CORRECTION.

	//PICT_GAIN
	{0x5841, 0x07,8},   //LMT_GAIN BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5842, 0x00,8},   //CS_YLCOEF BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1:QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5843, 0x00,8},   //CS_YHCOEF BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5844, 0x00,8},   //CS_SLP_C BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5845, 0x00,8},   //CS_CBHLEV BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5846, 0x00,8},   //CS_CRHLEV BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5847, 0x00,8},   //CS_CBHLEV_AP BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5848, 0x00,8},   //CS_CRHLEV_AP BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5849, 0x00,8},   //CS_CBLLEV BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x584A, 0x00,8},   //CS_CRLLEV BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x584B, 0x00,8},   //CS_CBLLEV_AP BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x584C, 0x00,8},   //CS_CRLLEV_AP BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x584D, 0x00,8},   //CS_APSH_H BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x584E, 0x00,8},   //CS_APSH_M BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x584F, 0x00,8},   //CS_APSH_L BIT 2-0 : BRIGHTNESS-LINKED THRESHOLD VALUE PATTERN TABLE TYPE SELECTION (0 TO 7)  BIT 4-3 : THRESHOLD INTERPOLATION METHOD SELECTION (0: LINEAR, 1: QUADRATIC CURVE, 2: LOGARITHMIC CURVE)
	{0x5855, 0x01,8},   //GAIN-LINKED MODE TARGET SETTING 0: AGC GAIN COORDINATION (INCLUDING SHUTTER INTERPOLATION), 1: AGC GAIN INTERPOLATION (NOT INCLUDING GAIN INTERPOLATION), 2: AE SCALE COORDINATION, 3: REFERENCE SCALE COORDINATION, 4: DARK LEVEL COORDINATION
	{0x5856, 0x01,8},   //GAIN-LINKED MODE TARGET SETTING 0: AGC GAIN COORDINATION (INCLUDING SHUTTER INTERPOLATION), 1: AGC GAIN INTERPOLATION (NOT INCLUDING GAIN INTERPOLATION), 2: AE SCALE COORDINATION, 3: REFERENCE SCALE COORDINATION, 4: DARK LEVEL COORDINATION
	{0x5857, 0x01,8},   //GAIN-LINKED MODE TARGET SETTING 0: AGC GAIN COORDINATION (INCLUDING SHUTTER INTERPOLATION), 1: AGC GAIN INTERPOLATION (NOT INCLUDING GAIN INTERPOLATION), 2: AE SCALE COORDINATION, 3: REFERENCE SCALE COORDINATION, 4: DARK LEVEL COORDINATION
	{0x5876, 0x0F00,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  LOW BRIGHTNESS/HIGH GAIN SIDE
	{0x5878, 0x0F00,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  MEDIUM BRIGHTNESS/MEDIUM GAIN SIDE        
	{0x587A, 0x0F00,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  HIGIH BRIGHTNESS/LOW GAIN SIDE  
	{0x587C, 0x0F00,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  LOW BRIGHTNESS/HIGH GAIN SIDE
	{0x587E, 0x0F00,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  MEDIUM BRIGHTNESS/MEDIUM GAIN SIDE        
	{0x5880, 0x0F00,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  HIGIH BRIGHTNESS/LOW GAIN SIDE  
	{0x5882, 0x0A91,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  LOW BRIGHTNESS/HIGH GAIN SIDE
	{0x5884, 0x0A92,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  MEDIUM BRIGHTNESS/MEDIUM GAIN SIDE        
	{0x5886, 0x0AC2,16}, //GAIN COORDINATION THRESHOLD VALUE SETTING  HIGIH BRIGHTNESS/LOW GAIN SIDE  
	{0x59BF, 0x40,8},   //LOW-LUMINANCE LEVEL THRESHOLD VALUE OF THRESHOLD VALUE A DO NOT SET A VALUE HIGHER THAN THE HIGH-L
	{0x59C0, 0x40,8},   //LOW-LUMINANCE LEVEL THRESHOLD VALUE OF THRESHOLD VALUE B DO NOT SET A VALUE HIGHER THAN THE HIGH-L
	{0x59C1, 0x40,8},   //LOW-LUMINANCE LEVEL THRESHOLD VALUE OF THRESHOLD VALUE C DO NOT SET A VALUE HIGHER THAN THE HIGH-L
	{0x59C2, 0x64,8},   //HIGH-LUMINANCE LEVEL THRESHOLD VALUE OF THRESHOLD VALUE A DO NOT SET A VALUE LOWER THAN THE LOW-LU
	{0x59C3, 0x64,8},   //HIGH-LUMINANCE LEVEL THRESHOLD VALUE OF THRESHOLD VALUE B DO NOT SET A VALUE LOWER THAN THE LOW-LU
	{0x59C4, 0x64,8},   //HIGH-LUMINANCE LEVEL THRESHOLD VALUE OF THRESHOLD VALUE C DO NOT SET A VALUE LOWER THAN THE LOW-LU
	{0x59C5, 0x28,8}, //SATURATION SUPPRESS GAIN INCLINATION SELECTION OF THRESHOLD VALUE A, SET IN THE N/512 (WHERE N =  
	{0x59C6, 0x28,8}, //SATURATION SUPPRESSION INCLINATION SELECTION OF THRESHOLD VALUE B, SET IN THE N/512 (WHERE N = 0  
	{0x59C7, 0x28,8}, //SATURATION SUPPRESSION GAIN INCLINATION SELECTION OF THRESHOLD VALUE C, SET IN THE N/512 (WHERE N 
	{0x59C8, 0x0C,8}, //THRESHOLD VALUE A, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CB
	{0x59C9, 0x0C,8}, //THRESHOLD VALUE B, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CB
	{0x59CA, 0x0C,8}, //THRESHOLD VALUE C, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CB
	{0x59CB, 0x0C,8}, //THRESHOLD VALUE A, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CR
	{0x59CC, 0x0C,8}, //THRESHOLD VALUE B, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CR
	{0x59CD, 0x0C,8}, //THRESHOLD VALUE C, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CR
	{0x59CE, 0x0C,8}, //THRESHOLD VALUE A, FOR APCON SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CB     
	{0x59CF, 0x0C,8}, //THRESHOLD VALUE B, FOR APCON SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CB     
	{0x59D0, 0x0C,8}, //THRESHOLD VALUE C, FOR APCON SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CB     
	{0x59D1, 0x0C,8}, //THRESHOLD VALUE A, FOR APCON SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CR     
	{0x59D2, 0x0C,8}, //THRESHOLD VALUE B, FOR APCON SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CR     
	{0x59D3, 0x0C,8}, //THRESHOLD VALUE C, FOR APCON SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, H CR     
	{0x59D4, 0xFFF0,16}, //THRESHOLD VALUE A, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CB
	{0x59D6, 0xFFF0,16}, //THRESHOLD VALUE B, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CB
	{0x59D8, 0xFFF0,16}, //THRESHOLD VALUE C, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CB
	{0x59DA, 0xFFF0,16}, //THRESHOLD VALUE A, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CR
	{0x59DC, 0xFFF0,16}, //THRESHOLD VALUE B, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CR
	{0x59DE, 0xFFF0,16}, //THRESHOLD VALUE C, FOR SATURATION SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CR
	{0x59E0, 0xFFF0,16}, //THRESHOLD VALUE A, FOR APCOM SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CB     
	{0x59E2, 0xFFF0,16}, //THRESHOLD VALUE B, FOR APCOM SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CB     
	{0x59E4, 0xFFF0,16}, //THRESHOLD VALUE C, FOR APCOM SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CB     
	{0x59E6, 0xFFF0,16}, //THRESHOLD VALUE A, FOR APCOM SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CR     
	{0x59E8, 0xFFF0,16}, //THRESHOLD VALUE B, FOR APCON SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CR     
	{0x59EA, 0xFFF0,16}, //THRESHOLD VALUE C, FOR APCOM SUPPRES{0x LOW-SATURATION LEVEL IDENTIFICATION, L CR     
	{0x59EC, 0xF9,8},   //THRESHOLD VALUE A, APCON DATA SHIFT REGISTER H (CAN BE SET FROM -7 TO 3)
	{0x59ED, 0xF9,8},   //THRESHOLD VALUE B, APCON DATA SHIFT REGISTER H (CAN BE SET FROM -7 TO 3)
	{0x59EE, 0xF9,8},   //THRESHOLD VALUE C, APCON DATA SHIFT REGISTER H (CAN BE SET FROM -7 TO 3)
	{0x59EF, 0xFE,8},   //THRESHOLD VALUE A, APCON DATA SHIFT REGISTER M (CAN BE SET FROM -7 TO 3)
	{0x59F0, 0xFE,8},   //THRESHOLD VALUE B, APCON DATA SHIFT REGISTER M (CAN BE SET FROM -7 TO 3)
	{0x59F1, 0xFE,8},   //THRESHOLD VALUE C, APCON DATA SHIFT REGISTER M (CAN BE SET FROM -7 TO 3)
	{0x59F2, 0xF9,8},   //THRESHOLD VALUE A, APCON DATA SHIFT REGISTER L (CAN BE SET FROM -7 TO 3)
	{0x59F3, 0xF9,8},   //THRESHOLD VALUE B, APCON DATA SHIFT REGISTER L (CAN BE SET FROM -7 TO 3)
	{0x59F4, 0xF9,8},   //THRESHOLD VALUE C, APCON DATA SHIFT REGISTER L (CAN BE SET FROM -7 TO 3)
                                                                                                               

	//CAT_ITP.INI
	{0x5826, 0x08,8},   //
	{0x5827, 0x00,8},   //
	{0x5828, 0x00,8},   //
	{0x5829, 0x08,8},   //    

	{0x4F2C, 0x0008,16},    // IR_DS_ADGRADCORE                         =        8 // A     
	{0x4F2E, 0x0000,16},    // IR_DS_ADGRADCORE                         =        0 // B     
	{0x4F30, 0x0000,16},    // IR_DS_ADGRADCORE                         =        0 // C     
	{0x4F32, 0x0008,16},    // IR_DS_HVGRADCORE                         =        8 // A     
	{0x4F34, 0x0000,16},    // IR_DS_HVGRADCORE                         =        0 // B     
	{0x4F36, 0x0000,16},    // IR_DS_HVGRADCORE                         =        0 // C     
	{0x4F38, 0x0B,8},    // IR_DS_ADGRADLIM                          =       10 // A     
	{0x4F39, 0x16,8},    // IR_DS_ADGRADLIM                          =       20 // B     
	{0x4F3A, 0x1F,8},    // IR_DS_ADGRADLIM                          =       20 // C     
	{0x4F3B, 0x15,8},    // IR_DS_HVGRADLIM                          =       20 // A     
	{0x4F3C, 0x1E,8},    // IR_DS_HVGRADLIM                          =       30 // B     
	{0x4F3D, 0x1F,8},    // IR_DS_HVGRADLIM                          =       30 // C     
	{0x4F3E, 0x0A,8},    // IR_DS_DFCT_TH                            =       10 // A     
	{0x4F3F, 0x1E,8},    // IR_DS_DFCT_TH                            =       30 // B     
	{0x4F40, 0x1E,8},    // IR_DS_DFCT_TH                            =       30 // C     
	{0x4F41, 0x01,8},    // PICT___DS_SLSEL                          =        1          
	{0x4F42, 0x03,8},    // PICT___AP_LD_Y_SEL                       =        3          
	{0x4F48, 0x00,8},    // IR_CV_SEL_Y                              =        0 // A     
	{0x4F49, 0x00,8},    // IR_CV_SEL_Y                              =        0 // B     
	{0x4F4A, 0x00,8},    // IR_CV_SEL_Y                              =        0 // C     
	{0x5858, 0x0A2A,16},    // PICT_GAIN___GAIN_TH_A_TYPE0              =     2602          
	{0x585A, 0x0339,16},    // PICT_GAIN___GAIN_TH_B_TYPE0              =      918          
	{0x585C, 0x00B1,16},    // PICT_GAIN___GAIN_TH_C_TYPE0              =        0        
	{0x5888, 0x0024,16},    // IR_YN_LUMTH                              =       32 // A     
	{0x588A, 0x000B,16},    // IR_YN_LUMTH                              =       11 // B     
	{0x588C, 0x0004,16},    // IR_YN_LUMTH                              =       10 // C     
	{0x588E, 0x0020,16},    // IR_LN_CTH                                =       32 // A     
	{0x5890, 0x0019,16},    // IR_LN_CTH                                =       20 // B     
	{0x5892, 0x0001,16},    // IR_LN_CTH                                =        0 // C    
	{0x5894, 0x0004,16},    // IR_AP_GC_L_POS_LOW                       =        2 // A     
	{0x5896, 0x0002,16},    // IR_AP_GC_L_POS_LOW                       =        2 // B     
	{0x5898, 0x0003,16},    // IR_AP_GC_L_POS_LOW                       =        2 // C     
	{0x589A, 0x0008,16},    // IR_AP_GC_L_POS_LOWSLOPEX                 =        6 // A     
	{0x589C, 0x0012,16},    // IR_AP_GC_L_POS_LOWSLOPEX                 =        6 // B     
	{0x589E, 0x0013,16},    // IR_AP_GC_L_POS_LOWSLOPEX                 =        6 // C     
	{0x58A6, 0x0004,16},    // IR_AP_GC_L_NEG_LOW                       =        2 // A     
	{0x58A8, 0x0002,16},    // IR_AP_GC_L_NEG_LOW                       =        2 // B     
	{0x58AA, 0x0003,16},    // IR_AP_GC_L_NEG_LOW                       =        2 // C     
	{0x58AC, 0x0008,16},    // IR_AP_GC_L_NEG_LOWSLOPEX                 =        6 // A     
	{0x58AE, 0x0012,16},    // IR_AP_GC_L_NEG_LOWSLOPEX                 =        6 // B     
	{0x58B0, 0x0013,16},    // IR_AP_GC_L_NEG_LOWSLOPEX                 =        6 // C     
	{0x58B8, 0x0001,16},    // IR_AP_GC_M_POS_LOW                       =        1 // A     
	{0x58BA, 0x0001,16},    // IR_AP_GC_M_POS_LOW                       =        1 // B     
	{0x58BC, 0x0002,16},    // IR_AP_GC_M_POS_LOW                       =        0 // C     
	{0x58BE, 0x0004,16},    // IR_AP_GC_M_POS_LOWSLOPEX                 =        4 // A     
	{0x58C0, 0x0005,16},    // IR_AP_GC_M_POS_LOWSLOPEX                 =        5 // B     
	{0x58C2, 0x000B,16},    // IR_AP_GC_M_POS_LOWSLOPEX                 =        4 // C     
	{0x58CA, 0x0001,16},    // IR_AP_GC_M_NEG_LOW                       =        1 // A     
	{0x58CC, 0x0001,16},    // IR_AP_GC_M_NEG_LOW                       =        1 // B     
	{0x58CE, 0x0002,16},    // IR_AP_GC_M_NEG_LOW                       =        0 // C     
	{0x58D0, 0x0004,16},    // IR_AP_GC_M_NEG_LOWSLOPEX                 =        4 // A     
	{0x58D2, 0x0005,16},    // IR_AP_GC_M_NEG_LOWSLOPEX                 =        5 // B     
	{0x58D4, 0x0007,16},    // IR_AP_GC_M_NEG_LOWSLOPEX                 =        4 // C     
	{0x58DC, 0x000A,16},    // IR_AP_GC_H_POS_LOW                       =       10 // A     
	{0x58DE, 0x0004,16},    // IR_AP_GC_H_POS_LOW                       =        4 // B     
	{0x58E0, 0x0004,16},    // IR_AP_GC_H_POS_LOW                       =        4 // C     
	{0x58E2, 0x000E,16},    // IR_AP_GC_H_POS_LOWSLOPEX                 =       14 // A     
	{0x58E4, 0x000C,16},    // IR_AP_GC_H_POS_LOWSLOPEX                 =       12 // B     
	{0x58E6, 0x000A,16},    // IR_AP_GC_H_POS_LOWSLOPEX                 =       10 // C     
	{0x58EE, 0x000A,16},    // IR_AP_GC_H_NEG_LOW                       =       10 // A     
	{0x58F0, 0x0004,16},    // IR_AP_GC_H_NEG_LOW                       =        4 // B     
	{0x58F2, 0x0003,16},    // IR_AP_GC_H_NEG_LOW                       =        5 // C     
	{0x58F4, 0x000E,16},    // IR_AP_GC_H_NEG_LOWSLOPEX                 =       14 // A     
	{0x58F6, 0x000C,16},    // IR_AP_GC_H_NEG_LOWSLOPEX                 =       12 // B     
	{0x58F8, 0x000E,16},    // IR_AP_GC_H_NEG_LOWSLOPEX                 =       13 // C     
	{0x5900, 0x0000,16},    // IR_AP_LD_L_TH_LOW                        =        0 // A     
	{0x5902, 0x0000,16},    // IR_AP_LD_L_TH_LOW                        =        0 // B     
	{0x5904, 0x0000,16},    // IR_AP_LD_L_TH_LOW                        =        0 // C     
	{0x5906, 0x015E,16},    // IR_AP_LD_L_TH_HIGH_0                     =      350 // A     
	{0x5908, 0x015E,16},    // IR_AP_LD_L_TH_HIGH_0                     =      350 // B     
	{0x590A, 0x015D,16},    // IR_AP_LD_L_TH_HIGH_0                     =      350 // C     
	{0x590C, 0x0226,16},    // IR_AP_LD_L_TH_HIGH_1_X                   =      550 // A     
	{0x590E, 0x0226,16},    // IR_AP_LD_L_TH_HIGH_1_X                   =      550 // B     
	{0x5910, 0x0225,16},    // IR_AP_LD_L_TH_HIGH_1_X                   =      550 // C     
	{0x591E, 0x0003,16},    // IR_AP_LD_L_CLIP_HIGH_Y                   =        3 // A     
	{0x5920, 0x0004,16},    // IR_AP_LD_L_CLIP_HIGH_Y                   =        4 // B     
	{0x5922, 0x0003,16},    // IR_AP_LD_L_CLIP_HIGH_Y                   =        4 // C     
	{0x5924, 0x0000,16},    // IR_AP_LD_M_TH_LOW                        =        0 // A      
	{0x5926, 0x0082,16},    // IR_AP_LD_M_TH_LOW                        =      130 // B     
	{0x5928, 0x0001,16},    // IR_AP_LD_M_TH_LOW                        =        0 // C     
	{0x592A, 0x0320,16},    // IR_AP_LD_M_TH_HIGH_0                     =      800 // A     
	{0x592C, 0x0320,16},    // IR_AP_LD_M_TH_HIGH_0                     =      800 // B     
	{0x592E, 0x031E,16},    // IR_AP_LD_M_TH_HIGH_0                     =      800 // C     
	{0x5930, 0x0384,16},    // IR_AP_LD_M_TH_HIGH_1_X                   =      900 // A     
	{0x5932, 0x0384,16},    // IR_AP_LD_M_TH_HIGH_1_X                   =      900 // B     
	{0x5934, 0x0383,16},    // IR_AP_LD_M_TH_HIGH_1_X                   =      900 // C     
	{0x5942, 0x0003,16},    // IR_AP_LD_M_CLIP_HIGH_Y                   =        3 // A     
	{0x5944, 0x0004,16},    // IR_AP_LD_M_CLIP_HIGH_Y                   =        4 // B     
	{0x5946, 0x0003,16},    // IR_AP_LD_M_CLIP_HIGH_Y                   =        4 // C     
	{0x5948, 0x0000,16},    // IR_AP_LD_H_TH_LOW                        =        0 // A     
	{0x594A, 0x0023,16},    // IR_AP_LD_H_TH_LOW                        =       35 // B     
	{0x594C, 0x0000,16},    // IR_AP_LD_H_TH_LOW                        =        0 // C    
	{0x594E, 0x0032,16},    // IR_AP_LD_H_TH_HIGH_0                     =       50 // A     
	{0x5950, 0x0023,16},    // IR_AP_LD_H_TH_HIGH_0                     =       35 // B     
	{0x5952, 0x0037,16},    // IR_AP_LD_H_TH_HIGH_0                     =       50 // C     
	{0x5954, 0x00C8,16},    // IR_AP_LD_H_TH_HIGH_1_X                   =      200 // A     
	{0x5956, 0x005D,16},    // IR_AP_LD_H_TH_HIGH_1_X                   =       93 // B     
	{0x5958, 0x00CA,16},    // IR_AP_LD_H_TH_HIGH_1_X                   =      200 // C     
	{0x5966, 0x0001,16},    // IR_AP_LD_H_CLIP_HIGH_Y                   =        1 // A     
	{0x5968, 0x0000,16},    // IR_AP_LD_H_CLIP_HIGH_Y                   =        0 // B     
	{0x596A, 0x0000,16},    // IR_AP_LD_H_CLIP_HIGH_Y                   =        0 // C     
	{0x596C, 0x41,8},    // IR_AP_PRE_GAIN_L                         =       64 // A     
	{0x596D, 0x41,8},    // IR_AP_PRE_GAIN_L                         =       64 // B     
	{0x596E, 0x40,8},    // IR_AP_PRE_GAIN_L                         =       64 // C     
	{0x596F, 0x42,8},    // IR_AP_PRE_GAIN_M                         =       64 // A     
	{0x5970, 0x42,8},    // IR_AP_PRE_GAIN_M                         =       64 // B     
	{0x5971, 0x42,8},    // IR_AP_PRE_GAIN_M                         =       64 // C     
	{0x5972, 0x41,8},    // IR_AP_PRE_GAIN_H                         =       64 // A     
	{0x5973, 0x41,8},    // IR_AP_PRE_GAIN_H                         =       64 // B     
	{0x5974, 0x48,8},    // IR_AP_PRE_GAIN_H                         =       70 // C     
	{0x5975, 0x28,8},    // IR_AP_POST_GAIN_L                        =        0 // A     
	{0x5976, 0x28,8},    // IR_AP_POST_GAIN_L                        =        0 // B     
	{0x5977, 0x28,8},    // IR_AP_POST_GAIN_L                        =        0 // C     
	{0x5978, 0x5C,8},    // IR_AP_POST_GAIN_M                        =       64 // A     
	{0x5979, 0x5B,8},    // IR_AP_POST_GAIN_M                        =       90 // B     
	{0x597A, 0x63,8},    // IR_AP_POST_GAIN_M                        =       64 // C     
	{0x597B, 0x00,8},    // IR_AP_POST_GAIN_H                        =        0 // A     
	{0x597C, 0x12,8},    // IR_AP_POST_GAIN_H                        =       85 // B     
	{0x597D, 0x6C,8},    // IR_AP_POST_GAIN_H                        =      120 // C     
	{0x597E, 0x10,8},    // IR_AP_GC_L_POS_LOWSLOPE                  =       16 // A     
	{0x597F, 0x10,8},    // IR_AP_GC_L_POS_LOWSLOPE                  =       16 // B     
	{0x5980, 0x10,8},    // IR_AP_GC_L_POS_LOWSLOPE                  =       16 // C     
	{0x5981, 0x10,8},    // IR_AP_GC_L_POS_HIGHSLOPE                 =       16 // A     
	{0x5982, 0x10,8},    // IR_AP_GC_L_POS_HIGHSLOPE                 =       16 // B     
	{0x5983, 0x06,8},    // IR_AP_GC_L_POS_HIGHSLOPE                 =       16 // C     
	{0x5984, 0x10,8},    // IR_AP_GC_L_NEG_LOWSLOPE                  =       16 // A     
	{0x5985, 0x10,8},    // IR_AP_GC_L_NEG_LOWSLOPE                  =       16 // B     
	{0x5986, 0x11,8},    // IR_AP_GC_L_NEG_LOWSLOPE                  =       16 // C     
	{0x5987, 0x05,8},    // IR_AP_GC_L_NEG_HIGHSLOPE                 =       16 // A     
	{0x5988, 0x10,8},    // IR_AP_GC_L_NEG_HIGHSLOPE                 =       16 // B     
	{0x5989, 0x05,8},    // IR_AP_GC_L_NEG_HIGHSLOPE                 =       16 // C     
	{0x598A, 0x10,8},    // IR_AP_GC_M_POS_LOWSLOPE                  =       16 // A     
	{0x598B, 0x10,8},    // IR_AP_GC_M_POS_LOWSLOPE                  =       16 // B     
	{0x598C, 0x08,8},    // IR_AP_GC_M_POS_LOWSLOPE                  =       16 // C     
	{0x598D, 0x10,8},    // IR_AP_GC_M_POS_HIGHSLOPE                 =       16 // A     
	{0x598E, 0x08,8},    // IR_AP_GC_M_POS_HIGHSLOPE                 =        8 // B     
	{0x598F, 0x06,8},    // IR_AP_GC_M_POS_HIGHSLOPE                 =       16 // C     
	{0x5990, 0x10,8},    // IR_AP_GC_M_NEG_LOWSLOPE                  =       16 // A     
	{0x5991, 0x10,8},    // IR_AP_GC_M_NEG_LOWSLOPE                  =       16 // B     
	{0x5992, 0x0C,8},    // IR_AP_GC_M_NEG_LOWSLOPE                  =       16 // C     
	{0x5993, 0x10,8},    // IR_AP_GC_M_NEG_HIGHSLOPE                 =       16 // A     
	{0x5994, 0x10,8},    // IR_AP_GC_M_NEG_HIGHSLOPE                 =       16 // B     
	{0x5995, 0x08,8},    // IR_AP_GC_M_NEG_HIGHSLOPE                 =       16 // C     
	{0x5996, 0x10,8},    // IR_AP_GC_H_POS_LOWSLOPE                  =       16 // A     
	{0x5997, 0x0C,8},    // IR_AP_GC_H_POS_LOWSLOPE                  =       12 // B     
	{0x5998, 0x0B,8},    // IR_AP_GC_H_POS_LOWSLOPE                  =       12 // C     
	{0x5999, 0x04,8},    // IR_AP_GC_H_POS_HIGHSLOPE                 =        4 // A     
	{0x599A, 0x03,8},    // IR_AP_GC_H_POS_HIGHSLOPE                 =        3 // B     
	{0x599B, 0x05,8},    // IR_AP_GC_H_POS_HIGHSLOPE                 =        8 // C     
	{0x599C, 0x10,8},    // IR_AP_GC_H_NEG_LOWSLOPE                  =       16 // A     
	{0x599D, 0x08,8},    // IR_AP_GC_H_NEG_LOWSLOPE                  =        8 // B     
	{0x599E, 0x08,8},    // IR_AP_GC_H_NEG_LOWSLOPE                  =       16 // C     
	{0x599F, 0x04,8},    // IR_AP_GC_H_NEG_HIGHSLOPE                 =        4 // A     
	{0x59A0, 0x03,8},    // IR_AP_GC_H_NEG_HIGHSLOPE                 =        3 // B     
	{0x59A1, 0x05,8},    // IR_AP_GC_H_NEG_HIGHSLOPE                 =        8 // C     
	{0x59A2, 0x00,8},    // IR_AP_LD_L_DELTA_LOW                     =        0 // A     
	{0x59A3, 0x00,8},    // IR_AP_LD_L_DELTA_LOW                     =        0 // B     
	{0x59A4, 0x00,8},    // IR_AP_LD_L_DELTA_LOW                     =        0 // C     
	{0x59A5, 0x20,8},    // IR_AP_LD_L_DELTA_HIGH_0                  =       32 // A     
	{0x59A6, 0x20,8},    // IR_AP_LD_L_DELTA_HIGH_0                  =       32 // B     
	{0x59A7, 0x1F,8},    // IR_AP_LD_L_DELTA_HIGH_0                  =       32 // C     
	{0x59A8, 0x50,8},    // IR_AP_LD_L_DELTA_HIGH_1                  =       80 // A     
	{0x59A9, 0x50,8},    // IR_AP_LD_L_DELTA_HIGH_1                  =       80 // B     
	{0x59AA, 0x4D,8},    // IR_AP_LD_L_DELTA_HIGH_1                  =       80 // C     
	{0x59AB, 0x00,8},    // IR_AP_LD_M_DELTA_LOW                     =        0 // A     
	{0x59AC, 0x90,8},    // IR_AP_LD_M_DELTA_LOW                     =      144 // B     
	{0x59AD, 0x00,8},    // IR_AP_LD_M_DELTA_LOW                     =        0 // C     
	{0x59AE, 0x50,8},    // IR_AP_LD_M_DELTA_HIGH_0                  =       80 // A     
	{0x59AF, 0x50,8},    // IR_AP_LD_M_DELTA_HIGH_0                  =       80 // B     
	{0x59B0, 0x52,8},    // IR_AP_LD_M_DELTA_HIGH_0                  =       80 // C     
	{0x59B1, 0x50,8},    // IR_AP_LD_M_DELTA_HIGH_1                  =       80 // A     
	{0x59B2, 0x50,8},    // IR_AP_LD_M_DELTA_HIGH_1                  =       80 // B     
	{0x59B3, 0x5C,8},    // IR_AP_LD_M_DELTA_HIGH_1                  =       80 // C     
	{0x59B4, 0x00,8},    // IR_AP_LD_H_DELTA_LOW                     =        0 // A     
	{0x59B5, 0xFF,8},    // IR_AP_LD_H_DELTA_LOW                     =      255 // B     
	{0x59B6, 0x00,8},    // IR_AP_LD_H_DELTA_LOW                     =        0 // C     
	{0x59B7, 0x30,8},    // IR_AP_LD_H_DELTA_HIGH_0                  =       48 // A     
	{0x59B8, 0xE0,8},    // IR_AP_LD_H_DELTA_HIGH_0                  =      224 // B     
	{0x59B9, 0x30,8},    // IR_AP_LD_H_DELTA_HIGH_0                  =       48 // C     
	{0x59BA, 0x20,8},    // IR_AP_LD_H_DELTA_HIGH_1                  =       32 // A     
	{0x59BB, 0x20,8},    // IR_AP_LD_H_DELTA_HIGH_1                  =       32 // B     
	{0x59BC, 0x20,8},    // IR_AP_LD_H_DELTA_HIGH_1  

	{0x0014, 0x03,8},   //CAPNUM
	{0x001D, 0x1B,8},   //OUTFMT_CAP     INTERLEAVE MODE SPOOF FREE-RUNNING MODE
	{0x0020, 0x00,8},   //READVECT_CAP
	{0x0024, 0x0A00,16},   //HSIZE_CAP  - 2560
	{0x002A, 0x0780,16},   //VSIZE_CAP  - 1920
	{0x021A, 0x0140,16},   //HSIZE_TN 320 WQVGA
	{0x021C, 0x00F0,16},   //VSIZE_TN 240
	{0x0224, 0x01,8},   	  //SIZE_HOLD_EN
	{0x02C8, 0x00,8},      //JPEGMAXMODE
	{0x0226, 0x0890,16},   //SIZE_HOLD_HOUT  2192
	{0x02CE, 0x0798,16},   //SIZE_HOLD_VOUT  1944
	{0x0381, 0x00,8},   //SENSMODE_CAP
	{0x0384, 0x03,8},   //FPSTYPE_CAP
	{0x000A, 0x01,8},   // AF_EXT

	{0x003A, 0x8201,16},	//DCK phase
	{0x0220, 0x1101,16},	//YUV thumb order
#endif

#else

	// ISX006-1 (FW 3013)
	//	 Parallel	 15fps
	//		  PLL	432MHz
	//		  DCK	54MHz
	//
	// inifile
	// size address data
	//
	{0x000b, 0x00,    8}, 	// PLL_CKSEL		 
	{0x02C7, 0x00,    8}, 	// SRCCK_DIV		 
	{0x02B8, 0x40,    8}, 	// VIF_CLKCONFIG1	 ( YUV : 108MHz )
	{0x02B9, 0x10,    8}, 	// VIF_CLKCONFIG2	 ( Monitor mode : 54MHz )	 
	{0x02BA, 0x50,    8}, 	// VIF_CLKCONFIG3	 
	{0x02BB, 0x50,    8}, 	// VIF_CLKCONFIG4	 
	{0x02BC, 0x10,    8}, 	// VIF_CLKCONFIG5	 ( JPEG mode :	54	MHz )
	{0x02BD, 0x50,    8}, 	// VIF_CLKCONFIG6	 
	{0x02BE, 0x50,    8}, 	// VIF_CLKCONFIG7	 
	{0x02C0, 0x40,    8}, 	// VIF_CLKCONFIG9	 ( Interleave mode : 108 MHz )
	{0x02C1, 0x40,    8}, 	// VIF_CLKCONFIG10	 
	{0x4C10, 0x40,    8}, 	// VIF_CLKCONFIG13	 
	{0x033E, 0x01,    8}, 	// HD_PLL_CKSEL 	 
	{0x0348, 0x00,    8}, 	// VIF_CLKCONFIG_HD1 
	{0x0349, 0x00,    8}, 	// VIF_CLKCONFIG_HD2 
	{0x034A, 0x01,    8}, 	// VIF_CLKCONFIG_HD3 
	{0x034B, 0x01,    8}, 	// VIF_CLKCONFIG_HD4 
	{0x4044, 0x21C0, 16},	// 1SECCLK_ALL
	{0x4046, 0x2A30, 16},	// 1SECCLK_HD_1_1
	{0x4048, 0x32A0, 16},	// 1SECCLK_HD_1_2
	{0x402B, 0x99,    8}, 	// FPS_SENSALL
	{0x402C, 0x012F, 16},	// FPS_SENS_HD_1_1
	{0x4050, 0x025D, 16},	// FPS_SENS_HD_1_2
	{0x4060, 0x21C0, 16},	// PIXEL_CLK
	{0x4062, 0x2A30, 16},	// PIXEL_CLK_HD_1_1
	{0x4064, 0x32A0, 16},	// PIXEL_CLK_HD_1_2
	{0x403E, 0x22E3, 16},	// FLSHT50
	{0x4074, 0x242D, 16},	// FLSHT50_HD_1_1
	{0x4078, 0x253A, 16},	// FLSHT50_HD_1_2
	{0x4040, 0x21D6, 16},	// FLSHT60
	{0x4076, 0x2320, 16},	// FLSHT60_HD_1_1
	{0x407A, 0x242D, 16},	// FLSHT60_HD_1_2
	{0x02A2, 0x0000, 16},	// VADJ_SENS_1_1
	{0x0364, 0x0027, 16},	// VADJ_SENS_HD_1_1
	{0x02A4, 0x0000, 16},	// VADJ_SENS_1_2
	{0x0366, 0x0000, 16},	// VADJ_SENS_HD_1_2
	{0x02A6, 0x0000, 16},	// VADJ_SENS_1_4
	{0x02A8, 0x0000, 16},	// VADJ_SENS_1_8
	{0x4C14, 0x0440, 16},	// FLC_OPD_HEIGHT_NORMAL_1_1
	{0x4C1C, 0x0420, 16},	// FLC_OPD_HEIGHT_HD_1_1
	{0x4C16, 0x03C0, 16},	// FLC_OPD_HEIGHT_NORMAL_1_2
	{0x4C1E, 0x02C0, 16},	// FLC_OPD_HEIGHT_HD_1_2
	{0x4C18, 0x01E0, 16},	// FLC_OPD_HEIGHT_NORMAL_1_4
	{0x4C1A, 0x00E0, 16},	// FLC_OPD_HEIGHT_NORMAL_1_8
	{0x02C4, 0x010A, 16},	// GPIO_FUNCSEL 	 : AF PWM & UART enable
	{0x4C86, 0x01,    8}, 	// VIF_DATA_MODE	 : USESRAM=1, ASYNCHS=0, RGBCCENTOR=0
	{0x001C, 0x00,    8},	// OUTFMT_MONI 0x00 -  YUV
	{0x0022, 0x0320, 16}, 	// HSIZE_MONI - 800
	{0x0028, 0x0258, 16}, 	// VSIZE_MONI - 600
	{0x0380, 0x01,    8},	// SENSMODE_MONI
	{0x0383, 0x02,    8},	// FPSTYPE_MONI (15fps)
	{0x0009, 0x16,    8}, 	// INCK_SET   
#endif

    {REG_TERM, VAL_TERM , 0}
};


static struct sensor_reg sensor_set_preview[] = {
	{0x0011, 0x00, 	8},     	//MODE SELECT_Preview
	
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_set_capture[] = {
	{0x0011, 0x02, 	 8},    //MODE SELECT
	
    {REG_TERM, VAL_TERM , 0}
};

 struct sensor_reg* sensor_reg_common[3] =
{
	sensor_initialize,
	sensor_set_preview,
	sensor_set_capture
};

static struct sensor_reg sensor_brightness_0[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_brightness_1[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_brightness_2[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_brightness_3[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_brightness_4[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_brightness[5] =
{
	sensor_brightness_0,
	sensor_brightness_1,
	sensor_brightness_2,
	sensor_brightness_3,
	sensor_brightness_4
};


static struct sensor_reg sensor_awb_auto[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_awb_daylight[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_awb_incandescent[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_awb_fluorescent[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_awb_cloudy[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_awb_sunset[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_awb[6] =
{
	sensor_awb_auto,
	sensor_awb_daylight,
	sensor_awb_incandescent,
	sensor_awb_fluorescent,
	sensor_awb_cloudy,
	sensor_awb_sunset
	
};


static struct sensor_reg sensor_iso_auto[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_iso_100[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_iso_200[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_iso_400[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_iso[4] =
{
	sensor_iso_auto,
	sensor_iso_100,
	sensor_iso_200,
	sensor_iso_400
};


static struct sensor_reg sensor_effect_normal[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_effect_gray[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_effect_negative[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_effect_sepia[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_effect_sharpness[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_effect_sketch[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_effect[6] =
{
	sensor_effect_normal,
	sensor_effect_gray,
	sensor_effect_negative,
	sensor_effect_sepia,
	sensor_effect_sharpness,
	sensor_effect_sketch,
};


static struct sensor_reg sensor_reg_flipnone[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_reg_hflip[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_reg_vflip[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_reg_hvflip[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_flip[4] =
{
	sensor_reg_flipnone,
	sensor_reg_hflip,
	sensor_reg_vflip,
	sensor_reg_hvflip,
};


static struct sensor_reg sensor_secne_auto[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_secne_night[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_secne_landscape[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_secne_portrait[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_secne_sport[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_scene[5] =
{
	sensor_secne_auto,
	sensor_secne_night,
	sensor_secne_landscape,
	sensor_secne_portrait,
	sensor_secne_sport
};

static struct sensor_reg sensor_me_mtrix[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_me_center_weighted[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_me_spot[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_metering_exposure[3] =
{
	sensor_me_mtrix,
	sensor_me_center_weighted,
	sensor_me_spot,
};

static struct sensor_reg sensor_af_single[] = {
    {REG_TERM, VAL_TERM , 0}
};

static struct sensor_reg sensor_af_manual[] = {
    {REG_TERM, VAL_TERM , 0}
};

struct sensor_reg* sensor_reg_af[2] =
{
	sensor_af_single,
	sensor_af_manual,
};

static int write_regs(const struct sensor_reg reglist[])
{
	int err, i;
	int err_cnt = 0;	
	unsigned char data[10];
	char data_r[2];
	unsigned short data_read = 0;
	unsigned char bytes;
	const struct sensor_reg *next = reglist;
	unsigned int total_cnt = 0;

	while (!((next->reg == REG_TERM) && (next->val == VAL_TERM)))
	{
		if(next->reg == REG_TERM && next->val != VAL_TERM)
		{
			mdelay(next->val);
			printk("Sensor init Delay[%d]!!!! \n", next->val);
			next++;
		}
		else
		{
			bytes = 0;
			data[bytes]= next->reg>>8;		 bytes++;		
			data[bytes]= (u8)next->reg&0xff; bytes++;

			if(next->val_size == 32)
			{
				data[bytes]= (u8)next->val&0xff; 	bytes++;
				data[bytes]= (next->val>>8)&0xff; 	bytes++;
				data[bytes]= (next->val>>16)&0xff; 	bytes++;
				data[bytes]= (next->val>>24)&0xff; 	bytes++;
				//printk("I2C[%d]: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x !!!! \n", bytes, data[0], data[1], data[2], data[3], data[4], data[5]); 
			}
			else if(next->val_size == 16)
			{
				data[bytes]= (u8)next->val&0xff; 	bytes++;
				data[bytes]= (next->val>>8)&0xff;	bytes++;
				//printk("I2C[%d]: 0x%x, 0x%x, 0x%x, 0x%x !!!! \n", bytes, data[0], data[1], data[2], data[3]); 
			}
			else //8
			{
				data[bytes]= (u8)next->val&0xff; 	bytes++;
				//printk("I2C[%d]: 0x%x, 0x%x, 0x%x !!!! \n", bytes, data[0], data[1], data[2]); 
			}		

			total_cnt++;
	        err = DDI_I2C_Write(data, 2, bytes-2);
			udelay(25);

			if(0)//!err)//read test
			{
				if(next->val_size == 32)
				{

				}
				else if(next->val_size == 16)
				{
					DDI_I2C_Read(next->reg, 2, data_r, 2);
					data_read = data_r[1];
					data_read = (data_read<<8)&0xff00 | data_r[0];
				}
				else
				{
					DDI_I2C_Read(next->reg, 2, data_r, 1);
					data_read = data_r[0];
				}

				if(next->val != data_read)
				{
					err = 1;
					printk("Err: 0x%x-(0x%x -> 0x%x) !!!! \n", next->reg, next->val, data_read);
				}
			}

			
	        if (err)
	        {
				err_cnt++;
				if(err_cnt >= 5)
				{
					printk("ERROR: Sensor I2C !!!! \n"); 
					return err;
				}
	        }
	        else
	        {		
	            next++;
				err_cnt = 0;
	        }
		}
	}

	printk("total reg count : %d \n",total_cnt);

	return 0;
}


/**********************************************************
*    Function  
**********************************************************/
void sensor_delay(int ms)
{
#if 0 //ZzaU
	unsigned int msec;

	msec = ms / 10; //10msec unit

	if(!msec)
		msleep(1);
	else
		msleep(msec);
#else
	mdelay(ms);
#endif
}


static int sensor_open(void)
{
	int ret;
	char data_r[2];
    static unsigned long jiffies_prev;
	
	dprintk("sensor_open In \n");

	//SUB CAM Power Down!!
	SUB_SENSOR_PWRDN_DISABLE(); sensor_delay(2);

	//MAIN
	MAIN_SENSOR_PWRDN_DISABLE();sensor_delay(2); // stand-by low
	SENSOR_RESET_LOW();			sensor_delay(2);

	//put the power using pmic.
	SENSOR_CORE_PWR_ON();		sensor_delay(2);		
	SENSOR_AF_PWR_ON();			sensor_delay(2);
	SENSOR_IO_PWR_ON();			sensor_delay(5);

	CIF_Open(); 				sensor_delay(10);

	SENSOR_RESET_HIGH();		sensor_delay(20);
	MAIN_SENSOR_PWRDN_ENABLE(); sensor_delay(30);

	jiffies_prev = jiffies;
	ret = write_regs(sensor_reg_common[0]);
	printk("init time : %d ms \n", (jiffies - jiffies_prev)*10);
	
	return ret;
}



static int sensor_close(void)
{
	dprintk("sensor_close In \n");

	CIF_ONOFF(OFF);

	SENSOR_RESET_LOW();				sensor_delay(1);
	MAIN_SENSOR_PWRDN_DISABLE();	sensor_delay(1);

	CIF_Close();					sensor_delay(1);

	SENSOR_IO_PWR_OFF();
	SENSOR_AF_PWR_OFF();
	SENSOR_CORE_PWR_OFF();

	return 0;
}

static int sensor_preview(void)
{
	return write_regs(sensor_reg_common[1]);
}

static int sensor_capture(void)
{
	int ret;
	int timeout = 16;
	char data_r[2] = {0x00, 0x00};
	char data_w[3] = {0x00, 0xFC, 0x02};
	
	ret = write_regs(sensor_reg_common[2]);

#if 1 //use this sleep until sensor company send right check-routine.
	mdelay(80);
#else
	{ 		
		while(1)
		{
			DDI_I2C_Read(0x00F8, 2, data_r, 1);	// 1byte read
			if(data_r[0]&0x02)					// Bit1 == 1 ?
			{
				printk("mode changed!! \n");
				break;
			}

			if(!timeout)
			{
				printk("mode-check error!!");
				break;
			}

			timeout--;
			mdelay(5); // wait for 5ms.
		}

		timeout = 16;
		while(1)
		{
			DDI_I2C_Write(data_w, 2, 1);  		// clear interrupt status :: {0x00FC, 0x02,  8}			
			DDI_I2C_Read(0x00F8, 2, data_r, 1);	// 1byte read
			
			if(data_r[0]&0x02 == 0x00)			// Bit1 == 0 ?
			{
				printk("jpeg-ready done!! \n");
				break;
			}

			if(!timeout)
			{
				printk("jpeg-check error!!");
				break;
			}

			timeout--;
			mdelay(5); // wait for 5ms.
		}
		
	} 
#endif

	return ret;
}

static int sensor_capturecfg(int width, int height)
{
	char data_width[4] = {0x00, 0x24, 0x00, 0x00};
	char data_height[4] = {0x00, 0x2A, 0x00, 0x00};
	unsigned short h_size, v_size;	

	h_size = width;
	v_size = height;

	data_width[2] = (u8)h_size&0xff;
	data_width[3] = (h_size>>8)&0xff;
	data_height[2] = (u8)v_size&0xff;
	data_height[3] = (v_size>>8)&0xff;
	
	DDI_I2C_Write(data_width, 2, 2);
	DDI_I2C_Write(data_height, 2, 2);

	printk("capture res = %d x %d !! \n", h_size, v_size);
	
	return 0;
}

static int sensor_zoom(int val)
{
	return 0;
}

static int sensor_autofocus(int val)
{
	return write_regs(sensor_reg_af[val]);
}

static int sensor_effect(int val)
{
	return write_regs(sensor_reg_effect[val]);
}

static int sensor_flip(int val)
{
	return write_regs(sensor_reg_flip[val]);
}

static int sensor_iso(int val)
{
	return write_regs(sensor_reg_iso[val]);
}

static int sensor_me(int val)
{
	return write_regs(sensor_reg_metering_exposure[val]);
}

static int sensor_wb(int val)
{
	return write_regs(sensor_reg_awb[val]);
}

static int sensor_bright(int val)
{
	return write_regs(sensor_reg_brightness[val]);
}

static int sensor_scene(int val)
{
	return write_regs(sensor_reg_scene[val]);
}

static int sensor_check_esd(int val)
{
	return 0;
}

static int sensor_check_luma(int val)
{
	return 0;
}


/**********************************************************
*	 Function  
**********************************************************/
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open			= sensor_open;
	sensor_func->Close			= sensor_close;

	sensor_func->Set_Preview	= sensor_preview;
	sensor_func->Set_Capture	= sensor_capture;
	sensor_func->Set_CaptureCfg = sensor_capturecfg;

	sensor_func->Set_Zoom		= sensor_zoom;
	sensor_func->Set_AF 		= sensor_autofocus;
	sensor_func->Set_Effect 	= sensor_effect;
	sensor_func->Set_Flip		= sensor_flip;
	sensor_func->Set_ISO		= sensor_iso;
	sensor_func->Set_ME 		= sensor_me;
	sensor_func->Set_WB 		= sensor_wb;
	sensor_func->Set_Bright 	= sensor_bright;
	sensor_func->Set_Scene		= sensor_scene;

	sensor_func->Check_ESD		= sensor_check_esd;
	sensor_func->Check_Luma 	= sensor_check_luma;
}
#endif 
