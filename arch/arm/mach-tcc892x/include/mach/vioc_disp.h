/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : vioc_disp.h
*
*  Description : display control component control module
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/15     0.1            created                      hskim
*******************************************************************************/
#ifndef __VIOC_DISP_H__
#define	__VIOC_DISP_H__

#include  <mach/reg_physical.h>


enum {
	VIOC_HDMI_1920X1080P_60Hz = 0,
	VIOC_HDMI_1920X1080P_59Hz,
	VIOC_HDMI_1920X1080P_50Hz,
	VIOC_HDMI_1920X1080I_60Hz,
	VIOC_HDMI_1920X1080I_59Hz,
	VIOC_HDMI_1920X1080I_50Hz,
	VIOC_HDMI_1280X720P_60Hz,
	VIOC_HDMI_1280X720P_59Hz,
	VIOC_HDMI_1280X720P_50Hz,
	VIOC_HDMI_720X480P_60Hz,
	VIOC_HDMI_720X480P_59Hz,
	VIOC_HDMI_720X576P_50Hz,
	VIOC_HDMI_720X480I_60Hz,
	VIOC_HDMI_720X480I_59Hz,
	VIOC_HDMI_720X576I_50Hz,
	VIOC_HDVE_720X480I_60Hz,
	VIOC_HDVE_720X576I_50Hz,
	VIOC_HDVE_720X480P_60Hz,
	VIOC_HDVE_720X576P_50Hz,
	VIOC_HDVE_1280X720P_60Hz,
	VIOC_HDVE_1280X720P_50Hz,
	VIOC_HDVE_1920X1080I_60Hz,
	VIOC_HDVE_1920X1080I_50Hz,
	VIOC_HDVE_1920X1080P_30Hz,
	VIOC_HDVE_SMPTE295I_50Hz,
	VIOC_HDVE_SMPTE295P_30Hz,
	VIOC_SDVE_NTSC_CVBS,
	VIOC_SDVE_PAL_CVBS,
	VIOC_SDVE_NTSC_SVIDEO,
	VIOC_SDVE_PAL_SVIDEO,
	VIOC_SDVE_NTSC_YPbPr,
	VIOC_SDVE_PAL_YPbPr,
	VIOC_SDVE_NTSC_RGB,
	VIOC_SDVE_PAL_RGB,
	VIOC_DEFAUKLT_TIME_MAX
};

typedef struct {
	unsigned int	nType;
	unsigned int	CLKDIV;
	unsigned int	IV;
	unsigned int	IH;
	unsigned int	IP;
	unsigned int	DP;
	unsigned int	NI;
	unsigned int	TV;
	unsigned int	LPW;
	unsigned int	LPC;
	unsigned int	LSWC;
	unsigned int	LEWC;
	unsigned int	FPW;
	unsigned int	FLC;
	unsigned int	FSWC;
	unsigned int	FEWC;
	unsigned int	FPW2;
	unsigned int	FLC2;
	unsigned int	FSWC2;
	unsigned int	FEWC2;
} VIOC_TIMING_INFO;




/* Interface APIs */
extern void VIOC_DISP_SetDefaultTimingParam(VIOC_DISP *pDISP, unsigned int nType);
extern void VIOC_DISP_SetControlConfigure(VIOC_DISP *pDISP, stLCDCTR *pCtrlParam);

extern void VIOC_DISP_SetSize (VIOC_DISP *pDISP, unsigned int nWidth, unsigned int nHeight);
extern void VIOC_DISP_GetSize(VIOC_DISP *pDISP, unsigned int *nWidth, unsigned int *nHeight);

extern void VIOC_DISP_SetBGColor(VIOC_DISP *pDISP, unsigned int BG0, unsigned int BG1, unsigned int BG2);
extern void VIOC_DISP_SetPosition(VIOC_DISP *pDISP, unsigned int startX, unsigned int startY );
extern void VIOC_DISP_GetPosition(VIOC_DISP *pDISP, unsigned int *startX, unsigned int *startY );
extern void VIOC_DISP_SetColorEnhancement(VIOC_DISP *pDISP, signed char contrast, signed char brightness, signed char hue );
extern void VIOC_DISP_GetColorEnhancement(VIOC_DISP *pDISP, signed char *contrast, signed char *brightness, signed char *hue );
extern void VIOC_DISP_SetClippingEnable(VIOC_DISP *pDISP, unsigned int enable);
extern void VIOC_DISP_GetClippingEnable(VIOC_DISP *pDISP, unsigned int *enable);
extern void VIOC_DISP_SetClipping(VIOC_DISP *pDISP, unsigned int uiUpperLimitY, unsigned int uiLowerLimitY, unsigned int uiUpperLimitUV, unsigned int uiLowerLimitUV);
extern void VIOC_DISP_GetClipping(VIOC_DISP *pDISP, unsigned int *uiUpperLimitY, unsigned int *uiLowerLimitY, unsigned int *uiUpperLimitUV, unsigned int *uiLowerLimitUV);
extern void VIOC_DISP_SetDither(VIOC_DISP *pDISP, unsigned int ditherEn, unsigned int ditherSel, unsigned char mat[4][4]);
extern void VIOC_DISP_SetTimingParam (VIOC_DISP *pDISP, stLTIMING *pTimeParam);
//extern void VIOC_DISP_SetPixelClockDiv(VIOC_DISP *pDISP, stLTIMING *pTimeParam);
extern void VIOC_DISP_SetPixelClockDiv(VIOC_DISP *pDISP, unsigned int div);
extern void VIOC_DISP_TurnOn (VIOC_DISP *pDISP);
extern void VIOC_DISP_TurnOff (VIOC_DISP *pDISP);
extern void VIOC_DISP_SetControl(VIOC_DISP *pDISP, stLCDCPARAM *pLcdParam);
extern void VIOC_DISP_SetPort( void );
extern void VIOC_DISP_SWReset( unsigned int DISP );
extern void VIOC_DISP_DisplayOnOff( unsigned int onOff );
extern void VIOC_DISP_SetIreqMask(VIOC_DISP * pDISP, unsigned int mask, unsigned int set);
extern void VIOC_DISP_IreqHandler( unsigned int vectorID );
#endif
