/*****************************************************************************/
/*!
 *  \file        project_settings_mrv.h \n
 *  \version     4.0 \n
 *  \author      Vogt \n
 *  \brief       File to hold the project specific defines for MARVIN
 *               drivers. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: project_settings_mrv.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
*/
/*****************************************************************************/

#ifndef _PROJECT_SETTTINGS_MRV_H
#define _PROJECT_SETTTINGS_MRV_H


/**********************************************************************/
/*!
 * \name MARVIN_HW
 *   select which MARVIN hardware is used.
 *   (R00 is used if no release number has been assigned)
 */
/**********************************************************************/
//@{
#define MARVIN_3_V22_R11         3  //!< MARVIN_3_V2 upgrade
#define MARVIN_3_V22_R13         4  //!< MARVIN_3_V2 upgrade + MI patch from 12/2006
#define MARVIN_3_V22_R15         5  //!< MARVIN_3_V2 upgrade + MI patch from 12/2006 (package tag 15.01.07)
#define MARVIN_3_V42_R10         8  //!< MARVIN_3_V4 upgrade
#define MARVIN_3_V42_R14         9  //!< MARVIN_3_V4 upgrade + MI patch from 12/2006
#define MARVIN_3_V42_R15        10  //!< MARVIN_3_V4 upgrade + MI patch from 12/2006 + (package tag 15.01.07)
#define MARVIN_3_V5_R12         11  //!< Codename: "EMP"
#define MARVIN_3_V6_R10         12  //!< successor of MARVIN_5_V7

#define MARVIN_5_V5_R17         20  //!< successor of MARVIN_3_V5
#define MARVIN_5_V52_R10        21  //!< successor of MARVIN_5_V5
#define MARVIN_5_V52_R11        31  //!< successor of MARVIN_5_V5
#define MARVIN_5_V52_R12        33  //!< successor of MARVIN_5_V5
#define MARVIN_5_V53_R10        32  //!< MARVIN 5v52 (without Bayer)
#define MARVIN_5_V54_R10        41  //!< successor of MARVIN_5_V5
#define MARVIN_5_V1_R10         22  //!< MARVIN 5 (product)
#define MARVIN_5_V3_R12         24  //!< MARVIN 5 (product with Chromatic Aberration)
#define MARVIN_5_V4_R11         25  //!< MARVIN 5 (with 16 beat burst)
#define MARVIN_5_V6_R12         27  //!< MARVIN 5
#define MARVIN_5_V6_R13         35  //!< MARVIN 5
#define MARVIN_5_V6_R16         43  //!< MARVIN 5
#define MARVIN_5_V62_R10        44  //!< MARVIN 5
#define MARVIN_5_V7_R10         28  //!< MARVIN 5 (without Bayer)
#define MARVIN_5_V7_R11         36  //!< MARVIN 5 (without Bayer)
#define MARVIN_5_V2_R20         29  //!< MARVIN 5 base
#define MARVIN_5_V2_R21         34  //!< MARVIN 5 base
#define MARVIN_5_V2_R22         37  //!< MARVIN 5 base
#define MARVIN_5_V2_R30         38  //!< MARVIN 5 base
#define MARVIN_5_V2_R40         42  //!< MARVIN 5 base
#define MARVIN_5_V4_R20         30  //!< MARVIN 5 (with 16 beat burst)
#define MARVIN_5_V4_R21         40  //!< MARVIN 5 (with 16 beat burst)

#define MARVIN_12_V1_R12        60  //!< MARVIN 12MP
#define MARVIN_12_V1_R20        61  //!< MARVIN 12MP
#define MARVIN_12_V1_R21        62  //!< MARVIN 12MP
#define MARVIN_12_V2_R10        63  //!< MARVIN 12MP

#define MARVIN_HW               MARVIN_12_V2_R10 //#*#PORT was MARVIN_5_V2_R30   //!< Currently used MARVIN version
//@}



/*********************************************************************
 * MRV_SUPPORT_xxx
 * Some compile-time-configurable features of the MARVIN driver.
 * Set the certain defines to a non-zero value to enable the feature
 *********************************************************************/

/*!
 * routines to convert state and configuration enums into human readable
 * text. (useful in e.g. debug outputs)
 */
///////////////////////////////////////////////////////////////////////////////////
//#define MRV_SUPPORT_STATE2STRING   1

/*!
 * routines to read, write and dump the register set of the MARVIN module
 */
/////////////////////////////////////////////////////////////////////////////////// 
//#define MRV_SUPPORT_REGDUMP   1

/*!
 * second level support routines for e.g. exposure control, auto focus
 * and white balance. Second level support routines are
 * those using the plain routines of mrv.h to implement a kind of
 * "helper" to program/access/use the MARVIN with a bit more
 * abstraction.
 */
///////////////////////////////////////////////////////////////////////////////////
//#define MRV_SUPPORT_SL   1



/*********************************************************************/
/*!
 * \mainpage MARVIN SOFTWARE
 * \b File: project_settings_mrv.h
 *
 * <!-- \section Global Descriptions and Informations (settings, definitions, software changes) -->
 *
 * For global descriptions and informations see under "Modules"
 *
 */
/*********************************************************************/

/*********************************************************************/
/*!
 * \addtogroup MARVIN_DEFINES_00 MARVIN Features
 * \b File: project_settings_mrv.h
 *
 * \par MARVIN Features
 * Depends on the used MARVIN_HW. Direct usage of MARVIN_HW should be
 * avoided wherever possible. \n
 * This makes it VERY MUCH easier to adapt the driver to new MARVIN
 * versions with a feature set suited to a certain customer. \n
 *
 * \par MARVIN_FEATURE_CHIP_ID (integer)
 *    ID value contained in the chip. This is to be able to identify
 *    the chip derivative during runtime of the software \n
 *
 * \par MARVIN_FEATURE_CAMBUSWIDTH: (integer)
 *    How many bits can be captured from the image sensor? \n
 *    MARVIN_FEATURE_8BITS  = sensor bus width is 8 bits \n
 *    MARVIN_FEATURE_10BITS = sensor bus width is 10 bits \n
 *    MARVIN_FEATURE_12BITS = sensor bus width is 12 bits \n
 *
 * \par MARVIN_FEATURE_XTALK: (integer)
 *    color correction/crosstalk matrix \n
 *    MARVIN_FEATURE_XTALK_9BITS  = coefficient range -2.0 ... +1.992 ( 9 Bit), obsolete, not used in the software anymore \n
 *    MARVIN_FEATURE_XTALK_10BITS = coefficient range -4.0 ... +3.992 (10 Bit) \n
 *    MARVIN_FEATURE_XTALK_11BITS = coefficient range -8.0 ... +7.992 (11 Bit) \n
 *    MARVIN_FEATURE_EXIST_NOT    = no xtalk matrix at all \n
 *
 * \par MARVIN_FEATURE_XTALK_OFFSET: (boolean)
 *    add an offset to the crosstalk matrix output
 *
 * \par MARVIN_FEATURE_GAMMAIN: (integer)
 *    Gamma correction/sensor linearisation at the input of the ISP block. \n
 *    MARVIN_FEATURE_GAMMAIN_10BITS = module output is 10 bits (= ISP precision of 10 bit) \n
 *    MARVIN_FEATURE_GAMMAIN_12BITS = module output is 12 bits (= ISP precision of 12 bit) \n
 *    MARVIN_FEATURE_EXIST_NOT      = no gamma in block \n
 *
 * \par MARVIN_FEATURE_GAMMAOUT: (boolean)
 *    Gamma correction at the output of the ISP block. \n
 *
 * \par MARVIN_FEATURE_FRAMESIZE: (integer)
 *    Maximum frame size (at sensor input) MARVIN can handle. \n
 *    MARVIN_FEATURE_1M9 = 1.9 megapixels \n
 *    MARVIN_FEATURE_3M1 = 3.1 megapixels \n
 *    MARVIN_FEATURE_5M3 = 5.3 megapixels \n
 *
 * \par MARVIN_FEATURE_SSCALE: (boolean)
 *    Selfpath feature, and therefore selfpath scaler feautel also. If set to
 *    MARVIN_FEATURE_EXIST_NOT, the whole self data path is not present. \n
 *
 * \par MARVIN_FEATURE_SSCALE_UP: (boolean)
 *    Upscale capability of the self path scaler. This feature enables
 *    the scaler to do upscaling up to the factor of 5 \n
 *
 * \par MARVIN_FEATURE_SSCALE_FACTORCALC: (integer)
 *    Specifies the strategy to calculate the scale factors for the self scaler.
 *    Note that this define is the successor of the MARVIN_FEATURE_SSCALE_LANES,
 *    which does not longer reflect the 'true' implementations of the scaler hardware
 *    and therefore has been removed. \n
 *    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV = 'traditional' behaviour: The scaler
 *      handles the U and V chroma components as if they were two joined pixels. Thus,
 *      when YUV422 subsampled input data is to be processed and no further subsampling
 *      is required, the scale facrors of luma and chroma pathes must be identical. \n
 *    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV = 'new style' behaviour: The scaler
 *      handles the U and V chroma components as if they belong to totally different
 *      picture planes. Thus, when YUV422 subsampled input data is to be processed
 *      and no further subsampling is required, the scale facrors of the chroma path
 *      must be calculated like those of the luma path, but with only half of the image
 *      width. \n
 * \par MARVIN_FEATURE_SSCALE_RES: (integer)
 *    Output resolution of the self path scaler
 *    MARVIN_FEATURE_SSCALE_RES_VGA  = 640x480
 *    MARVIN_FEATURE_SSCALE_RES_720P = 1280x720
 *
 * \par MARVIN_FEATURE_MSCALE_UP: (boolean)
 *    Upscale capability of the main path scaler. This feature enables
 *    the scaler to do upscaling up to the factor of 5 \n
 *
 * \par MARVIN_FEATURE_MSCALE_FACTORCALC: (integer)
 *    Specifies the strategy to calculate the scale factors for the main scaler. \n
 *    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV = 'traditional' behaviour: The scaler
 *      handles the U and V chroma components as if they were two joined pixels. Thus,
 *      when YUV422 subsampled input data is to be processed and no further subsampling
 *      is required, the scale facrors of luma and chroma pathes must be identical. \n
 *    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV = 'new style' behaviour: The scaler
 *      handles the U and V chroma components as if they belong to totally different
 *      picture planes. Thus, when YUV422 subsampled input data is to be processed
 *      and no further subsampling is required, the scale facrors of the chroma path
 *      must be calculated like those of the luma path, but with only half of the image
 *      width. \n
 *
 * \par MARVIN_FEATURE_SCALE_FACTORWIDTH: (integer)
 *    Width of the scalefactors for both main and self scaler \n
 *    MARVIN_FEATURE_12BITS = 12 bits precision
 *    MARVIN_FEATURE_14BITS = 14 bits precision
 *    MARVIN_FEATURE_16BITS = 16 bits precision
 *
 * \par MARVIN_FEATURE_AF_MEASURE: (boolean)
 *    Autofocus measurement block (attached to the demosaicing block) \n
 *
 * \par MARVIN_FEATURE_BAD_PIXEL: (boolean)
 *    Bad pixel detection/correction block \n
 *
 * \par MARVIN_FEATURE_BAD_PIXEL_WIDTH: (integer)
 *    Bad pixel detection/correction block register size\n
 *    MARVIN_FEATURE_10BITS     = 10 bits precision
 *    MARVIN_FEATURE_12BITS     = 12 bits precision
 *    MARVIN_FEATURE_EXIST_NOT  = no bad pixel detection/correction block \n
 *
 * \par MARVIN_FEATURE_BAD_PIXEL_RAM: (integer)
 *    Bad pixel table ram address size\n
 *    MARVIN_FEATURE_7BITS      = 128 entries
 *    MARVIN_FEATURE_9BITS      = 512 entries
 *    MARVIN_FEATURE_11BITS     = 2048 entries
 *    MARVIN_FEATURE_EXIST_NOT  = no bad pixel ram block \n
 *
 * \par MARVIN_FEATURE_SUPERIMPOSE: (boolean)
 *    Superimpose block, used to combine camera picture with a static
 *    one coming from the system memory (chroma keying) \n
 *
 * \par MARVIN_FEATURE_CHROM_ABERRATION: (boolean)
 *    Chromatic aberration block corrects color fringing
 *
 * \par MARVIN_FEATURE_IMAGE_EFFECTS: (integer)
 *    Image effects block, various modes like grayscale, sepia, emboss
 *    sketch, etc. \n
 *    MARVIN_FEATURE_IE_V1 = initial version
 *    MARVIN_FEATURE_IE_V2 = full range mode
 *    MARVIN_FEATURE_IE_V3 = sharpening filter
 *
 * \par MARVIN_FEATURE_LENS_SHADING: (boolean)
 *    Lens shading compensation block \n
 *
 * \par MARVIN_FEATURE_ISP_ERRORFLAGS: (boolean)
 *    Some registers containing more detailed error flags of the ISP.
 *    These may help during system integration. \n
 *
 * \par MARVIN_FEATURE_FRAMECOUNTER: (boolean)
 *    Frame counter register \n
 *
 * \par MARVIN_FEATURE_FLASH_LIGHT: (boolean)
 *    Support for frame-synchronized triggering of a LED or xenon based
 *    flashlight \n
 *
 * \par MARVIN_FEATURE_SHUTTER: (boolean)
 *    Support for driving an external mechanical shutter \n
 *
 * \par MARVIN_FEATURE_IMG_STABILIZATION: (integer)
 *    Support for digital image stabilization (=compensation against
 *    small movements) \n
 *    MARVIN_FEATURE_IMG_STABILIZATION_V1 = represents second output formatter at ISP output,
 *                                          no image stabilization functionality,
 *                                          located in the ISP bayer path only.\n
 *    MARVIN_FEATURE_IMG_STABILIZATION_V2 = represents image stabilization including output
 *                                          formatter, located in both bayer and YCbCr paths,
 *                                          but not in the raw data path. \n
 *    MARVIN_FEATURE_EXIST_NOT            = there is no output image stabilization \n
 *
 * \par MARVIN_FEATURE_ISP_HISTOGRAM: (integer)
 *    Histogram measurement block \n
 *    MARVIN_FEATURE_HIST_V1   = initial version with 8 bit \n
 *    MARVIN_FEATURE_HIST_V2   = expand to 16 bit precision \n
 *    MARVIN_FEATURE_HIST_V3   = 5x5 weight matrix measurement \n
 *    MARVIN_FEATURE_EXIST_NOT = no histogram block at all \n
 *
 * \par MARVIN_FEATURE_ISP_CSM: (boolean)
 *    Color space matrix. May not exist if there is no Bayer path at all.
 *
 * \par MARVIN_FEATURE_ISP_FILTER: (boolean)
 *    Additional blurring/sharpness filter \n
 *
 * \par MARVIN_FEATURE_SMIA: (integer)
 *    SMIA camera protocol version switch \n
 *    MARVIN_FEATURE_SMIA_MIPI_EMP = EMP version that contains just the the SMIA
 *                                   and MIPI application protocol with two embedded
 *                                   data areas\n
 *    MARVIN_FEATURE_SMIA_EMP      = EMP version that contains just the the SMIA
 *                                   application protocol \n
 *    MARVIN_FEATURE_SMIA_COMPLETE = SMIA module that contains the complete SMIA
 *                                   functionality. \n
 *    MARVIN_FEATURE_EXIST_NOT     = there is no SMIA module \n
 *
 * \par MARVIN_FEATURE_AUTO_EXPOSURE: (integer)
 *    measurement unit for automatic exposure control \n
 *    MARVIN_FEATURE_AUTO_EXPOSURE_V1 = first implemented auto-exposure algorithm version \n
 *    MARVIN_FEATURE_AUTO_EXPOSURE_V2 = second implemented auto-exposure algorithm version \n
 *    MARVIN_FEATURE_AUTO_EXPOSURE_V3 = third implemented auto-exposure algorithm version
 *                                      = V2 + 5x5 measurement windows support \n
 *    MARVIN_FEATURE_AUTO_EXPOSURE_V4 = fourth implemented auto-exposure algorithm version
 *                                      = V3 + RGB measurement support \n
 *    MARVIN_FEATURE_EXIST_NOT        = no automatic exposure control block at all \n
 *
 * \par MARVIN_FEATURE_MI_STATUSFLAGS: (boolean)
 *    MI status flags needed for debugging purposes \n
 *
 * \par MARVIN_FEATURE_MIPI: (integer)
 *    MIPI camera protocol block \n
 *    MARVIN_FEATURE_MIPI_V1 = first version \n
 *    MARVIN_FEATURE_MIPI_V2 = frame start count != 0, include extrabytes at the end \n
 *    MARVIN_FEATURE_MIPI_V3 = Performance upgrade, Short packet generic support \n
 *    MARVIN_FEATURE_MIPI_V4 = like version 2 + compressed mode \n
 *
 * \par MARVIN_FEATURE_SMALL_OUTUNIT: (boolean)
 *    A small output unit instead of MI module \n
 *
 * \par MARVIN_FEATURE_CLOCK_DOMAINS: (integer)
 *    MARVIN_CLOCK_DOMAINS_1 = One clock domain for the complete MARVIN. \n
 *    MARVIN_CLOCK_DOMAINS_2 = Two clock domains (Camera data clock and AHB clock) \n
 *
 * \par MARVIN_FEATURE_WB: (integer)
 *    measurement and correction unit for white balance \n
 *    MARVIN_FEATURE_WB_V1     = basic white balance block, obsolete, not used in the software anymore \n
 *    MARVIN_FEATURE_WB_V2     = like version 1, but Y_max added, obsolete, not used in the software anymore \n
 *    MARVIN_FEATURE_WB_V3     = like version 2, but green_diff_gain added, obsolete, not used in the software anymore \n
 *    MARVIN_FEATURE_WB_V4     = version 4 of the white balance block. Extended gain range 0..4,
 *       resolution 10 bit, separated green gains for red and blue rows of bayer pattern. \n
 *    MARVIN_FEATURE_WB_V5     = new RGB based measurements (instead YCbCr)
 *    MARVIN_FEATURE_EXIST_NOT = no white balance block at all \n
 *
 * \par MARVIN_FEATURE_OUTPUT_FORMATTER: (integer)
 *    position of the output formatter \n
 *    MARVIN_FEATURE_OUTPUT_FORMATTER_V1 = exists at ISP output (old style) \n
 *    MARVIN_FEATURE_OUTPUT_FORMATTER_V2 = exists at ISP input \n
 *    MARVIN_FEATURE_EXIST_NOT           = there is no output formatter, as
 *                                         the image stabilization contains
 *                                         already this functionality \n
 *
 * \par MARVIN_FEATURE_MI: (integer)
 *    MARVIN_FEATURE_MI_V1 = basic version \n
 *    MARVIN_FEATURE_MI_V2 = introducing self-path DMA read \n
 *    MARVIN_FEATURE_MI_V3 = self path DMA read, rotation, line stripe, 8 beat burst \n
 *
 * \par MARVIN_FEATURE_DMA_READ: (integer)
 *    MARVIN_FEATURE_DMA_READ_V1 = version 1 \n
 *    MARVIN_FEATURE_DMA_READ_V2 = version 2 \n
 *    MARVIN_FEATURE_DMA_READ_V3 = version 3 \n
 *    MARVIN_FEATURE_DMA_READ_V4 = read RGB Bayer to ISP\n
 *    MARVIN_FEATURE_EXIST_NOT   = there is no DMA read feature \n
 *
 * \par MARVIN_FEATURE_JPE_SIZE: (integer)
 *    MARVIN_FEATURE_JPE_SIZE_11BITS = 11 Bits for JPE_HSIZE and JPE_VSIZE, only Marvin1. \n
 *    MARVIN_FEATURE_JPE_SIZE_12BITS = 12 Bits for JPE_HSIZE and JPE_VSIZE, all MARVIN3. \n
 *    MARVIN_FEATURE_JPE_SIZE_13BITS = 13 Bits for JPE_HSIZE and JPE_VSIZE, all MARVIN5. \n
 *
 * \par MARVIN_FEATURE_BLACK_LEVEL: (integer)
 *    MARVIN_FEATURE_BLACK_LEVEL_V1 = version 1; basic    version with  8 Bit registers \n
 *    MARVIN_FEATURE_BLACK_LEVEL_V2 = version 2; extended version with 10 Bit registers \n
 *    MARVIN_FEATURE_BLACK_LEVEL_V3 = version 3; extended version with 12 Bit registers \n
 *    MARVIN_FEATURE_BLACK_LEVEL_V4 = version 4; advanced version with 2 independent measurement
 *                                               windows and signed values; 10 Bit registers \n
 *    MARVIN_FEATURE_BLACK_LEVEL_V5 = version 5; like version 4 with 12 Bit registers \n
 *    MARVIN_FEATURE_EXIST_NOT      = there is no BLS module at all \n
 *
 * \par MARVIN_FEATURE_DPMUX_YCSPLIT: (integer)
 *    MARVIN_FEATURE_YCS_V1 = traditional datapath setup; separate datapath for raw data,
 *                            y/c splitter does not support self path only mode \n
 *    MARVIN_FEATURE_YCS_V2 = version 2, raw data routed through main path,
 *                            y/c splitter supports self path only mode. \n
 *
 * \par MARVIN_FEATURE_DPMUX_MAINPATH: (integer)
 *    MARVIN_FEATURE_DPMUX_MAIN_V1 = Traditional mainpath muxer. No direct path from DMA-read
 *                                   to JPEG encoder, explicit RAW datapath to MI \n
 *    MARVIN_FEATURE_DPMUX_MAIN_V2 = new DPCL register settings, possibility to feed
 *                                   JPEG encoder directly via DMA-Read \n
 *
 * \par MARVIN_FEATURE_INPUT_AQUISITION: (integer)
 *    MARVIN_FEATURE_IAQU_V1 = Traditional version, supports following modes:
 *                               raw data mode,
 *                               raw picture according to ITU-R BT.601,
 *                               RGB Bayer according to ITU-R BT.601,
 *                               ITU-R BT601 (YCbCr data),
 *                               ITU-R BT656 (YCbCr data)
 *    MARVIN_FEATURE_IAQU_V2 = Additional modes:
 *                               RGB Bayer according to ITU-R BT.656,
 *                               raw picture according to ITU-R BT.656
 *
 * \par MARVIN_FEATURE_JPE: (integer)
 *    MARVIN_FEATURE_JPE_V1 = Basic version
 *    MARVIN_FEATURE_JPE_V2 = Enable bit frame synchronization
 *    MARVIN_FEATURE_JPE_V3 = flags for Motion JPEG
 *
 * \par MARVIN_FEATURE_JPE_CFG: (integer)
 *    MARVIN_FEATURE_JPE_CFG_V1 = Basic version
 *    MARVIN_FEATURE_JPE_CFG_V2 = Scalado version
 *
 * \par MARVIN_FEATURE_EXT_YCBCR_RANGE: (boolean)
 *    ???
 *
 * \par MARVIN_FEATURE_SP_DMA: (boolean)
 *    ???
 *
 * \par MARVIN_FEATURE_BAYER_RGB: (boolean)
 *    Bayer RGB as input format allowed
 *
 * \par MARVIN_FEATURE_MI_BURST_16: (boolean)
 *    MARVIN_FEATURE_EXIST          = AHB 16 beat burst \n
 *    MARVIN_FEATURE_EXIST_NOT      = AHB burst to 8 or 4 is possible \n
 * \par MARVIN_FEATURE_MI_LAST_PIXEL: (boolean)
 *    last pixel signalization \n
 *
 * \par MARVIN_FEATURE_DPCC: (integer)
 *    Defect pixel cluster correction block \n
 *    MARVIN_FEATURE_DPCC_V1   = first version of DPCC, inofficially aka DPCC-light \n
 *    MARVIN_FEATURE_EXIST_NOT = no DPCC block at all \n
 *
 * \par MARVIN_FEATURE_DPF: (integer)
 *    Denoising Prefilter block \n
 *    MARVIN_FEATURE_DPF_V1   = first version of DPF \n
 *    MARVIN_FEATURE_EXIST_NOT = no DPF block at all \n
 *
 * \par MARVIN_FEATURE_ISP_ENABLE: (boolean)
 *    ISP starts with a written one to the isp_enable bit
 *
 * \par MARVIN_FEATURE_WDR: (integer)
 *    Denoising Prefilter block \n
 *    MARVIN_FEATURE_WDR_V1   = first version of WDR \n
 *    MARVIN_FEATURE_EXIST_NOT = no WDR block at all \n
 *    
 */
 /*********************************************************************/

//! \name Values for all boolean features
//@{
#define MARVIN_FEATURE_EXIST_NOT        (0)
#define MARVIN_FEATURE_EXIST            (1)
//@}

// \name Values for MARVIN_FEATURE_FRAMESIZE and MARVIN_FEATURE_MI_FRAMESIZE
//@{
#define MARVIN_FEATURE_1M9    1
#define MARVIN_FEATURE_3M1    2
#define MARVIN_FEATURE_5M3    3
#define MARVIN_FEATURE_12M6   4
//@}

// \name Values for MARVIN_FEATURE_CAMBUSWIDTH and MARVIN_FEATURE_SCALE_FACTORWIDTH
//@{
#define MARVIN_FEATURE_7BITS  7
#define MARVIN_FEATURE_8BITS  8
#define MARVIN_FEATURE_9BITS  9
#define MARVIN_FEATURE_10BITS 10
#define MARVIN_FEATURE_11BITS 11
#define MARVIN_FEATURE_12BITS 12
#define MARVIN_FEATURE_14BITS 14
#define MARVIN_FEATURE_16BITS 16
//@}

// \name Values for MARVIN_FEATURE_SMIA
//@{
#define MARVIN_FEATURE_SMIA_COMPLETE   1
#define MARVIN_FEATURE_SMIA_EMP        2
#define MARVIN_FEATURE_SMIA_MIPI_EMP   3
//@}

// \name Values for MARVIN_FEATURE_AUTO_EXPOSURE
//@{
#define MARVIN_FEATURE_AUTO_EXPOSURE_V1  1
#define MARVIN_FEATURE_AUTO_EXPOSURE_V2  2
#define MARVIN_FEATURE_AUTO_EXPOSURE_V3  3
#define MARVIN_FEATURE_AUTO_EXPOSURE_V4  4
//@}

// \name Values for MARVIN_FEATURE_MIPI
//@{
#define MARVIN_FEATURE_MIPI_V1          1
#define MARVIN_FEATURE_MIPI_V2          2
#define MARVIN_FEATURE_MIPI_V3          3
#define MARVIN_FEATURE_MIPI_V4          4
//@}

// \name Values for MARVIN_FEATURE_CLOCK_DOMAINS
//@{
#define MARVIN_CLOCK_DOMAINS_1          1
#define MARVIN_CLOCK_DOMAINS_2          2
//@}

// \name Values for MARVIN_FEATURE_WB: (integer)
//@{
//#define MARVIN_FEATURE_WB_V1  1 obsolete
//#define MARVIN_FEATURE_WB_V2  2 obsolete
//#define MARVIN_FEATURE_WB_V3  3 obsolete
#define MARVIN_FEATURE_WB_V4  4
#define MARVIN_FEATURE_WB_V5  5
//@}

// \name Values for MARVIN_FEATURE_XTALK: (integer)
//@{
//#define MARVIN_FEATURE_XTALK_9BITS   2 // coefficient range -2.0 ... +1.992 ( 9 Bit), obsolete
#define MARVIN_FEATURE_XTALK_10BITS  3 // coefficient range -4.0 ... +3.992 (10 Bit)
#define MARVIN_FEATURE_XTALK_11BITS  4 // coefficient range -8.0 ... +7.992 (11 Bit)
//@}

// \name Values for MARVIN_FEATURE_GAMMAIN: (integer)
//@{
#define MARVIN_FEATURE_GAMMAIN_10BITS 1 //module output is 10 bits (= ISP precision of 10 bit)
#define MARVIN_FEATURE_GAMMAIN_12BITS 2 //module output is 12 bits (= ISP precision of 12 bit)
//@}

// \name Values for MARVIN_FEATURE_OUTPUT_FORMATTER: (integer)
//@{
#define MARVIN_FEATURE_OUTPUT_FORMATTER_V1  (2)
#define MARVIN_FEATURE_OUTPUT_FORMATTER_V2  (3)
//@}

// \name Values for MARVIN_FEATURE_IMG_STABILIZATION: (integer)
//@{
#define MARVIN_FEATURE_IMG_STABILIZATION_V1     (2)
#define MARVIN_FEATURE_IMG_STABILIZATION_V2     (3)
//@}

// \name Values for MARVIN_FEATURE_SSCALE_FACTORCALC and MARVIN_FEATURE_MSCALE_FACTORCALC: (integer)
//@{
#define MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV  (2)
#define MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV (3)
//@}

// \name Values for MARVIN_FEATURE_SSCALE_RES: (integer)
//@{
#define MARVIN_FEATURE_SSCALE_RES_VGA  (2)
#define MARVIN_FEATURE_SSCALE_RES_720P (3)
//@}

// \name Values for MARVIN_FEATURE_MI: (integer)
//@{
#define MARVIN_FEATURE_MI_V1    (2)
#define MARVIN_FEATURE_MI_V2    (3)
#define MARVIN_FEATURE_MI_V3    (4)
#define MARVIN_FEATURE_MI_V4    (5)
//@}

// \name Values for MARVIN_FEATURE_DMA_READ: (integer)
//@{
#define MARVIN_FEATURE_DMA_READ_V1    (2)
#define MARVIN_FEATURE_DMA_READ_V2    (3)
#define MARVIN_FEATURE_DMA_READ_V3    (4)
#define MARVIN_FEATURE_DMA_READ_V4    (5)
//@}

// \name Values for MARVIN_FEATURE_JPE_SIZE: (integer)
//@{
#define MARVIN_FEATURE_JPE_SIZE_11BITS 1
#define MARVIN_FEATURE_JPE_SIZE_12BITS 2
#define MARVIN_FEATURE_JPE_SIZE_13BITS 3
//@}

// \name Values for MARVIN_FEATURE_BLACK_LEVEL: (integer)
//@{
#define MARVIN_FEATURE_BLACK_LEVEL_V1   (2)
#define MARVIN_FEATURE_BLACK_LEVEL_V2   (3)
#define MARVIN_FEATURE_BLACK_LEVEL_V3   (4)
#define MARVIN_FEATURE_BLACK_LEVEL_V4   (5)
#define MARVIN_FEATURE_BLACK_LEVEL_V5   (6)
//@}

// \name Values for MARVIN_FEATURE_DPMUX_YCSPLIT: (integer)
//@{
#define MARVIN_FEATURE_YCS_V1 1
#define MARVIN_FEATURE_YCS_V2 2
//@}

// \name Values for MARVIN_FEATURE_DPMUX_MAINPATH: (integer)
//@{
#define MARVIN_FEATURE_DPMUX_MAIN_V1 1
#define MARVIN_FEATURE_DPMUX_MAIN_V2 2
//@}

// \name Values for MARVIN_FEATURE_INPUT_AQUISITION: (integer)
//@{
#define MARVIN_FEATURE_IAQU_V1 1
#define MARVIN_FEATURE_IAQU_V2 2
//@}

// \name Values for MARVIN_FEATURE_JPE: (integer)
//@{
#define MARVIN_FEATURE_JPE_V1   (2)
#define MARVIN_FEATURE_JPE_V2   (3)
#define MARVIN_FEATURE_JPE_V3   (4)

// \name Values for MARVIN_FEATURE_JPE_CFG: (integer)
//@{
#define MARVIN_FEATURE_JPE_CFG_V1   (2)
#define MARVIN_FEATURE_JPE_CFG_V2   (3)
//@}

// \name Values for MARVIN_FEATURE_ISP_FILTER: (integer)
//@{
#define MARVIN_FEATURE_ISP_FILTER_V1   1
#define MARVIN_FEATURE_ISP_FILTER_V2   2
//@}

// \name Values for MARVIN_FEATURE_LENS_SHADING: (integer)
//@{
#define MARVIN_FEATURE_LSC_V1   1
#define MARVIN_FEATURE_LSC_V2   2
//@}

// \name Values for MARVIN_FEATURE_HISTOGRAM: (integer)
//@{
#define MARVIN_FEATURE_HIST_V1   1
#define MARVIN_FEATURE_HIST_V2   2
#define MARVIN_FEATURE_HIST_V3   3
//@}

// \name Values for MARVIN_FEATURE_IMAGE_EFFECTS: (integer)
//@{
#define MARVIN_FEATURE_IE_V1     (2)
#define MARVIN_FEATURE_IE_V2     (3)
#define MARVIN_FEATURE_IE_V3     (4)
//@}

// \name Values for MARVIN_FEATURE_DPCC: (integer)
//@{
#define MARVIN_FEATURE_DPCC_V1  (2)
//@}

// \name Values for MARVIN_FEATURE_DPF: (integer)
//@{
#define MARVIN_FEATURE_DPF_V1  (2)
//@}

// \name Values for MARVIN_FEATURE_WDR: (integer)
//@{
#define MARVIN_FEATURE_WDR_V1  (2)
//@}

#if(MARVIN_HW == MARVIN_3_V22_R11)
    #define MARVIN_FEATURE_CHIP_ID              0x0065300F
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_10BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V1
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V1
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V2
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_12BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V2
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V1
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_3_V22_R13)
    #define MARVIN_FEATURE_CHIP_ID              0x1065300F
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_10BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V1
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V1
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V2
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_12BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V2
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V1
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_3_V22_R15)
    #define MARVIN_FEATURE_CHIP_ID              0x2065300F
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_10BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V1
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V1
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V2
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_12BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V2
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V1
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_3_V42_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x0075300F
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_10BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V1
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V1
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V2
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_12BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V2
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V1
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_3_V42_R14)
    #define MARVIN_FEATURE_CHIP_ID              0x1075300F
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_10BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V1
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V1
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V2
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_12BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V2
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V1
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_3_V42_R15)
    #define MARVIN_FEATURE_CHIP_ID              0x2075300F
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_10BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V1
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V1
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V2
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_12BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V2
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V1
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_3_V5_R12)
    #define MARVIN_FEATURE_CHIP_ID              0x1055300F   // m3_v5 designs prior to 08/2006 use chip ID 0x0055300F
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_10BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_10BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V1
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V1
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V1
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_EMP
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V2
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V1
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_12BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V2
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V1
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V1
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V1
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_3M1
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M3_v5\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_3_V6_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x0015301E
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_3M1
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M3_v6\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V5_R17)
    #define MARVIN_FEATURE_CHIP_ID              0x10153016
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V1
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_MIPI_EMP
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V2
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V1
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V1
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v5\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V52_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x00253016
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_MIPI_EMP
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V1
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V1
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V2
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v5\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V52_R11)
    #define MARVIN_FEATURE_CHIP_ID              0x10253016
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_MIPI_EMP
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V1
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V1
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V2
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v5\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V52_R12)
    #define MARVIN_FEATURE_CHIP_ID              0x20253016
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_MIPI_EMP
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V1
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V2
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v5\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V53_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x00353016
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_COMBINED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_MIPI_EMP
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V1
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V1
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V2
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v53\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V54_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x00453016
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V3
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_MIPI_EMP
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V1
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V1
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V2
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v5\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V1_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x00153010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_7BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V1
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V1
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V1
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V2
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V3
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V2
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V3_R12)
    #define MARVIN_FEATURE_CHIP_ID              0x20353010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v3\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V4_R11)
    #define MARVIN_FEATURE_CHIP_ID              0x10453010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V1
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V2
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V2
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V3
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v4\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V6_R12)
    #define MARVIN_FEATURE_CHIP_ID              0x00553010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v6\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V6_R13)
    #define MARVIN_FEATURE_CHIP_ID              0x10553010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V2
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v6\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT    

#elif(MARVIN_HW == MARVIN_5_V6_R16)
    #define MARVIN_FEATURE_CHIP_ID              0x20553010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V2
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V3
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v6\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT
    
#elif(MARVIN_HW == MARVIN_5_V62_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x00853010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_DPCC_V1
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V3
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V4
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v62\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_DPF_V1
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_WDR_V1
    
#elif(MARVIN_HW == MARVIN_5_V7_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x00653010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v7\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V7_R11)
    #define MARVIN_FEATURE_CHIP_ID              0x10653010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V2
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v7\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V2_R20)
    #define MARVIN_FEATURE_CHIP_ID              0x00753010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v2\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V2_R21)
    #define MARVIN_FEATURE_CHIP_ID              0x10753010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v2\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V2_R22)
    #define MARVIN_FEATURE_CHIP_ID              0x20753010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V2
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v2\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V2_R30)
    #define MARVIN_FEATURE_CHIP_ID              0x00853010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_9BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V3
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v2\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V2_R40)
    #define MARVIN_FEATURE_CHIP_ID              0x00953010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_720P
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_DPCC_V1
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V3
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V3
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V4
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_5M3
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v2\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_DPF_V1
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_WDR_V1

#elif(MARVIN_HW == MARVIN_5_V4_R20)
    #define MARVIN_FEATURE_CHIP_ID              0x20453010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v4\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_5_V4_R21)
    #define MARVIN_FEATURE_CHIP_ID              0x30453010
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_5M3
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_14BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V2
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M5_v4\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_12_V1_R12)
    #define MARVIN_FEATURE_CHIP_ID              0x10153017
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_12M6
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_16BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V1
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V2
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V4
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V3
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M12_v1\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_12_V1_R20)
    #define MARVIN_FEATURE_CHIP_ID              0x20153017
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_12M6
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_16BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V1
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V2
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M12_v1\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_12_V1_R21)
    #define MARVIN_FEATURE_CHIP_ID              0x30153017
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_12M6
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_VGA
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_16BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V2
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_SMIA_COMPLETE
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V1
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M12_v1\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_EXIST_NOT

#elif(MARVIN_HW == MARVIN_12_V2_R10)
    #define MARVIN_FEATURE_CHIP_ID              0x00253017
    #define MARVIN_FEATURE_CAMBUSWIDTH          MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_XTALK                MARVIN_FEATURE_XTALK_11BITS
    #define MARVIN_FEATURE_GAMMAIN              MARVIN_FEATURE_GAMMAIN_12BITS
    #define MARVIN_FEATURE_GAMMAOUT             MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMESIZE            MARVIN_FEATURE_12M6
    #define MARVIN_FEATURE_SP_DMA               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE               MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SSCALE_RES           MARVIN_FEATURE_SSCALE_RES_720P
    #define MARVIN_FEATURE_MSCALE_UP            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MSCALE_FACTORCALC    MARVIN_FEATURE_SCALEFACTOR_SEPARATED_UV
    #define MARVIN_FEATURE_SCALE_FACTORWIDTH    MARVIN_FEATURE_16BITS
    #define MARVIN_FEATURE_AF_MEASURE           MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_BAD_PIXEL            MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_BAD_PIXEL_WIDTH      MARVIN_FEATURE_12BITS
    #define MARVIN_FEATURE_BAD_PIXEL_RAM        MARVIN_FEATURE_11BITS
    #define MARVIN_FEATURE_DPCC                 MARVIN_FEATURE_DPCC_V1
    #define MARVIN_FEATURE_SUPERIMPOSE          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMAGE_EFFECTS        MARVIN_FEATURE_IE_V3
    #define MARVIN_FEATURE_LENS_SHADING         MARVIN_FEATURE_LSC_V2
    #define MARVIN_FEATURE_ISP_ERRORFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FRAMECOUNTER         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_FLASH_LIGHT          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_EXT_YCBCR_RANGE      MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_SHUTTER              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_IMG_STABILIZATION    MARVIN_FEATURE_IMG_STABILIZATION_V2
    #define MARVIN_FEATURE_ISP_HISTOGRAM        MARVIN_FEATURE_HIST_V3
    #define MARVIN_FEATURE_ISP_CSM              MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_FILTER           MARVIN_FEATURE_ISP_FILTER_V2
    #define MARVIN_FEATURE_SMIA                 MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_AUTO_EXPOSURE        MARVIN_FEATURE_AUTO_EXPOSURE_V4
    #define MARVIN_FEATURE_MI_STATUSFLAGS       MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MIPI                 MARVIN_FEATURE_MIPI_V3
    #define MARVIN_FEATURE_SMALL_OUTUNIT        MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_CLOCK_DOMAINS        MARVIN_CLOCK_DOMAINS_1
    #define MARVIN_FEATURE_WB                   MARVIN_FEATURE_WB_V5
    #define MARVIN_FEATURE_OUTPUT_FORMATTER     MARVIN_FEATURE_OUTPUT_FORMATTER_V2
    #define MARVIN_FEATURE_MI                   MARVIN_FEATURE_MI_V4
    #define MARVIN_FEATURE_DMA_READ             MARVIN_FEATURE_DMA_READ_V3
    #define MARVIN_FEATURE_JPE_SIZE             MARVIN_FEATURE_JPE_SIZE_13BITS
    #define MARVIN_FEATURE_BLACK_LEVEL          MARVIN_FEATURE_BLACK_LEVEL_V5
    #define MARVIN_FEATURE_DPMUX_YCSPLIT        MARVIN_FEATURE_YCS_V2
    #define MARVIN_FEATURE_DPMUX_MAINPATH       MARVIN_FEATURE_DPMUX_MAIN_V2
    #define MARVIN_FEATURE_INPUT_AQUISITION     MARVIN_FEATURE_IAQU_V2
    #define MARVIN_FEATURE_JPE                  MARVIN_FEATURE_JPE_V3
    #define MARVIN_FEATURE_JPE_CFG              MARVIN_FEATURE_JPE_CFG_V1   
    #define MARVIN_FEATURE_XTALK_OFFSET         MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_CHROM_ABERRATION     MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_BURST_16          MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_LAST_PIXEL        MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_MI_FRAMESIZE         MARVIN_FEATURE_12M6
    #define MARVIN3VALI_PICTURE_PATH  "..\\..\\CHECK\\M12_v2\\ref\\" //for the *.crc files
    #define MARVIN_FEATURE_BAYER_RGB            MARVIN_FEATURE_EXIST
    #define MARVIN_FEATURE_ISP_ENABLE           MARVIN_FEATURE_EXIST_NOT
    #define MARVIN_FEATURE_DPF                  MARVIN_FEATURE_DPF_V1
    #define MARVIN_FEATURE_WDR                  MARVIN_FEATURE_WDR_V1

#endif  // MARVIN_HW

#endif//_PROJECT_SETTTINGS_H
