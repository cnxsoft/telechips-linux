
#include <mach/tcc_jpeg_ioctl.h>

#ifndef _JPEGPARAM_H_
#define _JPEGPARAM_H_

#define JPEG_DECODE_BASE	0xF0710000

#define regw(a, d)	*((volatile unsigned int *) (a)) = (unsigned int) (d);

#define TRUE   1
#define FALSE  0

typedef char				int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed int			int32;
typedef unsigned int		uint32;

#define FRAME_LINE_CNT    64

extern unsigned int Y_QTable[64];
extern unsigned int C_QTable[64];

#define MAX_RESOLUTION_WIDTH    4096
#define MAX_RESOLUTION_HEIGHT   4096

#define EXIF_THUMBNAIL_MAKE_START_WIDTH     800
#define EXIF_THUMBNAIL_MAKE_START_HEIGHT    600

#define EXIF_THUMBNAIL_WIDTH     320
#define EXIF_THUMBNAIL_HEIGHT    240
#define EXIT_THUMBNAIL_QUALITY   3


typedef struct {
	unsigned short year;
	unsigned short month;
	unsigned short date;
	unsigned short hour;
	unsigned short min;
	unsigned short sec;
} jpeg_encode_time_type;

typedef struct {
	GPS_LATITUDE_TYPE 	Latitude;
	GPS_LONGITUDE_TYPE 	Longitude;
	int 				Altitude;
        jpeg_encode_time_type   time_stamp;
	char 				Processing_Method[128];
} jpeg_encode_gps_info_type;

typedef struct {
	signed short 	altitude;
	signed int	 	latitude;
	signed int	 	longitude;
} exif_gps_position_type;

typedef struct {
	int numerator;
	int denominator;
} exif_rational_type;

typedef struct {
	unsigned int thumbnail_width;
	unsigned int thumbnail_height;
	unsigned int thumbnail_image_size;
	unsigned int pBase0_Exif_Thumbnail;
	unsigned int pBase1_Exif_Thumbnail;
	unsigned int pBase2_Exif_Thumbnail;
	void         *pThumbnail_Buff;
} jpeg_encode_thumbnail_info_type;

typedef struct {
	unsigned int					ImageWidth;
	unsigned int					ImageHeight;
	unsigned short					Q_FactorValue;
	unsigned char					IsManufacturer;
	unsigned char					IsCopyRight;
	unsigned char					IsEncodeTime;
	unsigned char					IsGpsInfo;
	unsigned char					IsThumbnail;
	unsigned int					BitStreamSize;
	unsigned int					HeaderSize;
	unsigned int 					rotation;
	jpeg_encode_time_type			EncodingTime;
	jpeg_encode_gps_info_type		GpsInfo;
	jpeg_encode_thumbnail_info_type	ThumbnailInfo;
} jpeg_encode_option_type;

typedef struct {
	unsigned int	HeaderSize;
	unsigned int	Thumbnail_Position;
} jpeg_header_exif_rsp;

typedef struct {
	unsigned int uiCurrentPosition;
	unsigned int uiTagOffsetValue;
} exif_tag_rsp_info;

enum{
	YONLY = 0,
	YUV420,
	YUV422,
	YUV444,
	YUV422S,
	NOT_SUPPORTED
};

typedef struct {
	unsigned int		BitStreamSize;
	unsigned int		HeaderSize;
	unsigned short		ImageWidth;
	unsigned short		ImageHeight;
	unsigned short		PaddingWidth;
	unsigned short		PaddingHeight;
	ImageFormatType			ImgChromaType;
	unsigned char		IsNeedsPadding;
	//unsigned char		IsThumbnailImage;
} jpeg_parser_rsp;

/**********************************************************************/
/*                                                                    */
/*                      JPEG MARKER SEGMENTS                          */
/*                                                                    */
/**********************************************************************/
#define SOI			0xFFD8  // Start of Image
#define EOI			0xFFD9  // End of Image
#define SOS			0xFFDA  // Start of Scan
#define DQT 			0xFFDB  // DCT Quantization Table
#define DNL 			0xFFDC  // Define number of lines
#define DRI 			0xFFDD  // Restart interval
#define DHP 			0xFFDE  // Define hierarchical progression
#define EXP 			0xFFDF  // Expand Reference Component

//Start Of Frame Markers, Non-Differential, Huffman Coding
#define SOF 			0xFFC0  // SOF0 :: Baseline DCT
#define SOF1 			0xFFC1  // SOF1 :: Extended Sequential DCT
#define SOF2 			0xFFC2  // SOF2 :: Progressive DCT
#define SOF3 			0xFFC3  // SOF3 :: Lossless (Sequential)

//Start Of Frame Markers, Differential, Huffman Coding
#define SOF5 			0xFFC5  // SOF5 :: Differential Sequential DCT
#define SOF6 			0xFFC6  // SOF6 :: Differential Progressive DCT
#define SOF7 			0xFFC7  // SOF7 :: Differential Lossless (Sequential)

//Start Of Frame Markers, Non-Differential, Arithmetic Coding
#define JPG 			0xFFC8  // JPG  :: Reserved for JPEG Extensions
#define SOF9 			0xFFC9  // SOF9 :: Extended Sequential DCT
#define SOF10 			0xFFCA  // SOF10 :: Progressive DCT
#define SOF11 			0xFFCB  // SOF11 :: Lossless (Sequential)

//Start Of Frame Markers, Differential, Arithmetic Coding
#define SOF13 			0xFFCD  // SOF13 :: Differential Sequential DCT
#define SOF14 			0xFFCE  // SOF14 :: Differential Progressive DCT
#define SOF15			0xFFCF  // SOF15 :: Differential Lossless (Sequential)

//Huffman Table Specification
#define DHT			0xFFC4  // Define Huffman Table

//Arithmetic Coding Conditioning Specification
#define DAC			0xFFCC  // Define Arithmetic Coding Conditioning

//Restart Interval Terminations
#define RST0			0xFFD0
#define RST1			0xFFD1
#define RST2			0xFFD2
#define RST3			0xFFD3
#define RST4			0xFFD4
#define RST5			0xFFD5
#define RST6			0xFFD6
#define RST7			0xFFD7

#define JPG0			0xFFF0  // Reserved for JPEG extensions
#define JPG1			0xFFF1
#define JPG2			0xFFF2
#define JPG3			0xFFF3
#define JPG4			0xFFF4
#define JPG5			0xFFF5
#define JPG6			0xFFF6
#define JPG7			0xFFF7
#define JPG8			0xFFF8
#define JPG9			0xFFF9
#define JPG10			0xFFFA
#define JPG11			0xFFFB
#define JPG12			0xFFFC
#define JPG13			0xFFFD
#define COM			0xFFFE  // Comment

#define TEM			0xFF01  // For Temporary private use in arithmetic coding
#define RES			0xFF02  // Reserved
#define RES_BF			0xFFBF  // Reserved

#define APP0			0xFFE0  // Reserved for application segments
#define APP0_Size		0x0010  // 16byte
#define APP0_01		0x4A46  // Idenrified(2byte)
#define APP0_02		0x4946  // Identified(2Byte)
#define APP0_03		0x0001  // Identified(1byte) + Version(1byte)
#define APP0_04		0x0201  // Version(1byte) + Units(1byte)
#define APP0_05		0x0000  //Xthumb(1byte) + Ythumb(1byte)

#define APP0_XX		0x5858
#define APP0_XX_01		0x0010

////// EXIF FORMAT
#define APP1			0xFFE1  // APP! Marker
#define EXIF_HEADER1	0x4578  // EXIF HEADER = 0x4578 6966 0000
#define EXIF_HEADER2	0x6966
#define EXIF_HEADER3	0x0000

#define TIFF_HEADER1	0x4949  //TIFF HEADER = 0x4949 2A00 0800 0000
#define TIFF_HEADER2	0x2A00
#define TIFF_HEADER3	0x0800
#define TIFF_HEADER4	0x0000

#define APP2			0xFFE2
#define APP3			0xFFE3
#define APP4			0xFFE4
#define APP5			0xFFE5
#define APP6			0xFFE6
#define APP7			0xFFE7
#define APP8			0xFFE8
#define APP9			0xFFE9
#define APPA			0xFFEA
#define APPB			0xFFEB
#define APPC			0xFFEC
#define APPD			0xFFED
#define APPE			0xFFEE
#define APPF			0xFFEF


/**********************************************************************/
/*																	  */
/*						EXIF TAG INFORMATION						  */
/*																	  */
/**********************************************************************/
#define BYTE_TYPE			0x0001
#define ASCII_TYPE			0x0002
#define SHORT_TYPE			0x0003
#define LONG_TYPE			0x0004
#define RATIONAL_TYPE		0x0005
#define UNDEFINED_TYPE		0x0007
#define SLONG_TYPE			0x0009
#define SRATIONAL_TYPE		0x000A

typedef enum{
	IMAGE_WIDTH                   = 0x0100,
	IMAGE_LENGTH                  = 0x0101,
	BITS_PER_SAMPLE               = 0x0102,
	COMPRESSION                   = 0x0103,
	PHOTO_METRIC_INTERPRETATION   = 0x0106,
	IMAGE_DESCRIPTION             = 0x010E,
	MAKE                          = 0x010F,
	MODEL                         = 0x0110,
	STRIP_OFFSETS                 = 0x0111,
	ORIENTATION                   = 0x0112,
	SAMPLES_PER_PIXEL             = 0x0115,
	ROWS_PER_STRIP                = 0x0116,
	STRIP_BYTE_COUNTS             = 0x0117,
	X_RESOLUTION                  = 0x011A,
	Y_RESOLUTION                  = 0x011B,
	PLANAR_CONFIGURATION          = 0x011C,
	RESOLUTION_UNIT               = 0x0128,
	TRANSFER_FUNCTION             = 0x012D,
	SOFTWARE                      = 0x0131,
	DATE_TIME                     = 0x0132,
	ARTIST                        = 0x013B,
	WHITE_POINT                   = 0x013E,
	PRIMARY_CHROMATICITIES        = 0x013F,
	JPEGINTERCHANGE_FORMAT        = 0x0201,
	JPEGINTERCHANGE_FORMAT_L      = 0x0202,
	YCBCR_COEFFICIENTS            = 0x0211,
	YCBCR_SUBSAMPLING             = 0x0212,
	YCBCR_POSITIONING             = 0x0213,
	REFERENCE_BLACK_WHITE         = 0x0214,
	COPYRIGHT                     = 0x8298,
	EXPOSURE_TIME                 = 0x829A,
	F_NUMBER                      = 0x829D,
	EXIF_IFD_POINTER              = 0x8769,
	GPS_IFD_POINTER               = 0x8825,
	EXIF_VERSION                  = 0x9000,
	EXIF_DATE_TIME_ORIGINAL       = 0x9003,
	EXIF_DATE_TIME_DIGITIZED      = 0x9004,
	EXIF_COMPONENTS_CONFIGURATION = 0x9101,
	FLASH                         = 0x9209,
	EXIF_FOCAL_LENGTH 			  = 0x920A,
	EXIF_USER_COMMENT             = 0x9286,
	EXIF_FLASHPIX_VERSION         = 0xA000,
	EXIF_COLOR_SPACE              = 0xA001,
	EXIF_PIXEL_X_DIMENSION        = 0xA002,
	EXIF_PIXEL_Y_DIMENSION        = 0xA003,
	INTEROP_IFD_POINTER           = 0xA005,
	CUSTOM_RENDERED               = 0xA401,
	EXPOSURE_MODE                 = 0xA402,
	WHITE_BALANCE                 = 0xA403,
	DIGITAL_ZOOM_RATIO            = 0xA404,
	SCENE_CAPTURE_TYPE            = 0xA406,
	SUBJECT_DISTANCE_RANGE        = 0xA40C,
	PIM_TAG                       = 0xC4A5
} exif_tag_id_type;


typedef enum {
	GPS_VERSION_ID                = 0x0000,
	GPS_LATITUDE_REF              = 0x0001,
	GPS_LATITUDE                  = 0x0002,
	GPS_LONGITUDE_REF             = 0x0003,
	GPS_LONGITUDE                 = 0x0004,
	GPS_ALTITUDE_REF              = 0x0005,
	GPS_ALTITUDE                  = 0x0006,
	GPS_TIMESTAMP                 = 0x0007,
	GPS_SATELLITES                = 0x0008,
	GPS_STATUS                    = 0x0009,
	GPS_MEASURE_MODE              = 0x000A,
	GPS_DOP                       = 0x000B,
	GPS_SPEED_REF                 = 0x000C,
	GPS_SPEED                     = 0x000D,
	GPS_TRACK_REF                 = 0x000E,
	GPS_TRACK                     = 0x000F,
	GPS_IMG_DIRECTION_REF         = 0x0010,
	GPS_IMG_DIRECTION             = 0x0011,
	GPS_MAP_DATUM                 = 0x0012,
	GPS_DEST_LATITUDE_REF         = 0x0013,
	GPS_DEST_LATITUDE             = 0x0014,
	GPS_DEST_LONGITUDE_REF        = 0x0015,
	GPS_DEST_LONGITUDE            = 0x0016,
	GPS_DEST_BEARING_REF          = 0x0017,
	GPS_DEST_BEARING              = 0x0018,
	GPS_DEST_DISTANCE_REF         = 0x0019,
	GPS_DEST_DISTANCE             = 0x001A,
	GPS_PROCESSING_METHOD         = 0x001B,
	GPS_AREA_INFORMATION          = 0x001C,
	GPS_DATA_STAMP                = 0x001D,
	GPS_DIFFERENTIAL              = 0x001E
} gps_tag_id_type;

enum {
	THUMB_SIZE_ORIGINAL=0,
	THUMB_SIZE_320X240 = 1,
	THUMB_SIZE_240X240,
	THUMB_SIZE_176X144,
	THUMB_SIZE_80X60,
	THUMB_SIZE_MAX
}; 

typedef enum
{
	JPEG_FORMAT_UNKNOWN =-1,
	NOT_JPEGFORMAT=0,
	JFIF_JPEGFORMAT,
	EXIF_JPEGFORMAT
} JPEG_FORMAT;

typedef enum
{
	JPEG_PARSER_SUCCESS=0,
	JPEG_HW_NOT_SUPPORT,         //Undefined ChromaType, SOF5, SOF6, SOF7, SOF9, SOF10, SOF1, SOF13, SOF14, SOF15
	JPEG_FILE_LENGTH_ERR,

	/* Image Format */
	JPEG_FORMAT_BASELINE,
	JPEG_FORMAT_PROGRESSIVE,
	JPEG_FORMAT_EXT_SEQUENTIAL,
	JPEG_FORMAT_LOSSLESS,

	/* Marker Handling */
	JPEG_NO_ERR,
	JPEG_MARKER_NOT_FOUND,
	JPEG_MARKER_NOT_SUPPORT,
	JPEG_WITHOUT_SOI_MARKER,
	JPEG_WITHOUT_SOF_MARKER,
	JPEG_WITHOUT_SOS_MARKER,
	JPEG_CHROMA_NOT_SUPPORT,
	JPEG_RESOLUTION_NOT_SUPPORT,
	JPEG_QTABLE_PRECISION_ERR,
	JPEG_SOF_PRECISION_ERR,
	JPEG_SOS_PRECISION_ERR,
	JPEG_SOS_COMPONENT_ERR,
	JPEG_DHT_MARKER_BROKEN,

	JPEG_PARSER_UNKNOWN_ERR
} JPEG_PARSER_STATUS;

typedef enum
{
	JPEG_THUMBNAIL_PARSER_ERROR=0,
	JPEG_THUMBNAIL_PARSER_SUCCESS
}JPEG_THUMB_PARSER_STATUS;
//-------------------------------------------------->

#endif /* _JPEGPARAM_H_ */

