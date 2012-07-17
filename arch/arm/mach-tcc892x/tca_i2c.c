/****************************************************************************
 *   FileName    : tca_i2c.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/io.h>

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/tca_i2c.h>
#include <mach/gpio.h>


/*****************************************************************************
* Function Name : tca_i2c_setgpio(int ch)
* Description: I2C port configuration
* input parameter:
* 		int core; 	// I2C Core
*       int ch;   	// I2C master channel
******************************************************************************/
void tca_i2c_setgpio(int core, int ch)
{
	//PI2CPORTCFG i2c_portcfg = (PI2CPORTCFG)tcc_p2v(HwI2C_PORTCFG_BASE);

	switch (core)
	{
		case 0:
			#if defined(CONFIG_MACH_TCC8920ST)
				#if defined(CONFIG_STB_BOARD_DONGLE)
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 25<<8);
				tcc_gpio_config(TCC_GPE(14), GPIO_FN6|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPE(14), GPIO_FN6|GPIO_OUTPUT|GPIO_LOW);
				#elif defined(CONFIG_STB_BOARD_ISDBT_MODULE)
				//I2C[22] - GPIOG[2][3]
				//i2c_portcfg->PCFG0.bREG.MASTER0 = 22;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 22);
				tcc_gpio_config(TCC_GPG(2), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPG(3), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				#else
				//I2C[26] - GPIOG[18][19]
				//i2c_portcfg->PCFG0.bREG.MASTER0 = 26;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 26);
				tcc_gpio_config(TCC_GPG(18), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPG(19), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				#endif
			#elif defined(CONFIG_MACH_M805_892X)
				if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
				{
					//I2C[12] - GPIOC[2][3]
					//i2c_portcfg->PCFG0.bREG.MASTER0 = 12;
					BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 12);
					tcc_gpio_config(TCC_GPC(2), GPIO_FN7|GPIO_OUTPUT|GPIO_LOW);
					tcc_gpio_config(TCC_GPC(3), GPIO_FN7|GPIO_OUTPUT|GPIO_LOW);
				}
				else
				{
					#if defined(CONFIG_M805S_8923_0XA)
					//I2C[22] - GPIOG[2][3]
					//i2c_portcfg->PCFG0.bREG.MASTER0 = 22;
					BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 22);
					tcc_gpio_config(TCC_GPG(2), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
					tcc_gpio_config(TCC_GPG(3), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
					#else
					//I2C[23] - GPIOG[6][7]
					//i2c_portcfg->PCFG0.bREG.MASTER0 = 23;
					BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 23);
					tcc_gpio_config(TCC_GPG(6), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
					tcc_gpio_config(TCC_GPG(7), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
					#endif
				}
			#else
				if(system_rev == 0x1006)
				{
					//I2C[18] - GPIOF[13][14]
					//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER0 = 18;
					BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 18);
					tcc_gpio_config(TCC_GPF(13), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
					tcc_gpio_config(TCC_GPF(14), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
				}
				else if(system_rev == 0x1008)
				{
					//I2C[22] - GPIOG[2][3]
					//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER0 = 22;
					BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 22);
					tcc_gpio_config(TCC_GPG(2), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
					tcc_gpio_config(TCC_GPG(3), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				}
				else
				{
					//I2C[8] - GPIOB[9][10]
					//i2c_portcfg->PCFG0.bREG.MASTER0 = 8;
					BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x000000FF, 8);
					tcc_gpio_config(TCC_GPB(9), GPIO_FN11|GPIO_OUTPUT|GPIO_LOW);
					tcc_gpio_config(TCC_GPB(10), GPIO_FN11|GPIO_OUTPUT|GPIO_LOW);
				}
			#endif
			break;
		case 1:
			#if defined(CONFIG_MACH_M805_892X)
			if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			{
				//I2C[25] - GPIOG[12][13]
				//i2c_portcfg->PCFG0.bREG.MASTER1 = 25;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 25<<8);
				tcc_gpio_config(TCC_GPG(12), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPG(13), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
			}
			else
			{
				#if defined(CONFIG_M805S_8923_0XA)
				//I2C[21] - GPIOF[27][28]
				//i2c_portcfg->PCFG0.bREG.MASTER1 = 21;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 21<<8);
				tcc_gpio_config(TCC_GPF(27), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPF(28), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
				#else
				//I2C[24] - GPIOG[10][11]
				//i2c_portcfg->PCFG0.bREG.MASTER1 = 24;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 24<<8);
				tcc_gpio_config(TCC_GPG(10), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPG(11), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				#endif
			}
			#else
			if(system_rev == 0x1005 || system_rev == 0x1007)
			{
				//I2C[21] - GPIOF[27][28]
				//i2c_portcfg->PCFG0.bREG.MASTER1 = 21;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 21<<8);
				tcc_gpio_config(TCC_GPF(27), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPF(28), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
			}
			else if(system_rev == 0x1006)
			{
				//I2C[28] - GPIO_ADC[2][3]
				//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER1 = 28;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 28<<8);
				tcc_gpio_config(TCC_GPADC(2), GPIO_FN3|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPADC(3), GPIO_FN3|GPIO_OUTPUT|GPIO_LOW);
			}
			else if(system_rev == 0x1008)
			{
				//I2C[13] - GPIOC[12][13]
				//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER1 = 13;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 13<<8);
				tcc_gpio_config(TCC_GPC(12), GPIO_FN7|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPC(13), GPIO_FN7|GPIO_OUTPUT|GPIO_LOW);
			}
			else
			{
				//I2C[22] - GPIOG[2][3]
				//i2c_portcfg->PCFG0.bREG.MASTER1 = 22;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x0000FF00, 22<<8);
				tcc_gpio_config(TCC_GPG(2), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPG(3), GPIO_FN4|GPIO_OUTPUT|GPIO_LOW);
			}
			#endif
			break;
		case 2:
			#if defined(CONFIG_MACH_M805_892X)
			if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
			{
				//I2C[18] - GPIOF[13][14]
				//i2c_portcfg->PCFG0.bREG.MASTER1 = 18;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x00FF0000, 18<<16);
				tcc_gpio_config(TCC_GPF(13), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPF(14), GPIO_FN10|GPIO_OUTPUT|GPIO_LOW);
			}
			#else
			if(system_rev != 0x1006 && system_rev != 0x1008)
			{
				//I2C[28] - GPIO_ADC[2][3]
				//i2c_portcfg->PCFG0.bREG.MASTER2 = 28;
				BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0x00FF0000, 28<<16);
				tcc_gpio_config(TCC_GPADC(2), GPIO_FN3|GPIO_OUTPUT|GPIO_LOW);
				tcc_gpio_config(TCC_GPADC(3), GPIO_FN3|GPIO_OUTPUT|GPIO_LOW);
			}
			#endif
			break;
		case 3:
			//I2C[27] - GPIO_HDMI[2][3]
			//i2c_portcfg->PCFG0.bREG.MASTER3 = 27;
			BITCSET(((PI2CPORTCFG)io_p2v(HwI2C_PORTCFG_BASE))->PCFG0.nREG, 0xFF000000, 27<<24);
			tcc_gpio_config(TCC_GPHDMI(2), GPIO_FN1|GPIO_OUTPUT|GPIO_LOW);
			tcc_gpio_config(TCC_GPHDMI(3), GPIO_FN1|GPIO_OUTPUT|GPIO_LOW);
			//Not used..
			break;
		default:
			break;
	}
}

