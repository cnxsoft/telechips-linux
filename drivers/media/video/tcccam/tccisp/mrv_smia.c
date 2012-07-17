/*****************************************************************************/
/*!
 *  \file        mrv_smia.c \n
 *  \version     1.0 \n
 *  \author      Zupp \n
 *  \brief       Marvin SMIA low level driver functionality. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_smia.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */ 
/*****************************************************************************/

#include "stdinc.h"

#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    #include "reg_access.h"
    #include "mrv_priv.h"
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information

#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    #define MAX_NUMBER_OF_SMIA_DMA_CHANNELS     8
#endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaInitializeModule \n
*  \RETURNVALUE RET_SUCCESS: In case of successful register initialization. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  none \n
*  \DESCRIPTION Initializes SMIA filter registers with default reset values. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaInitializeModule(void)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 smia_ctrl = 0; // current register value uninterested

    REG_SET_SLICE(smia_ctrl, MRV_SMIA_OUTPUT_ENA,  DISABLE);
    REG_SET_SLICE(smia_ctrl, MRV_SMIA_FLUSH_FIFO,      OFF); // [hf]: should this be set to ON to clear fifo ?
    //REG_SET_SLICE(smia_ctrl, MRV_SMIA_CFG_UPD_ENA, DISABLE); // Slice removed from HW
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    REG_SET_SLICE(smia_ctrl, MRV_SMIA_SHUTDOWN_LANE,   OFF);
    REG_SET_SLICE(smia_ctrl, MRV_SMIA_DMA_CHANNEL_SEL,   0);
  #endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    REG_WRITE(ptMrvReg->smia_ctrl, smia_ctrl);

    REG_SET_SLICE(ptMrvReg->smia_imsc, MRV_SMIA_IMSC_ALL_IRQS, OFF); // mask out all IRQs
    REG_SET_SLICE(ptMrvReg->smia_icr,  MRV_SMIA_ICR_ALL_IRQS,   ON); // reset all IRQs
    REG_SET_SLICE(ptMrvReg->smia_isr,  MRV_SMIA_ISR_ALL_IRQS,  OFF); // don't set IRQs - not required?

    REG_SET_SLICE(ptMrvReg->smia_data_format_sel,    MRV_SMIA_DATA_FORMAT_SEL,    0);
    REG_SET_SLICE(ptMrvReg->smia_sof_emb_data_lines, MRV_SMIA_SOF_EMB_DATA_LINES, 0);
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    REG_SET_SLICE(ptMrvReg->smia_emb_hstart1,        MRV_SMIA_EMB_HSTART1,        0);
    REG_SET_SLICE(ptMrvReg->smia_emb_hsize1,         MRV_SMIA_EMB_HSIZE1,         0);
    REG_SET_SLICE(ptMrvReg->smia_emb_vstart1,        MRV_SMIA_EMB_VSTART1,        0);
    REG_SET_SLICE(ptMrvReg->smia_emb_vsize1,         MRV_SMIA_EMB_HSIZE1,         0);
    REG_SET_SLICE(ptMrvReg->smia_emb_hstart2,        MRV_SMIA_EMB_HSTART2,        0);
    REG_SET_SLICE(ptMrvReg->smia_emb_hsize2,         MRV_SMIA_EMB_HSIZE2,         0);
    REG_SET_SLICE(ptMrvReg->smia_emb_vstart2,        MRV_SMIA_EMB_VSTART2,        0);
    REG_SET_SLICE(ptMrvReg->smia_emb_vsize2,         MRV_SMIA_EMB_HSIZE2,         0);
  #else // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    REG_SET_SLICE(ptMrvReg->smia_emb_hstart,         MRV_SMIA_EMB_HSTART,         0);
    REG_SET_SLICE(ptMrvReg->smia_emb_hsize,          MRV_SMIA_EMB_HSIZE,          0);
    REG_SET_SLICE(ptMrvReg->smia_emb_vstart,         MRV_SMIA_EMB_VSTART,         0);
  #endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    REG_SET_SLICE(ptMrvReg->smia_num_lines,          MRV_SMIA_NUM_LINES,          0);

  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    REG_SET_SLICE(ptMrvReg->smia_fifo_fill_level,    MRV_SMIA_FIFO_FILL_LEVEL,    0); // [hf]: expedient value ???
  #endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP)

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}

/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetControlRegisterStatus \n
*  \RETURNVALUE RET_SUCCESS: In case of successful SMIA control register request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puSmiaControlRegisterStatus - Current SMIA control register \n
*                                             status will be written into this \n
*                                             variable. \n
*  \DESCRIPTION Function returns the current SMIA control register status. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaGetControlRegisterStatus(UINT32 *puSmiaControlRegisterStatus)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puSmiaControlRegisterStatus)
    {
        return RET_NULL_POINTER;
    }

    *puSmiaControlRegisterStatus = REG_READ(ptMrvReg->smia_ctrl);

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puSmiaControlRegisterStatus);               // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetControlRegisterStatus \n
*  \RETURNVALUE RET_SUCCESS: In case of successful SMIA control register modification. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  uSmiaControlRegisterStatus = New control register value. \n
*  \DESCRIPTION Function sets a new control register value. \n
*  \NOTES       With this function the complete control register will be \n
*               overwritten with the given value. Please use one of the following  \n
*               functions to modify  j u s t  one control register element value. \n
*
*****************************************************************************/
RESULT MrvSmiaSetControlRegisterStatus(const UINT32 uSmiaControlRegisterStatus)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    REG_WRITE(ptMrvReg->smia_ctrl, uSmiaControlRegisterStatus);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uSmiaControlRegisterStatus);               // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaActivateModule \n
*  \RETURNVALUE RET_SUCCESS: In case of successful module activation/deactivation. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  bModuleActive = TRUE  : SMIA module is activated. \n
*               bModuleActive = FALSE : SMIA module is deactivated. \n
*  \DESCRIPTION Activate/deactivates SMIA module actions. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaActivateModule(const BOOL bModuleActive)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32                  ulRegval = 0;

/*
    // debugging
    IsiReg_DumpAll( stdout );
*/

    /*
     *	Note:
     *      Don't change the other important bits of the register !!!
     */
    ulRegval = *(UINT32*)(MEM_CSC_REG_BASE + 0x50);
/*
    // debugging
    DBG_OUT( (DINFO, "Multiplexer = 0x%08X\n", ulRegval) );
*/
    // Clear multiplexer mode bits.
    ulRegval &= ~0x03000000;
    // Set multiplexer to mode 3 (SMIA).
    ulRegval |= 0x03000000;
    // Write the new mode bits into the register.
    *(UINT32*)(MEM_CSC_REG_BASE + 0x50) = ulRegval;

    // [hf]: vi_dpcl register ???

    REG_SET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_OUTPUT_ENA, (bModuleActive) ? ENABLE : DISABLE);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(bModuleActive);                            // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetDmaChannel \n
*  \RETURNVALUE RET_SUCCESS: In case of successful dma channel modification. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*               RET_OUTOFRANGE: One or more parameter values are out of range. \n
*  \PARAMETERS  uDmaChannel = SMIA module has to generate picture output data \n
*                             from this DMA channel. \n
*  \DESCRIPTION Function sets the SMIA module DMA channel for output picture 
*               data generation. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaSetDmaChannel(const UINT8 uDmaChannel)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (uDmaChannel >= MAX_NUMBER_OF_SMIA_DMA_CHANNELS)
    {
        return RET_OUTOFRANGE;
    }

    // Write the new Dma channel parameter value into SMIA control register
    REG_SET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_DMA_CHANNEL_SEL, uDmaChannel);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE)
    UNUSED_PARAM1(uDmaChannel);                              // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetDmaChannel \n
*  \RETURNVALUE RET_SUCCESS: In case of successful dma channel request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puDmaChannel = Into this variable the current SMIA output picture
*               data DMA channel will be written. \n
*  \DESCRIPTION Function returns the current SMIA output picture data DMA channel. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaGetDmaChannel(UINT8 *puDmaChannel)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puDmaChannel)
    {
        return RET_NULL_POINTER;
    }

    // Return current SMIA output picture data DMA channel
    *puDmaChannel = REG_GET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_DMA_CHANNEL_SEL);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE)
    UNUSED_PARAM(puDmaChannel);                              // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetCurrentDmaChannel \n
*  \RETURNVALUE RET_SUCCESS: Successful embedded data fifo request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puDmaChannel - The number of the currently received SMIA picture \n
*               data DMA channel will be written into this variable. \n
*  \DESCRIPTION Function returns the number of the currently received SMIA \n
*               picture data DMA channel. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaGetCurrentDmaChannel(BOOL *puDmaChannel)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puDmaChannel)
    {
        return RET_NULL_POINTER;
    }

    *puDmaChannel = REG_GET_SLICE(ptMrvReg->smia_status, MRV_SMIA_DMA_CHANNEL);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE)
    UNUSED_PARAM(puDmaChannel);                              // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetShutdownLane \n
*  \RETURNVALUE RET_SUCCESS: In case of successful shutdown lane modification. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  bShutdownLane = New SMIA shutdown lane value. \n
*  \DESCRIPTION Function sets the new SMIA shutdown lane value (SMIA module \n
*               output signal. Can be used as reset signal).  \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaSetShutdownLane(const BOOL bShutdownLane)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    REG_SET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_SHUTDOWN_LANE, (bShutdownLane) ? ON : OFF);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE)
    UNUSED_PARAM1(bShutdownLane);                            // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetShutdownLane \n
*  \RETURNVALUE RET_SUCCESS: In case of successful shutdown lane request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  pbShutdownLane = Current SMIA shutdown lane value value will be \n
*               written into this variable. \n
*  \DESCRIPTION Function returns the current SMIA shutdown lane value (SMIA module \n
*               output signal. Can be used as reset signal).  \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaGetShutdownLane(BOOL *pbShutdownLane)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!pbShutdownLane)
    {
        return RET_NULL_POINTER;
    }

    // Return the current shutdown lane value
    *pbShutdownLane = (REG_GET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_SHUTDOWN_LANE)) ? TRUE : FALSE;

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE)
    UNUSED_PARAM(pbShutdownLane);                            // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaActivateCfgUpdateSignal \n
*  \RETURNVALUE RET_SUCCESS: In case of successful cfg update signal activation/deactivation. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  bCfgUpdateSignalActive = TRUE  : cfg update signal is activated. \n
*               bCfgUpdateSignalActive = FALSE : cfg update signal is deactivated. \n
*  \DESCRIPTION At the end of each frame the hardware cfg update signal could be set. \n
*               This will be acivated/deactivated by the use of this function. \n
*  \NOTES       Because the corresponding line is removed from hardware
*               this function always returns RET_NOTSUPP. \n
*
*****************************************************************************/
RESULT MrvSmiaActivateCfgUpdateSignal(const BOOL bCfgUpdateSignalActive)
{
//#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
//    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
//
//    REG_SET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_CFG_UPD_ENA, (bCfgUpdateSignalActive) ? ENABLE : DISABLE);
//
//    return RET_SUCCESS; 
//#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(bCfgUpdateSignalActive);                   // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
//#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaFreeEmbDataFifo \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data fifo reset. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  none \n
*  \DESCRIPTION Function resets the SMIA embedded data fifo memory. This means \n
*               that the embedded data fifo read and write pointer will be set \n
*               to the start position. Not read embedded data will be discarded \n
*               after this action. Furthermore this function call resets the \n
*               EMB_DATA_OVFLW interrupt. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaFreeEmbDataFifo(void)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // Set FLUSH_FIFO bit - Reset SMIA embedded data fifo read and write pointer
    // This bit have to be set just for one CPU clock...the rest will be done on hardware site.
    REG_SET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_FLUSH_FIFO, ON);

    // Reset SMIA EMB_DATA_OVFLW interrupt
  #if ( MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE )
    REG_SET_SLICE(ptMrvReg->smia_icr, MRV_SMIA_ICR_SYNC_FIFO_OVFLW, ON);
  #else // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    REG_SET_SLICE(ptMrvReg->smia_icr, MRV_SMIA_ICR_EMB_DATA_OVFLW, ON);
  #endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)

    // Unset FLUSH_FIFO bit
    REG_SET_SLICE(ptMrvReg->smia_ctrl, MRV_SMIA_FLUSH_FIFO, OFF);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaEmbDataAvailabe \n
*  \RETURNVALUE RET_SUCCESS: Successful embedded data fifo request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  pbEmbDataAvail - TRUE: In case of available embedded data.\n
*                                FALSE: There is no embedded data stored in fifo. \n
*  \DESCRIPTION Function determines the availability of embedded data. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaEmbDataAvailabe(BOOL *pbEmbDataAvail)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!pbEmbDataAvail)
    {
        return RET_NULL_POINTER;
    }

    *pbEmbDataAvail = (REG_GET_SLICE(ptMrvReg->smia_status, MRV_SMIA_EMB_DATA_AVAIL)) ? TRUE : FALSE;

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(pbEmbDataAvail);                            // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaActivateInterrupts \n
*  \RETURNVALUE RET_SUCCESS: In case of successful interrupt register \n
*                            activation/deactivation. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  uActivatedInterrupt - Every bit of this parameter activates or \n
*                                     deactivates a specific SMIA interrupt \n
*                                     ("1" = interrupt is activated; "0" = interrupt 
*                                     is deactivated). \n
*  \DESCRIPTION Function activates/deactivate SMIA interrupts. \n 
*  \NOTES       Combine interrupt mask defines (see mrv_smia.h) as parater value \n
*               for a secure parameter handling \n
*               (f.e. SMIA_INTRPT_EMB_DATA_OVFLW | SMIA_INTRPT_FRAME_END). \n
*
*****************************************************************************/
RESULT MrvSmiaActivateInterrupts(const UINT32 uActivatedInterrupt)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (uActivatedInterrupt & ~ MRV_SMIA_IMSC_ALL_IRQS_MASK)
    {
        return RET_OUTOFRANGE;
    }

    // Activate/Deactivate SMIA interrupts
    REG_WRITE(ptMrvReg->smia_imsc, uActivatedInterrupt);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uActivatedInterrupt);                      // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetImscRegister \n
*  \RETURNVALUE RET_SUCCESS: In case of successful interrupt register request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puActivatedInterrupt - Every bit of this variable will be set \n
*                                      in case of an activated corresponding \n
*                                      interrupt. Is case of a deactivated \n
*                                      interrupt the bit will be set to "0". \n
*  \DESCRIPTION Function determines the activated or deactivated status of all \n
*               SMIA interrupts. \n 
*  \NOTES       Combine interrupt mask defines (see mrv_smia.h) to analyse the pointer \n
*               variable content. \n 
*               (f.e. SMIA_INTRPT_EMB_DATA_OVFLW | SMIA_INTRPT_FRAME_END). \n
*
*****************************************************************************/
RESULT MrvSmiaGetImscRegister(UINT32 *puActivatedInterrupt)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puActivatedInterrupt)
    {
        return RET_NULL_POINTER;
    }

    // Determine activated and deactivated register status
    *puActivatedInterrupt = REG_READ(ptMrvReg->smia_imsc);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puActivatedInterrupt);                      // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetGeneralInterruptState \n
*  \RETURNVALUE RET_SUCCESS: In case of successful general interrupt status request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puGeneralInterruptState - Pointer variable will be used to return \n
*                                         the general interrupt status value. \n
*  \DESCRIPTION Function determines the current status of activated AND \n
*               deactivated SMIA interrupts. \n 
*  \NOTES       Combine interrupt mask defines (see mrv_smia.h) to analyse the pointer \n
*               variable content. \n 
*               (f.e. SMIA_INTRPT_EMB_DATA_OVFLW | SMIA_INTRPT_FRAME_END). \n
*
*****************************************************************************/
RESULT MrvSmiaGetGeneralInterruptState(UINT32 *puGeneralInterruptState)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puGeneralInterruptState)
    {
        return RET_NULL_POINTER;
    }

    // return the interrupt status of activated AND deactivated SMIA interrupts
    *puGeneralInterruptState = REG_READ(ptMrvReg->smia_ris);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puGeneralInterruptState);                   // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetActivatedInterruptState \n
*  \RETURNVALUE RET_SUCCESS: In case of successful activated interrupt status request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puActivatedInterruptState - Pointer variable will be used to return \n
*                                           the activated interrupt status value. \n
*  \DESCRIPTION Function determines ONLY the current status of the activated SMIA \n
*               interrupts. \n
*  \NOTES       Combine interrupt mask defines (see mrv_smia.h) to analyse the pointer \n
*               variable content. \n 
*               (f.e. SMIA_INTRPT_EMB_DATA_OVFLW | SMIA_INTRPT_FRAME_END). \n
*
*****************************************************************************/
RESULT MrvSmiaGetActivatedInterruptState(UINT32 *puActivatedInterruptState)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puActivatedInterruptState)
    {
        return RET_NULL_POINTER;
    }

    // return ONLY the interrupt status of activated SMIA interrupts
    *puActivatedInterruptState = REG_READ(ptMrvReg->smia_mis);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puActivatedInterruptState);                 // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaResetInterrups \n
*  \RETURNVALUE RET_SUCCESS: In case of successful interrupt register reset. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  none \n
*  \DESCRIPTION Function resets all SMIA interrupt values. After this reset \n
*               action the SMIA_ICR register will automatically set back to \n
*               the value 0x00000000.
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaResetInterrups(void)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // Reset  a l l  SMIA interrupts
    // The register bits will be automatically set back to "0" by the hardware
    REG_WRITE(ptMrvReg->smia_icr, MRV_SMIA_ICR_ALL_IRQS_MASK);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetInterruptRegister \n
*  \RETURNVALUE RET_SUCCESS: In case of successful SMIA interrupt register manipulation. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  uInterruptRegisterMask - Mask to determine which SMIA register value \n
*                                        should be modified. \n
*               uNewInterruptRegisterValue - New interrupt register value. \n
*  \DESCRIPTION Function sets specific SMIA interrupts (given by the mask) to specific \n
*               new values. \n
*  \NOTES       Be careful to use this function to avoid some strange behavior. Basically \n
*               this function was implemented just for test activities. \n
*
*               Ensure that all uNewInterruptRegisterValue parameter bits are correct set \n
*               for thoose register values that should be modified.\n
*
*               Please use interrupt mask defines (see mrv_smia.h) as mask parater value \n
*               for a secure parameter handling 
*               (f.e. SMIA_INTRPT_EMB_DATA_OVFLW | SMIA_INTRPT_FRAME_END as subset or \n
*               SMIA_INTRPT_ALL to modify all interupt values). \n
*
*****************************************************************************/
RESULT MrvSmiaSetInterruptRegister(const UINT32 uInterruptRegisterMask, const UINT32 uNewInterruptRegisterValue)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 smia_isr = REG_READ(ptMrvReg->smia_isr);

    if ((uInterruptRegisterMask | uNewInterruptRegisterValue) & ~ MRV_SMIA_ISR_ALL_IRQS_MASK)
    {
        return RET_OUTOFRANGE;
    }

    // Manipulate the SMIA interrupt register with the given masked value
    smia_isr = (smia_isr & ~uInterruptRegisterMask) | (uNewInterruptRegisterValue & uInterruptRegisterMask);
    REG_WRITE(ptMrvReg->smia_isr, smia_isr);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uInterruptRegisterMask);                   // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uNewInterruptRegisterValue);               // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetPictureFormat \n
*  \RETURNVALUE RET_SUCCESS: In case of successful SMIA output picture data format modification. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*               RET_OUTOFRANGE: One or more parameter values are out of range. \n
*  \PARAMETERS  uPictureFormat = Into this picture format the SMIA module has to \n
*                                convert the incoming SMIA picture data. \n
*  \DESCRIPTION Function defines the SMIA module output picture format. \n
*  \NOTES       Please use the picture output format defines (see mrv_smia.h) \n
*               for a secure parameter handling (f.e. SMIA_PIC_FORMAT_RAW_10). \n
*
*****************************************************************************/
RESULT MrvSmiaSetPictureFormat(const UINT32 uPictureFormat)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    switch (uPictureFormat)
    {
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
    case MRV_SMIA_DATA_FORMAT_SEL_YUV422:
    case MRV_SMIA_DATA_FORMAT_SEL_YUV420:
    case MRV_SMIA_DATA_FORMAT_SEL_RGB444:
    case MRV_SMIA_DATA_FORMAT_SEL_RGB565:
    case MRV_SMIA_DATA_FORMAT_SEL_RGB888:
    case MRV_SMIA_DATA_FORMAT_SEL_RAW6:
    case MRV_SMIA_DATA_FORMAT_SEL_RAW7:
    case MRV_SMIA_DATA_FORMAT_SEL_COMPRESSED: 
  #endif // MARVIN_FEATURE_SMIA_COMPLETE
    case MRV_SMIA_DATA_FORMAT_SEL_RAW8:
    case MRV_SMIA_DATA_FORMAT_SEL_RAW10:
    case MRV_SMIA_DATA_FORMAT_SEL_RAW12:
    case MRV_SMIA_DATA_FORMAT_SEL_RAW8TO10:
        break;
    default:
        return RET_OUTOFRANGE;
    }

    // Set the new SMIA module output picture format
    REG_SET_SLICE(ptMrvReg->smia_data_format_sel, MRV_SMIA_DATA_FORMAT_SEL, uPictureFormat);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uPictureFormat);                           // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetPictureFormat \n
*  \RETURNVALUE RET_SUCCESS: In case of successful SMIA output picture data format determination. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puPictureFormat - The current SMIA module output picture format \n
*                                 will be witten into this variable. \n
*  \DESCRIPTION Function determines and returns the current SMIA module output \n
*               picture format. \n
*  \NOTES       Please use the picture output format defines (see mrv_smia.h) \n 
*               to check the current format (f.e. SMIA_PIC_FORMAT_RAW_10). \n
*
*****************************************************************************/
RESULT MrvSmiaGetPictureFormat(UINT32 *puPictureFormat)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puPictureFormat)
    {
        return RET_NULL_POINTER;
    }

    // Return the current SMIA module output picture format
    *puPictureFormat = REG_GET_SLICE(ptMrvReg->smia_data_format_sel, MRV_SMIA_DATA_FORMAT_SEL);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puPictureFormat);                           // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetFrameLines \n
*  \RETURNVALUE RET_SUCCESS: In case of successful SMIA frame line definition. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*               RET_OUTOFRANGE: One or more parameter values are out of range. \n
*  \PARAMETERS  uNumOfEmbDataLines - Number of embedded data lines that will be \n
*                                    received before picture data lines. \n
*               uNumOfPicDataLines - Number of picture data lines that will be \n
*                                    received after embedded data lines. \n
*  \DESCRIPTION Function defines the number of SMIA embedded data lines and the  \n
*               number of SMIA picture data lines. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaSetFrameLines(const UINT32 uNumOfEmbDataLines, const UINT32 uNumOfPicDataLines)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // range check
#if ( MRV_SMIA_SOF_EMB_DATA_LINES_MIN == 0 )
  #if ( MRV_SMIA_NUM_LINES_MIN == 0)  
    if ((uNumOfEmbDataLines > MRV_SMIA_SOF_EMB_DATA_LINES_MAX) || (uNumOfPicDataLines > MRV_SMIA_NUM_LINES_MAX)   )
  #else  
    if ((uNumOfPicDataLines < MRV_SMIA_NUM_LINES_MIN) ||
        (uNumOfEmbDataLines > MRV_SMIA_SOF_EMB_DATA_LINES_MAX) || (uNumOfPicDataLines > MRV_SMIA_NUM_LINES_MAX)   )
  #endif 
#else       
  #if ( MRV_SMIA_NUM_LINES_MIN == 0)  
    if ((uNumOfPicDataLines < MRV_SMIA_SOF_EMB_DATA_LINES_MIN) ||
        (uNumOfEmbDataLines > MRV_SMIA_SOF_EMB_DATA_LINES_MAX) || (uNumOfPicDataLines > MRV_SMIA_NUM_LINES_MAX)   )
  #else  
    if ((uNumOfEmbDataLines < MRV_SMIA_SOF_EMB_DATA_LINES_MIN) || (uNumOfPicDataLines < MRV_SMIA_NUM_LINES_MIN) ||
        (uNumOfEmbDataLines > MRV_SMIA_SOF_EMB_DATA_LINES_MAX) || (uNumOfPicDataLines > MRV_SMIA_NUM_LINES_MAX)   )
  #endif
#endif        
    {
        return RET_OUTOFRANGE;
    }

    // Set the number of embedded data lines
    REG_SET_SLICE(ptMrvReg->smia_sof_emb_data_lines, MRV_SMIA_SOF_EMB_DATA_LINES, uNumOfEmbDataLines);
    // Set the number of picture data lines
    REG_SET_SLICE(ptMrvReg->smia_num_lines, MRV_SMIA_NUM_LINES, uNumOfPicDataLines);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uNumOfEmbDataLines);                       // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uNumOfPicDataLines);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetFrameLines \n
*  \RETURNVALUE RET_SUCCESS: In case of successful SMIA frame line request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puNumOfEmbDataLines - The current number of embedded data lines \n
*                                     that will be received before picture data \n
*                                     lines will be written into this variable. \n  
*               puNumOfPicDataLines - The current number of picture data lines  \n
*                                     that will be received after embedded data \n
*                                     lines will be written into this variable. \n
*  \DESCRIPTION Function returns the current number of SMIA embedded data lines \n
*               and the number of SMIA picture data lines. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaGetFrameLines(UINT32 *puNumOfEmbDataLines, UINT32 *puNumOfPicDataLines)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puNumOfEmbDataLines || !puNumOfPicDataLines)
    {
        return RET_NULL_POINTER;
    }

    // Return the number of embedded data lines
    *puNumOfEmbDataLines = REG_GET_SLICE(ptMrvReg->smia_sof_emb_data_lines, MRV_SMIA_SOF_EMB_DATA_LINES);
    // Return the number of picture data lines
    *puNumOfPicDataLines = REG_GET_SLICE(ptMrvReg->smia_num_lines, MRV_SMIA_NUM_LINES);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puNumOfEmbDataLines);                       // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puNumOfPicDataLines);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetEmbDataStorage \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data storage configuration. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*               RET_OUTOFRANGE: One or more parameter values are out of range. \n
*  \PARAMETERS  uEmbDataLine - Embedded data from this SMIA data line will be stored. \n
*                              "0" means that no embedded data will be stored. \n
*                              "1" means embedded data from the first data line \n
*                              will be stored. \n
*                              To store embedded data that will be received \n
*                              behind the picture data lines a value with the \n
*                              number of embedded data lines before picture data \n
*                              lines + number of picture data lines + number of \n
*                              embedded data lines after picture data have to be \n
*                              set as parameter. \n
*               uEmbDataPos  - Embedded data from this data line position will \n
*                              be stored. \n
*                              "0" means that no embedded data will be stored. \n
*                              "1" means embedded data from the first data line \n
*                              position will be stored. \n
*               uNumOfEmbDataBytes - This number of embedded data bytes will be \n
*                                    stored. \n
*                                    This value has to be a multiple of the value "4"!! \n
*  \DESCRIPTION Because a lot of embedded data could be received via SMIA the \n
*               hardware stores only a subset of this data amount. This Function \n
*               configures the SMIA hardware module to store only an embedded data \n
*               subset. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaSetEmbDataStorage(const UINT32 uEmbDataLine, const UINT32 uEmbDataPos, const UINT32 uNumOfEmbDataBytes)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if ((uNumOfEmbDataBytes & ~ MRV_SMIA_EMB_HSIZE_VALID_MASK) ||
        (uEmbDataPos        & ~ MRV_SMIA_EMB_HSTART_VALID_MASK)  )
    {
        return RET_OUTOFRANGE;
    }

    // From this embedded data line the SMIA module should store some embedded data.
    REG_SET_SLICE(ptMrvReg->smia_emb_vstart, MRV_SMIA_EMB_VSTART, uEmbDataLine);
    // Beginning at this embedded data line position the SMIA module should store some embedded data.
    REG_SET_SLICE(ptMrvReg->smia_emb_hstart, MRV_SMIA_EMB_HSTART, uEmbDataPos);
    // This number of embedded data bytes should be stored from the SMIA module.
    REG_SET_SLICE(ptMrvReg->smia_emb_hsize,  MRV_SMIA_EMB_HSIZE,  uNumOfEmbDataBytes);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP)
    UNUSED_PARAM1(uEmbDataLine);                             // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uEmbDataPos);                              // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uNumOfEmbDataBytes);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetEmbDataStorage \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data storage request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puEmbDataLine - From this SMIA data line embedded data will be stored. \n
*               puEmbDataPos  - From this SMIA data line position embedded data will be stored. \n
*               puNumOfEmbDataBytes - This number of embedded data bytes will be \n
*                                     stored. \n
*  \DESCRIPTION Determines the current SMIA module embedded data storage information. \n
*               The SMIA line, position and number of bytes of embedded data that \n
*               will be stored from the SMIA module will be returned. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaGetEmbDataStorage(UINT32 *puEmbDataLine, UINT32 *puEmbDataPos, UINT32 *puNumOfEmbDataBytes)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puEmbDataLine || !puEmbDataPos ||  !puNumOfEmbDataBytes)
    {
        return RET_NULL_POINTER;
    }

    // From this embedded data line the SMIA module should store some embedded data.
    *puEmbDataLine       = REG_GET_SLICE(ptMrvReg->smia_emb_vstart, MRV_SMIA_EMB_VSTART);
    // Beginning at this embedded data line position the SMIA module should store some embedded data.
    *puEmbDataPos        = REG_GET_SLICE(ptMrvReg->smia_emb_hstart, MRV_SMIA_EMB_HSTART);
    // This number of embedded data bytes should be stored from the SMIA module.
    *puNumOfEmbDataBytes = REG_GET_SLICE(ptMrvReg->smia_emb_hsize,  MRV_SMIA_EMB_HSIZE);

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP)
    UNUSED_PARAM(puEmbDataLine);                             // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puEmbDataPos);                              // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puNumOfEmbDataBytes);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetEmbData \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data copy. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  puDestEmbDataBuffer - Into this variable the embedded data will \n
*                                     be copied. \n
*               ulDestEmbDataBufSize - Size of the destination buffer \n
*               puNumOfEmbDataBytes - Into this variable the number of copied \n
*                                     embedded data bytes will be written. \n
*  \DESCRIPTION This function copies embedded data from the hardware embedded \n
*               data fifo memory to the destination buffer. \n
*  \NOTES       \n
*
*****************************************************************************/
RESULT MrvSmiaGetEmbData(UINT8 *puDestEmbDataBuffer, const UINT32 ulDestEmbDataBufSize, UINT32 *puNumOfEmbDataBytes)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 smia_emb_data = 0;
    UINT32 ulDestEmbDataBufSizeAligned = 0;

    if (!puDestEmbDataBuffer || !puNumOfEmbDataBytes)
    {
        return RET_NULL_POINTER;
    }

    *puNumOfEmbDataBytes = 0;
    ulDestEmbDataBufSizeAligned = ulDestEmbDataBufSize & 0xFFFFFFFC;//aligned to 32bit
    
    while ( REG_GET_SLICE(ptMrvReg->smia_status, MRV_SMIA_EMB_DATA_AVAIL) 
          & (*puNumOfEmbDataBytes < ulDestEmbDataBufSizeAligned) )
    { // There are embedded data stored in fifo. Copy 4 bytes to the target buffer
  #if 1 // This 32-bit assign requires little endian! (first byte is in fifo bits [0:7])
        UNUSED_PARAM1(smia_emb_data);
        (*(UINT32*)puDestEmbDataBuffer) = REG_READ(ptMrvReg->smia_emb_data_fifo);
        // Increment target pointer for 4 bytes
        puDestEmbDataBuffer += 4;
  #else // This loop makes the code endianess indepedant but a bit slower
        smia_emb_data = REG_READ(ptMrvReg->smia_emb_data_fifo);
        for (int i = 0; i < 4; i++)
        { // copy bytes into buffer (first byte is in mipi_add_data bits [0:7] - LSB)
            *puDestEmbDataBuffer = smia_emb_data & 0xFF; // get LSB
            puDestAddbDataBuffer++;
            smia_emb_data >>= 8; // next byte
        }
  #endif
        *puNumOfEmbDataBytes += 4;
    }

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puDestEmbDataBuffer);                       // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(ulDestEmbDataBufSize);                       // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puNumOfEmbDataBytes);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
}

/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaSetEmbDataFillLevel \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data fill level setting. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  ulFillLevel - Fill level of embedded data fifo will be set (32bit aligned). \n
*  \DESCRIPTION Sets the fill level of embedded data fifo for the fill level \n
*               interrupt generation. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaSetEmbDataFillLevel(const UINT32 ulFillLevel)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ulFillLevel & ~ MRV_SMIA_FIFO_FILL_LEVEL_VALID_MASK)
    {
        return RET_OUTOFRANGE;
    }

    // Sets the fill level of embedded data fifo.
    REG_SET_SLICE(ptMrvReg->smia_fifo_fill_level, MRV_SMIA_FIFO_FILL_LEVEL, ulFillLevel);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(ulFillLevel);                              // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP)
}

/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaGetEmbDataFillLevel \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data fill level getting. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  ulFillLevel - Fill level of embedded data fifo will be fetched. \n
*  \DESCRIPTION Gets the fill level of embedded data fifo for the fill level \n
*               interrupt generation. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaGetEmbDataFillLevel(UINT32 *pulFillLevel)
{
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!pulFillLevel)
    {
        return RET_NULL_POINTER;
    }

    // Gets the fill level of embedded data fifo.
    *pulFillLevel = REG_GET_SLICE(ptMrvReg->smia_fifo_fill_level, MRV_SMIA_FIFO_FILL_LEVEL);

    return RET_SUCCESS; 
#else // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(pulFillLevel);                              // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP)
}

