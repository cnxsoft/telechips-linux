/*  $Date: 2009/10/23 $
 *  $Revision: 1.2 $
 */

/** \mainpage BMA250 Acceleration Sensor API
 * Copyright (C) 2009 Bosch Sensortec GmbH
 *  \section intro_sec Introduction
 * BMA250 3-axis digital Accelerometer Programming Interface
 * The BMA250 API enables quick access to Bosch Sensortec's 3-Axis digital accelerometer.
 * The only mandatory steps are: 
 *
 * 1. linking the target application's communication functions to the API (\ref BMA250_WR_FUNC_PTR, \ref BMA250_RD_FUNC_PTR)
 *
 * 2. calling the bma250_init() routine, which initializes all necessary data structures for using all functions
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

 /** \file bma250.h
    \brief Header file for all #define constants and function prototypes
  
    
*/



#ifndef __BMA250_H__
#define __BMA250_H__



/* BMA250 Macro for read and write commincation */


/**
   define for used read and write macros 
*/


/** Define the calling convention of YOUR bus communication routine.
	\note This includes types of parameters. This example shows the configuration for an SPI bus link.

	If your communication function looks like this:

	write_my_bus_xy(unsigned char device_addr, unsigned char register_addr, unsigned char * data, unsigned char length);

	The BMA250_WR_FUNC_PTR would equal:

    #define	BMA250_WR_FUNC_PTR char (* bus_write)(unsigned char, unsigned char, unsigned char *, unsigned char)
	
	Parameters can be mixed as needed refer to the \ref BMA250_BUS_WRITE_FUNC  macro.
	

*/
#define BMA250_WR_FUNC_PTR char (* bus_write)(unsigned char, unsigned char, unsigned char *, unsigned char)



/** link makro between API function calls and bus write function
	\note The bus write function can change since this is a system dependant issue.

	If the bus_write parameter calling order is like: reg_addr, reg_data, wr_len it would be as it is here.

	If the parameters are differently ordered or your communication function like I2C need to know the device address, 
	you can change this macro accordingly.


	define BMA250_BUS_WRITE_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
           bus_write(dev_addr, reg_addr, reg_data, wr_len)

    This macro lets all API functions call YOUR communication routine in a way that equals your definition in the \ref BMA250_WR_FUNC_PTR definition.


	      
*/
#define BMA250_BUS_WRITE_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
           bus_write(dev_addr, reg_addr, reg_data, wr_len)


/** Define the calling convention of YOUR bus communication routine.
	\note This includes types of parameters. This example shows the configuration for an SPI bus link.

	If your communication function looks like this:

	read_my_bus_xy(unsigned char device_addr, unsigned char register_addr, unsigned char * data, unsigned char length);

	The BMA250_RD_FUNC_PTR would equal:

    #define	BMA250_RD_FUNC_PTR char (* bus_read)(unsigned char, unsigned char, unsigned char *, unsigned char)
	
	Parameters can be mixed as needed refer to the \ref BMA250_BUS_READ_FUNC  macro.
	

*/

#define BMA250_SPI_RD_MASK 0x80   /* for spi read transactions on SPI the MSB has to be set */
#define BMA250_RD_FUNC_PTR char (* bus_read)(unsigned char, unsigned char, unsigned char *, unsigned char)


/** link makro between API function calls and bus read function
	\note The bus write function can change since this is a system dependant issue.

	If the bus_read parameter calling order is like: reg_addr, reg_data, wr_len it would be as it is here.

	If the parameters are differently ordered or your communication function like I2C need to know the device address, 
	you can change this macro accordingly.


	define BMA250_BUS_READ_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
           bus_read(dev_addr, reg_addr, reg_data, wr_len)

    This macro lets all API functions call YOUR communication routine in a way that equals your definition in the \ref BMA250_WR_FUNC_PTR definition.
*/

#define BMA250_BUS_READ_FUNC(dev_addr, reg_addr, reg_data, r_len)\
           bus_read(dev_addr, reg_addr, reg_data, r_len)







/** BMA250 I2C Address
*/

#define BMA250_I2C_ADDR		0x18


/* 
 *	
 *	register definitions 	
 *
 */

//#define ABSMIN				-512
//#define ABSMAX				512
#define SLOPE_THRESHOLD_VALUE 		32
#define SLOPE_DURATION_VALUE 		1
#define INTERRUPT_LATCH_MODE 		13
#define INTERRUPT_ENABLE 		1
#define INTERRUPT_DISABLE 		0
#define MAP_SLOPE_INTERRUPT 		2
#define SLOPE_X_INDEX 			5
#define SLOPE_Y_INDEX 			6
#define SLOPE_Z_INDEX 			7
#define BMA250_MAX_DELAY		200
#define BMA250_CHIP_ID			3
#define BMA250_RANGE_SET		0
#define BMA250_BW_SET			1
#define BMA250_SW_RESET_VALUE	0xB6

#define PAD_LOWG					0
#define PAD_HIGHG					1
#define PAD_SLOP					2
#define PAD_DOUBLE_TAP					3
#define PAD_SINGLE_TAP					4
#define PAD_ORIENT					5
#define PAD_FLAT					6

#define BMA250_CHIP_ID_REG                      0x00
#define BMA250_VERSION_REG                      0x01
#define BMA250_X_AXIS_LSB_REG                   0x02
#define BMA250_X_AXIS_MSB_REG                   0x03
#define BMA250_Y_AXIS_LSB_REG                   0x04
#define BMA250_Y_AXIS_MSB_REG                   0x05
#define BMA250_Z_AXIS_LSB_REG                   0x06
#define BMA250_Z_AXIS_MSB_REG                   0x07
#define BMA250_TEMP_RD_REG                      0x08
#define BMA250_STATUS1_REG                      0x09
#define BMA250_STATUS2_REG                      0x0A
#define BMA250_STATUS_TAP_SLOPE_REG             0x0B
#define BMA250_STATUS_ORIENT_HIGH_REG           0x0C
#define BMA250_RANGE_SEL_REG                    0x0F
#define BMA250_BW_SEL_REG                       0x10
#define BMA250_MODE_CTRL_REG                    0x11
#define BMA250_LOW_NOISE_CTRL_REG               0x12
#define BMA250_DATA_CTRL_REG                    0x13
#define BMA250_RESET_REG                        0x14
#define BMA250_INT_ENABLE1_REG                  0x16
#define BMA250_INT_ENABLE2_REG                  0x17
#define BMA250_INT1_PAD_SEL_REG                 0x19
#define BMA250_INT_DATA_SEL_REG                 0x1A
#define BMA250_INT2_PAD_SEL_REG                 0x1B
#define BMA250_INT_SRC_REG                      0x1E
#define BMA250_INT_SET_REG                      0x20
#define BMA250_INT_CTRL_REG                     0x21
#define BMA250_LOW_DURN_REG                     0x22
#define BMA250_LOW_THRES_REG                    0x23
#define BMA250_LOW_HIGH_HYST_REG                0x24
#define BMA250_HIGH_DURN_REG                    0x25
#define BMA250_HIGH_THRES_REG                   0x26
#define BMA250_SLOPE_DURN_REG                   0x27
#define BMA250_SLOPE_THRES_REG                  0x28
#define BMA250_TAP_PARAM_REG                    0x2A
#define BMA250_TAP_THRES_REG                    0x2B
#define BMA250_ORIENT_PARAM_REG                 0x2C
#define BMA250_THETA_BLOCK_REG                  0x2D
#define BMA250_THETA_FLAT_REG                   0x2E
#define BMA250_FLAT_HOLD_TIME_REG               0x2F
#define BMA250_STATUS_LOW_POWER_REG             0x31
#define BMA250_SELF_TEST_REG                    0x32
#define BMA250_EEPROM_CTRL_REG                  0x33
#define BMA250_SERIAL_CTRL_REG                  0x34
#define BMA250_CTRL_UNLOCK_REG                  0x35
#define BMA250_OFFSET_CTRL_REG                  0x36
#define BMA250_OFFSET_PARAMS_REG                0x37
#define BMA250_OFFSET_FILT_X_REG                0x38
#define BMA250_OFFSET_FILT_Y_REG                0x39
#define BMA250_OFFSET_FILT_Z_REG                0x3A
#define BMA250_OFFSET_UNFILT_X_REG              0x3B
#define BMA250_OFFSET_UNFILT_Y_REG              0x3C
#define BMA250_OFFSET_UNFILT_Z_REG              0x3D
#define BMA250_SPARE_0_REG                      0x3E
#define BMA250_SPARE_1_REG                      0x3F


/* register write and read delays */

#define BMA250_MDELAY_DATA_TYPE	unsigned int
#define BMA250_EE_W_DELAY 28	/* delay after EEP write is 28 msec */



/** BMA250 acceleration data 
	\brief Structure containing acceleration values for x,y and z-axis in signed short

*/

typedef struct  {
		short x, /**< holds x-axis acceleration data sign extended. Range -512 to 511. */
			  y, /**< holds y-axis acceleration data sign extended. Range -512 to 511. */
			  z; /**< holds z-axis acceleration data sign extended. Range -512 to 511. */
} bma250acc_t;


/** BMA250 image registers data structure
	\brief Register type that contains all BMA250 image registers from address 0x0b to 0x15
	This structure can hold the complete image data of BMA250

*/
typedef struct  {
		unsigned char	
		bma250_conf1 ,  /**<  image address 0x0b: interrupt enable bits, low-g settings */
		lg_threshold,	/**<  image address 0x0c: low-g threshold, depends on selected g-range */
		lg_duration,	/**<  image address 0x0d: low-g duration in ms */
		hg_threshold,	/**<  image address 0x0e: high-g threshold, depends on selected g-range */
		hg_duration,	/**<  image address 0x0f: high-g duration in ms */
		motion_thrs,	/**<  image address 0x10: any motion threshold */
		hysteresis,		/**<  image address 0x11: low-g and high-g hysteresis register */
		customer1,		/**<  image address 0x12: customer reserved register 1 */
		customer2,		/**<  image address 0x13: customer reserved register 2  */
		range_bwidth,	/**<  image address 0x14: range and bandwidth selection register */
		bma250_conf2,	/**<  image address 0x15: spi4, latched interrupt, auto-wake-up configuration */
		offs_gain_x,	/**<  image address 0x16: offset_x LSB and x-axis gain settings */
		offs_gain_y,	/**<  image address 0x17: offset_y LSB and y-axis gain settings */
		offs_gain_z,	/**<  image address 0x18: offset_z LSB and z-axis gain settings */
		offs_gain_t,	/**<  image address 0x19: offset_t LSB and temperature gain settings */
		offset_x,		/**<  image address 0x1a: offset_x calibration MSB register */
		offset_y,		/**<  image address 0x1b: offset_y calibration MSB register */ 
		offset_z,		/**<  image address 0x1c: offset_z calibration MSB register */ 
		offset_t;		/**<  image address 0x1d: temperature calibration MSB register */ 
} bma250regs_t;


/** bma250 typedef structure
	\brief This structure holds all relevant information about BMA250 and links communication to the 
*/

typedef struct {	
	bma250regs_t * image;	/**< pointer to bma250regs_t structure not mandatory */
	unsigned char mode;		/**< save current BMA250 operation mode */
	unsigned char chip_id;	/**< save BMA250's chip id which has to be 0x02 after calling bma250_init() */
	unsigned char dev_addr;   /**< initializes BMA250's I2C device address 0x38 */
	unsigned char int_mask;	  /**< stores the current BMA250 API generated interrupt mask */
	BMA250_WR_FUNC_PTR;		  /**< function pointer to the SPI/I2C write function */
	BMA250_RD_FUNC_PTR;		  /**< function pointer to the SPI/I2C read function */
	void (*delay_msec)( BMA250_MDELAY_DATA_TYPE ); /**< function pointer to a pause in mili seconds function */
} bma250_t;




	
/* 
 *	
 *	bit slice positions in registers
 *
 */

/** \cond BITSLICE */

#define BMA250_CHIP_ID__POS		0
#define BMA250_CHIP_ID__MSK		0xFF
#define BMA250_CHIP_ID__LEN		8
#define BMA250_CHIP_ID__REG		BMA250_CHIP_ID_REG


/* DATA REGISTERS */

#define BMA250_ACC_X_LSB__POS           6
#define BMA250_ACC_X_LSB__LEN           2
#define BMA250_ACC_X_LSB__MSK           0xC0
#define BMA250_ACC_X_LSB__REG           BMA250_X_AXIS_LSB_REG

#define BMA250_ACC_X_MSB__POS           0
#define BMA250_ACC_X_MSB__LEN           8
#define BMA250_ACC_X_MSB__MSK           0xFF
#define BMA250_ACC_X_MSB__REG           BMA250_X_AXIS_MSB_REG

#define BMA250_ACC_Y_LSB__POS           6
#define BMA250_ACC_Y_LSB__LEN           2
#define BMA250_ACC_Y_LSB__MSK           0xC0
#define BMA250_ACC_Y_LSB__REG           BMA250_Y_AXIS_LSB_REG

#define BMA250_ACC_Y_MSB__POS           0
#define BMA250_ACC_Y_MSB__LEN           8
#define BMA250_ACC_Y_MSB__MSK           0xFF
#define BMA250_ACC_Y_MSB__REG           BMA250_Y_AXIS_MSB_REG

#define BMA250_ACC_Z_LSB__POS           6
#define BMA250_ACC_Z_LSB__LEN           2
#define BMA250_ACC_Z_LSB__MSK           0xC0
#define BMA250_ACC_Z_LSB__REG           BMA250_Z_AXIS_LSB_REG

#define BMA250_ACC_Z_MSB__POS           0
#define BMA250_ACC_Z_MSB__LEN           8
#define BMA250_ACC_Z_MSB__MSK           0xFF
#define BMA250_ACC_Z_MSB__REG           BMA250_Z_AXIS_MSB_REG

#define BMA250_RANGE_SEL__POS             0
#define BMA250_RANGE_SEL__LEN             4
#define BMA250_RANGE_SEL__MSK             0x0F
#define BMA250_RANGE_SEL__REG             BMA250_RANGE_SEL_REG

#define BMA250_BANDWIDTH__POS             0
#define BMA250_BANDWIDTH__LEN             5
#define BMA250_BANDWIDTH__MSK             0x1F
#define BMA250_BANDWIDTH__REG             BMA250_BW_SEL_REG

#define BMA250_SLEEP_DUR__POS          1
#define BMA250_SLEEP_DUR__LEN          4
#define BMA250_SLEEP_DUR__MSK          0x1E
#define BMA250_SLEEP_DUR__REG          BMA250_MODE_CTRL_REG

#define BMA250_EN_LOW_POWER__POS          6
#define BMA250_EN_LOW_POWER__LEN          1
#define BMA250_EN_LOW_POWER__MSK          0x40
#define BMA250_EN_LOW_POWER__REG          BMA250_MODE_CTRL_REG

#define BMA250_EN_SUSPEND__POS            7
#define BMA250_EN_SUSPEND__LEN            1
#define BMA250_EN_SUSPEND__MSK            0x80
#define BMA250_EN_SUSPEND__REG            BMA250_MODE_CTRL_REG

#define BMA250_INT_MODE_SEL__POS                0
#define BMA250_INT_MODE_SEL__LEN                4
#define BMA250_INT_MODE_SEL__MSK                0x0F
#define BMA250_INT_MODE_SEL__REG                BMA250_INT_CTRL_REG

#define BMA250_LOWG_INT_S__POS          0
#define BMA250_LOWG_INT_S__LEN          1
#define BMA250_LOWG_INT_S__MSK          0x01
#define BMA250_LOWG_INT_S__REG          BMA250_STATUS1_REG

#define BMA250_HIGHG_INT_S__POS          1
#define BMA250_HIGHG_INT_S__LEN          1
#define BMA250_HIGHG_INT_S__MSK          0x02
#define BMA250_HIGHG_INT_S__REG          BMA250_STATUS1_REG

#define BMA250_SLOPE_INT_S__POS          2
#define BMA250_SLOPE_INT_S__LEN          1
#define BMA250_SLOPE_INT_S__MSK          0x04
#define BMA250_SLOPE_INT_S__REG          BMA250_STATUS1_REG

#define BMA250_DOUBLE_TAP_INT_S__POS     4
#define BMA250_DOUBLE_TAP_INT_S__LEN     1
#define BMA250_DOUBLE_TAP_INT_S__MSK     0x10
#define BMA250_DOUBLE_TAP_INT_S__REG     BMA250_STATUS1_REG

#define BMA250_SINGLE_TAP_INT_S__POS     5
#define BMA250_SINGLE_TAP_INT_S__LEN     1
#define BMA250_SINGLE_TAP_INT_S__MSK     0x20
#define BMA250_SINGLE_TAP_INT_S__REG     BMA250_STATUS1_REG

#define BMA250_ORIENT_INT_S__POS         6
#define BMA250_ORIENT_INT_S__LEN         1
#define BMA250_ORIENT_INT_S__MSK         0x40
#define BMA250_ORIENT_INT_S__REG         BMA250_STATUS1_REG

#define BMA250_FLAT_INT_S__POS           7
#define BMA250_FLAT_INT_S__LEN           1
#define BMA250_FLAT_INT_S__MSK           0x80
#define BMA250_FLAT_INT_S__REG           BMA250_STATUS1_REG

#define BMA250_DATA_INT_S__POS           7
#define BMA250_DATA_INT_S__LEN           1
#define BMA250_DATA_INT_S__MSK           0x80
#define BMA250_DATA_INT_S__REG           BMA250_STATUS2_REG

#define BMA250_SLOPE_FIRST_X__POS        0
#define BMA250_SLOPE_FIRST_X__LEN        1
#define BMA250_SLOPE_FIRST_X__MSK        0x01
#define BMA250_SLOPE_FIRST_X__REG        BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_SLOPE_FIRST_Y__POS        1
#define BMA250_SLOPE_FIRST_Y__LEN        1
#define BMA250_SLOPE_FIRST_Y__MSK        0x02
#define BMA250_SLOPE_FIRST_Y__REG        BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_SLOPE_FIRST_Z__POS        2
#define BMA250_SLOPE_FIRST_Z__LEN        1
#define BMA250_SLOPE_FIRST_Z__MSK        0x04
#define BMA250_SLOPE_FIRST_Z__REG        BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_SLOPE_SIGN_S__POS         3
#define BMA250_SLOPE_SIGN_S__LEN         1
#define BMA250_SLOPE_SIGN_S__MSK         0x08
#define BMA250_SLOPE_SIGN_S__REG         BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_TAP_FIRST_X__POS        4
#define BMA250_TAP_FIRST_X__LEN        1
#define BMA250_TAP_FIRST_X__MSK        0x10
#define BMA250_TAP_FIRST_X__REG        BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_TAP_FIRST_Y__POS        5
#define BMA250_TAP_FIRST_Y__LEN        1
#define BMA250_TAP_FIRST_Y__MSK        0x20
#define BMA250_TAP_FIRST_Y__REG        BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_TAP_FIRST_Z__POS        6
#define BMA250_TAP_FIRST_Z__LEN        1
#define BMA250_TAP_FIRST_Z__MSK        0x40
#define BMA250_TAP_FIRST_Z__REG        BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_TAP_FIRST_XYZ__POS        4
#define BMA250_TAP_FIRST_XYZ__LEN        3
#define BMA250_TAP_FIRST_XYZ__MSK        0x70
#define BMA250_TAP_FIRST_XYZ__REG        BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_TAP_SIGN_S__POS         7
#define BMA250_TAP_SIGN_S__LEN         1
#define BMA250_TAP_SIGN_S__MSK         0x80
#define BMA250_TAP_SIGN_S__REG         BMA250_STATUS_TAP_SLOPE_REG

#define BMA250_HIGHG_FIRST_X__POS        0
#define BMA250_HIGHG_FIRST_X__LEN        1
#define BMA250_HIGHG_FIRST_X__MSK        0x01
#define BMA250_HIGHG_FIRST_X__REG        BMA250_STATUS_ORIENT_HIGH_REG

#define BMA250_HIGHG_FIRST_Y__POS        1
#define BMA250_HIGHG_FIRST_Y__LEN        1
#define BMA250_HIGHG_FIRST_Y__MSK        0x02
#define BMA250_HIGHG_FIRST_Y__REG        BMA250_STATUS_ORIENT_HIGH_REG

#define BMA250_HIGHG_FIRST_Z__POS        2
#define BMA250_HIGHG_FIRST_Z__LEN        1
#define BMA250_HIGHG_FIRST_Z__MSK        0x04
#define BMA250_HIGHG_FIRST_Z__REG        BMA250_STATUS_ORIENT_HIGH_REG

#define BMA250_HIGHG_SIGN_S__POS         3
#define BMA250_HIGHG_SIGN_S__LEN         1
#define BMA250_HIGHG_SIGN_S__MSK         0x08
#define BMA250_HIGHG_SIGN_S__REG         BMA250_STATUS_ORIENT_HIGH_REG

#define BMA250_ORIENT_S__POS             4
#define BMA250_ORIENT_S__LEN             3
#define BMA250_ORIENT_S__MSK             0x70
#define BMA250_ORIENT_S__REG             BMA250_STATUS_ORIENT_HIGH_REG

#define BMA250_FLAT_S__POS               7
#define BMA250_FLAT_S__LEN               1
#define BMA250_FLAT_S__MSK               0x80
#define BMA250_FLAT_S__REG               BMA250_STATUS_ORIENT_HIGH_REG

#define BMA250_EN_SLOPE_X_INT__POS         0
#define BMA250_EN_SLOPE_X_INT__LEN         1
#define BMA250_EN_SLOPE_X_INT__MSK         0x01
#define BMA250_EN_SLOPE_X_INT__REG         BMA250_INT_ENABLE1_REG

#define BMA250_EN_SLOPE_Y_INT__POS         1
#define BMA250_EN_SLOPE_Y_INT__LEN         1
#define BMA250_EN_SLOPE_Y_INT__MSK         0x02
#define BMA250_EN_SLOPE_Y_INT__REG         BMA250_INT_ENABLE1_REG

#define BMA250_EN_SLOPE_Z_INT__POS         2
#define BMA250_EN_SLOPE_Z_INT__LEN         1
#define BMA250_EN_SLOPE_Z_INT__MSK         0x04
#define BMA250_EN_SLOPE_Z_INT__REG         BMA250_INT_ENABLE1_REG

#define BMA250_EN_SLOPE_XYZ_INT__POS         0
#define BMA250_EN_SLOPE_XYZ_INT__LEN         3
#define BMA250_EN_SLOPE_XYZ_INT__MSK         0x07
#define BMA250_EN_SLOPE_XYZ_INT__REG         BMA250_INT_ENABLE1_REG

#define BMA250_EN_DOUBLE_TAP_INT__POS      4
#define BMA250_EN_DOUBLE_TAP_INT__LEN      1
#define BMA250_EN_DOUBLE_TAP_INT__MSK      0x10
#define BMA250_EN_DOUBLE_TAP_INT__REG      BMA250_INT_ENABLE1_REG

#define BMA250_EN_SINGLE_TAP_INT__POS      5
#define BMA250_EN_SINGLE_TAP_INT__LEN      1
#define BMA250_EN_SINGLE_TAP_INT__MSK      0x20
#define BMA250_EN_SINGLE_TAP_INT__REG      BMA250_INT_ENABLE1_REG

#define BMA250_EN_ORIENT_INT__POS          6
#define BMA250_EN_ORIENT_INT__LEN          1
#define BMA250_EN_ORIENT_INT__MSK          0x40
#define BMA250_EN_ORIENT_INT__REG          BMA250_INT_ENABLE1_REG

#define BMA250_EN_FLAT_INT__POS            7
#define BMA250_EN_FLAT_INT__LEN            1
#define BMA250_EN_FLAT_INT__MSK            0x80
#define BMA250_EN_FLAT_INT__REG            BMA250_INT_ENABLE1_REG

#define BMA250_EN_HIGHG_X_INT__POS         0
#define BMA250_EN_HIGHG_X_INT__LEN         1
#define BMA250_EN_HIGHG_X_INT__MSK         0x01
#define BMA250_EN_HIGHG_X_INT__REG         BMA250_INT_ENABLE2_REG

#define BMA250_EN_HIGHG_Y_INT__POS         1
#define BMA250_EN_HIGHG_Y_INT__LEN         1
#define BMA250_EN_HIGHG_Y_INT__MSK         0x02
#define BMA250_EN_HIGHG_Y_INT__REG         BMA250_INT_ENABLE2_REG

#define BMA250_EN_HIGHG_Z_INT__POS         2
#define BMA250_EN_HIGHG_Z_INT__LEN         1
#define BMA250_EN_HIGHG_Z_INT__MSK         0x04
#define BMA250_EN_HIGHG_Z_INT__REG         BMA250_INT_ENABLE2_REG

#define BMA250_EN_HIGHG_XYZ_INT__POS         2
#define BMA250_EN_HIGHG_XYZ_INT__LEN         1
#define BMA250_EN_HIGHG_XYZ_INT__MSK         0x04
#define BMA250_EN_HIGHG_XYZ_INT__REG         BMA250_INT_ENABLE2_REG

#define BMA250_EN_LOWG_INT__POS            3
#define BMA250_EN_LOWG_INT__LEN            1
#define BMA250_EN_LOWG_INT__MSK            0x08
#define BMA250_EN_LOWG_INT__REG            BMA250_INT_ENABLE2_REG

#define BMA250_EN_NEW_DATA_INT__POS        4
#define BMA250_EN_NEW_DATA_INT__LEN        1
#define BMA250_EN_NEW_DATA_INT__MSK        0x10
#define BMA250_EN_NEW_DATA_INT__REG        BMA250_INT_ENABLE2_REG

#define BMA250_EN_INT1_PAD_LOWG__POS        0
#define BMA250_EN_INT1_PAD_LOWG__LEN        1
#define BMA250_EN_INT1_PAD_LOWG__MSK        0x01
#define BMA250_EN_INT1_PAD_LOWG__REG        BMA250_INT1_PAD_SEL_REG

#define BMA250_EN_INT1_PAD_HIGHG__POS       1
#define BMA250_EN_INT1_PAD_HIGHG__LEN       1
#define BMA250_EN_INT1_PAD_HIGHG__MSK       0x02
#define BMA250_EN_INT1_PAD_HIGHG__REG       BMA250_INT1_PAD_SEL_REG

#define BMA250_EN_INT1_PAD_SLOPE__POS       2
#define BMA250_EN_INT1_PAD_SLOPE__LEN       1
#define BMA250_EN_INT1_PAD_SLOPE__MSK       0x04
#define BMA250_EN_INT1_PAD_SLOPE__REG       BMA250_INT1_PAD_SEL_REG

#define BMA250_EN_INT1_PAD_DB_TAP__POS      4
#define BMA250_EN_INT1_PAD_DB_TAP__LEN      1
#define BMA250_EN_INT1_PAD_DB_TAP__MSK      0x10
#define BMA250_EN_INT1_PAD_DB_TAP__REG      BMA250_INT1_PAD_SEL_REG

#define BMA250_EN_INT1_PAD_SNG_TAP__POS     5
#define BMA250_EN_INT1_PAD_SNG_TAP__LEN     1
#define BMA250_EN_INT1_PAD_SNG_TAP__MSK     0x20
#define BMA250_EN_INT1_PAD_SNG_TAP__REG     BMA250_INT1_PAD_SEL_REG

#define BMA250_EN_INT1_PAD_ORIENT__POS      6
#define BMA250_EN_INT1_PAD_ORIENT__LEN      1
#define BMA250_EN_INT1_PAD_ORIENT__MSK      0x40
#define BMA250_EN_INT1_PAD_ORIENT__REG      BMA250_INT1_PAD_SEL_REG

#define BMA250_EN_INT1_PAD_FLAT__POS        7
#define BMA250_EN_INT1_PAD_FLAT__LEN        1
#define BMA250_EN_INT1_PAD_FLAT__MSK        0x80
#define BMA250_EN_INT1_PAD_FLAT__REG        BMA250_INT1_PAD_SEL_REG

#define BMA250_EN_INT2_PAD_LOWG__POS        0
#define BMA250_EN_INT2_PAD_LOWG__LEN        1
#define BMA250_EN_INT2_PAD_LOWG__MSK        0x01
#define BMA250_EN_INT2_PAD_LOWG__REG        BMA250_INT2_PAD_SEL_REG

#define BMA250_EN_INT2_PAD_HIGHG__POS       1
#define BMA250_EN_INT2_PAD_HIGHG__LEN       1
#define BMA250_EN_INT2_PAD_HIGHG__MSK       0x02
#define BMA250_EN_INT2_PAD_HIGHG__REG       BMA250_INT2_PAD_SEL_REG

#define BMA250_EN_INT2_PAD_SLOPE__POS       2
#define BMA250_EN_INT2_PAD_SLOPE__LEN       1
#define BMA250_EN_INT2_PAD_SLOPE__MSK       0x04
#define BMA250_EN_INT2_PAD_SLOPE__REG       BMA250_INT2_PAD_SEL_REG

#define BMA250_EN_INT2_PAD_DB_TAP__POS      4
#define BMA250_EN_INT2_PAD_DB_TAP__LEN      1
#define BMA250_EN_INT2_PAD_DB_TAP__MSK      0x10
#define BMA250_EN_INT2_PAD_DB_TAP__REG      BMA250_INT2_PAD_SEL_REG

#define BMA250_EN_INT2_PAD_SNG_TAP__POS     5
#define BMA250_EN_INT2_PAD_SNG_TAP__LEN     1
#define BMA250_EN_INT2_PAD_SNG_TAP__MSK     0x20
#define BMA250_EN_INT2_PAD_SNG_TAP__REG     BMA250_INT2_PAD_SEL_REG

#define BMA250_EN_INT2_PAD_ORIENT__POS      6
#define BMA250_EN_INT2_PAD_ORIENT__LEN      1
#define BMA250_EN_INT2_PAD_ORIENT__MSK      0x40
#define BMA250_EN_INT2_PAD_ORIENT__REG      BMA250_INT2_PAD_SEL_REG

#define BMA250_EN_INT2_PAD_FLAT__POS        7
#define BMA250_EN_INT2_PAD_FLAT__LEN        1
#define BMA250_EN_INT2_PAD_FLAT__MSK        0x80
#define BMA250_EN_INT2_PAD_FLAT__REG        BMA250_INT2_PAD_SEL_REG

#define BMA250_EN_INT1_PAD_NEWDATA__POS     0
#define BMA250_EN_INT1_PAD_NEWDATA__LEN     1
#define BMA250_EN_INT1_PAD_NEWDATA__MSK     0x01
#define BMA250_EN_INT1_PAD_NEWDATA__REG     BMA250_INT_DATA_SEL_REG

#define BMA250_EN_INT2_PAD_NEWDATA__POS     7
#define BMA250_EN_INT2_PAD_NEWDATA__LEN     1
#define BMA250_EN_INT2_PAD_NEWDATA__MSK     0x80
#define BMA250_EN_INT2_PAD_NEWDATA__REG     BMA250_INT_DATA_SEL_REG


#define BMA250_UNFILT_INT_SRC_LOWG__POS        0
#define BMA250_UNFILT_INT_SRC_LOWG__LEN        1
#define BMA250_UNFILT_INT_SRC_LOWG__MSK        0x01
#define BMA250_UNFILT_INT_SRC_LOWG__REG        BMA250_INT_SRC_REG

#define BMA250_UNFILT_INT_SRC_HIGHG__POS       1
#define BMA250_UNFILT_INT_SRC_HIGHG__LEN       1
#define BMA250_UNFILT_INT_SRC_HIGHG__MSK       0x02
#define BMA250_UNFILT_INT_SRC_HIGHG__REG       BMA250_INT_SRC_REG

#define BMA250_UNFILT_INT_SRC_SLOPE__POS       2
#define BMA250_UNFILT_INT_SRC_SLOPE__LEN       1
#define BMA250_UNFILT_INT_SRC_SLOPE__MSK       0x04
#define BMA250_UNFILT_INT_SRC_SLOPE__REG       BMA250_INT_SRC_REG

#define BMA250_UNFILT_INT_SRC_TAP__POS         4
#define BMA250_UNFILT_INT_SRC_TAP__LEN         1
#define BMA250_UNFILT_INT_SRC_TAP__MSK         0x10
#define BMA250_UNFILT_INT_SRC_TAP__REG         BMA250_INT_SRC_REG

#define BMA250_UNFILT_INT_SRC_DATA__POS        5
#define BMA250_UNFILT_INT_SRC_DATA__LEN        1
#define BMA250_UNFILT_INT_SRC_DATA__MSK        0x20
#define BMA250_UNFILT_INT_SRC_DATA__REG        BMA250_INT_SRC_REG

#define BMA250_INT1_PAD_ACTIVE_LEVEL__POS       0
#define BMA250_INT1_PAD_ACTIVE_LEVEL__LEN       1
#define BMA250_INT1_PAD_ACTIVE_LEVEL__MSK       0x01
#define BMA250_INT1_PAD_ACTIVE_LEVEL__REG       BMA250_INT_SET_REG

#define BMA250_INT2_PAD_ACTIVE_LEVEL__POS       2
#define BMA250_INT2_PAD_ACTIVE_LEVEL__LEN       1
#define BMA250_INT2_PAD_ACTIVE_LEVEL__MSK       0x04
#define BMA250_INT2_PAD_ACTIVE_LEVEL__REG       BMA250_INT_SET_REG

#define BMA250_INT1_PAD_OUTPUT_TYPE__POS        1
#define BMA250_INT1_PAD_OUTPUT_TYPE__LEN        1
#define BMA250_INT1_PAD_OUTPUT_TYPE__MSK        0x02
#define BMA250_INT1_PAD_OUTPUT_TYPE__REG        BMA250_INT_SET_REG

#define BMA250_INT2_PAD_OUTPUT_TYPE__POS        3
#define BMA250_INT2_PAD_OUTPUT_TYPE__LEN        1
#define BMA250_INT2_PAD_OUTPUT_TYPE__MSK        0x08
#define BMA250_INT2_PAD_OUTPUT_TYPE__REG        BMA250_INT_SET_REG


#define BMA250_INT_MODE_SEL__POS                0
#define BMA250_INT_MODE_SEL__LEN                4
#define BMA250_INT_MODE_SEL__MSK                0x0F
#define BMA250_INT_MODE_SEL__REG                BMA250_INT_CTRL_REG


#define BMA250_INT_RESET_LATCHED__POS           7
#define BMA250_INT_RESET_LATCHED__LEN           1
#define BMA250_INT_RESET_LATCHED__MSK           0x80
#define BMA250_INT_RESET_LATCHED__REG           BMA250_INT_CTRL_REG

#define BMA250_LOWG_DUR__POS                    0
#define BMA250_LOWG_DUR__LEN                    8
#define BMA250_LOWG_DUR__MSK                    0xFF
#define BMA250_LOWG_DUR__REG                    BMA250_LOW_DURN_REG

#define BMA250_LOWG_THRES__POS                  0
#define BMA250_LOWG_THRES__LEN                  8
#define BMA250_LOWG_THRES__MSK                  0xFF
#define BMA250_LOWG_THRES__REG                  BMA250_LOW_THRES_REG

#define BMA250_LOWG_HYST__POS                   0
#define BMA250_LOWG_HYST__LEN                   2
#define BMA250_LOWG_HYST__MSK                   0x03
#define BMA250_LOWG_HYST__REG                   BMA250_LOW_HIGH_HYST_REG

#define BMA250_LOWG_INT_MODE__POS               2
#define BMA250_LOWG_INT_MODE__LEN               1
#define BMA250_LOWG_INT_MODE__MSK               0x04
#define BMA250_LOWG_INT_MODE__REG               BMA250_LOW_HIGH_HYST_REG

#define BMA250_HIGHG_DUR__POS                    0
#define BMA250_HIGHG_DUR__LEN                    8
#define BMA250_HIGHG_DUR__MSK                    0xFF
#define BMA250_HIGHG_DUR__REG                    BMA250_HIGH_DURN_REG

#define BMA250_HIGHG_THRES__POS                  0
#define BMA250_HIGHG_THRES__LEN                  8
#define BMA250_HIGHG_THRES__MSK                  0xFF
#define BMA250_HIGHG_THRES__REG                  BMA250_HIGH_THRES_REG

#define BMA250_HIGHG_HYST__POS                  6
#define BMA250_HIGHG_HYST__LEN                  2
#define BMA250_HIGHG_HYST__MSK                  0xC0
#define BMA250_HIGHG_HYST__REG                  BMA250_LOW_HIGH_HYST_REG

#define BMA250_SLOPE_DUR__POS                    0
#define BMA250_SLOPE_DUR__LEN                    2
#define BMA250_SLOPE_DUR__MSK                    0x03
#define BMA250_SLOPE_DUR__REG                    BMA250_SLOPE_DURN_REG

#define BMA250_SLOPE_THRES__POS                  0
#define BMA250_SLOPE_THRES__LEN                  8
#define BMA250_SLOPE_THRES__MSK                  0xFF
#define BMA250_SLOPE_THRES__REG                  BMA250_SLOPE_THRES_REG

#define BMA250_TAP_DUR__POS                    0
#define BMA250_TAP_DUR__LEN                    3
#define BMA250_TAP_DUR__MSK                    0x07
#define BMA250_TAP_DUR__REG                    BMA250_TAP_PARAM_REG

#define BMA250_TAP_SHOCK_DURN__POS             6
#define BMA250_TAP_SHOCK_DURN__LEN             1
#define BMA250_TAP_SHOCK_DURN__MSK             0x40
#define BMA250_TAP_SHOCK_DURN__REG             BMA250_TAP_PARAM_REG

#define BMA250_TAP_QUIET_DURN__POS             7
#define BMA250_TAP_QUIET_DURN__LEN             1
#define BMA250_TAP_QUIET_DURN__MSK             0x80
#define BMA250_TAP_QUIET_DURN__REG             BMA250_TAP_PARAM_REG

#define BMA250_TAP_THRES__POS                  0
#define BMA250_TAP_THRES__LEN                  5
#define BMA250_TAP_THRES__MSK                  0x1F
#define BMA250_TAP_THRES__REG                  BMA250_TAP_THRES_REG

#define BMA250_TAP_SAMPLES__POS                6
#define BMA250_TAP_SAMPLES__LEN                2
#define BMA250_TAP_SAMPLES__MSK                0xC0
#define BMA250_TAP_SAMPLES__REG                BMA250_TAP_THRES_REG

#define BMA250_ORIENT_MODE__POS                  0
#define BMA250_ORIENT_MODE__LEN                  2
#define BMA250_ORIENT_MODE__MSK                  0x03
#define BMA250_ORIENT_MODE__REG                  BMA250_ORIENT_PARAM_REG

#define BMA250_ORIENT_BLOCK__POS                 2
#define BMA250_ORIENT_BLOCK__LEN                 2
#define BMA250_ORIENT_BLOCK__MSK                 0x0C
#define BMA250_ORIENT_BLOCK__REG                 BMA250_ORIENT_PARAM_REG

#define BMA250_ORIENT_HYST__POS                  4
#define BMA250_ORIENT_HYST__LEN                  3
#define BMA250_ORIENT_HYST__MSK                  0x70
#define BMA250_ORIENT_HYST__REG                  BMA250_ORIENT_PARAM_REG

#define BMA250_ORIENT_AXIS__POS                  7
#define BMA250_ORIENT_AXIS__LEN                  1
#define BMA250_ORIENT_AXIS__MSK                  0x80
#define BMA250_ORIENT_AXIS__REG                  BMA250_THETA_BLOCK_REG

#define BMA250_THETA_BLOCK__POS                  0
#define BMA250_THETA_BLOCK__LEN                  6
#define BMA250_THETA_BLOCK__MSK                  0x3F
#define BMA250_THETA_BLOCK__REG                  BMA250_THETA_BLOCK_REG

#define BMA250_THETA_FLAT__POS                  0
#define BMA250_THETA_FLAT__LEN                  6
#define BMA250_THETA_FLAT__MSK                  0x3F
#define BMA250_THETA_FLAT__REG                  BMA250_THETA_FLAT_REG

#define BMA250_FLAT_HOLD_TIME__POS              4
#define BMA250_FLAT_HOLD_TIME__LEN              2
#define BMA250_FLAT_HOLD_TIME__MSK              0x30
#define BMA250_FLAT_HOLD_TIME__REG              BMA250_FLAT_HOLD_TIME_REG

#define BMA250_LOW_POWER_MODE_S__POS            0
#define BMA250_LOW_POWER_MODE_S__LEN            1
#define BMA250_LOW_POWER_MODE_S__MSK            0x01
#define BMA250_LOW_POWER_MODE_S__REG            BMA250_STATUS_LOW_POWER_REG

#define BMA250_EN_FAST_COMP__POS                5
#define BMA250_EN_FAST_COMP__LEN                2
#define BMA250_EN_FAST_COMP__MSK                0x60
#define BMA250_EN_FAST_COMP__REG                BMA250_OFFSET_CTRL_REG

#define BMA250_FAST_COMP_RDY_S__POS             4
#define BMA250_FAST_COMP_RDY_S__LEN             1
#define BMA250_FAST_COMP_RDY_S__MSK             0x10
#define BMA250_FAST_COMP_RDY_S__REG             BMA250_OFFSET_CTRL_REG

#define BMA250_COMP_TARGET_OFFSET_X__POS        1
#define BMA250_COMP_TARGET_OFFSET_X__LEN        2
#define BMA250_COMP_TARGET_OFFSET_X__MSK        0x06
#define BMA250_COMP_TARGET_OFFSET_X__REG        BMA250_OFFSET_PARAMS_REG

#define BMA250_COMP_TARGET_OFFSET_Y__POS        3
#define BMA250_COMP_TARGET_OFFSET_Y__LEN        2
#define BMA250_COMP_TARGET_OFFSET_Y__MSK        0x18
#define BMA250_COMP_TARGET_OFFSET_Y__REG        BMA250_OFFSET_PARAMS_REG

#define BMA250_COMP_TARGET_OFFSET_Z__POS        5
#define BMA250_COMP_TARGET_OFFSET_Z__LEN        2
#define BMA250_COMP_TARGET_OFFSET_Z__MSK        0x60
#define BMA250_COMP_TARGET_OFFSET_Z__REG        BMA250_OFFSET_PARAMS_REG



#define BMA250_GET_BITSLICE(regvar, bitname)\
			(regvar & bitname##__MSK) >> bitname##__POS


#define BMA250_SET_BITSLICE(regvar, bitname, val)\
		  (regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK)  


/** \endcond */


/* CONSTANTS */


/* range and bandwidth */
#define BMA250_RANGE_2G                 0
#define BMA250_RANGE_4G                 1
#define BMA250_RANGE_8G                 2
#define BMA250_RANGE_16G                3

#define BMA250_BW_7_81HZ        0x08
#define BMA250_BW_15_63HZ       0x09
#define BMA250_BW_31_25HZ       0x0A
#define BMA250_BW_62_50HZ       0x0B
#define BMA250_BW_125HZ         0x0C
#define BMA250_BW_250HZ         0x0D
#define BMA250_BW_500HZ         0x0E
#define BMA250_BW_1000HZ        0x0F

/* mode settings */

#define BMA250_MODE_NORMAL      0
#define BMA250_MODE_LOWPOWER    1
#define BMA250_MODE_SUSPEND     2

/* wake up */

#define BMA250_WAKE_UP_PAUSE_0_5MS		  0
#define BMA250_WAKE_UP_PAUSE_1MS		  1
#define BMA250_WAKE_UP_PAUSE_2MS                2
#define BMA250_WAKE_UP_PAUSE_4MS		  3
#define BMA250_WAKE_UP_PAUSE_6MS		  4
#define BMA250_WAKE_UP_PAUSE_10MS		  5
#define BMA250_WAKE_UP_PAUSE_25MS		  6
#define BMA250_WAKE_UP_PAUSE_50MS		  7
#define BMA250_WAKE_UP_PAUSE_100MS		  8
#define BMA250_WAKE_UP_PAUSE_500MS		  9
#define BMA250_WAKE_UP_PAUSE_1000MS		  10


/* LG/HG thresholds are in LSB and depend on RANGE setting */
/* no range check on threshold calculation */

#define BMA250_SELF_TEST0_ON		1
#define BMA250_SELF_TEST1_ON		2

#define BMA250_EE_W_OFF			0
#define BMA250_EE_W_ON			1



/* low-g, high-g, any_motion */


#define BMA250_COUNTER_LG_RST		0
#define BMA250_COUNTER_LG_0LSB		BMA250_COUNTER_LG_RST
#define BMA250_COUNTER_LG_1LSB		1
#define BMA250_COUNTER_LG_2LSB		2
#define BMA250_COUNTER_LG_3LSB		3

#define BMA250_COUNTER_HG_RST		0
#define BMA250_COUNTER_HG_0LSB		BMA250_COUNTER_HG_RST
#define BMA250_COUNTER_HG_1LSB		1
#define BMA250_COUNTER_HG_2LSB		2
#define BMA250_COUNTER_HG_3LSB		3

#define BMA250_COUNTER_RST			0
#define BMA250_COUNTER_0LSB			BMA250_COUNTER_RST
#define BMA250_COUNTER_1LSB			1
#define BMA250_COUNTER_2LSB			2
#define BMA250_COUNTER_3LSB			3



/** Macro to convert floating point low-g-thresholds in G to 8-bit register values.<br>
  * Example: BMA250_LG_THRES_IN_G( 0.3, 2.0) generates the register value for 0.3G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA250_LG_THRES_IN_G( gthres, range)			((256 * gthres ) / range)

/** Macro to convert floating point high-g-thresholds in G to 8-bit register values.<br>
  * Example: BMA250_HG_THRES_IN_G( 1.4, 2.0) generates the register value for 1.4G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA250_HG_THRES_IN_G(gthres, range)				((256 * gthres ) / range)

/** Macro to convert floating point low-g-hysteresis in G to 8-bit register values.<br>
  * Example: BMA250_LG_HYST_THRES_IN_G( 0.2, 2.0) generates the register value for 0.2G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA250_LG_HYST_IN_G( ghyst, range )				((32 * ghyst) / range)

/** Macro to convert floating point high-g-hysteresis in G to 8-bit register values.<br>
  * Example: BMA250_HG_HYST_THRES_IN_G( 0.2, 2.0) generates the register value for 0.2G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */
#define BMA250_HG_HYST_IN_G( ghyst, range )				((32 * ghyst) / range)


/** Macro to convert floating point G-thresholds to 8-bit register values<br>
  * Example: BMA250_ANY_MOTION_THRES_IN_G( 1.2, 2.0) generates the register value for 1.2G threshold in 2G mode.
  * \brief convert g-values to 8-bit value
 */

#define BMA250_ANY_MOTION_THRES_IN_G( gthres, range)	((128 * gthres ) / range)


#define BMA250_ANY_MOTION_DUR_1		0
#define BMA250_ANY_MOTION_DUR_3		1
#define BMA250_ANY_MOTION_DUR_5		2
#define BMA250_ANY_MOTION_DUR_7		3



#define BMA250_SHADOW_DIS_OFF	0
#define BMA250_SHADOW_DIS_ON	1

#define BMA250_LATCH_INT_OFF	0
#define BMA250_LATCH_INT_ON		1

#define BMA250_NEW_DATA_INT_OFF	0
#define BMA250_NEW_DATA_INT_ON	1

#define BMA250_ENABLE_ADV_INT_OFF	0
#define BMA250_ENABLE_ADV_INT_ON	1

#define BMA250_EN_ANY_MOTION_OFF 	0
#define BMA250_EN_ANY_MOTION_ON 	1


#define BMA250_ALERT_OFF	0
#define BMA250_ALERT_ON		1

#define BMA250_ENABLE_LG_OFF	0
#define BMA250_ENABLE_LG_ON		1

#define BMA250_ENABLE_HG_OFF	0
#define BMA250_ENABLE_HG_ON		1



#define BMA250_INT_ALERT		(1<<7)
#define BMA250_INT_ANY_MOTION	(1<<6)
#define BMA250_INT_EN_ADV_INT	(1<<5)
#define BMA250_INT_NEW_DATA		(1<<4)
#define BMA250_INT_LATCH		(1<<3)
#define BMA250_INT_HG			(1<<1)
#define BMA250_INT_LG			(1<<0)


#define BMA250_INT_STATUS_HG			(1<<0)
#define BMA250_INT_STATUS_LG			(1<<1)
#define BMA250_INT_STATUS_HG_LATCHED	(1<<2)
#define BMA250_INT_STATUS_LG_LATCHED	(1<<3)
#define BMA250_INT_STATUS_ALERT			(1<<4)
#define BMA250_INT_STATUS_ST_RESULT		(1<<7)


#define BMA250_CONF1_INT_MSK	((1<<BMA250_ALERT__POS) | (1<<BMA250_EN_ANY_MOTION__POS) | (1<<BMA250_ENABLE_HG__POS) | (1<<BMA250_ENABLE_LG__POS))
#define BMA250_CONF2_INT_MSK	((1<<BMA250_ENABLE_ADV_INT__POS) | (1<<BMA250_NEW_DATA_INT__POS) | (1<<BMA250_LATCH_INT__POS))


#endif   // __BMA250_H__





