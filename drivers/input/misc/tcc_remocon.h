/*
 * IR driver for remote controller : tcc_remocon.h
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

#ifndef __TCC_REMOCON_H__
#define __TCC_REMOCON_H__

#include <linux/types.h>


/*******************************************************************************
 Remote KEY value
 This can be found from device/telechips/tccxxx/telechips_remote_controller.kl
********************************************************************************/ 

#define REM_1					2
#define REM_2					3
#define REM_3					4
#define REM_4					5
#define REM_5					6
#define REM_6					7
#define REM_7					8
#define REM_8					9
#define REM_9					10
#define REM_0					11

#define REM_A					30
#define REM_B					48
#define REM_C					46
#define REM_D					32
#define REM_E					18
#define REM_F					33
#define REM_G					34
#define REM_H					35
#define REM_I					23
#define REM_J					36
#define REM_K					37
#define REM_L					38
#define REM_M					50
#define REM_N					49
#define REM_O					24
#define REM_P					25
#define REM_Q					16
#define REM_R					19
#define REM_S					31
#define REM_T					20
#define REM_U					22
#define REM_V					47
#define REM_W					17
#define REM_X					45
#define REM_Y					21
#define REM_Z					44

#define REM_FUNCTION_F1			59
#define REM_FUNCTION_F2			60
#define REM_FUNCTION_F3			61
#define REM_FUNCTION_F4			62
#define REM_FUNCTION_F5			63
#define REM_FUNCTION_F6			64
#define REM_FUNCTION_F7			65
#define REM_FUNCTION_F8			66
#define REM_FUNCTION_F9			67
#define REM_FUNCTION_F10		68
#define REM_FUNCTION_F11		87
#define REM_FUNCTION_F12		88

#define REM_POWER				116
#define REM_SEARCH				217
#define REM_SLASH				53
#define REM_BACKSLASH			43
#define REM_SEMICOLON			39
#define REM_COMMA				51
#define REM_SPACE				57
#define REM_EQUALS				13

#define REM_DPAD_UP				103
#define REM_DPAD_DOWN			108
#define REM_DPAD_LEFT			105
#define REM_DPAD_RIGHT			106
#define REM_DPAD_ENTER			96
#define REM_ENTER				28

#define REM_MEDIA_STOP			166
#define REM_MEDIA_PLAY_PAUSE	164
#define REM_MEDIA_REWIND		168
#define REM_MEDIA_FAST_FORWARD	208
#define REM_MEDIA_PREVIOUS		165
#define REM_MEDIA_NEXT			163
#define REM_MEDIA_RECORD		167

#define REM_VOLUME_UP			115
#define REM_VOLUME_DOWN			114
#define REM_MINUS				12
#define REM_PERIOD				52
#define REM_MUTE				113
#define REM_PAGE_UP				104
#define REM_PAGE_DOWN			109
#define REM_BACK				158
#define REM_MENU				127
#define REM_HOME				172
#define REM_DEL					14
#define REM_TAB					15

#define isRepeatableKey(i)      ((i==REM_DPAD_UP) || (i==REM_DPAD_DOWN) || (i==REM_DPAD_LEFT) || (i==REM_DPAD_RIGHT) || \
                                 (i==REM_ENTER) || (i==REM_POWER) || (i==REM_MEDIA_REWIND) || (i==REM_MEDIA_FAST_FORWARD) || \
                                 (i==REM_MEDIA_PREVIOUS) || (i==REM_MEDIA_NEXT) || \
                                 (i==REM_VOLUME_UP) || (i==REM_VOLUME_DOWN) || (REM_1<=i && i<=REM_0) || (i==REM_HOME))


/*******************************************************************************
 Remote controller define
********************************************************************************/ 
#define NO_KEY			0
#define NEW_KEY			1
#define REPEAT_START 	2
#define REPEAT_KEY		3

#define STATUS0			0
#define STATUS1			1
#define STATUS2			2
#define STATUS3			3

/* 1 unit = 1 / [IOBUS/(HwREMOCON->uCLKDIVIDE.bCLKDIVIDE.CLK_DIV)]
              = 1 / [156/(500*20)]  us
              = 64.1us
*/
/* 1 unit = 1 / [IOBUS/(HwREMOCON->uCLKDIVIDE.bCLKDIVIDE.CLK_DIV)]
              = 1 / [165/(500*21)]  us
              = 63.63us
*/
/* 1 unit = 1 / [IOBUS/(HwREMOCON->uCLKDIVIDE.bCLKDIVIDE.CLK_DIV)]
			  = 1 / [165/(500*26)]	us
			  = 78.78us
*/

// Scan-code mapping for keypad
typedef struct {
	unsigned short	rcode;
	unsigned char	vkcode;
}SCANCODE_MAPPING;


/*******************************************************************************
 External variables
********************************************************************************/ 

void RemoconCommandOpen(void);
void RemoconCommandReset(void);
void RemoconConfigure(void);
void RemoconStatus(void);
void RemoconDivide(int state);
void DisableRemocon(void);
void RemoconReset(void);
void RemoconInit(int state);
void RemoconIrqClear(void);
void RemoconIntClear(void);
void RemoconIntWait(void);

extern void tcc_remocon_reset(void);
extern void tcc_remocon_set_io(void);

/*******************************************************************************
 Decide the remote module
 This can be changed with 'make menuconfig' command.
********************************************************************************/ 
#ifdef CONFIG_DARE_IR
	#include "tcc_remocon_DARE_IR.h"

#elif defined(CONFIG_CS_2000_IR_X_CANVAS)
	#include "tcc_remocon_CS_2000_IR_X_CANVAS.h"

#elif defined(CONFIG_NOVATEK_IR)
	#include "tcc_remocon_NOVATEK_IR.h"

#elif defined(CONFIG_JH_4202_1011_03)
	#include "tcc_remocon_jh_4202_1011_03.h"
#elif defined(CONFIG_SAMSUNG_42BIT)
	#include "tcc_remocon_SAMSUNG_42bit.h"
#elif defined(CONFIG_HDS892S)
	#include "tcc_remocon_HDS892S.h"
#else
	#error you don not select proper remocon module
#endif
 
#endif	//__TCC_REMOCON_H__

