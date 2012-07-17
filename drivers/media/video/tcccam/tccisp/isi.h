/*****************************************************************************/
/*!
 *  \file        isi.h \n
 *  \version     4.1 \n
 *  \author      Schuster \n
 *  \brief       Public header file for the ISI interface. \n
 *
 *  \revision    $Revision: 463 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-07-07 19:43:29 +0200 (Di, 07 Jul 2009) $ \n
 *               $Id: isi.h 463 2009-07-07 17:43:29Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _ISI_H
#define _ISI_H

/*****************************************************************************/
/*!
 * interface version
 * =================
 *please increment the version if you add something new to the interface. This helps
 *upper layer software to deal with different interface versions.
 */
/*****************************************************************************/
#define ISI_INTERFACE_VERSION 4

/////////////////////////////////////////////////////////////////////
// capabilities / configuration
/////////////////////////////////////////////////////////////////////

//all capabilites of all supported sensor types

// **** PLEASE NOTE ****
// If you add some capabilities here, please update the textual description
// array s_atCapDesc in isi_2str.c too. By doing so, you usually don't have to
// add further debug-string or user-interface code for the just added capability.
// It should work out of the box.

//! ulBusWidth;
#define ISI_BUSWIDTH_8BIT_ZZ    0x00000001 //to expand to a (possibly higher) resolution in marvin, the LSBs will be set to zero
#define ISI_BUSWIDTH_8BIT_EX    0x00000002 //to expand to a (possibly higher) resolution in marvin, the LSBs will be copied from the MSBs
#define ISI_BUSWIDTH_10BIT_EX   0x00000004 //formerly known as ISI_BUSWIDTH_10BIT (at times no marvin derivative was able to process more than 10 bit)
#define ISI_BUSWIDTH_10BIT_ZZ   0x00000008
#define ISI_BUSWIDTH_12BIT      0x00000010

#define ISI_BUSWIDTH_10BIT      ISI_BUSWIDTH_10BIT_EX


//! ulMode, operating mode of the image sensor in termns of output data format and
//! timing data transmission
#define ISI_MODE_BT601            0x00000001 // YUV-Data with separate h/v sync lines (ITU-R BT.601)
#define ISI_MODE_BT656            0x00000002 // YUV-Data with sync words inside the datastream (ITU-R BT.656)
#define ISI_MODE_BAYER            0x00000004 // Bayer data with separate h/v sync lines
#define ISI_MODE_DATA             0x00000008 // Any binary data without line/column-structure, (e.g. already JPEG encoded) h/v sync lines act as data valid signals
#define ISI_MODE_PICT             0x00000010 // RAW picture data with separate h/v sync lines
#define ISI_MODE_RGB565           0x00000020 // RGB565 data with separate h/v sync lines
#define ISI_MODE_SMIA             0x00000040 // SMIA conform data stream (see ulSmiaMode for details)
#define ISI_MODE_MIPI             0x00000080 // MIPI conform data stream (see ulMipiMode for details)
#define ISI_MODE_BAY_BT656        0x00000100 // Bayer data with sync words inside the datastream (similar to ITU-R BT.656)
#define ISI_MODE_RAW_BT656        0x00000200 // Raw picture data with sync words inside the datastream (similar to ITU-R BT.656)


//! ulSmiaMode
//#define ISI_SMIA_MODE_COMPRESSED           0x00000001  // compression mode
//#define ISI_SMIA_MODE_RAW_8_TO_10_DECOMP   0x00000002  // 8bit to 10 bit decompression
//#define ISI_SMIA_MODE_RAW_12               0x00000004  // 12 bit RAW Bayer Data
//#define ISI_SMIA_MODE_RAW_10               0x00000008  // 10 bit RAW Bayer Data
//#define ISI_SMIA_MODE_RAW_8                0x00000010  // 8 bit RAW Bayer Data
//#define ISI_SMIA_MODE_RAW_7                0x00000020  // 7 bit RAW Bayer Data
//#define ISI_SMIA_MODE_RAW_6                0x00000040  // 6 bit RAW Bayer Data
//#define ISI_SMIA_MODE_RGB_888              0x00000080  // RGB 888 Display ready Data
//#define ISI_SMIA_MODE_RGB_565              0x00000100  // RGB 565 Display ready Data
//#define ISI_SMIA_MODE_RGB_444              0x00000200  // RGB 444 Display ready Data
//#define ISI_SMIA_MODE_YUV_420              0x00000400  // YUV420 Data
//#define ISI_SMIA_MODE_YUV_422              0x00000800  // YUV422 Data
//#define ISI_SMIA_OFF                       0x80000000  // SMIA is disabled

//! ulMipiMode
#define ISI_MIPI_MODE_YUV420_8             0x00000001  // YUV 420  8-bit
#define ISI_MIPI_MODE_YUV420_10            0x00000002  // YUV 420 10-bit
#define ISI_MIPI_MODE_LEGACY_YUV420_8      0x00000004  // Legacy YUV 420 8-bit
#define ISI_MIPI_MODE_YUV420_CSPS_8        0x00000008  // YUV 420 8-bit (CSPS)
#define ISI_MIPI_MODE_YUV420_CSPS_10       0x00000010  // YUV 420 10-bit (CSPS)
#define ISI_MIPI_MODE_YUV422_8             0x00000020  // YUV 422 8-bit
#define ISI_MIPI_MODE_YUV422_10            0x00000040  // YUV 422 10-bit
#define ISI_MIPI_MODE_RGB444               0x00000080  // RGB 444
#define ISI_MIPI_MODE_RGB555               0x00000100  // RGB 555
#define ISI_MIPI_MODE_RGB565               0x00000200  // RGB 565
#define ISI_MIPI_MODE_RGB666               0x00000400  // RGB 666
#define ISI_MIPI_MODE_RGB888               0x00000800  // RGB 888
#define ISI_MIPI_MODE_RAW_6                0x00001000  // RAW_6
#define ISI_MIPI_MODE_RAW_7                0x00002000  // RAW_7
#define ISI_MIPI_MODE_RAW_8                0x00004000  // RAW_8
#define ISI_MIPI_MODE_RAW_10               0x00008000  // RAW_10
#define ISI_MIPI_MODE_RAW_12               0x00010000  // RAW_12
#define ISI_MIPI_OFF                       0x80000000  // MIPI is disabled


	//! ulFieldInv;
	#define ISI_FIELDINV_NOSWAP     0x00000001
	#define ISI_FIELDINV_SWAP       0x00000002

	//! ulFieldSel;
	#define ISI_FIELDSEL_BOTH       0x00000001
	#define ISI_FIELDSEL_EVEN       0x00000002
	#define ISI_FIELDSEL_ODD        0x00000004

	//! ulYCSeq;
	#define ISI_YCSEQ_YCBYCR        0x00000001
	#define ISI_YCSEQ_YCRYCB        0x00000002
	#define ISI_YCSEQ_CBYCRY        0x00000004
	#define ISI_YCSEQ_CRYCBY        0x00000008

	//! ulConv422;
	#define ISI_CONV422_COSITED     0x00000001
	#define ISI_CONV422_INTER       0x00000002
	#define ISI_CONV422_NOCOSITED   0x00000004

	//! ulBPat;
	#define ISI_BPAT_RGRGGBGB       0x00000001
	#define ISI_BPAT_GRGRBGBG       0x00000002
	#define ISI_BPAT_GBGBRGRG       0x00000004
	#define ISI_BPAT_BGBGGRGR       0x00000008

	//! ulHPol;
	#define ISI_HPOL_SYNCPOS        0x00000001 //sync signal pulses high between lines
	#define ISI_HPOL_SYNCNEG        0x00000002 //sync signal pulses low between lines
	#define ISI_HPOL_REFPOS         0x00000004 //reference signal is high as long as sensor puts out line data
	#define ISI_HPOL_REFNEG         0x00000008 //reference signal is low as long as sensor puts out line data

	//! ulVPol;
	#define ISI_VPOL_POS            0x00000001
	#define ISI_VPOL_NEG            0x00000002

	//! ulEdge;
	#define ISI_EDGE_RISING         0x00000001
	#define ISI_EDGE_FALLING        0x00000002


	//! ulBls;
	#define ISI_BLS_OFF             0x00000001 //turns on/off additional black lines at frame start
	#define ISI_BLS_TWO_LINES       0x00000002
	#define ISI_BLS_FOUR_LINES      0x00000004  // two lines top and two lines bottom
	#define ISI_BLS_SIX_LINES       0x00000008  // six lines top

	//! ulGamma;
	#define ISI_GAMMA_ON            0x00000001 //turns on/off gamma correction in the sensor ISP
	#define ISI_GAMMA_OFF           0x00000002

	//! ulCConv;
	#define ISI_CCONV_ON            0x00000001 //turns on/off color conversion matrix in the sensor ISP
	#define ISI_CCONV_OFF           0x00000002

	//! ulRes;
	#define ISI_RES_QQCIF           0x00000001 //1  88x72
	#define ISI_RES_QQVGA           0x00000002 //2  160x120
	#define ISI_RES_QCIF            0x00000004 //3  176x144
	#define ISI_RES_QVGA            0x00000008 //4  320x240
	#define ISI_RES_CIF             0x00000010 //5  352x288
	#define ISI_RES_VGA             0x00000020 //6  640x480
	#define ISI_RES_SVGA            0x00000040 //7  800x600
	#define ISI_RES_XGA             0x00000080 //8  1024x768
	#define ISI_RES_XGA_PLUS        0x00000100 //9  1280x960 max. resolution of OV9640 (QuadVGA)
	#define ISI_RES_SXGA            0x00000200 //10 1280x1024
	#define ISI_RES_UXGA            0x00000400 //11 1600x1200
	#define ISI_RES_QXGA            0x00000800 //12 2048x1536
	#define ISI_RES_QSXGA_2592_1944 0x00001000 //13 2592x1944 max. resolution of OV5630, IMX034PQH5, S5K4E1G(a bit more than QuadSXGA)
	#define ISI_RES_RAWMAX          0x00002000 //14 4096x2048
	#define ISI_RES_YUV_HMAX        0x00004000 //15 4080x1024
	#define ISI_RES_YUV_VMAX        0x00008000 //16 1024x4080
	#ifdef _DEBUG
	    #define ISI_RES_TEST        0x00010000 //17 depends on further defines (TEST_SIZE_H and TEST_SIZE_V)

	    #define TEST_SIZE_H         (2600)
	    #define TEST_SIZE_V         (2046)
	    //#define TEST_SIZE_V         (1950)
	#endif // #ifdef _DEBUG
	#define ISI_RES_L_AFM           0x00020000 //18 720x480
	#define ISI_RES_M_AFM           0x00040000 //19 128x96
	#define ISI_RES_S_AFM           0x00080000 //20 64x32
	#define ISI_RES_BP1             0x00100000 //21 352x240
	#define ISI_RES_QSXGA           0x00200000 //22 2586x2048, quadruple SXGA, 5,3 Mpix
	#define ISI_RES_QSXGA_PLUS      0x00400000 //23 2600x2048, max. resolution of M5, 5,32 Mpix
	#define ISI_RES_QSXGA_PLUS2     0x00800000 //24 2600x1950
	#define ISI_RES_QSXGA_PLUS3     0x01000000 //25 2686x2048,  5.30M
	#define ISI_RES_WQSXGA          0x02000000 //26 3200x2048,  6.56M
	#define ISI_RES_QUXGA           0x04000000 //27 3200x2400,  7.68M
	#define ISI_RES_WQUXGA          0x08000000 //28 3840x2400,  9.22M
	#define ISI_RES_HXGA            0x10000000 //29 4096x3072, 12.59M
	#define ISI_RES_XGA_1280_720    0x20000000 //30 1280x720 second resolution of S5K4E1G

	#define QQCIF_SIZE_H              (88)
	#define QQCIF_SIZE_V              (72)
	#define QQVGA_SIZE_H             (160)
	#define QQVGA_SIZE_V             (120)
	#define QCIF_SIZE_H              (176)
	#define QCIF_SIZE_V              (144)
	#define QVGA_SIZE_H              (320)
	#define QVGA_SIZE_V              (240)
	#define CIF_SIZE_H               (352)
	#define CIF_SIZE_V               (288)
	#define VGA_SIZE_H               (640)
	#define VGA_SIZE_V               (480)
	#define SVGA_SIZE_H              (800)
	#define SVGA_SIZE_V              (600)
	#define XGA_SIZE_H              (1024)
	#define XGA_SIZE_V               (768)
	#define XGA_PLUS_SIZE_H         (1280)
	#define XGA_PLUS_SIZE_V          (960)
	#define XGA_1280_720_SIZE_H     (1280)
	#define XGA_1280_720_SIZE_V     (720)
	#define SXGA_SIZE_H             (1280)
	#define SXGA_SIZE_V             (1024)
	#define QSVGA_SIZE_H            (1600)  // will be removed soon
	#define QSVGA_SIZE_V            (1200)  // will be removed soon
	#define UXGA_SIZE_H             (1600)
	#define UXGA_SIZE_V             (1200)
	#define QXGA_SIZE_H             (2048)
	#define QXGA_SIZE_V             (1536)
	#define QSXGA_2592_1944_SIZE_H  (2592)
	#define QSXGA_2592_1944_SIZE_V  (1944)
	#define RAWMAX_SIZE_H           (4096)
	#define RAWMAX_SIZE_V           (2048)
	#define YUV_HMAX_SIZE_H         (4080)
	#define YUV_HMAX_SIZE_V         (1024)
	#define YUV_VMAX_SIZE_H         (1024)
	#define YUV_VMAX_SIZE_V         (4080)
	#define BP1_SIZE_H               (352)
	#define BP1_SIZE_V               (240)
	#define L_AFM_SIZE_H             (720)
	#define L_AFM_SIZE_V             (480)
	#define M_AFM_SIZE_H             (128)
	#define M_AFM_SIZE_V              (96)
	#define S_AFM_SIZE_H              (64)
	#define S_AFM_SIZE_V              (32)
	#define QSXGA_SIZE_H            (2560)
	#define QSXGA_SIZE_V            (2048)
	#define QSXGA_MINUS_SIZE_V      (1920)
	#define QSXGA_PLUS_SIZE_H       (2600)
	#define QSXGA_PLUS_SIZE_V       (2048)
	#define QSXGA_PLUS2_SIZE_H      (2600)
	#define QSXGA_PLUS2_SIZE_V      (1950)
	#define QUXGA_SIZE_H            (3200)
	#define QUXGA_SIZE_V            (2400)
	#define SIZE_H_2500             (2500)
	#define QSXGA_PLUS3_SIZE_H      (2686)
	#define QSXGA_PLUS3_SIZE_V      (2048)
	#define WQSXGA_SIZE_H           (3200)
	#define WQSXGA_SIZE_V           (2048)
	#define WQUXGA_SIZE_H           (3200)
	#define WQUXGA_SIZE_V           (2400)
	#define HXGA_SIZE_H             (4096)
	#define HXGA_SIZE_V             (3072)

#if 0
	//! ulDwnSz
	#define ISI_DWNSZ_SUBSMPL       0x00000001 //Use subsampling to downsize output window
	#define ISI_DWNSZ_SCAL_BAY      0x00000002 //Use scaling with Bayer sampling to downsize output window
	#define ISI_DWNSZ_SCAL_COS      0x00000004 //Use scaling with co-sited sampling to downsize output window

	//! ulBLC;
	#define ISI_BLC_AUTO            0x00000001 //Camera BlackLevelCorrection on
	#define ISI_BLC_OFF             0x00000002 //Camera BlackLevelCorrection off

	//! ulAGC;
	#define ISI_AGC_AUTO            0x00000001 //Camera AutoGainControl on
	#define ISI_AGC_OFF             0x00000002 //Camera AutoGainControl off

	//! ulAWB;
	#define ISI_AWB_AUTO            0x00000001 //Camera AutoWhiteBalance on
	#define ISI_AWB_OFF             0x00000002 //Camera AutoWhiteBalance off

	//! ulAEC;
	#define ISI_AEC_AUTO            0x00000001 //Camera AutoExposureControl on
	#define ISI_AEC_OFF             0x00000002 //Camera AutoExposureControl off

	//! ulAecMode;
	#define ISI_AEC_MODE_STAND      0x00000001 // SW AEC: no motions
	#define ISI_AEC_MODE_SLOW       0x00000002 // SW AEC: slow motions
	#define ISI_AEC_MODE_FAST       0x00000004 // SW AEC: fast motions
	#define ISI_AEC_MODE_NORMAL     0x00000008 // SW AEC: 'normal' motions

	//! ulCieProfile
	// according to http://www.hunterlab.com/appnotes/an05_05.pdf, illuminants
	// A, D65 and F2 are most commonly used and should be selected prior to the
	// others if only a subset is to be supported. Illuminants B and E are mentioned
	// here: http://www.aim-dtp.net/aim/technology/cie_xyz/cie_xyz.htm.
	#define ISI_CIEPROF_A           0x00000001 // incandescent/tungsten,                            2856K
	#define ISI_CIEPROF_B           0x00000002 // Direct sunlight
	#define ISI_CIEPROF_C           0x00000004 // average sky daylight,                             6774K
	#define ISI_CIEPROF_D50         0x00000008 // horizon light,                                    5000K
	#define ISI_CIEPROF_D55         0x00000010 // mid morning daylight,                             5500K
	#define ISI_CIEPROF_D65         0x00000020 // indoor D65 daylight from fluorescent lamp,        6504K
	#define ISI_CIEPROF_D75         0x00000040 // overcast daylight,                                7500K
	#define ISI_CIEPROF_E           0x00000080 // Normalized reference source
	#define ISI_CIEPROF_FLUOR       0x00000100 // Fluorescent                                       4000K
	#define ISI_CIEPROF_FLUORH      0x00000200 // FluorescentH                                      3200K
	#define ISI_CIEPROF_TUNG        0x00000400 // Tungsten                                          3200K
	#define ISI_CIEPROF_TWI         0x00000800 // Twilight
	#define ISI_CIEPROF_SUN         0x00001000 // Sunset
	#define ISI_CIEPROF_FLASH       0x00002000 // Flashlight                                  5000K-7000K
	#define ISI_CIEPROF_SHADE       0x00004000 // Shade outdoor light                               7000K
	#define ISI_CIEPROF_DAY         0x00008000 // outdoor D65 daylight                              6500K
	#define ISI_CIEPROF_F1          0x00010000 // daylight flourescent                              6430K
	#define ISI_CIEPROF_F2          0x00020000 // cool white flourescent CWF                        4230K
	#define ISI_CIEPROF_F3          0x00040000 // white fluorescent                                 3450K
	#define ISI_CIEPROF_F4          0x00080000 // warm white flourescent                            2940K
	#define ISI_CIEPROF_F5          0x00100000 // daylight flourescent                              6350K
	#define ISI_CIEPROF_F6          0x00200000 // lite white flourescent                            4150K
	#define ISI_CIEPROF_F7          0x00400000 // similar to D65, daylight flourescent              6500K
	#define ISI_CIEPROF_F8          0x00800000 // similar to D50, Sylvania F40DSGN50 flourescent    5000K
	#define ISI_CIEPROF_F9          0x01000000 // cool white deluxe flourescent                     4150K
	#define ISI_CIEPROF_F10         0x02000000 // TL85, Ultralume 50                                5000K
	#define ISI_CIEPROF_F11         0x04000000 // TL84, Ultralume 40, SP41                          4000K
	#define ISI_CIEPROF_F12         0x08000000 // TL83, Ultralume 30                                3000K
	#define ISI_CIEPROF_CLOUDY      0x10000000 // D50 with increased red gain,                      5000K
	#define ISI_CIEPROF_SUNNY       0x20000000 // Sunny outdoor conditions,                         5000K
	#define ISI_CIEPROF_OLDISS      0x80000000 // used by old (prior VB6850) drivers for an image sensor specific
	                                           // (default) profile, do not use for new drivers, the default for
	                                           // new drivers is the profile of the IsiSensorDefaultConfig


	//! ulFlickerFreq
	#define ISI_FLICKER_OFF         0x00000001 // no compensation for flickering environmental illumination
	#define ISI_FLICKER_100         0x00000002 // compensation for 100Hz flicker frequency (at 50Hz mains frequency)
	#define ISI_FLICKER_120         0x00000004 // compensation for 120Hz flicker frequency (at 60Hz mains frequency)

#endif

/*****************************************************************************/
/*!
 * sensor capabilities struct:
 * a struct member may have 0, 1 or several bits set according to the capabilities
 * of the sensor. All struct members must be UINT32 and no padding is allowed. Thus,
 * access to the fields is also possible by means of a field of UINT32 values. Indicees
 * for the field-like access are given below.
 */
/*****************************************************************************/
typedef struct
{
    UINT32 ulBusWidth;
    UINT32 ulMode;
    UINT32 ulFieldInv;
    UINT32 ulFieldSel;
    UINT32 ulYCSeq;
    UINT32 ulConv422;
    UINT32 ulBPat;
    UINT32 ulHPol;
    UINT32 ulVPol;
    UINT32 ulEdge;
    UINT32 ulBls;
    UINT32 ulGamma;
    UINT32 ulCConv;
    UINT32 ulRes;
    UINT32 ulDwnSz;
    UINT32 ulBLC;
    UINT32 ulAGC;
    UINT32 ulAWB;
    UINT32 ulAEC;
    // extention ISI version 2
    UINT32 ulCieProfile;
    // extention ISI version 3
    UINT32 ulFlickerFreq;
    // extension ISI version 4
    UINT32 ulSmiaMode;
    UINT32 ulMipiMode;
    UINT32 ulAecMode;
} tsIsiSensorCaps;


#define ISI_CAP_BUSWIDTH         0
#define ISI_CAP_MODE             1
#define ISI_CAP_FIELDINV         2
#define ISI_CAP_FIELDSEL         3
#define ISI_CAP_YCSEQ            4
#define ISI_CAP_CONV422          5
#define ISI_CAP_BPAT             6
#define ISI_CAP_HPOL             7
#define ISI_CAP_VPOL             8
#define ISI_CAP_EDGE             9
#define ISI_CAP_BLS             10
#define ISI_CAP_GAMMA           11
#define ISI_CAP_CCONF           12
#define ISI_CAP_RES             13
#define ISI_CAP_DWNSZ           14
#define ISI_CAP_BLC             15
#define ISI_CAP_AGC             16
#define ISI_CAP_AWB             17
#define ISI_CAP_AEC             18
#define ISI_CAP_CIEPROFILE      19
#define ISI_CAP_FLICKERFREQ     20
#define ISI_CAP_SMIAMODE        21
#define ISI_CAP_MIPIMODE        22
#define ISI_CAP_AECMODE         23
// size of capabilities array (in number of UINT32 fields)
#define ISI_CAP_COUNT           24

/*****************************************************************************/
/*!
 * Sensor configuration struct:
 * same layout as the capabilities struct, but to configure the sensor all struct
 * members which are supported by the sensor must have only 1 bit set. Members which
 * are not supported by the sensor must not have any bits set.
 */
/*****************************************************************************/
typedef tsIsiSensorCaps tsIsiSensorConfig;


#if 0

	/////////////////////////////////////////////////////////////////////
	// single parameter support
	/////////////////////////////////////////////////////////////////////

	// all possible parameter IDs of all supported sensors.
	// Note that it is essential to start at ID 0 and add new parameters
	// without any gaps in between.

	// **** PLEASE NOTE ****
	// If you add some parameter IDs here, please update the textual description
	// array s_atParmDesc in isi_2str.c too. By doing so, you usually don't have to
	// add futher debug-string or user-interface code for the just added parameter.
	// It should work out of the box then.

	//! exposure
	#define ISI_PARM_EXPOSURE            0   // exposure time
	#define ISI_PARM_EXPTBL_INDEX        1   // index in the AE control table
	//! gain
	#define ISI_PARM_GAIN                2   // overall gain (all components)
	#define ISI_PARM_GAIN_LOCK           3   // lock gain against changes by AEC
	#define ISI_PARM_CGAIN_R             4   // component gain of the red pixels
	#define ISI_PARM_CGAIN_G             5   // component gain of the green pixels
	#define ISI_PARM_CGAIN_B             6   // component gain of the blue pixels
	#define ISI_PARM_CGAINB_GR           7   // component gain of the green pixels sharing a bayer line with the red ones
	#define ISI_PARM_CGAINB_GB           8   // component gain of the green pixels sharing a bayer line with the blue ones
	//! blacklevel
	#define ISI_PARM_BLKL                9   // black-level adjustment (all components)
	#define ISI_PARM_CBLKL_R            10   // component black-level of the red pixels
	#define ISI_PARM_CBLKL_G            11   // component black-level of the green pixels
	#define ISI_PARM_CBLKL_B            12   // component black-level of the blue pixels
	#define ISI_PARM_CBLKLB_GR          13   // component black-level of the green pixels sharing a bayer line with the red ones
	#define ISI_PARM_CBLKLB_GB          14   // component black-level of the green pixels sharing a bayer line with the blue ones
	//! resolution & cropping
	#define ISI_PARM_BASERES_X          15   // base resolution in pixel (X)
	#define ISI_PARM_BASERES_Y          16   // base resolution in pixel (Y)
	#define ISI_PARM_WINDOW_X           17   // window top-left pixel (X)
	#define ISI_PARM_WINDOW_Y           18   // window top-left pixel (Y)
	#define ISI_PARM_WINDOW_W           19   // window width in pixel
	#define ISI_PARM_WINDOW_H           20   // window height in pixel
	//! frame rate / clock
	#define ISI_PARM_FRAMERATE_FPS      21   // frame rate in frames per second, fixed point format, 16 bit fractional part
	#define ISI_PARM_FRAMERATE_PITCH    22   // frame rate fine adjustment
	#define ISI_PARM_CLK_DIVIDER        23   // clock divider setting
	#define ISI_PARM_CLK_INPUT          24   // input clock in Hz.
	#define ISI_PARM_CLK_PIXEL          25   // output (pixel-) clock in Hz. Note that for e.g. YUV422-formats, 2 pixel clock cycles are needed per pixel
	//! number of parameter IDs
	#define ISI_PARM__COUNT             26


	//! bit description of the result of the IsiParmInfo routine
	#define ISI_PARMINFO_GET       0x00000001  // parameter can be retrieved from the sensor
	#define ISI_PARMINFO_SET       0x00000002  // parameter can be set into the sensor
	#define ISI_PARMINFO_VOLATILE  0x00000004  // parameter can change at any time during operation
	#define ISI_PARMINFO_RANGE     0x00000008  // range information available for the parameter in question
	#define ISI_PARMINFO_DISCRETE  0x00000010  // range of possible values is not continous. If ISI_PARMINFO_SET
	                                           // is specified, the image sensor code is expected to choose a
	                                           // suitable supported value if an unsupported is given in the
	                                           // IsiParmSet() routine.
	#define ISI_PARMINFO_CONFIG    0x00000020  // parameter may change after a configuration update.
	#define ISI_PARMINFO_RCONFIG   0x00000040  // range information may change after a configuration update.


	// multi-camera support
	#define ISI_UNKNOWN_SENSOR_ID       (0)

#endif


/////////////////////////////////////////////////////////////////////
// structure / type definitions
/////////////////////////////////////////////////////////////////////


/*****************************************************************************/
/*!
 * Input gamma correction curve for R, G or B of the sensor.
 * Since this gamma curve is sensor specific, it will be deliveres by the sensors
 * specific code. This curve will be programmed into Marvin registers.
 */
/*****************************************************************************/

#define ISI_GAMMA_CURVE_ARR_SIZE (17)

typedef struct
{
    UINT16 usIspGammaY[ISI_GAMMA_CURVE_ARR_SIZE];
    UINT32 ulGammaDX0;      // if three curves are given separately for RGB
    UINT32 ulGammaDX1;      // only the struct for R holds valid DX values
} tsIsiGammaCurve;


/*****************************************************************************/
/*!
 * ISI fixed point constant values
 * They are represented as signed fixed point numbers with 12 bit integer and
 * 20 bit fractional part, thus ranging from -2048.0000000 (0x80000000)
 * to +2047.9999990 (0x7FFFFFFF).
 * In the following some frequently used constant values are defined.
 */
/*****************************************************************************/

#define ISI_FP_M0000_794944     (0xFFF347E9)    // -   0.794944
#define ISI_FP_M0000_500000     (0xFFF80000)    // -   0.500000
#define ISI_FP_M0000_404473     (0xFFF98748)    // -   0.404473
#define ISI_FP_M0000_062227     (0xFFFF011F)    // -   0.062227
#define ISI_FP_M0000_024891     (0xFFFF9A0C)    // -   0.024891

#define ISI_FP_P0000_000000     (0x00000000)    //     0.000000

#define ISI_FP_P0000_500000     (0x00080000)    // +   0.500000
#define ISI_FP_P0001_000000     (0x00100000)    // +   1.000000
#define ISI_FP_P0001_163636     (0x00129E40)    // +   1.163636
#define ISI_FP_P0001_600778     (0x00199CC9)    // +   1.600778
#define ISI_FP_P0001_991249     (0x001FDC27)    // +   1.991249
#define ISI_FP_P0016_000000     (0x01000000)    // +  16.000000
#define ISI_FP_P0128_000000     (0x08000000)    // + 128.000000
#define ISI_FP_P0255_000000     (0x0FF00000)    // + 255.000000
#define ISI_FP_P0256_000000     (0x10000000)    // + 256.000000

/*****************************************************************************/
/*!
 * Matrix coefficients used for CrossTalk and/or color conversion. The 9
 * coefficients are laid out as follows (zero based index):
 *    0 | 1 | 2
 *    3 | 4 | 5
 *    6 | 7 | 8
 * They are represented as signed fixed point numbers with 12 bit integer and
 * 20 bit fractional part, thus ranging from -2048.0000000 (0x80000000)
 * to +2047.9999990 (0x7FFFFFFF).
 */
/*****************************************************************************/
typedef struct
{
    INT32 lCoeff[9];
} tsIsi3x3Matrix;

/*****************************************************************************/
/*!
 * Matrix coefficients used for CrossTalk and/or color conversion. The 9
 * coefficients are laid out as follows (zero based index):
 *    0 | 1 | 2
 *    3 | 4 | 5
 *    6 | 7 | 8
 * They are represented as float numbers
 */
/*****************************************************************************/

typedef struct
{
    FLOAT fCoeff[9];
} tsIsi3x3FloatMatrix;

typedef struct
{
    FLOAT fCoeff[3];
} tsIsi3x1FloatMatrix;

typedef struct
{
    FLOAT fCoeff[4];
} tsIsi4x1FloatMatrix;

typedef struct
{
    FLOAT fCoeff[6];
} tsIsi3x2FloatMatrix;

typedef struct
{
    FLOAT fCoeff[2];
} tsIsi2x1FloatMatrix;

typedef struct
{
    FLOAT fCoeff[4];
} tsIsi2x2FloatMatrix;

typedef struct
{
    FLOAT fCoeff[1];
} tsIsi1x1FloatMatrix;

typedef struct
{
    FLOAT fGaussFactor;
} tsIsiGaussFactor;

typedef struct tsMrvPcaValues_
{
    FLOAT fPcaC1;
    FLOAT fPcaC2;
} tsMrvPcaValues;
/*****************************************************************************/
/*!
 * CrossTalk offset. In addition to the matrix multiplication an offset can be
 * added to the pixel values for R, G and B separately. This offset is applied
 * after the matrix multiplication. The values are arranged as unified, see above.
 */
/*****************************************************************************/
typedef struct
{
    INT32 usCtOffsetRed;
    INT32 usCtOffsetGreen;
    INT32 usCtOffsetBlue;
} tsIsiXTalkOffset;

typedef struct
{
    FLOAT fCtOffsetRed;
    FLOAT fCtOffsetGreen;
    FLOAT fCtOffsetBlue;
} tsIsiXTalkFloatOffset;

/*****************************************************************************/
/*!
 * white balancing gains
 * There are two green gains: One for the green Bayer patterns in the red and
 * one for the blue line. In the case the used MARVIN derivative is not able
 * to apply separate green gains the mean value of both greens will be used
 * for the green gain.
 * The component gains are represented as signed fixed point numbers with 12
 * bit integer and 20 bit fractional part, thus ranging from -2048.0000000
 * (0x80000000) to +2047.9999990 (0x7FFFFFFF).
 * Example: +1.0 is represented by 0x00100000.
 */
/*****************************************************************************/
typedef struct
{
    FLOAT fRed;
    FLOAT fGreenR;
    FLOAT fGreenB;
    FLOAT fBlue;
} tsIsiComponentGain;


/*****************************************************************************/
/*!
 * white balance values, default is 0x80 for all components.
 * The struct can be used to provide linear scaling factors to achive a suitable
 * white balance for certain lightning conditions.
 */
/*****************************************************************************/

typedef struct
{
    FLOAT fRed;
    FLOAT fGreen;
    FLOAT fBlue;
} tsIsiCompGain;

/*****************************************************************************/
/*!
 * cross-talk matrix dependent minimum / maximum red and blue gains
 */
/*****************************************************************************/
typedef struct
{
    UINT16 usRedLowerLimit;
    UINT16 usRedUpperLimit;
    UINT16 usBlueLowerLimit;
    UINT16 usBlueUpperLimit;
    UINT32 ulNextCieHigherTemp;
    UINT32 ulNextCieLowerTemp;
} tsIsiComponentGainLimits;

/*****************************************************************************/
/*!
 * contains pointers to parameter arrays for Rg/Bg color space clipping
 */
/*****************************************************************************/
typedef struct
{
    FLOAT* pafRg1;
    FLOAT* pafMaxDist1;
    UINT16 usArraySize1;
    FLOAT* pafRg2;
    FLOAT* pafMaxDist2;
    UINT16 usArraySize2;
} tsIsiAwbClipParm;

/*****************************************************************************/
/*!
 * contains pointers to parameter arrays for AWB out of range handling
 */
/*****************************************************************************/
typedef struct
{
    FLOAT* pafGlobalFade1;
    FLOAT* pafGlobalGainDistance1;
    UINT16 usArraySize1;
    FLOAT* pafGlobalFade2;
    FLOAT* pafGlobalGainDistance2;
    UINT16 usArraySize2;
} tsIsiAwbGlobalFadeParm;

/*****************************************************************************/
/*!
 * contains pointers to parameter arrays for near white pixel parameter calculations
 */
/*****************************************************************************/
typedef struct
{
    FLOAT* pafFade;
    FLOAT* pafCbMinRegionMax;
    FLOAT* pafCrMinRegionMax;
    FLOAT* pafMaxCSumRegionMax;
    FLOAT* pafCbMinRegionMin;
    FLOAT* pafCrMinRegionMin;
    FLOAT* pafMaxCSumRegionMin;
    UINT16 usArraySize;
} tsIsiAwbFade2Parm;

/*****************************************************************************/
/*!
 * parameters for a straight line in Hesse normal form in Rg/Bg colorspace
 */
/*****************************************************************************/
typedef struct
{
    FLOAT f_N0_Rg; //! Rg component of normal vector
    FLOAT f_N0_Bg; //! Bg component of normal vector
    FLOAT f_d;     //! Distance of normal vector
} tsIsiLine;

/*****************************************************************************/
/*!
 * sensor characteristic struct. Is filled in by sensor specific code after
 * main configuration. Features not supported by the sensor driver code
 * will be initialized with default values (1x linear gamma, standard
 * color conversion, cross talk and component gain settings).
 */
/*****************************************************************************/
typedef struct
{
    // In the case that all 3 gamma curves are identically, just
    // set all 3 pointers to the same address.
    const tsIsiGammaCurve          *ptGammaCurveR;          // input gammaR
    const tsIsiGammaCurve          *ptGammaCurveG;          // input gammaG
    const tsIsiGammaCurve          *ptGammaCurveB;          // input gammaB

    const tsIsi3x3FloatMatrix      *ptColorConvCoeff;       // ColorConversion  matrix coefficients
    const tsIsi3x3FloatMatrix      *ptCrossTalkCoeff;       // CrossTalk matrix coefficients
    const tsIsiXTalkFloatOffset    *ptCrossTalkOffset;      // CrossTalk offsets
    const tsIsi3x1FloatMatrix      *ptSVDMeanValue;
    const tsIsi3x2FloatMatrix      *ptPCAMatrix;
    const tsIsi2x1FloatMatrix      *ptGaussMeanValue;
    const tsIsi2x2FloatMatrix      *ptCovarianceMatrix;
    const tsIsiGaussFactor         *ptGaussFactor;
    const tsIsi2x1FloatMatrix      *ptThreshold;
    const tsIsi1x1FloatMatrix      *ptKFactor;
    const tsIsi1x1FloatMatrix      *ptGExpMiddle;
    const tsIsi1x1FloatMatrix      *ptVarDistrIn;
    const tsIsi1x1FloatMatrix      *ptMeanDistrIn;
    const tsIsi1x1FloatMatrix      *ptVarDistrOut;
    const tsIsi1x1FloatMatrix      *ptMeanDistrOut;
    const tsIsiComponentGain       *ptComponentGain;
    const tsIsiAwbClipParm         *ptAwbClipParm;
    const tsIsiAwbGlobalFadeParm   *ptAwbGlobalFadeParm;
    const tsIsiAwbFade2Parm        *ptAwbFade2Parm;
    const tsIsiLine                *ptCenterLine;
} tsIsiSensorAwbProfile;


/*****************************************************************************/
/*!
 *  Sensor-specific information structure for SMIA. Is filled in by sensor
 *  specific code. Features not supported by the sensor driver code will be set
 *  to NULL.
 */
/*****************************************************************************/
typedef struct
{
    UINT32  ulSofEmbDataLines;      // number of embedded data lines at frame
                                    //  start
    UINT32  ulEmbHStart;            // horizontal start position of captured
                                    //  embedded data
    UINT32  ulEmbHSize;             // number of captured embedded data dwords
                                    //  per line (1 dword = 32 bits)
    UINT32  ulEmbVStart;            // start line of embedded data extraction
                                    //  (the line which includes frame start is
                                    //  line 1)
    UINT32  ulNumLines;             // number of image data lines (dummy, black,
                                    //  dark, visible and manufacturer specific
                                    //  lines) per frame
    UINT32  ulBlacklevel;           // Blacklevel: value of register DATA_PEDESTAL
                                    //
    UINT32  ulDataWidth;            // Base for blacklevel
                                    //

} tsIsiSensorSmiaInfo;


/*****************************************************************************/
/*!
 *  General purpose window.
 *  Normally it is used to describe a WOI (Window Of Interest) inside the
 *  background area (e.g. image data area). The offset values count from 0 of
 *  the background area. The defined point is the upper left corner of the
 *  WOI with the specified width and height.
 */
/*****************************************************************************/
typedef struct
{
    UINT16 usHOffs;
    UINT16 usVOffs;
    UINT16 usHSize;
    UINT16 usVSize;
} tsIsiWindow;


/*****************************************************************************/
/*!
 *  Image data description.
 *  The frame size describes the complete image data area output of the sensor.
 *  This includes dummy, black, dark, visible and manufacturer specific pixels
 *  which could be combined in rows and / or in columns.
 *  The visible window describes the visible pixel area inside the image data
 *  area. In the case the image data area does only contain visible pixels,
 *  the offset values have to be 0 and the horizontal and vertical sizes are
 *  equal to the frame size.
 */
/*****************************************************************************/
typedef struct
{
    UINT16      usFrameHSize;
    UINT16      usFrameVSize;
    tsIsiWindow tVisibleWindow;
} tsIsiImageDataInfo;


//! black level compensation mean values
typedef struct
{
    UINT8 ucMeanA;
    UINT8 ucMeanB;
    UINT8 ucMeanC;
    UINT8 ucMeanD;
} tsIsiBlcMean;

//! autowhitebalance mean values
typedef struct
{
    UINT32 ulWhite;
    UINT8 ucMeanY__G;  // Y/G  value in YCbCr/RGB Mode
    UINT8 ucMeanCb__B; // Cb/B value in YCbCr/RGB Mode
    UINT8 ucMeanCr__R; // Cr/R value in YCbCr/RGB Mode
} tsIsiAwbMean;

//! autowhitebalance mean values
typedef struct
{
    UINT32 ulWhite;
    FLOAT fMeanY;
    FLOAT fMeanCb;
    FLOAT fMeanCr;
} tsIsiAwbFloatMean;

//! autoexposure mean values
typedef struct
{
    UINT8 ucOcc;
    UINT8 ucMean;
    UINT8 ucMax;
    UINT8 ucMin;
} tsIsiAecMean;

//! bad pixel element attribute
typedef enum
{
    eIsiBpHot,  // hot pixel
    eIsiBpDead  // dead pixel
} teIsiBpCorrAttr;

//! table element
typedef struct
{
   UINT16          usBpVerAddr;    // Bad Pixel vertical address
   UINT16          usBpHorAddr;    // Bad Pixel horizontal address
   teIsiBpCorrAttr eBpType;        // Bad pixel type (dead or hot)
} tsIsiBpTableElem;

//! Bad Pixel table
typedef struct
{
    UINT32            ulBpNumber;       // Number of detected bad pixel
    tsIsiBpTableElem *ptBpTableElem;    // Pointer to BP Table
    UINT32            ulBpTableElemNum; // Number of Table elements
} tsIsiBpTable;

////////////////////////////////////////////////////////////////////////
// extra functions
////////////////////////////////////////////////////////////////////////

typedef enum
{
   eBufType_16bitBayer,
   eBufType_8bitBayer,
   eBufType_Jpeg,
   eBufType_Yuv420,
   eBufType_Yuv422
} teBufConfig;

#if 0
//!LCD info
typedef struct
{
   UINT16 usMemXRes;          //number of pixels per LCDIF memory line
   UINT16 usMemYRes;          //number of LCDIF memory lines
   UINT16 usLcdXRes;          //number of visible pixels per LCD line
   UINT16 usLcdYRes;          //number of visible LCD lines
   UINT16 usLcdXOffs;         //offset of first visible pixel in a LCD line relative to LCD left border
   UINT16 usLcdYOffs;         //offset of first visible LCD line relative to LCD top border
   UINT8  ucBitsPerPixel;     //number of LCDIF memory bits used per pixel
   tsColorInfo *ptsColorInfo; //pointer to the color structure of the used color mode
   UINT8* pucFrameBuffer;     //pointer to the LCD's frame buffer (if there is such a thing like
                              //the virtual buffer of LCDIF in indirect mode)
} tsLcdInfo;
#endif

#define MEM_SNAPSHOT_MAX_SIZE           (0x01800000)  

//extern void MiscSetupExtMemory(int iWidth, int iHeight, teBufConfig eBufType, tsMrvMiPathConf* ptMrvMiPathConf, unsigned addr );
	
#if 0
	/////////////////////////////////////////////////////////////////////
	//constant (default) tables
	/////////////////////////////////////////////////////////////////////

	//! linear input gamma curves
	extern const tsIsiGammaCurve tIsiGamma1x;
	extern const tsIsiGammaCurve tIsiGamma2x;
	extern const tsIsiGammaCurve tIsiGamma1x06;


	//! standard color conversion matrix from the book "video demystified"
	extern const tsIsi3x3FloatMatrix  tIsiDefColConvCoeff;

	//! standard cross talk conversion matrix (no modification of RGB values)
	extern const tsIsi3x3FloatMatrix  tIsiDefXTalkCoeff;

	//! standard cross talk offset
	extern const tsIsiXTalkFloatOffset  tIsiDefXTalkOffset;

	//! standard component gain (1x for red, green and blue)
	extern const tsIsiComponentGain tIsiDefComponentGain;

	//! name of the current camera
	extern char g_acCameraName[];

	//! standard table for bad pixel generator
	extern const tsIsiBpTableElem atIsiDefBpTableElem_01[];
	//! standard table structure for bad pixel generator
	extern const tsIsiBpTable tIsiDefIsiBpTable;

	/////////////////////////////////////////////////////////////////////
	//function prototypes
	/////////////////////////////////////////////////////////////////////

	// Initialization
	void IsiInit( void );

	//! multi-camera support
	UINT32      IsiGetAmountOfSensors( void );
	UINT32      IsiGetNextSensorId( UINT32 ulSensorId );
	RESULT      IsiSelectSensor( UINT32 ulSensorId );
	UINT32      IsiGetSelectedSensorId( void );
	INT32       IsiGetSensorMrvBusId( UINT32 ulSensorId );
	const char* IsiGetSensorName( UINT32 ulSensorId );
	RESULT      IsiCreateSensor( UINT32 ulSensorId );
	RESULT      IsiReleaseSensor( UINT32 ulSensorId );
	RESULT      IsiGetSelectedSensorDefaultConfig( tsIsiSensorConfig *pIsiSensorConfig );


	//! Configuration
	RESULT IsiGetCaps( tsIsiSensorCaps *ptIsiSensorCaps );
	RESULT IsiSetupSensor( const tsIsiSensorConfig *ptIsiSensorConfig, tsIsiSensorAwbProfile *ptIsiSensorAwbProfile );

	//! sensor specific information
	RESULT IsiGetSensorProfile(  UINT32 ulCieProfile, tsIsiSensorAwbProfile *ptIsiSensorAwbProfile );
	RESULT IsiGetCalibData(  UINT32 ulCieProfile, tsIsiSensorAwbProfile *ptIsiSensorAwbProfile );
	RESULT IsiGetSensorSmiaInfo( tsIsiSensorSmiaInfo *ptIsiSensorSmiaInfo );
	RESULT IsiGetImageDataInfo(  tsIsiImageDataInfo *ptIsiImageDataInfo );
	RESULT IsiGetBadPixelTable(  tsIsiBpTable *ptIsiBpTable );
	RESULT IsiGetExposureLimits( OUT FLOAT *prMinExposure, OUT FLOAT *prMaxExposure );

	//! processing loop routines
	RESULT IsiBlacklevel( const tsIsiBlcMean *ptIsiBlcMean, UINT32 ulBPat );
	RESULT IsiWhiteBalancing( const tsIsiAwbMean *ptIsiAwbMean );
	RESULT IsiExposureControl( const FLOAT rNewExposure, UINT8 *pucNumberOfFramesToSkip );

	//! single parameter support
	RESULT IsiParmSet(int iParamIndex, INT32 lValue);
	RESULT IsiParmGet(int iParamIndex, INT32* plValue);
	RESULT IsiParmMinMax(int iParamIndex, INT32* plMin, INT32* plMax);
	UINT32 IsiParmInfo(int iParamIndex);

	//! some useful helper routines
	extern RESULT IsiRes2Size(UINT32 ulRes, UINT16 *pusHSize, UINT16 *pusVSize );


	//! isi_2str.c
	#if ISI_SUPPORT_CAPS2STRING
	    const char* Isi2Str_CapCategoryName( UINT32 ulCapIndex );
	    const char* Isi2Str_CapCategoryDesc( UINT32 ulCapIndex );
	    const char* Isi2Str_CapItemName( UINT32 ulCapIndex, UINT32 ulValue );
	    const char* Isi2Str_CapItemDesc( UINT32 ulCapIndex, UINT32 ulValue );

	    const char* Isi2Str_ParmName( UINT32 ulParmIndex );
	    const char* Isi2Str_ParmDesc( UINT32 ulParmIndex );
	#endif  // #if ISI_SUPPORT_CAPS2STRING

	//isi_regdump.c
	//! note that the camera may not support reading/writing registers at all
	RESULT IsiReg_Read( UINT32 ulRegAddr, UINT32* pulData );
	RESULT IsiReg_Write( UINT32 ulRegAddr, UINT32 ulData );
	#if ISI_SUPPORT_REGDUMP
	    const char* IsiReg_GetName(UINT32 ulRegAddr);
	    void IsiReg_DumpAll( FILE* hFile );
	#endif  // #if ISI_SUPPORT_REGDUMP

#endif 

#endif //_ISI_H
