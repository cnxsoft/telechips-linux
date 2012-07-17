/*****************************************************************************/
/*!
 *  \file        mrv_sls_wb_priv.h \n
 *  \version     1.0 \n
 *  \author      Frommelt \n
 *  \brief       Public header for all second level support routines
 *               of the MARVIN the MARVIN driver. Second level support routines are
 *               those using the plain routines of mrv.h to implement a kind of
 *               "helper" to program/access/use the MARVIN with a bit more
 *               abstraction. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_sls_wb_priv.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_SLS_WB_PRIV_H
#define _MRV_SLS_WB_PRIV_H

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
//! abbreviations for local debug control ( level | module )
#define DERR  ( DBG_ERR  | DBG_MRV )    //!< errors
#define DWARN ( DBG_WARN | DBG_MRV )    //!< warnings
#define DINFO ( DBG_INFO | DBG_MRV )    //!< information

//! limits for out of range tests.
#define DIVMIN     0.00001f  //!< lowest denominator for divisions
#define LOGMIN     0.0001f   //!< lowest value for logarithmic calculations

//! value to initialize the gexp prior filter
#define AWB_GEXP_PRIOR_MIDDLE 0.5f
//! max test cases.
#define AWB_MAX_TESTSTEPS 4
//! max aberration between the calculated and the reference values to set the result to success.
#define AWB_MAX_ABERRATION   0.0001f //  0.000001f
//! the max light profiles the function can handle.
#define AWB_MAX_PROFILECOUNT   10
//! square root of 2 pi
#define SQR2PI    2.506628274631000502415765284811f
//! Max number of addressable profiles
#define MAX_NUMBER_OF_EXISTING_PROFILES 32
//! Array size of crosstalk matrix
#define MATRIX_SIZE 9
//! Array size of color components
#define COMP_MATRIX_SIZE 3
//! Number of profil classes
#define NUMBER_OF_PROFIL_CLASS 3
//! Number of RGB planes
#define NUMBER_OF_RGB_GAIN_PLANES 3
//! Number of red plane in RGB array
#define RGB_GAIN_PLANE_R  0
//! Number of green plane in RGB array
#define RGB_GAIN_PLANE_G  1
//! Number of blue plane in RGB array
#define RGB_GAIN_PLANE_B  2
//! Max length of profil names
#define MAX_NAME_LENGHT 23
//! For initialization purposes
#define GAIN_NORMAL 1

//! Initialization values for near white parameters
#define REF_CR_MAX_R_INIT 128;
#define REF_CB_MAX_B_INIT 128;
#define MIN_C_INIT        20;
#define MAX_C_SUM_INIT    20;
#define MAX_Y_INIT        200;
#define MIN_Y_MAX_G_INIT  30;

/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/
//! structure for awb calculation
typedef struct tsMrvWbValues_
{
    tsIsiAwbFloatMean     tMrvAwbMean;                    //!< YCbCr mean values
    tsIsiCompGain         tMrvAwbMeanRgb;                 //!< RGB values converted from YCbCr
    tsIsi3x3FloatMatrix   tMrvXtalkMatrix;                //!< xtalk matrix
    tsIsi3x3FloatMatrix   tMrvInvXtalkMatrix;             //!< inverse xtalk matrix
    tsIsiXTalkFloatOffset tMrvXtalkOffset;                //!< xtalk offset
    tsIsiCompGain         tMrvAwbMeans;                   //!< reconverted mean values
    tsIsiCompGain         tMrvAwbGains;                   //!< gains from awb register
    UINT16                usMrvAwbPcaFilterSizeMax;       //!< max damping count chosed from menu
    UINT16                usMrvAwbPriorExpFilterSizeMax;  //!< max damping count chosed from menu
    FLOAT                 fExpPriorOut;                   //!< transfers fExpPriorOut from MrvSls_WbCalcRgbGains to MrvSls_WbProcess
}tsMrvWbValues;

//! structure for print purposes and In/Out classification
typedef struct tsMrvProfileInfo_
{
    char cProfileClass[NUMBER_OF_PROFIL_CLASS]; //!< Profile class: I = Input profile, O = Output profile
    char cProfileText[MAX_NAME_LENGHT]; //!< Profile names for print purposes

}tsMrvProfileInfo;

//! context structure for function MrvSls_Wb_Interp1
typedef struct tsMrvWbInterp1Ctx_
{
    FLOAT*  paf_x;       //!< x-array, lookup table
    FLOAT*  paf_Y;       //!< Y-array
    UINT16 usArraySize; //!< size of above arrays
    FLOAT  f_xi;        //!< value to find matching interval for in x-array
    FLOAT  f_Yi;        //!< result calculated from f_xi, paf_x and paf_Y
}tsMrvWbInterp1Ctx;

/*****************************************************************************
 * GLOBALS
 *****************************************************************************/
//! the distance of a calculated value to the center is assigned to one of three regions
enum RegionType
{
    eRegionA, //!< Only one lightsource selected
    eRegionB, //!< Interpolation of region A and Region B selected
    eRegionC  //!< Selection of a mixture of 3 or 4 lightsources
};

 //! data for print purposes and In/Out classification
tsMrvProfileInfo tMrvProfileInfos[MAX_NUMBER_OF_EXISTING_PROFILES] =
{
    "I","Inprof  CIE_A         ",
    "I","Inprof  CIE_B         ",
    "I","Inprof  CIE_C         ",
    "O","Outprof CIE_D50       ",
    "O","Outprof CIE_D55       ",
    "I","Inprof CIE_D65        ",
    "O","Outprof CIE_D75       ",
    "O","Outprof CIE_E         ",
    "I","not defined           ",
    "I","not defined           ",
    "I","not defined           ",
    "O","Outprof Twilight      ",
    "I","not defined           ",
    "I","not defined           ",
    "O","Outprof Shade         ",
    "O","Outprof Day           ",
    "I","Inprof  CIE_F1        ",
    "I","Inprof  CIE_F2  (CWF) ",
    "I","Inprof  CIE_F3        ",
    "I","Inprof  CIE_F4        ",
    "I","Inprof  CIE_F5        ",
    "I","Inprof  CIE_F6        ",
    "I","Inprof  CIE_F7        ",
    "I","Inprof  CIE_F8        ",
    "I","Inprof  CIE_F9        ",
    "I","Inprof  CIE_F10 (TL85)",
    "I","Inprof  CIE_F11 (TL84)",
    "I","Inprof  CIE_F12 (TL83)",
    "O","Outprof Cloudy        ",
    "O","Outprof Sunny         ",
    "I","not defined           ",
    "I","Default Profile       ",
};

 //! constants for conversion from Y,Cb,Cr to R,G,B
FLOAT YuvRgbCoeff[]  =
{
    1.1636025f,  - 0.0622839f,    1.6007823f,
    1.1636025f,  - 0.4045270f,  - 0.7949191f,
    1.1636025f,    1.9911744f,  - 0.0250092f
};

//! flag is true if the push button is pressed.
BOOL   bTriggerPushWb;

/* for the basic version the numbers of available sensor profiles and the filter size
 * are fixed. In later versions the space should be allocated dynamic.
 */
//! counter for prior exp damping filter
UINT16 usAwbPriorExpFilterCount;
//! coefficient for AWB damping filter
FLOAT fAwbIIRDampCoef;
//! damped gain values
tsIsiCompGain tAwbGainDamped;
//! damped matrix coefficients
FLOAT  fMrvMatrixCoeffDamped[MATRIX_SIZE];
//! damped gain offsets
tsIsiXTalkFloatOffset tMrvOffsetDamped;
//! damping filter for fGExp prior values
FLOAT  fExpPriorInFilter[AWB_DAMPING_FILTER_SIZE_MAX];
//! structure for data transfer AE -> SLS support
tsMrvAEtoSLS tMrvAEtoSLS;

//! testcase counter
UINT16 usTCCount;
//! testcase error counter
UINT16 usTCErrCount;
/*****************************************************************************
 * PRIVATE PROTOTYPES
 *****************************************************************************/
RESULT MrvSls_WbProcess(const tsConfig *ptConfig);

#endif //_MRV_SLS_WB_PRIV_H
