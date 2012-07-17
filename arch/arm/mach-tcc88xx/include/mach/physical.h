
/****************************************************************************
*   FileName    : TCC88xx_Physical.h
*   Description :
****************************************************************************
*
*   TCC Version 1.0
*   Copyright (c) Telechips, Inc.
*   ALL RIGHTS RESERVED
*
****************************************************************************/

/****************************************************************************

  Revision History

 ****************************************************************************/

/************************************************************************
*   TCC88xx Internal Register Definition File
************************************************************************/
#ifndef __TCC88xx_H__
#define __TCC88xx_H__

/************************************************************************
*   Bit Field Definition
************************************************************************/
#define Hw37                                    (1LL << 37)
#define Hw36                                    (1LL << 36)
#define Hw35                                    (1LL << 35)
#define Hw34                                    (1LL << 34)
#define Hw33                                    (1LL << 33)
#define Hw32                                    (1LL << 32)
#define Hw31                                    0x80000000
#define Hw30                                    0x40000000
#define Hw29                                    0x20000000
#define Hw28                                    0x10000000
#define Hw27                                    0x08000000
#define Hw26                                    0x04000000
#define Hw25                                    0x02000000
#define Hw24                                    0x01000000
#define Hw23                                    0x00800000
#define Hw22                                    0x00400000
#define Hw21                                    0x00200000
#define Hw20                                    0x00100000
#define Hw19                                    0x00080000
#define Hw18                                    0x00040000
#define Hw17                                    0x00020000
#define Hw16                                    0x00010000
#define Hw15                                    0x00008000
#define Hw14                                    0x00004000
#define Hw13                                    0x00002000
#define Hw12                                    0x00001000
#define Hw11                                    0x00000800
#define Hw10                                    0x00000400
#define Hw9                                     0x00000200
#define Hw8                                     0x00000100
#define Hw7                                     0x00000080
#define Hw6                                     0x00000040
#define Hw5                                     0x00000020
#define Hw4                                     0x00000010
#define Hw3                                     0x00000008
#define Hw2                                     0x00000004
#define Hw1                                     0x00000002
#define Hw0                                     0x00000001
#define HwZERO                                  0x00000000


/*******************************************************************************
*
*    TCC88x DataSheet PART 2 SMU & PMU
*
********************************************************************************/
/************************************************************************
*   1. CKC (Base Addr = 0xF0400000)
************************************************************************/
#define HwCKC_BASE									(0xF0400000)

/************************************************************************
*   2. VPIC(Vectored Priority Interrupt Controller) (Base Addr = 0xF0401000)
************************************************************************/
#define HwVPIC_BASE									(0xF0401000)
#define HwPIC_BASE									(0xF0401000)
#define HwVIC_BASE									(0xF0401200)
#define HwPIC						((PPIC)HwPIC_BASE)
#define HwVIC						((PVIC)HwVIC_BASE)

// Interrupt Enable 0
#define HwINT0_EHI0 				Hw31										// R/W, External Host Interface0 Interrupt Enable
#define HwINT0_I2C1					Hw30										// R/W, I2C1 Interrupt Enable
#define HwINT0_GDMA					Hw29										// R/W, General DMA Controller Interrupt Enable
#define HwINT0_TSADC				Hw28										// R/W, TSADC Interrupt Enable
#define HwINT0_CAM					Hw27										// R/W, Graphic Engine 2D Hardware Interrupt Enable
#define HwINT0_3DMMU				Hw26										// R/W, 3D MMU Interrupt Enable
#define HwINT0_3DGP 				Hw25										// R/W, 3D Geometary Interrupt Enable
#define HwINT0_3DPP 				Hw24										// R/W, 3D Pixel Processor Interrupt Enable
#define HwINT0_VCDC 				Hw23										// R/W, Video CODEC Interrupt Enable
#define HwINT0_JPGE 				Hw22										// R/W, JPEG Decoder Interrupt Enable
#define HwINT0_MIPID 				Hw21										// R/W, MIPI DSI Interrupt Enable
#define HwINT0_VIPET				Hw20										// R/W, VIPET Controller Interrupt Enable
#define HwINT0_LCD1 				Hw19										// R/W, LCD Controller1 Interrupt Enable
#define HwINT0_LCD0 				Hw18										// R/W, LCD Controller0 Interrupt Enable
#define HwINT0_HDMI					Hw17										// R/W, HDMI interrupt Enable
#define HwINT0_SC1					Hw16										// R/W, Mem-to-Mem Scaler1 Interrupt Enable
#define HwINT0_SC0					Hw15										// R/W, Mem-to-Mem Scaler0 Interrupt Enable
#define HwINT0_GMAC 				Hw14										// R/W, GMAC interrupt Enable
#define HwINT0_UB20H 				Hw13										// R/W, USB2.0 Host interrupt Enable
#define HwINT0_SATA1				Hw12										// R/W, SATA Host/PCI-express interrupt Enable
#define HwINT0_SATA0				Hw11										// R/W, SATA Host/Device interrupt Enable
#define HwINT0_ISP3					Hw10										// R/W, ISP3 interrupt Enable
#define HwINT0_ISP2					Hw9 										// R/W, ISP2 interrupt Enable
#define HwINT0_ISP1					Hw8 										// R/W, ISP1 interrupt Enable
#define HwINT0_ISP0					Hw7 										// R/W, ISP0 interrupt Enable
#define HwINT0_EI3					Hw6 										// R/W, External Interrupt3 Enable
#define HwINT0_TSD					Hw6 										// R/W, TS Demux interrupt Enable
#define HwINT0_EI2					Hw5 										// R/W, External Interrupt2 Enable
#define HwINT0_EI1					Hw4 										// R/W, External Interrupt1 Enable
#define HwINT0_EI0					Hw3 										// R/W, External Interrupt0 Enable
#define HwINT0_SMUI2C				Hw2 										// R/W, SMU_I2C Interrupt Enable
#define HwINT0_TC32					Hw1 										// R/W, Timer32 Interrupt Enable
#define HwINT0_TC0					Hw0 										// R/W, Timer0 Interrupt Enable

// Interrupt Enable 1
#define HwINT1_CEER					Hw31										// R/W, Not maskable error ARM DMA interrupt enable
#define HwINT1_CDMA					Hw30										// R/W, Secure ARM DMA select interrupt enable
#define HwINT1_CDMAS 				Hw29										// R/W, Non secure ARM DMA interrupt enable
#define HwINT1_CPMU 				Hw28										// R/W, ARM System Metrics interrupt enable
#define HwINT1_AUDIO				Hw27										// R/W, AUDIO interrupt enable
#define HwINT1_ADMA 				Hw26										// R/W, AUDIO DMA interrupt enable
#define HwINT1_DAI1TX				Hw25										// R/W, DAI transmit interrupt enable
#define HwINT1_DAI1RX				Hw24										// R/W, DAI receive interrupt enable
#define HwINT1_EI7					Hw25										// R/W, External Interrupt 7 enable
#define HwINT1_EI6					Hw24										// R/W, External Interrupt 6 enable
#define HwINT1_CDRX 				Hw23										// R/W, CDIF receive interrupt enable
#define HwINT1_TSIF0				Hw22										// R/W, TS interface 1 interrupt enable
#define HwINT1_TSIF1				Hw21										// R/W, TS interface 0 interrupt enable
#define HwINT1_DAI2TX 				Hw20										// R/W, GPS AGPS interrupt enable
#define HwINT1_DAI2RX 				Hw19										// R/W, GPS TCXO expired interrupt enable
#define HwINT1_EI5					Hw20										// R/W, External Interrupt 5 enable
#define HwINT1_EI4					Hw19										// R/W, External Interrupt 4 enable
#define HwINT1_CIPHER 				Hw18										// R/W, Cipher interrupt enable
#define HwINT1_UH11					Hw17										// R/W, USB 1.1 Host interrupt Enable
#define HwINT1_UOTG 				Hw16										// R/W, USB 2.0 OTG interrupt enable
#define HwINT1_UART 				Hw15										// R/W, UART interrupt enable
#define HwINT1_SPDTX				Hw14										// R/W, SPDIF transmitter interrupt enable
#define HwINT1_SD1					Hw13										// R/W, SD/MMC 1 interrupt enable
#define HwINT1_SD0					Hw12										// R/W, SD/MMC 0 interrupt enable
#define HwINT1_RTC					Hw11										// R/W, RTC interrupt enable
#define HwINT1_RMT					Hw10										// R/W, Remote Control interrupt enable
#define HwINT1_NFC					Hw9 										// R/W, Nand flash controller interrupt enable
#define HwINT1_MS					Hw8 										// R/W, Memory Stick interrupt enable
#define HwINT1_MPEFEC				Hw7 										// R/W, MPEFEC interrupt enable
#define HwINT1_I2C0					Hw6 										// R/W, I2C0 interrupt enable
#define HwINT1_HDD					Hw5 										// R/W, HDD controller interrupt enable
#define HwINT1_GPSB 				Hw4 										// R/W, GPSB Interrupt Enable
#define HwINT1_G2D		 			Hw3 										// R/W, Reserved
#define HwINT1_SD2	 				Hw2 										// R/W, SD/MMC 2 Interrupt enable
#define HwINT1_SD3		 			Hw1 										// R/W, SD/MMC 3 Interrupt enable
#define HwINT1_EHI1 				Hw0 										// R/W, External Host Interface1 Interrupt Enable

#define HwALLMSK_FIQ				Hw1 									// FIQ mask register
#define HwALLMSK_IRQ				Hw0 									// IRQ mask register


/************************************************************************
*   3. Timer/Counter (Base Addr = 0xF0403000)
************************************************************************/
#define HwTMR_BASE									(0xF0403000)

/************************************************************************
*   4. PMU(Power Management Unit) (Base Addr = 0xF0404000)
************************************************************************/
#define HwPMU_BASE									(0xF0404000)

// Wake UP Control
#define	HwPMU_WKUP_GPIOE25			Hw31
#define	HwPMU_WKUP_GPIOE24			Hw30
#define	HwPMU_WKUP_GPIOE05			Hw29
#define	HwPMU_WKUP_GPIOE04			Hw28
#define	HwPMU_WKUP_GPIOB31			Hw27
#define	HwPMU_WKUP_GPIOB30			Hw26
#define	HwPMU_WKUP_GPIOA15			Hw25
#define	HwPMU_WKUP_GPIOA14			Hw24
#define	HwPMU_WKUP_GPIOA13			Hw23
#define	HwPMU_WKUP_GPIOA12			Hw22
#define	HwPMU_WKUP_GPIOA11			Hw21
#define	HwPMU_WKUP_GPIOA10			Hw20
#define	HwPMU_WKUP_GPIOA07			Hw19
#define	HwPMU_WKUP_GPIOA06			Hw18
#define	HwPMU_WKUP_GPIOA05			Hw17
#define	HwPMU_WKUP_GPIOA04			Hw16
#define	HwPMU_WKUP_GPIOA03			Hw15
#define	HwPMU_WKUP_GPIOA02			Hw14
#define	HwPMU_WKUP_TSC_UPDOWN		Hw13										// Touch Screen Up/Down Signal
#define	HwPMU_WKUP_TSC_STOP_WKU		Hw12										// Touch Screen Stop Wakeup Signal
#define	HwPMU_WKUP_GPIOD18			Hw11
#define	HwPMU_WKUP_TSC_WKU			Hw10										// Touch Screen Wake-Up Signal
#define	HwPMU_WKUP_GPIOF23			Hw9
#define	HwPMU_WKUP_GPIOF24			Hw8
#define	HwPMU_WKUP_GPIOF25			Hw7
#define	HwPMU_WKUP_GPIOF26			Hw6
#define	HwPMU_WKUP_GPIOF27			Hw5
#define	HwPMU_WKUP_GPIOC31			Hw4
#define	HwPMU_WKUP_GPIOC30			Hw3
#define	HwPMU_WKUP_GPIOC29			Hw2
#define	HwPMU_WKUP_GPIOC28			Hw1
#define	HwPMU_WKUP_RTCWKUP			Hw0											// RTC Wakeup Output Signal


/************************************************************************
*   5. SMU_I2C (Base Addr = 0xF0405000)
************************************************************************/
#define HwSMUI2C_BASE								(0xF0405000)
#define HwSMUI2C_MASTER0_BASE						(0xF0405000)
#define HwSMUI2C_MASTER1_BASE						(0xF0405040)
#define HwSMUI2C_COMMON_BASE						(0xF0405080)


/************************************************************************
*   6. GPIO & Port Multiplexing (Base Addr = 0xF0308000)
************************************************************************/
#define HwGPIO_BASE									(0xF0308000)
#define HwGPIOA_BASE								(0xF0308000)
#define HwGPIOB_BASE								(0xF0308040)
#define HwGPIOC_BASE								(0xF0308080)
#define HwGPIOD_BASE								(0xF03080C0)
#define HwGPIOE_BASE								(0xF0308100)
#define HwGPIOF_BASE								(0xF0308140)
#define HwGPIOG_BASE								(0xF03081C0)

#define HwGPIO						((PGPIO)HwGPIO_BASE)
#define HwGPIOA						((PGPION)HwGPIOA_BASE)
#define HwGPIOB						((PGPION)HwGPIOB_BASE)
#define HwGPIOC						((PGPION)HwGPIOC_BASE)
#define HwGPIOD						((PGPION)HwGPIOD_BASE)
#define HwGPIOE						((PGPION)HwGPIOE_BASE)
#define HwGPIOF						((PGPION)HwGPIOF_BASE)
#define HwGPIOG						((PGPION)HwGPIOG_BASE)
#define HwEINTSEL					((PGPIOINT)HwEINTSEL_BASE)


#define	HwPORTCFG_GPFN0(X)			((X)<<0)	// 0~3
#define HwPORTCFG_GPFN0_MASK		(0xF)		// HwPORTCFG_GPFN0(15)
#define	HwPORTCFG_GPFN1(X)			((X)<<4)	// 4~7
#define HwPORTCFG_GPFN1_MASK		(0xF<<4)	// HwPORTCFG_GPFN1(15)
#define	HwPORTCFG_GPFN2(X)			((X)<<8)	// 8~11
#define HwPORTCFG_GPFN2_MASK		(0xF<<8)	// HwPORTCFG_GPFN2(15)
#define	HwPORTCFG_GPFN3(X)			((X)<<12)	// 12~15
#define HwPORTCFG_GPFN3_MASK		(0xF<<12)	// HwPORTCFG_GPFN3(15)
#define	HwPORTCFG_GPFN4(X)			((X)<<16)	// 16~19
#define HwPORTCFG_GPFN4_MASK		(0xF<<16)	// HwPORTCFG_GPFN4(15)
#define	HwPORTCFG_GPFN5(X)			((X)<<20)	// 20~23
#define HwPORTCFG_GPFN5_MASK		(0xF<<20)	// HwPORTCFG_GPFN5(15)
#define	HwPORTCFG_GPFN6(X)			((X)<<24)	// 24~27
#define HwPORTCFG_GPFN6_MASK		(0xF<<24)	// HwPORTCFG_GPFN6(15)
#define	HwPORTCFG_GPFN7(X)			((X)<<28)	// 28~31
#define HwPORTCFG_GPFN7_MASK		(0xF<<28)	// HwPORTCFG_GPFN7(15)

#define HwEINTSEL_BASE								(0xF0308184)
#define HwSYSMONEN_BASE								(0xF030818C)
#define HwECID_BASE									(0xF0308190)

/************************************************************************
*   8. SMU Config (Base Addr = 0xF05F4000)
************************************************************************/
#define HwSMUCONFIG_BASE							(0xF05F4000)




/*******************************************************************************
*
*    TCC88x DataSheet PART 3 GRAPHIC BUS
*
********************************************************************************/

/************************************************************************
*   4. 2D/3D GPU Register Define				(Base Addr = 0xF0000000) // R/W
************************************************************************/
#define HwGPU_BASE								(0xF0000000)

/************************************************************************
*   4-3. Pixel Processor Register Define		(Base Addr = 0xF0000000) // R/W
************************************************************************/
#define HwPIXELPROCESSOR_BASE					(0xF0000000)
#define HwGPU_PXL_FRAMEREGISTERS_BASE			(0xF0000000)
#define HwGPU_PXL_WRITEBACKREGISTER_BASE		(0xF0000100)
#define HWGPU_PXL_MANAGEMENTREGISTER_BASE		(0xF0001000)

/************************************************************************
*   4-4. Geometry Processor Register Define         (Base Addr = 0xF0002000) // R/W
************************************************************************/
#define HwGEOMETRYPROCESSOR_BASE				(0xF0002000)
#define HwGPU_GEO_CONTROLREGISTER_BASE			(0xF0002000)
//#define HwGPU_GEO_PLBCONFIGREGISTER_BASE		0x0100
//#define HwGPU_GEO_VERTEXSHADER_BASE			0x0000

/*************************************************************************
*   4-5. MMU Register Define                        (Base Addr = 0xF0003000) // R/W
************************************************************************/
#define HwGPUMMUCONFIG_BASE						(0xF0003000)

/************************************************************************
*   5. Graphic Bus Configuration Register Define  (Base Addr = 0xF0004000) // R/W
************************************************************************/
#define HwGRPBUSCONFIG_BASE						(0xF0004000)


/*******************************************************************************
*
*    TCC88x DataSheet PART 4 MEMORY BUS
*
********************************************************************************/
/*
* 0xF0300000    Memory Bus Matrix Control
* 0xF0301000    PL340 Controller (LPDDR)
* 0xF0302000    PL341 Controller (DDR2)
* 0xF0303000    Memory Controller Configuration
* 0xF0304000    DDR PHY Configuration
* 0xF0305000    Memory Bus Configuration 1
* 0xF0306000    LPCON Controller (LPDDR, LPDDR2)
* 0xF0307000    Memory Bus Configuration 2
* 0xF0308000    GPIO
* 0xF0309000    DDR2/3 Controller (DDR2, DDR3)
* 0xF0600000    Hardware MMU Table
*/

/************************************************************************
*   3. Multi-layer Bus Matrix Register (Base Addr = 0xF0300000) // R/W
************************************************************************/
#define HwMULTILAYERBUSMATRIX_BASE				(0xF0300000)
#define HwBUSPRIORITY_READ						(HwMULTILAYERBUSMATRIX_BASE + 0x408)
#define HwBUSPRIORITY_WRITE						(HwMULTILAYERBUSMATRIX_BASE + 0x40C)

#define HwBUSPRIORITY_IOBUS						(0x0 << 24)
#define HwBUSPRIORITY_GBUS						(0x1 << 24)
#define HwBUSPRIORITY_CPUBUS					(0x2 << 24)
#define HwBUSPRIORITY_DDIBUS					(0x3 << 24)
#define HwBUSPRIORITY_VBUS						(0x4 << 24)
#define HwBUSPRIORITY_HSIOBUS					(0x5 << 24)
#define HwBUSPRIORITY_CAMBUS					(0x6 << 24)

#define HwBUSPRIORITY_0							(0x0 << 8)
#define HwBUSPRIORITY_1							(0x1 << 8)
#define HwBUSPRIORITY_2							(0x2 << 8)
#define HwBUSPRIORITY_3							(0x3 << 8)
#define HwBUSPRIORITY_4							(0x4 << 8)
#define HwBUSPRIORITY_5							(0x5 << 8)
#define HwBUSPRIORITY_6							(0x6 << 8)
/************************************************************************
*   4. DDR SDRAM Memory Controller (Base Addr = 0xF0301000) // R/W
************************************************************************/
#define HwLPDDR_BASE							(0xF0301000)
#define HwDDR2_BASE								(0xF0302000)
#define HwLPCON_BASE							(0xF0306000)
#define HwDDR23_BASE							(0xF0309000)

#define HwDDRIFCFG_BASE							(0xF0303000)
#define HwDDRPHY_BASE							(0xF0304400)

/************************************************************************
*   6. Memory Bus Configuration (Base Addr = ) // R/W
************************************************************************/
#define HwMEMBUSCFG1_BASE						(0xF0305000)
#define HwMEMBUSCFG2_BASE						(0xF0307000)

/************************************************************************
*   7. Internal Memory BUS Configuration (Base Addr = ) // R/W
************************************************************************/
#define HwVMT_BASE								(0x20000000)
#define HwINTNLMEMBUSCFG_BASE					(0xF0600000)


/*******************************************************************************
*
*    TCC88x DataSheet PART 5 IO BUS
*
********************************************************************************/

/*******************************************************************************
*    4. Memory Stick Host Controller Register Define (Base Addr = 0xF0590000)
********************************************************************************/
#define HwSMSHC_BASE							(0xF0590000)
#define HwSMSHC_PORTCFG_BASE					(0xF05F1000)

/*******************************************************************************
*    5. SD/SDIO/MMC Host Controller Register Define (Base Addr = 0xF05A0000)
********************************************************************************/
#define HwSDMMC_BASE							(0xF05A0000)
#define HwSDMMC0_BASE							(0xF05A0000)
#define HwSDMMC1_BASE							(0xF05A0200)
#define HwSDMMC2_BASE							(0xF05A0400)
#define HwSDMMC3_BASE							(0xF05A0600)
#define HwSDMMC_CHCTRL_BASE						(0xF05A0800)

/*******************************************************************************
*    6. EHI Register Define (Base Addr = 0xF0570000)
********************************************************************************/
#define HwEHI_BASE								(0xF0570000)
#define HwEHI0_BASE								(0xF0570000)
#define HwEHI1_BASE								(0xF0580000)

/*******************************************************************************
*    7. DMA Controller Register Define (Base Addr = 0xF0540000)
********************************************************************************/
#define HwGDMA_BASE								(0xF0540000)
#define HwGDMA0_BASE							(0xF0540000)
#define HwGDMA1_BASE							(0xF0540100)
#define HwGDMA2_BASE							(0xF0540200)
#define HwGDMA3_BASE							(0xF0540300)

/*******************************************************************************
*    8. GPSB Controller Register Define (Base Addr = 0xF0535000)
********************************************************************************/
#define HwGPSBCH0_BASE							(0xF0535000)
#define HwGPSBCH1_BASE							(0xF0535100)
#define HwGPSBCH2_BASE							(0xF0535200)
#define HwGPSBCH3_BASE							(0xF0535300)
#define HwGPSBCH4_BASE							(0xF0535400)
#define HwGPSBCH5_BASE							(0xF0535500)
#define HwGPSB_PORTCFG_BASE						(0xF0535800)
#define HwGPSB_PIDTABLE_BASE					(0xF0535F00)

#define	HwGPSB_BASE(X)							(HwGPSBCH0_BASE+(X)*0x100)
#define	HwGPSB_MAX_CH				4

// Status
#define	HwGPSB_STAT_WOR				Hw8											// Write FIFO over-run error flag
#define	HwGPSB_STAT_RUR				Hw7											// Read FIFO under-run error flag
#define	HwGPSB_STAT_WUR				Hw6											// Write FIFO under-run error flag
#define	HwGPSB_STAT_ROR				Hw5											// Read FIFO over-run error flag
#define	HwGPSB_STAT_RF				Hw4											// Read FIFO full flag
#define	HwGPSB_STAT_WE				Hw3											// Write FIFO empty flag
#define	HwGPSB_STAT_RNE				Hw2											// Read FIFO not empty flag
#define	HwGPSB_STAT_WTH				Hw1											// Wrtie FIFO valid entry count is under threshold
#define	HwGPSB_STAT_RTH				Hw0											// Read FIFO valid entry increased over threshold

#define	HwGPSB_PIDT(X)				(HwGPSB_PIDTABLE_BASE+(X)*4)	// R/W, PID Table Register
#define	HwGPSB_PIDT_CH2				Hw31											// Channel 2 enable
#define	HwGPSB_PIDT_CH1				Hw30											// Channel 1 enable
#define	HwGPSB_PIDT_CH0				Hw29											// Channel 0 enable

#define	HwGPSB_DMAICR_ISD			Hw29										// IRQ Status for "Done Interrupt"
#define	HwGPSB_DMAICR_ISP			Hw28										// IRQ Status for "Packet Interrupt"
#define	HwGPSB_DMAICR_IRQS_TRANS	Hw20										// IRQ is generated when transmit is done.
#define	HwGPSB_DMAICR_IRQS_RCV		HwZERO										// IRQ is generated when receiving is done.
#define	HwGPSB_DMAICR_IED_EN		Hw17										// Enable "Done Interrupt"
#define	HwGPSB_DMAICR_IEP_EN		Hw16										// Enable "Packet Interrupt"
#define	HwGPSB_DMAICR_IRQPCNT_MASK	(Hw13-1)

// DMA Control
#define	HwGPSB_DMACTRL_DTE			Hw31										// Transmit DMA request enable
#define	HwGPSB_DMACTRL_DRE			Hw30										// Receive DMA request enable
#define	HwGPSB_DMACTRL_CT			Hw29										// Continuous mode enable
#define	HwGPSB_DMACTRL_END			Hw28										// Byte endian mode register
#define	HwGPSB_DMACTRL_MP			Hw19										// PID match mode register
#define	HwGPSB_DMACTRL_MS			Hw18										// Sync byte match control register
#define	HwGPSB_DMACTRL_TXAM(X)		((X)*Hw16)
#define	HwGPSB_DMACTRL_TXAM_MULTI	HwGPSB_DMACTRL_TXAM(0)						// TX address is rolling within full packet range
#define	HwGPSB_DMACTRL_TXAM_FIXED	HwGPSB_DMACTRL_TXAM(1)						// TX address is fixed to TXBASE
#define	HwGPSB_DMACTRL_TXAM_SINGLE	HwGPSB_DMACTRL_TXAM(2)						// TX address is rolling within 1 packet range
#define	HwGPSB_DMACTRL_TXAM_MASK	HwGPSB_DMACTRL_TXAM(3)
#define	HwGPSB_DMACTRL_RXAM(X)		((X)*Hw14)
#define	HwGPSB_DMACTRL_RXAM_MULTI	HwGPSB_DMACTRL_RXAM(0)						// RX address is rolling within full packet range
#define	HwGPSB_DMACTRL_RXAM_FIXED	HwGPSB_DMACTRL_RXAM(1)						// RX address is fixed to RXBASE
#define	HwGPSB_DMACTRL_RXAM_SINGLE	HwGPSB_DMACTRL_RXAM(2)						// RX address is rolling within 1 packet range
#define	HwGPSB_DMACTRL_RXAM_MASK	HwGPSB_DMACTRL_RXAM(3)
#define	HwGPSB_DMACTRL_MD_NOR		HwZERO										// normal mode
#define	HwGPSB_DMACTRL_MD_MP2TS		Hw4											// MPEG2-TS mode
#define	HwGPSB_DMACTRL_PCLR			Hw2											// Clear TX/RX Packet Counter
#define	HwGPSB_DMACTRL_EN			Hw0											// DMA enable

// Interrupt Enable
#define	HwGPSB_INTEN_DW				Hw31										// DMA request enable for transmit FIFO
#define	HwGPSB_INTEN_DR				Hw30										// DMA request enable for receive FIFO
#define	HwGPSB_INTEN_SWD_BHW		Hw27										// Swap byte in half-word
#define	HwGPSB_INTEN_SWD_HWW		Hw26										// Swap half-word in word
#define	HwGPSB_INTEN_SRD_BHW		Hw25										// Swap byte in half-word
#define	HwGPSB_INTEN_SRD_HWW		Hw24										// Swap half-word in word
#define	HwGPSB_INTEN_CFGWTH(X)		((X)*Hw20)									// Write FIFO threshold for Interrupt or DMA Request
#define	HwGPSB_INTEN_CFGWTH_MASK	HwGPSB_INTEN_CFGWTH(7)
#define	HwGPSB_INTEN_CFGRTH(X)		((X)*Hw16)									// Read FIFO threshold for Interrupt or DMA Request
#define	HwGPSB_INTEN_CFGRTH_MASK	HwGPSB_INTEN_CFGRTH(7)
#define	HwGPSB_INTEN_RC				Hw15										// Clear status[8:0] at the end of read cycle
#define	HwGPSB_INTEN_WOR			Hw8											// Write FIFO over-run error interrupt enable
#define	HwGPSB_INTEN_RUR			Hw7											//Read FIFO under-run error flag interrupt enable
#define	HwGPSB_INTEN_WUR			Hw6											// Write FIFO under-run error flag interrupt enable
#define	HwGPSB_INTEN_ROR			Hw5											// Read FIFO over-run error flag interrupt enable
#define	HwGPSB_INTEN_RF				Hw4											// Read FIFO full flag interrupt enable
#define	HwGPSB_INTEN_WE				Hw3											// Write FIFO empty flag interrupt enable
#define	HwGPSB_INTEN_RNE			Hw2											// Read FIFO not empty flag interrupt enable
#define	HwGPSB_INTEN_WTH			Hw1											// Wrtie FIFO valid entry count is under threshold interrupt enable
#define	HwGPSB_INTEN_RTH			Hw0											// Read FIFO valid entry increased over threshold interrupt enable


// Mode
#define	HwGPSB_MODE_DIVLDV(X)		((X)*Hw24)
#define	HwGPSB_MODE_DIVLDV_MASK		HwGPSB_MODE_DIVLDV(255)
#define	HwGPSB_MODE_TRE				Hw23										// Master recovery time (TRE+1)*SCKO
#define	HwGPSB_MODE_THL				Hw22										// Master hold time (THL+1)*SCKO
#define	HwGPSB_MODE_TSU				Hw21										// Master setup time (TSU+1)*SCKO
#define	HwGPSB_MODE_PCS				Hw20										// Polarity control for CS(FRM) - Master Only
#define	HwGPSB_MODE_PCS_HIGH		Hw20
#define	HwGPSB_MODE_PCD				Hw19										// Polarity control for CMD(FRM)- Master Only
#define	HwGPSB_MODE_PCD_HIGH		Hw19
#define	HwGPSB_MODE_PWD				Hw18										// Polarity control for transmit data - Master Only
#define	HwGPSB_MODE_PWD_RISE		Hw18
#define	HwGPSB_MODE_PRD				Hw17										// Polarity control for receive data - Master Only
#define	HwGPSB_MODE_PRD_FALL		Hw17
#define	HwGPSB_MODE_PCK				Hw16										// Polarity control for serial clock
#define	HwGPSB_MODE_CRF				Hw15										// Clear receive FIFO counter
#define	HwGPSB_MODE_CWF				Hw14										// Clear transmit FIFO counter
#define	HwGPSB_MODE_BWS(X)			((X)*Hw8)									// Bit width Selection.(BWS+1. Valid = 7~31)
#define	HwGPSB_MODE_BWS_MASK		HwGPSB_MODE_BWS(31)
#define	HwGPSB_MODE_SD				Hw7											// Data shift direction control
#define	HwGPSB_MODE_LB				Hw6											// Data looop-back enable
#define	HwGPSB_MODE_CTF				Hw4											// Continuous transfer mode enable
#define	HwGPSB_MODE_EN				Hw3											// Operation enable
#define	HwGPSB_MODE_SLV				Hw2											// Slave mode configuration
#define	HwGPSB_MODE_MD_SPI			HwZERO										// SPI compatible
#define	HwGPSB_MODE_MD_SSP			Hw0											// SSP Compatible

/*******************************************************************************
*    9. Overlay Mixer Register Define (Base Addr = 0xF0560000)
********************************************************************************/
#define HwOVERLAYMIXER_BASE						(0xF0560000)


#define	HwGE_FCH_SSB				(Hw24+Hw25+Hw26)							// Operation Mode
#define	HwGE_DCH_SSB				(Hw24+Hw25+Hw26)							// Operation Mode

// Front-End Channel 0 Control
#define	HwGE_FCHO_OPMODE			(Hw8+Hw9+Hw10)								// Operation Mode
#define	HwGE_FCHO_SDFRM				(Hw0+Hw1+Hw2+Hw3+Hw4)						// Source Data Format

// Front-End Channel 1 Control
#define	HwGE_FCH1_OPMODE			(Hw8+Hw9+Hw10)								// Operation Mode
#define	HwGE_FCH1_SDFRM				(Hw0+Hw1+Hw2+Hw3+Hw4)						// Source Data Format

// Front-End Channel 2 Control
#define	HwGE_FCH2_OPMODE			(Hw8+Hw9+Hw10)								// Operation Mode
#define	HwGE_FCH2_SDFRM				(Hw0+Hw1+Hw2+Hw3+Hw4)						// Source Data Format

// Source Control
#define Hw2D_SCTRL_S2_ARITHMODE 			(Hw27+Hw26+Hw25)
#define Hw2D_SCTRL_S1_ARITHMODE 			(Hw24+Hw23+Hw22)
#define Hw2D_SCTRL_S0_ARITHMODE 			(Hw21+Hw20+Hw19)
#define Hw2D_SCTRL_S2_Y2REN 				(Hw18)
#define Hw2D_SCTRL_S1_Y2REN 				(Hw17)
#define Hw2D_SCTRL_S0_Y2REN 				(Hw16)
#define Hw2D_SCTRL_S2_Y2RMODE 				(Hw14+Hw13)
#define Hw2D_SCTRL_S1_Y2RMODE 				(Hw12+Hw11)
#define Hw2D_SCTRL_S0_Y2RMODE 				(Hw10+Hw9)
#define Hw2D_SCTRL_S2_CHROMAEN				(Hw8)
#define Hw2D_SCTRL_S1_CHROMAEN				(Hw7)
#define Hw2D_SCTRL_S0_CHROMAEN				(Hw6)
#define Hw2D_SCTRL_S2_SEL					(Hw5+Hw4)
#define Hw2D_SCTRL_S1_SEL					(Hw3+Hw2)
#define Hw2D_SCTRL_S0_SEL					(Hw1+Hw0)

// Source Operator Pattern
#define	HwGE_OP_ALL					(HwGE_ALPHA + HwGE_PAT_RY + HwGE_PAT_GU + HwGE_PAT_BV)
#define	HwGE_ALPHA					(Hw24+Hw25+Hw26+Hw27+Hw28+Hw29+Hw30+Hw31)						// ALPHA VALUE
#define	HwGE_PAT_RY					(Hw16+Hw17+Hw18+Hw19+Hw20+Hw21+Hw22+Hw23)	// Pattern Value RED,   Y
#define	HwGE_PAT_GU					(Hw8+Hw9+Hw10+Hw11+Hw12+Hw13+Hw14+Hw15)		// Pattern Value GREEN, U
#define	HwGE_PAT_BV					(Hw0+Hw1+Hw2+Hw3+Hw4+Hw5+Hw6+Hw7)			// Pattern Value BULE,  V

// Source Operation Control
#define	HwGE_OP_CTRL_ASEL1			(Hw23+Hw24)
#define	HwGE_OP_CTRL_CSEL1			(Hw21+Hw22)									// Chroma-key Source Selection
#define	HwGE_OP_CTRL_OP1_MODE		(Hw16+Hw17+Hw18+Hw19+Hw20)					// Operation 1 Mode
#define	HwGE_OP_CTRL_ASEL0			(Hw7+Hw8)
#define	HwGE_OP_CTRL_CSEL0			(Hw5+Hw6)									// Chroma-key Source Selection
#define	HwGE_OP_CTRL_OP0_MODE		(Hw0+Hw1+Hw2+Hw3+Hw4)						// Operation 0 Mode

// Back -End Channel Control
#define	HwGE_BCH_DCTRL_MABC			Hw21
#define	HwGE_BCH_DCTRL_YSEL			Hw18										// YUV4:4:4 to YUVx:x:x Y Control
#define	HwGE_BCH_DCTRL_XSEL			(Hw16+Hw17)									// YUV4:4:4 to YUVx:x:x X Control
#define	HwGE_BCH_DCTRL_CEN			Hw15										// Destination Format Converter Control
#define	HwGE_BCH_DCTRL_CMODE		(Hw13+Hw14)									// RGBtoYUV Converter Type
#define	HwGE_BCH_DCTRL_DSUV			Hw11
#define	HwGE_BCH_DCTRL_OPMODE		(Hw8+Hw9+Hw10)								// Operation Mode COPY, MIRROR, ROTATE
#define	HwGE_BCH_DCTRL_DOP			Hw6
#define	HwGE_BCH_DCTRL_DEN			Hw5
#define	HwGE_BCH_DCTRL_DDFRM		(Hw0+Hw1+Hw2+Hw3+Hw4)						// Destination Data Format

// Graphic Engine Control
#define	HwGE_GE_INT_EN				Hw16										// Graphic Engine Interrupt Enable
#define	HwGE_GE_CTRL_EN				(Hw0+Hw1+Hw2)								// Graphic Engine Enable

// Graphic Engine Interrupt Request
#define	HwGE_GE_IREQ_FLG			Hw16										// Graphic Engine Flag Bit
#define	HwGE_GE_IREQ_IRQ			Hw0

/*******************************************************************************
*    10. Audio0(7.1ch) Register Define (Base Addr = 0xF0532000)
********************************************************************************/
#define HwAUDIO0_DMA_BASE						(0xF0532000)
#define HwAUDIO0_DAI_BASE						(0xF0533000)
#define HwAUDIO0_CDIF_BASE						(0xF0533080)
#define HwAUDIO0_SPDIFTX_BASE					(0xF0534000)
#define HwAUDIO0_SPDIFRX_BASE					(0xF0534800)

/*******************************************************************************
*    11. Audio1(Stereo) Register Define (Base Addr = 0xF0536000)
********************************************************************************/
#define HwAUDIO1_DAI0_BASE						(0xF0536000)
#define HwAUDIO1_CDIF_BASE						(0xF0536080)
#define HwAUDIO1_DAI1_BASE						(0xF0536100)
#define HwAUDIO1_SPDIFTX_BASE					(0xF0537000)

/*******************************************************************************
*    12. NFC(NAND Flash Controller) Register Define (Base Addr = 0xF05B0000)
********************************************************************************/
#define HwNFC_BASE								(0xF05B0000)

#define	HwNFC_CTRL_CS3SEL_1			Hw31							// Nand Flash nCS3 is High (Disabled)
#define	HwNFC_CTRL_CS3SEL_0			HwZERO							// Nand Flash nCS3 is Low (Enabled)
#define	HwNFC_CTRL_CS2SEL_1			Hw30							// Nand Flash nCS2 is High (Disabled)
#define	HwNFC_CTRL_CS2SEL_0			HwZERO							// Nand Flash nCS2 is Low (Enabled)
#define	HwNFC_CTRL_CS1SEL_1			Hw29							// Nand Flash nCS1 is High (Disabled)
#define	HwNFC_CTRL_CS1SEL_0			HwZERO							// Nand Flash nCS1 is Low (Enabled)
#define	HwNFC_CTRL_CS0SEL_1			Hw28							// Nand Flash nCS0 is High (Disabled)
#define	HwNFC_CTRL_CS0SEL_0			HwZERO							// Nand Flash nCS0 is Low (Enabled)
#define	HwNFC_CTRL_CFG_nCS3			HwNFC_CTRL_CS3SEL_1
#define	HwNFC_CTRL_CFG_nCS2			HwNFC_CTRL_CS2SEL_1
#define	HwNFC_CTRL_CFG_nCS1			HwNFC_CTRL_CS1SEL_1
#define	HwNFC_CTRL_CFG_nCS0			HwNFC_CTRL_CS0SEL_1
#define	HwNFC_CTRL_CFG_NOACT		(Hw31|Hw30|Hw29|Hw28)

#define	HwNFC_CTRL_RDNDLY(X)		((X)*Hw25)						// Nand Flash Read Data Delay
#define	HwNFC_CTRL_RDNDLY_0			HwNFC_CTRL_RDNDLY(0)			// Nand Flash Read Data Delay (NONE)
#define	HwNFC_CTRL_RDNDLY_1			HwNFC_CTRL_RDNDLY(1)			// Nand Flash Read Data Delay (1 Clock)
#define	HwNFC_CTRL_RDNDLY_2			HwNFC_CTRL_RDNDLY(2)			// Nand Flash Read Data Delay (2 Clock)
#define	HwNFC_CTRL_RDNDLY_3			HwNFC_CTRL_RDNDLY(3)			// Nand Flash Read Data Delay (3 Clock)
#define	HwNFC_CTRL_RDNDLY_4			HwNFC_CTRL_RDNDLY(4)			// Nand Flash Read Data Delay (4 Clock)
#define	HwNFC_CTRL_RDNDLY_5			HwNFC_CTRL_RDNDLY(5)			// Nand Flash Read Data Delay (5 Clock)
#define	HwNFC_CTRL_RDNDLY_6			HwNFC_CTRL_RDNDLY(6)			// Nand Flash Read Data Delay (6 Clock)
#define	HwNFC_CTRL_RDNDLY_7			HwNFC_CTRL_RDNDLY(7)			// Nand Flash Read Data Delay (7 Clock)

#define	HwNFC_CTRL_ARFW_FLAG_EN		Hw24							// Nand Flash Automatically Ready Flag Wait Enanble
#define	HwNFC_CTRL_ARFW_FLAG_DIS	~Hw24							// Nand Flash Automatically Ready Flag Wait Disable

#define	HwNFC_CTRL_DACK_EN			Hw20							// Nand Flash DMA Acknowledge Enable
#define	HwNFC_CTRL_DACK_DIS			~Hw20							// Nand Flash DMA Acknowledge Disable
#define	HwNFC_CTRL_DMODE_NDMA		Hw19							// Nand Flash DMA Mode Selection ( NDMA )
#define	HwNFC_CTRL_DMODE_GDMA		~Hw19							// Nand Flash DMA Mode Selection ( GDMA )
#define	HwNFC_CTRL_BMODE_BURST		Hw18							// Nand Flash Burst Operation Mode Selection For NDMA( Burst Mode[User Data + ECC] )
#define	HwNFC_CTRL_BMODE_NOR		~Hw18							// Nand Flash Burst Operation Mode Selection For NDMA( Normal Mode[User Data])
#define	HwNFC_CTRL_ECCON_EN			Hw17							// Nand Flash ECC Encoding/Decoding Enable
#define	HwNFC_CTRL_ECCON_DIS		~Hw17							// Nand Flash ECC Encoding/Decoding Disable
#define	HwNFC_CTRL_PRSEL_P			Hw16							// Nand Flash Program Mode Selection for Burst Transfer
#define	HwNFC_CTRL_PRSEL_R			~Hw16							// Nand Flash Read Mode Selection for Burst Transfer
#define	HwNFC_CTRL_DNUM(X)			((X)*Hw14)
#define	HwNFC_CTRL_DNUM_0			HwNFC_CTRL_DNUM(0)				// Nand Flash Multiplee Data Configuration Number
#define	HwNFC_CTRL_DNUM_1			HwNFC_CTRL_DNUM(1)				// Nand Flash Multiplee Data Configuration Number
#define	HwNFC_CTRL_DNUM_2			HwNFC_CTRL_DNUM(2)				// Nand Flash Multiplee Data Configuration Number

#define	HwNFC_CTRL_RDY0SEL			Hw12							// NAND Flash Extend Ready Input Selection
#define	HwNFC_CTRL_RDY3TO0SEL		~Hw12							// Address/Command Mask Enable
#define	HwNFC_CTRL_RDY_MSK_EN		Hw11							// NAND Flahs Ready Mask Enable(Mask)
#define	HwNFC_CTRL_RDY_MSK_DIS		~Hw11							// NAND Flahs Ready Mask DIsable(UnMask)
#define	HwNFC_CTRL_MASK_EN			Hw10							// Address/Command Mask Enable
#define	HwNFC_CTRL_MASK_DIS			~Hw10							// Address/Command Mask Enable

#define	HwNFC_CTRL_BW(X)			((X)*Hw8)
#define	HwNFC_CTRL_BW_8				HwNFC_CTRL_BW(0)				// 1Read/Write
#define	HwNFC_CTRL_BW_16			HwNFC_CTRL_BW(1)				// 2Read/Write
#define	HwNFC_CTRL_BW_32			HwNFC_CTRL_BW(2)				// 4Read/Write

#define	HwNFC_CTRL_BSIZE(X)			((X)*Hw6)
#define	HwNFC_CTRL_BSIZE_1			HwNFC_CTRL_BSIZE(0)				// 1Read/Write
#define	HwNFC_CTRL_BSIZE_2			HwNFC_CTRL_BSIZE(1)				// 2Read/Write
#define	HwNFC_CTRL_BSIZE_4			HwNFC_CTRL_BSIZE(2)				// 4Read/Write
#define	HwNFC_CTRL_BSIZE_8			HwNFC_CTRL_BSIZE(3)				// 8Read/Write

#define	HwNFC_CTRL_PSIZE(X)			((X)*Hw3)
#define	HwNFC_CTRL_PSIZE_256		HwNFC_CTRL_PSIZE(0)				// 1 Page = 256 Half-Word
#define	HwNFC_CTRL_PSIZE_512		HwNFC_CTRL_PSIZE(1)				// 1 Page = 512 Byte
#define	HwNFC_CTRL_PSIZE_1024		HwNFC_CTRL_PSIZE(2)				// 1 Page = 1024 Half-Word
#define	HwNFC_CTRL_PSIZE_2048		HwNFC_CTRL_PSIZE(3)				// 1 Page = 2048 Byte
#define	HwNFC_CTRL_PSIZE_4096		HwNFC_CTRL_PSIZE(4)				// 1 Page = 4096 Byte
#define	HwNFC_CTRL_PSIZE_MASK		HwNFC_CTRL_PSIZE(7)
#define	HwNFC_CTRL_CADDR			Hw0								// Number of Address Cycle

/*******************************************************************************
* NFC IRQCFC														(0xF05B0064)
*******************************************************************************/
#define HwNFC_IRQCFG_RDYIEN         Hw31							//NAND Flash Ready Interrupt Enable
#define HwNFC_IRQCFG_PIEN			Hw30							//NAND Flash Program Interrupt Enable
#define HwNFC_IRQCFG_RDIEN			Hw29							//NAND Flash Read Interrupt Enable

#define HwNFC_IRQCFG_SPWIEN			Hw27							//NAND Flash Spare Area Burst Write Interrupt
#define HwNFC_IRQCFG_SPRIEN			Hw26							//NAND Flash Spare Area Burst Read Interrupt

#define HwNFC_IRQCFG_MEIEN			Hw26							//MLC ECC Decoding Interrupt Enable

/*******************************************************************************
* NFC IRQ															(0xF05B0068)
*******************************************************************************/
#define HwNFC_IRQ_RDYIRQ         	Hw31
#define HwNFC_IRQ_PIRQ				Hw30
#define HwNFC_IRQ_RDIRQ				Hw29
#define HwNFC_IRQ_MEDIRQ			Hw23
#define HwNFC_IRQ_RDYFG				Hw15
#define HwNFC_IRQ_PFG				Hw14
#define HwNFC_IRQ_RDFG				Hw13

#define	HwECC_IREQ_MEDFG			Hw7								// MLC ECC Decoding Flag Register
#define	HwECC_IREQ_MEEFG			Hw6								// MLC ECC Encoding Flag Register
#define	HwECC_IREQ_SEDFG			Hw5								// SLC ECC Decoding Flag Register
#define	HwECC_IREQ_SEEFG			Hw4								// SLC ECC Encoding Flag Register
#define	HwECC_IREQ_ERRFG			Hw3								// MLC/SLC Error Flag

/*******************************************************************************
* NFC STATUS														(0xF05B006C)
*******************************************************************************/
#define HwNFC_STATUS_RDY_RDY		Hw31							// NAND Flash Ready Flag Status
#define	HwNFC_CTRL_RDY_BUSY			~Hw31							// External Nand Flash Controller is Busy
#define	HwNFC_CTRL_FS_RDY			Hw30							// FIFO status is Ready to write and read in FIFO
#define	HwNFC_CTRL_FS_BUSY			~Hw30							// FIFO status is Busy to write and read in FIFO

/*******************************************************************************
* ECC CONTROL REGISTER
*******************************************************************************/
#define HwECC_BASE								(0xF05B0200)

#define HwECC_CTRL_DSIZE_CLEAR		(Hw26|Hw25|Hw24|Hw23|Hw22|Hw21|Hw20|Hw19|Hw18|Hw17|Hw16)

#define HwECC_CTRL_CORPASS_EN		Hw5								// MLC Correction Pass Enable
#define HwECC_CTRL_CORPASS_DIS		~Hw5							// MLC Correction Pass Disable
#define HwECC_CTRL_ECCMODE_NDM_EN	Hw4								// ECC Mode Selection( 0: Nor 1: NFC Dedicated Memory)
#define HwECC_CTRL_ECCMODE_NDM_DIS	~Hw4							// ECC Mode Selection( 0: Nor 1: NFC Dedicated Memory)

#define	HwECC_CTRL_EN_SLCEN			Hw2								// SLC ECC Encoding Enable
#define	HwECC_CTRL_EN_SLCDE			(Hw2|Hw0)						// SLC ECC Decoding Enable
#define	HwECC_CTRL_EN_MCL4EN		(Hw2|Hw1)						// MLC ECC4 Encoding Enable
#define	HwECC_CTRL_EN_MCL4DE		(Hw2|Hw1|Hw0)					// MLC ECC4 Decoding Enable
#define	HwECC_CTRL_EN_MCL6EN		(Hw3)							// MLC ECC6 Encoding Enable
#define	HwECC_CTRL_EN_MCL6DE		(Hw3|Hw0)						// MLC ECC6 Decoding Enable
#define	HwECC_CTRL_EN_MCL12EN		(Hw3|Hw1)						// MLC ECC12 Encoding Enable
#define	HwECC_CTRL_EN_MCL12DE		(Hw3|Hw1|Hw0)					// MLC ECC12 Decoding Enable
#define	HwECC_CTRL_EN_MCL16EN		(Hw3|Hw2)						// MLC ECC16 Encoding Enable
#define	HwECC_CTRL_EN_MCL16DE		(Hw3|Hw2|Hw0)					// MLC ECC16 Decoding Enable
#define	HwECC_CTRL_EN_MCL24EN		(Hw3|Hw2|Hw1)					// MLC ECC24 Encoding Enable
#define	HwECC_CTRL_EN_MCL24DE		(Hw3|Hw2|Hw1|Hw0)				// MLC ECC24 Decoding Enable
#define	HwECC_CTRL_EN_DIS			~(Hw3|Hw2|Hw1|Hw0)				// ECC Disable

/*******************************************************************************
* ECC Error Status Register
*******************************************************************************/
#define HwNFCECCERR_BASE						 (0xF05B0260)

#define	HwERR_NUM_ERR1				Hw0								// 1 Error Occurred(MLC4)
#define	HwERR_NUM_ERR2				Hw1								// 2 Error Occurred(MLC4)
#define	HwERR_NUM_ERR3				(Hw1|Hw0)						// 3 Error Occurred(MLC4)
#define	HwERR_NUM_ERR4				Hw2								// 4 Error Occurred(MLC4)
#define	HwERR_NUM_ERR5				(Hw2|Hw0)						// 5 Error Occurred(MLC4)
#define	HwERR_NUM_ERR6				(Hw2|Hw1)						// 6 Error Occurred(MLC4)
#define	HwERR_NUM_ERR7				(Hw2|Hw1|Hw0)					// 7 Error Occurred(MLC4)
#define	HwERR_NUM_ERR8				Hw3								// 8 Error Occurred(MLC4)
#define	HwERR_NUM_ERR9				(Hw3|Hw0)						// 9 Error Occurred(MLC4)
#define	HwERR_NUM_ERR10				(Hw3|Hw1)						// 10 Error Occurred(MLC4)
#define	HwERR_NUM_ERR11				(Hw3|Hw1|Hw0)					// 11 Error Occurred(MLC4)
#define	HwERR_NUM_ERR12				(Hw3|Hw2)						// 12 Error Occurred(MLC4)
#define	HwERR_NUM_ERR13				(Hw3|Hw2|Hw0)					// 13 Error Occurred(MLC4)
#define	HwERR_NUM_ERR14				(Hw3|Hw2|Hw1)					// 14 Error Occurred(MLC4)
#define	HwERR_NUM_ERR15				(Hw3|Hw2|Hw1|Hw0)				// 15 Error Occurred(MLC4)
#define	HwERR_NUM_ERR16				Hw4								// 16 Error Occurred(MLC4)
#define	HwERR_NUM_ERR17				(Hw4|Hw0)						// 17 Error Occurred(MLC4)
#define	HwERR_NUM_ERR18				(Hw4|Hw1)						// 18 Error Occurred(MLC4)
#define	HwERR_NUM_ERR19				(Hw4|Hw1|Hw0)					// 19 Error Occurred(MLC4)
#define	HwERR_NUM_ERR20				(Hw4|Hw2)						// 20 Error Occurred(MLC4)
#define	HwERR_NUM_ERR21				(Hw4|Hw2|Hw0)					// 21 Error Occurred(MLC4)
#define	HwERR_NUM_ERR22				(Hw4|Hw2|Hw1)					// 22 Error Occurred(MLC4)
#define	HwERR_NUM_ERR23				(Hw4|Hw2|Hw1|Hw0)				// 23 Error Occurred(MLC4)
#define	HwERR_NUM_ERR24				(Hw4|Hw3)						// 24 Error Occurred(MLC4)
#define	HwERR_NUM_CORIMP			(Hw4|Hw3|Hw0)					// Correction Impossible Error

#define	HwERR_NUM_NOERR				HwZERO							// No Error

/*******************************************************************************
*    13. SMC(Static Memory Controller) Register Define (Base Addr = 0xF05F0000)
********************************************************************************/
#define HwSMC_BASE								(0xF05F0000)
#define HwSMC_CSNCFG0							(0xF05F0020)

/*******************************************************************************
*    14. EDI(External Device Interface) Register Define (Base Addr = 0xF05F6000)
********************************************************************************/
#define HwEDI_BASE								(0xF05F6000)

/*******************************************************************************
*    15. IDE Register Define (Base Addr = 0xF0520000)
********************************************************************************/
#define HwIDE_BASE								(0xF0520000)

/*******************************************************************************
*    16. USB 1.1 Host Controller & Transceiver Register Define (Base Addr = 0xF0500000)
********************************************************************************/
#define HwUSBHOST_BASE							(0xF0500000)

/*******************************************************************************
*    17. USB 2.0 OTG Controller Register Define (Base Addr = 0xF0550000)
********************************************************************************/
#define HwUSB20OTG_BASE							(0xF0550000)
#define HwUSBPHYCFG_BASE						(0xF05F5028)

/*******************************************************************************
*    18. I2C Controller Register Define (Base Addr = 0xF0550000)
********************************************************************************/
#define HwI2C_CORE0_BASE						(0xF0530000)
#define HwI2C_CORE0_MASTER0_BASE				(0xF0530000)
#define HwI2C_CORE0_MASTER1_BASE				(0xF0530040)
#define HwI2C_CORE0_SLAVE_BASE					(0xF0530080)
#define HwI2C_CORE0_INT_BASE					(0xF05300C0)

#define HwI2C_CORE1_BASE						(0xF0530100)
#define HwI2C_CORE1_MASTER0_BASE				(0xF0530100)
#define HwI2C_CORE1_MASTER1_BASE				(0xF0530140)
#define HwI2C_CORE1_SLAVE_BASE					(0xF0530180)
#define HwI2C_CORE1_INT_BASE					(0xF05301C0)

#define HwI2CIRQ_BASE							(0xF0530300)
#define HwI2CDREQSEL_BASE						(0xF0530400)

/*******************************************************************************
*    19. PWM(Pulse Width Modulation) Register Define (Base Addr = 0xF05D0000)
********************************************************************************/
#define HwPWM_BASE								(0xF05D0000)
#define HwPDM_BASE								(0xF05D0000)

/*******************************************************************************
*    20. Remote Control Interface Register Define (Base Addr = 0xF05F3000)
********************************************************************************/
#define HwREMOTE_BASE							(0xF05F3000)

/*******************************************************************************
*    21. TSADC Interface Register Define (Base Addr = 0xF05F4000)
********************************************************************************/
#define HwTSADC_BASE							(0xF05F4000)

/*******************************************************************************
*    22. TSIF(Transport Stream Interface) Register Define (Base Addr = 0x)
********************************************************************************/
#define HwTSIF_BASE								(0xF0539000)
#define HwTSIF0_BASE							(0xF0539000)
#define HwTSIF1_BASE							(0xF0539100)
#define HwTSIF_PORTSEL_BASE						(0xF0539800)

/*******************************************************************************
*    23. UART Register Define (Base Addr = 0xF0531000)
********************************************************************************/
#define HwUART_BASE								(0xF0531000)
#define HwUART0_BASE							(0xF0531000)
#define HwUART1_BASE							(0xF0531100)
#define HwUART2_BASE							(0xF0531200)
#define HwUART3_BASE							(0xF0531300)
#define HwUART4_BASE							(0xF0531400)
#define HwUART5_BASE							(0xF0531500)
#define HwUART_PORTMUX_BASE						(0xF0531600)

#define HwUART_CH_BASE(X)				(0xF0531000+(X)*0x100)

#define HwUART_CHSEL					(0xF0531600)	// R/W, Channel Selection Register
#define HwUART_CHSEL_MASK(X)			(7<<((X)*4))							// X: Channel Number
#define HwUART_CHSEL_SEL_PORT(X,Y)		((Y)<<((X)*4))							// X: Channel Number, Y: Port Numbert

/*******************************************************************************
*    24. MPEFEC Register Define (Base Addr = 0xF0510000)
********************************************************************************/
#define HwMPEFEC_BASE							(0xF0510000)

/*******************************************************************************
*    25. RTC(Real-Time Clock) Register Define (Base Addr = 0xF05F2000)
********************************************************************************/
#define HwRTC_BASE								(0xF05F2000)

/*******************************************************************************
*    26. IOBUS Configuration Register Define (Base Addr = 0xF05F5000)
********************************************************************************/
#define HwIOBUSCFG_BASE							(0xF05F5000)


// IOBUS AHB 0
#define HwIOBUSCFG_USB				Hw1											// USB2.0 OTG
#define HwIOBUSCFG_I2C				Hw7											// I2C Controller
#define HwIOBUSCFG_NFC				Hw8											// NFC Controller
#define HwIOBUSCFG_EHI0				Hw9											// External Host Interface 0
#define HwIOBUSCFG_EHI1				Hw10										// External Host Interface 1
#define HwIOBUSCFG_UART0			Hw11										// UART Controller 0
#define HwIOBUSCFG_UART1			Hw12										// UART Controller 1
#define HwIOBUSCFG_UART2			Hw13										// UART Controller 2
#define HwIOBUSCFG_UART3			Hw14										// UART Controller 3
#define HwIOBUSCFG_UART4			Hw15										// UART Controller 4
#define HwIOBUSCFG_UART5			Hw16										// UART Controller 5
#define HwIOBUSCFG_GPSB0			Hw17										// GPSB Controller 0
#define HwIOBUSCFG_GPSB1			Hw18										// GPSB Controller 1
#define HwIOBUSCFG_GPSB2			Hw19										// GPSB Controller 2
#define HwIOBUSCFG_GPSB3			Hw20										// GPSB Controller 3
#define HwIOBUSCFG_GPSB4			Hw21										// GPSB Controller 4
#define HwIOBUSCFG_GPSB5			Hw22										// GPSB Controller 5
#define HwIOBUSCFG_DAI				Hw23										// DAI/CDIF Interface


#define HwIOBUSCFG_SPDIF			Hw25										// SPDIF Tx Controller
#define HwIOBUSCFG_RTC				Hw26										// RTC
#define HwIOBUSCFG_TSADC			Hw27										// TSADC Controller

#define HwIOBUSCFG_ADMA				Hw31										// Audio DMA Controller




/*******************************************************************************
*
*    TCC88x DataSheet PART 6 HSIO BUS
*
********************************************************************************/

/************************************************************************
*   4. PCIe(PCI Expresss)  (Base Addr = 0xF0800000) // R/W
*************************************************************************/
#define HwPCIE_BASE								(0xF0800000)
#define HwPCIE_DBI_BASE							(0xF0810000)

/************************************************************************
*   5. SATA Host 0/1 (Base Addr = 0xF0820000, 0xF0850000) // R/W
*************************************************************************/
#define HwSATAHOST0_BASE						(0xF0820000)
#define HwSATAHOST1_BASE						(0xF0850000)

/************************************************************************
*   6. SATA Device (Base Addr = 0xF0860000) // R/W
*************************************************************************/
#define HwSATADEVICE_BASE						(0xF0860000)

/************************************************************************
*   7. SATA(PCIe) PHY 0/1 (Base Addr = 0xF0830000, 0xF0870000) // R/W
*************************************************************************/
#define HwSATAPHY0_BASE							(0xF0830000)
#define HwSATAPHY1_BASE							(0xF0870000)

/************************************************************************
*   8. Ethernet(GMAC) (Base Addr = 0xF0840000) // R/W
*************************************************************************/
#define HwGMAC_BASE								(0xF0840000)

/************************************************************************
*   9. USB 2.0 Host (Base Addr = 0xF0880000, 0xF0890000) // R/W
*************************************************************************/
#define HwUSB20HOST_EHCI_BASE					(0xF0880000)
#define HwUSB20HOST_OHCI_BASE					(0xF0890000)

/************************************************************************
*   10. Cipher (Base Addr = 0xF08A2000) // R/W
*************************************************************************/
#define HwCIPHER_BASE							(0xF08A2000)

/************************************************************************
*   11. DMAX Controller (Base Addr = 0xF08A3000) // R/W
*************************************************************************/
#define HwDMAX_BASE								(0xF08A3000)

/************************************************************************
*   12. TS-Demux (Base Addr = 0xF08B0000) // R/W
*************************************************************************/
#define HwTSDEMUX_BASE							(0xF08B0000)

/************************************************************************
*   13. HSDMA Controller  (Base Addr = 0xF08C0000) // R/W
*************************************************************************/
#define HwHSDMA_BASE							(0xF08C0000)

/************************************************************************
*   14. HSIO BUS Configuration (Base Addr = 0xF08A0000) // R/W
*************************************************************************/
#define HwHSIOBUSCFG_BASE						(0xF08A0000)
#define HwHSIOBUSMATRIX_BASE					(0xF08A1000)


/*******************************************************************************
*
*    TCC88x DataSheet PART 7 DISPLAY BUS
*
********************************************************************************/

/************************************************************************
*   4. LCD Controller (Base Addr = 0xF0200000) // R/W
*************************************************************************/
#define HwLCDC_BASE								(0xF0200000)
#define HwLCDC0_BASE							(0xF0200000)
#define HwLCDLUT0_BASE							(0xF0200400)
#define HwLCDC1_BASE							(0xF0204000)
#define HwLCDLUT1_BASE							(0xF0204400)
#define HwLCDC0_CH_BASE(X)						(HwLCDC_BASE + 0x80 + (0x38 * X))
#define HwLCDC1_CH_BASE(X)						(HwLCDC1_BASE + 0x80 + (0x38 * X))

// LCD Control
#define	HwLCTRL_EVP					Hw31										// External Vsync Polarity
#define	HwLCTRL_EVS					Hw30										// External Vsync Enable
#define	HwLCTRL_R2YMD				(Hw29+Hw28+Hw27+Hw26)						// RGB to YCbCr Conversion Option
#define	HwLCTRL_GEN					Hw25										// Gamma Correction Enable Bit
#define	HwLCTRL_656					Hw24										// CCIR 656 Mode
#define	HwLCTRL_CKG					Hw23										// Clock Gating Enable for Timing Generator
#define	HwLCTRL_BPP					(Hw22+Hw21+Hw20)							// Bit Per Pixel for STN_LCD
#define	HwLCTRL_PXDW				(Hw19+Hw18+Hw17+Hw16)						// PXDW
#define	HwLCTRL_ID					Hw15										// Inverted Data Enable
#define	HwLCTRL_IV					Hw14										// Inverted Vertical Sync
#define	HwLCTRL_IH					Hw13										// Inverted Horizontal Sync
#define	HwLCTRL_IP					Hw12										// Inverted Pixel Clock
#define	HwLCTRL_CLEN				Hw11										// Clipping Enable
#define	HwLCTRL_DP					Hw10										// Double Pixel Data
#define	HwLCTRL_R2Y					Hw9											// RGB to YCbCr Converter Enable for Output
#define	HwLCTRL_NI					Hw8											// Non-Interlace
#define	HwLCTRL_TV					Hw7											// TV mode
#define	HwLCTRL_TFT					Hw6											// TFT LCD mode
#define	HwLCTRL_STN					Hw5											// STN LCD mode
#define	HwLCTRL_MSEL				Hw4											// Master Select for Image 0
#define	HwLCTRL_OVP					(Hw3+Hw2+Hw1)								// Overlay priority
#define	HwLCTRL_LEN					Hw0											// LCD Controller Enable

// LCD Clock Divider
#define	HwLCLKDIV_CS				Hw31										// Clock Source
#define	HwLCLKDIV_ACDIV				Hw16										// AC bias clock divisor (STN only)
#define	HwLCLKDIV_LCLKDIV			Hw8											// LCLK clocks divider
#define	HwLCLKDIV_PXCLKDIV			(Hw8 - Hw0)											// Pixel Clock Divider

// LCD Vertical Timing 1
#define	HwLVTIME1_VDB				Hw27										// Back porchVSYNC delay
#define	HwLVTIME1_VDF				Hw22										// Front porch of VSYNC delay
#define	HwLVTIME1_FPW				Hw16										// Frame pulse width
#define	HwLVTIME1_FLC				Hw0											// Frame line count

// LCD Status
#define	HwLSTATUS_VS				Hw31										// Monitoring vertical sync
#define	HwLSTATUS_BUSY				Hw30										// Busy signal
#define	HwLSTATUS_EF				Hw29										// Even-Field(Read Only). 0:Odd field or frame, 1:Even field or frame
#define	HwLSTATUS_DEOF				Hw28										// DMA End of Frame flag
#define	HwLSTATUS_I0EOF				Hw27										// Image 0 End of Frame flag
#define	HwLSTATUS_I1EOF				Hw26										// Image 1 End of Frame flag
#define	HwLSTATUS_I2EOF				Hw25										// Image 2 End of Frame flag
#define	HwLSTATUS_IE2F				Hw12										// Image 2 end-of-frame falling edge flag
#define	HwLSTATUS_IE2R				Hw11										// Image 2 end-of-frame rising edge flag
#define	HwLSTATUS_IE1F				Hw10										// Image 1 end-of-frame falling edge flag
#define	HwLSTATUS_IE1R				Hw9											// Image 1 end-of-frame rising edge flag
#define	HwLSTATUS_IE0F				Hw8											// Image 0 end-of-frame falling edge flag
#define	HwLSTATUS_IE0R				Hw7											// Image 0 end-of-frame rising edge flag
#define	HwLSTATUS_DEF				Hw6											// DMA end-of-frame falling edge flag
#define	HwLSTATUS_DER				Hw5											// DMA end-of-frame rising edge flag
#define	HwLSTATUS_DD				Hw4											// Disable done
#define	HwLSTATUS_RU				Hw3											// Register update flag
#define	HwLSTATUS_VSF				Hw2											// VS falling flag
#define	HwLSTATUS_VSR				Hw1											// VS rising flag
#define	HwLSTATUS_FU				Hw0											// LCD output fifo under-run flag.

// LCD Interrupt
#define	HwLIM_FU					Hw0											//LCD output fifo under-run interrupt mask
#define	HwLIM_VSR					Hw1											// VS rising interrupt mask
#define	HwLIM_VSF					Hw2											// VS falling interrupt mask
#define	HwLIM_RU					Hw3											// Register update interrupt mask
#define	HwLIM_DD					Hw4											// Disable done interrupt mask
#define	HwLIM_DER					Hw5											// DMA end-of-frame rising edge interrupt mask
#define	HwLIM_DEF					Hw6											// DMA end-of-frame falling edge interrupt mask
#define	HwLIM_IE0R					Hw7											// Image 0 end-of-frame rising edge interrupt mask
#define	HwLIM_IE0F					Hw8											// Image 0 end-of-frame falling edge interrupt mask
#define	HwLIM_IE1R					Hw9											// Image 1 end-of-frame rising edge interrupt mask
#define	HwLIM_IE1F					Hw10										// Image 1 end-of-frame falling edge interrupt mask
#define	HwLIM_IE2R					Hw11										// Image 2 end-of-frame rising edge interrupt mask
#define	HwLIM_IE2F					Hw12										// Image 2 end-of-frame falling edge interrupt mask

// LCD Image Control
#define	HwLIC_INTL					Hw31										// DMA interlace type
#define	HwLIC_AEN					Hw30										// AlphaBlending Function for Each Image
#define	HwLIC_CEN					Hw29										// Chroma-Key Function for Each Image
#define	HwLIC_IEN					Hw28										// Image Display Function for Each Image
#define	HwLIC_SRC					Hw27										// Image Source Select
#define	HwLIC_AOPT					(Hw26+Hw25)									// Alpha -blending Option Selection Bits
#define	HwLIC_ASEL					Hw24										// Image Displaying Function for Each Image
#define	HwLCT_RU					Hw16										// data update enable
#define	HwLIC_PD					Hw15										// Bit padding
#define	HwLIC_Y2RMD					(Hw10+Hw9)									// YCbCr to RGB Conversion Option
#define	HwLIC_Y2R					Hw8											// YCbCr to RGB Conversion Enable Bit
#define	HwLIC_BR					Hw7											// Bit Reverse
#define	HwLIC_FMT					(Hw4+Hw3+Hw2+Hw1+Hw0)						// Image Format


// LCD Image Scale Ratio
#define	HwLISCALE_Y					(Hw7+Hw6+Hw5+Hw4)							// Y DownScale
#define	HwLISCALE_X					(Hw3+Hw2+Hw1+Hw0)							// X DownScale

// LCD Alpha Information
#define	HwLIA_A0					(Hw24-Hw16)									// Alpha Value 0
#define	HwLIA_A1					(Hw8-Hw0)									// Alpha Value 1

// LCD Chroma-key
#define	HwLICKEY					(Hw8-Hw0)									// Chroma key
#define	HwLICKEYMASK				(Hw24-Hw16)									// Chroma key mask
/************************************************************************
*   5. LCD System Interface (Base Addr = ) // R/W
*************************************************************************/
#define HwLCDSI_BASE							(0xF020C000)


/************************************************************************
*   6. Memory To Memory Scaler (Base Addr = 0xF0210000) // R/W
*************************************************************************/
#define HwM2MSCALER_BASE						(0xF0210000)
#define HwM2MSCALER0_BASE						(0xF0210000)
#define HwM2MSCALER1_BASE						(0xF0220000)

#define HwSCALERPOSTFILTER_BASE					(0xF025B000)



#define HwM2MSCALER_CFG0			0xF0210014
#define HwM2MSCALER_CFG1			0xF0220014

//-------------------------------------------- Scaler0 --------------------------------------------
// Scaler source image Y base address
#define	HwMSC_SRC_Y_BASE_ADDR		0xFFFFFFFC									// HwMSC_SRC_Y_BASE_ADDR [31: 2]		// Scaler source image Y base
// Scaler source image U base address
#define	HwMSC_SRC_U_BASE_ADDR		0xFFFFFFFC									// HwMSC_SRC_U_BASE_ADDR [31: 2]		// Scaler source image U base
// Scaler source image V base address
#define	HwMSC_SRC_V_BASE_ADDR		0xFFFFFFFC									// HwMSC_SRC_V_BASE_ADDR [31: 2]		// Scaler source image V base

// Source image size
#define	HwMSC_SRC_SIZE_H			0x00000FFF									// HwMSC_SRC_SIZE_H [11: 0]		// Input image Horizontal size by pixel
#define	HwMSC_SRC_SIZE_V			0x0FFF0000									// HwMSC_SRC_SIZE_V [27: 16]		// Input image Vertical size by pixel

// Source image line offset
#define	HwMSC_SRC_OFFSET_Y			0x00000FFF									// HwMSC_SRC_OFFSET_Y [11: 0]		// Input image line offset of luminace
#define	HwMSC_SRC_OFFSET_C			0x0FFF0000									// HwMSC_SRC_OFFSET_C [27: 16]		// Input image line offset of chrominance

// Source image Configuration
#define   HwMSC_SRC_CFG_MSBF			Hw31
#define   HwMSC_SRC_CFG_OMODE			Hw19
#define   HwMSC_SRC_CFG_IMODE			Hw17
#define   HwMSC_SRC_CFG_ARGB			Hw16
#define	HwMSC_SRC_CFG_INTM			Hw5											// interleaved mode cb/cr order	[5]
#define   HwMSC_SRC_CFG_INPATH			Hw4
#define	HwMSC_SRC_CFG_INTLV			Hw3											// interleaved type [3]
#define	HwMSC_SRC_CFG_YUV422_SEQ0	HwZERO										// YUV422 SEQuential order 0
#define	HwMSC_SRC_CFG_YUV422_SEQ1	Hw0											// YUV422 SEQuential order 1
#define	HwMSC_SRC_CFG_YUV422_SEP	Hw1											// YUV422 SEPERATED
#define	HwMSC_SRC_CFG_YUV420_SEP	(Hw1|Hw0)									// YUV420 SEPERATED
#define	HwMSC_SRC_CFG_RGB565		Hw2											// YUV422 SEPERATED
#define	HwMSC_SRC_CFG_RGB555		(Hw2|Hw0)									// YUV422 SEPERATED
#define	HwMSC_SRC_CFG_RGB444		(Hw2|Hw1)									// YUV422 SEPERATED
#define	HwMSC_SRC_CFG_RGB454		(Hw2|Hw1|Hw0)
//#define	HwMSC_SRC_CFG_INVALID		(Hw2|Hw1|Hw0)								// INVALID

// Scaler destination image Y base address
#define	HwMSC_DST_Y_BASE_ADDR		0xFFFFFFFC									// HwMSC_DST_Y_BASE_ADDR [31: 2]		// Scaler destination image Y base
// Scaler destination image U base address
#define	HwMSC_DST_U_BASE_ADDR		0xFFFFFFFC									// HwMSC_DST_U_BASE_ADDR [31: 2]		// Scaler destination image U base
// Scaler destination image V base address
#define	HwMSC_DST_V_BASE_ADDR		0xFFFFFFFC									// HwMSC_DST_V_BASE_ADDR [31: 2]		// Scaler destination image V base

// Destination image size
#define	HwMSC_DST_SIZE_H			0x00000FFF									// HwMSC_DST_SIZE_H	[11: 0]		// Input image Horizontal size by pixel
#define	HwMSC_DST_SIZE_V			0x0FFF0000									// HwMSC_DST_SIZE_V	[27: 16]		// Input image Vertical size by pixel

// Destination image line offset
#define	HwMSC_DST_OFFSET_Y			0x00000FFF									// HwMSC_DST_OFFSET_Y [11: 0]		// Input image line offset of luminace
#define	HwMSC_DST_OFFSET_C			0x0FFF0000									// HwMSC_DST_OFFSET_C [27: 16]		// Input image line offset of chrominance

// Destination image Configuration
#define	HwMSC_DST_CFG_COP			Hw11										// COP	[11]
#define	HwMSC_DST_CFG_WAIT			0x00000700									// WAIT	[10:8]
#define	HwMSC_DST_CFG_RDY			Hw6											// READY [6]
#define	HwMSC_DST_CFG_INTM			Hw5											// interleaved mode cb/cr order	[5]
#define	HwMSC_DST_CFG_PATH			Hw4											// [4]	0 : to memory (Master mode, Scaler master writes result to memory)
#define	HwMSC_DST_CFG_INTLV			Hw3											// interleaved type [3]

// 070802_hjb		//	1 : LCD (Slave mode, LCD master reads scaling results)
#define	HwMSC_DST_CFG_YUV422_SEQ0	HwZERO										// YUV422 SEQuential order 0
#define	HwMSC_DST_CFG_YUV422_SEQ1	Hw0											// YUV422 SEQuential order 1
#define	HwMSC_DST_CFG_YUV422_SEP	Hw1											// YUV422 SEPERATED
#define	HwMSC_DST_CFG_YUV420_SEP	(Hw1|Hw0)									// YUV420 SEPERATED
#define	HwMSC_DST_CFG_RGB565		Hw2											// YUV422 SEPERATED
#define	HwMSC_DST_CFG_RGB555		(Hw2|Hw0)									// YUV422 SEPERATED
#define	HwMSC_DST_CFG_RGB444		(Hw2|Hw1)									// YUV422 SEPERATED
#define	HwMSC_DST_CFG_RGB454		(Hw2|Hw1|Hw0)
//#define	HwMSC_DST_CFG_INVALID		(Hw2|Hw1|Hw0)								// INVALID

// Scale ratio
#define	HwMSC_HRATIO				0x00003FFF									// HwMSC_HRATIO [29:16] Horizontal scale factor
#define	HwMSC_VRATIO				0x3FFF0000									// HwMSC_VRATIO [13:0]	Vertical scale factor

// Scaler control
#define	HwMSC_INPTH					Hw24										// Rolling Operation Mode Enable
#define	HwMSC_REN					Hw23										// Rolling Operation Mode Enable
#define	HwMSC_MEN					Hw22										// middle Operation Mode Enable
#define	HwMSC_RLS					Hw19										// Release Stop mode
#define	HwMSC_RGSM					Hw17										// Rolling Go Stop Mode
#define	HwMSC_MGSM					Hw16										// Middle Go Stop Mode
#define	HwMSC_RIRQEN				Hw7											// Rolling interrupt enable
#define	HwMSC_MIRQEN				Hw6											// Middle interrupt enable
#define	HwMSC_CTRL_CON_EN			Hw5
#define	HwMSC_CTRL_BUSY_EN			Hw2											// HwMSC_CTRL_BUSY_EN [2] Scaler BUSY interrupt enable
#define	HwMSC_CTRL_RDY_EN			Hw1											// HwMSC_CTRL_RDY_EN	[1] Scaler READY intertupt enable
#define	HwMSC_CTRL_EN				Hw0											// HwMSC_CTRL_EN	[0] Scaler enable

// Scaler status
#define	HwMSC_IR					Hw7											// Rolling interrupt flag
#define	HwMSC_IM					Hw6											// Middle interrupt flag
#define	HwMSC_STATUS_IBUSY			Hw5											// HwMSC_STATUS_IBUSY [5] BUSY INTERRUPT FLAG
#define	HwMSC_STATUS_IRDY			Hw4											// HwMSC_STATUS_IRDY	[4] READY INTERRUPT FLAG
#define	HwMSC_STATUS_BUSY			Hw1											// HwMSC_STATUS_BUSY  [1] BUSY Status FLAG
#define	HwMSC_STATUS_RDY			Hw0											// HwMSC_STATUS_RDY	[0] READY Status FLAG


#define	HwMSC_DST_ROLL_CNT			0x0FFF0000
#define	HwMSC_DST_MID_CNT			0x00000FFF

#define	HwMSC_C_R_CNT				0x0FFF0000



/************************************************************************
*   7. NTSC/PAL Encoder Composite Output (Base Addr = 0xF0240000) // R/W
*************************************************************************/
#define HwNTSCPAL_BASE							(0xF0240000)
#define HwTVE_BASE								(0xF0240000)
#define HwTVE_VEN_BASE							(0xF0240800)

// Encoder Mode Control A
#define	HwTVECMDA_PWDENC_PD			Hw7											// Power down mode for entire digital logic of TV encoder
#define	HwTVECMDA_FDRST_1			Hw6											// Chroma is free running as compared to H-sync
#define	HwTVECMDA_FDRST_0			HwZERO										// Relationship between color burst & H-sync is maintained for video standards
#define	HwTVECMDA_FSCSEL(X)			((X)*Hw4)
#define	HwTVECMDA_FSCSEL_NTSC		HwTVECMDA_FSCSEL(0)							// Color subcarrier frequency is 3.57954545 MHz for NTSC
#define	HwTVECMDA_FSCSEL_PALX		HwTVECMDA_FSCSEL(1)							// Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
#define	HwTVECMDA_FSCSEL_PALM		HwTVECMDA_FSCSEL(2)							// Color subcarrier frequency is 3.57561149 MHz for PAL-M
#define	HwTVECMDA_FSCSEL_PALCN		HwTVECMDA_FSCSEL(3)							// Color subcarrier frequency is 3.58205625 MHz for PAL-combination N
#define	HwTVECMDA_FSCSEL_MASK		HwTVECMDA_FSCSEL(3)
#define	HwTVECMDA_PEDESTAL			Hw3											// Video Output has a pedestal
#define	HwTVECMDA_NO_PEDESTAL		HwZERO										// Video Output has no pedestal
#define	HwTVECMDA_PIXEL_SQUARE		Hw2											// Input data is at square pixel rates.
#define	HwTVECMDA_PIXEL_601			HwZERO										// Input data is at 601 rates.
#define	HwTVECMDA_IFMT_625			Hw1											// Output data has 625 lines
#define	HwTVECMDA_IFMT_525			HwZERO										// Output data has 525 lines
#define	HwTVECMDA_PHALT_PAL			Hw0											// PAL encoded chroma signal output
#define	HwTVECMDA_PHALT_NTSC		HwZERO										// NTSC encoded chroma signal output

// Encoder Mode Control B
#define	HwTVECMDB_YBIBLK_BLACK		Hw4											// Video data is forced to Black level for Vertical non VBI processed lines.
#define	HwTVECMDB_YBIBLK_BYPASS		HwZERO										// Input data is passed through forn non VBI processed lines.
#define	HwTVECMDB_CBW(X)			((X)*Hw2)
#define	HwTVECMDB_CBW_LOW			HwTVECMDB_CBW(0)							// Low Chroma band-width
#define	HwTVECMDB_CBW_MEDIUM		HwTVECMDB_CBW(1)							// Medium Chroma band-width
#define	HwTVECMDB_CBW_HIGH			HwTVECMDB_CBW(2)							// High Chroma band-width
#define	HwTVECMDB_CBW_MASK			HwTVECMDB_CBW(3)							// 
#define	HwTVECMDB_YBW(X)			((X)*Hw0)
#define	HwTVECMDB_YBW_LOW			HwTVECMDB_YBW(0)							// Low Luma band-width
#define	HwTVECMDB_YBW_MEDIUM		HwTVECMDB_YBW(1)							// Medium Luma band-width
#define	HwTVECMDB_YBW_HIGH			HwTVECMDB_YBW(2)							// High Luma band-width
#define	HwTVECMDB_YBW_MASK			HwTVECMDB_YBW(3)							// 

// Encoder Clock Generator
#define	HwTVEGLK_XT24_24MHZ			Hw4											// 24MHz Clock input
#define	HwTVEGLK_XT24_27MHZ			HwZERO										// 27MHz Clock input
#define	HwTVEGLK_GLKEN_RST_EN		Hw3											// Reset Genlock
#define	HwTVEGLK_GLKEN_RST_DIS		~Hw3										// Release Genlock
#define	HwTVEGLK_GLKE(X)			((X)*Hw1)
#define	HwTVEGLK_GLKE_INT			HwTVEGLK_GLKE(0)							// Chroma Fsc is generated from internal constants based on current user setting
#define	HwTVEGLK_GLKE_RTCO			HwTVEGLK_GLKE(2)							// Chroma Fsc is adjusted based on external RTCO input
#define	HwTVEGLK_GLKE_CLKI			HwTVEGLK_GLKE(3)							// Chroma Fsc tracks non standard encoder clock (CLKI) frequency
#define	HwTVEGLK_GLKE_MASK			HwTVEGLK_GLKE(3)							//
#define	HwTVEGLK_GLKEN_GLKPL_HIGH	Hw0											// PAL ID polarity is active high
#define	HwTVEGLK_GLKEN_GLKPL_LOW	HwZERO										// PAL ID polarity is active low

// Encoder Mode Control C
#define	HwTVECMDC_CSMDE_EN			Hw7											// Composite Sync mode enabled
#define	HwTVECMDC_CSMDE_DIS			~Hw7										// Composite Sync mode disabled (pin is tri-stated)
#define	HwTVECMDC_CSMD(X)			((X)*Hw5)
#define	HwTVECMDC_CSMD_CSYNC		HwTVECMDC_CSMD(0)							// CSYN pin is Composite sync signal
#define	HwTVECMDC_CSMD_KEYCLAMP		HwTVECMDC_CSMD(1)							// CSYN pin is Keyed clamp signal
#define	HwTVECMDC_CSMD_KEYPULSE		HwTVECMDC_CSMD(2)							// CSYN pin is Keyed pulse signal
#define	HwTVECMDC_CSMD_MASK			HwTVECMDC_CSMD(3)
#define	HwTVECMDC_RGBSYNC(X)		((X)*Hw3)
#define	HwTVECMDC_RGBSYNC_NOSYNC	HwTVECMDC_RGBSYNC(0)						// Disable RGBSYNC (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_RGB		HwTVECMDC_RGBSYNC(1)						// Sync on RGB output signal (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_G			HwTVECMDC_RGBSYNC(2)						// Sync on G output signal (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_MASK		HwTVECMDC_RGBSYNC(3)

// DAC Output Selection
#define	HwTVEDACSEL_DACSEL_CODE0	HwZERO										// Data output is diabled (output is code '0')
#define	HwTVEDACSEL_DACSEL_CVBS		Hw0											// Data output in CVBS format

// DAC Power Down
#define	HwTVEDACPD_PD_EN			Hw0											// DAC Power Down Enabled
#define	HwTVEDACPD_PD_DIS			~Hw0										// DAC Power Down Disabled

// Sync Control
#define	HwTVEICNTL_FSIP_ODDHIGH		Hw7											// Odd field active high
#define	HwTVEICNTL_FSIP_ODDLOW		HwZERO										// Odd field active low
#define	HwTVEICNTL_VSIP_HIGH		Hw6											// V-sync active high
#define	HwTVEICNTL_VSIP_LOW			HwZERO										// V-sync active low
#define	HwTVEICNTL_HSIP_HIGH		Hw5											// H-sync active high
#define	HwTVEICNTL_HSIP_LOW			HwZERO										// H-sync active low
#define	HwTVEICNTL_HSVSP_RISING		Hw4											// H/V-sync latch enabled at rising edge
#define	HwTVEICNTL_HVVSP_FALLING	HwZERO										// H/V-sync latch enabled at falling edge
#define	HwTVEICNTL_VSMD_START		Hw3											// Even/Odd field H/V sync output are aligned to video line start
#define	HwTVEICNTL_VSMD_MID			HwZERO										// Even field H/V sync output are aligned to video line midpoint
#define	HwTVEICNTL_ISYNC(X)			((X)*Hw0)
#define	HwTVEICNTL_ISYNC_FSI		HwTVEICNTL_ISYNC(0)							// Alignment input format from FSI pin
#define	HwTVEICNTL_ISYNC_HVFSI		HwTVEICNTL_ISYNC(1)							// Alignment input format from HSI,VSI,FSI pin
#define	HwTVEICNTL_ISYNC_HVSI		HwTVEICNTL_ISYNC(2)							// Alignment input format from HSI,VSI pin
#define	HwTVEICNTL_ISYNC_VFSI		HwTVEICNTL_ISYNC(3)							// Alignment input format from VSI,FSI pin
#define	HwTVEICNTL_ISYNC_VSI		HwTVEICNTL_ISYNC(4)							// Alignment input format from VSI pin
#define	HwTVEICNTL_ISYNC_ESAV_L		HwTVEICNTL_ISYNC(5)							// Alignment input format from EAV,SAV codes (line by line)
#define	HwTVEICNTL_ISYNC_ESAV_F		HwTVEICNTL_ISYNC(6)							// Alignment input format from EAV,SAV codes (frame by frame)
#define	HwTVEICNTL_ISYNC_FREE		HwTVEICNTL_ISYNC(7)							// Alignment is free running (Master mode)
#define	HwTVEICNTL_ISYNC_MASK		HwTVEICNTL_ISYNC(7)

// Offset Control
#define	HwTVEHVOFFST_INSEL(X)		((X)*Hw6)
#define	HwTVEHVOFFST_INSEL_BW16_27MHZ		HwTVEHVOFFST_INSEL(0)				// 16bit YUV 4:2:2 sampled at 27MHz
#define	HwTVEHVOFFST_INSEL_BW16_13P5MH		HwTVEHVOFFST_INSEL(1)				// 16bit YUV 4:2:2 sampled at 13.5MHz
#define	HwTVEHVOFFST_INSEL_BW8_13P5MHZ		HwTVEHVOFFST_INSEL(2)				// 8bit YUV 4:2:2 sampled at 13.5MHz
#define	HwTVEHVOFFST_INSEL_MASK		HwTVEHVOFFST_INSEL(3)
#define	HwTVEHVOFFST_VOFFST_256		Hw3											// Vertical offset bit 8 (Refer to HwTVEVOFFST)
#define	HwTVEHVOFFST_HOFFST_1024	Hw2											// Horizontal offset bit 10 (Refer to HwTVEHOFFST)
#define	HwTVEHVOFFST_HOFFST_512		Hw1											// Horizontal offset bit 9 (Refer to HwTVEHOFFST)
#define	HwTVEHVOFFST_HOFFST_256		Hw0											// Horizontal offset bit 8 (Refer to HwTVEHOFFST)

// Sync Output Control
#define	HwTVEHSVSO_VSOB_256			Hw6											// VSOB bit 8 (Refer to HwVSOB)
#define	HwTVEHSVSO_HSOB_1024		Hw5											// HSOB bit 10 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOB_512			Hw4											// HSOB bit 9 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOB_256			Hw3											// HSOB bit 8 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOE_1024		Hw2											// HSOE bit 10 (Refer to HwHSOE)
#define	HwTVEHSVSO_HSOE_512			Hw1											// HSOE bit 9 (Refer to HwHSOE)
#define	HwTVEHSVSO_HSOE_256			Hw0											// HSOE bit 8 (Refer to HwHSOE)

// Trailing Edge of Vertical Sync Control
#define	HwTVEVSOE_VSOST(X)			((X)*Hw6)									// Programs V-sync relative location for Odd/Even Fields.
#define	HwTVEVSOE_NOVRST_EN			Hw5											// No vertical reset on every field
#define	HwTVEVSOE_NOVRST_NORMAL		HwZERO										// Normal vertical reset operation (interlaced output timing)
#define	HwTVEVSOE_VSOE(X)			((X)*Hw0)									// Trailing Edge of Vertical Sync Control

// VBI Control Register
#define	HwTVEVCTRL_VBICTL(X)		((X)*Hw5)									// VBI Control indicating the current line is VBI.
#define	HwTVEVCTRL_VBICTL_NONE		HwTVEVCTRL_VBICTL(0)						// Do nothing, pass as active video.
#define	HwTVEVCTRL_VBICTL_10LINE	HwTVEVCTRL_VBICTL(1)						// Insert blank(Y:16, Cb,Cr: 128), for example, 10 through 21st line.
#define	HwTVEVCTRL_VBICTL_1LINE		HwTVEVCTRL_VBICTL(2)						// Insert blank data 1 line less for CC processing.
#define	HwTVEVCTRL_VBICTL_2LINE		HwTVEVCTRL_VBICTL(3)						// Insert blank data 2 line less for CC and CGMS processing.
#define	HwTVEVCTRL_MASK				HwTVEVCTRL_VBICTL(3)
#define	HwTVEVCTRL_CCOE_EN			Hw4											// Closed caption odd field enable.
#define	HwTVEVCTRL_CCEE_EN			Hw3											// Closed caption even field enable.
#define	HwTVEVCTRL_CGOE_EN			Hw2											// Copy generation management system enable odd field.
#define	HwTVEVCTRL_CGEE_EN			Hw1											// Copy generation management system enable even field.
#define	HwTVEVCTRL_WSSE_EN			Hw0											// Wide screen enable.

// Connection between LCDC & TVEncoder Control
#define	HwTVEVENCON_EN_EN			Hw0											// Connection between LCDC & TVEncoder Enabled
#define	HwTVEVENCON_EN_DIS			~Hw0										// Connection between LCDC & TVEncoder Disabled

// I/F between LCDC & TVEncoder Selection
#define	HwTVEVENCIF_MV_1			Hw1											// reserved
#define	HwTVEVENCIF_FMT_1			Hw0											// PXDATA[7:0] => CIN[7:0], PXDATA[15:8] => YIN[7:0]
#define	HwTVEVENCIF_FMT_0			HwZERO										// PXDATA[7:0] => YIN[7:0], PXDATA[15:8] => CIN[7:0]

/************************************************************************
*   8. HDMI (Base Addr = 0xF0254000) // R/W
*************************************************************************/
#define HwHDMI_BASE								(0xF0254000)
#define HwHDMICTRL_BASE							(0xF0254000)
#define HwHDMICORE_BASE							(0xF0255000)
#define HwHDMIAES_BASE							(0xF0256000)
#define HwHDMISPDIF_BASE						(0xF0257000)
#define HwHDMII2S_BASE							(0xF0258000)
#define HwHDMICEC_BASE							(0xF0259000)

/************************************************************************
*   9. Video and Image Quality Enhancer(VIQE) (Base Addr = ) // R/W
*************************************************************************/
#define HwVIQE_BASE								(0xF0252000)

/************************************************************************
*   10. MIPI(Mobile Industry Processor Interface) (Base Addr = ) // R/W
*************************************************************************/
#define HwMIPI_BASE								(0xF025B000)

/************************************************************************
*   11. DDI_CONFIG (Base Addr = 0xF0250000) // R/W
*************************************************************************/
#define HwDDI_CONFIG_BASE						(0xF0251000)

// HDMI Control register
#define	HwDDIC_HDMI_CTRL_SEL_LCDC1	Hw15
#define	HwDDIC_HDMI_CTRL_SEL_LCDC0	HwZERO
#define	HwDDIC_HDMI_CTRL_EN			Hw14
#define	HwDDIC_HDMI_CTRL_RST_HDMI	Hw0
#define	HwDDIC_HDMI_CTRL_RST_SPDIF	Hw1
#define	HwDDIC_HDMI_CTRL_RST_TMDS	Hw2
#define	HwDDIC_HDMI_CTRL_RST_NOTUSE	Hw3

// Power Down
#define	HwDDIC_PWDN_LCDSI1			Hw9											// LCD SI Interface
#define	HwDDIC_PWDN_HDMI			Hw8											// HDMI Interface
#define	HwDDIC_PWDN_DDIC			Hw7											// DDIBUS Cache
#define	HwDDIC_PWDN_MSCL1			Hw6											// Memory Scaler 1
#define	HwDDIC_PWDN_MSCL0			Hw5											// Memory Scaler 0
#define	HwDDIC_PWDN_LCDSI			Hw4											// LCDSI Interface
#define	HwDDIC_PWDN_LCDC1			Hw3											// LCD 1 Interface
#define	HwDDIC_PWDN_LCDC0			Hw2											// LCD 0 Interface
#define	HwDDIC_PWDN_VIQE			Hw1											// Video Image Quality Enhancer

// Soft Reset
#define	HwDDIC_SWRESET_LCDSI1		Hw9											// LCD SI Interface
#define	HwDDIC_SWRESET_HDMI			Hw8											// HDMI Interface
#define	HwDDIC_SWRESET_DDIC			Hw7											// DDIBUS Cache
#define	HwDDIC_SWRESET_MSCL1		Hw6											// Memory Scaler 1
#define	HwDDIC_SWRESET_MSCL0		Hw5											// Memory Scaler 0
#define	HwDDIC_SWRESET_LCDSI		Hw4											// LCDSI Interface
#define	HwDDIC_SWRESET_LCDC1		Hw3											// LCD 1 Interface
#define	HwDDIC_SWRESET_LCDC0		Hw2											// LCD 0 Interface
#define	HwDDIC_SWRESET_VIQE			Hw1											// Video Image Quality Enhancer
/************************************************************************
*   12. DDI_CACHE (Base Addr = ) // R/W
*************************************************************************/
#define HwDDICTRL_BASE							(0xF0250000)
#define HwDDICACHE_BASE							(0xF0250000)

//DDIC0_CHn_SEL CACHE Selection
#define DDIC_LCD0_DMA_0_0		0
#define DDIC_LCD0_DMA_0_1		1
#define DDIC_LCD0_DMA_0_2		2
#define DDIC_LCD0_DMA_1			3
#define DDIC_LCD0_DMA_2			4
#define DDIC_MSCL0_DMA_0		5
#define DDIC_MSCL0_DMA_1		6
#define DDIC_MSCL0_DMA_2		7
#define DDIC_VIQE_DMA_0_0		8
#define DDIC_VIQE_DMA_0_1		9
#define DDIC_VIQE_DMA_0_2		10
#define DDIC_LCD0_DMA_3			11
#define DDIC_LCD1_DMA_3			12
#define DDIC_Reserved				13


// DDIC1_CHn_SEL CACHE Selection
#define DDIC_LCD1_DMA_0_0		0
#define DDIC_LCD1_DMA_0_1		1
#define DDIC_LCD1_DMA_0_2		2
#define DDIC_LCD1_DMA_1			3
#define DDIC_LCD1_DMA_2			4
#define DDIC_MSCL1_DMA_0		5
#define DDIC_MSCL1_DMA_1		6
#define DDIC_MSCL1_DMA_2		7
#define DDIC_VIQE_DMA_1_0		8
#define DDIC_VIQE_DMA_1_1		9
#define DDIC_VIQE_DMA_1_2		10
#define DDIC_VIQE_DMA_2_0		11
#define DDIC_VIQE_DMA_2_1		12
#define DDIC_VIQE_DMA_2_2		13

/*******************************************************************************
*
*    TCC88x DataSheet PART 8 VIDEO BUS
*
********************************************************************************/
/************************************************************************
*   4. JPEG Encoder (Base Addr = 0xF0780000) // R/W
*************************************************************************/
#define HwJPEGENCODER_BASE 							(0xF0780000)

/* R/W, JPEG Codec Mode Register */
#define	HwJP_MOD_JPC					HwZERO								/* JPEG Encoder Mode */
#define	HwJP_MOD_JPD					Hw16								/* JPEG Decoder Mode */
#define	HwJP_MOD_MASTER					HwZERO								/* Master Mode */
#define	HwJP_MOD_SLAVE					Hw0									/* Slave Mode */

/* R/W, Interrupt Mask Register */
#define HwJP_INT_MASK_OPERATION_END		Hw0       								/* Encode/Decode Complete */
#define HwJP_INT_MASK_ERROR             Hw1       								/* Encode/Decode Error */
#define HwJP_INT_MASK_OUTPUT_FIFO       Hw2       								/* Output FIFO Buffer Status */
#define HwJP_INT_MASK_INPUT_FIFO		Hw3       								/* Input FIFO Buffer Status */
#define HwJP_INT_MASK_CODED_BUFFER		Hw4       								/* Output Buffer's Full/Empty Status */
#define HwJP_INT_MASK_YBUF_ROLLING      Hw5

/* R/W, Polling or Interrupt Mode Selection Register */
#define	HwJP_TRG_MOD_POLL				HwZERO								/* Polling Mode. No Interrupt Generated */
#define	HwJP_TRG_MOD_INT				Hw0									/* Interrupt Mode */

/* R/W, Image Format Information Register */
#define	HwJP_CHROMA_YONLY				HwZERO								/* Y Only */
#define	HwJP_CHROMA_420					Hw0									/* YUV420 (Y,U,V Separated Mode) */
#define	HwJP_CHROMA_422					Hw1									/* YUV422 (Y,U,V Separated Mode) */
#define	HwJP_CHROMA_444					(Hw0|Hw1)							/* YUV444 (Y,U,V Separated Mode) */
#define	HwJP_CHROMA_422S				Hw2									/* YUV422S (Y,U,V Separated Mode) */

/*  DECODE ONLY : R/W, Decoder Output Scaling Register*/
#define	HwJPD_OUT_SCL_4					Hw0									/* 1/4 (Area Ratio) */
#define	HwJPD_OUT_SCL_16				Hw1									/* 1/16 (Area Ratio) */

/* W, Codec Start Command Register */
#define	HwJP_START_RUN					Hw0									/*  Codec Start   */

/* R, Interrupt Flag Register */
#define	HwJP_INT_FLAG_YBUF_ROLLING		Hw5										/* Y Buffer Rolling Interrupt Status */
#define	HwJP_INT_FLAG_CODED_BUF_STAT	Hw4									/* Coded Buffer Status */
#define	HwJP_INT_FLAG_IFIFO_STAT		Hw3									/* Input FIFO Status */
#define	HwJP_INT_FLAG_OFIFO_STAT		Hw2									/* Output FIFO Status */
#define	HwJP_INT_FLAG_DECODING_ERR		Hw1									/* Decoding Error */
#define	HwJP_INT_FLAG_JOB_FINISHED		Hw0									/* Job Finished */
/************************************************************************
*   5. Video Codec (Base Addr = 0xF0700000) // R/W
*************************************************************************/
#define HwVIDEOCODEC_BASE 						(0xF0700000)


/************************************************************************
*   6. Video Cache (Base Addr = 0xF0710000) // R/W
*************************************************************************/
#define HwVIDEOCACHE_BASE 						(0xF0710000)

// Video Cache Ctrl
#define	HwVIDEOCACHE_PWRASVE 			       Hw31								// Video Cache
#define	HwVIDEOCACHE_CACHEON				Hw0									// Video Codec

// Video Cache Region Enable
#define	HwVIDEOCACHE_WRITE3_EN				Hw13								// Read reasion 3  Enable
#define	HwVIDEOCACHE_READ3_EN				Hw12								// Write reasion 3  Enable
#define	HwVIDEOCACHE_WRITE2_EN				Hw9									// Read reasion 2  Enable
#define	HwVIDEOCACHE_READ2_EN				Hw8								       // Write reasion 2  Enable
#define	HwVIDEOCACHE_WRITE1_EN				Hw5									// Read reasion 1  Enable
#define	HwVIDEOCACHE_READ1_EN				Hw4									// Write reasion 1  Enable
#define	HwVIDEOCACHE_WRITE0_EN				Hw1									// Read reasion 0  Enable
#define	HwVIDEOCACHE_READ0_EN				Hw0									// Write reasion 0  Enable

// VWB Ctrl
#define	HwVIDEOCACHE_VWB_TIMEOUT_VALUE	0xFFFF0000							// Timeout value.
#define	HwVIDEOCACHE_VWB_WCBV				Hw5									// Write cache bvalid option.
#define	HwVIDEOCACHE_VWB_RESET				Hw3									// Reset.
#define	HwVIDEOCACHE_VWB_STATUS				Hw2								       // Read only drain status.
#define	HwVIDEOCACHE_VWB_TIMEOUT_EN		Hw1									// Timeout enable.
#define	HwVIDEOCACHE_VWB_DRAIN_EN			Hw0									// Drain enable

/************************************************************************
*   7. Video Bus Configuration (Base Addr = 0xF0720000) // R/W
*************************************************************************/
#define HwVIDEOBUSCFG_BASE 						(0xF0720000)

// Power Down
#define	HwVIDEOC_PWDN_VIDEO_CACHE 			Hw3									// Video Cache
#define	HwVIDEOC_PWDN_VIDEO_CODEC			Hw2									// Video Codec
//#define	HwVIDEOC_PWDN_JPEG_DECODER			Hw1									// Jpeg decoder
#define	HwVIDEOC_PWDN_JPEG_ENCODER			Hw0									// Jpeg Encoder

// Soft Reset
#define	HwVIDEOC_SWRESET_VIDEO_CACHE 		Hw3									// Video Cache
#define	HwVIDEOC_SWRESET_VIDEO_CODEC		Hw2									// Video Codec
//#define	HwVIDEOC_SWRESET_JPEG_DECODER		Hw1									// Jpeg decoder
#define	HwVIDEOC_SWRESET_JPEG_ENCODER		Hw0									// Jpeg Encoder

/*******************************************************************************
*
*    TCC88x DataSheet PART 9 CAMERA BUS
*
********************************************************************************/

/************************************************************************
*   4. Camera Interface (Base Addr = 0xF0900000) // R/W
*************************************************************************/
//#define HwCIF_BASE								(0xF0900000)
//#define HwCIFEFFECTOR_BASE						(0xF0900100)
//#define HwCIFSCALER_BASE						(0xF0900200)

/************************************************************************
*   4.3 Camera Interface Register Define             (Base Addr = 0xB0200000) // R/W
*************************************************************************/
#define HwCIF_BASE                                	(0xF0900000)
#define HwCIF                                       ((PCIF)&HwCIF_BASE)

// Input Image Color/Pattern Configuration 1
#define	HwICPCR1_ON 				Hw31										// On/Off on CIF >> 0:Can't operate CIF , 1:Operating CIF
#define	HwICPCR1_PWD				Hw30										// Power down mode in camera >> 0:Disable, 1:Power down mode , This power down mode is connected the PWDN of camera sensor
#define	HwICPCR1_TV					Hw29										// TV signal   0: CIF sync signal, 1: TV sync signal
#define	HwICPCR1_TI					Hw28										// Vertical blasnk insert   0: Do not insert vertical blank in even field, 1: insert vertical blank in 1 line in even field
#define	HwICPCR1_UF					Hw27										// Use field signal (in interlace mode, it is decided whether field signal is used or not)  0: Don't Use , 1: Use field signal
#define	HwICPCR1_ITEN				Hw26										// Interlace enable, This field must be set in case of interlace input mode. 0: Disable , 1: Enable
#define	HwICPCR1_FS					Hw25										// Field port signal select (Select field port input between field signal and HS, when the field signal is used in interlace mode)
                                                                                                                                            	// 0: Field signal , 1: Horizontal sync
#define	HwICPCR1_FP					Hw24										// Field port polarity 0: Active low(odd field- high, HS-active low) , 1: Active High (odd field - low, HS-active High)
#define	HwICPCR1_BPS				Hw23										// Bypass Scaler >> 0:Non(CIF Scaler is used), 1:Bypass(CIF Scaler is not used)
#define	HwICPCR1_POL				Hw21										// PXCLK Polarity >> 0:Rising edge, Positive edge, 1:Falling edge, Negative edge
#define	HwICPCR1_SKPF				(Hw20|Hw19|Hw18)							// Skip Frame >> 0~7 #Frames skips	[20:18]
#define	HwICPCR1_M420_ZERO			HwZERO										// Format Convert (YUV422->YUV420) , Not-Convert
#define	HwICPCR1_M420_ODD			Hw17										// converted in odd line skip
#define	HwICPCR1_M420_EVEN			(Hw17|Hw16)									// converted in even line skip
#define	HwICPCR1_BP					Hw15										// Bypass(Non-Separate, if value is 1, and Separate if vlaue is 0) Data format to be stored to memory
																				 	// when BP value is 1, If M420 bit is set to 0, the data format is YUV422, Otherwise, the data format is YUV420
#define	HwICPCR1_BBS_LSB8			Hw14										// When bypass 16bits mode, LSB 8bits are stored in first
#define	HwICPCR1_C656				Hw13										// Convert 656 format 0:Disable, 1:Enable
#define	HwICPCR1_CP_RGB				Hw12										// RGB(555,565,bayer) color pattern
#define	HwICPCR1_PF_444				HwZERO										// 4:4:4 format    // "01" SHOULD BE WRITTEN TO THIS BIT
#define	HwICPCR1_PF_422				Hw10										// 4:2:2 format
#define	HwICPCR1_PF_420				Hw11										// 4:2:0 format or RGB(555,565,bayer) mode
#define	HwICPCR1_RGBM_BAYER			HwZERO										// Bayer RGB Mode
#define	HwICPCR1_RGBM_RGB555		Hw8											// RGB555 Mode
#define	HwICPCR1_RGBM_RGB565		Hw9											// RGB565 Mode
#define	HwICPCR1_RGBBM_16			HwZERO										// 16bit mode
#define	HwICPCR1_RGBBM_8DISYNC		Hw6											// 8bit disable sync
#define	HwICPCR1_RGBBM_8			Hw7											// 8bit mode
#define	HwICPCR1_CS_RGBMG			HwZERO										// 555RGB:RGB(MG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:BG->GR, CCIR656:YCbYCr
#define	HwICPCR1_CS_RGBLG			Hw4											// 555RGB:RGB(LG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:GR->BG, CCIR656:YCrYCb
#define	HwICPCR1_CS_BGRMG			Hw5											// 555RGB:BGR(MG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:RG->GB, CCIR656:CbYCrY
#define	HwICPCR1_CS_BGRLG			(Hw5|Hw4)									// 555RGB:BGR(LG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:GB->RG, CCIR656:CrYCbY
#define	HwICPCR1_BO_SW				Hw2											// Switch the MSB/LSB 8bit Bus, 1 (default , Don't change)
#define	HwICPCR1_HSP_HIGH			Hw1											// Horizontal Sync Polarity 1: Active high (default)
#define	HwICPCR1_VSP_HIGH			Hw0											// Vertical Sync Polarity 1: Active high (default)

// CCIR656 Format Configuration 1
#define	Hw656FCR1_PSL_1ST			HwZERO										// The status word is located the first byte of EAV & SAV
#define	Hw656FCR1_PSL_2ND			Hw25										// The status word is located the second byte of EAV & SAV
#define	Hw656FCR1_PSL_3RD			Hw26										// The status word is located the third byte of EAV & SAV
#define	Hw656FCR1_PSL_4TH			(Hw26|Hw25)									// The status word is located the forth byte of EAV & SAV
																				//FPV [23:16] 0x00FF0000,	SPV [15:8] 0x0000FF00, TPV [7:0]	0x000000FF
// CMOSIF DMA Configuratin 1
#define	HwCDCR1_TM_INC				Hw3											// INC Transfer
#define	HwCDCR1_LOCK_ON				Hw2											// Lock Transfer
#define	HwCDCR1_BS_1				HwZERO										// The DMA transfers the image data as 1 word to memory
#define	HwCDCR1_BS_2				Hw0											// The DMA transfers the image data as 2 word to memory
#define	HwCDCR1_BS_4				Hw1											// The DMA transfers the image data as 4 word to memory
#define	HwCDCR1_BS_8				(Hw1|Hw0)									// The DMA transfers the image data as 8 word to memory (default)

// FIFO Status
#define	HwFIFOSTATE_CLR				Hw21										// Clear FIFO states, 1:Clear, 0:Not Clear
#define	HwFIFOSTATE_REO				Hw19										// Overlay FIFO Read ERROR,	1:The empty signal of input overlay FIFO and read enable signal are High, 0:The empty signal of overlay FIFO is low, or empty is High and read enable signal is Low.
#define	HwFIFOSTATE_REV				Hw18										// V(B) Channel FiFO Read ERROR, 1:The empty signal of input V(B) channel FIFO and read enable signal are High, 0:The empty signal of V(B) channel FIFO is Low, or empty is High and read enable signal is Low.
#define	HwFIFOSTATE_REU				Hw17										// U(R) Channel FiFO Read ERROR, 1:The empty signal of input U(R) channel FIFO and read enable signal are High, 0:The empty signal of U(R) channel FIFO is Low, or empty is High and read enable signal is Low.
#define	HwFIFOSTATE_REY				Hw16										// Y(G) Channel FiFO Read ERROR, 1:The empty signal of input Y(G) channel FIFO and read enable signal are High, 0:The empty signal of Y(G) channel FIFO is Low, or empty is High and read enable signal is Low.
#define	HwFIFOSTATE_WEO				Hw13										// Overlay FIFO Write ERROR, 1:The full signal of overlay FIFO and write enable signal are High, 0:The full signal of overlay FIFO is Low, or full is High and write enable signal is Low.
#define	HwFIFOSTATE_WEV				Hw12										// V(B) Channel FiFO Write ERROR, 1:The full signal of V(B) channel FIFO and write enable signal are High, 0:The full signal of V(B) channel FIFO is Low, or full is High and write enable signal is Low.
#define	HwFIFOSTATE_WEU				Hw11										// U(R) Channel FiFO Write ERROR, 1:The full signal of U(R) channel FIFO and write enable signal are High, 0:The full signal of U(R) channel FIFO is Low, or full is High and write enable signal is Low.
#define	HwFIFOSTATE_WEY				Hw10										// Y(G) Channel FiFO Write ERROR, 1:The full signal of Y channel FIFO and write enable signal are High, 0:The full signal of Y channel FIFO is Low, or full is High and write enable signal is Low.
#define	HwFIFOSTATE_EO				Hw8											// Overlay FIFO Empty Signal, 1:The state of overlay FIFO is empty, 0:The state of overlay FIFO is non-empty.
#define	HwFIFOSTATE_EV				Hw7											// V(B) Channel FiFO Empty Signal, 1:The state of V(B) channel FIFO is empty, 0:The state of V(B) channel FIFO is non-empty.
#define	HwFIFOSTATE_EU				Hw6											// U(R) Channel FiFO Empty Signal, 1:The state of U(R) channel FIFO is empty, 0:The state of U(R) channel FIFO is non-empty.
#define	HwFIFOSTATE_EY				Hw5											// Y(G) Channel FiFO Empty Signal, 1:The state of Y channel FIFO is empty, 0:The state of Y channel FIFO is non-empty.
#define	HwFIFOSTATE_FO				Hw3											// Overlay FiFO FULL Signal, 1:The state of overlay FIFO is full, 0:The state of overlay FIFO is non-full.
#define	HwFIFOSTATE_FV				Hw2											// V(B) Channel FiFO FULL Signal, 1:The state of V(B) channel FIFO is full, 0:The state of V(B) channel FIFO is non-full.
#define	HwFIFOSTATE_FU				Hw1											// U(R) Channel FiFO FULL Signal, 1:The state of U(R) channel FIFO is full, 0:The state of U(R) channel FIFO is non-full.
#define	HwFIFOSTATE_FY				Hw0											// Y(G) Channel FiFO FULL Signal, 1:The state of Y(G) channel FIFO is full, 0:The state of Y(G) channel FIFO is non-full.

// Interrupt & CIF Operating
#define	HwCIRQ_IEN					Hw31										// Interrupt Enable	0:interrupt disable, 1:interrupt enable
#define	HwCIRQ_URV					Hw30										// Update Register in VSYNC	0:Register is update without VSYNC , 1:When VSYNC is posedge, register is updated.
#define	HwCIRQ_ITY					Hw29										// Interrupt Type	0:Pulse type, 1:Hold-up type when respond signal(ICR) is high
#define	HwCIRQ_ICR					Hw28										// Interrupt Clear 0:.... , 1:Interrupt Clear (using ITY is Hold-up type)
#define	HwCIRQ_MVN					Hw26										// Mask interrupt of VS negative edge,	0:Don't mask, 1:Mask
#define	HwCIRQ_MVP					Hw25										// Mask interrupt of VS positive edge,	0:Don't mask, 1:Mask
#define	HwCIRQ_MVIT					Hw24										// Mask interrupt of VCNT Interrupt,	0:Don't mask, 1:Mask
#define	HwCIRQ_MSE					Hw23										// Mask interrupt of Scaler Error,	0:Don't mask, 1:Mask
#define	HwCIRQ_MSF					Hw22										// Mask interrupt of Scaler finish,	0:Don't mask, 1:Mask
#define	HwCIRQ_MENS					Hw21										// Mask interrupt of Encoding start,	0:Don't mask, 1:Mask
#define	HwCIRQ_MRLV					Hw20										// Mask interrupt of Rolling V address,	0:Don't mask, 1:Mask
#define	HwCIRQ_MRLU					Hw19										// Mask interrupt of Rolling U address,	0:Don't mask, 1:Mask
#define	HwCIRQ_MRLY					Hw18										// Mask interrupt of Rolling Y address,	0:Don't mask, 1:Mask
#define	HwCIRQ_MSCF					Hw17										// Mask interrupt of Capture frame,	0:Don't mask, 1:Mask
#define	HwCIRQ_MSOF					Hw16										// Mask interrupt of Stored one frame,	0:Don't mask, 1:Mask
#define	HwCIRQ_SFEN					Hw15										// Generate SOF mode in interlace,	0:Generate SOF when 2-field images are stored memory.
																					//  1:Without field number, generate SOF when 1-field image is stored memory
#define	HwCIRQ_VSS					Hw12										// Status of vertical sync, Non-vertical sync blank area.
#define	HwCIRQ_FL					Hw11										// Field signal states, 0: even field,  1: Odd field
#define	HwCIRQ_VN					Hw10										// VS negative, 0:-, 1:When VS is generated if negative edge
#define	HwCIRQ_VP					Hw9											// VS positive, 0:-, 1:When VS is generated if positive edge
#define	HwCIRQ_VIT					Hw8											// VCNT Interrupt, 0:-, 1:When VCNT is generated....
#define	HwCIRQ_SE					Hw7											// Scaler Error, 0:-, 1:When Scale operation is not correct.
#define	HwCIRQ_SF					Hw6											// Scaler Finish, 0:-, 1:When Scale operation is finished
#define	HwCIRQ_ENS					Hw5											// Encoding start status, 0:-, 1:When Y address is bigger than encoding start address, this bit is high
#define	HwCIRQ_ROLV					Hw4											// Rolling V address status, 0:-, 1:If V address is move to start address, this bit is high
#define	HwCIRQ_ROLU					Hw3											// Rolling U address starus, 0:-, 1:If U address is move to start address, this bit is high
#define	HwCIRQ_ROLY					Hw2											// Rolling Y address starus, 0:-, 1:If Y address is move to start address, this bit is high
#define	HwCIRQ_SCF					Hw1											// Stored captured frame,	0:-, 1:If Captured frame is stored, this bit is high
#define	HwCIRQ_SOF					Hw0											// Stored One frame, 0-, 1:If one frame if stored, this bit is high.

// EINT  by kch
#define HwEINTSEL1_EINT7(X) 		((X)*Hw24)									// EINT7 Source Selection
#define HwEINTSEL1_EINT7_MASK		HwEINTSEL1_EINT7(0x7F)
#define HwEINTSEL1_EINT6(X) 		((X)*Hw16)									// EINT6 Source Selection
#define HwEINTSEL1_EINT6_MASK		HwEINTSEL1_EINT6(0x7F)
#define HwEINTSEL1_EINT5(X) 		((X)*Hw8)									// EINT5 Source Selection
#define HwEINTSEL1_EINT5_MASK		HwEINTSEL1_EINT5(0x7F)
#define HwEINTSEL1_EINT4(X) 		((X)*Hw0)									// EINT4 Source Selection
#define HwEINTSEL1_EINT4_MASK		HwEINTSEL1_EINT4(0x7F)
#define HwEINTSEL0_EINT3(X) 		((X)*Hw24)									// EINT3 Source Selection
#define HwEINTSEL0_EINT3_MASK		HwEINTSEL0_EINT3(0x7F)
#define HwEINTSEL0_EINT2(X) 		((X)*Hw16)									// EINT2 Source Selection
#define HwEINTSEL0_EINT2_MASK		HwEINTSEL0_EINT2(0x7F)
#define HwEINTSEL0_EINT1(X) 		((X)*Hw8)									// EINT1 Source Selection
#define HwEINTSEL0_EINT1_MASK		HwEINTSEL0_EINT1(0x7F)
#define HwEINTSEL0_EINT0(X) 		((X)*Hw0)									// EINT0 Source Selection
#define HwEINTSEL0_EINT0_MASK		HwEINTSEL0_EINT0(0x7F)

// Interrupt Selection
typedef enum
{
	SEL_GPIOA0, 	SEL_GPIOA1, 	SEL_GPIOA2, 	SEL_GPIOA3, 	SEL_GPIOA4, 	// 0
	SEL_GPIOA5, 	SEL_GPIOA6, 	SEL_GPIOA7, 	SEL_GPIOA8, 	SEL_GPIOA9, 	// 5
	SEL_GPIOA10,	SEL_GPIOA11,	SEL_GPIOA12,	SEL_GPIOA13,	SEL_GPIOA14,	// 10
	SEL_GPIOA15,	SEL_GPIOD5, 	SEL_GPIOD6, 	SEL_GPIOD7, 	SEL_GPIOD8, 	// 15
	SEL_GPIOD9, 	SEL_GPIOD10,	SEL_GPIOD13,	SEL_GPIOD14,	SEL_GPIOD15,	// 20
	SEL_GPIOD16,	SEL_GPIOD17,	SEL_GPIOD18,	SEL_GPIOD19,	SEL_GPIOD20,	// 25
	SEL_GPIOF23,	SEL_GPIOF24,	SEL_GPIOF25,	SEL_GPIOF26,	SEL_GPIOF27,	// 30
	SEL_GPIOF20,	SEL_GPIOF17,	SEL_GPIOF13,	SEL_GPIOF10,	SEL_GPIOF8, 	// 35
	SEL_GPIOC28,	SEL_GPIOC29,	SEL_GPIOC30,	SEL_GPIOC31,	SEL_GPIOC9, 	// 40
	SEL_GPIOC13,	SEL_GPIOB28,	SEL_GPIOB29,	SEL_GPIOB30,	SEL_GPIOB31,	// 45
	SEL_GPIOB8, 	SEL_GPIOB12,	SEL_GPIOE4, 	SEL_GPIOE5, 	SEL_GPIOE24,	// 50
	SEL_GPIOE25,	SEL_TSWKU,		SEL_TSSTOP, 	SEL_TSUPDN,		SEL_DUMMY59,     // 55
	SEL_DUMMY60,	SEL_RMWKUP,		SEL_USB0_VBON, 	SEL_USB0_VBOFF, 			     // 60
} ESEL;


// Overlay Control 1
#define	HwOCTRL1_OCNT_MAX			(Hw29|Hw28|Hw27|Hw26|Hw25|Hw24)				//[28:24] Overlay Count FIFO (Hw27|Hw26|Hw25|Hw24|Hw23)
#define	HwOCTRL1_OM_BLOCK			Hw16										// Overlay Method 0:Full image overlay, 1:Block image overlay	, Full image overlay mode, overlay image size is equal to the input image size.
#define	HwOCTRL1_OE_EN				Hw12										// Overlay enable 0:Disable, 1:Enable
#define	HwOCTRL1_XR1_100			Hw10										// XOR in AP1 is 3 (100%)	0:XOR operation, 1:100%	, When AP1 is 3 and CEN & AEN is 1, We select the 100% alpha value or XOR.
#define	HwOCTRL1_XR0_100			Hw9											// XOR in AP0 is 3 (100%)	0:XOR operation, 1:100%	, When AP0 is 3 and CEN & AEN is 1, We select the 100% alpha value or XOR.
#define	HwOCTRL1_AP1_25				HwZERO										// Alpha Value in alpha is 1		// 25%
#define	HwOCTRL1_AP1_50				Hw6											// Alpha Value in alpha is 1		// 50%
#define	HwOCTRL1_AP1_75				Hw7											// Alpha Value in alpha is 1		// 75%
#define	HwOCTRL1_AP1_100			(Hw7|Hw6)									// Alpha Value in alpha is 1		// 100% or XOR operation (for XR value)
#define	HwOCTRL1_AP0_25				HwZERO										// Alpha Value in alpha is 0		// 25%
#define	HwOCTRL1_AP0_50				Hw4											// Alpha Value in alpha is 0		// 50%
#define	HwOCTRL1_AP0_75				Hw5											// Alpha Value in alpha is 0		// 75%
#define	HwOCTRL1_AP0_100			(Hw5|Hw4)									// Alpha Value in alpha is 0		// 100% or XOR operation
																				// When 565RGB and AEN, alpha value is depend on AP0 value.
#define	HwOCTRL1_AEN_EN				Hw2											// Alpha enable	0:Disable, 1:Enable
#define	HwOCTRL1_CEN_EN				Hw0											// Chroma key enable	0:Disable, 1:Enable

// Overlay Control 2
#define	HwOCTRL2_CONV				Hw3											// Color Converter Enable 0:Disable, 1:Enable
#define	HwOCTRL2_RGB_565			HwZERO										// RGB mode 565RGB
#define	HwOCTRL2_RGB_555			Hw1											// RGB mode 555RGB
#define	HwOCTRL2_RGB_444			Hw2											// RGB mode 444RGB
#define	HwOCTRL2_RGB_332			(Hw2|Hw1)									// RGB mode 332RGB
#define	HwOCTRL2_MD					Hw0											// Color Mode	0:YUV Color, 1:RGB color

// Overlay Control 3 -- KEY Value
#define	HwOCTRL3_KEYR_MAX			0x00FF0000									// Chroma-key value R(U), Chroea-key value in R(U) channel, Default value is 0x00
#define	HwOCTRL3_KEYG_MAX			0x0000FF00									// Chroma-key value G(Y), Chroea-key value in G(Y) channel, Default value is 0x00
#define	HwOCTRL3_KEYB_MAX			0x000000FF									// Chroma-key value B(V), Chroea-key value in B(V) channel, Default value is 0x00

// Overlay Control 4 -- Mask KEY Value
#define	HwOCTRL4_MKEYR_MAX			0x00FF0000									// Mask Chroma-key value R(U), Chroea-key value in R(U) channel, Default value is 0x00
#define	HwOCTRL4_MKEYG_MAX			0x0000FF00									// Mask Chroma-key value G(Y), Chroea-key value in G(Y) channel, Default value is 0x00
#define	HwOCTRL4_MKEYB_MAX			0x000000FF									// Mask Chroma-key value B(V), Chroea-key value in B(V) channel, Default value is 0x00

// Camera Down Scaler
#define	HwCDS_SFH_1					HwZERO										// Horizontal Scale Factor, 1/1 down scale
#define	HwCDS_SFH_2					Hw4											// Horizontal Scale Factor, 1/2 down scale
#define	HwCDS_SFH_4					Hw5											// Horizontal Scale Factor, 1/4 down scale
#define	HwCDS_SFH_8					(Hw5|Hw4)									// Horizontal Scale Factor, 1/8 down scale
#define	HwCDS_SFV_1					HwZERO										// Vertical Scale Factor, 1/1 down scale
#define	HwCDS_SFV_2					Hw2											// Vertical Scale Factor, 1/2 down scale
#define	HwCDS_SFV_4					Hw3											// Vertical Scale Factor, 1/4 down scale
#define	HwCDS_SFV_8					(Hw3|Hw2)									// Vertical Scale Factor, 1/8 down scale
#define	HwCDS_SEN_EN				Hw0											// Scale enable, 0:Disable, 1:enable

// CMOSIF Capture mode1
#define	HwCCM1_ENCNUM				0xF0000000									// Encode INT number (using CAP mode) [31:28], value area (0~15), Encode interrupt number
#define	HwCCM1_ROLNUMV				0x0F000000									// Rolling number in V (using CAP mode) [27:24], value area (0~15), Rolling number
#define	HwCCM1_ROLNUMU				0x00F00000									// Rolling number in U (using CAP mode) [23:20], value area (0~15), Rolling number
#define	HwCCM1_ROLNUMY				0x000F0000									// Rolling number in Y (using CAP mode) [19:16], value area (0~15), Rolling number
#define	HwCCM1_CB					Hw10										// Capture Busy,	0:-, 1:Capture busy
#define	HwCCM1_EIT					Hw9											// Encodig INT count,	0:Always 1 pulse, 1:Counting encoding INT
#define	HwCCM1_UES					Hw8											// Using Encoding Start Address,	0:disable, 1:Enable
#define	HwCCM1_SKIPNUM				0x000000F0									// Skip frame number (using CAP mode) [7:4], value area (0~15), Skip frame number
#define	HwCCM1_RLV					Hw3											// Rolling address V,	0:disable, 1:Enable
#define	HwCCM1_RLU					Hw2											// Rolling address U,	0:disable, 1:Enable
#define	HwCCM1_RLY					Hw1											// Rolling address Y,	0:disable, 1:Enable
#define	HwCCM1_CAP					Hw0											// Image Capture,	0:Normal, 1:Image Capture

// CMOSIF Capture mode2
#define	HwCCM2_VCNT					0x000000F0									// Description (Using CAP mode) [7:4], Threshold line counter in interrupt 1:16 line, 2:32 line, 3: 48 line...
#define	HwCCM2_VEN					Hw0											// VCNT folling enable (Using CAP mode) 0:Normal(?) Disalbe아닌가?, 1:Enable

// CMOSIF R2Y confiquration
#define	HwCR2Y_FMT					(Hw4|Hw3|Hw2|Hw1)							// FMT[4:1]	0000 -> Input format 16bit 565RGB(RGB sequence) 자세한 사항 750A CIF SPEC. 1-22
#define	HwCR2Y_EN					Hw0											// R2Y Enable,	0:disable, 1:Enable

// CMOSIF Current Line Count
#define	HwCCLC_LCNT					0x0000FFFF									// LCNT[15:0]	Current Line Count


/************************************************************************
*   4.4 CIF Effecter Register Define             (Base Addr = 0xB0200100) // R/W
*************************************************************************/
#define HwCEM_BASE                          (0xF0900100)
#define HwCEM 								((PEFFECT)&HwCEM_BASE)

// CMOSIF Effect mode
#define	HwCEM_UVS					Hw15										// UV Swap	0:u-v-u-v sequence, 1:v-u-v-u sequence
#define	HwCEM_VB					Hw14										// V Bias (V channel value offset),	0:disable, 1:Enable
#define	HwCEM_UB					Hw13										// U Bias (U channel value offset),	0:disable, 1:Enable
#define	HwCEM_YB					Hw12										// Y Bias (Y channel value offset),	0:disable, 1:Enable
#define	HwCEM_YCS					Hw11										// YC Swap	0:u-y-v-y sequence, 1:y-u-y-v sequence
#define	HwCEM_IVY					Hw10										// Invert Y,	0:disable, 1:Enable
#define	HwCEM_STC					Hw9											// Strong C,	0:disable, 1:Enable
#define	HwCEM_YCL					Hw8											// Y Clamp (Y value clipping),	0:disable, 1:Enable
#define	HwCEM_CS					Hw7											// C Select (Color filter),	0:disable, 1:Enable(Color filter)
#define	HwCEM_SKT					Hw6											// Sketch Enable,	0:disable, 1:Enable
#define	HwCEM_EMM					Hw5											// Emboss mode,	0:Positive emboss, 1:Negative emboss
#define	HwCEM_EMB					Hw4											// Emboss,	0:disable, 1:Enable
#define	HwCEM_NEGA					Hw3											// Negative mode,	0:disable, 1:Enable
#define	HwCEM_GRAY					Hw2											// Gray mode,	0:disable, 1:Enable
#define	HwCEM_SEPI					Hw1											// Sepia mode,	0:disable, 1:Enable
#define	HwCEM_NOR					Hw0											// Normal mode,	0:Effect mode, 1:Normal mode

// CMOSIF Sepia UV Setting
#define	HwHwCSUV_SEPIA_U			0x0000FF00									// SEPIA_U[15:8] U channel threshold value for sepia
#define	HwHwCSUV_SEPIA_V			0x000000FF									// SEPIA_V[7:0] V channel threshold value for sepia

// CMOSIF Color selection
#define	HwCCS_USTART				0xFF000000									// USTART [31:24]	Color filter range start point of U channel
#define	HwCCS_UEND					0x00FF0000									// UEND	[23:16]	Color filter range end point of U channel
#define	HwCCS_VSTART				0x0000FF00									// VSTART [15:8]	Color filter range start point of V channel
#define	HwCCS_VEND					0x000000FF									// VEND	[7:0]	 Color filter range end point of V channel

// CMOSIF H-filter coefficent
#define	HwCHFC_COEF0				0x00FF0000									// COEF0	[23:16] Horizontal filter coefficient0 for emboss or sketch
#define	HwCHFC_COEF1				0x0000FF00									// COEF1	[15:8] Horizontal filter coefficient1 for emboss or sketch
#define	HwCHFC_COEF2				0x000000FF									// COEF2	[7:0] Horizontal filter coefficient2 for emboss or sketch

// CMOSIF Sketch threshold
#define	HwCST_THRESHOLD				0x000000FF									// Sketch [7:0] Sketch threshold

// CMOSIF Clamp threshold
#define	HwCCT_THRESHOLD				0x000000FF									// Clamp [7:0] Clamp threshold

// CMOSIF BIAS
#define	HwCBR_YBIAS					0x00FF0000									// Y_BIAS [23:16] Y value offset
#define	HwCBR_UBIAS					0x0000FF00									// U_BIAS [15:8]	U value offset
#define	HwCBR_VBIAS					0x000000FF									// V_BIAS [7:0]	V value offset

// CMOSIF Image size
#if defined (SENSOR_3M) || defined (SENSOR_5M)
#define	HwCEIS_HSIZE				0x0FFF0000									// HSIZE [26:16]	Horizontal size of input image
#else
#define	HwCEIS_HSIZE				0x07FF0000									// HSIZE [26:16]	Horizontal size of input image
#endif
#define	HwCEIS_VSIZE				0x000007FF									// VSIZE [10:0]	Vertical size of input image

#define HwCIC_H2H_WAIT 				0xFFFF0000       // H2H_WAIT [31:16]   Horizontal sync (hs)to hs wait cycle
#define HwCIC_STB_CYCLE             0x0000FF00      // STB_CYCLE [15:8]  CCIR strobe cycle,  Minimum Value of STB_CYCLE is 4.
#define HwCIC_INP_WAIT              (Hw6|Hw5|Hw4)      // INP_WAIT [6:4]     ???????????????
#define HwCIC_INPR                  Hw3     // ???????????????
#define HwCIC_FA                    Hw2     // Flush all
#define HwCIC_INE                   Hw1     // Inpath Enalbe,   0:disable, 1:Enable
#define HwCIC_INP                   Hw0     // Inpath Mode,   0:Camera mode, 1:Memory mode

// Y값은 32전체를 다 세팅하도록 하고 U, V값은 상위 4bit는 생략해도 cif에서 address를 추정하는 것이 가능하다.
//	HwCISA1_SRC_BASE는 Y값만 4비트 더 존재해서 구별해 놓은 것임 ,, 실제로 사용할 경우 그냥 32 address를 할당 한다.
// CMOSIF INPATH Source address in Y channel
#define	HwCISA1_SRC_BASE			0xF0000000									// SRC_BASE [31:28] Source base address (31 down to 28 bit assign in base address)
#define	HwCISA1_SRC_BASE_Y			0x0FFFFFFF									// SRC_BASE_Y [27:0] Source base address in Y channel (27 down to 0 bit assign in bass address)

// CMOSIF INPATH Source address in U channel
#define	HwCISA2_SRC_TYPE_422SEQ0	HwZERO										// 0: (4:2:2 SEQ0)
#define	HwCISA2_SRC_TYPE_422SEQ1	Hw28										// 1: (4:2:2 SEQ1)
#define	HwCISA2_SRC_TYPE_422SEPA	Hw29										// 2: (4:2:2 Separate)
#define	HwCISA2_SRC_TYPE_420SEPA	(Hw29|Hw28)									// 3: (4:2:0 Separate)
#define	HwCISA2_SRC_BASE_U			0x0FFFFFFF									// SRC_BASE_U [27:0] Source base address in U channal (27 down to 0 bit assign in base address)

// CMOSIF INPATH Source address in V channel
#define	HwCISA3_SRC_BASE_V			0x0FFFFFFF									// SRC_BASE_V [27:0] Source base address in V channal (27 down to 0 bit assign in base address)


// CMOSIF INPATH Source image offset
//#define	HwCISO_SRC_OFFSET_H			0x0FFF0000									// SRC_OFFSET_H [27:16] source address offset in H
//#define	HwCISO_SRC_OFFSET_V			0x00000FFF									// SRC_OFFSET_V [11:0]	source address offset in V
#define	HwCISO_SRC_OFFSET_Y			0x0FFF0000									// SRC_OFFSET_Y [27:16] source address offset in Y channel
#define	HwCISO_SRC_OFFSET_C			0x00000FFF									// SRC_OFFSET_C [11:0]	source address offset in C channel

// CMOSIF INPATH Source image size
#define	HwCISS_SRC_HSIZE			0x0FFF0000									// SRC_HSIZE [27:16] Horizontal size in source image
#define	HwCISS_SRC_VSIZE			0x00000FFF									// SRC_VSIZE [11:0]	Vertical size in source image


// CMOSIF INPATH Destination image size
#define	HwCIDS_DST_HSIZE			0x0FFF0000									// DST_HSIZE [27:16] Horizontal size in destination image
#define	HwCIDS_DST_VSIZE			0x00000FFF									// DST_VSIZE [11:0]	Vertical size in destination image

// HSCALE = SRC_HSIZE*256/DST_HSIZE
// VSCALE = SRC_VSIZE*256/DST_VSIZE
// CMOSIF INPATH Target scale
#define	HwCIS_HSCALE				0x3FFF0000									// HSCALE [29:16] Horizontal scale factor
#define	HwCIS_VSCALE				0x00003FFF									// VSCALE [13:0]	Vertical scale factor


/************************************************************************
*   4.5 CIF Scaler Register Define             (Base Addr = 0xB0200200) // R/W
*************************************************************************/
#define HwCSC_BASE                    	(0xF0900200)
#define HwCSC							((PCIFSCALER)&HwCSC_BASE)
// Scaler configuration
#define	HwSCC_EN					Hw0											// Scaler Enable	0:disable, 1:Enable

// HSCALE = SRC_HSIZE*256/DST_HSIZE
// VSCALE = SRC_VSIZE*256/DST_VSIZE
// Scale factor
#define	HwSCSF_HSCALE				0x3FFF0000									// HSCALE [29:16] Horizontal scale factor
#define	HwSCSF_VSCALE				0x00003FFF									// VSCALE [13:0]	Vertical scale factor

// Image offset
#define	HwSCSO_OFFSET_H				0x0FFF0000									// H [27:16] Horizontal offset
#define	HwSCSO_OFFSET_V				0x00000FFF									// V [11:0]	Vertical offset

// Source image size
#define	HwSCSS_HSIZE				0x0FFF0000									// H [27:16] Horizontal size in source image
#define	HwSCSS_VSIZE				0x00000FFF									// V [11:0]	Vertical size in source image

// Destination image size
#define	HwSCDS_HSIZE				0x0FFF0000									// H [27:16] Horizontal size in destination image
#define	HwSCDS_VSIZE				0x00000FFF									// V [11:0]	Vertical size in destination image




/************************************************************************
*   5. ISP(Image Signal Processor) (Base Addr = 0xF0910000) // R/W
*************************************************************************/
#define HwISPBASE								(0xF0910000)
#define HwISPIMGEFF_BASE						(0xF0910200)
#define HwISPSI_BASE							(0xF0910300)
#define HwISPISP_BASE							(0xF0910400)
#define HwISPFLASH_BASE							(0xF0910660)
#define HwISPSHUT_BASE							(0xF0910680)
#define HwISPCPROC_BASE							(0xF0910800)
#define HwISPMRSZ_BASE							(0xF0910C00)
#define HwISPSRSZ_BASE							(0xF0911000)
#define HwISPMI_BASE							(0xF0911400)
#define HwISPJPE_BASE							(0xF0911800)
#define HwISPMIPI_BASE							(0xF0911C00)

#define RegISP_BASE									0xF0910000
/************************************************************************
*   6. CAMBUS Configuration (Base Addr = 0xF0920000) // R/W
*************************************************************************/
#define HwCAMBUSMATRIX_BASE						(0xF0920000)
//#define HwCAMBUSCFG_BASE						(0xF0930000)

#define HwCAMBUSCFG_BASE                            (0xF0930000)
#define HwCAMBUSCFG									((PCAMBUSCFG)&HwCAMBUSCFG_BASE)

//    volatile unsigned int   PowerDownMode;              // 0xB0230000
// AHB HCLKs, supplied to each block, are controlled  by clock enable signal
#define	HwCAMBUS_MHCLK_ISP			Hw1										// case ISP, MASK Enable signals of HCLK clock gating logic, 0 : enable clock,  1: disable clock
#define	HwCAMBUS_MHCLK_CIF			Hw0										// case CIF, MASK Enable signals of HCLK clock gating logic, 0 : enable clock,  1: disable clock

//    volatile unsigned int   SoftResetRegister;            // 0xB0230004
// AHB SWRESETs, supplied to each block, are controlled  by SWRESET enable signal
#define	HwCAMBUS_SWRESET_ISP		Hw1									// case ISP, Enable signals of HCLK clock gating logic, 0 : disable SWRESET,  1: enable SWRESET
#define	HwCAMBUS_SWRESET_CIF		Hw0									// case CIF, Enable signals of HCLK clock gating logic, 0 : disable SWRESET,  1: enable SWRESET

//   volatile unsigned int   CAMBusClk0Sel;                 // 0xB0230008
#define	HwCAMBUS_CLKO_SEL			Hw0									// Camera Module Clock Selection, 0 : CIF_ICLK,  1: ISP CCLK

//    volatile unsigned int   MemoryEMACtrReg;           // 0xB023000C
//Extra Margin Adjustment pins provide the option of adding delays into internal timing pulses of memory
#define	HwCAMBUS_MEM_EMA			Hw2|Hw1|Hw0						// 001 (default)

//	volatile unsigned int   ISP_RC_CTRL;                    // 0xB0230020
#define	HwCAMBUS_RC_ISP				Hw0						//  ISP_read cache enable

//    volatile unsigned int   ISP_RC_A0MIN;                 // 0xB0230024
#define	HwCAMBUS_SA_ISP				0xFFFFFFFF						//  Start address of ISP prefetch address

//    volatile unsigned int   ISP_RC_A0MAX;                  // 0xB0230028
#define	HwCAMBUS_EA_ISP				0xFFFFFFFF						//  End address of ISP prefetch address

//    volatile unsigned int   CIF_RC_CTRL;                    // 0xB023002C
#define	HwCAMBUS_RC_CIF				Hw0						//  CIF_read cache enable

//    volatile unsigned int   CIF_RC_A0MIN;                 // 0xB0230030
#define	HwCAMBUS_SA_CIF				0xFFFFFFFF						//  Start address of ISP prefetch address

//    volatile unsigned int   CIF_RC_A0MAX;                 // 0xB0230034
#define	HwCAMBUS_EA_CIF				0xFFFFFFFF						//  Start address of ISP prefetch address

#endif /*__TCC88xx_H__*/
