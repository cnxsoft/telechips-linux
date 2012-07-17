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


/*****************************************************************************
* Function Name : tca_i2c_setgpio(int ch)
* Description: I2C port configuration
* input parameter:
* 		int core; 	// I2C Core
*       int ch;   	// I2C master channel
******************************************************************************/
void tca_i2c_setgpio(int core, int ch)
{
	PGPIO gpio = (PGPIO)tcc_p2v(HwGPIO_BASE);

	switch (core) {
		case 0:
		{
			if (ch == 0) {
				BITCSET(gpio->GPAFN0, (Hw8-Hw0), Hw4|Hw0);			/* GPIO_A[1:0] */
				BITSET(gpio->GPAEN, Hw1|Hw0);
				BITCLR(gpio->GPADAT, Hw1|Hw0);
			} else if (ch == 1) {
				BITCSET(gpio->GPAFN0, (Hw32-Hw28), Hw28);			/* GPIO_A[7] */
				BITCSET(gpio->GPAFN1, (Hw4-Hw0), Hw0);			    /* GPIO_A[8] */
				BITSET(gpio->GPAEN, Hw8|Hw7);
				BITCLR(gpio->GPADAT, Hw8|Hw7);
			}
			break;
		}
		case 1:
		{
			if (ch == 0) {
                /* Not used */
			} else if (ch == 1) {
                /* Not used */
			}
			break;
		}
	}
}

