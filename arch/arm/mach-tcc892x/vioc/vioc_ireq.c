/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : Vioc_ireq.c
*
*  Description : VIOC Interrupt Handler
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/23     0.1            created                      hskim
*******************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <mach/bsp.h>
#include <mach/io.h>
#include <linux/interrupt.h>

#include <mach/vioc_scaler.h>
#include <mach/vioc_ireq.h>

typedef void (* VIOCIreqFuncType)(int index, int irq, void *client_data); /* ireq hande function ptr type */

VIOCIreqFuncType VIOCIreqFunc[VIOC_IREQ_MAX] =
{
/* IREQ0 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_DEV0 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_DEV1 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_DEV2 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_TIMER */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA00 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA01 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA02 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA03 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA04 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA05 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA06 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA07 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA08 */ 
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA09 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA10 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA11 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA12 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA13 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA14 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA15 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA16 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RDMA17 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RESERVE0 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RESERVE1 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_MMU */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RESERVE2 */
	VIOC_IREQ_DummyHandler, 	 	/* VIOC_IREQ_RESERVE3 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE4 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_FIFO0 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_FIFO1 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE5 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE6 */
/* IREQ0 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA00 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA01 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA02 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA03 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA04 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA05 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA06 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA07 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WDMA08 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE7 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE8 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE9 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE10 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE11 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE12 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE13 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WMIX0 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WMIX1 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WMIX2 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WMIX3 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WMIX4 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_WMIX5 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE14 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE15 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE16 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE17 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_RESERVE18 */
	VIOC_IREQ_DummyHandler, 		/* VIOC_IREQ_VIQE */
	VIOC_SC_IreqHandler, 			/* VIOC_IREQ_SC0 */
	VIOC_SC_IreqHandler, 			/* VIOC_IREQ_SC1 */
	VIOC_SC_IreqHandler, 			/* VIOC_IREQ_SC2 */
	VIOC_SC_IreqHandler, 			/* VIOC_IREQ_SC3 */
};


void VIOC_IREQ_DummyHandler(int index, int irq, void *client_data)
{
	/* Dummpy */
	return;
}

static irqreturn_t VIOC_IREQ_Handler(int irq, void *client_data)
{
#if 1
	unsigned int Mask = 0x00000001UL;
	unsigned int VectorID, SYNCSTATUS0, SYNCSTATUS1;
	volatile PVIOC_IREQ_CONFIG pIREQConfig;

	pIREQConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	// read SYNCSTATUS0
	SYNCSTATUS0 = pIREQConfig->uSYNCSTATUS.nREG[0];
	for(VectorID = 0; VectorID < 32; VectorID++) {
		if((SYNCSTATUS0 >> VectorID) & Mask) {
			// call to interrupt handler for component
			VIOCIreqFunc[VectorID](VectorID, irq, client_data);
			return IRQ_HANDLED;
		}
	}

	// read SYNCSTATUS1
	SYNCSTATUS1 = pIREQConfig->uSYNCSTATUS.nREG[1];
	for(VectorID = 0; VectorID < 32; VectorID++) {
		if((SYNCSTATUS1 >> VectorID) & Mask) {
			// call to interrupt handler for component
			VIOCIreqFunc[(VectorID+32)]((VectorID+32), irq, client_data);
			return IRQ_HANDLED;
		}
	}

	return IRQ_NONE;
#else
	sHwINT	*pHwINT;
	unsigned int IREQBase;
	unsigned char VECTORID;
	VIOC_IREQ * pIREQ;
	
	pHwINT = (sHwINT *)&HwVIC_BASE;
	IREQBase = (unsigned int)HwVIOC_IREQ;
	pIREQ = (VIOC_IREQ *)IREQBase;

	//while( pIREQ->uVECTORID.bReg.IVALID == 0)
	{
		VECTORID = pIREQ->uVECTORID.bReg.VECTORID;
		VIOCIreqFunc[VECTORID](VECTORID);
	}

	/* System Interrupt Clear */
	pHwINT->ICLR = HwIEN_LCD;
#endif
}

/* Sync(with PLUSE_unit) mode Select */
void VIOC_IREQ_IrqSyncModeSet(unsigned int select)
{
	unsigned int RegMask = 0x00000001UL;
	unsigned int SelPos = select;
	PVIOC_IREQ_CONFIG pIREQConfig =  (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	if(select <= 64)
	{
		if(select/32 == 0) /* IREQ0 */
		{
			RegMask = (RegMask << SelPos);
			pIREQConfig->uIREQSELECT.nREG[0] |= RegMask;
		}
		else /* IREQ1 */
		{
			SelPos -= 32;
			RegMask = (RegMask << SelPos);
			pIREQConfig->uIREQSELECT.nREG[1] |= RegMask;
		}
	}
}

/* Async mode Select */
void VIOC_IREQ_IrqAsyncModeSet(unsigned int select)
{
	unsigned int RegMask = 0x00000001UL;
	unsigned int SelPos = select;
	PVIOC_IREQ_CONFIG pIREQConfig =  (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	if(select <= 64)
	{
		if(select/32 == 0) /* IREQ0 */
		{
			RegMask = (RegMask << SelPos);
			pIREQConfig->uIREQSELECT.nREG[0] &= ~RegMask;
		}
		else /* IREQ1 */
		{
			SelPos -= 32;
			RegMask = (RegMask << SelPos);
			pIREQConfig->uIREQSELECT.nREG[1] &= ~RegMask;
		}
	}
}

/* Interrupt Masked (Interrupt Blocking) */
void VIOC_IREQ_IrqMaskSet(unsigned int mask)
{
	unsigned int RegMask = 0x00000001UL;
	unsigned int MaskPos = mask;
	PVIOC_IREQ_CONFIG pIREQConfig =  (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);
	
	if(mask <= 64)
	{
		if(mask/32 == 0) /* IREQ0 */
		{
			RegMask = (RegMask << MaskPos);
			pIREQConfig->nIRQMASKSET.nREG[0] |= RegMask;
		}
		else /* IREQ1 */
		{
			MaskPos -= 32;
			RegMask = (RegMask << MaskPos);
			pIREQConfig->nIRQMASKSET.nREG[1] |= RegMask;
		}
	}
}

/* Interrupt UnMasked (Interrupt Enable) */
#if 1
void VIOC_IREQ_IrqMaskClear(unsigned int mask)
{
	unsigned int RegMask = 0x00000001UL;
	unsigned int MaskPos = mask;
	PVIOC_IREQ_CONFIG pIREQConfig =  (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);


	if(mask <= 64)
	{
		if(mask/32 == 0) /* IREQ0 */
		{
			RegMask = (RegMask << MaskPos);
			pIREQConfig->nIRQMASKCLR.nREG[0] |= RegMask;
		}
		else /* IREQ1 */
		{
			MaskPos -= 32;
			RegMask = (RegMask << MaskPos);
			pIREQConfig->nIRQMASKCLR.nREG[1] |= RegMask;
		}
	}
}
#else
void VIOC_IREQ_IrqMaskClear(PVIOC_IREQ_CONFIG pIREQConfig, unsigned int mask0, unsigned int mask1)
{

	pIREQ->nIRQMASKCLR0 = mask0;
#if 0	// pkjin20 : Chip Bug... Writing this register makes invalid interrupt. (Vector ID Register)
	pIREQ->nIRQMASKCLR1 = mask1;
#endif

#if 0 //[[ hskim_20110919_BEGIN -- ...
	if ( mask <= 64 )
	{
		if (mask/32 == 0) /* IREQ0 */
		{
			RegMask = (RegMask << MaskPos);
			pIREQ->nIRQMASKCLR0 |= RegMask;
		}
		else /* IREQ1 */
		{
			MaskPos -= 32;
			RegMask = (RegMask << MaskPos);
			pIREQ->nIRQMASKCLR1 |= RegMask;
		}
	}
#endif //]] hskim_20110919_END -- ...

}
#endif

void VIOC_IREQ_SetInterruptInit(void)
{
	volatile PPIC pPIC;
	pPIC = (volatile PPIC)tcc_p2v((unsigned int)HwPIC_BASE);

	pPIC->CLR0.bREG.LCDC = 0; 	// clear to VIOC interrupt
	pPIC->SEL0.bREG.LCDC = 1; 	// set to IRQ
	pPIC->MODE0.bREG.LCDC = 1; 	// set to level-triggered mode
	pPIC->POL0.bREG.LCDC = 0; 	// set to active-high signal 
	pPIC->INTMSK0.bREG.LCDC = 1; // set to IRQ output mask
	pPIC->SYNC0.bREG.LCDC = 1; 	// set to sync interrupt mode
	pPIC->IEN0.bREG.LCDC = 1; 	// set to VIOC interrupt enable
}

int VIOC_IREQ_RegisterHandle(void)
{
	int ret = 0;
	volatile PVIOC_IREQ_CONFIG pIREQConfig;
	pIREQConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	// set to IRQMASKSET0 & IRQMASKSET1
	pIREQConfig->nIRQMASKSET.nREG[0] = pIREQConfig->nIRQMASKSET.nREG[1] = 0xFFFFFFFF;

	// set to nIRQMASKCLR0 & nIRQMASKCLR1
	//pIREQConfig->nIRQMASKCLR.nREG[0] = pIREQConfig->nIRQMASKCLR.nREG[1] = 0xFFFFFFFF;

	// set to Sync Interrupt or Async Interrupt
	pIREQConfig->uIREQSELECT.nREG[0] = pIREQConfig->uIREQSELECT.nREG[1] = 0xFFFFFFFF;

	// set to VIOC configuration & enalbe of PIC
	//VIOC_IREQ_SetInterruptInit();

	// registe to VIOC interrupt handler
	// ret = request_irq(INT_LCD, VIOC_IREQ_Handler, IRQF_DISABLED, "VIOC", NULL);

	return ret;
}


//MODULE_AUTHOR("Telechips.");
//MODULE_DESCRIPTION("TCC VIOC(Interrupt) Driver");
//MODULE_LICENSE("GPL");
//arch_initcall(VIOC_IREQ_RegisterHandle);
//module_exit(VIOC_IREQ_DeregisterHandle);



