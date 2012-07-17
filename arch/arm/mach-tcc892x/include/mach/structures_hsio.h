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
	Not finished block "GMAC"
*/

#ifndef _STRUCTURES_HSIO_H_
#define _STRUCTURES_HSIO_H_


/*******************************************************************************
*
*    TCC892x DataSheet PART 6 HSIO BUS
*
********************************************************************************/

/************************************************************************
*   3. Ethernet MAC (GMAC)                       (Base Addr = 0x71701000)
*************************************************************************/

typedef struct _GMACDMA{
    volatile TCC_DEF32BIT_TYPE  BMODE;          // 0x000  R/W  0x00020101   Bus mode register
    volatile TCC_DEF32BIT_TYPE  TPD;            // 0x004  R/W  0x00000000   Transmit poll demand register
    volatile TCC_DEF32BIT_TYPE  RPD;            // 0x008  R/W  0x00000000   Receive poll demand register
    volatile TCC_DEF32BIT_TYPE  RDLA;           // 0x00C  R/W  0x00000000   Receive descriptor list address register
    volatile TCC_DEF32BIT_TYPE  TDLA;           // 0x010  R/W  0x00000000   Transmit descriptor list address reigster
    volatile TCC_DEF32BIT_TYPE  STS;            // 0x014  R    -            Status register
    volatile TCC_DEF32BIT_TYPE  OPMODE;         // 0x018  R/W  0x00000000   Operation mode register
    volatile TCC_DEF32BIT_TYPE  IE;             // 0x01C  R/W  0x00000000   Interrupt enable register
    volatile TCC_DEF32BIT_TYPE  MFBOC;          // 0x020  R/C  0x00000000   Missed frame and buffer overflow counter register
    unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  CHTD;           // 0x048  R    -            Current host transmit descriptor register
    volatile TCC_DEF32BIT_TYPE  CHRD;           // 0x04C  R    -            Current host receive descriptor register
    volatile TCC_DEF32BIT_TYPE  CHTBA;          // 0x050  R    -            Current host transmit buffer address register
    volatile TCC_DEF32BIT_TYPE  CHRBA;          // 0x054  R    -            Current host receive buffer address register
} GMACDMA, *PGMACDMA;

typedef struct _GMAC{
    volatile TCC_DEF32BIT_TYPE  MACC;           // 0x000  R/W  0x00000000   MAC configuration register
    volatile TCC_DEF32BIT_TYPE  MACFF;          // 0x004  R/W  0x00000000   MAC frame filter
    volatile TCC_DEF32BIT_TYPE  HTH;            // 0x008  R/W  0x00000000   Hash table high register
    volatile TCC_DEF32BIT_TYPE  HTL;            // 0x00C  R/W  0x00000000   Hash table low register
    volatile TCC_DEF32BIT_TYPE  GMIIA;          // 0x010  R/W  0x00000000   GMII address register
    volatile TCC_DEF32BIT_TYPE  GMIID;          // 0x014  R/W  0x00000000   GMII data register
    volatile TCC_DEF32BIT_TYPE  FC;             // 0x018  R/W  0x00000000   Flow control register
    volatile TCC_DEF32BIT_TYPE  VLANT;          // 0x01C  R/W  0x00000000   VLAN tag register
    volatile TCC_DEF32BIT_TYPE  VERSION;        // 0x020  R/W  0x0000--34   Version register
    unsigned :32;
    volatile TCC_DEF32BIT_TYPE  RWFF;           // 0x028  R/W  0x00000000   Remote wake-up frame register
    volatile TCC_DEF32BIT_TYPE  PMTCS;          // 0x02C  R/W  0x00000000   PMT control and status
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  IRQS;           // 0x038  R    -            Interrupt status register
    volatile TCC_DEF32BIT_TYPE  IRQM;           // 0x03C  R/W  0x00000000   Interrupt mask register
    volatile TCC_DEF32BIT_TYPE  MACA0H;         // 0x040  R/W  0x0000FFFF   MAC address0 high register
    volatile TCC_DEF32BIT_TYPE  MACA0L;         // 0x044  R/W  0xFFFFFFFF   MAC address0 low register
    volatile TCC_DEF32BIT_TYPE  MACA1H;         // 0x048  R/W  0x0000FFFF   MAC address1 high register
    volatile TCC_DEF32BIT_TYPE  MACA1L;         // 0x04C  R/W  0xFFFFFFFF   MAC address1 low register
    volatile TCC_DEF32BIT_TYPE  MACA2H;         // 0x050  R/W  0x0000FFFF   MAC address2 high register
    volatile TCC_DEF32BIT_TYPE  MACA2L;         // 0x054  R/W  0xFFFFFFFF   MAC address2 low register
    volatile TCC_DEF32BIT_TYPE  MACA3H;         // 0x058  R/W  0x0000FFFF   MAC address3 high register
    volatile TCC_DEF32BIT_TYPE  MACA3L;         // 0x05C  R/W  0xFFFFFFFF   MAC address3 low register
    volatile TCC_DEF32BIT_TYPE  MACA4H;         // 0x060  R/W  0x0000FFFF   MAC address4 high register
    volatile TCC_DEF32BIT_TYPE  MACA4L;         // 0x064  R/W  0xFFFFFFFF   MAC address4 low register
    volatile TCC_DEF32BIT_TYPE  MACA5H;         // 0x068  R/W  0x0000FFFF   MAC address5 high register
    volatile TCC_DEF32BIT_TYPE  MACA5L;         // 0x06C  R/W  0xFFFFFFFF   MAC address5 low register
    volatile TCC_DEF32BIT_TYPE  MACA6H;         // 0x070  R/W  0x0000FFFF   MAC address6 high register
    volatile TCC_DEF32BIT_TYPE  MACA6L;         // 0x074  R/W  0xFFFFFFFF   MAC address6 low register
    volatile TCC_DEF32BIT_TYPE  MACA7H;         // 0x078  R/W  0x0000FFFF   MAC address7 high register
    volatile TCC_DEF32BIT_TYPE  MACA7L;         // 0x07C  R/W  0xFFFFFFFF   MAC address7 low register
    volatile TCC_DEF32BIT_TYPE  MACA8H;         // 0x080  R/W  0x0000FFFF   MAC address8 high register
    volatile TCC_DEF32BIT_TYPE  MACA8L;         // 0x084  R/W  0xFFFFFFFF   MAC address8 low register
    volatile TCC_DEF32BIT_TYPE  MACA9H;         // 0x088  R/W  0x0000FFFF   MAC address9 high register
    volatile TCC_DEF32BIT_TYPE  MACA9L;         // 0x08C  R/W  0xFFFFFFFF   MAC address9 low register
    volatile TCC_DEF32BIT_TYPE  MACA10H;        // 0x090  R/W  0x0000FFFF   MAC address10 high register
    volatile TCC_DEF32BIT_TYPE  MACA10L;        // 0x094  R/W  0xFFFFFFFF   MAC address10 low register
    volatile TCC_DEF32BIT_TYPE  MACA11H;        // 0x098  R/W  0x0000FFFF   MAC address11 high register
    volatile TCC_DEF32BIT_TYPE  MACA11L;        // 0x09C  R/W  0xFFFFFFFF   MAC address11 low register
    volatile TCC_DEF32BIT_TYPE  MACA12H;        // 0x0A0  R/W  0x0000FFFF   MAC address12 high register
    volatile TCC_DEF32BIT_TYPE  MACA12L;        // 0x0A4  R/W  0xFFFFFFFF   MAC address12 low register
    volatile TCC_DEF32BIT_TYPE  MACA13H;        // 0x0A8  R/W  0x0000FFFF   MAC address13 high register
    volatile TCC_DEF32BIT_TYPE  MACA13L;        // 0x0AC  R/W  0xFFFFFFFF   MAC address13 low register
    volatile TCC_DEF32BIT_TYPE  MACA14H;        // 0x0B0  R/W  0x0000FFFF   MAC address14 high register
    volatile TCC_DEF32BIT_TYPE  MACA14L;        // 0x0B4  R/W  0xFFFFFFFF   MAC address14 low register
    volatile TCC_DEF32BIT_TYPE  MACA15H;        // 0x0B8  R/W  0x0000FFFF   MAC address15 high register
    volatile TCC_DEF32BIT_TYPE  MACA15L;        // 0x0BC  R/W  0xFFFFFFFF   MAC address15 low register
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  RGMIIS;         // 0x0D8  R    -            RGMII status register
    unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  MMC_CNTRL;              // 0x100  R/W  0x00000000   MMC control establishes the operating mode of MMC
    volatile TCC_DEF32BIT_TYPE  MMC_INTR_RX;            // 0x104  R/C  -            MMC receive interrupt maintains the interrupt generated from all of the receive statistic counters.
    volatile TCC_DEF32BIT_TYPE  MMC_INTR_TX;            // 0x108  R/C  -            MMC transmit interrupt maintains the interrupt generated form all of the transmit statistic counters.
    volatile TCC_DEF32BIT_TYPE  MMC_INTR_MASK_RX;       // 0x10C  R/W  0x00000000   MMC receive interrupt mask maintains the mask for the interrupt generated form all of the receive statistic counters
    volatile TCC_DEF32BIT_TYPE  MMC_INTR_MASK_TX;       // 0x110  R/W  0x00000000   MMC transmit interrupt mask maintains the mask for the interrupt generated form all of the transmit statistic counters
    volatile TCC_DEF32BIT_TYPE  TXOCTETCOUNT_GB;        // 0x114  R    -            Number of bytes transmitted, exclusive of preamble and retried bytes, in good and bad frames.
    volatile TCC_DEF32BIT_TYPE  TXFRAMECOUNT_GB;        // 0x118  R    -            Number of good and bad frames transmitted, exclusive of retried frames.
    volatile TCC_DEF32BIT_TYPE  TXBRADCASTFRAMES_G;     // 0x11C  R    -            Number of good broadcast frames transmitted.
    volatile TCC_DEF32BIT_TYPE  TXMULTICASTFRAMES_G;    // 0x120  R    -            Number of good multicast frames transmitted.
    volatile TCC_DEF32BIT_TYPE  TX64OCTETS_GB;          // 0x124  R    -            Number of good and bad frames transmitted with length 64 bytes, exclusive of preamble and retried frames.
    volatile TCC_DEF32BIT_TYPE  TX65TO127OCTETS_GB;     // 0x128  R    -            Number of good and bad frames transmitted with length between 65 and 127 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile TCC_DEF32BIT_TYPE  TX128TO255OCTETS_GB;    // 0x12C  R    -            Number of good and bad frames transmitted with length between 128 and 255 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile TCC_DEF32BIT_TYPE  TX256TO511OCTETS_GB;    // 0x130  R    -            Number of good and bad frames transmitted with length between 256 and 511 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile TCC_DEF32BIT_TYPE  TX512TO1023OCTETS_GB;   // 0x134  R    -            Number of good and bad frames transmitted with length between 512 and 1,023 (inclusive) bytes, exclusive of preamble and retried frames.
    volatile TCC_DEF32BIT_TYPE  TX1024TOMAXOCTETS_GB;   // 0x138  R    -            Number of good and bad frames transmitted with length between 1,024 and maxsize (inclusive) bytes, exclusive of preamble and retried frames.
    volatile TCC_DEF32BIT_TYPE  TXUNICASTFRAMES_GB;     // 0x13C  R    -            Number of good and bad unicast frames transmitted.
    volatile TCC_DEF32BIT_TYPE  TXMULTICASTFRAMES_GB;   // 0x140  R    -            Number of good and bad multicast frames transmitted.
    volatile TCC_DEF32BIT_TYPE  TXBRADCASTFRAMES_GB;    // 0x144  R    -            Number of good and bad broadcast frames transmitted.
    volatile TCC_DEF32BIT_TYPE  TXUNDERFLOWERRR;        // 0x148  R    -            Number of frames aborted due to frame underflow error.
    volatile TCC_DEF32BIT_TYPE  TXSINGLECOL_G;          // 0x14C  R    -            Number of successfully transmitted frames after a single collision in Half-duplex mode.
    volatile TCC_DEF32BIT_TYPE  TXMULTICOL_G;           // 0x150  R    -            Number of successfully transmitted frames after more than a single collision in Half-duplex mode.
    volatile TCC_DEF32BIT_TYPE  TXDEFERRED;             // 0x154  R    -            Number of successfully transmitted frames after a deferral in Half-duplex mode.
    volatile TCC_DEF32BIT_TYPE  TXLATECOL;              // 0x158  R    -            Number of frames aborted due to late collision error.
    volatile TCC_DEF32BIT_TYPE  TXEXESSCOL;             // 0x15C  R    -            Number of frames aborted due to excessive (16) collision errors.
    volatile TCC_DEF32BIT_TYPE  TXCARRIERERRR;          // 0x160  R    -            Number of frames aborted due to carrier sense error (no carrier or loss of carrier).
    volatile TCC_DEF32BIT_TYPE  TXOCTETCOUNT_G;         // 0x164  R    -            Number of bytes transmitted, exclusive of preamble, in good frames only.
    volatile TCC_DEF32BIT_TYPE  TXFRAMECOUNT_G;         // 0x168  R    -            Number of good frames transmitted.
    volatile TCC_DEF32BIT_TYPE  TXEXCESSDEF;            // 0x16C  R    -            Number of frames aborted due to excessive deferral error (deferred for more  than two max-sized frame times).
    volatile TCC_DEF32BIT_TYPE  TXPAUSEFRAMES;          // 0x170  R    -            Number of good PAUSE frames transmitted.
    volatile TCC_DEF32BIT_TYPE  TXVLANFRAMES_G;         // 0x174  R    -            Number of good VLAN frames transmitted, exclusive of retried frames.
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  RXFRAMECOUNT_GB;        // 0x180  R    -            Number of good and bad frames received.
    volatile TCC_DEF32BIT_TYPE  RXOCTETCOUNT_GB;        // 0x184  R    -            Number of bytes received, exclusive of preamble, in good and bad frames.
    volatile TCC_DEF32BIT_TYPE  RXOCTETCOUNT_G;         // 0x188  R    -            Number of bytes received, exclusive of preamble, only in good frames.
    volatile TCC_DEF32BIT_TYPE  RXBRADCASTFRAMES_G;     // 0x18C  R    -            Number of good broadcast frames received.
    volatile TCC_DEF32BIT_TYPE  RXMULTICASTFRAMES_G;    // 0x190  R    -            Number of good multicast frames received.
    volatile TCC_DEF32BIT_TYPE  RXCRCERRR;              // 0x194  R    -            Number of frames received with CRC error.
    volatile TCC_DEF32BIT_TYPE  RXALIGNMENTERRR;        // 0x198  R    -            Number of frames received with alignment (dribble) error. Valid only in 10/100 mode.
    volatile TCC_DEF32BIT_TYPE  RXRUNTERRR;             // 0x19C  R    -            Number of frames received with runt (<64 bytes and CRC error) error.
    volatile TCC_DEF32BIT_TYPE  RXJABBERERRR;           // 0x1A0  R    -            Number of giant frames received with length (including CRC) greater than 1,518 bytes (1,522 bytes for VLAN tagged) and with CRC error. If Jumbo Frame mode is enabled, then frames of length greater than 9,018 bytes (9,022 for VLAN tagged) are considered as giant frames.
    volatile TCC_DEF32BIT_TYPE  RXUNDERSIZE_G;          // 0x1A4  R    -            Number of frames received with length less than 64 bytes, without any errors.
    volatile TCC_DEF32BIT_TYPE  RXOVERSIZE_G;           // 0x1A8  R    -            Number of frames received with length greater than the maxsize (1,518 or 1,522 for VLAN tagged frames), without errors.
    volatile TCC_DEF32BIT_TYPE  RX64OCTETS_GB;          // 0x1AC  R    -            Number of good and bad frames received with length 64 bytes, exclusive of preamble.
    volatile TCC_DEF32BIT_TYPE  RX65TO127OCTETS_GB;     // 0x1B0  R    -            Number of good and bad frames received with length between 65 and 127 (inclusive) bytes, exclusive of preamble.
    volatile TCC_DEF32BIT_TYPE  RX128TO255OCTETS_GB;    // 0x1B4  R    -            Number of good and bad frames received with length between 128 and 255 (inclusive) bytes, exclusive of preamble.
    volatile TCC_DEF32BIT_TYPE  RX256TO511OCTETS_GB;    // 0x1B8  R    -            Number of good and bad frames received with length between 256 and 511 (inclusive) bytes, exclusive of preamble.
    volatile TCC_DEF32BIT_TYPE  RX512TO1023OCTETS_GB;   // 0x1BC  R    -            Number of good and bad frames received with length between 512 and 1,023 (inclusive) bytes, exclusive of preamble.
    volatile TCC_DEF32BIT_TYPE  RX1024TOMAXOCTETS_GB;   // 0x1C0  R    -            Number of good and bad frames received with length between 1,024 and maxsize (inclusive) bytes, exclusive of preamble and retried frames.
    volatile TCC_DEF32BIT_TYPE  RXUNICASTFRAMES_G;      // 0x1C4  R    -            Number of good unicast frames received.
    volatile TCC_DEF32BIT_TYPE  RXLENGTHERRR;           // 0x1C8  R    -            Number of frames received with length error (Length type field ¡Á frame size), for all frames with valid length field.
    volatile TCC_DEF32BIT_TYPE  RXOUTOFRANGETYPE;       // 0x1CC  R    -            Number of frames received with length field not equal to the valid frame size (greater than 1,500 but less than 1,536).
    volatile TCC_DEF32BIT_TYPE  RXPAUSEFRAMES;          // 0x1D0  R    -            Number of missed received frames due to FIFO overflow. This counter is not present in the GMAC-CORE configuration.
    volatile TCC_DEF32BIT_TYPE  rxfifooverflow;         // 0x1D4  R    -            Number of missed received frames due to FIFO overflow. This counter is not present in the GMAC-CORE configuration.
    volatile TCC_DEF32BIT_TYPE  rxvlanframes_gb;        // 0x1D8  R    -            Number of good and bad VLAN frames received.
    volatile TCC_DEF32BIT_TYPE  rxwatchdogerror;        // 0x1DC  R    -            Number of frames received with error due to watchdog timeout error (frames with a data load larger than 2,048 bytes).
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  mmc_ipc_intr_mask_rx;   // 0x200  R/W 0x00000000    MMC IPC Receive Checksum Offload Interrupt Mask maintains the mask for the interrupt generated from the receive IPC statistic counters.
    unsigned :32;
    volatile TCC_DEF32BIT_TYPE  mmc_ipc_intr_rx;        // 0x208  R/C 0x00000000    MMC Receive Checksum Offload Interrupt maintains the interrupt that the receive IPC statistic counters generate.
    unsigned :32;
    volatile TCC_DEF32BIT_TYPE  rxipv4_gd_frms;         // 0x210  R    -            Number of good IPv4 datagrams received with the TCP, UDP, or ICMP payload
    volatile TCC_DEF32BIT_TYPE  rxipv4_hdrerr_frms;     // 0x214  R    -            Number of IPv4 datagrams received with header (checksum, length, or version mismatch) errors
    volatile TCC_DEF32BIT_TYPE  rxipv4_nopay_frms;      // 0x218  R    -            Number of IPv4 datagram frames received that did not have a TCP, UDP, or ICMP payload processed by the Checksum engine
    volatile TCC_DEF32BIT_TYPE  rxipv4_frag_frms;       // 0x21C  R    -            Number of good IPv4 datagrams with fragmentation
    volatile TCC_DEF32BIT_TYPE  rxipv4_udsbl_frms;      // 0x220  R    -            Number of good IPv4 datagrams received that had a UDP payload with checksum disabled
    volatile TCC_DEF32BIT_TYPE  rxipv6_gd_frms;         // 0x224  R    -            Number of good IPv6 datagrams received with TCP, UDP, or ICMP payloads
    volatile TCC_DEF32BIT_TYPE  rxipv6_hdrerr_frms;     // 0x228  R    -            Number of IPv6 datagrams received with header errors (length or version mismatch)
    volatile TCC_DEF32BIT_TYPE  rxipv6_nopay_frms;      // 0x22C  R    -            Number of IPv6 datagram frames received that did not have a TCP, UDP, or ICMP payload. This includes all IPv6 datagrams with fragmentation or security extension headers
    volatile TCC_DEF32BIT_TYPE  rxudp_gd_frms;          // 0x230  R    -            Number of good IP datagrams with a good UDP payload. This counter is not updated when the rxipv4_udsbl_frms counter is incremented.
    volatile TCC_DEF32BIT_TYPE  rxudp_err_frms;         // 0x234  R    -            Number of good IP datagrams whose UDP payload has a checksum error
    volatile TCC_DEF32BIT_TYPE  rxtcp_gd_frms;          // 0x238  R    -            Number of good IP datagrams with a good TCP payload
    volatile TCC_DEF32BIT_TYPE  rxtcp_err_frms;         // 0x23C  R    -            Number of good IP datagrams whose TCP payload has a checksum error
    volatile TCC_DEF32BIT_TYPE  rxicmp_gd_frms;         // 0x240  R    -            Number of good IP datagrams with a good ICMP payload
    volatile TCC_DEF32BIT_TYPE  rxicmp_err_frms;        // 0x244  R    -            Number of good IP datagrams whose ICMP payload has a checksum error
    unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  rxipv4_gd_octets;       // 0x250  R    -            Number of bytes received in good IPv4 datagrams encapsulating TCP, UDP, or ICMP data. (Ethernet header, FCS, pad, or IP pad bytes are not included in this counter or in the octet counters listed below).
    volatile TCC_DEF32BIT_TYPE  rxipv4_hdrerr_octets;   // 0x254  R    -            Number of bytes received in IPv4 datagrams with header errors (checksum, length, version mismatch). The value in the Length field of IPv4 header is used to update this counter.
    volatile TCC_DEF32BIT_TYPE  rxipv4_nopay_octets;    // 0x258  R    -            Number of bytes received in IPv4 datagrams that did not have a TCP, UDP, or ICMP payload. The value in the IPv4 header¡¯s Length field is used to update this counter.
    volatile TCC_DEF32BIT_TYPE  rxipv4_frag_octets;     // 0x25C  R    -            Number of bytes received in fragmented IPv4 datagrams. The value in the IPv4 header¡¯s Length field is used to update this counter.
    volatile TCC_DEF32BIT_TYPE  rxipv4_udsbl_octets;    // 0x260  R    -            Number of bytes received in a UDP segment that had the UDP checksum disabled. This counter does not count IP Header bytes.
    volatile TCC_DEF32BIT_TYPE  rxipv6_gd_octets;       // 0x264  R    -            Number of bytes received in good IPv6 datagrams encapsulating TCP, UDP or ICMPv6 data
    volatile TCC_DEF32BIT_TYPE  rxipv6_hdrerr_octets;   // 0x268  R    -            Number of bytes received in IPv6 datagrams with header errors (length, version mismatch). The value in the IPv6 header¡¯s Length field is used to update this counter.
    volatile TCC_DEF32BIT_TYPE  rxipv6_nopay_octets;    // 0x26C  R    -            Number of bytes received in IPv6 datagrams that did not have a TCP, UDP, or ICMP payload. The value in the IPv6 header¡¯s Length field is used to update this counter.
    volatile TCC_DEF32BIT_TYPE  rxudp_gd_octets;        // 0x270  R    -            Number of bytes received in a good UDP segment. This counter (and the counters below) does not count IP header bytes.
    volatile TCC_DEF32BIT_TYPE  rxudp_err_octets;       // 0x274  R    -            Number of bytes received in a UDP segment that had checksum errors
    volatile TCC_DEF32BIT_TYPE  rxtcp_gd_octets;        // 0x278  R    -            Number of bytes received in a good TCP segment
    volatile TCC_DEF32BIT_TYPE  rxtcp_err_octets;       // 0x27C  R    -            Number of bytes received in a TCP segment with checksum errors
    volatile TCC_DEF32BIT_TYPE  rxicmp_gd_octets;       // 0x280  R    -            Number of bytes received in a good ICMP segment
    volatile TCC_DEF32BIT_TYPE  rxicmp_err_octets;      // 0x284  R    -            Number of bytes received in an ICMP segment with checksum errors
    TCC_DEF32BIT_TYPE           NOTDEF0[286];
    volatile TCC_DEF32BIT_TYPE  TSC;            // 0x700  R/W  0x00000000   Time stamp control register
    volatile TCC_DEF32BIT_TYPE  SSI;            // 0x704  R/W  0x00000000   Sub-second increment register
    volatile TCC_DEF32BIT_TYPE  TSH;            // 0x708  R    -            Time stamp high register
    volatile TCC_DEF32BIT_TYPE  TSL;            // 0x70C  R    -            Time stamp low register
    volatile TCC_DEF32BIT_TYPE  TSHU;           // 0x710  R/W  0x00000000   Time stamp high update register
    volatile TCC_DEF32BIT_TYPE  TSLU;           // 0x714  R/W  0x00000000   Time stamp low update register
    volatile TCC_DEF32BIT_TYPE  TSA;            // 0x718  R/W  0x00000000   Time stamp addend register
    volatile TCC_DEF32BIT_TYPE  TTH;            // 0x71C  R/W  0x00000000   Target time high register
    volatile TCC_DEF32BIT_TYPE  TTL;            // 0x720  R/W  0x00000000   Target time low register
    volatile TCC_DEF32BIT_TYPE  NOTDEF1[55];
    volatile TCC_DEF32BIT_TYPE  MACA16H;        // 0x800  R/W  0x0000FFFF   MAC address16 high register
    volatile TCC_DEF32BIT_TYPE  MACA16L;        // 0x804  R/W  0xFFFFFFFF   MAC address16 low register
    volatile TCC_DEF32BIT_TYPE  MACA17H;        // 0x808  R/W  0x0000FFFF   MAC address17 high register
    volatile TCC_DEF32BIT_TYPE  MACA17L;        // 0x80C  R/W  0xFFFFFFFF   MAC address17 low register
    volatile TCC_DEF32BIT_TYPE  MACA18H;        // 0x810  R/W  0x0000FFFF   MAC address18 high register
    volatile TCC_DEF32BIT_TYPE  MACA18L;        // 0x814  R/W  0xFFFFFFFF   MAC address18 low register
    volatile TCC_DEF32BIT_TYPE  MACA19H;        // 0x818  R/W  0x0000FFFF   MAC address19 high register
    volatile TCC_DEF32BIT_TYPE  MACA19L;        // 0x81C  R/W  0xFFFFFFFF   MAC address19 low register
    volatile TCC_DEF32BIT_TYPE  MACA20H;        // 0x820  R/W  0x0000FFFF   MAC address20 high register
    volatile TCC_DEF32BIT_TYPE  MACA20L;        // 0x824  R/W  0xFFFFFFFF   MAC address20 low register
    volatile TCC_DEF32BIT_TYPE  MACA21H;        // 0x828  R/W  0x0000FFFF   MAC address21 high register
    volatile TCC_DEF32BIT_TYPE  MACA21L;        // 0x82C  R/W  0xFFFFFFFF   MAC address21 low register
    volatile TCC_DEF32BIT_TYPE  MACA22H;        // 0x830  R/W  0x0000FFFF   MAC address22 high register
    volatile TCC_DEF32BIT_TYPE  MACA22L;        // 0x834  R/W  0xFFFFFFFF   MAC address22 low register
    volatile TCC_DEF32BIT_TYPE  MACA23H;        // 0x838  R/W  0x0000FFFF   MAC address23 high register
    volatile TCC_DEF32BIT_TYPE  MACA23L;        // 0x83C  R/W  0xFFFFFFFF   MAC address23 low register
    volatile TCC_DEF32BIT_TYPE  MACA24H;        // 0x840  R/W  0x0000FFFF   MAC address24 high register
    volatile TCC_DEF32BIT_TYPE  MACA24L;        // 0x844  R/W  0xFFFFFFFF   MAC address24 low register
    volatile TCC_DEF32BIT_TYPE  MACA25H;        // 0x848  R/W  0x0000FFFF   MAC address25 high register
    volatile TCC_DEF32BIT_TYPE  MACA25L;        // 0x84C  R/W  0xFFFFFFFF   MAC address25 low register
    volatile TCC_DEF32BIT_TYPE  MACA26H;        // 0x850  R/W  0x0000FFFF   MAC address26 high register
    volatile TCC_DEF32BIT_TYPE  MACA26L;        // 0x854  R/W  0xFFFFFFFF   MAC address26 low register
    volatile TCC_DEF32BIT_TYPE  MACA27H;        // 0x858  R/W  0x0000FFFF   MAC address27 high register
    volatile TCC_DEF32BIT_TYPE  MACA27L;        // 0x85C  R/W  0xFFFFFFFF   MAC address27 low register
    volatile TCC_DEF32BIT_TYPE  MACA28H;        // 0x860  R/W  0x0000FFFF   MAC address28 high register
    volatile TCC_DEF32BIT_TYPE  MACA28L;        // 0x864  R/W  0xFFFFFFFF   MAC address28 low register
    volatile TCC_DEF32BIT_TYPE  MACA29H;        // 0x868  R/W  0x0000FFFF   MAC address29 high register
    volatile TCC_DEF32BIT_TYPE  MACA29L;        // 0x86C  R/W  0xFFFFFFFF   MAC address29 low register
    volatile TCC_DEF32BIT_TYPE  MACA30H;        // 0x870  R/W  0x0000FFFF   MAC address30 high register
    volatile TCC_DEF32BIT_TYPE  MACA30L;        // 0x874  R/W  0xFFFFFFFF   MAC address30 low register
    volatile TCC_DEF32BIT_TYPE  MACA31H;        // 0x878  R/W  0x0000FFFF   MAC address31 high register
    volatile TCC_DEF32BIT_TYPE  MACA31L;        // 0x87C  R/W  0xFFFFFFFF   MAC address31 low register
} GMAC, *PGMAC;


/************************************************************************
*   4. USB 2.0 Host                  (Base Addr = 0x71200000, 0x71300000)
*************************************************************************/

typedef struct _USB_EHCI {
    volatile TCC_DEF32BIT_TYPE  HCCAPBASE;          // 0x000  R    0x01000010   // Capability Register
    volatile TCC_DEF32BIT_TYPE  HCSPARAMS;          // 0x004  R    0x00001116
    volatile TCC_DEF32BIT_TYPE  HCCPARAMS;          // 0x008  R/W  0x0000A010
    volatile TCC_DEF32BIT_TYPE  USBCMD;             // 0x010  R/W  0x00080000 or 0x00080B00 // Operational Registers
    volatile TCC_DEF32BIT_TYPE  USBSTS;             // 0x014  R/W  0x00001000
    volatile TCC_DEF32BIT_TYPE  USBINTR;            // 0x018  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  FRINDEX;            // 0x01C  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  CTRLDSSEGMENT;      // 0x020  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  PERIODICLISTBASE;   // 0x024  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  ASYNCLISTADDR;      // 0x028  R/W  0x00000000
    unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile TCC_DEF32BIT_TYPE  CONFIGFLAG;         // 0x050  R/W  0x00000000   // Auxiliary Power Well Registers
    volatile TCC_DEF32BIT_TYPE  PORTSC[15];         // 0x054~ R/W  0x00002000
    volatile TCC_DEF32BIT_TYPE  INSNREG[6];         // 0x090~ R/W  0x00000000, 0x00100010, 0x00000100,  // Synopsys-Specific Registers
} USB_EHCI, *PUSB_EHCI;

typedef struct _USB_OHCI {
    volatile TCC_DEF32BIT_TYPE  HcRevision;         // 0x000  R    0x00000010   // Control and status registers
    volatile TCC_DEF32BIT_TYPE  HcControl;          // 0x004  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcCommandStatus;    // 0x008  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcInterruptStatus;  // 0x00C  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcInterruptEnable;  // 0x010  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcInterruptDisable; // 0x014  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcHCCA;             // 0x018  R/W  0x00000000   // Memory pointer registers
    volatile TCC_DEF32BIT_TYPE  HcPeriodCurrentED;  // 0x01C  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcControlHeadED;    // 0x020  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcControlCurrentED; // 0x024  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcBulkHeadED;       // 0x028  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcBulkCurrentED;    // 0x02C  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcDoneHead;         // 0x030  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcRminterval;       // 0x034  R/W  0x00002EDF   // Frame counter registers
    volatile TCC_DEF32BIT_TYPE  HcFmRemaining;      // 0x038  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcFmNumber;         // 0x03C  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcPeriodStart;      // 0x040  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcLSThreshold;      // 0x044  R/W  0x00000628
    volatile TCC_DEF32BIT_TYPE  HcRhDescriptorA;    // 0x048  R/W  0x02001202   // Root hub registers
    volatile TCC_DEF32BIT_TYPE  HcRhDescriptorB;    // 0x04C  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcRhStatus;         // 0x050  R/W  0x00000000
    volatile TCC_DEF32BIT_TYPE  HcRhPortStatus1;    // 0x054  R/W  0x00000100
    volatile TCC_DEF32BIT_TYPE  HcRhPortStatus2;    // 0x058  R/W  0x00000100
} USB_OHCI, *PUSB_OHCI;


/************************************************************************
*   5. SECURITY (Cipher)                         (Base Addr = 0x71EC0000)
*************************************************************************/

typedef struct {
    unsigned SELECT         :2;
    unsigned ENC            :1;
    unsigned PRT            :1;
    unsigned OPMODE         :3;
    unsigned DESMODE        :2;
    unsigned KEYLEN         :2;
    unsigned KEYLD          :1;
    unsigned IVLD           :1;
    unsigned BCLR           :1;
    unsigned RCLR           :1;
    unsigned WCLR           :1;
} CIPHER_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_CTRL_IDX_TYPE    bREG;
} CIPHER_CTRL_TYPE;

typedef struct {
    unsigned ADDR           :32;
} CIPHER_BASE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_BASE_IDX_TYPE    bREG;
} CIPHER_BASE_TYPE;

typedef struct {
    unsigned SIZE           :13;
    unsigned                :3;
    unsigned COUNT          :13;
    unsigned                :3;
} CIPHER_PACKET_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_PACKET_IDX_TYPE  bREG;
} CIPHER_PACKET_TYPE;

typedef struct {
    unsigned EN             :1;
    unsigned                :1;
    unsigned PCLK           :1;
    unsigned                :11;
    unsigned RXAM           :2;
    unsigned TXAM           :2;
    unsigned                :10;
    unsigned END            :1;
    unsigned                :2;
    unsigned DE             :1;
} CIPHER_DMACTR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_DMACTR_IDX_TYPE  bREG;
} CIPHER_DMACTR_TYPE;

typedef struct {
    unsigned TXCNT          :13;
    unsigned                :3;
    unsigned RXCNT          :13;
    unsigned                :3;
} CIPHER_DMASTS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_DMASTS_IDX_TYPE  bREG;
} CIPHER_DMASTS_TYPE;

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
} CIPHER_IRQCTR_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_IRQCTR_IDX_TYPE  bREG;
} CIPHER_IRQCTR_TYPE;

typedef struct {
    unsigned BLKNUM         :32;
} CIPHER_BLKNUM_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_BLKNUM_IDX_TYPE  bREG;
} CIPHER_BLKNUM_TYPE;

typedef struct {
    unsigned ROUND          :16;
    unsigned                :16;
} CIPHER_ROUND_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_ROUND_IDX_TYPE   bREG;
} CIPHER_ROUND_TYPE;

typedef struct {
    unsigned KEY            :32;
} CIPHER_KEY_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_KEY_IDX_TYPE     bREG;
} CIPHER_KEY_TYPE;

typedef struct {
    unsigned IV             :32;
} CIPHER_IV_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    CIPHER_IV_IDX_TYPE      bREG;
} CIPHER_IV_TYPE;

typedef struct _CIPHER{
    volatile CIPHER_CTRL_TYPE   CTRL;           // 0x000  R/W  0x00000000   Cipher control regsiter
    volatile CIPHER_BASE_TYPE   TXBASE;         // 0x004  R/W  0x00000000   TX base address register
    volatile CIPHER_BASE_TYPE   RXBASE;         // 0x008  R/W  0x00000000   RX base address register
    volatile CIPHER_PACKET_TYPE PACKET;         // 0x00C  R/W  0x00000000   Packet register
    volatile CIPHER_DMACTR_TYPE DMACTR;         // 0x010  R/W  0x00000000   DMA control register
    volatile CIPHER_DMASTS_TYPE DMASTR;         // 0x014  RO   -            DMA status register
    volatile CIPHER_IRQCTR_TYPE IRQCTR;         // 0x018  R/W  0x00000000   Interrupt control register
    volatile CIPHER_BLKNUM_TYPE BLKNUM;         // 0x01C  RO   -            Block count register
    volatile CIPHER_ROUND_TYPE  ROUND;          // 0x020  R/W  0x00000000   Round register
    unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile CIPHER_KEY_TYPE    KEY0;           // 0x040  R/W  0x00000000   Key0 register
    volatile CIPHER_KEY_TYPE    KEY1;           // 0x044  R/W  0x00000000   Key1 register
    volatile CIPHER_KEY_TYPE    KEY2;           // 0x048  R/W  0x00000000   Key2 register
    volatile CIPHER_KEY_TYPE    KEY3;           // 0x04C  R/W  0x00000000   Key3 register
    volatile CIPHER_KEY_TYPE    KEY4;           // 0x050  R/W  0x00000000   Key4 register
    volatile CIPHER_KEY_TYPE    KEY5;           // 0x054  R/W  0x00000000   Key5 register
    volatile CIPHER_KEY_TYPE    KEY6;           // 0x058  R/W  0x00000000   Key6 register
    volatile CIPHER_KEY_TYPE    KEY7;           // 0x05C  R/W  0x00000000   Key7 register
    volatile CIPHER_IV_TYPE     IV0;            // 0x068  R/W  0x00000000   Initial vector0 register
    volatile CIPHER_IV_TYPE     IV1;            // 0x06C  R/W  0x00000000   Initial vector1 register
    volatile CIPHER_IV_TYPE     IV2;            // 0x070  R/W  0x00000000   Initial vector2 register
    volatile CIPHER_IV_TYPE     IV3;            // 0x074  R/W  0x00000000   Initial vector3 register
} CIPHER, *PCIPHER;


/************************************************************************
*   6. DMAX                                      (Base Addr = 0x71EB0000)
*************************************************************************/

typedef struct {
    unsigned EN             :1;
    unsigned TYPE           :1;
    unsigned                :6;
    unsigned ROPT           :1;
    unsigned RTYPE          :1;
    unsigned                :5;
    unsigned WB             :1;
    unsigned REN            :1;
    unsigned RPAUSE         :3;
    unsigned RWAIT          :3;
    unsigned IEN            :1;
    unsigned CH             :5;
    unsigned                :3;
} DMAX_CTRL_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    DMAX_CTRL_IDX_TYPE      bREG;
} DMAX_CTRL_TYPE;

typedef struct {
    unsigned ADDR           :32;
} DMAX_BASE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    DMAX_BASE_IDX_TYPE      bREG;
} DMAX_BASE_TYPE;

typedef struct {
    unsigned SIZE           :24;
    unsigned                :8;
} DMAX_SIZE_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    DMAX_SIZE_IDX_TYPE      bREG;
} DMAX_SIZE_TYPE;

typedef struct {
    unsigned NEMPTY         :8;
    unsigned                :21;
    unsigned IDLE           :1;
    unsigned EMP            :1;
    unsigned FULL           :1;
} DMAX_FIFO0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    DMAX_FIFO0_IDX_TYPE     bREG;
} DMAX_FIFO0_TYPE;

typedef struct {
    unsigned FIFO           :32;
} DMAX_FIFO_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    DMAX_FIFO_IDX_TYPE      bREG;
} DMAX_FIFO_TYPE;

typedef struct {
    unsigned INT            :32;
} DMAX_INT_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    DMAX_INT_IDX_TYPE       bREG;
} DMAX_INT_TYPE;

typedef struct _DMAX{
    volatile DMAX_CTRL_TYPE     CTRL;           // 0x000  R/W  0x00000000   Control Register
    volatile DMAX_BASE_TYPE     SBASE;          // 0x004  R/W  0x00000000   Source Base Address Register
    volatile DMAX_BASE_TYPE     DBASE;          // 0x008  R/W  0x00000000   Destination Base Address Register
    volatile DMAX_SIZE_TYPE     SIZE;           // 0x00C  R/W  0x00000000   Transfer Byte Size Register
    volatile DMAX_FIFO0_TYPE    FIFO0;          // 0x010  R/W  0x00000000   FIFO Status 0 Register
    volatile DMAX_FIFO_TYPE     FIFO1;          // 0x014  R/W  0x00000000   FIFO Status 1 Register
    volatile DMAX_FIFO_TYPE     FIFO2;          // 0x018  R/W  0x00000000   FIFO Status 2 Register
    volatile DMAX_FIFO_TYPE     FIFO3;          // 0x01C  R/W  0x00000000   FIFO Status 3 Register
    volatile DMAX_INT_TYPE      INTMSK;         // 0x020  R/W  0x00000000   Interrupt Mask Register
    volatile DMAX_INT_TYPE      INTSTS;         // 0x024  R/W  0x00000000   Interrupt Status Register
} DMAX, *PDMAX;


/************************************************************************
*   7. HSIO BUS Configuration Registers          (Base Addr = 0x71EA0000)
*************************************************************************/

typedef struct {
    unsigned DMAX           :1;
    unsigned GMAC_G         :1;
    unsigned GMAC           :1;
    unsigned                :3;
    unsigned US20H          :1;
    unsigned                :5;
    unsigned SEC_CTRL       :1;
    unsigned                :3;
    unsigned HSOCFG         :1;
    unsigned                :1;
    unsigned CIPHER         :1;
    unsigned                :13;
} HSIO_HCLK_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    HSIO_HCLK_IDX_TYPE      bREG;
} HSIO_HCLK_TYPE;

typedef struct {
    unsigned FGS            :2;
    unsigned FG             :2;
    unsigned                :28;
} USB20H_PCFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    USB20H_PCFG_IDX_TYPE    bREG;
} USB20H_PCFG_TYPE;

typedef struct {
    unsigned LBE            :1;
    unsigned VBD            :1;
    unsigned TBS            :1;
    unsigned TBSH           :1;
    unsigned DPPD           :1;
    unsigned DMPD           :1;
    unsigned VBDS           :1;
    unsigned                :1;
    unsigned SDI            :1;
    unsigned RCD            :2;
    unsigned RCS            :2;
    unsigned CM             :1;
    unsigned PR             :1;
    unsigned                :1;
    unsigned CDT            :3;
    unsigned                :1;
    unsigned OTGT           :3;
    unsigned                :1;
    unsigned SQRXT          :3;
    unsigned                :1;
    unsigned TXFSLST        :4;
} USB20H_PCFG0_IDX_TYPE;

typedef union {
    unsigned long            nREG;
    USB20H_PCFG0_IDX_TYPE    bREG;
} USB20H_PCFG0_TYPE;

typedef struct {
    unsigned TP             :1;
    unsigned                :1;
    unsigned TXRT           :1;
    unsigned                :2;
    unsigned TXVRT          :4;
    unsigned                :7;
    unsigned ABE            :1;
    unsigned WI             :1;
    unsigned TXHSXVT        :2;
    unsigned                :4;
    unsigned DCV            :1;
    unsigned CVB            :1;
    unsigned DVB            :1;
    unsigned IDPU           :1;
    unsigned OTG            :1;
    unsigned SM             :1;
    unsigned                :1;
    unsigned SR             :1;
} USB20H_PCFG1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    USB20H_PCFG1_IDX_TYPE   bREG;
} USB20H_PCFG1_TYPE;

typedef struct {
    unsigned FLA            :6;
    unsigned SM             :1;
    unsigned APO            :1;
    unsigned POA            :1;
    unsigned OCS            :1;
    unsigned                :6;
    unsigned TDI            :8;
    unsigned TAD            :4;
    unsigned TCK            :1;
    unsigned TDO            :1;
    unsigned                :2;
} USB20H_PCFG2_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    USB20H_PCFG2_IDX_TYPE   bREG;
} USB20H_PCFG2_TYPE;

typedef struct {
    unsigned USBSTS         :6;
    unsigned                :6;
    unsigned TDOUT          :4;
    unsigned                :16;
} USB20H_STS_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    USB20H_STS_IDX_TYPE     bREG;
} USB20H_STS_TYPE;

typedef struct {
    unsigned                :6;
    unsigned A2X_USB20H     :2;
    unsigned                :10;
    unsigned BWC            :1;
    unsigned BWU            :1;
    unsigned                :12;
} HSIO_CFG_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    HSIO_CFG_IDX_TYPE       bREG;
} HSIO_CFG_TYPE;

typedef struct {
    unsigned RXCLK_SEL      :2;
    unsigned                :2;
    unsigned RXDIV          :6;
    unsigned                :5;
    unsigned RR             :1;
    unsigned TXCLK_SEL      :2;
    unsigned                :2;
    unsigned TXDIV          :6;
    unsigned                :5;
    unsigned TR             :1;
} HSIO_ETHERCFG0_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    HSIO_ETHERCFG0_IDX_TYPE bREG;
} HSIO_ETHERCFG0_TYPE;

typedef struct {
    unsigned                :16;
    unsigned TCO            :1;
    unsigned FCTRL          :1;
    unsigned PHY_INFSEL     :3;
    unsigned                :9;
    unsigned CC             :1;
    unsigned CE             :1;
} HSIO_ETHERCFG1_IDX_TYPE;

typedef union {
    unsigned long           nREG;
    HSIO_ETHERCFG1_IDX_TYPE bREG;
} HSIO_ETHERCFG1_TYPE;

typedef struct _HSIOBUSCFG{
    volatile HSIO_HCLK_TYPE         PWDN;           // 0x000  R/W  0xFFFFFFFF   Module Clock Mask Register
    volatile HSIO_HCLK_TYPE         SWRESET;        // 0x004  R/W  0xFFFFFFFF   Module Software Reset Register
    unsigned :32; unsigned :32;
    volatile USB20H_PCFG_TYPE       USB20H_PCFG;    // 0x010  R/W  0x00000000    
    volatile USB20H_PCFG0_TYPE      USB20H_PCFG0;   // 0x014  R/W  0x00000000    
    volatile USB20H_PCFG1_TYPE      USB20H_PCFG1;   // 0x018  R/W  0x00000000
    volatile USB20H_PCFG2_TYPE      USB20H_PCFG2;   // 0x01C  R/W  0x00000000
    volatile USB20H_STS_TYPE        USB20H_STS;     // 0x020  R/W  0x00000000
    unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32; unsigned :32; unsigned :32;
    volatile HSIO_CFG_TYPE          HSIO_CFG;       // 0x050  R/W  0x03FF0001
    unsigned :32; unsigned :32; unsigned :32;
    unsigned :32; unsigned :32;
    volatile HSIO_ETHERCFG0_TYPE    ETHER_CFG0;     // 0x068  R/W  0x00000000
    volatile HSIO_ETHERCFG1_TYPE    ETHER_CFG1;     // 0x06C  R/W  0x00000000
} HSIOBUSCFG, *PHSIOBUSCFG;

#endif /* _STRUCTURES_HSIO_H_ */
