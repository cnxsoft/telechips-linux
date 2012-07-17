/*****************************************************************************/
/*!
 *  \file        mrv_is.c \n
 *  \version     1.0 \n
 *  \author      Schueler \n
 *  \brief       image stabilisation module driver. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_is.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h" 

#if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)
    #include "reg_access.h"
    #include "mrv_priv.h"
#endif // #if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)


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
 *  \FUNCTION    MrvIsSetConfig \n
 *  \RETURNVALUE \ref RET_NOTSUPP - this derivative of the MARVIN does not
 *                                  support image stabilisation \n
 *               \ref RET_SUCCESS - everything is okay \n
 *               \ref RET_FAILURE - eMode contain an
 *                                  unsupported value \n
 *  \PARAMETERS  pointer where to get the new image stabilisation settings \n
 *
 *  \DESCRIPTION Applies the new image stabilisation settings to the module. \n
 */
/*****************************************************************************/
RESULT MrvIsSetConfig(const tsMrvIsConfig *ptIsConfig)
{
#if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)
    
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // NULL pointer is not allowed
    if (!ptIsConfig)
    {
        DBG_OUT((DERR, "MrvIsSetConfig: ptIsConfig == NULL\n"));
        return RET_NULL_POINTER;
    }

  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    // set maximal margin distance for X
    if (ptIsConfig->usMaxDx > MRV_IS_IS_MAX_DX_MAX)
    {
        REG_SET_SLICE(ptMrvReg->isp_is_max_dx, MRV_IS_IS_MAX_DX, (UINT32)(MRV_IS_IS_MAX_DX_MAX));
    }
    else
    {
        REG_SET_SLICE(ptMrvReg->isp_is_max_dx, MRV_IS_IS_MAX_DX, (UINT32)(ptIsConfig->usMaxDx));
    }
    // set maximal margin distance for Y 
    if (ptIsConfig->usMaxDy > MRV_IS_IS_MAX_DY_MAX)
    {
        REG_SET_SLICE(ptMrvReg->isp_is_max_dy, MRV_IS_IS_MAX_DY, (UINT32)(MRV_IS_IS_MAX_DY_MAX));
    }
    else
    {
        REG_SET_SLICE(ptMrvReg->isp_is_max_dy, MRV_IS_IS_MAX_DY, (UINT32)(ptIsConfig->usMaxDy));
    }
  #endif // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)

    // set H offset
    REG_SET_SLICE(ptMrvReg->isp_is_h_offs, MRV_IS_IS_H_OFFS, (UINT32)(ptIsConfig->tMrvIsWindow.usHOffs));
    // set V offset
    REG_SET_SLICE(ptMrvReg->isp_is_v_offs, MRV_IS_IS_V_OFFS, (UINT32)(ptIsConfig->tMrvIsWindow.usVOffs));
    // set H size 
    REG_SET_SLICE(ptMrvReg->isp_is_h_size, MRV_IS_IS_H_SIZE, (UINT32)(ptIsConfig->tMrvIsWindow.usHSize));
    // set V size
    REG_SET_SLICE(ptMrvReg->isp_is_v_size, MRV_IS_IS_V_SIZE, (UINT32)(ptIsConfig->tMrvIsWindow.usVSize));
    
    return RET_SUCCESS;    
    
#else // #if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)
    // this derivative of the MARVIN does not support image stabilisation
    UNUSED_PARAM1(ptIsConfig);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIsGetConfig \n
 *  \RETURNVALUE \ref RET_NOTSUPP      - this derivative of the MARVIN does not
 *                                       support image stabilisation \n
 *               \ref RET_SUCCESS      - everything is okay \n
 *               \ref RET_NULL_POINTER - the given pointer is NULL \n
 *  \PARAMETERS  pointer where to store the current image stabilisation settings \n
 *
 *  \DESCRIPTION Returns the current settings of the image stabilisation module. \n
 */
/*****************************************************************************/
RESULT MrvIsGetConfig(tsMrvIsConfig *ptIsConfig)
{
#if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)

    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // NULL pointer is not allowed
    if (ptIsConfig == NULL)
    {
        DBG_OUT((DERR, "MrvIsGetConfig: ptIsConfig == NULL\n"));
        return RET_NULL_POINTER;
    }
    
    // reset the configuration structure
    memset(ptIsConfig, 0, sizeof(ptIsConfig));
    // get H offset
    ptIsConfig->tMrvIsWindow.usHOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_h_offs, MRV_IS_IS_H_OFFS);
    // get V offset
    ptIsConfig->tMrvIsWindow.usVOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_v_offs, MRV_IS_IS_V_OFFS);
    // get H size 
    ptIsConfig->tMrvIsWindow.usHSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_h_size, MRV_IS_IS_H_SIZE);
    // get V size
    ptIsConfig->tMrvIsWindow.usVSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_v_size, MRV_IS_IS_V_SIZE);
    
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    // get maximal margin distance for X 
    ptIsConfig->usMaxDx = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_max_dx, MRV_IS_IS_MAX_DX);
    // get maximal margin distance for Y 
    ptIsConfig->usMaxDy = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_max_dy, MRV_IS_IS_MAX_DY);
  #endif // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    
    return RET_SUCCESS;
    
#else // #if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)
    // this derivative of the MARVIN does not support image stabilisation
    UNUSED_PARAM(ptIsConfig);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIsSetDisplace \n
 *  \RETURNVALUE \ref RET_NOTSUPP      - this derivative of the MARVIN does not
 *                                       support image stabilisation v2 \n
 *               \ref RET_SUCCESS      - everything is okay \n
 *  
 *  \PARAMETERS  sDisPlX .. X component of displacement \n
 *               sDisPlY .. Y component of displacement \n
 *
 *  \DESCRIPTION Set the current displacement of the image stabilisation module. \n
 */
/*****************************************************************************/
RESULT MrvIsSetDisplace(INT16 sDisPlX, INT16 sDisPlY)
{
#if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_is_displace = REG_READ(ptMrvReg->isp_is_displace);
    
    ASSERT(sDisPlX > MRV_IS_DX_MIN);   // X value to low
    ASSERT(sDisPlX < MRV_IS_DX_MAX);   // X value to high
    ASSERT(sDisPlY > MRV_IS_DY_MIN);   // Y value to low
    ASSERT(sDisPlY < MRV_IS_DY_MAX);   // Y value to high
    
    REG_SET_SLICE(isp_is_displace, MRV_IS_DX, (UINT16)(sDisPlX));
    REG_SET_SLICE(isp_is_displace, MRV_IS_DY, (UINT16)(sDisPlY));
    REG_WRITE(ptMrvReg->isp_is_displace, isp_is_displace);
    
    return RET_SUCCESS;
#else   // #if( MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2 )
    // this derivative of the MARVIN does not support image stabilisation v2
    UNUSED_PARAM(sDisPlX);
    UNUSED_PARAM(sDisPlY);
    return RET_NOTSUPP;
#endif  // #if( MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2 )
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIsSetCtrl \n
 *  \RETURNVALUE \ref RET_NOTSUPP      - this derivative of the MARVIN does not
 *                                       support image stabilisation v2 \n
 *               \ref RET_SUCCESS      - everything is okay \n
 *  
 *  \PARAMETERS  pointer where to get the new control settings of image stabilisation \n
 *  \NOTES       In the case the pointer is NULL the IS module will be switched
 *               off. \n
 *
 *  \DESCRIPTION Set the current control of the image stabilisation module. \n
 */
/*****************************************************************************/
RESULT MrvIsSetCtrl(const tsMrvIsCtrl *ptMrvIsCtrl)
{
#if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // stop module first
    REG_SET_SLICE(ptMrvReg->isp_is_ctrl, MRV_IS_IS_EN, DISABLE);
    
    // NULL pointer means disable IS module
    if (ptMrvIsCtrl)
    { 
        if (ptMrvIsCtrl->ucRecenter > MRV_IS_IS_RECENTER_MAX)
        {
            DBG_OUT((DERR, "MrvIsSetCtrl: ucRecenter is out of range (%d, max. allowed %d)\n",
                     (UINT32)ptMrvIsCtrl->ucRecenter, MRV_IS_IS_RECENTER_MAX));
            return RET_OUTOFRANGE;
        }
    
        // set recenter value
        REG_SET_SLICE(ptMrvReg->isp_is_recenter, MRV_IS_IS_RECENTER, (UINT32)(ptMrvIsCtrl->ucRecenter));
        
        // set stabilisation mode
        switch (ptMrvIsCtrl->eIsMode)
        {
        case eMrvIsMode_On:
            REG_SET_SLICE(ptMrvReg->isp_is_ctrl, MRV_IS_IS_EN, ENABLE);
            break;
        case eMrvIsMode_Off:
            REG_SET_SLICE(ptMrvReg->isp_is_ctrl, MRV_IS_IS_EN, DISABLE);
            break;
        case eMrvIsMode_Unknown:    
        default:
            DBG_OUT((DERR, "MrvIsSetCtrl: eIsMode contains an unsupported value (0x%08X)\n", ptMrvIsCtrl->eIsMode));
            return RET_FAILURE;
        }            
    }
    
    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    // this derivative of the MARVIN does not support image stabilisation v2
    UNUSED_PARAM1(ptMrvIsCtrl);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIsGetCtrl \n
 *  \RETURNVALUE \ref RET_NOTSUPP      - this derivative of the MARVIN does not
 *                                       support image stabilisation v2 \n
 *               \ref RET_SUCCESS      - everything is okay \n
 *               \ref RET_NULL_POINTER - the given pointer is NULL \n
 *  
 *  \PARAMETERS  pointer where to store the control settings of image stabilisation \n
 *
 *  \DESCRIPTION Get the current control of the image stabilisation module. \n
 */
/*****************************************************************************/
RESULT MrvIsGetCtrl(tsMrvIsCtrl *ptMrvIsCtrl)
{
#if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // NULL pointer is not allowed
    if (!ptMrvIsCtrl)
    {
        DBG_OUT((DERR, "MrvIsGetCtrl: ptMrvIsCtrl == NULL\n"));
        return RET_NULL_POINTER;
    }
    
    // get recenter value
    ptMrvIsCtrl->ucRecenter = (UINT8)REG_GET_SLICE(ptMrvReg->isp_is_recenter, MRV_IS_IS_RECENTER);
  
    // check if the module is switched on
    if(REG_GET_SLICE(ptMrvReg->isp_is_ctrl, MRV_IS_IS_EN) != DISABLE)
    {
        ptMrvIsCtrl->eIsMode = eMrvIsMode_On;
    }
    else
    {
        ptMrvIsCtrl->eIsMode = eMrvIsMode_Off;
    }
    
    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
    // this derivative of the MARVIN does not support image stabilisation v2
    UNUSED_PARAM(ptMrvIsCtrl);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
}
