/*****************************************************************************/
/*!
 *  \file        mrv_smia.h \n
 *  \version     1.0 \n
 *  \author      Zupp \n
 *  \brief       Header file for Marvin SMIA low level driver functionality. \n
 *               This file contains all module relevant function prototypes
 *               and definitions. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_smia.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_SMIA_H
#define _MRV_SMIA_H


/******************************************************************************
 * Defines
 ******************************************************************************/

// SMIA interrup mask defines
#if ( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE )
#define SMIA_INTRPT_FIFO_FILL_LEVEL    0x00000020  // mask for SMIA fifo fill level interrupt
#define SMIA_INTRPT_SYNC_FIFO_OVFLW    0x00000010  // to be documented
#define SMIA_INTRPT_ERR_CS             0x00000008  // to be documented
#define SMIA_INTRPT_ERR_PROTOCOL       0x00000004  // to be documented
#define SMIA_INTRPT_EMB_DATA_OVFLW     0x00000002  // mask for SMIA embedded data overflow interrupt
#define SMIA_INTRPT_FRAME_END          0x00000001  // mask for SMIA frame end interrupt
#define SMIA_INTRPT_ALL                0x0000003F  // mask for all SMIA interrupts
#endif //#if( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE )

#if ( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) 
#define SMIA_INTRPT_EMB_DATA_OVFLW     0x00000002  // mask for SMIA embedded data overflow interrupt
#define SMIA_INTRPT_FRAME_END          0x00000001  // mask for SMIA frame end interrupt
#define SMIA_INTRPT_ALL                0x00000003  // mask for all SMIA interrupts
#endif //#if( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP )

// SMIA output picture format definitions
#if ( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE )
#define SMIA_PIC_FORMAT_COMPRESSED           0x0000000F  // mask for SMIA 
#define SMIA_PIC_FORMAT_RAW_8_TO_10_DECOMP   0x0000000D  // mask for SMIA 
#define SMIA_PIC_FORMAT_RAW_12               0x0000000C  // mask for SMIA 
#define SMIA_PIC_FORMAT_RAW_10               0x0000000B  // mask for SMIA 
#define SMIA_PIC_FORMAT_RAW_8                0x0000000A  // mask for SMIA 
#define SMIA_PIC_FORMAT_RAW_7                0x00000009  // mask for SMIA 
#define SMIA_PIC_FORMAT_RAW_6                0x00000008  // mask for SMIA 
#define SMIA_PIC_FORMAT_RGB_888              0x00000006  // mask for SMIA
#define SMIA_PIC_FORMAT_RGB_565              0x00000005  // mask for SMIA
#define SMIA_PIC_FORMAT_RGB_444              0x00000004  // mask for SMIA
#define SMIA_PIC_FORMAT_YUV_420              0x00000001  // mask for SMIA
#define SMIA_PIC_FORMAT_YUV_422              0x00000000  // mask for SMIA 
#endif //#if( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE )


#if ( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP)
#define SMIA_PIC_FORMAT_RAW_8_TO_10_DECOMP   0x0000000D  // mask for SMIA
#define SMIA_PIC_FORMAT_RAW_10               0x0000000B  // mask for SMIA  
#define SMIA_PIC_FORMAT_RAW_8                0x0000000A  // mask for SMIA
#endif //#if( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP )


/******************************************************************************
 * Public SMIA Interface
 ******************************************************************************/

RESULT MrvSmiaInitializeModule( void );
RESULT MrvSmiaGetControlRegisterStatus( UINT32 *puSmiaControlRegisterStatus );
RESULT MrvSmiaSetControlRegisterStatus( const UINT32 uSmiaControlRegisterStatus );
RESULT MrvSmiaActivateModule( const BOOL bModuleActive );
RESULT MrvSmiaSetDmaChannel( const UINT8 uDmaChannel );
RESULT MrvSmiaGetDmaChannel( UINT8 *puDmaChannel );
RESULT MrvSmiaSetShutdownLane( const BOOL bShutdownLane );
RESULT MrvSmiaGetShutdownLane( BOOL *pbShutdownLane );
RESULT MrvSmiaActivateCfgUpdateSignal( const BOOL bCfgUpdateSignalActive );
RESULT MrvSmiaResetInterrups( void );
RESULT MrvSmiaFreeEmbDataFifo( void );
RESULT MrvSmiaGetCurrentDmaChannel( BOOL *puDmaChannel );
RESULT MrvSmiaEmbDataAvailabe( BOOL *pbEmbDataAvail );
RESULT MrvSmiaActivateInterrupts( const UINT32 uActivatedInterrupt );
RESULT MrvSmiaGetImscRegister( UINT32 *puActivatedInterrupt );
RESULT MrvSmiaGetGeneralInterruptState( UINT32 *puGeneralInterruptStatus );
RESULT MrvSmiaGetActivatedInterruptState( UINT32 *puActivatedInterruptStatus );
RESULT MrvSmiaSetInterruptRegister( const UINT32 uInterruptRegisterMask, const UINT32 uNewInterruptRegisterValue );
RESULT MrvSmiaSetPictureFormat( const UINT32 uPictureFormat );
RESULT MrvSmiaGetPictureFormat( UINT32 *puPictureFormat );
RESULT MrvSmiaSetFrameLines( const UINT32 uNumOfEmbDataLines, const UINT32 uNumOfPicDataLines );
RESULT MrvSmiaGetFrameLines( UINT32 *puNumOfEmbDataLines, UINT32 *puNumOfPicDataLines );
RESULT MrvSmiaSetEmbDataStorage( const UINT32 uEmbDataLine, const UINT32 uEmbDataPos, const UINT32 uNumOfEmbDataBytes );
RESULT MrvSmiaGetEmbDataStorage( UINT32 *puEmbDataLine, UINT32 *puEmbDataPos, UINT32 *puNumOfEmbDataBytes );
RESULT MrvSmiaGetEmbData(UINT8 *puDestEmbDataBuffer, const UINT32 ulDestEmbDataBufSize, UINT32 *puNumOfEmbDataBytes);
RESULT MrvSmiaSetEmbDataFillLevel( const UINT32 ulFillLevel );
RESULT MrvSmiaGetEmbDataFillLevel( UINT32 *pulFillLevel );


#endif //#ifndef _MRV_SMIA_H
