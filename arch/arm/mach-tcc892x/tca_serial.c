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
#include <linux/gpio.h>

/*****************************************************************************
*
* Defines
*
******************************************************************************/

static int uart_port_map[24][2] = {
	{TCC_GPA(7), GPIO_FN(7)},     // UT_TXD(0)
	{TCC_GPA(13), GPIO_FN(7)},   // UT_TXD(1)
	{TCC_GPD(4), GPIO_FN(7)},     // UT_TXD(2)
	{TCC_GPD(11), GPIO_FN(7)},   // UT_TXD(3)
	{TCC_GPD(17), GPIO_FN(7)},   // UT_TXD(4)
	{TCC_GPB(0), GPIO_FN(10)},   // UT_TXD(5)
	{TCC_GPB(11), GPIO_FN(10)},	// UT_TXD(6)
	{TCC_GPB(19), GPIO_FN(10)}, // UT_TXD(7)
	{TCC_GPB(25), GPIO_FN(10)},       // UT_TXD(8)
	{TCC_GPC(0), GPIO_FN(6)},     // UT_TXD(9)
	{TCC_GPC(10), GPIO_FN(6)},     // UT_TXD(10)
	{TCC_GPC(23), GPIO_FN(6)},     // UT_TXD(11)
	{TCC_GPE(12), GPIO_FN(5)},     // UT_TXD(12)
	{TCC_GPE(28), GPIO_FN(5)},     // UT_TXD(13)
	{TCC_GPF(13), GPIO_FN(9)},     // UT_TXD(14) *
	{TCC_GPF(17), GPIO_FN(9)},     // UT_TXD(15)
	{TCC_GPF(25), GPIO_FN(9)},     // UT_TXD(16)
	{TCC_GPG(0), GPIO_FN(3)},     // UT_TXD(17)
	{TCC_GPG(4), GPIO_FN(3)},     // UT_TXD(18)
	{TCC_GPG(8), GPIO_FN(3)},     // UT_TXD(19)
	{TCC_GPG(14), GPIO_FN(3)},     // UT_RXD(20) *
	{TCC_GPG(18), GPIO_FN(3)},  	     // UT_RXD(21) *
	{TCC_GPHDMI(0), GPIO_FN(3)},     // UT_TXD(22)		
	{TCC_GPADC(2), GPIO_FN(4)},     // UT_TXD(23)
};
	
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
	[6] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[6].port.lock),
			.iotype		= UPIO_MEM,
			.irq		= INT_UART6,
			.uartclk	= 0,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 6,
		},
        .base_addr      = tcc_p2v(HwUART6_BASE),
        .name = "uart6"
	},
	[7] = {
		.port = {
			.lock		= __SPIN_LOCK_UNLOCKED(tcc_uart_port[7].port.lock),
			.iotype		= UPIO_MEM,
			.irq		= INT_UART7,
			.uartclk	= 0,
			.fifosize	= FIFOSIZE,
			.flags		= UPF_BOOT_AUTOCONF,
			.line		= 7,
		},
        .base_addr      = tcc_p2v(HwUART7_BASE),
        .name = "uart7"
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
	volatile PUARTPORTCFG pPORT = (volatile PUARTPORTCFG)tcc_p2v(HwUART_PORTCFG_BASE);
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	unsigned int phy_port, hw_flow_ctrl;

	if (nPort > 7)
		return;

	if (nFlag_fc == 0)
		hw_flow_ctrl = 0;
	else
		hw_flow_ctrl = 1;

	switch(nPort){
		case 0: phy_port = pPORT->PCFG0.bREG.UART0; break;
		case 1: phy_port = (pPORT->PCFG0.nREG & 0xff00) >> 8; break;
		case 2: phy_port = pPORT->PCFG0.bREG.UART2; break;
		case 3: phy_port = pPORT->PCFG0.bREG.UART3; break;
		case 4: phy_port = pPORT->PCFG1.bREG.UART4; break;			
		case 5: phy_port = pPORT->PCFG1.bREG.UART5; break;
		case 6: phy_port = pPORT->PCFG1.bREG.UART6; break;
		case 7: phy_port = pPORT->PCFG1.bREG.UART7; break;			
	}


	tcc_gpio_config(uart_port_map[phy_port][0], uart_port_map[phy_port][1]);	// TX
	tcc_gpio_config(uart_port_map[phy_port][0]+1, uart_port_map[phy_port][1]);	 // RX
	if(hw_flow_ctrl == 1){
		if(phy_port == 14 || phy_port == 20 || phy_port == 21){
		    tcc_gpio_config(uart_port_map[phy_port][0]-1, uart_port_map[phy_port][1]); //CTS	
		    tcc_gpio_config(uart_port_map[phy_port][0]-2, uart_port_map[phy_port][1]); //RTS	 
		}
		else{	
		    tcc_gpio_config(uart_port_map[phy_port][0]+2, uart_port_map[phy_port][1]); // CTS	
		    tcc_gpio_config(uart_port_map[phy_port][0]+3, uart_port_map[phy_port][1]); // RTS	
		}
	}


}

void tca_serial_intrinit(void)
{
    volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

    pPIC->SEL1.bREG.UART = 1;
    pPIC->INTMSK1.bREG.UART = 1;
    pPIC->MODE1.bREG.UART = 1; // Level trigger
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

int tca_serial_clock_disable(struct tcc_uart_port *tcc_port, int id)
{
	sprintf(tcc_port->name, "uart%d", id);
	tcc_port->clk = clk_get(NULL, tcc_port->name);
	if(IS_ERR(tcc_port->clk)) {
		printk("error: uart%d clock enable", id);
		tcc_port->clk = NULL;
		return -1;
	}
	clk_disable(tcc_port->clk);
	return 0;
}


int tca_serial_port_pullup(int nPort, int enable, unsigned long long uartPortCFG)
{
	#define PORT_NUM 8
	int i;
	unsigned char phy_port[PORT_NUM];

	for(i=0 ; i<PORT_NUM ; i++)
		phy_port[i] = (uartPortCFG>>(i*8))&0xFF;

	if(enable)
		tcc_gpio_config(uart_port_map[phy_port[nPort]][0], GPIO_PULLUP);
	else
		tcc_gpio_config(uart_port_map[phy_port[nPort]][0], GPIO_PULLDOWN);

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
			pDMA->CHCTRL0.nREG |= Hw3;
			break;
		case 1:
			pDMA->CHCTRL1.nREG |= Hw3;
			break;
		case 2:
			pDMA->CHCTRL2.nREG |= Hw3;
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
			return  (pDMA->C_DADR0.nREG);
		case 1:
			return  (pDMA->C_DADR1.nREG);
		case 2:
			return  (pDMA->C_DADR2.nREG);
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
			return  (pDMA->ST_DADR0.nREG);
		case 1:
			return  (pDMA->ST_DADR1.nREG);
		case 2:
			return  (pDMA->ST_DADR2.nREG);
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
			pDMA->CHCTRL0.nREG &= ~Hw2;
			break;
		case 1:
			pDMA->CHCTRL1.nREG &= ~Hw2;
			break;
		case 2:
			pDMA->CHCTRL2.nREG &= ~Hw2;
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
			pDMA->CHCTRL0.nREG |= Hw2;
			break;
		case 1:
			pDMA->CHCTRL1.nREG |= Hw2;
			break;
		case 2:
			pDMA->CHCTRL2.nREG |= Hw2;
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
    volatile PGDMACTRL pDMA = (volatile PGDMACTRL)pVirtualDmaAddr;

    switch(	m_DmaNumber ){
        case 0:
            pDMA->CHCTRL0.nREG &= ~Hw0;
            break;
        case 1:
            pDMA->CHCTRL1.nREG &= ~Hw0;
            break;
        case 2:
            pDMA->CHCTRL2.nREG &= ~Hw0;
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
    volatile PGDMACTRL pDMA = (volatile PGDMACTRL)pVirtualDmaAddr;

    switch(	m_DmaNumber ){
        case 0:
            pDMA->CHCTRL0.nREG |= Hw0;
            break;
        case 1:
            pDMA->CHCTRL1.nREG |= Hw0;
            break;
        case 2:
            pDMA->CHCTRL2.nREG |= Hw0;
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
		return  (pDMA->CHCTRL0.nREG);
	case 1:
		return  (pDMA->CHCTRL1.nREG);
	case 2:
		return  (pDMA->CHCTRL2.nREG);
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
			 pDMA->ST_SADR0.nREG = (int)pSRT;
			 pDMA->SPARAM0.nREG = uSPARAM;

			 // Set Dest Address & Dest Parameter (mask + increment)
			 pDMA->ST_DADR0.nREG = (int)pDST;
			 pDMA->DPARAM0.nREG = uDPARAM;
			 pDMA->RPTCTRL0.nREG &= ~Hw31;


			 pDMA->EXTREQ0.nREG = tca_uart_channelselect(channel, mode);
			 pDMA->HCOUNT0.nREG = uSize;
		}

		 pDMA->CHCTRL0.nREG = uCHCTRL;
		 pDMA->CHCTRL0.nREG |= Hw0;
		break;
	case 1:
		if (uSize){
		// Set Source Address & Source Parameter (mask + increment)
			 pDMA->ST_SADR1.nREG = (int)pSRT;
			 pDMA->SPARAM1.nREG = uSPARAM;

        	// Set Dest Address & Dest Parameter (mask + increment)
			 pDMA->ST_DADR1.nREG = (int)pDST;
			 pDMA->DPARAM1.nREG = uDPARAM;
			 pDMA->RPTCTRL1.nREG &= ~Hw31;

			 pDMA->EXTREQ1.nREG = tca_uart_channelselect(channel, mode);
			 pDMA->HCOUNT1.nREG = uSize;
        	}

		 pDMA->CHCTRL1.nREG = uCHCTRL;
		 pDMA->CHCTRL1.nREG |= Hw0;
        break;
	case 2:
		if (uSize){
		// Set Source Address & Source Parameter (mask + increment)
			 pDMA->ST_SADR2.nREG = (int)pSRT;
			 pDMA->SPARAM2.nREG = uSPARAM;

        	// Set Dest Address & Dest Parameter (mask + increment)
			 pDMA->ST_DADR2.nREG = (int)pDST;
			 pDMA->DPARAM2.nREG = uDPARAM;
			 pDMA->RPTCTRL2.nREG &= ~Hw31;

			 pDMA->EXTREQ2.nREG = tca_uart_channelselect(channel, mode);
			 pDMA->HCOUNT2.nREG = uSize;
        	}

		 pDMA->CHCTRL2.nREG = uCHCTRL;
		 pDMA->CHCTRL2.nREG |= Hw0;
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

