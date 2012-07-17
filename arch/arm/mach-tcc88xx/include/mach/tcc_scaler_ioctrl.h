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


#define TCC_SCALER_IOCTRL			0x1
#define TCC_SCALER_IOCTRL_KERENL	0x10


#ifndef ADDRESS_ALIGNED
#define ADDRESS_ALIGNED
#define ALIGN_BIT (0x8-1)
#define BIT_0 3
#define GET_ADDR_YUV42X_spY(Base_addr) 		(((((unsigned int)Base_addr) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV42X_spU(Yaddr, x, y) 		(((((unsigned int)Yaddr+(x*y)) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV422_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/2)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#define GET_ADDR_YUV420_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/4)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#endif

typedef enum
{
	SCALER_POLLING,
	SCALER_INTERRUPT,
	SCALER_NOWAIT
} Scaler_ResponseType;

typedef enum{
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
	SCALER_FMT_MAX
} SCALER_DATA_FM;

typedef struct
{
	Scaler_ResponseType responsetype; 	//scaler response type
	char 				*src_Yaddr;		// source address
	char 				*src_Uaddr;		// source address
	char 				*src_Vaddr;		// source address
	SCALER_DATA_FM 		src_fmt;		// source image format
	unsigned int  		src_ImgWidth;	// source image width
	unsigned int  		src_ImgHeight;	// source image height
	unsigned int 		src_winLeft;
	unsigned int		src_winTop;
	unsigned int		src_winRight;
	unsigned int		src_winBottom;
	char 				*dest_Yaddr;	// destination image address
	char 				*dest_Uaddr;	// destination image address
	char 				*dest_Vaddr;	// destination image address
	SCALER_DATA_FM 		dest_fmt;		// destination image format
	unsigned int  		dest_ImgWidth;	// destination image width
	unsigned int  		dest_ImgHeight;	// destination image height
	unsigned int		dest_winLeft;
	unsigned int		dest_winTop;
	unsigned int		dest_winRight;
	unsigned int		dest_winBottom;
	unsigned int 		viqe_onthefly; 		// 0 : m to m , 0x1 : onthefly read  , 0x2 : onthefly write , 0x3 : read & write onthefly

	unsigned int		interlaced;
}SCALER_TYPE;
