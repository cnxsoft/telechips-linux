/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/
#ifndef _JPEG_IOCTL_H_
#define _JPEG_IOCTL_H_

#define TCC_JPEGDEC_INT_POLL //int poll	

// For Decode!!
#define TCC_JPEGD_IOCTL_DEC          	100
#define TCC_JPEGD_IOCTL_HEADER_INFO  	101
#define TCC_JPEGD_IOCTL_USABLE				102

// For Encode!!
#define TCC_JPEGE_IOCTL_ENC          	201
#define TCC_JPEGE_IOCTL_MAKE_EXIF			202
#define TCC_JPEGE_IOCTL_USABLE				203


typedef enum {
	JPEG_ENCODE_MODE = 0,
	JPEG_DECODE_MODE,
	JPEG_DECODE_THUMB_MODE,
	JPEG_UNKNOWN_MODE
}JpegOperationType;

/* Decoder Output Scale Ratio */
typedef enum {
	NO_SCALE,
	HALF_SCALE,
	QUARTER_SCALE,
} Decode_Scale_Type;

enum jpeg_quantisation_val{
	JPEG_SUPERFINE = 3,
	JPEG_FINE	   = 4,
	JPEG_MEDIUM    = 5,
	JPEG_DEFAULT   = 4
};

struct jpeg_file_info {
	int file_size;
	int raw_data_addr;
	int jpeg_data_addr;
};

typedef enum {
	IMAGE_CHROMA_YONLY,
	IMAGE_CHROMA_420,
	IMAGE_CHROMA_422,
	IMAGE_CHROMA_444,
	IMAGE_CHROMA_422S,
	IMAGE_CHROMA_440,
	IMAGE_CHROMA_UNKNOWN
} ImageFormatType;

typedef enum {
	ENC_INPUT_420,
	ENC_INPUT_422
} EncodeInputType;

typedef struct {
	unsigned short year;
	unsigned short month;
	unsigned short date;
	unsigned short hour;
	unsigned short min;
	unsigned short sec;
} TCCXXX_JPEG_TIMESTAMP_TYPE;

typedef struct {
	int degrees;
	int minutes;
	int seconds;
} GPS_LATITUDE_TYPE;

typedef struct {
	int degrees;
	int minutes;
	int seconds;
} GPS_LONGITUDE_TYPE;

typedef struct {
	int 				Supported;
	GPS_LATITUDE_TYPE 	Latitude;
	GPS_LONGITUDE_TYPE 	Longitude;
	int 				Altitude;
        TCCXXX_JPEG_TIMESTAMP_TYPE time_stamp;
	char 				Processing_Method[128];
} TCCXXX_JPEG_GPS_INFO;

typedef struct {
	//input
	unsigned int source_addr; //physical-address
	unsigned int width;
	unsigned int height;
	unsigned int q_value; //jpeg_quantisation_val q_value;
	unsigned int src_format; //EncodeInputType src_format;
	unsigned int target_addr; 
	unsigned int target_size;
	
	//output
 	unsigned int header_offset;
	unsigned int thumb_offset;
	unsigned int bitstream_offset;	
	unsigned int header_size;
	unsigned int thumb_size;
	unsigned int bitstream_size;

	unsigned int normal_op;
}TCCXXX_JPEG_ENC_DATA;

typedef struct {
	//input
	unsigned int thumbjpg_addr; //physical-address
	unsigned int thumbjpg_size;
	unsigned int thumb_width;
	unsigned int thumb_height;

	unsigned int bitstream_size;
	unsigned int bitstream_width;
	unsigned int bitstream_height;
	
	unsigned int header_addr; //physical-address
	unsigned int header_size;

	unsigned int rotation;
	TCCXXX_JPEG_TIMESTAMP_TYPE time_stamp;

	TCCXXX_JPEG_GPS_INFO gps_info;
} TCCXXX_JPEG_ENC_EXIF_DATA;

typedef struct {
	//input
	unsigned int source_addr; //physical-address
	unsigned int file_length;

	//split input!!
	unsigned int split_body;
	unsigned int header_src_addr; //physical-address
	unsigned int header_src_len;
	unsigned int stream_src_addr; //physical-address
	unsigned int stream_src_len;
	
	//output
	unsigned int target_addr; //physical-address
	unsigned int target_size; 
	unsigned int image_format; //ImageFormatType image_format;
	unsigned int width;
	unsigned int height;
	unsigned int pad_width;
	unsigned int pad_height;
	unsigned int ratio; //Decode_Scale_Type ratio;

	unsigned int normal_op;
}TCCXXX_JPEG_DEC_DATA;

#endif

