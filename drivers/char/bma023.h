/*  $Date: 2009/10/23 $
 *  $Revision: 1.2 $
 */

/** \mainpage BMA023 Acceleration Sensor API
 * Copyright (C) 2009 Bosch Sensortec GmbH
 *  \section intro_sec Introduction
 * BMA023 3-axis digital Accelerometer Programming Interface
 * The BMA023 API enables quick access to Bosch Sensortec's 3-Axis digital accelerometer.
 * The only mandatory steps are: 
 *
 * 1. linking the target application's communication functions to the API (\ref BMA023_WR_FUNC_PTR, \ref BMA023_RD_FUNC_PTR)
 *
 * 2. calling the BMA023_init() routine, which initializes all necessary data structures for using all functions
 *
 * Author:	Francois.beauchaud@bosch-sensortec.com
 *
 * 
 * \section disclaimer_sec Disclaimer
 *
* 
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in 
  compliance with the License and the following stipulations. The Apache License , Version 2.0 is applicable unless 
  otherwise stated by the stipulations of the disclaimer below. 
 
* You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0
  
 

Disclaimer 

* Common:
* This Work is developed for the consumer goods industry. It may only be used 
* within the parameters of the respective valid product data sheet.  The Work 
* provided with the express understanding that there is no warranty of fitness for a particular purpose. 
* It is not fit for use in life-sustaining, safety or security sensitive systems or any system or device 
* that may lead to bodily harm or property damage if the system or device malfunctions. In addition, 
* the Work is not fit for use in products which interact with motor vehicle systems.  
* The resale and/or use of the Work are at the purchaser’s own risk and his own responsibility. The 
* examination of fitness for the intended use is the sole responsibility of the Purchaser. 
*
* The purchaser shall indemnify Bosch Sensortec from all third party claims, including any claims for 
* incidental, or consequential damages, arising from any Work or Derivative Work use not covered by the parameters of 
* the respective valid product data sheet or not approved by Bosch Sensortec and reimburse Bosch 
* Sensortec for all costs in connection with such claims.
*
* The purchaser must monitor the market for the purchased Work and Derivative Works, particularly with regard to 
* product safety and inform Bosch Sensortec without delay of all security relevant incidents.
*
* Engineering Samples are marked with an asterisk (*) or (e). Samples may vary from the valid 
* technical specifications of the product series. They are therefore not intended or fit for resale to third 
* parties or for use in end products. Their sole purpose is internal client testing. The testing of an 
* engineering sample may in no way replace the testing of a product series. Bosch Sensortec 
* assumes no liability for the use of engineering samples. By accepting the engineering samples, the 
* Purchaser agrees to indemnify Bosch Sensortec from all claims arising from the use of engineering 
* samples.
*
* Special:
* This Work and any related information (hereinafter called "Information") is provided free of charge 
* for the sole purpose to support your application work. The Work and Information is subject to the 
* following terms and conditions: 
*
* The Work is specifically designed for the exclusive use for Bosch Sensortec products by 
* personnel who have special experience and training. Do not use this Work or Derivative Works if you do not have the 
* proper experience or training. Do not use this Work or Derivative Works fot other products than Bosch Sensortec products.  
*
* The Information provided is believed to be accurate and reliable. Bosch Sensortec assumes no 
* responsibility for the consequences of use of such Information nor for any infringement of patents or 
* other rights of third parties which may result from its use. No license is granted by implication or 
* otherwise under any patent or patent rights of Bosch. Specifications mentioned in the Information are 
* subject to change without notice.
*
*/

 /** \file BMA023.h
    \brief Header file for all #define constants and function prototypes
  
    
*/



#ifndef __BMA023_H__
#define __BMA023_H__



/* BMA023 Macro for read and write commincation */


/**
   define for used read and write macros 
*/


/** Define the calling convention of YOUR bus communication routine.
	\note This includes types of parameters. This example shows the configuration for an SPI bus link.

	If your communication function looks like this:

	write_my_bus_xy(unsigned char device_addr, unsigned char register_addr, unsigned char * data, unsigned char length);

	The BMA023_WR_FUNC_PTR would equal:

    #define	BMA023_WR_FUNC_PTR char (* bus_write)(unsigned char, unsigned char, unsigned char *, unsigned char)
	
	Parameters can be mixed as needed refer to the \ref BMA023_BUS_WRITE_FUNC  macro.
	

*/
#define BMA023_WR_FUNC_PTR char (* bus_write)(unsigned char, unsigned char, unsigned char *, unsigned char)



/** link makro between API function calls and bus write function
	\note The bus write function can change since this is a system dependant issue.

	If the bus_write parameter calling order is like: reg_addr, reg_data, wr_len it would be as it is here.

	If the parameters are differently ordered or your communication function like I2C need to know the device address, 
	you can change this macro accordingly.


	define BMA023_BUS_WRITE_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
           bus_write(dev_addr, reg_addr, reg_data, wr_len)

    This macro lets all API functions call YOUR communication routine in a way that equals your definition in the \ref BMA023_WR_FUNC_PTR definition.


	      
*/
#define BMA023_BUS_WRITE_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
           bus_write(dev_addr, reg_addr, reg_data, wr_len)


/** Define the calling convention of YOUR bus communication routine.
	\note This includes types of parameters. This example shows the configuration for an SPI bus link.

	If your communication function looks like this:

	read_my_bus_xy(unsigned char device_addr, unsigned char register_addr, unsigned char * data, unsigned char length);

	The BMA023_RD_FUNC_PTR would equal:

    #define	BMA023_RD_FUNC_PTR char (* bus_read)(unsigned char, unsigned char, unsigned char *, unsigned char)
	
	Parameters can be mixed as needed refer to the \ref BMA023_BUS_READ_FUNC  macro.
	

*/

#define BMA023_SPI_RD_MASK 0x80   /* for spi read transactions on SPI the MSB has to be set */
#define BMA023_RD_FUNC_PTR char (* bus_read)(unsigned char, unsigned char, unsigned char *, unsigned char)


/** link makro between API function calls and bus read function
	\note The bus write function can change since this is a system dependant issue.

	If the bus_read parameter calling order is like: reg_addr, reg_data, wr_len it would be as it is here.

	If the parameters are differently ordered or your communication function like I2C need to know the device address, 
	you can change this macro accordingly.


	define BMA023_BUS_READ_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
           bus_read(dev_addr, reg_addr, reg_data, wr_len)

    This macro lets all API functions call YOUR communication routine in a way that equals your definition in the \ref BMA023_WR_FUNC_PTR definition.
*/

#define BMA023_BUS_READ_FUNC(dev_addr, reg_addr, reg_data, r_len)\
           bus_read(dev_addr, reg_addr, reg_data, r_len)







/** BMA023 I2C Address
*/

#define BMA023_I2C_ADDR		0x38


/*
	BMA023 API error codes
*/

#define E_SMB_NULL_PTR		(char)-127
#define E_COMM_RES		    (char)-1
#define E_OUT_OF_RANGE		(char)-2


/* 
 *	
 *	register definitions 	
 *
 */


#define BMA023_EEP_OFFSET   0x20
#define BMA023_IMAGE_BASE	0x0b
#define BMA023_IMAGE_LEN	19

#define BMA023_CHIP_ID_REG			0x00
#define BMA023_VERSION_REG			0x01
#define BMA023_X_AXIS_LSB_REG		0x02
#define BMA023_X_AXIS_MSB_REG		0x03
#define BMA023_Y_AXIS_LSB_REG		0x04
#define BMA023_Y_AXIS_MSB_REG		0x05
#define BMA023_Z_AXIS_LSB_REG		0x06
#define BMA023_Z_AXIS_MSB_REG		0x07
#define BMA023_TEMP_RD_REG			0x08
#define BMA023_STATUS_REG	0x09
#define BMA023_CTRL_REG		0x0a
#define BMA023_CONF1_REG	0x0b
#define BMA023_LG_THRESHOLD_REG	0x0c
#define BMA023_LG_DURATION_REG		0x0d
#define BMA023_HG_THRESHOLD_REG	0x0e
#define BMA023_HG_DURATION_REG		0x0f
#define BMA023_MOTION_THRS_REG		0x10
#define BMA023_HYSTERESIS_REG		0x11
#define BMA023_CUSTOMER1_REG		0x12
#define BMA023_CUSTOMER2_REG		0x13
#define BMA023_RANGE_BWIDTH_REG	0x14
#define BMA023_CONF2_REG	0x15

/* register write and read delays */

#define BMA023_MDELAY_DATA_TYPE	unsigned int
#define BMA023_EE_W_DELAY 28	/* delay after EEP write is 28 msec */



/** BMA023 acceleration data 
	\brief Structure containing acceleration values for x,y and z-axis in signed short

*/

typedef struct  {
		short x, /**< holds x-axis acceleration data sign extended. Range -512 to 511. */
			  y, /**< holds y-axis acceleration data sign extended. Range -512 to 511. */
			  z; /**< holds z-axis acceleration data sign extended. Range -512 to 511. */
} bma023acc_t;


/** BMA023 image registers data structure
	\brief Register type that contains all BMA023 image registers from address 0x0b to 0x15
	This structure can hold the complete image data of BMA023

*/
typedef struct  {
		unsigned char	
		bma023_conf1 ,  /**<  image address 0x0b: interrupt enable bits, low-g settings */
		lg_threshold,	/**<  image address 0x0c: low-g threshold, depends on selected g-range */
		lg_duration,	/**<  image address 0x0d: low-g duration in ms */
		hg_threshold,	/**<  image address 0x0e: high-g threshold, depends on selected g-range */
		hg_duration,	/**<  image address 0x0f: high-g duration in ms */
		motion_thrs,	/**<  image address 0x10: any motion threshold */
		hysteresis,		/**<  image address 0x11: low-g and high-g hysteresis register */
		customer1,		/**<  image address 0x12: customer reserved register 1 */
		customer2,		/**<  image address 0x13: customer reserved register 2  */
		range_bwidth,	/**<  image address 0x14: range and bandwidth selection register */
		bma150_conf2,	/**<  image address 0x15: spi4, latched interrupt, auto-wake-up configuration */
		offs_gain_x,	/**<  image address 0x16: offset_x LSB and x-axis gain settings */
		offs_gain_y,	/**<  image address 0x17: offset_y LSB and y-axis gain settings */
		offs_gain_z,	/**<  image address 0x18: offset_z LSB and z-axis gain settings */
		offs_gain_t,	/**<  image address 0x19: offset_t LSB and temperature gain settings */
		offset_x,		/**<  image address 0x1a: offset_x calibration MSB register */
		offset_y,		/**<  image address 0x1b: offset_y calibration MSB register */ 
		offset_z,		/**<  image address 0x1c: offset_z calibration MSB register */ 
		offset_t;		/**<  image address 0x1d: temperature calibration MSB register */ 
} bma023regs_t;


/** bma023 typedef structure
	\brief This structure holds all relevant information about BMA023 and links communication to the 
*/

typedef struct {	
	bma023regs_t * image;	/**< pointer to bma023regs_t structure not mandatory */
	unsigned char mode;		/**< save current BMA023 operation mode */
	unsigned char chip_id;	/**< save BMA023's chip id which has to be 0x02 after calling bma023_init() */
	unsigned char dev_addr;   /**< initializes BMA023's I2C device address 0x38 */
	unsigned char int_mask;	  /**< stores the current BMA023 API generated interrupt mask */
	BMA023_WR_FUNC_PTR;		  /**< function pointer to the SPI/I2C write function */
	BMA023_RD_FUNC_PTR;		  /**< function pointer to the SPI/I2C read function */
	void (*delay_msec)( BMA023_MDELAY_DATA_TYPE ); /**< function pointer to a pause in mili seconds function */
} bma023_t;




	
/* 
 *	
 *	bit slice positions in registers
 *
 */

/** \cond BITSLICE */

#define BMA023_CHIP_ID__POS		0
#define BMA023_CHIP_ID__MSK		0x07
#define BMA023_CHIP_ID__LEN		3
#define BMA023_CHIP_ID__REG		BMA023_CHIP_ID_REG


/* DATA REGISTERS */


#define BMA023_NEW_DATA_X__POS  	0
#define BMA023_NEW_DATA_X__LEN  	1
#define BMA023_NEW_DATA_X__MSK  	0x01
#define BMA023_NEW_DATA_X__REG		BMA023_X_AXIS_LSB_REG

#define BMA023_ACC_X_LSB__POS   	6
#define BMA023_ACC_X_LSB__LEN   	2
#define BMA023_ACC_X_LSB__MSK		0xC0
#define BMA023_ACC_X_LSB__REG		BMA023_X_AXIS_LSB_REG

#define BMA023_ACC_X_MSB__POS   	0
#define BMA023_ACC_X_MSB__LEN   	8
#define BMA023_ACC_X_MSB__MSK		0xFF
#define BMA023_ACC_X_MSB__REG		BMA023_X_AXIS_MSB_REG

#define BMA023_NEW_DATA_Y__POS  	0
#define BMA023_NEW_DATA_Y__LEN  	1
#define BMA023_NEW_DATA_Y__MSK  	0x01
#define BMA023_NEW_DATA_Y__REG		BMA023_Y_AXIS_LSB_REG

#define BMA023_ACC_Y_LSB__POS   	6
#define BMA023_ACC_Y_LSB__LEN   	2
#define BMA023_ACC_Y_LSB__MSK   	0xC0
#define BMA023_ACC_Y_LSB__REG		BMA023_Y_AXIS_LSB_REG

#define BMA023_ACC_Y_MSB__POS   	0
#define BMA023_ACC_Y_MSB__LEN   	8
#define BMA023_ACC_Y_MSB__MSK   	0xFF
#define BMA023_ACC_Y_MSB__REG		BMA023_Y_AXIS_MSB_REG

#define BMA023_NEW_DATA_Z__POS  	0
#define BMA023_NEW_DATA_Z__LEN  	1
#define BMA023_NEW_DATA_Z__MSK		0x01
#define BMA023_NEW_DATA_Z__REG		BMA023_Z_AXIS_LSB_REG

#define BMA023_ACC_Z_LSB__POS   	6
#define BMA023_ACC_Z_LSB__LEN   	2
#define BMA023_ACC_Z_LSB__MSK		0xC0
#define BMA023_ACC_Z_LSB__REG		BMA023_Z_AXIS_LSB_REG

#define BMA023_ACC_Z_MSB__POS   	0
#define BMA023_ACC_Z_MSB__LEN   	8
#define BMA023_ACC_Z_MSB__MSK		0xFF
#define BMA023_ACC_Z_MSB__REG		BMA023_Z_AXIS_MSB_REG

#define BMA023_TEMPERATURE__POS 	0
#define BMA023_TEMPERATURE__LEN 	8
#define BMA023_TEMPERATURE__MSK 	0xFF
#define BMA023_TEMPERATURE__REG		BMA023_TEMP_RD_REG




/* STATUS BITS */

#define BMA023_STATUS_HG__POS		0
#define BMA023_STATUS_HG__LEN		1
#define BMA023_STATUS_HG__MSK		0x01
#define BMA023_STATUS_HG__REG		BMA023_STATUS_REG

#define BMA023_STATUS_LG__POS		1
#define BMA023_STATUS_LG__LEN		1
#define BMA023_STATUS_LG__MSK		0x02
#define BMA023_STATUS_LG__REG		BMA023_STATUS_REG

#define BMA023_HG_LATCHED__POS  	2
#define BMA023_HG_LATCHED__LEN  	1
#define BMA023_HG_LATCHED__MSK		0x04
#define BMA023_HG_LATCHED__REG		BMA023_STATUS_REG

#define BMA023_LG_LATCHED__POS		3
#define BMA023_LG_LATCHED__LEN		1
#define BMA023_LG_LATCHED__MSK		8
#define BMA023_LG_LATCHED__REG		BMA023_STATUS_REG

#define BMA023_ALERT_PHASE__POS		4
#define BMA023_ALERT_PHASE__LEN		1
#define BMA023_ALERT_PHASE__MSK		0x10
#define BMA023_ALERT_PHASE__REG		BMA023_STATUS_REG


#define BMA023_ST_RESULT__POS		7
#define BMA023_ST_RESULT__LEN		1
#define BMA023_ST_RESULT__MSK		0x80
#define BMA023_ST_RESULT__REG		BMA023_STATUS_REG


/* CONTROL BITS */

#define BMA023_SLEEP__POS			0
#define BMA023_SLEEP__LEN			1
#define BMA023_SLEEP__MSK			0x01
#define BMA023_SLEEP__REG			BMA023_CTRL_REG

#define BMA023_SOFT_RESET__POS		1
#define BMA023_SOFT_RESET__LEN		1
#define BMA023_SOFT_RESET__MSK		0x02
#define BMA023_SOFT_RESET__REG		BMA023_CTRL_REG





#define BMA023_SELF_TEST__POS		2
#define BMA023_SELF_TEST__LEN		2
#define BMA023_SELF_TEST__MSK		0x0C
#define BMA023_SELF_TEST__REG		BMA023_CTRL_REG




#define BMA023_SELF_TEST0__POS		2
#define BMA023_SELF_TEST0__LEN		1
#define BMA023_SELF_TEST0__MSK		0x04
#define BMA023_SELF_TEST0__REG		BMA023_CTRL_REG

#define BMA023_SELF_TEST1__POS		3
#define BMA023_SELF_TEST1__LEN		1
#define BMA023_SELF_TEST1__MSK		0x08
#define BMA023_SELF_TEST1__REG		BMA023_CTRL_REG




#define BMA023_EE_W__POS			4
#define BMA023_EE_W__LEN			1
#define BMA023_EE_W__MSK			0x10
#define BMA023_EE_W__REG			BMA023_CTRL_REG

#define BMA023_UPDATE_IMAGE__POS	5
#define BMA023_UPDATE_IMAGE__LEN	1
#define BMA023_UPDATE_IMAGE__MSK	0x20
#define BMA023_UPDATE_IMAGE__REG	BMA023_CTRL_REG

#define BMA023_RESET_INT__POS		6
#define BMA023_RESET_INT__LEN		1
#define BMA023_RESET_INT__MSK		0x40
#define BMA023_RESET_INT__REG		BMA023_CTRL_REG



/* LOW-G, HIGH-G settings */



#define BMA023_ENABLE_LG__POS		0
#define BMA023_ENABLE_LG__LEN		1
#define BMA023_ENABLE_LG__MSK		0x01
#define BMA023_ENABLE_LG__REG		BMA023_CONF1_REG




#define BMA023_ENABLE_HG__POS		1
#define BMA023_ENABLE_HG__LEN		1
#define BMA023_ENABLE_HG__MSK		0x02
#define BMA023_ENABLE_HG__REG		BMA023_CONF1_REG


/* LG/HG counter */

	

#define BMA023_COUNTER_LG__POS			2
#define BMA023_COUNTER_LG__LEN			2
#define BMA023_COUNTER_LG__MSK			0x0C
#define BMA023_COUNTER_LG__REG			BMA023_CONF1_REG
	
#define BMA023_COUNTER_HG__POS			4
#define BMA023_COUNTER_HG__LEN			2
#define BMA023_COUNTER_HG__MSK			0x30
#define BMA023_COUNTER_HG__REG			BMA023_CONF1_REG




/* LG/HG duration is in ms */

#define BMA023_LG_DUR__POS			0
#define BMA023_LG_DUR__LEN			8
#define BMA023_LG_DUR__MSK			0xFF
#define BMA023_LG_DUR__REG			BMA023_LG_DURATION_REG

#define BMA023_HG_DUR__POS			0
#define BMA023_HG_DUR__LEN			8
#define BMA023_HG_DUR__MSK			0xFF
#define BMA023_HG_DUR__REG			BMA023_HG_DURATION_REG




				

#define BMA023_LG_THRES__POS		0
#define BMA023_LG_THRES__LEN		8
#define BMA023_LG_THRES__MSK		0xFF
#define BMA023_LG_THRES__REG		BMA023_LG_THRESHOLD_REG





#define BMA023_HG_THRES__POS		0
#define BMA023_HG_THRES__LEN		8
#define BMA023_HG_THRES__MSK		0xFF
#define BMA023_HG_THRES__REG		BMA023_HG_THRESHOLD_REG








#define BMA023_LG_HYST__POS			0
#define BMA023_LG_HYST__LEN			3
#define BMA023_LG_HYST__MSK			0x07
#define BMA023_LG_HYST__REG			BMA023_HYSTERESIS_REG




#define BMA023_HG_HYST__POS			3
#define BMA023_HG_HYST__LEN			3
#define BMA023_HG_HYST__MSK			0x38
#define BMA023_HG_HYST__REG			BMA023_HYSTERESIS_REG


/* ANY MOTION and ALERT settings */

#define BMA023_EN_ANY_MOTION__POS		6
#define BMA023_EN_ANY_MOTION__LEN		1
#define BMA023_EN_ANY_MOTION__MSK		0x40
#define BMA023_EN_ANY_MOTION__REG		BMA023_CONF1_REG


/* ALERT settings */


#define BMA023_ALERT__POS			7
#define BMA023_ALERT__LEN			1
#define BMA023_ALERT__MSK			0x80
#define BMA023_ALERT__REG			BMA023_CONF1_REG


/* ANY MOTION Duration */




#define BMA023_ANY_MOTION_THRES__POS	0
#define BMA023_ANY_MOTION_THRES__LEN	8
#define BMA023_ANY_MOTION_THRES__MSK	0xFF
#define BMA023_ANY_MOTION_THRES__REG	BMA023_MOTION_THRS_REG




#define BMA023_ANY_MOTION_DUR__POS		6
#define BMA023_ANY_MOTION_DUR__LEN		2
#define BMA023_ANY_MOTION_DUR__MSK		0xC0	
#define BMA023_ANY_MOTION_DUR__REG		BMA023_HYSTERESIS_REG


#define BMA023_CUSTOMER_RESERVED1__POS		0
#define BMA023_CUSTOMER_RESERVED1__LEN	 	8
#define BMA023_CUSTOMER_RESERVED1__MSK		0xFF
#define BMA023_CUSTOMER_RESERVED1__REG		BMA023_CUSTOMER1_REG

#define BMA023_CUSTOMER_RESERVED2__POS		0
#define BMA023_CUSTOMER_RESERVED2__LEN	 	8
#define BMA023_CUSTOMER_RESERVED2__MSK		0xFF
#define BMA023_CUSTOMER_RESERVED2__REG		BMA023_CUSTOMER2_REG



/* BANDWIDTH dependend definitions */

#define BMA023_BANDWIDTH__POS				0
#define BMA023_BANDWIDTH__LEN			 	3
#define BMA023_BANDWIDTH__MSK			 	0x07
#define BMA023_BANDWIDTH__REG				BMA023_RANGE_BWIDTH_REG




/* RANGE */

#define BMA023_RANGE__POS				3
#define BMA023_RANGE__LEN				2
#define BMA023_RANGE__MSK				0x18	
#define BMA023_RANGE__REG				BMA023_RANGE_BWIDTH_REG


/* WAKE UP */



#define BMA023_WAKE_UP__POS			0
#define BMA023_WAKE_UP__LEN			1
#define BMA023_WAKE_UP__MSK			0x01
#define BMA023_WAKE_UP__REG			BMA023_CONF2_REG




#define BMA023_WAKE_UP_PAUSE__POS		1
#define BMA023_WAKE_UP_PAUSE__LEN		2
#define BMA023_WAKE_UP_PAUSE__MSK		0x06
#define BMA023_WAKE_UP_PAUSE__REG		BMA023_CONF2_REG


/* ACCELERATION DATA SHADOW */



#define BMA023_SHADOW_DIS__POS			3
#define BMA023_SHADOW_DIS__LEN			1
#define BMA023_SHADOW_DIS__MSK			0x08
#define BMA023_SHADOW_DIS__REG			BMA023_CONF2_REG


/* LATCH Interrupt */



#define BMA023_LATCH_INT__POS			4
#define BMA023_LATCH_INT__LEN			1
#define BMA023_LATCH_INT__MSK			0x10
#define BMA023_LATCH_INT__REG			BMA023_CONF2_REG

/* new data interrupt */


#define BMA023_NEW_DATA_INT__POS		5
#define BMA023_NEW_DATA_INT__LEN		1
#define BMA023_NEW_DATA_INT__MSK		0x20
#define BMA023_NEW_DATA_INT__REG		BMA023_CONF2_REG



#define BMA023_ENABLE_ADV_INT__POS		6
#define BMA023_ENABLE_ADV_INT__LEN		1
#define BMA023_ENABLE_ADV_INT__MSK		0x40
#define BMA023_ENABLE_ADV_INT__REG		BMA023_CONF2_REG


#define BMA023_BMA023_SPI4_OFF	0
#define BMA023_BMA023_SPI4_ON	1

#define BMA023_SPI4__POS				7
#define BMA023_SPI4__LEN				1
#define BMA023_SPI4__MSK				0x80
#define BMA023_SPI4__REG				BMA023_CONF2_REG


#define BMA023_GET_BITSLICE(regvar, bitname)\
			(regvar & bitname##__MSK) >> bitname##__POS


#define BMA023_SET_BITSLICE(regvar, bitname, val)\
		  (regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK)  


/** \endcond */


/* CONSTANTS */


/* range and bandwidth */

#define BMA023_RANGE_2G			0 /**< sets range to 2G mode \see bma023_set_range() */
#define BMA023_RANGE_4G			1 /**< sets range to 4G mode \see bma023_set_range() */
#define BMA023_RANGE_8G			2 /**< sets range to 8G mode \see bma023_set_range() */


#define BMA023_BW_25HZ		  0	/**< sets bandwidth to 25HZ \see bma023_set_bandwidth() */
#define BMA023_BW_50HZ		  1	/**< sets bandwidth to 50HZ \see bma023_set_bandwidth() */
#define BMA023_BW_100HZ		  2	/**< sets bandwidth to 100HZ \see bma023_set_bandwidth() */
#define BMA023_BW_190HZ		  3	/**< sets bandwidth to 190HZ \see bma023_set_bandwidth() */
#define BMA023_BW_375HZ		  4	/**< sets bandwidth to 375HZ \see bma023_set_bandwidth() */
#define BMA023_BW_750HZ		  5	/**< sets bandwidth to 750HZ \see bma023_set_bandwidth() */
#define BMA023_BW_1500HZ	  6	/**< sets bandwidth to 1500HZ \see bma023_set_bandwidth() */

/* mode settings */

#define BMA023_MODE_NORMAL      0
#define BMA023_MODE_SLEEP       2
#define BMA023_MODE_WAKE_UP     3

/* wake up */

#define BMA023_WAKE_UP_PAUSE_20MS		  0
#define BMA023_WAKE_UP_PAUSE_80MS		  1
#define BMA023_WAKE_UP_PAUSE_320MS		2
#define BMA023_WAKE_UP_PAUSE_2560MS		3


/* LG/HG thresholds are in LSB and depend on RANGE setting */
/* no range check on threshold calculation */

#define BMA023_SELF_TEST0_ON		1
#define BMA023_SELF_TEST1_ON		2

#define BMA023_EE_W_OFF			0
#define BMA023_EE_W_ON			1



/* low-g, high-g, any_motion */


#define BMA023_COUNTER_LG_RST		0
#define BMA023_COUNTER_LG_0LSB		BMA023_COUNTER_LG_RST
#define BMA023_COUNTER_LG_1LSB		1
#define BMA023_COUNTER_LG_2LSB		2
#define BMA023_COUNTER_LG_3LSB		3

#define BMA023_COUNTER_HG_RST		0
#define BMA023_COUNTER_HG_0LSB		BMA023_COUNTER_HG_RST
#define BMA023_COUNTER_HG_1LSB		1
#define BMA023_COUNTER_HG_2LSB		2
#define BMA023_COUNTER_HG_3LSB		3

#define BMA023_COUNTER_RST			0
#define BMA023_COUNTER_0LSB			BMA023_COUNTER_RST
#define BMA023_COUNTER_1LSB			1
#define BMA023_COUNTER_2LSB			2
#define BMA023_COUNTER_3LSB			3



/** Macro to convert floating point low-g-thresholds in G to 8-bit register values.<br>
  * Example: BMA023_LG_THRES_IN_G( 0.3, 2.0) generates the register value for 0.3G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA023_LG_THRES_IN_G( gthres, range)			((256 * gthres ) / range)

/** Macro to convert floating point high-g-thresholds in G to 8-bit register values.<br>
  * Example: BMA023_HG_THRES_IN_G( 1.4, 2.0) generates the register value for 1.4G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA023_HG_THRES_IN_G(gthres, range)				((256 * gthres ) / range)

/** Macro to convert floating point low-g-hysteresis in G to 8-bit register values.<br>
  * Example: BMA023_LG_HYST_THRES_IN_G( 0.2, 2.0) generates the register value for 0.2G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA023_LG_HYST_IN_G( ghyst, range )				((32 * ghyst) / range)

/** Macro to convert floating point high-g-hysteresis in G to 8-bit register values.<br>
  * Example: BMA023_HG_HYST_THRES_IN_G( 0.2, 2.0) generates the register value for 0.2G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA023_HG_HYST_IN_G( ghyst, range )				((32 * ghyst) / range)


/** Macro to convert floating point G-thresholds to 8-bit register values<br>
  * Example: BMA023_ANY_MOTION_THRES_IN_G( 1.2, 2.0) generates the register value for 1.2G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */

#define BMA023_ANY_MOTION_THRES_IN_G( gthres, range)	((128 * gthres ) / range)


#define BMA023_ANY_MOTION_DUR_1		0
#define BMA023_ANY_MOTION_DUR_3		1
#define BMA023_ANY_MOTION_DUR_5		2
#define BMA023_ANY_MOTION_DUR_7		3



#define BMA023_SHADOW_DIS_OFF	0
#define BMA023_SHADOW_DIS_ON	1

#define BMA023_LATCH_INT_OFF	0
#define BMA023_LATCH_INT_ON		1

#define BMA023_NEW_DATA_INT_OFF	0
#define BMA023_NEW_DATA_INT_ON	1

#define BMA023_ENABLE_ADV_INT_OFF	0
#define BMA023_ENABLE_ADV_INT_ON	1

#define BMA023_EN_ANY_MOTION_OFF 	0
#define BMA023_EN_ANY_MOTION_ON 	1


#define BMA023_ALERT_OFF	0
#define BMA023_ALERT_ON		1

#define BMA023_ENABLE_LG_OFF	0
#define BMA023_ENABLE_LG_ON		1

#define BMA023_ENABLE_HG_OFF	0
#define BMA023_ENABLE_HG_ON		1



#define BMA023_INT_ALERT		(1<<7)
#define BMA023_INT_ANY_MOTION	(1<<6)
#define BMA023_INT_EN_ADV_INT	(1<<5)
#define BMA023_INT_NEW_DATA		(1<<4)
#define BMA023_INT_LATCH		(1<<3)
#define BMA023_INT_HG			(1<<1)
#define BMA023_INT_LG			(1<<0)


#define BMA023_INT_STATUS_HG			(1<<0)
#define BMA023_INT_STATUS_LG			(1<<1)
#define BMA023_INT_STATUS_HG_LATCHED	(1<<2)
#define BMA023_INT_STATUS_LG_LATCHED	(1<<3)
#define BMA023_INT_STATUS_ALERT			(1<<4)
#define BMA023_INT_STATUS_ST_RESULT		(1<<7)


#define BMA023_CONF1_INT_MSK	((1<<BMA023_ALERT__POS) | (1<<BMA023_EN_ANY_MOTION__POS) | (1<<BMA023_ENABLE_HG__POS) | (1<<BMA023_ENABLE_LG__POS))
#define BMA023_CONF2_INT_MSK	((1<<BMA023_ENABLE_ADV_INT__POS) | (1<<BMA023_NEW_DATA_INT__POS) | (1<<BMA023_LATCH_INT__POS))









/* Function prototypes */


/*

int bma023_init(bma150_t *);

int bma023_set_image (bma023regs_t *);

int bma023_get_image(bma023regs_t *);

int bma023_get_offset(unsigned char, unsigned short *); 

int bma023_set_offset(unsigned char, unsigned short ); 

int bma023_set_offset_eeprom(unsigned char, unsigned short);

int bma023_soft_reset(void); 

int bma023_update_image(void); 

int bma023_write_ee(unsigned char , unsigned char ) ;

int bma023_set_ee_w(unsigned char);

int bma023_get_ee_w(unsigned char*);

int bma023_selftest(unsigned char);

int bma023_get_selftest_result(unsigned char *);

int bma023_set_range(char); 

int bma023_get_range(unsigned char*);

int bma023_set_mode(unsigned char); 

int bma023_get_mode(unsigned char *);

int bma023_set_wake_up_pause(unsigned char);

int bma023_get_wake_up_pause(unsigned char *);

int bma023_set_bandwidth(char);

int bma023_get_bandwidth(unsigned char *);

int bma023_set_low_g_threshold(unsigned char);

int bma023_get_low_g_threshold(unsigned char*);

int bma023_set_low_g_hysteresis(unsigned char);

int bma023_set_low_g_countdown(unsigned char);

int bma023_get_low_g_countdown(unsigned char *);

int bma023_get_low_g_hysteresis(unsigned char*);

int bma023_set_low_g_duration(unsigned char);

int bma023_get_low_g_duration(unsigned char*);

int bma023_set_high_g_threshold(unsigned char);

int bma023_get_high_g_threshold(unsigned char*);

int bma023_set_high_g_hysteresis(unsigned char);

int bma023_set_high_g_countdown(unsigned char);

int bma023_get_high_g_countdown(unsigned char *);

int bma023_get_high_g_hysteresis(unsigned char*);

int bma023_set_high_g_duration(unsigned char);

int bma023_get_high_g_duration(unsigned char*);

int bma023_set_any_motion_threshold(unsigned char);

int bma023_get_any_motion_threshold(unsigned char*);

int bma023_set_any_motion_count(unsigned char);

int bma023_get_any_motion_count(unsigned char *);

int bma023_read_accel_x(short *);

int bma023_read_accel_y(short *);

int bma023_read_accel_z(short *);

int bma023_read_temperature(unsigned char*);

int bma023_read_accel_xyz(bma023acc_t *);

int bma023_get_interrupt_status(unsigned char *);

int bma023_reset_interrupt(void);

int bma023_set_interrupt_mask(unsigned char);

int bma023_get_interrupt_mask(unsigned char *);

int bma023_set_low_g_int(unsigned char);

int bma023_set_high_g_int(unsigned char);

int bma023_set_any_motion_int(unsigned char);

int bma023_set_alert_int(unsigned char);

int bma023_set_advanced_int(unsigned char);

int bma023_latch_int(unsigned char);

int bma023_set_new_data_int(unsigned char onoff);

int bma023_pause(int);

int bma023_read_reg(unsigned char , unsigned char *, unsigned char);

int bma023_write_reg(unsigned char , unsigned char*, unsigned char );

*/


#endif   // __BMA023_H__





