/* 
 * linux/drivers/char/tcc_dxb/ctrl/tcc_dxb_control.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: Telechips Linux BACK-LIGHT DRIVER
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <asm/mach-types.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <mach/tcc_dxb_ctrl.h>
#include "board-tcc8920.h"

////////////////////////////////////
#define GPIO_DXB0_SCLK_REV1008		TCC_GPC(26)
#define GPIO_DXB0_SFRM_REV1008		TCC_GPC(25)
#define GPIO_DXB0_SDI_REV1008		TCC_GPC(24)
#define GPIO_DXB0_SDO_REV1008		TCC_GPC(23)
#define GPIO_DXB0_RST_REV1008		TCC_GPE(4)
#define GPIO_DXB1_SCLK_REV1008		TCC_GPC(0)
#define GPIO_DXB1_SFRM_REV1008		TCC_GPC(1)
#define GPIO_DXB1_SDI_REV1008		TCC_GPC(2)
#define GPIO_DXB1_SDO_REV1008		TCC_GPC(3)
#define GPIO_DXB1_RST_REV1008		TCC_GPE(27)
#define INT_DXB1_IRQ_REV1008		TCC_GPE(26)
#define INT_DXB0_IRQ_REV1008		TCC_GPE(3)
////////////////////////////////////
#define GPIO_DXB0_SCLK_REV1006		TCC_GPD(8)
#define GPIO_DXB0_SFRM_REV1006		TCC_GPD(10)
#define GPIO_DXB0_SDI_REV1006		TCC_GPD(9)
#define GPIO_DXB0_SDO_REV1006		TCC_GPD(7)
#define GPIO_DXB0_RST_REV1006		TCC_GPG(3)
#define GPIO_DXB1_SCLK_REV1006		TCC_GPE(12)
#define GPIO_DXB1_SFRM_REV1006		TCC_GPE(13)
#define GPIO_DXB1_SDI_REV1006		TCC_GPE(14)
#define GPIO_DXB1_SDO_REV1006		TCC_GPE(15)
#define GPIO_DXB1_RST_REV1006		TCC_GPG(4)
#define INT_DXB1_IRQ_REV1006		TCC_GPG(19)
#define INT_DXB0_IRQ_REV1006		TCC_GPE(16)
//////////////////////////////////////
#define GPIO_DXB0_SCLK_REV1005		TCC_GPB(0)
#define GPIO_DXB0_SFRM_REV1005		TCC_GPB(1)
#define GPIO_DXB0_SDI_REV1005		TCC_GPB(2)
#define GPIO_DXB0_SDO_REV1005		TCC_GPB(3)
#define GPIO_DXB0_RST_REV1005		TCC_GPC(21)
#define GPIO_DXB1_SCLK_REV1005		TCC_GPG(0)
#define GPIO_DXB1_SFRM_REV1005		TCC_GPG(1)
#define GPIO_DXB1_SDI_REV1005		TCC_GPG(2)
#define GPIO_DXB1_SDO_REV1005		TCC_GPG(3)
#define GPIO_DXB1_RST_REV1005		TCC_GPC(22)
#define INT_DXB1_IRQ_REV1005		TCC_GPE(31)
#define INT_DXB0_IRQ_REV1005		TCC_GPB(15)

static int gGPIO_DXB0_SCLK = GPIO_DXB0_SCLK;
static int gGPIO_DXB0_SFRM = GPIO_DXB0_SFRM;
static int gGPIO_DXB0_SDI = GPIO_DXB0_SDI;
static int gGPIO_DXB0_SDO = GPIO_DXB0_SDO;
static int gGPIO_DXB0_RST = GPIO_DXB0_RST;
static int gGPIO_DXB1_SCLK = GPIO_DXB1_SCLK;
static int gGPIO_DXB1_SFRM = GPIO_DXB1_SFRM;
static int gGPIO_DXB1_SDI = GPIO_DXB1_SDI;
static int gGPIO_DXB1_SDO = GPIO_DXB1_SDO;
static int gGPIO_DXB1_RST = GPIO_DXB1_RST;
static int gINT_DXB1_IRQ = INT_DXB1_IRQ;
static int gINT_DXB0_IRQ = INT_DXB0_IRQ;


static unsigned int guiBoardType = BOARD_TDMB_TCC3150;

static void tcc_dxb_ctrl_power_off(int deviceIdx)
{
	if(machine_is_tcc8920())
	{
		if(guiBoardType == BOARD_DXB_TCC3510 
		|| guiBoardType == BOARD_TDMB_TCC3161
		||  guiBoardType == BOARD_ISDBT_TCC353X)
		{
			switch(deviceIdx)
			{
			case 0:
				tcc_gpio_config(GPIO_DXB1_PD,  GPIO_FN(0));
				gpio_request(GPIO_DXB1_PD, NULL);
				gpio_direction_output(GPIO_DXB1_PD, 0);
				gpio_set_value(gGPIO_DXB1_RST, 0);
				break;
			case 1:
				tcc_gpio_config(GPIO_DXB2_PD,  GPIO_FN(0));
				gpio_request(GPIO_DXB2_PD, NULL);
				gpio_direction_output(GPIO_DXB2_PD, 0);
				gpio_set_value(gGPIO_DXB0_RST, 0);
				break;
			default:
				break;
			}
		}
		else if(guiBoardType == BOARD_DVBT_DIB9090)
		{
			tcc_gpio_config(GPIO_DXB1_PD,  GPIO_FN(0));
			gpio_request(GPIO_DXB1_PD, NULL);
			gpio_direction_output(GPIO_DXB1_PD, 0);
			gpio_set_value(gGPIO_DXB1_RST, 0);
		}
		else if(guiBoardType == BOARD_TDMB_TCC3150)
		{
			switch(deviceIdx)
			{
			case 0:
				gpio_set_value(GPIO_DXB1_PD, 0);
				gpio_set_value(gGPIO_DXB0_RST, 0);
				break;
			default:
				break;
			}
		}
		else if(guiBoardType == BOARD_ISDBT_TOSHIBA)
		{
			switch (deviceIdx)
			{
			case 0:
				gpio_set_value(gGPIO_DXB0_RST, 0);
				break;
			default:
				break;
			}
		}
		else if (guiBoardType == BOARD_ISDBT_DIB10096)
		{
			//GPIO_E[7] = PWDN#
			tcc_gpio_config(gGPIO_DXB0_SDO, GPIO_FN(0));
			gpio_direction_output(gGPIO_DXB0_SDO, 0);

			//GPIO_E[4] = RST#
			tcc_gpio_config(gGPIO_DXB0_RST, GPIO_FN(0));
			gpio_direction_output(gGPIO_DXB0_RST, 0);
		}

		/* GPIO_EXPAND DXB_ON Power-off */
		gpio_request(GPIO_DXB_ON,NULL);
		gpio_direction_output(GPIO_DXB_ON, 0);
		/* GPIO_EXPAND DEEPPWDN Power-off */
		gpio_request(GPIO_DXB1_PD,NULL);
		gpio_direction_output(GPIO_DXB1_PD, 0);
	}
}

static void tcc_dxb_ctrl_power_on(int deviceIdx)
{
	if(machine_is_tcc8920())
	{
		/* GPIO_EXPAND DXB_ON Power-on */
		gpio_request(GPIO_DXB_ON,NULL);
		gpio_direction_output(GPIO_DXB_ON, 1);

		/* GPIO_EXPAND DEEPPWDN Power-on */
		gpio_request(GPIO_DXB1_PD,NULL);
		gpio_direction_output(GPIO_DXB1_PD, 1);

		if(guiBoardType == BOARD_ISDBT_TOSHIBA)
		{
			switch (deviceIdx)
			{
			case 0:
				tcc_gpio_config(gGPIO_DXB0_RST,  GPIO_FN(0));
				gpio_request(gGPIO_DXB0_RST, NULL);
				gpio_direction_output(gGPIO_DXB0_RST, 0);
				msleep(10);
				gpio_set_value(gGPIO_DXB0_RST, 1);
				break;
			default:
				break;
			}
		}
		else if(guiBoardType == BOARD_DXB_TCC3510 
			|| guiBoardType == BOARD_TDMB_TCC3161
			||  guiBoardType == BOARD_ISDBT_TCC353X)
		
		{
			switch(deviceIdx)
			{
			case 0:
				tcc_gpio_config(GPIO_DXB1_PD, GPIO_FN(0));
				tcc_gpio_config(gGPIO_DXB1_RST, GPIO_FN(0));

				gpio_request(GPIO_DXB1_PD, NULL);
				gpio_direction_output(GPIO_DXB1_PD, 0);
				gpio_set_value(gGPIO_DXB1_RST, 0);
				gpio_set_value(GPIO_DXB1_PD, 0);
				msleep (20);
				gpio_set_value(GPIO_DXB1_PD, 1);
				msleep (20);

				gpio_request(gGPIO_DXB1_RST, NULL);
				gpio_direction_output(gGPIO_DXB1_RST, 0);
				msleep (20);
				gpio_set_value(gGPIO_DXB1_RST, 1);
				break;
			case 1:
				tcc_gpio_config(GPIO_DXB2_PD,  GPIO_FN(0));
				tcc_gpio_config(gGPIO_DXB0_RST,  GPIO_FN(0));
	
				gpio_request(GPIO_DXB2_PD, NULL);
				gpio_direction_output(GPIO_DXB2_PD, 0);
	
				gpio_set_value(gGPIO_DXB0_RST, 0);
				gpio_set_value(GPIO_DXB2_PD, 0);
				msleep (20);
				gpio_set_value(GPIO_DXB2_PD, 1);
				msleep (20);
	
				gpio_request(gGPIO_DXB0_RST, NULL);
				gpio_direction_output(gGPIO_DXB0_RST, 0);
				msleep (20);
				gpio_set_value(gGPIO_DXB0_RST, 1);
				break;
			default:
				break;
			}
		}
		else if(guiBoardType == BOARD_DVBT_DIB9090)
		{
			tcc_gpio_config(GPIO_DXB1_PD, GPIO_FN(0));
			tcc_gpio_config(gGPIO_DXB1_RST, GPIO_FN(0));

			gpio_request(GPIO_DXB1_PD, NULL);
			gpio_direction_output(GPIO_DXB1_PD, 0);
			gpio_set_value(gGPIO_DXB1_RST, 0);
			gpio_set_value(GPIO_DXB1_PD, 0);
			msleep (20);
			gpio_set_value(GPIO_DXB1_PD, 1);
			msleep (20);

			gpio_request(gGPIO_DXB1_RST, NULL);
			gpio_direction_output(gGPIO_DXB1_RST, 0);
			msleep (20);
			gpio_set_value(gGPIO_DXB1_RST, 1);

			/* RF Control */
			tcc_gpio_config(gGPIO_DXB0_SDO,  GPIO_FN(0));
			gpio_request(gGPIO_DXB0_SDO, NULL);
			gpio_direction_output(gGPIO_DXB0_SDO, 0);
			gpio_set_value(gGPIO_DXB0_SDO, 1);
		}
		else if(guiBoardType == BOARD_TDMB_TCC3150)
		{
			switch(deviceIdx)
			{
			case 0:
				gpio_set_value(GPIO_DXB1_PD, 1);
				break;
			default:
				break;
			}
		}
		else if (guiBoardType == BOARD_ISDBT_DIB10096)
		{
			//DXB1_PD (LDO_EN) is already set

			//GPIO_E[7] = PWDN#
			tcc_gpio_config(gGPIO_DXB0_SDO, GPIO_FN(0)|GPIO_PULL_DISABLE);
			gpio_direction_output(gGPIO_DXB0_SDO, 1);

			//GPIO_E[4] = RST#
			tcc_gpio_config(gGPIO_DXB0_RST, GPIO_FN(0));
			gpio_direction_output(gGPIO_DXB0_RST, 0);
			msleep(20);
			gpio_set_value (gGPIO_DXB0_RST, 1);
		}
	}
}

static void tcc_dxb_ctrl_power_reset(int deviceIdx)
{
	if(machine_is_tcc8920())
	{
		if(guiBoardType == BOARD_DXB_TCC3510 
		|| guiBoardType == BOARD_TDMB_TCC3161
		||  guiBoardType == BOARD_ISDBT_TCC353X)
		{
			switch(deviceIdx)
			{
			case 0:
				tcc_gpio_config(gGPIO_DXB0_RST, GPIO_FN(0));
				gpio_request(gGPIO_DXB0_RST, NULL);
				gpio_direction_output(gGPIO_DXB0_RST, 0);
				msleep (20);
				gpio_set_value(gGPIO_DXB0_RST, 1);
				break;
			case 1:
				tcc_gpio_config(gGPIO_DXB1_RST, GPIO_FN(0));
				gpio_request(gGPIO_DXB1_RST, NULL);
				gpio_direction_output(gGPIO_DXB1_RST, 0);
				msleep (20);
				gpio_set_value(gGPIO_DXB1_RST, 1);
				break;
			default:
				break;
			}
		}
		else if(guiBoardType == BOARD_DVBT_DIB9090)
        {
           	tcc_gpio_config(gGPIO_DXB0_RST, GPIO_FN(0));
			gpio_request(gGPIO_DXB0_RST, NULL);
			gpio_direction_output(gGPIO_DXB0_RST, 0);
			msleep (100);
			gpio_set_value(gGPIO_DXB0_RST, 1);
			msleep (100);
        }
		else if(guiBoardType == BOARD_TDMB_TCC3150)	
		{
			switch(deviceIdx)
			{
			case 0:
				gpio_set_value(gGPIO_DXB0_RST, 0);
				msleep(100);		
				gpio_set_value(gGPIO_DXB0_RST, 1);
				msleep(100);		
				break;
			default:
				break;
			}
		}
		else if (guiBoardType == BOARD_ISDBT_TOSHIBA || guiBoardType == BOARD_ISDBT_DIB10096)
		{
			switch (deviceIdx)
			{
			case 0:
				tcc_gpio_config(gGPIO_DXB0_RST, GPIO_FN(0));
				gpio_request(gGPIO_DXB0_RST, NULL);
				gpio_direction_output(gGPIO_DXB0_RST, 0);
				msleep(10);
				gpio_set_value(gGPIO_DXB0_RST, 1);
				break;
			default:
				break;
			}
		}
	}
}

static void tcc_dxb_ctrl_set_board(unsigned int uiboardtype)
{
	guiBoardType = uiboardtype;
	if(guiBoardType == BOARD_TDMB_TCC3150)
	{
		tcc_gpio_config(gINT_DXB0_IRQ,  GPIO_FN(0));
		gpio_request(gINT_DXB0_IRQ, NULL);
		gpio_direction_input(gINT_DXB0_IRQ);

		tcc_gpio_config(GPIO_DXB1_PD,  GPIO_FN(0));
		gpio_request(GPIO_DXB1_PD, NULL);
		gpio_direction_output(GPIO_DXB1_PD, 0);

		tcc_gpio_config(gGPIO_DXB0_RST,	GPIO_FN(0));
		gpio_request(gGPIO_DXB0_RST, NULL);
		gpio_direction_output(gGPIO_DXB0_RST, 0);
	}
}

static void tcc_dxb_ctrl_get_info(ST_CTRLINFO_ARG *pstCtrlInfo)
{
	if(guiBoardType == BOARD_DVBT_DIB9090M_PA)
		pstCtrlInfo->uiI2C = 4; //this is only for tcc9300cm
	else	
		pstCtrlInfo->uiI2C = 1;
}

static void tcc_dxb_ctrl_rf_path(unsigned int flag)
{
}

static void tcc_dxb_init(void)
{
	if(machine_is_tcc8920())
	{
		/*PULL_UP is disabled to save current.*/
        if(system_rev == 0x1005 || system_rev == 0x1007)
        {
            gGPIO_DXB0_SCLK = GPIO_DXB0_SCLK_REV1005;
            gGPIO_DXB0_SFRM = GPIO_DXB0_SFRM_REV1005;
            gGPIO_DXB0_SDI = GPIO_DXB0_SDI_REV1005;
            gGPIO_DXB0_SDO = GPIO_DXB0_SDO_REV1005;
            gGPIO_DXB0_RST = GPIO_DXB0_RST_REV1005;
            gGPIO_DXB1_SCLK = GPIO_DXB1_SCLK_REV1005;
            gGPIO_DXB1_SFRM = GPIO_DXB1_SFRM_REV1005;
            gGPIO_DXB1_SDI = GPIO_DXB1_SDI_REV1005;
            gGPIO_DXB1_SDO = GPIO_DXB1_SDO_REV1005;
            gGPIO_DXB1_RST = GPIO_DXB1_RST_REV1005;
			gINT_DXB1_IRQ = INT_DXB1_IRQ_REV1005;
            gINT_DXB0_IRQ = INT_DXB0_IRQ_REV1005;
        }      
        else if(system_rev == 0x1006)
        {
            gGPIO_DXB0_SCLK = GPIO_DXB0_SCLK_REV1006;
            gGPIO_DXB0_SFRM = GPIO_DXB0_SFRM_REV1006;
            gGPIO_DXB0_SDI = GPIO_DXB0_SDI_REV1006;
            gGPIO_DXB0_SDO = GPIO_DXB0_SDO_REV1006;
            gGPIO_DXB0_RST = GPIO_DXB0_RST_REV1006;
            gGPIO_DXB1_SCLK = GPIO_DXB1_SCLK_REV1006;
            gGPIO_DXB1_SFRM = GPIO_DXB1_SFRM_REV1006;
            gGPIO_DXB1_SDI = GPIO_DXB1_SDI_REV1006;
            gGPIO_DXB1_SDO = GPIO_DXB1_SDO_REV1006;
            gGPIO_DXB1_RST = GPIO_DXB1_RST_REV1006;
			gINT_DXB1_IRQ = INT_DXB1_IRQ_REV1006;
            gINT_DXB0_IRQ = INT_DXB0_IRQ_REV1006;
        }        
        else if(system_rev == 0x1008)
        {
            gGPIO_DXB0_SCLK = GPIO_DXB0_SCLK_REV1008;
            gGPIO_DXB0_SFRM = GPIO_DXB0_SFRM_REV1008;
            gGPIO_DXB0_SDI = GPIO_DXB0_SDI_REV1008;
            gGPIO_DXB0_SDO = GPIO_DXB0_SDO_REV1008;
            gGPIO_DXB0_RST = GPIO_DXB0_RST_REV1008;
            gGPIO_DXB1_SCLK = GPIO_DXB1_SCLK_REV1008;
            gGPIO_DXB1_SFRM = GPIO_DXB1_SFRM_REV1008;
            gGPIO_DXB1_SDI = GPIO_DXB1_SDI_REV1008;
            gGPIO_DXB1_SDO = GPIO_DXB1_SDO_REV1008;
            gGPIO_DXB1_RST = GPIO_DXB1_RST_REV1008;
			gINT_DXB1_IRQ = INT_DXB1_IRQ_REV1008;
            gINT_DXB0_IRQ = INT_DXB0_IRQ_REV1008;
        }        
		//TCC_GPE(2)
		tcc_gpio_config(gGPIO_DXB0_SFRM, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB0_SFRM, NULL);
		gpio_direction_output(gGPIO_DXB0_SFRM, 0);
	
		//TCC_GPE(3)
		tcc_gpio_config(gGPIO_DXB0_SCLK, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB0_SCLK, NULL);
		gpio_direction_output(gGPIO_DXB0_SCLK, 0);
	
		//TCC_GPE(4)
		tcc_gpio_config(gGPIO_DXB0_RST, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB0_RST, NULL);
		gpio_direction_output(gGPIO_DXB0_RST, 0);
	
		//TCC_GPE(5)
		tcc_gpio_config(gINT_DXB0_IRQ, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gINT_DXB0_IRQ, NULL);
		gpio_direction_output(gINT_DXB0_IRQ, 0);
	
		//TCC_GPE(6)
		tcc_gpio_config(gGPIO_DXB0_SDI, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB0_SDI, NULL);
		gpio_direction_output(gGPIO_DXB0_SDI, 0);
	
		//TCC_GPE(7)
		tcc_gpio_config(gGPIO_DXB0_SDO, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB0_SDO, NULL);
		gpio_direction_output(gGPIO_DXB0_SDO, 0);
	
		//TCC_GPD(5)
		tcc_gpio_config(gGPIO_DXB1_SFRM, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB1_SFRM, NULL);
		gpio_direction_output(gGPIO_DXB1_SFRM, 0);
	
		//TCC_GPD(6)
		tcc_gpio_config(gGPIO_DXB1_SCLK, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB1_SCLK, NULL);
		gpio_direction_output(gGPIO_DXB1_SCLK, 0);
	
		//TCC_GPD(7)
		tcc_gpio_config(gGPIO_DXB1_SDI, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB1_SDI, NULL);
		gpio_direction_output(gGPIO_DXB1_SDI, 0);
	
		//TCC_GPD(8)
		tcc_gpio_config(gGPIO_DXB1_SDO, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB1_SDO, NULL);
		gpio_direction_output(gGPIO_DXB1_SDO, 0);
	
		//TCC_GPD(9)
		tcc_gpio_config(gGPIO_DXB1_RST, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gGPIO_DXB1_RST, NULL);
		gpio_direction_output(gGPIO_DXB1_RST, 0);
	
		//TCC_GPD(10)
		tcc_gpio_config(gINT_DXB1_IRQ, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(gINT_DXB1_IRQ, NULL);
		gpio_direction_output(gINT_DXB1_IRQ, 0);
	
		//smart Card Interface for CAS
		tcc_gpio_config(TCC_GPD(13), GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(TCC_GPD(13), NULL);
		gpio_direction_output(TCC_GPD(13), 0);
	
		//smart Card Interface for CAS
		tcc_gpio_config(TCC_GPD(14), GPIO_FN(0)|GPIO_PULL_DISABLE);//
		gpio_request(TCC_GPD(14), NULL);
		gpio_direction_output(TCC_GPD(14), 0);

//////////////////////////////////////////////////////////////////////////////////////////////////////
		tcc_gpio_config(GPIO_DXB_ON, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB_ON, NULL);
		gpio_direction_output(GPIO_DXB_ON, 0);

		tcc_gpio_config(GPIO_DXB1_PD, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB1_PD, NULL);
		gpio_direction_output(GPIO_DXB1_PD, 0);		
#if 0
		tcc_gpio_config(GPIO_DXB_UARTTX, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB_UARTTX, NULL);
		gpio_direction_output(GPIO_DXB_UARTTX, 0);

		//TCC_GPE(5)
		tcc_gpio_config(GPIO_DXB_UARTRX, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB_UARTRX, NULL);
		gpio_direction_output(GPIO_DXB_UARTRX, 0);
#endif
	}
}

static struct tcc_dxb_platform_data tcc_dxb_pdata = {
	.init		= tcc_dxb_init,
	.power_off	= tcc_dxb_ctrl_power_off,
	.power_on	= tcc_dxb_ctrl_power_on,
	.power_reset= tcc_dxb_ctrl_power_reset,
	.rf_path	= tcc_dxb_ctrl_rf_path,
	.set_board	= tcc_dxb_ctrl_set_board,
	.get_info	= tcc_dxb_ctrl_get_info,
};

static struct platform_device tcc_dxb_device = {
	.name	= "tcc_dxb_ctrl",
	.dev	= {
		.platform_data	= &tcc_dxb_pdata,
	},
};

static int __init tcc8920_init_tcc_dxb_ctrl(void)
{
	if(!machine_is_tcc8920() || machine_is_m801_88())
		return 0;

	printk("%s (built %s %s)\n", __func__, __DATE__, __TIME__);
	platform_device_register(&tcc_dxb_device);
	return 0;
}
device_initcall(tcc8920_init_tcc_dxb_ctrl);
