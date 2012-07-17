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

#ifndef __TCC_REMOCON_JH_4202_1011_03_H__
#define __TCC_REMOCON_JH_4202_1011_03_H__

//*******************************************
//	Remote controller define
//*******************************************

#define IR_SIGNAL_MAXIMUN_BIT_COUNT	32
#define IR_FIFO_READ_COUNT			17
#define IR_BUFFER_BIT_SHIFT			16
#define IR_ID_MASK				0x0000FFFF
#define IR_CODE_MASK			0xFFFF0000

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

#define REMOCON_ID			0xf30c	//vendor ID, prodcut ID
#define REMOCON_REPEAT		0xaaaaaaac

#define SCAN_PWR			0x7788
#define SCAN_MUTE			0x37c8

#define SCAN_NUM_1			0x3bc4
#define SCAN_NUM_2			0x5ba4
#define SCAN_NUM_3			0x1be4
#define SCAN_NUM_4			0x6b94
#define SCAN_NUM_5			0x2bd4
#define SCAN_NUM_6			0x4bb4
#define SCAN_NUM_7			0x0bf4
#define SCAN_NUM_8			0x738c
#define SCAN_NUM_9			0x33cc
#define SCAN_NUM_0			0x7b84

#define SCAN_NUM_MINUS		0x6699
#define SCAN_NUM_PERIOD		0x53ac

#define SCAN_MENU			0x1ee1
#define SCAN_CANCEL			0x12ed

#define SCAN_UP				0x7e81
#define SCAN_DOWN			0x3ec1
#define SCAN_LEFT			0x0ff0
#define SCAN_RIGHT			0x4fb0
#define SCAN_ENTER			0x6e91	//center, enter

#define SCAN_INPUT			0x17e8
#define SCAN_HOME			0x35ca

#define SCAN_VOLUP			0x5fa0
#define SCAN_VOLDN			0x1fe0

#define SCAN_CH_UP			0x7f80
#define SCAN_CH_DN			0x3fc0

#define SCAN_MOUSE			0x6a95
#define SCAN_DELETE			0x2ad5
#define SCAN_SIZE			0x08f7

#define SCAN_FB				0x47b8
#define SCAN_AUDIO			0xFFFF	//no mapping
#define SCAN_BLANK			0xFFFF	//no mapping
#define SCAN_FF				0x07f8

#define SCAN_STOP			0x39c6
#define SCAN_PLAY			0x7986
#define SCAN_PAUSE			0x51ae
#define SCAN_REC			0x21de

#define SCAN_RED			0x2ed1
#define SCAN_GREEN			0x2dd2
#define SCAN_YELLOW			0x2cd3
#define SCAN_BLUE			0x2fd0

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
#if 0
	{SCAN_ENTER,		REM_DPAD_CENTER},
#else
	{SCAN_ENTER,		REM_ENTER},
#endif

	{SCAN_INPUT,		REM_FUNCTION_F3},
	{SCAN_HOME,			REM_HOME},

	{SCAN_VOLUP,		REM_VOLUME_UP},
	{SCAN_VOLDN,		REM_VOLUME_DOWN},

	{SCAN_CH_UP,		REM_PAGE_UP},
	{SCAN_CH_DN,		REM_PAGE_DOWN},

	{SCAN_DELETE,		REM_DEL},
	{SCAN_MOUSE,		REM_FUNCTION_F2},
	{SCAN_SIZE,			REM_FUNCTION_F11},

#if 1
	{SCAN_FB,			REM_MEDIA_REWIND},
	{SCAN_FF,			REM_MEDIA_FAST_FORWARD},
#else
	{SCAN_FB,			REM_MEDIA_PREVIOUS},
	{SCAN_FF,			REM_MEDIA_NEXT},
#endif
	{SCAN_STOP, 		REM_MEDIA_STOP},
	{SCAN_PLAY,			REM_MEDIA_PLAY_PAUSE},
	{SCAN_PAUSE,		REM_MEDIA_PLAY_PAUSE},
//	{SCAN_REC,			REM_NOTIFICATION},

	{SCAN_RED, 			REM_FUNCTION_F5},
	{SCAN_GREEN, 		REM_FUNCTION_F6},
	{SCAN_YELLOW,		REM_FUNCTION_F7},
	{SCAN_BLUE,			REM_FUNCTION_F8},
};

#endif	// __TCC_REMOCON_CS_2000_H__

