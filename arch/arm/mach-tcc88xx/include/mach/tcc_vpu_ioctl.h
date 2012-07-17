
#ifndef _VDEC_IOCTL_H_
#define _VDEC_IOCTL_H_

typedef enum {
	VPU_DEC = 0,
	VPU_DEC_EXT,	
	VPU_ENC,
	VPU_MAX
}vputype;

#if !defined(CONFIG_TCC_VPU)
#define VPU_IN_KERNEL
#endif

////////////////////////////////////////////////////////////////////////////////////////
/* VPU MANAGER */
typedef enum
{
	OPEN_VIDEO,
    OPEN_DMB
} Open_Type;

typedef struct
{
	vputype				type;
	unsigned int		width;
	unsigned int		height;
	unsigned int		bitrate;
	unsigned int		framerate;
	unsigned int		isSWCodec;	
}CONTENTS_INFO;

typedef struct
{
	unsigned int count;
	unsigned int dmb_opened;
	unsigned int vid_opened;
}OPENED_gINFO;

typedef struct
{
	Open_Type type;
	unsigned int opened_cnt;
}OPENED_sINFO;

typedef enum
{
	BUFFER_ELSE,
	BUFFER_WORK,
    BUFFER_STREAM,
} Buffer_Type;

typedef struct
{
	unsigned int request_size;
	unsigned int phy_addr;
	unsigned int kernel_remap_addr;
	Buffer_Type buffer_type;
}MEM_ALLOC_INFO_t;

#define VPU_SET_CLK 		 10
#define VPU_GET_OPEN_INFO 	 20
#define VPU_SET_OPEN_INFO 	 30
#define VPU_FORCE_CLK_CLOSE  100 //fixed value

#ifdef VPU_IN_KERNEL
#define VPU_GET_FREEMEM_SIZE 40
#define VPU_HW_RESET		 50

////////////////////////////////////////////////////////////////////////////////////////
/* VPU DECODER */
#define DEVICE_INITIALIZE				100
#define V_DEC_INIT						11	//!< init
#define V_DEC_SEQ_HEADER				12	//!< decode sequence header 
#define V_DEC_GET_INFO					13
#define V_DEC_REG_FRAME_BUFFER			14	//!< register frame buffer
#define V_DEC_REG_FRAME_BUFFER2			15	//!< register frame buffer
#define V_DEC_DECODE					16	//!< decode
#define V_DEC_BUF_FLAG_CLEAR			17	//!< display buffer flag clear
#define V_DEC_FLUSH_OUTPUT				18	//!< flush delayed output frame
#define V_GET_RING_BUFFER_STATUS		19
#define V_FILL_RING_BUFFER_AUTO						20    //!< Fill the ring buffer
#define V_GET_INITIAL_INFO_FOR_STREAMING_MODE_ONLY	21    //!< Get initial Info for ring buffer use
#define V_DEC_CLOSE									22	//!< close

#define V_DEC_ALLOC_MEMORY				23
#define V_DEC_GENERAL_RESULT			24
#define V_DEC_INIT_RESULT				25
#define V_DEC_SEQ_HEADER_RESULT			26
#define V_DEC_DECODE_RESULT				27
#define V_DEC_UPDATE_RINGBUF_WP		28

#define V_GET_RING_BUFFER_STATUS_RESULT	29
#define V_FILL_RING_BUFFER_AUTO_RESULT	30
#define V_DEC_UPDATE_RINGBUF_WP_RESULT	31
#define V_GET_INITIAL_INFO_FOR_STREAMING_MODE_ONLY_RESULT 32

#define V_GET_VPU_VERSION					33
#define V_GET_VPU_VERSION_RESULT			34

typedef struct {
	int result;
	codec_handle_t gsVpuDecHandle;
	dec_init_t gsVpuDecInit;
}VDEC_INIT_t;

typedef struct {
	int result;
	unsigned int stream_size;
	dec_initial_info_t gsVpuDecInitialInfo;
}VDEC_SEQ_HEADER_t;

typedef struct {
	int result;
	dec_buffer_t gsVpuDecBuffer;
}VDEC_SET_BUFFER_t;

typedef struct {
	int result;
	dec_input_t gsVpuDecInput;
	dec_output_t gsVpuDecOutput;
}VDEC_DECODE_t;

typedef struct {
	int result;
	dec_ring_buffer_status_out_t gsVpuDecRingStatus;
}VDEC_RINGBUF_GETINFO_t;

typedef struct {
	int result;
	dec_init_t gsVpuDecInit;
	dec_ring_buffer_setting_in_t gsVpuDecRingFeed;
}VDEC_RINGBUF_SETBUF_t;

typedef struct {
	int result;
	int iCopiedSize;
	int iFlushBuf;
}VDEC_RINGBUF_SETBUF_PTRONLY_t;

typedef struct {
	int result;
	char * pszVersion;
	char * pszBuildData;
}VDEC_GET_VERSION_t;
////////////////////////////////////////////////////////////////////////////////////////
/* VPU ENCODER */
#define V_ENC_INIT					10	//!< init
#define V_ENC_REG_FRAME_BUFFER		11	//!< register frame buffer
#define V_ENC_PUT_HEADER			12
#define V_ENC_ENCODE				13	//!< encode
#define V_ENC_CLOSE					14	//!< close

#define V_ENC_ALLOC_MEMORY			16
#define V_ENC_GENERAL_RESULT		17
#define V_ENC_INIT_RESULT			18
#define V_ENC_PUT_HEADER_RESULT		19
#define V_ENC_ENCODE_RESULT			20

typedef struct {
	int result;
	codec_handle_t gsVpuEncHandle;
	enc_init_t gsVpuEncInit;
	enc_initial_info_t gsVpuEncInitialInfo;
}VENC_INIT_t;

typedef struct {
	int result;
	enc_buffer_t gsVpuEncBuffer;
}VENC_SET_BUFFER_t;

typedef struct {	
	int result;
	enc_header_t gsVpuEncHeader;
}VENC_PUT_HEADER_t;

typedef struct {
	int result;
	enc_input_t gsVpuEncInput;
	enc_output_t gsVpuEncOutput;
}VENC_ENCODE_t;
#endif /*VPU_IN_KERNEL*/
#endif
