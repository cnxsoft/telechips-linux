/****************************************************************************/
/* MODULE:                                                                  */
/*   jpeg_app_enc.c - JPEG H/W Codec Application                            */
/****************************************************************************/
/*                                                                          */
/*   TCC Version 0.1                                                        */
/*   Copyright (c) Telechips, Inc.                                          */
/*   ALL RIGHTS RESERVED                                                    */
/*                                                                          */
/****************************************************************************/
/* HISTORY:                                                                 */
/*     Created: 2008.03.12. GREGORY                                         */
/****************************************************************************/
#include <linux/fcntl.h>
#include <linux/kd.h>
#include <linux/apm_bios.h>
#include <linux/fb.h>
#include <linux/capability.h>
#include "linux/synclink.h"
#include <mach/bsp.h>
#include "tcc_jpeg.h"
#include "tcc_jpegenc_app.h"
#include <linux/delay.h>
#include <plat/pmap.h>

#define TCCXXX_JPEG_ENCODE
#define JPEG_ENCODER_BASE		0xF0720000

#ifdef TCCXXX_JPEG_ENCODE
extern JPEG_ENC_INFO  gJPEG_ENC_Info;

volatile char gJpegEncodeError;
exif_tag_rsp_info gTagInfo;
jpeg_encode_option_type gEncodeOption;
jpeg_header_exif_rsp gJpegHeaderExifRsp;
//extern void *pPageTable1, *pPageTable2, *pPageTable3;
uint8 gIsExifThumbnailProcess;
extern JPEG_BUFFER_CTRL gJPEGEnc_Buffer_Info;
extern void *pJpegenc_remapped_header, *pJpegenc_remapped_streamdata, *pJpegenc_remapped_header;

exif_gps_position_type position;
exif_rational_type exif_latitude[3] = 
{
	{0, 1},  /* degrees */
	{0, 1},  /* minutes */
	{0, 1}   /* seconds */
};
exif_rational_type exif_longitude[3] =
{
	{0, 1},	/* degrees */
	{0, 1},	/* minutes */
	{0, 1}	/* seconds */
};
exif_rational_type exif_timestamp[3] =
{
	{0, 1},	/* hours */
	{0, 1},	/* minutes */
	{0, 1}	/* seconds */
};
exif_rational_type exif_altitude 	= {0, 1}; /* meters */
uint8 exif_latitude_ref_info[4] 			= {'N', 0x00, 0x00, 0x00};
uint8 exif_longitude_ref_info[4] 			= {'E', 0x00, 0x00, 0x00};
uint8 exif_altitude_ref_info[4]                         = {0x00, 0x00, 0x00, 0x00};

unsigned char year[4];
unsigned char month[2];
unsigned char date[2];
unsigned char hour[2];
unsigned char minute[2];
unsigned char second[2];

/*------------------------------------------------------------*/
uint8 gManufacturerInfo[20]="Telechips";
uint8 gCopyRight[30]="Copyright. Telechips";
uint8 gModelNameInfo[20]="TELECHIPS INC";
/*------------------------------------------------------------*/

unsigned int Y_QTable[64] = {
	 8,   6,   5,	8,	12,  20,  26,  31,
	 6,   6,   7,  10,	13,  29,  30,  28,
	 7,   7,   8,  12,	20,  29,  35,  28,
	 7,   9,  11,  15,	26,  44,  40,  31,
	 9,  11,  19,  28,	34,  55,  52,  39,
	12,  18,  28,  32,	41,  52,  57,  46,
	25,  32,  39,  44,	52,  61,  60,  51,
	36,  46,  48,  49,	56,  50,  52,  50
};

unsigned int C_QTable[64] = {
	 9,   9,  12,  24,	50,  50,  50,  50,
	 9,  11,  13,  33,	50,  50,  50,  50,
	12,  13,  28,  50,	50,  50,  50,  50,
	24,  33,  50,  50,	50,  50,  50,  50,
	50,  50,  50,  50,	50,  50,  50,  50,
	50,  50,  50,  50,	50,  50,  50,  50,
	50,  50,  50,  50,	50,  50,  50,  50,
	50,  50,  50,  50,	50,  50,  50,  50
};


int hZZ[64] = {
	 0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

int q_mat[2][64];

//0xFF, 0xC0
unsigned char Sof_Info[17] = {
	0x00, 0x11, 0x08, 0x00, 0xF0, 0x01, 0x40, 0x03,
	0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11,
	0x01
};

/* ======== Luminance Huffman Table ======== */
//{0x00},
unsigned char Y_DC_Li[16] = { /* Number of huffman codes of length i(1~16) for luminance DC */
	0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char Y_DC_Vij[12] = { /* Value associated with each huffman code for luminance DC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b
};

//{0x10},
unsigned char Y_AC_Li[16] = { /* Number of huffman codes of length i(1~16) for luminance AC */
	0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03,
	0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d
};

unsigned char Y_AC_Vij[162] = { /* Value associated with each huffman code for luminance AC */
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};


/* ======== Chrominance Huffman Table ======== */

//{0x01},
unsigned char C_DC_Li[16] = {  /* Number of huffman codes of length i(1~16) for chrominance DC */
	0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char C_DC_Vij[12] = { /* Value associated with each huffman code for chrominance DC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b
};

//{0x11},
unsigned char C_AC_Li[16] = { /* Number of huffman codes of length i(1~16) for chrominance AC */
	0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04,
	0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77
};

unsigned char C_AC_Vij[162] = { /* Value associated with each huffman code for chrominance AC */
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

// 0xFF, 0xDA
unsigned char Sos_Info[10] = {
	/*0x00, 0x0C, */0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

unsigned int Make_Thumbnail_Enc_Info(unsigned int jpeg_BufferAddr, unsigned int ThumbnailWidth, unsigned int ThumbnailHeight, unsigned short factor);

void JPEG_Exif_Header_Info_Init(void)
{
	#if (1)
	unsigned int jpeg_heder_size = 0x30000;
	#else
	pmap_t pmap_jpeg_header;
	pmap_get_info("jpeg_header", &pmap_jpeg_header);
	#endif

	gEncodeOption.IsManufacturer 	= TRUE;
	gEncodeOption.IsCopyRight 		= TRUE;
	gEncodeOption.IsEncodeTime 		= TRUE;
	gEncodeOption.IsGpsInfo 		= FALSE;

	gEncodeOption.IsThumbnail 	= FALSE;
	gEncodeOption.ThumbnailInfo.thumbnail_width  = EXIF_THUMBNAIL_WIDTH;
	gEncodeOption.ThumbnailInfo.thumbnail_height = EXIF_THUMBNAIL_HEIGHT;
	gEncodeOption.ThumbnailInfo.pThumbnail_Buff = (void *)((uint32)gJPEGEnc_Buffer_Info.pBaseRawDataAddr + gJPEGEnc_Buffer_Info.pBaseRawDataSize - jpeg_heder_size);
}

/****************************************************************************
	JPEG_SW_Reset()

	Description : 
	Parameter : NONE
	Return : NONE
****************************************************************************/
void JPEG_SW_Reset(JpegOperationType mode)
{
#if !defined(CONFIG_ARCH_TCC892X)
	#if defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PVIDEOBUSCFG pVIDEOBUS = (PVIDEOBUSCFG)tcc_p2v(HwVIDEOBUSCFG_BASE);
	#else
	volatile PVIDEOCODEC pVIDEOBUS = (PVIDEOCODEC)tcc_p2v(HwVIDEOBUS_BASE);
	#endif
	if(mode == JPEG_ENCODE_MODE)
	{
		BITSET(pVIDEOBUS->SWRESET, HwVIDEOC_SWRESET_JPEG_ENCODER);  // Reset
		BITCLR(pVIDEOBUS->SWRESET, HwVIDEOC_SWRESET_JPEG_ENCODER);  // Normal
	}
	#if defined(CONFIG_ARCH_TCC92XX)	
	else  /* JPEG_DECODE_MODE */
	{
		BITSET(pVIDEOBUS->SWRESET, HwVIDEOC_SWRESET_JPEG_DECODER);  // Reset
		BITCLR(pVIDEOBUS->SWRESET, HwVIDEOC_SWRESET_JPEG_DECODER);  // Normal
	}
	#endif
#endif // CONFIG_ARCH_TCC892X
}
EXPORT_SYMBOL(JPEG_SW_Reset);


/****************************************************************************
	JPEG_INTERRUPT_Enable()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
void JPEG_INTERRUPT_Enable(JpegOperationType mode)
{
#if !defined(CONFIG_ARCH_TCC892X)
	volatile PJPEGENCODER pJPEGENC = (PJPEGENCODER)tcc_p2v(HwJPEGENCODER_BASE);
	volatile PPIC pPIC = (PPIC)tcc_p2v(HwPIC_BASE);
	if(mode == JPEG_ENCODE_MODE)
	{
		BITSET(pJPEGENC->JP_INT_MASK, HwJP_INT_MASK_YBUF_ROLLING);
		BITSET(pPIC->CLR0,  HwINT0_JPGE); 
		BITCLR(pPIC->POL0,  HwINT0_JPGE); 
		BITSET(pPIC->SEL0,  HwINT0_JPGE); 
		BITSET(pPIC->IEN0,  HwINT0_JPGE); 
		BITSET(pPIC->MODE0,  HwINT0_JPGE); 
	}
	#if defined(CONFIG_ARCH_TCC92XX)
	else
	{
		BITSET(pPIC->CLR0,  HwINT0_JPGD); 
		BITCLR(pPIC->POL0,  HwINT0_JPGD); 
		BITSET(pPIC->SEL0,  HwINT0_JPGD); 
		BITSET(pPIC->IEN0,  HwINT0_JPGD); 
		BITSET(pPIC->MODE0,  HwINT0_JPGD); 
	}
	#endif
#endif // CONFIG_ARCH_TCC892X
}

void JPEG_INTERRUPT_Disable(JpegOperationType mode)
{
#if !defined(CONFIG_ARCH_TCC892X)
	volatile PPIC pPIC = (PPIC)tcc_p2v(HwPIC_BASE);
	if(mode == JPEG_ENCODE_MODE)
	{
		BITSET(pPIC->CLR0, HwINT0_JPGE);
		BITCLR(pPIC->IEN0, HwINT0_JPGE);
		BITCLR(pPIC->SEL0, HwINT0_JPGE);
	}
	#if defined(CONFIG_ARCH_TCC92XX)
	else
	{
		BITSET(pPIC->CLR0, HwINT0_JPGD);
		BITCLR(pPIC->IEN0, HwINT0_JPGD);
		BITCLR(pPIC->SEL0, HwINT0_JPGD);
	}
	#endif
#endif // CONFIG_ARCH_TCC892X
}
EXPORT_SYMBOL(JPEG_INTERRUPT_Enable);
EXPORT_SYMBOL(JPEG_INTERRUPT_Disable);

void JPEG_VIDEOCACHE_Enable(unsigned char on, unsigned int read_min_addr, unsigned int read_max_addr, 
									unsigned int wrtie_min_addr, unsigned int wrtie_max_addr)
{
#if !defined(CONFIG_ARCH_TCC892X)
	#ifdef USE_VCACHE
	volatile PVIDEOCACHE pVIDEOCACHE = (PVIDEOCACHE)tcc_p2v(HwVIDEOCACHE_BASE);
	if(on)
	{		
		pVIDEOCACHE->R0MIN = read_min_addr;
		pVIDEOCACHE->R0MAX = read_max_addr;
		pVIDEOCACHE->R1MIN = wrtie_min_addr;
		pVIDEOCACHE->R1MAX = wrtie_max_addr;

		BITSET(pVIDEOCACHE->VCREG, (HwVIDEOCACHE_READ0_EN|HwVIDEOCACHE_WRITE0_EN|HwVIDEOCACHE_READ1_EN|HwVIDEOCACHE_WRITE1_EN));
		BITSET(pVIDEOCACHE->VCCTRL, HwVIDEOCACHE_CACHEON);
	}
	else
	{
		BITCLR(pVIDEOCACHE->VCCTRL, HwVIDEOCACHE_CACHEON);
	}
	#endif
#endif // CONFIG_ARCH_TCC892X
}

void JPEG_VIDEOCACHE_Drain(void)
{
#if !defined(CONFIG_ARCH_TCC892X)
	#ifdef USE_VCACHE
	unsigned long IFlag;
	volatile PVIDEOCACHE pVIDEOCACHE = (PVIDEOCACHE)tcc_p2v(HwVIDEOCACHE_BASE);

	BITSET(pVIDEOCACHE->VWBCTRL, HwVIDEOCACHE_VWB_DRAIN_EN);

	IFlag = pVIDEOCACHE->VWBCTRL;

	while((IFlag&0x4) == 1)
	{
		IFlag = pVIDEOCACHE->VWBCTRL;
	}

	BITSET(pVIDEOCACHE->VWBCTRL, 0x00000000); 
	BITCLR(pVIDEOCACHE->VCCTRL, HwVIDEOCACHE_CACHEON);	
	#endif
#endif // CONFIG_ARCH_TCC892X
}
EXPORT_SYMBOL(JPEG_VIDEOCACHE_Enable);
EXPORT_SYMBOL(JPEG_VIDEOCACHE_Drain);


/****************************************************************************
	tccxxx_jpeg_start()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int JPEG_ENCODE_Start(int ImageWidth, int ImageHeight, unsigned int *uiBitStreamSize, unsigned int *uiHeaderSize)
{
#if !defined(CONFIG_ARCH_TCC892X)
	volatile PJPEGENCODER pJPEGENC = (PJPEGENCODER)tcc_p2v(HwJPEGENCODER_BASE);
	BITCSET(pJPEGENC->JP_START, 0x0000000F, HwJP_START_RUN);
#endif // CONFIG_ARCH_TCC892X
	return 0;
}

int JPEG_ENCODE_Get_Result(int ImageWidth, int ImageHeight, unsigned int *uiBitStreamSize, unsigned int *uiHeaderSize)
{
	void *HeaderBuffer;

	*uiBitStreamSize = JPEG_Encoded_Data_Size();

	gEncodeOption.BitStreamSize 						= *uiBitStreamSize;
	gEncodeOption.ThumbnailInfo.thumbnail_image_size 	= 0;
	gEncodeOption.ImageWidth 						= ImageWidth;
	gEncodeOption.ImageHeight 						= ImageHeight;
	gEncodeOption.Q_FactorValue 						= JPEG_SUPERFINE;

	HeaderBuffer = pJpegenc_remapped_header;

	*uiHeaderSize = TCCXXX_JPEG_Make_Header((uint32)HeaderBuffer, &gEncodeOption, &gJpegHeaderExifRsp);
	*uiBitStreamSize = gEncodeOption.BitStreamSize;

	gIsExifThumbnailProcess = FALSE;

	return 0;
}


/****************************************************************************
	JPEG_Encoded_Data_Size()

	Description : Encoded Data에 대한 Total Size (Except JPEG Header)
				즉, Destination Buffer에 쓰여진 JPEG Data Size
	Parameter : NONE
	Return : Encoded Data Size
****************************************************************************/
unsigned int JPEG_Encoded_Data_Size(void)
{
	unsigned int uiBitStreamSize = 0;
#if !defined(CONFIG_ARCH_TCC892X)
	volatile PJPEGENCODER pJPEGENC = (PJPEGENCODER)tcc_p2v(HwJPEGENCODER_BASE);

	uiBitStreamSize = pJPEGENC->JP_DBUF_WCNT;
	uiBitStreamSize = uiBitStreamSize*16;
#endif // CONFIG_ARCH_TCC892X
	return uiBitStreamSize;
}

/****************************************************************************
	DQT_Set_Value()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
void DQT_Set_Value(int q_value)
{
#if !defined(CONFIG_ARCH_TCC892X)
	int i, j, val;
	int Wdata = 0;
	volatile PJPEGENCODER pJPEGENC = (PJPEGENCODER)tcc_p2v(HwJPEGENCODER_BASE);
	unsigned int *temp;

	// ======== Y Component ======== //
	for(i=0; i < 64; i++)
	{
		val = (Y_QTable[i] * q_value + 4) / 8;
		if(val > 255) val = 255;
		
		if(val == 1) 	q_mat[0][i] = 4095;
		else 			q_mat[0][i] = (4096 + (val >> 1)) / val;

		if(q_mat[0][i] == 16) q_mat[0][i] = 1;
	}
	
	for(i=0; i < 32; i++)
	{
		for(j=0; j < 64; j++)
		{
			if((2*i+1) == hZZ[j])
				Wdata = q_mat[0][j] << 12;
		}
		
		for(j=0; j < 64; j++)
		{
			if((2*i) == hZZ[j])
				Wdata = Wdata + q_mat[0][j];
		}

		pJPEGENC->JPC_QTAB0[i] =  Wdata;
	}

	// ======== C Component ======== //
	for(i=0; i < 64; i++)
	{
		val = (C_QTable[i] * q_value + 4) / 8;
		if(val > 255) val = 255;

		if(val == 1) 	q_mat[1][i] = 4095;
		else 			q_mat[1][i] = (4096 + (val >> 1)) / val;

		if(q_mat[1][i] == 16) q_mat[1][i] = 1;
	}
	
	for(i=0; i < 32; i=i+1)
	{
		for(j=0; j<64; j++)
		{
			if((2*i+1) == hZZ[j])
				Wdata = q_mat[1][j] << 12;
		}
		
		for(j=0; j < 64; j++)
		{
			if((2*i) == hZZ[j])
				Wdata = Wdata + q_mat[1][j];
		}

		pJPEGENC->JPC_QTAB1[i] = Wdata;
	}
#endif // CONFIG_ARCH_TCC892X
}

/****************************************************************************
	JPEG_SET_Encode_Config()

	Description : JPEG Block Initialize
	Parameter : NONE
	Return : NONE
****************************************************************************/
void JPEG_SET_Encode_Config(JPEG_ENC_INFO *jpeg_enc_info)
{
#if !defined(CONFIG_ARCH_TCC892X)
	volatile PJPEGENCODER pJPEGENC = (PJPEGENCODER)tcc_p2v(HwJPEGENCODER_BASE);
	
	JPEG_VIDEOCACHE_Enable(1, (unsigned int)gJPEGEnc_Buffer_Info.pBaseRawDataAddr, (unsigned int)gJPEGEnc_Buffer_Info.pBaseRawDataAddr + gJPEGEnc_Buffer_Info.pBaseRawDataSize, 
		(unsigned int)gJPEGEnc_Buffer_Info.pBaseBitstreamDataAddr,  (unsigned int)gJPEGEnc_Buffer_Info.pBaseBitstreamDataAddr + gJPEGEnc_Buffer_Info.pBaseBitstreamDataSize);

	BITCSET(pJPEGENC->JP_MOD, 0x000F000F, (HwJP_MOD_JPC|HwJP_MOD_MASTER));
	BITCSET(pJPEGENC->JP_INT_MASK, 0x000000FF, HwZERO);
	BITCSET(pJPEGENC->JP_INT_LEVEL, 0xFFFFFFFF, HwZERO);
	BITCSET(pJPEGENC->JP_TRG_MOD, 0x0000000F, HwJP_TRG_MOD_INT);
	BITCSET(pJPEGENC->JP_R_YBUF_ADDR, 0xFFFFFFFF, jpeg_enc_info->y_addr);
	BITCSET(pJPEGENC->JP_R_UBUF_ADDR, 0xFFFFFFFF, jpeg_enc_info->u_addr);
	BITCSET(pJPEGENC->JP_R_VBUF_ADDR, 0xFFFFFFFF, jpeg_enc_info->v_addr);
	BITCSET(pJPEGENC->JP_R_BUF_INFO, 0xFFFFFFFF, ((jpeg_enc_info->ifrm_hsize<<16)|jpeg_enc_info->ibuf_vsize));
	BITCSET(pJPEGENC->JP_SIZE, 0xFFFFFFFF, ((jpeg_enc_info->img_hsize<<16)|jpeg_enc_info->img_vsize));
	BITCSET(pJPEGENC->JP_CHROMA, 0x80000007, jpeg_enc_info->chroma); // TCC92XX Encoding Output : Only YUV422 Format
	BITCSET(pJPEGENC->JP_CBUF_ADDR, 0xFFFFFFFF, jpeg_enc_info->cbuf_addr);

	if(jpeg_enc_info->cbuf_size > 0xFFF)
		jpeg_enc_info->cbuf_size = 0xFFF;
	
	BITCSET(pJPEGENC->JP_CBUF_SIZE, 0x0000FFFF, jpeg_enc_info->cbuf_size);

	if(jpeg_enc_info->is_rolling)
	{
		BITCSET(pJPEGENC->JP_SBUF_WCNT, 0x0000FFFF, jpeg_enc_info->frame_cnt);
		BITCSET(pJPEGENC->JP_DBUF_RCNT, 0x0000FFFF, 0x000);
	}
	else
	{
		BITCSET(pJPEGENC->JP_SBUF_WCNT, 0x0000FFFF, 0xFFF);
		BITCSET(pJPEGENC->JP_DBUF_RCNT, 0x0000FFFF, 0xFFF);
	}
	
	DQT_Set_Value(jpeg_enc_info->q_value);
#endif // CONFIG_ARCH_TCC892X
}

/****************************************************************************
	exif_set_gps_position()

	Description :
	Parameter : NONE
	Return : NONE

****************************************************************************/
void EXIF_Set_GPS_Position(exif_gps_position_type *position)
{
	exif_latitude[0].numerator = position->latitude;
	if(position->latitude > 0)
		exif_latitude_ref_info[0] = 'N';
	else
		exif_latitude_ref_info[0] = 'S';
	
	exif_longitude[0].numerator = position->longitude;
	if(position->longitude > 0)
		exif_longitude_ref_info[0] = 'E';
	else
		exif_longitude_ref_info[0] = 'W';
	
	exif_altitude.numerator = position->altitude;
}

#if (0)
void EXIF_Set_GPS_Position(exif_gps_position_type *position)
{
	uint32 lat_temp1, long_temp1;
	uint32 min_temp, sec_temp;
	uint32 deg, min, sec;

	if(position->latitude > 0x200000)
	{
		/* negative value Take 2's compliment. */
		position->latitude = 0x3FFFFF-position->latitude;
		lat_temp1 = position->latitude / (4*3600);
		deg =  lat_temp1;
		min_temp = ((lat_temp1 - deg) * 60);
		min =  min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec =  sec_temp;
		exif_latitude_ref_info[0] = 'S';
	}
	else
	{
		lat_temp1 = position->latitude / (4*3600);
		deg =  lat_temp1;
		min_temp = ((lat_temp1 - deg) * 60);
		min =  min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec =  sec_temp;
		exif_latitude_ref_info[0] = 'N';
	}
	exif_latitude[0].numerator = deg;
	exif_latitude[1].numerator = min;
	exif_latitude[2].numerator = sec;

	if (position->longitude > 0x400000)
	{
		/* negative value Take 2's compliment. */
		position->longitude = 0x7FFFFF-position->longitude;
		long_temp1 = position->longitude / (4*3600);
		deg =  long_temp1;
		min_temp = ((long_temp1 - deg) * 60);
		min =  min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec =  sec_temp;
		exif_longitude_ref_info[0] = 'W';
	}
	else
	{
		long_temp1 = position->longitude / (4*3600);
		deg =  long_temp1;
		min_temp = ((long_temp1 - deg) * 60);
		min =  min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec =  sec_temp;
		exif_longitude_ref_info[0] = 'E';
	}
	exif_longitude[0].numerator = deg;
	exif_longitude[1].numerator = min;
	exif_longitude[2].numerator = sec;

	exif_altitude.numerator = position->altitude;
}

void EXIF_Set_GPS_Position(exif_gps_position_type *position)
{
	float lat_temp1, long_temp1;
	float min_temp, sec_temp;
	uint32 deg, min, sec;

	if(position->latitude > 0x200000)
	{
		/* negative value Take 2's compliment. */
		position->latitude = 0x3FFFFF-position->latitude;
		lat_temp1 = (float)position->latitude / (4*3600);
		deg = (uint32) lat_temp1;
		min_temp = ((lat_temp1 - deg) * 60);
		min = (uint32) min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec = (uint32) sec_temp;
		exif_latitude_ref_info[0] = 'S';
	}
	else
	{
		lat_temp1 = (float)position->latitude / (4*3600);
		deg = (uint32) lat_temp1;
		min_temp = ((lat_temp1 - deg) * 60);
		min = (uint32) min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec = (uint32) sec_temp;
		exif_latitude_ref_info[0] = 'N';
	}
	exif_latitude[0].numerator = deg;
	exif_latitude[1].numerator = min;
	exif_latitude[2].numerator = sec;

	if (position->longitude > 0x400000)
	{
		/* negative value Take 2's compliment. */
		position->longitude = 0x7FFFFF-position->longitude;
		long_temp1 = (float)position->longitude / (4*3600);
		deg = (uint32) long_temp1;
		min_temp = ((long_temp1 - deg) * 60);
		min = (uint32) min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec = (uint32) sec_temp;
		exif_longitude_ref_info[0] = 'W';
	}
	else
	{
		long_temp1 = (float)position->longitude / (4*3600);
		deg = (uint32) long_temp1;
		min_temp = ((long_temp1 - deg) * 60);
		min = (uint32) min_temp;
		sec_temp = ((min_temp - min) * 60);
		sec = (uint32) sec_temp;
		exif_longitude_ref_info[0] = 'E';
	}
	exif_longitude[0].numerator = deg;
	exif_longitude[1].numerator = min;
	exif_longitude[2].numerator = sec;

	exif_altitude.numerator = position->altitude;
}
#endif

/****************************************************************************
  EXIF_Set_Marker()

  Description :
  Parameter : NONE
  Return : NONE
****************************************************************************/
int EXIF_Set_Marker(unsigned int buffer, unsigned int marker)
{
	unsigned char *Exif_Set_Value;
	unsigned int StartAddress=0;

	Exif_Set_Value = (void *)(uint32)buffer;
	StartAddress = (uint32)Exif_Set_Value;

	*Exif_Set_Value ++= marker>>8;
	*Exif_Set_Value ++= marker;

	return ((uint32)Exif_Set_Value-StartAddress);
}

/****************************************************************************
  EXIF_Set_Header_Format()

  Description :
  Parameter : NONE
  Return : NONE
****************************************************************************/
int EXIF_Set_Header_Format(unsigned int buffer, unsigned int format, unsigned int count)
{
	int i;
	unsigned char *Exif_Set_Value;
	unsigned int StartAddress=0;

	Exif_Set_Value = (void *)(uint32)buffer;
	StartAddress = (uint32)Exif_Set_Value;

	for (i=0; i<count; i++)
	{
		*Exif_Set_Value ++= format>>8;
		*Exif_Set_Value ++= format;
	}

	return ((uint32)Exif_Set_Value-StartAddress);
}

/****************************************************************************
  EXIF_Set_Header_1bytes()

  Description :
  Parameter : NONE
  Return : NONE
****************************************************************************/
int EXIF_Set_Header_1bytes(unsigned int buffer, unsigned int data)
{
	unsigned char *Exif_Set_Value;
	unsigned int StartAddress=0;

	Exif_Set_Value = (void *)(uint32)buffer;
	StartAddress = (uint32)Exif_Set_Value;

	*Exif_Set_Value ++= data;
	*Exif_Set_Value ++= data>>8;

	return ((uint32)Exif_Set_Value-StartAddress);
}

/****************************************************************************
  EXIF_Set_Header_Info()

  Description :
  Parameter : NONE
  Return : NONE
****************************************************************************/
int EXIF_Set_Header_Info(unsigned int buffer, unsigned int Tag, unsigned int Type, unsigned long Count,
								unsigned int Offset, unsigned char IsData, exif_tag_rsp_info *rsp_info)
{
	unsigned char *Exif_Set_Value;
	unsigned int StartAddress=0;
	
	Exif_Set_Value = (void *)(uint32)(buffer);
	StartAddress = (uint32)Exif_Set_Value;

	/* Bytes 0-1 */ 
	*Exif_Set_Value ++= Tag;
	*Exif_Set_Value ++= Tag>>8;

	/* Bytes 2-3 */
	*Exif_Set_Value ++= Type;
	*Exif_Set_Value ++= Type>>8;

	/* Bytes 4-7 */
	*Exif_Set_Value ++= Count;
	*Exif_Set_Value ++= Count>>8;
	*Exif_Set_Value ++= 0x0000;
	*Exif_Set_Value ++= 0x0000;

	/* Bytes 8-11 */
	if (IsData == 0)
	{
		if (Offset == 0)
		{
			rsp_info->uiTagOffsetValue = (uint32)Exif_Set_Value;
			Exif_Set_Value += 2;
			*Exif_Set_Value ++= 0x0000;
			*Exif_Set_Value ++= 0x0000;
		}
		else
		{
			*Exif_Set_Value ++= Offset;
			*Exif_Set_Value ++= Offset>>8;
			*Exif_Set_Value ++= 0x0000;
			*Exif_Set_Value ++= 0x0000;
		}
	}

	rsp_info->uiCurrentPosition = ((uint32)Exif_Set_Value-StartAddress);
	return TRUE;
}

/****************************************************************************
  TCCXXX_JPEG_Make_Header()

  Description :
  Parameter : NONE
  Return : NONE
****************************************************************************/
int TCCXXX_JPEG_Make_Header(unsigned int jpeg_BufferAddr, jpeg_encode_option_type *EncodeOption, jpeg_header_exif_rsp *jpeg_header_rsp)
{
	unsigned char *Jpeg_Header=NULL;
	unsigned int JpegStartAddress;
	unsigned int Enc_Info_Size=0, Thumbnail_Enc_Info_Size=0, APP1_SIZE=0;
	unsigned char *APP1_Header_Size_Address=NULL, *refer_address=NULL;
	
	uint8 *XResolution_Offset_Address=NULL, *YResolution_Offset_Address=NULL, *ExifIFDPointer_Offset_Address=NULL;
	uint32 XResolution_Offset=0, YResolution_Offset=0, ExifIFDPointer_Offset=0;

	uint8 *ImageDescription_Offset_Address=NULL, *Make_Offset_Address=NULL, *Model_Offset_Address=NULL, *DateTime_Offset_Address=NULL, *CopyRight_Offset_Address=NULL;
	uint32 ImageDescription_Offset=0, Make_Offset=0, Model_Offset=0, DateTime_Offset=0, CopyRight_Offset=0;

	uint8 *GpsIFDPointer_Offset_Address=NULL, *NextIFDOffset_Offset_Address=NULL, *DateTimeOriginal_Offset_Address=NULL;
	uint32 GpsIFDPointer_Offset=0, NextIFDOffset_Offset=0, DateTimeOriginal_Offset=0;

	uint8 *Gps_Altitude_Offset_Address=NULL, *Gps_Latitude_Offset_Address=NULL, *Gps_Longitude_Offset_Address=NULL, *Gps_TimeStamp_Offset_Address=NULL;
	uint32 Gps_Altitude_Offset=0, Gps_Latitude_Offset=0, Gps_Longitude_Offset=0, Gps_TimeStamp_Offset=0;

	uint8 *Gps_Processing_method_Offset_Address=NULL, *Gps_DateStamp_Offset_Address=NULL;
	uint32 Gps_Processing_method_Offset=0, Gps_DateStamp_Offset=0;

	uint8 *ThumbnailImageDescription_Offset_Address=NULL, *Thumbnail_Offset_Address=NULL, *JPEGInterchangeFormatLength_Offset_Address=NULL, *ThumbnailDataOffsetAddress=NULL;
	uint32 ThumbnailImageDescription_Offset=0, Thumbnail_Offset=0, JPEGInterchangeFormatLength_Offset=0;

	uint32 uiJpegHeaderSize, ReturnValue, uiCount, uiOffset, uiIsData;
	uint8 temp, FalseCount=0;

	uint8 *FocalLength_Offset_Address=NULL;
	uint32 FocalLength_Offset=0;

	unsigned char ExifVersion[]="0220";
	unsigned char FlashpixVersion[]="0100";
	unsigned char ImageDescription[]="EXIF_JPEG_T422";


	Jpeg_Header = (uint8*)jpeg_BufferAddr;
	JpegStartAddress = (uint32)Jpeg_Header;

	ReturnValue = EXIF_Set_Marker((uint32)Jpeg_Header, SOI);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Marker((uint32)Jpeg_Header, APP1);	Jpeg_Header += ReturnValue;

	APP1_Header_Size_Address = Jpeg_Header;
	Jpeg_Header += 2;

	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, EXIF_HEADER1, 1);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, EXIF_HEADER2, 1);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, EXIF_HEADER3, 1);	Jpeg_Header += ReturnValue;

	refer_address = Jpeg_Header;

	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER1, 1); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER2, 1); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER3, 1); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER4, 1); 	Jpeg_Header += ReturnValue;

	if (EncodeOption->IsManufacturer == FALSE)    FalseCount++;
	if (EncodeOption->IsCopyRight == FALSE)        FalseCount++;
	if (EncodeOption->IsEncodeTime == FALSE)      FalseCount++;
	if (EncodeOption->IsGpsInfo == FALSE)           FalseCount++;

	*Jpeg_Header ++= (0x000C-FalseCount);            //0th IFD Tag NUMBER
	*Jpeg_Header ++= (0x000C-FalseCount)>>8;

	/* ImageDescription */
	uiCount = sizeof(ImageDescription);
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, IMAGE_DESCRIPTION, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	ImageDescription_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* Make */
	if (EncodeOption->IsManufacturer == TRUE)
	{
		uiCount = sizeof(gManufacturerInfo);
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, MAKE, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Make_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* Model Tag */
	uiCount = sizeof(gModelNameInfo);
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, MODEL, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Model_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* Orientation */
	uiCount = 1;
	uiOffset = EncodeOption->rotation;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, ORIENTATION, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* XResolution */
	uiCount = 1;
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, X_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	XResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* YResolution */
	uiCount = 1;
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, Y_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	YResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* ResolutionUnit */
	uiCount = 1;
	uiOffset = 2;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, RESOLUTION_UNIT, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* DateTime */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		uiCount = 0x0014;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, DATE_TIME, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		DateTime_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* YCbCrPositioning */
	uiCount = 1;
	uiOffset = 2;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, YCBCR_POSITIONING, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* CopyRight */
	if (EncodeOption->IsCopyRight == TRUE)
	{
		uiCount = sizeof(gCopyRight);
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, COPYRIGHT, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		CopyRight_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* Exif IFD Pointer */
	uiCount = 1;
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_IFD_POINTER, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	ExifIFDPointer_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* GPS Info IFD Pointer */
	if (EncodeOption->IsGpsInfo == TRUE)
	{
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_IFD_POINTER, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		GpsIFDPointer_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* Next IFD Offset */
	if(EncodeOption->IsThumbnail == TRUE)
	{
		NextIFDOffset_Offset_Address = Jpeg_Header;		//OFFSET
		Jpeg_Header += 2;
	}
	else
	{
		*Jpeg_Header ++= 0x0000;		// Next IFD Offset = 1st IFD Offset
		*Jpeg_Header ++= 0x0000;
	}

	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/*****************************************************************/
	/*					 0th IFD DATA INFORMATION					 */
	/*****************************************************************/

	/* ImageDescription */
	ImageDescription_Offset = Jpeg_Header-refer_address;
	*ImageDescription_Offset_Address ++= ImageDescription_Offset;
	*ImageDescription_Offset_Address ++= ImageDescription_Offset>>8;
	if (sizeof(ImageDescription) != 0)
	{
		memcpy(Jpeg_Header, &ImageDescription, sizeof(ImageDescription));
		Jpeg_Header+=sizeof(ImageDescription);
	}

	/* Make */
	if (EncodeOption->IsManufacturer == TRUE)
	{
		Make_Offset = Jpeg_Header - refer_address;
		*Make_Offset_Address ++= Make_Offset;
		*Make_Offset_Address ++= Make_Offset>>8;
		if (sizeof(gManufacturerInfo) != 0)
		{
			memcpy(Jpeg_Header, &gManufacturerInfo, sizeof(gManufacturerInfo));
			Jpeg_Header+=sizeof(gManufacturerInfo);
		}
	}

	/* Model Tag */
	Model_Offset = Jpeg_Header - refer_address;
	*Model_Offset_Address ++= Model_Offset;
	*Model_Offset_Address ++= Model_Offset>>8;
	if (sizeof(gModelNameInfo) != 0)
	{
		memcpy(Jpeg_Header, &gModelNameInfo, sizeof(gModelNameInfo));
		Jpeg_Header+=sizeof(gModelNameInfo);
	}

	/* XResolution Data */
	XResolution_Offset = Jpeg_Header - refer_address;
	*XResolution_Offset_Address ++= XResolution_Offset;
	*XResolution_Offset_Address ++= XResolution_Offset>>8;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048); 	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;

	/* YResolution Data */
	YResolution_Offset = Jpeg_Header - refer_address;
	*YResolution_Offset_Address ++= YResolution_Offset;
	*YResolution_Offset_Address ++= YResolution_Offset>>8;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048);	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;

	/* DateTime */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		DateTime_Offset = Jpeg_Header - refer_address;
		*DateTime_Offset_Address ++= DateTime_Offset;
		*DateTime_Offset_Address ++= DateTime_Offset>>8;
		
		temp = (EncodeOption->EncodingTime.year/1000); 		year[0] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/100; 	year[1] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/10; 	year[2] = temp+0x30; 
		temp = (EncodeOption->EncodingTime.year%10); 			year[3] = temp+0x30;
		memcpy(Jpeg_Header, &year, 4);
		Jpeg_Header += 4;
		*Jpeg_Header ++= 0x3A;  // ':'

		temp = EncodeOption->EncodingTime.month/10; 	 		month[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.month%10; 			month[1] = temp+0x30;
		memcpy(Jpeg_Header, &month, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x3A;  // ':'

		temp = EncodeOption->EncodingTime.date/10; 			date[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.date%10; 			date[1] = temp+0x30;
		memcpy(Jpeg_Header, &date, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x20; // Standard 규격.

		temp = EncodeOption->EncodingTime.hour/10; 			hour[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.hour%10; 			hour[1] = temp+0x30;
		memcpy(Jpeg_Header, &hour, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x3A;  // ':'

		temp = EncodeOption->EncodingTime.min/10; 			minute[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.min%10; 			minute[1] = temp+0x30;
		memcpy(Jpeg_Header, &minute, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x3A;  // ':'

		temp = EncodeOption->EncodingTime.sec/10;		 		second[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.sec%10; 			second[1] = temp+0x30;
		memcpy(Jpeg_Header, &second, 2);
		Jpeg_Header += 2;

		*Jpeg_Header ++= 0x00;
	}

	/* CopyRight */
	if (EncodeOption->IsCopyRight == TRUE)
	{
		CopyRight_Offset=Jpeg_Header-refer_address;
		*CopyRight_Offset_Address ++= CopyRight_Offset;
		*CopyRight_Offset_Address ++= CopyRight_Offset>>8;
		if (sizeof(gCopyRight) != 0)
		{
			memcpy(Jpeg_Header, &gCopyRight, sizeof(gCopyRight));
			Jpeg_Header+=sizeof(gCopyRight);
		}
		*Jpeg_Header ++= 0x00;
		*Jpeg_Header ++= 0x00;
	}

	/*****************************************************************/
	/*                                EXIF IFD                                  */
	/*****************************************************************/

	ExifIFDPointer_Offset=Jpeg_Header-refer_address;
	*ExifIFDPointer_Offset_Address ++= ExifIFDPointer_Offset;
	*ExifIFDPointer_Offset_Address ++= ExifIFDPointer_Offset>>8;

	/* Exif IFD NUMBER */
	*Jpeg_Header ++= 0x0008;
	*Jpeg_Header ++= 0x0008>>8;

	/* Exif Version */
	uiCount = 0x0004;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_VERSION, UNDEFINED_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= ExifVersion[0];		//EXIF VERSION = "0220"
	*Jpeg_Header ++= ExifVersion[1];
	*Jpeg_Header ++= ExifVersion[2];
	*Jpeg_Header ++= ExifVersion[3];

	/* DateTimeOriginal */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		uiCount = 0x0014;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_DATE_TIME_ORIGINAL, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		DateTimeOriginal_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* ComponentsConfiguration */
	uiCount = 0x0004;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_COMPONENTS_CONFIGURATION, UNDEFINED_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= 0x0201;				//DEFAULT = YCbCr
	*Jpeg_Header ++= 0x0201>>8;
	*Jpeg_Header ++= 0x0003;
	*Jpeg_Header ++= 0x0003>>8;

	/* FlashpixVersion */
	uiCount = 0x0004;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_FLASHPIX_VERSION, UNDEFINED_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= FlashpixVersion[0];	//DEFAULT = "0100"
	*Jpeg_Header ++= FlashpixVersion[1];
	*Jpeg_Header ++= FlashpixVersion[2];
	*Jpeg_Header ++= FlashpixVersion[3];

	/* Colorspace */
	uiCount = 0x0001;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_COLOR_SPACE, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= 0x0001;				//DEFAULT = sRGB
	*Jpeg_Header ++= 0x0001>>8;
	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/* Focal Length */
	uiCount = 1;
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_FOCAL_LENGTH, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	FocalLength_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* PixelXDimension */
	uiCount = 0x0001;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_PIXEL_X_DIMENSION, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= EncodeOption->ImageWidth;
	*Jpeg_Header ++= EncodeOption->ImageWidth>>8;
	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/* PixelYDimension */
	uiCount = 0x0001;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_PIXEL_Y_DIMENSION, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= EncodeOption->ImageHeight;
	*Jpeg_Header ++= EncodeOption->ImageHeight>>8;
	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/* Next IFD Offset */
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;

	/*****************************************************************/
	/*					EXIF IFD DATA Information			      */
	/*****************************************************************/

	/* DateTimeOriginal */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		DateTimeOriginal_Offset=Jpeg_Header-refer_address;
		*DateTimeOriginal_Offset_Address ++= DateTimeOriginal_Offset;
		*DateTimeOriginal_Offset_Address ++= DateTimeOriginal_Offset>>8;
		
		temp = EncodeOption->EncodingTime.year/1000; 			year[0] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/100; 	year[1] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/10; 	year[2] = temp+0x30; 
		temp = EncodeOption->EncodingTime.year%10; 			year[3] = temp+0x30;
		memcpy(Jpeg_Header, &year, 4);
		Jpeg_Header+=4;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->EncodingTime.month/10; 			month[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.month%10; 			month[1] = temp+0x30;
		memcpy(Jpeg_Header, &month, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->EncodingTime.date/10; 			date[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.date%10; 			date[1] = temp+0x30;
		memcpy(Jpeg_Header, &date, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x20; // Standard 규격.
		
		temp = EncodeOption->EncodingTime.hour/10; 			hour[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.hour%10; 			hour[1] = temp+0x30;
		memcpy(Jpeg_Header, &hour, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->EncodingTime.min/10; 			minute[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.min%10; 			minute[1] = temp+0x30;
		memcpy(Jpeg_Header, &minute, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->EncodingTime.sec/10;		 		second[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.sec%10; 			second[1] = temp+0x30;
		memcpy(Jpeg_Header, &second, 2);
		Jpeg_Header += 2;

		*Jpeg_Header++=0x00;
	}

	/* Focal Length */
	FocalLength_Offset=Jpeg_Header-refer_address;
	*FocalLength_Offset_Address ++= FocalLength_Offset;
	*FocalLength_Offset_Address ++= FocalLength_Offset>>8;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x1C);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x00);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0A);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x00);	Jpeg_Header += ReturnValue;

	/*****************************************************************/
	/*							  GPS IFD					*/
	/*****************************************************************/
	if (EncodeOption->IsGpsInfo == TRUE)
	{
		GpsIFDPointer_Offset = Jpeg_Header-refer_address;
		*GpsIFDPointer_Offset_Address ++= GpsIFDPointer_Offset;
		*GpsIFDPointer_Offset_Address ++= GpsIFDPointer_Offset>>8;
		
		/* GPS IFD NUMBER */
		*Jpeg_Header ++= 0x000A;
		*Jpeg_Header ++= 0x000A>>8;
		
		/* GPS Version ID */
		uiCount = 0x0004;
		uiOffset = 0x0202;	//GPS Version = "2.2.0.0"
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_VERSION_ID, BYTE_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		#if 1
		if(EncodeOption->GpsInfo.Latitude.degrees > 0)
			exif_latitude_ref_info[0] = 'N';
		else
			exif_latitude_ref_info[0] = 'S';

		if(EncodeOption->GpsInfo.Latitude.degrees == 0){
		    if(EncodeOption->GpsInfo.Latitude.minutes > 0)
			exif_latitude_ref_info[0] = 'N';
		    else
			exif_latitude_ref_info[0] = 'S';

		    if(EncodeOption->GpsInfo.Latitude.minutes == 0){
		        if(EncodeOption->GpsInfo.Latitude.seconds > 0)
			    exif_latitude_ref_info[0] = 'N';
		        else
			    exif_latitude_ref_info[0] = 'S';
                    }
                }

                 

		exif_latitude[0].numerator = abs(EncodeOption->GpsInfo.Latitude.degrees);
		exif_latitude[1].numerator = EncodeOption->GpsInfo.Latitude.minutes;
		exif_latitude[2].numerator = EncodeOption->GpsInfo.Latitude.seconds;
                exif_latitude[2].denominator = 10000;

		if(EncodeOption->GpsInfo.Longitude.degrees > 0)
			exif_longitude_ref_info[0] = 'E';
		else
			exif_longitude_ref_info[0] = 'W';

		if(EncodeOption->GpsInfo.Longitude.degrees == 0){
		    if(EncodeOption->GpsInfo.Longitude.minutes > 0)
			exif_longitude_ref_info[0] = 'E';
		    else
			exif_longitude_ref_info[0] = 'W';

		    if(EncodeOption->GpsInfo.Longitude.minutes == 0){
		        if(EncodeOption->GpsInfo.Longitude.seconds > 0)
			    exif_longitude_ref_info[0] = 'E';
		        else
			    exif_longitude_ref_info[0] = 'W';
                    }
                }

		exif_longitude[0].numerator = abs(EncodeOption->GpsInfo.Longitude.degrees);
		exif_longitude[1].numerator = EncodeOption->GpsInfo.Longitude.minutes;
		exif_longitude[2].numerator = EncodeOption->GpsInfo.Longitude.seconds;
		exif_longitude[2].denominator = 10000;

		exif_altitude.numerator = EncodeOption->GpsInfo.Altitude;
                if(EncodeOption->GpsInfo.Altitude >= 0)
                    exif_altitude_ref_info[0] = 0x00;
		else
                    exif_altitude_ref_info[0] = 0x01;

		#else
		position.latitude  = EncodeOption->GpsInfo.latitude;
		position.longitude = EncodeOption->GpsInfo.longitude;
		position.altitude  = EncodeOption->GpsInfo.altitude;
		EXIF_Set_GPS_Position(&position);
		#endif

		/* GPS Latitude Ref */
		uiCount = 0x0002;
		uiOffset = exif_latitude_ref_info[0];
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_LATITUDE_REF, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Latitude */
		uiCount = 0x0003;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_LATITUDE, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Gps_Latitude_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Longitude Ref */
		uiCount = 0x0002;
		uiOffset = exif_longitude_ref_info[0];
		uiIsData =0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_LONGITUDE_REF, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Longitude */
		uiCount = 0x0003;
		uiOffset = 0;
		uiIsData =0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_LONGITUDE, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Gps_Longitude_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Altitude Ref */
		uiCount = 0x0001;
		uiOffset = exif_altitude_ref_info[0];
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_ALTITUDE_REF, BYTE_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	        Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Altitude */
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_ALTITUDE, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Gps_Altitude_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS TimeStamp */
		uiCount = 0x0003;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_TIMESTAMP, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Gps_TimeStamp_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Processing Method */
		if(EncodeOption->GpsInfo.Processing_Method != NULL) {
			uiCount = sizeof(EncodeOption->GpsInfo.Processing_Method);
			uiOffset = 0;
			uiIsData =0;
			EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_PROCESSING_METHOD, UNDEFINED_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
			Gps_Processing_method_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
			Jpeg_Header += gTagInfo.uiCurrentPosition;
		}

		/* GPS DateStamp */
		uiCount = 0x000B;
		uiOffset = 0;
		uiIsData =0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_DATA_STAMP, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Gps_DateStamp_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/*****************************************************************/
		/*                     GPS IFD DATA Information                  */
		/*****************************************************************/

		/* GPS Latitude */
		Gps_Latitude_Offset = Jpeg_Header - refer_address;
		*Gps_Latitude_Offset_Address ++= Gps_Latitude_Offset;
		*Gps_Latitude_Offset_Address ++= Gps_Latitude_Offset>>8;
		memcpy(Jpeg_Header, &exif_latitude, sizeof(exif_latitude));
		Jpeg_Header += sizeof(exif_latitude);

		/* GPS Longtitude */
		Gps_Longitude_Offset = Jpeg_Header - refer_address;
		*Gps_Longitude_Offset_Address ++= Gps_Longitude_Offset;
		*Gps_Longitude_Offset_Address ++= Gps_Longitude_Offset>>8;
		memcpy(Jpeg_Header, &exif_longitude, sizeof(exif_longitude));
		Jpeg_Header += sizeof(exif_longitude);

		/* GPS Aatitude */
		Gps_Altitude_Offset = Jpeg_Header - refer_address;
		*Gps_Altitude_Offset_Address ++= Gps_Altitude_Offset;
		*Gps_Altitude_Offset_Address ++= Gps_Altitude_Offset>>8;
		memcpy(Jpeg_Header, &exif_altitude, sizeof(exif_altitude));
		Jpeg_Header += sizeof(exif_altitude);

		/* GPS TimeStamp */
		exif_timestamp[0].numerator = EncodeOption->GpsInfo.time_stamp.hour;
		exif_timestamp[1].numerator = EncodeOption->GpsInfo.time_stamp.min;
		exif_timestamp[2].numerator = EncodeOption->GpsInfo.time_stamp.sec;
		Gps_TimeStamp_Offset = Jpeg_Header - refer_address;
		*Gps_TimeStamp_Offset_Address ++= Gps_TimeStamp_Offset;
		*Gps_TimeStamp_Offset_Address ++= Gps_TimeStamp_Offset>>8;
		memcpy(Jpeg_Header, &exif_timestamp, sizeof(exif_timestamp));
		Jpeg_Header += sizeof(exif_timestamp);

		/* GPS Processing Method */
		if(EncodeOption->GpsInfo.Processing_Method != NULL) {
			Gps_Processing_method_Offset = Jpeg_Header - refer_address;
			*Gps_Processing_method_Offset_Address ++= Gps_Processing_method_Offset;
			*Gps_Processing_method_Offset_Address ++= Gps_Processing_method_Offset>>8;
			memcpy(Jpeg_Header, EncodeOption->GpsInfo.Processing_Method, sizeof(EncodeOption->GpsInfo.Processing_Method));
			Jpeg_Header += sizeof(EncodeOption->GpsInfo.Processing_Method);
		}

		/* GPS DateStamp */
		Gps_DateStamp_Offset = Jpeg_Header - refer_address;
		*Gps_DateStamp_Offset_Address ++= Gps_DateStamp_Offset;
		*Gps_DateStamp_Offset_Address ++= Gps_DateStamp_Offset>>8;

		temp = EncodeOption->GpsInfo.time_stamp.year/1000; 
		year[0] = temp+0x30;
		temp = (EncodeOption->GpsInfo.time_stamp.year%1000)/100;
        	year[1] = temp+0x30;
		temp = ((EncodeOption->GpsInfo.time_stamp.year%1000)%100)/10;
         	year[2] = temp+0x30; 
		temp = EncodeOption->GpsInfo.time_stamp.year%10;
		year[3] = temp+0x30;

		memcpy(Jpeg_Header, &year, 4);
		Jpeg_Header+=4;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->GpsInfo.time_stamp.month/10;		month[0] = temp+0x30;
		temp = EncodeOption->GpsInfo.time_stamp.month%10;		month[1] = temp+0x30;
		memcpy(Jpeg_Header, &month, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->GpsInfo.time_stamp.date/10;			date[0] = temp+0x30;
		temp = EncodeOption->GpsInfo.time_stamp.date%10;			date[1] = temp+0x30;
		memcpy(Jpeg_Header, &date, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x20; // Standard 규격.
	}

	if(EncodeOption->IsThumbnail == TRUE)
	{
		/* 1st IFD */
		NextIFDOffset_Offset = Jpeg_Header - refer_address;
		*NextIFDOffset_Offset_Address ++= NextIFDOffset_Offset;
		*NextIFDOffset_Offset_Address ++= NextIFDOffset_Offset>>8;

		/* Number of Interoperability */
		*Jpeg_Header ++= 0x0008;
		*Jpeg_Header ++= 0x0008>>8;

		/* Compression */
		uiCount = 0x0001;
		uiOffset = 0x0006;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, COMPRESSION, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* ImageDescription */
		uiCount = sizeof(ImageDescription);
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, IMAGE_DESCRIPTION, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		ThumbnailImageDescription_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* XResolution */
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, X_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		XResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* YResolution */
		uiCount = 0x0001;
		uiOffset= 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, Y_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		YResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* ResolutionUnit */
		uiCount = 0x0001;
		uiOffset = 0x0002;			//Default = 2 (Inches)
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, RESOLUTION_UNIT, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* JPEGInterchangeFormat */
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, JPEGINTERCHANGE_FORMAT, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Thumbnail_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* JPEGInterchangeFormatLength */
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, JPEGINTERCHANGE_FORMAT_L, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		JPEGInterchangeFormatLength_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* YCbCrPositioning */
		uiCount = 0x0001;
		uiOffset = 0x0002;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, YCBCR_POSITIONING, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* Next IFD Offset */
		*Jpeg_Header ++= 0x0000>>8;
		*Jpeg_Header ++= 0x0000;
		*Jpeg_Header ++= 0x0000>>8;
		*Jpeg_Header ++= 0x0000;

		/**********************************************/
		/*                   1st IFD Data                   */
		/**********************************************/

		/* ImageDescription */
		ThumbnailImageDescription_Offset = Jpeg_Header-refer_address;
		*ThumbnailImageDescription_Offset_Address ++= ThumbnailImageDescription_Offset;
		*ThumbnailImageDescription_Offset_Address ++= ThumbnailImageDescription_Offset>>8;
		if (sizeof(ImageDescription) != 0)
		{
			memcpy(Jpeg_Header, &ImageDescription, sizeof(ImageDescription));
			Jpeg_Header+=sizeof(ImageDescription);
		}

		/* XResolution Data */
		XResolution_Offset = Jpeg_Header - refer_address;
		*XResolution_Offset_Address ++= XResolution_Offset;
		*XResolution_Offset_Address ++= XResolution_Offset>>8;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048); 	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001); 	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;

		/* YResolution Data */
		YResolution_Offset = Jpeg_Header - refer_address;
		*YResolution_Offset_Address ++= YResolution_Offset;
		*YResolution_Offset_Address ++= YResolution_Offset>>8;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048);	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001);	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;

		/* JPEGInterchangeFormat */
		/* JPEGInterchangeFormatLength */

		ThumbnailDataOffsetAddress = Jpeg_Header;  //Gregory_20080301, a wedding anniversary

		Thumbnail_Offset = Jpeg_Header - refer_address;
		*Thumbnail_Offset_Address ++= Thumbnail_Offset;
		*Thumbnail_Offset_Address ++= Thumbnail_Offset>>8;

        #if !defined(CONFIG_ARCH_TCC892X)  // 2012/02/27
		Thumbnail_Enc_Info_Size = Make_Thumbnail_Enc_Info((uint32)Jpeg_Header, EncodeOption->ThumbnailInfo.thumbnail_width,
														EncodeOption->ThumbnailInfo.thumbnail_height, EncodeOption->Q_FactorValue);
		Jpeg_Header += Thumbnail_Enc_Info_Size;
		#endif // CONFIG_ARCH_TCC892X  // 20120220

		/*************************************************************/
		/*      Add Thumbnail Bitstream Data of Thumbnail Image   */
		/*************************************************************/
		jpeg_header_rsp->Thumbnail_Position = (uint32)Jpeg_Header;

		if (EncodeOption->ThumbnailInfo.thumbnail_image_size > 0)
			memcpy(Jpeg_Header, EncodeOption->ThumbnailInfo.pThumbnail_Buff, EncodeOption->ThumbnailInfo.thumbnail_image_size);

		Jpeg_Header += EncodeOption->ThumbnailInfo.thumbnail_image_size;

		JPEGInterchangeFormatLength_Offset = Jpeg_Header - ThumbnailDataOffsetAddress;
		*JPEGInterchangeFormatLength_Offset_Address ++= JPEGInterchangeFormatLength_Offset;
		*JPEGInterchangeFormatLength_Offset_Address ++= JPEGInterchangeFormatLength_Offset>>8;
	}

	/* APP1 Size Info Write */
	APP1_SIZE = Jpeg_Header - APP1_Header_Size_Address;
	*APP1_Header_Size_Address ++= APP1_SIZE>>8;
	*APP1_Header_Size_Address ++= APP1_SIZE;

	#if !defined(CONFIG_ARCH_TCC892X)
	Enc_Info_Size = Make_JPEG_Enc_Info((uint32)Jpeg_Header, EncodeOption->ImageWidth, EncodeOption->ImageHeight, EncodeOption->Q_FactorValue);
	Jpeg_Header += Enc_Info_Size;
	#endif // CONFIG_ARCH_TCC892X

	uiJpegHeaderSize = ((uint32)Jpeg_Header-JpegStartAddress);

	jpeg_header_rsp->HeaderSize = uiJpegHeaderSize;

	return uiJpegHeaderSize;
}

/****************************************************************************
  TCCXXX_JPEG_Make_Header()

  Description :
  Parameter : NONE
  Return : NONE
****************************************************************************/
#if 0
int TCCXXX_JPEG_SW_Make_Header(unsigned int jpeg_BufferAddr, jpeg_encode_option_type *EncodeOption, jpeg_header_exif_rsp *jpeg_header_rsp)
{
	unsigned char *Jpeg_Header = NULL;
	unsigned int JpegStartAddress;
	unsigned int Thumbnail_Enc_Info_Size=0, APP1_SIZE=0;
	unsigned char *APP1_Header_Size_Address = NULL, *refer_address = NULL;
	unsigned char *XResolution_Offset_Address = NULL, *YResolution_Offset_Address = NULL, *ExifIFDPointer_Offset_Address = NULL;
	unsigned int XResolution_Offset=0, YResolution_Offset=0, ExifIFDPointer_Offset=0;
	uint8 *ImageDescription_Offset_Address = NULL, *Make_Offset_Address = NULL, *Model_Offset_Address = NULL, *DateTime_Offset_Address = NULL, *CopyRight_Offset_Address = NULL;
	uint32 ImageDescription_Offset=0, Make_Offset=0, Model_Offset=0, DateTime_Offset=0, CopyRight_Offset=0;
	uint8 *GpsIFDPointer_Offset_Address = NULL, *NextIFDOffset_Offset_Address = NULL, *DateTimeOriginal_Offset_Address = NULL, *Gps_Latitude_Offset_Address = NULL, *Gps_Longitude_Offset_Address = NULL;
	uint32 GpsIFDPointer_Offset=0, NextIFDOffset_Offset=0, DateTimeOriginal_Offset=0, Gps_Latitude_Offset=0, Gps_Longitude_Offset=0;
	uint8 *ThumbnailImageDescription_Offset_Address = NULL, *Thumbnail_Offset_Address = NULL, *JPEGInterchangeFormatLength_Offset_Address = NULL, *ThumbnailDataOffsetAddress = NULL;
	uint32 ThumbnailImageDescription_Offset=0, Thumbnail_Offset=0, JPEGInterchangeFormatLength_Offset=0;
	unsigned int uiJpegHeaderSize, ReturnValue, uiCount, uiOffset, uiIsData;
	unsigned char temp, FalseCount=0;

	unsigned char ExifVersion[]="0220";
	unsigned char FlashpixVersion[]="0100";
	unsigned char ImageDescription[]="EXIF_JPEG_T422";

	Jpeg_Header = (uint8*)jpeg_BufferAddr;
	JpegStartAddress = (uint32)Jpeg_Header;

	ReturnValue = EXIF_Set_Marker((uint32)Jpeg_Header, SOI);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Marker((uint32)Jpeg_Header, APP1);	Jpeg_Header += ReturnValue;

	APP1_Header_Size_Address = Jpeg_Header;
	Jpeg_Header += 2;

	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, EXIF_HEADER1, 1);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, EXIF_HEADER2, 1);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, EXIF_HEADER3, 1);	Jpeg_Header += ReturnValue;

	refer_address = Jpeg_Header;

	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER1, 1); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER2, 1); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER3, 1); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_Format((uint32)Jpeg_Header, TIFF_HEADER4, 1); 	Jpeg_Header += ReturnValue;

	if (EncodeOption->IsManufacturer == FALSE)    FalseCount++;
	if (EncodeOption->IsCopyRight == FALSE)        FalseCount++;
	if (EncodeOption->IsEncodeTime == FALSE)      FalseCount++;
	if (EncodeOption->IsGpsInfo == FALSE)           FalseCount++;

	*Jpeg_Header ++= (0x000C-FalseCount);            //0th IFD Tag NUMBER
	*Jpeg_Header ++= (0x000C-FalseCount)>>8;

	/* ImageDescription */
	uiCount = sizeof(ImageDescription);
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, IMAGE_DESCRIPTION, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	ImageDescription_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* Make */
	if (EncodeOption->IsManufacturer == TRUE)
	{
		uiCount = sizeof(gManufacturerInfo);
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, MAKE, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Make_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* Model Tag */
	uiCount = sizeof(gModelNameInfo);
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, MODEL, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Model_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* Orientation */
	uiCount = 1;
	uiOffset = 1;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, ORIENTATION, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* XResolution */
	uiCount = 1;
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, X_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	XResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* YResolution */
	uiCount = 1;
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, Y_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	YResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* ResolutionUnit */
	uiCount = 1;
	uiOffset = 2;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, RESOLUTION_UNIT, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* DateTime */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		uiCount = 0x0014;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, DATE_TIME, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		DateTime_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* YCbCrPositioning */
	uiCount = 1;
	uiOffset = 2;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, YCBCR_POSITIONING, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* CopyRight */
	if (EncodeOption->IsCopyRight == TRUE)
	{
		uiCount = sizeof(gCopyRight);
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, COPYRIGHT, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		CopyRight_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* Exif IFD Pointer */
	uiCount = 1;
	uiOffset = 0;
	uiIsData = 0;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_IFD_POINTER, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	ExifIFDPointer_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
	Jpeg_Header += gTagInfo.uiCurrentPosition;

	/* GPS Info IFD Pointer */
	if (EncodeOption->IsGpsInfo == TRUE)
	{
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_IFD_POINTER, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		GpsIFDPointer_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* Next IFD Offset */
	if(EncodeOption->IsThumbnail == TRUE) {
		NextIFDOffset_Offset_Address = Jpeg_Header;		//OFFSET
		Jpeg_Header += 2;
	}
	else {
		*Jpeg_Header ++= 0x0000;		// Next IFD Offset = 1st IFD Offset
		*Jpeg_Header ++= 0x0000;
	}

	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/*****************************************************************/
	/*					 0th IFD DATA INFORMATION					 */
	/*****************************************************************/

	/* ImageDescription */
	ImageDescription_Offset = Jpeg_Header-refer_address;
	*ImageDescription_Offset_Address ++= ImageDescription_Offset;
	*ImageDescription_Offset_Address ++= ImageDescription_Offset>>8;
	if (sizeof(ImageDescription) != 0)
	{
		memcpy(Jpeg_Header, &ImageDescription, sizeof(ImageDescription));
		Jpeg_Header+=sizeof(ImageDescription);
	}

	/* Make */
	if (EncodeOption->IsManufacturer == TRUE)
	{
		Make_Offset = Jpeg_Header - refer_address;
		*Make_Offset_Address ++= Make_Offset;
		*Make_Offset_Address ++= Make_Offset>>8;
		if (sizeof(gManufacturerInfo) != 0)
		{
			memcpy(Jpeg_Header, &gManufacturerInfo, sizeof(gManufacturerInfo));
			Jpeg_Header+=sizeof(gManufacturerInfo);
		}
	}

	/* Model Tag */
	Model_Offset = Jpeg_Header - refer_address;
	*Model_Offset_Address ++= Model_Offset;
	*Model_Offset_Address ++= Model_Offset>>8;
	if (sizeof(gModelNameInfo) != 0)
	{
		memcpy(Jpeg_Header, &gModelNameInfo, sizeof(gModelNameInfo));
		Jpeg_Header+=sizeof(gModelNameInfo);
	}

	/* XResolution Data */
	XResolution_Offset = Jpeg_Header - refer_address;
	*XResolution_Offset_Address ++= XResolution_Offset;
	*XResolution_Offset_Address ++= XResolution_Offset>>8;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048); 	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001); 	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;

	/* YResolution Data */
	YResolution_Offset = Jpeg_Header - refer_address;
	*YResolution_Offset_Address ++= YResolution_Offset;
	*YResolution_Offset_Address ++= YResolution_Offset>>8;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048);	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;

	/* DateTime */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		DateTime_Offset = Jpeg_Header - refer_address;
		*DateTime_Offset_Address ++= DateTime_Offset;
		*DateTime_Offset_Address ++= DateTime_Offset>>8;
		
		temp = (EncodeOption->EncodingTime.year/1000); 		year[0] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/100;	year[1] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/10;	year[2] = temp+0x30; 
		temp = (EncodeOption->EncodingTime.year%10);		year[3] = temp+0x30;
		memcpy(Jpeg_Header, &year, 4);
		Jpeg_Header += 4;
		*Jpeg_Header ++= 0x3A;  // ':'

		temp = EncodeOption->EncodingTime.month/10; 		month[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.month%10;		month[1] = temp+0x30;
		memcpy(Jpeg_Header, &month, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x3A;  // ':'

		temp = EncodeOption->EncodingTime.date/10;			date[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.date%10;			date[1] = temp+0x30;
		memcpy(Jpeg_Header, &date, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x20; // Standard 규격.

		temp = EncodeOption->EncodingTime.hour/10;			hour[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.hour%10;		hour[1] = temp+0x30;
		memcpy(Jpeg_Header, &hour, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x3A;  // ':'

		temp = EncodeOption->EncodingTime.min/10;			minute[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.min%10;			minute[1] = temp+0x30;
		memcpy(Jpeg_Header, &minute, 2);
		Jpeg_Header += 2;
		*Jpeg_Header ++= 0x3A;  // ':'

		*Jpeg_Header ++= 0x30;
		*Jpeg_Header ++= 0x30;

		*Jpeg_Header ++= 0x00;
	}

	/* CopyRight */
	if (EncodeOption->IsCopyRight == TRUE)
	{
		CopyRight_Offset=Jpeg_Header-refer_address;
		*CopyRight_Offset_Address ++= CopyRight_Offset;
		*CopyRight_Offset_Address ++= CopyRight_Offset>>8;
		if (sizeof(gCopyRight) != 0)
		{
			memcpy(Jpeg_Header, &gCopyRight, sizeof(gCopyRight));
			Jpeg_Header+=sizeof(gCopyRight);
		}
		*Jpeg_Header ++= 0x00;
		*Jpeg_Header ++= 0x00;
	}

	/*****************************************************************/
	/*                                EXIF IFD                                  */
	/*****************************************************************/

	ExifIFDPointer_Offset=Jpeg_Header-refer_address;
	*ExifIFDPointer_Offset_Address ++= ExifIFDPointer_Offset;
	*ExifIFDPointer_Offset_Address ++= ExifIFDPointer_Offset>>8;

	/* Exif IFD NUMBER */
	*Jpeg_Header ++= 0x0007;
	*Jpeg_Header ++= 0x0007>>8;

	/* Exif Version */
	uiCount = 0x0004;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_VERSION, UNDEFINED_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= ExifVersion[0];		//EXIF VERSION = "0220"
	*Jpeg_Header ++= ExifVersion[1];
	*Jpeg_Header ++= ExifVersion[2];
	*Jpeg_Header ++= ExifVersion[3];

	/* DateTimeOriginal */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		uiCount = 0x0014;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_DATE_TIME_ORIGINAL, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		DateTimeOriginal_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;
	}

	/* ComponentsConfiguration */
	uiCount = 0x0004;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_COMPONENTS_CONFIGURATION, UNDEFINED_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= 0x0201;				//DEFAULT = YCbCr
	*Jpeg_Header ++= 0x0201>>8;
	*Jpeg_Header ++= 0x0003;
	*Jpeg_Header ++= 0x0003>>8;

	/* FlashpixVersion */
	uiCount = 0x0004;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_FLASHPIX_VERSION, UNDEFINED_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= FlashpixVersion[0];	//DEFAULT = "0100"
	*Jpeg_Header ++= FlashpixVersion[1];
	*Jpeg_Header ++= FlashpixVersion[2];
	*Jpeg_Header ++= FlashpixVersion[3];

	/* Colorspace */
	uiCount = 0x0001;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_COLOR_SPACE, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= 0x0001;				//DEFAULT = sRGB
	*Jpeg_Header ++= 0x0001>>8;
	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/* PixelXDimension */
	uiCount = 0x0001;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_PIXEL_X_DIMENSION, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= EncodeOption->ImageWidth;
	*Jpeg_Header ++= EncodeOption->ImageWidth>>8;
	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/* PixelYDimension */
	uiCount = 0x0001;
	uiOffset = 0;
	uiIsData = 1;
	EXIF_Set_Header_Info((uint32)Jpeg_Header, EXIF_PIXEL_Y_DIMENSION, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
	Jpeg_Header += gTagInfo.uiCurrentPosition;
	*Jpeg_Header ++= EncodeOption->ImageHeight;
	*Jpeg_Header ++= EncodeOption->ImageHeight>>8;
	*Jpeg_Header ++= 0x0000;
	*Jpeg_Header ++= 0x0000;

	/* Next IFD Offset */
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;
	ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;

	/*****************************************************************/
	/*					EXIF IFD DATA Information			      */
	/*****************************************************************/

	/* DateTimeOriginal */
	if (EncodeOption->IsEncodeTime == TRUE)
	{
		DateTimeOriginal_Offset=Jpeg_Header-refer_address;
		*DateTimeOriginal_Offset_Address ++= DateTimeOriginal_Offset;
		*DateTimeOriginal_Offset_Address ++= DateTimeOriginal_Offset>>8;
		
		temp = EncodeOption->EncodingTime.year/1000; 		year[0] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/100;	year[1] = temp+0x30;
		temp = (EncodeOption->EncodingTime.year%1000)/10;	year[2] = temp+0x30; 
		temp = EncodeOption->EncodingTime.year%10;			year[3] = temp+0x30;
		memcpy(Jpeg_Header, &year, 4);
		Jpeg_Header+=4;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->EncodingTime.month/10;		month[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.month%10;		month[1] = temp+0x30;
		memcpy(Jpeg_Header, &month, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->EncodingTime.date/10;			date[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.date%10;			date[1] = temp+0x30;
		memcpy(Jpeg_Header, &date, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x20; // Standard 규격.
		
		temp = EncodeOption->EncodingTime.hour/10;			hour[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.hour%10;		hour[1] = temp+0x30;
		memcpy(Jpeg_Header, &hour, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x3A;  // ':'
		
		temp = EncodeOption->EncodingTime.min/10;			minute[0] = temp+0x30;
		temp = EncodeOption->EncodingTime.min%10;			minute[1] = temp+0x30;
		memcpy(Jpeg_Header, &minute, 2);
		Jpeg_Header+=2;
		*Jpeg_Header++=0x3A;  // ':'
		
		*Jpeg_Header++=0x30;
		*Jpeg_Header++=0x30;

		*Jpeg_Header++=0x00;
	}

	/*****************************************************************/
	/*							  GPS IFD					*/
	/*****************************************************************/
	if (EncodeOption->IsGpsInfo == TRUE)
	{
		GpsIFDPointer_Offset = Jpeg_Header-refer_address;
		*GpsIFDPointer_Offset_Address ++= GpsIFDPointer_Offset;
		*GpsIFDPointer_Offset_Address ++= GpsIFDPointer_Offset>>8;
		
		/* GPS IFD NUMBER */
		*Jpeg_Header ++= 0x0003;
		*Jpeg_Header ++= 0x0003>>8;
		
		/* GPS Version ID */
		uiCount = 0x0004;
		uiOffset = 0x0202;	//GPS Version = "2.2.0.0"
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_VERSION_ID, BYTE_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Latitude */
		uiCount = 0x0003;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_LATITUDE, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Gps_Latitude_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* GPS Longitude */
		uiCount = 0x0003;
		uiOffset = 0;
		uiIsData =0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, GPS_LONGITUDE, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Gps_Longitude_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/*****************************************************************/
		/*                     GPS IFD DATA Information                  */
		/*****************************************************************/

		/* GPS Latitude */
		Gps_Latitude_Offset = Jpeg_Header - refer_address;
		*Gps_Latitude_Offset_Address ++= Gps_Latitude_Offset;
		*Gps_Latitude_Offset_Address ++= Gps_Latitude_Offset>>8;

		*Jpeg_Header ++= EncodeOption->GpsInfo.latitude;
		*Jpeg_Header ++= EncodeOption->GpsInfo.latitude>>8;
		*Jpeg_Header ++= EncodeOption->GpsInfo.latitude>>16;
		*Jpeg_Header ++= EncodeOption->GpsInfo.latitude>>24;

		/* GPS Longtitude */
		Gps_Longitude_Offset = Jpeg_Header - refer_address;
		*Gps_Longitude_Offset_Address ++= Gps_Longitude_Offset;
		*Gps_Longitude_Offset_Address ++= Gps_Longitude_Offset>>8;

		*Jpeg_Header ++= EncodeOption->GpsInfo.longitude;
		*Jpeg_Header ++= EncodeOption->GpsInfo.longitude>>8;
		*Jpeg_Header ++= EncodeOption->GpsInfo.longitude>>16;
		*Jpeg_Header ++= EncodeOption->GpsInfo.longitude>>24;
	}

	if(EncodeOption->IsThumbnail == TRUE)
	{
		/* 1st IFD */
		NextIFDOffset_Offset = Jpeg_Header - refer_address;
		*NextIFDOffset_Offset_Address ++= NextIFDOffset_Offset;
		*NextIFDOffset_Offset_Address ++= NextIFDOffset_Offset>>8;

		/* Number of Interoperability */
		*Jpeg_Header ++= 0x0008;
		*Jpeg_Header ++= 0x0008>>8;

		/* Compression */
		uiCount = 0x0001;
		uiOffset = 0x0006;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, COMPRESSION, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* ImageDescription */
		uiCount = sizeof(ImageDescription);
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, IMAGE_DESCRIPTION, ASCII_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		ThumbnailImageDescription_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* XResolution */
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, X_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		XResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* YResolution */
		uiCount = 0x0001;
		uiOffset= 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, Y_RESOLUTION, RATIONAL_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		YResolution_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* ResolutionUnit */
		uiCount = 0x0001;
		uiOffset = 0x0002;			//Default = 2 (Inches)
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, RESOLUTION_UNIT, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* JPEGInterchangeFormat */
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, JPEGINTERCHANGE_FORMAT, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Thumbnail_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* JPEGInterchangeFormatLength */
		uiCount = 0x0001;
		uiOffset = 0;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, JPEGINTERCHANGE_FORMAT_L, LONG_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		JPEGInterchangeFormatLength_Offset_Address = (uint8*)gTagInfo.uiTagOffsetValue;
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* YCbCrPositioning */
		uiCount = 0x0001;
		uiOffset = 0x0002;
		uiIsData = 0;
		EXIF_Set_Header_Info((uint32)Jpeg_Header, YCBCR_POSITIONING, SHORT_TYPE, uiCount, uiOffset, uiIsData, &gTagInfo);
		Jpeg_Header += gTagInfo.uiCurrentPosition;

		/* Next IFD Offset */
		*Jpeg_Header ++= 0x0000>>8;
		*Jpeg_Header ++= 0x0000;
		*Jpeg_Header ++= 0x0000>>8;
		*Jpeg_Header ++= 0x0000;

		/**********************************************/
		/*                   1st IFD Data                   */
		/**********************************************/

		/* ImageDescription */
		ThumbnailImageDescription_Offset = Jpeg_Header-refer_address;
		*ThumbnailImageDescription_Offset_Address ++= ThumbnailImageDescription_Offset;
		*ThumbnailImageDescription_Offset_Address ++= ThumbnailImageDescription_Offset>>8;
		if (sizeof(ImageDescription) != 0)
		{
			memcpy(Jpeg_Header, &ImageDescription, sizeof(ImageDescription));
			Jpeg_Header+=sizeof(ImageDescription);
		}

		/* XResolution Data */
		XResolution_Offset = Jpeg_Header - refer_address;
		*XResolution_Offset_Address ++= XResolution_Offset;
		*XResolution_Offset_Address ++= XResolution_Offset>>8;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048); 	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001); 	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000); 	Jpeg_Header += ReturnValue;

		/* YResolution Data */
		YResolution_Offset = Jpeg_Header - refer_address;
		*YResolution_Offset_Address ++= YResolution_Offset;
		*YResolution_Offset_Address ++= YResolution_Offset>>8;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0048);	Jpeg_Header += ReturnValue;    //DEFAULT = 72 DPI
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0001);	Jpeg_Header += ReturnValue;
		ReturnValue = EXIF_Set_Header_1bytes((uint32)Jpeg_Header, 0x0000);	Jpeg_Header += ReturnValue;

		/* JPEGInterchangeFormat */
		/* JPEGInterchangeFormatLength */

		ThumbnailDataOffsetAddress = Jpeg_Header;  //Gregory_20080301, a wedding anniversary

		Thumbnail_Offset = Jpeg_Header - refer_address;
		*Thumbnail_Offset_Address ++= Thumbnail_Offset;
		*Thumbnail_Offset_Address ++= Thumbnail_Offset>>8;
		
		Thumbnail_Enc_Info_Size = Make_Thumbnail_Enc_Info((uint32)Jpeg_Header, EncodeOption->ThumbnailInfo.thumbnail_width,
														EncodeOption->ThumbnailInfo.thumbnail_height, EncodeOption->Q_FactorValue);

		Jpeg_Header += Thumbnail_Enc_Info_Size;

		/*************************************************************/
		/*      Add Thumbnail Bitstream Data of Thumbnail Image   */
		/*************************************************************/
		jpeg_header_rsp->Thumbnail_Position = (uint32)Jpeg_Header;

		if (EncodeOption->ThumbnailInfo.thumbnail_image_size > 0)
			memcpy(Jpeg_Header, EncodeOption->ThumbnailInfo.pThumbnail_Buff, EncodeOption->ThumbnailInfo.thumbnail_image_size);

		Jpeg_Header += EncodeOption->ThumbnailInfo.thumbnail_image_size;

		JPEGInterchangeFormatLength_Offset = Jpeg_Header - ThumbnailDataOffsetAddress;
		*JPEGInterchangeFormatLength_Offset_Address ++= JPEGInterchangeFormatLength_Offset;
		*JPEGInterchangeFormatLength_Offset_Address ++= JPEGInterchangeFormatLength_Offset>>8;
	}

	/* APP1 Size Info Write */
	APP1_SIZE = Jpeg_Header - APP1_Header_Size_Address;
	*APP1_Header_Size_Address ++= APP1_SIZE>>8;
	*APP1_Header_Size_Address ++= APP1_SIZE;

	//Enc_Info_Size = Make_JPEG_Enc_Info((uint32)Jpeg_Header, EncodeOption->ImageWidth, EncodeOption->ImageHeight, EncodeOption->Q_FactorValue);

	//Jpeg_Header += Enc_Info_Size;

	uiJpegHeaderSize = ((uint32)Jpeg_Header-JpegStartAddress);

	jpeg_header_rsp->HeaderSize = uiJpegHeaderSize;

	return uiJpegHeaderSize;
}
#endif

/****************************************************************************
	tccxxx_jpeg_start()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int JPEG_Make_ExifHeader(TCCXXX_JPEG_ENC_EXIF_DATA *thumb_info, unsigned int virt_baseaddr)
{
	void *pVirt_BaseAddr = (void*)virt_baseaddr;

	JPEG_Exif_Header_Info_Init();

	if(thumb_info->thumbjpg_size)
		gEncodeOption.IsThumbnail = TRUE;
	
	gEncodeOption.ThumbnailInfo.pThumbnail_Buff 		= pJpegenc_remapped_streamdata;
	gEncodeOption.ThumbnailInfo.thumbnail_image_size 	= thumb_info->thumbjpg_size;
	gEncodeOption.ThumbnailInfo.thumbnail_width 		= thumb_info->thumb_width;
	gEncodeOption.ThumbnailInfo.thumbnail_height 		= thumb_info->thumb_height;
	
	gEncodeOption.BitStreamSize = thumb_info->bitstream_size;
	gEncodeOption.ImageWidth 	= thumb_info->bitstream_width;
	gEncodeOption.ImageHeight 	= thumb_info->bitstream_height;
	gEncodeOption.Q_FactorValue = JPEG_SUPERFINE;

	if(gEncodeOption.IsEncodeTime)
	{
		gEncodeOption.EncodingTime.year 	= thumb_info->time_stamp.year;
		gEncodeOption.EncodingTime.month 	= thumb_info->time_stamp.month;
		gEncodeOption.EncodingTime.date 	= thumb_info->time_stamp.date;
		gEncodeOption.EncodingTime.hour 	= thumb_info->time_stamp.hour;
		gEncodeOption.EncodingTime.min 		= thumb_info->time_stamp.min;
		gEncodeOption.EncodingTime.sec 		= thumb_info->time_stamp.sec;
	}

	gEncodeOption.rotation = thumb_info->rotation;

	memset(&gEncodeOption.GpsInfo, 0x0, sizeof(jpeg_encode_gps_info_type));
	if(thumb_info->gps_info.Supported)
	{
		gEncodeOption.IsGpsInfo 				= (unsigned char)thumb_info->gps_info.Supported;

		gEncodeOption.GpsInfo.Latitude.degrees 	= thumb_info->gps_info.Latitude.degrees;
		gEncodeOption.GpsInfo.Latitude.minutes 	= thumb_info->gps_info.Latitude.minutes;
		gEncodeOption.GpsInfo.Latitude.seconds 	= thumb_info->gps_info.Latitude.seconds;

		gEncodeOption.GpsInfo.Longitude.degrees = thumb_info->gps_info.Longitude.degrees;
		gEncodeOption.GpsInfo.Longitude.minutes = thumb_info->gps_info.Longitude.minutes;
		gEncodeOption.GpsInfo.Longitude.seconds = thumb_info->gps_info.Longitude.seconds;

		gEncodeOption.GpsInfo.Altitude 			= thumb_info->gps_info.Altitude;

		gEncodeOption.GpsInfo.time_stamp.year 	= thumb_info->gps_info.time_stamp.year;
		gEncodeOption.GpsInfo.time_stamp.month 	= thumb_info->gps_info.time_stamp.month;
		gEncodeOption.GpsInfo.time_stamp.date 	= thumb_info->gps_info.time_stamp.date;
		gEncodeOption.GpsInfo.time_stamp.hour 	= thumb_info->gps_info.time_stamp.hour;
		gEncodeOption.GpsInfo.time_stamp.min 	= thumb_info->gps_info.time_stamp.min;
		gEncodeOption.GpsInfo.time_stamp.sec 	= thumb_info->gps_info.time_stamp.sec;

		if(&thumb_info->gps_info.Processing_Method != NULL) {
			memcpy(gEncodeOption.GpsInfo.Processing_Method, "ASCII", 0x05);
			memcpy(gEncodeOption.GpsInfo.Processing_Method+0x08, thumb_info->gps_info.Processing_Method, strlen(thumb_info->gps_info.Processing_Method)+1);
		} else {
			*gEncodeOption.GpsInfo.Processing_Method = NULL;
		}
	}
	thumb_info->header_size = TCCXXX_JPEG_Make_Header((uint32)pVirt_BaseAddr, &gEncodeOption, &gJpegHeaderExifRsp);

	return thumb_info->header_size;
}


/****************************************************************************
	Make_JPEG_Enc_Info()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int Make_JPEG_Enc_Info(unsigned int jpeg_BufferAddr, unsigned int Width, unsigned int Height, unsigned short factor)
{
	unsigned char *Jpeg_Enc_Info;
	int i;
	unsigned int startAdd=0, Enc_Info_Size=0;
	int val;

	Jpeg_Enc_Info = (void *)(uint32)(jpeg_BufferAddr);
	startAdd = (uint32)Jpeg_Enc_Info;

	/* DQT */
	*Jpeg_Enc_Info ++= DQT>>8;
	*Jpeg_Enc_Info ++= DQT;

	*Jpeg_Enc_Info ++= (64+64+2+2)>>8;
	*Jpeg_Enc_Info ++= (64+64+2+2);
	*Jpeg_Enc_Info ++= 0x00;

	for(i=0; i<64; i++) {
		val = (((Y_QTable[i]*factor)+4)/8);
		if (val > 255)	 val = 255;
		*Jpeg_Enc_Info ++= val;
	}

	*Jpeg_Enc_Info++=0x01;

	for(i=0; i<64; i++) {
		val = (((C_QTable[i]*factor)+4)/8);
		if (val > 255)	 val = 255;
		*Jpeg_Enc_Info ++= val;
	}

	/* DCT */
	*Jpeg_Enc_Info ++= SOF>>8;
	*Jpeg_Enc_Info ++= SOF;
	memcpy(Jpeg_Enc_Info, &Sof_Info, 3);
	Jpeg_Enc_Info += 3;

	// IMAGE SIZE Info.
	*Jpeg_Enc_Info ++= (Height)>>8;
	*Jpeg_Enc_Info ++= (Height);
	*Jpeg_Enc_Info ++= (Width)>>8;
	*Jpeg_Enc_Info ++= (Width);

	memcpy(Jpeg_Enc_Info, &Sof_Info[7], sizeof(Sof_Info)-7);
	Jpeg_Enc_Info += sizeof(Sof_Info)-7;

	/* DHT */
	*Jpeg_Enc_Info ++= DHT>>8;
	*Jpeg_Enc_Info ++= DHT;
	*Jpeg_Enc_Info ++= (sizeof(Y_DC_Li)+sizeof(Y_DC_Vij)+sizeof(Y_AC_Li)+sizeof(Y_AC_Vij)+
						sizeof(C_DC_Li)+sizeof(C_DC_Vij)+sizeof(C_AC_Li)+sizeof(C_AC_Vij)+6)>>8;
	*Jpeg_Enc_Info ++= (sizeof(Y_DC_Li)+sizeof(Y_DC_Vij)+sizeof(Y_AC_Li)+sizeof(Y_AC_Vij)+
						sizeof(C_DC_Li)+sizeof(C_DC_Vij)+sizeof(C_AC_Li)+sizeof(C_AC_Vij)+6);

	*Jpeg_Enc_Info++=0x00;
	memcpy(Jpeg_Enc_Info, &Y_DC_Li, sizeof(Y_DC_Li));
	Jpeg_Enc_Info += sizeof(Y_DC_Li);

	memcpy(Jpeg_Enc_Info, &Y_DC_Vij, sizeof(Y_DC_Vij));
	Jpeg_Enc_Info += sizeof(Y_DC_Vij);

	*Jpeg_Enc_Info ++= 0x10;
	memcpy(Jpeg_Enc_Info, &Y_AC_Li, sizeof(Y_AC_Li));
	Jpeg_Enc_Info += sizeof(Y_AC_Li);

	memcpy(Jpeg_Enc_Info, &Y_AC_Vij, sizeof(Y_AC_Vij));
	Jpeg_Enc_Info += sizeof(Y_AC_Vij);

	*Jpeg_Enc_Info ++= 0x01;
	memcpy(Jpeg_Enc_Info, &C_DC_Li, sizeof(C_DC_Li));
	Jpeg_Enc_Info += sizeof(C_DC_Li);

	memcpy(Jpeg_Enc_Info, &C_DC_Vij, sizeof(C_DC_Vij));
	Jpeg_Enc_Info += sizeof(C_DC_Vij);

	*Jpeg_Enc_Info ++= 0x11;
	memcpy(Jpeg_Enc_Info, &C_AC_Li, sizeof(C_AC_Li));
	Jpeg_Enc_Info += sizeof(C_AC_Li);

	memcpy(Jpeg_Enc_Info, &C_AC_Vij, sizeof(C_AC_Vij));
	Jpeg_Enc_Info += sizeof(C_AC_Vij);

	/* SOS */
	*Jpeg_Enc_Info ++= SOS>>8;
	*Jpeg_Enc_Info ++= SOS;
	*Jpeg_Enc_Info ++= (sizeof(Sos_Info)+2)>>8;
	*Jpeg_Enc_Info ++= (sizeof(Sos_Info)+2);
	memcpy(Jpeg_Enc_Info, &Sos_Info, sizeof(Sos_Info));
	Jpeg_Enc_Info += sizeof(Sos_Info);

	Enc_Info_Size = ((uint32)Jpeg_Enc_Info-startAdd);

	return Enc_Info_Size;
}

unsigned int Make_Thumbnail_Enc_Info(unsigned int jpeg_BufferAddr, unsigned int ThumbnailWidth, unsigned int ThumbnailHeight, unsigned short factor)
{
	unsigned char *Jpeg_Thumbnail_Enc_Info;
	int i;
	unsigned int thumbnail_startAdd=0, Thumbnail_Enc_Info_Size=0;
	int val;

	Jpeg_Thumbnail_Enc_Info = (void *)(uint32)(jpeg_BufferAddr);
	thumbnail_startAdd = (uint32)Jpeg_Thumbnail_Enc_Info;

	/* Start of Image = 0xFFD8 */
	*Jpeg_Thumbnail_Enc_Info ++= SOI>>8;
	*Jpeg_Thumbnail_Enc_Info ++= SOI;

	//DQT // Make Quantization Table from Y_QTable and C_QTable
	*Jpeg_Thumbnail_Enc_Info ++= DQT>>8;
	*Jpeg_Thumbnail_Enc_Info ++= DQT;

	*Jpeg_Thumbnail_Enc_Info ++= (64+64+2+2)>>8;
	*Jpeg_Thumbnail_Enc_Info ++= (64+64+2+2);

	*Jpeg_Thumbnail_Enc_Info ++= 0x00;

	for(i=0; i<64; i++) {
		val = (((Y_QTable[i]*factor)+4)/8);///100;
		if (val > 255)	 val = 255;
		*Jpeg_Thumbnail_Enc_Info ++= val;
	}

	*Jpeg_Thumbnail_Enc_Info ++= 0x01;

	for(i=0; i<64; i++) {
		val = (((C_QTable[i]*factor)+4)/8);///100;
		if (val > 255)	 val = 255;
		*Jpeg_Thumbnail_Enc_Info ++= val;
	}

	// DCT
	*Jpeg_Thumbnail_Enc_Info ++= SOF>>8;
	*Jpeg_Thumbnail_Enc_Info ++= SOF;
	memcpy(Jpeg_Thumbnail_Enc_Info, &Sof_Info, 3); //0x00, 0x11, 0x08
	Jpeg_Thumbnail_Enc_Info+=3;

	// THUMBNAIL IMAGE SIZE Info.
	*Jpeg_Thumbnail_Enc_Info ++= (ThumbnailHeight)>>8;
	*Jpeg_Thumbnail_Enc_Info ++= (ThumbnailHeight);
	*Jpeg_Thumbnail_Enc_Info ++= (ThumbnailWidth)>>8;
	*Jpeg_Thumbnail_Enc_Info ++= (ThumbnailWidth);

	memcpy(Jpeg_Thumbnail_Enc_Info, &Sof_Info[7], sizeof(Sof_Info)-7); //7: 3byte + 4byte(Width(2)+Height(2))
	Jpeg_Thumbnail_Enc_Info += sizeof(Sof_Info)-7;

	//DHT
	*Jpeg_Thumbnail_Enc_Info ++= DHT>>8;
	*Jpeg_Thumbnail_Enc_Info ++= DHT;
	*Jpeg_Thumbnail_Enc_Info ++= (sizeof(Y_DC_Li)+sizeof(Y_DC_Vij)+sizeof(Y_AC_Li)+sizeof(Y_AC_Vij)+
	sizeof(C_DC_Li)+sizeof(C_DC_Vij)+sizeof(C_AC_Li)+sizeof(C_AC_Vij)+6)>>8;
	*Jpeg_Thumbnail_Enc_Info ++= (sizeof(Y_DC_Li)+sizeof(Y_DC_Vij)+sizeof(Y_AC_Li)+sizeof(Y_AC_Vij)+
	sizeof(C_DC_Li)+sizeof(C_DC_Vij)+sizeof(C_AC_Li)+sizeof(C_AC_Vij)+6);

	*Jpeg_Thumbnail_Enc_Info ++= 0x00;
	memcpy(Jpeg_Thumbnail_Enc_Info, &Y_DC_Li, sizeof(Y_DC_Li));
	Jpeg_Thumbnail_Enc_Info += sizeof(Y_DC_Li);

	memcpy(Jpeg_Thumbnail_Enc_Info, &Y_DC_Vij, sizeof(Y_DC_Vij));
	Jpeg_Thumbnail_Enc_Info += sizeof(Y_DC_Vij);

	*Jpeg_Thumbnail_Enc_Info ++= 0x10;
	memcpy(Jpeg_Thumbnail_Enc_Info, &Y_AC_Li, sizeof(Y_AC_Li));
	Jpeg_Thumbnail_Enc_Info += sizeof(Y_AC_Li);

	memcpy(Jpeg_Thumbnail_Enc_Info, &Y_AC_Vij, sizeof(Y_AC_Vij));
	Jpeg_Thumbnail_Enc_Info += sizeof(Y_AC_Vij);

	*Jpeg_Thumbnail_Enc_Info ++= 0x01;
	memcpy(Jpeg_Thumbnail_Enc_Info, &C_DC_Li, sizeof(C_DC_Li));
	Jpeg_Thumbnail_Enc_Info += sizeof(C_DC_Li);

	memcpy(Jpeg_Thumbnail_Enc_Info, &C_DC_Vij, sizeof(C_DC_Vij));
	Jpeg_Thumbnail_Enc_Info += sizeof(C_DC_Vij);

	*Jpeg_Thumbnail_Enc_Info ++= 0x11;
	memcpy(Jpeg_Thumbnail_Enc_Info, &C_AC_Li, sizeof(C_AC_Li));
	Jpeg_Thumbnail_Enc_Info += sizeof(C_AC_Li);

	memcpy(Jpeg_Thumbnail_Enc_Info, &C_AC_Vij, sizeof(C_AC_Vij));
	Jpeg_Thumbnail_Enc_Info += sizeof(C_AC_Vij);

	//SOS
	*Jpeg_Thumbnail_Enc_Info ++= SOS>>8;
	*Jpeg_Thumbnail_Enc_Info ++= SOS;
	*Jpeg_Thumbnail_Enc_Info ++= (sizeof(Sos_Info)+2)>>8;
	*Jpeg_Thumbnail_Enc_Info ++= (sizeof(Sos_Info)+2);

	memcpy(Jpeg_Thumbnail_Enc_Info, &Sos_Info, sizeof(Sos_Info));
	Jpeg_Thumbnail_Enc_Info += sizeof(Sos_Info);

	Thumbnail_Enc_Info_Size = Jpeg_Thumbnail_Enc_Info - thumbnail_startAdd;

	return Thumbnail_Enc_Info_Size;
}
#endif
