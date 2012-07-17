/*****************************************************************************/
/*!
 *  \file        mrv_isp.c \n
 *  \version     2.2 \n
 *  \author      Friebe \n
 *  \brief       Functions to access the Marvin ISP. \n
 *
 *  \revision    $Revision: 524 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-09-03 19:12:46 +0200 (Do, 03 Sep 2009) $ \n
 *               $Id: mrv_isp.c 524 2009-09-03 17:12:46Z neugebaa $ \n
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

#include "reg_access.h"
#include "mrv_priv.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/
// abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSelectPath \n
 *  \RETURNVALUE enum of the marvin ISP path that will be active  \n
 *  \PARAMETERS  ptIsiCfg .. [in] pointer to the ISI configuration structure \n
 *               pucWordsPerPixel .. [out] pointer to a variable to receive
 *                  the number of samples to take at marvins input that represents
 *                  a single pixel of the image sensor. \n
 *  \NOTES       This routine does not change any registers of the marvin.
 *               it is intended for informatorial purposes only. \n
 *
 *  \DESCRIPTION Selects the ISP path that will become active while processing
 *               data coming from an image sensor configured by the given ISI
 *               configuration struct. \n
 */
/*****************************************************************************/
eMrvIspPath MrvIspSelectPath(const tsIsiSensorConfig *ptIsiCfg, UINT8* pucWordsPerPixel)
{
    UINT8 ucWordsPerPixel;
    eMrvIspPath eRetVal;

    switch (ptIsiCfg->ulMode)
    {
    case ISI_MODE_DATA:
        eRetVal = eMrvIspPath_Raw;
        ucWordsPerPixel = 1;
        break;
    case ISI_MODE_PICT:
        eRetVal = eMrvIspPath_Raw;
        ucWordsPerPixel = 1;
        break;
    case ISI_MODE_RGB565:
        eRetVal = eMrvIspPath_Raw;
        ucWordsPerPixel = 2;
        break;
    case ISI_MODE_BT601:
        eRetVal = eMrvIspPath_YCbCr;
        ucWordsPerPixel = 2;
        break;
    case ISI_MODE_BT656:
        eRetVal = eMrvIspPath_YCbCr;
        ucWordsPerPixel = 2;
        break;
    case ISI_MODE_BAYER:
        eRetVal = eMrvIspPath_Bayer;
        ucWordsPerPixel = 1;
        break;
#if    ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) \
    ||  (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP)      \
    ||  (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
    case ISI_MODE_SMIA:
        switch (ptIsiCfg->ulSmiaMode)
        {
        case ISI_SMIA_MODE_RAW_12:
        case ISI_SMIA_MODE_RAW_10:
        case ISI_SMIA_MODE_RAW_8:
        case ISI_SMIA_MODE_RAW_8_TO_10_DECOMP:
            eRetVal = eMrvIspPath_Bayer;
            ucWordsPerPixel = 1;
                break;
        case ISI_SMIA_MODE_YUV_422:
            eRetVal = eMrvIspPath_YCbCr;
            ucWordsPerPixel = 2;
                break;
        case ISI_SMIA_MODE_YUV_420:
        case ISI_SMIA_MODE_RGB_444:
        case ISI_SMIA_MODE_RGB_565:
        case ISI_SMIA_MODE_RGB_888:
        case ISI_SMIA_MODE_COMPRESSED:
        case ISI_SMIA_MODE_RAW_7:
        case ISI_SMIA_MODE_RAW_6:
        default:
            eRetVal = eMrvIspPath_Raw;
            ucWordsPerPixel = 1;
            break;
        }
        break;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP)
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4) || \
         (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP) )
    case ISI_MODE_MIPI:
        switch (ptIsiCfg->ulMipiMode)
        {
        case ISI_MIPI_MODE_RAW_12:
        case ISI_MIPI_MODE_RAW_10:
        case ISI_MIPI_MODE_RAW_8:
            eRetVal = eMrvIspPath_Bayer;
            ucWordsPerPixel = 1;
                break;
        case ISI_MIPI_MODE_YUV422_8:
        case ISI_MIPI_MODE_YUV422_10:
            eRetVal = eMrvIspPath_YCbCr;
            ucWordsPerPixel = 2;
                break;
        case ISI_MIPI_MODE_YUV420_8:
        case ISI_MIPI_MODE_YUV420_10:
        case ISI_MIPI_MODE_LEGACY_YUV420_8:
        case ISI_MIPI_MODE_YUV420_CSPS_8:
        case ISI_MIPI_MODE_YUV420_CSPS_10:
        case ISI_MIPI_MODE_RGB444:
        case ISI_MIPI_MODE_RGB555:
        case ISI_MIPI_MODE_RGB565:
        case ISI_MIPI_MODE_RGB666:
        case ISI_MIPI_MODE_RGB888:
        case ISI_MIPI_MODE_RAW_7:
        case ISI_MIPI_MODE_RAW_6:
        default:
            eRetVal = eMrvIspPath_Raw;
            ucWordsPerPixel = 1;
            break;
        }
        break;
#endif
    case ISI_MODE_BAY_BT656:
        eRetVal = eMrvIspPath_Bayer;
        ucWordsPerPixel = 1;
        break;
    case ISI_MODE_RAW_BT656:
        eRetVal = eMrvIspPath_Raw;
        ucWordsPerPixel = 1;
        break;
    default:
        eRetVal = eMrvIspPath_Unknown;
        ucWordsPerPixel = 1;
    }

    if (pucWordsPerPixel)
    {
        *pucWordsPerPixel = ucWordsPerPixel;
    }
    return eRetVal;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetInputAquisition \n
 *  \RETURNVALUE RET_SUCCESS - everything ok \n
 *               RET_NOTSUPP - requested setting not supported \n
 *  \PARAMETERS  ISI configuration structure \n
 *
 *  \NOTES       the configuration is the same as for the ISI since
 *               the input acquisition setting must match the ISI
 *               setting \n
 *
 *  \DESCRIPTION configures the input acquisition according to the
 *               given config structure \n
 */
/*****************************************************************************/
RESULT MrvIspSetInputAquisition(const tsIsiSensorConfig *ptIsiCfg)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_ctrl     = REG_READ(ptMrvReg->isp_ctrl);
    UINT32 isp_acq_prop = REG_READ(ptMrvReg->isp_acq_prop);
    UINT8 ucSampleFactor; //factor between pixel count and amount of bytes to sample
    UINT8 ucBlackLines;   //number of additional black lines at frame start

    if (MrvIspSelectPath(ptIsiCfg, &ucSampleFactor) == eMrvIspPath_Unknown)
    {
        DBG_OUT((DERR, "MrvIspSetInputAquisition: no suitable ISP path detected\n"));
        return RET_NOTSUPP;
    }
    switch (ptIsiCfg->ulMode)
    {
    case ISI_MODE_DATA:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
        break;
    case ISI_MODE_PICT:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RAW);
        break;
    case ISI_MODE_RGB565:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RAW);
        break;
    case ISI_MODE_BT601:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_601);
        break;
    case ISI_MODE_BT656:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_656);
        break;
    case ISI_MODE_BAYER:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
        break;
    case ISI_MODE_BAY_BT656:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB656);
        break;
    case ISI_MODE_RAW_BT656:
        REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RAW656);
        break;
        // FIXME: implement support for the other modes too (Mipi)
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP))
    case ISI_MODE_SMIA:
        switch (ptIsiCfg->ulSmiaMode)
        {
        case ISI_SMIA_MODE_RAW_12:
        case ISI_SMIA_MODE_RAW_10:
        case ISI_SMIA_MODE_RAW_8:
        case ISI_SMIA_MODE_RAW_8_TO_10_DECOMP:
        case ISI_SMIA_MODE_RAW_7:
        case ISI_SMIA_MODE_RAW_6:
        case ISI_SMIA_MODE_YUV_422:
        case ISI_SMIA_MODE_YUV_420:
        case ISI_SMIA_MODE_RGB_888:
        case ISI_SMIA_MODE_RGB_565:
        case ISI_SMIA_MODE_RGB_444:
        case ISI_SMIA_MODE_COMPRESSED:
            {
                RESULT              iError             = RET_SUCCESS;
                tsIsiSensorSmiaInfo tIsiSensorSmiaInfo = {0};

                // get current settings from the sensor driver, if possible
                iError = IsiGetSensorSmiaInfo(&tIsiSensorSmiaInfo);
                if (iError != RET_SUCCESS)
                {
                    DBG_OUT((DERR, "MrvIspSetInputAquisition: Call of IsiGetSensorSmiaInfo() resulted in 0x%08lX.\n", iError));
                    return iError;
                }

                // Initialize the SMIA hardware module first
                iError = MrvSmiaInitializeModule();
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // Define the picture format of data to be received via SMIA
                switch (ptIsiCfg->ulSmiaMode)
                {
  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
                case ISI_SMIA_MODE_RAW_12:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RAW_12);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
  #endif // #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
                case ISI_SMIA_MODE_RAW_10:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RAW_10);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_SMIA_MODE_RAW_8:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RAW_8);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_SMIA_MODE_RAW_8_TO_10_DECOMP:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RAW_8_TO_10_DECOMP);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
                case ISI_SMIA_MODE_YUV_422:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_YUV_422);
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_601);
                    break;
  #endif // #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
                case ISI_SMIA_MODE_YUV_420:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_YUV_420);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
                    break;
                case ISI_SMIA_MODE_RAW_7:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RAW_7);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
                    break;
                case ISI_SMIA_MODE_RAW_6:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RAW_6);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
                    break;
                case ISI_SMIA_MODE_RGB_565:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RGB_565);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
                    break;
                case ISI_SMIA_MODE_RGB_888:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RGB_888);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
                    break;
                case ISI_SMIA_MODE_RGB_444:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_RGB_444);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
                    break;
                case ISI_SMIA_MODE_COMPRESSED:
                    iError = MrvSmiaSetPictureFormat(SMIA_PIC_FORMAT_COMPRESSED);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_DATA);
                    break;
  #endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE)
                default:
                    ASSERT("MrvIspSetInputAquisition()" == "default case should never be reached!");
                    iError = RET_NOTSUPP;
                    break;
                } // switch (ptIsiCfg->ulSmiaMode)

                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // Define the number of picture data and embedded data lines
                iError = MrvSmiaSetFrameLines(tIsiSensorSmiaInfo.ulSofEmbDataLines, tIsiSensorSmiaInfo.ulNumLines);
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // configure the embedded data without capturing any embedded data
                iError = MrvSmiaSetEmbDataStorage(0, tIsiSensorSmiaInfo.ulEmbHStart, tIsiSensorSmiaInfo.ulEmbHSize);
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // update the configuration --> obsolete

                // activate SMIA module
                iError = MrvSmiaActivateModule(TRUE);
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }
            }
                break;
        default:
            DBG_OUT((DERR, "MrvIspSetInputAquisition: ulSmiaMode not supported\n"));
            return RET_NOTSUPP;
        } // switch (ptIsiCfg->ulSmiaMode)
        break;
#endif // #if (MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP)
#if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)                // :(  Wer hat sich denn den Sch... ausgedacht :(
    case ISI_MODE_SMIA:
        switch (ptIsiCfg->ulSmiaMode)
        {
        case ISI_SMIA_MODE_RAW_12:
        case ISI_SMIA_MODE_RAW_10:
        case ISI_SMIA_MODE_RAW_8:
        case ISI_SMIA_MODE_RAW_8_TO_10_DECOMP:
        case ISI_SMIA_MODE_YUV_422:
            {
                RESULT              iError             = RET_SUCCESS;
                tsIsiSensorSmiaInfo tIsiSensorSmiaInfo = {0};

                // get current settings from the sensor driver, if possible
                iError = IsiGetSensorSmiaInfo(&tIsiSensorSmiaInfo);
                if (iError != RET_SUCCESS)
                {
                    DBG_OUT((DERR, "MrvIspSetInputAquisition: Call of IsiGetSensorSmiaInfo() resulted in 0x%08lX.\n", iError));
                    return iError;
                }

                // Initialize the SMIA hardware module first
                iError = MrvSmiaMipiInitializeModule();
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // Define the picture format of data to be received via SMIA
                switch (ptIsiCfg->ulSmiaMode)
                {
                case ISI_SMIA_MODE_RAW_12:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_12);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_SMIA_MODE_RAW_10:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_10);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_SMIA_MODE_RAW_8:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_8);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_SMIA_MODE_RAW_8_TO_10_DECOMP:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_8_TO_10_DECOMP);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_SMIA_MODE_YUV_422:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_8);
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_601);
                    break;
                default:
                    break;
                }
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // Define the number of picture data and embedded data lines
                iError = MrvSmiaMipiSetFrameLines(tIsiSensorSmiaInfo.ulSofEmbDataLines, tIsiSensorSmiaInfo.ulNumLines);
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // configure the embedded data without capturing any embedded data
                {
                    static tsMrvWindow tsEmbDataWindow1;                              // :( :( :( :(
                    static tsMrvWindow tsEmbDataWindow2;
                    tsEmbDataWindow1.usVOffs = 0;
                    tsEmbDataWindow1.usHOffs = 0;
                    tsEmbDataWindow1.usHSize = 0;
                    tsEmbDataWindow1.usVSize = 0;
                    tsEmbDataWindow2.usVOffs = 0;
                    tsEmbDataWindow2.usHOffs = 0;
                    tsEmbDataWindow2.usHSize = 0;
                    tsEmbDataWindow2.usVSize = 0;
                    iError = MrvSmiaMipiSetEmbDataStorage(&tsEmbDataWindow1, &tsEmbDataWindow2);
                }
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // update the configuration --> obsolete

                // activate SMIA module
                iError = MrvSmiaMipiActivateModule(TRUE);
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }
            }
                break;
        default:
            DBG_OUT((DERR, "MrvIspSetInputAquisition: ulSmiaMode not supported\n"));
            return RET_NOTSUPP;
        }
        break;
    case ISI_MODE_MIPI:
        switch (ptIsiCfg->ulMipiMode)
        {
        case ISI_MIPI_MODE_RAW_12:
        case ISI_MIPI_MODE_RAW_10:
        case ISI_MIPI_MODE_RAW_8:
        case ISI_MIPI_MODE_YUV422_8:
            {
                RESULT              iError             = RET_SUCCESS;
                tsIsiSensorSmiaInfo tIsiSensorSmiaInfo = {0};

                // get current settings from the sensor driver, if possible
                iError = IsiGetSensorSmiaInfo(&tIsiSensorSmiaInfo);
                if(iError != RET_SUCCESS)
                {
                    DBG_OUT((DERR, "MrvIspSetInputAquisition: Call of IsiGetSensorSmiaInfo() resulted in 0x%08lX.\n", iError));
                    return iError;
                }

                // Initialize the SMIA hardware module first
                iError = MrvSmiaMipiInitializeModule();
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // Define the picture format of data to be received via SMIA
                switch (ptIsiCfg->ulMipiMode)
                {
                case ISI_MIPI_MODE_RAW_12:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_12);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_MIPI_MODE_RAW_10:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_10);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_MIPI_MODE_RAW_8:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_8);
                    // set input acquisition to Bayer RGB mode
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_RGB);
                    break;
                case ISI_MIPI_MODE_YUV422_8:
                    iError = MrvSmiaMipiSetPictureFormat(SMIA_PIC_FORMAT_RAW_8);
                    REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_MODE, MRV_ISP_ISP_MODE_601);
                    break;
                default:
                    break;
                }
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // Define the number of picture data and embedded data lines
                iError = MrvSmiaMipiSetFrameLines(tIsiSensorSmiaInfo.ulSofEmbDataLines, tIsiSensorSmiaInfo.ulNumLines);
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // configure the embedded data without capturing any embedded data
                {
                    static tsMrvWindow tsEmbDataWindow1;                              // :( :( :( :(
                    static tsMrvWindow tsEmbDataWindow2;
                    tsEmbDataWindow1.usVOffs = 0;
                    tsEmbDataWindow1.usHOffs = 0;
                    tsEmbDataWindow1.usHSize = 0;
                    tsEmbDataWindow1.usVSize = 0;
                    tsEmbDataWindow2.usVOffs = 0;
                    tsEmbDataWindow2.usHOffs = 0;
                    tsEmbDataWindow2.usHSize = 0;
                    tsEmbDataWindow2.usVSize = 0;
                    iError = MrvSmiaMipiSetEmbDataStorage(&tsEmbDataWindow1, &tsEmbDataWindow2);
                }
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }

                // update the configuration --> obsolete

                // activate MIPI module
                iError = MrvSmiaMipiActivateModule(TRUE);
                if (iError != RET_SUCCESS)
                {
                    return iError;
                }
            }
                break;
        default:
            DBG_OUT((DERR, "MrvIspSetInputAquisition: ulMipiMode not supported\n"));
            return RET_NOTSUPP;
        } // switch (ptIsiCfg->ulMipiMode)
        break;
#endif  // #if (MARVIN_FEATURE_SMIA_MIPI_EMP)
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulMode not supported\n"));
        return RET_NOTSUPP;
    } // switch (ptIsiCfg->ulMode)

#if (MARVIN_FEATURE_CAMBUSWIDTH != MARVIN_FEATURE_8BITS) //greater marvin1
    switch (ptIsiCfg->ulBusWidth)
    {
  #if (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS) //marvin5
        //ISP_ACQ_PROP
        //Address: ISP_BASE1 + 0x0004
        //14:12   INPUT_SELECTION
    case ISI_BUSWIDTH_12BIT:
        //000- 12Bit external Interface
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_12EXT);
        break;
    case ISI_BUSWIDTH_10BIT_ZZ:
        //001- 10Bit Interface, append 2 zeroes as LSBs
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_10ZERO);
        break;
    case ISI_BUSWIDTH_10BIT_EX:
        //010- 10Bit Interface, append 2 MSBs as LSBs
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_10MSB);
        break;
    case ISI_BUSWIDTH_8BIT_ZZ:
        //011- 8Bit Interface, append 4 zeroes as LSBs
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_8ZERO);
        break;
    case ISI_BUSWIDTH_8BIT_EX:
        //100- 8Bit Interface, append 4 MSBs as LSBs
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_8MSB);
        break;
        //101...111 reserved
  #else // #if (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS) //marvin3
    case ISI_BUSWIDTH_10BIT:
        //set 10Bit Interface
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_10EXT);
        break;
    case ISI_BUSWIDTH_8BIT_ZZ:
        //set 8-bit camera with two zero's as LSB
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_8ZERO);
        break;
    case ISI_BUSWIDTH_8BIT_EX:
        //set 8Bit Interface append 2 MSB as LSBs
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_INPUT_SELECTION, MRV_ISP_INPUT_SELECTION_8MSB);
        break;
  #endif // #if (MARVIN_FEATURE_CAMBUSWIDTH == MARVIN_FEATURE_12BITS)
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulBusWidth not supported\n"));
        return RET_NOTSUPP;
    }
#endif  // #if (  MARVIN_FEATURE_CAMBUSWIDTH  != MARVIN_FEATURE_8BITS )

#if (MARVIN_FEATURE_INPUT_AQUISITION == MARVIN_FEATURE_IAQU_V1)
    switch (ptIsiCfg->ulFieldInv)
    {
    case ISI_FIELDINV_SWAP:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_FIELD_INV, ON);
        break;
    case ISI_FIELDINV_NOSWAP:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_FIELD_INV, OFF);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulFieldInv not supported\n"));
        return RET_NOTSUPP;
    }
#endif // #if (MARVIN_FEATURE_INPUT_AQUISITION == MARVIN_FEATURE_IAQU_V1)

    switch (ptIsiCfg->ulFieldSel)
    {
    case ISI_FIELDSEL_ODD:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_FIELD_SELECTION, MRV_ISP_FIELD_SELECTION_ODD);
        break;
    case ISI_FIELDSEL_EVEN:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_FIELD_SELECTION, MRV_ISP_FIELD_SELECTION_EVEN);
        break;
    case ISI_FIELDSEL_BOTH:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_FIELD_SELECTION, MRV_ISP_FIELD_SELECTION_BOTH);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulFieldSel not supported\n"));
        return RET_NOTSUPP;
    }

    switch (ptIsiCfg->ulYCSeq)
    {
    case ISI_YCSEQ_CRYCBY:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_CCIR_SEQ, MRV_ISP_CCIR_SEQ_CRYCBY);
        break;
    case ISI_YCSEQ_CBYCRY:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_CCIR_SEQ, MRV_ISP_CCIR_SEQ_CBYCRY);
        break;
    case ISI_YCSEQ_YCRYCB:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_CCIR_SEQ, MRV_ISP_CCIR_SEQ_YCRYCB);
        break;
    case ISI_YCSEQ_YCBYCR:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_CCIR_SEQ, MRV_ISP_CCIR_SEQ_YCBYCR);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulYCSeq not supported\n"));
        return RET_NOTSUPP;
    }

    switch (ptIsiCfg->ulConv422)
    {
    case ISI_CONV422_INTER:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_CONV_422, MRV_ISP_CONV_422_INTER);
        break;
    case ISI_CONV422_NOCOSITED:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_CONV_422, MRV_ISP_CONV_422_NONCO);
        break;
    case ISI_CONV422_COSITED:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_CONV_422, MRV_ISP_CONV_422_CO);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulConv422 not supported\n"));
        return RET_NOTSUPP;
    }

    switch (ptIsiCfg->ulBPat)
    {
    case ISI_BPAT_BGBGGRGR:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_BAYER_PAT, MRV_ISP_BAYER_PAT_BG);
        break;
    case ISI_BPAT_GBGBRGRG:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_BAYER_PAT, MRV_ISP_BAYER_PAT_GB);
        break;
    case ISI_BPAT_GRGRBGBG:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_BAYER_PAT, MRV_ISP_BAYER_PAT_GR);
        break;
    case ISI_BPAT_RGRGGBGB:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_BAYER_PAT, MRV_ISP_BAYER_PAT_RG);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulBPat not supported\n"));
        return RET_NOTSUPP;
    }

    switch (ptIsiCfg->ulVPol)
    {
    case ISI_VPOL_POS:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_VSYNC_POL, 1);
        break;
    case ISI_VPOL_NEG:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_VSYNC_POL, 0);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulVPol not supported\n"));
        return RET_NOTSUPP;
    }

    switch (ptIsiCfg->ulHPol)
    {
        //The trigger edge differs for vsync_pol and hsync_pol.
        //vsync_pol = 1 triggers on positive edge whereas
        //hsync_pol = 1 triggers on negative edge and vice versa
    case ISI_HPOL_SYNCPOS:
        //trigger on negative edge
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_HSYNC_POL, 1);
        break;
    case ISI_HPOL_SYNCNEG:
        //trigger on positive edge
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_HSYNC_POL, 0);
        break;
    case ISI_HPOL_REFPOS:
        //trigger on positive edge
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_HSYNC_POL, 0);
        break;
    case ISI_HPOL_REFNEG:
        //trigger on negative edge
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_HSYNC_POL, 1);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulHPol not supported\n"));
        return RET_NOTSUPP;
    }

    switch (ptIsiCfg->ulEdge)
    {
    case ISI_EDGE_RISING:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_SAMPLE_EDGE, 1);
        break;
    case ISI_EDGE_FALLING:
        REG_SET_SLICE(isp_acq_prop, MRV_ISP_SAMPLE_EDGE, 0);
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulEdge not supported\n"));
        return RET_NOTSUPP;
    }

    // now write values to registers
    REG_WRITE(ptMrvReg->isp_ctrl, isp_ctrl);
    REG_WRITE(ptMrvReg->isp_acq_prop, isp_acq_prop);

    //number of additional black lines at frame start
    switch (ptIsiCfg->ulBls)
    {
    case ISI_BLS_OFF:
        ucBlackLines = 0;
        break;
    case ISI_BLS_TWO_LINES:
        ucBlackLines = 2;
        break;
    case ISI_BLS_FOUR_LINES:
        ucBlackLines = 4;
        break;
    case ISI_BLS_SIX_LINES:
        ucBlackLines = 6;
        break;
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: black lines not supported\n"));
        return RET_NOTSUPP;
    }

    //The registers h_size and and h_offs hold the number of bytes to sample
    //(not the number of pixels). Dependend on the mode (e.g. YCbCr or RGB)
    //the number of pixels is calculated.
    // Note: offsets default to 0
    REG_SET_SLICE(ptMrvReg->isp_acq_h_offs, MRV_ISP_ACQ_H_OFFS, 0 * ucSampleFactor);
    REG_SET_SLICE(ptMrvReg->isp_acq_v_offs, MRV_ISP_ACQ_V_OFFS, 0);
    switch (ptIsiCfg->ulRes)
    {
#ifdef _DEBUG
    case ISI_RES_TEST:     // depends on further defines (TEST_SIZE_H and TEST_SIZE_V)
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, TEST_SIZE_H  * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, TEST_SIZE_V  + ucBlackLines);
        break;
#endif // #ifdef _DEBUG
    case ISI_RES_QQCIF:    //88x72
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QQCIF_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QQCIF_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QQVGA:    //160x120
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QQVGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QQVGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QCIF:     //176x144
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QCIF_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QCIF_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QVGA:     //320x240
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QVGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QVGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_CIF:      //352x288
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, CIF_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, CIF_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_VGA:      //640x480
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, VGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, VGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_SVGA:     //800x600
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, SVGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, SVGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_XGA:      //1024x768
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, XGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, XGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_XGA_PLUS: //1280x960
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, XGA_PLUS_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, XGA_PLUS_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_SXGA:     //1280x1024
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, SXGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, SXGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_UXGA:     //1600x1200
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QSVGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QSVGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QXGA:     //2048x1536
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QXGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QXGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_XGA_1280_720: //1280x720 WF
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, XGA_1280_720_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, XGA_1280_720_SIZE_V + ucBlackLines);
        break; 
    case ISI_RES_QSXGA_2592_1944: //2592x1944
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QSXGA_2592_1944_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QSXGA_2592_1944_SIZE_V + ucBlackLines);
        break;        
    case ISI_RES_QSXGA:     //2586x2048
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QSXGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QSXGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QSXGA_PLUS:     //2600x2048
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QSXGA_PLUS_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QSXGA_PLUS_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QSXGA_PLUS2:     //2600x1950
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QSXGA_PLUS2_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QSXGA_PLUS2_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QSXGA_PLUS3:    //2686x2048,  5.30M
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QSXGA_PLUS3_SIZE_V * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QSXGA_PLUS3_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_WQSXGA:         //3200x2048,  6.56M
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, WQSXGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, WQSXGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_QUXGA:         //3200x2400,  7.68M
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, QUXGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, QUXGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_WQUXGA:         //3840x2400,  9.22M
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, WQUXGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, WQUXGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_HXGA:           //4096x3072, 12.59M
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, HXGA_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, HXGA_SIZE_V + ucBlackLines);
        break;
    case ISI_RES_YUV_HMAX:     //4080x1024
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, YUV_HMAX_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, YUV_HMAX_SIZE_V);
        break;
    case ISI_RES_YUV_VMAX:     //1024x4080
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, YUV_VMAX_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, YUV_VMAX_SIZE_V);
        break;
    case ISI_RES_RAWMAX:     //4096x2048
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, RAWMAX_SIZE_H);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, RAWMAX_SIZE_V);
        break;
    case ISI_RES_BP1:       //352x240
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, BP1_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, BP1_SIZE_V);
        break;
    case ISI_RES_L_AFM:     //720x480
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, L_AFM_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, L_AFM_SIZE_V);
        break;
    case ISI_RES_M_AFM:     //128x96
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, M_AFM_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, M_AFM_SIZE_V);
        break;
    case ISI_RES_S_AFM:     //64x32
        REG_SET_SLICE(ptMrvReg->isp_acq_h_size, MRV_ISP_ACQ_H_SIZE, S_AFM_SIZE_H * ucSampleFactor);
        REG_SET_SLICE(ptMrvReg->isp_acq_v_size, MRV_ISP_ACQ_V_SIZE, S_AFM_SIZE_V);
        break;
// [hf]: Was ist mit QSXGA, QUXGA ?
//       Warum wird teilweise ucBlackLines nicht berücksichtigt?
    default:
        DBG_OUT((DERR, "MrvIspSetInputAquisition: ulRes not supoported\n"));
        return RET_NOTSUPP;
    }

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetOutputFormatter \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  output window offset and size
 *               enum for immediate, frame synchronous, or later (external) update \n
 *  \DESCRIPTION sets output window \n
 */
/*****************************************************************************/
void MrvIspSetOutputFormatter(const tsMrvWindow *ptWindow, teMrvConfUpdateTime eUpdateTime)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ptWindow)
    {
        //set output window
#if (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT)
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
    #error "no output formatter and no image stabilization is not allowed!"
  #else // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
        REG_SET_SLICE(ptMrvReg->isp_is_h_offs, MRV_IS_IS_H_OFFS, ptWindow->usHOffs);
        REG_SET_SLICE(ptMrvReg->isp_is_v_offs, MRV_IS_IS_V_OFFS, ptWindow->usVOffs);
        REG_SET_SLICE(ptMrvReg->isp_is_h_size, MRV_IS_IS_H_SIZE, ptWindow->usHSize);
        REG_SET_SLICE(ptMrvReg->isp_is_v_size, MRV_IS_IS_V_SIZE, ptWindow->usVSize);
  #endif // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
#else // #if (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT)
        REG_SET_SLICE(ptMrvReg->isp_out_h_offs, MRV_IS_IS_H_OFFS, ptWindow->usHOffs);
        REG_SET_SLICE(ptMrvReg->isp_out_v_offs, MRV_IS_IS_V_OFFS, ptWindow->usVOffs);
        REG_SET_SLICE(ptMrvReg->isp_out_h_size, MRV_IS_IS_H_SIZE, ptWindow->usHSize);
        REG_SET_SLICE(ptMrvReg->isp_out_v_size, MRV_IS_IS_V_SIZE, ptWindow->usVSize);
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
        // nothing more to do
  #else // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
        // use the same settings as output formatter without any offset
        REG_SET_SLICE(ptMrvReg->isp_is_h_offs, MRV_IS_IS_H_OFFS, 0);
        REG_SET_SLICE(ptMrvReg->isp_is_v_offs, MRV_IS_IS_V_OFFS, 0);
        REG_SET_SLICE(ptMrvReg->isp_is_h_size, MRV_IS_IS_H_SIZE, ptWindow->usHSize);
        REG_SET_SLICE(ptMrvReg->isp_is_v_size, MRV_IS_IS_V_SIZE, ptWindow->usVSize);
  #endif // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
#endif // #if (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT)

                switch (eUpdateTime)
                {
                case eMrvCfgUpdateFrameSync:
            //frame synchronous update of shadow registers
            REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, ON);
                        break;
                case eMrvCfgUpdateImmediate:
            //immediate update of shadow registers
            REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_CFG_UPD, ON);
                        break;
                case eMrvCfgUpdateLater:
                        // no update from within this function
                        break;
                default:
                        break;
                }
    }
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetOutputFormatter \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  pointer to structure to receive output formatter settings \n
 *  \DESCRIPTION retrieves output window \n
 */
/*****************************************************************************/
void MrvIspGetOutputFormatter(tsMrvWindow *ptWindow)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ptWindow)
    {
        //get output window
#if (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT)
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
    #error "no output formatter and no image stabilization is not allowed!"
  #else // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
        ptWindow->usHOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_h_offs, MRV_IS_IS_H_OFFS);
        ptWindow->usVOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_v_offs, MRV_IS_IS_V_OFFS);
        ptWindow->usHSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_h_size, MRV_IS_IS_H_SIZE);
        ptWindow->usVSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_is_v_size, MRV_IS_IS_V_SIZE);
  #endif // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_EXIST_NOT)
#else // #if (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT)
        ptWindow->usHOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_out_h_offs, MRV_IS_IS_H_OFFS);
        ptWindow->usVOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_out_v_offs, MRV_IS_IS_V_OFFS);
        ptWindow->usHSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_out_h_size, MRV_IS_IS_H_SIZE);
        ptWindow->usVSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_out_v_size, MRV_IS_IS_V_SIZE);
#endif // #if (MARVIN_FEATURE_OUTPUT_FORMATTER == MARVIN_FEATURE_EXIST_NOT)
    }
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetDemosaic \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  demosaic mode \n
 *               demosaic threshold \n
 *
 *  \DESCRIPTION programs the given Bayer pattern demosaic parameters \n
 */
/*****************************************************************************/
void MrvIspSetDemosaic(teMrvDemosaicMode eDemosaicMode, UINT8 ucDemosaicTh)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_demosaic = REG_READ(ptMrvReg->isp_demosaic);

    // set demosaic mode
    switch (eDemosaicMode)
    {
    case eMrvDemosaicStandard:
        REG_SET_SLICE(isp_demosaic, MRV_ISP_DEMOSAIC_MODE, MRV_ISP_DEMOSAIC_MODE_STD);
        break;
    case eMrvDemosaicEnhanced:
        REG_SET_SLICE(isp_demosaic, MRV_ISP_DEMOSAIC_MODE, MRV_ISP_DEMOSAIC_MODE_ENH);
        break;
    default:
        ASSERT( FALSE );
    }

    //set demosaic threshold
    REG_SET_SLICE(isp_demosaic, MRV_ISP_DEMOSAIC_TH, ucDemosaicTh);

    REG_WRITE(ptMrvReg->isp_demosaic, isp_demosaic);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetWbMode \n
 *  \PARAMETERS  white balancing mode \n
 *  \RETURNVALUE RET_SUCCESS - settings have been applied \n
 *               RET_FAILURE - unknown mode \n
 *  \DESCRIPTION Sets the dedicated AWB block mode. \n
 */
/*****************************************************************************/
RESULT MrvIspSetWbMode(teMrvAwbMode tWbMode)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    switch (tWbMode)
    {
    case eMrvAwbCompletelyOff:
        // just to be sure that a regular value is set: manual WB, no measurements
        REG_SET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MODE, MRV_ISP_AWB_MODE_NOMEAS);
        // switch ABW block off
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_AWB_ENABLE, DISABLE);
        break;
    case eMrvAwbManMeas:
    case eMrvAwbAuto:
    case eMrvAwbManPushAuto:
    case eMrvAwbOnlyMeas:
        // manual white balance, measure YCbCr or RGB means
        REG_SET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MODE, MRV_ISP_AWB_MODE_MEAS);
        // switch ABW block on
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_AWB_ENABLE, ENABLE);
        break;
    case eMrvAwbManNoMeas:
        // manual WB, no measurements
        REG_SET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MODE, MRV_ISP_AWB_MODE_NOMEAS);
        // switch ABW block on
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_AWB_ENABLE, ENABLE);
        break;
    default:
        // just to be sure that a regular value is set: manual WB, no measurements
        REG_SET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MODE, MRV_ISP_AWB_MODE_NOMEAS);
        // switch ABW block off
        REG_SET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_AWB_ENABLE, DISABLE);

        DBG_OUT((DERR, "MrvIspSetWbMode: tMrvAwbMode not supported.\nAWB block has been switched off.\n"));
        return RET_FAILURE;
    }

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetWbMode \n
 *  \PARAMETERS  pointer to white balancing mode \n
 *  \RETURNVALUE RET_SUCCESS      - everything is fine \n
 *               RET_FAILURE      - unknown mode \n
 *               RET_NULL_POINTER - pointer is NULL \n
 *  \DESCRIPTION Gets the current AWB block mode. \n
 *               Note that this is not truly the inverse of MrvIspSetWbMode,
 *               information may be lost by setting a mode and reading it
 *               back (eMrvAwbAuto and eMrvAwbManPushAuto is never returned). \n
 */
/*****************************************************************************/
RESULT MrvIspGetWbMode(teMrvAwbMode *ptWbMode)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ptWbMode == NULL)
    {
        return RET_NULL_POINTER;
    }

    if (REG_GET_SLICE(ptMrvReg->isp_ctrl, MRV_ISP_ISP_AWB_ENABLE) == DISABLE)
    {
        *ptWbMode = eMrvAwbCompletelyOff;
    }
    else
    {
        switch (REG_GET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MODE))
        {
        case MRV_ISP_AWB_MODE_MEAS:
            *ptWbMode = eMrvAwbManMeas;
            break;
        case MRV_ISP_AWB_MODE_NOMEAS:
            *ptWbMode = eMrvAwbManNoMeas;
            break;
        default:
            *ptWbMode = eMrvAwbCompletelyOff;

            DBG_OUT((DERR, "MrvIspGetWbMode: Unknown AWB mode.\n"));
            return RET_FAILURE;
        }
    }
    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetWbMeasConfig \n
 *  \PARAMETERS  pointer to white balancing measurement configuration
 *                 struct \n
 *  \RETURNVALUE RET_SUCCESS      - settings have been applied \n
 *               RET_NULL_POINTER - pointer is NULL \n
 *  \DESCRIPTION Sets the dedicated AWB measurement configuration.
 *               MAX_Y compare is disabled with ucMaxY = 0. \n
 */
/*****************************************************************************/
RESULT MrvIspSetWbMeasConfig(const tsMrvWbMeasConfig *ptWbMeasConfig)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_awb_thresh = REG_READ(ptMrvReg->isp_awb_thresh);

    if (ptWbMeasConfig == NULL)
    {
        return RET_NULL_POINTER;
    }

    // parameter range check
    if(    ( ptWbMeasConfig->tMrvAWBWindow.usHOffs > MRV_ISP_AWB_H_OFFS_MAX )
        || ( ptWbMeasConfig->tMrvAWBWindow.usVOffs > MRV_ISP_AWB_V_OFFS_MAX )
        || ( ptWbMeasConfig->tMrvAWBWindow.usHSize > MRV_ISP_AWB_H_SIZE_MAX )
        || ( ptWbMeasConfig->tMrvAWBWindow.usVSize > MRV_ISP_AWB_V_SIZE_MAX )
        || ( ptWbMeasConfig->tMrvAWBWindow.usHOffs > ptWbMeasConfig->tMrvAWBWindow.usHSize )
        || ( ptWbMeasConfig->tMrvAWBWindow.usVOffs > ptWbMeasConfig->tMrvAWBWindow.usVSize )
      )
    {
        return RET_OUTOFRANGE;
    }

    // measurement window
    REG_SET_SLICE(ptMrvReg->isp_awb_h_offs, MRV_ISP_AWB_H_OFFS, (UINT32)(ptWbMeasConfig->tMrvAWBWindow.usHOffs));
    REG_SET_SLICE(ptMrvReg->isp_awb_v_offs, MRV_ISP_AWB_V_OFFS, (UINT32)(ptWbMeasConfig->tMrvAWBWindow.usVOffs));
    REG_SET_SLICE(ptMrvReg->isp_awb_h_size, MRV_ISP_AWB_H_SIZE, (UINT32)(ptWbMeasConfig->tMrvAWBWindow.usHSize));
    REG_SET_SLICE(ptMrvReg->isp_awb_v_size, MRV_ISP_AWB_V_SIZE, (UINT32)(ptWbMeasConfig->tMrvAWBWindow.usVSize));

    // enable/disable MAX_Y compare
    // the bit is ignored in RGB measurement mode
    if( ptWbMeasConfig->ucMaxY == 0 )
    {
        REG_SET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MAX_EN, DISABLE);
    }
    else
    {
        REG_SET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MAX_EN, ENABLE);
    }

    // measurement thresholds
    REG_SET_SLICE(isp_awb_thresh, MRV_ISP_AWB_MAX_Y,        (UINT32)(ptWbMeasConfig->ucMaxY));    // ignored in RGB mode
    REG_SET_SLICE(isp_awb_thresh, MRV_ISP_AWB_MIN_Y__MAX_G, (UINT32)(ptWbMeasConfig->ucMinY_MaxG));
    REG_SET_SLICE(isp_awb_thresh, MRV_ISP_AWB_MAX_CSUM,     (UINT32)(ptWbMeasConfig->ucMaxCSum)); // ignored in RGB mode
    REG_SET_SLICE(isp_awb_thresh, MRV_ISP_AWB_MIN_C,        (UINT32)(ptWbMeasConfig->ucMinC));    // ignored in RGB mode
    REG_WRITE(ptMrvReg->isp_awb_thresh, isp_awb_thresh);
    
    REG_SET_SLICE(ptMrvReg->isp_awb_ref, MRV_ISP_AWB_REF_CR__MAX_R, (UINT32)(ptWbMeasConfig->ucRefCr_MaxR));
    REG_SET_SLICE(ptMrvReg->isp_awb_ref, MRV_ISP_AWB_REF_CB__MAX_B, (UINT32)(ptWbMeasConfig->ucRefCb_MaxB));

    // amount of measurement frames
    REG_SET_SLICE(ptMrvReg->isp_awb_frames, MRV_ISP_AWB_FRAMES, (UINT32)(ptWbMeasConfig->ucFrames));

#if (MARVIN_FEATURE_WB > MARVIN_FEATURE_WB_V4)
    // set measurement mode
    REG_SET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MEAS_MODE, (UINT32)(ptWbMeasConfig->ucMeasMode));
#endif

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetWbMeasConfig \n
 *  \PARAMETERS  pointer to white balancing measurement configuration struct \n
 *  \RETURNVALUE RET_SUCCESS      - everything is fine \n
 *               RET_NULL_POINTER - pointer is NULL \n
 *  \DESCRIPTION Gets the current AWB measurement configuration. \n
 */
/*****************************************************************************/
RESULT MrvIspGetWbMeasConfig(tsMrvWbMeasConfig *ptWbMeasConfig)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ptWbMeasConfig == NULL)
    {
        return RET_NULL_POINTER;
    }

    // measurement window
    ptWbMeasConfig->tMrvAWBWindow.usHOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_awb_h_offs, MRV_ISP_AWB_H_OFFS);
    ptWbMeasConfig->tMrvAWBWindow.usVOffs = (UINT16)REG_GET_SLICE(ptMrvReg->isp_awb_v_offs, MRV_ISP_AWB_V_OFFS);
    ptWbMeasConfig->tMrvAWBWindow.usHSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_awb_h_size, MRV_ISP_AWB_H_SIZE);
    ptWbMeasConfig->tMrvAWBWindow.usVSize = (UINT16)REG_GET_SLICE(ptMrvReg->isp_awb_v_size, MRV_ISP_AWB_V_SIZE);

    // measurement thresholds
    ptWbMeasConfig->ucMinC      = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_thresh, MRV_ISP_AWB_MIN_C);
    ptWbMeasConfig->ucMaxCSum   = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_thresh, MRV_ISP_AWB_MAX_CSUM);
    ptWbMeasConfig->ucMinY_MaxG = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_thresh, MRV_ISP_AWB_MIN_Y__MAX_G);
    ptWbMeasConfig->ucMaxY      = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_thresh, MRV_ISP_AWB_MAX_Y);
    
    ptWbMeasConfig->ucRefCb_MaxB = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_ref, MRV_ISP_AWB_REF_CB__MAX_B);
    ptWbMeasConfig->ucRefCr_MaxR = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_ref, MRV_ISP_AWB_REF_CR__MAX_R);

    // amount of measurement frames
    ptWbMeasConfig->ucFrames = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_frames, MRV_ISP_AWB_FRAMES);

    // overwrite ucMaxY if the feature is disabled
    if (REG_GET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MAX_EN) == DISABLE)
    {
        ptWbMeasConfig->ucMaxY = 0;
    }

#if (MARVIN_FEATURE_WB > MARVIN_FEATURE_WB_V4)
    // get measurement mode
    ptWbMeasConfig->ucMeasMode = REG_GET_SLICE(ptMrvReg->isp_awb_prop, MRV_ISP_AWB_MEAS_MODE);
#else
    ptWbMeasConfig->ucMeasMode = 0;
#endif

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetWbMeas \n
 *  \RETURNVALUE RET_NULL_POINTER - called with NULL pointer \n
 *  \PARAMETERS  pointer to measurement values struct \n
 *  \DESCRIPTION Returns the Marvin white balancing measurement values. \n
 */
/*****************************************************************************/
RESULT MrvIspGetWbMeas(tsIsiAwbMean *ptMrvAwbMean)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (ptMrvAwbMean == NULL) return RET_NULL_POINTER;

    ptMrvAwbMean->ulWhite     = REG_GET_SLICE(ptMrvReg->isp_awb_white_cnt, MRV_ISP_AWB_WHITE_CNT);
    ptMrvAwbMean->ucMeanY__G  = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_mean, MRV_ISP_AWB_MEAN_Y__G);
    ptMrvAwbMean->ucMeanCb__B = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_mean, MRV_ISP_AWB_MEAN_CB__B);
    ptMrvAwbMean->ucMeanCr__R = (UINT8)REG_GET_SLICE(ptMrvReg->isp_awb_mean, MRV_ISP_AWB_MEAN_CR__R);

    return RET_SUCCESS;
}

#ifdef FIXPOINT
/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetWbGains \n
 *  \PARAMETERS  pointer to gain values struct \n
 *  \RETURNVALUE RET_SUCCESS      - everything is fine \n
 *               RET_FAILURE      - the AWB block is in HW automatic mode \n
 *               RET_NULL_POINTER - pointer is NULL \n
 *  \DESCRIPTION Sets the desired gain values if the AWB block is not
 *               in the HW automatic mode. \n
 */
/*****************************************************************************/
RESULT MrvIspSetWbGains(const tsMrvWbGains *ptWbGains)
{
#if (MARVIN_FEATURE_WB != MARVIN_FEATURE_EXIST_NOT)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
#endif

    if (ptWbGains)
#if ((MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V4) || (MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V5))
    {
        UINT32 ulGainVal;

        ulGainVal = UtlFloatToFix_U0208(ptWbGains->fRed);
        REG_SET_SLICE(ptMrvReg->isp_awb_gain_rb, MRV_ISP_AWB_GAIN_R, ulGainVal);

        ulGainVal = UtlFloatToFix_U0208(ptWbGains->fBlue);
        REG_SET_SLICE(ptMrvReg->isp_awb_gain_rb, MRV_ISP_AWB_GAIN_B, ulGainVal);

        ulGainVal = UtlFloatToFix_U0208(ptWbGains->fGreenR);
        REG_SET_SLICE(ptMrvReg->isp_awb_gain_g, MRV_ISP_AWB_GAIN_GR, ulGainVal);

        ulGainVal = UtlFloatToFix_U0208(ptWbGains->fGreenB);
        REG_SET_SLICE(ptMrvReg->isp_awb_gain_g, MRV_ISP_AWB_GAIN_GB, ulGainVal);

        return RET_SUCCESS;
    }
#elif (MARVIN_FEATURE_WB == MARVIN_FEATURE_EXIST_NOT)
    return RET_NOTSUPP;
#else
    #error "unknown value for MARVIN_FEATURE_WB"
#endif

    return RET_NULL_POINTER;
}//MrvIspSetWbGains


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetWbGains \n
 *  \PARAMETERS  pointer to gain values struct \n
 *  \RETURNVALUE RET_SUCCESS      - everything is fine \n
 *               RET_NULL_POINTER - pointer is NULL \n
 *  \DESCRIPTION Gets the current gain values. \n
 */
/*****************************************************************************/
RESULT MrvIspGetWbGains(tsMrvWbGains *ptWbGains)
{
#if (MARVIN_FEATURE_WB != MARVIN_FEATURE_EXIST_NOT)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
#endif 

    if (ptWbGains)
#if ((MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V4) || (MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V5))
    {
        ptWbGains->fBlue   = UtlFixToFloat_U0208(REG_GET_SLICE(ptMrvReg->isp_awb_gain_rb, MRV_ISP_AWB_GAIN_B));
        ptWbGains->fRed    = UtlFixToFloat_U0208(REG_GET_SLICE(ptMrvReg->isp_awb_gain_rb, MRV_ISP_AWB_GAIN_R));
        ptWbGains->fGreenB = UtlFixToFloat_U0208(REG_GET_SLICE(ptMrvReg->isp_awb_gain_g, MRV_ISP_AWB_GAIN_GB));
        ptWbGains->fGreenR = UtlFixToFloat_U0208(REG_GET_SLICE(ptMrvReg->isp_awb_gain_g, MRV_ISP_AWB_GAIN_GR));

        return RET_SUCCESS;
    }
#elif (MARVIN_FEATURE_WB == MARVIN_FEATURE_EXIST_NOT) 
    return RET_NOTSUPP;
#else
    #error "unknown value for MARVIN_FEATURE_WB"
#endif

    return RET_NULL_POINTER;
}//MrvIspGetWbGains

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetColorConversion \n
 *  \RETURNVALUE RET_NOTSUPP      - feature does not exist \n
 *               RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER - ptColConvCoeff == NULL \n
 *  \PARAMETERS  pointer to array holding color conversion coefficients \n
 *
 *  \DESCRIPTION Sets the MARVIN color conversion registers, if possible. \n
 */
/*****************************************************************************/
RESULT MrvIspSetColorConversion
(
    const tsIsi3x3FloatMatrix *ptColConvCoeff
)
{
#if (MARVIN_FEATURE_ISP_CSM == MARVIN_FEATURE_EXIST)
    if (ptColConvCoeff != NULL)
    {
        UINT32 uli;
        INT32 lCoeff[9];
        volatile tsMrvRegister *ptMrvReg   = (tsMrvRegister*)MEM_MRV_REG_BASE;

        for (uli = 0; uli < 9; uli++)
        {
            lCoeff[uli] = UtlFloatToFix_S0207(ptColConvCoeff->fCoeff[uli]);
        }

        // set values
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_0, MRV_ISP_CC_COEFF_0, lCoeff[0]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_1, MRV_ISP_CC_COEFF_1, lCoeff[1]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_2, MRV_ISP_CC_COEFF_2, lCoeff[2]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_3, MRV_ISP_CC_COEFF_3, lCoeff[3]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_4, MRV_ISP_CC_COEFF_4, lCoeff[4]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_5, MRV_ISP_CC_COEFF_5, lCoeff[5]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_6, MRV_ISP_CC_COEFF_6, lCoeff[6]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_7, MRV_ISP_CC_COEFF_7, lCoeff[7]);
        REG_SET_SLICE(ptMrvReg->isp_cc_coeff_8, MRV_ISP_CC_COEFF_8, lCoeff[8]);

        return RET_SUCCESS;
    }
    else
    {
        return RET_NULL_POINTER;
    }
#elif (MARVIN_FEATURE_ISP_CSM == MARVIN_FEATURE_EXIST_NOT) 
    return RET_NOTSUPP;
#else
    #error "unknown value for MARVIN_FEATURE_ISP_CSM"
#endif
}//MrvIspSetColorConversion

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetColorConversion \n
 *  \RETURNVALUE RET_NOTSUPP      - feature does not exist \n
 *               RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER - ptColConvCoeff == NULL \n
 *  \PARAMETERS  pointer to array holding color conversion coefficients \n
 *
 *  \DESCRIPTION Gets the MARVIN color conversion registers, if possible. \n
 */
/*****************************************************************************/
RESULT MrvIspGetColorConversion
(
    tsIsi3x3FloatMatrix *ptColConvCoeff
)
{
#if (MARVIN_FEATURE_ISP_CSM == MARVIN_FEATURE_EXIST)
    if (ptColConvCoeff)
    {
        UINT32 uli;
        INT32 lCoeff[9];
        volatile tsMrvRegister *ptMrvReg   = (tsMrvRegister*)MEM_MRV_REG_BASE;

        // get values
        lCoeff[0] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_0, MRV_ISP_CC_COEFF_0);
        lCoeff[1] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_1, MRV_ISP_CC_COEFF_1);
        lCoeff[2] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_2, MRV_ISP_CC_COEFF_2);
        lCoeff[3] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_3, MRV_ISP_CC_COEFF_3);
        lCoeff[4] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_4, MRV_ISP_CC_COEFF_4);
        lCoeff[5] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_5, MRV_ISP_CC_COEFF_5);
        lCoeff[6] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_6, MRV_ISP_CC_COEFF_6);
        lCoeff[7] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_7, MRV_ISP_CC_COEFF_7);
        lCoeff[8] = REG_GET_SLICE(ptMrvReg->isp_cc_coeff_8, MRV_ISP_CC_COEFF_8);
     
        for (uli = 0; uli < 9; uli++)
        {
           ptColConvCoeff->fCoeff[uli] = UtlFixToFloat_S0207( lCoeff[uli]);
        }
        return RET_SUCCESS;
    }
    else
    {
        return RET_NULL_POINTER;
    }
#elif (MARVIN_FEATURE_ISP_CSM == MARVIN_FEATURE_EXIST_NOT) 
    return RET_NOTSUPP;
#else
    #error "unknown value for MARVIN_FEATURE_ISP_CSM"
#endif
}//MrvIspGetColorConversion

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetCrossTalk \n
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER - ptCrossTalkCoeff == NULL or
 *                                  ptCrossTalkOffset == NULL \n
 *               RET_NOTSUPP      - there is no MARVIN cross-talk module or
 *                                  offsets to the cross-talk matrix are not supported\n
 *  \PARAMETERS  pointer to array holding CrossTalk coefficients \n
 *               pointer to array holding CrossTalk offsets \n
 *
 *  \DESCRIPTION Sets the MARVIN CrossTalk registers.\n
 */
/*****************************************************************************/
RESULT MrvIspSetCrossTalk
(
    const tsIsi3x3FloatMatrix   *ptCrossTalkCoeff,
    const tsIsiXTalkFloatOffset *ptCrossTalkOffset
)
{
#if (MARVIN_FEATURE_XTALK != MARVIN_FEATURE_EXIST_NOT)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    if (ptCrossTalkCoeff != NULL)
    {
        UINT32  lCoeff[9];
        UINT32 uli = 0;

        for (uli = 0; uli < MRV_ISP_CT_COEFF_ARR_SIZE; uli++)
        {
#if (MARVIN_FEATURE_XTALK == MARVIN_FEATURE_XTALK_10BITS)
             lCoeff[uli] = UtlFloatToFix_S0307( ptCrossTalkCoeff->fCoeff[uli] );
#elif (MARVIN_FEATURE_XTALK == MARVIN_FEATURE_XTALK_11BITS)
             lCoeff[uli] = UtlFloatToFix_S0407( ptCrossTalkCoeff->fCoeff[uli] );
#else
#error "unknown value for MARVIN_FEATURE_XTALK"
#endif
            REG_SET_SLICE(ptMrvReg->isp_ct_coeff[uli], MRV_ISP_CT_COEFF, lCoeff[uli]);
        }
    }
    else
    {
        return RET_NULL_POINTER;
    }
#if (MARVIN_FEATURE_XTALK_OFFSET != MARVIN_FEATURE_EXIST_NOT)
    if (ptCrossTalkOffset)
    {
        UINT32  lRegVal;

        lRegVal = UtlFloatToFix_S1200( ptCrossTalkOffset->fCtOffsetRed );
        REG_SET_SLICE( ptMrvReg->isp_ct_offset_r, MRV_ISP_CT_OFFSET_R, lRegVal );

        lRegVal = UtlFloatToFix_S1200( ptCrossTalkOffset->fCtOffsetGreen );
        REG_SET_SLICE( ptMrvReg->isp_ct_offset_g, MRV_ISP_CT_OFFSET_G, lRegVal );
                
        lRegVal = UtlFloatToFix_S1200( ptCrossTalkOffset->fCtOffsetBlue );
        REG_SET_SLICE( ptMrvReg->isp_ct_offset_b, MRV_ISP_CT_OFFSET_B, lRegVal );
    }
    else
    {
        return RET_NULL_POINTER;
    }
#endif // MARVIN_FEATURE_XTALK_OFFSET
 
    return RET_SUCCESS;
#else   // #if( MARVIN_FEATURE_XTALK != MARVIN_FEATURE_EXIST_NOT )
    return RET_NOTSUPP;
#endif  // #if( MARVIN_FEATURE_XTALK != MARVIN_FEATURE_EXIST_NOT )
}//MrvIspSetCrossTalk

/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetCrossTalk \n
 *  \RETURNVALUE RET_SUCCESS      - everything ok \n
 *               RET_NULL_POINTER - ptCrossTalkCoeff == NULL or
 *                                  ptCrossTalkOffset == NULL \n
 *               RET_NOTSUPP      - there is no MARVIN cross-talk module or
 *                                  offsets to the cross-talk matrix are not supported\n
 *  \PARAMETERS  pointer to array holding CrossTalk coefficients \n
 *               pointer to array holding CrossTalk offsets \n
 *
 *  \DESCRIPTION Gets the MARVIN CrossTalk registers. \n
 */
/*****************************************************************************/
RESULT MrvIspGetCrossTalk
(
    tsIsi3x3FloatMatrix   *ptCrossTalkCoeff,
    tsIsiXTalkFloatOffset *ptCrossTalkOffset
)
{
#if (MARVIN_FEATURE_XTALK != MARVIN_FEATURE_EXIST_NOT)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    
    if (ptCrossTalkCoeff != NULL)
    {
        UINT32  lCoeff[9];
        UINT32 uli = 0;

        for (uli = 0; uli < MRV_ISP_CT_COEFF_ARR_SIZE; uli++)
        {
            lCoeff[uli] = REG_GET_SLICE(ptMrvReg->isp_ct_coeff[uli], MRV_ISP_CT_COEFF );
#if (MARVIN_FEATURE_XTALK == MARVIN_FEATURE_XTALK_10BITS)
              ptCrossTalkCoeff->fCoeff[uli] = UtlFixToFloat_S0307( lCoeff[uli] );
#elif (MARVIN_FEATURE_XTALK == MARVIN_FEATURE_XTALK_11BITS)
              ptCrossTalkCoeff->fCoeff[uli] = UtlFixToFloat_S0407( lCoeff[uli] );
#else
#error "unknown value for MARVIN_FEATURE_XTALK"
#endif
        }
    }
    else
    {
        return RET_NULL_POINTER;
    }
#if (MARVIN_FEATURE_XTALK_OFFSET != MARVIN_FEATURE_EXIST_NOT)
    if (ptCrossTalkOffset)
    {
        UINT32  lRegVal;

        lRegVal = REG_GET_SLICE( ptMrvReg->isp_ct_offset_r, MRV_ISP_CT_OFFSET_R );
        ptCrossTalkOffset->fCtOffsetRed = UtlFixToFloat_S1200( lRegVal );

        lRegVal = REG_GET_SLICE( ptMrvReg->isp_ct_offset_g, MRV_ISP_CT_OFFSET_G );
        ptCrossTalkOffset->fCtOffsetGreen = UtlFixToFloat_S1200( lRegVal );
                
        lRegVal = REG_GET_SLICE( ptMrvReg->isp_ct_offset_b, MRV_ISP_CT_OFFSET_B );
        ptCrossTalkOffset->fCtOffsetBlue = UtlFixToFloat_S1200( lRegVal );
    }
    else
    {
        return RET_NULL_POINTER;
    }
#endif // MARVIN_FEATURE_XTALK_OFFSET
 
    return RET_SUCCESS;
#else   // #if( MARVIN_FEATURE_XTALK != MARVIN_FEATURE_EXIST_NOT )
    return RET_NOTSUPP;
#endif  // #if( MARVIN_FEATURE_XTALK != MARVIN_FEATURE_EXIST_NOT )
}//MrvIspGetCrossTalk

#endif // #ifdef FIXPOINT

#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspAfmWnd2Regs (static) \n
 *  \RETURNVALUE RET_SUCCESS if all values are in range. \n
 *               RET_OUTOFRANGE otherwise \n
 *  \PARAMETERS  pWnd .. window to convert into register values \n
 *               pLT .. pointer to variable to receive value for
 *                      left-top register \n
 *               pRB .. pointer to variable to receive value for
 *                      right-bottom register \n
 *  \DESCRIPTION calculates left-top and right-bottom register values
 *               for a given AF measurement window \n
 */
/*****************************************************************************/
static RESULT MrvIspAfmWnd2Regs(const tsMrvWindow* pWnd, UINT32* pLT, UINT32* pRB)
{
    ASSERT((pWnd != NULL) && (pLT != NULL) && (pRB != NULL));

    if (pWnd->usHSize && pWnd->usVSize)
    {
        UINT32 ulLeft   = pWnd->usHOffs;
        UINT32 ulTop    = pWnd->usVOffs;
        UINT32 ulRight  = ulLeft + pWnd->usHSize - 1;
        UINT32 ulBottom = ulTop  + pWnd->usVSize - 1;

        // according to the spec, the upper left corner has some restrictions:
        if ((ulLeft   < MRV_AFM_A_H_L_MIN) || (ulLeft   > MRV_AFM_A_H_L_MAX) ||
            (ulTop    < MRV_AFM_A_V_T_MIN) || (ulTop    > MRV_AFM_A_V_T_MAX) ||
            (ulRight  < MRV_AFM_A_H_R_MIN) || (ulRight  > MRV_AFM_A_H_R_MAX) ||
            (ulBottom < MRV_AFM_A_V_B_MIN) || (ulBottom > MRV_AFM_A_V_B_MAX)   )
        {
            return RET_OUTOFRANGE;
        }

        // combine the values and return
        REG_SET_SLICE(*pLT, MRV_AFM_A_H_L, ulLeft);
        REG_SET_SLICE(*pLT, MRV_AFM_A_V_T, ulTop);
        REG_SET_SLICE(*pRB, MRV_AFM_A_H_R, ulRight);
        REG_SET_SLICE(*pRB, MRV_AFM_A_V_B, ulBottom);
    }
    else
    {
        *pLT = 0;
        *pRB = 0;
    }

    return RET_SUCCESS;
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspAfmRegs2Wnd (static) \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  ulLT .. left-top register value \n
 *               ulRB .. right-bottom register value \n
 *               pWnd .. pointer to stucture to receive the window
 *                       settings extracted from the register values \n
 *  \DESCRIPTION extracts window settings from left-top and right-bottom
 *               register values of a given AF measurement window \n
 */
/*****************************************************************************/
static void MrvIspAfmRegs2Wnd(UINT32 ulLT, UINT32 ulRB, tsMrvWindow* pWnd)
{
    int iSize; // we'll calculate with a signed value here to get the possible underflows right

    ASSERT(pWnd != NULL);

    // H & V offset are left and top values
    pWnd->usHOffs = REG_GET_SLICE(ulLT, MRV_AFM_A_H_L);
    pWnd->usVOffs = REG_GET_SLICE(ulLT, MRV_AFM_A_V_T);

    // calculate horizontal size out of left and right values
    iSize = REG_GET_SLICE(ulRB, MRV_AFM_A_H_R);
    iSize -= (pWnd->usHOffs + 1); //left and right are inside, hence plus one
    if (iSize < 0) { iSize = 0; }
    pWnd->usHSize = (UINT16)iSize;

    // calculate vertical size out of top and bottom values
    iSize = REG_GET_SLICE(ulRB, MRV_AFM_A_V_B);
    iSize -= (pWnd->usVOffs + 1); //top and bottom are inside, hence plus one
    if (iSize < 0) { iSize = 0; }
    pWnd->usVSize = (UINT16)iSize;
}

#endif // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetAutofocus \n
 *  \RETURNVALUE RET_OUTOFRANGE if any window boundary is out of range. \n
 *               RET_NOTSUPP if the AF measurement block is not present in
 *               the current marvin version and the block is to be enabled. \n
 *               RET_SUCCESS otherwise. \n
 *  \PARAMETERS  ptAfConfig .. pointer to the auto focus measurement
 *                 configuration. Specify NULL to disable the module \n
 *  \DESCRIPTION Sets the AF measurement configuration \n
 */
/*****************************************************************************/
RESULT MrvIspSetAutofocus(const tsMrvAfConfig *ptAfConfig)
{
#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 result = RET_SUCCESS;

    // disable measurement module
    REG_SET_SLICE(ptMrvReg->isp_afm_ctrl, MRV_AFM_AFM_EN, DISABLE);

    if (ptAfConfig)
    {
        UINT32 ulLT;
        UINT32 ulRB;

        // set measurement window boundaries
        if ((result = MrvIspAfmWnd2Regs(&(ptAfConfig->tWndPosA), &ulLT, &ulRB)) != RET_SUCCESS)
        {
            return result;
        }
        REG_WRITE(ptMrvReg->isp_afm_lt_a, ulLT);
        REG_WRITE(ptMrvReg->isp_afm_rb_a, ulRB);

        if ((result = MrvIspAfmWnd2Regs(&(ptAfConfig->tWndPosB), &ulLT, &ulRB)) != RET_SUCCESS)
        {
            return result;
        }
        REG_WRITE(ptMrvReg->isp_afm_lt_b, ulLT);
        REG_WRITE(ptMrvReg->isp_afm_rb_b, ulRB);

        if ((result = MrvIspAfmWnd2Regs(&(ptAfConfig->tWndPosC), &ulLT, &ulRB)) != RET_SUCCESS)
        {
            return result;
        }
        REG_WRITE(ptMrvReg->isp_afm_lt_c, ulLT);
        REG_WRITE(ptMrvReg->isp_afm_rb_c, ulRB);

        // set other af measurement paraneters
        REG_SET_SLICE(ptMrvReg->isp_afm_thres, MRV_AFM_AFM_THRES, ptAfConfig->ulThreshold);
        // Note [hf]: ulVarShift should be split into usVarLumShift and usVarAfmShift
        REG_SET_SLICE(ptMrvReg->isp_afm_var_shift, MRV_AFM_LUM_VAR_SHIFT, (ptAfConfig->ulVarShift >> 16));
        REG_SET_SLICE(ptMrvReg->isp_afm_var_shift, MRV_AFM_AFM_VAR_SHIFT, (ptAfConfig->ulVarShift >>  0));

        // enable measurement module
        REG_SET_SLICE(ptMrvReg->isp_afm_ctrl, MRV_AFM_AFM_EN, ENABLE);
    }

    return result;
#else
    if (!ptAfConfig)
    {
        return RET_SUCCESS;
    }

    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetAutofocus \n
 *  \RETURNVALUE RET_SUCCESS if the AF measurement block is present in
 *               the current marvin version. RET_NOTSUPP otherwise \n
 *  \PARAMETERS  ptAfConfig .. pointer to a variable to receive the
 *                 auto focus measurement configuration. \n
 *  \DESCRIPTION Reads the marvin afm configuration registers \n
 */
/*****************************************************************************/
RESULT MrvIspGetAutofocus(tsMrvAfConfig *ptAfConfig)
{
#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    ASSERT(ptAfConfig);

    // get measurement window boundaries
    MrvIspAfmRegs2Wnd(REG_READ(ptMrvReg->isp_afm_lt_a), REG_READ(ptMrvReg->isp_afm_rb_a), &(ptAfConfig->tWndPosA));
    MrvIspAfmRegs2Wnd(REG_READ(ptMrvReg->isp_afm_lt_b), REG_READ(ptMrvReg->isp_afm_rb_b), &(ptAfConfig->tWndPosB));
    MrvIspAfmRegs2Wnd(REG_READ(ptMrvReg->isp_afm_lt_c), REG_READ(ptMrvReg->isp_afm_rb_c), &(ptAfConfig->tWndPosC));
    // get other af measurement paraneters
    ptAfConfig->ulThreshold = REG_GET_SLICE(ptMrvReg->isp_afm_thres, MRV_AFM_AFM_THRES);
    // Note [hf]: ulVarShift should be split into usVarLumShift and usVarAfmShift
    ptAfConfig->ulVarShift  = REG_GET_SLICE(ptMrvReg->isp_afm_var_shift, MRV_AFM_LUM_VAR_SHIFT) << 16;
    ptAfConfig->ulVarShift |= REG_GET_SLICE(ptMrvReg->isp_afm_var_shift, MRV_AFM_AFM_VAR_SHIFT) << 0;

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM( ptAfConfig );
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetAutofocusMeas \n
 *  \RETURNVALUE none \n
 *  \PARAMETERS  tsMrvAfMeas .. pointer to a variable to receive
 *                 the last recent measurement values \n
 *  \DESCRIPTION Reads the marvin afm result registers \n
 *  \NOTES       If the measurement block is not included in the current marvin
 *               hardware, zeros are returned for the measurement values. \n
 */
/*****************************************************************************/
void MrvIspGetAutofocusMeas(tsMrvAfMeas *ptMrvAfMeas)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    ASSERT(ptMrvAfMeas != NULL);

#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)

    ptMrvAfMeas->ulAfmSumA = REG_GET_SLICE(ptMrvReg->isp_afm_sum_a, MRV_AFM_AFM_SUM_A);
    ptMrvAfMeas->ulAfmSumB = REG_GET_SLICE(ptMrvReg->isp_afm_sum_b, MRV_AFM_AFM_SUM_B);
    ptMrvAfMeas->ulAfmSumC = REG_GET_SLICE(ptMrvReg->isp_afm_sum_c, MRV_AFM_AFM_SUM_C);
    ptMrvAfMeas->ulAfmLumA = REG_GET_SLICE(ptMrvReg->isp_afm_lum_a, MRV_AFM_AFM_LUM_A);
    ptMrvAfMeas->ulAfmLumB = REG_GET_SLICE(ptMrvReg->isp_afm_lum_b, MRV_AFM_AFM_LUM_B);
    ptMrvAfMeas->ulAfmLumC = REG_GET_SLICE(ptMrvReg->isp_afm_lum_c, MRV_AFM_AFM_LUM_C);
#else
    ptMrvAfMeas->ulAfmSumA = 0;
    ptMrvAfMeas->ulAfmSumB = 0;
    ptMrvAfMeas->ulAfmSumC = 0;
    ptMrvAfMeas->ulAfmLumA = 0;
    ptMrvAfMeas->ulAfmLumB = 0;
    ptMrvAfMeas->ulAfmLumC = 0;

    UNUSED_PARAM(ptMrvReg);
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspWaitForAFMeas \n
 *  \RETURNVALUE RET_SUCCESS if the AF measurement is done.  \n
 *               RET_FAILURE if timeout \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Wait for end of autofocus measurement \n
 */
/*****************************************************************************/
RESULT MrvIspWaitForAFMeas(void)
{
#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)

    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    BOOL bError = FALSE;
    UINT32 ulStartTick,ulTickFreq;
    ulTickFreq  = OS_GetFrequency();

    // clear afm_fin irq
    REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_AFM_FIN, 1);
    ulStartTick = OS_GetTick();

    while (!REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_AFM_FIN))
    {
        // if timeout or 2 times vstart we have an error
        if ((((OS_GetTick() - ulStartTick) / ulTickFreq) > 5) || bError)
        {
            if (!bError)
            {
                DBG_OUT((DERR, "MrvIspWaitForAFMeas: TIMEOUT\n"));
            }


            return RET_FAILURE;
        }
    } // while


    return RET_SUCCESS;
#else
    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspChkAFMOvfSum \n
 *  \RETURNVALUE TRUE Interrupt is pending.  \n
 *               FALSE no Interrupt \n
 *  \PARAMETERS  BOOL bClear clears the IRQ  \n
 *  \DESCRIPTION Check for Sum Overflow Interrupt \n
 */
/*****************************************************************************/
BOOL MrvIspChkAFMOvfSum(BOOL bClear)
{
#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    BOOL bStatus = FALSE;

    if (REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_AFM_SUM_OF))
    {
        bStatus = TRUE;
    }

    if (bClear)
    { // clear afm_sum_of irq
        REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_AFM_SUM_OF, 1);
    }

    return bStatus;
#else // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(bClear);
    return FALSE;
#endif // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspChkAFMOvfLum \n
 *  \RETURNVALUE TRUE Interrupt is pending.  \n
 *               FALSE no Interrupt \n
 *  \PARAMETERS  BOOL bClear clears the IRQ  \n
 *  \DESCRIPTION Check for Luminance Overflow Interrupt \n
 */
/*****************************************************************************/
BOOL MrvIspChkAFMOvfLum(BOOL bClear)
{
#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    BOOL bStatus = FALSE;

    if (REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_AFM_LUM_OF))
    {
        bStatus = TRUE;
    }

    if (bClear)
    { // clear afm_luf_of irq
        REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_AFM_LUM_OF, 1);
    }
    return bStatus;

#else // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(bClear);
    return FALSE;
#endif // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetLsCorrection \n
 *  \RETURNVALUE RET_SUCCESS if the lens shading block is present in
 *               the current marvin version or if it is not present and
 *               the block is to be disabled. RET_NOTSUPP otherwise \n
 *  \PARAMETERS  tsMrvLsCorrConfig .. structure of the data tables
 *                 Specify NULL to disable the module \n
 *  \DESCRIPTION Programs the data tables, size tables and grad tables \n
 */
/*****************************************************************************/
RESULT MrvIspSetLsCorrection(const tsMrvLsCorrConfig *ptLsCorrConfig)
{
#if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 i,n;
    UINT32 data = 0;
    BOOL bEnabled = FALSE;

    if (!ptLsCorrConfig)
    { // disable lens shading module
        REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, DISABLE);
    }
    else
    {
        // test if lens shading correction is enabled
        if(REG_GET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN))
        { // switch off lens shading correction
            REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, DISABLE);
            // wait 1ms to make sure that the LSC have time enough to switch off
            OS_SleepMs( 1 );
            bEnabled = TRUE;
        }

        // clear address counters
        REG_WRITE(ptMrvReg->isp_lsc_r_table_addr, 0);
        REG_WRITE(ptMrvReg->isp_lsc_g_table_addr, 0);
        REG_WRITE(ptMrvReg->isp_lsc_b_table_addr, 0);

        // program data tables (table size is 9 * 17 = 153; see also MRV_LSC_?_RAM_ADDR_MAX)
        ASSERT(((MRV_MAX_LSC_SECTORS + 1) * ((MRV_MAX_LSC_SECTORS + 2) / 2)) == (MRV_LSC_R_RAM_ADDR_MAX + 1));
        for (n = 0; n < ((MRV_MAX_LSC_SECTORS + 1) * (MRV_MAX_LSC_SECTORS + 1)); n += MRV_MAX_LSC_SECTORS + 1) // 17 steps
        {
            for (i = 0; i < (MRV_MAX_LSC_SECTORS); i += 2) // 17 sectors with 2 values in one DWORD = 9 DWORDs (8 steps + 1 outside loop)
            {
                REG_SET_SLICE(data, MRV_LSC_R_SAMPLE_0, ptLsCorrConfig->ulLsRDataTbl[n+i]);
                REG_SET_SLICE(data, MRV_LSC_R_SAMPLE_1, ptLsCorrConfig->ulLsRDataTbl[n+i+1]);
                REG_WRITE(ptMrvReg->isp_lsc_r_table_data, data);
                REG_SET_SLICE(data, MRV_LSC_G_SAMPLE_0, ptLsCorrConfig->ulLsGDataTbl[n+i]);
                REG_SET_SLICE(data, MRV_LSC_G_SAMPLE_1, ptLsCorrConfig->ulLsGDataTbl[n+i+1]);
                REG_WRITE(ptMrvReg->isp_lsc_g_table_data, data);
                REG_SET_SLICE(data, MRV_LSC_B_SAMPLE_0, ptLsCorrConfig->ulLsBDataTbl[n+i]);
                REG_SET_SLICE(data, MRV_LSC_B_SAMPLE_1, ptLsCorrConfig->ulLsBDataTbl[n+i+1]);
                REG_WRITE(ptMrvReg->isp_lsc_b_table_data, data);
            }
            REG_SET_SLICE(data, MRV_LSC_R_SAMPLE_0, ptLsCorrConfig->ulLsRDataTbl[n+MRV_MAX_LSC_SECTORS]);
            REG_SET_SLICE(data, MRV_LSC_R_SAMPLE_1, 0);
            REG_WRITE(ptMrvReg->isp_lsc_r_table_data, data);
            REG_SET_SLICE(data, MRV_LSC_G_SAMPLE_0, ptLsCorrConfig->ulLsGDataTbl[n+MRV_MAX_LSC_SECTORS]);
            REG_SET_SLICE(data, MRV_LSC_G_SAMPLE_1, 0);
            REG_WRITE(ptMrvReg->isp_lsc_g_table_data, data);
            REG_SET_SLICE(data, MRV_LSC_B_SAMPLE_0, ptLsCorrConfig->ulLsBDataTbl[n+MRV_MAX_LSC_SECTORS]);
            REG_SET_SLICE(data, MRV_LSC_B_SAMPLE_1, 0);
            REG_WRITE(ptMrvReg->isp_lsc_b_table_data, data);
        }

        // program x size tables
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_01, MRV_LSC_X_SECT_SIZE_0, ptLsCorrConfig->ulLsXSizeTbl[0]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_01, MRV_LSC_X_SECT_SIZE_1, ptLsCorrConfig->ulLsXSizeTbl[1]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_23, MRV_LSC_X_SECT_SIZE_2, ptLsCorrConfig->ulLsXSizeTbl[2]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_23, MRV_LSC_X_SECT_SIZE_3, ptLsCorrConfig->ulLsXSizeTbl[3]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_45, MRV_LSC_X_SECT_SIZE_4, ptLsCorrConfig->ulLsXSizeTbl[4]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_45, MRV_LSC_X_SECT_SIZE_5, ptLsCorrConfig->ulLsXSizeTbl[5]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_67, MRV_LSC_X_SECT_SIZE_6, ptLsCorrConfig->ulLsXSizeTbl[6]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xsize_67, MRV_LSC_X_SECT_SIZE_7, ptLsCorrConfig->ulLsXSizeTbl[7]);

        // program y size tables
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_01, MRV_LSC_Y_SECT_SIZE_0, ptLsCorrConfig->ulLsYSizeTbl[0]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_01, MRV_LSC_Y_SECT_SIZE_1, ptLsCorrConfig->ulLsYSizeTbl[1]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_23, MRV_LSC_Y_SECT_SIZE_2, ptLsCorrConfig->ulLsYSizeTbl[2]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_23, MRV_LSC_Y_SECT_SIZE_3, ptLsCorrConfig->ulLsYSizeTbl[3]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_45, MRV_LSC_Y_SECT_SIZE_4, ptLsCorrConfig->ulLsYSizeTbl[4]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_45, MRV_LSC_Y_SECT_SIZE_5, ptLsCorrConfig->ulLsYSizeTbl[5]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_67, MRV_LSC_Y_SECT_SIZE_6, ptLsCorrConfig->ulLsYSizeTbl[6]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ysize_67, MRV_LSC_Y_SECT_SIZE_7, ptLsCorrConfig->ulLsYSizeTbl[7]);

        // program x grad tables
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_01, MRV_LSC_XGRAD_0, ptLsCorrConfig->ulLsXGradTbl[0]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_01, MRV_LSC_XGRAD_1, ptLsCorrConfig->ulLsXGradTbl[1]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_23, MRV_LSC_XGRAD_2, ptLsCorrConfig->ulLsXGradTbl[2]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_23, MRV_LSC_XGRAD_3, ptLsCorrConfig->ulLsXGradTbl[3]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_45, MRV_LSC_XGRAD_4, ptLsCorrConfig->ulLsXGradTbl[4]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_45, MRV_LSC_XGRAD_5, ptLsCorrConfig->ulLsXGradTbl[5]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_67, MRV_LSC_XGRAD_6, ptLsCorrConfig->ulLsXGradTbl[6]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_xgrad_67, MRV_LSC_XGRAD_7, ptLsCorrConfig->ulLsXGradTbl[7]);

        // program y grad tables
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_01, MRV_LSC_YGRAD_0, ptLsCorrConfig->ulLsYGradTbl[0]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_01, MRV_LSC_YGRAD_1, ptLsCorrConfig->ulLsYGradTbl[1]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_23, MRV_LSC_YGRAD_2, ptLsCorrConfig->ulLsYGradTbl[2]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_23, MRV_LSC_YGRAD_3, ptLsCorrConfig->ulLsYGradTbl[3]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_45, MRV_LSC_YGRAD_4, ptLsCorrConfig->ulLsYGradTbl[4]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_45, MRV_LSC_YGRAD_5, ptLsCorrConfig->ulLsYGradTbl[5]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_67, MRV_LSC_YGRAD_6, ptLsCorrConfig->ulLsYGradTbl[6]);
        REG_SET_SLICE(ptMrvReg->isp_lsc_ygrad_67, MRV_LSC_YGRAD_7, ptLsCorrConfig->ulLsYGradTbl[7]);

        if(bEnabled)
        { // switch on lens chading correction
            REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, ENABLE);
        }
    }

    return RET_SUCCESS;
#else
    if (!ptLsCorrConfig)
    {
        return RET_SUCCESS;
    }

    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspGetLsCorrection \n
 *  \RETURNVALUE RET_SUCCESS if the lens shading block is present in
 *               the current marvin version or if it is not present and
 *               the block is to be disabled. RET_NOTSUPP otherwise \n
 *  \PARAMETERS  tsMrvLsCorrConfig .. structure of the data tables
 *                 Specify NULL to disable the module \n
 *  \DESCRIPTION Reads out the data tables, size tables and grad tables \n
 */
/*****************************************************************************/
RESULT MrvIspGetLsCorrection(tsMrvLsCorrConfig *ptLsCorrConfig)
{
#if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 i,n, data;
    BOOL bEnabled = FALSE;

    if (!ptLsCorrConfig)
    { // disable lens shading module
        REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, DISABLE);
    }
    else
    {
        // test if lens chading correction is enabled
        if(REG_GET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN))
        { // switch off lens chading correction
            REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, DISABLE);
            bEnabled = TRUE;
        }

        // clear address counters
        REG_WRITE(ptMrvReg->isp_lsc_r_table_addr, 0);
        REG_WRITE(ptMrvReg->isp_lsc_g_table_addr, 0);
        REG_WRITE(ptMrvReg->isp_lsc_b_table_addr, 0);

        //read data tables (table size is 9 * 17 = 153; see also MRV_LSC_?_RAM_ADDR_MAX)
        ASSERT(((MRV_MAX_LSC_SECTORS + 1) * ((MRV_MAX_LSC_SECTORS + 2) / 2)) == (MRV_LSC_R_RAM_ADDR_MAX + 1));
        for (n = 0; n < ((MRV_MAX_LSC_SECTORS + 1) * (MRV_MAX_LSC_SECTORS + 1)); n += MRV_MAX_LSC_SECTORS + 1) // 17 steps
        {
            for (i = 0; i < (MRV_MAX_LSC_SECTORS); i += 2) // 17 sectors with 2 values in one DWORD = 9 DWORDs (8 steps + 1 outside loop)
            {
                // read into temp variable because only single register access is allowed (auto-increment registers)
                data = REG_READ(ptMrvReg->isp_lsc_r_table_data);
                ptLsCorrConfig->ulLsRDataTbl[n+i]   = REG_GET_SLICE(data, MRV_LSC_R_SAMPLE_0);
                ptLsCorrConfig->ulLsRDataTbl[n+i+1] = REG_GET_SLICE(data, MRV_LSC_R_SAMPLE_1);
                data = REG_READ(ptMrvReg->isp_lsc_g_table_data);
                ptLsCorrConfig->ulLsGDataTbl[n+i]   = REG_GET_SLICE(data, MRV_LSC_G_SAMPLE_0);
                ptLsCorrConfig->ulLsGDataTbl[n+i+1] = REG_GET_SLICE(data, MRV_LSC_G_SAMPLE_1);
                data = REG_READ(ptMrvReg->isp_lsc_b_table_data);
                ptLsCorrConfig->ulLsBDataTbl[n+i]   = REG_GET_SLICE(data, MRV_LSC_B_SAMPLE_0);
                ptLsCorrConfig->ulLsBDataTbl[n+i+1] = REG_GET_SLICE(data, MRV_LSC_B_SAMPLE_1);
            }
            data = REG_READ(ptMrvReg->isp_lsc_r_table_data);
            ptLsCorrConfig->ulLsRDataTbl[n+MRV_MAX_LSC_SECTORS] = REG_GET_SLICE(data, MRV_LSC_R_SAMPLE_0);
            data = REG_READ(ptMrvReg->isp_lsc_g_table_data);
            ptLsCorrConfig->ulLsGDataTbl[n+MRV_MAX_LSC_SECTORS] = REG_GET_SLICE(data, MRV_LSC_G_SAMPLE_0);
            data = REG_READ(ptMrvReg->isp_lsc_b_table_data);
            ptLsCorrConfig->ulLsBDataTbl[n+MRV_MAX_LSC_SECTORS] = REG_GET_SLICE(data, MRV_LSC_B_SAMPLE_0);
        }

        // read x size tables
        ptLsCorrConfig->ulLsXSizeTbl[0] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_01, MRV_LSC_X_SECT_SIZE_0);
        ptLsCorrConfig->ulLsXSizeTbl[1] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_01, MRV_LSC_X_SECT_SIZE_1);
        ptLsCorrConfig->ulLsXSizeTbl[2] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_23, MRV_LSC_X_SECT_SIZE_2);
        ptLsCorrConfig->ulLsXSizeTbl[3] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_23, MRV_LSC_X_SECT_SIZE_3);
        ptLsCorrConfig->ulLsXSizeTbl[4] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_45, MRV_LSC_X_SECT_SIZE_4);
        ptLsCorrConfig->ulLsXSizeTbl[5] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_45, MRV_LSC_X_SECT_SIZE_5);
        ptLsCorrConfig->ulLsXSizeTbl[6] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_67, MRV_LSC_X_SECT_SIZE_6);
        ptLsCorrConfig->ulLsXSizeTbl[7] = REG_GET_SLICE(ptMrvReg->isp_lsc_xsize_67, MRV_LSC_X_SECT_SIZE_7);

        // program y size tables
        ptLsCorrConfig->ulLsYSizeTbl[0] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_01, MRV_LSC_Y_SECT_SIZE_0);
        ptLsCorrConfig->ulLsYSizeTbl[1] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_01, MRV_LSC_Y_SECT_SIZE_1);
        ptLsCorrConfig->ulLsYSizeTbl[2] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_23, MRV_LSC_Y_SECT_SIZE_2);
        ptLsCorrConfig->ulLsYSizeTbl[3] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_23, MRV_LSC_Y_SECT_SIZE_3);
        ptLsCorrConfig->ulLsYSizeTbl[4] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_45, MRV_LSC_Y_SECT_SIZE_4);
        ptLsCorrConfig->ulLsYSizeTbl[5] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_45, MRV_LSC_Y_SECT_SIZE_5);
        ptLsCorrConfig->ulLsYSizeTbl[6] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_67, MRV_LSC_Y_SECT_SIZE_6);
        ptLsCorrConfig->ulLsYSizeTbl[7] = REG_GET_SLICE(ptMrvReg->isp_lsc_ysize_67, MRV_LSC_Y_SECT_SIZE_7);

        // read x grad tables
        ptLsCorrConfig->ulLsXGradTbl[0] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_01, MRV_LSC_XGRAD_0);
        ptLsCorrConfig->ulLsXGradTbl[1] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_01, MRV_LSC_XGRAD_1);
        ptLsCorrConfig->ulLsXGradTbl[2] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_23, MRV_LSC_XGRAD_2);
        ptLsCorrConfig->ulLsXGradTbl[3] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_23, MRV_LSC_XGRAD_3);
        ptLsCorrConfig->ulLsXGradTbl[4] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_45, MRV_LSC_XGRAD_4);
        ptLsCorrConfig->ulLsXGradTbl[5] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_45, MRV_LSC_XGRAD_5);
        ptLsCorrConfig->ulLsXGradTbl[6] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_67, MRV_LSC_XGRAD_6);
        ptLsCorrConfig->ulLsXGradTbl[7] = REG_GET_SLICE(ptMrvReg->isp_lsc_xgrad_67, MRV_LSC_XGRAD_7);

        // read y grad tables
        ptLsCorrConfig->ulLsYGradTbl[0] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_01, MRV_LSC_YGRAD_0);
        ptLsCorrConfig->ulLsYGradTbl[1] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_01, MRV_LSC_YGRAD_1);
        ptLsCorrConfig->ulLsYGradTbl[2] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_23, MRV_LSC_YGRAD_2);
        ptLsCorrConfig->ulLsYGradTbl[3] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_23, MRV_LSC_YGRAD_3);
        ptLsCorrConfig->ulLsYGradTbl[4] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_45, MRV_LSC_YGRAD_4);
        ptLsCorrConfig->ulLsYGradTbl[5] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_45, MRV_LSC_YGRAD_5);
        ptLsCorrConfig->ulLsYGradTbl[6] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_67, MRV_LSC_YGRAD_6);
        ptLsCorrConfig->ulLsYGradTbl[7] = REG_GET_SLICE(ptMrvReg->isp_lsc_ygrad_67, MRV_LSC_YGRAD_7);

        if(bEnabled)
        { // switch on lens chading correction
            REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, ENABLE);
        }
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
    UNUSED_PARAM(ptLsCorrConfig);
    if (!ptLsCorrConfig)
    {
        return RET_SUCCESS;
    }

    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
}


/*****************************************************************************/
/*!
 *  \FUNCTION     MrvIspLsCorrectionOnOff \n
 *  \RETURNVALUE RET_SUCCESS if the Bad Pixel block is present in
 *               the current marvin version or if it is not present and
 *               the block is to be disabled. RET_NOTSUPP otherwise \n
 *  \PARAMETERS  bLsCorrOnOff ..TRUE: switch on; FALSE: switch off \n
 *  \DESCRIPTION Switches the lens shading on/off \n
 */
/*****************************************************************************/
RESULT MrvIspLsCorrectionOnOff(BOOL bLsCorrOnOff)
{
#if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if(bLsCorrOnOff)
    { // switch on lens chading correction
        REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, ENABLE);
    }
    else
    { // switch off lens chading correction
        REG_SET_SLICE(ptMrvReg->isp_lsc_ctrl, MRV_LSC_LSC_EN, DISABLE);
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
    UNUSED_PARAM(bLsCorrOnOff);
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetBpCorrection \n
 *  \RETURNVALUE RET_SUCCESS if the Bad Pixel block is present in
 *               the current marvin version or if it is not present and
 *               the block is to be disabled. RET_NOTSUPP otherwise \n
 *  \PARAMETERS  ptBpConfig .. pointer to the bad pixel
 *                 configuration. Specify NULL to disable the module \n
 *  \DESCRIPTION Sets the Bad Pixel Correction configuration \n
 */
/*****************************************************************************/
RESULT MrvIspSetBpCorrection(const tsMrvBpCorrConfig *ptBpCorrConfig)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_bp_ctrl = REG_READ(ptMrvReg->isp_bp_ctrl);

    if (!ptBpCorrConfig)
    { // disable correction module
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_HOT_COR_EN, DISABLE);
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_DEAD_COR_EN, DISABLE);
    }
    else
    { // set bad pixel configuration
        if(ptBpCorrConfig->tBpCorrType == eMrvBpCorr_Direct)
        { // direct detection
            UINT32 isp_bp_cfg1 = REG_READ(ptMrvReg->isp_bp_cfg1);
            UINT32 isp_bp_cfg2 = REG_READ(ptMrvReg->isp_bp_cfg2);

            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_COR_TYPE, MRV_BP_COR_TYPE_DIRECT);

            // if register isp_bp_cfg1 is used in detection and correction, please stop
            ASSERT(!REG_GET_SLICE(ptMrvReg->isp_bp_ctrl, MRV_BP_BP_DET_EN));

            // threshold register only used for direct mode
            REG_SET_SLICE(isp_bp_cfg1, MRV_BP_HOT_THRES,  ptBpCorrConfig->usBpAbsHotThres);
            REG_SET_SLICE(isp_bp_cfg1, MRV_BP_DEAD_THRES, ptBpCorrConfig->usBpAbsDeadThres);
            REG_WRITE(ptMrvReg->isp_bp_cfg1, isp_bp_cfg1);
            REG_SET_SLICE(isp_bp_cfg2, MRV_BP_DEV_HOT_THRES,  ptBpCorrConfig->usBpDevHotThres);
            REG_SET_SLICE(isp_bp_cfg2, MRV_BP_DEV_DEAD_THRES, ptBpCorrConfig->usBpDevDeadThres);
            REG_WRITE(ptMrvReg->isp_bp_cfg2, isp_bp_cfg2);
        }
        else
        { // use bad pixel table
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_COR_TYPE, MRV_BP_COR_TYPE_TABLE);
        }

        if(ptBpCorrConfig->tBpCorrRep == eMrvBpCorr_RepLin)
        { // use linear approch
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_REP_APPR, MRV_BP_REP_APPR_INTERPOL);
        }
        else
        { // use best neighbour
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_REP_APPR, MRV_BP_REP_APPR_NEAREST);
        }

        switch (ptBpCorrConfig->tBpCorrMode)
        {
        case eMrvBpCorr_HotEn:
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_HOT_COR_EN,  ENABLE);  // enable Hot
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_DEAD_COR_EN, DISABLE); // disable Dead
            break;
        case eMrvBpCorr_DeadEn:
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_HOT_COR_EN,  DISABLE); // disable Hot
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_DEAD_COR_EN, ENABLE);  // enable Dead
            break;
        case eMrvBpCorr_HotDeadEn:
        default:
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_HOT_COR_EN,  ENABLE);  // enable Hot
            REG_SET_SLICE(isp_bp_ctrl, MRV_BP_DEAD_COR_EN, ENABLE);  // enable Dead
            break;
        }
    }

    REG_WRITE(ptMrvReg->isp_bp_ctrl, isp_bp_ctrl);
    return RET_SUCCESS;
#else
    if (!ptBpCorrConfig)
    {
        return RET_SUCCESS;
    }

    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetBpDetection \n
 *  \RETURNVALUE RET_SUCCESS if the Bad Pixel block is present in
 *               the current marvin version or if it is not present and
 *               the block is to be disabled. RET_NOTSUPP otherwise \n
 *  \PARAMETERS  ptBpDetConfig .. pointer to the bad pixel
 *                 configuration. Specify NULL to disable the module \n
 *  \DESCRIPTION Sets the Bad Pixel configuration for detection \n
 */
/*****************************************************************************/
RESULT MrvIspSetBpDetection(const tsMrvBpDetConfig *ptBpDetConfig)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if (!ptBpDetConfig)
    { // disable measurement module
        REG_SET_SLICE(ptMrvReg->isp_bp_ctrl, MRV_BP_BP_DET_EN, DISABLE);
    }
    else
    {
        // if register isp_bp_cfg1 is used in detection and correction, please stop
        ASSERT(REG_GET_SLICE(ptMrvReg->isp_bp_ctrl, MRV_BP_COR_TYPE) == MRV_BP_COR_TYPE_TABLE);

        // set dead threshold for bad pixel detection
        REG_SET_SLICE(ptMrvReg->isp_bp_cfg1, MRV_BP_DEAD_THRES, ptBpDetConfig->ulBpDeadThres);

        // enable measurement module
        REG_SET_SLICE(ptMrvReg->isp_bp_ctrl, MRV_BP_BP_DET_EN, ENABLE);
    }

    return RET_SUCCESS;
#else
    if (!ptBpDetConfig)
    {
        return RET_SUCCESS;
    }

    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspBpReadNewTable \n
 *  \RETURNVALUE returns the number of the new detected fixed pixel \n
 *  \PARAMETERS  ptBpNewTable .. pointer to the bad pixel table \n
 *               bClearNewTable delete the new table \n
 *  \DESCRIPTION ??? \n
 */
/*****************************************************************************/
RESULT MrvIspBpReadNewTable(tsMrvBpNewTable *ptBpNewTable, BOOL bClearNewTable)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32   i;

    if (!ptBpNewTable)
    { // null pointer received
        return RET_NULL_POINTER;
    }
    else
    {
        ptBpNewTable->ulBpNumber = REG_GET_SLICE(ptMrvReg->isp_bp_new_number, MRV_BP_BP_NEW_NUMBER);
        ASSERT(ptBpNewTable->ulBpNumber <= MRV_ISP_BP_NEW_TABLE_ARR_SIZE);

        for (i = 0; i < ptBpNewTable->ulBpNumber; i++)
        { // set dead threshold for bad pixel detection
            ptBpNewTable->atBpNewTableElem[i].ucBpMsbValue = (UINT8) REG_GET_SLICE(ptMrvReg->isp_bp_new_table[i], MRV_BP_NEW_VALUE);
            ptBpNewTable->atBpNewTableElem[i].usBpVerAddr  = (UINT16)REG_GET_SLICE(ptMrvReg->isp_bp_new_table[i], MRV_BP_NEW_V_ADDR);
            ptBpNewTable->atBpNewTableElem[i].usBpHorAddr  = (UINT16)REG_GET_SLICE(ptMrvReg->isp_bp_new_table[i], MRV_BP_NEW_H_ADDR);
        }

        if (bClearNewTable)
        {
            REG_SET_SLICE(ptMrvReg->isp_bp_new_number, MRV_BP_BP_NEW_NUMBER, 0);
        }
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(ptBpNewTable);
    UNUSED_PARAM(bClearNewTable);

    if (!ptBpNewTable)
    {
        return RET_NULL_POINTER;
    }

    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspBpWriteTable \n
 *  \RETURNVALUE returns the number of the new detected fixed pixel \n
 *  \PARAMETERS  ptBpTable .. pointer to the bad pixel table \n
 *               bClearNewTable delete the new table \n
 *  \DESCRIPTION ??? \n
 */
/*****************************************************************************/
RESULT MrvIspBpWriteTable(const tsIsiBpTable *ptBpTable)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if ((!ptBpTable) || (!ptBpTable->ptBpTableElem))
    { // null pointer received
        return RET_NULL_POINTER;
    }
    else
    {
        tsIsiBpTableElem *ptBpTableElemIdx;
        UINT32 i;
        UINT32 isp_bp_ctrl = REG_READ(ptMrvReg->isp_bp_ctrl);
        UINT32 isp_bp_ctrl_save = isp_bp_ctrl; // stores current BP base config
        UINT32 isp_bp_table_data = 0;

        // disable BP block
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_HOT_COR_EN,  DISABLE);
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_DEAD_COR_EN, DISABLE);
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_BP_DET_EN,   DISABLE);
        REG_WRITE(ptMrvReg->isp_bp_ctrl, isp_bp_ctrl);

        ASSERT(ptBpTable->ulBpNumber  <= (MRV_BP_BP_TABLE_ADDR_MAX + 1));
        REG_SET_SLICE(ptMrvReg->isp_bp_number, MRV_BP_BP_NUMBER, ptBpTable->ulBpNumber);
        REG_SET_SLICE(ptMrvReg->isp_bp_table_addr, MRV_BP_BP_TABLE_ADDR, 0); // set to first address

        for (i = 0, ptBpTableElemIdx = ptBpTable->ptBpTableElem; i < ptBpTable->ulBpNumber; i++, ptBpTableElemIdx++)
        { // set dead threshold for bad pixel detection
            if (ptBpTableElemIdx->eBpType == eIsiBpHot)
            {
                REG_SET_SLICE(isp_bp_table_data, MRV_BP_PIX_TYPE, MRV_BP_PIX_TYPE_HOT);
            }
            else
            {
                REG_SET_SLICE(isp_bp_table_data, MRV_BP_PIX_TYPE, MRV_BP_PIX_TYPE_DEAD);
            }

            REG_SET_SLICE(isp_bp_table_data, MRV_BP_V_ADDR, ptBpTableElemIdx->usBpVerAddr);
            REG_SET_SLICE(isp_bp_table_data, MRV_BP_H_ADDR, ptBpTableElemIdx->usBpHorAddr);
            REG_WRITE(ptMrvReg->isp_bp_table_data, isp_bp_table_data); // auto increment
        }

        // re-enable BP block
        REG_WRITE(ptMrvReg->isp_bp_ctrl, isp_bp_ctrl_save);
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(ptBpTable);
    if (!ptBpTable)
    {
        return RET_NULL_POINTER;
    }

    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspBpReadTable \n
 *  \RETURNVALUE returns the number of the new detected fixed pixel \n
 *  \PARAMETERS  ptBpTable .. pointer to the bad pixel table \n
 *               bClearNewTable delete the new table \n
 *  \DESCRIPTION ??? \n
 */
/*****************************************************************************/
RESULT MrvIspBpReadTable(tsIsiBpTable *ptBpTable)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    if ((!ptBpTable) || (!ptBpTable->ptBpTableElem))
    { // null pointer receive
        return RET_NULL_POINTER;
    }
    else
    {
        tsIsiBpTableElem *ptBpTableElemIdx;
        UINT32 i;
        UINT32 isp_bp_ctrl = REG_READ(ptMrvReg->isp_bp_ctrl);
        UINT32 isp_bp_ctrl_save = isp_bp_ctrl; // stores current BP base config
        UINT32 isp_bp_table_data = 0;

        // disable BP block
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_HOT_COR_EN,  DISABLE);
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_DEAD_COR_EN, DISABLE);
        REG_SET_SLICE(isp_bp_ctrl, MRV_BP_BP_DET_EN,   DISABLE);
        REG_WRITE(ptMrvReg->isp_bp_ctrl, isp_bp_ctrl);

        // read BP number
        ptBpTable->ulBpNumber = REG_GET_SLICE(ptMrvReg->isp_bp_number, MRV_BP_BP_NUMBER);
        ASSERT( MRV_BP_BP_TABLE_ADDR_MAX <= ptBpTable->ulBpTableElemNum);

        if(ptBpTable->ulBpNumber > ptBpTable->ulBpTableElemNum)
        {
            // re-enable BP block
            REG_WRITE(ptMrvReg->isp_bp_ctrl, isp_bp_ctrl_save);

            return RET_FAILURE;
        }

        REG_SET_SLICE(ptMrvReg->isp_bp_table_addr, MRV_BP_BP_TABLE_ADDR, 0); // set to first address

        for (i = 0, ptBpTableElemIdx = ptBpTable->ptBpTableElem; i < ptBpTable->ulBpNumber; i++, ptBpTableElemIdx++)
        { // set dead threshold for bad pixel detection
            isp_bp_table_data = REG_READ(ptMrvReg->isp_bp_table_data); // auto increment

            if (REG_GET_SLICE(isp_bp_table_data, MRV_BP_PIX_TYPE) == MRV_BP_PIX_TYPE_HOT)
            {
                ptBpTableElemIdx->eBpType = eIsiBpHot;
            }
            else
            {
                ptBpTableElemIdx->eBpType = eIsiBpDead;
            }

            ptBpTableElemIdx->usBpVerAddr = (UINT16)REG_GET_SLICE(isp_bp_table_data, MRV_BP_V_ADDR);
            ptBpTableElemIdx->usBpHorAddr = (UINT16)REG_GET_SLICE(isp_bp_table_data, MRV_BP_H_ADDR);
        }

        // re-enable BP block
        REG_WRITE(ptMrvReg->isp_bp_ctrl, isp_bp_ctrl_save);
    }

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(ptBpTable);
    if (!ptBpTable)
    {
        return RET_NULL_POINTER;
    }

    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspWaitForBadPixel \n
 *  \RETURNVALUE RET_SUCCESS if the Bad Pixel is detected.  \n
 *               RET_FAILURE if timeout \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Wait for a detection of a bad pixel \n
 */
/*****************************************************************************/
RESULT MrvIspWaitForBadPixel(void)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulStartTick,ulTickFreq;
    ulTickFreq  = OS_GetFrequency();

    // clear bp_det irq
    REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_BP_DET, 1);
    ulStartTick = OS_GetTick();

    while (!REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_BP_DET))
    {
        // if timeout then stop
        if ((((OS_GetTick() - ulStartTick) / ulTickFreq) > 100))
        {
            DBG_OUT((DERR, "MrvIspWaitForBadPixel: TIMEOUT\n"));


            return RET_FAILURE;
        }
    } // while


    return RET_SUCCESS;
#else
    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspChkBpIntStat \n
 *  \RETURNVALUE TRUE if the Bad Pixel Interrupt is detected.  \n
 *               FALSE if the Bad Pixel Interrupt is't detected \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Check if Bad Pixel Interrupt is detected. \n
 */
/*****************************************************************************/
BOOL MrvIspChkBpIntStat(void)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // detect bp irq
    if (REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_BP_DET))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspClearBpInt \n
 *  \RETURNVALUE RET_SUCCESS if the Bad Pixel block is present in
 *               the current marvin version or if it is not present and
 *               the block is to be disabled. RET_NOTSUPP otherwise \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Clear Bad Pixel Interrupt Status. \n
 */
/*****************************************************************************/
RESULT MrvIspClearBpInt(void)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // clear bp_det irq (only if it is signalled to prevent loss of irqs)
    if (REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_BP_DET))
    {
        REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_BP_DET, 1);
    }

    return RET_SUCCESS;
#else
    return RET_NOTSUPP;
#endif
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspChkBPNewTblFull \n
 *  \RETURNVALUE TRUE Interrupt is pending.  \n
 *               FALSE no Interrupt \n
 *  \PARAMETERS  BOOL bClear clears the IRQ  \n
 *  \DESCRIPTION Check for Interrupt \n
 */
/*****************************************************************************/
BOOL MrvIspChkBPNewTblFull(BOOL bClear)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    BOOL bStatus = FALSE;

    if (REG_GET_SLICE(ptMrvReg->isp_ris, MRV_ISP_RIS_BP_NEW_TAB_FUL))
    {
        bStatus = TRUE;
    }

    if (bClear)
    { // clear bp_newtablefull irq
        REG_SET_SLICE(ptMrvReg->isp_icr, MRV_ISP_ICR_BP_NEW_TAB_FUL, 1);
    }

    return bStatus;
#else // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(bClear);
    return FALSE;
#endif // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspInitializeFilterRegisters \n
 *  \RETURNVALUE RET_SUCCESS in case of successful register initialization. \n
 *               RET_NOTSUPP if feature is not included in software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION Initializes Isp filter registers with default reset values. \n
 */
/*****************************************************************************/
RESULT MrvIspInitializeFilterRegisters(void)
{
#if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    ptMrvReg->isp_filt_mode =            0x00000000;
    ptMrvReg->isp_filt_mask_sharp_0 =    0x00000020;
    ptMrvReg->isp_filt_mask_sharp_1 =    0x007C051E;
    ptMrvReg->isp_filt_mask_sharp_diag = 0x00007B2A;
    ptMrvReg->isp_filt_mask_blur_max =   0x00060706;
    ptMrvReg->isp_filt_mask_blur =       0x0002080C;
    ptMrvReg->isp_filt_mask_lin =        0x0002080C;
    ptMrvReg->isp_filt_mask_orth =       0x00000020;
    ptMrvReg->isp_filt_mask_diag =       0x0002080C;
    ptMrvReg->isp_filt_mask_h_diag =     0x00000020;
    ptMrvReg->isp_filt_blur_th0 =        0x0000000A;
    ptMrvReg->isp_filt_blur_th1 =        0x00000003;
    ptMrvReg->isp_filt_sharp0_th =       0x00000100;
    ptMrvReg->isp_filt_sharp1_th =       0x00000280;
    ptMrvReg->isp_filt_lum_weight =      0x00032040;

    return RET_SUCCESS;
#elif (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V2)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    ptMrvReg->isp_filt_mode =            0x00000000;
    ptMrvReg->isp_filt_fac_sh1 =         0x00000010;
    ptMrvReg->isp_filt_fac_sh0 =         0x0000000C;
    ptMrvReg->isp_filt_fac_mid =         0x0000000A;
    ptMrvReg->isp_filt_fac_bl0 =         0x00000006;
    ptMrvReg->isp_filt_fac_bl1 =         0x00000002;
    ptMrvReg->isp_filt_thresh_bl0 =      0x0000000D;
    ptMrvReg->isp_filt_thresh_bl1 =      0x00000005;
    ptMrvReg->isp_filt_thresh_sh0 =      0x0000001A;
    ptMrvReg->isp_filt_thresh_sh1 =      0x0000002C;
    ptMrvReg->isp_filt_lum_weight =      0x00032040;

    return RET_SUCCESS;
#else // #if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)
    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspInitializeFilterRegisters \n
 *  \RETURNVALUE RET_SUCCESS in case of successful register initialization. \n
 *               RET_NOTSUPP if feature is not included in software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *  \PARAMETERS  none \n
 *  \DESCRIPTION ??? \n
 */
/*****************************************************************************/
RESULT MrvIspActivateFilter(BOOL bActivateFilter)
{
#if (MARVIN_FEATURE_ISP_FILTER != MARVIN_FEATURE_EXIST_NOT)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    RESULT retVal = RET_SUCCESS;

    // Initialize ISP filter control registers first
    retVal = MrvIspInitializeFilterRegisters();
    if (retVal != RET_SUCCESS)
    {
        return retVal;
    }

    // Activate or deactivate filter algorythm
    REG_SET_SLICE(ptMrvReg->isp_filt_mode, MRV_FILT_FILT_ENABLE, (bActivateFilter) ? ENABLE : DISABLE);

    return retVal;
#else // #if (MARVIN_FEATURE_ISP_FILTER != MARVIN_FEATURE_EXIST_NOT)
    UNUSED_PARAM(bActivateFilter);

    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_ISP_FILTER != MARVIN_FEATURE_EXIST_NOT)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvIspSetFilterParams \n
 *  \RETURNVALUE RET_SUCCESS if filter coefficients and threshold values \n
 *               are successfully set. \n
 *               RET_OUTOFRANGE if parameter value is out of range. \n
 *               RET_NOTSUPP if feature is not included in software version. \n
 *               RET_NULL_POINTER: Invalid NULL pointer access. \n
 *               RET_CANCELED: Action canceled because the module is not active. \n
 *  \PARAMETERS  uNoiseReducLevel .. Filter noise level; Value range: 0 - 10 \n
 *               uSharpLevel      .. Filter sharp level; Value range: 0 - 8 \n
 *  \DESCRIPTION Write coefficient and threshold values into Isp filter \n
 *               registers for noise, sharpness and blurring filtering. \n
 */
/*****************************************************************************/
RESULT MrvIspSetFilterParams(UINT8 uNoiseReducLevel, UINT8 uSharpLevel)
{
#if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)

    //                                          out_coeff    mid_coeff   center_coeff
    const UINT32 MaskSharpBx4 = 0x00E9083E;  // 0x69 (-23)   0x08 (8)    0x3E (62); anti_bayer (121) notch + extra strong sharp  (max)
// [hf], 23.5.2007: Why E9 ? - all coeff masks are 7F so 69 is written !
    const UINT32 MaskSharpBx3 = 0x00ED0836;  // 0x6D (-19)   0x08 (8)    0x36 (54); anti_bayer (121) notch + strong sharp
    const UINT32 MaskSharpBx2 = 0x00F00830;  // 0x70 (-16)   0x08 (8)    0x30 (48); anti_bayer (121) notch + strong sharp
    const UINT32 MaskSharpBx1 = 0x00F3082A;  // 0x73 (-13)   0x08 (8)    0x2A (42); anti_bayer (121) notch + strong sharp
    const UINT32 MaskSharpBx0 = 0x00F60824;  // 0x76 (-10)   0x08 (8)    0x24 (36); anti_bayer (121) notch + sharp
    const UINT32 MaskSharpB3  = 0x00F80820;  // 0x78 ( -8)   0x08 (8)    0x20 (32); anti_bayer (121) notch + sharp
    const UINT32 MaskSharpB2  = 0x00FA081C;  // 0x7A ( -6)   0x08 (8)    0x1C (28); anti_bayer (121) notch + light sharp
    const UINT32 MaskSharpB1  = 0x00FC0818;  // 0x7C ( -4)   0x08 (8)    0x18 (24); anti_bayer (121) like bypass
    const UINT32 MaskSharp5a  = 0x007F7E26;  // 0x7F ( -1)   0x7E (-2)   0x26 (38);
    const UINT32 MaskSharp5b  = 0x007F7D28;  // 0x7F ( -1)   0x7D (-3)   0x28 (40);
    const UINT32 MaskSharp5c  = 0x007E7C4C;  // 0x7E ( -2)   0x7C (-4)   0x4C (44);
    const UINT32 MaskSharp5d  = 0x007D7A32;  // 0x7D ( -3)   0x7A (-6)   0x32 (50);
    const UINT32 Mask_8_16_8  = 0x00000810;  // 0x00 (  0)   0x08 (8)    0x10 (16); anti_bayer (121) blur
    const UINT32 Mask281282   = 0x0002080C;  // 0x02 (  2)   0x08 (8)    0x0C (12); anti_bayer (121)
    const UINT32 Mask48884    = 0x00040808;  // 0x04 (  4)   0x08 (8)    0x08 ( 8); anti_bayer (121)
    const UINT32 Mask58685    = 0x00050806;  // 0x05 (  5)   0x08 (8)    0x06 ( 6); anti_bayer (121) max blur
    const UINT32 MaskBypass1  = 0x00000020;  // 0x00 (  0)   0x00 (0)    0x20 (32);

    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_filt_mode = 0;

    if (!REG_GET_SLICE(ptMrvReg->isp_filt_mode, MRV_FILT_FILT_ENABLE))
    {
        return RET_CANCELED;
    }

    REG_WRITE(ptMrvReg->isp_filt_mode, isp_filt_mode); // switch off filter (and clear all bits) while changing its configuration

    if (((uNoiseReducLevel <= 10) || (uNoiseReducLevel == 99)) && (uSharpLevel <= 10))
    {
        switch (uNoiseReducLevel)
        {
        case 99: // Test Mode
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, MaskSharpBx1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, MaskSharpBx1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, MaskSharpBx1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB2);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, MaskSharpB2);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskSharpB2);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_SHARP); // diagonal sharpening mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_BYPASS);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_BYPASS);
            break;

        case 0: // NoiseReductionLevel = 0
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000002);  // 0.02 * 92 = 1.84 -> round up = 2
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x00000008);  // 0.08 * 92 = 7.36 -> round up = 8
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x0000005C);  // 1    * 92 = 92   -> round up = 92
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x0000008A);  // 1.5  * 92 = 138  -> round up = 138
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_SHARP); // diagonal sharpening mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_BYPASS);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_BYPASS);
            break;

        case 1: // NoiseReductionLevel = 1; minimum noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000002);  // 0.02 * 92 = 1.84 -> round up = 2
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x00000008);  // 0.08 * 92 = 7.36 -> round up = 8
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x0000001C);  // 0.3  * 92 = 27,6 -> round up = 28
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x0000008A);  // 1.5  * 92 = 138  -> round up = 138
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_SHARP); // diagonal sharpening mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_1);
            break;

        case 2: // NoiseReductionLevel = 2; very light noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000003);  // 0.03 * 92 = 2.76  -> round up = 3
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x0000000C);  // 0.12 * 92 = 11.04 -> round up = 12
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x0000001C);  // 0.3  * 92 = 27,6  -> round up = 28
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x0000008A);  // 1.5  * 92 = 138   -> round up = 138
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_SHARP); // diagonal sharpening mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_1);
            break;

        case 3: // NoiseReductionLevel = 3; light noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000005);  // 0.05 * 92 = 4.6   -> round up = 5
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x0000000F);  // 0.16 * 92 = 14.72 -> round up = 15
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x00000038);  // 0.6  * 92 = 55.2  -> round up = 56
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x000000B8);  // 2    * 92 = 184   -> round up = 184
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_SHARP); // diagonal sharpening mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_1);
            break;

        case 4: // NoiseReductionLevel = 4; medium noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000003);  // 0.03 * 92 = 2.76  -> round up = 3
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x00000008);  // 0.08 * 92 = 7.36  -> round up = 8
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x0000004A);  // 0.8  * 92 = 73.6  -> round up = 74
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x000000B8);  // 2    * 92 = 184   -> round up = 184
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask58685);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_BLUR); // diagonal blurring mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC10);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_2);
            break;

        case 5: // NoiseReductionLevel = 5; strong noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000003);  // 0.03 * 92 = 2.76  -> round up = 3
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x00000008);  // 0.08 * 92 = 7.36  -> round up = 8
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x0000004A);  // 0.8  * 92 = 73.6  -> round up = 74
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x000000B8);  // 2    * 92 = 184   -> round up = 184
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask58685);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_BLUR); // diagonal blurring mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC10);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_2);
            break;

        case 6: // NoiseReductionLevel = 6; very strong noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000006);  // 0.06 * 92 = 5.52 -> round up = 6
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x0000000F);  // 0.16 * 92 = 14.72 -> round up = 15
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x0000006F);  // 1.2  * 92 = 110.4 -> round up = 111
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x000000B8);  // 2    * 92 = 184   -> round up = 184
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask58685);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, MaskBypass1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_BLUR); // diagonal blurring mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC10);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_2);
            break;

        case 7: // NoiseReductionLevel = 7; heavy noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000013);  // 0.2 * 92 = 18.4 -> round up = 19
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x00000041);  // 0.7 * 92 = 64.4 -> round up = 65
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x000000B8);  // 2   * 92 = 184  -> round up = 184
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x00000170);  // 4   * 92 = 368  -> round up = 368
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask58685);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_BLUR); // diagonal blurring mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_2);
            break;

        case 8: // NoiseReductionLevel = 8; very heavy noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000041);  // 0.7 * 92 = 64.4 -> round up = 65
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x0000008A);  // 1.5 * 92 = 138  -> round up = 138
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x000000E6);  // 2.5 * 92 = 230  -> round up = 230
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x00000170);  // 4   * 92 = 368  -> round up = 368
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask58685);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_BLUR); // diagonal blurring mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_2);
            break;

        case 9: // NoiseReductionLevel = 9; extreme noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x00000053);  // 0.9 * 92 = 82.8  -> round up = 83
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x000000D4);  // 2.3 * 92 = 211.6 -> round up = 212
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x00000114);  // 3   * 92 = 276   -> round up = 276
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x00000170);  // 4   * 92 = 368   -> round up = 368
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask58685);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_BLUR); // diagonal blurring mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_2);
            break;

        case 10: // NoiseReductionLevel = 10; extreme noise
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th1,  MRV_FILT_BLUR_TH1,  0x0000021F);  // 5.9 * 92 = 542.8 -> round up = 543
            REG_SET_SLICE(ptMrvReg->isp_filt_blur_th0,  MRV_FILT_BLUR_TH0,  0x0000021F);  // 5.9 * 92 = 542.8 -> round up = 543
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x0000021F);  // 5.9 * 92 = 542.8 -> round up = 543
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH, 0x00000228);  // 6   * 92 = 552   -> round up = 552
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur_max, MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_blur,     MRV_FILT_COEFF_ALL, Mask281282);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_lin,      MRV_FILT_COEFF_ALL, Mask58685);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_orth,     MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_diag,     MRV_FILT_COEFF_ALL, Mask48884);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_h_diag,   MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,  MRV_FILT_COEFF_ALL, Mask_8_16_8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_DIAG_SHARP_MODE,    MRV_FILT_DIAG_SHARP_MODE_BLUR); // diagonal blurring mode
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_V_MODE, MRV_FILT_CHROMINANCE_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_CHROMINANCE_H_MODE, MRV_FILT_CHROMINANCE_H_MODE_DYN_2);
            break;

        default:
            return RET_OUTOFRANGE;
        }

        switch (uSharpLevel)
        {
        case 0: // SharpLevel = 0; no sharp enhancement
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th,    MRV_FILT_SHARP1_TH, 0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1, MRV_FILT_COEFF_ALL, REG_GET_SLICE(ptMrvReg->isp_filt_mask_sharp_0, MRV_FILT_COEFF_ALL));
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC);
            break;

        case 1: // SharpLevel = 1;
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x000000B8)
            { // Sharp1Thresh > 2 ; 2 * 92 = 184 -> round up = 184
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x000000B8);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1, MRV_FILT_COEFF_ALL, MaskSharpB2);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC);
            break;

        case 2: // SharpLevel = 2;
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x000000B8)
            { // Sharp1Thresh > 2 ; 2 * 92 = 184 -> round up = 184
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x000000B8);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpB1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpB2);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5a);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 3: // SharpLevel = 3;
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x000000B8)
            { // Sharp1Thresh > 2 ; 2 * 92 = 184 -> round up = 184
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x000000B8);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpB2);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpB3);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5a);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 4: // SharpLevel = 4;
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x000000B8)
            { // Sharp1Thresh > 2 ; 2 * 92 = 184 -> round up = 184
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x000000B8);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpB3);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpBx0);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5b);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 5: // SharpLevel = 5;
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x000000B8)
            { // Sharp1Thresh > 2 ; 2 * 92 = 184 -> round up = 184
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x000000B8);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpB3);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpBx1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5b);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 6: // SharpLevel = 6;
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x000000B8)
            { // Sharp1Thresh > 2 ; 2 * 92 = 184 -> round up = 184
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x000000B8);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpB3);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpBx2);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5c);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 7: // SharpLevel = 7;
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x0000008A)
            { // Sharp1Thresh > 1.5; 1.5 * 92 = 138 -> round up = 138
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x0000008A);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpBx1);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpBx3);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5c);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 8: // SharpLevel = 8;
            ptMrvReg->isp_filt_sharp0_th = 0x00000013; // 0.2 * 92 = 18.4 -> round up = 19
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x0000008A)
            { // Sharp1Thresh > 1.5; 1.5 * 92 = 138 -> round up = 138
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x0000008A);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpBx2);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpBx4);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5d);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 9: // SharpLevel = 9;
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x00000013); // 0.2 * 92 = 18.4 -> round up = 19
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x00000050)
            { // Sharp1Thresh > 1.5; 1.5 * 92 = 138 -> round up = 138           [hf]: --> 80 ?
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x00000050);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpBx2);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpBx4);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5d);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        case 10: // SharpLevel = 10;
            REG_SET_SLICE(ptMrvReg->isp_filt_sharp0_th, MRV_FILT_SHARP0_TH, 0x00000013); // 0.2 * 92 = 18.4 -> round up = 19
            if (REG_GET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH) > 0x00000020)
            { // Sharp1Thresh > 1.5; 1.5 * 92 = 138 -> round up = 138           [hf]: --> 32 ?
                REG_SET_SLICE(ptMrvReg->isp_filt_sharp1_th, MRV_FILT_SHARP1_TH,   0x00000020);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_0,    MRV_FILT_COEFF_ALL, MaskSharpBx3);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_1,    MRV_FILT_COEFF_ALL, MaskSharpBx4);
            REG_SET_SLICE(ptMrvReg->isp_filt_mask_sharp_diag, MRV_FILT_COEFF_ALL, MaskSharp5d);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,  MRV_FILT_FILT_MODE_DYNAMIC_DIAG);
            break;

        default:
            return RET_OUTOFRANGE;
        }

        // Set ISP filter mode register values
        REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_ENABLE, ENABLE); // enable filter
        REG_WRITE(ptMrvReg->isp_filt_mode, isp_filt_mode);

        return RET_SUCCESS;
    }
    else
    {
        // At least one function parameter is out of range
        return RET_OUTOFRANGE;
    }

#elif (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V2)

    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_filt_mode = 0;

    if (!REG_GET_SLICE(ptMrvReg->isp_filt_mode, MRV_FILT_FILT_ENABLE))
    {
        return RET_CANCELED;
    }

    REG_WRITE(ptMrvReg->isp_filt_mode, isp_filt_mode); // switch off filter (and clear all bits) while changing its configuration

    if (((uNoiseReducLevel <= 10) || (uNoiseReducLevel == 99)) && (uSharpLevel <= 10))
    {
        switch (uNoiseReducLevel)
        {
        case 99: // Test Mode
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1, 0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0, 0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0, 0x000003FF);  // 10 bit max value
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1, 0x000003FF);  // 10 bit max value
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 0 /* MRV_FILT_STAGE1_SELECT_MAX_BLUR */);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_BYPASS);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_BYPASS);
            break;

         case 0:
            // NoiseReductionLevel = 0
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1, 0x000000);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0, 0x000000);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0, 0x000000);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1, 0x000000);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 6);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC8);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_BYPASS);
            break;

        case 1:
            // NoiseReductionLevel = 1;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,   33);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,   18);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,    8);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,    2);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 6);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 2:
            // NoiseReductionLevel = 2;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,   44);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,   26);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,   13);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,    5);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 4 /* MRV_FILT_STAGE1_SELECT_DEFAULT */);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 3:
            // NoiseReductionLevel = 3;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,   51);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,   36);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,   23);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,   10);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 4 /* MRV_FILT_STAGE1_SELECT_DEFAULT */);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 4:
            // NoiseReductionLevel = 4;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,   67);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,   41);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,   26);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,   15);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 3);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 5:
            // NoiseReductionLevel = 5;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,  100);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,   75);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,   50);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,   20);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 3);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 6:
            // NoiseReductionLevel = 6;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,  120);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,   90);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,   60);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,   26);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 2);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 7:
            // NoiseReductionLevel = 7;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,  150);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,  120);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,   80);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,   51);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 2);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 8:
            // NoiseReductionLevel = 8;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,  200);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,  170);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,  140);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,  100);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, 2);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 9:
            // NoiseReductionLevel = 9;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,  300);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0,  250);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0,  180);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1,  150);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, (uSharpLevel > 3) ? 2 : 1);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        case 10:
            // NoiseReductionLevel = 10; extrem noise
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1, 1023);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0, 1023);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl0, MRV_FILT_FILT_THRESH_BL0, 1023);
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_bl1, MRV_FILT_FILT_THRESH_BL1, 1023);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_STAGE1_SELECT, (uSharpLevel > 5) ? 2 : ((uSharpLevel > 3) ? 1 : 0));
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_V_MODE, MRV_FILT_FILT_CHR_V_MODE_STATIC12);
            REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_CHR_H_MODE, MRV_FILT_FILT_CHR_H_MODE_DYN_2);
            break;

        default:
            return RET_OUTOFRANGE;
        }

        switch(uSharpLevel)
        {
        case 0: // SharpLevel = 0; no sharp enhancement
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x00000004);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x00000004);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000004);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000002);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000000);
            break;

        case 1: // SharpLevel = 1;
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x00000008);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x00000007);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000006);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000002);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000000);
            break;

        case 2: // SharpLevel = 2;
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x0000000C);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x0000000A);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000008);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000004);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000000);
            break;

        case 3: // SharpLevel = 3;
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x00000010);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x0000000C);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x0000000A);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000006);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000002);
            break;

        case 4: // SharpLevel = 4;
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x00000016);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x00000010);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x0000000C);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000008);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000004);
            break;

        case 5: // SharpLevel = 5;
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x0000001B);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x00000014);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000010);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x0000000A);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000004);
            break;

        case 6: // SharpLevel = 6;
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x00000020);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x0000001A);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000013);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x0000000C);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000006);
            break;

        case 7: // SharpLevel = 7;
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x00000026);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x0000001E);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000017);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000010);
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000008);
            break;

        case 8: // SharpLevel = 8;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0, 0x00000013);
            if (REG_GET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1) > 0x0000008A)
            {
                REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,   0x0000008A);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x0000002C); // 43
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x00000024); // 36
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x0000001D); // 29
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000015); // 21
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x0000000D); // 14
            break;

        case 9: // SharpLevel = 9;
            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0, 0x00000013);
            if (REG_GET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1) > 0x0000008A)
            {
                REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,   0x0000008A);
            }
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x00000030); // 48
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x0000002A); // 42
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000022); // 34
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x0000001A); // 26
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000014); // 20
            break;

        case 10: // SharpLevel = 10;
//            REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh0, MRV_FILT_FILT_THRESH_SH0, 0x00000013);
//            if (REG_GET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1) > 0x0000008A)
//            {
//                REG_SET_SLICE(ptMrvReg->isp_filt_thresh_sh1, MRV_FILT_FILT_THRESH_SH1,   0x0000008A);
//            }
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh1, MRV_FILT_FILT_FAC_SH1, 0x0000003F); // 63
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_sh0, MRV_FILT_FILT_FAC_SH0, 0x00000030); // 48
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_mid, MRV_FILT_FILT_FAC_MID, 0x00000028); // 40
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, 0x00000024); // 36
            REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, 0x00000020); // 32
            break;

        default:
            return RET_OUTOFRANGE;
        }

        if (uNoiseReducLevel > 7)
        {
            if (uSharpLevel > 7)
            {
                UINT32 filt_fac_bl0 = REG_GET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0);
                UINT32 filt_fac_bl1 = REG_GET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1);
                REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, (filt_fac_bl0)     >> 1); // * 0.50
                REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, (filt_fac_bl1)     >> 2); // * 0.25
            }
            else if (uSharpLevel > 4)
            {
                UINT32 filt_fac_bl0 = REG_GET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0);
                UINT32 filt_fac_bl1 = REG_GET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1);
                REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl0, MRV_FILT_FILT_FAC_BL0, (filt_fac_bl0 * 3) >> 2); // * 0.75
                REG_SET_SLICE(ptMrvReg->isp_filt_fac_bl1, MRV_FILT_FILT_FAC_BL1, (filt_fac_bl1)     >> 1); // * 0.50
            }
        }

        // Set ISP filter mode register values
        REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_MODE,   MRV_FILT_FILT_MODE_DYNAMIC);
        REG_SET_SLICE(isp_filt_mode, MRV_FILT_FILT_ENABLE, ENABLE); // enable filter
        REG_WRITE(ptMrvReg->isp_filt_mode, isp_filt_mode);

        return RET_SUCCESS;
    }
    else
    {
        // At least one function parameter is out of range
        return RET_OUTOFRANGE;
    }

#else // #if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)

    UNUSED_PARAM(uNoiseReducLevel);
    UNUSED_PARAM(uSharpLevel);

    return RET_NOTSUPP;
#endif // #if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)
}


