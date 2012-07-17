/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : Vioc_wdma.h
*
*  Description : Write DMA controller components
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/23     0.1            created                      hskim
*******************************************************************************/
#ifndef __VIOC_WDMA_H__
#define	__VIOC_WDMA_H__
#include <mach/reg_physical.h>

#define VIOC_WDMA_IREQ_UPD_MASK 		0x00000001UL
#define VIOC_WDMA_IREQ_SREQ_MASK 		0x00000002UL
#define VIOC_WDMA_IREQ_ROLL_MASK 		0x00000004UL
#define VIOC_WDMA_IREQ_ENR_MASK 		0x00000008UL
#define VIOC_WDMA_IREQ_ENF_MASK 		0x00000010UL
#define VIOC_WDMA_IREQ_EOFR_MASK 		0x00000020UL
#define VIOC_WDMA_IREQ_EOFF_MASK 		0x00000040UL
#define VIOC_WDMA_IREQ_SEOFR_MASK 		0x00000080UL
#define VIOC_WDMA_IREQ_SEOFF_MASK 		0x00000100UL
#define VIOC_WDMA_IREQ_STSEN_MASK 		0x20000000UL
#define VIOC_WDMA_IREQ_STBF_MASK 		0x40000000UL
#define VIOC_WDMA_IREQ_STEOF_MASK 		0x80000000UL
#define VIOC_WDMA_IREQ_ALL_MASK 		0xFFFFFFFFUL

typedef struct
{
	unsigned int ImgSizeWidth;
	unsigned int ImgSizeHeight;
	unsigned int ImgFormat;
	unsigned int BaseAddress;
	unsigned int BaseAddress1;
	unsigned int BaseAddress2;
	unsigned int Interlaced;
	unsigned int ContinuousMode;
	unsigned int SyncMode;
	unsigned int AlphaValue;
	unsigned int Hue;
	unsigned int Bright;
	unsigned int Contrast;
} VIOC_WDMA_IMAGE_INFO_Type;


/* Interface APIs. */
extern void VIOC_WDMA_SetDefaultImageConfig(VIOC_WDMA * pWDMA, VIOC_WDMA_IMAGE_INFO_Type * ImageCfg, unsigned int r2yEn);
extern void VIOC_WDMA_SetImageEnable(VIOC_WDMA * pWDMA, unsigned int nContinuous);
extern void VIOC_WDMA_SetImageUpdate(VIOC_WDMA * pWDMA);
extern void VIOC_WDMA_SetImageFormat(VIOC_WDMA *pWDMA, unsigned int nFormat);
extern void VIOC_WDMA_SetImageInterlaced(VIOC_WDMA *pWDMA, unsigned int intl);
extern void VIOC_WDMA_SetImageR2YMode(VIOC_WDMA *pWDMA, unsigned int r2y_mode);
extern void VIOC_WDMA_SetImageR2YEnable(VIOC_WDMA *pWDMA, unsigned int enable);
extern void VIOC_WDMA_SetImageRateControl(VIOC_WDMA *pWDMA, unsigned int enable, unsigned int rate);
extern void VIOC_WDMA_SetImageSyncControl(VIOC_WDMA *pWDMA, unsigned int enable, unsigned int RDMA);
extern void VIOC_WDMA_SetImageDither(VIOC_WDMA *pWDMA, unsigned int nDithEn, unsigned int nDithSel, unsigned char mat[4][4]);
extern void VIOC_WDMA_SetImageEnhancer(VIOC_WDMA *pWDMA, unsigned int nContrast, unsigned int nBright, unsigned int nHue);
extern void VIOC_WDMA_SetImageSize(VIOC_WDMA *pWDMA, unsigned int sw, unsigned int sh);
extern void VIOC_WDMA_SetImageBase(VIOC_WDMA *pWDMA, unsigned int nBase0, unsigned int nBase1, unsigned int nBase2);
extern void VIOC_WDMA_SetImageOffset(VIOC_WDMA *pWDMA, unsigned int imgFmt, unsigned int imgWidth);
extern void VIOC_WDMA_SetIreqMask(VIOC_WDMA * pWDMA, unsigned int mask, unsigned int set);
extern void VIOC_WDMA_IreqHandler(unsigned int vectorID);
extern void VIOC_WDMA_SWReset(PVIOC_IREQ_CONFIG pIrgConfig, unsigned int WDMA);

#endif
