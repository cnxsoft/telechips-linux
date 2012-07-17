/*
 * mach/tcc_ccfb_ioctl.h
 *
 * Copyright (C) 2011 Telechips, Inc. 
 *
 * This file used for DTV subtitle display. 
 *
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 *
 */

#ifndef _TCC_CCFB_H
#define _TCC_CCFB_H

/* ioctl enum */
#define CCFB_OPEN			0x100
#define CCFB_CLOSE			0x101
#define CCFB_GET_CONFIG	0x102
#define CCFB_SET_CONFIG	0x103
#define CCFB_DISP_UPDATE	0x104
#define CCFB_DISP_ENABLE	0x105
#define CCFB_DISP_DISABLE	0x106

typedef enum
{
	CCFB_LCDC_NONE,
	CCFB_LCDC_0,
	CCFB_LCDC_1,
	CCFB_LCDC_MAX
}ccfb_lcdc_type;

typedef enum
{
	CCFB_IMG_FMT_RGB565,
	CCFB_IMG_FMT_ARGB888,
	CCFB_IMG_FMT_MAX
}ccfb_img_fmt;

typedef struct
{
	uint32_t disp_fmt;
	uint32_t disp_x;
	uint32_t disp_y;
	uint32_t disp_w;
	uint32_t disp_h;
	uint32_t disp_m;	/* refer to LISC register for the value */

	uint32_t mem_w;
	uint32_t mem_h;

	uint32_t chroma_en;
	uint32_t chroma_R;
	uint32_t chroma_G;
	uint32_t chroma_B;

}ccfb_res_config_t;

typedef struct
{
	uint32_t			curLcdc;
	ccfb_res_config_t	res;
} ccfb_config_t;

#endif
