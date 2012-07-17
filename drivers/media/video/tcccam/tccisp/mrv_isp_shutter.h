/*****************************************************************************/
/*!
 *  \file        mrv_isp_shutter.h \n
 *  \version     1.0 \n
 *  \author      Kaiser \n
 *  \brief       File contains function prototypes and \n
 *               definitions for Marvin ISP Mechanical \n
 *               Shutter driver module. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_isp_shutter.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/
//! mechanical shutter configuration
typedef struct
{
    BITFIELD       bIsLowActive     :1;      //! shutter_open is low  active
    BITFIELD       bUsePositiveEdge :1;      //! use positive edge of trigger signal
    BITFIELD       bUseTrigger      :1;      //! use "shutter_trig" for trigger event(positive edge)
    BITFIELD       bUseRepetition   :1;      //! shutter is opened with the repedition rate of the trigger signal
    BITFIELD       bEnable          :1;      //! enable the mechanical shutter
} tsMrvMechShutterConfig;


/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
RESULT MrvIspMechShutterInitializeModule( void );
RESULT MrvIspMechShutterSetConfig( const tsMrvMechShutterConfig *ptShutterConfig );
RESULT MrvIspMechShutterGetConfig( tsMrvMechShutterConfig *ptShutterConfig );

RESULT MrvIspMechShutterSetOpenPolarity( const BOOL bIsLowActive );
RESULT MrvIspMechShutterGetOpenPolarity( BOOL *pbIsLowActive );
RESULT MrvIspMechShutterSetInputTriggerPolarity( const BOOL bUsePositiveEdge );
RESULT MrvIspMechShutterGetInputTriggerPolarity( BOOL *pbUsePositiveEdge );
RESULT MrvIspMechShutterSetTriggerSource( const BOOL bUseTrigger );
RESULT MrvIspMechShutterGetTriggerSource( BOOL *pbUseTrigger );
RESULT MrvIspMechShutterSetRepetition( const BOOL bUseRepetition );
RESULT MrvIspMechShutterGetRepetition( BOOL *pbUseRepetition );
RESULT MrvIspMechShutterEnable( const BOOL bEnable );
RESULT MrvIspMechShutterIsEnabled( BOOL *pbEnable );

RESULT MrvIspMechShutterGetPreDivider( UINT16 *puPreDivider );
RESULT MrvIspMechShutterSetPreDivider( const UINT16 uPreDivider );
RESULT MrvIspMechShutterGetDelay( UINT32 *puDelay );
RESULT MrvIspMechShutterSetDelay( const UINT32 uDelay );
RESULT MrvIspMechShutterGetTime( UINT32 *puTime );
RESULT MrvIspMechShutterSetTime( const UINT32 uTime );
