/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

#include "tcc_jpeg_param.h"

#ifndef _TCC_JPEG_H_
#define _TCC_JPEG_H_

#define TCC_JPEGDEC_INT_POLL
#define USE_VCACHE

#define TRUE   1
#define FALSE  0

#ifndef ADDRESS_ALIGNED
#define ADDRESS_ALIGNED
#define ALIGN_BIT (0x8-1)
#define BIT_0 3
#define GET_ADDR_YUV42X_spY(Base_addr) 		(((((unsigned int)Base_addr) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV42X_spU(Yaddr, x, y) 		(((((unsigned int)Yaddr+(x*y)) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV422_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/2)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#define GET_ADDR_YUV420_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/4)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#endif

typedef struct {
	unsigned char		is_rolling;
	unsigned int		y_addr;
	unsigned int		u_addr;
	unsigned int		v_addr;
	unsigned short		ifrm_hsize;
	unsigned short		ibuf_vsize;
	unsigned short		q_value;
	unsigned short		img_hsize;
	unsigned short		img_vsize;
	unsigned int		cbuf_addr;
	unsigned int		cbuf_size;
	unsigned int		frame_cnt;
	unsigned int		chroma;
} JPEG_ENC_INFO;

typedef struct {
	unsigned int	FileHandle;
	void			*lcd_buffer;
	unsigned int	cbuf_addr;
	unsigned int	cbuf_size;
	unsigned int	scale;
	unsigned int	ifrm_width;
	unsigned int	ibuf_height;
	unsigned int	FilePosition;
	unsigned char	ImageFormat;
} JPEG_DEC_INFO;

typedef struct _JPEG_DISPLAY_INFO
{
	unsigned char		JpegShootingModeFlag;
	unsigned char		JpegDecCodecFlag;
	unsigned char		JpegDecViewModeFlag;
	unsigned char		JpegDecodeStateFlag;
	unsigned char		JpegZoomPanFlag;
	unsigned char		JpegEditEffectFlag;
	unsigned char		JpegMakeThumbnailFlag;
	unsigned char		JpegErrorFlag;
	unsigned char		JpegBufferFullFlag;
	int 				JpegDisplayWidth;
	int 				JpegDisplayHeight;
	unsigned int		JpegCaptureBuffSize;
	unsigned int		JpegFileLength;
} JPEG_DISPLAY_INFO;

typedef struct _JPEG_BUFFER_CTRL
{
	void				*pBaseRawDataAddr;
	unsigned int		pBaseRawDataSize;
	void				*pBaseBitstreamDataAddr;
	unsigned int		pBaseBitstreamDataSize;
	void				*pBaseHeaderDataAddr;
	unsigned int		pBaseHeaderDataSize;
} JPEG_BUFFER_CTRL;

#endif
