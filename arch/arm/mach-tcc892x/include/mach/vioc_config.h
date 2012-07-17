/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : Vioc_config.h
*
*  Description :
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/08     0.1            created                      hskim
*******************************************************************************/

#ifndef __VIOC_CONFIG_H__
#define	__VIOC_CONFIG_H__

#include  <mach/reg_physical.h>

#if 0
typedef enum
{
	VIOC_CONFIG_WMIX0 = 0, /* 4 X 2 */
	VIOC_CONFIG_WMIX1,	/* 2 X 2 (0)*/
	VIOC_CONFIG_WMIX2,	/* 2 X 2 (1)*/
	VIOC_CONFIG_WMIX_MAX
} VIOC_CONFIG_WMIX_Type;

typedef	struct
{
	unsigned int	L0EVSEL	:  3;				// TCC8910 | TCC8010
	unsigned int	RESERVE0	:  1;
	unsigned int	L1EVSEL	:  3;				// TCC8910
	unsigned int	RESERVE1	:  1;
	unsigned int	L2EVSEL	:  3;				// TCC8910
	unsigned int	RESERVE2	:  1;
	unsigned int	RESERVE3	:  4;
	unsigned int	WMIX0_0	:  1;	// 16		// TCC8910 | TCC8010
	unsigned int	WMIX0_1	:  1;	// 17		// TCC8910 | TCC8010
	unsigned int	WMIX1_0	:  1;	// 18		// TCC8910 | TCC8010
	unsigned int	WMIX1_1	:  1;	// 19		// TCC8910 | TCC8010
	unsigned int	WMIX2_0	:  1;	// 20		// TCC8910 | TCC8010
	unsigned int	WMIX2_1	:  1;	// 21		// TCC8910 | TCC8010
	unsigned int	WMIX3_0	:  1;	// 22		// TCC8910
	unsigned int	WMIX3_1	:  1;	// 23		// TCC8910
	unsigned int	WMIX4_0	:  1;	// 24		// TCC8910
	unsigned int	WMIX4_1	:  1;	// 25		// TCC8910
	unsigned int	WMIX5_0	:  1;	// 26		// TCC8910
	unsigned int	WMIX5_1	:  1;	// 27		// TCC8910
	unsigned int	WMIX6_0	:  1;	// 28		// TCC8910
	unsigned int	WMIX6_1	:  1;	// 29		// TCC8910
#if	defined(TCC8910)
	unsigned int	RDMA12	:  1;	// 30
	unsigned int	RDMA14	:  1;	// 31
#elif defined(TCC801X)
	unsigned int	RDMA03	:  1;	// 30
	unsigned int	RDMA04	:  1;	// 31
#endif
}	VIOC_CONFIG_ETC;

typedef	union
{
	unsigned int			nReg;
	VIOC_CONFIG_ETC		bReg;
} VIOC_CONFIG_ETC_u;

typedef	struct
{
	unsigned int	SELECT    	:  8;
	unsigned int	RESERVE0	:  8;
	unsigned int	STATUS     	:  2;
	unsigned int	ERROR      	:  1;
	unsigned int	RESERVE1	:  5;
	unsigned int	RESERVE2	:  7;
	unsigned int	ENABLE     	:  1;
} VIOC_CONFIG_PATH;

typedef	union
{
	unsigned int			nReg;
	VIOC_CONFIG_PATH	bReg;
} VIOC_CONFIG_PATH_u;

typedef	struct
{
	volatile VIOC_CONFIG_ETC_u	uMISC;					// 16
	volatile VIOC_CONFIG_PATH_u	uSC0;					// 17
	volatile VIOC_CONFIG_PATH_u	uSC1;					// 18
	volatile VIOC_CONFIG_PATH_u	uSC2;					// 19
	volatile VIOC_CONFIG_PATH_u	uSC3;					// 20
	volatile VIOC_CONFIG_PATH_u	uVIQE;					// 21
	volatile VIOC_CONFIG_PATH_u	uDEINTLS;				// 22
	volatile VIOC_CONFIG_PATH_u	uFILT2D;				// 23
	volatile VIOC_CONFIG_PATH_u	uFCDEC0;				// 24
	volatile VIOC_CONFIG_PATH_u	uFCDEC1;				// 25
	volatile VIOC_CONFIG_PATH_u	uFCDEC2;				// 26
	volatile VIOC_CONFIG_PATH_u	uFCDEC3;				// 27
	volatile VIOC_CONFIG_PATH_u	uFCENC0;				// 28
	volatile VIOC_CONFIG_PATH_u	uFCENC1;				// 29
	volatile VIOC_CONFIG_PATH_u	uFDELAY0;				// 30
	volatile VIOC_CONFIG_PATH_u	uFDELAY1;				// 31
	volatile VIOC_CONFIG_PATH_u	uDEBLOCK;				// 32
}	VIOC_CONFIG;
#endif

typedef enum {
	WMIX00 = 0,
	WMIX03,
	WMIX10,
	WMIX13,
	WMIX30,
	WMIX40,
	WMIX50,
	WMIX60,
	WMIX_MAX
} VIOC_CONFIG_WMIX_PATH;


/* Interface APIs */
extern void VIOC_CONFIG_SetWindowMixerPath(unsigned int nChannel, unsigned int nSub, unsigned int nValue);
extern VIOC_CONFIG_PATH_u *VIOC_CONFIG_GetPathStruct(unsigned int nType);
extern int VIOC_CONFIG_PlugIn(unsigned int nType, unsigned int nValue);
extern int VIOC_CONFIG_PlugOut(unsigned int nType);
extern void VIOC_CONFIG_RDMA12PathCtrl(unsigned int Path);
extern void VIOC_CONFIG_RDMA14PathCtrl(unsigned int Path);
extern void VIOC_CONFIG_WMIXPath(unsigned int Path, unsigned int Mode);
extern int VIOC_CONFIG_CheckPlugInOut(unsigned int nDevice);
#endif



