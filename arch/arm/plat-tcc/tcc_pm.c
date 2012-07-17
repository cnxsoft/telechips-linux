/*
 * arch/arm/plat-tcc/tcc_pm.c  
 *
 * Author:  <linux@telechips.com>
 * Created: April 21, 2008
 * Description: LINUX POWER MANAGEMENT FUNCTIONS
 *
 * Copyright (C) 2008-2009 Telechips 
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *
 * ChangeLog:
 *
 *	Nov, 2008	:	Added TCC79X Suspend and Sleep functions
 *	Oct, 2008	:	Added TCC83X Suspend functions
 *	Apr, 2008	:	Added TCC83X Sleep functions
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/syscalls.h>		// sys_sync()
#include <asm/io.h>
#include <asm/tlbflush.h>
#include <asm/cacheflush.h>		// local_flush_tlb_all(), flush_cache_all();

#if defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#elif defined(CONFIG_ARCH_TCC83X)
#include <asm/arch/tcc83x_virt.h>
#endif
#include "tcc_pm.h"

#ifdef CONFIG_PM
extern void tca_bkl_powerup(void);
extern void IO_ARM_SaveREG(int sram_addr, void *, void *);

typedef void (* lpfunc)(void);
static lpfunc lpSelfRefresh;

/************************************************************************************************
* FUNCTION		: sdram_self_refresh
*
* DESCRIPTION	:  
*
************************************************************************************************/
static void sdram_self_refresh(void)
{
	// Change CPU clock Source to XIN
	// Clock Divided by Maximum value
	HwCLKDIVC |= 0xBF00;
	// Change CPU clock Source to XIN
	HwCLKCTRL = (HwCLKCTRL & ~0xF) | HwCLKCTRL_CKSEL_Fxindiv;
	// Disable PLL
	HwPLL0CFG |= HwPLL0CFG_PD_DIS;
	HwPLL1CFG |= HwPLL1CFG_PD_DIS;

#if defined(CONFIG_ARCH_TCC79X)
	BITCSET(HwPORTCFG5, Hw31|Hw30|Hw29|Hw28, Hw28);
	BITCSET(HwCPUD4, Hw23|Hw22, Hw22);
	BITSET(HwGPFEN, Hw28);
#endif

	/*
	 * SDRAM into Self Refresh
	 */
	HwSDCFG |= 0x1;
	while ((HwSDFSM & 0xF) != 0xA);

	/*
	 * Disable SDR CLK
	 */
	HwMCFG &= ~((1<<6) | (1<<1));

	/*
	 * SHDN Pin Low
	 */
#if defined(CONFIG_ARCH_TCC79X)
	while (!(HwGPADAT & Hw3)) HwGPFDAT &= ~Hw28;
	while (1) HwGPFDAT &= ~Hw28;
#elif defined(CONFIG_ARCH_TCC83X)
	HwGDATA_A &= ~Hw11;
	while (1) HwGDATA_A &= ~Hw11;
#endif
}

/************************************************************************************************
* FUNCTION		: sleep_mode_on 
*
* DESCRIPTION	: This Function have to be executed in SRAM. 
*                 All Registers related to Clock are Backed up. 
*
************************************************************************************************/
static void sleep_mode_on(int input)
{
#if defined(CONFIG_ARCH_TCC79X)
// Declare Pointers to backup CKC Registers
	unsigned long *BAKCLKCTRL, *BAKPLL0CFG, *BAKPLL1CFG, *BAKCLKDIVC, *BAKCLKDIVC1, *BAKMODECTR;
	unsigned long *BAKBCLKCTR0, *BAKBCLKCTR1, *BAKSWRESET0, *BAKSWRESET1;
	unsigned long *BAKBCLKMSKE0, *BAKBCLKMSKE1, *BAKPCK, *i, *j, *pPCK;
	unsigned long *BAWDTCTRL, *BABCKVCFG, *BAMCLKCTRL, *BASCLKCTRL;

	// Let CPU Speed Lower, Low Clock Operation
	// Assign Registers to Pointers
	BAKCLKCTRL   = (unsigned long *)(SRAM_ADDR_VAR);
	BAKPLL0CFG   = (unsigned long *)(SRAM_ADDR_VAR + 0x04);
	BAKPLL1CFG   = (unsigned long *)(SRAM_ADDR_VAR + 0x08);
	BAKCLKDIVC   = (unsigned long *)(SRAM_ADDR_VAR + 0x0C);
	BAKCLKDIVC1  = (unsigned long *)(SRAM_ADDR_VAR + 0x10);
	BAKMODECTR   = (unsigned long *)(SRAM_ADDR_VAR + 0x14);
	BAKBCLKCTR0  = (unsigned long *)(SRAM_ADDR_VAR + 0x18);
	BAKBCLKCTR1  = (unsigned long *)(SRAM_ADDR_VAR + 0x1C);
	BAKSWRESET0  = (unsigned long *)(SRAM_ADDR_VAR + 0x20);
	BAKSWRESET1  = (unsigned long *)(SRAM_ADDR_VAR + 0x24);
	BAWDTCTRL    = (unsigned long *)(SRAM_ADDR_VAR + 0x28);
	
	BABCKVCFG    = (unsigned long *)(SRAM_ADDR_VAR + 0xE0);
	BAMCLKCTRL   = (unsigned long *)(SRAM_ADDR_VAR + 0xE8);
	BASCLKCTRL   = (unsigned long *)(SRAM_ADDR_VAR + 0xEC);
	
	BAKBCLKMSKE0 = (unsigned long *)(SRAM_ADDR_VAR + 0xF0);
	BAKBCLKMSKE1 = (unsigned long *)(SRAM_ADDR_VAR + 0xF4);

	i = (unsigned long *)(SRAM_ADDR_VAR + 0xF8);
	j = (unsigned long *)(SRAM_ADDR_VAR + 0xFC);

	BAKPCK = (unsigned long *)(SRAM_ADDR_VAR + 0x80);
	
	// Backup Related Registers
	*BAKMODECTR   = HwMODECTR;
	*BAKPLL0CFG   = HwPLL0CFG;
	*BAKPLL1CFG   = HwPLL1CFG;
	*BAKCLKCTRL   = HwCLKCTRL;
	*BAKCLKDIVC   = HwCLKDIVC;
	*BAKCLKDIVC1  = HwCLKDIVC1;
	*BAKSWRESET0  = HwSWRESET;
	*BAKSWRESET1  = HwSWRESET1;
	
	*BABCKVCFG    = HwBCKVCFG;
	*BAMCLKCTRL   = HwMCLKCTRL;
	*BASCLKCTRL   = HwSCLKCTRL;
	
	*BAKBCLKMSKE0 = HwBCLKMASKE;
	*BAKBCLKMSKE1 = HwBCLKMASKE1;
	
	// SDRAM into Self Refresh
	HwSDCFG	|=	0x1;
	//while ((HwMCFG & 0xA0000) != 0xA0000);
	while((HwSDFSM & 0xF) != 0xA);
	
	// Disable SDR CLK
	HwMCFG &= ~((1<<6) | (1<<1));

	// Save BCLKCTR and enable all of bus clock
	*BAKBCLKCTR0	=	HwBCLKCTR;
	*BAKBCLKCTR1	=	HwBCLKCTR1;

	HwBCLKCTR = 0xFFFFFFFF;
	HwBCLKCTR1 = 0xFFFFFFFF;

	// Save All of PCK_XXX Register
	pPCK = (unsigned long *)(&(HwPCK_USB11H));
	for(*i = 0; *i < 24; (*i)++ ) {
		BAKPCK[*i] = *pPCK;
		if(((BAKPCK[*i] & 0x1f000000) != 0x16000000) && 
				((BAKPCK[*i] & 0x1f000000) != 0x17000000)){
			*pPCK = (BAKPCK[*i] & ~0x10000000);
		}
		pPCK ++;
	}

	// Clock Divided by Maximum value
	HwCLKDIVC |= 0xBF00;

	// Change CPU clock Source to XIN
	HwCLKCTRL = (HwCLKCTRL & ~0xF) | HwCLKCTRL_CKSEL_Fxindiv;
	
	// Disable PLL
	HwPLL0CFG |= HwPLL0CFG_PD_DIS;
	HwPLL1CFG |= HwPLL1CFG_PD_DIS;
	
	// Enter PowerDown mode, Set MODECTR to PWDN mode.
	HwMODECTR |= (Hw8|Hw9);

	for((*i) = 10; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10;
		
	// Restore MODECTR register
	HwMODECTR = *BAKMODECTR;

	// Restore PLL
	HwPLL0CFG = *BAKPLL0CFG;
	HwPLL1CFG = *BAKPLL1CFG;

	// wait for PLL normal
	for((*i) = 2; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10; 

	// Restore CLKCTR
	HwCLKCTRL  = *BAKCLKCTRL;
	HwCLKDIVC  = *BAKCLKDIVC;
	HwCLKDIVC1 = *BAKCLKDIVC1;
	
	// Restore PCK_XXX
	pPCK = (unsigned long *)(&HwPCK_USB11H);
	for((*i) = 0; (*i) < 24; (*i)++)
		*pPCK++ = BAKPCK[*i];

	// wait for PCLK normal
	for((*i) = 2; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10;

	// Restore BCLKCTR
	HwSWRESET    = *BAKSWRESET0;
	HwSWRESET1   = *BAKSWRESET1;
 	
	HwBCLKMASKE  = *BAKBCLKMSKE0;
	HwBCLKMASKE1 = *BAKBCLKMSKE1;

	HwBCLKCTR    = *BAKBCLKCTR0;
	HwBCLKCTR1   = *BAKBCLKCTR1;
	
	HwBCKVCFG    = *BABCKVCFG ;
	HwMCLKCTRL   = *BAMCLKCTRL;
	HwSCLKCTRL   = *BASCLKCTRL;

	// SDRAM exit Self Refresh
	//HwMCFG |= ((1<<6) | (1<<1));
	HwMCFG |= (1<<1);
	HwSDCFG	&= ~0x1;

	// Wait until SDRAM stable
	//while ((HwMCFG & 0xA0000) != 0xA0000);

	// wait for SDRAM normal
	for((*i) = 2; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10;
	(*i) = 0;
	
#elif defined(CONFIG_ARCH_TCC83X)
	// Declare Pointers to backup CKC Registers
	unsigned long *BAKCLKCTRL, *BAKPLL0CFG,	*BAKPLL1CFG, *BAKCLKDIVC, *BAKMODECTR;
	unsigned long *BAKBCLKCTR0,	*BAKBCLKCTR1, *BAKSWRESET0,	*BAKSWRESET1;
	unsigned long *BAKPCK, *i, *j, *pPCK;

	// Let CPU Speed Lower, Low Clock Operation
	// Assign Registers to Pointers
	BAKCLKCTRL  = (unsigned long *)(SRAM_ADDR_VAR);
	BAKPLL0CFG  = (unsigned long *)(SRAM_ADDR_VAR + 0x04);
	BAKPLL1CFG  = (unsigned long *)(SRAM_ADDR_VAR + 0x08);
	BAKCLKDIVC  = (unsigned long *)(SRAM_ADDR_VAR + 0x0C);
	BAKMODECTR  = (unsigned long *)(SRAM_ADDR_VAR + 0x10);
	BAKBCLKCTR0	= (unsigned long *)(SRAM_ADDR_VAR + 0x14);
	BAKBCLKCTR1	= (unsigned long *)(SRAM_ADDR_VAR + 0x60);
	BAKSWRESET0	= (unsigned long *)(SRAM_ADDR_VAR + 0x18);
	BAKSWRESET1	= (unsigned long *)(SRAM_ADDR_VAR + 0x64);

	i = (unsigned long *)(SRAM_ADDR_VAR + 0x68);
	j = (unsigned long *)(SRAM_ADDR_VAR + 0x6C);

	BAKPCK = (unsigned long *)(SRAM_ADDR_VAR + 0x1C);

	// Backup Related Registers
	*BAKMODECTR  = HwMODECTR;
	*BAKPLL0CFG  = HwPLL0CFG;
	*BAKPLL1CFG  = HwPLL1CFG;
	*BAKCLKCTRL  = HwCLKCTRL;
	*BAKCLKDIVC  = HwCLKDIVC;
	*BAKSWRESET0 = HwSWRESET;
	*BAKSWRESET1 = HwSWRESET1;

	// SDRAM into Self Refresh
	HwSDCFG	|=	0x1;
	//while ((HwMCFG & 0xA0000) != 0xA0000); 
	while((HwSDFSM & 0xF) != 0xA);

	// Disable SDR CLK
	HwMCFG &= ~((1<<6) | (1<<1));

	// Save BCLKCTR and enable all of bus clock
	*BAKBCLKCTR0 = HwBCLKCTR;
	*BAKBCLKCTR1 = HwBCLKCTR1;
	HwBCLKCTR  = 0xFFFFFFFF;
	HwBCLKCTR1 = 0xFFFFFFFF;

	// Save All of PCK_XXX Register
	pPCK = (unsigned long *)(&(HwPCLKCFG0));
	for (*i = 0; *i < 14; (*i)++) {
		BAKPCK[*i] = *pPCK;
		if (((BAKPCK[*i] & 0xF0000000) != 0xE0000000) && 
				((BAKPCK[*i] & 0xF0000000) != 0xF0000000)){
			*pPCK = (BAKPCK[*i] & ~0x80000000);
		}
		pPCK ++;
	}

	// Clock Divided by Maximum value
	BITSET(HwCLKDIVC, 0xDF00);

	// Change CPU clock Source to XIN
	BITCSET(HwCLKCTRL, 0xF, HwCLKCTRL_CKSEL_Fxindiv);	

	// Disable PLL
	BITSET(HwPLL0CFG, HwPLL0CFG_PD_DIS);
	BITSET(HwPLL1CFG, HwPLL1CFG_PD_DIS);

	// Enter Power Down Mode & Wakeup Clock XIN Select
	BITSET(HwMODECTR,(Hw8|Hw9));

	for ((*i) = 10; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10;

	// Restore MODECTR register
	HwMODECTR =	*BAKMODECTR;

	// Restore PLL
	HwPLL0CFG =	*BAKPLL0CFG;
	HwPLL1CFG =	*BAKPLL1CFG;

	// wait for PLL normal
	for ((*i) = 2; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10; 

	// Restore CLKCTR
	HwCLKCTRL =	*BAKCLKCTRL;
	HwCLKDIVC =	*BAKCLKDIVC;

	// Restore PCK_XXX
	pPCK = (unsigned long *)(&HwPCLKCFG0);
	for ((*i) = 0; (*i) < 14; (*i)++)
		*pPCK++ = BAKPCK[*i];

	// wait for PCLK normal
	for ((*i) = 2; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10; 

	// Restore BCLKCTR
	HwSWRESET	=	*BAKSWRESET0;
	HwSWRESET1	=	*BAKSWRESET1;	
	HwBCLKCTR	=	*BAKBCLKCTR0;
	HwBCLKCTR1 	=	*BAKBCLKCTR1;

	// SDRAM exit Self Refresh
	//HwMCFG |= ((1<<6) | (1<<1));
	HwMCFG |= (1<<1);

	HwSDCFG	&= ~0x1;

	// Wait until SDRAM stable
	/*
	while ((HwMCFG & 0xA0000) != 0xA0000);	// Can not exit!
	*/

	// wait for SDRAM normal
	for( (*i) = 2; (*i) > 0 ; (*i)--)
		(*j) += (*i) / 10; 
	(*i) = 0;
#endif
}

/************************************************************************************************
* FUNCTION		: suspend_mode_on 
*
* DESCRIPTION	:  
*                  
************************************************************************************************/
static void suspend_mode_on(void)
{
#if defined(CONFIG_ARCH_TCC79X)
	pTCC79X_REG mTCC79X_REG;
	PDMA		pDMA;
	PDMA		pDMA1;
	PDMA		pDMA2;
	PGPSB		pGPSB;
	PGPIO		pGPIO;
	PCIF		pCIF;
	PI2C		pI2C;
	PEFFECT		pEFFECT;
	PCIFSACLER	pCIFSACLER;
	PSCALER		pSCALER;
	PSDRAMIF	pSDRAMIF;
	PCKC		pCKC;
	PIEN		pIEN;
	PVIC		pVIC;
	PADC		pADC;
	PTIMER		pTIMER;
	PIOBC		pIOBC;
	PSYSC		pSYSC;
	PGE			pGE;
	PLCD		pLCD;

	pDMA		= (DMA *)&HwDMA_BASECH0;
	pDMA1		= (DMA *)&HwDMA_BASECH3;
	pDMA2		= (DMA *)&HwDMA_BASECH2;
	pGPSB		= (GPSB *)&HwGPSB_CH0_BASE;
	pGPIO		= (GPIO *)&HwPORT_BASE;
	pCIF		= (CIF *)&HwCIF_BASE;
	pI2C		= (I2C *)&HwI2CM_CH0_BASE;
	pEFFECT		= (EFFECT *)&HwCEM;
	pCIFSACLER	= (CIFSACLER *)&HwCSC;
	pSCALER		= (SCALER *)&HwMSC_BASE;
	pSDRAMIF	= (SDRAMIF *)&HwSRAMIF_BASE;
	pCKC		= (CKC *)&HwCKC_BASE;
	pIEN		= (IEN *)&HwIEN;
	pVIC		= (VIC *)&HwVAIRQ;
	pADC		= (ADC *)&HwADC_BASE;
	pTIMER		= (TIMER *)&HwTMR_BASE;
	pIOBC		= (IOBC *)&HwIOBC_BASE;
	pSYSC		= (SYSC *)&HwSYSCFG_BASE;
	pGE			= (GE *)&HwGE_FCH0_BASE;
	pLCD		= (LCD *)&HwLCD_BASE;

/*
 * BACKUP REGISTER
 */
 	mTCC79X_REG = ioremap_nocache(SDRAM_STORE_ADDRESS, PAGE_ALIGN(sizeof(TCC79X_REG)));

	memcpy((char *)&mTCC79X_REG->mDMA,			pDMA,		sizeof(DMA));
	memcpy((char *)&mTCC79X_REG->mDMA1,			pDMA1,		sizeof(DMA));
	memcpy((char *)&mTCC79X_REG->mDMA2,			pDMA2,		sizeof(DMA));

	mTCC79X_REG->mNFC.NFC_CTRL  = HwNFC_CTRL;
	mTCC79X_REG->mNFC.NFC_CTRL1 = HwNFC_CTRL1;
	mTCC79X_REG->mNFC.NFC_DSIZE = HwNFC_DSIZE;
	mTCC79X_REG->mNFC.NFC_IREQ  = HwNFC_IREQ;

	memcpy((char *)&mTCC79X_REG->mI2C,			pI2C,		sizeof(I2C));
	memcpy((char *)&mTCC79X_REG->mGPSB,			pGPSB,		sizeof(GPSB));
	
	// DAI
	mTCC79X_REG->mDAI.DAMR = HwDAMR;
	mTCC79X_REG->mDAI.DAVC = HwDAVC;

	memcpy((char *)&mTCC79X_REG->mGPIO,			pGPIO,		sizeof(GPIO));

	// ECC
	mTCC79X_REG->mECC.ECC_CTRL	= HwECC_CTRL;
	mTCC79X_REG->mECC.ECC_BASE  = HwECC_BASE;
	mTCC79X_REG->mECC.ECC_MASK  = HwECC_MASK;
	mTCC79X_REG->mECC.ECC_CLEAR = HwECC_CLR;
	mTCC79X_REG->mECC.SECC      = HwSLC_ECC0;
	mTCC79X_REG->mECC.IREQ      = HwECC_IREQ;
	
	memcpy((char *)&mTCC79X_REG->mCIF,			pCIF,		sizeof(CIF));
	memcpy((char *)&mTCC79X_REG->mEFFECT,		pEFFECT,	sizeof(EFFECT));
	memcpy((char *)&mTCC79X_REG->mCIFSACLER,	pCIFSACLER, sizeof(CIFSACLER));
	memcpy((char *)&mTCC79X_REG->mSCALER,		pSCALER,	sizeof(SCALER));

	// EMC0
	mTCC79X_REG->mEMC.CSCFG0 = HwCSCFG0;
	mTCC79X_REG->mEMC.CSCFG3 = HwCSCFG3;
	mTCC79X_REG->mEMC.CLKCFG = HwCLKCFG;
	
	memcpy((char *)&mTCC79X_REG->mSDRAMIF,		pSDRAMIF,	sizeof(SDRAMIF));
	memcpy((char *)&mTCC79X_REG->mCKC,			pCKC,		sizeof(CKC));
	memcpy((char *)&mTCC79X_REG->mIEN,			pIEN,		sizeof(IEN));
	memcpy((char *)&mTCC79X_REG->mVIC,			pVIC,		sizeof(VIC));
	memcpy((char *)&mTCC79X_REG->mADC,			pADC, 		sizeof(ADC));
	memcpy((char *)&mTCC79X_REG->mTIMER,		pTIMER, 	sizeof(TIMER));
	memcpy((char *)&mTCC79X_REG->mIOBC,			pIOBC,		sizeof(IOBC));
	memcpy((char *)&mTCC79X_REG->mSYSC,			pSYSC,		sizeof(SYSC));
	memcpy((char *)&mTCC79X_REG->mGE,			pGE,		sizeof(GE));
	memcpy((char *)&mTCC79X_REG->mLCD,			pLCD,		sizeof(LCD));

	mTCC79X_REG->uMARK = BSP_SUSPEND_MASK;

	/* flush TLB and I/D cache */
	local_flush_tlb_all();
	flush_cache_all();

	IO_ARM_SaveREG(SRAM_ADDR_STANDBY, &mTCC79X_REG->uMMU, &mTCC79X_REG->uCPSR );

	__asm__ volatile ("nop\n");

/*
 * RESTORE REGISTER
 */
	memcpy(pGPIO,		(char *)&mTCC79X_REG->mGPIO,		sizeof(GPIO));

	BITCSET(HwCLKCTRL, 0x7, 0);
	HwPLL1CFG  = mTCC79X_REG->mCKC.PLL1CFG;
	HwCLKCTRL  = mTCC79X_REG->mCKC.CLKCTRL;
	HwCLKDIVC  = mTCC79X_REG->mCKC.CLKDIVC;
	HwCLKDIVC1 = mTCC79X_REG->mCKC.CLKDIVC1;
	HwBCLKCTR  = mTCC79X_REG->mCKC.BCLKCTR0;
	HwBCLKCTR1 = mTCC79X_REG->mCKC.BCLKCTR1;

	memcpy(pDMA,		(char *)&mTCC79X_REG->mDMA,			sizeof(DMA));
	memcpy(pDMA1,		(char *)&mTCC79X_REG->mDMA1,		sizeof(DMA));
	memcpy(pDMA2,		(char *)&mTCC79X_REG->mDMA2,		sizeof(DMA));

	HwNFC_CTRL	= mTCC79X_REG->mNFC.NFC_CTRL;
	HwNFC_CTRL1	= mTCC79X_REG->mNFC.NFC_CTRL1;
	HwNFC_DSIZE	= mTCC79X_REG->mNFC.NFC_DSIZE;
	HwNFC_IREQ	= mTCC79X_REG->mNFC.NFC_IREQ;

	memcpy(pGPSB ,		(char *)&mTCC79X_REG->mGPSB,		sizeof(GPSB));
	
	// DAI
	HwDAMR = mTCC79X_REG->mDAI.DAMR;
	HwDAVC = mTCC79X_REG->mDAI.DAVC;

	memcpy(pGPIO ,		(char *)&mTCC79X_REG->mGPIO,		sizeof(GPIO));

	// ECC
	HwECC_CTRL = mTCC79X_REG->mECC.ECC_CTRL;
	HwECC_BASE = mTCC79X_REG->mECC.ECC_BASE;
	HwECC_MASK = mTCC79X_REG->mECC.ECC_MASK;
	HwECC_CLR  = mTCC79X_REG->mECC.ECC_CLEAR;
	HwSLC_ECC0 = mTCC79X_REG->mECC.SECC;
	HwECC_IREQ = mTCC79X_REG->mECC.IREQ;
	
	// CIF
	memcpy(pCIF ,		(char *)&mTCC79X_REG->mCIF,			sizeof(CIF));
	memcpy(pEFFECT,		(char *)&mTCC79X_REG->mEFFECT,		sizeof(EFFECT));
	memcpy(pCIFSACLER,	(char *)&mTCC79X_REG->mCIFSACLER,	sizeof(CIFSACLER));
	memcpy(pSCALER,		(char *)&mTCC79X_REG->mSCALER,		sizeof(SCALER));

	// EMC0
	HwCSCFG0 = mTCC79X_REG->mEMC.CSCFG0;
	HwCSCFG3 = mTCC79X_REG->mEMC.CSCFG3;
	HwCLKCFG = mTCC79X_REG->mEMC.CLKCFG;

	memcpy(pI2C,		(char *)&mTCC79X_REG->mI2C,			sizeof(I2C));
	memcpy(pSDRAMIF,	(char *)&mTCC79X_REG->mSDRAMIF,		sizeof(SDRAMIF));
	memcpy(pVIC,		(char *)&mTCC79X_REG->mVIC,			sizeof(VIC));
	memcpy(pADC,		(char *)&mTCC79X_REG->mADC, 		sizeof(ADC));
	memcpy(pTIMER,		(char *)&mTCC79X_REG->mTIMER, 		sizeof(TIMER));
	memcpy(pIOBC,		(char *)&mTCC79X_REG->mIOBC,		sizeof(IOBC));
	memcpy(pSYSC,		(char *)&mTCC79X_REG->mSYSC,		sizeof(SYSC));
	memcpy(pGE,			(char *)&mTCC79X_REG->mGE,			sizeof(GE));
	memcpy(pLCD,		(char *)&mTCC79X_REG->mLCD,			sizeof(LCD));
	memcpy(pCKC,		(char *)&mTCC79X_REG->mCKC,			sizeof(CKC));

	HwINTMSK = mTCC79X_REG->mIEN.INTMSK;
	memcpy(pIEN,		(char *)&mTCC79X_REG->mIEN,			sizeof(IEN));

	iounmap(mTCC79X_REG);

#elif defined(CONFIG_ARCH_TCC83X)
	pTCC83X_REG mTCC83X_REG;
	PGPIO		pGPIO;
	PPIC		pPIC;
	PCKC		pCKC;
	PVMT		pVMT;
	PDMA		pDMA0;
	PDMA		pDMA1;
	PTIMER		pTIMER;
	PADC		pADC;
	PSCFG		pSCFG;
	PEMC		pEMC;
	PADMA		pADMA;
	PDAI		pDAI;
	PSPI		pSPI0;
	PSPI		pSPI1;
	PECCSLC		pECCSLC;
	PNFC		pNFC;
	PI2C		pI2C;
	PEHI		pEHI;
	PCIF		pCIF;
	PEFFECT		pEFFECT;
	PCIFSCALER	pCIFSCALER;
	PLCD		pLCD;
	PLCDSI0		pLCDSI0;
	PLCDSI1		pLCDSI1;
	PGE			pGE;
	PJPEG		pJPEG;
	PSCALER		pSCALER;

	pGPIO		= (GPIO *)&HwGDATA_A;
	pPIC		= (PIC *)&HwIEN;
	pCKC		= (CKC *)&HwCLKCTRL;
	pVMT		= (VMT *)&HwVMT_REGION0;
	pDMA0		= (DMA *)&HwST_SADR0;
	pDMA1		= (DMA *)&HwDMA1_ST_SADR0;
	pTIMER		= (TIMER *)&HwTCFG0;
	pADC		= (ADC *)&HwADCCON;
	pSCFG		= (SCFG *)&HwBMI;
	pEMC		= (EMC *)&HwSDCFG;
	pADMA		= (ADMA *)&HwRxDaDar;
	pDAI		= (DAI *)&HwDAI_BASE;
	pSPI0		= (SPI *)&HwSDO;
	pSPI1		= (SPI *)&HwSDO1;
	pECCSLC		= (ECCSLC *)&HwECC_CTRL;
	pNFC		= (NFC *)&HwNFC_CMD;
	pI2C		= (I2C *)&HwI2CM_CH0_BASE;
	pEHI		= (EHI *)&HwEHST;
	pCIF		= (CIF *)&HwICPCR1;
	pEFFECT		= (EFFECT *)&HwCEM;
	pCIFSCALER	= (CIFSCALER *)&HwCSC;
	pLCD		= (LCD *)&HwLCTRL;
	pLCDSI0		= (LCDSI0 *)&HwLCDSICTRL0;
	pLCDSI1		= (LCDSI1 *)&HwLCDSICTRL1;
	pGE			= (GE *)&Hw2D_FCHO_SADDR0;
	pJPEG		= (JPEG *)&HwJP_RST;					//TODO: many register is W only 
	pSCALER		= (SCALER *)&HwSRCBASEY;

/*
 * BACKUP REGISTER
 */
	mTCC83X_REG = ioremap_nocache(SDRAM_STORE_ADDRESS, PAGE_ALIGN(sizeof(TCC83X_REG)));	// Size 8KB

	memcpy((char *)&mTCC83X_REG->mLCD,			pLCD,		sizeof(LCD));
	memcpy((char *)&mTCC83X_REG->mLCDSI0,		pLCDSI0,	sizeof(LCDSI0));
	memcpy((char *)&mTCC83X_REG->mLCDSI1,		pLCDSI1,	sizeof(LCDSI1));
	memcpy((char *)&mTCC83X_REG->mGPIO,			pGPIO,		sizeof(GPIO));
	memcpy((char *)&mTCC83X_REG->mPIC,			pPIC,		sizeof(PIC));
	memcpy((char *)&mTCC83X_REG->mVMT,			pVMT,		sizeof(VMT));
	memcpy((char *)&mTCC83X_REG->mDMA0,			pDMA0,		sizeof(DMA));
	memcpy((char *)&mTCC83X_REG->mDMA1,			pDMA1,		sizeof(DMA));
	memcpy((char *)&mTCC83X_REG->mTIMER,		pTIMER,		sizeof(TIMER));
	memcpy((char *)&mTCC83X_REG->mADC,			pADC,		sizeof(ADC));
	memcpy((char *)&mTCC83X_REG->mSCFG,			pSCFG,		sizeof(SCFG));
	memcpy((char *)&mTCC83X_REG->mEMC,			pEMC,		sizeof(EMC));
	memcpy((char *)&mTCC83X_REG->mADMA,			pADMA,		sizeof(ADMA));
	memcpy((char *)&mTCC83X_REG->mDAI,			pDAI,		sizeof(DAI));
	memcpy((char *)&mTCC83X_REG->mSPI0,			pSPI0,		sizeof(SPI));
	memcpy((char *)&mTCC83X_REG->mSPI1,			pSPI1,		sizeof(SPI));
	memcpy((char *)&mTCC83X_REG->mNFC,			pNFC,		sizeof(NFC));
	memcpy((char *)&mTCC83X_REG->mI2C,			pI2C,		sizeof(I2C));
	memcpy((char *)&mTCC83X_REG->mEHI,			pEHI,		sizeof(EHI));
	memcpy((char *)&mTCC83X_REG->mCIF,			pCIF,		sizeof(CIF));
	memcpy((char *)&mTCC83X_REG->mEFFECT,		pEFFECT,	sizeof(EFFECT));
	memcpy((char *)&mTCC83X_REG->mCIFSCALER,	pCIFSCALER,	sizeof(CIFSCALER));
	memcpy((char *)&mTCC83X_REG->mGE,			pGE,		sizeof(GE));
	memcpy((char *)&mTCC83X_REG->mJPEG,			pJPEG,		sizeof(JPEG));
	memcpy((char *)&mTCC83X_REG->mSCALER,		pSCALER,	sizeof(SCALER));
	memcpy((char *)&mTCC83X_REG->mCKC,			pCKC,		sizeof(CKC));

	mTCC83X_REG->uMARK = BSP_SUSPEND_MASK;

	/* flush TLB and I/D cache */
	local_flush_tlb_all();
	flush_cache_all();

	IO_ARM_SaveREG(SRAM_ADDR_STANDBY, &mTCC83X_REG->uMMU, &mTCC83X_REG->uCPSR );

	__asm__ volatile ("nop\n");

/*
 * RESTORE REGISTER
 */
	memcpy(pGPIO,		(char *)&mTCC83X_REG->mGPIO,		sizeof(GPIO));

	// CKC
	HwPLL1CFG = mTCC83X_REG->mCKC.PLL1CFG;
	HwCLKCTRL = mTCC83X_REG->mCKC.CLKCTRL;
	HwCLKDIVC = mTCC83X_REG->mCKC.CLKDIVC;
	HwBCLKCTR = mTCC83X_REG->mCKC.BCLKCTR0;
	HwBCLKCTR1 = mTCC83X_REG->mCKC.BCLKCTR1;

	memcpy(pPIC,		(char *)&mTCC83X_REG->mPIC,			sizeof(PIC));
	memcpy(pVMT,		(char *)&mTCC83X_REG->mVMT,			sizeof(VMT));
	memcpy(pDMA0,		(char *)&mTCC83X_REG->mDMA0,		sizeof(DMA));
	memcpy(pDMA1,		(char *)&mTCC83X_REG->mDMA1,		sizeof(DMA));
	memcpy(pTIMER,		(char *)&mTCC83X_REG->mTIMER,		sizeof(TIMER));
	memcpy(pADC,		(char *)&mTCC83X_REG->mADC,			sizeof(ADC));

	//SCFG
	HwDTCMWAIT = mTCC83X_REG->mSCFG.DTCM_WAIT;
	HwECC_SEL = mTCC83X_REG->mSCFG.ECCSEL;
	HwREMAP = mTCC83X_REG->mSCFG.REMAP;
	HwNFC_ACK_SEL = mTCC83X_REG->mSCFG.NFC_ACK_SEL;
	HwGP_B_DRV = mTCC83X_REG->mSCFG.GP_B_DRV;
	HwGP_C_DRV = mTCC83X_REG->mSCFG.GP_C_DRV;
	HwGP_E_DRV = mTCC83X_REG->mSCFG.GP_E_DRV;
	HwGP_F_DRV = mTCC83X_REG->mSCFG.GP_F_DRV;
	HwGP_XA0_DRV = mTCC83X_REG->mSCFG.GP_XA0_DRV;
	HwGP_XA1_DRV = mTCC83X_REG->mSCFG.GP_XA1_DRV;
	HwGP_XD0_DRV = mTCC83X_REG->mSCFG.GP_XD0_DRV;
	HwGP_XD1_DRV = mTCC83X_REG->mSCFG.GP_XD1_DRV;
	HwUSBH_48M_SEL = mTCC83X_REG->mSCFG.USBH_48M_SEL;
	
	// EMC
	HwCSCFG0 = mTCC83X_REG->mEMC.CSCFG0;
	HwCSCFG1 = mTCC83X_REG->mEMC.CSCFG1;
	HwCSCFG2 = mTCC83X_REG->mEMC.CSCFG2;
	HwCSCFG3 = mTCC83X_REG->mEMC.CSCFG3;
	HwCLKCFG = mTCC83X_REG->mEMC.CLKCFG;

	memcpy(pADMA,		(char *)&mTCC83X_REG->mADMA,		sizeof(ADMA));
	memcpy(pDAI,		(char *)&mTCC83X_REG->mDAI,			sizeof(DAI));
	memcpy(pSPI0,		(char *)&mTCC83X_REG->mSPI0,		sizeof(SPI));
	memcpy(pSPI1,		(char *)&mTCC83X_REG->mSPI1,		sizeof(SPI));
	memcpy(pNFC,		(char *)&mTCC83X_REG->mNFC,			sizeof(NFC));
	memcpy(pI2C,		(char *)&mTCC83X_REG->mI2C,			sizeof(I2C));
	memcpy(pEHI,		(char *)&mTCC83X_REG->mEHI,			sizeof(EHI));
	memcpy(pCIF,		(char *)&mTCC83X_REG->mCIF,			sizeof(CIF));
	memcpy(pEFFECT,		(char *)&mTCC83X_REG->mEFFECT,		sizeof(EFFECT));
	memcpy(pCIFSCALER,	(char *)&mTCC83X_REG->mCIFSCALER,	sizeof(CIFSCALER));
	memcpy(pLCD,		(char *)&mTCC83X_REG->mLCD,			sizeof(LCD));
	memcpy(pLCDSI0,		(char *)&mTCC83X_REG->mLCDSI0,		sizeof(LCDSI0));
	memcpy(pLCDSI1,		(char *)&mTCC83X_REG->mLCDSI1,		sizeof(LCDSI1));
	memcpy(pGE,			(char *)&mTCC83X_REG->mGE,			sizeof(GE));
	memcpy(pJPEG,		(char *)&mTCC83X_REG->mJPEG,		sizeof(JPEG));
	memcpy(pSCALER,		(char *)&mTCC83X_REG->mSCALER,		sizeof(SCALER));

	// CKC
	memcpy(pCKC,		(char *)&mTCC83X_REG->mCKC,			sizeof(CKC));
	
	iounmap(mTCC83X_REG);	
#endif
}

static void tcc_console_init(void)
{
#if defined(CONFIG_ARCH_TCC79X)
	HwUART0_LCR  = HwUART0_LCR_EPS_EVEN | HwUART0_LCR_STB_ONE | HwUART0_LCR_WLS_8;
    HwUART0_LCR &= HwUART0_LCR_DLAB_OFF;
    HwUART0_IER  = 0;

    HwUART0_LCR |= HwUART0_LCR_DLAB_ON;
    HwUART0_FCR  = Hw5|Hw4|Hw2|Hw1|Hw0;
    HwUART0_DLL  = 28;
    HwUART0_DLM  = 0;
    HwUART0_LCR &= HwUART0_LCR_DLAB_OFF;
    HwUART0_IER  = HwUART0_IER_ERXI_EN;
    BITCLR(HwPORTCFG9, Hw15-Hw12);  // Use GPE[1] as UART 0
    BITCLR(HwPORTCFG9, Hw11-Hw8);   // Use GPE[0] as UART 0
#elif defined(CONFIG_ARCH_TCC83X)
	HwUART3_LCR  = HwUART_LCR_EPS_EVEN | HwUART_LCR_STB_ONE | HwUART_LCR_WLS_8;
	HwUART3_LCR &= HwUART_LCR_DLAB_OFF; 
	HwUART3_IER  = 0;

	HwUART3_LCR |= HwUART_LCR_DLAB_ON;
	HwUART3_FCR  = Hw5|Hw4|Hw2|Hw1|Hw0; 
	HwUART3_DLL  = 28;
	HwUART3_DLM  = 0;
	HwUART3_LCR &= HwUART_LCR_DLAB_OFF;
	HwUART3_IER  = HwUART_IER_ERXI_EN; 
	BITCSET(HwGSEL_F, Hw12|Hw13,Hw13); //UART3_RXD for GPIO_F12
#endif
}

/************************************************************************************************
* FUNCTION		: copy_func_to_sram 
*
* DESCRIPTION	: This Function copies sleep_mode_on Function to SRAM. 
* 				  mode 0: sleep mode, mode 1: suspend mode
*                  
************************************************************************************************/
static void copy_func_to_sram(int mode)
{
    volatile unsigned int *fPtr, *p;
    int i;

	/*
	 * copy function sleep_mode_on to SRAM
	 */
	if (mode == BSP_SLEEP_KEY) {
		fPtr = (volatile unsigned int *)sleep_mode_on;
		lpSelfRefresh = (lpfunc)SRAM_ADDR_STANDBY;
		p = (volatile unsigned int *)SRAM_ADDR_STANDBY;

		for (i = 0; i < SRAM_FUNC_SIZE; i++) {
			*(p++) = *(fPtr++);
		}

		while (--i)
			;

		// Jump to SRAM excute self-refresh mode
		lpSelfRefresh();

	} else if (mode == BSP_SUSPEND_KEY) {
		fPtr = (volatile unsigned int *)sdram_self_refresh;
		p = (volatile unsigned int *)SRAM_ADDR_STANDBY;

		for (i = 0; i < SRAM_FUNC_SIZE; i++) {
			*(p++) = *(fPtr++);
		}

		while (--i)
			;

		suspend_mode_on();
	}
	
}

void enter_sleep_mode(void)
{
#if defined(CONFIG_ARCH_TCC83X)
	HwWKEN_B = 0xFFFFFFFF;
#endif
	HwWKEN   = 0xFFFFFFFF;

#if defined(CONFIG_ARCH_TCC79X)
	BITCLR(HwWKEN, HwINT_EI1);		// enable EINT1 wakeup interrupt
#elif defined(CONFIG_ARCH_TCC83X)
	BITCLR(HwWKEN_B, HwWKEN_EI0);	// enable EINT0 wakeup interrupt
#endif

	copy_func_to_sram(BSP_SLEEP_KEY);

#if defined(CONFIG_ARCH_TCC83X)
	HwWKEN_B = 0xFFFFFFFF;
#endif
	HwWKEN   = 0xFFFFFFFF;
}

void enter_suspend_mode(void)
{
	copy_func_to_sram(BSP_SUSPEND_KEY);
}

/************************************************************************************************
* FUNCTION		: plat_tcc_pm
*
* DESCRIPTION	: It is called by Power key driver. 
*                  
************************************************************************************************/
int plat_tcc_pm(int mode)
{
	int err = 0;
	unsigned long save_cpsr;

	/* 
	 * if "CONFIG_USB_OHCI_HCD=y" and "TCC USBH irq = disable" 
	 * then you must enable USBH irq before "suspend device usb1"
	 */
#ifdef CONFIG_USB_OHCI_HCD
	bool save_usbh_irq;
#if defined(CONFIG_ARCH_TCC83X)
	save_usbh_irq = HwIEN & HwIEN_USBH;
	if (!save_usbh_irq)
		BITSET(HwIEN, HwIEN_USBH);
#elif defined(CONFIG_ARCH_TCC79X)
	save_usbh_irq = HwIRQMSK & HwINT_UH;
	if (!save_usbh_irq)
		BITSET(HwIRQMSK, HwINT_UH);
#endif
#endif

	/*
	 * all filesystem flush especially SD/MMC
	 */
	sys_sync();

	/*
	 * send SUSPEND message to all platform driver
	 */
	err = device_suspend(PMSG_SUSPEND);
	if (err)
		goto exit;

	/*
	 * Before enter Power Down Mode, MUST mask CPSR Irq bit
	 */
	local_irq_save(save_cpsr);
	local_irq_disable();

	if (mode == BSP_SUSPEND_KEY) {
		enter_suspend_mode();
	} else if (mode == BSP_SLEEP_KEY) {
		enter_sleep_mode();
	}

	/*
	 * backlight ON
	 */
	tca_bkl_powerup();
	 
	/*
	 * console init
	 */
	tcc_console_init();

	/*
	 * restore CPSR Irq bit
	 */
	local_irq_restore(save_cpsr);

	/*
	 * restore TCC USBH irq status
	 */
#ifdef CONFIG_USB_OHCI_HCD
#if defined(CONFIG_ARCH_TCC83X)
	if (save_usbh_irq)
		BITSET(HwIEN, HwIEN_USBH);
	else
		BITCLR(HwIEN, HwIEN_USBH);
#elif defined(CONFIG_ARCH_TCC79X)
	if (save_usbh_irq)
		BITSET(HwIRQMSK, HwINT_UH);
	else
		BITCLR(HwIRQMSK, HwINT_UH);
#endif
#endif

	/*
	 * send RESUME message to all platform driver
	 */
	device_resume(PMSG_RESUME);
	
exit:
	return err;
}
EXPORT_SYMBOL(plat_tcc_pm);

#endif	/* CONFIG_PM */

/************************************************************************************************
* FUNCTION		: plat_tcc_reboot
*
* DESCRIPTION	: system reboot. 
*                  
************************************************************************************************/
void plat_tcc_reboot(void)
{
	HwBCLKCTR = -1;

	while (1) {
#if defined(CONFIG_ARCH_TCC79X)
		HwWDTCTRL = (HwWDTCTRL_WE + 0x1); 
#elif defined(CONFIG_ARCH_TCC83X)
		HwTWDCFG = 0x49; //0x41 : TCKSEL = 0x40 , Watch Dog Enable = 0x1
#endif
	};  
}
EXPORT_SYMBOL(plat_tcc_reboot);

