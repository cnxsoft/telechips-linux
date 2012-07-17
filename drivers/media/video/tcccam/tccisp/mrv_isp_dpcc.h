/*****************************************************************************/
/*!
 *  \file        mrv_isp_dpcc.h \n
 *  \version     1.0 \n
 *  \author      Meinicke \n
 *  \brief       Defect Pixel Cluster Correction driver, public header. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_isp_dpcc.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_ISP_DPCC_H
#define _MRV_ISP_DPCC_H

#if( MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1 )

#define DPCC_FILENAME_BUFSZ 50 // Size of DPCC configuration file name buffer
#define DPCC_PARAMETER_SET_COUNT  (6)   // number of parameter sets supported

/********************************
 * public variables
 ********************************/

 /* Settings to be read from DPCC configuration file */

//[DPCC_MODE]
typedef struct
{
    UINT32 Stage3Enable;
    UINT32 Stage2Enable;
    UINT32 Stage1Enable;
    UINT32 GrayscaleMode;
    // UINT32 DPCC_ENABLE; put into global configuration file
}tsDpccMode;

//[DPCC_OUTPUT_MODE]
typedef struct
{
    UINT32 Stage3Rb3x3;
    UINT32 Stage3G3x3;
    UINT32 Stage3InclRbCenter;
    UINT32 Stage3InclGreenCenter;
    UINT32 Stage2Rb3x3;
    UINT32 Stage2G3x3;
    UINT32 Stage2InclRbCenter;
    UINT32 Stage2InclGreenCenter;
    UINT32 Stage1Rb3x3;
    UINT32 Stage1G3x3;
    UINT32 Stage1InclRbCenter;
    UINT32 Stage1InclGreenCenter;
}tsDpccOutputMode;

//[DPCC_SET_USE]
typedef struct
{
    UINT32 Stage3UseFixSet;
    UINT32 Stage3UseSet3;
    UINT32 Stage3UseSet2;
    UINT32 Stage3UseSet1;
    UINT32 Stage2UseFixSet;
    UINT32 Stage2UseSet3;
    UINT32 Stage2UseSet2;
    UINT32 Stage2UseSet1;
    UINT32 Stage1UseFixSet;
    UINT32 Stage1UseSet3;
    UINT32 Stage1UseSet2;
    UINT32 Stage1UseSet1;
}tsDpccSetUse;

//[DPCC_METHODS_SET1]
typedef struct
{
    UINT32 RgRedBlue1Enable;
    UINT32 RndRedBlue1Enable;
    UINT32 RoRedBlue1Enable;
    UINT32 LcRedBlue1Enable;
    UINT32 PgRedBlue1Enable;
    UINT32 RgGreen1Enable;
    UINT32 RndGreen1Enable;
    UINT32 RoGreen1Enable;
    UINT32 LcGreen1Enable;
    UINT32 PgGreen1Enable;
}tsDpccMethodsSet1;

//[DPCC_METHODS_SET2]
typedef struct
{
    UINT32 RgRedBlue2Enable;
    UINT32 RndRedBlue2Enable;
    UINT32 RoRedBlue2Enable;
    UINT32 LcRedBlue2Enable;
    UINT32 PgRedBlue2Enable;
    UINT32 RgGreen2Enable;
    UINT32 RndGreen2Enable;
    UINT32 RoGreen2Enable;
    UINT32 LcGreen2Enable;
    UINT32 PgGreen2Enable;
}tsDpccMethodsSet2;

//[DPCC_METHODS_SET3]
typedef struct
{
    UINT32 RgRedBlue3Enable;
    UINT32 RndRedBlue3Enable;
    UINT32 RoRedBlue3Enable;
    UINT32 LcRedBlue3Enable;
    UINT32 PgRedBlue3Enable;
    UINT32 RgGreen3Enable;
    UINT32 RndGreen3Enable;
    UINT32 RoGreen3Enable;
    UINT32 LcGreen3Enable;
    UINT32 PgGreen3Enable;
}tsDpccMethodsSet3;

//[DPCC_LINE_THRESH_1]
typedef struct
{
    UINT32 LineThr1Rb;
    UINT32 LineThr1G;
}tsDpccLineThresh1;

//[DPCC_LINE_MAD_FAC_1]
typedef struct
{
    UINT32 LineMadFac1Rb;
    UINT32 LineMadFac1G;
}tsDpccLineMadFac1;

//[DPCC_PG_FAC_1]
typedef struct
{
    UINT32 PgFac1Rb;
    UINT32 PgFac1G;
}tsDpccPgFac1;

//[DPCC_RND_THRESH_1]
typedef struct
{
    UINT32 RndThr1Rb;
    UINT32 RndThr1G;
}tsDpccRndThresh1;

//[DPCC_RG_FAC_1]
typedef struct
{
    UINT32 RgFac1Rb;
    UINT32 RgFac1G;
}tsDpccRgFac1;

//[DPCC_LINE_THRESH_2]
typedef struct
{
    UINT32 LineThr2Rb;
    UINT32 LineThr2G;
}tsDpccLineThresh2;

//[DPCC_LINE_MAD_FAC_2]
typedef struct
{
    UINT32 LineMadFac2Rb;
    UINT32 LineMadFac2G;
}tsDpccLineMadFac2;

//[DPCC_PG_FAC_2]
typedef struct
{
    UINT32 PgFac2Rb;
    UINT32 PgFac2G;
}tsDpccPgFac2;

//[DPCC_RND_THRESH_2]
typedef struct
{
    UINT32 RndThr2Rb;
    UINT32 RndThr2G;
}tsDpccRndThresh2;

//[DPCC_RG_FAC_2]
typedef struct
{
    UINT32 RgFac2Rb;
    UINT32 RgFac2G;
}tsDpccRgFac2;

//[DPCC_LINE_THRESH_3]
typedef struct
{
    UINT32 LineThr3Rb;
    UINT32 LineThr3G;
}tsDpccLineThresh3;

//[DPCC_LINE_MAD_FAC_3]
typedef struct
{
    UINT32 LineMadFac3Rb;
    UINT32 LineMadFac3G;
}tsDpccLineMadFac3;

//[DPCC_PG_FAC_3]
typedef struct
{
    UINT32 PgFac3Rb;
    UINT32 PgFac3G;
}tsDpccPgFac3;

//[DPCC_RND_THRESH_3]
typedef struct
{
    UINT32 RndThr3Rb;
    UINT32 RndThr3G;
}tsDpccRndThresh3;

//[DPCC_RG_FAC_3]
typedef struct
{
    UINT32 RgFac3Rb;
    UINT32 RgFac3G;
}tsDpccRgFac3;

//[DPCC_RO_LIMITS]
typedef struct
{
    UINT32 RoLim3Rb;
    UINT32 RoLim3G;
    UINT32 RoLim2Rb;
    UINT32 RoLim2G;
    UINT32 RoLim1Rb;
    UINT32 RoLim1G;
}tsDpccRoLimits;

//[DPCC_RND_OFFS]
typedef struct
{
    UINT32 RndOffs3Rb;
    UINT32 RndOffs3G;
    UINT32 RndOffs2Rb;
    UINT32 RndOffs2G;
    UINT32 RndOffs1Rb;
    UINT32 RndOffs1G;
}tsDpccRndOffs;

//[DPCC_BPT_MODE]
typedef struct
{
    UINT32 BptRb3x3;
    UINT32 BptG3x3;
    UINT32 BptInclRbCenter;
    UINT32 BptInclGreenCenter;
    UINT32 BptUseFixSet;
    UINT32 BptUseSet3;
    UINT32 BptUseSet2;
    UINT32 BptUseSet1;
    UINT32 BptCorEn;
    UINT32 BptDetEn;
}tsDpccBptCtrl;

//registers forming a parameter set
typedef struct
{
    // Settings to be read in from DPCC configuration file, where they are stored slice-wise
    tsDpccMode          tDpccMode;        // Mode control for DPCC block                  (rw) MRV_DPCC_BASE + 0x0000
    tsDpccOutputMode    tDpccOutputMode;  // Output Interpolation mode                    (rw) MRV_DPCC_BASE + 0x0004
    tsDpccSetUse        tDpccSetUse;      // DPCC parameter set use                       (rw) MRV_DPCC_BASE + 0x0008
    tsDpccMethodsSet1   tDpccMethodsSet1; // Methods enables for set 1                    (rw) MRV_DPCC_BASE + 0x000C
    tsDpccMethodsSet2   tDpccMethodsSet2; // Methods enables for set 2                    (rw) MRV_DPCC_BASE + 0x0010
    tsDpccMethodsSet3   tDpccMethodsSet3; // Methods enables for set 3                    (rw) MRV_DPCC_BASE + 0x0014
    tsDpccLineThresh1   tDpccLineThresh1; // Line threshold set 1                         (rw) MRV_DPCC_BASE + 0x0018
    tsDpccLineMadFac1   tDpccLineMadFac1; // MAD factor for Line check set 1              (rw) MRV_DPCC_BASE + 0x001C
    tsDpccPgFac1        tDpccPgFac1;      // Peak gradient factor for set 1               (rw) MRV_DPCC_BASE + 0x0020
    tsDpccRndThresh1    tDpccRndThresh1;  // Rank Neighbor Difference threshold for set 1 (rw) MRV_DPCC_BASE + 0x0024
    tsDpccRgFac1        tDpccRgFac1;      // Rank gradient factor for set 1               (rw) MRV_DPCC_BASE + 0x0028
    tsDpccLineThresh2   tDpccLineThresh2; // Line threshold set 2                         (rw) MRV_DPCC_BASE + 0x002C
    tsDpccLineMadFac2   tDpccLineMadFac2; // MAD factor for Line check set 2              (rw) MRV_DPCC_BASE + 0x0030
    tsDpccPgFac2        tDpccPgFac2;      // Peak gradient factor for set 2               (rw) MRV_DPCC_BASE + 0x0034
    tsDpccRndThresh2    tDpccRndThresh2;  // Rank Neighbor Difference threshold for set 2 (rw) MRV_DPCC_BASE + 0x0038
    tsDpccRgFac2        tDpccRgFac2;      // Rank gradient factor for set 2               (rw) MRV_DPCC_BASE + 0x003C
    tsDpccLineThresh3   tDpccLineThresh3; // Line threshold set 3                         (rw) MRV_DPCC_BASE + 0x0040
    tsDpccLineMadFac3   tDpccLineMadFac3; // MAD factor for Line check set 3              (rw) MRV_DPCC_BASE + 0x0044
    tsDpccPgFac3        tDpccPgFac3;      // Peak gradient factor for set 3               (rw) MRV_DPCC_BASE + 0x0048
    tsDpccRndThresh3    tDpccRndThresh3;  // Rank Neighbor Difference threshold for set 3 (rw) MRV_DPCC_BASE + 0x004C
    tsDpccRgFac3        tDpccRgFac3;      // Rank gradient factor for set 3               (rw) MRV_DPCC_BASE + 0x0050
    tsDpccRoLimits      tDpccRoLimits;    // Rank Order Limits                            (rw) MRV_DPCC_BASE + 0x0054
    tsDpccRndOffs       tDpccRndOffs;     // Differential Rank Offsets                    (rw) MRV_DPCC_BASE + 0x0058
}tsDpccParameterSet;

typedef struct
{
    // Settings to be read in from global configuration file
    char   szCfgFileName[DPCC_FILENAME_BUFSZ];// Name of DPCC configuration file
    BOOL   bDpccEnable;                       // DPCC on/off
    UINT8  ucParameterSet;                    // parameter set to use; less than DPCC_PARAMETER_SET_COUNT
    UINT16 usVerboseLevel;                    // verbose level (bit mask)

    tsDpccParameterSet  atSet[DPCC_PARAMETER_SET_COUNT]; // parameter sets
    tsDpccBptCtrl       tDpccBptCtrl;     // Bad Pixel Table Settings                     (rw) MRV_DPCC_BASE + 0x005C
}tsMrvDpccConfig;


typedef enum
{
	eBptCorrectionDisable,
	eBptCorrectionEnable,
	eBptCorrectionKeepUnchanged
}teMrvDpccBptCorrectionEnable;

/********************************
 * public functions
 ********************************/
RESULT MrvIspDpcc_Enable( void );
RESULT MrvIspDpcc_Disable( void );
RESULT MrvIspDpcc_BptDetection( const BOOL bActive );
RESULT MrvIspDpcc_BptCorrection( const BOOL bActive );
RESULT MrvIspDpcc_SetConfig( const tsMrvDpccConfig *ptMrvDpccConfig, UINT8 ucParameterSet );
RESULT MrvIspDpcc_GetConfig( tsMrvDpccConfig *ptMrvDpccConfig );

// following three functions are used by mrv_sls_dpcc to write values step by step
RESULT MrvIspDpccBptWriteTableStart(UINT32 ulBpNumber, UINT32* pRetBptCtrlSave);
RESULT MrvIspDpccBptWriteTableValue(UINT32 ulBpValue);
RESULT MrvIspDpccBptWriteTableEnd(UINT32 ulBptCtrlSave, BOOL bCancel,
                                  teMrvDpccBptCorrectionEnable eEnable);

#elif ( MARVIN_FEATURE_DPCC != MARVIN_FEATURE_EXIST_NOT )
#error unknown value for MARVIN_FEATURE_DPCC
#endif  // MARVIN_FEATURE_DPCC

#endif // _MRV_ISP_DPCC_H

