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

#ifndef _STRUCTURES_VIDEO_H_
#define _STRUCTURES_VIDEO_H_


/*******************************************************************************
*
*    TCC892x DataSheet PART 8 VIDEO BUS
*
********************************************************************************/

/************************************************************************
*   4. JPEG Encoder (Base Addr = 0x00000000) // R/W
*************************************************************************/



/************************************************************************
*   6. Video Bus Configuration (Base Addr = 0x75200000) // R/W
*************************************************************************/
typedef struct {
    unsigned JENC       	:1;
    unsigned 				:1;
    unsigned VCOD       	:1;
    unsigned VBUS       	:1;
    unsigned                :28;
} VIDEOBUSCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    VIDEOBUSCFG_IDX_TYPE    bREG;
} VIDEOBUSCFG_TYPE;

typedef struct _VIDEOBUSCFG {
    volatile VIDEOBUSCFG_TYPE     PWDN;           // 0x000  R/W  0x00000001   Video bus power down
    volatile VIDEOBUSCFG_TYPE     SWRESET;        // 0x004  R/W  0x00000001   Video bus software reset
} VIDEOBUSCFG, *PVIDEOBUSCFG;

#endif /* _STRUCTURES_VIDEO_H_ */
