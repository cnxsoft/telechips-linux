/*****************************************************************************/
/*!
 *  \file        mrv_mipi.h \n
 *  \version     1.0 \n
 *  \author      Zupp \n
 *  \brief       Header file for Marvin MIPI low level driver functionality. \n
 *               This file contains all module relevant function prototypes
 *               and definitions. \n
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_mipi.h 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#ifndef _MRV_MIPI_H
#define _MRV_MIPI_H


/******************************************************************************
 * Defines
 ******************************************************************************/

#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))

// MIPI control register (mipi_ctrl) access defines
#define MIPI_CTRL_ERR_SOT_SYNC_HS_SKIP          0x00020000  // Skip pic data in case of interrupt or not
#define MIPI_CTRL_ERR_SOT_HS_SKIP               0x00010000  // Skip pic data in case of interrupt or not
#define MIPI_CTRL_USED_4_LANES                  0x00003000  // Define the number of used MIPI lanes
#define MIPI_CTRL_USED_3_LANES                  0x00002000  // Define the number of used MIPI lanes
#define MIPI_CTRL_USED_2_LANES                  0x00001000  // Define the number of used MIPI lanes
#define MIPI_CTRL_USED_1_LANE                   0x00000000  // Define the number of used MIPI lanes
#define MIPI_CTRL_SHUTDOWN_VALUE_LANE_4         0x00000800  // Set the lane 4 shutdown harware signal
#define MIPI_CTRL_SHUTDOWN_VALUE_LANE_3         0x00000400  // Set the lane 3 shutdown harware signal
#define MIPI_CTRL_SHUTDOWN_VALUE_LANE_2         0x00000200  // Set the lane 2 shutdown harware signal
#define MIPI_CTRL_SHUTDOWN_VALUE_LANE_1         0x00000100  // Set the lane 1 shutdown harware signal
#define MIPI_CTRL_FLUSH_FIFO                    0x00000002  // Delete additional data fifo memory
#define MIPI_CTRL_OUTPUT_ENABLE                 0x00000001  // Activate/deactivate the MIPI module

// MIPI lane stopstate request definitions
#define MIPI_STATUS_STOPSTATE_LANE_4            0x00000080  // Is lane 4 in stopstate (active/inactive)
#define MIPI_STATUS_STOPSTATE_LANE_3            0x00000040  // Is lane 3 in stopstate (active/inactive)
#define MIPI_STATUS_STOPSTATE_LANE_2            0x00000020  // Is lane 2 in stopstate (active/inactive)
#define MIPI_STATUS_STOPSTATE_LANE_1            0x00000010  // Is lane 1 in stopstate (active/inactive)
#define MIPI_STATUS_ADD_DATA_AVAIL              0x00000001  // Is Additional data available (stored in fifo memory)

// MIPI interrup mask defines
#define MIPI_INTRPT_ADDB_DATA_OVFLW             0x02000000  // mask for MIPI embedded data overflow interrupt
#define MIPI_INTRPT_FRAME_END                   0x01000000  // mask for MIPI frame end interrupt
#define MIPI_INTRPT_ERR_CS                      0x00800000  // to be documented
#define MIPI_INTRPT_ERR_ECC1                    0x00400000  // to be documented
#define MIPI_INTRPT_ERR_ECC2                    0x00200000  // to be documented
#define MIPI_INTRPT_ERR_PROTOCOL                0x00100000  // to be documented
#define MIPI_INTRPT_ERR_CONTROL_ALL_LANES       0x000F0000  // to be documented
#define MIPI_INTRPT_ERR_CONTROL_LANE_4          0x00080000  // to be documented
#define MIPI_INTRPT_ERR_CONTROL_LANE_3          0x00040000  // to be documented
#define MIPI_INTRPT_ERR_CONTROL_LANE_2          0x00020000  // to be documented
#define MIPI_INTRPT_ERR_CONTROL_LANE_1          0x00010000  // to be documented
#define MIPI_INTRPT_ERR_EOT_SYNC_ALL_LANE       0x0000F000  // to be documented
#define MIPI_INTRPT_ERR_EOT_SYNC_LANE_4         0x00008000  // to be documented
#define MIPI_INTRPT_ERR_EOT_SYNC_LANE_3         0x00004000  // to be documented
#define MIPI_INTRPT_ERR_EOT_SYNC_LANE_2         0x00002000  // to be documented
#define MIPI_INTRPT_ERR_EOT_SYNC_LANE_1         0x00001000  // to be documented
#define MIPI_INTRPT_ERR_SOT_SYNC_ALL_LANES      0x00000F00  // to be documented
#define MIPI_INTRPT_ERR_SOT_SYNC_LANE_4         0x00000800  // to be documented
#define MIPI_INTRPT_ERR_SOT_SYNC_LANE_3         0x00000400  // to be documented
#define MIPI_INTRPT_ERR_SOT_SYNC_LANE_2         0x00000200  // to be documented
#define MIPI_INTRPT_ERR_SOT_SYNC_LANE_1         0x00000100  // to be documented
#define MIPI_INTRPT_ERR_SOT_ALL_LANES           0x000000F0  // to be documented
#define MIPI_INTRPT_ERR_SOT_LANE_4              0x00000080  // to be documented
#define MIPI_INTRPT_ERR_SOT_LANE_3              0x00000040  // to be documented
#define MIPI_INTRPT_ERR_SOT_LANE_2              0x00000020  // to be documented
#define MIPI_INTRPT_ERR_SOT_LANE_1              0x00000010  // to be documented
#define MIPI_INTRPT_SYNC_FIFO_OVFLW_ALL_LANES   0x0000000F  // to be documented
#define MIPI_INTRPT_SYNC_FIFO_OVFLW_LANE_4      0x00000008  // to be documented
#define MIPI_INTRPT_SYNC_FIFO_OVFLW_LANE_3      0x00000004  // to be documented
#define MIPI_INTRPT_SYNC_FIFO_OVFLW_LANE_2      0x00000002  // to be documented
#define MIPI_INTRPT_SYNC_FIFO_OVFLW_LANE_1      0x00000001  // to be documented
#define MIPI_INTRPT_ERR_ALL                     0x02FFFFFF  // mask for all error interrupts
#define MIPI_INTRPT_ALL                         0x03FFFFFF  // mask for all MIPI interrupts

// MIPI virtual channel definitions
#define MIPI_VIRTUAL_CHANNEL_3                  0x000000C0  // mask for MIPI virtual channel 3 access
#define MIPI_VIRTUAL_CHANNEL_2                  0x00000080  // mask for MIPI virtual channel 2 access
#define MIPI_VIRTUAL_CHANNEL_1                  0x00000040  // mask for MIPI virtual channel 1 access
#define MIPI_VIRTUAL_CHANNEL_0                  0x00000000  // mask for MIPI virtual channel 0 access
#define MIPI_VIRTUAL_CHANNEL_AREA               0x000000C0  // mask for All MIPI virtual channel bits

// MIPI package data type definitions
#define MIPI_DATA_TYPE_AREA                     0x0000003F  // mask for All MIPI data type bits

// MIPI Data Types
#define MIPI_PIC_FORMAT_YUV420_8                0x00000018  // YUV 420  8-bit
#define MIPI_PIC_FORMAT_YUV420_10               0x00000019  // YUV 420 10-bit
#define MIPI_PIC_FORMAT_LEGACY_YUV420_8         0x0000001A  // Legacy YUV 420 8-bit
#define MIPI_PIC_FORMAT_YUV420_CSPS_8           0x0000001C  // YUV 420 8-bit (CSPS)
#define MIPI_PIC_FORMAT_YUV420_CSPS_10          0x0000001D  // YUV 420 10-bit (CSPS)
#define MIPI_PIC_FORMAT_YUV422_8                0x0000001E  // YUV 422 8-bit
#define MIPI_PIC_FORMAT_YUV422_10               0x0000001F  // YUV 422 10-bit
#define MIPI_PIC_FORMAT_RGB444                  0x00000020  // RGB 444
#define MIPI_PIC_FORMAT_RGB555                  0x00000021  // RGB 555
#define MIPI_PIC_FORMAT_RGB565                  0x00000022  // RGB 565
#define MIPI_PIC_FORMAT_RGB666                  0x00000023  // RGB 666
#define MIPI_PIC_FORMAT_RGB888                  0x00000024  // RGB 888
#define MIPI_PIC_FORMAT_RAW_6                   0x00000028  // RAW_6
#define MIPI_PIC_FORMAT_RAW_7                   0x00000029  // RAW_7
#define MIPI_PIC_FORMAT_RAW_8                   0x0000002A  // RAW_8
#define MIPI_PIC_FORMAT_RAW_10                  0x0000002B  // RAW_10
#define MIPI_PIC_FORMAT_RAW_12                  0x0000002C  // RAW_12
#define MIPI_PIC_FORMAT_USER_0                  0x00000030  // User define 0
#define MIPI_PIC_FORMAT_USER_1                  0x00000031  // User define 1
#define MIPI_PIC_FORMAT_USER_2                  0x00000032  // User define 2
#define MIPI_PIC_FORMAT_USER_3                  0x00000033  // User define 3
#define MIPI_PIC_FORMAT_USER_4                  0x00000034  // User define 4
#define MIPI_PIC_FORMAT_USER_5                  0x00000035  // User define 5
#define MIPI_PIC_FORMAT_USER_6                  0x00000036  // User define 6
#define MIPI_PIC_FORMAT_USER_7                  0x00000037  // User define 7

#define MIPI_COM_12_8_12        0 // 0x00    12-8-12
#define MIPI_COM_12_7_12        1 // 0x01    12-8-12
#define MIPI_COM_12_6_12        2 // 0x02    12-8-12
#define MIPI_COM_10_8_10        3 // 0x03    10-8-10
#define MIPI_COM_10_7_10        4 // 0x04    10-7-10
#define MIPI_COM_10_6_10        5 // 0x05    10-6-10
#define MIPI_COM_OFF          255 // 0xFF    off

#define MIPI_COM_PREDICTOR_1          1
#define MIPI_COM_PREDICTOR_2          2


#endif

// MIPI additional data selector block definitions
typedef enum
{
    eMipiAddDataSelector1 = 0,
    eMipiAddDataSelector2,
    eMipiAddDataSelector3,
    eMipiAddDataSelector4
} teMipiAddDataSelector;

/******************************************************************************
 * Public MIPI Interface
 ******************************************************************************/
RESULT MrvMipiInitializeModule( void );
RESULT MrvMipiGetControlRegisterVal( UINT32 *puMipiControlRegisterVal );
RESULT MrvMipiSetControlRegisterVal( const UINT32 uMipiControlRegisterVal );
RESULT MrvMipiActivateModule( const BOOL bModuleActive );
RESULT MrvMipiFreeEmbDataFifo( void );
RESULT MrvMipiSetShutdownLane( const BOOL bShutdownLaneVal, const UINT8 uMipiLane );
RESULT MrvMipiDefineNumberOfUsedLanes( const UINT8 uMipiLanes );
RESULT MrvSkipDataProcessingInCaseOfSotIrq( const BOOL bSkipDataProcessing );
RESULT MrvSkipDataProcessingInCaseOfSotSyncIrq( const BOOL bSkipDataProcessing );
RESULT MrvMipiGetStatusRegisterVal( UINT32 *puMipiStatusRegisterVal );
RESULT MrvMipiAddDataAvailable( BOOL *pbMipiAddDataAvailable );
RESULT MrvMipiGetLaneStopState( UINT8 uLaneNumber, BOOL *pbStopState );
RESULT MrvMipiActivateInterrupts( const UINT32 uActivatedInterrupts );
RESULT MrvMipiGetImscRegister( UINT32 *puActivatedInterrupt );
RESULT MrvMipiGetGeneralInterruptState( UINT32 *puGeneralInterruptState );
RESULT MrvMipiGetActivatedInterruptState( UINT32 *puActivatedInterruptState );
RESULT MrvMipiResetAllInterrups( void );
RESULT MrvMipiResetInterrups( const UINT32 uInterruptResetMask );
RESULT MrvMipiSetInterruptRegister( const UINT32 uInterruptRegisterMask, const UINT32 uNewInterruptRegisterValue );
RESULT MrvMipiGetCurrentDataId( UINT8 *puVirtualChannel, UINT8 *puDataType );
RESULT MrvMipiGetDataSelector( UINT8 *puVirtualChannel, UINT8 *puDataType );
RESULT MrvMipiSetDataSelector( const UINT8 uVirtualChannel, const UINT8 uDataType );
RESULT MrvMipiGetCompressedMode(UINT8 *puCompMode, UINT8 *puPred);
RESULT MrvMipiSetCompressedMode(const UINT8 uCompMode, const UINT8 uPred);
RESULT MrvMipiGetAdditionalDataSelector( const teMipiAddDataSelector eAddDataSelectorNum, UINT8 *puVirtualChannel, UINT8 *puDataType );
RESULT MrvMipiSetAdditionalDataSelector(  const teMipiAddDataSelector eAddDataSelectorNum, const UINT8 uVirtualChannel, const UINT8 uDataType );
RESULT MrvMipiGetAdditionalData(UINT8 *puDestAddbDataBuffer, const UINT32 ulDestAddbDataBufSize, UINT32 *puNumOfAddDataBytes);
RESULT MrvMipiWaitForIrq( UINT32 ulBitMask );
RESULT MrvMipiSetAddDataFillLevel(const UINT32 ulFillLevel);

#endif //#ifndef _MRV_MIPI_H
