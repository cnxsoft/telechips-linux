/*
 * drivers/media/video/tcccam/tvp5150.h
 *
 * Register definitions for the tvp5150 CameraChip.
 *
 * Author: khcho (khcho@telechips.com)
 *
 * Copyright (C) 2008 Telechips, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 */

 #include <mach/globals.h>
#include "../sensor_if.h"

#ifndef __TVP5150_H__
#define __TVP5150_H__

//#define	CCR_VIDEODECODER_AK8856	1

#if	CCR_VIDEODECODER_AK8856
#define SENSOR_I2C_ADDR 0x88
#else
#define SENSOR_I2C_ADDR 0xB8
//#define SENSOR_I2C_ADDR 0xBA
#endif

#define REG_TERM 0xFF
#define VAL_TERM 0xFF

//CLOCK
#define CKC_CAMERA_MCLK				245000
#define CKC_CAMERA_MCLK_SRC			DIRECTPLL1
#define CKC_CAMERA_SCLK				2000000
#define CKC_CAMERA_SCLK_SRC			DIRECTPLL2

#define FRAMESKIP_COUNT_FOR_CAPTURE 1
#define SENSOR_FRAMERATE	15

#define PRV_W 720
#define PRV_H 240//480	//525
#define PRV_ZOFFX 0
#define PRV_ZOFFY 8

#define CAP_W 720 
#define CAP_H 240	//525	//(240 - 2)
#define CAP_ZOFFX 0
#define CAP_ZOFFY 0

#define CAM_2XMAX_ZOOM_STEP 25
#define CAM_CAPCHG_WIDTH 720

struct sensor_reg
{
	unsigned char reg;
	unsigned char val[1];
};

struct capture_size
{
	unsigned long width;
	unsigned long height;
};

extern struct capture_size sensor_sizes[];

extern struct sensor_reg *sensor_reg_common[];
extern struct sensor_reg *sensor_reg_brightness[];
extern struct sensor_reg *sensor_reg_awb[];
extern struct sensor_reg *sensor_reg_iso[];
extern struct sensor_reg *sensor_reg_effect[];
extern struct sensor_reg *sensor_reg_flip[];
extern struct sensor_reg *sensor_reg_scene[];
extern struct sensor_reg *sensor_reg_metering_exposure[];
extern struct sensor_reg *sensor_reg_af[];

extern void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func);

#endif /*__TVP5150_H__*/

