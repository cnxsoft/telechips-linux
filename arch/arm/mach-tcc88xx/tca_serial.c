/****************************************************************************
 *   FileName    : tca_tccserial.c
 *   Description :
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/

/*****************************************************************************
*
* Include
*
******************************************************************************/
#include <mach/bsp.h>
#include <mach/io.h>
#include <plat/serial.h>
#include <mach/tca_serial.h>
#include <asm/mach-types.h>
#include <linux/clk.h>

/*****************************************************************************
*
* Defines
*
******************************************************************************/

struct tcc_uart_port tcc_serial_ports[NR_PORTS] = {
	[0] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[0].port.lock),
			.iotype		= UPIO_MEM,
			.irq		= INT_UART0,
			.uartclk	= 0,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 0,
		},
		.base_addr      = tcc_p2v(HwUART0_BASE),
        .name = "uart0"
	},
	[1] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[1].port.lock),
			.iotype		= UPIO_MEM,
			.uartclk	= 0,
			.irq		= INT_UART1,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 1,
		},
        .base_addr      = tcc_p2v(HwUART1_BASE),
        .name = "uart1"
	},
	[2] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[2].port.lock),
			.iotype		= UPIO_MEM,
			.irq		= INT_UART2,
			.uartclk	= 0,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 2,
		},
        .base_addr      = tcc_p2v(HwUART2_BASE),
        .name = "uart2"
	},
	[3] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[3].port.lock),
			.iotype		= UPIO_MEM,
			.irq		= INT_UART3,
			.uartclk	= 0,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 3,
		},
        .base_addr      = tcc_p2v(HwUART3_BASE),
        .name = "uart3"
	},
	[4] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[4].port.lock),
			.iotype		= UPIO_MEM,
			.irq		= INT_UART4,
			.uartclk	= 0,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 4,
		},
        .base_addr      = tcc_p2v(HwUART4_BASE),
        .name = "uart4"
	},
	[5] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[5].port.lock),
			.iotype		= UPIO_MEM,
			.irq		= INT_UART5,
			.uartclk	= 0,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 5,
		},
        .base_addr      = tcc_p2v(HwUART5_BASE),
        .name = "uart5"
	},

};

/*****************************************************************************
* Function Name : tca_serial_portinit(int nCh, int nPort, unsigned long* pvGpioAddr, unsigned long* pvPortMuxAddr)
******************************************************************************
* Desription    : tca_serial_portinit
* Parameter     : int nCh, int nPort, unsigned long* pvGpioAddr, unsigned long* pvPortMuxAddr
* Return        : None
******************************************************************************/
void tca_serial_portinit(int nFlag_fc, int nPort)
{
	tca_serial_gpio_setting(nFlag_fc, nPort);
}
void tca_serial_gpio_setting(int nFlag_fc, int nPort)
{
	volatile PUARTPORTMUX pPortmux = (volatile PUARTPORTMUX)tcc_p2v(HwUART_PORTMUX_BASE);
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	unsigned int phy_port, hw_flow_ctrl;

	if (nPort > 5)
		return;

	if (nFlag_fc == 0)
		hw_flow_ctrl = 0;
	else
		hw_flow_ctrl = 1;

 	phy_port = (pPortmux->CHSEL >> (nPort*4))&0xF;
//	printk("\n%s: ch:%d, nPort:%d\n\n", __func__, nPort, phy_port);

	switch(phy_port) {
		case 0:
			BITCSET(pGPIO->GPEFN0, 0x000000FF, 0x00000011);	// RXD, TXD
			if (hw_flow_ctrl)
				BITCSET(pGPIO->GPEFN0, 0x0000FF00, 0x00001100);	// RTX, CTX	
			break;
		case 1:
			BITCSET(pGPIO->GPAFN1, 0x00000FF0, 0x00000110);	// RXD, TXD
			if (hw_flow_ctrl)
				BITCSET(pGPIO->GPAFN1, 0x000FF000, 0x00011000);	// RTX, CTS
			break;
		case 2:
			BITCSET(pGPIO->GPFFN3, 0x0000FF00, 0x00006600);	// RXD, TXD
			break;
		case 3:
			BITCSET(pGPIO->GPEFN1, 0x0000FF00, 0x00001100);	// RXD, TXD
			break;
		case 4:
			BITCSET(pGPIO->GPDFN1, 0x0FF00000, 0x01100000);	// RXD, TXD
			if (hw_flow_ctrl) {
				BITCSET(pGPIO->GPDFN1, 0xF0000000, 0x10000000);	// CTS
				BITCSET(pGPIO->GPDFN2, 0x0000000F, 0x00000001);	// RTS
			}
			break;
		case 5:
			BITCSET(pGPIO->GPDFN2, 0x00000FF0, 0x00000110);	// RXD, TXD
			if (hw_flow_ctrl)
				BITCSET(pGPIO->GPDFN2, 0x000FF000, 0x00011000);	// RTS, CTS
			break;
	}
}

void tca_serial_gpio_default(int nCh, int nPort)
{
	volatile PUARTPORTMUX pPortmux = (volatile PUARTPORTMUX)tcc_p2v(HwUART_PORTMUX_BASE);
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	unsigned int phy_port, hw_flow_ctrl;

	if (nPort > 5)
		return;

	if (nPort == 0)
		hw_flow_ctrl = 0;
	else
		hw_flow_ctrl = 1;

 	phy_port = (pPortmux->CHSEL >> (nPort*4))&0xF;

	switch(phy_port) {
		case 0:
			BITCSET(pGPIO->GPEFN0, 0x000000FF, 0x00000000);	// RXD, TXD
			if (hw_flow_ctrl)
				BITCSET(pGPIO->GPEFN0, 0x0000FF00, 0x00000000);	// RTX, CTX	
			break;
		case 1:
			BITCSET(pGPIO->GPAFN1, 0x00000FF0, 0x00000000);	// RXD, TXD
			if (hw_flow_ctrl)
				BITCSET(pGPIO->GPAFN1, 0x000FF000, 0x00000000);	// RTX, CTS
			break;
		case 2:
			BITCSET(pGPIO->GPFFN3, 0x0000FF00, 0x00000000);	// RXD, TXD
			break;
		case 3:
			BITCSET(pGPIO->GPEFN1, 0x0000FF00, 0x00000000);	// RXD, TXD
			break;
		case 4:
			BITCSET(pGPIO->GPDFN1, 0x0FF00000, 0x00000000);	// RXD, TXD
			if (hw_flow_ctrl) {
				BITCSET(pGPIO->GPDFN1, 0xF0000000, 0x00000000);	// CTS
				BITCSET(pGPIO->GPDFN2, 0x0000000F, 0x00000000);	// RTS
			}
			break;
		case 5:
			BITCSET(pGPIO->GPDFN2, 0x00000FF0, 0x00000000);	// RXD, TXD
			if (hw_flow_ctrl)
				BITCSET(pGPIO->GPDFN2, 0x000FF000, 0x00000000);	// RTS, CTS
			break;
	}
}

void tca_serial_intrinit(void)
{
    volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

    pPIC->SEL1      |= Hw15;
    pPIC->INTMSK1   |= Hw15;
    pPIC->MODE1     |= Hw15;    // Level trigger
}

int tca_serial_clock_enable(struct tcc_uart_port *tcc_port, int id)
{
	sprintf(tcc_port->name, "uart%d", id);
	tcc_port->clk = clk_get(NULL, tcc_port->name);
	if(IS_ERR(tcc_port->clk)) {
		printk("error: uart%d clock enable", id);
		tcc_port->clk = NULL;
		return -1;
	}
	clk_enable(tcc_port->clk);
	return 0;
}

/*****************************************************************************
* Function Name : tca_serial_dmaclrinterrupt(unsigned nDmanum, unsigned long* pVirtualDmaAddr)
******************************************************************************
* Desription    : tca_serial_dmaclrinterrupt
* Parameter     : unsigned nDmanum, unsigned long* pVirtualDmaAddr
* Return        : success(SUCCESS)
******************************************************************************/
unsigned int tca_serial_dmaclrinterrupt(unsigned nDmanum, unsigned long* pVirtualDmaAddr)
{
	PGDMACTRL pDMA = (PGDMACTRL)pVirtualDmaAddr;
	switch(	nDmanum ){
		case 0:
			pDMA->CHCTRL0 |= Hw3;
			break;
		case 1:
			pDMA->CHCTRL1 |= Hw3;
			break;
		case 2:
			pDMA->CHCTRL2 |= Hw3;
			break;
		default:
			return 0;
	}
	return 1;
}

/*****************************************************************************
* Function Name : tca_dma_dmacurrentaddress(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
******************************************************************************
* Desription    : tca_dma_dmacurrentaddress
* Parameter     : int m_DmaNumber, unsigned long* pVirtualDmaAddr
* Return        :
******************************************************************************/
int tca_dma_dmacurrentaddress(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
{
	PGDMACTRL pDMA = (PGDMACTRL)pVirtualDmaAddr;
	switch(m_DmaNumber) {
		case 0:
			return  (pDMA->C_DADR0);
		case 1:
			return  (pDMA->C_DADR1);
		case 2:
			return  (pDMA->C_DADR2);
		default:
			return 0;
	}
}

/*****************************************************************************
* Function Name : tca_dma_dmadeststartaddress(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
******************************************************************************
* Desription    : tca_dma_dmadeststartaddress
* Parameter     : int m_DmaNumber, unsigned long* pVirtualDmaAddr
* Return        :
******************************************************************************/
int tca_dma_dmadeststartaddress(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
{
	PGDMACTRL pDMA = (PGDMACTRL)pVirtualDmaAddr;
	switch(m_DmaNumber){
		case 0:
			return  (pDMA->ST_DADR0);
		case 1:
			return  (pDMA->ST_DADR1);
		case 2:
			return  (pDMA->ST_DADR2);
		default:
			return 0;
	}
	return 0;
}

/*****************************************************************************
* Function Name : tca_dma_clrien(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
******************************************************************************
* Desription    : tca_dma_clrien
* Parameter     : int m_DmaNumber, unsigned long* pVirtualDmaAddr
* Return        :
******************************************************************************/
int tca_dma_clrien(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
{
	PGDMACTRL pDMA = (PGDMACTRL)pVirtualDmaAddr;
	switch(m_DmaNumber){
		case 0:
			pDMA->CHCTRL0 &= ~Hw2;
			break;
		case 1:
			pDMA->CHCTRL1 &= ~Hw2;
			break;
		case 2:
			pDMA->CHCTRL2 &= ~Hw2;
			break;
		default:
			return 0;
	}
	return 0;
}

/*****************************************************************************
* Function Name : tca_dma_setien(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
******************************************************************************
* Desription    : tca_dma_setien
* Parameter     : int m_DmaNumber, unsigned long* pVirtualDmaAddr
* Return        :
******************************************************************************/
int tca_dma_setien(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
{
	PGDMACTRL pDMA = (PGDMACTRL)pVirtualDmaAddr;
	switch(m_DmaNumber){
		case 0:
			pDMA->CHCTRL0 |= Hw2;
			break;
		case 1:
			pDMA->CHCTRL1 |= Hw2;
			break;
		case 2:
			pDMA->CHCTRL2 |= Hw2;
			break;
		default:
			return 0;
	}
	return 0;
}

/*****************************************************************************
* Function Name : tca_dma_clren(unsigned long m_DmaNumber, int m_DmaChNumber)
******************************************************************************
* Desription    : Clear DMA enable bit
* Parameter     : unsigned long m_DmaNumber, int m_DmaChNumber
* Return        :
******************************************************************************/
int tca_dma_clren(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
{
    volatile PGDMACTRL pDMA = (volatile PGDMACTRL)tcc_p2v(m_DmaNumber);

    switch(	m_DmaNumber ){
        case 0:
            pDMA->CHCTRL0 &= ~Hw0;
            break;
        case 1:
            pDMA->CHCTRL1 &= ~Hw0;
            break;
        case 2:
            pDMA->CHCTRL2 &= ~Hw0;
            break;
        default:
            return 0;
    }
    return 1;
}

/*****************************************************************************
* Function Name : tca_dma_seten(unsigned long m_DmaNumber, int m_DmaChNumber)
******************************************************************************
* Desription    : Set DMA enable bit
* Parameter     : unsigned long m_DmaNumber, int m_DmaChNumber
* Return        :
******************************************************************************/
int tca_dma_seten(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
{
    volatile PGDMACTRL pDMA = (volatile PGDMACTRL)tcc_p2v(m_DmaNumber);

    switch(	m_DmaNumber ){
        case 0:
            pDMA->CHCTRL0 |= Hw0;
            break;
        case 1:
            pDMA->CHCTRL1 |= Hw0;
            break;
        case 2:
            pDMA->CHCTRL2 |= Hw0;
            break;
        default:
            return 0;
    }
    return 1;
}

/*****************************************************************************
* Function Name : tca_dma_ctrl(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
******************************************************************************
* Desription    : tca_dma_ctrl
* Parameter     : int m_DmaNumber, unsigned long* pVirtualDmaAddr
* Return        :
******************************************************************************/
int    tca_dma_ctrl(int m_DmaNumber, unsigned long* pVirtualDmaAddr)
{
	PGDMACTRL pDMA = (PGDMACTRL)pVirtualDmaAddr;

	switch(m_DmaNumber){
	case 0:
		return  (pDMA->CHCTRL0);
	case 1:
		return  (pDMA->CHCTRL1);
	case 2:
		return  (pDMA->CHCTRL2);
	default:
		return 0;
	}
}


/*****************************************************************************
* Function Name : tca_dma_setconfig(
	iunsigned uCH,
	void *pSRT,
	unsigned uSPARAM,
	void *pDST,
	unsigned uDPARAM,
	unsigned	uCHCTRL,
	unsigned uSize,
	unsigned channel,
	unsigned mode,
	unsigned long* pVirtualDmaAddr
)
******************************************************************************
* Desription    : tca_dma_setconfig
* Parameter     :
* Return        :
******************************************************************************/
void tca_dma_setconfig(
    unsigned uCH,
	void* pSRT,
	unsigned uSPARAM,
	void* pDST,
	unsigned uDPARAM,
	unsigned	uCHCTRL,
	unsigned uSize,
	unsigned channel,
	unsigned mode,
	unsigned long* pVirtualDmaAddr
)
{
	PGDMACTRL pDMA = (PGDMACTRL)pVirtualDmaAddr;

	switch( uCH )   {
	case 0:
		if (uSize){
        	// Set Source Address & Source Parameter (mask + increment)
			 pDMA->ST_SADR0 = (int)pSRT;
			 pDMA->SPARAM0 = uSPARAM;

			 // Set Dest Address & Dest Parameter (mask + increment)
			 pDMA->ST_DADR0 = (int)pDST;
			 pDMA->DPARAM0 = uDPARAM;
			 pDMA->RPTCTRL0 &= ~Hw31;


			 pDMA->EXTREQ0 = tca_uart_channelselect(channel, mode);
			 pDMA->HCOUNT0 = uSize;
		}

		 pDMA->CHCTRL0 = uCHCTRL;
		 pDMA->CHCTRL0 |= Hw0;
		break;
	case 1:
		if (uSize){
		// Set Source Address & Source Parameter (mask + increment)
			 pDMA->ST_SADR1 = (int)pSRT;
			 pDMA->SPARAM1 = uSPARAM;

        	// Set Dest Address & Dest Parameter (mask + increment)
			 pDMA->ST_DADR1 = (int)pDST;
			 pDMA->DPARAM1 = uDPARAM;
			 pDMA->RPTCTRL1 &= ~Hw31;

			 pDMA->EXTREQ1 = tca_uart_channelselect(channel, mode);
			 pDMA->HCOUNT1 = uSize;
        	}

		 pDMA->CHCTRL1 = uCHCTRL;
		 pDMA->CHCTRL1 |= Hw0;
        break;
	case 2:
		if (uSize){
		// Set Source Address & Source Parameter (mask + increment)
			 pDMA->ST_SADR2 = (int)pSRT;
			 pDMA->SPARAM2 = uSPARAM;

        	// Set Dest Address & Dest Parameter (mask + increment)
			 pDMA->ST_DADR2 = (int)pDST;
			 pDMA->DPARAM2 = uDPARAM;
			 pDMA->RPTCTRL2 &= ~Hw31;

			 pDMA->EXTREQ2 = tca_uart_channelselect(channel, mode);
			 pDMA->HCOUNT2 = uSize;
        	}

		 pDMA->CHCTRL2 = uCHCTRL;
		 pDMA->CHCTRL2 |= Hw0;
        break;
	}
}

/*****************************************************************************
* Function Name : tca_uart_channelselectint channel, int mode)
******************************************************************************
* Desription    : tca_serial_portinit
* Parameter     : int channel, int mode
* Return        :
******************************************************************************/
int tca_uart_channelselect(int channel, int mode)
{

	switch(channel){
	case 0:
		return (mode?HwEXTREQ_UART0_RX:HwEXTREQ_UART0_TX);
		break;
	case 1:
		return (mode?HwEXTREQ_UART1_RX:HwEXTREQ_UART1_TX);
		break;
	case 2:
		return (mode?HwEXTREQ_UART2_RX:HwEXTREQ_UART2_TX);
		break;
	case 3:
		return (mode?HwEXTREQ_UART3_RX:HwEXTREQ_UART3_TX);
	default :
		break;
	}
	return 0;
}

