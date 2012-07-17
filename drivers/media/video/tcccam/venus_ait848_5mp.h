/*
 * drivers/media/video/tcccam/mv9317_3mp.h
 *
 * Register definitions for the mv9317 CameraChip.
 *
 * Author: zzau (zzau@telechips.com)
 *
 * Copyright (C) 2008 Telechips, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 */
#include <mach/globals.h>
#include "sensor_if.h"

#ifndef AIT848_H
#define AIT848_H

#ifndef  BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


/* The MV9317 I2C sensor chip has a fixed slave address of 0x5D. */
#define SENSOR_I2C_ADDR		0x06
	
#define REG_TERM 0xFFFF	/* terminating list entry for reg */
#define VAL_TERM 0xFFFF	/* terminating list entry for val */

//CLOCK
#define CKC_CAMERA_MCLK				120000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL1
#define CKC_CAMERA_SCLK				1280000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL1

#define FRAMESKIP_COUNT_FOR_CAPTURE 0

// ZOOM Setting!!
#define PRV_W			1280
#define PRV_H			720
#define PRV_ZOFFX		8
#define PRV_ZOFFY		6
	
#define CAP_W			2560
#define CAP_H			1920
#define CAP_ZOFFX		16
#define CAP_ZOFFY		12
	
#define CAM_2XMAX_ZOOM_STEP 	31
#define CAM_CAPCHG_WIDTH  		1024
	
	
struct sensor_reg {
	unsigned char reg;
	unsigned char val;
};

struct capture_size {
	unsigned long width;
	unsigned long height;
};

extern struct capture_size sensor_sizes[];

extern int sensor_venus_chip_enable(void);
extern int sensor_venus_chip_reset(void);	
extern void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func);
#endif /* AIT848_H */

