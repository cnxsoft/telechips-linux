/****************************************************************************
 *   FileName    : IO_GPSB.c
 *   Description : TCC7xx GPSB Management Functions
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#include <linux/delay.h>
#include <linux/types.h>
#include <asm/io.h>
	 

#include <mach/dev_gpsb.h>
#include <mach/bsp.h>
#ifndef NULL
#define NULL	0
#endif


/**************************************************************************
*  FUNCTION NAME : 
*      GPSB *IO_GPSB_GetBaseAddr(unsigned uCH);
*  
*  DESCRIPTION : 
*  INPUT:
*			uCH	= Channel Number of GPSB Driver
*  
*  OUTPUT:	GPSB - Return Type
*  			= 
*  REMARK  :	
**************************************************************************/
GPSB *IO_GPSB_GetBaseAddr(unsigned uCH)
{
	return (PGPSB )tcc_p2v(HwGPSB_BASE(uCH));;
}

/**************************************************************************
*  FUNCTION NAME : 
*      void IO_GPSB_InitSWReset(unsigned uCH);
*  
*  DESCRIPTION : 
*  INPUT:
*			uCH		= Channel Number of GPSB Driver
*  
*  OUTPUT:	void - Return Type
*  REMARK  :	
**************************************************************************/
void IO_GPSB_InitSWReset(unsigned uCH)
{
	PIOBUSCFG	pIOBUSCFG = (PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE);
#if defined(CONFIG_ARCH_TCC92XX)	
	BITSET(pIOBUSCFG->HRSTEN0, (HwIOBUSCFG_GPSB0 << uCH));
#elif defined(CONFIG_ARCH_TCC93XX)
	//..
#else // ..

#endif
}


/**************************************************************************
*  FUNCTION NAME : 
*      unsigned IO_GPSB_GetInterruptEnabledChannels(void);
*  
*  DESCRIPTION : 
*  INPUT:
*  OUTPUT:	unsigned - each channels mapped to bit number
*  REMARK  :	
**************************************************************************/
unsigned IO_GPSB_GetInterruptEnabledChannels(void)
{
	int iCH;
	unsigned  uEnabledChFlag = 0;
	sHwGPSB *pHwGPSB;

	for(iCH=0; iCH<2; iCH++)
	{
		pHwGPSB = IO_GPSB_GetBaseAddr(iCH);
		if ( ISSET(pHwGPSB->DMAICR, HwGPSB_DMAICR_IEP_EN|HwGPSB_DMAICR_IED_EN) )
		{
			uEnabledChFlag |= 1<<iCH;
		}
	}

	for(; iCH<DRV_GPSB_CHMAX; iCH++)
	{
		pHwGPSB = IO_GPSB_GetBaseAddr(iCH);
		if ( ISSET(pHwGPSB->INTEN, 0x1FF) )
		{
			uEnabledChFlag |= 1<<iCH;
		}
	}

	return uEnabledChFlag;
}

/**************************************************************************
*  FUNCTION NAME : 
*      void IO_GPSB_InitPORT(
*      	unsigned uCH,
*      	unsigned uPort,
*      	unsigned uSDOSDI,
*      	unsigned uSwCtrl);
*      	
*  DESCRIPTION : 
*  INPUT:
*			uCH		= Channel Number of GPSB Driver
*			uPort	= Port Number of GPSB Driver
*			uSDOSDI	= Port Setting of GPSB Driver
*			uSwCtrl	= Control Setting of GPSB Driver
*  
*  OUTPUT:	
*  			None 
*  REMARK  :	
**************************************************************************/
void IO_GPSB_InitPORT(unsigned uCH, unsigned uPort, unsigned uSDOSDI, unsigned uSwCtrl)
{

	//-----------------------------
	//
	//        TCC92XX GPSB
	//
	//  The Number of Channel: 6
	//  The Number of Port   : 13
	//
	//-----------------------------
 	#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)
	PGPSBPORTCFG pGPSBPortCfg = (PGPSBPORTCFG)tcc_p2v(HwGPSBPORTCFG_BASE);
	#else
	PGPSBPORTCFG pGPSBPortCfg = (PGPSBPORTCFG)tcc_p2v(HwGPSB_PORTCFG_BASE);
	#endif
	PGPION pGPIO;
	
	if(uCH < 4)
		BITCSET(pGPSBPortCfg->PCFG0, (0xFF<<(8*uCH)), (uPort<<(8*uCH)));
	else
	{
		BITCSET(pGPSBPortCfg->PCFG1, (0xFF<<(8*(uCH-4))), (uPort<<(8*(uCH-4))));
	}
	
	switch (uPort)
	{
		case DRV_GPSB_PORT0:	// Connect with Port 0
			pGPIO = (PGPION)tcc_p2v(HwGPIOB_BASE);
			BITCSET(pGPIO->GPFN2, HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN1(4) | HwPORTCFG_GPFN3(4));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN6(4));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(4));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN6(4) | HwPORTCFG_GPFN7(4));
					break;
			}
			
			break;

		case DRV_GPSB_PORT1:	// Connect with Port 1
			pGPIO = (PGPION)tcc_p2v(HwGPIOB_BASE);
			BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK, HwPORTCFG_GPFN0(4) | HwPORTCFG_GPFN1(4));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN2_MASK, HwPORTCFG_GPFN2(4));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN3(4));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN2(4) | HwPORTCFG_GPFN3(4));
					break;
			}
			
			break;

		case DRV_GPSB_PORT2:	// Connect with Port 2
			pGPIO = (PGPION)tcc_p2v(HwGPIOC_BASE);
			BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK, HwPORTCFG_GPFN4(3) | HwPORTCFG_GPFN5(3));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN3(3));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN2_MASK, HwPORTCFG_GPFN2(3));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN2(3) | HwPORTCFG_GPFN3(3));
					break;
			}
			
			break;

		case DRV_GPSB_PORT3:	// Connect with Port 3
			pGPIO = (PGPION)tcc_p2v(HwGPIOC_BASE);
			BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK, HwPORTCFG_GPFN0(3) | HwPORTCFG_GPFN1(3));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(3));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN6(3));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN7_MASK | HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN7(3) | HwPORTCFG_GPFN6(3));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT4:	// Connect with Port 4
			pGPIO = (PGPION)tcc_p2v(HwGPIOE_BASE);
			BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK, HwPORTCFG_GPFN0(2) | HwPORTCFG_GPFN1(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN2_MASK, HwPORTCFG_GPFN2(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN3(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN2(2) | HwPORTCFG_GPFN3(2));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT5:	// Connect with Port 5
			pGPIO = (PGPION)tcc_p2v(HwGPIOE_BASE);
			BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN2(2) | HwPORTCFG_GPFN3(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN6(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN6(2) | HwPORTCFG_GPFN7(2));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT6:	// Connect with Port 6
			pGPIO = (PGPION)tcc_p2v(HwGPIOD_BASE);
			BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK, HwPORTCFG_GPFN1(2) | HwPORTCFG_GPFN2(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN3(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN4_MASK, HwPORTCFG_GPFN4(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN4_MASK, HwPORTCFG_GPFN3(2) | HwPORTCFG_GPFN4(2));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT7:	// Connect with Port 7
			pGPIO = (PGPION)tcc_p2v(HwGPIOF_BASE);
			BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK, HwPORTCFG_GPFN4(2) | HwPORTCFG_GPFN5(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN3(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN2_MASK, HwPORTCFG_GPFN2(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN2_MASK, HwPORTCFG_GPFN3(2) | HwPORTCFG_GPFN2(2));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT8:	// Connect with Port 8
			pGPIO = (PGPION)tcc_p2v(HwGPIOF_BASE);
			BITCSET(pGPIO->GPFN2, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK, HwPORTCFG_GPFN0(2) | HwPORTCFG_GPFN1(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN6(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN7_MASK | HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN7(2) | HwPORTCFG_GPFN6(2));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT9:	// Connect with Port 9
			pGPIO = (PGPION)tcc_p2v(HwGPIOF_BASE);
			BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN2(2) | HwPORTCFG_GPFN3(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN1_MASK, HwPORTCFG_GPFN1(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN0(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN1(2) | HwPORTCFG_GPFN0(2));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT10:	// Connect with Port 10
			pGPIO = (PGPION)tcc_p2v(HwGPIOC_BASE);
			BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN7_MASK | HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN7(2) | HwPORTCFG_GPFN6(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN5_MASK, HwPORTCFG_GPFN5(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN4_MASK, HwPORTCFG_GPFN4(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN3, HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN4_MASK, HwPORTCFG_GPFN5(2) | HwPORTCFG_GPFN4(2));
					break;
			}
			
			break;	

		case DRV_GPSB_PORT11:	// Connect with Port 11
			pGPIO = (PGPION)tcc_p2v(HwGPIOD_BASE);
			BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK, HwPORTCFG_GPFN5(2) | HwPORTCFG_GPFN6(2));
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN0(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN0, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(2));	
					BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN0(2));	
					break;
			}
			
			break;	

		case DRV_GPSB_PORT12:	// Connect with Port 12
			pGPIO = (PGPION)tcc_p2v(HwGPIOD_BASE);
			BITCSET(pGPIO->GPFN1, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(2));	
			BITCSET(pGPIO->GPFN2, HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN0(2));	
			switch (uSDOSDI)
			{
				case DRV_GPSB_SDOSDI_NOUSE:		
					break;
				case DRV_GPSB_SDOSDI_SDI:		
					BITCSET(pGPIO->GPFN2, HwPORTCFG_GPFN3_MASK, HwPORTCFG_GPFN3(2));	
					break;
				case DRV_GPSB_SDOSDI_SDO:		
					BITCSET(pGPIO->GPFN2, HwPORTCFG_GPFN4_MASK, HwPORTCFG_GPFN4(2));	
					break;
				case DRV_GPSB_SDOSDI_ALLUSE:	
					BITCSET(pGPIO->GPFN2, HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN4_MASK, HwPORTCFG_GPFN3(2) | HwPORTCFG_GPFN4(2));
					break;
			}
			
			break;	
	}
}

/**************************************************************************
*  FUNCTION NAME : 
*      void IO_GPSB_InitChannel(
*      	unsigned uCH,
*      	unsigned uMode,
*      	unsigned uSCK,
*      	unsigned uSwCtrl,
*      	unsigned uSlave,
*      	unsigned uSDOSDI);
*      	
*  DESCRIPTION : 
*  INPUT:
*			uCH		= Channel Number of GPSB Driver
*			uPort	= Port Number of GPSB Driver
*			uMode	= MODE Register of GPSB Driver
*			uSwCtrl	= Control Setting of GPSB Driver
*			uSDOSDI	= Port Setting of GPSB Driver
*  
*  OUTPUT:	
*  			None 
*  REMARK  :	
**************************************************************************/

void IO_CKC_EnableBUS(unsigned int X)
{
	PIOBUSCFG pHwIOBUSCFG = (PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE);
	
#if defined(CONFIG_ARCH_TCC92XX)        
	BITSET(pHwIOBUSCFG->HCLKEN0, (X));
#elif defined(CONFIG_ARCH_TCC93XX)
	//..
#else // ..

#endif
}

void IO_CKC_DisableBUS(unsigned int X)
{
	PIOBUSCFG pHwIOBUSCFG = (PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE);

#if defined(CONFIG_ARCH_TCC92XX)        
	BITCLR(pHwIOBUSCFG->HCLKEN0, (X));
	BITCLR(pHwIOBUSCFG->HCLKEN0, (X));
#elif defined(CONFIG_ARCH_TCC93XX)
        //..
#else // ..

#endif

}


void IO_GPSB_InitChannel(unsigned uCH, unsigned uPort, unsigned uMode, unsigned uSwCtrl, unsigned uSDOSDI)
{
//	sGPSBPORT	*pPORT;
 	sHwGPSB 	*pHwGPSB;
	PIOBUSCFG	pIOBUSCFG = (PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE);
		
	//Set the Port Information
	IO_GPSB_InitPORT(uCH, uPort, uSDOSDI, uSwCtrl);
		
	if ( !(IO_GPSB_GetInterruptEnabledChannels()&(~(1<<uCH))) )
	{
		PPIC pPIC_Reg = (PPIC)tcc_p2v(HwPIC_BASE);
		
		BITCLR(pPIC_Reg->IEN1, HwINT1_GPSB);	/* Disable GPSB Interrupt */
		pPIC_Reg->CLR1       = HwINT1_GPSB;	/* Clear GPSB interrupt */
	}

	#ifdef CONFIG_ARCH_TCC92XX // to do
	tca_ckc_setperi(PERI_GPSB0 + uCH ,ENABLE,  90000, PCDIRECTPLL2);
	#endif//
		
	pHwGPSB = IO_GPSB_GetBaseAddr(uCH);

	BITSET(pHwGPSB->MODE, HwGPSB_MODE_EN);
	#ifdef CONFIG_ARCH_TCC92XX
	BITCLR(pIOBUSCFG->HRSTEN0, (HwIOBUSCFG_GPSB0 << uCH));
	IO_CKC_EnableBUS((HwIOBUSCFG_GPSB0 << uCH));
	BITSET(pIOBUSCFG->HRSTEN0, (HwIOBUSCFG_GPSB0 << uCH));
	#else
	// 
	#endif
}

/**************************************************************************
*  FUNCTION NAME : 
*      void IO_GPSB_DisableChannel(unsigned uCH);
*  
*  DESCRIPTION : 
*  INPUT:
*			uCH		= Channel Number of GPSB Driver
*  
*  OUTPUT:	void - Return Type
*  REMARK  :	
**************************************************************************/
void IO_GPSB_DisableChannel(unsigned uCH)
{
 	GPSB *pGPSB;
	
	//Get the Base Address
	pGPSB = IO_GPSB_GetBaseAddr(uCH);

	IO_GPSB_InitSWReset(uCH);
	
	pGPSB->INTEN = 0;
	
	BITCSET(pGPSB->DMAICR, HwGPSB_DMAICR_IEP_EN|HwGPSB_DMAICR_IED_EN|HwGPSB_DMAICR_ISP|HwGPSB_DMAICR_ISD,
							HwGPSB_DMAICR_ISP|HwGPSB_DMAICR_ISD );
#ifdef CONFIG_ARCH_TCC92XX // to do
	IO_CKC_DisableBUS((HwIOBUSCFG_GPSB0 << uCH));
	tca_ckc_setperi(PERI_GPSB0 + uCH , DISABLE,  90000, PCDIRECTPLL2);
#else

#endif
}

/**************************************************************************
*  FUNCTION NAME : 
*      void IO_GPSB_WaitDone(sGPSBPORT *pPORT);
*  
*  DESCRIPTION : 
*  INPUT:
*			uCH		= Channel Number of GPSB Driver
*			pPORT	= 
*  
*  OUTPUT:	void - Return Type
*  REMARK  :	
**************************************************************************/
void IO_GPSB_WaitDone(unsigned uCH, sGPSBPORT *pPORT)
{
 	sHwGPSB *pGPSB;
	
	//Get the Base Address
	pGPSB = IO_GPSB_GetBaseAddr(uCH);
		
	if(pGPSB->MODE & HwGPSB_MODE_PCS)
		while(ISSET(*(pPORT->BASE), pPORT->uFRM));
	else
		while(ISZERO(*(pPORT->BASE), pPORT->uFRM));
}

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned	IO_GPSB_TxRxData(int iCH, void *pTxBuf, void *pRxBuf, unsigned uLength);
*  
*  DESCRIPTION : Transmit & Receive a packet data.
*  
*  INPUT:
*			iCH		= GPSB Channel & I/O Port number (same as IO_GPSB_InitCH)
*			pRxBuf	= base address of received data.
*			pTxBuf	= base address of transmitting data.
*			uLength	= data size (byte unit)
*  
*  OUTPUT:	unsigned - Return Type
*  			= 0
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:04:33    by vizirdo
**************************************************************************/
unsigned	IO_GPSB_TxRxData(unsigned uCH, void *pTxBuf, void *pRxBuf, unsigned uLength)
{
	unsigned	uBitSize;
	volatile unsigned uDummy;
	sHwGPSB	*pGPSB;
	
	pGPSB		= IO_GPSB_GetBaseAddr(uCH);

	uBitSize		= ((pGPSB->MODE & (Hw13-Hw8)) >> 8) + 1;

	//Clear Rx/Tx FIFO Conter
	BITSET(pGPSB->MODE, HwGPSB_MODE_CWF | HwGPSB_MODE_CRF);
	BITCLR(pGPSB->MODE, HwGPSB_MODE_CWF | HwGPSB_MODE_CRF);
				
	while (uLength > 0)
	{
		if (uBitSize > 16)
		{
			if (pTxBuf)
			{
				pGPSB->PORT	= word_of(pTxBuf);
				pTxBuf	= (void*)((unsigned)pTxBuf + 4);
			}
			else
				pGPSB->PORT		= 0;
			while (ISZERO(pGPSB->STAT, HwGPSB_STAT_RNE));
			if (pRxBuf)
			{
				word_of(pRxBuf)	= pGPSB->PORT;
				pRxBuf	= (void*)((unsigned)pRxBuf + 4);
			}
			else
				uDummy	= pGPSB->PORT;
			if (uLength > 4)
				uLength	-= 4;
			else
				uLength	= 0;
		}
		else if (uBitSize > 8)
		{
			if (pTxBuf)
			{
				pGPSB->PORT		= hword_of(pTxBuf);
				pTxBuf	= (void*)((unsigned)pTxBuf + 2);
			}
			else
				pGPSB->PORT		= 0;
			while (ISZERO(pGPSB->STAT, HwGPSB_STAT_RNE));
			if (pRxBuf)
			{
				hword_of(pRxBuf)	= pGPSB->PORT;
				pRxBuf	= (void*)((unsigned)pRxBuf + 2);
			}
			else
				uDummy	= pGPSB->PORT;
			if (uLength > 2)
				uLength	-= 2;
			else
				uLength	= 0;
		}
		else
		{
			if (pTxBuf)
			{
				pGPSB->PORT		= byte_of(pTxBuf);
				pTxBuf	= (void*)((unsigned)pTxBuf + 1);
			}
			else
				pGPSB->PORT		= 0;
			while (ISZERO(pGPSB->STAT, HwGPSB_STAT_RNE));
			if (pRxBuf)
			{
				byte_of(pRxBuf)	= pGPSB->PORT;
				pRxBuf	= (void*)((unsigned)pRxBuf + 1);
			}
			else
				uDummy	= pGPSB->PORT;
			if (uLength > 1)
				uLength	-= 1;
			else
				uLength	= 0;
		}
	}
	return	0;
}


/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSB_StartDMA(unsigned uCH, unsigned uINTMode, unsigned uINTEN, unsigned uDMACTR, 
*      	unsigned uPacketSize, unsigned uPacketNum, unsigned *pBuffer);
*  
*  DESCRIPTION : Start GPSB RX DMA at slave mode.
*  
*  INPUT:
*			pDRV_GPSB	= GPSB Driver Data Structure
*			uDMADir		= DMA Transfer Direction
*			uPacketSize = Packet Size
*			uPacketNum 	= Packet Num	
*			pucTxBuf 	= Tx Base Address
*			pucRxBuf 	= Rx Base Address
*			uFIFOThr	= FIFO Threshold
*			uContMode 	= DMA Control Register
*  
*  OUTPUT:	int - Return Type
*  			= 0 : setup is successful.
*			= -1 : illegal parameter
*  
*  REMARK:	created on 2006년 11월 16일 목요일 오후 9:29:16    by vizirdo
**************************************************************************/
int IO_GPSB_StartDMA(sDRV_GPSB *pDRV_GPSB, unsigned uDMADir, unsigned uPacketSize, unsigned uPacketNum, 
						unsigned *pucTxBuf, unsigned *pucRxBuf, unsigned uFIFOThr, unsigned uContMode)
{
	sHwGPSB		*pHwGPSB;
	S32			iCount;

	pHwGPSB		= pDRV_GPSB->pHwGPSB;

	//Clear GPSB Interrupt
	{
		PPIC pPIC_Reg = (PPIC)tcc_p2v(HwPIC_BASE);
		pPIC_Reg->CLR1 = HwINT1_GPSB;	/* Clear GPSB interrupt */
	}
	
	if((pucTxBuf == NULL) && (pucRxBuf == NULL))
		return DRV_GPSB_ERROR_INVALID_ARGUMENT;

	//Set DMA Packet Size and Packet Number in Buffer
	pHwGPSB->PACKET	= uPacketSize | ((uPacketNum-1)<<16);

	//Set Rx/Tx Base Address
	if(pucTxBuf)
		pHwGPSB->TXBASE = (volatile unsigned int)pucTxBuf;					
	if(pucRxBuf)
		pHwGPSB->RXBASE = (volatile unsigned int)pucRxBuf;					

	//Clear Rx/Tx FIFO Conter
	BITSET(pHwGPSB->MODE, HwGPSB_MODE_CWF | HwGPSB_MODE_CRF);
	BITCLR(pHwGPSB->MODE, HwGPSB_MODE_CWF | HwGPSB_MODE_CRF);

	//Set Interrupt Enable Register
	if(uDMADir == DRV_GPSB_DMA_RECEIVE)
	{
		pHwGPSB->INTEN |= HwGPSB_INTEN_CFGRTH(uFIFOThr)	// receive FIFO Threshold = uFIFOThr
			              | HwGPSB_INTEN_DR				// RxDMA Enable
			              | HwGPSB_INTEN_RC				// Irq  Stat clear
			              ;
	}
	else
	{
		pHwGPSB->INTEN |= HwGPSB_INTEN_CFGWTH(uFIFOThr)	// receive FIFO Threshold = uFIFOThr
			              | HwGPSB_INTEN_DW				// TxDMA Enable
			              | HwGPSB_INTEN_RC				// Irq  Stat clear
			              ;
	}

	//Clear RXPCNT, TXPCNT, IRQPCNT
	pHwGPSB->DMASTR = 0;
	
	//Set DMA Control Register
	if(pDRV_GPSB->DMAMODE.DMAMode == DRV_GPSB_DMAMODE_TS)
	{
		pHwGPSB->DMACTR |= pDRV_GPSB->DMAMODE.PIDMatch*HwGPSB_DMACTRL_MP		// PID match mode register
			               | pDRV_GPSB->DMAMODE.SyncMatch*HwGPSB_DMACTRL_MS		// Sync byte match control register
			               | uContMode*HwGPSB_DMACTRL_CT						// Continuous Mode
			               | HwGPSB_DMACTRL_MD_MP2TS							// DMA Mode Register
			               ;
	}
	else
	{
		pHwGPSB->DMACTR |= uContMode*HwGPSB_DMACTRL_CT							// Continuous Mode
			               | HwGPSB_DMACTRL_MD_NOR								// DMA Mode Register
			               ;
	}
	
	if(uDMADir == DRV_GPSB_DMA_RECEIVE)
		BITSET(pHwGPSB->DMACTR, HwGPSB_DMACTRL_DRE);
	else
		BITSET(pHwGPSB->DMACTR, HwGPSB_DMACTRL_DTE);

	//Enable GPSB DMA and Interrupt
	//Address is Fixed to BASE
	BITCSET(pHwGPSB->DMACTR, HwGPSB_DMACTRL_TXAM_MASK, HwGPSB_DMACTRL_TXAM_MULTI);
	BITCSET(pHwGPSB->DMACTR, HwGPSB_DMACTRL_RXAM_MASK, HwGPSB_DMACTRL_RXAM_MULTI);
	//Clear TX/RX Packet Counter
	BITSET(pHwGPSB->DMACTR, HwGPSB_DMACTRL_PCLR);
	//Select the IRQ Mode
	if(uDMADir == DRV_GPSB_DMA_RECEIVE)
		BITCLR(pHwGPSB->DMAICR, HwGPSB_DMAICR_IRQS_TRANS);
	else
		BITSET(pHwGPSB->DMAICR, HwGPSB_DMAICR_IRQS_TRANS);
	//Initialize PID Table
	for(iCount=0; iCount<DRV_GPSB_PID_MAX_NUM; iCount++)
		BITCLR(*(volatile unsigned long *)HwGPSB_PIDT(iCount), HwGPSB_PIDT_CH1|HwGPSB_PIDT_CH0|DRV_GPSB_PID_VALUE_MASK);
	//IRQ Packet Count
	if(pDRV_GPSB->DMAMODE.DMAMode == DRV_GPSB_DMAMODE_TS)
		BITCSET(pHwGPSB->DMAICR, HwGPSB_DMAICR_IRQPCNT_MASK, (uPacketNum>>2)-1);
	else
		BITCSET(pHwGPSB->DMAICR, HwGPSB_DMAICR_IRQPCNT_MASK, 0);					
	//Enable DMA Mode
	BITSET(pHwGPSB->DMAICR, HwGPSB_DMAICR_IED_EN);
	BITSET(pHwGPSB->DMAICR, HwGPSB_DMAICR_IEP_EN);
	BITSET(pHwGPSB->DMACTR, HwGPSB_DMACTRL_EN);	


	// GPSB Operation Enable
	BITSET(pHwGPSB->MODE, HwGPSB_MODE_EN);			

	{
		PPIC pPIC_Reg = (PPIC)tcc_p2v(HwPIC_BASE);
		
		BITSET(pPIC_Reg->MODE1 , HwINT1_GPSB);   //use level triggered interrupt 
		BITSET(pPIC_Reg->IEN1, HwINT1_GPSB);    //Enable GPSB Interrupt	
	}
	
	return 0;
}
/* end of file */

