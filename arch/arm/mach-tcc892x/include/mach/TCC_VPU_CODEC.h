
#include "TCC892x_VPU_CODEC.h"

#ifndef _TCC_VPU_CODEC_H_
#define _TCC_VPU_CODEC_H_

#define RETCODE_MULTI_CODEC_EXIT_TIMEOUT	99

typedef struct dec_user_info_t
{
	unsigned int  bitrate_mbps;				//!< video BitRate (Mbps)
	unsigned int  frame_rate;				//!< video FrameRate (fps)
	unsigned int  m_bJpegOnly;				//!< If this is set, content is jpeg only file (ex. **.jpg)
	unsigned int  jpg_length;	
	unsigned int  jpg_ScaleRatio; 			////!< 0 ( Original Size )
											//!< 1 ( 1/2 Scaling Down )
											//!< 2 ( 1/4 Scaling Down )
											//!< 3 ( 1/8 Scaling Down )	
}dec_user_info_t;

#endif
