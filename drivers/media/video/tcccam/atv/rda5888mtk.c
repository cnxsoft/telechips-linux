// ---------------------------------------------------------------------------
// Copyright 2009-2010 (c) RDA Microelectronics, Inc.
//
// File: rda5888sprd.c
// Revision: 0
// Author: wuhp
// Date: 2010-01-27
// Description: 
// ---------------------------------------------------------------------------
//#include "drv_comm.h"
//#include "IntrCtrl.h"
//#include "reg_base.h"
//#include "gpio_sw.h"
//#include "isp_if.h"
//#include "upll_ctrl.h"
//#include "sccb.h"

#include <linux/delay.h>

#include "rda5888mtk.h"
#include "rda5888app.h"
#include "rda5888fm.h"

uint8 g_nRdamtvTimer = 0;
uint8 const FM_RADIO_INPUT_LEVEL_THRESHOLD = 5;

static bool m_bIsRDAMTVMode = 0;	// kal_bool -> bool
static bool m_bIsFullScr = FALSE;	// KAL_FALSE -> FALSE
static RDAMTV_POWER_STATE m_eRDAMTVPowerState = RDAMTV_POWER_OFF;

#define RDAMTV_OUTPUT_PIXELS        (360)
#define RDAMTV_OUTPUT_LINES         (312)

extern void ov7670_poweron_for_tv(void);

//extern void cis_module_power_on(kal_bool on);
extern int pmic_vmc_enable();
extern int pmic_vasw_enable();
extern int image_sensor_func_config();

void RDAEXT_DelayMs(uint32 ms)
{

	#if 1 //ZzaU
        unsigned int msec;

        msec = ms / 10; //10msec unit

        if(!msec)
                msleep(1);
        else
                msleep(msec);
	#else
		uint16 i, j; 

		for (j = 0; j < ms; j++)
		{
			for(i = 0; i < 30000; i++)
			   i++;
		}
	#endif
}

void rdamtv_if_module_power_on(unsigned char on)
{
	RDAEXT_DebugPrint(0, "rdamtv_if_module_power_on, power = %d\n", on);

	#if 0
	GPIO_ModeSetup(RDA5888_SDA_PIN,0);
	GPIO_ModeSetup(RDA5888_SCL_PIN,0);
	GPIO_ModeSetup(RDA5888_POWER_PIN,0);
	GPIO_InitIO(1, RDA5888_SDA_PIN);
	GPIO_InitIO(1, RDA5888_SCL_PIN);
	GPIO_InitIO(1, RDA5888_POWER_PIN);
	#endif

	if (on)
	{
		//GPIO_WriteIO(1, RDA5888_POWER_PIN); // ATV power on

		if (g_nRdamtvTimer == 0)
		{
		    //GPTI_GetHandle(&g_nRdamtvTimer);
		}

        #if defined(MT6235)||defined(MT6235B) 
			cis_module_power_on(KAL_TRUE);
		#endif
		
		// to close fm and camera
		//CL6012X_PowerOffProc();
		//BK1080_PowerOffProc();

		//cis_module_power_on(KAL_FALSE);

		#if 0
		// Sensor Power, CMOS Sensor Power Down Signal Output      
		GPIO_ModeSetup(11, 0);  
		GPIO_InitIO(1, 11);
		GPIO_WriteIO(1, 11);	  
		GPIO_ModeSetup(10, 0);   
		GPIO_InitIO(1, 10);
		GPIO_WriteIO(1, 10);
		#endif
	}
	else
	{
		#if 0
		GPIO_WriteIO(0, RDA5888_POWER_PIN); // ATV power off
		GPTI_StopItem(g_nRdamtvTimer);
		GPIO_WriteIO(1,RDA5888_SCL_PIN);
		GPIO_WriteIO(1,RDA5888_SDA_PIN);
		#endif

		#if defined(MT6235)||defined(MT6235B) 
		    cis_module_power_on(KAL_FALSE);
		#endif
	}
}

int8 rdamtv_if_Init(void)
{
	RDAEXT_DebugPrint(0,"rdamtv_if_Init start1\n");

	if (m_eRDAMTVPowerState == RDAMTV_POWER_ON)
		return 1;										

	rdamtv_if_module_power_on(TRUE);
	RDAEXT_DelayMs(50);

	RDAEXT_DebugPrint(0,"rdamtv_if_Init start2\n");

	RDAAPP_Init();

	RDAEXT_DelayMs(50);

	#if 0
	kal_sleep_task(2); // To wait for Stable Power

	RESET_CMOS_SENSOR_MODE1;	// High Active
	SET_CMOS_CLOCK_POLARITY_LOW;
	SET_VSYNC_POLARITY_HIGH;
	SET_HSYNC_POLARITY_LOW;
	ENABLE_CAMERA_INDATA_FORMAT;

	SET_CAMERA_INPUT_TYPE(INPUT_YUV422);	//INPUT_YUV422
	SET_CAMERA_INPUT_ORDER(INPUT_ORDER_CbYCrY1);
	DISABLE_CAMERA_TG_CLK_48M;

	// Set Low to MCLK
	//SET_CMOS_RISING_EDGE(0);
	//SET_CMOS_FALLING_EDGE(0);
	DISABLE_CAMERA_TG_PHASE_COUNTER;
	DISABLE_CAMERA_CLOCK_OUTPUT_TO_CMOS;

	isp_contrast_setting(170);
	isp_bright_setting(80);

	set_isp_driving_current(ISP_DRIVING_2MA); 

	#endif
	m_eRDAMTVPowerState = RDAMTV_POWER_ON;
	
	return 1;
}	

static void rdamtv_if_power_off(void)
{	
	if (m_eRDAMTVPowerState == RDAMTV_POWER_OFF) 
		return ;
	
	rdamtv_if_module_power_on(FALSE);
	//UPLL_Disable(UPLL_OWNER_ISP);
	
	m_eRDAMTVPowerState = RDAMTV_POWER_OFF;
}

#if 1 // for adjust grab wnd para.
int g_nGrabStartX = 28;
int g_nGrabStartY = 2;
int g_nGrabWidth = 340;
int g_nGrabHeight = 278;
#endif

uint16 grab_start_x;//extern kal_uint16 grab_start_x;
uint16 grab_start_y;//extern kal_uint16 grab_start_y;
uint16 exposure_window_width;//extern kal_uint16 exposure_window_width;//320
uint16 exposure_window_height;//extern kal_uint16 exposure_window_height;//288
#if 0
static void rdamtv_if_preview(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{
    int nTvArea;

	//MPEG4_encode_mode=KAL_FALSE;
	ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;

	//SET_TG_PIXEL_CLK_DIVIDER(3);
	//SET_CMOS_DATA_LATCH(2);
	SET_TG_OUTPUT_CLK_DIVIDER(3); // MCLK = 26MHz
	SET_CMOS_RISING_EDGE(1);
	SET_CMOS_FALLING_EDGE(1);
	SET_TG_PIXEL_CLK_DIVIDER(3);
	SET_CMOS_DATA_LATCH(2);

    nTvArea = RDAAPP_GetTVArea();
    kal_prompt_trace(0, "[rdamtv]rdamtv_if_preview, m_nCurTvArea = %d\n", nTvArea);

	/*if ((RDAAPP_GetTPMode()) || (mmi_analogtv_get_state() == 4)) // TP mode or auto search state.
	{
		image_window->grab_start_x = 48;
		image_window->grab_start_y = 4;
		image_window->exposure_window_width = 340;
		image_window->exposure_window_height = 238;
		grab_change_flag = 1;
		kal_prompt_trace(0, "[rdamtv]rdamtv_if_preview_2\n");
		return;
	}*/
	
	switch (nTvArea)
	{
		case RDAMTV_AREA_CHINA:
		case RDAMTV_AREA_CHINA_SHENZHEN:
		case RDAMTV_AREA_CHINA_HONGKONG:
		case RDAMTV_AREA_ARGENTINA:   
		case RDAMTV_AREA_WESTERNEUROP:
		case RDAMTV_AREA_TURKEY:
		case RDAMTV_AREA_AFGHANISTA:
		case RDAMTV_AREA_SINGAPORE:
		case RDAMTV_AREA_THAI:
		case RDAMTV_AREA_CAMBODIA:
		case RDAMTV_AREA_INDONESIA:
		case RDAMTV_AREA_MALAYSIA:
		case RDAMTV_AREA_LAOS:
		case RDAMTV_AREA_INDIA:
		case RDAMTV_AREA_PAKISTAN:
		case RDAMTV_AREA_SPAIN:
		case RDAMTV_AREA_PORTUGAL:
		case RDAMTV_AREA_VIETNAM:
		case RDAMTV_AREA_UK:
		case RDAMTV_AREA_SOUTH_AFRICA:	
		case RDAMTV_AREA_RUSSIA:
#if 0
			image_window->grab_start_x = g_nGrabStartX;
			image_window->grab_start_y = g_nGrabStartY;
			image_window->exposure_window_width = g_nGrabWidth;
			image_window->exposure_window_height = g_nGrabHeight;
#else
#if (RDA5888_CHIP_VER == 4)
			image_window->grab_start_x = 16;// 4
			image_window->grab_start_y = 2;// 4
			image_window->exposure_window_width = 340;//352
			image_window->exposure_window_height = 284;//280
#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6))
			image_window->grab_start_x = 28;
			image_window->grab_start_y = 2;
			image_window->exposure_window_width = 340;
			image_window->exposure_window_height = 278;
#endif
#endif
			break;

		case RDAMTV_AREA_CHINA_TAIWAN:	
		case RDAMTV_AREA_AMERICA:	
		case RDAMTV_AREA_CANADA:
		case RDAMTV_AREA_JAPAN:
		case RDAMTV_AREA_BRAZIL: 
		case RDAMTV_AREA_BURMA:
		case RDAMTV_AREA_MEXICO:
		case RDAMTV_AREA_PHILIPINES:
		case RDAMTV_AREA_KOREA:
		case RDAMTV_AREA_CHILE:
		case RDAMTV_AREA_VENEZUELA:
#if 0
			image_window->grab_start_x = g_nGrabStartX;
			image_window->grab_start_y = g_nGrabStartY;
			image_window->exposure_window_width = g_nGrabWidth;
			image_window->exposure_window_height = g_nGrabHeight;
#else
#if (RDA5888_CHIP_VER == 4)			
			image_window->grab_start_x = 48;// 4
			image_window->grab_start_y = 2;// 4
			image_window->exposure_window_width = 320;//352
			image_window->exposure_window_height = 240;//280
#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6))
			image_window->grab_start_x = 48;// 4
			image_window->grab_start_y = 4;// 4
			image_window->exposure_window_width = 340;//352
			image_window->exposure_window_height = 238;//280
#endif
#endif
			break;
		
		default:
			image_window->grab_start_x=16;// 4
			image_window->grab_start_y=2;// 4
			image_window->exposure_window_width=336;//352
			image_window->exposure_window_height=284;//280
			break;
	}
}
#endif

void rdamtv_if_set_fullscreen(bool on)
{
    RDAEXT_DebugPrint(0, "[rdamtv]rdamtv_if_set_fullscreen\n");

	m_bIsFullScr = on;
}

bool rdamtv_if_is_fullscreen(void)
{
    RDAEXT_DebugPrint(0, "[rdamtv]rdamtv_if_is_fullscreen\n");
	
    return (m_bIsFullScr = m_bIsFullScr ? FALSE : TRUE);
}

/*************************************************************************
//these functions not used, set default
*************************************************************************/
static void rdamtv_if_get_id(uint8 *sensor_write_id, uint8 *sensor_read_id)
{
    RDAEXT_DebugPrint(0, "[rdamtv]rdamtv_if_get_id\n");
	
	return;
}

static void rdamtv_if_get_size(uint16 *sensor_width, uint16 *sensor_height)
{
	*sensor_width = 352;
	*sensor_height = 232;
}

static void rdamtv_if_get_period(uint16 *pixel_number, uint16 *line_number)
{
    RDAEXT_DebugPrint(0, "[rdamtv]rdamtv_if_get_period\n");
	
	*pixel_number = 352;
	*line_number = 232;
}

#if 0
static void rdamtv_if_capture(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{
	return;
}
#endif

static void rdamtv_if_write_reg(uint32 addr, uint32 para)
{
	return;
}

uint32 rdamtv_if_read_reg(uint32 addr)
{
	return 0x40;
}

static void rdamtv_if_set_shutter(uint16 shutter)
{
	return;
}

static void rdamtv_if_night_mode(bool enable)
{
	return;
}

static uint16 rdamtv_if_set_gain(uint16 gain)
{
	return 0x40;
}

static void rdamtv_if_set_flashlight(bool enable)
{
	return;
}

static uint32 rdamtv_if_yuv_setting(uint32 cmd, uint32 para)
{
	return TRUE;
}

static void rdamtv_if_check_id(void)
{

}

static uint16 rdamtv_if_read_gain(void)
{
	return 0x40;
}

#if 0
image_sensor_func_struct rdamtv_if_func =
{
#if (defined(__RC_MUlTI_YUV_SENOR__))
    rdamtv_if_check_id,
#endif

#if (defined(__RC_SINGLE_YUV_SENOR_OV7670__))
    rdamtv_if_read_gain,
#endif
	rdamtv_if_Init,
	rdamtv_if_get_id,
	rdamtv_if_get_size,
	rdamtv_if_get_period,
	rdamtv_if_preview,
	rdamtv_if_capture,
	rdamtv_if_write_reg,
	rdamtv_if_read_reg,
	rdamtv_if_set_shutter,
	rdamtv_if_night_mode, 
	rdamtv_if_power_off,
	rdamtv_if_set_gain,
	rdamtv_if_set_flashlight,
	rdamtv_if_yuv_setting
};
#endif

void rdamtv_if_func_config(void)
{
	if(RDAMTV_APP_MODULE == rdamtv_if_get_cur_module())
	{
		RDAEXT_DebugPrint(0, "[rdamtv]rdamtv_if_func_config_1\n");
		
		//image_sensor_func = &rdamtv_if_func;	
	}
	else
	{
	    RDAEXT_DebugPrint(0, "[rdamtv]rdamtv_if_func_config_2\n");
		
		#if !defined(WIN32)
	    //image_sensor_func_config(); 
		#endif
	}
}

unsigned char rdamtv_if_get_cur_module(void)
{
	return m_bIsRDAMTVMode;
}

void rdamtv_if_set_cur_module(unsigned char is_atv_app)
{
	m_bIsRDAMTVMode = is_atv_app;
}

unsigned short rdamtv_if_get_chn_count(void)
{
    extern int g_nCurChCnt;

	return (g_nCurChCnt+1);
}


////////////

void FMDrv_PowerOnReset(void)
{
    uint16 reg;

	RDAEXT_DebugPrint(0, "[rdamtv]FMDrv_PowerOnReset");

#if 0	
    // hardware initial
    GPIO_ModeSetup(RDA5888_SDA_PIN,0);
	GPIO_ModeSetup(RDA5888_SCL_PIN,0);
	GPIO_ModeSetup(RDA5888_POWER_PIN,0);
	GPIO_InitIO(1, RDA5888_SDA_PIN);
	GPIO_InitIO(1, RDA5888_SCL_PIN);
	GPIO_InitIO(1, RDA5888_POWER_PIN);

	// power on
    GPIO_WriteIO(1, RDA5888_POWER_PIN);
#endif

	RDAEXT_DelayMs(100);
	
    RDA5888FM_Init();
}

void FMDrv_PowerOffProc(void)
{
    RDAEXT_DebugPrint(0, "[rdamtv]FMDrv_PowerOffProc");

#if 0
    // hardware initial
    GPIO_ModeSetup(RDA5888_SDA_PIN,0);
	GPIO_ModeSetup(RDA5888_SCL_PIN,0);
	GPIO_ModeSetup(RDA5888_POWER_PIN,0);
	GPIO_InitIO(1, RDA5888_SDA_PIN);
	GPIO_InitIO(1, RDA5888_SCL_PIN);
	GPIO_InitIO(1, RDA5888_POWER_PIN);

	// power off and set iic pin state.
    GPIO_WriteIO(0, RDA5888_POWER_PIN);
    GPIO_WriteIO(1, RDA5888_SDA_PIN);
    GPIO_WriteIO(1, RDA5888_SCL_PIN);
#endif
}

void FMDrv_SetFreq(signed short curf )
{
	RDA5888FM_SetFreq(curf);
}

uint8 FMDrv_GetSigLvl(int16 curf )
{
	return RDA5888FM_GetSigLevel(curf);
}

void FMDrv_FineTune(int16 freq)
{
}

uint8 FMDrv_ValidStop(int16 freq, int8 signalvl, bool is_step_up)
{
    uint8 rt;
	
    rt = RDA5888FM_IsValidFreq(freq);

	return rt;
}

void FMDrv_Mute(uint8 mute)
{
    RDAEXT_DebugPrint(0, "[rdamtv]FMDrv_Mute");
}

void FMDrv_ChipInit(void)
{
	RDAEXT_DebugPrint(0, "[rdamtv]FMDrv_ChipInit");
}

bool FMDrv_IsChipValid(void)
{
    return RDA5888FM_IsValidChip();
}

