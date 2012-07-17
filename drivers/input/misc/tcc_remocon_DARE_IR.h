/*
 * IR driver for remote controller : tcc_remocon_DARE_IR.h
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

#ifndef __TCC_REMOCON_DARE_H__
#define __TCC_REMOCON_DARE_H__

//*******************************************
//	Remote controller define
//*******************************************

#define IR_SIGNAL_MAXIMUN_BIT_COUNT	32
#define IR_FIFO_READ_COUNT			17
#define IR_BUFFER_BIT_SHIFT			16
#define IR_ID_MASK				0x0000FFFF
#define IR_CODE_MASK			0xFFFF0000

//Low Bit
#define LOW_MIN			0x0600
#define LOW_MAX			0x0BFF
//High Bit
#define HIGH_MIN		0x1600
#define HIGH_MAX		0x1DFF
//Start Bit
#define START_MIN		0x4000
#define START_MAX		0x50FF
//Repeat Bit
#define REPEAT_MIN		0x2000
#define REPEAT_MAX		0x27FF

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

#define REMOCON_ID			0x40bf	//vendor ID, prodcut ID

#define SCAN_PWR			0x02fd

#define SCAN_NUM_1			0x48b7	//"1"
#define SCAN_NUM_2			0x6897	//"2"
#define SCAN_NUM_3			0x40bf  //"3"
#define SCAN_NUM_4			0x58a7 	//"4"
#define SCAN_NUM_5			0x7887  //"5"
#define SCAN_NUM_6			0xc03f 	//"6"
#define SCAN_NUM_7			0xf00f	//"7"
#define SCAN_NUM_8			0xd02f	//"8"
#define SCAN_NUM_9			0xe01f  //"9"
#define SCAN_NUM_0			0x22dd	//"0"

#define SCAN_UP				0x827d	//up
#define SCAN_DOWN			0xa25d	//down
#define SCAN_LEFT			0xb24d	//left
#define SCAN_RIGHT			0xba45	//right
#define SCAN_ENTER			0x8877	//center, enter

#define SCAN_STOP			0x1ae5
#define SCAN_PLAY			0x0af5
#define SCAN_PAUSE			0xFFFF	//no mapping
#define SCAN_FB				0xFFFF	//fast backward
#define SCAN_FF				0xFFFF	//fast forward
#define SCAN_LAST			0xb847
#define SCAN_NEXT			0x00ff
#define SCAN_REC			0xFFFF	//no mapping
#define SCAN_CURRENT		0xFFFF	//no mapping

#define SCAN_VOLUP			0x8a75
#define SCAN_VOLDN			0x9a65

#define SCAN_MENU			0xFFFF	//no mapping
#define SCAN_SLEEP			0xFFFF	//no mapping
#define SCAN_CANCEL			0xFFFF	//no mapping
#define SCAN_GUIDE			0xFFFF	//no mapping

#define SCAN_NUM_MINUS		0xFFFF	//no mapping
#define SCAN_NUM_PREV		0xFFFF	//no mapping

#define SCAN_MUTE			0x708f
#define SCAN_INPUT			0xFFFF	//no mapping

#define SCAN_CH_UP			0xFFFF	//no mapping
#define SCAN_CH_DN			0xFFFF	//no mapping

#define SCAN_AUTO_CHANNEL	0xFFFF	//no mapping
#define SCAN_ADD_DELETE		0xFFFF	//no mapping
#define SCAN_SIZE			0xFFFF	//no mapping
#define SCAN_MULTI_SOUND	0xFFFF	//no mapping

/* Remocon specific key mapping */

#define SCAN_SUBTITLE		0x906f
#define SCAN_AUDIO			0xd827
#define SCAN_INFO			0xe817

#define SCAN_GOTO			0xf807
#define SCAN_TITLE			0x807f
#define SCAN_SETUP			0x50af

#define SCAN_REPEAT			0xc837

#define SCAN_CLEAR			0x12ed
#define SCAN_VIDEO			0x10ef
#define SCAN_PN				0x6a95
#define SCAN_DIGEST			0x609f
#define SCAN_HDMI			0x32cd
#define SCAN_FUNCTION		0xa05f
#define SCAN_RADIO			0x926d


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
	{SCAN_ENTER,		REM_ENTER},

	{SCAN_STOP, 		REM_MEDIA_STOP},
	{SCAN_PLAY,			REM_MEDIA_PLAY_PAUSE},
	{SCAN_PAUSE,		REM_MEDIA_PLAY_PAUSE},
	{SCAN_REC,			REM_NOTIFICATION},
	{SCAN_FB,			REM_MEDIA_REWIND},
	{SCAN_FF,			REM_MEDIA_FAST_FORWARD},

	{SCAN_VOLUP,		REM_VOLUME_UP},
	{SCAN_VOLDN,		REM_VOLUME_DOWN},

	{SCAN_NUM_MINUS,	REM_MINUS},
	{SCAN_NUM_PREV,		REM_PERIOD},

	{SCAN_MUTE,			REM_MUTE},
	{SCAN_INPUT,		REM_FUNCTION_F1},

	{SCAN_CH_UP,		REM_PAGE_UP},
	{SCAN_CH_DN,		REM_PAGE_DOWN},

	{SCAN_SLEEP,		REM_FUNCTION_F10},
	{SCAN_CANCEL,		REM_BACK},
	{SCAN_MENU,			REM_MENU},
	{SCAN_GUIDE,		REM_HOME},

	{SCAN_AUTO_CHANNEL,	REM_FUNCTION_F2},
	{SCAN_ADD_DELETE,	REM_DEL},
	{SCAN_SIZE,			REM_FUNCTION_F11},
	{SCAN_MULTI_SOUND,	REM_FUNCTION_F12},

	{SCAN_CURRENT,		REM_TAB},
};

#endif // __TCC_REMOCON_DARE_H__
