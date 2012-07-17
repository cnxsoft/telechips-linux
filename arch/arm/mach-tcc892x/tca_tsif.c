/****************************************************************************
 *   FileName    :  tcc_tsif_hwset.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#include <linux/module.h>
#include <linux/delay.h>
#include <mach/io.h>
#include <linux/gpio.h>
#include <mach/bsp.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>

#include <asm/mach-types.h>
#include <mach/tca_tsif.h>
#if defined(SUPPORT_PIDFILTER_INTERNAL)
//#define     USE_DMA_SYNCMATCH
#define     UPDATE_ONLY_CHANGED_PID
#endif
#define     DEFAULT_PID     0x0  //0 is PAT PID, PAT will be always registered. to prevent disabling pid filter.
//#define DEBUG_INFO

const struct tcc_freq_table_t gtTSIFClockLimitTable = {
      468750,      0, 600000,      0, 140000,      0,      0, 100000,      0 // Core 1.30V
};

static void tsif_delay(int m_sec)
{
    mdelay(m_sec);
//  msleep(1);
}

static void tcc_tsif_clearfifopacket(struct tcc_tsif_handle *h)
{
    /* In order to clear fifo, we need to external clocks( tsif clock ).
     * Without clock, it will go hang situation. Therefore 
     * We remove clear fifo codes.
     */
#if 0    
#ifdef DEBUG_INFO
	printk("%s - in\n", __func__);
#endif    
    //don't use this clear fifo method
    //this clear only read side, write size doesn't be cleard
    //BITSET(h->regs->TSRXCR, Hw7);		//set to Hw7 : empties RX Fifo

    //this clear both read/write size fifo
    BITSET(h->regs->TSRXCR, Hw30);
	BITCLR(h->regs->TSRXCR, Hw30);
#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif
#endif    
}

static int tca_tsif_set_port(struct tcc_tsif_handle *h)
{
	volatile unsigned long* TSIFPORT = (volatile unsigned long *)tcc_p2v(HwTSIF_TSCHS_BASE);
	int ret = 0;
    printk("%s : select port => %d\n", __func__, h->gpio_port);

	switch (h->gpio_port) {
		case 0:
				tcc_gpio_config(TCC_GPD(8), GPIO_FN(2)|GPIO_SCHMITT_INPUT);		//clk
				tcc_gpio_config(TCC_GPD(9), GPIO_FN(2));		//valid
				tcc_gpio_config(TCC_GPD(10), GPIO_FN(2));		//sync
				tcc_gpio_config(TCC_GPD(7), GPIO_FN(2));		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPD(6), GPIO_FN(2));	//d1
					tcc_gpio_config(TCC_GPD(5), GPIO_FN(2));	//d2
					tcc_gpio_config(TCC_GPD(4), GPIO_FN(2));	//d3
					tcc_gpio_config(TCC_GPD(3), GPIO_FN(2));	//d4
					tcc_gpio_config(TCC_GPD(2), GPIO_FN(2));	//d5
					tcc_gpio_config(TCC_GPD(1), GPIO_FN(2));	//d6
					tcc_gpio_config(TCC_GPD(0), GPIO_FN(2));	//d7
				}
			break;
		case 1:
				tcc_gpio_config(TCC_GPB(0), GPIO_FN(7)|GPIO_SCHMITT_INPUT);		//clk
				tcc_gpio_config(TCC_GPB(2), GPIO_FN(7));		//valid
				tcc_gpio_config(TCC_GPB(1), GPIO_FN(7));		//sync
				tcc_gpio_config(TCC_GPB(3), GPIO_FN(7));		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPB(4), GPIO_FN(7));	//d1
					tcc_gpio_config(TCC_GPB(5), GPIO_FN(7));	//d2
					tcc_gpio_config(TCC_GPB(6), GPIO_FN(7));	//d3
					tcc_gpio_config(TCC_GPB(7), GPIO_FN(7));	//d4
					tcc_gpio_config(TCC_GPB(8), GPIO_FN(7));	//d5
					tcc_gpio_config(TCC_GPB(9), GPIO_FN(7));	//d6
					tcc_gpio_config(TCC_GPB(10), GPIO_FN(7));	//d7
				}
			break;
		case 2:
				tcc_gpio_config(TCC_GPB(28), GPIO_FN(7)|GPIO_SCHMITT_INPUT);		//clk
				tcc_gpio_config(TCC_GPB(26), GPIO_FN(7));		//valid
				tcc_gpio_config(TCC_GPB(27), GPIO_FN(7));		//sync
				tcc_gpio_config(TCC_GPB(25), GPIO_FN(7));		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPB(24), GPIO_FN(7));	//d1
					tcc_gpio_config(TCC_GPB(23), GPIO_FN(7));	//d2
					tcc_gpio_config(TCC_GPB(22), GPIO_FN(7));	//d3
					tcc_gpio_config(TCC_GPB(21), GPIO_FN(7));	//d4
					tcc_gpio_config(TCC_GPB(20), GPIO_FN(7));	//d5
					tcc_gpio_config(TCC_GPB(19), GPIO_FN(7));	//d6
					tcc_gpio_config(TCC_GPB(18), GPIO_FN(7));	//d7
				}
			break;
		case 3:
				tcc_gpio_config(TCC_GPC(26), GPIO_FN(3)|GPIO_SCHMITT_INPUT);		//clk
				tcc_gpio_config(TCC_GPC(24), GPIO_FN(3));		//valid
				tcc_gpio_config(TCC_GPC(25), GPIO_FN(3));		//sync
				tcc_gpio_config(TCC_GPC(23), GPIO_FN(3));		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPC(22), GPIO_FN(3));	//d1
					tcc_gpio_config(TCC_GPC(21), GPIO_FN(3));	//d2
					tcc_gpio_config(TCC_GPC(20), GPIO_FN(3));	//d3
					tcc_gpio_config(TCC_GPC(19), GPIO_FN(3));	//d4
					tcc_gpio_config(TCC_GPC(18), GPIO_FN(3));	//d5
					tcc_gpio_config(TCC_GPC(17), GPIO_FN(3));	//d6
					tcc_gpio_config(TCC_GPC(16), GPIO_FN(3));	//d7
				}
			break;
		case 4:
				tcc_gpio_config(TCC_GPE(26), GPIO_FN(4)|GPIO_SCHMITT_INPUT);		//clk
				tcc_gpio_config(TCC_GPE(24), GPIO_FN(4));		//valid
				tcc_gpio_config(TCC_GPE(25), GPIO_FN(4));		//sync
				tcc_gpio_config(TCC_GPE(23), GPIO_FN(4));		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPE(22), GPIO_FN(4));	//d1
					tcc_gpio_config(TCC_GPE(21), GPIO_FN(4));	//d2
					tcc_gpio_config(TCC_GPE(20), GPIO_FN(4));	//d3
					tcc_gpio_config(TCC_GPE(19), GPIO_FN(4));	//d4
					tcc_gpio_config(TCC_GPE(18), GPIO_FN(4));	//d5
					tcc_gpio_config(TCC_GPE(17), GPIO_FN(4));	//d6
					tcc_gpio_config(TCC_GPE(16), GPIO_FN(4));	//d7
				}
			break;
		case 5:
				tcc_gpio_config(TCC_GPF(0), GPIO_FN(2)|GPIO_SCHMITT_INPUT);		//clk
				tcc_gpio_config(TCC_GPF(2), GPIO_FN(2));		//valid
				tcc_gpio_config(TCC_GPF(1), GPIO_FN(2));		//sync
				tcc_gpio_config(TCC_GPF(3), GPIO_FN(2));		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPF(4), GPIO_FN(2));	//d1
					tcc_gpio_config(TCC_GPF(5), GPIO_FN(2));	//d2
					tcc_gpio_config(TCC_GPF(6), GPIO_FN(2));	//d3
					tcc_gpio_config(TCC_GPF(7), GPIO_FN(2));	//d4
					tcc_gpio_config(TCC_GPF(8), GPIO_FN(2));	//d5
					tcc_gpio_config(TCC_GPF(9), GPIO_FN(2));	//d6
					tcc_gpio_config(TCC_GPF(10), GPIO_FN(2));	//d7
				}
			break;		
		default:
				printk("%s : select wrong port => %d\n", __func__, h->gpio_port);
				ret = -1;
			break;
	}

	if(ret != -1) {
		unsigned int direction = 1 << (24 + h->gpio_port);
		unsigned int channel = (h->id & 0x3) << (12 + (2 * h->gpio_port));
		unsigned int port = (h->gpio_port & 0xf) << (4 * h->id);
		unsigned int clr = (0x3 << (12 + (2 * h->gpio_port))) | (0xf << (4 * h->id));

		BITCLR(*TSIFPORT, 0xffffffff);
		BITCLR(*TSIFPORT, direction);			//clk, sync, valid, data input direction select
		BITCSET(*TSIFPORT, clr, (channel | port));	//tsif channel & port number select
	}

	return ret;
}

static void tca_clear_pid_tables(struct tcc_tsif_handle *h)
{
    int i;
#if defined(SUPPORT_PIDFILTER_DMA)
    volatile unsigned long* PIDT;
    for (i = 0; i < 32; i++) {
        PIDT = (volatile unsigned long *)tcc_p2v(HwTSIF_PIDT(i));
        *PIDT = 0;
    }
#endif        
#ifdef      SUPPORT_PIDFILTER_INTERNAL
    for (i = 0; i < 16; i++) {            
        BITCLR(h->regs->TSPID[i], (Hw32-Hw0));
    }
//    printk("%s\n", __func__);
#endif
}

static void tcc_tsif_release_port(struct tcc_tsif_handle *h)
{
	volatile unsigned long* TSIFPORT = (volatile unsigned long *)tcc_p2v(HwTSIF_TSCHS_BASE);

	switch (h->gpio_port) {
		case 0:
				tcc_gpio_config(TCC_GPD(8), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//clk
				tcc_gpio_config(TCC_GPD(9), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//valid
				tcc_gpio_config(TCC_GPD(10), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//sync
				tcc_gpio_config(TCC_GPD(0), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPD(1), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d1
					tcc_gpio_config(TCC_GPD(2), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d2
					tcc_gpio_config(TCC_GPD(3), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d3
					tcc_gpio_config(TCC_GPD(4), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d4
					tcc_gpio_config(TCC_GPD(5), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d5
					tcc_gpio_config(TCC_GPD(6), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d6
					tcc_gpio_config(TCC_GPD(7), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d7
				}
			break;
		case 1:
				tcc_gpio_config(TCC_GPB(0), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//clk
				tcc_gpio_config(TCC_GPB(2), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//valid
				tcc_gpio_config(TCC_GPB(1), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//sync
				tcc_gpio_config(TCC_GPB(3), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPB(4), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d1
					tcc_gpio_config(TCC_GPB(5), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d2
					tcc_gpio_config(TCC_GPB(6), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d3
					tcc_gpio_config(TCC_GPB(7), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d4
					tcc_gpio_config(TCC_GPB(8), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d5
					tcc_gpio_config(TCC_GPB(9), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d6
					tcc_gpio_config(TCC_GPB(10), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d7
				}
			break;
		case 2:
				tcc_gpio_config(TCC_GPB(28), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//clk
				tcc_gpio_config(TCC_GPB(26), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//valid
				tcc_gpio_config(TCC_GPB(27), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//sync
				tcc_gpio_config(TCC_GPB(25), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPB(24), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d1
					tcc_gpio_config(TCC_GPB(23), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d2
					tcc_gpio_config(TCC_GPB(22), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d3
					tcc_gpio_config(TCC_GPB(21), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d4
					tcc_gpio_config(TCC_GPB(20), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d5
					tcc_gpio_config(TCC_GPB(19), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d6
					tcc_gpio_config(TCC_GPB(18), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d7
				}
			break;
		case 3:
				tcc_gpio_config(TCC_GPC(26), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//clk
				tcc_gpio_config(TCC_GPC(24), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//valid
				tcc_gpio_config(TCC_GPC(25), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//sync
				tcc_gpio_config(TCC_GPC(23), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPC(22), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d1
					tcc_gpio_config(TCC_GPC(21), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d2
					tcc_gpio_config(TCC_GPC(20), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d3
					tcc_gpio_config(TCC_GPC(19), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d4
					tcc_gpio_config(TCC_GPC(18), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d5
					tcc_gpio_config(TCC_GPC(17), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d6
					tcc_gpio_config(TCC_GPC(16), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d7
				}
			break;
		case 4:
				tcc_gpio_config(TCC_GPE(26), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//clk
				tcc_gpio_config(TCC_GPE(24), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//valid
				tcc_gpio_config(TCC_GPE(25), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//sync
				tcc_gpio_config(TCC_GPE(23), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPE(22), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d1
					tcc_gpio_config(TCC_GPE(21), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d2
					tcc_gpio_config(TCC_GPE(20), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d3
					tcc_gpio_config(TCC_GPE(19), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d4
					tcc_gpio_config(TCC_GPE(18), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d5
					tcc_gpio_config(TCC_GPE(17), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d6
					tcc_gpio_config(TCC_GPE(16), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);	//d7
				}
			break;
		case 5:
				tcc_gpio_config(TCC_GPF(0), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//clk
				tcc_gpio_config(TCC_GPF(2), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//valid
				tcc_gpio_config(TCC_GPF(1), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//sync
				tcc_gpio_config(TCC_GPF(3), GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);		//d0

				if(!(h->serial_mode)) {
					tcc_gpio_config(TCC_GPF(4), GPIO_FN(2)|GPIO_OUTPUT|GPIO_LOW);	//d1
					tcc_gpio_config(TCC_GPF(5), GPIO_FN(2)|GPIO_OUTPUT|GPIO_LOW);	//d2
					tcc_gpio_config(TCC_GPF(6), GPIO_FN(2)|GPIO_OUTPUT|GPIO_LOW);	//d3
					tcc_gpio_config(TCC_GPF(7), GPIO_FN(2)|GPIO_OUTPUT|GPIO_LOW);	//d4
					tcc_gpio_config(TCC_GPF(8), GPIO_FN(2)|GPIO_OUTPUT|GPIO_LOW);	//d5
					tcc_gpio_config(TCC_GPF(9), GPIO_FN(2)|GPIO_OUTPUT|GPIO_LOW);	//d6
					tcc_gpio_config(TCC_GPF(10), GPIO_FN(2)|GPIO_OUTPUT|GPIO_LOW);	//d7
				}
			break;		
		default:
				printk("%s : select wrong port => %d\n", __func__, h->gpio_port);
			break;
	}

	{
		unsigned int clr = (0x3 << (12 + (2 * h->gpio_port))) | (0xf << (4 * h->id));

		BITCLR(*TSIFPORT, clr);
	}
}


static void tcc_tsif_set(struct tcc_tsif_handle *h)
{
#ifdef DEBUG_INFO
	printk("%s - in\n", __func__);
#endif
	//BITSET(h->regs->TSRXCR, Hw17);
	if(h->msb_first)		BITSET(h->regs->TSRXCR, Hw16);
	else					BITCLR(h->regs->TSRXCR, Hw16);

	if(h->big_endian)		BITSET(h->regs->TSRXCR, Hw15);
	else					BITCLR(h->regs->TSRXCR, Hw15);
		
	if(h->serial_mode)		BITSET(h->regs->TSRXCR, Hw14);
	else					BITCLR(h->regs->TSRXCR, Hw14);

	if(h->clk_polarity)		BITSET(h->regs->TSRXCR, Hw13);
	else					BITCLR(h->regs->TSRXCR, Hw13);

	if(h->valid_polarity)	BITSET(h->regs->TSRXCR, Hw12);
	else					BITCLR(h->regs->TSRXCR, Hw12);

	if(h->sync_polarity)	BITSET(h->regs->TSRXCR, Hw11);
	else					BITCLR(h->regs->TSRXCR, Hw11);

	if(h->sync_delay)		BITSET(h->regs->TSRXCR, Hw5);
	else					BITCLR(h->regs->TSRXCR, Hw5);
#ifdef DEBUG_INFO
	printk("%s : mode set : %s, %s, %s, %s, %s, %s, sync delay %d port 0x%X\n", __func__, 
																		(h->msb_first		? "msb first" 		: "lsb first"),
																		(h->big_endian 		? "big endian" 		: "little endian"),
																		(h->serial_mode		? "serial mode" 	: "parallel mode"),
																		(h->clk_polarity	? "falling edge" 	: "rising edge"),
																		(h->valid_polarity 	? "valid high active": "valid low active"),
																		(h->sync_polarity	? "sync high active" : "sync low active"),
																		 h->sync_delay, h->gpio_port );
#endif
}

#ifdef GDMA
static void tcc_tsif_isr(struct tcc_tsif_handle *h)
{
	volatile PGDMACTRL dma_regs;

	if(h->dma_controller == 0)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
	else if(h->dma_controller == 1)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE);
	else if(h->dma_controller == 2)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA2_BASE);

	if(h->dma_ch == 0)
	{
		if(dma_regs->CHCTRL0.nREG & Hw3) {
			BITSET(dma_regs->CHCTRL0.nREG, Hw3);		//dma done flag set
			h->dma_phy_curpos = dma_regs->C_DADR0.nREG;
		}
	}
	else if(h->dma_ch == 1)
	{
		if(dma_regs->CHCTRL1.nREG & Hw3) {
			BITSET(dma_regs->CHCTRL1.nREG, Hw3);		//dma done flag set
			h->dma_phy_curpos = dma_regs->C_DADR1.nREG;
		}
	}
	else
	{
		if(dma_regs->CHCTRL2.nREG & Hw3) {
			BITSET(dma_regs->CHCTRL2.nREG, Hw3);		//dma done flag set
			h->dma_phy_curpos = dma_regs->C_DADR2.nREG;
		}
	}

	h->dma_intr_cnt++;
}

static void tcc_tsif_dma_init(struct tcc_tsif_handle *h)
{
	volatile PGDMACTRL dma_regs;
	unsigned int mask_addr;
	unsigned int tsif_tsdi_addr;
	unsigned int reg_val = 0;

	if(h->dma_controller == 0)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
	else if(h->dma_controller == 1)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE);
	else if(h->dma_controller == 2)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA2_BASE);
	
#ifdef DEBUG_INFO	
	printk("[%s]: dma set dest addr 0x%08x:%dbyte\n", __func__, h->rx_dma.dma_addr, h->rx_dma.buf_size);
#endif

	if(h->dma_ch == 0) {
		tsif_tsdi_addr = (int)&(h->regs->TSDI);
		tsif_tsdi_addr -= IO_OFFSET;
		BITCSET(dma_regs->ST_SADR0.nREG, (Hw32-Hw0), tsif_tsdi_addr);		//source addr set
		BITCLR(dma_regs->SPARAM0.nREG, (Hw32-Hw0));

		BITCSET(dma_regs->ST_DADR0.nREG, (Hw32-Hw0), h->rx_dma.dma_addr);	//destination addr set
		mask_addr = TSIF_MASK_SIZE << 8;								// 1mbyte mask set
		BITCSET(dma_regs->DPARAM0.nREG, (Hw32-Hw0), (mask_addr|TSIF_DMA_INCREASE_SIZE));		//inc & mask addr set

		BITCSET(dma_regs->HCOUNT0.nREG, (Hw32-Hw0), TSIF_DMA_HOPE_CNT(h->dma_intr_packet_cnt));		//hop count set
		BITCLR(dma_regs->CHCTRL0.nREG, (Hw32-Hw0));							//repeat ctrl reg set

		if(h->id == 0)			BITCSET(dma_regs->EXTREQ0.nREG, (Hw32-Hw0), Hw3);		//ext request set tsif0
		else if(h->id == 1)		BITCSET(dma_regs->EXTREQ0.nREG, (Hw32-Hw0), Hw7);		//ext request set tsif1
		else					BITCSET(dma_regs->EXTREQ0.nREG, (Hw32-Hw0), Hw31);	//ext request set tsif2

		//20110530 koo : transfer type setting => edge or level
		//reg_val = Hw15|Hw11|Hw9|Hw8|Hw5|Hw4|Hw2|Hw1;
		reg_val = Hw15|Hw11|Hw5|Hw4|Hw2|Hw1;
		
		if(GDMA_WORD_SIZE == 4)		reg_val |= Hw5 | Hw4;
		else if(GDMA_WORD_SIZE == 2)	reg_val |= Hw4;
		if(GDMA_BURST_SIZE == 8)		reg_val |= Hw7 | Hw6;
		else if(GDMA_BURST_SIZE == 4)	reg_val |= Hw7;
		else if(GDMA_BURST_SIZE == 2)	reg_val |= Hw6;
		BITCSET(dma_regs->CHCTRL0.nREG, (Hw32-Hw0), reg_val);		//channel ctrl reg set
	} else if(h->dma_ch == 1) {
		tsif_tsdi_addr = (int)&(h->regs->TSDI);
		tsif_tsdi_addr -= IO_OFFSET;
		BITCSET(dma_regs->ST_SADR1.nREG, (Hw32-Hw0), tsif_tsdi_addr);		//source addr set
		BITCLR(dma_regs->SPARAM1.nREG, (Hw32-Hw0));

		BITCSET(dma_regs->ST_DADR1.nREG, (Hw32-Hw0), h->rx_dma.dma_addr);	//destination addr set
		mask_addr = TSIF_MASK_SIZE << 8;						    	// 1mbyte mask set
		BITCSET(dma_regs->DPARAM1.nREG, (Hw32-Hw0), (mask_addr|TSIF_DMA_INCREASE_SIZE));		//inc & mask addr set

		BITCSET(dma_regs->HCOUNT1.nREG, (Hw32-Hw0), TSIF_DMA_HOPE_CNT(h->dma_intr_packet_cnt));		//hop count set
		BITCLR(dma_regs->CHCTRL1.nREG, (Hw32-Hw0));							//repeat ctrl reg set

		if(h->id == 0)			BITCSET(dma_regs->EXTREQ1.nREG, (Hw32-Hw0), Hw3);		//ext request set tsif0
		else if(h->id == 1)		BITCSET(dma_regs->EXTREQ1.nREG, (Hw32-Hw0), Hw7);		//ext request set tsif1
		else					BITCSET(dma_regs->EXTREQ1.nREG, (Hw32-Hw0), Hw31);	//ext request set tsif2

		//20110530 koo : transfer type setting => edge or level
		//reg_val = Hw15|Hw11|Hw9|Hw8|Hw5|Hw4|Hw2|Hw1;
		reg_val = Hw15|Hw11|Hw5|Hw4|Hw2|Hw1;

		if(GDMA_WORD_SIZE == 4)		reg_val |= Hw5 | Hw4;
		else if(GDMA_WORD_SIZE == 2)	reg_val |= Hw4;
		if(GDMA_BURST_SIZE == 8)		reg_val |= Hw7 | Hw6;
		else if(GDMA_BURST_SIZE == 4)	reg_val |= Hw7;
		else if(GDMA_BURST_SIZE == 2)	reg_val |= Hw6;
		BITCSET(dma_regs->CHCTRL1.nREG, (Hw32-Hw0), reg_val);			//channel ctrl reg set
	} else { 
		tsif_tsdi_addr = (int)&(h->regs->TSDI);
		tsif_tsdi_addr -= IO_OFFSET;
		BITCSET(dma_regs->ST_SADR2.nREG, (Hw32-Hw0), tsif_tsdi_addr);		//source addr set
		BITCLR(dma_regs->SPARAM2.nREG, (Hw32-Hw0));

		BITCSET(dma_regs->ST_DADR2.nREG, (Hw32-Hw0), h->rx_dma.dma_addr);	//destination addr set
		mask_addr = TSIF_MASK_SIZE << 8;								// 1mbyte mask set
		BITCSET(dma_regs->DPARAM2.nREG, (Hw32-Hw0), (mask_addr|TSIF_DMA_INCREASE_SIZE));		//inc & mask addr set

		BITCSET(dma_regs->HCOUNT2.nREG, (Hw32-Hw0), TSIF_DMA_HOPE_CNT(h->dma_intr_packet_cnt));		//hop count set
		BITCLR(dma_regs->CHCTRL2.nREG, (Hw32-Hw0));							//repeat ctrl reg set

		if(h->id == 0)			BITCSET(dma_regs->EXTREQ2.nREG, (Hw32-Hw0), Hw3);		//ext request set tsif0
		else if(h->id == 1)		BITCSET(dma_regs->EXTREQ2.nREG, (Hw32-Hw0), Hw7);		//ext request set tsif1
		else					BITCSET(dma_regs->EXTREQ2.nREG, (Hw32-Hw0), Hw31);	//ext request set tsif2

		//20110530 koo : transfer type setting => edge or level
		//reg_val = Hw15|Hw11|Hw9|Hw8|Hw5|Hw4|Hw2|Hw1;
		reg_val = Hw15|Hw11|Hw5|Hw4|Hw2|Hw1;
		
		if(GDMA_WORD_SIZE == 4)		reg_val |= Hw5 | Hw4;
		else if(GDMA_WORD_SIZE == 2)	reg_val |= Hw4;
		if(GDMA_BURST_SIZE == 8)		reg_val |= Hw7 | Hw6;
		else if(GDMA_BURST_SIZE == 4)	reg_val |= Hw7;
		else if(GDMA_BURST_SIZE == 2)	reg_val |= Hw6;
		BITCSET(dma_regs->CHCTRL2.nREG, (Hw32-Hw0), reg_val);			//channel ctrl reg set
	}
}

static int tcc_tsif_dmastart(struct tcc_tsif_handle *h)
{
	volatile PGDMACTRL dma_regs;

	if(h->dma_controller == 0)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
	else if(h->dma_controller == 1)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE);
	else if(h->dma_controller == 2)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA2_BASE);
	
	h->dma_phy_rdpos 	= h->rx_dma.dma_addr;
	h->dma_phy_curpos 	= h->dma_phy_rdpos;
	h->dma_phy_maxaddr	= h->dma_phy_curpos + h->dma_total_size;
	h->dma_virt_rdpos 	= (unsigned int)h->rx_dma.v_addr;
	h->dma_virt_curpos 	= h->dma_virt_rdpos;
	h->dma_virt_maxaddr	= h->dma_virt_curpos + h->dma_total_size;
	h->dma_recv_size	= 0;
	h->dma_intr_cnt		= 0;

	tcc_tsif_dma_init(h);

	if(h->dma_ch == 0)	 		BITSET(dma_regs->CHCTRL0.nREG, 	Hw0);	//dma enable
	else if(h->dma_ch == 1)	BITSET(dma_regs->CHCTRL1.nREG, 	Hw0);	//dma enable
	else						BITSET(dma_regs->CHCTRL2.nREG, 	Hw0);	//dma enable
		
	BITSET(h->regs->TSRXCR, Hw31);								//tsif enable

#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif

	return 0;
}

static int tcc_tsif_dmastop(struct tcc_tsif_handle *h)
{
	volatile PGDMACTRL dma_regs;

	if(h->dma_controller == 0)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
	else if(h->dma_controller == 1)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE);
	else if(h->dma_controller == 2)
		dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA2_BASE);

	if(h->dma_ch == 0)			BITCLR(dma_regs->CHCTRL0.nREG, 	Hw0);	//dma disable
	else if(h->dma_ch == 1)	BITCLR(dma_regs->CHCTRL1.nREG, 	Hw0);	//dma disable
	else						BITCLR(dma_regs->CHCTRL2.nREG, 	Hw0);	//dma disable

	BITCLR(h->regs->TSRXCR, Hw31);								//tsif disable

#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif

	return 0;
}

static void tcc_tsif_dma_deinit(struct tcc_tsif_handle *h)
{
	//volatile PGDMACTRL dma_regs;

	//dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
}
#else //GDMA
static void tcc_tsif_isr(struct tcc_tsif_handle *h)
{
	volatile PTSIFDMA dma_regs;
	unsigned long dma_done_reg = 0;

	if(h->id == 0)			dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA0_BASE);
	else if(h->id == 1)		dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA1_BASE);
	else					dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA2_BASE);

	dma_done_reg = dma_regs->DMAICR.nREG;
	if(dma_done_reg & (Hw29|Hw28)) {
		BITSET(dma_regs->DMAICR.nREG, (Hw29|Hw28));
		h->cur_q_pos = (int)(dma_regs->DMASTR.nREG >> 17);
	}
	return;
}

static void tcc_tsif_dma_init(struct tcc_tsif_handle *h)
{
	volatile PTSIFDMA dma_regs;
	
	if(h->id == 0)			dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA0_BASE);
	else if(h->id == 1)		dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA1_BASE);
	else					dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA2_BASE);

	//BITCSET(dma_regs->TXBASE, (Hw32-Hw0), h->regs->TSDI);
	BITCSET(dma_regs->RXBASE.nREG, (Hw32-Hw0), h->rx_dma.dma_addr);

	//BITCSET(dma_regs->PACKET.nREG, (Hw32-Hw0), MPEG_PACKET_SIZE);
	//BITSET(dma_regs->PACKET.nREG, ((h->dma_total_size/MPEG_PACKET_SIZE)&0x1FFF)<<16);
	BITCSET(dma_regs->RXBASE.nREG, (Hw32-Hw0), h->rx_dma.dma_addr);
   	BITCLR(dma_regs->DMAICR.nREG, Hw20); //set RX interrupt
	BITCLR(dma_regs->DMACTR.nREG, Hw0);
	BITCLR(dma_regs->DMACTR.nREG, Hw28); //0:little endian, 1:Big endian
	BITSET(dma_regs->DMACTR.nREG, Hw29); //enable continues
	BITSET(dma_regs->DMACTR.nREG, Hw30); //enable recv DMA requeset 
	BITSET(dma_regs->DMACTR.nREG, Hw1);	 //enable TSSEL

}

static int tcc_tsif_dmastart(struct tcc_tsif_handle *h)
{
	volatile PTSIFDMA dma_regs;

	unsigned int packet_cnt = (h->dma_total_packet_cnt & 0x1FFF) - 1;
	unsigned int packet_size = (MPEG_PACKET_SIZE & 0x1FFF);
	unsigned int intr_packet_cnt = (h->dma_intr_packet_cnt & 0x1FFF) - 1;
   if(h->regs->TSRXCR & Hw31)
   {
        return 1;    
   }
#ifdef DEBUG_INFO
	printk("%s - in\n", __func__);
#endif
	if(h->id == 0)			dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA0_BASE);
	else if(h->id == 1)		dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA1_BASE);
	else					dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA2_BASE);

	BITSET(dma_regs->DMAICR.nREG, Hw29|Hw28);
	BITCLR(dma_regs->DMACTR.nREG, Hw0);

	dma_regs->PACKET.nREG = ( packet_cnt << 16) | packet_size;
	BITCSET(dma_regs->DMAICR.nREG, 0x1FFF, intr_packet_cnt);	

	BITSET(dma_regs->DMAICR.nREG, Hw16);
	//BITSET(dma_regs->DMAICR.nREG, Hw17);				//enable DMA Done Interrupt

	BITSET(dma_regs->DMACTR.nREG, Hw2);				//clear tx/rx packet counter
    BITCLR(dma_regs->DMACTR.nREG, Hw19|Hw18);//disable PID & Sync Byte match
    BITCLR(dma_regs->DMACTR.nREG, Hw5|Hw4);	//00:normal mode, 01:MPEG2_TS mode
#if defined(SUPPORT_PIDFILTER_DMA)
    if(h->mpeg_ts == Hw0)
    {
   	    BITCSET(dma_regs->DMACTR.nREG, Hw5|Hw4, Hw4);	//00:normal mode, 01:MPEG2_TS mode
    }
    else if(h->mpeg_ts == (Hw0|Hw1))
    {
       	BITSET(dma_regs->DMACTR.nREG, Hw19|Hw18);		//enable PID & Sync Byte match
   	    BITCSET(dma_regs->DMACTR.nREG, Hw5|Hw4, Hw4);	//00:normal mode, 01:MPEG2_TS mode
    }
#endif
    if( (h->mpeg_ts == 0) || (h->mpeg_ts == Hw0))
    {   	
      	BITCLR(h->regs->TSRXCR, Hw17);      	
    }
    else if(h->mpeg_ts == (Hw0|Hw1))
    { 
#if defined(USE_DMA_SYNCMATCH)        
       	BITCSET(dma_regs->DMACTR.nREG, Hw19|Hw18, Hw18);		//Sync Byte match
        BITCSET(dma_regs->DMACTR.nREG, Hw5|Hw4, Hw4);	//00:normal mode, 01:MPEG2_TS mode
#endif        
#if defined(SUPPORT_PIDFILTER_INTERNAL)
        BITCSET(h->regs->TSPID[0], (Hw16-Hw0), Hw13);    //to prevent disabling pid filter        
      	BITSET(h->regs->TSRXCR, Hw17);      	
#endif
    }
	
	BITSET(dma_regs->DMACTR.nREG, Hw0);				//enable DMA
	BITSET(h->regs->TSRXCR, Hw31);
    if( h->mpeg_ts == 0)
    {
        BITSET(h->regs->TSRXCR, Hw6);
    }
#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif
	return 0;
}

static int tcc_tsif_dmastop(struct tcc_tsif_handle *h)
{
	volatile PTSIFDMA dma_regs;
    if(!(h->regs->TSRXCR & Hw31))
        return 1;    

#ifdef DEBUG_INFO
	printk("%s - in\n", __func__);
#endif
	if(h->id == 0)			dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA0_BASE);
	else if(h->id == 1)		dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA1_BASE);
	else					dma_regs = (volatile PTSIFDMA)tcc_p2v(HwTSIF_DMA2_BASE);
	BITCLR(h->regs->TSRXCR, Hw31);
    BITCLR(dma_regs->DMAICR.nREG, Hw16);
    BITCLR(dma_regs->DMAICR.nREG, Hw17);				//enable DMA Done Interrupt

//	BITSET(dma_regs->DMAICR.nREG, Hw29|Hw28);
#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif
	return 0;
}

static void tcc_tsif_dma_deinit(struct tcc_tsif_handle *h)
{
}
#endif //GDMA

static void tcc_tsif_hw_init(struct tcc_tsif_handle *h)
{
	unsigned int reg_val;
#ifdef DEBUG_INFO
	printk("%s - in\n", __func__);
#endif
	tca_tsif_set_port(h);
 
	if(TSIF_BYTE_SIZE == 4)		reg_val = Hw17 | Hw9 | Hw8 | Hw7;
	else if(TSIF_BYTE_SIZE == 2)	reg_val = Hw17 | Hw8 | Hw7;
	else							reg_val = Hw17 | Hw7;
	BITCSET(h->regs->TSRXCR, 	(Hw32-Hw0), reg_val);
	
	BITCLR(h->regs->TSRXCR, Hw17);
	reg_val = (TSIF_RXFIFO_THRESHOLD << 5) | Hw4;
	BITCSET(h->regs->TSIC,(Hw32-Hw0), reg_val);

#if defined(SUPPORT_PIDFILTER_INTERNAL)
    BITSET(h->regs->TSRXCR, Hw17);
#endif	
#if defined(SUPPORT_PIDFILTER_DMA) || defined(USE_DMA_SYNCMATCH)
    //DMA Sync Delay Options
    //25: enable sync delay, 20-24 : Delay Value
    BITSET(h->regs->TSRXCR,Hw25|Hw24|Hw23|Hw21);
#endif

	tcc_tsif_dma_init(h);
	
#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif
}

static void tcc_tsif_hw_deinit(struct tcc_tsif_handle *h)
{
#ifdef DEBUG_INFO
	printk("%s - in\n", __func__);
#endif
	tcc_tsif_release_port(h);

	BITCLR(h->regs->TSRXCR, 	(Hw32-Hw0));
	BITCLR(h->regs->TSIC,	(Hw32-Hw0));

	tcc_tsif_dma_deinit(h);

#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif
}

static int tcc_tsif_resync(struct tcc_tsif_handle *h)
{
#if 0
    if(h->regs->TSIS & Hw1)
    {
        printk("%s:%d\n",__func__,__LINE__);
    	BITCLR(h->regs->TSRXCR, Hw31);
	    BITSET(h->regs->TSRXCR, Hw31);
    }
#endif    
    return 0;
}

/******************************
 * return value
 *
 * ret == 0: success
 * ret > 0 or ret < 0: fail
 ******************************/
int tca_tsif_init(struct tcc_tsif_handle *h,
					volatile struct tcc_tsif_regs *regs,
					dma_alloc_f tea_dma_alloc,
					dma_free_f tea_dma_free,
					int dma_size,
					int tsif_ch,
					int dma_controller,
					int dma_ch,
                    int port)
{
	int ret = 1;
	volatile PPIC pic_regs = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	
	if(regs)
	{   
		//h->regs = (volatile struct tcc_tsif_regs *)tcc_p2v((regs));
		h->regs = regs;

		h->dma_controller	= dma_controller;
		h->id 				= tsif_ch;
		h->dma_ch			= dma_ch;
#ifdef GDMA
		h->irq 				= (INT_DMA_BASE + 3*dma_controller + dma_ch); //INT_DMA
#else //GDMA
		h->irq				= INT_TSIF_BASE + tsif_ch;
#endif //GDMA
		h->gpio_port 		= port;

		h->dma_stop 		= tcc_tsif_dmastop;
		h->dma_start 		= tcc_tsif_dmastart;
		h->clear_fifo_packet= tcc_tsif_clearfifopacket;
		h->tsif_set			= tcc_tsif_set;
		h->tsif_isr			= tcc_tsif_isr;
    	h->hw_init          = tcc_tsif_hw_init;
    	h->tsif_resync      = tcc_tsif_resync;

		h->tea_dma_alloc 	= tea_dma_alloc;
		h->tea_dma_free 	= tea_dma_free;
		h->dma_total_size 	= dma_size;

		if(h->tea_dma_alloc) {
			if(h->tea_dma_alloc(&(h->rx_dma), h->dma_total_size) == 0) {
				ret = 0;
			}
		} else {
			/* Already, tsif has rx_dma buf */
			ret = 0;
		}

        /* interrupt init */
		BITSET(pic_regs->MODE1.nREG, (INT_TSIF-32));	// level-trigger
		BITCLR(pic_regs->POL1.nREG, (INT_TSIF-32));	// active-high            
        tcc_cpufreq_set_limit_table(&gtTSIFClockLimitTable, TCC_FREQ_LIMIT_DXB, 1);
		
		if(ret)
			tca_tsif_clean(h);
	}

	return ret;
}

void tca_tsif_clean(struct tcc_tsif_handle *h)
{
	if(h) {
		if(h->tea_dma_free) {
			h->tea_dma_free(&(h->rx_dma));
		}

		tcc_tsif_hw_deinit(h);
        tcc_cpufreq_set_limit_table(&gtTSIFClockLimitTable, TCC_FREQ_LIMIT_DXB, 0);
	}
}

static int tca_tsif_update_changed_pids(struct tcc_tsif_handle *h, unsigned int *pids, unsigned int count)
{
    int i,j,reg_index, reg_data;
    int num_registered, registered_pids[32], *new_pids;
    new_pids =  (int *)pids;
#if defined(SUPPORT_PIDFILTER_INTERNAL)
//backup pids to handle
     for (i = 0; i < count; i++) 
         h->match_pids[i]=  new_pids[i];
     h->match_pids_count = count;
     h->mpeg_ts = Hw0|Hw1;

//get current registerd pids
    num_registered = 0;
    for (i=0; i<32; i++)
	{
	    reg_index = i/2;
        if (i%2)
            reg_data = ((h->regs->TSPID[reg_index] & 0xFFFF0000)>>16);
        else
            reg_data = ((h->regs->TSPID[reg_index] & 0x0000FFFF));
        if(reg_data & Hw13)
            registered_pids[num_registered++] = reg_data & 0x1FFF;
    }

#if 0
{
    for(i=0;i<count;i++)
        printk("pids[%d] = 0x%X\n", i, pids[i]);

    for(i=0;i<num_registered;i++)
        printk("registered_pids[%d] = 0x%X\n", i, registered_pids[i]);
}
#endif

//check pids
    for(i=0;i<count;i++)
    {
        for(j=0;j<num_registered;j++)
        {
            if( registered_pids[j] != -1 && new_pids[i] == registered_pids[j] )
            {
                new_pids[i] = registered_pids[j] = -1;
                break;
            }
        }
    }
#if 0
{
    for(i=0;i<count;i++)
        printk("::::pids[%d] = 0x%X\n", i, pids[i]);

    for(i=0;i<num_registered;i++)
        printk("::::registered_pids[%d] = 0x%X\n", i, registered_pids[i]);    
}
#endif


/*NOW PID SYNC!!!!
 */
//delete pids    
    for (i=0; i<32; i++)
	{
   	    reg_index = i/2;
        if (i%2)
            reg_data = ((h->regs->TSPID[reg_index] & 0xFFFF0000)>>16);
        else
        {
            reg_data = ((h->regs->TSPID[reg_index] & 0x0000FFFF));
            if(i==0 && (reg_data & Hw13))
            {
                if( (reg_data&0x1FFF) == DEFAULT_PID )
                   continue; //don't delete PID, if pid is default pid.
                else
                    printk("%s:ERROR !!!, default packet is not PAT [0x%X]!!!!",__func__, reg_data&0x1FFF);
            }
        }

        if(reg_data & Hw13)
        {
            for(j=0;j<num_registered;j++)
            {
                if( registered_pids[j] != -1 && registered_pids[j] == (reg_data&0x1FFF) )
                {
                    if(i%2)
                    {
                        BITCLR(h->regs->TSPID[reg_index], Hw29);
                        printk("PIDT %d-H : 0x%08X [DELETE]\n",reg_index, (h->regs->TSPID[reg_index] & 0xFFFF0000)>>16);
                    }
                    else    
                    {
                        BITCLR(h->regs->TSPID[reg_index], Hw13);
                        printk("PIDT %d-L : 0x%08X [DELETE]\n",reg_index, h->regs->TSPID[reg_index] & 0xFFFF);                   
                    }
                    registered_pids[j] = -1;
                }
            }
        }
    }

//add pids
    for (i=0; i<count; i++)
    {
         if( new_pids[i] != -1)
         {
            //find free slot 
            for (j=0; j<32; j++)
            {
                reg_index = j/2;
                if (j%2)
                    reg_data = ((h->regs->TSPID[reg_index] & 0xFFFF0000)>>16);
                else
                    reg_data = ((h->regs->TSPID[reg_index] & 0x0000FFFF));

                if((reg_data & Hw13) == 0) //if free slot
                {
                    reg_data = Hw13|(new_pids[i]&0x1FFF);
                    if(j%2)
                    {                        
                        BITCSET(h->regs->TSPID[reg_index],(Hw32-Hw16),reg_data<<16);
                        printk("PIDT %d-H : 0x%08X [ADD]\n",reg_index, (h->regs->TSPID[reg_index] & 0xFFFF0000)>>16);
                    }
                    else    
                    {
                        BITCSET(h->regs->TSPID[reg_index],(Hw16-Hw0),reg_data);
                        printk("PIDT %d-L : 0x%08X [ADD]\n",reg_index, h->regs->TSPID[reg_index] & 0xFFFF);                   
                    }
                    new_pids[i] = -1;
                    break;
                }
            }
         }

    }
#endif    
    return 0;
}



int tca_tsif_register_pids(struct tcc_tsif_handle *h, unsigned int *pids, unsigned int count)
{
    int ret = 0, tsif_channel;
    tsif_channel = h->id;

    //supporting pids is 32 
    if (count <= 32 && tsif_channel < 2) {        
        int i = 0;
#if defined(SUPPORT_PIDFILTER_DMA)
        volatile unsigned long* PIDT;
        for (i = 0; i < 32; i++) {
            PIDT = (volatile unsigned long *)tcc_p2v(HwTSIF_PIDT(i));
            *PIDT = 0;
        }
        if (count > 0) {
            for (i = 0; i < count; i++) {
                PIDT = (volatile unsigned long *)tcc_p2v(HwTSIF_PIDT(i));
                h->match_pids[i]=  pids[i];
                *PIDT = pids[i] & 0x1FFFFFFF;
                BITSET(*PIDT, HwTSIF_PIDT_CH0<<tsif_channel);
                printk("PIDT 0x%08X : 0x%08X\n", (unsigned int)PIDT, (unsigned int)*PIDT);
            }            
            h->mpeg_ts = Hw0|Hw1;
        } 
#endif
#if defined(SUPPORT_PIDFILTER_INTERNAL)
        BITCSET(h->regs->TSPID[0], (Hw16-Hw0), Hw13);    //to prevent disabling pid filter        
    #if defined(UPDATE_ONLY_CHANGED_PID)
        tca_tsif_update_changed_pids(h, pids, count);
    #else
        for (i = 1; i < 16; i++) {            
		    BITCLR(h->regs->TSPID[i], (Hw32-Hw0));
        }

        if (count > 0) {
            for (i = 0; i < count; i++) {
                int reg_index = i/2;
                h->match_pids[i]=  pids[i];
                if(i%2)
                {
                    BITSET(h->regs->TSPID[reg_index], Hw29);
                    BITSET(h->regs->TSPID[reg_index],(pids[i]&0x1FFF)<<16);
                    printk("PIDT %d-H : 0x%08X\n",reg_index, (h->regs->TSPID[reg_index] & 0xFFFF0000)>>16);
                }
                else
                {
                    BITSET(h->regs->TSPID[reg_index], Hw13);
                    BITSET(h->regs->TSPID[reg_index], pids[i]&0x1FFF);
                    printk("PIDT %d-L : 0x%08X\n",reg_index, h->regs->TSPID[reg_index] & 0xFFFF);                   
                }
                h->mpeg_ts = Hw0|Hw1;
            }            
        }
    #endif
#endif
        h->match_pids_count = count;
    }
    else {
        h->match_pids_count = 0;
        printk("tsif: PID TABLE is so big !!!\n");
        ret = -EINVAL;
    }
    return ret;
}

EXPORT_SYMBOL(tca_tsif_init);
EXPORT_SYMBOL(tca_tsif_clean);
EXPORT_SYMBOL(tca_tsif_register_pids);

int tca_tsif_can_support(void)
{
#if     defined(CONFIG_M805S_8925_0XX)
    return 1;
#else    

   	if(machine_is_tcc8920())
    {
        //0x1006:8925, 0x1008:8923, 0x1005,0x1007:8920
        //if(system_rev == 0x1005 || system_rev == 0x1006 || system_rev == 0x1007 || system_rev == 0x1008)
        if( system_rev == 0x1006 || system_rev == 0x1008)
        { 
            return 1;
        }
    }

    #ifdef  SUPPORT_STB_TSIF_INTERFACE
	if(machine_is_tcc8920st())
    {
        return 1;
    }
    #endif
#endif
    return 0;
}

EXPORT_SYMBOL(tca_tsif_can_support);


