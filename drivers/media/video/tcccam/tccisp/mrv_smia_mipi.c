/*****************************************************************************/
/*!
 *  \file        mrv_smia_mipi.c \n
 *  \version     1.0 \n
 *  \author      Schueler \n
 *  \brief       Marvin SMIA Mipi low level driver functionality
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_smia_mipi.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "stdinc.h"

#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    #include "reg_access.h"
    #include "mrv_priv.h"
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information



/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiInitializeModule \n
*  \RETURNVALUE RET_SUCCESS: In case of successful register initialization. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  none \n
*  \DESCRIPTION Initializes SMIA filter registers with default reset values. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaMipiInitializeModule(void)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaInitializeModule();
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}

/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetControlRegisterStatus \n
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
RESULT MrvSmiaMipiGetControlRegisterStatus(UINT32 *puSmiaControlRegisterStatus)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetControlRegisterStatus(puSmiaControlRegisterStatus);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puSmiaControlRegisterStatus);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiSetControlRegisterStatus \n
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
RESULT MrvSmiaMipiSetControlRegisterStatus(const UINT32 uSmiaControlRegisterStatus)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaSetControlRegisterStatus(uSmiaControlRegisterStatus); 
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uSmiaControlRegisterStatus);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiActivateModule \n
*  \RETURNVALUE RET_SUCCESS: In case of successful module activation/deactivation. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  bModuleActive = TRUE  : SMIA module is activated. \n
*               bModuleActive = FALSE : SMIA module is deactivated. \n
*  \DESCRIPTION Activate/deactivates SMIA module actions. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaMipiActivateModule(const BOOL bModuleActive)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaActivateModule(bModuleActive); 
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(bModuleActive);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}



/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiActivateCfgUpdateSignal \n
*  \RETURNVALUE RET_SUCCESS: In case of successful cfg update signal activation/deactivation. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  bCfgUpdateSignalActive = TRUE  : cfg update signal is activated. \n
*               bCfgUpdateSignalActive = FALSE : cfg update signal is deactivated. \n
*  \DESCRIPTION At the end of each frame the hardware cfg update signal could be set. \n
*               This will be acivated/deactivated by the use of this function. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaMipiActivateCfgUpdateSignal(const BOOL bCfgUpdateSignalActive)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaActivateCfgUpdateSignal(bCfgUpdateSignalActive);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(bCfgUpdateSignalActive);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiResetInterrups \n
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
RESULT MrvSmiaMipiResetInterrups(void)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaResetInterrups();
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiFreeEmbDataFifo \n
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
RESULT MrvSmiaMipiFreeEmbDataFifo(void)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaFreeEmbDataFifo();
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiEmbDataAvailabe \n
*  \RETURNVALUE RET_SUCCESS: Successful embedded data fifo request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  pbEmbDataAvail - TRUE: In case of available embedded data.\n
*                                FALSE: There is no embedded data stored in fifo. \n
*  \DESCRIPTION Function determines the availability of embedded data. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaMipiEmbDataAvailabe(BOOL *pbEmbDataAvail)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaEmbDataAvailabe(pbEmbDataAvail);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(pbEmbDataAvail);
    return RET_NOTSUPP;
#endif // #if ( MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiActivateInterrupts \n
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
RESULT MrvSmiaMipiActivateInterrupts(const UINT32 uActivatedInterrupt)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaActivateInterrupts(uActivatedInterrupt);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uActivatedInterrupt);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetImscRegister \n
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
RESULT MrvSmiaMipiGetImscRegister(UINT32 *puActivatedInterrupt)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetImscRegister(puActivatedInterrupt);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puActivatedInterrupt);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetGeneralInterruptState \n
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
RESULT MrvSmiaMipiGetGeneralInterruptState(UINT32 *puGeneralInterruptState)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetGeneralInterruptState(puGeneralInterruptState); 
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puGeneralInterruptState);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetActivatedInterruptState \n
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
RESULT MrvSmiaMipiGetActivatedInterruptState(UINT32 *puActivatedInterruptState)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetActivatedInterruptState(puActivatedInterruptState);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puActivatedInterruptState);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiSetInterruptRegister \n
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
RESULT MrvSmiaMipiSetInterruptRegister(const UINT32 uInterruptRegisterMask, const UINT32 uNewInterruptRegisterValue)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaSetInterruptRegister(uInterruptRegisterMask, uNewInterruptRegisterValue); 
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uInterruptRegisterMask);
    UNUSED_PARAM1(uNewInterruptRegisterValue);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiSetPictureFormat \n
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
RESULT MrvSmiaMipiSetPictureFormat(const UINT32 uPictureFormat)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaSetPictureFormat(uPictureFormat);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uPictureFormat);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetPictureFormat \n
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
RESULT MrvSmiaMipiGetPictureFormat(UINT32 *puPictureFormat)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetPictureFormat(puPictureFormat);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puPictureFormat);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiSetFrameLines \n
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
RESULT MrvSmiaMipiSetFrameLines(const UINT32 uNumOfEmbDataLines, const UINT32 uNumOfPicDataLines)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaSetFrameLines(uNumOfEmbDataLines, uNumOfPicDataLines);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(uNumOfEmbDataLines);
    UNUSED_PARAM1(uNumOfPicDataLines);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetFrameLines \n
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
RESULT MrvSmiaMipiGetFrameLines(UINT32 *puNumOfEmbDataLines, UINT32 *puNumOfPicDataLines)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetFrameLines(puNumOfEmbDataLines, puNumOfPicDataLines);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puNumOfEmbDataLines);
    UNUSED_PARAM(puNumOfPicDataLines);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiSetEmbDataStorage \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data storage configuration. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*               RET_OUTOFRANGE: One or more parameter values are out of range. \n
*  \PARAMETERS  ptsEmbDataWindow1 - Embedded data from this SMIA data window will be stored. \n
*               (ptsEmbDataWindow1->usVOffs)
*                              "0" means that no embedded data will be stored. \n
*                              "1" means embedded data from the first data line \n
*                              will be stored. \n
*                              To store embedded data that will be received \n
*                              behind the picture data lines a value with the \n
*                              number of embedded data lines before picture data \n
*                              lines + number of picture data lines + number of \n
*                              embedded data lines after picture data have to be \n
*                              set as parameter. \n
*               (ptsEmbDataWindow1->usHOffs)
*                              Embedded data from this data line position will \n
*                              be stored. \n
*                              "0" means that no embedded data will be stored. \n
*                              "1" means embedded data from the first data line \n
*                              position will be stored. \n
*               (ptsEmbDataWindow1->usHSize)
*                              This number of embedded data bytes in a line will be stored. \n
*                              This value has to be a multiple of the value "4"!! \n
*               (ptsEmbDataWindow1->usVSize)
*                              This number of embedded data lines will be stored. \n
*               ptsEmbDataWindow2 - Embedded data from this SMIA data window will be stored. \n
*                                   (like window 1) \n
*  \DESCRIPTION Because a lot of embedded data could be received via SMIA the \n
*               hardware stores only a subset of this data amount. This Function \n
*               configures the SMIA hardware module to store only an embedded data subset. \n
*  \NOTES       The both window areas shouldn't overlap \n
*
*****************************************************************************/
RESULT MrvSmiaMipiSetEmbDataStorage(const tsMrvWindow *ptsEmbDataWindow1, const tsMrvWindow *ptsEmbDataWindow2)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!ptsEmbDataWindow1 && !ptsEmbDataWindow2) // at least one window must be specified
    {
        return RET_NULL_POINTER;
    }

    if (ptsEmbDataWindow1)
    {
        if ((ptsEmbDataWindow1->usHOffs & ~ MRV_SMIA_EMB_HSTART1_VALID_MASK) ||
            (ptsEmbDataWindow1->usHSize & ~ MRV_SMIA_EMB_HSIZE1_VALID_MASK)    )
        {
            return RET_OUTOFRANGE;
        }

        // From this embedded data line the SMIA module should store some embedded data.
        REG_SET_SLICE(ptMrvReg->smia_emb_vstart1, MRV_SMIA_EMB_VSTART1, ptsEmbDataWindow1->usVOffs);
        // Beginning at this embedded data line position the SMIA module should store some embedded data.
        REG_SET_SLICE(ptMrvReg->smia_emb_hstart1, MRV_SMIA_EMB_HSTART1, ptsEmbDataWindow1->usHOffs);
        // This number of embedded data bytes in a line should be stored from the SMIA module.
        REG_SET_SLICE(ptMrvReg->smia_emb_hsize1,  MRV_SMIA_EMB_HSIZE1,  ptsEmbDataWindow1->usHSize);
        // This number of embedded data lines should be stored from the SMIA module.
        REG_SET_SLICE(ptMrvReg->smia_emb_vsize1,  MRV_SMIA_EMB_VSIZE1,  ptsEmbDataWindow1->usVSize);
    }

    if (ptsEmbDataWindow2)
    {
        if ((ptsEmbDataWindow2->usHOffs & ~ MRV_SMIA_EMB_HSTART2_VALID_MASK) ||
            (ptsEmbDataWindow2->usHSize & ~ MRV_SMIA_EMB_HSIZE2_VALID_MASK)    )
        {
            return RET_OUTOFRANGE;
        }

        // From this embedded data line the SMIA module should store some embedded data.
        REG_SET_SLICE(ptMrvReg->smia_emb_vstart2, MRV_SMIA_EMB_VSTART2, ptsEmbDataWindow2->usVOffs);
        // Beginning at this embedded data line position the SMIA module should store some embedded data.
        REG_SET_SLICE(ptMrvReg->smia_emb_hstart2, MRV_SMIA_EMB_HSTART2, ptsEmbDataWindow2->usHOffs);
        // This number of embedded data bytes in a line should be stored from the SMIA module.
        REG_SET_SLICE(ptMrvReg->smia_emb_hsize2,  MRV_SMIA_EMB_HSIZE2,  ptsEmbDataWindow2->usHSize);
        // This number of embedded data lines should be stored from the SMIA module.
        REG_SET_SLICE(ptMrvReg->smia_emb_vsize2,  MRV_SMIA_EMB_VSIZE2,  ptsEmbDataWindow2->usVSize);
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(ptsEmbDataWindow1);
    UNUSED_PARAM1(ptsEmbDataWindow2);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetEmbDataStorage \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data storage request. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  ptsEmbDataWindow1 - Embedded data from this SMIA data window 1 will be stored. \n
*               ptsEmbDataWindow2 - Embedded data from this SMIA data window 2 will be stored. \n
*  \DESCRIPTION Determines the current SMIA module embedded data storage information. \n
*               The SMIA window 1 and/or 2 of embedded data that \n
*               will be stored from the SMIA module will be returned. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaMipiGetEmbDataStorage(tsMrvWindow *ptsEmbDataWindow1, tsMrvWindow *ptsEmbDataWindow2)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!ptsEmbDataWindow1 && !ptsEmbDataWindow2) // at least one window must be specified
    {
        return RET_NULL_POINTER;
    }

    if (ptsEmbDataWindow1)
    {
        // From this embedded data line the SMIA module should store some embedded data.
        ptsEmbDataWindow1->usVOffs = REG_GET_SLICE(ptMrvReg->smia_emb_vstart1, MRV_SMIA_EMB_VSTART1);
        // Beginning at this embedded data line position the SMIA module should store some embedded data.
        ptsEmbDataWindow1->usHOffs = REG_GET_SLICE(ptMrvReg->smia_emb_hstart1, MRV_SMIA_EMB_HSTART1);
        // This number of embedded data bytes in a line should be stored from the SMIA module.
        ptsEmbDataWindow1->usHSize = REG_GET_SLICE(ptMrvReg->smia_emb_hsize1,  MRV_SMIA_EMB_HSIZE1);
        // This number of embedded data lines should be stored from the SMIA module.
        ptsEmbDataWindow1->usVSize = REG_GET_SLICE(ptMrvReg->smia_emb_vsize1,  MRV_SMIA_EMB_VSIZE1);
    }

    if (ptsEmbDataWindow2)
    {
        // From this embedded data line the SMIA module should store some embedded data.
        ptsEmbDataWindow2->usVOffs = REG_GET_SLICE(ptMrvReg->smia_emb_vstart2, MRV_SMIA_EMB_VSTART2);
        // Beginning at this embedded data line position the SMIA module should store some embedded data.
        ptsEmbDataWindow2->usHOffs = REG_GET_SLICE(ptMrvReg->smia_emb_hstart2, MRV_SMIA_EMB_HSTART2);
        // This number of embedded data bytes in a line should be stored from the SMIA module.
        ptsEmbDataWindow2->usHSize = REG_GET_SLICE(ptMrvReg->smia_emb_hsize2,  MRV_SMIA_EMB_HSIZE2);
        // This number of embedded data lines should be stored from the SMIA module.
        ptsEmbDataWindow2->usVSize = REG_GET_SLICE(ptMrvReg->smia_emb_vsize2,  MRV_SMIA_EMB_VSIZE2);
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(ptsEmbDataWindow1);
    UNUSED_PARAM(ptsEmbDataWindow2);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}


/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetEmbData \n
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
RESULT MrvSmiaMipiGetEmbData(UINT8 *puDestEmbDataBuffer, const UINT32 ulDestEmbDataBufSize, UINT32 *puNumOfEmbDataBytes)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetEmbData(puDestEmbDataBuffer, ulDestEmbDataBufSize, puNumOfEmbDataBytes);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(puDestEmbDataBuffer);
    UNUSED_PARAM1(ulDestEmbDataBufSize);
    UNUSED_PARAM(puNumOfEmbDataBytes);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}

/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiSetEmbDataFillLevel \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data fill level setting. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  ulFillLevel - Fill level of embedded data fifo will be set. \n
*  \DESCRIPTION Sets the fill level of embedded data fifo for the fill level \n
*               interrupt generation. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaMipiSetEmbDataFillLevel(const UINT32 ulFillLevel)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaSetEmbDataFillLevel(ulFillLevel);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM1(ulFillLevel);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}

/*!****************************************************************************
*
*  \FUNCTION    MrvSmiaMipiGetEmbDataFillLevel \n
*  \RETURNVALUE RET_SUCCESS: In case of successful embedded data fill level getting. \n
*               RET_NOTSUPP: This feature is not included in current software version. \n
*               RET_NULL_POINTER: Invalid NULL pointer access. \n
*  \PARAMETERS  ulFillLevel - Fill level of embedded data fifo will be fetched. \n
*  \DESCRIPTION Gets the fill level of embedded data fifo for the fill level \n
*               interrupt generation. \n
*  \NOTES       (emtpy) \n
*
*****************************************************************************/
RESULT MrvSmiaMipiGetEmbDataFillLevel(UINT32 *pulFillLevel)
{
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
    return MrvSmiaGetEmbDataFillLevel(pulFillLevel);
#else // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    UNUSED_PARAM(pulFillLevel);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
}



