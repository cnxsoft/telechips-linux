/****************************************************************************
 *   FileName    : dev_gpsb.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#ifndef	_DEV_GPSB_
#define	_DEV_GPSB_

#include <mach/bsp.h>

#ifndef _U8_
#define _U8_
typedef unsigned char			U8;
#endif
#ifndef _U16_
#define _U16_
typedef unsigned short int		U16;
#endif
#ifndef S32
	typedef signed int S32;
#endif
#ifndef _U32_
#define _U32_
typedef unsigned int			U32;
#endif
#ifndef _BOOL_
#define _BOOL_
typedef unsigned int			BOOL;
#endif

#ifndef hword_of
#define	hword_of(X)					( *(volatile unsigned short *)((X)) )
#endif

#ifndef byte_of
#define	byte_of(X)					( *(volatile unsigned char *)((X)) )
#endif
#ifndef word_of
#define	word_of(X)					( *(volatile unsigned int *)((X)) )
#endif

#ifndef TCC92XX
#define TCC92XX
#endif//

/*==============================================================================
				GPSB
  ==============================================================================*/

#define	DRV_GPSB_MAX_HANDLE		10

// GPSB Driver Function Definition
enum
{
	DRV_GPSB_FUNC_INIT,					// GPSB Driver Initialization : Argument = None
	DRV_GPSB_FUNC_PROCESS_INTERRUPT,	// Process GPSB Interrupt : Argument = None

	DRV_GPSB_FUNC_OPEN,					// Open Handle : Argument = (Channel Number, Port Number, *sDRV_GPSB)
	DRV_GPSB_FUNC_INSTALL_HANDLER,		// Install Handler : Argument = (Channel, NewHandler, [* OldHandler])
	DRV_GPSB_FUNC_UNINSTALL_HANDLER,	// Install Handler : Argument = (Channel)

	DRV_GPSB_FUNC_GETHwGPSB,			// Get HwGPSB Base Address : Argument = (Handle, *sHwGPSB)
	DRV_GPSB_FUNC_SETCFG,				// Configurate GPSB : Argument = (Handle, MODE, SCKFREQ, SWCTRL, SDOSDI)
	DRV_GPSB_FUNC_SENDDATA,				// Send Data using GPSB Channel : Argument = (Handle, Cmd, *Buffer, Length, BitWidth)
	DRV_GPSB_FUNC_RCVDATA,				// Receive Data using GPSB Channel : Argument = (Handle, Cmd, *Buffer, Length, BitWidth)
	DRV_GPSB_FUNC_UPDATEPID,			// Update PID Table = (Handle, *PIDTable)
	DRV_GPSB_FUNC_OPENDMA,				// Set Registers for DMA Operation : Argument = (Handle)
	DRV_GPSB_FUNC_CLOSEDMA,				// Set Registers for Normal Operation : Argument = (Handle)
	DRV_GPSB_FUNC_STARTDMA,				// Set Registers for Starting DMA Operation : Argument = (Handle, DMADir, PktSize, PktNum, *TxBuf, *RxBuf, FIFOThr, ContMode)
	DRV_GPSB_FUNC_STOPDMA,				// Set Registers for Stopping DMA Operation : Argument = (Handle)

	DRV_GPSB_FUNC_START,				// Start Handle : Argument = (Handle)
	DRV_GPSB_FUNC_STOP,					// Stop Handle : Argument = (Handle)
	DRV_GPSB_FUNC_CLOSE,				// Close Handle : Argument = (Handle)
	
	DRV_GPSB_FUNC_MAX
};

// GPSB Driver Status Definition
enum
{
	DRV_GPSB_STATUS_INVALID,	// Not Opened
	DRV_GPSB_STATUS_IDLE,		// Openad but Not Activated
	DRV_GPSB_STATUS_PAUSE,		// Opened & Activated & Pause
	DRV_GPSB_STATUS_ACTIVE,		// Opened & Activated
	DRV_GPSB_STATUS_MAX
};

// GPSB DMA Mode Information Structure
typedef struct
{
	U8			DMAMode;		// DMA Mode - Normal, MPEG2-TS			
	U8			PIDMatch;		// PID Match Control - 0: Disable, 1: Enable
	U8			SyncMatch;		// Sync Byte Match Control - 0: Disable, 1: Enable			
	U8			PIDNum;			// Number of PID
} sGPSB_DMA;

// GPSB Driver Data Structure
typedef struct
{
	U32			MODE;			// Mode Register
	U32			SCKFREQ;		// SCK Frequency
	U8			SWCTRL;			// Control Setting
	U8			SDOSDI;			// Output/Input Use Setting
	U8			CHSTS;			// Handle Status
	U8			CHANNEL;		// Channel Number
	U8			PORT;			// Port Number
	sGPSB_DMA	DMAMODE;		// DMA Mode Information
	GPSB		*pHwGPSB;		// Base Address
} sDRV_GPSB;

typedef struct
{
	volatile unsigned long *BASE;
	unsigned	uFRM, uSCK, uSDI, uSDO;
	unsigned	uMODE;
} sGPSBPORT;


// GPSB Driver Return Value Definition
#define	DRV_GPSB_ERROR_YES		1
#define	DRV_GPSB_ERROR_NO		0
#define	DRV_GPSB_ERROR_OK		0
enum
{
	// Error for Driver State
	DRV_GPSB_ERROR_NOTINIT	= (int)0xF0057400,	// Non-init Function come before init function
	DRV_GPSB_ERROR_DUPINIT,						// Init function called more than twice

	// Error for Input Parameter
	DRV_GPSB_ERROR_INVALID_FUNC,				// Function Code is out of range
	DRV_GPSB_ERROR_INVALID_ARGUMENT,			// Number of Argument is out of range
	DRV_GPSB_ERROR_INVALID_HANDLE,				// Handle value is out of range
	DRV_GPSB_ERROR_INVALID_CH,					// Channel value is out of range

	// Error for Resource Availability
	DRV_GPSB_ERROR_NOTAVAILABLE_HANDLE,			// No available handle
	DRV_GPSB_ERROR_NOTAVAILABLE_CH,				// No available channel

	// Error for Illegal State
	DRV_GPSB_ERROR_INTERNAL,					// Internal Bug

	DRV_GPSB_ERROR_MAX
};

// GPSB Channel Control Method Definition
enum
{
	DRV_GPSB_CTRL_HW = 0,
	DRV_GPSB_CTRL_SW,
	DRV_GPSB_CTRL_MAX
};
#define	DRV_GPSB_SwCtrlMode(X)		((X)&0x3F)

// GPSB Tx/Rx Swap Mode Definition
#define	DRV_GPSB_SWAP_TX		Hw6
#define	DRV_GPSB_SWAP_RX		Hw7

#define	DRV_GPSB_SwapMode(X)		((X) & (DRV_GPSB_SWAP_TX | DRV_GPSB_SWAP_RX))
#define	DRV_GPSB_SwapMode_Tx(X)	(((X) & DRV_GPSB_SWAP_TX) ? 1 : 0)
#define	DRV_GPSB_SwapMode_Rx(X)	(((X) & DRV_GPSB_SWAP_RX) ? 1 : 0)

// GPSB Channel Master/Slave Definition
enum
{
	DRV_GPSB_MODE_MASTER = 0,
	DRV_GPSB_MODE_SLAVE,
	DRV_GPSB_MODE_MAX
};

// GPSB Channel I/O Definition
enum
{
	DRV_GPSB_SDOSDI_NOUSE = 0,
	DRV_GPSB_SDOSDI_SDI,
	DRV_GPSB_SDOSDI_SDO,
	DRV_GPSB_SDOSDI_ALLUSE,
	DRV_GPSB_SDOSDI_MAX
};

// GPSB Channel Number Definition
enum
{
	DRV_GPSB_CH0 = 0,
	DRV_GPSB_CH1,
	DRV_GPSB_CH2,
	DRV_GPSB_CH3,
	DRV_GPSB_CH4,
	DRV_GPSB_CH5,
	DRV_GPSB_CHMAX
};

// GPSB Port Number Definition
enum
{
	DRV_GPSB_PORT0 = 0,
	DRV_GPSB_PORT1,
	DRV_GPSB_PORT2,
	DRV_GPSB_PORT3,
	DRV_GPSB_PORT4,
	DRV_GPSB_PORT5,
	DRV_GPSB_PORT6,
	DRV_GPSB_PORT7,
	DRV_GPSB_PORT8,
	DRV_GPSB_PORT9,
	DRV_GPSB_PORT10,
	DRV_GPSB_PORT11,
	DRV_GPSB_PORT12,
	DRV_GPSB_PORTMAX
};

// GPSB DMA Mode Definition
enum
{
	DRV_GPSB_DMAMODE_NOR = 0,
	DRV_GPSB_DMAMODE_TS,
	DRV_GPSB_DMAMODE_MAX
};

// GPSB DMA Transfer Direction Definition
enum
{
	DRV_GPSB_DMA_RECEIVE = 0,
	DRV_GPSB_DMA_TRANSMIT,
	DRV_GPSB_DMA_MAX
};

// GPSB Driver Channel Mode Definition
#define	DRV_GPSB_MODE_Divisor_SHIFT			24
#define	DRV_GPSB_MODE_Divisor(X)			((X)<<DRV_GPSB_MODE_Divisor_SHIFT)
#define	DRV_GPSB_MODE_MRecoverTime_2SCKO	Hw23
#define	DRV_GPSB_MODE_MRecoverTime_SCKO		0
#define	DRV_GPSB_MODE_MHoldTime_2SCKO		Hw22
#define	DRV_GPSB_MODE_MHoldTime_SCKO		0
#define	DRV_GPSB_MODE_MSetupTime_2SCKO		Hw21
#define	DRV_GPSB_MODE_MSetupTime_SCKO		0
#define	DRV_GPSB_MODE_CSActiveHigh			Hw20
#define	DRV_GPSB_MODE_CSActiveLow			0
#define	DRV_GPSB_MODE_CMDActiveHigh			Hw19
#define	DRV_GPSB_MODE_CMDActiveLow			0
#define	DRV_GPSB_MODE_TransRisingEdge		Hw18
#define	DRV_GPSB_MODE_TransFallingEdge		0
#define	DRV_GPSB_MODE_RcvRisingEdge			Hw17
#define	DRV_GPSB_MODE_RcvFallingEdge		0
#define	DRV_GPSB_MODE_SCKPolCtrl_High		Hw16
#define	DRV_GPSB_MODE_SCKPolCtrl_Low		0
#define	DRV_GPSB_MODE_ClearRcvFifoCount		Hw15
#define	DRV_GPSB_MODE_ClearTransFifoCount	Hw14
#define	DRV_GPSB_MODE_BitWidth_SHIFT		8
#define	DRV_GPSB_MODE_BitWidth(X)			((X)<<DRV_GPSB_MODE_BitWidth_SHIFT)
#define	DRV_GPSB_MODE_DataShiftLSBFirst		Hw7
#define	DRV_GPSB_MODE_DataShiftMSBFirst		0
#define	DRV_GPSB_MODE_EnableLoopBack		Hw6
#define	DRV_GPSB_MODE_DisableLoopBack		0
#define	DRV_GPSB_MODE_EnableContTrans		Hw4
#define	DRV_GPSB_MODE_DisableContTrans		0
#define	DRV_GPSB_MODE_EnableOperation		Hw3
#define	DRV_GPSB_MODE_DisableOperation		0
#define	DRV_GPSB_MODE_SlaveMode				Hw2
#define	DRV_GPSB_MODE_MasterMode			0
#define	DRV_GPSB_MODE_OpMode_SHIFT			0
#define	DRV_GPSB_MODE_OpMode(X)				((X)<<DRV_GPSB_MODE_OpMode_SHIFT)
#define	DRV_GPSB_MODE_OpMod_SPI				DRV_GPSB_MODE_OpMode(0)
#define	DRV_GPSB_MODE_OpMod_SSP				DRV_GPSB_MODE_OpMode(1)

// GPSB PID Table Definition
#define	DRV_GPSB_PID_MAX_NUM				32
#define	DRV_GPSB_PID_VALUE_MASK				0x1FFF


extern unsigned	IO_GPSB_TxRxData(unsigned uCH, void *pTxBuf, void *pRxBuf, unsigned uLength);

extern void IO_GPSB_InitChannel(unsigned uCH, unsigned uPort, unsigned uMode, unsigned uSwCtrl, unsigned uSDOSDI);

extern GPSB *IO_GPSB_GetBaseAddr(unsigned uCH);

extern S32	DRV_GPSB(U32 Func, U32 ArgN, ...);

extern void IO_GPSB_DisableChannel(unsigned uCH);

#endif

/* end of file */

