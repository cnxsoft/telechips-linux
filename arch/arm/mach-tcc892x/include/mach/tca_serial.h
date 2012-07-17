/****************************************************************************
 *   FileName    : tca_tccserial.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef _TCA_SERIAL_H_
#define _TCA_SERIAL_H_

#include <linux/clk.h>
#include <linux/wait.h>
#include <linux/serial_core.h>

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
*
* Defines
*
******************************************************************************/
//UART
#define	HwUART_IER_EMSI_EN              Hw3								// Enable Modem status interrupt
#define	HwUART_IER_EMSI_DIS             ~Hw3							// Disable Modem status interrupt
#define	HwUART_IER_ELSI_EN              Hw2								// Enable receiver line status interrupt
#define	HwUART_IER_ELSI_DIS             ~Hw2							// Disable receiver line status interrupt
#define	HwUART_IER_ETXI_EN              Hw1								// Enable transmitter holding register empty interrupt
#define	HwUART_IER_ETXI_DIS             ~Hw1							// Disable transmitter holding register empty interrupt
#define	HwUART_IER_ERXI_EN              Hw0								// Enable received data available interrupt
#define	HwUART_IER_ERXI_DIS             ~Hw0							// Disable received data available interrupt
#define	HwUART_IIR_STF                  Hw27							// 1:TX is done (SmartCard TX done flag)
#define	HwUART_IIR_IPF                  Hw0								// 1:Interrupt has not generated, 0:Interrupt pending

#define	HwEXTREQ_UART1_RX               Hw30							// Connected Hardware = UART Channel 1 Receiver
#define	HwEXTREQ_UART1_TX               Hw29							// Connected Hardware = UART Channel 1 Tranceiver
#define	HwEXTREQ_UART0_RX               Hw27							// Connected Hardware = UART Channel 0 Receiver
#define	HwEXTREQ_UART0_TX               Hw26							// Connected Hardware = UART Channel 0 Tranceiver
#define	HwEXTREQ_UART3_RX               Hw11							// Connected Hardware = UART Channel 3 Receiver
#define	HwEXTREQ_UART3_TX               Hw10							// Connected Hardware = UART Channel 3 Tranceiver
#define	HwEXTREQ_UART2_RX               Hw9								// Connected Hardware = UART Channel 2 Receiver
#define	HwEXTREQ_UART2_TX               Hw8								// Connected Hardware = UART Channel 2 Tranceiver

//DMA
#define	HwCHCTRL_SYNC_EN                Hw13							// Synchronize Hardware Request
#define HwCHCTRL_HRD_WR			Hw12							// ACK/EOT signals are issued When DMA-Write Operation
#define	HwCHCTRL_BST_BURST		Hw10							// DMA transfer executed with no arbitration(burst operation)
#define	HwCHCTRL_BST_ARB		(0)							// DMA transfer executed wth arbitration
#define	HwCHCTRL_TYPE_SINGE		(0)							// SINGLE transfer with edge-triggered detection
#define	HwCHCTRL_TYPE_HW		Hw8							// HW Transfer
#define	HwCHCTRL_TYPE_SW		Hw9							// SW transfer
#define	HwCHCTRL_HRD_RD			(0)							// ACK/EOT signals are issued When DMA-Read Operation
#define	HwCHCTRL_TYPE_SL                (Hw9+Hw8)						// SINGLE transfer with level-triggered detection
#define	HwCHCTRL_BSIZE_1                HwZERO							// 1 Burst transfer consists of 1 read or write cycle
#define	HwCHCTRL_WSIZE_8                HwZERO							// Each cycle read or write 8bit data
#define	HwCHCTRL_IEN_ON                 Hw2								// At the same time the FLAG goes to 1, DMA interrupt request is generated
#define	HwCHCTRL_FLAG                   Hw3								// W : Clears FLAG to 0, R : Represents that all hop of transfer are fulfilled
#define	HwCHCTRL_CONT_C                 Hw15							// DMA transfer begins from C_SADR / C_DADR address. It must be used after the former transfer has been executed, so that C_SADR and C_DADR contain a meaningful vlaue
#define	HwCHCTRL_REP_EN                 Hw1								// The DMA channel remains enabled. When another DMA request has occurred, the DMA channel start transfer data again with the same manner(type,address,increment,mask) as the latest transfer of that channel


#define UART1           1
#define NR_PORTS        (8)
#define FIFOSIZE        (16) /* defend on FCR */

#define CONSOLE_PORT        0
#define CONSOLE_BAUDRATE    115200
#define CONSOLE_BASE        (tcc_p2v(HwUART0_BASE))

#define SERIAL_TX_DMA_INT	        INT_DMA1_CH0
#define SERIAL_TX_DMA_CH_NUM        0
#define SERIAL_TX_DMA_MODE          0
#define SERIAL_TX_DMA_BUF_SIZE      FIFOSIZE

#define SERIAL_RX_DMA_CH_NUM        2
#define SERIAL_RX_DMA_MODE          1
#define SERIAL_RX_DMA_BUF_SIZE      0x8000



/*****************************************************************************
*
* Variables
*
******************************************************************************/
extern struct tcc_uart_port tcc_serial_ports[NR_PORTS];



/*****************************************************************************
*
* APIs
*
******************************************************************************/
extern int	 tca_uart_channelselect(int channel, int mode);
extern void tca_serial_gpio_setting(int nCh, int nPort);
extern void tca_serial_gpio_default(int nCh, int nPort);
extern void tca_serial_portinit(int nCh, int nPort);
extern void tca_serial_intrinit(void);
extern int  tca_serial_clock_enable(struct tcc_uart_port *tcc_port, int id);
extern int  tca_serial_clock_disable(struct tcc_uart_port *tcc_port, int id);
extern int tca_serial_port_pullup(int nPort, int enable, unsigned long long uartPortCFG);

int	tca_dma_ctrl(int m_DmaNumber, unsigned long* pVirtualDmaAddr);
int	tca_dma_dmacurrentaddress(int m_DmaNumber, unsigned long* pVirtualDmaAddr);
int tca_dma_dmadeststartaddress(int m_DmaNumber,unsigned long* pVirtualDmaAddr);
int tca_dma_clrien(int m_DmaNumber, unsigned long* pVirtualDmaAddr);
int tca_dma_setien(int m_DmaNumber, unsigned long* pVirtualDmaAddr);
int tca_dma_clren(int m_DmaNumber, unsigned long* pVirtualDmaAddr);
int tca_dma_seten(int m_DmaNumber, unsigned long* pVirtualDmaAddr);
unsigned int tca_serial_dmaclrinterrupt(unsigned nDmanum, unsigned long* pVirtualDmaAddr);
void tca_dma_setconfig( unsigned uCH,	void* pSRT, 	
					   unsigned uSPARAM,	void* pDST, 	
					   unsigned uDPARAM,	unsigned	uCHCTRL,	
					   unsigned uSize,	unsigned channel,	
					   unsigned mode, unsigned long* pVirtualDmaAddr);


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef _TCA_SERIAL_H_  */

