/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   image_sensor.h
 *
 * Version:
 * --------
 *   GT2005 MTK 6235 DRV V1.1.0
 *
 * Release Date:
 * ------------
 *   2010-10-25
 *
 ****************************************************************************/
#ifndef _IMAGE_SENSOR_GT2005_H
#define _IMAGE_SENSOR_GT2005_H

	//#include "isp_if.h"

	#define __IMAGE_SENSOR_2M__
	
	//------------------------Engineer mode---------------------------------
	
	#define FACTORY_START_ADDR 70
	
	typedef enum {
	   CAM_PARAM_ZOOM_FACTOR=0,
	   CAM_PARAM_CONTRAST,
	   CAM_PARAM_BRIGHTNESS,
	   CAM_PARAM_HUE,
	   CAM_PARAM_GAMMA,
	   CAM_PARAM_WB,
	   CAM_PARAM_EXPOSURE,
	   CAM_PARAM_EFFECT,
	   CAM_PARAM_BANDING,
	   CAM_PARAM_SATURATION,
	   CAM_PARAM_NIGHT_MODE,
	   CAM_PARAM_EV_VALUE	  
	} GT2005_SENSOR_SETTING_GROUP_ENUM;

	typedef enum {
	   CAM_WB_AUTO=0,
	   CAM_WB_CLOUD,
	   CAM_WB_DAYLIGHT,
	   CAM_WB_INCANDESCENCE,
	   CAM_WB_TUNGSTEN,
	   CAM_WB_FLUORESCENT,
	   CAM_WB_MANUAL
	} GT2005_WB_GROUP_ENUM;

	typedef enum {
	   CAM_EV_NEG_4_3=0,
	   CAM_EV_NEG_3_3,
	   CAM_EV_NEG_2_3,
	   CAM_EV_NEG_1_3,
	   CAM_EV_ZERO,
	   CAM_EV_POS_1_3,
	   CAM_EV_POS_2_3,
	   CAM_EV_POS_3_3,
	   CAM_EV_POS_4_3
	} GT2005_EV_GROUP_ENUM;

	typedef enum {
	   CAM_EFFECT_ENC_NORMAL=0,
	   CAM_EFFECT_ENC_GRAYSCALE,
	   CAM_EFFECT_ENC_SEPIA,
	   CAM_EFFECT_ENC_SEPIAGREEN,
	   CAM_EFFECT_ENC_SEPIABLUE,
	   CAM_EFFECT_ENC_COLORINV
	} GT2005_EFFECT_GROUP_ENUM;

	typedef enum {
	   CAM_BANDING_50HZ=0,
	   CAM_BANDING_60HZ
	} GT2005_BANDING_GROUP_ENUM;

	
	typedef enum group_enum {
	   AWB_GAIN=0,
	   PRE_GAIN,
	   SENSOR_DBLC,
	   GAMMA_ENABLE,
	   CMMCLK_CURRENT,
	   FRAME_RATE_LIMITATION,
	   REGISTER_EDITOR,
	   GROUP_TOTAL_NUMS
	} FACTORY_CCT_GROUP_ENUM;
	
	typedef enum register_index {
	   AWB_GAIN_R_INDEX=FACTORY_START_ADDR,
	   AWB_GAIN_B_INDEX,
	   SENSOR_DBLC_INDEX,
	   GAMMA_ENABLE_INDEX,
	   CMMCLK_CURRENT_INDEX,	   
	   FACTORY_END_ADDR
	} FACTORY_REGISTER_INDEX;
	
	typedef enum cct_register_index {
	   GLOBAL_GAIN_INDEX=0,
	   PRE_GAIN_R_INDEX,
	   PRE_GAIN_B_INDEX,
	   CCT_END_ADDR
	} CCT_REGISTER_INDEX;
	
	typedef struct
	{
	   unsigned char   item_name_ptr[50];         // item name
	   unsigned int	item_value;                // item value
	   bool    is_true_false;             // is this item for enable/disable functions
	   bool	   is_read_only;              // is this item read only
	   bool	   is_need_restart;           // after set this item need restart
	   unsigned int	min;                       // min value of item value	
	   unsigned int	max;                       // max value of item value	
	} ENG_sensor_info;
	
	// API FOR ENGINEER FACTORY MODE
	//void  get_sensor_group_count(kal_int32* sensor_count_ptr);
	//void  get_sensor_group_info(kal_uint16 group_idx, kal_int8* group_name_ptr, kal_int32* item_count_ptr);
	//void  get_sensor_item_info(kal_uint16 group_idx,kal_uint16 item_idx, ENG_sensor_info* info_ptr);
	//kal_bool set_sensor_item_info(kal_uint16 group_idx, kal_uint16 item_idx, kal_int32 item_value);
	
	//------------------------Engineer mode---------------------------------
	
	typedef struct {
		unsigned int	addr;
		unsigned int	para;
	} sensor_reg_struct;
	
	typedef struct {
		sensor_reg_struct	reg[FACTORY_END_ADDR];
		sensor_reg_struct	cct[CCT_END_ADDR];
	} sensor_data_struct;

	// write camera_para to sensor register 
	//void camera_para_to_sensor(void);
	// update camera_para from sensor register 
	//void sensor_to_camera_para(void);
	// config sensor callback function 

	unsigned char GT2005_Init(void);
	void GT2005_Preview(void);
	void GT2005_Capture(void);
	unsigned int GT2005_YUV_SensorSetting(unsigned int cmd, unsigned int para);
	void image_sensor_func_config(void);
	// Compact Image Sensor Module Power ON/OFF
	//void cis_module_power_on(kal_bool on);
	
	/* HW PRODUCE I2C SIGNAL TO CONTROL SENSOR REGISTER */
	#define HW_SCCB
	
	/* OUTPUT DEBUG INFO. BY UART */
	//#define OUTPUT_DEBUG_INFO

	typedef enum _SENSOR_TYPE {
		CMOS_SENSOR=0,
		CCD_SENSOR
	} SENSOR_TYPE;

	typedef struct {
		unsigned short		id;
		SENSOR_TYPE		type;
	} SensorInfo;

	/* MAXIMUM EXPLOSURE LINES USED BY AE */
	extern unsigned short MAX_EXPOSURE_LINES;
	extern unsigned char  MIN_EXPOSURE_LINES;
	/* AE CONTROL CRITERION */
	extern unsigned char   AE_AWB_CAL_PERIOD;
	extern unsigned char   AE_GAIN_DELAY_PERIOD;
	extern unsigned char   AE_SHUTTER_DELAY_PERIOD;
	
	/* 1M RESOLUTION SIZE */
	#define IMAGE_SENSOR_1M_WIDTH					1280
	#define IMAGE_SENSOR_1M_HEIGHT					960

	/* 2M RESOLUTION SIZE */
	#define IMAGE_SENSOR_2M_WIDTH					1600
	#define IMAGE_SENSOR_2M_HEIGHT					1200

	/* SENSOR VGA SIZE */
	#define IMAGE_SENSOR_VGA_WIDTH					640
	#define IMAGE_SENSOR_VGA_HEIGHT					480
	
	#define GT2005_WRITE_ID         (0x78)
	#define GT2005_READ_ID           (0x79)	

	/* SENSOR CHIP VERSION */
	#define GT2005_SENSOR_ID		(0x5138)	
	
    	#define IMAGE_SENSOR_FULL_WIDTH					1600	
	#define IMAGE_SENSOR_FULL_HEIGHT					1200	
	
	#define PV_PERIOD_PIXEL_NUMS						800
	#define PV_PERIOD_LINE_NUMS						600
	
	#define SENSOR_I2C_DELAY							0xFF

	#define SET_RESET_CMOS_SENSOR_HIGH  (REG_ISP_CMOS_SENSOR_MODE_CONFIG |= REG_CMOS_SENSOR_RESET_BIT)
    	#define SET_RESET_CMOS_SENSOR_LOW   (REG_ISP_CMOS_SENSOR_MODE_CONFIG &= ~REG_CMOS_SENSOR_RESET_BIT)

    	#define PWRDN_PIN_HIGH               (REG_ISP_CMOS_SENSOR_MODE_CONFIG |= REG_CMOS_SENSOR_POWER_ON_BIT)
    	#define PWRDN_PIN_LOW                (REG_ISP_CMOS_SENSOR_MODE_CONFIG &= ~REG_CMOS_SENSOR_POWER_ON_BIT)

	#ifndef HW_SCCB
		#define I2C_START_TRANSMISSION \
	    { \
			volatile kal_uint32 j; \
	        SET_SCCB_DATA_OUTPUT; \
			SET_SCCB_CLK_OUTPUT; \
			SET_SCCB_DATA_HIGH; \
			for(j=0;j<SENSOR_I2C_DELAY;j++);\
			SET_SCCB_CLK_HIGH; \
			for(j=0;j<SENSOR_I2C_DELAY;j++);\
			SET_SCCB_DATA_LOW; \
			for(j=0;j<SENSOR_I2C_DELAY;j++);\
			SET_SCCB_CLK_LOW; \
	    }
		
		#define I2C_STOP_TRANSMISSION \
		{ \
			volatile kal_uint32 j; \
			SET_SCCB_CLK_OUTPUT; \
			SET_SCCB_DATA_OUTPUT; \
			SET_SCCB_CLK_LOW; \
			SET_SCCB_DATA_LOW; \
			for(j=0;j<SENSOR_I2C_DELAY;j++);\
			SET_SCCB_CLK_HIGH; \
			for(j=0;j<SENSOR_I2C_DELAY;j++);\
			SET_SCCB_DATA_HIGH; \
		}
	#endif

#endif /* _IMAGE_SENSOR_GT2005_H */
