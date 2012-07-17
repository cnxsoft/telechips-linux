/*****************************************************************************/
/*!
 *  \file        mrv_ie.c \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       image effects interface \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_ie.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

#if ((MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V1) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V2) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) )
    #include "reg_access.h"
    #include "mrv_priv.h"
#endif // #if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)


// //////////////////////////////////////////////////////////////////////
// internal (static) helpers
// only needed if the image effects feature is compiled in. 
// //////////////////////////////////////////////////////////////////////
#if ((MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V1) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V2) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) )

/*!********************************************************************
 *
 *  \FUNCTION    MrvIeTint_Cx2RegVal (static) \n
 *  \RETURNVALUE Chrominance component value in register representation \n
 *  \PARAMETERS  Chrominance component value in usual representation
 *  \DESCRIPTION Translates a chrominance component value from usual
 *               representation (range 16..240, 128=neutral grey) 
 *               to the one used by the ie_tint register \n
 *  \NOTE  The value is returned as 32 bit unsigned to support shift
 *         operation without explicit cast. \n
 *         The translation formular implemented here is taken from 
 *         the image effects functional specification document, 
 *         Doc-ID 30-001-481.130, revision 1.1 from november, 21st. 2005
 *
 *********************************************************************/
static UINT32 MrvIeTint_Cx2RegVal(UINT8 ucCx)
{
   INT32 lTemp;
   UINT32 ulRegVal;
   
   // apply scaling as specified in the image effects functional specification
   // from november, 21st. 2005, Doc-ID 30-001-481.130, revision 1.1
   lTemp = 128 - (INT32)ucCx;
   lTemp = ((lTemp * 64) / 110);
   // convert from two's complement to sign/value
   if (lTemp < 0) {
      ulRegVal = 0x80;
      lTemp *= (-1);
   } else {
      ulRegVal = 0;
   }
   // saturate at 7 bits
   if (lTemp > 0x7F) {
      lTemp = 0x7F;
   }
   // combine sign and value to build the regiter value
   ulRegVal |= (UINT32)lTemp;
   
   return ulRegVal;   
}

/*!********************************************************************
 *
 *  \FUNCTION    MrvIeTint_RegVal2Cx (static) \n
 *  \RETURNVALUE Chrominance component value in usual representation \n
 *  \PARAMETERS  Chrominance component value in register representation \n
 *  \DESCRIPTION Translates a chrominance component value from its 
 *               register representation (used in the ie_tint register)
 *               in the usual representation in range 16..240 with 
 *               128=neutral grey. \n
 *  \NOTE  The translation formular implemented here is taken from 
 *         the image effects functional specification document, 
 *         Doc-ID 30-001-481.130, revision 1.1 from november, 21st. 2005
 *
 *********************************************************************/
static UINT8 MrvIeTint_RegVal2Cx(UINT8 ucRegVal)
{
   INT32 lTemp;
   
   // convert from sign/value to two's complement
   if (ucRegVal & 0x80) {
      lTemp = -(ucRegVal & 0x7F);
   } else {
      lTemp = ucRegVal;
   }
   // apply scaling
   lTemp = ((lTemp * 110) / 64);
   lTemp = 128 - lTemp;
   // saturate at 16 resp. 240
   if (lTemp > 240) {
      lTemp = 240;
   }
   if (lTemp < 16) {
      lTemp = 16;
   }
   
   return (UINT8)lTemp;   
}


/*!********************************************************************
 *
 *  \FUNCTION    MrvIeMx_RegVal2Dec (static) \n
 *  \RETURNVALUE Matrix coefficient value in usual (decimal) representation \n
 *  \PARAMETERS  Matrix coefficient value in register representation \n
 *  \DESCRIPTION Translates a matrix coefficient value from its 4 bit 
 *               register representation (used in the ie_mat_X registers)
 *               in the usual decimal representation, which can be 
 *               one of (-8,-4,-2,-1, 0, 1, 2, 4, 8)
 *  \NOTE  The translation formular implemented here is taken from 
 *         the image effects functional specification document, 
 *         Doc-ID 30-001-481.130, revision 1.1 from november, 21st. 2005
 *
 *********************************************************************/
static INT8 MrvIeMx_RegVal2Dec(UINT8 ucRegVal)
{
   if (ucRegVal & 0x08)
   {  // the coefficient is used
      // only lower 3 bits are of interest
      switch (ucRegVal & 0x07)
      {
         case 0: return 1;
         case 1: return 2;
         case 2: return 4;
         case 3: return 8;
         case 4: return (-1);
         case 5: return (-2);
         case 6: return (-4);
         case 7: return (-8);
         default: break; // thank Lint for this
      }
   }
   // coefficient is unused   
   return 0;
}


/*!********************************************************************
 *
 *  \FUNCTION    MrvIeMx_Dec2RegVal (static) \n
 *  \RETURNVALUE Matrix coefficient value in register representation \n
 *  \PARAMETERS  Matrix coefficient value in usual (decimal) representation \n
 *  \DESCRIPTION Translates usual (decimal) matrix coefficient into the
 *               4 bit  register representation (used in the ie_mat_X registers).
 *               for unsupported decimal numbers, a supported replacement is
 *               selected automatically. \n
 *  \NOTE  The value is returned as 32 bit unsigned to support shift
 *         operation without explicit cast. \n
 *         The translation formular implemented here is taken from 
 *         the image effects functional specification document, 
 *         Doc-ID 30-001-481.130, revision 1.1 from november, 21st. 2005 \n
 *
 *********************************************************************/
static UINT32 MrvIeMx_Dec2RegVal(INT8 cDec)
{
   if (cDec <= (-6)) {
      return 0x0f;  // equivlent to -8
   }
   else if (cDec <= (-3)) {
      return 0x0e;  // equivlent to -4
   }
   else if (cDec == (-2)) {
      return 0x0d;  // equivlent to -2
   }
   else if (cDec == (-1)) {
      return 0x0c;  // equivlent to -1
   }
   else if (cDec == 0) {
      return 0x00;  // equivlent to 0 (entry not used)
   }
   else if (cDec == 1) {
      return 0x08;  // equivlent to 1
   }
   else if (cDec == 2) {
      return 0x09;  // equivlent to 2
   }
   else if (cDec < 6) {
      return 0x0a;  // equivlent to 4
   }
   else {
      return 0x0b;  // equivlent to 8
   }
}

#endif // #if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)



// //////////////////////////////////////////////////////////////////////
// interface routines
// //////////////////////////////////////////////////////////////////////

/*!********************************************************************
 *
 *  \FUNCTION    MrvIeSetConfig \n
 *  \RETURNVALUE RET_SUCCESS in case the configuration has been applied \n
 *               RET_NOTSUPP in case the marvin derivative does not support image effects \n
 *               RET_OUTOFRANGE in case the given parameters are out of range or unknown.
 *                   Note that the configuration has not been changed in this case\n
 *  \PARAMETERS  pointer to image effects configuration structure 
 *               or NULL to disable the image effects submodule \n
 *
 *  \DESCRIPTION translates the values of the given configuration
 *               structure into register settings for the image effects
 *               submodule and loads the registers.  \n
 *
 *********************************************************************/
RESULT MrvIeSetConfig(const tsMrvIeConfig *ptIeConfig)
{
#if ((MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V1) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V2) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) )
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
   
   if (!ptIeConfig)
   {  // just disable the module, i.e. put it in bypass mode
      REG_SET_SLICE(ptMrvReg->img_eff_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_BYPASS);
   }
   else
   {  // apply the given settings
      UINT32 ul_ie_ctrl = REG_READ(ptMrvReg->img_eff_ctrl);
      UINT32 ul_ie_csel = REG_READ(ptMrvReg->img_eff_color_sel);
      UINT32 ul_ie_tint = REG_READ(ptMrvReg->img_eff_tint);
      UINT32 ul_ie_mat1 = REG_READ(ptMrvReg->img_eff_mat_1);
      UINT32 ul_ie_mat2 = REG_READ(ptMrvReg->img_eff_mat_2);
      UINT32 ul_ie_mat3 = REG_READ(ptMrvReg->img_eff_mat_3);
      UINT32 ul_ie_mat4 = REG_READ(ptMrvReg->img_eff_mat_4);
      UINT32 ul_ie_mat5 = REG_READ(ptMrvReg->img_eff_mat_5);
#if (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
      UINT32 ul_ie_sharpen = REG_READ(ptMrvReg->img_eff_sharpen);
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 

      // overall operation mode
      switch (ptIeConfig->eMode)
      {
      case eMrvIeMode_Off:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_BYPASS);
         break;
      case eMrvIeMode_Grayscale:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_EFFECT_MODE, MRV_IMGEFF_EFFECT_MODE_GRAY);
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_PROCESS);
         break;
      case eMrvIeMode_Negative:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_EFFECT_MODE, MRV_IMGEFF_EFFECT_MODE_NEGATIVE);
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_PROCESS);
         break;
      case eMrvIeMode_Sepia:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_EFFECT_MODE, MRV_IMGEFF_EFFECT_MODE_SEPIA);
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_PROCESS);
         break;
      case eMrvIeMode_ColorSel:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_EFFECT_MODE, MRV_IMGEFF_EFFECT_MODE_COLOR_SEL);
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_PROCESS);
         break;
      case eMrvIeMode_Emboss:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_EFFECT_MODE, MRV_IMGEFF_EFFECT_MODE_EMBOSS);
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_PROCESS);
         break;
      case eMrvIeMode_Sketch:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_EFFECT_MODE, MRV_IMGEFF_EFFECT_MODE_SKETCH);
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_PROCESS);
         break;
#if (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
      case eMrvIeMode_Sharpen:
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_EFFECT_MODE, MRV_IMGEFF_EFFECT_MODE_SHARPEN);
         REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_BYPASS_MODE, MRV_IMGEFF_BYPASS_MODE_PROCESS);
         break;
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
      default:
         return RET_OUTOFRANGE;
      }
      
#if ((MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V2) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) )
      //YCbCr full range or BT601
      REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_FULL_RANGE, ptIeConfig->bFullRange?ON:OFF);
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V2) 

      //use next frame sync update
      REG_SET_SLICE(ul_ie_ctrl, MRV_IMGEFF_CFG_UPD, ON);

      // color selection settings
      REG_SET_SLICE(ul_ie_csel, MRV_IMGEFF_COLOR_THRESHOLD, (UINT32)(ptIeConfig->ucColorThres));
      REG_SET_SLICE(ul_ie_csel, MRV_IMGEFF_COLOR_SELECTION, (UINT32)(ptIeConfig->ucColorSel));

      // tint color settings
      REG_SET_SLICE(ul_ie_tint, MRV_IMGEFF_INCR_CB, MrvIeTint_Cx2RegVal(ptIeConfig->ucTintCb));
      REG_SET_SLICE(ul_ie_tint, MRV_IMGEFF_INCR_CR, MrvIeTint_Cx2RegVal(ptIeConfig->ucTintCr));
      
      // matrix coefficients
      REG_SET_SLICE(ul_ie_mat1, MRV_IMGEFF_EMB_COEF_11_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_11));
      REG_SET_SLICE(ul_ie_mat1, MRV_IMGEFF_EMB_COEF_12_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_12));
      REG_SET_SLICE(ul_ie_mat1, MRV_IMGEFF_EMB_COEF_13_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_13));
      REG_SET_SLICE(ul_ie_mat1, MRV_IMGEFF_EMB_COEF_21_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_21));
      REG_SET_SLICE(ul_ie_mat2, MRV_IMGEFF_EMB_COEF_22_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_22));
      REG_SET_SLICE(ul_ie_mat2, MRV_IMGEFF_EMB_COEF_23_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_23));
      REG_SET_SLICE(ul_ie_mat2, MRV_IMGEFF_EMB_COEF_31_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_31));
      REG_SET_SLICE(ul_ie_mat2, MRV_IMGEFF_EMB_COEF_32_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_32));
      REG_SET_SLICE(ul_ie_mat3, MRV_IMGEFF_EMB_COEF_33_4,  MrvIeMx_Dec2RegVal(ptIeConfig->tMatEmboss.cCoeff_33));
      REG_SET_SLICE(ul_ie_mat3, MRV_IMGEFF_SKET_COEF_11_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_11));
      REG_SET_SLICE(ul_ie_mat3, MRV_IMGEFF_SKET_COEF_12_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_12));
      REG_SET_SLICE(ul_ie_mat3, MRV_IMGEFF_SKET_COEF_13_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_13));
      REG_SET_SLICE(ul_ie_mat4, MRV_IMGEFF_SKET_COEF_21_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_21));
      REG_SET_SLICE(ul_ie_mat4, MRV_IMGEFF_SKET_COEF_22_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_22));
      REG_SET_SLICE(ul_ie_mat4, MRV_IMGEFF_SKET_COEF_23_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_23));
      REG_SET_SLICE(ul_ie_mat4, MRV_IMGEFF_SKET_COEF_31_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_31));
      REG_SET_SLICE(ul_ie_mat5, MRV_IMGEFF_SKET_COEF_32_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_32));
      REG_SET_SLICE(ul_ie_mat5, MRV_IMGEFF_SKET_COEF_33_4, MrvIeMx_Dec2RegVal(ptIeConfig->tMatSketch.cCoeff_33));

#if (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
      //sharpening filter
      REG_SET_SLICE(ul_ie_sharpen, MRV_IMGEFF_CORING_THR, ptIeConfig->ucCoringThr);
      REG_SET_SLICE(ul_ie_sharpen, MRV_IMGEFF_SHARP_FACTOR, ptIeConfig->ucSharp);
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
      // write changed values back to registers
      REG_WRITE(ptMrvReg->img_eff_ctrl, ul_ie_ctrl);
      REG_WRITE(ptMrvReg->img_eff_color_sel, ul_ie_csel);
      REG_WRITE(ptMrvReg->img_eff_tint, ul_ie_tint);
      REG_WRITE(ptMrvReg->img_eff_mat_1, ul_ie_mat1);
      REG_WRITE(ptMrvReg->img_eff_mat_2, ul_ie_mat2);
      REG_WRITE(ptMrvReg->img_eff_mat_3, ul_ie_mat3);
      REG_WRITE(ptMrvReg->img_eff_mat_4, ul_ie_mat4);
      REG_WRITE(ptMrvReg->img_eff_mat_5, ul_ie_mat5);
#if (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
      REG_WRITE(ptMrvReg->img_eff_sharpen, ul_ie_sharpen);
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
                           

      // frame synchronous update of shadow registers
      REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
   }

   return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)
   UNUSED_PARAM1(ptIeConfig);
   return RET_NOTSUPP;      
#endif // #if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)
}


/*!********************************************************************
 *
 *  \FUNCTION    MrvIeGetConfig \n
 *  \RETURNVALUE RET_SUCCESS in case the configuration has been read \n
 *               RET_NOTSUPP in case the marvin derivative does not support image effects \n
 *               RET_NULL_POINTER in case the given parameter is a NULL pointer \n
 *  \PARAMETERS  pointer to image effects configuration structure 
 *               to receive the current settings \n
 *
 *  \DESCRIPTION reads the current configuration of the image effects 
 *               module and fills in the given configuration structure
 *               accordingly. \n
 *  \NOTE By passing a NULL pointer, this routine can also be used to 
 *        check if the currently used marvin derivative supports 
 *        image effects (return code would be RET_NULL_POINTER)
 *        or not (return code would be RET_NOTSUPP)
 *
 *********************************************************************/
RESULT MrvIeGetConfig(tsMrvIeConfig *ptIeConfig)
{
#if ((MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V1) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V2) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) )
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
   UINT32 ulTemp;
   
   if (!ptIeConfig)
   {
      return RET_NULL_POINTER;
   }
   
   // read current mode
   ulTemp = REG_READ(ptMrvReg->img_eff_ctrl);
   ptIeConfig->eMode = eMrvIeMode_Off; // assume off state by default
   if (REG_GET_SLICE(ulTemp, MRV_IMGEFF_BYPASS_MODE) == MRV_IMGEFF_BYPASS_MODE_BYPASS)
   {
      ptIeConfig->eMode = eMrvIeMode_Off; // assume off state by default
   }
   else
   {
      switch (REG_GET_SLICE(ulTemp, MRV_IMGEFF_EFFECT_MODE))
      {
      case MRV_IMGEFF_EFFECT_MODE_GRAY:      ptIeConfig->eMode = eMrvIeMode_Grayscale; break;
      case MRV_IMGEFF_EFFECT_MODE_NEGATIVE:  ptIeConfig->eMode = eMrvIeMode_Negative;  break;
      case MRV_IMGEFF_EFFECT_MODE_SEPIA:     ptIeConfig->eMode = eMrvIeMode_Sepia;     break;
      case MRV_IMGEFF_EFFECT_MODE_COLOR_SEL: ptIeConfig->eMode = eMrvIeMode_ColorSel;  break;
      case MRV_IMGEFF_EFFECT_MODE_EMBOSS:    ptIeConfig->eMode = eMrvIeMode_Emboss;    break;
      case MRV_IMGEFF_EFFECT_MODE_SKETCH:    ptIeConfig->eMode = eMrvIeMode_Sketch;    break;
      case MRV_IMGEFF_EFFECT_MODE_SHARPEN:   ptIeConfig->eMode = eMrvIeMode_Sharpen;    break;
      default:
         ASSERT("MrvIeGetConfig()" == "unknown effect mode read from register");
      }
   }
   
#if ((MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V2) || \
     (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) )
   if (REG_GET_SLICE(ulTemp, MRV_IMGEFF_FULL_RANGE) == MRV_IMGEFF_FULL_RANGE_FULL)
   {
      ptIeConfig->bFullRange = MRV_IMGEFF_FULL_RANGE_FULL; //YCbCr full range
   }
   else
   {
      ptIeConfig->bFullRange = MRV_IMGEFF_FULL_RANGE_BT601; //YCbCr BT601
   }
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V2) 
     
   // color selection settings
   ulTemp = REG_READ(ptMrvReg->img_eff_color_sel);
   ptIeConfig->ucColorThres = (UINT8)REG_GET_SLICE(ulTemp, MRV_IMGEFF_COLOR_THRESHOLD);
   ptIeConfig->ucColorSel   = (UINT8)REG_GET_SLICE(ulTemp, MRV_IMGEFF_COLOR_SELECTION);

   // tint color settings
   ulTemp = REG_READ(ptMrvReg->img_eff_tint);
   ptIeConfig->ucTintCb = MrvIeTint_RegVal2Cx(REG_GET_SLICE(ulTemp, MRV_IMGEFF_INCR_CB));
   ptIeConfig->ucTintCr = MrvIeTint_RegVal2Cx(REG_GET_SLICE(ulTemp, MRV_IMGEFF_INCR_CR));

   // matrix coefficients
   ulTemp = REG_READ(ptMrvReg->img_eff_mat_1);
   ptIeConfig->tMatEmboss.cCoeff_11 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_11_4));
   ptIeConfig->tMatEmboss.cCoeff_12 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_12_4));
   ptIeConfig->tMatEmboss.cCoeff_13 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_13_4));
   ptIeConfig->tMatEmboss.cCoeff_21 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_21_4));

   ulTemp = REG_READ(ptMrvReg->img_eff_mat_2);
   ptIeConfig->tMatEmboss.cCoeff_22 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_22_4));
   ptIeConfig->tMatEmboss.cCoeff_23 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_23_4));
   ptIeConfig->tMatEmboss.cCoeff_31 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_31_4));
   ptIeConfig->tMatEmboss.cCoeff_32 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_32_4));
   
   ulTemp = REG_READ(ptMrvReg->img_eff_mat_3);
   ptIeConfig->tMatEmboss.cCoeff_33 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_EMB_COEF_33_4));
   ptIeConfig->tMatSketch.cCoeff_11 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_11_4));
   ptIeConfig->tMatSketch.cCoeff_12 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_12_4));
   ptIeConfig->tMatSketch.cCoeff_13 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_13_4));
   
   ulTemp = REG_READ(ptMrvReg->img_eff_mat_4);
   ptIeConfig->tMatSketch.cCoeff_21 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_21_4));
   ptIeConfig->tMatSketch.cCoeff_22 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_22_4));
   ptIeConfig->tMatSketch.cCoeff_23 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_23_4));
   ptIeConfig->tMatSketch.cCoeff_31 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_31_4));
   
   ulTemp = REG_READ(ptMrvReg->img_eff_mat_5);
   ptIeConfig->tMatSketch.cCoeff_32 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_32_4));
   ptIeConfig->tMatSketch.cCoeff_33 = MrvIeMx_RegVal2Dec(REG_GET_SLICE(ulTemp, MRV_IMGEFF_SKET_COEF_33_4));

#if (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 
   // sharpen filter
   ulTemp = REG_READ(ptMrvReg->img_eff_sharpen);
   ptIeConfig->ucCoringThr   = (UINT8)REG_GET_SLICE(ulTemp, MRV_IMGEFF_CORING_THR);
   ptIeConfig->ucSharp       = (UINT8)REG_GET_SLICE(ulTemp, MRV_IMGEFF_SHARP_FACTOR);
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS == MARVIN_FEATURE_IE_V3) 

   return RET_SUCCESS;      
#else // #if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)
   UNUSED_PARAM(ptIeConfig);
   return RET_NOTSUPP;      
#endif // #if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)
}


/*!********************************************************************
 *
 *  \FUNCTION    MrvIeTint_RGB2Cx (static) \n
 *  \RETURNVALUE Chrominance component value \n
 *  \PARAMETERS  RGB values in usual representation, Component selector
 *  \DESCRIPTION Translates a RGB value into the chrominance component value 
 *               in usual representation (range 16..240, 128=neutral grey) \n
 *  \NOTE  The translation formular implemented here is taken from 
 *         the image effects functional specification document, 
 *         Doc-ID 30-001-481.130, revision 1.1 from november, 21st. 2005 \n
 *
 *********************************************************************/
UINT8 MrvIeTint_RGB2Cx(UINT8 ucR, UINT8 ucG, UINT8 ucB, BOOL CBSelection)
{
   INT32 lTemp, lTempR, lTempG, lTempB;
   UINT32 ulCx;
   
   if (CBSelection)
   { // Cb calculation
     // apply scaling as specified in the image effects functional specification
     // from november, 21st. 2005, Doc-ID 30-001-481.130, revision 1.1 
     // For Integercalculation multiplied/divided with 16384
     lTempR = 2425 * (INT32)ucR;
     lTempG = 4768 * (INT32)ucG;
     lTempB = 7193 * (INT32)ucB;
     lTemp  = ((2097152 - lTempR) - lTempG) + lTempB;
     ulCx   = ((UINT32)lTemp) >> 14;   // (UINT32) (lTemp / 16384);
   }
   else
   { // Cr calculation
     // apply scaling as specified in the image effects functional specification
     // from november, 21st. 2005, Doc-ID 30-001-481.130, revision 1.1 
     // For Integercalculation multiplied/divided with 16384
     lTempR = 7192 * (INT32)ucR;
     lTempG = 6029 * (INT32)ucG;
     lTempB = 1163 * (INT32)ucB;
     lTemp  = ((2097152 + lTempR) - lTempG) - lTempB;
     ulCx   = ((UINT32)lTemp) >> 14;   // (UINT32) (lTemp / 16384);   
   }
   
   return (UINT8) ulCx;   
}

