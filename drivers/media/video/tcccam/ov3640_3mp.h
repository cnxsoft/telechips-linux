/*
 * drivers/media/video/tcccam/ov3640.h
 *
 * Register definitions for the ov3640 CameraChip.
 *
 * Author: zzau (zzau@telechips.com)
 * Modify: taoyu (taoyu@gtc.jp)
 *
 * Copyright (C) 2008 Telechips, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 */
#include <mach/globals.h>
#include "sensor_if.h"

#ifndef OV3640_H
#define OV3640_H

/* The OV3640 I2C sensor chip has a fixed slave address of 0x78. */
#define SENSOR_I2C_ADDR		0x78

#define REG_TERM 0xffff	/* terminating list entry for reg */
#define VAL_TERM 0xff	/* terminating list entry for val */

//CLOCK
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL1
#define CKC_CAMERA_SCLK				800000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2

#define FRAMESKIP_COUNT_FOR_CAPTURE 1

// ZOOM Setting!!
#define PRV_W			1024
#define PRV_H			768
#define PRV_ZOFFX		8
#define PRV_ZOFFY		6

#define CAP_W			2048
#define CAP_H			1536
#define CAP_ZOFFX		16
#define CAP_ZOFFY		12

#define CAM_2XMAX_ZOOM_STEP 	31
#define CAM_CAPCHG_WIDTH  		1024


struct sensor_reg {
	unsigned short reg;
	unsigned char val;
};

struct capture_size {
	unsigned long width;
	unsigned long height;
};

extern struct capture_size sensor_sizes[];
extern void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func);

#endif /* MT9D112_H */

