/*************************************************************************************************/
/* AIT ISP Host API                                                                              */
/* All rights reserved by Alpah Image Technology Corp                                            */
/*-----------------------------------------------------------------------------------------------*/
/* File name: Venus_API.h                                                                        */
/* Description: ISP Host API hardware independent layer                                          */
/*                                                                                               */
/* Version 0.0A   20090326                                                                       */
/*************************************************************************************************/

#ifndef _VENUS_API_H_
#define _VENUS_API_H_


#include "venus_project.h"


/*****************************************************************************/
/* Host Base Address                                                         */
/*****************************************************************************/
#if AIT838_TEST_API
	#define	HOST_BASE_ADDR						(0x6530)
#elif AIT848_TEST_API
	#define	HOST_BASE_ADDR						(0x65E0)
#endif


/*****************************************************************************/
/* Host Parameter Register Index                                             */
/*****************************************************************************/
#define	HOST_PARAMETER_0					(HOST_BASE_ADDR + 0x10)
#define	HOST_PARAMETER_1					(HOST_BASE_ADDR + 0x11)
#define	HOST_PARAMETER_2					(HOST_BASE_ADDR + 0x12)
#define	HOST_PARAMETER_3					(HOST_BASE_ADDR + 0x13)
#define	HOST_PARAMETER_4					(HOST_BASE_ADDR + 0x14)
#define	HOST_PARAMETER_5					(HOST_BASE_ADDR + 0x15)
#define	HOST_PARAMETER_6					(HOST_BASE_ADDR + 0x16)
#define	HOST_PARAMETER_7					(HOST_BASE_ADDR + 0x17)
#define	HOST_PARAMETER_8					(HOST_BASE_ADDR + 0x18)
#define	HOST_PARAMETER_9					(HOST_BASE_ADDR + 0x19)
#define	HOST_PARAMETER_A					(HOST_BASE_ADDR + 0x1A)
#define	HOST_PARAMETER_B					(HOST_BASE_ADDR + 0x1B)
#define	HOST_PARAMETER_C					(HOST_BASE_ADDR + 0x1C)
#define	HOST_PARAMETER_D					(HOST_BASE_ADDR + 0x1D)

/*****************************************************************************/
/* Host Command & Status Register Index                                      */
/*****************************************************************************/
#define HOST_CMD_ADDR0						(HOST_BASE_ADDR + 0x08)
#define HOST_CMD_ADDR1						(HOST_BASE_ADDR + 0x09)
#define HOST_STATUS_ADDR					(HOST_BASE_ADDR + 0x1E)

#define HOST_STATUS_READY					(0x10)
#define VENUS_CMD_TAIL						(0xE0)



/*************************************************************************************************/
/* Typedefine                                                                                    */
/*************************************************************************************************/
#define VA_PREVIEW_STATUS_STOP				0 //VENUS_PREVIEW_STATUS_STOP
#define VA_PREVIEW_STATUS_START				1 //VENUS_PREVIEW_STATUS_START

#define VA_ISP_AUTO_FOCUS_STATUS_STOP		0 //VENUS_ISP_AUTO_FOCUS_STATUS_STOP
#define VA_ISP_AUTO_FOCUS_STATUS_START1		1 //VENUS_ISP_AUTO_FOCUS_STATUS_START1
#define VA_ISP_AUTO_FOCUS_STATUS_START2		2 //VENUS_ISP_AUTO_FOCUS_STATUS_START2

typedef enum {
	VENUS_COLOR_FORMAT_YUV422,
	VENUS_COLOR_FORMAT_YUV444,
	VENUS_COLOR_FORMAT_RGB888,
	VENUS_COLOR_FORMAT_RGB555,
	VENUS_COLOR_FORMAT_RGB565,
	VENUS_COLOR_FORMAT_RGB444,
	VENUS_COLOR_FORMAT_BAYERRAW
} VENUS_COLOR_FORMAT;

// new enumeration (to support different thumbnail and main image type)
typedef enum {
	VENUS_SNAPSHOT_FORMAT_JPEG				= 0,
	VENUS_SNAPSHOT_FORMAT_YUV422			= 1,
	VENUS_SNAPSHOT_FORMAT_YUV420			= 2,
	VENUS_SNAPSHOT_FORMAT_RGB565			= 3,
	VENUS_SNAPSHOT_FORMAT_BAYER				= 4
} VENUS_SNAPSHOT_FORMAT;


/*****************************************************************************/
/* AE Mode Definition                                                        */
/*****************************************************************************/
#define AE_MODE_AUTO						0
#define AE_MODE_MANUAL						1
#define AE_MODE_ISO							2
#define AE_MODE_SHUTTER						3


/*****************************************************************************/
/* AE ISO Definition                                                         */
/*****************************************************************************/
// modified enumeration (to support different ISO settings)
typedef enum {
	AE_ISO_AUTO								= 0,
	AE_ISO_50								= 1,
	AE_ISO_100								= 2,
	AE_ISO_200								= 3,
	AE_ISO_400								= 4,
	AE_ISO_800								= 5,
	AE_ISO_1600								= 6,
	AE_ISO_3200								= 7
} VENUS_AE_ISO;

/*****************************************************************************/
/* AE Window Type Definition                                                 */
/*****************************************************************************/
typedef enum {
	VENUS_AE_WINTYPE_AVERAGE				= 0,
	VENUS_AE_WINTYPE_CENTER					= 1,
	VENUS_AE_WINTYPE_SPOT					= 2,
	VENUS_AE_WINTYPE_MATRIX					= 3
} VENUS_AE_WINTYPE;

typedef enum {
	VENUS_AF_RESULT_MOVING,
	VENUS_AF_RESULT_READY,
	VENUS_AF_RESULT_FOCUSED,
	VENUS_AF_RESULT_MISSED
} VENUS_AF_STATUS_RESULT;


/*****************************************************************************/
/* AWB Mode Definition                                                       */
/*****************************************************************************/
// new enumeration (to support different manual white balance type)
typedef enum {
	AWB_MODE_BYPASS							= 0,
	AWB_MODE_AUTO							= 1,
	AWB_MODE_CLOUDY							= 2,
	AWB_MODE_DAYLIGHT						= 3,
	AWB_MODE_COOLWHITE						= 4,
	AWB_MODE_ALIGHT							= 5,
	AWB_MODE_FLUORSENT						= 6,
	AWB_MODE_EFFECT							= 7,
	AWB_MODE_DAWN							= 8,
	AWB_MODE_SUNSET							= 9
} VENUS_AWB_MODE;


/*************************************************************************************************/
/* Venus Preview Definition                                                                      */
/*************************************************************************************************/
typedef enum {
	VENUS_PREVIEW_STATUS_STOP,
	VENUS_PREVIEW_STATUS_START
} VENUS_PREVIEW_STATUS;


typedef enum {
	VENUS_ISC_MODE_AUTO						= 0,
	VENUS_ISC_MODE_ON						= 1,
	VENUS_ISC_MODE_OFF						= 2
} VENUS_ISC_MODE;

typedef enum {
	VENUS_FLICKER_MODE_OFF					= 0,
	VENUS_FLICKER_MODE_AUTO					= 1,
	VENUS_FLICKER_MODE_60HZ					= 2,
	VENUS_FLICKER_MODE_50HZ					= 3
} VENUS_FLICKER_MODE;

typedef enum {
	VENUS_AF_MODE_AUTO						= 0,
	VENUS_AF_MODE_MANUAL					= 1,
	VENUS_AF_MODE_MACRO						= 2
} VENUS_AF_MODE;

/*****************************************************************************/
/* Image Effect                                                              */
/*****************************************************************************/
typedef enum {
	VENUS_IMAGE_EFFECT_NORMAL				= 0,
	VENUS_IMAGE_EFFECT_GREY					= 1,
	VENUS_IMAGE_EFFECT_SEPIA				= 2,
	VENUS_IMAGE_EFFECT_NEGATIVE				= 3,
	VENUS_IMAGE_EFFECT_ANTIQUE				= 4,
	VENUS_IMAGE_EFFECT_WATERCOLOR			= 5,
	VENUS_IMAGE_EFFECT_PORTRAIT				= 6,
	VENUS_IMAGE_EFFECT_LANDSCAPE			= 7,
	VENUS_IMAGE_EFFECT_SUNSET				= 8,
	VENUS_IMAGE_EFFECT_DUSK					= 9,
	VENUS_IMAGE_EFFECT_DAWN					= 10,
	// modified enumeration (to support more image effects)
	VENUS_IMAGE_EFFECT_RED					= 11,
	VENUS_IMAGE_EFFECT_GREEN				= 12,
	VENUS_IMAGE_EFFECT_BLUE					= 13,
	VENUS_IMAGE_EFFECT_YELLOW				= 15,
	VENUS_IMAGE_EFFECT_EMBOSS				= 17,
	VENUS_IMAGE_EFFECT_OIL					= 18,
	VENUS_IMAGE_EFFECT_BW					= 19,
	VENUS_IMAGE_EFFECT_SKETCH				= 20,
	VENUS_IMAGE_EFFECT_CRAYONE				= 21,
	VENUS_IMAGE_EFFECT_WHITEBOARD			= 22,
	VENUS_IMAGE_EFFECT_BLACKBOARD			= 23,
	VENUS_IMAGE_EFFECT_VIVID				= 24
} VENUS_IMAGE_EFFECT;

typedef enum {
	VENUS_ISP_WIDE_DYNAMIC_RANGE_STATUS_ENABLE,
	VENUS_ISP_WIDE_DYNAMIC_RANGE_STATUS_DISABLE
} VENUS_ISP_WIDE_DYNAMIC_RANGE_STATUS;

typedef enum {
	VENUS_LIGHT_COND_LOW					= 0,
	VENUS_LIGHT_COND_MED,
	VENUS_LIGHT_COND_BRIGHT
} VENUS_LIGHT_COND;

typedef enum {
	VENUS_AE_FPS_AUTO					= 0,
	VENUS_AE_FPS_30						= 1,
	VENUS_AE_FPS_15						= 2,
	VENUS_AE_FPS_12						= 3,
	VENUS_AE_FPS_7_5					= 4,
	VENUS_AE_FPS_7						= 5,
	VENUS_AE_FPS_5						= 6
} VENUS_AE_FPS;

typedef enum {
	VENUS_LIGHT_TEMP_7000K,
	VENUS_LIGHT_TEMP_6500K,
	VENUS_LIGHT_TEMP_5000K,
	VENUS_LIGHT_TEMP_4000K,
	VENUS_LIGHT_TEMP_3000K,
	VENUS_LIGHT_TEMP_2000K
} VENUS_LIGHT_TEMP;

/*****************************************************************************/
/* Scene Mode                                                                */
/*****************************************************************************/
typedef enum {
	VENUS_SCENE_MODE_AUTO					= 0,
	VENUS_SCENE_MODE_PORTRAIT				= 1,
	VENUS_SCENE_MODE_LANDSCAPE				= 2,
	VENUS_SCENE_MODE_SPORTS					= 3,
	VENUS_SCENE_MODE_SUNSET					= 4,
	VENUS_SCENE_MODE_DUSK					= 5,
	VENUS_SCENE_MODE_DAWN					= 6,
	VENUS_SCENE_MODE_NIGHT_SHOT				= 7,
	VENUS_SCENE_MODE_AGAINST_LIGHT			= 8,
	VENUS_SCENE_MODE_TEXT					= 9,
	VENUS_SCENE_MODE_MANUAL				= 10,
	// modified enumeration (to support more scene modes)
	VENUS_SCENE_MODE_INDOOR				= 11,
	VENUS_SCENE_MODE_SNOW				= 12,
	VENUS_SCENE_MODE_FALL				= 13,
	VENUS_SCENE_MODE_WAVE				= 14,
	VENUS_SCENE_MODE_FIREWORKS			= 15,
	VENUS_SCENE_MODE_SHOW_WIN			= 16,
	VENUS_SCENE_MODE_CANDLE				= 17,
	VENUS_SCENE_MODE_NONE				= 18
} VENUS_SCENE_MODE;

typedef enum {
	VENUS_SNAPSHOT_MODE_JPEG				= 0,
	VENUS_SNAPSHOT_MODE_RAW_YCBCR			= 1,
	VENUS_SNAPSHOT_MODE_RAW_RGB565			= 2, // not used
	VENUS_SNAPSHOT_MODE_RAW_BAYER			= 3,
	VENUS_SNAPSHOT_MODE_THUMBNAIL			= 4, // not used
	VENUS_SNAPSHOT_MODE_IMAGE_STABILIZED	= 5, // not used
	VENUS_SNAPSHOT_MODE_MULTI_SHOT			= 6, // not used
	VENUS_SNAPSHOT_MODE_RAWPROC_FRAME		= 7,
	VENUS_SNAPSHOT_MODE_GRAPHIC_FRAME		= 8
} VENUS_SNAPSHOT_MODE;

typedef enum {
	VENUS_SNAPSHOT_DESTINATION_FB			= 0,
	VENUS_SNAPSHOT_DESTINATION_EXT_MEM		= 1
} VENUS_SNAPSHOT_DESTINATION;

typedef enum {
	VENUS_FD_STATUS_LOCK					= 68,
	VENUS_FD_STATUS_ENABLE					= 4
} VENUS_FD_STATUS;

typedef enum {
	VENUS_SNAPSHOT_QUALITY_SUPERFINE		= 0,
	VENUS_SNAPSHOT_QUALITY_FINE				= 1,
	VENUS_SNAPSHOT_QUALITY_NORMAL				= 2,
	VENUS_SNAPSHOT_QUALITY_ECONOMIC				= 3
} VENUS_SNAPSHOT_QUALITY;

typedef enum {
//	VENUS_PREVIEW_FORMAT_YUV444			= 0,     // Not support yet
	VENUS_PREVIEW_FORMAT_YUV422			= 1,
	VENUS_PREVIEW_FORMAT_YUV420			= 2,     // Not support yet
//	VENUS_PREVIEW_FORMAT_RGB888			= 3,     // Not support yet
	VENUS_PREVIEW_FORMAT_RGB565			= 4 
//	VENUS_PREVIEW_FORMAT_BAYER			= 5         // Not support yet
} VENUS_PREVIEW_FORMAT;

/*****************************************************************************/
/* Venus Sensor Resolution Definition                                        */
/*****************************************************************************/
#define SENSOR_RESOL_NULL					(0x00)
#define	SENSOR_RESOL_QVGA_320x240			(0x02)	// 320 * 240
#define	SENSOR_RESOL_VGA_512x384			(0x03)	// 512 * 384
#define	SENSOR_RESOL_VGA_640x480			(0x04)	// 640 * 480
#define	SENSOR_RESOL_SVGA_800x600			(0x06)	// 800 * 600
#define	SENSOR_RESOL_XGA_1024x768			(0x08)	// 1024* 768
#define	SENSOR_RESOL_SXGA_1280x1024			(0x0A)	// 1280 * 1024 // 1.3M
#define SENSOR_RESOL_SXGA_1280x960			(0x0B)	// 1280 * 960
#define	SENSOR_RESOL_UXGA_1600x1200			(0x0C)	// 1600 * 1200 // 2M
#define	SENSOR_RESOL_QXGA_2048x1536			(0x0E)	// 2048 * 1536 // 3M
#define	SENSOR_RESOL_QSXGA_2560x1920		(0x10)	// 2560 * 2048 // 5M
#define	SENSOR_RESOL_QSXGA_2560x2048		(0x11)	// 2560 * 2048 // 5M
#define	SENSOR_RESOL_QUXGA_3264x2448		(0x12)	// 3264 * 2448 // 8M
#define   SENSOR_RESOL_SXGA_1280x720			(0x13)	// 1280 * 960

/*************************************************************************************************/
/* Venus : Check if ISP is ready for command                                                     */
/*************************************************************************************************/
uint8 VA_CheckReadyForCmd(void);

/*************************************************************************************************/
/* Venus : Enable/Disable Face Detection                                                         */
/*************************************************************************************************/
uint8 VA_FaceDetectionEnable(void);
uint8 VA_FaceDetectionDisable(void);
uint8 VA_FaceDetectionSetUnlock(void);

/*************************************************************************************************/
/* Venus : Zoom Settings                                                                         */
/*************************************************************************************************/
uint8 VA_SetZoom(uint16 x);

/*************************************************************************************************/
/* Venus : RawProc Store/Fetch                                                                   */
/*************************************************************************************************/
uint8 VA_SetRawProcStore(uint32 addr);
uint8 VA_SetRawProcFetch(uint32 addr, uint8 type);

/*************************************************************************************************/
/* Venus : Snapshot                                                                              */
/*************************************************************************************************/
uint8 VA_SetSnapshotMode(uint8 snapshot_mode);

uint8 VA_SetSnapshotMainResolution(uint16 width, uint16 height);

uint8 VA_SetSnapshotThumbnailResolution(uint16 width, uint16 height);

// new API (to support different main image formats)
uint8 VA_SetSnapshotMainFormat(VENUS_SNAPSHOT_FORMAT format);

// new API (to support different thumbnail formats)
uint8 VA_SetSnapshotThumbFormat(VENUS_SNAPSHOT_FORMAT format);

uint32 VA_GetSnapshotMainJPEGCompressedSize(void);
uint32 VA_GetSnapshotThumbJPEGCompressedSize(void);
uint32 VA_GetSnapshotMainJPEGStartAddress(void);
uint32 VA_GetSnapshotThumbJPEGStartAddress(void);
uint16 VA_GetLastSnapshotISOSpeed(void);
uint32 VA_GetLastSnapshotShutterSpeed(void);

uint8 VA_SetSnapshotImageStabilizerCount(uint8 is_count);

uint8 VA_SetSnapshotMultiShotCount(uint8 ms_count);

uint8 VA_SetDumpDataLinePixelsViaCCIR(uint32 line_pixels);
uint8 VA_SetDumpDataSpeedViaCCIR(uint8 speed);

uint8 VA_TriggerSnapshot(void);

uint8 VA_SetSnapshotFlashControl(uint8 flash_en);

uint8 VA_PrepareSnapshotDataForCCIR(uint32 thumbnail_offset);
uint8 VA_DumpDataViaCCIR(uint32 start_addr, uint32 size);

/*************************************************************************************************/
/* Venus : Configure Sensor                                                                      */
/*************************************************************************************************/
uint8 VA_InitializeSensor(void);

/*************************************************************************************************/
/* Venus : Start/Stop Preview                                                                    */
/*************************************************************************************************/
uint8 VA_SetPreviewControl(uint8 status);

/*************************************************************************************************/
/* Venus : Configure Preview                                                                     */
/*************************************************************************************************/
uint8 VA_SetPreviewResolution(uint16 width, uint16 height);
uint8 VA_SetPreviewFormat(VENUS_PREVIEW_FORMAT format);
uint8 VA_SetPreviewFPS(VENUS_AE_FPS fps);


/*************************************************************************************************/
/* Venus : Start/Stop ISP Auto Focus                                                             */
/*************************************************************************************************/
uint8 VA_SetISPAutoFocusControl(uint8 status);
uint8 VA_CheckAFStatus(uint8 *status);


uint8 VA_GetFWVersion(uint8 *version);


uint8 VA_SetAEWnd(uint8 type);
uint8 VA_GetLowlightCond(uint8 *status);

uint8 VA_AE_AWB_Lock(uint8 Item);
uint8 VA_AE_AWB_Unlock(uint8 Item);

uint8 VA_SetSharpness(uint8 level);

// new parameter type (to make the API easier to maintain)
uint8 VA_SetISC(VENUS_ISC_MODE enable);

uint8 VA_SetJPEGQuality(VENUS_SNAPSHOT_QUALITY quality);

uint8 VA_SetAETarget(uint8 level);
// new parameter type (to make the API easier to maintain)
uint8 VA_SetAWB(VENUS_AWB_MODE temp);

// new parameter type (to make the API easier to maintain)
uint8 VA_SetImgEffect(VENUS_IMAGE_EFFECT effectID);

uint8 VA_SetSaturation(uint8 sat);
uint8 VA_SetContrast(uint8 cmd);

// new parameter type (to make the API easier to maintain)
uint8 VA_SetISO(VENUS_AE_ISO iso);

// new parameter type (to make the API easier to maintain)
uint8 VA_SetISPSceneMode(VENUS_SCENE_MODE scene);
uint8 VA_GetISCStatus(uint8 *status);
uint8 VA_SetWDR(uint8 enable);
uint8 VA_SetFlickerMode(uint8 mode);

uint8 VA_AFMode(uint8 enable);
uint8 VA_HostEvent(uint16 event);

/*************************************************************************************************/
/* Venus : SPI                                                                                   */
/*************************************************************************************************/
uint32 VA_GetFlashID(void);
uint8 VA_EraseFlash(void);
uint8 VA_ReadDataFromFlash(uint32 dst_addr, uint32 flash_addr, uint32 size);
uint8 VA_WriteDataToFlash(uint32 src_addr, uint32 flash_addr, uint32 size);

/*************************************************************************************************/
/* Venus : Memory                                                                                */
/*************************************************************************************************/
uint32 VA_VerifyExtMem(uint32 mem_size);
uint32 VA_CompareMem(uint32 mem_addr0, uint32 mem_addr1, uint32 mem_size);

/*************************************************************************************************/
/* Venus : Sensor                                                                                */
/*************************************************************************************************/
uint16 VA_GetSensorReg(uint16 sensor_reg_addr);
uint16 VA_SetSensorReg(uint16 sensor_reg_addr, uint16 sensor_reg_value);

uint8 VA_SetSensorResolutionAsPreview(uint8 sensor_resolution);
uint8 VA_SetSensorResolutionAsSnapshot(uint8 sensor_resolution);

uint8 VA_SetSensorMirrorFlip(uint8 sensor_mirrorflip);

/*************************************************************************************************/
/* Venus : Motor                                                                                 */
/*************************************************************************************************/
uint16 VA_GetMotorReg(uint16 motor_reg_addr);
uint16 VA_SetMotorReg(uint16 motor_reg_addr, uint16 motor_reg_value);

/*************************************************************************************************/
/* Venus : Macros                                                                                */
/*************************************************************************************************/
#define VA_MIN(a, b)						(((a) < (b)) ? (a) : (b))
#define VA_MAX(a, b)						(((a) > (b)) ? (a) : (b))
#define VR_ARRSIZE(a)						(sizeof((a)) / sizeof((a[0])))

/*************************************************************************************************/
/* Venus : States                                                                                */
/*************************************************************************************************/
uint8 VA_SetChipStandby(void);
uint8 VA_SetChipStreaming(void);
uint8 VA_SetChipTriState(void);
uint8 VA_SetChipPowerOff(void);
uint8 VA_CheckISPStatus(void);


#endif // _VENUS_API_H_

