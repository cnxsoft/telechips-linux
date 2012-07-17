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


#define TCC_WMIXER_IOCTRL 					0x1


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
	WMIXER_POLLING,
	WMIXER_INTERRUPT,
	WMIXER_NOWAIT
} WMIXER_RESPONSE_TYPE;


typedef struct {
	unsigned int 			rsp_type; 		// wmix response type

	unsigned int 			src_y_addr;		// source y address
	unsigned int 			src_u_addr;		// source u address
	unsigned int 			src_v_addr;		// source v address
	unsigned int 			src_fmt; 	// source image format

	unsigned int 			dst_y_addr; 	// destination image address
	unsigned int 			dst_u_addr; 	// destination image address
	unsigned int 			dst_v_addr; 	// destination image address
	unsigned int 			dst_fmt;		// destination image format

	unsigned int 			img_width; 	// source image width
	unsigned int 			img_height; // source image height
} WMIXER_INFO_TYPE;


