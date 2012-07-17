/*****************************************************************************/
/*!
 *  \file        mrv_sls_dp.c \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Implementation for second level support routines
 *               to provide a simplified way to configure the datapath, cropping and
 *               scaler functionality.
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_sls_dp.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

// #if MRV_SUPPORT_SL
#ifdef SL_SUPPORT_PARTLY

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information


// uncomment the following to workaround an MI bug that causes the two bytes
// of a RGB 565 value to be stored in the wrong order
//#define WORKAROUND_RGB565_SEQUENCE_BUG


//! mask for all chroma subsampling settings 
#define MRV_DPD_CSS__MASK  (MRV_DPD_CSS_H__MASK | MRV_DPD_CSS_V__MASK)

	#if (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_12BITS)
	#define SCALER_COFFS_NONCOSITED 0x400
	#elif(MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_14BITS)
	// if subsampling by 2 use 1/4 pixel distance for non cosited 
	#define SCALER_COFFS_NONCOSITED 0x1000 
	#elif(MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_16BITS)
	// if subsampling by 2 use 1/4 pixel distance for non cosited 
	#define SCALER_COFFS_NONCOSITED 0x4000 
	#else
	#error //not defined
	#endif
#define FIXEDPOINT_ONE 0x1000

	//! limitations of main and self scaler
	#if( MARVIN_FEATURE_FRAMESIZE == MARVIN_FEATURE_1M9 )
	   #define MAIN_SCALER_WIDTH_MAX 1280
	#elif( MARVIN_FEATURE_FRAMESIZE == MARVIN_FEATURE_3M1 )
		#if( MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
		   #define MAIN_SCALER_WIDTH_MAX 2048
		#else //( MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
		   #define MAIN_SCALER_WIDTH_MAX 1600
		#endif //( MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
	#elif( MARVIN_FEATURE_FRAMESIZE == MARVIN_FEATURE_5M3 )
	   #define MAIN_SCALER_WIDTH_MAX 2600
	#elif( MARVIN_FEATURE_FRAMESIZE == MARVIN_FEATURE_12M6 )
	   #define MAIN_SCALER_WIDTH_MAX 4096
	#else   // MARVIN_FEATURE_FRAMESIZE
	   #error "unsupported setting of MARVIN_FEATURE_FRAMESIZE"
	#endif  // MARVIN_FEATURE_FRAMESIZE

	#if(MARVIN_FEATURE_SSCALE_RES == MARVIN_FEATURE_SSCALE_RES_720P)
	    #define SELF_SCALER_WIDTH_MAX 1280
	#elif(MARVIN_FEATURE_SSCALE_RES == MARVIN_FEATURE_SSCALE_RES_VGA)
	    #define SELF_SCALER_WIDTH_MAX 640
	#else
	    #define SELF_SCALER_WIDTH_MAX 0 //no resolution defined
	#endif

#define SCALER_MIN 16

	#if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
	   #define SELF_UPSCALE_FACTOR_MAX 5
	#else
	   #define SELF_UPSCALE_FACTOR_MAX 1
	#endif   

	#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
	   #define MAIN_UPSCALE_FACTOR_MAX 5
	#else
	   #define MAIN_UPSCALE_FACTOR_MAX 1
	#endif   


//! upscale lookup table for smooth edges (linear interpolation between pixels)
const tsMrvRszLut s_tMrvRszLut_SmoothLin = // smooth edges
   { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
     0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
     0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
     0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
     0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};

//! upscale lookup table for sharp edges (no interpolation, just duplicate pixels)
const tsMrvRszLut s_tMrvRszLut_Sharp = // sharp edges
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F};

//#endif // SL_SUPPORT_PARTLY
//#ifdef MRV_SUPPORT_SL

//! structure combining virtual ISP windows settings
typedef struct 
{
    tsMrvWindow tWndBlacklines;
    tsMrvWindow tWndZoomCrop;
} tsMrvVirtualIspWnds;

//! static storage to remember last applied virtual ISP window settings
static tsMrvVirtualIspWnds s_tLastIspWnds = {0};



/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_GetScaleReg \n
 *  \RETURNVALUE value to program into scaler register \n
 *  \PARAMETERS  usIn               - input value (number of pixels) \n
 *               usOut              - output value (number of pixels) \n
 *
 *  \DESCRIPTION Calculates the value to program into the tsMrvScale or
 *               tsMrvSScale structures to scale from usIn pixels to usOut
 *               pixels. \n
 *               The formulas are taken from the MARVIN / MARVIN3PLUS user
 *               manuals (fixed-point calculation using 32 bit during
 *               processing, will overflow at an output size of 1048575 pixels). \n
 */
/*****************************************************************************/
static UINT32 MrvSls_GetScaleReg( UINT16 usIn, UINT16 usOut )
{   
   if (usIn > usOut) 
   {  // downscaling
      return (UINT32)( ( (((UINT32)usOut - 1) * RSZ_SCALER_BYPASS) / (UINT32)(usIn - 1) ) + 1 );
      // changed to avoid LINT warnings (Warning 573)
      // changed to avoid LINT warnings (Info 734)
   }
   else if (usIn < usOut)
   {  // upscaling
      return (UINT32)( ( (((UINT32)usIn - 1) * RSZ_SCALER_BYPASS) / (UINT32)(usOut - 1) ) | (UINT32)RSZ_UPSCALE_ENABLE );
      // changed to avoid LINT warnings (Warning 573)
      // changed to avoid LINT warnings (Info 734)
   }
   // no scaling
   return RSZ_SCALER_BYPASS;
}
   

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_CalcScaleFactors \n
 *  \RETURNVALUE One of the MRV_DPD_DMA_IN_xxx defines describing the YCbCr
 *               data format present at the output of the scaler. If it doesn't
 *               match any of the given formats, (UINT32)(-1) is returned. \n
 *  \PARAMETERS  ptSource        - pointer to the data path description
 *                                 containing information source of data
 *                                 feeding the path.
 *               ptPath          - pointer to the data path descriptor for
 *                                 which the scale factors are to be calculated \n
 *               ptScale         - pointer to a structure to receive the
 *                                 calculated results \n
 *               iImplementation - hint about how the scaler hardware is
 *                                 implemented
 *                                 (MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
 *                                 or MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV) \n
 *
 *  \DESCRIPTION Calculates the values of the tsMrvScale structure for the given
 *               input size and data path descriptor. \n
 */
/*****************************************************************************/
/*static*/ UINT32 MrvSls_CalcScaleFactors( const tsMrvDatapathDesc *ptSource,
                                     const tsMrvDatapathDesc *ptPath,
                                     tsMrvScale* ptScale,
                                     int iImplementation )                                    
{
   UINT32 ulScalerOutputFormat;
   UINT32 ulCSSflags;
   UINT32 ulScalerInputFormat;
   
   UINT16 usChromaInW;
   UINT16 usChromaInH;
   UINT16 usChromaOutWCR;
   UINT16 usChromaOutWCB;
   UINT16 usChromaOutH;
   
   memset(ptScale, 0, sizeof(tsMrvScale));
      
   // calculate Y scale factors
   ptScale->ulScaleHY = MrvSls_GetScaleReg(ptSource->usOutW, ptPath->usOutW);
   ptScale->ulScaleVY = MrvSls_GetScaleReg(ptSource->usOutH, ptPath->usOutH);

   // figure out the color input format of the scaler
   switch (ptPath->ulFlags & MRV_DPD_MODE__MASK)
   {
      case MRV_DPD_MODE_DMAYC_DIRECT:
      case MRV_DPD_MODE_DMAYC_ISP:
      case MRV_DPD_MODE_DMAJPEG_DIRECT:
      case MRV_DPD_MODE_DMAJPEG_ISP:
         // DMA-read originated data
         ulScalerInputFormat = ptPath->ulFlags & MRV_DPD_DMA_IN__MASK;
         break;
      default:
         // ISP originated data
         ulScalerInputFormat = MRV_DPD_DMA_IN_422;
         break;
   }

   // Note:
   // The scalers can exist in one of two versions, which require a slightly different 
   // programming of the chrominance scale factors. The Specification does not state this clearly, 
   // but the difference is 'by design' and verified with an FPGA evaluation board. 
   if (iImplementation == MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV)
   {
      // This is the more powerful implementation of the scaler, which handles the U and V chrominance
      // values in two independant datapathes, and therefore requires a programming strictly according
      // to the specification. Thus, the chrominance scaling factors are *not* just copies of the
      // ones for the luminance divided by the color format downsampling factor, but must be calculated
      // separately using the same formular as used for luminance, but with the correct in and out
      // dimentions of the chrominance buffers. In practice, this usually will result in a scaler factor
      // that differs from the formerly used value just about two or three, but this difference counts!
      switch (ulScalerInputFormat)
      {
         case MRV_DPD_DMA_IN_422:
            usChromaInW  = ptSource->usOutW / 2;
            usChromaInH  = ptSource->usOutH;
            usChromaOutWCR = ptPath->usOutW / 2;
            usChromaOutWCB = (ptPath->usOutW + 1) / 2;
            usChromaOutH = ptPath->usOutH;
            break;
         case MRV_DPD_DMA_IN_420:
            usChromaInW  = ptSource->usOutW / 2;
            usChromaInH  = ptSource->usOutH / 2;
            usChromaOutWCR = ptPath->usOutW / 2;
            usChromaOutWCB = (ptPath->usOutW + 1) / 2;
            usChromaOutH = ptPath->usOutH / 2;
            break;
         case MRV_DPD_DMA_IN_411:
            usChromaInW  = ptSource->usOutW / 4;
            usChromaInH  = ptSource->usOutH;
            usChromaOutWCR = ptPath->usOutW / 4;
            usChromaOutWCB = (ptPath->usOutW + 2) / 4;
            usChromaOutH = ptPath->usOutH;
            break;
         case MRV_DPD_DMA_IN_444:
         default:
            usChromaInW  = ptSource->usOutW;
            usChromaInH  = ptSource->usOutH;
            usChromaOutWCB = usChromaOutWCR = ptPath->usOutW;
            usChromaOutH = ptPath->usOutH;
            break;
      }
   }
   else
   {
      // This is the former implementation of the scaler, which sticks to the input format YUV422,
      // and processes both U and V crominance values in a shared datapath using time multiplex processing. 
      // Thus, the scaler factors for the chrominance path can be calculated the same way as
      // for the luminance path. 
      ASSERT(iImplementation == MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV);
      ASSERT(ulScalerInputFormat == MRV_DPD_DMA_IN_422);
      usChromaInW  = ptSource->usOutW;
      usChromaInH  = ptSource->usOutH;
      usChromaOutWCB = usChromaOutWCR = ptPath->usOutW;
      usChromaOutH = ptPath->usOutH;
   }

   // calculate chrominance scale factors
   switch (ptPath->ulFlags & MRV_DPD_CSS_H__MASK)
   {
      case MRV_DPD_CSS_H2:   usChromaOutWCB /= 2;usChromaOutWCR /= 2; break;
      case MRV_DPD_CSS_H4:   usChromaOutWCB /= 4;usChromaOutWCR /= 4; break;
      case MRV_DPD_CSS_HUP2: usChromaOutWCB *= 2;usChromaOutWCR *= 2; break;
      case MRV_DPD_CSS_HUP4: usChromaOutWCB *= 4;usChromaOutWCR *= 4; break;
      default: /*leave usChromaOutW untouched*/ break;
   }
   ptScale->ulScaleHCR = MrvSls_GetScaleReg(usChromaInW, usChromaOutWCR);
   ptScale->ulScaleHCB = MrvSls_GetScaleReg(usChromaInW, usChromaOutWCB);
   switch (ptPath->ulFlags & MRV_DPD_CSS_V__MASK)
   {
      case MRV_DPD_CSS_V2:   usChromaOutH /= 2; break;
      case MRV_DPD_CSS_V4:   usChromaOutH /= 4; break;
      case MRV_DPD_CSS_VUP2: usChromaOutH *= 2; break;
      case MRV_DPD_CSS_VUP4: usChromaOutH *= 4; break;
      default: /*leave usChromaOutH untouched*/ break;
   }
   ptScale->ulScaleVC = MrvSls_GetScaleReg(usChromaInH, usChromaOutH);
         
   // additional chrominance phase shifts
   if (ptPath->ulFlags & MRV_DPD_CSS_HSHIFT) {
      ptScale->usPhaseHC = SCALER_COFFS_NONCOSITED;
   }
   if (ptPath->ulFlags & MRV_DPD_CSS_VSHIFT) {
      ptScale->usPhaseVC = SCALER_COFFS_NONCOSITED;
   }
   // additional luminance phase shifts
   if (ptPath->ulFlags & MRV_DPD_LUMA_HSHIFT) {
      ptScale->usPhaseHY = SCALER_COFFS_NONCOSITED;
   }   
   if (ptPath->ulFlags & MRV_DPD_LUMA_VSHIFT) {
      ptScale->usPhaseVY = SCALER_COFFS_NONCOSITED;
   }
   
   // try to figure out the outcoming YCbCr format
   ulCSSflags = ptPath->ulFlags & MRV_DPD_CSS__MASK;
   if (ulCSSflags == (MRV_DPD_CSS_H_OFF | MRV_DPD_CSS_V_OFF))
   {  // trivial case: the output format is not changed
      ulScalerOutputFormat = ulScalerInputFormat;
   }
   else
   {  // output format gets changed by the scaler setting
      ulScalerOutputFormat = (UINT32)(-1);  // assume invalid format by default
      switch (ulScalerInputFormat)
      {
         case MRV_DPD_DMA_IN_444:
            if (ulCSSflags == (MRV_DPD_CSS_H2 | MRV_DPD_CSS_V_OFF))
            {  // conversion 444 -> 422
               ulScalerOutputFormat = MRV_DPD_DMA_IN_422;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_H4 | MRV_DPD_CSS_V_OFF))
            {  // conversion 444 -> 411
               ulScalerOutputFormat = MRV_DPD_DMA_IN_411;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_H2 | MRV_DPD_CSS_V2))
            {  // conversion 444 -> 420
               ulScalerOutputFormat = MRV_DPD_DMA_IN_420;
            }
            break;
            
         case MRV_DPD_DMA_IN_422:
            if (ulCSSflags == (MRV_DPD_CSS_HUP2 | MRV_DPD_CSS_V_OFF))
            {  // conversion 422 -> 444
               ulScalerOutputFormat = MRV_DPD_DMA_IN_444;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_H2 | MRV_DPD_CSS_V_OFF))
            {  // conversion 422 -> 411
               ulScalerOutputFormat = MRV_DPD_DMA_IN_411;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_H_OFF | MRV_DPD_CSS_V2))
            {  // conversion 422 -> 420
               ulScalerOutputFormat = MRV_DPD_DMA_IN_420;
            }
            break;
         
         case MRV_DPD_DMA_IN_420:
            if (ulCSSflags == (MRV_DPD_CSS_HUP2 | MRV_DPD_CSS_VUP2))
            {  // conversion 420 -> 444
               ulScalerOutputFormat = MRV_DPD_DMA_IN_444;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_H2 | MRV_DPD_CSS_VUP2))
            {  // conversion 420 -> 411
               ulScalerOutputFormat = MRV_DPD_DMA_IN_411;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_H_OFF | MRV_DPD_CSS_VUP2))
            {  // conversion 420 -> 422
               ulScalerOutputFormat = MRV_DPD_DMA_IN_422;
            }
            break;

        case MRV_DPD_DMA_IN_411:
            if (ulCSSflags == (MRV_DPD_CSS_HUP4 | MRV_DPD_CSS_V_OFF))
            {  // conversion 411 -> 444
               ulScalerOutputFormat = MRV_DPD_DMA_IN_444;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_HUP2 | MRV_DPD_CSS_V_OFF))
            {  // conversion 411 -> 422
               ulScalerOutputFormat = MRV_DPD_DMA_IN_422;
            }
            else if (ulCSSflags == (MRV_DPD_CSS_HUP2 | MRV_DPD_CSS_V2))
            {  // conversion 411 -> 420
               ulScalerOutputFormat = MRV_DPD_DMA_IN_420;
            }
            break;

         default:
            // DMA input format not supported
            break;
       }
   }
   
   
   return ulScalerOutputFormat;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_GetRszLut \n
 *  \RETURNVALUE address of up-scaling lookup table to use \n
 *  \PARAMETERS  ulFlags - flags of the data path \n
 *
 *  \DESCRIPTION Returns the address of up-scaling lookup table to use for the
 *               given data path flags. \n
 */
/*****************************************************************************/
/*static*/const tsMrvRszLut* MrvSls_GetRszLut( UINT32 ulFlags )
{
    const tsMrvRszLut* ptRetVal;
    switch (ulFlags & MRV_DPD_UPSCALE__MASK)
    {
       case MRV_DPD_UPSCALE_SHARP: ptRetVal = &s_tMrvRszLut_Sharp; break;
       default:                    ptRetVal = &s_tMrvRszLut_SmoothLin; break;
    }              
    return ptRetVal;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_CalcMainPathSettings \n
 *  \RETURNVALUE RET_SUCCESS - everything is fine \n
 *               RET_NOTSUPP - a feature is not supported \n
 *  \PARAMETERS  ptSource    - pointer to the data path description containing
 *                             information about the source of data feeding the
 *                             main path. This includes frame size only. The
 *                             ulFlags field is not used in here.
 *               ptMain      - pointer to the data path description for the
 *                             main data path. \n
 *               ptScaleM    - pointer to a structure to receive the calculated
 *                             results for the main path scaler \n
 *               ptMrvMiCtrl - pointer to MI configuration struct which is used
 *                             to store main path related parameters \n
 *
 *  \DESCRIPTION Fills in scale factors and MI configuration for the main path.
 *               Note that only self path related settings will be written into
 *               the MI configuration struct, so this routine can be used for
 *               both ISP and DMA originated data path setups. \n
 *
 *  \NOTES  Following fields are being filled in: \n
 *            ptScaleM:             \n
 *               [all fields]       \n
 *            ptMrvMiCtrl:          \n
 *               eMrvMifMpPicForm   \n
 *               eMainPath          \n
 */
/*****************************************************************************/
/*static*/RESULT MrvSls_CalcMainPathSettings( const tsMrvDatapathDesc *ptSource,
                                           const tsMrvDatapathDesc *ptMain,
                                           tsMrvScale* ptScaleM,
                                           tsMrvMiCtrl* ptMrvMiCtrl)
{
    UINT32 ulMainF;

#ifdef SL_FULL_SUPPORT    
    PRE_CONDITION(ptSource != NULL);
    PRE_CONDITION(ptScaleM != NULL);
    PRE_CONDITION(ptMrvMiCtrl != NULL);
#endif        
    // assume datapath deactivation if no selfpath pointer is given)
    if (ptMain) {
        ulMainF = ptMain->ulFlags;
    } else {
        ulMainF = 0;
    }
    
    // initialize the given parameters
    // (don't initialize ptMrvMiCtrl, because we just fill in parts of this structure)
    memset(ptScaleM, 0, sizeof(tsMrvScale));
    ptScaleM->ulScaleHY  = RSZ_SCALER_BYPASS;
    ptScaleM->ulScaleHCB = RSZ_SCALER_BYPASS;
    ptScaleM->ulScaleHCR = RSZ_SCALER_BYPASS;
    ptScaleM->ulScaleVY  = RSZ_SCALER_BYPASS;
    ptScaleM->ulScaleVC  = RSZ_SCALER_BYPASS;
        
    if (ulMainF & MRV_DPD_ENABLE)
    {
        switch (ulMainF & MRV_DPD_MODE__MASK)
        {
            case MRV_DPD_MODE_ISPYC:
            case MRV_DPD_MODE_DMAYC_ISP:
                ptMrvMiCtrl->eMainPath = eMrvPathOn; 
                break;
	#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
	            case MRV_DPD_MODE_DPCC:
	                ptMrvMiCtrl->eMainPath = eMrvPathDpcc; 
	                break;
	#endif                
            case MRV_DPD_MODE_ISPJPEG:
            case MRV_DPD_MODE_DMAJPEG_DIRECT:
            case MRV_DPD_MODE_DMAJPEG_ISP:
                ptMrvMiCtrl->eMainPath = eMrvPathJpe;
                break;
            case MRV_DPD_MODE_ISPRAW:

	#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
	                DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: RAW path not available with small output unit\n") );
	                return RET_NOTSUPP;
	#else                    
	                ptMrvMiCtrl->eMainPath = eMrvPathRaw;
	                break;
	#endif
            case MRV_DPD_MODE_ISPRAW_16B:        

	#if (MARVIN_FEATURE_MI < MARVIN_FEATURE_MI_V3)
	                DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: RAW 16bit path not available with this MI\n") );
	                return RET_NOTSUPP;
	#else                    
	                ptMrvMiCtrl->eMainPath = eMrvPathRaw16;
	                break;
	#endif

            default:
                DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: unknown/unsupported mode for main path\n") );
                return RET_NOTSUPP;
        }            
        if (ulMainF & MRV_DPD_HWRGB__MASK) {
            DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: RGB conversion not supported for main path\n") );
            return RET_NOTSUPP;
        }
        if (ulMainF & (MRV_DPD_H_FLIP | MRV_DPD_V_FLIP | MRV_DPD_90DEG_CCW)) {
            DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: flipping & rotating not supported for main path\n") );
            return RET_NOTSUPP;
        }
        if (ulMainF & MRV_DPD_NORESIZE)
        {
            if (ulMainF & MRV_DPD_CSS__MASK){
                DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: chroma subsampling in main path needs rezizer\n") );
                return RET_NOTSUPP;            
            }
            if (ulMainF & (MRV_DPD_LUMA_HSHIFT | MRV_DPD_LUMA_VSHIFT)){
                DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: luma shifting in main path needs rezizer\n") );
                return RET_NOTSUPP;            
            }
        }
        else
        {
            if ((ptMrvMiCtrl->eMainPath == eMrvPathRaw)||(ptMrvMiCtrl->eMainPath == eMrvPathRaw)) {
                DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: scaler not available in RAW mode of main path\n") );
                return RET_NOTSUPP;            
            }
            if( ptMain != NULL )    // changed to avoid LINT warnings (Warning 613)
            {
                // upscaling only to factor MAIN_UPSCALE_FACTOR_MAX possible
                if ( (((UINT32)(ptSource->usOutW) * MAIN_UPSCALE_FACTOR_MAX) < ptMain->usOutW) ||
                     (((UINT32)(ptSource->usOutH) * MAIN_UPSCALE_FACTOR_MAX) < ptMain->usOutH) )
                {
                    DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: main scaler upscaling capability exceeded\n") );
                    return RET_NOTSUPP;            
                }              
                if ((ptMain->usOutW > MAIN_SCALER_WIDTH_MAX) ||
                    (ptMain->usOutW < SCALER_MIN) ||
                    (ptMain->usOutH < SCALER_MIN)) {
                    DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: main scaler output range exceeded\n") );
                    return RET_NOTSUPP;
                }
            }
            else
            {
                ASSERT( ptMain != NULL );
            }
            //Remember that the input picture width should be even if the scaler is used
            //(otherwise the scaler may show unexpected behaviour in some rare cases)
            if (ptSource->usOutW & 0x01) {
                DBG_OUT( (DERR, "ERR: MrvSls_CalcMainPathSettings: input picture width must be even! (is %hu)\n", ptSource->usOutW) );
                return RET_NOTSUPP;            
            }
         
            // calculate scale factors. 
            (void) MrvSls_CalcScaleFactors(ptSource, ptMain,
                                           ptScaleM, MARVIN_FEATURE_MSCALE_FACTORCALC );
        }
    }
    else
    {
        ptMrvMiCtrl->eMainPath = eMrvPathOff;
    }

    if(ulMainF & MRV_DPD_INTERLEAVED)
    {
    	ptMrvMiCtrl->eMrvMifMpPicForm = eMrvMif_PicForm_Interleaved;
    }
    else
    {
        ptMrvMiCtrl->eMrvMifMpPicForm = eMrvMif_PicForm_Planar;
    }
    
    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_CalcSelfPathSettings \n
 *  \RETURNVALUE RET_SUCCESS - everything is fine \n
 *               RET_NOTSUPP - a feature is not supported \n
 *  \PARAMETERS  ptSource    - pointer to the data path description
 *                             containing information source of data
 *                             feeding the self path. This includes frame size
 *                             only. The ulFlags field is not used in here. \n
 *               ptSelf      - pointer to the data path description for the
 *                             self data path. \n
 *               ptScaleS    - pointer to a structure to receive the calculated
 *                             results for the self path scaler \n
 *               ptMrvMiCtrl - pointer to MI configuration struct which is used
 *                             to store self path related parameters \n
 *
 *  \DESCRIPTION Fills in scale factors and MI configuration for the self path.
 *               Note that only self path related settings will be written into
 *               the MI config struct, so this routine can be used for both ISP
 *               and DMA originated datapath setups. \n
 *
 *  \NOTES  Following fields are being filled in: \n
 *            ptScaleS :            \n
 *               [all fields]       \n
 *            ptMrvMiCtrl :         \n
 *               eMrvMifSpOutForm   \n
 *               eMrvMifSpInForm    \n
 *               eMrvMifSpPicForm   \n
 *               eMrvMifSpMode      \n
 *               eSelfPath          \n
 */
/*****************************************************************************/
/*static*/ RESULT MrvSls_CalcSelfPathSettings( const tsMrvDatapathDesc *ptSource,
                                           const tsMrvDatapathDesc *ptSelf,
                                           tsMrvScale* ptScaleS,
                                           tsMrvMiCtrl* ptMrvMiCtrl)
{
#if( MARVIN_FEATURE_SSCALE != MARVIN_FEATURE_EXIST_NOT )        
    UINT32 ulScalerOutColFormat;
#endif  // #if( MARVIN_FEATURE_SSCALE != MARVIN_FEATURE_EXIST_NOT )        
    UINT32 ulSelfF;

#ifdef SL_FULL_SUPPORT	
    PRE_CONDITION(ptSource != NULL);
    PRE_CONDITION(ptScaleS != NULL);
    PRE_CONDITION(ptMrvMiCtrl != NULL);
#endif        
    // assume datapath deactivation if no selfpath pointer is given)
    if (ptSelf) {
        ulSelfF = ptSelf->ulFlags;
    } else {
        ulSelfF = 0;
    }
    
    // initialize the given parameters
    // (don't initialize ptMrvMiCtrl, because we just fill in parts of this structure)
    memset(ptScaleS, 0, sizeof(tsMrvScale));
    ptScaleS->ulScaleHY  = RSZ_SCALER_BYPASS;
    ptScaleS->ulScaleHCB = RSZ_SCALER_BYPASS;
    ptScaleS->ulScaleHCR = RSZ_SCALER_BYPASS;
    ptScaleS->ulScaleVY  = RSZ_SCALER_BYPASS;
    ptScaleS->ulScaleVC  = RSZ_SCALER_BYPASS;
        
    if (ulSelfF & MRV_DPD_ENABLE)
    {

	#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST_NOT)
	        UNUSED_PARAM(ptSource);        
	        DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: there's no self path available in the hardware\n") );
	        return RET_NOTSUPP;

	#else                
	        switch (ulSelfF & MRV_DPD_MODE__MASK)
	        {
		#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
		            case MRV_DPD_MODE_DPCC:
		                ulScalerOutColFormat = MRV_DPD_DMA_IN_422; 
		                ptMrvMiCtrl->eSelfPath = eMrvPathDpcc; 
		                break;
		#endif                
	            case MRV_DPD_MODE_ISPYC:
	                ptMrvMiCtrl->eSelfPath = eMrvPathOn;
	                ulScalerOutColFormat = MRV_DPD_DMA_IN_422; 
	                break;
	            case MRV_DPD_MODE_DMAYC_ISP:
	            case MRV_DPD_MODE_DMAYC_DIRECT:
	                ptMrvMiCtrl->eSelfPath = eMrvPathOn; 
	                ulScalerOutColFormat = ulSelfF & MRV_DPD_DMA_IN__MASK;
	                break;
	            default:
	                DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: unknown/unsupported mode for self path\n") );
	                return RET_NOTSUPP;
	        }            

	        if (ulSelfF & MRV_DPD_NORESIZE)
	        {
	            if (ulSelfF & MRV_DPD_CSS__MASK){
	                DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: chroma super/subsampling in self path needs rezizer\n") );
	                return RET_NOTSUPP;            
	            }
	            if (ulSelfF & (MRV_DPD_LUMA_HSHIFT | MRV_DPD_LUMA_VSHIFT)){
	                DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: luma shifting in self path needs rezizer\n") );
	                return RET_NOTSUPP;            
	            }
	            if( ptSelf != NULL )    // changed to avoid LINT warnings (Warning 613)
	            {
	                if ((ptSource->usOutW != ptSelf->usOutW) ||
	                    (ptSource->usOutH != ptSelf->usOutH)) {
	                    DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: different in/out sizes needs resizer\n") );
	                    return RET_NOTSUPP;            
	                }
	            }
	            else
	            {
	                ASSERT( ptSelf != NULL );
	            }
	        }
	        else
	        {
	            if( ptSelf != NULL )    // changed to avoid LINT warnings (Warning 613)
	            {
	                // upscaling only to factor SELF_UPSCALE_FACTOR_MAX possible
	                if ( (((UINT32)(ptSource->usOutW) * SELF_UPSCALE_FACTOR_MAX) < ptSelf->usOutW) ||
	                     (((UINT32)(ptSource->usOutH) * SELF_UPSCALE_FACTOR_MAX) < ptSelf->usOutH) )
	                {
	                    DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: self scaler upscaling capability exceeded\n") );
	                    return RET_NOTSUPP;            
	                }              
	                if ((ptSelf->usOutW > SELF_SCALER_WIDTH_MAX) ||
	                    (ptSelf->usOutW < SCALER_MIN) ||
	                    (ptSelf->usOutH < SCALER_MIN)) {
	                    DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: self scaler output range exceeded\n") );
	                    return RET_NOTSUPP;            
	                }
	            }
	            else
	            {
	                ASSERT( ptSelf != NULL );
	            }
	            //Remember that the input picture width should be even if the scaler is used
	            //(otherwise the scaler may show unexpected behaviour in some rare cases)
	            if (ptSource->usOutW & 0x01) {
	                DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: input picture width must be even! (is %hu)\n", ptSource->usOutW) );
	                return RET_NOTSUPP;            
	            }
	         
	            // calculate scale factors. 
	            ulScalerOutColFormat = MrvSls_CalcScaleFactors(ptSource, ptSelf,
	                                                           ptScaleS, MARVIN_FEATURE_SSCALE_FACTORCALC );
	        }

	        // figure out the input format setting
	        switch (ulScalerOutColFormat)
	        {
	            case MRV_DPD_DMA_IN_444: ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr444; break;
	            case MRV_DPD_DMA_IN_422: ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr422; break;
	            case MRV_DPD_DMA_IN_420: ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr420; break;
	            case MRV_DPD_DMA_IN_411: // no break, does not seem to be supported by HW
	            default:
	                DBG_OUT( (DERR, "ERR: MrvSls_CalcSelfPathSettings: MI self path input color format not supported\n") );
	                return RET_NOTSUPP;            
	        }
	        
	        // figure out the output format setting
	        switch (ulSelfF & MRV_DPD_HWRGB__MASK)
	        {
	            case MRV_DPD_HWRGB_565: ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_565; break;
	            case MRV_DPD_HWRGB_666: ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_666; break;
	            case MRV_DPD_HWRGB_888: ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_888; break;
	            case MRV_DPD_HWRGB_OFF:
	                ptMrvMiCtrl->eMrvMifSpOutForm = ptMrvMiCtrl->eMrvMifSpInForm;
	                break;
	            default:
	                DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: MI self path output color format not supported\n") );
	                return RET_NOTSUPP;            
	        }
	        
	        // picture flipping / rotation
	        switch (ulSelfF & (MRV_DPD_90DEG_CCW | MRV_DPD_V_FLIP | MRV_DPD_H_FLIP))
	        {
	            case (                                 MRV_DPD_H_FLIP): ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Horizontal_Flip; break;
	            case (                  MRV_DPD_V_FLIP               ): ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Vertical_Flip; break;
	            case (                  MRV_DPD_V_FLIP|MRV_DPD_H_FLIP): ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rotation_180_deg; break;
	            case (MRV_DPD_90DEG_CCW                              ): ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rotation_090_deg; break;
	            case (MRV_DPD_90DEG_CCW               |MRV_DPD_H_FLIP): ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rot_270_V_Flip; break;
	            case (MRV_DPD_90DEG_CCW|MRV_DPD_V_FLIP               ): ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rot_090_V_Flip; break;
	            case (MRV_DPD_90DEG_CCW|MRV_DPD_V_FLIP|MRV_DPD_H_FLIP): ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rotation_270_deg; break;
	            default:                                                ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Original;  break;
	        }
	#endif //#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST_NOT) #else

    }
    else
    {
        ptMrvMiCtrl->eSelfPath = eMrvPathOff;
    }

    if(ulSelfF & MRV_DPD_INTERLEAVED)
    {
    	ptMrvMiCtrl->eMrvMifSpPicForm = eMrvMif_PicForm_Interleaved;
    }
    else
    {
        ptMrvMiCtrl->eMrvMifSpPicForm = eMrvMif_PicForm_Planar;
    }

    
    return RET_SUCCESS;
}                                           


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_CalcDpMuxSettings \n
 *  \RETURNVALUE RET_SUCCESS - everything is fine \n
 *               RET_NOTSUPP - something went wrong \n
 *  \PARAMETERS  ptMiCtrl     - pointer to the mi configuration struct \n
 *               peYcsChnMode - pointer to the enum to receive the appropriate
 *                              setting for the Ycs cannel mode \n
 *               peDpSwitch   - pointer to the enum to receive the appropriate
 *                              setting for the main data path switch \n
 *
 *  \DESCRIPTION Translates the given memory interface configuration struct
 *               into appropriate values to program the data path multiplexers. \n
 */
/*****************************************************************************/
static RESULT MrvSls_CalcDpMuxSettings( const tsMrvMiCtrl *ptMiCtrl,
                                        teMrvYcsChnMode *peYcsChnMode,
                                        teMrvDpSwitch *peDpSwitch )
{
    switch (ptMiCtrl->eMainPath)
    {
    case eMrvPathRaw:
    case eMrvPathRaw16:
        *peDpSwitch   = eMrvDpRaw;
        *peYcsChnMode = eMrvYcsMvRaw;
        if ((ptMiCtrl->eSelfPath != eMrvPathOff) && (ptMiCtrl->eSelfPath != eMrvPathDpcc)) {
            DBG_OUT( (DERR, "ERR: MrvSls_CalcDpMuxSettings: self path can not be combined with RAW mode of main path\n") );
            return RET_NOTSUPP;
        }
        break;
        
    case eMrvPathJpe:
        *peDpSwitch   = eMrvDpJpeg;
        if ((ptMiCtrl->eSelfPath != eMrvPathOff) && (ptMiCtrl->eSelfPath != eMrvPathDpcc)) {
            *peYcsChnMode = eMrvYcsMvSp;
        } else {
            //ptMiCtrl->eSelfPath == eMrvPathDpcc need no split path self enable
            *peYcsChnMode = eMrvYcsMv;
        }
        break;
        
    case eMrvPathOn:
        *peDpSwitch   = eMrvDpMv;
        if ((ptMiCtrl->eSelfPath != eMrvPathOff) && (ptMiCtrl->eSelfPath != eMrvPathDpcc)) {
            *peYcsChnMode = eMrvYcsMvSp;
        } else {
            //ptMiCtrl->eSelfPath == eMrvPathDpcc need no split path self enable
            *peYcsChnMode = eMrvYcsMv;
        }
        break;
        
    case eMrvPathOff:
    case eMrvPathDpcc:
        *peDpSwitch   = eMrvDpMv;
        if (ptMiCtrl->eSelfPath != eMrvPathOff) {
            *peYcsChnMode = eMrvYcsSp;
        } else {
            *peYcsChnMode = eMrvYcsOff;
        }
        break;
        
    default:
        ASSERT(0);
        //break;
    }
    
    return RET_SUCCESS;
}

#endif // SL_SUPPORT_PARTLY
#ifdef MRV_SUPPORT_SL

#define ISPWND_COMBINE_WNDS    0x00000001  // the windows to cut away black pixels and to zoom/crop the 
                                           // image must be combined before they are applyed to the marvin registers
#define ISPWND_APPLY_OUTFORM   0x00000002  // call of the MrvIspSetOutputFormatter() routine necessary
#define ISPWND_APPLY_ISCONF    0x00000004  // call of the MrvIsSetConfig() routine necessary
#define ISPWND_NO_CROPPING     0x00000008  // no cropping supported at all

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_GetIspWndStyle \n
 *  \RETURNVALUE combination of the ISPWND_ flags \n
 *  \PARAMETERS  eIspPath - path through the ISP that is to be used \n
 *  \DESCRIPTION Returns information about how to combine black pixel and 
 *               zoom/crop windows for programming the ISP output formatter 
 *               and the image stabilization unit for the given marvin derivative
 *               and ISP path. 
 */
/*****************************************************************************/
static UINT32 MrvSls_GetIspWndStyle(eMrvIspPath eIspPath)
{
    UINT32 ulRes = 0;

#if (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_OUTPUT_FORMATTER_V1)
    // output formatter exists at ISP output (old style)
    #if (MARVIN_FEATURE_IMG_STABILIZATION != MARVIN_FEATURE_EXIST_NOT)
        #error "unexpected MARVIN_FEATURE_IMG_STABILIZATION value combined with MARVIN_FEATURE_OUTPUT_FORMATTER_V1"
    #endif
    // --> we need to combine blackline and zoom/crop window and program the one
    //     and only output formatter with it.
    ulRes = ISPWND_COMBINE_WNDS |
            ISPWND_APPLY_OUTFORM;


#elif (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_OUTPUT_FORMATTER_V2)
    // output formatter exists at ISP input
    #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
        // no image stabilization
        // --> we need to combine blackline and zoom/crop window and program the one
        //     and only output formatter with it.
        ulRes = ISPWND_COMBINE_WNDS |
                ISPWND_APPLY_OUTFORM;
        
    #elif (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V1)
        // image stabilization exists in isp bayer path only
        if (eIspPath == eMrvIspPath_Bayer)
        {   // --> we need to program the output formatter with the blackline window and 
            //     the image stabilization formatter with the zoom/crop window.
            ulRes = ISPWND_APPLY_OUTFORM |
                    ISPWND_APPLY_ISCONF;
        }
        else
        {   // --> we need to combine blackline and zoom/crop window and program the
            //     output formatter with it. The image stabilisation module is present, but can't be used here
            ulRes = ISPWND_COMBINE_WNDS |
                    ISPWND_APPLY_OUTFORM;
        }

    #elif (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)        
        // image stabilization in both bayer and YCbCr paths        
        if ((eIspPath == eMrvIspPath_Bayer) ||
            (eIspPath == eMrvIspPath_YCbCr))
        {   // --> we need to program the output formatter with the blackline window and 
            //     the image stabilization formatter with the zoom/crop window.
            ulRes = ISPWND_APPLY_OUTFORM |
                    ISPWND_APPLY_ISCONF;
        }
        else
        {   // --> we need to combine blackline and zoom/crop window and program the
            //     output formatter with it. The image stabilisation module is present, but can't be used here
            ulRes = ISPWND_COMBINE_WNDS |
                    ISPWND_APPLY_OUTFORM;
        }
    
    #else
        #error "unexpected MARVIN_FEATURE_IMG_STABILIZATION value combined with MARVIN_FEATURE_OUTPUT_FORMATTER_V2"
    #endif


#elif (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT)
    // no output formatter at all
    #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V1)
        // image stabilization in isp bayer path only
        if (eIspPath == eMrvIspPath_Bayer)
        {   // --> we need to combine blackline and zoom/crop window and program the one
            //     and only image stabilization formatter with it.
            ulRes = ISPWND_COMBINE_WNDS |
                    ISPWND_APPLY_ISCONF;
        }
        else
        {   // --> sorry, marvin can't crop in this ISP path(s) 
            ulRes = ISPWND_NO_CROPPING;
        }
      
    #elif (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)        
        // image stabilization in both bayer and YCbCr paths        
        if ((eIspPath == eMrvIspPath_Bayer) ||
            (eIspPath == eMrvIspPath_YCbCr))
        {   // --> we need to combine blackline and zoom/crop window and program the one
            //     and only image stabilization formatter with it.
            ulRes = ISPWND_COMBINE_WNDS |
                    ISPWND_APPLY_ISCONF;
        }
        else
        {   // --> sorry, marvin can't crop in this ISP path(s) 
            ulRes = ISPWND_NO_CROPPING;
        }
         
    #else
        #error "unexpected MARVIN_FEATURE_IMG_STABILIZATION value combined with MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT"
    #endif


#else
    #error "unknown setting for MARVIN_FEATURE_OUTPUT_FORMATTER"
#endif    

    return ulRes;
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_SetIspWindows \n
 *  \RETURNVALUE RET_SUCCESS - everything is fine \n
 *               RET_NOTSUPP - something went wrong \n
 *  \PARAMETERS  ptIsiSensorConfig - pointer to the sensor configuration \n
 *               ptWndBlackline - pointer to blackline window configuration \n
 *               ptWndZoomCrop - pointer to the zoom/crop window configuration \n
 *
 *  \DESCRIPTION Applies the given windows for cutting away blacklines coming from 
 *               the image sensor and further cropping of the image for other 
 *               purposes like e.g. digital zoom to the output formatter and/or
 *               image stabilisation modules of Marvins ISP. \n
 */
/*****************************************************************************/
static RESULT MrvSls_SetIspWindows(const tsIsiSensorConfig *ptIsiSensorConfig,
                                   const tsMrvWindow* ptWndBlackline, 
                                   const tsMrvWindow* ptWndZoomCrop)
{
    tsMrvWindow   tWndOutForm;  
    tsMrvIsConfig tIsConf;
    eMrvIspPath   eIspPath;
    UINT32        ulWndStyle;
    
    memset( &tWndOutForm, 0, sizeof(tWndOutForm) );
    memset( &tIsConf, 0, sizeof(tIsConf) );
    
    // figure out the path through the ISP to process the data from the image sensor
    eIspPath = MrvIspSelectPath(ptIsiSensorConfig, NULL);
    if (eIspPath == eMrvIspPath_Unknown)
    {
        DBG_OUT( (DERR, "MrvSls_SetIspWindows: failed to detect marvin ISP path to use\n") );
        return RET_NOTSUPP;
    }
    
    // get the recommended way to configure output formatter and/or image stabilization
    ulWndStyle = MrvSls_GetIspWndStyle(eIspPath);
    if (ulWndStyle & ISPWND_NO_CROPPING)
    {   // cropping not possible -> make sure that it is *not* supposed to be used
        UINT16 usIsiX;
        UINT16 usIsiY;
        (void)IsiRes2Size(ptIsiSensorConfig->ulRes, &usIsiX, &usIsiY );  // changed to avoid LINT warnings (Warning 534)        
        if ((ptWndZoomCrop->usHSize != usIsiX) ||
            (ptWndZoomCrop->usVSize != usIsiY) ||
            (ptWndZoomCrop->usHOffs != 0) ||
            (ptWndZoomCrop->usVOffs != 0))
        {
            DBG_OUT( (DERR, "MrvSls_SetIspWindows: cropping not supported in selected ISP data path\n") );
            return RET_NOTSUPP;
        }
        if ((ptWndBlackline->usHSize != usIsiX) ||
            (ptWndBlackline->usVSize != usIsiY) ||
            (ptWndBlackline->usHOffs != 0) ||
            (ptWndBlackline->usVOffs != 0))
        {
            DBG_OUT( (DERR, "MrvSls_SetIspWindows: black lines are not supported in selected ISP data path\n") );
            return RET_NOTSUPP;
        }
    }
        
    // The image stabilization is allowed to move the window in both directions by
    // the same amount of pixels we have calculated for the offsets. The initial 
    // image stabilization window is equal to the zoom/crop window
    tIsConf.usMaxDx = ptWndZoomCrop->usHOffs;
    tIsConf.usMaxDy = ptWndZoomCrop->usVOffs;
    tIsConf.tMrvIsWindow = *ptWndZoomCrop;      

    if (ulWndStyle & ISPWND_COMBINE_WNDS)
    {   // combine both blackline and zoom/crop window
        tWndOutForm = *ptWndZoomCrop;
        tWndOutForm.usVOffs += ptWndBlackline->usVOffs;
        tWndOutForm.usHOffs += ptWndBlackline->usHOffs;
        tIsConf.tMrvIsWindow = tWndOutForm;
        if (ulWndStyle & ISPWND_APPLY_OUTFORM)
        {   // if the output formatter is to be used, offsets are cut away there, so
            // we don't need additional ones in the imags stabilization unit
            tIsConf.tMrvIsWindow.usHOffs = 0;
            tIsConf.tMrvIsWindow.usVOffs = 0;
        }
    }
    else
    {   // do not combine windows --> blacklines done with output formatter, 
        // zoom/cropping done with image stabilization
        tWndOutForm = *ptWndBlackline;
        tIsConf.tMrvIsWindow = *ptWndZoomCrop;
    }

    // finally, apply the settings to marvin
    if (ulWndStyle & ISPWND_APPLY_OUTFORM) {   
        MrvIspSetOutputFormatter( &tWndOutForm, eMrvCfgUpdateImmediate );
    }
    if (ulWndStyle & ISPWND_APPLY_ISCONF)
    {
        RESULT iRes = MrvIsSetConfig(&tIsConf);
        if (iRes != RET_SUCCESS) {
            DBG_OUT( (DERR, "ERR: MrvSls_DatapathIsp: failed to set image stabilization config\n") );
            return iRes;
        }   
    }
    
    // success - remember our virtual settings
    s_tLastIspWnds.tWndBlacklines = *ptWndBlackline;
    s_tLastIspWnds.tWndZoomCrop = *ptWndZoomCrop;
    
    return RET_SUCCESS;
}   


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_GetIspWindows
 *  \RETURNVALUE none
 *  \PARAMETERS  ptWndBlackline - pointer to structure to receive last applied 
 *                                blackline window configuration \n
 *               ptWndZoomCrop - pointer to structure to receive last applied 
 *                               zoom/crop window configuration \n
 *
 *  \DESCRIPTION Returns the (virtual) ISP window configuration applied with 
 *               the last recent call to MrvSls_SetIspWindows() \n
 */
/*****************************************************************************/
static void MrvSls_GetIspWindows(tsMrvWindow* ptWndBlackline, 
                                 tsMrvWindow* ptWndZoomCrop)
{
    if (ptWndBlackline) {
        *ptWndBlackline = s_tLastIspWnds.tWndBlacklines;
    }
    if (ptWndZoomCrop) {
        *ptWndZoomCrop = s_tLastIspWnds.tWndZoomCrop;
    }        
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_DatapathIsp \n
 *  \RETURNVALUE RET_SUCCESS in case of success \n
 *  \PARAMETERS  ptSysConf         - pointer to the system configuration \n
 *               ptMain            - pointer to the data path description for
 *                                   the main data path. \n
 *               ptSelf            - pointer to the data path description for
 *                                   the self data path. \n
 *               iZoom             - digital zoom factor to be applied. The
 *                                   zoom factor N can be calculated as
 *                                   follows: \n
 *                                   N = 1 + (iZoom / 1024) \n
 *                                   So a value of iZoom=0 result in a factor
 *                                   of 1, which means 'no zoom'. \n
 *                        
 *  \DESCRIPTION Configures main and self data pathes and scaler for data
 *               coming from the ISP. \n
 *
 *  Following MARVIN subsystems are programmed: \n
 *     - ISP output formatter \n
 *     - Image stabilization module \n
 *     - YC-Splitter \n
 *     - Self path DMA-read multiplexer \n
 *     - Main path multiplexer \n
 *     - Main & Self path resizer \n
 *     - Small output unit \n
 *     - Memory Interface (MI) input source, en/disable and data format \n
 *
 *  Following MARVIN subsystems are *NOT* programmed: \n
 *     - All ISP functionality but the output formatter & image stabilization module \n
 *     - Color Processing block \n
 *     - JPEG encode subsystem (quantisation tables etc.) \n
 *     - Memory Interface (MI) output buffer addresses and sizes \n
 */
/*****************************************************************************/
RESULT MrvSls_DatapathIsp( const tsPlSystemConfig *ptSysConf,
                           const tsMrvDatapathDesc *ptMain, 
                           const tsMrvDatapathDesc *ptSelf,
                           int iZoom )
{   
   // return value of several sub-functions called from here
   RESULT iRes;

   // copy of flags for main and self path to simplify access (no pointer de-reference)
   UINT32 ulMainF;
   UINT32 ulSelfF;
   
   // resolution from sensor configuration
   UINT16 usIsiX;
   UINT16 usIsiY;
   
   // things to apply to MARVIN
   tsMrvScale tScaleM;
   tsMrvScale tScaleS;
   teMrvYcsChnMode eChnMode;
   teMrvDpSwitch eDpSwitch;
   tsMrvMiCtrl tMrvMiCtrl;
   tsMrvDatapathDesc tSource;
   tsMrvWindow tWndBlackline; // ISP windowing because of cutting away blacklines from the sensor
   tsMrvWindow tWndZoomCrop;  // ISP windowing because of aspect ratio change and/or zoom 
   
   // assume dapapath deactivation for not provided descriptors 
   ulMainF = 0;
   ulSelfF = 0;
   if (ptMain) {
      ulMainF = ptMain->ulFlags;
   }
   if (ptSelf) {
      ulSelfF = ptSelf->ulFlags;
   }
   
   // initialize variables on the stack
   iRes = RET_SUCCESS;
   (void)IsiRes2Size(ptSysConf->ptIsiConfig->ulRes, &usIsiX, &usIsiY );  // changed to avoid LINT warnings (Warning 534)
   memset( &tMrvMiCtrl, 0, sizeof(tsMrvMiCtrl) );
   memset( &tWndBlackline, 0, sizeof(tWndBlackline) );
   memset( &tWndZoomCrop, 0, sizeof(tWndZoomCrop) );

   //////////////////////////////////////////
   // ISP Windowing - fill in tWndOutForm, bApplyOutForm, tIsConf and bApplyIsConf
   {
      const tsMrvDatapathDesc* ptTarget = NULL;

      // by default, size of both blackline and zoom/crop window is what the camera delivers.
      // (no cropping, no offset)
      tWndBlackline.usHSize = usIsiX;
      tWndBlackline.usVSize = usIsiY;
      tWndZoomCrop = tWndBlackline;
      // check if we have to crop because of aspect ratio preservement of an 
      // output channel
      if ((ulMainF & MRV_DPD_ENABLE) &&
          (ulMainF & MRV_DPD_KEEPRATIO))
      {
         ptTarget = ptMain;
      }
      if ((ulSelfF & MRV_DPD_ENABLE) &&
          (ulSelfF & MRV_DPD_KEEPRATIO))
      {
         if (ptTarget) {
            DBG_OUT( (DERR, "ERR: MrvSls_DatapathIsp: MRV_DPD_KEEPRATIO only allowed for one path\n") );
            return RET_NOTSUPP;
         }
         ptTarget = ptSelf;
      }
      // if so, calculate the cropping
      if (ptTarget)
      {
         UINT32 ulAspectCam = (0x1000 * ((UINT32)usIsiX)) / usIsiY;
         UINT32 ulAspectTarget = (0x1000 * ((UINT32)(ptTarget->usOutW))) / ptTarget->usOutH;
         if (ulAspectCam < ulAspectTarget)
         {  // camera aspect is more 'portrait-like' as target aspect. We have to crop the camera
            // picture by cutting off a bit of the top & bottom
            tWndZoomCrop.usVSize = (UINT16)(((UINT32)usIsiX * (UINT32)(ptTarget->usOutH)) / ptTarget->usOutW);   // changed to avoid LINT warnings (Info 734)
         }
         else
         {  // camera aspect is more 'landscape-like' as target aspect. We have to crop the camera
            // picture by cutting off a bit of the left and right
            tWndZoomCrop.usHSize = (UINT16)(((UINT32)usIsiY * (UINT32)(ptTarget->usOutW)) / ptTarget->usOutH);   // changed to avoid LINT warnings (Info 734)
         }         
      }
      // now, we may also want to do digital zoom. If so, we need to shrink the ISP window by
      // the desired zoom factor.
      if (iZoom > 0)
      {
         tWndZoomCrop.usVSize = (UINT16)(((UINT32)(tWndZoomCrop.usVSize) * 1024) / (1024 + (UINT32)iZoom));   // changed to avoid LINT warnings (Warning 573)
         tWndZoomCrop.usHSize = (UINT16)(((UINT32)(tWndZoomCrop.usHSize) * 1024) / (1024 + (UINT32)iZoom));   // changed to avoid LINT warnings (Warning 573)
      }
      //Remember that the output formatter usHSize should be even if the scaler is used
      //(otherwise the scaler may show unexpected behaviour in some rare cases)
      tWndZoomCrop.usHSize &= ~0x01;      
      // At last, we care about the offset of the ISP window. We want it centered
      // on the image data delivered by the sensor (not counting possible black lines)
      tWndZoomCrop.usHOffs = (usIsiX - tWndZoomCrop.usHSize) / 2;
      tWndZoomCrop.usVOffs = (usIsiY - tWndZoomCrop.usVSize) / 2;
      // If the image sensor delivers blacklines, we cut them away with moving tWndBlackline
      // window by the given amount of lines
      switch(ptSysConf->ptIsiConfig->ulBls )
      {
         case ISI_BLS_OFF: break; //no black lines
         case ISI_BLS_TWO_LINES: tWndBlackline.usVOffs += 2; break; //two black lines at frame start
         case ISI_BLS_SIX_LINES: tWndBlackline.usVOffs += 6; break; //six black lines at frame start
         case ISI_BLS_FOUR_LINES: tWndBlackline.usVOffs += 2; break; //two black lines at frame start and two at the end
         default:
            DBG_OUT( (DERR, "MrvSls_DatapathIsp: unsupported black lines setting in sensor config\n") );
            return RET_NOTSUPP;
      }      
      // if we are instructed to show the blacklines and the sensor generates them, 
      // we have to move the ISP windows to the upper border of the whole sensor, and deny
      // the image stabilization to move around the window in vertical direction.
      if (ptSysConf->ptIsiConfig->ulBls != ISI_BLS_OFF)
      {
         if (((ulMainF & MRV_DPD_ENABLE) && (ulMainF & MRV_DPD_BLACKLINES_TOP)) ||
             ((ulSelfF & MRV_DPD_ENABLE) && (ulSelfF & MRV_DPD_BLACKLINES_TOP)))
         {
            if ((ulMainF & MRV_DPD_ENABLE) &&
                (ulSelfF & MRV_DPD_ENABLE) &&
                ((ulMainF & MRV_DPD_BLACKLINES_TOP) != (ulSelfF & MRV_DPD_BLACKLINES_TOP)))
            {
               DBG_OUT( (DERR, "MrvSls_DatapathIsp: concurrent MRV_DPD_BLACKLINES_TOP setting in main and self path\n") );
               return RET_NOTSUPP;
            }
            tWndBlackline.usVOffs = 0;
            tWndZoomCrop.usVOffs = 0;
         }
      }
      
      // copy settings to datapath description structure for later usage
      tSource.usOutW = tWndZoomCrop.usHSize;
      tSource.usOutH = tWndZoomCrop.usVSize;
      tSource.ulFlags = MRV_DPD_DMA_IN_422;
      
      // At this point, tWndZoomCrop and tWndBlackline contain the window sizes that reflect
      // the users request. We have to configure the ISP output formatter and the image stabilization
      // formatter in order to achieve this, but how they interact is highly dependant of the 
      // current marvin derivative and which datapath of the ISP is activated. Therefore, translating
      // tWndZoomCrop and tWndBlackline into marvin register settings is a bit complicated and will
      // be done by the MrvSls_SetIspWindows() routine.
   }                 
   // ISP Window
   //////////////////////////////////////////


    //////////////////////////////////////////
    // MAIN path - fill in eMainPath, tScaleM  and ptMainRszLut
    // basic selfpath settings
    iRes = MrvSls_CalcMainPathSettings(&tSource, ptMain, &tScaleM, &tMrvMiCtrl);
    if (iRes != RET_SUCCESS) {
        return iRes;
    }
    // additional settings specific for main path fed from ISP
    if (ulMainF & MRV_DPD_ENABLE)
    {
        switch (ulMainF & MRV_DPD_MODE__MASK)
        {
#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
            case MRV_DPD_MODE_DPCC:
#endif                
            case MRV_DPD_MODE_ISPYC:
            case MRV_DPD_MODE_ISPRAW:
            case MRV_DPD_MODE_ISPRAW_16B:
            case MRV_DPD_MODE_ISPJPEG:
                // allowed cases, just proceed
                break;
            default:
                DBG_OUT( (DERR, "MrvSls_DatapathIsp: main path mode not supported for data coming from the ISP\n") );
                return RET_NOTSUPP;
        }
    }            
    // MAIN path   
    //////////////////////////////////////////

   
    //////////////////////////////////////////
    // SELF path - fill in eSelfPath & tScaleS
    // basic selfpath settings
    iRes = MrvSls_CalcSelfPathSettings(&tSource, ptSelf, &tScaleS, &tMrvMiCtrl);
    if (iRes != RET_SUCCESS) {
        return iRes;
    }
    if(ptSysConf->ptConfig->Flags.fYCbCrNonCosited)
    {
        tMrvMiCtrl.eMrvMifSpInPhase = eMrvMifCol_Phase_Non_Cosited;    //predefined for best JPEG
    }
    else
    {
        tMrvMiCtrl.eMrvMifSpInPhase = eMrvMifCol_Phase_Cosited;    //BT601 conform
    }
    if(ptSysConf->ptConfig->Flags.fYCbCrFullRange)
    {
        tMrvMiCtrl.eMrvMifSpInRange = eMrvMifCol_Range_Full;    //predefined for best JPEG
    }
    else
    {
        tMrvMiCtrl.eMrvMifSpInRange = eMrvMifCol_Range_Std;    //BT601 conform
    }
    
    // additional settings specific for self path fed from ISP
    if (ulSelfF & MRV_DPD_ENABLE)
    {
        switch (ulSelfF & MRV_DPD_MODE__MASK)
        {
#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
            case MRV_DPD_MODE_DPCC: 
                break;
#endif                
            case MRV_DPD_MODE_ISPYC:
                // only allowed case, just proceed
                break;
            default:
                DBG_OUT( (DERR, "MrvSls_DatapathIsp: self path mode not supported for data coming from the ISP\n") );
                return RET_NOTSUPP;
        }
    }
    // SELF path   
    //////////////////////////////////////////
   
   
    //////////////////////////////////////////
    // Datapath multiplexers
    iRes = MrvSls_CalcDpMuxSettings(&tMrvMiCtrl, &eChnMode, &eDpSwitch);
    if (iRes != RET_SUCCESS) {
        return iRes;
    }
    // Datapath multiplexers
    //////////////////////////////////////////


    // hardcoded global settings of the memory interface
#ifdef WORKAROUND_RGB565_SEQUENCE_BUG
    if ((ulSelfF & MRV_DPD_ENABLE) &&
        ((ulSelfF & MRV_DPD_HWRGB__MASK) == MRV_DPD_HWRGB_565))
    {
        tMrvMiCtrl.bByteSwapEnable  = TRUE;
    } else {
        tMrvMiCtrl.bByteSwapEnable  = FALSE;
    }
#else    
    tMrvMiCtrl.bByteSwapEnable  = FALSE;
#endif    
    tMrvMiCtrl.eInitVals        = eMrvMif_InitOffsAndBase;
    
    // If we reach this point, we have collected all values to program the 
    // MARVIN for the requested datapath setup. Now all we've left to do is
    // apply these to MARVINs register set. For this, we mostly use the low level
    // MARVIN driver routines. 

    iRes = MrvSls_SetIspWindows(ptSysConf->ptIsiConfig, &tWndBlackline, &tWndZoomCrop);
    if( iRes != RET_SUCCESS ) {
        DBG_OUT( (DERR, "ERR: MrvSls_DatapathIsp: failed to set ISP window configuration\n") );
        return iRes;
    }   
    iRes = MrvSetDataPath(eChnMode, eDpSwitch);
    if( iRes != RET_SUCCESS ) {
        return iRes;
    }
    iRes = MrvSetMipiSmia(ptSysConf->ptIsiConfig->ulMode);
    if( iRes != RET_SUCCESS ) {
        return iRes;
    }
    MrvSmlOutSetPath( tMrvMiCtrl.eMainPath );
    if (tMrvMiCtrl.eSelfPath != eMrvPathOff) {
        MrvResSetSelfResize( &tScaleS, eMrvCfgUpdateImmediate, MrvSls_GetRszLut(ulSelfF) );
    }
    if (tMrvMiCtrl.eMainPath != eMrvPathOff) {
        MrvResSetMainResize( &tScaleM, eMrvCfgUpdateImmediate, MrvSls_GetRszLut(ulMainF) );
    }      
    // the DMA-read mode multiplexer is set to eMrvDmaRd_off by default
    MrvSetDmaReadMode(eMrvDmaRd_off, eMrvCfgUpdateImmediate);   
    iRes = MrvMif_SetPathAndOrientation( &tMrvMiCtrl );
    if (iRes != RET_SUCCESS) {
        DBG_OUT( (DERR, "ERR: MrvSls_DatapathIsp: failed to set MI path and orientation\n") );
        return iRes;
    }
    
    //here the extended YCbCr mode is configured
    if(ptSysConf->ptConfig->Flags.fYCbCrFullRange)
    {
        (void)MrvSetExtYCMode();
    }
    else
    {
        (void)MrvSetYCMode();    
    }
    if( iRes != RET_SUCCESS ) {
        DBG_OUT( (DERR, "ERR: MrvSls_DatapathIsp: failed to set ISP YCbCr extended mode\n") );
        return iRes;
    }   
   
    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_DatapathDmaRead \n
 *  \RETURNVALUE RET_SUCCESS in case of success \n
 *  \PARAMETERS  ptSysConf - pointer to the system configuration \n
 *               ptSource  - pointer to the description for the input data
 *                           format. This includes frame size
 *                           only. The ulFlags field is not used in here. \n
 *               ptMain    - pointer to the data path description for the main
 *                           data path. \n
 *               ptSelf    - pointer to the data path description for the self
 *                           data path. \n
 *                        
 *  \DESCRIPTION Configures main and self data pathes and scaler for data
 *               coming from the from system memory via the DMA read feature. \n
 *
 *  Following MARVIN subsystems are programmed: \n
 *     - DMA-read multiplexer \n
 *     - Main & Self path resizer \n
 *     - Memory Interface (MI) input source, en/disable and data format \n
 *     - Main path multiplexer (only in case of DMA-read routed through ISP
 *       path) \n
 *     - YC-Splitter (only in case of DMA-read routed through ISP path) \n
 *
 *  Following MARVIN subsystems are *NOT* programmed: \n
 *     - All ISP functionality \n
 *     - Color Processing block \n
 *     - JPEG encode subsystem (quantisation tables etc.) \n
 *     - Memory Interface (MI) output buffer addresses and sizes \n
 */
/*****************************************************************************/
RESULT MrvSls_DatapathDmaRead( const tsPlSystemConfig *ptSysConf,
                               const tsMrvDatapathDesc *ptSource,
                               const tsMrvDatapathDesc *ptMain, 
                               const tsMrvDatapathDesc *ptSelf )
{
    RESULT iRes;
    
    // copy of flags for main and self path to simplify access (no pointer de-reference)
    UINT32 ulMainF;
    UINT32 ulSelfF;
    
    // things to apply to MARVIN
    tsMrvMiCtrl tMrvMiCtrl;
    tsMrvMiDmaConf tMrvMiDmaConf;
    tsMrvScale tScaleS;
    tsMrvScale tScaleM;
    teMrvDmaReadMode eDmaRdModeS;
    teMrvDmaReadMode eDmaRdModeM;
    teMrvYcsChnMode eChnMode;
    teMrvDpSwitch eDpSwitch;
    
    PRE_CONDITION(ptSource != NULL);
    
    // assume dapapath deactivation for not provided descriptors 
    ulMainF = 0;
    ulSelfF = 0;
    if (ptMain) {
        ulMainF = ptMain->ulFlags;
    }
    if (ptSelf) {
        ulSelfF = ptSelf->ulFlags;
    }
    
    // initialize variables on the stack
    memset( &tMrvMiCtrl, 0, sizeof(tsMrvMiCtrl) );    
    memset( &tMrvMiDmaConf, 0, sizeof(tMrvMiDmaConf) );    
    eDmaRdModeS = eMrvDmaRd_off;
    eDmaRdModeM = eMrvDmaRd_off;
    eChnMode = eMrvYcsOff;
    eDpSwitch = eMrvDpMv;

    //////////////////////////////////////////
    // MAIN path - fill in eMainPath, tScaleM  and ptMainRszLut
    // basic selfpath settings
    iRes = MrvSls_CalcMainPathSettings(ptSource, ptMain, &tScaleM, &tMrvMiCtrl);
    if (iRes != RET_SUCCESS) {
        return iRes;
    }
    // additional settings specific for main path fed from ISP
    if (ulMainF & MRV_DPD_ENABLE)
    {
        switch (ulMainF & MRV_DPD_MODE__MASK)
        {
            case MRV_DPD_MODE_DMAYC_ISP: //no break
            case MRV_DPD_MODE_DMAJPEG_ISP:
                eDmaRdModeM = eMrvDmaRd_IePath;
                tMrvMiDmaConf.bViaCamPath = TRUE;
                break;
            case MRV_DPD_MODE_DMAJPEG_DIRECT:
                eDmaRdModeM = eMrvDmaRd_JpgEnc;
                break;
            default:
                DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: main path mode not supported for data coming from system memory (DMA-read)\n") );
                return RET_NOTSUPP;
        }
        if( ptMain != NULL )    // changed to avoid LINT warnings (Warning 613)
        {
            if (ulMainF & MRV_DPD_KEEPRATIO)
            {
                UINT32 ulAspectSource = (0x1000 * ((UINT32)ptSource->usOutW)) / ptSource->usOutH;
                UINT32 ulAspectTarget = (0x1000 * ((UINT32)(ptMain->usOutW))) / ptMain->usOutH;
                if (ulAspectSource != ulAspectTarget) {
                    DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: no cropping available to adjust main path aspect ratio mismatch\n") );
                    return RET_NOTSUPP;            
                }
            }
        }
        else
        {
            ASSERT( ptMain != NULL );
        }
    }            
    // MAIN path   
    //////////////////////////////////////////


    //////////////////////////////////////////
    // SELF path - fill in eSelfPath & tScaleS & self path related members of tMrvMiCtrl    
    // basic selfpath settings
    iRes = MrvSls_CalcSelfPathSettings(ptSource, ptSelf, &tScaleS, &tMrvMiCtrl);
    if (iRes != RET_SUCCESS) {
        return iRes;
    }
    //here the extended YCbCr mode is configured
    if(ptSysConf->ptConfig->Flags.fYCbCrNonCosited)
    {
        tMrvMiCtrl.eMrvMifSpInPhase = eMrvMifCol_Phase_Non_Cosited;    //predefined for best JPEG
    }
    else
    {
        tMrvMiCtrl.eMrvMifSpInPhase = eMrvMifCol_Phase_Cosited;    //BT601 conform
    }
    if(ptSysConf->ptConfig->Flags.fYCbCrFullRange)
    {
        tMrvMiCtrl.eMrvMifSpInRange = eMrvMifCol_Range_Full;    //predefined for best JPEG
    }
    else
    {
        tMrvMiCtrl.eMrvMifSpInRange = eMrvMifCol_Range_Std;    //BT601 conform
    }
    // additional settings specific for self path fed from DMA-read path
    if (ulSelfF & MRV_DPD_ENABLE)
    {
        switch (ulSelfF & MRV_DPD_MODE__MASK)
        {
            case MRV_DPD_MODE_DMAYC_DIRECT:
                eDmaRdModeS = eMrvDmaRd_SelfPath;
                break;
            case MRV_DPD_MODE_DMAYC_ISP:
                eDmaRdModeS = eMrvDmaRd_IePath;
                tMrvMiDmaConf.bViaCamPath = TRUE;
                break;
            default:
                DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: self path mode not supported for data coming from system memory (DMA-read)\n") );
                return RET_NOTSUPP;
        }
        if( ptSelf != NULL )    // changed to avoid LINT warnings (Warning 613)
        {
            if (ulSelfF & MRV_DPD_KEEPRATIO)
            {
                UINT32 ulAspectSource = (0x1000 * ((UINT32)ptSource->usOutW)) / ptSource->usOutH;
                UINT32 ulAspectTarget = (0x1000 * ((UINT32)(ptSelf->usOutW))) / ptSelf->usOutH;
                if (ulAspectSource != ulAspectTarget) {
                    DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: no cropping available to adjust self path aspect ration mismatch\n") );
                    return RET_NOTSUPP;            
                }
            }
        }
        else
        {
            ASSERT( ptSelf != NULL );
        }
    }
    // SELF path   
    //////////////////////////////////////////


    //////////////////////////////////////////
    // MI settings    
    // cross check and selection for DMA-read source data format
    {
        UINT32 ulFlags;
        if (ulSelfF & MRV_DPD_ENABLE)
        {
            ulFlags = ulSelfF;
            if ((ulMainF & MRV_DPD_ENABLE) &&
                (ulMainF & MRV_DPD_DMA_IN__MASK) != (ulSelfF & MRV_DPD_DMA_IN__MASK))
            {
                DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: DMA-read source data color format for main and self path do not match\n") );
                return RET_NOTSUPP;            
            }
        }
        else
        {
            ulFlags = ulMainF;
        }
        switch (ulFlags & MRV_DPD_DMA_IN__MASK)
        {
            case MRV_DPD_DMA_IN_444: tMrvMiDmaConf.eMrvMifColFormat = eMrvMifCol_Format_YCbCr444; break;
            case MRV_DPD_DMA_IN_422: tMrvMiDmaConf.eMrvMifColFormat = eMrvMifCol_Format_YCbCr422; break;
            case MRV_DPD_DMA_IN_420: tMrvMiDmaConf.eMrvMifColFormat = eMrvMifCol_Format_YCbCr420; break;
            case MRV_DPD_DMA_IN_411: // no break, does not seem to be supported by HW
            default:
                DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: DMA-read source data color format not supported\n") );
                return RET_NOTSUPP;                        
        }
    }
    // hardcoded global settings of the memory interface
    tMrvMiCtrl.bByteSwapEnable  = FALSE;
    tMrvMiCtrl.eInitVals        = eMrvMif_InitOffsAndBase;
    tMrvMiDmaConf.bByteSwapEnable   = FALSE;
    tMrvMiDmaConf.bContinuousEnable = FALSE;
    if(ulSelfF & MRV_DPD_INTERLEAVED)
    {
    	tMrvMiDmaConf.eMrvMifPicForm = eMrvMif_PicForm_Interleaved;
    }
    else
    {
        tMrvMiDmaConf.eMrvMifPicForm    = eMrvMif_PicForm_Planar;    
    }

    // MI settings
    //////////////////////////////////////////


    //////////////////////////////////////////
    // Datapath multiplexers

    // cross check for DMA-read mode
    if ((eDmaRdModeM != eMrvDmaRd_off) &&
        (eDmaRdModeS != eMrvDmaRd_off) &&
        (eDmaRdModeM != eDmaRdModeS))
    {
        DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: DMA-read mode for main and self path do not match\n") );
        return RET_NOTSUPP;            
    }
    // select single DMA-read mode to use
    if (eDmaRdModeM != eMrvDmaRd_off) {
        eDmaRdModeS = eDmaRdModeM;
    } else {
        eDmaRdModeM = eDmaRdModeS;
    }

    if (eDmaRdModeM == eMrvDmaRd_IePath)
    {
        iRes = MrvSls_CalcDpMuxSettings(&tMrvMiCtrl, &eChnMode, &eDpSwitch);
        if (iRes != RET_SUCCESS) {
            return iRes;
        }
    }
    // Datapath multiplexers
    //////////////////////////////////////////


    // If we reach this point, we have collected all values to program the 
    // MARVIN for the requested DMA-read setup. Now all we've left to do is
    // apply these to MARVINs register set. For this, we use the low level
    // MARVIN driver routines.
    
    if (tMrvMiCtrl.eSelfPath != eMrvPathOff) {
        MrvResSetSelfResize( &tScaleS, eMrvCfgUpdateImmediate, MrvSls_GetRszLut(ulSelfF) );
    }
    if (tMrvMiCtrl.eMainPath != eMrvPathOff) {
        MrvResSetMainResize( &tScaleM, eMrvCfgUpdateImmediate, MrvSls_GetRszLut(ulMainF) );
    } 
    MrvSetDmaReadMode( eDmaRdModeM, eMrvCfgUpdateImmediate );
    if (eDmaRdModeM == eMrvDmaRd_IePath)
    {   // if the DMA-read path is configured to replace the ISP data input, we have
        // to set the Y/C-Splitter and the main path multiplexer too.
        iRes = MrvSetDataPath(eChnMode, eDpSwitch);
        if (iRes != RET_SUCCESS) {
            DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: failed to set main datapath multiplexers\n") );
            return iRes;
        }
    }
    if (eDmaRdModeM == eMrvDmaRd_SelfPath)
    {   // if the data from the system memory is to be routed directly to the self path scaler, 
        // there may be activity on the main path at the same time too (simultaneous capturing of
        // frames coming from the ISP). In that case, we are not allowed to change the MI configuration
        // for the main path, so we overwrite the selfpath-related parts only.
        tsMrvMiCtrl tCurrentMiCtrl;
                
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        // //////////////////
        // Special handling for MI version 2:
        // If DMA-read is to be routed directly to the self path, the self path MUST NOT be
        // enabled. Otherwise, a frame end condition from the DMA-read data would trigger
        // the CIF-frame end interrupt too. The data is processed even if the path is disabled,
        // because setting the dma_sp_enable bit (which starts the DMA transfer) automatically
        // enables the self path regardless of the cif_sp_enable bit.
        tMrvMiCtrl.eSelfPath = eMrvPathOff;        
        // <end of workaround>
        // //////////////////
#endif //if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        
        // read currently active settings from MI...
        iRes = MrvMif_GetPathAndOrientation( &tCurrentMiCtrl );
        if (iRes == RET_SUCCESS)
        {
            // ... and change the selfpath related only
            tCurrentMiCtrl.eMrvMifSpInRange = tMrvMiCtrl.eMrvMifSpInRange;
            tCurrentMiCtrl.eMrvMifSpInPhase = tMrvMiCtrl.eMrvMifSpInPhase;
            tCurrentMiCtrl.eMrvMifSpOutForm = tMrvMiCtrl.eMrvMifSpOutForm;
            tCurrentMiCtrl.eMrvMifSpInForm  = tMrvMiCtrl.eMrvMifSpInForm;
            tCurrentMiCtrl.eMrvMifSpPicForm = tMrvMiCtrl.eMrvMifSpPicForm;
            tCurrentMiCtrl.eMrvMifSpMode    = tMrvMiCtrl.eMrvMifSpMode;
            tCurrentMiCtrl.eSelfPath        = tMrvMiCtrl.eSelfPath;            
            iRes = MrvMif_SetPathAndOrientation( &tCurrentMiCtrl );
        }
    }
    else
    {    
        iRes = MrvMif_SetPathAndOrientation( &tMrvMiCtrl );
    }
    if (iRes != RET_SUCCESS) {
        DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: failed to set MI path and orientation\n") );
        return iRes;
    }
    iRes = MrvMif_SetDmaConfig( &tMrvMiDmaConf );
    if (iRes != RET_SUCCESS) {
        DBG_OUT( (DERR, "ERR: MrvSls_DatapathDmaRead: failed to set MI DMA config\n") );
        return iRes;
    }   
    
    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION     MrvSls_AlignPixel \n
 *  \RETURNVALUE  RET_SUCCESS - everything okay \n
 *                RET_FAILURE - one or more errors occured, but the value(s)
 *                              were corrected to the allowed range(s) \n
 *  \PARAMETERS   piX   - pointer to current x-offset \n
 *                piY   - pointer to current y-offset \n
 *                ptWnd - pointer to currently used window \n
 *
 *  \DESCRIPTION  Checks the range of the parameters and corrects the values,
 *                if necessary. \n
 */
/*****************************************************************************/
static RESULT MrvSls_AlignPixel(int* piX, int* piY, const tsMrvWindow* ptWnd)
{
   RESULT iRes = RET_SUCCESS;
   if (*piX < 0) { 
      iRes = RET_FAILURE;  // input pixel is left beneth output coordinate system
      *piX = 0;  // -> proceed anyway by changing the pixel so it is inside again
   }
   if (*piY < 0) {
      iRes = RET_FAILURE;  // input pixel is above output coordinate system
      *piY = 0;  // -> proceed anyway by changing the pixel so it is inside again
   }
   if (*piX >= ptWnd->usHSize) { 
      iRes = RET_FAILURE;  // input pixel is right beneth output coordinate system
      *piX = ptWnd->usHSize - 1;  // -> proceed anyway by changing the pixel so it is inside again
   }
   if (*piY >= ptWnd->usVSize) { 
      iRes = RET_FAILURE;  // input pixel is below output coordinate system
      *piY = ptWnd->usVSize - 1;  // -> proceed anyway by changing the pixel so it is inside again
   }   
   return iRes;   
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_GetScaledValue \n
 *  \RETURNVALUE scaled value \n
 *  \PARAMETERS  usIn    - input value (X or Y component of pixel \n
 *               usScale - scaler factor value from the tsMrvScale or
 *                         tsMrvSScale structures \n
 *
 *  \DESCRIPTION Calculates the output value for a given input and scaler value. \n
 *               The formulas are taken from the MARVIN / MARVIN3PLUS user
 *               manuals (fixed-point calculation using 32 bit during
 *               processing) \n
 */
/*****************************************************************************/
static UINT16 MrvSls_GetScaledValue( UINT16 usIn, UINT32 ulScale )
{  
   if (usIn == 0)
   {  // zero is special case, scale factor does not matter
      return 0;
   } 
   else if (ulScale == RSZ_SCALER_BYPASS)
   {  // no scaling at all
      return usIn;
   }
   else if (ulScale & RSZ_UPSCALE_ENABLE)
   {  // upscaling
      return (UINT16)((((UINT32)(usIn-1) * RSZ_SCALER_BYPASS) / ulScale) + 1);              // changed to avoid LINT warnings (Info 734)
   }
   else
   {  // downscaling
      return (UINT16)((((UINT32)(usIn-1) * (UINT32)(ulScale-1)) / RSZ_SCALER_BYPASS) + 1);  // changed to avoid LINT warnings (Info 734)
   }
}
   

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_Transform_Cam2X \n
 *  \RETURNVALUE RET_SUCCESS in case of success \n
 *               RET_FAILURE if the output window is no exact representation
 *                           of the input because of cropping \n
 *               In both cases, the output coordinates are valid. \n
 *  \PARAMETERS  ptWndIn   - pointer to window coordinates given in the camera
 *                           resolution coordinate system. \n
 *               ptWndOut  - pointer to a variable to receive the window
 *                           coordinates transformed to those of the main data
 *                           path output. \n
 *               bMainPath - TRUE for main path, FALSE for self path \n
 *
 *  \DESCRIPTION If you need a coordinate resp. window position expressed in
 *               pixel positions of the main or self data path output, but have them only
 *               in pixel positions of the camera input, probably this routine is
 *               what you need.  \n
 *
 *  \NOTES  <ul>
 *          <li> Calculation is done based on the settings applied by the 
 *               last call of the MrvSls_DatapathIsp() routine. \n
 *          <li> A digital Zoom applied with the iZoom parameter of the
 *               MrvSls_DatapathIsp() routine is not taken into account here,
 *               i.e. a changed iZoom value would lead to different output
 *               coordinates even if the input coordinates remain constant. If
 *               this is not as intended, transform the camera coordinates to
 *               ISP coordinates with the MrvSls_Transform_VCam2Cam() routine
 *               before feeding them into this routine.\n
 *          </ul>
 */
/*****************************************************************************/
static RESULT MrvSls_Transform_Cam2X( const tsMrvWindow* ptWndIn,
                                      tsMrvWindow* ptWndOut,
                                      BOOL bMainPath)
{
   tsMrvWindow tIspWnd;
   int iX1, iY1, iX2, iY2;
   UINT32 ulScaleHY, ulScaleVY;
   RESULT iRes = RET_SUCCESS;
   
   // points of input window
   iX1 = ptWndIn->usHOffs; 
   iY1 = ptWndIn->usVOffs; 
   iX2 = ptWndIn->usHOffs + ptWndIn->usHSize - 1; 
   iY2 = ptWndIn->usVOffs + ptWndIn->usVSize - 1; 
   
   // get the current values from MARVIN
   MrvSls_GetIspWindows(NULL, &tIspWnd);
   if (bMainPath)
   {
      tsMrvScale tMainScale;
      MrvResGetMainResize(&tMainScale);
      ulScaleHY = tMainScale.ulScaleHY;
      ulScaleVY = tMainScale.ulScaleVY;
   }
   else
   {
      tsMrvScale tSelfScale;
      MrvResGetSelfResize(&tSelfScale);
      ulScaleHY = tSelfScale.ulScaleHY;
      ulScaleVY = tSelfScale.ulScaleVY;
   }
   
   // make the pixels relative to the upper left corner of the isp window
   iX1 -= tIspWnd.usHOffs;
   iY1 -= tIspWnd.usVOffs;
   iX2 -= tIspWnd.usHOffs;
   iY2 -= tIspWnd.usVOffs;
   // put both pixels inside isp window if they are located outside
   iRes |= MrvSls_AlignPixel(&iX1, &iY1, &tIspWnd);
   iRes |= MrvSls_AlignPixel(&iX2, &iY2, &tIspWnd);
   
   // handle scaling
   ptWndOut->usHOffs = MrvSls_GetScaledValue((UINT16)iX1, ulScaleHY);                           // changed to avoid LINT warnings (Info 734)
   ptWndOut->usVOffs = MrvSls_GetScaledValue((UINT16)iY1, ulScaleVY);                           // changed to avoid LINT warnings (Info 734)
   ptWndOut->usHSize = (MrvSls_GetScaledValue((UINT16)iX2, ulScaleHY) - ptWndOut->usHOffs) + 1; // changed to avoid LINT warnings (Info 734)
   ptWndOut->usVSize = (MrvSls_GetScaledValue((UINT16)iY2, ulScaleVY) - ptWndOut->usVOffs) + 1; // changed to avoid LINT warnings (Info 734)
   
   return iRes;   
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_Transform_Cam2Main \n
 *  \RETURNVALUE RET_SUCCESS in case of success \n
 *               RET_FAILURE if the output window is no exact representation
 *                           of the input because of cropping \n
 *               In both cases, the output coordinates are valid. \n
 *  \PARAMETERS  ptWndIn  - pointer to window coordinates given in the camera
 *                          resolution coordinate system. \n
 *               ptWndOut - pointer to a variable to receive the window
 *                          coordinates transformed to those of the main
 *                          data path output. \n
 *
 *  \DESCRIPTION If you need a coordinate resp. window position expressed in
 *               pixel positions of the main data path output, but have them
 *               only in pixel positions of the camera input, probably this
 *               routine is what you need. \n
 */
/*****************************************************************************/
RESULT MrvSls_Transform_Cam2Main( const tsMrvWindow* ptWndIn,
                                  tsMrvWindow* ptWndOut)
{
   return MrvSls_Transform_Cam2X(ptWndIn, ptWndOut, TRUE);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_Transform_Cam2Self \n
 *  \RETURNVALUE RET_SUCCESS in case of success \n
 *               RET_FAILURE if the output window is no exact representation
 *                           of the input because of cropping \n
 *               In both cases, the output coordinates are valid. \n
 *  \PARAMETERS  ptWndIn  - pointer to window coordinates given in the camera
 *                          resolution coordinate system. \n
 *               ptWndOut - pointer to a variable to receive the window
 *                          coordinates transformed to those of the self
 *                          data path output. \n
 *
 *  \DESCRIPTION If you need a coordinate resp. window position expressed in
 *               pixel positions of the self data path output, but have them
 *               only in pixel positions of the camera input, probably this
 *               routine is what you need. \n
 */
/*****************************************************************************/
RESULT MrvSls_Transform_Cam2Self( const tsMrvWindow* ptWndIn,
                                  tsMrvWindow* ptWndOut)
{
   return MrvSls_Transform_Cam2X(ptWndIn, ptWndOut, FALSE);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSls_Transform_VCam2Cam \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  ptIsiSensorConfig - pointer to current sensor configuration \n
 *               ptWndIn           - pointer to window coordinates given in
 *                                   the virtual camera resolution coordinate
 *                                   system. \n
 *               ptWndOut - pointer to a variable to receive the window
 *                         coordinates transformed to those of the real
 *                         camera resolution \n
 *                        
 *  \DESCRIPTION If you need a coordinate resp. window position expressed in
 *               pixel positions of the real camera coordinate, but have them
 *               only in pixel positions of the virtual camera input, probably
 *               this routine is what you need. \n
 *
 *  \NOTES  <ul>
 *          <li> Calculation is done based on the settings applied by the 
 *               last call of the MrvSls_DatapathIsp() routine. \n
 *          <li> Only digital Zoom applied with the iZoom parameter of the
 *               MrvSls_DatapathIsp() routine is taken into account here.
 *               The scale factors of main and self scaler are not of
 *               interest. \n
 *          <li> There's no clipping of the coordinate range. The output
 *               coordinates may be outside the frame size delivered from
 *               the sensor. \n
 *          </ul>
 */
/*****************************************************************************/
void MrvSls_Transform_VCam2Cam( const tsIsiSensorConfig *ptIsiSensorConfig,
                                const tsMrvWindow* ptWndIn,
                                tsMrvWindow* ptWndOut)
{
   tsMrvWindow tIspWnd;
   UINT32 ulX1, ulY1, ulX2, ulY2;   // changed to avoid LINT warnings (Warning 573)
   UINT32 ulOfsX, ulOfsY;           // changed to avoid LINT warnings (Info 737)
   UINT32 ulZoom;
   UINT16 usIsiX, usIsiY;
   
   // points of input window
   ulX1 = ptWndIn->usHOffs; 
   ulY1 = ptWndIn->usVOffs; 
   ulX2 = ptWndIn->usHOffs + ptWndIn->usHSize - 1; 
   ulY2 = ptWndIn->usVOffs + ptWndIn->usVSize - 1; 
   
   // get the current values from MARVIN
   MrvSls_GetIspWindows(NULL, &tIspWnd);
   (void)IsiRes2Size(ptIsiSensorConfig->ulRes, &usIsiX, &usIsiY );  // changed to avoid LINT warnings (Warning 534)
   
   // figure out wether the picture is horizontally or vertically cropped
   // and calcuöate the zoom factor (>1 if picture is being zoomed).
   {
      UINT32 ulAspectCam = (FIXEDPOINT_ONE * ((UINT32)usIsiX)) / usIsiY;
      UINT32 ulAspectVCam = (FIXEDPOINT_ONE * ((UINT32)(tIspWnd.usHSize))) / tIspWnd.usVSize;
      if (ulAspectCam < ulAspectVCam)
      {  // real camera aspect is more 'portrait-like' as virtual aspect. We assume cropping
         // by cutting off a bit of the top & bottom of the real camera frame, so we better use
         // the width for calculating the zoom factor
         ulZoom = (FIXEDPOINT_ONE * ((UINT32)usIsiX)) / tIspWnd.usHSize;
      }
      else
      {  // real camera aspect is more 'landscape-like' as virtual aspect. We assume cropping
         // by cutting off a bit of the left and right of the real camera frame, so we better use
         // the height for calculating the zoom factor
         ulZoom = (FIXEDPOINT_ONE * ((UINT32)usIsiY)) / tIspWnd.usVSize;
      }         
   }
      
   // scale down all pixel positions
   ulX1 = (ulX1 * FIXEDPOINT_ONE) / ulZoom;
   ulY1 = (ulY1 * FIXEDPOINT_ONE) / ulZoom;
   ulX2 = (ulX2 * FIXEDPOINT_ONE) / ulZoom;
   ulY2 = (ulY2 * FIXEDPOINT_ONE) / ulZoom;
   
   // calculate the zoom-related offsets 
   ulOfsX = ( usIsiX - (((UINT32)usIsiX * FIXEDPOINT_ONE) / ulZoom) ) / 2;
   ulOfsY = ( usIsiY - (((UINT32)usIsiY * FIXEDPOINT_ONE) / ulZoom) ) / 2;
   
   // care about additional blacklines at image start
   switch( ptIsiSensorConfig->ulBls )
   {
      case ISI_BLS_OFF: break; //no black lines
      case ISI_BLS_TWO_LINES: ulOfsY += 2; break; //two black lines at frame start
      case ISI_BLS_SIX_LINES: ulOfsY += 6; break; //six black lines at frame start
      case ISI_BLS_FOUR_LINES: ulOfsY += 2; break; //two black lines at frame start and two at the end
      default:
         DBG_OUT( (DERR, "MrvSls_Transform_VCam2Cam: unsupported black lines seting in sensor config\n") );
         break;
   }

   // apply zoom-related offsets
   ulX1 += ulOfsX;
   ulY1 += ulOfsY;
   ulX2 += ulOfsX;
   ulY2 += ulOfsY;

   // write back to output window
   ptWndOut->usHOffs = (UINT16)ulX1;    // changed to avoid LINT warnings (Info 734)
   ptWndOut->usVOffs = (UINT16)ulY1;    // changed to avoid LINT warnings (Info 734)
   ptWndOut->usHSize = (UINT16)((ulX2 - ulX1) + 1);     // changed to avoid LINT warnings (Info 834)
   ptWndOut->usVSize = (UINT16)((ulY2 - ulY1) + 1);     // changed to avoid LINT warnings (Info 834)
}

#endif //if MRV_SUPPORT_SL

#ifdef SL_SUPPORT_PARTLY
////////////////////////////////////////////////////////////////////////
// extra functions
////////////////////////////////////////////////////////////////////////


/*****************************************************************************/
/*!
 *  \FUNCTION    MiscSetupExtMemory \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  iWidth .. picture width in pixels \n
 *               iHeight .. picture height in pixels \n
 *               eBufType .. buffer type \n
 *               ptMrvMiPathConf .. pointer to variable to receive the 
 *                  buffer addresses and sizes. \n
 *  \DESCRIPTION calculates buffer addresses (and sizes) in the
 *               unmanaged external memory. \n
 */
/*****************************************************************************/
void MiscSetupExtMemory(int iWidth, int iHeight, teBufConfig eBufType, tsMrvMiPathConf* ptMrvMiPathConf, unsigned addr )
{
   UINT32 ulSize = (UINT32)iWidth * (UINT32)iHeight;
   
   // initialize buffer descriptor triplet
   memset( ptMrvMiPathConf, 0, sizeof(tsMrvMiPathConf) );
   
   ptMrvMiPathConf->ulLLength    = (UINT32)iWidth;
   ptMrvMiPathConf->ulYPicWidth  = ptMrvMiPathConf->ulLLength;
   ptMrvMiPathConf->ulYPicHeight = (UINT32)iHeight;
   // Y buffer always starts at the start of the external memory
   ptMrvMiPathConf->tYBuffer.pucBuffer = (UINT8*)addr;
   
   switch (eBufType)
   {
      case eBufType_8bitBayer:
         ptMrvMiPathConf->tYBuffer.ulSize = ulSize;
         break;
      case eBufType_16bitBayer:
         ptMrvMiPathConf->tYBuffer.ulSize = ulSize * 2;
         break;
      case eBufType_Jpeg:
         ptMrvMiPathConf->tYBuffer.ulSize = MEM_SNAPSHOT_MAX_SIZE;   // this size is normally far too much, but we are on the safe side...
         break;
      case eBufType_Yuv420:
         ptMrvMiPathConf->tYBuffer.ulSize = ulSize;
         ptMrvMiPathConf->tCbBuffer.pucBuffer = ptMrvMiPathConf->tYBuffer.pucBuffer + ptMrvMiPathConf->tYBuffer.ulSize;
         ptMrvMiPathConf->tCbBuffer.ulSize = ulSize / 4;
         ptMrvMiPathConf->tCrBuffer.pucBuffer = ptMrvMiPathConf->tCbBuffer.pucBuffer + ptMrvMiPathConf->tCbBuffer.ulSize;
         ptMrvMiPathConf->tCrBuffer.ulSize = ulSize / 4;
         break;
      case eBufType_Yuv422:
         ptMrvMiPathConf->tYBuffer.ulSize = ulSize;
         ptMrvMiPathConf->tCbBuffer.pucBuffer = ptMrvMiPathConf->tYBuffer.pucBuffer + ptMrvMiPathConf->tYBuffer.ulSize;
         ptMrvMiPathConf->tCbBuffer.ulSize = ulSize / 2;
         ptMrvMiPathConf->tCrBuffer.pucBuffer = ptMrvMiPathConf->tCbBuffer.pucBuffer + ptMrvMiPathConf->tCbBuffer.ulSize;
         ptMrvMiPathConf->tCrBuffer.ulSize = ulSize / 2;
         break;
      default:
#ifdef _DEBUG
         ASSERT(0);
#else
         break;
#endif
   }
}

#endif    // #ifdef SL_SUPPORT_PARTLY
//#endif //if MRV_SUPPORT_SL




