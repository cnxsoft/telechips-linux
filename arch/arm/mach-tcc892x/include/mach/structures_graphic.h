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

#ifndef _STRUCTURES_GRAPHIC_H_
#define _STRUCTURES_GRAPHIC_H_


/*******************************************************************************
*
*    TCC892x DataSheet PART 3 GRAPHIC BUS
*
********************************************************************************/

/************************************************************************
*   4. GRPBUS Configuration                      (Base Addr = 0x70004000)
************************************************************************/

typedef struct {
    unsigned GRP            :1;
    unsigned                :31;
} GRPBUSCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GRPBUSCFG_IDX_TYPE      bREG;
} GRPBUSCFG_TYPE;

typedef struct {
    unsigned L2C            :1;
    unsigned GPMMU          :1;
    unsigned GP             :1;
    unsigned PPMMU          :1;
    unsigned PP             :1;
    unsigned                :27;
} GPU_IDLESTS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    GPU_IDLESTS_IDX_TYPE    bREG;
} GPU_IDLESTS_TYPE;

typedef struct {
    unsigned WC_SEL         :4;
    unsigned WC             :4;
    unsigned RC_SEL         :4;
    unsigned RC             :4;
    unsigned                :16;
} GPU_CACHE_CTRL_TYPE;

typedef union {
    unsigned long           nREG;
    GPU_CACHE_CTRL_TYPE     bREG;
} GPU_CACHE_CTRL;

typedef struct _GRPBUSCFG {
    volatile GRPBUSCFG_TYPE     PWDN;           // 0x000  R/W  0x00000001   Graphics bus power down
    volatile GRPBUSCFG_TYPE     SWRESET;        // 0x004  R/W  0x00000001   Graphics bus software reset
    unsigned :32;
    volatile GPU_IDLESTS_TYPE   GPU_IDLESTS;    // 0x00C  R    -            GPU idle status
    volatile GPU_CACHE_CTRL     GPU_CACHECTRL;  // 0x010  R/W  0x00000000   GPU system level cache control
} GRPBUSCFG, *PGRPBUSCFG;

#endif /* _STRUCTURES_GRAPHIC_H_ */
