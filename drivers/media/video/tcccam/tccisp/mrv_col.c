/*****************************************************************************/
/*!
 *  \file        mrv_col.c \n
 *  \version     1.0 \n
 *  \author      Friebe \n
 *  \brief       color processing interface \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_col.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

#include "mrv_priv.h"


/*!********************************************************************
 *
 *  \FUNCTION    MrvColSetColorProcessing \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  pointer to Marvin color settings (NULL=disable stage) \n
 *
 *  \NOTES       if a NULL pointer is handed over the color registers
 *               are not changed but the color processing stage is
 *               disabled completely \n
 *
 *  \DESCRIPTION writes the color values for contrast, brightness,
 *               saturation and hue into the appropriate Marvin
 *               registers \n
 *
 *********************************************************************/
void MrvColSetColorProcessing( const tsMrvColorSettings *ptMrvCol )
{
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

   if( ptMrvCol == NULL )
   {
      ptMrvReg->c_proc_ctrl = 0; //disable color processing (bypass)
   }
   else
   {
      ptMrvReg->c_proc_contrast   = ptMrvCol->ucContrast;
      ptMrvReg->c_proc_brightness = ptMrvCol->ucBrightness;
      ptMrvReg->c_proc_saturation = ptMrvCol->ucSaturation;
      ptMrvReg->c_proc_hue        = ptMrvCol->ucHue;

#if( MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST )
      //modify color processing registers    
      if (ptMrvCol->ulFlags & MRV_CPROC_C_OUT_RANGE)
      {
        ptMrvReg->c_proc_ctrl = ptMrvReg->c_proc_ctrl | MRV_CPROC_C_OUT_RANGE;
      }
    
      if (ptMrvCol->ulFlags & MRV_CPROC_Y_IN_RANGE)
      {
        ptMrvReg->c_proc_ctrl = ptMrvReg->c_proc_ctrl | MRV_CPROC_Y_IN_RANGE;
      }
    
      if (ptMrvCol->ulFlags & MRV_CPROC_Y_OUT_RANGE)
      {
        ptMrvReg->c_proc_ctrl = ptMrvReg->c_proc_ctrl | MRV_CPROC_Y_OUT_RANGE;
      }  
    
      if (ptMrvCol->ulFlags & MRV_CPROC_ENABLE)
      {
        ptMrvReg->c_proc_ctrl = ptMrvReg->c_proc_ctrl | MRV_CPROC_ENABLE;             
      }
#else
      // switch color processing on
      ptMrvReg->c_proc_ctrl = ptMrvReg->c_proc_ctrl | MRV_CPROC_ENABLE;             
#endif //#if( MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST )
   }
}


/*!********************************************************************
 *
 *  \FUNCTION    MrvColGetColorProcessing \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  pointer to Marvin color settings \n
 *  \DESCRIPTION get the color values out of the appropriate Marvin
 *               registers \n
 *
 *********************************************************************/
void MrvColGetColorProcessing( tsMrvColorSettings *ptMrvCol )
{
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
   
   ptMrvCol->ucContrast   = (UINT8)ptMrvReg->c_proc_contrast;
   ptMrvCol->ucBrightness = (UINT8)ptMrvReg->c_proc_brightness;
   ptMrvCol->ucSaturation = (UINT8)ptMrvReg->c_proc_saturation;
   ptMrvCol->ucHue        = (UINT8)ptMrvReg->c_proc_hue;
   ptMrvCol->ulFlags      = (UINT32)ptMrvReg->c_proc_ctrl;
}
