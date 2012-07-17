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

#ifndef __TCC_REMOCON_CS_2000_H__
#define __TCC_REMOCON_CS_2000_H__

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
#define LOW_MAX			0x0B00
//High Bit
#define HIGH_MIN		0x1800
#define HIGH_MAX		0x1E00
//Start Bit
#define START_MIN		0x4000
#define START_MAX		0x5000
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
#define REM_ID_ERROR		0xff

#define REMOCON_ID			0xfb04	//vendor ID, prodcut ID
#define REMOCON_REPEAT		0x00000004

#define SCAN_PWR			0xf708

#define SCAN_NUM_0			0xef10
#define SCAN_NUM_1			0xee11
#define SCAN_NUM_2			0xed12
#define SCAN_NUM_3			0xec13
#define SCAN_NUM_4			0xeb14
#define SCAN_NUM_5			0xea15
#define SCAN_NUM_6			0xe916
#define SCAN_NUM_7			0xe817
#define SCAN_NUM_8			0xe718
#define SCAN_NUM_9			0xe619

#define SCAN_UP				0xbf40
#define SCAN_DOWN			0xbe41
#define SCAN_LEFT			0xf807
#define SCAN_RIGHT			0xf906
#define SCAN_ENTER			0xbb44	//center, enter

#define SCAN_STOP			0x4eb1
#define SCAN_PLAY			0x4fb0
#define SCAN_PAUSE			0x45ba
#define SCAN_FB				0x718e
#define SCAN_FF				0x708f
#define SCAN_LAST			0xffff	//no mapping
#define SCAN_NEXT			0xffff	//no mapping
#define SCAN_REC			0x42bd
#define SCAN_CURRENT		0x619e

#define SCAN_VOLUP			0xfd02
#define SCAN_VOLDN			0xfc03

#define SCAN_MENU			0xbc43
#define SCAN_SLEEP			0xf10e
#define SCAN_CANCEL			0xa45b
#define SCAN_GUIDE			0x56a9

#define SCAN_NUM_MINUS		0xb34c
#define SCAN_NUM_PREV		0xe51a

#define SCAN_MUTE			0xf609
#define SCAN_INPUT			0xf40b

#define SCAN_CH_UP			0xff00
#define SCAN_CH_DN			0xfe01

#define SCAN_AUTO_CHANNEL	0xab54
#define SCAN_ADD_DELETE		0xaa55
#define SCAN_SIZE			0x8877
#define SCAN_MULTI_SOUND	0xf50a
	

/*****************************************************************************
*
* External Variables
*
******************************************************************************/
static SCANCODE_MAPPING key_mapping[] = 
{
	{SCAN_PWR, 			REM_POWER},

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

	{SCAN_UP,			REM_DPAD_UP},
	{SCAN_DOWN,			REM_DPAD_DOWN},	
	{SCAN_LEFT,			REM_DPAD_LEFT},
	{SCAN_RIGHT,		REM_DPAD_RIGHT},
#if 0
	{SCAN_ENTER,		REM_DPAD_CENTER},
#else
	{SCAN_ENTER,		REM_ENTER},
#endif

	{SCAN_STOP, 		REM_MEDIA_STOP},
	{SCAN_PLAY,			REM_MEDIA_PLAY_PAUSE},
	{SCAN_PAUSE,		REM_MEDIA_PLAY_PAUSE},
	{SCAN_REC,			REM_MEDIA_RECORD},
#if 1
	{SCAN_FB,			REM_MEDIA_REWIND},
	{SCAN_FF,			REM_MEDIA_FAST_FORWARD},
#else
	{SCAN_FB,			REM_MEDIA_PREVIOUS},
	{SCAN_FF,			REM_MEDIA_NEXT},
#endif

	{SCAN_VOLUP,		REM_VOLUME_UP},
	{SCAN_VOLDN,		REM_VOLUME_DOWN},

	{SCAN_NUM_MINUS,	REM_MINUS},
	{SCAN_NUM_PREV,		REM_PERIOD},

	{SCAN_MUTE,			REM_MUTE},
	
	{SCAN_CH_UP,		REM_PAGE_UP},
	{SCAN_CH_DN,		REM_PAGE_DOWN},

	{SCAN_CANCEL,		REM_BACK},
	{SCAN_MENU,			REM_MENU},
	{SCAN_GUIDE,		REM_HOME},
	{SCAN_ADD_DELETE,	REM_DEL},

	{SCAN_INPUT,		REM_FUNCTION_F3},
	{SCAN_AUTO_CHANNEL,	REM_FUNCTION_F2},
	{SCAN_SLEEP,		REM_FUNCTION_F10},
	{SCAN_SIZE,			REM_FUNCTION_F11},
	{SCAN_MULTI_SOUND,	REM_FUNCTION_F12},

	{SCAN_CURRENT,		REM_TAB},
};

#endif	// __TCC_REMOCON_CS_2000_H__

