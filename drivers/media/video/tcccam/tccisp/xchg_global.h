/*****************************************************************************/
/*!
 *  \file        xchg_global.h \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Interface to exchange configuration data. \n
 *
 *  \revision    $Revision: 463 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-07-07 19:43:29 +0200 (Di, 07 Jul 2009) $ \n
 *               $Id: xchg_global.h 463 2009-07-07 17:43:29Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _XCHG_GLOBAL_H
#define _XCHG_GLOBAL_H


typedef struct _tsSnapshotConfig {
   UINT32 ulFlags; // snapshot flags
   int iUserZoom;  // user zoom factor to use ( Zoomfactor = 1 + (<value>*1024) )
   int iUserW;     // user width (in pixel)
   int iUserH;     // user height (in pixel)
   UINT8 ucCompressionRatio; // compression ratio for JPEG snapshots
   char * pszFileNameExifData; // file name for additional EXIF data
} tsSnapshotConfig;

typedef struct _tsSnapshotSeriesConfig {
   UINT32 ulFlags;        // flags for parameter to change
   UINT16 usSnapStart;    // frame number when snapshot series starts
   UINT16 usSnapDelay;    // frames between snapshots
   UINT16 usSnapMaxNumber;// max number of snapshots 
   UINT32 ulStartValue;   // start value of the changing value  
   UINT32 ulMaxValue;     // max value of the changing value
   UINT32 ulValueOffset;  // offset of the changing value 
} tsSnapshotSeriesConfig;

typedef struct _tsViewfinderConfig {
   UINT32 ulFlags; // how to display the viewfinder
   int iZoom;      // zoom factor to use ( Zoomfactor = 1 + (<value>*1024) )
   int iLcdContrast; // contrast setting for LCD
   // following settings are only used in FF_VFFLAG_MODE_USER mode
   int iX;   // start pixel of upper left corner on LCD
   int iY;   // start pixel of upper left corner on LCD
   int iW;   // width (in pixel)
   int iH;   // height (in pixel)
   BOOL bKeepAspect; // keeps the aspect ratio by cropping the input to match the output aspect ratio.
} tsViewfinderConfig;

#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
typedef struct tsMrvDpccBptConfig_
{
	UINT16 usMeasurementCount; // number of measurements in sequence
	UINT16 usMeasurementDelay; // number of frames between two measurements
	UINT8  ucThresholdHi100;   // min. number of detections required for true positive (precent relative to meas. count)
	UINT8  ucThresholdLo100;   // min. number of detections required if already detected in previous sequence(s) (percent)
	UINT8  ucParameterSet;     // parameter set to use; less than DPCC_PARAMETER_SET_COUNT
	BOOL   bUseMainPath;       // use main or self path to receive list from detection unit
	BOOL   bEnableDetection;   // enable detection unit
	BOOL   bEnableCorrection;  // enable correction unit
	BOOL   bDumpList;          // dump pixel list of each measurement sequence into file
	char*  pszFilename;        // file (base) name for pixel lists; timestamp added
} tsMrvDpccBptConfig;
/* example: { 50, 14, 50, 20, 1, ... }
   Detect defect pixels using second register set each 15th frame,
   collect 50 measurements where all pixels detected at least 25 times (50%) are written into bad pixel table.
   But also pixels detected at least 10 times (20%) are written if they were written in previous sequence.
   So we need a total of 750 frames for one complete measurement sequence.
 */
#elif ( MARVIN_FEATURE_DPCC != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPCC
#endif  // MARVIN_FEATURE_DPCC


//! structure for configuration of the camera parameter setup
typedef struct _tsCamParamConfig {
   UINT32 ulUseParmFlags[(((ISI_PARM__COUNT-1)/32) + 1)]; // individual flags for each parameter telling
                                                          // if it sould be applied or not.
   UINT32 ulCamParm[ISI_PARM__COUNT];  // values for the camera parameters to apply
   BOOL   bEnable;  // enable flag for the whole camera parameter initialization functionality
} tsCamParamConfig;


//! Number of supported DIP-Switches
#define FF_DIPSWITCH_COUNT    10



typedef struct _tsCfgFlags {
    // following flag tripels controls the behaviour of the associated marvin control loops.
    // For feature XXX, the 3 flags are totally independant and have the following meaning:
    // fXXX : If set, there is any kind of software interaction during runtime that may lead to a
    //        modification of the feature-dependant settings. For each frame, a feature specific loop
    //        control routine is called that may perform other actions based on feature specific configuration.
    //        If not set, only base settings will be applied during setup, or the reset values are left
    //        unchanged. No control routine will be called inside the processing loop
    // fXXXprint : If set, some status informations will be printed out inside the processing loop.
    //        Status printing is independant of the other flags regarding this feature.
    // fXXX_dis : If set, the feature dependant submodule of the marvin is disabled or is turned
    //        into bypass mode. Note that it is still possible to set one or more of the other flags
    //        too, but this wouldn't make much sense...

    // lens shading correction
    unsigned int fLSC      :1;
    unsigned int fLSCprint :1;
    unsigned int fLSC_dis  :1;

    // bad pixel correction
    unsigned int fBPC      :1;
    unsigned int fBPCprint :1;
    unsigned int fBPC_dis  :1;
    
    // bad pixel detection
    unsigned int fBPD      :1;

    // black level correction
    unsigned int fBLS      :1;
    unsigned int fBLS_man  :1;  // only fixed values
    unsigned int fBLS_smia :1;  // fixed value read from smia interface
    unsigned int fBLSprint :1;
    unsigned int fBLS_dis  :1;

    // (automatic) white balancing (if automatic or manual can be configured elsewhere)
    unsigned int fAWB      :1;
    unsigned int fAWB_bayes:1;
    unsigned int fAWBprint :1;
    unsigned int fAWBprint2:1;
    unsigned int fAWB_dis  :1;

    // automatic exposure (and gain) control
    unsigned int fAEC      :1;
    unsigned int fAECprint :1;
    unsigned int fAEC_dis  :1;
    unsigned int fAEC_ScEval:1;    

    // auto focus
    unsigned int fAF       :1;
    unsigned int fAFprint  :1;
    unsigned int fAF_dis   :1;

    // denoising pre filter
    unsigned int fDPF       :1;
    unsigned int fDPFprint  :1;
    unsigned int fDPF_dis   :1;

    // enable flags for various other components of the marvin
    unsigned int fCP         :1;  // color processing (brightness, contrast, saturation, hue)
    unsigned int fGamma      :1;  // input gamma block
    unsigned int fDemosaic   :1;  // demosaicing
    unsigned int fGamma2     :1;  // output gamma block
    unsigned int fIspFilters :1;  // Isp de-noise and sharpenize filters
    unsigned int fCAC        :1;  // Isp CAC

    // demo stuff
    unsigned int fCpSatLoop     :1;     // demo: saturation loop enable
    unsigned int fCpContrLoop   :1;     // demo: contrast loop enable
    unsigned int fCpBrightLoop  :1;     // demo: brightness loop enable
    unsigned int fScalerLoop    :1;     // demo: scaler loop enable
    unsigned int fCycleIeMode   :1;     // demo: use joystick to cycle through the image effect modes

    // others
    unsigned int fContinousAF      :1;     // enable continous autofocus
    //unsigned int fAfBeforeSnapshot :1; (*** now included in tSnapshot.ulFlags ***)
    unsigned int fYCbCrFullRange      :1;  // enable YCbCr full range
    unsigned int fYCbCrNonCosited      :1; // enable YCbCr color phase non cosited

}tsCfgFlags;

typedef struct _tsConfig
{
    tsCfgFlags Flags;
    tsMrvLsCorrConfig LscCfg;
    tsMrvBpCorrConfig BpcCfg;
    tsMrvBpDetConfig BpdCfg;

#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
    tsMrvDpccConfig DpccConfig;
    tsMrvDpccBptConfig DpccBptConfig;
#elif ( MARVIN_FEATURE_DPCC != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPCC
#endif  // MARVIN_FEATURE_DPCC

    tsMrvWbConfig WbConfig;
    tsMrvCacConfig CacConfig;
    tsMrvAecConfig AecCfg;
    tsMrvBlsConfig BlsCfg;
    tsMrvAfConfig  AfCfg;
    tsMrvColorSettings Color;
    tsMrvIeConfig ImgEffCfg;
    teMrvDemosaicMode eDemosaicMode;
    UINT8 ucDemosaicTh;
    UINT8 ucExposure;
    UINT32 ulReportDetails;  // what to include in reports;
                             // an or'ed combination of the FF_REPORT_xxx defines
    tsMrvDpfConfig DpfConfig;
    tsMrvWdrConfig WdrConfig;    
    tsViewfinderConfig tViewFinder;
    tsSnapshotConfig tSnapshotA;  // primary snapshot
    tsSnapshotConfig tSnapshotB;  // secondary snapshot
    tsSnapshotSeriesConfig tSnapshotSeries;  // secondary snapshot
    tsCamParamConfig tCamParams;
    teMrvAfmMode eAfmMode;   // auto focus measurement mode
    teMrvAfssMode eAfssMode; // auto focus search strategy mode
    BOOL bWbGetGainsFromSensorDriver;
    UINT8 ucFilterLevelNoiseReduc;
    UINT8 ucFilterLevelSharp;
    int aiDipSwitchUsage[FF_DIPSWITCH_COUNT];
}tsConfig;

/*!*********************************************************************
 * various config and info structs concentrated into one struct
 * for simplification
 *********************************************************************/
typedef struct tsPlSystemConfig_
{
   tsIsiSensorConfig     *ptIsiConfig;
   tsIsiSensorCaps       *ptIsiCaps;
   tsIsiSensorAwbProfile *ptIsiSensorAwbProfile;
   tsLcdInfo             *ptLcdInfo;
   tsConfig              *ptConfig;
   UINT32                 ulFocusMax;
   BOOL                   bReportOnNextSnapshot;    // if set, the next snapshot will also include a report
                                                    // regardless of if this is configured in *ptConfig. 
} tsPlSystemConfig;

/******************************************************************************
* GLOBALS
******************************************************************************/

//! the one and only configuration struct
extern tsConfig s_Config;


#endif //#ifndef _XCHG_GLOBAL_H
