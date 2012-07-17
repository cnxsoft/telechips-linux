/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : CAMERA INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCC93XX
*    SOURCE        : TCC_ISP_API_interface.h
*
*    START DATE    : 2010. 2. 19.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM BT 3 TEAM
*                : TELECHIPS, INC.
* 
************************************************************************/

#ifndef __ISP_INTERFACE_h__
#define __ISP_INTERFACE_h__

#if 0
static int isp_debug	   = 1;
#else
static int isp_debug	   = 0;
#endif

	#define MSGPRINTF(msg...)	if (isp_debug) { printk( "isp_interface : " msg); }	
	#define CAMERA_INCLUDE
	#define FEATURE_TCC_ISP
//#include "simdef.h"

//#define SENSOR_8M_SONY_IMX073PQH6_C_1LANE
//#define SENSOR_8M_SONY_IMX073PQH6_C_2LANE
#define SENSOR_PARALLEL
//#define TCCISP_GEN_CFG_UPD
//#define TCCISP_ONE_IRQ
#define TCCISP_UPDATE
#ifdef CAMERA_INCLUDE  
//#include "sensor_api.h"
#ifdef FEATURE_TCC_ISP
#include <linux/delay.h>
#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <mach/bsp.h>

#include "drv_isp.h"

//#define ISP_VER V1111_2222_3333_4444
// 1111 : camera app. version
// 2222 : isp version
// 3333 : 지원 sensor에 따라 
// 4444 : minor version
#define ISP_VER V0000_0001_0010_0000
// sensor : 8M_Sony_IMX046ES

//#define TCC9300S
//#define HDMI_EN
//#define PREV8M
#define PARALLELCAMERA_MODE
//#define MIPII2C
//#define I2CCAM  2



///////////////////////////////////////////////////
// etc
///////////////////////////////////////////////////
// predefine 
//
typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

//#include "TCC_ISP_API.h"

/**
 * 센서 preset값을 메모리에 load하여, 해당 pointer를 전달한다.
 * @param pProfile  [out] load하 센서 preset값의 포인터
 * @return error code. 
 */
int ISP_SensorLoad( DRVISPPreset*** pPreset);

// will be added debugging functions

///////////////////////////////////////////////////
// Device interface
///////////////////////////////////////////////////

int DRV_CKC_Isp(unsigned clk);

/**
 * 카메라 센서를 제어하기 위한 함수. 초기화.
 * @return error code. 
 */
int _DRV_I2C_Init(void);

/**
 * 카메라 센서를 제어하기 위한 함수. Write 함수
 * @param slaveaddr [in] 
 * @param subaddr 	[in] 
 * @param data 		[in] 
 * @param mode		[in] 
 *					- [1:0] 0:data 8bit, 1:data 16bit 2:data 32bit 
 *					- or [3:2] 0:addr 8bit, 1:addr 16bit 2:addr 32bit  
 * @return error code. 
 */
int _DRV_I2C_Write(unsigned slaveaddr, unsigned subaddr, unsigned data, unsigned mode);

/**
 * 카메라 센서값을 읽음. Read 함수
 * @param slaveaddr [in] 
 * @param subaddr 	[in] 
 * @param data 		[out] 
 * @param mode		[in] 
 *					- [1:0] 0:data 8bit, 1:data 16bit 2:data 32bit 
 *					- or [3:2] 0:addr 8bit, 1:addr 16bit 2:addr 32bit  
 * @return error code. 
 */
int _DRV_I2C_Read( unsigned slaveaddr, unsigned subaddr, unsigned *data, unsigned mode);

/**
 * 카메라 센서를 제어하기 위한 함수. 종료.
 * @return error code. 
 */
int _DRV_I2C_Exit(void);

/**
 * ISP Interrupt 활성화 설정
 * @param clk 	[in] clock(hz)
 * @return error code. 
 */
int DRV_INTC_Enable( unsigned mask );

/**
 * ISP Interrupt 비활성화 설정
 * @param clk 	[in] clock(hz)
 * @return error code. 
 */
int DRV_INTC_Disable( unsigned mask );

/* TBD
typedef struct
{
	unsigned start;
	unsigned end_expected;
	unsigned delay;
	unsigned state;				
} DrvISPTimer_Handle;

int Drv_Timer_initHandler(void);
int Drv_Timer_destroy ( DrvISPTimer_Handle* hTimer );
int Drv_Timer_create( DrvISPTimer_Handle** hTimer, unsigned duration, unsigned suspend);
int Drv_Timer_wait( DrvISPTimer_Handle* hTimer, unsigned suspend);
int Drv_Timer_continue( DrvISPTimer_Handle* hTimer, unsigned suspend);
*/

/**
 * 카메라 센서 공급 clock 설정
 * @param clk 	[in] clock(hz)
 * @return error code. 
 */
int	DRV_SensorCkc(unsigned clk);

/**
 * 카메라 센서 reset pin 제어
 * @param enable 	[in] 0:low, 1:high
 * @return error code. 
 */
int DRV_SensorReset(unsigned enable);

/**
 * 카메라 센서 standby pin 제어
 * @param enable 	[in] 0:low, 1:high
 * @return error code. 
 */
int DRV_SensorStandby(unsigned enable);

// TBD
int DRV_Flesh  (unsigned onoff);
int DRV_AFDrive(unsigned handle);
int DRV_Xenon  (unsigned handle);

/**
 *  Isp FSM transition function.  * -> PowerOff
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_Init(HIsp hisp);

/**
 *  Isp FSM transition function.  PowerOff -> *
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_Exit(HIsp hisp);

/**
 *  Isp FSM transition function.  PowerOff -> Idle
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_TurnOn(HIsp hisp);

/**
 *  Isp FSM transition function.  Idle -> PowerOff
 * 	주의!! 해당 센서의 booting sequence와 delay time 고려해서 코딩 
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_TurnOff(HIsp hisp);

/**
 *  Isp FSM transition function.  Standby -> Idle
 * 	주의!! 해당 센서의 power off sequence와 delay time 고려해서 코딩 
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_WakeUp(HIsp hisp);

/**
 *  Isp FSM transition function.  Idle -> Standby
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_Sleep(HIsp hisp);

/**
 *  Isp FSM transition function.  Idle -> Streaming
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_Stream(HIsp hisp);

/**
 *  Isp FSM transition function.  Streaming -> Idle
 * @param hisp 	[in] isp handler
 * @return error code. 
 */
int DRV_Trans_Stop(HIsp hisp);


///////////////////////////////////////////////////
// Platform adaptation functions
///////////////////////////////////////////////////

// event handler
// mutex 
// task create / destroy
// memory allocation / free
// 

/**
 *  TimeDelay function (unit : ms)
 * @param min 	[in] 최소 지연시간 
 * @param max 	[in] 최대 지연시간 
 * @return error code. 
 */
int SAL_TimeDelay (int duration);

/**
 *  TimeDelay function (unit : micro)
 * @param min 	[in] 최소 지연시간 
 * @param max 	[in] 최대 지연시간 
 * @return error code. 
 */
//int SAL_TimeDelay_micro (int min, int max);
int SAL_TimeDelay_micro (int duration);


///////////////////////////////////////////////////
// ISP_API
///////////////////////////////////////////////////
#define TCC_ISP_PREVIEW_STATUS_STOP    0
#define TCC_ISP_PREVIEW_STATUS_START   1

#define SENSOR_HSYNC_POLARITY     ACT_HIGH
#define SENSOR_VSYNC_POLARITY     ACT_LOW

#define ISP_AUTO_FOCUS_CTRL_STOP      0
#define ISP_AUTO_FOCUS_CTRL_START1    1
#define ISP_AUTO_FOCUS_CTRL_START2    2

typedef enum {
    ISP_FORMAT_JPEG,
	ISP_FORMAT_YUV422,
	ISP_FORMAT_YUV420,
	ISP_FORMAT_RGB565,
	ISP_FORMAT_BAYER,
	ISP_FORMAT_MAX
} FORMAT;

typedef enum{
	ISP_AE_FPS_AUTO,
	ISP_AE_FPS_60,
	ISP_AE_FPS_30,
	ISP_AE_FPS_15,
	ISP_AE_FPS_12,
	ISP_AE_FPS_7_5,
	ISP_AE_FPS_7,
	ISP_AE_FPS_5,
	ISP_NAE_FPS_60,
	ISP_NAE_FPS_30,
	ISP_NAE_FPS_15,
	ISP_FPS_MAX	
} FPS_TYPE;

typedef enum{
	ISP_NOMAL,
	ISP_HOR_MIRROR,
	ISP_VER_FLIP,
	ISP_MIRROR_FLIP,
	ISP_FLIP_MAX
} sensor_mirrorflip_type;

typedef enum{
	ISP_Single_Shot,
	ISP_Multi_Shot,
	ISP_Frame_Shot,
	ISP_Mosaic_Shot, //mosaic shot 에서도 (Non frame shot // frame shot) 고려 해볼것
    ISP_Panorama_Shot,
    ISP_Best_Shot,
    ISP_ZeroDelay_Shot,
    ISP_Shot_Max
} CAPTURE_MODE_TYPE;

#if 0
typedef enum {
    FORMAT_JPEG,
	FORMAT_YUV422,
	FORMAT_YUV420,
	FORMAT_RGB565,
	FORMAT_BAYER,
	FORMAT_MAX
} IMAGE_FORMAT;
#endif
typedef enum {
	ISP_JPEG_QUALITY_SUPERFINE,
	ISP_JPEG_QUALITY_FINE,
	ISP_JPEG_QUALITY_NORMAL,
	ISP_JPEG_QUALITY_ECONOMIC,
	ISP_JPEG_QUALITY_MAX
} JPEG_QUALITY;

typedef enum {
	ISP_ROTATE_NONE,
	ISP_RATATE_90,
	ISP_ROTATE_180,
	ISP_RORATE_270,
	ISP_ROTATE_MIRROR,
	ISP_ROTATE_FLIP,
	ISP_ROTATE_MIRRFLIP,
	ISP_ROTATE_MAX
} ROTATE_TYPE;

typedef enum{
	ISP_AE_MODE_AUTO,
	ISP_AE_MODE_MANUAL,
	ISP_AE_MODE_ISO,
	ISP_AE_MODE_SUTTER,
	ISP_AE_MODE_MAX
} EXPOSURE_MODE_TYPE;

typedef enum{
	ISP_AE_WINTYPE_AVERAGE,
	ISP_AE_WINTYPE_CENTER,
	ISP_AE_WINTYPE_SPOT,
	ISP_AE_WINTYPE_MATRIX,
	ISP_AE_WINTYPE_MAX
} AE_WINTYPE;

typedef enum{
	ISP_AE_ISO_AUTO,
	ISP_AE_ISO_50,
	ISP_AE_ISO_100,
	ISP_AE_ISO_200,
	ISP_AE_ISO_400,
	ISP_AE_ISO_800,
	ISP_AE_ISO_1600,
	ISP_AE_ISO_3200,
	ISP_AE_ISO_MAX
} AE_ISO;

typedef enum{
	ISP_AWB_MODE_BYPASS,
	ISP_AWB_MODE_AUTO,
	ISP_AWB_MODE_CLOU,
	ISP_AWB_MODE_DAYLIGHT,
	ISP_AWB_MODE_COOLWHITE,
	ISP_AWB_MODE_ALIGHT,
	ISP_AWB_MODE_FLOURSENT,
	ISP_AWB_MODE_EFFECT,
	ISP_AWB_MODE_DAWN,
	ISP_AWB_MODE_SUNSET,
	ISP_AWB_MODE_MAX
} AWB_MODE;

typedef enum{
	ISP_AF_NONE,
	ISP_AF_MODE_MANUAL,
	ISP_AF_MODE_AUTO,
	ISP_AF_MODE_MACRO,
	ISP_AF_MODE_PanFocus,
	ISP_AF_MODE_NONE
} AF_MODE_TYPE;


typedef enum{
	ISP_AF_RESULT_MOVING,
	ISP_AF_RESULT_READY,
	ISP_AF_RESULT_FOCUSED,
	ISP_AF_RESULT_MISSED,
	ISP_AF_RESULT_MAX
} AF_STATUS_RESULT;

typedef enum{
	ISP_SCENE_MODE_AUTO,
	ISP_SCENE_MODE_PORTRAIT,
	ISP_SCENE_MODE_LAMDSCAPE,
	ISP_SCENE_MODE_SPORTS,
	ISP_SCENE_MODE_SUNSET,
	ISP_SCENE_MODE_DUSK,
	ISP_SCENE_MODE_DAWN,
	ISP_SCENE_MODE_NIGHT_SHOT,
	ISP_SCENE_MODE_AGAINST_LIGHT,
	ISP_SCENE_MODE_TEXT,
	ISP_SCENE_MODE_MANUAL,
	ISP_SCENE_MODE_INDOOR,
	ISP_SCENE_MODE_SNOW,
	ISP_SCENE_MODE_FALL,
	ISP_SCENE_MODE_WAVE,
	ISP_SCENE_MODE_FIREWORKS,
	ISP_SCENE_MODE_SHOW_WIN,
	ISP_SCENE_MODE_CANDLE,
	ISP_SCENE_MODE_NONE,
	ISP_SCENE_MODE_MAX
} SCENE_MODE;

typedef enum{
	ISP_IMAGE_EFFECT_NORMAL,
	ISP_IMAGE_EFFECT_GRAY,
	ISP_IMAGE_EFFECT_SEPIA,
	ISP_IMAGE_EFFECT_NEGATIVE,
	ISP_IMAGE_EFFECT_ANTIQUE,
	ISP_IMAGE_EFFECT_WATERCOLOR,
	ISP_IMAGE_EFFECT_PORTRAIT,		//?
	ISP_IMAGE_EFFECT_LANDSCAPE,		//?
	ISP_IMAGE_EFFECT_SUNSET,
	ISP_IMAGE_EFFECT_DUSK,
	ISP_IMAGE_EFFECT_DAWN,
	ISP_IMAGE_EFFECT_RED,
	ISP_IMAGE_EFFECT_GREEN,
	ISP_IMAGE_EFFECT_BLUE,
	ISP_IMAGE_EFFECT_YELLOW,
	ISP_IMAGE_EFFECT_EMBOSS,
	ISP_IMAGE_EFFECT_OIL,
	ISP_IMAGE_EFFECT_BW,
	ISP_IMAGE_EFFECT_SKETCH,
	ISP_IMAGE_EFFECT_MAX
} IMAGE_EFFECT;

typedef enum{
	ISP_FLICKER_MODE_OFF,
	ISP_FLICKER_MODE_AUTO,
	ISP_FLICKER_MODE_60HZ,
	ISP_FLICKER_MODE_50HZ,
	ISP_FLICKER_MODE_MAX
} FLICKER_MODE;

typedef enum{
	ISP_LOW_LIGHT_CONDITION,
	ISP_MED_LIGHT_CONDITION,
	ISP_BRIGHT_LIGHT_CONDITION,
	ISP_LIGHT_MAX
} LIGHT_COND;

typedef enum{
	ISP_LIGHT_TEMP_7000K,
	ISP_LIGHT_TEMP_6500K,
	ISP_LIGHT_TEMP_5000K,
	ISP_LIGHT_TEMP_4000K,
	ISP_LIGHT_TEMP_3000K,
	ISP_LIGHT_TEMP_2000K,
	ISP_LIGHT_TEMP_MAX
} LIGHT_TEMP;

typedef enum{
	ISP_PRE_FLASH_OFF,
	ISP_PRE_FLASH_ON,
	ISP_POST_FLASH_OFF,
	ISP_POST_FLASH_ON,
	ISP_FLASH_MAX
} FLASH_CTRL_TYPE;

typedef enum{
	ISP_FD_STATUS_LOCK,
	ISP_FD_STATUS_ENABLE,
	ISP_FD_STATUS_MAX
} FD_STATUS;

typedef enum{
	ISP_ISC_MODE_AUTO,
	ISP_ISC_MODE_ON,
	ISP_ISC_MODE_OFF,
	ISP_ISC_MODE_MAX
} ISC_MODE;

typedef struct ISPJpegInfo {
	unsigned 	size;
	unsigned char *addr;
	unsigned 	tickcount;
	unsigned 	framenum;
	unsigned 	flags;
	unsigned 	idx;
	unsigned	iso;
	unsigned	exposure;
	unsigned	wb;
} ISPJpegInfo;
/*******************************************************************************
	ISP basic preview/capture API
********************************************************************************/
extern uint8 ISP_Init(void);
extern uint8 ISP_Exit(void);
extern void ISP_SetInterrupt();  //
extern void ISP_ClearInterrupt();

extern uint8 ISP_CheckStatusForCmd(void);	// ????????????????????

//extern uint8 ISP_InitalizeSensor(void);
//extern uint8 ISP_SetSensor_OutputResolution(uint16 Hsze, uint16 Vsize);
//extern uint8 ISP_SetSensorMirrorFlip(sensor_mirrorflip_type sensor_mirrorflip);

//extern uint8 ISP_SetPreview_FPS(FPS_TYPE fps);
extern uint8 ISP_SetPreview_Control(uint8 status);

extern uint8 ISP_SetCapture_Mode(CAPTURE_MODE_TYPE mode);
extern uint8 ISP_SetCapture_MultiShotCount(uint8 count);
extern uint8 ISP_Capture_Shooting(int mode);

extern uint8 ISP_Stop_Irq(void);

//extern uint8 ISP_SetCapture_Control(uint8 status);
//extern uint8 ISP_SetCapture_Format(FORMAT format);

extern uint8 ISP_SetPreview_Window(uint16 sx, uint16 sy, uint16 width, uint16 height);  //
extern uint8 ISP_SetCapture_Window(uint16 sx, uint16 sy, uint16 width, uint16 height);

extern uint8 ISP_SetPreviewH_Resolution(uint16 width, uint16 height);
extern uint8 ISP_SetPreviewH_Format(FORMAT format);
extern uint8 ISP_SetPreviewH_StartAddress(uint32 *y, uint32 *u, uint32 *v);
extern uint8 ISP_SetPreviewH_Size (uint32 szy, uint32 szu, uint32 szv);

extern uint8 ISP_SetPreview_Resolution(uint16 width, uint16 height);
extern uint8 ISP_SetPreview_Format(FORMAT format);
extern uint8 ISP_SetPreview_StartAddress(uint32 *y, uint32 *u, uint32 *v);
extern uint8 ISP_SetPreview_Size (uint32 szy, uint32 szu, uint32 szv);

extern uint8 ISP_SetCapture_MainResolution(uint16 width, uint16 height);
extern uint8 ISP_SetCapture_MainFormat(FORMAT format);
extern uint8 ISP_SetCapture_MainImageStartAddress(uint32 *y, uint32 *u, uint32 *v); 
extern uint8 ISP_SetCapture_MainImageSize (uint32 szy, uint32 szu, uint32 szv);

extern uint8 ISP_SetCapture_ThumbResolution(uint16 width, uint16 height);
extern uint8 ISP_SetCapture_ThumbFormat(FORMAT format);
extern uint8 ISP_SetCapture_ThumbImageStartAddress(uint32 *y, uint32 *u, uint32 *v);
extern uint8 ISP_SetCapture_ThumbImageSize (uint32 szy, uint32 szu, uint32 szv);

extern uint32 ISP_GetCapture_MainImage_Length(void); //
extern uint32 ISP_GetCapture_ThumbImage_Length(void);  //

//extern uint8 ISP_SetPreview_Overlay(uint8 onoff, FORMAT type, uint32 chroma_value, uint32 sx, uint32 sy, uint32 widht, uint32 height, uint8 ext_type);

extern uint8 ISP_SetJpegQuality(JPEG_QUALITY quality);  //
extern uint8 ISP_SetJpeg_ExifData(void* raw_data);
extern uint8 ISP_getJpeg_ExifData(ISPJpegInfo* jpg_info);
//extern uint8 ISP_getJpeg_ExifData(void* raw_data);

//extern uint8 ISP_SetRollingCapture_JumpingLine(uint32 line_pixels);
//extern uint8 ISP_SetRollingCapture_InterruptLine(uint32 line_int);

/*****************************************************************
  ISP COMMON LIST  
*****************************************************************/  
extern uint8 ISP_SetSyncPolarity(uint8 Hsync, uint8 Vsync);  // 

extern uint8 ISP_SetZoom(uint16 zoom);
extern uint8 ISP_SetRotate(ROTATE_TYPE Rotate_type);

extern uint8 ISP_SetImg_Effect(IMAGE_EFFECT effectID);
extern uint8 ISP_SetSharpness(uint8 level);
extern uint8 ISP_SetSmooth(uint8 level);
extern uint8 ISP_SetSaturation(uint8 level);
extern uint8 ISP_SetContrast(uint8 level);
extern uint8 ISP_SetPreview_Zoom(uint16 zoom);
extern uint8 ISP_SetCapture_Zoom(uint16 zoom);
extern uint8 ISP_SetPreview_MakeZoomTable(uint16 stepx, uint16 stepy, uint16 width, uint16 height, uint16 number);
extern uint8 ISP_SetCapture_MakeZoomTable(uint16 stepx, uint16 stepy, uint16 width, uint16 height, uint16 number);
//extern uint8 ISP_SetPreview_Zoom_Apply (int mode);
//extern uint8 ISP_SetCapture_Zoom_Apply (int mode);
extern uint8 ISP_Zoom_Apply (int mode);
extern uint8 ISP_SetImg_Effect(IMAGE_EFFECT effectID);

/*********************************************************************
  ISP 3A LIST
**********************************************************************/

extern uint8 ISP_SetExposure_Mode(EXPOSURE_MODE_TYPE mode);
extern uint8 ISP_SetAutoExposure_Windowtype(AE_WINTYPE type);
extern uint8 ISP_SetISO(AE_ISO iso);
extern uint8 ISP_SetAWB(AWB_MODE mode);
extern uint8 ISP_AE_AWB_Lock(uint8 AE_enable, uint8 AWB_enable);
extern uint8 ISP_SetAutoFocus_Mode(AF_MODE_TYPE mode);
extern uint8 ISP_SetAutoFocus_Control(uint8 ctrl_cmd);
extern AF_STATUS_RESULT ISP_GetAutoFocus_Status(void);
extern uint8 ISP_SetSceneMode(SCENE_MODE scene);
extern LIGHT_COND ISP_GetLowlight_Condition(void);
extern uint8 ISP_SetColorTemperature(LIGHT_TEMP Temp);
extern LIGHT_TEMP ISP_GetColorTemperature(void);

extern uint8 ISP_SetWDR(uint8 enable);
extern uint8 ISP_SetFlickerMode(FLICKER_MODE mode);
extern void ISP_SetNoiseReduction(uint8 onoff);
extern void ISP_SetHistogram(uint8 onoff);
extern void ISP_GetHistogram_values(uint8 * value[]);
extern uint8 ISP_SetFlashControl(FLASH_CTRL_TYPE flash_ctrl);

/*****************************************************************
	ISP External API
******************************************************************/
extern uint8 ISP_SetFaceDetection(FD_STATUS status);
extern uint8 ISP_SetImageStabilizer_Mode(ISC_MODE mode);
extern uint8 ISP_SetImageStabilizer_Count(uint8 count);



///////////////////////////////////////////////////
// ISP Process
///////////////////////////////////////////////////

/**
 * MASK_ISPINT_OUT_FRAME_END event 발생시 수행되는 함수
 * event발생후 가능한 빨리 호출해야 한다.
 * @return error code. 
 */
int ISP_Process(void);

///////////////////////////////////////////////////
// Event handler
///////////////////////////////////////////////////
            
// ISP interrupt의해 불리는 함수. 초기화시 연결요
/**
 * ISP block(ISP)에서 인터럽트 발생시 호출 
 * system 초기화시 인터럽트 등록과정이 필요함
 */
void ISP_EventHandler1(void);	
int ISP_EventHandler1_getEvent(unsigned event);
/**
 * ISP block(MI)에서 인터럽트 발생시 호출 
 * system 초기화시 인터럽트 등록과정이 필요함
 */
void ISP_EventHandler2(void);
int ISP_EventHandler2_getEvent(unsigned event);
/**
 * ISP block(JPE)에서 인터럽트 발생시 호출 
 * system 초기화시 인터럽트 등록과정이 필요함
 */
void ISP_EventHandler3(void);
int ISP_EventHandler3_getEvent(unsigned event);
/**
 * ISP block(MIPI)에서 인터럽트 발생시 호출 
 * system 초기화시 인터럽트 등록과정이 필요함
 */
void ISP_EventHandler4(void);
int ISP_EventHandler4_getEvent(unsigned event);

int ISP_CheckInterrupt(void);

#endif
#endif // #ifdef NUCLEUS_OS


#endif   // #ifndef __ISP_INTERFACE_h__

