/*****************************************************************************/
/*!
 *  \file        mrv.c \n
 *  \version     2.1 \n
 *  \author      Friebe \n
 *  \brief       MARVIN main initialization, start and stop. \n
 *
 *  \revision    $Revision: 524 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-09-03 19:12:46 +0200 (Do, 03 Sep 2009) $ \n
 *               $Id: mrv.c 524 2009-09-03 17:12:46Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */  
/*****************************************************************************/

#include "os.h"
#include "stdinc.h"

#include "mrv_priv.h"
#include "reg_access.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  (DBG_ERR  | DBG_MRV)    //!< errors
#define DWARN (DBG_WARN | DBG_MRV)    //!< warnings
#define DINFO (DBG_INFO | DBG_MRV)    //!< information

// Delay after reset. The slowest module is the JPEG encoder. It needs 400 clock
// cycles to fill its tables. In worst case we have 30MHz in the FPGA. Thus 13us
// minimum delay are needed.
#define MRV_DELAY_AFTER_RESET 15


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvGetMrvId \n
 *  \RETURNVALUE MARVIN VI ID \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Returns the MARVIN hardware ID. \n
 */
/*****************************************************************************/
UINT32 MrvGetMrvId(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32                 ulResult = 0;


    ulResult = REG_GET_SLICE(ptMrvReg->vi_id, MRV_REV_ID);

    return (ulResult);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvVerifyMrvId \n
 *  \RETURNVALUE RET_SUCCESS - if the ID is as expected \n
 *               RET_FAILURE - if the ID is not as expected \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Gets the hardware ID and compares it with the expected one. \n
 */
/*****************************************************************************/
static RESULT MrvVerifyChipId(void)
{
    UINT32 ulMrvId = MrvGetMrvId();
    if (ulMrvId != MARVIN_FEATURE_CHIP_ID)
    {
        DBG_OUT((DWARN, "WARNING: Marvin HW-Id does not match! read:0x%08lX, expected:0x%08X\n", ulMrvId, MARVIN_FEATURE_CHIP_ID));
        return RET_FAILURE;
    }
    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvInit \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES       Reset of the entire MARVIN by vi_marvin_rst, register vi_ircl[7]
 *               has the same behaviour as the asynchronous hardware reset. All
 *               status and configuration register information is reset. \n
 *               Note that all MARVIN software resets (all other bits in the
 *               register isp_ctrl) are just reseting processing and status
 *               information. They are not reseting register contents to their
 *               default values.
 *               (MARVIN versions < MARVIN_3_V42 where MARVIN_FEATURE_CLOCK_DOMAINS =
 *               MARVIN_CLOCK_DOMAINS_2: Parts of MARVIN are running with the
 *               sensor clock. Thus the sensor should be initialised first.) \n
 *
 *  \DESCRIPTION Triggers an entire reset of MARVIN (equaling an asynchronous
 *               hardware reset). \n
 *               Checks the hardware ID. A debug warning is issued if the
 *               module ID does not match the expected ID. \n
 *               Enables all clocks of all sub-modules. \n
 *               MARVIN is in idle state afterwards. \n
 */
/*****************************************************************************/
void MrvInit(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // verify ID, but no consequences if it doesn't match
    (void)MrvVerifyChipId();

    // enable main clock
    REG_SET_SLICE(ptMrvReg->vi_ccl, MRV_VI_CCLFDIS, MRV_VI_CCLFDIS_ENABLE);

    // enable all clocks to make sure that all submodules will be able to perform
    // the reset correctly
    REG_SET_SLICE(ptMrvReg->vi_iccl, MRV_VI_ALL_CLK_ENABLE, ENABLE);

    // Reset of the entire MARVIN triggered by software.
    // The reset bit will be cleared by the reset itself.
    // The default value of the clock registers is all clocks on. So we don't have to
    // enable the clocks again afterwards.
    REG_SET_SLICE(ptMrvReg->vi_ircl, MRV_VI_MARVIN_RST, ON);
    OS_SleepUs(MRV_DELAY_AFTER_RESET);

#if (MARVIN_FEATURE_CLOCK_DOMAINS == MARVIN_CLOCK_DOMAINS_2)
    REG_WRITE(ptMrvReg->vi_toc, MRV_TIME_OUT_THRESH);
#endif  // #if( MARVIN_FEATURE_CLOCK_DOMAINS == MARVIN_CLOCK_DOMAINS_2 )
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvStart \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  usNumberOfFrames - number of Frames to capture (0=continuous) \n
 *               eUpdateTime \n eMrvCfgUpdateFrameSync: the changed configuration becomes
 *                                  valid at the last pixel of the just
 *                                  processed frame. Thus, the next complete
 *                                  frame will be the first to be captured. \n
 *                              eMrvCfgUpdateImmediate: MARVIN starts right away with the new
 *                                  configuration. This is useful if the image
 *                                  sensor delivers only a limited number of
 *                                  frames, and one cannot rely on the
 *                                  configuration update at the last pixel of
 *                                  the previous frame. \n
 *                              eMrvCfgUpdateLater: the changed configuration becomes
 *                                  valid when MrvUpdateConfiguration() is called. \n
 *
 *  \NOTES       If a certain number of frames to be captured has been
 *               specified, the ISP automatically disables its output formatter
 *               after the specified number of frames. A MrvStop() is not
 *               required in this case. \n
 *               If bImmediate is set to TRUE, the function call must be
 *               synchronized with the frame timing of the sensor.
 *
 *  \DESCRIPTION Programs the number of frames to capture. Clears frame end
 *               interrupt to allow waiting in MrvWaitForFrameEnd().
 *               Enables the ISP input acquisition and output formatter.
 *               If bImmediate=FALSE, the hardware assures that enabling is
 *               done frame synchronously. \n
 */
/*****************************************************************************/
void MrvStart(UINT16 usNumberOfFrames, teMrvConfUpdateTime eUpdateTime)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulIspCtrl = REG_READ(ptMrvReg->isp_ctrl);
    UINT32 ulEofIrqMask = MrvGetFrameEndIrqMask_ISP();

    ASSERT(usNumberOfFrames <= MRV_ISP_ACQ_NR_FRAMES_MAX);   // max. 10 bits allowed

    REG_SET_SLICE(ptMrvReg->isp_acq_nr_frames, MRV_ISP_ACQ_NR_FRAMES, usNumberOfFrames);
    REG_WRITE(ptMrvReg->mi_icr, ulEofIrqMask); // clear frame end interrupt

    // Enable ISP input Acquisition and output formatter.
    // Input Acquisition is always enabled synchronous to the image sensor (no
    // configuration update required).
    // As soon as the input acquisition is started bit in_enable_shd in the
    // register isp_flags_shd is set by hardware. In the following a frame
    // end recognized by the input acquisition unit leads to ris_in_frame_end=1
    // in isp_ris. However a recognized frame end and no signaled errors are
    // no guarantee for a valid configuration.
    // The output formatter is enabled frame synchronously according to the
    // internal sync signals. Bit MRV_GEN_CFG_UPD has to be set.
    // Bit isp_on_shd in isp_flags_shd is set when the output formatter is
    // started. A recognized frame end is signaled with ris_out_frame_end
    // in isp_ris.
    // The configuration of the input acquisition and the output formatter
    // has to be correct to generate proper internal sync signals and thus
    // a proper frame-synchronous update signal.
    // If the output formatter does not start check the following:
    //    sync polarities
    //    sample edge
    //    mode in register isp_ctrl
    //    sampling window of input acquisition <= picture size of image sensor
    //    output formatter window <= sampling window of input acquisition
    // If problems with the window sizes are suspected preferably add some
    // offsets and reduce the window sizes, so that the above relations are
    // true by all means.
  switch (eUpdateTime)
  {
  case eMrvCfgUpdateFrameSync:
    REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_GEN_CFG_UPD, ENABLE);
    break;
  case eMrvCfgUpdateImmediate:
    // MRV_ISP_ISP_CFG_UPD is used instead of MRV_ISP_ISP_GEN_CFG_UPD.
    // This updates the configuration right away and MARVIN is ready
    // to aquire the next incoming frame.
    REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_CFG_UPD, ENABLE);
    break;
  case eMrvCfgUpdateLater:
    // no update from within this function but enable ISP and Input
    break;
  default:
    break;
  }
  REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_INFORM_ENABLE, ENABLE);
  REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_ENABLE, ENABLE);
  REG_WRITE(ptMrvReg->isp_ctrl, ulIspCtrl);
  
  //#*#PORT debug begin
  //while(1){
  //	printf( "0x%08lx\n", *((unsigned long*)0x600805a8) );
  //}
  //#*#PORT debug end
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvStop \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  eUpdateTime \n eMrvCfgUpdateFrameSync: MARVIN is stopped at the
 *                                  last pixel of the just processed frame. \n
 *                              eMrvCfgUpdateImmediate: MARVIN is stopped right
 *                                   away. \n
 *                              eMrvCfgUpdateLater: MARVIN is stopped when
 *                                  MrvUpdateConfiguration() is called. \n
 *
 *  \NOTES       The output formatter is used to stop processing, because this
 *               unit is able to generate an ISP off interrupt based on the ISP
 *               output frame after windowing. \n
 *               The ISP off interrupt is generated after putting out the last
 *               pixel of the last frame. \n
 *               After stopping data capturing the processing pipeline is still
 *               working for a while because of the pipeline delay of some
 *               pixel lines. This delay depends on the mode of operation. The
 *               maximum delay is about 11 lines. \n
 *               It must be waited until the complete frame has been written
 *               into system memory. The latter is done by waiting for the
 *               frame end interrupt of the memory interface (see
 *               MrvWaitForFrameEnd()). \n
 *               The shadow bit of ISP_ENABLE is ISP_ON_SHD in the register \n
 *               ISP_FLAGS_SHD. A configuration update is needed to change \n
 *               the ISPs state. \n
 *
 *  \DESCRIPTION Clear frame end interrupt to allow waiting in
 *               MrvWaitForFrameEnd(). Disable output formatter (frame
 *               synchronously). Disable input formatter \n
 */
/*****************************************************************************/
void MrvStop(teMrvConfUpdateTime eUpdateTime)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulIspCtrl = REG_READ(ptMrvReg->isp_ctrl);
    UINT32 ulEofIrqMask = MrvGetFrameEndIrqMask_ISP();

    REG_WRITE(ptMrvReg->mi_icr, ulEofIrqMask);                      // clear frame end interrupt
    REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_INFORM_ENABLE, DISABLE);   // disable input formatter
    REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_ENABLE, DISABLE);          // disable output formatter

    switch (eUpdateTime)
    {
    case eMrvCfgUpdateFrameSync:
      REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_GEN_CFG_UPD, ENABLE);
      break;
    case eMrvCfgUpdateImmediate:
      REG_SET_SLICE(ulIspCtrl, MRV_ISP_ISP_CFG_UPD, ENABLE);
      break;
    case eMrvCfgUpdateLater:
      break;
    default:
      break;
    }

    REG_WRITE(ptMrvReg->isp_ctrl, ulIspCtrl);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvWaitForVSync \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait \n
 *               RET_NOTSUPP - polarity not supported \n
 *  \PARAMETERS  vertical sync polarity \n
 *
 *  \NOTES       For edge detecting this lacks some accuracy. Also, dependent
 *               on processor and image sensor speed, vertical syncs may be
 *               missed. It is just guaranteed that at least one vertical sync
 *               passes until the function returns. \n
 *
 *  \DESCRIPTION Waits until rear edge of vertical sync occurs. \n
 */
/*****************************************************************************/
RESULT MrvWaitForVSync(UINT32 ulVPol)
{
    RESULT iError;

    iError =  MrvWaitForVSyncHelper();

    UNUSED_PARAM(ulVPol);     // added to avoid LINT warnings (Info 715)
    return iError;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvWaitForVSyncHelper \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES       \n
 *
 *  \DESCRIPTION Waits until given level of vertical sync is detected. \n
 */
/*****************************************************************************/
RESULT MrvWaitForVSyncHelper(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulStartTick,ulTickFreq;
    ulTickFreq  = OS_GetFrequency();

    // clear vstart irq
    /* [Vogt] well, this looks a bit strange, i'll assume the intention was
       clearing the ISP VSTART interrupt, like written below:
    UINT32 MrvStatus;
    MrvStatus = ptMrvReg->isp_icr;
    MrvStatus = MRV_ISPINT_VSTART;
    */
    REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_V_START, 1);

    if (!REG_GET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_INFORM_ENABLE))
    {
        DBG_OUT((DERR, "MrvWaitForVSyncHelper: ISP input not enabled! (set right sync mode)\n"));
        return RET_FAILURE;
    }

    ulStartTick = OS_GetTick();
    while (!REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_V_START))
    {
        if (((OS_GetTick() - ulStartTick) / ulTickFreq) > 2)
        {
            DBG_OUT((DERR, "MrvWaitForVSyncHelper: timeout\n"));
            return RET_FAILURE;
        }
    }

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSetDataPath \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_NOTSUPP - data path setting not supported \n
 *  \PARAMETERS  MrvYcsChnMode and DpSwitch for data path \n
 *
 *  \NOTES       See specification for details. \n
 *
 *  \DESCRIPTION Changes the data path settings. \n
 */
/*****************************************************************************/
RESULT MrvSetDataPath(teMrvYcsChnMode MrvYcsChnMode, teMrvDpSwitch DpSwitch)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 vi_dpcl = REG_READ(ptMrvReg->vi_dpcl);
    UINT32 vi_chan_mode;
    UINT32 vi_mp_mux;

    // get desired setting for ycs_chan_mode (or vi_chan_mode) bits
    switch (MrvYcsChnMode)
    {
    case eMrvYcsOff:   vi_chan_mode = MRV_VI_CHAN_MODE_OFF;    break;
    case eMrvYcsY:     vi_chan_mode = MRV_VI_CHAN_MODE_Y;      break;
    case eMrvYcsMvRaw: vi_chan_mode = MRV_VI_CHAN_MODE_MP_RAW; break;
    case eMrvYcsMv:    vi_chan_mode = MRV_VI_CHAN_MODE_MP;     break;
    case eMrvYcsSp:    vi_chan_mode = MRV_VI_CHAN_MODE_SP;     break;
    case eMrvYcsMvSp:  vi_chan_mode = MRV_VI_CHAN_MODE_MP_SP;  break;
    default:
        DBG_OUT((DERR, "MrvSetDataPath: unknown value for teMrvYcsChnMode\n"));
        return RET_NOTSUPP;
    }

    if (vi_chan_mode & ~ (MRV_VI_CHAN_MODE_MASK >> MRV_VI_CHAN_MODE_SHIFT))
    {
        DBG_OUT((DERR, "MrvSetDataPath: teMrvYcsChnMode value not supported by this marvin derivative\n"));
        return RET_NOTSUPP;
    }

    // get desired setting for vi_dp_switch (or vi_dp_mux) bits
    switch (DpSwitch)
    {
    case eMrvDpRaw:  vi_mp_mux = MRV_VI_MP_MUX_RAW;  break;
    case eMrvDpJpeg: vi_mp_mux = MRV_VI_MP_MUX_JPEG; break;
    case eMrvDpMv:   vi_mp_mux = MRV_VI_MP_MUX_MP;   break;
    default:
        DBG_OUT((DERR, "MrvSetDataPath: unknown value for teDpSwitch\n"));
        return RET_NOTSUPP;
    }

    if (vi_mp_mux & ~ MRV_VI_MP_MUX_MASK)
    {
        DBG_OUT((DERR, "MrvSetDataPath: teDpSwitch value not supported by this marvin derivative\n"));
        return RET_NOTSUPP;
    }

    // program settings into MARVIN vi_dpcl register
    REG_SET_SLICE(vi_dpcl, MRV_VI_CHAN_MODE, vi_chan_mode);
    REG_SET_SLICE(vi_dpcl, MRV_VI_MP_MUX, vi_mp_mux);
    REG_WRITE(ptMrvReg->vi_dpcl, vi_dpcl);

    return RET_SUCCESS;
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSetMipiSmia \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_NOTSUPP - data path setting not supported \n
 *  \PARAMETERS  MrvYcsChnMode and DpSwitch for data path \n
 *
 *  \NOTES       See specification for details. \n
 *
 *  \DESCRIPTION Changes the data path settings to SMIA or MIPI. \n
 */
/*****************************************************************************/
RESULT MrvSetMipiSmia(UINT32 ulMode)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 if_select;

    // get desired setting for if_select bits
    switch (ulMode)
    {
    case ISI_MODE_SMIA:      if_select = MRV_IF_SELECT_SMIA; break;
    case ISI_MODE_MIPI:      if_select = MRV_IF_SELECT_MIPI; break;
    case ISI_MODE_BAYER:     // no break
    case ISI_MODE_BT601:     // no break
    case ISI_MODE_BT656:     // no break
    case ISI_MODE_PICT:      // no break
    case ISI_MODE_DATA:      // no break
    case ISI_MODE_BAY_BT656: // no break
    case ISI_MODE_RAW_BT656: if_select = MRV_IF_SELECT_PAR; break;
    default:
        DBG_OUT( (DERR, "MrvSetMipiSmia: unknown value for ulMode\n") );
        return RET_NOTSUPP;
    }

    // program settings into MARVIN vi_dpcl register
    REG_SET_SLICE(ptMrvReg->vi_dpcl, MRV_IF_SELECT, if_select);

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvWaitForMi \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait \n
 *  \PARAMETERS  Mask for MI_RIS register to specify the bits to wait for \n
 *
 *  \DESCRIPTION Waits until the specified bits becomes signaled in the mi_ris
 *               register. \n
 */
/*****************************************************************************/
static RESULT MrvWaitForMi(UINT32 ulBitMask)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

	UINT32 ulStartTick = OS_GetTick();
	UINT32 ulTickFreq  = OS_GetFrequency();

    // Wait for the current BitMask. If the BitMask is zero, then it's no waiting.
    while ((ptMrvReg->mi_ris & ulBitMask) != ulBitMask)
    {
/*
        // debugging MARVIN status
        DBG_OUT( (DINFO, "MrvWaitForMi: Register ISP_FLAGS_SHD  = 0x%08X\n", ptMrvReg->isp_flags_shd) );
*/

        //timeout 12 seconds, works also if OS_GetTick overflows (don't change types, first subtraction)
        if( ((OS_GetTick() - ulStartTick) / ulTickFreq) > 12)
        {
            //Note: Don't use REG_READ because content of registers would be already printed here.

            DBG_OUT((DERR, "MrvWaitForMi: TIMEOUT\n"));

            // some more info about MARVINs state
            DBG_OUT((DINFO, "  ISP_RIS          = 0x%08lX%s\n", ptMrvReg->isp_ris,
                     (REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_DATA_LOSS) ? " - MRV_ISPINT_DATA_LOSS" : "")));
#if (MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)
            DBG_OUT((DINFO, "  ISP_ERR          = 0x%08lX\n", ptMrvReg->isp_err));
#endif  // #if (MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)

            DBG_OUT((DINFO, "\n"));
            DBG_OUT((DINFO, "  MI_RIS           = 0x%08lX\n", ptMrvReg->mi_ris));
#if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST )
            DBG_OUT((DINFO, "  MI_STATUS        = 0x%08lX\n", ptMrvReg->mi_status));
#endif  // #if( MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3 )
            DBG_OUT((DINFO, "  MI_DMA_STATUS    = 0x%08lX\n", ptMrvReg->mi_dma_status));
#endif  // #if( MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V3)

            DBG_OUT((DINFO, "\n"));
            DBG_OUT((DINFO, "  JPE_ENCODER_BUSY = 0x%08lX\n", ptMrvReg->jpe_encoder_busy));
            DBG_OUT((DINFO, "  JPE_DEBUG        = 0x%08lX\n", ptMrvReg->jpe_debug));
            DBG_OUT((DINFO, "  JPE_ERROR_RIS    = 0x%08lX\n", ptMrvReg->jpe_error_ris));
            DBG_OUT((DINFO, "  JPE_STATUS_RIS   = 0x%08lX\n", ptMrvReg->jpe_status_ris));

#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP)      || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
            DBG_OUT((DINFO, "\n"));
            DBG_OUT((DINFO, "  SMIA_STATUS      = 0x%08lX\n", ptMrvReg->smia_status));
            DBG_OUT((DINFO, "  SMIA_RIS         = 0x%08lX\n", ptMrvReg->smia_ris));
#endif  // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)

#if( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4) )
            DBG_OUT((DINFO, "\n"));
            DBG_OUT((DINFO, "  MIPI_STATUS      = 0x%08lX\n", ptMrvReg->mipi_status));
            DBG_OUT((DINFO, "  MIPI_RIS         = 0x%08lX\n", ptMrvReg->mipi_ris));
#endif  // #if( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4) )

            DBG_OUT((DINFO, "\n"));
            DBG_OUT((DINFO, "  VI_DPCL          = 0x%08lX\n", ptMrvReg->vi_dpcl));
            DBG_OUT((DINFO, "  ISP_CTRL         = 0x%08lX\n", ptMrvReg->isp_ctrl));
            DBG_OUT((DINFO, "  ISP_ACQ_PROP     = 0x%08lX\n", ptMrvReg->isp_acq_prop));
            DBG_OUT((DINFO, "  MI_CTRL          = 0x%08lX\n", ptMrvReg->mi_ctrl));
            DBG_OUT((DINFO, "  MI_CTRL_SHD      = 0x%08lX\n", ptMrvReg->mi_ctrl_shd));

            DBG_OUT((DINFO, "  MI_BYTE_CNT      = 0x%08lX\n", ptMrvReg->mi_byte_cnt));

            DBG_OUT((DINFO, "\nMrvWaitForMi: TIMEOUT end\n\n"));

/*
            // debugging MARVIN status
            MrvReg_DumpDebugInfoNow(stdout);
*/

            // Try to recover. Softreset of submodules (but not entire marvin) resets
            // processing and status information, but not configuration register content.
            // Bits are sticky. So we have to clear them. Reset affects the MARVIN 1..2
            // clock cycles after the bits are set to high. So we don't have to wait in
            // software before clearing them.
            // Note that only modules with clock enabled will be affected.
            REG_SET_SLICE(ptMrvReg->vi_ircl, MRV_VI_ALL_SOFT_RST, ON);
            REG_SET_SLICE(ptMrvReg->vi_ircl, MRV_VI_ALL_SOFT_RST, OFF);
            OS_SleepUs(MRV_DELAY_AFTER_RESET);
            // isp config update, neccessary to update v/hsize into shadow registers
            REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_CFG_UPD, ON);

#ifdef TODO
			MrvReg_DumpAll( stdout ); //#*#PORT debug
#endif //TODO

            return RET_FAILURE;
        } // if timeout
    } // while ((ptMrvReg->mi_ris & ulBitMask) != ulBitMask)

#if (MARVIN_FEATURE_ISP_ENABLE == MARVIN_FEATURE_EXIST)
    // set isp_enable to zero if isp status is off
    if(!REG_GET_SLICE(ptMrvReg->isp_flags_shd, MRV_ISP_ISP_ON_SHD ))
    {
    	REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_ENABLE, OFF)
    }
#endif

    if (REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_DATA_LOSS))
    {
        DBG_OUT((DWARN, "WARN: *** no failure, but MRV_ISPINT_DATA_LOSS *** \n"));
    }

	//MrvReg_DumpAll( stdout ); //#*#PORT debug

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvWaitForFrameEnd \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait for frame end \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES        \n
 *
 *  \DESCRIPTION Waits until a frame is written to memory (frame end
 *               interrupt occurs). \n
 *               Waits for the frame end interrupt of the memory interface. \n
 */
/*****************************************************************************/
RESULT MrvWaitForFrameEnd(void)
{
    return MrvWaitForMi(MrvGetFrameEndIrqMask_ISP());
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvWaitForMacroBlock \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait for frame end \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES       \n
 *
 *  \DESCRIPTION Waits until a macro block is written to memory (macro block line
 *               interrupt occurs). \n
 *               Waits for the frame end interrupt of the memory interface. \n
 */
/*****************************************************************************/
RESULT MrvWaitForMacroBlock(void)
{
    return MrvWaitForMi(MRV_MI_MBLK_LINE_MASK);
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvWaitForDmaReady \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait for frame end \n
 *               RET_NOTSUPP - feature is not supported by current marvin derivative \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES        \n
 *
 *  \DESCRIPTION Waits until a frame has been completely read by the DMA-read
 *               block of the memory interface. (dma ready interrupt occurs). \n
 *               Note that that does not necessarily mean that the data has
 *               already been processed. \n
 */
/*****************************************************************************/
RESULT MrvWaitForDmaReady(void)
{
#ifdef MRV_MI_DMA_READY
    return MrvWaitForMi(MRV_MI_DMA_READY_MASK);
#else
    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvWaitForDmaFrameEnd \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_FAILURE - timeout during wait for DMA frame end \n
 *               RET_NOTSUPP - if the used memory interface version does not
 *                             support this feature \n
 *  \PARAMETERS  none \n
 *
 *  \NOTES       \n
 *
 *  \DESCRIPTION Waits until a frame of self path is written to memory (DMA
 *               frame end interrupt occurs). \n
 *               Waits for the self path frame end interrupt of the memory
 *               interface. \n
 */
/*****************************************************************************/
RESULT MrvWaitForDmaFrameEnd(void)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    return MrvWaitForMi(MrvGetFrameEndIrqMask_DMA());
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    ASSERT(0);
 //   return RET_NOTSUPP;
#endif // MARVIN_FEATURE_MI
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvGetFrameEndIrqMask_ISP \n
 *  \RETURNVALUE mask for frame end interrupt(s) to check for frame ends of ISP originated data \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Returns the mask for the frame end interrupts, which are used for Isp. \n
 */
/*****************************************************************************/
UINT32 MrvGetFrameEndIrqMask_ISP(void)
{
#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    switch (REG_GET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_DMA_SWITCH))
    {
    case MRV_VI_DMA_SWITCH_IE:     //2: path to image effects block (i.e. replacement for data coming from the ISP)
        return 0; //datapath is used by DMA

    case MRV_VI_DMA_SWITCH_SELF:   //0: direct path to self path mux
    case MRV_VI_DMA_SWITCH_SI:     //1: path to superimpose block
    case MRV_VI_DMA_SWITCH_JPG:    //3: direct path to JPEG encoder (R2B-buffer-less encodein mode)
    default:
        {   //main and/or self path depends on the YC-splitter setting
            switch (REG_GET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_CHAN_MODE))
            {
            case MRV_VI_CHAN_MODE_MP:    return MRV_MI_MP_FRAME_END_MASK;
 #if (MARVIN_FEATURE_DPMUX_YCSPLIT != MARVIN_FEATURE_YCS_V1)
            case MRV_VI_CHAN_MODE_SP:    return MRV_MI_SP_FRAME_END_MASK;
 #endif //(MARVIN_FEATURE_DPMUX_YCSPLIT != MARVIN_FEATURE_YCS_V1)
            case MRV_VI_CHAN_MODE_MP_SP: return MRV_MI_MP_FRAME_END_MASK | MRV_MI_SP_FRAME_END_MASK;
            default:                     return 0;
            }
        }
    }
#else
    return (MRV_MI_MP_FRAME_END_MASK);
#endif // MARVIN_FEATURE_MI
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvGetFrameEndIrqMask_DMA \n
 *  \RETURNVALUE mask for frame end interrupt(s) to check for DMA-read feature \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Returns the mask for the frame end interrupts, which are used for DMA. \n
 */
/*****************************************************************************/
UINT32 MrvGetFrameEndIrqMask_DMA(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

#if (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3)
    switch (REG_GET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_DMA_SWITCH))
    {
    case MRV_VI_DMA_SWITCH_SELF:    //0: direct path to self path mux
        return MRV_MI_SP_FRAME_END_MASK; //self path

    case MRV_VI_DMA_SWITCH_SI:    //1: path to superimpose block
        return 0; // no DMA read possible while superimpose is active

    case MRV_VI_DMA_SWITCH_IE:    //2: path to image effects block (i.e. replacement for data coming from the ISP)
    {   //main or self path depends on the YC-splitter setting
        switch (REG_GET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_CHAN_MODE))
        {
        case MRV_VI_CHAN_MODE_MP:    return MRV_MI_MP_FRAME_END_MASK;
 #if (MARVIN_FEATURE_DPMUX_YCSPLIT != MARVIN_FEATURE_YCS_V1)
        case MRV_VI_CHAN_MODE_SP:    return MRV_MI_SP_FRAME_END_MASK;
 #endif //(MARVIN_FEATURE_DPMUX_YCSPLIT != MARVIN_FEATURE_YCS_V1)
        case MRV_VI_CHAN_MODE_MP_SP: return MRV_MI_MP_FRAME_END_MASK | MRV_MI_SP_FRAME_END_MASK;
        default:                     return 0;
        }
    }

    case MRV_VI_DMA_SWITCH_JPG:    //3: direct path to JPEG encoder (R2B-buffer-less encodein mode)
        return MRV_MI_MP_FRAME_END_MASK; // main path

    default:
        return 0;
    }
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
  #if ((MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_EXIST_NOT)   || \
       (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V1) || \
       (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V2))
    // no DMA-read via IE-Path possible, so DMA data will always travel though the
    // self path and therefore always trigger the second frame end interrupt
    UNUSED_PARAM(ptMrvReg);
    return (MRV_MI_DMA_FRAME_END_MASK);
  #elif((MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V3) || \
        (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V4))
    // DMA-read via IE-Path may be selected. in this case, it depends on the
    // YC-splitter setting if they travel though the main- or self path or both.
    if (REG_GET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_DMA_SWITCH) == MRV_VI_DMA_SWITCH_IE)
    {   //main or self path depends on the YC-splitter setting
        switch (REG_GET_SLICE(ptMrvReg->vi_dpcl, MRV_VI_CHAN_MODE))
        {
        case MRV_VI_CHAN_MODE_MP:    return MRV_MI_MP_FRAME_END_MASK;
        case MRV_VI_CHAN_MODE_SP:    return MRV_MI_DMA_FRAME_END_MASK;
        case MRV_VI_CHAN_MODE_MP_SP: return MRV_MI_MP_FRAME_END_MASK | MRV_MI_DMA_FRAME_END_MASK;
        default:                     return 0;
        }
    }
    else
    {
        return MRV_MI_DMA_FRAME_END_MASK;
    }
  #else
    #error "unknown setting for MARVIN_FEATURE_DMA_READ"
  #endif
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
    UNUSED_PARAM(ptMrvReg);
    return 0;
#endif // MARVIN_FEATURE_MI
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvResetInterruptStatus \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Writes '0xFFFFFFFF' into all *_icr registers to clear all
 *               interrupts. \n
 */
/*****************************************************************************/
void MrvResetInterruptStatus(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_ALL, ON);         // ISP interrupt clear register
    REG_SET_SLICE(ptMrvReg->isp_err_clr, MRV_ISP_ALL_ERR, ON);     // ISP error interrupt clear register
    REG_SET_SLICE(ptMrvReg->mi_icr, MRV_MI_ALLIRQS, ON);           // MI interrupt clear register
#if( MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
    REG_SET_SLICE(ptMrvReg->emp_out_icr, MRV_MI_MP_FRAME_END, ON);           // EMP interrupt clear register
#endif
    REG_SET_SLICE(ptMrvReg->jpe_error_icr, MRV_JPE_ALL_ERR, ON);   // JPEG error interrupt clear register
    REG_SET_SLICE(ptMrvReg->jpe_status_icr, MRV_JPE_ALL_STAT, ON); // JPEG status interrupt clear register
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSet601Mode \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Changes the ISP control register to ITU 601 mode. \n
 */
/*****************************************************************************/
void MrvSet601Mode(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_ctrl = REG_READ(ptMrvReg->isp_ctrl);

    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_601);
    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_INFORM_ENABLE, ENABLE);
    REG_WRITE(ptMrvReg->isp_ctrl, isp_ctrl);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSet656Mode \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Changes the ISP control register to ITU 656 mode. \n
 */
/*****************************************************************************/
void MrvSet656Mode(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_ctrl = REG_READ(ptMrvReg->isp_ctrl);

    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_656);
    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_INFORM_ENABLE, ENABLE);
    REG_WRITE(ptMrvReg->isp_ctrl, isp_ctrl);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSetDmaReadMode \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  eMode        - desired operating mode of the DMA-read block \n
 *               eUpdateTime \n eMrvCfgUpdateImmediate: immediate update \n
 *                              else: update after next cfg_upd of selected input \n
 *  \DESCRIPTION Selects DMA read mode (i.e. sink of the data read from  system
 *               memory by the DMA-read block). \n
 *  \NOTE:       eUpdateTime is only used on Marvin3plus,
 *               on all other Marvin derivates immediate update is made \n
 */
/*****************************************************************************/
void MrvSetDmaReadMode(teMrvDmaReadMode eMode, teMrvConfUpdateTime eUpdateTime)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 vi_dma_switch  = 0;        // added to avoid LINT warnings (Info 530)
    UINT32 vi_dma_spmux   = 0;        // added to avoid LINT warnings (Info 530)
    UINT32 vi_dma_iemux   = 0;        // added to avoid LINT warnings (Info 530)
    BOOL   bDmaJpegSelect = FALSE;    // added to avoid LINT warnings (Info 530)


// error checks and MARVIN derivative dependant stuff
#if (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_EXIST_NOT)
    // DMA-read not supported at all
    #define DMA_READ_MODE_PROGRAMMING__VI_SPMCL 0
    #define DMA_READ_MODE_PROGRAMMING__VI_DPCL  0
    ASSERT(eMode == eMrvDmaRd_off);
#elif (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V1)
    // DMA-read feature directly connected to the self path only.
    // Programming is done via the vi_spmcl register
    #define DMA_READ_MODE_PROGRAMMING__VI_SPMCL 1
    #define DMA_READ_MODE_PROGRAMMING__VI_DPCL  0
    ASSERT((eMode == eMrvDmaRd_off) ||
           (eMode == eMrvDmaRd_SelfPath));
#elif (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V2)
    // DMA-read feature connected through a dedicated DMA-read multiplexer.
    // Programming is done via both vi_spmcl and vi_dpcl register
    #define DMA_READ_MODE_PROGRAMMING__VI_SPMCL 1
    #define DMA_READ_MODE_PROGRAMMING__VI_DPCL  1
    ASSERT((eMode == eMrvDmaRd_off)      ||
           (eMode == eMrvDmaRd_SelfPath) ||
           (eMode == eMrvDmaRd_Superimpose));
#elif (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V3)
    // DMA-read feature connected through a dedicated DMA-read multiplexer.
    // Programming is done via vi_dpcl register only
    #define DMA_READ_MODE_PROGRAMMING__VI_SPMCL 0
    #define DMA_READ_MODE_PROGRAMMING__VI_DPCL  1
    ASSERT((eMode == eMrvDmaRd_off)      ||
           (eMode == eMrvDmaRd_SelfPath) ||
           (eMode == eMrvDmaRd_IePath)   ||
           (eMode == eMrvDmaRd_Superimpose));
#elif (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V4)
    // DMA-read feature connected through a dedicated DMA-read multiplexer.
    // Programming is done via vi_dpcl register only
    #define DMA_READ_MODE_PROGRAMMING__VI_SPMCL 0
    #define DMA_READ_MODE_PROGRAMMING__VI_DPCL  1
    ASSERT((eMode == eMrvDmaRd_off)      ||
           (eMode == eMrvDmaRd_SelfPath) ||
           (eMode == eMrvDmaRd_IePath)   ||
           (eMode == eMrvDmaRd_JpgEnc)   ||
           (eMode == eMrvDmaRd_Superimpose));
#else
    #error "unknown setting for MARVIN_FEATURE_DMA_READ"
#endif

    switch (eMode)
    {
    case eMrvDmaRd_off:
        vi_dma_switch  = MRV_VI_DMA_SWITCH_SELF;
        vi_dma_spmux   = MRV_VI_DMA_SPMUX_CAM;
        vi_dma_iemux   = MRV_VI_DMA_IEMUX_CAM;
        bDmaJpegSelect = FALSE;
        break;
    case eMrvDmaRd_SelfPath:
        vi_dma_switch  = MRV_VI_DMA_SWITCH_SELF;
        vi_dma_spmux   = MRV_VI_DMA_SPMUX_DMA;
        vi_dma_iemux   = MRV_VI_DMA_IEMUX_CAM;
        bDmaJpegSelect = FALSE;
        break;
    case eMrvDmaRd_IePath:
        vi_dma_switch  = MRV_VI_DMA_SWITCH_IE;
        vi_dma_spmux   = MRV_VI_DMA_SPMUX_CAM;
        vi_dma_iemux   = MRV_VI_DMA_IEMUX_DMA;
        bDmaJpegSelect = FALSE;
        break;
    case eMrvDmaRd_JpgEnc:
        vi_dma_switch  = MRV_VI_DMA_SWITCH_JPG;
        vi_dma_spmux   = MRV_VI_DMA_SPMUX_CAM;
        vi_dma_iemux   = MRV_VI_DMA_IEMUX_CAM;
        bDmaJpegSelect = TRUE;
        break;
    case eMrvDmaRd_Superimpose:
        vi_dma_switch  = MRV_VI_DMA_SWITCH_SI;
        vi_dma_spmux   = MRV_VI_DMA_SPMUX_CAM;
        vi_dma_iemux   = MRV_VI_DMA_IEMUX_CAM;
        bDmaJpegSelect = FALSE;
        break;
    default:
        ASSERT(0);                         // unknown DMA-read mode
        //return;
    }

#if (DMA_READ_MODE_PROGRAMMING__VI_SPMCL)
    // updating vi_spmcl
    if (vi_dma_spmux == MRV_VI_DMA_SPMUX_DMA)
    { // DMA read access for self path
        ptMrvReg->vi_spmcl |= MRV_SP_DMA_SEL;
    }
    else
    { // input data for self path from ISP
        ptMrvReg->vi_spmcl &= ~MRV_SP_DMA_SEL;
    }

  switch (eUpdateTime)
  {
  case eMrvCfgUpdateFrameSync:
    // no update from within this function
    break;
  case eMrvCfgUpdateImmediate:
        //immediate update of shadow registers
        ptMrvReg->vi_spmcl |= MRV_SPMCL_SOFT_UPD;
    break;
  case eMrvCfgUpdateLater:
    // no update from within this function
    break;
  default:
    break;
  }
#else // #if (DMA_READ_MODE_PROGRAMMING__VI_SPMCL)
    UNUSED_PARAM(ptMrvReg);       // added to avoid LINT warnings (Warning 550)
    UNUSED_PARAM(vi_dma_spmux);   // added to avoid LINT warnings (Warning 550)
    UNUSED_PARAM(eUpdateTime);    // added to avoid LINT warnings (Warning 550)
#endif // #if (DMA_READ_MODE_PROGRAMMING__VI_SPMCL)

#if (DMA_READ_MODE_PROGRAMMING__VI_DPCL)
    // updating vi_dpcl
    {
        UINT32 vi_dpcl = REG_READ(ptMrvReg->vi_dpcl);

        REG_SET_SLICE(vi_dpcl, MRV_VI_DMA_SWITCH, vi_dma_switch);
        REG_SET_SLICE(vi_dpcl, MRV_VI_DMA_SPMUX, vi_dma_spmux);
        REG_SET_SLICE(vi_dpcl, MRV_VI_DMA_IEMUX, vi_dma_iemux);
  #if ((MRV_VI_MP_MUX_JPGDIRECT & ~(MRV_VI_MP_MUX_MASK >> MRV_VI_MP_MUX_SHIFT)) == 0)
        if (bDmaJpegSelect)
        { // change the main path mode only in case direct JPEG DMA is selected
            REG_SET_SLICE(vi_dpcl, MRV_VI_MP_MUX, MRV_VI_MP_MUX_JPGDIRECT);
        }
  #else // #if ((MRV_DP_JPGDIRECT & ~MRV_DP__MASK) == 0)
        // direct DMA to JPEG not supported
        UNUSED_PARAM(bDmaJpegSelect); // added to avoid LINT warnings (Warning 550)
  #endif // #if ((MRV_DP_JPGDIRECT & ~MRV_DP__MASK) == 0)
        REG_WRITE(ptMrvReg->vi_dpcl, vi_dpcl);
    }
#else // #if (DMA_READ_MODE_PROGRAMMING__VI_DPCL)
    UNUSED_PARAM(ptMrvReg);       // added to avoid LINT warnings (Warning 550)
    UNUSED_PARAM(vi_dma_switch);  // added to avoid LINT warnings (Warning 550)
    UNUSED_PARAM(vi_dma_spmux);   // added to avoid LINT warnings (Warning 550)
    UNUSED_PARAM(vi_dma_iemux);   // added to avoid LINT warnings (Warning 550)
    UNUSED_PARAM(bDmaJpegSelect); // added to avoid LINT warnings (Warning 550)
#endif // #if (DMA_READ_MODE_PROGRAMMING__VI_DPCL)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSelfDmaSelect \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  bDma        \n 1: DMA read \n
 *                              0: ISP access \n
 *               eUpdateTime \n eMrvCfgUpdateImmediate: immediate update \n
 *                              else: update after next cfg_upd of selected input \n
 *
 *  \DESCRIPTION Selects DMA read from system of self path. \n
 *  \NOTE:       eUpdateTime is only used on Marvin3plus,
 *               on all other Marvin derivates immediate update is made \n
 */
/*****************************************************************************/
void MrvSelfDmaSelect(BOOL bDma, teMrvConfUpdateTime eUpdateTime)
{
    MrvSetDmaReadMode((bDma ? eMrvDmaRd_SelfPath : eMrvDmaRd_off), eUpdateTime);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvUpdateConfiguration \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Sets MRV_GEN_CFG_UPD in isp_ctrl to initiate configuration
 *               update on next frame end. (like on eMrvCfgUpdateFrameSync) \n
 */
/*****************************************************************************/
void MrvUpdateConfiguration(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulCtrl=0;
    ulCtrl = REG_READ(ptMrvReg->isp_ctrl);
    //frame synchronous update of shadow registers
    REG_SET_SLICE(ulCtrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
    REG_WRITE(ptMrvReg->isp_ctrl, ulCtrl);
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSetExtYCMode \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Set extended mode with unrestricted values for YCbCr
 *               Y (0-255) CbCr (0-255)
 */
/*****************************************************************************/
void MrvSetExtYCMode(void)
{
#if (MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST )
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_ctrl = REG_READ(ptMrvReg->isp_ctrl);

    //modify isp_ctrl register
    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_CSM_C_RANGE, MRV_ISP_ISP_CSM_C_RANGE_FULL);
    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_CSM_Y_RANGE, MRV_ISP_ISP_CSM_Y_RANGE_FULL);
    REG_WRITE(ptMrvReg->isp_ctrl, isp_ctrl);

    // program RGB to YUV color conversion with extended range
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_0, MRV_ISP_CC_COEFF_0, 0x0026);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_1, MRV_ISP_CC_COEFF_1, 0x004B);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_2, MRV_ISP_CC_COEFF_2, 0x000F);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_3, MRV_ISP_CC_COEFF_3, 0x01EA);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_4, MRV_ISP_CC_COEFF_4, 0x01D6);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_5, MRV_ISP_CC_COEFF_5, 0x0040);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_6, MRV_ISP_CC_COEFF_6, 0x0040);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_7, MRV_ISP_CC_COEFF_7, 0x01CA);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_8, MRV_ISP_CC_COEFF_8, 0x01F6);
#endif //#if (MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST )
}

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvSetYCMode \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Set YCbCr BT601 mode
 *              
 */
/*****************************************************************************/
void MrvSetYCMode(void)
{
#if (MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST )
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_ctrl = REG_READ(ptMrvReg->isp_ctrl);

    //modify isp_ctrl register
    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_CSM_C_RANGE, MRV_ISP_ISP_CSM_Y_RANGE_BT601);
    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_CSM_Y_RANGE, MRV_ISP_ISP_CSM_Y_RANGE_BT601);
    REG_WRITE(ptMrvReg->isp_ctrl, isp_ctrl);

    // program RGB to YUV color conversion with extended range
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_0, MRV_ISP_CC_COEFF_0, 0x0021);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_1, MRV_ISP_CC_COEFF_1, 0x0040);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_2, MRV_ISP_CC_COEFF_2, 0x000D);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_3, MRV_ISP_CC_COEFF_3, 0x01ED);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_4, MRV_ISP_CC_COEFF_4, 0x01DB);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_5, MRV_ISP_CC_COEFF_5, 0x0038);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_6, MRV_ISP_CC_COEFF_6, 0x0038);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_7, MRV_ISP_CC_COEFF_7, 0x01D1);
    REG_SET_SLICE(ptMrvReg->isp_cc_coeff_8, MRV_ISP_CC_COEFF_8, 0x01F7);
#endif //#if (MARVIN_FEATURE_EXT_YCBCR_RANGE == MARVIN_FEATURE_EXIST )
}
