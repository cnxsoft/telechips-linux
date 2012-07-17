 /****************************************************************************
 *   FileName    : tca_remocon.h
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/


//*******************************************
//	Remote controller define
//*******************************************

#ifndef	__TCC_CEC_INTERFACE_H__
#define	__TCC_CEC_INTERFACE_H__



/** Maximum CEC frame size */
#define CEC_MAX_FRAME_SIZE                16
/** Not valid CEC physical address */
#define CEC_NOT_VALID_PHYSICAL_ADDRESS    0xFFFF

/** CEC broadcast address (as destination address) */
#define CEC_MSG_BROADCAST        0x0F
/** CEC unregistered address (as initiator address) */
#define CEC_LADDR_UNREGISTERED   0x0F

/*
 * CEC Messages
 */

//@{
/** @name Messages for the One Touch Play Feature */
#define CEC_OPCODE_ACTIVE_SOURCE            0x82
#define CEC_OPCODE_IMAGE_VIEW_ON            0x04
#define CEC_OPCODE_TEXT_VIEW_ON             0x0D
//@}

//@{
/** @name Messages for the Routing Control Feature */
#define CEC_OPCODE_INACTIVE_SOURCE          0x9D
#define CEC_OPCODE_REQUEST_ACTIVE_SOURCE    0x85
#define CEC_OPCODE_ROUTING_CHANGE           0x80
#define CEC_OPCODE_ROUTING_INFORMATION      0x81
#define CEC_OPCODE_SET_STREAM_PATH          0x86
//@}

//@{
/** @name Messages for the Standby Feature */
#define CEC_OPCODE_STANDBY                  0x36
//@}

//@{
/** @name Messages for the One Touch Record Feature */
#define CEC_OPCODE_RECORD_OFF               0x0B
#define CEC_OPCODE_RECORD_ON                0x09
#define CEC_OPCODE_RECORD_STATUS            0x0A
#define CEC_OPCODE_RECORD_TV_SCREEN         0x0F
//@}

//@{
/** @name Messages for the Timer Programming Feature */
#define CEC_OPCODE_CLEAR_ANALOGUE_TIMER     0x33
#define CEC_OPCODE_CLEAR_DIGITAL_TIMER      0x99
#define CEC_OPCODE_CLEAR_EXTERNAL_TIMER     0xA1
#define CEC_OPCODE_SET_ANALOGUE_TIMER       0x34
#define CEC_OPCODE_SET_DIGITAL_TIMER        0x97
#define CEC_OPCODE_SET_EXTERNAL_TIMER       0xA2
#define CEC_OPCODE_SET_TIMER_PROGRAM_TITLE  0x67
#define CEC_OPCODE_TIMER_CLEARED_STATUS     0x43
#define CEC_OPCODE_TIMER_STATUS             0x35
//@}

//@{
/** @name Messages for the System Information Feature */
#define CEC_OPCODE_CEC_VERSION              0x9E
#define CEC_OPCODE_GET_CEC_VERSION          0x9F
#define CEC_OPCODE_GIVE_PHYSICAL_ADDRESS    0x83
#define CEC_OPCODE_GET_MENU_LANGUAGE        0x91
//#define CEC_OPCODE_POLLING_MESSAGE
#define CEC_OPCODE_REPORT_PHYSICAL_ADDRESS  0x84
#define CEC_OPCODE_SET_MENU_LANGUAGE        0x32
//@}

//@{
/** @name Messages for the Deck Control Feature */
#define CEC_OPCODE_DECK_CONTROL             0x42
#define CEC_OPCODE_DECK_STATUS              0x1B
#define CEC_OPCODE_GIVE_DECK_STATUS         0x1A
#define CEC_OPCODE_PLAY                     0x41
//@}

//@{
/** @name Messages for the Tuner Control Feature */
#define CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS 0x08
#define CEC_OPCODE_SELECT_ANALOGUE_SERVICE  0x92
#define CEC_OPCODE_SELECT_DIGITAL_SERVICE   0x93
#define CEC_OPCODE_TUNER_DEVICE_STATUS      0x07
#define CEC_OPCODE_TUNER_STEP_DECREMENT     0x06
#define CEC_OPCODE_TUNER_STEP_INCREMENT     0x05
//@}

//@{
/** @name Messages for the Vendor Specific Commands Feature */
#define CEC_OPCODE_DEVICE_VENDOR_ID         0x87
#define CEC_OPCODE_GET_DEVICE_VENDOR_ID     0x8C
#define CEC_OPCODE_VENDOR_COMMAND           0x89
#define CEC_OPCODE_VENDOR_COMMAND_WITH_ID   0xA0
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN 0x8A
#define CEC_OPCODE_VENDOR_REMOVE_BUTTON_UP  0x8B
//@}

//@{
/** @name Messages for the OSD Display Feature */
#define CEC_OPCODE_SET_OSD_STRING           0x64
//@}

//@{
/** @name Messages for the Device OSD Transfer Feature */
#define CEC_OPCODE_GIVE_OSD_NAME            0x46
#define CEC_OPCODE_SET_OSD_NAME             0x47
//@}

//@{
/** @name Messages for the Device Menu Control Feature */
#define CEC_OPCODE_MENU_REQUEST             0x8D
#define CEC_OPCODE_MENU_STATUS              0x8E
#define CEC_OPCODE_USER_CONTROL_PRESSED     0x44
#define CEC_OPCODE_USER_CONTROL_RELEASED    0x45
//@}

//@{
/** @name Messages for the Remote Control Passthrough Feature */
//@}

//@{
/** @name Messages for the Power Status Feature */
#define CEC_OPCODE_GIVE_DEVICE_POWER_STATUS 0x8F
#define CEC_OPCODE_REPORT_POWER_STATUS      0x90
//@}

//@{
/** @name Messages for General Protocol messages */
#define CEC_OPCODE_FEATURE_ABORT            0x00
#define CEC_OPCODE_ABORT                    0xFF
//@}

//@{
/** @name Messages for the System Audio Control Feature */
#define CEC_OPCODE_GIVE_AUDIO_STATUS        0x71
#define CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS 0x7D
#define CEC_OPCODE_REPORT_AUDIO_STATUS      0x7A
#define CEC_OPCODE_SET_SYSTEM_AUDIO_MODE    0x72
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST 0x70
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS 0x7E
//@}

//@{
/** @name Messages for the Audio Rate Control Feature */
#define CEC_OPCODE_SET_AUDIO_RATE           0x9A
//@}


//@{
/** @name CEC Operands */

//TODO: not finished

#define CEC_DECK_CONTROL_MODE_STOP      0x03
#define CEC_PLAY_MODE_PLAY_FORWARD      0x24
#define CEC_PLAY_MODE_PLAY_STILL        0x25	// PAUSE

// ...

#define CEC_USER_CONTROL_MODE_SELECT			0x00
#define CEC_USER_CONTROL_MODE_UP				0x01
#define CEC_USER_CONTROL_MODE_DOWN				0x02
#define CEC_USER_CONTROL_MODE_LEFT				0x03
#define CEC_USER_CONTROL_MODE_RIGHT				0x04
#define CEC_USER_CONTROL_MODE_RIGHT_UP			0x05
#define CEC_USER_CONTROL_MODE_RIGHT_DOWN		0x06
#define CEC_USER_CONTROL_MODE_LEFT_UP			0x07
#define CEC_USER_CONTROL_MODE_LEFT_DOWN			0x08

#define CEC_USER_CONTROL_MODE_ROOT_MENU			0x09
#define CEC_USER_CONTROL_MODE_SETUP_MENU		0x0A
#define CEC_USER_CONTROL_MODE_CONTENT_MENU		0x0B
#define CEC_USER_CONTROL_MODE_FAVORITE_MENU		0x0C
#define CEC_USER_CONTROL_MODE_EXIT				0x0D

#define CEC_USER_CONTROL_MODE_NUMBER_0			0x20
#define CEC_USER_CONTROL_MODE_NUMBER_1			0x21
#define CEC_USER_CONTROL_MODE_NUMBER_2			0x22
#define CEC_USER_CONTROL_MODE_NUMBER_3			0x23
#define CEC_USER_CONTROL_MODE_NUMBER_4			0x24
#define CEC_USER_CONTROL_MODE_NUMBER_5			0x25
#define CEC_USER_CONTROL_MODE_NUMBER_6			0x26
#define CEC_USER_CONTROL_MODE_NUMBER_7			0x27
#define CEC_USER_CONTROL_MODE_NUMBER_8			0x28
#define CEC_USER_CONTROL_MODE_NUMBER_9			0x29

#define CEC_USER_CONTROL_MODE_DOT				0x2A

#define CEC_USER_CONTROL_MODE_CHANNEL_UP		0x30
#define CEC_USER_CONTROL_MODE_CHANNEL_DOWN		0x31

#define	CEC_USER_CONTROL_MODE_INPUT_SELECT		0x34

#define CEC_USER_CONTROL_MODE_VOLUME_UP			0x41
#define CEC_USER_CONTROL_MODE_VOLUME_DOWN		0x42

#define CEC_USER_CONTROL_MODE_MUTE				0x43
#define CEC_USER_CONTROL_MODE_PLAY				0x44
#define CEC_USER_CONTROL_MODE_STOP				0x45
#define CEC_USER_CONTROL_MODE_PAUSE				0x46
#define CEC_USER_CONTROL_MODE_RECORD			0x47
#define CEC_USER_CONTROL_MODE_REWIND		0x48
#define CEC_USER_CONTROL_MODE_FAST_FORWARD	0x49
#define CEC_USER_CONTROL_MODE_EJECT				0x4A
#define CEC_USER_CONTROL_MODE_FORWARD			0x4B
#define CEC_USER_CONTROL_MODE_BACKWARD			0x4C

#define CEC_USER_CONTROL_F1						0x71
#define CEC_USER_CONTROL_F2						0x72
#define CEC_USER_CONTROL_F3						0x73
#define CEC_USER_CONTROL_F3						0x74

#define CEC_POWER_STATUS_ON						0x00
#define CEC_POWER_STATUS_STANBY					0x01
#define CEC_POWER_STATUS_INTRANSITIONSTANBYON	0x02
#define CEC_POWER_STATUS_INTRANSITIONONTOSTANBY	0x03



/*****************************************************************************
*
* IR remote controller scancode
*
******************************************************************************/
	#define SCAN_POWER			0x0001

	#define SCAN_PWR			0x8877

	#define SCAN_NUM_1			0xC43B
	#define SCAN_NUM_2			0xA45B
	#define SCAN_NUM_3			0xE41B
	#define SCAN_NUM_4			0x946B
	#define SCAN_NUM_5			0xD42B
	#define SCAN_NUM_6			0xB44B
	#define SCAN_NUM_7			0xF40B
	#define SCAN_NUM_8			0x8C73
	#define SCAN_NUM_9			0xCC33
	#define SCAN_NUM_0			0x847B
	#define SCAN_NUM_MINUS		0x9966
	#define SCAN_NUM_PREV		0xAC53

	#define SCAN_MUTE			0xC837
	#define SCAN_INPUT			0xE817

	#define SCAN_VOLUP			0xA05F
	#define SCAN_VOLDN			0xE01F

	#define SCAN_CH_UP			0x807F
	#define SCAN_CH_DN			0xC03F

	#define SCAN_SLEEP			0xB847
	#define SCAN_CANCEL			0xED12
	#define SCAN_MENU			0xE11E
	#define SCAN_GUIDE			0xCAB5

	#define SCAN_UP				0x817E
	#define SCAN_DOWN			0xC13E
	#define SCAN_LEFT			0xF00F
	#define SCAN_RIGHT			0xB04F
	#define SCAN_CENTER			0x916E

	#define SCAN_AUTO_CHANNEL	0x956A
	#define SCAN_ADD_DELETE		0xD52A
	#define SCAN_SIZE			0xF708
	#define SCAN_MULTI_SOUND	0xA857

	#define SCAN_STOP			0xC6B9
	#define SCAN_PLAY			0x86F9
	#define SCAN_PAUSE			0xAED1
	#define SCAN_REC			0xDEA1
	#define SCAN_FB				0xB8C7
	#define SCAN_FF				0xF887
	#define SCAN_CURRENT			0xBCC3
	


/*****************************************************************************
*
* External Variables
*
******************************************************************************/
#define NO_KEY		0

// Scan-code mapping for keypad
typedef struct {
	unsigned short	rcode;
	unsigned char	vkcode;
}SCANCODE_MAPPING;


static SCANCODE_MAPPING key_mapping[] = 
{
	{SCAN_PWR, 		107},

	{SCAN_NUM_1,		2},		// 1
	{SCAN_NUM_2,		3},		// 2
	{SCAN_NUM_3,		4},		// 3
	{SCAN_NUM_4,		5},		// 4
	{SCAN_NUM_5,		6},		// 5
	{SCAN_NUM_6,		7},		// 6
	{SCAN_NUM_7,		8},		// 7
	{SCAN_NUM_8,		9},		// 8
	{SCAN_NUM_9,		10},	// 9
	{SCAN_NUM_0,		11},	// 0
	{SCAN_NUM_MINUS,	12},	// MINUS
	{SCAN_NUM_PREV,		52},	// PERIOD

	{SCAN_MUTE,			113},	// MUTE
	{SCAN_INPUT,		61},	// F1

	{SCAN_VOLUP,		115},	// VOLUME_UP
	{SCAN_VOLDN,		114},	// VOLUME_DOWN

	{SCAN_CH_UP,		104},	// PAGE_UP
	{SCAN_CH_DN,		109},	// PAGE_DOWN

	{SCAN_SLEEP,		68},	// F10
	{SCAN_CANCEL,		158},	// BACK
	{SCAN_MENU,			127},	// MENU
	{SCAN_GUIDE,		172},	// HOME

	{SCAN_UP,			103},	// DPAD_UP
	{SCAN_DOWN,			108},	// DPAD_DOWN
	{SCAN_LEFT,			105},	// DPAD_LEFT
	{SCAN_RIGHT,		106},	// DPAD_RIGHT
	{SCAN_CENTER,		28},	// ENTER

	{SCAN_AUTO_CHANNEL,	60},	// F2
	{SCAN_ADD_DELETE,	14},	// DEL
	{SCAN_SIZE,			87},	// F11
	{SCAN_MULTI_SOUND,	88},	// F12

	{SCAN_STOP, 		166},	// MEDIA_STOP
	{SCAN_PLAY,			164},	// MEDIA_PLAY_PAUSE
	{SCAN_PAUSE,		164},	// MEDIA_PLAY_PAUSE
	{SCAN_REC,			167},	// NOTIFICATION

	{SCAN_FB,			168},	// MEDIA_REWIND
	{SCAN_FF,			208},	// MEDIA_FAST_FORWARD

	{SCAN_CURRENT,		15},	// TAB
};



/*****************************************************************************
*
* Function define
*
******************************************************************************/
extern int TccCECInterface_Init(void);
extern int TccCECInterface_GetKeyCode(unsigned short kc);
extern int TccCECInterface_SendData(unsigned int uiOpcode, unsigned int uiData);
extern unsigned int TccCECInterface_ConvertKeyCode(unsigned int uiOpcode, unsigned int uiData);
extern int TccCECInterface_IgnoreMessage(unsigned char opcode, unsigned char lsrc);
extern int TccCECInterface_CheckMessageSize(unsigned char opcode, int size);
extern int TccCECInterface_CheckMessageMode(unsigned char opcode, int broadcast);
extern int TccCECInterface_ParseMessage(char *buffer, int size);

#endif /* end of __TCC_CEC_INTERFACE_H__ */

