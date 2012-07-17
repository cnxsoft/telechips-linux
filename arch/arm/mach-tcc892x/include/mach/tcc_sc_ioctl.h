/* linux/arch/arm/mach-tcc92xx/include/mach/tcc_sc_ioctl.h
 *
 * Copyright (C) 2009, 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
#ifndef _TCC_SC_IOCTL_H_
#define _TCC_SC_IOCTL_H_

#define OFFSET_THR		0x00	/* Transmit Holding register            */
#define OFFSET_RBR		0x00	/* Receive Buffer register              */
#define OFFSET_DLL		0x00	/* Divisor Latch (Low-Byte)             */
#define OFFSET_IER		0x04	/* Interrupt Enable Register            */
#define OFFSET_DLM		0x04	/* Divisor Latch (High-Byte)            */
#define OFFSET_IIR		0x08	/* Interrupt Identification Register    */
#define OFFSET_FCR		0x08	/* FIFO Control Register                */
#define OFFSET_LCR		0x0C	/* Line Control Register                */
#define OFFSET_MCR		0x10	/* Modem Control Register               */
#define OFFSET_LSR		0x14	/* Line Status Register                 */
#define OFFSET_MSR		0x18	/* Modem Status Register                */
#define OFFSET_SCR		0x1C	/* SCR Scratch Register                 */
#define OFFSET_AFT		0x20	/* AFC Trigger Level Register           */
#define OFFSET_UCR		0x24	/* UART Control Register                */
#define OFFSET_SCCR		0x60	/* Smart Card Control Register */
#define OFFSET_STC  	0x64    /* Smart Card TX Count Register  */

// UART Driver Channel Configuration Definition
#define	DRV_UART_IER_EnableMSI				Hw3
#define	DRV_UART_IER_DisableMSI				0
#define	DRV_UART_IER_EnableRcvLSI			Hw2
#define	DRV_UART_IER_DisableRcvLSI			0
#define	DRV_UART_IER_EnableTXI				Hw1
#define	DRV_UART_IER_DisableTXI				0
#define	DRV_UART_IER_EnableRXI				Hw0
#define	DRV_UART_IER_DisableRXI				0

#define	DRV_UART_FCR_RxTriggerLvl_SHIFT		6
#define	DRV_UART_FCR_RxTriggerLvl(X)		((X)<<DRV_UART_FCR_RxTriggerLvl_SHIFT)
#define	DRV_UART_FCR_RxTriggerLvl_01		DRV_UART_FCR_RxTriggerLvl(0)
#define	DRV_UART_FCR_RxTriggerLvl_04		DRV_UART_FCR_RxTriggerLvl(1)
#define	DRV_UART_FCR_RxTriggerLvl_08		DRV_UART_FCR_RxTriggerLvl(2)
#define	DRV_UART_FCR_RxTriggerLvl_14		DRV_UART_FCR_RxTriggerLvl(3)
#define	DRV_UART_FCR_TxWindowLvl_SHIFT		4
#define	DRV_UART_FCR_TxWindowLvl(X)			((X)<<DRV_UART_FCR_TxWindowLvl_SHIFT)
#define	DRV_UART_FCR_TxWindowLvl_16			DRV_UART_FCR_TxWindowLvl(0)
#define	DRV_UART_FCR_TxWindowLvl_08			DRV_UART_FCR_TxWindowLvl(1)
#define	DRV_UART_FCR_TxWindowLvl_04			DRV_UART_FCR_TxWindowLvl(2)
#define	DRV_UART_FCR_TxWindowLvl_01			DRV_UART_FCR_TxWindowLvl(3)
#define	DRV_UART_FCR_EnableDMA				Hw3
#define	DRV_UART_FCR_DisableDMA				0
#define	DRV_UART_FCR_TxFifoReset			Hw2
#define	DRV_UART_FCR_RxFifoReset			Hw1
#define	DRV_UART_FCR_EnableFifo				Hw0
#define	DRV_UART_FCR_DisableFifo				0

#define	DRV_UART_LCR_AccessDivisor			Hw7
#define	DRV_UART_LCR_NotAccessDivisor		0
#define	DRV_UART_LCR_EnableBreak			Hw6
#define	DRV_UART_LCR_DisableBreak			0
#define	DRV_UART_LCR_EnableStickParity		Hw5
#define	DRV_UART_LCR_DisableStickParity		0
#define	DRV_UART_LCR_EnableEvenParity		Hw4
#define	DRV_UART_LCR_EnableOddParity		0
#define	DRV_UART_LCR_EnableParity			Hw3
#define	DRV_UART_LCR_DisableParity			0
#define	DRV_UART_LCR_OneStopBit				Hw2
#define	DRV_UART_LCR_TwoStopBit				0
#define	DRV_UART_LCR_WordLength_SHIFT		0
#define	DRV_UART_LCR_WordLength(X)			((X)<<DRV_UART_LCR_WordLength_SHIFT)
#define	DRV_UART_LCR_WordLength_5Bit		DRV_UART_LCR_WordLength(0)
#define	DRV_UART_LCR_WordLength_6Bit		DRV_UART_LCR_WordLength(1)
#define	DRV_UART_LCR_WordLength_7Bit		DRV_UART_LCR_WordLength(2)
#define	DRV_UART_LCR_WordLength_8Bit		DRV_UART_LCR_WordLength(3)

#define HwUART_LCR_DLAB_ON					Hw7 			// Access the divisor latches of the baud generator
#define HwUART_LCR_DLAB_OFF 				~Hw7			// Access the receiver buff, the transmitter holding register, or the interrupt enable register
#define HwUART_LCR_SB_ON					Hw6 			// The serial output is forced to the spacing (logic 0) state
#define HwUART_LCR_SB_OFF					~Hw6			// Disable the break
#define HwUART_LCR_SP_ON					Hw5 			// When bits 3, 4 and 5 are logic 1 the parity bits is transmitted and checked as a logic 0. If bits 3 and 5 are 1 and bit 4 is a logic 0 then the parity bit is transmitted and checked as a logic 1
#define HwUART_LCR_SP_OFF					~Hw5			// Disable stick parity
#define HwUART_LCR_EPS_EVEN 				Hw4 			// Generate or check even parity
#define HwUART_LCR_EPS_ODD					~Hw4			// Generate or check odd parity
#define HwUART_LCR_PEN_ON					Hw3 			// A parity bit is generated (TX) or Checked (RX)
#define HwUART_LCR_PEN_OFF					~Hw3			// Disabled
#define HwUART_LCR_STB_ONE					Hw2 			// One stop bit is generated in the transmitted data
#define HwUART_LCR_STB_WLS					~Hw2			// When 5bit word length is selected, one and a half stop bits are generated. When either a 6-, 7-, or 8-bit word length is selected, two stop bits are generated.
#define HwUART_LCR_WLS_5					~(Hw1+Hw0)		// 5 bits word length
#define HwUART_LCR_WLS_6					Hw0 			// 6 bits word length
#define HwUART_LCR_WLS_7					Hw1 			// 7 bits word length
#define HwUART_LCR_WLS_8					(Hw1+Hw0)		// 8 bits word length

#define HwUART_MCR_RS_START 				Hw6 			// nRTS is deasserted at the RX Start Condition
#define HwUART_MCR_RS_STOP					HwZERO			// nRTS is deasserted at the RX Stop Condition
#define HwUART_MCR_AFE_EN					Hw5 			// Auto Flow Control Enabled
#define HwUART_MCR_LOOP_ON					Hw4 			// Loop Back Enable
#define HwUART_MCR_LOOP_OFF 				HwZERO			// Loop Back Disable
#define HwUART_MCR_RTS						Hw1 			// This bit informs the external modem that the uart is ready to exchange data

#define HwUART_LSR_ERF_FIFO 				Hw7 			// In the FIFO mode this bit is set when there is at least one parity error, framing error or break indication in the FIFO
#define HwUART_LSR_ERF_16450				~Hw7			// In the 16450 mode
#define HwUART_LSR_TEMT_ON					Hw6 			// Transmitter holding register and the transmitter shift register are both empty
#define HwUART_LSR_TEMT_OFF 				~Hw6			//
#define HwUART_LSR_THRE_ON					Hw5 			// UART is ready to accept a new char for transmission
#define HwUART_LSR_THRE_OFF 				~Hw5			//
#define HwUART_LSR_BI_ON					Hw4 			// The received data input is held in the spacing (logic 0) state for longer than a full word transmission time
#define HwUART_LSR_BI_OFF					~Hw4			//
#define HwUART_LSR_FE_ON					Hw3 			// The received character did not have a valid stop bit
#define HwUART_LSR_FE_OFF					~Hw3			//
#define HwUART_LSR_PE_ON					Hw2 			// The received data character does not have the correct even or odd parity
#define HwUART_LSR_PE_OFF					~Hw2			//
#define HwUART_LSR_OE_ON					Hw1 			// The receiver buffer register was not read by the CPU before the next character was transferred into the receiver buffer register
#define HwUART_LSR_OE_OFF					~Hw1			//
#define HwUART_LSR_DR_ON					Hw0 			// The receiver data ready
#define HwUART_LSR_DR_OFF					~Hw0			//

#define HwUART_MSR_CTS						Hw4 			// This bit is the complement of the Claer to Send input
#define HwUART_MSR_DCTS 					Hw0 			// This bit is the Delta Clear to Send indicator

#define HwUART_SCCR_SEN_EN					Hw31			// SmartCard Interface Enable
#define HwUART_SCCR_SEN_DIS 				~Hw31			// SmartCard Interface Disable
#define HwUART_SCCR_STEN_EN 				Hw30			// SmartCard TX Enable
#define HwUART_SCCR_STEN_DIS				~Hw30			// SmartCard TX Disable
#define HwUART_SCCR_DEN_EN					Hw29			// SmartCard Clock Divider Enable
#define HwUART_SCCR_DEN_DIS 				~Hw29			// SmartCard Clock Divider Disable
#define HwUART_SCCR_P_EN					Hw28			// SmartCard Clock Post Divider Enable
#define HwUART_SCCR_P_DIS					~Hw28			// SmartCard Clock Post Divider Disable
#define HwUART_SCCR_STF 					Hw27			// SmartCard Clock TX Done Flag
#define HwUART_SCCR_STE_EN					Hw26			// SmartCard Clock TX Done Interrupt Enable
#define HwUART_SCCR_STE_DIS 				~Hw26			// SmartCard Clock TX Done Interrupt Disable
#define HwUART_SCCR_DIV(X)					((X)*Hw0)		// SmartCard Clock Divisor
#define HwUART_SCCR_DIV_MASK				(0xFFFF)			

#define HwUART_STC_SCNT(X)					((X)*Hw0)		// SmartCard Start TX Count
#define HwUART_STC_SCNT_MASK				(0xFFFF)						

#define HwUART_IER_EMSI_EN					Hw3 			// Enable Modem status interrupt
#define HwUART_IER_EMSI_DIS 				~Hw3			// Disable Modem status interrupt
#define HwUART_IER_ELSI_EN					Hw2 			// Enable receiver line status interrupt
#define HwUART_IER_ELSI_DIS 				~Hw2			// Disable receiver line status interrupt
#define HwUART_IER_ETXI_EN					Hw1 			// Enable transmitter holding register empty interrupt
#define HwUART_IER_ETXI_DIS 				~Hw1			// Disable transmitter holding register empty interrupt
#define HwUART_IER_ERXI_EN					Hw0 			// Enable received data available interrupt
#define HwUART_IER_ERXI_DIS 				~Hw0			// Disable received data available interrupt

// UART SmartCard Clock Definition
#define DRV_UART_SCCLK						4*1000*1000//40000 // 4.0 MHz
// UART SmartCard Clock Conversion Factor Definition
#define DRV_UART_SCCLK_FACTOR_0				372 //Internal Clock
#define DRV_UART_SCCLK_FACTOR_1				372
#define DRV_UART_SCCLK_FACTOR_2				558
#define DRV_UART_SCCLK_FACTOR_3				744
#define DRV_UART_SCCLK_FACTOR_4				1116
#define DRV_UART_SCCLK_FACTOR_5				1488
#define DRV_UART_SCCLK_FACTOR_6				1860
#define DRV_UART_SCCLK_FACTOR_7				372 //Reserved
#define DRV_UART_SCCLK_FACTOR_8				372 //Reserved
#define DRV_UART_SCCLK_FACTOR_9				512
#define DRV_UART_SCCLK_FACTOR_A				768
#define DRV_UART_SCCLK_FACTOR_B				1024
#define DRV_UART_SCCLK_FACTOR_C				1536
#define DRV_UART_SCCLK_FACTOR_D				2048
#define DRV_UART_SCCLK_FACTOR_E				372 //Reserved
#define DRV_UART_SCCLK_FACTOR_F				372 //Reserved
// UART SmartCard Delay Time Definition
#define DRV_UART_SC_DELAY_TIME				4000 // 4.0 ms

// UART Channel Number Definition
enum
{
	DRV_UART_CH0 = 0,
	DRV_UART_CH1,
	DRV_UART_CH2,
	DRV_UART_CH3,
	DRV_UART_CH4,
	DRV_UART_CH5,
	DRV_UART_CHMAX
};

// UART Port Number Definition
enum
{
	DRV_UART_PORT0 = 0,
	DRV_UART_PORT1,
	DRV_UART_PORT2,
	DRV_UART_PORT3,
	DRV_UART_PORT4,
	DRV_UART_PORT5,
	DRV_UART_PORT6,
	DRV_UART_PORT7,
	DRV_UART_PORTMAX
};

// UART Line Parity Definition
enum
{
	DRV_UART_PARITY_NONE = 0,
	DRV_UART_PARITY_ODD,
	DRV_UART_PARITY_EVEN,
	DRV_UART_PARITY_MAX
};

// UART Buffer Definition
enum
{
	DRV_UART_BUFFER_TX = 0,
	DRV_UART_BUFFER_RX,
	DRV_UART_BUFFER_TXRX,
	DRV_UART_BUFFER_MAX
};

// UART Smart Card Control Definition
enum
{
	DRV_UART_SCCTRL_OFF = 0,
	DRV_UART_SCCTRL_ON,
	DRV_UART_SCCTRL_MAX
};

// UART Interrupt Definition for Smart Card
enum
{
	DRV_UART_SCINT_DISABLE = 0,
	DRV_UART_SCINT_ENABLE,
	DRV_UART_SCINT_NOCHANGE,
	DRV_UART_SCINT_MAX
};

// UART Driver Data Structure
typedef struct
{
	unsigned int	BAUD;		// Baud Rate
	unsigned char	IER;		// Interrupt Enable Register
	unsigned char	FCR;		// FIFO Control Register
	unsigned char	LCR;		// Line Control Register
	unsigned char	AFT;		// Auto Flow Control Trigger Level Register
	unsigned int	Fuart;		// UART Main Clock Frequency
	unsigned int	SCCTRL;		// UART Smart Card Control
	unsigned int	SCCLK;		// UART Smart Card Clock

	unsigned char	CHSTS;		// Handle Status
	unsigned char	PORT;		// Port Number
} sDRV_UART;

// UART Driver Buffer structure for Smart Card
#define IO_UART_RX_BUFFER_SIZE		512
#define IO_UART_TX_BUFFER_SIZE		512

typedef struct
{
	unsigned short	usTXHead;
	unsigned short	usTXTail;
	unsigned short	usRXHead;
	unsigned short	usRXTail;
	unsigned char	sRX_Buffer[IO_UART_RX_BUFFER_SIZE];
	unsigned char	sTX_Buffer[IO_UART_TX_BUFFER_SIZE];
} sDRV_UART_BUF;

enum
{
	TCC_SC_IOCTL_ENABLE,
	TCC_SC_IOCTL_DISABLE,
	TCC_SC_IOCTL_ACTIVATE,
	TCC_SC_IOCTL_RESET,
	TCC_SC_IOCTL_SEND_RCV,
	TCC_SC_IOCTL_SET_PARAMS,
	TCC_SC_IOCTL_GET_PARAMS,
	TCC_SC_IOCTL_SELECT_VOL,
	TCC_SC_IOCTL_DETECT_CARD,
	TCC_SC_IOCTL_MAX
};

typedef struct
{
	unsigned char 	ucTS;
	unsigned char 	ucT0;
	unsigned short 	usTA[4];
	unsigned short 	usTB[4];
	unsigned short 	usTC[4];
	unsigned short 	usTD[4];
	unsigned char 	ucHC[15]; // Historical Characters - Max 15 Bytes
	unsigned int 	ucTCK;
} sTCC_SC_ATR_CHARS;

typedef struct
{
	unsigned char 	*pucWriteBuf;
	unsigned int 	uiWriteLength;
	unsigned char 	*pucReadBuf;
	unsigned int 	*puiReadLength;
} sTCC_SC_BUFFER;

typedef struct
{
	unsigned char 	ucProtocol;
	unsigned char 	ucF;
	unsigned char 	ucD;
	unsigned char 	ucN;
	unsigned char 	ucCWI;
	unsigned char 	ucBWI;
	unsigned int 	uiClock;
	unsigned int 	uiBaudrate;
} sTCC_SC_PARAMS;

#define TA_CHECK_BIT	0x10
#define TB_CHECK_BIT	0x20
#define TC_CHECK_BIT	0x40
#define TD_CHECK_BIT	0x80

typedef struct
{
	unsigned char ucNAD;
	unsigned char ucPCB;
	unsigned char ucLEN;
	unsigned char *pcINF;
	unsigned char ucEDC[2];
} sTCC_BLOCK_FRAME;


#define NAD_SAD_MASK		0x07
#define NAD_SAD_SHIFT		0
#define NAD_DAD_MASK		0x70
#define NAD_DAD_SHIFT		4

#define GET_NAD_SAD(x)		(x & NAD_SAD_MASK)>>NAD_SAD_SHIFT
#define GET_NAD_DAD(x)		(x & NAD_DAD_MASK)>>NAD_DAD_SHIFT
#define GET_NAD_VPP1(x)		(x & 0x80)>>7
#define GET_NAD_VPP2(x)		(x & 0x08)>>3

#endif 
