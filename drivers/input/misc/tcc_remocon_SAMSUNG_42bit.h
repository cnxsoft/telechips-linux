/*
 * IR driver for remote controller : tcc_remocon_CS_2000_IR_X_CANVAS.h
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __TCC_REMOCON_SAMSUNG_42BIT_H__
#define __TCC_REMOCON_SAMSUNG_42BIT_H__

//*******************************************
//	Remote controller define
//*******************************************

#define IR_SIGNAL_MAXIMUN_BIT_COUNT	42
#define IR_FIFO_READ_COUNT			22
#define IR_BUFFER_BIT_SHIFT			24	// 16
#define IR_ID_MASK				0x00000FFFFFF
#define IR_CODE_MASK			0xFFFFF000000

//Low Bit
#define LOW_MIN			0x0700
#define LOW_MAX			0x0FFF
//High Bit
#define HIGH_MIN		0x1800
#define HIGH_MAX		0x1E00
//Start Bit
#define START_MIN		0x4000
#define START_MAX		0x7000
//Repeat Bit
#define REPEAT_MIN		0x2000
#define REPEAT_MAX		0x2800

#define LOW_MIN_TCC892X		LOW_MIN * 2
#define LOW_MAX_TCC892X		LOW_MAX * 2
#define HIGH_MIN_TCC892X	HIGH_MIN * 2
#define HIGH_MAX_TCC892X	HIGH_MAX * 2
#define START_MIN_TCC892X	START_MIN * 2
#define START_MAX_TCC892X	START_MAX * 2
#define REPEAT_MIN_TCC892X	REPEAT_MIN * 2
#define REPEAT_MAX_TCC892X	REPEAT_MAX * 2


/*****************************************************************************
*
* IR remote controller scancode
*
******************************************************************************/
#define REM_ID_ERROR		0xFF

#define REMOCON_ID			0xf92036	//vendor ID, prodcut ID
#define REMOCON_REPEAT		0xaaaaaaac

#define SCAN_PWR			0x2d52b
#define SCAN_MUTE			0x2b947

#define SCAN_NUM_1			0x3c03f
#define SCAN_NUM_2			0x3bc43
#define SCAN_NUM_3			0x3b847
#define SCAN_NUM_4			0x3b44b
#define SCAN_NUM_5			0x3b04f
#define SCAN_NUM_6			0x3ac53
#define SCAN_NUM_7			0x3a857
#define SCAN_NUM_8			0x3a45b
#define SCAN_NUM_9			0x3a05f
#define SCAN_NUM_0			0x39c63

#define SCAN_NUM_EQUAL		0x3946b
#define SCAN_NUM_MINUS		0x3c837
#define SCAN_NUM_PERIOD		0x2fd03

#define SCAN_MENU			0x2b54b
#define SCAN_CANCEL			0x2d12f

#define SCAN_UP				0x28d73
#define SCAN_DOWN			0x28977
#define SCAN_LEFT			0x2916f
#define SCAN_RIGHT			0x2956b
#define SCAN_DPAD_CENTER	0x29967	//center
#define SCAN_ENTER			0x2dd23	//enter

#define SCAN_HOME			0x2cd33

#define SCAN_VOLUP			0x2c13f
#define SCAN_VOLDN			0x2bd43

#define SCAN_CH_UP			0x2c937
#define SCAN_CH_DN			0x2c53b

#define SCAN_DELETE			0x2d927


/*****************************************************************************
*
* External Variables
*
******************************************************************************/
static SCANCODE_MAPPING key_mapping[] = 
{
	{SCAN_PWR, 			REM_POWER},
	{SCAN_MUTE, 		REM_MUTE},

	{SCAN_NUM_1,		REM_1},
	{SCAN_NUM_2,		REM_2},
	{SCAN_NUM_3,		REM_3},
	{SCAN_NUM_4,		REM_4},
	{SCAN_NUM_5,		REM_5},
	{SCAN_NUM_6,		REM_6},
	{SCAN_NUM_7,		REM_7},
	{SCAN_NUM_8,		REM_8},
	{SCAN_NUM_9,		REM_9},
	{SCAN_NUM_0,		REM_0},

	{SCAN_NUM_MINUS,	REM_MINUS},
	{SCAN_NUM_PERIOD,	REM_PERIOD},

	{SCAN_MENU,			REM_MENU},
	{SCAN_CANCEL,		REM_BACK},

	{SCAN_UP,			REM_DPAD_UP},
	{SCAN_DOWN,			REM_DPAD_DOWN},	
	{SCAN_LEFT,			REM_DPAD_LEFT},
	{SCAN_RIGHT,		REM_DPAD_RIGHT},

	{SCAN_DPAD_CENTER,	REM_DPAD_CENTER},
	{SCAN_ENTER,		REM_ENTER},

	{SCAN_HOME,			REM_HOME},

	{SCAN_VOLUP,		REM_VOLUME_UP},
	{SCAN_VOLDN,		REM_VOLUME_DOWN},

	{SCAN_CH_UP,		REM_PAGE_UP},
	{SCAN_CH_DN,		REM_PAGE_DOWN},

	{SCAN_DELETE,		REM_DEL},
};

#endif	// __TCC_REMOCON_CS_2000_H__

