
/****************************************************************************
 *   FileName    : tca_keypad.c
 *   Description : 
 ****************************************************************************
*
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
*
 ****************************************************************************/
#include "tca_keypad.h"
#include <asm/mach-types.h>

typedef struct {
	unsigned int	StartVal;
	unsigned int	EndVal;
	unsigned char	KeyCode;
}SCANCODE;

const SCANCODE stScanCodeMapping_9200[] = {
#if 0
	{0x14A, 0x168, KEY_ESC},
	{0x1A4, 0x1C2, KEY_1 },
	{0x0  , 0x28 , KEY_2 },
	{0x50 , 0x6E , KEY_3 },
	{0x10E, 0x12C, KEY_4 },
	{0x3A2, 0x3B6, KEY_5 },
	{0xE1 , 0xF5 , KEY_6 },
	{0x32F, 0x343, KEY_7 },
	{0x2B2, 0x2C6, KEY_8 },
	{0x91 , 0xA5 , KEY_9 },
	{0x235, 0x249, KEY_0 },
#else
#if 1
	{0x23F, 0x243, KEY_BACK},	// VK_LWIN
	{0x11E, 0x121, KEY_7},		// VK_MENU
	{0x9A , 0x9F , KEY_DOWN},	// VK_SPACE
	{0x5A , 0x5F , KEY_UP},		// VK_UP
	{0x2BA, 0x2C1, KEY_RIGHT},	// VK_TAB
	{0x8  , 0x1E , KEY_ENTER},	// VK_ESCAPE
	{0x338, 0x340, KEY_LEFT},	// VK_RBUTTON
	{0x1B4, 0x1B9, KEY_MENU},	// VK_LEFT
	{0xEA , 0xEF , KEY_8},		// VK_RIGHT
	{0x158, 0x15C, KEY_HOME},	// VK_DOWN
	{0x384, 0x3CB, KEY_CAMERA},	// VK_RETURN
#else
	// Grater than 4050 -> IDLE
	// {ADC MinValue, MaxValue, Virtual Key Value}
	{0x55A, 0x582,  KEY_ESC},	// VK_LWIN
	{0x6C2, 0x6F4,  KEY_1},		// VK_MENU
	{0x5A , 0x82 ,  KEY_2},		// VK_SPACE
	{0x15E, 0x186,  KEY_3},		// VK_UP
	{0x460, 0x492,  KEY_4},		// VK_TAB
	{0xE92, 0x1004, KEY_5},		// VK_ESCAPE
	{0x398, 0x3CA,  KEY_6},		// VK_RBUTTON
	{0xCD0, 0xD0C,  KEY_7},		// VK_LEFT
	{0xADC, 0xB18,  KEY_8},		// VK_RIGHT
	{0x258, 0x294,  KEY_9},		// VK_DOWN
	{0x8E8, 0x92E,  KEY_0},		// VK_RETURN
#endif
#endif
};

const SCANCODE stScanCodeMapping_9300[] = {
#if 1
        {0xEC0, 0xECF, KEY_MENU},
        {0xAF0, 0xAFF, KEY_CLEAR},
        {0x6C0, 0x6CF, KEY_UP},
        {0x460, 0x46F, KEY_HOME},
        {0x250, 0x25F, KEY_BACK},
        {0x50 , 0x5F , KEY_DOWN},
//	{0x3B0, 0x3BF, KEY_MENU},
//	{0x1B0, 0x1BF, KEY_CLEAR},
//	{0x90 , 0x9F , KEY_UP},
//	{0x2C0, 0x2c , KEY_HOME},
//	{0x110, 0x11F, KEY_BACK},
//	{0x10 , 0x1F , KEY_DOWN},
#else
	/* TCC9302F_D2_16X2@89BD_SV1.0 */
	{4044,4050, KEY_MENU},
	{3010,3020, KEY_UP},
	{1875,1885, KEY_HOME},
	{1210,1220, KEY_DOWN},
	{645,655, KEY_BACK},
	{100,105, KEY_CLEAR},
#endif
};

const SCANCODE stScanCodeMapping_8800[] = {
	{0x650, 0x7FF, KEY_MENU},
	{0x200, 0x2FF, KEY_DOWN},	
	{0x500, 0x5FF, KEY_HOME},
	{0x100, 0x1FF, KEY_UP},
	{0xC00, 0xE8F, KEY_BACK},
};

const SCANCODE stScanCodeMapping_8801[] = {
	{0x650, 0x7FF, KEY_HOME},
	{0x200, 0x2FF, KEY_DOWN},	
	{0x500, 0x5FF, KEY_MENU},
	{0x100, 0x1FF, KEY_UP},
	{0xC00, 0xE8F, KEY_BACK},
};


/*****************************************************************************
* Function Name : tca_keypad_getscancode(void)
******************************************************************************
* Desription  	: 
* Parameter   	: 
* Return      	: 
* Note          : 
******************************************************************************/
unsigned int tca_keypad_getscancode(void)
{
	unsigned int	adcdata;
	int adc_ch = 0;	// Keypad-> ADC0
	volatile PTSADC pTSADC = (volatile PTSADC)tcc_p2v(HwTSADC_BASE);

#if defined(CONFIG_ARCH_TCC892X)
	pTSADC->ADCCON.nREG |= Hw2|adc_ch<<3;
	pTSADC->ADCCON.bREG.STBY = 0;
	pTSADC->ADCCON.bREG.EN_ST = 1;

	do{
		adcdata = pTSADC->ADCDAT0.nREG & 0x3ff;
	}while(!(pTSADC->ADCCON.nREG & Hw15));

	pTSADC->ADCCON.bREG.STBY = 1;
	return (adcdata);	
#else
	BITSET(pTSADC->ADCCON,Hw2| adc_ch<<3);
	BITCLR(pTSADC->ADCCON,Hw2);		
	BITSET(pTSADC->ADCCON,Hw0);			


	do{
		adcdata = pTSADC->ADCDAT0 & 0x3ff;
	}while(!(pTSADC->ADCCON & Hw15));

	BITSET(pTSADC->ADCCON,Hw2);
	return (adcdata);	
#endif
}

/*****************************************************************************
* Function Name : tca_keypad_getkeycodebyscancode()
******************************************************************************
* Desription  	:
* Parameter   	:
* Return      	:
* Note          : 
******************************************************************************/
int tca_keypad_getkeycodebyscancode(unsigned int adcdata)
{
	int i;
	int key = -1;

	//printk("Out Key Value :: %d\n", adcdata);
	if (machine_is_tcc8800()) {
		if (system_rev >= 0x600) {
			for (i = 0; i < sizeof(stScanCodeMapping_8801)/sizeof(SCANCODE); i++) {
				if ((adcdata >= stScanCodeMapping_8801[i].StartVal) && (adcdata <= stScanCodeMapping_8801[i].EndVal))
					key = stScanCodeMapping_8801[i].KeyCode;
			}
		}
		else {
			for (i = 0; i < sizeof(stScanCodeMapping_8800)/sizeof(SCANCODE); i++) {
				if ((adcdata >= stScanCodeMapping_8800[i].StartVal) && (adcdata <= stScanCodeMapping_8800[i].EndVal))
					key = stScanCodeMapping_8800[i].KeyCode;
			}
		}
	}
	else if (machine_is_tcc9200() || machine_is_tcc9201() || machine_is_tcc8900()) {
		for (i = 0; i < sizeof(stScanCodeMapping_9200)/sizeof(SCANCODE); i++) {
			if ((adcdata >= stScanCodeMapping_9200[i].StartVal) && (adcdata <= stScanCodeMapping_9200[i].EndVal))
				key = stScanCodeMapping_9200[i].KeyCode;
		}
	}
	else if (machine_is_tcc9300cm()){	
		for (i = 0; i < sizeof(stScanCodeMapping_9300)/sizeof(SCANCODE); i++) {
			if ((adcdata >= stScanCodeMapping_9300[i].StartVal) && (adcdata <= stScanCodeMapping_9300[i].EndVal))
				key = stScanCodeMapping_9300[i].KeyCode;
		}
	}

	return key;
}

