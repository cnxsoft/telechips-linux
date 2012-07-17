/* linux/arch/arm/mach-msm/irq.c
 *
 * Copyright (C) 2008, 2009 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#define TCC_SCALER_IOCTRL 					0x1
#define TCC_SCALER_IOCTRL_KERENL 			0x10
#define TCC_SCALER_VIOC_PLUGIN 				0x100
#define TCC_SCALER_VIOC_PLUGOUT 			0x101
#define TCC_SCALER_VIOC_ALPHA_INIT 			0x102
#define TCC_SCALER_VIOC_ALPHA_RUN 			0x103

#ifndef ADDRESS_ALIGNED
#define ADDRESS_ALIGNED
#define ALIGN_BIT 							(0x8-1)
#define BIT_0 								3
#define GET_ADDR_YUV42X_spY(Base_addr) 		(((((unsigned int)Base_addr) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV42X_spU(Yaddr, x, y) 	(((((unsigned int)Yaddr+(x*y)) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV422_spV(Uaddr, x, y) 	(((((unsigned int)Uaddr+(x*y/2)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#define GET_ADDR_YUV420_spV(Uaddr, x, y) 	(((((unsigned int)Uaddr+(x*y/4)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#endif

typedef enum {
	SCALER_POLLING,
	SCALER_INTERRUPT,
	SCALER_NOWAIT
} Scaler_ResponseType;

typedef enum {
	SCALER_YUV422_sq0,
	SCALER_YUV422_sq1,
	SCALER_YUV422_sp,
	SCALER_YUV420_sp,
	SCALER_YUV422_inter,
	SCALER_YUV420_inter,
	SCALER_RGB565,
	SCALER_RGB555,
	SCALER_RGB454,
	SCALER_RGB444,
	SCALER_ARGB8888,
	SCALER_FMT_MAX
} SCALER_DATA_FM;

typedef struct {
	Scaler_ResponseType responsetype; 	//scaler response type
	char 				*src_Yaddr;		// source address
	char 				*src_Uaddr;		// source address
	char 				*src_Vaddr;		// source address
	unsigned int 		src_fmt;		// source image format
	unsigned int  		src_ImgWidth;	// source image width
	unsigned int  		src_ImgHeight;	// source image height
	unsigned int 		src_winLeft;
	unsigned int		src_winTop;
	unsigned int		src_winRight;
	unsigned int		src_winBottom;
	char 				*dest_Yaddr;	// destination image address
	char 				*dest_Uaddr;	// destination image address
	char 				*dest_Vaddr;	// destination image address
	unsigned int 		dest_fmt;		// destination image format
	unsigned int  		dest_ImgWidth;	// destination image width
	unsigned int  		dest_ImgHeight;	// destination image height
	unsigned int		dest_winLeft;
	unsigned int		dest_winTop;
	unsigned int		dest_winRight;
	unsigned int		dest_winBottom;
	unsigned int 		viqe_onthefly; 	// 0 : m to m , 0x1 : onthefly read  , 0x2 : onthefly write , 0x3 : read & write onthefly
	unsigned int		interlaced;
	unsigned char 		plugin_path;
	unsigned char 		divide_path;	// 0 : Normal, 1: SBS(SideBySide), 2: TNB(Top & Botton)
}SCALER_TYPE;

typedef struct {
	unsigned short 	scaler_no; 		// scaler number : 0, 1, 2
	unsigned short 	bypass_mode; 	// scaler bypass option
	unsigned short 	plugin_path; 	// scaler plug-in path

	unsigned short 	src_width; 	 	// scaler input data width
	unsigned short 	src_height; 	// scaler input data height
	unsigned short 	src_win_left;
	unsigned short	src_win_top;
	unsigned short	src_win_right;
	unsigned short	src_win_bottom;

	unsigned short 	dst_width; 		// scaler output data width
	unsigned short 	dst_height; 	// scaler output data height
	unsigned short 	dst_win_left;
	unsigned short	dst_win_top;
	unsigned short	dst_win_right;
	unsigned short	dst_win_bottom;
} SCALER_PLUGIN_Type;

typedef struct {
	unsigned char 	rsp_type;
	unsigned char 	region;

	unsigned char 	src0_fmt;
	unsigned char 	src0_layer;
	unsigned short 	src0_acon0;
	unsigned short 	src0_acon1;
	unsigned short 	src0_ccon0;
	unsigned short 	src0_ccon1;
	unsigned short 	src0_rop_mode;
	unsigned short 	src0_asel;
	unsigned short 	src0_alpha0;
	unsigned short 	src0_alpha1;
	unsigned int 	src0_Yaddr;
	unsigned int 	src0_Uaddr;
	unsigned int 	src0_Vaddr;
	unsigned short 	src0_width;
	unsigned short 	src0_height;
	unsigned short 	src0_winLeft;
	unsigned short	src0_winTop;
	unsigned short	src0_winRight;
	unsigned short	src0_winBottom;


	unsigned char 	src1_fmt;
	unsigned char 	src1_layer;
	unsigned short 	src1_acon0;
	unsigned short 	src1_acon1;
	unsigned short 	src1_ccon0;
	unsigned short 	src1_ccon1;
	unsigned short 	src1_rop_mode;
	unsigned short 	src1_asel;
	unsigned short 	src1_alpha0;
	unsigned short 	src1_alpha1;
	unsigned int 	src1_Yaddr;
	unsigned int 	src1_Uaddr;
	unsigned int 	src1_Vaddr;
	unsigned short 	src1_width;
	unsigned short 	src1_height;
	unsigned short 	src1_winLeft;
	unsigned short	src1_winTop;
	unsigned short	src1_winRight;
	unsigned short	src1_winBottom;

	unsigned char 	dst_fmt;
	unsigned int 	dst_Yaddr;
	unsigned int 	dst_Uaddr;
	unsigned int 	dst_Vaddr;
	unsigned short 	dst_width;
	unsigned short 	dst_height;
	unsigned short 	dst_winLeft;
	unsigned short	dst_winTop;
	unsigned short	dst_winRight;
	unsigned short	dst_winBottom;
} SCALER_ALPHABLENDING_TYPE;


