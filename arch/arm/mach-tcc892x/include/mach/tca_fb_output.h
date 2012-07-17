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
	#elif defined(CONFIG_TCC_OUTPUT_ATTACH)
	#define TCC_OUTPUT_STARTER_ATTACH
	#else
	#define TCC_OUTPUT_STARTER_SINGLE
	//#define TCC_OUTPUT_STARTER_DUAL
	//#define TCC_OUTPUT_STARTER_AUTO_DETECT
	//#define TCC_OUTPUT_STARTER_ATTACH
	#endif
#endif

#if defined(CONFIG_TCC_OUTPUT_AUTO_DETECTION)
	//#define TCC_OUTPUT_AUTO_ALL
	#define TCC_OUTPUT_AUTO_HDMI_CVBS
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
extern void TCC_OUTPUT_UPDATE_OnOff(char onoff, char type);
extern void TCC_OUTPUT_LCDC_OnOff(char output_type, char output_lcdc_num, char onoff);
extern void TCC_OUTPUT_LCDC_CtrlLayer(char output_type, char interlace, char format);
extern void TCC_OUTPUT_LCDC_CtrlChroma(lcdc_chroma_params lcdc_chroma);
extern void TCC_OUTPUT_LCDC_OutputEnable(char output_lcdc, unsigned int onoff);
extern char TCC_OUTPUT_FB_Update(unsigned int width, unsigned int height, unsigned int bits_per_pixel, unsigned int addr, unsigned int type);
extern void TCC_OUTPUT_FB_UpdateSync(unsigned int type);
extern char TCC_OUTPUT_FB_Update_Video(struct tcc_lcdc_image_update *ImageInfo, unsigned int type);

extern void TCC_OUTPUT_FB_WaitVsyncInterrupt(unsigned int type);
extern int TCC_OUTPUT_SetOutputResizeMode(tcc_display_resize mode);

extern int TCC_OUTPUT_FB_BackupVideoImg(char output_type);
extern int TCC_OUTPUT_FB_RestoreVideoImg(char output_type);

extern void TCC_OUTPUT_FB_Update_Disable(unsigned int disable);

extern int TCC_OUTPUT_FB_MouseShow(unsigned int enable, unsigned int type);
extern int TCC_OUTPUT_FB_MouseMove(unsigned int width, unsigned int height, tcc_mouse *mouse, unsigned int type);
extern int TCC_OUTPUT_FB_MouseSetIcon(tcc_mouse_icon *mouse_icon);
extern int TCC_OUTPUT_FB_Set3DMode(char enable, char mode);
extern int TCC_OUTPUT_FB_Get3DMode(char *mode);
extern void TCC_OUTPUT_FB_Update3D(unsigned int src_wd, unsigned int src_ht, char lcdc_num);

extern void TCC_OUTPUT_FB_AttachOutput(char src_lcdc_num, char output_type, char resolution, char starer_flag);
extern void TCC_OUTPUT_FB_DetachOutput(char disable_all);
extern void TCC_OUTPUT_FB_AttachUpdateFlag(char src_lcdc_num);
extern void TCC_OUTPUT_FB_AttachUpdateScreen(char src_lcdc_num);
extern void TCC_OUTPUT_FB_AttachSetSate(char attach_state);
extern char TCC_OUTPUT_FB_AttachGetSate(void);

#ifdef __cplusplus
 } 
#endif


#endif	//__TCA_FB_HDMI_H__

