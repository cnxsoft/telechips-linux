#ifndef __ITVDEMUX_H__
#define __ITVDEMUX_H__

#include <asm/types.h>
#ifdef __KERNEL__
#include <linux/time.h>
#else
#include <time.h>
#endif

#define ITV_DMX_FILTER_SIZE	16

typedef enum {
	ITV_DMX_OUT_DECODER, 
	ITV_DMX_OUT_TAP, 
	ITV_DMX_OUT_TS_TAP, 
	ITV_DMX_OUT_TSDEMUX_TAP
} itv_demux_output_e;

typedef enum {
	ITV_DMX_INPUT_FRONTEND, 
	ITV_DMX_INPUT_STREAM
} itv_demux_input_e;

typedef enum {
	ITV_DMX_PES_AUDIO_0, 
	ITV_DMX_PES_VIDEO_0, 
	ITV_DMX_PES_TELETEXT_0, 
	ITV_DMX_PES_SUBTITLE_0, 
	ITV_DMX_PES_PCR_0, 

	ITV_DMX_PES_AUDIO_1, 
	ITV_DMX_PES_VIDEO_1, 
	ITV_DMX_PES_TELETEXT_1, 
	ITV_DMX_PES_SUBTITLE_1, 
	ITV_DMX_PES_PCR_1, 

	ITV_DMX_PES_AUDIO_2, 
	ITV_DMX_PES_VIDEO_2, 
	ITV_DMX_PES_TELETEXT_2, 
	ITV_DMX_PES_SUBTITLE_2, 
	ITV_DMX_PES_PCR_2, 

	ITV_DMX_PES_AUDIO_3, 
	ITV_DMX_PES_VIDEO_3, 
	ITV_DMX_PES_TELETEXT_3, 
	ITV_DMX_PES_SUBTITLE_3, 
	ITV_DMX_PES_PCR_3, 

	ITV_DMX_PES_OTHER
} itv_demux_pes_type_e;

#define ITV_DMX_PES_AUDIO    ITV_DMX_PES_AUDIO_0
#define ITV_DMX_PES_VIDEO    ITV_DMX_PES_VIDEO_0
#define ITV_DMX_PES_TELETEXT ITV_DMX_PES_TELETEXT_0
#define ITV_DMX_PES_SUBTITLE ITV_DMX_PES_SUBTITLE_0
#define ITV_DMX_PES_PCR      ITV_DMX_PES_PCR_0

typedef enum {
	ITV_DMX_FILTER_TYPE_TS_NONE, 
	ITV_DMX_FILTER_TYPE_TS_SEC, 
	ITV_DMX_FILTER_TYPE_TS_PES
} itv_demux_filter_type_e;

typedef struct itv_demux_filter_t {
	__u8 filter[ITV_DMX_FILTER_SIZE];
	__u8 mask[ITV_DMX_FILTER_SIZE];
	__u8 mode[ITV_DMX_FILTER_SIZE];
} itv_demux_filter_t;

typedef struct itv_demux_sec_filter_params_t {
	__u16				pid;
	itv_demux_filter_t	filter;
	__u32				timeout;
	__u32				flags;
#define ITV_DMX_CHECK_CRC       1
#define ITV_DMX_ONESHOT         2
#define ITV_DMX_IMMEDIATE_START 4
#define ITV_DMX_KERNEL_CLIENT   0x8000
} itv_demux_sec_filter_params_t;

typedef struct itv_demux_pes_filter_params_t
{
	__u16					pid;
	itv_demux_input_e		input;
	itv_demux_output_e		output;
	itv_demux_pes_type_e	pes_type;
	__u32					flags;
} itv_demux_pes_filter_params_t;

#define ITV_DMX_START					_IO('o', 41)
#define ITV_DMX_STOP					_IO('o', 42)
#define ITV_DMX_SET_SEC_FILTER			_IOW('o', 43, itv_demux_sec_filter_params_t)
#define ITV_DMX_SET_PES_FILTER			_IOW('o', 44, itv_demux_pes_filter_params_t)

#endif	/* __ITVDEMUX_H__ */
