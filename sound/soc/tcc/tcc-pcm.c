/*
 * linux/sound/soc/tcc/tcc9200-pcm.c  
 *
 * Based on:    linux/sound/arm/pxa2xx-pcm.c
 * Author:  <linux@telechips.com>
 * Created: Nov 30, 2004
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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>

#include <linux/delay.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/tca_i2s.h>

#include "tcc-i2s.h"
#include "tcc-pcm.h"

#include "tcc/tca_tcchwcontrol.h"

#undef alsa_dbg
#if 0
#define alsa_dbg(f, a...)  printk("== alsa-debug == " f, ##a)
#else
#define alsa_dbg(f, a...)
#endif

#if defined(CONFIG_PM)
static ADMA gADMA;
#if defined(CONFIG_ARCH_TCC892X)
static ADMA gADMA1;
#endif
#endif

tcc_interrupt_info_x tcc_alsa_info;
EXPORT_SYMBOL(tcc_alsa_info);

static const struct snd_pcm_hardware tcc_pcm_hardware_play = {
    .info = (SNDRV_PCM_INFO_MMAP
             | SNDRV_PCM_INFO_MMAP_VALID
             | SNDRV_PCM_INFO_INTERLEAVED
             | SNDRV_PCM_INFO_BLOCK_TRANSFER
             | SNDRV_PCM_INFO_PAUSE
             | SNDRV_PCM_INFO_RESUME),

    .formats = (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_U16_LE | SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_LE),	// Planet 20120306
#if defined(CONFIG_SND_SOC_TCC_MULTICHANNEL)
    .rates        = SNDRV_PCM_RATE_8000_192000,
    .rate_min     = 8000,
    .rate_max     = 192000,
    .channels_min = 2,
    .channels_max = 8,
#else
    .rates        = SNDRV_PCM_RATE_8000_96000,
    .rate_min     = 8000,
    .rate_max     = 96000,
    .channels_min = 2,
    .channels_max = 2,
#endif
    .period_bytes_min = 128,
    .period_bytes_max = __play_buf_size,
    .periods_min      = 2,
    .periods_max      = __play_buf_cnt,
    .buffer_bytes_max = __play_buf_cnt * __play_buf_size,
    .fifo_size = 16,  //ignored
};

static const struct snd_pcm_hardware tcc_pcm_hardware_capture = {
    .info = (SNDRV_PCM_INFO_MMAP
             | SNDRV_PCM_INFO_MMAP_VALID
             | SNDRV_PCM_INFO_INTERLEAVED
             | SNDRV_PCM_INFO_BLOCK_TRANSFER
             | SNDRV_PCM_INFO_PAUSE
             | SNDRV_PCM_INFO_RESUME),

    .formats      = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE,
    .rates        = SNDRV_PCM_RATE_8000_96000,
    .rate_min     = 8000,
    .rate_max     = 96000,
    .channels_min = 2,
    .channels_max = 2,                                

    .period_bytes_min = 128,
    .period_bytes_max = __cap_buf_size,
    .periods_min      = 2,
    .periods_max      = __cap_buf_cnt,
    .buffer_bytes_max = __cap_buf_cnt * __cap_buf_size,
    .fifo_size = 16, //ignored
};

static int alsa_get_intr_num(struct snd_pcm_substream *substream)
{
    if (substream) {
#if defined(CONFIG_ARCH_TCC892X) || defined(CONFIG_ARCH_TCC93XX)

#if defined(CONFIG_MACH_M805_892X)
        return INT_ADMA1;
#else
#if defined(CONFIG_ARCH_TCC892X)
        if(substream->pcm->device == __SPDIF_DEV_NUM__)
        {
            return INT_ADMA1;
        }
        else
        {
            return INT_ADMA0;
        }
#else
        return INT_ADMA0;
#endif
#endif	/* #if defined(CONFIG_MACH_M805_892X) */

#else
        return INT_ADMA;
#endif
    }
    return -1;
}

static void set_dma_outbuffer(unsigned int addr, unsigned int length, unsigned int period)
{
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);
#else
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
#endif
    unsigned long dma_buffer = 0;
	int bit_count;

    BITCLR(pADMA->ChCtrl, Hw28);

//    dma_buffer = 0xFFFFFF00 / ((length >> 4) << 8);
//    dma_buffer = dma_buffer * ((length >> 4) << 8);

	bit_count = 31;
	while(bit_count > 3)
	{
		if((0x00000001 << bit_count) & length) 
			break;

		bit_count--;
	};

	if(bit_count <= 3)
	{
		printk("Error : not valid length\n");
		return;
	}
	else
	{
		length = 0x00000001<<bit_count;
		alsa_dbg("[%s], original len[%u]\n", __func__, length);
		printk("[%s], original len[%u]\n", __func__, length);
	}
	
    dma_buffer = 0xFFFFFF00 & (~((length<<4)-1));

    pADMA->TxDaParam = dma_buffer | 4;
    // generate interrupt when half of buffer was played
    pADMA->TxDaTCnt = (period >> 0x05) - 1;

    alsa_dbg("[%s], addr[0x%08X], len[%u], period[%u]\n", __func__, addr, length, period);
    alsa_dbg("[%s] HwTxDaParam [0x%X]\n", __func__, (int)(dma_buffer | 4));
    alsa_dbg("[%s] HwTxDaTCnt [%d]\n", __func__, ((period) >> 0x05) - 1);
    pADMA->TxDaSar = addr;

    pADMA->TransCtrl |= (Hw28 | Hw16 | (Hw9|Hw8) | (Hw0 | Hw1));
    pADMA->RptCtrl = 0;
    pADMA->ChCtrl |= Hw12;
}

static void set_dma_spdif_outbuffer(unsigned int addr, unsigned int length, unsigned int period)
{
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA1);
    volatile PADMASPDIFTX pADMASPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX1);
#else
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
	volatile PADMASPDIFTX pADMASPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX);
#endif
    unsigned long dma_buffer = 0;
	int bit_count;

    alsa_dbg("%s, addr[0x%08X], len[%u], period[%u]\n", __func__, addr, length, period);
    BITCLR(pADMA->ChCtrl, Hw29);

//    dma_buffer = 0xFFFFFF00 / (((length) >> 4) << 8);
//    dma_buffer = dma_buffer * (((length) >> 4) << 8);

	bit_count = 31;
	while(bit_count > 3)
	{
		if((0x00000001 << bit_count) & length) 
			break;

		bit_count--;
	};

	if(bit_count <= 3)
	{
		printk("Error : not valid length\n");
		return;
	}
	else
	{
		length = 0x00000001<<bit_count;
		alsa_dbg("[%s], original len[%u]\n", __func__, length);
		printk("[%s], original len[%u]\n", __func__, length);
	}
	
    dma_buffer = 0xFFFFFF00 & (~((length<<4)-1));

    pADMA->TxSpParam = dma_buffer | 4;
    // generate interrupt when half of buffer was played
#if defined(CONFIG_ARCH_TCC892X)
    pADMA->TxSpTCnt = (period >> 0x04) - 1;
#else
    pADMA->TxSpTCnt = (period >> 0x05) - 1;
#endif

    alsa_dbg("[%s] HwTxDaParam [0x%X]\n", __func__, (int)(dma_buffer | 4));
    alsa_dbg("[%s] HwTxDaTCnt [%d]\n", __func__, ((period) >> 0x05) - 1);

    pADMA->TxSpSar = addr;
#if defined(CONFIG_ARCH_TCC892X)
    pADMA->TransCtrl |= (Hw28 | Hw17 | (Hw11) | (Hw3 | Hw2));
#else
    pADMA->TransCtrl |= (Hw28 | Hw17 | (Hw11|Hw10) | (Hw3 | Hw2));
#endif
    pADMA->RptCtrl = 0;

    memset((void *)pADMASPDIFTX->UserData, 0, 24);
    memset((void *)pADMASPDIFTX->ChStatus, 0, 24);
    memset((void *)pADMASPDIFTX->TxBuffer, 0, 128);

    pADMASPDIFTX->TxConfig |= HwZERO | Hw8 | Hw1;
    pADMASPDIFTX->TxBuffer[0] = 0;

    /*
     * Hw2: interrupt output enable
     * Hw1: data being valid
     * Hw0: enable the transmission
     */
    pADMASPDIFTX->TxConfig |= HwZERO | Hw1 | Hw0;
    pADMASPDIFTX->TxIntStat = 0x1E; /* Clear Interrupt Status */


    alsa_dbg("%s, spdif current addr[0x%08X] \n", __func__, pADMA->TxSpCsar);
    pADMA->ChCtrl |= (Hw13);
}

static void set_dma_inbuffer(unsigned int addr, unsigned int length, unsigned int period)
{
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);
#else
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
#endif
    unsigned long dma_buffer = 0;
	int bit_count;


    BITCLR(pADMA->ChCtrl, Hw30);

//    dma_buffer = 0xFFFFFF00 / ((length >> 4) << 8);
//    dma_buffer = dma_buffer * ((length >> 4) << 8);

	bit_count = 31;
	while(bit_count > 3)
	{
		if((0x00000001 << bit_count) & length) 
			break;

		bit_count--;
	};

	if(bit_count <= 3)
	{
		printk("Error : not valid length\n");
		return;
	}
	else
	{
		length = 0x00000001<<bit_count;
		alsa_dbg("[%s], original len[%u]\n", __func__, length);
		printk("[%s], original len[%u]\n", __func__, length);
	}
	
    dma_buffer = 0xFFFFFF00 & (~((length<<4)-1));

    pADMA->RxDaParam = dma_buffer | 4;
    // generate interrupt when half of buffer was filled
    pADMA->RxDaTCnt = (period >> 0x05) - 1;

    alsa_dbg("[%s], addr[0x%08X], len[%d]\n", __func__, addr, length);
    alsa_dbg("[%s] HwRxDaParam [0x%X]\n", __func__, (int)dma_buffer | 4);
    alsa_dbg("[%s] HwRxDaTCnt [%d]\n", __func__, ((period) >> 0x04) - 1);

    pADMA->RxDaDar = addr;
    pADMA->TransCtrl |= (Hw29 | Hw18 | (Hw13|Hw12) | (Hw4 | Hw5) );
    pADMA->RptCtrl = 0;
    pADMA->ChCtrl |= Hw14;
}

static irqreturn_t tcc_dma_done_handler(int irq, void *dev_id)
{
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);
#else
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
#endif
    struct snd_pcm_runtime *runtime;
    struct tcc_runtime_data *prtd;
    int dmaInterruptSource = 0;

    unsigned long reg_temp = 0;                 

    if (pADMA->IntStatus & (Hw4 | Hw0)) {
        dmaInterruptSource |= DMA_CH_OUT;
        reg_temp |= Hw4 | Hw0;
    }
    if (pADMA->IntStatus & (Hw6 | Hw2)) {
        dmaInterruptSource |= DMA_CH_IN;
        reg_temp |= Hw6 | Hw2;
    }
    if (pADMA->IntStatus & (Hw5 | Hw1)) {
        dmaInterruptSource |= DMA_CH_SPDIF;
        reg_temp |= Hw5 | Hw1;
    }

    if (reg_temp) { 
        pADMA->IntStatus |= reg_temp;
    }

    if ((dmaInterruptSource & DMA_CH_SPDIF)
        && (tcc_alsa_info.flag & TCC_RUNNING_SPDIF)) {

        snd_pcm_period_elapsed(tcc_alsa_info.spdif_ptr);

        runtime = tcc_alsa_info.spdif_ptr->runtime;
        prtd = (struct tcc_runtime_data *)runtime->private_data;
    }

    if ((dmaInterruptSource & DMA_CH_OUT)
        && (tcc_alsa_info.flag & TCC_RUNNING_PLAY)) {
        snd_pcm_period_elapsed(tcc_alsa_info.play_ptr);

        runtime = tcc_alsa_info.play_ptr->runtime;
        prtd = (struct tcc_runtime_data *) runtime->private_data;
    }
    if ((dmaInterruptSource & DMA_CH_IN)
        && (tcc_alsa_info.flag & TCC_RUNNING_CAPTURE)) {
        snd_pcm_period_elapsed(tcc_alsa_info.cap_ptr);

        runtime = tcc_alsa_info.cap_ptr->runtime;
        prtd = (struct tcc_runtime_data *)runtime->private_data;
    }

    return IRQ_HANDLED;
}

#if defined(CONFIG_ARCH_TCC892X)
static irqreturn_t tcc_dma1_done_handler(int irq, void *dev_id)
{
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA1);
    struct snd_pcm_runtime *runtime;
    struct tcc_runtime_data *prtd;
    int dmaInterruptSource = 0;

    unsigned long reg_temp = 0;                 

    if (pADMA->IntStatus & (Hw4 | Hw0)) {
        dmaInterruptSource |= DMA_CH_OUT;
        reg_temp |= Hw4 | Hw0;
    }
    if (pADMA->IntStatus & (Hw6 | Hw2)) {
        dmaInterruptSource |= DMA_CH_IN;
        reg_temp |= Hw6 | Hw2;
    }
    if (pADMA->IntStatus & (Hw5 | Hw1)) {
        dmaInterruptSource |= DMA_CH_SPDIF;
        reg_temp |= Hw5 | Hw1;
    }

    if (reg_temp) { 
        pADMA->IntStatus |= reg_temp;
    }

    if ((dmaInterruptSource & DMA_CH_SPDIF)
        && (tcc_alsa_info.flag & TCC_RUNNING_SPDIF)) {

        snd_pcm_period_elapsed(tcc_alsa_info.spdif_ptr);

        runtime = tcc_alsa_info.spdif_ptr->runtime;
        prtd = (struct tcc_runtime_data *)runtime->private_data;
    }

    if ((dmaInterruptSource & DMA_CH_OUT)
        && (tcc_alsa_info.flag & TCC_RUNNING_PLAY)) {
        snd_pcm_period_elapsed(tcc_alsa_info.play_ptr);

        runtime = tcc_alsa_info.play_ptr->runtime;
        prtd = (struct tcc_runtime_data *) runtime->private_data;
    }
    if ((dmaInterruptSource & DMA_CH_IN)
        && (tcc_alsa_info.flag & TCC_RUNNING_CAPTURE)) {
        snd_pcm_period_elapsed(tcc_alsa_info.cap_ptr);

        runtime = tcc_alsa_info.cap_ptr->runtime;
        prtd = (struct tcc_runtime_data *)runtime->private_data;
    }

    return IRQ_HANDLED;
}
#endif

static dma_addr_t get_dma_addr_offset(dma_addr_t start_addr, dma_addr_t cur_addr, unsigned int total_size, unsigned int dma_len)
{
    if ((start_addr <= cur_addr) && ((start_addr + total_size) > cur_addr)) {
        unsigned long remainder = cur_addr - start_addr;
        return (remainder - (remainder % dma_len));
    }
    return 0;
}

static int tcc_pcm_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
    struct snd_pcm_runtime     *runtime = substream->runtime;
    struct tcc_runtime_data    *prtd    = runtime->private_data;
    struct snd_soc_pcm_runtime *rtd     = substream->private_data;
    struct tcc_pcm_dma_params  *dma;

    size_t totsize = params_buffer_bytes(params);
    size_t period = params_period_bytes(params); 
    size_t period_bytes = params_period_bytes(params);
    int chs = params_channels(params);
    int ret = 0;

#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA    pADMA     = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);
    volatile PADMADAI pADMA_DAI = (volatile PADMADAI)tcc_p2v(BASE_ADDR_DAI0);
	volatile PADMASPDIFTX pADMA_SPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX1);
    irq_handler_t handler;

    if (substream->pcm->device == __SPDIF_DEV_NUM__)
    {
        pADMA     = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA1);
        handler   = tcc_dma1_done_handler;
    }
    else
        handler   = tcc_dma_done_handler;
#else
    volatile PADMA    pADMA     = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
    volatile PADMADAI pADMA_DAI = (volatile PADMADAI)tcc_p2v(BASE_ADDR_DAI);
	volatile PADMASPDIFTX pADMA_SPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX);
#endif

    dma = snd_soc_dai_get_dma_data(rtd->cpu_dai, substream);

    /* return if this is a bufferless transfer e.g.
     * codec <--> BT codec or GSM modem -- lg FIXME */
    if (!dma) {
        return -EACCES;
    }

    /* this may get called several times by oss emulation
     * with different params */
    if (prtd->params == NULL) {
        prtd->params = dma;
        alsa_dbg(" prtd->params = dma\n");

        mutex_lock(&(tcc_alsa_info.mutex));
        if (!(tcc_alsa_info.flag & TCC_INTERRUPT_REQUESTED)) {
#if defined(CONFIG_ARCH_TCC892X)
            ret = request_irq(alsa_get_intr_num(substream),
                              handler,
                              IRQF_SHARED,
                              "alsa-audio",
                              &tcc_alsa_info);
#else
            ret = request_irq(alsa_get_intr_num(substream),
                              tcc_dma_done_handler,
                              IRQF_SHARED,
                              "alsa-audio",
                              &tcc_alsa_info);
#endif
            if (ret < 0) {
                mutex_unlock(&(tcc_alsa_info.mutex));
                return -EBUSY;
            }
            tcc_alsa_info.flag |= TCC_INTERRUPT_REQUESTED;
        }
        mutex_unlock(&(tcc_alsa_info.mutex));

        if (substream->pcm->device == __SPDIF_DEV_NUM__) {
            prtd->dma_ch = DMA_CH_SPDIF;
        } else {
            prtd->dma_ch = (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) ? DMA_CH_OUT : DMA_CH_IN;
        }
    } else {
        /* should works more? */
    }        

    //DMA 버퍼 컨트롤
    //DMA 전송 버퍼 포인터 초기화 

    snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);        
    runtime->dma_bytes = totsize;

    prtd->dma_buffer_end = runtime->dma_addr + totsize;
    prtd->dma_buffer = runtime->dma_addr;
    prtd->period_size = period_bytes;
    prtd->nperiod = period;

    alsa_dbg(" totsize=0x%X period=0x%X  period num=%d, rate=%d, chs=%d\n",
             totsize, period_bytes, period, params_rate(params), params_rate(params));

    {
            dma_addr_t offset = 0;
            dma_addr_t cur_addr = 0;
            unsigned int total_dma_size = 0;

            if (substream->pcm->device == __SPDIF_DEV_NUM__) {
                cur_addr = pADMA->TxSpCsar;
                total_dma_size = tcc_pcm_hardware_play.buffer_bytes_max;
            } else {
                cur_addr = (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) ? pADMA->TxDaCsar : pADMA->RxDaCdar;
                total_dma_size = tcc_pcm_hardware_capture.buffer_bytes_max;
            }

            offset = get_dma_addr_offset(prtd->dma_buffer, cur_addr, total_dma_size, totsize);

            prtd->dma_buffer += offset;
            prtd->dma_buffer_end = prtd->dma_buffer + totsize;

            runtime->dma_addr = prtd->dma_buffer;
            runtime->dma_area += offset;

            alsa_dbg("########## dma_addr[0x%X], cur_addr[0x%X], totsize[0x%X], period[0x%X], chs[%d]\n",
                     runtime->dma_addr, cur_addr, totsize, period, chs);
    }

    if (substream->pcm->device == __SPDIF_DEV_NUM__) {
        if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
            set_dma_spdif_outbuffer(prtd->dma_buffer, totsize, period_bytes);
        } else {
            printk("cannot support S/PDIF capture !!!!!\n");
            return -1;
        }
    } else {
        if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
            set_dma_outbuffer(prtd->dma_buffer, totsize, period_bytes);
        } else {
            set_dma_inbuffer(prtd->dma_buffer, totsize, period_bytes);
        }
    }

#if defined(CONFIG_ARCH_TCC892X)
    if (substream->pcm->device == __I2S_DEV_NUM__) {
        BITCLR(pADMA_DAI->MCCR0, Hw31 | Hw30 | Hw29 | Hw28);
        if (chs > 2) {                                  // 5.1ch or 7.1ch
            pADMA_DAI->DAMR |= (Hw29 | Hw28);
#if defined(CONFIG_ARCH_TCC892X)
            pADMA->RptCtrl  |= (Hw26 | Hw25 | Hw24);
#else
            pADMA->RptCtrl  |= (Hw25 | Hw24);
#endif
            pADMA->ChCtrl   |= (Hw24 | Hw21 | Hw20);
        }
        else {                                          // 2 ch
            BITSET(pADMA_DAI->DAMR, (Hw29));
#if defined(CONFIG_ARCH_TCC892X)

#if defined(CONFIG_MACH_M805_892X)
            BITSET(pADMA->RptCtrl, (Hw24));
#else
            BITSET(pADMA->RptCtrl, (Hw26 | Hw25 | Hw24));
#endif

#else
            BITSET(pADMA->RptCtrl, (Hw25 | Hw24));
#endif
            BITCLR(pADMA_DAI->DAMR, (Hw28));
            BITCLR(pADMA->ChCtrl,(Hw24 | Hw21 | Hw20));
        }
    }
    else
    {
        BITCLR(pADMA->RptCtrl, (Hw26 | Hw25 | Hw24));
        BITCLR(pADMA->ChCtrl,(Hw24 | Hw21 | Hw20));
    }
#else
    BITCLR(pADMA_DAI->MCCR0, Hw31 | Hw30 | Hw29 | Hw28);
    if (chs > 2) {                                  // 5.1ch or 7.1ch
        pADMA_DAI->DAMR |= (Hw29 | Hw28);
#if defined(CONFIG_ARCH_TCC892X)
        pADMA->RptCtrl  |= (Hw26 | Hw25 | Hw24);
#else
        pADMA->RptCtrl  |= (Hw25 | Hw24);
#endif
        pADMA->ChCtrl   |= (Hw24 | Hw21 | Hw20);
    }
    else {                                          // 2 ch
        BITSET(pADMA_DAI->DAMR, (Hw29));
#if defined(CONFIG_ARCH_TCC892X)

#if defined(CONFIG_MACH_M805_892X)
        BITSET(pADMA->RptCtrl, (Hw24));
#else
        BITSET(pADMA->RptCtrl, (Hw26 | Hw25 | Hw24));
#endif

#else
        BITSET(pADMA->RptCtrl, (Hw25 | Hw24));
#endif
        BITCLR(pADMA_DAI->DAMR, (Hw28));
        BITCLR(pADMA->ChCtrl,(Hw24 | Hw21 | Hw20));
    }
#endif

	if (substream->pcm->device == __SPDIF_DEV_NUM__) {
		//Set Audio or Data Format
		printk("%s : runtime->format11 = %d\n", __func__, params_format(params));

		if (params_format(params) == SNDRV_PCM_FORMAT_U16) {
			BITSET(pADMA_SPDIFTX->TxChStat, Hw0);	//Data format
			printk("%s : set SPDIF TX Channel STATUS to Data format \n",__func__);
		}
		else {
			BITCLR(pADMA_SPDIFTX->TxChStat, Hw0);	//Audio format
			printk("%s : set SPDIF TX Channel STATUS to PCM format \n",__func__);
		}
	}


    return ret;
}

static int tcc_pcm_hw_free(struct snd_pcm_substream *substream)
{
    struct tcc_runtime_data *prtd = substream->runtime->private_data;

    alsa_dbg("[%s] \n", __func__);

    if (prtd->dma_ch) {
        snd_pcm_set_runtime_buffer(substream, NULL);
        //추가 필요  ??
    }

    return 0;
}

static int tcc_pcm_prepare(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    alsa_dbg("[%s] \n", __func__);

    //DMA initialize

    /* Disable Noise */
    //memset((void *)(&HwDADO_L0), 0, 32);
    memset(runtime->dma_area, 0x00, runtime->dma_bytes);

    return 0;
}


static void tcc_pcm_delay(int delay)
{
    volatile int loop;

    for(loop = 0;loop < delay;loop++)
        ;
}

static int tcc_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
    //struct tcc_runtime_data *prtd = substream->runtime->private_data;
    //    audio_stream_t *s = &output_stream;        
    int ret = 0;
#if defined(CONFIG_ARCH_TCC892X)
	volatile PADMADAI pDAI  = (volatile PADMADAI)tcc_p2v(BASE_ADDR_DAI0);
	volatile PADMASPDIFTX pADMASPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX1);
	volatile PADMA    pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);

	if (substream->pcm->device == __SPDIF_DEV_NUM__) {
		pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA1);
	}
#else
    volatile PADMADAI pDAI  = (volatile PADMADAI)tcc_p2v(BASE_ADDR_DAI);
    volatile PADMA    pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
	volatile PADMASPDIFTX pADMASPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX);
#endif

    alsa_dbg("%s() cmd[%d] frame_bits[%d]\n", __func__, cmd, substream->runtime->frame_bits);
    switch (cmd) {
    case SNDRV_PCM_TRIGGER_START:
    case SNDRV_PCM_TRIGGER_RESUME:        
    case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
        if (substream->pcm->device == __SPDIF_DEV_NUM__) {
            pADMASPDIFTX->TxConfig |= HwZERO | Hw0 | Hw2 | Hw1;
            BITSET(pADMA->ChCtrl, Hw1 | Hw29);
        } else {
            unsigned long tmp = 0;
            if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
                alsa_dbg("%s() playback start\n", __func__);
                //tca_i2s_start(pDAI, pADMASPDIFTX, 0);
                BITSET(pADMA->ChCtrl, Hw0 | Hw28);
                tmp = pDAI->DAVC;
                pDAI->DAVC = 0x10;
                pDAI->DAMR |= Hw14;
                mdelay(10);
                pDAI->DAVC = tmp;

            } else {
                //tca_i2s_start(pDAI, pADMASPDIFTX, 1);
                BITSET(pADMA->ChCtrl, Hw2 | Hw30);
                tmp = pDAI->DAVC;
                pDAI->DAVC = 0x10;
                pDAI->DAMR |= Hw13;
                mdelay(10);
                pDAI->DAVC = tmp;
            }
        }
        break;

    case SNDRV_PCM_TRIGGER_STOP:
    case SNDRV_PCM_TRIGGER_SUSPEND:
    case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
        if (substream->pcm->device == __SPDIF_DEV_NUM__) {
            pADMASPDIFTX->TxConfig &= ~(Hw0 | Hw2 | Hw1);
            BITCLR(pADMA->ChCtrl, Hw1 | Hw29);
        } else {
            if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
                //tca_i2s_stop(pDAI, pADMASPDIFTX, 0);
                pDAI->DAMR &= ~Hw14;
                BITCLR(pADMA->ChCtrl, Hw0);
                tcc_pcm_delay(100);
                BITCLR(pADMA->ChCtrl, Hw28);
            } else {
                //tca_i2s_stop(pDAI, pADMASPDIFTX, 1);
                pDAI->DAMR &= ~Hw13;
                BITCLR(pADMA->ChCtrl, Hw2 | Hw30);
            }
        }
        break;

    default:
        ret = -EINVAL;
    }

    return ret;
}


//buffer point update
//current position   range 0-(buffer_size-1)
static snd_pcm_uframes_t tcc_pcm_pointer(struct snd_pcm_substream *substream)
{
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);
#else
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
#endif
    struct snd_pcm_runtime *runtime = substream->runtime;
    snd_pcm_uframes_t x;
    dma_addr_t ptr = 0;

    //alsa_dbg(" %s \n", __func__);

    if (substream->pcm->device == __SPDIF_DEV_NUM__) {
#if defined(CONFIG_ARCH_TCC892X)
        pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA1);
#endif
        ptr = pADMA->TxSpCsar;
    } else {
        ptr = (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) ? pADMA->TxDaCsar : pADMA->RxDaCdar;
    }

    x = bytes_to_frames(runtime, ptr - runtime->dma_addr);

    if (x < runtime->buffer_size) {
        return x;
    }
    return 0;
}

static int tcc_pcm_open(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct tcc_runtime_data *prtd;
    int ret;

    alsa_dbg("[%s] open %s device, %s\n", __func__, 
										substream->pcm->device == __SPDIF_DEV_NUM__ ? "spdif":"pcm", 
										substream->stream == SNDRV_PCM_STREAM_PLAYBACK ? "output" : "input");

    mutex_lock(&(tcc_alsa_info.mutex));
    if (substream->pcm->device == __SPDIF_DEV_NUM__) {
        if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
            tcc_alsa_info.flag |= TCC_RUNNING_SPDIF;
            tcc_alsa_info.spdif_ptr = substream;
            snd_soc_set_runtime_hwparams(substream, &tcc_pcm_hardware_play);
            alsa_dbg("[%s] open spdif device\n", __func__);
        } else {
            printk("cannot support S/PDIF capture !!!!!\n");
            return -1;
        }
    } else {
        if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {  
            tcc_alsa_info.flag |= TCC_RUNNING_PLAY;
            tcc_alsa_info.play_ptr = substream;
            snd_soc_set_runtime_hwparams(substream, &tcc_pcm_hardware_play);
        } else {
            tcc_alsa_info.flag |= TCC_RUNNING_CAPTURE;
            tcc_alsa_info.cap_ptr = substream;
            snd_soc_set_runtime_hwparams(substream, &tcc_pcm_hardware_capture);
        }
    }
    mutex_unlock(&(tcc_alsa_info.mutex));

    prtd = kzalloc(sizeof(struct tcc_runtime_data), GFP_KERNEL);

    if (prtd == NULL) {
        ret = -ENOMEM;
        goto out;
    }
    runtime->private_data = prtd;

    return 0;
    kfree(prtd);
out:
    return ret;
}

static int tcc_pcm_close(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct tcc_runtime_data *prtd = runtime->private_data;
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMADAI pDAI = (volatile PADMADAI)tcc_p2v(BASE_ADDR_DAI0);
    volatile PADMASPDIFTX pADMASPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX1);
#else
    volatile PADMADAI pDAI = (volatile PADMADAI)tcc_p2v(BASE_ADDR_DAI);
    volatile PADMASPDIFTX pADMASPDIFTX = (volatile PADMASPDIFTX)tcc_p2v(BASE_ADDR_SPDIFTX);
#endif

    alsa_dbg("[%s] close %s device, %s\n", __func__, substream->pcm->device == __SPDIF_DEV_NUM__ ? "spdif":"pcm", 
													substream->stream == SNDRV_PCM_STREAM_PLAYBACK ? "output" : "input");

    mutex_lock(&(tcc_alsa_info.mutex));
    if (substream->pcm->device == __SPDIF_DEV_NUM__) {
        tcc_alsa_info.flag &= ~TCC_RUNNING_SPDIF;
        //tca_i2s_stop(pDAI, pADMASPDIFTX, 0);
        pADMASPDIFTX->TxConfig &= ~Hw0;
        alsa_dbg("[%s] close spdif device\n", __func__);
    } else {
        if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
            tcc_alsa_info.flag &= ~TCC_RUNNING_PLAY;
            //tca_i2s_stop(pDAI, pADMASPDIFTX, 0);
            pDAI->DAMR &= ~Hw14;
        } else {
            tcc_alsa_info.flag &= ~TCC_RUNNING_CAPTURE;
            //tca_i2s_stop(pDAI, pADMASPDIFTX, 1);
            pDAI->DAMR &= ~Hw13;
        }
    }
    // dma_free_writecombine(substream->pcm->card->dev, PAGE_SIZE,); 

    if (prtd) {
        kfree(prtd);
    }

    if (tcc_alsa_info.flag & TCC_INTERRUPT_REQUESTED) {
        if (!(tcc_alsa_info.flag & (TCC_RUNNING_SPDIF | TCC_RUNNING_PLAY | TCC_RUNNING_CAPTURE))) {
            free_irq(alsa_get_intr_num(substream), &tcc_alsa_info);
            tcc_alsa_info.flag &= ~TCC_INTERRUPT_REQUESTED;
        }
    }
    mutex_unlock(&(tcc_alsa_info.mutex));

    return 0;
}

static int tcc_pcm_mmap(struct snd_pcm_substream *substream, struct vm_area_struct *vma)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    return dma_mmap_writecombine(substream->pcm->card->dev, vma,
        runtime->dma_area,
        runtime->dma_addr,
        runtime->dma_bytes);
}

struct snd_pcm_ops tcc_pcm_ops = {
    .open  = tcc_pcm_open,
    .close  = tcc_pcm_close,
    .ioctl  = snd_pcm_lib_ioctl,
    .hw_params = tcc_pcm_hw_params,
    .hw_free = tcc_pcm_hw_free,
    .prepare = tcc_pcm_prepare,
    .trigger = tcc_pcm_trigger,
    .pointer = tcc_pcm_pointer,
    .mmap = tcc_pcm_mmap,
};

static int tcc_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
    struct snd_pcm_substream *substream = pcm->streams[stream].substream;
    struct snd_dma_buffer *buf = &substream->dma_buffer;
    size_t size = 0;

    if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
        size = tcc_pcm_hardware_play.buffer_bytes_max;
    } else {
        size = tcc_pcm_hardware_capture.buffer_bytes_max;
    }

    buf->dev.type = SNDRV_DMA_TYPE_DEV;
    buf->dev.dev = pcm->card->dev;
    buf->private_data = NULL;

    alsa_dbg("tcc_pcm_preallocate_dma_buffer size [%d]\n", size);
    buf->area = dma_alloc_writecombine(pcm->card->dev, size, &buf->addr, GFP_KERNEL);
    if (!buf->area) {
        return -ENOMEM;
    }

    buf->bytes = size;

    return 0;
}


static void tcc_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
    struct snd_pcm_substream *substream;
    struct snd_dma_buffer *buf;
    int stream;

    alsa_dbg(" %s \n", __func__);

    for (stream = 0; stream < 2; stream++) {
        substream = pcm->streams[stream].substream;
        if (!substream) { continue; }

        buf = &substream->dma_buffer;
        if (!buf->area) { continue; }

        dma_free_writecombine(pcm->card->dev, buf->bytes, buf->area, buf->addr);
        buf->area = NULL;
    }
    mutex_init(&(tcc_alsa_info.mutex));
}

static u64 tcc_pcm_dmamask = DMA_32BIT_MASK;

int tcc_pcm_new(struct snd_card *card, struct snd_soc_dai *dai, struct snd_pcm *pcm)
{
    int ret = 0;

    alsa_dbg("[%s] \n", __func__);

    memset(&tcc_alsa_info, 0, sizeof(tcc_interrupt_info_x));
    mutex_init(&(tcc_alsa_info.mutex));

    if (!card->dev->dma_mask) {
        card->dev->dma_mask = &tcc_pcm_dmamask;
    }
    if (!card->dev->coherent_dma_mask) {
        card->dev->coherent_dma_mask = DMA_32BIT_MASK;
    }
    if (dai->driver->playback.channels_min) {
        ret = tcc_pcm_preallocate_dma_buffer(pcm, SNDRV_PCM_STREAM_PLAYBACK);
        if (ret) { goto out; }
    }

    if (dai->driver->capture.channels_min) {
        ret = tcc_pcm_preallocate_dma_buffer(pcm, SNDRV_PCM_STREAM_CAPTURE);
        if (ret) { goto out; }
    } 
	{
#if defined(CONFIG_ARCH_TCC892X)
        volatile ADMASPDIFTX *pADMASPDIFTX = (volatile ADMASPDIFTX *)tcc_p2v(BASE_ADDR_SPDIFTX1);
#else
        volatile ADMASPDIFTX *pADMASPDIFTX = (volatile ADMASPDIFTX *)tcc_p2v(BASE_ADDR_SPDIFTX);
#endif

        memset((void *)pADMASPDIFTX->UserData, 0, 24);
        memset((void *)pADMASPDIFTX->ChStatus, 0, 24);
        memset((void *)pADMASPDIFTX->TxBuffer, 0, 128);

        //unsigned int bitSet = 0x0;

        pADMASPDIFTX->TxConfig |= HwZERO
            //16bits 
            |Hw8 //Clock Divider Ratio is 2(384*fs to default)
            //| Hw7 | Hw6 //User Data Enalbe Bits is set reserved
            //| Hw5 | Hw4 //Channel Status Enalbe Bits is set reserved
            //| Hw2 //interrupt output enable
            | Hw1 //data being valid
            //| Hw0 //enable the transmission
            ;
        //Set SPDIF Transmit Channel Status
        pADMASPDIFTX->TxChStat |= HwZERO
            //| 0x00000400 // Store Channel Status bit
            //| 0x00000200 // Store User Data bit
            //| 0x00000100 // Store Validity bit
            //44.1kHz
            //| Hw3 //Original/Commercially Pre-recored data
            //| Hw2 //Pre-emphasis is 50/15us
            //| Hw1 //Copy permitted
            //Audio format
            ;		

        //pADMASPDIFTX->TxIntMask = HwZERO
        //	| Hw2//Higher Data buffer is empty
        //	| Hw1//Lower Data buffer is empty
        //	;

        pADMASPDIFTX->DMACFG = Hw7;
        msleep(100);

        pADMASPDIFTX->DMACFG = HwZERO
            //| Hw14	//Swap Sample Enable
            | Hw13 	//Read Address 16bit Mode
            | Hw11	//DMA Request enable for user data buffer
            | Hw10	//DMA Request enable for sample data buffer
            //| Hw8	//Enable address
            //| Hw3	//FIFO Threshold for DMA request
            | Hw1| Hw0	// [3:0] FIFO Threshold for DMA Request
            ;

        /* Initialize Sample Data Buffer */
        //while(pADMASPDIFTX->DMACFG & 0x00300000) pADMASPDIFTX->TxBuffer[0] = 0;
        pADMASPDIFTX->TxBuffer[0] = 0;

        pADMASPDIFTX->TxConfig |= HwZERO
            //| Hw2 //interrupt output enable
            | Hw1 //data being valid
            | Hw0 //enable the transmission
            ;

        pADMASPDIFTX->TxIntStat = 0x1E; /* Clear Interrupt Status */
    }

out:
    return ret;
}


#if defined(CONFIG_PM)
int tcc_pcm_suspend(struct snd_soc_dai *dai)
{
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);
#else
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
#endif

    alsa_dbg(" %s \n", __func__);
    gADMA.TransCtrl = pADMA->TransCtrl;
    gADMA.RptCtrl   = pADMA->RptCtrl;
    gADMA.ChCtrl    = pADMA->ChCtrl;
    gADMA.GIntReq   = pADMA->GIntReq;

#if defined(CONFIG_ARCH_TCC892X)
    pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA1);
    gADMA1.TransCtrl = pADMA->TransCtrl;
    gADMA1.RptCtrl   = pADMA->RptCtrl;
    gADMA1.ChCtrl    = pADMA->ChCtrl;
    gADMA1.GIntReq   = pADMA->GIntReq;
#endif
    return 0;
}

int tcc_pcm_resume(struct snd_soc_dai *dai)
{
#if defined(CONFIG_ARCH_TCC892X)
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA0);
#else
    volatile PADMA pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA);
#endif

    alsa_dbg(" %s \n", __func__);
    pADMA->TransCtrl = gADMA.TransCtrl;
    pADMA->RptCtrl   = gADMA.RptCtrl;
    pADMA->ChCtrl    = gADMA.ChCtrl;
    pADMA->GIntReq   = gADMA.GIntReq;

#if defined(CONFIG_ARCH_TCC892X)
    pADMA = (volatile PADMA)tcc_p2v(BASE_ADDR_ADMA1);
    pADMA->TransCtrl = gADMA1.TransCtrl;
    pADMA->RptCtrl   = gADMA1.RptCtrl;
    pADMA->ChCtrl    = gADMA1.ChCtrl;
    pADMA->GIntReq   = gADMA1.GIntReq;
#endif
    return 0;
}
#endif

static struct snd_soc_platform_driver tcc_soc_platform = {
    .ops      = &tcc_pcm_ops,
    .pcm_new  = tcc_pcm_new,
    .pcm_free = tcc_pcm_free_dma_buffers,
#if defined(CONFIG_PM)
    .suspend  = tcc_pcm_suspend,
    .resume   = tcc_pcm_resume,
#endif
};

static __devinit int tcc_pcm_probe(struct platform_device *pdev)
{
    alsa_dbg(" %s \n", __func__);
	return snd_soc_register_platform(&pdev->dev, &tcc_soc_platform);
}

static int __devexit tcc_pcm_remove(struct platform_device *pdev)
{
    alsa_dbg(" %s \n", __func__);
	snd_soc_unregister_platform(&pdev->dev);
	return 0;
}

static struct platform_driver tcc_pcm_driver = {
	.driver = {
			.name = "tcc-pcm-audio",
			.owner = THIS_MODULE,
	},

	.probe = tcc_pcm_probe,
	.remove = __devexit_p(tcc_pcm_remove),
};

static int __init snd_tcc_platform_init(void)
{
    alsa_dbg(" %s \n", __func__);
	return platform_driver_register(&tcc_pcm_driver);
}
module_init(snd_tcc_platform_init);

static void __exit snd_tcc_platform_exit(void)
{
    alsa_dbg(" %s \n", __func__);
	platform_driver_unregister(&tcc_pcm_driver);
}
module_exit(snd_tcc_platform_exit);





/************************************************************************
 * Dummy function for S/PDIF. This is a codec part.
************************************************************************/
static int tcc_dummy_probe(struct snd_soc_codec *codec) { return 0; }
static int tcc_dummy_remove(struct snd_soc_codec *codec) { return 0; }
static int tcc_dummy_set_bias_level(struct snd_soc_codec *codec, enum snd_soc_bias_level level) { return 0; }

static struct snd_soc_codec_driver soc_codec_dev_tcc_dummy = {
	.probe =	tcc_dummy_probe,
	.remove =	tcc_dummy_remove,
	.set_bias_level = tcc_dummy_set_bias_level,
	.reg_cache_size = 0,
	.reg_word_size = sizeof(u16),
	.reg_cache_default = NULL,
	.dapm_widgets = NULL,
	.num_dapm_widgets = 0,
	.dapm_routes = NULL,
	.num_dapm_routes = 0,
};

static int  iec958_pcm_prepare(struct snd_pcm_substream *substream, struct snd_soc_dai *cpu_dai) { return 0; }
static int  iec958_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params, struct snd_soc_dai *cpu_dai) { return 0; }
static void iec958_shutdown(struct snd_pcm_substream *substream, struct snd_soc_dai *cpu_dai) { }
static int  iec958_mute(struct snd_soc_dai *dai, int mute) { return 0; }
static int  iec958_set_dai_sysclk(struct snd_soc_dai *codec_dai, int clk_id, unsigned int freq, int dir) { return 0; }
static int  iec958_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt) { return 0; }

static struct snd_soc_dai_ops iec958_dai_ops = {
    .set_sysclk = iec958_set_dai_sysclk,
    .set_fmt    = iec958_set_dai_fmt,
    .digital_mute = iec958_mute,

    .prepare    = iec958_pcm_prepare,
    .hw_params  = iec958_hw_params,
    .shutdown   = iec958_shutdown,
};

static struct snd_soc_dai_driver iec958_dai = {
	.name = "IEC958",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
#if defined(CONFIG_SND_SOC_TCC_MULTICHANNEL)
		.channels_max = 8,
        .rates    = SNDRV_PCM_RATE_8000_192000,
        .rate_max = 192000,
#else
        .channels_max = 2,
        .rates    = SNDRV_PCM_RATE_8000_96000,
        .rate_max = 96000,
#endif
		.formats  = (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_U16_LE),},	// Planet 20120306
    .ops = &iec958_dai_ops,
};

static int tcc_iec958_probe(struct platform_device *pdev)
{
    int ret = 0;
    alsa_dbg(" %s \n", __func__);
    ret = snd_soc_register_codec(&pdev->dev, &soc_codec_dev_tcc_dummy, &iec958_dai, 1);
    printk("%s()   ret: %d\n", __func__, ret);
    return ret;

	//return snd_soc_register_dai(&pdev->dev, &iec958_dai);
}

static int tcc_iec958_remove(struct platform_device *pdev)
{
    alsa_dbg(" %s \n", __func__);
    snd_soc_unregister_codec(&pdev->dev);
    //snd_soc_unregister_dai(&pdev->dev);
	return 0;
}

static struct platform_driver tcc_iec958_driver = {
	.driver = {
			.name = "tcc-iec958",
			.owner = THIS_MODULE,
	},

	.probe = tcc_iec958_probe,
	.remove = __devexit_p(tcc_iec958_remove),
};

static int __init snd_tcc_iec958_platform_init(void)
{
    alsa_dbg(" %s \n", __func__);
	return platform_driver_register(&tcc_iec958_driver);
}
module_init(snd_tcc_iec958_platform_init);

static void __exit snd_tcc_iec958_platform_exit(void)
{
    alsa_dbg(" %s \n", __func__);
	platform_driver_unregister(&tcc_iec958_driver);
}
module_exit(snd_tcc_iec958_platform_exit);

/***********************************************************************/



MODULE_AUTHOR("Telechips");
MODULE_DESCRIPTION("Telechips TCC PCM DMA module");
MODULE_LICENSE("GPL");

