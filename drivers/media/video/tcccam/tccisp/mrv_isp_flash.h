/*****************************************************************************/
/*!
 *  \file        mrv_isp_flash.h \n
 *  \version     1.0 \n
 *  \author      Zupp \n
 *  \brief       File contains function prototypes and \n
 *                                    definitions for driver Marvin ISP Flash \n
 *                                    Light Control driver module. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_isp_flash.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_FLASH_LIGHT_H
#define _MRV_FLASH_LIGHT_H


/******************************************************************************
 * Typedefs
 ******************************************************************************/

typedef struct
{
    BOOL bPreflashOn;
    BOOL bFlashOn;
    BOOL bPrelightOn;
} tsMrvFlashCmd;


typedef struct
{
    BOOL  bPrelightOffAtEndOfFlash;
    BOOL  bVsyncEdgePositive;
    BOOL  bOutputPolarityLowActive;
    BOOL  bUseExternalTrigger;
    UINT8 ucCaptureDelay;
} tsMrvFlashConfig;


/******************************************************************************
 * Public Flash Light Interface
 ******************************************************************************/
RESULT MrvFlashLightInit( void );
RESULT MrvFlashSetCmdRegister( const tsMrvFlashCmd *ptMrvFlashCmd );
RESULT MrvFlashLightSetConfigRegister( const tsMrvFlashConfig *tMrvFlashConfig );
RESULT MrvFlashLightGetConfigRegister( tsMrvFlashConfig *tMrvFlashConfig );
RESULT MrvFlashLightSetPrelightModeConfig( const BOOL bPrelightModeConfig );
RESULT MrvFlashLightGetPrelightModeConfig( BOOL *pbPrelightModeConfig );
RESULT MrvFlashLightSetVsyncTriggerEdge( const BOOL bVsyncTriggerEdge );
RESULT MrvFlashLightGetVsyncTriggerEdge( BOOL *pbVsyncTriggerEdge );
RESULT MrvFlashLightSetOutSignalPolarity( const BOOL bOutputSignalPolarity );
RESULT MrvFlashLightGetOutSignalPolarity( BOOL *pbOutputSignalPolarity );
RESULT MrvFlashLightSetTriggerSource( const BOOL bTriggerSource );
RESULT MrvFlashLightGetTriggerSource( BOOL *pbTriggerSource );
RESULT MrvFlashLightSetCaptureDelayFrameNum( const UINT8 uFrameNum );
RESULT MrvFlashLightGetCaptureDelayFrameNum( UINT8 *puFrameNum );
RESULT MrvFlashLightSetMaxFlashTime( const UINT16 uMaxFlashTime );
RESULT MrvFlashLightGetMaxFlashTime( UINT16 *puMaxFlashTime );
RESULT MrvFlashLightSetFlashTime( const UINT32 uFlashTime );
RESULT MrvFlashLightGetFlashTime( UINT32 *puFlashTime );
RESULT MrvFlashLightSetFlashDelay( const UINT32 uFlashDelay );
RESULT MrvFlashLightGetFlashDelay( UINT32 *puFlashDelay );
RESULT MrvFlashLightSetDelayPreDevider( const UINT16 uFlashDelayPreDevider );
RESULT MrvFlashLightGetDelayPreDevider( UINT16 *puFlashDelayPreDevider );


#endif //#ifndef _MRV_FLASH_LIGHT_H
