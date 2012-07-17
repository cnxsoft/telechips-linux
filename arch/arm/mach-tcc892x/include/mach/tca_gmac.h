/*
 * linux/driver/net/tcc_gmac/tca_gmac.h
 * 	
 * Author : Telechips <linux@telechips.com>
 * Created : June 22, 2010
 * Description : This is the driver for the Telechips MAC 10/100/1000 on-chip Ethernet controllers.  
 *               Telechips Ethernet IPs are built around a Synopsys IP Core.
 *
 * Copyright (C) 2010 Telechips
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 */ 

#ifndef _TCA_GMAC_H_
#define _TCA_GMAC_H_

#include <mach/bsp.h>
#include <mach/gpio.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <asm/mach-types.h>

extern const struct tcc_freq_table_t gtHSIOClockLimitTable;

#define PHY_INFSEL_ONLY_MODE //PHY_INFSEL Only Mode

// GMAC
#define GPIO_GMAC_TXD0      TCC_GPC(5)
#define GPIO_GMAC_TXD1      TCC_GPC(6)
#define GPIO_GMAC_TXD2      TCC_GPC(12)
#define GPIO_GMAC_TXD3      TCC_GPC(13)
#define GPIO_GMAC_TXD4      TCC_GPC(22)
#define GPIO_GMAC_TXD5      TCC_GPC(23)
#define GPIO_GMAC_TXD6      TCC_GPC(24)
#define GPIO_GMAC_TXD7      TCC_GPC(25)
#define GPIO_GMAC_TXEN      TCC_GPC(7)
#define GPIO_GMAC_TXER      TCC_GPC(15)
#define GPIO_GMAC_TXCLK     TCC_GPC(0)

#define GPIO_GMAC_RXD0      TCC_GPC(3)
#define GPIO_GMAC_RXD1      TCC_GPC(4)
#define GPIO_GMAC_RXD2      TCC_GPC(10)
#define GPIO_GMAC_RXD3      TCC_GPC(11)
#define GPIO_GMAC_RXD4      TCC_GPC(18)
#define GPIO_GMAC_RXD5      TCC_GPC(19)
#define GPIO_GMAC_RXD6      TCC_GPC(20)
#define GPIO_GMAC_RXD7      TCC_GPC(21)
#define GPIO_GMAC_RXDV      TCC_GPC(8)
#define GPIO_GMAC_RXER      TCC_GPC(14)
#define GPIO_GMAC_RXCLK     TCC_GPC(26)

#define GPIO_GMAC_COL       TCC_GPC(16)
#define GPIO_GMAC_CRS       TCC_GPC(17)

#define GPIO_GMAC_MDC       TCC_GPC(1)
#define GPIO_GMAC_MDIO      TCC_GPC(2)

//#define GPIO_PHY_RST        TCC_GPC(28)
//#define GPIO_PHY_RST        TCC_GPC(9)
#define GPIO_PHY_ON         TCC_GPC(27)
#define GPIO_E_PHY_ON         TCC_GPE(25)//tcc8925

//#define INT_PHY_IRQ         TCC_GPC(9)

static struct clk *hsio_clk=NULL;
static struct clk *gmac_clk=NULL;
static struct clk *gmac_global_clk=NULL;
static void tca_gmac_clk_init(void)
{
	hsio_clk = clk_get(NULL, "hsio");
	BUG_ON(hsio_clk == NULL);

	gmac_clk = clk_get(NULL, "gmac");
	BUG_ON(gmac_clk == NULL);

	gmac_global_clk = clk_get(NULL, "gmac_global");
	BUG_ON(gmac_clk == NULL);
}

static void tca_gmac_clk_enable(void)
{
	if(hsio_clk) {
		clk_enable(hsio_clk);
		tcc_cpufreq_set_limit_table(&gtHSIOClockLimitTable, TCC_FREQ_LIMIT_GMAC, 1);
	}

	if (gmac_global_clk) {
		clk_enable(gmac_global_clk);
	}

	if(gmac_clk) {
		clk_enable(gmac_clk);
		clk_set_rate(gmac_clk, 125*1000*1000);
	}
}

static void tca_gmac_clk_disable(void)
{

	if(gmac_clk) {
		clk_set_rate(gmac_clk, 6*1000*1000);
		clk_disable(gmac_clk);
	}

	if (gmac_global_clk) {
		clk_disable(gmac_global_clk);
	}

	if(hsio_clk) {
		tcc_cpufreq_set_limit_table(&gtHSIOClockLimitTable, TCC_FREQ_LIMIT_GMAC, 0);
		clk_disable(hsio_clk);
	}
}

static void tca_gmac_phy_pwr_on(void)
{
	if(machine_is_tcc8920() && (system_rev == 0x1006))//tcc8925
		gpio_direction_output(GPIO_E_PHY_ON, 1);
	else
		gpio_direction_output(GPIO_PHY_ON, 1);
	
	msleep(10);
}

static void tca_gmac_phy_pwr_off(void)
{
	
	if(machine_is_tcc8920st()){
		gpio_direction_output(TCC_GPC(9), 0);
	}
	else{
		if(system_rev == 0x1006){//tcc8925
			gpio_direction_output(TCC_GPC(9), 0);
		}
		else{
			gpio_direction_output(TCC_GPC(28), 0);
		}
	}
	
	msleep(10);
	
	if(machine_is_tcc8920() && (system_rev == 0x1006))//tcc8925
		gpio_direction_output(GPIO_E_PHY_ON, 0);
	else
		gpio_direction_output(GPIO_PHY_ON, 0);
	
	msleep(10);
}

static void tca_gmac_phy_reset(void)
{
	if(machine_is_tcc8920st()){
		gpio_direction_output(TCC_GPC(9), 0);
		msleep(10);
		gpio_direction_output(TCC_GPC(9), 1);
		msleep(300);
	}
	else{
		if(system_rev == 0x1006){//tcc8925
			gpio_direction_output(TCC_GPC(9), 0);
			msleep(10);
			gpio_direction_output(TCC_GPC(9), 1);
			msleep(100);
		}
		else{
			gpio_direction_output(TCC_GPC(28), 0);
			msleep(10);
			gpio_direction_output(TCC_GPC(28), 1);
			msleep(100);
		}
	}
}

static void tca_gmac_portinit(void)
{
	volatile PHSIOBUSCFG pHSIOCFG= (volatile PHSIOBUSCFG)tcc_p2v(HwHSIOBUSCFG_BASE);
	volatile PGPIO pGPIO= (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	volatile PCKC pCKC= (volatile PCKC)tcc_p2v(HwCKC_BASE);
	
	if (machine_is_tcc8920() || machine_is_tcc8920st()) {
	#if 1
		pGPIO->GPCFN0.bREG.GPFN01 = 1; //MDC
		pGPIO->GPCFN0.bREG.GPFN02 = 1; //MDIO

		pGPIO->GPCFN0.bREG.GPFN00 = 1; //TXCLK
		pGPIO->GPCFN0.bREG.GPFN05 = 1; //TXD0
		pGPIO->GPCFN0.bREG.GPFN06 = 1; //TXD1
		pGPIO->GPCFN1.bREG.GPFN12 = 1; //TXD2
		pGPIO->GPCFN1.bREG.GPFN13 = 1; //TXD3
		pGPIO->GPCFN0.bREG.GPFN07 = 1; //TXEN

		pGPIO->GPCFN3.bREG.GPFN26 = 1; //RXCLK
		pGPIO->GPCFN0.bREG.GPFN03 = 1; //RXD0
		pGPIO->GPCFN0.bREG.GPFN04 = 1; //RXD1
		pGPIO->GPCFN1.bREG.GPFN10 = 1; //RXD2
		pGPIO->GPCFN1.bREG.GPFN11 = 1; //RXD3
		pGPIO->GPCFN1.bREG.GPFN08 = 1; //RXDV

#if defined(CONFIG_TCC_GMAC_MII_MODE)
		if(machine_is_tcc8920())
		pGPIO->GPCFN1.bREG.GPFN15 = 1; //TXER
		
		pGPIO->GPCFN1.bREG.GPFN14 = 1; //RXER
		pGPIO->GPCFN2.bREG.GPFN16 = 1; //COL
		pGPIO->GPCFN2.bREG.GPFN17 = 1; //CRS
#elif defined(CONFIG_TCC_GMAC_GMII_MODE)
		pGPIO->GPCFN1.bREG.GPFN15 = 1; //TXER
		pGPIO->GPCFN1.bREG.GPFN14 = 1; //RXER
		pGPIO->GPCFN2.bREG.GPFN16 = 1; //COL
		pGPIO->GPCFN2.bREG.GPFN17 = 1; //sCRS

		pGPIO->GPCFN2.bREG.GPFN22 = 1; //GPIO_GMAC_TXD4
		pGPIO->GPCFN2.bREG.GPFN23 = 1; //GPIO_GMAC_TXD5
		pGPIO->GPCFN3.bREG.GPFN24= 1; //GPIO_GMAC_TXD6
		pGPIO->GPCFN3.bREG.GPFN25 = 1; //GPIO_GMAC_TXD7
		
		pGPIO->GPCFN2.bREG.GPFN18 = 1; //GPIO_GMAC_RXD4
		pGPIO->GPCFN2.bREG.GPFN19 = 1; //GPIO_GMAC_RXD5
		pGPIO->GPCFN2.bREG.GPFN20= 1; //GPIO_GMAC_RXD6
		pGPIO->GPCFN2.bREG.GPFN21 = 1; //GPIO_GMAC_RXD7
#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
#endif

	#else
		tcc_gpio_config(GPIO_GMAC_MDC, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_MDIO, GPIO_FN(1));

		tcc_gpio_config(GPIO_GMAC_TXCLK, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXD0, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXD1, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXD2, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXD3, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXEN, GPIO_FN(1));

		tcc_gpio_config(GPIO_GMAC_RXCLK, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXD0, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXD1, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXD2, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXD3, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXDV, GPIO_FN(1));

#if defined(CONFIG_TCC_GMAC_MII_MODE)
		if(machine_is_tcc8920())
		tcc_gpio_config(GPIO_GMAC_TXER, GPIO_FN(1));
		
		tcc_gpio_config(GPIO_GMAC_RXER, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_COL, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_CRS, GPIO_FN(1));

#elif defined(CONFIG_TCC_GMAC_GMII_MODE)
		tcc_gpio_config(GPIO_GMAC_TXER, GPIO_FN(1));
	
		tcc_gpio_config(GPIO_GMAC_RXER, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_COL, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_CRS, GPIO_FN(1));

		tcc_gpio_config(GPIO_GMAC_TXD4, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXD5, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXD6, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_TXD7, GPIO_FN(1));

		tcc_gpio_config(GPIO_GMAC_RXD4, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXD5, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXD6, GPIO_FN(1));
		tcc_gpio_config(GPIO_GMAC_RXD7, GPIO_FN(1));
#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
#endif
	#endif

#if 1
#define GMAC_RX_DRV_STRENGTH   3
#define GMAC_TX_DRV_STRENGTH   1

		pGPIO->GPCCD0.bREG.GPCD01 = GMAC_RX_DRV_STRENGTH; //MDC
		pGPIO->GPCCD0.bREG.GPCD02 = GMAC_RX_DRV_STRENGTH; //MDIO

		pGPIO->GPCCD0.bREG.GPCD00 = 2; //TXCLK
		pGPIO->GPCCD0.bREG.GPCD05 = GMAC_TX_DRV_STRENGTH; //TXD0
		pGPIO->GPCCD0.bREG.GPCD06 = GMAC_TX_DRV_STRENGTH; //TXD1
		pGPIO->GPCCD0.bREG.GPCD12 = GMAC_TX_DRV_STRENGTH; //TXD2
		pGPIO->GPCCD0.bREG.GPCD13 = GMAC_TX_DRV_STRENGTH; //TXD3
		pGPIO->GPCCD0.bREG.GPCD07 = GMAC_TX_DRV_STRENGTH; //TXEN

		pGPIO->GPCCD1.bREG.GPCD26 = GMAC_RX_DRV_STRENGTH; //RXCLK
		pGPIO->GPCCD0.bREG.GPCD03 = GMAC_RX_DRV_STRENGTH; //RXD0
		pGPIO->GPCCD0.bREG.GPCD04 = GMAC_RX_DRV_STRENGTH; //RXD1
		pGPIO->GPCCD0.bREG.GPCD10 = GMAC_RX_DRV_STRENGTH; //RXD2
		pGPIO->GPCCD0.bREG.GPCD11 = GMAC_RX_DRV_STRENGTH; //RXD3
		pGPIO->GPCCD0.bREG.GPCD08 = GMAC_RX_DRV_STRENGTH; //RXEN

		pGPIO->GPCSR.bREG.GP01 = 0; //fast slew rate MDC
		pGPIO->GPCSR.bREG.GP02 = 0; //fast slew rate MDIO

		pGPIO->GPCSR.bREG.GP00 = 0; //fast slew rate TXCLK
		pGPIO->GPCSR.bREG.GP05 = 0; //fast slew rate TXD0
		pGPIO->GPCSR.bREG.GP06 = 0; //fast slew rate TXD1
		pGPIO->GPCSR.bREG.GP12 = 0; //fast slew rate TXD2
		pGPIO->GPCSR.bREG.GP13 = 0; //fast slew rate TXD3
		pGPIO->GPCSR.bREG.GP07 = 0; //fast slew rate TXEN
                                                          
		pGPIO->GPCSR.bREG.GP26 = 0; //fast slew rate RXCLK
		pGPIO->GPCSR.bREG.GP03 = 0; //fast slew rate RXD0
		pGPIO->GPCSR.bREG.GP04 = 0; //fast slew rate RXD1
		pGPIO->GPCSR.bREG.GP10 = 0; //fast slew rate RXD2
		pGPIO->GPCSR.bREG.GP11 = 0; //fast slew rate RXD3
		pGPIO->GPCSR.bREG.GP08 = 0; //fast slew rate RXEN

#if defined(CONFIG_TCC_GMAC_MII_MODE)
		if(machine_is_tcc8920())
		pGPIO->GPCCD0.bREG.GPCD15 = GMAC_TX_DRV_STRENGTH; //TXER
		
		pGPIO->GPCCD0.bREG.GPCD14 = GMAC_TX_DRV_STRENGTH; //RXER
		pGPIO->GPCCD1.bREG.GPCD16 = GMAC_TX_DRV_STRENGTH; //COL
		pGPIO->GPCCD1.bREG.GPCD17 = GMAC_TX_DRV_STRENGTH; //CRS

		if(machine_is_tcc8920())
		pGPIO->GPCSR.bREG.GP15 = 0; //fast slew rate TXER
		
		pGPIO->GPCSR.bREG.GP14 = 0; //fast slew rate RXER
		pGPIO->GPCSR.bREG.GP16 = 0; //fast slew rate COL
		pGPIO->GPCSR.bREG.GP17 = 0; //fast slew rate CRS

#elif defined(CONFIG_TCC_GMAC_GMII_MODE)
		pGPIO->GPCCD0.bREG.GPCD15 = GMAC_TX_DRV_STRENGTH; //TXER
		pGPIO->GPCCD0.bREG.GPCD14 = GMAC_TX_DRV_STRENGTH; //RXER
		pGPIO->GPCCD1.bREG.GPCD16 = GMAC_TX_DRV_STRENGTH; //COL
		pGPIO->GPCCD1.bREG.GPCD17 = GMAC_TX_DRV_STRENGTH; //CRS

		pGPIO->GPCCD1.bREG.GPCD22 = GMAC_TX_DRV_STRENGTH; //TXD4
		pGPIO->GPCCD1.bREG.GPCD23 = GMAC_TX_DRV_STRENGTH; //TXD5
		pGPIO->GPCCD1.bREG.GPCD24 = GMAC_TX_DRV_STRENGTH; //TXD6
		pGPIO->GPCCD1.bREG.GPCD25 = GMAC_TX_DRV_STRENGTH; //TXD7

		pGPIO->GPCCD0.bREG.GPCD18 = GMAC_RX_DRV_STRENGTH; //RXD4
		pGPIO->GPCCD0.bREG.GPCD19 = GMAC_RX_DRV_STRENGTH; //RXD5
		pGPIO->GPCCD1.bREG.GPCD20 = GMAC_RX_DRV_STRENGTH; //RXD6
		pGPIO->GPCCD1.bREG.GPCD21 = GMAC_RX_DRV_STRENGTH; //RXD7

		pGPIO->GPCSR.bREG.GP15 = 0; //fast slew rate TXER
		pGPIO->GPCSR.bREG.GP14 = 0; //fast slew rate RXER
		pGPIO->GPCSR.bREG.GP16 = 0; //fast slew rate COL
		pGPIO->GPCSR.bREG.GP17 = 0; //fast slew rate CRS
		
		pGPIO->GPCSR.bREG.GP22 = 0; //fast slew rate TXD4
		pGPIO->GPCSR.bREG.GP23 = 0; //fast slew rate TXD5
		pGPIO->GPCSR.bREG.GP24 = 0; //fast slew rate TXD6
		pGPIO->GPCSR.bREG.GP25 = 0; //fast slew rate TXD7
		
		pGPIO->GPCSR.bREG.GP18 = 0; //fast slew rate RXD4
		pGPIO->GPCSR.bREG.GP19 = 0; //fast slew rate RXD5
		pGPIO->GPCSR.bREG.GP20 = 0; //fast slew rate RXD6
		pGPIO->GPCSR.bREG.GP21 = 0; //fast slew rate RXD7
#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
#endif

#else
#define GMAC_DRV_STRENGTH 3
#define GMAC_DRV_STRENGTH_2 2

		//Setup GPIO Strength
		tcc_gpio_config(GPIO_GMAC_MDC, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_MDIO, GPIO_CD(GMAC_DRV_STRENGTH));

		tcc_gpio_config(GPIO_GMAC_TXD0, GPIO_CD(GMAC_DRV_STRENGTH_2));
		tcc_gpio_config(GPIO_GMAC_TXD1, GPIO_CD(GMAC_DRV_STRENGTH_2));
		tcc_gpio_config(GPIO_GMAC_TXD2, GPIO_CD(GMAC_DRV_STRENGTH_2));
		tcc_gpio_config(GPIO_GMAC_TXD3, GPIO_CD(GMAC_DRV_STRENGTH_2));
		tcc_gpio_config(GPIO_GMAC_TXEN, GPIO_CD(GMAC_DRV_STRENGTH_2));

		tcc_gpio_config(GPIO_GMAC_RXCLK, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXD0, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXD1, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXD2, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXD3, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXDV, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_TXCLK, GPIO_CD(GMAC_DRV_STRENGTH_2));

#if defined(CONFIG_TCC_GMAC_MII_MODE)
		tcc_gpio_config(GPIO_GMAC_TXER, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXER, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_COL, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_CRS, GPIO_CD(GMAC_DRV_STRENGTH));


#elif defined(CONFIG_TCC_GMAC_GMII_MODE)
		tcc_gpio_config(GPIO_GMAC_TXER, GPIO_CD(GMAC_DRV_STRENGTH));
	
		tcc_gpio_config(GPIO_GMAC_RXER, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_COL, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_CRS, GPIO_CD(GMAC_DRV_STRENGTH));

		tcc_gpio_config(GPIO_GMAC_TXD4, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_TXD5, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_TXD6, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_TXD7, GPIO_CD(GMAC_DRV_STRENGTH));

		tcc_gpio_config(GPIO_GMAC_RXD4, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXD5, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXD6, GPIO_CD(GMAC_DRV_STRENGTH));
		tcc_gpio_config(GPIO_GMAC_RXD7, GPIO_CD(GMAC_DRV_STRENGTH));
#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
#endif
#endif
		if(machine_is_tcc8920st()){
			tcc_gpio_config(TCC_GPC(9), GPIO_FN(0));
			tcc_gpio_config(GPIO_PHY_ON, GPIO_FN(0));
			
			gpio_request(TCC_GPC(9), "PHY_RST");
			gpio_request(GPIO_PHY_ON, "PHY_ON");
			
			gpio_direction_output(TCC_GPC(9), 0);
			gpio_direction_output(GPIO_PHY_ON, 0);
		}
		else{
			if(system_rev == 0x1006){//tcc8925
				tcc_gpio_config(TCC_GPC(9), GPIO_FN(0));
				tcc_gpio_config(GPIO_E_PHY_ON, GPIO_FN(0));
				
				gpio_request(TCC_GPC(9), "PHY_RST");
				gpio_request(GPIO_E_PHY_ON, "PHY_ON");
				
				gpio_direction_output(TCC_GPC(9), 0);
				gpio_direction_output(GPIO_E_PHY_ON, 0);
				
			}
			else{
				tcc_gpio_config(TCC_GPC(28), GPIO_FN(0));
				tcc_gpio_config(GPIO_PHY_ON, GPIO_FN(0));
				
				gpio_request(TCC_GPC(28), "PHY_RST");
				gpio_request(GPIO_PHY_ON, "PHY_ON");
				
				gpio_direction_output(TCC_GPC(28), 0);
				gpio_direction_output(GPIO_PHY_ON, 0);
			}
		}

	    pHSIOCFG->ETHER_CFG1.bREG.CE = 0; //Clock Disable
#if defined(PHY_INFSEL_ONLY_MODE) //PHY_INFSEL Only Mode
	#if defined(CONFIG_TCC_GMAC_MII_MODE)
	    pHSIOCFG->ETHER_CFG1.bREG.PHY_INFSEL = 6;
	#elif defined(CONFIG_TCC_GMAC_GMII_MODE)
	    pHSIOCFG->ETHER_CFG1.bREG.PHY_INFSEL = 0;
	#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
	    pHSIOCFG->ETHER_CFG1.bREG.PHY_INFSEL = 1;
#endif
#else //HSIOBUS CFG Mode
	#if defined(CONFIG_TCC_GMAC_MII_MODE)
	#elif defined(CONFIG_TCC_GMAC_GMII_MODE)
	#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
	    pHSIOCFG->ETHER_CFG1.bREG.PHY_INFSEL = 1;
	    pHSIOCFG->ETHER_CFG0.bREG.TR = 1;
	    pHSIOCFG->ETHER_CFG0.bREG.RR = 1;
	    pHSIOCFG->ETHER_CFG0.bREG.TXDIV = 0;
	    pHSIOCFG->ETHER_CFG0.bREG.RXDIV = 0;
	    pHSIOCFG->ETHER_CFG1.bREG.TCO = 1;
	#endif
#endif
	    pHSIOCFG->ETHER_CFG1.bREG.CE = 1; //Clock Enable
	}
}

static void tca_gmac_irq_init(void)
{
	volatile PPIC pPIC= (volatile PPIC) tcc_p2v(HwPIC_BASE);
	
	pPIC->INTMSK1.bREG.GMAC = 1;
	pPIC->SEL1.bREG.GMAC = 1;
	pPIC->POL1.bREG.GMAC = 0; // Active Low
	pPIC->MODE1.bREG.GMAC = 1; // Level Triggered
	pPIC->SYNC1.bREG.GMAC = 1; //Sync to SMU Clk
}

static void tcc_gmac_set_clk_rate(struct tcc_gmac_priv *priv, unsigned int clk_rate)
{
#if defined(PHY_INFSEL_ONLY_MODE) //PHY_INFSEL Only Mode
	switch(clk_rate) {
		case 1250000:
			//priv->hw->clk_rate = GMII_CLK_RANGE_100_150M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
		case 250000:
			//priv->hw->clk_rate = GMII_CLK_RANGE_20_35M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
		case 25000:
			//priv->hw->clk_rate = GMII_CLK_RANGE_20_35M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
		default:
			//priv->hw->clk_rate = GMII_CLK_RANGE_100_150M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
	}
#else
	volatile PHSIOBUSCFG pHSIOCFG= (volatile PHSIOBUSCFG)tcc_p2v(HwHSIOBUSCFG_BASE);

	#if defined(CONFIG_TCC_GMAC_RGMII_MODE)
	switch(clk_rate) {
		case 1250000:
	    	pHSIOCFG->ETHER_CFG1.bREG.CE = 0; //Clock Disable
			pHSIOCFG->ETHER_CFG0.bREG.TXDIV = 0;
	    	pHSIOCFG->ETHER_CFG1.bREG.CE = 1; //Clock Enable
			//priv->hw->clk_rate = GMII_CLK_RANGE_100_150M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
		case 250000:
	    	pHSIOCFG->ETHER_CFG1.bREG.CE = 0; //Clock Disable
			pHSIOCFG->ETHER_CFG0.bREG.TXDIV = 4;
	    	pHSIOCFG->ETHER_CFG1.bREG.CE = 1; //Clock Enable
			//priv->hw->clk_rate = GMII_CLK_RANGE_20_35M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
		case 25000:
	    	pHSIOCFG->ETHER_CFG1.bREG.CE = 0; //Clock Disable
			pHSIOCFG->ETHER_CFG0.bREG.TXDIV = 49;
	    	pHSIOCFG->ETHER_CFG1.bREG.CE = 1; //Clock Enable
			//priv->hw->clk_rate = GMII_CLK_RANGE_20_35M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
		default:
			//priv->hw->clk_rate = GMII_CLK_RANGE_100_150M;
			priv->hw->clk_rate = GMII_CLK_RANGE_35_60M;
			break;
	}
	#endif
	
#endif
}

#endif /*_TCA_GMAC_H_*/
