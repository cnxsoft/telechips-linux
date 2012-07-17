/*
 * include/linux/goodix_touch.h
 *
 * Copyright (C) 2010 - 2011 Goodix, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#ifndef 	_LINUX_GOODIX_TOUCH_H
#define		_LINUX_GOODIX_TOUCH_H

#include <linux/earlysuspend.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>

//#include <linux/51boardpcb.h>
//*************************TouchScreen Work Part*****************************
#define GOODIX_I2C_NAME "Goodix-TS"
//define default resolution of the touchscreen
//#define TOUCH_MAX_HEIGHT 	800//1024//4096			
//#define TOUCH_MAX_WIDTH		600//600//4096


/*
#define INT_PORT  	    S3C64XX_GPL(10)//S3C64XX_GPN(15)						//Int IO port  S3C64XX_GPL(10)
#ifdef INT_PORT
	#define TS_INT 		gpio_to_irq(INT_PORT)			//Interrupt Number,EINT18(119)
	#define INT_CFG    	S3C_GPIO_SFN(3)//(2)					//IO configer as EINT
#else
	#define TS_INT	0
#endif	
*/

//whether need send cfg?
//#define DRIVER_SEND_CFG

//set trigger mode
#define INT_TRIGGER			0

#define POLL_TIME		10	//actual query spacing interval:POLL_TIME+6

#define GOODIX_MULTI_TOUCH
#ifdef GOODIX_MULTI_TOUCH
	#define MAX_FINGER_NUM	8//10	
#else
	#define MAX_FINGER_NUM	1	
#endif
//--------------------------- emdoor tp define -----------------------------------

#define DRIVER_SEND_CFG 							1

#if defined(TP_GOODIX_EM7127_BM_81070027023_01)//em7127
#define TP_GOODIX_SIZE_7	
#endif

#if defined(TP_GOODIX_EM73_HOTA_C113171A1)//em73
#define TP_GOODIX_SIZE_7							1
#define TP_GOODIX_REVERSE_X							1
#endif

#if defined(TP_GOODIX_EM86_DPT_N3371A)//em86
//#define TP_GOODIX_SIZE_8							1
#define HAVE_TOUCH_KEY								1
#define TOUCH_WITH_LIGHT_KEY						1
#endif

#if defined(TP_GOODIX_EM8127_BM_81080028A23_00)
#define TP_GOODIX_SIZE_8  1
#endif

#if defined(TP_GOODIX_EM1125_BM_81100026023_02)//em1125
#define TP_GOODIX_SIZE_101							1
#endif

#if defined(TP_GOODIX_EM101_TTCT097006_ZJFPC)
//#define TP_GOODIX_SIZE_10     1
#define TP_GOODIX_SWAP_X_Y    1
#define TP_GOODIX_REVERSE_Y   1
#define HAVE_TOUCH_KEY								1
#define TOUCH_WITH_LIGHT_KEY						1
#endif

#if defined(TP_GOODIX_EM101_300_L3512A_A00)
//#define TP_GOODIX_SIZE_10     1
//#define TP_GOODIX_SWAP_X_Y    1
//#define TP_GOODIX_REVERSE_Y   1
#define HAVE_TOUCH_KEY							1
#define TOUCH_WITH_LIGHT_KEY						1
#endif

//define default resolution of the touchscreen
#if defined(TP_GOODIX_SIZE_7)
#define TOUCH_MAX_HEIGHT 	800	
#define TOUCH_MAX_WIDTH		480
#elif defined(TP_GOODIX_SIZE_8)
#define TOUCH_MAX_HEIGHT 	800		
#define TOUCH_MAX_WIDTH		600
#elif defined(TP_GOODIX_SIZE_101)
#define TOUCH_MAX_HEIGHT 	1024			
#define TOUCH_MAX_WIDTH		600
#elif defined(TP_GOODIX_SIZE_10)
#define TOUCH_MAX_HEIGHT 	1024			
#define TOUCH_MAX_WIDTH		768
#elif defined(CONFIG_MACH_TCC8800)
#define TOUCH_MAX_HEIGHT 	1024			
#define TOUCH_MAX_WIDTH		768
#else
#define TOUCH_MAX_HEIGHT 	4096			
#define TOUCH_MAX_WIDTH		4096
#endif

#define TP_GOODIX_USE_IRQ							1
#if !defined(TP_NOT_USE_OFFON_BACKLIGHT)     //for relsove some goodix  touch can not use when off /on backlight
#define TP_GOODIX_USE_PW_CTRL						1
#endif
#define TP_GOODIX_CHECK_SUM							1//the new fw support check sum,but old fw not

#if defined(TP_GOODIX_CHECK_SUM)
#define CHECK_SUM_WITH_LOOP							1
#endif

#if defined(TP_GOODIX_USE_IRQ)
#define  TS_INT 		INT_EI2	//Interrupt Number,INT_EI2 as 5
#define  TP_GOODIX_USE_IRQ_WITH_HRTIMER				1
#else
#define TS_INT	0
#endif

//static const char *goodix_ts_name = "Goodix Capacitive TouchScreen";
//--------------------------- end emdoor tp define -------------------------------

//#define swap(x, y) do { typeof(x) z = x; x = y; y = z; } while (0)

struct goodix_ts_data {
	uint16_t addr;
	uint8_t bad_data;
	struct i2c_client *client;
	struct input_dev *input_dev;
	int use_reset;		//use RESET flag
	int use_irq;		//use EINT flag
	int read_mode;		//read moudle mode,20110221 by andrew
	struct hrtimer timer;
	struct work_struct  work;
	char phys[32];
	int retry;
	struct early_suspend early_suspend;
	int (*power)(struct goodix_ts_data * ts, int on);
	uint16_t abs_x_max;
	uint16_t abs_y_max;
	uint8_t max_touch_num;
	uint8_t int_trigger_type;
	uint8_t green_wake_mode;
};

static const char *goodix_ts_name = "tcc-ts-goodix-cap";
static struct workqueue_struct *goodix_wq;
struct i2c_client * i2c_connect_client = NULL; 
static struct proc_dir_entry *goodix_proc_entry;
static struct kobject *goodix_debug_kobj;
	
#ifdef CONFIG_HAS_EARLYSUSPEND
static void goodix_ts_early_suspend(struct early_suspend *h);
static void goodix_ts_late_resume(struct early_suspend *h);
#endif 

//*****************************End of Part I *********************************

//*************************Touchkey Surpport Part*****************************
//#define HAVE_TOUCH_KEY
#ifdef HAVE_TOUCH_KEY
	#define READ_COOR_ADDR 0x00
#if defined(TP_GOODIX_EM101_TTCT097006_ZJFPC)
const uint16_t touch_key_array[]={
									  //KEY_MENU,				//MENU
									  //KEY_HOME,				//HOME
									  //KEY_SEND				//CALL
									  KEY_HOME,				
									  KEY_MENU,
									  KEY_BACK,
									  KEY_SEARCH,				
									 };
#else
const uint16_t touch_key_array[]={
								  //KEY_MENU,				//MENU
								  //KEY_HOME,				//HOME
								  //KEY_SEND				//CALL
								  KEY_SEARCH,				
								  KEY_BACK,
								  KEY_MENU,
								  KEY_HOME, 			
								 };
#endif
	#define MAX_KEY_NUM	 (sizeof(touch_key_array)/sizeof(touch_key_array[0]))
#else
	#define READ_COOR_ADDR 0x01
#endif
//*****************************End of Part II*********************************

//*************************Firmware Update part*******************************
#define CONFIG_TOUCHSCREEN_GOODIX_IAP
#ifdef CONFIG_TOUCHSCREEN_GOODIX_IAP
#define UPDATE_NEW_PROTOCOL

unsigned int oldcrc32 = 0xFFFFFFFF;
unsigned int crc32_table[256];
unsigned int ulPolynomial = 0x04c11db7;
unsigned char rd_cfg_addr;
unsigned char rd_cfg_len;
unsigned char g_enter_isp = 0;

static int goodix_update_write(struct file *filp, const char __user *buff, unsigned long len, void *data);
static int goodix_update_read( char *page, char **start, off_t off, int count, int *eof, void *data );

#define PACK_SIZE 					64					//update file package size
#define MAX_TIMEOUT					60000				//update time out conut
#define MAX_I2C_RETRIES				20					//i2c retry times

//I2C buf address
#define ADDR_CMD					80
#define ADDR_STA					81
#ifdef UPDATE_NEW_PROTOCOL
	#define ADDR_DAT				0
#else
	#define ADDR_DAT				82
#endif

//moudle state
#define NEW_UPDATE_START			0x01
#define UPDATE_START				0x02
#define SLAVE_READY					0x08
#define UNKNOWN_ERROR				0x00
#define FRAME_ERROR					0x10
#define CHECKSUM_ERROR				0x20
#define TRANSLATE_ERROR				0x40
#define FLASH_ERROR					0X80

//error no
#define ERROR_NO_FILE				2	//ENOENT
#define ERROR_FILE_READ				23	//ENFILE
#define ERROR_FILE_TYPE				21	//EISDIR
#define ERROR_GPIO_REQUEST			4	//EINTR
#define ERROR_I2C_TRANSFER			5	//EIO
#define ERROR_NO_RESPONSE			16	//EBUSY
#define ERROR_TIMEOUT				110	//ETIMEDOUT

//update steps
#define STEP_SET_PATH              1
#define STEP_CHECK_FILE            2
#define STEP_WRITE_SYN             3
#define STEP_WAIT_SYN              4
#define STEP_WRITE_LENGTH          5
#define STEP_WAIT_READY            6
#define STEP_WRITE_DATA            7
#define STEP_READ_STATUS           8
#define FUN_CLR_VAL                9
#define FUN_CMD                    10
#define FUN_WRITE_CONFIG           11

//fun cmd
#define CMD_DISABLE_TP             0
#define CMD_ENABLE_TP              1
#define CMD_READ_VER               2
#define CMD_READ_RAW               3
#define CMD_READ_DIF               4
#define CMD_READ_CFG               5
#define CMD_SYS_REBOOT             101

//read mode
#define MODE_RD_VER                1
#define MODE_RD_RAW                2
#define MODE_RD_DIF                3
#define MODE_RD_CFG                4


#endif
//*****************************End of Part III********************************
struct goodix_i2c_platform_data {
	uint32_t version;	/* Use this entry for panels with */
	//reservation
};

#endif /* _LINUX_GOODIX_TOUCH_H */
