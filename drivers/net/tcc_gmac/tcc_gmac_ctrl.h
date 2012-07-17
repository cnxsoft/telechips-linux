/*
 * linux/driver/net/tcc_gmac/tcc_gmac_drv.h
 * 	
 * Based on : STMMAC of STLinux 2.4
 * Author : Telechips <linux@telechips.com>
 * Created : June 22, 2010
 * Description : This is the driver for the Telechips MAC 10/100/1000 on-chip Ethernet controllers.  
 *               Telechips Ethernet IPs are built around a Synopsys IP Core.
 *
 * Copyright (C) 2010 Telechips
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 */ 

#ifndef _TCC_GMAC_CTRL_H_
#define _TCC_GMAC_CTRL_H_

/*****************************************
 *
 *        TCC GMAC Debug Functions
 *
 *****************************************/

#undef TCC_GMAC_DEBUG
// #define TCC_GMAC_DEBUG 
#undef FRAME_FILTER_DEBUG
/* #define FRAME_FILTER_DEBUG */
#ifdef TCC_GMAC_DEBUG
#define CTRL_DBG(fmt, args...)  printk(fmt, ## args)
#else
#define CTRL_DBG(fmt, args...)  do { } while (0)
#endif

/*****************************************
 *
 *   TCC GMAC DMA Control Definitions
 *
 *****************************************/

/* DMA CRS Control and Status Register Mapping */
#define DMA_BUS_MODE			0x00001000	/* Bus Mode */
#define DMA_XMT_POLL_DEMAND		0x00001004	/* Transmit Poll Demand */
#define DMA_RCV_POLL_DEMAND		0x00001008	/* Received Poll Demand */
#define DMA_RCV_BASE_ADDR		0x0000100c	/* Receive List Base */
#define DMA_TX_BASE_ADDR		0x00001010	/* Transmit List Base */
#define DMA_STATUS				0x00001014	/* Status Register */
#define DMA_CONTROL				0x00001018	/* Ctrl (Operational Mode) */
#define DMA_INTR_ENA			0x0000101c	/* Interrupt Enable */
#define DMA_MISSED_FRAME_CTR	0x00001020	/* Missed Frame Counter */
#define DMA_CUR_TX_BUF_ADDR		0x00001050	/* Current Host Tx Buffer */
#define DMA_CUR_RX_BUF_ADDR		0x00001054	/* Current Host Rx Buffer */

/* DMA Control register defines */
#define DMA_CONTROL_ST			0x00002000	/* Start/Stop Transmission */
#define DMA_CONTROL_SR			0x00000002	/* Start/Stop Receive */

/* DMA Normal interrupt */
#define DMA_INTR_ENA_NIE		0x00010000	/* Normal Summary */
#define DMA_INTR_ENA_TIE		0x00000001	/* Transmit Interrupt */
#define DMA_INTR_ENA_TUE		0x00000004	/* Transmit Buffer Unavailable */
#define DMA_INTR_ENA_RIE		0x00000040	/* Receive Interrupt */
#define DMA_INTR_ENA_ERE		0x00004000	/* Early Receive */

#define DMA_INTR_NORMAL	(DMA_INTR_ENA_NIE | DMA_INTR_ENA_RIE | \
			DMA_INTR_ENA_TIE)

/* DMA Abnormal interrupt */
#define DMA_INTR_ENA_AIE		0x00008000	/* Abnormal Summary */
#define DMA_INTR_ENA_FBE		0x00002000	/* Fatal Bus Error */
#define DMA_INTR_ENA_ETE		0x00000400	/* Early Transmit */
#define DMA_INTR_ENA_RWE		0x00000200	/* Receive Watchdog */
#define DMA_INTR_ENA_RSE		0x00000100	/* Receive Stopped */
#define DMA_INTR_ENA_RUE		0x00000080	/* Receive Buffer Unavailable */
#define DMA_INTR_ENA_UNE		0x00000020	/* Tx Underflow */
#define DMA_INTR_ENA_OVE		0x00000010	/* Receive Overflow */
#define DMA_INTR_ENA_TJE		0x00000008	/* Transmit Jabber */
#define DMA_INTR_ENA_TSE		0x00000002	/* Transmit Stopped */

#define DMA_INTR_ABNORMAL	(DMA_INTR_ENA_AIE | DMA_INTR_ENA_FBE | \
				DMA_INTR_ENA_UNE)

/* DMA default interrupt mask */
#define DMA_INTR_DEFAULT_MASK	(DMA_INTR_NORMAL | DMA_INTR_ABNORMAL)

/* DMA Status register defines */
#define DMA_STATUS_GPI			0x10000000	/* PMT interrupt */
#define DMA_STATUS_GMI			0x08000000	/* MMC interrupt */
#define DMA_STATUS_GLI			0x04000000	/* GMAC Line interface int */
#define DMA_STATUS_GMI			0x08000000
#define DMA_STATUS_GLI			0x04000000
#define DMA_STATUS_EB_MASK		0x00380000	/* Error Bits Mask */
#define DMA_STATUS_EB_TX_ABORT	0x00080000	/* Error Bits - TX Abort */
#define DMA_STATUS_EB_RX_ABORT	0x00100000	/* Error Bits - RX Abort */
#define DMA_STATUS_TS_MASK		0x00700000	/* Transmit Process State */
#define DMA_STATUS_TS_SHIFT		20
#define DMA_STATUS_RS_MASK		0x000e0000	/* Receive Process State */
#define DMA_STATUS_RS_SHIFT		17
#define DMA_STATUS_NIS			0x00010000	/* Normal Interrupt Summary */
#define DMA_STATUS_AIS			0x00008000	/* Abnormal Interrupt Summary */
#define DMA_STATUS_ERI			0x00004000	/* Early Receive Interrupt */
#define DMA_STATUS_FBI			0x00002000	/* Fatal Bus Error Interrupt */
#define DMA_STATUS_ETI			0x00000400	/* Early Transmit Interrupt */
#define DMA_STATUS_RWT			0x00000200	/* Receive Watchdog Timeout */
#define DMA_STATUS_RPS			0x00000100	/* Receive Process Stopped */
#define DMA_STATUS_RU			0x00000080	/* Receive Buffer Unavailable */
#define DMA_STATUS_RI			0x00000040	/* Receive Interrupt */
#define DMA_STATUS_UNF			0x00000020	/* Transmit Underflow */
#define DMA_STATUS_OVF			0x00000010	/* Receive Overflow */
#define DMA_STATUS_TJT			0x00000008	/* Transmit Jabber Timeout */
#define DMA_STATUS_TU			0x00000004	/* Transmit Buffer Unavailable */
#define DMA_STATUS_TPS			0x00000002	/* Transmit Process Stopped */
#define DMA_STATUS_TI			0x00000001	/* Transmit Interrupt */


/*****************************************
 *
 *   TCC GMAC MAC Control Definitions
 *
 *****************************************/

#define GMAC_CONTROL			0x00000000	/* Configuration */
#define GMAC_FRAME_FILTER		0x00000004	/* Frame Filter */
#define GMAC_HASH_HIGH			0x00000008	/* Multicast Hash Table High */
#define GMAC_HASH_LOW			0x0000000c	/* Multicast Hash Table Low */
#define GMAC_MII_ADDR			0x00000010	/* MII Address */
#define GMAC_MII_DATA			0x00000014	/* MII Data */
#define GMAC_FLOW_CTRL			0x00000018	/* Flow Control */
#define GMAC_VLAN_TAG			0x0000001c	/* VLAN Tag */
#define GMAC_VERSION			0x00000020	/* GMAC CORE Version */
#define GMAC_WAKEUP_FILTER		0x00000028	/* Wake-up Frame Filter */
#define GMAC_HASH_TABLE_0		0x00000500	/* Hash Table 0 */
#define GMAC_HASH_TABLE_1		0x00000504	/* Hash Table 1 */
#define GMAC_HASH_TABLE_2		0x00000508	/* Hash Table 2 */
#define GMAC_HASH_TABLE_3		0x0000050c	/* Hash Table 3 */
#define GMAC_HASH_TABLE_4		0x00000510	/* Hash Table 4 */
#define GMAC_HASH_TABLE_5		0x00000514	/* Hash Table 5 */
#define GMAC_HASH_TABLE_6		0x00000518	/* Hash Table 6 */
#define GMAC_HASH_TABLE_7		0x0000051c	/* Hash Table 7 */

#define GMAC_INT_STATUS			0x00000038	/* interrupt status register */
enum tcc_gmac_irq_status {
	time_stamp_irq = 0x0200,
	mmc_rx_csum_offload_irq = 0x0080,
	mmc_tx_irq = 0x0040,
	mmc_rx_irq = 0x0020,
	mmc_irq = 0x0010,
	pmt_irq = 0x0008,
	pcs_ane_irq = 0x0004,
	pcs_link_irq = 0x0002,
	rgmii_irq = 0x0001,
};
#define GMAC_INT_MASK		0x0000003c	/* interrupt mask register */

/* PMT Control and Status */
#define GMAC_PMT		0x0000002c
enum power_event {
	pointer_reset = 0x80000000,
	global_unicast = 0x00000200,
	wake_up_rx_frame = 0x00000040,
	magic_frame = 0x00000020,
	wake_up_frame_en = 0x00000004,
	magic_pkt_en = 0x00000002,
	power_down = 0x00000001,
};

/* GMAC HW ADDR regs */
#define GMAC_ADDR_HIGH(reg)			(0x00000040+(reg * 8))
#define GMAC_ADDR_LOW(reg)			(0x00000044+(reg * 8))
#define GMAC_MAX_UNICAST_ADDRESSES	16

#define GMAC_AN_CTRL			0x000000c0	/* AN control */
#define GMAC_AN_STATUS			0x000000c4	/* AN status */
#define GMAC_ANE_ADV			0x000000c8	/* Auto-Neg. Advertisement */
#define GMAC_ANE_LINK			0x000000cc	/* Auto-Neg. link partener ability */
#define GMAC_ANE_EXP			0x000000d0	/* ANE expansion */
#define GMAC_TBI				0x000000d4	/* TBI extend status */
#define GMAC_GMII_STATUS		0x000000d8	/* S/R-GMII status */

/* GMAC Configuration defines */
#define GMAC_CONTROL_TC			0x01000000	/* Transmit Conf. in RGMII/SGMII */
#define GMAC_CONTROL_WD			0x00800000	/* Disable Watchdog on receive */
#define GMAC_CONTROL_JD			0x00400000	/* Jabber disable */
#define GMAC_CONTROL_BE			0x00200000	/* Frame Burst Enable */
#define GMAC_CONTROL_JE			0x00100000	/* Jumbo frame */
enum inter_frame_gap {
	GMAC_CONTROL_IFG_88 = 0x00040000,
	GMAC_CONTROL_IFG_80 = 0x00020000,
	GMAC_CONTROL_IFG_40 = 0x000e0000,
};
#define GMAC_CONTROL_DCRS		0x00010000 /* Disable carrier sense during tx */
#define GMAC_CONTROL_PS			0x00008000 /* Port Select 0:GMI 1:MII */
#define GMAC_CONTROL_FES		0x00004000 /* Speed 0:10 1:100 */
#define GMAC_CONTROL_DO			0x00002000 /* Disable Rx Own */
#define GMAC_CONTROL_LM			0x00001000 /* Loop-back mode */
#define GMAC_CONTROL_DM			0x00000800 /* Duplex Mode */
#define GMAC_CONTROL_IPC		0x00000400 /* Checksum Offload */
#define GMAC_CONTROL_DR			0x00000200 /* Disable Retry */
#define GMAC_CONTROL_LUD		0x00000100 /* Link up/down */
#define GMAC_CONTROL_ACS		0x00000080 /* Automatic Pad Stripping */
#define GMAC_CONTROL_DC			0x00000010 /* Deferral Check */
#define GMAC_CONTROL_TE			0x00000008 /* Transmitter Enable */
#define GMAC_CONTROL_RE			0x00000004 /* Receiver Enable */

#define GMAC_CORE_INIT (GMAC_CONTROL_JD | GMAC_CONTROL_PS | GMAC_CONTROL_ACS | \
			GMAC_CONTROL_IPC | /*GMAC_CONTROL_JE |*/ GMAC_CONTROL_BE | GMAC_CONTROL_TC)

/* GMAC Frame Filter defines */
#define GMAC_FRAME_FILTER_PR	0x00000001	/* Promiscuous Mode */
#define GMAC_FRAME_FILTER_HUC	0x00000002	/* Hash Unicast */
#define GMAC_FRAME_FILTER_HMC	0x00000004	/* Hash Multicast */
#define GMAC_FRAME_FILTER_DAIF	0x00000008	/* DA Inverse Filtering */
#define GMAC_FRAME_FILTER_PM	0x00000010	/* Pass all multicast */
#define GMAC_FRAME_FILTER_DBF	0x00000020	/* Disable Broadcast frames */
#define GMAC_FRAME_FILTER_SAIF	0x00000100	/* Inverse Filtering */
#define GMAC_FRAME_FILTER_SAF	0x00000200	/* Source Address Filter */
#define GMAC_FRAME_FILTER_HPF	0x00000400	/* Hash or perfect Filter */
#define GMAC_FRAME_FILTER_RA	0x80000000	/* Receive all mode */

/* GMII ADDR  defines */
#define GMAC_MII_ADDR_WRITE		0x00000002	/* MII Write */
#define GMAC_MII_ADDR_BUSY		0x00000001	/* MII Busy */

#define GMII_CLK_RANGE_60_100M      0x00000000  /* MDC = Clk/42 */
#define GMII_CLK_RANGE_100_150M     0x00000004  /* MDC = Clk/62 */
#define GMII_CLK_RANGE_20_35M       0x00000008  /* MDC = Clk/16 */
#define GMII_CLK_RANGE_35_60M       0x0000000C  /* MDC = Clk/26 */
#define GMII_CLK_RANGE_150_250M     0x00000010  /* MDC = Clk/102 */
#define GMII_CLK_RANGE_250_300M     0x00000014  /* MDC = Clk/122 */

#define GMII_ADDR_SHIFT         11
#define GMII_REG_SHIFT          6
#define GMII_REG_MSK            0x000007c0
#define GPHY_ADDR_MASK          0x0000f800

#define GMII_DATA_REG           0x014
#define GMII_DATA_REG_MSK       0x0000ffff

/* GMAC FLOW CTRL defines */
#define GMAC_FLOW_CTRL_PT_MASK	0xffff0000	/* Pause Time Mask */
#define GMAC_FLOW_CTRL_PT_SHIFT	16
#define GMAC_FLOW_CTRL_RFE		0x00000004	/* Rx Flow Control Enable */
#define GMAC_FLOW_CTRL_TFE		0x00000002	/* Tx Flow Control Enable */
#define GMAC_FLOW_CTRL_FCB_BPA	0x00000001	/* Flow Control Busy ... */

/*--- DMA BLOCK defines ---*/
/* DMA Bus Mode register defines */
#define DMA_BUS_MODE_SFT_RESET	0x00000001	/* Software Reset */
#define DMA_BUS_MODE_DA			0x00000002	/* Arbitration scheme */
#define DMA_BUS_MODE_DSL_MASK	0x0000007c	/* Descriptor Skip Length */
#define DMA_BUS_MODE_DSL_SHIFT	2	/*   (in DWORDS)      */
/* Programmable burst length (passed thorugh platform)*/
#define DMA_BUS_MODE_PBL_MASK	0x00003f00	/* Programmable Burst Len */
#define DMA_BUS_MODE_PBL_SHIFT	8

enum rx_tx_priority_ratio {
	double_ratio = 0x00004000,	/*2:1 */
	triple_ratio = 0x00008000,	/*3:1 */
	quadruple_ratio = 0x0000c000,	/*4:1 */
};

#define DMA_BUS_MODE_FB			0x00010000	/* Fixed burst */
#define DMA_BUS_MODE_RPBL_MASK	0x003e0000	/* Rx-Programmable Burst Len */
#define DMA_BUS_MODE_RPBL_SHIFT	17
#define DMA_BUS_MODE_USP		0x00800000
#define DMA_BUS_MODE_4PBL		0x01000000
#define DMA_BUS_MODE_AAL		0x02000000

/* DMA CRS Control and Status Register Mapping */
#define DMA_HOST_TX_DESC		0x00001048	/* Current Host Tx descriptor */
#define DMA_HOST_RX_DESC		0x0000104c	/* Current Host Rx descriptor */
/*  DMA Bus Mode register defines */
#define DMA_BUS_PR_RATIO_MASK	 0x0000c000	/* Rx/Tx priority ratio */
#define DMA_BUS_PR_RATIO_SHIFT	 14
#define DMA_BUS_FB	  	 		 0x00010000	/* Fixed Burst */

/* DMA operation mode defines (start/stop tx/rx are placed in common header)*/
#define DMA_CONTROL_DT			0x04000000 /* Disable Drop TCP/IP csum error */
#define DMA_CONTROL_RSF			0x02000000 /* Receive Store and Forward */
#define DMA_CONTROL_DFF			0x01000000 /* Disaable flushing */
/* Threshold for Activating the FC */
enum rfa {
	act_full_minus_1 = 0x00800000,
	act_full_minus_2 = 0x00800200,
	act_full_minus_3 = 0x00800400,
	act_full_minus_4 = 0x00800600,
};
/* Threshold for Deactivating the FC */
enum rfd {
	deac_full_minus_1 = 0x00400000,
	deac_full_minus_2 = 0x00400800,
	deac_full_minus_3 = 0x00401000,
	deac_full_minus_4 = 0x00401800,
};
#define DMA_CONTROL_TSF		0x00200000 /* Transmit  Store and Forward */
#define DMA_CONTROL_FTF		0x00100000 /* Flush transmit FIFO */

enum ttc_control {
	DMA_CONTROL_TTC_64 = 0x00000000,
	DMA_CONTROL_TTC_128 = 0x00004000,
	DMA_CONTROL_TTC_192 = 0x00008000,
	DMA_CONTROL_TTC_256 = 0x0000c000,
	DMA_CONTROL_TTC_40 = 0x00010000,
	DMA_CONTROL_TTC_32 = 0x00014000,
	DMA_CONTROL_TTC_24 = 0x00018000,
	DMA_CONTROL_TTC_16 = 0x0001c000,
};
#define DMA_CONTROL_TC_TX_MASK	0xfffe3fff

#define DMA_CONTROL_EFC			0x00000100
#define DMA_CONTROL_FEF			0x00000080
#define DMA_CONTROL_FUF			0x00000040

enum rtc_control {
	DMA_CONTROL_RTC_64 = 0x00000000,
	DMA_CONTROL_RTC_32 = 0x00000008,
	DMA_CONTROL_RTC_96 = 0x00000010,
	DMA_CONTROL_RTC_128 = 0x00000018,
};
#define DMA_CONTROL_TC_RX_MASK	0xffffffe7

#define DMA_CONTROL_OSF	0x00000004	/* Operate on second frame */

/* MMC registers offset */
#define GMAC_MMC_CTRL				0x100
#define GMAC_MMC_RX_INTR			0x104
#define GMAC_MMC_TX_INTR			0x108
#define GMAC_MMC_RX_CSUM_OFFLOAD	0x208


/*****************************************
 *
 *   TCC GMAC Descriptor Definitions
 *
 *****************************************/
struct dma_desc {
	union {
		/* Receive descriptor */
		struct {
			/* RDES0 */
			u32 payload_csum_error:1;
			u32 crc_error:1;
			u32 dribbling:1;
			u32 error_gmii:1;
			u32 receive_watchdog:1;
			u32 frame_type:1;
			u32 late_collision:1;
			u32 ipc_csum_error:1;
			u32 last_descriptor:1;
			u32 first_descriptor:1;
			u32 vlan_tag:1;
			u32 overflow_error:1;
			u32 length_error:1;
			u32 sa_filter_fail:1;
			u32 descriptor_error:1;
			u32 error_summary:1;
			u32 frame_length:14;
			u32 da_filter_fail:1;
			u32 own:1;
			/* RDES1 */
			u32 buffer1_size:13;
			u32 reserved1:1;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 buffer2_size:13;
			u32 reserved2:2;
			u32 disable_ic:1;
		} erx;		/* -- enhanced -- */

		/* Transmit descriptor */
		struct {
			/* TDES0 */
			u32 deferred:1;
			u32 underflow_error:1;
			u32 excessive_deferral:1;
			u32 collision_count:4;
			u32 vlan_frame:1;
			u32 excessive_collisions:1;
			u32 late_collision:1;
			u32 no_carrier:1;
			u32 loss_carrier:1;
			u32 payload_error:1;
			u32 frame_flushed:1;
			u32 jabber_timeout:1;
			u32 error_summary:1;
			u32 ip_header_error:1;
			u32 time_stamp_status:1;
			u32 reserved1:2;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 checksum_insertion:2;
			u32 reserved2:1;
			u32 time_stamp_enable:1;
			u32 disable_padding:1;
			u32 crc_disable:1;
			u32 first_segment:1;
			u32 last_segment:1;
			u32 interrupt:1;
			u32 own:1;
			/* TDES1 */
			u32 buffer1_size:13;
			u32 reserved3:3;
			u32 buffer2_size:13;
			u32 reserved4:3;
		} etx;		/* -- enhanced -- */
	} des01;
	unsigned int des2;
	unsigned int des3;
};

/* Transmit checksum insertion control */
enum tdes_csum_insertion {
	cic_disabled = 0,	/* Checksum Insertion Control */
	cic_only_ip = 1,	/* Only IP header */
	cic_no_pseudoheader = 2,	/* IP header but pseudoheader is not calculated */
	cic_full = 3,		/* IP header and pseudoheader */
};

struct mac_device_info *tcc_gmac_setup(unsigned long ioaddr);

#endif /*_TCC_GMAC_CTRL_H_*/
