/****************************************************************************
 *   FileName    : TCC88xx_Structures.h
 *   Description :
 ****************************************************************************
*
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
*
 ****************************************************************************/

/************************************************************************
*   TCC88xx Internal Register Definition File
************************************************************************/
#ifndef __TCC88xxSTRUCTURES_H__
#define __TCC88xxSTRUCTURES_H__

/*******************************************************************************
*
*    TCC88x DataSheet PART 2 SMU & PMU
*
********************************************************************************/
/************************************************************************
*   1. CKC (Base Addr = 0xF0400000)
************************************************************************/
//#define HwCKC_BASE									((volatile unsigned long*)0xF0400000)
typedef struct _CKC {
	volatile unsigned int	CLK0CTRL;		//0x000			RW	0x14000000
	volatile unsigned int	CLK1CTRL;		//0x004			RW	0x14000000
	volatile unsigned int	CLK2CTRL;		//0x008			RW	0x14000000
	volatile unsigned int	CLK3CTRL;		//0x00C			RW	0x14000000
	volatile unsigned int	CLK4CTRL;		//0x010			RW	0x14000000
	volatile unsigned int	CLK5CTRL;		//0x014			RW	0x14000000
	volatile unsigned int	CLK6CTRL;		//0x018			RW	0x14000000
	volatile unsigned int	CLK7CTRL;		//0x01C			RW	0x14000000
	volatile unsigned int	PLL0CFG;		//0x020			RW	0x14000000
	volatile unsigned int	PLL1CFG;		//0x024			RW	0x14000000
	volatile unsigned int	PLL2CFG;		//0x028			RW	0x14000000
	volatile unsigned int	PLL3CFG;		//0x02C			RW	0x14000000
	volatile unsigned int	CLKDIVC0;		//0x030			RW	0x14000000
	volatile unsigned int	CLKDIVC1;		//0x034			RW	0x14000000
	volatile unsigned int	CLKDIVC2;		//0x038			RW	0x14000000
	volatile unsigned int	Reserved0[2];	//0x03C	~ 0x43	RW	0x14000000
	volatile unsigned int	SWRESET;		//0x044			RW	0x14000000
	volatile unsigned int	CLK8CTRL;		//0x048			RW	0x14000000
	volatile unsigned int	CLK9CTRL;		//0x04C			RW	0x14000000
	volatile unsigned int	PLL4CFG;		//0x050			RW	0x14000000
	volatile unsigned int	PLL5CFG;		//0x054			RW	0x14000000
	volatile unsigned int	Reserved1[10];	//0x058~0x7F	RW	0x14000000
	volatile unsigned int	PCLK_TCX;		//0x080			RW	0x14000000
	volatile unsigned int	PCLK_TCT;		//0x084			RW	0x14000000
	volatile unsigned int	PCLK_TCZ;		//0x088			RW	0x14000000
	volatile unsigned int	PCLK_LCD0;		//0x08C			RW	0x14000000
	volatile unsigned int	PCLK_LCD1;		//0x090			RW	0x14000000
	volatile unsigned int	PCLK_LCDSI0;	//0x094			RW	0x14000000
	volatile unsigned int	PCLK_LCDSI1;	//0x098			RW	0x14000000
	volatile unsigned int	PCLK_HDMIA;		//0x09C			RW	0x14000000
	volatile unsigned int	PCLK_DSI;		//0x0A0			RW	0x14000000
	volatile unsigned int	Reserved2[1];	//0x0A4			RW	0x14000000
	volatile unsigned int	PCLK_HDMI;		//0x0A8			RW	0x14000000
	volatile unsigned int	PCLK_USB11H;	//0x0AC			RW	0x14000000
	volatile unsigned int	PCLK_SDMMC0;	//0x0B0			RW	0x14000000
	volatile unsigned int	PCLK_MSTICK;	//0x0B4			RW	0x14000000
	volatile unsigned int	PCLK_I2C0;		//0x0B8			RW	0x14000000
	volatile unsigned int	PCLK_UART0;		//0x0BC			RW	0x14000000
	volatile unsigned int	PCLK_UART1;		//0x0C0			RW	0x14000000
	volatile unsigned int	PCLK_UART2;		//0x0C4			RW	0x14000000
	volatile unsigned int	PCLK_UART3;		//0x0C8			RW	0x14000000
	volatile unsigned int	PCLK_UART4;		//0x0CC			RW	0x14000000
	volatile unsigned int	PCLK_UART5;		//0x0D0			RW	0x14000000
	volatile unsigned int	PCLK_GPSB0;		//0x0D4			RW	0x14000000
	volatile unsigned int	PCLK_GPSB1;		//0x0D8			RW	0x14000000
	volatile unsigned int	PCLK_GPSB2;		//0x0DC			RW	0x14000000
	volatile unsigned int	PCLK_GPSB3;		//0x0E0			RW	0x14000000
	volatile unsigned int	PCLK_GPSB4;		//0x0E4			RW	0x14000000
	volatile unsigned int	PCLK_GPSB5;		//0x0E8			RW	0x14000000
	volatile unsigned int	PCLK_ADC;		//0x0EC			RW	0x14000000
	volatile unsigned int	PCLK_SPDIF;		//0x0F0			RW	0x14000000
	volatile unsigned int	PCLK_EHI0;		//0x0F4			RW	0x14000000
	volatile unsigned int	PCLK_EHI1;		//0x0F8			RW	0x14000000
	volatile unsigned int	PCLK_AUD0;		//0x0FC			RW	0x14000000
	volatile unsigned int	PCLK_PDM;		//0x100			RW	0x14000000
	volatile unsigned int	PCLK_SDMMC1;	//0x104			RW	0x14000000
	volatile unsigned int	PCLK_SDMMC2;	//0x108			RW	0x14000000
	volatile unsigned int	PCLK_SDMMC3;	//0x10C			RW	0x14000000
	volatile unsigned int	PCLK_DAI0;		//0x110			RW	0x14000000
	volatile unsigned int	PCLK_DAI1;		//0x114			RW	0x14000000
	volatile unsigned int	PCLK_DAI2;		//0x118			RW	0x14000000
	volatile unsigned int	PCLK_I2C1;		//0x11C			RW	0x14000000
	volatile unsigned int	PCLK_PCIE;		//0x120			RW	0x14000000
	volatile unsigned int	PCLK_SATAH0;	//0x124			RW	0x14000000
	volatile unsigned int	PCLK_SATAH1;	//0x128			RW	0x14000000
	volatile unsigned int	PCLK_SATAD;		//0x12C			RW	0x14000000
	volatile unsigned int	PCLK_USB20H;	//0x130			RW	0x14000000
	volatile unsigned int	PCLK_GMAC;		//0x134			RW	0x14000000
	volatile unsigned int	PCLK_CIFMC;		//0x138			RW	0x14000000
	volatile unsigned int	PCLK_CIFSC;		//0x13C			RW	0x14000000
	volatile unsigned int	PCLK_ISPJ;		//0x140			RW	0x14000000
	volatile unsigned int	PCLK_ISPS;		//0x144			RW	0x14000000
	volatile unsigned int	PCLK_FILTER;	//0x148			RW	0x14000000
	volatile unsigned int	Reserved3[3];	//0x14C ~ 0x157	RW	0x14000000
	volatile unsigned int	PCLK_OUT0;		//0x158			RW	0x14000000
	volatile unsigned int	PCLK_OUT1;		//0x15C			RW	0x14000000
} CKC, *PCKC;

/************************************************************************
*   2. VPIC(Vectored Priority Interrupt Controller) (Base Addr = 0xF0403000)
************************************************************************/
//#define HwVPIC_BASE                                 ((volatile unsigned long*)0xF0403000)
//#define HwPRIOINTRCTR_BASE                          ((volatile unsigned long*)0xF0403000)
//#define HwVECTINTRCTR_BASE                          ((volatile unsigned long*)0xF0403200)

typedef struct _PIC{
    volatile unsigned int   IEN0;               // 0x000  R/W  0x00000000  Interrupt Enable0 Register
    volatile unsigned int   IEN1;               // 0x004  R/W  0x00000000  Interrupt Enable1 Register
    volatile unsigned int   CLR0;               // 0x008  R/W  0x00000000  Interrupt Clear0 Register
    volatile unsigned int   CLR1;               // 0x00C  R/W  0x00000000  Interrupt Clear1 Register
    volatile unsigned int   STS0;               // 0x010  R    Unknown     Interrupt Status0 Register
    volatile unsigned int   STS1;               // 0x014  R    Unknown     Interrupt Status1 Register
    volatile unsigned int   SEL0;               // 0x018  R/W  0x00000000  IRQ or FIR Selection0 Register
    volatile unsigned int   SEL1;               // 0x01C  R/W  0x00000000  IRQ or FIR Selection1 Register
    volatile unsigned int   SRC0;               // 0x020  R    Unknown     Source Interrupt Status0 Register
    volatile unsigned int   SRC1;               // 0x024  R    Unknown     Source Interrupt Status1 Register
    volatile unsigned int   MSTS0;              // 0x028  R    0x00000000  Masked Status0 Register
    volatile unsigned int   MSTS1;              // 0x02C  R    0x00000000  Masked Status1 Register
    volatile unsigned int   TIG0;               // 0x030  R/W  0x00000000  Test Interrupt Generation0 Register
    volatile unsigned int   TIG1;               // 0x034  R/W  0x00000000  Test Interrupt Generation1 Register
    volatile unsigned int   POL0;               // 0x038  R/W  0x00000000  Interrupt Polarity0 Register
    volatile unsigned int   POL1;               // 0x03C  R/W  0x00000000  Interrupt Polarity1 Register
    volatile unsigned int   IRQ0;               // 0x040  R    0x00000000  IRQ Raw Status0 Register
    volatile unsigned int   IRQ1;               // 0x044  R    0x00000000  IRQ Raw Status1 Register
    volatile unsigned int   FIQ0;               // 0x048  R    Unknown     FIQ Status0 Register
    volatile unsigned int   FIQ1;               // 0x04C  R    Unknown     FIQ Status1 Register
    volatile unsigned int   MIRQ0;              // 0x050  R    0x00000000  Masked IRQ Status0 Register
    volatile unsigned int   MIRQ1;              // 0x054  R    0x00000000  Masked IRQ Status1 Register
    volatile unsigned int   MFIQ0;              // 0x058  R    0x00000000  Masked FIQ Status0 Register
    volatile unsigned int   MFIQ1;              // 0x05C  R    0x00000000  Masked FIQ Status1 Register
    volatile unsigned int   MODE0;              // 0x060  R/W  0x00000000  Trigger Mode0 Register ? Level or Edge
    volatile unsigned int   MODE1;              // 0x064  R/W  0x00000000  Trigger Mode1 Register ? Level or Edge
    volatile unsigned int   SYNC0;              // 0x068  R/W  0xFFFFFFFF  Synchronization Enable0 Register
    volatile unsigned int   SYNC1;              // 0x06C  R/W  0xFFFFFFFF  Synchronization Enable1 Register
    volatile unsigned int   WKEN0;              // 0x070  R/W  0x00000000  Wakeup Event Enable0 Register
    volatile unsigned int   WKEN1;              // 0x074  R/W  0x00000000  Wakeup Event Enable1 Register
    volatile unsigned int   MODEA0;             // 0x078  R/W  0x00000000  Both Edge or Single Edge0 Register
    volatile unsigned int   MODEA1;             // 0x07C  R/W  0x00000000  Both Edge or Single Edge1 Register
    volatile unsigned int   EI37SEL;             // 0x080  R/W  0x00000000  External Interrupt 3~7 Selection   
    volatile unsigned int   NOTDEFINE0[31];     //-  0x84-0xFC             Reserved
    volatile unsigned int   INTMSK0;            //  0x100  R/W  0xFFFFFFFF  Interrupt Output Masking0 Register
    volatile unsigned int   INTMSK1;            //  0x104  R/W  0xFFFFFFFF  Interrupt Output Masking1 Register
    volatile unsigned int   ALLMSK;             //  0x108  R/W  0x00000003  All Mask Register
} PIC, *PPIC;

typedef struct _VIC{
    volatile unsigned int   VAIRQ;              // 0x200  R    0x800000XX  IRQ Vector Register
    volatile unsigned int   VAFIQ;              // 0x204  R    0x800000XX  FIQ Vector Register
    volatile unsigned int   VNIRQ;              // 0x208  R    0x800000XX  IRQ Vector Number Register
    volatile unsigned int   VNFIQ;              // 0x20C  R    0x800000XX  FIQ Vector Number Register
    volatile unsigned int   VCTRL;              // 0x210  R/W  0x00000000  Vector Control Register
    volatile unsigned int   NOTDEFINE0[3];      // 0x214-0x218-0x21c       Reserved
    volatile unsigned int   PRIO0;              // 0x220  R/W  0x03020100  Priorities for Interrupt 0 ~ 3
    volatile unsigned int   PRIO1;              // 0x224  R/W  0x07060504  Priorities for Interrupt 4 ~ 7
    volatile unsigned int   PRIO2;              // 0x228  R/W  0x0B0A0908  Priorities for Interrupt 8 ~ 11
    volatile unsigned int   PRIO3;              // 0x22C  R/W  0x0F0E0D0C  Priorities for Interrupt 12 ~ 15
    volatile unsigned int   PRIO4;              // 0x230  R/W  0x13121110  Priorities for Interrupt 16 ~ 19
    volatile unsigned int   PRIO5;              // 0x234  R/W  0x17161514  Priorities for Interrupt 20 ~ 23
    volatile unsigned int   PRIO6;              // 0x238  R/W  0x1B1A1918  Priorities for Interrupt 24 ~ 27
    volatile unsigned int   PRIO7;              // 0x23C  R/W  0x1F1E1D1C  Priorities for Interrupt 28 ~ 31
    volatile unsigned int   PRIO8;              // 0x240  R/W  0x23222120  Priorities for Interrupt 32 ~ 35
    volatile unsigned int   PRIO9;              // 0x244  R/W  0x27262524  Priorities for Interrupt 36 ~ 39
    volatile unsigned int   PRIO10;             // 0x248  R/W  0x2B2A2928  Priorities for Interrupt 40 ~ 43
    volatile unsigned int   PRIO11;             // 0x24C  R/W  0x2F2E2D2C  Priorities for Interrupt 44 ~ 47
    volatile unsigned int   PRIO12;             // 0x250  R/W  0x33323130  Priorities for Interrupt 48 ~ 51
    volatile unsigned int   PRIO13;             // 0x254  R/W  0x37363534  Priorities for Interrupt 52 ~ 55
    volatile unsigned int   PRIO14;             // 0x258  R/W  0x3B3A3938  Priorities for Interrupt 56 ~ 59
    volatile unsigned int   PRIO15;             // 0x25C  R/W  0x3F3E3D3C  Priorities for Interrupt 60 ~ 63
} VIC, *PVIC;

/************************************************************************
*   3. Timer/Counter (Base Addr = 0xF0403000)
************************************************************************/
//#define HwTMR_BASE									((volatile unsigned long*)0xF0403000)
typedef struct _TIMER{
    volatile unsigned int   TCFG0;         // 0x00  R/W  0x00    Timer/Counter 0 Configuration Register
    volatile unsigned int   TCNT0;         // 0x04  R/W  0x0000  Timer/Counter 0 Counter Register
    volatile unsigned int   TREF0;         // 0x08  R/W  0xFFFF  Timer/Counter 0 Reference Register
    volatile unsigned int   TMREF0;        // 0x0C  R/W  0x0000  Timer/Counter 0 Middle Reference Register
    volatile unsigned int   TCFG1;         // 0x10  R/W  0x00    Timer/Counter 1 Configuration Register
    volatile unsigned int   TCNT1;         // 0x14  R/W  0x0000  Timer/Counter 1 Counter Register
    volatile unsigned int   TREF1;         // 0x18  R/W  0xFFFF  Timer/Counter 1 Reference Register
    volatile unsigned int   TMREF1;        // 0x1C  R/W  0x0000  Timer/Counter 1 Middle Reference Register
    volatile unsigned int   TCFG2;         // 0x20  R/W  0x00    Timer/Counter 2 Configuration Register
    volatile unsigned int   TCNT2;         // 0x24  R/W  0x0000  Timer/Counter 2 Counter Register
    volatile unsigned int   TREF2;         // 0x28  R/W  0xFFFF  Timer/Counter 2 Reference Register
    volatile unsigned int   TMREF2;        // 0x2C  R/W  0x0000  Timer/Counter 2 Middle Reference Register
    volatile unsigned int   TCFG3;         // 0x30  R/W  0x00    Timer/Counter 3 Configuration Register
    volatile unsigned int   TCNT3;         // 0x34  R/W  0x0000  Timer/Counter 3 Counter Register
    volatile unsigned int   TREF3;         // 0x38  R/W  0xFFFF  Timer/Counter 3 Reference Register
    volatile unsigned int   TMREF3;        // 0x3C  R/W  0x0000  Timer/Counter 3 Middle Reference Register
    volatile unsigned int   TCFG4;         // 0x40  R/W  0x00    Timer/Counter 4 Configuration Register
    volatile unsigned int   TCNT4;         // 0x44  R/W  0x00000 Timer/Counter 4 Counter Register
    volatile unsigned int   TREF4;         // 0x48  R/W  0xFFFFF Timer/Counter 4 Reference Register
    volatile unsigned int   NOTDEFINE0;
    volatile unsigned int   TCFG5;         // 0x50  R/W  0x00    Timer/Counter 5 Configuration Register
    volatile unsigned int   TCNT5;         // 0x54  R/W  0x00000 Timer/Counter 5 Counter Register
    volatile unsigned int   TREF5;         // 0x58  R/W  0xFFFFF Timer/Counter 5 Reference Register
    volatile unsigned int   NOTDEFINE1;
    volatile unsigned int   TIREQ;         // 0x60  R/W  0x0000  Timer/Counter n Interrupt Request Register
    volatile unsigned int   NOTDEFINE2[3];
    volatile unsigned int   TWDCFG;        // 0x70  R/W  0x0000  Reserved
    volatile unsigned int   TWDCLR;        // 0x74  W  -  Reserved
    volatile unsigned int   NOTDEFINE3[2];
    volatile unsigned int   TC32EN;        // 0x80  R/W  0x00007FFF  32-bit Counter Enable / Pre-scale Value
    volatile unsigned int   TC32LDV;       // 0x84  R/W  0x00000000  32-bit Counter Load Value
    volatile unsigned int   TC32CMP0;      // 0x88  R/W  0x00000000  32-bit Counter Match Value 0
    volatile unsigned int   TC32CMP1;      // 0x8C  R/W  0x00000000  32-bit Counter Match Value 1
    volatile unsigned int   TC32PCNT;      // 0x90  R/W  -           32-bit Counter Current Value (pre-scale counter)
    volatile unsigned int   TC32MCNT;      // 0x94  R/W  -           32-bit Counter Current Value (main counter)
    volatile unsigned int   TC32IRQ;       // 0x98  R/W  0x0000----  32-bit Counter Interrupt Control
} TIMER, *PTIMER;

typedef struct _TIMERN{
    volatile unsigned int   TCFG;          // 0x000   R/W  Timer/Counter Configuration Register
    volatile unsigned int   TCNT;          // 0x004   R/W  Timer/Counter Counter Register
    volatile unsigned int   TREF;          // 0x008   R/W  Timer/Counter Reference Register
    volatile unsigned int   TMREF;         // 0x00C   R/W  Timer/Counter Middle Reference Register
} TIMERN, *PTIMERN;


/************************************************************************
*   4. PMU(Power Management Unit) (Base Addr = )
************************************************************************/
//#define HwPMU_BASE									((volatile unsigned long*)0xF0404000)

typedef struct _PMU{
    volatile unsigned int   CONTROL;                //  0x00 R/W   PMU Control Register
    volatile unsigned int   WKUPEN0;                //  0x04 R/W   Wakeup Enable Configuration Register
    volatile unsigned int   WKUPPOL0;               //  0x08 R/W   Wakeup Polarity Configuration Register
    volatile unsigned int   WATCHDOG;               //  0x0C R/W   Watchdog Control Register
    volatile unsigned int   CONFIG0;                //  0x10 R/W   Boot Configuration Register
    volatile unsigned int   CONFIG1;                //  0x14 R/W   Remap & Status Register
    volatile unsigned int   PWROFF;                 //  0x18 R/W   Power-Off Control Register
    volatile unsigned int   PWRCFG;                 //  0x1C R/W   PMU Configuration
    volatile unsigned int   PWRCNT;                 //  0x20 R/W   Power Up/Down Sequence Counter
    volatile unsigned int   WKUPSTS0;               //  0x24 R     Wakeup Status0
    volatile unsigned int   WKUPSTS1;               //  0x28 R     Wakeup Status1
    volatile unsigned int   WKUPEN1;                //  0x2C R/W   Wakeup Enable Configuration1 Register
    volatile unsigned int   WKUPPOL1;               //  0x30 R/W   Wakeup Polarity Configuration1 Register
    volatile unsigned int   WKUPSTS_ALL0;           //  0x34 R     Wakeup Status All0
    volatile unsigned int   WKUPSTS_ALL1;           //  0x38 R     Wakeup Status All1
    volatile unsigned int   MEM_PDN;                //  0x3C R/W   Memory Power Down Control Register
    volatile unsigned int   MEM_SLN;                //  0x40 R/W   Memory Sleep Control Register
} PMU, *PPMU;

/************************************************************************
*   5. SMU_I2C (Base Addr = )
************************************************************************/
//#define HwSMUI2C_BASE								((volatile unsigned long*)0xF0405000)
//#define HwSMUI2C_MASTER0_BASE						((volatile unsigned long*)0xF0405000)
//#define HwSMUI2C_MASTER1_BASE						((volatile unsigned long*)0xF0405040)
//#define HwSMUI2C_COMMON_BASE						((volatile unsigned long*)0xF0405080)

typedef struct _SMUI2CMASTER{
    volatile unsigned int  PRES;                // 0x00 R/W 0xFFFF Clock Prescale register
    volatile unsigned int  CTRL;                // 0x04 R/W 0x0000 Control Register
    volatile unsigned int  TXR;                 // 0x08 W   0x0000 Transmit Register
    volatile unsigned int  CMD;                 // 0x0C W   0x0000 Command Register
    volatile unsigned int  RXR;                 // 0x10 R   0x0000 Receive Register
    volatile unsigned int  SR;                  // 0x14 R   0x0000 Status register
    volatile unsigned int  TIME;                // 0x18 R/W 0x0000 Timing Control Register
} SMUI2CMASTER, *PSMUI2CMASTER;

typedef struct _SMUI2CICLK{
    volatile unsigned int  ICLK;                // 0x00 R/W 0xFFFF Clock Prescale register
} SMUI2CICLK, *PSMUI2CICLK;

/************************************************************************
*   6. GPIO & Port Multiplexing (Base Addr = )
************************************************************************/
typedef struct _GPIO{
    volatile unsigned int   GPADAT;                 //   0x000  R/W  0x00000000  GPA Data Register
    volatile unsigned int   GPAEN;                  //   0x004  R/W  0x00000000  GPA Output Enable Register
    volatile unsigned int   GPASET;                 //   0x008  W  -  OR function on GPA Output Data
    volatile unsigned int   GPACLR;                 //   0x00C  W  -  BIC function on GPA Output Data
    volatile unsigned int   GPAXOR;                 //   0x010  W  -  XOR function on GPA Output Data
    volatile unsigned int   GPACD0;                 //   0x014  W  0x55555555  Driver strength Control 0 on GPA Output Data
    volatile unsigned int   GPACD1;                 //   0x018  W  0x00000000  Driver strength Control 1 on GPA Output Data
    volatile unsigned int   GPAPD0;                 //   0x01C  W  0x55555555  Pull-Up/Down function on GPA Output Data
    volatile unsigned int   GPAPD1;                 //   0x020  W  0x00000000  Pull-Up/Down function on GPA Output Data
    volatile unsigned int   GPAFN0;                 //   0x024  W  0x00000000  Port Configuration on GPA Output Data
    volatile unsigned int   GPAFN1;                 //   0x028  W  0x00000000  Port Configuration on GPA Output Data
    volatile unsigned int   GPAFN2;                 //   0x02C  W  0x00000000  Port Configuration on GPA Output Data
    volatile unsigned int   GPAFN3;                 //   0x030  W  0x00000000  Port Configuration on GPA Output Data
    volatile unsigned int   GPAIEN;                 //   0x034  R/W  0x00000000 GPA Input Enable Register
    volatile unsigned int   GPAIS;                  //   0x038  R/W  0x00000000 GPA Input Type Selection Register
    volatile unsigned int   GPASR;                  //   0x03C  R/W  0x00000000 GPA Slew rate Control Register
    volatile unsigned int   GPBDAT;                 //   0x040  R/W  0x00000000  GPB Data Register
    volatile unsigned int   GPBEN;                  //   0x044  R/W  0x00000000  GPB Output Enable Register
    volatile unsigned int   GPBSET;                 //   0x048  W  -  OR function on GPB Output Data
    volatile unsigned int   GPBCLR;                 //   0x04C  W  -  BIC function on GPB Output Data
    volatile unsigned int   GPBXOR;                 //   0x050  W  -  XOR function on GPB Output Data
    volatile unsigned int   GPBCD0;                 //   0x054  W  0x55555555  Driver strength Control 0 on GPB Output Data
    volatile unsigned int   GPBCD1;                 //   0x058  W  0x00000000  Driver strength Control 1 on GPB Output Data
    volatile unsigned int   GPBPD0;                 //   0x05C  W  0x55555555  Pull-Up/Down function on GPB Output Data
    volatile unsigned int   GPBPD1;                 //   0x060  W  0x00000000  Pull-Up/Down function on GPB Output Data
    volatile unsigned int   GPBFN0;                 //   0x064  W  0x00000000  Port Configuration on GPB Output Data
    volatile unsigned int   GPBFN1;                 //   0x068  W  0x00000000  Port Configuration on GPB Output Data
    volatile unsigned int   GPBFN2;                 //   0x06C  W  0x00000000  Port Configuration on GPB Output Data
    volatile unsigned int   GPBFN3;                 //   0x070  W  0x00000000  Port Configuration on GPB Output Data
    volatile unsigned int   GPBIEN;                 //   0x074  R/W  0x00000000 GPB Input Enable Register
    volatile unsigned int   GPBIS;                  //   0x078  R/W  0x00000000 GPB Input Type Selection Register
    volatile unsigned int   GPBSR;                  //   0x07C  R/W  0x00000000 GPB Slew rate Control Register
    volatile unsigned int   GPCDAT;                 //   0x080  R/W  0x00000000  GPC Data Register
    volatile unsigned int   GPCEN;                  //   0x084  R/W  0x00000000  GPC Output Enable Register
    volatile unsigned int   GPCSET;                 //   0x088  W  -  OR function on GPC Output Data
    volatile unsigned int   GPCCLR;                 //   0x08C  W  -  BIC function on GPC Output Data
    volatile unsigned int   GPCXOR;                 //   0x090  W  -  XOR function on GPC Output Data
    volatile unsigned int   GPCCD0;                 //   0x094  W  0x55555555  Driver strength Control 0 on GPC Output Data
    volatile unsigned int   GPCCD1;                 //   0x098  W  0x00000000  Driver strength Control 1 on GPC Output Data
    volatile unsigned int   GPCPD0;                 //   0x09C  W  0x55555555  Pull-Up/Down function on GPC Output Data
    volatile unsigned int   GPCPD1;                 //   0x0A0  W  0x00000000  Pull-Up/Down function on GPC Output Data
    volatile unsigned int   GPCFN0;                 //   0x0A4  W  0x00000000  Port Configuration on GPC Output Data
    volatile unsigned int   GPCFN1;                 //   0x0A8  W  0x00000000  Port Configuration on GPC Output Data
    volatile unsigned int   GPCFN2;                 //   0x0AC  W  0x00000000  Port Configuration on GPC Output Data
    volatile unsigned int   GPCFN3;                 //   0x0B0  W  0x00000000  Port Configuration on GPC Output Data
    volatile unsigned int   GPCIEN;                 //   0x0B4  R/W  0x00000000 GPC Input Enable Register
    volatile unsigned int   GPCIS;                  //   0x0B8  R/W  0x00000000 GPC Input Type Selection Register
    volatile unsigned int   GPCSR;                  //   0x0BC  R/W  0x00000000 GPC Slew rate Control Register
    volatile unsigned int   GPDDAT;                 //   0x0C0  R/W  0x00000000  GPD Data Register
    volatile unsigned int   GPDEN;                  //   0x0C4  R/W  0x00000000  GPD Output Enable Register
    volatile unsigned int   GPDSET;                 //   0x0C8  W  -  OR function on GPD Output Data
    volatile unsigned int   GPDCLR;                 //   0x0CC  W  -  BIC function on GPD Output Data
    volatile unsigned int   GPDXOR;                 //   0x0D0  W  -  XOR function on GPD Output Data
    volatile unsigned int   GPDCD0;                 //   0x0D4  W  0x55555555  Driver strength Control 0 on GPD Output Data
    volatile unsigned int   GPDCD1;                 //   0x0D8  W  0x00000000  Driver strength Control 1 on GPD Output Data
    volatile unsigned int   GPDPD0;                 //   0x0DC  W  0x55555555  Pull-Up/Down function on GPD Output Data
    volatile unsigned int   GPDPD1;                 //   0x0E0  W  0x00000000  Pull-Up/Down function on GPD Output Data
    volatile unsigned int   GPDFN0;                 //   0x0E4  W  0x00000000  Port Configuration on GPD Output Data
    volatile unsigned int   GPDFN1;                 //   0x0E8  W  0x00000000  Port Configuration on GPD Output Data
    volatile unsigned int   GPDFN2;                 //   0x0EC  W  0x00000000  Port Configuration on GPD Output Data
    volatile unsigned int   GPDFN3;                 //   0x0F0  W  0x00000000  Port Configuration on GPD Output Data
    volatile unsigned int   GPDIEN;                 //   0x0F4  R/W  0x00000000 GPD Input Enable Register
    volatile unsigned int   GPDIS;                  //   0x0F8  R/W  0x00000000 GPD Input Type Selection Register
    volatile unsigned int   GPDSR;                  //   0x0FC  R/W  0x00000000 GPD Slew rate Control Register
    volatile unsigned int   GPEDAT;                 //   0x100  R/W  0x00000000  GPE Data Register
    volatile unsigned int   GPEEN;                  //   0x104  R/W  0x00000000  GPE Output Enable Register
    volatile unsigned int   GPESET;                 //   0x108  W  -  OR function on GPE Output Data
    volatile unsigned int   GPECLR;                 //   0x10C  W  -  BIC function on GPE Output Data
    volatile unsigned int   GPEXOR;                 //   0x110  W  -  XOR function on GPE Output Data
    volatile unsigned int   GPECD0;                 //   0x114  W  0x55555555  Driver strength Control 0 on GPE Output Data
    volatile unsigned int   GPECD1;                 //   0x118  W  0x00000000  Driver strength Control 1 on GPE Output Data
    volatile unsigned int   GPEPD0;                 //   0x11C  W  0x55555555  Pull-Up/Down function on GPE Output Data
    volatile unsigned int   GPEPD1;                 //   0x120  W  0x00000000  Pull-Up/Down function on GPE Output Data
    volatile unsigned int   GPEFN0;                 //   0x124  W  0x00000000  Port Configuration on GPE Output Data
    volatile unsigned int   GPEFN1;                 //   0x128  W  0x00000000  Port Configuration on GPE Output Data
    volatile unsigned int   GPEFN2;                 //   0x12C  W  0x00000000  Port Configuration on GPE Output Data
    volatile unsigned int   GPEFN3;                 //   0x130  W  0x00000000  Port Configuration on GPE Output Data
    volatile unsigned int   GPEIEN;                 //   0x134  R/W  0x00000000 GPE Input Enable Register
    volatile unsigned int   GPEIS;                  //   0x138  R/W  0x00000000 GPE Input Type Selection Register
    volatile unsigned int   GPESR;                  //   0x13C  R/W  0x00000000 GPE Slew rate Control Register
    volatile unsigned int   GPFDAT;                 //   0x140  R/W  0x00000000  GPF Data Register
    volatile unsigned int   GPFEN;                  //   0x144  R/W  0x00000000  GPF Output Enable Register
    volatile unsigned int   GPFSET;                 //   0x148  W  -  OR function on GPF Output Data
    volatile unsigned int   GPFCLR;                 //   0x14C  W  -  BIC function on GPF Output Data
    volatile unsigned int   GPFXOR;                 //   0x150  W  -  XOR function on GPF Output Data
    volatile unsigned int   GPFCD0;                 //   0x154  W  0x55555555  Driver strength Control 0 on GPF Output Data
    volatile unsigned int   GPFCD1;                 //   0x158  W  0x00000000  Driver strength Control 1 on GPF Output Data
    volatile unsigned int   GPFPD0;                 //   0x15C  W  0x55555555  Pull-Up/Down function on GPF Output Data
    volatile unsigned int   GPFPD1;                 //   0x160  W  0x00000000  Pull-Up/Down function on GPF Output Data
    volatile unsigned int   GPFFN0;                 //   0x164  W  0x00000000  Port Configuration on GPF Output Data
    volatile unsigned int   GPFFN1;                 //   0x168  W  0x00000000  Port Configuration on GP Output Data
    volatile unsigned int   GPFFN2;                 //   0x16C  W  0x00000000  Port Configuration on GPF Output Data
    volatile unsigned int   GPFFN3;                 //   0x170  W  0x00000000  Port Configuration on GPF Output Data
    volatile unsigned int   GPFIEN;                 //   0x174  R/W  0x00000000 GPF Input Enable Register
    volatile unsigned int   GPFIS;                  //   0x178  R/W  0x00000000 GPF Input Type Selection Register
    volatile unsigned int   GPFSR;                  //   0x17C  R/W  0x00000000 GPF Slew rate Control Register
	volatile unsigned int	Reserved0[1];			//	 0x180	-		-
    volatile unsigned int   EINTSEL0;               //   0x184  R/W  0x00000000  External Interrupt Select Register 0
    volatile unsigned int   EINTSEL1;               //   0x188  R/W  0x00000000  External Interrupt Select Register 1
    volatile unsigned int   EINTSEL2;               //   0x18C	R/W  0x00000000  External Interrupt Select Register 2
	volatile unsigned int	Reserved1[1];			//	 0x190	-		-
	volatile unsigned int	ECID0;					//	 0x194	R/W	0x00000000	CID Output Register
	volatile unsigned int	ECID1;					//	 0x198	R/W		-		CID Serial Input Register
	volatile unsigned int	ECID2;					//	 0x19C	R/W		-		CID Parallel Input 0 Register
	volatile unsigned int	ECID3;					//	 0x1A0	R/W		-		CID Parallel Input 1 Register
	volatile unsigned int	Reserved2[7];			//	 0x1A4 ~ 0x1BF
    volatile unsigned int   GPGDAT;                 //   0x1C0  R/W  0x00000000  GPG Data Register
    volatile unsigned int   GPGEN;                  //   0x1C4  R/W  0x00000000  GPG Output Enable Register
    volatile unsigned int   GPGSET;                 //   0x1C8  W  -  OR function on GPG Output Data
    volatile unsigned int   GPGCLR;                 //   0x1CC  W  -  BIC function on GPG Output Data
    volatile unsigned int   GPGXOR;                 //   0x1D0  W  -  XOR function on GPG Output Data
    volatile unsigned int   GPGCD0;                 //   0x1D4  W  0x55555555  Driver strength Control 0 on GPG Output Data
    volatile unsigned int   GPGCD1;                 //   0x1D8  W  0x00000000  Driver strength Control 1 on GPG Output Data
    volatile unsigned int   GPGPD0;                 //   0x1DC  W  0x55555555  Pull-Up/Down function on GPG Output Data
    volatile unsigned int   GPGPD1;                 //   0x1E0  W  0x00000000  Pull-Up/Down function on GPG Output Data
    volatile unsigned int   GPGFN0;                 //   0x1E4  W  0x00000000  Port Configuration on GPG Output Data
    volatile unsigned int   GPGFN1;                 //   0x1E8  W  0x00000000  Port Configuration on GPG Output Data
    volatile unsigned int   GPGFN2;                 //   0x1EC  W  0x00000000  Port Configuration on GPG Output Data
    volatile unsigned int   GPGFN3;                 //   0x1F0  W  0x00000000  Port Configuration on GPG Output Data
    volatile unsigned int   GPGIEN;                 //   0x1F4  R/W  0x00000000 GPG Input Enable Register
    volatile unsigned int   GPGIS;                  //   0x1F8  R/W  0x00000000 GPG Input Type Selection Register
    volatile unsigned int   GPGSR;                  //   0x1FC  R/W  0x00000000 GPG Slew rate Control Register
} GPIO, *PGPIO;

typedef struct _GPION{
    volatile unsigned int   GPDAT;                  // 0x000  R/W   GPA Data Register
    volatile unsigned int   GPEN;                   // 0x004  R/W   GPA Output Enable Register
    volatile unsigned int   GPSET;                  // 0x008  W     OR function on GPA Output Data
    volatile unsigned int   GPCLR;                  // 0x00C  W     BIC function on GPA Output Data
    volatile unsigned int   GPXOR;                  // 0x010  W     XOR function on GPA Output Data
    volatile unsigned int   GPCD0;                  // 0x014  W     Driver strength Control 0 on GPA Output Data
    volatile unsigned int   GPCD1;                  // 0x018  W     Driver strength Control 1 on GPA Output Data
    volatile unsigned int   GPPD0;                  // 0x01C  W     Pull-Up/Down function on GPA Output Data
    volatile unsigned int   GPPD1;                  // 0x020  W     Pull-Up/Down function on GPA Output Data
    volatile unsigned int   GPFN0;                  // 0x024  W     Port Configuration on GPA Output Data
    volatile unsigned int   GPFN1;                  // 0x028  W     Port Configuration on GPA Output Data
    volatile unsigned int   GPFN2;                  // 0x02C  W     Port Configuration on GPA Output Data
    volatile unsigned int   GPFN3;                  // 0x030  W     Port Configuration on GPA Output Data
} GPION, *PGPION;

/************************************************************************
*   8. SMU Config (Base Addr = )
************************************************************************/
//#define HwSMUCONFIG_BASE							((volatile unsigned long*)0xF05F4000)
typedef struct _SMUCONFIG{
    volatile unsigned int HCLKMASK;			//  0x00  R/W  0  Module Clock Mask Register
    volatile unsigned int SWRESET;			//  0x04  R/W  0  Module Software Reset Register
    volatile unsigned int CFG_PLLFIN_SEL;	//  0x08  R/W  0  PLL FIN Source Selection Register
    volatile unsigned int CFG_RTC_WAIT;		//  0x0C  R/W  0  RTC Bus Interface Wait Control Register
} SMUCONFIG, *PSMUCONFIG;


/*******************************************************************************
*
*    TCC88x DataSheet PART 3 GRAPHIC BUS
*
********************************************************************************/

/************************************************************************
*   4. 2D/3D GPU Register Define				(Base Addr = 0xF0000000) // R/W
************************************************************************/
//#define HwGPU_BASE								((volatile unsigned long*)0xF0000000)

/************************************************************************
*   4-3. Pixel Processor Register Define		(Base Addr = 0xF0000000) // R/W
************************************************************************/
//#define HwPIXELPROCESSOR_BASE					((volatile unsigned long*)0xF0000000)
//#define HwGPU_PXL_FRAMEREGISTERS_BASE			((volatile unsigned long*)0xF0000000)
//#define HwGPU_PXL_WRITEBACKREGISTER_BASE		((volatile unsigned long*)0xF0000100)
//#define HWGPU_PXL_MANAGEMENTREGISTER_BASE		((volatile unsigned long*)0xF0001000)
//
typedef struct _GPUPIXELPROCESSOR{
    volatile unsigned int   REND_LIST_ADDR;              // 0x0000 R/W 0x00000000 Renderer List Address
    volatile unsigned int   REND_RSW_BASE;               // 0x0004 R/W 0x00000000 Renderer State Word Base Address
    volatile unsigned int   REND_VERTEX_BASE;            // 0x0008 R/W 0x00000000 Renderer Vertex Base Address
    volatile unsigned int   FEATURE_ENABLE;              // 0x000C R/W 0x00000002 Feature Enable
    volatile unsigned int   Z_CLEAR_VALUE;               // 0x0010 R/W 0x00000009 Z Clear Value
    volatile unsigned int   STENCIL_CLEAR_VALUE;         // 0x0014 R/W 0x00000000 Stencil Clear value
    volatile unsigned int   ABGR_CLEAR_VALUE_0;          // 0x0018 R/W 0x00000000 ABGR Clear Value 0
    volatile unsigned int   ABGR_CLEAR_VALUE_1;          // 0x001C R/W 0x00000000 ABGR Clear Value 1
    volatile unsigned int   ABGR_CLEAR_VALUE_2;          // 0x0020 R/W 0x00000000 ABGR Clear Value 2
    volatile unsigned int   ABGR_CLEAR_VALUE_3;          // 0x0024 R/W 0x00000000 ABGR Clear Value 3
    volatile unsigned int   BOUNDING_BOX_LEFT_RIGHT;     // 0x0028 R/W 0x00000000 Bounding Box left Right
    volatile unsigned int   BOUNDING_BOX_BOTTOM;         // 0x002C R/W 0x00000000 Bounding Box Bottom
    volatile unsigned int   FS_STACK_ADDR;               // 0x0030 R/W 0x00000000 FS Stack Address
    volatile unsigned int   FS_STACK_SIZE_AND_INIT_VAL;  // 0x0034 R/W 0x00000000 FS Stack Size and Initial Value
    volatile unsigned int   ORIGIN_OFFSET_X;             // 0x0040 R/W 0x00000000 Origin Offset X
    volatile unsigned int   ORIGIN_OFFSET_Y;             // 0x0044 R/W 0x00000000 Origin Offset Y
    volatile unsigned int   SUBPIXEL_SPECIFIER;          // 0x0048 R/W 0x00000075 Subpixel Specifier
    volatile unsigned int   TIEBREAK_MODE;               // 0x004C R/W 0x00000000 Tiebreak mode Register
    volatile unsigned int   NOTDEFINE0[44];
    volatile unsigned int   WB0_SOURCE_SELECT;           // 0x0100 R/W 0x00000000 WB0 Source Select
    volatile unsigned int   WB0_TARGET_ADDR;             // 0x0104 R/W 0x00000000 WB0 Target Addres
    volatile unsigned int   WB0_TARGET_PIXEL_FORMAT;     // 0x0108 R/W 0x00000000 WB0 Target Pixel Format
    volatile unsigned int   WB0_TARGET_AA_FORMAT;        // 0x010C R/W 0x00000000 WB0 Target AA Format
    volatile unsigned int   WB0_TARGET_LAYOUT;           // 0x0110 R/W 0x00000000 WB0 Target Layout
    volatile unsigned int   WB0_TARGET_SCANLINE_LENGTH;  // 0x0114 R/W 0x00000000 WB0 Target Scanline length
    volatile unsigned int   WB0_TARGET_FLAGS;            // 0x0118 R/W 0x00000000 WB0 Target Flags
    volatile unsigned int   WB0_MRT_ENABLE;              // 0x011C R/W 0x00000000 WB0 MRT Enagle
    volatile unsigned int   WB0_MRT_OFFSET;              // 0x0120 R/W 0x00000000 WB0 MRT Offset
    volatile unsigned int   WB0_GLOBAL_TEST_ENABLE;      // 0x0124 R/W 0x00000000 WB0 Global Test Enable
    volatile unsigned int   WB0_GLOBAL_TEST_REF_VALUE;   // 0x0128 R/W 0x00000000 WB0 Global Test Reference
    volatile unsigned int   WB0_GLOBAL_TEST_CMP_FUNC;    // 0x012C R/W 0x00000000 WB0 Global Test Compare Function
    volatile unsigned int   NOTDEFINE1[52];
    volatile unsigned int   WB1_SOURCE_SELECT;           // 0x0200 R/W 0x00000000 WB1 Source Select
    volatile unsigned int   WB1_TARGET_ADDR;             // 0x0204 R/W 0x00000000 WB1 Target Addres
    volatile unsigned int   WB1_TARGET_PIXEL_FORMAT;     // 0x0208 R/W 0x00000000 WB1 Target Pixel Format
    volatile unsigned int   WB1_TARGET_AA_FORMAT;        // 0x020C R/W 0x00000000 WB1 Target AA Format
    volatile unsigned int   WB1_TARGET_LAYOUT;           // 0x0210 R/W 0x00000000 WB1 Target Layout
    volatile unsigned int   WB1_TARGET_SCANLINE_LENGTH;  // 0x0214 R/W 0x00000000 WB1 Target Scanline length
    volatile unsigned int   WB1_TARGET_FLAGS;            // 0x0218 R/W 0x00000000 WB1 Target Flags
    volatile unsigned int   WB1_MRT_ENABLE;              // 0x021C R/W 0x00000000 WB1 MRT Enagle
    volatile unsigned int   WB1_MRT_OFFSET;              // 0x0220 R/W 0x00000000 WB1 MRT Offset
    volatile unsigned int   WB1_GLOBAL_TEST_ENABLE;      // 0x0224 R/W 0x00000000 WB1 Global Test Enable
    volatile unsigned int   WB1_GLOBAL_TEST_REF_VALUE;   // 0x0228 R/W 0x00000000 WB1 Global Test Reference
    volatile unsigned int   WB1_GLOBAL_TEST_CMP_FUNC;    // 0x022C R/W 0x00000000 WB1 Global Test Compare Function
    volatile unsigned int   NOTDEFINE2[52];
    volatile unsigned int   WB2_SOURCE_SELECT;           // 0x0300 R/W 0x00000000 WB2 Source Select
    volatile unsigned int   WB2_TARGET_ADDR;             // 0x0304 R/W 0x00000000 WB2 Target Addres
    volatile unsigned int   WB2_TARGET_PIXEL_FORMAT;     // 0x0308 R/W 0x00000000 WB2 Target Pixel Format
    volatile unsigned int   WB2_TARGET_AA_FORMAT;        // 0x030C R/W 0x00000000 WB2 Target AA Format
    volatile unsigned int   WB2_TARGET_LAYOUT;           // 0x0310 R/W 0x00000000 WB2 Target Layout
    volatile unsigned int   WB2_TARGET_SCANLINE_LENGTH;  // 0x0314 R/W 0x00000000 WB2 Target Scanline length
    volatile unsigned int   WB2_TARGET_FLAGS;            // 0x0318 R/W 0x00000000 WB2 Target Flags
    volatile unsigned int   WB2_MRT_ENABLE;              // 0x031C R/W 0x00000000 WB2 MRT Enagle
    volatile unsigned int   WB2_MRT_OFFSET;              // 0x0320 R/W 0x00000000 WB2 MRT Offset
    volatile unsigned int   WB2_GLOBAL_TEST_ENABLE;      // 0x0324 R/W 0x00000000 WB2 Global Test Enable
    volatile unsigned int   WB2_GLOBAL_TEST_REF_VALUE;   // 0x0328 R/W 0x00000000 WB2 Global Test Reference
    volatile unsigned int   WB2_GLOBAL_TEST_CMP_FUNC;    // 0x032C R/W 0x00000000 WB2 Global Test Compare Function
    volatile unsigned int   NOTDEFINE3[820];
    volatile unsigned int   VERSION;                     // 0x1000 R 0xC8070005 Version
    volatile unsigned int   CURRENT_REND_LIST_ADDR;      // 0x1004 R/W 0x00000000 Current Renderer List Address
    volatile unsigned int   STATUS;                      // 0x1008 R/W 0x00000000 Status
    volatile unsigned int   CTRL_MGMT;                   // 0x100C W N/A Control Management
    volatile unsigned int   INT_RAWSTAT;                 // 0x1020 R/W 0x00000000 Interrupt Rawstat
    volatile unsigned int   INT_CLEAR;                   // 0x1024 W N/A Interrupt Clear
    volatile unsigned int   INT_MASK;                    // 0x1028 RW 0x00001FF Interrupt Mask
    volatile unsigned int   INT_STATUS;                  // 0x102c R 0x00000000 Interrupt Status
    volatile unsigned int   WRITE_BOUNDARY_ENABLE;       // 0x1040 R/W 0x00000000 Write Boundary Enable
    volatile unsigned int   WRITE_BOUNDARY_LOW;          // 0x1044 R/W 0x00000000 Write Boundary Low
    volatile unsigned int   WRITE_BOUNDARY_HIGH;         // 0x1048 R/W 0x00000000 Write Boundary High
    volatile unsigned int   WRITE_BOUNDARY_ADDRESS;      // 0x104C R 0x00000000 Write Boundary Address
    volatile unsigned int   BUS_ERROR_STATUS;            // 0x1050 R 0x00000000 Bus Error Status
    volatile unsigned int   WATCHDOG_DISABLE;            // 0x1060 R/W 0x00000000 Watchdog Disable
    volatile unsigned int   WATCHDOG_TIMEOUT;            // 0x1064 R/W 0x000F4240 Watchdog Timeout
    volatile unsigned int   PERF_CNT_0_ENABLE;           // 0x1080 R/W 0x00000000 Performance Counter 0 Enable
    volatile unsigned int   PERF_CNT_0_SRC;              // 0x1084 R/W 0x00000000 Performance Counter 0 SRC
    volatile unsigned int   PERF_CNT_0_LIMIT;            // 0x1088 R/W 0x00000000 Performance Counter 0 Limit
    volatile unsigned int   PERF_CNT_0_VALUE;            // 0x108C R/W 0x00000000 Performance Counter 0 Value
    volatile unsigned int   PERF_CNT_1_ENABLE;           // 0x10A0 R/W 0x00000000 Performance Counter 1 Enable
    volatile unsigned int   PERF_CNT_1_SRC;              // 0x10A4 R/W 0x00000000 Performance Counter 1 SRC
    volatile unsigned int   PERF_CNT_1_LIMIT;            // 0x10A8 R/W 0x00000000 Performance Counter 1 Limit
    volatile unsigned int   PERF_CNT_1_VALUE;            // 0x10AC R/W 0x00000000 Performance Counter 1 Value
} GPUPIXELPROCESSOR,*PGPUPIXELPROCESSOR;

/************************************************************************
*   4-4. Geometry Processor Register Define         (Base Addr = 0xF0002000) // R/W
************************************************************************/
//#define HwGEOMETRYPROCESSOR_BASE				((volatile unsigned long*)0xF0002000)
//#define HwGPU_GEO_CONTROLREGISTER_BASE			((volatile unsigned long*)0xF0002000)
//#define HwGPU_GEO_PLBCONFIGREGISTER_BASE		0x0100
//#define HwGPU_GEO_VERTEXSHADER_BASE			0x0000

typedef struct _GPUGEOMETRYPROCESSOR{
    volatile unsigned int   CONTR_REG_VSCL_START_ADDR;          // 0x2000 R/W 0x00000000 Control Register VSCL Start Address
    volatile unsigned int   CONTR_REG_VSCL_END_ADDR;            // 0x2004 R/W 0x00000000 Control Register VSCL End Address
    volatile unsigned int   CONTR_REG_PLBCL_START_ADDR;         // 0x2008 R/W 0x00000000 Control Register PLBCL Start Address
    volatile unsigned int   CONTR_REG_PLBCL_END_ADDR;           // 0x200C R/W 0x00000000 Control Register PLBCL End Address
    volatile unsigned int   CONTR_REG_PLB_ALLOC_START_ADDR;     // 0x2010 R/W 0x00000000 Control Register PLB Allocate Start Address
    volatile unsigned int   CONTR_REG_PLB_ALLOC_END_ADDR;       // 0x2014 R/W 0x00000000 Control Register PLB Allocate End Address
    volatile unsigned int   CONTR_REG_CMD;                      // 0x2020 W   N/A        Control Register Command
    volatile unsigned int   CONTR_REG_INT_RAWSTAT;              // 0x2024 R/W 0x00000000 Control Register Interrupt Rawstat
    volatile unsigned int   CONTR_REG_INT_CLEAR;                // 0x2028 W   N/A        Control Register Interrupt Clear
    volatile unsigned int   CONTR_REG_INT_MASK;                 // 0x202C R/W 0x00000000 Control Register Interrupt Mask
    volatile unsigned int   CONTR_REG_INT_STAT;                 // 0x2030 R   0x00000000 Control Register Interrupt Status
    volatile unsigned int   CONTR_REG_WRITE_BOUND_LOW;          // 0x2034 R/W 0x00000000 Control Register Write Boundary Low
    volatile unsigned int   CONTR_REG_WRITE_BOUND_HIGH;         // 0x2038 R/W 0xFFFFFF00 Control Register Write Boundary High
    volatile unsigned int   CONTR_REG_PERF_CNT_0_ENABLE;        // 0x203C R/W 0x00000000 Control Register Performance Counter 0 Enable
    volatile unsigned int   CONTR_REG_PERF_CNT_1_ENABLE;        // 0x2040 R/W 0x00000000 Control Register Performance Counter 1 Enable
    volatile unsigned int   CONTR_REG_PERF_CNT_0_SRC;           // 0x2044 R/W 0x00000000 Control Register Performance Counter 0 Source
    volatile unsigned int   CONTR_REG_PERF_CNT_1_SRC;           // 0x2048 R/W 0x00000000 Control Register Performance Counter 1 Source
    volatile unsigned int   CONTR_REG_PERF_CNT_0_VAL;           // 0x204C R   0x00000000 Control Register Performance Counter 0 Value
    volatile unsigned int   CONTR_REG_PERF_CNT_1_VAL;           // 0x2050 R   0x00000000 Control Register Performance Counter 1 Value
    volatile unsigned int   CONTR_REG_PERF_CNT_0_LIMIT;         // 0x2054 R/W 0x00000000 Control Register Performance Counter 0 Limit
    volatile unsigned int   CONTR_REG_PERF_CNT_1_LIMIT;         // 0x2058 R/W 0x00000000 Control Register Performance Counter 1 Limit
    volatile unsigned int   CONTR_REG_STATUS;                   // 0x2068 R   0x00000000 Control Register Status
    volatile unsigned int   CONTR_REG_VERSION;                  // 0x206C R   0x0A070005 Control Register VERSION
    volatile unsigned int   CONTR_REG_VSCL_INITIAL_ADDR;        // 0x2080 R   0x00000000 Control Register VSCL Initial Address
    volatile unsigned int   CONTR_REG_PLBCL_INITIAL_ADDR;       // 0x2084 R   0x00000000 Control Register PLBCL Initial Address
    volatile unsigned int   CONTR_REG_WRITE_BOUNDARY_ERROR_ADDR;// 0x2088 R   0x00000000 Control Register Write Error Address
    volatile unsigned int   CONTR_REG_AXI_BUS_ERROR_STAT;       // 0x2094 R   0x00000000 Control AXI Bus Error Status
    volatile unsigned int   CONTR_REG_WATCHDOG_DISABLE;         // 0x20A0 R/W 0x00000000 Control Register Watchdog Disable
    volatile unsigned int   CONTR_REG_WATCHDOG_TIMEOUT;         // 0x20A4 R/W 0x000F4240 Control Register Watchdog Timeout
} GPUGEOMETRYPROCESSOR, *PGPUGEOMETRYPROCESSOR;


// MaliGP2 PLB Configuration Register Map
typedef struct _GPUPLBCFG{
    volatile unsigned char  PLB_CONF_REG_VERTEX_ARRAY_ADDR;    // 0x0100 W 0x00000000 PLB Configuration Register Vertex Array Address
    volatile unsigned char  PLB_CONF_REG_INDEX_ARRAY_ADDR;     // 0x0101 W 0x00000000 PLB Configuration Register Index Array Address
    volatile unsigned char  PLB_CONF_REG_POINT_SIZE_ADDR;      // 0x0102 W 0x00000000 PLB Configuration Register Point Size Address
    volatile unsigned char  PLB_CONF_REG_HEAP_START_ADDR;      // 0x0103 W 0x00000000 PLB Configuration Register Heap Start Address
    volatile unsigned char  PLB_CONF_REG_HEAP_END_ADDR;        // 0x0104 W 0x00000000 PLB Configuration Register Heap End Address
    volatile unsigned char  PLB_CONF_REG_VIEWPORT_TO;          // 0x0105 W 0x00000000 PLB Configuration Register Viewport Top
    volatile unsigned char  PLB_CONF_REG_VIEWPORT_BOTTOM;      // 0x0106 W 0x00000000 PLB Configuration Register Viewport Bottom
    volatile unsigned char  PLB_CONF_REG_VIEWPORT_LEFT;        // 0x0107 W 0x00000000 PLB Configuration Register Viewport Left
    volatile unsigned char  PLB_CONF_REG_VIEWPORT_RIGHT;       // 0x0108 W 0x00000000 PLB Configuration Register Viewport Right
    volatile unsigned char  PLB_CONF_REG_SCREENSIZE;           // 0x0109 W 0x00000000 PLB Configuration Register Screen Size
    volatile unsigned char  PLB_CONF_REG_OFFSET_VERTEX_ARRAY;  // 0x010A W 0x00000000 PLB Configuration Register Offset Vertex Array
    volatile unsigned char  PLB_CONF_REG_PARAMS;               // 0x010B W 0x00000000 PLB Configuration Register Parameters
    volatile unsigned char  PLB_CONF_REG_TILE_SIZE;            // 0x010C W 0x00000000 PLB Configuration Register Tile Size
    volatile unsigned char  PLB_CONF_REG_POchar_SIZE;          // 0x010D W 0x00000000 PLB Configuration Register Pochar Size
    volatile unsigned char  PLB_CONF_REG_Z_NEAR;               // 0x010E W 0x00000000 PLB Configuration Register Z Near
    volatile unsigned char  PLB_CONF_REG_Z_FAR;                // 0x010F W 0x3F800000 PLB Configuration Register Z Far
} GPUPLBCFG, *PGPUPLBCFG;


// MaliGP2 Vertex Shader Register Map
typedef union _VSCFGREG1{
    //volatile unsigned char    VS_CONF_REG_INP_ADDR[32];      // 0x0000-0x001E W 0x00000000 VS Configuration Register Input Address
    //volatile unsigned char    VS_CONF_REG_INP_SPEC[32];      // 0x0001-0x001F W 0x0000003F VS Configuration Register Input Specifier
    volatile unsigned char  ADDR[32];                    // 0x0000-0x001E W 0x00000000 VS Configuration Register Input Address
    volatile unsigned char  SPEC[32];                    // 0x0001-0x001F W 0x0000003F VS Configuration Register Input Specifier
} VSCFGREG;
/*
typedef union _VSCFGREG2{
    volatile unsigned char  ADDR[32];          // 0x0020-0x003E W 0x00000000 VS Configuration Register Output Address
    volatile unsigned char  SPEC[32];          // 0x0021-0x003F W 0x0000003F VS Configuration Register Output Specifier
} VSCFGREG2;
*/
typedef struct _GPUVERTEXSHADER{
    VSCFGREG  VS_CONF_REG_INP;
    VSCFGREG  VS_CONF_REG_OUTP;
    volatile unsigned char  VS_CONF_REG_PROG_PARAM;             // 0x0040 W 0x00000000 VS Configuration Register Program Parameter Create
    volatile unsigned char  VS_CONF_REG_PREFETCH;               // 0x0041 W 0x00000000 VS Configuration Register Prefetch
    volatile unsigned char  VS_CONF_REG_OPMOD;                  // 0x0042 W 0x0F000000 VS Configuration Register OPMOD
    volatile unsigned char  VS_CONF_REG_VERTICES_ALT_STRIDE;    // 0x0043 W 0x00000000 VS Configuration Register Vertices Alternative Stride
    volatile unsigned char  VS_CONF_REG_INPUT_ALT_STRIDE_0;     // 0x0044 W 0x00000000 VS Configuration Register Input Alternative Stride 0
    volatile unsigned char  VS_CONF_REG_INPUT_ALT_STRIDE_1;     // 0x0045 W 0x00000000 VS Configuration Register Input Alternative Stride 1
    volatile unsigned char  VS_CONF_REG_INPUT_ALT_STRIDE_2;     // 0x0046 W 0x00000000 VS Configuration Register Input Alternative Stride 2
    volatile unsigned char  VS_CONF_REG_INPUT_ALT_STRIDE_3;     // 0x0047 W 0x00000000 VS Configuration Register Input Alternative Stride 3
    volatile unsigned char  VS_CONF_REG_OUTPUT_ALT_STRIDE_0;    // 0x0048 W 0x00000000 VS Configuration Register Output Alternative Stride 0
    volatile unsigned char  VS_CONF_REG_OUTPUT_ALT_STRIDE_1;    // 0x0049 W 0x00000000 VS Configuration Register Output Alternative Stride 1
    volatile unsigned char  VS_CONF_REG_OUTPUT_ALT_STRIDE_2;    // 0x004A W 0x00000000 VS Configuration Register Output Alternative Stride 2
    volatile unsigned char  VS_CONF_REG_OUTPUT_ALT_STRIDE_3;    // 0x004B W 0x00000000 VS Configuration Register Output Alternative Stride 3
} GPUVERTEXSHADER, *PGPUVERTEXSHADER;

/*************************************************************************
*   4-5. MMU Register Define                        (Base Addr = 0xF0003000) // R/W
************************************************************************/
//#define HwGPUMMUCONFIG_BASE						((volatile unsigned long*)0xF0003000)

typedef struct _GPUMMUCONFIG{
    volatile unsigned int   MMU_DTE_ADDR;                 // 0x3000 R/W 0x00000000 MMU Current Page Table Address
    volatile unsigned int   MMU_STATUS;                   // 0x3004 R   0x00000018 MMU Status
    volatile unsigned int   MMU_COMMAND;                  // 0x3008 W   N/A        MMU Command
    volatile unsigned int   MMU_PAGE_FAULT_ADDR;          // 0x300C R   0x00000000 MMU Logical Address of Last Page Fault
    volatile unsigned int   MMU_ZAP_ONE_LINE;             // 0x3010 W   N/A        MMU Zap Cache Line
    volatile unsigned int   MMU_INT_RAWSTA;               // 0x3014 R/W 0x00000000 MMU Raw Interrupt Status
    volatile unsigned int   MMU_INT_CLEAR;                // 0x3018 W   N/A        MMU Interrupt Clear
    volatile unsigned int   MMU_INT_MASK;                 // 0x301C R/W 0x00000000 MMU Interrupt Mask
    volatile unsigned int   MMU_INT_STATUS;               // 0x3020 R   0x00000000 MMU Interrup Status
} GPUMMUCONFIG, *PGPUMMUCONFIG;


/************************************************************************
*   5. Graphic Bus Configuration Register Define  (Base Addr = 0xF0004000) // R/W
************************************************************************/
//#define HwGRPBUSCONFIG_BASE						((volatile unsigned long*)0xF0004000)

typedef struct _GRPBUSCONFIG {
    volatile unsigned int   GRPBUS_PWRDOWN;				// 0x0000 R/W 0x00000000 Graphics bus power down
    volatile unsigned int   GRPBUS_SWRESET;				// 0x0004 R/W 0x00000000 Graphics bus software reset
    volatile unsigned int   GRPBUS_GPU_IDLE;			// 0x0008 R/W 0x00000003 GPU idle configuration
    volatile unsigned int   GRPBUS_GPU_IDLE_STATUS;		// 0x000C R/W N/A        GPU idle status
}GRPBUSCONFIG, *PGRPBUSCONFIG;




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
//#define HwMULTILAYERBUSMATRIX_BASE				((volatile unsigned long*)0xF0300400)
typedef struct _MULTILAYERBUSMATRIX {
	volatile unsigned int	QT0;			//0x00			RW	0x00000000
	volatile unsigned int	QA0;			//0x04			RW	0x00000000
	volatile unsigned int	AR0;			//0x08			RW	0x00000000
	volatile unsigned int	AW0; 			//0x0C			RW	0x00000000
	volatile unsigned int	Reserved0[4];	//0x10 ~ 0x1F	-		-
	volatile unsigned int	QT1;			//0x20			RW	0x00000000
	volatile unsigned int	QA1;			//0x24			RW	0x00000000
	volatile unsigned int	AR1;			//0x28			RW	0x00000000
	volatile unsigned int	AW1;			//0x2C			RW	0x00000000
	volatile unsigned int	Reserved1[4];	//0x30 ~ 0x3F	-		-
	volatile unsigned int	QT2;			//0x40			RW	0x00000000
	volatile unsigned int	QA2;			//0x44			RW	0x00000000
	volatile unsigned int	AR2;			//0x48			RW	0x00000000
	volatile unsigned int	AW2;			//0x4C			RW	0x00000000
} MULTILAYERBUSMATRIX, *PMULTILAYERBUSMATRIX;

/************************************************************************
*   4. DDR SDRAM Memory Controller (Base Addr = 0xF0301000) // R/W
************************************************************************/
//#define HwLPDDR_BASE							((volatile unsigned long*)0xF0301000)
//#define HwDDR2_BASE								((volatile unsigned long*)0xF0302000)
//#define HwLPCON_BASE							((volatile unsigned long*)0xF0306000)
//#define HwDDR23_BASE							((volatile unsigned long*)0xF0309000)

//#define HwDDRIFCFG_BASE							((volatile unsigned long*)0xF0303000)
//#define HwDDRPHY_BASE							((volatile unsigned long*)0xF0304400)

typedef struct _DRAM{
    volatile unsigned int   STAT;                   //  0x000 RO  -  Status Register
    volatile unsigned int   CMD;                    //  0x004 WO  -  Command Register
    volatile unsigned int   DCMD;                   //  0x008 WO  -  Direct COmmnad Register
    volatile unsigned int   CFG;                    //  0x00C R/W 0x00010020 Configuration Register
    volatile unsigned int   REF;                    //  0x010 R/W 0x00000A60 Refresh Period Register
    volatile unsigned int   CAS;                    //  0x014 R/W 0x00000006 CAS Latency Register
    volatile unsigned int   DQSS;                   //  0x018 R/W 0x00000001 t_dqss Register
    volatile unsigned int   MRD;                    //  0x01C R/W 0x00000002 t_mrd Register
    volatile unsigned int   RAS;                    //  0x020 R/W 0x00000007 t_ras Register
    volatile unsigned int   RC;                     //  0x024 R/W 0x0000000B t_rc Register
    volatile unsigned int   RCD;                    //  0x028 R/W 0x0000001D t_rcd Register
    volatile unsigned int   RFC;                    //  0x02C R/W 0x000000212 t_rfc Register
    volatile unsigned int   RP;                     //  0x030 R/W 0x0000001D t_rp Register
    volatile unsigned int   RRD;                    //  0x034 R/W 0x00000002 t_rrd Register
    volatile unsigned int   WR;                     //  0x038 R/W 0x00000003 t_wr Register
    volatile unsigned int   WTR;                    //  0x03C R/W 0x00000002 t_wtr Register
    volatile unsigned int   XP;                     //  0x040 R/W 0x00000001 t_xp Register
    volatile unsigned int   XSR;                    //  0x044 R/W 0x0000000A t_xsr Register
    volatile unsigned int   ESR;                    //  0x048 R/W 0x00000014 t_esr Register
    volatile unsigned int   CFG2;                   //  0x04C R/W  -         Memory_cfg2 Register
    volatile unsigned int   CFG3;                   //  0x050 R/W 0x00000007 Memory_cfg3 Register
    volatile unsigned int   NOTDEFINE0[43];         //  0x054 - 0x0FC        Reserved
    volatile unsigned int   ID0;                    //  0x100 R/W 0x00000000 AXI ID0 configuration Register
    volatile unsigned int   ID1;                    //  0x104 R/W 0x00000000 AXI ID1 configuration Register
    volatile unsigned int   ID2;                    //  0x108 R/W 0x00000000 AXI ID2 configuration Register
    volatile unsigned int   ID3;                    //  0x10C R/W 0x00000000 AXI ID3 configuration Register
    volatile unsigned int   ID4;                    //  0x110 R/W 0x00000000 AXI ID4 configuration Register
    volatile unsigned int   ID5;                    //  0x114 R/W 0x00000000 AXI ID5 configuration Register
    volatile unsigned int   ID6;                    //  0x118 R/W 0x00000000 AXI ID6 configuration Register
    volatile unsigned int   ID7;                    //  0x11C R/W 0x00000000 AXI ID7 configuration Register
    volatile unsigned int   ID8;                    //  0x120 R/W 0x00000000 AXI ID8 configuration Register
    volatile unsigned int   ID9;                    //  0x124 R/W 0x00000000 AXI ID9 configuration Register
    volatile unsigned int   ID10;                   //  0x128 R/W 0x00000000 AXI ID10 configuration Register
    volatile unsigned int   ID11;                   //  0x12C R/W 0x00000000 AXI ID11 configuration Register
    volatile unsigned int   ID12;                   //  0x130 R/W 0x00000000 AXI ID12 configuration Register
    volatile unsigned int   ID13;                   //  0x134 R/W 0x00000000 AXI ID13 configuration Register
    volatile unsigned int   ID14;                   //  0x138 R/W 0x00000000 AXI ID14 configuration Register
    volatile unsigned int   ID15;                   //  0x13C R/W 0x00000000 AXI ID15 configuration Register
    volatile unsigned int   NOTDEFINE1[48];         //  0x140 - 0x1FC        Reserved
    volatile unsigned int   CH0;                    //  0x200 R/W 0x0000FF00 CHIP ID0 configuration Register
    volatile unsigned int   CH1;                    //  0x204 R/W 0x0000FF00 CHIP ID1 configuration Register
    volatile unsigned int   CH2;                    //  0x208 R/W 0x0000FF00 CHIP ID2 configuration Register
    volatile unsigned int   CH3;                    //  0x20C R/W 0x0000FF00 CHIP ID3 configuration Register
} DRAM, *PDRAM;

typedef struct _DMC2{
    volatile unsigned int   CONCONTROL;       // 0x000  R/W  0x3FFF_1310    Controller Control Register
    volatile unsigned int   MEMCONTROL;       // 0x004  R/W  0x0020_2100    Memory Control Register
    volatile unsigned int   MEMCONFIG0;       // 0x008  R/W  0x20F8_0312    Memory Chip0 Configuration Register
    volatile unsigned int   MEMCONFIG1;       // 0x00C  R/W  0x28F8_0312    Memory Chip1 Configuration Register
    volatile unsigned int   DIRECTCMD;        // 0x010  R/W  0x0000_0000    Memory Direct Command Register
    volatile unsigned int   PRECHCONFIG;      // 0x014  R/W  0xFF00_0000    Precharge Policy Configuration Register
    volatile unsigned int   PHYCONTROL0;      // 0x018  R/W  0x0010_1000    PHY Control0 Register
    volatile unsigned int   PHYCONTROL1;      // 0x01C  R/W  0x0000_0000    PHY Control1 Register
    volatile unsigned int   PHYCONTROL2;      // 0x020  R/W  0x0000_0000    PHY Control2 Register
    volatile unsigned int   PHYCONTROL3;      // 0x024  R/W  0x0000_0000    PHY Control3 Register
    volatile unsigned int   PWRDNCONFIG;      // 0x028  R/W  0xFFFF_00FF    Dynamic Power Down Configuration Register
    volatile unsigned int   _NOTDEFINE0[1];   //
    volatile unsigned int   TIMINGAREF;       // 0x030  R/W  0x0000_040E    AC Timing Register for SDRAM Auto refresh
    volatile unsigned int   TIMINGROW;        // 0x034  R/W  0x0F23_3286    AC Timing Register for SDRAM Row
    volatile unsigned int   TIMINGDATA;       // 0x038  R/W  0x1213_0204    AC Timing Register for SDRAM Data
    volatile unsigned int   TIMINGPOWER;      // 0x03C  R/W  0x0E1B_0422    AC Timing Register for Power Mode of SDRAM
    volatile unsigned int   PHYSTATUS;        // 0x040  R    0x0000_0000    PHY Status Register
    volatile unsigned int   PHYZQCONTROL;     // 0x044  R/W  0xE385_5731    PHY ZQ I/O Control Register
    volatile unsigned int   CHIP0STATUS;      // 0x048  R    0x0000_0000    Memory Chip0 Status Register
    volatile unsigned int   CHIP1STATUS;      // 0x04C  R    0x0000_0000    Memory Chip1 Status Register
    volatile unsigned int   AREFSTATUS;       // 0x050  R    0x0000_FFFF    Counter Status Register for Auto Refresh
    volatile unsigned int   MRSTATUS;         // 0x054  R    0x0000_0000    Memory Mode Registers Status Register
    volatile unsigned int   PHYTEST0;         // 0x058  R/W  0x0000_0000    PHY Test Register 0
    volatile unsigned int   PHYTEST1;         // 0x05C  R    0x0000_0000    PHY Test Register 1
    volatile unsigned int   QOSCONTROL0;      // 0x060  R/W  0x0000_0000    Quality of Service Control Register 0
    volatile unsigned int   QOSCONFIG0;       // 0x064  R/W  0x0000_0000    Quality of Service Configuration Register 0
    volatile unsigned int   QOSCONTROL1;      // 0x068  R/W  0x0000_0000    Quality of Service Control Register 1
    volatile unsigned int   QOSCONFIG1;       // 0x06C  R/W  0x0000_0000    Quality of Service Configuration Register 1
    volatile unsigned int   QOSCONTROL2;      // 0x070  R/W  0x0000_0000    Quality of Service Control Register 2
    volatile unsigned int   QOSCONFIG2;       // 0x074  R/W  0x0000_0000    Quality of Service Configuration Register 2
    volatile unsigned int   QOSCONTROL3;      // 0x078  R/W  0x0000_0000    Quality of Service Control Register 3
    volatile unsigned int   QOSCONFIG3;       // 0x07C  R/W  0x0000_0000    Quality of Service Configuration Register 3
    volatile unsigned int   QOSCONTROL4;      // 0x080  R/W  0x0000_0000    Quality of Service Control Register 4
    volatile unsigned int   QOSCONFIG4;       // 0x084  R/W  0x0000_0000    Quality of Service Configuration Register 4
    volatile unsigned int   QOSCONTROL5;      // 0x088  R/W  0x0000_0000    Quality of Service Control Register 5
    volatile unsigned int   QOSCONFIG5;       // 0x08C  R/W  0x0000_0000    Quality of Service Configuration Register 5
    volatile unsigned int   QOSCONTROL6;      // 0x090  R/W  0x0000_0000    Quality of Service Control Register 6
    volatile unsigned int   QOSCONFIG6;       // 0x094  R/W  0x0000_0000    Quality of Service Configuration Register 6
    volatile unsigned int   QOSCONTROL7;      // 0x098  R/W  0x0000_0000    Quality of Service Control Register 7
    volatile unsigned int   QOSCONFIG7;       // 0x09C  R/W  0x0000_0000    Quality of Service Configuration Register 7
    volatile unsigned int   QOSCONTROL8;      // 0x0A0  R/W  0x0000_0000    Quality of Service Control Register 8
    volatile unsigned int   QOSCONFIG8;       // 0x0A4  R/W  0x0000_0000    Quality of Service Configuration Register 8
    volatile unsigned int   QOSCONTROL9;      // 0x0A8  R/W  0x0000_0000    Quality of Service Control Register 9
    volatile unsigned int   QOSCONFIG9;       // 0x0AC  R/W  0x0000_0000    Quality of Service Configuration Register 9
    volatile unsigned int   QOSCONTROL10;     // 0x0B0  R/W  0x0000_0000    Quality of Service Control Register 10
    volatile unsigned int   QOSCONFIG10;      // 0x0B4  R/W  0x0000_0000    Quality of Service Configuration Register 10
    volatile unsigned int   QOSCONTROL11;     // 0x0B8  R/W  0x0000_0000    Quality of Service Control Register 11
    volatile unsigned int   QOSCONFIG11;      // 0x0BC  R/W  0x0000_0000    Quality of Service Configuration Register 11
    volatile unsigned int   QOSCONTROL12;     // 0x0C0  R/W  0x0000_0000    Quality of Service Control Register 12
    volatile unsigned int   QOSCONFIG12;      // 0x0C4  R/W  0x0000_0000    Quality of Service Configuration Register 12
    volatile unsigned int   QOSCONTROL13;     // 0x0C8  R/W  0x0000_0000    Quality of Service Control Register 13
    volatile unsigned int   QOSCONFIG13;      // 0x0CC  R/W  0x0000_0000    Quality of Service Configuration Register 13
    volatile unsigned int   QOSCONTROL14;     // 0x0D0  R/W  0x0000_0000    Quality of Service Control Register 14
    volatile unsigned int   QOSCONFIG14;      // 0x0D4  R/W  0x0000_0000    Quality of Service Configuration Register 14
    volatile unsigned int   QOSCONTROL15;     // 0x0D8  R/W  0x0000_0000    Quality of Service Control Register 15
    volatile unsigned int   QOSCONFIG15;      // 0x0DC  R/W  0x0000_0000    Quality of Service Configuration Register 15
} DMC2, *PDMC2, LPCON, *PLPCON;

typedef struct _DMC3{
    volatile unsigned int   CONCONTROL;        // 0x000  R/W  0x0FFF_1010    Controller control register
    volatile unsigned int   MEMCONTROL;        // 0x004  R/W  0x0000_010A    Memory control register
    volatile unsigned int   MEMCONFIG0;        // 0x008  R/W  0x20F8_0312    Memory chip0 configuration register
    volatile unsigned int   MEMCONFIG1;        // 0x00C  R/W  0x28F8_0312    Memory chip1 configuration register
    volatile unsigned int   DIRECTCMD;         // 0x010  R/W  0x0000_0000    Memory direct command register
    volatile unsigned int   PRECHCONFIG;       // 0x014  R/W  0xFF00_0000    Precharge policy configuration register
    volatile unsigned int   MEMPWD;            // 0x018  R/W  0x0000_0000    Memory power down register
    volatile unsigned int   PWRDNCONFIG;       // 0x01C  R/W  0xFFFF_00FF    Dynamic power down configuration register
    volatile unsigned int   _NOTDEFINE0[56];
    volatile unsigned int   AREF;              // 0x100  R/W  0x0000_040E    AC timing register for auto refresh of SDRAM
    volatile unsigned int   TRFC;              // 0x104  R/W  0x0000_000F    Auto refresh to other command
    volatile unsigned int   TRRD;              // 0x108  R/W  0x0000_0002    Bank to Bank active delay
    volatile unsigned int   TRP;               // 0x10C  R/W  0x0000_0003    Row precharge delay
    volatile unsigned int   TRCD;              // 0x110  R/W  0x0000_0003    Activated to read or write command
    volatile unsigned int   TRC;               // 0x114  R/W  0x0000_000A    Same bank activate to activate delay
    volatile unsigned int   TRAS;              // 0x118  R/W  0x0000_0006    Row open period
    volatile unsigned int   TWTR;              // 0x11C  R/W  0x0000_0001    Write to read delay
    volatile unsigned int   TWR;               // 0x120  R/W  0x0000_0002    Write recovery time
    volatile unsigned int   TRTP;              // 0x124  R/W  0x0000_0001    Read to precharge delay
    volatile unsigned int   CL;                // 0x128  R/W  0x0000_0003    CAS latency
    volatile unsigned int   WL;                // 0x12C  R/W  0x0000_0002    Write latency
    volatile unsigned int   RL;                // 0x130  R/W  0x0000_0004    Read latency
    volatile unsigned int   FAW;               // 0x134  R/W  0x0000_001E    Quadrapule row activation command delay
    volatile unsigned int   TXSR;              // 0x138       0x0000_001B    Self refresh exit delay
    volatile unsigned int   TXP;               // 0x13C  R/W  0x0000_0004    Power down exit delay
    volatile unsigned int   TCKE;              // 0x140  R/W  0x0000_0002    Minimum power down period
    volatile unsigned int   TMRD;              // 0x144  R/W  0x0000_0002    Mode register set delay
    volatile unsigned int   TZQINIT;           // 0x148  R/W  0x0000_0201    ZQ calibration delay after reset or power up
    volatile unsigned int   TZQOPER;           // 0x14C  R/W  0x0000_0101    Full ZQ calibration delay
    volatile unsigned int   TZQS;              // 0x150  R/W  0x0000_0041    Short ZQ calibration delay
    volatile unsigned int   _NOTDEFINE1[43];
    volatile unsigned int   AXISTATUS;         // 0x200  R    0x0000_00031   Axi transaction status register
    volatile unsigned int   AREFSTATUS;        // 0x204  R    0x0000_FFFF1   Counter status register for auto refresh
    volatile unsigned int   CHIP0STATUS;       // 0x208  R    0x0000_000     Memory chip0 status register
    volatile unsigned int   CHIP1STATUS;       // 0x20C  R    0x0000_000     Memory chip1 status register
//error!!!
    volatile unsigned int   MRSTATUS;          // 0x208  R    0x0000_000     Memory mode registers status register
    volatile unsigned int   _NOTDEFINE2[61];
//error!!!
    volatile unsigned int   QOSCONTROL0;       // 0x300  R/W  0x00FF_0000    Quality of service control register 0
    volatile unsigned int   QOSCONFIG0;        // 0x304  R/W  0x0000_0000    Quality of service configuration register 0
    volatile unsigned int   QOSCONTROL1;       // 0x308  R/W  0x00FF_0000    Quality of service control register 1
    volatile unsigned int   QOSCONFIG1;        // 0x30C  R/W  0x0000_0000    Quality of service configuration register 1
    volatile unsigned int   QOSCONTROL2;       // 0x310  R/W  0x00FF_0000    Quality of service control register 2
    volatile unsigned int   QOSCONFIG2;        // 0x314  R/W  0x0000_0000    Quality of service configuration register 2
    volatile unsigned int   QOSCONTROL3;       // 0x318  R/W  0x00FF_0000    Quality of service control register 3
    volatile unsigned int   QOSCONFIG3;        // 0x31C  R/W  0x0000_0000    Quality of service configuration register 3
    volatile unsigned int   QOSCONTROL4;       // 0x320  R/W  0x00FF_0000    Quality of service control register 4
    volatile unsigned int   QOSCONFIG4;        // 0x324  R/W  0x0000_0000    Quality of service configuration register 4
    volatile unsigned int   QOSCONTROL5;       // 0x328  R/W  0x00FF_0000    Quality of service control register 5
    volatile unsigned int   QOSCONFIG5;        // 0x32C  R/W  0x0000_0000    Quality of service configuration register 5
    volatile unsigned int   QOSCONTROL6;       // 0x330  R/W  0x00FF_0000    Quality of service control register 6
    volatile unsigned int   QOSCONFIG6;        // 0x334  R/W  0x0000_0000    Quality of service configuration register 6
    volatile unsigned int   QOSCONTROL7;       // 0x338  R/W  0x00FF_0000    Quality of service control register 7
    volatile unsigned int   _NOTDEFINE3[1];
    volatile unsigned int   QOSCONFIG7;        // 0x340  R/W  0x0000_0000    Quality of service configuration register 7
    volatile unsigned int   _NOTDEFINE4[47];
    volatile unsigned int   HYCONTROL0;        // 0x400  R/W  0x0000_0000    PHY control0 register
    volatile unsigned int   HYCONTROL1;        // 0x404  R/W  0x0000_0000    PHY control1 register
    volatile unsigned int   HYCONTROL2;        // 0x408  R/W  0x0000_0000    PHY control2 register
    volatile unsigned int   HYCONTROL3;        // 0x40C  R/W  0x0000_0000    PHY control3 register
    volatile unsigned int   HYCONTROL4;        // 0x410  R/W  0x0000_0000    PHY control4 register
    volatile unsigned int   HYCONTROL5;        // 0x414  R/W  0x0000_0000    PHY control5 register
    volatile unsigned int   PHYSTATUS0;        // 0x418  R    0x0000_000     PHY status register 0
    volatile unsigned int   PHYSTATUS1;        // 0x41C  R    0x0000_000     PHY status register 1
    volatile unsigned int   PHYSTATUS2;        // 0x420  R    0x0000_000     PHY status register 2
    volatile unsigned int   PHYTEST0;          // 0x424  R/W  0x0000_0000    PHY feedback test signals
    volatile unsigned int   PHYTEST1;          // 0x428  R    0x0000_000     PHY feedback status signals
} DMC3, *PDMC3, DDR23, *PDDR23;

typedef struct _DRAMMISC{
    volatile unsigned int   M0CFG0;         //0x00 R/W  0x80400000 SDR/DDR SDRAM Controller Configuration Register 0
    volatile unsigned int   M0CFG1;         //0x04 R/RW 0x00000018 SDR/DDR SDRAM Controller Configuration Register 1
    volatile unsigned int   NOTDEFINE0[2];  //0x08-0x0C            Reserved
    volatile unsigned int   M1CFG0;         //0x10 R/W  0x80000000 DDR2 SDRAM Controller Configuration Register 0
    volatile unsigned int   M1CFG1;         //0x14 R/W  0x00000000 DDR2 SDRAM Controller Configuration Register 1
    volatile unsigned int   NOTDEFINE1[2];  //0x18 - 0x1C          Reserved
    volatile unsigned int   COMMON;         //0x20 R/W  0x00010103 Common Control Register
    volatile unsigned int   PHYCTRL;        //0x24 R/W  0x00000000 SDRAM PHY Control Register
    volatile unsigned int   NOTDEFINE2[2];  //0x28 - 0x2C   Reserved
    volatile unsigned int   M2CFG0;         //0x30      0x00000000 LPDDR/KPDDR2/DDR2 SDRAM Controller Configuration Register 0
    volatile unsigned int   M2CFG1;         //0x34      0x00000000 LPDDR/KPDDR2/DDR2 SDRAM Controller Configuration Register 1
    volatile unsigned int   NOTDEFINE3[2];  //0x38 - 0x3C Reserved
    volatile unsigned int   M3CFG0;         //0x40      0x00000000 DDR2/DDR3 SDRAM Controller Configuration Register
} DRAMMISC, *PDRAMMISC, DRAMIFCFG, *PDRAMIFCFG, DMCCFG, *PDMCCFG;

typedef struct _DRAMPHY{
    volatile unsigned int   REG0;           // 0x400 R/W  0x00000000  PHY Mode Control Register
    volatile unsigned int   REG1;           // 0x404 R/W  0x00000000  DLL Control & Status Register
    volatile unsigned int   REG2;           // 0x408 R/W  0x00000000  DLL Phase Detector configuration Register
    volatile unsigned int   REG3;           // 0x40C R/W  0x00000000  Gate Control Register
    volatile unsigned int   REG4;           // 0x410 R/W  0x00000000  Read Data Slice 0 Control Register
    volatile unsigned int   REG5;           // 0x414 R/W  0x00000000  Read Data Slice 1 Control Register
    volatile unsigned int   REG6;           // 0x418 RO   0x00000000  Read Data Slice 2 Control Register
    volatile unsigned int   REG7;           // 0x41C R/W  0x00000000  Read Data Slice 3 Control Register
    volatile unsigned int   REG8;           // 0x420 R/W  0x00000000  CLK Delay Register
    volatile unsigned int   REG9;           // 0x424 R/W  0x00000000  DLL Force Lock Value Register
    volatile unsigned int   REG10;          // 0x428 R/W  0x00060000  ZQ Calibration Control Register
    volatile unsigned int   REG11;          // 0x42C RO   0x00000000  ZQ Calibration Status Register
    volatile unsigned int   REG12;          // 0x430 R/W  0x00000000  Read Delay Register
} DRAMPHY, *PDRAMPHY;

/************************************************************************
*   6. Memory Bus Configuration (Base Addr = ) // R/W
************************************************************************/
//#define HwMEMBUSCFG1_BASE						((volatile unsigned long*)0xF0305000)
//#define HwMEMBUSCFG2_BASE						((volatile unsigned long*)0xF0307000)

typedef struct _MEMBUSCFG {
	volatile unsigned int	Reserved0[1];		//0x00			-		-
	volatile unsigned int	HCLKMASK;		//0x04			RW	0x00000000	Memory Bus HCLK Mask Register
	volatile unsigned int	SWRESET;		//0x08			RW	0x00000000	Memory Bus Soft Reset Register
	volatile unsigned int	Reserved1[13];	//0x0C ~0x3F	-		-
	volatile unsigned int	PB0A0EN;		//0x40			RW	0x00000000	Prefetch Buffer 0 Region0 (ARM Data) Enable Register
	volatile unsigned int	PB0A0MIN;		//0x44			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 0 MIN Register
	volatile unsigned int	PB0A0MAX;		//0x48			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 0 MAX Register
	volatile unsigned int	Reserved2[1];		//0x4C			-		-
	volatile unsigned int	PB0A1EN;		//0x50			RW	0x00000000	Prefetch Buffer 0 Region1 (ARM Data) Enable Register
	volatile unsigned int	PB0A1MIN;		//0x54			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 1 MIN Register
	volatile unsigned int	PB0A1MAX;		//0x58			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 1 MAX Register
	volatile unsigned int	Reserved3[1];		//0x5C			-		-
	volatile unsigned int	PB0A2EN;		//0x60			RW	0x00000000	Prefetch Buffer 0 Region2 (ARM Data) Enable Register
	volatile unsigned int	PB0A2MIN;		//0x64			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 2 MIN Register
	volatile unsigned int	PB0A2MAX;		//0x68			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 2 MAX Register
	volatile unsigned int	Reserved4[1];		//0x6C			-		-
	volatile unsigned int	PB0A3EN;		//0x70			RW	0x00000000	Prefetch Buffer 0 Region3 (ARM Data) Enable Register
	volatile unsigned int	PB0A3MIN;		//0x74			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 3 MIN Register
	volatile unsigned int	PB0A3MAX;		//0x78			RW	0x00000000	Prefetch Buffer 0 (ARM Data) Address 3 MAX Register
	volatile unsigned int	Reserved5[1];		//0x7C			-		-
	volatile unsigned int	PB1A0EN;		//0x80			RW	0x00000000	Prefetch Buffer 1 Region0 (IO Bus) Enable Register
	volatile unsigned int	PB1A0MIN;		//0x84			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 0 MIN Register
	volatile unsigned int	PB1A0MAX;		//0x88			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 0 MAX Register
	volatile unsigned int	Reserved6[1];		//0x8C			-		-
	volatile unsigned int	PB1A1EN;		//0x90			RW	0x00000000	Prefetch Buffer 1 Region1 (IO Bus) Enable Register
	volatile unsigned int	PB1A1MIN;		//0x94			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 1 MIN Register
	volatile unsigned int	PB1A1MAX;		//0x98			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 1 MAX Register
	volatile unsigned int	Reserved7[1];		//0x9C			-		-
	volatile unsigned int	PB1A2EN;		//0xA0			RW	0x00000000	Prefetch Buffer 1 Region2 (IO Bus) Enable Register
	volatile unsigned int	PB1A2MIN;		//0xA4			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 2 MIN Register
	volatile unsigned int	PB1A2MAX;		//0xA8			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 2 MAX Register
	volatile unsigned int	Reserved8[1];		//0xAC			-		-
	volatile unsigned int	PB1A3EN;		//0xB0			RW	0x00000000	Prefetch Buffer 1 Region3 (IO Bus) Enable Register
	volatile unsigned int	PB1A3MIN;		//0xB4			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 3 MIN Register
	volatile unsigned int	PB1A3MAX;		//0xB8			RW	0x00000000	Prefetch Buffer 1 (IO Bus) Address 3 MAX Register
	volatile unsigned int	Reserved9[1];		//0xBC			-		-
	volatile unsigned int	PB2A0EN;		//0xC0			RW	0x00000000	Prefetch Buffer 2 Region0 (HSIO Bus) Enable Register
	volatile unsigned int	PB2A0MIN;		//0xC4			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus) Address 0 MIN Register
	volatile unsigned int	PB2A0MAX;		//0xC8			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus) Address 0 MAX Register
	volatile unsigned int	Reserved10[1];		//0xCC			-		-
	volatile unsigned int	PB2A1EN;		//0xD0			RW	0x00000000	Prefetch Buffer 2 Region1 (HSIO Bus) Enable Register
	volatile unsigned int	PB2A1MIN;		//0xD4			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus) Address 1 MIN Register
	volatile unsigned int	PB2A1MAX;		//0xD8			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus) Address 1 MAX Register
	volatile unsigned int	Reserved11[1];		//0xDC			-		-
	volatile unsigned int	PB2A2EN;		//0xE0			RW	0x00000000	Prefetch Buffer 2 Region2 (HSIO Bus) Enable Register
	volatile unsigned int	PB2A2MIN;		//0xE4			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus ta) Address 2 MIN Register
	volatile unsigned int	PB2A2MAX;		//0xE8			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus) Address 2 MAX Register
	volatile unsigned int	Reserved12[1];		//0xEC			-		-
	volatile unsigned int	PB2A3EN;		//0xF0			RW	0x00000000	Prefetch Buffer 2 Region3 (HSIO Bus) Enable Register
	volatile unsigned int	PB2A3MIN;		//0xF4			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus) Address 3 MIN Register
	volatile unsigned int	PB2A3MAX;		//0xF8			RW	0x00000000	Prefetch Buffer 2 (HSIO Bus) Address 3 MAX Register
} MEMBUSCFG, *PMEMBUSCFG;

/************************************************************************
*   7. Internal Memory BUS Configuration (Base Addr = ) // R/W
************************************************************************/
//#define HwINTNLMEMBUSCFG_BASE					((volatile unsigned long*)0xF0600000)
typedef struct _INTNLMEMBUSCFG{
    volatile unsigned int   REGION0;            // 0x00  R/W  0x0  Region Configuration Register 0
    volatile unsigned int   REGION1;            // 0x04  R/W  0x0  Region Configuration Register 1
    volatile unsigned int   REGION2;            // 0x08  R/W  0x0  Region Configuration Register 2
    volatile unsigned int   REGION3;            // 0x0C  R/W  0x0  Region Configuration Register 3
    volatile unsigned int   REGION4;            // 0x10  R/W  0x0  Region Configuration Register 4
    volatile unsigned int   REGION5;            // 0x14  R/W  0x0  Region Configuration Register 5
    volatile unsigned int   REGION6;            // 0x18  R/W  0x0  Region Configuration Register 6
    volatile unsigned int   REGION7;            // 0x1C  R/W  0x0  Region Configuration Register 7
}VMTREGION, *PVMTREGION, IMCCFG, *PIMCCFG, INTNLMEMBUSCFG, *PINTNLMEMBUSCFG;




/*******************************************************************************
*
*    TCC88x DataSheet PART 5 IO BUS
*
********************************************************************************/

/*******************************************************************************
*    4. Memory Stick Host Controller Register Define (Base Addr = 0xF0590000)
********************************************************************************/
//#define HwSMSHC_BASE							((volatile unsigned long*)0xF0590000)
//#define HwSMSHC_PORTCFG_BASE					((volatile unsigned long*)0xF05F1000)

typedef struct _SMSHC{
    volatile unsigned int   MS_CONTROL_PROGRAMCOUNTREG;         //  0x00    // R/W 0x0070_1000
    volatile unsigned int   MS_SYSTEMREG;                       //  0x04    // R/W 0x0800_XXXX
    volatile unsigned int   MS_FLAGREG;                         //  0x08    // R   0x4000_XXXX
    volatile unsigned int   MS_MEMORY_CONTROLREG;               //  0x0C    // R/W 0x0001_7000
    volatile unsigned int   MS_GENERALREG01;                    //  0x10    // R/W 0x8000_9000
    volatile unsigned int   MS_GENERALREG23;                    //  0x14    // R/W 0xA000_B000
    volatile unsigned int   MS_GENERALREG45;                    //  0x18    // R/W 0xC000_D000
    volatile unsigned int   MS_TIMERREG;                        //  0x1C    // R   0xE000XXXX
    volatile unsigned int   MS_INSTRUCTIONREG;                  //  0x20    // R/W 0xXXXX_XXXX
    volatile unsigned int   MS_GENERALDATAFIFO;                 //  0x24    // R/W 0x0000_0000
    volatile unsigned int   MS_PAGEBUFFER;                      //  0x28    // R/W 0xXXXX_XXXX
    volatile unsigned int   MS_VERSIONREG;                      //  0x2C    // R   0xXXXX_XXXX
    volatile unsigned int   MS_MSHC_COMMANDREG;                 //  0x30    // R/W 0x0000_XXXX
    volatile unsigned int   MS_MSHC_DATAREG;                    //  0x34    // R/W 0x0000_0000
    volatile unsigned int   MS_MSHC_STATUSREG;                  //  0x38    // R   0x1000_XXXX
    volatile unsigned int   MS_MSHC_SYSTEMREG;                  //  0x3C    // R/W 0x20A5_XXXX
    volatile unsigned int   MS_MSHC_USERCUSTOMREG;              //  0x40    // R   0x0220_XXXX
    volatile unsigned int   MS_MSHC_FIFOCTRLREG;                //  0x44    // R/W 0x0001_XXXX
    volatile unsigned int   NOTDEFINE0;                         //
    volatile unsigned int   MS_MSHC_DMACTRLREG;                 //  0x4C    // R/W 0x0000_XXXX
} SMSHC, *PSMSHC;

typedef struct _SMSHCPORTCFG{
    volatile unsigned int   MS_PORTCFG;                         //   0xB0870100    // R/W 0x00000000
    volatile unsigned int   MS_PORTDLY;                         //   0xB0870104    // R/W 0x00000000
} SMSHCPORTCFG, *PSMSHCPORTCFG;

/*******************************************************************************
*    5. SD/SDIO/MMC Host Controller Register Define (Base Addr = 0xF05A0000)
********************************************************************************/
//#define HwSDMMC_BASE							((volatile unsigned long*)0xF05A0000)
//#define HwSDMMC0_BASE							((volatile unsigned long*)0xF05A0000)
//#define HwSDMMC1_BASE							((volatile unsigned long*)0xF05A0200)
//#define HwSDMMC2_BASE							((volatile unsigned long*)0xF05A0400)
//#define HwSDMMC3_BASE							((volatile unsigned long*)0xF05A0600)
//#define HwSDMMC_CHCTRL_BASE					((volatile unsigned long*)0xF05A8000)

typedef struct _SDHOST{
    volatile unsigned short     SDMA;                       // 0x000 R/W 0x0000 SDMA System Address
    volatile unsigned short     NOTDEFINE0;                 // 0x002
    volatile unsigned short     BSIZE;                      // 0x004 R/W 0x0000 Block Size
    volatile unsigned short     BCNT;                       // 0x006 R/W 0x0000 Block Count
    volatile unsigned short     ARG;                        // 0x008 R/W 0x0000 Argument
    volatile unsigned short     NOTDEFINE1;                 // 0x00A
    volatile unsigned short     TMODE;                      // 0x00C R/W 0x0000 Transfer Mode
    volatile unsigned short     CMD;                        // 0x00E R/W 0x0000 Command
    volatile unsigned short     RESP0;                      // 0x010 R 0x0000 Response0
    volatile unsigned short     RESP1;                      // 0x012 R 0x0000 Response1
    volatile unsigned short     RESP2;                      // 0x014 R 0x0000 Response2
    volatile unsigned short     RESP3;                      // 0x016 R 0x0000 Response3
    volatile unsigned short     RESP4;                      // 0x018 R 0x0000 Response4
    volatile unsigned short     RESP5;                      // 0x01A R 0x0000 Response5
    volatile unsigned short     RESP6;                      // 0x01C R 0x0000 Response6
    volatile unsigned short     RESP7;                      // 0x01E R 0x0000 Response7
    volatile unsigned short     DATAL;                      // 0x020 R/W - Buffer Data Port(Low)
    volatile unsigned short     DATAH;                      // 0x022 R/W - Buffer Data Port(High)
    volatile unsigned short     STATEL;                     // 0x024 R 0x0000 Present State(Low)
    volatile unsigned short     STATEH;                     // 0x026 R 0x0000 Present State(High)
    volatile unsigned short     CONTL;                      // 0x028 R/W 0x0000 Power Control / Host Control
    volatile unsigned short     CONTH;                      // 0x02A R/W 0x0000 Wakeup Control / Block Gap Control
    volatile unsigned short     CLK;                        // 0x02C R/W 0x0000 Clock Control
    volatile unsigned short     TIME;                       // 0x02E R/W 0x0000 Software Reset / Timeout Control
    volatile unsigned short     STSL;                       // 0x030 R 0x0000 Normal Interrupt Status(Low)
    volatile unsigned short     STSH;                       // 0x032 R 0x0000 Normal Interrupt Status(High)
    volatile unsigned short     STSENL;                     // 0x034 R/W 0x0000 Normal Interrupt Status Enable(Low)
    volatile unsigned short     STSENH;                     // 0x036 R/W 0x0000 Normal Interrupt Status Enable(High)
    volatile unsigned short     INTENL;                     // 0x038 R/W 0x0000 Normal Interrupt Signal Enable(Low)
    volatile unsigned short     INTENH;                     // 0x03A R/W 0x0000 Normal Interrupt Signal Enable(High)
    volatile unsigned short     CMD12ERR;                   // 0x03C R 0x0000 Auto CMD12 Error Status
    volatile unsigned short     CONT2;                      // 0x03E R/W 0x0000 Host Control2
    volatile unsigned short     CAPL;                       // 0x040 R 0x30B0 Capabilities(Low)
    volatile unsigned short     CAPH;                       // 0x042 R 0x69EF Capabilities(High)
    volatile unsigned short     CAPL2;                      // 0x044 R 0x0000 Capabilities(Low)
    volatile unsigned short     NOTDEFINE3;                 // 0x046
    volatile unsigned short     CURL;                       // 0x048 R 0x0001 Maximum Current Capabilities(Low)
    volatile unsigned short     CURH;                       // 0x04A R 0x0000 Maximum Current Capabilities(High)
    volatile unsigned short     NOTDEFINE4[2];              // 0x04C, 0x04E
    volatile unsigned short     FORCEL;                     // 0x050 W 0x0000 Force event for AutoCmd12 Error
    volatile unsigned short     FORCEH;                     // 0x052 W 0x0000 Force event for Error Interrupt Status
    volatile unsigned short     AUDIO_DMAERR;               // 0x054 R/W 0x0000 AUDIO DMA Error Status
    volatile unsigned short     NOTDEFINE5;                 // 0x056
    volatile unsigned short     ADDR0;                      // 0x058 R/W 0x0000 AUDIO DMA Address[15:0]
    volatile unsigned short     ADDR1;                      // 0x05A R/W 0x0000 AUDIO DMA Address[31:16]
    volatile unsigned short     ADDR2;                      // 0x05C R/W 0x0000 AUDIO DMA Address[47:32]
    volatile unsigned short     ADDR3;                      // 0x05E R/W 0x0000 AUDIO DMA Address[63:48]
    volatile unsigned short     NOTDEFINE6[8];              // 0x060~0x07E
    volatile unsigned short     BTCONTL;                    // 0x070 R/W 0x0000 Boot data timeout control(LOW)
    volatile unsigned short     BTCONTH;                    // 0x072 R/W 0x0000 Boot data timeout control(High)
    volatile unsigned short     DBGSEL;                     // 0x074 R/W 0x0000 Debug Selection
    volatile unsigned short     NOTDEFINE7[61];             // 0x076~0x07E
    volatile unsigned short     SPIINT;                     // 0x0F0 R 0x0000 SPI Interrupt Support
    volatile unsigned short     NOTDEFINE8[5];              // 0x0F2~0x0FA
    volatile unsigned short     SLOT;                       // 0x0FC R 0x0000 Slot Interrupt Status
    volatile unsigned short     VERSION;                    // 0x0FE R 0x0002 Host Controller Version
} SDHOST, *PSDHOST;

typedef struct _SDCHCTRL{
    volatile unsigned int   SDPORTCTRL;                     // 0x00 R/W 0x0000  Host Controller Control Register
    volatile unsigned int   SD0CMDDAT;                      // 0x04 R/W 0x0000  SD/MMC0 output delay control register
    volatile unsigned int   SD1CMDDAT;                      // 0x08 R/W 0x0000  SD/MMC1 output delay control register
    volatile unsigned int   SD2CMDDAT;                      // 0x0C R/W 0x0000  SD/MMC2 output delay control register
    volatile unsigned int   SD3CMDDAT;                      // 0x10 R/W 0x0000  SD/MMC3 output delay control register
    volatile unsigned int   SD0PRESET1;                     // 0x14 R/W 0x0000  SD/MMC0 Preset Register1
    volatile unsigned int   SD0PRESET2;                     // 0x18 R/W 0x0000  SD/MMC0 Preset Register2
    volatile unsigned int   SD0PRESET3;                     // 0x1C R/W 0x0000  SD/MMC0 Preset Register3
    volatile unsigned int   SD0PRESET4;                     // 0x20 R/W 0x0000  SD/MMC0 Preset Register4
    volatile unsigned int   SD0PRESET5;                     // 0x24 R/W 0x0000  SD/MMC0 Preset Register5
    volatile unsigned int   SD0PRESET6;                     // 0x28 R/W 0x0000  SD/MMC0 Preset Register6
    volatile unsigned int   SD1PRESET1;                     // 0x2C R/W 0x0000  SD/MMC1 Preset Register1
    volatile unsigned int   SD1PRESET2;                     // 0x30 R/W 0x0000  SD/MMC1 Preset Register2
    volatile unsigned int   SD1PRESET3;                     // 0x34 R/W 0x0000  SD/MMC1 Preset Register3
    volatile unsigned int   SD1PRESET4;                     // 0x38 R/W 0x0000  SD/MMC1 Preset Register4
    volatile unsigned int   SD1PRESET5;                     // 0x3C R/W 0x0000  SD/MMC1 Preset Register5
    volatile unsigned int   SD1PRESET6;                     // 0x40 R/W 0x0000  SD/MMC1 Preset Register6
    volatile unsigned int   SD2PRESET1;                     // 0x44 R/W 0x0000  SD/MMC2 Preset Register1
    volatile unsigned int   SD2PRESET2;                     // 0x48 R/W 0x0000  SD/MMC2 Preset Register2
    volatile unsigned int   SD2PRESET3;                     // 0x4C R/W 0x0000  SD/MMC2 Preset Register3
    volatile unsigned int   SD2PRESET4;                     // 0x50 R/W 0x0000  SD/MMC2 Preset Register4
    volatile unsigned int   SD2PRESET5;                     // 0x54 R/W 0x0000  SD/MMC2 Preset Register5
    volatile unsigned int   SD2PRESET6;                     // 0x58 R/W 0x0000  SD/MMC2 Preset Register6
    volatile unsigned int   SD3PRESET1;                     // 0x5C R/W 0x0000  SD/MMC3 Preset Register1
    volatile unsigned int   SD3PRESET2;                     // 0x60 R/W 0x0000  SD/MMC3 Preset Register2
    volatile unsigned int   SD3PRESET3;                     // 0x64 R/W 0x0000  SD/MMC3 Preset Register3
    volatile unsigned int   SD3PRESET4;                     // 0x68 R/W 0x0000  SD/MMC3 Preset Register4
    volatile unsigned int   SD3PRESET5;                     // 0x6C R/W 0x0000  SD/MMC3 Preset Register5
    volatile unsigned int   SD3PRESET6;                     // 0x70 R/W 0x0000  SD/MMC3 Preset Register6
} SDCHCTRL, *PSDCHCTRL;

/*******************************************************************************
*    6. EHI Register Define (Base Addr = 0xF0570000)
********************************************************************************/
//#define HwEHI_BASE								((volatile unsigned long*)0xF0570000)
//#define HwEHI0_BASE								((volatile unsigned long*)0xF0570000)
//#define HwEHI1_BASE								((volatile unsigned long*)0xF0580000)

typedef struct _EHI{
    volatile unsigned int   EHST;               //0x00 R/W R/W 0x00000080 Status register
    volatile unsigned int   EHIINT;             //0x04 R/W R/W 0x00000000 Internal interrupt control register
    volatile unsigned int   EHEINT;             //0x08 R/W R/W 0x00000000 External interrupt control register
    volatile unsigned int   EHA;                //0x0C R R/W   0x00000000 Address register
    volatile unsigned int   EHAM;               //0x10 R/W R   0x00000000 Address masking register
    volatile unsigned int   EHD;                //0x14 R/W R/W 0x00000000 Data register
    volatile unsigned int   EHSEM;              //0x18 R/W R/W 0x00000000 Semaphore register
    volatile unsigned int   EHCFG;              //0x1C R/W R/W 0x00000000 Configuration registers
    volatile unsigned int   EHIND;              //0x20 R W     0x00000000 Index register
    volatile unsigned int   EHRWCS;             //0x24 R R/W   0x00000000 Read/Write Control/Status register
} EHI, *PEHI;

/*******************************************************************************
*    7. DMA Controller Register Define (Base Addr = 0xF0540000)
********************************************************************************/
//#define HwGDMA_BASE								((volatile unsigned long*)0xF0540000)
//#define HwGDMA0_BASE							((volatile unsigned long*)0xF0540000)
//#define HwGDMA1_BASE							((volatile unsigned long*)0xF0540100)
//#define HwGDMA2_BASE							((volatile unsigned long*)0xF0540200)
//#define HwGDMA3_BASE							((volatile unsigned long*)0xF0540300)

typedef struct _GDMACTRL{
    volatile unsigned int  ST_SADR0;                // 0x00 R/W 0x00000000 Start Address of Source Block
    volatile unsigned int  SPARAM0;                 // 0x04 R/W 0x00000000 Parameter of Source Block
    volatile unsigned int  NOTDEFINE0;              // 0x08
    volatile unsigned int  C_SADR0;                 // 0x0C R   0x00000000 Current Address of Source Block
    volatile unsigned int  ST_DADR0;                // 0x10 R/W 0x00000000 Start Address of Destination Block
    volatile unsigned int  DPARAM0;                 // 0x14 R/W 0x00000000 Parameter of Destination Block
    volatile unsigned int  NOTDEFINE1;              // 0x18
    volatile unsigned int  C_DADR0;                 // 0x1C R   0x00000000 Current Address of Destination Block
    volatile unsigned int  HCOUNT0;                 // 0x20 R/W 0x00000000 Initial and Current Hop count
    volatile unsigned int  CHCTRL0;                 // 0x24 R/W 0x00000000 Channel Control Register
    volatile unsigned int  RPTCTRL0;                // 0x28 R/W 0x00000000 Repeat Control Register
    volatile unsigned int  EXTREQ0;                 // 0x2C R/W 0x00000000 External DMA Request Register
    volatile unsigned int  ST_SADR1;                // 0x30 R/W 0x00000000 Start Address of Source Block
    volatile unsigned int  SPARAM1;                 // 0x34 R/W 0x00000000 Parameter of Source Block
    volatile unsigned int  NOTDEFINE2;              // 0x38
    volatile unsigned int  C_SADR1;                 // 0x3C R   0x00000000 Current Address of Source Block
    volatile unsigned int  ST_DADR1;                // 0x40 R/W 0x00000000 Start Address of Destination Block
    volatile unsigned int  DPARAM1;                 // 0x44 R/W 0x00000000 Parameter of Destination Block
    volatile unsigned int  NOTDEFINE3;              // 0x48
    volatile unsigned int  C_DADR1;                 // 0x4C R   0x00000000 Current Address of Destination Block
    volatile unsigned int  HCOUNT1;                 // 0x50 R/W 0x00000000 Initial and Current Hop count
    volatile unsigned int  CHCTRL1;                 // 0x54 R/W 0x00000000 Channel Control Register
    volatile unsigned int  RPTCTRL1;                // 0x58 R/W 0x00000000 Repeat Control Register
    volatile unsigned int  EXTREQ1;                 // 0x5C R/W 0x00000000 External DMA Request Register
    volatile unsigned int  ST_SADR2;                // 0x60 R/W 0x00000000 Start Address of Source Block
    volatile unsigned int  SPARAM2;                 // 0x64 R/W 0x00000000 Parameter of Source Block
    volatile unsigned int  NOTDEFINE4;              // 0x68
    volatile unsigned int  C_SADR2;                 // 0x6C R   0x00000000 Current Address of Source Block
    volatile unsigned int  ST_DADR2;                // 0x70 R/W 0x00000000 Start Address of Destination Block
    volatile unsigned int  DPARAM2;                 // 0x74 R/W 0x00000000 Parameter of Destination Block
    volatile unsigned int  NOTDEFINE5;              // 0x78
    volatile unsigned int  C_DADR2;                 // 0x7C R   0x00000000 Current Address of Destination Block
    volatile unsigned int  HCOUNT2;                 // 0x80 R/W 0x00000000 Initial and Current Hop count
    volatile unsigned int  CHCTRL2;                 // 0x84 R/W 0x00000000 Channel Control Register
    volatile unsigned int  RPTCTRL2;                // 0x88 R/W 0x00000000 Repeat Control Register
    volatile unsigned int  EXTREQ2;                 // 0x8C R/W 0x00000000 External DMA Request Register
    volatile unsigned int  CHCONFIG;                // 0x90 R/W 0x00000000 Channel Configuration Register
} GDMACTRL, *PGDMACTRL;

typedef struct _GDMANCTRL{
    volatile unsigned int   ST_SADR;                // 0x000  R/W   Start Address of Source Block
    volatile unsigned int   SPARAM[2];              // 0x004~ R/W   Parameter of Source Block
    volatile unsigned int   C_SADR;                 // 0x00C  R     Current Address of Source Block
    volatile unsigned int   ST_DADR;                // 0x010  R/W   Start Address of Destination Block
    volatile unsigned int   DPARAM[2];              // 0x014~ R/W   Parameter of Destination Block
    volatile unsigned int   C_DADR;                 // 0x01C  R     Current Address of Destination Block
    volatile unsigned int   HCOUNT;                 // 0x020  R/W   Initial and Current Hop count
    volatile unsigned int   CHCTRL;                 // 0x024  R/W   Channel Control Register
    volatile unsigned int   RPTCTRL;                // 0x028  R/W   Repeat Control Register
    volatile unsigned int   EXTREQ;                 // 0x02C  R/W   External DMA Request Register
} GDMANCTRL, *PGDMANCTRL;

/*******************************************************************************
*    8. GPSB Controller Register Define (Base Addr = 0xF0535000)
********************************************************************************/
//#define HwGPSB_BASE							((volatile unsigned long*)0xF0535000)
//#define HwGPSB0_BASE							((volatile unsigned long*)0xF0535000)
//#define HwGPSB1_BASE							((volatile unsigned long*)0xF0535100)
//#define HwGPSB2_BASE							((volatile unsigned long*)0xF0535200)
//#define HwGPSB3_BASE							((volatile unsigned long*)0xF0535300)
//#define HwGPSB4_BASE							((volatile unsigned long*)0xF0535400)
//#define HwGPSB5_BASE							((volatile unsigned long*)0xF0535500)
//#define HwGPSB_PORTCFG_BASE					((volatile unsigned long*)0xF0535800)
//#define HwGPSB_PIDTABLE_BASE					((volatile unsigned long*)0xF0535F00)

typedef struct _GPSB{
    volatile unsigned int   PORT;               // 0x000 R/W 0x0000 Data port
    volatile unsigned int   STAT;               // 0x004 R/W 0x0000 Status register
    volatile unsigned int   INTEN;              // 0x008 R/W 0x0000 Interrupt enable
    volatile unsigned int   MODE;               // 0x00C R/W 0x0004 Mode register
    volatile unsigned int   CTRL;               // 0x010 R/W 0x0000 Control register
    volatile unsigned int   EVTCTRL;            // 0x014 R/W 0x0000 Counter & Ext. Event Control
    volatile unsigned int   CCV;                // 0x018 R 0x0000 Counter Current Value
    volatile unsigned int   NOTDEFINE0;
    volatile unsigned int   TXBASE;             // 0x020 R/W 0x0000 TX base address register
    volatile unsigned int   RXBASE;             // 0x024 R/W 0x0000 RX base address register
    volatile unsigned int   PACKET;             // 0x028 R/W 0x0000 Packet register
    volatile unsigned int   DMACTR;             // 0x02C R/W 0x0000 DMA control register
    volatile unsigned int   DMASTR;             // 0x030 R/W 0x0000 DMA status register
    volatile unsigned int   DMAICR;             // 0x034 R/W 0x0000 DMA interrupt control register
} GPSB, *PGPSB, sHwGPSB;

typedef struct _NONDMA_GPSB{
    volatile unsigned int   PORT;               // 0x000 R/W 0x0000 Data port
    volatile unsigned int   STAT;               // 0x004 R/W 0x0000 Status register
    volatile unsigned int   INTEN;              // 0x008 R/W 0x0000 Interrupt enable
    volatile unsigned int   MODE;               // 0x00C R/W 0x0004 Mode register
    volatile unsigned int   CTRL;               // 0x010 R/W 0x0000 Control register
    volatile unsigned int   EVTCTRL;            // 0x014 R/W 0x0000 Counter & Ext. Event Control
    volatile unsigned int   CCV;                // 0x018 R 0x0000 Counter Current Value
} NONDMA_GPSB, *PNONDMA_GPSB;

typedef struct _GPSBPORTCFG{
    volatile unsigned int   PCFG0;              // 0x800 R/W 0x03020100 Port Configuration Register 0
    volatile unsigned int   PCFG1;              // 0x804 R/W 0x00000504 Port Configuration Port Config Register 1
    volatile unsigned int   CIRQST;             // 0x808 R 0x0000 Channel IRQ Status Register
} GPSBPORTCFG, *PGPSBPORTCFG;

typedef struct _GPSBPIDTABLE{
    volatile unsigned int   PIDT[0x80/4]; // 0xF00 R/W   PID Table
} GPSBPIDTABLE, *PGPSBPIDTABLE;

/*******************************************************************************
*    9. Overlay Mixer Register Define (Base Addr = 0xF0560000)
********************************************************************************/
//#define HwOVERLAYMIXER_BASE						((volatile unsigned long*)0xF0560000)

typedef struct _OVERLAYMIXER{
    volatile unsigned int   FCH0_SADDR0;        // 0x00 R/W 0x00000000 Front-End Channel 0 Source Address 0
    volatile unsigned int   FCH0_SADDR1;        // 0x04 R/W 0x00000000 Front-End Channel 0 Source Address 1
    volatile unsigned int   FCH0_SADDR2;        // 0x08 R/W 0x00000000 Front-End Channel 0 Source Address 2
    volatile unsigned int   FCH0_SFSIZE;        // 0x0C R/W 0x00000000 Front-End Channel 0 Source Frame Pixel Size
    volatile unsigned int   FCH0_SOFF;          // 0x10 R/W 0x00000000 Front-End Channel 0 Source Pixel Offset
    volatile unsigned int   FCH0_SISIZE;        // 0x14 R/W 0x00000000 Front-End Channel 0 Source Image Pixel Size
    volatile unsigned int   FCH0_WOFF;          // 0x18 R/W 0x00000000 Front-End Channel 0 Window Pixel Offset
    volatile unsigned int   FCH0_SCTRL;         // 0x1C R/W 0x00000000 Front-End Channel 0 Control
    volatile unsigned int   FCH1_SADDR0;        // 0x20 R/W 0x00000000 Front-End Channel 1 Source Address 0
    volatile unsigned int   FCH1_SADDR1;        // 0x24 R/W 0x00000000 Front-End Channel 1 Source Address 1
    volatile unsigned int   FCH1_SADDR2;        // 0x28 R/W 0x00000000 Front-End Channel 1 Source Address 2
    volatile unsigned int   FCH1_SFSIZE;        // 0x2C R/W 0x00000000 Front-End Channel 1 Source Frame Pixel Size
    volatile unsigned int   FCH1_SOFF;          // 0x30 R/W 0x00000000 Front-End Channel 1 Source Pixel Offset
    volatile unsigned int   FCH1_SISIZE;        // 0x34 R/W 0x00000000 Front-End Channel 1 Source Image Pixel Size
    volatile unsigned int   FCH1_WOFF;          // 0x38 R/W 0x00000000 Front-End Channel 1 Window Pixel Offset
    volatile unsigned int   FCH1_SCTRL;         // 0x3C R/W 0x00000000 Front-End Channel 1 Control
    volatile unsigned int   FCH2_SADDR0;        // 0x40 R/W 0x00000000 Front-End Channel 1 Source Address 0
    volatile unsigned int   FCH2_SADDR1;        // 0x44 R/W 0x00000000 Front-End Channel 1 Source Address 1
    volatile unsigned int   FCH2_SADDR2;        // 0x48 R/W 0x00000000 Front-End Channel 1 Source Address 2
    volatile unsigned int   FCH2_SFSIZE;        // 0x4C R/W 0x00000000 Front-End Channel 1 Source Frame Pixel Size
    volatile unsigned int   FCH2_SOFF;          // 0x50 R/W 0x00000000 Front-End Channel 1 Source Pixel Offset
    volatile unsigned int   FCH2_SISIZE;        // 0x54 R/W 0x00000000 Front-End Channel 1 Source Image Pixel Size
    volatile unsigned int   FCH2_WOFF;          // 0x58 R/W 0x00000000 Front-End Channel 1 Window Pixel Offset
    volatile unsigned int   FCH2_SCTRL;         // 0x5C R/W 0x00000000 Front-End Channel 1 Control
    volatile unsigned int   S0_CHROMA;          // 0x60 R/W 0x00000000 Source 0 Chroma-Key Parameter
    volatile unsigned int   S0_PAR;             // 0x64 R/W 0x00000000 Source 0 Arithmetic Parameter
    volatile unsigned int   S1_CHROMA;          // 0x68 R/W 0x00000000 Source 1 Chroma-Key Parameter
    volatile unsigned int   S1_PAR;             // 0x6C R/W 0x00000000 Source 1 Arithmetic Parameter
    volatile unsigned int   S2_CHROMA;          // 0x70 R/W 0x00000000 Source 2 Chroma-Key Parameter
    volatile unsigned int   S2_PAR;             // 0x74 R/W 0x00000000 Source 2 Arithmetic Parameter
    volatile unsigned int   S_CTRL;             // 0x78 R/W 0x00000000 Source Control Register
    volatile unsigned int   NOTDEFINE0;         // 0x7C - - Reserved
    volatile unsigned int   OP0_PAT;            // 0x80 R/W 0x00000000 Source Operator 0 Pattern
    volatile unsigned int   OP1_PAT;            // 0x84 R/W 0x00000000 Source Operator 1 Pattern
    volatile unsigned int   OP_CTRL;            // 0x88 R/W 0x00000000 Source Operation Control Register
    volatile unsigned int   NOTDEFINE1;         // 0x8C - - Reserved
    volatile unsigned int   BCH_DADDR0;         // 0x90 R/W 0x00000000 Back-End Channel Destination Address 0
    volatile unsigned int   BCH_DADDR1;         // 0x94 R/W 0x00000000 Back -End Channel Destination Address 1
    volatile unsigned int   BCH_DADDR2;         // 0x98 R/W 0x00000000 Back -End Channel Destination Address 2
    volatile unsigned int   BCH_DFSIZE;         // 0x9C R/W 0x00000000 Back -End Channel Destination Frame Pixel Size
    volatile unsigned int   BCH_DOFF;           // 0xA0 R/W 0x00000000 Back -End Channel Destination Pixel Offset
    volatile unsigned int   BCH_DCTRL;          // 0xA4 R/W 0x00000000 Back -End Channel Control

    volatile unsigned int   NOTDEFINE2[2];      // 0xA8 - 0xAF - - Reserved
    volatile unsigned int   BCH_DDMAT0;         // 0xB0 R/W 0x00000000 Back-End Channel Destination Dither Matrix 0
    volatile unsigned int   BCH_DDMAT1;         // 0xB4 R/W 0x00000000 Back-End Channel Destination Dither Matrix 1
    volatile unsigned int   BCH_DDMAT2;         // 0xB8 R/W 0x00000000 Back-End Channel Destination Dither Matrix 2
    volatile unsigned int   BCH_DDMAT3;         // 0xBC R/W 0x00000000 Back-End Channel Destination Dither Matrix 3
    volatile unsigned int   OM_CTRL;            // 0xC0 R/W 0x00000000 Overlay Mixer Control
    volatile unsigned int   OM_IREQ;            // 0xC4 R/W 0x00000000 Overlay Mixer Interrupt Request
    volatile unsigned int   NOTDEFINE3[206];    // 0xC8 - 0x3FF - - Reserved

    volatile unsigned int   FCH0_LUT[256];      // 0x400 ? 0x7FF R/W - Front-End Channel 0 Lookup Table
    volatile unsigned int   FCH1_LUT[256];      // 0x800 ? 0xBFF R/W - Front-End Channel 1 Lookup Table
    volatile unsigned int   FCH2_LUT[256];      // 0xC00 ? 0xFFF R/W - Front-End Channel 2 Lookup Table
}OVERLAYMIXER, *POVERLAYMIXER;

/*******************************************************************************
*    10. Audio0(7.1ch) Register Define (Base Addr = 0xF0532000)
********************************************************************************/

/*******************************************************************************
*   10-1. Audio DMA Controller Register 7.1 ch Define   (Base Addr = 0xF0532000)
********************************************************************************/
//#define HwAUDIO0_DMA_BASE                       ((volatile unsigned long*)0xF0532000)
typedef struct _ADMA{
    volatile unsigned int   RxDaDar;             //  0x00 R/W 0x00000000 DAI Rx (Right) Data Destination Address
    volatile unsigned int   RxDaParam;           //  0x04 R/W 0x00000000 DAI Rx Parameters
    volatile unsigned int   RxDaTCnt;            //  0x08 R/W 0x00000000 DAI Rx Transmission Counter Register
    volatile unsigned int   RxDaCdar;            //  0x0C R 0x00000000 DAI Rx (Right) Data Current Destination Address
    volatile unsigned int   RxCdDar;             //  0x10 R/W 0x00000000 CDIF(SPDIF) Rx (Right) Data Destination Address
    volatile unsigned int   RxCdParam;           //  0x14 R/W 0x00000000 CDIF(SPDIF) Rx Parameters
    volatile unsigned int   RxCdTCnt;            //  0x18 R/W 0x00000000 CDIF(SPDIF) Rx Transmission Counter Register
    volatile unsigned int   RxCdCdar;            //  0x1C R 0x00000000 CDIF(SPDIF) Rx (Right) Data Current Destination Address
    volatile unsigned int   NOTDEFINE0[2];
    volatile unsigned int   RxDaDarL;            //  0x28 R/W 0x00000000 DAI Rx Left Data Destination Address
    volatile unsigned int   RxDaCdarL;           //  0x2C R 0x00000000 DAI Rx Left Data Current Destination Address
    volatile unsigned int   RxCdDarL;            //  0x30 R/W 0x00000000 CDIF(SPDIF) Rx Left Data Destination Address
    volatile unsigned int   RxCdCdarL;           //  0x34 R 0x00000000 CDIF(SPDIF) Rx Left Data Current Destination Address
    volatile unsigned int   TransCtrl;           //  0x38 R/W 0x0000AA00 DMA Transfer Control Register
    volatile unsigned int   RptCtrl;             //  0x3C R/W 0x00000000 DMA Repeat Control Register
    volatile unsigned int   TxDaSar;             //  0x40 R/W 0x00000000 DAI Tx (Right) Data Source Address
    volatile unsigned int   TxDaParam;           //  0x44 R/W 0x00000000 DAI Tx Parameters
    volatile unsigned int   TxDaTCnt;            //  0x48 R/W 0x00000000 DAI Tx Transmission Counter Register
    volatile unsigned int   TxDaCsar;            //  0x4C R 0x00000000 DAI Tx (Right) Data Current Source Address
    volatile unsigned int   TxSpSar;             //  0x50 R/W 0x00000000 SPDIF Tx (Right) Data Source Address
    volatile unsigned int   TxSpParam;           //  0x54 R/W 0x00000000 SPDIF Tx Parameters
    volatile unsigned int   TxSpTCnt;            //  0x58 R/W 0x00000000 SPDIF Tx Transmission Counter Register
    volatile unsigned int   TxSpCsar;            //  0x5C R 0x00000000 SPDIF Tx (Right) Data Current Source Address
    volatile unsigned int   NOTDEFINE1[2];
    volatile unsigned int   TxDaSarL;            //  0x68 R/W 0x00000000 DAI Tx Left Data Source Address
    volatile unsigned int   TxDaCsarL;           //  0x6C R 0x00000000 DAI Tx Left Data Current Source Address
    volatile unsigned int   TxSpSarL;            //  0x70 R/W 0x00000000 SPDIF Tx Left Data Source Address
    volatile unsigned int   TxSpCsarL;           //  0x74 R 0x00000000 SPDIF Tx Left Data Current Source address
    volatile unsigned int   ChCtrl;              //  0x78 R/W 0x00008000 DMA Channel Control Register
    volatile unsigned int   IntStatus;           //  0x7C R/W 0x00000000 DMA Interrupt Status Register
    volatile unsigned int   GIntReq;             //  0x80 R/W 0x00000000 General Interrupt Request
    volatile unsigned int   GIntStatus;          //  0x84 R 0x00000000 General Interrupt Status
    volatile unsigned int   NOTDEFINE2[6];
    volatile unsigned int   RxDaDar1;            //  0x100 R/W 0x00000000 DAI1 Rx (Right) Data Destination Address
    volatile unsigned int   RxDaDar2;            //  0x104 R/W 0x00000000 DAI2 Rx (Right) Data Destination Address
    volatile unsigned int   RxDaDar3;            //  0x108 R/W 0x00000000 DAI3 Rx (Right) Data Destination Address
    volatile unsigned int   RxDaCar1;            //  0x10C R 0x00000000 DAI1 Rx (Right) Data Current Destination Address
    volatile unsigned int   RxDaCar2;            //  0x110 R 0x00000000 DAI2 Rx (Right) Data Current Destination Address
    volatile unsigned int   RxDaCar3;            //  0x114 R 0x00000000 DAI3 Rx (Right) Data Current Destination Address
    volatile unsigned int   RxDaDarL1;           //  0x118 R/W 0x00000000 DAI1 Rx Left Data Destination Address
    volatile unsigned int   RxDaDarL2;           //  0x11C R/W 0x00000000 DAI2 Rx Left Data Destination Address
    volatile unsigned int   RxDaDarL3;           //  0x120 R/W 0x00000000 DAI3 Rx Left Data Destination Address
    volatile unsigned int   RxDaCarL1;           //  0x124 R 0x00000000 DAI1 Rx Left Data Current Destination Address
    volatile unsigned int   RxDaCarL2;           //  0x128 R 0x00000000 DAI2 Rx Left Data Current Destination Address
    volatile unsigned int   RxDaCarL3;           //  0x12C R 0x00000000 DAI3 Rx Left Data Current Destination Address
    volatile unsigned int   TxDaSar1;            //  0x130 R/W 0x00000000 DAI1 Tx (Right) Data Source Address
    volatile unsigned int   TxDaSar2;            //  0x134 R/W 0x00000000 DAI2 Tx (Right) Data Source Address
    volatile unsigned int   TxDaSar3;            //  0x138 R/W 0x00000000 DAI3 Tx (Right) Data Source Address
    volatile unsigned int   TxDaCsar1;           //  0x13C R 0x00000000 DAI1 Tx (Right) Data Current Source Address
    volatile unsigned int   TxDaCsar2;           //  0x140 R 0x00000000 DAI2 Tx (Right) Data Current Source Address
    volatile unsigned int   TxDaCsar3;           //  0x144 R 0x00000000 DAI3 Tx (Right) Data Current Source Address
    volatile unsigned int   TxDaDarL1;           //  0x148 R/W 0x00000000 DAI1 Tx Left Data Source Address
    volatile unsigned int   TxDaDarL2;           //  0x14C R/W 0x00000000 DAI2 Tx Left Data Source Address
    volatile unsigned int   TxDaDarL3;           //  0x150 R/W 0x00000000 DAI3 Tx Left Data Source Address
    volatile unsigned int   TxDaCarL1;           //  0x154 R 0x00000000 DAI1 Tx Left Data Current Source Address
    volatile unsigned int   TxDaCarL2;           //  0x158 R 0x00000000 DAI2 Tx Left Data Current Source Address
    volatile unsigned int   TxDaCarL3;           //  0x15C R 0x00000000 DAI3 Tx Left Data Current Source Address
} ADMA, *PADMA;

/*******************************************************************************
*   10-2. Audio DMA DAI Register 7.1 ch Define          (Base Addr = 0xF0533000)
********************************************************************************/
//#define HwAUDIO0_DAI_BASE                       ((volatile unsigned long*)0xF0533000)
typedef struct _ADMADAI{
    volatile unsigned int   DADIR0;              //  0x00 R - Digital Audio Input Register 0
    volatile unsigned int   DADIR1;              //  0x04 R - Digital Audio Input Register 1
    volatile unsigned int   DADIR2;              //  0x08 R - Digital Audio Input Register 2
    volatile unsigned int   DADIR3;              //  0x0C R - Digital Audio Input Register 3
    volatile unsigned int   DADIR4;              //  0x10 R - Digital Audio Input Register 4
    volatile unsigned int   DADIR5;              //  0x14 R - Digital Audio Input Register 5
    volatile unsigned int   DADIR6;              //  0x18 R - Digital Audio Input Register 6
    volatile unsigned int   DADIR7;              //  0x1C R - Digital Audio Input Register 7
    volatile unsigned int   DADOR0;              //  0x20 R/W - Digital Audio Output Register 0
    volatile unsigned int   DADOR1;              //  0x24 R/W - Digital Audio Output Register 1
    volatile unsigned int   DADOR2;              //  0x28 R/W - Digital Audio Output Register 2
    volatile unsigned int   DADOR3;              //  0x2C R/W - Digital Audio Output Register 3
    volatile unsigned int   DADOR4;              //  0x30 R/W - Digital Audio Output Register 4
    volatile unsigned int   DADOR5;              //  0x34 R/W - Digital Audio Output Register 5
    volatile unsigned int   DADOR6;              //  0x38 R/W - Digital Audio Output Register 6
    volatile unsigned int   DADOR7;              //  0x3C R/W - Digital Audio Output Register 7
    volatile unsigned int   DAMR;                //  0x40 R/W 0x00000000 Digital Audio Mode Register
    volatile unsigned int   DAVC;                //  0x44 R/W 0x0000 Digital Audio Volume Control Register
    volatile unsigned int   MCCR0;               //  0x48 R/W 0x00000000 Multi Channel Control Register 0
    volatile unsigned int   MCCR1;               //  0x4C R/W 0x00000000 Multi Channel Control Register 1
} ADMADAI, *PADMADAI;

/*******************************************************************************
*   10-3. Audio DMA CDIF Register 7.1 ch Define   (Base Addr = 0xF0533080)
********************************************************************************/
//#define HwAUDIO0_CDIF_BASE                      ((volatile unsigned long*)0xF0533080)
typedef struct _ADMACDIF{
    volatile unsigned int   CDDI_0;               //  0x80 R CD Digital Audio Input Register 0
    volatile unsigned int   CDDI_1;               //  0x84 R CD Digital Audio Input Register 1
    volatile unsigned int   CDDI_2;               //  0x88 R CD Digital Audio Input Register 2
    volatile unsigned int   CDDI_3;               //  0x8C R CD Digital Audio Input Register 3
    volatile unsigned int   CDDI_4;               //  0x90 R CD Digital Audio Input Register 4
    volatile unsigned int   CDDI_5;               //  0x94 R CD Digital Audio Input Register 5
    volatile unsigned int   CDDI_6;               //  0x98 R CD Digital Audio Input Register 6
    volatile unsigned int   CDDI_7;               //  0x9C R CD Digital Audio Input Register 7
    volatile unsigned int   CICR;                 //  0xA0 R/W 0x0000 CD Interface Control Register
} ADMACDIF, *PADMACDIF;

/*******************************************************************************
*   10-4/10-5. ADMA_SPDIF Tx/Rx Register 7.1 ch Define   (Base Addr = 0xF0534000/0xF0534800)
********************************************************************************/
//#define HwAUDIO0_SPDIFTX_BASE                   ((volatile unsigned long*)0xF0534000)
//#define HwAUDIO0_SPDIFRX_BASE                   ((volatile unsigned long*)0xF0534800)
typedef struct _ADMASPDIFTX{
    volatile unsigned int   TxVersion;            //  0x00  R   0x00003111 Version Register
    volatile unsigned int   TxConfig;             //  0x04  R/W 0x00000000 Configuration Register
    volatile unsigned int   TxChStat;             //  0x08  R/W 0x00000000 Channel Status Control Register
    volatile unsigned int   TxIntMask;            //  0x0C  R/W 0x00000000 Interrupt Mask Register
    volatile unsigned int   TxIntStat;            //  0x10  R/W 0x00000000 Interrupt Status Register
    volatile unsigned int   NOTDEFINE0[27];
    volatile unsigned int   UserData[24];         //  0x80~0xDC   W -      User Data Buffer
    volatile unsigned int   NOTDEFINE1[8];
    volatile unsigned int   ChStatus[24];         //  0x100~0x15C W -      Channel Status Buffer
    volatile unsigned int   NOTDEFINE2[40];
    volatile unsigned int   TxBuffer[128];        //  0x200~0x23C W -      Transmit Data Buffer
    volatile unsigned int   DMACFG;               //  0x400 R/W 0x00000007 Additional Configuration for DMA
    //volatile unsigned int NOTDEFINE4[159];
    //volatile unsigned int CSBUDB[24];           //  0x680~0x6DC W -      Merged Window for CSB/UDB
} ADMASPDIFTX, *PADMASPDIFTX;

typedef union _RXCAP{
    volatile unsigned int   RxCapCtln[16];        //  0x840~0x87C(even) W 0x00000000 Channel Status Capture Control Register
    volatile unsigned int   RxCapn[16];           //  0x840~0x87C(odd)  W 0x00000000 Captured Channel Status / user bit
} RXCAP;

typedef struct _ADMASPDIFRX{
    volatile unsigned int   RxVersion;            //  0x800 R   0x00080111 Version Register
    volatile unsigned int   RxConfig;             //  0x804 R/W 0x00000000 Configuration Register
    volatile unsigned int   RxStatus;             //  0x808 R   0x00000000 Signal Status Buffer
    volatile unsigned int   RxIntMask;            //  0x80C R/W 0x00000000 Interrupt Mask Register
    volatile unsigned int   RxIntStat;            //  0x810 R/W 0x00000000 Interrupt Status register
    volatile unsigned int   NOTDEFINE0[11];
    RXCAP   RxCap;
    //volatile unsigned int RxCapCtln[16];        //  0x840~0x87C(even) W 0x00000000 Channel Status Capture Control Register
    //volatile unsigned int RxCapn[16];           //  0x840~0x87C(odd)  W 0x00000000 Captured Channel Status / user bit
    volatile unsigned int   RxBuffer[8];          //  0xA00~0xA1C       W -          Receive Data Buffer
} ADMASPDIFRX, *PADMASPDIFRX;



/*******************************************************************************
*    11. Audio1(Stereo) Register Define (Base Addr = 0xF0536000)
********************************************************************************/

/*******************************************************************************
*   11-2. Audio DAI Register Stereo Define              (Base Addr = 0xF0536000)
********************************************************************************/
//#define HwAUDIO1_DAI0_BASE                      ((volatile unsigned long*)0xF0536000)
//#define HwAUDIO1_DAI1_BASE                      ((volatile unsigned long*)0xF0536100)
typedef struct _STEREOADMADAI{
    volatile unsigned int   DADIR0;             // 0x00 R   -   Digital Audio Input Register 0
    volatile unsigned int   DADIR1;             // 0x04 R   -   Digital Audio Input Register 1
    volatile unsigned int   DADIR2;             // 0x08 R   -   Digital Audio Input Register 2
    volatile unsigned int   DADIR3;             // 0x0C R   -   Digital Audio Input Register 3
    volatile unsigned int   DADIR4;             // 0x10 R   -   Digital Audio Input Register 4
    volatile unsigned int   DADIR5;             // 0x14 R   -   Digital Audio Input Register 5
    volatile unsigned int   DADIR6;             // 0x18 R   -   Digital Audio Input Register 6
    volatile unsigned int   DADIR7;             // 0x1C R   -   Digital Audio Input Register 7
    volatile unsigned int   DADOR0;             // 0x20 R/W -   Digital Audio Output Register 0
    volatile unsigned int   DADOR1;             // 0x24 R/W -   Digital Audio Output Register 1
    volatile unsigned int   DADOR2;             // 0x28 R/W -   Digital Audio Output Register 2
    volatile unsigned int   DADOR3;             // 0x2C R/W -   Digital Audio Output Register 3
    volatile unsigned int   DADOR4;             // 0x30 R/W -   Digital Audio Output Register 4
    volatile unsigned int   DADOR5;             // 0x34 R/W -   Digital Audio Output Register 5
    volatile unsigned int   DADOR6;             // 0x38 R/W -   Digital Audio Output Register 6
    volatile unsigned int   DADOR7;             // 0x3C R/W -   Digital Audio Output Register 7
    volatile unsigned int   DAMR;               // 0x40 R/W 0x00000000  Digital Audio Mode Register
    volatile unsigned int   DAVC;               // 0x44 R/W 0x0000  Digital Audio Volume Control Register
} STREREOADMADAI, *PSTREREOADMADAI;

/*******************************************************************************
*    11-3. Audio DMA CDIF Register Stereo Define        (Base Addr = 0xF0536080)
********************************************************************************/
//#define HwAUDIO1_CDIF_BASE                      ((volatile unsigned long*)0xF0536080)
typedef struct _STEREOADMACDIF{
    volatile unsigned int   CDDI_0;         // 0x80 R       CD Digital Audio Input Register 0
    volatile unsigned int   CDDI_1;         // 0x84 R       CD Digital Audio Input Register 1
    volatile unsigned int   CDDI_2;         // 0x88 R       CD Digital Audio Input Register 2
    volatile unsigned int   CDDI_3;         // 0x8C R       CD Digital Audio Input Register 3
    volatile unsigned int   CDDI_4;         // 0x90 R       CD Digital Audio Input Register 4
    volatile unsigned int   CDDI_5;         // 0x94 R       CD Digital Audio Input Register 5
    volatile unsigned int   CDDI_6;         // 0x98 R       CD Digital Audio Input Register 6
    volatile unsigned int   CDDI_7;         // 0x9C R       CD Digital Audio Input Register 7
    volatile unsigned int   CICR;           // 0xA0 R/W 0x0000  CD Interface Control Register
} STEREOADMACDIF, *PSTEREOADMACDIF;

/*******************************************************************************
*    11-4. Aduio DMA SPDIF Tx Register Stereo Define    (Base Addr = 0xF0536100)
********************************************************************************/
//#define HwAUDIO1_SPDIFTX_BASE                   ((volatile unsigned long*)0xF0537000)
typedef struct _STEREOADMASPDIF{
    volatile unsigned int   TxVersion;            // 0x00 R Version Register
    volatile unsigned int   TxConfig;             // 0x04 R/W Configuration Register
    volatile unsigned int   TxChStat;             // 0x08 R/W Channel Status Control Register
    volatile unsigned int   TxIntMask;            // 0x0C R/W Interrupt Mask Register
    volatile unsigned int   TxIntStat;            // 0x10 R/W Interrupt Status Register
    volatile unsigned int   UserData[24];         // 0x80~0xDC W - User Data Buffer
    volatile unsigned int   ChStatus[24];         // 0x100~0x15C W - Channel Status Buffer
    volatile unsigned int   TxBuffer[128];        // 0x200~0x3FC W - Transmit Data Buffer
    volatile unsigned int   DMACFG;               // 0x400 R/W - Additional Configuration for DMA
    volatile unsigned int   CSBUDB[24];           // 0x680~0x6DC W - Merged Windows for CSB/UDB
} STEREOADMASPDIF, *PSTEREOADMASPDIF;


/*******************************************************************************
*    12. NFC(NAND Flash Controller) Register Define (Base Addr = 0xF05B0000)
********************************************************************************/
//#define HwNFC_BASE								((volatile unsigned long*)0xF05B0000)
//#define HwECC_BASE								((volatile unsigned long*)0xF05B0200)
//#define HwNFCDMA_BASE								((volatile unsigned long*)0xF05B0100)
//#define HwNFCSUBPAGE_BASE							((volatile unsigned long*)0xF05B0120)
//#define HwNFCECCERR_BASE							((volatile unsigned long*)0xF05B0260)
//#define HwNFCMEMORY_BASE							((volatile unsigned long*)0xF05B0400)
typedef struct _NFC{
    volatile unsigned int   NFC_CMD;                // 0x00     W       NAND Flash Command Register
    volatile unsigned int   NFC_LADDR;              // 0x04     W       NAND Flash Linear Address Register
    volatile unsigned int   NFC_SADDR;              // 0x08     W       NAND Flash Signal Address Register
    volatile unsigned int   NFC_SDATA;              // 0x0C     R/W     NAND Flash Single Data Register
    volatile unsigned int   NFC_WDATA;           	// 0x1x     R/W     NAND Flash Word Data Register
    volatile unsigned int	NOTDEFINE0[3];
    volatile unsigned int   NFC_LDATA;           	// 0x2x/3x R/W      NAND Flash Linear Data Register
    volatile unsigned int	NOTDEFINE1[7];			// 0x01x  R/W	NAND Flash Word Data Register
    volatile unsigned int   NFC_MDATA;              // 0x40     R/W     NAND Flash Multiple Data Register
    volatile unsigned int   NFC_CACYC;              // 0x44     R/W     NAND Flash Command & Address Cycle Register
    volatile unsigned int   NFC_WRCYC;              // 0x48     R/W     NAND Flash Write Cycle Register
    volatile unsigned int   NFC_RECYC;              // 0x4C     R/W     NAND Flash Read Cycle Register
    volatile unsigned int   NFC_CTRL;               // 0x50     R/W     NFC Control Register
    volatile unsigned int   NFC_DSIZE;              // 0x54     R/W     NFC Data Size Register
    volatile unsigned int   NFC_SPARE;              // 0x58     R/W     NFC Spare Burst Transfer Control Register
    volatile unsigned int   NFC_PRSTART;            // 0x5C     R/W     NFC Burst Program/Read Start
    volatile unsigned int   NFC_RST;                // 0X60     R/W     NFC Reset Register
    volatile unsigned int   NFC_IRQCFG;             // 0x64     R/W     NFC Interrupt Request Control Register
    volatile unsigned int   NFC_IRQ;                // 0x68     R/W     NFC Interrupt Request Register
    volatile unsigned int   NFC_STATUS;             // 0x6C     R/W     NFC Stutus Register
    volatile unsigned int   NFC_RFWBASE;            // 0x70     R/W     NFC Read Flag Wait Base Word Register
    volatile unsigned int   NOTDEFINE2[61];         // 0x74~0xFC
    volatile unsigned int   NDMA_ADDR;              // 0X100    R/W     NFC DMA Source/Destination Register
    volatile unsigned int   NDMA_PARAM;             // 0X104    R/W     NFC DMA Parameter Register
    volatile unsigned int   NDMA_CADDR;             // 0X108    R       NFC DMA Current Address Register
    volatile unsigned int   NDMA_CTRL;              // 0x10C    R/W     NFC DMA Controll Register
    volatile unsigned int   NDMA_SPCTRL;            // 0x110    R/W     NFC DMA SUBPAGE Control Regiter
    volatile unsigned int   NDMA_STATUS;            // 0x114    R/W     NFC DMA Stutus Register
    volatile unsigned int   NOTDEFINE3[2];          // 0x118~011C
    volatile unsigned int   SP_CFG0;                // 0x120    R/W     NFC Sub Page Configuration Register 0
    volatile unsigned int   SP_CFG1;                // 0x124    R/W     NFC Sub Page Configuration Register 1
    volatile unsigned int   SP_CFG2;                // 0x128    R/W     NFC Sub Page Configuration Register 2
    volatile unsigned int   SP_CFG3;                // 0x12C    R/W     NFC Sub Page Configuration Register 3
    volatile unsigned int   SP_CFG4;                // 0x130    R/W     NFC Sub Page Configuration Register 4
    volatile unsigned int   SP_CFG5;                // 0x134    R/W     NFC Sub Page Configuration Register 5
    volatile unsigned int   SP_CFG6;                // 0x138    R/W     NFC Sub Page Configuration Register 6
    volatile unsigned int   SP_CFG7;                // 0x13C    R/W     NFC Sub Page Configuration Register 7
    volatile unsigned int   SP_CFG8;                // 0x140    R/W     NFC Sub Page Configuration Register 8
    volatile unsigned int   SP_CFG9;                // 0x144    R/W     NFC Sub Page Configuration Register 9
    volatile unsigned int   SP_CFG10;               // 0x148    R/W     NFC Sub Page Configuration Register 10
    volatile unsigned int   SP_CFG11;               // 0x14C    R/W     NFC Sub Page Configuration Register 11
    volatile unsigned int   SP_CFG12;               // 0x150    R/W     NFC Sub Page Configuration Register 12
    volatile unsigned int   SP_CFG13;               // 0x154    R/W     NFC Sub Page Configuration Register 13
    volatile unsigned int   SP_CFG14;               // 0x158    R/W     NFC Sub Page Configuration Register 14
    volatile unsigned int   SP_CFG15;               // 0x15C    R/W     NFC Sub Page Configuration Register 15
    volatile unsigned int   SP_CFG16;               // 0x160    R/W     NFC Sub Page Configuration Register 16
    volatile unsigned int   SP_CFG17;               // 0x164    R/W     NFC Sub Page Configuration Register 17
    volatile unsigned int   SP_CFG18;               // 0x168    R/W     NFC Sub Page Configuration Register 18
    volatile unsigned int   SP_CFG19;               // 0x16C    R/W     NFC Sub Page Configuration Register 19
    volatile unsigned int   SP_CFG20;               // 0x170    R/W     NFC Sub Page Configuration Register 20
    volatile unsigned int   SP_CFG21;               // 0x174    R/W     NFC Sub Page Configuration Register 21
    volatile unsigned int   SP_CFG22;               // 0x178    R/W     NFC Sub Page Configuration Register 22
    volatile unsigned int   SP_CFG23;               // 0x17C    R/W     NFC Sub Page Configuration Register 23
    volatile unsigned int   SP_CFG24;               // 0x180    R/W     NFC Sub Page Configuration Register 24
    volatile unsigned int   SP_CFG25;               // 0x184    R/W     NFC Sub Page Configuration Register 25
    volatile unsigned int   SP_CFG26;               // 0x188    R/W     NFC Sub Page Configuration Register 26
    volatile unsigned int   SP_CFG27;               // 0x18C    R/W     NFC Sub Page Configuration Register 27
    volatile unsigned int   SP_CFG28;               // 0x190    R/W     NFC Sub Page Configuration Register 28
    volatile unsigned int   SP_CFG29;               // 0x194    R/W     NFC Sub Page Configuration Register 29
    volatile unsigned int   SP_CFG30;               // 0x198    R/W     NFC Sub Page Configuration Register 30
    volatile unsigned int   SP_CFG31;               // 0x19C    R/W     NFC Sub Page Configuration Register 31
} NFC, *PNFC;

typedef struct _ECC{
	volatile unsigned int   ECC_CTRL;               // 0x00     W       NAND Flash Command Register
    volatile unsigned int   ERRNUM;           		// 0x04     W       NAND Flash Linear Address Register
    volatile unsigned int   ECC_BASE;				// 0x0C			 Reserved
    volatile unsigned int   ECC_MASK;				// 0x0C			 Reserved
	volatile unsigned int   ECC_CLEAR;
	volatile unsigned int   ECC_RSTART;
   	volatile unsigned int   ECC_STLDCTL;
	volatile unsigned int   ECC_STATUS;
	volatile unsigned int	ECC_CODE0;				// 0x220  R/W	1st  ECC Code Register
	volatile unsigned int	ECC_CODE1;				// 0x224  R/W	2nd  ECC Code Register
	volatile unsigned int	ECC_CODE2;				// 0x228  R/W	3rd  ECC Code Register
	volatile unsigned int	ECC_CODE3;				// 0x22C  R/W	4th  ECC Code Register
	volatile unsigned int	ECC_CODE4;				// 0x230  R/W	5th  ECC Code Register
	volatile unsigned int	ECC_CODE5;				// 0x234  R/W	6th  ECC Code Register
	volatile unsigned int	ECC_CODE6;				// 0x238  R/W	7th  ECC Code Register
	volatile unsigned int	ECC_CODE7;				// 0x23C  R/W	8th  ECC Code Register
	volatile unsigned int	ECC_CODE8;				// 0x240  R/W	9th  ECC Code Register
	volatile unsigned int	ECC_CODE9;				// 0x244  R/W	10th  ECC Code Register
	volatile unsigned int	ECC_CODE10; 			// 0x248  R/W	11th  ECC Code Register
	volatile unsigned int	ECC_CODE11; 			// 0x24C  R/W	12th  ECC Code Register
	volatile unsigned int	ECC_CODE12; 			// 0x250  R/W	13th  ECC Code Register
	volatile unsigned int	ECC_CODE13; 			// 0x254  R/W	14th  ECC Code Register
	volatile unsigned int	ECC_CODE14; 			// 0x258  R/W	15th  ECC Code Register
	volatile unsigned int	ECC_CODE15; 			// 0x25C  R/W	16th  ECC Code register
	volatile unsigned int	ECC_EADDR0; 			// 0x260  R  	ECC Error Address Register0
	volatile unsigned int	ECC_EADDR1; 			// 0x264  R  	ECC Error Address Register1
	volatile unsigned int	ECC_EADDR2; 			// 0x268  R  	ECC Error Address Register2
	volatile unsigned int	ECC_EADDR3; 			// 0x26C  R  	ECC Error Address Register3
	volatile unsigned int	ECC_EADDR4; 			// 0x270  R  	ECC Error Address Register4
	volatile unsigned int	ECC_EADDR5; 			// 0x274  R  	ECC Error Address Register5
	volatile unsigned int	ECC_EADDR6; 			// 0x278  R  	ECC Error Address Register6
	volatile unsigned int	ECC_EADDR7; 			// 0x27C  R  	ECC Error Address Register7
	volatile unsigned int	ECC_EADDR8; 			// 0x280  R  	ECC Error Address Register8
	volatile unsigned int	ECC_EADDR9; 			// 0x284  R  	ECC Error Address Register9
	volatile unsigned int	ECC_EADDR10;			// 0x288  R  	ECC Error Address Register10
	volatile unsigned int	ECC_EADDR11;			// 0x28C  R  	ECC Error Address Register11
	volatile unsigned int	ECC_EADDR12;			// 0x290  R  	ECC Error Address Register12
	volatile unsigned int	ECC_EADDR13;			// 0x294  R  	ECC Error Address Register13
	volatile unsigned int	ECC_EADDR14;			// 0x298  R  	ECC Error Address Register14
	volatile unsigned int	ECC_EADDR15;			// 0x29C  R  	ECC Error Address Register15
	volatile unsigned int	ECC_EADDR16; 			// 0x2A0  R  	ECC Error Address Register4
	volatile unsigned int	ECC_EADDR17; 			// 0x2A4  R  	ECC Error Address Register5
	volatile unsigned int	ECC_EADDR18; 			// 0x2A8  R  	ECC Error Address Register6
	volatile unsigned int	ECC_EADDR19; 			// 0x2AC  R  	ECC Error Address Register7
	volatile unsigned int	ECC_EADDR20; 			// 0x2B0  R  	ECC Error Address Register8
	volatile unsigned int	ECC_EADDR21; 			// 0x2B4  R  	ECC Error Address Register9
	volatile unsigned int	ECC_EADDR22;			// 0x2B8  R  	ECC Error Address Register10
	volatile unsigned int	ECC_EADDR23;			// 0x2BC  R  	ECC Error Address Register11
#if 0
	volatile unsigned int  	NOTDEFINE0;         	// 0x74~0xFC
	volatile unsigned int   SECC_0;
    volatile unsigned int   SECC_1;					// 0x0C			 Reserved
	volatile unsigned int   SECC_2;
	volatile unsigned int   SECC_3;
	volatile unsigned int  	SECC_4;         		// 0x74~0xFC
	volatile unsigned int   SECC_5;
    volatile unsigned int   SECC_6;					// 0x0C			 Reserved
	volatile unsigned int   SECC_7;
	volatile unsigned int   SECC_8;
	volatile unsigned int  	SECC_9;        			// 0x74~0xFC
	volatile unsigned int   SECC_10;
    volatile unsigned int   SECC_11;				// 0x0C			 Reserved
	volatile unsigned int   SECC_12;
	volatile unsigned int   SECC_13;
	volatile unsigned int  	SECC_14;         		// 0x74~0xFC
	volatile unsigned int  	SECC_15;         		// 0x74~0xFC
	volatile unsigned int  	NOTDEFINE1;         		// 0x74~0xFC
	volatile unsigned int   MECC4_0;
    volatile unsigned int   MECC4_1;				// 0x0C			 Reserved
    volatile unsigned int  	NOTDEFINE2[3];         	// 0x74~0xFC
    volatile unsigned int   MECC6_0;
    volatile unsigned int   MECC6_1;				// 0x0C			 Reserved
    volatile unsigned int  	MECC6_2;         		// 0x74~0xFC
    volatile unsigned int  	NOTDEFINE3[2];          // 0x74~0xFC
    volatile unsigned int   MECC12_0;
    volatile unsigned int   MECC12_1;				// 0x0C			 Reserved
    volatile unsigned int  	MECC12_2;         		// 0x74~0xFC
    volatile unsigned int   MECC12_3;
    volatile unsigned int   MECC12_4;				// 0x0C			 Reserved
    volatile unsigned int  	MECC12_5;         		// 0x74~0xFC
    volatile unsigned int  	NOTDEFINE4[3];         	// 0x74~0xFC
	volatile unsigned int   MECC16_0;
    volatile unsigned int   MECC16_1;				// 0x0C			 Reserved
    volatile unsigned int  	MECC16_2;        		// 0x74~0xFC
    volatile unsigned int   MECC16_3;
    volatile unsigned int   MECC16_4;				// 0x0C			 Reserved
    volatile unsigned int  	MECC16_5;         		// 0x74~0xFC
    volatile unsigned int   MECC16_6;				// 0x0C			 Reserved
    volatile unsigned int  	MECC16_7;         		// 0x74~0xFC
    volatile unsigned int  	NOTDEFINE5;         	// 0x74~0xFC
    volatile unsigned int   MECC24_0;
    volatile unsigned int   MECC24_1;				// 0x0C			 Reserved
    volatile unsigned int  	MECC24_2;         		// 0x74~0xFC
    volatile unsigned int   MECC24_3;
    volatile unsigned int   MECC24_4;				// 0x0C			 Reserved
    volatile unsigned int  	MECC24_5;         		// 0x74~0xFC
    volatile unsigned int   MECC24_6;				// 0x0C			 Reserved
    volatile unsigned int  	MECC24_7;         		// 0x74~0xFC
    volatile unsigned int  	MECC24_8;         		// 0x74~0xFC
    volatile unsigned int   MECC24_9;				// 0x0C			 Reserved
    volatile unsigned int  	MECC24_10;         		// 0x74~0xFC
#endif
} ECC, *PECC;


/*******************************************************************************
*    13. SMC(Static Memory Controller) Register Define (Base Addr = 0xF05F0000)
********************************************************************************/
//#define HwSMC_BASE								((volatile unsigned long*)0xF05F0000)
typedef struct _SMC{
    volatile unsigned int   STATUS;              //  0x00 R/W  Unknown Status Register
    volatile unsigned int   NOTDEFINE0[7];
    volatile unsigned int   CSNCFG0;             //  0x20 R/W  0x4b40_3183 External Chip Select0 Config Register
    volatile unsigned int   CSNCFG1;             //  0x24 R/W  0x4b40_1104 External Chip Select1 Config Register
    volatile unsigned int   CSNCFG2;             //  0x28 R/W  0x4b40_4082 External Chip Select2 Config Register
    volatile unsigned int   CSNCFG3;             //  0x2C R/W  0x4b40_20C5 External Chip Select3 Config. Register
} SMC, *PSMC;

/*******************************************************************************
*    14. EDI(External Device Interface) Register Define (Base Addr = 0xF05F6000)
********************************************************************************/
//#define HwEDI_BASE								((volatile unsigned long*)0xF05F6000)
typedef struct _EDI{
    volatile unsigned int   EDI_CTRL;           // 0x00  R/W   0x00000000   EDI Control Register.
    volatile unsigned int   EDI_CSNCFG0;        // 0x04  R/W   0x00543210   EDI CSN Configuration Register 0.
    volatile unsigned int   EDI_CSNCFG1;        // 0x08  R/W   0x00BA9876   EDI CSN Configuration Register 1.
    volatile unsigned int   EDI_WENCFG;         // 0x0C  R/W   0x00000004   EDI WEN Configuration Register
    volatile unsigned int   EDI_OENCFG;         // 0x10  R/W   0x00000004   EDI OEN Configuration Register
    volatile unsigned int   EDI_RDYCFG;         // 0x14  R/W   0x00043210   EDI Ready Configuration Register
    volatile unsigned int   _Reserved0;         // 0x18   -  -  -
    volatile unsigned int   _Reserved1;         // 0x1C   -  -
    volatile unsigned int   EDI_REQOFF;         // 0x20  R/W   0x00000000   EDI Request OFF Flag Register
} EDI, *PEDI;

/*******************************************************************************
*    15. IDE Register Define (Base Addr = 0xF0520000)
********************************************************************************/
//#define HwIDE_BASE								((volatile unsigned long*)0xF0520000)
typedef struct _IDE{
	volatile unsigned int	CS00;                           //  0x00 R/W - PIO CS0n Access Register
	volatile unsigned int	CS01;                           //  0x04
	volatile unsigned int	CS02;                           //  0x08
	volatile unsigned int	CS03;                           //  0x0C
	volatile unsigned int	CS04;                           //  0x10
	volatile unsigned int	CS05;                           //  0x14
	volatile unsigned int	CS06;                           //  0x18
	volatile unsigned int	CS07;                           //  0x1C
	volatile unsigned int	CS10;                           //  0x20 R/W - PIO CS1n Access Register
	volatile unsigned int	CS11;                           //  0x24
	volatile unsigned int	CS12;                           //  0x28
	volatile unsigned int	CS13;                           //  0x2C
	volatile unsigned int	CS14;                           //  0x30
	volatile unsigned int	CS15;                           //  0x34
	volatile unsigned int	CS16;                           //  0x38
	volatile unsigned int	CS17;                           //  0x3C
	volatile unsigned int	PIOCTRL;                        //  0x40 R/W 0x00600000 PIO Mode Control Register
	volatile unsigned int	UDMACTRL;                       //  0x44 R/W 0x00000000 UDMA Mode Control Register
	volatile unsigned int	IDMACTRL;                       //  0x48 R/W 0x00000000 IDMA Control Register
	volatile unsigned int	IDMASA;                         //  0x4C R/W 0x00000000 IDMA Source Address Register
	volatile unsigned int	IDMASP;                         //  0x50 R/W 0x00000000 IDMA Source Parameter Register
	volatile unsigned int	IDMACSA;                        //  0x54 R 0x00000000 IDMA Current Source Address Register
	volatile unsigned int	IDMADA;                         //  0x58 R/W 0x00000000 IDMA Destination Address Register
	volatile unsigned int	IDMADP;                         //  0x5C R/W 0x00000000 IDMA Destination Parameter Register
	volatile unsigned int	IDMACDA;                        //  0x60 R 0x00000000 IDMA Current Destination Address Register
	volatile unsigned int	IDEINT;                         //  0x64 R/W 0x0000_0000 IDE Interrupt Register
	volatile unsigned int	UDMATCNT;                       //  0x68 R/W 0x00FF_FFFF UDMA Transfer Counter Register
	volatile unsigned int	UDMAIN;                         //  0x6C R - UDMA-IN Access Register
	volatile unsigned int	UDMAOUT;                        //  0x70 W - UDMA-OUT Access register
	volatile unsigned int	UDMACRC;                        //  0x74 R 0x0000_4ABA UDMA CRC Register
	volatile unsigned int	UDMACTCNT;                      //  0x78 R 0x00FF_FFFF UDMA Current Transfer Counter Register
} IDE, *PIDE;


/*******************************************************************************
*    16. USB 1.1 Host Controller & Transceiver Register Define (Base Addr = 0xF0500000)
********************************************************************************/
//#define HwUSBHOST_BASE							((volatile unsigned long*)0xF0500000)
typedef struct _USBHOST11{
	volatile unsigned int	HcRevision;                         // 0x00 R 0x00000010
	volatile unsigned int	HcControl;                          // 0x04 R/W 0x00000000
	volatile unsigned int	HcCommandStatus;                    // 0x08 R 0x00000000
	volatile unsigned int	HcInterruptStatus;                  // 0x0C R 0x00000000
	volatile unsigned int	HcInterruptEnable;                  // 0x10 R/W 0x00000000
	volatile unsigned int	HcInterruptDisable;                 // 0x14 W 0x00000000
	volatile unsigned int	HcHCCA;                             // 0x18 R/W 0x00000000
	volatile unsigned int	HcPeriodCurrentED;                  // 0x1C R 0x00000000
	volatile unsigned int	HcControlHeadED;                    // 0x20 R/W 0x00000000
	volatile unsigned int	HcControlCurrentED;                 // 0x24 R/W 0x00000000
	volatile unsigned int	HcBulkHeadED;                       // 0x28 R/W 0x00000000
	volatile unsigned int	HcBulkCurrentED;                    // 0x2C R/W 0x00000000
	volatile unsigned int	HcDoneHead;                         // 0x30 R 0x00000000
	volatile unsigned int	HcRmInterval;                       // 0x34 R/W 0x00002EDF
	volatile unsigned int	HcFmRemaining;                      // 0x38 R/W 0x00000000
	volatile unsigned int	HcFmNumber;                         // 0x3C R/W 0x00000000
	volatile unsigned int	HcPeriodStart;                      // 0x40 R/W 0x00000000
	volatile unsigned int	HcLSThreshold;                      // 0x44 R/W 0x00000628
	volatile unsigned int	HcRhDescriptorA;                    // 0x48 R/W 0x02001202
	volatile unsigned int	HcRhDescriptorB;                    // 0x4C R/W 0x00000000
	volatile unsigned int	HcRhStatus;                         // 0x50 R/W 0x00000000
	volatile unsigned int	HcRhPortStatus1;                    // 0x54 R/W 0x00000100
	volatile unsigned int	HcRhPortStatus2;                    // 0x58 R/W 0x00000100
} USBHOST11, *PUSBHOST11;


typedef struct _USBOTGCFG{
    volatile unsigned int   OTGCR;                  // 0x000  R/W   USBOTG Configuration Register
	volatile unsigned int	OTGID;	    
    volatile unsigned int   UPCR0;                  // 0x028  R/W   USB PHY Configuration Register0
    volatile unsigned int   UPCR1;                  // 0x02C  R/W   USB PHY Configuration Register1
    volatile unsigned int   UPCR2;                  // 0x030  R/W   USB PHY Configuration Register2
    volatile unsigned int   UPCR3;                  // 0x034  R/W   USB PHY Configuration Register3
}USBOTGCFG, *PUSBOTGCFG;


/*******************************************************************************
*    17. USB 2.0 OTG Controller Register Define (Base Addr = 0xF0550000)
********************************************************************************/
//#define HwUSB20OTG_BASE							((volatile unsigned long*)0xF0550000)
typedef struct _USB20OTG{
    // Core Global CSR Map
    volatile unsigned int   GOTGCTL;                // 0x000  R/W   OTG Control and Status Register
    volatile unsigned int   GOTGINT;                // 0x004        OTG Interrupt Register
    volatile unsigned int   GAHBCFG;                // 0x008        Core AHB Configuration Register
    volatile unsigned int   GUSBCFG;                // 0x00C        Core USB Configuration register
    volatile unsigned int   GRSTCTL;                // 0x010        Core Reset Register
    volatile unsigned int   GINTSTS;                // 0x014        Core Interrupt Register
    volatile unsigned int   GINTMSK;                // 0x018        Core Interrupt Mask Register
    volatile unsigned int   GRXSTSR;                // 0x01C        Receive Status Debug Read register(Read Only)
    volatile unsigned int   GRXSTSP;                // 0x020        Receive Status Read /Pop register(Read Only)
    volatile unsigned int   GRXFSIZ;                // 0x024        Receive FIFO Size Register
    volatile unsigned int   GNPTXFSIZ;              // 0x028        Non-periodic Transmit FIFO Size register
    volatile unsigned int   GNPTXSTS;               // 0x02C        Non-periodic Transmit FIFO/Queue Status register (Read Only)
    volatile unsigned int   NOTDEFINE0[3];          // 0x030~       Reserved
    volatile unsigned int   GUID;                   // 0x03C        User ID Register
    volatile unsigned int   NOTDEFINE1;             // 0x040        Reserved
    volatile unsigned int   GHWCFG1;                // 0x044        User HW Config1 Register(Read Only)
    volatile unsigned int   GHWCFG2;                // 0x048        User HW Config2 Register(Read Only)
    volatile unsigned int   GHWCFG3;                // 0x04C        User HW Config3 Register(Read Only)
    volatile unsigned int   GHWCFG4;                // 0x050        User HW Config4 Register(Read Only)
    volatile unsigned int   NOTDEFINE2[43];         // 0x054~       Reserved
    volatile unsigned int   HPTXFSIZ;               // 0x100        Host Periodic Transmit FIFO Size Register
    volatile unsigned int   DIEPTXFn[15];           // 0x104~       Device IN Endpoint Transmit FIFO Size register
    volatile unsigned int   NOTDEFINE3[176];        // 0x140~       Reserved
    // Host Mode CSR Map
    volatile unsigned int   HCFG;                   // 0x400        Host Configuration Register
    volatile unsigned int   HFIR;                   // 0x404        Host Frame Interval Register
    volatile unsigned int   HFNUM;                  // 0x408        Host Frame Number/Frame Time Remaining register
    volatile unsigned int   NOTDEFINE4;             // 0x40C        Reserved
    volatile unsigned int   HPTXSTS;                // 0x410        Host Periodic Transmit FIFO/Queue Status Register
    volatile unsigned int   HAINT;                  // 0x414        Host All Channels Interrupt Register
    volatile unsigned int   HAINTMSK;               // 0x418        Host All Channels Interrupt Mask register
    volatile unsigned int   NOTDEFINE5[9];          // 0x41C~       Not Used
    volatile unsigned int   HPRT;                   // 0x440        Host Port Control and Status register
    volatile unsigned int   NOTDEFINE6[47];         // 0x444~       Reserved
    volatile unsigned int   HCCHARn;                // 0x500        Host Channel 0 Characteristics Register
    volatile unsigned int   HCSPLTn;                // 0x504        Host Channel 0 Split Control Register
    volatile unsigned int   HCINTn;                 // 0x508        Host Channel 0 Interrupt Register
    volatile unsigned int   HCINTMSKn;              // 0x50C        Host Channel 0 Interrupt Mask Register
    volatile unsigned int   HCTSIZn;                // 0x510        Host Channel 0 Transfer Size Register
    volatile unsigned int   HCDMAn;                 // 0x514        Host Channel 0 DMA Address Register
    volatile unsigned int   NOTDEFINE7[2];          // 0x518~       Reserved
    volatile unsigned int   HCH1[8];                // 0x520~       Host Channel 1 Registers
    volatile unsigned int   HCH2[8];                // 0x540~       Host Channel 2 Registers
    volatile unsigned int   HCH3[8];                // 0x560~       Host Channel 3 Registers
    volatile unsigned int   HCH4[8];                // 0x580~       Host Channel 4 Registers
    volatile unsigned int   HCH5[8];                // 0x5A0~       Host Channel 5 Registers
    volatile unsigned int   HCH6[8];                // 0x5C0~       Host Channel 6 Registers
    volatile unsigned int   HCH7[8];                // 0x5E0~       Host Channel 7 Registers
    volatile unsigned int   HCH8[8];                // 0x600~       Host Channel 8 Registers
    volatile unsigned int   HCH9[8];                // 0x620~       Host Channel 9 Registers
    volatile unsigned int   HCH10[8];               // 0x640~       Host Channel 10 Registers
    volatile unsigned int   HCH11[8];               // 0x660~       Host Channel 11 Registers
    volatile unsigned int   HCH12[8];               // 0x680~       Host Channel 12 Registers
    volatile unsigned int   HCH13[8];               // 0x6A0~       Host Channel 13 Registers
    volatile unsigned int   HCH14[8];               // 0x6C0~       Host Channel 14 Registers
    volatile unsigned int   HCH15[8];               // 0x6E0~       Host Channel 15 Registers
    volatile unsigned int   NOTDEFINE8[64];         // 0x700~       Reserved
    // Device Mode CSR Map
    volatile unsigned int   DCFG;                   // 0x800        Device Configuration Register
    volatile unsigned int   DCTL;                   // 0x804        Device Control Register
    volatile unsigned int   DSTS;                   // 0x808        Device Status Register (Read Only)
    volatile unsigned int   NOTDEFINE9;             // 0x80C        Reserved
    volatile unsigned int   DIEPMSK;                // 0x810        Device IN Endpoint Common Interrupt Mask Register
    volatile unsigned int   DOEPMSK;                // 0x814        Device OUT Endpoint Common Interrupt Mask register
    volatile unsigned int   DAINT;                  // 0x818        Device All Endpoints Interrupt Register
    volatile unsigned int   DAINTMSK;               // 0x81C        Device All Endpoints Interrupt Mask Register
    volatile unsigned int   NOTDEFINE10[2];         // 0x820~       Reserved
    volatile unsigned int   DVBUSDIS;               // 0x828        Device VBUS Discharge Time Register
    volatile unsigned int   DVBUSPULSE;             // 0x82C        Device VBUS Pulsing Time register
    volatile unsigned int   DTHRCTL;                // 0x830        Device Threshold Control register
    volatile unsigned int   DIEPEMPMSK;             // 0x834        Device IN Endpoint FIFO Empty Interrupt Mask register
    volatile unsigned int   NOTDEFINE11[50];        // 0x838~       Reserved

    volatile unsigned int   DIEPCTL0;               // 0x900        Device Control IN Endpoint 0 Control Register
    volatile unsigned int   NOTDEFINE12;            // 0x904        Reserved
    volatile unsigned int   DIEPINT0;               // 0x908        Device IN Endpoint 0 Interrupt Register
    volatile unsigned int   NOTDEFINE13;            // 0x90C        Reserved
    volatile unsigned int   DIEPTSIZ0;              // 0x910        Device IN Endpoint 0 Transfer Size register
    volatile unsigned int   DIEPDMA0;               // 0x914        Device IN Endpoint 0 DMA Address Register
    volatile unsigned int   DTXFSTS0;               // 0x918        Device IN Endpoint Transmit FIFO Status Register
    volatile unsigned int   NOTDEFINE14;            // 0x91C        Reserved

    volatile unsigned int   DEVINENDPT[15][8];      // 0x920~       Device IN Endpoint 1~15 Registers
    volatile unsigned int   DOEPCTL0;               // 0xB00        Device Control OUT Endpoint 0 Control register
    volatile unsigned int   NOTDEFINE15;            // 0xB04        Reserved
    volatile unsigned int   DOEPINT0;               // 0xB08        Device OUT Endpoint 0 Interrupt Register
    volatile unsigned int   NOTDEFINE16;            // 0xB0C        Reserved
    volatile unsigned int   DOEPTSIZ0;              // 0xB10        Device OUT Endpoint 0 Transfer Size Register
    volatile unsigned int   DOEPDMA0;               // 0xB14        Device OUT Endpoint 0 DMA Address register
    volatile unsigned int   NOTDEFINE17[2];         // 0xB18~       Reserved
    volatile unsigned int   DEVOUTENDPT[15][8];     // 0xB20~       Device OUT Endpoint 1~15 Registers
    volatile unsigned int   NOTDEFINE18[64];        // 0xD00~       Reserved
    // Power and Clock Gating CSR Map
    volatile unsigned int   PCGCR;                  // 0xE00        Power and Clock Gating Control Register
    volatile unsigned int   NOTDEFINE19[127];       // 0xE04~       Reserved
    // Data FIFO(DFIFO) Access Register Map
    volatile unsigned int   DFIFOENDPT[16][1024];   // 0x1000~      Device IN Endpoint 0~16/Host Out Channel 0~16: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT0[1024];      // 0x1000~      Device IN Endpoint 0/Host Out Channel 0: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT1[1024];      // 0x2000~      Device IN Endpoint 1/Host Out Channel 1: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT2[1024];      // 0x3000~      Device IN Endpoint 2/Host Out Channel 2: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT3[1024];      // 0x4000~      Device IN Endpoint 3/Host Out Channel 3: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT4[1024];      // 0x5000~      Device IN Endpoint 4/Host Out Channel 4: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT5[1024];      // 0x6000~      Device IN Endpoint 5/Host Out Channel 5: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT6[1024];      // 0x7000~      Device IN Endpoint 6/Host Out Channel 6: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT7[1024];      // 0x8000~      Device IN Endpoint 7/Host Out Channel 7: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT8[1024];      // 0x9000~      Device IN Endpoint 8/Host Out Channel 8: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT9[1024];      // 0xA000~      Device IN Endpoint 9/Host Out Channel 9: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT10[1024];     // 0xB000~      Device IN Endpoint 10/Host Out Channel 10: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT11[1024];     // 0xC000~      Device IN Endpoint 11/Host Out Channel 11: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT12[1024];     // 0xD000~      Device IN Endpoint 12/Host Out Channel 12: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT13[1024];     // 0xE000~      Device IN Endpoint 13/Host Out Channel 13: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT14[1024];     // 0xF000~      Device IN Endpoint 14/Host Out Channel 14: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT15[1024];     // 0x10000~     Device IN Endpoint 15/Host Out Channel 15: DFIFO Write/Read Access
} USB20OTG, *PUSB20OTG;

typedef struct _USBOTG{
    // Core Global CSR Map
    volatile unsigned int   GOTGCTL;                // 0x000  R/W   OTG Control and Status Register
    volatile unsigned int   GOTGINT;                // 0x004        OTG Interrupt Register
    volatile unsigned int   GAHBCFG;                // 0x008        Core AHB Configuration Register
    volatile unsigned int   GUSBCFG;                // 0x00C        Core USB Configuration register
    volatile unsigned int   GRSTCTL;                // 0x010        Core Reset Register
    volatile unsigned int   GINTSTS;                // 0x014        Core Interrupt Register
    volatile unsigned int   GINTMSK;                // 0x018        Core Interrupt Mask Register
    volatile unsigned int   GRXSTSR;                // 0x01C        Receive Status Debug Read register(Read Only)
    volatile unsigned int   GRXSTSP;                // 0x020        Receive Status Read /Pop register(Read Only)
    volatile unsigned int   GRXFSIZ;                // 0x024        Receive FIFO Size Register
    volatile unsigned int   GNPTXFSIZ;              // 0x028        Non-periodic Transmit FIFO Size register
    volatile unsigned int   GNPTXSTS;               // 0x02C        Non-periodic Transmit FIFO/Queue Status register (Read Only)
    volatile unsigned int   NOTDEFINE0[3];          // 0x030~       Reserved
    volatile unsigned int   GUID;                   // 0x03C        User ID Register
    volatile unsigned int   NOTDEFINE1;             // 0x040        Reserved
    volatile unsigned int   GHWCFG1;                // 0x044        User HW Config1 Register(Read Only)
    volatile unsigned int   GHWCFG2;                // 0x048        User HW Config2 Register(Read Only)
    volatile unsigned int   GHWCFG3;                // 0x04C        User HW Config3 Register(Read Only)
    volatile unsigned int   GHWCFG4;                // 0x050        User HW Config4 Register(Read Only)
    volatile unsigned int   NOTDEFINE2[43];         // 0x054~       Reserved
    volatile unsigned int   HPTXFSIZ;               // 0x100        Host Periodic Transmit FIFO Size Register
    volatile unsigned int   DIEPTXFn[15];           // 0x104~       Device IN Endpoint Transmit FIFO Size register
    volatile unsigned int   NOTDEFINE3[176];        // 0x140~       Reserved
    // Host Mode CSR Map
    volatile unsigned int   HCFG;                   // 0x400        Host Configuration Register
    volatile unsigned int   HFIR;                   // 0x404        Host Frame Interval Register
    volatile unsigned int   HFNUM;                  // 0x408        Host Frame Number/Frame Time Remaining register
    volatile unsigned int   NOTDEFINE4;             // 0x40C        Reserved
    volatile unsigned int   HPTXSTS;                // 0x410        Host Periodic Transmit FIFO/Queue Status Register
    volatile unsigned int   HAINT;                  // 0x414        Host All Channels Interrupt Register
    volatile unsigned int   HAINTMSK;               // 0x418        Host All Channels Interrupt Mask register
    volatile unsigned int   NOTDEFINE5[9];          // 0x41C~       Not Used
    volatile unsigned int   HPRT;                   // 0x440        Host Port Control and Status register
    volatile unsigned int   NOTDEFINE6[47];         // 0x444~       Reserved
    volatile unsigned int   HCCHARn;                // 0x500        Host Channel 0 Characteristics Register
    volatile unsigned int   HCSPLTn;                // 0x504        Host Channel 0 Split Control Register
    volatile unsigned int   HCINTn;                 // 0x508        Host Channel 0 Interrupt Register
    volatile unsigned int   HCINTMSKn;              // 0x50C        Host Channel 0 Interrupt Mask Register
    volatile unsigned int   HCTSIZn;                // 0x510        Host Channel 0 Transfer Size Register
    volatile unsigned int   HCDMAn;                 // 0x514        Host Channel 0 DMA Address Register
    volatile unsigned int   NOTDEFINE7[2];          // 0x518~       Reserved
    volatile unsigned int   HCH1[8];                // 0x520~       Host Channel 1 Registers
    volatile unsigned int   HCH2[8];                // 0x540~       Host Channel 2 Registers
    volatile unsigned int   HCH3[8];                // 0x560~       Host Channel 3 Registers
    volatile unsigned int   HCH4[8];                // 0x580~       Host Channel 4 Registers
    volatile unsigned int   HCH5[8];                // 0x5A0~       Host Channel 5 Registers
    volatile unsigned int   HCH6[8];                // 0x5C0~       Host Channel 6 Registers
    volatile unsigned int   HCH7[8];                // 0x5E0~       Host Channel 7 Registers
    volatile unsigned int   HCH8[8];                // 0x600~       Host Channel 8 Registers
    volatile unsigned int   HCH9[8];                // 0x620~       Host Channel 9 Registers
    volatile unsigned int   HCH10[8];               // 0x640~       Host Channel 10 Registers
    volatile unsigned int   HCH11[8];               // 0x660~       Host Channel 11 Registers
    volatile unsigned int   HCH12[8];               // 0x680~       Host Channel 12 Registers
    volatile unsigned int   HCH13[8];               // 0x6A0~       Host Channel 13 Registers
    volatile unsigned int   HCH14[8];               // 0x6C0~       Host Channel 14 Registers
    volatile unsigned int   HCH15[8];               // 0x6E0~       Host Channel 15 Registers
    volatile unsigned int   NOTDEFINE8[64];         // 0x700~       Reserved
    // Device Mode CSR Map
    volatile unsigned int   DCFG;                   // 0x800        Device Configuration Register
    volatile unsigned int   DCTL;                   // 0x804        Device Control Register
    volatile unsigned int   DSTS;                   // 0x808        Device Status Register (Read Only)
    volatile unsigned int   NOTDEFINE9;             // 0x80C        Reserved
    volatile unsigned int   DIEPMSK;                // 0x810        Device IN Endpoint Common Interrupt Mask Register
    volatile unsigned int   DOEPMSK;                // 0x814        Device OUT Endpoint Common Interrupt Mask register
    volatile unsigned int   DAINT;                  // 0x818        Device All Endpoints Interrupt Register
    volatile unsigned int   DAINTMSK;               // 0x81C        Device All Endpoints Interrupt Mask Register
    volatile unsigned int   NOTDEFINE10[2];         // 0x820~       Reserved
    volatile unsigned int   DVBUSDIS;               // 0x828        Device VBUS Discharge Time Register
    volatile unsigned int   DVBUSPULSE;             // 0x82C        Device VBUS Pulsing Time register
    volatile unsigned int   DTHRCTL;                // 0x830        Device Threshold Control register
    volatile unsigned int   DIEPEMPMSK;             // 0x834        Device IN Endpoint FIFO Empty Interrupt Mask register
    volatile unsigned int   NOTDEFINE11[50];        // 0x838~       Reserved

    //volatile unsigned int DIEPCTL0;               // 0x900        Device Control IN Endpoint 0 Control Register
    //volatile unsigned int NOTDEFINE12;            // 0x904        Reserved
    //volatile unsigned int DIEPINT0;               // 0x908        Device IN Endpoint 0 Interrupt Register
    //volatile unsigned int NOTDEFINE13;            // 0x90C        Reserved
    //volatile unsigned int DIEPTSIZ0;              // 0x910        Device IN Endpoint 0 Transfer Size register
    //volatile unsigned int DIEPDMA0;               // 0x914        Device IN Endpoint 0 DMA Address Register
    //volatile unsigned int DTXFSTS0;               // 0x918        Device IN Endpoint Transmit FIFO Status Register
    //volatile unsigned int NOTDEFINE14;            // 0x91C        Reserved

    volatile unsigned int   DEVINENDPT[16][8];      // 0x900~       Device IN Endpoint 1~15 Registers

    //volatile unsigned int DOEPCTL0;               // 0xB00        Device Control OUT Endpoint 0 Control register
    //volatile unsigned int NOTDEFINE15;            // 0xB04        Reserved
    //volatile unsigned int DOEPINT0;               // 0xB08        Device OUT Endpoint 0 Interrupt Register
    //volatile unsigned int NOTDEFINE16;            // 0xB0C        Reserved
    //volatile unsigned int DOEPTSIZ0;              // 0xB10        Device OUT Endpoint 0 Transfer Size Register
    //volatile unsigned int DOEPDMA0;               // 0xB14        Device OUT Endpoint 0 DMA Address register
    //volatile unsigned int NOTDEFINE17[2];         // 0xB18~       Reserved

    volatile unsigned int   DEVOUTENDPT[16][8];     // 0xB00~       Device OUT Endpoint 1~15 Registers
    volatile unsigned int   NOTDEFINE18[64];        // 0xD00~       Reserved

    // Power and Clock Gating CSR Map
    volatile unsigned int   PCGCR;                  // 0xE00        Power and Clock Gating Control Register
    volatile unsigned int   NOTDEFINE19[127];       // 0xE04~       Reserved
    // Data FIFO(DFIFO) Access Register Map
    volatile unsigned int   DFIFOENDPT[16][1024];   // 0x1000~      Device IN Endpoint 0~16/Host Out Channel 0~16: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT0[1024];      // 0x1000~      Device IN Endpoint 0/Host Out Channel 0: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT1[1024];      // 0x2000~      Device IN Endpoint 1/Host Out Channel 1: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT2[1024];      // 0x3000~      Device IN Endpoint 2/Host Out Channel 2: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT3[1024];      // 0x4000~      Device IN Endpoint 3/Host Out Channel 3: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT4[1024];      // 0x5000~      Device IN Endpoint 4/Host Out Channel 4: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT5[1024];      // 0x6000~      Device IN Endpoint 5/Host Out Channel 5: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT6[1024];      // 0x7000~      Device IN Endpoint 6/Host Out Channel 6: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT7[1024];      // 0x8000~      Device IN Endpoint 7/Host Out Channel 7: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT8[1024];      // 0x9000~      Device IN Endpoint 8/Host Out Channel 8: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT9[1024];      // 0xA000~      Device IN Endpoint 9/Host Out Channel 9: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT10[1024];     // 0xB000~      Device IN Endpoint 10/Host Out Channel 10: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT11[1024];     // 0xC000~      Device IN Endpoint 11/Host Out Channel 11: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT12[1024];     // 0xD000~      Device IN Endpoint 12/Host Out Channel 12: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT13[1024];     // 0xE000~      Device IN Endpoint 13/Host Out Channel 13: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT14[1024];     // 0xF000~      Device IN Endpoint 14/Host Out Channel 14: DFIFO Write/Read Access
    //volatile unsigned int DFIFOENDPT15[1024];     // 0x10000~     Device IN Endpoint 15/Host Out Channel 15: DFIFO Write/Read Access
} USBOTG, *PUSBOTG;

typedef struct _USBPHYCFG
{
	volatile unsigned int	UPCR0;					// 0x028  R/W	USB PHY Configuration Register0
	volatile unsigned int	UPCR1;					// 0x02C  R/W	USB PHY Configuration Register1
	volatile unsigned int	UPCR2;					// 0x030  R/W	USB PHY Configuration Register2
	volatile unsigned int	UPCR3;					// 0x034  R/W	USB PHY Configuration Register3
}USBPHYCFG, *PUSBPHYCFG;

/*******************************************************************************
*    18. I2C Controller Register Define (Base Addr = 0xF0550000)
********************************************************************************/
//#define HwI2C_CORE0_BASE						((volatile unsigned long*)0xF0530000)
//#define HwI2C_CORE0_MASTER0_BASE				((volatile unsigned long*)0xF0530000)
//#define HwI2C_CORE0_MASTER1_BASE				((volatile unsigned long*)0xF0530040)
//#define HwI2C_CORE0_SLAVE_BASE					((volatile unsigned long*)0xF0530080)
//#define HwI2C_CORE0_INT_BASE					((volatile unsigned long*)0xF05300C0)

//#define HwI2C_CORE1_BASE						((volatile unsigned long*)0xF0530100)
//#define HwI2C_CORE1_MASTER0_BASE				((volatile unsigned long*)0xF0530100)
//#define HwI2C_CORE1_MASTER1_BASE				((volatile unsigned long*)0xF0530140)
//#define HwI2C_CORE1_SLAVE_BASE					((volatile unsigned long*)0xF0530180)
//#define HwI2C_CORE1_INT_BASE					((volatile unsigned long*)0xF05301C0)

//#define HwI2CIRQ_BASE                           ((volatile unsigned long*)0xF0530300)
//#define HwI2CDREQSEL_BASE                       ((volatile unsigned long*)0xF0530400)

typedef struct _I2CMASTER{
    volatile unsigned int  PRES;                // 0x00 R/W 0xFFFF Clock Prescale register
    volatile unsigned int  CTRL;                // 0x04 R/W 0x0000 Control Register
    volatile unsigned int  TXR;                 // 0x08 W 0x0000 Transmit Register
    volatile unsigned int  CMD;                 // 0x0C W 0x0000 Command Register
    volatile unsigned int  RXR;                 // 0x10 R 0x0000 Receive Register
    volatile unsigned int  SR;                  // 0x14 R 0x0000 Status register
    volatile unsigned int  TIME;                // 0x18 R/W 0x0000 Timing Control Register
}I2CMASTER, *PI2CMASTER;

typedef struct _I2CSLAVE{
    volatile unsigned int  PORT;                // 0x80 R/W - Data Access port (TX/RX FIFO)
    volatile unsigned int  CTL;                 // 0x84 R/W 0x00000000 Control register
    volatile unsigned int  ADDR;                // 0x88 W 0x00000000 Address register
    volatile unsigned int  INT;                 // 0x8C W 0x00000000 Interrupt Enable Register
    volatile unsigned int  STAT;                // 0x90 R 0x00000000 Status Register
    volatile unsigned int  NOTDEFINE0[2];
    volatile unsigned int  MBF;                 // 0x9C R/W 0x00000000 Buffer Valid Flag
    volatile unsigned int  MB0;                 // 0xA0 R/W 0x00000000 Data Buffer 0 (Byte 3 ~ 0)
    volatile unsigned int  MB1;                 // 0xA4 R/W 0x00000000 Data Buffer 1 (Byte 7 ~ 4)
}I2CSLAVE, *PI2CSLAVE;

typedef struct _I2C{
    volatile unsigned int  PRES0;                // 0x00 R/W 0xFFFF Clock Prescale register
    volatile unsigned int  CTRL0;                // 0x04 R/W 0x0000 Control Register
    volatile unsigned int  TXR0;                 // 0x08 W 0x0000 Transmit Register
    volatile unsigned int  CMD0;                 // 0x0C W 0x0000 Command Register
    volatile unsigned int  RXR0;                 // 0x10 R 0x0000 Receive Register
    volatile unsigned int  SR0;                  // 0x14 R 0x0000 Status register
    volatile unsigned int  TIME0;                // 0x18 R/W 0x0000 Timing Control Register
    volatile unsigned int  NOTUSING[9];          // 1c, 20, 24,28,2c,30,34,38,3c,
    volatile unsigned int  PRES1;                // 0x40 R/W 0xFFFF Clock Prescale register
    volatile unsigned int  CTRL1;                // 0x44 R/W 0x0000 Control Register
    volatile unsigned int  TXR1;                 // 0x48 W 0x0000 Transmit Register
    volatile unsigned int  CMD1;                 // 0x4C W 0x0000 Command Register
    volatile unsigned int  RXR1;                 // 0x50 R 0x0000 Receive Register
    volatile unsigned int  SR1;                  // 0x54 R 0x0000 Status register
    volatile unsigned int  TIME1;                // 0x58 R/W 0x0000 Timing Control Register
    volatile unsigned int  NOTUSING1[26];
    volatile unsigned int  IRQSTR;              // 0xC0 R 0x00000000 IRQ Status Register
}I2C, *PI2C;

typedef struct _SMUI2C{
	volatile unsigned int  PRES0;                // 0x00 R/W 0xFFFF Clock Prescale register
	volatile unsigned int  CTRL0;                // 0x04 R/W 0x0000 Control Register
	volatile unsigned int  TXR0;                 // 0x08 W 0x0000 Transmit Register
	volatile unsigned int  CMD0;                 // 0x0C W 0x0000 Command Register
	volatile unsigned int  RXR0;                 // 0x10 R 0x0000 Receive Register
	volatile unsigned int  SR0;                  // 0x14 R 0x0000 Status register
	volatile unsigned int  TIME0;                // 0x18 R/W 0x0000 Timing Control Register
	volatile unsigned int  NOTUSING[9];			 // 1c, 20, 24,28,2c,30,34,38,3c,
	volatile unsigned int  PRES1;                // 0x40 R/W 0xFFFF Clock Prescale register
	volatile unsigned int  CTRL1;                // 0x44 R/W 0x0000 Control Register
	volatile unsigned int  TXR1;                 // 0x48 W 0x0000 Transmit Register
	volatile unsigned int  CMD1;                 // 0x4C W 0x0000 Command Register
	volatile unsigned int  RXR1;                 // 0x50 R 0x0000 Receive Register
	volatile unsigned int  SR1;                  // 0x54 R 0x0000 Status register
	volatile unsigned int  TIME1;                // 0x58 R/W 0x0000 Timing Control Register
	volatile unsigned int  NOTUSING1[10];
	volatile unsigned int  IRQSTR;              // 0x80 R 0x00000000 IRQ Status Register
}SMUI2C, *PSMUI2C;

typedef struct _I2CSTATUS{
    volatile unsigned int  IRQSTR;              // 0xC0 R 0x00000000 IRQ Status Register
}I2CSTATUS, *PI2CSTATUS;

typedef struct _I2CGROUPIRQ{
    volatile unsigned int  I2C_IRQ;             // 0xB0019300 R 3'b000 IRQ Status of I2C Channel2 Group
}I2CGROUPIRQ, *PI2CGROUPIRQ;

typedef struct _I2CGROUPDMA{
    volatile unsigned int  DREQ_SEL;            // 0xB0019400 R/W 1'b0 DMA Request Selection of I2C Group
}I2CGROUPDMA, *PI2CGROUPDMA;

/*******************************************************************************
*    19. PWM(Pulse Width Modulation) Register Define (Base Addr = 0xF05D0000)
********************************************************************************/
//#define HwPWM_BASE								((volatile unsigned long*)0xF05D0000)
//#define HwPDM_BASE								((volatile unsigned long*)0xF05D0000)
typedef struct _PDMCONFIG{
    volatile unsigned int  PdmEn;               //  0x0004  R/W 0x000f  PDM block enable & PDM Trigger
    volatile unsigned int  PdmMode;             //  0x0008  R/W 0x000f  PDM output mode & PDMCLKDiv
    volatile unsigned int  PdmAPstn12;          //  0x0010 R/W 0x2010  PDM block A position 1 & 2
    volatile unsigned int  PdmAPstn34;          //  0x0014  R/W 0x4030  PDM block A position 3 & 4
    volatile unsigned int  PdmBPstn12;          //  0x0018  R/W 0x2010  PDM block B position 1 & 2
    volatile unsigned int  PdmBPstn34;          //  0x001C  R/W 0x4030  PDM block B position 3 & 4
    volatile unsigned int  PdmCPstn12;          //  0x0020  R/W 0x2010  PDM block C position 1 & 2
    volatile unsigned int  PdmCPstn34;          //  0x0024  R/W 0x4030  PDM block C position 3 & 4
    volatile unsigned int  PdmDPstn12;          //  0x0028  R/W 0x2010  PDM block D position 1 & 2
    volatile unsigned int  PdmDPstn34;          //  0x002C  R/W 0x4030  PDM block D position 3 & 4
    volatile unsigned int  PdmOutRegA1;         //  0x0030  R/W 0xAAAAAAAA  PDM A1 block Reg mode Output value
    volatile unsigned int  PdmOutRegA2;         //  0x0034  R/W 0x55555555  PDM A2 block Reg mode Output value
    volatile unsigned int  PdmOutRegA3;         //  0x0038  R/W 0xAAAAAAAA  PDM A3 block Reg mode Output value
    volatile unsigned int  PdmOutRegA4;         //  0x003C  R/W 0x55555555  PDM A4 block Reg mode Output value
    volatile unsigned int  PdmOutRegB1;         //  0x0040  R/W 0xAAAAAAAA  PDM B1 block Reg mode Output value
    volatile unsigned int  PdmOutRegB2;         //  0x0044  R/W 0x55555555  PDM B2 block Reg mode Output value
    volatile unsigned int  PdmOutRegB3;         //  0x0048  R/W 0xAAAAAAAA  PDM B3 block Reg mode Output value
    volatile unsigned int  PdmOutRegB4;         //  0x004C  R/W 0x55555555  PDM B4 block Reg mode Output value
    volatile unsigned int  PdmOutRegC1;         //  0x0050  R/W 0xAAAAAAAA  PDM C1 block Reg mode Output value
    volatile unsigned int  PdmOutRegC2;         //  0x0054  R/W 0x55555555  PDM C2 block Reg mode Output value
    volatile unsigned int  PdmOutRegC3;         //  0x0058  R/W 0xAAAAAAAA  PDM C3 block Reg mode Output value
    volatile unsigned int  PdmOutRegC4;         //  0x005C  R/W 0x55555555  PDM C4 block Reg mode Output value
    volatile unsigned int  PdmOutRegD1;         //  0x0060  R/W 0xAAAAAAAA  PDM D1 block Reg mode Output value
    volatile unsigned int  PdmOutRegD2;         //  0x0064  R/W 0x55555555  PDM D2 block Reg mode Output value
    volatile unsigned int  PdmOutRegD3;         //  0x0068  R/W 0xAAAAAAAA  PDM D3 block Reg mode Output value
    volatile unsigned int  PdmOutRegD4;         //  0x006C  R/W 0x55555555  PDM D4 block Reg mode Output value
} PDMCONFIG, *PPDMCONFIG;

typedef struct _PWMCONFIG{
    volatile unsigned int  PdmEn;               //  0x0004  R/W 0x000f  PWM block enable & PWM Trigger
    volatile unsigned int  PdmMode;             //  0x0008  R/W 0x000f  PWM output mode & PWMCLKDiv
    volatile unsigned int  PdmAPstn12;          //  0x0010 R/W 0x2010  PWM block A position 1 & 2
    volatile unsigned int  PdmAPstn34;          //  0x0014  R/W 0x4030  PWM block A position 3 & 4
    volatile unsigned int  PdmBPstn12;          //  0x0018  R/W 0x2010  PWM block B position 1 & 2
    volatile unsigned int  PdmBPstn34;          //  0x001C  R/W 0x4030  PWM block B position 3 & 4
    volatile unsigned int  PdmCPstn12;          //  0x0020  R/W 0x2010  PWM block C position 1 & 2
    volatile unsigned int  PdmCPstn34;          //  0x0024  R/W 0x4030  PWM block C position 3 & 4
    volatile unsigned int  PdmDPstn12;          //  0x0028  R/W 0x2010  PWM block D position 1 & 2
    volatile unsigned int  PdmDPstn34;          //  0x002C  R/W 0x4030  PWM block D position 3 & 4
    volatile unsigned int  PdmOutRegA1;         //  0x0030  R/W 0xAAAAAAAA  PWM A1 block Reg mode Output value
    volatile unsigned int  PdmOutRegA2;         //  0x0034  R/W 0x55555555  PWM A2 block Reg mode Output value
    volatile unsigned int  PdmOutRegA3;         //  0x0038  R/W 0xAAAAAAAA  PWM A3 block Reg mode Output value
    volatile unsigned int  PdmOutRegA4;         //  0x003C  R/W 0x55555555  PWM A4 block Reg mode Output value
    volatile unsigned int  PdmOutRegB1;         //  0x0040  R/W 0xAAAAAAAA  PWM B1 block Reg mode Output value
    volatile unsigned int  PdmOutRegB2;         //  0x0044  R/W 0x55555555  PWM B2 block Reg mode Output value
    volatile unsigned int  PdmOutRegB3;         //  0x0048  R/W 0xAAAAAAAA  PWM B3 block Reg mode Output value
    volatile unsigned int  PdmOutRegB4;         //  0x004C  R/W 0x55555555  PWM B4 block Reg mode Output value
    volatile unsigned int  PdmOutRegC1;         //  0x0050  R/W 0xAAAAAAAA  PWM C1 block Reg mode Output value
    volatile unsigned int  PdmOutRegC2;         //  0x0054  R/W 0x55555555  PWM C2 block Reg mode Output value
    volatile unsigned int  PdmOutRegC3;         //  0x0058  R/W 0xAAAAAAAA  PWM C3 block Reg mode Output value
    volatile unsigned int  PdmOutRegC4;         //  0x005C  R/W 0x55555555  PWM C4 block Reg mode Output value
    volatile unsigned int  PdmOutRegD1;         //  0x0060  R/W 0xAAAAAAAA  PWM D1 block Reg mode Output value
    volatile unsigned int  PdmOutRegD2;         //  0x0064  R/W 0x55555555  PWM D2 block Reg mode Output value
    volatile unsigned int  PdmOutRegD3;         //  0x0068  R/W 0xAAAAAAAA  PWM D3 block Reg mode Output value
    volatile unsigned int  PdmOutRegD4;         //  0x006C  R/W 0x55555555  PWM D4 block Reg mode Output value
} PWMCONFIG, *PPWMCONFIG;


/*******************************************************************************
*    20. Remote Control Interface Register Define (Base Addr = 0xF05F3000)
********************************************************************************/
//#define HwREMOTE_BASE							((volatile unsigned long*)0xF05F3000)
typedef struct _REMOTECON{
    volatile unsigned int  RDATA;              // 0x00 R/W 0x0000 IR Data Transfer Address
    volatile unsigned int  CMD;                // 0x04 R/W 0x0000 Command Register
    volatile unsigned int  INPOL;              // 0x08 R/W 0x0000 Control Register
    volatile unsigned int  STA;                // 0x0C W   0x0000 Status register
    volatile unsigned int  NOTDEFINE0[12];
    volatile unsigned int  CLKDIV;             // 0x40 R   0x0000 Clock Divide Register
}REMOTECON, *PREMOTECON;

/*******************************************************************************
*    21. TSADC Interface Register Define (Base Addr = 0xF05F4000)
********************************************************************************/
//#define HwTSADC_BASE							((volatile unsigned long*)0xF05F4000)
typedef struct _TSADC{
    volatile unsigned int  ADCCON;              // 0x00 R/W 0x00007F84 ADC Control Register
    volatile unsigned int  ADCTSC;              // 0x04 R/W 0x00000058 ADC Touch Screen Control Register
    volatile unsigned int  ADCDLY;              // 0x08 R/W 0x000000FF ADC Start or Interval Delay Register
    volatile unsigned int  ADCDAT0;             // 0x0C R - ADC Conversion Data Register
    volatile unsigned int  ADCDAT1;             // 0x10 R - ADC Conversion Data Register
    volatile unsigned int  ADCUPDN;             // 0x14 R/W 0x00000000 Stylus Up or Down Interrupt Register
    volatile unsigned int  ADCCLRINT;           // 0x18 W - Clear ADC Interrrupt
    volatile unsigned int  NOTDEFINE0;          // 0x1C - - Reserved
    volatile unsigned int  ADCCLRUPDN;          // 0x20 W - Clear Pen UP/DOWN Interrupt
}TSADC, *PTSADC;

/*******************************************************************************
*    22. TSIF(Transport Stream Interface) Register Define (Base Addr = 0x)
********************************************************************************/
//#define HwTSIF_BASE								((volatile unsigned long*)0xF0539000)
//#define HwTSIF0_BASE							((volatile unsigned long*)0xF0539000)
//#define HwTSIF1_BASE							((volatile unsigned long*)0xF0539100)
//#define HwTSIF_PORTSEL_BASE						((volatile unsigned long*)0xF0539800)
typedef struct _TSIF{
    volatile unsigned int  TSDI;                //0x00  R   0x0000  TSIF Input Data Register
    volatile unsigned int  TSCR;                //0x04  R/W 0x0300  TSIF Control Register
    volatile unsigned int  TSPID[16];           //0x08~0x44 R/W 0x0000  TSIF PID Register
    volatile unsigned int  TSIC;                //0x48  R/W 0x0080  TSIF Interrupt Control Register
    volatile unsigned int  TSIS;                //0x4C  R   0x0000  TSIF Interrupt Status Register
    volatile unsigned int  TSISP;               //0x50  R   0x0000  TSIF interrupt Status(PID) Register
    volatile unsigned int  NOTDEFINED[43];      //0x54~0xFC
    volatile unsigned int  TSDI1;               //0x100 R   0x0000  TSIF Input Data Register
    volatile unsigned int  TSCR1;               //0x104 R/W 0x0300  TSIF Control Register
    volatile unsigned int  TSPID1[16];          //0x108~0x44    R/W 0x0000  TSIF PID Register
    volatile unsigned int  TSIC1;               //0x148 R/W 0x0080  TSIF Interrupt Control Register
    volatile unsigned int  TSIS1;               //0x14C R   0x0000  TSIF Interrupt Status Register
    volatile unsigned int  TSISP1;              //0x150 R   0x0000  TSIF interrupt Status(PID) Register
    volatile unsigned int  NOTDEFINED1[427];    //0x154~0x7FC
    volatile unsigned int  TSCHS1;              //0x800 R/W 0x0000  TSIF Channel(Port) Select Register
}TSIF, *PTSIF;

typedef struct _TSIFPORTSEL{
    volatile unsigned int  TSCHS; //0x800 R/W 0x0000 TSIF Channel(Port) Select Register
}TSIFPORTSEL, *PTSIFPORTSEL;

/*******************************************************************************
*    23. UART Register Define (Base Addr = 0xF0531000)
********************************************************************************/
//#define HwUART_BASE								((volatile unsigned long*)0xF0531000)
//#define HwUART0_BASE							((volatile unsigned long*)0xF0531000)
//#define HwUART1_BASE							((volatile unsigned long*)0xF0531100)
//#define HwUART2_BASE							((volatile unsigned long*)0xF0531200)
//#define HwUART3_BASE							((volatile unsigned long*)0xF0531300)
//#define HwUART4_BASE							((volatile unsigned long*)0xF0531400)
//#define HwUART5_BASE							((volatile unsigned long*)0xF0531500)
//#define HwUART_PORTMUX_BASE						((volatile unsigned long*)0xF0531600)

typedef union _UARTREG1{
    volatile unsigned int  RBR;                 // 0x00 R Unknown Receiver Buffer Register(DLAB = 0)
    volatile unsigned int  THR;                 // 0x00 W 0x00 Transmitter Holding Register (DLAB=0)
    volatile unsigned int  DLL;                 // 0x00 R/W 0x00 Divisor Latch (LSB) (DLAB=1)
    volatile unsigned int  UTRXD;                 
    volatile unsigned int  UTTXD;                 
}UARTREG1;

typedef union _UARTREG2{
    volatile unsigned int  IER;                 // 0x04 R/W 0x00 Interrupt Enable Register (DLAB=0)
    volatile unsigned int  DLM;                 // 0x04 R/W 0x00 Divisor Latch (MSB) (DLAB=1)
}UARTREG2;

typedef union _UARTREG3{
    volatile unsigned int  IIR;                 // 0x08 R Unknown Interrupt Ident. Register (DLAB=0)
    volatile unsigned int  FCR;                 // 0x08 W 0xC0 FIFO Control Register (DLAB=1)
}UARTREG3;

typedef struct _UART{
/*
    volatile unsigned int  RBR;                 // 0x00 R Unknown Receiver Buffer Register(DLAB = 0)
    volatile unsigned int  THR;                 // 0x00 W 0x00 Transmitter Holding Register (DLAB=0)
    volatile unsigned int  DLL;                 // 0x00 R/W 0x00 Divisor Latch (LSB) (DLAB=1)
    volatile unsigned int  IER;                 // 0x04 R/W 0x00 Interrupt Enable Register (DLAB=0)
    volatile unsigned int  DLM;                 // 0x04 R/W 0x00 Divisor Latch (MSB) (DLAB=1)
    volatile unsigned int  IIR;                 // 0x08 R Unknown Interrupt Ident. Register (DLAB=0)
    volatile unsigned int  FCR;                 // 0x08 W 0xC0 FIFO Control Register (DLAB=1)
*/
    UARTREG1    REG1;
    UARTREG2    REG2;
    UARTREG3    REG3;
    volatile unsigned int  LCR;                 // 0x0C R/W 0x03 Line Control Register
    volatile unsigned int  MCR;                 // 0x10 R/W 0x00 MODEM Control Register
    volatile unsigned int  LSR;                 // 0x14 R Unknown Line Status Register
    volatile unsigned int  MSR;                 // 0x18 R Unknown MODEM Status Register
    volatile unsigned int  SCR;                 // 0x1C R/W 0x00 Scratch Register
    volatile unsigned int  AFT;                 // 0x20 R/W 0x00 AFC Trigger Level Register
    volatile unsigned int  UCR;                 // 0x24 R/W 0x00 UART Control register
    volatile unsigned int  NOTDEFINE0[6];
    volatile unsigned int  SRBR;                // 0x40 R Unknown Rx Buffer Register
    volatile unsigned int  STHR;                // 0x44 W 0x00 Transmitter Holding Register
    volatile unsigned int  SDLL;                // 0x48 R/W 0x00 Divisor Latch (LSB)
    volatile unsigned int  SDLM;                // 0x4C R/W 0x00 Divisor Latch (MSB)
    volatile unsigned int  SIER;                // 0x50 R/W 0x00 Interrupt Enable register
    volatile unsigned int  NOTDEFINE1[3];
    volatile unsigned int  SCCR;                // 0x60 R/W 0x00 Smart Card Control Register
    volatile unsigned int  STC;                 // 0x64 R/W 0x00 Smart Card TX Count register
    volatile unsigned int  NOTDEFINE2[6];
    volatile unsigned int  IRCFG;               // 0x80 R/W 0x00 IRDA Configuration Register
}UART, *PUART;


typedef struct _UARTPORTMUX{
    volatile unsigned int  CHSEL;                   // 0x00 R/W 0x3210 Channel Selection Register
    volatile unsigned int  CHST;                    // 0x00 R 0x0000 Channel Status Register
}UARTPORTMUX, *PUARTPORTMUX;


/*******************************************************************************
*    24. MPEFEC Register Define (Base Addr = 0x0xF0510000)
********************************************************************************/
//#define HwMPEFEC_BASE							((volatile unsigned long*)0xF0510000)
typedef struct _MPEFEC{
    volatile unsigned int  MERR;                    // 0x00 R/W 0x00000000 MPE_FEC Enable/ RESET [1] -> MPE_FEC Enable [0] -> MPE_FEC RESET
    volatile unsigned int  MSR;                     // 0x04 W 0x00000000 MPE_FEC Start (Auto clear)
    volatile unsigned int  MFRNR;                   // 0x08 R/W 0x03FF03FF MPEFEC Frame Row Number Register
	volatile unsigned int  MDSAR;                   // 0x0C R/W 0x00000000 MPEFEC Frame Source Address Register
	volatile unsigned int  EFR;                     // 0x10 R/W 0x00000000 Erasure Flag Source Address Register
    volatile unsigned int  MCR;                     // 0x14 R/W 0x00014000 MPEFEC Control Registe
	volatile unsigned int  MSTR;                    // 0x18 R 0x00000000 MPE_FEC Status [1] -> mpe_done [0] -> mpe_over
    volatile unsigned int  MIER;                    // 0x1C R 0x00000000 MPE_FEC IRQ Enable [1] -> mpe_done IRQ Enable [0] -> mpe_over IRQ Enable
    volatile unsigned int  MICR;                    // 0x20 W 0x00000000 MPE_FEC IRQ_clear [1] -> mpe_done IRQ clear [0] -> mpe_over IRQ clear
    volatile unsigned int  MARR;                    // 0x24 R/W 0x03FF0000 MPEFEC Active row Register
    volatile unsigned int  ECNT;                    // 0x28 R/W 0x00000000 Error Count Register
    volatile unsigned int  NOTDEFINE0[3];           // 0x34-0x3C R/W Reserved
}MPEFEC, *PMPEFEC;


/*******************************************************************************
*    25. RTC(Real-Time Clock) Register Define (Base Addr = 0xF05F2000)
********************************************************************************/
//#define HwRTC_BASE								((volatile unsigned long*)0xF05F2000)
typedef struct _RTC{
    volatile unsigned int  RTCCON;                  // 0x00 R/W 0x00 RTC Control Register
    volatile unsigned int  INTCON;                  // 0x04 R/W -    RTC Interrupt Control Register
    volatile unsigned int  RTCALM;                  // 0x08 R/W -    RTC Alarm Control Register
    volatile unsigned int  ALMSEC;                  // 0x0C R/W -    Alarm Second Data Register

    volatile unsigned int  ALMMIN;                  // 0x10 R/W -    Alarm Minute Data Register
    volatile unsigned int  ALMHOUR;                 // 0x14 R/W -    Alarm Hour Data Register
    volatile unsigned int  ALMDATE;                 // 0x18 R/W -    Alarm Date Data Register
    volatile unsigned int  ALMDAY;                  // 0x1C R/W -    Alarm Day of Week Data Register

    volatile unsigned int  ALMMON;                  // 0x20 R/W -    Alarm Month Data Register
    volatile unsigned int  ALMYEAR;                 // 0x24 R/W -    Alarm Year Data Register
    volatile unsigned int  BCDSEC;                  // 0x28 R/W -    BCD Second Register
    volatile unsigned int  BCDMIN;                  // 0x2C R/W -    BCD Minute Register

    volatile unsigned int  BCDHOUR;                 // 0x30 R/W -    BCD Hour Register
    volatile unsigned int  BCDDATE;                 // 0x34 R/W -    BCD Date Register
    volatile unsigned int  BCDDAY;                  // 0x38 R/W -    BCD Day of Week Register
    volatile unsigned int  BCDMON;                  // 0x3C R/W -    BCD Month Register

    volatile unsigned int  BCDYEAR;                 // 0x40 R/W -    BCD Year Register
    volatile unsigned int  RTCIM;                   // 0x44 R/W -    RTC Interrupt Mode Register
    volatile unsigned int  RTCPEND;                 // 0x48 R/W -    RTC Interrupt Pending Register
    volatile unsigned int  RTCSTR;                  // 0x4C R/W -    RTC Interrupt Status Register
}RTC, *PRTC;


/*******************************************************************************
*    26. IOBUS Configuration Register Define (Base Addr = 0xF05F5000)
********************************************************************************/
//#define HwIOBUSCFG_BASE							((volatile unsigned long*)0xF05F5000)
typedef struct _IOBUSCFG{
	volatile unsigned int USBOTG;					// 0x00 R/W - Refer  to  USB  OTG  Configuration  Register
	volatile unsigned int USB11H;					// 0x04 R/W - Refer to USB 1.1 Host configuration Register
	volatile unsigned int IOBAPB;					// 0x08 R/W - IOBUS APB wait Counter & EMA(Extra Margin Adjustment) Setting
	volatile unsigned int Reserved0[1];				// 0x0C R/W -
	volatile unsigned int HCLKEN0;					// 0x10 R/W - IOBUS AHB Clock Enable Register 0
	volatile unsigned int HCLKEN1;					// 0x14 R/W - IOBUS AHB Clock Enable Register 1
	volatile unsigned int HCLKMEN0;					// 0x18 R/W - DMA AHB clock mask enable Register
	volatile unsigned int Reserved1[1];				// 0x1C R/W -
	volatile unsigned int HRSTEN0;					// 0x20 R/W - IOBUS AHB HRESET Control Register 0
	volatile unsigned int HRSTEN1;					// 0x24 R/W - IOBUS AHB HRESET Control Register 1
	volatile unsigned int USBOTG0;					// 0x28 R/W - Refer   to   USB   PHY   Configuration   Register0
	volatile unsigned int USBOTG1;					// 0x2C R/W - Refer   to   USB   PHY   Configuration   Register1
	volatile unsigned int USBOTG2;					// 0x30 R/W - Refer   to   USB   PHY   Configuration   Register2
	volatile unsigned int USBOTG3;					// 0x34 R/W - Refer   to   USB   PHY   Configuration   Register3
	volatile unsigned int IO_A2X;					// 0x38 R/W - IOBUS AHB2AXI Control Register
	volatile unsigned int IO_BWRAP;					// 0x3C R/W - Overay-Mixer BWRAP Control Register
}IOBUSCFG, *PIOBUSCFG;


/*******************************************************************************
*
*    TCC88x DataSheet PART 6 HSIO BUS
*
********************************************************************************/

/************************************************************************
*   4. PCIe(PCI Expresss)  (Base Addr = 0xF0800000) // R/W
*************************************************************************/
//#define HwPCIE_BASE								((volatile unsigned long*)0xF0800000)
//#define HwPCIE_DBI_BASE							((volatile unsigned long*)0xF0810000)

/************************************************************************
*   5. SATA Host 0/1 (Base Addr = 0xF0820000, 0xF0850000) // R/W
*************************************************************************/
//#define HwSATAHOST0_BASE						((volatile unsigned long*)0xF0820000)
//#define HwSATAHOST1_BASE						((volatile unsigned long*)0xF0850000)

typedef struct _SATAHOST {
	// SATA Host Generic Register
	volatile unsigned int CAP;					//0x00			RO      -
	volatile unsigned int GHC;					//0x04			RW	0x80000000
	volatile unsigned int IS;					//0x08			RW	0x00000000
	volatile unsigned int PI;					//0x0C			RO      -
	volatile unsigned int VS;					//0x10			RO	0x00010100
	volatile unsigned int CCC_CTL;				//0x14			RW	    -
	volatile unsigned int CCC_PORTS;			//0x18			RW	0x00000000
	volatile unsigned int Reserved0[2];			//0x1C ~ 0x23		0x00000000
	volatile unsigned int CAP2;					//0x24			RO	0x00000000
	volatile unsigned int Reserved1[30];		//0x28 ~ 0x9F		0x00000000
	volatile unsigned int BISTAFT;				//0xA0			RO	0x00000000
	volatile unsigned int BISTCR;				//0xA4			RW	0x00000700
	volatile unsigned int BISTFCTR;				//0xA8			RO	0x00000000
	volatile unsigned int BISTSR;				//0xAC			RO	0x00000000
	volatile unsigned int BISTDECR;				//0xB0			RO	0x00000000
	volatile unsigned int Reserved2[2];			//0xB4 ~ 0xBB		0x00000000
	volatile unsigned int OOBR;					//0xBC			RW	0x00000000
	volatile unsigned int Reserved3[8];			//0xC0 ~ 0xDF		0x00000000
	volatile unsigned int TIMER1MS;				//0xE0			RO	0x00000000
	volatile unsigned int Reserved4[1];			//0xE4 ~ 0xE7		0x00000000
	volatile unsigned int GPARAM1R;				//0xE8			RO	0xD8000000
	volatile unsigned int GPARAM2R;				//0xEC			RO	0x00000E1E
	volatile unsigned int PPARAMR;				//0xF0			RO	0x00000022
	volatile unsigned int TESTR;				//0xF4			RW	0x00000000
	volatile unsigned int VERSIONR;				//0xF8			RO      -
	volatile unsigned int IDR;					//0xFC			RO      -

	// SATA Host Port Register
	volatile unsigned int P0CLB;				//0x100			RW	0x00000000
	volatile unsigned int Reserved5[1];			//0x104	~ 0x107	RW	0x00000000
	volatile unsigned int P0FB;					//0x108			RW	0x00000000
	volatile unsigned int Reserved6[1];			//0x10C	~ 0x10F	RW	0x00000000
	volatile unsigned int P0IS;					//0x110			RW	0x00000000
	volatile unsigned int P0IE;					//0x114			RW	0x00000000
	volatile unsigned int P0CMD;				//0x118			RW	0x00000000
	volatile unsigned int Reserved7[1];			//0x11C ~ 0x11F	RW	0x00000000
	volatile unsigned int P0TFD;				//0x120			RO	0x0000007F
	volatile unsigned int P0SIG;				//0x124			RO	0xFFFFFFFF
	volatile unsigned int P0SSTS;				//0x128			RO	0x00000000
	volatile unsigned int P0SCTL;				//0x12C			RW	0x00000000
	volatile unsigned int P0SERR;				//0x130			RW	0x00000000
	volatile unsigned int P0SACT;				//0x134			RW	0x00000000
	volatile unsigned int P0CI;					//0x138			RW	0x00000000
	volatile unsigned int P0SNTF;				//0x13C			RW	0x00000000
	volatile unsigned int Reserved8[12];		//0x140	~ 0x16F	RW	0x00000000
	volatile unsigned int P0DMACR;				//0x170			RW	0x00000044
} SATAHOST, *PSATAHOST;


/************************************************************************
*   6. SATA Device (Base Addr = 0xF0860000) // R/W
*************************************************************************/
//#define HwSATADEVICE_BASE						((volatile unsigned long*)0xF0860000)

typedef struct _SATADEVICE{
	//SATA Device Global Register
	volatile unsigned int CFGR;					//0x00			HwInit		0x00000000
	volatile unsigned int GCR;					//0x04			See Desc	0x00000000
	volatile unsigned int Reserved0[46];		//0x08 ~ 0xBF	-			0x00000000
	volatile unsigned int BISTAFR;				//0xA0			RO			0x00000000
	volatile unsigned int BISTCR;				//0xA4			RW			0x00000700
	volatile unsigned int BISTFCTR;				//0xA8			RO			0x00000000
	volatile unsigned int BISTSR;				//0xAC			RO			0x00000000
	volatile unsigned int BISTDECR;				//0xB0			RO			0x00000000
	volatile unsigned int Reserved1[2];			//0xB4 ~ 0xBB	-			0x00000000
	volatile unsigned int OOBR;					//0xBC			RW/RO		0x00000000
	volatile unsigned int Reserved2[10];		//0xC0 ~ 0xE7	-			0x00000000
	volatile unsigned int GPARAM1R;				//0xE8			RO			0xD8000000
	volatile unsigned int GPARAM2R;				//0xEC			RO			0x00000E3C
	volatile unsigned int PPARAMR;				//0xF0			RO			0x00000022
	volatile unsigned int TESTR;				//0xF4			RW			0x00000000
	volatile unsigned int VERSIONR;				//0xF8			RO			0x00000000
	volatile unsigned int IDR;					//0xFC			RO			0x00000000

	//SATA Device Port Register
	volatile unsigned int PCLBR;				//0x100			RW			0x00000000
	volatile unsigned int Reserved3[2];			//0x100	~ 0x107	-			0x00000000
	volatile unsigned int PFBR;					//0x108			RW			0x00000000
	volatile unsigned int Reserved4[1];			//0x10C			-			0x00000000
	volatile unsigned int PISR;					//0x110			RW/RO		0x00000000
	volatile unsigned int PIER;					//0x114			RW			0x00000000
	volatile unsigned int PCMDR;				//0x118			RW/RO		0x00000000
	volatile unsigned int Reserved5[3];			//0x11C ~ 0x127	-			0x00000000
	volatile unsigned int PSSTSR;				//0x128			RO			0x00000000
	volatile unsigned int PSCTLR;				//0x12C			RW			0x00000000
	volatile unsigned int PSERR;				//0x130			RW			0x00000000
	volatile unsigned int Reserved6;			//0x134 ~ 0x137	-			0x00000000
	volatile unsigned int PDTR;					//0x138			RW			0x00000000
	volatile unsigned int Reserved7[13];		//0x13C ~ 0x16F	-			0x00000000
	volatile unsigned int PDMACR;				//0x170			RW/RO		0x00000044
	volatile unsigned int Reserved8[1];			//0x174 ~ 0x177	-			0x00000000
	volatile unsigned int PHYCR;				//0x178			RW			0x00000000
	volatile unsigned int PHYSR;				//0x17C			RO			0x00000000
	volatile unsigned int PPICR;				//0x180			RO			0x00000000
	volatile unsigned int PPBCR;				//0x184			RO			0x00000000
} SATADEVICE, *PSATADEVICE;

/************************************************************************
*   7. SATA(PCIe) PHY 0/1 (Base Addr = 0xF0830000, 0xF0870000) // R/W
*************************************************************************/
//#define HwSATAPHY0_BASE							((volatile unsigned long*)0xF0830000)
//#define HwSATAPHY1_BASE							((volatile unsigned long*)0xF0870000)

typedef struct _SATAPHY_CMU{
	//SATA PHY Common CMU Register
	volatile unsigned char CMU[256];				//0x000			RW			0x00000000
													//CMU 115~238 : Reserved
													//CMU 240~254 : Reserved
} SATAPHY_CMU, *PSATAPHY_CMU;

typedef struct _SATAPHY_LANE0{
	//SATA PHY Common CMU Register
	volatile unsigned char LANE0[82];				//0x200			RW			0x00000000
													//LANE0_9~15 : Reserved
} SATAPHY_LANE0, *PSATAPHY_LANE0;

typedef struct _SATAPHY_CLANE{
	//SATA PHY Common CMU Register
	volatile unsigned char CLANE[82];				//0xA00			RW			0x00000000
													//CLANE_24~47 : Reserved
} SATAPHY_LANE, *PSATAPHY_CLANE;

/************************************************************************
*   8. Ethernet(GMAC) (Base Addr = 0xF0840000) // R/W
*************************************************************************/
//#define HwGMAC_BASE								((volatile unsigned long*)0xF0840000)
typedef struct _GMACDMA{
    volatile unsigned int BMODE;         // 0x00    32  R/W     0x00020101  Bus mode register
    volatile unsigned int TPD;           // 0x04    32  R/W     0x00000000  Transmit poll demand register
    volatile unsigned int RPD;           // 0x08    32  R/W     0x00000000  Receive poll demand register
    volatile unsigned int RDLA;          // 0x0C    32  R/W     0x00000000  Receive descriptor list address register
    volatile unsigned int TDLA;          // 0x10    32  R/W     0x00000000  Transmit descriptor list address reigster
    volatile unsigned int STS;           // 0x14    32  RO      -               Status register
    volatile unsigned int OPMODE;        // 0x18    32  R/W     0x00000000  Operation mode register
    volatile unsigned int IE;            // 0x1C    32  R/W     0x00000000  Interrupt enable register
    volatile unsigned int MFBOC;         // 0x20    32  R/C     0x00000000  Missed frame and buffer overflow counter register
    volatile unsigned int _NOTDEFINED[9];
    volatile unsigned int CHTD;          // 0x48    32  RO      -           Current host transmit descriptor register
    volatile unsigned int CHRD;          // 0x4C    32  RO      -           Current host receive descriptor register
    volatile unsigned int CHTBA;         // 0x50    32  RO      -           Current host transmit buffer address register
    volatile unsigned int CHRBA;         // 0x54    32  RO      -           Current host receive buffer address register
} GMACDMA, *PGMACDMA;

//#define HwGMAC_BASE                            *(volatile unsigned long*)0xB0820000
typedef struct _GMAC{
    volatile unsigned int MACC;                     // 0x000    32  R/W 0x00000000  MAC configuration register
    volatile unsigned int MACFF;                    // 0x004    32  R/W 0x00000000  MAC frame filter
    volatile unsigned int HTH;                      // 0x008    32  R/W 0x00000000  Hash table high register
    volatile unsigned int HTL;                      // 0x00C    32  R/W 0x00000000  Hash table low register
    volatile unsigned int GMIIA;                    // 0x010    32  R/W 0x00000000  GMII address register
    volatile unsigned int GMIID;                    // 0x014    32  R/W 0x00000000  GMII data register
    volatile unsigned int FC;                       // 0x018    32  R/W 0x00000000  Flow control register
    volatile unsigned int VLANT;                    // 0x01C    32  R/W 0x00000000  VLAN tag register
    volatile unsigned int VERSION;                  // 0x020    32  R/W 0x0000--34  Version register
    volatile unsigned int _NOTDEFINED0[1];
    volatile unsigned int RWFF;                     // 0x028    32  R/W 0x00000000  Remote wake-up frame register
    volatile unsigned int PMTCS;                    // 0x02C    32  R/W 0x00000000  PMT control and status
    volatile unsigned int _NOTDEFINED1[2];
    volatile unsigned int IRQS;                     // 0x038    32  RO  -           Interrupt status register
    volatile unsigned int IRQM;                     // 0x03C    32  R/W 0x00000000  Interrupt mask register
    volatile unsigned int MACA0H;                   // 0x040    32  R/W 0x0000FFFF  MAC address0 high register
    volatile unsigned int MACA0L;                   // 0x044    32  R/W 0xFFFFFFFF  MAC address0 low register
    volatile unsigned int MACA1H;                   // 0x048    32  R/W 0x0000FFFF  MAC address1 high register
    volatile unsigned int MACA1L;                   // 0x04C    32  R/W 0xFFFFFFFF  MAC address1 low register
    volatile unsigned int MACA2H;                   // 0x050    32  R/W 0x0000FFFF  MAC address2 high register
    volatile unsigned int MACA2L;                   // 0x054    32  R/W 0xFFFFFFFF  MAC address2 low register
    volatile unsigned int MACA3H;                   // 0x058    32  R/W 0x0000FFFF  MAC address3 high register
    volatile unsigned int MACA3L;                   // 0x05C    32  R/W 0xFFFFFFFF  MAC address3 low register
    volatile unsigned int MACA4H;                   // 0x060    32  R/W 0x0000FFFF  MAC address4 high register
    volatile unsigned int MACA4L;                   // 0x064    32  R/W 0xFFFFFFFF  MAC address4 low register
    volatile unsigned int MACA5H;                   // 0x068    32  R/W 0x0000FFFF  MAC address5 high register
    volatile unsigned int MACA5L;                   // 0x06C    32  R/W 0xFFFFFFFF  MAC address5 low register
    volatile unsigned int MACA6H;                   // 0x070    32  R/W 0x0000FFFF  MAC address6 high register
    volatile unsigned int MACA6L;                   // 0x074    32  R/W 0xFFFFFFFF  MAC address6 low register
    volatile unsigned int MACA7H;                   // 0x078    32  R/W 0x0000FFFF  MAC address7 high register
    volatile unsigned int MACA7L;                   // 0x07C    32  R/W 0xFFFFFFFF  MAC address7 low register
    volatile unsigned int MACA8H;                   // 0x080    32  R/W 0x0000FFFF  MAC address8 high register
    volatile unsigned int MACA8L;                   // 0x084    32  R/W 0xFFFFFFFF  MAC address8 low register
    volatile unsigned int MACA9H;                   // 0x088    32  R/W 0x0000FFFF  MAC address9 high register
    volatile unsigned int MACA9L;                   // 0x08C    32  R/W 0xFFFFFFFF  MAC address9 low register
    volatile unsigned int MACA10H;                  // 0x090    32  R/W 0x0000FFFF  MAC address10 high register
    volatile unsigned int MACA10L;                  // 0x094    32  R/W 0xFFFFFFFF  MAC address10 low register
    volatile unsigned int MACA11H;                  // 0x098    32  R/W 0x0000FFFF  MAC address11 high register
    volatile unsigned int MACA11L;                  // 0x09C    32  R/W 0xFFFFFFFF  MAC address11 low register
    volatile unsigned int MACA12H;                  // 0x0A0    32  R/W 0x0000FFFF  MAC address12 high register
    volatile unsigned int MACA12L;                  // 0x0A4    32  R/W 0xFFFFFFFF  MAC address12 low register
    volatile unsigned int MACA13H;                  // 0x0A8    32  R/W 0x0000FFFF  MAC address13 high register
    volatile unsigned int MACA13L;                  // 0x0AC    32  R/W 0xFFFFFFFF  MAC address13 low register
    volatile unsigned int MACA14H;                  // 0x0B0    32  R/W 0x0000FFFF  MAC address14 high register
    volatile unsigned int MACA14L;                  // 0x0B4    32  R/W 0xFFFFFFFF  MAC address14 low register
    volatile unsigned int MACA15H;                  // 0x0B8    32  R/W 0x0000FFFF  MAC address15 high register
    volatile unsigned int MACA15L;                  // 0x0BC    32  R/W 0xFFFFFFFF  MAC address15 low register
    volatile unsigned int _NOTDEFINED2[6];
    volatile unsigned int RGMIIS;                   // 0x0D8    32  RO  -           RGMII status register
    volatile unsigned int _NOTDEFINED3[9];
    volatile unsigned int MMC_CNTRL;                // 0x100    32  RW  0x00000000  MMC control establishes the operating mode of MMC
    volatile unsigned int MMC_INTR_RX;              // 0x104    32  R/C -           MMC receive interrupt maintains the interrupt generated from all of the receive statistic counters.
    volatile unsigned int MMC_INTR_TX;              // 0x108    32  R/C -           MMC transmit interrupt maintains the interrupt generated form all of the transmit statistic counters.
    volatile unsigned int MMC_INTR_MASK_RX;         // 0x10C    32  RW  0x00000000  MMC receive interrupt mask maintains the mask for the interrupt generated form all of the receive statistic counters
    volatile unsigned int MMC_INTR_MASK_TX;         // 0x110    32  RW  0x00000000  MMC transmit interrupt mask maintains the mask for the interrupt generated form all of the transmit statistic counters
    volatile unsigned int TXOCTETCOUNT_GB;          // 0x114    32  RO  -           Number of bytes transmitted, exclusive of preamble and retried bytes, in good and bad frames.
    volatile unsigned int TXFRAMECOUNT_GB;          // 0x118    32  RO  -           Number of good and bad frames transmitted, exclusive of retried frames.
    volatile unsigned int TXBROADCASTFRAMES_G;      // 0x11C    32  RO  -           Number of good broadcast frames transmitted.
    volatile unsigned int TXMULTICASTFRAMES_G;      // 0x120    32  RO  -           Number of good multicast frames transmitted.
    volatile unsigned int TX64OCTETS_GB;            // 0x124    32  RO  -           Number of good and bad frames transmitted with length 64 bytes, exclusive of preamble and retried frames.
    volatile unsigned int TX65TO127OCTETS_GB;       // 0x128    32  RO  -           Number of good and bad frames transmitted with length between 65 and 127 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile unsigned int TX128TO255OCTETS_GB;      // 0x12C    32  RO  -           Number of good and bad frames transmitted with length between 128 and 255 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile unsigned int TX256TO511OCTETS_GB;      // 0x130    32  RO  -           Number of good and bad frames transmitted with length between 256 and 511 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile unsigned int TX512TO1023OCTETS_GB;     // 0x134    32  RO  -           Number of good and bad frames transmitted with length between 512 and 1,023 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile unsigned int TX1024TOMAXOCTETS_GB;     // 0x138    32  RO  -           Number of good and bad frames transmitted with length between 1,024 and maxsize (inclusive) bytes, exclusive of preamble and retried frames.
    volatile unsigned int TXUNICASTFRAMES_GB;       // 0x13C    32  RO  -           Number of good and bad unicast frames transmitted.
    volatile unsigned int TXMULTICASTFRAMES_GB;     // 0x140    32  RO  -           Number of good and bad multicast frames transmitted.
    volatile unsigned int TXBROADCASTFRAMES_GB;     // 0x144    32  RO  -           Number of good and bad broadcast frames transmitted.
    volatile unsigned int TXUNDERFLOWERROR;         // 0x148    32  RO  -           Number of frames aborted due to frame underflow error.
    volatile unsigned int TXSINGLECOL_G;            // 0x14C    32  RO  -           Number of successfully transmitted frames after a single collision in Half-duplex mode.
    volatile unsigned int TXMULTICOL_G;             // 0x150    32  RO  -           Number of successfully transmitted frames after more than a single collision in Half-duplex mode.
    volatile unsigned int TXDEFERRED;               // 0x154    32  RO  -           Number of successfully transmitted frames after a deferral in Half-duplex mode.
    volatile unsigned int TXLATECOL;                // 0x158    32  RO  -           Number of frames aborted due to late collision error.
    volatile unsigned int TXEXESSCOL;               // 0x15C    32  RO  -           Number of frames aborted due to excessive (16) collision errors.
    volatile unsigned int TXCARRIERERROR;           // 0x160    32  RO  -           Number of frames aborted due to carrier sense error (no carrier or loss of carrier).
    volatile unsigned int TXOCTETCOUNT_G;           // 0x164    32  RO  -           Number of bytes transmitted, exclusive of preamble, in good frames only.
    volatile unsigned int TXFRAMECOUNT_G;           // 0x168    32  RO  -           Number of good frames transmitted.
    volatile unsigned int TXEXCESSDEF;              // 0x16C    32  RO  -           Number of frames aborted due to excessive deferral error (deferred for more  than two max-sized frame times).
    volatile unsigned int TXPAUSEFRAMES;            // 0x170    32  RO  -           Number of good PAUSE frames transmitted.
    volatile unsigned int TXVLANFRAMES_G;           // 0x174    32  RO  -           Number of good VLAN frames transmitted, exclusive of retried frames.
    volatile unsigned int _NOTDEFINED4[2];
    volatile unsigned int RXFRAMECOUNT_GB;          // 0x180    32  RO  -           Number of good and bad frames received.
    volatile unsigned int RXOCTETCOUNT_GB;          // 0x184    32  RO  -           Number of bytes received, exclusive of preamble, in good and bad frames.
    volatile unsigned int RXOCTETCOUNT_G;           // 0x188    32  RO  -           Number of bytes received, exclusive of preamble, only in good frames.
    volatile unsigned int RXBROADCASTFRAMES_G;      // 0x18C    32  RO  -           Number of good broadcast frames received.
    volatile unsigned int RXMULTICASTFRAMES_G;      // 0x190    32  RO  -           Number of good multicast frames received.
    volatile unsigned int RXCRCERROR;               // 0x194    32  RO  -           Number of frames received with CRC error.
    volatile unsigned int RXALIGNMENTERROR;         // 0x198    32  RO  -           Number of frames received with alignment (dribble) error. Valid only in 10/100 mode.
    volatile unsigned int RXRUNTERROR;              // 0x19C    32  RO  -           Number of frames received with runt (<64 bytes and CRC error) error.
    volatile unsigned int RXJABBERERROR;            // 0x1A0    32  RO  -           Number of giant frames received with length (including CRC) greater than 1,518 bytes (1,522 bytes for VLAN tagged) and with CRC error. If Jumbo Frame mode is enabled, then frames of length greater than 9,018 bytes (9,022 for VLAN tagged) are considered as giant frames.
    volatile unsigned int RXUNDERSIZE_G;            // 0x1A4    32  RO  -           Number of frames received with length less than 64 bytes, without any errors.
    volatile unsigned int RXOVERSIZE_G;             // 0x1A8    32  RO  -           Number of frames received with length greater than the maxsize (1,518 or 1,522 for VLAN tagged frames), without errors.
    volatile unsigned int RX64OCTETS_GB;            // 0x1AC    32  RO  -           Number of good and bad frames received with length 64 bytes, exclusive of preamble.
    volatile unsigned int RX65TO127OCTETS_GB;       // 0x1B0    32  RO  -           Number of good and bad frames received with length between 65 and 127 (inclusive) bytes, exclusive of preamble.
    volatile unsigned int RX128TO255OCTETS_GB;      // 0x1B4    32  RO  -           Number of good and bad frames received with length between 128 and 255 (inclusive) bytes, exclusive of preamble.
    volatile unsigned int RX256TO511OCTETS_GB;      // 0x1B8    32  RO  -           Number of good and bad frames received with length between 256 and 511 (inclusive) bytes, exclusive of preamble.
    volatile unsigned int RX512TO1023OCTETS_GB;     // 0x1BC    32  RO  -           Number of good and bad frames received with length between 512 and 1,023 (inclusive) bytes, exclusive of preamble.
    volatile unsigned int RX1024TOMAXOCTETS_GB;     // 0x1C0    32  RO  -           Number of good and bad frames received with length between 1,024 and maxsize (inclusive) bytes, exclusive of preamble and retried frames.
    volatile unsigned int RXUNICASTFRAMES_G;        // 0x1C4    32  RO  -           Number of good unicast frames received.
    volatile unsigned int RXLENGTHERROR;            // 0x1C8    32  RO  -           Number of frames received with length error (Length type field ¡Á frame size), for all frames with valid length field.
    volatile unsigned int RXOUTOFRANGETYPE;         // 0x1CC    32  RO  -           Number of frames received with length field not equal to the valid frame size (greater than 1,500 but less than 1,536).
    volatile unsigned int RXPAUSEFRAMES;            // 0x1D0    32  RO  -           Number of missed received frames due to FIFO overflow. This counter is not present in the GMAC-CORE configuration.
    volatile unsigned int rxfifooverflow;           // 0x1D4    32  RO  -           Number of missed received frames due to FIFO overflow. This counter is not present in the GMAC-CORE configuration.
    volatile unsigned int rxvlanframes_gb;          // 0x1D8    32  RO  -           Number of good and bad VLAN frames received.
    volatile unsigned int rxwatchdogerror;          // 0x1DC    32  RO  -           Number of frames received with error due to watchdog timeout error (frames with a data load larger than 2,048 bytes).
    volatile unsigned int _NOTDEFINED5[8];
    volatile unsigned int mmc_ipc_intr_mask_rx;     // 0x200    32  R/W 0x00000000  MMC IPC Receive Checksum Offload Interrupt Mask maintains the mask for the interrupt generated from the receive IPC statistic counters.
    volatile unsigned int _NOTDEFINED6[1];
    volatile unsigned int mmc_ipc_intr_rx;          // 0x208    32  R/C 0x00000000  MMC Receive Checksum Offload Interrupt maintains the interrupt that the receive IPC statistic counters generate.
    volatile unsigned int _NOTDEFINED7[1];
    volatile unsigned int rxipv4_gd_frms;           // 0x210    32  RO  -           Number of good IPv4 datagrams received with the TCP, UDP, or ICMP payload
    volatile unsigned int rxipv4_hdrerr_frms;       // 0x214    32  RO  -           Number of IPv4 datagrams received with header (checksum, length, or version mismatch) errors
    volatile unsigned int rxipv4_nopay_frms;        // 0x218    32  RO  -           Number of IPv4 datagram frames received that did not have a TCP, UDP, or ICMP payload processed by the Checksum engine
    volatile unsigned int rxipv4_frag_frms;         // 0x21C    32  RO  -           Number of good IPv4 datagrams with fragmentation
    volatile unsigned int rxipv4_udsbl_frms;        // 0x220    32  RO  -           Number of good IPv4 datagrams received that had a UDP payload with checksum disabled
    volatile unsigned int rxipv6_gd_frms;           // 0x224    32  RO  -           Number of good IPv6 datagrams received with TCP, UDP, or ICMP payloads
    volatile unsigned int rxipv6_hdrerr_frms;       // 0x228    32  RO  -           Number of IPv6 datagrams received with header errors (length or version mismatch)
    volatile unsigned int rxipv6_nopay_frms;        // 0x22C    32  RO  -           Number of IPv6 datagram frames received that did not have a TCP, UDP, or ICMP payload. This includes all IPv6 datagrams with fragmentation or security extension headers
    volatile unsigned int rxudp_gd_frms;            // 0x230    32  RO  -           Number of good IP datagrams with a good UDP payload. This counter is not updated when the rxipv4_udsbl_frms counter is incremented.
    volatile unsigned int rxudp_err_frms;           // 0x234    32  RO  -           Number of good IP datagrams whose UDP payload has a checksum error
    volatile unsigned int rxtcp_gd_frms;            // 0x238    32  RO  -           Number of good IP datagrams with a good TCP payload
    volatile unsigned int rxtcp_err_frms;           // 0x23C    32  RO  -           Number of good IP datagrams whose TCP payload has a checksum error
    volatile unsigned int rxicmp_gd_frms;           // 0x240    32  RO  -           Number of good IP datagrams with a good ICMP payload
    volatile unsigned int rxicmp_err_frms;          // 0x244    32  RO  -           Number of good IP datagrams whose ICMP payload has a checksum error
    volatile unsigned int _NOTDEFINED8[2];
    volatile unsigned int rxipv4_gd_octets;         // 0x250    32  RO  -           Number of bytes received in good IPv4 datagrams encapsulating TCP, UDP, or ICMP data. (Ethernet header, FCS, pad, or IP pad bytes are not included in this counter or in the octet counters listed below).
    volatile unsigned int rxipv4_hdrerr_octets;     // 0x254    32  RO  -           Number of bytes received in IPv4 datagrams with header errors (checksum, length, version mismatch). The value in the Length field of IPv4 header is used to update this counter.
    volatile unsigned int rxipv4_nopay_octets;      // 0x258    32  RO  -           Number of bytes received in IPv4 datagrams that did not have a TCP, UDP, or ICMP payload. The value in the IPv4 header¡¯s Length field is used to update this counter.
    volatile unsigned int rxipv4_frag_octets;       // 0x25C    32  RO  -           Number of bytes received in fragmented IPv4 datagrams. The value in the IPv4 header¡¯s Length field is used to update this counter.
    volatile unsigned int rxipv4_udsbl_octets;      // 0x260    32  RO  -           Number of bytes received in a UDP segment that had the UDP checksum disabled. This counter does not count IP Header bytes.
    volatile unsigned int rxipv6_gd_octets;         // 0x264    32  RO  -           Number of bytes received in good IPv6 datagrams encapsulating TCP, UDP or ICMPv6 data
    volatile unsigned int rxipv6_hdrerr_octets;     // 0x268    32  RO  -           Number of bytes received in IPv6 datagrams with header errors (length, version mismatch). The value in the IPv6 header¡¯s Length field is used to update this counter.
    volatile unsigned int rxipv6_nopay_octets;      // 0x26C    32  RO  -           Number of bytes received in IPv6 datagrams that did not have a TCP, UDP, or ICMP payload. The value in the IPv6 header¡¯s Length field is used to update this counter.
    volatile unsigned int rxudp_gd_octets;          // 0x270    32  RO  -           Number of bytes received in a good UDP segment. This counter (and the counters below) does not count IP header bytes.
    volatile unsigned int rxudp_err_octets;         // 0x274    32  RO  -           Number of bytes received in a UDP segment that had checksum errors
    volatile unsigned int rxtcp_gd_octets;          // 0x278    32  RO  -           Number of bytes received in a good TCP segment
    volatile unsigned int rxtcp_err_octets;         // 0x27C    32  RO  -           Number of bytes received in a TCP segment with checksum errors
    volatile unsigned int rxicmp_gd_octets;         // 0x280    32  RO  -           Number of bytes received in a good ICMP segment
    volatile unsigned int rxicmp_err_octets;        // 0x284    32  RO  -           Number of bytes received in an ICMP segment with checksum errors
    volatile unsigned int _NOTDEFINED9[286];
    volatile unsigned int TSC;                      // 0x700    32  R/W 0x00000000  Time stamp control register
    volatile unsigned int SSI;                      // 0x704    32  R/W 0x00000000  Sub-second increment register
    volatile unsigned int TSH;                      // 0x708    32  RO  -           Time stamp high register
    volatile unsigned int TSL;                      // 0x70C    32  RO  -           Time stamp low register
    volatile unsigned int TSHU;                     // 0x710    32  R/W 0x00000000  Time stamp high update register
    volatile unsigned int TSLU;                     // 0x714    32  R/W 0x00000000  Time stamp low update register
    volatile unsigned int TSA;                      // 0x718    32  R/W 0x00000000  Time stamp addend register
    volatile unsigned int TTH;                      // 0x71C    32  R/W 0x00000000  Target time high register
    volatile unsigned int TTL;                      // 0x720    32  R/W 0x00000000  Target time low register
    volatile unsigned int _NOTDEFINED10[55];
    volatile unsigned int MACA16H;                  // 0x800    32  R/W 0x0000FFFF  MAC address16 high register
    volatile unsigned int MACA16L;                  // 0x804    32  R/W 0xFFFFFFFF  MAC address16 low register
    volatile unsigned int MACA17H;                  // 0x808    32  R/W 0x0000FFFF  MAC address17 high register
    volatile unsigned int MACA17L;                  // 0x80C    32  R/W 0xFFFFFFFF  MAC address17 low register
    volatile unsigned int MACA18H;                  // 0x810    32  R/W 0x0000FFFF  MAC address18 high register
    volatile unsigned int MACA18L;                  // 0x814    32  R/W 0xFFFFFFFF  MAC address18 low register
    volatile unsigned int MACA19H;                  // 0x818    32  R/W 0x0000FFFF  MAC address19 high register
    volatile unsigned int MACA19L;                  // 0x81C    32  R/W 0xFFFFFFFF  MAC address19 low register
    volatile unsigned int MACA20H;                  // 0x820    32  R/W 0x0000FFFF  MAC address20 high register
    volatile unsigned int MACA20L;                  // 0x824    32  R/W 0xFFFFFFFF  MAC address20 low register
    volatile unsigned int MACA21H;                  // 0x828    32  R/W 0x0000FFFF  MAC address21 high register
    volatile unsigned int MACA21L;                  // 0x82C    32  R/W 0xFFFFFFFF  MAC address21 low register
    volatile unsigned int MACA22H;                  // 0x830    32  R/W 0x0000FFFF  MAC address22 high register
    volatile unsigned int MACA22L;                  // 0x834    32  R/W 0xFFFFFFFF  MAC address22 low register
    volatile unsigned int MACA23H;                  // 0x838    32  R/W 0x0000FFFF  MAC address23 high register
    volatile unsigned int MACA23L;                  // 0x83C    32  R/W 0xFFFFFFFF  MAC address23 low register
    volatile unsigned int MACA24H;                  // 0x840    32  R/W 0x0000FFFF  MAC address24 high register
    volatile unsigned int MACA24L;                  // 0x844    32  R/W 0xFFFFFFFF  MAC address24 low register
    volatile unsigned int MACA25H;                  // 0x848    32  R/W 0x0000FFFF  MAC address25 high register
    volatile unsigned int MACA25L;                  // 0x84C    32  R/W 0xFFFFFFFF  MAC address25 low register
    volatile unsigned int MACA26H;                  // 0x850    32  R/W 0x0000FFFF  MAC address26 high register
    volatile unsigned int MACA26L;                  // 0x854    32  R/W 0xFFFFFFFF  MAC address26 low register
    volatile unsigned int MACA27H;                  // 0x858    32  R/W 0x0000FFFF  MAC address27 high register
    volatile unsigned int MACA27L;                  // 0x85C    32  R/W 0xFFFFFFFF  MAC address27 low register
    volatile unsigned int MACA28H;                  // 0x860    32  R/W 0x0000FFFF  MAC address28 high register
    volatile unsigned int MACA28L;                  // 0x864    32  R/W 0xFFFFFFFF  MAC address28 low register
    volatile unsigned int MACA29H;                  // 0x868    32  R/W 0x0000FFFF  MAC address29 high register
    volatile unsigned int MACA29L;                  // 0x86C    32  R/W 0xFFFFFFFF  MAC address29 low register
    volatile unsigned int MACA30H;                  // 0x870    32  R/W 0x0000FFFF  MAC address30 high register
    volatile unsigned int MACA30L;                  // 0x874    32  R/W 0xFFFFFFFF  MAC address30 low register
    volatile unsigned int MACA31H;                  // 0x878    32  R/W 0x0000FFFF  MAC address31 high register
    volatile unsigned int MACA31L;                  // 0x87C    32  R/W 0xFFFFFFFF  MAC address31 low register
} GMAC, *PGMAC;


/************************************************************************
*   9. USB 2.0 Host (Base Addr = 0xF0880000, 0xF0890000) // R/W
*************************************************************************/
//#define HwUSB20HOST_EHCI_BASE					((volatile unsigned long*)0xF0880000)
//#define HwUSB20HOST_OHCI_BASE					((volatile unsigned long*)0xF0890000)

typedef struct _USB20_EHCI {
	volatile unsigned int HCCAPBASE;			//0x000			RO	0x01000010
	volatile unsigned int HCSPARAMS;			//0x004			RO	0x00001116
	volatile unsigned int HCCPARAMS;			//0x008			RW	0x0000A010
	volatile unsigned int USBCMD;				//0x010			RW	0x00080000 or 0x00080B00
	volatile unsigned int USBSTS;				//0x014			RW	0x00001000
	volatile unsigned int USBINTR;				//0x018			RW	0x00000000
	volatile unsigned int FRINDEX;				//0x01C			RW	0x00000000
	volatile unsigned int CTRLDSSEGMENT;		//0x020			RW	0x00000000
	volatile unsigned int PERIODICLISTBASE;		//0x024			RW	0x00000000
	volatile unsigned int ASYNCLISTADDR;		//0x028			RW	0x00000000
	volatile unsigned int Reserved0[10];		//0x02C ~ 0x04F -        -
	volatile unsigned int CONFIGFLAG;			//0x050			RW	0x00000000
	volatile unsigned int PORTSC[15];			//0x054	~ 0x08C	RW	0x00002000
	volatile unsigned int INSNREG[6];			//0x090 ~ 0x107	RW	0x00000000, 0x00100010, 0x00000100,
												//					0x00000000, 0x00000000, 0x00001000
} USB20_EHCI, *PUSB20_EHCI;

/************************************************************************
*   10. Cipher (Base Addr = 0xF08A2000) // R/W
*************************************************************************/
//#define HwCIPHER_BASE							((volatile unsigned long*)0xF08A2000)
typedef struct _CIPHER{
    volatile unsigned int CTRL  ;           // 0x00  32 R/W  0x00000000 Cipher control regsiter
    volatile unsigned int TXBASE;           // 0x04  32 R/W  0x00000000 TX base address register
    volatile unsigned int RXBASE;           // 0x08  32 R/W  0x00000000 RX base address register
    volatile unsigned int PACKET;           // 0x0C  32 R/W  0x00000000 Packet register
    volatile unsigned int DMACTR;           // 0x10  32 R/W  0x00000000 DMA control register
    volatile unsigned int DMASTR;           // 0x14  32 RO   -          DMA status register
    volatile unsigned int IRQCTR;           // 0x18  32 R/W  0x00000000 Interrupt control register
    volatile unsigned int BLKNUM;           // 0x1C  32 RO   -          Block count register
    volatile unsigned int ROUND ;           // 0x20  32 R/W  0x00000000 Round register
    volatile unsigned int _NOTDEFINED[7];
    volatile unsigned int KEY0  ;           // 0x40  32 R/W  0x00000000 Key0 register
    volatile unsigned int KEY1  ;           // 0x44  32 R/W  0x00000000 Key1 register
    volatile unsigned int KEY2  ;           // 0x48  32 R/W  0x00000000 Key2 register
    volatile unsigned int KEY3  ;           // 0x4C  32 R/W  0x00000000 Key3 register
    volatile unsigned int KEY4  ;           // 0x50  32 R/W  0x00000000 Key4 register
    volatile unsigned int KEY5  ;           // 0x54  32 R/W  0x00000000 Key5 register
    volatile unsigned int KEY6  ;           // 0x58  32 R/W  0x00000000 Key6 register
    volatile unsigned int KEY7  ;           // 0x5C  32 R/W  0x00000000 Key7 register
    volatile unsigned int KEY8  ;           // 0x60  32 R/W  0x00000000 Key8 register
    volatile unsigned int KEY9  ;           // 0x64  32 R/W  0x00000000 Key9 register
    volatile unsigned int IV0   ;           // 0x68  32 R/W  0x00000000 Initial vector0 register
    volatile unsigned int IV1   ;           // 0x6C  32 R/W  0x00000000 Initial vector1 register
    volatile unsigned int IV2   ;           // 0x70  32 R/W  0x00000000 Initial vector2 register
    volatile unsigned int IV3   ;           // 0x74  32 R/W  0x00000000 Initial vector3 register
} CIPHER, *PCIPHER;

/************************************************************************
*   11. DMAX Controller (Base Addr = 0xF08A3000) // R/W
*************************************************************************/
//#define HwDMAX_BASE								((volatile unsigned long*)0xF08A3000)





/************************************************************************
*   12. TS-Demux (Base Addr = 0xF08B0000) // R/W
*************************************************************************/
//#define HwTSDEMUX_BASE							((volatile unsigned long*)0xF08B0000)

/************************************************************************
*   13. HSDMA Controller  (Base Addr = 0xF08C0000) // R/W
*************************************************************************/
//#define HwHSDMA_BASE							((volatile unsigned long*)0xF08C0000)

typedef struct _HSDMACTRL{
    volatile unsigned int  ST_SADR0;                // 0x00 R/W 0x00000000 Start Address of Source Block
    volatile unsigned int  SPARAM0;                 // 0x04 R/W 0x00000000 Parameter of Source Block
    volatile unsigned int  NOTDEFINE0;              // 0x08
    volatile unsigned int  C_SADR0;                 // 0x0C R   0x00000000 Current Address of Source Block
    volatile unsigned int  ST_DADR0;                // 0x10 R/W 0x00000000 Start Address of Destination Block
    volatile unsigned int  DPARAM0;                 // 0x14 R/W 0x00000000 Parameter of Destination Block
    volatile unsigned int  NOTDEFINE1;              // 0x18
    volatile unsigned int  C_DADR0;                 // 0x1C R   0x00000000 Current Address of Destination Block
    volatile unsigned int  HCOUNT0;                 // 0x20 R/W 0x00000000 Initial and Current Hop count
    volatile unsigned int  CHCTRL0;                 // 0x24 R/W 0x00000000 Channel Control Register
    volatile unsigned int  RPTCTRL0;                // 0x28 R/W 0x00000000 Repeat Control Register
    volatile unsigned int  EXTREQ0;                 // 0x2C R/W 0x00000000 External DMA Request Register
    volatile unsigned int  ST_SADR1;                // 0x30 R/W 0x00000000 Start Address of Source Block
    volatile unsigned int  SPARAM1;                 // 0x34 R/W 0x00000000 Parameter of Source Block
    volatile unsigned int  NOTDEFINE2;              // 0x38
    volatile unsigned int  C_SADR1;                 // 0x3C R   0x00000000 Current Address of Source Block
    volatile unsigned int  ST_DADR1;                // 0x40 R/W 0x00000000 Start Address of Destination Block
    volatile unsigned int  DPARAM1;                 // 0x44 R/W 0x00000000 Parameter of Destination Block
    volatile unsigned int  NOTDEFINE3;              // 0x48
    volatile unsigned int  C_DADR1;                 // 0x4C R   0x00000000 Current Address of Destination Block
    volatile unsigned int  HCOUNT1;                 // 0x50 R/W 0x00000000 Initial and Current Hop count
    volatile unsigned int  CHCTRL1;                 // 0x54 R/W 0x00000000 Channel Control Register
    volatile unsigned int  RPTCTRL1;                // 0x58 R/W 0x00000000 Repeat Control Register
    volatile unsigned int  EXTREQ1;                 // 0x5C R/W 0x00000000 External DMA Request Register
    volatile unsigned int  ST_SADR2;                // 0x60 R/W 0x00000000 Start Address of Source Block
    volatile unsigned int  SPARAM2;                 // 0x64 R/W 0x00000000 Parameter of Source Block
    volatile unsigned int  NOTDEFINE4;              // 0x68
    volatile unsigned int  C_SADR2;                 // 0x6C R   0x00000000 Current Address of Source Block
    volatile unsigned int  ST_DADR2;                // 0x70 R/W 0x00000000 Start Address of Destination Block
    volatile unsigned int  DPARAM2;                 // 0x74 R/W 0x00000000 Parameter of Destination Block
    volatile unsigned int  NOTDEFINE5;              // 0x78
    volatile unsigned int  C_DADR2;                 // 0x7C R   0x00000000 Current Address of Destination Block
    volatile unsigned int  HCOUNT2;                 // 0x80 R/W 0x00000000 Initial and Current Hop count
    volatile unsigned int  CHCTRL2;                 // 0x84 R/W 0x00000000 Channel Control Register
    volatile unsigned int  RPTCTRL2;                // 0x88 R/W 0x00000000 Repeat Control Register
    volatile unsigned int  EXTREQ2;                 // 0x8C R/W 0x00000000 External DMA Request Register
    volatile unsigned int  CHCONFIG;                // 0x90 R/W 0x00000000 Channel Configuration Register
} HSDMACTRL, *PHSDMACTRL;

typedef struct _HSDMANCTRL{
    volatile unsigned int   ST_SADR;                // 0x000  R/W   Start Address of Source Block
    volatile unsigned int   SPARAM[2];              // 0x004~ R/W   Parameter of Source Block
    volatile unsigned int   C_SADR;                 // 0x00C  R     Current Address of Source Block
    volatile unsigned int   ST_DADR;                // 0x010  R/W   Start Address of Destination Block
    volatile unsigned int   DPARAM[2];              // 0x014~ R/W   Parameter of Destination Block
    volatile unsigned int   C_DADR;                 // 0x01C  R     Current Address of Destination Block
    volatile unsigned int   HCOUNT;                 // 0x020  R/W   Initial and Current Hop count
    volatile unsigned int   CHCTRL;                 // 0x024  R/W   Channel Control Register
    volatile unsigned int   RPTCTRL;                // 0x028  R/W   Repeat Control Register
    volatile unsigned int   EXTREQ;                 // 0x02C  R/W   External DMA Request Register
} HSDMANCTRL, *PHSDMANCTRL;

/************************************************************************
*   14. HSIO BUS Configuration (Base Addr = 0xF08A0000) // R/W
*************************************************************************/
//#define HwHSIOBUSCFG_BASE						((volatile unsigned long*)0xF08A0000)
//#define HwHSIOBUSMATRIX_BASE					((volatile unsigned long*)0xF08A1000)

typedef struct _HSIOBUSCFG{
	//volatile unsigned int SATA_P0CTRL;				//0x00	RW	0x00000000	
	//volatile unsigned int SATA_P1CTRL;				//0x04	RW	0x00000000	
	volatile unsigned int SATA_PCTRL[2];				//0x00	RW	0x00000000	

	volatile unsigned int PCIE_LCTRL;				//0x08	RW	0x00000004
	//volatile unsigned int SATAH0_LCTRL;			//0x0C	RW	0x0000000A	
	//volatile unsigned int SATAHD_LCTRL;			//0x10	RW	0x0000000A	
	volatile unsigned int SATA_LCTRL[2]; 			//0x0C	RW	0x0000000A	

	//volatile unsigned int SATA_P0STS;				//0x14	RW	0x00000000
	//volatile unsigned int SATA_P1STS;				//0x18	RW	0x00000000
	volatile unsigned int SATA_PSTS[2];				//0x14	RW	0x00000000
	
	volatile unsigned int PCIE_LSTS;				//0x1C	RW	0x00000000

	//volatile unsigned int SATAH0_LSTS;				//0x20	RW	0x00000000	
	//volatile unsigned int SATAH1_LSTS;				//0x24	RW	0x00000000
	volatile unsigned int SATAHD_LSTS[2];			//0x20	RW	0x00000000	
	
	volatile unsigned int SATAD_LSTS;				//0x28	RW	0x00000000
	volatile unsigned int SATA_MCTRL;				//0x2C	RW	0x00000000
	volatile unsigned int Reserved0[1];				//0x30	-        -
	volatile unsigned int USB20H_CFG0;				//0x34	RW	0x00000000
	volatile unsigned int USB20H_CFG1;				//0x38	RW	0x00000000
	volatile unsigned int USB20H_CFG2;				//0x3C	RW	0x00000000
	volatile unsigned int USB20H_STS;				//0x40	RW	0x00000000
	volatile unsigned int ETHER_CFG;				//0x44	RW	0x00000000
	volatile unsigned int HCLKMASK;					//0x48	RW	0xffffffff
	volatile unsigned int SWRESET;					//0x4C	RW	0xffffffff
	volatile unsigned int HSIO_CFG;					//0x50	RW	0x03FF0001
	volatile unsigned int HSIO_INTEN;				//0x54	RW	0x00000073
	volatile unsigned int HSIO_INTSTS;				//0x58	RW	0x00000000
	volatile unsigned int HSIO_USERINT;				//0x5C	RW	0x00000000
	volatile unsigned int PCIE_INCTRL0;				//0x60	RW	0x00000000
	volatile unsigned int PCIE_INCTRL1;				//0x64	RW	0x00000000
	volatile unsigned int PCIE_INCTRL2;				//0x68	RW	0x00000000
	volatile unsigned int PCIE_OUTCTRL0;			//0x6C	RW	0x00000000
	volatile unsigned int PCIE_OUTCTRL1;			//0x70	RW	0x00000000
	volatile unsigned int PCIE_OUTCTRL2;			//0x74	RW	0x00000000
	volatile unsigned int PCIE_OUTSTS;				//0x78	RW	0x00000000
	volatile unsigned int PCIE_STSINFO;				//0x7C	RW	0x00000000
	volatile unsigned int PCIE_DBG;					//0x80	RW	0x00000000
} HSIOBUSCFG, *PHSIOBUSCFG;


/*******************************************************************************
*
*    TCC88x DataSheet PART 7 DISPLAY BUS
*
********************************************************************************/

/************************************************************************
*   4. LCD Controller (Base Addr = 0xF0200000) // R/W
*************************************************************************/
//#define HwLCDC_BASE								((volatile unsigned long*)0xF0200000)
//#define HwLCDC0_BASE							((volatile unsigned long*)0xF0200000)
//#define HwLCDLUT0_BASE                          ((volatile unsigned long*)0xF0200400)
//#define HwLCDC1_BASE							((volatile unsigned long*)0xF0204000)
//#define HwLCDLUT0_BASE                          ((volatile unsigned long*)0xF0204400)

typedef struct _LCDC{
    volatile unsigned int   LCTRL;              //  0x000   R/W     0x0080004A  LCD Control Register
    volatile unsigned int   LBC;                //  0x004   R/W     0x00000000  LCD Background Color Register
    volatile unsigned int   LCLKDIV;            //  0x008   R/W     0x00000000  LCD Clock Divider Register
    volatile unsigned int   LHTIME1;            //  0x00C   R/W     0x00000000  LCD Horizontal Timing Register 1
    volatile unsigned int   LHTIME2;            //  0x010   R/W     0x00000000  LCD Horizontal Timing Register 2
    volatile unsigned int   LVTIME1;            //  0x014   R/W     0x00000000  LCD Vertical Timing Register 1
    volatile unsigned int   LVTIME2;            //  0x018   R/W     0x00000000  LCD Vertical Timing Register 2
    volatile unsigned int   LVTIME3;            //  0x01C   R/W     0x00000000  LCD Vertical Timing Register 3
    volatile unsigned int   LVTIME4;            //  0x020   R/W     0x00000000  LCD Vertical Timing Register 4
    volatile unsigned int   LLUTR;              //  0x024   R/W     0x00000000  LCD Lookup Register for Red
    volatile unsigned int   LLUTG;              //  0x028   R/W     0x00000000  LCD Lookup Register for Green
    volatile unsigned int   LLUTB;              //  0x02C   R/W     0x00000000  LCD Lookup Register for Blue
    volatile unsigned int   LDP7L;              //  0x030   R/W     0x6D2B5401  LCD Modulo 7 Dithering Pattern (low) Register
    volatile unsigned int   LDP7H;              //  0x034   R/W     0x0000003F  LCD Modulo 7 Dithering Pattern (high) Register
    volatile unsigned int   LDP5;               //  0x038   R/W     0x1d0b0610  LCD Modulo 5 Dithering Pattern Register
    volatile unsigned int   LDP4;               //  0x03C   R/W     0x00000768  LCD Modulo 4 Dithering Pattern Register
    volatile unsigned int   LDP3;               //  0x040   R/W     0x00000034  LCD Modulo 3 Dithering Pattern Register
    volatile unsigned int   LCP1;               //  0x044   R/W     0x000000FF  LCD Clipping Register1
    volatile unsigned int   LCP2;               //  0x048   R/W     0x000000FF  LCD Clipping Register2
    volatile unsigned int   LDS;                //  0x04C   R/W     0x00000000  LCD Display Size Register
    volatile unsigned int   LSTATUS;            //  0x050   R/CLR   0x00000000  LCD Status Register
    volatile unsigned int   LIM;                //  0x054   R/W     0x0000FFFF  LCD Interrupt Register.
    volatile unsigned int   LGR0;               //  0x058   R/W     0x00000000  LCD Gamma Correction Register 0 for Red Color
    volatile unsigned int   LGR1;               //  0x05C   R/W     0x00000000  LCD Gamma Correction Register 1 for Red Color
    volatile unsigned int   LGG0;               //  0x060   R/W     0x00000000  LCD Gamma Correction Register 0 for Green Color
    volatile unsigned int   LGG1;               //  0x064   R/W     0x00000000  LCD Gamma Correction Register 1 for Green Color
    volatile unsigned int   LGB0;               //  0x068   R/W     0x00000000  LCD Gamma Correction Register 0 for Blue Color
    volatile unsigned int   LGB1;               //  0x06C   R/W     0x00000000  LCD Gamma Correction Register 1 for Blue Color
    volatile unsigned int   LENH;               //  0x070   R/W     0x00000020  LCD Color Enhancement Register
    volatile unsigned int   NOTDEFINE0;         //  0x074   R/W     0x000000C2  Reserved
    volatile unsigned int   DITH0;              //  0x078   R/W     0x00000000  LCD RGB dithering pattern Register
    volatile unsigned int   DITH1;              //  0x07C   R/W     0x00000000  LCD RGB dithering pattern Register
    volatile unsigned int   LI0C;               //  0x080   R/W     0x00000000  LCD Image 0 Control Register
    volatile unsigned int   LI0P;               //  0x084   R/W     0x00000000  LCD Image 0 Position Register
    volatile unsigned int   LI0S;               //  0x088   R/W     0x00000000  LCD Image 0 Size Register
    volatile unsigned int   LI0BA0;             //  0x08C   R/W     0x00000000  LCD Image 0 Base Address 0 Register.
    volatile unsigned int   LI0CA;              //  0x090   R/W     0x00000000  LCD Image 0 Current Address Register.
    volatile unsigned int   LI0BA1;             //  0x094   R/W     0x00000000  LCD Image 0 Base Address 1 Register
    volatile unsigned int   LI0BA2;             //  0x098   R/W     0x00000000  LCD Image 0 Base Address 2 Register
    volatile unsigned int   LI0O;               //  0x09C   R/W     0x00000000  LCD Image 0 Offset Register
    volatile unsigned int   LI0SR;              //  0x0A0   R/W     0x00000000  LCD Image 0 Scale ratio
    volatile unsigned int   LI0A;               //  0x0A4   R/W     0x00000000  LCD Image 0 Alpha Configuration Register
    volatile unsigned int   LI0KR;              //  0x0A8   R/W     0x00000000  LCD Image 0 Keying Register for RED or LUMA(Y)
    volatile unsigned int   LI0KG;              //  0x0AC   R/W     0x00000000  LCD Image 0 Keying Register for BLUE or CHROMA(Cb)
    volatile unsigned int   LI0KB;              //  0x0B0   R/W     0x00000000  LCD Image 0 Keying Register for GREEN or CHROMA(Cr)
    volatile unsigned int   LI0EN;              //  0x0B4   R/W     0x00000020  LCD Image 0 Enhancement Register
    volatile unsigned int   LI1C;               //  0x0B8   R/W     0x00000000  LCD Image 1 Control Register
    volatile unsigned int   LI1P;               //  0x0BC   R/W     0x00000000  LCD Image 1 Position Register
    volatile unsigned int   LI1S;               //  0x0C0   R/W     0x00000000  LCD Image 1 Size Register
    volatile unsigned int   LI1BA0;             //  0x0C4   R/W     0x00000000  LCD Image 1 Base Address 0 Register.
    volatile unsigned int   LI1CA;              //  0x0C8   R/W     0x00000000  LCD Image 1 Current Address Register.
    volatile unsigned int   LI1BA1;             //  0x0CC   R/W     0x00000000  Not Used
    volatile unsigned int   LI1BA2;             //  0x0D0   R/W     0x00000000  Not Used
    volatile unsigned int   LI1O;               //  0x0D4   R/W     0x00000000  LCD Image 1 Offset Register
    volatile unsigned int   LI1SR;              //  0x0D8   R/W     0x00000000  LCD Image 1 Scale ratio-
    volatile unsigned int   LI1A;               //  0x0DC   R/W     0x00000000  LCD Image 1 Alpha Configuration Register
    volatile unsigned int   LI1KR;              //  0x0E0   R/W     0x00000000  LCD Image 1 Keying Register for RED or LUMA(Y)
    volatile unsigned int   LI1KG;              //  0x0E4   R/W     0x00000000  LCD Image 1 Keying Register for BLUE or CHROMA(Cb)
    volatile unsigned int   LI1KB;              //  0x0E8   R/W     0x00000000  LCD Image 1 Keying Register for GREEN or CHROMA(Cr)
    volatile unsigned int   LI1EN;              //  0x0EC   R/W     0x00000020  LCD Image 1 Enhancement Register
    volatile unsigned int   LI2C;               //  0x0F0   R/W     0x00000000  LCD Image 2 Control Register
    volatile unsigned int   LI2P;               //  0x0F4   R/W     0x00000000  LCD Image 2 Position Register
    volatile unsigned int   LI2S;               //  0x0F8   R/W     0x00000000  LCD Image 2 Size Register
    volatile unsigned int   LI2BA0;             //  0x0FC   R/W     0x00000000  LCD Image 2 Base Address 0 Register.
    volatile unsigned int   LI2CA;              //  0x100   R/W     0x00000000  LCD Image 2 Current Address Register.
    volatile unsigned int   LI2BA1;             //  0x104   R/W     0x00000000  Not Used
    volatile unsigned int   LI2BA2;             //  0x108   R/W     0x00000000  Not Used
    volatile unsigned int   LI2O;               //  0x10C   R/W     0x00000000  LCD Image 2 Offset Register
    volatile unsigned int   LI2SR;              //  0x110   R/W     0x00000000  LCD Image 2 Scale ratio
    volatile unsigned int   LI2A;               //  0x114   R/W     0x00000000  LCD Image 2 Alpha Register
    volatile unsigned int   LI2KR;              //  0x118   R/W     0x00000000  LCD Image 2 Keying Register for RED or LUMA(Y)
    volatile unsigned int   LI2KG;              //  0x11C   R/W     0x00000000  LCD Image 2 Keying Register for BLUE or CHROMA(Cb)
    volatile unsigned int   LI2KB;              //  0x120   R/W     0x00000000  LCD Image 2 Keying Register for GREEN or CHROMA(Cr)
    volatile unsigned int   LI2EN;              //  0x124   R/W     0x00000020  LCD Image 2 Enhancement Register

    volatile unsigned int   LI3C;               //  0x128   R/W     0x00000000  LCD Image 3 Control Register
    volatile unsigned int   LI3P;               //  0x12C   R/W     0x00000000  LCD Image 3 Position Register
    volatile unsigned int   LI3S;               //  0x130   R/W     0x00000000  LCD Image 3 Size Register
    volatile unsigned int   LI3BA0;             //  0x134   R/W     0x00000000  LCD Image 3 Base Address 0 Register.
    volatile unsigned int   LI3CA;              //  0x138   R/W     0x00000000  LCD Image 3 Current Address Register.
    volatile unsigned int   LI3BA1;             //  0x13C   R/W     0x00000000  Not Used
    volatile unsigned int   LI3BA2;             //  0x140   R/W     0x00000000  Not Used
    volatile unsigned int   LI3O;               //  0x144   R/W     0x00000000  LCD Image 3 Offset Register
    volatile unsigned int   LI3SR;              //  0x148   R/W     0x00000000  LCD Image 3 Scale ratio
    volatile unsigned int   LI3A;               //  0x14C   R/W     0x00000000  LCD Image 3 Alpha Register
    volatile unsigned int   LI3KR;              //  0x150   R/W     0x00000000  LCD Image 3 Keying Register for RED or LUMA(Y)
    volatile unsigned int   LI3KG;              //  0x154   R/W     0x00000000  LCD Image 3 Keying Register for BLUE or CHROMA(Cb)
    volatile unsigned int   LI3KB;              //  0x158   R/W     0x00000000  LCD Image 3 Keying Register for GREEN or CHROMA(Cr)
    volatile unsigned int   LI3EN;              //  0x15C   R/W     0x00000020  LCD Image 3 Enhancement Register
	
    volatile unsigned int   LUTIDX;             //  0x160   R/W     0x00000070  Lookup Table index Register
} LCDC, *PLCDC;

typedef struct {
	volatile unsigned int	LIxC;				//   LCD Image x Control Register
	volatile unsigned int	LIxP;				//   LCD Image x Position Register
	volatile unsigned int	LIxS;				//   LCD Image x Size Register
	volatile unsigned int	LIxBA0;				//   LCD Image x Base Address 0 Register.
	volatile unsigned int	LIxCA;				//   LCD Image x Current Address Register.
	volatile unsigned int	LIxBA1;				//   LCD Image x Base Address 1 Register
	volatile unsigned int	LIxBA2;				//   LCD Image x Base Address 2 Register
	volatile unsigned int	LIxO;				//   LCD Image x Offset Register
	volatile unsigned int	LIxSR;				//   LCD Image x Scale ratio
	volatile unsigned int	LIxA;				//   LCD Image x Alpha Configuration Register
	volatile unsigned int	LIxKR;				//   LCD Image x Keying Register for RED or LUMA(Y)
	volatile unsigned int	LIxKG;				//   LCD Image x Keying Register for BLUE or CHROMA(Cb)
	volatile unsigned int	LIxKB;				//   LCD Image x Keying Register for GREEN or CHROMA(Cr)
	volatile unsigned int	LIxEN;				//   LCD Image x Enhancement Register
} LCDC_IMG, *PLCDC_IMG;

typedef struct _LCDC_CHANNEL{
	volatile unsigned int	LIC;				//   0x78  R/W  0x00000000  LCD Image 0 Control Register 
	volatile unsigned int	LIP;				//   0x7C  R/W  0x00000000  LCD Image 0 Position Register 
	volatile unsigned int	LIS;				//   0x80  R/W  0x00000000  LCD Image 0 Size Register 
	volatile unsigned int	LIBA0;				//   0x84  R/W  0x00000000  LCD Image 0 Base Address 0 Register. 
	volatile unsigned int	LICA;				//   0x88  R/W  0x00000000  LCD Image 0 Current Address Register. 
	volatile unsigned int	LIBA1;				//   0x8C  R/W  0x00000000  LCD Image 0 Base Address 1 Register 
	volatile unsigned int	LIBA2;				//   0x90  R/W  0x00000000  LCD Image 0 Base Address 2 Register 
	volatile unsigned int	LIO;				//   0x94  R/W  0x00000000  LCD Image 0 Offset Register 
	volatile unsigned int	LISR;				//   0x98  R/W  0x00000000  LCD Image 0 Scale ratio 
	volatile unsigned int	LIA;				//   0x9C  R/W  0x00000000  LCD Image 0 Alpha Configuration Register 
	volatile unsigned int	LIKR;				//   0xA0  R/W  0x00000000  LCD Image 0 Keying Register for RED or LUMA(Y) 
	volatile unsigned int	LIKG;				//   0xA4  R/W  0x00000000  LCD Image 0 Keying Register for BLUE or CHROMA(Cb) 
	volatile unsigned int	LIKB;				//   0xA8  R/W  0x00000000  LCD Image 0 Keying Register for GREEN or CHROMA(Cr) 
	volatile unsigned int	LIEN;				//   0xAC  R/W  0x00000000  LCD Image 0 Enhancement Register 
}LCDC_CHANNEL, *PLCDC_CHANNEL;

typedef struct _LUT_TYPE{
    volatile unsigned char  BCr;    // [7:0]
    volatile unsigned char  GCb;    // [15:8]
    volatile unsigned char  RY;     // [23:16]
    volatile unsigned char  dummy;  // [31:24]]
} LUT_TYPE, *PLUT_TYPE;

typedef struct _LCDLUT{
    volatile LUT_TYPE   LUTDAT[256];
} LCDLUT, *PLCDLUT;

/************************************************************************
*   5. LCD System Interface (Base Addr = ) // R/W
*************************************************************************/
//#define HwLCDSI_BASE							((volatile unsigned long*)0xF020C000)

typedef struct _LCDSI{
    volatile unsigned int   CTRL0;                  //  0x00 R/W 0xA0229011  Control register for LCSN[0] when LXA=0
    volatile unsigned int   CTRL1;                  //  0x04 R/W 0xA0429021  Control register for LCSN[0] when LXA=1
    volatile unsigned int   CTRL2;                  //  0x08 R/W 0xA0129009  Control register for LCSN[0] when LXA=2
    volatile unsigned int   CTRL3;                  //  0x0C R/W 0xA0229011  Control register for LCSN[0] when LXA=3
    volatile unsigned int   CTRL4;                  //  0x10 R/W 0xA0229011  Control register for LCSN[1] when LXA=0
    volatile unsigned int   CTRL5;                  //  0x14 R/W 0xA0429021  Control register for LCSN[1] when LXA=1
    volatile unsigned int   CTRL6;                  //  0x18 R/W 0xA0129009  Control register for LCSN[1] when LXA=2
    volatile unsigned int   CTRL7;                  //  0x1C R/W 0xA0229011  Control register for LCSN[1] when LXA=3

    volatile unsigned int   BSWAP0;                 //  0x20 R/W 0xE4E4E4E4  Byte Swap Control register for LCSN[0] when LXA=0 or LXA=1
    volatile unsigned int   BSWAP1;                 //  0x24 R/W 0xE4E4E4E4  Byte Swap Control register for LCSN[0] when LXA=2 or LXA=3
    volatile unsigned int   BSWAP2;                 //  0x28 R/W 0xE4E4E4E4  Byte Swap Control register for LCSN[1] when LXA=0 or LXA=1
    volatile unsigned int   BSWAP3;                 //  0x2C R/W 0xE4E4E4E4  Byte Swap Control register for LCSN[1] when LXA=2 or LXA=3

    volatile unsigned int   LCTRL;                  //  0x30 R/W 0x00000020  Control register for Writing operation through LCDC
    volatile unsigned int   MODE;                   //  0x34 R/W 0x00000000  68/80 mode select
    volatile unsigned int   STATUS;                 //  0x38 R   -           LCDSI status
    volatile unsigned int   _Dummy[1];              //  -

    volatile unsigned int   DATA0;                  //  0x40 R/W             If this register is read or written, reading or writing operations are generated on LCSN[0] while LXA = 0.
    volatile unsigned int   _Dummy0[(0x10-0x4)/4];  //  -
    volatile unsigned int   DATA1;                  //  0x50 R/W             If this register is read or written, reading or writing operations are generated on LCSN[0] while LXA = 1.
    volatile unsigned int   _Dummy1[(0x10-0x4)/4];  //  -
    volatile unsigned int   DATA2;                  //  0x60 R/W             If this register is read or written, reading or writing operations are generated on LCSN[0] while LXA = 2.
    volatile unsigned int   _Dummy2[(0x10-0x4)/4];  //  -
    volatile unsigned int   DATA3;                  //  0x70 R/W             If this register is read or written, reading or writing operations are generated on LCSN[0] while LXA = 3.
    volatile unsigned int   _Dummy3[(0x10-0x4)/4];  //  -
    volatile unsigned int   DATA4;                  //  0x80 R/W             If this register is read or written, reading or writing operations are generated on LCSN[1] while LXA = 0.
    volatile unsigned int   _Dummy4[(0x10-0x4)/4];  //  -
    volatile unsigned int   DATA5 ;                 //  0x90 R/W             If this register is read or written, reading or writing operations are generated on LCSN[1] while LXA = 1.
    volatile unsigned int   _Dummy5[(0x10-0x4)/4];  //  -
    volatile unsigned int   DATA6 ;                 //  0xA0 R/W             If this register is read or written, reading or writing operations are generated on LCSN[1] while LXA = 2.
    volatile unsigned int   _Dummy6[(0x10-0x4)/4];  //  -
    volatile unsigned int   DATA7 ;                 //  0xB0 R/W             If this register is read or written, reading or writing operations are generated on LCSN[1] while LXA = 3.
} LCDSI, *PLCDSI, sHwLCDSI;

/************************************************************************
*   6. Memory To Memory Scaler (Base Addr = 0xF0210000) // R/W
*************************************************************************/
//#define HwM2MSCALER_BASE						((volatile unsigned long*)0xF0210000)
//#define HwM2MSCALER0_BASE						((volatile unsigned long*)0xF0210000)
//#define HwM2MSCALER1_BASE						((volatile unsigned long*)0xF0220000)

//#define HwSCALERPOSTFILTER_BASE					((volatile unsigned long*)0xF025B000)

typedef struct _M2MSCALER{
    volatile unsigned int   SRCBASEY;               // 0x000 R/W 0x00000000 Scaler source base address for Y
    volatile unsigned int   SRCBASEU;               // 0x004 R/W 0x00000000 Scaler source base address for U (Cb)
    volatile unsigned int   SRCBASEV;               // 0x008 R/W 0x00000000 Scaler source base address for V (Cr)
    volatile unsigned int   SRCSIZE;                // 0x00c R/W 0x00000000 Source image size register
    volatile unsigned int   SRCOFF;                 // 0x010 R/W 0x00000000 Source image line offset register
    volatile unsigned int   SRCCFG;                 // 0x014 R/W 0x00000000 Source image configuration register
    volatile unsigned int   NOTDEFINE0[2];
    volatile unsigned int   DSTBASEY;               // 0x020 R/W 0x00000000 Scaler destination base address for Y
    volatile unsigned int   DSTBASEU;               // 0x024 R/W 0x00000000 Scaler destination base address for U (Cb)
    volatile unsigned int   DSTBASEV;               // 0x028 R/W 0x00000000 Scaler destination base address for V (Cr)
    volatile unsigned int   DSTSIZE;                // 0x02c R/W 0x00000000 Destination image size register
    volatile unsigned int   DSTOFF;                 // 0x030 R/W 0x00000000 Destination image line offset register
    volatile unsigned int   DSTCFG;                 // 0x034 R/W 0x00000000 Destination image configuration register
    volatile unsigned int   NOTDEFINE1[2];
    volatile unsigned int   MSCINF;                 // 0x040 R/W 0x00000000 Scaling information register
    volatile unsigned int   MSCCTR;                 // 0x044 R/W 0x00000000 Scaler control register
    volatile unsigned int   MSCSTR;                 // 0x048 R/W 0x00000000 Scaler status register
    volatile unsigned int   HSTROBE;                // 0x04C R/W 0x000A0002 Horizontal Strobe Timing Control Register
    volatile unsigned int   DSTSLINE;               // 0x050 R/W 0x00000000 Destination Rolling Status Register
    volatile unsigned int   DSTCLINE;               // 0x054 R   0x00000000 Destination Rolling Status Register
    volatile unsigned int   CLIP0;                  // 0x058 R/W 0x00000000 RGB-to-YCbCr Clipping Configuration Register 0
    volatile unsigned int   CLIP1;                  // 0x05C R/W 0x000000FF RGB-to-YCbCr Clipping Configuration Register 1
    volatile unsigned int   VSTROBE;                // 0x060 R/W 0x0000000A Vertical Strobe Timing Control Register
} M2MSCALER, *PM2MSCALER;

typedef struct _M2MSCALERHPF{
    volatile unsigned int   PARAM0;         //  0x40  RW  0x12C83C28  Filter Parameters
    volatile unsigned int   PARAM1;         //  0x44  RW  0x743C0238
    volatile unsigned int   PARAM2;         //  0x48  RW  0xFF408081
    volatile unsigned int   PARAM3;         //  0x4C  RW  0x003801FC
    volatile unsigned int   PARAM4;         //  0x50  RW  0x12641E14
    volatile unsigned int   PARAM5;         //  0x54  RW  0x582D011C
    volatile unsigned int   PARAM6;         //  0x58  RW  0xFF408080
    volatile unsigned int   PARAM7;         //  0x5C  RW  0x002B01FE
    volatile unsigned int   Reserved0[5];   //  0x60  RO  0x00000000  These registers should not be changed.
                                            //  0x64  RW  0x00000000
                                            //  0x68  RO  0x00000000
                                            //  0x6C  RW  0x00000000
                                            //  0x70  RW  0x00000160
    volatile unsigned int   CTRL;           //  0x74  RW  0x00000080  Control register
    volatile unsigned int   SIZE;           //  0x78  RW  0x00000000  Image size of the output image of the scaler
    volatile unsigned int   Reserved1[1];   //  0x7C  RW  0x00040000  This register should not be changed.
} M2MSCALERHPF, *PM2MSCALERHPF;



/************************************************************************
*   7. NTSC/PAL Encoder Composite Output (Base Addr = 0xF0240000) // R/W
*************************************************************************/
//#define HwNTSCPAL_BASE							((volatile unsigned long*)0xF0240000)
//#define HwNTSCPAL_ENCODER_BASE					((volatile unsigned long*)0xF0240000)
//#define HwNTSCPAL_ENCODER_CTRL_BASE				((volatile unsigned long*)0xF0240800)

typedef struct _NTSCPAL {
	volatile unsigned int	STATA;			//0x00
	volatile unsigned int	ECMDA;			//0x04
	volatile unsigned int	ECMDB;			//0x08
	volatile unsigned int	GLK;			//0x0C
	volatile unsigned int	SCH;			//0x10
	volatile unsigned int	HUE;			//0x14
	volatile unsigned int	SAT;			//0x18
	volatile unsigned int	CONT;			//0x1C
	volatile unsigned int	BRIGHT;			//0x20
	volatile unsigned int	FSC_ADJM;		//0x24
	volatile unsigned int	FSC_ADJL;		//0x28
	volatile unsigned int	ECMDC;			//0x2C
	volatile unsigned int	Reserved0[4];	//0x30~0x3F
	volatile unsigned int	DACSEL;			//0x40
	volatile unsigned int	Reserved1[3];	//0x44~0x4F
	volatile unsigned int	DACPD;			//0x50
	volatile unsigned int	Reserved2[11];	//0x54~0x7F
	volatile unsigned int	ICNTL;			//0x80
	volatile unsigned int	HVOFFST;		//0x84
	volatile unsigned int	HOFFST;			//0x88
	volatile unsigned int	VOFFST;			//0x8C
	volatile unsigned int	HSVSO;			//0x90
	volatile unsigned int	HSOE;			//0x94
	volatile unsigned int	HSOB;			//0x98
	volatile unsigned int	VSOB;			//0x9C
	volatile unsigned int	VSOE;			//0xA0
} NTSCPAL, *PNTSCPAL;

typedef struct _NTSCPAL_ENCODER_CTRL {
	volatile unsigned int   VENCON;           //0x00
	volatile unsigned int   VENCIF;           //0x04
//} NTSCPAL_ENCODER_CTRL, *PNTSCPAL_ENCODER_CTRL;
} NTSCPALOP, *PNTSCPALOP;

/************************************************************************
*   8. HDMI (Base Addr = 0xF0254000) // R/W
*************************************************************************/
//#define HwHDMI_BASE								((volatile unsigned long*)0xF0254000)
//#define HwHDMICTRL_BASE							((volatile unsigned long*)0xF0254000)
//#define HwHDMICORE_BASE							((volatile unsigned long*)0xF0255000)
//#define HwHDMIAES_BASE							((volatile unsigned long*)0xF0256000)
//#define HwHDMISPDIF_BASE						((volatile unsigned long*)0xF0257000)
//#define HwHDMII2S_BASE							((volatile unsigned long*)0xF0258000)
//#define HwHDMICEC_BASE							((volatile unsigned long*)0xF0259000)

typedef struct _HDMICTRL{
    volatile unsigned int   INTC_CON;           //  0x0000  R/W  Interrupt Control Register  0x00
    volatile unsigned int   INTC_FLAG;          //  0x0004  R/W  Interrupt Flag Register  0x00
    volatile unsigned int   AESKEY_VALID;       //  0x0008  R  aeskey_valid Register  0x00
    volatile unsigned int   HPD;                //  0x000C  R  HPD signal  0x00
}HDMICTRL, *PHDMICTRL;

typedef struct _HDMICORE{
    volatile unsigned int   HDMI_CON_0;             //  0x0000  R/W  HDMI system control register 0  0x00
    volatile unsigned int   HDMI_CON_1;             //  0x0004  R/W  HDMI system control register 1  0x00
    volatile unsigned int   HDMI_CON_2;             //  0x0008  R/W  HDMI system control register 2  0x00
    volatile unsigned int   NOTDEFINE_;             //  0x000C
    volatile unsigned int   STATUS;                 //  0x0010  R/W  HDMI system status register  0x00
    volatile unsigned int   PHY_STATUS;             //  0x0014  R  PHY status register  0x00
    volatile unsigned int   NOTDEFINE0[2];          //  0x18, 0x1C
    volatile unsigned int   STATUS_EN;              //  0x0020  R/W  HDMI system status enable register  0x00
    volatile unsigned int   NOTDEFINE1[3];          //  0x24, 0x28, 0x2C
    volatile unsigned int   HPD;                    //  0x0030  R/W  HPD control register  0x00
    volatile unsigned int   NOTDEFINE2[3];          //  0x34, 0x38, 0x3C
    volatile unsigned int   MODE_SEL;               //  0x0040  R/W  HDMI/DVI mode selection  0x00
    volatile unsigned int   ENC_EN;                 //  0x0044  R/W  HDCP encryption enable register  0x00
    volatile unsigned int   NOTDEFINE3[2];          //  0x48, 0x4C
//Video Related Registers
    volatile unsigned int   BLUE_SCREEN_0;          //  0x0050  R/W  Pixel values for blue screen  0x00
    volatile unsigned int   BLUE_SCREEN_1;          //  0x0054  R/W  Pixel values for blue screen 0x00
    volatile unsigned int   BLUE_SCREEN_2;          //  0x0058  R/W  Pixel values for blue screen 0x00
    volatile unsigned int   NOTDEFINE4;             //  0x5C
    volatile unsigned int   HDMI_YMAX;              //  0x0060  R/W  Maximum Y (or "R,G,B)" pixel value 0x00
    volatile unsigned int   HDMI_YMIN;              //  0x0064  R/W  Minimum Y (or "R,G,B)" pixel value 0x00
    volatile unsigned int   HDMI_CMAX;              //  0x0068  R/W  Maximum Cb/Cr pixel value  0x00
    volatile unsigned int   HDMI_CMIN;              //  0x006C  R/W  Minimum Cb/Cr pixel value  0x00
    volatile unsigned int   NOTDEFINE5[12];         //  0x70~0x9C
    volatile unsigned int   H_BLANK[2];             //  0x00A0 ,0x00A4 R/W  Horizontal blanking setting  0x00
    volatile unsigned int   NOTDEFINE6[2];          //  0xA8 0xAC
    volatile unsigned int   V_BLANK[3];             //  0x00B0,0x00B4, 0x00B8 R/W  Vertical blanking setting  0x00
    volatile unsigned int   NOTDEFINE7[2];          //  0xB8 0xBC
    volatile unsigned int   H_V_LINE[3];            //  0x00C0,0x00C4, 0x00C8 R/W  Horizontal line & vertical line setting  0x00
    volatile unsigned int   NOTDEFINE8[6];          //  0xCC, 0xD0, 0xD4, 0xD8, 0xDC, 0xE0
    volatile unsigned int   VSYNC_POL;              //  0x00E4  R/W  Vertical sync polarity control register  0x00
    volatile unsigned int   INT_PRO_MODE ;          // 0x00E8  R/W  Interlace/Progressive control register  0x00
    volatile unsigned int   NOTDEFINE9[9];          //  0xEC, 0xF0, 0xF4, 0xF8, 0xFC, 0x100, 0x104, 0x108, 0x10C
    volatile unsigned int   V_BLANK_F[3];           //  0x0110,0x0114, 0x0118 R/W  Vertical blanking setting for bottom field  0x00
    volatile unsigned int   NOTDEFINE10;            //  0x011C
    volatile unsigned int   H_SYNC_GEN[3] ;         // 0x0120,0x0124 ,0x0128 R/W  Horizontal sync generation setting  0x00
    volatile unsigned int   NOTDEFINE11;            //  0x012C
    volatile unsigned int   V_SYNC_GEN1[3];         //  0x0130,0x0134,0x0138  R/W  Vertical sync generation for top field or frame  0x01
    volatile unsigned int   NOTDEFINE12;            //  0x013C
    volatile unsigned int   V_SYNC_GEN2[3];         //  0x0140,0x0144,0x0148  R/W Vertical sync generation for bottom field   - vertical position 0x01
    volatile unsigned int   NOTDEFINE13;            //  0x014C
    volatile unsigned int   V_SYNC_GEN3[3];         //  0x0150,0x0154,0x0158  R/W Vertical sync generation for bottom field   - horizontal position 0x01
    volatile unsigned int   NOTDEFINE14;            //  0x015C
//Audio Related Registers
    volatile unsigned int   ASP_CON;                //  0x0160  R/W  ASP packet control register  0x00
    volatile unsigned int   ASP_SP_FLAT;            //  0x0164  R/W  ASP packet sp_flat bit control  0x00
    volatile unsigned int   NOTDEFINE15[2];         //  0x0168,0x016C
    volatile unsigned int   ASP_CHCFG[4];           //  0x0170,0x0174,0x0178,0x017C  R/W  ASP audio channel configuration  0x04
    volatile unsigned int   ACR_CON;                //  0x0180  R/W  ACR packet control register  0x00
    volatile unsigned int   ACR_MCTS[3];            //  0x0184,0x0188,0x018C  R/W  Measured CTS value  0x01
    volatile unsigned int   ACR_CTS[3];             //  0x0190,0x0194,0x0198  R/W  CTS value for fixed CTS transmission mode.  0xe8
    volatile unsigned int   NOTDEFINE16;            //  0x019C
    volatile unsigned int   ACR_N[3];               //  0x01A0,0x01A4,0x01A8  R/W  N value for ACR packet  0xe8
    volatile unsigned int   NOTDEFINE17;            //  0x01AC
    volatile unsigned int   ACR_LSB2;               //  0x01B0  R/W  Alternate LSB for fixed CTS transmission mode  0x00
    volatile unsigned int   ACR_TXCNT;              //  0x01B4  R/W  Number of ACR packet transmission per frame  0x1f
    volatile unsigned int   ACR_TXINTERVAL;         //  0x01B8  R/W  Interval for ACR packet transmission  0x63
    volatile unsigned int   ACR_CTS_OFFSET;         //  0x01BC  R/W  CTS offset for measured CTS mode  0x00
//Packet Related Registers
    volatile unsigned int   GCP_CON ;               // 0x01C0  R/W  ACR packet control register  0x00
    volatile unsigned int   NOTDEFINE18[3];         //  0x01C4,0x01C8,0x01CC,
    volatile unsigned int   GCP_BYTE[3];            //  0x01D0,0x01D4,0x01D8  R/W  GCP packet body  0x00
    volatile unsigned int   NOTDEFINE19;            //  0x01DC,
    volatile unsigned int   ACP_CON;                //  0x01E0  R/W  ACP packet control register  0x00
    volatile unsigned int   NOTDEFINE20[3];         //  0x01E4,0x01E8,0x01EC,
    volatile unsigned int   ACP_TYPE;               //  0x01F0  R/W  ACP packet header  0x00
    volatile unsigned int   NOTDEFINE21[3];         //  0x01F4,0x01F8,0x01FC,
    volatile unsigned int   ACP_DATA[17];           // 0x0200~0x0240  R/W ACP packet body 0x00
    volatile unsigned int   NOTDEFINE22[3];         //  0x0244,0x0248,0x024C,
    volatile unsigned int   ISRC_CON;               //  0x0250  R/W  ACR packet control register  0x00
    volatile unsigned int   NOTDEFINE23[4];         //  0x0254,0x0258,0x025C,0x0260
    volatile unsigned int   ISRC1_HEADER1;          //  0x0264  R/W  ISCR1 packet header  0x00
    volatile unsigned int   NOTDEFINE24[2];         //  0x0268,0x026C,
    volatile unsigned int   ISRC1_DATA[16];         //  0x0270~0x02AC R/W ISRC1 packet body 0x00
    volatile unsigned int   ISRC2_DATA[16];         //  0x02B0~0x02EC R/W ISRC2 packet body 0x00
    volatile unsigned int   NOTDEFINE25[4];         //  0x02F0,0x02F4,0x02F8,0x2FC

    volatile unsigned int   AVI_CON;                //  0x0300  R/W  AVI packet control register  0x00
    volatile unsigned int   NOTDEFINE26[3];         //  0x0304,0x0308,0x030c
    volatile unsigned int   AVI_CHECK_SUM;          //  0x0310  R/W  AVI packet checksum  0x00
    volatile unsigned int   NOTDEFINE27[3];         //  0x0314,0x0318,0x031c
    volatile unsigned int   AVI_BYTE[13];           //  0x0320~0x0350 R/W AVI packet body 0x00
    volatile unsigned int   NOTDEFINE28[3];         //  0x0354,0x0358,0x035c
    volatile unsigned int   AUI_CON;                //  0x0360  R/W  AUI packet control register  0x00
    volatile unsigned int   NOTDEFINE29[3];         //  0x0364,0x0368,0x036c
    volatile unsigned int   AUI_CHECK_SUM;          //  0x0370  R/W  AUI packet checksum  0x00
    volatile unsigned int   NOTDEFINE30[3];         //  0x0374,0x0378,0x037c
    volatile unsigned int   AUI_BYTE[5];            // 0x0380~0x0390  R/W AUI packet body 0x00
    volatile unsigned int   NOTDEFINE31[3];         //  0x0394,0x0398,0x039c

    volatile unsigned int   MPG_CON;                //  0x03A0  R/W  ACR packet control register  0x00
    volatile unsigned int   NOTDEFINE32[3];         //  0x03A4,0x03A8,0x03Ac
    volatile unsigned int   MPG_CHECK_SUM;          //  0x03B0  R/W  MPG packet checksum  0x00
    volatile unsigned int   NOTDEFINE33[3];         //  0x03B4,0x03B8,0x03Bc

    volatile unsigned int   MPG_BYTE[5];            // 0x03C0~0x03D0  R/W MPG packet body 0x00
    volatile unsigned int   NOTDEFINE34[11];        //  0x03D4,0x03D8,0x03Dc
                                                    //  0x03E0,
                                                    //  0x03F
    volatile unsigned int   SPD_CON;                //  0x0400  R/W  SPD packet control register  0x00
    volatile unsigned int   NOTDEFINE35[3];         //  0x0344,0x0348,0x034c
    volatile unsigned int   SPD_HEADER0;            //  0x0410~  R/W  SPD packet header  0x00
    volatile unsigned int   SPD_HEADER1;            //  0x0414  R/W  SPD packet header  0x00
    volatile unsigned int   SPD_HEADER2;            //  0x0418  R/W  SPD packet header  0x00
    volatile unsigned int   NOTDEFINE36;            //  0x041C
    volatile unsigned int   SPD_DATA[28];           //   0x0420~0x048C R/W SPD packet body 0x00

//HDCP Related Registes
    volatile unsigned int   GAMUT_CON;              //  0x0500  R/W  GAMUT packet control register  0x00
    volatile unsigned int   GAMUT_HEADER0;          //  0x0504  R/W  GAMUT packet header  0x00
    volatile unsigned int   GAMUT_HEADER1;          //  0x0508  R/W  GAMUT packet header  0x00
    volatile unsigned int   GAMUT_HEADER2;          //  0x050C  R/W  GAMUT packet header  0x00
    volatile unsigned int   GAMUT_DATA[28];         // 0x0510~0x057C R/W  GAMUT packet body  0x00
    volatile unsigned int   NOTDEFINE37[16];        //  0x0580~
                                                    //  0x0590,~
                                                    //  0x05A0~
                                                    //  0x05B0~
    volatile unsigned int   DC_CONTROL;             //  0x05C0  R/W  Deep Color Control Register  0x00
    volatile unsigned int   VIDEO_PATTERN_GEN;      //  0x05C4  R/W  Video Pattern Generation Register  0x00
    volatile unsigned int   HPD_GEN ;               // 0x05C8  R/W  HPD Duration value register  0x01
    volatile unsigned int   NOTDEFINE38[113];       //  0x05CC
                                                    //  0x05D0,
                                                    //  0x05E0
                                                    //  0x05F0

    volatile unsigned int   HDCP_SHA1[20];          //  0x0600~0x064C R/W  SHA-1 value from repeater  0x00
    volatile unsigned int   HDCP_KSV_LIST[5];       // 0x0650~0x0660 R/W  KSV list from repeater  0x00

    volatile unsigned int   HDCP_KSV_LIST_CON;      //  0x0664  R/W  KSV list control  0x00
    volatile unsigned int   NOTDEFINE39[2];         //  0x0668,0x066C
    volatile unsigned int   HDCP_SHA_RESULT;        //  0x0670  R/W  SHA-1 checking result register  0x00
    volatile unsigned int   NOTDEFINE40[3];         //  0x0674,0x0678,0x067c
    volatile unsigned int   HDCP_CTRL1;             //  0x0680  R/W  HDCP control register1  0x00
    volatile unsigned int   HDCP_CTRL2;             //  0x0684  R/W  HDCP control register2  0x00
    volatile unsigned int   NOTDEFINE41[2];         //  0x0688,0x068c
    volatile unsigned int   HDCP_CHECK_RESULT;      //  0x0690  R/W  Ri and Pj value checking result  0x00
    volatile unsigned int   NOTDEFINE42[3];         //  0x0394,0x0398,0x039c
    volatile unsigned int   HDCP_BKSV[5];           // 0x06A0~0x06B0 R/W  KSV of Rx  0x00
    volatile unsigned int   NOTDEFINE43[3];         //  0x06B4,0x06B8,0x06Bc

    volatile unsigned int   HDCP_AKSV[5];           //0x06C0~ 0x06D0 R/W  KSV of Tx  0x00
    volatile unsigned int   NOTDEFINE44[3];         //  0x06D4,0x06D8,0x06Dc
    volatile unsigned int   HDCP_An[8];             // 0x06E0~ 0x06FC R/W  An value  0x00
    volatile unsigned int   HDCP_BCAPS;             //  0x0700  R/W  BCAPS from Rx  0x00
    volatile unsigned int   NOTDEFINE45[3];         //  0x0704,0x0708,0x070c
    volatile unsigned int   HDCP_BSTATUS_0;         //  0x0710  R/W  BSTATUS from Rx  0x00
    volatile unsigned int   HDCP_BSTATUS_1;         //  0x0714  R/W  BSTATUS from Rx  0x00
    volatile unsigned int   NOTDEFINE46[10];        //  0x0718,0x071c
                                                    //  0x0720,
                                                    //  0x0730
    volatile unsigned int   HDCP_Ri_0;              //  0x0740  R/W  Ri value of Tx  0x00
    volatile unsigned int   HDCP_Ri_1;              //  0x0744  R/W  Ri value of Tx  0x00
    volatile unsigned int   NOTDEFINE47[13];        //  0x0748
                                                    //  0x0750,
                                                    //  0x0760,
                                                    //  0x0770
    volatile unsigned int   HDCP_I2C_INT;           //  0x0780  R/W  I2C interrupt flag  0x00
    volatile unsigned int   NOTDEFINE48[3];         //  0x0784,0x0788,0x078c

    volatile unsigned int   HDCP_AN_INT;            //  0x0790  R/W  An value ready interrupt flag  0x00
    volatile unsigned int   NOTDEFINE49[3];         //  0x0794,0x0798,0x079c

    volatile unsigned int   HDCP_WATCGDOG_INT;      //  0x07A0  R/W  Wachdog interrupt flag  0x00
    volatile unsigned int   NOTDEFINE50[3];         //  0x07A4,0x07A8,0x07Ac

    volatile unsigned int   HDCP_Ri_INT;            //  0x07B0  R/W  Ri value update interrupt flag  0x00
    volatile unsigned int   NOTDEFINE51[7];         //  0x07B4,0x07B8,0x07Bc
                                                    //  0x07C0,
    volatile unsigned int   HDCP_Ri_Compare_0;      //  0x07D0  R/W  HDCP Ri Interrupt Frame number index register 0  0x80
    volatile unsigned int   HDCP_Ri_Compare_1;      //  0x07D4  R/W  HDCP Ri Interrupt Frame number index register 1  0x7f
    volatile unsigned int   NOTDEFINE52[2];         //  0x07D8,0x07Dc

    volatile unsigned int   HDCP_Frame_Count;       // 0x07E0  R  Current value of the frame count index in the hardware 0x00
}HDMICORE, *PHDMICORE;

typedef struct _HDMIAES{
    volatile unsigned int   AES_START;              //   0x0000  R/W  AES_START  0x00
    volatile unsigned int   NOTDEFINE0[7];          //  0x0004,0x0008,0x000c
                                                    //  0x0010,
    volatile unsigned int   AES_DATA_SIZE_L;        //   0x0020  R/W  AES_DATA_SIZE_L  0x20
    volatile unsigned int   AES_DATA_SIZE_H;        //   0x0024  R/W  AES_DATA_SIZE_H  0x01
    volatile unsigned int   NOTDEFINE1[6];          //  0x0028,0x002c
                                                    //  0x0030
    volatile unsigned int   AES_DATA;               //   0x0040  W  AES_DATA  -
}HDMIAES, *PHDMIAES;

typedef struct _HDMISPDIF{
    volatile unsigned int   SPDIFIN_CLK_CTRL;           //  0x0000  R/W  SPDIFIN Clock Control Register  0x02
    volatile unsigned int   SPDIFIN_OP_CTRL;            //  0x0004  R/W  SPDIFIN Operation Control Register 1  0x00
    volatile unsigned int   SPDIFIN_IRQ_MASK;           //  0x0008  R/W  SPDIFIN Interrupt Request Mask Register  0x00
    volatile unsigned int   SPDIFIN_IRQ_STATUS;         //  0x000C  R/W  SPDIFIN Interrupt Request Status Register  0x00
    volatile unsigned int   SPDIFIN_CONFIG_1;           //  0x0010  R/W  SPDIFIN Configuration Register 1  0x02
    volatile unsigned int   SPDIFIN_CONFIG_2;           //  0x0014  R/W  SPDIFIN Configuration Register 2  0x00
    volatile unsigned int   NOTDEFINE0[2];              //  0x0018 0x001C -  Reserved  -
    volatile unsigned int   SPDIFIN_USER_VALUE_1;       //  0x0020  R  SPDIFIN User Value Register 1  0x00
    volatile unsigned int   SPDIFIN_USER_VALUE_2;       //  0x0024  R  SPDIFIN User Value Register 2  0x00
    volatile unsigned int   SPDIFIN_USER_VALUE_3;       //  0x0028  R  SPDIFIN User Value Register 3  0x00
    volatile unsigned int   SPDIFIN_USER_VALUE_4;       //  0x002C  R  SPDIFIN User Value Register 4  0x00
    volatile unsigned int   SPDIFIN_CH_STATUS_0_1;      //  0x0030  R  SPDIFIN Channel Status Register 0-1  0x00
    volatile unsigned int   SPDIFIN_CH_STATUS_0_2;      //  0x0034  R  SPDIFIN Channel Status Register 0-2  0x00
    volatile unsigned int   SPDIFIN_CH_STATUS_0_3;      //  0x0038  R  SPDIFIN Channel Status Register 0-3  0x00
    volatile unsigned int   SPDIFIN_CH_STATUS_0_4;      //  0x003C  R  SPDIFIN Channel Status Register 0-4  0x00
    volatile unsigned int   SPDIFIN_CH_STATUS_1;        //  0x0040  R  SPDIFIN Channel Status Register 1  0x00
    volatile unsigned int   NOTDEFINE1;                 //  0x0044 -  Reserved  -
    volatile unsigned int   SPDIFIN_FRAME_PERIOD_1;     //  0x0048  R  SPDIFIN Frame Period Register 1  0x00
    volatile unsigned int   SPDIFIN_FRAME_PERIOD_2;     //  0x004C  R  SPDIFIN Frame Period Register 2  0x00
    volatile unsigned int   SPDIFIN_Pc_INFO_1;          //  0x0050  R  SPDIFIN Pc Info Register 1  0x00
    volatile unsigned int   SPDIFIN_Pc_INFO_2;          //  0x0054  R  SPDIFIN Pc Info Register 2  0x00
    volatile unsigned int   SPDIFIN_Pd_INFO_1;          //  0x0058  R  SPDIFIN Pd Info Register 1  0x00
    volatile unsigned int   SPDIFIN_Pd_INFO_2;          //  0x005C  R  SPDIFIN Pd Info Register 2  0x00
    volatile unsigned int   SPDIFIN_DATA_BUF_0_1;       //  0x0060  R  SPDIFIN Data Buffer Register 0_1  0x00
    volatile unsigned int   SPDIFIN_DATA_BUF_0_2;       //  0x0064  R  SPDIFIN Data Buffer Register 0_2  0x00
    volatile unsigned int   SPDIFIN_DATA_BUF_0_3;       //  0x0068  R  SPDIFIN Data Buffer Register 0_3  0x00
    volatile unsigned int   SPDIFIN_USER_BUF_0;         //  0x006C  R  SPDIFIN User Buffer Register 0  0x00
    volatile unsigned int   SPDIFIN_DATA_BUF_1_1;       //  0x0070  R  SPDIFIN Data Buffer Register 1_1  0x00
    volatile unsigned int   SPDIFIN_DATA_BUF_1_2;       //  0x0074  R  SPDIFIN Data Buffer Register 1_2  0x00
    volatile unsigned int   SPDIFIN_DATA_BUF_1_3;       //  0x0078  R  SPDIFIN Data Buffer Register 1_3  0x00
    volatile unsigned int   SPDIFIN_USER_BUF_1;         //  0x007C  R  SPDIFIN User Buffer Register 1  0x00
}HDMISPDIF, *PHDMISPDIF;

typedef struct _HDMII2S{
    volatile unsigned int   I2S_CLK_CON;                //  0x0000  R/W  I2S Clock Enable Register  0x00
    volatile unsigned int   I2S_CON_1;                  //  0x0004  R/W  I2S Control Register 1  0x00
    volatile unsigned int   I2S_CON_2;                  //  0x0008  R/W  I2S Control Register 2  0x00
    volatile unsigned int   I2S_PIN_SEL_0;              //  0x000C  R/W  I2S Input Pin Selection Register 0  0x77
    volatile unsigned int   I2S_PIN_SEL_1;              //  0x0010  R/W  I2S Input Pin Selection Register 1  0x77
    volatile unsigned int   I2S_PIN_SEL_2;              //  0x0014  R/W  I2S Input Pin Selection Register 2  0x77
    volatile unsigned int   I2S_PIN_SEL_3;              //  0x0018  R/W  I2S Input Pin Selection Register 3  0x07
    volatile unsigned int   I2S_DSD_CON;                //  0x001C  R/W  I2S DSD Control Register  0x02
    volatile unsigned int   I2S_MUX_CON;                //  0x0020  R/W  I2S In/Mux Control Register  0x60
    volatile unsigned int   I2S_CH_ST_CON;              //  0x0024  R/W  I2S Channel Status Control Register  0x00
    volatile unsigned int   I2S_CH_ST_0;                //  0x0028  R/W  I2S Channel Status Block 0  0x00
    volatile unsigned int   I2S_CH_ST_1;                //  0x002C  R/W  I2S Channel Status Block 1  0x00
    volatile unsigned int   I2S_CH_ST_2;                //  0x0030  R/W  I2S Channel Status Block 2  0x00
    volatile unsigned int   I2S_CH_ST_3;                //  0x0034  R/W  I2S Channel Status Block 3  0x00
    volatile unsigned int   I2S_CH_ST_4;                //  0x0038  R/W  I2S Channel Status Block 4  0x00
    volatile unsigned int   I2S_CH_ST_SH_0;             //  0x003C  R  I2S Channel Status Block Shadow Register 0  0x00
    volatile unsigned int   I2S_CH_ST_SH_1;             //  0x0040  R  I2S Channel Status Block Shadow Register 1  0x00
    volatile unsigned int   I2S_CH_ST_SH_2;             //  0x0044  R  I2S Channel Status Block Shadow Register 2  0x00
    volatile unsigned int   I2S_CH_ST_SH_3;             //  0x0048  R  I2S Channel Status Block Shadow Register 3  0x00
    volatile unsigned int   I2S_CH_ST_SH_4;             //  0x004C  R  I2S Channel Status Block Shadow Register 4  0x00
    volatile unsigned int   I2S_VD_DATA;                //  0x0050  R/W  I2S Audio Sample Validity Register  0x00
    volatile unsigned int   I2S_MUX_CH;                 //  0x0054  R/W  I2S Channel Enable Register  0x03
    volatile unsigned int   I2S_MUX_CUV;                //  0x0058  R/W  I2S CUV Enable Register  0x03
    volatile unsigned int   I2S_IRQ_MASK;               //  0x005C  R/W  I2S Interrupt Request Mask Register  0x03
    volatile unsigned int   I2S_IRQ_STATUS;             //  0x0060  R/W  I2S Interrupt Request Status Register  0x00
    volatile unsigned int   I2S_CH0_L_0;                //  0x0064  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH0_L_1;                //  0x0068  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH0_L_2;                //  0x006C  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH0_L_3;                //  0x0070  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH0_R_0;                //  0x0074  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH0_R_1;                //  0x0078  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH0_R_2;                //  0x007C  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH0_R_3;                //  0x0080  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_L_0;                //  0x0084  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_L_1;                //  0x0088  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_L_2;                //  0x008C  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_L_3;                //  0x0090  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_R_0;                //  0x0094  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_R_1;                //  0x0098  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_R_2;                //  0x009C  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH1_R_3;                //  0x00A0  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH2_L_0;                //  0x00A4  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH2_L_1;                //  0x00A8  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH2_L_2;                //  0x00AC  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH2_L_3;                //  0x00B0  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH2_R_0;                //  0x00B4  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH2_R_1;                //  0x00B8  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH2_R_2;                //  0x00BC  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_Ch2_R_3;                //  0x00C0  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH3_L_0;                //  0x00C4  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH3_L_1;                //  0x00C8  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH3_L_2;                //  0x00CC  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH3_R_0;                //  0x00D0  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH3_R_1;                //  0x00D4  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CH3_R_2;                //  0x00D8  R  I2S PCM Output Data Register  0x00
    volatile unsigned int   I2S_CUV_L_R;                //  0x00DC  R  I2S CUV Output Data Register  0x00
}HDMII2S, *PHDMII2S;

typedef struct _HDMICEC{
    volatile unsigned int   CEC_TX_STATUS_0;            //  0x0000  R  CEC Tx status register 0.  0x00
    volatile unsigned int   CEC_TX_STATUS_1;            //  0x0004  R  CEC Tx status register 1. Number of blocks transferred. 0x00
    volatile unsigned int   CEC_RX_STATUS_0;            //  0x0008  R  CEC Rx status register 0.  0x00
    volatile unsigned int   CEC_RX_STATUS_1;            //  0x000C  R  CEC Rx status register 1. Number of blocks received. 0x00
    volatile unsigned int   CEC_INTR_MASK;              //  0x0010  R/W  CEC interrupt mask register  0x00
    volatile unsigned int   CEC_INTR_CLEAR;             //  0x0014  R/W  CEC interrupt clear register  0x00
    volatile unsigned int   NOTDEFINE0[2];              //  0x0018 0x001C -  Reserved  -
    volatile unsigned int   CEC_LOGIC_ADDR;             //  0x0020  R/W  HDMI Tx logical address register  0x0F
    volatile unsigned int   NOTDEFINE1[3];              //  0x0024 0x0028, 0x002C -  Reserved  -
    volatile unsigned int   CEC_DIVISOR_0;              //  0x0030  R/W  Clock divisor for 0.05ms period count ([7:0] of 32-bit) 0x00
    volatile unsigned int   CEC_DIVISOR_1;              //  0x0034  R/W  Clock divisor for 0.05ms period count ([15:8] of 32-bit) 0x00
    volatile unsigned int   CEC_DIVISOR_2;              //  0x0038  R/W  Clock divisor for 0.05ms period count ([23:16] of 32-bit) 0x00
    volatile unsigned int   CEC_DIVISOR_3;              //  0x003C  R/W  Clock divisor for 0.05ms period count ([31:24] of 32-bit) 0x00
//CEC Tx related Registers
    volatile unsigned int   CEC_TX_CTRL;                //  0x0040  R/W  CEC Tx control register  0x10
    volatile unsigned int   CEC_TX_BYTE_NUM;            //  0x0044  R/W  Number of blocks in a message to be transferred  0x00
    volatile unsigned int   NOTDEFINE2[6];              //  0x0048 0x004C -  Reserved  -
                                                        //  0x0050
    volatile unsigned int   CEC_TX_STATUS_2;            //  0x0060  R  CEC Tx status register 2  0x00
    volatile unsigned int   CEC_TX_STATUS_3;            //  0x0064  R  CEC Tx status register 3  0x00
    volatile unsigned int   NOTDEFINE3[6];              //  0x0068 0x006C -  Reserved  -
                                                        //  0x0070
    volatile unsigned int   NOTDEFINE4[3];              //  0x0074 0x0078, 0x007C -  Reserved  -
    volatile unsigned int   CEC_TX_BUFFER[16];          // 0x0080 ~ 0x00BC R/W  Byte #0 ~ #15 of CEC message to be transferred. (#0 is transferred 1st ) 0x00
//CEC Rx related Registers
    volatile unsigned int   CEC_RX_CTRL;                //  0x00C0  R/W  CEC Rx control register  0x00
    volatile unsigned int   NOTDEFINE5[7];              //  0x00C4 0x00C8, 0x00CC -  Reserved  -
                                                        //  0x00D0
    volatile unsigned int   CEC_RX_STATUS_2;            //  0x00E0  R  CEC Rx status register 2  0x00
    volatile unsigned int   CEC_RX_STATUS_3;            //  0x00E4  R  CEC Rx status register 3eived 1st )  0x00
    volatile unsigned int   NOTDEFINE6[2];              //  0x00E8, 0x00EC -  Reserved  -
    volatile unsigned int   CEC_RX_BUFFER[16];          // 0x0100 ~ 0x013C R  Byte #0 ~ #15 of CEC message received (#0 is received 1st ) 0x00
}HDMICEC, *PHDMICEC;

/************************************************************************
*   9. Video and Image Quality Enhancer(VIQE) (Base Addr = ) // R/W
*************************************************************************/
//#define HwVIQE_BASE								((volatile unsigned long*)0xF0252000)

typedef struct _VIQE{
    volatile unsigned int CTRL;                 // 0x000    R/W     0x00000000    VIQE General Control Register
    volatile unsigned int SIZE;                 // 0x004    R/W     0x00000000    VIQE SIZE Register
    volatile unsigned int TIMEGEN;              // 0x008    R/W     0x00000000    VIQE Time Generator Register
    volatile unsigned int LUMADLY;              // 0x00C    R/W     0x00000000    VIQE Luma Delay Register
    volatile unsigned int IMGCONF;              // 0x010    R/W     0x00000000    VIQE Image Configuration Register
    volatile unsigned int IMGFMT;               // 0x014    R/W     0x00000000    VIQE Image Format Register
    volatile unsigned int MISCC;                // 0x018    R/W     0x00000000    VIQE Misc, Control Register
    volatile unsigned int FRMC;                 // 0x01C    R/W     0x00000000    VIQE Frame Control Register
    volatile unsigned int INT;                  // 0x020    R/W     0x00000000    VIQE Interrupt Register
    volatile unsigned int INTMASK;              // 0x024    R/W     0x00000000    VIQE Interrupt Mask Register
    volatile unsigned int _NOTDEFINE0[5];
    volatile unsigned int VERSION;              // 0x03c    R       0x4d2b3401    VIQE Version Register
    volatile unsigned int _NOTDEFINE1[16];
    volatile unsigned int DI_CTRL;              // 0x080    R/W     0x00000000    De-interlacer Control Register
    volatile unsigned int DI_ENGINE0;           // 0x084    R/W     0x00000000    De-interlacer Engine 0 Register
    volatile unsigned int DI_ENGINE1;           // 0x088    R/W     0x00000000    De-interlacer Engine 1 Register
    volatile unsigned int DI_ENGINE2;           // 0x08C    R/W     0x00000000    De-interlacer Engine 2 Register
    volatile unsigned int DI_ENGINE3;           // 0x090    R/W     0x00000000    De-interlacer Engine 3 Register
    volatile unsigned int DI_ENGINE4;           // 0x094    R/W     0x00000000    De-interlacer Engine 4 Register
    volatile unsigned int PD_THRES0;            // 0x098    R/W     0x00000000    De-interlacer Pulldown Threshold 0 Register
    volatile unsigned int PD_THRES1;            // 0x09C    R/W     0x00000000    De-interlacer Pulldown Threshold 1 Register
    volatile unsigned int PD_JUDDER;            // 0x0A0    R/W     0x00000000    De-interlacer Pulldown Judder Register
    volatile unsigned int PD_JUDDER_M;          // 0x0A4    R/W     0x00000000    De-interlacer Pulldown Judder Misc. Register
    volatile unsigned int DI_MISCC;             // 0x0A8    R/W     0x00000000    De-interlacer Misc. Control Register
    volatile unsigned int DI_STATUS;            // 0x0AC    R                     De-interlacer Status Register
    volatile unsigned int PD_STATUS;            // 0x0B0    R                     De-interlacer Pulldown Status Register
    volatile unsigned int DI_REGION0;           // 0x0B4    R/W     0x00000000    De-interlacer Region 0 Register
    volatile unsigned int DI_REGION1;           // 0x0B8    R/W     0x00000000    De-interlacer Region 1 Register
    volatile unsigned int DI_INT;               // 0x0BC    R/W     0x00000000    De-interlacer Interrupt Register
    volatile unsigned int _NOTDEFINE2[16];
    volatile unsigned int DN_C_H_Y0;            // 0x100    R/W     0xbfffffa4    Denoise-3D horizontal coefficient #0 in luminance
    volatile unsigned int DN_C_H_Y1;            // 0x104    R/W     0x15556aaa    Denoise-3D horizontal coefficient #1 in luminance
    volatile unsigned int DN_C_V_Y0;            // 0x108    R/W     0xaaaaaaa4    Denoise-3D vertical coefficient #0 in luminance
    volatile unsigned int DN_C_V_Y1;            // 0x10C    R/W     0x15556aaa    Denoise-3D vertical coefficient #1 in luminance
    volatile unsigned int DN_C_T_Y0;            // 0x110    R/W     0xaaaaaaa4    Denoise-3D temporal coefficient #0 in luminance
    volatile unsigned int DN_C_T_Y1;            // 0x114    R/W     0x15556aaa    Denoise-3D temporal coefficient #1 in luminance
    volatile unsigned int DN_C_H_C0;            // 0x118    R/W     0xbfffffa4    Denoise-3D horizontal coefficient #0 in chrominance
    volatile unsigned int DN_C_H_C1;            // 0x11C    R/W     0x15556aaa    Denoise-3D horizontal coefficient #1 in chrominance
    volatile unsigned int DN_C_V_C0;            // 0x120    R/W     0xaaaaaaa4    Denoise-3D vertical coefficient #0 in chrominance
    volatile unsigned int DN_C_V_C1;            // 0x124    R/W     0x15556aaa    Denoise-3D vertical coefficient #1 in chrominance
    volatile unsigned int DN_C_T_C0;            // 0x128    R/W     0xaaaaaaa4    Denoise-3D temporal coefficient #0 in chrominance
    volatile unsigned int DN_C_T_C1;            // 0x12C    R/W     0x15556aaa    Denoise-3D temporal coefficient #1 in chrominance
    volatile unsigned int DN_STATE0_3D;         // 0x130    R/W     0x00000000    Denoise-3D count states and int. mask
    volatile unsigned int DN_STATE1_3D;         // 0x134    R       -             Denoise-3D count states
    volatile unsigned int DN_DIV_IMG_3D;        // 0x138    R/W     0x00000168    Denoise-3D image divide
    volatile unsigned int _NOTDEFINE3[1];
    volatile unsigned int DN_C_2D_Y0;           // 0x140    R/W     0x12320e0a    Denoise-2D coefficient #0 in luminance
    volatile unsigned int DN_C_2D_Y1;           // 0x144    R/W     0x373c051d    Denoise-2D coefficient #1 in luminance
    volatile unsigned int DN_C_2D_Y2;           // 0x148    R/W     0x4a0640ff    Denoise-2D coefficient #2 in luminance
    volatile unsigned int DN_C_2D_Y3;           // 0x14C    R/W     0x003100fb    Denoise-2D coefficient #3 in luminance
    volatile unsigned int DN_C_2D_C0;           // 0x150    R/W     0x12190805    Denoise-2D coefficient #0 in chrominance
    volatile unsigned int DN_C_2D_C1;           // 0x154    R/W     0x373c0507    Denoise-2D coefficient #1 in chrominance
    volatile unsigned int DN_C_2D_C2;           // 0x158    R/W     0x4a0640ff    Denoise-2D coefficient #2 in chrominance
    volatile unsigned int DN_C_2D_C3;           // 0x15C    R/W     0x003100fb    Denoise-2D coefficient #3 in chrominance
    volatile unsigned int DN_FIFOSTATE;         // 0x160    R/W     0x00000000    FIFO states
    volatile unsigned int DN_STATE0_2D;         // 0x164    R/W     0x00000000    Denoise-2D count states and int.mask
    volatile unsigned int DN_STATE1_2D;         // 0x168    R       -             Denoise-2D count states
    volatile unsigned int DN_CTRL;              // 0x16C    R/W     0x00000000    Denoise FIFO and coefficient ctrl
    volatile unsigned int DN_DIV_IMG_2D;        // 0x170    R/W     0x00000168    Denoise-2D image divide
    volatile unsigned int _NOTDEFINE4[3];
    volatile unsigned int RD_IMG0_BASE0;        // 0x180    R/W     0x00000000    RDMA image #0 base address in Y channel
    volatile unsigned int RD_IMG0_BASE1;        // 0x184    R/W     0x00000000    RDMA image #0 base address in U channel
    volatile unsigned int RD_IMG0_BASE2;        // 0x188    R/W     0x00000000    RDMA image #0 base address in V channel
    volatile unsigned int RD_IMG0_OFS;          // 0x18C    R/W     0x00000000    RDMA image #0 address offset
    volatile unsigned int RD_IMG1_BASE0;        // 0x190    R/W     0x00000000    RDMA image #1 base address in Y channel
    volatile unsigned int RD_IMG1_BASE1;        // 0x194    R/W     0x00000000    RDMA image #1 base address in U channel
    volatile unsigned int RD_IMG1_BASE2;        // 0x198    R/W     0x00000000    RDMA image #1 base address in V channel
    volatile unsigned int RD_IMG1_OFS;          // 0x19C    R/W     0x00000000    RDMA image #1 address offset
    volatile unsigned int RD_IMG2_BASE0_0;      // 0x1A0    R/W     0x00000000    RDMA Comp. data #0 base address in Y channel
    volatile unsigned int RD_IMG2_BASE1_0;      // 0x1A4    R/W     0x00000000    RDMA Comp. data #0 base address in U channel
    volatile unsigned int RD_IMG2_BASE2_0;      // 0x1A8    R/W     0x00000000    RDMA Comp. data #0 base address in V channel
    volatile unsigned int RD_IMG2_BASE0_1;      // 0x1AC    R/W     0x00000000    RDMA Comp. data #1 base address in Y channel
    volatile unsigned int RD_IMG2_BASE1_1;      // 0x1B0    R/W     0x00000000    RDMA Comp. data #1 base address in U channel
    volatile unsigned int RD_IMG2_BASE2_1;      // 0x1B4    R/W     0x00000000    RDMA Comp. data #1 base address in V channel
    volatile unsigned int RD_CUR_ADDR0;         // 0x1B8    R       -             RDMA image #0 current address
    volatile unsigned int RD_CUR_ADDR1;         // 0x1BC    R       -             RDMA image #1 current address
    volatile unsigned int RD_CUR_ADDR2;         // 0x1C0    R       -             RDMA Huffman data current address
    volatile unsigned int RD_FIFOSTATE;         // 0x1C4    R/W     0x00000000    RDMA FIFO States
    volatile unsigned int RD_LINE_STATE0;       // 0x1C8    R       -             RDMA count states #0
    volatile unsigned int RD_LINE_STATE1;       // 0x1CC    R/W     0x00000000    RDMA count states #1
    volatile unsigned int RD_CTRL;              // 0x1D0    R/W     0x00000000    RDMA control register
    volatile unsigned int RD_HUFF_PL0;          // 0x1D4    R/W     0x00000000    RDMA Comp. data number in Y channel
    volatile unsigned int RD_HUFF_PL1;          // 0x1D8    R/W     0x00000000    RDMA Comp. data number in C channel
    volatile unsigned int _NOTDEFINE5[1];       // 0x1DC    -       -             -
    volatile unsigned int RD_IMG2_ENDADR0;      // 0x1E0    R/W     0x00000000    RDMA Comp. data end address in Y channel
    volatile unsigned int RD_IMG2_ENDADR1;      // 0x1E4    R/W     0x00000000    RDMA Comp. data end address in U channel
    volatile unsigned int RD_IMG2_ENDADR2;      // 0x1E8    R/W     0x00000000    RDMA Comp. data end address in V channel
    volatile unsigned int RD_ROLL_CTRL;         // 0x1EC    R/W     0x00000000    RDMA Comp. DMA rolling-mode control register
    volatile unsigned int RD_IMG2_CURADR0;      // 0x1E0    R/W     0x00000000    RDMA Comp. current address in Y channel
    volatile unsigned int RD_IMG2_CURADR1;      // 0x1E4    R/W     0x00000000    RDMA Comp. current address in U channel
    volatile unsigned int RD_IMG2_CURADR2;      // 0x1E8    R/W     0x00000000    RDMA Comp. current address in V channel
    volatile unsigned int _NOTDEFINE6[5];       //
    volatile unsigned int WD_BASE0_0;           // 0x200    R/W     0x00000000    Comp. DMA #0 base address in Y channel
    volatile unsigned int WD_BASE1_0;           // 0x204    R/W     0x00000000    Comp. DMA #0 base address in U channel
    volatile unsigned int WD_BASE2_0;           // 0x208    R/W     0x00000000    Comp. DMA #0 base address in V channel
    volatile unsigned int WD_BASE0_1;           // 0x20C    R/W     0x00000000    Comp. DMA #1 base address in Y channel
    volatile unsigned int WD_BASE1_1;           // 0x210    R/W     0x00000000    Comp. DMA #1 base address in U channel
    volatile unsigned int WD_BASE2_1;           // 0x214    R/W     0x00000000    Comp. DMA #1 base address in V channel
    volatile unsigned int WD_CUR_ADDR;          // 0x218    R       -             Comp. DMA current address
    volatile unsigned int WD_STATE;             // 0x21C    R/W     0x00000000    Comp. DMA states
    volatile unsigned int WD_CTRL;              // 0x220    R/W     0x00000000    Comp. DMA control register
    volatile unsigned int _NOTDEFINE7[3];       //
    volatile unsigned int WD_HUFF_CNT0;         // 0x230    R       -             Comp. DMA encoded data count in Y channel
    volatile unsigned int WD_HUFF_CNT1;         // 0x234    R       -             Comp. DMA encoded data count in U channel
    volatile unsigned int WD_HUFF_CNT2;         // 0x238    R                     Comp. DMA encoded data count in V channel
    volatile unsigned int _NOTDEFINE8[1];       // 0x23C    -       -             -
    volatile unsigned int WD_ENDADR0;           // 0x240    R/W     0x00000000    Comp. DMA end-address in Y channel
    volatile unsigned int WD_ENDADR1;           // 0x244    R/W     0x00000000    Comp. DMA end-address in U channel
    volatile unsigned int WD_ENDADR2;           // 0x248    R/W     0x00000000    Comp. DMA end-address in V channel
    volatile unsigned int WD_ROLL_CTRL;         // 0x24C    R/W     0x00000000    Comp. DMA rolling-mode control register
    volatile unsigned int WD_CUR_ADR0;          // 0x240    R/W     0x00000000    Comp. DMA current address in Y channel
    volatile unsigned int WD_CUR_ADR1;          // 0x244    R/W     0x00000000    Comp. DMA current address in U channel
    volatile unsigned int WD_CUR_ADR2;          // 0x248    R/W     0x00000000    Comp. DMA current address in V channel
    volatile unsigned int _NOTDEFINE9[13];      //
    volatile unsigned int OD_BASE0;             // 0x280    R/W     0x00000000    ODMA base address in Y channel
    volatile unsigned int OD_BASE1;             // 0x284    R/W     0x00000000    ODMA base address in U channel
    volatile unsigned int OD_BASE2;             // 0x288    R/W     0x00000000    ODMA base address in V channel
    volatile unsigned int OD_SIZE;              // 0x28C    R/W     0x00000000    ODMA image size
    volatile unsigned int OD_OFS;               // 0x290    R/W     0x00000000    ODMA address offset
    volatile unsigned int OD_CFG;               // 0x294    R/W     0x00000000    ODMA image type
    volatile unsigned int _NOTDEFINE10[3];      //
    volatile unsigned int OD_CTRL;              // 0x2A4    R/W     0x00000000    ODMA control register
    volatile unsigned int OD_STATE;             // 0x2A8    R/W     0x00000000    ODMA States
    volatile unsigned int _NOTDEFINE11[85];     //
    volatile unsigned int GM_CTRL;              // 0x400    R/W     0x00000000    Gamut-mapper Control Register
    volatile unsigned int GM_STATUS;            // 0x404    R/W     0x00000000    Gamut-mapper Status Register
    volatile unsigned int GM_REGION0;           // 0x408    R/W     0x00000000    Gamut-mapper Region 0 Register
    volatile unsigned int GM_REGION1;           // 0x40C    R/W     0x00000000    Gamut-mapper Region 1 Register
    volatile unsigned int _NOTDEFINE12[3];      //
    volatile unsigned int GM_INT;               // 0x41C    R/W     0x00000000    Gamut-mapper Interrupt Register
    volatile unsigned int _NOTDEFINE13[120];
    volatile unsigned int HI_CTRL;              // 0x600    R/W     0x00000000    Histogram Control Register
    volatile unsigned int HI_STATUS;            // 0x604    R/W     0x00000000    Histogram Status Register
    volatile unsigned int HI_CONFIG;            // 0x608    R/W     0x00000000    Histogram Configuration Register
    volatile unsigned int HI_REGION0;           // 0x60C    R/W     0x00000000    Histogram Region 0 Register
    volatile unsigned int HI_REGION1;           // 0x610    R/W     0x00000000    Histogram Region 1 Register
    volatile unsigned int _NOTDEFINE14[2];
    volatile unsigned int HI_INT;               // 0x61C    R/W        0x00000000 Histogram Interrupt Register
    volatile unsigned int HI_SEGS[4];           // 0x620 ~ 0x62C  R/W  0x00000000 Histogram Segments Register
    volatile unsigned int HI_CDFS[4];           // 0x630 ~ 0x63C  R    0x00000000 Histogram CDF Register
    volatile unsigned int HI_CNTS[8];           // 0x640 ~ 0x65C  R    0x00000000 Histogram CNT Register
    volatile unsigned int HI_SCALE[4];          // 0x660 ~ 0x66C  R/W  0x00000000 Histogram Scale Register
    volatile unsigned int HI_LUTS[64];          // 0x700 ~ 0x7FC  R/W  0x00000000 Histogram LUT Table Register
}VIQE, *PVIQE;

/************************************************************************
*   10. MIPI(Mobile Industry Processor Interface) (Base Addr = ) // R/W
*************************************************************************/
//#define HwMIPI_BASE								((volatile unsigned long*)0xF025B000)
typedef struct _MIPI_DSI{
    volatile unsigned int   STATUS;         //  0x00    R       0x0010010F  Status register
    volatile unsigned int   SWRST;          //  0x04    R/W     0x00000000  Software reset register
    volatile unsigned int   CLKCTRL;        //  0x08    R/W     0x0000FFFF  Clock control register
    volatile unsigned int   TIMEOUT;        //  0x0C    R/W     0x00FFFFFF  Time-out register
    volatile unsigned int   CONFIG;         //  0x10    R/W     0x02000000  Configuration register
    volatile unsigned int   ESCMODE;        //  0x14    R/W     0x00000000  Escape mode register
    volatile unsigned int   MDRESOL;        //  0x18    R/W     0x03000400  Main display image resolution register
    volatile unsigned int   MVPORCH;        //  0x1C    R/W     0xF0000000  Main display vertical porch register
    volatile unsigned int   MHPORCH;        //  0x20    R/W     0x00000000  Main display horizontal porch register
    volatile unsigned int   MSYNC;          //  0x24    R/W     0x00000000  Main display sync area register
    volatile unsigned int   SDRESOL;        //  0x28    R/W     0x03000400  Sub display image resolution register
    volatile unsigned int   INTSRC;         //  0x2C    R/W     0x00000000  Interrupt source register
    volatile unsigned int   INTMSK;         //  0x30    R/W     0xB337FFFF  Interrupt mask register
    volatile unsigned int   PKTHDR;         //  0x34    W       0x00000000  Packet header FIFO register
    volatile unsigned int   PAYLOAD;        //  0x38    W       0x00000000  Payload FIFO register
    volatile unsigned int   RXFIFO;         //  0x3C    R       0xXXXXXXXX  Read FIFO register
    volatile unsigned int   RESERVED;       //  0x40    R/W     0x000001FF  Reserved register
    volatile unsigned int   FIFOCTRL;       //  0x44    R/W     0x0155551F  FIFO status & control register
    volatile unsigned int   MEMACCHR;       //  0x48    R/W     0x00004040  FIFO memory AC characteristic register
    volatile unsigned int   PLLCTRL;        //  0x4C    R/W     0x00000000  PLL control register
    volatile unsigned int   PLLTMR;         //  0x50    R/W     0xFFFFFFFF  PLL timer register
    volatile unsigned int   PHYACCHR;       //  0x54    R/W     0x00000000  D-PHY AC characteristic register
    volatile unsigned int   PHYACCHR1;      //  0x58    R/W     0x00000000  D-PHY AC characteristic register 1
}MIPI_DSI, *PMIPI_DSI;


/************************************************************************
*   11. DDI_CONFIG (Base Addr = 0xF0250000) // R/W
*************************************************************************/
//#define HwDDI_CONFIG_BASE							((volatile unsigned long*)0xF0251000)
typedef struct _DDICONFIG{
    volatile unsigned int   NTSCPAL_SEL     ;   //  0x000       R/W 0x00000001  NTSCPAL_Encoder select
	volatile unsigned int	LVDS_CTRL;          // 0x004 R/W 0x04444443 LVDS Control register
	volatile unsigned int	LVDS_TXO_SEL0;      // 0x008 R/W 0x03020100 LVDS TXOUT select #0
	volatile unsigned int	LVDS_TXO_SEL1;      // 0x00C R/W 0x09080504 LVDS TXOUT select #1
	volatile unsigned int	LVDS_TXO_SEL2;      // 0x010 R/W 0x0D0C0B0A LVDS TXOUT select #2
	volatile unsigned int	LVDS_TXO_SEL3;      // 0x014 R/W 0x13121110 LVDS TXOUT select #3
	volatile unsigned int	LVDS_TXO_SEL4;      // 0x018 R/W 0x1A191514 LVDS TXOUT select #4
	volatile unsigned int	LVDS_TXO_SEL5;      // 0x01C R/W 0x0E070618 LVDS TXOUT select #5
	volatile unsigned int	LVDS_TXO_SEL6;      // 0x020 R/W 0x1B17160F LVDS TXOUT select #6
	volatile unsigned int	LVDS_TXO_SEL7;      // 0x024 R/W 0x1F1E1F1E LVDS TXOUT select #7
	volatile unsigned int	LVDS_TXO_SEL8;      // 0x028 R/W 0x001E1F1E LVDS TXOUT select #8
    volatile unsigned int   HDMI_CTRL       ;   //  0x02C       R/W 0x00008002  HDMI Control register
    volatile unsigned int   PWDN            ;   //  0x030       R/W 0x00000000  Power Down
    volatile unsigned int   SWRESET         ;   //  0x034       R/W 0x00000000  Soft Reset
    volatile unsigned int   ON_THE_FLY      ;   //  0x038       R/W 0x00000000  On-The-Fly mode
    volatile unsigned int   NOTDEFINE1[2]   ;   //
    volatile unsigned int   HDMI_AES        ;   //  0x044       R/W 0x00000000  HDMI AES
    volatile unsigned int   HDMI_AES_DATA0  ;   //  0x048       RW  0x00000000  HDMI AES DATA #0
    volatile unsigned int   HDMI_AES_DATA1  ;   //  0x04C       R/W 0x00000000  HDMI AES DATA #1
    volatile unsigned int   HDMI_AES_HW0    ;   //  0x050       R/W 0x00000000  HDMI AES HW #0
    volatile unsigned int   HDMI_AES_HW1    ;   //  0x054       R/W 0x00000000  HDMI AES HW #1
    volatile unsigned int   HDMI_AES_HW2    ;   //  0x058       R/W 0x00000000  HDMI AES HW #2
    volatile unsigned int   DDI_EMA         ;   //  0x05C       R/W 0x00000001  DDI Memory EMA
}DDICONFIG, *PDDICONFIG;

/************************************************************************
*   12. DDI_CACHE (Base Addr = ) // R/W
*************************************************************************/
//#define HwDDICTRL_BASE							((volatile unsigned long*)0xF0250000)
//#define HwDDICACHE_BASE							((volatile unsigned long*)0xF0250000)

typedef struct _DDICACHE{
	volatile unsigned int   DDIC_CTRL       ;       //  0x000   R/W     0x00000000      DDI_CACHE Control
	volatile unsigned int   DDIC0_CFG0      ;       //  0x004   R/W     0x00000000      DDI_CACHE0 Configuration #0
	volatile unsigned int   DDIC0_CFG1      ;       //  0x008   R/W     0x00000000      DDI_CACHE0 Configuration #1
	volatile unsigned int   DDIC1_CFG0      ;       //  0x00C   R/W     0x00000000      DDI_CACHE1 Configuration #0
	volatile unsigned int   DDIC1_CFG1      ;       //  0x010   R/W     0x00000000      DDI_CACHE1 Configuration #1
	volatile unsigned int   RESERVED0[3]    ;       //
	volatile unsigned int   DDIC0_CH0_AMIN  ;       //  0x020   R/W     0x00000000      DDI_CACHE0 Ch0 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC0_CH0_AMAX  ;       //  0x024   R/W     0x00000000      DDI_CACHE0 Ch0 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC0_CH1_AMIN  ;       //  0x028   R/W     0x00000000      DDI_CACHE0 Ch1 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC0_CH1_AMAX  ;       //  0x02C   R/W     0x00000000      DDI_CACHE0 Ch1 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC0_CH2_AMIN  ;       //  0x030   R/W     0x00000000      DDI_CACHE0 Ch2 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC0_CH2_AMAX  ;       //  0x034   R/W     0x00000000      DDI_CACHE0 Ch2 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC0_CH3_AMIN  ;       //  0x038   R/W     0x00000000      DDI_CACHE0 Ch3 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC0_CH3_AMAX  ;       //  0x03C   R/W     0x00000000      DDI_CACHE0 Ch3 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC0_CH4_AMIN  ;       //  0x040   R/W     0x00000000      DDI_CACHE0 Ch4 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC0_CH4_AMAX  ;       //  0x044   R/W     0x00000000      DDI_CACHE0 Ch4 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC0_CH5_AMIN  ;       //  0x048   R/W     0x00000000      DDI_CACHE0 Ch5 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC0_CH5_AMAX  ;       //  0x04C   R/W     0x00000000      DDI_CACHE0 Ch5 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC1_CH0_AMIN  ;       //  0x050   R/W     0x00000000      DDI_CACHE1 Ch0 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC1_CH0_AMAX  ;       //  0x054   R/W     0x00000000      DDI_CACHE1 Ch0 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC1_CH1_AMIN  ;       //  0x058   R/W     0x00000000      DDI_CACHE1 Ch1 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC1_CH1_AMAX  ;       //  0x05C   R/W     0x00000000      DDI_CACHE1 Ch1 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC1_CH2_AMIN  ;       //  0x060   R/W     0x00000000      DDI_CACHE1 Ch2 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC1_CH2_AMAX  ;       //  0x064   R/W     0x00000000      DDI_CACHE1 Ch2 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC1_CH3_AMIN  ;       //  0x068   R/W     0x00000000      DDI_CACHE1 Ch3 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC1_CH3_AMAX  ;       //  0x06C   R/W     0x00000000      DDI_CACHE1 Ch3 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC1_CH4_AMIN  ;       //  0x070   R/W     0x00000000      DDI_CACHE1 Ch4 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC1_CH4_AMAX  ;       //  0x074   R/W     0x00000000      DDI_CACHE1 Ch4 Cacheable Address Range (MAX)
	volatile unsigned int   DDIC1_CH5_AMIN  ;       //  0x078   R/W     0x00000000      DDI_CACHE1 Ch5 Cacheable Address Range (MIN)
	volatile unsigned int   DDIC1_CH5_AMAX  ;       //  0x07C   R/W     0x00000000      DDI_CACHE1 Ch5 Cacheable Address Range (MAX)
}DDICACHE, *PDDICACHE;


typedef union _ddicctrl{
    unsigned int ddicctrl;
    struct{
        unsigned int ddic0_en:6;
        unsigned int reserved0:9;
        unsigned int ddic1_en:6;
        unsigned int reserved1:9;
    }B;
}UDDICCTRL, *PUDDICCTRL;

typedef union _ddiccfg0{
		unsigned int ddiccfg0;
		struct{
			unsigned int sel00:4;
			unsigned int sel01:4;
			unsigned int sel02:4;
			unsigned int sel03:4;
			unsigned int sel04:4;
			unsigned int sel05:4;
			unsigned int reserved:8;
		}B;
}UDDICCFG0,  *PUDDICCFG0;

typedef union _ddiccfg1{
		unsigned int ddiccfg1;
		struct{
			unsigned int sel06:4;
			unsigned int sel07:4;
			unsigned int sel08:4;
			unsigned int sel09:4;
			unsigned int sel10:4;
			unsigned int sel11:4;
			unsigned int sel12:4;
			unsigned int sel13:4;
		}B;
}UDDICCFG1,  *PUDDICCFG1;

typedef struct _ddicache{
	UDDICCTRL	uDDICCTRL;
	UDDICCFG0	uDDICCFG00;
	UDDICCFG1	uDDICCFG01;
	UDDICCFG0	uDDICCFG10;
	UDDICCFG1	uDDICCFG11;
}SDDICACHE, *PSDDICACHE;

/*******************************************************************************
*
*    TCC88x DataSheet PART 8 VIDEO BUS
*
********************************************************************************/
/************************************************************************
*   4. JPEG Encoder (Base Addr = 0xF0780000) // R/W
*************************************************************************/
//#define HwJPEGENC_BASE 							((volatile unsigned long*)0xF0780000)
typedef struct _JPEGENCODER{
    volatile unsigned int   NOTDEFINE0;     //
    volatile unsigned int   JP_MOD;         //  0x004   R/W     ALL 0x00000000  JPEG codec mode register
    volatile unsigned int   JP_INT_MASK;    //  0x008   R/W     ALL 0x0000001f  Interrupt mask register
    volatile unsigned int   JP_INT_LEVEL;   //  0x00c   R/W     SLV 0x000000ff  FIFO interrupt level register
    volatile unsigned int   JP_TRG_MOD;     //  0x010   R/W     ALL 0x00000000  Polling or Interrupt mode selection register
    volatile unsigned int   NOTDEFINE1[3];
    volatile unsigned int   JP_R_YBUF_ADDR;    //  0x020   R/W     JP  0x00000000  Raw data buffer Y address register
    volatile unsigned int   JP_R_UBUF_ADDR;    //  0x024   R/W     JP  0x00000000  Raw data buffer U address register
    volatile unsigned int   JP_R_VBUF_ADDR;    //  0x028   R/W     JP  0x00000000  Raw data V address register
    volatile unsigned int   JP_R_BUF_INFO;     //  0x02c   R/W     JP  0x00000000  Raw data buffer information register
    volatile unsigned int   JP_SIZE;        //  0x030   R/W     JP  0x00000000  Image size information register
    volatile unsigned int   JP_CHROMA;      //  0x034   R/W     JP  0x00000000  Image format information register
    volatile unsigned int   JP_CBUF_ADDR;   //  0x38    R/W     JP  0x00000000  Coded data buffer address register
    volatile unsigned int   JP_CBUF_SIZE;   //  0x03c   R/W     JP  0x00000fff  Coded data buffer size register
    volatile unsigned int   NOTDEFINE2[12];
    volatile unsigned int   JP_START;       //  0x070   W       ALL 0x00000000  Codec start command register
    volatile unsigned int   NOTDEFINE3[3];
    volatile unsigned int   JP_SBUF_WCNT;   //  0x080   R/W     MST 0x00000000  Source buffer write count register
    volatile unsigned int   JP_SBUF_RCNT;   //  0x084   R       MST 0x00000000  Source buffer read count register
    volatile unsigned int   JP_DBUF_WCNT;   //  0x088   R       MST 0x00000000  Destination buffer write count register
    volatile unsigned int   JP_DBUF_RCNT;   //  0x08c   R/W     MST 0x00000000  Destination buffer read count register
    volatile unsigned int   JP_IFIFO_ST;    //  0x090   R       SLV 0x00000000  Input FIFO status register
    volatile unsigned int   JP_OFIFO_ST;    //  0x094   R       SLV 0x00000000  Output FIFO status register
    volatile unsigned int   NOTDEFINE4[2];
    volatile unsigned int   JP_INT_FLAG;    //  0x0a0   R       ALL 0x00000000  Interrupt flag register
    volatile unsigned int   JP_INT_ACK;     //  0x0a4   R       ALL 0x00000000  Interrupt ack register
    volatile unsigned int  NOTDEFINE5[6];
    volatile unsigned int   JP_IFIFO_WD;    //  0x0c0   W       SLV 0x00000000  Input FIFO write data register
    volatile unsigned int   NOTDEFINE6[7];
    volatile unsigned int   JP_OFIFO_RD;    //  0x0e0   R       SLV 0x00000000  Output FIFO read data register
    volatile unsigned int   NOTDEFINE7[7];
    volatile unsigned int   JPC_QTAB0[64];  //  0x100 - W       JPC 0x00000000  Encoder Q table 0 (64 entries)
    volatile unsigned int   JPC_QTAB1[64];  //  0x200 - W       JPC 0x00000000  Encoder Q table 1 (64 entries)
}JPEGENCODER, *PJPEGENCODER;


/************************************************************************
*   5. Video Codec (Base Addr = 0xF0700000) // R/W
*************************************************************************/
//#define HwVIDEOCODEC_BASE 						((volatile unsigned long*)0xF0700000)

/************************************************************************
*   6. Video Cache (Base Addr = 0xF0710000) // R/W
*************************************************************************/
//#define HwVIDEOCACHE_BASE 						((volatile unsigned long*)0xF0710000)
typedef struct _VIDEOCACHE{
    volatile unsigned int   VCCTRL  ;       //  0x000   R/W     -   0x00000000  Video cache control register
    volatile unsigned int   VCREG   ;       //  0x004   R/W     -   0x00000000  Video cache read/write register
    volatile unsigned int   VWBCTRL ;       //  0x008   R/W     -   0x00000000  Video cache write valid register
    volatile unsigned int   NODEFILED0[6];
    volatile unsigned int   R0MIN   ;       //  0x024   R/W     -   -           Video cache read reason 0 minimum register
    volatile unsigned int   R0MAX   ;       //  0x028   R/W     -   -           Video cache read reason 0 maximum register
    volatile unsigned int   R1MIN   ;       //  0x02c   R/W     -   -           Video cache read reason 1 minimum register
    volatile unsigned int   R1MAX   ;       //  0x030   R/W     -   -           Video cache read reason 1 maximum register
    volatile unsigned int   R2MIN   ;       //  0x034   R/W     -   -           Video cache read reason 2 minimum register
    volatile unsigned int   R2MAX   ;       //  0x038   R/W     -   -           Video cache read reason 2 maximum register
    volatile unsigned int   R3MIN   ;       //  0x03c   R/W     -   -           Video cache read reason 3 minimum register
    volatile unsigned int   R3MAX   ;       //  0x040   R/W     -   --          Video cache read reason 3 maximum register
}VIDEOCACHE, *PVIDEOCACHE;


/************************************************************************
*   7. Video Bus Configuration (Base Addr = 0xF0720000) // R/W
*************************************************************************/
//#define HwVIDEOBUSCFG_BASE 						((volatile unsigned long*)0xF0720000)

typedef struct _VIDEOBUSCFG{
    volatile unsigned int   PWDN;           // 0x000
    volatile unsigned int   SWRESET;        // 0x004
    volatile unsigned int   MEM_EMA;        // 0x008
}VIDEOBUSCFG, *PVIDEOBUSCFG;


/*******************************************************************************
*
*    TCC88x DataSheet PART 9 CAMERA BUS
*
********************************************************************************/

/************************************************************************
*   4. Camera Interface (Base Addr = 0xF0900000) // R/W
*************************************************************************/
//#define HwCIF_BASE							((volatile unsigned long*)0xF0900000)
//#define HwCIFEFFECTOR_BASE					((volatile unsigned long*)0xF0900100)
//#define HwCIFSCALER_BASE						((volatile unsigned long*)0xF0900200)

typedef struct _CIF{
    volatile unsigned int   ICPCR1;             //  0x00    W/R 0x00000000  Input Image Color/Pattern Configuration Register 1
    volatile unsigned int   CCIR656FCR1;        //  0x04    W/R 0x06ff0000  CCIR656 Format Configuration Register 1
    volatile unsigned int   CCIR656FCR2;        //  0x08    W/R 0x010b      CCIR656 Format Configuration Register 2
    volatile unsigned int   IIS;                //  0x0C    W/R 0x00000000  Input Image Size
    volatile unsigned int   IIW1;               //  0x10    W/R 0x00000000  Input Image Windowing 1
    volatile unsigned int   IIW2;               //  0x14    W/R 0x00000000  Input Image Windowing 2
    volatile unsigned int   CDCR1;              //  0x18    W/R 0x0003      DMA Configuration Register 1
    volatile unsigned int   CDCR2;              //  0x1C    W/R 0x00000000  DMA Configuration Register 2
    volatile unsigned int   CDCR3;              //  0x20    W/R 0x00000000  DMA Configuration Register 3
    volatile unsigned int   CDCR4;              //  0x24    W/R 0x00000000  DMA Configuration Register 4
    volatile unsigned int   CDCR5;              //  0x28    W/R 0x00000000  DMA Configuration Register 5
    volatile unsigned int   CDCR6;              //  0x2C    W/R 0x00000000  DMA Configuration Register 6
    volatile unsigned int   CDCR7;              //  0x30    W/R 0x00000000  DMA Configuration Register 7
    volatile unsigned int   CDCR8;              //  0x34    W/R 0x00000000  DMA Configuration Register 8
    volatile unsigned int   FIFOSTATE;          //  0x38    R   0x00000000  FIFO Status Register
    volatile unsigned int   CIRQ;               //  0x3C    W/R 0x00000000  Interrupt & Status register
    volatile unsigned int   OCTRL1;             //  0x40    W/R 0x37000000  Overlay Control 1
    volatile unsigned int   OCTRL2;             //  0x44    W/R 0x00000000  Overlay Control 2
    volatile unsigned int   OCTRL3;             //  0x48    W/R 0x00000000  Overlay Control 3
    volatile unsigned int   OCTRL4;             //  0x4C    W/R 0x00000000  Overlay Control 4
    volatile unsigned int   OIS;                //  0x50    W/R 0x00000000  Overlay Image Size
    volatile unsigned int   OIW1;               //  0x54    W/R 0x00000000  Overlay Image Windowing 1
    volatile unsigned int   OIW2;               //  0x58    W/R 0x00000000  Overlay Image Windowing 2
    volatile unsigned int   COBA;               //  0x5C    W/R 0x00000000  Overlay Base Address
    volatile unsigned int   COBO;               //  0x60    W/R 0x00000000  Overlay Base Address Offset
    volatile unsigned int   CDS;                //  0x64    W/R 0x00000000  Camera Down Scaler
    volatile unsigned int   CCM1;               //  0x68    W/R 0x00000000  Capture Mode Configuration 1
    volatile unsigned int   CCM2;               //  0x6C    W/R 0x00000000  Capture Mode Configuration 2
    volatile unsigned int   CESA;               //  0x70    W/R 0x00000000  Point Encoding Start Address
    volatile unsigned int   CR2Y;               //  0x74    W/R 0x00000000  RGB2YUV Format converter Configuration
    volatile unsigned int   CCYA;               //  0x78    R   -           Current Y Address
    volatile unsigned int   CCUA;               //  0x7C    R   -           Current U Address
    volatile unsigned int   CCVA;               //  0x80    R   -           Current V Address
    volatile unsigned int   CCLC;               //  0x84    R               Current Line count
} CIF, *PCIF;

typedef struct _EFFECT{
    volatile unsigned int   CEM;                //  0x00    W/R     0x00000000  Effect mode register
    volatile unsigned int   CSUV;               //  0x04    W/R     0x00000000  Sepia UV setting
    volatile unsigned int   CCS;                //  0x08    W/R     0x00000000  Color selection register
    volatile unsigned int   CHFC;               //  0x0C    W/R     0x00000000  H-filter coefficent0
    volatile unsigned int   CST;                //  0x10    W/R     0x00000000  Sketch threshold register
    volatile unsigned int   CCT;                //  0x14    W/R     0x00000000  Clamp threshold register
    volatile unsigned int   CBR;                //  0x18    W/R     0x00000000  BIAS register
    volatile unsigned int   CEIS;               //  0x1C    W/R     0x00000000  Image size register
    volatile unsigned int   Reserved0[8];      //  0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C, 0x40
    volatile unsigned int   CISA1;              //  0x44    W/R     0x00000000  Source address in Y channel
    volatile unsigned int   CISA2;              //  0x48    W/R     0x00000000  Source address in U channel
    volatile unsigned int   CISA3;              //  0x4C    W/R     0x00000000  Source address in V channel
    volatile unsigned int   CISS;               //  0x50    W/R     0x00000000  Source image size
    volatile unsigned int   CISO;               //  0x54    W/R     0x00000000  Source image offset
    volatile unsigned int   CIDS;               //  0x58    W/R     0x00000000  Destination image size
    volatile unsigned int   CIS;                //  0x5C    W/R     0x00000000  Target scale
} EFFECT, *PEFFECT;

typedef struct _CIFSCALER{
    volatile unsigned int   CSC;            //  0x00    W/R     0x00000000  Scaler configuration
    volatile unsigned int   CSSF;           //  0x04    W/R     0x00000000  Scale factor
    volatile unsigned int   CSSO;           //  0x08    W/R     0x00000000  Image offset
    volatile unsigned int   CSSS;           //  0x0C    W/R     0x00000000  Source image size
    volatile unsigned int   CSDS;           //  0x10    W/R     0x00000000  Destination image size
} CIFSCALER, *PCIFSCALER;

/************************************************************************
*   5. ISP(Image Signal Processor) (Base Addr = 0xF0910000) // R/W
*************************************************************************/
//#define HwISPBASE								((volatile unsigned long*)0xF0910000)
//#define HwISPIMGEFF_BASE						((volatile unsigned long*)0xF0910200)
//#define HwISPSI_BASE							((volatile unsigned long*)0xF0910300)
//#define HwISPISP_BASE							((volatile unsigned long*)0xF0910400)
//#define HwISPFLASH_BASE						((volatile unsigned long*)0xF0910660)
//#define HwISPSHUT_BASE						((volatile unsigned long*)0xF0910680)
//#define HwISPCPROC_BASE						((volatile unsigned long*)0xF0910800)
//#define HwISPMRSZ_BASE						((volatile unsigned long*)0xF0910C00)
//#define HwISPSRSZ_BASE						((volatile unsigned long*)0xF0911000)
//#define HwISPMI_BASE							((volatile unsigned long*)0xF0911400)
//#define HwISPJPE_BASE							((volatile unsigned long*)0xF0911800)
//#define HwISPMIPI_BASE						((volatile unsigned long*)0xF0911C00)

/************************************************************************
*   6. CAMBUS Configuration (Base Addr = 0xF0920000) // R/W
*************************************************************************/
//#define HwCAMBUSMATRIX_BASE					((volatile unsigned long*)0xF0920000)
//#define HwCAMBUSCFG_BASE						((volatile unsigned long*)0xF0930000)

typedef struct _CAMBUSCFG {
	volatile unsigned int	HCLKMASK;			//0x00			RW
	volatile unsigned int	SoftResetRegister;			//0x04			RW
    volatile unsigned int   CAMBusClk0Sel;              // 0xB0230008
    volatile unsigned int   MemoryEMACtrReg;            // 0xB023000C
    volatile unsigned int   NOTDEFINED[4];              //
    volatile unsigned int   ISP_RC_CTRL;                // 0xB0230020
    volatile unsigned int   ISP_RC_A0MIN;               // 0xB0230024
    volatile unsigned int   ISP_RC_A0MAX;               // 0xB0230028
    volatile unsigned int   CIF_RC_CTRL;                // 0xB023002C
    volatile unsigned int   CIF_RC_A0MIN;               // 0xB0230030
    volatile unsigned int   CIF_RC_A0MAX;         
} CAMBUSCFG, *PCAMBUSCFG;

#endif /*__TCC88xxSTRUCTURES_H__*/
