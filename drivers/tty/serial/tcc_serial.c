/*
 * linux/drivers/serial/tcc_serial.c
 *
 * Based on: drivers/serial/s3c2410.c and driver/serial/bfin_5xx.c
 * Author:  <linux@telechips.com>
 * Created: June 10, 2008
 * Description: Driver for onboard UARTs on the Telechips TCC Series
 *
 * Copyright (C) 2008-2009 Telechips
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA V
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/console.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/serial_reg.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>


#include <mach/gpio.h>
#include <mach/hardware.h>
#include <mach/bsp.h>
#include <plat/serial.h>
#include <mach/tca_ckc.h>
#include <mach/tca_serial.h>

#include <linux/dma-mapping.h>
#include <linux/cpufreq.h>

#define DRV_NAME "tcc-uart"

#define TIME_STEP                   1//(1*HZ)


#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>


//struct timer_list  *timer;
//static int rx_dma_tail=0;
void *kerneltimer_timeover(void *arg );
//static int timer_state=0;
static DECLARE_WORK(work_queue, kerneltimer_timeover);
//static unsigned long start_value;

#define TCC_GPS_UART 3

#define TIMEOUT         jiffies_to_msecs(1000)

#define UART_IER_ELSI   UART_IER_RLSI

#define UART_FLOW_CONTROL 1
#define UART_NOT_FLOW_CONTROL 0


/* UARTx IIR Masks */
#define IIR_IDD         0x0E    /* Interrupt ID, Source, Reset */
#define IIR_IPF         Hw0     /* Interrupt Flag   */

/* UARTx_FCR Masks                                  */
#define FCR_FE          Hw0     /* FIFO Enable      */
#define FCR_RXFR        Hw1     /* RX FIFO Reset    */
#define FCR_TXFR        Hw2     /* TX FIFO Reset    */
#define FCR_DRTE        Hw3     /* DMA Mode Select  */

/* UARTx_LCR Masks                                                  */
#define LCR_WLS(x)      (((x)-5) & 0x03)    /* Word Length Select   */
#define LCR_STB         Hw2                 /* Stop Bits            */
#define LCR_PEN         Hw3                 /* Parity Enable        */
#define LCR_EPS         Hw4                 /* Even Parity Select   */
#define LCR_SP          Hw5                 /* Stick Parity         */
#define LCR_SB          Hw6                 /* Set Break            */
#define LCR_DLAB        Hw7                 /* Divisor Latch Access */

/* UARTx_MCR Mask                                                   */
#define MCR_RTS         Hw1     /* Request To Sent                  */
#define MCR_LOOP        Hw4     /* Loopback Mode Enable             */
#define MCR_AFE         Hw5     /* Auto Flow Control Enable         */
#define MCR_RS          Hw6     /* RTS Deassert Condition Control   */

/* UARTx_LSR Masks                                          */
#define LSR_DR          Hw0     /* Data Ready               */
#define LSR_OE          Hw1     /* Overrun Error            */
#define LSR_PE          Hw2     /* Parity Error             */
#define LSR_FE          Hw3     /* Framing Error            */
#define LSR_BI          Hw4     /* Break Interrupt          */
#define LSR_THRE        Hw5     /* THR Empty                */
#define LSR_TEMT        Hw6     /* TSR and UART_THR Empty   */

/* UARTx_IER Masks */
#define IER_ERXI        Hw0
#define IER_ETXI        Hw1
#define IER_ELSI        Hw2

/* UARTx UCR Masks */
#define UCR_TxDE        Hw0
#define UCR_RxDE        Hw1
#define UCR_TWA         Hw2
#define UCR_RWA         Hw3


#define OFFSET_THR    0x00	/* Transmit Holding register            */
#define OFFSET_RBR    0x00	/* Receive Buffer register              */
#define OFFSET_DLL    0x00	/* Divisor Latch (Low-Byte)             */
#define OFFSET_IER    0x04	/* Interrupt Enable Register            */
#define OFFSET_DLM    0x04	/* Divisor Latch (High-Byte)            */
#define OFFSET_IIR    0x08	/* Interrupt Identification Register    */
#define OFFSET_FCR    0x08	/* FIFO Control Register                */
#define OFFSET_LCR    0x0C	/* Line Control Register                */
#define OFFSET_MCR    0x10	/* Modem Control Register               */
#define OFFSET_LSR    0x14	/* Line Status Register                 */
#define OFFSET_MSR    0x18	/* Modem Status Register                */
#define OFFSET_SCR    0x1C	/* SCR Scratch Register                 */
#define OFFSET_AFT    0x20	/* AFC Trigger Level Register           */
#define OFFSET_UCR    0x24	/* UART Control Register                */


#define portaddr(port, reg) ((port)->membase + (reg))

#define rd_regb(port, reg) (__raw_readb(portaddr(port, reg)))
#define rd_regl(port, reg) (__raw_readl(portaddr(port, reg)))

#define wr_regl(port, reg, val) \
    do { __raw_writel(val, portaddr(port, reg)); } while(0)
#define wr_regb(port, reg, val) \
    do { __raw_writeb(val, portaddr(port, reg)); } while(0)


/* configuration defines */
#if 0
#define dbg(fmt,arg...) printk("==== tcc uart: "fmt, ##arg);
#else /* no debug */
#define dbg(x...) do {} while(0)
#endif

/* UART name and device definitions */

#define TCC_SERIAL_NAME	    "ttyTCC"
#define TCC_SERIAL_MAJOR	204
#define TCC_SERIAL_MINOR	64

/* conversion functions */
#define tcc_dev_to_port(__dev) (struct uart_port *)dev_get_drvdata(__dev)
#define tx_enabled(port)	((port)->unused[0])
#define rx_enabled(port)	((port)->unused[1])
#define port_used(port)		((port)->unused1)

#if defined(CONFIG_GPS)
extern int gps_k_flag;
#endif 

#if defined(CONFIG_BT)
static int bt_used = 0;
#endif
EXPORT_SYMBOL(bt_used);

static struct clk *ddi_clk;
static struct clk *mali_clk;

#if defined(CONFIG_ARCH_TCC892X)
static unsigned long uartPortCFG0, uartPortCFG1;
#else
static unsigned long uartPortCFG;
#endif
void kerneltimer_registertimer(struct timer_list* ptimer, unsigned long timeover,struct uart_port *port )
{
     init_timer( ptimer );
     ptimer->expires  = get_jiffies_64() + timeover;
     ptimer->data     = (unsigned long)port;
     ptimer->function = (void *)kerneltimer_timeover;
     add_timer( ptimer);
}

void my_uart_rx_process(unsigned long arg)
{
    volatile int current_tail,current_head, i, rts_state;
    struct uart_port *port;
    struct tcc_uart_port *tcc_port;
    unsigned int ch = 0;
    unsigned uerstat = 0, flag;
    struct tty_struct *tty;
    unsigned char *buffer;
    int cur_addr;


    port = (struct uart_port *)arg;
    tcc_port = (struct tcc_uart_port *)port;

    cur_addr = tca_dma_dmacurrentaddress(tcc_port->rx_dma_buffer.dma_ch, (unsigned long *)tcc_port->rx_dma_buffer.dma_core);
    buffer = tcc_port->rx_dma_buffer.addr;
    current_tail = 	tcc_port->rx_dma_tail & (SERIAL_RX_DMA_BUF_SIZE-1);
    current_head = cur_addr & (SERIAL_RX_DMA_BUF_SIZE-1);


    if (current_head != current_tail) {
        if (current_head > current_tail) rts_state = current_head - current_tail;
        else                             rts_state = SERIAL_RX_DMA_BUF_SIZE - current_tail - current_head;

        if (rts_state > (SERIAL_RX_DMA_BUF_SIZE/2)) {
            printk("RTS ON[0x%x : 0x%x]\n", cur_addr, current_tail);
            wr_regl(port, OFFSET_MCR, rd_regl(port, OFFSET_MCR) & ~Hw1);
        }

        spin_lock(&tcc_port->rx_lock);
        tty = port->state->port.tty;

        flag = TTY_NORMAL;
        port->icount.rx++;

        if (uerstat & UART_LSR_BI) {
            port->icount.brk++;
            goto out;
        }

        if (uerstat & UART_LSR_FE)
            port->icount.frame++;
        if (uerstat & UART_LSR_OE)
            port->icount.overrun++;

        uerstat &= port->read_status_mask;

        if (uerstat & UART_LSR_BI)
            flag = TTY_BREAK;
        else if (uerstat & UART_LSR_PE)
            flag = TTY_PARITY;
        else if (uerstat & ( UART_LSR_FE | UART_LSR_OE))
            flag = TTY_FRAME;

        if (current_head < current_tail) current_head += (SERIAL_RX_DMA_BUF_SIZE);

        for(i=current_tail;i<current_head;i++) {
            if (tcc_port->timer_state == 0) break;
            if (i >= SERIAL_RX_DMA_BUF_SIZE)
                ch = buffer[i-SERIAL_RX_DMA_BUF_SIZE];
            else
                ch = buffer[i];

            if (uart_handle_sysrq_char(port, ch))
                goto out;
            /* put the received char into UART buffer */
            uart_insert_char(port, uerstat, UART_LSR_OE, ch, flag);
            tty_flip_buffer_push(tty);
            tcc_port->rx_dma_tail++;
        }

        if (rts_state > SERIAL_RX_DMA_BUF_SIZE/2) {
            printk("RTS OFF\n");
            wr_regl(port, OFFSET_MCR, rd_regl(port, OFFSET_MCR) | Hw1);
        }
out:
    spin_unlock(&tcc_port->rx_lock);

    dbg("[0x%x : 0x%x : 0x%x : 0x%x]\n", buffer, cur_addr, current_head, current_tail);

    }
}

void *kerneltimer_timeover(void *arg )
{
    struct tcc_uart_port *tcc_port = arg;

    int cur_addr = tca_dma_dmacurrentaddress(tcc_port->rx_dma_buffer.dma_ch, (unsigned long *)tcc_port->rx_dma_buffer.dma_core);

    
    if (tcc_port->dma_start_value == cur_addr) {
        if (tcc_port->dma_start_value == 0x20000000) my_uart_rx_process((unsigned long)arg);
    } else {
        if (tcc_port->dma_start_value != 0x20000000) {
            tcc_port->rx_dma_tail = (int) tcc_port->rx_dma_buffer.dma_addr;
            tcc_port->dma_start_value = 0x20000000;
        }
        my_uart_rx_process((unsigned long)arg);
    }

    if (tcc_port->timer_state == 1)
        kerneltimer_registertimer( tcc_port->dma_timer, TIME_STEP,arg );

    return 0;
}

int kerneltimer_init(struct uart_port *port)
{
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;
    tcc_port->dma_timer= kmalloc( sizeof( struct timer_list ), GFP_KERNEL );
    if(tcc_port->dma_timer == NULL ) return -ENOMEM;
    memset( tcc_port->dma_timer, 0, sizeof( struct timer_list) );
    kerneltimer_registertimer( tcc_port->dma_timer,TIME_STEP,port );

    return 0;
}


/* translate a port to the device name */
static inline const char *tcc_serial_portname(struct uart_port *port)
{
	return to_platform_device(port->dev)->name;
}


void tcc_serial_uart_putchar(struct uart_port *port, int ch)
{
	while (!(rd_regl(port, OFFSET_LSR) & LSR_THRE))
		barrier();

    wr_regb(port, OFFSET_THR, ch);
}

static void tcc_serial_uart_push_fifo(struct uart_port *port,int fifosize)
{	
	int i;
	struct circ_buf *xmit = &port->state->xmit;

	while (!(rd_regl(port, OFFSET_LSR) & LSR_THRE))
		barrier();					

    for(i=0;i<fifosize;i++) {
		
        wr_regb(port, OFFSET_THR, xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;

    }
}

static void tcc_serial_tx(struct uart_port *port)
{
    struct circ_buf *xmit = &port->state->xmit;
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

    if(tcc_port->tx_dma_use)
        tcc_port->tx_done = 0;
 
    tcc_port->fifosize = uart_circ_chars_pending(xmit);
    if(tcc_port->fifosize > FIFOSIZE) {
        tcc_port->fifosize = FIFOSIZE;
        if((xmit->tail + tcc_port->fifosize) > UART_XMIT_SIZE ) {
            tcc_port->fifosize = UART_XMIT_SIZE - xmit->tail;
        }
    }

	if( tcc_port->tx_dma_use && (uart_circ_empty(xmit) || uart_tx_stopped(port)) ) {
		wr_regl(port, OFFSET_IER, (rd_regl(port, OFFSET_IER) & ~IER_ETXI));
		tcc_port->tx_done = 1;	//// 100510

		if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
			uart_write_wakeup(port);
		}
        return;
    }

    if (port->x_char) {
        tcc_serial_uart_putchar(port, port->x_char);
        port->icount.tx++;
        port->x_char = 0;
        tcc_port->fifosize--;
    }

	if(tcc_port->tx_dma_use)
	{
	     if(tcc_port->fifosize > 0)
	     {
	         wr_regl(port, OFFSET_UCR, rd_regl(port, OFFSET_UCR) & ~Hw0);
	         memcpy(tcc_port->tx_dma_buffer.addr, (xmit->buf+xmit->tail), tcc_port->fifosize );

	         //DMA Setting
	         tca_dma_setconfig(tcc_port->tx_dma_buffer.dma_ch,
	                           (void *)tcc_port->tx_dma_buffer.dma_addr,
	                           0x1, /* src Param */
	                           portaddr(port, 0x0) - IO_OFFSET,
	                           0x0, /* dest Param */
	                           HwCHCTRL_SYNC_EN        |
	                           HwCHCTRL_TYPE_SL        |
	                           HwCHCTRL_BSIZE_1        |
	                           HwCHCTRL_WSIZE_8        |
	                           HwCHCTRL_IEN_ON         |
	                           HwCHCTRL_FLAG           ,
                               tcc_port->fifosize,
                               tcc_port->tx_dma_buffer.dma_port,
                               tcc_port->tx_dma_buffer.dma_mode, /* mode */
                               (unsigned long *)tcc_port->tx_dma_buffer.dma_core);

            wr_regl(port, OFFSET_UCR, rd_regl(port, OFFSET_UCR) |Hw0);
        }
    }
    else {
        while (tcc_port->fifosize > 0){
            if (uart_circ_empty(xmit)|| uart_tx_stopped(port)) {
                wr_regl(port, OFFSET_IER, (rd_regl(port, OFFSET_IER) & ~IER_ETXI));
                break;
            }
            else {
		#if 1
		tcc_serial_uart_push_fifo(port, tcc_port->fifosize);
		tcc_port->fifosize = 0;
	        break;
		#else
                tcc_serial_uart_putchar(port, xmit->buf[xmit->tail]);
                xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
                port->icount.tx++;
                tcc_port->fifosize--;
		#endif
            }
        } //// while
        if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
            uart_write_wakeup(port);
        }
	}

}


static void tcc_serial_stop_tx(struct uart_port *port)
{
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

	dbg("%s\n", __func__);

    tcc_port->tx_done = 1;

    while(!((rd_regl(port, OFFSET_LSR)) & LSR_TEMT))
        continue;

    if(tx_enabled(port)){
        disable_irq(port->irq);
        tx_enabled(port) = 0;
    }
}

static void tcc_serial_start_tx(struct uart_port *port)
{
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

    if(!tx_enabled(port)) {
        enable_irq(port->irq);
        tx_enabled(port) = 1;
    }

	if (tcc_port->tx_dma_use) {
		while (!(rd_regl(port, OFFSET_LSR) & LSR_THRE))
			barrier();

        if(tcc_port->tx_done) {
			tcc_serial_tx(port);
			wr_regl(port, OFFSET_IER, rd_regl(port, OFFSET_IER) | IER_ETXI);
		}
	}
	else {
		tcc_serial_tx(port);
		wr_regl(port, OFFSET_IER, rd_regl(port, OFFSET_IER) | IER_ETXI);
	}
}


static void tcc_serial_stop_rx(struct uart_port *port)
{
	dbg("%s  line[%d] irq[%d]\n", __func__, port->line, port->irq);
    u32 ier;

    if (rx_enabled(port)) {
        ier = rd_regl(port, OFFSET_IER);
        wr_regl(port, OFFSET_IER, (ier & ~IER_ERXI));

        disable_irq(port->irq);
        mdelay(10);
        rx_enabled(port) = 0;
     }
}

static void tcc_serial_enable_ms(struct uart_port *port)
{
    dbg("%s\n", __func__);
}

static irqreturn_t tcc_serial_interrupt_dma(int irq, void *id)
{
    struct uart_port *port = id;
    struct tcc_uart_port *tcc_port = id;
    struct circ_buf *xmit = &port->state->xmit;

    spin_lock(&port->lock);
    if (tcc_port->tx_dma_use) {
        /* XXX */
        xmit->tail = (xmit->tail + tcc_port->fifosize) & (UART_XMIT_SIZE - 1);
        port->icount.tx+= tcc_port->fifosize;
        tcc_port->fifosize = 0;

        tca_dma_clrien(tcc_port->tx_dma_buffer.dma_ch, (unsigned long *)tcc_port->tx_dma_buffer.dma_core);
        tca_serial_dmaclrinterrupt(tcc_port->tx_dma_buffer.dma_ch, (unsigned long *)tcc_port->tx_dma_buffer.dma_core);

		tcc_serial_tx(port);

	    if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS){
			uart_write_wakeup(port);
	    }
    }
    spin_unlock(&port->lock);
	return IRQ_HANDLED;
}

static irqreturn_t tcc_serial_interrupt(int irq, void *id)
{
    unsigned int ch = 0;
    struct uart_port *port = id;
    struct tcc_uart_port *tcc_port = id;
    unsigned uerstat, flag;
    struct tty_struct *tty = port->state->port.tty;
    unsigned int iir_data = 0;

    uerstat = 0;

    spin_lock(&port->lock);

    iir_data = rd_regl(port, OFFSET_IIR);
    iir_data = (iir_data & 0x0E) >> 1;
    if (iir_data == 2 || iir_data == 6) {
        ch = rd_regb(port, OFFSET_RBR);

        if(!tcc_port->rx_dma_use)
        {
            flag = TTY_NORMAL;
            port->icount.rx++;

		uerstat = rd_regl(port, OFFSET_LSR);

            if (uerstat & UART_LSR_BI) {
                dbg("break!\n");
                port->icount.brk++;
                goto out;
            }

            if (uerstat & UART_LSR_FE)
                port->icount.frame++;

            if (uerstat & UART_LSR_OE){
        	   printk("*************UART_LSR_OE!!!!!!!!!!!\n" );
                port->icount.overrun++;
            	}

            uerstat &= port->read_status_mask;

            if (uerstat & UART_LSR_BI)
                flag = TTY_BREAK;
            else if (uerstat & UART_LSR_PE)
                flag = TTY_PARITY;
            else if (uerstat & ( UART_LSR_FE | UART_LSR_OE))
                flag = TTY_FRAME;

            if (uart_handle_sysrq_char(port, ch))
                goto out;
            /* put the received char into UART buffer */
            uart_insert_char(port, uerstat, UART_LSR_OE, ch, flag);

            tty_flip_buffer_push(tty);

        }
    }
    else if (iir_data == 1) {
        if (tcc_port->tx_dma_use) {
            while (!(rd_regl(port, OFFSET_LSR) & LSR_THRE))
                barrier();
        }
        else {
            tcc_serial_tx(port);
        }
    }

out:
    spin_unlock(&port->lock);
	return IRQ_HANDLED;
}

static unsigned int tcc_serial_tx_empty(struct uart_port *port)
{
	unsigned short lsr;

	lsr = rd_regl(port, OFFSET_LSR);
	if (lsr & LSR_TEMT)
		return TIOCSER_TEMT;
	else
		return 0;

}

static unsigned int tcc_serial_get_mctrl(struct uart_port *port)
{
	dbg("%s\n", __func__);
    return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

static void tcc_serial_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	dbg("%s\n", __func__);
	/* todo - possibly remove AFC and do manual CTS */
}

static void tcc_serial_break_ctl(struct uart_port *port, int break_state)
{
	dbg("%s\n", __func__);
}

static void tcc_serial_shutdown(struct uart_port *port)
{
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

    // clock control for BT
    if(tcc_port->bt_use) {
       #if defined(CONFIG_BT)
	bt_used = 0;
       #endif
    }

    dbg("%s\n", __func__);
    if(tcc_port->rx_dma_use)
    {
    #if ! defined(CONFIG_ARCH_TCC92XX)
	clk_disable(ddi_clk);
    #endif
	clk_disable(mali_clk);
        tcc_port->timer_state=0;
        mdelay(1);
        del_timer(tcc_port->dma_timer);
        kfree(tcc_port->dma_timer);
     }

    wr_regl(port, OFFSET_IER, 0x0);
    free_irq(port->irq, port);

    if(tcc_port->tx_dma_use)
    	free_irq(tcc_port->tx_dma_buffer.dma_intr, port);

	port_used(port) = 0;	// for suspend/resume

    dbg("%s   line[%d] out...\n", __func__, port->line);
}


/* 
 * while application opening the console device, this function will invoked
 * This function will initialize the interrupt handling
 */
static int tcc_serial_startup(struct uart_port *port)
{
	int retval=0;
	unsigned int lcr;

    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

    dbg("%s() line[%d] in...\n", __func__, port->line);

    if(tcc_port->bt_use){
        #if defined(CONFIG_BT)
	    bt_used = 1;
        #endif
	    //tcc_pca953x_setup(PCA9539_U3_SLAVE_ADDR, BT_ON, OUTPUT, HIGH, SET_DIRECTION|SET_VALUE);
	    dbg("tcc_uart: BT power ON\n");
		gpio_request(TCC_GPEXT1(7), "BT ON");						
		gpio_direction_output(TCC_GPEXT1(7), 1);
	}

    tx_enabled(port) = 1;
    rx_enabled(port) = 1;
    port_used(port) = 1;	// for suspend/resume

    // clock control for BT

#if defined(CONFIG_TCC_BT_DEV)
    if(tcc_port->bt_suspend != 1){
        if(port->line == 0)
            tca_serial_portinit(UART_NOT_FLOW_CONTROL , port->line);
        else
            tca_serial_portinit(UART_FLOW_CONTROL, port->line);
    }
#else
    if(port->line == 0)
        tca_serial_portinit(UART_NOT_FLOW_CONTROL , port->line);
    else
        tca_serial_portinit(UART_FLOW_CONTROL , port->line);
#endif



    /* clear interrupt */
    wr_regl(port, OFFSET_IER, 0x0);

    lcr = rd_regl(port, OFFSET_LCR);
    wr_regl(port, OFFSET_LCR, (lcr | LCR_DLAB));

    wr_regl(port, OFFSET_FCR, (FCR_TXFR|FCR_RXFR|FCR_FE));    /* FIFO Enable, Rx/Tx FIFO reset */
    tcc_port->fifosize = FIFOSIZE;
    tcc_port->reg.bFCR = 0x07;          /* for resume restore */

    if(tcc_port->tx_dma_use)
        tcc_port->tx_done  = 1;

    wr_regl(port, OFFSET_LCR, (lcr & (~LCR_DLAB)));

    rd_regl(port, OFFSET_IIR);

    retval = request_irq(port->irq, tcc_serial_interrupt, IRQF_SHARED, tcc_port->name , port);
    dbg("request serial irq:%d,retval:%d\n", port->irq, retval);

    if(tcc_port->tx_dma_use) {
    	retval = request_irq(tcc_port->tx_dma_buffer.dma_intr, tcc_serial_interrupt_dma, IRQF_SHARED, "uart1_dma" , port);
        dbg("request serial dma irq:%d,retval:%d\n", tcc_port->tx_dma_buffer.dma_intr, retval);
    }


    /* Rx DMA */
    if(tcc_port->rx_dma_use) {
    	#if ! defined(CONFIG_ARCH_TCC92XX)
	    clk_enable(ddi_clk);
	#endif
	clk_enable(mali_clk);
        int cur_addr = tca_dma_dmacurrentaddress(tcc_port->rx_dma_buffer.dma_ch, (unsigned long *)tcc_port->rx_dma_buffer.dma_core);

        dbg("rx_dma_tail : %x\n",tcc_port->rx_dma_tail);

        wr_regl(port, OFFSET_MCR, rd_regl(port, OFFSET_MCR) | MCR_AFE | MCR_RTS);
        wr_regl(port, OFFSET_AFT, 0x00000021);

        // Set Source Address & Source Parameter (mask + increment)
        wr_regl(port, OFFSET_UCR, rd_regl(port, OFFSET_UCR) & ~UCR_RxDE);

        tca_dma_setconfig(tcc_port->rx_dma_buffer.dma_ch,
                          portaddr(port, 0x0) - IO_OFFSET,
                          0x0, /* src Param */
                          (void *)tcc_port->rx_dma_buffer.dma_addr,
                          0x1, /* dest Param */
                          //HwCHCTRL_CONT_C       |
                          HwCHCTRL_SYNC_EN        |
			  //HwCHCTRL_BST_BURST	    |
			  HwCHCTRL_HRD_WR	  |
                          //HwCHCTRL_TYPE_SL        |
                          HwCHCTRL_BSIZE_1        |
                          HwCHCTRL_WSIZE_8        |
                       	  //HwCHCTRL_IEN_ON       |
                          HwCHCTRL_REP_EN         |
                          HwCHCTRL_FLAG           ,
                          tcc_port->rx_dma_buffer.buf_size,
                          tcc_port->rx_dma_buffer.dma_port,
                          tcc_port->rx_dma_buffer.dma_mode, /* mode */
                          (unsigned long *)tcc_port->rx_dma_buffer.dma_core);

        wr_regl(port, OFFSET_UCR, rd_regl(port, OFFSET_UCR) | UCR_RxDE);

        tcc_port->dma_start_value = cur_addr;
        if (cur_addr != 0x20000000)
            tcc_port->rx_dma_tail = cur_addr;
        else
            tcc_port->rx_dma_tail = (int) tcc_port->rx_dma_buffer.dma_addr;
        tcc_port->timer_state=1;

        kerneltimer_init(port);
    }

    wr_regl(port, OFFSET_IER, IER_ERXI);

    if(tcc_port->rx_dma_use) {
		dbg("line %d IER tx Only\n", port->line);
		wr_regl(port, OFFSET_IER, 0);
    }

    //volatile PIOBUSCFG pIOBUSCFG = (volatile PIOBUSCFG)(tcc_p2v(HwIOBUSCFG_BASE));
    //dbg(" HCLKEN0[0x%08x]\n", pIOBUSCFG->HCLKEN0);

	tcc_port->opened = 1;
	
    dbg(" %s() out...\n", __func__);
	return retval;
}


/* power power management control */
static void tcc_serial_pm(struct uart_port *port, unsigned int level, unsigned int old)
{
}

static void tcc_serial_set_baud(struct tcc_uart_port *tcc_port, unsigned int baud)
{
    /* Set UARTx peripheral clock */
    switch(baud) {
	case 921600:
	case 115200:
        case 57600:
        case 38400:
        case 19200:
        case 14400:
        case 9600:
            clk_set_rate(tcc_port->clk, 29491200);    // 29.491MHz
            break;
        default :
	      clk_set_rate(tcc_port->clk, 48*1000*1000);    // 48MHz
            break;
    }
}

static void tcc_serial_set_termios(struct uart_port *port,
				       struct ktermios *termios,
				       struct ktermios *old)
{
	unsigned long flags;
	unsigned int baud, quot;
	unsigned int ulcon;
	unsigned int umcon, lsr;
    int uart_clk = 0;
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

	/*
	 * We don't support modem control lines.
	 */
	termios->c_cflag &= ~(HUPCL | CMSPAR);
	termios->c_cflag |= CLOCAL;


	/* Ask the core to calculate the baud rate. */
	baud = uart_get_baud_rate(port, termios, old, 0, 3500000);

    if(tcc_port->port.line != CONSOLE_PORT) {
        if(baud != port->uartclk)
            tcc_serial_set_baud(tcc_port, baud);
        port->uartclk = baud;
    }

	/*
	 * Ask the core to calculate the divisor for us.
	 */
	 
   uart_clk = (unsigned int)tca_ckc_getperi(port->line + PERI_UART0);	
	quot = ((uart_clk*100) + ((16*baud)>>1))/(16*baud);	

    /*
     * set byte size
     */
	switch (termios->c_cflag & CSIZE) {
	case CS5:
		dbg("config: 5bits/char  cflag[0x%x]\n", termios->c_cflag );
		ulcon = 0;
		break;
	case CS6:
		dbg("config: 6bits/char  cflag[0x%x]\n", termios->c_cflag );
		ulcon = 1;
		break;
	case CS7:
		dbg("config: 7bits/char  cflag[0x%x]\n", termios->c_cflag );
		ulcon = 2;
		break;
	case CS8:
	default:
		dbg("config: 8bits/char  cflag[0x%x]\n", termios->c_cflag );
		ulcon = 3;
		break;
	}

	/* preserve original lcon IR settings */
	if (termios->c_cflag & CSTOPB)
		ulcon |= LCR_STB;   /* HwUART_LCR_STB_ONE */

	umcon = (termios->c_cflag & CRTSCTS) ? (MCR_AFE|MCR_RTS) : MCR_RTS; /* HwUART_MCR_RTS_ON */

	if (termios->c_cflag & PARENB) {
		if (termios->c_cflag & PARODD)
			ulcon |= (LCR_PEN);
		else
			ulcon |= (LCR_EPS|LCR_PEN);
	} else {
		ulcon &= ~(LCR_EPS|LCR_PEN);
	}

	spin_lock_irqsave(&port->lock, flags);

	do {
		lsr = rd_regl(port, OFFSET_LSR);
	} while (!(lsr & LSR_TEMT));

	printk("[UART%02d] setting ulcon: %08x, umcon: %08x, brddiv to %d, baud %d, uart_clk %d\n", port->line, ulcon, umcon, quot, baud, uart_clk);

    wr_regl(port, OFFSET_MCR, umcon);
    wr_regl(port, OFFSET_LCR, (ulcon | LCR_DLAB));
    if(quot > 0xFF) {
        wr_regl(port, OFFSET_DLL, quot & 0x00FF);
        wr_regl(port, OFFSET_DLM, quot >> 8);
    }else if (quot > 0) {
        wr_regl(port, OFFSET_DLL, quot);
        wr_regl(port, OFFSET_DLM, 0x0);
    }
    wr_regl(port, OFFSET_LCR, (ulcon & (~LCR_DLAB)));

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	/*
	 * Which character status flags are we interested in?
	 */
	port->read_status_mask = 0;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= 0;

	/*
	 * Which character status flags should we ignore?
	 */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= 0;
	if (termios->c_iflag & IGNBRK && termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= 0;

	/*
	 * Ignore all characters if CREAD is not set.
	 */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |= 0;

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *tcc_serial_type(struct uart_port *port)
{
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;
    
    return tcc_port->name;
}

#define MAP_SIZE (0x100)

static void tcc_serial_release_port(struct uart_port *port)
{
    /* TODO */
	//release_mem_region(port->mapbase, MAP_SIZE);
}

static int tcc_serial_request_port(struct uart_port *port)
{
    return 0;
    /*
	return request_mem_region(port->mapbase, MAP_SIZE, "tcc7901") ? 0 : -EBUSY;
    */
}

static void tcc_serial_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE && tcc_serial_request_port(port) == 0)
        port->type = PORT_TCC;
}

/*
 * verify the new serial_struct (for TIOCSSERIAL).
 */
static int tcc_serial_verify_port(struct uart_port *port, struct serial_struct *ser)
{
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;
    
    if(tcc_port->info == NULL)
        return -EINVAL;
        
	if (ser->type != PORT_UNKNOWN && ser->type != tcc_port->info->type)
		return -EINVAL;
	return 0;
}


#ifdef CONFIG_SERIAL_TCC_CONSOLE
static struct console tcc_serial_console;
#define TCC_SERIAL_CONSOLE      &tcc_serial_console
#else
#define TCC_SERIAL_CONSOLE      NULL
#endif


static struct uart_ops tcc_serial_ops = {
	.pm				= tcc_serial_pm,
	.tx_empty		= tcc_serial_tx_empty,
	.get_mctrl		= tcc_serial_get_mctrl,
	.set_mctrl		= tcc_serial_set_mctrl,
	.stop_tx		= tcc_serial_stop_tx,
	.start_tx		= tcc_serial_start_tx,
	.stop_rx		= tcc_serial_stop_rx,
	.enable_ms		= tcc_serial_enable_ms,
	.break_ctl		= tcc_serial_break_ctl,
	.startup		= tcc_serial_startup,
	.shutdown		= tcc_serial_shutdown,
	.set_termios	= tcc_serial_set_termios,
	.type			= tcc_serial_type,
	.release_port	= tcc_serial_release_port,
	.request_port	= tcc_serial_request_port,
	.config_port	= tcc_serial_config_port,
	.verify_port	= tcc_serial_verify_port,
};


static struct uart_driver tcc_uart_drv = {
	.owner          = THIS_MODULE,
	.dev_name       = TCC_SERIAL_NAME,
	.nr             = NR_PORTS,
	.cons           = TCC_SERIAL_CONSOLE,
	.driver_name    = DRV_NAME,
	.major          = TCC_SERIAL_MAJOR,
	.minor          = TCC_SERIAL_MINOR,
};

/*  initialise  serial port information */
/* cpu specific variations on the serial port support */
static struct tcc_uart_info tcc_uart_inf = {
	.name           = "Telechips UART",
	.type           = PORT_TCC,
	.fifosize       = FIFOSIZE,
};

static void *tcc_free_dma_buf(tcc_dma_buf_t *dma_buf)
{
	dbg("%s\n", __func__);
    if (dma_buf) {
        if (dma_buf->dma_addr != 0) {
            dma_free_writecombine(0, dma_buf->buf_size, dma_buf->addr, dma_buf->dma_addr);
        }
        memset(dma_buf, 0, sizeof(tcc_dma_buf_t));
    }
    return NULL;
}

static void *tcc_malloc_dma_buf(tcc_dma_buf_t *dma_buf, int buf_size)
{
	dbg("%s\n", __func__);
    if (dma_buf) {
        tcc_free_dma_buf(dma_buf);
        dma_buf->buf_size = buf_size;
        dma_buf->addr = dma_alloc_writecombine(0, dma_buf->buf_size, &dma_buf->dma_addr, GFP_KERNEL);
        dbg("Malloc DMA buffer @0x%X(Phy=0x%X), size:%d\n",
               (unsigned int)dma_buf->addr,
               (unsigned int)dma_buf->dma_addr,
               dma_buf->buf_size);
        return dma_buf->addr;
    }
    return NULL;
}


static int tcc_serial_remove(struct platform_device *dev)
{
	struct uart_port *port = tcc_dev_to_port(&dev->dev);
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

	if (port)
		uart_remove_one_port(&tcc_uart_drv, port);

    if(tcc_port->tx_dma_use)
        tcc_free_dma_buf(&(tcc_port->tx_dma_buffer));

    if(tcc_port->rx_dma_use)
    {
        tcc_free_dma_buf(&(tcc_port->rx_dma_buffer));
        wr_regl(port, OFFSET_UCR, rd_regl(port, OFFSET_UCR) & ~Hw1);

        // DMA channel is disabled.
        tca_dma_clren(tcc_port->rx_dma_buffer.dma_ch, (unsigned long *)tcc_port->rx_dma_buffer.dma_core);
    }

    if(tcc_port->clk != NULL) {
    	clk_disable(tcc_port->clk);
    	clk_put(tcc_port->clk);
    }

	return 0;
}

/* UART power management code */

#ifdef CONFIG_PM
/*-------------------------------------------------
 * TODO: handling DMA_PORT suspend/resume (TCC79X)
 *       DMA stop and start ...
 *-------------------------------------------------*/
static int tcc_serial_suspend(struct platform_device *dev, pm_message_t state)
{
    struct uart_port *port = tcc_dev_to_port(&dev->dev);
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

#if defined(CONFIG_ARCH_TCC892X)
    uartPortCFG0 = *(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE);
    uartPortCFG1 = *(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE + 0x4);
#else
    uartPortCFG = *(volatile unsigned long *)tcc_p2v(HwUART_CHSEL);
#endif
    dbg("%s in...\n", __func__);

    if (port) {
        //port->suspended = 1;

        uart_suspend_port(&tcc_uart_drv, port);
        if(port->line == TCC_GPS_UART){
#if defined(CONFIG_GPS)
            if(gps_k_flag){
#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8920)
                if(machine_is_m801_88() || machine_is_m803()) // demo set
                {
                    gpio_set_value(TCC_GPG(4), 0);
                }
                else if(machine_is_tcc8800() || machine_is_tcc8920())
                {
                    gpio_direction_output(TCC_GPEXT1(6), 0);
                }
#elif defined(CONFIG_MACH_TCC8900)
                if(machine_is_tcc8900())
                {
                    gpio_set_value(TCC_GPD(25), 0);
                }
#elif defined(CONFIG_MACH_M805_892X)
                if(machine_is_m805_892x())
                {
                    if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
                        gpio_set_value(TCC_GPE(14), 0);
                    else
                        gpio_set_value(TCC_GPC(6), 0);
                }
#endif
            }
#endif
        }
        if(tcc_port->tx_dma_use) {
            //tcc_pca953x_setup(PCA9539_U3_SLAVE_ADDR, BT_ON, OUTPUT, LOW, SET_DIRECTION|SET_VALUE);
        }

#if defined(CONFIG_ARCH_TCC892X)

    #if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
    if(!port->cons || (port->cons->index != port->line)){
    #endif
        if(tca_serial_clock_disable(tcc_port, dev->id))
            return -EINVAL;

	if(tca_ckc_getiopwdn(RB_UART_CONTROLLER)){
		if(tcc_port->opened == 1){
			tca_serial_port_pullup(port->line , 1, uartPortCFG0|((unsigned long long)uartPortCFG1<<32));
		}
	}

    #if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
    }
    #endif
#endif

    }
    dbg("%s out...\n", __func__);
	return 0;
}

static int tcc_serial_resume(struct platform_device *dev)
{
    struct uart_port *port = tcc_dev_to_port(&dev->dev);
    struct tcc_uart_port *tcc_port = (struct tcc_uart_port *)port;

#if defined(CONFIG_ARCH_TCC892X)
    #if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
    if(!port->cons || (port->cons->index != port->line)){
    #endif
		if(tcc_port->opened == 1){
			tca_serial_port_pullup(port->line , 0, uartPortCFG0|((unsigned long long)uartPortCFG1<<32));
		}

        if(tca_serial_clock_enable(tcc_port, dev->id))
            return -EINVAL;

    #if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
    }
    #endif
#endif

#if defined(CONFIG_ARCH_TCC892X)
    *(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE) = uartPortCFG0;
    *(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE + 0x4) = 	uartPortCFG1;
#else
    *(volatile unsigned long *)tcc_p2v(HwUART_CHSEL) = uartPortCFG ;
#endif

    if(port) {
        if (port->suspended) {

#if defined(CONFIG_TCC_BT_DEV)
            if(tcc_port->bt_use == 1)
                tcc_port->bt_suspend = 1;
#endif
            if(tcc_port->tx_dma_use){
                //tcc_pca953x_setup(PCA9539_U3_SLAVE_ADDR, BT_ON, OUTPUT, HIGH, SET_DIRECTION|SET_VALUE);
            }

            uart_resume_port(&tcc_uart_drv, port);

            if(port->line == TCC_GPS_UART){
#if defined(CONFIG_GPS)
                if(gps_k_flag){
#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8920)
                    if(machine_is_m801_88() || machine_is_m803()) // demo set
                    {
                        gpio_set_value(TCC_GPG(4), 1);
                    }
                    else if(machine_is_tcc8800() || machine_is_tcc8920())
                    {
                        gpio_direction_output(TCC_GPEXT1(6), 1);
                    }
#elif defined(CONFIG_MACH_TCC8900)
                    if(machine_is_tcc8900())
                    {
                        gpio_set_value(TCC_GPD(25), 1);
                    }
#elif defined(CONFIG_MACH_M805_892X)
                    if(machine_is_m805_892x())
                    {
                        if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
                            gpio_set_value(TCC_GPE(14), 1);
                        else
                            gpio_set_value(TCC_GPC(6), 1);
                    }
#endif      
                }
#endif
            }

#if defined(CONFIG_TCC_BT_DEV)
            if(tcc_port->bt_use == 1)
                tcc_port->bt_suspend = 0;
#endif

            port->suspended = 0;
        }
	
    }


    dbg("%s out...\n", __func__);
	return 0;
}

#else
#define tcc_serial_suspend NULL
#define tcc_serial_resume  NULL
#endif

static int tcc_serial_probe(struct platform_device *dev)
{
    int ret;
    struct resource *mem, *irq;
    struct tcc_uart_port *tcc_port;
    struct tcc_uart_platform_data *tcc_platform_data;


    dbg("\n\n%s: dev->id = %d \n", __func__, dev->id);

    ddi_clk = clk_get(0, "lcdc0");
    mali_clk = clk_get(0, "mali_clk");

    mem = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&dev->dev, "[UART%d] no memory resource?\n", dev->id);
		return -EINVAL;
	}
	irq = platform_get_resource(dev, IORESOURCE_IRQ, 0);
	if (!irq) {
		dev_err(&dev->dev, "[UART%d] no irq resource?\n", dev->id);
		return -ENODEV;
	}
    tcc_platform_data = (struct tcc_uart_platform_data *)dev->dev.platform_data;


    tcc_port = &tcc_serial_ports[dev->id];


    /* Bus Clock Enable of UARTx */
    if(tca_serial_clock_enable(tcc_port, dev->id))
        return -EINVAL;

    /* Set Interrrupt */
    tca_serial_intrinit();

	dbg("initialising uart ports...\n");

    tcc_port->bt_use     = 0;
    tcc_port->tx_dma_use = 0;
    tcc_port->rx_dma_use = 0;

	tcc_port->port.iotype   = UPIO_MEM;
    tcc_port->port.irq      = irq->start;
    tcc_port->port.uartclk  = 0;
	tcc_port->port.flags    = UPF_BOOT_AUTOCONF;
	tcc_port->port.ops      = &tcc_serial_ops;
	tcc_port->port.fifosize = FIFOSIZE;
	tcc_port->port.line     = dev->id;
	tcc_port->port.dev      = &dev->dev;
	tcc_port->port.type     = PORT_TCC;
	tcc_port->port.mapbase  = mem->start;
	tcc_port->port.membase  = (unsigned char __iomem *)tcc_port->port.mapbase;
    tcc_port->port.irq      = platform_get_irq(dev, 0);
    tcc_port->baud          = 0;
    tcc_port->info          = &tcc_uart_inf;

    init_waitqueue_head(&(tcc_port->wait_q));

    ret = uart_add_one_port(&tcc_uart_drv, &tcc_port->port);
    if (ret){
        dbg("uart_add_one_port failure\n");
        goto probe_err;
    }

    spin_lock_init(&tcc_port->rx_lock);

    platform_set_drvdata(dev, &tcc_port->port);


    if(tcc_platform_data == NULL)
        return ret;

    tcc_port->bt_use = tcc_platform_data->bt_use;

    if(tcc_platform_data->tx_dma_use) {
        if (!tcc_malloc_dma_buf(&(tcc_port->tx_dma_buffer), tcc_platform_data->tx_dma_buf_size)) {
            dbg("Unable to attach UART TX DMA 1 channel\n");
            ret = -ENOMEM;
            goto probe_err;
        }

        tcc_port->tx_dma_use = 1;
        tcc_port->tx_dma_buffer.dma_core = tcc_platform_data->tx_dma_base;
        tcc_port->tx_dma_buffer.dma_ch   = tcc_platform_data->tx_dma_ch;
        tcc_port->tx_dma_buffer.dma_port = dev->id;
        tcc_port->tx_dma_buffer.dma_intr = tcc_platform_data->tx_dma_intr;
        tcc_port->tx_dma_buffer.dma_mode = tcc_platform_data->tx_dma_mode;
        tcc_port->tx_dma_buffer.buf_size = tcc_platform_data->tx_dma_buf_size;

        init_waitqueue_head(&(tcc_port->wait_dma_q));
    }

    if(tcc_platform_data->rx_dma_use)
    {
        if(!tcc_malloc_dma_buf(&(tcc_port->rx_dma_buffer), tcc_platform_data->rx_dma_buf_size)) {
            dbg("Unable to attach UART RX DMA 1 channel\n");
            ret = -ENOMEM;
            goto probe_err;
        }

        tcc_port->rx_dma_use = 1;
        tcc_port->rx_dma_buffer.dma_core = tcc_platform_data->rx_dma_base;
        tcc_port->rx_dma_buffer.dma_ch   = tcc_platform_data->rx_dma_ch;
        tcc_port->rx_dma_buffer.dma_port = dev->id;
        tcc_port->rx_dma_buffer.dma_intr = tcc_platform_data->rx_dma_intr;
        tcc_port->rx_dma_buffer.dma_mode = tcc_platform_data->rx_dma_mode;
        tcc_port->rx_dma_buffer.buf_size = tcc_platform_data->rx_dma_buf_size;
        tcc_port->rx_dma_tail = (int) tcc_port->rx_dma_buffer.dma_addr;
    }

    return ret;

probe_err:
    dbg("probe_err\n");
    if(tcc_port == NULL)        return ret;

    if(tcc_port->clk != NULL)   clk_disable(tcc_port->clk);
    if(tcc_port->tx_dma_use)    tcc_free_dma_buf(&(tcc_port->tx_dma_buffer));
    if(tcc_port->rx_dma_use)    tcc_free_dma_buf(&(tcc_port->rx_dma_buffer));

    return ret;
}

static struct platform_driver tcc_serial_drv = {
	.probe			= tcc_serial_probe,
	.remove			= tcc_serial_remove,
	.suspend		= tcc_serial_suspend,
	.resume			= tcc_serial_resume,
	//	.suspend_late	= tcc_serial_suspend,
	//	.resume_early	= tcc_serial_resume,
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};


/* module initialisation code */
static int __init tcc_serial_modinit(void)
{
	int ret;

	ret = uart_register_driver(&tcc_uart_drv);
	if (ret < 0) {
		dbg(KERN_ERR "failed to register UART driver\n");
		return ret;
	}

    ret = platform_driver_register(&tcc_serial_drv);
    if(ret != 0) {
        uart_unregister_driver(&tcc_uart_drv);
        return ret;
    }

	return 0;
}

static void __exit tcc_serial_modexit(void)
{
	platform_driver_unregister(&tcc_serial_drv);
	uart_unregister_driver(&tcc_uart_drv);
}

module_init(tcc_serial_modinit);
module_exit(tcc_serial_modexit);










/*********************************************************************************************************
 *
 * The following is Console driver
 *
 *********************************************************************************************************/
#ifdef CONFIG_SERIAL_TCC_CONSOLE

static struct uart_port *cons_uart;

static void tcc_serial_console_putchar(struct uart_port *port, int ch)
{
	while (!(rd_regl(port, OFFSET_LSR) & LSR_THRE))
		barrier();

    wr_regb(port, OFFSET_THR, ch);

}


static void tcc_console_write(struct console *co, const char *s,
			     unsigned int count)
{
	struct uart_port *port;
	unsigned int t_ier, b_ier;

	port = &tcc_serial_ports[co->index].port;

    t_ier = rd_regl(port, OFFSET_IER);
    b_ier = t_ier;

    wr_regl(port, OFFSET_IER, t_ier & ~IER_ETXI);
    uart_console_write(cons_uart, s, count, tcc_serial_console_putchar);
    wr_regl(port, OFFSET_IER, b_ier);
}

static void __init tcc_serial_get_options(struct uart_port *port, int *baud,
			   int *parity, int *bits)
{
}

static int __init tcc_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	int baud = CONSOLE_BAUDRATE;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	dbg("tcc_serial_console_setup: co=%p (%d), %s\n", co, co->index, options);

	/* is this a valid port */
	if (co->index == -1 || co->index >= NR_PORTS)
		co->index = CONSOLE_PORT;

	port = &tcc_serial_ports[co->index].port;
	port->ops = &tcc_serial_ops;

	/* is the port configured? */
	if (port->mapbase == 0x0) {
        dbg("port->mapbase is 0\n");
        port->mapbase = tcc_serial_ports[co->index].base_addr;
        port->membase = (unsigned char __iomem *)port->mapbase;
		port = &tcc_serial_ports[co->index].port;
        port->ops = &tcc_serial_ops;
	}

	cons_uart = port;

	if (options) {
        dbg("uart_parse_options\n");
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	} else {
		dbg("tcc_serial_get_options\n");
		tcc_serial_get_options(port, &baud, &parity, &bits);
	}

	dbg("tcc_serial_console_setup: port=%p (%d)\n", port, co->index);
	return uart_set_options(port, co, baud, parity, bits, flow);
}


/* tcc_console_init
 *
 * initialise the console from one of the uart drivers
*/
static struct console tcc_serial_console =
{
	.name		= TCC_SERIAL_NAME,
	.device		= uart_console_device,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.write		= tcc_console_write,
	.setup		= tcc_console_setup,
};

static int tcc_console_init(void)
{
	dbg("%s\n", __func__);

	tcc_serial_console.data = &tcc_uart_drv;
	register_console(&tcc_serial_console);

	return 0;
}

console_initcall(tcc_console_init);
#endif /* CONFIG_SERIAL_TCC_CONSOLE */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("Telechips TCC Serial port driver");
