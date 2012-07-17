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
#include "board-m805_892x.h"

#include <linux/err.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/consumer.h>

#if defined(CONFIG_REGULATOR)
static struct regulator *vdd_dxb = NULL;
#endif
static int g_power_status = 0; //0:pwer off state, 1:power on status
static unsigned int guiBoardType = BOARD_TDMB_TCC3150;

static void tcc_dxb_ctrl_power_off(int deviceIdx)
{
	if(machine_is_m805_892x())
	{
        if(g_power_status == 0) //already power off
            return;

        g_power_status = 0; //power off status
		switch(deviceIdx)
		{
		case 0:
			tcc_gpio_config(GPIO_DXB_PWDN, GPIO_FN(0));//DXB_PWDN#
			gpio_request(GPIO_DXB_PWDN, NULL);
			gpio_direction_output(GPIO_DXB_PWDN, 0);
			break;
		default:
			break;
		}
	
		tcc_gpio_config(GPIO_DXB_PWREN, GPIO_FN(0));     //DXB_PWREN
		gpio_request(GPIO_DXB_PWREN, NULL);
		gpio_direction_output(GPIO_DXB_PWREN, 0);

		#if defined(CONFIG_REGULATOR) && defined(CONFIG_M805S_8925_0XX)
		if (vdd_dxb)
        {
            printk("regulator_disable !!!\n");
			msleep(20);
			regulator_disable(vdd_dxb);
        }
		#endif
	}
}

static void tcc_dxb_ctrl_power_on(int deviceIdx)
{
	if(machine_is_m805_892x())
	{
        if(g_power_status == 1) //already power on
            return;

        g_power_status = 1; //power on status
		#if defined(CONFIG_REGULATOR) && defined(CONFIG_M805S_8925_0XX)
		if (vdd_dxb)
        {
            printk("regulator_enable !!!\n");
			regulator_enable(vdd_dxb);
			msleep(20);
        }
		#endif

		/* GPIO_EXPAND DXB_ON Power-on */
		tcc_gpio_config(GPIO_DXB_PWREN,  GPIO_FN(0));
		gpio_request(GPIO_DXB_PWREN, NULL);
		gpio_direction_output(GPIO_DXB_PWREN, 1);
	
		switch(deviceIdx)
		{
		case 0:
			tcc_gpio_config(GPIO_DXB_PWDN,  GPIO_FN(0));
			tcc_gpio_config(GPIO_DXB0_RST,  GPIO_FN(0));
	
			gpio_request(GPIO_DXB_PWDN, NULL);
			gpio_direction_output(GPIO_DXB_PWDN, 1);
			gpio_request(GPIO_DXB0_RST, NULL);
			gpio_direction_output(GPIO_DXB0_RST, 0);
			msleep(20);
			gpio_set_value(GPIO_DXB0_RST, 1);
			break;
		default:
			break;
		}
	}
}

static void tcc_dxb_ctrl_power_reset(int deviceIdx)
{
	if(machine_is_m801_88())
	{
		switch(deviceIdx)
		{
		case 0:
			tcc_gpio_config(GPIO_DXB0_RST, GPIO_FN(0));
			gpio_request(GPIO_DXB0_RST, NULL);
			gpio_direction_output(GPIO_DXB0_RST, 0);
			msleep(20);
			gpio_set_value(GPIO_DXB0_RST, 1);
			break;
		default:
			break;
		}
	}
}

static void tcc_dxb_ctrl_set_board(unsigned int uiboardtype)
{
	guiBoardType = uiboardtype;
}

static void tcc_dxb_ctrl_get_info(ST_CTRLINFO_ARG *pstCtrlInfo)
{
}

static void tcc_dxb_ctrl_rf_path(unsigned int flag)
{
	if(machine_is_m805_892x())
	{
		tcc_gpio_config(GPIO_RFSW_CTL0, GPIO_FN(0));
		gpio_request(GPIO_RFSW_CTL0, NULL);
		gpio_direction_output(GPIO_RFSW_CTL0, 0);
	
		printk("[%s:%d] flag:%d\n", __func__, __LINE__, flag);
	
		switch(flag)
		{
		case DXB_RF_PATH_UHF:
			gpio_set_value(GPIO_RFSW_CTL0, 1);
			tcc_gpio_config(GPIO_RFSW_CTL3, GPIO_FN(0));
			gpio_request(GPIO_RFSW_CTL3, NULL);
			gpio_direction_output(GPIO_RFSW_CTL3, 0);
			break;
	
		case DXB_RF_PATH_VHF:
			gpio_set_value(GPIO_RFSW_CTL0, 0);
			tcc_gpio_config(GPIO_RFSW_CTL1, GPIO_FN(0));
			gpio_request(GPIO_RFSW_CTL1, NULL);
			gpio_direction_output(GPIO_RFSW_CTL1, 1);    		
	
			tcc_gpio_config(GPIO_RFSW_CTL2, GPIO_FN(0));
			gpio_request(GPIO_RFSW_CTL2, NULL);
			gpio_direction_output(GPIO_RFSW_CTL2, 0);
			break;
		default:
			break;
		}
	}
}

static void tcc_dxb_init(void)
{
	if(machine_is_m805_892x())
	{
		/*PULL_UP is disabled to save current.*/
	
		//TCC_GPE(2)
		tcc_gpio_config(GPIO_DXB0_SFRM, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB0_SFRM, NULL);
		gpio_direction_output(GPIO_DXB0_SFRM, 0);
	
		//TCC_GPE(3)
		tcc_gpio_config(GPIO_DXB0_SCLK, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB0_SCLK, NULL);
		gpio_direction_output(GPIO_DXB0_SCLK, 0);
	
		//TCC_GPE(9)
		tcc_gpio_config(GPIO_DXB0_RST, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB0_RST, NULL);
		gpio_direction_output(GPIO_DXB0_RST, 0);
	
		//TCC_GPE(5)
		tcc_gpio_config(INT_DXB0_IRQ, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(INT_DXB0_IRQ, NULL);
		gpio_direction_output(INT_DXB0_IRQ, 0);
	
		//TCC_GPE(6)
		tcc_gpio_config(GPIO_DXB0_SDI, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB0_SDI, NULL);
		gpio_direction_output(GPIO_DXB0_SDI, 0);
	
		//TCC_GPE(7)
		tcc_gpio_config(GPIO_DXB0_SDO, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB0_SDO, NULL);
		gpio_direction_output(GPIO_DXB0_SDO, 0);
	
		//TCC_GPD(5)
		tcc_gpio_config(GPIO_DXB1_SFRM, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB1_SFRM, NULL);
		gpio_direction_output(GPIO_DXB1_SFRM, 0);
	
		//TCC_GPD(6)
		tcc_gpio_config(GPIO_DXB1_SCLK, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB1_SCLK, NULL);
		gpio_direction_output(GPIO_DXB1_SCLK, 0);
	
		//TCC_GPD(7)
		tcc_gpio_config(GPIO_DXB1_SDI, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB1_SDI, NULL);
		gpio_direction_output(GPIO_DXB1_SDI, 0);
	
		//TCC_GPD(8)
		tcc_gpio_config(GPIO_DXB1_SDO, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB1_SDO, NULL);
		gpio_direction_output(GPIO_DXB1_SDO, 0);
	
		//TCC_GPE(8)
		tcc_gpio_config(GPIO_DXB_PWDN, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB_PWDN, NULL);
		gpio_direction_output(GPIO_DXB_PWDN, 0);
	
		//TCC_GPG(5)
		tcc_gpio_config(GPIO_DXB_PWREN, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_DXB_PWREN, NULL);
		gpio_direction_output(GPIO_DXB_PWREN, 0);	//DXB_PWREN //power off
	
		//TCC_GPE(1)
		tcc_gpio_config(GPIO_RFSW_CTL0, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_RFSW_CTL0, NULL);
		gpio_direction_output(GPIO_RFSW_CTL0, 0);
	
		//TCC_GPE(1)
		tcc_gpio_config(GPIO_RFSW_CTL1, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_RFSW_CTL1, NULL);
		gpio_direction_output(GPIO_RFSW_CTL1, 0);
	
		//TCC_GPE(1)
		tcc_gpio_config(GPIO_RFSW_CTL2, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_RFSW_CTL2, NULL);
		gpio_direction_output(GPIO_RFSW_CTL2, 0);
	
		//TCC_GPE(1)
		tcc_gpio_config(GPIO_RFSW_CTL3, GPIO_FN(0)|GPIO_PULL_DISABLE);
		gpio_request(GPIO_RFSW_CTL3, NULL);
		gpio_direction_output(GPIO_RFSW_CTL3, 0);
		
		#if defined(CONFIG_REGULATOR) && defined(CONFIG_M805S_8925_0XX)
		if (vdd_dxb == NULL)
		{
			vdd_dxb = regulator_get(NULL, "vdd_dxb");
			if (IS_ERR(vdd_dxb)) {
				printk("Failed to obtain vdd_dxb\n");
				vdd_dxb = NULL;
			}
		}
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

static int __init m805_892x_init_tcc_dxb_ctrl(void)
{
	if(!machine_is_m805_892x())
		return 0;

	printk("%s (built %s %s)\n", __func__, __DATE__, __TIME__);
	platform_device_register(&tcc_dxb_device);
	return 0;
}
device_initcall(m805_892x_init_tcc_dxb_ctrl);
