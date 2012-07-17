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

#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <asm/mach-types.h>

extern const struct tcc_freq_table_t gtHSIOClockLimitTable;

static struct clk *hsio_clk=NULL;
static struct clk *gmac_clk=NULL;
static void tca_gmac_clk_init(void)
{
	hsio_clk = clk_get(NULL, "hsio");
	BUG_ON(hsio_clk == NULL);

	gmac_clk = clk_get(NULL, "gmac");
	BUG_ON(gmac_clk == NULL);

}

static void tca_gmac_clk_enable(void)
{
	if(hsio_clk) {
		clk_enable(hsio_clk);
#if defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtHSIOClockLimitTable, TCC_FREQ_LIMIT_GMAC, 1);
#endif
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

	if(hsio_clk) {
#if defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtHSIOClockLimitTable, TCC_FREQ_LIMIT_GMAC, 0);
#endif
		clk_disable(hsio_clk);
	}
}

static void tca_gmac_phy_pwr_on(void)
{
	volatile PGPIO pGPIO= (PGPIO)tcc_p2v(HwGPIO_BASE);
	if(machine_is_tcc8800() || machine_is_tcc8800st()) 
	{
		BITSET(pGPIO->GPGDAT, Hw29);
	}
	else if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm())
	{
		BITSET(pGPIO->GPFDAT, Hw29); //Phy Chip VDD on
	}
	msleep(10);
}

static void tca_gmac_phy_pwr_off(void)
{
	volatile PGPIO pGPIO= (PGPIO)tcc_p2v(HwGPIO_BASE);

	if(machine_is_tcc8800() || machine_is_tcc8800st()) 
	{
		BITCLR(pGPIO->GPGDAT, Hw29);
	}
	else if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm())
	{
		BITCLR(pGPIO->GPFDAT, Hw29); //Phy Chip VDD on
	}
}

static void tca_gmac_phy_reset(void)
{
	volatile PGPIO pGPIO= (PGPIO)tcc_p2v(HwGPIO_BASE);

	if(machine_is_tcc8800() || machine_is_tcc8800st()) 
	{
		BITCLR(pGPIO->GPGDAT, Hw28); //Phy Reset On
		msleep(10);
		BITSET(pGPIO->GPGDAT, Hw28); //Phy Reset Off
		msleep(300);
	}
	else if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm())
	{
		BITCLR(pGPIO->GPFDAT, Hw27); //Phy Reset On
		msleep(10);
		BITSET(pGPIO->GPFDAT, Hw27); //Phy Reset Off
		msleep(500);
	}
}

static void tca_gmac_portinit(void)
{
	volatile PHSIOBUSCFG pHSIOCFG= (PHSIOBUSCFG)tcc_p2v(HwHSIOBUSCFG_BASE);
	volatile PGPIO pGPIO= (PGPIO)tcc_p2v(HwGPIO_BASE);

	if(machine_is_tcc8800() || machine_is_tcc8800st()) 
	{
		/* Port Confiugration for GMAC (GPIOG 0~27)*/
		BITCSET(pGPIO->GPGFN0, Hw20-Hw0, Hw16|Hw12|Hw8|Hw4|Hw0);//0~4
		BITCSET(pGPIO->GPGFN0, Hw32-Hw24, Hw28|Hw24);//6~7
		BITCSET(pGPIO->GPGFN1, Hw32-Hw0, Hw28|Hw24|Hw20|Hw16|Hw12|Hw8|Hw4|Hw0);//8~15
		if(machine_is_tcc8800st())
		{
			BITCSET(pGPIO->GPGFN2, Hw16-Hw4, Hw12|Hw8|Hw4);//17~19
		}
		else{
			BITCSET(pGPIO->GPGFN2, Hw16-Hw0, Hw12|Hw8|Hw4|Hw0);//16~19
		}

#if 1
		/* Driver Strength */
		#if 0 //Strengh0
		BITCLR(pGPIO->GPGCD0, Hw32-Hw0);
		BITCLR(pGPIO->GPGCD1, Hw24-Hw0);
		#endif
		#if 0 //Strengh1
		BITCSET(pGPIO->GPGCD0, Hw32-Hw0, Hw30|Hw28|Hw26|Hw24|Hw22
										|Hw20|Hw18|Hw16|Hw14|Hw12
										|Hw10|Hw8|Hw6|Hw4|Hw2|Hw0);
		BITCSET(pGPIO->GPGCD1, Hw24-Hw0, Hw22|Hw20
										|Hw18|Hw16|Hw14|Hw12|Hw10
										|Hw8 |Hw6 |Hw4 |Hw2 |Hw0);
		#endif
		#if 0 //Strengh2
		BITCSET(pGPIO->GPGCD0, Hw32-Hw0, Hw31
										|Hw29|Hw27|Hw25|Hw23|Hw21
										|Hw19|Hw17|Hw15|Hw13|Hw11
										|Hw9|Hw7|Hw5|Hw3|Hw1);
		BITCSET(pGPIO->GPGCD1, Hw24-Hw0, Hw23|Hw21
										|Hw19|Hw17|Hw15|Hw13|Hw11
										|Hw9 |Hw7 |Hw5 |Hw3 |Hw1);
		#endif
		#if 1 //Strengh1
		if(machine_is_tcc8800st())
		{
			/*
			BITCLR(pGPIO->GPGCD0, Hw10-Hw0);
			BITCLR(pGPIO->GPGCD0, Hw32-Hw12);
			BITCLR(pGPIO->GPGCD1, Hw8-Hw2);
			*/
			/*
			BITCSET(pGPIO->GPGCD0,   Hw10-Hw0, Hw8|Hw6|Hw4|Hw2|Hw0);//0~4
			BITCSET(pGPIO->GPGCD0,	Hw32-Hw12, Hw30|Hw28|Hw26|Hw24|Hw22|Hw20|Hw18|Hw16|Hw14|Hw12);//6~15

			BITCSET(pGPIO->GPGCD1, Hw8-Hw2, Hw6 |Hw4 |Hw2);//17~19
*/
			//strength 2
			/*
			BITCSET(pGPIO->GPGCD0,   Hw10-Hw0, Hw9|Hw7|Hw5|Hw3|Hw1);//0~4
			BITCSET(pGPIO->GPGCD0,	Hw32-Hw12, Hw31|Hw29|Hw27|Hw25|Hw23|Hw21|Hw19|Hw17|Hw15|Hw13);//6~15

			BITCSET(pGPIO->GPGCD1, Hw8-Hw2, Hw7 |Hw5 |Hw3);//17~19
			*/

			//strength 3
			
			BITSET(pGPIO->GPGCD0, Hw10-Hw0);// 0~4
			BITSET(pGPIO->GPGCD0, Hw32-Hw12);// 6  ~ 15
			BITSET(pGPIO->GPGCD1, Hw8-Hw2);//17~19
			
			
		}
		else{//Strengh3
			BITSET(pGPIO->GPGCD0, Hw10-Hw0);// 0~4
			BITSET(pGPIO->GPGCD0, Hw32-Hw12);// 6  ~ 15
			BITSET(pGPIO->GPGCD1, Hw24-Hw0);
		}
		#endif
#endif

		/* Port Configuration for VDD, Phy Rest pin */
		BITCLR(pGPIO->GPGFN3, (Hw24-Hw16)); //GPIO Mode
		BITSET(pGPIO->GPGEN, Hw29|Hw28); // Output Enable
		BITCLR(pGPIO->GPGIEN, Hw29|Hw28); //Input Disable

		BITCLR(pGPIO->GPGPD1, Hw25|Hw24); //Reset Pin Pull-up/down disable

#if defined(CONFIG_TCC_GMAC_MII_MODE)||defined(CONFIG_TCC_GMAC_GMII_MODE)
		BITSET(pHSIOCFG->ETHER_CFG, 0x130);
#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
		BITSET(pHSIOCFG->ETHER_CFG, 0x121);
#endif
	}
	else if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm()) 
	{
			/* Port Confiugration for GMAC (GPIOF 8~26)*/
		//	BITCSET(pGPIO->GPFFN0, Hw32-Hw0, (Hw29|Hw28) | (Hw25|Hw24) | (Hw21|Hw20) | (Hw17|Hw16) 
		//									  | (Hw13|Hw12) | (Hw9 |Hw8 ) | (Hw5 |Hw4 ) | (Hw1 |Hw0 ));
			BITCSET(pGPIO->GPFFN1, Hw32-Hw0, (Hw29|Hw28) | (Hw25|Hw24) | (Hw21|Hw20) | (Hw17|Hw16) 
											  | (Hw13|Hw12) | (Hw9 |Hw8 ) | (Hw5 |Hw4 ) | (Hw1 |Hw0 ));
		if(machine_is_tcc9300st()){
			//port 17 gpio for SD 
			BITCSET(pGPIO->GPFFN2, Hw32-Hw0, (Hw29|Hw28) | (Hw25|Hw24) | (Hw21|Hw20) | (Hw17|Hw16) 
											  | (Hw13|Hw12) | (Hw9 |Hw8 ) | (Hw1 |Hw0 ));
		}
		else{
			BITCSET(pGPIO->GPFFN2, Hw32-Hw0, (Hw29|Hw28) | (Hw25|Hw24) | (Hw21|Hw20) | (Hw17|Hw16) 
											  | (Hw13|Hw12) | (Hw9 |Hw8 ) | (Hw5 |Hw4 ) | (Hw1 |Hw0 ));
		}
			BITCSET(pGPIO->GPFFN3, Hw32-Hw0, (Hw9 |Hw8 ) | (Hw5 |Hw4 ) | (Hw1 |Hw0 ));
		
#if 0
			BITSET(pGPIO->GPFPD0, Hw32-Hw0); //Pull-up Enable
			BITSET(pGPIO->GPFPD1, Hw22-Hw0); //Pull-up Enable
#endif
		
#if 0
			BITCSET(pGPIO->GPFPD0, Hw32-Hw0, Hw30|Hw28|Hw26|Hw24|Hw22
											|Hw20|Hw18|Hw16|Hw14|Hw12
											|Hw10|Hw8|Hw6|Hw4|Hw2|Hw0); //Pull-Down Enable
			BITCSET(pGPIO->GPFPD1, Hw22-Hw0, Hw20|Hw18|Hw16|Hw14|Hw12
											|Hw10|Hw8|Hw6|Hw4|Hw2|Hw0); //Pull-Down Enable
#endif
		
#if 1
			/* Driver Strength */
			//BITSET(pGPIO->GPFCD0, Hw32-Hw0);
			//BITSET(pGPIO->GPFCD1, Hw22-Hw0);
			// 8 ~ 15 settting
			BITCSET(pGPIO->GPFCD0, Hw32-Hw16,  Hw30|Hw28|Hw26|Hw24|Hw22
											|Hw20|Hw18|Hw16);
			// 16 ~ 26 setting
			BITCSET(pGPIO->GPFCD1, Hw22-Hw0,  Hw20|Hw18|Hw16|Hw14|Hw12
											|Hw10|Hw8|Hw6|Hw4|Hw2|Hw0);
#endif
		
			/* Port Configuration for VDD, Phy Rest pin */
			BITCLR(pGPIO->GPFFN3, (Hw24-Hw20) | (Hw16-Hw12)); //GPIO Mode
			BITSET(pGPIO->GPFEN, Hw29|Hw27); // Output Enable
			BITCLR(pGPIO->GPFIEN, Hw29|Hw27); //Input Disable
		
			BITCLR(pGPIO->GPFPD1, Hw23|Hw22); //Reset Pin Pull-up/down disable
		
#if defined(CONFIG_TCC_GMAC_MII_MODE)||defined(CONFIG_TCC_GMAC_GMII_MODE)
			BITCSET(pHSIOCFG->ETHER_CFG, Hw6-Hw0, Hw4|Hw5);
#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
			BITCSET(pHSIOCFG->ETHER_CFG, Hw6-Hw0, Hw1 | Hw5);
#endif

	}
	
}

static void tca_gmac_irq_init(void)
{
	volatile PPIC pPIC= (volatile PPIC) tcc_p2v(HwPIC_BASE);
	
	if(machine_is_tcc8800() || machine_is_tcc8800st()) 
	{
		BITSET(pPIC->INTMSK1, 1 <<(INT_GMAC));
		BITSET(pPIC->SEL1, 1 <<(INT_GMAC));
		BITCLR(pPIC->POL1, 1 <<(INT_GMAC));	// Active Low
		BITSET(pPIC->MODE1, 1 <<(INT_GMAC));	// Level Triggered
		BITSET(pPIC->SYNC1, 1 <<(INT_GMAC));	// Sync to SMU Clk
	}
	else if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm()) 
	{
		BITSET(pPIC->INTMSK1, 1 <<(INT_GMAC-32));
		BITSET(pPIC->SEL1, 1 <<(INT_GMAC-32));
		BITCLR(pPIC->POL1, 1 <<(INT_GMAC-32));	// Active Low
		BITSET(pPIC->MODE1, 1 <<(INT_GMAC-32)); // Level Triggered
		BITSET(pPIC->SYNC1, 1 <<(INT_GMAC-32)); // Sync to SMU Clk
	}
	
}

static void tcc_gmac_set_clk_rate(struct tcc_gmac_priv *priv, unsigned int clk_rate)
{
	unsigned int mClock = clk_rate;
	
	switch(clk_rate) {
		case 1250000:
			priv->hw->clk_rate = GMII_CLK_RANGE_100_150M;
			break;
		case 250000:
			priv->hw->clk_rate = GMII_CLK_RANGE_20_35M;
			break;
		case 25000:
			priv->hw->clk_rate = GMII_CLK_RANGE_20_35M;
			break;
		default:
			mClock = 1250000;
			priv->hw->clk_rate = GMII_CLK_RANGE_100_150M;
			break;
	}
	
	if(gmac_clk) {
		clk_set_rate(gmac_clk, mClock*100);		// Unit. Hz
	}
	
}

#endif /*_TCA_GMAC_H_*/
