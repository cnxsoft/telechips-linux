/*
 * linux/sound/soc/tcc/tcc-pcm.h   
 *
 * Based on:    linux/sound/arm/pxa2xx-pcm.h
 * Author:  <linux@telechips.com>
 * Created:	Nov 30, 2004
 * Modified:    Nov 25, 2008
 * Description: ALSA PCM interface for the Intel PXA2xx chip
 *
 * Copyright:	MontaVista Software, Inc.
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _tcc_PCM_H
#define _tcc_PCM_H

#include <mach/hardware.h>
#include <mach/bsp.h>

#define __play_buf_size 32768
//#define __play_buf_size 16384
//#define __play_buf_size 8192
//#define __play_buf_size 4096
#define __play_buf_cnt  32
//#define __play_buf_cnt  16

#define __cap_buf_size 4096
#define __cap_buf_cnt  32

#define DMA_CH_OUT 0x0001
#define DMA_CH_IN  0x0002
#define DMA_CH_SPDIF 0x0004

#define TCC_INTERRUPT_REQUESTED 0x0001
#define TCC_RUNNING_PLAY        0x0002
#define TCC_RUNNING_CAPTURE     0x0004
#define TCC_RUNNING_SPDIF       0x0008

#define TCC_TX_INTERRUPT_REQUESTED 0x0010
#define TCC_RX_INTERRUPT_REQUESTED 0x0020
#define NEXT_BUF(_s_,_b_) { \
    (_s_)->_b_##_idx++; \
    (_s_)->_b_##_idx %= (_s_)->nbfrags; \
    (_s_)->_b_ = (_s_)->buffers + (_s_)->_b_##_idx; }

typedef struct _tcc_interrupt_info_x {
    struct snd_pcm_substream *play_ptr;
    struct snd_pcm_substream *cap_ptr;
    struct snd_pcm_substream *spdif_ptr;
    struct mutex mutex;
    unsigned int flag;

    volatile ADMASPDIFTX adma_spdif_tx_base;
    volatile ADMA adma;
    volatile ADMADAI dai;
} tcc_interrupt_info_x;

extern tcc_interrupt_info_x tcc_alsa_info;

struct tcc_pcm_dma_params {
    char *name; /* stream identifier */
    int channel;
    dma_addr_t dma_addr;
    int dma_size; /* Size of the DMA transfer */
};

struct tcc_runtime_data {
    int dma_ch;
    struct tcc_pcm_dma_params *params;
    dma_addr_t dma_buffer;          /* physical address of dma buffer */
    dma_addr_t dma_buffer_end;      /* first address beyond DMA buffer */
    size_t period_size;
    size_t nperiod;     
    dma_addr_t period_ptr;          /* physical address of next period */
};

typedef struct {
    int size;       /* buffer size */
    char *start;        /* point to actual buffer */
    dma_addr_t dma_addr;    /* physical buffer address */
    struct semaphore sem;    /* down before touching the buffer */
    int master;     /* owner for buffer allocation, contain size when true */
    int dma_size;
} audio_buf_t;

typedef struct {
    audio_buf_t *buffers;   /* pointer to audio buffer structures */
    audio_buf_t *buf;   /* current buffer used by read/write */
    u_int buf_idx;      /* index for the pointer above */
    u_int play_idx;     /* the current buffer that playing */
    u_int fragsize;     /* fragment i.e. buffer size */
    u_int nbfrags;      /* nbr of fragments */
} audio_stream_t;

typedef struct {
    char *addr;
    dma_addr_t dma_addr;
    int  buf_size;  // total size of DMA
    int  page_size; // size of each page
    int  dma_half;  // 0 or 1, mark the next available buffer
}dma_buf_t;

#ifdef CONFIG_ARCH_TCC79X
#define HwDMA_GDMA 0xF0040000

#define TCC_TX_DMA_CH_NUM 0
#define TCC_RX_DMA_CH_NUM 1

#define HwDMA_GMDA_BASE(x)	(HwDMA_GDMA + ((x) * 0x100))
#define HwDMA_CHANNEL(x) (0x30 * (x))

#define DMA_Base HwDMA_GMDA_BASE(0)
#define Tx_DMA_Base DMA_Base + HwDMA_CHANNEL(TCC_TX_DMA_CH_NUM)
#define Rx_DMA_Base DMA_Base + HwDMA_CHANNEL(TCC_RX_DMA_CH_NUM)

#define Rx_ST_SADR *(volatile unsigned long *)(Rx_DMA_Base)
#define Rx_SPARAM  *(volatile unsigned long *)(Rx_DMA_Base + 0x04)
#define Rx_C_SADR  *(volatile unsigned long *)(Rx_DMA_Base + 0x0C)
#define Rx_ST_DADR *(volatile unsigned long *)(Rx_DMA_Base + 0x10)
#define Rx_DPARAM  *(volatile unsigned long *)(Rx_DMA_Base + 0x14)
#define Rx_C_DADR  *(volatile unsigned long *)(Rx_DMA_Base + 0x1C)
#define Rx_HCOUNT  *(volatile unsigned long *)(Rx_DMA_Base + 0x20)
#define Rx_CHCTRL  *(volatile unsigned long *)(Rx_DMA_Base + 0x24)
#define Rx_RPTCTRL *(volatile unsigned long *)(Rx_DMA_Base + 0x28)
#define Rx_EXTREQ  *(volatile unsigned long *)(Rx_DMA_Base + 0x2C)
#define Rx_CHCONFIG  *(volatile unsigned long *)(DMA_Base+0x90)

#define Tx_ST_SADR *(volatile unsigned long *)(Tx_DMA_Base)
#define Tx_SPARAM  *(volatile unsigned long *)(Tx_DMA_Base + 0x04)
#define Tx_C_SADR  *(volatile unsigned long *)(Tx_DMA_Base + 0x0C)
#define Tx_ST_DADR *(volatile unsigned long *)(Tx_DMA_Base + 0x10)
#define Tx_DPARAM  *(volatile unsigned long *)(Tx_DMA_Base + 0x14)
#define Tx_C_DADR  *(volatile unsigned long *)(Tx_DMA_Base + 0x1C)
#define Tx_HCOUNT  *(volatile unsigned long *)(Tx_DMA_Base + 0x20)
#define Tx_CHCTRL  *(volatile unsigned long *)(Tx_DMA_Base + 0x24)
#define Tx_RPTCTRL *(volatile unsigned long *)(Tx_DMA_Base + 0x28)
#define Tx_EXTREQ  *(volatile unsigned long *)(Tx_DMA_Base + 0x2C)
#define Tx_CHCONFIG  *(volatile unsigned long *)(DMA_Base+0x90)

#endif


#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
extern void tea_spdif_setclock(unsigned int clock_rate);
#endif

/* platform data */
//extern struct snd_soc_platform tcc_soc_platform;

#endif
