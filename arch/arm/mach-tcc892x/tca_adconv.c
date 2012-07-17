/****************************************************************************
 *	 FileName	 : tca_adconv.c
 *	 Description :
 ****************************************************************************
*
 *	 TCC Version 1.0
 *	 Copyright (c) Telechips, Inc.
 *	 ALL RIGHTS RESERVED
*
 ****************************************************************************/
#include <mach/bsp.h>
#include <asm/mach-types.h>
#include <mach/tsadc.h>
#include <mach/tca_adconv.h>
#include <mach/tcc_fb.h>
#include "devices.h"

/************************************************************************************************
* Global Defines
************************************************************************************************/
#define MASK_ADCDATA10(n)   ( (n)&0x3FF )
#define MASK_ADCDATA12(n)   ( (n)&0xFFF )

 /************************************************************************************************
* Global Handle
************************************************************************************************/


/************************************************************************************************
* Type Define
************************************************************************************************/
#if defined(_LINUX_)
#include <linux/delay.h>
#define Sleep(time) msleep(time)
#endif


/************************************************************************************************
* Global Variable define
************************************************************************************************/
static PGPIO  pGPIO;
static PTSADC pTSADC;


/************************************************************************************************
* FUNCTION		:
*
* DESCRIPTION	:
*
************************************************************************************************/

unsigned int    tca_adc_adcinitialize(unsigned int devAddress, void* param )
{
    unsigned int    ret = 0;
    pTSADC = (PTSADC)devAddress;

    if(tcc_panel_id == PANEL_ID_AT070TN93 || tcc_panel_id == PANEL_ID_TD070RDH11)
	pTSADC->ADCDLY.nREG = ADC_DELAY(150);
    else if(tcc_panel_id == PANEL_ID_N101L6 || tcc_panel_id == PANEL_ID_CLAA102NA0DCW)
	pTSADC->ADCDLY.nREG = ADC_DELAY(200);
    else
	pTSADC->ADCDLY.nREG = ADC_DELAY(50);

    pTSADC->ADCCON.nREG = RESSEL_12BIT | PRESCALER_EN | PRESCALER_VAL(11)|Hw2;
    pTSADC->ADCTSC.nREG = ADCTSC_WAIT_PENDOWN;
    pTSADC->CLR_INT_EOC.nREG = CLEAR_ADC_INT;
    pTSADC->ADCUPDN.nREG = CLEAR_ADCWK_INT;

    return ret;
}

unsigned int    tca_adc_portinitialize(unsigned int devAddress, void* param)
{
    unsigned int    ret = 0;

     return ret;
}


unsigned int    tca_adc_read(unsigned int channel)
{
    unsigned int    ret = 0;
    unsigned int    uiCh = 0;
    switch(channel)
    {
#if 0
        case ADC_CHANNEL0:
            uiCh = SEL_MUX_AIN0;
            break;

        case ADC_CHANNEL1:
            uiCh = SEL_MUX_AIN1;
            break;
#endif

        case ADC_CHANNEL2:
            uiCh = SEL_MUX_AIN2;
            break;

        case ADC_CHANNEL3:
            uiCh = SEL_MUX_AIN3;
            break;

        case ADC_CHANNEL4:
            uiCh = SEL_MUX_AIN4;
            break;

        case ADC_CHANNEL5:
            uiCh = SEL_MUX_AIN5;
            break;

#if 0
        case ADC_CHANNEL6:
            uiCh = SEL_MUX_YM;
            break;

        case ADC_CHANNEL7:
            uiCh = SEL_MUX_YP;
            break;

        case ADC_CHANNEL8:
            uiCh = SEL_MUX_XM;
            break;

        case ADC_CHANNEL9:
            uiCh = SEL_MUX_XP;
            break;
#endif



    }
	BITCLR(pTSADC->ADCCON.nREG, Hw2);//wakeup


	if(channel <= ADC_CHANNEL5)
	{
		BITCLR(pTSADC->ADCCON.nREG ,SEL_MUX_MASK );
		mdelay(1);
	}

	
	BITSET(pTSADC->ADCCON.nREG ,(uiCh|ENABLE_START_EN) );
    while (pTSADC->ADCCON.nREG & ENABLE_START_EN)
    {	// Wait for Start Bit Cleared
		ndelay(5);
    }
    while (!(pTSADC->ADCCON.nREG & ECFLG_END))
    {	// Wait for ADC Conversion Ended
		ndelay(5);
   }

    ret = MASK_ADCDATA12( (pTSADC->ADCDAT0.nREG) );

    BITCSET(pTSADC->ADCCON.nREG, SEL_MUX_MASK ,ENABLE_START_EN);
    BITSET(pTSADC->ADCCON.nREG, Hw2);

    return ret;
}


unsigned int    tca_adc_tsread(int* x, int* y, int *xp, int *ym)
{

	BITCLR(pTSADC->ADCCON.nREG, Hw2);//wakeup
////////////////read Z2
	pTSADC->ADCTSC.nREG = Hw3|Hw4|Hw5;
	BITCSET(pTSADC->ADCCON.nREG,Hw6-Hw3,  Hw5|Hw4);
	pTSADC->ADCCON.nREG |= ENABLE_START_EN;	// ADC Conversion Start
	while (pTSADC->ADCCON.nREG & ENABLE_START_EN)
	{	// Wait for Start Bit Cleared
		;//Sleep(1);
	}

	while (!(pTSADC->ADCCON.nREG & ECFLG_END))
	{	// Wait for ADC Conversion Ended
		;//Sleep(1);
	}
		
	*ym= D_XPDATA_MASK12(pTSADC->ADCDAT0.nREG);

////////////////read Z1
	pTSADC->ADCTSC.nREG = Hw4|Hw5|Hw3;
	BITCSET(pTSADC->ADCCON.nREG,Hw6-Hw3,  Hw6|Hw3);
	pTSADC->ADCCON.nREG |= ENABLE_START_EN;	// ADC Conversion Start

	while (pTSADC->ADCCON.nREG & ENABLE_START_EN)
	{	// Wait for Start Bit Cleared
		;//Sleep(1);
	}

	while (!(pTSADC->ADCCON.nREG & ECFLG_END))
	{	// Wait for ADC Conversion Ended
		;//Sleep(1);
	}
		
	*xp= D_XPDATA_MASK12(pTSADC->ADCDAT0.nREG);
////////////////read x
	pTSADC->ADCTSC.nREG = Hw3|Hw0;
	//gpio setting - 30 high
	BITCSET(pTSADC->ADCCON.nREG,Hw6-Hw3, Hw5|Hw4|Hw3);
	pTSADC->ADCCON.nREG |= ENABLE_START_EN;	// ADC Conversion Start

	while (pTSADC->ADCCON.nREG & ENABLE_START_EN)
	{	// Wait for Start Bit Cleared
		;//Sleep(1);
	}

	while (!(pTSADC->ADCCON.nREG & ECFLG_END))
	{	// Wait for ADC Conversion Ended
		;//Sleep(1);
	}
		//read x value
	*x= D_XPDATA_MASK12(pTSADC->ADCDAT0.nREG);
//		pTSADC->ADCTSC = ADCTSC_AUTO_ADC5;


////////////////read y
	pTSADC->ADCTSC.nREG = Hw3|Hw1;
	//gpio setting - 31,28 high
	BITCSET(pTSADC->ADCCON.nREG,Hw6-Hw3,  Hw6|Hw3);
	pTSADC->ADCCON.nREG |= ENABLE_START_EN;	// ADC Conversion Start

	while (pTSADC->ADCCON.nREG & ENABLE_START_EN)
	{	// Wait for Start Bit Cleared
		;//Sleep(1);
	}

	while (!(pTSADC->ADCCON.nREG & ECFLG_END))
	{	// Wait for ADC Conversion Ended
		;//Sleep(1);
	}
	//read y value
	*y= D_XPDATA_MASK12(pTSADC->ADCDAT1.nREG);


//		pTSADC->ADCTSC = ADCTSC_AUTO_ADC5;



		//*ym=0;
	    pTSADC->ADCCON.bREG.STBY = 1;

	    pTSADC->CLR_INT_EOC.bREG.CLR = Hw0;
	    pTSADC->ADCUPDN.bREG.DOWN = Hw0;
	    pTSADC->ADCTSC.nREG |= ADCTSC_WAIT_PENDOWN;

		return 1;

}

unsigned int    tca_adc_powerdown(void)
{
    unsigned int    ret = 0;
    pTSADC->CLR_INT_EOC.bREG.CLR = Hw0;
    pTSADC->ADCUPDN.bREG.DOWN = Hw0;
    pTSADC->ADCTSC.nREG |= ADCTSC_WAIT_PENDOWN;

//    BITSET(pTSADC->ADCCON, STDBM_STANDBY);
    pTSADC->ADCCON.bREG.STBY = 1;
    return ret;
}

unsigned int    tca_adc_powerup(void)
{
    unsigned int    ret = 0;
//    BITCLR(pTSADC->ADCCON, STDBM_STANDBY); // Because STDBM_NORMAL is [ 0<<2 ]
    pTSADC->ADCCON.bREG.STBY = 0;
     return ret;

}
