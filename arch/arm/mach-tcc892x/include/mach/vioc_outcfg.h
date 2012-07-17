#ifndef __VIOC_CPUIF_H__
#define	__VIOC_CPUIF_H__
#include <mach/reg_physical.h>

////////////////////////////////////////////////////////////////////////////////
//
//	OUTPUT CONFIGURATION
//
#define		VIOC_OUTCFG_HDMI			0
#define		VIOC_OUTCFG_SDVENC		1
#define		VIOC_OUTCFG_HDVENC		2
#define		VIOC_OUTCFG_M80			3
#define		VIOC_OUTCFG_MRGB			4

#define		VIOC_OUTCFG_DISP0			0
#define		VIOC_OUTCFG_DISP1			1
#define		VIOC_OUTCFG_DISP2			2

extern void VIOC_OUTCFG_SetOutConfig (unsigned  nType, unsigned nDisp);

#endif
