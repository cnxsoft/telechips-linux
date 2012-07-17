/****************************************************************************
*   FileName    : tca_adconv.h
*   Description : 
****************************************************************************
*
*   TCC Version : 1.0
*   Copyright (c) Telechips, Inc.
*   ALL RIGHTS RESERVED
*	resivion: %v
****************************************************************************/


#ifndef __TCC_TSADC_H__
#define __TCC_TSADC_H__
//


enum {
    ADC_CHANNEL0 = 0,
    ADC_CHANNEL1,
    ADC_CHANNEL2,
    ADC_CHANNEL3,
    ADC_CHANNEL4,
    ADC_CHANNEL5,
    ADC_TOUCHSCREEN,
    ADC_CHANNEL7,
};

struct tcc_adc_platform_data{
	int panel;
};

unsigned int    tca_adc_adcinitialize(unsigned int devAddress, void* param );
unsigned int    tca_adc_portinitialize(unsigned int devAddress, void* param);
unsigned int    tca_adc_read(unsigned int channel);
unsigned int    tca_adc_tsautoread(int* xp, int* yp, int *xm, int *ym);
unsigned int    tca_adc_tsread(int* xpos, int* ypos, int *xp, int *ym);
unsigned int    tca_adc_powerdown(void);
unsigned int    tca_adc_powerup(void);

#endif //__TCC_TSADC_H__
