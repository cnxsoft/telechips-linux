// sukhyun
#ifndef _simdef_h_
#define _simdef_h_

#include "tcc_type.h"
#include "io_vegas.h"
#include "simdef_system.h"

  
//----------------------------------------
// codaHX
#define VCACHE_EN
//#define VEGAS_ME_REDUCE
//#define VEGAS_SECOND_EN
//#define VEGAS_SECOND_USE_DDR
#define ENC_CODEC AVC_ENC
#define DEC_CODEC AVC_DEC
//----------------------------------------
#if 0
#define ISP_BUF_ADDR_MP_Y	0x48000000
#define ISP_BUF_ADDR_SP_Y	0x4B000000
#define ISP_BUF_ADDR_JPEG	0x4C000000
#define OSD_BUF_ADDR		0x4F000000
#else
//	0x10000000	256M
//	0x08000000	128M
//	0x04000000	64M
//	0x02000000	32M
//	0x01000000	16M
//	0x00100000	1M
	#if 1
	#define ISP_BUF_ADDR_MP_Y	0x49000000			
	#define ISP_BUF_ADDR_SP_Y	0x4B000000
	#define ISP_BUF_ADDR_JPEG	0x4C000000
	#define OSD_BUF_ADDR		0x4F000000
	#else
	#define ISP_BUF_ADDR_MP_Y	0x59000000			
	#define ISP_BUF_ADDR_SP_Y	0x5B000000
	#define ISP_BUF_ADDR_JPEG	0x5C000000
	#define OSD_BUF_ADDR		0x5F000000
	#endif
#endif
//----------------------------------------

#define ISP_BUF_ADDR_MP_CB  (ISP_BUF_ADDR_MP_Y +0x01000000)
#define ISP_BUF_ADDR_MP_CR  (ISP_BUF_ADDR_MP_CB+0x00800000)
#define ISP_BUF_ADDR_SP_CB  (ISP_BUF_ADDR_SP_Y +0x00800000)
#define ISP_BUF_ADDR_SP_CR  (ISP_BUF_ADDR_SP_CB+0x00400000)

// #define SIZE_3280_2462	0x007B3860
// #define SIZE_1640_1232	0x001ED480
// Sensor Size
/*
#ifdef SENSOR_SONY8M_1640_1232_27_1LANE
    #define SENSOR_IN_H_SIZE    1640
    #define SENSOR_IN_V_SIZE    1232
#else
    #define SENSOR_IN_H_SIZE    800
    #define SENSOR_IN_V_SIZE    600
#endif
*/
#define PREVIEW_SENSOR_IN_H_SIZE    1640
#define PREVIEW_SENSOR_IN_V_SIZE    1230
#define CAPTURE_SENSOR_IN_H_SIZE    3280
#define CAPTURE_SENSOR_IN_V_SIZE    2462

// Default PATH setting
//#define ISP_EN_JPEG // DO NOT USE WITH ISP_EN_MP
#define ISP_EN_MP // DO NOT USE WITH ISP_EN_JPEG
#define ISP_EN_SP

#define RGB888  6
#define RGB666  5
#define RGB565  4
#define YUV444  3
#define YUV422  2
#define YUV420  1
#define YUV400  0

#define PLANAR      0
#define SEMI_PLANAR 1
#define INTERLEAVED 2

//============================================================
// MP
//============================================================
// mp_write_format
// 0 - planar/raw8, 1 - semi planar, 2 - interleaved/raw12
#define MP_WR_FMT   PLANAR
#define MP_OUT_FMT  YUV422 //YUV420

#define ISP_OUT_SIZE_MP_H 1600 //640
#define ISP_OUT_SIZE_MP_V 1080 //480
#define ISP_OUT_SIZE_MP_PIX (ISP_OUT_SIZE_MP_H*ISP_OUT_SIZE_MP_V)


#if ( MP_WR_FMT == INTERLEAVED )
    #if ( MP_OUT_FMT == YUV422 )
        #define ISP_BUF_SIZE_MP_Y   (ISP_OUT_SIZE_MP_PIX*2)*2
        #define ISP_BUF_SIZE_MP_CB  0
        #define ISP_BUF_SIZE_MP_CR  0
    #else // YUV420
        #define ISP_BUF_SIZE_MP_Y   (ISP_OUT_SIZE_MP_PIX*1.5)*2
        #define ISP_BUF_SIZE_MP_CB  0
        #define ISP_BUF_SIZE_MP_CR  0
    #endif
#endif
#if ( MP_WR_FMT == SEMI_PLANAR )
    #if ( MP_OUT_FMT == YUV422 )
        #define ISP_BUF_SIZE_MP_Y   (ISP_OUT_SIZE_MP_PIX * 1)*2
        #define ISP_BUF_SIZE_MP_CB  (ISP_OUT_SIZE_MP_PIX * 1)*2
        #define ISP_BUF_SIZE_MP_CR  0
    #else
        #define ISP_BUF_SIZE_MP_Y   (ISP_OUT_SIZE_MP_PIX * 1)*2
        #define ISP_BUF_SIZE_MP_CB  (ISP_OUT_SIZE_MP_PIX * 0.5)*2
        #define ISP_BUF_SIZE_MP_CR  0
    #endif
#endif
#if ( MP_WR_FMT == PLANAR )
    #if ( MP_OUT_FMT == YUV422 )
        #define ISP_BUF_SIZE_MP_Y   (ISP_OUT_SIZE_MP_PIX * 1)*2
        #define ISP_BUF_SIZE_MP_CB  (ISP_OUT_SIZE_MP_PIX * 0.5)*2
        #define ISP_BUF_SIZE_MP_CR  (ISP_OUT_SIZE_MP_PIX * 0.5)*2
    #else
        #define ISP_BUF_SIZE_MP_Y   (ISP_OUT_SIZE_MP_PIX * 1)*2
        #define ISP_BUF_SIZE_MP_CB  (ISP_OUT_SIZE_MP_PIX * 0.25)*2
        #define ISP_BUF_SIZE_MP_CR  (ISP_OUT_SIZE_MP_PIX * 0.25)*2
    #endif
#endif

#define ISP_BUF_ADDR_MP_Y1      ISP_BUF_ADDR_MP_Y
#define ISP_BUF_ADDR_MP_CB1     ISP_BUF_ADDR_MP_CB
#define ISP_BUF_ADDR_MP_CR1     ISP_BUF_ADDR_MP_CR

//#define ISP_BUF_ADDR_MP_Y2      (ISP_BUF_ADDR_MP_Y+(ISP_BUF_SIZE_MP_Y*0.5))
//#define ISP_BUF_ADDR_MP_CB2     (ISP_BUF_ADDR_MP_CB+(ISP_BUF_SIZE_MP_CB*0.5))
//#define ISP_BUF_ADDR_MP_CR2     (ISP_BUF_ADDR_MP_CR+(ISP_BUF_SIZE_MP_CR*0.5))
#define ISP_BUF_ADDR_MP_Y2      (ISP_BUF_ADDR_MP_Y+(ISP_BUF_SIZE_MP_Y*0.5))
#define ISP_BUF_ADDR_MP_CB2     (ISP_BUF_ADDR_MP_CB+(ISP_BUF_SIZE_MP_CB*0.5))
#define ISP_BUF_ADDR_MP_CR2     (ISP_BUF_ADDR_MP_CR+(ISP_BUF_SIZE_MP_CR*0.5))


//============================================================
// SP
//============================================================
// sp_input_format
// 3 - yuv444, 2 - yuv422, 1 - yuv420, 0 - yuv400
#define SP_IN_FMT   YUV422

// sp_output_format
// 6 - rgb888, 5 - rgb666, 4 - rgb565
// 3 - yuv444, 2 - yuv422, 1 - yuv420, 0 - yuv400
#define SP_OUT_FMT  YUV422 //RGB888

// sp_write_format
// 0 - planar(sep), 1 - semi planar(int), 2 - interleaved(seq)
// NOTE: when RGB output, this field ignore
#define SP_WR_FMT   PLANAR

#define ISP_OUT_SIZE_SP_H 640 //320
#define ISP_OUT_SIZE_SP_V 480 //240
#define ISP_OUT_SIZE_SP_PIX (ISP_OUT_SIZE_SP_H*ISP_OUT_SIZE_SP_V)

#define ISP_BUF_SIZE_SP_Y       (ISP_OUT_SIZE_SP_PIX*1)*2
#define ISP_BUF_SIZE_SP_CB      (ISP_OUT_SIZE_SP_PIX*0.5)*2
#define ISP_BUF_SIZE_SP_CR      (ISP_OUT_SIZE_SP_PIX*0.5)*2


#define ISP_BUF_ADDR_SP_Y1      ISP_BUF_ADDR_SP_Y
#define ISP_BUF_ADDR_SP_CB1     ISP_BUF_ADDR_SP_CB
#define ISP_BUF_ADDR_SP_CR1     ISP_BUF_ADDR_SP_CR

#define ISP_BUF_ADDR_SP_Y2      (ISP_BUF_ADDR_SP_Y+(ISP_BUF_SIZE_SP_Y*0.5))
#define ISP_BUF_ADDR_SP_CB2     (ISP_BUF_ADDR_SP_CB+(ISP_BUF_SIZE_SP_CB*0.5))
#define ISP_BUF_ADDR_SP_CR2     (ISP_BUF_ADDR_SP_CR+(ISP_BUF_SIZE_SP_CR*0.5))


#endif //#ifndef _simdef_h_
