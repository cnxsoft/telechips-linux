/*
 * PWM Timer Count 
 *
 * Copyright (C) 2009, 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <mach/bsp.h>
#include <asm/mach-types.h>
#include <mach/gpio.h>
#include <mach/tca_tco.h>
#include <mach/io.h>

#if defined(CONFIG_ARCH_TCC892X)
#include <mach/reg_physical.h>
#endif



#define	HwTCO_BASE_ADDR(X)		(HwTMR_BASE+(X)*0x10)

PTIMERN IO_TCO_GetBaseAddr(unsigned uCH)
{
	return (volatile PTIMERN)tcc_p2v(HwTCO_BASE_ADDR(uCH));
}

unsigned IO_TCO_GetGpioFunc(unsigned uCH, unsigned uGPIO)
{
#ifdef CONFIG_ARCH_TCC892X
	switch(uGPIO)
	{
		case TCC_GPB(9): 	// TCO0
		case TCC_GPB(10):	// TCO1
		case TCC_GPB(11):	// TCO2
		case TCC_GPB(12):	// TCO3
		case TCC_GPB(13):	// TCO4
		case TCC_GPB(14):	// TCO5
			return GPIO_FN(12);

		case TCC_GPC(0): 	// TCO0
		case TCC_GPC(1):	// TCO1
		case TCC_GPC(2):	// TCO2
		case TCC_GPC(3):	// TCO3
		case TCC_GPC(4):	// TCO4
		case TCC_GPC(5):	// TCO5
			return GPIO_FN(9);

		case TCC_GPD(9): 	// TCO0
		case TCC_GPD(10):	// TCO1
		case TCC_GPD(11):	// TCO2
		case TCC_GPD(12):	// TCO3
		case TCC_GPD(13):	// TCO4
		case TCC_GPD(14):	// TCO5
			return GPIO_FN(9);

		case TCC_GPE(12): 	// TCO0
		case TCC_GPE(13):	// TCO1
		case TCC_GPE(14):	// TCO2
		case TCC_GPE(15):	// TCO3
		case TCC_GPE(16):	// TCO4
		case TCC_GPE(17):	// TCO5
			return GPIO_FN(7);
			
		case TCC_GPF(16): 	// TCO0
		case TCC_GPF(17):	// TCO1
		case TCC_GPF(18):	// TCO2
		case TCC_GPF(19):	// TCO3
		case TCC_GPF(20):	// TCO4
		case TCC_GPF(21):	// TCO5
			return GPIO_FN(11);	
			
		case TCC_GPG(5): 	// TCO0
		case TCC_GPG(6):	// TCO1
		case TCC_GPG(7):	// TCO2
		case TCC_GPG(8):	// TCO3
		case TCC_GPG(9):	// TCO4
		case TCC_GPG(10):	// TCO5
			return GPIO_FN(7);				

		default:
			break;
	}
#elif defined(CONFIG_ARCH_TCC88XX)
	switch(uGPIO)
	{
		case TCC_GPA(4): 	// TCO0
		case TCC_GPB(5):	// TCO1
		case TCC_GPB(6):	// TCO2
		case TCC_GPB(7):	// TCO3
		case TCC_GPB(14):	// TCO4
		case TCC_GPB(15):	// TCO5
			return GPIO_FN(2);

		default:
			break;
	}
#else
#error code : not define TCO gpio functional setting at chipset
#endif//
	return 0;
}


int tca_tco_pwm_ctrl(unsigned tco_ch, unsigned uGPIO, unsigned int max_cnt, unsigned int level_cnt)
{
	unsigned uFGPIO;
	volatile PTIMERN pTIMER = IO_TCO_GetBaseAddr(tco_ch);

//	printk("tco:%d, GPIO(G:%d, Num:%d) max:%d level:%d TCOaddr:0x%p \n", tco_ch, (uGPIO>>5), (uGPIO &0x1F), max_cnt, level_cnt, pTIMER);
	if(pTIMER == NULL)
		return -1;

	gpio_direction_output(uGPIO, 1);
 
	if(max_cnt <= level_cnt)	{
		tcc_gpio_config(uGPIO,  GPIO_FN(0));
		gpio_set_value(uGPIO, 1);
	}
	else if(level_cnt == 0) {
		tcc_gpio_config(uGPIO, GPIO_FN(0));
		gpio_set_value(uGPIO, 0);
	}
	else 	{
		pTIMER->TREF = max_cnt;
		pTIMER->TCFG	= 0x105;	
		pTIMER->TMREF = level_cnt;
		pTIMER->TCFG	= 0x105;
		uFGPIO = IO_TCO_GetGpioFunc(tco_ch, uGPIO);
		tcc_gpio_config(uGPIO,  uFGPIO);
	}
}

