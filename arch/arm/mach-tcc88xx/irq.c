/*
 * linux/arch/arm/mach-tcc88xx/irq.c
 *
 * Author:  <linux@telechips.com>
 * Created: August, 2010
 * Description: Interrupt handler for Telechips TCC8800 chipset
 *
 * Copyright (C) Telechips, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>

#include <linux/agpgart.h>
#include <linux/types.h>

#include <asm/irq.h>
#include <asm/mach/irq.h>
#include <asm/io.h>

#include <mach/bsp.h>

// Global
static volatile PPIC pPIC;
static volatile PVIC pVIC;
static volatile PGPSBPORTCFG pPGPSBPORTCFG;
static volatile PUARTPORTMUX pUARTPORTMUX;
static volatile PGDMACTRL pPGDMACTRL0, pPGDMACTRL1, pPGDMACTRL2, pPGDMACTRL3;
static volatile PTIMER pTIMER;

/******************************************
 * Disable IRQ
 *
 * If mask_ack exist, this is not called.
 *****************************************/
static void tcc8800_mask_irq(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq < 32) {
        BITCLR(pPIC->INTMSK0,   (1 << irq));
    } else {
        BITCLR(pPIC->INTMSK1,   (1 << (irq - 32)));
    }
}

static void tcc8800_mask_irq_uart(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_UART) {
        BITCLR(pPIC->INTMSK1, Hw15);
    }
}

static void tcc8800_mask_irq_gpsb(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_GPSB) {
        BITCLR(pPIC->INTMSK1, Hw4);
    }
}

static void tcc8800_mask_irq_dma(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_GDMA) {
        BITCLR(pPIC->INTMSK0, Hw29);
    }
}

static void tcc8800_mask_irq_tc0(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_TC0) {
        BITCLR(pPIC->INTMSK0, Hw0);
    }
}

/******************************************
 * Enable IRQ
 *****************************************/
static void tcc8800_irq_enable(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq < 32) {
        BITSET(pPIC->CLR0,      (1 << irq));
        BITSET(pPIC->IEN0,      (1 << irq));
        BITSET(pPIC->INTMSK0,   (1 << irq));
    } else {
        BITSET(pPIC->CLR1,      (1 << (irq - 32)));
        BITSET(pPIC->IEN1,      (1 << (irq - 32)));
        BITSET(pPIC->INTMSK1,   (1 << (irq - 32)));
    }
}

static void tcc8800_unmask_irq(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq < 32) {
        BITSET(pPIC->INTMSK0,   (1 << irq));
        BITSET(pPIC->CLR0,      (1 << irq));
    } else {
        BITSET(pPIC->INTMSK1,   (1 << (irq - 32)));
        BITSET(pPIC->CLR1,      (1 << (irq - 32)));
    }
}
static void tcc8800_unmask_irq_uart(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_UART) {
        BITSET(pPIC->INTMSK1, Hw15);
    }
}
static void tcc8800_unmask_irq_gpsb(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_GPSB) {
        BITSET(pPIC->INTMSK1, Hw4);
    }
}
static void tcc8800_unmask_irq_dma(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_GDMA) {
        BITSET(pPIC->INTMSK0, Hw29);
    }
}

static void tcc8800_unmask_irq_tc0(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_TC0) {
        BITSET(pPIC->INTMSK0, Hw0);
    }
}

/******************************************
 * Ack IRQ (Disable IRQ)
 *****************************************/

static void tcc8800_irq_disable(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq < 32){
        BITCLR(pPIC->IEN0,      (1 << irq));
        BITCLR(pPIC->INTMSK0,   (1 << irq));
    } else {
        BITCLR(pPIC->IEN1,      (1 << (irq - 32)));
        BITCLR(pPIC->INTMSK1,   (1 << (irq - 32)));
    }
}


static void tcc8800_mask_ack_irq(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq < 32){
        BITCLR(pPIC->INTMSK0,   (1 << irq));
    } else {
        BITCLR(pPIC->INTMSK1,   (1 << (irq - 32)));
    }
}

static void tcc8800_mask_ack_irq_uart(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_UART) {
        BITCLR(pPIC->INTMSK1, Hw15);
    }
}

static void tcc8800_mask_ack_irq_gpsb(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_GPSB) {
        BITCLR(pPIC->INTMSK1, Hw4);
    }
}

static void tcc8800_mask_ack_irq_dma(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_GDMA) {
        BITCLR(pPIC->INTMSK0, Hw29);
    }
}

static void tcc8800_mask_ack_irq_tc0(struct irq_data *data)
{
    unsigned int irq = data->irq;

    if (irq != INT_TC0) {
        BITCLR(pPIC->INTMSK0, Hw0);
    }
}

/******************************************
 * wake IRQ
 *****************************************/
static int tcc8800_wake_irq(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8800_wake_irq_uart(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8800_wake_irq_gpsb(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8800_wake_irq_dma(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8800_wake_irq_tc0(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static void tcc8800_irq_dummy(struct irq_data *data)
{
}


/******************************************
 * set type IRQ
 *****************************************/
static int tcc8800_irq_set_type(struct irq_data *data, unsigned int type)
{
    volatile unsigned int *mode;    // trigger(0), level(1)
    volatile unsigned int *modea;   // single edge(0), both edge(1)
    volatile unsigned int *pol;     // High active(0), low active(1)
    unsigned int irq = data->irq;

    type &= IRQ_TYPE_SENSE_MASK;

    if(type == IRQ_TYPE_NONE)
        return 0;

    if(irq < 32) {
        mode  = &pPIC->MODE0;
        modea = &pPIC->MODEA0;
        pol   = &pPIC->POL0;
    }
    else {
        mode  = &pPIC->MODE1;
        modea = &pPIC->MODEA1;
        pol   = &pPIC->POL1;
		irq -= 32;
    }

    /* Edge trigger mode */
    if(type == IRQ_TYPE_EDGE_BOTH) {
        BITCLR(*mode, (1 << irq));   // trigger
        BITSET(*modea, (1 << irq));   // both
    }
    else if(type == IRQ_TYPE_EDGE_RISING) {
        BITCLR(*mode, (1 << irq));
        BITCLR(*modea, (1 << irq));
        BITCLR(*pol, (1 << irq));
    }
    else if(type == IRQ_TYPE_EDGE_FALLING) {
        BITCLR(*mode, (1 << irq));
        BITCLR(*modea, (1 << irq));
        BITSET(*pol, (1 << irq));
    }
    else if(type == IRQ_TYPE_LEVEL_HIGH) {  /* Edge trigger mode */
        BITSET(*mode, (1 << irq));   // level
        BITCLR(*pol, (1 << irq));
    }
    else {  /* Edge trigger mode */
        BITSET(*mode, (1 << irq));   // level
        BITSET(*pol, (1 << irq));
    }

    return 0;
}


static void tcc8800_irq_uart_handler(unsigned irq, struct irq_desc *desc)
{
    if (pUARTPORTMUX->CHST & Hw0) {
        irq = INT_UART0;
    } else if (pUARTPORTMUX->CHST & Hw1) {
        irq = INT_UART1;
    } else if (pUARTPORTMUX->CHST & Hw2) {
        irq = INT_UART2;
    } else if (pUARTPORTMUX->CHST & Hw3) {
        irq = INT_UART3;
    } else if (pUARTPORTMUX->CHST & Hw4) {
        irq = INT_UART4;
    } else if (pUARTPORTMUX->CHST & Hw5) {
        irq = INT_UART5;
    } else {
	    //BITSET(pPIC->INTMSK1 , Hw15); // using INTMSK
        BITSET(pPIC->CLR1, Hw15);
        goto out;
    }

    desc = irq_desc + irq;
    desc->handle_irq(irq, desc);
out:
    return;
}

static void tcc8800_irq_gpsb_handler(unsigned irq, struct irq_desc *desc)
{
    if (pPGPSBPORTCFG->CIRQST & Hw3) {
        irq = INT_GPSB1_DMA;
    } else if (pPGPSBPORTCFG->CIRQST & Hw1) {
        irq = INT_GPSB0_DMA;
    } else if (pPGPSBPORTCFG->CIRQST & Hw5) {
        irq = INT_GPSB2_DMA;
    } else if (pPGPSBPORTCFG->CIRQST & Hw0) {
        irq = INT_GPSB0_CORE;
    } else if (pPGPSBPORTCFG->CIRQST & Hw2) {
        irq = INT_GPSB1_CORE;
    } else if (pPGPSBPORTCFG->CIRQST & Hw4) {
        irq = INT_GPSB2_CORE;
	} else if (pPGPSBPORTCFG->CIRQST & Hw6) {
        irq = INT_GPSB3_CORE;
	} else if (pPGPSBPORTCFG->CIRQST & Hw8) {
        irq = INT_GPSB4_CORE;
	} else if (pPGPSBPORTCFG->CIRQST & Hw10) {
        irq = INT_GPSB5_CORE;
    } else {
	    //BITSET(pPIC->INTMSK1 , Hw4);	// using INTMSK
        BITSET(pPIC->CLR1, Hw4);
        goto out;
    }

    desc = irq_desc + irq;
    desc->handle_irq(irq, desc);
out:
    return;
}

static void tcc8800_irq_dma_handler(unsigned irq, struct irq_desc *desc)
{
	if (pPGDMACTRL0->CHCONFIG & (Hw18|Hw17|Hw16)) {
		if (pPGDMACTRL0->CHCONFIG & Hw16) {
			irq = INT_DMA0_CH0;
		} else if (pPGDMACTRL0->CHCONFIG & Hw17) {
			irq = INT_DMA0_CH1;
		} else if (pPGDMACTRL0->CHCONFIG & Hw18) {
			irq = INT_DMA0_CH2;
		} else {
			goto out1;
		}
	} else if (pPGDMACTRL1->CHCONFIG & (Hw18|Hw17|Hw16)) {
		if (pPGDMACTRL1->CHCONFIG & Hw16) {
			irq = INT_DMA1_CH0;
		} else if (pPGDMACTRL1->CHCONFIG & Hw17) {
			irq = INT_DMA1_CH1;
		} else if (pPGDMACTRL1->CHCONFIG & Hw18) {
			irq = INT_DMA1_CH2;
		} else {
			goto out1;
		}

	} else if (pPGDMACTRL2->CHCONFIG & (Hw18|Hw17|Hw16)) {
		if (pPGDMACTRL2->CHCONFIG & Hw16) {
			irq = INT_DMA2_CH0;
		} else if (pPGDMACTRL2->CHCONFIG & Hw17) {
			irq = INT_DMA2_CH1;
		} else if (pPGDMACTRL2->CHCONFIG & Hw18) {
			irq = INT_DMA2_CH2;
		} else {
			goto out1;
		}

	} else if (pPGDMACTRL3->CHCONFIG & (Hw18|Hw17|Hw16)) {
		if (pPGDMACTRL3->CHCONFIG & Hw16) {
			irq = INT_DMA3_CH0;
		} else if (pPGDMACTRL3->CHCONFIG & Hw17) {
			irq = INT_DMA3_CH1;
		} else if (pPGDMACTRL3->CHCONFIG & Hw18) {
			irq = INT_DMA3_CH2;
		} else {
			goto out1;
		}

	} else {
out1:
		BITSET(pPIC->CLR0, Hw29);
		goto out2;
	}

    desc = irq_desc + irq;
    desc->handle_irq(irq, desc);
out2:
    return;
}

static void tcc8800_irq_tc0_handler(unsigned irq, struct irq_desc *desc)
{
	if (pTIMER->TIREQ & Hw0) {
		irq = INT_TC0_TI0;
	} else if (pTIMER->TIREQ & Hw1) {
		irq = INT_TC0_TI1;
	} else if (pTIMER->TIREQ & Hw2) {
		irq = INT_TC0_TI2;
	} else if (pTIMER->TIREQ & Hw3) {
		irq = INT_TC0_TI3;
	} else if (pTIMER->TIREQ & Hw4) {
		irq = INT_TC0_TI4;
	} else if (pTIMER->TIREQ & Hw5) {
		irq = INT_TC0_TI5;
	} else {
		//BITSET(pPIC->INTMSK1 , Hw0);	// using INTMSK
		BITSET(pPIC->CLR0, Hw0);
		goto out;
	}

	desc = irq_desc + irq;
	desc->handle_irq(irq, desc);
out:
	return;
}


static struct irq_chip tcc8800_irq_chip = {
    .name       = "IRQ",
    .irq_enable     = tcc8800_irq_enable,
    .irq_disable    = tcc8800_irq_disable,
    .irq_ack        = tcc8800_mask_ack_irq,
    .irq_mask_ack   = tcc8800_mask_ack_irq,
    .irq_mask       = tcc8800_mask_irq,
    .irq_unmask     = tcc8800_unmask_irq,
    .irq_set_wake   = tcc8800_wake_irq,
    .irq_set_type   = tcc8800_irq_set_type,
};

static struct irq_chip tcc8800_irq_uart_chip = {
    .name       = "IRQ_UART",
    .irq_enable     = tcc8800_irq_dummy,
    .irq_disable    = tcc8800_irq_dummy,
    .irq_ack        = tcc8800_mask_ack_irq_uart,
    .irq_mask_ack   = tcc8800_mask_ack_irq_uart,
    .irq_mask       = tcc8800_mask_irq_uart,
    .irq_unmask     = tcc8800_unmask_irq_uart,
    .irq_set_wake   = tcc8800_wake_irq_uart,
};

static struct irq_chip tcc8800_irq_gpsb_chip = {
    .name       = "IRQ_GPSB",
    .irq_enable     = tcc8800_irq_dummy,
    .irq_disable    = tcc8800_irq_dummy,
    .irq_ack        = tcc8800_mask_ack_irq_gpsb,
    .irq_mask_ack   = tcc8800_mask_ack_irq_gpsb,
    .irq_mask       = tcc8800_mask_irq_gpsb,
    .irq_unmask     = tcc8800_unmask_irq_gpsb,
    .irq_set_wake   = tcc8800_wake_irq_gpsb,
};

static struct irq_chip tcc8800_irq_dma_chip = {
    .name       = "IRQ_DMA",
    .irq_enable     = tcc8800_irq_dummy,
    .irq_disable    = tcc8800_irq_dummy,
    .irq_ack        = tcc8800_mask_ack_irq_dma,
    .irq_mask_ack   = tcc8800_mask_ack_irq_dma,
    .irq_mask       = tcc8800_mask_irq_dma,
    .irq_unmask     = tcc8800_unmask_irq_dma,
    .irq_set_wake   = tcc8800_wake_irq_dma,
};

static struct irq_chip tcc8800_irq_tc0_chip = {
    .name       = "IRQ_TC0",
    .irq_enable     = tcc8800_irq_dummy,
    .irq_disable    = tcc8800_irq_dummy,
    .irq_ack        = tcc8800_mask_ack_irq_tc0,
    .irq_mask_ack   = tcc8800_mask_ack_irq_tc0,
    .irq_mask       = tcc8800_mask_irq_tc0,
    .irq_unmask     = tcc8800_unmask_irq_tc0,
    .irq_set_wake   = tcc8800_wake_irq_tc0,
};

void __init tcc_init_irq(void)
{
	int irqno;

	printk(KERN_DEBUG "%s\n", __func__);

	//reset interrupt
	pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	pVIC = (volatile PVIC)tcc_p2v(HwVIC_BASE);
	pPGPSBPORTCFG = (volatile PGPSBPORTCFG)tcc_p2v(HwGPSB_PORTCFG_BASE);
	pUARTPORTMUX = (volatile PUARTPORTMUX)tcc_p2v(HwUART_PORTMUX_BASE);
	pPGDMACTRL0 = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
	pPGDMACTRL1 = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE);
	pPGDMACTRL2 = (volatile PGDMACTRL)tcc_p2v(HwGDMA2_BASE);
	pPGDMACTRL3 = (volatile PGDMACTRL)tcc_p2v(HwGDMA3_BASE);
	pTIMER = (volatile PTIMER)tcc_p2v(HwTMR_BASE);


	/* ADD IOREMAP */

	//clear IEN Field
	BITCLR(pPIC->IEN0 , 0xFFFFFFFF); // All Interrupt Disable
	BITCLR(pPIC->IEN1 , 0xFFFFFFFF); // All Interrupt Disable

	//clear SEL Field
	BITSET(pPIC->SEL0 , 0xFFFFFFFF); //using IRQ
	BITSET(pPIC->SEL1 , 0xFFFFFFFF); //using IRQ

	//clear TIG Field
	BITCLR(pPIC->TIG0 , 0xFFFFFFFF); //Test Interrupt Disable
	BITCLR(pPIC->TIG1 , 0xFFFFFFFF); //Test Interrupt Disable

	//clear POL Field
	BITCLR(pPIC->POL0 , 0xFFFFFFFF); //Default ACTIVE Low
	BITCLR(pPIC->POL1 , 0xFFFFFFFF); //Default ACTIVE Low

	//clear MODE Field
	BITSET(pPIC->MODE0 , 0xFFFFFFFF); //Trigger Mode - Level Trigger Mode
	BITSET(pPIC->MODE1 , 0xFFFFFFFF); //Trigger Mode - Level Trigger Mode

	//clear SYNC Field
	BITSET(pPIC->SYNC0 , 0xFFFFFFFF); //SYNC Enable
	BITSET(pPIC->SYNC1 , 0xFFFFFFFF); //SYNC Enable

	//clear WKEN Field
	BITCLR(pPIC->WKEN0 , 0xFFFFFFFF); //Wakeup all disable
	BITCLR(pPIC->WKEN1 , 0xFFFFFFFF); //Wakeup all disable

	//celar MODEA Field
	BITCLR(pPIC->MODEA0 , 0xFFFFFFFF); //both edge - all disable
	BITCLR(pPIC->MODEA1 , 0xFFFFFFFF); //both edge - all disable

	//clear INTMSK Field
	BITCLR(pPIC->INTMSK0 , 0xFFFFFFFF); //not using INTMSK
	BITCLR(pPIC->INTMSK1 , 0xFFFFFFFF); //not using INTMSK

	//clear ALLMSK Field
	BITCSET(pPIC->ALLMSK , 0xFFFFFFFF, 0x1); //using only IRQ

	/* Install the interrupt handlers */
	for(irqno = INT_TC0; irqno <= INT_CEER; irqno++)
	{
		if (irqno == INT_UART) {
            irq_set_chip(INT_UART, &tcc8800_irq_uart_chip);
			irq_set_chained_handler(INT_UART, tcc8800_irq_uart_handler);
		} else if (irqno == INT_GPSB) {
			irq_set_chip(INT_GPSB, &tcc8800_irq_gpsb_chip);
			irq_set_chained_handler(INT_GPSB, tcc8800_irq_gpsb_handler);
		} else if (irqno == INT_GDMA) {
			irq_set_chip(INT_GDMA, &tcc8800_irq_dma_chip);
			irq_set_chained_handler(INT_GDMA, tcc8800_irq_dma_handler);
		} else if (irqno == INT_TC0) {
			irq_set_chip(INT_TC0, &tcc8800_irq_tc0_chip);
			irq_set_chained_handler(INT_TC0, tcc8800_irq_tc0_handler);
		} else {
			irq_set_chip(irqno, &tcc8800_irq_chip);
			irq_set_handler(irqno, handle_level_irq);
			set_irq_flags(irqno, IRQF_VALID);
		}
	}

	/* Install the interrupt UART Group handlers */
	for (irqno = INT_UART0; irqno <= INT_UART5; irqno++) {
		irq_set_chip(irqno, &tcc8800_irq_uart_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt GPSB Group handlers */
	for (irqno = INT_GPSB0_DMA; irqno <= INT_GPSB5_CORE; irqno++) {
		irq_set_chip(irqno, &tcc8800_irq_gpsb_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt DMA Group handlers */
	for (irqno = INT_DMA0_CH0; irqno <= INT_DMA3_CH2; irqno++) {
		irq_set_chip(irqno, &tcc8800_irq_dma_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt TC0 Group handlers */
	for (irqno = INT_TC0_TI0; irqno <= INT_TC0_TI5; irqno++) {
		irq_set_chip(irqno, &tcc8800_irq_tc0_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

    /* IEN SET */
	BITSET(pPIC->IEN1,      Hw15);	/* UART */
	BITSET(pPIC->INTMSK1,   Hw15);
	BITSET(pPIC->IEN1,      Hw4);	/* GPSB */
	BITSET(pPIC->INTMSK1,   Hw4);
	BITSET(pPIC->IEN0,      Hw29);	/* DMA */
	BITSET(pPIC->INTMSK0,   Hw29);
	BITSET(pPIC->IEN0,		Hw0);	/* TC0 */
	BITSET(pPIC->INTMSK0,	Hw0);
}

/* end of file */
