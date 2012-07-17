/*
 * linux/arch/arm/mach-tcc892x/irq.c
 *
 * Author:  <linux@telechips.com>
 * Description: Interrupt handler for Telechips TCC892x chipset
 *
 * Copyright (C) 2011 Telechips, Inc.
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
static volatile PUARTPORTCFG pUARTPORTCFG;
static volatile PGDMACTRL pPGDMACTRL0, pPGDMACTRL1, pPGDMACTRL2, pPGDMACTRL3;
static volatile PTIMER pTIMER;
static volatile PVIOC_IREQ_CONFIG pVIOC_IREQ_CONFIG;
static volatile PTSIFIRQSTATUS pPTSIFIRQSTATUS;
static unsigned int gvioc_mask0, gvioc_mask1;

/******************************************
 * Disable IRQ
 *
 * If mask_ack exist, this is not called.
 *****************************************/
static void tcc8920_mask_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;

	if (irq >= 32)
		pPIC->INTMSK1.nREG &= ~(1<<(irq-32));
	else
		pPIC->INTMSK0.nREG &= ~(1<<irq);
}

static void tcc8920_mask_irq_uart(struct irq_data *data)
{
	unsigned int irq = data->irq;

	if (irq != INT_UART) {
		pPIC->INTMSK1.bREG.UART = 0;
	}
}

static void tcc8920_mask_irq_gpsb(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_GPSB) {
        pPIC->INTMSK1.bREG.GPSB = 0;
    }
}

static void tcc8920_mask_irq_dma(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_GDMA) {
        pPIC->INTMSK0.bREG.GDMA = 0;
    }
}

static void tcc8920_mask_irq_tc0(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_TC0) {
        pPIC->INTMSK0.bREG.TC0 = 0;
    }
}

static void tcc8920_mask_irq_vioc(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_LCD) {
        pPIC->INTMSK0.bREG.LCDC = 0;
    }
}

static void tcc8920_mask_irq_tsif(struct irq_data *data)
{
	unsigned int irq = data->irq;
    if (irq != INT_TSIF) {
        pPIC->INTMSK1.bREG.TSIF = 0;
    }
}

/******************************************
 * Enable IRQ
 *****************************************/
static void tcc8920_irq_enable(struct irq_data *data)
{
	unsigned int irq = data->irq;

	if (irq >= 32) {
		pPIC->CLR1.nREG |= (1 << (irq-32));
		pPIC->IEN1.nREG |= (1 << (irq-32));
		pPIC->INTMSK1.nREG |= (1 << (irq-32));
	}
	else {
		pPIC->CLR0.nREG |= (1 << irq);
		pPIC->IEN0.nREG |= (1 << irq);
		pPIC->INTMSK0.nREG |= (1 << irq);
	}
}

static void tcc8920_irq_enable_vioc(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_LCD) {
        irq -= INT_VIOC_BASE;
    	if (irq >= 32) {
            gvioc_mask1 |= (1 << (irq-32));
    		pVIOC_IREQ_CONFIG->nIRQMASKCLR.nREG[1] = (1 << (irq-32));
    	}
    	else {
            gvioc_mask0 |= (1 << irq);
    		pVIOC_IREQ_CONFIG->nIRQMASKCLR.nREG[0] = (1 << irq);
    	}
    }
}

static void tcc8920_unmask_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;

	if (irq >= 32) {
		pPIC->INTMSK1.nREG |= (1 << (irq-32));
		pPIC->CLR1.nREG |= (1 << (irq-32));
	}
	else {
		pPIC->INTMSK0.nREG |= (1 << irq);
		pPIC->CLR0.nREG |= (1 << irq);
	}
}
static void tcc8920_unmask_irq_uart(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_UART) {
		pPIC->INTMSK1.bREG.UART = 1;
    }
}
static void tcc8920_unmask_irq_gpsb(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_GPSB) {
        pPIC->INTMSK1.bREG.GPSB = 1;
    }
}
static void tcc8920_unmask_irq_dma(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_GDMA) {
        pPIC->INTMSK0.bREG.GDMA = 1;
    }
}

static void tcc8920_unmask_irq_tc0(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_TC0) {
        pPIC->INTMSK0.bREG.TC0 = 1;
    }
}

static void tcc8920_unmask_irq_vioc(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_LCD) {
        pPIC->INTMSK0.bREG.LCDC = 1;
    }
}

static void tcc8920_unmask_irq_tsif(struct irq_data *data)
{
	unsigned int irq = data->irq;
    if (irq != INT_TSIF) {
        pPIC->INTMSK1.bREG.TSIF = 1;
    }
}
/******************************************
 * Ack IRQ (Disable IRQ)
 *****************************************/

static void tcc8920_irq_disable(struct irq_data *data)
{
	unsigned int irq = data->irq;

	if (irq >= 32) {
		pPIC->IEN1.nREG &= ~(1 << (irq-32));
		pPIC->INTMSK1.nREG &= ~(1 << (irq-32));
	}
	else {
		pPIC->IEN0.nREG &= ~(1 << irq);
		pPIC->INTMSK0.nREG &= ~(1 << irq);
	}
}

static void tcc8920_irq_disable_vioc(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_LCD) {
        irq -= INT_VIOC_BASE;
    	if (irq >= 32) {
            gvioc_mask1 &= ~(1 << (irq-32));
    		pVIOC_IREQ_CONFIG->nIRQMASKSET.nREG[1] = (1 << (irq-32));
    	}
    	else {
            gvioc_mask0 &= ~(1 << irq);
    		pVIOC_IREQ_CONFIG->nIRQMASKSET.nREG[0] = (1 << irq);
    	}
    }
}


static void tcc8920_mask_ack_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;

	if (irq >= 32)
		pPIC->INTMSK1.nREG &= ~(1 << (irq-32));
	else
		pPIC->INTMSK0.nREG &= ~(1 << irq);
}

static void tcc8920_mask_ack_irq_uart(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_UART) {
        pPIC->INTMSK1.bREG.UART = 0;
    }
}

static void tcc8920_mask_ack_irq_gpsb(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_GPSB) {
        pPIC->INTMSK1.bREG.GPSB = 0;
    }
}

static void tcc8920_mask_ack_irq_dma(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_GDMA) {
        pPIC->INTMSK0.bREG.GDMA = 0;
    }
}

static void tcc8920_mask_ack_irq_tc0(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_TC0) {
        pPIC->INTMSK0.bREG.TC0 = 0;
    }
}

static void tcc8920_mask_ack_irq_vioc(struct irq_data *data)
{
	unsigned int irq = data->irq;

    if (irq != INT_LCD) {
        pPIC->INTMSK0.bREG.LCDC = 0;
    }
}

static void tcc8920_mask_ack_irq_tsif(struct irq_data *data)
{
	unsigned int irq = data->irq;
    if (irq != INT_TSIF) {
        pPIC->INTMSK1.bREG.TSIF = 0;
    }
}

/******************************************
 * wake IRQ
 *****************************************/
static int tcc8920_wake_irq(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8920_wake_irq_uart(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8920_wake_irq_gpsb(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8920_wake_irq_dma(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8920_wake_irq_tc0(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8920_wake_irq_vioc(struct irq_data *data, unsigned int enable)
{
    return 0;
}

static int tcc8920_wake_irq_tsif(struct irq_data *data, unsigned int enable)
{
    return 0;
}
static void tcc8920_irq_dummy(struct irq_data *data)
{
}


/******************************************
 * set type IRQ
 *****************************************/
static int tcc8920_irq_set_type(struct irq_data *data, unsigned int type)
{
	unsigned int irq = data->irq;

    type &= IRQ_TYPE_SENSE_MASK;

    if(type == IRQ_TYPE_NONE)
        return 0;

    /* Edge trigger mode */
    if(type == IRQ_TYPE_EDGE_BOTH) {
		if (irq >= 32) {
			pPIC->MODE1.nREG &= ~(1 << (irq-32));   // trigger
			pPIC->MODEA1.nREG |= (1 << (irq-32));   // both
		}
		else {
			pPIC->MODE0.nREG &= ~(1 << irq);   // trigger
			pPIC->MODEA0.nREG |= (1 << irq);   // both
		}
    }
    else if(type == IRQ_TYPE_EDGE_RISING) {
		if (irq >= 32) {
			pPIC->MODE1.nREG &= ~(1 << (irq-32));
			pPIC->MODEA1.nREG &= ~(1 << (irq-32));
			pPIC->POL1.nREG &= ~(1 << (irq-32));
		}
		else {
			pPIC->MODE0.nREG &= ~(1 << irq);
			pPIC->MODEA0.nREG &= ~(1 << irq);
			pPIC->POL0.nREG &= ~(1 << irq);
		}
    }
    else if(type == IRQ_TYPE_EDGE_FALLING) {
		if (irq >= 32) {
			pPIC->MODE1.nREG &= ~(1 << (irq-32));
			pPIC->MODEA1.nREG &= ~(1 << (irq-32));
			pPIC->POL1.nREG |= (1 << (irq-32));
		}
		else {
			pPIC->MODE0.nREG &= ~(1 << irq);
			pPIC->MODEA0.nREG &= ~(1 << irq);
			pPIC->POL0.nREG |= (1 << irq);
		}
    }
    else if(type == IRQ_TYPE_LEVEL_HIGH) {  /* Edge trigger mode */
		if (irq >= 32) {
	 		pPIC->MODE1.nREG |= (1 << (irq-32));   // level
	 		pPIC->POL1.nREG &= ~(1 << (irq-32));
		}
		else {
	 		pPIC->MODE0.nREG |= (1 << irq);   // level
	 		pPIC->POL0.nREG &= ~(1 << irq);
		}
    }
    else {  /* Edge trigger mode */
		if (irq >= 32) {
			pPIC->MODE1.nREG |= (1 << (irq-32));   // level
			pPIC->POL1.nREG |= (1 << (irq-32));
		}
		else {
			pPIC->MODE0.nREG |= (1 << irq);   // level
			pPIC->POL0.nREG |= (1 << irq);
		}
    }

    return 0;
}


static void tcc8920_irq_uart_handler(unsigned irq, struct irq_desc *desc)
{
	if (pUARTPORTCFG->ISTS.bREG.U0)
		irq = INT_UART0;
	else if (pUARTPORTCFG->ISTS.bREG.U1)
		irq = INT_UART1;
	else if (pUARTPORTCFG->ISTS.bREG.U2)
		irq = INT_UART2;
	else if (pUARTPORTCFG->ISTS.bREG.U3)
		irq = INT_UART3;
	else if (pUARTPORTCFG->ISTS.bREG.U4)
		irq = INT_UART4;
	else if (pUARTPORTCFG->ISTS.bREG.U5)
		irq = INT_UART5;
	else if (pUARTPORTCFG->ISTS.bREG.U6)
		irq = INT_UART6;
	else if (pUARTPORTCFG->ISTS.bREG.U7)
		irq = INT_UART7;
	else {
		pPIC->CLR1.bREG.UART = 1;
		goto out;
	}

	desc = irq_desc + irq;
	desc->handle_irq(irq, desc);
out:
	return;
}

static void tcc8920_irq_gpsb_handler(unsigned irq, struct irq_desc *desc)
{
	if (pPGPSBPORTCFG->CIRQST.bREG.ISTD0)
		irq = INT_GPSB0_DMA;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTD1)
		irq = INT_GPSB1_DMA;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTD2)
		irq = INT_GPSB2_DMA;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTC0)
		irq = INT_GPSB0_CORE;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTC1)
		irq = INT_GPSB1_CORE;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTC2)
		irq = INT_GPSB2_CORE;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTC3)
		irq = INT_GPSB3_CORE;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTC4)
		irq = INT_GPSB4_CORE;
	else if (pPGPSBPORTCFG->CIRQST.bREG.ISTC5)
		irq = INT_GPSB5_CORE;
	else {
		pPIC->CLR1.bREG.GPSB = 1;
		goto out;
	}

	desc = irq_desc + irq;
	desc->handle_irq(irq, desc);
out:
	return;
}

static void tcc8920_irq_dma_handler(unsigned irq, struct irq_desc *desc)
{
	if (pPGDMACTRL0->CHCONFIG.bREG.MIS0)
		irq = INT_DMA0_CH0;
	else if (pPGDMACTRL0->CHCONFIG.bREG.MIS1)
		irq = INT_DMA0_CH1;
	else if (pPGDMACTRL0->CHCONFIG.bREG.MIS2)
		irq = INT_DMA0_CH2;
	else if (pPGDMACTRL1->CHCONFIG.bREG.MIS0)
		irq = INT_DMA1_CH0;
	else if (pPGDMACTRL1->CHCONFIG.bREG.MIS1)
		irq = INT_DMA1_CH1;
	else if (pPGDMACTRL1->CHCONFIG.bREG.MIS2)
		irq = INT_DMA1_CH2;
	else if (pPGDMACTRL2->CHCONFIG.bREG.MIS0)
		irq = INT_DMA2_CH0;
	else if (pPGDMACTRL2->CHCONFIG.bREG.MIS1)
		irq = INT_DMA2_CH1;
	else if (pPGDMACTRL2->CHCONFIG.bREG.MIS2)
		irq = INT_DMA2_CH2;
	else {
		pPIC->CLR0.bREG.GDMA = 1;
		goto out;
	}

	desc = irq_desc + irq;
	desc->handle_irq(irq, desc);
out:
	return;
}

static void tcc8920_irq_tc_handler(unsigned irq, struct irq_desc *desc)
{
	if (pTIMER->TIREQ.bREG.TI0)
		irq = INT_TC_TI0;
	else if (pTIMER->TIREQ.bREG.TI1)
		irq = INT_TC_TI1;
	else if (pTIMER->TIREQ.bREG.TI2)
		irq = INT_TC_TI2;
	else if (pTIMER->TIREQ.bREG.TI3)
		irq = INT_TC_TI3;
	else if (pTIMER->TIREQ.bREG.TI4)
		irq = INT_TC_TI4;
	else if (pTIMER->TIREQ.bREG.TI5)
		irq = INT_TC_TI5;
	else {
		pPIC->CLR0.bREG.TC0 = 1;
		goto out;
	}

	desc = irq_desc + irq;
	desc->handle_irq(irq, desc);
out:
	return;
}


static void tcc8920_irq_vioc_handler(unsigned irq, struct irq_desc *desc)
{
    int i;
    int flag;
    unsigned int bitmask;



    // SYNCSTATUS ==>  0: async interrupt, 1: sync interrupt
    for (i = 0;i < (INT_VIOC_NUM - INT_VIOC_BASE + 1);i++) {
        if (i < 32) {
			bitmask = 1 << i;
            if (gvioc_mask0 & bitmask) {
                flag = pVIOC_IREQ_CONFIG->uIREQSELECT.nREG[0] & bitmask ? 
                        (pVIOC_IREQ_CONFIG->uSYNCSTATUS.nREG[0] & bitmask) : 
                        (pVIOC_IREQ_CONFIG->uRAWSTATUS.nREG[0] & bitmask);
                if (flag)       break;
            }
        }
        else {
			bitmask = 1 << (i-32);
            if (gvioc_mask1 & bitmask) {
                flag = pVIOC_IREQ_CONFIG->uIREQSELECT.nREG[1] & bitmask ? 
                        (pVIOC_IREQ_CONFIG->uSYNCSTATUS.nREG[1] & bitmask) : 
                        (pVIOC_IREQ_CONFIG->uRAWSTATUS.nREG[1] & bitmask);
                if (flag)       break;
            }
        }
    }

    if (i >= (INT_VIOC_NUM - INT_VIOC_BASE)) {
        /* clear interrupt */
        pPIC->CLR0.bREG.LCDC = 1;
        goto out;
    }

	desc = irq_desc + INT_VIOC_BASE + i;
	desc->handle_irq(irq, desc);
out:
	return;
}

static void tcc8920_irq_tsif_handler(unsigned irq, struct irq_desc *desc)
{
	if(pPTSIFIRQSTATUS->IRQSTS.bREG.DMA_CH0)
		irq = INT_TSIF_DMA0;
	else if(pPTSIFIRQSTATUS->IRQSTS.bREG.DMA_CH1)
		irq = INT_TSIF_DMA1;
	else if(pPTSIFIRQSTATUS->IRQSTS.bREG.DMA_CH2)
		irq = INT_TSIF_DMA2;
	else {
		pPIC->CLR1.bREG.TSIF = 1;
		goto out;
	}

	desc = irq_desc + irq;
	desc->handle_irq(irq, desc);
out:
	return;
}


static struct irq_chip tcc8920_irq_chip = {
    .name       = "IRQ",
    .irq_enable     = tcc8920_irq_enable,
    .irq_disable    = tcc8920_irq_disable,
    .irq_ack        = tcc8920_mask_ack_irq,
    .irq_mask_ack   = tcc8920_mask_ack_irq,
    .irq_mask       = tcc8920_mask_irq,
    .irq_unmask     = tcc8920_unmask_irq,
    .irq_set_wake   = tcc8920_wake_irq,
    .irq_set_type   = tcc8920_irq_set_type,
};

static struct irq_chip tcc8920_irq_uart_chip = {
    .name       = "IRQ_UART",
    .irq_enable     = tcc8920_irq_dummy,
    .irq_disable    = tcc8920_irq_dummy,
    .irq_ack        = tcc8920_mask_ack_irq_uart,
    .irq_mask_ack   = tcc8920_mask_ack_irq_uart,
    .irq_mask       = tcc8920_mask_irq_uart,
    .irq_unmask     = tcc8920_unmask_irq_uart,
    .irq_set_wake   = tcc8920_wake_irq_uart,
};

static struct irq_chip tcc8920_irq_gpsb_chip = {
    .name       = "IRQ_GPSB",
    .irq_enable     = tcc8920_irq_dummy,
    .irq_disable    = tcc8920_irq_dummy,
    .irq_ack        = tcc8920_mask_ack_irq_gpsb,
    .irq_mask_ack   = tcc8920_mask_ack_irq_gpsb,
    .irq_mask       = tcc8920_mask_irq_gpsb,
    .irq_unmask     = tcc8920_unmask_irq_gpsb,
    .irq_set_wake   = tcc8920_wake_irq_gpsb,
};

static struct irq_chip tcc8920_irq_dma_chip = {
    .name       = "IRQ_DMA",
    .irq_enable     = tcc8920_irq_dummy,
    .irq_disable    = tcc8920_irq_dummy,
    .irq_ack        = tcc8920_mask_ack_irq_dma,
    .irq_mask_ack   = tcc8920_mask_ack_irq_dma,
    .irq_mask       = tcc8920_mask_irq_dma,
    .irq_unmask     = tcc8920_unmask_irq_dma,
    .irq_set_wake   = tcc8920_wake_irq_dma,
};

static struct irq_chip tcc8920_irq_tc_chip = {
    .name       = "IRQ_TC",
    .irq_enable     = tcc8920_irq_dummy,
    .irq_disable    = tcc8920_irq_dummy,
    .irq_ack        = tcc8920_mask_ack_irq_tc0,
    .irq_mask_ack   = tcc8920_mask_ack_irq_tc0,
    .irq_mask       = tcc8920_mask_irq_tc0,
    .irq_unmask     = tcc8920_unmask_irq_tc0,
    .irq_set_wake   = tcc8920_wake_irq_tc0,
};

static struct irq_chip tcc8920_irq_vioc_chip = {
    .name       = "IRQ_VIOC",
    .irq_enable     = tcc8920_irq_enable_vioc,
    .irq_disable    = tcc8920_irq_disable_vioc,
    .irq_ack        = tcc8920_mask_ack_irq_vioc,
    .irq_mask_ack   = tcc8920_mask_ack_irq_vioc,
    .irq_mask       = tcc8920_mask_irq_vioc,
    .irq_unmask     = tcc8920_unmask_irq_vioc,
    .irq_set_wake   = tcc8920_wake_irq_vioc,
};

static struct irq_chip tcc8920_irq_tsif_chip = {
    .name       = "IRQ_TSIF",
    .irq_enable     = tcc8920_irq_dummy,
    .irq_disable    = tcc8920_irq_dummy,
    .irq_ack        = tcc8920_mask_ack_irq_tsif,
    .irq_mask_ack   = tcc8920_mask_ack_irq_tsif,
    .irq_mask       = tcc8920_mask_irq_tsif,
    .irq_unmask     = tcc8920_unmask_irq_tsif,
    .irq_set_wake   = tcc8920_wake_irq_tsif,
};
void __init tcc_init_irq(void)
{
	int irqno;

	printk(KERN_DEBUG "%s\n", __func__);

	//reset interrupt
	pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	pVIC = (volatile PVIC)tcc_p2v(HwVIC_BASE);
	pPGPSBPORTCFG = (volatile PGPSBPORTCFG)tcc_p2v(HwGPSB_PORTCFG_BASE);
	pUARTPORTCFG = (volatile PUARTPORTCFG)tcc_p2v(HwUART_PORTCFG_BASE);
	pPGDMACTRL0 = (volatile PGDMACTRL)tcc_p2v(HwGDMA0_BASE);
	pPGDMACTRL1 = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE);
	pPGDMACTRL2 = (volatile PGDMACTRL)tcc_p2v(HwGDMA2_BASE);
	pTIMER = (volatile PTIMER)tcc_p2v(HwTMR_BASE);
    pVIOC_IREQ_CONFIG = (volatile PVIOC_IREQ_CONFIG)tcc_p2v(HwVIOC_IREQ);
	pPTSIFIRQSTATUS = (volatile PTSIFIRQSTATUS)tcc_p2v(HwTSIF_IRQSTS_BASE);

	/* ADD IOREMAP */

	//clear IEN Field
	pPIC->IEN0.nREG = (unsigned long long)0x00000000; // All Interrupt Disable
	pPIC->IEN1.nREG = (unsigned long long)0x00000000; // All Interrupt Disable

	//clear SEL Field
	pPIC->SEL0.nREG = (unsigned long long)0xFFFFFFFF; //using IRQ
	pPIC->SEL1.nREG = (unsigned long long)0xFFFFFFFF; //using IRQ

	//clear TIG Field
	pPIC->TIG0.nREG = (unsigned long long)0x00000000; //Test Interrupt Disable
	pPIC->TIG1.nREG = (unsigned long long)0x00000000; //Test Interrupt Disable

	//clear POL Field
	pPIC->POL0.nREG = (unsigned long long)0x00000000; //Default ACTIVE Low
	pPIC->POL1.nREG = (unsigned long long)0x00000000; //Default ACTIVE Low

	//clear MODE Field
	pPIC->MODE0.nREG = (unsigned long long)0xFFFFFFFF; //Trigger Mode - Level Trigger Mode
	pPIC->MODE1.nREG = (unsigned long long)0xFFFFFFFF; //Trigger Mode - Level Trigger Mode

	//clear SYNC Field
	pPIC->SYNC0.nREG = (unsigned long long)0xFFFFFFFF; //SYNC Enable
	pPIC->SYNC1.nREG = (unsigned long long)0xFFFFFFFF; //SYNC Enable

	//clear WKEN Field
	pPIC->WKEN0.nREG = (unsigned long long)0x00000000; //Wakeup all disable
	pPIC->WKEN1.nREG = (unsigned long long)0x00000000; //Wakeup all disable

	//celar MODEA Field
	pPIC->MODEA0.nREG = (unsigned long long)0x00000000; //both edge - all disable
	pPIC->MODEA1.nREG = (unsigned long long)0x00000000; //both edge - all disable

	//clear INTMSK Field
	pPIC->INTMSK0.nREG = (unsigned long long)0x00000000; //not using INTMSK
	pPIC->INTMSK1.nREG = (unsigned long long)0x00000000; //not using INTMSK

	//clear ALLMSK Field
	pPIC->ALLMSK.bREG.IRQ = 1; //using only IRQ
	pPIC->ALLMSK.bREG.FIQ = 0;

	/* Install the interrupt handlers */
	for(irqno = INT_TC0; irqno <= INT_NUM; irqno++)
	{
		if (irqno == INT_UART) {
			irq_set_chip(INT_UART, &tcc8920_irq_uart_chip);
			irq_set_chained_handler(INT_UART, tcc8920_irq_uart_handler);
		} else if (irqno == INT_GPSB) {
			irq_set_chip(INT_GPSB, &tcc8920_irq_gpsb_chip);
			irq_set_chained_handler(INT_GPSB, tcc8920_irq_gpsb_handler);
		} else if (irqno == INT_GDMA) {
			irq_set_chip(INT_GDMA, &tcc8920_irq_dma_chip);
			irq_set_chained_handler(INT_GDMA, tcc8920_irq_dma_handler);
		} else if (irqno == INT_TC0) {
			irq_set_chip(INT_TC0, &tcc8920_irq_tc_chip);
			irq_set_chained_handler(INT_TC0, tcc8920_irq_tc_handler);
		} else if (irqno == INT_LCD) {
			irq_set_chip(INT_LCD, &tcc8920_irq_vioc_chip);
			irq_set_chained_handler(INT_LCD, tcc8920_irq_vioc_handler);
		} else if (irqno == INT_TSIF) {
			irq_set_chip(INT_TSIF, &tcc8920_irq_tsif_chip);
			irq_set_chained_handler(INT_TSIF, tcc8920_irq_tsif_handler);
		} else {
			irq_set_chip(irqno, &tcc8920_irq_chip);
			irq_set_handler(irqno, handle_level_irq);
			set_irq_flags(irqno, IRQF_VALID);
		}
	}

	/* Install the interrupt UART Group handlers */
	for (irqno = INT_UT_BASE; irqno <= INT_UART_NUM; irqno++) {
		irq_set_chip(irqno, &tcc8920_irq_uart_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt GPSB Group handlers */
	for (irqno = INT_GPSB_BASE; irqno <= INT_GPSB_NUM; irqno++) {
		irq_set_chip(irqno, &tcc8920_irq_gpsb_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt DMA Group handlers */
	for (irqno = INT_DMA_BASE; irqno <= INT_DMA_NUM; irqno++) {
		irq_set_chip(irqno, &tcc8920_irq_dma_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt TC0 Group handlers */
	for (irqno = INT_TC_BASE; irqno <= INT_TC_NUM; irqno++) {
		irq_set_chip(irqno, &tcc8920_irq_tc_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt VIOC Group handlers */
	for (irqno = INT_VIOC_BASE; irqno <= INT_VIOC_NUM; irqno++) {
		irq_set_chip(irqno, &tcc8920_irq_vioc_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

	/* Install the interrupt TSIF Group handlers */
	for (irqno = INT_TSIF_BASE; irqno <= INT_TSIF_NUM; irqno++) {
		irq_set_chip(irqno, &tcc8920_irq_tsif_chip);
		irq_set_handler(irqno, handle_level_irq);
		set_irq_flags(irqno, IRQF_VALID);
	}

    /* IEN SET */
	pPIC->IEN1.bREG.UART = 1;
	pPIC->INTMSK1.bREG.UART = 1;
	pPIC->IEN1.bREG.GPSB = 1;
	pPIC->INTMSK1.bREG.GPSB = 1;
	pPIC->IEN0.bREG.GDMA = 1;
	pPIC->INTMSK0.bREG.GDMA = 1;
	pPIC->IEN0.bREG.TC0 = 1;
	pPIC->INTMSK0.bREG.TC0 = 1;
	pPIC->IEN1.bREG.TSIF = 1;
	pPIC->INTMSK1.bREG.TSIF = 1;

    /* Video Input Output Control */
    pVIOC_IREQ_CONFIG->nIRQMASKSET.nREG[0] = 0xffffffff;    /* Disable VIOC IRQ */
    pVIOC_IREQ_CONFIG->nIRQMASKSET.nREG[1] = 0xffffffff;
    pPIC->IEN0.bREG.LCDC = 1;                               /* Enable LCDC interrupt */
    pPIC->INTMSK0.bREG.LCDC = 1;                            /* unmask LCDC */
    gvioc_mask0 = 0;                                        /* Clear mask value for VIOC */
    gvioc_mask1 = 0;
	}

/* end of file */
