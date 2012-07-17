
/****************************************************************************
 *   FileName    : tca_rtc.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
/*****************************************************************************
*
* Defines
*
******************************************************************************/


/*****************************************************************************
*
* Enum
*
******************************************************************************/
#ifndef __TCA_RTC_H__
#define __TCA_RTC_H__

//#if defined(_LINUX_)
//#include <mach/bsp.h>
//#else
//#include "bsp.h"
//#endif


/*****************************************************************************
*
* Type Defines
*
******************************************************************************/
#define FROM_BCD(n)     ((((n) >> 4) * 10) + ((n) & 0xf))
#define TO_BCD(n)       ((((n) / 10) << 4) | ((n) % 10))
#define RTC_YEAR_DATUM  1980


/*****************************************************************************
*
* Structures
*
******************************************************************************/
typedef struct _rtctime {
  unsigned int wYear;
  unsigned int wMonth;
  unsigned int wDayOfWeek;
  unsigned int wDay;
  unsigned int wHour;
  unsigned int wMinute;
  unsigned int wSecond;
  unsigned int wMilliseconds;
} rtctime;

/*****************************************************************************
*
* External Variables
*
******************************************************************************/


/*****************************************************************************
*
* External Functions
*
******************************************************************************/
#ifdef __cplusplus
extern 
"C" { 
#endif

#include <mach/bsp.h>
volatile void tca_rtc_gettime(unsigned int devbaseaddresss,rtctime *pTime);
volatile void tca_rtc_settime(unsigned int devbaseaddresss,rtctime *pTime);
volatile unsigned int	tca_rtc_checkvalidtime(unsigned int devbaseaddresss);

#ifdef __cplusplus
 } 
#endif


#endif //__TCA_RTC_H__

