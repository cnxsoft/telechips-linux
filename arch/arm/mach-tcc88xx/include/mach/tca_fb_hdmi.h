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

#ifndef __TCA_FB_HDMI_H__
#define __TCA_FB_HDMI_H__

/*****************************************************************************
*
* Defines
*
******************************************************************************/


/*****************************************************************************
*
* Enum
*
******************************************************************************/


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
#ifdef __cplusplus
extern 
"C" { 
#endif
#include <mach/tccfb_ioctrl.h>


extern void TCC_HDMI_LCDC_init(void);

extern char TCC_HDMI_FB_update(unsigned int width, unsigned int height, unsigned int bits_per_pixel, unsigned int addr);
extern void TCC_HDMI_FB_update_sync(void);

extern void TCC_HDMI_DISPLAY_UPDATE(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo);
extern void TCC_TVOUT_LCDC_OnOff(char hdmi_lcdc_num, char onoff);
extern void TCC_HDMI_LCDC_OnOff(char hdmi_lcdc_num, char onoff);

extern unsigned char TCC_HDMI_MSC_Wait_signal_disable(char ch);
extern unsigned char TCC_HDMI_MSC_enable(char ch);


extern void TCC_HDMI_LCDC_Timing(char hdmi_lcdc, struct lcdc_timimg_parms_t *mode);

extern void TCC_HDMI_FB_Set_AlphaBlending(void);

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
void TCC_HDMI_DISPLAY_PROCESS(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo, char force_update);
void TCC_HDMI_DISPLAY_IMAGE(exclusive_ui_update UpdateImage);
#endif

#ifdef __cplusplus
 } 
#endif


#endif	//__TCA_FB_HDMI_H__

