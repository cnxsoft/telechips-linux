/****************************************************************************/
/* MODULE:                                                                  */
/*   jpeg_app_dec.c - JPEG H/W Codec Application                            */
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
#include <linux/rtc.h>
#include <linux/apm_bios.h>
#include <linux/fb.h>
#include <linux/capability.h>
#include "linux/synclink.h"
#include <mach/bsp.h>
#include "tcc_jpeg.h"
#include "tcc_jpegdec_app.h"
#include "tcc_jpegenc_app.h"
#include <mach/tcc_scaler_ioctrl.h>


#define TCCXXX_JPEG_DECODE

#ifdef TCCXXX_JPEG_DECODE

extern JPEG_DEC_INFO  gJPEG_DEC_Info;
extern JPEG_DISPLAY_INFO JPEG_Info;
extern JPEG_BUFFER_CTRL gJPEGDec_Buffer_Info;
extern jpeg_parser_rsp gJpegInfo;

extern void *pJpegdec_remapped_header;


int NCFRAM; 	// Number of Components in FRAME
uint8 SOICHK, SOFCHK, SOSCHK, EOICHK;

uint8 CHROMATYPE;

struct COMP_PARA{
	int ID;
	int HSAMP;	   // horizontal sampling factor
	int VSAMP;	   // vertical sampling factor
	int QID;	   // quantize table id
	int HS_COMP;   // horizontal size of component
	int VS_COMP;   // vertical size of component
	int DCHUFFID;  // huffman table id for DC coefficients
	int ACHUFFID;  // huffman table id for AC coefficients
} COMP[4];

struct SCOMP_PARA{
	int  ID; 
	int YH_POS;	// horizontal position(0, 1) for Y
	int YV_POS;	// vertical position(0, 1) for Y
	int PRED;	// DC prediction value
	int QID;
	int DCHUFFID;
	int ACHUFFID;
} SCOMP[6];

const static unsigned char dht_data[] = {
  0xff, 0xc4, 0x01, 0xa2, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
  0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x01, 0x00, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
  0x0a, 0x0b, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05,
  0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04,
  0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22,
  0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15,
  0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36,
  0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
  0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
  0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
  0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95,
  0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
  0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2,
  0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5,
  0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
  0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
  0xfa, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05,
  0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
  0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
  0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33,
  0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25,
  0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
  0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
  0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
  0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
  0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94,
  0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
  0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
  0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
  0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
  0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};

/****************************************************************************
	tccxxx_jpeg_decode_start()

	Description : JPEG Decode Mode.
	JPEG Decoding Start & LCD Display
	Parameter : NONE
	Return : NONE
****************************************************************************/
int JPEG_DECODE_Start(JpegOperationType type)
{
	BITCSET(HwJPEGDEC->JP_START, 0x0000000F, HwJP_START_RUN);

	return 0;
}

/****************************************************************************
	JPEG_SET_Decode_Config()

	Description : JPEG Decode Mode.
	JPEG Decoding을 위한 Initialize
	Parameter : NONE
	Return : NONE
****************************************************************************/
int JPEG_SET_Decode_Config (JPEG_DEC_INFO *jpeg_dec_info)
{
	char *pBase0_Decode, *pBase1_Decode, *pBase2_Decode;

	pBase0_Decode = jpeg_dec_info->lcd_buffer;
	pBase1_Decode = (char*)GET_ADDR_YUV42X_spU((unsigned int)pBase0_Decode , jpeg_dec_info->ifrm_width, jpeg_dec_info->ibuf_height );//(char *)((unsigned int)pBase0_Decode + (jpeg_dec_info->ifrm_width*jpeg_dec_info->ibuf_height));

	if (jpeg_dec_info->ImageFormat == HwJP_CHROMA_420)
	{
		pBase2_Decode = (char*)GET_ADDR_YUV420_spV((unsigned int)pBase1_Decode , jpeg_dec_info->ifrm_width, jpeg_dec_info->ibuf_height );//(char *)((unsigned int)pBase1_Decode + ((jpeg_dec_info->ifrm_width*jpeg_dec_info->ibuf_height)/4));
	}
	else
	{
		pBase2_Decode = (char*)GET_ADDR_YUV422_spV((unsigned int)pBase1_Decode , jpeg_dec_info->ifrm_width, jpeg_dec_info->ibuf_height );//(char *)((unsigned int)pBase1_Decode + ((jpeg_dec_info->ifrm_width*jpeg_dec_info->ibuf_height)/2));
	}

	JPEG_VIDEOCACHE_Enable(1, (unsigned int)gJPEGDec_Buffer_Info.pBaseBitstreamDataAddr,  (unsigned int)gJPEGDec_Buffer_Info.pBaseBitstreamDataAddr + gJPEGDec_Buffer_Info.pBaseBitstreamDataSize,
							(unsigned int)gJPEGDec_Buffer_Info.pBaseRawDataAddr, (unsigned int)gJPEGDec_Buffer_Info.pBaseRawDataAddr + gJPEGDec_Buffer_Info.pBaseRawDataSize);

	BITCSET(HwJPEGDEC->JP_MOD, 0x000F000F, HwJP_MOD_JPD | HwJP_MOD_MASTER);
	BITCSET(HwJPEGDEC->JP_INT_MASK, 0x000000FF, HwZERO);
	BITCSET(HwJPEGDEC->JP_INT_LEVEL, 0xFFFFFFFF, HwZERO);
	BITCSET(HwJPEGDEC->JP_TRG_MOD, 0x0000000F, HwJP_TRG_MOD_INT);
	BITCSET(HwJPEGDEC->JP_R_YBUF_ADDR, 0xFFFFFFFF, (uint32)pBase0_Decode);
	BITCSET(HwJPEGDEC->JP_R_UBUF_ADDR, 0xFFFFFFFF, (uint32)pBase1_Decode);
	BITCSET(HwJPEGDEC->JP_R_VBUF_ADDR, 0xFFFFFFFF, (uint32)pBase2_Decode);
	BITCSET(HwJPEGDEC->JP_CBUF_ADDR, 0xFFFFFFFF, jpeg_dec_info->cbuf_addr);
	BITCSET(HwJPEGDEC->JP_CBUF_SIZE, 0xFFFFFFFF, jpeg_dec_info->cbuf_size);
	BITCSET(HwJPEGDEC->JPD_OUT_SCL, 0xFFFFFFFF, jpeg_dec_info->scale);
	BITCSET(HwJPEGDEC->JP_R_BUF_INFO, 0xFFFFFFFF, ((jpeg_dec_info->ifrm_width<<16)|jpeg_dec_info->ibuf_height));
	//BITCSET(HwJPEGDEC->JP_SBUF_RP_A, 0xFFFFFFFF, HwZERO);
	//BITCSET(HwJPEGDEC->JP_DBUF_WP_A, 0xFFFFFFFF, HwZERO);
	BITCSET(HwJPEGDEC->JP_SBUF_WCNT, 0xFFFFFFFF, 0xFFF);
	BITCSET(HwJPEGDEC->JP_DBUF_RCNT, 0xFFFFFFFF, 0xFFF);

	//JPEG_Info.JpegDisplayWidth = jpeg_dec_info->ifrm_width;
	//JPEG_Info.JpegDisplayHeight = jpeg_dec_info->ibuf_height;

	//if ((jpeg_dec_info->ImageFormat == HwJP_CHROMA_420) || (jpeg_dec_info->ImageFormat == HwJP_CHROMA_422S))
	//	gChromaInfo = HwJP_CHROMA_420;
	//else if (jpeg_dec_info->ImageFormat == HwJP_CHROMA_YONLY)
	//	gChromaInfo = HwJP_CHROMA_YONLY;
	//else //HwJP_CHROMA_422, HwJP_CHROMA_444
	//	gChromaInfo = HwJP_CHROMA_422;

	return 0;
}

	


	



/****************************************************************************
	JPEG_Dec_Scale_Set()

	Description : JPEG Decode Mode.
	JPEG Decoding시 Decoder Scale Value Set.
	Parameter : NONE
	Return : Vertical Size
****************************************************************************/
unsigned int JPEG_Dec_Scale_Set(unsigned short usWidth, unsigned short usHeight, unsigned char ImageFormat)
{
	unsigned int uiTotalSize;
	unsigned char ucDecScale;

	uiTotalSize = usWidth*usHeight*2;

	ucDecScale = 1;

	return ucDecScale;
}

/****************************************************************************
	JPEG_Dec_IFrame_Width()

	Description :
	Parameter :
	Return :
****************************************************************************/
unsigned int JPEG_Dec_IFrame_Width(unsigned char ucScale, unsigned short usJpegHsize)
{
	unsigned int uiIFrameWidth=0;

	switch(ucScale)
	{
		case 0:
			uiIFrameWidth = usJpegHsize;
			break;
		case 1:
			uiIFrameWidth = usJpegHsize/2;
			break;
		case 2:
			uiIFrameWidth = usJpegHsize/4;
			break;
		default:
			break;
	}
	return uiIFrameWidth;
}

/****************************************************************************
	JPEG_Dec_IBuf_Height()

	Description :
	Parameter :
	Return :
****************************************************************************/
unsigned int JPEG_Dec_IBuf_Height(unsigned char ucScale, unsigned short usJpegVsize)
{
	unsigned int uiIBufHeight=0;

	switch(ucScale)
	{
		case 0:
			uiIBufHeight = usJpegVsize;
			break;
		case 1:
			uiIBufHeight = usJpegVsize/2;
			break;
		case 2:
			uiIBufHeight = usJpegVsize/4;
			break;
		default:
			break;
	}
	return uiIBufHeight;
}

/****************************************************************************
	ReadMarkerInfo()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int ReadMarkerInfo(unsigned char *point)
{
	unsigned int markerdata;

	markerdata = (((uint32)*point & 0x00FF)<<8) |((uint32)*(point+1) & 0x00FF);
	return markerdata;
}

/****************************************************************************
	JumpPointToDQT()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int JumpPointToDQT(unsigned char *point,int Read_Size,int file_length)
{
	int jumppointval=0;
	unsigned int checkdata1,checkdata2;

	while(Read_Size < file_length)
	{
		checkdata1 = (((uint32)*point & 0x00FF)<<8) | ((uint32)*(point+1) & 0x00FF);
		checkdata2 = (((uint32)*(point+1) & 0x00FF)<<8) | ((uint32)*(point+2) & 0x00FF);
		if (checkdata1 == DQT)
			break;
		else if (checkdata2 == DQT)
		{
			jumppointval++;
			break;
		}
		else
		{
			jumppointval++;
			point++;
			Read_Size++;
		}
	}
	return jumppointval;
}

/****************************************************************************
	ReadSizeOffset()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
unsigned int ReadSizeOffset(unsigned char *point)
{
	unsigned int offsetsize;

	offsetsize = (((uint32)*point & 0x00FF)<<8) | ((uint32)*(point+1) & 0x00FF);
	return offsetsize;
}

/****************************************************************************
	CheckJXFF()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int CheckJXFF(unsigned char *point)
{
	if ((ReadMarkerInfo(point+2)==0x4A46)&&(ReadMarkerInfo(point+4)==0x5858))
		return 1;
	else
		return 0;
}

/****************************************************************************
	CheckRESMarker()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
unsigned int CheckRESMarker(int MarkerValue) // Check Reserved Markers
{
	if((MarkerValue>=0xFF02) && (MarkerValue<=0xFFBF))
		return 1;
	else
		return 0;
}

/****************************************************************************
	ReadImageHeight()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
unsigned int ReadImageHeight(unsigned char *point)
{
	unsigned int height;

	height = (((uint32)*(point+3) & 0x00FF)<<8) | ((uint32)*(point+4) & 0x00FF);
	return height; //JPEG Decoder Input Parameter
}

/****************************************************************************
	ReadImageWidth()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
unsigned int ReadImageWidth(unsigned char *point)
{
	unsigned int width;

	width = (((uint32)*(point+5) & 0x00FF)<<8) | ((uint32)*(point+6) & 0x00FF);
	return width; //JPEG Decoder Input Parameter
}

/****************************************************************************
	Parser_Reg_Restart_Set()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int Parser_Reg_Restart_Set(unsigned char *Jpeg_Header, int flage_restart)
{
	uint32 RestartInterval;
	uint8 interval[2];

	if(flage_restart)
	{
		Jpeg_Header+=2; // FFDD : Lr(2) : Define restart interval segment length
		interval[0]=*Jpeg_Header++;
		interval[1]=*Jpeg_Header++;
		RestartInterval = ((uint32)interval[0]<<8 ) | ((uint32)interval[1]);
		BITCSET(HwJPEGDEC->JPD_RST_INTV, 0xFFFFFFFF, RestartInterval);
	}
	else
	{
		BITCSET(HwJPEGDEC->JPD_RST_INTV, 0xFFFFFFFF, 0x00000000);
	}
	return JPEG_NO_ERR;
}

/****************************************************************************
	Parser_Reg_Chroma_Set()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int Parser_Reg_Chroma_Set(unsigned char *Jpeg_Header)
{
	uint8 in_code;
	int i, j, nblk;
	uint32 max_hsamp, max_vsamp;
	uint16 HS_IMG, VS_IMG;

	Jpeg_Header++;	// header length MSB
	Jpeg_Header++;	// header length LSB
		 
	in_code = *Jpeg_Header++;
	if(in_code !=8 )
		return JPEG_SOF_PRECISION_ERR;	// Precision is %d, it must be 8 !!!

	VS_IMG = *Jpeg_Header++<<8;
	VS_IMG = VS_IMG + *Jpeg_Header++;	// Maximum number of line 
	HS_IMG = *Jpeg_Header++<<8;
	HS_IMG = HS_IMG + *Jpeg_Header++;	// Max. number of samples per line

	if( HS_IMG > MAX_RESOLUTION_WIDTH || VS_IMG > MAX_RESOLUTION_HEIGHT || HS_IMG < 8 || VS_IMG < 8)
		return JPEG_RESOLUTION_NOT_SUPPORT;

	NCFRAM = *Jpeg_Header++;	// Number of Image Components in Frame

	max_hsamp = 0;
	max_vsamp = 0;
	nblk = 0;

	for (i=0; i<NCFRAM; i++)
	{
		COMP[i].ID = *Jpeg_Header++;		 // Component Identifier
		in_code = *Jpeg_Header++;
		COMP[i].HSAMP = (in_code>>4)&0xF; // Horizontal Sampling Factor
		COMP[i].VSAMP = in_code&0xF;		 // Vertical Sampling Factor
		if (COMP[i].HSAMP > max_hsamp) max_hsamp = COMP[i].HSAMP;
		if (COMP[i].VSAMP > max_vsamp) max_vsamp = COMP[i].VSAMP;
		COMP[i].QID = *Jpeg_Header++;	 // Quant Table Destination Selector

		nblk += COMP[i].HSAMP * COMP[i].VSAMP;
	}

	for (i=0; i<NCFRAM; i++)
	{
		COMP[i].HS_COMP = (HS_IMG * COMP[i].HSAMP)/max_hsamp;
		j = (HS_IMG * COMP[i].HSAMP) % max_hsamp;
		if (j!=0) COMP[i].HS_COMP++;
		COMP[i].VS_COMP = (VS_IMG * COMP[i].VSAMP)/max_vsamp;
		j = (VS_IMG * COMP[i].VSAMP) % max_vsamp;
		if (j!=0) COMP[i].VS_COMP++;
	}

	if(NCFRAM==1)									   CHROMATYPE = HwJP_CHROMA_YONLY;	// Y_Only
	else if(max_hsamp==2 && max_vsamp==2 && nblk==6)   CHROMATYPE = HwJP_CHROMA_420;	// YUV420
	else if(max_hsamp==2 && max_vsamp==1 && nblk==4)   CHROMATYPE = HwJP_CHROMA_422;	// YUV422
	else if(max_hsamp==1 && max_vsamp==1 && nblk==3)   CHROMATYPE = HwJP_CHROMA_444;	// YUV444
	else if(max_hsamp==1 && max_vsamp==2 && nblk==4)   CHROMATYPE = HwJP_CHROMA_422S;	// YUV422S
	else if(max_hsamp==4 && max_vsamp==1 && nblk==6)   {
		CHROMATYPE = 5;  // YUV420S
		return JPEG_CHROMA_NOT_SUPPORT;  //This is Supported by SW Only !!!
	}
	else {
		return JPEG_CHROMA_NOT_SUPPORT;  //This is Not Supported !!!
	}

	BITCSET(HwJPEGDEC->JP_CHROMA, 0xFFFFFFFF, CHROMATYPE);
	return JPEG_NO_ERR;
}

/****************************************************************************
	Parser_Get_QTableID()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int Parser_Get_QTableID(unsigned char *Jpeg_Header)
{
	uint16 QTableID;
	uint8 NF;
	uint8 QT[3];
	int i;

	Jpeg_Header+=7; //FFC0 : Size(2) + P(1) + Y(2) +X(2)

	NF=*Jpeg_Header++; // LS Max=3 // Number of image components in frame

	Jpeg_Header+=2; // Tq1

	for (i=0; i<NF; i++)
	{
		QT[i]=*Jpeg_Header++; // Tq
		switch(QT[i])
		{
			case 0x00:
				QT[i]=0x0;
				break;
			case 0x01:
				QT[i]=0x1;
				break;
			case 0x10:
				QT[i]=0x2;
				break;
			case 0x11:
				QT[i]=0x3;
				break;
		}
		Jpeg_Header+=2; //Cnf(1) + HV(1)
	}
	QTableID = ((uint16)QT[0]<<4 ) | ((uint16)QT[1]<<2 ) | ((uint16)QT[2]);

	return QTableID;
}

/****************************************************************************
	Parser_Reg_Table_ID_Set()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int Parser_Reg_Table_ID_Set(unsigned char *Jpeg_Header, unsigned short QTableID)
{
	int  NCSCAN;
	int  NBLK; 
	int  CIDSCAN[4];
	unsigned char in_code;
	int  i,j,k=0,l;
	int  hsamp_max, vsamp_max;
	int  qtabid, htabid;

	Jpeg_Header+=2;  //Skip header length description.

	NCSCAN = *Jpeg_Header++; // LS Max=3 // Number of image components in scan

	if(NCSCAN != NCFRAM)
	return JPEG_SOS_COMPONENT_ERR; //# of Comp in SCAN is Not Equal to FRAME

	hsamp_max = 0;
	vsamp_max = 0;
	NBLK = 0;

	qtabid = 0;
	htabid = 0;

	for (i=0; i<NCSCAN; i++)
	{
		CIDSCAN[i] = *Jpeg_Header++;
		for (j=0; j<NCFRAM; j++) if (CIDSCAN[i]==COMP[j].ID)	k=j;
		in_code = *Jpeg_Header++;
		COMP[k].DCHUFFID = (in_code>>4)&0xf;
		COMP[k].ACHUFFID = in_code&0xf;
		if (COMP[k].HSAMP > hsamp_max) hsamp_max = COMP[k].HSAMP;  // ?
		if (COMP[k].VSAMP > vsamp_max) vsamp_max = COMP[k].VSAMP;  // ?

		if (NCSCAN==1)
		{
			SCOMP[NBLK].ID=CIDSCAN[i];
			SCOMP[NBLK].YH_POS=0;
			SCOMP[NBLK].YV_POS=0;
			SCOMP[NBLK].QID=COMP[k].QID;
			SCOMP[NBLK].DCHUFFID=COMP[k].DCHUFFID;
			SCOMP[NBLK].ACHUFFID=COMP[k].ACHUFFID;
			NBLK++;
		}
		else
		{
			for(j=0; j<COMP[k].VSAMP; j++)
			{
				for(l=0; l<COMP[k].HSAMP; l++)
				{
					SCOMP[NBLK].ID=CIDSCAN[i];
					SCOMP[NBLK].YV_POS=j;
					SCOMP[NBLK].YH_POS=l;
					SCOMP[NBLK].QID=COMP[k].QID;
					SCOMP[NBLK].DCHUFFID=COMP[k].DCHUFFID;
					SCOMP[NBLK].ACHUFFID=COMP[k].ACHUFFID;
					NBLK++;
				}
			}
		}

		qtabid |= COMP[k].QID<<(4-k*2);
		htabid |= (COMP[k].DCHUFFID*2+COMP[k].ACHUFFID)<<(4-k*2);
	}

	BITCSET(HwJPEGDEC->JPD_TBL_ID, 0xFFFFFFFF, (qtabid<<16)|htabid);

	return JPEG_NO_ERR;

}
	


/****************************************************************************
	Parser_Reg_ImgSize_Set()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
void Parser_Reg_ImgSize_Set(unsigned char chroma, unsigned int ImgWidth, unsigned int ImgHeight)
{
	uint32 usJpegWidth=0, usJpegHeight=0;

	switch(chroma)
	{
		case YUV420:
		case YUV422:
			usJpegWidth = ImgWidth/16;
			break;
		case YONLY:
		case YUV444:
		case YUV422S:
			usJpegWidth = ImgWidth/8;
			break;
	}

	switch(chroma)
	{
		case YUV420:
		case YUV422S:
			usJpegHeight = ImgHeight/16;
			break;
		case YONLY:
		case YUV422:
		case YUV444:
			usJpegHeight = ImgHeight/8;
			break;
	}
	BITCSET(HwJPEGDEC->JP_SIZE, 0xFFFFFFFF, ((usJpegWidth<<16)|usJpegHeight));
}

/****************************************************************************
	Parser_Reg_QValue_Set()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int Parser_Reg_QValue_Set(unsigned char *Jpeg_Header)
{
	uint32 header_length;
	int i,j,k;
	uint8 in_code;
	int Pq, Tq;
	uint8 iqmat[3][64];

	header_length = *Jpeg_Header++<<8;			   //Lq is 16bits len
	header_length = header_length + *Jpeg_Header++;
	k=(header_length-2)/65;   //quantize para = 64, Pq + Tq = 1(byte)

	for (i=0; i<k; i++)
	{
		in_code = *Jpeg_Header++;
		Pq = (in_code>>4)&0xF;

		if(Pq != 0) //"Quant table element precision is not 8"
			return JPEG_QTABLE_PRECISION_ERR;

		Tq = in_code&0xF;

		for (j=0; j<64; j++)
		{
			iqmat[Tq][j] = *Jpeg_Header++; // zigzag order
			regw(JPEG_DECODE_BASE+(0x300+(Tq*0x100)+(j*4)), iqmat[Tq][j]);
		}
	}
	return JPEG_NO_ERR;
}

/****************************************************************************
	Parser_Reg_HuffValue_Set()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
int Parser_Reg_HuffValue_Set(unsigned char *Jpeg_Header)
{
	unsigned int header_length;
	int i=0, j, k;
	unsigned char in_code;
	int Tc, Th;
	int min, min_addr;
	unsigned char huff_cnt[2][2][16];
	unsigned char huff_min_addr[2][2][16];
	unsigned char huff_code_min[2][2][16];
	unsigned short huff_code_max[2][2][16];
	unsigned char huff_code_val[2][2][16];

	unsigned char *uiCurrentPosition;
	unsigned char *uiEndPosition;
	uint32 uiNextMarker;

	header_length = *Jpeg_Header++<<8;			   //Lq is 16bits len
	header_length = header_length + *Jpeg_Header++;

	uiCurrentPosition = Jpeg_Header;
	uiEndPosition = (uint8 *)((unsigned int)uiCurrentPosition + (header_length-2));
	uiNextMarker = (((uint32)*uiEndPosition & 0x00FF)<<8) |((uint32)*(uiEndPosition+1) & 0x00FF);

/*
	if (!(uiNextMarker==SOS || uiNextMarker==DQT || uiNextMarker==DHT || uiNextMarker==DNL || uiNextMarker==DRI ||
		  uiNextMarker==DHP || uiNextMarker==EXP || uiNextMarker==DAC || uiNextMarker==SOF))
	{
		return JPEG_DHT_MARKER_BROKEN;
	}
*/

	while (i < header_length-2)
	{
		in_code = *Jpeg_Header++;
		Tc = (in_code>>4)&0xF;
		Th = in_code&0xF;
		i++;
		min=0;
		min_addr=0;

		for (j=0; j<16; j++)
		{
			huff_cnt[Th][Tc][j] = *Jpeg_Header++;
			i++;
			huff_min_addr[Th][Tc][j] = min_addr;
			regw(JPEG_DECODE_BASE+(0x700+Th*0x80+Tc*0x40+j*4), huff_min_addr[Th][Tc][j]);

			if(Th==0) {
				regw(JPEG_DECODE_BASE+0x700+0x80+Tc*0x40+j*4, huff_min_addr[Th][Tc][j]);
			}

			min_addr = min_addr+huff_cnt[Th][Tc][j];
			huff_code_min[Th][Tc][j] = min;
			huff_code_max[Th][Tc][j] = min+huff_cnt[Th][Tc][j];
			regw(JPEG_DECODE_BASE+(0x600+Th*0x80+Tc*0x40+j*4), huff_code_max[Th][Tc][j]);

			if(Th==0) {
				regw(JPEG_DECODE_BASE+0x600+0x80+Tc*0x40+j*4, huff_code_max[Th][Tc][j]);
			}
			min = huff_code_max[Th][Tc][j]*2;
		}

		for (j=0; j<16; j++)  for (k=0; k<huff_cnt[Th][Tc][j]; k++)
		{
			huff_code_val[Th][Tc][k+huff_min_addr[Th][Tc][j]] = *Jpeg_Header++;
			regw(JPEG_DECODE_BASE+(0x800+Th*0x400+Tc*0x40+(huff_min_addr[Th][Tc][j]+k)*4), huff_code_val[Th][Tc][huff_min_addr[Th][Tc][j]+k]);

			if(Th==0) {
				regw(JPEG_DECODE_BASE+0x800+0x400+Tc*0x40+(huff_min_addr[Th][Tc][j]+k)*4, huff_code_val[Th][Tc][huff_min_addr[Th][Tc][j]+k]);
			}
			i++;
		}
	}
	return JPEG_NO_ERR;
}

/****************************************************************************
	ImagePaddingForDecode()

	Description :
	Parameter : NONE
	Return : NONE

****************************************************************************/
JPEG_PARSER_STATUS ImagePaddingForDecode(unsigned int Chroma, unsigned int ImgWidth, unsigned int ImgHeight,
								unsigned int *NeedsPadding, unsigned int *pad_width, unsigned int *pad_height)
{
	*NeedsPadding = FALSE;

	if (Chroma == YUV420)
	{
		if ((ImgWidth%16 != 0) || (ImgHeight%16 != 0))
			*NeedsPadding = TRUE;
		while (ImgWidth%16 != 0)	ImgWidth++;
		while (ImgHeight%16 != 0)	ImgHeight++;
	}
	else if (Chroma == YUV422)
	{
		if ((ImgWidth%16 != 0) || (ImgHeight%8 != 0))
			*NeedsPadding = TRUE;
		while (ImgWidth%16 != 0)	ImgWidth++;
		while (ImgHeight%8 != 0)	ImgHeight++;
	}
	else if ((Chroma == YUV444) || (Chroma == YONLY))
	{
		if ((ImgWidth%8 != 0) || (ImgHeight%8 != 0))
			*NeedsPadding = TRUE;
		while (ImgWidth%8 != 0) 	ImgWidth++;
		while (ImgHeight%8 != 0)	ImgHeight++;
	}
	else if (Chroma == YUV422S)
	{
		if ((ImgWidth%8 != 0) || (ImgHeight%16 != 0))
			*NeedsPadding = TRUE;
		while (ImgWidth%8 != 0) 	ImgWidth++;
		while (ImgHeight%16 != 0)	ImgHeight++;
	}
	else
	{
		return JPEG_CHROMA_NOT_SUPPORT;
	}

	*pad_width = ImgWidth;
	*pad_height = ImgHeight;

	return JPEG_NO_ERR;

}

/****************************************************************************
	JPEG_IMAGE_Parser()

	Description :
	Parameter : NONE
	Return : NONE
****************************************************************************/
JPEG_PARSER_STATUS JPEG_IMAGE_Parser(unsigned int file_length, unsigned int *BitStreamBuff,
													unsigned char IsEhiInterface, jpeg_parser_rsp *JpegImageInfo)
{
	int Read_Size=0, Offset_Size=0;
	unsigned char *point;
	int Marker;
	int BitstreamSize, Header_size;
	int jumppointval=0;
	int checkJFXX=0;
	unsigned short TableIDQ=0;
	int restartinterval=0;
	int check_RES=0; // JPEG Reserved Maeker 
	JPEG_PARSER_STATUS ErrFlag;

	unsigned int NeedsPadding;
	unsigned int pad_width, pad_height;

	unsigned int DHTCHECK = 0;
	
	SOICHK = 0;
	SOFCHK = 0;
	SOSCHK = 0;
	EOICHK = 0;

	point = (uint8*)BitStreamBuff;

	if (file_length > 0)
	{
		while(Read_Size < file_length)
		{
			Marker = ReadMarkerInfo(point);
			point+=2;;
			Read_Size+=2;
			switch(Marker)
			{
				case SOI:
					SOICHK = 1;
					break;

				case APP0:
				case APP1:
				case APP2:
				case APP3:
				case APP4:
				case APP5:
				case APP6:
				case APP7:
				case APP8:
				case APP9:
				case APPA:
				case APPB:
				case APPC:
				case APPD:
				case APPE:
				case APPF:
					Offset_Size = ReadSizeOffset(point);
					checkJFXX = CheckJXFF(point);
					if(checkJFXX)
					{
						Read_Size+=Offset_Size;
						point+=Offset_Size;
                                                #if (0) //20101008 ysseung   modify to jpeg decode.
						if(ReadMarkerInfo(point)!=DQT) //FFD9 다음에 바로 FFDB 이 없을경우
						{
							jumppointval = JumpPointToDQT(point, Read_Size, file_length);
							point+=jumppointval;
						}
                                                #endif
					}
					else
					{
						Read_Size+=Offset_Size;
						point+=Offset_Size;
					}
					break;

				case COM:
					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;

				case JPG0:
				case JPG1:
				case JPG2:
				case JPG3:
				case JPG4:
				case JPG5:
				case JPG6:
				case JPG7:
				case JPG8:
				case JPG9:
				case JPG10:
				case JPG11:
				case JPG12:
				case JPG13:
					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;

				case DQT:
					ErrFlag = Parser_Reg_QValue_Set(point); // Q table Info. Register Setting

					if (ErrFlag != JPEG_NO_ERR)
						return ErrFlag;

					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;

				case SOF:
					if (!SOICHK)
						return JPEG_WITHOUT_SOI_MARKER;

					ErrFlag = Parser_Reg_Chroma_Set(point);

					if (ErrFlag != JPEG_NO_ERR)
						return ErrFlag;

					JpegImageInfo->ImgChromaType = CHROMATYPE;

					TableIDQ = Parser_Get_QTableID(point);
					Offset_Size = ReadSizeOffset(point);
					JpegImageInfo->ImageHeight = ReadImageHeight(point);
					JpegImageInfo->ImageWidth = ReadImageWidth(point);

					ErrFlag = ImagePaddingForDecode(JpegImageInfo->ImgChromaType, JpegImageInfo->ImageWidth,
												JpegImageInfo->ImageHeight, &NeedsPadding, &pad_width, &pad_height);

					if (ErrFlag != JPEG_NO_ERR)
						return ErrFlag;

					JpegImageInfo->IsNeedsPadding = NeedsPadding;
					JpegImageInfo->PaddingWidth = pad_width;
					JpegImageInfo->PaddingHeight = pad_height;

					Read_Size+=Offset_Size;
					point+=Offset_Size;
					Parser_Reg_ImgSize_Set(JpegImageInfo->ImgChromaType, JpegImageInfo->PaddingWidth, JpegImageInfo->PaddingHeight);
					SOFCHK = 1;
					break;

				case SOF1:
					return JPEG_FORMAT_EXT_SEQUENTIAL;

				case SOF2:
					return JPEG_FORMAT_PROGRESSIVE;

				case SOF3:
					return JPEG_FORMAT_LOSSLESS;

				case SOF5:
				case SOF6:
				case SOF7:
					return JPEG_HW_NOT_SUPPORT; 

				case JPG: //FFC8
					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;					

				case SOF9:
				case SOF10:
				case SOF11:
				case SOF13:
				case SOF14:
				case SOF15:
					return JPEG_HW_NOT_SUPPORT;

				case RST0:
				case RST1:
				case RST2:
				case RST3:
				case RST4:
				case RST5:
				case RST6:
				case RST7:
					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;		

				case DHT:
					ErrFlag = Parser_Reg_HuffValue_Set(point);
					DHTCHECK = 1;
					
					if (ErrFlag != JPEG_NO_ERR)
						return ErrFlag;

					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;

				case DAC:
					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;

				case SOS:
					if (!SOFCHK)
						return JPEG_WITHOUT_SOF_MARKER;

					ErrFlag = Parser_Reg_Table_ID_Set(point, TableIDQ);

					if (ErrFlag != JPEG_NO_ERR)
						return ErrFlag;

					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					SOSCHK = 1;
					break;

				case DNL:
					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;

				case DHP:
				case EXP:
				case TEM:
					return JPEG_HW_NOT_SUPPORT;

				case DRI:
					restartinterval=1;
					ErrFlag = Parser_Reg_Restart_Set(point, restartinterval);
					Offset_Size = ReadSizeOffset(point);
					Read_Size+=Offset_Size;
					point+=Offset_Size;
					break;

				default: //Original Image Bitstream Data
					if (!SOICHK)  return JPEG_WITHOUT_SOI_MARKER;
					if (!SOFCHK)  return JPEG_WITHOUT_SOF_MARKER;
					if (!SOSCHK)  return JPEG_WITHOUT_SOS_MARKER;

					check_RES = CheckRESMarker(Marker); // Reserved Markers ===> Skip
					if (check_RES == 1) 
					{	
						Offset_Size = ReadSizeOffset(point);
						Read_Size+=Offset_Size;
						point+=Offset_Size;
						break;
					}
					Header_size = (Read_Size+jumppointval-2);
					BitstreamSize = file_length-Header_size;
					JpegImageInfo->BitStreamSize = BitstreamSize;
					JpegImageInfo->HeaderSize = Header_size;

					if(restartinterval != 1)
						Parser_Reg_Restart_Set(point, restartinterval); //DRI 없는 경우.

					if(!DHTCHECK) //ZzaU::huffman table for mjpeg
					{
						ErrFlag = Parser_Reg_HuffValue_Set(dht_data+2);
						DHTCHECK = 1;
					}
					return JPEG_PARSER_SUCCESS;
			}
		}
		return JPEG_PARSER_UNKNOWN_ERR;
	}
	return JPEG_FILE_LENGTH_ERR;
}
#endif

