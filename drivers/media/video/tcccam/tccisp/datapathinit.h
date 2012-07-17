/*****************************************************************************/
/*!
 *  \file        DatapathInit.h \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Interface to various helper routines to initialize 
 *               datapath components. \n
 *
 *  \revision    $Revision: 463 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-07-07 19:43:29 +0200 (Di, 07 Jul 2009) $ \n
 *               $Id: DatapathInit.h 463 2009-07-07 17:43:29Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/


#ifndef _DATAPATH_INIT_H
#define _DATAPATH_INIT_H

extern void   DpInitBasicLcd( const tsPlSystemConfig *ptSysConf, int iContrast );

extern void   DpInitLcd( const tsPlSystemConfig *ptSysConf, int iContrast );
extern void   DpInitIsi( tsIsiSensorConfig *ptIsiConfig, tsIsiSensorCaps *ptIsiCaps );
extern RESULT DpInitChangeIsiConfig( const tsIsiSensorConfig *ptIsiConfig, tsIsiSensorAwbProfile *ptIsiSensorAwbProfile );

extern RESULT DpInitSetupViewfinderPath( const tsPlSystemConfig *ptSysConf, int iZoom, BOOL bInitBuffers );

extern void DpInitCamParams( const tsPlSystemConfig *ptSysConf);

// marvin related
extern RESULT DpInitMrv( tsPlSystemConfig *ptSysConf);
// some of its individual blocks
extern RESULT DpInitMrvImageEffects( const tsPlSystemConfig *ptSysConf, BOOL bEnable );
extern RESULT DpInitMrvIspFilter( const tsPlSystemConfig *ptSysConf, BOOL bEnable );
extern RESULT DpInitMrvIspCAC( const tsPlSystemConfig *ptSysConf, BOOL bEnable );
extern RESULT DpInitMrvIspLensShade( const tsPlSystemConfig *ptSysConf, BOOL bEnable );
extern RESULT DpInitMrvIspBadPixel( const tsPlSystemConfig *ptSysConf, BOOL bEnable );
extern RESULT DpInitBadPixelGenerator( const tsPlSystemConfig *ptSysConf );
extern RESULT DpInitBls( const tsPlSystemConfig *ptSysConf );
extern void MrvSls_WbProcess(tsConfig *ptConfig);



#endif //#ifndef _MISCELLANEOUS_H
