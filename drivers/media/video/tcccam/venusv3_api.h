/*************************************************************************************************/
/* AIT ISP Host API                                                                              */
/* All rights reserved by Alpah Image Technology Corp                                            */
/*-----------------------------------------------------------------------------------------------*/
/* File name: VenusV3_API.h                                                                      */
/* Description: ISP Host API hardware dependent layer                                            */
/*                                                                                               */
/* Version 0.0A   20090326                                                                       */
/*************************************************************************************************/

#ifndef _VENUSV3_API_H_
#define _VENUSV3_API_H_


#include "venus_project.h"


/*************************************************************************************************/
/* Venus : Definition                                                                            */
/*************************************************************************************************/
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_FWDN_EN)
#define V3_FORCE_TO_DOWNLOAD_FW				1
#define V3_FORCE_TO_SKIP_DOWNLOAD_FW		0

#define V3_FORCE_TO_SKIP_FW_BURNING			0
#else
#define V3_FORCE_TO_DOWNLOAD_FW				0
#define V3_FORCE_TO_SKIP_DOWNLOAD_FW		1

#define V3_FORCE_TO_SKIP_FW_BURNING			1
#endif
// for FW downloading
#define V3_FW_DOWNLOAD_SIZE_PER_PART		0x80 // 0x1000 // (4KB)
#define V3_FW_DOWNLOAD_PARTS_PER_BANK		(0x8000 / V3_FW_DOWNLOAD_SIZE_PER_PART) // (32KB / 4KB = 8)
#define V3_FW_DOWNLOAD_DELAY_PER_PART		1
#define V3_FW_DOWNLOAD_CHECK				1

// for FW updating
#define V3_FW_UPDATE_SIZE_PER_PART			0x80 // (4KB)
#define V3_FW_UPDATE_PARTS_PER_BANK			(0x8000 / V3_FW_DOWNLOAD_SIZE_PER_PART) // (32KB / 4KB = 8)
#define V3_FW_UPDATE_DELAY_PER_PART			1
#define V3_FW_UPDATE_CHECK					1

#define V3_SAVE_REARRANGED_FW				0

#define V3_FW_FLASH_RW_RECHECK_CNT			50

#define V3_TIMEOUT_CNT						200

#define V3_FW_SIZE							0x20000 // (128KB)


/*************************************************************************************************/
/* Venus : Extern Variables                                                                      */
/*************************************************************************************************/
// for preview
extern uint8 V3_NeedInitSensor;

// for firmware download & update
extern uint8 *V3_FirmwareBinPtr;
extern uint32 V3_FirmwareBinSize;


/*************************************************************************************************/
/* Venus : Sample Functions                                                                      */
/*************************************************************************************************/
uint8 V3A_ChipEnable(void);
uint8 V3A_ChipReset(void);

uint8 V3A_PowerOn(void);
uint8 V3A_PowerOff(void);
uint8 V3A_PLLOn(void);
uint8 V3A_PowerUp(void);

uint8 V3A_CheckFirmwareVersion(void);
uint8 V3A_CheckFirmwareUpdate(void);
uint8 V3A_FirmwareStart(void);
uint8 V3A_UpdateFirmware(void);
uint8 V3A_DownloadFirmware(void);
uint8 VenusV3_ReadFirmwareFile(void);
uint8 VenusV3_FreeFirmwareMemory(void);


/*************************************************************************************************/
/* Venus : Memory                                                                                */
/*************************************************************************************************/
uint8 V3A_ReadMemAddr(uint32 addr);
uint8 V3A_WriteMemAddr(uint32 addr, uint8 value);
uint8 AccessTest(void);

#endif // _VENUSV3_API_H_

