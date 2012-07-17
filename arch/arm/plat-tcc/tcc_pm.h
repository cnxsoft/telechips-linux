/*
 * arch/arm/plat-tcc/tcc_pm.h
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
 */
#ifndef __TCC_PM_H__
#define __TCC_PM_H__

#include <mach/reg_struct.h>


#define BSP_SUSPEND_MASK      	0x424E4654 //"TFNB"
#define BSP_SUSPEND_KEY			0xE1
#define BSP_SLEEP_KEY			0xE0

#if defined(CONFIG_ARCH_TCC79X)
	// SRAM addr: physical 0x10000000 -> virtual 0xF8000000
	#define SRAM_ADDR_STANDBY		0xF8000000
	#define SRAM_ADDR_VAR			0xF8001000
	#define SRAM_FUNC_SIZE			0xA00
	#define SDRAM_STORE_ADDRESS		0x20300000
#elif defined(CONFIG_ARCH_TCC83X)
	// SRAM addr: physical 0x30000000 -> virtual 0xF7000000
	#define SRAM_ADDR_STANDBY		0xF7000000
	#define SRAM_ADDR_VAR			0xF7001000
	#define SRAM_FUNC_SIZE			0xA00
	#define SDRAM_STORE_ADDRESS		0x20300000
#endif

#if defined(CONFIG_ARCH_TCC79X)
typedef struct _TCC79X_REG_{
	unsigned int uMARK;
	unsigned int uMMU[6];
	unsigned int uCPSR[64];
	NFC			mNFC; //
	CKC			mCKC; //
	IEN			mIEN; //
	VIC			mVIC; //
	TIMER		mTIMER; //
//	I2CM		mI2CM;
	DAI			mDAI;//
	DMA			mDMA; //
	DMA			mDMA1; //
	DMA			mDMA2; //
	I2C			mI2C; //
//else add
//	UART		mUART0;
//	UART		mUART1;
//	UART		mUART2;
//	UART		mUART3;
//	UARTCH		mUARTCH;
	ECCSLC		mECC;
	
	SDRAMIF		mSDRAMIF; //
	LCD			mLCD; //
	IOBC		mIOBC; //
	SYSC		mSYSC; //
//	VMT			mVMT;

	GPSB		mGPSB;//
	ADC			mADC; //
	
//	SDMMC		mSDMMC0;
//	SDMMC		mSDMMC1;
//	SDPORTCTRL	mSDPORTCTRL;
	
	LCDSI0		mLCDSI0; //
	LCDSI1		mLCDSI1; //
	NTSCPAL		mNTSCPAL; //
	NTSCPALOP	mNTSCPALOP; //
	SCALER		mSCALER; //
//	USB			mUSB;
//	USBHOST		mUSBHOST;
//	IDE			mIDE;
//	MSHC		mMSHC;
	
//	ECCSLC		mECCSLC;
//	ECCMLC4		mECCMLC4;
//	ECCMLC8		mECCMLC8;
	CIF			mCIF; //
	EFFECT		mEFFECT;//
	CIFSACLER	mCIFSACLER;//
	GE			mGE;//
//	EHI			mEHI;//
//	CDIF		mCDIF;
//	DAICDIF		mDAICDIF;
//	RTC			mRTC;
//else add
	EMC			mEMC;
//	NAND		mNAND;
//	VMT_BASE	mVMT_BASE;
//	MAILBOX		mMAILBOX;
	GPIO		mGPIO;//
} TCC79X_REG, *pTCC79X_REG;

#elif defined(CONFIG_ARCH_TCC83X)
typedef struct _TCC83X_REG_{
	unsigned int uMARK;
	unsigned int uMMU[6];
	unsigned int uCPSR[64];
	GPIO		mGPIO;
	PIC			mPIC;
	CKC			mCKC;
	VMT			mVMT;
	DMA			mDMA0;
	DMA			mDMA1;
//	USB_DMA		mUSB_DMA;
	TIMER		mTIMER;
//	RTC			mRTC;
	ADC			mADC;
	SCFG		mSCFG;
	EMC			mEMC;
//	NAND		mNAND;
	ADMA		mADMA;
//	SPDIF		mSPDIF;
	DAI			mDAI;
//	CDIF		mCDIF;
//	UART		mUART0;
//	UART		mUART1;
//	UART		mUART2;
//	UART		mUART3;
	SPI			mSPI0;
	SPI			mSPI1;
//	USB			mUSB;
//	USBHOST		mUSBHOST;
	ECCSLC		mECCSLC;
//	ECCMLC4		mECCMLC4;
//	ECCMLC8		mECCMLC8;
	NFC			mNFC;
	I2C			mI2C;
//	SDMMC		mSDMMC0;
//	SDMMC		mSDMMC1;
	EHI			mEHI;
//	IDE			mIDE;
	CIF			mCIF;
	EFFECT		mEFFECT;
	CIFSCALER	mCIFSCALER;
	LCD			mLCD;
	LCDSI0		mLCDSI0;
	LCDSI1		mLCDSI1;
	GE			mGE;
	JPEG		mJPEG;
	SCALER		mSCALER;
//	BDMA		mBDMA;
//	BDMA_VLCDDATA mBDMA_VLCDDATA;
} TCC83X_REG, *pTCC83X_REG;
#endif

#endif  /*__TCC_PM_H__*/
