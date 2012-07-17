/*
 * drivers/media/video/tcccam/sensor_if.h
 *
 * Copyright (C) 2008 Telechips, Inc. 
 *
 * Sensor interface to TCC camera capture drivers
 * Sensor driver should implement this interface
 *
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 */

#include <linux/videodev2.h>

#ifndef TCC_SENSOR_IF_H
#define TCC_SENSOR_IF_H

#define CAM_CORE 	0
#define CAM_IO 		1
#define CAM_ETC		2 //AF or RAM

//#define TCC_POWER_CONTROL

typedef struct
{
	int (*Open)(bool bChangeCamera);
	int (*Close)(void);
	int (*PowerDown)(bool bChangeCamera);

	int (*Set_Preview)(void);
	int (*Set_Capture)(void);
	int (*Set_CaptureCfg)(int width, int height);

	int	(*Set_Zoom)(int val);
	int (*Set_AF)(int val);
	int (*Set_Effect)(int val);
	int (*Set_Flip)(int val);
	int (*Set_ISO)(int val);
	int (*Set_ME)(int val);
	int (*Set_WB)(int val);
	int (*Set_Bright)(int val);
	int (*Set_Scene)(int val);
	int (*Set_Exposure)(int val);
	int (*Set_FocusMode)(int val);
	
	int (*Check_ESD)(int val);
	int (*Check_Luma)(int val);
}
SENSOR_FUNC_TYPE;

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
enum sensor_reg_width {
	WORD_LEN,
	BYTE_LEN,
	MS_DELAY
};

struct sensor_reg {
	unsigned short reg;
	unsigned short val;
	enum sensor_reg_width width;
};

struct capture_size {
	unsigned long width;
	unsigned long height;
};

typedef struct tcc_sensor_info
{
	int i2c_addr;
	int reg_term;
	int val_term;
	int m_clock;
	int m_clock_source;
	int s_clock;
	int s_clock_source;
	int p_clock_pol;
	int v_sync_pol;
	int h_sync_pol;
#ifdef CONFIG_ARCH_TCC892X
	int de_pol;
#endif
	int format;
	int preview_w;
	int preview_h;
	int preview_zoom_offset_x;
	int preview_zoom_offset_y;
	int capture_w;
	int capture_h;
	int capture_zoom_offset_x;
	int capture_zoom_offset_y;
	int max_zoom_step;
	int cam_capchg_width;
	int capture_skip_frame;
	int framerate;
	struct capture_size *sensor_sizes;
}TCC_SENSOR_INFO_TYPE;

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
#define SENSOR_5M
#define USING_HW_I2C
#include "mt9p111_5mp.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
#define SENSOR_3M
#define USING_HW_I2C
#include "mt9t111_3mp.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_S5K5CAGA)
#define SENSOR_3M
#define USING_HW_I2C
#include "s5k5caga_3mp.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MV9317)
#define SENSOR_3M
#define USING_HW_I2C
#include "mv9317_3mp.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9D112)
#define SENSOR_2M
#define USING_HW_I2C
#include "mt9d112_2mp.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
#define SENSOR_1_3M 
#define USING_HW_I2C 
#include "mt9m113_1.3mp.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_SR130PC10)
#define SENSOR_1_3M 
#define USING_HW_I2C 
#include "sr130pc10_1.3mp.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
#define SENSOR_VGA
#define USING_HW_I2C
#include "ov7690_vga.h"
#endif
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_SIV100B)
#define SENSOR_VGA
#define USING_HW_I2C
#include "siv100b_vga.h"
#endif
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
#define SENSOR_5M
#define USE_SENSOR_ZOOM_IF
#define USE_SENSOR_EFFECT_IF
#include "venus_ait848_5mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
#define SENSOR_5M
#define USING_HW_I2C
#include "mt9p111_5mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
#define SENSOR_3M
#define USING_HW_I2C
#include "mt9t111_3mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
#define SENSOR_3M
#define USING_HW_I2C
#include "mt9t113_3mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_S5K5CAGA)
#define SENSOR_3M
#define USING_HW_I2C
#include "s5k5caga_3mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MV9317)
#define SENSOR_3M
#define USING_HW_I2C
#include "mv9317_3mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9D112)
#define SENSOR_2M
#define USING_HW_I2C
#include "mt9d112_2mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_OV3640)
#define SENSOR_3M
#define USING_HW_I2C
#include "ov3640_3mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_S5K4BAFB)
#define SENSOR_2M
#define USING_HW_I2C
//#define TCC_VCORE_30FPS_CAMERASENSOR
#include "s5k4bafb_2mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_ISX006)
#define SENSOR_5M
#define USE_SENSOR_ZOOM_IF
#define USE_SENSOR_EFFECT_IF
#define USING_HW_I2C
#include "isx006_5mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
#define SENSOR_VGA
#define USING_HW_I2C
#include "ov7690_vga.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_SIV100B)
#define SENSOR_VGA
#define USING_HW_I2C
#include "siv100b_vga.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_GT2005)
#define SENSOR_2M
#define USING_HW_I2C
#include "gt2005_2mp.h"
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
#define SENSOR_1_3M 
#define USING_HW_I2C 
#include "mt9m113_1.3mp.h" 
#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_SR130PC10)
#define SENSOR_1_3M 
#define USING_HW_I2C 
#include "sr130pc10_1.3mp.h" 
#elif defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150)
#define USING_HW_I2C
#define SENSOR_TVP5150
#define SENSOR_BT656
#include "atv/tvp5150.h"
#elif defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
#define USING_HW_I2C
#define SENSOR_RDA5888
#include "atv/rda5888_atv.h"
#elif defined(CONFIG_VIDEO_CAMERA_NEXTCHIP_TEST)
#define SENSOR_3M
#define USING_HW_I2C
#include "nextchip_test.h"
#endif
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

extern int sensor_if_change_mode(unsigned char capture_mode);
extern int sensor_if_adjust_autofocus(void);
extern int sensor_if_query_control(struct v4l2_queryctrl *qc);
extern int sensor_if_get_control(struct v4l2_control *vc);
extern int sensor_if_set_control(struct v4l2_control *vc, unsigned char init);
extern int sensor_if_check_control(void);
extern int sensor_if_set_current_control(void);
extern int sensor_if_enum_pixformat(struct v4l2_fmtdesc *fmt);
extern int sensor_if_configure(struct v4l2_pix_format *pix, unsigned long xclk);	
extern int sensor_if_init(struct v4l2_pix_format *pix);
extern int sensor_if_deinit(void);
extern void sensor_pwr_control(int name, int enable);
#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
extern void sensor_if_set_enable(void);
extern int sensor_if_get_enable(void);
#endif
extern int sensor_if_get_max_resolution(int index);
extern int sensor_if_get_sensor_framerate(int *nFrameRate);
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern void sensor_if_set_facing_front(void);
extern void sensor_if_set_facing_back(void);
extern void sensor_init_func_set_facing_front(SENSOR_FUNC_TYPE *sensor_func);
extern void sensor_init_func_set_facing_back(SENSOR_FUNC_TYPE *sensor_func);
#endif

extern int sensor_isit_change_camera(void);
extern int sensor_if_restart(void);
extern int sensor_if_capture_config(int width, int height);
extern int sensor_if_isESD(void);
extern int sensor_if_cleanup(void);
extern void sensor_delay(int ms);

extern void sensor_turn_on_camera_flash(void);
extern void sensor_turn_off_camera_flash(void);

extern void sensor_power_enable(void);
extern void sensor_power_disable(void);
extern int sensor_get_powerdown(void);
extern void sensor_powerdown_enable(void);
extern void sensor_powerdown_disable(void);
extern void sensor_reset_high(void);
extern void sensor_reset_low(void);

#endif
