/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : vioc_disp.c
*
*  Description : display control component control module
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/15     0.1            created                      hskim
*******************************************************************************/
#include <mach/vioc_disp.h>
#include <mach/globals.h>
#include <mach/io.h>

VIOC_TIMING_INFO gVIOC_DefaultTimeSet[VIOC_DEFAUKLT_TIME_MAX] = 
{
	{VIOC_HDMI_1920X1080P_60Hz, 	0, 0, 0, 0, 0, 1, 0, 43, 1919, 147, 87, 4, 1079, 35, 3, 4, 1079, 35, 3},
	{VIOC_HDMI_1920X1080P_59Hz, 	0, 0, 0, 0, 0, 1, 0, 43, 1919, 147, 87, 4, 1079, 35, 3, 4, 1079, 35, 3},
	{VIOC_HDMI_1920X1080P_50Hz, 	0, 0, 0, 0, 0, 1, 0, 43, 1919, 163, 511, 4, 1079, 35, 3, 4, 1079, 35, 3},
	{VIOC_HDMI_1920X1080I_60Hz, 		0, 0, 0, 0, 0, 0, 1, 43, 1919, 147, 87, 9, 1079, 29, 3, 9, 1079, 31, 3},
	{VIOC_HDMI_1920X1080I_59Hz, 		0, 0, 0, 0, 0, 0, 1, 43, 1919, 147, 87, 9, 1079, 29, 3, 9, 1079, 31, 3},
	{VIOC_HDMI_1920X1080I_50Hz, 		0, 0, 0, 0, 0, 0, 1, 43, 1919, 163, 511, 9, 1079, 29, 3, 9, 1079, 31, 3},
	{VIOC_HDMI_1280X720P_60Hz, 		0, 0, 0, 0, 0, 1, 0, 39, 1279, 219, 109, 4, 719, 19, 4, 4, 719, 19, 4},
	{VIOC_HDMI_1280X720P_59Hz, 		0, 0, 0, 0, 0, 1, 0, 39, 1279, 219, 109, 4, 719, 19, 4, 4, 719, 19, 4},
	{VIOC_HDMI_1280X720P_50Hz, 		0, 0, 0, 0, 0, 1, 0, 39, 1279, 219, 439, 4, 719, 19, 4, 4, 719, 19, 4},
	{VIOC_HDMI_720X480P_60Hz, 		0, 1, 1, 0, 0, 1, 0, 61, 719, 59, 15, 5, 479, 29, 8, 5, 479, 29, 8},
	{VIOC_HDMI_720X480P_59Hz, 		0, 1, 1, 0, 0, 1, 0, 61, 719, 59, 15, 5, 479, 29, 8, 5, 479, 29, 8},
	{VIOC_HDMI_720X576P_50Hz, 		0, 1, 1, 0, 0, 1, 0, 63, 719, 67, 11, 5, 575, 38, 4, 4, 575, 38, 4},
	{VIOC_HDMI_720X480I_60Hz, 		0, 1, 1, 0, 1, 0, 1, 123, 1439, 113, 37, 5, 479, 29, 7, 5, 479, 31, 7},
	{VIOC_HDMI_720X480I_59Hz, 		0, 1, 1, 0, 1, 0, 1, 123, 1439, 113, 37, 5, 479, 29, 7, 5, 479, 31, 7},
	{VIOC_HDMI_720X576I_50Hz, 		0, 1, 1, 0, 1, 0, 1, 125, 1439, 137, 23, 5, 575, 37, 3, 5, 575, 39, 3},
	{VIOC_HDVE_720X480I_60Hz, 		1, 1, 1, 0, 0, 0, 1, 129, 719, 3, 3, 5, 479, 25, 2, 5, 479, 36, 1},
	{VIOC_HDVE_720X576I_50Hz, 		1, 1, 1, 0, 0, 0, 1, 135, 719, 3, 3, 5, 575, 39, 2, 5, 575, 40, 1},
	{VIOC_HDVE_720X480P_60Hz, 	1, 1, 1, 0, 0, 1, 1, 129, 719, 3, 3, 5, 479, 37, 0, 5, 479, 37, 0},
	{VIOC_HDVE_720X576P_50Hz, 		1, 1, 1, 0, 0, 1, 1, 135, 719, 3, 3, 5, 575, 41, 0, 5, 575, 41, 0},
	{VIOC_HDVE_1280X720P_60Hz, 		1, 1, 1, 0, 0, 1, 0, 361, 1279, 3, 3, 5, 719, 22, 0, 5, 719, 22, 5},
	{VIOC_HDVE_1280X720P_50Hz, 		1, 1, 1, 0, 0, 1, 0, 491, 1279, 203, 3, 5, 719, 22, 0, 5, 719, 22, 5},
	{VIOC_HDVE_1920X1080I_60Hz, 	1, 1, 1, 0, 0, 0, 1, 271, 1919, 3, 3, 5, 1079, 35, 2, 5, 1079, 36, 1},
	{VIOC_HDVE_1920X1080I_50Hz, 	1, 1, 1, 0, 0, 0, 1, 411, 1919, 303, 3, 5, 1079, 35, 2, 5, 1079, 36, 1},
	{VIOC_HDVE_1920X1080P_30Hz, 	1, 1, 1, 0, 0, 1, 0, 271, 1919, 3, 3, 5, 1079, 37, 0, 5, 1079, 37, 0},
	{VIOC_HDVE_SMPTE295I_50Hz, 		1, 1, 1, 0, 0, 0, 1, 411, 1919, 303, 3, 5, 1079, 161, 2, 5, 1079, 160, 1},
	{VIOC_HDVE_SMPTE295P_30Hz, 		1, 1, 1, 0, 0, 1, 0, 271, 1919, 3, 3, 5, 1079, 162, 0, 5, 1079, 162, 0},
	{VIOC_SDVE_NTSC_CVBS, 			0, 1, 1, 1, 0, 0, 1, 211, 1439, 31, 31, 0, 479, 36, 6, 0, 479, 37, 5},
	{VIOC_SDVE_PAL_CVBS, 			0, 1, 1, 1, 0, 0, 1, 127, 1439, 137, 21, 0, 575, 42, 4, 0, 575, 43, 3},
	{VIOC_SDVE_NTSC_SVIDEO, 		0, 1, 1, 1, 0, 0, 1, 211, 1439, 31, 31, 0, 479, 36, 6, 0, 479, 37, 5},
	{VIOC_SDVE_PAL_SVIDEO, 		0, 1, 1, 1, 0, 0, 1, 127, 1439, 137, 21, 0, 575, 42, 4, 0, 575, 43, 3},
	{VIOC_SDVE_NTSC_YPbPr, 			0, 1, 1, 1, 0, 0, 1, 211, 1439, 31, 31, 0, 479, 36, 6, 0, 479, 37, 5},
	{VIOC_SDVE_PAL_YPbPr, 			0, 1, 1, 1, 0, 0, 1, 127, 1439, 137, 21, 0, 575, 42, 4, 0, 575, 43, 3},
	{VIOC_SDVE_NTSC_RGB, 			0, 1, 1, 1, 0, 0, 1, 211, 1439, 31, 31, 0, 479, 36, 6, 0, 479, 37, 5},
	{VIOC_SDVE_PAL_RGB, 			0, 1, 1, 1, 0, 0, 1, 127, 1439, 137, 21, 0, 575, 42, 4, 0, 575, 43, 3}
};

void VIOC_DISP_SetDefaultTimingParam	(VIOC_DISP *pDISP, unsigned int  nType)
{
	pDISP->uCLKDIV.bREG.PXCLKDIV	= gVIOC_DefaultTimeSet[nType].CLKDIV;
	pDISP->uCTRL.bREG.IV		= gVIOC_DefaultTimeSet[nType].IV;
	pDISP->uCTRL.bREG.IH		= gVIOC_DefaultTimeSet[nType].IH;
	pDISP->uCTRL.bREG.IP		= gVIOC_DefaultTimeSet[nType].IP;
	pDISP->uCTRL.bREG.DP		= gVIOC_DefaultTimeSet[nType].DP;
	pDISP->uCTRL.bREG.NI		= gVIOC_DefaultTimeSet[nType].NI;
	pDISP->uCTRL.bREG.TV		= gVIOC_DefaultTimeSet[nType].TV;
	pDISP->uLHTIME1.bREG.LPC		= gVIOC_DefaultTimeSet[nType].LPC;
	pDISP->uLHTIME1.bREG.LPW		= gVIOC_DefaultTimeSet[nType].LPW;
	pDISP->uLHTIME2.bREG.LEWC	= gVIOC_DefaultTimeSet[nType].LEWC;
	pDISP->uLHTIME2.bREG.LSWC	= gVIOC_DefaultTimeSet[nType].LSWC;
	pDISP->uLVTIME1.bREG.FLC		= gVIOC_DefaultTimeSet[nType].FLC;
	pDISP->uLVTIME1.bREG.FPW		= gVIOC_DefaultTimeSet[nType].FPW;
	pDISP->uLVTIME2.bREG.FEWC	= gVIOC_DefaultTimeSet[nType].FEWC;
	pDISP->uLVTIME2.bREG.FSWC	= gVIOC_DefaultTimeSet[nType].FSWC;
	pDISP->uLVTIME3.bREG.FLC		= gVIOC_DefaultTimeSet[nType].FLC2;
	pDISP->uLVTIME3.bREG.FPW		= gVIOC_DefaultTimeSet[nType].FPW2;
	pDISP->uLVTIME4.bREG.FEWC	= gVIOC_DefaultTimeSet[nType].FEWC2;
	pDISP->uLVTIME4.bREG.FSWC	= gVIOC_DefaultTimeSet[nType].FSWC2;
}



void VIOC_DISP_SetSize(VIOC_DISP *pDISP, unsigned int nWidth, unsigned int nHeight)
{
	//pDISP->uLSIZE.bREG.HSIZE = nWidth;
	//pDISP->uLSIZE.bREG.VSIZE = nHeight;

	BITCSET(pDISP->uLSIZE.nREG, 0xFFFFFFFF, (nHeight << 16) | (nWidth) );	
}


void VIOC_DISP_GetSize(VIOC_DISP *pDISP, unsigned int *nWidth, unsigned int *nHeight)
{
	*nWidth = pDISP->uLSIZE.bREG.HSIZE;
	*nHeight = pDISP->uLSIZE.bREG.VSIZE;
}

// BG0 : Red, 	BG1 : Green , 	BG2, Blue
void VIOC_DISP_SetBGColor(VIOC_DISP *pDISP, unsigned int BG0, unsigned int BG1, unsigned int BG2)
{
	/*
	pDISP->uBG.bREG.BG0 = BG0;
	pDISP->uBG.bREG.BG1 = BG1;
	pDISP->uBG.bREG.BG2 = BG2;
	pDISP->uBG.bREG.BG3 = 1;
	*/

	BITCSET(pDISP->uBG.nREG,0xFFFFFFFF, 1 << 24 | BG2 << 16 | BG1 << 8 | BG0);
}

void VIOC_DISP_SetPosition(VIOC_DISP *pDISP, unsigned int startX, unsigned int startY )
{
	//pDISP->uLPOS.bREG.XPOS = startX;
	//pDISP->uLPOS.bREG.YPOS = startY;

	BITCSET(pDISP->uLPOS.nREG, 0x1FFF1FFF, startY << 16 | startX);
}

void VIOC_DISP_GetPosition(VIOC_DISP *pDISP, unsigned int *startX, unsigned int *startY )
{
	*startX = pDISP->uLPOS.bREG.XPOS;
	*startY = pDISP->uLPOS.bREG.YPOS;
}

void VIOC_DISP_SetColorEnhancement(VIOC_DISP *pDISP, signed char  contrast, signed char  brightness, signed char  hue )
{
	/*
	pDISP->uLENH.bREG.CONRAST = contrast;
	pDISP->uLENH.bREG.BRIGHTNESS = brightness;
	pDISP->uLENH.bREG.HUE = hue;
	*/
	
	BITCSET(pDISP->uLENH.nREG, 0x00FFFFFF, hue << 24 | brightness << 16 || contrast );
}

void VIOC_DISP_GetColorEnhancement(VIOC_DISP *pDISP, signed char  *contrast, signed char  *brightness, signed char *hue )
{
	/*
	*contrast = pDISP->uLENH.bREG.CONRAST;
	*brightness = pDISP->uLENH.bREG.BRIGHTNESS;
	*hue = pDISP->uLENH.bREG.HUE;
	*/
	*contrast = pDISP->uLENH.nREG & 0xFF;
	*brightness = pDISP->uLENH.nREG & 0xFF00; 
	*hue = pDISP->uLENH.nREG & 0xFF0000; 
}

void VIOC_DISP_SetClippingEnable(VIOC_DISP *pDISP, unsigned int enable)
{
	//pDISP->uCTRL.bREG.CLEN = enable;
	BITCSET(pDISP->uCTRL.nREG, 1<11,enable<<11);
}

void VIOC_DISP_GetClippingEnable(VIOC_DISP *pDISP, unsigned int *enable)
{
	//*enable = pDISP->uCTRL.bREG.CLEN;
	*enable = pDISP->uCTRL.nREG & (1<<11);
}

void VIOC_DISP_SetClipping(VIOC_DISP *pDISP, unsigned int uiUpperLimitY, unsigned int uiLowerLimitY, unsigned int uiUpperLimitUV, unsigned int uiLowerLimitUV)
{
	/*
	pDISP->uCLIPY.bREG.CLPH = uiUpperLimitY;
	pDISP->uCLIPY.bREG.CLPL = uiLowerLimitY;
	pDISP->uCLIPC.bREG.CLPH = uiUpperLimitUV;
	pDISP->uCLIPC.bREG.CLPL = uiLowerLimitUV;
	*/

	BITCSET(pDISP->uCLIPY.nREG, 0x00FF00FF, uiUpperLimitY << 16 | uiLowerLimitY );
	BITCSET(pDISP->uCLIPC.nREG, 0x00FF00FF, uiUpperLimitUV<< 16 | uiLowerLimitUV);	
	
}

void VIOC_DISP_GetClipping(VIOC_DISP *pDISP, unsigned int *uiUpperLimitY, unsigned int *uiLowerLimitY, unsigned int *uiUpperLimitUV, unsigned int *uiLowerLimitUV)
{
	/*
	*uiUpperLimitY = pDISP->uCLIPY.bREG.CLPH;
	*uiLowerLimitY = pDISP->uCLIPY.bREG.CLPL;
	*uiUpperLimitUV = pDISP->uCLIPC.bREG.CLPH;
	*uiLowerLimitUV = pDISP->uCLIPC.bREG.CLPL;
	*/

	*uiUpperLimitY = pDISP->uCLIPY.nREG & 0x00FF0000;
	*uiLowerLimitY = pDISP->uCLIPY.nREG & 0x000000FF;
	*uiUpperLimitUV = pDISP->uCLIPC.nREG & 0x00FF0000;
	*uiLowerLimitUV = pDISP->uCLIPC.nREG & 0x000000FF;
	
}

void VIOC_DISP_SetDither(VIOC_DISP *pDISP, unsigned int ditherEn, unsigned int ditherSel, unsigned char mat[4][4])
{
#if 0
	pDISP->uDMAT.bREG.DITH00 = mat[0][0];
	pDISP->uDMAT.bREG.DITH01 = mat[0][1];
	pDISP->uDMAT.bREG.DITH02 = mat[0][2];
	pDISP->uDMAT.bREG.DITH03 = mat[0][3];

	pDISP->uDMAT.bREG.DITH10 = mat[1][0];
	pDISP->uDMAT.bREG.DITH11 = mat[1][1];
	pDISP->uDMAT.bREG.DITH12 = mat[1][2];
	pDISP->uDMAT.bREG.DITH13 = mat[1][3];

	pDISP->uDMAT.bREG.DITH20 = mat[2][0];
	pDISP->uDMAT.bREG.DITH21 = mat[2][1];
	pDISP->uDMAT.bREG.DITH22 = mat[2][2];
	pDISP->uDMAT.bREG.DITH23 = mat[2][3];

	pDISP->uDMAT.bREG.DITH30 = mat[3][0];
	pDISP->uDMAT.bREG.DITH31 = mat[3][1];
	pDISP->uDMAT.bREG.DITH32 = mat[3][2];
	pDISP->uDMAT.bREG.DITH33 = mat[3][3];

	pDISP->uDITHCTRL.bREG.DEN = ditherEn; /* Dither Enable*/
	pDISP->uDITHCTRL.bREG.DSEL = ditherSel; /*Dither Mode 0: LSB Toggle mode, 1: Adder Mode */
#endif
	BITCSET(pDISP->uDMAT.nREG[0], 0x00007777, mat[0][3] << 12 | mat[0][2] << 8 | mat[0][1] << 4 | mat[0][0] );
	BITCSET(pDISP->uDMAT.nREG[0], 0x77770000, mat[1][3] << 28 | mat[1][2] << 24 | mat[1][1] << 18 | mat[1][0] << 16 );

	BITCSET(pDISP->uDMAT.nREG[1], 0x00007777, mat[2][3] << 12 | mat[2][2] << 8 | mat[2][1] << 4 | mat[2][0] );
	BITCSET(pDISP->uDMAT.nREG[1], 0x77770000, mat[3][3] << 28 | mat[3][2] << 24 | mat[3][1] << 18 | mat[3][0] << 16 );

	BITCSET(pDISP->uDITHCTRL.nREG, 0x3 << 30, ditherEn << 31 | ditherSel << 30 );

}

void VIOC_DISP_SetTimingParam (VIOC_DISP *pDISP, stLTIMING *pTimeParam)
{
	/*
	pDISP->uLHTIME1.bREG.LPC = pTimeParam->lpc - 1;
	pDISP->uLHTIME1.bREG.LPW = pTimeParam->lpw;
	pDISP->uLHTIME2.bREG.LEWC = pTimeParam->lewc -1;
	pDISP->uLHTIME2.bREG.LSWC = pTimeParam->lswc -1;
	pDISP->uLVTIME1.bREG.FLC = pTimeParam->flc;
	pDISP->uLVTIME1.bREG.FPW = pTimeParam->fpw;
	pDISP->uLVTIME2.bREG.FEWC = pTimeParam->fewc;
	pDISP->uLVTIME2.bREG.FSWC = pTimeParam->fswc;
	pDISP->uLVTIME3.bREG.FLC = pTimeParam->flc2;
	pDISP->uLVTIME3.bREG.FPW = pTimeParam->fpw2;
	pDISP->uLVTIME4.bREG.FEWC = pTimeParam->fewc2;
	pDISP->uLVTIME4.bREG.FSWC = pTimeParam->fswc2;
	*/

//	Horizon
	BITCSET(pDISP->uLHTIME1.nREG, 0x00003FFF, (pTimeParam->lpc - 1) );
	BITCSET(pDISP->uLHTIME1.nREG, 0x01FF0000, pTimeParam->lpw << 16 );
	BITCSET(pDISP->uLHTIME2.nREG, 0x01FF01FF, ((pTimeParam->lswc-1) << 16) | (pTimeParam->lewc -1));

//	Vertical timing
	BITCSET(pDISP->uLVTIME1.nREG, 0x00003FFF, pTimeParam->flc);
	BITCSET(pDISP->uLVTIME1.nREG, 0x3F << 16 , pTimeParam->fpw << 16);	
	BITCSET(pDISP->uLVTIME2.nREG, 0x01FF01FF,  (pTimeParam->fswc << 16) | pTimeParam->fewc);

	BITCSET(pDISP->uLVTIME3.nREG, 0x00003FFF, pTimeParam->flc2);
	BITCSET(pDISP->uLVTIME3.nREG, 0x3F << 16 , pTimeParam->fpw2 << 16);	
	BITCSET(pDISP->uLVTIME4.nREG, 0x01FF01FF, (pTimeParam->fswc2<< 16) | pTimeParam->fewc2);
	
	
}

void VIOC_DISP_SetControlConfigure(VIOC_DISP *pDISP, stLCDCTR *pCtrlParam)
{
	
#if 0
	/* Default 0x00 */
	pDISP->uCTRL.nREG = 0;
	/*pDISP->uCTRL.bREG.SWAP = pCtrlParam-> */
	/*pDISP->uCTRL.bREG.SRST = pCtrlParam-> */
	pDISP->uCTRL.bREG.R2Y = pCtrlParam->r2y;
	pDISP->uCTRL.bREG.CLEN = pCtrlParam->clen;
	pDISP->uCTRL.bREG.ID = pCtrlParam->id;
	pDISP->uCTRL.bREG.IV = pCtrlParam->iv;
	pDISP->uCTRL.bREG.IH = pCtrlParam->ih;
	pDISP->uCTRL.bREG.IP = pCtrlParam->ip;
	pDISP->uCTRL.bREG.PXDW = pCtrlParam->pxdw;
	pDISP->uCTRL.bREG.SREQ = 1; /* Reset Default */
	pDISP->uCTRL.bREG.CKG = pCtrlParam->ckg;
	pDISP->uCTRL.bREG.C656 = pCtrlParam->ccir656;
	/*pDISP->uCTRL.bREG.ADVI = pCtrlParam-> */
	pDISP->uCTRL.bREG.R2YMD =pCtrlParam->r2ymd;
	pDISP->uCTRL.bREG.EVS = pCtrlParam->evs;
	pDISP->uCTRL.bREG.EVP = pCtrlParam->evp;
	pDISP->uCTRL.bREG.DP = pCtrlParam->dp;
	pDISP->uCTRL.bREG.NI = pCtrlParam->ni;
	pDISP->uCTRL.bREG.TV = pCtrlParam->tv;


#else
	BITCSET(pDISP->uCTRL.nREG,0xFFFFFFFF,
								(pCtrlParam->evp << 31 ) |
								(pCtrlParam->evs << 30 ) |
								(pCtrlParam->r2ymd << 28 ) |
								(pCtrlParam->advi << 26) |
								(pCtrlParam->ccir656 << 24 ) |								
								(pCtrlParam->ckg << 23 ) |
								(1  << 22 /*Reset default*/ ) |							
								(pCtrlParam->id << 15) |
								(pCtrlParam->iv << 14) |
								(pCtrlParam->ih<< 13) |
								(pCtrlParam->ip<< 12) |
								(pCtrlParam->clen << 11 ) |
								(pCtrlParam->r2y << 10) |
								(pCtrlParam->pxdw << 16 ) |
								(pCtrlParam->dp <<9) |
								(pCtrlParam->ni << 8 ) |	
								(pCtrlParam->tv << 7 ) 
			);
#endif
	
}



void VIOC_DISP_TurnOn (VIOC_DISP *pDISP)
{
	//pDISP->uCTRL.bREG.LEN   = 1;
	BITCSET(pDISP->uCTRL.nREG, 0x1, 0x1);
}

void VIOC_DISP_TurnOff(VIOC_DISP *pDISP)
{
	//pDISP->uCTRL.bREG.LEN   = 0;
	BITCSET(pDISP->uCTRL.nREG, 0x1, 0x0);
}

void VIOC_DISP_SetControl(VIOC_DISP *pDISP, stLCDCPARAM *pLcdParam)
{
	/* LCD Controller Stop */
	VIOC_DISP_TurnOff(pDISP);
	/* LCD Controller CTRL Parameter Set */
	VIOC_DISP_SetControlConfigure(pDISP, &pLcdParam->LCDCTRL);
	/* LCD Timing Se */
	VIOC_DISP_SetTimingParam(pDISP, &pLcdParam->LCDCTIMING);
	/* LCD Display Size Set */
	VIOC_DISP_SetSize(pDISP, pLcdParam->LCDCTIMING.lpc, pLcdParam->LCDCTIMING.flc);
	/* LCD Controller Enable */
	VIOC_DISP_TurnOn(pDISP);
}

void VIOC_DISP_SetPort( void )
{

}

void VIOC_DISP_SWReset( unsigned int DISP )
{
	volatile PVIOC_IREQ_CONFIG pIREQConfig;
	pIREQConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<(20+DISP)), (0x1<<(20+DISP))); // disp reset
	BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<(20+DISP)), (0x0<<(20+DISP))); // disp reset
}

void VIOC_DISP_DisplayOnOff( unsigned int onOff )
{
}

/* set 1 : IREQ Masked( interrupt disable), set 0 : IREQ UnMasked( interrput enable)  */
void VIOC_DISP_SetIreqMask(VIOC_DISP * pDISP, unsigned int mask, unsigned int set)
{
	if( set == 0 ) /* Interrupt Enable*/
	{
		//pDISP->uLIRQMASK.nREG &= ~mask;
		BITSCLR(pDISP->uLIRQMASK.nREG, mask,mask);
	}
	else/* Interrupt Diable*/
	{
		//pDISP->uLIRQMASK.nREG |= mask;
		BITCSET(pDISP->uLIRQMASK.nREG, mask,mask);
	}
}

void VIOC_DISP_IreqHandler( unsigned int vectorID )
{
#if 0
	unsigned int 	DISPBase;
	VIOC_DISP *pDISP;
	unsigned int idx;
	
	if ( vectorID > VIOC_IREQ_MAX)
		return;

	idx = vectorID - VIOC_IREQ_DEV0;
	
	DISPBase = ((unsigned int)HwVIOC_DISP0+ idx*0x100);
	pDISP = (VIOC_DISP *)DISPBase;

	if (pDISP->uLSTATUS.nREG & VIOC_DISP_IREQ_FU_MASK && pDISP->uLIRQMASK.bREG.FU == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pDISP->uLSTATUS.bREG.FU = 1;
		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}

	if (pDISP->uLSTATUS.nREG & VIOC_DISP_IREQ_VSR_MASK && pDISP->uLIRQMASK.bREG.VSR == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pDISP->uLSTATUS.bREG.VSR = 1;

		/* DO Action for Interrupt */
		/*TEST_GPIO_DAT ^= 1; */
		/* Interrput Re-Setting */
	}

	if (pDISP->uLSTATUS.nREG & VIOC_DISP_IREQ_VSF_MASK && pDISP->uLIRQMASK.bREG.VSF == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pDISP->uLSTATUS.bREG.VSF = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}

	if (pDISP->uLSTATUS.nREG & VIOC_DISP_IREQ_RU_MASK && pDISP->uLIRQMASK.bREG.RU == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pDISP->uLSTATUS.bREG.RU = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}
	
	if (pDISP->uLSTATUS.nREG & VIOC_DISP_IREQ_DD_MASK && pDISP->uLIRQMASK.bREG.DD == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pDISP->uLSTATUS.bREG.DD = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}

	if (pDISP->uLSTATUS.nREG & VIOC_DISP_IREQ_SREQ_MASK && pDISP->uLIRQMASK.bREG.SREQ == 0 )
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pDISP->uLSTATUS.bREG.SREQ = 1;

		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}
	
	//pRDMA->uIRQMSK 
#endif//
}

// vim:ts=4:et:sw=4:sts=4

