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


/*
    Not finished block "SD, OVERLAY, AUDIO0/1, NFC, USBOTG"
*/

#ifndef _STRUCTURES_IO_H_
#define _STRUCTURES_IO_H_

/*******************************************************************************
*
*    TCC88x DataSheet PART 5 IO BUS
*
********************************************************************************/

/*******************************************************************************
*   3. SD/SDIO/MMC Host Controller                      (Base Addr = 0x76020000)
********************************************************************************/

typedef struct {
    unsigned SDMA           :32;
} SD_SDMA_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_SDMA_IDX_TYPE        bREG;
} SD_SDMA_TYPE;

typedef struct {
    unsigned BSIZE          :12;
    unsigned SDMABUS        :3;
    unsigned BSIZE2         :1;
} SD_BSIZE_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_BSIZE_IDX_TYPE       bREG;
} SD_BSIZE_TYPE;

typedef struct {
    unsigned BCNT           :16;
} SD_BCNT_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_BCNT_IDX_TYPE        bREG;
} SD_BCNT_TYPE;

typedef struct {
    unsigned ARG            :32;
} SD_ARG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_ARG_IDX_TYPE         bREG;
} SD_ARG_TYPE;

typedef struct {
    unsigned DMAEN          :1;
    unsigned BCNTEN         :1;
    unsigned ACMD12         :1;
    unsigned                :1;
    unsigned DIR            :1;
    unsigned MS             :1;
    unsigned ATACMD         :1;
    unsigned SPI            :1;
    unsigned                :8;
} SD_TMODE_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_TMODE_IDX_TYPE       bREG;
} SD_TMODE_TYPE;

typedef struct {
    unsigned RTYPE          :2;
    unsigned                :1;
    unsigned CRCHK          :1;
    unsigned CICHK          :1;
    unsigned DATSEL         :1;
    unsigned CTYPE          :2;
    unsigned CMDINDEX       :6;
    unsigned                :2;
} SD_CMD_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_CMD_IDX_TYPE         bREG;
} SD_CMD_TYPE;

typedef struct {
    unsigned RESP           :16;
} SD_RESP_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_RESP_IDX_TYPE        bREG;
} SD_RESP_TYPE;

typedef struct {
    unsigned DATA           :16;
} SD_DATA_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_DATA_IDX_TYPE        bREG;
} SD_DATA_TYPE;

typedef struct {
    unsigned NOCMD          :1;
    unsigned NODAT          :1;
    unsigned DATACT         :1;
    unsigned RTREQ          :1;
    unsigned                :4;
    unsigned WRACT          :1;
    unsigned RDACT          :1;
    unsigned WREN           :1;
    unsigned RDEN           :1;
    unsigned                :4;
} SD_STATEL_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_STATEL_IDX_TYPE      bREG;
} SD_STATEL_TYPE;

typedef struct {
    unsigned CDIN           :1;
    unsigned CDST           :1;
    unsigned SDCD           :1;
    unsigned SDWP           :1;
    unsigned DAT            :4;
    unsigned CMD            :1;
    unsigned DAT2           :4;
    unsigned                :3;
} SD_STATEH_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_STATEH_IDX_TYPE      bREG;
} SD_STATEH_TYPE;

typedef struct {
    unsigned LED            :1;
    unsigned SD4            :1;
    unsigned HS             :1;
    unsigned SELDMA         :2;
    unsigned SD8            :1;
    unsigned DETCD          :1;
    unsigned DETSEL         :1;
    unsigned POW            :1;
    unsigned                :3;
    unsigned HRST           :1;
    unsigned                :3;
} SD_CONTL_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_CONTL_IDX_TYPE       bREG;
} SD_CONTL_TYPE;

typedef struct {
    unsigned BGSTOP         :1;
    unsigned CONREQ         :1;
    unsigned RDWAIT         :1;
    unsigned BGINT          :1;
    unsigned SPI            :1;
    unsigned BTEN           :1;
    unsigned ABTEN          :1;
    unsigned                :1;
    unsigned WKINT          :1;
    unsigned WKIN           :1;
    unsigned WKOUT          :1;
    unsigned                :5;
} SD_CONTH_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_CONTH_IDX_TYPE       bREG;
} SD_CONTH_TYPE;

typedef struct {
    unsigned CLKEN          :1;
    unsigned CLKRDY         :1;
    unsigned SCKEN          :1;
    unsigned                :2;
    unsigned CGENSEL        :1;
    unsigned                :2;
    unsigned SDCLKSEL       :8;
} SD_CLK_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_CLK_IDX_TYPE         bREG;
} SD_CLK_TYPE;

typedef struct {
    unsigned TIMEOUT        :4;
    unsigned                :4;
    unsigned RSTALL         :1;
    unsigned RSTCMD         :1;
    unsigned RSTDAT         :1;
    unsigned                :5;
} SD_TIME_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_TIME_IDX_TYPE        bREG;
} SD_TIME_TYPE;

typedef struct {
    unsigned CDONE          :1;
    unsigned TDONE          :1;
    unsigned BLKGAP         :1;
    unsigned DMA            :1;
    unsigned WRRDY          :1;
    unsigned RDRDY          :1;
    unsigned CDIN           :1;
    unsigned CDOUT          :1;
    unsigned CDINT          :1;
    unsigned                :3;
    unsigned RTE            :1;
    unsigned BAR            :1;
    unsigned BEINT          :1;
    unsigned ERR            :1;
} SD_NOR_INT_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_NOR_INT_IDX_TYPE     bREG;
} SD_NOR_INT_TYPE;

typedef struct {
    unsigned CMDTIME        :1;
    unsigned CMDCRC         :1;
    unsigned CMDEND         :1;
    unsigned CINDEX         :1;
    unsigned DATTIME        :1;
    unsigned DATCRC         :1;
    unsigned DATEND         :1;
    unsigned                :1;
    unsigned ACMD12         :1;
    unsigned ADMA           :1;
    unsigned                :2;
    unsigned TRERR          :1;
    unsigned CRR            :1;
    unsigned VEND           :2;
} SD_ERR_INT_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_ERR_INT_IDX_TYPE     bREG;
} SD_ERR_INT_TYPE;

typedef struct {
    unsigned NORUN          :1;
    unsigned TIMEOUT        :1;
    unsigned CRC            :1;
    unsigned ENDBIT         :1;
    unsigned INDEX          :1;
    unsigned                :2;
    unsigned NOCMD          :1;
    unsigned                :8;
} SD_CMD12ERR_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_CMD12ERR_IDX_TYPE    bREG;
} SD_CMD12ERR_TYPE;

typedef struct {
    unsigned UHSSEL         :3;
    unsigned                :3;
    unsigned TUN            :1;
    unsigned SCSEL          :1;
    unsigned                :6;
    unsigned AIEN           :1;
    unsigned PREN           :1;
} SD_CONT2_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_CONT2_IDX_TYPE       bREG;
} SD_CONT2_TYPE;

typedef struct {
    unsigned TIMEOUTCLK     :6;
    unsigned                :1;
    unsigned TUNIT          :1;
    unsigned BASECLK        :8;
    unsigned MAXBLK         :2;
    unsigned EXTBUS         :1;
    unsigned ADMA2          :1;
    unsigned                :1;
    unsigned HS             :1;
    unsigned SDMA           :1;
    unsigned RESUME         :1;
    unsigned V33            :1;
    unsigned V30            :1;
    unsigned V18            :1;
    unsigned                :1;
    unsigned BUS64          :1;
    unsigned AIS            :1;
    unsigned SLTP           :2;
} SD_CAP1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_CAP1_IDX_TYPE        bREG;
} SD_CAP1_TYPE;

typedef struct {
    unsigned SDR50          :1;
    unsigned SDR104         :1;
    unsigned DDR50          :1;
    unsigned                :1;
    unsigned DRTA           :1;
    unsigned DRTC           :1;
    unsigned DRTD           :1;
    unsigned                :1;
    unsigned RTTCNT         :4;
    unsigned                :1;
    unsigned SDR50_2        :1;
    unsigned RTMODE         :2;
    unsigned CLKMULT        :8;
    unsigned SPI            :1;
    unsigned SPIBLK         :1;
    unsigned                :6;
} SD_CAP2_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_CAP2_IDX_TYPE        bREG;
} SD_CAP2_TYPE;

typedef struct {
    unsigned MAXCURV33      :8;
    unsigned MAXCURV30      :8;
    unsigned MAXCURV18      :8;
    unsigned                :8;
} SD_CURRENT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_CURRENT_IDX_TYPE     bREG;
} SD_CURRENT_TYPE;

typedef struct {
    unsigned NORUN          :1;
    unsigned TIMEOUT        :1;
    unsigned CRC            :1;
    unsigned ENDBIT         :1;
    unsigned INDEX          :1;
    unsigned                :2;
    unsigned NOCMD          :1;
    unsigned                :8;
    unsigned CMDTIME        :1;
    unsigned CMDCRC         :1;
    unsigned CMDEND         :1;
    unsigned CINDEX         :1;
    unsigned DATTIME        :1;
    unsigned DATCRC         :1;
    unsigned DATEND         :1;
    unsigned                :1;
    unsigned ACMD12         :1;
    unsigned ADMA           :1;
    unsigned                :2;
    unsigned TRERR          :1;
    unsigned CERR           :1;
    unsigned VEND           :2;
} SD_FORCE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_FORCE_IDX_TYPE       bREG;
} SD_FORCE_TYPE;

typedef struct {
    unsigned ERRSTATE       :2;
    unsigned LEN            :1;
    unsigned                :29;
} SD_ADMAERR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_ADMAERR_IDX_TYPE     bREG;
} SD_ADMAERR_TYPE;

typedef struct {
    unsigned ADDR           :16;
} SD_ADMAADDR_IDX_TYPE;

typedef union {
    unsigned short          nREG;
    SD_ADMAADDR_IDX_TYPE    bREG;
} SD_ADMAADDR_TYPE;

typedef struct {
    unsigned SDCLKFREQ      :10;
    unsigned CG             :1;
    unsigned                :3;
    unsigned DRSTR          :2;
    unsigned SDCLKFREQ2     :10;
    unsigned CG2            :1;
    unsigned                :3;
    unsigned DRSTR2         :2;
} SD_PRESET_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SD_PRESET_IDX_TYPE      bREG;
} SD_PRESET_TYPE;

typedef struct _SDHOST{
    volatile SD_SDMA_TYPE       SDMA;           // 0x000  R/W  0x00000000   SDMA System Address
    volatile SD_BSIZE_TYPE      BSIZE;          // 0x004  R/W  0x0000       Block Size
    volatile SD_BCNT_TYPE       BCNT;           // 0x006  R/W  0x0000       Block Count
    volatile SD_ARG_TYPE        ARG;            // 0x008  R/W  0x00000000   Argument
    volatile SD_TMODE_TYPE      TMODE;          // 0x00C  R/W  0x0000       Transfer Mode
    volatile SD_CMD_TYPE        CMD;            // 0x00E  R/W  0x0000       Command
    volatile SD_RESP_TYPE       RESP0;          // 0x010  R    0x0000       Response0
    volatile SD_RESP_TYPE       RESP1;          // 0x012  R    0x0000       Response1
    volatile SD_RESP_TYPE       RESP2;          // 0x014  R    0x0000       Response2
    volatile SD_RESP_TYPE       RESP3;          // 0x016  R    0x0000       Response3
    volatile SD_RESP_TYPE       RESP4;          // 0x018  R    0x0000       Response4
    volatile SD_RESP_TYPE       RESP5;          // 0x01A  R    0x0000       Response5
    volatile SD_RESP_TYPE       RESP6;          // 0x01C  R    0x0000       Response6
    volatile SD_RESP_TYPE       RESP7;          // 0x01E  R    0x0000       Response7
    volatile SD_DATA_TYPE       DATAL;          // 0x020  R/W  -            Buffer Data Port(Low)
    volatile SD_DATA_TYPE       DATAH;          // 0x022  R/W  -            Buffer Data Port(High)
    volatile SD_STATEL_TYPE     STATEL;         // 0x024  R    0x0000       Present State(Low)
    volatile SD_STATEH_TYPE     STATEH;         // 0x026  R    0x0000       Present State(High)
    volatile SD_CONTL_TYPE      CONTL;          // 0x028  R/W  0x0000       Power Control / Host Control
    volatile SD_CONTH_TYPE      CONTH;          // 0x02A  R/W  0x0000       Wakeup Control / Block Gap Control
    volatile SD_CLK_TYPE        CLK;            // 0x02C  R/W  0x0000       Clock Control
    volatile SD_TIME_TYPE       TIME;           // 0x02E  R/W  0x0000       Software Reset / Timeout Control
    volatile SD_NOR_INT_TYPE    STSL;           // 0x030  R    0x0000       Normal Interrupt Status
    volatile SD_ERR_INT_TYPE    STSH;           // 0x032  R    0x0000       Error Interrupt Status
    volatile SD_NOR_INT_TYPE    STSENL;         // 0x034  R/W  0x0000       Normal Interrupt Status Enable
    volatile SD_ERR_INT_TYPE    STSENH;         // 0x036  R/W  0x0000       Error Interrupt Status Enable
    volatile SD_NOR_INT_TYPE    INTENL;         // 0x038  R/W  0x0000       Normal Interrupt Signal Enable
    volatile SD_ERR_INT_TYPE    INTENH;         // 0x03A  R/W  0x0000       Error Interrupt Signal Enable
    volatile SD_CMD12ERR_TYPE   CMD12ERR;       // 0x03C  R    0x0000       Auto CMD12 Error Status
    volatile SD_CONT2_TYPE      CONT2;          // 0x03E  R/W  0x0000       Host Control2
    volatile SD_CAP1_TYPE       CAP1;           // 0x040  R    0x000030B0   Capabilities1
    volatile SD_CAP2_TYPE       CAP2;           // 0x044  R    0x00000000   Capabilities2
    volatile SD_CURRENT_TYPE    CURR;           // 0x048  R    0x00000000   Maximum Current Capabilities
    unsigned :32;
    volatile SD_FORCE_TYPE      FORCE;          // 0x050  R/W  0x00000000   Force event for AutoCmd12 Error
    volatile SD_ADMAERR_TYPE    ADMAERR;        // 0x054  R/W  0x00000000   AUDIO DMA Error Status
    volatile SD_ADMAADDR_TYPE   ADDR0;          // 0x058  R/W  0x0000       AUDIO DMA Address[15:00]
    volatile SD_ADMAADDR_TYPE   ADDR1;          // 0x05A  R/W  0x0000       AUDIO DMA Address[31:16]
    volatile SD_ADMAADDR_TYPE   ADDR2;          // 0x05C  R/W  0x0000       AUDIO DMA Address[47:32]
    volatile SD_ADMAADDR_TYPE   ADDR3;          // 0x05E  R/W  0x0000       AUDIO DMA Address[63:48]
    volatile SD_PRESET_TYPE     PRESET;         // 0x060  R    0x00000000   Preset Values
    unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF16BIT_TYPE  BTCONTL;        // 0x070  R/W  0x0000       Boot data timeout control(Low)
    volatile TCC_DEF16BIT_TYPE  BTCONTH;        // 0x072  R/W  0x0000       Boot data timeout control(High)
    volatile TCC_DEF16BIT_TYPE  DBGSEL;         // 0x074  R/W  0x0000       Debug Selection
    unsigned :16; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF16BIT_TYPE  SPIINT;         // 0x0F0  R    0x0000       SPI Interrupt Support
    unsigned :16; unsigned :32; unsigned :32;
    volatile TCC_DEF16BIT_TYPE  SLOT;           // 0x0FC  R    0x0000       Slot Interrupt Status
    volatile TCC_DEF16BIT_TYPE  VERSION;        // 0x0FE  R    0x0002       Host Controller Version
} SDHOST, *PSDHOST;

typedef struct _SDCHCTRL{
    volatile TCC_DEF32BIT_TYPE  SDCTRL;         // 0x000  R/W  0x00000000   Host Controller Control Register
    volatile TCC_DEF32BIT_TYPE  SDRESERVED;     // 0x004  R/W  0x00000000   Reserved Register
    volatile TCC_DEF32BIT_TYPE  SD0CMDDAT;      // 0x008  R/W  0x00000000   SD/MMC0 output delay control register
    volatile TCC_DEF32BIT_TYPE  SD1CMDDAT;      // 0x00C  R/W  0x00000000   SD/MMC1 output delay control register
    volatile TCC_DEF32BIT_TYPE  SD2CMDDAT;      // 0x010  R/W  0x00000000   SD/MMC2 output delay control register
    volatile TCC_DEF32BIT_TYPE  SD3CMDDAT;      // 0x014  R/W  0x00000000   SD/MMC3 output delay control register
    volatile TCC_DEF32BIT_TYPE  SD0CAPREG0;     // 0x018  R/W  0x00000000   SD/MMC0 Capabilities 0 register
    volatile TCC_DEF32BIT_TYPE  SD0CAPREG1;     // 0x01C  R/W  0x00000000   SD/MMC0 Capabilities 1 register
    volatile TCC_DEF32BIT_TYPE  SD0INITSPD;     // 0x020  R/W  0x00000000   SD/MMC0 Initialization & Default Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD0HIGHSPD;     // 0x024  R/W  0x00000000   SD/MMC0 High Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD0PRESET5;     // 0x028  R/W  0x00000000   SD/MMC0 Preset Register5
    volatile TCC_DEF32BIT_TYPE  SD0PRESET6;     // 0x02C  R/W  0x00000000   SD/MMC0 Preset Register6
    volatile TCC_DEF32BIT_TYPE  SD1CAPREG0;     // 0x030  R/W  0x00000000   SD/MMC1 Capabilities 0 register
    volatile TCC_DEF32BIT_TYPE  SD1CAPREG1;     // 0x034  R/W  0x00000000   SD/MMC1 Capabilities 1 register
    volatile TCC_DEF32BIT_TYPE  SD1INITSPD;     // 0x038  R/W  0x00000000   SD/MMC1 Initialization & Default Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD1HIGHSPD;     // 0x03C  R/W  0x00000000   SD/MMC1 High Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD1PRESET5;     // 0x040  R/W  0x00000000   SD/MMC1 Preset Register5
    volatile TCC_DEF32BIT_TYPE  SD1PRESET6;     // 0x044  R/W  0x00000000   SD/MMC1 Preset Register6
    volatile TCC_DEF32BIT_TYPE  SD2CAPREG0;     // 0x048  R/W  0x00000000   SD/MMC2 Capabilities 0 register
    volatile TCC_DEF32BIT_TYPE  SD2CAPREG1;     // 0x04C  R/W  0x00000000   SD/MMC2 Capabilities 1 register
    volatile TCC_DEF32BIT_TYPE  SD2INITSPD;     // 0x050  R/W  0x00000000   SD/MMC2 Initialization & Default Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD2HIGHSPD;     // 0x054  R/W  0x00000000   SD/MMC2 High Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD2PRESET5;     // 0x058  R/W  0x00000000   SD/MMC2 Preset Register5
    volatile TCC_DEF32BIT_TYPE  SD2PRESET6;     // 0x05C  R/W  0x00000000   SD/MMC2 Preset Register6
    volatile TCC_DEF32BIT_TYPE  SD3CAPREG0;     // 0x060  R/W  0x00000000   SD/MMC3 Capabilities 0 register
    volatile TCC_DEF32BIT_TYPE  SD3CAPREG1;     // 0x064  R/W  0x00000000   SD/MMC3 Capabilities 1 register
    volatile TCC_DEF32BIT_TYPE  SD3INITSPD;     // 0x068  R/W  0x00000000   SD/MMC3 Initialization & Default Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD3HIGHSPD;     // 0x06C  R/W  0x00000000   SD/MMC3 High Speed Config register
    volatile TCC_DEF32BIT_TYPE  SD3PRESET5;     // 0x070  R/W  0x00000000   SD/MMC3 Preset Register5
    volatile TCC_DEF32BIT_TYPE  SD3PRESET6;     // 0x074  R/W  0x00000000   SD/MMC3 Preset Register6
    volatile TCC_DEF32BIT_TYPE  SD01DELAY;      // 0x078  R/W  0x00000000   SD/MMC0/1 Clock delay controller
    volatile TCC_DEF32BIT_TYPE  SD23DELAY;      // 0x07C  R/W  0x00000000   SD/MMC2/3 Clock delay controller
} SDCHCTRL, *PSDCHCTRL;


/*******************************************************************************
*   4. EHI                                              (Base Addr = 0x76000000)
********************************************************************************/

typedef struct {
    unsigned ST             :7;
    unsigned RDY            :1;
    unsigned FOR            :1;
    unsigned FUR            :1;
    unsigned WFF            :1;
    unsigned WFNE           :1;
    unsigned RFNE           :1;
    unsigned                :19;
} EHI_STATUS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_STATUS_IDX_TYPE     bREG;
} EHI_STATUS_TYPE;

typedef struct {
    unsigned IRQ            :1;
    unsigned IRQ_ST         :7;
    unsigned                :24;
} EHI_INT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_INT_IDX_TYPE        bREG;
} EHI_INT_TYPE;

typedef struct {
    unsigned                :2;
    unsigned EHA            :30;
} EHI_ADDR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_ADDR_IDX_TYPE       bREG;
} EHI_ADDR_TYPE;

typedef struct {
    unsigned EHD            :32;
} EHI_DATA_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_DATA_IDX_TYPE       bREG;
} EHI_DATA_TYPE;

typedef struct {
    unsigned FLG            :2;
    unsigned ST             :6;
    unsigned                :24;
} EHI_SEMAPHORE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_SEMAPHORE_IDX_TYPE  bREG;
} EHI_SEMAPHORE_TYPE;

typedef struct {
    unsigned MD             :1;
    unsigned BW             :2;
    unsigned RDYE           :1;
    unsigned RDYP           :1;
    unsigned                :1;
    unsigned CSIRQ          :1;
    unsigned WIRQ           :1;
    unsigned                :24;
} EHI_CONFIG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_CONFIG_IDX_TYPE     bREG;
} EHI_CONFIG_TYPE;

typedef struct {
    unsigned EHIND          :8;
    unsigned                :24;
} EHI_INDEX_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_INDEX_IDX_TYPE      bREG;
} EHI_INDEX_TYPE;

typedef struct {
    unsigned BSIZE          :4;
    unsigned RW             :2;
    unsigned LK_            :1;
    unsigned AI             :1;
    unsigned                :24;
} EHI_RWCONTROL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EHI_RWCONTROL_IDX_TYPE  bREG;
} EHI_RWCONTROL_TYPE;

typedef struct _EHI{
    volatile EHI_STATUS_TYPE    EHST;           // 0x000  R/W  0x00000080   Status register
    volatile EHI_INT_TYPE       EHIINT;         // 0x004  R/W  0x00000000   Internal interrupt control register
    volatile EHI_INT_TYPE       EHEINT;         // 0x008  R/W  0x00000000   External interrupt control register
    volatile EHI_ADDR_TYPE      EHA;            // 0x00C  R    0x00000000   Address register
    volatile EHI_ADDR_TYPE      EHAM;           // 0x010  R/W  0x00000000   Address masking register
    volatile EHI_DATA_TYPE      EHD;            // 0x014  R/W  0x00000000   Data register
    volatile EHI_SEMAPHORE_TYPE EHSEM;          // 0x018  R/W  0x00000000   Semaphore register
    volatile EHI_CONFIG_TYPE    EHCFG;          // 0x01C  R/W  0x00000000   Configuration registers
    volatile EHI_INDEX_TYPE     EHIND;          // 0x020  R    0x00000000   Index register
    volatile EHI_RWCONTROL_TYPE EHRWCS;         // 0x024  R    0x00000000   Read/Write Control/Status register
} EHI, *PEHI;


/*******************************************************************************
*   5. DMA Controller                                   (Base Addr = 0x76030000)
********************************************************************************/

typedef struct {
    unsigned ADDR           :32;
} GDMA_ADDR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GDMA_ADDR_IDX_TYPE      bREG;
} GDMA_ADDR_TYPE;

typedef struct {
    unsigned INC            :8;
    unsigned MASK           :24;
} GDMA_PARAM_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GDMA_PARAM_IDX_TYPE     bREG;
} GDMA_PARAM_TYPE;

typedef struct {
    unsigned COUNT          :32;
} GDMA_COUNT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GDMA_COUNT_IDX_TYPE     bREG;
} GDMA_COUNT_TYPE;

typedef struct {
    unsigned EN             :1;
    unsigned REP            :1;
    unsigned IEN            :1;
    unsigned FLG            :1;
    unsigned WSIZE          :2;
    unsigned BSIZE          :2;
    unsigned TYPE           :2;
    unsigned BST            :1;
    unsigned LOCK           :1;
    unsigned HRD            :1;
    unsigned SYN            :1;
    unsigned DTM            :1;
    unsigned CONT           :1;
    unsigned                :16;
} GDMA_CHCTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GDMA_CHCTRL_IDX_TYPE    bREG;
} GDMA_CHCTRL_TYPE;

typedef struct {
    unsigned RPTCNT         :24;
    unsigned                :6;
    unsigned EOT            :1;
    unsigned DRI            :1;
} GDMA_RPTCTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GDMA_RPTCTRL_IDX_TYPE   bREG;
} GDMA_RPTCTRL_TYPE;

typedef struct {
    unsigned GPSB3_TX       :1;        // 0
    unsigned GPSB4_TX       :1;
    unsigned GPSB5_TX       :1;
    unsigned TSIF0          :1;
    unsigned GPSB3_RX       :1;
    unsigned GPSB4_RX       :1;        // 5
    unsigned GPSB5_RX       :1;
    unsigned TSIF1          :1;
    unsigned UART2_TX       :1;
    unsigned UART2_RX       :1;
    unsigned UART3_TX       :1;        // 10
    unsigned UART3_RX       :1;
    unsigned SPDIF1_TX_P    :1;
    unsigned SPDIF1_TX_U    :1;
    unsigned DAI1_TX        :1;
    unsigned DAI1_RX        :1;        // 15
    unsigned                :2;
    unsigned NFC            :1;
    unsigned I2C0_RX        :1;
    unsigned SPDIF0_TX_P    :1;        // 20
    unsigned SPDIF0_TX_U    :1;
    unsigned CDIF0_RX       :1;
    unsigned DAI0_TX        :1;
    unsigned DAI0_RX        :1;
    unsigned I2C0_TX        :1;        // 25
    unsigned UART0_TX       :1;
    unsigned UART0_RX       :1;
    unsigned SPDIF0_RX      :1;
    unsigned UART1_TX       :1;
    unsigned UART1_RX       :1;        // 30
    unsigned TSIF2          :1;
} GDMA_EXTREQ_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GDMA_EXTREQ_IDX_TYPE    bREG;
} GDMA_EXTREQ_TYPE;

typedef struct {
    unsigned FIX            :1;
    unsigned                :3;
    unsigned PRI            :3;
    unsigned                :1;
    unsigned SWP0           :1;
    unsigned SWP1           :1;
    unsigned SWP2           :1;
    unsigned                :5;
    unsigned MIS0           :1;
    unsigned MIS1           :1;
    unsigned MIS2           :1;
    unsigned                :1;
    unsigned IS0            :1;
    unsigned IS1            :1;
    unsigned IS2            :1;
    unsigned                :9;
} GDMA_CHCONFIG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GDMA_CHCONFIG_IDX_TYPE  bREG;
} GDMA_CHCONFIG_TYPE;

typedef struct _GDMACTRL{
    volatile GDMA_ADDR_TYPE     ST_SADR0;       // 0x000  R/W  0x00000000   Start Address of Source Block
    volatile GDMA_PARAM_TYPE    SPARAM0;        // 0x004  R/W  0x00000000   Parameter of Source Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_SADR0;        // 0x00C  R    0x00000000   Current Address of Source Block
    volatile GDMA_ADDR_TYPE     ST_DADR0;       // 0x010  R/W  0x00000000   Start Address of Destination Block
    volatile GDMA_PARAM_TYPE    DPARAM0;        // 0x014  R/W  0x00000000   Parameter of Destination Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_DADR0;        // 0x01C  R    0x00000000   Current Address of Destination Block
    volatile GDMA_COUNT_TYPE    HCOUNT0;        // 0x020  R/W  0x00000000   Initial and Current Hop count
    volatile GDMA_CHCTRL_TYPE   CHCTRL0;        // 0x024  R/W  0x00000000   Channel Control Register
    volatile GDMA_RPTCTRL_TYPE  RPTCTRL0;       // 0x028  R/W  0x00000000   Repeat Control Register
    volatile GDMA_EXTREQ_TYPE   EXTREQ0;        // 0x02C  R/W  0x00000000   External DMA Request Register
    volatile GDMA_ADDR_TYPE     ST_SADR1;       // 0x030  R/W  0x00000000   Start Address of Source Block
    volatile GDMA_PARAM_TYPE    SPARAM1;        // 0x034  R/W  0x00000000   Parameter of Source Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_SADR1;        // 0x03C  R    0x00000000   Current Address of Source Block
    volatile GDMA_ADDR_TYPE     ST_DADR1;       // 0x040  R/W  0x00000000   Start Address of Destination Block
    volatile GDMA_PARAM_TYPE    DPARAM1;        // 0x044  R/W  0x00000000   Parameter of Destination Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_DADR1;        // 0x04C  R    0x00000000   Current Address of Destination Block
    volatile GDMA_COUNT_TYPE    HCOUNT1;        // 0x050  R/W  0x00000000   Initial and Current Hop count
    volatile GDMA_CHCTRL_TYPE   CHCTRL1;        // 0x054  R/W  0x00000000   Channel Control Register
    volatile GDMA_RPTCTRL_TYPE  RPTCTRL1;       // 0x058  R/W  0x00000000   Repeat Control Register
    volatile GDMA_EXTREQ_TYPE   EXTREQ1;        // 0x05C  R/W  0x00000000   External DMA Request Register
    volatile GDMA_ADDR_TYPE     ST_SADR2;       // 0x060  R/W  0x00000000   Start Address of Source Block
    volatile GDMA_PARAM_TYPE    SPARAM2;        // 0x064  R/W  0x00000000   Parameter of Source Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_SADR2;        // 0x06C  R    0x00000000   Current Address of Source Block
    volatile GDMA_ADDR_TYPE     ST_DADR2;       // 0x070  R/W  0x00000000   Start Address of Destination Block
    volatile GDMA_PARAM_TYPE    DPARAM2;        // 0x074  R/W  0x00000000   Parameter of Destination Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_DADR2;        // 0x07C  R    0x00000000   Current Address of Destination Block
    volatile GDMA_COUNT_TYPE    HCOUNT2;        // 0x080  R/W  0x00000000   Initial and Current Hop count
    volatile GDMA_CHCTRL_TYPE   CHCTRL2;        // 0x084  R/W  0x00000000   Channel Control Register
    volatile GDMA_RPTCTRL_TYPE  RPTCTRL2;       // 0x088  R/W  0x00000000   Repeat Control Register
    volatile GDMA_EXTREQ_TYPE   EXTREQ2;        // 0x08C  R/W  0x00000000   External DMA Request Register
    volatile GDMA_CHCONFIG_TYPE CHCONFIG;       // 0x090  R/W  0x00000000   Channel Configuration Register
} GDMACTRL, *PGDMACTRL;

typedef struct _GDMANCTRL{
    volatile GDMA_ADDR_TYPE     ST_SADR;        // 0x000  R/W  0x00000000   Start Address of Source Block
    volatile GDMA_PARAM_TYPE    SPARAM;         // 0x004  R/W  0x00000000   Parameter of Source Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_SADR;         // 0x00C  R    0x00000000   Current Address of Source Block
    volatile GDMA_ADDR_TYPE     ST_DADR;        // 0x010  R/W  0x00000000   Start Address of Destination Block
    volatile GDMA_PARAM_TYPE    DPARAM;         // 0x014  R/W  0x00000000   Parameter of Destination Block
    unsigned :32;
    volatile GDMA_ADDR_TYPE     C_DADR;         // 0x01C  R    0x00000000   Current Address of Destination Block
    volatile GDMA_COUNT_TYPE    HCOUNT;         // 0x020  R/W  0x00000000   Initial and Current Hop count
    volatile GDMA_CHCTRL_TYPE   CHCTRL;         // 0x024  R/W  0x00000000   Channel Control Register
    volatile GDMA_RPTCTRL_TYPE  RPTCTRL;        // 0x028  R/W  0x00000000   Repeat Control Register
    volatile GDMA_EXTREQ_TYPE   EXTREQ;         // 0x02C  R/W  0x00000000   External DMA Request Register
} GDMANCTRL, *PGDMANCTRL;


/*******************************************************************************
*   6. GPSB                                             (Base Addr = 0x76900000)
********************************************************************************/

typedef struct {
    unsigned DATA           :32;
} GPSB_PORT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_PORT_IDX_TYPE      bREG;
} GPSB_PORT_TYPE;

typedef struct {
    unsigned RTH            :1;
    unsigned WTH            :1;
    unsigned RNE            :1;
    unsigned WE             :1;
    unsigned RF             :1;
    unsigned ROR            :1;
    unsigned WUR            :1;
    unsigned RUR            :1;
    unsigned WOR            :1;
    unsigned                :7;
    unsigned RBVCNT         :4;
    unsigned                :4;
    unsigned WBVCNT         :4;
    unsigned                :4;
} GPSB_STATUS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_STATUS_IDX_TYPE    bREG;
} GPSB_STATUS_TYPE;

typedef struct {
    unsigned IRQEN          :9;
    unsigned                :6;
    unsigned RC             :1;
    unsigned CFGRTH         :3;
    unsigned                :1;
    unsigned CFGWTH         :3;
    unsigned                :1;
    unsigned SBR            :1;
    unsigned SHR            :1;
    unsigned SBT            :1;
    unsigned SHT            :1;
    unsigned                :2;
    unsigned DR             :1;
    unsigned DW             :1;
} GPSB_INTEN_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_INTEN_IDX_TYPE     bREG;
} GPSB_INTEN_TYPE;

typedef struct {
    unsigned MD             :2;
    unsigned SLV            :1;
    unsigned EN             :1;
    unsigned CTF            :1;
    unsigned SDO            :1;
    unsigned LB             :1;
    unsigned SD             :1;
    unsigned BWS            :5;
    unsigned                :1;
    unsigned CWF            :1;
    unsigned CRF            :1;
    unsigned PCK            :1;
    unsigned PRD            :1;
    unsigned PWD            :1;
    unsigned PCD            :1;
    unsigned PCS            :1;
    unsigned TSU            :1;
    unsigned THL            :1;
    unsigned TRE            :1;
    unsigned DIVLDV         :8;
} GPSB_MODE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_MODE_IDX_TYPE      bREG;
} GPSB_MODE_TYPE;

typedef struct {
    unsigned PSW            :5;
    unsigned                :2;
    unsigned PLW            :1;
    unsigned RDSTART        :5;
    unsigned                :3;
    unsigned CMDSTART       :5;
    unsigned                :3;
    unsigned CMDEND         :5;
    unsigned                :1;
    unsigned LCR            :1;
    unsigned LCW            :1;
} GPSB_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_CTRL_IDX_TYPE      bREG;
} GPSB_CTRL_TYPE;

typedef struct {
    unsigned TXCV           :16;
    unsigned                :11;
    unsigned EXTDPOL        :1;
    unsigned EXTDCHK        :1;
    unsigned EXTEN          :1;
    unsigned TXCREP         :1;
    unsigned TXCRX          :1;
} GPSB_EVTCTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_EVTCTRL_IDX_TYPE   bREG;
} GPSB_EVTCTRL_TYPE;

typedef struct {
    unsigned TXC            :16;
    unsigned                :15;
    unsigned FSDI           :1;
} GPSB_CCV_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_CCV_IDX_TYPE       bREG;
} GPSB_CCV_TYPE;

typedef struct {
    unsigned TX_BASE        :32;
} GPSB_TX_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_TX_IDX_TYPE        bREG;
} GPSB_TX_TYPE;

typedef struct {
    unsigned RX_BASE        :32;
} GPSB_RX_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_RX_IDX_TYPE        bREG;
} GPSB_RX_TYPE;

typedef struct {
    unsigned SIZE           :13;
    unsigned                :3;
    unsigned COUNT          :13;
    unsigned                :3;
} GPSB_PACKET_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_PACKET_IDX_TYPE    bREG;
} GPSB_PACKET_TYPE;

typedef struct {
    unsigned EN             :1;
    unsigned TSIF           :1;
    unsigned PCLR           :1;
    unsigned                :1;
    unsigned MD             :2;
    unsigned                :8;
    unsigned RXAM           :2;
    unsigned TXAM           :2;
    unsigned MS             :1;
    unsigned MP             :1;
    unsigned                :8;
    unsigned END            :1;
    unsigned CT             :1;
    unsigned DRE            :1;
    unsigned DTE            :1;
} GPSB_DMACTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_DMACTRL_IDX_TYPE   bREG;
} GPSB_DMACTRL_TYPE;

typedef struct {
    unsigned TXPCNT         :13;
    unsigned                :4;
    unsigned RXPCNT         :13;
    unsigned                :2;
} GPSB_DMASTAT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_DMASTAT_IDX_TYPE   bREG;
} GPSB_DMASTAT_TYPE;

typedef struct {
    unsigned IRQPCNT        :13;
    unsigned                :3;
    unsigned IEP            :1;
    unsigned IED            :1;
    unsigned                :2;
    unsigned IRQS           :1;
    unsigned                :7;
    unsigned ISP            :1;
    unsigned ISD            :1;
    unsigned                :2;
} GPSB_DMAICR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_DMAICR_IDX_TYPE    bREG;
} GPSB_DMAICR_TYPE;

typedef struct _GPSB{
    volatile GPSB_PORT_TYPE     PORT;           // 0x000  R/W  0x00000000   Data port
    volatile GPSB_STATUS_TYPE   STAT;           // 0x004  R/W  0x00000000   Status register
    volatile GPSB_INTEN_TYPE    INTEN;          // 0x008  R/W  0x00000000   Interrupt enable
    volatile GPSB_MODE_TYPE     MODE;           // 0x00C  R/W  0x00000004   Mode register
    volatile GPSB_CTRL_TYPE     CTRL;           // 0x010  R/W  0x00000000   Control register
    volatile GPSB_EVTCTRL_TYPE  EVTCTRL;        // 0x014  R/W  0x00000000   Counter & Ext. Event Control
    volatile GPSB_CCV_TYPE      CCV;            // 0x018  R    0x00000000   Counter Current Value
    unsigned :32;
    volatile GPSB_TX_TYPE       TXBASE;         // 0x020  R/W  0x00000000   TX base address register
    volatile GPSB_RX_TYPE       RXBASE;         // 0x024  R/W  0x00000000   RX base address register
    volatile GPSB_PACKET_TYPE   PACKET;         // 0x028  R/W  0x00000000   Packet register
    volatile GPSB_DMACTRL_TYPE  DMACTR;         // 0x02C  R/W  0x00000000   DMA control register
    volatile GPSB_DMASTAT_TYPE  DMASTR;         // 0x030  R/W  0x00000000   DMA status register
    volatile GPSB_DMAICR_TYPE   DMAICR;         // 0x034  R/W  0x00000000   DMA interrupt control register
} GPSB, *PGPSB, sHwGPSB;

typedef struct _NONDMA_GPSB{
    volatile GPSB_PORT_TYPE     PORT;           // 0x000  R/W  0x00000000   Data port
    volatile GPSB_STATUS_TYPE   STAT;           // 0x004  R/W  0x00000000   Status register
    volatile GPSB_INTEN_TYPE    INTEN;          // 0x008  R/W  0x00000000   Interrupt enable
    volatile GPSB_MODE_TYPE     MODE;           // 0x00C  R/W  0x00000004   Mode register
    volatile GPSB_CTRL_TYPE     CTRL;           // 0x010  R/W  0x00000000   Control register
    volatile GPSB_EVTCTRL_TYPE  EVTCTRL;        // 0x014  R/W  0x00000000   Counter & Ext. Event Control
    volatile GPSB_CCV_TYPE      CCV;            // 0x018  R    0x00000000   Counter Current Value
} NONDMA_GPSB, *PNONDMA_GPSB;

typedef struct {
    unsigned CH0            :8;
    unsigned CH1            :8;
    unsigned CH2            :8;
    unsigned CH3            :8;
} GPSB_PCFG0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_PCFG0_IDX_TYPE     bREG;
} GPSB_PCFG0_TYPE;

typedef struct {
    unsigned CH4            :8;
    unsigned CH5            :8;
    unsigned                :16;
} GPSB_PCFG1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_PCFG1_IDX_TYPE     bREG;
} GPSB_PCFG1_TYPE;

typedef struct {
    unsigned ISTC0          :1;
    unsigned ISTD0          :1;
    unsigned ISTC1          :1;
    unsigned ISTD1          :1;
    unsigned ISTC2          :1;
    unsigned ISTD2          :1;
    unsigned ISTC3          :1;
    unsigned                :1;
    unsigned ISTC4          :1;
    unsigned                :1;
    unsigned ISTC5          :1;
    unsigned                :21;
} GPSB_CIRQST_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_CIRQST_IDX_TYPE    bREG;
} GPSB_CIRQST_TYPE;

typedef struct _GPSBPORTCFG{
    volatile GPSB_PCFG0_TYPE    PCFG0;          // 0x800  R/W  0x03020100   Port Configuration Register 0
    volatile GPSB_PCFG1_TYPE    PCFG1;          // 0x800  R/W  0x00000504   Port Configuration Register 1
    unsigned :32;
    volatile GPSB_CIRQST_TYPE   CIRQST;         // 0x80C  R    0x00000000   Channel IRQ Status Register
} GPSBPORTCFG, *PGPSBPORTCFG;

typedef struct {
    unsigned PID            :13;
    unsigned                :16;
    unsigned CH0            :1;
    unsigned CH1            :1;
    unsigned CH2            :1;
} GPSB_PIDTBL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPSB_PIDTBL_IDX_TYPE    bREG;
} GPSB_PIDTBL_TYPE;

typedef struct _GPSBPIDTABLE{
    volatile GPSB_PIDTBL_TYPE   PIDT[0x80/4];   // 0xF00  R/W  -            PID Table
} GPSBPIDTABLE, *PGPSBPIDTABLE;


/*******************************************************************************
*   7. Overlay Mixer                                    (Base Addr = 0x76040000)
********************************************************************************/

typedef struct _OVERLAYMIXER{
    volatile TCC_DEF32BIT_TYPE  FCH0_SADDR0;    // 0x000  R/W  0x00000000   Front-End Channel 0 Source Address 0
    volatile TCC_DEF32BIT_TYPE  FCH0_SADDR1;    // 0x004  R/W  0x00000000   Front-End Channel 0 Source Address 1
    volatile TCC_DEF32BIT_TYPE  FCH0_SADDR2;    // 0x008  R/W  0x00000000   Front-End Channel 0 Source Address 2
    volatile TCC_DEF32BIT_TYPE  FCH0_SFSIZE;    // 0x00C  R/W  0x00000000   Front-End Channel 0 Source Frame Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH0_SOFF;      // 0x010  R/W  0x00000000   Front-End Channel 0 Source Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH0_SISIZE;    // 0x014  R/W  0x00000000   Front-End Channel 0 Source Image Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH0_WOFF;      // 0x018  R/W  0x00000000   Front-End Channel 0 Window Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH0_SCTRL;     // 0x01C  R/W  0x00000000   Front-End Channel 0 Control

    volatile TCC_DEF32BIT_TYPE  FCH1_SADDR0;    // 0x020  R/W  0x00000000   Front-End Channel 1 Source Address 0
    volatile TCC_DEF32BIT_TYPE  FCH1_SADDR1;    // 0x024  R/W  0x00000000   Front-End Channel 1 Source Address 1
    volatile TCC_DEF32BIT_TYPE  FCH1_SADDR2;    // 0x028  R/W  0x00000000   Front-End Channel 1 Source Address 2
    volatile TCC_DEF32BIT_TYPE  FCH1_SFSIZE;    // 0x02C  R/W  0x00000000   Front-End Channel 1 Source Frame Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH1_SOFF;      // 0x030  R/W  0x00000000   Front-End Channel 1 Source Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH1_SISIZE;    // 0x034  R/W  0x00000000   Front-End Channel 1 Source Image Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH1_WOFF;      // 0x038  R/W  0x00000000   Front-End Channel 1 Window Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH1_SCTRL;     // 0x03C  R/W  0x00000000   Front-End Channel 1 Control

    volatile TCC_DEF32BIT_TYPE  FCH2_SADDR0;    // 0x040  R/W  0x00000000   Front-End Channel 2 Source Address 0
    volatile TCC_DEF32BIT_TYPE  FCH2_SADDR1;    // 0x044  R/W  0x00000000   Front-End Channel 2 Source Address 1
    volatile TCC_DEF32BIT_TYPE  FCH2_SADDR2;    // 0x048  R/W  0x00000000   Front-End Channel 2 Source Address 2
    volatile TCC_DEF32BIT_TYPE  FCH2_SFSIZE;    // 0x04C  R/W  0x00000000   Front-End Channel 2 Source Frame Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH2_SOFF;      // 0x050  R/W  0x00000000   Front-End Channel 2 Source Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH2_SISIZE;    // 0x054  R/W  0x00000000   Front-End Channel 2 Source Image Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH2_WOFF;      // 0x058  R/W  0x00000000   Front-End Channel 2 Window Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH2_SCTRL;     // 0x05C  R/W  0x00000000   Front-End Channel 2 Control

    volatile TCC_DEF32BIT_TYPE  FCH3_SADDR0;    // 0x060  R/W  0x00000000   Front-End Channel 3 Source Address 0
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  FCH3_SFSIZE;    // 0x06C  R/W  0x00000000   Front-End Channel 3 Source Frame Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH3_SOFF;      // 0x070  R/W  0x00000000   Front-End Channel 3 Source Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH3_SISIZE;    // 0x074  R/W  0x00000000   Front-End Channel 3 Source Image Pixel Size
    volatile TCC_DEF32BIT_TYPE  FCH3_WOFF;      // 0x078  R/W  0x00000000   Front-End Channel 3 Window Pixel Offset
    volatile TCC_DEF32BIT_TYPE  FCH3_SCTRL;     // 0x07C  R/W  0x00000000   Front-End Channel 3 Control

    volatile TCC_DEF32BIT_TYPE  S0_CHROMA;      // 0x080  R/W  0x00000000   Source 0 Chroma-Key Parameter
    volatile TCC_DEF32BIT_TYPE  S0_PAR;         // 0x084  R/W  0x00000000   Source 0 Arithmetic Parameter
    volatile TCC_DEF32BIT_TYPE  S1_CHROMA;      // 0x088  R/W  0x00000000   Source 1 Chroma-Key Parameter
    volatile TCC_DEF32BIT_TYPE  S1_PAR;         // 0x08C  R/W  0x00000000   Source 1 Arithmetic Parameter
    volatile TCC_DEF32BIT_TYPE  S2_CHROMA;      // 0x090  R/W  0x00000000   Source 2 Chroma-Key Parameter
    volatile TCC_DEF32BIT_TYPE  S2_PAR;         // 0x094  R/W  0x00000000   Source 2 Arithmetic Parameter
    volatile TCC_DEF32BIT_TYPE  S3_CHROMA;      // 0x098  R/W  0x00000000   Source 3 Chroma-Key Parameter
    volatile TCC_DEF32BIT_TYPE  S3_PAR;         // 0x09C  R/W  0x00000000   Source 3 Arithmetic Parameter
    volatile TCC_DEF32BIT_TYPE  SF_CTRL;        // 0x0A0  R/W  0x00000000   Source Function Control Register
    volatile TCC_DEF32BIT_TYPE  SA_CTRL;        // 0x0A4  R/W  0x00000000   Source Arithmetic Control Register
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  OP0_ALPHA;      // 0x0B0  R/W  0x00000000   Source Operator 0 Alpha
    volatile TCC_DEF32BIT_TYPE  OP1_ALPHA;      // 0x0B4  R/W  0x00000000   Source Operator 1 Alpha
    volatile TCC_DEF32BIT_TYPE  OP2_ALPHA;      // 0x0B4  R/W  0x00000000   Source Operator 2 Alpha
    unsigned :32;
    volatile TCC_DEF32BIT_TYPE  OP0_PAT;        // 0x0C0  R/W  0x00000000   Source Operator 0 Pattern
    volatile TCC_DEF32BIT_TYPE  OP1_PAT;        // 0x0C4  R/W  0x00000000   Source Operator 1 Pattern
    volatile TCC_DEF32BIT_TYPE  OP2_PAT;        // 0x0C8  R/W  0x00000000   Source Operator 2 Pattern
    unsigned :32;
    volatile TCC_DEF32BIT_TYPE  OP0_CTRL;       // 0x0D0  R/W  0x00000000   Source Operation 0 Control Register
    volatile TCC_DEF32BIT_TYPE  OP1_CTRL;       // 0x0D4  R/W  0x00000000   Source Operation 1 Control Register
    volatile TCC_DEF32BIT_TYPE  OP2_CTRL;       // 0x0D8  R/W  0x00000000   Source Operation 2 Control Register
    unsigned :32;
    volatile TCC_DEF32BIT_TYPE  BCH_DADDR0;     // 0x0E0  R/W  0x00000000   Back-End Channel Destination Address 0
    volatile TCC_DEF32BIT_TYPE  BCH_DADDR1;     // 0x0E4  R/W  0x00000000   Back-End Channel Destination Address 1
    volatile TCC_DEF32BIT_TYPE  BCH_DADDR2;     // 0x0E8  R/W  0x00000000   Back-End Channel Destination Address 2
    volatile TCC_DEF32BIT_TYPE  BCH_DFSIZE;     // 0x0EC  R/W  0x00000000   Back-End Channel Destination Frame Pixel Size
    volatile TCC_DEF32BIT_TYPE  BCH_DOFF;       // 0x0F0  R/W  0x00000000   Back-End Channel Destination Pixel Offset
    volatile TCC_DEF32BIT_TYPE  BCH_DCTRL;      // 0x0F4  R/W  0x00000000   Back-End Channel Control
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  BCH_DDMAT0;     // 0x100  R/W  0x00000000   Back-End Channel Destination Dither Matrix 0
    volatile TCC_DEF32BIT_TYPE  BCH_DDMAT1;     // 0x104  R/W  0x00000000   Back-End Channel Destination Dither Matrix 1
    volatile TCC_DEF32BIT_TYPE  BCH_DDMAT2;     // 0x108  R/W  0x00000000   Back-End Channel Destination Dither Matrix 2
    volatile TCC_DEF32BIT_TYPE  BCH_DDMAT3;     // 0x10C  R/W  0x00000000   Back-End Channel Destination Dither Matrix 3
    volatile TCC_DEF32BIT_TYPE  OM_CTRL;        // 0x110  R/W  0x00000000   Overlay Mixer Control
    volatile TCC_DEF32BIT_TYPE  OM_IREQ;        // 0x114  R/W  0x00000000   Overlay Mixer Interrupt Request
    unsigned :32; unsigned :32;
    TCC_DEF32BIT_TYPE           NOTDEFINED[184];  // 0x120 - 0x3FF //kch 120->184
    volatile TCC_DEF32BIT_TYPE  FCH0_LUT[256];  // 0x400  R/W  -            Front-End Channel 0 Lookup Table
    volatile TCC_DEF32BIT_TYPE  FCH1_LUT[256];  // 0x800  R/W  -            Front-End Channel 1 Lookup Table
    volatile TCC_DEF32BIT_TYPE  FCH2_LUT[256];  // 0xC00  R/W  -            Front-End Channel 2 Lookup Table
} OVERLAYMIXER, *POVERLAYMIXER;


/*******************************************************************************
*   8. Audio0(7.1ch)                                    (Base Addr = 0x76200000)
********************************************************************************/

typedef struct _ADMA{
	volatile unsigned int	RxDaDar;		// 0x000  R/W  0x00000000	DAI Rx (Right) Data Destination Address
	volatile unsigned int	RxDaParam;		// 0x004  R/W  0x00000000	DAI Rx Parameters
	volatile unsigned int	RxDaTCnt;		// 0x008  R/W  0x00000000	DAI Rx Transmission Counter Register
	volatile unsigned int	RxDaCdar;		// 0x00C  R    0x00000000	DAI Rx (Right) Data Current Destination Address
	volatile unsigned int	RxCdDar;		// 0x010  R/W  0x00000000	CDIF(SPDIF) Rx (Right) Data Destination Address
	volatile unsigned int	RxCdParam;		// 0x014  R/W  0x00000000	CDIF(SPDIF) Rx Parameters
	volatile unsigned int	RxCdTCnt;		// 0x018  R/W  0x00000000	CDIF(SPDIF) Rx Transmission Counter Register
	volatile unsigned int	RxCdCdar;		// 0x01C  R    0x00000000	CDIF(SPDIF) Rx (Right) Data Current Destination Address
	unsigned :32; unsigned :32;
	volatile unsigned int	RxDaDarL;		// 0x028  R/W  0x00000000	DAI Rx Left Data Destination Address
	volatile unsigned int	RxDaCdarL;		// 0x02C  R    0x00000000	DAI Rx Left Data Current Destination Address
	volatile unsigned int	RxCdDarL;		// 0x030  R/W  0x00000000	CDIF(SPDIF) Rx Left Data Destination Address
	volatile unsigned int	RxCdCdarL;		// 0x034  R    0x00000000	CDIF(SPDIF) Rx Left Data Current Destination Address
	volatile unsigned int	TransCtrl;		// 0x038  R/W  0x0000AA00	DMA Transfer Control Register
	volatile unsigned int	RptCtrl;		// 0x03C  R/W  0x00000000	DMA Repeat Control Register
	volatile unsigned int	TxDaSar;		// 0x040  R/W  0x00000000	DAI Tx (Right) Data Source Address
	volatile unsigned int	TxDaParam;		// 0x044  R/W  0x00000000	DAI Tx Parameters
	volatile unsigned int	TxDaTCnt;		// 0x048  R/W  0x00000000	DAI Tx Transmission Counter Register
	volatile unsigned int	TxDaCsar;		// 0x04C  R    0x00000000	DAI Tx (Right) Data Current Source Address
	volatile unsigned int	TxSpSar;		// 0x050  R/W  0x00000000	SPDIF Tx (Right) Data Source Address
	volatile unsigned int	TxSpParam;		// 0x054  R/W  0x00000000	SPDIF Tx Parameters
	volatile unsigned int	TxSpTCnt;		// 0x058  R/W  0x00000000	SPDIF Tx Transmission Counter Register
	volatile unsigned int	TxSpCsar;		// 0x05C  R    0x00000000	SPDIF Tx (Right) Data Current Source Address
	unsigned :32; unsigned :32;
	volatile unsigned int	TxDaSarL;		// 0x068  R/W  0x00000000	DAI Tx Left Data Source Address
	volatile unsigned int	TxDaCsarL;		// 0x06C  R    0x00000000	DAI Tx Left Data Current Source Address
	volatile unsigned int	TxSpSarL;		// 0x070  R/W  0x00000000	SPDIF Tx Left Data Source Address
	volatile unsigned int	TxSpCsarL;		// 0x074  R    0x00000000	SPDIF Tx Left Data Current Source address
	volatile unsigned int	ChCtrl;			// 0x078  R/W  0x00008000	DMA Channel Control Register
	volatile unsigned int	IntStatus;		// 0x07C  R/W  0x00000000	DMA Interrupt Status Register
	volatile unsigned int	GIntReq;		// 0x080  R/W  0x00000000	General Interrupt Request
	volatile unsigned int	GIntStatus;		// 0x084  R    0x00000000	General Interrupt Status
	unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile unsigned int	RxDaDar1;		// 0x100  R/W  0x00000000	DAI1 Rx (Right) Data Destination Address
	volatile unsigned int	RxDaDar2;		// 0x104  R/W  0x00000000	DAI2 Rx (Right) Data Destination Address
	volatile unsigned int	RxDaDar3;		// 0x108  R/W  0x00000000	DAI3 Rx (Right) Data Destination Address
	volatile unsigned int	RxDaCar1;		// 0x10C  R    0x00000000	DAI1 Rx (Right) Data Current Destination Address
	volatile unsigned int	RxDaCar2;		// 0x110  R    0x00000000	DAI2 Rx (Right) Data Current Destination Address
	volatile unsigned int	RxDaCar3;		// 0x114  R    0x00000000	DAI3 Rx (Right) Data Current Destination Address
	volatile unsigned int	RxDaDarL1;		// 0x118  R/W  0x00000000	DAI1 Rx Left Data Destination Address
	volatile unsigned int	RxDaDarL2;		// 0x11C  R/W  0x00000000	DAI2 Rx Left Data Destination Address
	volatile unsigned int	RxDaDarL3;		// 0x120  R/W  0x00000000	DAI3 Rx Left Data Destination Address
	volatile unsigned int	RxDaCarL1;		// 0x124  R    0x00000000	DAI1 Rx Left Data Current Destination Address
	volatile unsigned int	RxDaCarL2;		// 0x128  R    0x00000000	DAI2 Rx Left Data Current Destination Address
	volatile unsigned int	RxDaCarL3;		// 0x12C  R    0x00000000	DAI3 Rx Left Data Current Destination Address
	volatile unsigned int	TxDaSar1;		// 0x130  R/W  0x00000000	DAI1 Tx (Right) Data Source Address
	volatile unsigned int	TxDaSar2;		// 0x134  R/W  0x00000000	DAI2 Tx (Right) Data Source Address
	volatile unsigned int	TxDaSar3;		// 0x138  R/W  0x00000000	DAI3 Tx (Right) Data Source Address
	volatile unsigned int	TxDaCsar1;		// 0x13C  R    0x00000000	DAI1 Tx (Right) Data Current Source Address
	volatile unsigned int	TxDaCsar2;		// 0x140  R    0x00000000	DAI2 Tx (Right) Data Current Source Address
	volatile unsigned int	TxDaCsar3;		// 0x144  R    0x00000000	DAI3 Tx (Right) Data Current Source Address
	volatile unsigned int	TxDaDarL1;		// 0x148  R/W  0x00000000	DAI1 Tx Left Data Source Address
	volatile unsigned int	TxDaDarL2;		// 0x14C  R/W  0x00000000	DAI2 Tx Left Data Source Address
	volatile unsigned int	TxDaDarL3;		// 0x150  R/W  0x00000000	DAI3 Tx Left Data Source Address
	volatile unsigned int	TxDaCarL1;		// 0x154  R    0x00000000	DAI1 Tx Left Data Current Source Address
	volatile unsigned int	TxDaCarL2;		// 0x158  R    0x00000000	DAI2 Tx Left Data Current Source Address
	volatile unsigned int	TxDaCarL3;		// 0x15C  R    0x00000000	DAI3 Tx Left Data Current Source Address
} ADMA, *PADMA;

typedef struct _ADMADAI{
	volatile unsigned int	DADIR0;			// 0x000  R    -			Digital Audio Input Register 0
	volatile unsigned int	DADIR1;			// 0x004  R    -			Digital Audio Input Register 1
	volatile unsigned int	DADIR2;			// 0x008  R    -			Digital Audio Input Register 2
	volatile unsigned int	DADIR3;			// 0x00C  R    -			Digital Audio Input Register 3
	volatile unsigned int	DADIR4;			// 0x010  R    -			Digital Audio Input Register 4
	volatile unsigned int	DADIR5;			// 0x014  R    -			Digital Audio Input Register 5
	volatile unsigned int	DADIR6;			// 0x018  R    -			Digital Audio Input Register 6
	volatile unsigned int	DADIR7;			// 0x01C  R    -			Digital Audio Input Register 7
	volatile unsigned int	DADOR0;			// 0x020  R/W  -			Digital Audio Output Register 0
	volatile unsigned int	DADOR1;			// 0x024  R/W  -			Digital Audio Output Register 1
	volatile unsigned int	DADOR2;			// 0x028  R/W  -			Digital Audio Output Register 2
	volatile unsigned int	DADOR3;			// 0x02C  R/W  -			Digital Audio Output Register 3
	volatile unsigned int	DADOR4;			// 0x030  R/W  -			Digital Audio Output Register 4
	volatile unsigned int	DADOR5;			// 0x034  R/W  -			Digital Audio Output Register 5
	volatile unsigned int	DADOR6;			// 0x038  R/W  -			Digital Audio Output Register 6
	volatile unsigned int	DADOR7;			// 0x03C  R/W  -			Digital Audio Output Register 7
	volatile unsigned int	DAMR;			// 0x040  R/W  0x00000000	Digital Audio Mode Register
	volatile unsigned int	DAVC;			// 0x044  R/W  0x0000		Digital Audio Volume Control Register
	volatile unsigned int	MCCR0;			// 0x048  R/W  0x00000000	Multi Channel Control Register 0
	volatile unsigned int	MCCR1;			// 0x04C  R/W  0x00000000	Multi Channel Control Register 1
} ADMADAI, *PADMADAI;

typedef struct _ADMACDIF{
	volatile unsigned int	CDDI_0;			// 0x080  R    -			CD Digital Audio Input Register 0
	volatile unsigned int	CDDI_1;			// 0x084  R    -			CD Digital Audio Input Register 1
	volatile unsigned int	CDDI_2;			// 0x088  R    -			CD Digital Audio Input Register 2
	volatile unsigned int	CDDI_3;			// 0x08C  R    -			CD Digital Audio Input Register 3
	volatile unsigned int	CDDI_4;			// 0x090  R    -			CD Digital Audio Input Register 4
	volatile unsigned int	CDDI_5;			// 0x094  R    -			CD Digital Audio Input Register 5
	volatile unsigned int	CDDI_6;			// 0x098  R    -			CD Digital Audio Input Register 6
	volatile unsigned int	CDDI_7;			// 0x09C  R    -			CD Digital Audio Input Register 7
	volatile unsigned int	CICR;			// 0x0A0  R/W  0x0000		CD Interface Control Register
} ADMACDIF, *PADMACDIF;

typedef struct _ADMASPDIFTX{
	volatile unsigned int	TxVersion;		// 0x000  R    0x00003111	Version Register
	volatile unsigned int	TxConfig;		// 0x004  R/W  0x00000000	Configuration Register
	volatile unsigned int	TxChStat;		// 0x008  R/W  0x00000000	Channel Status Control Register
	volatile unsigned int	TxIntMask;		// 0x00C  R/W  0x00000000	Interrupt Mask Register
	volatile unsigned int	TxIntStat;		// 0x010  R/W  0x00000000	Interrupt Status Register
	volatile unsigned int	NOTDEF0[27];
	volatile unsigned int	UserData[24];	// 0x080~0x0DC  W  -		User Data Buffer
	volatile unsigned int	NOTDEF1[8];
	volatile unsigned int	ChStatus[24];	// 0x100~0x15C  W  -		Channel Status Buffer
	volatile unsigned int	NOTDEF2[40];
	volatile unsigned int	TxBuffer[16];	// 0x200~0x23C  W  -		Transmit Data Buffer
	volatile unsigned int	NOTDEF3[112];
	volatile unsigned int	DMACFG;			// 0x400  R/W  0x00000007	Additional Configuration for DMA
	volatile unsigned int	NOTDEF4[159]; //kch 287->159
	volatile unsigned int	CSBUDB[24];		// 0x680~0x6DC  W  -		Merged Window for CSB/UDB
} ADMASPDIFTX, *PADMASPDIFTX;

typedef union _RXCAP{
	volatile unsigned int	RxCapCtln[16];		//  0x840~0x87C(even) W -	Channel Status Capture Control Register
	volatile unsigned int	RxCapn[16];			//  0x840~0x87C(odd)  W -	Captured Channel Status / user bit
} RXCAP;

typedef struct _ADMASPDIFRX{
	volatile unsigned int	RxVersion;		// 0x800  R    0x00080111	Version Register
	volatile unsigned int	RxConfig;		// 0x804  R/W  0x00000000	Configuration Register
	volatile unsigned int	RxStatus;		// 0x808  R    0x00000000	Signal Status Buffer
	volatile unsigned int	RxIntMask;		// 0x80C  R/W  0x00000000	Interrupt Mask Register
	volatile unsigned int	RxIntStat;		// 0x810  R/W  0x00000000	Interrupt Status register
	unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile RXCAP				RxCap;			// 0x840~0x870  W  -		RxCapCtl[n] / RxCap[n]
	volatile unsigned int	RxBuffer[8];	// 0xA00~0xA1C  W  -		Receive Data Buffer
} ADMASPDIFRX, *PADMASPDIFRX;


/*******************************************************************************
*   9. Audio1(Stereo)                                   (Base Addr = 0x76100000)
********************************************************************************/

typedef struct _ADMADAI1{
	volatile unsigned int	DADIR0;			// 0x000  R    -			Digital Audio Input Register 0
	volatile unsigned int	DADIR1;			// 0x004  R    -			Digital Audio Input Register 1
	volatile unsigned int	DADIR2;			// 0x008  R    -			Digital Audio Input Register 2
	volatile unsigned int	DADIR3;			// 0x00C  R    -			Digital Audio Input Register 3
	volatile unsigned int	DADIR4;			// 0x010  R    -			Digital Audio Input Register 4
	volatile unsigned int	DADIR5;			// 0x014  R    -			Digital Audio Input Register 5
	volatile unsigned int	DADIR6;			// 0x018  R    -			Digital Audio Input Register 6
	volatile unsigned int	DADIR7;			// 0x01C  R    -			Digital Audio Input Register 7
	volatile unsigned int	DADOR0;			// 0x020  R/W  -			Digital Audio Output Register 0
	volatile unsigned int	DADOR1;			// 0x024  R/W  -			Digital Audio Output Register 1
	volatile unsigned int	DADOR2;			// 0x028  R/W  -			Digital Audio Output Register 2
	volatile unsigned int	DADOR3;			// 0x02C  R/W  -			Digital Audio Output Register 3
	volatile unsigned int	DADOR4;			// 0x030  R/W  -			Digital Audio Output Register 4
	volatile unsigned int	DADOR5;			// 0x034  R/W  -			Digital Audio Output Register 5
	volatile unsigned int	DADOR6;			// 0x038  R/W  -			Digital Audio Output Register 6
	volatile unsigned int	DADOR7;			// 0x03C  R/W  -			Digital Audio Output Register 7
	volatile unsigned int	DAMR;			// 0x040  R/W  0x00000000	Digital Audio Mode Register
	volatile unsigned int	DAVC;			// 0x044  R/W  0x0000		Digital Audio Volume Control Register
} ADMADAI1, *PADMADAI1;


/*******************************************************************************
*   10. NFC                                             (Base Addr = 0x76600000)
********************************************************************************/
#if 0
typedef struct _NFC{
    volatile TCC_DEF32BIT_TYPE  NFC_CMD;        // 0x000  W    -            NAND Flash Command Register
    volatile TCC_DEF32BIT_TYPE  NFC_LADDR;      // 0x004  W    -            NAND Flash Linear Address Register
    volatile TCC_DEF32BIT_TYPE  NFC_SADDR;      // 0x008  W    -            NAND Flash Signal Address Register
    volatile TCC_DEF32BIT_TYPE  NFC_SDATA;      // 0x00C  R/W  -            NAND Flash Single Data Register
    volatile TCC_DEF32BIT_TYPE  NFC_WDATA;      // 0x01x  R/W  -            NAND Flash Word Data Register
    unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  NFC_LDATA;      // 0x02x  R/W  -            NAND Flash Linear Data Register
    unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  NFC_MDATA;      // 0x040  R/W  -            NAND Flash Multiple Data Register
    volatile TCC_DEF32BIT_TYPE  NFC_CACYC;      // 0x044  R/W  0x00010101   NAND Flash Command & Address Cycle Register
    volatile TCC_DEF32BIT_TYPE  NFC_WRCYC;      // 0x048  R/W  0x00010101   NAND Flash Write Cycle Register
    volatile TCC_DEF32BIT_TYPE  NFC_RECYC;      // 0x04C  R/W  0x00010101   NAND Flash Read Cycle Register
    volatile TCC_DEF32BIT_TYPE  NFC_CTRL;       // 0x050  R/W  0xF006C800   NFC Control Register
    volatile TCC_DEF32BIT_TYPE  NFC_DSIZE;      // 0x054  R/W  0x0000FFFF   NFC Data Size Register
    volatile TCC_DEF32BIT_TYPE  NFC_SPARE;      // 0x058  R/W  0x00000200   NFC Spare Burst Transfer Control Register
    volatile TCC_DEF32BIT_TYPE  NFC_PRSTART;    // 0x05C  R/W  0x00000000   NFC Burst Program/Read Start
    volatile TCC_DEF32BIT_TYPE  NFC_RST;        // 0x060  R/W  0x00000000   NFC Reset Register
    volatile TCC_DEF32BIT_TYPE  NFC_IRQCFG;     // 0x064  R/W  0x00000000   NFC Interrupt Request Control Register
    volatile TCC_DEF32BIT_TYPE  NFC_IRQ;        // 0x068  R/W  0x00000000   NFC Interrupt Request Register
    volatile TCC_DEF32BIT_TYPE  NFC_STATUS;     // 0x06C  R/W  -            NFC Stutus Register
    volatile TCC_DEF32BIT_TYPE  NFC_RFWBASE;    // 0x070  R/W  0x00000000   NFC Read Flag Wait Base Word Register
    unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  NDMA_ADDR;      // 0x100  R/W  0x00000000   NFC DMA Source/Destination Register
    volatile TCC_DEF32BIT_TYPE  NDMA_PARAM;     // 0x104  R/W  0x00000000   NFC DMA Parameter Register
    volatile TCC_DEF32BIT_TYPE  NDMA_CADDR;     // 0x108  R    0x00000000   NFC DMA Current Address Register
    volatile TCC_DEF32BIT_TYPE  NDMA_CTRL;      // 0x10C  R/W  0x00000000   NFC DMA Controll Register
    volatile TCC_DEF32BIT_TYPE  NDMA_SPCTRL;    // 0x110  R/W  0x00000000   NFC DMA SUBPAGE Control Regiter
    volatile TCC_DEF32BIT_TYPE  NDMA_STATUS;    // 0x114  R/W  0x00000000   NFC DMA Stutus Register
    unsigned :32; unsigned :32;                 // 0x118~011C
    volatile TCC_DEF32BIT_TYPE  SP_CFG[32];     // 0x120  R/W  0x00000000   NFC Sub Page Configuration Register 0
} NFC, *PNFC;

typedef struct _ECC{
    volatile TCC_DEF32BIT_TYPE  ECC_CTRL;       // 0x200  R/W  0x00000000   ECC Control Register
    volatile TCC_DEF32BIT_TYPE  ERRNUM;         // 0x204  R/W  -            SLC/MLC ECC Error Number Register
    volatile TCC_DEF32BIT_TYPE  ECC_BASE;       // 0x208  R/W  0xF05B0000   ECC Base Address
    volatile TCC_DEF32BIT_TYPE  ECC_MASK;       // 0x20C  R/W  0x00008000   ECC Mask Address
    volatile TCC_DEF32BIT_TYPE  ECC_CLEAR;      // 0x210  R/W  -            NFC ECC Register Clear
    volatile TCC_DEF32BIT_TYPE  ECC_RSTART;     // 0x214  R/W  -            NFC ECC Decoding Start
    volatile TCC_DEF32BIT_TYPE  ECC_STLDCTL;    // 0x218  R/W  -            NFC ECC Store Status and Load Control Register
    volatile TCC_DEF32BIT_TYPE  ECC_STATUS;     // 0x21C  R/W  -            NFC ECC Status Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE0;      // 0x220  R/W  0x00000000   1st  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE1;      // 0x224  R/W  0x00000000   2nd  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE2;      // 0x228  R/W  0x00000000   3rd  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE3;      // 0x22C  R/W  0x00000000   4th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE4;      // 0x230  R/W  0x00000000   5th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE5;      // 0x234  R/W  0x00000000   6th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE6;      // 0x238  R/W  0x00000000   7th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE7;      // 0x23C  R/W  0x00000000   8th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE8;      // 0x240  R/W  0x00000000   9th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE9;      // 0x244  R/W  0x00000000   10th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE10;     // 0x248  R/W  0x00000000   11th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE11;     // 0x24C  R/W  0x00000000   12th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE12;     // 0x250  R/W  0x00000000   13th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE13;     // 0x254  R/W  0x00000000   14th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE14;     // 0x258  R/W  0x00000000   15th  ECC Code Register
    volatile TCC_DEF32BIT_TYPE  ECC_CODE15;     // 0x25C  R/W  0x00000000   16th  ECC Code register
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR0;     // 0x260  R    0x00000000   ECC Error Address Register0
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR1;     // 0x264  R    0x00000000   ECC Error Address Register1
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR2;     // 0x268  R    0x00000000   ECC Error Address Register2
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR3;     // 0x26C  R    0x00000000   ECC Error Address Register3
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR4;     // 0x270  R    0x00000000   ECC Error Address Register4
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR5;     // 0x274  R    0x00000000   ECC Error Address Register5
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR6;     // 0x278  R    0x00000000   ECC Error Address Register6
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR7;     // 0x27C  R    0x00000000   ECC Error Address Register7
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR8;     // 0x280  R    0x00000000   ECC Error Address Register8
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR9;     // 0x284  R    0x00000000   ECC Error Address Register9
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR10;    // 0x288  R    0x00000000   ECC Error Address Register10
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR11;    // 0x28C  R    0x00000000   ECC Error Address Register11
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR12;    // 0x290  R    0x00000000   ECC Error Address Register12
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR13;    // 0x294  R    0x00000000   ECC Error Address Register13
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR14;    // 0x298  R    0x00000000   ECC Error Address Register14
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR15;    // 0x29C  R    0x00000000   ECC Error Address Register15
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR16;    // 0x2A0  R    0x00000000   ECC Error Address Register4
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR17;    // 0x2A4  R    0x00000000   ECC Error Address Register5
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR18;    // 0x2A8  R    0x00000000   ECC Error Address Register6
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR19;    // 0x2AC  R    0x00000000   ECC Error Address Register7
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR20;    // 0x2B0  R    0x00000000   ECC Error Address Register8
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR21;    // 0x2B4  R    0x00000000   ECC Error Address Register9
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR22;    // 0x2B8  R    0x00000000   ECC Error Address Register10
    volatile TCC_DEF32BIT_TYPE  ECC_EADDR23;    // 0x2BC  R    0x00000000   ECC Error Address Register11
    volatile TCC_DEF32BIT_TYPE  NOTDEFINE[80];  // 0x2C0  R    0x00000000   ECC Error Address Register11
    volatile TCC_DEF32BIT_TYPE  NFC_MEMORY[384];// 0x400  R    0x00000000   ECC Error Address Register11
} ECC, *PECC;

#else
typedef struct _NFC{
    volatile unsigned int       NFC_CMD;        // 0x00   W                 NAND Flash Command Register
    volatile unsigned int       NFC_LADDR;      // 0x04   W                 NAND Flash Linear Address Register
    volatile unsigned int       NFC_SADDR;      // 0x08   W                 NAND Flash Signal Address Register
    volatile unsigned int       NFC_SDATA;      // 0x0C   R/W               NAND Flash Single Data Register
    volatile unsigned int       NFC_WDATA;      // 0x1x   R/W               NAND Flash Word Data Register
    volatile unsigned int       NOTDEFINE0[3];
    volatile unsigned int       NFC_LDATA;      // 0x2x/3x R/W      NAND Flash Linear Data Register
    volatile unsigned int       NOTDEFINE1[7];  // 0x01x  R/W       NAND Flash Word Data Register
    volatile unsigned int       NFC_MDATA;      // 0x40     R/W     NAND Flash Multiple Data Register
    volatile unsigned int       NFC_CACYC;      // 0x44     R/W     NAND Flash Command & Address Cycle Register
    volatile unsigned int       NFC_WRCYC;      // 0x48     R/W     NAND Flash Write Cycle Register
    volatile unsigned int       NFC_RECYC;      // 0x4C     R/W     NAND Flash Read Cycle Register
    volatile unsigned int       NFC_CTRL;       // 0x50     R/W     NFC Control Register
    volatile unsigned int       NFC_DSIZE;      // 0x54     R/W     NFC Data Size Register
    volatile unsigned int       NFC_SPARE;      // 0x58     R/W     NFC Spare Burst Transfer Control Register
    volatile unsigned int       NFC_PRSTART;    // 0x5C     R/W     NFC Burst Program/Read Start
    volatile unsigned int       NFC_RST;        // 0X60     R/W     NFC Reset Register
    volatile unsigned int       NFC_IRQCFG;     // 0x64     R/W     NFC Interrupt Request Control Register
    volatile unsigned int       NFC_IRQ;        // 0x68     R/W     NFC Interrupt Request Register
    volatile unsigned int       NFC_STATUS;     // 0x6C     R/W     NFC Stutus Register
    volatile unsigned int       NFC_RFWBASE;    // 0x70     R/W     NFC Read Flag Wait Base Word Register
    volatile unsigned int       NOTDEFINE2[35]; // 0x74~0xFC
    volatile unsigned int       NDMA_ADDR;      // 0X100    R/W     NFC DMA Source/Destination Register
    volatile unsigned int       NDMA_PARAM;     // 0X104    R/W     NFC DMA Parameter Register
    volatile unsigned int       NDMA_CADDR;     // 0X108    R       NFC DMA Current Address Register
    volatile unsigned int       NDMA_CTRL;      // 0x10C    R/W     NFC DMA Controll Register
    volatile unsigned int       NDMA_SPCTRL;    // 0x110    R/W     NFC DMA SUBPAGE Control Regiter
    volatile unsigned int       NDMA_STATUS;    // 0x114    R/W     NFC DMA Stutus Register
    volatile unsigned int       NOTDEFINE3[2];  // 0x118~011C
    volatile unsigned int       SP_CFG[32];     // 0x120    R/W     NFC Sub Page Configuration Register 0    
} NFC, *PNFC;

typedef struct _ECC{
    volatile unsigned int       ECC_CTRL;               // 0x00     W       NAND Flash Command Register
    volatile unsigned int       ERRNUM;                   // 0x04     W       NAND Flash Linear Address Register
    volatile unsigned int       ECC_BASE;                // 0x0C             Reserved 
    volatile unsigned int       ECC_MASK;                // 0x0C             Reserved 
    volatile unsigned int       ECC_CLEAR;
    volatile unsigned int       ECC_RSTART;
    volatile unsigned int       ECC_STLDCTL;
    volatile unsigned int       ECC_STATUS;
    volatile unsigned int       ECC_CODE0;                // 0x220  R/W    1st  ECC Code Register
    volatile unsigned int       ECC_CODE1;                // 0x224  R/W    2nd  ECC Code Register
    volatile unsigned int       ECC_CODE2;                // 0x228  R/W    3rd  ECC Code Register
    volatile unsigned int       ECC_CODE3;                // 0x22C  R/W    4th  ECC Code Register
    volatile unsigned int       ECC_CODE4;                // 0x230  R/W    5th  ECC Code Register
    volatile unsigned int       ECC_CODE5;                // 0x234  R/W    6th  ECC Code Register
    volatile unsigned int       ECC_CODE6;                // 0x238  R/W    7th  ECC Code Register
    volatile unsigned int       ECC_CODE7;                // 0x23C  R/W    8th  ECC Code Register
    volatile unsigned int       ECC_CODE8;                // 0x240  R/W    9th  ECC Code Register
    volatile unsigned int       ECC_CODE9;                // 0x244  R/W    10th  ECC Code Register
    volatile unsigned int       ECC_CODE10;             // 0x248  R/W    11th  ECC Code Register
    volatile unsigned int       ECC_CODE11;             // 0x24C  R/W    12th  ECC Code Register
    volatile unsigned int       ECC_CODE12;             // 0x250  R/W    13th  ECC Code Register
    volatile unsigned int       ECC_CODE13;             // 0x254  R/W    14th  ECC Code Register
    volatile unsigned int       ECC_CODE14;             // 0x258  R/W    15th  ECC Code Register
    volatile unsigned int       ECC_CODE15;             // 0x25C  R/W    16th  ECC Code register
    volatile unsigned int       ECC_EADDR0;             // 0x260  R      ECC Error Address Register0
    volatile unsigned int       ECC_EADDR1;             // 0x264  R      ECC Error Address Register1
    volatile unsigned int       ECC_EADDR2;             // 0x268  R      ECC Error Address Register2
    volatile unsigned int       ECC_EADDR3;             // 0x26C  R      ECC Error Address Register3
    volatile unsigned int       ECC_EADDR4;             // 0x270  R      ECC Error Address Register4
    volatile unsigned int       ECC_EADDR5;             // 0x274  R      ECC Error Address Register5
    volatile unsigned int       ECC_EADDR6;             // 0x278  R      ECC Error Address Register6
    volatile unsigned int       ECC_EADDR7;             // 0x27C  R      ECC Error Address Register7
    volatile unsigned int       ECC_EADDR8;             // 0x280  R      ECC Error Address Register8
    volatile unsigned int       ECC_EADDR9;             // 0x284  R      ECC Error Address Register9
    volatile unsigned int       ECC_EADDR10;            // 0x288  R      ECC Error Address Register10
    volatile unsigned int       ECC_EADDR11;            // 0x28C  R      ECC Error Address Register11
    volatile unsigned int       ECC_EADDR12;            // 0x290  R      ECC Error Address Register12
    volatile unsigned int       ECC_EADDR13;            // 0x294  R      ECC Error Address Register13
    volatile unsigned int       ECC_EADDR14;            // 0x298  R      ECC Error Address Register14
    volatile unsigned int       ECC_EADDR15;            // 0x29C  R      ECC Error Address Register15
    volatile unsigned int       ECC_EADDR16;             // 0x2A0  R      ECC Error Address Register4
    volatile unsigned int       ECC_EADDR17;             // 0x2A4  R      ECC Error Address Register5
    volatile unsigned int       ECC_EADDR18;             // 0x2A8  R      ECC Error Address Register6
    volatile unsigned int       ECC_EADDR19;             // 0x2AC  R      ECC Error Address Register7
    volatile unsigned int       ECC_EADDR20;             // 0x2B0  R      ECC Error Address Register8
    volatile unsigned int       ECC_EADDR21;             // 0x2B4  R      ECC Error Address Register9
    volatile unsigned int       ECC_EADDR22;            // 0x2B8  R      ECC Error Address Register10
    volatile unsigned int       ECC_EADDR23;            // 0x2BC  R      ECC Error Address Register11
    volatile unsigned int       NOTDEFINE[80];          // 0x2C0~0x3FC
    volatile unsigned int       NFC_MEMORY[384];        // 0x400~0x9FF
} ECC, *PECC;
#endif

/*******************************************************************************
*   11. SMC(Static Memory Controller)                   (Base Addr = 0x76060000)
********************************************************************************/

typedef struct {
    unsigned RDY            :1;
    unsigned                :15;
    unsigned CST            :6;
    unsigned                :10;
} SMC_STATUS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMC_STATUS_IDX_TYPE     bREG;
} SMC_STATUS_TYPE;

typedef struct {
    unsigned HLD            :4;
    unsigned PW             :8;
    unsigned STP            :4;
    unsigned                :6;
    unsigned RDY            :1;
    unsigned URDY           :1;
    unsigned MTYPE          :2;
    unsigned MSIZE          :2;
    unsigned AM             :2;
    unsigned PTnWE          :1;
    unsigned PTnOE          :1;
} SMC_CSCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMC_CSCFG_IDX_TYPE      bREG;
} SMC_CSCFG_TYPE;

typedef struct {
    unsigned CS0_OFFSET     :4;
    unsigned                :4;
    unsigned CS1_OFFSET     :4;
    unsigned                :4;
    unsigned CS2_OFFSET     :4;
    unsigned                :4;
    unsigned CS3_OFFSET     :4;
    unsigned                :4;
} SMC_CSNOFFSET_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMC_CSNOFFSET_IDX_TYPE  bREG;
} SMC_CSNOFFSET_TYPE;

typedef struct {
    unsigned                :14;
    unsigned INDIRADDR      :12;
    unsigned                :6;
} SMC_INDIRADDR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    SMC_INDIRADDR_IDX_TYPE  bREG;
} SMC_INDIRADDR_TYPE;

typedef struct _SMC{
    volatile SMC_STATUS_TYPE    STATUS;         // 0x000  R/W  0x00010001   Unknown Status Register
    unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile SMC_CSCFG_TYPE     CSNCFG0;        // 0x020  R/W  0x4B403183   External Chip Select0 Config Register
    volatile SMC_CSCFG_TYPE     CSNCFG1;        // 0x024  R/W  0x4B401104   External Chip Select1 Config Register
    volatile SMC_CSCFG_TYPE     CSNCFG2;        // 0x028  R/W  0x4B404082   External Chip Select2 Config Register
    volatile SMC_CSCFG_TYPE     CSNCFG3;        // 0x02C  R/W  0x4B4020C5   External Chip Select3 Config. Register
    volatile SMC_CSNOFFSET_TYPE CSNOFFSET;      // 0x030  R/W  0x00000000   Wapping Address Mode OFFSET Register
    volatile SMC_INDIRADDR_TYPE INDIRADDR;      // 0x034  R/W  0x00000000   Indirect Address
} SMC, *PSMC;


/*******************************************************************************
*   12. EDI                                             (Base Addr = 0x76065000)
********************************************************************************/

typedef struct {
    unsigned PRIORITY       :5;
    unsigned                :3;
    unsigned AM             :1;
    unsigned                :23;
} EDI_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EDI_CTRL_IDX_TYPE       bREG;
} EDI_CTRL_TYPE;

typedef struct {
    unsigned CFGCS0         :4;
    unsigned CFGCS1         :4;
    unsigned CFGCS2         :4;
    unsigned CFGCS3         :4;
    unsigned CFGCS4         :4;
    unsigned CFGCS5         :4;
    unsigned                :8;
} EDI_CSNCFG0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EDI_CSNCFG0_IDX_TYPE    bREG;
} EDI_CSNCFG0_TYPE;

typedef struct {
    unsigned CFGCS6         :4;
    unsigned CFGCS7         :4;
    unsigned CFGCS8         :4;
    unsigned CFGCS9         :4;
    unsigned CFGCS10        :4;
    unsigned CFGCS11        :4;
    unsigned                :8;
} EDI_CSNCFG1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EDI_CSNCFG1_IDX_TYPE    bREG;
} EDI_CSNCFG1_TYPE;

typedef struct {
    unsigned CFGOEN0        :2;
    unsigned CFGOEN1        :2;
    unsigned                :28;
} EDI_OENCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EDI_OENCFG_IDX_TYPE     bREG;
} EDI_OENCFG_TYPE;

typedef struct {
    unsigned CFGWEN0        :2;
    unsigned CFGWEN1        :2;
    unsigned                :28;
} EDI_WENCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EDI_WENCFG_IDX_TYPE     bREG;
} EDI_WENCFG_TYPE;

typedef struct {
    unsigned CFGRDY0        :3;
    unsigned                :1;
    unsigned CFGRDY1        :3;
    unsigned                :1;
    unsigned CFGRDY2        :3;
    unsigned                :1;
    unsigned CFGRDY3        :3;
    unsigned                :1;
    unsigned CFGRDY4        :3;
    unsigned                :13;
} EDI_RDYCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EDI_RDYCFG_IDX_TYPE     bREG;
} EDI_RDYCFG_TYPE;

typedef struct {
    unsigned REQOFF         :2;
    unsigned                :30;
} EDI_REQOFF_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    EDI_REQOFF_IDX_TYPE     bREG;
} EDI_REQOFF_TYPE;

typedef struct _EDI{
    volatile EDI_CTRL_TYPE      EDI_CTRL;       // 0x000  R/W  0x00000000   EDI Control Register.
    volatile EDI_CSNCFG0_TYPE   EDI_CSNCFG0;    // 0x004  R/W  0x00543210   EDI CSN Configuration Register 0.
    volatile EDI_CSNCFG1_TYPE   EDI_CSNCFG1;    // 0x008  R/W  0x00BA9876   EDI CSN Configuration Register 1.
    volatile EDI_OENCFG_TYPE    EDI_OENCFG;     // 0x00C  R/W  0x00000004   EDI OEN Configuration Register
    volatile EDI_WENCFG_TYPE    EDI_WENCFG;     // 0x010  R/W  0x00000004   EDI WEN Configuration Register
    volatile EDI_RDYCFG_TYPE    EDI_RDYCFG;     // 0x014  R/W  0x00043210   EDI Ready Configuration Register
    unsigned :32; unsigned :32;
    volatile EDI_REQOFF_TYPE    EDI_REQOFF;     // 0x020  R/W  0x00000000   EDI Request OFF Flag Register
} EDI, *PEDI;


/*******************************************************************************
*   13. IDE                                             (Base Addr = 0x76065000)
********************************************************************************/

typedef struct {
    unsigned DATA           :16;
    unsigned                :16;
} IDE_CS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_CS_IDX_TYPE         bREG;
} IDE_CS_TYPE;

typedef struct {
    unsigned RDY            :1;
    unsigned HLD            :6;
    unsigned PW             :6;
    unsigned STP            :4;
    unsigned                :3;
    unsigned MD             :1;
    unsigned SYNC           :2;
    unsigned                :9;
} IDE_PIOCTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_PIOCTRL_IDX_TYPE    bREG;
} IDE_PIOCTRL_TYPE;

typedef struct {
    unsigned UEN            :1;
    unsigned UIO            :1;
    unsigned HT             :1;
    unsigned MS             :1;
    unsigned TS             :1;
    unsigned HTB            :1;
    unsigned                :2;
    unsigned SNH            :3;
    unsigned                :1;
    unsigned MAX3           :2;
    unsigned                :1;
    unsigned AHM            :1;
    unsigned FDV            :7;
    unsigned ITL            :2;
    unsigned OTL            :3;
    unsigned HRTL           :2;
    unsigned                :2;
} IDE_UDMACTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_UDMACTRL_IDX_TYPE   bREG;
} IDE_UDMACTRL_TYPE;

typedef struct {
    unsigned IDE            :1;
    unsigned REP            :1;
    unsigned CON            :1;
    unsigned LOC            :1;
    unsigned                :28;
} IDE_IDMACTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_IDMACTRL_IDX_TYPE   bREG;
} IDE_IDMACTRL_TYPE;

typedef struct {
    unsigned ADDR           :32;
} IDE_DMAADDR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_DMAADDR_IDX_TYPE    bREG;
} IDE_DMAADDR_TYPE;

typedef struct {
    unsigned INC            :8;
    unsigned MASK           :24;
} IDE_DMAPARAM_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_DMAPARAM_IDX_TYPE   bREG;
} IDE_DMAPARAM_TYPE;

typedef struct {
    unsigned OPE            :1;
    unsigned TRE            :1;
    unsigned FEE            :1;
    unsigned URE            :1;
    unsigned                :4;
    unsigned OPS            :1;
    unsigned TRS            :1;
    unsigned FES            :1;
    unsigned URS            :1;
    unsigned                :4;
    unsigned VOE            :5;
    unsigned                :1;
    unsigned VEE            :5;
    unsigned                :2;
    unsigned DT             :1;
    unsigned NWA            :1;
    unsigned UB             :1;
} IDE_INT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_INT_IDX_TYPE        bREG;
} IDE_INT_TYPE;

typedef struct {
    unsigned CNT            :24;
    unsigned                :8;
} IDE_DMATCNT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_DMATCNT_IDX_TYPE    bREG;
} IDE_DMATCNT_TYPE;

typedef struct {
    unsigned DATA           :32;
} IDE_DMADATA_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_DMADATA_IDX_TYPE    bREG;
} IDE_DMADATA_TYPE;

typedef struct {
    unsigned CRC            :15;
    unsigned                :15;
} IDE_DMACRC_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IDE_DMACRC_IDX_TYPE     bREG;
} IDE_DMACRC_TYPE;

typedef struct _IDE{
    volatile IDE_CS_TYPE        CS00;           // 0x000  R/W  -            PIO CS0n Access Register
    volatile IDE_CS_TYPE        CS01;           // 0x004  R/W  -
    volatile IDE_CS_TYPE        CS02;           // 0x008  R/W  -
    volatile IDE_CS_TYPE        CS03;           // 0x00C  R/W  -
    volatile IDE_CS_TYPE        CS04;           // 0x010  R/W  -
    volatile IDE_CS_TYPE        CS05;           // 0x014  R/W  -
    volatile IDE_CS_TYPE        CS06;           // 0x018  R/W  -
    volatile IDE_CS_TYPE        CS07;           // 0x01C  R/W  -
    volatile IDE_CS_TYPE        CS10;           // 0x020  R/W  -            PIO CS1n Access Register
    volatile IDE_CS_TYPE        CS11;           // 0x024  R/W  -
    volatile IDE_CS_TYPE        CS12;           // 0x028  R/W  -
    volatile IDE_CS_TYPE        CS13;           // 0x02C  R/W  -
    volatile IDE_CS_TYPE        CS14;           // 0x030  R/W  -
    volatile IDE_CS_TYPE        CS15;           // 0x034  R/W  -
    volatile IDE_CS_TYPE        CS16;           // 0x038  R/W  -
    volatile IDE_CS_TYPE        CS17;           // 0x03C  R/W  -
    volatile IDE_PIOCTRL_TYPE   PIOCTRL;        // 0x040  R/W  0x00600000   PIO Mode Control Register
    volatile IDE_UDMACTRL_TYPE  UDMACTRL;       // 0x044  R/W  0x00000000   UDMA Mode Control Register
    volatile IDE_IDMACTRL_TYPE  IDMACTRL;       // 0x048  R/W  0x00000000   IDMA Control Register
    volatile IDE_DMAADDR_TYPE   IDMASA;         // 0x04C  R/W  0x00000000   IDMA Source Address Register
    volatile IDE_DMAPARAM_TYPE  IDMASP;         // 0x050  R/W  0x00000000   IDMA Source Parameter Register
    volatile IDE_DMAADDR_TYPE   IDMACSA;        // 0x054  R    0x00000000   IDMA Current Source Address Register
    volatile IDE_DMAADDR_TYPE   IDMADA;         // 0x058  R/W  0x00000000   IDMA Destination Address Register
    volatile IDE_DMAPARAM_TYPE  IDMADP;         // 0x05C  R/W  0x00000000   IDMA Destination Parameter Register
    volatile IDE_DMAADDR_TYPE   IDMACDA;        // 0x060  R    0x00000000   IDMA Current Destination Address Register
    volatile IDE_INT_TYPE       IDEINT;         // 0x064  R/W  0x00000000   IDE Interrupt Register
    volatile IDE_DMATCNT_TYPE   UDMATCNT;       // 0x068  R/W  0x00FFFFFF   UDMA Transfer Counter Register
    volatile IDE_DMADATA_TYPE   UDMAIN;         // 0x06C  R    -            UDMA-IN Access Register
    volatile IDE_DMADATA_TYPE   UDMAOUT;        // 0x070  W    -            UDMA-OUT Access register
    volatile IDE_DMACRC_TYPE    UDMACRC;        // 0x074  R    0x00004ABA   UDMA CRC Register
    volatile IDE_DMATCNT_TYPE   UDMACTCNT;      // 0x078  R    0x00FFFFFF   UDMA Current Transfer Counter Register
} IDE, *PIDE;


/*******************************************************************************
*   14. USB 2.0 OTG Controller                          (Base Addr = 0x76A00000)
********************************************************************************/

typedef struct _USB20OTG{
    // Core Global CSR Map
    volatile unsigned int       GOTGCTL;            // 0x000  R/W   OTG Control and Status Register
    volatile unsigned int       GOTGINT;            // 0x004        OTG Interrupt Register
    volatile unsigned int       GAHBCFG;            // 0x008        Core AHB Configuration Register
    volatile unsigned int       GUSBCFG;            // 0x00C        Core USB Configuration register
    volatile unsigned int       GRSTCTL;            // 0x010        Core Reset Register
    volatile unsigned int       GINTSTS;            // 0x014        Core Interrupt Register
    volatile unsigned int       GINTMSK;            // 0x018        Core Interrupt Mask Register
    volatile unsigned int       GRXSTSR;            // 0x01C        Receive Status Debug Read register(Read Only)
    volatile unsigned int       GRXSTSP;            // 0x020        Receive Status Read /Pop register(Read Only)
    volatile unsigned int       GRXFSIZ;            // 0x024        Receive FIFO Size Register
    volatile unsigned int       GNPTXFSIZ;          // 0x028        Non-periodic Transmit FIFO Size register
    volatile unsigned int       GNPTXSTS;           // 0x02C        Non-periodic Transmit FIFO/Queue Status register (Read Only)
    volatile unsigned int       NOTDEFINE0[3];      // 0x030~       Reserved
    volatile unsigned int       GUID;               // 0x03C        User ID Register
    volatile unsigned int       NOTDEFINE1;         // 0x040        Reserved
    volatile unsigned int       GHWCFG1;            // 0x044        User HW Config1 Register(Read Only)
    volatile unsigned int       GHWCFG2;            // 0x048        User HW Config2 Register(Read Only)
    volatile unsigned int       GHWCFG3;            // 0x04C        User HW Config3 Register(Read Only)
    volatile unsigned int       GHWCFG4;            // 0x050        User HW Config4 Register(Read Only)
    volatile unsigned int       NOTDEFINE2[43];     // 0x054~       Reserved
    volatile unsigned int       HPTXFSIZ;           // 0x100        Host Periodic Transmit FIFO Size Register
    volatile unsigned int       DIEPTXFn[15];       // 0x104~       Device IN Endpoint Transmit FIFO Size register
    volatile unsigned int       NOTDEFINE3[176];    // 0x140~       Reserved
// Host Mode CSR Map
    volatile unsigned int       HCFG;               // 0x400        Host Configuration Register
    volatile unsigned int       HFIR;               // 0x404        Host Frame Interval Register
    volatile unsigned int       HFNUM;              // 0x408        Host Frame Number/Frame Time Remaining register
    volatile unsigned int       NOTDEFINE4;         // 0x40C        Reserved
    volatile unsigned int       HPTXSTS;            // 0x410        Host Periodic Transmit FIFO/Queue Status Register
    volatile unsigned int       HAINT;              // 0x414        Host All Channels Interrupt Register
    volatile unsigned int       HAINTMSK;           // 0x418        Host All Channels Interrupt Mask register
    volatile unsigned int       NOTDEFINE5[9];      // 0x41C~       Not Used
    volatile unsigned int       HPRT;               // 0x440        Host Port Control and Status register
    volatile unsigned int       NOTDEFINE6[47];     // 0x444~       Reserved
    volatile unsigned int       HCCHARn;            // 0x500        Host Channel 0 Characteristics Register
    volatile unsigned int       HCSPLTn;            // 0x504        Host Channel 0 Split Control Register
    volatile unsigned int       HCINTn;             // 0x508        Host Channel 0 Interrupt Register
    volatile unsigned int       HCINTMSKn;          // 0x50C        Host Channel 0 Interrupt Mask Register
    volatile unsigned int       HCTSIZn;            // 0x510        Host Channel 0 Transfer Size Register
    volatile unsigned int       HCDMAn;             // 0x514        Host Channel 0 DMA Address Register
    volatile unsigned int       NOTDEFINE7[2];      // 0x518~       Reserved
    volatile unsigned int       HCH1[8];            // 0x520~       Host Channel 1 Registers
    volatile unsigned int       HCH2[8];            // 0x540~       Host Channel 2 Registers
    volatile unsigned int       HCH3[8];            // 0x560~       Host Channel 3 Registers
    volatile unsigned int       HCH4[8];            // 0x580~       Host Channel 4 Registers
    volatile unsigned int       HCH5[8];            // 0x5A0~       Host Channel 5 Registers
    volatile unsigned int       HCH6[8];            // 0x5C0~       Host Channel 6 Registers
    volatile unsigned int       HCH7[8];            // 0x5E0~       Host Channel 7 Registers
    volatile unsigned int       HCH8[8];            // 0x600~       Host Channel 8 Registers
    volatile unsigned int       HCH9[8];            // 0x620~       Host Channel 9 Registers
    volatile unsigned int       HCH10[8];           // 0x640~       Host Channel 10 Registers
    volatile unsigned int       HCH11[8];           // 0x660~       Host Channel 11 Registers
    volatile unsigned int       HCH12[8];           // 0x680~       Host Channel 12 Registers
    volatile unsigned int       HCH13[8];           // 0x6A0~       Host Channel 13 Registers
    volatile unsigned int       HCH14[8];           // 0x6C0~       Host Channel 14 Registers
    volatile unsigned int       HCH15[8];           // 0x6E0~       Host Channel 15 Registers
    volatile unsigned int       NOTDEFINE8[64];     // 0x700~       Reserved
// Device Mode CSR Map
    volatile unsigned int       DCFG;               // 0x800        Device Configuration Register
    volatile unsigned int       DCTL;               // 0x804        Device Control Register
    volatile unsigned int       DSTS;               // 0x808        Device Status Register (Read Only)
    volatile unsigned int       NOTDEFINE9;         // 0x80C        Reserved
    volatile unsigned int       DIEPMSK;            // 0x810        Device IN Endpoint Common Interrupt Mask Register
    volatile unsigned int       DOEPMSK;            // 0x814        Device OUT Endpoint Common Interrupt Mask register
    volatile unsigned int       DAINT;              // 0x818        Device All Endpoints Interrupt Register
    volatile unsigned int       DAINTMSK;           // 0x81C        Device All Endpoints Interrupt Mask Register
    volatile unsigned int       NOTDEFINE10[2];     // 0x820~       Reserved
    volatile unsigned int       DVBUSDIS;           // 0x828        Device VBUS Discharge Time Register
    volatile unsigned int       DVBUSPULSE;         // 0x82C        Device VBUS Pulsing Time register
    volatile unsigned int       DTHRCTL;            // 0x830        Device Threshold Control register
    volatile unsigned int       DIEPEMPMSK;         // 0x834        Device IN Endpoint FIFO Empty Interrupt Mask register
    volatile unsigned int       NOTDEFINE11[50];    // 0x838~       Reserved

    volatile unsigned int       DIEPCTL0;           // 0x900        Device Control IN Endpoint 0 Control Register
    volatile unsigned int       NOTDEFINE12;        // 0x904        Reserved
    volatile unsigned int       DIEPINT0;           // 0x908        Device IN Endpoint 0 Interrupt Register
    volatile unsigned int       NOTDEFINE13;        // 0x90C        Reserved
    volatile unsigned int       DIEPTSIZ0;          // 0x910        Device IN Endpoint 0 Transfer Size register
    volatile unsigned int       DIEPDMA0;           // 0x914        Device IN Endpoint 0 DMA Address Register
    volatile unsigned int       DTXFSTS0;           // 0x918        Device IN Endpoint Transmit FIFO Status Register
    volatile unsigned int       NOTDEFINE14;        // 0x91C        Reserved

    volatile unsigned int       DEVINENDPT[15][8];  // 0x920~       Device IN Endpoint 1~15 Registers
    volatile unsigned int       DOEPCTL0;           // 0xB00        Device Control OUT Endpoint 0 Control register
    volatile unsigned int       NOTDEFINE15;        // 0xB04        Reserved
    volatile unsigned int       DOEPINT0;           // 0xB08        Device OUT Endpoint 0 Interrupt Register
    volatile unsigned int       NOTDEFINE16;        // 0xB0C        Reserved
    volatile unsigned int       DOEPTSIZ0;          // 0xB10        Device OUT Endpoint 0 Transfer Size Register
    volatile unsigned int       DOEPDMA0;           // 0xB14        Device OUT Endpoint 0 DMA Address register
    volatile unsigned int       NOTDEFINE17[2];     // 0xB18~       Reserved
    volatile unsigned int       DEVOUTENDPT[15][8]; // 0xB20~       Device OUT Endpoint 1~15 Registers
    volatile unsigned int       NOTDEFINE18[64];    // 0xD00~       Reserved
    // Power and Clock Gating CSR Map
    volatile unsigned int       PCGCR;              // 0xE00        Power and Clock Gating Control Register
    volatile unsigned int       NOTDEFINE19[127];   // 0xE04~       Reserved
    // Data FIFO(DFIFO) Access Register Map
    volatile unsigned int       DFIFOENDPT[16][1024];   // 0x1000~      Device IN Endpoint 0~16/Host Out Channel 0~16: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT0[1024];  // 0x1000~      Device IN Endpoint 0/Host Out Channel 0: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT1[1024];  // 0x2000~      Device IN Endpoint 1/Host Out Channel 1: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT2[1024];  // 0x3000~      Device IN Endpoint 2/Host Out Channel 2: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT3[1024];  // 0x4000~      Device IN Endpoint 3/Host Out Channel 3: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT4[1024];  // 0x5000~      Device IN Endpoint 4/Host Out Channel 4: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT5[1024];  // 0x6000~      Device IN Endpoint 5/Host Out Channel 5: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT6[1024];  // 0x7000~      Device IN Endpoint 6/Host Out Channel 6: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT7[1024];  // 0x8000~      Device IN Endpoint 7/Host Out Channel 7: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT8[1024];  // 0x9000~      Device IN Endpoint 8/Host Out Channel 8: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT9[1024];  // 0xA000~      Device IN Endpoint 9/Host Out Channel 9: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT10[1024]; // 0xB000~      Device IN Endpoint 10/Host Out Channel 10: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT11[1024]; // 0xC000~      Device IN Endpoint 11/Host Out Channel 11: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT12[1024]; // 0xD000~      Device IN Endpoint 12/Host Out Channel 12: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT13[1024]; // 0xE000~      Device IN Endpoint 13/Host Out Channel 13: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT14[1024]; // 0xF000~      Device IN Endpoint 14/Host Out Channel 14: DFIFO Write/Read Access
    //volatile unsigned int       DFIFOENDPT15[1024]; // 0x10000~     Device IN Endpoint 15/Host Out Channel 15: DFIFO Write/Read Access
} USB20OTG, *PUSB20OTG;

typedef struct _USBOTG{
    // Core Global CSR Map
    volatile unsigned int       GOTGCTL;            // 0x000  R/W   OTG Control and Status Register
    volatile unsigned int       GOTGINT;            // 0x004        OTG Interrupt Register
    volatile unsigned int       GAHBCFG;            // 0x008        Core AHB Configuration Register
    volatile unsigned int       GUSBCFG;            // 0x00C        Core USB Configuration register
    volatile unsigned int       GRSTCTL;            // 0x010        Core Reset Register
    volatile unsigned int       GINTSTS;            // 0x014        Core Interrupt Register
    volatile unsigned int       GINTMSK;            // 0x018        Core Interrupt Mask Register
    volatile unsigned int       GRXSTSR;            // 0x01C        Receive Status Debug Read register(Read Only)
    volatile unsigned int       GRXSTSP;            // 0x020        Receive Status Read /Pop register(Read Only)
    volatile unsigned int       GRXFSIZ;            // 0x024        Receive FIFO Size Register
    volatile unsigned int       GNPTXFSIZ;          // 0x028        Non-periodic Transmit FIFO Size register
    volatile unsigned int       GNPTXSTS;           // 0x02C        Non-periodic Transmit FIFO/Queue Status register (Read Only)
    volatile unsigned int       NOTDEFINE0[3];      // 0x030~       Reserved
    volatile unsigned int       GUID;               // 0x03C        User ID Register
    volatile unsigned int       NOTDEFINE1;         // 0x040        Reserved
    volatile unsigned int       GHWCFG1;            // 0x044        User HW Config1 Register(Read Only)
    volatile unsigned int       GHWCFG2;            // 0x048        User HW Config2 Register(Read Only)
    volatile unsigned int       GHWCFG3;            // 0x04C        User HW Config3 Register(Read Only)
    volatile unsigned int       GHWCFG4;            // 0x050        User HW Config4 Register(Read Only)
    volatile unsigned int       NOTDEFINE2[43];     // 0x054~       Reserved
    volatile unsigned int       HPTXFSIZ;           // 0x100        Host Periodic Transmit FIFO Size Register
    volatile unsigned int       DIEPTXFn[15];       // 0x104~       Device IN Endpoint Transmit FIFO Size register
    volatile unsigned int       NOTDEFINE3[176];    // 0x140~       Reserved
    // Host Mode CSR Map
    volatile unsigned int       HCFG;               // 0x400        Host Configuration Register
    volatile unsigned int       HFIR;               // 0x404        Host Frame Interval Register
    volatile unsigned int       HFNUM;              // 0x408        Host Frame Number/Frame Time Remaining register
    volatile unsigned int       NOTDEFINE4;         // 0x40C        Reserved
    volatile unsigned int       HPTXSTS;            // 0x410        Host Periodic Transmit FIFO/Queue Status Register
    volatile unsigned int       HAINT;              // 0x414        Host All Channels Interrupt Register
    volatile unsigned int       HAINTMSK;           // 0x418        Host All Channels Interrupt Mask register
    volatile unsigned int       NOTDEFINE5[9];      // 0x41C~       Not Used
    volatile unsigned int       HPRT;               // 0x440        Host Port Control and Status register
    volatile unsigned int       NOTDEFINE6[47];     // 0x444~       Reserved
    volatile unsigned int       HCCHARn;            // 0x500        Host Channel 0 Characteristics Register
    volatile unsigned int       HCSPLTn;            // 0x504        Host Channel 0 Split Control Register
    volatile unsigned int       HCINTn;             // 0x508        Host Channel 0 Interrupt Register
    volatile unsigned int       HCINTMSKn;          // 0x50C        Host Channel 0 Interrupt Mask Register
    volatile unsigned int       HCTSIZn;            // 0x510        Host Channel 0 Transfer Size Register
    volatile unsigned int       HCDMAn;             // 0x514        Host Channel 0 DMA Address Register
    volatile unsigned int       NOTDEFINE7[2];      // 0x518~       Reserved
    volatile unsigned int       HCH1[8];            // 0x520~       Host Channel 1 Registers
    volatile unsigned int       HCH2[8];            // 0x540~       Host Channel 2 Registers
    volatile unsigned int       HCH3[8];            // 0x560~       Host Channel 3 Registers
    volatile unsigned int       HCH4[8];            // 0x580~       Host Channel 4 Registers
    volatile unsigned int       HCH5[8];            // 0x5A0~       Host Channel 5 Registers
    volatile unsigned int       HCH6[8];            // 0x5C0~       Host Channel 6 Registers
    volatile unsigned int       HCH7[8];            // 0x5E0~       Host Channel 7 Registers
    volatile unsigned int       HCH8[8];            // 0x600~       Host Channel 8 Registers
    volatile unsigned int       HCH9[8];            // 0x620~       Host Channel 9 Registers
    volatile unsigned int       HCH10[8];           // 0x640~       Host Channel 10 Registers
    volatile unsigned int       HCH11[8];           // 0x660~       Host Channel 11 Registers
    volatile unsigned int       HCH12[8];           // 0x680~       Host Channel 12 Registers
    volatile unsigned int       HCH13[8];           // 0x6A0~       Host Channel 13 Registers
    volatile unsigned int       HCH14[8];           // 0x6C0~       Host Channel 14 Registers
    volatile unsigned int       HCH15[8];           // 0x6E0~       Host Channel 15 Registers
    volatile unsigned int       NOTDEFINE8[64];     // 0x700~       Reserved
    // Device Mode CSR Map
    volatile unsigned int       DCFG;               // 0x800        Device Configuration Register
    volatile unsigned int       DCTL;               // 0x804        Device Control Register
    volatile unsigned int       DSTS;               // 0x808        Device Status Register (Read Only)
    volatile unsigned int       NOTDEFINE9;         // 0x80C        Reserved
    volatile unsigned int       DIEPMSK;            // 0x810        Device IN Endpoint Common Interrupt Mask Register
    volatile unsigned int       DOEPMSK;            // 0x814        Device OUT Endpoint Common Interrupt Mask register
    volatile unsigned int       DAINT;              // 0x818        Device All Endpoints Interrupt Register
    volatile unsigned int       DAINTMSK;           // 0x81C        Device All Endpoints Interrupt Mask Register
    volatile unsigned int       NOTDEFINE10[2];     // 0x820~       Reserved
    volatile unsigned int       DVBUSDIS;           // 0x828        Device VBUS Discharge Time Register
    volatile unsigned int       DVBUSPULSE;         // 0x82C        Device VBUS Pulsing Time register
    volatile unsigned int       DTHRCTL;            // 0x830        Device Threshold Control register
    volatile unsigned int       DIEPEMPMSK;         // 0x834        Device IN Endpoint FIFO Empty Interrupt Mask register
    volatile unsigned int       NOTDEFINE11[50];    // 0x838~       Reserved

    //volatile unsigned int     DIEPCTL0;           // 0x900        Device Control IN Endpoint 0 Control Register
    //volatile unsigned int     NOTDEFINE12;        // 0x904        Reserved
    //volatile unsigned int     DIEPINT0;           // 0x908        Device IN Endpoint 0 Interrupt Register
    //volatile unsigned int     NOTDEFINE13;        // 0x90C        Reserved
    //volatile unsigned int     DIEPTSIZ0;          // 0x910        Device IN Endpoint 0 Transfer Size register
    //volatile unsigned int     DIEPDMA0;           // 0x914        Device IN Endpoint 0 DMA Address Register
    //volatile unsigned int     DTXFSTS0;           // 0x918        Device IN Endpoint Transmit FIFO Status Register
    //volatile unsigned int     NOTDEFINE14;        // 0x91C        Reserved

    volatile unsigned int       DEVINENDPT[16][8];  // 0x900~       Device IN Endpoint 1~15 Registers

    //volatile unsigned int     DOEPCTL0;           // 0xB00        Device Control OUT Endpoint 0 Control register
    //volatile unsigned int     NOTDEFINE15;        // 0xB04        Reserved
    //volatile unsigned int     DOEPINT0;           // 0xB08        Device OUT Endpoint 0 Interrupt Register
    //volatile unsigned int     NOTDEFINE16;        // 0xB0C        Reserved
    //volatile unsigned int     DOEPTSIZ0;          // 0xB10        Device OUT Endpoint 0 Transfer Size Register
    //volatile unsigned int     DOEPDMA0;           // 0xB14        Device OUT Endpoint 0 DMA Address register
    //volatile unsigned int     NOTDEFINE17[2];     // 0xB18~       Reserved

    volatile unsigned int       DEVOUTENDPT[16][8]; // 0xB00~       Device OUT Endpoint 1~15 Registers
    volatile unsigned int       NOTDEFINE18[64];    // 0xD00~       Reserved

    // Power and Clock Gating CSR Map
    volatile unsigned int       PCGCR;              // 0xE00        Power and Clock Gating Control Register
    volatile unsigned int       NOTDEFINE19[127];   // 0xE04~       Reserved
    // Data FIFO(DFIFO) Access Register Map
    volatile unsigned int       DFIFOENDPT[16][1024];   // 0x1000~      Device IN Endpoint 0~16/Host Out Channel 0~16: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT0[1024];  // 0x1000~      Device IN Endpoint 0/Host Out Channel 0: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT1[1024];  // 0x2000~      Device IN Endpoint 1/Host Out Channel 1: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT2[1024];  // 0x3000~      Device IN Endpoint 2/Host Out Channel 2: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT3[1024];  // 0x4000~      Device IN Endpoint 3/Host Out Channel 3: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT4[1024];  // 0x5000~      Device IN Endpoint 4/Host Out Channel 4: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT5[1024];  // 0x6000~      Device IN Endpoint 5/Host Out Channel 5: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT6[1024];  // 0x7000~      Device IN Endpoint 6/Host Out Channel 6: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT7[1024];  // 0x8000~      Device IN Endpoint 7/Host Out Channel 7: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT8[1024];  // 0x9000~      Device IN Endpoint 8/Host Out Channel 8: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT9[1024];  // 0xA000~      Device IN Endpoint 9/Host Out Channel 9: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT10[1024]; // 0xB000~      Device IN Endpoint 10/Host Out Channel 10: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT11[1024]; // 0xC000~      Device IN Endpoint 11/Host Out Channel 11: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT12[1024]; // 0xD000~      Device IN Endpoint 12/Host Out Channel 12: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT13[1024]; // 0xE000~      Device IN Endpoint 13/Host Out Channel 13: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT14[1024]; // 0xF000~      Device IN Endpoint 14/Host Out Channel 14: DFIFO Write/Read Access
    //volatile unsigned int     DFIFOENDPT15[1024]; // 0x10000~     Device IN Endpoint 15/Host Out Channel 15: DFIFO Write/Read Access
} USBOTG, *PUSBOTG;

typedef struct _USBPHYCFG
{
    volatile unsigned int       UPCR0;              // 0x028  R/W    USB PHY Configuration Register0
    volatile unsigned int       UPCR1;              // 0x02C  R/W    USB PHY Configuration Register1
    volatile unsigned int       UPCR2;              // 0x030  R/W    USB PHY Configuration Register2
    volatile unsigned int       UPCR3;              // 0x034  R/W    USB PHY Configuration Register3
} USBPHYCFG, *PUSBPHYCFG;


/*******************************************************************************
*   15. I2C Controller                                  (Base Addr = 0x76300000)
********************************************************************************/

typedef struct {
    unsigned PRES           :16;
    unsigned                :16;
} I2C_PRES_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2C_PRES_IDX_TYPE       bREG;
} I2C_PRES_TYPE;

typedef struct {
    unsigned                :5;
    unsigned MOD            :1;
    unsigned IEN            :1;
    unsigned EN             :1;
    unsigned                :24;
} I2C_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2C_CTRL_IDX_TYPE       bREG;
} I2C_CTRL_TYPE;

typedef struct {
    unsigned DATA           :16;
    unsigned                :16;
} I2C_TRANS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2C_TRANS_IDX_TYPE      bREG;
} I2C_TRANS_TYPE;

typedef struct {
    unsigned IACK           :1;
    unsigned                :2;
    unsigned ACK            :1;
    unsigned WR             :1;
    unsigned RD             :1;
    unsigned STO            :1;
    unsigned STA            :1;
    unsigned                :24;
} I2C_CMD_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2C_CMD_IDX_TYPE        bREG;
} I2C_CMD_TYPE;

typedef struct {
    unsigned IF             :1;
    unsigned TIP            :1;
    unsigned                :3;
    unsigned AL             :1;
    unsigned BUSY           :1;
    unsigned RxACK          :1;
    unsigned                :24;
} I2C_SR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2C_SR_IDX_TYPE         bREG;
} I2C_SR_TYPE;

typedef struct {
    unsigned FC             :5;
    unsigned CKSEL          :1;
    unsigned                :2;
    unsigned RC             :8;
    unsigned                :16;
} I2C_TR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2C_TR_IDX_TYPE         bREG;
} I2C_TR_TYPE;

typedef struct _I2CMASTER{
    volatile I2C_PRES_TYPE      PRES;           // 0x000  R/W  0x0000FFFF   Clock Prescale register
    volatile I2C_CTRL_TYPE      CTRL;           // 0x004  R/W  0x00000000   Control Register
    volatile I2C_TRANS_TYPE     TXR;            // 0x008  W    0x00000000   Transmit Register
    volatile I2C_CMD_TYPE       CMD;            // 0x00C  W    0x00000000   Command Register
    volatile I2C_TRANS_TYPE     RXR;            // 0x010  R    0x00000000   Receive Register
    volatile I2C_SR_TYPE        SR;             // 0x014  R    0x00000000   Status register
    volatile I2C_TR_TYPE        TIME;           // 0x018  R/W  0x00000000   Timing Control Register
} I2CMASTER, *PI2CMASTER;

typedef struct {
    unsigned PORT           :8;
    unsigned                :8;
    unsigned RXVC           :3;
    unsigned                :5;
    unsigned TXVC           :3;
    unsigned                :5;
} I2CS_DPORT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_DPORT_IDX_TYPE     bREG;
} I2CS_DPORT_TYPE;

typedef struct {
    unsigned EN             :1;
    unsigned                :1;
    unsigned CLR            :1;
    unsigned SDA            :1;
    unsigned WS             :1;
    unsigned RCLR           :1;
    unsigned                :2;
    unsigned RXTH           :2;
    unsigned                :2;
    unsigned TXTH           :2;
    unsigned                :2;
    unsigned DRQEN          :4;
    unsigned FC             :5;
    unsigned                :5;
    unsigned SLV            :2;
} I2CS_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_CTRL_IDX_TYPE      bREG;
} I2CS_CTRL_TYPE;

typedef struct {
    unsigned ADDR           :8;
    unsigned                :24;
} I2CS_ADDR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_ADDR_IDX_TYPE      bREG;
} I2CS_ADDR_TYPE;

typedef struct {
    unsigned IRQEN          :12;
    unsigned                :4;
    unsigned IRQSTAT        :12;
    unsigned                :4;
} I2CS_INT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_INT_IDX_TYPE       bREG;
} I2CS_INT_TYPE;

typedef struct {
    unsigned SADR           :8;
    unsigned                :15;
    unsigned DDIR           :1;
    unsigned                :8;
} I2CS_STAT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_STAT_IDX_TYPE      bREG;
} I2CS_STAT_TYPE;

typedef struct {
    unsigned MBFR           :8;
    unsigned                :8;
    unsigned MBFT           :8;
    unsigned                :8;
} I2CS_MBF_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_MBF_IDX_TYPE       bREG;
} I2CS_MBF_TYPE;

typedef struct {
    unsigned DATA0          :8;
    unsigned DATA1          :8;
    unsigned DATA2          :8;
    unsigned DATA3          :8;
} I2CS_DATA0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_DATA0_IDX_TYPE     bREG;
} I2CS_DATA0_TYPE;

typedef struct {
    unsigned DATA4          :8;
    unsigned DATA5          :8;
    unsigned DATA6          :8;
    unsigned DATA7          :8;
} I2CS_DATA1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CS_DATA1_IDX_TYPE     bREG;
} I2CS_DATA1_TYPE;

typedef struct _I2CSLAVE{
    volatile I2CS_DPORT_TYPE    PORT;           // 0x000  R/W  -            Data Access port (TX/RX FIFO)
    volatile I2CS_CTRL_TYPE     CTL;            // 0x004  R/W  0x00000000   Control register
    volatile I2CS_ADDR_TYPE     ADDR;           // 0x008  W    0x00000000   Address register
    volatile I2CS_INT_TYPE      INT;            // 0x00C  W    0x00000000   Interrupt Enable Register
    volatile I2CS_STAT_TYPE     STAT;           // 0x010  R    0x00000000   Status Register
    unsigned :32; unsigned :32;
    volatile I2CS_MBF_TYPE      MBF;            // 0x01C  R/W  0x00000000   Buffer Valid Flag
    volatile I2CS_DATA0_TYPE    MB0;            // 0x020  R/W  0x00000000   Data Buffer (Byte 7 ~ 0)
    volatile I2CS_DATA1_TYPE    MB1;            // 0x024  R/W  0x00000000   Data Buffer (Byte 7 ~ 0)
} I2CSLAVE, *PI2CSLAVE;

typedef struct {
    unsigned MASTER0        :8;
    unsigned MASTER1        :8;
    unsigned MASTER2        :8;
    unsigned MASTER3        :8;
} I2CP_CFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CP_CFG_IDX_TYPE       bREG;
} I2CP_CFG_TYPE;

typedef struct {
    unsigned SLAVE0         :8;
    unsigned SLAVE1         :8;
    unsigned                :16;
} I2CPS_CFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CPS_CFG_IDX_TYPE      bREG;
} I2CPS_CFG_TYPE;

typedef struct {
    unsigned M0             :1;
    unsigned M1             :1;
    unsigned M2             :1;
    unsigned M3             :1;
    unsigned S0             :1;
    unsigned S1             :1;
    unsigned                :26;
} I2CP_IRQ_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    I2CP_IRQ_IDX_TYPE       bREG;
} I2CP_IRQ_TYPE;

typedef struct _I2CPORTCFG{
    volatile I2CP_CFG_TYPE     PCFG0;           // 0x000  R/W  0x03020100   I2C Master Port Configuration
    volatile I2CPS_CFG_TYPE    PCFG1;           // 0x004  R/W  0x00000504   I2C Slave Port Configuration
    unsigned :32;
    volatile I2CP_IRQ_TYPE     ISTS;            // 0x00C  R    0x00000000   IRQ Status
} I2CPORTCFG, *PI2CPORTCFG;


/*******************************************************************************
*   16. PWM(Pulse Width Modulation)                     (Base Addr = 0x76050000)
********************************************************************************/

typedef struct {
    unsigned BUSY_A         :1;
    unsigned BUSY_B         :1;
    unsigned BUSY_C         :1;
    unsigned BUSY_D         :1;
    unsigned                :28;
} PWM_STATUS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWM_STATUS_IDX_TYPE     bREG;
} PWM_STATUS_TYPE;

typedef struct {
    unsigned EN_A           :1;
    unsigned EN_B           :1;
    unsigned EN_C           :1;
    unsigned EN_D           :1;
    unsigned                :12;
    unsigned TRIG_A         :1;
    unsigned TRIG_B         :1;
    unsigned TRIG_C         :1;
    unsigned TRIG_D         :1;
    unsigned                :12;
} PWM_ENABLE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWM_ENABLE_IDX_TYPE     bREG;
} PWM_ENABLE_TYPE;

typedef struct {
    unsigned MODE_A         :4;
    unsigned MODE_B         :4;
    unsigned MODE_C         :4;
    unsigned MODE_D         :4;
    unsigned INV_A          :1;
    unsigned INV_B          :1;
    unsigned INV_C          :1;
    unsigned INV_D          :1;
    unsigned                :4;
    unsigned DIV_A          :2;
    unsigned DIV_B          :2;
    unsigned DIV_C          :2;
    unsigned DIV_D          :2;
} PWM_MODE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWM_MODE_IDX_TYPE       bREG;
} PWM_MODE_TYPE;

typedef struct {
    unsigned LOOP_A         :4;
    unsigned LOOP_B         :4;
    unsigned LOOP_C         :4;
    unsigned LOOP_D         :4;
    unsigned                :16;
} PWM_LOOP_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWM_LOOP_IDX_TYPE       bREG;
} PWM_LOOP_TYPE;

typedef struct {
    unsigned Pstn1          :16;
    unsigned Pstn2          :16;
} PWM_PSTEP0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWM_PSTEP0_IDX_TYPE     bREG;
} PWM_PSTEP0_TYPE;

typedef struct {
    unsigned Pstn3          :16;
    unsigned Pstn4          :16;
} PWM_PSTEP1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWM_PSTEP1_IDX_TYPE     bREG;
} PWM_PSTEP1_TYPE;

typedef struct {
    unsigned DATA           :32;
} PWM_OUTREG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    PWM_OUTREG_IDX_TYPE     bREG;
} PWM_OUTREG_TYPE;

typedef struct _PWM{
    volatile PWM_STATUS_TYPE    PwmSt;          // 0x000  R    0x00000000   PWM block status
    volatile PWM_ENABLE_TYPE    PwmEn;          // 0x004  R/W  0x0000000F   PWM block enable & PWM Trigger
    volatile PWM_MODE_TYPE      PwmMode;        // 0x008  R/W  0x000000AA   PWM output mode & PWMCLKDiv
    volatile PWM_LOOP_TYPE      PwmLoop;        // 0x00C  R/W  0x00000000   PWM block operation Steps
    volatile PWM_PSTEP0_TYPE    PwmAPstn0;      // 0x010  R/W  0x00200010   PWM block A position 1 & 2
    volatile PWM_PSTEP1_TYPE    PwmAPstn1;      // 0x014  R/W  0x00400030   PWM block A position 3 & 4
    volatile PWM_PSTEP0_TYPE    PwmBPstn0;      // 0x018  R/W  0x00200010   PWM block B position 1 & 2
    volatile PWM_PSTEP1_TYPE    PwmBPstn1;      // 0x01C  R/W  0x00400030   PWM block B position 3 & 4
    volatile PWM_PSTEP0_TYPE    PwmCPstn0;      // 0x020  R/W  0x00200010   PWM block C position 1 & 2
    volatile PWM_PSTEP1_TYPE    PwmCPstn1;      // 0x024  R/W  0x00400030   PWM block C position 3 & 4
    volatile PWM_PSTEP0_TYPE    PwmDPstn0;      // 0x028  R/W  0x00200010   PWM block D position 1 & 2
    volatile PWM_PSTEP1_TYPE    PwmDPstn1;      // 0x02C  R/W  0x00400030   PWM block D position 3 & 4
    volatile PWM_OUTREG_TYPE    PwmOutRegA1;    // 0x030  R/W  0xAA0000AA   PWM A1 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegA2;    // 0x034  R/W  0x00AAAA00   PWM A2 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegA3;    // 0x038  R/W  0x00AAAA00   PWM A3 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegA4;    // 0x03C  R/W  0xAA0000AA   PWM A4 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegB1;    // 0x040  R/W  0x55000000   PWM B1 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegB2;    // 0x044  R/W  0x00550000   PWM B2 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegB3;    // 0x048  R/W  0x00005500   PWM B3 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegB4;    // 0x04C  R/W  0x00000055   PWM B4 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegC1;    // 0x050  R/W  0xAA000000   PWM C1 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegC2;    // 0x054  R/W  0x00AA0000   PWM C2 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegC3;    // 0x058  R/W  0x0000AA00   PWM C3 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegC4;    // 0x05C  R/W  0x000000AA   PWM C4 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegD1;    // 0x060  R/W  0x55000000   PWM D1 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegD2;    // 0x064  R/W  0x00550000   PWM D2 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegD3;    // 0x068  R/W  0x00005500   PWM D3 block Reg mode Output value
    volatile PWM_OUTREG_TYPE    PwmOutRegD4;    // 0x06C  R/W  0x00000055   PWM D4 block Reg mode Output value
} PWM, *PPWM;


/*******************************************************************************
*   17. Remote Control Interface                        (Base Addr = 0x76063000)
********************************************************************************/

typedef struct {
    unsigned DATA           :32;
} REMOTE_RDATA_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    REMOTE_RDATA_IDX_TYPE   bREG;
} REMOTE_RDATA_TYPE;

typedef struct {
    unsigned FF             :1;
    unsigned EN             :1;
    unsigned CLEAR          :1;
    unsigned TH             :9;
    unsigned WS             :1;
    unsigned DEN            :1;
    unsigned FWEN           :1;
    unsigned                :17;
} REMOTE_CMD_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    REMOTE_CMD_IDX_TYPE     bREG;
} REMOTE_CMD_TYPE;

typedef struct {
    unsigned INV            :1;
    unsigned                :7;
    unsigned SCLK           :1;
    unsigned FIL            :1;
    unsigned FT             :2;
    unsigned                :20;
} REMOTE_INPOL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    REMOTE_INPOL_IDX_TYPE   bREG;
} REMOTE_INPOL_TYPE;

typedef struct {
    unsigned ICF            :12;
    unsigned OF             :1;
    unsigned                :19;
} REMOTE_STA_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    REMOTE_STA_IDX_TYPE     bREG;
} REMOTE_STA_TYPE;

typedef struct {
    unsigned END_CNT        :8;
    unsigned                :8;
    unsigned CLK_DIV        :16;
} REMOTE_CLKDIV_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    REMOTE_CLKDIV_IDX_TYPE  bREG;
} REMOTE_CLKDIV_TYPE;

typedef struct _REMOTECON{
    volatile REMOTE_RDATA_TYPE  RDATA;          // 0x000  R/W  0x00000000   IR Data Transfer Address
    volatile REMOTE_CMD_TYPE    CMD;            // 0x004  R/W  0x00000000   Command Register
    volatile REMOTE_INPOL_TYPE  INPOL;          // 0x008  R/W  0x00000000   Control Register
    volatile REMOTE_STA_TYPE    STA;            // 0x00C  W    0x00000000   Status register
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile REMOTE_CLKDIV_TYPE CLKDIV;         // 0x040  R    0x00000000   Clock Divide Register
} REMOTECON, *PREMOTECON;


/*******************************************************************************
*   18. TSADC Interface                                 (Base Addr = 0x76064000)
********************************************************************************/

typedef struct {
    unsigned EN_ST          :1;
    unsigned RD_ST          :1;
    unsigned STBY           :1;
    unsigned ASEL           :4;
    unsigned PS_VAL         :8;
    unsigned PS_EN          :1;
    unsigned E_FLG          :1;
    unsigned RES            :1;
    unsigned                :14;
} TSADC_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSADC_CTRL_IDX_TYPE     bREG;
} TSADC_CTRL_TYPE;

typedef struct {
    unsigned XY_PST         :2;
    unsigned AUTO           :1;
    unsigned PUON           :1;
    unsigned XPEN           :1;
    unsigned XMEN           :1;
    unsigned YPEN           :1;
    unsigned YMEN           :1;
    unsigned UDEN           :1;
    unsigned                :23;
} TSADC_TSC_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSADC_TSC_IDX_TYPE      bREG;
} TSADC_TSC_TYPE;

typedef struct {
    unsigned DELAY          :16;
    unsigned CLKsrc         :1;
    unsigned                :15;
} TSADC_DELAY_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSADC_DELAY_IDX_TYPE    bREG;
} TSADC_DELAY_TYPE;

typedef struct {
    unsigned XPDATA         :10;
    unsigned XPDATA_12      :2;
    unsigned XYPST          :2;
    unsigned AUTO           :1;
    unsigned UPDN           :1;
    unsigned                :16;
} TSADC_DATA_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSADC_DATA_IDX_TYPE     bREG;
} TSADC_DATA_TYPE;

typedef struct {
    unsigned DOWN           :1;
    unsigned UP             :1;
    unsigned                :30;
} TSADC_UPDN_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSADC_UPDN_IDX_TYPE     bREG;
} TSADC_UPDN_TYPE;

typedef struct {
    unsigned CLR            :1;
    unsigned                :31;
} TSADC_INTCLR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSADC_INTCLR_IDX_TYPE   bREG;
} TSADC_INTCLR_TYPE;

typedef struct _TSADC{
    volatile TSADC_CTRL_TYPE    ADCCON;         // 0x000  R/W  0x00007F84   ADC Control Register
    volatile TSADC_TSC_TYPE     ADCTSC;         // 0x004  R/W  0x00000058   ADC Touch Screen Control Register
    volatile TSADC_DELAY_TYPE   ADCDLY;         // 0x008  R/W  0x000000FF   ADC Start or Interval Delay Register
    volatile TSADC_DATA_TYPE    ADCDAT0;        // 0x00C  R    0x00008000   ADC Conversion Data0 Register
    volatile TSADC_DATA_TYPE    ADCDAT1;        // 0x010  R    0x00008000   ADC Conversion Data1 Register
    volatile TSADC_UPDN_TYPE    ADCUPDN;        // 0x014  R/W  0x00000000   Stylus Up or Down Interrupt Register
    volatile TSADC_INTCLR_TYPE  CLR_INT_EOC;    // 0x018  W    0x00000000   Clear INT_EOC Interrrupt
    unsigned :32;
    volatile TSADC_INTCLR_TYPE  CLR_INT_WKU;    // 0x020  W    0x00000000   Clear INT_WKU(Pen UP/DOWN) Interrupt
} TSADC, *PTSADC;


/*******************************************************************************
*   19. TSIF(Transport Stream Interface)                (Base Addr = 0x76800000)
********************************************************************************/

typedef struct {
    unsigned DATA           :32;
} TSIF_DATA_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIF_DATA_IDX_TYPE      bREG;
} TSIF_DATA_TYPE;

typedef struct {
    unsigned TSD            :5;
    unsigned TSDE           :1;
    unsigned TVSC           :1;
    unsigned CRF            :1;
    unsigned TBS            :2;
    unsigned TrgPos         :1;
    unsigned TSP            :1;
    unsigned TVEP           :1;
    unsigned TCKP           :1;
    unsigned TPSM           :1;
    unsigned END            :1;
    unsigned ML             :1;
    unsigned SBE            :1;
    unsigned GC             :1;
    unsigned                :12;
    unsigned EN             :1;
} TSIF_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIF_CTRL_IDX_TYPE      bREG;
} TSIF_CTRL_TYPE;

typedef struct {
    unsigned PIDEV          :13;
    unsigned PIDEE          :1;
    unsigned                :2;
    unsigned PIDOV          :13;
    unsigned PIDOE          :1;
    unsigned                :2;
} TSIF_PID_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIF_PID_IDX_TYPE       bREG;
} TSIF_PID_TYPE;

typedef struct {
    unsigned RUR            :1;
    unsigned ROR            :1;
    unsigned RFul           :1;
    unsigned REmp           :1;
    unsigned RCFul          :1;
    unsigned RxCnt          :3;
    unsigned                :1;
    unsigned PID            :1;
    unsigned                :22;
} TSIF_INTC_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIF_INTC_IDX_TYPE      bREG;
} TSIF_INTC_TYPE;

typedef struct {
    unsigned RUR            :1;
    unsigned ROR            :1;
    unsigned RFul           :1;
    unsigned REmp           :1;
    unsigned RCFul          :1;
    unsigned                :23;
} TSIF_INTS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIF_INTS_IDX_TYPE      bREG;
} TSIF_INTS_TYPE;

typedef struct {
    unsigned PIDI00         :1;
    unsigned PIDI01         :1;
    unsigned PIDI02         :1;
    unsigned PIDI03         :1;
    unsigned PIDI04         :1;
    unsigned PIDI05         :1;
    unsigned PIDI06         :1;
    unsigned PIDI07         :1;
    unsigned PIDI08         :1;
    unsigned PIDI09         :1;
    unsigned PIDI10         :1;
    unsigned PIDI11         :1;
    unsigned PIDI12         :1;
    unsigned PIDI13         :1;
    unsigned PIDI14         :1;
    unsigned PIDI15         :1;
    unsigned PIDI16         :1;
    unsigned PIDI17         :1;
    unsigned PIDI18         :1;
    unsigned PIDI19         :1;
    unsigned PIDI20         :1;
    unsigned PIDI21         :1;
    unsigned PIDI22         :1;
    unsigned PIDI23         :1;
    unsigned PIDI24         :1;
    unsigned PIDI25         :1;
    unsigned PIDI26         :1;
    unsigned PIDI27         :1;
    unsigned PIDI28         :1;
    unsigned PIDI29         :1;
    unsigned PIDI30         :1;
    unsigned PIDI31         :1;
} TSIF_INTSP_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIF_INTSP_IDX_TYPE     bREG;
} TSIF_INTSP_TYPE;

typedef struct _TSIF{
    volatile TSIF_DATA_TYPE     TSDI;           // 0x000  R    0x00000000   TSIF Input Data Register
    volatile TSIF_CTRL_TYPE     TSCR;           // 0x004  R/W  0x00000300   TSIF Control Register
    volatile TSIF_PID_TYPE      TSPID[16];      // 0x008~ R/W  0x00000000   TSIF PID Register
    volatile TSIF_INTC_TYPE     TSIC;           // 0x048  R/W  0x00000080   TSIF Interrupt Control Register
    volatile TSIF_INTS_TYPE     TSIS;           // 0x04C  R    0x00000000   TSIF Interrupt Status Register
    volatile TSIF_INTSP_TYPE    TSISP;          // 0x050  R    0x00000000   TSIF interrupt Status(PID) Register
} TSIF, *PTSIF;

typedef struct {
    unsigned TX_BASE        :32;
} TSIFDMA_TX_IDX_TYPE;

typedef union {
    unsigned long          	 nREG;
    TSIFDMA_TX_IDX_TYPE	bREG;
} TSIFDMA_TX_TYPE;

typedef struct {
    unsigned RX_BASE        :32;
} TSIFDMA_RX_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIFDMA_RX_IDX_TYPE        bREG;
} TSIFDMA_RX_TYPE;

typedef struct {
    unsigned SIZE           :13;
    unsigned                :3;
    unsigned COUNT          :13;
    unsigned                :3;
} TSIFDMA_PACKET_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIFDMA_PACKET_IDX_TYPE    bREG;
} TSIFDMA_PACKET_TYPE;

typedef struct {
    unsigned EN             :1;
    unsigned TSIF           :1;
    unsigned PCLR           :1;
    unsigned                :1;
    unsigned MD             :2;
    unsigned                :8;
    unsigned RXAM           :2;
    unsigned TXAM           :2;
    unsigned MS             :1;
    unsigned MP             :1;
    unsigned                :8;
    unsigned END            :1;
    unsigned CT             :1;
    unsigned DRE            :1;
    unsigned DTE            :1;
} TSIFDMA_DMACTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIFDMA_DMACTRL_IDX_TYPE   bREG;
} TSIFDMA_DMACTRL_TYPE;

typedef struct {
    unsigned TXPCNT         :13;
    unsigned                :4;
    unsigned RXPCNT         :13;
    unsigned                :2;
} TSIFDMA_DMASTAT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIFDMA_DMASTAT_IDX_TYPE   bREG;
} TSIFDMA_DMASTAT_TYPE;

typedef struct {
    unsigned IRQPCNT        :13;
    unsigned                :3;
    unsigned IEP            :1;
    unsigned IED            :1;
    unsigned                :2;
    unsigned IRQS           :1;
    unsigned                :7;
    unsigned ISP            :1;
    unsigned ISD            :1;
    unsigned                :2;
} TSIFDMA_DMAICR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIFDMA_DMAICR_IDX_TYPE    bREG;
} TSIFDMA_DMAICR_TYPE;

typedef struct _TSIFDMA{
	volatile TSIFDMA_TX_TYPE       		TXBASE;         // 0x020  R/W  0x00000000   TX base address register
	volatile TSIFDMA_RX_TYPE       	RXBASE;         // 0x024  R/W  0x00000000   RX base address register
	volatile TSIFDMA_PACKET_TYPE   	PACKET;         // 0x028  R/W  0x00000000   Packet register
	volatile TSIFDMA_DMACTRL_TYPE  	DMACTR;         // 0x02C  R/W  0x00000000   DMA control register
	volatile TSIFDMA_DMASTAT_TYPE  	DMASTR;         // 0x030  R/W  0x00000000   DMA status register
	volatile TSIFDMA_DMAICR_TYPE  	DMAICR;         // 0x034  R/W  0x00000000   DMA interrupt control register
} TSIFDMA, *PTSIFDMA;

typedef struct {
    unsigned RX_CH0		:1;
    unsigned RX_CH1		:1;
    unsigned RX_CH2		:1;
    unsigned TX_CH0		:1;
    unsigned TX_CH1		:1;
    unsigned TX_CH2		:1;
    unsigned DMA_CH0	:1;
    unsigned DMA_CH1	:1;
    unsigned DMA_CH2	:1;
    unsigned 				:23;
} TSIF_IRQSTS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    TSIF_IRQSTS_IDX_TYPE    bREG;
} TSIF_IRQSTS_TYPE;

typedef struct _TSIFIRQSTATUS{
    volatile TSIF_IRQSTS_TYPE		IRQSTS;         // 0x00C  R    0x00000000   IRQ Status Register
} TSIFIRQSTATUS, *PTSIFIRQSTATUS;


/*******************************************************************************
*   20. UART                                            (Base Addr = 0x76370000)
********************************************************************************/

typedef struct {
    unsigned DATA           :8;
    unsigned                :24;
} UART_PBR_IDX_TYPE;

typedef struct {
    unsigned DATA           :8;
    unsigned                :24;
} UART_THR_IDX_TYPE;

typedef struct {
    unsigned DIV            :8;
    unsigned                :24;
} UART_DLL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_PBR_IDX_TYPE       RBR;                // 0x000  R    Unknown      Receiver Buffer Register(DLAB = 0)
    UART_THR_IDX_TYPE       THR;                // 0x000  W    0x00000000   Transmitter Holding Register (DLAB=0)
    UART_DLL_IDX_TYPE       DLL;                // 0x000  R/W  0x00000000   Divisor Latch (LSB) (DLAB=1)
    volatile unsigned int  UTRXD;                 
    volatile unsigned int  UTTXD;                 
} UART_REG1_TYPE;

typedef struct {
    unsigned DIV            :8;
    unsigned                :24;
} UART_DLM_IDX_TYPE;

typedef struct {
    unsigned ERXI           :1;
    unsigned ETXI           :1;
    unsigned ELSI           :1;
    unsigned EMSI           :1;
    unsigned                :28;
} UART_IER_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_IER_IDX_TYPE       IER;                // 0x004  R/W  0x00000000   Interrupt Enable Register (DLAB=0)
    UART_DLM_IDX_TYPE       DLM;                // 0x004  R/W  0x00000000   Divisor Latch (MSB) (DLAB=1)
} UART_REG2_TYPE;

typedef struct {
    unsigned IPF            :1;
    unsigned IID            :3;
    unsigned                :23;
    unsigned STF            :1;
    unsigned                :4;
} UART_IIR_IDX_TYPE;

typedef struct {
    unsigned FE             :1;
    unsigned RXFR           :1;
    unsigned TXFR           :1;
    unsigned DRTE           :1;
    unsigned TXT            :2;
    unsigned RXT            :2;
    unsigned                :24;
} UART_FCR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_IIR_IDX_TYPE       IIR;                // 0x008  R    Unknown      Interrupt Ident. Register (DLAB=0)
    UART_FCR_IDX_TYPE       FCR;                // 0x008  W    0x000000C0   FIFO Control Register (DLAB=1)
} UART_REG3_TYPE;

typedef struct {
    unsigned WLS            :2;
    unsigned STB            :1;
    unsigned PEN            :1;
    unsigned EPS            :1;
    unsigned SP             :1;
    unsigned SB             :1;
    unsigned DLAB           :1;
    unsigned                :24;
} UART_LCR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_LCR_IDX_TYPE       bREG;
} UART_LCR_TYPE;

typedef struct {
    unsigned                :1;
    unsigned RTS            :1;
    unsigned                :2;
    unsigned LOOP           :1;
    unsigned AFE            :1;
    unsigned RS             :1;
    unsigned                :25;
} UART_MCR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_MCR_IDX_TYPE       bREG;
} UART_MCR_TYPE;

typedef struct {
    unsigned DR             :1;
    unsigned OE             :1;
    unsigned PE             :1;
    unsigned FE             :1;
    unsigned BI             :1;
    unsigned THRE           :1;
    unsigned TEMT           :1;
    unsigned ERF            :1;
    unsigned                :24;
} UART_LSR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_LSR_IDX_TYPE       bREG;
} UART_LSR_TYPE;

typedef struct {
    unsigned DCTS           :1;
    unsigned                :3;
    unsigned CTS            :1;
    unsigned                :27;
} UART_MSR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_MSR_IDX_TYPE       bREG;
} UART_MSR_TYPE;

typedef struct {
    unsigned DATA           :8;
    unsigned                :24;
} UART_SCR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_SCR_IDX_TYPE       bREG;
} UART_SCR_TYPE;

typedef struct {
    unsigned DTL            :4;
    unsigned ATL            :4;
    unsigned                :24;
} UART_AFT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_AFT_IDX_TYPE       bREG;
} UART_AFT_TYPE;

typedef struct {
    unsigned TxDE           :1;
    unsigned RxDE           :1;
    unsigned TWA            :1;
    unsigned RWA            :1;
    unsigned                :28;
} UART_UCR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_UCR_IDX_TYPE       bREG;
} UART_UCR_TYPE;

typedef struct {
    unsigned RxD            :8;
    unsigned                :24;
} UART_SRBR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_SRBR_IDX_TYPE      bREG;
} UART_SRBR_TYPE;

typedef union {
    unsigned long           nREG;
    UART_THR_IDX_TYPE       bREG;
} UART_STHR_TYPE;

typedef union {
    unsigned long           nREG;
    UART_DLL_IDX_TYPE       bREG;
} UART_SDLL_TYPE;

typedef union {
    unsigned long           nREG;
    UART_DLM_IDX_TYPE       bREG;
} UART_SDLM_TYPE;

typedef union {
    unsigned long           nREG;
    UART_IER_IDX_TYPE       bREG;
} UART_SIER_TYPE;

typedef struct {
    unsigned DIV            :16;
    unsigned                :10;
    unsigned STE            :1;
    unsigned STF            :1;
    unsigned P              :1;
    unsigned DEN            :1;
    unsigned STEN           :1;
    unsigned SEN            :1;
} UART_SCCR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_SCCR_IDX_TYPE      bREG;
} UART_SCCR_TYPE;

typedef struct {
    unsigned S_CNT          :16;
    unsigned C_CNT          :16;
} UART_STC_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_STC_IDX_TYPE       bREG;
} UART_STC_TYPE;

typedef struct {
    unsigned TXE            :1;
    unsigned RXE            :1;
    unsigned TXP            :1;
    unsigned RXP            :1;
    unsigned                :28;
} UART_IRCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_IRCFG_IDX_TYPE     bREG;
} UART_IRCFG_TYPE;

typedef struct _UART{
    volatile UART_REG1_TYPE     REG1;           // 0x000  PBR(R) / THR(W) / DLL(R/W)
    volatile UART_REG2_TYPE     REG2;           // 0x004  IER(R/W) / DLM(R/W)
    volatile UART_REG3_TYPE     REG3;           // 0x008  IIR(R) / FCR(W)
    volatile UART_LCR_TYPE      LCR;            // 0x00C  R/W  0x00000003   Line Control Register
    volatile UART_MCR_TYPE      MCR;            // 0x010  R/W  0x00000040   MODEM Control Register
    volatile UART_LSR_TYPE      LSR;            // 0x014  R    Unknown      Line Status Register
    volatile UART_MSR_TYPE      MSR;            // 0x018  R    Unknown      MODEM Status Register
    volatile UART_SCR_TYPE      SCR;            // 0x01C  R/W  0x00000000   Scratch Register
    volatile UART_AFT_TYPE      AFT;            // 0x020  R/W  0x00000000   AFC Trigger Level Register
    volatile UART_UCR_TYPE      UCR;            // 0x024  R/W  0x00000000   UART Control register
    unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile UART_SRBR_TYPE     SRBR;           // 0x040  R    Unknown      Rx Buffer Register
    volatile UART_STHR_TYPE     STHR;           // 0x044  W    0x00000000   Transmitter Holding Register
    volatile UART_SDLL_TYPE     SDLL;           // 0x048  R/W  0x00000000   Divisor Latch (LSB)
    volatile UART_SDLM_TYPE     SDLM;           // 0x04C  R/W  0x00000000   Divisor Latch (MSB)
    volatile UART_SIER_TYPE     SIER;           // 0x050  R/W  0x00000000   Interrupt Enable register
    unsigned :32; unsigned :32; unsigned :32;
    volatile UART_SCCR_TYPE     SCCR;           // 0x060  R/W  0x00000000   Smart Card Control Register
    volatile UART_STC_TYPE      STC;            // 0x064  R/W  0x00000000   Smart Card TX Count register
    unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile UART_IRCFG_TYPE    IRCFG;          // 0x080  R/W  0x00000000   IRDA Configuration Register
} UART, *PUART;

typedef struct {
    unsigned UART0          :8;
    unsigned UART1          :8;
    unsigned UART2          :8;
    unsigned UART3          :8;
} UART_PORTCFG0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_PORTCFG0_IDX_TYPE  bREG;
} UART_PORTCFG0_TYPE;

typedef struct {
    unsigned UART4          :8;
    unsigned UART5          :8;
    unsigned UART6          :8;
    unsigned UART7          :8;
} UART_PORTCFG1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_PORTCFG1_IDX_TYPE  bREG;
} UART_PORTCFG1_TYPE;

typedef struct {
    unsigned U0             :1;
    unsigned U1             :1;
    unsigned U2             :1;
    unsigned U3             :1;
    unsigned U4             :1;
    unsigned U5             :1;
    unsigned U6             :1;
    unsigned U7             :1;
    unsigned                :24;
} UART_ISTS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    UART_ISTS_IDX_TYPE      bREG;
} UART_ISTS_TYPE;

typedef struct _UARTPORTCFG{
    volatile UART_PORTCFG0_TYPE PCFG0;          // 0x000  R/W  0x03020100   Port Configuration 0
    volatile UART_PORTCFG1_TYPE PCFG1;          // 0x004  R/W  0x07060504   Port Configuration 1
    unsigned :32;
    volatile UART_ISTS_TYPE     ISTS;           // 0x00C  R    0x00000000   IRQ Status
} UARTPORTCFG, *PUARTPORTCFG;


/*******************************************************************************
*   21. MPEFEC                                        (Base Addr = 0x0x76010000)
********************************************************************************/

typedef struct {
    unsigned INIT           :1;
    unsigned ME             :1;
    unsigned                :30;
} MPEFEC_MERR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MERR_IDX_TYPE    bREG;
} MPEFEC_MERR_TYPE;

typedef struct {
    unsigned MS             :1;
    unsigned                :31;
} MPEFEC_MSR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MSR_IDX_TYPE     bREG;
} MPEFEC_MSR_TYPE;

typedef struct {
    unsigned FRN            :10;
    unsigned                :6;
    unsigned ARN            :10;
    unsigned                :6;
} MPEFEC_MFRNR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MFRNR_IDX_TYPE   bREG;
} MPEFEC_MFRNR_TYPE;

typedef struct {
    unsigned MDSA           :32;
} MPEFEC_MFSAR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MFSAR_IDX_TYPE   bREG;
} MPEFEC_MFSAR_TYPE;

typedef struct {
    unsigned MEFSA          :32;
} MPEFEC_EFSAR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_EFSAR_IDX_TYPE   bREG;
} MPEFEC_EFSAR_TYPE;

typedef struct {
    unsigned PCL            :7;
    unsigned                :1;
    unsigned SCL            :8;
    unsigned ET             :7;
    unsigned                :1;
    unsigned EE             :1;
    unsigned DE             :1;
    unsigned                :6;
} MPEFEC_MCR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MCR_IDX_TYPE     bREG;
} MPEFEC_MCR_TYPE;

typedef struct {
    unsigned MS             :1;
    unsigned                :31;
} MPEFEC_MSTR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MSTR_IDX_TYPE    bREG;
} MPEFEC_MSTR_TYPE;

typedef struct {
    unsigned MIE            :1;
    unsigned                :31;
} MPEFEC_MIER_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MIER_IDX_TYPE    bREG;
} MPEFEC_MIER_TYPE;

typedef struct {
    unsigned MIC            :1;
    unsigned                :31;
} MPEFEC_MICR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MICR_IDX_TYPE    bREG;
} MPEFEC_MICR_TYPE;

typedef struct {
    unsigned ERN            :10;
    unsigned                :6;
    unsigned SRN            :10;
    unsigned                :6;
} MPEFEC_MARR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_MARR_IDX_TYPE    bREG;
} MPEFEC_MARR_TYPE;

typedef struct {
    unsigned ECNT           :32;
} MPEFEC_ECR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MPEFEC_ECR_IDX_TYPE     bREG;
} MPEFEC_ECR_TYPE;

typedef struct _MPEFEC{
    volatile MPEFEC_MERR_TYPE   MERR;           // 0x000  R/W  0x00000000   MPE_FEC Enable/ RESET [1] -> MPE_FEC Enable [0] -> MPE_FEC RESET
    volatile MPEFEC_MSR_TYPE    MSR;            // 0x004  W    0x00000000   MPE_FEC Start (Auto clear)
    volatile MPEFEC_MFRNR_TYPE  MFRNR;          // 0x008  R/W  0x03FF03FF   MPEFEC Frame Row Number Register
    volatile MPEFEC_MFSAR_TYPE  MFSAR;          // 0x00C  R/W  0x00000000   MPEFEC Frame Source Address Register
    volatile MPEFEC_EFSAR_TYPE  EFSAR;          // 0x010  R/W  0x00000000   Erasure Flag Source Address Register
    volatile MPEFEC_MCR_TYPE    MCR;            // 0x014  R/W  0x00014000   MPEFEC Control Registe
    volatile MPEFEC_MSTR_TYPE   MSTR;           // 0x018  R    0x00000000   MPE_FEC Status [1] -> mpe_done [0] -> mpe_over
    volatile MPEFEC_MIER_TYPE   MIER;           // 0x01C  R    0x00000000   MPE_FEC IRQ Enable [1] -> mpe_done IRQ Enable [0] -> mpe_over IRQ Enable
    volatile MPEFEC_MICR_TYPE   MICR;           // 0x020  W    0x00000000   MPE_FEC IRQ_clear [1] -> mpe_done IRQ clear [0] -> mpe_over IRQ clear
    volatile MPEFEC_MARR_TYPE   MARR;           // 0x024  R/W  0x03FF0000   MPEFEC Active row Register
    volatile MPEFEC_ECR_TYPE    ECR;            // 0x028  R/W  0x00000000   Error Count Register
} MPEFEC, *PMPEFEC;


/*******************************************************************************
*   22. RTC(Real-Time Clock)                            (Base Addr = 0x76062000)
********************************************************************************/
#if 0	// The new structure is difficult. - 111110, hjbae
typedef struct {
    unsigned STARTB         :1;
    unsigned RTCWEN         :1;
    unsigned CLKSEL         :1;
    unsigned CNTSEL         :1;
    unsigned CLKRST         :1;
    unsigned OSCEN          :1;
    unsigned AIOUTEN        :1;
    unsigned WUOUTEN        :1;
    unsigned                :24;
} RTC_RTCCON_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_RTCCON_IDX_TYPE     bREG;
} RTC_RTCCON_TYPE;

typedef struct {
    unsigned INTWREN        :1;
    unsigned STATUS         :2;
    unsigned                :5;
    unsigned FSET           :3;
    unsigned                :1;
    unsigned XDRV           :2;
    unsigned                :1;
    unsigned PROT           :1;
    unsigned                :16;
} RTC_INTCON_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_INTCON_IDX_TYPE     bREG;
} RTC_INTCON_TYPE;

typedef struct {
    unsigned SECEN          :1;
    unsigned MINEN          :1;
    unsigned HOUREN         :1;
    unsigned DATEEN         :1;
    unsigned DAYEN          :1;
    unsigned MONEN          :1;
    unsigned YEAREN         :1;
    unsigned ALMEN          :1;
    unsigned                :24;
} RTC_RTCALM_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_RTCALM_IDX_TYPE     bREG;
} RTC_RTCALM_TYPE;

typedef struct {
    unsigned SECDATA        :7;
    unsigned                :25;
} RTC_SEC_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_SEC_IDX_TYPE        bREG;
} RTC_SEC_TYPE;

typedef struct {
    unsigned MINDATA        :7;
    unsigned                :25;
} RTC_MIN_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_MIN_IDX_TYPE        bREG;
} RTC_MIN_TYPE;

typedef struct {
    unsigned HOURDATA       :6;
    unsigned                :26;
} RTC_HOUR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_HOUR_IDX_TYPE       bREG;
} RTC_HOUR_TYPE;

typedef struct {
    unsigned DATEDATA       :6;
    unsigned                :26;
} RTC_DATE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_DATE_IDX_TYPE       bREG;
} RTC_DATE_TYPE;

typedef struct {
    unsigned DAYDATA        :3;
    unsigned                :29;
} RTC_DAY_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_DAY_IDX_TYPE        bREG;
} RTC_DAY_TYPE;

typedef struct {
    unsigned MONDATA        :5;
    unsigned                :27;
} RTC_MON_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_MON_IDX_TYPE        bREG;
} RTC_MON_TYPE;

typedef struct {
    unsigned YEARDATA       :16;
    unsigned                :16;
} RTC_YEAR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_YEAR_IDX_TYPE       bREG;
} RTC_YEAR_TYPE;

typedef struct {
    unsigned INTMODE        :2;
    unsigned WKUPMODE       :1;
    unsigned PWDN           :1;
    unsigned                :28;
} RTC_RTCIM_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_RTCIM_IDX_TYPE      bREG;
} RTC_RTCIM_TYPE;

typedef struct {
    unsigned PEND           :1;
    unsigned                :31;
} RTC_RTCPEND_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_RTCPEND_IDX_TYPE    bREG;
} RTC_RTCPEND_TYPE;

typedef struct {
    unsigned CNT            :4;
    unsigned                :2;
    unsigned ALM            :1;
    unsigned PRI            :1;
    unsigned                :24;
} RTC_RTCSTR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    RTC_RTCSTR_IDX_TYPE     bREG;
} RTC_RTCSTR_TYPE;

typedef struct _RTC{
    volatile RTC_RTCCON_TYPE    RTCCON;         // 0x000  R/W  0x00000000   RTC Control Register
    volatile RTC_INTCON_TYPE    INTCON;         // 0x004  R/W  -            RTC Interrupt Control Register
    volatile RTC_RTCALM_TYPE    RTCALM;         // 0x008  R/W  -            RTC Alarm Control Register
    volatile RTC_SEC_TYPE       ALMSEC;         // 0x00C  R/W  -            Alarm Second Data Register
    volatile RTC_MIN_TYPE       ALMMIN;         // 0x010  R/W  -            Alarm Minute Data Register
    volatile RTC_HOUR_TYPE      ALMHOUR;        // 0x014  R/W  -            Alarm Hour Data Register
    volatile RTC_DATE_TYPE      ALMDATE;        // 0x018  R/W  -            Alarm Date Data Register
    volatile RTC_DAY_TYPE       ALMDAY;         // 0x01C  R/W  -            Alarm Day of Week Data Register
    volatile RTC_MON_TYPE       ALMMON;         // 0x020  R/W  -            Alarm Month Data Register
    volatile RTC_YEAR_TYPE      ALMYEAR;        // 0x024  R/W  -            Alarm Year Data Register
    volatile RTC_SEC_TYPE       BCDSEC;         // 0x028  R/W  -            BCD Second Register
    volatile RTC_MIN_TYPE       BCDMIN;         // 0x02C  R/W  -            BCD Minute Register
    volatile RTC_HOUR_TYPE      BCDHOUR;        // 0x030  R/W  -            BCD Hour Register
    volatile RTC_DATE_TYPE      BCDDATE;        // 0x034  R/W  -            BCD Date Register
    volatile RTC_DAY_TYPE       BCDDAY;         // 0x038  R/W  -            BCD Day of Week Register
    volatile RTC_MON_TYPE       BCDMON;         // 0x03C  R/W  -            BCD Month Register
    volatile RTC_YEAR_TYPE      BCDYEAR;        // 0x040  R/W  -            BCD Year Register
    volatile RTC_RTCIM_TYPE     RTCIM;          // 0x044  R/W  -            RTC Interrupt Mode Register
    volatile RTC_RTCPEND_TYPE   RTCPEND;        // 0x048  R/W  -            RTC Interrupt Pending Register
    volatile RTC_RTCSTR_TYPE    RTCSTR;         // 0x04C  R/W  0x00000000   RTC Interrupt Status Register
} RTC, *PRTC;
#else
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
#endif


/*******************************************************************************
*   23. IOBUS Configuration Register                    (Base Addr = 0x76066000)
********************************************************************************/

typedef struct {
    unsigned SEL            :32;
} IO_DMAREQSEL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IO_DMAREQSEL_IDX_TYPE   bREG;
} IO_DMAREQSEL_TYPE;

typedef struct {
    unsigned EHI            :1;        // 00
    unsigned MPEFEC         :1;
    unsigned SDMMC0         :1;
    unsigned SDMMC1         :1;
    unsigned SDMMC2         :1;
    unsigned SDMMC3         :1;        // 05
    unsigned SDMMC_CTRL     :1;
    unsigned DMAC0          :1;
    unsigned DMAC1          :1;
    unsigned DMAC2          :1;
    unsigned DMAC_CH        :1;        // 10
    unsigned OVERLAY        :1;
    unsigned PWM            :1;
    unsigned SMC            :1;
    unsigned                :1;
    unsigned RTC            :1;        // 15
    unsigned REMOTE         :1;
    unsigned TSADC          :1;
    unsigned EDI            :1;
    unsigned EDI_CTRL       :1;
    unsigned SHOULD_BE_1    :1;        // 20
    unsigned ADMA0          :1;
    unsigned DAI0           :1;
    unsigned SPDIF0         :1;
    unsigned AUDIO0         :1;
    unsigned ADMA1          :1;        // 25
    unsigned DAI1           :1;
    unsigned SPDIF1         :1;
    unsigned AUDIO1         :1;
    unsigned I2CM0          :1;
    unsigned I2CM1          :1;        // 30
    unsigned I2CM2          :1;
} IO_HCLK0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IO_HCLK0_IDX_TYPE       bREG;
} IO_HCLK0_TYPE;

typedef struct {
    unsigned I2CM3          :1;     // 0
    unsigned I2CS0          :1;
    unsigned I2CS1          :1;
    unsigned I2C_CTRL       :1;
    unsigned UART0          :1;
    unsigned UART1          :1;     // 5
    unsigned UART2          :1;
    unsigned UART3          :1;
    unsigned UART4          :1;
    unsigned UART5          :1;
    unsigned UART6          :1;     // 10
    unsigned UART7          :1;
    unsigned UART_CTRL      :1;
    unsigned IDE            :1;
    unsigned NFC            :1;
    unsigned TSIF0          :1;     // 15
    unsigned TSIF1          :1;
    unsigned TSIF2          :1;
    unsigned TSIF_CTRL      :1;
    unsigned GPSB0          :1;
    unsigned GPSB1          :1;     // 20
    unsigned GPSB2          :1;
    unsigned GPSB3          :1;
    unsigned GPSB4          :1;
    unsigned GPSB5          :1;
    unsigned GPSB_CTRL      :1;     // 25
    unsigned OTG            :1;
    unsigned                :5;
} IO_HCLK1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IO_HCLK1_IDX_TYPE       bREG;
} IO_HCLK1_TYPE;

typedef struct {
    unsigned SG             :1;
    unsigned SR             :1;
    unsigned                :2;
    unsigned NS             :1;
    unsigned NP             :1;
    unsigned OS             :1;
    unsigned OP             :1;
    unsigned EP             :1;
    unsigned ES             :1;
    unsigned GS             :1;
    unsigned GP             :1;
    unsigned                :20;
} IO_MEMPWR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IO_MEMPWR_IDX_TYPE      bREG;
} IO_MEMPWR_TYPE;

typedef struct {
    unsigned RTCWAIT        :3;
    unsigned                :29;
} IO_RTCWAIT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IO_RTCWAIT_IDX_TYPE     bREG;
} IO_RTCWAIT_TYPE;

typedef struct {
    unsigned OMB            :1;
    unsigned                :15;
    unsigned A2XMOD0        :3;
    unsigned BW0            :1;
    unsigned A2XMOD1        :3;
    unsigned BW1            :1;
    unsigned A2XMOD2        :3;
    unsigned BW2            :1;
    unsigned                :4;
} IO_A2X_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IO_A2X_IDX_TYPE         bREG;
} IO_A2X_TYPE;

typedef struct {
    unsigned BWRAP          :4;
    unsigned                :28;
} IO_BWRAP_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IO_BWRAP_IDX_TYPE       bREG;
} IO_BWRAP_TYPE;

typedef struct _IOBUSCFG{
    volatile TCC_DEF32BIT_TYPE  USBOTG;         // 0x000  R/W  -            Refer to OTGCR in "14.2 Register Description for USB 2.0 OTG Controller"
    volatile IO_DMAREQSEL_TYPE  DMAREQSEL0;     // 0x004  R/W  -            DMA Request Selector for DMA0
    volatile IO_DMAREQSEL_TYPE  DMAREQSEL1;     // 0x008  R/W  -            DMA Request Selector for DMA1
    volatile IO_DMAREQSEL_TYPE  DMAREQSEL2;     // 0x00C  R/W  -            DMA Request Selector for DMA2
    volatile IO_HCLK0_TYPE      HCLKEN0;        // 0x010  R/W  0xFFFFFFFF   IOBUS AHB Clock Enable Register
    volatile IO_HCLK1_TYPE      HCLKEN1;        // 0x014  R/W  0xFFFFFFFF   IOBUS AHB Clock Enable Register
    volatile IO_HCLK0_TYPE      HRSTEN0;        // 0x018  R/W  0xFFFFFFFF   IOBUS AHB HRESET Control Register 0
    volatile IO_HCLK1_TYPE      HRSTEN1;        // 0x01C  R/W  0xFFFFFFFF   IOBUS AHB HRESET Control Register 1
    volatile IO_MEMPWR_TYPE     MEMPWR;         // 0x020  R/W  -            Memory Power Controll
    volatile IO_RTCWAIT_TYPE    RTCWAIT;        // 0x024  R/W  0x0000       RTC Wait Count
    volatile TCC_DEF32BIT_TYPE  USBOTG0;        // 0x028  R/W  -            Refer to UPCR0 in "14.2 Register Description for USB 2.0 OTG Controller"
    volatile TCC_DEF32BIT_TYPE  USBOTG1;        // 0x02C  R/W  -            Refer to UPCR1 in "14.2 Register Description for USB 2.0 OTG Controller"
    volatile TCC_DEF32BIT_TYPE  USBOTG2;        // 0x030  R/W  -            Refer to UPCR2 in "14.2 Register Description for USB 2.0 OTG Controller"
    volatile TCC_DEF32BIT_TYPE  USBOTG3;        // 0x034  R/W  -            Refer to UPCR3 in "14.2 Register Description for USB 2.0 OTG Controller"
    volatile IO_A2X_TYPE        IO_A2X;         // 0x038  R/W  0x00000077   IOBUS AHB2AXI Control Register
    volatile IO_BWRAP_IDX_TYPE  IO_BWRAP;       // 0x03C  R/W  0x00000000   Overay-Mixer BWRAP Control Register
} IOBUSCFG, *PIOBUSCFG;




/*******************************************************************************
*   etc
********************************************************************************/

typedef struct _NAND_EDI{
    volatile unsigned int   EDI_CTRL;           // 0x00  R/W   0x00000000   EDI Control Register.
    volatile unsigned int   EDI_CSNCFG0;        // 0x04  R/W   0x00543210   EDI CSN Configuration Register 0.
    volatile unsigned int   EDI_CSNCFG1;        // 0x08  R/W   0x00BA9876   EDI CSN Configuration Register 1.
    volatile unsigned int   EDI_WENCFG;         // 0x0C  R/W   0x00000004   EDI WEN Configuration Register
    volatile unsigned int   EDI_OENCFG;         // 0x10  R/W   0x00000004   EDI OEN Configuration Register
    volatile unsigned int   EDI_RDYCFG;         // 0x14  R/W   0x00043210   EDI Ready Configuration Register
    volatile unsigned int   _Reserved0;         // 0x18   -  -  -
    volatile unsigned int   _Reserved1;         // 0x1C   -  -
    volatile unsigned int   EDI_REQOFF;         // 0x20  R/W   0x00000000   EDI Request OFF Flag Register
} NAND_EDI, *NAND_PEDI;

/*******************************************************************************
*    7. DMA Controller Register Define (Base Addr = 0x76030000)
********************************************************************************/
typedef struct _NAND_GDMACTRL{
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
} NAND_GDMACTRL, *NAND_PGDMACTRL;

typedef struct _NAND_GDMANCTRL{
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
} NAND_GDMANCTRL, *NAND_PGDMANCTRL;

#endif /* _STRUCTURES_IO_H_ */
