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

#ifndef _STRUCTURES_SMU_PMU_H_
#define _STRUCTURES_SMU_PMU_H_


/*******************************************************************************
*
*    TCC892x DataSheet PART 2 SMU & PMU
*
********************************************************************************/

/************************************************************************
*   1. CKC                                       (Base Addr = 0x74000000)
************************************************************************/

typedef struct {
    unsigned SEL            :4;
    unsigned CONFIG         :16;
    unsigned                :1;
    unsigned EN             :1;
    unsigned                :10;
} CPUCLK_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CPUCLK_IDX_TYPE         bREG;
} CPUCLK_TYPE;

typedef struct {
    unsigned SEL            :4;
    unsigned CONFIG         :4;
    unsigned                :13;
    unsigned EN             :1;
    unsigned                :7;
    unsigned CFGREQ         :1;
    unsigned SYNCREQ        :1;
    unsigned CHGREQ         :1;
} CLKCTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CLKCTRL_IDX_TYPE        bREG;
} CLKCTRL_TYPE;

typedef struct {
    unsigned P              :6;
    unsigned                :2;
    unsigned M              :10;
    unsigned                :2;
    unsigned LEN            :1;
    unsigned LOCK           :1;
    unsigned                :2;
    unsigned S              :3;
    unsigned                :3;
    unsigned VSEL           :1;
    unsigned EN             :1;
} PLLCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PLLCFG_IDX_TYPE         bREG;
} PLLCFG_TYPE;

typedef struct {
    unsigned P3DIV          :6;
    unsigned                :1;
    unsigned P3TE           :1;
    unsigned P2DIV          :6;
    unsigned                :1;
    unsigned P2TE           :1;
    unsigned P1DIV          :6;
    unsigned                :1;
    unsigned P1TE           :1;
    unsigned P0DIV          :6;
    unsigned                :1;
    unsigned P0TE           :1;
} CLKDIVC0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CLKDIVC0_IDX_TYPE       bREG;
} CLKDIVC0_TYPE;

typedef struct {
    unsigned XTDIV          :6;
    unsigned                :1;
    unsigned XTTE           :1;
    unsigned XDIV           :6;
    unsigned                :1;
    unsigned XTE            :1;
    unsigned P5DIV          :6;
    unsigned                :1;
    unsigned P5TE           :1;
    unsigned P4DIV          :6;
    unsigned                :1;
    unsigned P4TE           :1;
} CLKDIVC1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CLKDIVC1_IDX_TYPE       bREG;
} CLKDIVC1_TYPE;

typedef struct {
    unsigned                :1;
    unsigned                :1;
    unsigned DDI            :1;
    unsigned GPU            :1;
    unsigned                :1;
    unsigned VBUS           :1;
    unsigned VCORE          :1;
    unsigned                :1;
    unsigned HSIO           :1;
    unsigned                :23;
} SWRESET_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SWRESET_IDX_TYPE        bREG;
} SWRESET_TYPE;

typedef struct {
    unsigned DIV            :12;
    unsigned                :12;
    unsigned SEL            :5;
    unsigned EN             :1;
    unsigned                :2;
} PCLK_XXX_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PCLK_XXX_IDX_TYPE       bREG;
} PCLK_XXX_TYPE;

typedef struct {
    unsigned DIV            :16;
    unsigned                :8;
    unsigned SEL            :5;
    unsigned EN             :1;
    unsigned                :1;
    unsigned MD             :1;
} PCLK_YYY_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PCLK_YYY_IDX_TYPE       bREG;
} PCLK_YYY_TYPE;

typedef struct _CKC {
    volatile CPUCLK_TYPE        CLKCTRL0;       //0x000  R/W  0x002FF014    CPU Clock
    volatile CLKCTRL_TYPE       CLKCTRL1;       //0x004  R/W  0x002FF014    Memory Bus
    volatile CLKCTRL_TYPE       CLKCTRL2;       //0x008  R/W  0x002FF014    Display Bus
    volatile CLKCTRL_TYPE       CLKCTRL3;       //0x00C  R/W  0x002FF014    Graphic Bus
    volatile CLKCTRL_TYPE       CLKCTRL4;       //0x010  R/W  0x002FF014    I/O Bus
    volatile CLKCTRL_TYPE       CLKCTRL5;       //0x014  R/W  0x002FF014    Bus Clock for Video Codec
    volatile CLKCTRL_TYPE       CLKCTRL6;       //0x018  R/W  0x002FF014    Core Clock for Video Codec
    volatile CLKCTRL_TYPE       CLKCTRL7;       //0x01C  R/W  0x002FF014    HSIO Bus
    volatile CLKCTRL_TYPE       CLKCTRL8;       //0x020  R/W  0x002FF014    SMU Hardware
    unsigned :32; unsigned :32; unsigned :32;
    volatile PLLCFG_TYPE        PLL0CFG;        //0x030  R/W
    volatile PLLCFG_TYPE        PLL1CFG;        //0x034  R/W
    volatile PLLCFG_TYPE        PLL2CFG;        //0x038  R/W
    volatile PLLCFG_TYPE        PLL3CFG;        //0x03C  R/W
    volatile PLLCFG_TYPE        PLL4CFG;        //0x040  R/W
    volatile PLLCFG_TYPE        PLL5CFG;        //0x044  R/W
    unsigned :32; unsigned :32;
    volatile CLKDIVC0_TYPE      CLKDIVC0;       //0x050  R/W  0x81818181    PLL0/1/2/3 Divider Configuration Register
    volatile CLKDIVC1_TYPE      CLKDIVC1;       //0x054  R/W  0x81818181    PLL4/5/XIN/XTIN Divider Configuration Register
    volatile unsigned long      CLKDIVC2;       //0x058  R/W  0x81818181
    volatile unsigned long      CLKDIVC3;       //0x05C  R/W  0x81818181
    volatile unsigned long      SWRSTPRD;
    volatile SWRESET_TYPE       SWRESET;        //0x064  R/W  0x00000000
    unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile PCLK_XXX_TYPE      PCLKCTRL00;     //0x080  R/W  0x24000000    Timer X
    volatile PCLK_XXX_TYPE      PCLKCTRL01;     //0x084  R/W  0x24000000    Timer T
    volatile PCLK_XXX_TYPE      PCLKCTRL02;     //0x088  R/W  0x24000000    Timer Z
    volatile PCLK_XXX_TYPE      PCLKCTRL03;     //0x08C  R/W  0x24000000    LCD Clock for Controller 0
    volatile PCLK_XXX_TYPE      PCLKCTRL04;     //0x090  R/W  0x24000000    LCD Clock for System Interface 0
    volatile PCLK_XXX_TYPE      PCLKCTRL05;     //0x094  R/W  0x24000000    LCD Clock for Controller 1
    volatile PCLK_XXX_TYPE      PCLKCTRL06;     //0x098  R/W  0x24000000    LCD Clock for System Interface 1
    volatile PCLK_XXX_TYPE      PCLKCTRL07;     //0x09C  R/W  0x24000000
    volatile PCLK_XXX_TYPE      PCLKCTRL08;     //0x0A0  R/W  0x24000000    LCD Timer Clock
    volatile PCLK_XXX_TYPE      PCLKCTRL09;     //0x0A4  R/W  0x24000000    JPEG Clcok
    volatile PCLK_XXX_TYPE      PCLKCTRL10;     //0x0A8  R/W  0x24000000
    volatile PCLK_XXX_TYPE      PCLKCTRL11;     //0x0AC  R/W  0x24000000
    volatile PCLK_XXX_TYPE      PCLKCTRL12;     //0x0B0  R/W  0x24000000    GMAC 1 Operation Clock
    volatile PCLK_XXX_TYPE      PCLKCTRL13;     //0x0B4  R/W  0x24000000    USB OTG
    volatile PCLK_XXX_TYPE      PCLKCTRL14;     //0x0B8  R/W  0x24000000
    volatile PCLK_XXX_TYPE      PCLKCTRL15;     //0x0BC  R/W  0x24000000    External Clock Out #0
    volatile PCLK_XXX_TYPE      PCLKCTRL16;     //0x0C0  R/W  0x24000000    Reference Clock for USB 2.0 Host
    volatile PCLK_XXX_TYPE      PCLKCTRL17;     //0x0C4  R/W  0x24000000    Ref. Clcok for HDMI Link Controller
    volatile PCLK_YYY_TYPE      PCLKCTRL18;     //0x0C8  R/W  0x24000000    Audio Clock for HDMI Link Ctrl.
    volatile PCLK_XXX_TYPE      PCLKCTRL19;     //0x0CC  R/W  0x24000000    External Clock Out #1
    volatile PCLK_XXX_TYPE      PCLKCTRL20;     //0x0D0  R/W  0x24000000    Operating Clock for EHI Controller
    volatile PCLK_XXX_TYPE      PCLKCTRL21;     //0x0D4  R/W  0x24000000    Clock for SDMMC Controller 0
    volatile PCLK_XXX_TYPE      PCLKCTRL22;     //0x0D8  R/W  0x24000000    Clock for SDMMC Controller 1
    volatile PCLK_XXX_TYPE      PCLKCTRL23;     //0x0DC  R/W  0x24000000    Clock for SDMMC Controller 2
    volatile PCLK_XXX_TYPE      PCLKCTRL24;     //0x0E0  R/W  0x24000000    Clock for SDMMC Controller 3
    volatile PCLK_YYY_TYPE      PCLKCTRL25;     //0x0E4  R/W  0x24000000    Clock for DAI Controller of Audio 1(ADMA)
    volatile PCLK_YYY_TYPE      PCLKCTRL26;     //0x0E8  R/W  0x24000000    Filter Clock for Audio DAM Block
    volatile PCLK_YYY_TYPE      PCLKCTRL27;     //0x0EC  R/W  0x24000000    Clock for SPDIF TX Block of Audio 1
    volatile PCLK_YYY_TYPE      PCLKCTRL28;     //0x0F0  R/W  0x24000000    Clock for DAI Block (MC ADMA)
    volatile PCLK_YYY_TYPE      PCLKCTRL29;     //0x0F4  R/W  0x24000000    Filter Clock for Audio DAM Block
    volatile PCLK_YYY_TYPE      PCLKCTRL30;     //0x0F8  R/W  0x24000000    Clock for SPDIF TX/RX Block
    volatile PCLK_XXX_TYPE      PCLKCTRL31;     //0x0FC  R/W  0x24000000    Clock for PDM Controller
    volatile PCLK_XXX_TYPE      PCLKCTRL32;     //0x100  R/W  0x24000000
    volatile PCLK_YYY_TYPE      PCLKCTRL33;     //0x104  R/W  0x24000000    Clock for ADC (Touch Screen)
    volatile PCLK_XXX_TYPE      PCLKCTRL34;     //0x108  R/W  0x24000000    Clock for I2C Master Controller 0
    volatile PCLK_XXX_TYPE      PCLKCTRL35;     //0x10C  R/W  0x24000000    Clock for I2C Master Controller 1
    volatile PCLK_XXX_TYPE      PCLKCTRL36;     //0x110  R/W  0x24000000    Clock for I2C Master Controller 2
    volatile PCLK_XXX_TYPE      PCLKCTRL37;     //0x114  R/W  0x24000000    Clock for I2C Master Controller 3
    volatile PCLK_XXX_TYPE      PCLKCTRL38;     //0x118  R/W  0x24000000    Clock for UART Controller 0
    volatile PCLK_XXX_TYPE      PCLKCTRL39;     //0x11C  R/W  0x24000000    Clock for UART Controller 1
    volatile PCLK_XXX_TYPE      PCLKCTRL40;     //0x120  R/W  0x24000000    Clock for UART Controller 2
    volatile PCLK_XXX_TYPE      PCLKCTRL41;     //0x124  R/W  0x24000000    Clock for UART Controller 3
    volatile PCLK_XXX_TYPE      PCLKCTRL42;     //0x128  R/W  0x24000000    Clock for UART Controller 4
    volatile PCLK_XXX_TYPE      PCLKCTRL43;     //0x12C  R/W  0x24000000    Clock for UART Controller 5
    volatile PCLK_XXX_TYPE      PCLKCTRL44;     //0x130  R/W  0x24000000    Clock for UART Controller 6
    volatile PCLK_XXX_TYPE      PCLKCTRL45;     //0x134  R/W  0x24000000    Clock for UART Controller 7
    volatile PCLK_XXX_TYPE      PCLKCTRL46;     //0x138  R/W  0x24000000    Clock for GPSB Controller 0
    volatile PCLK_XXX_TYPE      PCLKCTRL47;     //0x13C  R/W  0x24000000    Clock for GPSB Controller 1
    volatile PCLK_XXX_TYPE      PCLKCTRL48;     //0x140  R/W  0x24000000    Clock for GPSB Controller 2
    volatile PCLK_XXX_TYPE      PCLKCTRL49;     //0x144  R/W  0x24000000    Clock for GPSB Controller 3
    volatile PCLK_XXX_TYPE      PCLKCTRL50;     //0x148  R/W  0x24000000    Clock for GPSB Controller 4
    volatile PCLK_XXX_TYPE      PCLKCTRL51;     //0x14C  R/W  0x24000000    Clock for GPSB Controller 5
} CKC, *PCKC;


/************************************************************************
*   2. VPIC                                      (Base Addr = 0x74100000)
************************************************************************/

typedef struct {
    unsigned TC0            :1;     //00
    unsigned TC1            :1;     //01
    unsigned SMU_I2C        :1;     //02
    unsigned EINT0          :1;     //03
    unsigned EINT1          :1;     //04
    unsigned EINT2          :1;     //05
    unsigned EINT3          :1;     //06
    unsigned EINT4          :1;     //07
    unsigned EINT5          :1;     //08
    unsigned EINT6          :1;     //09
    unsigned EINT7          :1;     //10
    unsigned EINT8          :1;     //11
    unsigned EINT9          :1;     //12
    unsigned EINT10         :1;     //13
    unsigned EINT11         :1;     //14
    unsigned JPGENC         :1;     //15
    unsigned MALIMMU        :1;     //16
    unsigned MALIGPMMU      :1;     //17
    unsigned DC             :1;     //18
    unsigned L2CACHE        :1;     //19
    unsigned LCDC           :1;     //20
    unsigned JPGDEC         :1;     //21
    unsigned                :1;     //22
    unsigned VCOD           :1;     //23
    unsigned MALI           :1;     //24
    unsigned MALIGP         :1;     //25
    unsigned MALIPMU        :1;     //26
    unsigned OVERALY        :1;     //27
    unsigned TSADC          :1;     //28
    unsigned GDMA           :1;     //29
    unsigned DMAX           :1;     //30
    unsigned EHI            :1;     //31
} PIC0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PIC0_IDX_TYPE           bREG;
} PIC_SRC0_TYPE;

typedef struct {
    unsigned SDMMC3         :1;     //32
    unsigned SDMMC2         :1;     //33
    unsigned HDMI           :1;     //34
    unsigned                :1;     //35
    unsigned GPSB           :1;     //36
    unsigned                :1;     //37
    unsigned I2C            :1;     //38
    unsigned MPEFEC         :1;     //39
    unsigned                :1;     //40
    unsigned NFC            :1;     //41
    unsigned REMOCON        :1;     //42
    unsigned RTC            :1;     //43
    unsigned SDMMC0         :1;     //44
    unsigned SDMMC1         :1;     //45
    unsigned GDMAHS         :1;     //46
    unsigned UART           :1;     //47
    unsigned USBOTG         :1;     //48
    unsigned USBH           :1;     //49
    unsigned                :1;     //50
    unsigned GMAC           :1;     //51
    unsigned CIPHER         :1;     //52
    unsigned TSIF           :1;     //53
    unsigned                :1;     //54
    unsigned                :1;     //55
    unsigned USBVBON        :1;     //56
    unsigned USBVBOFF       :1;     //57
    unsigned ADMA1          :1;     //58
    unsigned AUDIO1         :1;     //59
    unsigned ADMA0          :1;     //60
    unsigned AUDIO0         :1;     //61
    unsigned                :1;     //62
    unsigned ARMPMU         :1;     //63
} PIC1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PIC1_IDX_TYPE           bREG;
} PIC_SRC1_TYPE;

typedef struct {
    unsigned IRQ            :1;
    unsigned FIQ            :1;
    unsigned                :30;
} ALLMSK_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    ALLMSK_IDX_TYPE         bREG;
} ALLMSK_TYPE;

typedef struct _PIC{
    volatile PIC_SRC0_TYPE      IEN0;           // 0x000  R/W  0x00000000   Interrupt Enable Register
    volatile PIC_SRC1_TYPE      IEN1;           // 0x004  R/W  0x00010000   Interrupt Enable Register
    volatile PIC_SRC0_TYPE      CLR0;           // 0x008  R/W  0x00000000   Interrupt Clear Register
    volatile PIC_SRC1_TYPE      CLR1;           // 0x00C  R/W  0x00000000   Interrupt Clear Register
    volatile PIC_SRC0_TYPE      STS0;           // 0x010  R    0x00118038   Interrupt Status Register
    volatile PIC_SRC1_TYPE      STS1;           // 0x014  R    0xF0000100   Interrupt Status Register
    volatile PIC_SRC0_TYPE      SEL0;           // 0x018  R/W  0x00000000   IRQ or FIR Selection Register
    volatile PIC_SRC1_TYPE      SEL1;           // 0x01C  R/W  0x00010000   IRQ or FIR Selection Register
    volatile PIC_SRC0_TYPE      SRC0;           // 0x020  R    0x00118038   Source Interrupt Status Register
    volatile PIC_SRC1_TYPE      SRC1;           // 0x024  R    0xF0000100   Source Interrupt Status Register
    volatile PIC_SRC0_TYPE      MSTS0;          // 0x028  R    0x00000000   Masked Status Register
    volatile PIC_SRC1_TYPE      MSTS1;          // 0x02C  R    0x00000000   Masked Status Register
    volatile PIC_SRC0_TYPE      TIG0;           // 0x030  R/W  0x00000000   Test Interrupt Generation Register
    volatile PIC_SRC1_TYPE      TIG1;           // 0x034  R/W  0x00000000   Test Interrupt Generation Register
    volatile PIC_SRC0_TYPE      POL0;           // 0x038  R/W  0x00000000   Interrupt Polarity Register
    volatile PIC_SRC1_TYPE      POL1;           // 0x03C  R/W  0x00000000   Interrupt Polarity Register
    volatile PIC_SRC0_TYPE      IRQ0;           // 0x040  R    0x00000000   IRQ Raw Status Register
    volatile PIC_SRC1_TYPE      IRQ1;           // 0x044  R    0x00000000   IRQ Raw Status Register
    volatile PIC_SRC0_TYPE      FIQ0;           // 0x048  R    Unknown      FIQ Status Register
    volatile PIC_SRC1_TYPE      FIQ1;           // 0x04C  R    Unknown      FIQ Status Register
    volatile PIC_SRC0_TYPE      MIRQ0;          // 0x050  R    0x00000000   Masked IRQ Status Register
    volatile PIC_SRC1_TYPE      MIRQ1;          // 0x054  R    0x00000000   Masked IRQ Status Register
    volatile PIC_SRC0_TYPE      MFIQ0;          // 0x058  R    0x00000000   Masked FIQ Status Register
    volatile PIC_SRC1_TYPE      MFIQ1;          // 0x05C  R    0x00000000   Masked FIQ Status Register
    volatile PIC_SRC0_TYPE      MODE0;          // 0x060  R/W  0x00000000   Trigger Mode Register ? Level or Edge
    volatile PIC_SRC1_TYPE      MODE1;          // 0x064  R/W  0x00000000   Trigger Mode Register ? Level or Edge
    volatile PIC_SRC0_TYPE      SYNC0;          // 0x068  R/W  0xFFFFFFFF   Synchronization Enable Register
    volatile PIC_SRC1_TYPE      SYNC1;          // 0x06C  R/W  0xFFFFFFFF   Synchronization Enable Register
    volatile PIC_SRC0_TYPE      WKEN0;          // 0x070  R/W  0x00000000   Wakeup Event Enable Register
    volatile PIC_SRC1_TYPE      WKEN1;          // 0x074  R/W  0x00000000   Wakeup Event Enable Register
    volatile PIC_SRC0_TYPE      MODEA0;         // 0x078  R/W  0x00000000   Both Edge or Single Edge Register
    volatile PIC_SRC1_TYPE      MODEA1;         // 0x07C  R/W  0x00000000   Both Edge or Single Edge Register
    volatile unsigned long      EI37SEL;        // 0x080  R/W  0x00000000   External INT 3~7 Selection Register
    unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile PIC_SRC0_TYPE      INTMSK0;        // 0x100  R/W  0xFFFFFFFF   Interrupt Output Masking Register
    volatile PIC_SRC1_TYPE      INTMSK1;        // 0x104  R/W  0xFFFFFFFF   Interrupt Output Masking Register
    volatile ALLMSK_TYPE        ALLMSK;         // 0x108  R/W  0x00000003   All Mask Register
} PIC, *PPIC;

typedef struct {
    unsigned VA             :9;
    unsigned                :22;
    unsigned INV            :1;
} VAFIRQ_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    VAFIRQ_IDX_TYPE         bREG;
} VAFIRQ_TYPE;

typedef struct {
    unsigned VN             :7;
    unsigned                :24;
    unsigned INV            :1;
} VNFIRQ_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    VNFIRQ_IDX_TYPE         bREG;
} VNFIRQ_TYPE;

typedef struct {
    unsigned                :26;
    unsigned IHD            :1;
    unsigned FHD            :1;
    unsigned IFLG           :1;
    unsigned FFLG           :1;
    unsigned FPOL           :1;
    unsigned RCL            :1;
} VCTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    VCTRL_IDX_TYPE          bREG;
} VCTRL_TYPE;

typedef struct _VIC{
    volatile VAFIRQ_TYPE        VAIRQ;          // 0x200  R    0x800000XX   IRQ Vector Register
    volatile VAFIRQ_TYPE        VAFIQ;          // 0x204  R    0x800000XX   FIQ Vector Register
    volatile VNFIRQ_TYPE        VNIRQ;          // 0x208  R    0x800000XX   IRQ Vector Number Register
    volatile VNFIRQ_TYPE        VNFIQ;          // 0x20C  R    0x800000XX   FIQ Vector Number Register
    volatile VCTRL_TYPE         VCTRL;          // 0x210  R/W  0x00000000   Vector Control Register
    unsigned :32; unsigned :32; unsigned :32;
    volatile unsigned long      PRIO0;          // 0x220  R/W  0x03020100   Priorities for Interrupt 0 ~ 3
    volatile unsigned long      PRIO1;          // 0x224  R/W  0x07060504   Priorities for Interrupt 4 ~ 7
    volatile unsigned long      PRIO2;          // 0x228  R/W  0x0B0A0908   Priorities for Interrupt 8 ~ 11
    volatile unsigned long      PRIO3;          // 0x22C  R/W  0x0F0E0D0C   Priorities for Interrupt 12 ~ 15
    volatile unsigned long      PRIO4;          // 0x230  R/W  0x13121110   Priorities for Interrupt 16 ~ 19
    volatile unsigned long      PRIO5;          // 0x234  R/W  0x17161514   Priorities for Interrupt 20 ~ 23
    volatile unsigned long      PRIO6;          // 0x238  R/W  0x1B1A1918   Priorities for Interrupt 24 ~ 27
    volatile unsigned long      PRIO7;          // 0x23C  R/W  0x1F1E1D1C   Priorities for Interrupt 28 ~ 31
    volatile unsigned long      PRIO8;          // 0x240  R/W  0x23222120   Priorities for Interrupt 32 ~ 35
    volatile unsigned long      PRIO9;          // 0x244  R/W  0x27262524   Priorities for Interrupt 36 ~ 39
    volatile unsigned long      PRIO10;         // 0x248  R/W  0x2B2A2928   Priorities for Interrupt 40 ~ 43
    volatile unsigned long      PRIO11;         // 0x24C  R/W  0x2F2E2D2C   Priorities for Interrupt 44 ~ 47
    volatile unsigned long      PRIO12;         // 0x250  R/W  0x33323130   Priorities for Interrupt 48 ~ 51
    volatile unsigned long      PRIO13;         // 0x254  R/W  0x37363534   Priorities for Interrupt 52 ~ 55
    volatile unsigned long      PRIO14;         // 0x258  R/W  0x3B3A3938   Priorities for Interrupt 56 ~ 59
    volatile unsigned long      PRIO15;         // 0x25C  R/W  0x3F3E3D3C   Priorities for Interrupt 60 ~ 63
} VIC, *PVIC;


/************************************************************************
*   3. Timer/Counter                             (Base Addr = 0x74300000)
************************************************************************/

typedef struct {
    unsigned EN             :1;
    unsigned CON            :1;
    unsigned PWM            :1;
    unsigned IEN            :1;
    unsigned TCKSEL         :3;
    unsigned POL            :1;
    unsigned CC             :1;
    unsigned STOP           :1;
    unsigned                :22;
} TCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TCFG_IDX_TYPE           bREG;
} TCFG_TYPE;

typedef struct {
    unsigned TCNT           :20;
    unsigned                :12;
} TCNT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TCNT_IDX_TYPE           bREG;
} TCNT_TYPE;

typedef struct {
    unsigned TREF           :20;
    unsigned                :12;
} TREF_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TREF_IDX_TYPE           bREG;
} TREF_TYPE;

typedef struct {
    unsigned TMREF          :16;
    unsigned                :16;
} TMREF_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TMREF_IDX_TYPE          bREG;
} TMREF_TYPE;

typedef struct {
    unsigned TI0            :1;
    unsigned TI1            :1;
    unsigned TI2            :1;
    unsigned TI3            :1;
    unsigned TI4            :1;
    unsigned TI5            :1;
    unsigned TWI            :1;
    unsigned                :1;
    unsigned TF0            :1;
    unsigned TF1            :1;
    unsigned TF2            :1;
    unsigned TF3            :1;
    unsigned TF4            :1;
    unsigned TF5            :1;
    unsigned TWF            :1;
    unsigned                :17;
} TIREQ_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TIREQ_IDX_TYPE          bREG;
} TIREQ_TYPE;

typedef struct {
    unsigned EN             :1;
    unsigned                :2;
    unsigned IEN            :1;
    unsigned TCLKSEL        :3;
    unsigned                :25;
} TWDCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TWDCFG_IDX_TYPE         bREG;
} TWDCFG_TYPE;

typedef struct {
    unsigned TWDCLR         :16;
    unsigned                :16;
} TWDCLR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TWDCLR_IDX_TYPE         bREG;
} TWDCLR_TYPE;

typedef struct {
    unsigned TWDCNT         :16;
    unsigned                :16;
} TWDCNT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TWDCNT_IDX_TYPE         bREG;
} TWDCNT_TYPE;

typedef struct {
    unsigned PRESCALE       :24;
    unsigned EN             :1;
    unsigned LOADZERO       :1;
    unsigned STOPMODE       :1;
    unsigned                :1;
    unsigned LDM0           :1;
    unsigned LDM1           :1;
    unsigned                :2;
} TC32EN_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TC32EN_IDX_TYPE         bREG;
} TC32EN_TYPE;

typedef struct {
    unsigned LOADVAL        :32;
} TC32LDV_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TC32LDV_IDX_TYPE        bREG;
} TC32LDV_TYPE;

typedef struct {
    unsigned CMP            :32;
} TC32CMP_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TC32CMP_IDX_TYPE        bREG;
} TC32CMP_TYPE;

typedef struct {
    unsigned PCNT           :24;
    unsigned                :8;
} TC32PCNT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TC32PCNT_IDX_TYPE       bREG;
} TC32PCNT_TYPE;

typedef struct {
    unsigned MCNT           :32;
} TC32MCNT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TC32MCNT_IDX_TYPE       bREG;
} TC32MCNT_TYPE;

typedef struct {
    unsigned IRQMSTAT       :5;
    unsigned                :3;
    unsigned IRQRSTAT       :5;
    unsigned                :3;
    unsigned IRQEN0         :1;
    unsigned IRQEN1         :1;
    unsigned IRQEN2         :1;
    unsigned IRQEN3         :1;
    unsigned IRQEN4         :1;
    unsigned                :3;
    unsigned BITSEL         :6;
    unsigned RSYNC          :1;
    unsigned IRQCLR         :1;
} TC32IRQ_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TC32IRQ_IDX_TYPE        bREG;
} TC32IRQ_TYPE;

typedef struct _TIMER{
    volatile TCFG_TYPE          TCFG0;          // 0x000  R/W  0x----0000   Timer/Counter 0 Configuration Register
    volatile TCNT_TYPE          TCNT0;          // 0x004  R/W  0x0000       Timer/Counter 0 Counter Register
    volatile TREF_TYPE          TREF0;          // 0x008  R/W  0xFFFF       Timer/Counter 0 Reference Register
    volatile TMREF_TYPE         TMREF0;         // 0x00C  R/W  0x0000       Timer/Counter 0 Middle Reference Register
    volatile TCFG_TYPE          TCFG1;          // 0x010  R/W  0x00         Timer/Counter 1 Configuration Register
    volatile TCNT_TYPE          TCNT1;          // 0x014  R/W  0x0000       Timer/Counter 1 Counter Register
    volatile TREF_TYPE          TREF1;          // 0x018  R/W  0xFFFF       Timer/Counter 1 Reference Register
    volatile TMREF_TYPE         TMREF1;         // 0x01C  R/W  0x0000       Timer/Counter 1 Middle Reference Register
    volatile TCFG_TYPE          TCFG2;          // 0x020  R/W  0x00         Timer/Counter 2 Configuration Register
    volatile TCNT_TYPE          TCNT2;          // 0x024  R/W  0x0000       Timer/Counter 2 Counter Register
    volatile TREF_TYPE          TREF2;          // 0x028  R/W  0xFFFF       Timer/Counter 2 Reference Register
    volatile TMREF_TYPE         TMREF2;         // 0x02C  R/W  0x0000       Timer/Counter 2 Middle Reference Register
    volatile TCFG_TYPE          TCFG3;          // 0x030  R/W  0x00         Timer/Counter 3 Configuration Register
    volatile TCNT_TYPE          TCNT3;          // 0x034  R/W  0x0000       Timer/Counter 3 Counter Register
    volatile TREF_TYPE          TREF3;          // 0x038  R/W  0xFFFF       Timer/Counter 3 Reference Register
    volatile TMREF_TYPE         TMREF3;         // 0x03C  R/W  0x0000       Timer/Counter 3 Middle Reference Register
    volatile TCFG_TYPE          TCFG4;          // 0x040  R/W  0x00         Timer/Counter 4 Configuration Register
    volatile TCNT_TYPE          TCNT4;          // 0x044  R/W  0x00000      Timer/Counter 4 Counter Register
    volatile TREF_TYPE          TREF4;          // 0x048  R/W  0xFFFFF      Timer/Counter 4 Reference Register
    unsigned :32;
    volatile TCFG_TYPE          TCFG5;          // 0x050  R/W  0x00         Timer/Counter 5 Configuration Register
    volatile TCNT_TYPE          TCNT5;          // 0x054  R/W  0x00000      Timer/Counter 5 Counter Register
    volatile TREF_TYPE          TREF5;          // 0x058  R/W  0xFFFFF      Timer/Counter 5 Reference Register
    unsigned :32;
    volatile TIREQ_TYPE         TIREQ;          // 0x060  R/W  0x0000       Timer/Counter n Interrupt Request Register
    unsigned :32; unsigned :32; unsigned :32;
    volatile TWDCFG_TYPE        TWDCFG;         // 0x070  R/W  0x0000       Watchdog Timer Configuration Register
    volatile TWDCLR_TYPE        TWDCLR;         // 0x074    W  -            Watchdog Timer Clear Register
    volatile TWDCNT_TYPE        TWDCNT;         // 0x078  R/W  0xFFE0       Watchdog Timer Counter Register
    unsigned :32;
    volatile TC32EN_TYPE        TC32EN;         // 0x080  R/W  0x00007FFF   32-bit Counter Enable / Pre-scale Value
    volatile TC32LDV_TYPE       TC32LDV;        // 0x084  R/W  0x00000000   32-bit Counter Load Value
    volatile TC32CMP_TYPE       TC32CMP0;       // 0x088  R/W  0x00000000   32-bit Counter Match Value 0
    volatile TC32CMP_TYPE       TC32CMP1;       // 0x08C  R/W  0x00000000   32-bit Counter Match Value 1
    volatile TC32PCNT_TYPE      TC32PCNT;       // 0x090  R/W  -            32-bit Counter Current Value (pre-scale counter)
    volatile TC32MCNT_TYPE      TC32MCNT;       // 0x094  R/W  -            32-bit Counter Current Value (main counter)
    volatile TC32IRQ_TYPE       TC32IRQ;        // 0x098  R/W  0x0000----   32-bit Counter Interrupt Control
} TIMER, *PTIMER;

typedef struct _TIMERN{
    volatile unsigned int   TCFG;          // 0x000   R/W  Timer/Counter Configuration Register
    volatile unsigned int   TCNT;          // 0x004   R/W  Timer/Counter Counter Register
    volatile unsigned int   TREF;          // 0x008   R/W  Timer/Counter Reference Register
    volatile unsigned int   TMREF;         // 0x00C   R/W  Timer/Counter Middle Reference Register
} TIMERN, *PTIMERN;


/************************************************************************
*   4. PMU(Power Management Unit)                (Base Addr = 0x74400000)
************************************************************************/

typedef struct {
    unsigned PU_HSB         :1;
    unsigned PD_HSB         :1;
    unsigned PU_DB          :1;
    unsigned PD_DB          :1;
    unsigned PU_GB          :1;
    unsigned PD_GB          :1;
    unsigned PU_VB          :1;
    unsigned PD_VB          :1;
    unsigned PU_MB          :1;
    unsigned PD_MB          :1;
    unsigned SUB_STATE      :13;
    unsigned MAIN_STATE     :4;
    unsigned                :4;
    unsigned WKUPF          :1;
} PMU_PWRSTS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_PWRSTS_IDX_TYPE     bREG;
} PMU_PWRSTS_TYPE;

typedef struct {
    unsigned CNT            :16;
    unsigned STR            :8;
    unsigned                :6;
    unsigned CLR            :1;
    unsigned EN             :1;
} PMU_WDTCTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_WDTCTRL_IDX_TYPE    bREG;
} PMU_WDTCTRL_TYPE;

typedef struct {
    unsigned STOP           :1;
    unsigned                :15;
    unsigned PWREN          :1;
    unsigned                :15;
} PMU_TSADC_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_TSADC_IDX_TYPE      bREG;
} PMU_TSADC_TYPE;

typedef struct {
    unsigned MASK           :1;
    unsigned                :3;
    unsigned REG            :1;
    unsigned                :3;
    unsigned REGMD          :1;
    unsigned                :19;
    unsigned GB             :1;
    unsigned DB             :1;
    unsigned VB             :1;
    unsigned HSB            :1;
} PMU_SYSRST_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_SYSRST_IDX_TYPE     bREG;
} PMU_SYSRST_TYPE;

typedef struct {
    unsigned BM             :6;
    unsigned                :1;
    unsigned TSEL           :1;
    unsigned GCLK_CNT       :8;
    unsigned PKGI           :2;
    unsigned                :10;
    unsigned REMAP          :2;
    unsigned                :2;
} PMU_CONFIG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_CONFIG_IDX_TYPE     bREG;
} PMU_CONFIG_TYPE;

typedef struct {
    unsigned MEMCFG         :32;
} PMU_MEMCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_MEMCFG_IDX_TYPE     bREG;
} PMU_MEMCFG_TYPE;

typedef struct {
    unsigned USERDAT        :32;
} PMU_USSTATUS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_USSTATUS_IDX_TYPE   bREG;
} PMU_USSTATUS_TYPE;

typedef struct {
    unsigned TSADC_UPDOWN   :1;     //00
    unsigned TSADC_STOP_WKU :1;     //01
    unsigned TSADC_WAKEUP   :1;     //02
    unsigned RTC_WAKEUP     :1;     //03
    unsigned RTC_PMWAKEUP   :1;     //04
    unsigned GPIO_D08       :1;     //05
    unsigned GPIO_D09       :1;     //06
    unsigned GPIO_D12       :1;     //07
    unsigned GPIO_D13       :1;     //08
    unsigned GPIO_D14       :1;     //09
    unsigned GPIO_B11       :1;     //10
    unsigned GPIO_B12       :1;     //11
    unsigned GPIO_B13       :1;     //12
    unsigned GPIO_B14       :1;     //13
    unsigned GPIO_B15       :1;     //14
    unsigned GPIO_C00       :1;     //15
    unsigned GPIO_G05       :1;     //16
    unsigned GPIO_G08       :1;     //17
    unsigned GPIO_G09       :1;     //18
    unsigned GPIO_G10       :1;     //19
    unsigned GPIO_G11       :1;     //20
    unsigned GPIO_G12       :1;     //21
    unsigned GPIO_G13       :1;     //22
    unsigned GPIO_G14       :1;     //23
    unsigned GPIO_G16       :1;     //24
    unsigned GPIO_G17       :1;     //25
    unsigned GPIO_G18       :1;     //26
    unsigned GPIO_G19       :1;     //27
    unsigned GPIO_HDMI00    :1;     //28
    unsigned GPIO_HDMI01    :1;     //29
    unsigned GPIO_ADC04     :1;     //30
    unsigned GPIO_ADC05     :1;     //31
} PMU_WAKEUP0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_WAKEUP0_IDX_TYPE    bREG;
} PMU_WAKEUP0_TYPE;

typedef struct {
    unsigned GPIO_E00       :1;     //32
    unsigned GPIO_E01       :1;     //33
    unsigned GPIO_E02       :1;     //34
    unsigned GPIO_E03       :1;     //35
    unsigned GPIO_E04       :1;     //36
    unsigned GPIO_E05       :1;     //37
    unsigned GPIO_E06       :1;     //38
    unsigned GPIO_E07       :1;     //39
    unsigned GPIO_E08       :1;     //40
    unsigned GPIO_E09       :1;     //41
    unsigned GPIO_E10       :1;     //42
    unsigned GPIO_E11       :1;     //43
    unsigned GPIO_E12       :1;     //44
    unsigned GPIO_E13       :1;     //45
    unsigned GPIO_E14       :1;     //46
    unsigned GPIO_E15       :1;     //47
    unsigned GPIO_E16       :1;     //48
    unsigned GPIO_E17       :1;     //49
    unsigned GPIO_E18       :1;     //50
    unsigned GPIO_E19       :1;     //51
    unsigned GPIO_E20       :1;     //52
    unsigned GPIO_E21       :1;     //53
    unsigned GPIO_E22       :1;     //54
    unsigned GPIO_E23       :1;     //55
    unsigned GPIO_E24       :1;     //56
    unsigned GPIO_E25       :1;     //57
    unsigned GPIO_E26       :1;     //58
    unsigned GPIO_E27       :1;     //59
    unsigned GPIO_E28       :1;     //60
    unsigned GPIO_E29       :1;     //61
    unsigned GPIO_E30       :1;     //62
    unsigned GPIO_E31       :1;     //63
} PMU_WAKEUP1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_WAKEUP1_IDX_TYPE    bREG;
} PMU_WAKEUP1_TYPE;

typedef struct {
    unsigned DATA           :32;
} PWRDN_COMMON_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWRDN_COMMON_IDX_TYPE   bREG;
} PWRDN_COMMON_TYPE;

typedef struct {
    unsigned PD_XIN         :1;
    unsigned                :3;
    unsigned IO_RTO         :1;
    unsigned                :3;
    unsigned SSTL_RTO       :1;
    unsigned                :7;
    unsigned PD_RETN        :1;
    unsigned                :15;
} PWRDN_XIN_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWRDN_XIN_IDX_TYPE      bREG;
} PWRDN_XIN_TYPE;

typedef struct {
    unsigned OTP            :1;
    unsigned RTC            :1;
    unsigned PLL            :1;
    unsigned ECID           :1;
    unsigned HDMI           :1;
    unsigned                :1;
    unsigned VDAC           :1;
    unsigned TSADC          :1;
    unsigned USBHP          :1;
    unsigned USBOP          :1;
    unsigned                :1;
    unsigned LVDS           :1;
    unsigned                :20;
} PMU_ISOL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PMU_ISOL_IDX_TYPE       bREG;
} PMU_ISOL_TYPE;

typedef struct _PMU{
    volatile PMU_PWRSTS_TYPE    PMU_PWRSTS;     // 0x000  R    -            PMU Power Mode Status Register
    volatile PMU_WDTCTRL_TYPE   PMU_WDTCTRL;    // 0x004  R/W  0x0000FFFF   Watchdog Control Register
    volatile PMU_TSADC_TYPE     PMU_TSADC;      // 0x008  R/W  0x00000001   Touch Screen ADC Control Register
    volatile PMU_SYSRST_TYPE    PMU_SYSRST;     // 0x00C  R/W  0xF0000010   System Reset Output Control Register
    volatile PMU_CONFIG_TYPE    PMU_CONFIG;     // 0x010  R/W  -            PMU Configuration Register
    volatile PMU_MEMCFG_TYPE    PMU_MEMCFG;     // 0x014  R/W  0x04110131   On-chip Memory Configuration Register
    volatile PMU_USSTATUS_TYPE  PMU_USSTATUS;   // 0x018  R/W  0x0          User Status Register
    unsigned :32;
    volatile PMU_WAKEUP0_TYPE   PMU_WKUP0;      // 0x020  R/W  0x0          Wake-up Enable Register
    volatile PMU_WAKEUP1_TYPE   PMU_WKUP1;      // 0x024  R/W  0x0          Wake-up Enable Register
    volatile PMU_WAKEUP0_TYPE   PMU_WKPOL0;     // 0x028  R/W  0x0          Wake-up Polarity Register
    volatile PMU_WAKEUP1_TYPE   PMU_WKPOL1;     // 0x02C  R/W  0x0          Wake-up Polarity Register
    volatile PMU_WAKEUP0_TYPE   PMU_WKCLR0;     // 0x030  W    -            Wake-up Clear Register
    volatile PMU_WAKEUP1_TYPE   PMU_WKCLR1;     // 0x034  W    -            Wake-up Clear Register
    volatile PMU_WAKEUP0_TYPE   PMU_WKSTS0;     // 0x038  R    -            Wake-up Status Register
    volatile PMU_WAKEUP1_TYPE   PMU_WKSTS1;     // 0x03C  R    -            Wake-up Status Register
    volatile PWRDN_COMMON_TYPE  PWRDN_TOP;      // 0x040  W    0            TOP Power-Down Mode Enable Register
    volatile PWRDN_COMMON_TYPE  PWRDN_MBUS;     // 0x044  W    0            Memory Bus Power-Down Enter
    volatile PWRDN_COMMON_TYPE  PWRDN_VBUS;     // 0x048  W    0            Video Bus Power-Down Enter
    volatile PWRDN_COMMON_TYPE  PWRDN_GBUS;     // 0x04C  W    0            Graphic Bus Power-Down Enter
    volatile PWRDN_COMMON_TYPE  PWRDN_DBUS;     // 0x050  W    0            Display Bus Power-Down Enter
    volatile PWRDN_COMMON_TYPE  PWRDN_HSBUS;    // 0x054  W    0            HSIO Bus Power-Down Enter
    volatile PWRDN_XIN_TYPE     PWRDN_XIN;      // 0x058  R/W  0x00000110   Oscillator & I/O Configuration Register
    volatile PMU_ISOL_TYPE      PMU_ISOL;       // 0x05C  R/W  0x0          Internal IP isolation Register
    unsigned :32;
    volatile PWRDN_COMMON_TYPE  PWRUP_MBUS;     // 0x064  W    0            Memory Bus Power-Down Exit
    volatile PWRDN_COMMON_TYPE  PWRUP_VBUS;     // 0x068  W    0            Video Bus Power-Down Exit
    volatile PWRDN_COMMON_TYPE  PWRUP_GBUS;     // 0x06C  W    0            Graphic Bus Power-Down Exit
    volatile PWRDN_COMMON_TYPE  PWRUP_DBUS;     // 0x070  W    0            Display Bus Power-Down Exit
    volatile PWRDN_COMMON_TYPE  PWRUP_HSBUS;    // 0x074  W    0            HSIO Bus Power-Down Exit
    volatile PMU_WAKEUP0_TYPE   PMU_WKMOD0;     // 0x078  R/W  0x0          Wake-up Mode Register
    volatile PMU_WAKEUP1_TYPE   PMU_WKMOD1;     // 0x07C  R/W  0x0          Wake-up Mode Register
} PMU, *PPMU;


/************************************************************************
*   5. SMU_I2C                                   (Base Addr = 0x74500000)
************************************************************************/

typedef struct {
    unsigned PRES           :16;
    unsigned                :16;
} SMU_PRES_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_PRES_IDX_TYPE       bREG;
} SMU_PRESS_TYPE;

typedef struct {
    unsigned                :5;
    unsigned MOD            :1;
    unsigned IEN            :1;
    unsigned EN             :1;
    unsigned                :24;
} SMU_CTR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_CTR_IDX_TYPE        bREG;
} SMU_CTR_TYPE;

typedef struct {
    unsigned DATA           :16;
    unsigned                :16;
} SMU_TXR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_TXR_IDX_TYPE        bREG;
} SMU_TXR_TYPE;

typedef struct {
    unsigned IACK           :1;
    unsigned                :2;
    unsigned ACK            :1;
    unsigned WR             :1;
    unsigned RD             :1;
    unsigned STO            :1;
    unsigned STA            :1;
    unsigned                :24;
} SMU_CMD_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_CMD_IDX_TYPE        bREG;
} SMU_CMD_TYPE;

typedef struct {
    unsigned DATA           :16;
    unsigned                :16;
} SMU_RXR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_RXR_IDX_TYPE        bREG;
} SMU_RXR_TYPE;

typedef struct {
    unsigned IF             :1;
    unsigned TIP            :1;
    unsigned                :3;
    unsigned AL             :1;
    unsigned BUSY           :1;
    unsigned RxACK          :1;
    unsigned                :24;
} SMU_SR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_SR_IDX_TYPE         bREG;
} SMU_SR_TYPE;

typedef struct {
    unsigned FC             :5;
    unsigned CKSEL          :1;
    unsigned                :2;
    unsigned RC             :8;
    unsigned                :16;
} SMU_TR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_TR_IDX_TYPE         bREG;
} SMU_TR_TYPE;

typedef struct {
    unsigned DIVIDER        :16;
    unsigned                :15;
    unsigned CNT_EN         :1;
} SMU_ICLK_IDX_TYPE;

typedef union {
    unsigned long            nREG;
    SMU_ICLK_IDX_TYPE        bREG;
} SMU_ICLK_TYPE;

typedef struct _SMU_I2C {
    volatile SMU_PRESS_TYPE     PRES;           // 0x000  R/W  0xFFFF       Clock Prescale register
    volatile SMU_CTR_TYPE       CTRL;           // 0x004  R/W  0x0000       Control Register
    volatile SMU_TXR_TYPE       TXR;            // 0x008  W    0x0000       Transmit Register
    volatile SMU_CMD_TYPE       CMD;            // 0x00C  W    0x0000       Command Register
    volatile SMU_RXR_TYPE       RXR;            // 0x010  R    0x0000       Receive Register
    volatile SMU_SR_TYPE        SR;             // 0x014  R    0x0000       Status register
    volatile SMU_TR_TYPE        TIME;           // 0x018  R/W  0x0000       Timing Control Register
    volatile SMU_ICLK_TYPE      ICLK;           // 0x080  R/W  0x0010       I2C_SCL divider Register
} SMU_I2C, *PSMU_I2C;


/************************************************************************
*   6. GPIO & Port Multiplexing                  (Base Addr = 0x74200000)
************************************************************************/

typedef struct {
    unsigned GP00           :1;
    unsigned GP01           :1;
    unsigned GP02           :1;
    unsigned GP03           :1;
    unsigned GP04           :1;
    unsigned GP05           :1;
    unsigned GP06           :1;
    unsigned GP07           :1;
    unsigned GP08           :1;
    unsigned GP09           :1;
    unsigned GP10           :1;
    unsigned GP11           :1;
    unsigned GP12           :1;
    unsigned GP13           :1;
    unsigned GP14           :1;
    unsigned GP15           :1;
    unsigned GP16           :1;
    unsigned GP17           :1;
    unsigned GP18           :1;
    unsigned GP19           :1;
    unsigned GP20           :1;
    unsigned GP21           :1;
    unsigned GP22           :1;
    unsigned GP23           :1;
    unsigned GP24           :1;
    unsigned GP25           :1;
    unsigned GP26           :1;
    unsigned GP27           :1;
    unsigned GP28           :1;
    unsigned GP29           :1;
    unsigned GP30           :1;
    unsigned GP31           :1;
} GPIO_PORT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPIO_PORT_IDX_TYPE      bREG;
} GPIO_PORT_TYPE;

typedef struct {
    unsigned GPCD00         :2;
    unsigned GPCD01         :2;
    unsigned GPCD02         :2;
    unsigned GPCD03         :2;
    unsigned GPCD04         :2;
    unsigned GPCD05         :2;
    unsigned GPCD06         :2;
    unsigned GPCD07         :2;
    unsigned GPCD08         :2;
    unsigned GPCD09         :2;
    unsigned GPCD10         :2;
    unsigned GPCD11         :2;
    unsigned GPCD12         :2;
    unsigned GPCD13         :2;
    unsigned GPCD14         :2;
    unsigned GPCD15         :2;
} GPIO_CD0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPIO_CD0_IDX_TYPE       bREG;
} GPIO_CD0_TYPE;

typedef struct {
    unsigned GPCD16         :2;
    unsigned GPCD17         :2;
    unsigned GPCD18         :2;
    unsigned GPCD19         :2;
    unsigned GPCD20         :2;
    unsigned GPCD21         :2;
    unsigned GPCD22         :2;
    unsigned GPCD23         :2;
    unsigned GPCD24         :2;
    unsigned GPCD25         :2;
    unsigned GPCD26         :2;
    unsigned GPCD27         :2;
    unsigned GPCD28         :2;
    unsigned GPCD29         :2;
    unsigned GPCD30         :2;
    unsigned GPCD31         :2;
} GPIO_CD1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPIO_CD1_IDX_TYPE       bREG;
} GPIO_CD1_TYPE;

typedef struct {
    unsigned GPFN00         :4;
    unsigned GPFN01         :4;
    unsigned GPFN02         :4;
    unsigned GPFN03         :4;
    unsigned GPFN04         :4;
    unsigned GPFN05         :4;
    unsigned GPFN06         :4;
    unsigned GPFN07         :4;
} GPIO_FN0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPIO_FN0_IDX_TYPE       bREG;
} GPIO_FN0_TYPE;

typedef struct {
    unsigned GPFN08         :4;
    unsigned GPFN09         :4;
    unsigned GPFN10         :4;
    unsigned GPFN11         :4;
    unsigned GPFN12         :4;
    unsigned GPFN13         :4;
    unsigned GPFN14         :4;
    unsigned GPFN15         :4;
} GPIO_FN1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPIO_FN1_IDX_TYPE       bREG;
} GPIO_FN1_TYPE;

typedef struct {
    unsigned GPFN16         :4;
    unsigned GPFN17         :4;
    unsigned GPFN18         :4;
    unsigned GPFN19         :4;
    unsigned GPFN20         :4;
    unsigned GPFN21         :4;
    unsigned GPFN22         :4;
    unsigned GPFN23         :4;
} GPIO_FN2_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPIO_FN2_IDX_TYPE       bREG;
} GPIO_FN2_TYPE;

typedef struct {
    unsigned GPFN24         :4;
    unsigned GPFN25         :4;
    unsigned GPFN26         :4;
    unsigned GPFN27         :4;
    unsigned GPFN28         :4;
    unsigned GPFN29         :4;
    unsigned GPFN30         :4;
    unsigned GPFN31         :4;
} GPIO_FN3_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPIO_FN3_IDX_TYPE       bREG;
} GPIO_FN3_TYPE;

typedef struct {
    unsigned GPFN00         :4;
    unsigned GPFN01         :4;
    unsigned GPFN02         :4;
    unsigned GPFN03         :4;
    unsigned                :16;
} HDMI_FN_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    HDMI_FN_IDX_TYPE        bREG;
} HDMI_FN_TYPE;

typedef struct {
    unsigned                :8;
    unsigned GPFN02         :4;
    unsigned GPFN03         :4;
    unsigned GPFN04         :4;
    unsigned GPFN05         :4;
    unsigned                :8;
} ADC_FN_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    ADC_FN_IDX_TYPE         bREG;
} ADC_FN_TYPE;

typedef struct {
    unsigned EINT00SEL      :7;
    unsigned                :1;
    unsigned EINT01SEL      :7;
    unsigned                :1;
    unsigned EINT02SEL      :7;
    unsigned                :1;
    unsigned EINT03SEL      :7;
    unsigned                :1;
} EINTSEL0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EINTSEL0_IDX_TYPE       bREG;
} EINTSEL0_TYPE;

typedef struct {
    unsigned EINT04SEL      :7;
    unsigned                :1;
    unsigned EINT05SEL      :7;
    unsigned                :1;
    unsigned EINT06SEL      :7;
    unsigned                :1;
    unsigned EINT07SEL      :7;
    unsigned                :1;
} EINTSEL1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EINTSEL1_IDX_TYPE       bREG;
} EINTSEL1_TYPE;

typedef struct {
    unsigned EINT08SEL      :7;
    unsigned                :1;
    unsigned EINT09SEL      :7;
    unsigned                :1;
    unsigned EINT10SEL      :7;
    unsigned                :1;
    unsigned EINT11SEL      :7;
    unsigned                :1;
} EINTSEL2_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EINTSEL2_IDX_TYPE       bREG;
} EINTSEL2_TYPE;

typedef struct _GPIO{
    volatile GPIO_PORT_TYPE     GPADAT;         // 0x000  R/W  0x00000000   GPA Data Register
    volatile GPIO_PORT_TYPE     GPAEN;          // 0x004  R/W  0x00000000   GPA Output Enable Register
    volatile GPIO_PORT_TYPE     GPASET;         // 0x008  W    -            OR function on GPA Output Data
    volatile GPIO_PORT_TYPE     GPACLR;         // 0x00C  W    -            BIC function on GPA Output Data
    volatile GPIO_PORT_TYPE     GPAXOR;         // 0x010  W    -            XOR function on GPA Output Data
    volatile GPIO_CD0_TYPE      GPACD0;         // 0x014  R/W  0x00000000   Driver strength Control on GPA
    volatile GPIO_CD1_TYPE      GPACD1;         // 0x018  R/W  0x00000000   Driver strength Control on GPA
    volatile GPIO_PORT_TYPE     GPAPE;          // 0x01C  R/W  0x00000000   Pull-Up/Down Enable function on GPA
    volatile GPIO_PORT_TYPE     GPAPS;          // 0x020  R/W  0x00000000   Pull-Up/Down Selection function on GPA
    volatile GPIO_PORT_TYPE     GPAIEN;         // 0x024  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPA
    volatile GPIO_PORT_TYPE     GPAIS;          // 0x028  R/W  0x00000000   Schimitt input Function on GPA
    volatile GPIO_PORT_TYPE     GPASR;          // 0x02C  R/W  0x00000000   Fast Slew Rate Function GPA
    volatile GPIO_FN0_TYPE      GPAFN0;         // 0x030  R/W  0x00000000   Port Configuration on GPA Output Data
    volatile GPIO_FN1_TYPE      GPAFN1;         // 0x034  R/W  0x00000000   Port Configuration on GPA Output Data
    volatile GPIO_FN2_TYPE      GPAFN2;         // 0x038  R/W  0x00000000   Port Configuration on GPA Output Data
    volatile GPIO_FN3_TYPE      GPAFN3;         // 0x03C  R/W  0x00000000   Port Configuration on GPA Output Data

    volatile GPIO_PORT_TYPE     GPBDAT;         // 0x040  R/W  0x00000000   GPB Data Register
    volatile GPIO_PORT_TYPE     GPBEN;          // 0x044  R/W  0x00000000   GPB Output Enable Register
    volatile GPIO_PORT_TYPE     GPBSET;         // 0x048  W    -            OR function on GPB Output Data
    volatile GPIO_PORT_TYPE     GPBCLR;         // 0x04C  W    -            BIC function on GPB Output Data
    volatile GPIO_PORT_TYPE     GPBXOR;         // 0x050  W    -            XOR function on GPB Output Data
    volatile GPIO_CD0_TYPE      GPBCD0;         // 0x054  R/W  0x00000000   Driver strength Control on GPB
    volatile GPIO_CD1_TYPE      GPBCD1;         // 0x058  R/W  0x00000000   Driver strength Control on GPB
    volatile GPIO_PORT_TYPE     GPBPE;          // 0x05C  R/W  0x00000000   Pull-Up/Down Enable function on GPB
    volatile GPIO_PORT_TYPE     GPBPS;          // 0x060  R/W  0x00000000   Pull-Up/Down Selection function on GPB
    volatile GPIO_PORT_TYPE     GPBIEN;         // 0x064  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPB
    volatile GPIO_PORT_TYPE     GPBIS;          // 0x068  R/W  0x00000000   Schimitt input Function on GPB
    volatile GPIO_PORT_TYPE     GPBSR;          // 0x06C  R/W  0x00000000   Fast Slew Rate Function GPB
    volatile GPIO_FN0_TYPE      GPBFN0;         // 0x070  R/W  0x00000000   Port Configuration on GPB Output Data
    volatile GPIO_FN1_TYPE      GPBFN1;         // 0x074  R/W  0x00000000   Port Configuration on GPB Output Data
    volatile GPIO_FN2_TYPE      GPBFN2;         // 0x078  R/W  0x00000000   Port Configuration on GPB Output Data
    volatile GPIO_FN3_TYPE      GPBFN3;         // 0x07C  R/W  0x00000000   Port Configuration on GPB Output Data

    volatile GPIO_PORT_TYPE     GPCDAT;         // 0x080  R/W  0x00000000   GPC Data Register
    volatile GPIO_PORT_TYPE     GPCEN;          // 0x084  R/W  0x00000000   GPC Output Enable Register
    volatile GPIO_PORT_TYPE     GPCSET;         // 0x088  W    -            OR function on GPC Output Data
    volatile GPIO_PORT_TYPE     GPCCLR;         // 0x08C  W    -            BIC function on GPC Output Data
    volatile GPIO_PORT_TYPE     GPCXOR;         // 0x090  W    -            XOR function on GPC Output Data
    volatile GPIO_CD0_TYPE      GPCCD0;         // 0x094  R/W  0x00000000   Driver strength Control 0 on GPC
    volatile GPIO_CD1_TYPE      GPCCD1;         // 0x098  R/W  0x00000000   Driver strength Control 0 on GPC
    volatile GPIO_PORT_TYPE     GPCPE;          // 0x09C  R/W  0x00000000   Pull-Up/Down Enable function on GPC
    volatile GPIO_PORT_TYPE     GPCPS;          // 0x0A0  R/W  0x00000000   Pull-Up/Down Selection function on GPC
    volatile GPIO_PORT_TYPE     GPCIEN;         // 0x0A4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPC
    volatile GPIO_PORT_TYPE     GPCIS;          // 0x0A8  R/W  0x00000000   Schimitt input Function on GPC
    volatile GPIO_PORT_TYPE     GPCSR;          // 0x0AC  R/W  0x00000000   Fast Slew Rate Function GPC
    volatile GPIO_FN0_TYPE      GPCFN0;         // 0x0B0  R/W  0x00000000   Port Configuration on GPC Output Data
    volatile GPIO_FN1_TYPE      GPCFN1;         // 0x0B4  R/W  0x00000000   Port Configuration on GPC Output Data
    volatile GPIO_FN2_TYPE      GPCFN2;         // 0x0B8  R/W  0x00000000   Port Configuration on GPC Output Data
    volatile GPIO_FN3_TYPE      GPCFN3;         // 0x0BC  R/W  0x00000000   Port Configuration on GPC Output Data

    volatile GPIO_PORT_TYPE     GPDDAT;         // 0x0C0  R/W  0x00000000   GPD Data Register
    volatile GPIO_PORT_TYPE     GPDEN;          // 0x0C4  R/W  0x00000000   GPD Output Enable Register
    volatile GPIO_PORT_TYPE     GPDSET;         // 0x0C8  W    -            OR function on GPD Output Data
    volatile GPIO_PORT_TYPE     GPDCLR;         // 0x0CC  W    -            BIC function on GPD Output Data
    volatile GPIO_PORT_TYPE     GPDXOR;         // 0x0D0  W    -            XOR function on GPD Output Data
    volatile GPIO_CD0_TYPE      GPDCD0;         // 0x0D4  R/W  0x00000000   Driver strength Control 0 on GPD
    volatile GPIO_CD1_TYPE      GPDCD1;         // 0x0D8  R/W  0x00000000   Driver strength Control 0 on GPD
    volatile GPIO_PORT_TYPE     GPDPE;          // 0x0DC  R/W  0x00000000   Pull-Up/Down Enable function on GPD
    volatile GPIO_PORT_TYPE     GPDPS;          // 0x0E0  R/W  0x00000000   Pull-Up/Down Selection function on GPD
    volatile GPIO_PORT_TYPE     GPDIEN;         // 0x0E4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPD
    volatile GPIO_PORT_TYPE     GPDIS;          // 0x0E8  R/W  0x00000000   Schimitt input Function on GPD
    volatile GPIO_PORT_TYPE     GPDSR;          // 0x0EC  R/W  0x00000000   Fast Slew Rate Function GPD
    volatile GPIO_FN0_TYPE      GPDFN0;         // 0x0F0  R/W  0x00000000   Port Configuration on GPD Output Data
    volatile GPIO_FN1_TYPE      GPDFN1;         // 0x0F4  R/W  0x00000000   Port Configuration on GPD Output Data
    volatile GPIO_FN2_TYPE      GPDFN2;         // 0x0F8  R/W  0x00000000   Port Configuration on GPD Output Data
    volatile GPIO_FN3_TYPE      GPDFN3;         // 0x0FC  R/W  0x00000000   Port Configuration on GPD Output Data

    volatile GPIO_PORT_TYPE     GPEDAT;         // 0x100  R/W  0x00000000   GPE Data Register
    volatile GPIO_PORT_TYPE     GPEEN;          // 0x104  R/W  0x00000000   GPE Output Enable Register
    volatile GPIO_PORT_TYPE     GPESET;         // 0x108  W    -            OR function on GPE Output Data
    volatile GPIO_PORT_TYPE     GPECLR;         // 0x10C  W    -            BIC function on GPE Output Data
    volatile GPIO_PORT_TYPE     GPEXOR;         // 0x110  W    -            XOR function on GPE Output Data
    volatile GPIO_CD0_TYPE      GPECD0;         // 0x114  R/W  0x00000000   Driver strength Control 0 on GPE
    volatile GPIO_CD1_TYPE      GPECD1;         // 0x118  R/W  0x00000000   Driver strength Control 0 on GPE
    volatile GPIO_PORT_TYPE     GPEPE;          // 0x11C  R/W  0x00000000   Pull-Up/Down Enable function on GPE
    volatile GPIO_PORT_TYPE     GPEPS;          // 0x120  R/W  0x00000000   Pull-Up/Down Selection function on GPE
    volatile GPIO_PORT_TYPE     GPEIEN;         // 0x124  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPE
    volatile GPIO_PORT_TYPE     GPEIS;          // 0x128  R/W  0x00000000   Schimitt input Function on GPE
    volatile GPIO_PORT_TYPE     GPESR;          // 0x12C  R/W  0x00000000   Fast Slew Rate Function GPE
    volatile GPIO_FN0_TYPE      GPEFN0;         // 0x130  R/W  0x00000000   Port Configuration on GPE Output Data
    volatile GPIO_FN1_TYPE      GPEFN1;         // 0x134  R/W  0x00000000   Port Configuration on GPE Output Data
    volatile GPIO_FN2_TYPE      GPEFN2;         // 0x138  R/W  0x00000000   Port Configuration on GPE Output Data
    volatile GPIO_FN3_TYPE      GPEFN3;         // 0x13C  R/W  0x00000000   Port Configuration on GPE Output Data

    volatile GPIO_PORT_TYPE     GPFDAT;         // 0x140  R/W  0x00000000   GPF Data Register
    volatile GPIO_PORT_TYPE     GPFEN;          // 0x144  R/W  0x00000000   GPF Output Enable Register
    volatile GPIO_PORT_TYPE     GPFSET;         // 0x148  W    -            OR function on GPF Output Data
    volatile GPIO_PORT_TYPE     GPFCLR;         // 0x14C  W    -            BIC function on GPF Output Data
    volatile GPIO_PORT_TYPE     GPFXOR;         // 0x150  W    -            XOR function on GPF Output Data
    volatile GPIO_CD0_TYPE      GPFCD0;         // 0x154  R/W  0x00000000   Driver strength Control 0 on GPF
    volatile GPIO_CD1_TYPE      GPFCD1;         // 0x158  R/W  0x00000000   Driver strength Control 0 on GPF
    volatile GPIO_PORT_TYPE     GPFPE;          // 0x15C  R/W  0x00000000   Pull-Up/Down Enable function on GPF
    volatile GPIO_PORT_TYPE     GPFPS;          // 0x160  R/W  0x00000000   Pull-Up/Down Selection function on GPF
    volatile GPIO_PORT_TYPE     GPFIEN;         // 0x164  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPF
    volatile GPIO_PORT_TYPE     GPFIS;          // 0x168  R/W  0x00000000   Schimitt input Function on GPF
    volatile GPIO_PORT_TYPE     GPFSR;          // 0x16C  R/W  0x00000000   Fast Slew Rate Function GPF
    volatile GPIO_FN0_TYPE      GPFFN0;         // 0x170  R/W  0x00000000   Port Configuration on GPF Output Data
    volatile GPIO_FN1_TYPE      GPFFN1;         // 0x174  R/W  0x00000000   Port Configuration on GP Output Data
    volatile GPIO_FN2_TYPE      GPFFN2;         // 0x178  R/W  0x00000000   Port Configuration on GPF Output Data
    volatile GPIO_FN3_TYPE      GPFFN3;         // 0x17C  R/W  0x00000000   Port Configuration on GPF Output Data

    volatile GPIO_PORT_TYPE     GPGDAT;         // 0x180  R/W  0x00000000   GPG Data Register
    volatile GPIO_PORT_TYPE     GPGEN;          // 0x184  R/W  0x00000000   GPG Output Enable Register
    volatile GPIO_PORT_TYPE     GPGSET;         // 0x188  W    -            OR function on GPG Output Data
    volatile GPIO_PORT_TYPE     GPGCLR;         // 0x18C  W    -            BIC function on GPG Output Data
    volatile GPIO_PORT_TYPE     GPGXOR;         // 0x190  W    -            XOR function on GPG Output Data
    volatile GPIO_CD0_TYPE      GPGCD0;         // 0x194  R/W  0x00000000   Driver strength Control 0 on GPG
    volatile GPIO_CD1_TYPE      GPGCD1;         // 0x198  R/W  0x00000000   Driver strength Control 0 on GPG
    volatile GPIO_PORT_TYPE     GPGPE;          // 0x19C  R/W  0x00000000   Pull-Up/Down Enable function on GPG
    volatile GPIO_PORT_TYPE     GPGPS;          // 0x1A0  R/W  0x00000000   Pull-Up/Down Selection function on GPG
    volatile GPIO_PORT_TYPE     GPGIEN;         // 0x1A4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPG
    volatile GPIO_PORT_TYPE     GPGIS;          // 0x1A8  R/W  0x00000000   Schimitt input Function on GPG
    volatile GPIO_PORT_TYPE     GPGSR;          // 0x1AC  R/W  0x00000000   Fast Slew Rate Function GPG
    volatile GPIO_FN0_TYPE      GPGFN0;         // 0x1B0  R/W  0x00000000   Port Configuration on GPG Output Data
    volatile GPIO_FN1_TYPE      GPGFN1;         // 0x1B4  R/W  0x00000000   Port Configuration on GPG Output Data
    volatile GPIO_FN2_TYPE      GPGFN2;         // 0x1B8  R/W  0x00000000   Port Configuration on GPG Output Data
    volatile GPIO_FN3_TYPE      GPGFN3;         // 0x1BC  R/W  0x00000000   Port Configuration on GPG Output Data

    volatile GPIO_PORT_TYPE     GPHDMIDAT;      // 0x1C0  R/W  0x00000000   GPHDMI Data Register
    volatile GPIO_PORT_TYPE     GPHDMIEN;       // 0x1C4  R/W  0x00000000   GPHDMI Output Enable Register
    volatile GPIO_PORT_TYPE     GPHDMISET;      // 0x1C8  W    -            OR function on GPHDMI Output Data
    volatile GPIO_PORT_TYPE     GPHDMICLR;      // 0x1CC  W    -            BIC function on GPHDMI Output Data
    volatile GPIO_PORT_TYPE     GPHDMIXOR;      // 0x1D0  W    -            XOR function on GPHDMI Output Data
    volatile GPIO_CD0_TYPE      GPHDMICD;       // 0x1D4  R/W  0x00000000   Driver strength Control on GPHDMI
    unsigned :32;
    volatile GPIO_PORT_TYPE     GPHDMIPE;       // 0x1DC  R/W  0x00000000   Pull-Up/Down Enable function on GPHDMI
    volatile GPIO_PORT_TYPE     GPHDMIPS;       // 0x1E0  R/W  0x00000000   Pull-Up/Down Selection function on GPHDMI
    volatile GPIO_PORT_TYPE     GPHDMIIEN;      // 0x1E4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPHDMI
    volatile GPIO_PORT_TYPE     GPHDMIIS;       // 0x1E8  R/W  0x00000000   Schimitt input Function on GPHDMI
    volatile GPIO_PORT_TYPE     GPHDMISR;       // 0x1EC  R/W  0x00000000   Fast Slew Rate Function GPHDMI
    volatile HDMI_FN_TYPE       GPHDMIFN0;      // 0x1F0  R/W  0x00000000   Port Configuration on GPHDMI Output Data
    unsigned :32; unsigned :32; unsigned :32;

    volatile GPIO_PORT_TYPE     GPADCDAT;       // 0x200  R/W  0x00000000   GPADC Data Register
    volatile GPIO_PORT_TYPE     GPADCEN;        // 0x204  R/W  0x00000000   GPADC Output Enable Register
    volatile GPIO_PORT_TYPE     GPADCSET;       // 0x208  W    -            OR function on GPADC Output Data
    volatile GPIO_PORT_TYPE     GPADCCLR;       // 0x20C  W    -            BIC function on GPADC Output Data
    volatile GPIO_PORT_TYPE     GPADCXOR;       // 0x210  W    -            XOR function on GPADC output Data
    volatile GPIO_CD0_TYPE      GPADCCD;        // 0x214  R/W  0x00000000   Driver strength Control on GPADC
    unsigned :32;
    volatile GPIO_PORT_TYPE     GPADCPE;        // 0x21C  R/W  0x00000000   Pull-Up/Down Enable function on GPADC
    volatile GPIO_PORT_TYPE     GPADCPS;        // 0x220  R/W  0x00000000   Pull-Up/Down Selection function on GPADC
    volatile GPIO_PORT_TYPE     GPADCIEN;       // 0x224  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPADC
    volatile GPIO_PORT_TYPE     GPADCIS;        // 0x228  R/W  0x00000000   Schimitt Input Function on GPADC
    volatile GPIO_PORT_TYPE     GPADCSR;        // 0x22C  R/W  0x00000000   Fast Slew Rate Function on GPADC
    volatile ADC_FN_TYPE        GPADCFN0;       // 0x230  R/W  0x00000000   Port Configuration on GPADC Output Data
    unsigned :32; unsigned :32; unsigned :32;

    volatile EINTSEL0_TYPE      EINTSEL0;       // 0x240  R/W  -            External Interrupt Select Register 0
    volatile EINTSEL1_TYPE      EINTSEL1;       // 0x244  R/W  -            External Interrupt Select Register 1
    volatile EINTSEL2_TYPE      EINTSEL2;       // 0x248  R/W  -            External Interrupt Select Register 2
    unsigned :32;

    volatile unsigned long      ECID0;          // 0x250  R/W  -            CID Output Register
    volatile unsigned long      ECID1;          // 0x254  R    -            CID Serial output data Register
    volatile unsigned long      ECID2;          // 0x258  R    -            CID Parallel output data 0 Register
    volatile unsigned long      ECID3;          // 0x25C  R    -            CID Parallel output data 1 Register
} GPIO, *PGPIO;

typedef struct _GPION{
    volatile GPIO_PORT_TYPE     GPDAT;          // 0x000  R/W  0x00000000   GP Data Register
    volatile GPIO_PORT_TYPE     GPEN;           // 0x004  R/W  0x00000000   GP Output Enable Register
    volatile GPIO_PORT_TYPE     GPSET;          // 0x008  W    -            OR function on GP Output Data
    volatile GPIO_PORT_TYPE     GPCLR;          // 0x00C  W    -            BIC function on GP Output Data
    volatile GPIO_PORT_TYPE     GPXOR;          // 0x010  W    -            XOR function on GP Output Data
    volatile GPIO_CD0_TYPE      GPCD0;          // 0x014  R/W  0x00000000   Driver strength Control on GP
    volatile GPIO_CD1_TYPE      GPCD1;          // 0x018  R/W  0x00000000   Driver strength Control on GP
    volatile GPIO_PORT_TYPE     GPPE;           // 0x01C  R/W  0x00000000   Pull-Up/Down Enable function on GP
    volatile GPIO_PORT_TYPE     GPPS;           // 0x020  R/W  0x00000000   Pull-Up/Down Selection function on GP
    volatile GPIO_PORT_TYPE     GPIEN;          // 0x024  R/W  0xFFFFFFFF   Input Buffer Enable Function on GP
    volatile GPIO_PORT_TYPE     GPIS;           // 0x028  R/W  0x00000000   Schimitt input Function on GP
    volatile GPIO_PORT_TYPE     GPSR;           // 0x02C  R/W  0x00000000   Fast Slew Rate Function GP
    volatile GPIO_FN0_TYPE      GPFN0;          // 0x030  R/W  0x00000000   Port Configuration on GP Output Data
    volatile GPIO_FN1_TYPE      GPFN1;          // 0x034  R/W  0x00000000   Port Configuration on GP Output Data
    volatile GPIO_FN2_TYPE      GPFN2;          // 0x038  R/W  0x00000000   Port Configuration on GP Output Data
    volatile GPIO_FN3_TYPE      GPFN3;          // 0x03C  R/W  0x00000000   Port Configuration on GP Output Data
} GPION, *PGPION;


/************************************************************************
*   7. SMU Config                                (Base Addr = 0xF05F4000)
************************************************************************/

typedef struct {
    unsigned CKC            :1;
    unsigned TIMER          :1;
    unsigned PMU            :1;
    unsigned RTC            :1;
    unsigned I2C            :1;
    unsigned                :27;
} SMU_CONFIG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMU_CONFIG_IDX_TYPE     bREG;
} SMU_CONFIG_TYPE;

typedef struct {
    unsigned PLL0_FIN       :2;
    unsigned PLL1_FIN       :2;
    unsigned PLL2_FIN       :2;
    unsigned PLL3_FIN       :2;
    unsigned PLL4_FIN       :2;
    unsigned                :22;
} CFG_PLLFIN_SEL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CFG_PLLFIN_SEL_IDX_TYPE bREG;
} CFG_PLLFIN_SEL_TYPE;

typedef struct {
    unsigned RTC_WAIT       :3;
    unsigned                :29;
} CFG_RTC_WAIT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CFG_RTC_WAIT_IDX_TYPE   bREG;
} CFG_RTC_WAIT_TYPE;

typedef struct _SMUCONFIG{
    volatile SMU_CONFIG_TYPE        HCLKMASK;       // 0x000  R/W  0            Module Clock Mask Register
    volatile SMU_CONFIG_TYPE        SWRESET;        // 0x004  R/W  0            Module Software Reset Register
    volatile CFG_PLLFIN_SEL_TYPE    CFG_PLLFIN_SEL; // 0x008  R/W  0            PLL FIN Source Selection Register
    volatile unsigned long          CFG_RTC_WAIT;   // 0x00C  R/W  0            RTC Bus Interface Wait Control Register
} SMUCONFIG, *PSMUCONFIG;







/************************************************************************
*   etc.
************************************************************************/

typedef struct _NAND_PIC{
    volatile unsigned int   IEN0;               // 0x000  R/W  0x00000000   Interrupt Enable0 Register
    volatile unsigned int   IEN1;               // 0x004  R/W  0x00000000   Interrupt Enable1 Register
    volatile unsigned int   CLR0;               // 0x008  R/W  0x00000000   Interrupt Clear0 Register
    volatile unsigned int   CLR1;               // 0x00C  R/W  0x00000000   Interrupt Clear1 Register
    volatile unsigned int   STS0;               // 0x010  R    Unknown      Interrupt Status0 Register
    volatile unsigned int   STS1;               // 0x014  R    Unknown      Interrupt Status1 Register
    volatile unsigned int   SEL0;               // 0x018  R/W  0x00000000   IRQ or FIR Selection0 Register
    volatile unsigned int   SEL1;               // 0x01C  R/W  0x00000000   IRQ or FIR Selection1 Register
    volatile unsigned int   SRC0;               // 0x020  R    Unknown      Source Interrupt Status0 Register
    volatile unsigned int   SRC1;               // 0x024  R    Unknown      Source Interrupt Status1 Register
    volatile unsigned int   MSTS0;              // 0x028  R    0x00000000   Masked Status0 Register
    volatile unsigned int   MSTS1;              // 0x02C  R    0x00000000   Masked Status1 Register
    volatile unsigned int   TIG0;               // 0x030  R/W  0x00000000   Test Interrupt Generation0 Register
    volatile unsigned int   TIG1;               // 0x034  R/W  0x00000000   Test Interrupt Generation1 Register
    volatile unsigned int   POL0;               // 0x038  R/W  0x00000000   Interrupt Polarity0 Register
    volatile unsigned int   POL1;               // 0x03C  R/W  0x00000000   Interrupt Polarity1 Register
    volatile unsigned int   IRQ0;               // 0x040  R    0x00000000   IRQ Raw Status0 Register
    volatile unsigned int   IRQ1;               // 0x044  R    0x00000000   IRQ Raw Status1 Register
    volatile unsigned int   FIQ0;               // 0x048  R    Unknown      FIQ Status0 Register
    volatile unsigned int   FIQ1;               // 0x04C  R    Unknown      FIQ Status1 Register
    volatile unsigned int   MIRQ0;              // 0x050  R    0x00000000   Masked IRQ Status0 Register
    volatile unsigned int   MIRQ1;              // 0x054  R    0x00000000   Masked IRQ Status1 Register
    volatile unsigned int   MFIQ0;              // 0x058  R    0x00000000   Masked FIQ Status0 Register
    volatile unsigned int   MFIQ1;              // 0x05C  R    0x00000000   Masked FIQ Status1 Register
    volatile unsigned int   MODE0;              // 0x060  R/W  0x00000000   Trigger Mode0 Register ? Level or Edge
    volatile unsigned int   MODE1;              // 0x064  R/W  0x00000000   Trigger Mode1 Register ? Level or Edge
    volatile unsigned int   SYNC0;              // 0x068  R/W  0xFFFFFFFF   Synchronization Enable0 Register
    volatile unsigned int   SYNC1;              // 0x06C  R/W  0xFFFFFFFF   Synchronization Enable1 Register
    volatile unsigned int   WKEN0;              // 0x070  R/W  0x00000000   Wakeup Event Enable0 Register
    volatile unsigned int   WKEN1;              // 0x074  R/W  0x00000000   Wakeup Event Enable1 Register
    volatile unsigned int   MODEA0;             // 0x078  R/W  0x00000000   Both Edge or Single Edge0 Register
    volatile unsigned int   MODEA1;             // 0x07C  R/W  0x00000000   Both Edge or Single Edge1 Register
    volatile unsigned int   EI37SEL;            // 0x080  R/W  0x00000000   External INT 3~7Selection Register
    volatile unsigned int   NOTDEFINE0[31];     //-  0x84-0xFC              Reserved
    volatile unsigned int   INTMSK0;            // 0x100  R/W  0xFFFFFFFF   Interrupt Output Masking0 Register
    volatile unsigned int   INTMSK1;            // 0x104  R/W  0xFFFFFFFF   Interrupt Output Masking1 Register
    volatile unsigned int   ALLMSK;             // 0x108  R/W  0x00000003   All Mask Register
} NAND_PIC, *NAND_PPIC;


/************************************************************************
*   6. GPIO & Port Multiplexing (Base Addr = 0x74200000)
************************************************************************/
typedef struct _NAND_GPIO{
    volatile unsigned int   GPADAT;             // 0x000  R/W  0x00000000   GPA Data Register 
    volatile unsigned int   GPAEN;              // 0x004  R/W  0x00000000   GPA Output Enable Register 
    volatile unsigned int   GPASET;             // 0x008  W    -            OR function on GPA Output Data 
    volatile unsigned int   GPACLR;             // 0x00C  W    -            BIC function on GPA Output Data 
    volatile unsigned int   GPAXOR;             // 0x010  W    -            XOR function on GPA Output Data 
    volatile unsigned int   GPACD0;             // 0x014  R/W  0x00000000   Driver strength Control 0 on GPA 
    volatile unsigned int   GPACD1;             // 0x018  R/W  0x00000000   Driver strength Control 1 on GPA 
    volatile unsigned int   GPAPE;              // 0x01C  R/W  0x00000000   Pull-Up/Down Enable function on GPA 
    volatile unsigned int   GPAPS;              // 0x020  R/W  0x00000000   Pull-Up/Down Selection function on GPA 
    volatile unsigned int   GPAIEN;             // 0x024  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPA 
    volatile unsigned int   GPAIS;              // 0x028  R/W  0x00000000   Schimitt Input Function on GPA 
    volatile unsigned int   GPASR;              // 0x02C  R/W  0x00000000   Fast Slew Rate Function GPA 
    volatile unsigned int   GPAFN0;             // 0x030  R/W  0x00000000   Port Configuration on GPA Output Data 
    volatile unsigned int   GPAFN1;             // 0x034  R/W  0x00000000   Port Configuration on GPA Output Data 
    volatile unsigned int   GPAFN2;             // 0x038  R/W  0x00000000   Port Configuration on GPA Output Data 
    volatile unsigned int   GPAFN3;             // 0x03C  R/W  0x00000000   Port Configuration on GPA Output Data 
    volatile unsigned int   GPBDAT;             // 0x040  R/W  0x00000000   GPB Data Register 
    volatile unsigned int   GPBEN;              // 0x044  R/W  0x00000000   GPB Output Enable Register 
    volatile unsigned int   GPBSET;             // 0x048  W    -            OR function on GPB Output Data 
    volatile unsigned int   GPBCLR;             // 0x04C  W    -            BIC function on GPB Output Data 
    volatile unsigned int   GPBXOR;             // 0x050  W    -            XOR function on GPB Output Data 
    volatile unsigned int   GPBCD0;             // 0x054  R/W  0x00000000   Driver strength Control 0 on GPB 
    volatile unsigned int   GPBCD1;             // 0x058  R/W  0x00000000   Driver strength Control 1 on GPB 
    volatile unsigned int   GPBPE;              // 0x05C  R/W  0x00000000   Pull-Up/Down Enable function on GPB 
    volatile unsigned int   GPBPS;              // 0x060  R/W  0x00000000   Pull-Up/Down Selection function on GPB 
    volatile unsigned int   GPBIEN;             // 0x064  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPB 
    volatile unsigned int   GPBIS;              // 0x068  R/W  0x00000000   Schimitt Input Function on GPB 
    volatile unsigned int   GPBSR;              // 0x06C  R/W  0x00000000   Fast Slew Rate Function GPB 
    volatile unsigned int   GPBFN0;             // 0x070  R/W  0x00000000   Port Configuration on GPB Output Data 
    volatile unsigned int   GPBFN1;             // 0x074  R/W  0x00000000   Port Configuration on GPB Output Data 
    volatile unsigned int   GPBFN2;             // 0x078  R/W  0x00000000   Port Configuration on GPB Output Data 
    volatile unsigned int   GPBFN3;             // 0x07C  R/W  0x00000000   Port Configuration on GPB Output Data 
    volatile unsigned int   GPCDAT;             // 0x080  R/W  0x00000000   GPC Data Register 
    volatile unsigned int   GPCEN;              // 0x084  R/W  0x00000000   GPC Output Enable Register 
    volatile unsigned int   GPCSET;             // 0x088  W    -            OR function on GPC Output Data 
    volatile unsigned int   GPCCLR;             // 0x08C  W    -            BIC function on GPC Output Data 
    volatile unsigned int   GPCXOR;             // 0x090  W    -            XOR function on GPC Output Data 
    volatile unsigned int   GPCCD0;             // 0x094  R/W  0x00000000   Driver strength Control 0 on GPC 
    volatile unsigned int   GPCCD1;             // 0x098  R/W  0x00000000   Driver strength Control 1 on GPC 
    volatile unsigned int   GPCPE;              // 0x09C  R/W  0x00000000   Pull-Up/Down Enable function on GPC 
    volatile unsigned int   GPCPS;              // 0x0A0  R/W  0x00000000   Pull-Up/Down Selection function on GPC 
    volatile unsigned int   GPCIEN;             // 0x0A4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPC 
    volatile unsigned int   GPCIS;              // 0x0A8  R/W  0x00000000   Schimitt Input Function on GPC 
    volatile unsigned int   GPCSR;              // 0x0AC  R/W  0x00000000   Fast Slew Rate Function GPC 
    volatile unsigned int   GPCFN0;             // 0x0B0  R/W  0x00000000   Port Configuration on GPC Output Data 
    volatile unsigned int   GPCFN1;             // 0x0B4  R/W  0x00000000   Port Configuration on GPC Output Data 
    volatile unsigned int   GPCFN2;             // 0x0B8  R/W  0x00000000   Port Configuration on GPC Output Data 
    volatile unsigned int   GPCFN3;             // 0x0BC  R/W  0x00000000   Port Configuration on GPC Output Data 
    volatile unsigned int   GPDDAT;             // 0x0C0  R/W  0x00000000   GPD Data Register 
    volatile unsigned int   GPDEN;              // 0x0C4  R/W  0x00000000   GPD Output Enable Register 
    volatile unsigned int   GPDSET;             // 0x0C8  W    -            OR function on GPD Output Data 
    volatile unsigned int   GPDCLR;             // 0x0CC  W    -            BIC function on GPD Output Data 
    volatile unsigned int   GPDXOR;             // 0x0D0  W    -            XOR function on GPD Output Data 
    volatile unsigned int   GPDCD0;             // 0x0D4  R/W  0x00000000   Driver strength Control 0 on GPD 
    volatile unsigned int   GPDCD1;             // 0x0D8  R/W  0x00000000   Driver strength Control 1 on GPD 
    volatile unsigned int   GPDPE;              // 0x0DC  R/W  0x00000000   Pull-Up/Down Enable function on GPD 
    volatile unsigned int   GPDPS;              // 0x0E0  R/W  0x00000000   Pull-Up/Down Selection function on GPD 
    volatile unsigned int   GPDIEN;             // 0x0E4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPD 
    volatile unsigned int   GPDIS;              // 0x0E8  R/W  0x00000000   Schimitt Input Function on GPD 
    volatile unsigned int   GPDSR;              // 0x0EC  R/W  0x00000000   Fast Slew Rate Function GPD 
    volatile unsigned int   GPDFN0;             // 0x0F0  R/W  0x00000000   Port Configuration on GPD Output Data 
    volatile unsigned int   GPDFN1;             // 0x0F4  R/W  0x00000000   Port Configuration on GPD Output Data 
    volatile unsigned int   GPDFN2;             // 0x0F8  R/W  0x00000000   Port Configuration on GPD Output Data 
    volatile unsigned int   GPDFN3;             // 0x0FC  R/W  0x00000000   Port Configuration on GPD Output Data 
    volatile unsigned int   GPEDAT;             // 0x100  R/W  0x00000000   GPE Data Register 
    volatile unsigned int   GPEEN;              // 0x104  R/W  0x00000000   GPE Output Enable Register 
    volatile unsigned int   GPESET;             // 0x108  W    -            OR function on GPE Output Data 
    volatile unsigned int   GPECLR;             // 0x10C  W    -            BIC function on GPE Output Data 
    volatile unsigned int   GPEXOR;             // 0x110  W    -            XOR function on GPE Output Data 
    volatile unsigned int   GPECD0;             // 0x114  R/W  0x00000000   Driver strength Control 0 on GPE 
    volatile unsigned int   GPECD1;             // 0x118  R/W  0x00000000   Driver strength Control 1 on GPE 
    volatile unsigned int   GPEPE;              // 0x11C  R/W  0x00000000   Pull-Up/Down Enable function on GPE 
    volatile unsigned int   GPEPS;              // 0x120  R/W  0x00000000   Pull-Up/Down Selection function on GPE 
    volatile unsigned int   GPEIEN;             // 0x124  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPE 
    volatile unsigned int   GPEIS;              // 0x128  R/W  0x00000000   Schimitt Input Function on GPE 
    volatile unsigned int   GPESR;              // 0x12C  R/W  0x00000000   Fast Slew Rate Function GPE 
    volatile unsigned int   GPEFN0;             // 0x130  R/W  0x00000000   Port Configuration on GPE Output Data 
    volatile unsigned int   GPEFN1;             // 0x134  R/W  0x00000000   Port Configuration on GPE Output Data 
    volatile unsigned int   GPEFN2;             // 0x138  R/W  0x00000000   Port Configuration on GPE Output Data 
    volatile unsigned int   GPEFN3;             // 0x13C  R/W  0x00000000   Port Configuration on GPE Output Data 
    volatile unsigned int   GPFDAT;             // 0x140  R/W  0x00000000   GPF Data Register 
    volatile unsigned int   GPFEN;              // 0x144  R/W  0x00000000   GPF Output Enable Register 
    volatile unsigned int   GPFSET;             // 0x148  W    -            OR function on GPF Output Data 
    volatile unsigned int   GPFCLR;             // 0x14C  W    -            BIC function on GPF Output Data 
    volatile unsigned int   GPFXOR;             // 0x150  W    -            XOR function on GPF Output Data 
    volatile unsigned int   GPFCD0;             // 0x154  R/W  0x00000000   Driver strength Control 0 on GPF 
    volatile unsigned int   GPFCD1;             // 0x158  R/W  0x00000000   Driver strength Control 1 on GPF 
    volatile unsigned int   GPFPE;              // 0x15C  R/W  0x00000000   Pull-Up/Down Enable function on GPF 
    volatile unsigned int   GPFPS;              // 0x160  R/W  0x00000000   Pull-Up/Down Selection function on GPF 
    volatile unsigned int   GPFIEN;             // 0x164  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPF 
    volatile unsigned int   GPFIS;              // 0x168  R/W  0x00000000   Schimitt Input Function on GPF 
    volatile unsigned int   GPFSR;              // 0x16C  R/W  0x00000000   Fast Slew Rate Function GPF 
    volatile unsigned int   GPFFN0;             // 0x170  R/W  0x00000000   Port Configuration on GPF Output Data 
    volatile unsigned int   GPFFN1;             // 0x174  R/W  0x00000000   Port Configuration on GPF Output Data 
    volatile unsigned int   GPFFN2;             // 0x178  R/W  0x00000000   Port Configuration on GPF Output Data 
    volatile unsigned int   GPFFN3;             // 0x17C  R/W  0x00000000   Port Configuration on GPF Output Data 
    volatile unsigned int   GPGDAT;             // 0x180  R/W  0x00000000   GPG Data Register 
    volatile unsigned int   GPGEN;              // 0x184  R/W  0x00000000   GPG Output Enable Register 
    volatile unsigned int   GPGSET;             // 0x188  W    -            OR function on GPG Output Data 
    volatile unsigned int   GPGCLR;             // 0x18C  W    -            BIC function on GPG Output Data 
    volatile unsigned int   GPGXOR;             // 0x190  W    -            XOR function on GPG Output Data 
    volatile unsigned int   GPGCD0;             // 0x194  R/W  0x00000000   Driver strength Control 0 on GPG 
    volatile unsigned int   GPGCD1;             // 0x198  R/W  0x00000000   Driver strength Control 1 on GPG 
    volatile unsigned int   GPGPE;              // 0x19C  R/W  0x00000000   Pull-Up/Down Enable function on GPG 
    volatile unsigned int   GPGPS;              // 0x1A0  R/W  0x00000000   Pull-Up/Down Selection function on GPG 
    volatile unsigned int   GPGIEN;             // 0x1A4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPG 
    volatile unsigned int   GPGIS;              // 0x1A8  R/W  0x00000000   Schimitt Input Function on GPG 
    volatile unsigned int   GPGSR;              // 0x1AC  R/W  0x00000000   Fast Slew Rate Function GPG 
    volatile unsigned int   GPGFN0;             // 0x1B0  R/W  0x00000000   Port Configuration on GPG Output Data 
    volatile unsigned int   GPGFN1;             // 0x1B4  R/W  0x00000000   Port Configuration on GPG Output Data 
    volatile unsigned int   GPGFN2;             // 0x1B8  R/W  0x00000000   Port Configuration on GPG Output Data 
    volatile unsigned int   GPGFN3;             // 0x1BC  R/W  0x00000000   Port Configuration on GPG Output Data 
    volatile unsigned int   GPHDMIDAT;          // 0x1C0  R/W  0x00000000   GPHDMI Data Register     
    volatile unsigned int   GPHDMIEN;           // 0x1C4  R/W  0x00000000   GPHDMI Output Enable Register     
    volatile unsigned int   GPHDMISET;          // 0x1C8  W    -            OR function on GPHDMI Output Data     
    volatile unsigned int   GPHDMICLR;          // 0x1CC  W    -            BIC function on GPHDMI Output Data     
    volatile unsigned int   GPHDMIXOR;          // 0x1D0  W    -            XOR function on GPHDMI Output Data     
    volatile unsigned int   GPHDMICD;           // 0x1D4  R/W  0x00000000   Driver strength Control on GPHDMI 
    volatile unsigned int   Reserved0;          //    
    volatile unsigned int   GPHDMIPE;           // 0x1DC  R/W  0x00000000   Pull-Up/Down Enable function on GPHDMI     
    volatile unsigned int   GPHDMIPS;           // 0x1E0  R/W  0x00000000   Pull-Up/Down Selection function on GPHDMI     
    volatile unsigned int   GPHDMIIEN;          // 0x1E4  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPHDMI     
    volatile unsigned int   GPHDMIIS;           // 0x1E8  R/W  0x00000000   Schimitt Input Function on GPHDMI     
    volatile unsigned int   GPHDMISR;           // 0x1EC  R/W  0x00000000   Fast Slew Rate Function GPHDMI     
    volatile unsigned int   GPHDMIFN0;          // 0x1F0  R/W  0x00000000   Port Configuration on GPHDMI Output Data     
    volatile unsigned int   Reserved1[3];       //    
    volatile unsigned int   GPADCDAT;           // 0x200  R/W  0x00000000   GPADC Data Register     
    volatile unsigned int   GPADCEN;            // 0x204  R/W  0x00000000   GPADC Output Enable Register     
    volatile unsigned int   GPADCSET;           // 0x208  W    -            OR function on GPADC Output Data     
    volatile unsigned int   GPADCCLR;           // 0x20C  W    -            BIC function on GPADC Output Data     
    volatile unsigned int   GPADCXOR;           // 0x210  W    -            XOR function on GPADC Output Data     
    volatile unsigned int   GPADCCD;            // 0x214  R/W  0x00000000   Driver strength Control on GPADC     
    volatile unsigned int   Reserved2;          //    
    volatile unsigned int   GPADCPE;            // 0x21C  R/W  0x00000000   Pull-Up/Down Enable function on GPADC     
    volatile unsigned int   GPADCPS;            // 0x220  R/W  0x00000000   Pull-Up/Down Selection function on GPADC     
    volatile unsigned int   GPADCIEN;           // 0x224  R/W  0xFFFFFFFF   Input Buffer Enable Function on GPADC     
    volatile unsigned int   GPADCIS;            // 0x228  R/W  0x00000000   Schimitt Input Function on GPADC     
    volatile unsigned int   GPADCSR;            // 0x22C  R/W  0x00000000   Fast Slew Rate Function GPADC     
    volatile unsigned int   GPADCFN0;           // 0x230  R/W  0x00000000   Port Configuration on GPADC Output Data     
    volatile unsigned int   Reserved3[3];       //     
    volatile unsigned int   EINTSEL0;           // 0x240  R/W               External Interrupt Select Register 0     
    volatile unsigned int   EINTSEL1;           // 0x244  R/W               External Interrupt Select Register 1     
    volatile unsigned int   EINTSEL2;           // 0x248  R/W               External Interrupt Select Register 2     
    volatile unsigned int   Reserved4;          //    
    volatile unsigned int   ECID0;              // 0x250  R/W               CID output Register 
    volatile unsigned int   ECID1;              // 0x254  R                 CID serial output data Register 
    volatile unsigned int   ECID2;              // 0x258  R    -            CID parallel output data 0 Register 
    volatile unsigned int   ECID3;              // 0x25C  R    -            CID parallel output data 1 Register 
} NAND_GPIO, *NAND_PGPIO;                

typedef struct _NAND_GPION{
    volatile unsigned int   GPDAT;              // 0x000  R/W               GPA Data Register
    volatile unsigned int   GPEN;               // 0x004  R/W               GPA Output Enable Register
    volatile unsigned int   GPSET;              // 0x008  W                 OR function on GPA Output Data
    volatile unsigned int   GPCLR;              // 0x00C  W                 BIC function on GPA Output Data
    volatile unsigned int   GPXOR;              // 0x010  W                 XOR function on GPA Output Data
    volatile unsigned int   GPCD0;              // 0x014  W                 Driver strength Control 0 on GPA Output Data
    volatile unsigned int   GPCD1;              // 0x018  W                 Driver strength Control 1 on GPA Output Data
    volatile unsigned int   GPPD0;              // 0x01C  W                 Pull-Up/Down function on GPA Output Data
    volatile unsigned int   GPPD1;              // 0x020  W                 Pull-Up/Down function on GPA Output Data
    volatile unsigned int   GPFN0;              // 0x024  W                 Port Configuration on GPA Output Data
    volatile unsigned int   GPFN1;              // 0x028  W                 Port Configuration on GPA Output Data
    volatile unsigned int   GPFN2;              // 0x02C  W                 Port Configuration on GPA Output Data
    volatile unsigned int   GPFN3;              // 0x030  W                 Port Configuration on GPA Output Data
} NAND_GPION, *NAND_PGPION;

#endif /* _STRUCTURES_SMU_PMU_H_ */
