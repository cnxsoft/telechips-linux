/*****************************************************************************/
/*!
 *  \file        mrv_isp_cac.h \n
 *  \version     1.0 \n
 *  \author      Friebe \n
 *  \brief       Public function prototypes and definitions for Marvin
 *               CAC low level driver. \n
 *               The Marvin CAC driver configures the CAC hardware module. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_isp_cac.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_ISP_CAC_H
#define _MRV_ISP_CAC_H


/******************************************************************************
 * Public CAC Defines and Typedefs
******************************************************************************/

//! configuration of chromatic aberration correction block (given to the
//! CAC driver)
typedef struct
{
   UINT16 usHSize;        //! size of the input image in pixels
   UINT16 usVSize;        //! 
   INT16  sHCenterOffset; //! offset between image center and optical 
   INT16  sVCenterOffset; //! center of the input image in pixels
   UINT8  ucHClipMode;    //! maximum red/blue pixel shift in horizontal
   UINT8  ucVClipMode;    //! and vertival direction, range 0..2
   UINT16 usABlue;        //! parameters for radial shift calculation,
   UINT16 usARed;         //! 9 bit twos complement with 4 fractional
   UINT16 usBBlue;        //! digits, valid range -16..15.9375
   UINT16 usBRed;
   UINT16 usCBlue;
   UINT16 usCRed;
   float  fAspectRatio;   //! 0 = square pixel sensor, all other = aspect
                          //! ratio of non-square pixel sensor
} tsMrvCacConfig;


//! register values of chromatic aberration correction block (delivered by
//! the CAC driver)
typedef struct
{
   UINT8  ucHClipMode;    //! maximum red/blue pixel shift in horizontal
   UINT8  ucVClipMode;    //! and vertival direction, range 0..2
   BOOL   bCacEnabled;    //! TRUE=enabled, FALSE=disabled
   UINT16 usHCountStart;  //! preload value of the horizontal CAC pixel
                          //! counter, range 1..4095
   UINT16 usVCountStart;  //! preload value of the vertical CAC pixel
                          //! counter, range 1..4095
   UINT16 usABlue;        //! parameters for radial shift calculation,
   UINT16 usARed;         //! 9 bit twos complement with 4 fractional
   UINT16 usBBlue;        //! digits, valid range -16..15.9375
   UINT16 usBRed;
   UINT16 usCBlue;
   UINT16 usCRed;
   UINT8  ucXNormShift;   //! horizontal normalization shift, range 0..7
   UINT8  ucXNormFactor;  //! horizontal normalization factor, range 16..31
   UINT8  ucYNormShift;   //! vertical normalization shift, range 0..7
   UINT8  ucYNormFactor;  //! vertical normalization factor, range 16..31
} tsMrvCacRegValues;

/******************************************************************************
 * Public CAC Interface
 ******************************************************************************/

RESULT MrvIspCacSetConfig( const tsMrvCacConfig *ptCacConfig );
RESULT MrvIspCacGetRegValues( tsMrvCacRegValues *ptCacRegValues );
RESULT MrvIspCacEnable( void );
RESULT MrvIspCacDisable( void );


#endif //#ifndef _MRV_ISP_CAC_H
