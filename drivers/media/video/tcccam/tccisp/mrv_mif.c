/*****************************************************************************/
/*!
 *  \file        mrv_mif.c \n
 *  \version     1.0 \n
 *  \author      Friebe \n
 *  \brief       functions to access the Marvin memory interface \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_mif.c 432 2009-06-30 09:48:59Z neugebaa $ \n
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


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information


#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
static teMrvMifColFormat s_eMrvMifColFormat_DMA = eMrvMifCol_Format_YCbCr422;
static teMrvMifPicForm   s_eMrvMifPicForm_DMA   = eMrvMif_PicForm_Planar;
#endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)


#if (MARVIN_FEATURE_MI < MARVIN_FEATURE_MI_V3)
static teMrvMifColFormat s_eMrvMifSpOutForm_SP  = eMrvMifCol_Format_YCbCr422;
static teMrvMifPicForm   s_eMrvMifPicForm_SP    = eMrvMif_PicForm_Planar;
static UINT32            s_ulMrvMifSpYPicWidth  = 0;
static UINT32            s_ulMrvMifSpYPicHeight = 0;
#endif  // #if (MARVIN_FEATURE_MI < MARVIN_FEATURE_MI_V3)


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSetMainBuffer \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  pointer to buffer descriptors of Y, Cb and Cr buffer
 *               enum for immediate, frame synchronous, or later (external) update \n
 *
 *  \NOTES       Update of the shadow registers is not independent of
 *               the self buffer. Calling MrvMifSetMainBuffer also
 *               updates the shadow registers of the self buffer as
 *               a side effect. \n
 *
 *  \DESCRIPTION writes the contents of the buffer descriptors to
 *               the appropriate Marvin registers \n
 */
/*****************************************************************************/
void MrvMifSetMainBuffer(const tsMrvBufferOld *ptYBuffer, const tsMrvBufferOld *ptCbBuffer, const tsMrvBufferOld *ptCrBuffer, teMrvConfUpdateTime eUpdateTime)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // check range 32, 24 bit and alignment (lower 2 bits must be zero)
    ASSERT(((UINT32)ptYBuffer->pucBuffer        & ~(MRV_MI_MP_Y_BASE_AD_INIT_VALID_MASK))   == 0);
    ASSERT((        ptYBuffer->ulSize           & ~(MRV_MI_MP_Y_SIZE_INIT_VALID_MASK))      == 0);
    ASSERT((        ptYBuffer->ulOffs           & ~(MRV_MI_MP_Y_OFFS_CNT_INIT_VALID_MASK))  == 0);
    ASSERT((        ptYBuffer->ulIrqOffsLLength & ~(MRV_MI_MP_Y_IRQ_OFFS_INIT_VALID_MASK))  == 0);
    ASSERT(((UINT32)ptCbBuffer->pucBuffer       & ~(MRV_MI_MP_CB_BASE_AD_INIT_VALID_MASK))  == 0);
    ASSERT((        ptCbBuffer->ulSize          & ~(MRV_MI_MP_CB_SIZE_INIT_VALID_MASK))     == 0);
    ASSERT((        ptCbBuffer->ulOffs          & ~(MRV_MI_MP_CB_OFFS_CNT_INIT_VALID_MASK)) == 0);
    ASSERT(((UINT32)ptCrBuffer->pucBuffer       & ~(MRV_MI_MP_CR_BASE_AD_INIT_VALID_MASK))  == 0);
    ASSERT((        ptCrBuffer->ulSize          & ~(MRV_MI_MP_CR_SIZE_INIT_VALID_MASK))     == 0);
    ASSERT((        ptCrBuffer->ulOffs          & ~(MRV_MI_MP_CR_OFFS_CNT_INIT_VALID_MASK)) == 0);
    
    REG_SET_SLICE(ptMrvReg->mi_mp_y_base_ad_init,   MRV_MI_MP_Y_BASE_AD_INIT,  (UINT32)ptYBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_mp_y_size_init,      MRV_MI_MP_Y_SIZE_INIT,             ptYBuffer->ulSize);
    REG_SET_SLICE(ptMrvReg->mi_mp_y_offs_cnt_init,  MRV_MI_MP_Y_OFFS_CNT_INIT,         ptYBuffer->ulOffs);
    REG_SET_SLICE(ptMrvReg->mi_mp_y_irq_offs_init,  MRV_MI_MP_Y_IRQ_OFFS_INIT,         ptYBuffer->ulIrqOffsLLength);
    
    REG_SET_SLICE(ptMrvReg->mi_mp_cb_base_ad_init,  MRV_MI_MP_CB_BASE_AD_INIT, (UINT32)ptCbBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_mp_cb_size_init,     MRV_MI_MP_CB_SIZE_INIT,            ptCbBuffer->ulSize);
    REG_SET_SLICE(ptMrvReg->mi_mp_cb_offs_cnt_init, MRV_MI_MP_CB_OFFS_CNT_INIT,        ptCbBuffer->ulOffs);
    
    REG_SET_SLICE(ptMrvReg->mi_mp_cr_base_ad_init,  MRV_MI_MP_CR_BASE_AD_INIT, (UINT32)ptCrBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_mp_cr_size_init,     MRV_MI_MP_CR_SIZE_INIT,            ptCrBuffer->ulSize);
    REG_SET_SLICE(ptMrvReg->mi_mp_cr_offs_cnt_init, MRV_MI_MP_CR_OFFS_CNT_INIT,        ptCrBuffer->ulOffs);
    
    // update base and offset registers during next immidiate or automatic update request
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_BASE,   ON);
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_OFFSET, ON);
    
	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        //frame synchronous update of shadow registers, update is done after the current frame
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        //immediate update of shadow registers (will disturb an ongoing frame processing)
        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
#else   // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(ptYBuffer);      // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(ptCbBuffer);     // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(ptCrBuffer);     // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(eUpdateTime);    // added to avoid LINT warnings (Info 715)
    ASSERT(0);  // not supported
#endif  // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSetSelfBuffer \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  pointer to buffer descriptors of Y, Cb and Cr buffer
 *               enum for immediate, frame synchronous, or later (external) update \n
 *
 *  \NOTES       Update of the shadow registers is not independent of
 *               the main buffer. Calling MrvMifSetSelfBuffer also
 *               updates the shadow registers of the main buffer as
 *               a side effect. \n
 *
 *  \DESCRIPTION writes the contents of the buffer descriptors to
 *               the appropriate Marvin registers \n
 */
/*****************************************************************************/
void MrvMifSetSelfBuffer(const tsMrvBufferOld *ptYBuffer, const tsMrvBufferOld *ptCbBuffer, const tsMrvBufferOld *ptCrBuffer, teMrvConfUpdateTime eUpdateTime)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // check range 32, 24 bit and alignment (lower 2 bits must be zero)
    ASSERT(((UINT32)ptYBuffer->pucBuffer        & ~(MRV_MI_SP_Y_BASE_AD_INIT_VALID_MASK))   == 0);
    ASSERT((        ptYBuffer->ulSize           & ~(MRV_MI_SP_Y_SIZE_INIT_VALID_MASK))      == 0);
    ASSERT((        ptYBuffer->ulOffs           & ~(MRV_MI_SP_Y_OFFS_CNT_INIT_VALID_MASK))  == 0);
    ASSERT((        ptYBuffer->ulIrqOffsLLength & ~(MRV_MI_SP_Y_LLENGTH_VALID_MASK))        == 0);
    ASSERT(((UINT32)ptCbBuffer->pucBuffer       & ~(MRV_MI_SP_CB_BASE_AD_INIT_VALID_MASK))  == 0);
    ASSERT((        ptCbBuffer->ulSize          & ~(MRV_MI_SP_CB_SIZE_INIT_VALID_MASK))     == 0);
    ASSERT((        ptCbBuffer->ulOffs          & ~(MRV_MI_SP_CB_OFFS_CNT_INIT_VALID_MASK)) == 0);
    ASSERT(((UINT32)ptCrBuffer->pucBuffer       & ~(MRV_MI_SP_CR_BASE_AD_INIT_VALID_MASK))  == 0);
    ASSERT((        ptCrBuffer->ulSize          & ~(MRV_MI_SP_CR_SIZE_INIT_VALID_MASK))     == 0);
    ASSERT((        ptCrBuffer->ulOffs          & ~(MRV_MI_SP_CR_OFFS_CNT_INIT_VALID_MASK)) == 0);
    
    REG_SET_SLICE(ptMrvReg->mi_sp_y_base_ad_init,   MRV_MI_SP_Y_BASE_AD_INIT,  (UINT32)ptYBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_sp_y_size_init,      MRV_MI_SP_Y_SIZE_INIT,             ptYBuffer->ulSize);
    REG_SET_SLICE(ptMrvReg->mi_sp_y_offs_cnt_init,  MRV_MI_SP_Y_OFFS_CNT_INIT,         ptYBuffer->ulOffs);
    REG_SET_SLICE(ptMrvReg->mi_sp_y_llength,        MRV_MI_SP_Y_LLENGTH,               ptYBuffer->ulIrqOffsLLength);
    
    REG_SET_SLICE(ptMrvReg->mi_sp_cb_base_ad_init,  MRV_MI_SP_CB_BASE_AD_INIT, (UINT32)ptCbBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_sp_cb_size_init,     MRV_MI_SP_CB_SIZE_INIT,            ptCbBuffer->ulSize);
    REG_SET_SLICE(ptMrvReg->mi_sp_cb_offs_cnt_init, MRV_MI_SP_CB_OFFS_CNT_INIT,        ptCbBuffer->ulOffs);
    
    REG_SET_SLICE(ptMrvReg->mi_sp_cr_base_ad_init,  MRV_MI_SP_CR_BASE_AD_INIT, (UINT32)ptCrBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_sp_cr_size_init,     MRV_MI_SP_CR_SIZE_INIT,            ptCrBuffer->ulSize);
    REG_SET_SLICE(ptMrvReg->mi_sp_cr_offs_cnt_init, MRV_MI_SP_CR_OFFS_CNT_INIT,        ptCrBuffer->ulOffs);
    
    // update base and offset registers during next immidiate or automatic update request
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_BASE,   ON);
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_OFFSET, ON);

	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        //frame synchronous update of shadow registers, update is made after the current frame
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        //immediate update of shadow registers, disturb the current picture frame operation
        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_IN,  ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
//    MrvReg_DumpAll( stdout );
#else   // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(ptYBuffer);      // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(ptCbBuffer);     // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(ptCrBuffer);     // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(eUpdateTime);    // added to avoid LINT warnings (Info 715)
    ASSERT(0);  // not supported
#endif  // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSetDmaBuffer \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  pointer to buffer descriptors of Y, Cb and Cr buffer \n
 *               bDmaByteSwap:  true    first pixel is expected in bits 31:24 \n
 *                              false   first pixel is expected in bits 7:0 \n
 *
 *  \NOTES       To start the DMA function MrvMifStartDma is used.  \n
 *               Use function MrvSetDmaReadMode() to select DMA path. \n
 *
 *  \DESCRIPTION writes the contents of the buffer descriptors to
 *               the appropriate Marvin registers \n
 */
/*****************************************************************************/
void MrvMifSetDmaBuffer(const tsMrvDmaBuffer *ptYBuffer, const tsMrvDmaBuffer *ptCbBuffer, const tsMrvDmaBuffer *ptCrBuffer, BOOL bDmaByteSwap)
{
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    // check range
    ASSERT(((UINT32)ptYBuffer->pucBuffer   & ~(MRV_MI_DMA_Y_PIC_START_AD_MASK))  == 0);
    ASSERT((        ptYBuffer->ulPicSize   & ~(MRV_MI_DMA_Y_PIC_SIZE_MASK))      == 0);
    ASSERT((        ptYBuffer->ulPicWidth  & ~(MRV_MI_DMA_Y_PIC_WIDTH_MASK))     == 0);
    ASSERT((        ptYBuffer->ulLLength   & ~(MRV_MI_DMA_Y_LLENGTH_MASK))       == 0);
    ASSERT(((UINT32)ptCbBuffer->pucBuffer  & ~(MRV_MI_DMA_CB_PIC_START_AD_MASK)) == 0);
    ASSERT((        ptCbBuffer->ulPicSize  & ~(MRV_MI_DMA_CB_PIC_SIZE_MASK))     == 0);
    ASSERT((        ptCbBuffer->ulPicWidth & ~(MRV_MI_DMA_CB_PIC_WIDTH_MASK))    == 0);
    ASSERT((        ptCbBuffer->ulLLength  & ~(MRV_MI_DMA_CB_LLENGTH_MASK))      == 0);
    ASSERT(((UINT32)ptCrBuffer->pucBuffer  & ~(MRV_MI_DMA_CR_PIC_START_AD_MASK)) == 0);
    ASSERT((        ptCrBuffer->ulPicSize  & ~(MRV_MI_DMA_CR_PIC_SIZE_MASK))     == 0);
    ASSERT((        ptCrBuffer->ulPicWidth & ~(MRV_MI_DMA_CR_PIC_WIDTH_MASK))    == 0);
    ASSERT((        ptCrBuffer->ulLLength  & ~(MRV_MI_DMA_CR_LLENGTH_MASK))      == 0);

    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_start_ad,  MRV_MI_DMA_Y_PIC_START_AD,  (UINT32)ptYBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_size,      MRV_MI_DMA_Y_PIC_SIZE,              ptYBuffer->ulPicSize);
    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_width,     MRV_MI_DMA_Y_PIC_WIDTH,             ptYBuffer->ulPicWidth);
    REG_SET_SLICE(ptMrvReg->mi_dma_y_llength,       MRV_MI_DMA_Y_LLENGTH,               ptYBuffer->ulLLength);
    
    REG_SET_SLICE(ptMrvReg->mi_dma_cb_pic_start_ad, MRV_MI_DMA_CB_PIC_START_AD, (UINT32)ptCbBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_dma_cb_pic_size,     MRV_MI_DMA_CB_PIC_SIZE,             ptCbBuffer->ulPicSize);
    REG_SET_SLICE(ptMrvReg->mi_dma_cb_pic_width,    MRV_MI_DMA_CB_PIC_WIDTH,            ptCbBuffer->ulPicWidth);
    REG_SET_SLICE(ptMrvReg->mi_dma_cb_llength,      MRV_MI_DMA_CB_LLENGTH,              ptCbBuffer->ulLLength);
    
    REG_SET_SLICE(ptMrvReg->mi_dma_cr_pic_start_ad, MRV_MI_DMA_CR_PIC_START_AD, (UINT32)ptCrBuffer->pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_dma_cr_pic_size,     MRV_MI_DMA_CR_PIC_SIZE,             ptCrBuffer->ulPicSize);
    REG_SET_SLICE(ptMrvReg->mi_dma_cr_pic_width,    MRV_MI_DMA_CR_PIC_WIDTH,            ptCrBuffer->ulPicWidth);
    REG_SET_SLICE(ptMrvReg->mi_dma_cr_llength,      MRV_MI_DMA_CR_LLENGTH,              ptCrBuffer->ulLLength);
    
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_DMA_BYTE_SWAP, (bDmaByteSwap) ? ON : OFF);
    // ON:  first pixel is expected in bits 31:24 of a memory word
    // OFF: first pixel is expected in bits 7:0 of a memory word

#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(ptYBuffer);      // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(ptCbBuffer);     // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(ptCrBuffer);     // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(bDmaByteSwap);   // added to avoid LINT warnings (Info 715)
    ASSERT(0);  // not supported
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifStartDma \n
 *  \RETURNVALUE RET_SUCCESS - after a new DMA start \n
 *               RET_BUSY    - the DMA path is currently in use \n
 *               RET_NOTSUPP - no DMA read feature available \n
 *  \PARAMETERS  none \n
 *               
 *  \NOTES       Call the function \ref MrvMif_SetDmaBuffer() first. \n
 *               
 *  \DESCRIPTION Start the DMA transfer. \n
 */
/*****************************************************************************/
RESULT MrvMifStartDma(void)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulEofIrqMask = MrvGetFrameEndIrqMask_DMA();
    
  #if ( (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    if (REG_GET_SLICE(ptMrvReg->mi_dma_status, MRV_MI_DMA_ACTIVE))
  #elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    if (REG_GET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_DMA_SP_EN))
  #else
    #error "unknown value for MARVIN_FEATURE_MI"
  #endif  // MARVIN_FEATURE_MI
    { // last DMA frame is not finished
        DBG_OUT( (DERR, "MrvMifStartDma: DMA is busy\n") );
        return RET_BUSY;
    }

    // clear frame end interrupt 
    REG_WRITE(ptMrvReg->mi_icr, ulEofIrqMask);
    
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     REG_SET_SLICE(ptMrvReg->mi_dma_start, MRV_MI_DMA_START, ON);
  #elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_DMA_SP_EN, ENABLE);
  #endif // MARVIN_FEATURE_MI
    
    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSetPath \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  discrete values to control the dat path \n
 *               tMainPath: eMrvPathRaw16 bypass JPEG encoder \n
 *                          eMrvPathRaw bypass JPEG encoder \n
 *                          eMrvPathJpe choose JPEG encoder output \n
 *                          eMrvPathOff turn main picture path off \n
 *               tSelfPath: eMrvPathOn  turn self picture path on \n
 *                          eMrvPathOff turn self picture path off \n
 *               flag to enable byte swapping \n
 *
 *  \NOTES       data path switching requires immediate update \n
 *
 *  \DESCRIPTION sets the Marvin memory interface path \n
 */
/*****************************************************************************/
void MrvMifSetPath(teMrvPath tMainPath, teMrvPath tSelfPath, BOOL bByteSwap)
{
    UNUSED_PARAM(tMainPath);      // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(tSelfPath);      // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(bByteSwap);      // added to avoid LINT warnings (Info 715)
    ASSERT("MrvMifSetPath()" == "MI version not supported");
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifMainSkip \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  bInitOffset= \n
 *               FALSE: offset counters for all main buffers are set to
 *                      the beginning of the current (skipped) frame \n
 *               TRUE:  offset counters are resetted to the programmed
 *                      values in MI_MP_Y/CB/CR_OFFS \n
 *
 *  \NOTES       For frame skipping the memory interface discards all
 *               remaining pixels of the current frame at its main
 *               picture input ports. It completes the current burst
 *               transfer and flushes all main picture internal data
 *               FIFOs. Frame skipping does not affect processing of
 *               the self-picture, and both main and self-picture
 *               frame end interrupt generation. \n
 *
 *  \DESCRIPTION Skips a frame by resetting the buffer pointers to the
 *               start of the last frame. Offsets are processed according
 *               to bInitOffset. \n
 */
/*****************************************************************************/
void MrvMifMainSkip(BOOL bInitOffset)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_init = REG_READ(ptMrvReg->mi_init);
    
 #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_OFFSET_EN, (bInitOffset) ? ON : OFF);
    REG_SET_SLICE(mi_init, MRV_MI_MI_SKIP, ON);
    REG_SET_SLICE(mi_init, MRV_MI_MI_CFG_UPD, ON);
    REG_WRITE(ptMrvReg->mi_init, mi_init);
#elif ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2))
    REG_SET_SLICE(mi_init, MRV_MI_MI_SKIP, ON);
    REG_SET_SLICE(mi_init, MRV_MI_INIT_OFFSET, (bInitOffset) ? ON : OFF);
    REG_WRITE(ptMrvReg->mi_init, mi_init);
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    ASSERT("MrvMifMainSkip()" == "MI version not supported");
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSelfMirror \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  flag mirroring enabled or disabled \n
 *               enum for immediate, frame synchronous, or later (external) update \n
 *
 *  \NOTES       If mirorring is enabled the self-picture line length
 *               defined in MI_SP_LLGENTH must be 8 pixel aligned
 *               which gives a 32 bit alignment for the chrominance
 *               data and luminance and chrominance buffer sizes
 *               (set in MI_SP_Y_SIZE, MI_SP_CB_SIZE, and MI_SP_CR_SIZE)
 *               must be integer multiple of the line length as well. \n
 *
 *  \DESCRIPTION turns horizontal mirroring of the self picture on/off \n
 */
/*****************************************************************************/
void MrvMifSelfMirror(BOOL bMirror, teMrvConfUpdateTime eUpdateTime)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);

    REG_SET_SLICE(mi_ctrl, MRV_MI_H_FLIP, (bMirror) ? ON : OFF);

    switch (eUpdateTime)
    {
    case eMrvCfgUpdateFrameSync:
        // frame synchronous update of shadow registers
        REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
        break;
    case eMrvCfgUpdateImmediate:
        // immediate update of shadow registers
        REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
        REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_IN,  ON);
        REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
        break;
    case eMrvCfgUpdateLater:
        // no update from within this function (but write mirror flag)
        REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
        break;
    default:
        break;
    }
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(bMirror);        // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(eUpdateTime);    // added to avoid LINT warnings (Info 715)
    ASSERT("MrvMifSelfMirror()" == "MI version not supported");
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSemiPlanar \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  BOOL \n
 *
 *  \DESCRIPTION if bSet = TRUE set semi planar mode
 *               back to 0 \n
 */
/*****************************************************************************/
void MrvMifSemiPlanar(BOOL bSet)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);

    REG_SET_SLICE(mi_ctrl, MRV_MI_SEMI_PLANAR_EN, (bSet) ? ON : OFF);
   
    // immediate update of shadow registers
    REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
    REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_IN,  ON);

    REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(bSet);       // added to avoid LINT warnings (Info 715)
    ASSERT("MrvMifSemiPlanar()" == "MI version not supported");
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
}   


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifResetOffsets \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  enum for immediate, frame synchronous, or later (external) update \n
 *
 *  \DESCRIPTION sets all main picture and self picture buffer offsets
 *               back to 0 \n
 */
/*****************************************************************************/
void MrvMifResetOffsets(teMrvConfUpdateTime eUpdateTime)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    REG_SET_SLICE(ptMrvReg->mi_mp_y_offs_cnt_init,  MRV_MI_MP_Y_OFFS_CNT_INIT,  0);
    REG_SET_SLICE(ptMrvReg->mi_mp_cb_offs_cnt_init, MRV_MI_MP_CB_OFFS_CNT_INIT, 0);
    REG_SET_SLICE(ptMrvReg->mi_mp_cr_offs_cnt_init, MRV_MI_MP_CR_OFFS_CNT_INIT, 0);
    
    REG_SET_SLICE(ptMrvReg->mi_sp_y_offs_cnt_init,  MRV_MI_SP_Y_OFFS_CNT_INIT,  0);
    REG_SET_SLICE(ptMrvReg->mi_sp_cb_offs_cnt_init, MRV_MI_SP_CB_OFFS_CNT_INIT, 0);
    REG_SET_SLICE(ptMrvReg->mi_sp_cr_offs_cnt_init, MRV_MI_SP_CR_OFFS_CNT_INIT, 0);
    
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     // immediate update of shadow registers (will disturb an ongoing frame processing)
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_OFFSET_EN, ON);
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_BASE_EN,   ON);  // [hf]: why ???
    
	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
		// not supported on MI V3 - [hf]: ???
        //// frame synchronous update of shadow registers, update is done after the current frame
        //REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        // update base and offset registers during next immediate or automatic update request
        REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
#elif ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2))
    // update offset registers during next immediate or automatic update request
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_OFFSET, ON);
    
	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        // frame synchronous update of shadow registers, update is done after the current frame
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        // immediate update of shadow registers (will disturb an ongoing frame processing)
        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    ASSERT("MrvMifResetOffsets()" == "MI version not supported");
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSetRgb565 \n
 *
 *  \RETURNVALUE none \n
 *  \PARAMETERS  bool for switching on/off \n
 *
 *  \DESCRIPTION sets the rgb hw conversion \n
 */
/*****************************************************************************/
void MrvMifSetRgb565(BOOL bSet, BOOL bTypeIn444)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);
    
    if (bSet)
    {
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_565);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
    }
    else
    {
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, DISABLE);
    }

    #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_INPUT_FORMAT, (bTypeIn444) ? MRV_MI_RGB_INPUT_FORMAT_YUV444 : MRV_MI_RGB_INPUT_FORMAT_YUV422);
    #else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        if (bTypeIn444)
        {
            ASSERT("MrvMifSetRgb565()" == "Input type 444 not supported in MI_V1");
        }
        //REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_INPUT_FORMAT, MRV_MI_RGB_INPUT_FORMAT_YUV422);  not defined in MI_V1
    #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)

        REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_OUT, ON); // soft_upd_out
        REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_IN,  ON); // soft_upd_in

        REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);

#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(bSet);       // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(bTypeIn444); // added to avoid LINT warnings (Info 715)
    ASSERT("MrvMifSetRgb565()" == "MI version not supported");
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSetRgb666 \n
 *
 *  \RETURNVALUE none \n
 *  \PARAMETERS  bool for switching on/off \n
 *
 *  \DESCRIPTION sets the rgb hw conversion \n
 */
/*****************************************************************************/
void MrvMifSetRgb666(BOOL bSet, BOOL bTypeIn444)
{
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);

    if (bSet)
    {
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_666);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
    }
    else
    {
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, DISABLE);
    }

    REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_INPUT_FORMAT, (bTypeIn444) ? MRV_MI_RGB_INPUT_FORMAT_YUV444 : MRV_MI_RGB_INPUT_FORMAT_YUV422);

    REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_OUT, ON); // soft_upd_out
    REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_IN,  ON); // soft_upd_in

    REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(bSet);       // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(bTypeIn444); // added to avoid LINT warnings (Info 715)
    ASSERT("MrvMifSetRgb666()" == "MI version not supported");
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifSetRgb888 \n
 *
 *  \RETURNVALUE none \n
 *  \PARAMETERS  bool for switching on/off \n
 *
 *  \DESCRIPTION sets the rgb hw conversion \n
 */
/*****************************************************************************/
void MrvMifSetRgb888(BOOL bSet, BOOL bTypeIn444)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);
    
    if (bSet)
    {
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_888);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
    }
    else
    {
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, DISABLE);
    }

    #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_INPUT_FORMAT, (bTypeIn444) ? MRV_MI_RGB_INPUT_FORMAT_YUV444 : MRV_MI_RGB_INPUT_FORMAT_YUV422);
    #else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        if (bTypeIn444)
        {
            ASSERT("MrvMifSetRgb565()" == "Input type 444 not supported in MI_V1");
        }
        //REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_INPUT_FORMAT, MRV_MI_RGB_INPUT_FORMAT_YUV422);  not defined in MI_V1
    #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)

        REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_OUT, ON); // soft_upd_out
        REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_IN,  ON); // soft_upd_in

        REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);

#else   // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(bSet);       // added to avoid LINT warnings (Info 715)
    UNUSED_PARAM(bTypeIn444); // added to avoid LINT warnings (Info 715)
    ASSERT("MrvMifSetRgb888()" == "MI version not supported");
#endif  // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifGetByteCnt \n
 *  \RETURNVALUE Byte Count of last data transfer \n
 *
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION This function get the byte count from the last JPEG
 *               or raw data transfer \n
 */
/*****************************************************************************/
UINT32 MrvMifGetByteCnt(void)
{
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

   return (UINT32)REG_GET_SLICE(ptMrvReg->mi_byte_cnt, MRV_MI_BYTE_CNT);
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMifGetPixelCnt \n
 *  \RETURNVALUE Pixel Count of last data transfer \n
 *
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION This function get the pixel count from the last dpcc transfer \n
 */
/*****************************************************************************/
UINT32 MrvMifGetPixelCnt(void)
{
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

   return (UINT32)REG_GET_SLICE(ptMrvReg->mi_pixel_cnt, MRV_MI_BYTE_CNT);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_SetSelfPicOrientation \n
 *
 *  \RETURNVALUE RET_SUCCESS - the desired self picture orientation is set \n
 *               RET_NOTSUPP - this feature is not supported by this MARVIN
 *                             version \n
 *
 *  \PARAMETERS  tMrvMifSpMode     - desired self picture orientation \n
 *               bActivateSelfPath - if the self picture path should be
 *                                      activated after configuration \n
 *
 *  \DESCRIPTION Sets the desired self picture orientation, if possible. \n
 *
 *  \NOTES       This function should be only called in the case that the
 *               self picture path processes currently no data. Otherwise it
 *               will result in unpredictable behaviour of the MI. \n
 *               This function will exchange the register values of
 *               mi_sp_y_pic_width and mi_sp_y_pic_height, according to the
 *               desired picture orientation. Therefore this function has
 *               to be called \b after every call of MrvMif_SetSelfBuffer(). \n
 */
/*****************************************************************************/
RESULT MrvMif_SetSelfPicOrientation(teMrvMifSpMode tMrvMifSpMode)
{
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);
    
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     UINT32 ulOutputFormat =  REG_GET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT);
  #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    // apply the desired self picture orientation, if possible
    switch (tMrvMifSpMode)
    {
    case eMrvMifSp_Original:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, 0);
    	break;

    case eMrvMifSp_Horizontal_Flip:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, MRV_MI_ROT_AND_FLIP_H_FLIP);
    	break;
        
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     case eMrvMifSp_Vertical_Flip:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, MRV_MI_ROT_AND_FLIP_V_FLIP);
    	break;

    case eMrvMifSp_Rotation_090_deg:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, MRV_MI_ROT_AND_FLIP_ROTATE);
    	break;

    case eMrvMifSp_Rotation_180_deg:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, MRV_MI_ROT_AND_FLIP_H_FLIP | MRV_MI_ROT_AND_FLIP_V_FLIP);
    	break;

    case eMrvMifSp_Rotation_270_deg:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, MRV_MI_ROT_AND_FLIP_H_FLIP | MRV_MI_ROT_AND_FLIP_V_FLIP | MRV_MI_ROT_AND_FLIP_ROTATE);
    	break;

    case eMrvMifSp_Rot_090_V_Flip:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, MRV_MI_ROT_AND_FLIP_V_FLIP | MRV_MI_ROT_AND_FLIP_ROTATE);
    	break;

    case eMrvMifSp_Rot_270_V_Flip:
        REG_SET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP, MRV_MI_ROT_AND_FLIP_H_FLIP | MRV_MI_ROT_AND_FLIP_ROTATE);
    	break;
  #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    default:
        DBG_OUT((DERR, "MrvMif_SetSelfPicOrientation(): unknown value for tMrvMifSpMode\n"));
        return RET_NOTSUPP;
    }

#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     if (REG_GET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP) & MRV_MI_ROT_AND_FLIP_ROTATE)
    {
        switch (ulOutputFormat)
        {
        case MRV_MI_SP_OUTPUT_FORMAT_RGB888:
        case MRV_MI_SP_OUTPUT_FORMAT_RGB666:
        case MRV_MI_SP_OUTPUT_FORMAT_RGB565:
            break; // rotation supported on this output modes
        default:
            DBG_OUT((DERR, "MrvMif_SetSelfPicOrientation(): rotation is only allowed for RGB modes.\n"));
            return RET_NOTSUPP;
        }
    }
  #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    REG_SET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
    REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
  #elif ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, ON);
  #endif // #if (MARVIN_FEATURE_MI)

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_MI >= V2 && <= V3)
    UNUSED_PARAM(tMrvMifSpMode);
    UNUSED_PARAM(bActivateSelfPath);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_MI >= V2 && <= V3)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_CheckMiPathConf \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_OUTOFRANGE   - one or more of the structure members are
 *                                  out of range \n
 *               RET_SUCCESS      - the desired settings have been applied \n
 *
 *  \PARAMETERS  ptMrvMiPathConf  - pointer to tsMrvMiPathConf structure \n
 *
 *  \DESCRIPTION Checks the main or self picture path buffer structure. \n
 */
/*****************************************************************************/
static RESULT MrvMif_CheckMiPathConf(const tsMrvMiPathConf *ptMrvMiPathConf, BOOL bMainBuffer)
{
    // check pointers
    if (!ptMrvMiPathConf)
    {
        DBG_OUT((DERR, "MrvMif_CheckMiPathConf(): ptMrvMiPathConf is NULL\n"));
        return RET_NULL_POINTER;
    }

    if (!ptMrvMiPathConf->tYBuffer.pucBuffer)
    {
        DBG_OUT( (DERR, "MrvMif_CheckMiPathConf(): ptMrvMiPathConf->tYBuffer.pucBuffer is NULL\n") );
        return RET_NULL_POINTER;
    }

    // Y buffer: check ranges and alignment
    if (bMainBuffer)
    {
        if ((((UINT32)ptMrvMiPathConf->tYBuffer.pucBuffer & ~(MRV_MI_MP_Y_BASE_AD_INIT_VALID_MASK))  != 0) ||
            ((        ptMrvMiPathConf->tYBuffer.ulSize    & ~(MRV_MI_MP_Y_SIZE_INIT_VALID_MASK))     != 0) ||
            ((        ptMrvMiPathConf->tYBuffer.ulSize    &  (MRV_MI_MP_Y_SIZE_INIT_VALID_MASK))     == 0) ||
            ((        ptMrvMiPathConf->tYBuffer.ulOffs    & ~(MRV_MI_MP_Y_OFFS_CNT_INIT_VALID_MASK)) != 0)   )
        { // value is zero or at least one bit outside *_VALID_MASK is set ('1')
            DBG_OUT((DERR, "MrvMif_CheckMiPathConf(): Y buffer value out of range.\n"));
            return RET_OUTOFRANGE;
        }
    }
    else // (bMainBuffer)
    {
        if ((((UINT32)ptMrvMiPathConf->tYBuffer.pucBuffer & ~(MRV_MI_SP_Y_BASE_AD_INIT_VALID_MASK))  != 0) ||
            ((        ptMrvMiPathConf->tYBuffer.ulSize    & ~(MRV_MI_SP_Y_SIZE_INIT_VALID_MASK))     != 0) ||
            ((        ptMrvMiPathConf->tYBuffer.ulSize    &  (MRV_MI_SP_Y_SIZE_INIT_VALID_MASK))     == 0) ||
            ((        ptMrvMiPathConf->tYBuffer.ulOffs    & ~(MRV_MI_SP_Y_OFFS_CNT_INIT_VALID_MASK)) != 0) ||
            ((        ptMrvMiPathConf->ulLLength          & ~(MRV_MI_SP_Y_LLENGTH_VALID_MASK))       != 0) ||
            ((        ptMrvMiPathConf->ulLLength          &  (MRV_MI_SP_Y_LLENGTH_VALID_MASK))       == 0)   )
        { // value is zero or at least one bit outside *_VALID_MASK is set ('1')
            DBG_OUT((DERR, "MrvMif_CheckMiPathConf(): Y buffer value out of range.\n"));
            return RET_OUTOFRANGE;
        }
    }

    if (ptMrvMiPathConf->tCbBuffer.pucBuffer != 0)
    { // Cb buffer: check ranges and alignment
        if (bMainBuffer)
        {
            if ((((UINT32)ptMrvMiPathConf->tCbBuffer.pucBuffer & ~(MRV_MI_MP_CB_BASE_AD_INIT_VALID_MASK))  != 0) ||
                ((        ptMrvMiPathConf->tCbBuffer.ulSize    & ~(MRV_MI_MP_CB_SIZE_INIT_VALID_MASK))     != 0) ||
                ((        ptMrvMiPathConf->tCbBuffer.ulSize    &  (MRV_MI_MP_CB_SIZE_INIT_VALID_MASK))     == 0) ||
                ((        ptMrvMiPathConf->tCbBuffer.ulOffs    & ~(MRV_MI_MP_CB_OFFS_CNT_INIT_VALID_MASK)) != 0)   )
            { // value is zero or at least one bit outside *_VALID_MASK is set ('1')
                DBG_OUT((DERR, "MrvMif_CheckMiPathConf(): Cb buffer value out of range.\n"));
                return RET_OUTOFRANGE;
            }
        }
        else // (bMainBiffer)
        {
            if ((((UINT32)ptMrvMiPathConf->tCbBuffer.pucBuffer & ~(MRV_MI_SP_CB_BASE_AD_INIT_VALID_MASK))  != 0) ||
                ((        ptMrvMiPathConf->tCbBuffer.ulSize    & ~(MRV_MI_SP_CB_SIZE_INIT_VALID_MASK))     != 0) ||
                ((        ptMrvMiPathConf->tCbBuffer.ulSize    &  (MRV_MI_SP_CB_SIZE_INIT_VALID_MASK))     == 0) ||
                ((        ptMrvMiPathConf->tCbBuffer.ulOffs    & ~(MRV_MI_SP_CB_OFFS_CNT_INIT_VALID_MASK)) != 0)   )
            { // value is zero or at least one bit outside *_VALID_MASK is set ('1')
                DBG_OUT((DERR, "MrvMif_CheckMiPathConf(): Cb buffer value out of range.\n"));
                return RET_OUTOFRANGE;
            }
        }
    }

    if (ptMrvMiPathConf->tCrBuffer.pucBuffer != 0)
    { // Cr buffer: check ranges and alignment
        if (bMainBuffer)
        {
            if ((((UINT32)ptMrvMiPathConf->tCrBuffer.pucBuffer & ~(MRV_MI_MP_CR_BASE_AD_INIT_VALID_MASK))  != 0) ||
                ((        ptMrvMiPathConf->tCrBuffer.ulSize    & ~(MRV_MI_MP_CR_SIZE_INIT_VALID_MASK))     != 0) ||
                ((        ptMrvMiPathConf->tCrBuffer.ulSize    &  (MRV_MI_MP_CR_SIZE_INIT_VALID_MASK))     == 0) ||
                ((        ptMrvMiPathConf->tCrBuffer.ulOffs    & ~(MRV_MI_MP_CR_OFFS_CNT_INIT_VALID_MASK)) != 0)   )
            { // value is zero or at least one bit outside *_VALID_MASK is set ('1')
                DBG_OUT((DERR, "MrvMif_CheckMiPathConf(): Cr buffer value out of range.\n"));
                return RET_OUTOFRANGE;
            }
        }
        else // (bMainBiffer)
        {
            if ((((UINT32)ptMrvMiPathConf->tCrBuffer.pucBuffer & ~(MRV_MI_SP_CR_BASE_AD_INIT_VALID_MASK))  != 0) ||
                ((        ptMrvMiPathConf->tCrBuffer.ulSize    & ~(MRV_MI_SP_CR_SIZE_INIT_VALID_MASK))     != 0) ||
                ((        ptMrvMiPathConf->tCrBuffer.ulSize    &  (MRV_MI_SP_CR_SIZE_INIT_VALID_MASK))     == 0) ||
                ((        ptMrvMiPathConf->tCrBuffer.ulOffs    & ~(MRV_MI_SP_CR_OFFS_CNT_INIT_VALID_MASK)) != 0)   )
            { // value is zero or at least one bit outside *_VALID_MASK is set ('1')
                DBG_OUT((DERR, "MrvMif_CheckMiPathConf(): Cr buffer value out of range.\n"));
                return RET_OUTOFRANGE;
            }
        }
    }

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_SetMainBuffer \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_OUTOFRANGE   - one or more of the structure members are
 *                                  out of range \n
 *               RET_SUCCESS      - the desired settings have been applied \n
 *
 *  \PARAMETERS  ptMrvMiPathConf  - pointer to tsMrvMiPathConf structure \n
 *               eUpdateTime      - enum for immediate, frame synchronous,
 *                                  or later (external) update \n
 *
 *  \DESCRIPTION Configures the main picture path buffers of the MI. \n
 *
 *  \NOTES       The bImmediate flag should be only set in the case that the
 *               MI processes currently no data. Otherwise it will result
 *               in unpredictable behaviour of the MI. \n
 */
/*****************************************************************************/
RESULT MrvMif_SetMainBuffer(const tsMrvMiPathConf *ptMrvMiPathConf, teMrvConfUpdateTime eUpdateTime)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    RESULT iError  = RET_FAILURE;

    iError = MrvMif_CheckMiPathConf(ptMrvMiPathConf, TRUE);
    if (iError != RET_SUCCESS)
    {
        DBG_OUT( (DWARN, "MrvMif_SetMainBuffer: MrvMif_CheckMiPathConf() returned 0x%08X\n", iError) );
        return iError;
    }
    
    // set register values
    REG_SET_SLICE(ptMrvReg->mi_mp_y_base_ad_init,  MRV_MI_MP_Y_BASE_AD_INIT, (UINT32)ptMrvMiPathConf->tYBuffer.pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_mp_y_size_init,     MRV_MI_MP_Y_SIZE_INIT,            ptMrvMiPathConf->tYBuffer.ulSize);
    REG_SET_SLICE(ptMrvReg->mi_mp_y_offs_cnt_init, MRV_MI_MP_Y_OFFS_CNT_INIT,        ptMrvMiPathConf->tYBuffer.ulOffs);      
    
    if( ptMrvMiPathConf->tCbBuffer.pucBuffer != 0 )
    {
        REG_SET_SLICE(ptMrvReg->mi_mp_cb_base_ad_init,  MRV_MI_MP_CB_BASE_AD_INIT, (UINT32)ptMrvMiPathConf->tCbBuffer.pucBuffer);
        REG_SET_SLICE(ptMrvReg->mi_mp_cb_size_init,     MRV_MI_MP_CB_SIZE_INIT,            ptMrvMiPathConf->tCbBuffer.ulSize);
        REG_SET_SLICE(ptMrvReg->mi_mp_cb_offs_cnt_init, MRV_MI_MP_CB_OFFS_CNT_INIT,        ptMrvMiPathConf->tCbBuffer.ulOffs);
    }
    
    if( ptMrvMiPathConf->tCrBuffer.pucBuffer != 0 )
    {
        REG_SET_SLICE(ptMrvReg->mi_mp_cr_base_ad_init,  MRV_MI_MP_CR_BASE_AD_INIT, (UINT32)ptMrvMiPathConf->tCrBuffer.pucBuffer);
        REG_SET_SLICE(ptMrvReg->mi_mp_cr_size_init,     MRV_MI_MP_CR_SIZE_INIT,            ptMrvMiPathConf->tCrBuffer.ulSize);
        REG_SET_SLICE(ptMrvReg->mi_mp_cr_offs_cnt_init, MRV_MI_MP_CR_OFFS_CNT_INIT,        ptMrvMiPathConf->tCrBuffer.ulOffs);
    }
    
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    // update base and offset registers during next immediate or automatic update request
#if(MARVIN_FEATURE_SMALL_OUTUNIT != MARVIN_FEATURE_EXIST)    
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_OFFSET_EN, ENABLE);
#else    
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_OFFSET_EN, DISABLE);
#endif    
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_BASE_EN,   ENABLE);

	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        // frame synchronous update of shadow registers, update is done after the current frame
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        // immediate update of shadow registers (will disturb an ongoing frame processing)
        REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    // update base and offset registers during next immediate or automatic update request
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_OFFSET, ON);
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_BASE,   ON);
    
	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        // frame synchronous update of shadow registers, update is done after the current frame
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        // immediate update of shadow registers (will disturb an ongoing frame processing)
        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    return iError;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_GetMainBuffer \n
 *  \RETURNVALUE RET_SUCCESS      - Everything is okay. \n
 *               RET_NULL_POINTER - Any of the given pointers is NULL. \n
 *  \PARAMETERS  Pointer to structure where to store the register values. \n
 *
 *  \DESCRIPTION Reads the current main picture buffer registers and returns
 *               them, if possible. \n
 */
/*****************************************************************************/
RESULT MrvMif_GetMainBuffer(tsMrvMiPathConf *ptMrvMiPathConf)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ptMrvMiPathConf)
    {
        memset(ptMrvMiPathConf, 0, sizeof(tsMrvMiPathConf));
        // get register values Y buffer
        ptMrvMiPathConf->tYBuffer.pucBuffer  = (UINT8 *)REG_GET_SLICE(ptMrvReg->mi_mp_y_base_ad_init,    MRV_MI_MP_Y_BASE_AD_INIT);
        ptMrvMiPathConf->tYBuffer.ulSize     =          REG_GET_SLICE(ptMrvReg->mi_mp_y_size_init,       MRV_MI_MP_Y_SIZE_INIT);
        ptMrvMiPathConf->tYBuffer.ulOffs     =          REG_GET_SLICE(ptMrvReg->mi_mp_y_offs_cnt_start,  MRV_MI_MP_Y_OFFS_CNT_START);
        // get register values Cb buffer
        ptMrvMiPathConf->tCbBuffer.pucBuffer = (UINT8 *)REG_GET_SLICE(ptMrvReg->mi_mp_cb_base_ad_init,   MRV_MI_MP_CB_BASE_AD_INIT);
        ptMrvMiPathConf->tCbBuffer.ulSize    =          REG_GET_SLICE(ptMrvReg->mi_mp_cb_size_init,      MRV_MI_MP_CB_SIZE_INIT);
        ptMrvMiPathConf->tCbBuffer.ulOffs    =          REG_GET_SLICE(ptMrvReg->mi_mp_cb_offs_cnt_start, MRV_MI_MP_CB_OFFS_CNT_START);
        // get register values Cr buffer
        ptMrvMiPathConf->tCrBuffer.pucBuffer = (UINT8 *)REG_GET_SLICE(ptMrvReg->mi_mp_cr_base_ad_init,   MRV_MI_MP_CR_BASE_AD_INIT);
        ptMrvMiPathConf->tCrBuffer.ulSize    =          REG_GET_SLICE(ptMrvReg->mi_mp_cr_size_init,      MRV_MI_MP_CR_SIZE_INIT);
        ptMrvMiPathConf->tCrBuffer.ulOffs    =          REG_GET_SLICE(ptMrvReg->mi_mp_cr_offs_cnt_start, MRV_MI_MP_CR_OFFS_CNT_START);
    }
    else
    {
        DBG_OUT((DERR, "MrvMif_GetMainBuffer(): ptMrvMiPathConf is NULL\n"));
        return RET_NULL_POINTER;
    }

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_DisableAllPaths \n
 *
 *  \RETURNVALUE none \n
 *
 *  \PARAMETERS  boolean value to decide if we have to wait for frame end \n
 *
 *  \DESCRIPTION Stops all paths of the MI. \n
 *
 *  \NOTES       The bImmediate flag should be only set in the case that the
 *               MI processes currently no data. Otherwise it will result
 *               in unpredictable behaviour of the MI. \n
 */
/*****************************************************************************/
void MrvMif_DisableAllPaths(BOOL bPerformWaitForFrameEnd)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);
    UINT32 ulEofIrqMask = MrvGetFrameEndIrqMask_ISP();

    if (bPerformWaitForFrameEnd)
    { // clear frame end interrupt first
        REG_WRITE(ptMrvReg->mi_icr, ulEofIrqMask);
    }

    // stop after the current picture frame operation
    REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE,   0x0);
    REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);

#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, ON);
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    if (bPerformWaitForFrameEnd)
    { // wait for frame end to be sure that the self picture path has finished
        (void)MrvWaitForFrameEnd();
    }
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_SetSelfBuffer \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_OUTOFRANGE   - one or more of the structure members are
 *                                  out of range \n
 *               RET_SUCCESS      - the desired settings have been applied \n
 *
 *  \PARAMETERS  ptMrvMiPathConf  - pointer to tsMrvMiPathConf structure \n
 *               eUpdateTime      - enum for immediate, frame synchronous,
 *                                  or later (external) update \n
 *
 *  \DESCRIPTION Configures the self picture path buffers of the MI. \n
 *
 *  \NOTES       Prerequisite to call this function is that the DMA read
 *               path was configured via MrvMif_SetPathAndOrientation() after
 *               any MARVIN reset. \n
 */
/*****************************************************************************/
RESULT MrvMif_SetSelfBuffer(const tsMrvMiPathConf *ptMrvMiPathConf, teMrvConfUpdateTime eUpdateTime)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    RESULT iError = RET_FAILURE;

    iError = MrvMif_CheckMiPathConf(ptMrvMiPathConf, FALSE);
    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetSelfBuffer(): MrvMif_CheckMiPathConf() returned 0x%08X\n", iError));
        return iError;
    }

    // set register values
    REG_SET_SLICE(ptMrvReg->mi_sp_y_base_ad_init,  MRV_MI_SP_Y_BASE_AD_INIT, (UINT32)ptMrvMiPathConf->tYBuffer.pucBuffer);
    REG_SET_SLICE(ptMrvReg->mi_sp_y_size_init,     MRV_MI_SP_Y_SIZE_INIT,            ptMrvMiPathConf->tYBuffer.ulSize);
    REG_SET_SLICE(ptMrvReg->mi_sp_y_offs_cnt_init, MRV_MI_SP_Y_OFFS_CNT_INIT,        ptMrvMiPathConf->tYBuffer.ulOffs);

#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    {
        UINT8 ucPp32B = 0;

        // ulLLength is counted in pixels, but this MARVIN version needs it in bytes for the register
        iError = MrvMif_GetPixelPer32BitOfLine(&ucPp32B, s_eMrvMifSpOutForm_SP, s_eMrvMifPicForm_SP, TRUE);
        if (iError != RET_SUCCESS)
        {
            return iError;
        }

        // ucPp32B is 1, 2, or 4 (0 only on error, 8 only on chrominance buffer possible)
        REG_SET_SLICE(ptMrvReg->mi_sp_y_llength, MRV_MI_SP_Y_LLENGTH, (ptMrvMiPathConf->ulLLength << 2) / ucPp32B);
    }
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    // ulLLength is counted in pixels and this value could be stored directly into the register
    REG_SET_SLICE(ptMrvReg->mi_sp_y_llength, MRV_MI_SP_Y_LLENGTH, ptMrvMiPathConf->ulLLength);
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)

    if (ptMrvMiPathConf->tCbBuffer.pucBuffer)
    {
        REG_SET_SLICE(ptMrvReg->mi_sp_cb_base_ad_init,  MRV_MI_SP_CB_BASE_AD_INIT, (UINT32)ptMrvMiPathConf->tCbBuffer.pucBuffer);
        REG_SET_SLICE(ptMrvReg->mi_sp_cb_size_init,     MRV_MI_SP_CB_SIZE_INIT,            ptMrvMiPathConf->tCbBuffer.ulSize);
        REG_SET_SLICE(ptMrvReg->mi_sp_cb_offs_cnt_init, MRV_MI_SP_CB_OFFS_CNT_INIT,        ptMrvMiPathConf->tCbBuffer.ulOffs);
    }

    if (ptMrvMiPathConf->tCrBuffer.pucBuffer)
    {
        REG_SET_SLICE(ptMrvReg->mi_sp_cr_base_ad_init,  MRV_MI_SP_CR_BASE_AD_INIT, (UINT32)ptMrvMiPathConf->tCrBuffer.pucBuffer);
        REG_SET_SLICE(ptMrvReg->mi_sp_cr_size_init,     MRV_MI_SP_CR_SIZE_INIT,            ptMrvMiPathConf->tCrBuffer.ulSize);
        REG_SET_SLICE(ptMrvReg->mi_sp_cr_offs_cnt_init, MRV_MI_SP_CR_OFFS_CNT_INIT,        ptMrvMiPathConf->tCrBuffer.ulOffs);
    }
    
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    if((!ptMrvMiPathConf->ulYPicWidth) || (!ptMrvMiPathConf->ulYPicHeight))
    {
        return RET_FAILURE;
    }

    REG_SET_SLICE(ptMrvReg->mi_sp_y_pic_width,  MRV_MI_SP_Y_PIC_WIDTH,  ptMrvMiPathConf->ulYPicWidth);
    REG_SET_SLICE(ptMrvReg->mi_sp_y_pic_height, MRV_MI_SP_Y_PIC_HEIGHT, ptMrvMiPathConf->ulYPicHeight);
    REG_SET_SLICE(ptMrvReg->mi_sp_y_pic_size,   MRV_MI_SP_Y_PIC_SIZE,   ptMrvMiPathConf->ulYPicHeight * ptMrvMiPathConf->ulLLength);
    
    // update base and offset registers during next immediate or automatic update request
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_OFFSET_EN, ENABLE);
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_INIT_BASE_EN,   ENABLE);

	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        // frame synchronous update of shadow registers, update is done after the current frame
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        // immediate update of shadow registers, disturb the current picture frame operation
        REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
  #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2) // [hf]: I think, this must be located here!
    s_ulMrvMifSpYPicWidth  = ptMrvMiPathConf->ulYPicWidth;
    s_ulMrvMifSpYPicHeight = ptMrvMiPathConf->ulYPicHeight;
  #endif  // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)

    // update base and offset registers during next immediate or automatic update request
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_OFFSET, ON);
    REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_INIT_BASE,   ON);

	switch (eUpdateTime)
	{
	case eMrvCfgUpdateFrameSync:
        // frame synchronous update of shadow registers, update is done after the current frame
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
		break;
	case eMrvCfgUpdateImmediate:
        // immediate update of shadow registers, disturb the current picture frame operation
        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_IN,  ON);
        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
		break;
	case eMrvCfgUpdateLater:
		// no update from within this function
		break;
	default:
		break;
	}
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    
//    MrvReg_DumpAll( stdout );
    return iError;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_GetSelfBuffer \n
 *  \RETURNVALUE RET_SUCCESS      - Everything is okay. \n
 *               RET_NULL_POINTER - Any of the given pointers is NULL. \n
 *               RET_FAILURE      - Something went wrong while calling
 *                                  MrvMif_GetPixelPer32BitOfLine(). \n
 *  \PARAMETERS  Pointer to structure where to store the register values. \n
 *
 *  \DESCRIPTION Reads the current self picture buffer registers and returns
 *               them, if possible. \n
 */
/*****************************************************************************/
RESULT MrvMif_GetSelfBuffer(tsMrvMiPathConf *ptMrvMiPathConf)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ptMrvMiPathConf)
    {
        memset(ptMrvMiPathConf, 0, sizeof(tsMrvMiPathConf));
        // get register values Y buffer
        ptMrvMiPathConf->tYBuffer.pucBuffer  = (UINT8 *)REG_GET_SLICE(ptMrvReg->mi_sp_y_base_ad_init,    MRV_MI_SP_Y_BASE_AD_INIT);
        ptMrvMiPathConf->tYBuffer.ulSize     =          REG_GET_SLICE(ptMrvReg->mi_sp_y_size_init,       MRV_MI_SP_Y_SIZE_INIT);
        ptMrvMiPathConf->tYBuffer.ulOffs     =          REG_GET_SLICE(ptMrvReg->mi_sp_y_offs_cnt_start,  MRV_MI_SP_Y_OFFS_CNT_START);
        // get register values Cb buffer
        ptMrvMiPathConf->tCbBuffer.pucBuffer = (UINT8 *)REG_GET_SLICE(ptMrvReg->mi_sp_cb_base_ad_init,   MRV_MI_SP_CB_BASE_AD_INIT);
        ptMrvMiPathConf->tCbBuffer.ulSize    =          REG_GET_SLICE(ptMrvReg->mi_sp_cb_size_init,      MRV_MI_SP_CB_SIZE_INIT);
        ptMrvMiPathConf->tCbBuffer.ulOffs    =          REG_GET_SLICE(ptMrvReg->mi_sp_cb_offs_cnt_start, MRV_MI_SP_CB_OFFS_CNT_START);
        // get register values Cr buffer
        ptMrvMiPathConf->tCrBuffer.pucBuffer = (UINT8 *)REG_GET_SLICE(ptMrvReg->mi_sp_cr_base_ad_init,   MRV_MI_SP_CR_BASE_AD_INIT);
        ptMrvMiPathConf->tCrBuffer.ulSize    =          REG_GET_SLICE(ptMrvReg->mi_sp_cr_size_init,      MRV_MI_SP_CR_SIZE_INIT);
        ptMrvMiPathConf->tCrBuffer.ulOffs    =          REG_GET_SLICE(ptMrvReg->mi_sp_cr_offs_cnt_start, MRV_MI_SP_CR_OFFS_CNT_START);
        
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
        {
            RESULT iError = RET_SUCCESS;
            UINT8  ucPp32B = 0;

            // ulLLength is counted in pixels, but this MARVIN version needs it in bytes for the register
            iError = MrvMif_GetPixelPer32BitOfLine(&ucPp32B, s_eMrvMifSpOutForm_SP, s_eMrvMifPicForm_SP, TRUE);
            if (iError != RET_SUCCESS)
            {
                return iError;
            }

            ptMrvMiPathConf->ulLLength    = (REG_GET_SLICE(ptMrvReg->mi_sp_y_llength, MRV_MI_SP_Y_LLENGTH) * ucPp32B) >> 2;
            ptMrvMiPathConf->ulYPicWidth  = s_ulMrvMifSpYPicWidth;
            ptMrvMiPathConf->ulYPicHeight = s_ulMrvMifSpYPicHeight;
        }
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
        // ulLLength is counted in pixels and this value could be read out directly from the register
        ptMrvMiPathConf->ulLLength    = REG_GET_SLICE(ptMrvReg->mi_sp_y_llength,    MRV_MI_SP_Y_LLENGTH);
        ptMrvMiPathConf->ulYPicWidth  = REG_GET_SLICE(ptMrvReg->mi_sp_y_pic_width,  MRV_MI_SP_Y_PIC_WIDTH);
        ptMrvMiPathConf->ulYPicHeight = REG_GET_SLICE(ptMrvReg->mi_sp_y_pic_height, MRV_MI_SP_Y_PIC_HEIGHT);
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V1) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V2)
    }
    else
    {
        DBG_OUT((DERR, "MrvMif_GetSelfBuffer(): ptMrvMiPathConf is NULL\n"));
        return RET_NULL_POINTER;
    }

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_SetDmaBuffer \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_OUTOFRANGE   - one or more of the structure members are
 *                                  out of range \n
 *               RET_SUCCESS      - the desired settings have been applied \n
 *
 *  \PARAMETERS  ptMrvMiDmaConf   - pointer to tsMrvMiDmaConf structure \n
 *
 *  \DESCRIPTION Configures the DMA path buffers of the MI. \n
 *
 *  \NOTES       Prerequisite to call this function is that the self picture
 *               path was configured via MrvMif_SetDmaConfig() after any
 *               MARVIN reset. \n
 */
/*****************************************************************************/
RESULT MrvMif_SetDmaBuffer(const tsMrvMiPathConf *ptMrvMiPathConf)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    RESULT iError  = RET_SUCCESS;
  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    UINT8  ucPp32B = 0;
  #endif  // MARVIN_FEATURE_MI
    tsMrvMiCtrl tMrvMiCtrl;
    if(MrvMif_GetPathAndOrientation(&tMrvMiCtrl) == RET_SUCCESS)
    {
        if((tMrvMiCtrl.eMrvMifSpPicForm != eMrvMif_PicForm_Planar) && (tMrvMiCtrl.eMrvMifMpPicForm != eMrvMif_PicForm_Planar))
        {
            // check path only if dma doesn't be in planar mode
            iError = MrvMif_CheckMiPathConf(ptMrvMiPathConf, FALSE);
            if (iError != RET_SUCCESS)
            {
                DBG_OUT((DERR, "MrvMif_SetDmaBuffer(): MrvMif_CheckMiPathConf() returned 0x%08X\n", iError));
                return iError;
            }
        }
    }
    if((ptMrvMiPathConf->tYBuffer.pucBuffer == 0) ||
       (ptMrvMiPathConf->ulYPicWidth        == 0) ||
       (ptMrvMiPathConf->ulYPicHeight       == 0) ||
       (ptMrvMiPathConf->ulLLength          == 0)   )
    {
        return RET_OUTOFRANGE;
    }

    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_start_ad, MRV_MI_DMA_Y_PIC_START_AD, (UINT32)ptMrvMiPathConf->tYBuffer.pucBuffer);

  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    // ulYPicWidth and ulLLength are counted in pixels, but this MARVIN version needs them in bytes for the register
    iError = MrvMif_GetPixelPer32BitOfLine(&ucPp32B, s_eMrvMifColFormat_DMA, s_eMrvMifPicForm_DMA, TRUE);
    if (iError != RET_SUCCESS)
    {
        return iError;
    }

    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_width, MRV_MI_DMA_Y_PIC_WIDTH, ((ptMrvMiPathConf->ulYPicWidth)                               << 2) / ucPp32B);
    REG_SET_SLICE(ptMrvReg->mi_dma_y_llength,   MRV_MI_DMA_Y_LLENGTH,   ((ptMrvMiPathConf->ulLLength)                                 << 2) / ucPp32B);
    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_size,  MRV_MI_DMA_Y_PIC_SIZE,  ((ptMrvMiPathConf->ulLLength * ptMrvMiPathConf->ulYPicHeight) << 2) / ucPp32B);
  #elif ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))

    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_width, MRV_MI_DMA_Y_PIC_WIDTH,  ptMrvMiPathConf->ulYPicWidth);
    REG_SET_SLICE(ptMrvReg->mi_dma_y_llength,   MRV_MI_DMA_Y_LLENGTH,    ptMrvMiPathConf->ulLLength);
    REG_SET_SLICE(ptMrvReg->mi_dma_y_pic_size,  MRV_MI_DMA_Y_PIC_SIZE,  (ptMrvMiPathConf->ulLLength * ptMrvMiPathConf->ulYPicHeight));
  #else // MARVIN_FEATURE_MI
    #error "unknown MARVIN_FEATURE_MI value"
  #endif // MARVIN_FEATURE_MI
    
  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    if ((ptMrvMiPathConf->tCbBuffer.pucBuffer) || (ptMrvMiPathConf->tCrBuffer.pucBuffer))
    { // calculate ucPp32B for Cb and Cr buffer
        // ulYPicWidth and ulLLength are counted in pixels, but this MARVIN version needs them in bytes for the register
        iError = MrvMif_GetPixelPer32BitOfLine(&ucPp32B, s_eMrvMifColFormat_DMA, s_eMrvMifPicForm_DMA, FALSE);
        if (iError != RET_SUCCESS)
        {
            return iError;
        }
    }
  #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)

    if (ptMrvMiPathConf->tCbBuffer.pucBuffer)
    {
        REG_SET_SLICE(ptMrvReg->mi_dma_cb_pic_start_ad, MRV_MI_DMA_CB_PIC_START_AD, (UINT32)ptMrvMiPathConf->tCbBuffer.pucBuffer);

  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        // ulYPicWidth and ulLLength are counted in pixels, but this MARVIN version needs them in bytes for the register
        REG_SET_SLICE(ptMrvReg->mi_dma_cb_pic_width, MRV_MI_DMA_CB_PIC_WIDTH, ((ptMrvMiPathConf->ulYPicWidth) << 2) / ucPp32B);
        REG_SET_SLICE(ptMrvReg->mi_dma_cb_llength,   MRV_MI_DMA_CB_LLENGTH,   ((ptMrvMiPathConf->ulLLength)   << 2) / ucPp32B);

        if (s_eMrvMifColFormat_DMA == eMrvMifCol_Format_YCbCr420)
        { // in this case the vertical size is half as much in compare to the normal Cb buffer
            REG_SET_SLICE(ptMrvReg->mi_dma_cb_pic_size, MRV_MI_DMA_CB_PIC_SIZE, ((ptMrvMiPathConf->ulLLength * ptMrvMiPathConf->ulYPicHeight) << 1) / ucPp32B);
        }
        else
        { // in this case the vertical size is the same as of the normal Cb buffer
            REG_SET_SLICE(ptMrvReg->mi_dma_cb_pic_size, MRV_MI_DMA_CB_PIC_SIZE, ((ptMrvMiPathConf->ulLLength * ptMrvMiPathConf->ulYPicHeight) << 2) / ucPp32B);
        }
  #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    }
    
    if (ptMrvMiPathConf->tCrBuffer.pucBuffer)
    {
        REG_SET_SLICE(ptMrvReg->mi_dma_cr_pic_start_ad, MRV_MI_DMA_CR_PIC_START_AD, (UINT32)ptMrvMiPathConf->tCrBuffer.pucBuffer);

  #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        // ulYPicWidth and ulLLength are counted in pixels, but this MARVIN version needs them in bytes for the register
        REG_SET_SLICE(ptMrvReg->mi_dma_cr_pic_width, MRV_MI_DMA_CR_PIC_WIDTH, ((ptMrvMiPathConf->ulYPicWidth) << 2) / ucPp32B);
        REG_SET_SLICE(ptMrvReg->mi_dma_cr_llength,   MRV_MI_DMA_CR_LLENGTH,   ((ptMrvMiPathConf->ulLLength)   << 2) / ucPp32B);

        if (s_eMrvMifColFormat_DMA == eMrvMifCol_Format_YCbCr420)
        { // in this case the vertical size is half as much in compare to the normal Cr buffer
            REG_SET_SLICE(ptMrvReg->mi_dma_cr_pic_size, MRV_MI_DMA_CR_PIC_SIZE, ((ptMrvMiPathConf->ulLLength * ptMrvMiPathConf->ulYPicHeight) << 1) / ucPp32B);
        }
        else
        { // in this case the vertical size is the same as of the normal Cr buffer
            REG_SET_SLICE(ptMrvReg->mi_dma_cr_pic_size, MRV_MI_DMA_CR_PIC_SIZE, ((ptMrvMiPathConf->ulLLength * ptMrvMiPathConf->ulYPicHeight) << 2) / ucPp32B);
        }
  #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    }
    
    return iError;
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(ptMrvMiPathConf);        // added to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_SetDmaConfig \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_OUTOFRANGE   - one or more of the structure members are
 *                                  out of range \n
 *               RET_SUCCESS      - the desired settings have been applied \n
 *               RET_BUSY         - the DMA path is currently in use \n
 *               RET_NOTSUPP      - no DMA read feature available \n
 *
 *  \PARAMETERS  ptMrvMiDmaConf   - pointer to tsMrvMiDmaConf structure \n
 *
 *  \DESCRIPTION Configures the DMA path of the MI. \n
 */
/*****************************************************************************/
RESULT MrvMif_SetDmaConfig(const tsMrvMiDmaConf *ptMrvMiDmaConf)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    RESULT iError = RET_SUCCESS;
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    UINT32 mi_dma_ctrl = REG_READ(ptMrvReg->mi_dma_ctrl);
  #endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    if (!ptMrvMiDmaConf)
    {
        return RET_NULL_POINTER;
    }
    
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    switch (ptMrvMiDmaConf->eBurstLengthChrom)
    {
    case eMrvMif_BurstLength_4:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_BURST_LEN_CHROM, MRV_MI_DMA_BURST_LEN_CHROM_4);
        break;
    case eMrvMif_BurstLength_8:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_BURST_LEN_CHROM, MRV_MI_DMA_BURST_LEN_CHROM_8);
        break;
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    case eMrvMif_BurstLength_16:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_BURST_LEN_CHROM, MRV_MI_DMA_BURST_LEN_CHROM_16);
        break;
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    default:
        return RET_OUTOFRANGE;
    }

    switch (ptMrvMiDmaConf->eBurstLengthLum)
    {
    case eMrvMif_BurstLength_4:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_BURST_LEN_LUM, MRV_MI_DMA_BURST_LEN_LUM_4);
        break;
    case eMrvMif_BurstLength_8:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_BURST_LEN_LUM, MRV_MI_DMA_BURST_LEN_LUM_8);
        break;
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    case eMrvMif_BurstLength_16:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_BURST_LEN_LUM, MRV_MI_DMA_BURST_LEN_LUM_16);
        break;
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    default:
        return RET_OUTOFRANGE;
    }

    switch(ptMrvMiDmaConf->eMrvMifPicForm)
    {
    case eMrvMif_PicForm_Planar:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_READ_FORMAT, MRV_MI_DMA_READ_FORMAT_PLANAR);
        break;
    case eMrvMif_PicForm_SemiPlanar:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_READ_FORMAT, MRV_MI_DMA_READ_FORMAT_SEMIPLANAR);
        break;
    case eMrvMif_PicForm_Interleaved:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_READ_FORMAT, MRV_MI_DMA_READ_FORMAT_INTERLEAVED);
        break;
    default:
        return RET_OUTOFRANGE;
    }

    switch (ptMrvMiDmaConf->eMrvMifColFormat)
    {
    case eMrvMifCol_Format_YCbCr422:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_INOUT_FORMAT, MRV_MI_DMA_INOUT_FORMAT_YUV422);
        break;
    case eMrvMifCol_Format_YCbCr444:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_INOUT_FORMAT, MRV_MI_DMA_INOUT_FORMAT_YUV444);
        break;
    case eMrvMifCol_Format_YCbCr420:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_INOUT_FORMAT, MRV_MI_DMA_INOUT_FORMAT_YUV420);
        break;
    case eMrvMifCol_Format_YCbCr400:
        REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_INOUT_FORMAT, MRV_MI_DMA_INOUT_FORMAT_YUV400);
        break;
    case eMrvMifCol_Format_RGB_565:  // added to avoid LINT warnings (Info 788)
    case eMrvMifCol_Format_RGB_666:  // added to avoid LINT warnings (Info 788)
    case eMrvMifCol_Format_RGB_888:  // added to avoid LINT warnings (Info 788)
    default:
        // note: the RGB-modes of the enum are not supported as DMA input 
        return RET_OUTOFRANGE;
    }

    REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_CONTINUOUS_EN,     (ptMrvMiDmaConf->bContinuousEnable) ? ENABLE : DISABLE);
    REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_FRAME_END_DISABLE, (ptMrvMiDmaConf->bFrameEndDisable)  ?     ON :     OFF);
    REG_SET_SLICE(mi_dma_ctrl, MRV_MI_DMA_BYTE_SWAP,         (ptMrvMiDmaConf->bByteSwapEnable)   ?     ON :     OFF);
    // ON:  first pixel is expected in bits 31:24 of a memory word
    // OFF: first pixel is expected in bits 7:0 of a memory word

    // now write all settings into register
    REG_WRITE(ptMrvReg->mi_dma_ctrl, mi_dma_ctrl);
    
    if (ptMrvMiDmaConf->bStartDma)
    {
        if (REG_GET_SLICE(ptMrvReg->mi_dma_status, MRV_MI_DMA_ACTIVE))
        { // last DMA frame is not finished
            DBG_OUT((DERR, "MrvMif_SetDmaConfig(): DMA is busy\n"));
            return RET_BUSY;
        }

        REG_SET_SLICE(ptMrvReg->mi_dma_start, MRV_MI_DMA_START, ON);
    }

  #elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)

    switch (ptMrvMiDmaConf->eMrvMifPicForm)
    {
    case eMrvMif_PicForm_Planar:
    case eMrvMif_PicForm_SemiPlanar:
    case eMrvMif_PicForm_Interleaved:
        s_eMrvMifPicForm_DMA = ptMrvMiDmaConf->eMrvMifPicForm;
        break;
    default:
        return RET_OUTOFRANGE;
    }

    switch(ptMrvMiDmaConf->eMrvMifColFormat)
    {
    case eMrvMifCol_Format_YCbCr422:
    case eMrvMifCol_Format_YCbCr444:
    case eMrvMifCol_Format_YCbCr420:
    case eMrvMifCol_Format_YCbCr400:
        s_eMrvMifColFormat_DMA = ptMrvMiDmaConf->eMrvMifColFormat;
    	break;
    case eMrvMifCol_Format_RGB_888:
    case eMrvMifCol_Format_RGB_666:
    case eMrvMifCol_Format_RGB_565:
    default:
        return RET_OUTOFRANGE;
    }
    
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_BYTE_SWAP, (ptMrvMiDmaConf->bByteSwapEnable) ? ON : OFF);
    // ON:  first pixel is expected in bits 31:24 of a memory word
    // OFF: first pixel is expected in bits 7:0 of a memory word
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_DMA_PATH, (ptMrvMiDmaConf->bViaCamPath) ?  MRV_MI_DMA_PATH_MP_SP: MRV_MI_DMA_PATH_SP_ONLY);

    if (ptMrvMiDmaConf->bStartDma)
    {
        if (REG_GET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_DMA_SP_EN))
        { // last DMA frame is not finished
            DBG_OUT((DERR, "MrvMif_SetDmaConfig(): DMA is busy\n"));
            return RET_BUSY;
        }

        REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_DMA_SP_EN, ENABLE);
    }
  #endif // MARVIN_FEATURE_MI

    return iError;
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(ptMrvMiDmaConf);     // added to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_SetPathAndOrientation \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_OUTOFRANGE   - one or more of the structure members are
 *                                  out of range or not supported by the used
 *                                  version of the MI \n
 *               RET_SUCCESS      - the desired settings have been applied \n
 *               RET_NOTSUPP      - an error occured \n
 *
 *  \PARAMETERS  ptMrvMiDmaConf   - pointer to tsMrvMiDmaConf structure \n
 *
 *  \DESCRIPTION Configures the DMA path of the MI. \n
 *
 *  \NOTES       This function should be only called in the case that the
 *               self picture path processes currently no data. Otherwise it
 *               will result in unpredictable behaviour of the MI. \n
 */
/*****************************************************************************/
RESULT MrvMif_SetPathAndOrientation(const tsMrvMiCtrl *ptMrvMiCtrl)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    RESULT iError  = RET_OUTOFRANGE;
    UINT32 mi_ctrl = 0; // we don't need current value from register

    if(!ptMrvMiCtrl)
    {
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): ptMrvMiCtrl is NULL\n"));
        return RET_NULL_POINTER;
    }

    if((ptMrvMiCtrl->ulIrqOffsInit & ~(MRV_MI_MP_Y_IRQ_OFFS_INIT_VALID_MASK)) != 0)
    {
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->ulIrqOffsInit value\n"));
        return iError;
    }

    REG_SET_SLICE(ptMrvReg->mi_mp_y_irq_offs_init, MRV_MI_MP_Y_IRQ_OFFS_INIT, ptMrvMiCtrl->ulIrqOffsInit);

    // main picture path
    switch (ptMrvMiCtrl->eMainPath)
    {
    case eMrvPathOff:
        if(ptMrvMiCtrl->eSelfPath == eMrvPathOff)
        {
        	
           REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x0);
        }
        else
        {
            if(ptMrvMiCtrl->eSelfPath == eMrvPathOn)
            {
            	
                REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x2);
            }
            else
            {
                if(ptMrvMiCtrl->eSelfPath == eMrvPathDpcc)
                {
                    REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0xE);
                }
                else
                {	
                    DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eSelfPath value\n"));
                    return iError;
                }
            }
        }
        break;
    case eMrvPathOn:
        if(ptMrvMiCtrl->eSelfPath == eMrvPathOff)
        {
            REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x1);
        }
        else
        {
            if(ptMrvMiCtrl->eSelfPath == eMrvPathOn)
            {
            	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x3);
            }
            else
            {
                if(ptMrvMiCtrl->eSelfPath == eMrvPathDpcc)
                {
                	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x9);
                }
                else
                {	
                    DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eSelfPath value\n"));
                    return iError;
                }
            }
        }
        break;
    case eMrvPathJpe:
        if(ptMrvMiCtrl->eSelfPath == eMrvPathOff)
        {
        	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x4);
        }
        else
        {
            if(ptMrvMiCtrl->eSelfPath == eMrvPathOn)
            {
            	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x6);
            }
            else
            {
                if(ptMrvMiCtrl->eSelfPath == eMrvPathDpcc)
                {
                	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0xC);
                }
                else
                {	
                    DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eSelfPath value\n"));
                    return iError;
                }
            }
        }
        break;
    case eMrvPathRaw:
        if(ptMrvMiCtrl->eSelfPath == eMrvPathOff)
        {
        	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x8);
        }
        else
        {
                if(ptMrvMiCtrl->eSelfPath == eMrvPathDpcc)
                {
                	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0xF);
                }
                else
                {	
                    DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eSelfPath value\n"));
                    return iError;
                }
            
        }
        break;
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     case eMrvPathRaw16:
        if(ptMrvMiCtrl->eSelfPath == eMrvPathOff)
        {
        	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0x8);
        }
        else
        {
                if(ptMrvMiCtrl->eSelfPath == eMrvPathDpcc)
                {
                	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0xF);
                }
                else
                {	
                    DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eSelfPath value\n"));
                    return iError;
                }
            
        }
        REG_SET_SLICE(mi_ctrl, MRV_MI_MP_WRITE_FORMAT, MRV_MI_MP_WRITE_FORMAT_INTERLEAVED);
        break;
     case eMrvPathDpcc:
        if(ptMrvMiCtrl->eSelfPath == eMrvPathOff)
        {
        	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0xD);
        }
        else
        {
            if(ptMrvMiCtrl->eSelfPath == eMrvPathOn)
            {
            	REG_SET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE, 0xA);
            }
            else
            {	
                DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eSelfPath value\n"));
                return iError;
            }
        }
        break;
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3)
    default:
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eMainPath value\n"));
        return iError;
    }
    
    // self picture path output format
    switch (ptMrvMiCtrl->eMrvMifSpOutForm)
    {
  #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    case eMrvMifCol_Format_YCbCr422:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT, MRV_MI_SP_OUTPUT_FORMAT_YUV422);
        break;
    case eMrvMifCol_Format_YCbCr444:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT, MRV_MI_SP_OUTPUT_FORMAT_YUV444);
        break;
    case eMrvMifCol_Format_YCbCr420:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT, MRV_MI_SP_OUTPUT_FORMAT_YUV420);
        break;
    case eMrvMifCol_Format_YCbCr400:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT, MRV_MI_SP_OUTPUT_FORMAT_YUV400);
        break;
    case eMrvMifCol_Format_RGB_565:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT, MRV_MI_SP_OUTPUT_FORMAT_RGB565);
        break;
    case eMrvMifCol_Format_RGB_888:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT, MRV_MI_SP_OUTPUT_FORMAT_RGB888);
        break;
    case eMrvMifCol_Format_RGB_666:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT, MRV_MI_SP_OUTPUT_FORMAT_RGB666);
        break;
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    case eMrvMifCol_Format_YCbCr422:
    case eMrvMifCol_Format_YCbCr444:
    case eMrvMifCol_Format_YCbCr420:
        s_eMrvMifSpOutForm_SP = ptMrvMiCtrl->eMrvMifSpOutForm;
        break;
    case eMrvMifCol_Format_RGB_565:
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_565);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
        s_eMrvMifSpOutForm_SP = ptMrvMiCtrl->eMrvMifSpOutForm;
        break;
    case eMrvMifCol_Format_RGB_888:
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_888);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
        s_eMrvMifSpOutForm_SP = ptMrvMiCtrl->eMrvMifSpOutForm;
        break;
    case eMrvMifCol_Format_RGB_666:
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_666);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
        s_eMrvMifSpOutForm_SP = ptMrvMiCtrl->eMrvMifSpOutForm;
        break;
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    case eMrvMifCol_Format_YCbCr422:
    case eMrvMifCol_Format_YCbCr420:
        s_eMrvMifSpOutForm_SP = ptMrvMiCtrl->eMrvMifSpOutForm;
        break;
    case eMrvMifCol_Format_RGB_565:
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_565);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
        s_eMrvMifSpOutForm_SP = ptMrvMiCtrl->eMrvMifSpOutForm;
        break;
    case eMrvMifCol_Format_RGB_888:
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT, MRV_MI_RGB_FORMAT_888);
        REG_SET_SLICE(mi_ctrl, MRV_MI_RGB_EN, ENABLE);
        s_eMrvMifSpOutForm_SP = ptMrvMiCtrl->eMrvMifSpOutForm;
        break;
#else // MARVIN_FEATURE_MI
    #error "unknown MARVIN_FEATURE_MI value"
#endif // MARVIN_FEATURE_MI
    default:
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eMrvMifSpOutForm value\n"));
        return iError;
    }
    
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
     // self picture path input format
    switch (ptMrvMiCtrl->eMrvMifSpInForm)
    {
    case eMrvMifCol_Format_YCbCr422:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_INPUT_FORMAT, MRV_MI_SP_INPUT_FORMAT_YUV422);
        break;
    case eMrvMifCol_Format_YCbCr444:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_INPUT_FORMAT, MRV_MI_SP_INPUT_FORMAT_YUV444);
        break;
    case eMrvMifCol_Format_YCbCr420:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_INPUT_FORMAT, MRV_MI_SP_INPUT_FORMAT_YUV420);
        break;
    case eMrvMifCol_Format_YCbCr400:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_INPUT_FORMAT, MRV_MI_SP_INPUT_FORMAT_YUV400);
        break;
    case eMrvMifCol_Format_RGB_565:         // added to avoid LINT warnings (Info 788)
    case eMrvMifCol_Format_RGB_666:         // added to avoid LINT warnings (Info 788)
    case eMrvMifCol_Format_RGB_888:         // added to avoid LINT warnings (Info 788)
    default:
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eMrvMifSpInForm value\n"));
        return iError;
    }
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4) 
    switch (ptMrvMiCtrl->eMrvMifSpInRange)
    {
    case eMrvMifCol_Range_Full:
        REG_SET_SLICE(mi_ctrl, MRV_MI_Y_FULL_RANGE, ON);
        REG_SET_SLICE(mi_ctrl, MRV_MI_CBCR_FULL_RANGE, ON);
        break;
    case eMrvMifCol_Range_Std:
        REG_SET_SLICE(mi_ctrl, MRV_MI_Y_FULL_RANGE, OFF);
        REG_SET_SLICE(mi_ctrl, MRV_MI_CBCR_FULL_RANGE, OFF);
        break;
    default:
        DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): bad ptMrvMiCtrl->eMrvMifSpInRange value \n"));
        return iError;
    }
    
    switch (ptMrvMiCtrl->eMrvMifSpInPhase)
    {
    case eMrvMifCol_Phase_Non_Cosited:
        REG_SET_SLICE(mi_ctrl, MRV_MI_422NONCOSITED, ON);
        break;
    case eMrvMifCol_Phase_Cosited:
        REG_SET_SLICE(mi_ctrl, MRV_MI_422NONCOSITED, OFF);
        break;
    default:
        DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): bad ptMrvMiCtrl->eMrvMifSpInPhase value \n"));
        return iError;
    }
#endif // (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4) 
#endif // #if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))

    iError = RET_SUCCESS;
    
    // self picture path write format
    switch (ptMrvMiCtrl->eMrvMifSpPicForm)
    {
 #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
    case eMrvMif_PicForm_Planar:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SP_WRITE_FORMAT, MRV_MI_SP_WRITE_FORMAT_PLANAR);
        break;
    case eMrvMif_PicForm_SemiPlanar:
        if ((ptMrvMiCtrl->eMrvMifSpOutForm == eMrvMifCol_Format_YCbCr422) ||
            (ptMrvMiCtrl->eMrvMifSpOutForm == eMrvMifCol_Format_YCbCr420)   )
        {
            REG_SET_SLICE(mi_ctrl, MRV_MI_SP_WRITE_FORMAT, MRV_MI_SP_WRITE_FORMAT_SEMIPLANAR);
        }
        else
        {
            iError = RET_NOTSUPP;
        }
        break;
    case eMrvMif_PicForm_Interleaved:
        if (ptMrvMiCtrl->eMrvMifSpOutForm == eMrvMifCol_Format_YCbCr422)
        {
            REG_SET_SLICE(mi_ctrl, MRV_MI_SP_WRITE_FORMAT, MRV_MI_SP_WRITE_FORMAT_INTERLEAVED);
        }
        else
        {
            iError = RET_NOTSUPP;
        }
        break;
    default:
        iError = RET_OUTOFRANGE;
        break;
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    case eMrvMif_PicForm_Planar:
        s_eMrvMifPicForm_SP = ptMrvMiCtrl->eMrvMifSpPicForm;
        break;
    case eMrvMif_PicForm_SemiPlanar:
        REG_SET_SLICE(mi_ctrl, MRV_MI_SEMI_PLANAR_EN, ON);
        s_eMrvMifPicForm_SP = ptMrvMiCtrl->eMrvMifSpPicForm;
        break;
    case eMrvMif_PicForm_Interleaved:                       // added to avoid LINT warnings (Info 788)
    default:
        iError = RET_OUTOFRANGE;
        break;
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    case eMrvMif_PicForm_Planar:
        s_eMrvMifPicForm_SP = ptMrvMiCtrl->eMrvMifSpPicForm;
        break;
    case eMrvMif_PicForm_Interleaved:                       // added to avoid LINT warnings (Info 788)
    case eMrvMif_PicForm_SemiPlanar:                        // added to avoid LINT warnings (Info 788)
    default:
        iError = RET_OUTOFRANGE;
        break;
#else // MARVIN_FEATURE_MI
    #error "unknown MARVIN_FEATURE_MI value"
#endif // MARVIN_FEATURE_MI
    }

    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eMrvMifSpPicForm value\n"));
        return iError;
    }
    
    if (ptMrvMiCtrl->eMainPath == eMrvPathOn) 
    {   // for YCbCr mode only, permitted for raw mode
        // main picture path write format
        switch (ptMrvMiCtrl->eMrvMifMpPicForm)
        {
 #if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
        case eMrvMif_PicForm_Planar:
            REG_SET_SLICE(mi_ctrl, MRV_MI_MP_WRITE_FORMAT, MRV_MI_MP_WRITE_FORMAT_PLANAR);
            break;
        case eMrvMif_PicForm_SemiPlanar:
            REG_SET_SLICE(mi_ctrl, MRV_MI_MP_WRITE_FORMAT, MRV_MI_MP_WRITE_FORMAT_SEMIPLANAR);
            break;
        case eMrvMif_PicForm_Interleaved:
            REG_SET_SLICE(mi_ctrl, MRV_MI_MP_WRITE_FORMAT, MRV_MI_MP_WRITE_FORMAT_INTERLEAVED);
            break;
        default:
            iError = RET_OUTOFRANGE;
            break;
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
        case eMrvMif_PicForm_Planar:
            break;
        case eMrvMif_PicForm_SemiPlanar:
            REG_SET_SLICE(mi_ctrl, MRV_MI_SEMI_PLANAR_EN, ENABLE);
            break;
        case eMrvMif_PicForm_Interleaved:                       // added to avoid LINT warnings (Info 788)
        default:
            iError = RET_OUTOFRANGE;
            break;
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
        case eMrvMif_PicForm_Planar:
            break;
        case eMrvMif_PicForm_Interleaved:                       // added to avoid LINT warnings (Info 788)
        case eMrvMif_PicForm_SemiPlanar:                        // added to avoid LINT warnings (Info 788)
        default:
            iError = RET_OUTOFRANGE;
            break;
#else // MARVIN_FEATURE_MI
        #error "unknown MARVIN_FEATURE_MI value"
#endif // MARVIN_FEATURE_MI
        } // switch eMrvMifMpPicForm
    } //if eMainPath == YUV

    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eMrvMifMpPicForm value\n"));
        return iError;
    }
    
#if ((MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4) && (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3))
     // burst length for chrominance for write port
    switch (ptMrvMiCtrl->eBurstLengthChrom)
    {
    case eMrvMif_BurstLength_4:
        REG_SET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_CHROM, MRV_MI_BURST_LEN_CHROM_4);
        break;
    case eMrvMif_BurstLength_8:
        REG_SET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_CHROM, MRV_MI_BURST_LEN_CHROM_8);
        break;
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    case eMrvMif_BurstLength_16:
        REG_SET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_CHROM, MRV_MI_BURST_LEN_CHROM_16);
        break;
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    default:
        iError = RET_OUTOFRANGE;
        break;
    }

    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eBurstLengthChrom value\n"));
        return iError;
    }
    
    // burst length for luminance for write port
    switch (ptMrvMiCtrl->eBurstLengthLum)
    {
    case eMrvMif_BurstLength_4:
        REG_SET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_LUM, MRV_MI_BURST_LEN_LUM_4);
        break;
    case eMrvMif_BurstLength_8:
        REG_SET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_LUM, MRV_MI_BURST_LEN_LUM_8);
        break;
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    case eMrvMif_BurstLength_16:
        REG_SET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_LUM, MRV_MI_BURST_LEN_LUM_16);
        break;
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    default:
        iError = RET_OUTOFRANGE;
        break;
    }

    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eBurstLengthLum value\n"));
        return iError;
    }

    // enable updating of the shadow registers for main and self picture to their init values
    switch (ptMrvMiCtrl->eInitVals)
    {
    case eMrvMif_NoInitVals:
        break;
    case eMrvMif_InitOffs:
        REG_SET_SLICE(mi_ctrl, MRV_MI_INIT_OFFSET_EN, ENABLE);
        break;
    case eMrvMif_InitBase:
        REG_SET_SLICE(mi_ctrl, MRV_MI_INIT_BASE_EN, ENABLE);
        break;
    case eMrvMif_InitOffsAndBase:
        REG_SET_SLICE(mi_ctrl, MRV_MI_INIT_OFFSET_EN, ENABLE);
        REG_SET_SLICE(mi_ctrl, MRV_MI_INIT_BASE_EN, ENABLE);
        break;
    default:
        iError = RET_OUTOFRANGE;
        break;
    }

    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetPathAndOrientation(): bad ptMrvMiCtrl->eInitVals value\n"));
        return iError;
    }
#endif  // #if( MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3 )
    
    // enable change of byte order for write port
    REG_SET_SLICE(mi_ctrl, MRV_MI_BYTE_SWAP, (ptMrvMiCtrl->bByteSwapEnable) ? ON : OFF);

#if (MARVIN_FEATURE_MI_LAST_PIXEL == MARVIN_FEATURE_EXIST)
    // enable or disable the last pixel signalization
    REG_SET_SLICE(mi_ctrl, MRV_MI_LAST_PIXEL_SIG_EN, (ptMrvMiCtrl->bLastPixelEnable) ? ON : OFF);
#endif //#if (MARVIN_FEATURE_MI_LAST_PIXEL == MARVIN_FEATURE_EXIST)

    // now write settings into register
    REG_WRITE(ptMrvReg->mi_ctrl, mi_ctrl);
    
    // self picture path operating mode
    if ((ptMrvMiCtrl->eSelfPath == eMrvPathOn)  ||
        (ptMrvMiCtrl->eSelfPath == eMrvPathOff) ||
        (ptMrvMiCtrl->eSelfPath == eMrvPathDpcc)   )
    {
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
        //do not call if not supported
        //support has been restricted to >= MI_V2 && <= MI_V3 in MrvMif_SetSelfPicOrientation, so we do the same here
        //a future V4 may also support rotation
        iError = MrvMif_SetSelfPicOrientation(ptMrvMiCtrl->eMrvMifSpMode);
#endif //MARVIN_FEATURE_MI >= MI_V2 && <= MI_V3
    }
    else
    {
        DBG_OUT( (DERR, "MrvMif_SetPathAndOrientation: bad ptMrvMiCtrl->eSelfPath value\n") );
        iError = RET_OUTOFRANGE;
    }

 #if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
     REG_SET_SLICE(ptMrvReg->mi_init, MRV_MI_MI_CFG_UPD,  ON);
#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_IN,  ON);
    REG_SET_SLICE(ptMrvReg->mi_ctrl, MRV_MI_CFG_UPD_OUT, ON);
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    return iError;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_GetPathAndOrientation \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_FAILURE      - an unknown register value was set \n
 *               RET_SUCCESS      - everything okay \n
 *
 *  \PARAMETERS  ptMrvMiDmaConf   - pointer where to store the tsMrvMiDmaConf
 *                                  structure \n
 *
 *  \DESCRIPTION Gets the current configuration of the MI. \n
 */
/*****************************************************************************/
RESULT MrvMif_GetPathAndOrientation(tsMrvMiCtrl *ptMrvMiCtrl)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mi_ctrl = REG_READ(ptMrvReg->mi_ctrl);
    UINT32 MiPath;

    if (!ptMrvMiCtrl)
    {
        DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): ptMrvMiCtrl is NULL\n"));
        return RET_NULL_POINTER;
    }

    // reset the complete structure
    memset(ptMrvMiCtrl, 0, sizeof(tsMrvMiCtrl));




    MiPath = REG_GET_SLICE(mi_ctrl, MRV_MI_PATH_ENABLE);
    switch(MiPath)
    {
    	case 0x0: //0000
            ptMrvMiCtrl->eMainPath = eMrvPathOff;
            ptMrvMiCtrl->eSelfPath = eMrvPathOff;
            break;   	
    	case 0x1: //0001
        ptMrvMiCtrl->eMainPath = eMrvPathOn;
            ptMrvMiCtrl->eSelfPath = eMrvPathOff;
            break;   	
    	case 0x2: //0010
            ptMrvMiCtrl->eMainPath = eMrvPathOff;
            ptMrvMiCtrl->eSelfPath = eMrvPathOn;
            break;   	
    	case 0x3: //0011
            ptMrvMiCtrl->eMainPath = eMrvPathOn;
            ptMrvMiCtrl->eSelfPath = eMrvPathOn;
            break;   	
    	case 0x4: //0100
        ptMrvMiCtrl->eMainPath = eMrvPathJpe;
            ptMrvMiCtrl->eSelfPath = eMrvPathOff;
            break;   	
    	case 0x6: //0110
            ptMrvMiCtrl->eMainPath = eMrvPathJpe;
            ptMrvMiCtrl->eSelfPath = eMrvPathOn;
            break;   	
    	case 0x8: //1000
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
         if (REG_GET_SLICE(mi_ctrl, MRV_MI_MP_WRITE_FORMAT) == MRV_MI_MP_WRITE_FORMAT_INTERLEAVED)
        {
            ptMrvMiCtrl->eMainPath = eMrvPathRaw16;
        }
        else
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3)
        {
            ptMrvMiCtrl->eMainPath = eMrvPathRaw;
        }
            ptMrvMiCtrl->eSelfPath = eMrvPathOff;
            break;   	
    	case 0x9: //1001
            ptMrvMiCtrl->eMainPath = eMrvPathOn;
            ptMrvMiCtrl->eSelfPath = eMrvPathDpcc;
            break;   	
    	case 0xA: //1010
            ptMrvMiCtrl->eMainPath = eMrvPathDpcc;
            ptMrvMiCtrl->eSelfPath = eMrvPathOn;
            break;   	
    	case 0xC: //1100
            ptMrvMiCtrl->eMainPath = eMrvPathJpe;
            ptMrvMiCtrl->eSelfPath = eMrvPathDpcc;
            break;   	
    	case 0xD: //1101
            ptMrvMiCtrl->eMainPath = eMrvPathDpcc;
            ptMrvMiCtrl->eSelfPath = eMrvPathOff;
            break;   	
    	case 0xE: //1110
            ptMrvMiCtrl->eMainPath = eMrvPathOff;
            ptMrvMiCtrl->eSelfPath = eMrvPathDpcc;
            break;   	
    	case 0xF: //1111
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
                if (REG_GET_SLICE(mi_ctrl, MRV_MI_MP_WRITE_FORMAT) == MRV_MI_MP_WRITE_FORMAT_INTERLEAVED)
                {
                    ptMrvMiCtrl->eMainPath = eMrvPathRaw16;
    }
    else
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3)
    {
                    ptMrvMiCtrl->eMainPath = eMrvPathRaw;
                }
            ptMrvMiCtrl->eSelfPath = eMrvPathDpcc;
            break;   	
            	
    	case 0x5: //0101
    	case 0x7: //0111
    	case 0xB: //1011
        default:
            DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): unknown  MI path  value (0x%08X)\n", MiPath));
            return RET_FAILURE;
    }

    // main picture path IRQ offset
    ptMrvMiCtrl->ulIrqOffsInit = REG_GET_SLICE(ptMrvReg->mi_mp_y_irq_offs_init, MRV_MI_MP_Y_IRQ_OFFS_INIT);

    // self picture path output color format
 #if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    switch (REG_GET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT))
    {
    case MRV_MI_SP_OUTPUT_FORMAT_YUV422:
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_YCbCr422;
        break;
    case MRV_MI_SP_OUTPUT_FORMAT_YUV444:
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_YCbCr444;
        break;
    case MRV_MI_SP_OUTPUT_FORMAT_YUV420:
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_YCbCr420;
        break;
    case MRV_MI_SP_OUTPUT_FORMAT_YUV400:
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_YCbCr400;
        break;
    case MRV_MI_SP_OUTPUT_FORMAT_RGB565:
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_565;
        break;
    case MRV_MI_SP_OUTPUT_FORMAT_RGB888:
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_888;
        break;
    case MRV_MI_SP_OUTPUT_FORMAT_RGB666:
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_666;
        break;
    default:
        DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): unknown self picture path output type value (0x%08X)\n", REG_GET_SLICE(mi_ctrl, MRV_MI_SP_OUTPUT_FORMAT)));
        return RET_FAILURE;
    }

#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    if (REG_GET_SLICE(mi_ctrl, MRV_MI_RGB_EN))
    {
        switch (REG_GET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT))
        {
        case MRV_MI_RGB_FORMAT_565:
            ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_565;
    	    break;
        case MRV_MI_RGB_FORMAT_888:
            ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_888;
    	    break;
        case MRV_MI_RGB_FORMAT_666:
            ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_666;
    	    break;
        default:
            DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): unknown self picture path output type value (0x%08X)\n", REG_GET_SLICE(mi_ctrl, MRV_MI_RGB_EN)));
            return RET_FAILURE;
        }
    }
    else
    {
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_YCbCr422;
    }

#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    if (REG_GET_SLICE(mi_ctrl, MRV_MI_RGB_EN))
    {
        switch (REG_GET_SLICE(mi_ctrl, MRV_MI_RGB_FORMAT))
        {
        case MRV_MI_RGB_FORMAT_888:
            ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_888;
    	    break;
        case MRV_MI_RGB_FORMAT_565:
        default:
            ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_RGB_565;
            break;
        }
    }
    else
    {
        ptMrvMiCtrl->eMrvMifSpOutForm = eMrvMifCol_Format_YCbCr422;
    }

#else // MARVIN_FEATURE_MI
    #error "unknown MARVIN_FEATURE_MI value"
#endif // MARVIN_FEATURE_MI

    // self picture path input color format
 #if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    switch (REG_GET_SLICE(mi_ctrl, MRV_MI_SP_INPUT_FORMAT))
    {
    case MRV_MI_SP_INPUT_FORMAT_YUV422:
        ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr422;
    	break;
    case MRV_MI_SP_INPUT_FORMAT_YUV444:
        ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr444;
    	break;
    case MRV_MI_SP_INPUT_FORMAT_YUV420:
        ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr420;
    	break;
    default:
        ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr400;
    	break;
    }
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4) 
    switch (REG_GET_SLICE(mi_ctrl, MRV_MI_Y_FULL_RANGE))
    {
    case 1:
        ptMrvMiCtrl->eMrvMifSpInRange = eMrvMifCol_Range_Full;
        break;
    case 0:
        ptMrvMiCtrl->eMrvMifSpInRange = eMrvMifCol_Range_Std;
        break;
    default:
        DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): unknown self picture path input range value (0x%08X)\n", REG_GET_SLICE(mi_ctrl, MRV_MI_Y_FULL_RANGE)));
        return RET_FAILURE;
    }
    
    switch (REG_GET_SLICE(mi_ctrl, MRV_MI_422NONCOSITED))
    {
    case 1:
        ptMrvMiCtrl->eMrvMifSpInPhase = eMrvMifCol_Phase_Non_Cosited;
        break;
    case 0:
        ptMrvMiCtrl->eMrvMifSpInPhase = eMrvMifCol_Phase_Cosited;
        break;
    default:
        DBG_OUT((DERR, "MrvMif_GetPathAndOrientation(): unknown self picture path input phase value (0x%08X)\n", REG_GET_SLICE(mi_ctrl, MRV_MI_422NONCOSITED)));
        return RET_FAILURE;
    }
#endif // (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4) 

#else // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)
    ptMrvMiCtrl->eMrvMifSpInForm = eMrvMifCol_Format_YCbCr422;
#endif // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

    // main and self picture path storage format
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    switch (REG_GET_SLICE(mi_ctrl, MRV_MI_SP_WRITE_FORMAT))
    {
    case MRV_MI_SP_WRITE_FORMAT_INTERLEAVED:
        ptMrvMiCtrl->eMrvMifSpPicForm = eMrvMif_PicForm_Interleaved;
        break;
    case MRV_MI_SP_WRITE_FORMAT_SEMIPLANAR:
        ptMrvMiCtrl->eMrvMifSpPicForm = eMrvMif_PicForm_SemiPlanar;
        break;
    case MRV_MI_SP_WRITE_FORMAT_PLANAR:
    default:
        ptMrvMiCtrl->eMrvMifSpPicForm = eMrvMif_PicForm_Planar;
        break;
    }

    switch (REG_GET_SLICE(mi_ctrl, MRV_MI_MP_WRITE_FORMAT))
    {
    case MRV_MI_MP_WRITE_FORMAT_INTERLEAVED:
        ptMrvMiCtrl->eMrvMifMpPicForm = eMrvMif_PicForm_Interleaved;
    	break;
    case MRV_MI_MP_WRITE_FORMAT_SEMIPLANAR:
        ptMrvMiCtrl->eMrvMifMpPicForm = eMrvMif_PicForm_SemiPlanar;
    	break;
    case MRV_MI_MP_WRITE_FORMAT_PLANAR:
    default:
        ptMrvMiCtrl->eMrvMifMpPicForm = eMrvMif_PicForm_Planar;
    	break;
    }

#elif ((MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2) || (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1))
    if (REG_GET_SLICE(mi_ctrl, MRV_MI_SEMI_PLANAR_EN))
    {
        ptMrvMiCtrl->eMrvMifSpPicForm = eMrvMif_PicForm_SemiPlanar;
    }
    else
    {
        ptMrvMiCtrl->eMrvMifSpPicForm = eMrvMif_PicForm_Planar;
    }
    ptMrvMiCtrl->eMrvMifMpPicForm = ptMrvMiCtrl->eMrvMifSpPicForm;

#else // MARVIN_FEATURE_MI
    #error "unknown MARVIN_FEATURE_MI value"
#endif // MARVIN_FEATURE_MI

#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))

    // burst length chrominances
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    if (REG_GET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_CHROM) == MRV_MI_BURST_LEN_CHROM_16)
    {
        ptMrvMiCtrl->eBurstLengthChrom = eMrvMif_BurstLength_16;
    }
    else 
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    {
    	if (REG_GET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_CHROM) == MRV_MI_BURST_LEN_CHROM_8)
      {
          ptMrvMiCtrl->eBurstLengthChrom = eMrvMif_BurstLength_8;
      }
      else
      {
          ptMrvMiCtrl->eBurstLengthChrom = eMrvMif_BurstLength_4;
      }
    }

    // burst length luminances
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    if (REG_GET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_LUM) == MRV_MI_BURST_LEN_LUM_16)
    {
        ptMrvMiCtrl->eBurstLengthLum = eMrvMif_BurstLength_16;
    }
    else 
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
    {
    	if (REG_GET_SLICE(mi_ctrl, MRV_MI_BURST_LEN_LUM) == MRV_MI_BURST_LEN_LUM_8)
    	{
         ptMrvMiCtrl->eBurstLengthLum = eMrvMif_BurstLength_8;
    	}
    	else
    	{
     	   ptMrvMiCtrl->eBurstLengthLum = eMrvMif_BurstLength_4;
    	}
    }

    // initial values
    switch ((REG_GET_SLICE(mi_ctrl, MRV_MI_INIT_OFFSET_EN) ? 2 : 0) +
            (REG_GET_SLICE(mi_ctrl, MRV_MI_INIT_BASE_EN)   ? 1 : 0)  )
    {
    case 3: // MRV_MI_INIT_OFFSET_EN + MRV_MI_INIT_BASE_EN
        ptMrvMiCtrl->eInitVals = eMrvMif_InitOffsAndBase;
        break;
    case 2: // MRV_MI_INIT_OFFSET_EN
        ptMrvMiCtrl->eInitVals = eMrvMif_InitOffs;
        break;
    case 1: // MRV_MI_INIT_BASE_EN
        ptMrvMiCtrl->eInitVals = eMrvMif_InitBase;
        break;
    case 0: // both disabled
    default:
        ptMrvMiCtrl->eInitVals = eMrvMif_NoInitVals;
        break;
    }

#elif ((MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2) || (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1))

    // burst length chrominances
    ptMrvMiCtrl->eBurstLengthChrom = eMrvMif_BurstLength_4;

    // burst length luminances
    ptMrvMiCtrl->eBurstLengthLum = eMrvMif_BurstLength_4;

    // initial values
    if (REG_GET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_OUT) || REG_GET_SLICE(mi_ctrl, MRV_MI_CFG_UPD_IN))
    {
        ptMrvMiCtrl->eInitVals = eMrvMif_InitOffsAndBase;
    }
    else
    {
        ptMrvMiCtrl->eInitVals = eMrvMif_NoInitVals;
    }

#else // MARVIN_FEATURE_MI
    #error "unknown MARVIN_FEATURE_MI value"
#endif // MARVIN_FEATURE_MI

    // enable byte swapping
    ptMrvMiCtrl->bByteSwapEnable = REG_GET_SLICE(mi_ctrl, MRV_MI_BYTE_SWAP) ? TRUE : FALSE;

#if (MARVIN_FEATURE_MI_LAST_PIXEL == MARVIN_FEATURE_EXIST)
    // enable or disable the last pixel signalization
    ptMrvMiCtrl->bLastPixelEnable = REG_GET_SLICE(mi_ctrl, MRV_MI_LAST_PIXEL_SIG_EN) ? TRUE : FALSE;
#endif //#if (MARVIN_FEATURE_MI_LAST_PIXEL == MARVIN_FEATURE_EXIST)

    // self picture path orientation
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))

    switch (REG_GET_SLICE(mi_ctrl, MRV_MI_ROT_AND_FLIP))
    {
    case MRV_MI_ROT_AND_FLIP_H_FLIP:
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Horizontal_Flip;
    	break;
    case MRV_MI_ROT_AND_FLIP_V_FLIP:
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Vertical_Flip;
    	break;
    case MRV_MI_ROT_AND_FLIP_ROTATE:
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rotation_090_deg;
    	break;
    case (MRV_MI_ROT_AND_FLIP_H_FLIP | MRV_MI_ROT_AND_FLIP_V_FLIP):
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rotation_180_deg;
    	break;
    case (MRV_MI_ROT_AND_FLIP_H_FLIP | MRV_MI_ROT_AND_FLIP_V_FLIP | MRV_MI_ROT_AND_FLIP_ROTATE):
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rotation_270_deg;
    	break;
    case (MRV_MI_ROT_AND_FLIP_V_FLIP | MRV_MI_ROT_AND_FLIP_ROTATE):
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rot_090_V_Flip;
    	break;
    case (MRV_MI_ROT_AND_FLIP_H_FLIP | MRV_MI_ROT_AND_FLIP_ROTATE):
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Rot_270_V_Flip;
    	break;
    default:
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Original;
        break;
    }

#elif ((MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2) || (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1))

    if (REG_GET_SLICE(mi_ctrl, MRV_MI_H_FLIP))
    {
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Horizontal_Flip;
    }
    else
    {
        ptMrvMiCtrl->eMrvMifSpMode = eMrvMifSp_Original;
    }

#else // MARVIN_FEATURE_MI
    #error "unknown MARVIN_FEATURE_MI value"
#endif // MARVIN_FEATURE_MI

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_SetConfiguration \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - one or more pointers were NULL \n
 *               RET_OUTOFRANGE   - one or more of the values are out of
 *                                  range \n
 *               RET_SUCCESS      - the desired settings have been applied \n
 *
 *  \PARAMETERS  ptMrvMiDmaConf   - pointer to tsMrvMiDmaConf structure \n
 *
 *  \DESCRIPTION Configures the complete MI, if possible. \n
 *
 *  \NOTES       This function should be only called in the case that the
 *               self picture path processes currently no data. Otherwise it
 *               will result in unpredictable behaviour of the MI. \n
 */
/*****************************************************************************/
RESULT MrvMif_SetConfiguration(const tsMrvMiCtrl     *ptMrvMiCtrl,
                               const tsMrvMiPathConf *ptMrvMiMpPathConf,
                               const tsMrvMiPathConf *ptMrvMiSpPathConf,
                               const tsMrvMiDmaConf  *ptMrvMiDmaConf    )
{
    RESULT iError = RET_FAILURE;

    iError = MrvMif_SetPathAndOrientation(ptMrvMiCtrl);
    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetConfiguration(): MrvMif_SetPathAndOrientation() returned 0x%08X\n", iError));
        return iError;
    }

	if (ptMrvMiCtrl->eMainPath != eMrvPathOff)
	{
	    iError = MrvMif_SetMainBuffer(ptMrvMiMpPathConf, eMrvCfgUpdateImmediate);
	    if (iError != RET_SUCCESS)
	    {
	        DBG_OUT((DERR, "MrvMif_SetConfiguration(): MrvMif_SetMainBuffer() returned 0x%08X\n", iError));
	        return iError;
	    }
	}
    // set self picture path
	if (ptMrvMiCtrl->eSelfPath != eMrvPathOff)
	{
	    iError = MrvMif_SetSelfBuffer(ptMrvMiSpPathConf, eMrvCfgUpdateImmediate);
	    if (iError != RET_SUCCESS)
	    {
	        DBG_OUT((DERR, "MrvMif_SetConfiguration(): MrvMif_SetSelfBuffer() returned 0x%08X\n", iError));
	        return iError;
	    }
	}
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    // set DMA configuration
    iError = MrvMif_SetDmaConfig(ptMrvMiDmaConf);
    if (iError != RET_SUCCESS)
    {
        DBG_OUT((DERR, "MrvMif_SetConfiguration(): MrvMif_SetDmaConfig() returned 0x%08X\n", iError));
        return iError;
    }
#else // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    UNUSED_PARAM(ptMrvMiDmaConf);                     // added to avoid LINT warnings (Info 715)
#endif // #if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)

    return iError;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMif_GetPixelPer32BitOfLine \n
 *
 *  \RETURNVALUE RET_NULL_POINTER - the pointer is NULL \n
 *               RET_SUCCESS      - everything is okay, the returned value is
 *                                  valid \n
 *               RET_FAILURE      - something went wrong, the returned value
 *                                  is not valid and should not be used \n
 *
 *  \PARAMETERS  pucPixelPer32Bit - pointer where to store the return value \n
 *               eMrvMifSpFormat  - contains the desired output format \n
 *               eMrvMifPicForm   - contains the desired buffer usage \n
 *               bLuminanceBuffer - if TRUE the amount of pixels per 32 bit of
 *                                  the Y buffer is returned, if FALSE the
 *                                  amount of pixels per 32 bit of one of the
 *                                  chrominance buffers is returned \n
 *
 *  \DESCRIPTION Returns the amount of pixels per 32 bits of the desired
 *               buffer, if possible. This is calculated for the first line,
 *               so one has to pay attention that some formats (e.g. YCbCr
 *               4:2:0) use chrominance values for more than one line.
 *               In the case the desired buffer is unused, or a forbidden
 *               combination of eMrvMifSpFormat and eMrvMifPicForm is
 *               chosen, the RESULT value will be RET_FAILURE. \n
 *
 *  \NOTES       Some eMrvMifSpFormat values result in writing 32 bits, but not
 *               all of the 32 bits are used, e.g. RGB888 one complete byte is
 *               unused. Therefore one pixel could be stored in 32 bits (8 bits
 *               unused, 8 bits for red, 8 bits for green and 8 bits for
 *               blue). \n
 */
/*****************************************************************************/
RESULT MrvMif_GetPixelPer32BitOfLine(UINT8             *pucPixelPer32Bit,
                                     teMrvMifColFormat  eMrvMifSpFormat,
                                     teMrvMifPicForm    eMrvMifPicForm,
                                     BOOL               bLuminanceBuffer )
{
    if (!pucPixelPer32Bit)
    {
        return RET_NULL_POINTER;
    }

    switch (eMrvMifPicForm)
    {
    case eMrvMif_PicForm_Planar:
        switch (eMrvMifSpFormat)
        {
        case eMrvMifCol_Format_YCbCr444:
            *pucPixelPer32Bit = 4;
            return RET_SUCCESS;

        case eMrvMifCol_Format_YCbCr422:
        case eMrvMifCol_Format_YCbCr420:
            if (bLuminanceBuffer)
            {
                *pucPixelPer32Bit = 4;
            }
            else
            {
                *pucPixelPer32Bit = 8;
            }
            return RET_SUCCESS;

        case eMrvMifCol_Format_YCbCr400:
            if (bLuminanceBuffer)
            {
                *pucPixelPer32Bit = 4;
                return RET_SUCCESS;
            }
            break;

        case eMrvMifCol_Format_RGB_565:
            if (bLuminanceBuffer)
            {
                *pucPixelPer32Bit = 2;
                return RET_SUCCESS;
            }
            break;

        case eMrvMifCol_Format_RGB_666:
        case eMrvMifCol_Format_RGB_888:
            if (bLuminanceBuffer)
            {
                *pucPixelPer32Bit = 1;
                return RET_SUCCESS;
            }
            break;

        default:
            break;
        }
        break;

    case eMrvMif_PicForm_SemiPlanar:
        switch (eMrvMifSpFormat)
        {
        case eMrvMifCol_Format_YCbCr422:
        case eMrvMifCol_Format_YCbCr420:
            *pucPixelPer32Bit = 4;
            return RET_SUCCESS;

        default:
            break;
        }
        break;

    case eMrvMif_PicForm_Interleaved:
        switch (eMrvMifSpFormat)
        {
        case eMrvMifCol_Format_YCbCr422:
            *pucPixelPer32Bit = 2;
            return RET_SUCCESS;

        default:
            break;
        }
        break;

    default:
        break;
    }

    *pucPixelPer32Bit = 0;
    return RET_FAILURE;
}

