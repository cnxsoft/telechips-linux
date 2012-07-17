
/****************************************************************************
 *   FileName    : tca_alarm.C
 *   Description :
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

/*****************************************************************************
*
* Header Files Include
*
******************************************************************************/
#include <linux/kernel.h>
#include <asm/io.h>
#include "tca_alarm.h"
#include <asm/mach-types.h>
#ifdef CONFIG_ARCH_TCC88XX
#include <mach/pm.h>
#endif
/*****************************************************************************
*
* Defines
*
******************************************************************************/

/*****************************************************************************
*
* structures
*
******************************************************************************/


/*****************************************************************************
*
* Variables
*
******************************************************************************/



/*****************************************************************************
*
* Functions
*
******************************************************************************/

/*****************************************************************************
* Function Name : tca_rtcgettime()
******************************************************************************/
volatile int tca_alarm_gettime(unsigned int rtcbaseaddress, rtctime *pTime)
{
	unsigned uCON;
	PRTC	pRTC = (PRTC)rtcbaseaddress;

#if 0
	BITSET(pRTC->RTCCON, Hw1);	// RTC Register write enabled
	BITSET(pRTC->INTCON, Hw0);	// Interrupt Block Write Enable
#endif

	pTime->wSecond			= pRTC->ALMSEC;
	pTime->wMinute			= pRTC->ALMMIN;
	pTime->wHour			= pRTC->ALMHOUR;
	pTime->wDayOfWeek 		= pRTC->ALMDAY;
	pTime->wDay				= pRTC->ALMDATE;
	pTime->wMonth			= pRTC->ALMMON;
	pTime->wYear			= pRTC->ALMYEAR;
	uCON = pRTC->RTCALM;

	BITCLR(pRTC->INTCON, Hw0);	// Interrupt Block Write Disable
	BITCLR(pRTC->RTCCON, Hw1);	// RTC Register write Disable

	/* Second */
	if (ISZERO(uCON, Hw0))
		pTime->wSecond	= (unsigned char) 0;
	else
		pTime->wSecond	= FROM_BCD( pTime->wSecond );

	/* Minute */
	if (ISZERO(uCON, Hw1))
		pTime->wMinute	= (unsigned char) 0;
	else
		pTime->wMinute	= FROM_BCD( pTime->wMinute );

	/* Hour */
	if (ISZERO(uCON, Hw2))
		pTime->wHour	= (unsigned char) 0;
	else
		pTime->wHour	= FROM_BCD( pTime->wHour );

	/* date */
	if (ISZERO(uCON, Hw3))
		pTime->wDay	= (unsigned char) 0;
	else
		pTime->wDay	= FROM_BCD( pTime->wDay );

	/* month */
	if (ISZERO(uCON, Hw5))
		pTime->wMonth	= (unsigned char) 0;
	else
		pTime->wMonth	= FROM_BCD( pTime->wMonth );

	/* year */
	if (ISZERO(uCON, Hw6))
		pTime->wYear	= (unsigned short) 0;
	else
//		pTime->wYear	= FROM_BCD( pTime->wYear );
		pTime->wYear	= (FROM_BCD( pTime->wYear>>8 )*100) + FROM_BCD( pTime->wYear&0x00FF );	// YearFix : hjbae

	/* weekdays */
	if (ISZERO(uCON, Hw4))
		pTime->wDayOfWeek		= 1;
	else
		pTime->wDayOfWeek = FROM_BCD( pTime->wDayOfWeek );

	pRTC->RTCALM  = uCON;

	return 0;
}

/*****************************************************************************
* Function Name : tca_rtcsettime()
******************************************************************************/
volatile int tca_alarm_settime(unsigned int rtcbaseaddress, rtctime *pTime)
{
	unsigned	uCON;
	PRTC	pRTC = (PRTC)rtcbaseaddress;
#if defined(CONFIG_ARCH_TCC93XX)||defined(CONFIG_ARCH_TCC892X)
	PPIC	pPIC = (PPIC)tcc_p2v(HwPIC_BASE);
#endif

	BITSET(pRTC->RTCCON, Hw1);	// RTC Register write enabled
	BITSET(pRTC->INTCON, Hw0);	// Interrupt Block Write Enable

	uCON	= 0xEF; // Not wDayOfWeek
	/* Second */
	if ( pTime->wSecond > 59 )
		BITCLR(uCON, Hw0);//HwRTCALM_SECEN_EN
	else
		pRTC->ALMSEC	= TO_BCD( pTime->wSecond );

	/* Minute */
	if ( pTime->wMinute > 59 )
		BITCLR(uCON, Hw1);//HwRTCALM_MINEN_EN
	else
		pRTC->ALMMIN	= TO_BCD( pTime->wMinute );

	/* Hour */
	if ( pTime->wHour > 23 )
		BITCLR(uCON, Hw2);//HwRTCALM_HOUREN_EN
	else
		pRTC->ALMHOUR	= TO_BCD( pTime->wHour );

	/* Date */
	if ( pTime->wDay > 31 || pTime->wDay < 1 )
		BITCLR(uCON, Hw3);//HwRTCALM_DATEEN_EN
	else
		pRTC->ALMDATE	= TO_BCD( pTime->wDay );

	/* month */
	if (pTime->wMonth > 12 || pTime->wMonth < 1)
		BITCLR(uCON, Hw5);//HwRTCALM_MONEN_EN
	else
		pRTC->ALMMON	= TO_BCD( pTime->wMonth );

	/* year */
	if (pTime->wYear > 2099 || pTime->wYear < 1900)
		BITCLR(uCON, Hw6);//HwRTCALM_YEAREN_EN
	else
//		pRTC->ALMYEAR	= TO_BCD( pTime->wYear );
		pRTC->ALMYEAR	= (TO_BCD( pTime->wYear/100 ) << 8) | TO_BCD( pTime->wYear%100 );	// YearFix : hjbae

	/* Day */
	if ( pTime->wDayOfWeek > 6)
		BITCLR(uCON, Hw4);//HwRTCALM_DAYEN_EN
	else
		pRTC->ALMDAY	= pTime->wDayOfWeek+1;


	// Enable ALARM
	pRTC->RTCALM	= uCON;

	#if 1	//for One-Shot write to INTMODE - 110106, hjbae
	if(machine_is_tcc8800() || machine_is_m801_88() || machine_is_m803())
	{
		#ifdef CONFIG_ARCH_TCC88XX
			#if defined(CONFIG_SLEEP_MODE) && defined(TCC_PM_SLEEP_WFI_USED)
			BITSET(pRTC->RTCIM, Hw2|Hw1|Hw0);	//INTMODE : NormalMode / ActiveHigh / LevelTrigger
			#else
			BITSET(pRTC->RTCIM, Hw3|Hw2|Hw1|Hw0);	//INTMODE : PowerDownMode / ActiveHigh / LevelTrigger
			#endif
		#else
			BITSET(pRTC->RTCIM, Hw3|Hw2|Hw1|Hw0);	//INTMODE : PowerDownMode / ActiveHigh / LevelTrigger
		#endif
	}
	else
		BITSET(pRTC->RTCIM, Hw3|Hw2|Hw1|Hw0);	//INTMODE : PowerDownMode / ActiveHigh / LevelTrigger
	#else
	//BITCSET(pRTC->RTCIM, Hw1, Hw0);	// INTMODE : Supports on the edge alarm interrupt.
	BITSET(pRTC->RTCIM, Hw1|Hw0);	//INTMODE : Supports on the level alarm interrupt.
	BITSET(pRTC->RTCIM, Hw3);		// Operation mode - Power Down Mode for PMU RTCWKUP
	//BITCLR(pRTC->RTCIM, Hw3);		// Operation mode - Normal Mode for WFI mode

	BITSET(pRTC->RTCIM, Hw2);		// Wakeup mode selection bit - PMWKUP active high
//	BITCLR(pRTC->RTCIM, Hw2);		// Wakeup mode selection bit - PMWKUP active low
	#endif

	pRTC->RTCPEND	= 0;			// alarm clear
	BITSET(pRTC->RTCSTR, Hw0);		// alarm clear

	BITSET(pRTC->RTCCON, Hw6|Hw7);  // Alarm Interrupt Output Enable (Hw6) Wake-up Output Enable (Hw7)

	if( !(pRTC->INTCON & Hw15) )
	{
		BITSET(pRTC->RTCCON,Hw0);		// RTC Start bit - Halt
		BITSET(pRTC->INTCON,Hw15);		// protect bit - enable
		BITCLR(pRTC->RTCCON,Hw0);		// RTC Start bit - Run
	}

	printk("RTC[%02X] INT[%04X] ALM[%02X] IM[%01X] PEND[%01X] STR[%02X]\n", pRTC->RTCCON, pRTC->INTCON, pRTC->RTCALM, pRTC->RTCIM, pRTC->RTCPEND, pRTC->RTCSTR);
	//printk("\n%04x.%02x.%02x-%02x:%02x:%02x", pRTC->ALMYEAR, pRTC->ALMMON, pRTC->ALMDATE, pRTC->ALMHOUR, pRTC->ALMMIN, pRTC->ALMSEC);

	#if 1	//for RMWKUP -> RTC Alarm Interrupt - 110106, hjbae
	BITCLR(pRTC->INTCON, Hw0);		// Interrupt Block Write Enable bit - disable
	BITCLR(pRTC->RTCCON, Hw1);		// RTC write enable bit - disable
	#endif

	//BITCLR(HwPMU->WKUPEN, HwPMU_WKUP_RTCWKUP);	// RTC WKUP Disable
	//BITCLR(HwPMU->WKUPPOL, HwPMU_WKUP_RTCWKUP);	// ACTIVE HIGH
	#if 1
#if defined(CONFIG_ARCH_TCC93XX)
	pPIC->CLR0 |= HwINT0_RTC;
	pPIC->SEL0	|= HwINT0_RTC;
	pPIC->IEN0 |= HwINT0_RTC;
	//pPIC->MODE0 |= HwINT0_RTC; // Level-triggered
	//pPIC->POL0
	//pPIC->WKEN0 |= HwINT0_RTC;
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC88XX)
	HwPIC->CLR1 |= HwINT1_RTC;
	HwPIC->SEL1	|= HwINT1_RTC;
	HwPIC->IEN1 |= HwINT1_RTC;
	//HwPIC->MODE1 |= HwINT1_RTC; // Level-triggered
	//HwPIC->POL1
	//HwPIC->WKEN1 |= HwINT1_RTC;
#elif defined(CONFIG_ARCH_TCC892X)
	pPIC->CLR1.bREG.RTC		= 1;
	pPIC->SEL1.bREG.RTC		= 1;
	pPIC->IEN1.bREG.RTC		= 1;
#endif
	#endif

	return 0;
}

/************************************************************************************************
* FUNCTION		:  tca_alarm_setint
*
* DESCRIPTION	:
*
************************************************************************************************/
volatile int tca_alarm_setint(unsigned int rtcbaseaddress)
{
	volatile rtctime lpTime;

	//Set Alarm
	tca_rtc_gettime(rtcbaseaddress, (rtctime *)&lpTime);

#if defined(WINCE_ONLY)
	if(lpTime.wSecond < 57){
		lpTime.wSecond += 3;
	}
	else{
		if(lpTime.wMinute < 59){
			lpTime.wMinute += 1;
			lpTime.wSecond = 0;
		}
		else{
			if(lpTime.wHour < 23){
				lpTime.wHour += 1;
				lpTime.wMinute = 0;
				lpTime.wSecond = 0;
			}
			else{
				lpTime.wHour = 0;
				lpTime.wMinute = 0;
				lpTime.wSecond = 0;
			}
		}
	}

#else
	if(lpTime.wSecond < 55)
		lpTime.wSecond += 5;
#endif

	tca_alarm_settime(rtcbaseaddress, (rtctime *)&lpTime);

	return 0;
}
/************************************************************************************************
* FUNCTION		:  tca_alarm_setpmwkup
*
* DESCRIPTION	:
*
************************************************************************************************/
volatile int tca_alarm_setpmwkup(unsigned int rtcbaseaddress,unsigned int vicbaseaddress)
{
	rtctime lpTime;
	PRTC	pRTC = (PRTC)rtcbaseaddress;
	PPIC    pPIC = (PPIC)vicbaseaddress;

	//Set Alarm
	tca_rtc_gettime(rtcbaseaddress, (rtctime *)&lpTime);

#if defined(WINCE_ONLY)

	if(lpTime.wSecond < 57){
		lpTime.wSecond += 3;
	}
	else{
		if(lpTime.wMinute < 59){
			lpTime.wMinute += 1;
			lpTime.wSecond = 0;
		}
		else{
			if(lpTime.wHour < 23){
				lpTime.wHour += 1;
				lpTime.wMinute = 0;
				lpTime.wSecond = 0;
			}
			else{
				lpTime.wHour = 0;
				lpTime.wMinute = 0;
				lpTime.wSecond = 0;
			}
		}
	}

#else
	if(lpTime.wSecond < 55)
		lpTime.wSecond += 5;
#endif

	tca_alarm_settime(rtcbaseaddress, (rtctime *)&lpTime);

#if defined(CONFIG_ARCH_TCC93XX)
	pPIC->CLR0		= HwINT0_RTC;
	pPIC->MSTS0		|= HwINT0_RTC;
	pPIC->SEL0		|= HwINT0_RTC;
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC88XX)
	pPIC->CLR1 		= HwINT1_RTC;
	pPIC->MSTS1 		|= HwINT1_RTC;
	pPIC->SEL1		|= HwINT1_RTC;
#elif defined(CONFIG_ARCH_TCC892X)
	pPIC->CLR1.bREG.RTC		= 1;
	pPIC->MSTS1.bREG.RTC		= 1;
	pPIC->SEL1.bREG.RTC		= 1;
#endif

	BITSET(pRTC->RTCCON, Hw1);// Enable RTCEN
	BITSET(pRTC->INTCON, Hw0);// Enable INTWREN
	BITCLR(pRTC->RTCIM, 0xf);
	BITSET(pRTC->RTCIM, Hw0|Hw2|Hw3);	 // Enable ALMINT_EDGE_EN, Enable PMWKUP_ACTIVE_HIGH, Enable PWDN_POWERDOWN_MODE
	BITSET(pRTC->RTCCON, Hw1 | Hw7);

	//PMWKUP Disable Start
		//pRTC->RTCCON = 0;
		//BITSET(pRTC->RTCCON, Hw1);
		//BITSET(pRTC->INTCON, Hw0);
		//pRTC->RTCIM = 0;
		//BITSET(pRTC->RTCIM, Hw0);
	//PMWKUP Disable End

	return 0;
}

/************************************************************************************************
* FUNCTION		:  tca_alarm_disable
*
* DESCRIPTION	:  Disable the RTC Alarm during the power off state
*
************************************************************************************************/
volatile int tca_alarm_disable(unsigned int rtcbaseaddress)
{
	PRTC pRTC = (PRTC)rtcbaseaddress;

	if (pRTC == NULL) {
		printk("failed RTC ioremap()\n");
	}
	else {
		BITCLR(pRTC->RTCCON, Hw7|Hw6);	// Disable - Wake Up Interrupt Output(Hw7), Alarm Interrupt Output(Hw6)

		BITSET(pRTC->RTCCON, Hw1);	// Enable - RTC Write
		BITSET(pRTC->INTCON, Hw0);	// Enable - Interrupt Block Write

		BITCLR(pRTC->RTCALM, Hw7|Hw6|Hw5|Hw4|Hw3|Hw2|Hw1|Hw0);	// Disable - Alarm Control

		BITCSET(pRTC->RTCIM, Hw3|Hw2|Hw1|Hw0, Hw3|Hw2);	// Power down mode, Active HIGH, Disable alarm interrupt

		BITCLR(pRTC->INTCON, Hw0);	// Disable - Interrupt Block Write
		BITCLR(pRTC->RTCCON, Hw1);	// Disable - RTC Write
	}

	return 0;
}
