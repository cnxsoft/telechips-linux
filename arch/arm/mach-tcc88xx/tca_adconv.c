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

    if(machine_is_m801_88())
    	pTSADC->ADCDLY = ADC_DELAY(150);
    else if(machine_is_m803())
	pTSADC->ADCDLY = ADC_DELAY(220);	
    else if(system_rev == 0x0610 || system_rev == 0x0613 || system_rev == 0x0614 || system_rev == 0x0615 || system_rev == 0x0620 || system_rev == 0x0621 || system_rev == 0x0622 || system_rev == 0x0623 || system_rev == 0x0624)
    	pTSADC->ADCDLY = ADC_DELAY(150);
    else
	pTSADC->ADCDLY = ADC_DELAY(50);

    pTSADC->ADCCON = RESSEL_12BIT | PRESCALER_EN | PRESCALER_VAL(11)|Hw2;
    pTSADC->ADCTSC = ADCTSC_WAIT_PENDOWN;
    pTSADC->ADCCLRINT = CLEAR_ADC_INT;
    pTSADC->ADCCLRUPDN = CLEAR_ADCWK_INT;

    return ret;
}

unsigned int    tca_adc_portinitialize(unsigned int devAddress, void* param)
{
    unsigned int    ret = 0;
//    pGPIO = (PGPIO)devAddress;
//    BITCSET(pGPIO->GPEFN3 , 0xFFFF0000, Hw30|Hw26|Hw22|Hw18 );  // Function AIN[0-3]
//    BITCLR(pGPIO->GPEEN, Hw32-Hw28);// 0xF0000000); // Port as Input
    // ADD for BAT, KEYPAD
//    BITCSET(pGPIO->GPGFN2 , 0x00000F00, Hw8);  // Function AIN[4]
//    BITCLR(pGPIO->GPGEN, Hw22); // for KEYPAD
//    BITSET(pGPIO->GPECD1,0xFF030000); // Strength Max

     return ret;
}


unsigned int    tca_adc_read(unsigned int channel)
{
    unsigned int    ret = 0;
    unsigned int    uiCh = 0;
    switch(channel)
    {
        case ADC_CHANNEL0:
            uiCh = SEL_MUX_AIN0;
            break;

        case ADC_CHANNEL1:
            uiCh = SEL_MUX_AIN1;
            break;

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

        case ADC_CHANNEL10:
            uiCh = SEL_MUX_AIN10;
            break;

        case ADC_CHANNEL11:
            uiCh = SEL_MUX_AIN11;
            break;

        case ADC_CHANNEL12:
            uiCh = SEL_MUX_AIN12;
            break;

        case ADC_CHANNEL13:
            uiCh = SEL_MUX_AIN13;
            break;

        case ADC_CHANNEL14:
            uiCh = SEL_MUX_AIN14;
            break;

        case ADC_CHANNEL15:
            uiCh = SEL_MUX_AIN15;
            break;

    }
	BITCLR(pTSADC->ADCCON, Hw2);//wakeup

	if(channel <= ADC_CHANNEL5 || channel >= ADC_CHANNEL10)
	{
		BITCLR(pTSADC->ADCCON ,SEL_MUX_MASK );
		mdelay(1);
	}
	BITSET(pTSADC->ADCCON ,(uiCh|ENABLE_START_EN) );
    while (pTSADC->ADCCON & ENABLE_START_EN)
    {	// Wait for Start Bit Cleared
		ndelay(5);
    }
    while (!(pTSADC->ADCCON & ECFLG_END))
    {	// Wait for ADC Conversion Ended
		ndelay(5);
   }

    ret = MASK_ADCDATA12( (pTSADC->ADCDAT0) );

    BITCSET(pTSADC->ADCCON, SEL_MUX_MASK ,ENABLE_START_EN);
    BITSET(pTSADC->ADCCON, Hw2);

    return ret;
}

unsigned int    tca_adc_tsautoread(int* xp, int* yp, int* xm, int* ym)
{
    unsigned int    ret = 0;

	BITCLR(pTSADC->ADCCON, Hw2);//wakeup
    pTSADC->ADCTSC = ADCTSC_WAIT_PENUP;
    pTSADC->ADCUPDN = Hw0;

    //BITSET( pTSADC->ADCTSC , XY_PST_XPOS);	// Auto Conversion
    //BITSET( pTSADC->ADCTSC , XY_PST_YPOS);	// Auto Conversion

    //#############################################################
    //read xp value
    BITCLR( pTSADC->ADCTSC , Hw4);
    BITSET( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion
    BITSET( pTSADC->ADCCON , ENABLE_START_EN );	// ADC Conversion Start

    while (pTSADC->ADCCON & ENABLE_START_EN)
    {	// Wait for Start Bit Cleared
        ;//Sleep(1);
    }

    while (!(pTSADC->ADCCON & ECFLG_END))
    {	// Wait for ADC Conversion Ended
        ;//Sleep(1);
    }
    pTSADC->ADCDAT0 = D_XY_PST_XPOS | D_AUTO_PST_EN;
    *xp= D_XPDATA_MASK12(pTSADC->ADCDAT0);

    BITCLR( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion

    //#############################################################
    //read xm value
    BITCLR( pTSADC->ADCTSC , Hw5);
    BITSET( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion
    BITSET( pTSADC->ADCCON , ENABLE_START_EN );	// ADC Conversion Start

    while (pTSADC->ADCCON & ENABLE_START_EN)
    {	// Wait for Start Bit Cleared
        ;//Sleep(1);
    }

    while (!(pTSADC->ADCCON & ECFLG_END))
    {	// Wait for ADC Conversion Ended
        ;//Sleep(1);
    }

    pTSADC->ADCDAT0 = D_XY_PST_XPOS | D_AUTO_PST_EN;
    *xm = D_XPDATA_MASK12(pTSADC->ADCDAT0);

    BITCLR( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion

    //#############################################################
    //read yp value
    BITCLR( pTSADC->ADCTSC , Hw6);
    BITSET( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion
    BITSET( pTSADC->ADCCON , ENABLE_START_EN );	// ADC Conversion Start

    while (pTSADC->ADCCON & ENABLE_START_EN)
    {	// Wait for Start Bit Cleared
        ;//Sleep(1);
    }

    while (!(pTSADC->ADCCON & ECFLG_END))
    {	// Wait for ADC Conversion Ended
        ;//Sleep(1);
    }

    pTSADC->ADCDAT1 = D_XY_PST_YPOS| D_AUTO_PST_EN;
    *yp= D_YPDATA_MASK12(pTSADC->ADCDAT1);

    BITCLR( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion

    //#############################################################
    //read ym value
    BITCLR( pTSADC->ADCTSC , Hw7);
    BITSET( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion
    BITSET( pTSADC->ADCCON , ENABLE_START_EN );	// ADC Conversion Start

    while (pTSADC->ADCCON & ENABLE_START_EN)
    {	// Wait for Start Bit Cleared
        ;//Sleep(1);
    }

    while (!(pTSADC->ADCCON & ECFLG_END))
    {	// Wait for ADC Conversion Ended
        ;//Sleep(1);
    }

    pTSADC->ADCDAT1 = D_XY_PST_YPOS| D_AUTO_PST_EN;
    *ym = D_YPDATA_MASK12(pTSADC->ADCDAT1);

    BITCLR( pTSADC->ADCTSC , ADCTSC_AUTO_ADC4);	// Auto Conversion

    pTSADC->ADCCLRINT = CLEAR_ADC_INT;
    pTSADC->ADCCLRUPDN = CLEAR_ADCWK_INT;
    pTSADC->ADCTSC = ADCTSC_WAIT_PENDOWN;
	BITSET(pTSADC->ADCCON, Hw2);//Standby

	//printk("[TOUCH       ]RAW:(%d,%d)(%d,%d) \r\n", *xp, *yp, *xm, *ym);

	return ret;
}

unsigned int    tca_adc_tsread(int* x, int* y, int *xp, int *ym)
{
	if (system_rev == 0x500) {
		BITCLR(pTSADC->ADCCON, Hw2);//wakeup
////////////////read x
		pTSADC->ADCTSC = Hw3|Hw0;
		//gpio setting - 30 high
		BITCSET(pTSADC->ADCCON,Hw6-Hw3,  Hw6);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		//read x value
		*y= D_XPDATA_MASK12(pTSADC->ADCDAT0);
		pTSADC->ADCTSC = ADCTSC_AUTO_ADC5;


////////////////read y
		pTSADC->ADCTSC = Hw3|Hw1;
		//gpio setting - 31,28 high
		BITCSET(pTSADC->ADCCON,Hw6-Hw3,  Hw5|Hw4);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		//read y value
		*x= D_XPDATA_MASK12(pTSADC->ADCDAT1);


		pTSADC->ADCTSC = ADCTSC_AUTO_ADC5;



////////////////read Z1
		pTSADC->ADCTSC = Hw3|Hw4|Hw5;

		BITCSET(pTSADC->ADCCON,Hw6-Hw3,  Hw6|Hw3);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		//read x value
		*xp= D_XPDATA_MASK12(pTSADC->ADCDAT0);

		pTSADC->ADCTSC = ADCTSC_AUTO_ADC5;


////////////////read Z2
		pTSADC->ADCTSC = Hw3|Hw4|Hw5;

		BITCSET(pTSADC->ADCCON,Hw6-Hw3,  Hw5|Hw4);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		//read x value
		*ym= D_XPDATA_MASK12(pTSADC->ADCDAT0);

		//*ym=0;
		BITSET(pTSADC->ADCCON, Hw2);//Standby
		BITCLR( pTSADC->ADCTSC , ADCTSC_AUTO_ADC5);	// Auto Conversion

		pTSADC->ADCCLRINT = CLEAR_ADC_INT;
		pTSADC->ADCCLRUPDN = CLEAR_ADCWK_INT;
		pTSADC->ADCTSC = ADCTSC_WAIT_PENDOWN;
	} else {
		BITCLR(pTSADC->ADCCON, Hw2);//wakeup
////////////////read Z2
		pTSADC->ADCTSC = Hw3|Hw4|Hw5;
		BITCSET(pTSADC->ADCCON,Hw6-Hw3,  Hw5|Hw4);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		
		*ym= D_XPDATA_MASK12(pTSADC->ADCDAT0);

////////////////read Z1
		pTSADC->ADCTSC = Hw4|Hw5|Hw3;
		BITCSET(pTSADC->ADCCON,Hw6-Hw3,  Hw6|Hw3);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		
		*xp= D_XPDATA_MASK12(pTSADC->ADCDAT0);
////////////////read x
		pTSADC->ADCTSC = Hw3|Hw2|Hw0;
		//gpio setting - 30 high
		BITCSET(pTSADC->ADCCON,Hw6-Hw3, Hw5|Hw4|Hw3);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		//read x value
		*x= D_XPDATA_MASK12(pTSADC->ADCDAT0);
//		pTSADC->ADCTSC = ADCTSC_AUTO_ADC5;


////////////////read y
		pTSADC->ADCTSC = Hw3|Hw2|Hw1;
		//gpio setting - 31,28 high
		BITCSET(pTSADC->ADCCON,Hw6-Hw3,  Hw6|Hw3);
		pTSADC->ADCCON |= ENABLE_START_EN;	// ADC Conversion Start

		while (pTSADC->ADCCON & ENABLE_START_EN)
		{	// Wait for Start Bit Cleared
			;//Sleep(1);
		}

		while (!(pTSADC->ADCCON & ECFLG_END))
		{	// Wait for ADC Conversion Ended
			;//Sleep(1);
		}
		//read y value
		*y= D_XPDATA_MASK12(pTSADC->ADCDAT1);


//		pTSADC->ADCTSC = ADCTSC_AUTO_ADC5;



		//*ym=0;
		BITSET(pTSADC->ADCCON, Hw2);//Standby

		pTSADC->ADCCLRINT = CLEAR_ADC_INT;
		pTSADC->ADCCLRUPDN = CLEAR_ADCWK_INT;
		pTSADC->ADCTSC = ADCTSC_WAIT_PENDOWN;

		return 1;
	}
}

unsigned int    tca_adc_powerdown(void)
{
    unsigned int    ret = 0;
    pTSADC->ADCCLRINT = Hw0;
    pTSADC->ADCCLRUPDN = Hw0;
    pTSADC->ADCTSC |= ADCTSC_WAIT_PENDOWN;

    BITSET(pTSADC->ADCCON, STDBM_STANDBY);
    return ret;
}

unsigned int    tca_adc_powerup(void)
{
    unsigned int    ret = 0;
    BITCLR(pTSADC->ADCCON, STDBM_STANDBY); // Because STDBM_NORMAL is [ 0<<2 ]
     return ret;

}
