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

#ifndef _STRUCTURES_MEMORY_H_
#define _STRUCTURES_MEMORY_H_


/*******************************************************************************
*
*    TCC892x DataSheet PART 4 MEMORY BUS
*
********************************************************************************/

/************************************************************************
*   5. Memory BUS Configuration                  (Base Addr = 0x73810000)
************************************************************************/

typedef struct {
    unsigned LPCON          :1;
    unsigned DENALI         :1;
    unsigned PHY            :1;
    unsigned                :1;
    unsigned DRAM           :1;
    unsigned SRAM           :1;
    unsigned OTPROM         :1;
    unsigned BUSCFG         :1;
    unsigned IOBUS          :1;
    unsigned DBUS           :1;
    unsigned GBUS           :1;
    unsigned VBUS           :1;
    unsigned HSIOBUS        :1;
    unsigned ARM            :1;
    unsigned                :18;
} MEMBUSCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MEMBUSCFG_IDX_TYPE      bREG;
} MEMBUSCFG_TYPE;

typedef struct {
    unsigned                :26;
    unsigned TLBRDW         :2;
    unsigned RAMRDW         :2;
    unsigned ROMRDW         :2;
} IMEMCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    IMEMCFG_IDX_TYPE        bREG;
} IMEMCFG_TYPE;

typedef struct {
    unsigned IMEMBUS        :1;     // TZPROT[0]
    unsigned OTPROM         :1;     // TZPROT[1]
    unsigned REGCTRL        :1;     // TZPROT[2]
    unsigned DDRMEMBUS      :1;     // TZPROT[3]
    unsigned NOTUSED        :1;     // TZPROT[4]
    unsigned                :10;
    unsigned DCLKR          :1;
    unsigned REMAP          :2;
    unsigned                :13;
    unsigned RMIR           :1;
} MBUSCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MBUSCFG_IDX_TYPE        bREG;
} MBUSCFG_TYPE;

typedef struct {
    unsigned IOBUS          :1;
    unsigned CPUBUS         :1;
    unsigned DBUS0          :1;
    unsigned HSIOBUS        :1;
    unsigned VBUS0          :1;
    unsigned GBUS           :1;
    unsigned DBUS1          :1;
    unsigned VBUS1          :1;
    unsigned ALL_OR_ED      :1;
    unsigned NOTUSED        :1;
    unsigned                :22;
} MBUSSTS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    MBUSSTS_IDX_TYPE        bREG;
} MBUSSTS_TYPE;

typedef struct _MEMBUSCFG {
    volatile MEMBUSCFG_TYPE     HCLKMASK;       // 0x000  R/W  0x00000000   Memory Bus HCLK Mask Register
    volatile MEMBUSCFG_TYPE     SWRESET;        // 0x004  R/W  0x00000000   Memory Bus Soft Reset Register
    unsigned :32;
    volatile IMEMCFG_TYPE       IMEMCFG;        // 0x00C  R/W  -            Internal Memory Configuration Register
    volatile MBUSCFG_TYPE       MBUSCFG;        // 0x010  R/W  -            Memory Bus Configuration Register
    volatile MBUSSTS_TYPE       MBUSSTS;        // 0x014  R    -            Memory Bus Status Register
} MEMBUSCFG, *PMEMBUSCFG;


#endif /* _STRUCTURES_MEMORY_H_ */
