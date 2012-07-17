/*
 * drivers/media/video/tcccam/ov7690_vga.h
 *
 * Register definitions for the ov7690 CameraChip.
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

#ifndef SIV100B_H
#define SIV100B_H

#define SIV100B_REG_TERM 				0xff	/* terminating list entry for reg */
#define SIV100B_VAL_TERM 				0xff	/* terminating list entry for val */

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern void sensor_info_init_siv100b(TCC_SENSOR_INFO_TYPE *sensor_info);
extern void sensor_init_fnc_siv100b(SENSOR_FUNC_TYPE *sensor_func);
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
/* The SIV100B I2C sensor chip has a fixed slave address of 0x7A. */
#define SENSOR_I2C_ADDR		0x66
#define SENSOR_FRAMERATE	17

//CLOCK
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#if defined(CONFIG_USE_ISP)
#define CAM_POLARITY_VSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_HSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_PCLK		0   // 1: positive edge	0: negative edge
#endif //CONFIG_USE_ISP
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				800000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#elif defined(CONFIG_ARCH_TCC92XX)
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				800000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#elif defined(CONFIG_ARCH_TCC93XX)
#if defined(CONFIG_USE_ISP)
#define CAM_POLARITY_VSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_HSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_PCLK	1   // 1: positive edge	0: negative edge
#endif //CONFIG_USE_ISP
#define CKC_CAMERA_MCLK				240000
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL1
#define CKC_CAMERA_SCLK				800000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#else
// todo : 
#endif

#define FRAMESKIP_COUNT_FOR_CAPTURE 1

// ZOOM Setting!!
#define PRV_W			640
#define PRV_H			480
#define PRV_ZOFFX		8
#define PRV_ZOFFY		6

#define CAP_W			640
#define CAP_H			480
#define CAP_ZOFFX		8
#define CAP_ZOFFY		6

#define CAM_2XMAX_ZOOM_STEP 	24
#define CAM_CAPCHG_WIDTH  		640

#ifdef CONFIG_USE_ISP
#define CAM_MAX_ZOOM_STEP    CAM_2XMAX_ZOOM_STEP+1
#endif

struct sensor_reg {
	unsigned char reg;
	unsigned char val;
};

struct capture_size {
	unsigned long width;
	unsigned long height;
};

extern struct capture_size sensor_sizes[];
extern void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func);
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#endif /* SIV100B_H */

