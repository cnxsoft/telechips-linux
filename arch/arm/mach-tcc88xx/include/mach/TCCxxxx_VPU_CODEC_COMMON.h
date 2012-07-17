/*!
 ***********************************************************************
 \par Copyright
 \verbatim
  ________  _____           _____   _____           ____  ____   ____		
     /     /       /       /       /       /     /   /    /   \ /			
    /     /___    /       /___    /       /____ /   /    /____/ \___			
   /     /       /       /       /       /     /   /    /           \		
  /     /_____  /_____  /_____  /_____  /     / _ /_  _/_      _____/ 		
   																				
  Copyright (c) 2009 Telechips Inc.
  Luther Bldg, 7-20 Sincheon-dong, Songpa-gu, Seoul, Korea
 \endverbatim
 ***********************************************************************
 */
/*!
 ***********************************************************************
 *
 * \file
 *		TCCxxxx_VPU_CODEC_COMMON.h
 * \date
 *		2011/03/02
 * \author
 *		AV algorithm group(AValgorithm@telechips.com) 
 * \brief
 *		main api
 * \version
 *		0.0.1 : first beta release (2011/03/02)
 *
 ***********************************************************************
 */

#ifndef _TCCXXXX_VPU_CODEC_COMMON_H_
#define _TCCXXXX_VPU_CODEC_COMMON_H_


#define PA 0 // physical address
#define VA 1 // virtual  address


#define PIC_TYPE_I		0x000
#define PIC_TYPE_P		0x001
#define PIC_TYPE_B		0x002
#define PIC_TYPE_B_PB	0x102 //! only for MPEG-4 Packed PB-frame


#define STD_AVC		0 //!< DEC / ENC : AVC / H.264 / MPEG-4 Part 10
#define STD_VC1		1 //!< DEC
#define STD_MPEG2	2 //!< DEC
#define STD_MPEG4	3 //!< DEC / ENC
#define STD_H263	4 //!< DEC / ENC
#define STD_DIV3	5 //!< DEC
#define STD_RV		6 //!< DEC
#define STD_AVS		7 //!< DEC
#define STD_SH263	9 //!< DEC : Sorenson Spark / Sorenson's H.263
#define STD_MJPG	10//!< DEC
#define STD_VP8		11//!< DEC
#define STD_THEORA	12//!< DEC
#define STD_MVC		14//!< DEC


#define RETCODE_SUCCESS						0
#define RETCODE_FAILURE						1
#define RETCODE_INVALID_HANDLE				2
#define RETCODE_INVALID_PARAM				3
#define RETCODE_INVALID_COMMAND				4
#define RETCODE_ROTATOR_OUTPUT_NOT_SET		5
#define RETCODE_ROTATOR_STRIDE_NOT_SET		6
#define RETCODE_FRAME_NOT_COMPLETE			7
#define RETCODE_INVALID_FRAME_BUFFER		8
#define RETCODE_INSUFFICIENT_FRAME_BUFFERS	9
#define RETCODE_INVALID_STRIDE				10
#define RETCODE_WRONG_CALL_SEQUENCE			11
#define RETCODE_CALLED_BEFORE				12
#define RETCODE_NOT_INITIALIZED				13
#define RETCODE_USERDATA_BUF_NOT_SET		14
#define RETCODE_CODEC_FINISH				15		//the end of decoding
#define RETCODE_CODEC_EXIT					16
#define RETCODE_CODEC_SPECOUT				17

#define RETCODE_WRAP_AROUND					-10


typedef int codec_handle_t; 		//!< handle
typedef int codec_result_t; 		//!< return value
typedef unsigned int codec_addr_t;	//!< address


#define COMP_Y 0
#define COMP_U 1
#define COMP_V 2


#define ALIGNED_BUFF(buf, mul) ( ( (unsigned int)buf + (mul-1) ) & ~(mul-1) )


//------------------------------------------------------------------------------
// Definition of decoding process
//------------------------------------------------------------------------------

// Output Status
#define VPU_DEC_OUTPUT_FAIL			0
#define VPU_DEC_OUTPUT_SUCCESS		1

// Decoding Status
#define VPU_DEC_SUCCESS								1
#define VPU_DEC_INFO_NOT_SUFFICIENT_SPS_PPS_BUFF	2
#define VPU_DEC_INFO_NOT_SUFFICIENT_SLICE_BUFF		3
#define VPU_DEC_BUF_FULL							4
#define VPU_DEC_SUCCESS_FIELD_PICTURE				5

// Decoder Op Code 
#define VPU_DEC_INIT				0x00	//!< init
#define VPU_DEC_SEQ_HEADER			0x01	//!< decode sequence header 
#define VPU_DEC_GET_INFO			0x02
#define VPU_DEC_REG_FRAME_BUFFER	0x03	//!< register frame buffer
#define VPU_DEC_REG_FRAME_BUFFER2	0x04	//!< register frame buffer
#define VPU_DEC_DECODE				0x10	//!< decode
#define VPU_DEC_BUF_FLAG_CLEAR		0x11	//!< display buffer flag clear
#define VPU_DEC_FLUSH_OUTPUT		0x12	//!< flush delayed output frame
#define GET_RING_BUFFER_STATUS		0x13
#define FILL_RING_BUFFER_AUTO		0x14    //!< Fill the ring buffer
#define GET_INITIAL_INFO_FOR_STREAMING_MODE_ONLY			0x15    //!< Get initial Info for ring buffer use
#define VPU_UPDATE_WRITE_BUFFER_PTR    0x16    //!< Fill the ring buffer\r
#define VPU_DEC_CLOSE				0x20	//!< close
#define VPU_CODEC_GET_VERSION		0x3000

//------------------------------------------------------------------------------
// Definition of encoding process
//------------------------------------------------------------------------------

#define MPEG4_VOL_HEADER 	0x00
#define MPEG4_VOS_HEADER 	0x01
#define MPEG4_VIS_HEADER 	0x02
#define AVC_SPS_RBSP 		0x10
#define AVC_PPS_RBSP 		0x11

// Encoder Op Code
#define VPU_ENC_INIT				0x00	//!< init
#define VPU_ENC_REG_FRAME_BUFFER	0x01	//!< register frame buffer
#define VPU_ENC_PUT_HEADER			0x10
#define VPU_ENC_ENCODE				0x12	//!< encode
#define VPU_ENC_CLOSE				0x20	//!< close


#endif //_TCCXXXX_VPU_CODEC_COMMON_H_
