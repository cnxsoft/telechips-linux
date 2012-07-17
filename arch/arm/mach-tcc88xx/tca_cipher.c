/****************************************************************************
 *   FileName    : tca_cipher.c
 *   Description :
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/globals.h>

#if defined(CONFIG_ARCH_TCC93XX)
#include <mach/TCC93x_Structures.h>
#elif defined(CONFIG_ARCH_TCC88XX)
#include <mach/structures.h>
#else
#include <mach/TCC92x_Structures.h>
#endif
#include <mach/tcc_pca953x.h>
#include <mach/gpio.h>
#include <mach/tcc_cipher_ioctl.h>


#define MAX_CIPHER_BUFFER_LENGTH	184

#define MIN_CIPHER_BLOCK_SIZE		8

static int debug = 0;
#define dprintk(msg...)	if(debug) { printk( "tca_cipher: " msg); }

static int iIrqCipher=-1;
static int iDoneIrqHandled = FALSE;
static int iPacketIrqHandled = FALSE;

static dma_addr_t SrcDma;	/* physical */
static u_char *pSrcCpu;		/* virtual */
static dma_addr_t DstDma;	/* physical */
static u_char *pDstCpu;		/* virtual */
	
static struct clk *cipher_clk = NULL;

#if !defined(CONFIG_ARCH_TCC92XX)
extern struct tcc_freq_table_t gtHSIOClockLimitTable;
#endif
	
void tca_cipher_dma_enable(unsigned uEnable, unsigned uEndian, unsigned uAddrModeTx, unsigned uAddrModeRx)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s\n", __func__);

	if(uEnable)
	{
		/* Set the Byte Endian Mode */
		if(uEndian == TCC_CIPHER_DMA_ENDIAN_LITTLE)
			BITCLR(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_ByteEndian);
		else
			BITSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_ByteEndian);

		/* Set the Addressing Mode */
		BITCSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_AddrModeTx_Mask, HwCIPHER_DMACTR_AddrModeTx(uAddrModeTx));
		BITCSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_AddrModeRx_Mask, HwCIPHER_DMACTR_AddrModeRx(uAddrModeRx));

		/* DMA Enable */
		BITSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_Enable);

		/* Clear Done Interrupt Flag */
		iDoneIrqHandled = 0;
	}
	else
	{
		/* DMA Disable */
		BITCLR(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_Enable);
	}
}

void tca_cipher_dma_enable_request(unsigned uEnable)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s, Enable=%d\n", __func__, uEnable);

	/* Clear TX/RX packet counter */
	BITSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_ClearPacketCount);

	/* DMA Request Enable/Disable */
	if(uEnable)
		BITSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_RequestEnable);
	else
		BITCLR(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_RequestEnable);
}

void tca_cipher_interrupt_config(unsigned uTxSel, unsigned uDoneIrq, unsigned uPacketIrq)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);
	
	dprintk("%s\n", __func__);

	/* IRQ Select Direction */
	if(uTxSel)
		BITCLR(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_SelectIrq);
	else
		BITSET(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_SelectIrq);

	/* Enable for "Done" Interrupt */
	if(uDoneIrq)
		BITSET(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_EnableDoneIrq);
	else
		BITCLR(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_EnableDoneIrq);

	/* Enable for "Packet" Interrupt */
	if(uPacketIrq)
		BITSET(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_EnablePacketIrq);
	else
		BITCLR(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_EnablePacketIrq);
}

void tca_cipher_interrupt_enable(unsigned uEnable)
{
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwVPIC_BASE);
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s, Enabel=%d\n", __func__, uEnable);

	if(uEnable)
	{
		BITSET(pHwPIC->CLR1, HwINT1_CIPHER);
		BITSET(pHwPIC->SEL1, HwINT1_CIPHER);
		BITCLR(pHwPIC->POL1, HwINT1_CIPHER);
		BITCLR(pHwPIC->MODE1, HwINT1_CIPHER);
		BITSET(pHwPIC->INTMSK1, HwINT1_CIPHER);
		BITSET(pHwPIC->IEN1, HwINT1_CIPHER);
	}
	else
	{
		BITCLR(pHwPIC->INTMSK1, HwINT1_CIPHER);
		BITCLR(pHwPIC->IEN1, HwINT1_CIPHER);
	}
}

irqreturn_t tca_cipher_interrupt_handler(int irq, void *dev_id)
{
	PPIC pHwPIC = (volatile PPIC)tcc_p2v(HwVPIC_BASE);
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	//dprintk("%s\n", __func__);

	dprintk("%s, RXPCNT=%d, TXPCNT=%d\n", __func__, (pHwCIPHER->DMASTR & 0xFF00)>>16, (pHwCIPHER->DMASTR & 0xFF));

	if(pHwCIPHER->IRQCTR & HwCIPHER_IRQCTR_DoneIrqStatus)
	{
		dprintk("%s, Done Interrupt\n", __func__);

		/* Clear IRQ Status */
		BITSET(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_DoneIrqStatus);

		/* Set Done Interrupt Flag */
		iDoneIrqHandled = TRUE;
	}
	else if(pHwCIPHER->IRQCTR & HwCIPHER_IRQCTR_PacketIrqStatus)
	{
		dprintk("%s, Packet Interrupt\n", __func__);

		/* Clear IRQ Status */
		BITSET(pHwCIPHER->IRQCTR, HwCIPHER_IRQCTR_PacketIrqStatus);
	}
	else
	{
		dprintk("%s, No Cipher Interrupt\n", __func__);
	}
	
    return IRQ_HANDLED;
}

void tca_cipher_set_opmode(unsigned uOpMode)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s, uOpMode=%d\n", __func__, uOpMode);

	switch(uOpMode)
	{
		case TCC_CIPHER_OPMODE_ECB:
			BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_OperationMode_Mask, HwCIPHER_CTRL_OperationMode_ECB);
			break;
		case TCC_CIPHER_OPMODE_CBC:
			BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_OperationMode_Mask, HwCIPHER_CTRL_OperationMode_CBC);
			break;
		case TCC_CIPHER_OPMODE_CFB:
			BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_OperationMode_Mask, HwCIPHER_CTRL_OperationMode_CFB);
			break;
		case TCC_CIPHER_OPMODE_OFB:
			BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_OperationMode_Mask, HwCIPHER_CTRL_OperationMode_OFB);
			break;
		case TCC_CIPHER_OPMODE_CTR:
		case TCC_CIPHER_OPMODE_CTR_1:
		case TCC_CIPHER_OPMODE_CTR_2:
		case TCC_CIPHER_OPMODE_CTR_3:
			BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_OperationMode_Mask, HwCIPHER_CTRL_OperationMode_CTR);
			break;

		default:
			printk("%s, Err: Invalid Operation Mode\n", __func__);
			break;
	}

	dprintk("%s, Operation Mode Set End\n", __func__);
	
}

void tca_cipher_set_algorithm(unsigned uAlgorithm, unsigned uArg1, unsigned uArg2)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);
	dprintk("%s, uArg2 =%d\n", __func__, uArg2);
	
	switch(uAlgorithm)
	{
		case TCC_CIPHER_ALGORITM_AES:
			{
				/* uArg1: The Key Length in AES */
				/* uArg2: None                  */
				BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_Algorithm_Mask, HwCIPHER_CTRL_Algorithm_AES);
				BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_KeyLength_Mask, HwCIPHER_CTRL_KeyLength(uArg1));
			}
			break;

		case TCC_CIPHER_ALGORITM_DES:
			{
				/* uArg1: The Mode in DES     */
				/* uArg2: Parity Bit Location */
				BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_Algorithm_Mask, HwCIPHER_CTRL_Algorithm_DES);
				BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_DESMode_Mask, HwCIPHER_CTRL_DESMode(uArg1));

				if(uArg2)
					BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_ParityBit);
				else
					BITCLR(pHwCIPHER->CTRL, HwCIPHER_CTRL_ParityBit);
			}
			break;

		case TCC_CIPHER_ALGORITM_MULTI2:
		case TCC_CIPHER_ALGORITM_MULTI2_1:
			{
				/* uArg1: Round in Multi2 */
				/* uArg2: None */
				BITCSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_Algorithm_Mask, HwCIPHER_CTRL_Algorithm_MULTI2);
				pHwCIPHER->ROUND = uArg1;
			}
			break;

		default:
			dprintk("%s, Err: Invalid Algorithm\n", __func__);
			break;
	}

	dprintk("%s, Algorithm Set End\n", __func__);
	
}

void tca_cipher_set_baseaddr(unsigned uTxRx, unsigned char *pBaseAddr)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s\n", __func__);

	if(uTxRx == TCC_CIPHER_BASEADDR_TX)
		pHwCIPHER->TXBASE = pBaseAddr;
	else
		pHwCIPHER->RXBASE = pBaseAddr;
}

void tca_cipher_set_packet(unsigned uCount, unsigned uSize)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s, Count: %d, Size: %d\n", __func__, uCount, uSize);

	if(uCount)
		uCount -= 1;

	pHwCIPHER->PACKET = ((uCount & 0xFF) << 16) | (uSize & 0xFF);
}

void tca_cipher_set_key(unsigned char *pucData, unsigned uLength, unsigned uOption)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);
	unsigned long ulAlgorthm, ulAESKeyLength, ulDESMode;
	unsigned long *pulKeyData = (unsigned long *)pucData;

	dprintk("%s, Lenth: %d, Option: %d\n", __func__, uLength, uOption);

	ulAlgorthm = pHwCIPHER->CTRL & HwCIPHER_CTRL_Algorithm_Mask;

	/* Write Key Data */
	switch(ulAlgorthm)
	{
		case HwCIPHER_CTRL_Algorithm_AES:
			{
				ulAESKeyLength = pHwCIPHER->CTRL & HwCIPHER_CTRL_KeyLength_Mask;

				if(ulAESKeyLength == HwCIPHER_CTRL_keyLength_128)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
					pHwCIPHER->KEY2 = *pulKeyData++;
					pHwCIPHER->KEY3 = *pulKeyData++;
				}
				else if(ulAESKeyLength == HwCIPHER_CTRL_KeyLength_192)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
					pHwCIPHER->KEY2 = *pulKeyData++;
					pHwCIPHER->KEY3 = *pulKeyData++;
					pHwCIPHER->KEY4 = *pulKeyData++;
					pHwCIPHER->KEY5 = *pulKeyData++;
				}
				else if(ulAESKeyLength == HwCIPHER_CTRL_KeyLength_256)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
					pHwCIPHER->KEY2 = *pulKeyData++;
					pHwCIPHER->KEY3 = *pulKeyData++;
					pHwCIPHER->KEY4 = *pulKeyData++;
					pHwCIPHER->KEY5 = *pulKeyData++;
					pHwCIPHER->KEY6 = *pulKeyData++;
					pHwCIPHER->KEY7 = *pulKeyData++;
				}
			}
			break;

		case HwCIPHER_CTRL_Algorithm_DES:
			{
				ulDESMode = pHwCIPHER->CTRL & HwCIPHER_CTRL_DESMode_Mask;

				if(ulDESMode == HwCIPHER_CTRL_DESMode_SingleDES)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
				}
				else if(ulDESMode == HwCIPHER_CTRL_DESMode_DoubleDES)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
					pHwCIPHER->KEY2 = *pulKeyData++;
					pHwCIPHER->KEY3 = *pulKeyData++;
				}
				else if(ulDESMode == HwCIPHER_CTRL_DESMode_TripleDES2)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
					pHwCIPHER->KEY2 = *pulKeyData++;
					pHwCIPHER->KEY3 = *pulKeyData++;
				}
				else if(ulDESMode == HwCIPHER_CTRL_DESMode_TripleDES3)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
					pHwCIPHER->KEY2 = *pulKeyData++;
					pHwCIPHER->KEY3 = *pulKeyData++;
					pHwCIPHER->KEY4 = *pulKeyData++;
					pHwCIPHER->KEY5 = *pulKeyData++;
				}
			}
			break;

		case HwCIPHER_CTRL_Algorithm_MULTI2:
			{
				if(uOption == TCC_CIPHER_KEY_MULTI2_DATA)
				{
					pHwCIPHER->KEY0 = *pulKeyData++;
					pHwCIPHER->KEY1 = *pulKeyData++;
				}
				else
				{
					pHwCIPHER->KEY2 = *pulKeyData++;
					pHwCIPHER->KEY3 = *pulKeyData++;
					pHwCIPHER->KEY4 = *pulKeyData++;
					pHwCIPHER->KEY5 = *pulKeyData++;
					pHwCIPHER->KEY6 = *pulKeyData++;
					pHwCIPHER->KEY7 = *pulKeyData++;
					pHwCIPHER->KEY8 = *pulKeyData++;
					pHwCIPHER->KEY9 = *pulKeyData++;
				}
			}
			break;
			
		default:
			break;
	}

	/* Load Key Data */
	BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_KeyDataLoad);
}

void tca_cipher_set_vector(unsigned char *pucData, unsigned uLength)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);
	unsigned long ulAlgorthm;
	unsigned long *pulVectorData = (unsigned long *)pucData;

	dprintk("%s, Length: %d\n", __func__, uLength);

	ulAlgorthm = pHwCIPHER->CTRL & HwCIPHER_CTRL_Algorithm_Mask;

	/* Write Initial Vector */
	switch(ulAlgorthm)
	{
		case HwCIPHER_CTRL_Algorithm_AES:
			{
				pHwCIPHER->IV0 = *pulVectorData++;
				pHwCIPHER->IV1 = *pulVectorData++;
				pHwCIPHER->IV2 = *pulVectorData++;
				pHwCIPHER->IV3 = *pulVectorData++;
			}
			break;

		case HwCIPHER_CTRL_Algorithm_DES:
			{
				pHwCIPHER->IV0 = *pulVectorData++;
				pHwCIPHER->IV1 = *pulVectorData++;
			}
			break;

		case HwCIPHER_CTRL_Algorithm_MULTI2:
			{
				pHwCIPHER->IV0 = *pulVectorData++;
				pHwCIPHER->IV1 = *pulVectorData++;
			}
			break;

		default:
			break;
	}

	/* Load Initial Vector */
	BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_InitVectorLoad);
}

int tca_cipher_get_packetcount(unsigned uTxRx)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);
	int iPacketCount;

	dprintk("%s\n", __func__);

	if(uTxRx == TCC_CIPHER_PACKETCOUNT_TX)
		iPacketCount = (pHwCIPHER->DMASTR & 0x00FF);
	else
		iPacketCount = (pHwCIPHER->DMASTR & 0xFF00) >> 16;

	return iPacketCount;
}

int tca_cipher_get_blocknum(void)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s\n", __func__);
	
	return pHwCIPHER->BLKNUM;
}

void tca_cipher_clear_counter(unsigned uIndex)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s, Index: 0x%02x\n", __func__, uIndex);
	
	/* Clear Transmit FIFO Counter */
	if(uIndex & TCC_CIPHER_CLEARCOUNTER_TX)
		BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_ClearTxFIFO);
	/* Clear Receive FIFO Counter */
	if(uIndex & TCC_CIPHER_CLEARCOUNTER_RX)
		BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_ClearRxFIFO);
	/* Clear Block Counter */
	if(uIndex & TCC_CIPHER_CLEARCOUNTER_BLOCK)
		BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_ClearBlkCount);
}

void tca_cipher_wait_done(void)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s\n", __func__);

	/* Wait for Done Interrupt */
	while(1)
	{
		if(iDoneIrqHandled)
		{
			dprintk("Receive Done IRQ Handled\n");
			/* Clear Done IRQ Flag */
			iDoneIrqHandled = FALSE;
			break;
		}

		msleep(1);
	}

	/* Wait for DMA to be Disabled */
	while(1)
	{
		if(!(pHwCIPHER->DMACTR & HwCIPHER_DMACTR_Enable))
		{
			dprintk("DMA Disabled\n");
			break;
		}

		msleep(1);
	}
}

int tca_cipher_encrypt(unsigned char *pucSrcAddr, unsigned char *pucDstAddr, unsigned uLength)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

 	dprintk("%s, Length=%d\n", __func__, uLength);
	
	if(uLength> MAX_CIPHER_BUFFER_LENGTH)
	{
		if(pSrcCpu != NULL)
			dma_free_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, pSrcCpu, SrcDma);
		if(pDstCpu != NULL)
			dma_free_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, pDstCpu, DstDma);

		/* Allocate Physical & Virtual Address for DMA */
		pSrcCpu = dma_alloc_writecombine(0, uLength, &SrcDma, GFP_KERNEL);
		pDstCpu = dma_alloc_writecombine(0, uLength, &DstDma, GFP_KERNEL);
	}

	/* Init Virtual Address */
	memset(pSrcCpu, 0x00, uLength);
	memset(pDstCpu, 0x00, uLength);

	/* Copy Plain Text from Source Buffer */
	copy_from_user(pSrcCpu, pucSrcAddr, uLength);
	
 	/* Clear All Conunters */
	tca_cipher_clear_counter(TCC_CIPHER_CLEARCOUNTER_ALL);

	/* Select Encryption */
	BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_Encrytion);

	/* Set the Base Address */
	pHwCIPHER->TXBASE = SrcDma;
	pHwCIPHER->RXBASE = DstDma;

	/* Set the Packet Information */
	tca_cipher_set_packet(1, uLength);
	/* Configure Interrupt - Receiving */
	tca_cipher_interrupt_config(FALSE, TRUE, FALSE);
	/* Request Enable DMA */
	tca_cipher_dma_enable_request(TRUE);
	/* Clear Packet Counter */
	BITSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_ClearPacketCount);
	
	/* Load Key & InitVector Value */
	BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_KeyDataLoad);
	BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_InitVectorLoad);

	/* Enable DMA */ 
	if(uLength > MIN_CIPHER_BLOCK_SIZE)
		tca_cipher_dma_enable(TRUE, TCC_CIPHER_DMA_ENDIAN_LITTLE, TCC_CIPHER_DMA_ADDRMODE_MULTI, TCC_CIPHER_DMA_ADDRMODE_MULTI);	
	else
		tca_cipher_dma_enable(TRUE, TCC_CIPHER_DMA_ENDIAN_LITTLE, TCC_CIPHER_DMA_ADDRMODE_SINGLE, TCC_CIPHER_DMA_ADDRMODE_SINGLE);	
 
	while(!pHwCIPHER->IRQCTR & HwCIPHER_IRQCTR_DoneIrqStatus);
	while(pHwCIPHER->DMACTR & HwCIPHER_DMACTR_Enable);
	
	/* Copy Cipher Text to Destination Buffer */
	copy_to_user(pucDstAddr, pDstCpu, uLength);

	tca_cipher_dma_enable_request(FALSE);
	tca_cipher_dma_enable(FALSE, NULL, NULL, NULL);	

	#if 0 /* For Debugging */
	{
		int i;
		unsigned int *pDataAddr;

		pDataAddr = (unsigned int *)pHwCIPHER;
		printk("\n[ Register Setting ]\n");
		for(i=0; i<=(0x74/4); i+=4)
		{
			printk("0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n", pDataAddr + i, *(pDataAddr+i+0), *(pDataAddr+i+1), *(pDataAddr+i+2), *(pDataAddr+i+3));
		}
		
		pDataAddr = (unsigned int *)pSrcCpu;
		printk("\n[ Plain Text ]\n");
		for(i=0; i<(uLength/4); i+=4)
		{
			printk("0x%08x 0x%08x 0x%08x 0x%08x\n", pDataAddr[i+0], pDataAddr[i+1], pDataAddr[i+2], pDataAddr[i+3]);
		}

		pDataAddr = (unsigned int *)pDstCpu;
		printk("\n[ Cipher Text ]\n");
		for(i=0; i<(uLength/4); i+=4)
		{
			printk("0x%08x 0x%08x 0x%08x 0x%08x\n", pDataAddr[i+0], pDataAddr[i+1], pDataAddr[i+2], pDataAddr[i+3]);
		}
		printk("\n");
	}
	#endif
	if(uLength> MAX_CIPHER_BUFFER_LENGTH)
	{
		/* Release Physical & Virtual Address for DMA */
		dma_free_writecombine(0, uLength, pSrcCpu, SrcDma);
		dma_free_writecombine(0, uLength, pDstCpu, DstDma);
	}	
	return 0;
}

int tca_cipher_decrypt(unsigned char *pucSrcAddr, unsigned char *pucDstAddr, unsigned uLength)
{
	PCIPHER pHwCIPHER = (volatile PCIPHER)tcc_p2v(HwCIPHER_BASE);

	dprintk("%s, Length=%d\n", __func__, uLength);
	
	if(uLength> MAX_CIPHER_BUFFER_LENGTH)
	{
		if(pSrcCpu != NULL)
			dma_free_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, pSrcCpu, SrcDma);
		if(pDstCpu != NULL)
			dma_free_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, pDstCpu, DstDma);

		/* Allocate Physical & Virtual Address for DMA */
		pSrcCpu = dma_alloc_writecombine(0, uLength, &SrcDma, GFP_KERNEL);
		pDstCpu = dma_alloc_writecombine(0, uLength, &DstDma, GFP_KERNEL);
	}

	/* Init Virtual Address */
	memset(pSrcCpu, 0x00, uLength);
	memset(pDstCpu, 0x00, uLength);

	/* Copy Cipher Text from Source Buffer */
	copy_from_user(pSrcCpu, pucSrcAddr, uLength);
	
 	/* Clear All Conunters */
	tca_cipher_clear_counter(TCC_CIPHER_CLEARCOUNTER_ALL);

	/* Select Decryption */
	BITCLR(pHwCIPHER->CTRL, HwCIPHER_CTRL_Encrytion);

	/* Set the Base Address */
	pHwCIPHER->TXBASE = SrcDma;
	pHwCIPHER->RXBASE = DstDma;

	/* Set the Packet Information */
	tca_cipher_set_packet(1, uLength);
	/* Configure Interrupt - Receiving */
	tca_cipher_interrupt_config(FALSE, TRUE, FALSE);
	/* Request Enable DMA */
	tca_cipher_dma_enable_request(TRUE);
	/* Clear Packet Counter */
	BITSET(pHwCIPHER->DMACTR, HwCIPHER_DMACTR_ClearPacketCount);
	
	/* Load Key & InitVector Value */
	BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_KeyDataLoad);
	BITSET(pHwCIPHER->CTRL, HwCIPHER_CTRL_InitVectorLoad);

	/* Enable DMA */ 
	if(uLength > MIN_CIPHER_BLOCK_SIZE)
		tca_cipher_dma_enable(TRUE, TCC_CIPHER_DMA_ENDIAN_LITTLE, TCC_CIPHER_DMA_ADDRMODE_MULTI, TCC_CIPHER_DMA_ADDRMODE_MULTI);	
	else
		tca_cipher_dma_enable(TRUE, TCC_CIPHER_DMA_ENDIAN_LITTLE, TCC_CIPHER_DMA_ADDRMODE_SINGLE, TCC_CIPHER_DMA_ADDRMODE_SINGLE);	
 
	while(!pHwCIPHER->IRQCTR & HwCIPHER_IRQCTR_DoneIrqStatus);
	while(pHwCIPHER->DMACTR & HwCIPHER_DMACTR_Enable);
	
	tca_cipher_dma_enable_request(FALSE);
	tca_cipher_dma_enable(FALSE, NULL, NULL, NULL);	
	
	/* Copy Plain Text to Destination Buffer */
//	memcpy(pucDstAddr, pDstCpu, uLength);
	copy_to_user(pucDstAddr, pDstCpu, uLength);
	
	#if 0 /* For Debugging */
	{
		int i;
		unsigned int *pDataAddr;

		pDataAddr = (unsigned int *)pHwCIPHER;
		printk("\n[ Register Setting ]\n");
		for(i=0; i<=(0x74/4); i+=4)
		{
			printk("0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n", pDataAddr + i, *(pDataAddr+i+0), *(pDataAddr+i+1), *(pDataAddr+i+2), *(pDataAddr+i+3));
		}
		
		pDataAddr = (unsigned int *)pSrcCpu;
		printk("\n[ Cipher Text ]\n");
		for(i=0; i<(uLength/4); i+=4)
		{
			printk("0x%08x 0x%08x 0x%08x 0x%08x\n", pDataAddr[i+0], pDataAddr[i+1], pDataAddr[i+2], pDataAddr[i+3]);
		}

		pDataAddr = (unsigned int *)pDstCpu;
		printk("\n[ Plain Text ]\n");
		for(i=0; i<(uLength/4); i+=4)
		{
			printk("0x%08x 0x%08x 0x%08x 0x%08x\n", pDataAddr[i+0], pDataAddr[i+1], pDataAddr[i+2], pDataAddr[i+3]);
		}
		printk("\n");
	}
	#endif
	
	if(uLength> MAX_CIPHER_BUFFER_LENGTH)
	{
		/* Release Physical & Virtual Address for DMA */
		dma_free_writecombine(0, uLength, pSrcCpu, SrcDma);
		dma_free_writecombine(0, uLength, pDstCpu, DstDma);
	}
	return 0;
}

int tca_cipher_open(struct inode *inode, struct file *filp)
{
#if !defined(CONFIG_ARCH_TCC92XX)
	tcc_cpufreq_set_limit_table(&gtHSIOClockLimitTable, TCC_FREQ_LIMIT_CIPHER, 1);
#endif

	cipher_clk = clk_get(NULL, "cipher");
	if(IS_ERR(cipher_clk)) {
		cipher_clk = NULL;
		dprintk("cipher clock error : cannot get clock\n");
		return -EINVAL;
	}
	clk_enable(cipher_clk);

	/* Enable Cipher Interrupt */
	tca_cipher_interrupt_enable(TRUE);


#if 1
	pSrcCpu = dma_alloc_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, &SrcDma, GFP_KERNEL);
	pDstCpu = dma_alloc_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, &DstDma, GFP_KERNEL);
#else
	pSrcCpu = dma_alloc_coherent(0, MAX_CIPHER_BUFFER_LENGTH, &SrcDma, GFP_KERNEL);
	pDstCpu = dma_alloc_coherent(0, MAX_CIPHER_BUFFER_LENGTH, &DstDma, GFP_KERNEL);
#endif
	return 0;
}

int tca_cipher_release(struct inode *inode, struct file *file)
{
	dprintk("%s\n", __func__);

#if 1
	dma_free_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, pSrcCpu, SrcDma);
	dma_free_writecombine(0, MAX_CIPHER_BUFFER_LENGTH, pDstCpu, DstDma);
#else
	dma_free_coherent(0, MAX_CIPHER_BUFFER_LENGTH, pSrcCpu, SrcDma);
	dma_free_coherent(0, MAX_CIPHER_BUFFER_LENGTH, pDstCpu, DstDma);
#endif	
	
	/* Enable Cipher Interrupt */
	tca_cipher_interrupt_enable(FALSE);

	if (cipher_clk) {
		clk_disable(cipher_clk);
		clk_put(cipher_clk);
		cipher_clk = NULL;
	}

#if !defined(CONFIG_ARCH_TCC92XX)
	tcc_cpufreq_set_limit_table(&gtHSIOClockLimitTable, TCC_FREQ_LIMIT_CIPHER, 0);
#endif

	return 0;
}



