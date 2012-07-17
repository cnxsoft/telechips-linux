/*
 * IR driver for remote controller : tcc_remocon_NOVATEK_IR.h
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

#ifndef __TCC_REMOCON_NOVATEK_H__
#define __TCC_REMOCON_NOVATEK_H__

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
#define REM_ID_ERROR		0xFF

#define REMOCON_ID			0xff00	//vendor ID, prodcut ID
#define REMOCON_REPEAT		0x0

#define SCAN_PWR			0xa25d

#define SCAN_NUM_1			0xb649
#define SCAN_NUM_2			0xf40b
#define SCAN_NUM_3			0xf50a
#define SCAN_NUM_4			0xbe41
#define SCAN_NUM_5			0xfc03
#define SCAN_NUM_6			0xfd02
#define SCAN_NUM_7			0xba45
#define SCAN_NUM_8			0xf807
#define SCAN_NUM_9			0xf906
#define SCAN_NUM_0			0xa659
#define SCAN_NUM_10			0xe41b

#define SCAN_UP				0xe817
#define SCAN_DOWN			0xad52
#define SCAN_LEFT			0xa956
#define SCAN_RIGHT			0xb14e
#define SCAN_ENTER			0xec13	//center, enter

#define SCAN_STOP			0xa55a
#define SCAN_PLAY			0xbd42
#define SCAN_PAUSE			0xa15e
#define SCAN_FB				0xb54a
#define SCAN_FF				0xb946
#define SCAN_LAST			0xee11
#define SCAN_NEXT			0xef10
#define SCAN_REC			0xffff	//no mapping
#define SCAN_CURRENT		0xffff	//no mapping

#define SCAN_VOLUP			0xe718
#define SCAN_VOLDN			0xe619

#define SCAN_MENU			0xe916
#define SCAN_SLEEP			0xffff	//no mapping
#define SCAN_CANCEL			0xffff	//no mapping
#define SCAN_GUIDE			0xa05f

#define SCAN_NUM_MINUS		0xffff	//no mapping
#define SCAN_NUM_PREV		0xffff	//no mapping

#define SCAN_MUTE			0xa758
#define SCAN_INPUT			0xffff	//no mapping

#define SCAN_CH_UP			0xffff	//no mapping
#define SCAN_CH_DN			0xffff	//no mapping

#define SCAN_AUTO_CHANNEL	0xffff	//no mapping
#define SCAN_ADD_DELETE		0xffff	//no mapping
#define SCAN_SIZE			0xffff	//no mapping
#define SCAN_MULTI_SOUND	0xffff	//no mapping

/* Remocon specific key mapping */

#define SCAN_RETURN			0xed12
#define SCAN_LOAD			0xe11e
#define SCAN_SUBTITLE		0xb24d
#define SCAN_AUDIO			0xe01f
#define SCAN_ANGLE			0xf00f
#define SCAN_INFO			0xf10e
#define SCAN_GOTO			0xe51a
#define SCAN_TITLE			0xaa55
#define SCAN_SETUP			0xae51
#define SCAN_AB				0xa857
#define SCAN_REPEAT			0xab54
#define SCAN_STEP			0xea15
#define SCAN_PBC			0xeb14
#define SCAN_ZOOM			0xa45b
#define SCAN_SLOW			0xb847
#define SCAN_RANDOM			0xbb44
#define SCAN_KEYDEC			0xfa05
#define SCAN_KEYADD			0xfb04
#define SCAN_PROGRAM		0xbc43
#define SCAN_CLEAR			0xbf40
#define SCAN_RESUME			0xfe01
#define SCAN_VIDEO			0xff00
#define SCAN_PN				0xb44b
#define SCAN_DIGEST			0xb748
#define SCAN_HDMI			0xf609
#define SCAN_FUNCTION		0xf708
#define SCAN_EPG			0xb04f
#define SCAN_TELETEXT		0xb34c
#define SCAN_SERVICE		0xf20d
#define SCAN_EXIT			0xf30c
#define SCAN_LIST			0xa35c
#define SCAN_BROWSE			0xe21d
#define SCAN_RADIO			0xe31c
#define SCAN_FADEC			0xac53
#define SCAN_FAADD			0xaf50


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
	{SCAN_SUBTITLE,		REM_NOTIFICATION},
#if 1
	{SCAN_FB,			REM_MEDIA_REWIND},
	{SCAN_FF,			REM_MEDIA_FAST_FORWARD},
#else
	{SCAN_FB,			REM_MEDIA_PREVIOUS},
	{SCAN_FF,			REM_MEDIA_NEXT},
#endif

	{SCAN_VOLUP,		REM_VOLUME_UP},
	{SCAN_VOLDN,		REM_VOLUME_DOWN},

	{SCAN_NUM_10,		REM_MINUS},
	{SCAN_GOTO,			REM_PERIOD},

	{SCAN_MUTE,			REM_MUTE},

	{SCAN_NEXT,			REM_PAGE_UP},
	{SCAN_LAST,			REM_PAGE_DOWN},

	{SCAN_RETURN,		REM_BACK},
	{SCAN_MENU,			REM_MENU},
	{SCAN_SETUP,		REM_HOME},
	{SCAN_CLEAR,		REM_DEL},

	{SCAN_INFO,			REM_FUNCTION_F3},
	{SCAN_ANGLE,		REM_FUNCTION_F2},
	{SCAN_PN,			REM_FUNCTION_F10},
	{SCAN_DIGEST,		REM_FUNCTION_F11},
	{SCAN_HDMI,			REM_FUNCTION_F12},
};

#endif	// __TCC_REMOCON_NOVATEK_H__
