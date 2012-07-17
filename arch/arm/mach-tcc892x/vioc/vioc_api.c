/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : Vioc_api.c
*
*  Description : VIOC user application interface Module
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/08     0.1            created                      hskim
*******************************************************************************/

#include <linux/kernel.h>
#include <mach/bsp.h>
#include <mach/io.h>

#include <mach/vioc_scaler.h>
#include <mach/vioc_ireq.h>
#include <mach/vioc_config.h>
#include <mach/vioc_plugin_tcc892x.h>
#include <mach/vioc_api.h>


/* Scaler Set */
/*
	API : VIOC_API_SCALER_Set/GetXXX()
*/
void VIOC_API_SCALER_SetConfig(unsigned int scaler, VIOC_SCALER_INFO_Type * Info)
{
	volatile PVIOC_SC pScaler;
	pScaler = (volatile PVIOC_SC)tcc_p2v((unsigned int)HwVIOC_SC0 + scaler*0x100);

	if(Info->BYPASS == TRUE) 	VIOC_SC_SetBypass(pScaler, TRUE);
 	else 						VIOC_SC_SetBypass(pScaler, FALSE);

	//VIOC_SC_SetSrcSize((VIOC_SC *)baseAddr, Info->SRC_WIDTH, Info->SRC_HEIGHT);
	VIOC_SC_SetDstSize(pScaler, Info->DST_WIDTH, Info->DST_HEIGHT);
	VIOC_SC_SetOutSize(pScaler, Info->OUTPUT_WIDTH, Info->OUTPUT_HEIGHT);
	VIOC_SC_SetOutPosition(pScaler, Info->OUTPUT_POS_X, Info->OUTPUT_POS_Y);
}

void VIOC_API_SCALER_SetUpdate(unsigned int scaler)
{
	volatile PVIOC_SC pScaler;
	pScaler = (volatile PVIOC_SC)tcc_p2v((unsigned int)HwVIOC_SC0 + scaler*0x100);

	VIOC_SC_SetUpdate(pScaler);
}

int VIOC_API_SCALER_SetInterruptEnable(unsigned int scaler, unsigned int interrupt)
{
	int ret = 0; // VIOC_DRIVER_NOERR;
	volatile PVIOC_SC pScaler;
	pScaler = (volatile PVIOC_SC)tcc_p2v((unsigned int)HwVIOC_SC0 + scaler*0x100);

	if(scaler >= VIOC_SCALER_MAX)
	{
		ret = -3; // VIOC_DRIVER_ERR_INVALID;
	}
	else
	{
		VIOC_SC_SetInterruptMask(pScaler, interrupt, FALSE);
	}

	return ret;
}

int VIOC_API_SCALER_SetInterruptDiable(unsigned int scaler, unsigned int interrupt)
{
	int ret = 0; // VIOC_DRIVER_NOERR;
	volatile PVIOC_SC pScaler;
	pScaler = (volatile PVIOC_SC)tcc_p2v((unsigned int)HwVIOC_SC0 + scaler*0x100);

	if(scaler >= VIOC_SCALER_MAX)
	{
		ret = -3; // VIOC_DRIVER_ERR_INVALID;
	}
	else
	{
		VIOC_SC_SetInterruptMask(pScaler, interrupt, TRUE);
	}

	return ret;
}

int VIOC_API_SCALER_SetPlugIn(unsigned int scaler, unsigned int path)
{
	int ret;

	ret = VIOC_CONFIG_PlugIn(scaler, path);
	if(ret == VIOC_DEVICE_CONNECTED)
		ret = VIOC_DRIVER_NOERR;
	else
		ret = VIOC_DRIVER_ERR;

	return ret;
}

int VIOC_API_SCALER_SetPlugOut(unsigned int scaler)
{
	int ret;

	ret = VIOC_CONFIG_PlugOut(scaler);
	if(ret == VIOC_PATH_DISCONNECTED)
		ret = VIOC_DRIVER_NOERR;
	else
		ret = VIOC_DRIVER_ERR;

	return ret;
}

void VIOC_API_IREQ_Init(void)
{
	VIOC_IREQ_RegisterHandle();
}



