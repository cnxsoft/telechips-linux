#ifndef _VENUS_HOSTCMD_H_
#define _VENUS_HOSTCMD_H_


/*****************************************************************************/
/* Venus DSC Function API (Common)                                          */
/*****************************************************************************/
#define VENUS_CHIP_STANDBY							(0x0B)
#define VENUS_CHIP_STREAMING						(0x0C)
#define VENUS_CHIP_TRISTATE							(0xC0)
#define VENUS_CHIP_POWEROFF							(0xC1)
#define VENUS_BUTTON_EVENT                          (0xC2)
    #define VENUS_BUTTON_HALF_PRESSED               (0x0000)
    #define VENUS_BUTTON_HALF_RELEASED              (0x0001)

#define VENUS_INIT_SENSOR							(0x10)
#define VENUS_WRITE_SENSOR							(0x11)
#define VENUS_READ_SENSOR							(0x12)

#define VENUS_SENSOR_RESOLUTION						(0x32)
#define VENUS_SENSOR_PREVIEW_RESOLUTION				(0x33)
#define VENUS_SENSOR_SNAPSHOT_RESOLUTION			(0x34)
#define VENUS_SENSOR_MIRROR_FLIP					(0x35) // Jokery for SEC's request 20081230
	#define	VENUS_SENSOR_ORIENT_NORMAL						(0x00)
	#define	VENUS_SENSOR_ORIENT_HOR_MIRROR					(0x01)
	#define	VENUS_SENSOR_ORIENT_VER_FLIP					(0x02)
	#define	VENUS_SENSOR_ORIENT_MIRROR_FLIP					(0x03)
#define VENUS_SENSOR_FPS                            (0x36)

#define VENUS_WRITE_MOTOR							(0x15)
#define VENUS_READ_MOTOR							(0x16)

#define VENUS_EXT_MEM_TEST							(0x60)
#define VENUS_COMPARE_MEM							(0x61)

#define VENUS_RAWPROC_STORE							(0x6A)
#define VENUS_RAWPROC_FETCH							(0x6B)

#define VENUS_GET_FW_VERSION						(0xA2)

#define VENUS_CHECK_ISP_STATUS						(0xCD)
	#define	VENUS_CHECK_ISP_STATUS_OK						(0x00)
	#define	VENUS_CHECK_ISP_STATUS_FAIL						(0x01)

#define VENUS_WRITE_TAG								(0xC7)
#define VENUS_READ_TAG								(0xC8)

/*****************************************************************************/
/* Venus DSC Function API (Preview)                                          */
/*****************************************************************************/
#define VENUS_PREVIEW_ZOOM							(0x02)
#define VENUS_PREVIEW_STOP							(0x0E)
#define VENUS_PREVIEW_START							(0x0F)

#define VENUS_SET_PREVIEW_RESOLUTION				(0x84)
#define VENUS_SET_PREVIEW_FORMAT					(0x85)
#define VENUS_SET_PREVIEW_FPS						(0x86)

/*****************************************************************************/
/* Venus DSC Function API (Snapshot)                                         */
/*****************************************************************************/
#define VENUS_CAPTURE								(0x03)

#define VENUS_FLASH_EN								(0x09)

#define VENUS_SET_SNAPSHOT_MODE						(0x70)

#define VENUS_SET_SNAPSHOT_MAIN_RESOLUTION			(0x71)
#define VENUS_SET_THUMBNAIL_RESOLUTION				(0x72)

#define VENUS_SET_SNAPSHOT_MAIN_FORMAT				(0x80)
#define VENUS_SET_SNAPSHOT_THUMB_FORMAT				(0x81)

#define VENUS_SET_SNAPSHOT_DIRECTION				(0x82)

#define VENUS_GET_MAIN_JPEG_COMPRESSED_SIZE			(0x73)
#define VENUS_GET_THUMB_JPEG_COMPRESSED_SIZE		(0x74)

#define VENUS_GET_MAIN_JPEG_START_ADDRESS			(0x75)
#define VENUS_GET_THUMB_JPEG_START_ADDRESS			(0x76)

#define VENUS_GET_LAST_SHOT_ISO_SPEED				(0x87)
#define VENUS_GET_LAST_SHOT_SHUTTER_SPEED			(0x88)

#define VENUS_SET_MAIN_DESTNATION					(0x77)
#define VENUS_SET_THUMBNAIL_DESTNATION				(0x78)

#define VENUS_SET_IMAGE_STABILIZER_COUNT			(0x79)
#define VENUS_SET_MULTISHOT_COUNT					(0x7A)
#define VENUS_SET_JPEG_MAX_SIZE						(0x7B)

#define VENUS_PREPARE_JPEG_DATA_FOR_CCIR			(0x7C)
#define VENUS_SET_DUMP_DATA_LINE_PIXELS_VIA_CCIR	(0x7D)
#define VENUS_DUMP_DATA_VIA_CCIR					(0x7E)
#define VENUS_SET_DUMP_DATA_SPEED_VIA_CCIR			(0x7F)
#define VENUS_DUMP_DATA_BY_MODE						(0x83)

#define VENUS_GET_ISC_STATUS						(0xA3) // nop

#define VENUS_SET_JPEG_QUALITY						(0xA6)
#define VENUS_SET_JPEG_MANUAL_QUALITY				(0xAD)
#define VENUS_SET_ISC								(0xA9)

/*****************************************************************************/
/* Venus SPI Control API                                                     */
/*****************************************************************************/
#define VENUS_SPI_ID_READ							(0x18)
#define VENUS_SPI_DATA_READ							(0x19)
#define VENUS_SPI_DATA_WRITE						(0x1A)
#define VENUS_SPI_SECTOR_ERASE						(0x1B)
#define VENUS_SPI_CHIP_ERASE						(0x1C)

/*****************************************************************************/
/* Venus FD Control API                                                      */
/*****************************************************************************/
#define VENUS_FD_ENABLE								(0x62)
#define VENUS_FD_DISABLE							(0x63)
#define VENUS_FD_UNLOCK								(0x68)

/*****************************************************************************/
/* Venus 3A Control API                                                      */
/*****************************************************************************/
#define VENUS_LOCK_AE_AWB							(0xCA)
	#define LOCK_AE                                     (0x01)
	#define LOCK_AWB                                    (0x02)
#define VENUS_UNLOCK_AE_AWB							(0xCB)
	#define UNLOCK_AE                                   (0x01)
	#define UNLOCK_AWB                                  (0x02)
#define VENUS_GET_BV								(0xCC)

#define VENUS_SET_AF_MODE							(0x17)
	#define AF_MODE_AUTO								(0)
	#define AF_MODE_MANUAL								(1)
	#define AF_MODE_MACRO								(2)
        #define AF_MODE_PanFocus							(4)
#define VENUS_SET_AE_WINDOW							(0x3C)
#define VENUS_GET_AE_WINDOW							(0x3E)
#define VENUS_SET_AE_WINDOW_WEIGHT					(0x40)
#define VENUS_GET_AE_WINDOW_WEIGHT					(0x42)
#define VENUS_SET_AWB_WINDOW						(0x44)
#define VENUS_GET_AWB_WINDOW						(0x46)
#define VENUS_SET_AF_WINDOW							(0x48)
#define VENUS_GET_AF_WINDOW							(0x4A)

#define VENUS_AF_SET_SAMPLE							(0x50)
#define VENUS_AF_SET_WEIGHT							(0x52)
#define VENUS_AF_GET_STATUS             			(0x53)

#define VENUS_AF_GET_ACC                            (0xBC)
#define VENUS_AF_ALGO_ACC                           (0xBD)
#define VENUS_AF_SPECIAL_DATA                       (0xBE)

#define VENUS_SET_ISP_AUTO_FOCUS_START1				(0xA0)
#define VENUS_SET_ISP_AUTO_FOCUS_START2				(0xA5)
#define VENUS_SET_ISP_AUTO_FOCUS_STOP				(0xA1)

#define VENUS_SET_AF_POS							(0xA7)
#define VENUS_SET_AE_MODE							(0xA8)

#define	VENUS_GET_AF_POS							(0xAC)

#define VENUS_SET_ISP_MANUAL_FOCUS_START			(0xB6)

/*****************************************************************************/
/* Venus ISP Control API                                                     */
/*****************************************************************************/
#define VENUS_EXPO_VALUE							(0x06)
#define VENUS_WB_MODE								(0x07)

#define VENUS_CONTRAST								(0x13)
#define VENUS_SATURATION							(0x14)
#define VENUS_SHARPNESS                 			(0x4E)

#define VENUS_SET_GAMMA								(0x5C)
#define VENUS_GET_GAMMA								(0x5E)

#define VENUS_GET_LIGHT_COND						(0xA4)

#define VENUS_SET_FLICKER_MODE						(0xAA)

#define VENUS_SET_ISO								(0xAB)

#define VENUS_SET_IMAGE_EFFECT						(0xB0)
#define VENUS_SET_SCENE_MODE						(0xB1)
#define VENUS_SET_WDR								(0xB2)

/*****************************************************************************/
/* Venus Calibration API                                                     */
/*****************************************************************************/
#define VENUS_5M_RAWPROC_STORE						(0xB3)
#define VENUS_XGA_RAWPROC_STORE						(0xB4)
#define VENUS_PREVIEWDRAWRECT						(0xB5)

#define VENUS_ISP_MODE_SELECT						(0xB7)
#define VENUS_SPI_READ_CALI							(0xB8)
	#define VENUS_SPI_READ_PM_DATA						(0x00)
	#define VENUS_SPI_READ_CALI_DATA					(0x01)
#define	VENUS_CALIBRATION_MODE						(0xB9)
#define	VENUS_HYV_PLATFORM							(0xBB)
#define VENUS_AWBCALIBYISP_SET						(0xC3)
#define VENUS_AWBCALIBYISP_GET						(0xC4)
#define VENUS_FLASH_DATA_READ                       (0xC5)
#define VENUS_GET_RGB_RATIO_AFTER_AWB               (0xC6)

/*****************************************************************************/
/* Venus IQ Tuning API                                                       */
/*****************************************************************************/
#define VENUS_IQ_TUNNING                            (0xFF)
	#define IQ_PARAM								(0x01)
	#define IQ_AE_CONFIG								(0xF0)
	#define IQ_AF_CONFIG								(0xF1)
	#define IQ_AWB_CONFIG								(0xF2)
	#define IQ_ANTIFLICKER								(0xF3)

/*****************************************************************************/
/* Venus Simulation API                                                      */
/*****************************************************************************/
#define VENUS_DO_SIMULATION							(0xD0)

#define VENUS_SET_SIMULATION_RAW_RESOL				(0xD1)
#define VENUS_SET_SIMULATION_RAW_ADDR				(0xD2)

#define VENUS_SET_SIMULATION_MAIN_RESOL				(0xD3)
#define VENUS_GET_SIMULATION_MAIN_ADDR				(0xD4)
#define VENUS_GET_SIMULATION_MAIN_SIZE				(0xD5)


#endif // _VENUS_HOSTCMD_H_
