/*
 * linux/drivers/video/tcc_composite.h
 *
 * Author:  <linux@telechips.com>
 * Created: March 18, 2010
 * Description: TCC Component TV-Out Driver
 *
 * Copyright (C) 20010-2011 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
#ifndef	_TCC_COMPOSITE_H_
#define	_TCC_COMPOSITE_H_  

/*==============================================================================
				NTSC/PAL Encoder
  ==============================================================================*/
#define	LCD_CH	1
#define	IMG_CH0	2
#define	IMG_CH1	4
#define	IMG_CH2	8

// flag
#define SET_IMAGE_INTL	((0x1)<<12)	// DMA Interlace Mode
#define SET_IMAGE_AEN	((0x1)<<11)	// Alpha-blending Function for Each Image
#define SET_IMAGE_CEN	((0x1)<<10)	// Chroma-keying Function for Each Image
#define SET_IMAGE_IEN	((0x1)<<9)	// Image Displaying Function for Each Image
#define SET_IMAGE_SRC	((0x1)<<8)	// Image Source Select
#define SET_IMAGE_AOPT	((0x1)<<7)	// Alpha-blending Option selection
#define SET_IMAGE_ASEL	((0x1)<<6)	// Alpha-blending Alpha type
#define SET_IMAGE_UPD	((0x1)<<5)	// Data Update Enable
#define SET_IMAGE_PD	((0x1)<<4)	// Bit padding
#define SET_IMAGE_Y2RMD	((0x1)<<3)	// Y2R mode set
#define SET_IMAGE_Y2R	((0x1)<<2)	// Y2R Conversion Enable bit
#define SET_IMAGE_BR	((0x1)<<1)	// Bit Reverse
#define SET_IMAGE_FMT	(0x1)		// Image Format 

// type
#define SET_IMAGE_SIZE			0x00000001
#define SET_IMAGE_POSITION		0x00000002
#define SET_IMAGE_OFFSET		0x00000004
#define SET_IMAGE_SCALE			0x00000008
#define READ_IMAGE_POSITION		0x00000020
#define SET_IMAGE_ALL			0x0000003F

//#define TCC_COMPOSITE_CCIR656

typedef enum{
	NTSC_M			= 0x0000,
	NTSC_M_J		= 0x0001,
	NTSC_N			= 0x0010,
	NTSC_N_J		= 0x0011,
	NTSC_443		= 0x0020,
	PAL_M			= 0x0100,
	PAL_N			= 0x0110,
	PAL_B			= 0x0120,
	PAL_G			= 0x0130,
	PAL_H			= 0x0140,
	PAL_I			= 0x0150,
	PSEUDO_NTSC		= 0x1000,
	PSEUDO_PAL		= 0x1010,
	COMPOSITE_MODE_MAX
}COMPOSITE_MODE_TYPE;

typedef enum{
	COMPOSITE_LCDC_AOPT_0,	// 100% ~ 0.39% transparency
	COMPOSITE_LCDC_AOPT_1,	// 100% ~ 0.39% transparency
	COMPOSITE_LCDC_AOPT_2,	// 100% ~ 0% transparency
	COMPOSITE_LCDC_AOPT_MAX
}COMPOSITE_LCDC_AOPT_TYPE;

typedef enum{
	COMPOSITE_LCDC_ALPHA_VALUE 	= 0,
	COMPOSITE_LCDC_ALPHA_PIXVALUE = 1	//alpha RGB
}COMPOSITE_LCDC_ASEL_TYPE;

typedef enum{
	COMPOSITE_LCDC_YUV2RGB_TYPE0,
	COMPOSITE_LCDC_YUV2RGB_TYPE1,
	COMPOSITE_LCDC_YUV2RGB_TYPE2,
	COMPOSITE_LCDC_YUV2RGB_TYPE3
}COMPOSITE_LCDC_YUV2RGB_TYPE;

typedef enum{
	COMPOSITE_LCDC_IMG_FMT_1BPP,
	COMPOSITE_LCDC_IMG_FMT_2BPP,
	COMPOSITE_LCDC_IMG_FMT_4BPP,
	COMPOSITE_LCDC_IMG_FMT_8BPP,
	COMPOSITE_LCDC_IMG_FMT_RGB332		= 8,
	COMPOSITE_LCDC_IMG_FMT_RGB444		= 9,
	COMPOSITE_LCDC_IMG_FMT_RGB565		= 10,
	COMPOSITE_LCDC_IMG_FMT_RGB555		= 11,
	COMPOSITE_LCDC_IMG_FMT_RGB888		= 12,
	COMPOSITE_LCDC_IMG_FMT_RGB666		= 13,
	COMPOSITE_LCDC_IMG_FMT_YUV420SP		= 24,	
	COMPOSITE_LCDC_IMG_FMT_YUV422SP		= 25, 
	COMPOSITE_LCDC_IMG_FMT_YUV422SQ		= 26, 
	COMPOSITE_LCDC_IMG_FMT_YUV420ITL0	= 28, 
	COMPOSITE_LCDC_IMG_FMT_YUV420ITL1	= 29, 
	COMPOSITE_LCDC_IMG_FMT_YUV422ITL0	= 30, 
	COMPOSITE_LCDC_IMG_FMT_YUV422ITL1	= 31, 
	COMPOSITE_LCDC_IMG_FMT_MAX
}COMPOSITE_LCDC_IMG_FMT_TYPE;

typedef struct{
	char INTL;							// DMA Interlace Mode
	char AEN;							// Alpha-blending Function for Each Image
	char CEN;							// Chroma-keying Function for Each Image
	char IEN;							// Image Displaying Function for Each Image
	char SRC;							// Image Source Select
	COMPOSITE_LCDC_AOPT_TYPE AOPT;		// Alpha-blending Option selection
	COMPOSITE_LCDC_ASEL_TYPE ASEL;		// Alpha-blending Alpha type
	char UPD;							// Data Update Enable
	char PD;							// Bit padding
	COMPOSITE_LCDC_YUV2RGB_TYPE Y2RMD;	// Y2R mode set
	char Y2R;							// Y2R Conversion Enable bit
	char BR;							// Bit Reverse
	COMPOSITE_LCDC_IMG_FMT_TYPE FMT;	// Image Format 
}COMPOSITE_LCDC_IMG_CTRL_TYPE;

typedef struct{
	unsigned int composite_clk; 		// pixel clock
	unsigned int composite_bus_width;	// data bus width
	unsigned int composite_lcd_width;	// lcd width
	unsigned int composite_lcd_height;	// lcd height
	unsigned int composite_LPW; 		// line pulse width
	unsigned int composite_LPC; 		// line pulse count (active horizontal pixel - 1)
	unsigned int composite_LSWC;		// line start wait clock (the number of dummy pixel clock - 1)
	unsigned int composite_LEWC;		// line end wait clock (the number of dummy pixel clock - 1)
	unsigned int composite_VDB; 		// Back porch Vsync delay
	unsigned int composite_VDF; 		// front porch of Vsync delay

	unsigned int composite_FPW1;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int composite_FLC1;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int composite_FSWC1;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int composite_FEWC1;		// frame start wait cycle is the number of lines to insert at the begining each frame

	unsigned int composite_FPW2;		// TFT/TV : Frame pulse width is the pulse width of frmae clock
	unsigned int composite_FLC2;		// frmae line count is the number of lines in each frmae on the screen
	unsigned int composite_FSWC2;		// frmae start wait cycle is the number of lines to insert at the end each frame
	unsigned int composite_FEWC2; 		// frame start wait cycle is the number of lines to insert at the begining each frame
}COMPOSITE_SPEC_TYPE;

#endif //_TCC_COMPOSITE_H_
