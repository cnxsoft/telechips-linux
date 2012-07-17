/*
 * drivers/media/video/tcc83xx/GT2005_2mp.h
 *
 * Register definitions for the gt2005 CameraChip.
 *
 * Author: Byunghoon Mun (joonimbc@telechips.com)
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 */
#include <mach/globals.h>
#include "sensor_if.h"

#ifndef GT2005_H
#define GT2005_H

/* The GT2005 I2C sensor chip has a fixed slave address of 0x78. */
#define SENSOR_I2C_ADDR		0x78
#define SENSOR_FRAMERATE	20

#define REG_TERM 0x0000	/* terminating list entry for reg */
#define VAL_TERM 0x0000	/* terminating list entry for val */

//CLOCK
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL1
#define CKC_CAMERA_SCLK				640000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL1

#define FRAMESKIP_COUNT_FOR_CAPTURE 1

// ZOOM Setting!!
#define PRV_W			800
#define PRV_H			600
#define PRV_ZOFFX		8
#define PRV_ZOFFY		6

#define CAP_W			1600
#define CAP_H			1200
#define CAP_ZOFFX		16
#define CAP_ZOFFY		12

#define CAM_2XMAX_ZOOM_STEP 	16
#define CAM_CAPCHG_WIDTH  		800


struct sensor_reg {
	unsigned short reg;
	unsigned short val;
};

struct capture_size {
	unsigned long width;
	unsigned long height;
};

extern struct capture_size sensor_sizes[];
extern void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func);

#endif /* GT2005_H */

