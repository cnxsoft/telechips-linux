// ---------------------------------------------------------------------------
// Copyright 2009-2010 (c) RDA Microelectronics, Inc.
//
// File: rda5888sprd.h
// Revision: 0
// Author: wuhp
// Date: 2010-01-27
// Description: 
// ---------------------------------------------------------------------------
#ifndef __RDA5888API_H__
#define __RDA5888API_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "linux/kernel.h"
#include "rda5888drv.h"

extern uint8 g_nRdamtvTimer;

void RDAEXT_DelayMs(uint32 ms);

#define RDAEXT_DebugPrint	printk	//kal_prompt_trace

#define RDAMTV_APP_MODULE	1
#define CAMERA_APP_MODULE	0

#define ATV_STATE	1
#define FM_STATE	0

typedef enum
{
	RDAMTV_POWER_OFF,
	RDAMTV_POWER_ON
}  RDAMTV_POWER_STATE;

/*functions for MMI/ISP/MEDIA*/
unsigned char rdamtv_if_get_cur_module(void);
void rdamtv_if_set_cur_module(unsigned char is_atv_app);
unsigned short rdamtv_if_get_chn_count(void);
void rdamtv_if_func_config(void);


#ifdef __cplusplus
}
#endif

#endif

