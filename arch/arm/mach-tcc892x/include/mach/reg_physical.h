/*
 * Copyright (c) 2011 Telechips, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PLATFORM_REG_PHYSICAL_H_
#define _PLATFORM_REG_PHYSICAL_H_

#define VIOC_TCC8920

#ifdef __cplusplus
extern "C" {
#endif

//---> Sangwon_temp
    /* temporally type */
    typedef struct {
        unsigned VALUE          :16;
    } TCC_DEF16BIT_IDX_TYPE;

    typedef union {
        unsigned short          nREG;
        TCC_DEF16BIT_IDX_TYPE   bREG;
    } TCC_DEF16BIT_TYPE;

    typedef struct {
        unsigned VALUE          :32;
    } TCC_DEF32BIT_IDX_TYPE;

    typedef union {
        unsigned long           nREG;
        TCC_DEF32BIT_IDX_TYPE   bREG;
    } TCC_DEF32BIT_TYPE;
// <--- Sangwon_temp


/************************************************************************
*    Bit Field Definition
************************************************************************/
#define    Hw37                                 (1LL << 37)
#define    Hw36                                 (1LL << 36)
#define    Hw35                                 (1LL << 35)
#define    Hw34                                 (1LL << 34)
#define    Hw33                                 (1LL << 33)
#define    Hw32                                 (1LL << 32)
#define    Hw31                                 0x80000000
#define    Hw30                                 0x40000000
#define    Hw29                                 0x20000000
#define    Hw28                                 0x10000000
#define    Hw27                                 0x08000000
#define    Hw26                                 0x04000000
#define    Hw25                                 0x02000000
#define    Hw24                                 0x01000000
#define    Hw23                                 0x00800000
#define    Hw22                                 0x00400000
#define    Hw21                                 0x00200000
#define    Hw20                                 0x00100000
#define    Hw19                                 0x00080000
#define    Hw18                                 0x00040000
#define    Hw17                                 0x00020000
#define    Hw16                                 0x00010000
#define    Hw15                                 0x00008000
#define    Hw14                                 0x00004000
#define    Hw13                                 0x00002000
#define    Hw12                                 0x00001000
#define    Hw11                                 0x00000800
#define    Hw10                                 0x00000400
#define    Hw9                                  0x00000200
#define    Hw8                                  0x00000100
#define    Hw7                                  0x00000080
#define    Hw6                                  0x00000040
#define    Hw5                                  0x00000020
#define    Hw4                                  0x00000010
#define    Hw3                                  0x00000008
#define    Hw2                                  0x00000004
#define    Hw1                                  0x00000002
#define    Hw0                                  0x00000001
#define    HwZERO                               0x00000000



/*******************************************************************************
*
*    TCC892x DataSheet PART 2 SMU & PMU
*
********************************************************************************/
#include "structures_smu_pmu.h"

#define HwCKC_BASE                              (0x74000000)

#define HwPIC_BASE                              (0x74100000)
#define HwVIC_BASE                              (0x74100200)

#define HwTMR_BASE                              (0x74300000)

#define HwPMU_BASE                              (0x74400000)

#define HwSMUI2C_BASE                           (0x74500000)

#define HwGPIO_BASE                             (0x74200000)
#define HwGPIOA_BASE                            (0x74200000)
#define HwGPIOB_BASE                            (0x74200040)
#define HwGPIOC_BASE                            (0x74200080)
#define HwGPIOD_BASE                            (0x742000C0)
#define HwGPIOE_BASE                            (0x74200100)
#define HwGPIOF_BASE                            (0x74200140)
#define HwGPIOG_BASE                            (0x74200180)
#define HwGPIOHDMI_BASE                         (0x742001C0)
#define HwGPIOADC_BASE                          (0x74200200)

#define HwSMUCONFIG_BASE                        (0x74600000)



/*******************************************************************************
*
*    TCC892x DataSheet PART 3 GRAPHIC BUS
*
********************************************************************************/
#include "structures_graphic.h"

#define HwGPU_BASE                              (0x70000000)

#define HwGRPBUSCONFIG_BASE                     (0x70010000)



/*******************************************************************************
*
*    TCC88x DataSheet PART 4 MEMORY BUS
*
********************************************************************************/
#include "structures_memory.h"

#define HwMEMORY_BASE                           (0x73000000)

#define HwMBUSCFG_BASE                          (0x73810000)



/*******************************************************************************
*
*    TCC88x DataSheet PART 5 IO BUS
*
********************************************************************************/
#include "structures_io.h"

#define HwEHI_BASE                              (0x76000000)

#define HwMPEFEC_BASE                           (0x76010000)

#define HwSDMMC0_BASE                           (0x76020000)
#define HwSDMMC1_BASE                           (0x76020200)
#define HwSDMMC2_BASE                           (0x76020400)
#define HwSDMMC3_BASE                           (0x76020600)
#define HwSDMMC_CHCTRL_BASE                     (0x76020800)

#define HwGDMA0_BASE                            (0x76030000)
#define HwGDMA1_BASE                            (0x76030100)
#define HwGDMA2_BASE                            (0x76030200)

#define HwOVERLAYMIXER_BASE                     (0x76040000)

#define HwPWM_BASE                              (0x76050000)

#define HwSMC_BASE                              (0x76060000)

#define HwRTC_BASE                              (0x76062000)

#define HwREMOTE_BASE                           (0x76063000)

#define HwTSADC_BASE                            (0x76064000)

#define HwEDI_BASE                              (0x76065000)

#define HwIOBUSCFG_BASE                         (0x76066000)

#define HwPROTECT_BASE                          (0x76067000)

#define HwAXIBM_BASE                            (0x76068000)

#define HwAUDIO1_ADMA_BASE                      (0x76100000)
#define HwAUDIO1_DAI_BASE                       (0x76101000)
#define HwAUDIO1_CDIF_BASE                      (0x76101080)
#define HwAUDIO1_SPDIFTX_BASE                   (0x76102000)

#define HwAUDIO0_ADMA_BASE                      (0x76200000)
#define HwAUDIO0_DAI_BASE                       (0x76201000)
#define HwAUDIO0_CDIF_BASE                      (0x76201080)
#define HwAUDIO0_SPDIFTX_BASE                   (0x76202000)
#define HwAUDIO0_SPDIFRX_BASE                   (0x76202800)

#define HwI2C_MASTER0_BASE                      (0x76300000)
#define HwI2C_MASTER1_BASE                      (0x76310000)
#define HwI2C_MASTER2_BASE                      (0x76320000)
#define HwI2C_MASTER3_BASE                      (0x76330000)
#define HwI2C_SLAVE0_BASE                       (0x76340000)
#define HwI2C_SLAVE1_BASE                       (0x76350000)
#define HwI2C_PORTCFG_BASE                      (0x76360000)

#define HwUART0_BASE                            (0x76370000)
#define HwUART1_BASE                            (0x76380000)
#define HwUART2_BASE                            (0x76390000)
#define HwUART3_BASE                            (0x763A0000)
#define HwUART4_BASE                            (0x763B0000)
#define HwUART5_BASE                            (0x763C0000)
#define HwUART6_BASE                            (0x763D0000)
#define HwUART7_BASE                            (0x763E0000)
#define HwUART_PORTCFG_BASE                     (0x763F0000)

#define HwIDE_BASE                              (0x76400000)

#define HwMSTICK_BASE                           (0x76500000)

#define HwNFC_BASE                              (0x76600000)
#define HwECC_BASE                              (0x76600200)
#define HwECCBASE                               (0x76600208)

#define HwSMC0_BASE                             (0x76700000)
#define HwSMC1_BASE                             (0x76700400)
#define HwSMC2_BASE                             (0x76700800)
#define HwSMC3_BASE                             (0x76700C00)

#define HwTSIF0_BASE                            (0x76800000)
#define HwTSIF1_BASE                            (0x76810000)
#define HwTSIF2_BASE                            (0x76820000)
#define HwTSIF_TSCHS_BASE                     (0x76830000)
#define HwTSIF_IRQSTS_BASE                     (0x7683000C)
#define HwTSIF_DMA0_BASE                        (0x76840000)
#define HwTSIF_DMA1_BASE                        (0x76850000)
#define HwTSIF_DMA2_BASE                        (0x76860000)
#define HwTSIF_PID_BASE                         (0x76870000)

#define	HwTSIF_PIDT(X)				(HwTSIF_PID_BASE+(X)*4)	// R/W, PID Table Register
#define	HwTSIF_PIDT_CH2				Hw31											// Channel 2 enable
#define	HwTSIF_PIDT_CH1				Hw30											// Channel 1 enable
#define	HwTSIF_PIDT_CH0				Hw29											// Channel 0 enable


/*******************************************************************************
GPSB Controller Register Define
********************************************************************************/
#define HwGPSB0_BASE							(0x76900000)
#define HwGPSB1_BASE							(0x76910000)
#define HwGPSB2_BASE							(0x76920000)
#define HwGPSB3_BASE							(0x76930000)
#define HwGPSB4_BASE							(0x76940000)
#define HwGPSB5_BASE							(0x76950000)
#define HwGPSB_PORTCFG_BASE						(0x76960000)
#define HwGPSB_PIDTABLE_BASE					(0x76970000)

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
 *******************************************************************************/

#define HwUSBOTG_BASE                           (0x76A00000)
#define HwUSBPHYCFG_BASE                        (0x76066028)

#define HwGE_FCH_SSB                    (Hw24+Hw25+Hw26)                    // Operation Mode
#define HwGE_DCH_SSB                    (Hw24+Hw25+Hw26)                    // Operation Mode

// Front-End Channel 0 Control
#define HwGE_FCHO_OPMODE                (Hw8+Hw9+Hw10)                      // Operation Mode
#define HwGE_FCHO_SDFRM                 (Hw0+Hw1+Hw2+Hw3+Hw4)               // Source Data Format

// Front-End Channel 1 Control
#define HwGE_FCH1_OPMODE                (Hw8+Hw9+Hw10)                      // Operation Mode
#define HwGE_FCH1_SDFRM                 (Hw0+Hw1+Hw2+Hw3+Hw4)               // Source Data Format

// Front-End Channel 2 Control
#define HwGE_FCH2_OPMODE                (Hw8+Hw9+Hw10)                      // Operation Mode
#define HwGE_FCH2_SDFRM                 (Hw0+Hw1+Hw2+Hw3+Hw4)               // Source Data Format

// Source Control
#define Hw2D_SACTRL_S2_ARITHMODE        (Hw10+Hw9+Hw8)
#define Hw2D_SACTRL_S1_ARITHMODE        (Hw6+Hw5+Hw4)
#define Hw2D_SACTRL_S0_ARITHMODE        (Hw2+Hw1+Hw0)
#define Hw2D_SFCTRL_S2_Y2REN            (Hw26)
#define Hw2D_SFCTRL_S1_Y2REN            (Hw25)
#define Hw2D_SFCTRL_S0_Y2REN            (Hw24)
#define Hw2D_SFCTRL_S2_Y2RMODE          (Hw21+Hw20)
#define Hw2D_SFCTRL_S1_Y2RMODE          (Hw19+Hw18)
#define Hw2D_SFCTRL_S0_Y2RMODE          (Hw17+Hw16)
#define Hw2D_SACTRL_S2_CHROMAEN         (Hw18)
#define Hw2D_SACTRL_S1_CHROMAEN         (Hw17)
#define Hw2D_SACTRL_S0_CHROMAEN         (Hw16)
#define Hw2D_SFCTRL_S3_SEL			(Hw6+Hw7)
#define Hw2D_SFCTRL_S2_SEL              (Hw5+Hw4)
#define Hw2D_SFCTRL_S1_SEL              (Hw3+Hw2)
#define Hw2D_SFCTRL_S0_SEL              (Hw1+Hw0)

// Source Operator Pattern
#define HwGE_OP_ALL                     (HwGE_ALPHA + HwGE_PAT_RY + HwGE_PAT_GU + HwGE_PAT_BV)
#define HwGE_ALPHA                      (HwGE_PAT_GU + HwGE_PAT_BV )                // ALPHA VALUE
#define HwGE_PAT_RY                     (Hw16+Hw17+Hw18+Hw19+Hw20+Hw21+Hw22+Hw23)   // Pattern Value RED,   Y
#define HwGE_PAT_GU                     (Hw8+Hw9+Hw10+Hw11+Hw12+Hw13+Hw14+Hw15)     // Pattern Value GREEN, U
#define HwGE_PAT_BV                     (Hw0+Hw1+Hw2+Hw3+Hw4+Hw5+Hw6+Hw7)           // Pattern Value BULE,  V

// Source Operation Control
#define HwGE_OP_CTRL_ACON1              (Hw30+Hw29+Hw28)                    // Alpha-value control 1
#define HwGE_OP_CTRL_ACON0              (Hw26+Hw25+Hw24)                    // Alpha-value control 0
#define HwGE_OP_CTRL_CCON1              (Hw23+Hw22+Hw21+Hw20)               // color control 1
#define HwGE_OP_CTRL_CCON0              (Hw19+Hw18+Hw17+Hw16)               // color control 1
#define HwGE_OP_CTRL_ATUNE              (Hw13+Hw12)                         // Alpha value tuning 
#define HwGE_OP_CTRL_CSEL               (Hw9+Hw8)                           // chroma-key 
#define HwGE_OP_CTRL_OPMODE             (Hw4+Hw3+Hw2+Hw1+Hw0)               // operation mode

// Back -End Channel Control
#define HwGE_BCH_DCTRL_MABC             Hw21
#define HwGE_BCH_DCTRL_YSEL             Hw18                                // YUV4:4:4 to YUVx:x:x Y Control
#define HwGE_BCH_DCTRL_XSEL             (Hw16+Hw17)                         // YUV4:4:4 to YUVx:x:x X Control
#define HwGE_BCH_DCTRL_CEN              Hw15                                // Destination Format Converter Control
#define HwGE_BCH_DCTRL_CMODE            (Hw13+Hw14)                         // RGBtoYUV Converter Type
#define HwGE_BCH_DCTRL_DSUV             Hw11
#define HwGE_BCH_DCTRL_OPMODE           (Hw8+Hw9+Hw10)                      // Operation Mode COPY, MIRROR, ROTATE
#define HwGE_BCH_DCTRL_DOP              Hw6
#define HwGE_BCH_DCTRL_DEN              Hw5
#define HwGE_BCH_DCTRL_DDFRM            (Hw0+Hw1+Hw2+Hw3+Hw4)               // Destination Data Format

// Graphic Engine Control
#define HwGE_GE_INT_EN                  Hw16                                // Graphic Engine Interrupt Enable
#define HwGE_GE_CTRL_EN                 (Hw0+Hw1+Hw2)                       // Graphic Engine Enable

// Graphic Engine Interrupt Request
#define HwGE_GE_IREQ_FLG                Hw16                                // Graphic Engine Flag Bit
#define HwGE_GE_IREQ_IRQ                Hw0


// DISP Control Reg
#define HwDISP_EVP                      Hw31                                // External Vsync Polarity
#define HwDISP_EVS                      Hw30                                // External Vsync Enable
#define HwDISP_R2YMD                    (Hw29+Hw28)                         // RGB to YCbCr Conversion Option
#define HwDISP_ADVI                     Hw26                                // Advanced interlaced mode
#define HwDISP_656                      Hw24                                // CCIR 656 Mode
#define HwDISP_CKG                      Hw23                                // Clock Gating Enable for Timing Generator
#define HwDISP_SREQ                     Hw22                                // stop request
#define HwDISP_PXDW                     (Hw19+Hw18+Hw17+Hw16)               // PXDW
#define HwDISP_ID                       Hw15                                // Inverted Data Enable
#define HwDISP_IV                       Hw14                                // Inverted Vertical Sync
#define HwDISP_IH                       Hw13                                // Inverted Horizontal Sync
#define HwDISP_IP                       Hw12                                // Inverted Pixel Clock
#define HwDISP_CLEN                     Hw11                                // Clipping Enable
#define HwDISP_DP                       Hw10                                // Double Pixel Data
#define HwDISP_R2Y                      Hw9                                 // RGB to YCbCr Converter Enable for Output 
#define HwDISP_NI                       Hw8                                 // Non-Interlace
#define HwDISP_TV                       Hw7                                 // TV mode 
#define HwDISP_SRST                     Hw6                                 // Device display reset 
#define HwDISP_SWAP                     (Hw3+Hw2+Hw1)                       // Output RGB overlay swap
#define HwDISP_LEN                      Hw0                                 // LCD Controller Enable

// RDMA Control Reg
#define HwDMA_INTL                      Hw31                                // Interlaced Image
#define HwDMA_BFMD                      Hw30                                // Bfield mode
#define HwDMA_BF                        Hw29                                // Bottom field
#define HwDMA_IEN                       Hw28                                // Image Display Function for Each Image
#define HwDMA_STRM                      Hw27                                // streaming mode
#define HwDMA_ASEL                      Hw24                                // Image Displaying Function for Each Image
#define HwDMA_UVI                       Hw23                                // UV ineterpolation
#define HwDMA_NUVIHH                    Hw23                                // No uv interpolation
#define HwDMA_UPD                       Hw16                                // data update enable
#define HwDMA_PD                        Hw15                                // Bit padding
#define HwDMA_Y2RMD                     (Hw10+Hw9)                          // YCbCr to RGB Conversion Option
#define HwDMA_Y2R                       Hw8                                 // YCbCr to RGB Conversion Enable Bit
#define HwDMA_BR                        Hw7                                 // Bit Reverse
#define HwDMA_FMT                       (Hw4+Hw3+Hw2+Hw1+Hw0)               // Image Format

//DISP status

#define HwLSTATUS_VS                    Hw31                                // Monitoring vertical sync
#define HwLSTATUS_BUSY                  Hw30                                // Busy signal
#define HwLSTATUS_EF                    Hw29                                // Even-Field(Read Only). 0:Odd field or frame, 1:Even field or frame
#define HwLSTATUS_DEOF                  Hw28                                // DMA End of Frame flag
#define HwLSTATUS_I0EOF                 Hw27                                // Image 0 End of Frame flag
#define HwLSTATUS_I1EOF                 Hw26                                // Image 1 End of Frame flag
#define HwLSTATUS_I2EOF                 Hw25                                // Image 2 End of Frame flag
#define HwLSTATUS_IE2F                  Hw12                                // Image 2 end-of-frame falling edge flag
#define HwLSTATUS_IE2R                  Hw11                                // Image 2 end-of-frame rising edge flag
#define HwLSTATUS_IE1F                  Hw10                                // Image 1 end-of-frame falling edge flag
#define HwLSTATUS_IE1R                  Hw9                                 // Image 1 end-of-frame rising edge flag
#define HwLSTATUS_IE0F                  Hw8                                 // Image 0 end-of-frame falling edge flag
#define HwLSTATUS_IE0R                  Hw7                                 // Image 0 end-of-frame rising edge flag
#define HwLSTATUS_DEF                   Hw6                                 // DMA end-of-frame falling edge flag
#define HwLSTATUS_SREQ                  Hw5                                 // Device stop request
#define HwLSTATUS_DD                    Hw4                                 // Disable done
#define HwLSTATUS_RU                    Hw3                                 // Register update flag
#define HwLSTATUS_VSF                   Hw2                                 // VS falling flag
#define HwLSTATUS_VSR                   Hw1                                 // VS rising flag
#define HwLSTATUS_FU                    Hw0                                 // LCD output fifo under-run flag.


/*******************************************************************************
*
*    TCC88x DataSheet PART 6 HSIO BUS
*
********************************************************************************/
#include "structures_hsio.h"

#define HwGMAC_BASE                             (0x71700000)

#define HwUSB20HOST_EHCI_BASE                   (0x71200000)
#define HwUSB20HOST_OHCI_BASE                   (0x71300000)

#define HwHSIOBUSCFG_BASE                       (0x71EA0000)

#define HwDMAX_BASE                             (0x71EB0000)

#define HwCIPHER_BASE                           (0x71EC0000)



/*******************************************************************************
*
*    TCC88x DataSheet PART 7 DISPLAY BUS
*
********************************************************************************/
#include "structures_display.h"

#define HwVIOC_BASE                             (0x72000000)

#define BASE_ADDR_CPUIF                         (0x72100000)
#define BASE_ADDR_VIOC                          HwVIOC_BASE
/* DISP */
#define HwVIOC_LCD              ( (BASE_ADDR_VIOC  + 0x0000         )) // [15:14] == 2'b00, [13:12] == 1'b00
#define HwVIOC_DISP0            ( (BASE_ADDR_VIOC  + 0x0000 + 0x0000)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_DISP1            ( (BASE_ADDR_VIOC  + 0x0000 + 0x0100)) // 64 words    // TCC8910 
#define HwVIOC_DISP2            ( (BASE_ADDR_VIOC  + 0x0000 + 0x0200)) // 64 words    // TCC8910

/* RDMA */
#define HwVIOC_RDMA00           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0400)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_RDMA01           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0500)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_RDMA02           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0600)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_RDMA03           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0700)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_RDMA04           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0800)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_RDMA05           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0900)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_RDMA06           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0A00)) // 64 words    // TCC8910 | TCC8010
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_RDMA07           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0B00)) // 64 words    // TCC8910
#define HwVIOC_RDMA08           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0C00)) // 64 words    // TCC8910
#define HwVIOC_RDMA09           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0D00)) // 64 words    // TCC8910
#define HwVIOC_RDMA10           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0E00)) // 64 words    // TCC8910
#define HwVIOC_RDMA11           ( (BASE_ADDR_VIOC  + 0x0000 + 0x0F00)) // 64 words    // TCC8910
#define HwVIOC_RDMA12           ( (BASE_ADDR_VIOC  + 0x0000 + 0x1000)) // 64 words    // TCC8910
#define HwVIOC_RDMA13           ( (BASE_ADDR_VIOC  + 0x0000 + 0x1100)) // 64 words    // TCC8910
#define HwVIOC_RDMA14           ( (BASE_ADDR_VIOC  + 0x0000 + 0x1200)) // 64 words    // TCC8910
#define HwVIOC_RDMA15           ( (BASE_ADDR_VIOC  + 0x0000 + 0x1300)) // 64 words    // TCC8910
#define HwVIOC_RDMA16           ( (BASE_ADDR_VIOC  + 0x0000 + 0x1400)) // 64 words    // TCC8910
#define HwVIOC_RDMA17           ( (BASE_ADDR_VIOC  + 0x0000 + 0x1500)) // 64 words    // TCC8910
#endif

/* WMIX */
#define HwVIOC_WMIX0            ( (BASE_ADDR_VIOC  + 0x0000 + 0x1800)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WMIX1            ( (BASE_ADDR_VIOC  + 0x0000 + 0x1900)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WMIX2            ( (BASE_ADDR_VIOC  + 0x0000 + 0x1A00)) // 64 words    // TCC8910 | TCC8010
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_WMIX3            ( (BASE_ADDR_VIOC  + 0x0000 + 0x1B00)) // 64 words    // TCC8910
#define HwVIOC_WMIX4            ( (BASE_ADDR_VIOC  + 0x0000 + 0x1C00)) // 64 words    // TCC8910
#define HwVIOC_WMIX5            ( (BASE_ADDR_VIOC  + 0x0000 + 0x1D00)) // 64 words    // TCC8910
#define HwVIOC_WMIX6            ( (BASE_ADDR_VIOC  + 0x0000 + 0x1E00)) // 64 words    // TCC8910
#endif

#define HwVIOC_WMIX0_ALPHA      ( (BASE_ADDR_VIOC  + 0x0000 + 0x1840)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WMIX1_ALPHA      ( (BASE_ADDR_VIOC  + 0x0000 + 0x1940)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WMIX2_ALPHA      ( (BASE_ADDR_VIOC  + 0x0000 + 0x1A40)) // 64 words    // TCC8910 | TCC8010

/* SCALER */
#define HwVIOC_SC0              ( (BASE_ADDR_VIOC  + 0x0000 + 0x2000)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_SC1              ( (BASE_ADDR_VIOC  + 0x0000 + 0x2100)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_SC2              ( (BASE_ADDR_VIOC  + 0x0000 + 0x2200)) // 64 words    // TCC8910 | TCC8010
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_SC3              ( (BASE_ADDR_VIOC  + 0x0000 + 0x2300)) // 64 words    // TCC8910
#endif
/* WDMA */
#define HwVIOC_WDMA00           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2800)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WDMA01           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2900)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WDMA02           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2A00)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WDMA03           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2B00)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WDMA04           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2C00)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WDMA05           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2D00)) // 64 words    // TCC8910 | TCC8010
#define HwVIOC_WDMA06           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2E00)) // 64 words    // TCC8910 | TCC8010
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_WDMA07           ( (BASE_ADDR_VIOC  + 0x0000 + 0x2F00)) // 64 words    // TCC8910
#define HwVIOC_WDMA08           ( (BASE_ADDR_VIOC  + 0x0000 + 0x3000)) // 64 words    // TCC8910
#endif
/* DEINTLS */
#define HwVIOC_DEINTLS          ( (BASE_ADDR_VIOC  + 0x0000 + 0x3800)) // 64 words    // TCC8910 | TCC8010
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_FDLY0            ( (BASE_ADDR_VIOC  + 0x0000 + 0x3900)) // 64 words    // TCC8910
#define HwVIOC_FDLY1            ( (BASE_ADDR_VIOC  + 0x0000 + 0x3A00)) // 64 words    // TCC8910
#define HwVIOC_FIFO             ( (BASE_ADDR_VIOC  + 0x0000 + 0x3B00)) // 64 words    // TCC8910
#define HwVIOC_DEBLOCK          ( (BASE_ADDR_VIOC  + 0x0000 + 0x3C00)) // 64 words    // TCC8910
#endif
//*********************************************************************************************************************************************
// [15:14] == 2'b01
//*********************************************************************************************************************************************
/* VIN */
#define HwVIOC_VINDEMUX         ( (BASE_ADDR_VIOC  + 0xA800         ))                // TCC8910 | TCC8010
#define HwVIOC_VIN00            ( (BASE_ADDR_VIOC  + 0x4000         ))                // TCC8910 | TCC8010
#define HwVIOC_VIN01            ( (BASE_ADDR_VIOC  + 0x4400         ))                // TCC8910 | TCC8010 
#define HwVIOC_VIN10            ( (BASE_ADDR_VIOC  + 0x5000         ))                // TCC8910 | TCC8010 
#define HwVIOC_VIN11            ( (BASE_ADDR_VIOC  + 0x5400         ))                // TCC8910 | TCC8010 
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_VIN20            ( (BASE_ADDR_VIOC  + 0x6000         ))                // TCC8910 
#define HwVIOC_VIN21            ( (BASE_ADDR_VIOC  + 0x6400         ))                // TCC8910 
#define HwVIOC_VIN30            ( (BASE_ADDR_VIOC  + 0x7000         ))                // TCC8910 
#define HwVIOC_VIN31            ( (BASE_ADDR_VIOC  + 0x7400         ))                // TCC8910 
#endif

//*********************************************************************************************************************************************
// [15:14] == 2'b10
//*********************************************************************************************************************************************
#define HwVIOC_FILT2D           ( (BASE_ADDR_VIOC  + 0x8000        ))                // TCC8910
#define HwVIOC_LUT              ( (BASE_ADDR_VIOC  + 0x9000        ))                // TCC8910 | TCC8010
#define HwVIOC_LUT_TAB          ( (BASE_ADDR_VIOC  + 0x9400        ))
#define HwVIOC_CONFIG           ( (BASE_ADDR_VIOC  + 0xA000        ))                // TCC8910 | TCC8010
#define HwVIOC_IREQ             ( (BASE_ADDR_VIOC  + 0xA000 + 0x000)) //  16 word
#define HwVIOC_PCONFIG          ( (BASE_ADDR_VIOC  + 0xA000 + 0x040)) //  32 word
#define HwVIOC_POWER            ( (BASE_ADDR_VIOC  + 0xA000 + 0x0C0)) //  16 word
#define HwVIOC_FCODEC           ( (BASE_ADDR_VIOC  + 0xB000        ))                // TCC8910

//*********************************************************************************************************************************************
// [15:14] == 2'b11
//*********************************************************************************************************************************************
/* VIOC TIMER */
#define HwVIOC_TIMER_BASE       ( (BASE_ADDR_VIOC  + 0xC000        ))                // TCC8910 | TCC8010
#define HwVIOC_TIMER            ( (BASE_ADDR_VIOC  + 0xC000        ))

/* VIQE */
#define HwVIOC_VIQE0_BASE       ( (BASE_ADDR_VIOC  + 0xD000        ))                // TCC8910 | TCC8010
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_VIQE1_BASE       ( (BASE_ADDR_VIOC  + 0xE000        ))                // TCC8910 
#endif
#define  HwVIOC_VIQE0                           (HwVIOC_VIQE0_BASE)
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define  HwVIOC_VIQE1                           (HwVIOC_VIQE1_BASE)
#define HwVIOC_MMU_BASE         ( (BASE_ADDR_VIOC  + 0xF000        ))                // TCC8910 
#define HwVIOC_MMU              ( (BASE_ADDR_VIOC  + 0xF000        ))
#endif

/* CPUIF */
#define HwVIOC_CPUIF            ( (BASE_ADDR_CPUIF + 0x0000        ))                // 
#define HwVIOC_CPUIF0           ( (BASE_ADDR_CPUIF + 0x0000        ))                // TCC8910 | TCC8010
// [09:08] == 2'b00,
// [09:08] == 2'b00, [07:06] == 2'b01, [05] == 1'b0
// [09:08] == 2'b00, [07:06] == 2'b01, [05] == 1'b1
// [09:08] == 2'b00, [07:06] == 2'b10, [05] == 1'b0
// [09:08] == 2'b00, [07:06] == 2'b10, [05] == 1'b1
#if defined (VIOC_TCC8910) || defined (VIOC_TCC8920)
#define HwVIOC_CPUIF1           ( (BASE_ADDR_CPUIF + 0x0100        ))                // TCC8910
#endif

// [09:08] == 2'b01,
// [09:08] == 2'b01, [07:06] == 2'b01, [05] == 1'b0
// [09:08] == 2'b01, [07:06] == 2'b01, [05] == 1'b1
// [09:08] == 2'b01, [07:06] == 2'b10, [05] == 1'b0
// [09:08] == 2'b01, [07:06] == 2'b10, [05] == 1'b1
/* OUT CONFIGURE */
#define HwVIOC_OUTCFG           ( (BASE_ADDR_CPUIF + 0x0200        )) // [09:08] == 2'b10, 


#define HwNTSCPAL_BASE                          (0x72200000)
#define HwTVE_BASE								(0x72200000)
#define HwNTSCPAL_ENC_CTRL_BASE                 (0x72200800)

// Encoder Mode Control A
#define	HwTVECMDA_PWDENC_PD							Hw7											// Power down mode for entire digital logic of TV encoder
#define	HwTVECMDA_FDRST_1							Hw6											// Chroma is free running as compared to H-sync
#define	HwTVECMDA_FDRST_0							HwZERO										// Relationship between color burst & H-sync is maintained for video standards
#define	HwTVECMDA_FSCSEL(X)							((X)*Hw4)
#define	HwTVECMDA_FSCSEL_NTSC						HwTVECMDA_FSCSEL(0)							// Color subcarrier frequency is 3.57954545 MHz for NTSC
#define	HwTVECMDA_FSCSEL_PALX						HwTVECMDA_FSCSEL(1)							// Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
#define	HwTVECMDA_FSCSEL_PALM						HwTVECMDA_FSCSEL(2)							// Color subcarrier frequency is 3.57561149 MHz for PAL-M
#define	HwTVECMDA_FSCSEL_PALCN						HwTVECMDA_FSCSEL(3)							// Color subcarrier frequency is 3.58205625 MHz for PAL-combination N
#define	HwTVECMDA_FSCSEL_MASK						HwTVECMDA_FSCSEL(3)
#define	HwTVECMDA_PEDESTAL							Hw3											// Video Output has a pedestal
#define	HwTVECMDA_NO_PEDESTAL						HwZERO										// Video Output has no pedestal
#define	HwTVECMDA_PIXEL_SQUARE						Hw2											// Input data is at square pixel rates.
#define	HwTVECMDA_PIXEL_601							HwZERO										// Input data is at 601 rates.
#define	HwTVECMDA_IFMT_625							Hw1											// Output data has 625 lines
#define	HwTVECMDA_IFMT_525							HwZERO										// Output data has 525 lines
#define	HwTVECMDA_PHALT_PAL							Hw0											// PAL encoded chroma signal output
#define	HwTVECMDA_PHALT_NTSC						HwZERO										// NTSC encoded chroma signal output

// Encoder Mode Control B
#define	HwTVECMDB_YBIBLK_BLACK						Hw4											// Video data is forced to Black level for Vertical non VBI processed lines.
#define	HwTVECMDB_YBIBLK_BYPASS						HwZERO										// Input data is passed through forn non VBI processed lines.
#define	HwTVECMDB_CBW(X)							((X)*Hw2)
#define	HwTVECMDB_CBW_LOW							HwTVECMDB_CBW(0)							// Low Chroma band-width
#define	HwTVECMDB_CBW_MEDIUM						HwTVECMDB_CBW(1)							// Medium Chroma band-width
#define	HwTVECMDB_CBW_HIGH							HwTVECMDB_CBW(2)							// High Chroma band-width
#define	HwTVECMDB_CBW_MASK							HwTVECMDB_CBW(3)							// 
#define	HwTVECMDB_YBW(X)							((X)*Hw0)
#define	HwTVECMDB_YBW_LOW							HwTVECMDB_YBW(0)							// Low Luma band-width
#define	HwTVECMDB_YBW_MEDIUM						HwTVECMDB_YBW(1)							// Medium Luma band-width
#define	HwTVECMDB_YBW_HIGH							HwTVECMDB_YBW(2)							// High Luma band-width
#define	HwTVECMDB_YBW_MASK							HwTVECMDB_YBW(3)							// 

// Encoder Clock Generator
#define	HwTVEGLK_XT24_24MHZ							Hw4											// 24MHz Clock input
#define	HwTVEGLK_XT24_27MHZ							HwZERO										// 27MHz Clock input
#define	HwTVEGLK_GLKEN_RST_EN						Hw3											// Reset Genlock
#define	HwTVEGLK_GLKEN_RST_DIS						~Hw3										// Release Genlock
#define	HwTVEGLK_GLKE(X)							((X)*Hw1)
#define	HwTVEGLK_GLKE_INT							HwTVEGLK_GLKE(0)							// Chroma Fsc is generated from internal constants based on current user setting
#define	HwTVEGLK_GLKE_RTCO							HwTVEGLK_GLKE(2)							// Chroma Fsc is adjusted based on external RTCO input
#define	HwTVEGLK_GLKE_CLKI							HwTVEGLK_GLKE(3)							// Chroma Fsc tracks non standard encoder clock (CLKI) frequency
#define	HwTVEGLK_GLKE_MASK							HwTVEGLK_GLKE(3)							//
#define	HwTVEGLK_GLKEN_GLKPL_HIGH					Hw0											// PAL ID polarity is active high
#define	HwTVEGLK_GLKEN_GLKPL_LOW					HwZERO										// PAL ID polarity is active low

// Encoder Mode Control C
#define	HwTVECMDC_CSMDE_EN							Hw7											// Composite Sync mode enabled
#define	HwTVECMDC_CSMDE_DIS							~Hw7										// Composite Sync mode disabled (pin is tri-stated)
#define	HwTVECMDC_CSMD(X)							((X)*Hw5)
#define	HwTVECMDC_CSMD_CSYNC						HwTVECMDC_CSMD(0)							// CSYN pin is Composite sync signal
#define	HwTVECMDC_CSMD_KEYCLAMP						HwTVECMDC_CSMD(1)							// CSYN pin is Keyed clamp signal
#define	HwTVECMDC_CSMD_KEYPULSE						HwTVECMDC_CSMD(2)							// CSYN pin is Keyed pulse signal
#define	HwTVECMDC_CSMD_MASK							HwTVECMDC_CSMD(3)
#define	HwTVECMDC_RGBSYNC(X)						((X)*Hw3)
#define	HwTVECMDC_RGBSYNC_NOSYNC					HwTVECMDC_RGBSYNC(0)						// Disable RGBSYNC (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_RGB						HwTVECMDC_RGBSYNC(1)						// Sync on RGB output signal (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_G							HwTVECMDC_RGBSYNC(2)						// Sync on G output signal (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_MASK						HwTVECMDC_RGBSYNC(3)

// DAC Output Selection
#define	HwTVEDACSEL_DACSEL_CODE0					HwZERO										// Data output is diabled (output is code '0')
#define	HwTVEDACSEL_DACSEL_CVBS						Hw0											// Data output in CVBS format

// DAC Power Down
#define	HwTVEDACPD_PD_EN							Hw0											// DAC Power Down Enabled
#define	HwTVEDACPD_PD_DIS							~Hw0										// DAC Power Down Disabled

// Sync Control
#define	HwTVEICNTL_FSIP_ODDHIGH						Hw7											// Odd field active high
#define	HwTVEICNTL_FSIP_ODDLOW						HwZERO										// Odd field active low
#define	HwTVEICNTL_VSIP_HIGH						Hw6											// V-sync active high
#define	HwTVEICNTL_VSIP_LOW							HwZERO										// V-sync active low
#define	HwTVEICNTL_HSIP_HIGH						Hw5											// H-sync active high
#define	HwTVEICNTL_HSIP_LOW							HwZERO										// H-sync active low
#define	HwTVEICNTL_HSVSP_RISING						Hw4											// H/V-sync latch enabled at rising edge
#define	HwTVEICNTL_HVVSP_FALLING					HwZERO										// H/V-sync latch enabled at falling edge
#define	HwTVEICNTL_VSMD_START						Hw3											// Even/Odd field H/V sync output are aligned to video line start
#define	HwTVEICNTL_VSMD_MID							HwZERO										// Even field H/V sync output are aligned to video line midpoint
#define	HwTVEICNTL_ISYNC(X)							((X)*Hw0)
#define	HwTVEICNTL_ISYNC_FSI						HwTVEICNTL_ISYNC(0)							// Alignment input format from FSI pin
#define	HwTVEICNTL_ISYNC_HVFSI						HwTVEICNTL_ISYNC(1)							// Alignment input format from HSI,VSI,FSI pin
#define	HwTVEICNTL_ISYNC_HVSI						HwTVEICNTL_ISYNC(2)							// Alignment input format from HSI,VSI pin
#define	HwTVEICNTL_ISYNC_VFSI						HwTVEICNTL_ISYNC(3)							// Alignment input format from VSI,FSI pin
#define	HwTVEICNTL_ISYNC_VSI						HwTVEICNTL_ISYNC(4)							// Alignment input format from VSI pin
#define	HwTVEICNTL_ISYNC_ESAV_L						HwTVEICNTL_ISYNC(5)							// Alignment input format from EAV,SAV codes (line by line)
#define	HwTVEICNTL_ISYNC_ESAV_F						HwTVEICNTL_ISYNC(6)							// Alignment input format from EAV,SAV codes (frame by frame)
#define	HwTVEICNTL_ISYNC_FREE						HwTVEICNTL_ISYNC(7)							// Alignment is free running (Master mode)
#define	HwTVEICNTL_ISYNC_MASK						HwTVEICNTL_ISYNC(7)

// Offset Control
#define	HwTVEHVOFFST_INSEL(X)						((X)*Hw6)
#define	HwTVEHVOFFST_INSEL_BW16_27MHZ				HwTVEHVOFFST_INSEL(0)						// 16bit YUV 4:2:2 sampled at 27MHz
#define	HwTVEHVOFFST_INSEL_BW16_13P5MH				HwTVEHVOFFST_INSEL(1)						// 16bit YUV 4:2:2 sampled at 13.5MHz
#define	HwTVEHVOFFST_INSEL_BW8_13P5MHZ				HwTVEHVOFFST_INSEL(2)						// 8bit YUV 4:2:2 sampled at 13.5MHz
#define	HwTVEHVOFFST_INSEL_MASK						HwTVEHVOFFST_INSEL(3)
#define	HwTVEHVOFFST_VOFFST_256						Hw3											// Vertical offset bit 8 (Refer to HwTVEVOFFST)
#define	HwTVEHVOFFST_HOFFST_1024					Hw2											// Horizontal offset bit 10 (Refer to HwTVEHOFFST)
#define	HwTVEHVOFFST_HOFFST_512						Hw1											// Horizontal offset bit 9 (Refer to HwTVEHOFFST)
#define	HwTVEHVOFFST_HOFFST_256						Hw0											// Horizontal offset bit 8 (Refer to HwTVEHOFFST)

// Sync Output Control
#define	HwTVEHSVSO_VSOB_256							Hw6											// VSOB bit 8 (Refer to HwVSOB)
#define	HwTVEHSVSO_HSOB_1024						Hw5											// HSOB bit 10 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOB_512							Hw4											// HSOB bit 9 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOB_256							Hw3											// HSOB bit 8 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOE_1024						Hw2											// HSOE bit 10 (Refer to HwHSOE)
#define	HwTVEHSVSO_HSOE_512							Hw1											// HSOE bit 9 (Refer to HwHSOE)
#define	HwTVEHSVSO_HSOE_256							Hw0											// HSOE bit 8 (Refer to HwHSOE)

// Trailing Edge of Vertical Sync Control
#define	HwTVEVSOE_VSOST(X)							((X)*Hw6)									// Programs V-sync relative location for Odd/Even Fields.
#define	HwTVEVSOE_NOVRST_EN							Hw5											// No vertical reset on every field
#define	HwTVEVSOE_NOVRST_NORMAL						HwZERO										// Normal vertical reset operation (interlaced output timing)
#define	HwTVEVSOE_VSOE(X)							((X)*Hw0)									// Trailing Edge of Vertical Sync Control

// VBI Control Register
#define	HwTVEVCTRL_VBICTL(X)						((X)*Hw5)									// VBI Control indicating the current line is VBI.
#define	HwTVEVCTRL_VBICTL_NONE						HwTVEVCTRL_VBICTL(0)						// Do nothing, pass as active video.
#define	HwTVEVCTRL_VBICTL_10LINE					HwTVEVCTRL_VBICTL(1)						// Insert blank(Y:16, Cb,Cr: 128), for example, 10 through 21st line.
#define	HwTVEVCTRL_VBICTL_1LINE						HwTVEVCTRL_VBICTL(2)						// Insert blank data 1 line less for CC processing.
#define	HwTVEVCTRL_VBICTL_2LINE						HwTVEVCTRL_VBICTL(3)						// Insert blank data 2 line less for CC and CGMS processing.
#define	HwTVEVCTRL_MASK								HwTVEVCTRL_VBICTL(3)					
#define	HwTVEVCTRL_CCOE_EN							Hw4											// Closed caption odd field enable.
#define	HwTVEVCTRL_CCEE_EN							Hw3											// Closed caption even field enable.
#define	HwTVEVCTRL_CGOE_EN							Hw2											// Copy generation management system enable odd field.
#define	HwTVEVCTRL_CGEE_EN							Hw1											// Copy generation management system enable even field.
#define	HwTVEVCTRL_WSSE_EN							Hw0											// Wide screen enable.

// Connection between LCDC & TVEncoder Control
#define	HwTVEVENCON_EN_EN							Hw0											// Connection between LCDC & TVEncoder Enabled
#define	HwTVEVENCON_EN_DIS							~Hw0										// Connection between LCDC & TVEncoder Disabled

// I/F between LCDC & TVEncoder Selection
#define	HwTVEVENCIF_MV_1							Hw1											// reserved
#define	HwTVEVENCIF_FMT_1							Hw0											// PXDATA[7:0] => CIN[7:0], PXDATA[15:8] => YIN[7:0]
#define	HwTVEVENCIF_FMT_0							HwZERO										// PXDATA[7:0] => YIN[7:0], PXDATA[15:8] => CIN[7:0]




#define HwHDMI_CTRL_BASE                        (0x72300000)
#define HwHDMI_CORE_BASE                        (0x72310000)
#define HwHDMI_AES_BASE                         (0x72320000)
#define HwHDMI_SPDIF_BASE                       (0x72330000)
#define HwHDMI_I2S_BASE                         (0x72340000)
#define HwHDMI_CEC_BASE                         (0x72350000)

/************************************************************************
*   11. DDI_CONFIG (Base Addr = 0x72380000) // R/W
*************************************************************************/

#define HwDDI_CONFIG_BASE                       (0x72380000)


// Power Down
#define HwDDIC_PWDN_HDMI                      Hw2           // HDMI
#define HwDDIC_PWDN_NTSC                      Hw1            // NTSL/PAL
#define HwDDIC_PWDN_LCDC                       Hw0         // LCDC

// Soft Reset
#define HwDDIC_SWRESET_HDMI                  Hw2           // HDMI
#define HwDDIC_SWRESET_NTSC                  Hw1              // NTSL/PAL
#define HwDDIC_SWRESET_LCDC                   Hw0             // LCDC




/*******************************************************************************
*
*    TCC88x DataSheet PART 8 VIDEO BUS
*
********************************************************************************/
#include "structures_video.h"

#define HwVBUS_BASE                             (0x75000000)
#define HwVIDEOBUSCONFIG_BASE                   (0x75200000)








/*******************************************************************************
*
*    etc
*
********************************************************************************/

/*******************************************************************************
*    NFC(NAND Flash Controller) Register Define 
********************************************************************************/
#define HwNFC_CTRL_CS3SEL_1             Hw31                            // Nand Flash nCS3 is High (Disabled)
#define HwNFC_CTRL_CS3SEL_0             HwZERO                          // Nand Flash nCS3 is Low (Enabled)
#define HwNFC_CTRL_CS2SEL_1             Hw30                            // Nand Flash nCS2 is High (Disabled)
#define HwNFC_CTRL_CS2SEL_0             HwZERO                          // Nand Flash nCS2 is Low (Enabled)
#define HwNFC_CTRL_CS1SEL_1             Hw29                            // Nand Flash nCS1 is High (Disabled)
#define HwNFC_CTRL_CS1SEL_0             HwZERO                          // Nand Flash nCS1 is Low (Enabled)
#define HwNFC_CTRL_CS0SEL_1             Hw28                            // Nand Flash nCS0 is High (Disabled)
#define HwNFC_CTRL_CS0SEL_0             HwZERO                          // Nand Flash nCS0 is Low (Enabled)
#define HwNFC_CTRL_CFG_nCS3             HwNFC_CTRL_CS3SEL_1
#define HwNFC_CTRL_CFG_nCS2             HwNFC_CTRL_CS2SEL_1
#define HwNFC_CTRL_CFG_nCS1             HwNFC_CTRL_CS1SEL_1
#define HwNFC_CTRL_CFG_nCS0             HwNFC_CTRL_CS0SEL_1
#define HwNFC_CTRL_CFG_NOACT            (Hw31|Hw30|Hw29|Hw28)

#define HwNFC_CTRL_RDNDLY(X)            ((X)*Hw25)                      // Nand Flash Read Data Delay
#define HwNFC_CTRL_RDNDLY_0             HwNFC_CTRL_RDNDLY(0)            // Nand Flash Read Data Delay (NONE)
#define HwNFC_CTRL_RDNDLY_1             HwNFC_CTRL_RDNDLY(1)            // Nand Flash Read Data Delay (1 Clock)
#define HwNFC_CTRL_RDNDLY_2             HwNFC_CTRL_RDNDLY(2)            // Nand Flash Read Data Delay (2 Clock)
#define HwNFC_CTRL_RDNDLY_3             HwNFC_CTRL_RDNDLY(3)            // Nand Flash Read Data Delay (3 Clock)
#define HwNFC_CTRL_RDNDLY_4             HwNFC_CTRL_RDNDLY(4)            // Nand Flash Read Data Delay (4 Clock)
#define HwNFC_CTRL_RDNDLY_5             HwNFC_CTRL_RDNDLY(5)            // Nand Flash Read Data Delay (5 Clock)
#define HwNFC_CTRL_RDNDLY_6             HwNFC_CTRL_RDNDLY(6)            // Nand Flash Read Data Delay (6 Clock)
#define HwNFC_CTRL_RDNDLY_7             HwNFC_CTRL_RDNDLY(7)            // Nand Flash Read Data Delay (7 Clock)

#define HwNFC_CTRL_ARFW_FLAG_EN         Hw24                            // Nand Flash Automatically Ready Flag Wait Enanble
#define HwNFC_CTRL_ARFW_FLAG_DIS        ~Hw24                           // Nand Flash Automatically Ready Flag Wait Disable

#define HwNFC_CTRL_DACK_EN              Hw20                            // Nand Flash DMA Acknowledge Enable
#define HwNFC_CTRL_DACK_DIS             ~Hw20                           // Nand Flash DMA Acknowledge Disable
#define HwNFC_CTRL_DMODE_NDMA           Hw19                            // Nand Flash DMA Mode Selection ( NDMA )
#define HwNFC_CTRL_DMODE_GDMA           ~Hw19                           // Nand Flash DMA Mode Selection ( GDMA )
#define HwNFC_CTRL_BMODE_BURST          Hw18                            // Nand Flash Burst Operation Mode Selection For NDMA( Burst Mode[User Data + ECC] )
#define HwNFC_CTRL_BMODE_NOR            ~Hw18                           // Nand Flash Burst Operation Mode Selection For NDMA( Normal Mode[User Data])
#define HwNFC_CTRL_ECCON_EN             Hw17                            // Nand Flash ECC Encoding/Decoding Enable
#define HwNFC_CTRL_ECCON_DIS            ~Hw17                           // Nand Flash ECC Encoding/Decoding Disable
#define HwNFC_CTRL_PRSEL_P              Hw16                            // Nand Flash Program Mode Selection for Burst Transfer
#define HwNFC_CTRL_PRSEL_R              ~Hw16                           // Nand Flash Read Mode Selection for Burst Transfer
#define HwNFC_CTRL_DNUM(X)              ((X)*Hw14)
#define HwNFC_CTRL_DNUM_0               HwNFC_CTRL_DNUM(0)              // Nand Flash Multiplee Data Configuration Number
#define HwNFC_CTRL_DNUM_1               HwNFC_CTRL_DNUM(1)              // Nand Flash Multiplee Data Configuration Number
#define HwNFC_CTRL_DNUM_2               HwNFC_CTRL_DNUM(2)              // Nand Flash Multiplee Data Configuration Number

#define HwNFC_CTRL_RDY0SEL              Hw12                            // NAND Flash Extend Ready Input Selection
#define HwNFC_CTRL_RDY3TO0SEL           ~Hw12                           // Address/Command Mask Enable
#define HwNFC_CTRL_RDY_MSK_EN           Hw11                            // NAND Flahs Ready Mask Enable(Mask)
#define HwNFC_CTRL_RDY_MSK_DIS          ~Hw11                           // NAND Flahs Ready Mask DIsable(UnMask)
#define HwNFC_CTRL_MASK_EN              Hw10                            // Address/Command Mask Enable
#define HwNFC_CTRL_MASK_DIS             ~Hw10                           // Address/Command Mask Enable

#define HwNFC_CTRL_BW(X)                ((X)*Hw8)
#define HwNFC_CTRL_BW_8                 HwNFC_CTRL_BW(0)                // 1Read/Write
#define HwNFC_CTRL_BW_16                HwNFC_CTRL_BW(1)                // 2Read/Write
#define HwNFC_CTRL_BW_32                HwNFC_CTRL_BW(2)                // 4Read/Write

#define HwNFC_CTRL_BSIZE(X)             ((X)*Hw6)
#define HwNFC_CTRL_BSIZE_1              HwNFC_CTRL_BSIZE(0)             // 1Read/Write
#define HwNFC_CTRL_BSIZE_2              HwNFC_CTRL_BSIZE(1)             // 2Read/Write
#define HwNFC_CTRL_BSIZE_4              HwNFC_CTRL_BSIZE(2)             // 4Read/Write
#define HwNFC_CTRL_BSIZE_8              HwNFC_CTRL_BSIZE(3)             // 8Read/Write

#define HwNFC_CTRL_PSIZE(X)             ((X)*Hw3)
#define HwNFC_CTRL_PSIZE_256            HwNFC_CTRL_PSIZE(0)             // 1 Page = 256 Half-Word
#define HwNFC_CTRL_PSIZE_512            HwNFC_CTRL_PSIZE(1)             // 1 Page = 512 Byte
#define HwNFC_CTRL_PSIZE_1024           HwNFC_CTRL_PSIZE(2)             // 1 Page = 1024 Half-Word
#define HwNFC_CTRL_PSIZE_2048           HwNFC_CTRL_PSIZE(3)             // 1 Page = 2048 Byte
#define HwNFC_CTRL_PSIZE_4096           HwNFC_CTRL_PSIZE(4)             // 1 Page = 4096 Byte
#define HwNFC_CTRL_PSIZE_MASK           HwNFC_CTRL_PSIZE(7)
#define HwNFC_CTRL_CADDR                Hw0                             // Number of Address Cycle

/*******************************************************************************
* NFC SPARE                                                         (0xF05B0058)
*******************************************************************************/
#define HwNFC_SPARE_SPW                 Hw17
#define HwNFC_SPARE_SPR                 Hw16

/*******************************************************************************
* NFC IRQCFC                                                        (0xF05B0064)
*******************************************************************************/
#define HwNFC_IRQCFG_RDYIEN             Hw31                            //NAND Flash Ready Interrupt Enable
#define HwNFC_IRQCFG_PIEN               Hw30                            //NAND Flash Program Interrupt Enable
#define HwNFC_IRQCFG_RDIEN              Hw29                            //NAND Flash Read Interrupt Enable
#define HwNFC_IRQCFG_SPWIEN             Hw27                            //NAND Flash Spare Area Burst Write Interrupt
#define HwNFC_IRQCFG_SPRIEN             Hw26                            //NAND Flash Spare Area Burst Read Interrupt
#define HwNFC_IRQCFG_MEIEN              Hw26                            //MLC ECC Decoding Interrupt Enable 

/*******************************************************************************
* NFC IRQ                                                            (0xF05B0068)
*******************************************************************************/
#define HwNFC_IRQ_RDYIRQ                Hw31                            // NAND Flash Ready End Interrupt
#define HwNFC_IRQ_PIRQ                  Hw30                            // NAND Flash Program End Interrupt
#define HwNFC_IRQ_RDIRQ                 Hw29                            // NAND Flash Read End Interrupt
#define HwNFC_IRQ_SPWIRQ                Hw27                            // NAND Flash Spare Burst Write End Interrupt
#define HwNFC_IRQ_SPRIRQ                Hw26                            // NAND Flash Spare Burst Read End Interrupt
#define HwNFC_IRQ_MEDIRQ                Hw23                            // MLC ECC Decoding End Interrupt
#define HwNFC_IRQ_RDYFG                 Hw15                            // NAND Flash Ready End Flag
#define HwNFC_IRQ_PFG                   Hw14                            // NAND Flash Program End Flag
#define HwNFC_IRQ_RDFG                  Hw13                            // NAND Flash Read End Flag 
#define HwNFC_IRQ_SPWFG                 Hw11                            // NANF Flash Spare Burst Write End Flag 
#define HwNFC_IRQ_SPRFG                 Hw10                            // NANF Flash Spare Burst Read End Flag 
#define HwECC_IREQ_MEDFG                Hw7                             // MLC ECC Decoding Flag Register
#define HwECC_IREQ_MEEFG                Hw6                             // MLC ECC Encoding Flag Register
#define HwECC_IREQ_SEDFG                Hw5                             // SLC ECC Decoding Flag Register
#define HwECC_IREQ_SEEFG                Hw4                             // SLC ECC Encoding Flag Register
#define HwECC_IREQ_ERRFG                Hw3                             // MLC/SLC Error Flag

/*******************************************************************************
* NFC STATUS                                                        (0xF05B006C)
*******************************************************************************/
#define HwNFC_STATUS_RDY_RDY            Hw31
#define HwNFC_CTRL_FS_RDY               Hw30                            // FIFO status is Ready to write and read in FIFO
#define HwNFC_CTRL_FS_BUSY              ~Hw30                           // FIFO status is Busy to write and read in FIFO

/*******************************************************************************
* NFC(NAND Flash Controller) NDMA Register Define (Base Addr = 0xF05B010C)
********************************************************************************/
#define HwNFC_NDMA_LOCK                 Hw11
#define HwNFC_NDMA_ARB                  Hw10
#define HwNFC_NDMA_BSIZE_1              (0)    
#define HwNFC_NDMA_BSIZE_2              Hw6
#define HwNFC_NDMA_BSIZE_4              Hw7
#define HwNFC_NDMA_BSIZE_8              (Hw6|Hw7)
#define HwNFC_NDMA_WSIZE_8              (0)                    
#define HwNFC_NDMA_WSIZE_16             Hw4                    
#define HwNFC_NDMA_WSIZE_32             (Hw5|Hw4)        
#define HwNFC_NDMA_SEQ                  Hw1            
#define HwNFC_NDMA_FLAG                 Hw3
#define HwNFC_NDMA_EN                   Hw0

#define HwECC_NDMA_ERR                  Hw27

/*******************************************************************************
* ECC CONTROL REGISTER 
*******************************************************************************/
#define HwECC_CTRL_DSIZE_CLEAR          (Hw26|Hw25|Hw24|Hw23|Hw22|Hw21|Hw20|Hw19|Hw18|Hw17|Hw16)

#define HwECC_CTRL_CORPASS_EN           Hw5                             // MLC Correction Pass Enable
#define HwECC_CTRL_CORPASS_DIS          ~Hw5                            // MLC Correction Pass Disable
#define HwECC_CTRL_ECCMODE_NDM_EN       Hw4                             // ECC Mode Selection( 0: Nor 1: NFC Dedicated Memory)
#define HwECC_CTRL_ECCMODE_NDM_DIS      ~Hw4                            // ECC Mode Selection( 0: Nor 1: NFC Dedicated Memory)

#define HwECC_CTRL_EN_SLCEN             Hw2                             // SLC ECC Encoding Enable
#define HwECC_CTRL_EN_SLCDE             (Hw2|Hw0)                       // SLC ECC Decoding Enable
#define HwECC_CTRL_EN_MCL4EN            (Hw2|Hw1)                       // MLC ECC4 Encoding Enable
#define HwECC_CTRL_EN_MCL4DE            (Hw2|Hw1|Hw0)                   // MLC ECC4 Decoding Enable
#define HwECC_CTRL_EN_MCL6EN            (Hw3)                           // MLC ECC6 Encoding Enable
#define HwECC_CTRL_EN_MCL6DE            (Hw3|Hw0)                       // MLC ECC6 Decoding Enable
#define HwECC_CTRL_EN_MCL12EN           (Hw3|Hw1)                       // MLC ECC12 Encoding Enable
#define HwECC_CTRL_EN_MCL12DE           (Hw3|Hw1|Hw0)                   // MLC ECC12 Decoding Enable
#define HwECC_CTRL_EN_MCL16EN           (Hw3|Hw2)                       // MLC ECC16 Encoding Enable
#define HwECC_CTRL_EN_MCL16DE           (Hw3|Hw2|Hw0)                   // MLC ECC16 Decoding Enable
#define HwECC_CTRL_EN_MCL24EN           (Hw3|Hw2|Hw1)                   // MLC ECC24 Encoding Enable
#define HwECC_CTRL_EN_MCL24DE           (Hw3|Hw2|Hw1|Hw0)               // MLC ECC24 Decoding Enable
#define HwECC_CTRL_EN_DIS               ~(Hw3|Hw2|Hw1|Hw0)              // ECC Disable

/*******************************************************************************
* ECC Error Status Register
*******************************************************************************/
#define HwERR_NUM_ERR1                  Hw0                             // 1 Error Occurred(MLC4)
#define HwERR_NUM_ERR2                  Hw1                             // 2 Error Occurred(MLC4)
#define HwERR_NUM_ERR3                  (Hw1|Hw0)                       // 3 Error Occurred(MLC4)
#define HwERR_NUM_ERR4                  Hw2                             // 4 Error Occurred(MLC4)
#define HwERR_NUM_ERR5                  (Hw2|Hw0)                       // 5 Error Occurred(MLC4)
#define HwERR_NUM_ERR6                  (Hw2|Hw1)                       // 6 Error Occurred(MLC4)
#define HwERR_NUM_ERR7                  (Hw2|Hw1|Hw0)                   // 7 Error Occurred(MLC4)
#define HwERR_NUM_ERR8                  Hw3                             // 8 Error Occurred(MLC4)
#define HwERR_NUM_ERR9                  (Hw3|Hw0)                       // 9 Error Occurred(MLC4)
#define HwERR_NUM_ERR10                 (Hw3|Hw1)                       // 10 Error Occurred(MLC4)
#define HwERR_NUM_ERR11                 (Hw3|Hw1|Hw0)                   // 11 Error Occurred(MLC4)
#define HwERR_NUM_ERR12                 (Hw3|Hw2)                       // 12 Error Occurred(MLC4)
#define HwERR_NUM_ERR13                 (Hw3|Hw2|Hw0)                   // 13 Error Occurred(MLC4)
#define HwERR_NUM_ERR14                 (Hw3|Hw2|Hw1)                   // 14 Error Occurred(MLC4)
#define HwERR_NUM_ERR15                 (Hw3|Hw2|Hw1|Hw0)               // 15 Error Occurred(MLC4)
#define HwERR_NUM_ERR16                 Hw4                             // 16 Error Occurred(MLC4)
#define HwERR_NUM_ERR17                 (Hw4|Hw0)                       // 17 Error Occurred(MLC4)
#define HwERR_NUM_ERR18                 (Hw4|Hw1)                       // 18 Error Occurred(MLC4)
#define HwERR_NUM_ERR19                 (Hw4|Hw1|Hw0)                   // 19 Error Occurred(MLC4)
#define HwERR_NUM_ERR20                 (Hw4|Hw2)                       // 20 Error Occurred(MLC4)
#define HwERR_NUM_ERR21                 (Hw4|Hw2|Hw0)                   // 21 Error Occurred(MLC4)
#define HwERR_NUM_ERR22                 (Hw4|Hw2|Hw1)                   // 22 Error Occurred(MLC4)
#define HwERR_NUM_ERR23                 (Hw4|Hw2|Hw1|Hw0)               // 23 Error Occurred(MLC4)
#define HwERR_NUM_ERR24                 (Hw4|Hw3)                       // 24 Error Occurred(MLC4)
#define HwERR_NUM_CORIMP                (Hw4|Hw3|Hw0)                   // Correction Impossible Error

#define HwERR_NUM_NOERR                 HwZERO                          // No Error

#define HwGPIO                          ((NAND_PGPIO)HwGPIO_BASE)
#define HwGPIOA                         ((NAND_PGPION)HwGPIOA_BASE)
#define HwGPIOB                         ((NAND_PGPION)HwGPIOB_BASE)
#define HwGPIOC                         ((NAND_PGPION)HwGPIOC_BASE)
#define HwGPIOD                         ((NAND_PGPION)HwGPIOD_BASE)
#define HwGPIOE                         ((NAND_PGPION)HwGPIOE_BASE)
#define HwGPIOF                         ((NAND_PGPION)HwGPIOF_BASE)
#define HwGPIOG                         ((NAND_PGPION)HwGPIOG_BASE)



#ifdef __cplusplus
}
#endif

#endif /* _PLATFORM_REG_PHYSICAL_H_ */
