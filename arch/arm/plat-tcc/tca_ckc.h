/****************************************************************************
 *   FileName    : tca_ckc.h
 *   Description : 
 ****************************************************************************
*
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
*
 ****************************************************************************/


/************************************************************************************************
*                                    Revision History                                           *
*                                                                                               *
* Version : 1.0    : 2007, 9, 17                                                                *
************************************************************************************************/

#ifndef __TCA_CKC_H__
#define __TCA_CKC_H__

// LINUX
#if defined(__KERNEL__) || defined(_LINUX_)
	#if defined(__KERNEL__)	// Kernel
		#include <asm/plat-tcc/tca_ckcstr.h>
		#if defined(_TCC79x_)
			#include <mach/tcc79x.h>
		#elif defined(_TCC83x_)
			#include <mach/tcc83x_virt.h>
		#endif
	#elif defined(_LINUX_)	// bootloader
		#include <common.h>
		#if defined(TCC79X)
			#ifndef _TCC79x_
			#define _TCC79x_
			#endif
		#elif defined(TCC83XX)
			#ifndef _TCC83x_
			#define _TCC83x_
			#endif
		#endif 
		#include <tca_ckcstr.h>
		#if defined(TCC79X)
			#include <tcc79x_phy.h>
		#elif defined(TCC83XX)
			#include <tcc83x_phy.h>
		#endif
	#endif

// WINCE
#else
	#include "tca_ckcstr.h"
	#ifdef _TCBOOT_
		#if defined(_TCC79x_) 
			#include "TCC79x_Physical.h"
		#elif defined(_TCC83x_) 
			#include "TCC83x_Physical.h"
		#endif
	#else 
		#if defined(_TCC79x_) 
			#include "TCC79x_Virtual.h"
		#elif defined(_TCC83x_) 
			#include "TCC83x_Virtual.h"
		#endif
	#endif
#endif


/************************************************************************************************
*                                        MACRO                                                 *
************************************************************************************************/
#ifndef IO_CKC_Fxin
#define  	IO_CKC_Fxin          120000      //Xin is 12MHz
#endif

typedef struct {
	unsigned			uFpll;
	unsigned char		P, M, S, dummy;
} sfPLL;
#define	PLLFREQ(P, M, S)		(( 120000 * (M) )  / (P) ) >> (S) // 100Hz Unit..

#define	FPLL_t(P, M, S)	PLLFREQ(P,M,S), P, M, S

		// PLL table for XIN=12MHz
		 // P,   M,  S
sfPLL	pIO_CKC_PLL[]	=
{
		{FPLL_t(3, 78, 1)} 			//  156MHz //{0x3, 0x82, 0x1}  
//		,{FPLL_t(3, 90, 1)}			// 180.0 MHz		
//		,{FPLL_t(3, 96, 1)}			// 192.0 MHz
//		,{FPLL_t(3, 98, 1)}			// 196.0 MHz
//		,{FPLL_t(3, 108, 1)}		// 216.0 MHz
		,{FPLL_t(3, 110, 1)}		// 220 MHz
		,{FPLL_t(3, 120, 1)}		// 240 MHz
//		,{FPLL_t(3, 125, 1)}		// 250 MHz
//		,{FPLL_t(3, 127, 1)}		// 254.0 MHz (Target = 253.642)
//		,{FPLL_t(3, 129, 1)}		// 258.0 MHz (Target = 258.048)
		,{FPLL_t(3, 130, 1)}		// 260.0 MHz (Target = 259.6608)		
		,{FPLL_t(3, 133, 1)}		// 266.0 MHz (Target = 259.6608)
//		,{FPLL_t(3, 135, 1)}		// 270.0 MHz
//		,{FPLL_t(3, 140, 1)}		// 280 MHz
//		,{FPLL_t(3, 145, 1)}		// 290 MHz
		,{FPLL_t(3, 75, 0)}			// 300 MHz
//		,{FPLL_t(3, 76, 0)}			// 304 MHz (Target = 304.8192)
//		,{FPLL_t(3, 77, 0)}			// 308 MHz (Target = 307.2)
		,{FPLL_t(3, 80, 0)}			// 320 MHz  Add PMS
		,{FPLL_t(2, 55, 0)}			// 330 MHz (Target = 307.2)
		,{FPLL_t(1, 30, 0)}			// 360 MHz 
	    ,{FPLL_t(2, 133,1)}         // 399 MHz
		,{FPLL_t(1, 40, 0)}			// 480 MHz Add PMS
	    ,{FPLL_t(2, 162, 1)}		// 486 MHz  Add PMS
//		,{FPLL_t(3, 122, 1)}		// 488 MHz 
//		,{FPLL_t(1, 41, 0)}			// 492 MHz (Target = 491.52)
		,{FPLL_t(4, 166, 0)}		// 498 MHz (Target = 496.7424)
};
	
#define	NUM_PLL	(sizeof(pIO_CKC_PLL)/sizeof(sfPLL))

#define	tca_wait()				{ volatile int i; for (i=0; i<0x2000; i++); }

/************************************************************************************************
*                                     DEFINE                                            *
************************************************************************************************/
#if defined(_TCC79x_) 
	#define	PERICLOCKENABLE	Hw28
	#define	PERICLCOKDISABLE	~Hw28
#elif defined(_TCC83x_)
	#define	PERICLOCKENABLE	Hw15
	#define	PERICLCOKDISABLE	~Hw15
#endif

unsigned int _ckc_max_cpu = 0;
unsigned int _ckc_max_bus = 0;

/************************************************************************************************
*                                        ENUM                                                   *
************************************************************************************************/
/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_checksdram(void)
{
	unsigned int	Tmp;

	Tmp = HwSDCFG;

	if(Tmp != 0)
		return 1;

	return 0;
}
/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
* ***************************************************************************************/
void tca_ckc_setsystemsource( unsigned int ClockSource )
{
	unsigned int	Tmp;

	Tmp = HwCLKCTRL;

	BITCSET(Tmp, 0x7, ClockSource);
		 
	HwCLKCTRL = Tmp;

};

/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_getsystemsource(void)
{
	unsigned int sysclksrc;

	sysclksrc = (HwCLKCTRL & 0x7);

	return sysclksrc;
};

/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
*	0 : bus clock disabled
*	1 : bus clock enabled
*
* ***************************************************************************************/
int tca_ckc_setperibus(unsigned int Controller, unsigned int Mode)
{
	unsigned int hIndex[] = {Hw0,Hw1,Hw2,Hw3,Hw4,Hw5,Hw6,Hw7,Hw8,Hw9,Hw10,Hw11,Hw12,Hw13,Hw14,Hw15
							,Hw16,Hw17,Hw18,Hw19,Hw20,Hw21,Hw22,Hw23,Hw24,Hw25,Hw26,Hw27,Hw28,Hw29,Hw30,Hw31};
	unsigned int rb_min;
	unsigned int rb_max;
	unsigned int rb_seperate;
	int retVal = 0;
	
#if defined(_TCC79x_) 
	rb_min = RB_USB11HOST;
	rb_max = RB_ALLPERIPERALS;
	rb_seperate = RB_MAINBUSCOMPO;
#elif defined(_TCC83x_)
	rb_min = RB_DAI;
	rb_max = RB_ALLPERIPERALS;
	rb_seperate = RB_ARM;
#endif

	if(Controller <  rb_min || Controller >=  rb_max)
	{
		return -1;
	}
		
	if(Controller >  rb_seperate)
	{
		Controller -=	(rb_seperate+1);
		
		if(Mode)
			BITSET(HwBCLKCTR1, hIndex[Controller]);
		else
			BITCLR(HwBCLKCTR1, hIndex[Controller]);
	}
	else
	{
		if(Mode)
			BITSET(HwBCLKCTR, hIndex[Controller]);
		else
			BITCLR(HwBCLKCTR, hIndex[Controller]);
	}
	
	retVal = 1;
	
	return retVal;
};


/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
*	0 : bus clock disabled
*	1 : bus clock enabled
*
* ***************************************************************************************/
int tca_ckc_getperibus(unsigned int Controller)
{
	unsigned int hIndex[] = {Hw0,Hw1,Hw2,Hw3,Hw4,Hw5,Hw6,Hw7,Hw8,Hw9,Hw10,Hw11,Hw12,Hw13,Hw14,Hw15
							,Hw16,Hw17,Hw18,Hw19,Hw20,Hw21,Hw22,Hw23,Hw24,Hw25,Hw26,Hw27,Hw28,Hw29,Hw30,Hw31};
	unsigned int rb_min;
	unsigned int rb_max;
	unsigned int rb_seperate;

	int retVal = 0;
	
#if defined(_TCC79x_) 
	rb_min = RB_USB11HOST;
	rb_max = RB_ALLPERIPERALS;
	rb_seperate = RB_MAINBUSCOMPO;
#elif defined(_TCC83x_)
	rb_min = RB_DAI;
	rb_max = RB_ALLPERIPERALS;
	rb_seperate = RB_ARM;
#endif

	if(Controller <  rb_min || Controller >=  rb_max)
	{
		return -1;
	}
		
	if(Controller >  rb_seperate)
	{
		Controller -=	(rb_seperate+1);

		retVal = (HwBCLKCTR1 & hIndex[Controller]) ;

	}
	else
	{
		retVal = (HwBCLKCTR & hIndex[Controller]) ;
	}

	if(retVal != 0)
		retVal = 1; // Enable
		
	return retVal;
};

/*****************************************************************************
* Function Name : tca_ckc_setbusmask()
******************************************************************************/
#ifdef _TCC79x_
int tca_ckc_setperibusmask( unsigned int Controller, unsigned int Mode)
{
   unsigned int hIndex[] = {Hw0,Hw1,Hw2,Hw3,Hw4,Hw5,Hw6,Hw7,Hw8,Hw9,Hw10,Hw11,Hw12,Hw13,Hw14,Hw15
						,Hw16,Hw17,Hw18,Hw19,Hw20,Hw21,Hw22,Hw23,Hw24,Hw25,Hw26,Hw27,Hw28,Hw29,Hw30,Hw31};
	if(Controller <  RB_USB11HOST || Controller >=  RB_ALLPERIPERALS)
	{
		return -1;
	}
		
	if(Controller >  RB_MAINBUSCOMPO)
	{
		Controller -=	RB_UART3;
		
		if(Mode)
			BITSET(HwBCLKMASKE1, hIndex[Controller]);
		else
			BITCLR(HwBCLKMASKE1, hIndex[Controller]);
	}
	else
	{
		if(Mode)
			BITSET(HwBCLKMASKE, hIndex[Controller]);
		else
			BITCLR(HwBCLKMASKE, hIndex[Controller]);
	}

	return 0;

};
#endif

/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
*	0 : reset inactive state
*	1 : reset active state
* ***************************************************************************************/
int tca_ckc_resetperi(unsigned int Controller, unsigned int Mode)
{
	unsigned int hIndex[] = {Hw0,Hw1,Hw2,Hw3,Hw4,Hw5,Hw6,Hw7,Hw8,Hw9,Hw10,Hw11,Hw12,Hw13,Hw14,Hw15
						,Hw16,Hw17,Hw18,Hw19,Hw20,Hw21,Hw22,Hw23,Hw24,Hw25,Hw26,Hw27,Hw28,Hw29,Hw30,Hw31};

	unsigned int rb_min;
	unsigned int rb_max;
	unsigned int rb_seperate;
	
#if defined(_TCC79x_) 
	rb_min = RB_USB11HOST;
	rb_max = RB_ALLPERIPERALS;
	rb_seperate = RB_MAINBUSCOMPO;
#elif defined(_TCC83x_)
	rb_min = RB_DAI;
	rb_max = RB_ALLPERIPERALS;
	rb_seperate = RB_ARM;
#endif

	if(Controller <  rb_min || Controller >=  rb_max)
	{
		return -1;
	}
		
	if(Controller >  rb_seperate)
	{
		Controller -=	(rb_seperate+1);
		
		if(Mode)
			BITSET(HwSWRESET1, hIndex[Controller]);
		else
			BITCLR(HwSWRESET1, hIndex[Controller]);
	}
	else
	{
		if(Mode)
			BITSET(HwSWRESET, hIndex[Controller]);
		else
			BITCLR(HwSWRESET, hIndex[Controller]);
	}

	
	return 0;
};

unsigned int tca_ckc_currentpll(unsigned int uCH)
{
	volatile unsigned	tPLL;
	volatile unsigned	tPLLCFG;
	unsigned	iP=0, iM=0, iS=0;
	if(uCH == 0)
		tPLLCFG = HwPLL0CFG;
	else if (uCH == 1)
		tPLLCFG = HwPLL1CFG;

	//Fpll Clock
#if defined(_TCC79x_) 
	iS	= (tPLLCFG & 0x70000) >> 16;
#elif defined(_TCC83x_)
	iS	= (tPLLCFG & 0x30000) >> 16;
#endif
	iM	= (tPLLCFG & 0x0FF00) >> 8;
	iP	= (tPLLCFG & 0x0003F) >> 0;

	tPLL= (((IO_CKC_Fxin * iM )/ iP) >> (iS));

	return tPLL;

};

/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
* ***************************************************************************************/
void tca_ckc_peri(void *upAddr, int ishift, int iSrc, int iDiv)
{
	
#if defined(_TCC79x_) 
	if( upAddr ==& HwPCK_DAI){ // For TCC82X DCO mode
	
		*(volatile unsigned long *)upAddr &= HwZERO;
		*(volatile unsigned long *)upAddr |=(iSrc);

		if(iDiv==65535 || iDiv==0){
			BITCSET(*(volatile unsigned long *)upAddr, 0x1f000FFF,HwPCK_SEL_XTIN);
		}else{
		
			iDiv = 0x10000LL-iDiv;
			
			BITCSET(*(volatile unsigned long *)upAddr, 0x0000FFFF,iDiv);
		}
			
		*(volatile unsigned long *)upAddr |=PERICLOCKENABLE;
	}
	else
	{		
		// if divide value is zero, then clock disable
		if(iDiv == 0)
        {   
            BITCSET(*(volatile unsigned long*)upAddr, 0x1F000FFF,(iSrc)|(1));
			BITCLR(*(volatile unsigned long *)upAddr, PERICLOCKENABLE); // Peri controller disable
        }
		else{
			BITCSET(*(volatile unsigned long *)upAddr, 0x1F000FFF, (iSrc)|(iDiv-1) );
			BITSET(*(volatile unsigned long *)upAddr, PERICLOCKENABLE);
		}
	}
#elif defined(_TCC83x_)
	if( upAddr == &HwPCLKCFG7){ // For TCC82X DCO mode
	
		*(volatile unsigned long *)upAddr &= HwZERO;//HwPCLKCFG7_DCLK1_PLL0;
		
		*(volatile unsigned long *)upAddr |=(Hw21 |(iSrc<<5)); //HwPCLKCFG7_DSEL_ON
		*(volatile unsigned long *)upAddr &= ~(Hw16|Hw20);//(HwPCLKCFG7_DCLK1_DCOM|HwPCLKCFG7_DCLK1_EN_OFF);

		if(iDiv==65535 || iDiv==0){
			BITCSET(*(volatile unsigned long *)upAddr, 0x000E0000,(Hw19|Hw18));
		}else{
			BITCSET(*(volatile unsigned long *)upAddr, 0x0000FFFF,iDiv);
		}
			
		*(volatile unsigned long *)upAddr |=Hw20;
	}
	else
	{
		if(iDiv == 0)
			BITCLR(*(volatile unsigned long *)upAddr, PERICLOCKENABLE<<ishift ); // Peri controller disable
		else{
			BITCSET(*(volatile unsigned long *)upAddr, 0xFFFF << ishift, ((iSrc)|(iDiv-1)) <<ishift );
		BITSET(*(volatile unsigned long *)upAddr, PERICLOCKENABLE<<ishift );
		}
	}
#endif


};

int tca_ckc_setperi(unsigned int pName, unsigned int  nUnit,unsigned int pFreq , unsigned int pSource )
{
	unsigned div;
	int iShift;
	int iSrc;
	unsigned  uPll;
	void *pPLLCFG;
	unsigned iSourceClk = 0;
	unsigned int rb_min;

#if defined(_TCC79x_) 
	pPLLCFG = (void*)((&HwPCK_USB11H)+pName);
	rb_min = PERI_USB11H;
#elif defined(_TCC83x_)
	unsigned int iPeriCnt[] = {0,1,2,2,3,3,4,5,5,6,6,7,8,8,9,9,10,10,11,12,13 };
	pPLLCFG = (void *)((&HwPCLKCFG0)+iPeriCnt[pName]);
	rb_min = PERI_RFREQ;
#endif

	if(pName <  rb_min || pName >=  PERI_ALLPERIPERALS)
	{
		return -1;
	}

	switch(pSource)
	{
		case DIRECTPLL0 :
			iSourceClk =  tca_ckc_currentpll(0);
			iSrc = HwPCK_SEL_PLL0;
		break;
		case DIRECTPLL1 :
			iSourceClk =  tca_ckc_currentpll(1);
			iSrc = HwPCK_SEL_PLL1;
		break;
		case DIVIDEPLL0 :
			iSourceClk =  tca_ckc_currentpll(0);
			iSrc = HwPCK_SEL_PLL0DIV;
		break;
		case DIVIDEPLL1 :
			iSourceClk =  tca_ckc_currentpll(1);
			iSrc = HwPCK_SEL_PLL1DIV;
		break;
		case DIRECTXIN :
			iSourceClk = IO_CKC_Fxin;
			iSrc = HwPCK_SEL_XIN;
		break;
		case DIVIDEXIN :
			iSourceClk = IO_CKC_Fxin;
			iSrc = HwPCK_SEL_XINDIV;//HwPCLKCFG_SEL_DIVXIN;
		break;
		/*
		case DIRECTXTIN :
			iSourceClk = uIO_CKC_Fpll[0];
			iSrc = HwPCK_SEL_XIN;
		break;
		case DIVIDEXTIN :
			iSourceClk = uIO_CKC_Fpll[1];
			iSrc = HwPCK_SEL_XINDIV;
		break;
		*/
		default : 
			iSourceClk =  tca_ckc_currentpll(0);
			iSrc = HwPCK_SEL_PLL0;
		break;
	}
	
#if defined(_TCC79x_)
		iShift = 0; // Not Used parameter
#elif defined(_TCC83x_)
	if( PeriClockDefine[pName][1] )
		iShift = HwPCLKCFG_H;
	else
		iShift = HwPCLKCFG_L;
#endif

	if(nUnit)	//Mhz
		pFreq *= 10000; 
	else 	//Khz
		pFreq *= 10; 

	if(pName == PERI_DAI)
	{
		div	= (pFreq *32768);
		uPll = iSourceClk;
		div = div/uPll;
		div <<= 1;				
	}
#ifdef _TCC79x_
  else if(pName == PERI_USB11H)
  {
  		div=1;
  		iSrc = HwPCK_SEL_U48MHZ;
  }
#endif
	else
	{
		if (pFreq != 0)
			div	= (iSourceClk + (pFreq>>1)) / pFreq ;	// should be even number of division factor
		else
			div	= 0;
	}

#ifdef _TCC83x_
	if(pName == PERI_USB11H)
	{
		HwUSBH_48M_SEL |= UH48M_SEL;
	}
	else
#endif
	tca_ckc_peri(pPLLCFG, iShift, iSrc, div);

	return 0;
};

/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :  Retun Unit is 100Hz
*
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_getperi(unsigned int pName)
{
#if defined(_TCC79x_)
	unsigned int Data; 
	unsigned int Div;
	unsigned int ClkSrc;
	unsigned int Temp;
	
	Data = *(volatile unsigned int*)((&HwPCK_USB11H)+pName);
	ClkSrc = (Data&0xF000000)>>24;
	Div = Data & 0xFFF;
	
	if(ClkSrc == DIRECTXIN)
		return 120000/(Div+1);
	else if(ClkSrc == DIRECTPLL0){
		Temp = tca_ckc_currentpll(0);
		return Temp/(Div+1);
	}
	else if(ClkSrc == DIRECTPLL1){
		Temp = tca_ckc_currentpll(1);
		return Temp/(Div+1);
	}
	else if(ClkSrc == CLKFROM48M){
		Temp = 480000;
		return Temp/(Div+1);
	}
	else
		return -1; // Not Support Others
		
#elif defined(_TCC83x_) 
	void	*pPLLCFG;	
	unsigned pRegVal=0;
	unsigned gSel=0;
	unsigned gDiv=0;
	unsigned gDvm=0;
	unsigned int Temp;
	int iShift;
	
	unsigned int iPeriCnt[] = {0,1,2,2,3,3,4,5,5,6,6,7,8,8,9,9,10,10,11,12,13 };
//Select Peri Register
	pPLLCFG = (void *)((&HwPCLKCFG0)+iPeriCnt[pName]);
	pRegVal = *(volatile unsigned long *)pPLLCFG;

	if(pName <  PERI_RFREQ || pName >=  PERI_ALLPERIPERALS)
	{
		return -1;
	}
	
//Select MSB or LSB
	if(PeriClockDefine[pName][1])
		iShift = HwPCLKCFG_H;
	else
		iShift = HwPCLKCFG_L;

//Check DAI 1
	if(iPeriCnt[pName] == 7) // DCLK1, Dco,Divider Mode 
	{
		gSel = (pRegVal & 0xE0000)>>17;
		gDiv = (pRegVal & 0xFFFF);
		gDvm = (pRegVal & 0x1FFFF)>>16;

		if( !((pRegVal & (0x1FFFFF<<iShift))>>20))
			return 0; // Disable Peri
			
		if(gSel == DIRECTXIN)
		{
			if(gDvm) // Divider Mode
				return ((120000*(gDiv+1))/65536);
			else //DCO Mode
				return (120000/(gDiv+1));
		}
		else if(gSel == DIRECTPLL0){
			Temp = tca_ckc_currentpll(0);
			if(gDvm) // Divider Mode
				return ((Temp*(gDiv+1))/65536);
			else //DCO Mode
				return (Temp/(gDiv+1));
		}
		else if(gSel == DIRECTPLL1){
			Temp = tca_ckc_currentpll(1);
			if(gDvm) // Divider Mode
				return ((Temp*(gDiv+1))/65536);
			else //DCO Mode
				return (Temp/(gDiv+1));
		}
		else
			return -1; // Not Support Others

	}
	else
	{
		if(iShift)
			gSel = (pRegVal & (0x7000<<iShift))>>28;
		else
			gSel = (pRegVal & (0x7000<<iShift))>>12;
			
		gDiv =( pRegVal & (0x07FF<<iShift))>>iShift;

		if( !((pRegVal & (0x8000<<iShift))>>15))
			return 0; // Disable Peri
		
		if(gSel == DIRECTXIN)
			return (120000/(gDiv+1));
		else if(gSel == DIRECTPLL0){
			Temp = tca_ckc_currentpll(0);
			return (Temp/(gDiv+1));
		}
		else if(gSel == DIRECTPLL1){
			Temp = tca_ckc_currentpll(1);
			return (Temp/(gDiv+1));
		}
		else
			return -1; // Not Support Others

	}

#endif		
};

/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_getcurrentsys(void)
{
	unsigned int sysclksrc;
	unsigned int currentclock = -1;
	sysclksrc = tca_ckc_getsystemsource();
	switch(sysclksrc)
	{
	case DIRECTPLL0:
		currentclock = tca_ckc_currentpll(0);
		break;
	case DIRECTPLL1:
		currentclock = tca_ckc_currentpll(1);
		break;
	case DIVIDEPLL0:
	case DIVIDEPLL1:
	case DIRECTXIN:
		currentclock = IO_CKC_Fxin; // Not Support others
		break;
	case DIVIDEXIN:
	case DIRECTXTIN:
	case DIVIDEXTIN:
		currentclock = -1; // Not Support others
		break;
	}
	
	return currentclock;
};

/****************************************************************************************
* FUNCTION :
* INPUT :
* OUTPUT :
*
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_getcurrentbus(void)
{
	unsigned int divideval;
	unsigned int sysclock;
	sysclock = (unsigned int)tca_ckc_getcurrentsys();
	divideval = (HwCLKCTRL &0x1F0)>>4;
	return sysclock / (divideval+1);
};

int tca_ckc_getcurrentcpu(void)
{
	unsigned div=0,Fbus=0,Fsys=0,Fcpu=0;

	Fsys = tca_ckc_getcurrentsys();
	Fbus = tca_ckc_getcurrentbus();
	
	div = (HwCLKCTRL & 0xF0000) >> 16;
	Fcpu = Fbus +(((Fsys -Fbus)*(div+1))/16);

	return Fcpu;
};

int tca_ckc_getcurrentsubcpu(void)
{
	unsigned div=0,Fbus=0,Fsys=0,Fcpu=0;

	Fsys = tca_ckc_getcurrentsys();
	Fbus = tca_ckc_getcurrentbus();
	
	div = (HwCLKCTRL & 0xf000) >> 12;
	Fcpu = Fbus +(((Fsys -Fbus)*(div+1))/16);

	return Fcpu;
};

void tca_ckc_validpll(unsigned int *pFreq)
{
	unsigned int uCnt;
	sfPLL		*pPLL;

	pPLL	= &pIO_CKC_PLL[0];
	for (uCnt = 0; uCnt < NUM_PLL; uCnt ++, pPLL ++)
	{
		*pFreq = pPLL->uFpll ;		
		pFreq++;
	}
};

void tca_ckc_setpll(unsigned char P,unsigned char M,unsigned char S ,unsigned int uCH)
{
    void *pPLLCFG;
	int	oriSysClkSource = 0;
	pPLLCFG =(void *)((&HwPLL0CFG)+uCH);	

//Change bus Clock
	oriSysClkSource = tca_ckc_getsystemsource() ;
	if(oriSysClkSource  == uCH)
	{
		if(uCH == 0)
			tca_ckc_setsystemsource(DIRECTPLL1); 
		else if (uCH == 1)
			tca_ckc_setsystemsource(DIRECTPLL0); 		

#if defined(_TCC79x_)
		BITCSET(HwCLKCTRL,0x0000001F0,Hw5); // Bus Divder is 3
#elif defined(_TCC83x_) 
		BITCSET(HwCLKCTRL,0x000000FF0,Hw5); // Bus Divder is 3
#endif
	}
	
	tca_wait();	

//Change PLL Clock
//Disable PLL 
	BITSET(*(volatile unsigned long *)pPLLCFG, Hw31);//PLL Power Down.    (0=PowerUp, 1=PowerDown) 
	
    //Set P
	BITCSET(*(volatile unsigned long *)pPLLCFG, 0x3f, P);
	//Set M
	BITCSET(*(volatile unsigned long *)pPLLCFG, 0xff00, (M<<8));
	//Set S
#if defined(_TCC79x_)
	BITCSET(*(volatile unsigned long *)pPLLCFG, 0x70000, (S<<16));
#elif defined(_TCC83x_) 
	BITCSET(*(volatile unsigned long *)pPLLCFG, 0x30000, (S<<16));
#endif

    //Enable PLL
	BITCLR(*(volatile unsigned long *)pPLLCFG, Hw31);//PLL Power Down.    (0=PowerUp, 1=PowerDown) 
	
	tca_wait();	
	
	if(tca_ckc_getsystemsource() != oriSysClkSource )
	{
		if(uCH == 0)
			tca_ckc_setsystemsource(DIRECTPLL0); 
		else if(uCH == 1)
			tca_ckc_setsystemsource(DIRECTPLL1); 
	}
	
	tca_wait();	
};

void tca_ckc_setmaincpu(unsigned pllVal ,unsigned cpuVal ,unsigned busVal )
{
	unsigned	FcpuDiv = 0;
	unsigned 	sysVal	= 0;
	if(pllVal  < (unsigned int)tca_ckc_getcurrentsys())
		sysVal = (unsigned int)tca_ckc_getcurrentsys();
	else
		sysVal = pllVal;
		
	FcpuDiv	= (((cpuVal- busVal)*16)+((sysVal - busVal)>>1)) / (sysVal - busVal);

	if (FcpuDiv > 16)
		FcpuDiv	= 16;

	if(FcpuDiv == 0)
		FcpuDiv = 1;
		
	BITCSET(HwCLKCTRL, 0xf0000, (FcpuDiv-1)<<16);

};

void tca_ckc_setsubcpu(unsigned pllVal ,unsigned cpuVal ,unsigned busVal )
{
	unsigned	FcpuDiv = 0;
	
	if(pllVal  < (unsigned int)tca_ckc_getcurrentsys())
		pllVal = (unsigned int)tca_ckc_getcurrentsys();
		
	FcpuDiv	= (((cpuVal- busVal)*16)+((pllVal - busVal)>>1)) / (pllVal - busVal);
	
	if (FcpuDiv > 16)
		FcpuDiv	= 16;

	if(FcpuDiv == 0)
		FcpuDiv = 1;
		
	BITCSET(HwCLKCTRL, 0xf000, (FcpuDiv-1)<<12);

};

void tca_ckc_setcpu(unsigned pllVal ,unsigned cpuVal ,unsigned busVal )
{
	tca_ckc_setmaincpu( pllVal , cpuVal , busVal);
#ifdef _TCC79x_
	tca_ckc_setsubcpu( pllVal , cpuVal , busVal);
#endif
};

void tca_ckc_setbus(unsigned busVal )
{
	unsigned	FbusDiv = 0;
 unsigned 	sysVal	= 0; //SYS Clock is divided before fed to CPU & Bus clock generation logic
	
//	FbusDiv = (tca_ckc_currentpll(tca_ckc_getsystemsource())+(busVal>>1)) / busVal ;
	sysVal = (unsigned int)tca_ckc_getcurrentsys();

	FbusDiv = (unsigned int)(sysVal+(busVal>>1)) / busVal ;

	if (FbusDiv > 32)
		FbusDiv	= 32;
	else if (FbusDiv <= 1)
		FbusDiv	= 2;
#if defined(_TCC79x_)
	BITCSET(HwCLKCTRL, 0x1f0, ((FbusDiv-1)<<4));
#elif  defined(_TCC83x_) 
	BITCSET(HwCLKCTRL, 0xff0, ((FbusDiv-1)<<4));
#endif

};

#ifdef _TCC79x_
void tca_ckc_minsubcpu(void )
{
	BITCSET(HwCLKCTRL, 0xf000, (1)<<12);
};

void tca_ckc_closesubcpu(void )
{
//	BITSET(HwSWRESET, HwSWRESET_VCH_ON|HwSWRESET_SUBPRO|HwSWRESET_SUBPROPERI);
//	BITSET(HwSWRESET1,Hw5);	
	BITCLR(HwBCLKCTR,(IO_CKC_BUS_VideoC|IO_CKC_BUS_VideoH|IO_CKC_BUS_VideoP));	
	BITCLR(HwBCLKCTR1,Hw5);	 // Video Encoder
};

void tca_ckc_opensubcpu(void )
{
	BITSET(HwBCLKCTR,(Hw19|Hw23|Hw25));
	BITSET(HwBCLKCTR1,Hw5);	

	BITSET(HwSWRESET, Hw19|Hw23|Hw25);
	BITSET(HwSWRESET1,Hw5);	
	
	BITCLR(HwSWRESET, Hw19|Hw23|Hw25);	
	BITCLR(HwSWRESET1,Hw5);		

};
#endif

int tca_ckc_setclock(unsigned int pLL,unsigned  int cPU,unsigned  int bUS,unsigned int uCH)
{
	unsigned	uCnt;
	int retval = 0;
	unsigned int currentsys = 0;
	unsigned int sysSource = 0;
	sfPLL		*pPLL;
	
	currentsys = tca_ckc_getcurrentsys();
	sysSource = tca_ckc_getsystemsource();
	
	pLL = pLL*10000;
	cPU = cPU*10000;
	bUS = bUS*10000;

	
	//Check P,M,S
	pPLL	= &pIO_CKC_PLL[0];
	for (uCnt = 0; uCnt < NUM_PLL; uCnt ++, pPLL ++)
		if (pPLL->uFpll == pLL)
			break;


	if(pLL && !cPU && !bUS) // Only Change PLL Clock - Not Change CPU/BUS
	{
		if (uCnt < NUM_PLL)
		{
			if((uCH == sysSource && pLL >= currentsys && tca_ckc_checksdram()) || (uCH != sysSource && pLL != 0))
			{
			tca_ckc_setpll(pPLL->P,pPLL->M ,pPLL->S,uCH);
			}
			retval = 1;
		}

	}
	else if(!pLL && cPU && bUS) // Only Change CPU/BUS Clock - Not Change PLL Clock
	{
		if(cPU <= currentsys && tca_ckc_checksdram())
		{
		//Set Bus
		tca_ckc_setbus(bUS);

		tca_wait();	

		//Set Cpu
		tca_ckc_setcpu(pLL,cPU,bUS);
		retval = 1;
		}
	}
	else if(pLL && cPU && bUS) // Change PLL ,CPU and BUS Clock
	{

		if (uCnt < NUM_PLL)
		{
//			if(uCH != tca_ckc_getsystemsource() && tca_ckc_checksdram())
			if(tca_ckc_checksdram()) // DDR do not change system clock
			{
				if((uCH == sysSource && pLL >= currentsys) || (uCH != sysSource && pLL != 0))
				{
					tca_ckc_setpll(pPLL->P,pPLL->M ,pPLL->S,uCH);
				}

				if(cPU <= currentsys)
				{
			//Set Bus
			tca_ckc_setbus(bUS);

			tca_wait();	

			//Set Cpu
			tca_ckc_setcpu(pLL,cPU,bUS);
			retval = 1;
		}
			}
		}
	}

	return retval;
};
	
void tca_ckc_initclock(void)
{
	_ckc_max_cpu = tca_ckc_getcurrentcpu();
	_ckc_max_bus = tca_ckc_getcurrentbus();
};

void tca_ckc_getmaxclock(unsigned int *mFCpu, unsigned int *mFBus)
{

	*mFCpu = _ckc_max_cpu;
	*mFBus = _ckc_max_bus;
};

void tca_ckc_setmaxclock(void)
{
/*
	tca_ckc_setbus(_ckc_max_bus);

	tca_wait();	
	//Set Cpu
	tca_ckc_setcpu(0,_ckc_max_cpu,_ckc_max_bus);
	*/
	tca_ckc_setclock(_ckc_max_cpu,_ckc_max_cpu,_ckc_max_bus,tca_ckc_getsystemsource());
}
/****************************************************************************************
* DESCRIPTION : Start Clock 
* ***************************************************************************************/
#define CKC_IDLE_FPLL	266;
#define CKC_IDLE_FCPU	266;
#define CKC_IDLE_FBUS	133;

stSYSTEM_CKC _gSystemClock[CLOCK_PRIORITY_NUM];
unsigned int _gCurrentPriorityClock = 0;

unsigned int tca_ckc_getcurrentpriority(void)
{
	return	_gCurrentPriorityClock;
}

void tca_setinitpriorityclock(unsigned int mFCpu, unsigned int mFBus)
{
	int i;

	for(i = 0; i < (CLOCK_PRIORITY_NUM-1); i++)
	{
		_gSystemClock[i].mFPll		= 0;
		_gSystemClock[i].mFCpu		= 0;
		_gSystemClock[i].mFBus		= 0;
		_gSystemClock[i].mFUsed 	= 0;
	}

		_gSystemClock[0].mFPll		= 0;//not change PLL
		#if 0
		_gSystemClock[0].mFCpu		= CKC_IDLE_FCPU;
		_gSystemClock[0].mFBus		= CKC_IDLE_FBUS;
		#else
		_gSystemClock[0].mFCpu		= mFCpu;
		_gSystemClock[0].mFBus		= mFBus;
		#endif
		_gSystemClock[0].mFUsed 	= 1;
		_gCurrentPriorityClock 	= IDLE_PRIORITY; //IDLE_PRIORITY
		
}

void tca_setpriorityclock(unsigned int mFPll, unsigned int mFCpu, unsigned int mFBus, unsigned int icPrio)
{
	int	state;

	mFPll = 0; //not change PLL
	
	if (_gCurrentPriorityClock <= icPrio)				// Check ,icPrio is over than Current Priority  
	{
		state = tca_ckc_setclock(mFPll,mFCpu,mFBus,1);
		_gCurrentPriorityClock = icPrio;
	}

	_gSystemClock[icPrio].mFPll	= mFPll;
	_gSystemClock[icPrio].mFCpu	= mFCpu;
	_gSystemClock[icPrio].mFBus	= mFBus;
	_gSystemClock[icPrio].mFUsed 	= 1;

}

void tca_checksetlowerpriority(unsigned int icPrio)
{
	int	i = 0;
	int	state;
	if(_gSystemClock[icPrio].mFUsed)
		_gSystemClock[icPrio].mFUsed = 0;

	for(i = icPrio -1; i >= IDLE_PRIORITY; i--) 
	{
		if(_gSystemClock[i].mFUsed)
		{
			state = tca_ckc_setclock(_gSystemClock[i].mFPll,_gSystemClock[i].mFCpu,_gSystemClock[i].mFBus,1);			
			_gCurrentPriorityClock = i;
			break;
		} //if(_gSystemClock[i].mFUsed)
	}//for(i = icPrio -1; i < 0)

}

int tca_checksetupperpriority(unsigned int icPrio)
{
	//int	i = 0;
	int retval = 0;
	//for(i = icPrio+1; i < (CLOCK_PRIORITY_NUM-1); i++) 
	if(_gCurrentPriorityClock > icPrio)
	{
		if(_gSystemClock[_gCurrentPriorityClock].mFUsed)
		{
			if(_gSystemClock[icPrio].mFUsed)
				_gSystemClock[icPrio].mFUsed = 0;

			retval = 1;
		}
	}

	return retval;
}

/*
*	icPrio : Return Priority
*	Don't Return IDLE_PRIORITY
*/
void tca_returnpriorityclock(unsigned int icPrio)
{	
	//int	state;
	unsigned int check = 0;

	if(icPrio == IDLE_PRIORITY)
		return;
		
	if(!(_gSystemClock[icPrio].mFUsed))
		return;

	check = tca_checksetupperpriority(icPrio);	// icPrio와 같거나 보다 상위 Priority Clock이 있는지 확인 
	
	if(check == 0) 									//상위 Priority Clock이 없다면 
		tca_checksetlowerpriority(icPrio);
	
}

/****************************************************************************************
* DESCRIPTION : End Clock 
* ***************************************************************************************/
	
#endif  /* __TCA_CKC_H__ */

