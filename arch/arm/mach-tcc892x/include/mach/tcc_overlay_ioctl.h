/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

#ifndef _OVERLAY_H
#define _OVERLAY_H

#ifndef ADDRESS_ALIGNED
#define ADDRESS_ALIGNED
#define ALIGN_BIT (0x8-1)
#define BIT_0 3
#define GET_ADDR_YUV42X_spY(Base_addr) 		(((((unsigned int)Base_addr) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV42X_spU(Yaddr, x, y) 		(((((unsigned int)Yaddr+(x*y)) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV422_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/2)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#define GET_ADDR_YUV420_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/4)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#endif


#define OVERLAY_GET_POSITION		10
#define OVERLAY_GET_SCREEN_INFO		20
#define OVERLAY_SET_POSITION		30
#define OVERLAY_QUEUE_BUFFER		40
#define OVERLAY_SET_CONFIGURE		50
#define OVERLAY_SET_DISABLE			60
#define OVERLAY_SET_CROMA			70

#define OVERLAY_FORBID				100


typedef struct
{
	uint32_t sx;
	uint32_t sy;
	uint32_t width;
	uint32_t height;
	uint32_t format;
	uint32_t transform;
} overlay_config_t;

typedef struct
{
	uint32_t enable;
	uint32_t chromaR; // 0~8 key color , 16~ 24 mask 
	uint32_t chromaG; // 0~8 key color , 16~ 24 mask 
	uint32_t chromaB; // 0~8 key color , 16~ 24 mask 
} overlay_chroma_t;

#endif
