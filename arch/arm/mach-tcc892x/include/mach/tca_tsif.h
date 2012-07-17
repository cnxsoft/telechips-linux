/****************************************************************************
 *   FileName    : tcc_tsif_hwset.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef __TCC_TSIF_MODULE_HWSET_H__
#define __TCC_TSIF_MODULE_HWSET_H__

#include <mach/memory.h>
#include <mach/irqs.h>
#define    SUPPORT_STB_TSIF_INTERFACE //Support TSIF interface at STB solution

#define    SUPPORT_PIDFILTER_INTERNAL    //filtering support using internal logic(TSPID reg)
//#define    SUPPORT_PIDFILTER_DMA       //filtering suuport using dma logic

//20111212 koo : GDMA가 not defined 상태면 tsif-dma using으로 변경 됨. 
//#define GDMA 

#define TSIF_CH 					0		//tsif use ch0,1,2

#define TSIF_DMA_CONTROLLER		1		//tsif use gdma controller 1
#define TSIF_DMA_CH				2		//tsif use gdma ch2-n (n:0,1,2)

#define GPIO_D_PORT_TSIFNUM		0
#define GPIO_B0_PORT_TSIFNUM	1
#define GPIO_B1_PORT_TSIFNUM	2
#define GPIO_C_PORT_TSIFNUM		3
#define GPIO_E_PORT_TSIFNUM		4
#define GPIO_F_PORT_TSIFNUM		5


#if defined(CONFIG_MACH_TCC8920ST)
#define TSIF_GPIO_PORT	GPIO_D_PORT_TSIFNUM
#elif defined(CONFIG_MACH_TCC8920) || defined(CONFIG_MACH_TCC8925)
#define TSIF_GPIO_PORT	GPIO_F_PORT_TSIFNUM
#else
#define TSIF_GPIO_PORT	GPIO_F_PORT_TSIFNUM
#endif


#define TSIF_BYTE_SIZE			4		// 1, 2, 4byte
#define TSIF_RXFIFO_THRESHOLD	1		// 0-7 (0=>8 depth)
#define GDMA_WORD_SIZE			4		// 1, 2, 4byte
#define GDMA_BURST_SIZE			4		// 1, 2, 4, 8 = (TSIF_BYTE_SIZE * TSIF_RXFIFO_THRESHOLD) / GDMA_WORD_SIZE

#define MPEG_PACKET_SIZE 		(188)
#if defined(CONFIG_MACH_TCC9300ST)
#define TSIF_DMA_SIZE 				(CONSISTENT_DMA_SIZE / 8)
#elif defined(CONFIG_MACH_TCC8800)
#define TSIF_DMA_SIZE 				(CONSISTENT_DMA_SIZE / 8)
#elif defined(CONFIG_MACH_TCC8800ST)
#define TSIF_DMA_SIZE 				(CONSISTENT_DMA_SIZE / 8)
#elif defined(CONFIG_MACH_TCC8920ST)
#define TSIF_DMA_SIZE 				(CONSISTENT_DMA_SIZE / 8)
#else
#define TSIF_DMA_SIZE 				(CONSISTENT_DMA_SIZE / 8)
#endif
#define TSIF_MASK_SIZE			((0x10000000 - TSIF_DMA_SIZE) >> 4)

#define TSIF_DMA_HOPE_CNT(cnt)  (((cnt) * MPEG_PACKET_SIZE) / (TSIF_BYTE_SIZE * TSIF_RXFIFO_THRESHOLD))
#define TSIF_DMA_INCREASE_SIZE	0x4

#ifdef GDMA
#ifdef SUPPORT_PIDFILTER_INTERNAL
#define TSIF_DMA_PACKET_CNT		4
#else
#define TSIF_DMA_PACKET_CNT		512
#endif
#else
#define TSIF_DMA_PACKET_CNT		1
#endif


#define WAIT_TIME_FOR_DMA_DONE	(1000 * 8)

#pragma pack(push, 4)
struct tcc_tsif_regs {
    volatile unsigned long TSDI, TSRXCR, TSPID[16], TSIC, TSIS, TSISP, TSTXC;
};
#pragma pack(pop)


struct tea_dma_buf {
    void *v_addr;
    unsigned int dma_addr;
    int buf_size; // total size of DMA
};

typedef struct tcc_tsif_handle tcc_tsif_handle_t;
typedef int (*dma_alloc_f)(struct tea_dma_buf *tdma, unsigned int size);
typedef void (*dma_free_f)(struct tea_dma_buf *tdma);

struct tcc_tsif_handle {
    volatile struct tcc_tsif_regs *regs;
    struct tea_dma_buf rx_dma;
	//int flag;
    int irq;
    void *private_data;
    int id;
	int dma_controller;
	int dma_ch;
	//int is_slave;
	int gpio_port;

    int (*dma_stop)(struct tcc_tsif_handle *h);
    int (*dma_start)(struct tcc_tsif_handle *h);
    void (*clear_fifo_packet)(struct tcc_tsif_handle *h);
	void (*tsif_set)(struct tcc_tsif_handle *h);
	void (*tsif_isr)(struct tcc_tsif_handle *h);
	void (*hw_init)(struct tcc_tsif_handle *h);
    int (*tsif_resync)(struct tcc_tsif_handle *h);
	
    dma_alloc_f tea_dma_alloc; // tea function.
    dma_free_f tea_dma_free; // tea function.

	//int clk;	// Mhz
	
    /* add for slave */
    unsigned int dma_total_packet_cnt, dma_intr_packet_cnt;
    int q_pos, cur_q_pos, prev_q_pos;
    int dma_total_size;

	int dma_phy_maxaddr;
	int dma_phy_rdpos;
	int dma_phy_curpos;
	int dma_virt_maxaddr;
	int dma_virt_rdpos;
	int dma_virt_curpos;
	int dma_recv_size;
	int dma_intr_cnt;
	
	unsigned char msb_first;
	unsigned char big_endian;
	unsigned char serial_mode;
	unsigned char clk_polarity;
	unsigned char valid_polarity;
	unsigned char sync_polarity;
	unsigned char sync_delay;
	unsigned int dma_phy_addr;
	unsigned int dma_phy_size;
	unsigned int mpeg_ts; //0:spi mode, 1(bit0):mpeg_ts format sigal, 2(bit1):mpeg_ts siganl, use pid filtering & ts sync
    unsigned int match_pids[32];
    unsigned int match_pids_count;
};

extern int tca_tsif_init(struct tcc_tsif_handle *h, volatile struct tcc_tsif_regs *regs, 
		dma_alloc_f tea_dma_alloc, dma_free_f tea_dma_free, int dma_size, int tsif_ch, int dma_controller, int dma_ch, int port);
extern void tca_tsif_clean(struct tcc_tsif_handle *h);

extern int tca_tsif_register_pids(struct tcc_tsif_handle *h, unsigned int *pids, unsigned int count);
extern int tca_tsif_can_support(void);
#endif /*__TCC_TSIF_MODULE_HWSET_H__*/
