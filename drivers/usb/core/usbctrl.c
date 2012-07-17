/*
 * usbctrl.c: common usb control API
 *
 *  Copyright (C) 2011, Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/cpufreq.h>

#include <asm/io.h>
#include <asm/mach-types.h>

#include <mach/bsp.h>
#include <mach/gpio.h>
#include <mach/tcc_board_power.h>

#if !defined(CONFIG_ARCH_TCC92XX)
extern const struct tcc_freq_table_t gtHSIONormalClockLimitTable;
#endif

static struct clk *g_pOHCIClk = NULL;
static struct clk *g_pEHCIClk = NULL; 
static int is_usb_otg = 0;
static unsigned int g_iExec = 0;


void tcc_ohci_clkset(int on)
{
	char *pDev = NULL;
	
	if (machine_is_tcc8900()){
		pDev = "usb_host"; 
	}else if (machine_is_tcc8800() || machine_is_tcc8800st()){
		pDev = "usb11h";
	}else{
		pDev = "unknown";
	}
	
	if (on){
		if(g_pOHCIClk){
			//printk("~~> [WARNING] OHCI clock is already turned on\n");
			return;
		}

		g_pOHCIClk = clk_get(NULL, (const char*)pDev);
		if (IS_ERR(g_pOHCIClk)){
			printk(KERN_ERR "ERR - usb_host clk_get fail.\n");
			return;
		}else{
			clk_enable(g_pOHCIClk);
			clk_set_rate(g_pOHCIClk, 48*1000*1000);
		}
	}else{
		if (g_pOHCIClk) {
		#if defined(CONFIG_TCC_DWC_OTG)
			if (!is_usb_otg)
		#endif
			{
				if(g_pOHCIClk){
					clk_disable(g_pOHCIClk);
				}
				g_pOHCIClk = NULL;
			}
		}
	}
}

/* [id] 0:OHCI , 1:EHCI's OHCI , -1:OTG */
void tcc_ohci_clock_control(int id, int on)
{
	if (id == 1){
		return;
	}

	if(g_iExec){
		printk("tcc_ohci_clock_control is still running...(%d)\n", id);
		return;
	}
	
	g_iExec = 1;
	
	if (on) {
		tcc_ohci_clkset(1);
		is_usb_otg = 1;
	} else {
	#if !defined(CONFIG_USB_OHCI_HCD) || !defined(CONFIG_USB_OHCI_HCD_MODULE)
		tcc_ohci_clkset(0);
	#endif
		is_usb_otg = 0;
	}
	
	g_iExec = 0;
}

void tcc_ehci_clkset(int on)
{
	if (machine_is_tcc8800st() || machine_is_tcc8800() || 
		machine_is_m801_88() || machine_is_m803() || 
		machine_is_tcc8920()  || machine_is_tcc8920st() || machine_is_m805_892x()) 
	{
		if(on)
		{
			if(g_pEHCIClk){
				//printk("~~> [WARNING] EHCI clock is already turned on\n");
				return;
			}

		#if !defined(CONFIG_ARCH_TCC92XX)
			tcc_cpufreq_set_limit_table((struct tcc_freq_table_t *)&gtHSIONormalClockLimitTable, TCC_FREQ_LIMIT_EHCI, 1);
		#endif
			
			g_pEHCIClk = clk_get(NULL, "usb20h");
			if (IS_ERR(g_pEHCIClk)){
				printk("ERR - usb20h clk_get fail.\n");	
				return;
			}

			clk_enable(g_pEHCIClk);
			clk_set_rate(g_pEHCIClk, 48*1000*1000);
		}
		else
		{
			if(g_pEHCIClk){
				clk_disable(g_pEHCIClk);
			}

		#if !defined(CONFIG_ARCH_TCC92XX)
			tcc_cpufreq_set_limit_table((struct tcc_freq_table_t *)&gtHSIONormalClockLimitTable, TCC_FREQ_LIMIT_EHCI, 0);
		#endif
			g_pEHCIClk = NULL;
		}
	}
}


/*************************************************************************************
OTG/OHCI/EHCI VBUS CONTROL
*************************************************************************************/
void tcc_otg_vbus_init(void)
{
	if(machine_is_tcc8800() || machine_is_tcc8920()){
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_INIT);
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_ON);
	}
}

void tcc_otg_vbus_exit(void)
{
	if(machine_is_tcc8800() || machine_is_tcc8920()){
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_OFF);		
	}
}

/*
 * OTG VBUS Power Control. It is controlled GPIO_EXPAND DVBUS_ON.
 * - Host mode: DVBUS_ON is HIGH
 * - Device mode: DVBUS_ON is LOW
 */
void tcc_otg_vbus_ctrl(int port_index, int on)
{
#if defined(CONFIG_ARCH_TCC88XX)
	if (machine_is_m801_88() || machine_is_m803()) 
	{
		GPIO *pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);		
		BITSET(pGPIO->GPFEN, Hw12);
		(on)? BITSET(pGPIO->GPFDAT, Hw12):BITCLR(pGPIO->GPFDAT, Hw12);
	} 
	else if (machine_is_tcc8800()) 
	{
		int gpio_otg0_vbus_en;

		gpio_otg0_vbus_en = TCC_GPEXT2(13);
		gpio_request(gpio_otg0_vbus_en, "otg0_vbus_en");
		gpio_direction_output(gpio_otg0_vbus_en, (on)?1:0);
	}
	if (machine_is_tcc8800st()) {
		int gpio_otg0_en;

		tcc_gpio_config(TCC_GPB(14), GPIO_FN(0));
	
		gpio_otg0_en = TCC_GPB(14);
		gpio_request(gpio_otg0_en, "OTG0_VBUS_EN");
		gpio_direction_output(gpio_otg0_en, (on)?1:0);
	}
#elif defined(CONFIG_ARCH_TCC92XX)
	if (machine_is_tcc8900()) {
		int gpio_pwr_gp1 = TCC_GPEXT3(7);
		int gpio_dvbus = TCC_GPEXT3(0);	
		gpio_request(gpio_pwr_gp1, "pwr_gp1");
		gpio_request(gpio_dvbus, "dvbus");	
		if (on) {
			gpio_direction_output(gpio_pwr_gp1, 1);
			gpio_direction_output(gpio_dvbus, 1);
		} else {
			gpio_direction_output(gpio_dvbus, 0);
		}
	} else if (machine_is_m57te()) {
		int gpio_pwr_gp1 = TCC_GPA(15);
		gpio_request(gpio_pwr_gp1, "OTG_DRVVBUS");
		if (on) {
			gpio_direction_output(gpio_pwr_gp1, 1);
		} else {
			gpio_direction_output(gpio_pwr_gp1, 0);
		}
	}
#elif defined(CONFIG_MACH_TCC9300)
	if (machine_is_tcc9300()) {
		int gpio_otg0_en, gpio_otg1_en;

		if(port_index != 0) {
			gpio_otg1_en = TCC_GPEXT2(14);
			gpio_request(gpio_otg1_en, "OTG1_VBUS_EN");
			gpio_direction_output(gpio_otg1_en, (on)?1:0);
			} else {
			gpio_otg0_en = TCC_GPEXT2(13);
			gpio_request(gpio_otg0_en, "OTG0_VBUS_EN");
			gpio_direction_output(gpio_otg0_en, (on)?1:0);
		}
	} else if (machine_is_tcc9300cm()) {
		GPIO *pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	
#define DVBUS_ON	0
#define PWR_GP1		7

		if(port_index != 0) {
			BITSET(pGPIO->GPEEN, Hw29);
			if (on) {
				BITSET(pGPIO->GPEDAT, Hw29);	//OTG1_VBUS_EN
				gpio_set_value(TCC_GPEXT3(DVBUS_ON), 1);
				gpio_set_value(TCC_GPEXT3(PWR_GP1), 1);
		} else {
			BITCLR(pGPIO->GPEDAT, Hw29);
			gpio_set_value(TCC_GPEXT3(DVBUS_ON), 0);	
			gpio_set_value(TCC_GPEXT3(PWR_GP1), 0);	
		}
		} else {
			BITSET(pGPIO->GPEEN, Hw28);
			if (on) {
				BITSET(pGPIO->GPEDAT, Hw28);	//OTG0_VBUS_EN
				gpio_set_value(TCC_GPEXT3(DVBUS_ON), 1);
				gpio_set_value(TCC_GPEXT3(PWR_GP1), 1);
			} else {
				BITCLR(pGPIO->GPEDAT, Hw28);
				gpio_set_value(TCC_GPEXT3(DVBUS_ON), 0);
				gpio_set_value(TCC_GPEXT3(PWR_GP1), 0);
			}
		}
	} else if (machine_is_tcc9300st()) {
		int gpio_otg0_en, gpio_otg1_en;

		if(port_index != 0)
		{
			gpio_otg1_en = TCC_GPE(19);
			gpio_request(gpio_otg1_en, "OTG1_VBUS_EN");
			gpio_direction_output(gpio_otg1_en, (on)?1:0);
		}
		else
		{
			gpio_otg0_en = TCC_GPB(14);
			gpio_request(gpio_otg0_en, "OTG0_VBUS_EN");
			gpio_direction_output(gpio_otg0_en, (on)?1:0);
		}
	}
#elif defined(CONFIG_ARCH_TCC892X)
	if (machine_is_tcc8920()) {
		int gpio_otg0_vbus_en;

		gpio_otg0_vbus_en = TCC_GPEXT2(13);
		gpio_request(gpio_otg0_vbus_en, "otg0_vbus_en");
		gpio_direction_output(gpio_otg0_vbus_en, (on)?1:0);
	} else if (machine_is_tcc8920st()) {
		int gpio_otg0_vbus_en;

		gpio_otg0_vbus_en = TCC_GPD(11);
		gpio_request(gpio_otg0_vbus_en, "otg0_vbus_en");
		gpio_direction_output(gpio_otg0_vbus_en, (on)?1:0);
	} else if (machine_is_m805_892x()) {
		int gpio_otg0_vbus_en;

		if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			gpio_otg0_vbus_en = TCC_GPD(3);
		}
		else {
			gpio_otg0_vbus_en = TCC_GPF(12);
		}
		gpio_request(gpio_otg0_vbus_en, "otg0_vbus_en");
		gpio_direction_output(gpio_otg0_vbus_en, (on)?1:0);
	}
#endif
}

/************************************************************************************/
int tcc_ohci_vbus_init(void)
{
	if (machine_is_tcc8800()){
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_INIT);
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_ON);
	}
	return 0;
	}

int tcc_ohci_vbus_exit(void)
{
	if (machine_is_tcc8800()){
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_OFF);
	}
	return 0;
	}

int tcc_ohci_vbus_ctrl(int id, int on)
	{
	if (id == 1){
		return 1;
	}else{
		if (machine_is_tcc8900()){
			int gpio_pwr_gp1, gpio_hvbus;

			gpio_pwr_gp1 = TCC_GPEXT3(4);
			gpio_hvbus = TCC_GPEXT3(1);

			gpio_request(gpio_pwr_gp1, "pwr_gp1");
			gpio_request(gpio_hvbus, "hvbus");

			if (on){
				gpio_direction_output(gpio_pwr_gp1, 1);
				gpio_direction_output(gpio_hvbus, 1);
			}else{
				gpio_direction_output(gpio_hvbus, 0);
			}
		}else if(machine_is_tcc8800()){
			int gpio_fs_host_en, gpio_host_bus_en;
			
			gpio_fs_host_en = TCC_GPEXT5(3);
			gpio_host_bus_en = TCC_GPEXT2(15);

			gpio_request(gpio_fs_host_en, "fs_host_en");
			gpio_request(gpio_host_bus_en, "host_bus_en");

			gpio_direction_output(gpio_fs_host_en, (on)?1:0);
			gpio_direction_output(gpio_host_bus_en, (on)?1:0);
		}
	}
	return 1;
}

/************************************************************************************/
int tcc_ehci_vbus_init(void)
{
	if (machine_is_tcc8800() || machine_is_tcc8920()){
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_INIT);
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_ON);
	}
	return 0;
}

int tcc_ehci_vbus_exit(void)
{
	if (machine_is_tcc8800() || machine_is_tcc8920()){
		tcc_power_control(TCC_V5P_POWER, TCC_POWER_OFF);
	}
	return 0;
}

int tcc_ehci_vbus_ctrl(int on)
{
	if (machine_is_tcc8800() || machine_is_tcc8920())
	{
		int gpio_otg1_vbus_en, gpio_hs_host_en;

		gpio_hs_host_en = TCC_GPEXT5(2);
		gpio_otg1_vbus_en = TCC_GPEXT2(14);	
		
		gpio_request(gpio_hs_host_en, "hs_host_en");
		gpio_request(gpio_otg1_vbus_en, "otg1_vbus_en");		

		/* Don't control gpio_hs_host_en because this power also supported in USB core. */
		gpio_direction_output(gpio_hs_host_en, 1);	
		gpio_direction_output(gpio_otg1_vbus_en, (on)?1:0);

		return 0;
	}
	else if(machine_is_tcc8800st())
	{
		int gpio_host1_vbus_en;
		gpio_host1_vbus_en = TCC_GPB(16);
		gpio_request(gpio_host1_vbus_en, "host1_vbus_en");
		gpio_direction_output(gpio_host1_vbus_en, (on)?1:0);

		return 0;
	}
	else if(machine_is_tcc8920st())
	{
		int gpio_host1_vbus_en;
		 	
		#if defined(CONFIG_STB_BOARD_HDB892F) 	
		gpio_host1_vbus_en = TCC_GPE(21);
		#else
		gpio_host1_vbus_en = TCC_GPC(25);
		#endif

		tcc_gpio_config(gpio_host1_vbus_en, GPIO_FN(0));
		gpio_request(gpio_host1_vbus_en, "host1_vbus_en");
		gpio_direction_output(gpio_host1_vbus_en, (on)?1:0);
		return 0;
	}
	else if(machine_is_m801_88() || machine_is_m803() || machine_is_m805_892x())
	{
		return 0;
	}

	return -1;
}

/*************************************************************************************
EXPORT DEFINES
*************************************************************************************/
EXPORT_SYMBOL(tcc_otg_vbus_init);
EXPORT_SYMBOL(tcc_otg_vbus_exit);

EXPORT_SYMBOL(tcc_ohci_vbus_init);
EXPORT_SYMBOL(tcc_ohci_vbus_exit);
EXPORT_SYMBOL(tcc_ohci_vbus_ctrl);

EXPORT_SYMBOL(tcc_ehci_vbus_init);
EXPORT_SYMBOL(tcc_ehci_vbus_exit);
EXPORT_SYMBOL(tcc_ehci_vbus_ctrl);

EXPORT_SYMBOL(tcc_ohci_clock_control);
EXPORT_SYMBOL(tcc_ohci_clkset);
EXPORT_SYMBOL(tcc_ehci_clkset);
