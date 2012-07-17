/*
 *  drivers/media/video/tcccam/tcc_cam.h
 *
 * Copyright (C) 2008 Telechips, Inc. 
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 */

#include <media/v4l2-common.h>
#include <mach/tcc_jpeg_ioctl.h>

#ifndef TCCXX_CIF_H
#define TCCXX_CIF_H

//#define JPEG_ENCODE_WITH_CAPTURE
#ifdef JPEG_ENCODE_WITH_CAPTURE
#define JPEG_TESTCODE
#endif

//#define DMA_ELEM_SIZE   4
//#define FIFO_TRIGGER_LVL (32)

/* Maximum number of buffers */
#define TCC_CAMERA_MAX_BUFNBRS 		14

#define ALIGNED_BUFF(buf, mul) ( ( (unsigned int)buf + (mul-1) ) & ~(mul-1) )

/*
 * ---------------------------------------------------------------------------
 *  tccxxx Camera Interface
 * ---------------------------------------------------------------------------
 */
#if 0//ndef	_LCDC_H_
#define PAGE0						0
#define PAGE1						1

#define SET_LCD_ENABLE				0x00000001
#define SET_LCD_DISABLE				0x00000002
#define SET_CH0_ENABLE				0x00000004
#define SET_CH0_DISABLE				0x00000008
#define SET_CH1_ENABLE				0x00000010
#define SET_CH1_DISABLE				0x00000020
#define SET_CH2_ENABLE				0x00000040
#define SET_CH2_DISABLE				0x00000080
#define SET_TFT_ENABLE				0x00000100
#define SET_TFT_DISABLE				0x00000200
#define SET_TVOUT_ENABLE			0x00000400
#define SET_TVOUT_DISABLE			0x00000800
#define SET_STN_ENABLE				0x00001000
#define SET_STN_DISABLE				0x00002000
#define SET_NOTINTERLACE_ENABLE		0x00004000
#define SET_NOTINTERLACE_DISABLE	0x00008000
#define SET_DP2_ENABLE				0x00010000
#define SET_DP2_DISABLE				0x00020000
#define SET_RGB2YUV_ENABLE			0x00040000
#define SET_RGB2YUV_DISABLE			0x00080000
#define SET_CCIR656_ENABLE			0x00100000
#define SET_CCIR656_DISABLE			0x00200000
#define SET_CH0YUV2RGB_ENABLE		0x00400000
#define SET_CH0YUV2RGB_DISABLE		0x00800000
#define SET_CH1CHROMA_ENABLE		0x01000000
#define SET_CH1CHROMA_DISABLE		0x02000000
#define SET_CH2CHROMA_ENABLE		0x04000000
#define SET_CH2CHROMA_DISABLE		0x08000000
#define SET_CH1ALPHA_ENABLE			0x10000000
#define SET_CH1ALPHA_DISABLE		0x20000000
#define SET_CH2ALPHA_ENABLE			0x40000000
#define SET_CH2ALPHA_DISABLE		0x80000000

#define	LCD_CH	1
#define	IMG_CH0	2
#define	IMG_CH1	4
#define	IMG_CH2	8


#define SET_IMAGE_SIZE			0x00000001
#define SET_IMAGE_POSITION		0x00000002
#define SET_IMAGE_OFFSET		0x00000004
#define SET_IMAGE_SCALE			0x00000008
#define SET_IMAGE_FORMAT		0x00000010
#define READ_IMAGE_POSITION		0x00000020
#define SET_IMAGE_ALL			0x0000003F
#define SET_LCD_POLARITY		0x00000040
#define SET_LCD_MASK			0x00000080
#define SET_LCD_ALL				0x000000C0

#define	IMG_LIT_ENDIAN		((0x0)<<7)
#define	IMG_BIG_ENDIAN		((0x1)<<7)
#define	IMG_RGB				((0x0)<<4)
#define	IMG_YUV420			((0x1)<<4)
#define	IMG_YUV422			((0x3)<<4)
#define	IMG_YUV422_DISP		((0x7)<<4)
#define	IMG_BPP_1			((0x0)<<0)
#define	IMG_BPP_2			((0x1)<<0)
#define	IMG_BPP_4			((0x2)<<0)
#define	IMG_BPP_332			((0x3)<<0)
#define	IMG_BPP_444			((0x4)<<0)
#define	IMG_BPP_565			((0x5)<<0)
#define	IMG_BPP_555			((0x6)<<0)
#define	IMG_BPP_888			((0x7)<<0)
#endif


enum cif_order422 {
	CIF_YCBYCR = 0,
	CIF_YCRYCB,
	CIF_CBYCRY,
	CIF_CRYCBY,
};

enum cifout_fmt {
	CIF_YUV422 = 0,
	CIF_YUV420_ODD,
	CIF_YUV420_EVEN,
};

enum cifoper_mode {
	OPER_PREVIEW = 0,
	OPER_CAPTURE,
};

enum cif_caminfo_tobeopen{
    CIF_FACING_BACK = 0,
    CIF_FACING_FRONT = 1 /* The camera faces to the user */
};

enum jpeg_resolution{
	CAM_UXGA = 0,     // 1600 X 1200,
	CAM_SXGA,         // 1280 X 1024, //1280 X 960
	CAM_XGA,          // 1024 X 768, 
	CAM_SVGA,         // 800 X 600,   
	CAM_VGA,          // 640 X 480,   
	CAM_QVGA,         // 320 X 240, 
	CAM_QCIF,		  // 176 X 144,
	CAM_MAX
};

enum tcc_effect_type{
	TCC_EFFECT_NORMAL,
	TCC_EFFECT_GRAY,                    
	TCC_EFFECT_NEGATIVE,
	TCC_EFFECT_SEPHIA,
	TCC_EFFECT_AQUA,
	TCC_EFFECT_GREEN,
	TCC_EFFECT_SKETCH,
	TCC_EFFECT_UNDEFINED 
};

typedef struct _ZOOM_DATA
{
	unsigned short XOffset;
	unsigned short YOffset;
	unsigned short Src_HSize;
	unsigned short Src_VSize;    
} ZOOM_DATA;

typedef struct _FRAME_DATA
{
	unsigned short framex;
	unsigned short framey;
	unsigned short cap_x;
	unsigned short cap_y;    
} FRAME_DATA;

typedef struct {
    unsigned int            p_Y;  
    unsigned int            p_Cb;
    unsigned int            p_Cr;
}img_buf_t;

typedef struct {
    unsigned short            source_x; 		/* CIF Input-Selection width/height */
    unsigned short            source_y;
    unsigned short            win_hor_ofst;	/* CIF Input-Selection Hori/Vert Offset (ex. For Zoom)*/
    unsigned short            win_ver_ofst;		

    unsigned short            scaler_x;		/* CIF Scaler out */
    unsigned short            scaler_y;
    unsigned short            target_x;		/* CIF Crop out :: real out */
    unsigned short            target_y;
}cif_main_set;

typedef struct
{
	unsigned short 			chromakey;
	
	unsigned char			mask_r;
	unsigned char			mask_g;
	unsigned char			mask_b;
	
	unsigned char			key_y;
	unsigned char			key_u;
	unsigned char			key_v;
	
}si_chromakey_info;

typedef struct
{
	unsigned short 			start_x;
	unsigned short 			start_y;
	unsigned short 			width;
	unsigned short 			height;
	
	unsigned int 			buff_offset;

	si_chromakey_info		chromakey_info;			
}cif_SuperImpose;

enum capture_status{
	CAPTURE_DONE = 0,
	CAPTURE_NO_INT,
	CAPTURE_OVERFLOW,
	CAPTURE_NONE
};

typedef struct
{
	unsigned int start_phy_addr; // start address of bitstream + thumb + header!!

 	unsigned int header_offset;
	unsigned int thumb_offset;
	unsigned int bitstream_offset;	
	unsigned int header_size;
	unsigned int thumb_size;
	unsigned int bitstream_size;
}jpeg_info;

typedef struct cif_c_t {
	enum cif_order422      	order422;
			
	unsigned char           polarity_pclk;
	unsigned char           polarity_vsync;
	unsigned char           polarity_href;
#ifdef CONFIG_ARCH_TCC892X
	unsigned char           polarity_de;
#endif
	enum cifoper_mode		oper_mode;
	unsigned char 			zoom_step; // for capture!!
	cif_main_set			main_set;

		
	/* 2 pingpong Frame memory */
	dma_addr_t            	base_buf;
	unsigned int            pp_totalsize;
	unsigned char           pp_num;        /* used pingpong memory number */
	img_buf_t               preview_buf[TCC_CAMERA_MAX_BUFNBRS];
	//img_buf_t				rotate_buf[2];
	img_buf_t				capture_buf;

	cif_SuperImpose			si_overlay;

	jpeg_info				jpg_info;
	enum cifout_fmt         fmt;

	volatile unsigned char	now_frame_num; // current buffer_index

	volatile unsigned char			esd_restart;

	//Retry Capture
	volatile enum capture_status	cap_status;
	volatile unsigned char			retry_cnt;
} cif_cfg_t;

/* NUM_CAMDMA_CHANNELS is the number of logical channels used for
 * DMA data transfer.
 */
#define NUM_CAMDMA_CHANNELS 2
/*
 * info for buffer allocation
 */
struct cif_dma_buffer {
	unsigned char *area;	/* virtual pointer */
	dma_addr_t addr;	/* physical address */
	size_t bytes;		/* buffer size in bytes */
	void *private_data; /* private for allocator; don't touch */
};

#if 0
/* buffer info */

struct cif_buffer_info {
	int state;			/* state of buffer */
	unsigned long size; 	/* size of jpg frame */
	struct timeval timestamp;	/* timestamp */
	unsigned long sequence; 	/* sequence number */
};

#endif 

enum tcc_cif_buffer_state {
	TCC_CIF_BUF_UNUSED,  /* not used */
	TCC_CIF_BUF_QUEUED,
	TCC_CIF_BUF_USING,    /* currently grabbing / playing */
	TCC_CIF_BUF_GRABBING,
	TCC_CIF_BUF_DONE,     /* done */	
	TCC_CIF_BUF_ERROR,
};


enum tcc_stream_state {
	STREAM_OFF,
	STREAM_INTERRUPT,
	STREAM_ON,
};


/*
 * Tracking of streaming I/O buffers.
 */
struct tccxxx_cif_buffer {
	struct list_head buf_list;
	struct v4l2_buffer v4lbuf;
	enum tcc_cif_buffer_state state;	
//	char *buffer;	/* Where it lives in kernel space */
	int mapcount;
	unsigned long vma_use_count;		
	struct TCCxxxCIF *cam;	
};

struct TCCxxxCIF{
	enum cif_caminfo_tobeopen cam_info;
	
	cif_cfg_t cif_cfg;
	/* frequncy (in Hz) of camera interface functional clock (ocp_clk) */
	//unsigned long ocp_clk; 

	struct timer_list cam_timer;

	struct list_head list;
	struct list_head done_list;
	
//	struct cif_buffer	buf[TCC_CAMERA_MAX_BUFNBRS]; 		
	//wkjung 0412
	struct tccxxx_cif_buffer static_buf[TCC_CAMERA_MAX_BUFNBRS]; 			
	struct tccxxx_cif_buffer *buf; 			
	struct cif_dma_buffer cif_buf;
	unsigned int n_sbufs;		/* How many we have */	

	unsigned int wakeup_int;
	wait_queue_head_t frame_wait;	/* Waiting on frame data */	
	spinlock_t dev_lock;  /* Access to device */	
	struct mutex	lock;	

	enum tcc_stream_state stream_state;
	
	int new;
};


#define VIDIOC_USER_CIF_OVERLAY 					_IOWR ('V', BASE_VIDIOC_PRIVATE, cif_SuperImpose)
#define VIDIOC_USER_JPEG_CAPTURE 					_IOWR ('V', BASE_VIDIOC_PRIVATE+1, int)
#define VIDIOC_USER_GET_CAPTURE_INFO 				_IOWR ('V', BASE_VIDIOC_PRIVATE+2, TCCXXX_JPEG_ENC_DATA)
#define VIDIOC_USER_PROC_AUTOFOCUS 				_IOWR ('V', BASE_VIDIOC_PRIVATE+3, int)
#define VIDIOC_USER_SET_CAMINFO_TOBEOPEN 		_IOWR ('V', BASE_VIDIOC_PRIVATE+4, int)
#define VIDIOC_USER_GET_MAX_RESOLUTION 			_IOWR ('V', BASE_VIDIOC_PRIVATE+5, int)
#define VIDIOC_USER_GET_SENSOR_FRAMERATE		_IOWR ('V', BASE_VIDIOC_PRIVATE+6, int)
#define VIDIOC_USER_GET_ZOOM_SUPPORT			_IOWR ('V', BASE_VIDIOC_PRIVATE+7, int)
#define VIDIOC_USER_SET_CAMERA_ADDR				_IOWR ('V', BASE_VIDIOC_PRIVATE+8, struct v4l2_requestbuffers)


extern int	tccxxx_cif_buffer_set(struct v4l2_requestbuffers *req);
extern int  tccxxx_cif_cam_restart(struct v4l2_pix_format *pix, unsigned long xclk);
extern int  tccxxx_cif_start_stream(void);
extern int  tccxxx_cif_stop_stream(void);
extern void tccxxx_cif_set_overlay(void);
extern int 	tccxxx_cif_set_effect (u8 nCameraEffect);
extern int 	tccxxx_cif_capture(int quality);
extern int 	tccxxx_cif_set_zoom(unsigned char arg);
extern int 	tccxxx_cif_set_resolution(unsigned int pixel_fmt, unsigned short width, unsigned short height);
extern int 	tccxxx_cif_open(void);
extern int 	tccxxx_cif_close(void);
extern int 	tccxxx_cif_init(void);
extern int 	tcc_get_sensor_info(int index);
extern void tccxxx_set_camera_addr(int index, unsigned int addr, unsigned int cameraStatus);
#endif /* TCCXX_CIF_H */
