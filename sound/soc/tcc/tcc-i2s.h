/*
 * linux/sound/soc/tcc/tcc-i2s.h
 *
 * Based on:    linux/sound/soc/pxa/pxa2xx-i2s.h
 * Author:  <linux@telechips.com>
 * Created:    Nov 25, 2008
 * Description: ALSA PCM interface for the Intel PXA2xx chip
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 
 * as published by the Free Software Foundation
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

#ifndef _TCC_I2S_H
#define _TCC_I2S_H

/* pxa2xx DAI ID's */
#define TCC_DAI_I2S			0

/* I2S clock */
#define TCC_I2S_SYSCLK		0

//=============================================================================
#define AUDIO_DMA_PAGE_NUMBER   (2)                     // Number of DMA Buffer
#define AUDIO_DMA_OUT_PAGE_SIZE (1024)                  // Size in bytes        
#define AUDIO_DMA_IN_PAGE_SIZE  (1024)                  // Size in bytes        
#define AUDIO_DMA_OUT_BUF_SIZE  (AUDIO_DMA_PAGE_NUMBER * AUDIO_DMA_OUT_PAGE_SIZE)
#define AUDIO_DMA_IN_BUF_SIZE   (AUDIO_DMA_PAGE_NUMBER * AUDIO_DMA_IN_PAGE_SIZE)

//===================== Register Configuration Constants ======================

#define IIS_INTERNAL_CLOCK_ENABLE       (1<<17)                 // Enable CPU clock to IIS controller

//----- GPIO Configuration Masks -----
#define ENABLE_I2SSDO                   0x00000200
#define ENABLE_I2SSDI                   0x00000080
#define ENABLE_I2SCDCLK                 0x00000020
#define ENABLE_I2SSCLK                  0x00000008
#define ENABLE_I2SLRCLK                 0x00000002
#define DISABLE_I2S_PULLUPS             0x0000001F

//----- Register definitions for IISCON control register (global config register) -----
#define LR_CHANNEL_INDEX                0x00000100              // Left/right channel index         (read-only)     
#define TRANSMIT_FIFO_READY             0x00000080              // Indicates transmit FIFO is ready (read-only)
#define RECEIVE_FIFO_READY              0x00000040              // Indicates receive FIFO is ready  (read-only)
#define TRANSMIT_DMA_REQUEST_ENABLE     0x00000020              // Enables transmit DMA request
#define RECEIVE_DMA_REQUEST_ENABLE      0x00000010              // Enables receive DMA request
#define TRANSMIT_IDLE_CMD               0x00000008              // Pauses transmit
#define RECEIVE_IDLE_CMD                0x00000004              // Pauses receive
#define IIS_PRESCALER_ENABLE            0x00000002              // Enables clock prescaler
#define IIS_INTERFACE_ENABLE            0x00000001              // Enables IIS controller

//----- Register definitions for IISMOD status register (global status register) -----
#define IIS_MASTER_MODE                 0x00000000              // Selects master/slave mode
#define IIS_SLAVE_MODE                  0x00000100              
#define IIS_NOTRANSFER_MODE             0x00000000              // Selects transfer mode
#define IIS_RECEIVE_MODE                0x00000040
#define IIS_TRANSMIT_MODE               0x00000080
#define IIS_TRANSMIT_RECEIVE_MODE       0x000000C0
#define ACTIVE_CHANNEL_LEFT             0x00000000              // Selects active channel
#define ACTIVE_CHANNEL_RIGHT            0x00000020
#define SERIAL_INTERFACE_IIS_COMPAT     0x00000000              // Selects serial interface format
#define SERIAL_INTERFACE_MSBL_COMPAT    0x00000010
#define DATA_8_BITS_PER_CHANNEL         0x00000000              // Selects # of data bits per channel
#define DATA_16_BITS_PER_CHANNEL        0x00000008              
#define MASTER_CLOCK_FREQ_256fs         0x00000000              // Selects master clock frequency
#define MASTER_CLOCK_FREQ_384fs         0x00000004              
#define SERIAL_BIT_CLOCK_FREQ_16fs      0x00000000              // Selects serial data bit clock frequency
#define SERIAL_BIT_CLOCK_FREQ_32fs      0x00000001              
#define SERIAL_BIT_CLOCK_FREQ_48fs      0x00000002              


//----- Register definitions for IISPSR control register (global config register) -----
//      FORMAT:         bits[9:5] - Prescaler Control A
//                      bits[4:0] - Prescaler Control B
//
//                      Range: 0-31 and the division factor is N+1 (a.k.a. 1-32)
//
//      The I2SLRCLK frequency is determined as follows:
//
//              I2SLRCLK = CODECLK / I2SCDCLK       and     (prescaler+1) = PCLK / CODECLK
//
//      Thus, rearranging the equations a bit we can see that:
//
//              prescaler = (PCLK / CODECLK) - 1 
//      or
//              prescaler = ((PCLK / (IS2LRCLK * IS2CDCLK)) - 1
//      
// Here are some popular values for IS2LRCLK:
//      
#define IS2LRCLK_800                    800
#define IS2LRCLK_11025                  11025
#define IS2LRCLK_16000                  16000
#define IS2LRCLK_22050                  22050
#define IS2LRCLK_32000                  32000
#define IS2LRCLK_44100                  44100
#define IS2LRCLK_48000                  48000
#define IS2LRCLK_64000                  64000
#define IS2LRCLK_88200                  88200
#define IS2LRCLK_96000                  96000
    

//----- Register definitions for IISFCON control register (global config register) -----
#define TRANSMIT_FIFO_ACCESS_NORMAL     0x00000000              // Selects the transmit FIFO access mode
#define TRANSMIT_FIFO_ACCESS_DMA        0x00008000              
#define RECEIVE_FIFO_ACCESS_NORMAL      0x00000000              // Selects the receive FIFO access mode
#define RECEIVE_FIFO_ACCESS_DMA         0x00004000              
#define TRANSMIT_FIFO_ENABLE            0x00002000              // Enables transmit FIFO
#define RECEIVE_FIFO_ENABLE             0x00001000              // Enables receive FIFO
//
// READ-ONLY Fields:    bits[11:6] - Transmit FIFO datacount
//                      bits[5:0]  - Receive FIFO datacount

//----- Register definitions for IISFIFO control register (global config register) -----
//      NOTE: This register is used to access the transmit/receive FIFO
#define MAX_TRANSMIT_FIFO_ENTRIES       24
#define MAX_RECEIVE_FIFO_ENTRIES        24

#define __I2S_DEV_NUM__   0
#define __SPDIF_DEV_NUM__ 1

#endif
