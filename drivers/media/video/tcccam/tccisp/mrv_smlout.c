/*****************************************************************************/
/*!
 *  \file        mrv_smlout.c \n
 *  \version     1.0 \n
 *  \author      Schueler \n
 *  \brief       functions to access the Marvin small output interface \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_smlout.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

#if (MARVIN_FEATURE_SMALL_OUTUNIT  == MARVIN_FEATURE_EXIST)
    #include "reg_access.h"
    #include "mrv_priv.h"
#endif // #if (MARVIN_FEATURE_SMALL_OUTUNIT  == MARVIN_FEATURE_EXIST)


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSmlOutSetPath \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  discrete values to control the dat path \n
 *               tMainPath: eMrvPathOn  turn picture path on \n 
 *                          eMrvPathJpe choose JPEG encoder output \n
 *                          eMrvPathOff turn main picture path off \n
 *  \DESCRIPTION sets the Marvin small output interface path \n
 */
/*****************************************************************************/
void MrvSmlOutSetPath(teMrvPath tMainPath)
{
#if (MARVIN_FEATURE_SMALL_OUTUNIT  == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // switch main picture path
    switch (tMainPath)
    {
    case eMrvPathOn:
        REG_SET_SLICE(ptMrvReg->emp_out_ctrl, MRV_EMPOUT_JPEG_ENABLE, DISABLE);
        REG_SET_SLICE(ptMrvReg->emp_out_ctrl, MRV_EMPOUT_MP_ENABLE,    ENABLE);
        break;
    case eMrvPathJpe:
        REG_SET_SLICE(ptMrvReg->emp_out_ctrl, MRV_EMPOUT_JPEG_ENABLE,  ENABLE);
        REG_SET_SLICE(ptMrvReg->emp_out_ctrl, MRV_EMPOUT_MP_ENABLE,   DISABLE);
        break;
    case eMrvPathOff:
        REG_SET_SLICE(ptMrvReg->emp_out_ctrl, MRV_EMPOUT_JPEG_ENABLE, DISABLE);
        REG_SET_SLICE(ptMrvReg->emp_out_ctrl, MRV_EMPOUT_MP_ENABLE,   DISABLE);
        break;
    default:
        ASSERT("MrvSmlOutSetPath()" == "Wrong main picture path!");
        break;
    }
#else // #if (MARVIN_FEATURE_SMALL_OUTUNIT  == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(tMainPath);
#endif // #if (MARVIN_FEATURE_SMALL_OUTUNIT  == MARVIN_FEATURE_EXIST)
}

