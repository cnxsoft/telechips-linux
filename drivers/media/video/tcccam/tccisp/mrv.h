/*****************************************************************************/
/*!
 *  \file        mrv.h \n
 *  \version     1.1 \n
 *  \author      Friebe \n
 *  \brief       Public header for the Marvin driver \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_H
#define _MRV_H


/******************************************************************************
 * DEFINES
 ******************************************************************************/
// MARVIN VI ID defines -> changed to MARVIN_FEATURE_CHIP_ID and moved to
// the other chip features in project_settings.h


// JPEG compression ratio defines
#define JPEG_HIGH_COMPRESSION    1
#define JPEG_LOW_COMPRESSION     2
#define JPEG_01_PERCENT          3  // Low Compression / High Quality
#define JPEG_20_PERCENT          4
#define JPEG_30_PERCENT          5
#define JPEG_40_PERCENT          6
#define JPEG_50_PERCENT          7  // Mid Compression / Mid Quality
#define JPEG_60_PERCENT          8
#define JPEG_70_PERCENT          9
#define JPEG_80_PERCENT         10
#define JPEG_90_PERCENT         11
#define JPEG_99_PERCENT         12  // High Compression / Low Quality

#define MRV_DATA_TBL_SIZE      289  // Size of lens shading data table in 16 Bit words
#define MRV_GRAD_TBL_SIZE        8  // Size of lens shading grad table in 16 Bit words
#define MRV_MAX_LSC_SECTORS     16  // Number of lens shading sectors in x or y direction
#define LSC_FILENAME_BUFSZ      50  // Size of LSC configuration file name buffer
#define LSC_FILENAME_ARRAYSZ    32  // Size of Array of LSC configuration file name

#define MRV_FIXEDPOINT_ONE   (0x1000)  // Value representing 1.0 for fixed-point values
                                       // used by marvin drivers

/******************************************************************************
 * TYPEDEFS
 ******************************************************************************/
//! JPEG encoding
typedef enum
{
#if (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)
   eMrvJpe_ScaladoMode   = 0x08,  // single snapshot with Scalado encoding
#endif //(MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2)
   eMrvJpe_LargeContMode = 0x04,  // motion JPEG with header generation
   eMrvJpe_ShortContMode = 0x02,  // motion JPEG only first frame with header
   eMrvJpe_SingleShot    = 0x01   // JPEG with single snapshot
} teMrvJpeEncMode;

//! for demosaic mode
typedef enum
{
    eMrvDemosaicStandard,
    eMrvDemosaicEnhanced
} teMrvDemosaicMode;

/*****************************************************************************/

//! general purpose window
typedef tsIsiWindow tsMrvWindow;

/*****************************************************************************/

//! scale settings for both self and main resize unit
typedef struct
{
    UINT32 ulScaleHY;
    UINT32 ulScaleHCB;
    UINT32 ulScaleHCR;
    UINT32 ulScaleVY;
    UINT32 ulScaleVC;
    UINT16 usPhaseHY;
    UINT16 usPhaseHC;
    UINT16 usPhaseVY;
    UINT16 usPhaseVC;
} tsMrvScale;

//! A Lookup table for the upscale parameter in the self and main scaler
typedef struct
{
    UINT8   ucMrvRszLut[64];
} tsMrvRszLut;

#if (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_16BITS)
    #define RSZ_UPSCALE_ENABLE 0x20000  //<! flag to set in scalefactor values to enable upscaling
#else
#define RSZ_UPSCALE_ENABLE 0x8000  //<! flag to set in scalefactor values to enable upscaling
#endif // #if (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_16BITS)

/*! Flag to set in scalefactor values to bypass the scaler block. \n
 * Since this define is also used in calculations of scale factors and coordinates,
 * it needs to reflect the scale factor precision. In other words: \n
 * RSZ_SCALER_BYPASS = <max. scalefactor value> + 1
 */
#if (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_12BITS)
    #define RSZ_SCALER_BYPASS  0x1000
#elif (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_14BITS)
    #define RSZ_SCALER_BYPASS  0x4000
#elif (MARVIN_FEATURE_SCALE_FACTORWIDTH == MARVIN_FEATURE_16BITS)
    #define RSZ_SCALER_BYPASS  0x10000
#endif


/*****************************************************************************/

//! color settings
typedef struct
{
    UINT8  ucContrast;
    UINT8  ucBrightness;
    UINT8  ucSaturation;
    UINT8  ucHue;
    UINT32 ulFlags;
}tsMrvColorSettings;


#define MRV_CPROC_C_OUT_RANGE       0x08 // Color processing chrominance clipping range
#define MRV_CPROC_Y_IN_RANGE        0x04 // Color processing luminance input range (offset processing)
#define MRV_CPROC_Y_OUT_RANGE       0x02 // Color processing luminance output clipping range
#define MRV_CPROC_ENABLE            0x01 // color processing enable

/*****************************************************************************/

//! black level config
typedef struct
{
    BOOL bBlsAuto;
    BOOL bHEnable;
    BOOL bVEnable;
    UINT16 usHStart;
    UINT16 usHStop;
    UINT16 usVStart;
    UINT16 usVStop;
    UINT8  ucBlcSamples;
    UINT8 ucRefA;
    UINT8 ucRefB;
    UINT8 ucRefC;
    UINT8 ucRefD;
} tsMrvBlcConfig;

//! black level compensation mean values
typedef struct
{
    UINT8 ucMeanA;
    UINT8 ucMeanB;
    UINT8 ucMeanC;
    UINT8 ucMeanD;
} tsMrvBlcMean;

// -----------------------------------------

//! BLS window
typedef struct
{
    BOOL   bEnableWindow;   //!< En-/disable the measurement window.
    UINT16 usStartH;        //!< Horizontal start address.
    UINT16 usStopH;         //!< Horizontal stop address.
    UINT16 usStartV;        //!< Vertical start address.
    UINT16 usStopV;         //!< Vertical stop address.
} tsMrvBlsWindow;

//! BLS mean measured values
typedef struct
{
    UINT16 usMeasA;         //!< Mean measured value for Bayer pattern position A.
    UINT16 usMeasB;         //!< Mean measured value for Bayer pattern position B.
    UINT16 usMeasC;         //!< Mean measured value for Bayer pattern position C.
    UINT16 usMeasD;         //!< Mean measured value for Bayer pattern position D.
} tsMrvBlsMeasured;

//! BLS fixed subtraction values. The values will be subtracted from the sensor values.
//! Therefore a negative value means addition instead of subtraction!
typedef struct
{
    INT16 sFixedA;          //!< Fixed (signed!) subtraction value for Bayer pattern position A.
    INT16 sFixedB;          //!< Fixed (signed!) subtraction value for Bayer pattern position B.
    INT16 sFixedC;          //!< Fixed (signed!) subtraction value for Bayer pattern position C.
    INT16 sFixedD;          //!< Fixed (signed!) subtraction value for Bayer pattern position D.
} tsMrvBlsSubtraction;

//! BLS configuration
typedef struct
{
    BOOL                bEnableAutomatic;       //!< Automatic mode activated means that the measured values are
                                                //!  subtracted. Otherwise the fixed subtraction values will be
                                                //!  subtracted.
    BOOL                bDisableH;              //!< En-/disable horizontal accumulation for mean black value.
                                                //!  BLS module versions 4 or higher imply that it is enabled.
    BOOL                bDisableV;              //!< En-/disable vertical accumulation for mean black value.
                                                //!  BLS module versions 4 or higher imply that it is enabled.
    tsMrvBlsWindow      tMrvBlsWindow1;         //!< Measurement window 1.
    tsMrvBlsWindow      tMrvBlsWindow2;         //!< Measurement window 2.
                                                //!  BLS module version 3 and lower do not support a second
                                                //!  measurement window. Therefore the second window has to
                                                //!  be disabled for these versions.
    UINT8               ucBlsSamples;           //!< Set amount of measured pixels for each Bayer position (A, B,
                                                //!  C and D) to 2^ucBlsSamples.
    tsMrvBlsSubtraction tMrvBlsSubtraction;     //!< Fixed subtraction values.
} tsMrvBlsConfig;

/*****************************************************************************/

//! white balancing modes for the marvin hardware
//! these modes are used for both, controlling the upper software layers
//! and the driver, the upper sw layers need a more detailed view of things
//! while the driver is only differentiation between module enabled/disabled
//! and measurement on/off
typedef enum
{
    eMrvAwbCompletelyOff = 0, //! turn the module off in isp_ctrl
    eMrvAwbAuto,              //! AWB       - measurement is on
    eMrvAwbManMeas,           //! manual WB - measurement is on
    eMrvAwbManNoMeas,         //! manual WB - measurement is off
    eMrvAwbManPushAuto,       //! manual WB, AWB while push button is pressed - measurement is on
    eMrvAwbOnlyMeas           //! only measurements, not even manual ctrl.    - measurement is on
} teMrvAwbMode;

//! white balancing modes for the marvin hardware
//  Note: Add a new profile on the end of the table.
//        In the other case all config files have to updated.
typedef enum
{
    eMrvAwbSubOff = 0,
    eMrvAwbAutoOn,
    eMrvAwbManDay,
    eMrvAwbManCloudy,
    eMrvAwbManShade,
    eMrvAwbManFluorcnt,
    eMrvAwbManFluorcntH,
    eMrvAwbManTungsten,
    eMrvAwbManTwilight,
    eMrvAwbManSunny,
    eMrvAwbManSunset,
    eMrvAwbManFlash,
    eMrvAwbManCieD65,
    eMrvAwbManCieD75,
    eMrvAwbManCieF11,
    eMrvAwbManCieF12,
    eMrvAwbManCieA,
    eMrvAwbManCieF2,
    eMrvAwbManCieD50
} teMrvAwbSubMode;

//! white balancing gains
typedef tsIsiComponentGain tsMrvWbGains;
//! dpf noise level gains
typedef tsIsiComponentGain tsMrvDpfNLGain;

//! near white discrimination
#define MEAS_MODE_NEAR_WHITE_DISC 0
//! RGB based measurement mode
#define MEAS_MODE_RGB             1

//! white balancing measurement configuration
//! note that there are two basically different hardware versions/measurement
//! modes, YCbCr Mode and RGB mode, with different register meanings
typedef struct
{
    tsMrvWindow tMrvAWBWindow;          //!< white balance measurement window (in pixels)
    UINT8       ucMaxY;                 //!< YCbCr Mode: only pixels values Y <= ucMaxY contribute to WB measurement (set to 0 to disable this feature)
                                        //!  RGB Mode  : unused
    UINT8       ucRefCr_MaxR;           //!< YCbCr Mode: Cr reference value
                                        //!  RGB Mode  : only pixels values R < MaxR contribute to WB measurement
    UINT8       ucMinY_MaxG;            //!< YCbCr Mode: only pixels values Y >= ucMinY contribute to WB measurement
                                        //!  RGB Mode  : only pixels values G < MaxG contribute to WB measurement
    UINT8       ucRefCb_MaxB;           //!< YCbCr Mode: Cb reference value
                                        //!  RGB Mode  : only pixels values B < MaxB contribute to WB measurement
    UINT8       ucMaxCSum;              //!< YCbCr Mode: chrominance sum maximum value, only consider pixels with Cb+Cr smaller than threshold for WB measurements
                                        //!  RGB Mode  : unused
    UINT8       ucMinC;                 //!< YCbCr Mode: chrominance minimum value, only consider pixels with Cb/Cr each greater than threshold value for WB measurements
                                        //!  RGB Mode  : unused
    UINT8       ucFrames;               //!< number of frames+1 used for mean value calculation (ucFrames=0 means 1 Frame)
    UINT8       ucMeasMode;             //!< 0:near white discrimination, YCbCr mode; 1:RGB mode
} tsMrvWbMeasConfig;

//! white balancing configuration
typedef struct
{
    // mode of operation
    teMrvAwbMode        eMrvWbMode;
    teMrvAwbSubMode     eMrvWbSubMode;
    // measurement configuration
    tsMrvWbMeasConfig   tMrvWbMeasConf;
    // gain values
    tsMrvWbGains        tMrvWbGains;
    // Threshold for deviation in percent of the current white value from the white value of the last frame below which AWB is suspended
    UINT8               ucAwbSuspendWhiteThresPerc;
    // Threshold for deviation in percent of the current white value from the last white value before suspension of AWB above which AWB is resumed
    UINT8               ucAwbRestartWhiteThresPerc;
    // minimum number of white pixels, increase of fRegionSize starts below
    UINT32              ulWhiteMin;
    // maximum number of white pixels, decrease of fRegionSize starts beyond
    UINT32              ulWhiteMax;
    //Near white pixel discrimination capture range
    FLOAT               fRegionSize;
    // increase value for near white pixel region adaption
    FLOAT               fRegionSizeIncrease;
    // decrease value for near white pixel region adaption
    FLOAT               fRegionSizeDecrease;
    // Minimal allowed Rg value for projected point under indoor conditions
    FLOAT               fRgProjIndoorMin;
    // Maximal allowed Rg value for projected point
    FLOAT               fRgProjMax;
    // Maximal allowed Rg value for blue sky scene
    FLOAT               fRgProjMaxSky;
    // Minimal allowed Rg value for projected point under outdoor conditions
    FLOAT               fRgProjOutdoorMin;
    //! a value in the intervall ]0,0.5[
    FLOAT               fDampingFilterThreshold;
    //! change the damping value in steps towards AWB_IIR_DAMPING_COEF_MIN
    FLOAT               fIIRDampingCoefSub;
    //! change the damping value in steps towards AWB_IIR_DAMPING_COEF_MAX
    FLOAT               fIIRDampingCoefAdd;
    //! minimum damping
    FLOAT               fIIRDampingCoefMin;
    //! maximum damping, e.g. 0.85 means damped value is composed by 15% of the new value and 85% of old damped value (lots of damping)
    FLOAT               fIIRDampingCoefMax;
    // PriorExp Damping for awb auto mode
    UINT8               ucAwbPriorExpDamping;
    // PriorExp Damping for AWB auto push mode
    UINT8               ucAwbPriorExpPushDamping;
    // Pca Damping for awb auto mode
    UINT8               ucAwbPcaDamping;
    // Pca Damping for AWB auto push mode
    UINT8               ucAwbPcaPushDamping;
    // Max Y in AWB auto mode
    UINT8               ucAwbAutoMaxY;
    // Max Y in AWB auto push mode
    UINT8               ucAwbPushMaxY;
    // Max Y in AWB measurement only mode
    UINT8               ucAwbMeasureMaxY;
} tsMrvWbConfig;

/*****************************************************************************/

//! possible AEC modes
typedef enum
{
    eMrvAec_off,       // AEC turned off
    eMrvAec_integral,  // AEC measurements based on (almost) the entire picture
    eMrvAec_spot,      // AEC measurements based on a single little square in the center of the picture
    eMrvAec_mfield5,   // AEC measurements based on 5 little squares spread over the picture
    eMrvAec_mfield9    // AEC measurements based on 9 little squares spread over the picture
} teMrvAecMode;

//! histogram weight 5x5 matrix coefficients (possible values are 1=0x10,15/16=0x0F,14/16,...,1/16,0)
typedef struct
{
   UINT8 ucWeight_00;   UINT8 ucWeight_10;   UINT8 ucWeight_20;   UINT8 ucWeight_30;   UINT8 ucWeight_40;
   UINT8 ucWeight_01;   UINT8 ucWeight_11;   UINT8 ucWeight_21;   UINT8 ucWeight_31;   UINT8 ucWeight_41;
   UINT8 ucWeight_02;   UINT8 ucWeight_12;   UINT8 ucWeight_22;   UINT8 ucWeight_32;   UINT8 ucWeight_42;
   UINT8 ucWeight_03;   UINT8 ucWeight_13;   UINT8 ucWeight_23;   UINT8 ucWeight_33;   UINT8 ucWeight_43;
   UINT8 ucWeight_04;   UINT8 ucWeight_14;   UINT8 ucWeight_24;   UINT8 ucWeight_34;   UINT8 ucWeight_44;
} tsMrvHistMatrix;



//! autoexposure config
typedef struct
{
// Size of 1 window of MARVIN's 5x5 mean luminance measurement grid and offset of grid
    tsMrvWindow tMrvAECMeanLumaWindow;
// Size and offset of histogram window
    tsMrvWindow tMrvAECHistCalcWindow;
// Current mean luminance objective
    FLOAT  rSetpoint;
// Current mean luminance objective determined by scene evaluation
    FLOAT  rSetpointBySceneEval;
// Control Tolerance %
    FLOAT rCtrlTolerance;
// Damping factor in case of overexposure
    FLOAT  rDampOver;
// Damping factor in case of underexposure
    FLOAT  rDampUnder;
// Current exposure setting
    FLOAT  rExposure;
// tolerance limit for AWB suspension
    FLOAT rAwbSuspTolerance;
// possible AEC modes
    teMrvAecMode eAdvancedAecMode;
} tsMrvAecConfig;


/*****************************************************************************/

//! vi_dpcl path selector, channel mode \n
//! Configuration of the Y/C splitter
typedef enum
{
    eMrvYcsY,     //8bit data/Y only output (depreciated, please use eMrvYcsMvRaw for new implementations)
    eMrvYcsMvSp,  //separated 8bit Y, C routed to both main and self path
    eMrvYcsMv,    //separated 8bit Y, C routed to main path only (self path input switched off)
    eMrvYcsSp,    //separated 8bit Y, C routed to self path only (main path input switched off)
    eMrvYcsMvRaw, //raw camera data routed to main path (8 or 16 bits, depends on marvin drivative)
    eMrvYcsOff    //both main and self path input switched off
} teMrvYcsChnMode;

//! vi_dpcl path selector, main path cross-switch
typedef enum
{
    eMrvDpRaw,  //raw data mode
    eMrvDpJpeg, //JPEG encoding mode
    eMrvDpMv    //main video path only
} teMrvDpSwitch;

//! DMA-read mode selector
typedef enum
{
    eMrvDmaRd_off         = 0, //!< DMA-read feature deactivated
    eMrvDmaRd_SelfPath    = 1, //!< data from the DMA-read block feeds the self path
    eMrvDmaRd_Superimpose = 2, //!< data from the DMA-read block feeds the Superimpose block
    eMrvDmaRd_IePath      = 3, //!< data from the DMA-read block feeds the Image effects path
    eMrvDmaRd_JpgEnc      = 4  //!< data from the DMA-read block feeds the JPEG encoder directly
} teMrvDmaReadMode;

//! ISP path selector
typedef enum
{
    eMrvIspPath_Unknown    = 0, //!< Isp path is unknown or invalid
    eMrvIspPath_Raw        = 1, //!< Raw data bypass
    eMrvIspPath_YCbCr      = 2, //!< YCbCr path
    eMrvIspPath_Bayer      = 3  //!< Bayer RGB path
} eMrvIspPath;


/*****************************************************************************/

//! possible autofocus measurement modes
typedef enum
{
    eMrvAfm_Off,             // no autofocus measurement
    eMrvAfm_HW,              // use AF hardware to measure sharpness
    eMrvAfm_SwTenengrad,     // use "Tenengrad" algorithm implemented in software
    eMrvAfm_SwTreshSqrtGrad, // use "Threshold Squared Gradient" algorithm implemented in software
    eMrvAfm_SwFSWMedian,     // use "Frequency selective weighted median" algorithm implemented in software
    eMrvAfm_HW_norm,         // use AF hardware and normalize with mean luminance
    eMrvAfm_SwTenengrad_norm,// use "Tenengrad" algorithm and normalize with mean luminance
    eMrvAfm_SwFSWMedian_norm // use "Frequency selective weighted median" algorithm and normalize with mean luminance
} teMrvAfmMode;


//! possible autofocus search strategy modes
typedef enum
{
    eMrvAfss_Off,          // no focus searching
    eMrvAfss_FullRange,    // scan the full focus range to find the point of best focus
    eMrvAfss_Hillclimbing, // use hillclimbing search
    eMrvAfss_AdaptiveRange,// similar to full range search, but with multiple subsequent scans with
                           // decreasing range and step size will be performed.
    eMrvAfss_HelimorphOpt, // search strategy suggested by OneLimited for their Helimorph actuator
    eMrvAfss_OV2630LPD4Opt // search strategy optimized for omnivision 2630 module equipped with
                           // autofocus lend driven through a LPD4 stepper motor produced by
                           // Nidec Copal USA Corp. of Torrance, CA.
} teMrvAfssMode;

/*****************************************************************************/

//! possible bad pixel correction type
typedef enum
{
    eMrvBpCorr_Table,      // correction of bad pixel from the table
    eMrvBpCorr_Direct      // direct detection and correction
} teMrvBpCorrType;

//! possible bad pixel replace approach
typedef enum
{
    eMrvBpCorr_RepNb,      // nearest neighbour approach
    eMrvBpCorr_RepLin      // simple bilinear interpolation approach
} teMrvBpCorrRep;

//! possible bad pixel correction mode
typedef enum
{
    eMrvBpCorr_HotEn,      // hot pixel correction
    eMrvBpCorr_DeadEn,     // dead pixel correction
    eMrvBpCorr_HotDeadEn  // hot and dead pixel correction
} teMrvBpCorrMode;

/*****************************************************************************/

//! Gamma out curve (independent from the sensor characteristic).
#define MRV_GAMMA_OUT_CURVE_ARR_SIZE (17)

typedef struct
{
    UINT16  usIspGammaY[MRV_GAMMA_OUT_CURVE_ARR_SIZE];
    UINT8   ucGammaSegmentation;
} tsMrvGammaOutCurve;

/*****************************************************************************/

//! configuration of autofocus measurement block
typedef struct
{
   tsMrvWindow tWndPosA;    // position and size of measurement window A
   tsMrvWindow tWndPosB;    // position and size of measurement window B
   tsMrvWindow tWndPosC;    // position and size of measurement window C
   UINT32      ulThreshold; // AF measurment threshold
   UINT32      ulVarShift;  // measurement variable shift (before sum operation)
} tsMrvAfConfig;

//! measurement results of autofocus measurement block
typedef struct
{
   UINT32  ulAfmSumA;    // sharpness value of window A
   UINT32  ulAfmSumB;    // sharpness value of window B
   UINT32  ulAfmSumC;    // sharpness value of window C
   UINT32  ulAfmLumA;    // luminance value of window A
   UINT32  ulAfmLumB;    // luminance value of window B
   UINT32  ulAfmLumC;    // luminance value of window C
} tsMrvAfMeas;

/*****************************************************************************/

//! configuration for correction of bad pixel block
typedef struct
{
   char*           pszBpTableFileName;  // Name of BP table file
   teMrvBpCorrType tBpCorrType;         // bad pixel correction type
   teMrvBpCorrRep  tBpCorrRep;          // replace approach
   teMrvBpCorrMode tBpCorrMode;         // bad pixel correction mode
   UINT16          usBpAbsHotThres;     // Absolute hot pixel threshold
   UINT16          usBpAbsDeadThres;    // Absolute dead pixel threshold
   UINT16          usBpDevHotThres;     // Hot Pixel deviation Threshold
   UINT16          usBpDevDeadThres;    // Dead Pixel deviation Threshold
} tsMrvBpCorrConfig;

/*****************************************************************************/

//! configuration for correction of lens shading
typedef struct
{
    char    *pszCfgFileName[LSC_FILENAME_ARRAYSZ]; //Name array of LSC configuration file
    UINT16  ulLsRDataTbl[MRV_DATA_TBL_SIZE];   // correction values of R color part
    UINT16  ulLsGDataTbl[MRV_DATA_TBL_SIZE];   // correction values of G color part
    UINT16  ulLsBDataTbl[MRV_DATA_TBL_SIZE];   // correction values of B color part
    UINT16  ulLsXGradTbl[MRV_GRAD_TBL_SIZE];   // multiplication factors of x direction
    UINT16  ulLsYGradTbl[MRV_GRAD_TBL_SIZE];   // multiplication factors of y direction
    UINT16  ulLsXSizeTbl[MRV_GRAD_TBL_SIZE];   // sector sizes of x direction
    UINT16  ulLsYSizeTbl[MRV_GRAD_TBL_SIZE];   // sector sizes of y direction
} tsMrvLsCorrConfig;

/*****************************************************************************/

//! configuration for detection of bad pixel block
typedef struct
{
   UINT32  ulBpDeadThres;      // abs_dead_thres    Absolute dead pixel threshold
} tsMrvBpDetConfig;

//! new table element
typedef struct
{
   UINT16  usBpVerAddr;        // Bad Pixel vertical address
   UINT16  usBpHorAddr;        // Bad Pixel horizontal address
   UINT8   ucBpMsbValue;       // MSB value of fixed pixel (deceide if dead or hot)
} tsMrvBpNewTableElem;

//! new Bad Pixel table
typedef struct
{
   UINT32               ulBpNumber;          // Number of possible new detected bad pixel
   tsMrvBpNewTableElem  atBpNewTableElem[8]; // Array of Table element
} tsMrvBpNewTable;

/*****************************************************************************/
//! Denoising Pre Filter


//[DPF_NOISE_LEVEL]
typedef struct _tsMrvDpfNLLElem
{ // list element of noise level lookup table
    struct _tsMrvDpfNLLElem *ptMrvDpfNLLNextPtr; // nullpointer is end of list 
    FLOAT fMinGain;
    FLOAT fMaxGain;   
    UINT32 IspDpfNllCoeff0;
    UINT32 IspDpfNllCoeff1;
    UINT32 IspDpfNllCoeff2;
    UINT32 IspDpfNllCoeff3;
    UINT32 IspDpfNllCoeff4;
    UINT32 IspDpfNllCoeff5;
    UINT32 IspDpfNllCoeff6;
    UINT32 IspDpfNllCoeff7;
    UINT32 IspDpfNllCoeff8;
    UINT32 IspDpfNllCoeff9;
    UINT32 IspDpfNllCoeff10;
    UINT32 IspDpfNllCoeff11;
    UINT32 IspDpfNllCoeff12;
    UINT32 IspDpfNllCoeff13;
    UINT32 IspDpfNllCoeff14;
    UINT32 IspDpfNllCoeff15;
    UINT32 IspDpfNllCoeff16;
    UINT32 ulMrvDpfNllSeg; // 1 = logarithmic, 0 = equidistant segmentation
} tsMrvDpfNLLElem;

typedef struct
{ // constants for dynamic stength calculation
    FLOAT fOffset;
    FLOAT fGradient;
} tsMrvDpfDynStrength;

typedef struct
{ // struct for MrvSlsDpf_Init()
    UINT32 ulSigmaGreen;
    UINT32 ulSigmaRedBlue;
    UINT32 ulDpfEnable;
    UINT32 ulNlGainEnable;
} tsMrvSlsDpfInit;

typedef struct
{ // struct for MrvSlsDpf_Control()
    tsMrvDpfNLGain *pulMrvDpfNlGain;
    FLOAT fMrvAnalogueGain;
    tsMrvDpfDynStrength  tMrvDpfDynStrength;
    tsMrvDpfNLLElem *ptMrvDpfNLLElemFirst; 
    UINT32 ulDpfUpd;
} tsMrvSlsDpfControl;

typedef struct
{
    tsMrvSlsDpfInit    tDpfInit;
    tsMrvSlsDpfControl tDpfControl;
    char*              pszDpfTableFileName;  // Name of DPF table file
    
}tsMrvDpfConfig;

/*****************************************************************************/

//! image effect modes
typedef enum
{
   eMrvIeMode_Off,       // no image effect (bypass)
   eMrvIeMode_Grayscale, // Set a fixed chrominance of 128 (neutral grey)
   eMrvIeMode_Negative,  // Luminance and chrominance data is being inverted
   eMrvIeMode_Sepia,     // Chrominance is changed to produce a historical like brownish image color
   eMrvIeMode_ColorSel,  // Converting picture to grayscale while maintaining one color component.
   eMrvIeMode_Emboss,    // Edge detection, will look like an relief made of metal
   eMrvIeMode_Sketch,    // Edge detection, will look like a pencil drawing
   eMrvIeMode_Sharpen    // Edge detection, will look like a sharper drawing
} teMrvIeMode;

//! image effect color selection
typedef enum
{
   eMrvIe_MaintainRed   = 0x04,  // in eMrvIeMode_ColorSel mode, maintain the red color
   eMrvIe_MaintainGreen = 0x02,  // in eMrvIeMode_ColorSel mode, maintain the green color
   eMrvIe_MaintainBlue  = 0x01   // in eMrvIeMode_ColorSel mode, maintain the blue color
} teMrvIeColorSel;

//! image effect 3x3 matrix coefficients (possible values are -8, -4, -2, -1, 0, 1, 2, 4, 8)
typedef struct
{
   INT8 cCoeff_11;   INT8 cCoeff_12;   INT8 cCoeff_13;
   INT8 cCoeff_21;   INT8 cCoeff_22;   INT8 cCoeff_23;
   INT8 cCoeff_31;   INT8 cCoeff_32;   INT8 cCoeff_33;
} tsMrvIeMatrix;

//! image effect configuration struct
typedef struct
{
   teMrvIeMode   eMode;        // image effect mode
   UINT8         ucColorSel;   // color selection, or'ed combination of the teMrvIeColorSel enums
   UINT8         ucColorThres; // threshold for color selection
   UINT8         ucTintCb;     // Cb chroma component of 'tint' color for sepia effect
   UINT8         ucTintCr;     // Cr chroma component of 'tint' color for sepia effect
   tsMrvIeMatrix tMatEmboss;   // coefficient maxrix for emboss effect
   tsMrvIeMatrix tMatSketch;   // coefficient maxrix for sketch effect
   UINT8         ucSharp;      // Factor for sharpening function
   UINT8         ucCoringThr;  // Threshold for coring function
   BOOL          bFullRange;   // YCbCr full range 0...255
} tsMrvIeConfig;


/*****************************************************************************/

//! super impose transparency modes
typedef enum
{
    eMrvSi_TransUnknown = 0,    //!< SI transparency mode is unknown (module is switched off)
    eMrvSi_TransEnable  = 1,    //!< SI transparency mode enabled
    eMrvSi_TransDisable = 2     //!< SI transparency mode disabled
} teMrvSiTransMode;


//! super impose reference image
typedef enum
{
    eMrvSi_RefImgUnknown = 0,   //!< SI reference image is unknown (module is switched off)
    eMrvSi_RefImgSensor  = 1,   //!< SI reference image from sensor
    eMrvSi_RefImgMemory  = 2    //!< SI reference image from memory
} teMrvSiRefImage;


//! super impose configuration struct
typedef struct
{
    teMrvSiTransMode eTransMode;    //!< transparency mode on/off
    teMrvSiRefImage  eRefImage;     //!< reference image from sensor/memory
    UINT16           usOffsX;       //!< x offset (coordinate system of the reference image)
    UINT16           usOffsY;       //!< y offset (coordinate system of the reference image)
    UINT8            ucTransCompY;  //!< Y component of transparent key color
    UINT8            ucTransCompCb; //!< Cb component of transparent key color
    UINT8            ucTransCompCr; //!< Cr component of transparent key color
} tsMrvSiConfig;

/*****************************************************************************/

//! image stabilisation modes
typedef enum
{
    eMrvIsMode_Unknown  = 0,    //!< IS  mode is unknown (module is switched off)
    eMrvIsMode_On       = 1,    //!< IS  mode enabled
    eMrvIsMode_Off      = 2     //!< IS  mode disabled
} teMrvIsMode;


//! image stabilisation configuration struct
typedef struct
{
    tsMrvWindow      tMrvIsWindow;  //!< position and size of image stabilisation window
    UINT16           usMaxDx;       //!< maximal margin distance for X
    UINT16           usMaxDy;       //!< maximal margin distance for Y
} tsMrvIsConfig;


//! image stabilisation control struct
typedef struct
{
    teMrvIsMode      eIsMode;       //!< image stabilisation mode on/off
    UINT8            ucRecenter;    //!< recenter every frame by ((cur_v_offs–V_OFFS)/(2^RECENTER))
} tsMrvIsCtrl;


/*****************************************************************************/

//! for data path switching
typedef enum
{
    eMrvPathDpcc,
    eMrvPathRaw16,
    eMrvPathRaw,
    eMrvPathJpe,
    eMrvPathOff,
    eMrvPathOn
} teMrvPath;


//! buffer for memory interface
typedef struct
{
    UINT8 *pucBuffer;
    UINT32 ulSize;
    UINT32 ulOffs;
    UINT32 ulIrqOffsLLength; //not used for Cb and Cr buffers, IRQ offset for
    UINT8 *pucMallocStart;   //stores the malloc pointer address
} tsMrvBufferOld;            //main buffer and line length for self buffer


//! buffer for DMA memory interface
typedef struct
{
    UINT8 *pucBuffer;   // start of the buffer memory. Note that panning in an larger picture memory
                        // is possible by altering the buffer start address (and choosing
                        // ulPicWidth < ulLLength)
    UINT32 ulPicSize;   // size of the entire picture in bytes
    UINT32 ulPicWidth;  // width of the picture area of interest (not necessaryly the entire picture)
    UINT32 ulLLength;   // inter-line-increment. This is the amount of bytes between
                        // pixels in the same column but on different lines.
} tsMrvDmaBuffer;


//! color format for self picture input/output and DMA input
typedef enum
{
    eMrvMifCol_Format_YCbCr422 = 0, //!< YCbCr 4:2:2 format
    eMrvMifCol_Format_YCbCr444 = 1, //!< YCbCr 4:4:4 format
    eMrvMifCol_Format_YCbCr420 = 2, //!< YCbCr 4:2:0 format
    eMrvMifCol_Format_YCbCr400 = 3, //!< YCbCr 4:0:0 format
    eMrvMifCol_Format_RGB_565  = 4, //!< RGB   565   format
    eMrvMifCol_Format_RGB_666  = 5, //!< RGB   666   format
    eMrvMifCol_Format_RGB_888  = 6  //!< RGB   888   format
} teMrvMifColFormat;

//! color range for self picture input of RGB
typedef enum
{
    eMrvMifCol_Range_Std  = 0,
    eMrvMifCol_Range_Full = 1
} teMrvMifColRange;

//! color phase for self picture input of RGB
typedef enum
{
    eMrvMifCol_Phase_Cosited     = 0,
    eMrvMifCol_Phase_Non_Cosited = 1
} teMrvMifColPhase;


//! picture write/read format
typedef enum                            //!  The following examples apply to YCbCr 4:2:2 images, as all modes
{                                       //!  support this image format:
    eMrvMif_PicForm_Planar      = 0,    //!< planar     : separated buffers for Y, Cb and Cr
    eMrvMif_PicForm_SemiPlanar  = 1,    //!< semi-planar: one buffer for Y and a combined buffer for Cb and Cr
    eMrvMif_PicForm_Interleaved = 2     //!< interleaved: one buffer for all (Y0, Cb01, Y1, Cr01, Y2, Cb23, etc.)
} teMrvMifPicForm;


//! self picture operating modes
typedef enum
{
    eMrvMifSp_Original         = 0, //!< no rotation, no horizontal or vertical flipping
    eMrvMifSp_Vertical_Flip    = 1, //!< vertical   flipping (no additional rotation)
    eMrvMifSp_Horizontal_Flip  = 2, //!< horizontal flipping (no additional rotation)
    eMrvMifSp_Rotation_090_deg = 3, //!< rotation  90 degrees ccw (no additional flipping)
    eMrvMifSp_Rotation_180_deg = 4, //!< rotation 180 degrees ccw (equal to horizontal plus vertical flipping)
    eMrvMifSp_Rotation_270_deg = 5, //!< rotation 270 degrees ccw (no additional flipping)
    eMrvMifSp_Rot_090_V_Flip   = 6, //!< rotation  90 degrees ccw plus vertical flipping
    eMrvMifSp_Rot_270_V_Flip   = 7  //!< rotation 270 degrees ccw plus vertical flipping
} teMrvMifSpMode;


//! MI burst length settings
#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
typedef enum
{
    eMrvMif_BurstLength_4  = 0,  //!< burst length = 4
    eMrvMif_BurstLength_8  = 1,   //!< burst length = 8
    eMrvMif_BurstLength_16 = 2   //!< burst length = 16
} teMrvMifBurstLength;
#else //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)
typedef enum
{
    eMrvMif_BurstLength_4 = 0,  //!< burst length = 4
    eMrvMif_BurstLength_8 = 1   //!< burst length = 8
} teMrvMifBurstLength;
#endif //#if (MARVIN_FEATURE_MI_BURST_16 == MARVIN_FEATURE_EXIST)


//! MI apply initial values settings
typedef enum
{
    eMrvMif_NoInitVals      = 0,    //!< do not set initial values
    eMrvMif_InitOffs        = 1,    //!< set initial values for offset registers
    eMrvMif_InitBase        = 2,    //!< set initial values for base address registers
    eMrvMif_InitOffsAndBase = 3     //!< set initial values for offset and base address registers
} teMrvMifInitVals;

//! MI when to update configuration
typedef enum
{
    eMrvCfgUpdateFrameSync  = 0,
    eMrvCfgUpdateImmediate  = 1,
    eMrvCfgUpdateLater      = 2
} teMrvConfUpdateTime;

//! control register of the MI
typedef struct
{
    teMrvMifColFormat   eMrvMifSpOutForm;   //!< self picture path output format
    teMrvMifColFormat   eMrvMifSpInForm;    //!< self picture path input format
    teMrvMifColRange    eMrvMifSpInRange;   //!< self picture path input range
    teMrvMifColPhase    eMrvMifSpInPhase;   //!< self picture path input phase
    teMrvMifPicForm     eMrvMifSpPicForm;   //!< self picture path write format
    teMrvMifPicForm     eMrvMifMpPicForm;   //!< main picture path write format
    teMrvMifBurstLength eBurstLengthChrom;  //!< burst length for chrominance for write port
    teMrvMifBurstLength eBurstLengthLum;    //!< burst length for luminance for write port
    teMrvMifInitVals    eInitVals;          //!< enable updating of the shadow registers
                                            //!     for main and self picture to their init
                                            //!     values
    BOOL                bByteSwapEnable;    //!< enable change of byte order for write port
    BOOL                bLastPixelEnable;    //!< enable the last pixel signalization
    teMrvMifSpMode      eMrvMifSpMode;      //!< self picture path operating mode
    teMrvPath           eMainPath;          //!< enable path
    teMrvPath           eSelfPath;          //!< enable path
    UINT32              ulIrqOffsInit;      //!< offset counter interrupt generation for fill_mp_y
                                            //!  (counted in bytes)
} tsMrvMiCtrl;


//! buffer for memory interface
typedef struct
{
    UINT8  *pucBuffer;  //!< buffer start address
    UINT32  ulSize;     //!< buffer size (counted in bytes)
    UINT32  ulOffs;     //!< buffer offset count (counted in bytes)
} tsMrvBuffer;


//! main or self picture path, or DMA configuration
typedef struct
{
    UINT32      ulYPicWidth;    //!< Y  picture width  (counted in pixels)
    UINT32      ulYPicHeight;   //!< Y  picture height (counted in pixels)
    UINT32      ulLLength;      //!< line length means the distance from one pixel to the vertically next
                                //!  pixel below including the not-used blanking area, etc.
                                //!  (counted in pixels)
    tsMrvBuffer tYBuffer;       //!< Y  buffer structure
    tsMrvBuffer tCbBuffer;      //!< Cb buffer structure
    tsMrvBuffer tCrBuffer;      //!< Cr buffer structure
} tsMrvMiPathConf;


//! DMA configuration
typedef struct
{
    BOOL                  bStartDma;            //!< start DMA immediately after configuration
    BOOL                  bFrameEndDisable;     //!< suppress v_end so that no frame end can be
                                                //!     detected by the following instances
    BOOL                  bByteSwapEnable;      //!< enable change of byte order for read port
    BOOL                  bContinuousEnable;    //!< Enables continuous mode. If set the same frame is read back
                                                //!     over and over. A start pulse on dma_start is need only for the
                                                //!     first time. To stop continuous mode reset this bit (takes
                                                //!     effect after the next frame end) or execute a soft reset.
    teMrvMifColFormat     eMrvMifColFormat;     //!< DMA input color format
    teMrvMifPicForm       eMrvMifPicForm;       //!< DMA read buffer format
    teMrvMifBurstLength   eBurstLengthChrom;    //!< burst length for chrominance for read port
    teMrvMifBurstLength   eBurstLengthLum;      //!< burst length for luminance for read port
    BOOL                  bViaCamPath;          //!< Set this to TRUE if the DMA-read data is routed through
                                                //!     the path that is normally used for the live camera
                                                //!     data (e.g. through the image effects module).
} tsMrvMiDmaConf;

/*****************************************************************************/

//! Wide Dynamic Range WDR

#define WDR_ENABLE              0x01 // enable the WDR module
#define WDR_COLOR_SPACE_SELECT  0x02 // 0: Luminance/Chr. color space 1: RGB color space
#define WDR_DISABLE_CHR_MAPPING 0x04 // disable chrominance mapping
#define WDR_USE_IREF            0x08 // use illumination reference
#define WDR_USE_Y9_8            0x10 // use RGB and Y*9/8 for maximum value calculation
#define WDR_USE_RGB7_8          0x20 // decrease RGB max by 7/8
#define WDR_DISABLE_TRANSIENT   0x40 // disable transient between Y and Iref

typedef struct tsMrvLumCurve_
{
    UINT32 ulDyn[4]; // Tone Curve sample points       0=Dyn1, 1=Dyn2, 2=Dyn3, 3=Dyn4
    UINT16 usYm[33]; // Tonemapping curve coefficients 0=Ym0,  1=Ym1 ........ 32=Ym32

}tsMrvLumCurve;

typedef struct tsMrvWdrConfig_
{
    UINT8  ucControl;       // Control bits of the ISP_WDR_CTRL register without RGB factor
    UINT16 usLumOffset;     // Luminance Offset (a) for RGB operation mode
    UINT16 usRGBOffset;     // RGB Offset (b) for RGB operation mode
    UINT16 usDminThreshold; // Lower threshold for deltaMin value
    UINT8  ucDminStrength;  // strenght factor for DMIN
    UINT8  ucRGBFactor;     // defines Iref = (WDR_RGB_FACTOR * RGBmax + (8-WDR_RGB_FACTOR) * Y)/8
    tsMrvLumCurve tMrvLumCurve1;     // first  tonmapping curve
    tsMrvLumCurve tMrvLumCurve2;     // second tonmapping curve
    tsMrvLumCurve tMrvLumCurve3;     // third  tonmapping curve
    char*  pszWdrToneCurveFileName;  // Name of WDR tonecurve file

}tsMrvWdrConfig;

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
// mrv.c
UINT32 MrvGetMrvId( void );
void   MrvInit( void );
void   MrvStart( UINT16 usNumberOfFrames, teMrvConfUpdateTime eUpdateTime );
void   MrvStop( teMrvConfUpdateTime eUpdateTime );
RESULT MrvWaitForVSync( UINT32 ulVPol );
RESULT MrvSetDataPath( teMrvYcsChnMode MrvYcsChnMode, teMrvDpSwitch DpSwitch);
RESULT MrvSetMipiSmia( UINT32 ulMode);
RESULT MrvWaitForFrameEnd( void );
RESULT MrvWaitForMacroBlock(void);
RESULT MrvWaitForDmaReady( void );
RESULT MrvWaitForDmaFrameEnd( void );
UINT32 MrvGetFrameEndIrqMask_ISP( void );
UINT32 MrvGetFrameEndIrqMask_DMA( void );
void   MrvResetInterruptStatus( void );
void   MrvSet601Mode( void );
void   MrvSet656Mode( void );

void   MrvSetDmaReadMode(teMrvDmaReadMode eMode, teMrvConfUpdateTime eUpdateTime);
// MrvSelfDmaSelect() is still available for compatibility reasons. For new developments, use MrvSetDmaReadMode() instead
void   MrvSelfDmaSelect(BOOL bDma, teMrvConfUpdateTime eUpdateTime);

void   MrvUpdateConfiguration( void );
void   MrvSetExtYCMode(void);
void   MrvSetYCMode(void);

//mrv_isp.c
RESULT MrvIspSetInputAquisition( const tsIsiSensorConfig *ptIsiCfg );
eMrvIspPath MrvIspSelectPath( const tsIsiSensorConfig *ptIsiCfg, UINT8* pucWordsPerPixel );
void   MrvIspSetOutputFormatter( const tsMrvWindow *ptWindow, teMrvConfUpdateTime eUpdateTime );
void   MrvIspGetOutputFormatter( tsMrvWindow *ptWindow );
void   MrvIspSetGamma( const tsIsiGammaCurve *ptR, const tsIsiGammaCurve *ptG, const tsIsiGammaCurve *ptB );
void   MrvIspGetGamma( tsIsiGammaCurve *ptR, tsIsiGammaCurve *ptG, tsIsiGammaCurve *ptB );
void   MrvIspSetDemosaic( teMrvDemosaicMode eDemosaicMode, UINT8 ucDemosaicTh );
RESULT MrvIspSetWbMode( teMrvAwbMode tWbMode );
RESULT MrvIspSetWbMeasConfig( const tsMrvWbMeasConfig *tWbMeasConfig );
RESULT MrvIspGetWbMode( teMrvAwbMode *ptWbMode );
RESULT MrvIspGetWbMeasConfig( tsMrvWbMeasConfig *tWbMeasConfig );
RESULT MrvIspGetWbMeas( tsIsiAwbMean *ptMrvAwbMean );
RESULT MrvIspSetColorConversion( const tsIsi3x3FloatMatrix *ptColConvCoeff );
RESULT MrvIspGetColorConversion( tsIsi3x3FloatMatrix *ptColConvCoeff );
void   MrvIspSetExposureControl( const tsMrvAecConfig *ptMrvAecConfig );
RESULT MrvIspSetWbGains( const tsMrvWbGains *ptWbGains );
RESULT MrvIspGetWbGains( tsMrvWbGains *ptWbGains );
RESULT MrvIspSetAutofocus( const tsMrvAfConfig *ptAfConfig );
RESULT MrvIspGetAutofocus( tsMrvAfConfig *ptAfConfig );
void   MrvIspGetAutofocusMeas( tsMrvAfMeas *ptMrvAfMeas );
RESULT MrvIspWaitForAFMeas( void );
BOOL   MrvIspChkAFMOvfSum( BOOL bClear );
BOOL   MrvIspChkAFMOvfLum( BOOL bClear );
RESULT MrvIspSetLsCorrection( const tsMrvLsCorrConfig *ptLsCorrConfig );
RESULT MrvIspGetLsCorrection( tsMrvLsCorrConfig *ptLsCorrConfig );
RESULT MrvIspLsCorrectionOnOff(BOOL bLsCorrOnOff);
RESULT MrvIspSetBpCorrection( const tsMrvBpCorrConfig *ptBpCorrConfig );
RESULT MrvIspSetBpDetection( const tsMrvBpDetConfig *ptBpDetConfig );
RESULT MrvIspBpReadNewTable( tsMrvBpNewTable *ptBpNewTable, BOOL bClearNewTable);
RESULT MrvIspBpWriteTable( const tsIsiBpTable *ptBpTable);
RESULT MrvIspBpReadTable( tsIsiBpTable *ptBpTable);
RESULT MrvIspDpccBptWriteTable( const tsIsiBpTable *ptBpTable);
RESULT MrvIspDpccBptReadTable( tsIsiBpTable *ptBpTable);
RESULT MrvIspWaitForBadPixel( void );
BOOL   MrvIspChkBPNewTblFull( BOOL bClear );
BOOL   MrvIspChkBpIntStat( void );
RESULT MrvIspClearBpInt( void );
void   MrvIspSetGamma2( const tsMrvGammaOutCurve *ptGamma );
RESULT MrvIspSetCrossTalk( const tsIsi3x3FloatMatrix *ptCrossTalkCoeff, const tsIsiXTalkFloatOffset *ptCrossTalkOffset );
RESULT MrvIspGetCrossTalk( tsIsi3x3FloatMatrix *ptCrossTalkCoeff, tsIsiXTalkFloatOffset *ptCrossTalkOffset );
RESULT MrvIspInitializeFilterRegisters( void );
RESULT MrvIspActivateFilter( BOOL bActivateFilter );
RESULT MrvIspSetFilterParams( UINT8 uNoiseReducLevel, UINT8 uSharpLevel );
int DPF_Read_NLL_Table(tsMrvDpfConfig* ptDpfCfg);

//mrv_isp_bls.c
void   MrvIspSetBlackLevel( const tsMrvBlcConfig *ptConfig );
void   MrvIspGetBlackLevelMeas( tsMrvBlcMean *ptMrvBlcMean );
RESULT MrvIspBlsSetConfig( const tsMrvBlsConfig *ptBlsConfig );
RESULT MrvIspBlsSetFixedValues( const tsMrvBlsSubtraction *ptMrvBlsSubtraction );
RESULT MrvIspBlsGetFixedValues( tsMrvBlsSubtraction *ptMrvBlsSubtraction );
RESULT MrvIspBlsGetMeasuredValues( tsMrvBlsMeasured *ptMrvBlsMeasured );

//mrv_ie.c
RESULT MrvIeSetConfig( const tsMrvIeConfig *ptIeConfig );
RESULT MrvIeGetConfig( tsMrvIeConfig *ptIeConfig );
UINT8  MrvIeTint_RGB2Cx(UINT8 ucR, UINT8 ucG, UINT8 ucB, BOOL CBSelection);

//mrv_si.c
RESULT MrvSiSetConfig( const tsMrvSiConfig *ptSiConfig );
RESULT MrvSiGetConfig( tsMrvSiConfig *ptSiConfig );

//mrv_is.c
RESULT MrvIsSetConfig( const tsMrvIsConfig *ptIsConfig );
RESULT MrvIsGetConfig( tsMrvIsConfig *ptIsConfig );
RESULT MrvIsSetDisplace(INT16 sDisPlX, INT16 sDisPlY );
RESULT MrvIsSetCtrl( const tsMrvIsCtrl *ptMrvIsCtrl);
RESULT MrvIsGetCtrl(tsMrvIsCtrl *ptMrvIsCtrl);

//mrv_col.c
void   MrvColSetColorProcessing( const tsMrvColorSettings *ptMrvCol );
void   MrvColGetColorProcessing( tsMrvColorSettings *ptMrvCol );

//mrv_res.c
void   MrvResSetMainResize( const tsMrvScale *ptScale, teMrvConfUpdateTime eUpdateTime, const tsMrvRszLut *ptMrvRszLut );
void   MrvResGetMainResize( tsMrvScale *ptScale );

void   MrvResSetSelfResize( const tsMrvScale *ptScale, teMrvConfUpdateTime eUpdateTime, const tsMrvRszLut *ptMrvRszLut );
void   MrvResGetSelfResize( tsMrvScale *ptScale );

//mrv_jpe.c
RESULT MrvJpeInit( UINT32 ulResolution, UINT8 ucCompressionRatio, BOOL bJpeScale );
RESULT MrvJpeInitEx( UINT16 usHSize, UINT16 usVSize, UINT8 ucCompressionRatio, BOOL bJpeScale );

//mrv_smlout
void MrvSmlOutSetPath( teMrvPath tMainPath );

//mrv_mif
void   MrvMifMainSkip( BOOL bInitOffset );
void   MrvMifResetOffsets( teMrvConfUpdateTime eUpdateTime );
UINT32 MrvMifGetByteCnt( void );
UINT32 MrvMifGetPixelCnt( void );
RESULT MrvMifStartDma( void );

RESULT MrvMif_SetSelfPicOrientation( teMrvMifSpMode tMrvMifSpMode);
RESULT MrvMif_SetMainBuffer( const tsMrvMiPathConf *ptMrvMiPathConf, teMrvConfUpdateTime eUpdateTime );
RESULT MrvMif_SetSelfBuffer( const tsMrvMiPathConf *ptMrvMiPathConf, teMrvConfUpdateTime eUpdateTime );
RESULT MrvMif_SetDmaBuffer(  const tsMrvMiPathConf *ptMrvMiPathConf );
void   MrvMif_DisableAllPaths( BOOL bPerformWaitForFrameEnd );
RESULT MrvMif_GetMainBuffer( tsMrvMiPathConf *ptMrvMiPathConf );
RESULT MrvMif_GetSelfBuffer( tsMrvMiPathConf *ptMrvMiPathConf );
RESULT MrvMif_SetPathAndOrientation( const tsMrvMiCtrl *ptMrvMiCtrl );
RESULT MrvMif_GetPathAndOrientation( tsMrvMiCtrl *ptMrvMiCtrl );
RESULT MrvMif_SetConfiguration( const tsMrvMiCtrl *ptMrvMiCtrl, const tsMrvMiPathConf *ptMrvMiMpPathConf, const tsMrvMiPathConf *ptMrvMiSpPathConf, const tsMrvMiDmaConf *ptMrvMiDmaConf );
RESULT MrvMif_SetDmaConfig( const tsMrvMiDmaConf *ptMrvMiDmaConf );
RESULT MrvMif_GetPixelPer32BitOfLine( UINT8 *pucPixelPer32Bit, teMrvMifColFormat eMrvMifSpFormat, teMrvMifPicForm eMrvMifPicForm, BOOL bLuminanceBuffer );

// mrv_wdr
RESULT MrvIspWdrSetCurve(const tsMrvLumCurve *ptLumCurve);
RESULT MrvIspWdrGetCurve(tsMrvLumCurve *ptLumCurve);
RESULT MrvIspWdrSetConfig(const tsMrvWdrConfig *ptConfig );
RESULT MrvIspWdrGetConfig(tsMrvWdrConfig *ptConfig );
int WDR_Load_ToneCurve(tsMrvWdrConfig *ptWdrCfg );

// the functions between 'start' and 'end' will be removed soon
// start
void MrvMifSetMainBuffer( const tsMrvBufferOld *ptYBuffer, const tsMrvBufferOld *ptCbBuffer, const tsMrvBufferOld *ptCrBuffer, teMrvConfUpdateTime eUpdateTime );
void MrvMifSetSelfBuffer( const tsMrvBufferOld *ptYBuffer, const tsMrvBufferOld *ptCbBuffer, const tsMrvBufferOld *ptCrBuffer, teMrvConfUpdateTime eUpdateTime );
void MrvMifSetPath( teMrvPath eMainPath, teMrvPath eSelfPath, BOOL bByteSwap );
void MrvMifSetDmaBuffer( const tsMrvDmaBuffer *ptYBuffer, const tsMrvDmaBuffer *ptCbBuffer, const tsMrvDmaBuffer *ptCrBuffer, BOOL bDmaByteSwap );
void MrvMifSelfMirror( BOOL bMirror, teMrvConfUpdateTime eUpdateTime );
void MrvMifSemiPlanar( BOOL bSet );
void MrvMifSetRgb888( BOOL bSet, BOOL bTypeIn444 );
void MrvMifSetRgb666( BOOL bSet, BOOL bTypeIn444 );
void MrvMifSetRgb565( BOOL bSet, BOOL bTypeIn444 );
// end


//mrv_2str.c
#ifdef MRV_SUPPORT_STATE2STRING
   extern const char *Mrv2Str_AwbMode(teMrvAwbMode eMode);
   extern const char *Mrv2Str_DemosaicMode(teMrvDemosaicMode eMode);
   extern const char *Mrv2Str_Window(tsMrvWindow tWindow);
   extern const char *Mrv2Str_SlsAecMode(teMrvAecMode eMode);
   extern const char *Mrv2Str_JpgCompression(int iCompression);
   extern const char *Mrv2Str_Path(teMrvPath ePath);
   extern const char *Mrv2Str_YcsChnMode(teMrvYcsChnMode eMode);
   extern const char *Mrv2Str_DpSwitch(teMrvDpSwitch eDataPath);
   extern const char *Mrv2Str_AfmMode(teMrvAfmMode eMode);
   extern const char *Mrv2Str_AfssMode(teMrvAfssMode eMode);
   extern const char *Mrv2Str_IeMode(teMrvIeMode eMode);
#endif // MRV_SUPPORT_STATE2STRING

//mrv_regdump.c
#ifdef MRV_SUPPORT_REGDUMP
   extern UINT32      MrvReg_Read(UINT32 ulRegAddr);
   extern void        MrvReg_Write(UINT32 ulRegAddr, UINT32 ulData);
   extern const char* MrvReg_GetName(UINT32 ulRegAddr);
   extern void        MrvReg_DumpAll( FILE* hFile );
   extern void        MrvReg_DumpBPTable( FILE* hFile, UINT32 ulBpNumber );
   extern void        MrvReg_DumpIrqStatus( FILE* hFile );
   extern void        MrvReg_DumpDebugInfoInit( void );
   extern void        MrvReg_DumpDebugInfoExit( void );
   extern void        MrvReg_DumpDebugInfoNow( FILE *ptFile );
   extern void        MrvReg_MonitorMrvInputStatus( FILE *ptFile, UINT32 ulMicroSec );
#endif // MRV_SUPPORT_REGDUMP


#endif //#ifndef _MRV_H
