// ---------------------------------------------------------------------------
// Copyright 2009-2010 (c) RDA Microelectronics, Inc.
//
// File: rda5888fm.h
// Revision: 0
// Author: wuhp
// Date: 2010-01-27
// Description: 
// ---------------------------------------------------------------------------
#ifndef __RDAMTV_FM_H__
#define __RDAMTV_FM_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "rda5888drv.h"

#ifdef RDA5888_FM_ENABLE
void RDA5888FM_Init(void);
uint16 RDA5888FM_GetRSSI(void);
void RDA5888FM_SetFreq(int16 curf);
uint8 RDA5888FM_GetSigLevel(int16 curf);
uint8 RDA5888FM_IsValidFreq(int16 freq);
void RDA5888FM_SetMute(uint8 mute);
bool RDA5888FM_IsValidChip(void);
#endif

#ifdef __cplusplus
}
#endif
#endif
