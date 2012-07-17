/*****************************************************************************/
/*!
 *  \file        mrv_si.c \n
 *  \version     1.0 \n
 *  \author      Groennert \n
 *  \brief       Super impose module driver. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_si.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    #include "reg_access.h"
    #include "mrv_priv.h"
#endif // #if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information


/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/


/*****************************************************************************
 * GLOBALS
 *****************************************************************************/


/*****************************************************************************
 * PRIVATE PROTOTYPES
 *****************************************************************************/


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSiSetConfig \n
 *  \RETURNVALUE \ref RET_NOTSUPP - this derivative of the MARVIN does not
 *                                  support super impose \n
 *               \ref RET_SUCCESS - everything is okay \n
 *               \ref RET_FAILURE - eTransMode and/or eRefImage contain an
 *                                  unsupported value \n
 *  \PARAMETERS  pointer where to get the new super impose settings \n
 *  \NOTES       In the case the pointer is NULL the SI module will be switched
 *               off. \n
 *
 *  \DESCRIPTION Applies the new super impose settings to the module. \n
 */
/*****************************************************************************/
RESULT MrvSiSetConfig(const tsMrvSiConfig *ptSiConfig)
{
#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 super_imp_ctrl = REG_READ(ptMrvReg->super_imp_ctrl);

    // stop module first
    REG_SET_SLICE(ptMrvReg->super_imp_ctrl, MRV_SI_BYPASS_MODE, MRV_SI_BYPASS_MODE_BYPASS);

    // NULL pointer means disable SI module
    if (ptSiConfig)
    {
        // set transparency mode (Note: 1 means disabled, 0 enabled)
        switch (ptSiConfig->eTransMode)
        {
        case eMrvSi_TransDisable:
            REG_SET_SLICE(super_imp_ctrl, MRV_SI_TRANSPARENCY_MODE, MRV_SI_TRANSPARENCY_MODE_DISABLED);
            break;
        case eMrvSi_TransEnable:
            REG_SET_SLICE(super_imp_ctrl, MRV_SI_TRANSPARENCY_MODE, MRV_SI_TRANSPARENCY_MODE_ENABLED);
            break;
        case eMrvSi_TransUnknown:    
        default:
            DBG_OUT((DERR, "MrvSiSetConfig(): eTransMode contains an unsupported value (0x%08X)\n", ptSiConfig->eTransMode));
            return RET_FAILURE;
        }

        // set reference image
        switch (ptSiConfig->eRefImage)
        {
        case eMrvSi_RefImgMemory:
            REG_SET_SLICE(super_imp_ctrl, MRV_SI_REF_IMAGE, MRV_SI_REF_IMAGE_MEM);
            break;
        case eMrvSi_RefImgSensor:
            REG_SET_SLICE(super_imp_ctrl, MRV_SI_REF_IMAGE, MRV_SI_REF_IMAGE_IE);
            break;
        case eMrvSi_RefImgUnknown:    
        default:
            DBG_OUT((DERR, "MrvSiSetConfig(): eRefImage contains an unsupported value (0x%08X)\n", ptSiConfig->eRefImage));
            return RET_FAILURE;
        }

        // set x offset value (bit 0 has to be 0)
        if (ptSiConfig->usOffsX > MRV_SI_OFFSET_X_MAX)
        {
            REG_SET_SLICE(ptMrvReg->super_imp_offset_x, MRV_SI_OFFSET_X, MRV_SI_OFFSET_X_MAX);
        }
        else
        {
            REG_SET_SLICE(ptMrvReg->super_imp_offset_x, MRV_SI_OFFSET_X, (UINT32)ptSiConfig->usOffsX);
        }

        // set y offset value
        if (ptSiConfig->usOffsY > MRV_SI_OFFSET_Y_MAX)
        {
            REG_SET_SLICE(ptMrvReg->super_imp_offset_y, MRV_SI_OFFSET_Y, MRV_SI_OFFSET_Y_MAX);
        }
        else
        {
            REG_SET_SLICE(ptMrvReg->super_imp_offset_y, MRV_SI_OFFSET_Y, (UINT32)ptSiConfig->usOffsY);
        }

        // set Y component of transparent key color
        REG_SET_SLICE(ptMrvReg->super_imp_color_y,  MRV_SI_Y_COMP,  (UINT32)ptSiConfig->ucTransCompY);
        // set Cb component of transparent key color
        REG_SET_SLICE(ptMrvReg->super_imp_color_cb, MRV_SI_CB_COMP, (UINT32)ptSiConfig->ucTransCompCb);
        // set Cr component of transparent key color
        REG_SET_SLICE(ptMrvReg->super_imp_color_cr, MRV_SI_CR_COMP, (UINT32)ptSiConfig->ucTransCompCr);

        // enable the module and set control register value
        REG_SET_SLICE(super_imp_ctrl, MRV_SI_BYPASS_MODE, MRV_SI_BYPASS_MODE_PROCESS);
        REG_WRITE(ptMrvReg->super_imp_ctrl, super_imp_ctrl);
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    // this derivative of the MARVIN does not support super impose
    UNUSED_PARAM1(ptSiConfig);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSiGetConfig \n
 *  \RETURNVALUE \ref RET_NOTSUPP      - this derivative of the MARVIN does not
 *                                       support super impose \n
 *               \ref RET_SUCCESS      - everything is okay \n
 *               \ref RET_NULL_POINTER - the given pointer is NULL \n
 *  \PARAMETERS  pointer where to store the current super impose settings \n
 *
 *  \DESCRIPTION Returns the current settings of the super impose module. \n
 */
/*****************************************************************************/
RESULT MrvSiGetConfig(tsMrvSiConfig *ptSiConfig)
{
#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 super_imp_ctrl = REG_READ(ptMrvReg->super_imp_ctrl);

    // NULL pointer is not allowed
    if (!ptSiConfig)
    {
        DBG_OUT((DERR, "MrvSiGetConfig(): ptSiConfig == NULL\n"));
        return RET_NULL_POINTER;
    }

    // check if the module is switched on
    if (REG_GET_SLICE(super_imp_ctrl, MRV_SI_BYPASS_MODE) == MRV_SI_BYPASS_MODE_PROCESS)
    { // fill configuration structure
        if (REG_GET_SLICE(super_imp_ctrl, MRV_SI_TRANSPARENCY_MODE) == MRV_SI_TRANSPARENCY_MODE_DISABLED)
        {
            ptSiConfig->eTransMode = eMrvSi_TransDisable;
        }
        else
        {
            ptSiConfig->eTransMode = eMrvSi_TransEnable;
        }

        if (REG_GET_SLICE(super_imp_ctrl, MRV_SI_REF_IMAGE) == MRV_SI_REF_IMAGE_MEM)
        {
            ptSiConfig->eRefImage = eMrvSi_RefImgMemory;
        }
        else
        {
            ptSiConfig->eRefImage = eMrvSi_RefImgSensor;
        }

        ptSiConfig->usOffsX =       (UINT16)REG_GET_SLICE(ptMrvReg->super_imp_offset_x, MRV_SI_OFFSET_X);
        ptSiConfig->usOffsY =       (UINT16)REG_GET_SLICE(ptMrvReg->super_imp_offset_y, MRV_SI_OFFSET_Y);
        ptSiConfig->ucTransCompY  = (UINT8) REG_GET_SLICE(ptMrvReg->super_imp_color_y,  MRV_SI_Y_COMP);
        ptSiConfig->ucTransCompCb = (UINT8) REG_GET_SLICE(ptMrvReg->super_imp_color_cb, MRV_SI_CB_COMP);
        ptSiConfig->ucTransCompCr = (UINT8) REG_GET_SLICE(ptMrvReg->super_imp_color_cr, MRV_SI_CR_COMP);
    }
    else
    { // clear configuration structure
        memset(ptSiConfig, 0, sizeof(ptSiConfig));
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
    // this derivative of the MARVIN does not support super impose
    UNUSED_PARAM(ptSiConfig);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
}
