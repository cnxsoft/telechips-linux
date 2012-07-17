/*****************************************************************************/
/*!
 *  \file        mrv_sls.h \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Public header for all second level support routines
 *               of the MARVIN the MARVIN driver. Second level support routines are
 *               those using the plain routines of mrv.h to implement a kind of
 *               "helper" to program/access/use the MARVIN with a bit more
 *               abstraction. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_sls.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_SLS_H
#define _MRV_SLS_H

#ifdef MRV_SUPPORT_SL

// ////////////////////////////////////////////////////////////////////////////
// message system part for MARVIN
// ////////////////////////////////////////////////////////////////////////////

/*
 *  Message groups:
 *  There are 16 different message groups available. Each bit represents one
 *  group. Hence it is easy to register one call-back function for several
 *  message groups.
 */
#define MRV_SLS_MSG_GROUP_OTHERS        (UINT32)(0x00000001)    //!< message group for 'others' who do not have an own group
#define MRV_SLS_MSG_GROUP_AEC           (UINT32)(0x00000002)    //!< message group for AEC
#define MRV_SLS_MSG_GROUP_AF            (UINT32)(0x00000004)    //!< message group for AF
#define MRV_SLS_MSG_GROUP_AWB           (UINT32)(0x00000008)    //!< message group for AWB

/*
 *  Message IDs:
 *  The message IDs will be counted, so there are possibly 65535 different
 *  message IDs available.
 *
 *  Segmentation:
 *  IDs for AEC:    0x00010000 - 0x01000000
 *  IDs for AF:     0x01010000 - 0x02000000
 *  IDs for AWB:    0x02010000 - 0x03000000
 *  IDs for others: 0x10010000 - 0xFFFF0000
 */


// ///////////////////////////////////////////////////////////////////////
// white balancing start
// ///////////////////////////////////////////////////////////////////////

/*
 *  Message IDs segmentation:
 *  IDs for AWB:    0x02010000 - 0x03000000
 */

// measurement adjustment message:
//  The pMsgData leads to an INT8 value, which represents the current
//  measurement adjustment status.
#define MRV_SLS_MSG_ID_AWB_MEAS_ADJ     ((UINT32)(0x02010000) | MRV_SLS_MSG_GROUP_AWB )     //!< message system message ID for MARVIN AWB measurement adjustment
#define MRV_SLS_WB_MEAS_ADJ_TOO_LITTLE  (INT8)(-2)              //!< (*pMsgData) value: not enough near-white pixels, no more adjustments allowed
#define MRV_SLS_WB_MEAS_ADJ_INCREASING  (INT8)(-1)              //!< (*pMsgData) value: not enough near-white pixels, still adjusting
#define MRV_SLS_WB_MEAS_ADJ_OKAY        (INT8)( 0)              //!< (*pMsgData) value: amount of near-white pixels is in the configured range, no adjustments necessary
#define MRV_SLS_WB_MEAS_ADJ_DECREASING  (INT8)( 1)              //!< (*pMsgData) value: too much near-white pixels, still adjusting
#define MRV_SLS_WB_MEAS_ADJ_TOO_MUCH    (INT8)( 2)              //!< (*pMsgData) value: too much near-white pixels, no more adjustments allowed
#define MRV_SLS_WB_MEAS_ADJ_CORRECTION  (INT8)( 3)              //!< (*pMsgData) value: some of the measurement configuration values were out of range and have been corrected
#define MRV_SLS_WB_MEAS_ADJ_ERROR       (INT8)( 4)              //!< (*pMsgData) value: an error occured while getting or setting the measurement configuration

// gain adjustment message:
//  The pMsgData leads to an UINT8 value, which represents the current
//  gain adjustment status.
#define MRV_SLS_MSG_ID_AWB_GAIN_ADJ     ((UINT32)(0x02020000) | MRV_SLS_MSG_GROUP_AWB )     //!< message system message ID for MARVIN AWB gain adjustment
#define MRV_SLS_WB_GAIN_ADJ_ERROR       (INT8)(-1)              //!< (*pMsgData) value: an error occured, so no adjustments were done
#define MRV_SLS_WB_GAIN_ADJ_OKAY        (INT8)( 0)              //!< (*pMsgData) value: the measurement values show that no adjustments are necessary
#define MRV_SLS_WB_GAIN_ADJ_CHANGE      (INT8)( 1)              //!< (*pMsgData) value: the measurement values show that adjustments are necessary


// effective white balancing gains
typedef struct
{
    UINT16 usGainRed;
    UINT16 usGainGreen;
    UINT16 usGainBlue;
} tsMrvEffectiveWbGains;


// initialization of white balancing mode
RESULT MrvSls_AwbInit(const tsMrvWbConfig *ptWbConfig, const tsIsiSensorConfig *ptIsiCfg);
void   MrvSls_Wb_ManualOneShotActivation( teMrvAwbMode eMode );
extern BOOL bTriggerPushWb;
// ///////////////////////////////////////////////////////////////////////
// white balancing end
// ///////////////////////////////////////////////////////////////////////

//! AE information structure for SLS support
typedef struct tsMrvAEtoSLS_
{
    FLOAT  fIntegrationTimeMulGain; // Integration time multiplied with gain will be delivered from the AE
    FLOAT  fIntegrationTime;        // EXIF:Integration time for TAG_EXPOSURETIME
    FLOAT  fGain;                   // EXIF:Gain for calculation TAG_ISO_EQUIVALENT    
    BOOL   bAwbWaitForAe;             // TRUE: suspend AWB control loop, set by MrvSls_Aec_CalcNewExposure
}tsMrvAEtoSLS;

//! AWB prior exp filter count max. It should be higher than the min count
#define AWB_PRIOREXP_FILTER_SIZE_MAX   50
//! AWB prior exp filter count min.
#define AWB_PRIOREXP_FILTER_SIZE_MIN   1
//! AWB pca filter count max. It should be higher than the min count
#define AWB_DAMPING_FILTER_SIZE_MAX    50
//! AWB pca filter count min.
#define AWB_DAMPING_FILTER_SIZE_MIN    6
// ///////////////////////////////////////////////////////////////////////
// automatic exposure control section start
// ///////////////////////////////////////////////////////////////////////

// Setup of AEC system
RESULT MrvSls_Aec_Setup( IN UINT32 ulResId, INOUT const tsConfig *ptConfig );

// Returns AEC configuration
// (currently only the luminance measurement grid parameters)
RESULT MrvSls_Aec_GetConfig( OUT tsMrvWindow *ptWnd );

// Execution of AEC
RESULT MrvSls_Aec_Execute( INOUT const tsConfig *ptConfig );

// Get the current measured mean luminance value
RESULT MrvSls_Aec_GetIntegralMean( OUT FLOAT *prIntegralMeanLuma );

// ///////////////////////////////////////////////////////////////////////
// automatic exposure control section end
// ///////////////////////////////////////////////////////////////////////
#endif
#ifdef SL_SUPPORT_PARTLY
// ///////////////////////////////////////////////////////////////////////
// simplified datapath and output formatter/resizer adjustment
// can be used to setup the main and self datapathes in a convenient way.
// ///////////////////////////////////////////////////////////////////////

//! data path descriptor
typedef struct tsMrvDatapathDesc_
{
   UINT16 usOutW;  // width of output picture (after scaling) in pixels
   UINT16 usOutH;  // height of output picture (after scaling) in pixels
   UINT32 ulFlags; // how to configure the datapath. An or'ed combination of the
                   // MRV_DPD_xxx defines
} tsMrvDatapathDesc;

// //////////////////////////////////////////
// possible Frags for the Datapath descriptor

// general features
#define MRV_DPD_DISABLE         0x00000000  // disables the datapath
#define MRV_DPD_ENABLE          0x00000001  // enables the datapath in general
#define MRV_DPD_NORESIZE        0x00000002  // the usOutW and usOutH members will be ignored.  and the
                                            // resize module of the datapath is switched off. Note that
                                            // the resize module is also needed for croma subsampling
#define MRV_DPD_KEEPRATIO       0x00000004  // The input picture from ISP is being cropped to match the
                                            // aspect ratio of the desired output. If this flag is not
                                            // set, different scaling factors for X and Y axis
                                            // may be used.
#define MRV_DPD_MIRROR          0x00000008  // mirror the output picture (only applicable for self path)

// data path mode
#define MRV_DPD_MODE__MASK          0x000000F0  // mode mask (4 bits)
#define MRV_DPD_MODE_ISPRAW_16B     0x00000000  // 16(12) bit raw data from ISP block (only applicable for main path)
#define MRV_DPD_MODE_ISPYC          0x00000010  // separated Y, Cb and Cr data from ISP block
#define MRV_DPD_MODE_ISPRAW         0x00000020  // raw data from ISP block (only applicable for main path)
#define MRV_DPD_MODE_ISPJPEG        0x00000030  // Jpeg encoding with data from ISP block (only applicable for main path)
#define MRV_DPD_MODE_DMAYC_DIRECT   0x00000040  // YCbCr data from system memory directly routed to the main/self path (DMA-read, only applicable for self path)
#define MRV_DPD_MODE_DMAYC_ISP      0x00000050  // YCbCr data from system memory routed through the main processing chain substituting ISP data (DMA-read)
#define MRV_DPD_MODE_DMAJPEG_DIRECT 0x00000060  // YCbCr data from system memory directly routed to the jpeg encoder (DMA-read, R2B-bufferless encoding, only applicable for main path)
#define MRV_DPD_MODE_DMAJPEG_ISP    0x00000070  // Jpeg encoding with YCbCr data from system memory routed through the main processing chain substituting ISP data (DMA-read, only applicable for main path)
#define MRV_DPD_MODE_DPCC           0x00000080  // DPCC bad pixel table output to main path


// additional chroma subsampling (CSS) amount and sample position
#define MRV_DPD_CSS_H__MASK     0x00000700  // horizontal subsampling
#define MRV_DPD_CSS_H_OFF       0x00000000  // no horizontal subsampling
#define MRV_DPD_CSS_H2          0x00000100  // horizontal subsampling by 2
#define MRV_DPD_CSS_H4          0x00000200  // horizontal subsampling by 4
#define MRV_DPD_CSS_HUP2        0x00000500  // 2 times horizontal upsampling
#define MRV_DPD_CSS_HUP4        0x00000600  // 4 times horizontal upsampling
#define MRV_DPD_CSS_V__MASK     0x00003800  // vertical subsampling
#define MRV_DPD_CSS_V_OFF       0x00000000  // no vertical subsampling
#define MRV_DPD_CSS_V2          0x00000800  // vertical subsampling by 2
#define MRV_DPD_CSS_V4          0x00001000  // vertical subsampling by 4
#define MRV_DPD_CSS_VUP2        0x00002800  // 2 times vertical upsampling
#define MRV_DPD_CSS_VUP4        0x00003000  // 4 times vertical upsampling
#define MRV_DPD_CSS_HSHIFT      0x00004000  // apply horizontal chroma phase shift by half the sample distance
#define MRV_DPD_CSS_VSHIFT      0x00008000  // apply vertical chroma phase shift by half the sample distance

// Hardware RGB conversion (currently, only supported for self path)
#define MRV_DPD_HWRGB__MASK     0x00030000  // output mode mask (3 bits, not all combination used yet)
#define MRV_DPD_HWRGB_OFF       0x00000000  // no rgb conversion
#define MRV_DPD_HWRGB_565       0x00010000  // conversion to RGB565
#define MRV_DPD_HWRGB_666       0x00020000  // conversion to RGB666
#define MRV_DPD_HWRGB_888       0x00030000  // conversion to RGB888

// top blackline support
#define MRV_DPD_BLACKLINES_TOP  0x00040000  // If set, blacklines at the top of the sensor are
                                            // shown in the output (if there are any). Note that this
                                            // will move the window of interest out of the center
                                            // to the upper border, so especially at configurations
                                            // with digital zoom, the field of sight is not centered
                                            // on the optical axis anymore. If the sensor does not deliver
                                            // blacklines, setting this bit has no effect.

// DMA-read feature input format. (depends on chip derivative if supported for both pathes, self or not at all)
#define MRV_DPD_DMA_IN__MASK    0x00180000  // input mode mask (2 bits)
#define MRV_DPD_DMA_IN_422      0x00000000  // input is YCbCr 422
#define MRV_DPD_DMA_IN_444      0x00080000  // input is YCbCr 444
#define MRV_DPD_DMA_IN_420      0x00100000  // input is YCbCr 420
#define MRV_DPD_DMA_IN_411      0x00180000  // input is YCbCr 411

// Upscaling interpolation mode (tells how newly created pixels will be interpolated from the existing ones)
#define MRV_DPD_UPSCALE__MASK       0x00200000  // Upscaling interpolation mode mask (2 bits, not all combinations used yet)
#define MRV_DPD_UPSCALE_SMOOTH_LIN  0x00000000  // smooth edges, linear interpolation
#define MRV_DPD_UPSCALE_SHARP       0x00200000  // sharp edges, no interpolation, just duplicate pixels, creates the typical 'blocky' effect.

// interleaved picture format
#define MRV_DPD_PICFORM__MASK   0x00400000 
#define MRV_DPD_PLANAR          0x00000000 
#define MRV_DPD_INTERLEAVED     0x00400000 

// additional luminance phase shift
#define MRV_DPD_LUMA_HSHIFT     0x00800000  // apply horizontal luminance phase shift by half the sample distance
#define MRV_DPD_LUMA_VSHIFT     0x01000000  // apply vertical luminance phase shift by half the sample distance

// picture flipping and rotation
// Note that when combining the flags, the rotation is applied first.
// This enables to configure all 8 possible orientations
#define MRV_DPD_H_FLIP          MRV_DPD_MIRROR  // horizontal flipping - same as mirroring
#define MRV_DPD_V_FLIP          0x02000000      // vertical flipping
#define MRV_DPD_90DEG_CCW       0x04000000      // rotation 90 degrees counter-clockwise

// switch to differentiate between full range of values for YCbCr (0-255)
// and restricted range (16-235 for Y) (16-240 for CbCr)
#define MRV_DPD_YCBCREXT        0x10000000      // if set leads to unrestricted range (0-255) for YCbCr

// package length of a system interface transfer
#define MRV_DPD_BURST__MASK      0x60000000  // burst mask (2 bits)
#define MRV_DPD_BURST_4          0x00000000   // AHB 4 beat burst
#define MRV_DPD_BURST_8          0x20000000   // AHB 8 beat burst
#define MRV_DPD_BURST_16         0x40000000   // AHB 16 beat burst

#endif //#ifdef SL_SUPPORT_PARTLY

#ifdef MRV_SUPPORT_SL

// configures main and self datapathes and scaler for data coming from the ISP
RESULT MrvSls_DatapathIsp( const tsPlSystemConfig *ptSysConf,
                           const tsMrvDatapathDesc *ptMain,
                           const tsMrvDatapathDesc *ptSelf,
                           int iZoom );

// configures main and self datapathes and scaler for data coming from system memory via the DMA read feature
RESULT MrvSls_DatapathDmaRead( const tsPlSystemConfig *ptSysConf,
                               const tsMrvDatapathDesc *ptSource,
                               const tsMrvDatapathDesc *ptMain,
                               const tsMrvDatapathDesc *ptSelf);


// Coordinate transformations:
//
// The pixel data coming from the sensor passes through the ISP output formatter where
// they may be cropped and through the main path scaler where they may be stretched
// and/or squeezed. Thus, the coordinate systems of input and output are different,
// but somewhat related. Further, we can do digital zoom, which adds a third coordinate
// system: the virtual input (e.g. a cropped sensor frame zoomed in to the full sensor
// frame size. Following routines are intended to transform pixel resp. window positions
// from one coordinate systen to another. Folloin coordinate systems exist:
// Cam  : original frame coming from the camera
// VCam : virtual camera; a system in which a cropped original camera frame is up-scaled
//        to the camera frame size. If no digital zoom is to be done, Cam and VCam are
//        identical.
// Main : output of main path
// Self : output of self path

// coordinate transformation from (real) camera coordinate system to main path output
RESULT MrvSls_Transform_Cam2Main( const tsMrvWindow* ptWndIn,
                                  tsMrvWindow* ptWndOut);
// coordinate transformation from (real) camera coordinate system to self path output
RESULT MrvSls_Transform_Cam2Self( const tsMrvWindow* ptWndIn,
                                  tsMrvWindow* ptWndOut);
// coordinate transformation from virtual camera to real camera coordinate system
void MrvSls_Transform_VCam2Cam( const tsIsiSensorConfig *ptIsiSensorConfig,
                                const tsMrvWindow* ptWndIn,
                                tsMrvWindow* ptWndOut);


// ///////////////////////////////////////////////////////////////////////
// Still image snapshot support
// The routine re-configures the main path for taking the snapshot. On
// successful return, the snapshot has been stored in the given memory
// location. Note that the settings of MARVIN will not be restored.
// ///////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/*
 * take the desired snapshot. The type of snapshot (YUV, RAW or JPEG) is determined by
 * the datapath selection bits in tsMrvDatapathDesc::ulFlags. Note that the MARVIN
 * configuration may be changed but will not be restored after the snapshot.
 */
/*****************************************************************************/
RESULT MrvSls_DoSnapshot(const tsPlSystemConfig *ptSysConf,
                         const tsMrvDatapathDesc *ptMain,
                         int iZoom,
                         UINT8 ucJpegCompression,
                         tsMrvMiPathConf* ptMrvMiPathConf);



// ///////////////////////////////////////////////////////////////////////
// Autofocus (AF) measurement and regulation support
// mid-level:
//   different measurement and search algorithms
// high-level:
//   fully performs the control loop with high-level configuration
//   (e.g. measure & search method)
// ///////////////////////////////////////////////////////////////////////

#define MRV_AF_LENSPOS_UNKNOWN 0xffffffff

typedef enum teAfSysTrigger_ {
   eStopSearch, // stop a currently active search
   eOneShot,    // (re-)start a one-shot search
   eContinous   // (re-)start a continous search
} teAfSysTrigger;

/*****************************************************************************/
/*!
 * Autofocus status structure.
 * If autofocus system is enabled, it sends status information to registered
 * submodules through the 'message system'.
 */
/*****************************************************************************/
typedef struct tsMrvAfStatus_
{
    const tsMrvAfMeas *ptMrvAfMeas;  // pointer to current AF measurement values
    UINT32 ulLensPos;  // lens position at which the measurements were taken,
                       // MRV_AF_LENSPOS_UNKNOWN for unknown
    RESULT SearchRes;  // Search result:
                       // RET_BUSY while searching,
                       // RET_IDLE while in continous search, but currently inactive,
                       // RET_SUCCESS if one-shot focus was found,
                       // RET_FAILURE if given up,
                       // RET_CANCELED if canceled by other activity.
    teAfSysTrigger eActiveTrigger; // currently active trigger
} tsMrvAfStatus;

/*
 *  Message IDs segmentation:
 *  IDs for AF:    0x01010000 - 0x02000000
 */
// AF status message:
//  The pMsgData leads to an tsMrvAfStatus structure with current status information
//  of the autofocus system.
#define MRV_SLS_MSG_ID_AF_STATUS     ((UINT32)(0x01010000) | MRV_SLS_MSG_GROUP_AF )     //!< message system message ID for MARVIN AF status


// initialization of mid-level
void   MrvSls_AfMeasSetBuffer( const tsMrvDmaBuffer* ptBuffer );
void   MrvSls_AfSearchInit( UINT32 ulLensPosMin, UINT32 ulLensPosMax );
void   MrvSls_AfSearchReset( void );
// mid-level AF measurement routines
typedef UINT32 (tfAfMeasure)( const tsMrvWindow* ptWindow, INT32 iThreshold );
tfAfMeasure MrvSls_AfMeasTenengrad;
tfAfMeasure MrvSls_AfMeasSqGradient;
tfAfMeasure MrvSls_AfMeasFSWMedian;
tfAfMeasure MrvSls_AfMeasTgNormalized;
tfAfMeasure MrvSls_AfMeasFswmNormalized;
tfAfMeasure MrvSls_AfMeasTgNonDistortedNorm;
tfAfMeasure MrvSls_AfMeasFswmNonDistortedNorm;
// mid-level AF search routines
typedef RESULT (tfAfSearch)( const tsMrvAfMeas *ptMrvAfMeas, UINT32* pulLensPos, BOOL bContinous );
tfAfSearch MrvSls_AfSearchFull;
tfAfSearch MrvSls_AfSearchHillClimb;
tfAfSearch MrvSls_AfSearchAdaptiveRange;
tfAfSearch MrvSls_AfSearchHelimorphOpt;
tfAfSearch MrvSls_AfSearchOV2630LPD4Opt;


// initialization of high-level AF system
void MrvSls_AfSysInit( teMrvAfssMode eAfssMode,
                       teMrvAfmMode eAfmMode,
                       const tsMrvAfConfig* ptAfWnds,
                       void* pYBufAddr,
                       UINT32* pulFocusMax );
// Useful AF utility (gets the biggest value out of the tripel)
UINT32 MrvSls_AfGetSingleVal(const tsMrvAfMeas *ptMrvAfMeas);
// high-level AF measurement
void MrvSls_AfSysMeasure( tsMrvAfMeas        *ptMrvAfMeas,
                          BOOL                printaf_value);
// high-level AF search
void MrvSls_AfSysSearch( const tsMrvAfMeas *ptMrvAfMeas );

// high-level AF trigger, can be used to start/stop an AF search sequence
RESULT MrvSls_AfSysTrigger( teAfSysTrigger eTrigger );
// can be used to retrieve information about the currently active trigger
teAfSysTrigger MrvSls_AfSysGetActiveTrigger( void );

// Initialization of the Bad Pixel Detection and Correction
RESULT MrvSls_BpInit( const tsMrvBpCorrConfig *ptBpCorrConfig, const tsMrvBpDetConfig  *ptBpDetConfig, const BOOL bEnableDetection);
// Bad Pixel Correction
RESULT MrvSls_BpCorrection( void );
// Disable Bad Pixel Correction and dectection
RESULT MrvSls_BpEnd( const tsMrvBpCorrConfig *ptBpCorrConfig);
// Get actual number of elements in bp table
UINT32 MrvSls_BpGetTableNumber( void );
// Get max number of elements of bp table
UINT32 MrvSls_BpGetTableElemNum( void );

#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )
// defines for MrvSlsDpcc_SetBpTable() ucDetectCount:
// Persistent pixel - a pixel with this special detect count is always written into HW table
#define PERSISTENT_PIXEL            (0xFFu)
// max. detect count
#define MAX_DETECT_COUNT            (PERSISTENT_PIXEL - 1)

// Initialization of Defect Pixel Cluster Correction Module
RESULT MrvSlsDpcc_Init( const tsMrvDpccConfig *ptMrvDpccConfig, const tsMrvDpccBptConfig *ptDpccBptConfig );
// Table related processing (on each frame)
RESULT MrvSlsDpcc_ProcessBpt( tsPlSystemConfig *ptSysConf );
// Set a predefined defect pixel table
RESULT MrvSlsDpcc_SetBpTable( UINT16 usPixelCount, const UINT32* pTable, UINT8 ucDetectValue );
// Disable Defect Pixel Cluster Correction Module
RESULT MrvSlsDpcc_End( const tsMrvDpccConfig *ptMrvDpccConfig, const tsMrvDpccBptConfig *ptDpccBptConfig );
#elif ( MARVIN_FEATURE_DPCC != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPCC
#endif  // MARVIN_FEATURE_DPCC

// ************ Section for Denoising PreFilter  ************************
#if( MARVIN_FEATURE_DPF == MARVIN_FEATURE_DPF_V1 )
// possible Flags for the Control Update descriptor
#define MRV_DPF_UPD_DISABLE         0x00000000  // disables the update
#define MRV_DPF_UPD_NL_GAIN         0x00000001  // update of noise level gain
#define MRV_DPF_UPD_STRENGTH        0x00000002  // update of strength with
                                                // analogue gain(a-gain)
#define MRV_DPF_UPD_NLL_TABLE       0x00000004  // update of noise level 
                                                // lookup table with a-gain

// DPF prototypes
// Initialization of Denoising PreFilter Module
RESULT MrvSlsDpf_Init( tsMrvDpfConfig *ptMrvDpfConfig );
// Dynamic Control of Denoising PreFilter Module
RESULT MrvSlsDpf_Control(const tsMrvSlsDpfControl *ptMrvSlsDpfControl);
#elif ( MARVIN_FEATURE_DPF != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPF
#endif  // MARVIN_FEATURE_DPF
// Capture a whole JPEG snapshot
UINT32 MrvSls_JpeCapture( teMrvConfUpdateTime eUpdateTime );
// Encode JPEG without header
RESULT MrvSls_JpeEncode( teMrvConfUpdateTime eUpdateTime, teMrvJpeEncMode tMrvJpeEncMode );
// Encode motion JPEG
RESULT MrvSls_JpeEncMotion( teMrvJpeEncMode tMrvJpeEncMode, UINT16 usFramesNum, UINT32* pulByteCount);

/*****************************************************************************/
/*!
 * functions and definitions for exif header.
 */
/*****************************************************************************/

#define JPEG_HEADER_TO_CUT 20

// generate exif header
RESULT MrvSls_ExifGenerateHeader(const tsPlSystemConfig *ptSysConf, const tsSnapshotConfig *ptSnapConf, char *pExifBuffer, UINT32 *pulSize);
// initialization exif sls support
RESULT MrvSls_InitExif(char **pExifBuffer);
// deinitialization exif sls support
void MrvSls_ExitExif(char *pExifBuffer);


#endif //if MRV_SUPPORT_SL

#ifdef SL_SUPPORT_PARTLY
// Capture a whole JPEG snapshot
UINT32 MrvSls_JpeCapture( teMrvConfUpdateTime eUpdateTime );
// Encode JPEG without header
RESULT MrvSls_JpeEncode( teMrvConfUpdateTime eUpdateTime, teMrvJpeEncMode tMrvJpeEncMode );
// Encode motion JPEG
RESULT MrvSls_JpeEncMotion( teMrvJpeEncMode tMrvJpeEncMode, UINT16 usFramesNum, UINT32* pulByteCount);

#endif

#endif //_MRV_SLS_H
