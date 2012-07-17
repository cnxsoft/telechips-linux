/*****************************************************************************/
/*!
 *  \file        mrv_mipi.c \n
 *  \version     1.0 \n
 *  \author      Zupp \n
 *  \brief       Marvin MIPI low level driver functionality. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_mipi.c 432 2009-06-30 09:48:59Z neugebaa $ \n
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

#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    #include "reg_access.h"
    #include "mrv_priv.h"
#endif


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MIPI )    //!< errors
#define DWARN ( DBG_WARN | DBG_MIPI )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MIPI )    //!< information


#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    #define MAX_NUMBER_OF_MIPI_LANES     4
#endif


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiInitializeModule \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful register initialization. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Initializes MIPI filter registers with default reset values. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiInitializeModule(void)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mipi_ctrl = 0; // current register value uninterested

    ptMrvReg->vi_ircl             = 0x00000800; //mipi reset
    ptMrvReg->vi_ircl             = 0x00000000; //mipi reset

    REG_SET_SLICE(mipi_ctrl, MRV_MIPI_ERR_SOT_SYNC_HS_SKIP,  ON);
    REG_SET_SLICE(mipi_ctrl, MRV_MIPI_ERR_SOT_HS_SKIP,      OFF);
    REG_SET_SLICE(mipi_ctrl, MRV_MIPI_NUM_LANES,              3);
    REG_SET_SLICE(mipi_ctrl, MRV_MIPI_SHUTDOWN_LANE,          0);
    REG_SET_SLICE(mipi_ctrl, MRV_MIPI_FLUSH_FIFO,           OFF); // [hf]: should this be set to 1 to clear fifo ?
    REG_SET_SLICE(mipi_ctrl, MRV_MIPI_OUTPUT_ENA,       DISABLE);
    REG_WRITE(ptMrvReg->mipi_ctrl, mipi_ctrl);

    REG_SET_SLICE(ptMrvReg->mipi_imsc,           MRV_MIPI_IMSC_ALL_IRQS,    OFF); // mask out all IRQs
    REG_SET_SLICE(ptMrvReg->mipi_icr,            MRV_MIPI_ICR_ALL_IRQS,      ON); // reset all IRQs

    REG_SET_SLICE(ptMrvReg->mipi_img_data_sel,   MRV_MIPI_VIRTUAL_CHANNEL,    0); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_img_data_sel,   MRV_MIPI_DATA_TYPE,          0); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_1, MRV_MIPI_ADD_DATA_VC_1,      3); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_1, MRV_MIPI_ADD_DATA_TYPE_1, 0x3F); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_2, MRV_MIPI_ADD_DATA_VC_2,      3); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_2, MRV_MIPI_ADD_DATA_TYPE_2, 0x3F); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_3, MRV_MIPI_ADD_DATA_VC_3,      3); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_3, MRV_MIPI_ADD_DATA_TYPE_3, 0x3F); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_4, MRV_MIPI_ADD_DATA_VC_4,      3); // [hf]: required / expedient ???
    REG_SET_SLICE(ptMrvReg->mipi_add_data_sel_4, MRV_MIPI_ADD_DATA_TYPE_4, 0x3F); // [hf]: required / expedient ???
    
    REG_SET_SLICE(ptMrvReg->mipi_add_data_fill_level, MRV_MIPI_ADD_DATA_FILL_LEVEL, 0); // [hf]: expedient value ???
    
    return RET_SUCCESS; 
#else

    return RET_NOTSUPP;
#endif
} // MrvMipiInitializeModule()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetControlRegisterVal \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful MIPI control register request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puMipiControlRegisterVal - Current MIPI control register value \n
 *                                          will be written into this variable.\n
 *  \DESCRIPTION Function returns the current MIPI control register value. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiGetControlRegisterVal(UINT32 *puMipiControlRegisterVal)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

   if ((!puMipiControlRegisterVal))
   {
      return RET_NULL_POINTER;
   }

   *puMipiControlRegisterVal = REG_READ(ptMrvReg->mipi_ctrl);

   return RET_SUCCESS; 
#else
   UNUSED_PARAM(puMipiControlRegisterVal);         // changed to avoid LINT warnings (Info 715)
   return RET_NOTSUPP;
#endif
} // MrvMipiGetControlRegisterVal()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiSetControlRegisterVal \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful MIPI control register modification. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  uMipiControlRegisterVal = New control register value. \n
 *  \DESCRIPTION Function sets a new control register value. \n
 *  \NOTES       With this function the complete control register will be \n
 *               overwritten with the given value. Please use one of the following  \n
 *               functions to modify  j u s t  one control register element value. \n
 *               The public mipi header contains several defines to set the \n
 *               function parameter value. \n
 *
 *****************************************************************************/
RESULT MrvMipiSetControlRegisterVal(const UINT32 uMipiControlRegisterVal)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
   volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

   REG_WRITE(ptMrvReg->mipi_ctrl, uMipiControlRegisterVal);

   return RET_SUCCESS; 
#else
   UNUSED_PARAM1(uMipiControlRegisterVal);         // changed to avoid LINT warnings (Info 715)
   return RET_NOTSUPP;
#endif
} // MrvMipiSetControlRegisterVal()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiActivateModule \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful module activation/deactivation. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  bModuleActive = TRUE  : MIPI module is activated. \n
 *               bModuleActive = FALSE : MIPI module is deactivated. \n
 *  \DESCRIPTION Activate/deactivates MIPI module actions. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiActivateModule(const BOOL bModuleActive)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 vi_dpcl = REG_READ(ptMrvReg->vi_dpcl);

    //*(UINT32*)(MEM_MRV_REG_BASE + 0x18) = 0x00000205; //Mode MIPI
    REG_SET_SLICE(vi_dpcl, MRV_IF_SELECT,    MRV_IF_SELECT_MIPI);
    REG_SET_SLICE(vi_dpcl, MRV_VI_CHAN_MODE, MRV_VI_CHAN_MODE_MP_RAW); // [hf]: ???
    //REG_SET_SLICE(vi_dpcl, MRV_VI_MP_MUX,    MRV_VI_MP_MUX_MP);        // [hf]: ???
    REG_WRITE(ptMrvReg->vi_dpcl, vi_dpcl);

    REG_SET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_OUTPUT_ENA, (bModuleActive) ? ENABLE : DISABLE);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(bModuleActive);                   // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiActivateModule()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiFreeEmbDataFifo \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful embedded data fifo reset. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Function resets the MIPI additional data fifo memory. This means \n
 *               that the additional data fifo read and write pointer will be set \n
 *               to the start position. Not read additional data will be discarded \n
 *               after this action. \n
 *               Furthermore this function call resets the ADD_DATA_OVFLW interrupt. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiFreeEmbDataFifo(void)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
   
    // Set FLUSH_FIFO bit - Reset MIPI embedded data fifo read and write pointer
    // This bit have to be set just for one CPU clock...the rest will be done on hardware site.
    REG_SET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_FLUSH_FIFO, ON);
   
    // Reset MIPI ADD_DATA_OVFLW and ADD_DATA_FILL_LEVEL interrupt
    // Register value will be automatically set back to "0" by the hardware
    REG_SET_SLICE(ptMrvReg->mipi_icr, MRV_MIPI_ICR_ADD_DATA_OVFLW, 1);
    REG_SET_SLICE(ptMrvReg->mipi_icr, MRV_MIPI_ICR_ADD_DATA_FILL_LEVEL, 1);  // [hf]: I think this should be also reset

    // Unset FLUSH_FIFO bit
    REG_SET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_FLUSH_FIFO, OFF);

    return RET_SUCCESS; 
#else

   return RET_NOTSUPP;
#endif
} // MrvMipiFreeEmbDataFifo()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiSetShutdownLane \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful shutdow lane value modification. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
 *  \PARAMETERS  bShutdownLaneVal = New MIPI shutdown lane value for a specific lane. \n
 *               uMipiLane        = For this MIPI lane the shutdown value should be set. \n
 *                                  Value range: 1 (lane 1) to 4 (lane 4). \n
 *  \DESCRIPTION Function sets the MIPI shutdown value (MIPI module output signal \n
 *               that can be used as reset signal) for a specific MIPI lane. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiSetShutdownLane(const BOOL bShutdownLaneVal, const UINT8 uMipiLane)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 shutdown_lane = REG_GET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_SHUTDOWN_LANE);

    if ((uMipiLane > MAX_NUMBER_OF_MIPI_LANES) || (uMipiLane < 1))
    {
        return RET_OUTOFRANGE;
    }

    if(bShutdownLaneVal)
    { // Set shutdown bit for given lane to "1"
        shutdown_lane |=  (1u << (uMipiLane - 1));
    }
    else
    { // Set shutdown bit for given lane to "0"
        shutdown_lane &= ~(1u << (uMipiLane - 1));
    }
    REG_SET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_SHUTDOWN_LANE, shutdown_lane);

    return RET_SUCCESS; 
#else
   UNUSED_PARAM1(bShutdownLaneVal);                // changed to avoid LINT warnings (Info 715)
   UNUSED_PARAM1(uMipiLane);                       // changed to avoid LINT warnings (Info 715)
   return RET_NOTSUPP;
#endif
} // MrvMipiSetShutdownLane()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiDefineNumberOfUsedLanes \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful number of used lane definition. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
 *  \PARAMETERS  uMipiLanes = This number of MIPI lane have to be used for MIPI. \n
 *                            data transmission. \n
 *  \DESCRIPTION Function defines the number of MIPI lanes that have to be used. \n
 *               for a MIPI data transmission. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiDefineNumberOfUsedLanes(const UINT8 uMipiLanes)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if ((uMipiLanes > MAX_NUMBER_OF_MIPI_LANES) || (uMipiLanes < 1))
    {
        return RET_OUTOFRANGE;
    }

    REG_SET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_NUM_LANES, (uMipiLanes - 1)); // uMipiLanes is one-based, register stores zero-based

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(uMipiLanes);                      // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiDefineNumberOfUsedLanes()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvSkipDataProcessingInCaseOfSotIrq \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful setting. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  bSkipDataProcessing = TRUE:  Processing will be skipped. \n
 *                                     FALSE: Processing will NOT be skipped. \n
 *  \DESCRIPTION Depending on the SOT interrupt this Function defines that \n
 *               the received MIPI data will be processed or skipped. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvSkipDataProcessingInCaseOfSotIrq(const BOOL bSkipDataProcessing)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    REG_SET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_ERR_SOT_HS_SKIP, (bSkipDataProcessing) ? ON : OFF);
   
    return RET_SUCCESS;
#else
    UNUSED_PARAM1(bSkipDataProcessing);             // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvSkipDataProcessingInCaseOfSotIrq()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvSkipDataProcessingInCaseOfSotSyncIrq \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful setting. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  bSkipDataProcessing = TRUE:  Processing will be skipped. \n
 *                                     FALSE: Processing will NOT be skipped. \n
 *  \DESCRIPTION Depending on the SOT_SYNC interrupt this Function defines that \n
 *               the received MIPI data will be processed or skipped. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvSkipDataProcessingInCaseOfSotSyncIrq( const BOOL bSkipDataProcessing )
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    REG_SET_SLICE(ptMrvReg->mipi_ctrl, MRV_MIPI_ERR_SOT_SYNC_HS_SKIP, (bSkipDataProcessing) ? ON : OFF);
   
    return RET_SUCCESS;
#else
    UNUSED_PARAM1(bSkipDataProcessing);             // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvSkipDataProcessingInCaseOfSotSyncIrq()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetStatusRegisterVal \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful MIPI status register request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puMipiStatusRegisterVal = Current MIPI status register value \n
 *                                         will be written into this variable.\n
 *  \DESCRIPTION Function returns the current MIPI status register value. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiGetStatusRegisterVal(UINT32 *puMipiStatusRegisterVal)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puMipiStatusRegisterVal)
    {
        return RET_NULL_POINTER;
    }
   
    *puMipiStatusRegisterVal = REG_READ(ptMrvReg->mipi_status);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(puMipiStatusRegisterVal);          // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetStatusRegisterVal()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiAddDataAvailable \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful MIPI additional data request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  pbMipiAddDataAvailable = Status of current additional data memory \n
 *                                        will be written into this variable.\n
 *                                        TRUE:  Additional data is available \n
 *                                        FALSE: Additional data is NOT available \n
 *  \DESCRIPTION Function identifies the availability of additional data. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiAddDataAvailable(BOOL *pbMipiAddDataAvailable)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if(!pbMipiAddDataAvailable)
    {
        return RET_NULL_POINTER;
    }
   
    *pbMipiAddDataAvailable = (REG_GET_SLICE(ptMrvReg->mipi_status, MRV_MIPI_ADD_DATA_AVAIL)) ? TRUE : FALSE;

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(pbMipiAddDataAvailable);           // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiAddDataAvailable()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetLaneStopState \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful MIPI additional data request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
 *  \PARAMETERS  uLaneNumber = Stop state of this MIPI line should be identified \n
 *               pbStopState = The current MIPI lane stop state will be witten into.\n
 *                             this variable. \n
 *  \DESCRIPTION Function identifies the stop state of a specific MIPI lane \n
 *               (is this lane active/inactive for MIPI data transmission). \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiGetLaneStopState(UINT8 uLaneNumber, BOOL *pbStopState)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 uLane = 0;

    if (!pbStopState)
    {
        return RET_NULL_POINTER;
    }

    if ((uLaneNumber > MAX_NUMBER_OF_MIPI_LANES) || (uLaneNumber < 1))
    {
        return RET_OUTOFRANGE;
    }

    uLane = 1 << (uLaneNumber - 1);
    *pbStopState = (REG_GET_SLICE(ptMrvReg->mipi_status, MRV_MIPI_STOPSTATE) & uLane) ? TRUE : FALSE;

   return RET_SUCCESS; 
#else
    UNUSED_PARAM(uLaneNumber);                      // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(pbStopState);                      // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetLaneStopState()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiActivateInterrupts \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful interrupt register \n
 *                            activation/deactivation. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
 *  \PARAMETERS  uActivatedInterrupts = Every bit of this parameter activates or \n
 *                                      deactivates a specific MIPI interrupt \n
 *                                      ("1" = interrupt is activated; "0" = interrupt 
 *                                      is deactivated). \n
 *  \DESCRIPTION Function activates/deactivate MIPI interrupts. \n 
 *  \NOTES       Combine interrupt mask defines (see mrv_mipi.h) as parater value \n
 *               for a secure parameter handling \n
 *               (f.e. MIPI_INTRPT_ADDB_DATA_OVFLW | MIPI_INTRPT_FRAME_END). \n
 *
 *****************************************************************************/
RESULT MrvMipiActivateInterrupts(const UINT32 uActivatedInterrupts)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (uActivatedInterrupts & ~ MRV_MIPI_IMSC_ALL_IRQS_MASK)
    {
        return RET_OUTOFRANGE;
    }

    REG_WRITE(ptMrvReg->mipi_imsc, uActivatedInterrupts);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(uActivatedInterrupts);            // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiActivateInterrupts()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetImscRegister \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful interrupt register request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puActivatedInterrupt - Every bit of this variable will be set \n
 *                                      in case of an activated corresponding \n
 *                                      interrupt. Is case of a deactivated \n
 *                                      interrupt the bit will be set to "0". \n
 *  \DESCRIPTION Function determines the activated or deactivated status of all \n
 *               MIPI interrupts. \n 
 *  \NOTES       Combine interrupt mask defines (see mrv_mipi.h) to analyse the pointer \n
 *               variable content. \n 
 *               (f.e. MIPI_INTRPT_ADDB_DATA_OVFLW | MIPI_INTRPT_FRAME_END). \n
 *
 *****************************************************************************/
RESULT MrvMipiGetImscRegister(UINT32 *puActivatedInterrupt)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puActivatedInterrupt)
    {
        return RET_NULL_POINTER;
    }
   
    // Determine activated and deactivated register status
    *puActivatedInterrupt = REG_READ(ptMrvReg->mipi_imsc);

    return RET_SUCCESS;
#else
    UNUSED_PARAM(puActivatedInterrupt);             // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetImscRegister()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetGeneralInterruptState \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful general interrupt status request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puGeneralInterruptStatus = Pointer variable will be used to return \n
 *                                          the general interrupt status value. \n
 *  \DESCRIPTION Function determines the current state of activated AND \n
 *               deactivated MIPI interrupts. \n 
 *  \NOTES       Combine interrupt mask defines (see mrv_mipi.h) to analyse the pointer \n
 *               variable content. \n 
 *               (f.e. MIPI_INTRPT_ADDB_DATA_OVFLW | MIPI_INTRPT_FRAME_END). \n
 *
 *****************************************************************************/
RESULT MrvMipiGetGeneralInterruptState(UINT32 *puGeneralInterruptState)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puGeneralInterruptState)
    {
        return RET_NULL_POINTER;
    }

    // return the interrupt status of activated AND deactivated MIPI interrupts
    *puGeneralInterruptState = REG_READ(ptMrvReg->mipi_ris);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(puGeneralInterruptState);          // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetGeneralInterruptState()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetActivatedInterruptState \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful activated interrupt status request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puActivatedInterruptState = Pointer variable will be used to return \n
 *                                           the activated interrupt status value. \n
 *  \DESCRIPTION Function determines ONLY the current status of the activated MIPI \n
 *               interrupts. \n 
 *  \NOTES       Combine interrupt mask defines (see mrv_mipi.h) to analyse the pointer \n
 *               variable content. \n 
 *               (f.e. MIPI_INTRPT_ADDB_DATA_OVFLW | MIPI_INTRPT_FRAME_END). \n
 *
 *****************************************************************************/
RESULT MrvMipiGetActivatedInterruptState(UINT32 *puActivatedInterruptState)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puActivatedInterruptState)
    {
        return RET_NULL_POINTER;
    }

    // return ONLY the interrupt status of activated MIPI interrupts
    *puActivatedInterruptState = REG_READ(ptMrvReg->mipi_mis);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(puActivatedInterruptState);        // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetActivatedInterruptState()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiResetAllInterrups \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful interrupt register reset. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Function resets all MIPI interrupt values. After this reset \n
 *               action the MIPI_ICR register will be automatically set back to \n
 *               the value 0x00000000.
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiResetAllInterrups(void)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // Reset  a l l  MIPI interrupts
    // The register bits will be automatically set back to "0" by the hardware
    REG_WRITE(ptMrvReg->mipi_icr, MRV_MIPI_ICR_ALL_IRQS_MASK);

    return RET_SUCCESS;
#else

    return RET_NOTSUPP;
#endif
} // MrvMipiResetAllInterrups()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiResetInterrups \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful interrupt register reset. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range \n
 *  \PARAMETERS  uInterruptResetMask = Each bit of this parameter corresponds  \n
 *                                     to a Mipi interrupt. Bit = "1" means that \n
 *                                     the interrupt will be reset. \n
 *  \DESCRIPTION Function resets all MIPI interrupt values. After this reset \n
 *               action the MIPI_ICR register will be automatically set back to \n
 *               the value 0x00000000.
 *  \NOTES       Combine interrupt mask defines (see mrv_mipi.h) to analyse the pointer \n
 *               variable content. \n 
 *               (f.e. MIPI_INTRPT_ADDB_DATA_OVFLW | MIPI_INTRPT_FRAME_END). \n
 *
 *****************************************************************************/
RESULT MrvMipiResetInterrups(const UINT32 uInterruptResetMask)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (uInterruptResetMask & ~ MRV_MIPI_ICR_ALL_IRQS_MASK)
    {
        return RET_OUTOFRANGE;
    }
   
    // Reset specified MIPI interrupts
    // The register bits will be automatically set back to "0" by the hardware
    REG_WRITE(ptMrvReg->mipi_icr, uInterruptResetMask);
   
    return RET_SUCCESS;
#else
    UNUSED_PARAM1(uInterruptResetMask);             // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiResetInterrups()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiSetInterruptRegister \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful MIPI interrupt register manipulation. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range \n
 *  \PARAMETERS  uInterruptRegisterMask     = Mask to determine which MIPI register  \n
 *                                            value should be modified. \n
 *               uNewInterruptRegisterValue = New interrupt register value. \n
 *  \DESCRIPTION Function sets specific MIPI interrupts (given by the mask) to  \n
 *               specific new values. \n
 *  \NOTES       Be careful to use this function to avoid some strange behavior.  \n
 *               Basically this function was implemented just for test activities. \n
 *
 *               Ensure that all uNewInterruptRegisterValue parameter bits are correct set \n
 *               for thoose register values that should be modified.\n
 *
 *               Please use interrupt mask defines (see mrv_mipi.h) as mask parater value \n
 *               for a secure parameter handling 
 *               (f.e. MIPI_INTRPT_ADDB_DATA_OVFLW | MIPI_INTRPT_FRAME_END as subset or \n
 *               MIPI_INTRPT_ALL to modify all interupt values). \n
 *
 *****************************************************************************/
RESULT MrvMipiSetInterruptRegister(const UINT32 uInterruptRegisterMask, const UINT32 uNewInterruptRegisterValue)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mipi_isr = REG_READ(ptMrvReg->mipi_isr);

    if ((uInterruptRegisterMask | uNewInterruptRegisterValue) & ~ MRV_MIPI_ISR_ALL_IRQS_MASK)
    {
        return RET_OUTOFRANGE;
    }
   
    // Manipulate the MIPI interrupt register with the given masked value
    mipi_isr = (mipi_isr & ~uInterruptRegisterMask) | (uNewInterruptRegisterValue & uInterruptRegisterMask);
    REG_WRITE(ptMrvReg->mipi_isr, mipi_isr);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(uInterruptRegisterMask);          // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uNewInterruptRegisterValue);      // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiSetInterruptRegister()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetCurrentDataId \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful current data request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puVirtualChannel = The virtual channel of the currently handled \n
 *                                  MIPI Package will be written into this variable. \n
 *               puDataType       = The data type of the currently handled MIPI \n
 *                                  Package will be written into this variable. \n
 *  \DESCRIPTION Function determines the virtual channel and the data type of the \n
 *               current handled MIPI package. \n 
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiGetCurrentDataId(UINT8 *puVirtualChannel, UINT8 *puDataType)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puVirtualChannel || !puDataType)
    {
        return RET_NULL_POINTER;
    }

    // return the virtual channel of the currently handled MIPI package
    *puVirtualChannel = REG_GET_SLICE(ptMrvReg->mipi_cur_data_id, MRV_MIPI_VIRTUAL_CHANNEL);

    // return the data type of the currently handled MIPI package
    *puDataType = REG_GET_SLICE(ptMrvReg->mipi_cur_data_id, MRV_MIPI_DATA_TYPE);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(puVirtualChannel);                 // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puDataType);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetCurrentDataId()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetDataSelector \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful data selector request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puVirtualChannel = The virtual channel of the MIPI data package \n
 *                                  that will be processed will be written into \n
 *                                  this variable. \n
 *               puDataType       = The data type of the MIPI data package that \n
 *                                  will be processed will be written into this \n
 *                                  variable. \n
 *  \DESCRIPTION Function returns the configured MIPI virtual channel and data type. \n 
 *               of the data packages that will be processed. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiGetDataSelector(UINT8 *puVirtualChannel, UINT8 *puDataType)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puVirtualChannel || !puDataType)
    {
        return RET_NULL_POINTER;
    }
   
    // return the virtual channel of the currently handled MIPI package
    *puVirtualChannel = REG_GET_SLICE(ptMrvReg->mipi_img_data_sel, MRV_MIPI_VIRTUAL_CHANNEL_SEL);
   
    // return the data type of the currently handled MIPI package
    *puDataType = REG_GET_SLICE(ptMrvReg->mipi_img_data_sel, MRV_MIPI_DATA_TYPE_SEL);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(puVirtualChannel);                 // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puDataType);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetDataSelector()

/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetCompressedMode \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful data selector request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puCompMode       = Compressed mode of the MIPI data package \n
 *                                  that will be processed will be written into \n
 *                                  this variable. \n
 *               puPred           = Predictor of the MIPI data package \n
 *                                  that will be processed will be written into \n
 *                                  this variable. \n
 *  \DESCRIPTION Function returns the configured MIPI compressed mode  \n 
 *               of the data packages that will be processed. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiGetCompressedMode(UINT8 *puCompMode, UINT8 *puPred)
{
#if (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!puCompMode)
    {
        return RET_NULL_POINTER;
    }
    
    if (REG_GET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_COMPRESS_EN) == 1 )
    {
        // return the compressed raw data type of the currently handled MIPI package
        *puCompMode = REG_GET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_COMPRESSED_SEL);
    }
    else
    {
    	*puCompMode = MRV_MIPI_COMPRESSED_SEL_OFF;
    }
    if (REG_GET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_PREDICTOR_SEL) == 1 )
    {
        // return the predictor of the currently handled MIPI package
        *puPred = MIPI_COM_PREDICTOR_2;
    }
    else
    {
    	*puPred = MIPI_COM_PREDICTOR_1;
    }

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(puCompMode);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetCompressedMode()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiSetDataSelector \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful data selector request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
 *  \PARAMETERS  uVirtualChannel = Virtual channel of the MIPI data package \n
 *                                 that have to be processed. \n
 *               uDataType       = Data type of the MIPI data package that have \n
 *                                 to be processed. \n
 *  \DESCRIPTION Function configures the MIPI virtual channel and data type. \n 
 *               of the data packages that have to be processed. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiSetDataSelector(const UINT8 uVirtualChannel, const UINT8 uDataType)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if ((uVirtualChannel > MRV_MIPI_VIRTUAL_CHANNEL_MAX) || (uDataType > MRV_MIPI_DATA_TYPE_MAX))
    {
        return RET_OUTOFRANGE;
    }

    // configure the virtual channel of data that should be handled
    REG_SET_SLICE(ptMrvReg->mipi_img_data_sel, MRV_MIPI_VIRTUAL_CHANNEL_SEL, uVirtualChannel);

    // configure the data type of data that should be handled
    REG_SET_SLICE(ptMrvReg->mipi_img_data_sel, MRV_MIPI_DATA_TYPE_SEL, uDataType);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(uVirtualChannel);                 // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uDataType);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiSetDataSelector()

#ifdef TODO
	/*!****************************************************************************
	 *
	 *  \FUNCTION    MrvMipiSetCompressedMode \n
	 *  \RETURNVALUE RET_SUCCESS: In case of successful data selector request. \n
	 *               RET_NOTSUPP: This feature is not included in current software version. \n
	 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
	 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
	 *  \PARAMETERS  uCompMode       = Compressed Mode of the MIPI data package \n
	 *                                 that have to be processed. \n
	 *               uPred           = Predictor of the MIPI data package \n
	 *                                 that have to be processed. \n
	 *  \DESCRIPTION Function configures the MIPI compressed mode. \n
	 *  \NOTES       (emtpy) \n
	 *
	 *****************************************************************************/
	RESULT MrvMipiSetCompressedMode(const UINT8 uCompMode, const UINT8 uPred)
	{
	#if (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4)
	    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

	    if(uCompMode == MRV_MIPI_COMPRESSED_SEL_OFF)
	    {
	       REG_SET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_COMPRESS_EN, OFF);    	
	    }
	    else
	    {    	
	        if ((uCompMode > MRV_MIPI_COMP_MODE_MAX))
	        {
	            return RET_OUTOFRANGE;
	        }
	    
	        switch(uPred)
	        {
	        case MIPI_COM_PREDICTOR_1:
	            REG_SET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_PREDICTOR_SEL, OFF);
	            break;
	        case MIPI_COM_PREDICTOR_2:
	            REG_SET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_PREDICTOR_SEL, ON);
	            break;
	        default:
	            return RET_OUTOFRANGE;
	        }
	    
	       // configure the mipi compressed raw data type of data that should be handled
	       REG_SET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_COMPRESSED_SEL, uCompMode);
	       REG_SET_SLICE(ptMrvReg->mipi_compressed_mode, MRV_MIPI_COMPRESS_EN, ON);
	    }

	    return RET_SUCCESS; 
	#else
	    UNUSED_PARAM1(uCompMode);                 // changed to avoid LINT warnings (Info 715)
	    return RET_NOTSUPP;
	#endif
	} // MrvMipiSetCompressedMode()
#endif //TODO

/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetAdditionalDataSelector \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful additional data selector request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
 *  \PARAMETERS  eAddDataSelectorNum = For this additional data selector the \n
 *                                     configured virtual channel and data type \n
 *                                     have to be determined. \n
 *               puVirtualChannel = The virtual channel of the MIPI additional  \n
 *                                  data package that will be processed will be \n
 *                                  written into this variable. \n
 *               puDataType       = The data type of the MIPI additional data \n
 *                                  package that will be processed will be written \n
 *                                  into this variable. \n
 *  \DESCRIPTION Function returns the configured MIPI virtual channel and data type. \n 
 *               of the additional data packages that will be processed. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiGetAdditionalDataSelector(const teMipiAddDataSelector eAddDataSelectorNum, UINT8 *puVirtualChannel, UINT8 *puDataType)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mipi_add_data_sel = 0;

    if (!puVirtualChannel || !puDataType)
    {
        return RET_NULL_POINTER;
    }

    switch (eAddDataSelectorNum)
    {
    case eMipiAddDataSelector1:
        mipi_add_data_sel = REG_READ(ptMrvReg->mipi_add_data_sel_1);
        break;
    case eMipiAddDataSelector2:
        mipi_add_data_sel = REG_READ(ptMrvReg->mipi_add_data_sel_2);
        break;
    case eMipiAddDataSelector3:
        mipi_add_data_sel = REG_READ(ptMrvReg->mipi_add_data_sel_3);
        break;
    case eMipiAddDataSelector4:
        mipi_add_data_sel = REG_READ(ptMrvReg->mipi_add_data_sel_4);
        break;
    default:
        return RET_OUTOFRANGE;
    }

    // return the virtual channel of the currently handled MIPI package
    *puVirtualChannel = REG_GET_SLICE(mipi_add_data_sel, MRV_MIPI_ADD_DATA_VC_1);

    // return the data type of the currently handled MIPI package
    *puDataType = REG_GET_SLICE(mipi_add_data_sel, MRV_MIPI_ADD_DATA_TYPE_1);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(eAddDataSelectorNum);             // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puVirtualChannel);                 // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puDataType);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetAdditionalDataSelector()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiSetAdditionalDataSelector \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful data selector request. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_OUTOFRANGE: One or more parameter values are out of range. \n
 *  \PARAMETERS  eAddDataSelectorNum = For this additional data selector the \n
 *                                     virtual channel and data type of data \n
 *                                     that should be stored as additional data. \n
 *                                     will be set. \n
 *  \PARAMETERS  uVirtualChannel = From this virtual channel data should be \n
 *                                 stored as additional data. \n
 *               uDataType       = Mipi package data with this data type should be \n
 *                                 stored as additional data. \n
 *  \DESCRIPTION Function configures the MIPI virtual channel and data type \n 
 *               of MIPI packages that should be stored as additional data. \n
 *  \NOTES       (emtpy) \n
 *
 *****************************************************************************/
RESULT MrvMipiSetAdditionalDataSelector(const teMipiAddDataSelector eAddDataSelectorNum, const UINT8 uVirtualChannel, const UINT8 uDataType)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mipi_add_data_sel = 0;

    if ((uVirtualChannel > MRV_MIPI_VIRTUAL_CHANNEL_MAX) || (uDataType > MRV_MIPI_DATA_TYPE_MAX))
    {
        return RET_OUTOFRANGE;
    }

    // configure the virtual channel of data that should be stored as additional data
    REG_SET_SLICE(mipi_add_data_sel, MRV_MIPI_ADD_DATA_VC_1, uVirtualChannel);

    // configure the data type of data that should be stored as additional data
    REG_SET_SLICE(mipi_add_data_sel, MRV_MIPI_ADD_DATA_TYPE_1, uDataType);

    // Determine the additional data selector block
    switch (eAddDataSelectorNum)
    {
    case eMipiAddDataSelector1:
        REG_WRITE(ptMrvReg->mipi_add_data_sel_1, mipi_add_data_sel);
        break;
    case eMipiAddDataSelector2:
        REG_WRITE(ptMrvReg->mipi_add_data_sel_2, mipi_add_data_sel);
        break;
    case eMipiAddDataSelector3:
        REG_WRITE(ptMrvReg->mipi_add_data_sel_3, mipi_add_data_sel);
        break;
    case eMipiAddDataSelector4:
        REG_WRITE(ptMrvReg->mipi_add_data_sel_4, mipi_add_data_sel);
        break;
    default:
        return RET_OUTOFRANGE;
    }

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(eAddDataSelectorNum);             // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uVirtualChannel);                 // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(uDataType);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiSetDataSelector()


/*!****************************************************************************
 *
 *  \FUNCTION    MrvMipiGetAdditionalData \n
 *  \RETURNVALUE RET_SUCCESS: In case of successful embedded data copy. \n
 *               RET_NOTSUPP: This feature is not included in current software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  puDestAddbDataBuffer - Into this variable the additional data will \n
 *                                     be copied. \n
 *               ulDestAddbDataBufSize - Size of the destination buffer \n
 *               puNumOfAddDataBytes - Into this variable the number of copied \n
 *                                     additional data bytes will be written. \n
 *  \DESCRIPTION This function copies additional data from the hardware additional \n
 *               data fifo memory to the destination buffer. \n
 *  \NOTES       \n
 *
 *****************************************************************************/
RESULT MrvMipiGetAdditionalData(UINT8 *puDestAddbDataBuffer, const UINT32 ulDestAddbDataBufSize, UINT32 *puNumOfAddDataBytes)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 mipi_add_data = 0;
    UINT32 ulDestAddbDataBufMaxSizeAligned = 0;

    if (!puDestAddbDataBuffer || !puNumOfAddDataBytes)
    {
        return RET_NULL_POINTER;
    }

    *puNumOfAddDataBytes = 0;
    ulDestAddbDataBufMaxSizeAligned = ulDestAddbDataBufSize & 0xFFFFFFFC;//aligned to 32bit

    while ( (REG_GET_SLICE(ptMrvReg->mipi_status, MRV_MIPI_ADD_DATA_AVAIL) 
          & *puNumOfAddDataBytes < (ulDestAddbDataBufMaxSizeAligned) ) )
    { // there is additional data stored in fifo. copy next 4 bytes into target buffer
  #if 1 // This 32-bit assign requires little endian! (first byte is in fifo bits [0:7])
        UNUSED_PARAM(mipi_add_data);
        *(UINT32*)puDestAddbDataBuffer = REG_READ(ptMrvReg->mipi_add_data_fifo);
        puDestAddbDataBuffer += 4; // next 4 bytes
  #else // This loop makes the code endianess indepedant but a bit slower
        mipi_add_data = REG_READ(ptMrvReg->mipi_add_data_fifo);
        for (int i = 0; i < 4; i++)
        { // copy bytes into buffer (first byte is in mipi_add_data bits [0:7] - LSB)
            *puDestAddbDataBuffer = mipi_add_data & 0xFF; // get LSB
            puDestAddbDataBuffer++;
            mipi_add_data >>= 8; // next byte
        }
  #endif
        *puNumOfAddDataBytes += 4;
    }

    return RET_SUCCESS; 
#else
    UNUSED_PARAM(puDestAddbDataBuffer);             // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM1(ulDestAddbDataBufSize);              // changed to avoid LINT warnings (Info 715)
    UNUSED_PARAM(puNumOfAddDataBytes);              // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiGetAdditionalData()


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvMipiWaitForIrq \n
 *  \RETURNVALUE RET_SUCCESS    - everything ok \n
 *               RET_FAILURE    - timeout during wait \n
 *               RET_OUTOFRANGE - invalid or no IRQ bit(s) specified
 *  \PARAMETERS  Mask for MIPI_RIS register to specify the bits to wait for \n
 *
 *  \NOTES       \n
 *
 *  \DESCRIPTION Waits until the specified bit becomes signaled in the mi_ris
 *               register. \n
 */
/*****************************************************************************/
RESULT MrvMipiWaitForIrq(UINT32 ulBitMask)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulStartTick,ulTickFreq;
    ulTickFreq  = OS_GetFrequency();

    if ((ulBitMask & ~ MRV_MIPI_RIS_ALL_IRQS_MASK) || !(ulBitMask & MRV_MIPI_RIS_ALL_IRQS_MASK))
    {
        return RET_OUTOFRANGE;
    }

    ulStartTick = OS_GetTick();

    while ((ptMrvReg->mipi_ris & ulBitMask) == 0) // don't use REG_READ here ;)
    {
        if (((OS_GetTick() - ulStartTick) / ulTickFreq) > 12)
        {
            DBG_OUT((DERR, "MrvMipiWaitForIrq(): TIMEOUT\n"));

            // some more info about MIPI state
            DBG_OUT((DINFO, "  MIPI_RIS      = 0x%08lX\n", ptMrvReg->mipi_ris));

            return RET_FAILURE;
        }
    }


    return RET_SUCCESS;
#else
    UNUSED_PARAM(ulBitMask);                       // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
} // MrvMipiWaitForIrq()


/*!****************************************************************************
*
*  \FUNCTION    MrvMipiSetAddDataFillLevel \n
*  \RETURNVALUE RET_SUCCESS: In case of successful additional data fill level setting. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  ulFillLevel - Fill level of additional data fifo will be set (32bit aligned). \n
*  \DESCRIPTION Sets the fill level of additional data fifo for the fill level \n
*               interrupt generation. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvMipiSetAddDataFillLevel(const UINT32 ulFillLevel)
{
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    if (ulFillLevel & ~ MRV_MIPI_ADD_DATA_FILL_LEVEL_MASK)
    {
        return RET_OUTOFRANGE;
    }

    // Sets the fill level of embedded data fifo.
    REG_SET_SLICE(ptMrvReg->mipi_add_data_fill_level, MRV_MIPI_ADD_DATA_FILL_LEVEL, ulFillLevel);

    return RET_SUCCESS; 
#else
    UNUSED_PARAM1(ulFillLevel);                              // changed to avoid LINT warnings (Info 715)
    return RET_NOTSUPP;
#endif
}
