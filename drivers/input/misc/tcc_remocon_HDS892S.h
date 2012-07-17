/*
 * IR driver for remote controller : tcc_remocon_HDS892S.h
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

#ifndef __TCC_HDS892S_H__
#define __TCC_HDS892S_H__

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

#define REMOCON_ID			0xff00	//vendor ID, prodcut ID
#define REMOCON_REPEAT		0xaaaaaaac

#define SCAN_PWR			0xe51a
#define SCAN_MUTE			0xe619

#define SCAN_MENU			0xe916
#define SCAN_HOME			0xea15
#define SCAN_CANCEL			0xf20d

#define SCAN_UP				0xeb14
#define SCAN_DOWN			0xf30c
#define SCAN_LEFT			0xed12
#define SCAN_RIGHT			0xee11
#define SCAN_ENTER			0xef10	//center, enter

#define SCAN_VOLUP			0xf50a
#define SCAN_VOLDN			0xf906

#define SCAN_MOUSE			0xf708

#define SCAN_FB				0xfd02
#define SCAN_FF				0xfe01

#define SCAN_PREV			0xfa05
#define SCAN_NEXT			0xf609

#define SCAN_PLAY			0xff00


/*****************************************************************************
*
* External Variables
*
******************************************************************************/
static SCANCODE_MAPPING key_mapping[] = 
{
	{SCAN_PWR, 			REM_POWER},
	{SCAN_MUTE, 		REM_MUTE},

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

	{SCAN_HOME,			REM_HOME},

	{SCAN_VOLUP,		REM_VOLUME_UP},
	{SCAN_VOLDN,		REM_VOLUME_DOWN},

	{SCAN_MOUSE,		REM_FUNCTION_F2},


	{SCAN_FB,			REM_MEDIA_REWIND},
	{SCAN_FF,			REM_MEDIA_FAST_FORWARD},
	{SCAN_PREV,			REM_MEDIA_PREVIOUS},
	{SCAN_NEXT,			REM_MEDIA_NEXT},
	{SCAN_PLAY,			REM_MEDIA_PLAY_PAUSE},
};

#endif	// __TCC_REMOCON_CS_2000_H__

