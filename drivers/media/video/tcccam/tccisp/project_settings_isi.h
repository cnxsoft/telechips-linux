/*****************************************************************************/
/*!
 *  \file        project_settings_isi.h \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       file to hold the project
 *               specific defines for the image sensor interface (ISI)
 *               and the different image sensor drivers that can be
 *               selected by using the ISI\n
 *
 *  \revision    $Revision: 450 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-07-03 14:35:42 +0200 (Fr, 03 Jul 2009) $ \n
 *               $Id: project_settings_isi.h 450 2009-07-03 12:35:42Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _PROJECT_SETTTINGS_ISI_H
#define _PROJECT_SETTTINGS_ISI_H


/*********************************************************************
 * ISI_SUPPORT_xxx
 * Some compile-time-configurable features of the image sensor interface.
 * Set the certain defines to a non-zero value to enable the feature
 *********************************************************************/

/*!
 * routines to convert capabilities and configuration settings into
 * human readable text. (useful in e.g. debug outputs)
 */
#define ISI_SUPPORT_CAPS2STRING   1

/*!
 * routines to read, write and dump the register set of the connected
 * image sensor
 */
#define ISI_SUPPORT_REGDUMP   1

/*!
 * if set, the register tables of the sensors will contain the register names
 * as strings. This makes e.g. register Dumps much more readable, but also consumes
 * a noticable amount of ROM.
 */
#define ISI_SUPPORT_REGISTERNAMES   1

/*!
* if set, the xtalk compensation works with unified values
*/
#define UNIFIED_XTALK_COLOR   1


/*****************************************************************************/
/*!
 * \addtogroup MARVIN_DEFINES_01 Adding of new camera drivers
 * \b File: project_settings_isi.h
 *
 *
 * \par INFO (What to do if a new camera driver has to be added)
 *
 * \par This file:
 * Define a new camera ID (used in the software to distinguish
 * between the possible cameras) and do not forget to adapt the
 * AMOUNT_OF_CAM_DRIVERS define to the new maximum.
 *
 *
 * \code
 * #define CAMERA_MT9M111           2
 *
 *
 * // further common settings for MT9M111 cameras in MT9M111.h
 * #define MT9M111_AMOUNT_OF_USED_CAMS     (1)                 // one MT9M111 camera
 *
 * // first MT9M111 camera
 * #define MT9M111_1_CLKIN                 (9000000)           // 9.0 MHz
 * #define MT9M111_1_CONNECT               (CAM_CONNECT_PIF)   // connected via PIF
 * #define MT9M111_1_I2C_BUS_ID            (0)                 // I2C bus no. 0
 * #define MT9M111_1_I2C_BASEADR           (0x48)              // I2C base address
 * #define MT9M111_1_MRV_DATA_BUS_ID       (0)                 // MARVIN data bus no. 0
 * \endcode
 *
 * \par Other files:
 *   add header file:
 *   - isi_sensor.h
 *
 *   - specify the basic camera configuration,
 *   - specify the I2C startbit,
 *   - specify the correct amount of address and data bits used for I2C:
 *   - camera driver
 *     (in the example the files MT9M111.c and MT9M111.h are used)
 *
 *   add correct clock and connection settings:
 *   - isisup.c
 *
 */
/*****************************************************************************/


/*****************************************************************************/
/*!
 * \name Camera HW
 * - select which camera hardware is used.
 */
/*****************************************************************************/
//@{
#define CAMERA_OV9640           1
#define CAMERA_MT9M111          2
#define CAMERA_OV3620           3
#define CAMERA_TC7030           4
#define CAMERA_OV9650           5
#define CAMERA_ET8EA3_AS        6
#define CAMERA_MT9D111          7
#define CAMERA_CEM              8
#define CAMERA_OV2630           9
#define CAMERA_MT9T012          10
#define CAMERA_VB6850           11
#define CAMERA_S5K3C1FX         12
#define CAMERA_OV5630           13
#define CAMERA_IMX034PQH5       14
#define CAMERA_S5K4E1G          15

#define AMOUNT_OF_CAM_DRIVERS   (15)

//-> was formerly defined in preprocessor target settings, but this was dropped
//   to keep the number of build targets low. However, with the \#ifndef
//   statement, it is still be possible to overrule it from there (for e.g.
//   special *.mcp files designed for a specific customer)
//
// Note: In the case there is no CAMERA_HW define at all, the framefun
//       application will initially use the first sensor driver which could be
//       found. Then the user could select the desired sensor via
//       "Select Image Sensor" in the main menu.
#ifndef CAMERA_HW
    // select default camera here
    #define CAMERA_HW  CAMERA_OV5630
#endif
//@}


/*****************************************************************************/
/*!
 * \name Camera connection defines
 */
/*****************************************************************************/
//@{
#define CAM_CONNECT_PIF     (1)
#define CAM_CONNECT_DEBUG   (2)
//@}


/*****************************************************************************/
/*!
 * \name I2C start bit defines
 */
/*****************************************************************************/
//@{
#define I2C_COMPLIANT_STARTBIT  1
#define I2C_MICRON_STARTBIT     2
//@}


/*****************************************************************************/
/*!
 * \name Project specific camera definitions
 */
/*****************************************************************************/
//@{
#define AMOUNT_OF_I2C_BUSES         (1)                 //!< one I2C bus


#define USE_CAM_DRV_EN              (1)
#define USE_CAM_DRV_DIS             (0)

#define CEM_DRIVER_USAGE            USE_CAM_DRV_DIS
#define ET8EA3_AS_DRIVER_USAGE      USE_CAM_DRV_DIS
#define MT9D111_DRIVER_USAGE        USE_CAM_DRV_DIS     //!< this driver is completely untested at the moment, so it is deactivated
#define MT9M111_DRIVER_USAGE        USE_CAM_DRV_DIS
#define MT9T012_DRIVER_USAGE        USE_CAM_DRV_DIS     //!< AEC, AWB, BLC not yet tested
#define OV3620_DRIVER_USAGE         USE_CAM_DRV_DIS
#define OV2630_DRIVER_USAGE         USE_CAM_DRV_DIS
#define OV9640_DRIVER_USAGE         USE_CAM_DRV_DIS
#define OV9650_DRIVER_USAGE         USE_CAM_DRV_DIS
#define TC7030_DRIVER_USAGE         USE_CAM_DRV_DIS
#define VB6850_DRIVER_USAGE         USE_CAM_DRV_DIS
#define S5K3C1FX_DRIVER_USAGE       USE_CAM_DRV_DIS    //!< this driver is completely untested at the moment, so it is deactivated
#define OV5630_DRIVER_USAGE         USE_CAM_DRV_EN
#define IMX034PQH5_DRIVER_USAGE     USE_CAM_DRV_DIS
#define S5K4E1G_DRIVER_USAGE        USE_CAM_DRV_DIS
//@}

// SMIA settings
#if ( (VB6850_DRIVER_USAGE  == USE_CAM_DRV_EN) \
 || ( IMX034PQH5_DRIVER_USAGE == USE_CAM_DRV_EN ) \
|| (S5K4E1G_DRIVER_USAGE  == USE_CAM_DRV_EN) )
#define SMIA_COMMON_DRIVER_USAGE USE_CAM_DRV_EN  // the common SMIA functionalities are needed
#else
#define SMIA_COMMON_DRIVER_USAGE USE_CAM_DRV_DIS  // the common SMIA functionalities are not needed
#endif

/*****************************************************************************/
/*!
 * \name Further common settings for SMIA cameras in SMIA.h
 */
/*****************************************************************************/
//@{
#define SMIA_I2C_BASEADR                (0x10)              //!< I2C base address for any SMIA sensor
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for CEM cameras in CEM.h
 */
/*****************************************************************************/
//@{
#define CEM_AMOUNT_OF_USED_CAMS         (1)                 //!< one CEM camera

// first CEM camera
#define CEM_1_CLKIN                     (36000000)          //!< 36.0 MHz
#define CEM_1_CONNECT                   (CAM_CONNECT_DEBUG) //!< connected via DEBUG
#define CEM_1_I2C_BUS_ID                (0)                 //!< I2C bus no. 0
#define CEM_1_I2C_BASEADR               (0x30)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define CEM_1_MRV_DATA_BUS_ID           (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for ET8EA3_AS cameras in ET8EA3_AS.h
 */
/*****************************************************************************/
//@{
#define ET8EA3_AS_AMOUNT_OF_USED_CAMS   (1)                 //!< one ET8EA3_AS camera

// first ET8EA3_AS camera
#define ET8EA3_AS_1_CLKIN               (13500000)          //!< 13.5 MHz
#define ET8EA3_AS_1_CONNECT             (CAM_CONNECT_PIF)   //!< connected via PIF
#define ET8EA3_AS_1_I2C_BUS_ID          (0)                 //!< I2C bus no. 0
#define ET8EA3_AS_1_I2C_BASEADR         (0x3E)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define ET8EA3_AS_1_MRV_DATA_BUS_ID     (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for MT9D111 cameras in MT9D111.h
 */
/*****************************************************************************/
//@{
#define MT9D111_AMOUNT_OF_USED_CAMS     (1)                 //!< one MT9D111 camera

//! first MT9D111 camera
#define MT9D111_1_CLKIN                 (18000000)          //!< 18.0 MHz
#define MT9D111_1_CONNECT               (CAM_CONNECT_PIF)   //!< connected via PIF
#define MT9D111_1_I2C_BUS_ID            (0)                 //!< I2C bus no. 0
#define MT9D111_1_I2C_BASEADR           (0x48)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define MT9D111_1_MRV_DATA_BUS_ID       (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for MT9M111 cameras in MT9M111.h
 */
/*****************************************************************************/
//@{
#define MT9M111_AMOUNT_OF_USED_CAMS     (1)                 //!< one MT9M111 camera

//! first MT9M111 camera
#define MT9M111_1_CLKIN                 (9000000)           //!< 9.0 MHz
#define MT9M111_1_CONNECT               (CAM_CONNECT_PIF)   //!< connected via PIF
#define MT9M111_1_I2C_BUS_ID            (0)                 //!< I2C bus no. 0
#define MT9M111_1_I2C_BASEADR           (0x48)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define MT9M111_1_MRV_DATA_BUS_ID       (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for MT9T012 cameras in MT9T012.h
 */
/*****************************************************************************/
//@{
#define MT9T012_AMOUNT_OF_USED_CAMS     (1)                 //!< one MT9T012 camera

//! first MT9T012 camera
#define MT9T012_1_CLKIN                 (13500000)          //!< 13.5 MHz
#define MT9T012_1_CONNECT               (CAM_CONNECT_PIF)   //!< connected via PIF
#define MT9T012_1_I2C_BUS_ID            (0)                 //!< I2C bus no. 0
#define MT9T012_1_I2C_BASEADR           (0x10)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define MT9T012_1_MRV_DATA_BUS_ID       (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name FRAME_RATE
 * - select which mode is used. (this is usually set via the
 *   build targets from the code warrior project
 */
/*****************************************************************************/
//@{

#define OV2630_SVGA_8FPS5_JPEG_SCIMOB1     0 //with JPEG capturing 13500000Mhz
                                             //OV2630_SVGA_8FPS5_JPEG_SCIMOB1.cfg
#define OV2630_SVGA_13FPS5_DEMO_SCIMOB1    1 //No JPEG capturing
                                             //OV2630_SVGA_13FPS5_DEMO_SCIMOB1.cfg
#define OV2630_SVGA_15FPS_DEBUG_SCIMOB1    2 //No JPEG capturing, no exp. table, use only cam ISP
                                             //OV2630_SVGA_15FPS_DEBUG_SCIMOB1.cfg

#define OV2630_UXGA_2FPS8_JPEG_SCIMOB1     3 //with JPEG capturing 13500000Mhz
                                             //OV2630_UXGA_2FPS8_JPEG_SCIMOB1.cfg
#define OV2630_UXGA_4FPS8_DEMO_SCIMOB1     4 //No JPEG capturing
                                             //OV2630_UXGA_4FPS8_DEMO_SCIMOB1.cfg
#define OV2630_UXGA_7FPS5_DEBUG_SCIMOB1    5 //No JPEG capturing, no exp. table, use only cam ISP
                                             //
#define OV2630_UXGA_XX_DEBUG_SCIMOB3       6 //
                                             //
#define OV2630_SVGA_XX_DEBUG_SCIMOB3       7 //
                                             //

#define OV2630_SVGA_18FPS_DEMO_SCIMOB3       8 // with JPEG capturing (PL110 is switched off), view-finding only 320x240
                                               // OV2630_SVGA_18FPS_DEMO_SCIMOB3.cfg
#define OV2630_UXGA_6FPS2_DEBUG_SCIMOB3      9 // with JPEG capturing (PL110 is switched off), view-finding only with direct LCD framebuffer access activated
                                               // OV2630_UXGA_6FPS2_DEBUG_SCIMOB3.cfg
// Hack start: To be able to change the clock frequency
#define OV2630_DEMO_SCIMOB3                 10 // Demo Feb 06
// Hack end

#ifndef FRAME_RATE
    #define FRAME_RATE  OV2630_DEMO_SCIMOB3
#endif
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for OV2630 cameras in omnivision2630.h
 */
/*****************************************************************************/
//@{
#define OV2630_AMOUNT_OF_USED_CAMS      (1)                 //!< one OV2630 camera

// first OV2630 camera
#if (FRAME_RATE == OV2630_SVGA_8FPS5_JPEG_SCIMOB1)
    #define OV2630_1_CLKIN              (13500000)          //! 13.5 MHz for JPEG capturing
#elif (FRAME_RATE == OV2630_SVGA_13FPS5_DEMO_SCIMOB1)
    #define OV2630_1_CLKIN              (21600000)          //! 21.6 Mhz for 13.5 fps + mrv ISP
#elif (FRAME_RATE == OV2630_SVGA_15FPS_DEBUG_SCIMOB1)
    #define OV2630_1_CLKIN              (12000000)          //! 12 Mhz for 24Mhz Cam clk (15fps)
#elif (FRAME_RATE == OV2630_UXGA_2FPS8_JPEG_SCIMOB1)
    #define OV2630_1_CLKIN              (13500000)          //! 13.5 Mhz for 2.8 fps + mrv ISP
#elif (FRAME_RATE == OV2630_UXGA_4FPS8_DEMO_SCIMOB1)
    #define OV2630_1_CLKIN              (23077000)          //! 23.077 Mhz for 4.8 fps + mrv ISP
#elif (FRAME_RATE == OV2630_UXGA_7FPS5_DEBUG_SCIMOB1)
    #define OV2630_1_CLKIN              (12000000)          //!
#elif (FRAME_RATE == OV2630_UXGA_XX_DEBUG_SCIMOB3)
    #define OV2630_1_CLKIN              (10000000)          //!
#elif (FRAME_RATE == OV2630_SVGA_XX_DEBUG_SCIMOB3)
    #define OV2630_1_CLKIN              ( 7200000)          //!
#elif (FRAME_RATE == OV2630_SVGA_18FPS_DEMO_SCIMOB3)
    #define OV2630_1_CLKIN              ( 7200000)          //!<  7.2 MHz for 18 fps of camera (view-finding only 320x240)
#elif (FRAME_RATE == OV2630_UXGA_6FPS2_DEBUG_SCIMOB3)
    #define OV2630_1_CLKIN              (10000000)          //!< 10.0 MHz for 6.25 fps of camera (view-finding only with direct LCD framebuffer access activated)
// Hack start: To be able to change the clock frequency
#elif (FRAME_RATE == OV2630_DEMO_SCIMOB3)
    #define OV2630_1_CLKIN              (10000000)          //!< 10.0 MHz for Demo Feb 06 (not really needed, as the camera driver will change this frequency)
// Hack end
#endif
#define OV2630_1_CONNECT                (CAM_CONNECT_PIF)   //!< connected via PIF
#define OV2630_1_I2C_BUS_ID             (0)                 //!< I2C bus no. 0
#define OV2630_1_I2C_BASEADR            (0x30)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define OV2630_1_MRV_DATA_BUS_ID        (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for OV3620 cameras in omnivision3620.h
 */
/*****************************************************************************/
//@{
#define OV3620_AMOUNT_OF_USED_CAMS      (1)                 //!< one OV3620 camera

// first OV3620 camera
#define OV3620_1_CLKIN              (27000000)          //!< 27.0 MHz
#define OV3620_1_CONNECT                (CAM_CONNECT_PIF)   //!< connected via PIF
#define OV3620_1_I2C_BUS_ID             (0)                 //!< I2C bus no. 0
#define OV3620_1_I2C_BASEADR            (0x30)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define OV3620_1_MRV_DATA_BUS_ID        (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for OV9640 cameras in omnivision9640.h
 */
/*****************************************************************************/
//@{
#define OV9640_AMOUNT_OF_USED_CAMS      (1)                 //!< one OV9640 camera

// first OV9640 camera
#define OV9640_1_CLKIN              (36000000)          //!< 36.0 MHz
#define OV9640_1_CONNECT                (CAM_CONNECT_PIF)   //!< connected via PIF
#define OV9640_1_I2C_BUS_ID             (0)                 //!< I2C bus no. 0
#define OV9640_1_I2C_BASEADR            (0x30)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define OV9640_1_MRV_DATA_BUS_ID        (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for OV9650 cameras in omnivision9650.h
 */
/*****************************************************************************/
//@{
#define OV9650_AMOUNT_OF_USED_CAMS      (1)                 //!< one OV9650 camera

// first OV9650 camera
#define OV9650_1_CLKIN                  (36000000)          //!< 36.0 MHz
#define OV9650_1_CONNECT                (CAM_CONNECT_PIF)   //!< connected via PIF
#define OV9650_1_I2C_BUS_ID             (0)                 //!< I2C bus no. 0
#define OV9650_1_I2C_BASEADR            (0x30)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define OV9650_1_MRV_DATA_BUS_ID        (0)                 //!< MARVIN data bus no. 0
//@}S5K3C1FX


/*****************************************************************************/
/*!
 * \name Further common settings for TC7030 cameras in transchip7030.h
 */
/*****************************************************************************/
//@{
#define TC7030_AMOUNT_OF_USED_CAMS      (1)                 //!< one TC7030 camera

// first TC7030 camera
#define TC7030_1_CLKIN                  (20000000)          //!< 20.0 MHz
#define TC7030_1_CONNECT                (CAM_CONNECT_PIF)   //!< connected via PIF
#define TC7030_1_I2C_BUS_ID             (0)                 //!< I2C bus no. 0
#define TC7030_1_I2C_BASEADR            (0x56)              //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define TC7030_1_MRV_DATA_BUS_ID        (0)                 //!< MARVIN data bus no. 0
//@}


/*****************************************************************************/
/*!
 * \name Further common settings for VB6850 cameras in VB6850.h
 */
/*****************************************************************************/
//@{
#define VB6850_AMOUNT_OF_USED_CAMS      (1)                 //!< one VB6850 camera

// first VB6850 camera
#define VB6850_1_CLKIN                  (10000000)          //!< 10.0 MHz
#define VB6850_1_CONNECT                (CAM_CONNECT_PIF)   //!< connected via PIF
#define VB6850_1_I2C_BUS_ID             (0)                 //!< I2C bus no. 0
#define VB6850_1_I2C_BASEADR            SMIA_I2C_BASEADR    //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define VB6850_1_MRV_DATA_BUS_ID        (1)                 //!< MARVIN data bus no. 1
//@}

/*****************************************************************************/
/*!
 * \name Further common settings for S5K3C1FX cameras in S5K3C1FX.h
 */
/*****************************************************************************/
//@{
#define S5K3C1FX_AMOUNT_OF_USED_CAMS      (1)                 //!< one S5K3C1FX camera

// first S5K3C1FX camera
#define S5K3C1FX_1_CLKIN                  (13500000)        //!< 13.5 MHz
#define S5K3C1FX_1_CONNECT                (CAM_CONNECT_PIF) //!< connected via PIF
#define S5K3C1FX_1_I2C_BUS_ID             (0)               //!< I2C bus no. 0
#define S5K3C1FX_1_I2C_BASEADR            (0x11)            //!< I2C base address
                                                            //  Note: The address will be shifted by the
                                                            //          I2C driver and the read/write
                                                            //          bit 0 will be applied after the
                                                            //          shift operation.
#define S5K3C1FX_1_MRV_DATA_BUS_ID        (0)               //!< MARVIN data bus no. 0
//@}

/*****************************************************************************/
/*!
 * \name Further common settings for OV5630 cameras in OV5630.h
 */
/*****************************************************************************/
//@{
#define OV5630_AMOUNT_OF_USED_CAMS     (1)                 //!< one OV5630 camera

//! first OV5630 camera
#define OV5630_1_CLKIN                 (10000000)           //!< min. 6MHz, max. 27MHz, typ. 24MHz, too low does not work,
                                                           //   e.g. with 3.375MHz I2C does not work anymore
                                                           //   with an extended adapter it works up to 9 MHz
#define OV5630_1_CONNECT               (CAM_CONNECT_PIF)   //!< connected via PIF
#define OV5630_1_I2C_BUS_ID            (0)                 //!< I2C bus no. 0
#define OV5630_1_I2C_BASEADR           (0x36)              //!< I2C base address
                                                           //  Note: The address will be shifted left once,
                                                           //        by the I2C driver and the read/write
                                                           //        bit 0 will be applied after the shift operation.
                                                           //        thus, effective addresses are:
                                                           //        Read: 0x6C, Write: 0x6D
#define OV5630_1_MRV_DATA_BUS_ID       (0)                 //!< MARVIN data bus no. 0
//@}

/*****************************************************************************/
/*!
 * \name Further common settings for IMX034PQH5 cameras in IMX034PQH5.h
 */
/*****************************************************************************/
//@{
#define IMX034PQH5_AMOUNT_OF_USED_CAMS (1)                 //!< one IMX034PQH5 camera

//! first IMX034PQH5 camera
#define IMX034PQH5_1_CLKIN              (12000000)          //!< min. 6MHz, max. 27MHz, typ. 24MHz, too low does not work, e.g. with 3.375MHz I2C does not work anymore
#define IMX034PQH5_1_CONNECT            (CAM_CONNECT_PIF)   //!< connected via PIF
#define IMX034PQH5_1_I2C_BUS_ID         (0)                 //!< I2C bus no. 0
#define IMX034PQH5_1_I2C_BASEADR        (0x1A)              //!< I2C base address (SMIA Standard: 0x10 = SMIA_I2C_BASEADR)
                                                            //  Note: The address will be shifted left once,
                                                            //        by the I2C driver and the read/write
                                                            //        bit 0 will be applied after the shift operation.
                                                            //        thus, effective addresses are:
                                                            //        Read: 0x35, Write: 0x34
#define IMX034PQH5_1_MRV_DATA_BUS_ID    (0)                 //!< MARVIN data bus no. 0
//@}
/*****************************************************************************/
/*!
 * \name Further common settings for S5K4E1G cameras in S5K4E1G.h
 */
/*****************************************************************************/
//@{
#define S5K4E1G_AMOUNT_OF_USED_CAMS     (1)                 //!< one S5K4E1G camera

//! first S5K4E1G camera
#define S5K4E1G_1_CLKIN                 (10000000)          //!< min. 6MHz, max. 27MHz, typ. 24MHz, too low does not work, e.g. with 3.375MHz I2C does not work anymore
#define S5K4E1G_1_CONNECT               (CAM_CONNECT_PIF)   //!< connected via PIF
#define S5K4E1G_1_I2C_BUS_ID            (0)                 //!< I2C bus no. 0
#define S5K4E1G_1_I2C_BASEADR           (0x36)              //!< I2C base address
                                                            //  Note: The address will be shifted left once,
                                                            //        by the I2C driver and the read/write
                                                            //        bit 0 will be applied after the shift operation.
                                                            //        thus, effective addresses are:
                                                            //        Read: 0x6C, Write: 0x6D
#define S5K4E1G_1_MRV_DATA_BUS_ID       (0)                 //!< MARVIN data bus no. 0
//@}


#endif//_PROJECT_SETTTINGS_ISI_H
