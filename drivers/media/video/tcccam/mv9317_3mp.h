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

#ifndef MV9317_H
#define MV9317_H

#define MV9317_REG_TERM 				0xFF	/* terminating list entry for reg */
#define MV9317_VAL_TERM 				0xFF	/* terminating list entry for val */

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern void sensor_info_init_mv9317(TCC_SENSOR_INFO_TYPE *sensor_info);
extern void sensor_init_fnc_mv9317(SENSOR_FUNC_TYPE *sensor_func);
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
/* The MV9317 I2C sensor chip has a fixed slave address of 0x5D. */
#define SENSOR_I2C_ADDR		0x50
#define SENSOR_FRAMERATE	20

//CLOCK
#if defined(CONFIG_ARCH_TCC92XX)
#define CKC_CAMERA_MCLK				720000		//(1-e)96M-25.8fps,  (2-o)86.4M - 23.26fps,  (3-e)82.5M - 22.2fps, (1-o)76.8M-20.6fps
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				720000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#elif defined(CONFIG_ARCH_TCC93XX)
#if defined(CONFIG_USE_ISP)
#define CAM_POLARITY_VSYNC 	1   // 1: low active,	0: high active	
#define CAM_POLARITY_HSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_PCLK 		0   // 1: positive edge	0: negative edge
#define CKC_CAMERA_MCLK				720000		//(1-e)96M-25.8fps,  (2-o)86.4M - 23.26fps,  (3-e)82.5M - 22.2fps, (1-o)76.8M-20.6fps, 64Mhz-17fps
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				720000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#else
#define CKC_CAMERA_MCLK				720000		//(1-e)96M-25.8fps,  (2-o)86.4M - 23.26fps,  (3-e)82.5M - 22.2fps, (1-o)76.8M-20.6fps
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				720000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#endif
#elif defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#if defined(CONFIG_USE_ISP)
#define CAM_POLARITY_VSYNC 	 1   // 1: low active,	0: high active	
#define CAM_POLARITY_HSYNC	0   // 1: low active,	0: high active	
#define CAM_POLARITY_PCLK 		0   // 1: positive edge	0: negative edge
#define CKC_CAMERA_MCLK				720000		//(1-e)96M-25.8fps,  (2-o)86.4M - 23.26fps,  (3-e)82.5M - 22.2fps, (1-o)76.8M-20.6fps, 64Mhz-17fps
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				720000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#else
#define CKC_CAMERA_MCLK				720000		//(1-e)96M-25.8fps,  (2-o)86.4M - 23.26fps,  (3-e)82.5M - 22.2fps, (1-o)76.8M-20.6fps
#define CKC_CAMERA_MCLK_SRC			PCDIRECTPLL2
#define CKC_CAMERA_SCLK				720000
#define CKC_CAMERA_SCLK_SRC			PCDIRECTPLL2
#endif
#endif

#define FRAMESKIP_COUNT_FOR_CAPTURE 		3

// ZOOM Setting!!
#define PRV_W			1024
#define PRV_H			768
#define PRV_ZOFFX		16
#define PRV_ZOFFY		12

#define CAP_W			2048
#define CAP_H			1536
#define CAP_ZOFFX		32
#define CAP_ZOFFY		24

#define CAM_2XMAX_ZOOM_STEP 	15
#define CAM_CAPCHG_WIDTH  		1024

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
#endif /* MV9317_H */

