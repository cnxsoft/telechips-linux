/*****************************************************************************/
/*!
 *  \file        mrv_isp_exposure.h \n
 *  \version     1.0 \n
 *  \author      Kaiser \n
 *  \brief       File contains function prototypes and
 *               definitions for Marvin ISP Exposure
 *                                    measurement module. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_isp_exposure.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

/******************************************************************************
 * Public Mean Luminance Measurement Defines and Typedefs
******************************************************************************/

#define MRV_MEAN_LUMA_ARR_SIZE_COL 5 // must be > Zero
#define MRV_MEAN_LUMA_ARR_SIZE_ROW 5 // must be > Zero
#define MRV_MEAN_LUMA_ARR_SIZE     (MRV_MEAN_LUMA_ARR_SIZE_COL*MRV_MEAN_LUMA_ARR_SIZE_ROW)

// Structure contains a 2-dim 5x5 array for mean luminance values from 5x5 MARVIN measurement grid.
typedef struct
{
    UINT8 ucMeanLumaBlock[MRV_MEAN_LUMA_ARR_SIZE_COL][MRV_MEAN_LUMA_ARR_SIZE_ROW];
} tsMrvMeanLuma;

// Structure contains bits autostop and exp_meas_mode of isp_exp_ctrl
typedef struct
{
    BOOL bAutoStop;
#if (MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4)
    BOOL bExpMeasMode;
    BOOL bExpStart;
#endif //MARVIN_FEATURE_AUTO_EXPOSURE
} tsIspExpCtrl;

/******************************************************************************
 * Public Mean Luminance Measurement Interface
 ******************************************************************************/

RESULT MrvIspMeasExposureInitializeModule( void );

RESULT MrvIspMeasExposureSetConfig( const tsMrvWindow *ptWnd, const tsIspExpCtrl *ptIspExpCtrl );
RESULT MrvIspMeasExposureGetConfig( tsMrvWindow *ptWnd, tsIspExpCtrl *ptIspExpCtrl );

RESULT MrvIspMeasExposureGetMeanLumaValues( tsMrvMeanLuma *ptMrvMeanLuma );
RESULT MrvIspMeasExposureGetMeanLumaByNum( UINT8 BlockNum, UINT8 *pLuma );
RESULT MrvIspMeasExposureGetMeanLumaByPos( UINT8 XPos, UINT8 YPos, UINT8 *pLuma );
