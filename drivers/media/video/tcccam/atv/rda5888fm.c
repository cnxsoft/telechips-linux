// ---------------------------------------------------------------------------
// Copyright 2009-2010 (c) RDA Microelectronics, Inc.
//
// File: rda5888fm.c
// Revision: 0
// Author: wuhp
// Date: 2010-01-27
// Description: 
// ---------------------------------------------------------------------------

#include <linux/string.h>
#include "rda5888app.h"

#ifdef RDA5888_FM_ENABLE
#include "rda5888fm.h"

#include "rda5888mtk.h"

#if defined(__MTK_TARGET__)
#include "rda5888mtk.h"
#elif defined(SPRD_CUSTOMER)
#include "rda5888sprd.h"
#endif

// for rda5888d
#define FM_RSSI_OFFSET 	          57
#define FM_SEEK_TIMER             20
#define FM_SEEK_THRESHOLD          5

// for rda5888s
#define FM_SEEK_THRESHOLD2        67 // default: 65, range: 61~68.
#define FM_SEEK_CHECK_TIMES        2

void RDA_FMEnable(void)
{
	RDA5888_FieldWriteReg(0x1e0, 0x0002, 0); //reg:0x1e0[1]->0
	//TLG_Delay(20);
	RDA5888_FieldWriteReg(0x1e0, 0x0002, 1); //reg:0x1e0[1]->1
}

int RDA_FMIsReady(void)
{
    uint32 i;
	uint16 reg, seek_ready;

	for (i = 0; i < 10; i++)
	{
		seek_ready = RDA5888_FieldReadReg(0x1fc, 0x1000);
		if (seek_ready)
			break;
		RDAEXT_DelayMs(5);
	}

	RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDA_FMIsReady, i = %d\n", i);

	return seek_ready;
}

uint16 RDA5888FM_GetRSSI(void)
{
#if (RDA5888_CHIP_VER == 4)
    uint16 reg, i, data1, data2, data3, rssi[3];

    for (i = 0; i < 3; i++)
	{
	    // 将寄存器12FH写成000ah，读取117H的14~8bit的值，记为D1，再将寄存器12FH写成0019h，
	    // 读取117H的14~8bit的值，记为D2；
	    RDA5888_WriteReg(0x12F,0x000a);
		data1 = RDA5888_FieldReadReg(0x117, 0x7f00);
		
	    RDA5888_WriteReg(0x12F,0x0019);
		data2 = RDA5888_FieldReadReg(0x117, 0x7f00);

		//读取96H的2~0bit的值，此值即为档位值，根据此值获得档值对应的D3?
		data3 = RDA5888_FieldReadReg(0x96, 0x0007);
		
		switch (data3)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				data3 = 10;
				break;
				
			case 5:
				data3 = 28;
			    break;
				
			case 6:
				data3 = 44;
				break;
				
			case 7:
				data3 = 55;
				break;

			default:
				break;
		}
		rssi[i]= data1 - data2 - data3 - FM_RSSI_OFFSET + 107;
	}

	return ((rssi[0]+rssi[1]+rssi[2])/3);
#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6)) 
    uint16 rssi;

	rssi = RDA5888_FieldReadReg(0x1ff, 0x3f80);

	return rssi;
#endif	
}

void RDA5888FM_Init(void)
{
	RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_Init");

    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]CHIP_VER: %d, LIB_VER: %d.%d.%d", RDA5888_CHIP_VER, 
		RDA5888DRV_VER_MAJOR, RDA5888DRV_VER_MINOR, RDA5888DRV_VER_PATCH);
	
    memset(&g_stRdamtvCtx, 0x00, sizeof(rdamtv_ctx_t));
	
    g_stRdamtvCtx.work_mode = RDAMTV_MODE_FM_RADIO;

    RDA5888_DCDCInit();

	RDA5888FM_SetFreq(870);
	RDAEXT_DelayMs((uint16)20);

	RDA5888_DSPInit();
	
#ifdef RDA5888_26MCRYSTAL_ENABLE
	RDA5888_26MCrystalInit();
#endif

#if (RDA5888_CHIP_VER == 4)
	//将寄存器183H的3bit置为1，寄存器105H的15bit置为1，向寄存器121H写入0020h；
	RDA5888_FieldWriteReg(0x183, 0x0008, 0x1); //reg:0x183[3]->1
	RDA5888_FieldWriteReg(0x105, 0x8000, 0x1); //reg:0x105[15]->1
	RDA5888_WriteReg(0x121,0x0020);
#elif (RDA5888_CHIP_VER == 5)
    // set fm seek threshold
	//RDA5888_WriteReg(0x1f4, 0x20ad); // 9'h1f4,16'h1d2d; bits <13:7>
	RDA5888_FieldWriteReg(0x1f4, 0x3f8, FM_SEEK_THRESHOLD2); // 9'h1f4,16'h1d2d; bits <13:7>
#elif (RDA5888_CHIP_VER == 6)
	//item4: FM mode
	RDA5888_WriteReg(0x103,0x020d); // 9'h103,16'h020d
	RDA5888_WriteReg(0x11e,0x61e2); // 9'h11e,16'h61e2

	// set fm seek threshold
	RDA5888_FieldWriteReg(0x1f4, 0x3f8, FM_SEEK_THRESHOLD2); // 9'h1f4,16'h1d2d; bits <13:7>
#endif
}

// 99MHZ, 0x0a = 0xc537, 0x0b = 0x0168
void RDA5888FM_SetFreq(int16 curf )
{
    uint16 rega, regb;
	uint32 c_freq = curf;
	
    c_freq *= 100; // convert to khz.
	c_freq -= 3875; // couvert to center frequency.

    //RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_SetFreq, rega = %x, regb = %x", rega, regb);

#if (RDA5888_CHIP_VER == 6)
	//set center freq
	c_freq = (c_freq << 10);
	regb = (uint16)((c_freq&0xffff0000)>>16);
	rega = (uint16)((c_freq&0x0000ffff));
	
	RDA5888_WriteReg(0x033, rega);
	RDA5888_WriteReg(0x032, regb);
#else
#ifdef RDA5888_26MCRYSTAL_ENABLE
	//set center freq
	regb = (uint16)(((unsigned long)c_freq<<8)/65000);
	rega = (uint16)((((unsigned long)((c_freq<<8) - (regb * 65000))) << 16) /65000);
#else
	//set center freq
	regb = (uint16)(((unsigned long)c_freq<<8)/67500);
	rega = (uint16)((((unsigned long)((c_freq<<8) - (regb * 67500))) << 16) /67500);
#endif

	RDA5888_WriteReg(0x0a, rega);
	RDA5888_WriteReg(0x0b, regb);
#endif


    // set rx on
#if (RDA5888_CHIP_VER == 6)
	RDA5888_FieldWriteReg(0x030, 0x0008, 0x0);
	RDA5888_FieldWriteReg(0x030, 0x0008, 0x1);
#else
	RDA5888_FieldWriteReg(0x09e, 0x0001, 0x0);
	RDA5888_FieldWriteReg(0x09e, 0x0001, 0x1);
#endif
	RDAEXT_DelayMs(5);
}

void RDA5888FM_SetMute(uint8 cMute)
{
    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_SetMute");

#if (RDA5888_CHIP_VER == 4)	
	if (cMute)
	{
	    RDA5888_WriteReg(0x10,0x0000);  // close adac  0xc500
	}
	else
	{
	    RDA5888_WriteReg(0x10,0x8500);  // open adac
	}
#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6))
	if (cMute)
	{
		RDA5888_FieldWriteReg(0x1e0, 0x0200, 1);
	}
	else
	{
		RDA5888_FieldWriteReg(0x1e0, 0x0200, 0);
	}
#endif
}

uint8 RDA5888FM_GetSigLevel(int16 curf)
{
    uint8 rssi = 0;
	
    if (875 == curf)
    {
		rssi = 1;
    }
	else
	{
        rssi = RDA5888FM_GetRSSI();
	}

	RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_GetSigLevel, rssi = %d", rssi);

	return rssi;
}

uint8 RDA5888FM_IsValidFreq(int16 freq)
{
#if (RDA5888_CHIP_VER == 4)
    uint16 rssi_1, rssi_2, rssi_3;

    //RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_IsValidFreq");
	
	// get rssi_1
    RDA5888FM_SetFreq(freq-1);
	RDAEXT_DelayMs(FM_SEEK_TIMER);
	rssi_1 = RDA5888FM_GetRSSI();

	// get rssi_2
    RDA5888FM_SetFreq(freq);
	RDAEXT_DelayMs(FM_SEEK_TIMER);
	rssi_2 = RDA5888FM_GetRSSI();

	// get rssi_3
    RDA5888FM_SetFreq(freq+1);
	RDAEXT_DelayMs(FM_SEEK_TIMER);
	rssi_3 = RDA5888FM_GetRSSI();

	//RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_IsValidFreq, rssi: %d, %d, %d", rssi_1, rssi_2, rssi_3);

	// remove 96.0MHZ channel
	if (freq-1 == 960)
	{
	    rssi_1 = 0;
	}
	else if (freq == 960)
	{
	    rssi_2 = 0;
	}
	else if (freq+1 == 960)
	{
	    rssi_3 = 0;
	}

	if (((rssi_2 - rssi_1) >= FM_SEEK_THRESHOLD)
		&&((rssi_2 - rssi_3) >= FM_SEEK_THRESHOLD))
	{
	    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_IsValidFreq, freq = %d, rssi = %d!!!", freq, rssi_2);
	    return 1;
	}
	else
	{
	    return 0;
	}

#elif ((RDA5888_CHIP_VER == 5) || (RDA5888_CHIP_VER == 6))
    uint16 reg, seek_done, temp, cnt;

    RDA5888FM_SetFreq(freq);

	// check seek_done
	cnt = 0;
	do
	{
	    RDA_FMEnable();
		if(!RDA_FMIsReady())
			return 0; // wait seek ready timeout.
			
	    seek_done = RDA5888_FieldReadReg(0x1fc, 0x0800);
        if ((freq == 945) || (freq == 960) || (freq == 1080))
        {
			seek_done &= RDA5888_FieldReadReg(0x1fc, 0x0004);;
			break;
        }
		else
		{
		    if (seek_done)
		    {
		        break; // done!!!
		    }
		}
	} while(++cnt < FM_SEEK_CHECK_TIMES);

	RDAEXT_DebugPrint(RDA5888_TRACE_MOD "FMDrv_ValidStop, freq = %d, seek_done = %d, reg = %x\n", freq, seek_done, reg);

	return seek_done;
#endif	
}

bool RDA5888FM_IsValidChip(void)
{
    uint16 reg;

	RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_IsValidChip");
	
	RDA5888_ReadReg(0x0,&reg);
	
	if( (0x31E2 == reg) || (0x5888 == reg))
	{
	    return TRUE;
	}
	else
	{
	    RDAEXT_DebugPrint(RDA5888_TRACE_MOD "[rdamtv]RDAFM_IsValidChip, check fail!");
	    return FALSE;
	}
}
#endif
