/*
 * hdmi cec interface driver
 *
 * Copyright (C) 2010 Telechips Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */


#include <linux/init.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/capability.h>
#include <linux/uio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <linux/platform_device.h>    
#include <linux/input.h>       
#include <linux/sched.h>
#include <asm/io.h>
#include <linux/device.h>

#include "tcc_cec_interface.h"

#define CEC_INTERFACE_DEBUG 0

#if CEC_INTERFACE_DEBUG
#define DPRINTK(args...)    printk(args)
#else
#define DPRINTK(args...)
#endif

#if 0
#define	DEVICE_NAME		"tcc_cec"
#define TCC_CECVERSION 0.001
#endif /* 0 */

#define DEVICE_NAME	"tcc-remote"
#define TCC_IRVERSION 0.001

struct tcc_remote
{
    struct input_dev *dev;  
    struct work_struct work_q;
};

#if 0
typedef struct
{
	unsigned int BitCnt;	//Data Bit Count
	unsigned int Buf;		//Data Buffer
	unsigned int Id;		//Remocon ID
	unsigned int Code;		//Parsing Return Value
	unsigned int Val;		//Parsing Value
	unsigned int Prev;		//Previous Data
	unsigned int RepKey;	//Repeat key		: 0 or 1
	unsigned int T1,T2;		//Repeat Key Time Check
	unsigned int Stat;		//Remocon Status
	unsigned int PwrOff;	//Power Off Mode	: 0 or 1
}REM_DATA;


static REM_DATA Rem;
#endif /* 0 */

static struct input_dev *input_dev;
static struct tcc_remote *rem;

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int TccCECInterface_Init(void);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			None
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101222	11:46:24
**************************************************************************/
int TccCECInterface_Init(void)
{
	int ret;
	unsigned int i;
	
	DPRINTK(KERN_INFO "%s Start\n", __FUNCTION__);

	rem = kzalloc(sizeof(struct tcc_remote), GFP_KERNEL);
	input_dev = input_allocate_device();

	if (!rem || !input_dev)
	{
		DPRINTK(KERN_INFO "%s init fail\n", __FUNCTION__);
		input_free_device(input_dev);

		return -1;
	}
	
	//platform_set_drvdata(pdev, rem);

	rem->dev = input_dev;

#if 0
	rem->dev->name = "telechips cec interface driver";
	rem->dev->phys = DEVICE_NAME;
	rem->dev->evbit[0] = BIT(EV_KEY);
	rem->dev->id.version = TCC_CECVERSION;
#endif /* 0 */
	rem->dev->name = "telechips remote controller";
	rem->dev->phys = DEVICE_NAME;
	rem->dev->evbit[0] = BIT(EV_KEY);
	rem->dev->id.version = TCC_IRVERSION;

	//input_dev->dev.parent = &pdev->dev;
	for (i = 0; i < ARRAY_SIZE(key_mapping); i++)
	{
		set_bit(key_mapping[i].vkcode & KEY_MAX, rem->dev->keybit);
	}

    ret = input_register_device(rem->dev);
    if (ret) 
    {
		DPRINTK(KERN_INFO "%s input_register_device fail\n", __FUNCTION__);
		return -1;
    }

	DPRINTK(KERN_INFO "%s End\n", __FUNCTION__);
	
	return 1;

}

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int TccCECInterface_GetKeyCode(unsigned short kc);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			kc	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101222	11:48:52
**************************************************************************/
int TccCECInterface_GetKeyCode(unsigned short kc)
{
    int i;
	for (i = 0;i < sizeof(key_mapping)/sizeof(SCANCODE_MAPPING);i++)
      if (kc == key_mapping[i].rcode) 
            return key_mapping[i].vkcode;
    return -1;
}


/**************************************************************************
*  FUNCTION NAME : 
*  
*      int TccCECInterface_SendData(unsigned int uiData);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			uiData	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101222	11:46:17
**************************************************************************/
int TccCECInterface_SendData(unsigned int uiOpcode, unsigned int uiData)
{
	DPRINTK(KERN_INFO "%s\n", __FUNCTION__);
	
    struct input_dev *dev = rem->dev;
	int nRem;
	unsigned int	uiKeyCode;

	uiKeyCode = TccCECInterface_ConvertKeyCode(uiOpcode, uiData);

	DPRINTK(KERN_INFO "uiKeyCode = 0x%x\n", uiKeyCode);
	
	nRem = TccCECInterface_GetKeyCode(uiKeyCode);
	if(nRem == -1)
		return;

	DPRINTK(KERN_INFO "nRem = 0x%x\n", nRem);

	input_report_key(dev, nRem, 1);
    input_sync(dev);
	
	input_report_key(dev, nRem, 0);
    input_sync(dev);

	return 1;
}


/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned int TccCECInterface_ConvertKeyCode(unsigned int uiData);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			uiData	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101222	15:1:45
**************************************************************************/
unsigned int TccCECInterface_ConvertKeyCode(unsigned int uiOpcode, unsigned int uiData)
{
	unsigned int uiKeyCode;

	if( uiOpcode == CEC_OPCODE_PLAY)
	{
		switch(uiData)
		{
			case CEC_PLAY_MODE_PLAY_FORWARD:
				uiKeyCode = SCAN_PLAY;
				break;
			case CEC_PLAY_MODE_PLAY_STILL:
				uiKeyCode = SCAN_PAUSE;
				break;
			default:
				uiKeyCode = 0;
				break;
		}
	}
	else if( uiOpcode == CEC_OPCODE_DECK_CONTROL)
	{
		switch(uiData)
		{
			case CEC_DECK_CONTROL_MODE_STOP:
				uiKeyCode = SCAN_STOP;
				break;
			default:
				uiKeyCode = 0;
				break;
		}
	}
	else if( uiOpcode == CEC_OPCODE_USER_CONTROL_PRESSED)
	{
		switch(uiData)
		{
			case CEC_USER_CONTROL_MODE_SELECT:
				uiKeyCode = SCAN_CENTER;
				break;
			case CEC_USER_CONTROL_MODE_UP:
				uiKeyCode = SCAN_UP;
				break;
			case CEC_USER_CONTROL_MODE_DOWN:
				uiKeyCode = SCAN_DOWN;
				break;
			case CEC_USER_CONTROL_MODE_LEFT:
				uiKeyCode = SCAN_LEFT;
				break;
			case CEC_USER_CONTROL_MODE_RIGHT:
				uiKeyCode = SCAN_RIGHT;
				break;
			case CEC_USER_CONTROL_MODE_EXIT:
				uiKeyCode = SCAN_CANCEL;
				break;
			case CEC_USER_CONTROL_MODE_PLAY:
				uiKeyCode = SCAN_PLAY;
				break;
			case CEC_USER_CONTROL_MODE_PAUSE:
				uiKeyCode = SCAN_PAUSE;
				break;
			case CEC_USER_CONTROL_MODE_STOP:
				uiKeyCode = SCAN_STOP;
				break;
			case CEC_USER_CONTROL_MODE_FORWARD:
			case CEC_USER_CONTROL_MODE_FAST_FORWARD:
				uiKeyCode = SCAN_FF;
				break;
			case CEC_USER_CONTROL_MODE_BACKWARD:
			case CEC_USER_CONTROL_MODE_REWIND:
				uiKeyCode = SCAN_FB;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_0:
				uiKeyCode = SCAN_NUM_0;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_1:
				uiKeyCode = SCAN_NUM_1;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_2:
				uiKeyCode = SCAN_NUM_2;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_3:
				uiKeyCode = SCAN_NUM_3;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_4:
				uiKeyCode = SCAN_NUM_4;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_5:
				uiKeyCode = SCAN_NUM_5;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_6:
				uiKeyCode = SCAN_NUM_6;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_7:
				uiKeyCode = SCAN_NUM_7;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_8:
				uiKeyCode = SCAN_NUM_8;
				break;
			case CEC_USER_CONTROL_MODE_NUMBER_9:
				uiKeyCode = SCAN_NUM_9;
				break;
			case CEC_USER_CONTROL_MODE_DOT:
				//uiKeyCode = SCAN_NUM_MINUS;	//minus
				uiKeyCode = SCAN_NUM_PREV;		//dot
				break;
			default:
				uiKeyCode = 0;
				break;
		}
	}
	
	return uiKeyCode;
}


/**************************************************************************
*  FUNCTION NAME : 
*  
*      int TccCECInterface_IgnoreMessage(unsigned char opcode, unsigned char lsrc);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			lsrc	= 
*			opcode	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101224	11:58:14
**************************************************************************/
int TccCECInterface_IgnoreMessage(unsigned char opcode, unsigned char lsrc)
{
    int retval = 0;

    /* if a message coming from address 15 (unregistered) */
    if (lsrc == CEC_LADDR_UNREGISTERED) {
        switch (opcode) {
            case CEC_OPCODE_DECK_CONTROL:
            case CEC_OPCODE_PLAY:
                retval = 1;
            default:
                break;
        }
    }

    return retval;
}

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int TccCECInterface_CheckMessageSize(unsigned char opcode, int size);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			opcode	= 
*			size	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101224	11:58:8
**************************************************************************/
int TccCECInterface_CheckMessageSize(unsigned char opcode, int size)
{
    int retval = 1;

    switch (opcode) {
        case CEC_OPCODE_PLAY:
        case CEC_OPCODE_DECK_CONTROL:
		case CEC_OPCODE_USER_CONTROL_PRESSED:
        case CEC_OPCODE_SET_MENU_LANGUAGE:
            if (size != 3) retval = 0;
            break;
		case CEC_OPCODE_USER_CONTROL_RELEASED:
            if (size != 2) retval = 0;
            break;
        case CEC_OPCODE_FEATURE_ABORT:
            if (size != 4) retval = 0;
            break;
        default:
            break;
    }

    return retval;
}


/**************************************************************************
*  FUNCTION NAME : 
*  
*      int TccCECInterface_CheckMessageMode(unsigned char opcode, int broadcast);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			broadcast	= 
*			opcode	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101224	11:57:59
**************************************************************************/
int TccCECInterface_CheckMessageMode(unsigned char opcode, int broadcast)
{
    int retval = 1;

    switch (opcode) {
        case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:
        case CEC_OPCODE_SET_MENU_LANGUAGE:
            if (!broadcast) retval = 0;
            break;
        case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
        case CEC_OPCODE_DECK_CONTROL:
        case CEC_OPCODE_PLAY:
		case CEC_OPCODE_USER_CONTROL_PRESSED:
		case CEC_OPCODE_USER_CONTROL_RELEASED:
        case CEC_OPCODE_FEATURE_ABORT:
        case CEC_OPCODE_ABORT:
            if (broadcast) retval = 0;
            break;
        default:
            break;
    }

    return retval;
}



/**************************************************************************
*  FUNCTION NAME : 
*  
*      int TccCECInterface_ParseMessage(char *buffer, int size);
*  
*  DESCRIPTION : You can add file description here.
*  
*  INPUT:
*			buffer	= 
*			size	= 
*  
*  OUTPUT:	int - Return Type
*  			= 
*  
*  REMARK:	created on 20101224	11:57:52
**************************************************************************/
int TccCECInterface_ParseMessage(char *buffer, int size)
{
    unsigned char lsrc, ldst, opcode;

    lsrc = buffer[0] >> 4;

	DPRINTK(KERN_INFO "CEC lsrc %d buffer[1]:%d !!\n", lsrc , buffer[1]);

    opcode = buffer[1];

    if (TccCECInterface_IgnoreMessage(opcode, lsrc)) {
        DPRINTK(KERN_INFO "### ignore message coming from address 15 (unregistered)\n");
        return -1;
    }

    if (!TccCECInterface_CheckMessageSize(opcode, size)) {
        DPRINTK(KERN_INFO "### invalid message size ###\n");
        return -1;
    }

    /* check if message broadcast/directly addressed */
    if (!TccCECInterface_CheckMessageMode(opcode, (buffer[0] & 0x0F) == CEC_MSG_BROADCAST ? 1 : 0)) {
        DPRINTK(KERN_INFO "### invalid message mode (directly addressed/broadcast) ###\n");
        return -1;
    }

    ldst = lsrc;

//TODO: macroses to extract src and dst logical addresses
//TODO: macros to extract opcode

    switch (opcode) {

        case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
		{
			DPRINTK(KERN_INFO "### GIVE PHYSICAL ADDRESS ###\n");
			break;
        }
        case CEC_OPCODE_SET_MENU_LANGUAGE:
        {
            DPRINTK(KERN_INFO "the menu language will be changed!!!\n");
            break;
        }

        case CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:// TV
        case CEC_OPCODE_ACTIVE_SOURCE:        // TV, CEC Switches
        case CEC_OPCODE_ROUTING_CHANGE:        // CEC Switches
        case CEC_OPCODE_ROUTING_INFORMATION:    // CEC Switches
        case CEC_OPCODE_SET_STREAM_PATH:    // CEC Switches
        case CEC_OPCODE_SET_SYSTEM_AUDIO_MODE:    // TV
        case CEC_OPCODE_DEVICE_VENDOR_ID:    // ???
            break;

        case CEC_OPCODE_DECK_CONTROL:
            if (buffer[2] == CEC_DECK_CONTROL_MODE_STOP) {
                DPRINTK(KERN_INFO "### DECK CONTROL : STOP ###\n");
				TccCECInterface_SendData(CEC_OPCODE_DECK_CONTROL, CEC_DECK_CONTROL_MODE_STOP);
            }
            break;

        case CEC_OPCODE_PLAY:
            if (buffer[2] == CEC_PLAY_MODE_PLAY_FORWARD) {
                DPRINTK(KERN_INFO "### PLAY MODE : PLAY ###\n");
				TccCECInterface_SendData(CEC_OPCODE_PLAY, CEC_PLAY_MODE_PLAY_FORWARD);
            }
			else if (buffer[2] == CEC_PLAY_MODE_PLAY_STILL) {
				DPRINTK(KERN_INFO "### PAUSE MODE : PAUSE ###\n");

				TccCECInterface_SendData(CEC_OPCODE_PLAY, CEC_PLAY_MODE_PLAY_STILL);
			}
            break;

        case CEC_OPCODE_STANDBY:
            DPRINTK(KERN_INFO "### switching device into standby... ###\n");
            break;

		case CEC_OPCODE_USER_CONTROL_PRESSED:
			DPRINTK(KERN_INFO "CEC_OPCODE_USER_CONTROL_PRESSED : operation id = %d", buffer[2]);

			switch(buffer[2])
			{
				case CEC_USER_CONTROL_MODE_FAST_FORWARD:
				case CEC_USER_CONTROL_MODE_REWIND:

				case CEC_USER_CONTROL_MODE_SELECT:
				case CEC_USER_CONTROL_MODE_UP:
				case CEC_USER_CONTROL_MODE_DOWN:
				case CEC_USER_CONTROL_MODE_LEFT:
				case CEC_USER_CONTROL_MODE_RIGHT:
				case CEC_USER_CONTROL_MODE_EXIT:

				case CEC_USER_CONTROL_MODE_NUMBER_0:
				case CEC_USER_CONTROL_MODE_NUMBER_1:
				case CEC_USER_CONTROL_MODE_NUMBER_2:
				case CEC_USER_CONTROL_MODE_NUMBER_3:
				case CEC_USER_CONTROL_MODE_NUMBER_4:
				case CEC_USER_CONTROL_MODE_NUMBER_5:
				case CEC_USER_CONTROL_MODE_NUMBER_6:
				case CEC_USER_CONTROL_MODE_NUMBER_7:
				case CEC_USER_CONTROL_MODE_NUMBER_8:
				case CEC_USER_CONTROL_MODE_NUMBER_9:
				case CEC_USER_CONTROL_MODE_DOT:

					TccCECInterface_SendData(CEC_OPCODE_USER_CONTROL_PRESSED, buffer[2]);

					break;
				default:
					break;

			}
			break;

        case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:
            break;
        default:
			break;

    }

	return 1;
}


