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
#include <mach/io.h>

#include <mach/bsp.h>
#include <mach/tca_tsif.h>

//#define DEBUG_INFO
static void tcc_tsif_hw_init(struct tcc_tsif_handle *h);

static void tcc_tsif_clearfifopacket(struct tcc_tsif_handle *h)
{
	BITSET(h->regs->TSCR, Hw7);		//set to Hw7 : empties RX Fifo

	BITSET(h->regs->TSCR, Hw30);
	BITCLR(h->regs->TSCR, Hw30);

#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif 
}

static int tca_tsif_set_port(struct tcc_tsif_handle *h)
{
	volatile PGPIO gpio_regs = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
    volatile unsigned long* TSIFPORT = (volatile unsigned long *)tcc_p2v(HwTSIF_PORTSEL_BASE);
	int ret = 0;
    /*gpio_port is not gpio port such as A, B, C, D
     * It is tsif port number
     */

	switch (h->gpio_port)
	{
		case 1:
		{
		}
		break;
		case 2:
		{
		}
		break;

		case 3:
		{
		}
		break;
 
		case 4:
		{
		}
		break;

		case 5:
		{
		}
		break;		
		default:
		{
			ret = -1;
		}
		break;
	}
	
		return ret;
}

static void tcc_tsif_release_port(struct tcc_tsif_handle *h)
{
	volatile PGPIO gpio_regs = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
    volatile unsigned long* TSIFPORT = (volatile unsigned long *)tcc_p2v(HwTSIF_PORTSEL_BASE);

	switch (h->gpio_port)
	{
    	case 1:
		{
		}
		break;
		case 2:
		{
		}
		break;

		case 3:
		{
		}
		break;
 
		case 4:
		{
		}
		break;

		case 5:
		{
		}
		break;			
		default:
		{
			printk("error release port\n");
		}
		break;
	}

    /* tsif ch release */
	if(h->id == 0) {
		BITCLR(*TSIFPORT, Hw3|Hw2);
	} else {
		BITCLR(*TSIFPORT, Hw1|Hw0);
	}
}


static void tcc_tsif_set(struct tcc_tsif_handle *h)
{
//20110526 koo : android tcc880x kernel에서는 module init 후에 tsif & gpsb1을 swret state 및 clk disable 상태로 만들기 때문에 module init 시의 tsif & dma reg 설정 value가 
//				reset 되기 때문에 start 시 다시 설정.
	tcc_tsif_hw_init(h);

#ifdef GDMA
#if !defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000) && !defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136)
	BITSET(h->regs->TSCR, Hw17);
#endif
#else //GDMA
	BITSET(h->regs->TSCR, Hw17);
#endif //GDMA

	if(h->msb_first)		BITSET(h->regs->TSCR, Hw16);
	else					BITCLR(h->regs->TSCR, Hw16);

	if(h->big_endian)		BITSET(h->regs->TSCR, Hw15);
	else					BITCLR(h->regs->TSCR, Hw15);
		
	if(h->serial_mode) {
		BITSET(h->regs->TSCR, Hw14);
		BITSET(h->regs->TSCR, Hw4|Hw3|Hw1);
	} 
	else {
		BITCLR(h->regs->TSCR, Hw14);
		//20110413 koo : tcc880x의 경우 gpsb-dma를 사용할 때 parallel sync delay를 5clk로 주었을 때 read 시 data가 깨지는 증상이 발생하여 
		//					4clk으로 설정하여 사용하고 tcc930x의 경우에는 5clk으로 설정하여 사용하도록 soc team에서 guide 받음.
		//					 soc team에서는 4clk과 5clk 사이 margin 으로 인해 발생되는 문제로 추정함.

		BITSET(h->regs->TSCR, Hw2);
	}

	if(h->clk_polarity)		BITSET(h->regs->TSCR, Hw13);
	else					BITCLR(h->regs->TSCR, Hw13);

	if(h->valid_polarity)	BITSET(h->regs->TSCR, Hw12);
	else					BITCLR(h->regs->TSCR, Hw12);

	if(h->sync_polarity)	BITSET(h->regs->TSCR, Hw11);
	else					BITCLR(h->regs->TSCR, Hw11);

	if(h->sync_delay)		BITSET(h->regs->TSCR, Hw5);
	else					BITCLR(h->regs->TSCR, Hw5);
	
	//BITSET(h->regs->TSCR, (h->sync_delay & 0x1f));

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
		if(dma_regs->CHCTRL0 & Hw3) {
			BITSET(dma_regs->CHCTRL0, Hw3);		//dma done flag set
			h->dma_phy_curpos = dma_regs->C_DADR0;
		}
	}
	else if(h->dma_ch == 1)
	{
		if(dma_regs->CHCTRL1 & Hw3) {
			BITSET(dma_regs->CHCTRL1, Hw3);		//dma done flag set
			h->dma_phy_curpos = dma_regs->C_DADR1;
		}
	}
	else
	{
		if(dma_regs->CHCTRL2 & Hw3) {
			BITSET(dma_regs->CHCTRL2, Hw3);		//dma done flag set
			h->dma_phy_curpos = dma_regs->C_DADR2;
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
		BITCSET(dma_regs->ST_SADR0, (Hw32-Hw0), tsif_tsdi_addr);		//source addr set
		BITCLR(dma_regs->SPARAM0, (Hw32-Hw0));

		BITCSET(dma_regs->ST_DADR0, (Hw32-Hw0), h->rx_dma.dma_addr);	//destination addr set
		mask_addr = TSIF_MASK_SIZE << 8;								// 1mbyte mask set
		BITCSET(dma_regs->DPARAM0, (Hw32-Hw0), (mask_addr|TSIF_DMA_INCREASE_SIZE));		//inc & mask addr set

		BITCSET(dma_regs->HCOUNT0, (Hw32-Hw0), TSIF_DMA_HOPE_CNT(h->dma_intr_packet_cnt));		//hop count set
		BITCLR(dma_regs->CHCTRL0, (Hw32-Hw0));							//repeat ctrl reg set

		if(h->id == 0)	BITCSET(dma_regs->EXTREQ0, (Hw32-Hw0), Hw3);	//ext request set tsif0
		else			BITCSET(dma_regs->EXTREQ0, (Hw32-Hw0), Hw7);	//ext request set tsif1

		//20110530 koo : transfer type setting => edge or level
		//reg_val = Hw15|Hw11|Hw9|Hw8|Hw5|Hw4|Hw2|Hw1;
		reg_val = Hw15|Hw11|Hw5|Hw4|Hw2|Hw1;
		
		if(GDMA_WORD_SIZE == 4)			reg_val |= Hw5 | Hw4;
		else if(GDMA_WORD_SIZE == 2)	reg_val |= Hw4;
		if(GDMA_BURST_SIZE == 8)		reg_val |= Hw7 | Hw6;
		else if(GDMA_BURST_SIZE == 4)	reg_val |= Hw7;
		else if(GDMA_BURST_SIZE == 2)	reg_val |= Hw6;
		BITCSET(dma_regs->CHCTRL0, (Hw32-Hw0), reg_val);		//channel ctrl reg set
	} else if(h->dma_ch == 1) {
		tsif_tsdi_addr = (int)&(h->regs->TSDI);
		tsif_tsdi_addr -= IO_OFFSET;
		BITCSET(dma_regs->ST_SADR1, (Hw32-Hw0), tsif_tsdi_addr);		//source addr set
		BITCLR(dma_regs->SPARAM1, (Hw32-Hw0));

		BITCSET(dma_regs->ST_DADR1, (Hw32-Hw0), h->rx_dma.dma_addr);	//destination addr set
		mask_addr = TSIF_MASK_SIZE << 8;						    	// 1mbyte mask set
		BITCSET(dma_regs->DPARAM1, (Hw32-Hw0), (mask_addr|TSIF_DMA_INCREASE_SIZE));		//inc & mask addr set

		BITCSET(dma_regs->HCOUNT1, (Hw32-Hw0), TSIF_DMA_HOPE_CNT(h->dma_intr_packet_cnt));		//hop count set
		BITCLR(dma_regs->CHCTRL1, (Hw32-Hw0));							//repeat ctrl reg set

		if(h->id == 0)	BITCSET(dma_regs->EXTREQ1, (Hw32-Hw0), Hw3);	//ext request set tsif0
		else			BITCSET(dma_regs->EXTREQ1, (Hw32-Hw0), Hw7);	//ext request set tsif1

		//20110530 koo : transfer type setting => edge or level
		//reg_val = Hw15|Hw11|Hw9|Hw8|Hw5|Hw4|Hw2|Hw1;
		reg_val = Hw15|Hw11|Hw5|Hw4|Hw2|Hw1;

		if(GDMA_WORD_SIZE == 4)			reg_val |= Hw5 | Hw4;
		else if(GDMA_WORD_SIZE == 2)	reg_val |= Hw4;
		if(GDMA_BURST_SIZE == 8)		reg_val |= Hw7 | Hw6;
		else if(GDMA_BURST_SIZE == 4)	reg_val |= Hw7;
		else if(GDMA_BURST_SIZE == 2)	reg_val |= Hw6;
		BITCSET(dma_regs->CHCTRL1, (Hw32-Hw0), reg_val);			//channel ctrl reg set
	} else { 
		tsif_tsdi_addr = (int)&(h->regs->TSDI);
		tsif_tsdi_addr -= IO_OFFSET;
		BITCSET(dma_regs->ST_SADR2, (Hw32-Hw0), tsif_tsdi_addr);		//source addr set
		BITCLR(dma_regs->SPARAM2, (Hw32-Hw0));

		BITCSET(dma_regs->ST_DADR2, (Hw32-Hw0), h->rx_dma.dma_addr);	//destination addr set
		mask_addr = TSIF_MASK_SIZE << 8;								// 1mbyte mask set
		BITCSET(dma_regs->DPARAM2, (Hw32-Hw0), (mask_addr|TSIF_DMA_INCREASE_SIZE));		//inc & mask addr set

		BITCSET(dma_regs->HCOUNT2, (Hw32-Hw0), TSIF_DMA_HOPE_CNT(h->dma_intr_packet_cnt));		//hop count set
		BITCLR(dma_regs->CHCTRL2, (Hw32-Hw0));							//repeat ctrl reg set

		if(h->id == 0)	BITCSET(dma_regs->EXTREQ2, (Hw32-Hw0), Hw3);	//ext request set tsif0
		else			BITCSET(dma_regs->EXTREQ2, (Hw32-Hw0), Hw7);	//ext request set tsif1

		//20110530 koo : transfer type setting => edge or level
		//reg_val = Hw15|Hw11|Hw9|Hw8|Hw5|Hw4|Hw2|Hw1;
		reg_val = Hw15|Hw11|Hw5|Hw4|Hw2|Hw1;
		
		if(GDMA_WORD_SIZE == 4)			reg_val |= Hw5 | Hw4;
		else if(GDMA_WORD_SIZE == 2)	reg_val |= Hw4;
		if(GDMA_BURST_SIZE == 8)		reg_val |= Hw7 | Hw6;
		else if(GDMA_BURST_SIZE == 4)	reg_val |= Hw7;
		else if(GDMA_BURST_SIZE == 2)	reg_val |= Hw6;
		BITCSET(dma_regs->CHCTRL2, (Hw32-Hw0), reg_val);			//channel ctrl reg set
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

	if(h->dma_ch == 0)	 	BITSET(dma_regs->CHCTRL0, 	Hw0);	//dma enable
	else if(h->dma_ch == 1)	BITSET(dma_regs->CHCTRL1, 	Hw0);	//dma enable
	else					BITSET(dma_regs->CHCTRL2, 	Hw0);	//dma enable
		
	BITSET(h->regs->TSCR, Hw31);								//tsif enable

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

	if(h->dma_ch == 0)		BITCLR(dma_regs->CHCTRL0, 	Hw0);	//dma disable
	else if(h->dma_ch == 1)	BITCLR(dma_regs->CHCTRL1, 	Hw0);	//dma disable
	else					BITCLR(dma_regs->CHCTRL2, 	Hw0);	//dma disable

	BITCLR(h->regs->TSCR, Hw31);								//tsif disable

#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif

	return 0;
}

static void tcc_tsif_dma_deinit(struct tcc_tsif_handle *h)
{
	volatile PGDMACTRL dma_regs;

	dma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
}
#else //GDMA
static void tcc_tsif_isr(struct tcc_tsif_handle *h)
{
	volatile PGPSB dma_regs;
	unsigned long dma_done_reg = 0;

	if(h->id == 0)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH0_BASE);
	else if(h->id == 1)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH1_BASE);

	dma_done_reg = dma_regs->DMAICR;
	if( dma_done_reg & (Hw29|Hw28) ) {
		BITSET(dma_regs->DMAICR, (Hw29|Hw28));
		h->cur_q_pos = (int)(dma_regs->DMASTR >> 17);
	}

	return;
}

static void tcc_tsif_dma_init(struct tcc_tsif_handle *h)
{
	volatile PGPSB dma_regs;
	
	if(h->id == 0)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH0_BASE);
	else if(h->id == 1)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH1_BASE);

	//BITCSET(dma_regs->TXBASE, (Hw32-Hw0), h->regs->TSDI);
	BITCSET(dma_regs->RXBASE, (Hw32-Hw0), h->rx_dma.dma_addr);

	//BITCSET(dma_regs->PACKET, (Hw32-Hw0), MPEG_PACKET_SIZE);
	//BITSET(dma_regs->PACKET, ((h->dma_total_size/MPEG_PACKET_SIZE)&0x1FFF)<<16);

	BITCLR(dma_regs->DMACTR, Hw28); //0:little endian, 1:Big endian
}

static int tcc_tsif_dmastart(struct tcc_tsif_handle *h)
{
	volatile PGPSB dma_regs;
	unsigned int packet_cnt = (h->dma_total_packet_cnt & 0x1FFF) -1;
	unsigned int packet_size = (MPEG_PACKET_SIZE & 0x1FFF);
	unsigned int intr_packet_cnt = (h->dma_intr_packet_cnt & 0x1FFF) -1;

	if(h->id == 0)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH0_BASE);
	else if(h->id == 1)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH1_BASE);

	BITCSET(dma_regs->RXBASE, (Hw32-Hw0), h->rx_dma.dma_addr);

	dma_regs->PACKET = ( packet_cnt << 16) | packet_size;
	BITCSET(dma_regs->DMAICR, 0x1FFF, intr_packet_cnt);	

	BITSET(dma_regs->DMAICR, Hw16); 			//enable DMA packet interrupt
	//BITSET(dma_regs->DMAICR, Hw17);				//enable DMA Done Interrupt
	BITCLR(dma_regs->DMAICR, Hw20);				//set RX interrupt
	BITSET(dma_regs->DMAICR, Hw16);

	BITSET(dma_regs->DMACTR, Hw2);				//clear tx/rx packet counter
	//BITCLR(dma_regs->DMACTR, Hw2);

	BITSET(dma_regs->DMACTR, Hw30);				//enable recv DMA requeset 

#ifndef TSIF_PIDMATCH_USE	
	BITSET(dma_regs->DMACTR, Hw19|Hw18);		//enable PID & Sync Byte match
#endif	

	//BITCSET(dma_regs->DMACTR, Hw5|Hw4, Hw29);
	BITCSET(dma_regs->DMACTR, Hw5|Hw4, Hw4);	//00:normal mode, 01:MPEG2_TS mode
	BITSET(dma_regs->DMACTR, Hw29);				//enable continues

	BITSET(dma_regs->DMACTR, Hw1);				//enable TSSEL
	BITSET(dma_regs->DMACTR, Hw0);				//enable DMA

	BITSET(h->regs->TSCR, Hw31);

	return 0;
}

static int tcc_tsif_dmastop(struct tcc_tsif_handle *h)
{
	volatile PGPSB dma_regs;
	
	if(h->id == 0)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH0_BASE);
	else if(h->id == 1)
		dma_regs = (volatile PGPSB)tcc_p2v(HwGPSBCH1_BASE);

	BITCLR(dma_regs->DMACTR, Hw30); //disable DMA receive
	BITSET(dma_regs->DMAICR, Hw29|Hw28);
	BITCLR(dma_regs->DMACTR, Hw0);
	
	BITCLR(h->regs->TSCR, Hw31);

	return 0;
}

static void tcc_tsif_dma_deinit(struct tcc_tsif_handle *h)
{
}
#endif //GDMA

static void tcc_tsif_hw_init(struct tcc_tsif_handle *h)
{
	unsigned int reg_val;
	int i;

	tca_tsif_set_port(h);

	if(TSIF_BYTE_SIZE == 4)			reg_val = Hw17 | Hw9 | Hw8 | Hw7;
	else if(TSIF_BYTE_SIZE == 2)	reg_val = Hw17 | Hw8 | Hw7;
	else							reg_val = Hw17 | Hw7;
	BITCSET(h->regs->TSCR, 	(Hw32-Hw0), reg_val);
	
	//20100120 koo atsc 동작 시 little endian으로 동작 시키면 tsif reg에서 0x47 detect는 되지만 dma buf에 4byte align이 뒤바뀌어져 있음. 하여 0x47 detect mode clear
	BITCLR(h->regs->TSCR, Hw17);
#if !defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000) && !defined(CONFIG_iTV_FE_TUNER_MODULE_FQD1136)
	BITSET(h->regs->TSCR, Hw18);
#endif
	reg_val = (TSIF_RXFIFO_THRESHOLD << 5) | Hw4;
	BITCSET(h->regs->TSIC,(Hw32-Hw0), reg_val);
	
 	for(i=0; i<16; i++) {	
		BITCLR(h->regs->TSPID[i], 	(Hw32-Hw0));
	}

	tcc_tsif_dma_init(h);
	
#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif
}

static void tcc_tsif_hw_deinit(struct tcc_tsif_handle *h)
{
	tcc_tsif_release_port(h);

	BITCLR(h->regs->TSCR, 	(Hw32-Hw0));
	BITCLR(h->regs->TSIC,	(Hw32-Hw0));

	tcc_tsif_dma_deinit(h);

#ifdef DEBUG_INFO
	printk("%s\n", __func__);
#endif
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
	
	if(regs)
	{   
		h->regs = (volatile struct tca_spi_regs *)tcc_p2v((regs));
		h->dma_controller	= dma_controller;
		h->id 				= tsif_ch;
		h->dma_ch			= dma_ch;
#ifdef GDMA
		h->irq 				= (INT_DMA0_BASE + 3*dma_controller + dma_ch); //INT_DMA
#else //GDMA
		h->irq				= (tsif_ch == 0) ? INT_GPSB0_DMA : INT_GPSB1_DMA;
#endif //GDMA
		h->gpio_port 		= port;

		h->dma_stop 		= tcc_tsif_dmastop;
		h->dma_start 		= tcc_tsif_dmastart;
		h->clear_fifo_packet= tcc_tsif_clearfifopacket;
		h->tsif_set			= tcc_tsif_set;
		h->tsif_isr			= tcc_tsif_isr;

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

		tcc_tsif_hw_init(h);
		
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
	}
}

int tca_tsif_register_pids(struct tcc_tsif_handle *h, unsigned int *pids, unsigned int count)
{
    return 0;
}

EXPORT_SYMBOL(tca_tsif_init);
EXPORT_SYMBOL(tca_tsif_clean);
EXPORT_SYMBOL(tca_tsif_register_pids);

int tca_tsif_can_support(void)
{
    return 0;
}

EXPORT_SYMBOL(tca_tsif_can_support);

