/*****************************************************************************/
/*!
 *  \file        mrv_isp_hist_calc.h \n
 *  \version     1.0 \n
 *  \author      Zupp \n
 *  \brief       File contains public function prototypes and
 *               definitions for Marvin histogram calculation
 *               low level driver. \n
 *               The histogram calculation is a measurement block
 *               which counts the number of pixels of the same value. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_isp_hist_calc.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_ISP_HIST_CALC_H
#define _MRV_ISP_HIST_CALC_H


/******************************************************************************
 * Public Histogram Calculation Defines and Typedefs
******************************************************************************/

#define MRV_HIST_DATA_BIN_ARR_SIZE 16
    typedef struct
    {
        UINT16 usHistBin[MRV_HIST_DATA_BIN_ARR_SIZE];
    } tsMrvHistDataBin;
/******************************************************************************
 * Public Histogram Calculation Interface
 ******************************************************************************/
RESULT MrvHistCalcInit( void );

RESULT MrvHistCalcSetMode( 
   const UINT8 uHistogramMode
   );
   
RESULT MrvHistCalcGetMode( 
   UINT8 *puHistogramMode
   );
   
RESULT MrvHistCalcSetStepSize( 
   const UINT8 uHistogramStepSize
   );

RESULT MrvHistCalcGetStepSize( 
   UINT8 *puHistogramStepSize
   );
RESULT MrvHistCalcSetWeightMatrix(
   const tsMrvHistMatrix *ptMrvHistMatrix
   );
RESULT MrvHistCalcGetWeightMatrix(
   tsMrvHistMatrix *ptMrvHistMatrix
   );

RESULT MrvHistCalcSetHistogramWindow( 
   const tsMrvWindow *ptWnd
   );
   
RESULT MrvHistCalcGetHistogramWindow( 
   tsMrvWindow *ptWnd
   );
   
RESULT MrvHistCalcGetOneMeasureResultDataBin(
   UINT8   ucDataBinIndex,
   UINT16 *pusMeasureResultDataBin
   );
   
RESULT MrvHistCalcGetAllMeasureResultDataBin( 
   tsMrvHistDataBin *psHistDataBin
   );

#endif //#ifndef _MRV_ISP_HIST_CALC_H
