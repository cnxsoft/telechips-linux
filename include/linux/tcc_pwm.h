/****************************************************************************
 *	 FileName	 : ioctl_pwrstr.h
 *	 Description : 
 ****************************************************************************
*
 *	 TCC Version 1.0
 *	 Copyright (c) Telechips, Inc.
 *	 ALL RIGHTS RESERVED
*
 ****************************************************************************/


/************************************************************************************************
*									 Revision History											*
*																								*
* Version : 1.0    : 2009, 8, 19																*
************************************************************************************************/

#ifndef __IOCTL_PWRSTR_H__
#define __IOCTL_PWRSTR_H__


typedef enum {
	DEVICE_LCD = 0,		// display device (LCD, ...)
	DEVICE_BACKLIGHT,	// backlight only.
	DEVICE_ADC,
	DEVICE_CAMERA,
	DEVICE_GPSB,
	DEVICE_HDMI,
	DEVICE_KEYPAD,
	DEVICE_POWERBUTTON,
	DEVICE_SDHC,
	DEVICE_UART,
	DEVICE_TOUCH,
	DEVICE_TVOUT,
	DEVICE_OTG,			// usb
	DEVICE_OHCI,		// usb
	DEVICE_USERINTR,	// Linux Uesr Interrupt
	DEVICE_SATA,
	DEVICE_GPIOEXP,
//	DEIVCE_XXXX,
//	...

	DEVICE_MAX
} PWR_DEVICE_ID_TYPE;

typedef enum {
	PWR_CMD_OFF = 0,
	PWR_CMD_ON,
	PWR_CMD_GETSTATUS,
	PWR_CMD_MAX
} PWR_CMD_TYPE;

typedef enum {
	PWR_STATUS_OFF = 0,
	PWR_STATUS_ON,
	PWR_STATUS_UNKNOWN,
	PWR_STATUS_MAX
} PWR_STATUS_TYPE;

// for input
typedef struct _stpwrioctl{
	unsigned int deviceid;	// refer to 'PWR_DEVICE_ID_TYPE'
	unsigned int cmd;		// refer to 'PWR_CMD_TYPE'
}stpwrioctl;

// for ouput
typedef struct _stpwrinfo{
	unsigned int status;	// refer to 'PWR_STATUS_TYPE'
}stpwrinfo;


typedef struct pwr_ioctl_param {
    stpwrioctl in;
    stpwrinfo out;    // refer to 'PWR_STATUS_TYPE'
} pwr_ioctl_param_t;

#endif /* __IOCTL_STR_H__ */

