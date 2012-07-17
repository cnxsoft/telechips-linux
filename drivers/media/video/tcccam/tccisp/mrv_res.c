/*****************************************************************************/
/*!
 *  \file        mrv_res.c \n
 *  \version     1.0 \n
 *  \author      Friebe \n
 *  \brief       functions to access resize sub-modules \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_res.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

#include "reg_access.h"
#include "mrv_priv.h"


#define RSZ_FLAGS_MASK (RSZ_UPSCALE_ENABLE | RSZ_SCALER_BYPASS)


/*
typedef struct
{
    volatile UINT32* preg_rsz_ctrl;
    volatile UINT32* preg_rsz_scale;
    volatile UINT32* preg_rsz_phase;
} tsScaleInfo;
*/

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvApplyScaleInfo \n
 *  \DESCRIPTION writes the values of a single sub-scaler to the given 
 *               Marvin registers \n
 *  \PARAMETERS  ptScale .. pointer to structure with pointers to the marvin 
 *                          registers to use \n
 *               ulScale .. scaler value from the tsMrvScale-type structure 
 *                          (including the scaler flags) \n
 *               ulPhase .. phase value from the tsMrvScale-type structure \n
 *               ulEnableMask .. bit mask of the associated enable flag in \n
 *                               the scalers control register
 *               ulUpscaleMask .. bit mask of the associated upscale flag in \n
 *                               the scalers control register
 *  \RETURNVALUE nonzero, if the given ulScale parameter indicates that the 
 *               scaler should be active and upscaling. \n
 *               zero in all other cases \n
 */
/*****************************************************************************/
/*
static int MrvApplyScaleInfo(const tsScaleInfo *ptScale,
                                   UINT32 ulScale,
                                   UINT32 ulPhase,
                                   UINT32 ulEnableMask, 
                                   UINT32 ulUpscaleMask)
{
    int ret = 0; // default: no upscaling (scaler or upscaling disabled)

    ASSERT((RSZ_FLAGS_MASK & MRV_RSZ_SCALE_MASK) == 0); // flags must be "outside" scaler value
    ASSERT((ulScale & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MASK);

    if (ulScale & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        *(ptScale->preg_rsz_ctrl) = *(ptScale->preg_rsz_ctrl) & ~ulEnableMask;
    }
    else
    { // enable scaler
        *(ptScale->preg_rsz_ctrl)  = *(ptScale->preg_rsz_ctrl) | ulEnableMask;
        //program scale factor and phase
        REG_SET_SLICE(*(ptScale->preg_rsz_scale), MRV_RSZ_SCALE, ulScale);
        REG_SET_SLICE(*(ptScale->preg_rsz_phase), MRV_RSZ_SCALE, ulPhase);
        
        if (ulScale & RSZ_UPSCALE_ENABLE)
        { // enable upscaling mode
            *(ptScale->preg_rsz_ctrl)  = *(ptScale->preg_rsz_ctrl) | ulUpscaleMask;
            ret = -1; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            *(ptScale->preg_rsz_ctrl)  = *(ptScale->preg_rsz_ctrl) & (~ulUpscaleMask);
        }
    }

    return ret;
}
*/


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvResSetMainResize \n
 *  \PARAMETERS  pointer to scaler values \n
 *               enum for immediate, frame synchronous, or later (external) update \n
 *  \RETURNVALUE none \n
 *
 *  \NOTES       The usScaleXxx fields of the struct pointed to by the 
 *               ptScale parameter contain both the scale factor value and
 *               the combination of the RSZ_SCALER_BYPASS and the 
 *               RSZ_UPSCALE_ENABLE flag.  \n
 *            
 *  \DESCRIPTION writes the scaler values to the appropriate Marvin
 *               registers \n
 */
/*****************************************************************************/
void MrvResSetMainResize(const tsMrvScale *ptScale, teMrvConfUpdateTime eUpdateTime, const tsMrvRszLut *ptMrvRszLut)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mrsz_ctrl  = REG_READ(ptMrvReg->mrsz_ctrl);
    BOOL   iUpscaling = FALSE;

    ASSERT((RSZ_FLAGS_MASK & MRV_RSZ_SCALE_MASK) == 0); // flags must be "outside" scaler value
    ASSERT((ptScale->ulScaleHY  & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleHCB & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleHCR & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleVY  & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleVC  & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);

    // horizontal luminance scale factor
    if (ptScale->ulScaleHY & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HY_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HY_ENABLE, ENABLE);
        //program scale factor and phase
        REG_SET_SLICE(ptMrvReg->mrsz_scale_hy, MRV_MRSZ_SCALE_HY, (UINT32)ptScale->ulScaleHY);
        REG_SET_SLICE(ptMrvReg->mrsz_phase_hy, MRV_MRSZ_PHASE_HY, (UINT32)ptScale->usPhaseHY);
        
        if (ptScale->ulScaleHY & RSZ_UPSCALE_ENABLE)
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // enable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HY_UP, MRV_MRSZ_SCALE_HY_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HY_UP, MRV_MRSZ_SCALE_HY_UP_DOWNSCALE);
        }
#else // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            UNUSED_PARAM(iUpscaling);
            ASSERT("MrvResSetMainResize()" == "Upscaling not supported!");
        }
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    }

    // horizontal chrominance scale factors
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    ASSERT((ptScale->ulScaleHCB & RSZ_FLAGS_MASK) == (ptScale->ulScaleHCR & RSZ_FLAGS_MASK));
#else // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    ASSERT( ptScale->ulScaleHCB                   ==  ptScale->ulScaleHCR                  );
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)

    if (ptScale->ulScaleHCB & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HC_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HC_ENABLE, ENABLE);
        //program scale factor and phase
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(ptMrvReg->mrsz_scale_hcb, MRV_MRSZ_SCALE_HCB, (UINT32)ptScale->ulScaleHCB);
        REG_SET_SLICE(ptMrvReg->mrsz_scale_hcr, MRV_MRSZ_SCALE_HCR, (UINT32)ptScale->ulScaleHCR);
#else // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(ptMrvReg->mrsz_scale_hc,  MRV_MRSZ_SCALE_HC, (UINT32)ptScale->ulScaleHCB);
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(ptMrvReg->mrsz_phase_hc,  MRV_MRSZ_PHASE_HC,  (UINT32)ptScale->usPhaseHC);
        
        if (ptScale->ulScaleHCB & RSZ_UPSCALE_ENABLE)
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // enable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HC_UP, MRV_MRSZ_SCALE_HC_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HC_UP, MRV_MRSZ_SCALE_HC_UP_DOWNSCALE);
        }
#else // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            ASSERT("MrvResSetMainResize()" == "Upscaling not supported!");
        }
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    }

    // vertical luminance scale factor
    if (ptScale->ulScaleVY & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VY_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VY_ENABLE, ENABLE);
        //program scale factor and phase
        REG_SET_SLICE(ptMrvReg->mrsz_scale_vy, MRV_MRSZ_SCALE_VY, (UINT32)ptScale->ulScaleVY);
        REG_SET_SLICE(ptMrvReg->mrsz_phase_vy, MRV_MRSZ_PHASE_VY, (UINT32)ptScale->usPhaseVY);
        
        if (ptScale->ulScaleVY & RSZ_UPSCALE_ENABLE)
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // enable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VY_UP, MRV_MRSZ_SCALE_VY_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VY_UP, MRV_MRSZ_SCALE_VY_UP_DOWNSCALE);
        }
#else // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            ASSERT("MrvResSetMainResize()" == "Upscaling not supported!");
        }
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    }
   
    // vertical chrominance scale factor
    if (ptScale->ulScaleVC & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VC_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VC_ENABLE, ENABLE);
        //program scale factor and phase
        REG_SET_SLICE(ptMrvReg->mrsz_scale_vc, MRV_MRSZ_SCALE_VC, (UINT32)ptScale->ulScaleVC);
        REG_SET_SLICE(ptMrvReg->mrsz_phase_vc, MRV_MRSZ_PHASE_VC, (UINT32)ptScale->usPhaseVC);
        
        if (ptScale->ulScaleVC & RSZ_UPSCALE_ENABLE)
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // enable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VC_UP, MRV_MRSZ_SCALE_VC_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VC_UP, MRV_MRSZ_SCALE_VC_UP_DOWNSCALE);
        }
#else // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            ASSERT("MrvResSetMainResize()" == "Upscaling not supported!");
        }
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    }
   
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    // apply upscaling lookup table
    if (ptMrvRszLut)
    {  
        UINT32 i;
        for (i = 0; i <= MRV_MRSZ_SCALE_LUT_ADDR_MASK; i++)
        {
            REG_SET_SLICE(ptMrvReg->mrsz_scale_lut_addr, MRV_MRSZ_SCALE_LUT_ADDR, i);
            REG_SET_SLICE(ptMrvReg->mrsz_scale_lut, MRV_MRSZ_SCALE_LUT, ptMrvRszLut->ucMrvRszLut[i]);
        }
    }
    else if (iUpscaling)
    {
        ASSERT("MrvResSetMainResize()" == "Upscaling requires lookup table!");
    }
#else //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM1(ptMrvRszLut);
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)

    // handle immediate update flag and write mrsz_ctrl
	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        //frame synchronous update of shadow registers
        REG_WRITE(ptMrvReg->mrsz_ctrl, mrsz_ctrl);
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        //immediate update of shadow registers
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_CFG_UPD, ON);
#else // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        if(mrsz_ctrl != 0)
        {
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_CFG_UPD_C, ON);
            REG_SET_SLICE(mrsz_ctrl, MRV_MRSZ_CFG_UPD_Y, ON);
        }
#endif //#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_WRITE(ptMrvReg->mrsz_ctrl, mrsz_ctrl);
		break;
	case eMrvCfgUpdateLater:
	default:
		// no update from within this function
        REG_WRITE(ptMrvReg->mrsz_ctrl, mrsz_ctrl);
		break;
	}
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvResSetSelfResize \n
 *  \PARAMETERS  pointer to scaler values \n
 *               enum for immediate, frame synchronous, or later (external) update \n
 *  \RETURNVALUE none \n
 *
 *  \NOTES       The usScaleXxx fields of the struct pointed to by the 
 *               ptScale parameter contain both the scale factor value and
 *               the combination of the RSZ_SCALER_BYPASS and the 
 *               RSZ_UPSCALE_ENABLE flag.  \n
 *            
 *  \DESCRIPTION writes the scaler values to the appropriate Marvin
 *               registers \n
 */
/*****************************************************************************/
void MrvResSetSelfResize(const tsMrvScale *ptScale, teMrvConfUpdateTime eUpdateTime, const tsMrvRszLut *ptMrvRszLut)
{
#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 srsz_ctrl  = REG_READ(ptMrvReg->srsz_ctrl);
    BOOL   iUpscaling = FALSE;

    ASSERT((RSZ_FLAGS_MASK & MRV_RSZ_SCALE_MASK) == 0); // flags must be "outside" scaler value
    ASSERT((ptScale->ulScaleHY  & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleHCB & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleHCR & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleVY  & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);
    ASSERT((ptScale->ulScaleVC  & ~RSZ_FLAGS_MASK) <= MRV_RSZ_SCALE_MAX);

    // horizontal luminance scale factor
    if (ptScale->ulScaleHY & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HY_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HY_ENABLE, ENABLE);
        //program scale factor and phase
        REG_SET_SLICE(ptMrvReg->srsz_scale_hy, MRV_SRSZ_SCALE_HY, (UINT32)ptScale->ulScaleHY);
        REG_SET_SLICE(ptMrvReg->srsz_phase_hy, MRV_SRSZ_PHASE_HY, (UINT32)ptScale->usPhaseHY);
        
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        if (ptScale->ulScaleHY & RSZ_UPSCALE_ENABLE)
        { // enable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HY_UP, MRV_SRSZ_SCALE_HY_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HY_UP, MRV_SRSZ_SCALE_HY_UP_DOWNSCALE);
        }
  #else // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            ASSERT("MrvResSetSelfResize()" == "Upscaling not supported!");
        }
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }

    // horizontal chrominance scale factors
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    ASSERT((ptScale->ulScaleHCB & RSZ_FLAGS_MASK) == (ptScale->ulScaleHCR & RSZ_FLAGS_MASK));
  #else // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    ASSERT( ptScale->ulScaleHCB                   ==  ptScale->ulScaleHCR                  );
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)

    if (ptScale->ulScaleHCB & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HC_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HC_ENABLE, ENABLE);
        //program scale factor and phase
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(ptMrvReg->srsz_scale_hcb, MRV_SRSZ_SCALE_HCB, (UINT32)ptScale->ulScaleHCB);
        REG_SET_SLICE(ptMrvReg->srsz_scale_hcr, MRV_SRSZ_SCALE_HCB, (UINT32)ptScale->ulScaleHCR);
  #else // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(ptMrvReg->srsz_scale_hc,  MRV_SRSZ_SCALE_HC, (UINT32)ptScale->ulScaleHCB);
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(ptMrvReg->srsz_phase_hc,  MRV_SRSZ_PHASE_HC,  (UINT32)ptScale->usPhaseHC);
        
        if (ptScale->ulScaleHCB & RSZ_UPSCALE_ENABLE)
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        { // enable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HC_UP, MRV_SRSZ_SCALE_HC_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HC_UP, MRV_SRSZ_SCALE_HC_UP_DOWNSCALE);
        }
  #else // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            UNUSED_PARAM(iUpscaling);
            ASSERT("MrvResSetSelfResize()" == "Upscaling not supported!");
        }
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }

    // vertical luminance scale factor
    if (ptScale->ulScaleVY & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VY_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VY_ENABLE, ENABLE);
        //program scale factor and phase
        REG_SET_SLICE(ptMrvReg->srsz_scale_vy, MRV_SRSZ_SCALE_VY, (UINT32)ptScale->ulScaleVY);
        REG_SET_SLICE(ptMrvReg->srsz_phase_vy, MRV_SRSZ_PHASE_VY, (UINT32)ptScale->usPhaseVY);
        
        if (ptScale->ulScaleVY & RSZ_UPSCALE_ENABLE)
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        { // enable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VY_UP, MRV_SRSZ_SCALE_VY_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VY_UP, MRV_SRSZ_SCALE_VY_UP_DOWNSCALE);
        }
  #else // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            ASSERT("MrvResSetSelfResize()" == "Upscaling not supported!");
        }
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
   
    // vertical chrominance scale factor
    if (ptScale->ulScaleVC & RSZ_SCALER_BYPASS)
    { // disable (bypass) scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VC_ENABLE, DISABLE);
    }
    else
    { // enable scaler
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VC_ENABLE, ENABLE);
        //program scale factor and phase
        REG_SET_SLICE(ptMrvReg->srsz_scale_vc, MRV_SRSZ_SCALE_VC, (UINT32)ptScale->ulScaleVC);
        REG_SET_SLICE(ptMrvReg->srsz_phase_vc, MRV_SRSZ_PHASE_VC, (UINT32)ptScale->usPhaseVC);
        
        if (ptScale->ulScaleVC & RSZ_UPSCALE_ENABLE)
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        { // enable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VC_UP, MRV_SRSZ_SCALE_VC_UP_UPSCALE);
            iUpscaling = TRUE; // scaler and upscaling enabled
        }
        else
        { // disable upscaling mode
            REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VC_UP, MRV_SRSZ_SCALE_VC_UP_DOWNSCALE);
        }
  #else // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        { // upscaling mode -> not supported
            ASSERT("MrvResSetSelfResize()" == "Upscaling not supported!");
        }
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
   
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    // apply upscaling lookup table
    if (ptMrvRszLut)
    {  
        UINT32 i;
        for (i = 0; i <= MRV_SRSZ_SCALE_LUT_ADDR_MASK; i++)
        {
            REG_SET_SLICE(ptMrvReg->srsz_scale_lut_addr, MRV_SRSZ_SCALE_LUT_ADDR, i);
            REG_SET_SLICE(ptMrvReg->srsz_scale_lut, MRV_SRSZ_SCALE_LUT, ptMrvRszLut->ucMrvRszLut[i]);
        }
    }
    else if (iUpscaling)
    {
        ASSERT("MrvResSetSelfResize()" == "Upscaling requires lookup table!");
    }
  #else //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(ptMrvRszLut);
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)

    // handle immediate update flag and write mrsz_ctrl
	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        //frame synchronous update of shadow registers
        REG_WRITE(ptMrvReg->srsz_ctrl, srsz_ctrl);
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        //immediate update of shadow registers
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_CFG_UPD, ON);
  #else // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_CFG_UPD_C, ON);
        REG_SET_SLICE(srsz_ctrl, MRV_SRSZ_CFG_UPD_Y, ON);
  #endif //#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        REG_WRITE(ptMrvReg->srsz_ctrl, srsz_ctrl);
		break;
	case eMrvCfgUpdateLater:
	default:
		// no update from within this function
        REG_WRITE(ptMrvReg->srsz_ctrl, srsz_ctrl);
		break;
	}
#else //#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(ptScale);
    UNUSED_PARAM(eUpdateTime);
    UNUSED_PARAM1(ptMrvRszLut);
#endif //#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvResGetMainResize \n
 *  \PARAMETERS  pointer to struct to receive scaler values \n
 *  \RETURNVALUE none \n
 *
 *  \DESCRIPTION retrieves the main picture scaler values from the
 *               appropriate Marvin registers \n
 */
/*****************************************************************************/
void MrvResGetMainResize(tsMrvScale *ptScale)
{
    // FIXME: phase registers ???

    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mrsz_ctrl = REG_READ(ptMrvReg->mrsz_ctrl);

    memset(ptScale, 0, sizeof(tsMrvScale));

    if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HY_ENABLE))
    {
        ptScale->ulScaleHY = REG_GET_SLICE(ptMrvReg->mrsz_scale_hy, MRV_MRSZ_SCALE_HY);

#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HY_UP) == MRV_MRSZ_SCALE_HY_UP_UPSCALE)
        {
            ptScale->ulScaleHY |= RSZ_UPSCALE_ENABLE;
        }
#endif // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleHY = RSZ_SCALER_BYPASS;
    }

    if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HC_ENABLE))
    {
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        ptScale->ulScaleHCB = REG_GET_SLICE(ptMrvReg->mrsz_scale_hcb, MRV_MRSZ_SCALE_HCB);
        ptScale->ulScaleHCR = REG_GET_SLICE(ptMrvReg->mrsz_scale_hcr, MRV_MRSZ_SCALE_HCR);

        if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_HC_UP) == MRV_MRSZ_SCALE_HC_UP_UPSCALE)
        {
            ptScale->ulScaleHCB |= RSZ_UPSCALE_ENABLE;
            ptScale->ulScaleHCR |= RSZ_UPSCALE_ENABLE;
        }
#else  // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        ptScale->ulScaleHCB = REG_GET_SLICE(ptMrvReg->mrsz_scale_hc,  MRV_MRSZ_SCALE_HC);
        ptScale->ulScaleHCR = ptScale->ulScaleHCB;
#endif // #if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleHCB = RSZ_SCALER_BYPASS;
        ptScale->ulScaleHCR = RSZ_SCALER_BYPASS;
    }

    if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VY_ENABLE))
    {
        ptScale->ulScaleVY = REG_GET_SLICE(ptMrvReg->mrsz_scale_vy, MRV_MRSZ_SCALE_VY);

#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VY_UP) == MRV_MRSZ_SCALE_VY_UP_UPSCALE)
        {
            ptScale->ulScaleVY |= RSZ_UPSCALE_ENABLE;
        }
#endif // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleVY = RSZ_SCALER_BYPASS;
    }

    if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VC_ENABLE))
    {
        ptScale->ulScaleVC = REG_GET_SLICE(ptMrvReg->mrsz_scale_vc, MRV_MRSZ_SCALE_VC);

#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
        if (REG_GET_SLICE(mrsz_ctrl, MRV_MRSZ_SCALE_VC_UP) == MRV_MRSZ_SCALE_VC_UP_UPSCALE)
        {
            ptScale->ulScaleVC |= RSZ_UPSCALE_ENABLE;
        }
#endif // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleVC = RSZ_SCALER_BYPASS;
    }
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvResGetSelfResize \n
 *  \PARAMETERS  pointer to struct to receive scaler values \n
 *  \RETURNVALUE none \n
 *
 *  \DESCRIPTION retrieves the self picture scaler values from the
 *               appropriate Marvin registers \n
 */
/*****************************************************************************/
void MrvResGetSelfResize(tsMrvScale *ptScale)
{
    // FIXME: phase registers ???

#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 srsz_ctrl = REG_READ(ptMrvReg->srsz_ctrl);

    memset(ptScale, 0, sizeof(tsMrvScale));

    if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HY_ENABLE))
    {
        ptScale->ulScaleHY = REG_GET_SLICE(ptMrvReg->srsz_scale_hy, MRV_SRSZ_SCALE_HY);

  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HY_UP) == MRV_SRSZ_SCALE_HY_UP_UPSCALE)
        {
            ptScale->ulScaleHY |= RSZ_UPSCALE_ENABLE;
        }
  #endif // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleHY = RSZ_SCALER_BYPASS;
    }

    if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HC_ENABLE))
    {
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        ptScale->ulScaleHCB = REG_GET_SLICE(ptMrvReg->srsz_scale_hcb, MRV_SRSZ_SCALE_HCB);
        ptScale->ulScaleHCR = REG_GET_SLICE(ptMrvReg->srsz_scale_hcr, MRV_SRSZ_SCALE_HCR);

        if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_HC_UP) == MRV_SRSZ_SCALE_HC_UP_UPSCALE)
        {
            ptScale->ulScaleHCB |= RSZ_UPSCALE_ENABLE;
            ptScale->ulScaleHCR |= RSZ_UPSCALE_ENABLE;
        }
  #else  // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        ptScale->ulScaleHCB = REG_GET_SLICE(ptMrvReg->srsz_scale_hc,  MRV_SRSZ_SCALE_HC);
        ptScale->ulScaleHCR = ptScale->ulScaleHCB;
  #endif // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleHCB = RSZ_SCALER_BYPASS;
        ptScale->ulScaleHCR = RSZ_SCALER_BYPASS;
    }

    if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VY_ENABLE))
    {
        ptScale->ulScaleVY = REG_GET_SLICE(ptMrvReg->srsz_scale_vy, MRV_SRSZ_SCALE_VY);

  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VY_UP) == MRV_SRSZ_SCALE_VY_UP_UPSCALE)
        {
            ptScale->ulScaleVY |= RSZ_UPSCALE_ENABLE;
        }
  #endif // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleVY = RSZ_SCALER_BYPASS;
    }

    if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VC_ENABLE))
    {
        ptScale->ulScaleVC = REG_GET_SLICE(ptMrvReg->srsz_scale_vc, MRV_SRSZ_SCALE_VC);

  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
        if (REG_GET_SLICE(srsz_ctrl, MRV_SRSZ_SCALE_VC_UP) == MRV_SRSZ_SCALE_VC_UP_UPSCALE)
        {
            ptScale->ulScaleVC |= RSZ_UPSCALE_ENABLE;
        }
  #endif // #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
    }
    else
    {
        ptScale->ulScaleVC = RSZ_SCALER_BYPASS;
    }
#else  // #if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
    memset(ptScale, 0, sizeof(tsMrvScale));
#endif // #if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
}
