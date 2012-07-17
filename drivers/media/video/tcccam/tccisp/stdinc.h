/*****************************************************************************/
/*!
 *  \file        stdinc.h \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Standard include file; collects some very common includes in
 *               reasonable order. \n
 *
 *  \revision    $Revision: 463 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-07-07 19:43:29 +0200 (Di, 07 Jul 2009) $ \n
 *               $Id: stdinc.h 463 2009-07-07 17:43:29Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _STDINC_H
#define _STDINC_H

//#define SL_FULL_SUPPORT
#define SL_SUPPORT_PARTLY

#include "debug_filter.h"
#include "project_settings.h"
//#include <stdarg.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <time.h>
//#include <math.h>
//#include <float.h>
#include "def.h"
//#include "mem.h"
//#include "mem_usage.h"
//#include "core.h"
//#include "csc.h"
//#include "gpio.h"
//#include "i2c.h"
#include "isi.h"
//#include "keyled.h"
//#include "motodrive.h"
//#include "mdi_motor.h"
//#include "lcd.h"
//#include "lcd_sls.h"
#include "mrv.h"
//#include "algorithms.h"
//#include "message_system.h"
//#include "fontprint_interface.h"
//#include "uisupport_interface.h"
//#include "debug.h"
#include "mrv_isp_cac.h"
#include "mrv_mipi.h"
//#include "mrv_smia.h"
//#include "mrv_smia_mipi.h"
#include "mrv_isp_dpcc.h"
#include "mrv_isp_dpf.h"
#include "mrv_isp_flash.h"
#include "mrv_isp_shutter.h"
#include "mrv_isp_hist_calc.h"
#include "mrv_isp_exposure.h"
//#include "cem_rc.h"
//#include "xchg_global.h"
#include "mrv_sls.h"
#include "utl_fixfloat.h"

#include "isp_interface.h"

//#include <reg_physical.h>

// extra definition
// ======================================================================
//#define MEM_MRV_REG_BASE                                  (volatile unsigned long*)0xB0210000
//#define IO_OFFSET		0x00000000	/* Virtual IO = 0xB0000000 */
#define MEM_MRV_REG_BASE                                  (volatile unsigned long*)(RegISP_BASE+IO_OFFSET)//(volatile unsigned long*)(0xB0210000+IO_OFFSET)
/* Physical value to Virtual Address */
//#define tcc_p2v(pa)	((unsigned int)(pa) + IO_OFFSET)



#define ASSERT(...) 
#define DBG_OUT( x )    

extern void* memset(void *dest, int c, int count );

extern UINT32 MrvSls_CalcScaleFactors( const tsMrvDatapathDesc *ptSource,
                                     const tsMrvDatapathDesc *ptPath,
                                     tsMrvScale* ptScale,
                                     int iImplementation )   ;
extern const tsMrvRszLut* MrvSls_GetRszLut( UINT32 ulFlags );
extern RESULT MrvSls_CalcMainPathSettings( const tsMrvDatapathDesc *ptSource,
                                           const tsMrvDatapathDesc *ptMain,
                                           tsMrvScale* ptScaleM,
                                           tsMrvMiCtrl* ptMrvMiCtrl);
extern RESULT MrvSls_CalcSelfPathSettings( const tsMrvDatapathDesc *ptSource,
                                           const tsMrvDatapathDesc *ptSelf,
                                           tsMrvScale* ptScaleS,
                                           tsMrvMiCtrl* ptMrvMiCtrl);
extern void MiscSetupExtMemory(int iWidth, int iHeight, teBufConfig eBufType, tsMrvMiPathConf* ptMrvMiPathConf, unsigned addr );

#endif //_STDINC_H
