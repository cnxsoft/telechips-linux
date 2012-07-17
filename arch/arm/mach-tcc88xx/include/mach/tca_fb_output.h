/****************************************************************************
 *   FileName    : tca_backlight.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef __TCA_FB_OUTPUT_H__
#define __TCA_FB_OUTPUT_H__

#ifdef __cplusplus
extern 
"C" { 
#endif
#include <mach/tccfb_ioctrl.h>

/*****************************************************************************
*
* Defines
*
******************************************************************************/
#define TCC_FB_OUT_MAX_WIDTH		1920
#define TCC_FB_OUT_MAX_HEIGHT		1080

#if defined(CONFIG_TCC_OUTPUT_STARTER)
	#if defined(CONFIG_TCC_OUTPUT_AUTO_DETECTION)
	#define TCC_OUTPUT_STARTER_AUTO_DETECT
	#else
	#define TCC_OUTPUT_STARTER_DUAL
	//#define TCC_OUTPUT_STARTER_SINGLE
	//#define TCC_OUTPUT_STARTER_AUTO_DETECT
	#endif
#endif

/*****************************************************************************
*
* Enum
*
******************************************************************************/

typedef enum{
	TCC_OUTPUT_LCDC0,
	TCC_OUTPUT_LCDC1,
	TCC_OUTPUT_LCDC_MAX
} TCC_OUTPUT_LCDC;

typedef enum{
	TCC_OUTPUT_NONE,
	TCC_OUTPUT_COMPOSITE,
	TCC_OUTPUT_COMPONENT,
	TCC_OUTPUT_HDMI,
	TCC_OUTPUT_MAX
} TCC_OUTPUT_TYPE;

/*****************************************************************************
*
* Type Defines
*
******************************************************************************/


/*****************************************************************************
*
* Structures
*
******************************************************************************/


/*****************************************************************************
*
* External Variables
*
******************************************************************************/


/*****************************************************************************
*
* External Functions
*
******************************************************************************/

extern void TCC_OUTPUT_LCDC_Init(void);
extern void TCC_OUTPUT_LCDC_OnOff(char output_type, char output_lcdc_num, char onoff);
extern void TCC_OUTPUT_LCDC_CtrlLayer(char output_type, char interlace, char format);
extern void TCC_OUTPUT_LCDC_CtrlChroma(lcdc_chroma_params lcdc_chroma);
extern char TCC_OUTPUT_FB_Update(unsigned int width, unsigned int height, unsigned int bits_per_pixel, unsigned int addr, unsigned int type);
extern void TCC_OUTPUT_FB_UpdateSync(unsigned int type);
extern char TCC_OUTPUT_FB_Update_Video(struct tcc_lcdc_image_update *ImageInfo, unsigned int type);

extern void TCC_OUTPUT_FB_WaitVsyncInterrupt(unsigned int type);
extern int TCC_OUTPUT_SetOutputResizeMode(tcc_display_resize mode);

extern int TCC_OUTPUT_FB_BackupVideoImg(char output_type);
extern int TCC_OUTPUT_FB_RestoreVideoImg(char output_type);

extern int TCC_OUTPUT_FB_Set3DMode(char enable, char mode);
extern int TCC_OUTPUT_FB_Get3DMode(char *mode);

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
extern int TCC_OUTPUT_EXCLUSIVE_UI_Init(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetAddr(unsigned int *pBaseAddr);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetParam(exclusive_ui_params exclusive_ui_param);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetParam(exclusive_ui_params *pExclusive_ui_param);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetEnable(char enable);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetEnable(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetSize(exclusive_ui_size ui_size);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetInterlace(unsigned int interlace);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetInterlace(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetImage(struct tcc_lcdc_image_update *ImageInfo);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Ctrl(unsigned int enable);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetUpdate(char value);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetUpdate(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetRatio(char aspect_ratio);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetRatio(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetAR(unsigned int aspect_ratio_x, unsigned int aspect_ratio_y);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetAR(exclusive_ui_ar_params *aspect_ratio);
extern int TCC_OUTPUT_EXCLUSIVE_UI_CfgAR(char lcdc_num, unsigned int width, unsigned int height, exclusive_ui_ar_params *aspect_ratio);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetBypass(char bypass);
extern int TCC_OUTPUT_EXCLUSIVE_UI_GetBypass(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_ViqeMtoM(char lcdc_num, unsigned int width, unsigned int height);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Restore_Viqe(struct tcc_lcdc_image_update *ImageInfo, char lcdc_num, unsigned int width, unsigned int height, char index);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Restore_Mixer(struct tcc_lcdc_image_update *ImageInfo, char lcdc_num, unsigned int width, unsigned int height, char index);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Update(exclusive_ui_frame ui_frame);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Update_Force(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Update_Mixer(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Clear(void);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Clear_Force(char clear_force);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Scaler(char lcdc_num, char scaler_num, char OnOff);
extern int TCC_OUTPUT_EXCLUSIVE_UI_SetOnTheFly_Viqe(char lcdc_num, char scaler_num, char OnOff);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Direct(char lcdc_num, char enable);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Mixer(struct tcc_lcdc_image_update *ImageInfo, char index, char lastflag);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Scaler(struct tcc_lcdc_image_update *ImageInfo, char scaler_num, unsigned int width, unsigned int height, char index, char lastflag);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Scaler_Viqe(struct tcc_lcdc_image_update *ImageInfo, unsigned int width, unsigned int height, char index, char lcdc_num);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(struct tcc_lcdc_image_update *ImageInfo, char lcdc_num);
extern int TCC_OUTPUT_EXCLUSIVE_UI_Process_Deinterlace(struct tcc_lcdc_image_update *ImageInfo, char force_update);
#endif //CONFIG_TCC_EXCLUSIVE_UI_LAYER

extern void TCC_OUTPUT_FB_Update_Disable(unsigned int disable);

extern int TCC_OUTPUT_FB_MouseShow(unsigned int enable, unsigned int type);
extern int TCC_OUTPUT_FB_MouseMove(unsigned int width, unsigned int height, tcc_mouse *mouse, unsigned int type);
extern int TCC_OUTPUT_FB_MouseSetIcon(tcc_mouse_icon *mouse_icon);

#ifdef __cplusplus
 } 
#endif


#endif	//__TCA_FB_HDMI_H__

