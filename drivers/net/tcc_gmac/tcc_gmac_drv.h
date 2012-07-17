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

#ifndef _TCC_GMAC_DRV_H_
#define _TCC_GMAC_DRV_H_

#if defined(CONFIG_TCC_GMAC_MII_MODE)
	#define DRV_MODULE_VERSION "TCC GMAC MII Version\n"
#elif defined(CONFIG_TCC_GMAC_GMII_MODE)
	#define DRV_MODULE_VERSION "TCC GMAC GMII Version\n"
#elif defined(CONFIG_TCC_GMAC_RGMII_MODE)
	#define DRV_MODULE_VERSION "TCC GMAC RMII Version\n"
#endif

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#define TCC_GMAC_VLAN_TAG_USED
#include <linux/if_vlan.h>
#endif

#include "tcc_gmac_ctrl.h"
#include <linux/netdevice.h>
#include <linux/phy.h>

void tcc_gmac_set_ethtool_ops(struct net_device *netdev);

struct tcc_gmac_extra_stats {
	/* Transmit errors */
	unsigned long tx_underflow ____cacheline_aligned;
	unsigned long tx_carrier;
	unsigned long tx_losscarrier;
	unsigned long tx_heartbeat;
	unsigned long tx_deferred;
	unsigned long tx_vlan;
	unsigned long tx_jabber;
	unsigned long tx_frame_flushed;
	unsigned long tx_payload_error;
	unsigned long tx_ip_header_error;
	/* Receive errors */
	unsigned long rx_desc;
	unsigned long rx_partial;
	unsigned long rx_runt;
	unsigned long rx_toolong;
	unsigned long rx_collision;
	unsigned long rx_crc;
	unsigned long rx_length;
	unsigned long rx_mii;
	unsigned long rx_multicast;
	unsigned long rx_gmac_overflow;
	unsigned long rx_watchdog;
	unsigned long da_rx_filter_fail;
	unsigned long sa_rx_filter_fail;
	unsigned long rx_missed_cntr;
	unsigned long rx_overflow_cntr;
	unsigned long rx_vlan;
	/* Tx/Rx IRQ errors */
	unsigned long tx_undeflow_irq;
	unsigned long tx_process_stopped_irq;
	unsigned long tx_jabber_irq;
	unsigned long rx_overflow_irq;
	unsigned long rx_buf_unav_irq;
	unsigned long rx_process_stopped_irq;
	unsigned long rx_watchdog_irq;
	unsigned long tx_early_irq;
	unsigned long fatal_bus_error_irq;
	/* Extra info */
	unsigned long threshold;
	unsigned long tx_pkt_n;
	unsigned long rx_pkt_n;
	unsigned long poll_n;
	unsigned long sched_timer_n;
	unsigned long normal_irq_n;
};

#define HASH_TABLE_SIZE 64
#define PAUSE_TIME 0x200

/* Flow Control defines */
#define FLOW_OFF	0
#define FLOW_RX		1
#define FLOW_TX		2
#define FLOW_AUTO	(FLOW_TX | FLOW_RX)

#define SF_DMA_MODE 1 /* DMA STORE-AND-FORWARD Operation Mode */

#define HW_CSUM 1
#define NO_HW_CSUM 0
enum rx_frame_status { /* IPC status */
	good_frame = 0,
	discard_frame = 1,
	csum_none = 2,
};

enum tx_dma_irq_status {
	tx_hard_error = 1,
	tx_hard_error_bump_tc = 2,
	handle_tx_rx = 3,
};

/* GMAC TX FIFO is 8K, Rx FIFO is 16K */
#define BUF_SIZE_16KiB 16384
#define BUF_SIZE_8KiB 8192
#define BUF_SIZE_4KiB 4096
#define BUF_SIZE_2KiB 2048

/* Power Down and WOL */
#define PMT_NOT_SUPPORTED 0
#define PMT_SUPPORTED 1

/* Common MAC defines */
#define MAC_CTRL_REG		0x00000000	/* MAC Control */
#define MAC_ENABLE_TX		0x00000008	/* Transmitter Enable */
#define MAC_RNABLE_RX		0x00000004	/* Receiver Enable */

/* MAC Management Counters register */
#define MMC_CONTROL		0x00000100	/* MMC Control */
#define MMC_RX_INTR		0x00000104	/* MMC High Interrupt */
#define MMC_TX_INTR		0x00000108	/* MMC Low Interrupt */
#define MMC_RX_INTR_MASK	0x0000010c	/* MMC High Interrupt Mask */
#define MMC_TX_INTR_MASK	0x00000110	/* MMC Low Interrupt Mask */

#define MMC_CONTROL_MAX_FRM_MASK	0x0003ff8	/* Maximum Frame Size */
#define MMC_CONTROL_MAX_FRM_SHIFT	3
#define MMC_CONTROL_MAX_FRAME		0x7FF

struct tcc_gmac_desc_ops {
	/* DMA RX descriptor ring initialization */
	void (*init_rx_desc) (struct dma_desc *p, unsigned int ring_size,
			      int disable_rx_ic);
	/* DMA TX descriptor ring initialization */
	void (*init_tx_desc) (struct dma_desc *p, unsigned int ring_size);

	/* Invoked by the xmit function to prepare the tx descriptor */
	void (*prepare_tx_desc) (struct dma_desc *p, int is_fs, int len,
				 int csum_flag);
	/* Set/get the owner of the descriptor */
	void (*set_tx_owner) (struct dma_desc *p);
	int (*get_tx_owner) (struct dma_desc *p);
	/* Invoked by the xmit function to close the tx descriptor */
	void (*close_tx_desc) (struct dma_desc *p);
	/* Clean the tx descriptor as soon as the tx irq is received */
	void (*release_tx_desc) (struct dma_desc *p);
	/* Clear interrupt on tx frame completion. When this bit is
	 * set an interrupt happens as soon as the frame is transmitted */
	void (*clear_tx_ic) (struct dma_desc *p);
	/* Last tx segment reports the transmit status */
	int (*get_tx_ls) (struct dma_desc *p);
	/* Return the transmit status looking at the TDES1 */
	int (*tx_status) (void *data, struct tcc_gmac_extra_stats *x,
			  struct dma_desc *p, unsigned long ioaddr);
	/* Get the buffer size from the descriptor */
	int (*get_tx_len) (struct dma_desc *p);
	/* Handle extra events on specific interrupts hw dependent */
	int (*get_rx_owner) (struct dma_desc *p);
	void (*set_rx_owner) (struct dma_desc *p);
	/* Get the receive frame size */
	int (*get_rx_frame_len) (struct dma_desc *p);
	/* Return the reception status looking at the RDES1 */
	int (*rx_status) (void *data, struct tcc_gmac_extra_stats *x,
			  struct dma_desc *p);
};

struct tcc_gmac_dma_ops {
	/* DMA core initialization */
	int (*init) (unsigned long ioaddr, int pbl, u32 dma_tx, u32 dma_rx);
	/* Dump DMA registers */
	void (*dump_regs) (unsigned long ioaddr);
	/* Set tx/rx threshold in the csr6 register
	 * An invalid value enables the store-and-forward mode */
	void (*dma_mode) (unsigned long ioaddr, int txmode, int rxmode);
	/* To track extra statistic (if supported) */
	void (*dma_diagnostic_fr) (void *data, struct tcc_gmac_extra_stats *x,
				   unsigned long ioaddr);
	void (*enable_dma_transmission) (unsigned long ioaddr);
	void (*enable_dma_irq) (unsigned long ioaddr);
	void (*disable_dma_irq) (unsigned long ioaddr);
	void (*start_tx) (unsigned long ioaddr);
	void (*stop_tx) (unsigned long ioaddr);
	void (*start_rx) (unsigned long ioaddr);
	void (*stop_rx) (unsigned long ioaddr);
	int (*dma_interrupt) (unsigned long ioaddr,
			      struct tcc_gmac_extra_stats *x);
};

struct tcc_gmac_core_ops {
	/* MAC core initialization */
	void (*core_init) (unsigned long ioaddr) ____cacheline_aligned;
	/* Dump MAC registers */
	void (*dump_regs) (unsigned long ioaddr);
	/* Handle extra events on specific interrupts hw dependent */
	void (*host_irq_status) (unsigned long ioaddr);
	/* Multicast filter setting */
	void (*set_filter) (struct net_device *dev);
	/* Flow control setting */
	void (*flow_ctrl) (unsigned long ioaddr, unsigned int duplex,
			   unsigned int fc, unsigned int pause_time);
	/* Set power management mode (e.g. magic frame) */
	void (*pmt) (unsigned long ioaddr, unsigned long mode);
	/* Set/Get Unicast MAC addresses */
	void (*set_umac_addr) (unsigned long ioaddr, unsigned char *addr,
			       unsigned int reg_n);
	void (*get_umac_addr) (unsigned long ioaddr, unsigned char *addr,
			       unsigned int reg_n);
};

struct mac_link {
	int port;
	int duplex;
	int speed;
};

struct mii_regs {
	unsigned int addr;	/* MII Address */
	unsigned int data;	/* MII Data */
};

struct mac_device_info {
	struct tcc_gmac_core_ops	*mac;
	struct tcc_gmac_desc_ops	*desc;
	struct tcc_gmac_dma_ops	*dma;
	unsigned int pmt;	/* support Power-Down */
	struct mii_regs mii;	/* MII register Addresses */
	unsigned int clk_rate;
	struct mac_link link;
};

struct tcc_gmac_priv {
	/* Frequently used values are kept adjacent for cache effect */
	struct dma_desc *dma_tx ____cacheline_aligned;
	dma_addr_t dma_tx_phy;
	struct sk_buff **tx_skbuff;
	unsigned int cur_tx;
	unsigned int dirty_tx;
	unsigned int dma_tx_size;
	int tx_coe;
	int tx_coalesce;

	struct dma_desc *dma_rx ;
	unsigned int cur_rx;
	unsigned int dirty_rx;
	struct sk_buff **rx_skbuff;
	dma_addr_t *rx_skbuff_dma;
	struct sk_buff_head rx_recycle;

	struct net_device *dev;
//	int is_gmac;
	dma_addr_t dma_rx_phy;
	unsigned int dma_rx_size;
	int rx_csum;
	unsigned int dma_buf_sz;
	struct device *device;
	struct mac_device_info *hw;

	struct tcc_gmac_extra_stats xstats;
	struct napi_struct napi;

	int pbl;
//	int bus_id;
	int phy_addr;
	int phy_mask;
	int (*phy_reset) (void *priv);
//	void (*fix_mac_speed) (void *priv, unsigned int speed);
//	void (*bus_setup)(unsigned long ioaddr);
//	void *bsp_priv;

	int phy_irq;
	struct phy_device *phydev;
	int oldlink;
	int speed;
	int oldduplex;
	unsigned int flow_ctrl;
	unsigned int pause;
	struct mii_bus *mii;

	u32 msg_enable;
	spinlock_t lock;
	int wolopts;
	int wolenabled;
	int shutdown;
#ifdef TCC_GMAC_VLAN_TAG_USED
	struct vlan_group *vlgrp;
#endif
};

#endif /*_TCC_GMAC_DRV_H_*/

