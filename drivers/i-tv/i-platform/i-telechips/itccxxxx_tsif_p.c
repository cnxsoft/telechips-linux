#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/poll.h>
#include <linux/spi/spi.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/dma.h>

#include <mach/bsp.h>

#if defined(CONFIG_ARCH_TCC892X)
#include <mach/tca_tsif.h>
#else
#include "../../../spi/tcc/tca_tsif_module_hwset.h"
#endif

#include <i-tv/itv_common.h>
#include "itccxxxx_tsif_p.h"

//20110321 koo : tsif/dma reg & input ts data debug option
//#define TSIF_DEBUG

extern int tca_tsif_init(struct tcc_tsif_handle *h, volatile struct tcc_tsif_regs *regs, dma_alloc_f tea_dma_alloc, dma_free_f tea_dma_free, int dma_size, int tsif_ch, int dma_controller, int dma_ch, int port);
extern void tca_tsif_clean(struct tcc_tsif_handle *h);



typedef enum {
	P_TSIF_STATE_DEINIT, 
	P_TSIF_STATE_INIT, 
	P_TSIF_STATE_START, 
	P_TSIF_STATE_STOP 
} e_p_tsif_state;

typedef struct {
	e_p_tsif_state state;

	int mode;	// true : TSIF_Serial mode, false : TSIF_Parallel mode.
	struct tcc_tsif_handle handle;

	void *cb_data;
	void (*callback)(void *);
	
	#define PIDTABLE_MAXCNT		32
	
	unsigned short pidtable[PIDTABLE_MAXCNT];

	spinlock_t lock;
} st_p_tsif;

st_p_tsif p_tsif_inst = {
	.state = P_TSIF_STATE_DEINIT, 
};

static void tsif_p_free_dma_buffer(struct tea_dma_buf *tdma)
{
	if(tdma) {
		if(tdma->v_addr)
			dma_free_writecombine(0, tdma->buf_size, tdma->v_addr, tdma->dma_addr);
		memset(tdma, 0, sizeof(struct tea_dma_buf));
	}
}

static int tsif_p_alloc_dma_buffer(struct tea_dma_buf *tdma, unsigned int size)
{
	int error = -1;

	if(tdma) {
		tsif_p_free_dma_buffer(tdma);
		tdma->buf_size = size;
		tdma->v_addr = dma_alloc_writecombine(0, tdma->buf_size, &tdma->dma_addr, GFP_KERNEL);
		printk("[%s] Alloc DMA buffer @0x%X(Phy=0x%X), size:%d\n", __func__, (unsigned int)tdma->v_addr,
				(unsigned int)tdma->dma_addr, tdma->buf_size);

		error = tdma->v_addr ? 0 : 1;
	}

	return error;
}

#ifdef TSIF_DEBUG
struct timeval time_;
unsigned int cur_time=0;
unsigned int base_time=0;
unsigned int debug_time=0;
unsigned int debug_cnt=0;
#define CHK_TIME			10	//sec
struct task_struct *debug_thread = NULL;
#endif

static irqreturn_t tsif_p_dma_handler(int irq, void *dev_id)
{
	st_p_tsif *instance = &p_tsif_inst;
	struct tcc_tsif_handle *handle = (struct tcc_tsif_handle *)dev_id;

	handle->tsif_isr(handle);

#ifdef GDMA
	handle->cur_q_pos = (int)handle->dma_phy_curpos + (int)(handle->rx_dma.v_addr - handle->rx_dma.dma_addr);
#else
	handle->q_pos = (int)(handle->rx_dma.v_addr + (handle->cur_q_pos * MPEG_PACKET_SIZE));
#endif

#ifdef TSIF_DEBUG
	{
		int cnt;
		unsigned char* pos;
 
#ifdef GDMA
		if(handle->cur_q_pos > handle->prev_q_pos) {
			pos = (int)handle->prev_q_pos;
			cnt = handle->cur_q_pos - (int)handle->prev_q_pos;
			
			do_gettimeofday(&time_);
			cur_time = ((time_.tv_sec * 1000) & 0x00ffffff) + (time_.tv_usec / 1000);
			if((cur_time - base_time) > (CHK_TIME * 1000)) {
				printk(">> [0x%08x - 0x%08x / %d] : 0x%02x\n", handle->prev_q_pos, handle->cur_q_pos, cnt, *pos);
				if(*pos != 0x47) {
					if((pos - (int)handle->rx_dma.v_addr) >= 2) {
						printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos-2), *(pos-1), *pos, *(pos+1), *(pos+2));
					} else {
						unsigned char* pos_addr;
						int pos_size;

						pos_size = pos - (int)handle->rx_dma.v_addr;

						if(pos_size == 0) {
							pos_addr = (TSIF_DMA_SIZE + (int)handle->rx_dma.v_addr) - 2;
							printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos_addr), *(pos_addr+1), *pos, *(pos+1), *(pos+2));
						} else if(pos_size == 1) {
							pos_addr = (TSIF_DMA_SIZE + (int)handle->rx_dma.v_addr) - 1;
							printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos_addr), *(pos-1), *pos, *(pos+1), *(pos+2));
						}
					}
				}
				base_time = cur_time;
			}

			handle->prev_q_pos += (cnt / MPEG_PACKET_SIZE) * MPEG_PACKET_SIZE;
		} else {
			int res, ovr;
			
			pos = (int)handle->prev_q_pos;
			res = (TSIF_DMA_SIZE + (int)handle->rx_dma.v_addr) - (int)handle->prev_q_pos;
			ovr = handle->cur_q_pos - (int)handle->rx_dma.v_addr;
			cnt = res + ovr;
			
			do_gettimeofday(&time_);
			cur_time = ((time_.tv_sec * 1000) & 0x00ffffff) + (time_.tv_usec / 1000);
			if((cur_time - base_time) > (CHK_TIME * 1000)) {
				printk(">> [0x%08x - 0x%08x / %d] : 0x%02x\n", handle->prev_q_pos, handle->cur_q_pos, cnt, *pos);
				if(*pos != 0x47) {
					if((pos - (int)handle->rx_dma.v_addr) >= 2) {
						printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos-2), *(pos-1), *pos, *(pos+1), *(pos+2));
					} else {
						unsigned char* pos_addr;
						int pos_size;

						pos_size = pos - (int)handle->rx_dma.v_addr;

						if(pos_size == 0) {
							pos_addr = (TSIF_DMA_SIZE + (int)handle->rx_dma.v_addr) - 2;
							printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos_addr), *(pos_addr+1), *pos, *(pos+1), *(pos+2));
						} else if(pos_size == 1) {
							pos_addr = (TSIF_DMA_SIZE + (int)handle->rx_dma.v_addr) - 1;
							printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos_addr), *(pos-1), *pos, *(pos+1), *(pos+2));
						}
					}
				}
				
				base_time = cur_time;
			}

			handle->prev_q_pos = (int)handle->rx_dma.v_addr + (ovr - (cnt % MPEG_PACKET_SIZE));
		}
#else	//#ifdef GDMA
		if(handle->q_pos > handle->prev_q_pos) {
			pos = (int)handle->prev_q_pos;
			cnt = handle->q_pos - (int)handle->prev_q_pos;
		} else {
			pos = (int)handle->prev_q_pos;
			cnt = ((handle->dma_total_packet_cnt * MPEG_PACKET_SIZE) + (int)handle->rx_dma.v_addr) - (int)handle->prev_q_pos;
		}

		do_gettimeofday(&time_);
		cur_time = ((time_.tv_sec * 1000) & 0x00ffffff) + (time_.tv_usec / 1000);
		if((cur_time - base_time) > (CHK_TIME * 1000)) {
			printk(">> [0x%08x - 0x%08x / %d] : 0x%02x\n", handle->prev_q_pos, handle->q_pos, cnt, *pos);
			if(*pos != 0x47) {
				if((pos - (int)handle->rx_dma.v_addr) >= 2) {
					printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos-2), *(pos-1), *pos, *(pos+1), *(pos+2));
				} else {
					unsigned char* pos_addr;
					int pos_size;

					pos_size = pos - (int)handle->rx_dma.v_addr;

					if(pos_size == 0) {
						pos_addr = (TSIF_DMA_SIZE + (int)handle->rx_dma.v_addr) - 2;
						printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos_addr), *(pos_addr+1), *pos, *(pos+1), *(pos+2));
					} else if(pos_size == 1) {
						pos_addr = (TSIF_DMA_SIZE + (int)handle->rx_dma.v_addr) - 1;
						printk("\t[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x]\n", *(pos_addr), *(pos-1), *pos, *(pos+1), *(pos+2));
					}
				}
			}
			
			base_time = cur_time;
		}

		handle->prev_q_pos = handle->q_pos;
#endif	//#ifdef GDMA
	}
#endif	//#ifdef TSIF_DEBUG

	if(instance->callback)
		instance->callback(instance->cb_data);

	return IRQ_HANDLED;
}

int tcc_tsif_p_get_pos(void)
{
	st_p_tsif *instance = &p_tsif_inst;
	struct tcc_tsif_handle *handle = &instance->handle;

#ifdef GDMA
	return handle->cur_q_pos;
#else
	return handle->q_pos;
#endif
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_get_pos);

void tcc_tsif_p_insert_pid(int pid)
{
	st_p_tsif *instance = &p_tsif_inst;
	int i, pos=0;		
	
	spin_lock(&instance->lock);
	
#ifdef GDMA
#ifdef SUPPORT_PIDFILTER_INTERNAL
	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] == pid) {
			printk("[%s] pid table already insert val : %d\n", __func__, pid);
			return;
		}
	}

	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] == 0xffff) {
			pos = i;
			break;
		}
		
		if(i == (PIDTABLE_MAXCNT - 1)) {
			printk("[%s] pid table full : %d\n", __func__, (i + 1));
			return;
		}
	}

#if defined(CONFIG_ARCH_TCC88XX)
	PTSIF	tsif_regs;
	
	tsif_regs = (volatile PTSIF)tcc_p2v((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF0_BASE);

	tsif_regs->TSPID[pos] = 0;
	tsif_regs->TSPID[pos] = (pid & 0x1FFF) | Hw13;
#elif defined(CONFIG_ARCH_TCC892X)
	PTSIF	tsif_regs;
	int idx;
	
	tsif_regs = (volatile PTSIF)tcc_p2v(((TSIF_CH == 0) ? HwTSIF0_BASE : ((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF2_BASE)));

	idx = pos / 2;
	if(pos % 2) {
		tsif_regs->TSPID[idx].nREG &= 0xffff0000;
		tsif_regs->TSPID[idx].nREG |= (pid & 0x1FFF) | Hw13;
	} else {
		tsif_regs->TSPID[idx].nREG &= 0x0000ffff;
		tsif_regs->TSPID[idx].nREG |= ((pid & 0x1FFF) << 16) | Hw29;
	}
#endif

	instance->pidtable[pos] = pid;

	//printk("[%s] pidtable insert : %d-%d\n", __func__, pos, pid);
#endif
#else
	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] == pid) {
			printk("[%s] pid table already insert val : %d\n", __func__, pid);
			return;
		}
	}

	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] == 0xffff) {
			pos = i;
			break;
		}
		
		if(i == (PIDTABLE_MAXCNT - 1)) {
			printk("[%s] pid table full : %d\n", __func__, (i + 1));
			return;
		}
	}
#if defined(CONFIG_ARCH_TCC88XX)
#ifdef SUPPORT_PIDFILTER_INTERNAL
	PTSIF	tsif_regs;
	tsif_regs = (volatile PTSIF)tcc_p2v((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF0_BASE);

	tsif_regs->TSPID[pos] = 0;
	tsif_regs->TSPID[pos] = (pid & 0x1FFF) | Hw13;
#else
	HwGPSB_PIDT(pos) = 0;
	HwGPSB_PIDT(pos) = (pid & 0x1FFF) | ((TSIF_CH == 1) ? Hw30 : Hw29);
#endif
#elif defined(CONFIG_ARCH_TCC892X)
#ifdef SUPPORT_PIDFILTER_INTERNAL
	PTSIF	tsif_regs;
	int idx;

	tsif_regs = (volatile PTSIF)tcc_p2v(((TSIF_CH == 0) ? HwTSIF0_BASE : ((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF2_BASE)));

	idx = pos / 2;
	if(pos % 2) {
		tsif_regs->TSPID[idx].nREG &= 0xffff0000;
		tsif_regs->TSPID[idx].nREG |= (pid & 0x1FFF) | Hw13;
	} else {
		tsif_regs->TSPID[idx].nREG &= 0x0000ffff;
		tsif_regs->TSPID[idx].nREG |= ((pid & 0x1FFF) << 16) | Hw29;
	}
#else
	volatile unsigned long* PIDT;

	PIDT = (volatile unsigned long *)tcc_p2v(HwTSIF_PIDT(pos));
	*PIDT = 0x0;
	*PIDT = (pid & 0x1FFF) | ((TSIF_CH == 0) ? Hw29 : ((TSIF_CH == 1) ? Hw30 : Hw31));
#endif	
#endif

	instance->pidtable[pos] = pid;

	//printk("[%s] pidtable insert : %d-%d\n", __func__, pos, pid);
#endif

	spin_unlock(&instance->lock);
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_insert_pid);

void tcc_tsif_p_remove_pid(int pid)
{
	st_p_tsif *instance = &p_tsif_inst;
	int i, pos=0;		
	
	spin_lock(&instance->lock);
	
#ifdef GDMA	
#ifdef SUPPORT_PIDFILTER_INTERNAL
	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] == pid) {
			pos = i;
			break;
		}
		
		if(i == (PIDTABLE_MAXCNT - 1)) {
			printk("[%s] pid table not found : %d\n", __func__, pid);
			return;
		}
	}

#if defined(CONFIG_ARCH_TCC88XX)
	PTSIF	tsif_regs;
	tsif_regs = (volatile PTSIF)tcc_p2v((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF0_BASE);

	tsif_regs->TSPID[i] = 0;
#elif defined(CONFIG_ARCH_TCC892X)	
	PTSIF	tsif_regs;
	int idx;
	
	tsif_regs = (volatile PTSIF)tcc_p2v(((TSIF_CH == 0) ? HwTSIF0_BASE : ((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF2_BASE)));

	idx = pos / 2;
	if(pos % 2) {
		tsif_regs->TSPID[idx].nREG &= 0xffff0000;
	} else {
		tsif_regs->TSPID[idx].nREG &= 0x0000ffff;
	}
#endif

	instance->pidtable[pos] = 0xffff;
	
	//printk("[%s] pidtable remove : %d-%d\n", __func__, pos, pid);
#endif
#else
	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] == pid) {
			pos = i;
			break;
		}
		
		if(i == (PIDTABLE_MAXCNT - 1)) {
			printk("[%s] pid table not found : %d\n", __func__, pid);
			return;
		}
	}

#if defined(CONFIG_ARCH_TCC88XX)
#ifdef SUPPORT_PIDFILTER_INTERNAL
	PTSIF	tsif_regs;
	tsif_regs = (volatile PTSIF)tcc_p2v((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF0_BASE);

	tsif_regs->TSPID[i] = 0;
#else
	HwGPSB_PIDT(i) = 0;
#endif
#elif defined(CONFIG_ARCH_TCC892X)
#ifdef SUPPORT_PIDFILTER_INTERNAL
	PTSIF	tsif_regs;
	int idx;
	
	tsif_regs = (volatile PTSIF)tcc_p2v(((TSIF_CH == 0) ? HwTSIF0_BASE : ((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF2_BASE)));

	idx = pos / 2;
	if(pos % 2) {
		tsif_regs->TSPID[idx].nREG &= 0xffff0000;
	} else {
		tsif_regs->TSPID[idx].nREG &= 0x0000ffff;
	}
#else
	volatile unsigned long* PIDT;

	PIDT = (volatile unsigned long *)tcc_p2v(HwTSIF_PIDT(pos));
	*PIDT = 0x0;
#endif
#endif

	instance->pidtable[pos] = 0xffff;

	//printk("[%s] pidtable remove : %d-%d\n", __func__, pos, pid);
#endif

	spin_unlock(&instance->lock);
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_remove_pid);

void tcc_tsif_p_check_pid(void)
{
	st_p_tsif *instance = &p_tsif_inst;

	PTSIF	tsif_regs;
	int i, pos=0;

	spin_lock(&instance->lock);

	tsif_regs = (volatile PTSIF)tcc_p2v(((TSIF_CH == 0) ? HwTSIF0_BASE : ((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF2_BASE)));

	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] != 0xffff) {
			pos = i / 2;
			
			if(i % 2) {
				tsif_regs->TSPID[pos].nREG &= 0x0000ffff;
				tsif_regs->TSPID[pos].nREG |= (Hw13 | instance->pidtable[i]) << 16;
			} else {
				tsif_regs->TSPID[pos].nREG &= 0xffff0000;
				tsif_regs->TSPID[pos].nREG |= (Hw13 | instance->pidtable[i]);
			}
		}
	}

#if 0
	for(i=0; i<PIDTABLE_MAXCNT; i++) {
		if(instance->pidtable[i] != 0xffff) {
			pos = i / 2;
			printk("[%s] pid[%d] : %4d / 0x%08x : 0x%08x\n", __func__, i, instance->pidtable[i], (int)&tsif_regs->TSPID[pos].nREG, tsif_regs->TSPID[pos].nREG);
		}
	}
#endif

	spin_unlock(&instance->lock);
}

void tcc_tsif_p_set_packetcnt(int cnt)
{
	st_p_tsif *instance = &p_tsif_inst;
	struct tcc_tsif_handle *handle = &instance->handle;
	
	handle->dma_intr_packet_cnt = cnt;
	handle->dma_total_packet_cnt = ((TSIF_DMA_SIZE / MPEG_PACKET_SIZE) / cnt) * cnt;
	
	printk("[%s] packet count set : %d\n", __func__, cnt);	
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_set_packetcnt);

#ifdef TSIF_DEBUG
void tcc_tsif_p_debug(int mod)
{
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	PGPION	gpio_regs;
	PTSIF	tsif_regs;
	volatile unsigned int* tsif_port_reg;

#ifdef GDMA
	PGDMACTRL	gdma_regs;
#else
#if defined(CONFIG_ARCH_TCC892X)
	PTSIFDMA	dma_regs;
#else
	PGPSB		dma_regs;
#endif
#endif

#if defined(CONFIG_ARCH_TCC88XX)
	tsif_port_reg = (volatile unsigned int *)tcc_p2v(HwTSIF_PORTSEL_BASE);

	gpio_regs = (volatile PGPION)tcc_p2v(((TSIF_GPIO_PORT == 0xE) ? HwGPIOE_BASE : HwGPIOF_BASE));
	tsif_regs = (volatile PTSIF)tcc_p2v((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF0_BASE);

#ifdef GDMA
	gdma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE); 
#else
	dma_regs = (volatile PGPSB)tcc_p2v((TSIF_CH == 1) ? HwGPSBCH1_BASE : HwGPSBCH0_BASE); 
#endif	
#elif defined(CONFIG_ARCH_TCC892X)
	tsif_port_reg = (volatile unsigned int *)tcc_p2v(HwTSIF_TSCHS_BASE);
	tsif_regs = (volatile PTSIF)tcc_p2v((TSIF_CH == 0) ? HwTSIF0_BASE : ((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF2_BASE));

#if (TSIF_GPIO_PORT == GPIO_D_PORT_TSIFNUM)
	gpio_regs = (volatile PGPION)tcc_p2v(HwGPIOD_BASE);
#elif (TSIF_GPIO_PORT == GPIO_B0_PORT_TSIFNUM)
	gpio_regs = (volatile PGPION)tcc_p2v(HwGPIOB_BASE);
#elif (TSIF_GPIO_PORT == GPIO_B1_PORT_TSIFNUM)
	gpio_regs = (volatile PGPION)tcc_p2v(HwGPIOB_BASE);
#elif (TSIF_GPIO_PORT == GPIO_C_PORT_TSIFNUM)
	gpio_regs = (volatile PGPION)tcc_p2v(HwGPIOC_BASE);
#elif (TSIF_GPIO_PORT == GPIO_E_PORT_TSIFNUM)
	gpio_regs = (volatile PGPION)tcc_p2v(HwGPIOE_BASE);
#elif (TSIF_GPIO_PORT == GPIO_F_PORT_TSIFNUM)
	gpio_regs = (volatile PGPION)tcc_p2v(HwGPIOF_BASE);
#endif

#ifdef GDMA
	gdma_regs = (volatile PGDMACTRL)tcc_p2v(HwGDMA1_BASE); 
#else
	dma_regs = (volatile PTSIFDMA)tcc_p2v((TSIF_CH == 0) ? HwTSIF_DMA0_BASE : ((TSIF_CH == 1) ? HwTSIF_DMA1_BASE : HwTSIF_DMA2_BASE));
#endif
#endif

	if(mod == 0) {
		printk("\n####################################\n");
		printk("GPFN0  : 0x%08x > 0x%08x\n", (int)&gpio_regs->GPFN0, gpio_regs->GPFN0);
		printk("GPFN1  : 0x%08x > 0x%08x\n", (int)&gpio_regs->GPFN1, gpio_regs->GPFN1);
		printk("GPFN2  : 0x%08x > 0x%08x\n", (int)&gpio_regs->GPFN2, gpio_regs->GPFN2);
		printk("GPFN3  : 0x%08x > 0x%08x\n\n", (int)&gpio_regs->GPFN3, gpio_regs->GPFN3);
		printk("TSCR   : 0x%08x > 0x%08x\n", (int)&tsif_regs->TSCR, tsif_regs->TSCR);
		printk("TSIC   : 0x%08x > 0x%08x\n", (int)&tsif_regs->TSIC, tsif_regs->TSIC);
		printk("TSCHS  : 0x%08x > 0x%08x\n\n", (int)tsif_port_reg, *tsif_port_reg);
#ifdef GDMA
		printk("ST_SADR: 0x%08x > 0x%08x\n", (int)&gdma_regs->ST_SADR2, gdma_regs->ST_SADR2);
		printk("SPARAM : 0x%08x > 0x%08x\n", (int)&gdma_regs->SPARAM2, gdma_regs->SPARAM2);
		printk("ST_DADR: 0x%08x > 0x%08x\n", (int)&gdma_regs->ST_DADR2, gdma_regs->ST_DADR2);
		printk("DPARAM : 0x%08x > 0x%08x\n", (int)&gdma_regs->DPARAM2, gdma_regs->DPARAM2);
		printk("HCOUNT : 0x%08x > 0x%08x\n", (int)&gdma_regs->HCOUNT2, gdma_regs->HCOUNT2);
		printk("CHCTRL : 0x%08x > 0x%08x\n", (int)&gdma_regs->CHCTRL2, gdma_regs->CHCTRL2);
		printk("RPTCTRL: 0x%08x > 0x%08x\n", (int)&gdma_regs->RPTCTRL2, gdma_regs->RPTCTRL2);
		printk("EXTREQ : 0x%08x > 0x%08x\n", (int)&gdma_regs->EXTREQ2, gdma_regs->EXTREQ2);
#else
		printk("RXBASE : 0x%08x > 0x%08x\n", (int)&dma_regs->RXBASE, dma_regs->RXBASE);
		printk("PACKET : 0x%08x > 0x%08x\n", (int)&dma_regs->PACKET, dma_regs->PACKET);
		printk("DMACTR : 0x%08x > 0x%08x\n", (int)&dma_regs->DMACTR, dma_regs->DMACTR);
		printk("DMAICR : 0x%08x > 0x%08x\n", (int)&dma_regs->DMAICR, dma_regs->DMAICR);
#endif	
		printk("####################################\n\n");
	} else {
		printk("\nTSIS   : 0x%08x > 0x%08x\n", (int)&tsif_regs->TSIS, tsif_regs->TSIS);
#ifdef GDMA	
		printk("C_DADR : 0x%08x > 0x%08x\n", (int)&gdma_regs->C_DADR2, gdma_regs->C_DADR2);
		printk("HCOUNT : 0x%08x > 0x%08x\n", (int)&gdma_regs->HCOUNT2, gdma_regs->HCOUNT2);
#else
		printk("DMASTR : 0x%08x > 0x%08x\n",(int)&dma_regs->DMASTR, dma_regs->DMASTR);
		printk("DMAICR : 0x%08x > 0x%08x\n\n", (int)&dma_regs->DMAICR, dma_regs->DMAICR);
#endif
	}
#endif
}

int tcc_tsif_p_debug_thread(void *data)
{
	while (!kthread_should_stop()) {
		if(debug_time != 0) {
			if(debug_time == cur_time) {
				tcc_tsif_p_debug(1);
#if 0			
				if(debug_cnt++ > 5) {
					st_p_tsif *instance = &p_tsif_inst;
					struct tcc_tsif_handle *handle = &instance->handle;

					handle->dma_stop(handle);
					handle->dma_start(handle);

					printk("\n\t## dma restart...\n\n");
				}
#endif				
			} else {
				debug_cnt = 0;
			}
		}

		debug_time = cur_time;

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(CHK_TIME*HZ);
	}

	return 0;
}
#endif	//#ifdef TSIF_DEBUG

int tcc_tsif_p_start(void)
{
	st_p_tsif *instance = &p_tsif_inst;
	struct tcc_tsif_handle *handle = &instance->handle;

	if(instance->state == P_TSIF_STATE_DEINIT)
	{
		printk("[%s] Could not start : 0x%x\n", __func__, instance->state);
		return -EACCES;
	}

//20110526 koo : android tcc880x kernel에서는 module init 후에 tsif & gpsb1을 swret state 및 clk disable 상태로 만들기 때문에 tsif start 시 swreset cancel & clk enable 시켜줌
#if defined(CONFIG_ARCH_TCC88XX)
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), ON);
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), OFF);
	tca_ckc_setiobus(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), ENABLE);
#ifndef GDMA
	tca_ckc_setioswreset(RB_GPSBCONTROLLER1, ON);
	tca_ckc_setioswreset(RB_GPSBCONTROLLER1, OFF);
	tca_ckc_setiobus(RB_GPSBCONTROLLER1, ENABLE);
#endif
#elif defined(CONFIG_ARCH_TCC892X)
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), ON);
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), OFF);
	tca_ckc_setiopwdn(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), DISABLE);
#endif

//20120319 koo : android tcc880x kernel에서는 module init 후에 tsif & gpsb1을 swret state 및 clk disable 상태로 만들기 때문에 module init 시의 tsif & dma reg 설정 value가 
//				reset 되기 때문에 start 시 다시 설정.
	handle->hw_init(handle);

	handle->msb_first		= 0x01;

#ifdef GDMA
	handle->big_endian		= 0x01;		//1:big endian, 0:little endian
#else
	handle->big_endian		= 0x00;		//1:big endian, 0:little endian
#endif	
	handle->serial_mode		= instance->mode;
//20111114 koo : clk polarityy change
#if defined(CONFIG_iTV_FE_DEMOD_MODULE_LGDT3305) || defined(CONFIG_iTV_FE_DEMOD_MODULE_LGDT3305_MODULE)
	handle->clk_polarity	= 0x01;			//falling edge : lgdt3305
#else	
	handle->clk_polarity	= 0x00;			//rising edge : s5h1411
#endif
	handle->valid_polarity	= 0x01;
	handle->sync_polarity	= 0x00;
	handle->sync_delay		= 0x00;

	handle->mpeg_ts = Hw1 | Hw0;

	handle->dma_stop(handle);
	handle->tsif_set(handle);
	handle->clear_fifo_packet(handle);
	handle->dma_start(handle);
	handle->prev_q_pos = (int)handle->rx_dma.v_addr;
	
	instance->state = P_TSIF_STATE_START;

	tcc_tsif_p_check_pid();

#ifdef TSIF_DEBUG
	tcc_tsif_p_debug(0);
	if(debug_thread == NULL) {
		debug_time = 0;
		debug_cnt = 0;
		//debug_thread = kthread_run(tcc_tsif_p_debug_thread, NULL, "itv_debug_thread");
	}
#endif

	return 0;
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_start);

int tcc_tsif_p_stop(void)
{
	st_p_tsif *instance = &p_tsif_inst;
	struct tcc_tsif_handle *handle = &instance->handle;

#ifdef TSIF_DEBUG
	if(debug_thread != NULL) {
		kthread_stop(debug_thread); 
		debug_thread = NULL;
	}
#endif

	if(instance->state != P_TSIF_STATE_START)
		return 0;

	handle->dma_stop(handle);

	instance->state = P_TSIF_STATE_STOP;

//20110526 koo : tsif start 시 swreset cancel & clk enable 시켜주기 때문에 stop 시 다시 swreset state & clk disable 시켜줌.
#if defined(CONFIG_ARCH_TCC88XX)
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), ON);
	tca_ckc_setiobus(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), DISABLE);
#ifndef GDMA
	tca_ckc_setioswreset(RB_GPSBCONTROLLER1, ON);
	tca_ckc_setiobus(RB_GPSBCONTROLLER1, DISABLE);
#endif
#elif defined(CONFIG_ARCH_TCC892X)
	tca_ckc_setiopwdn(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), ENABLE);
#endif

	return 0;
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_stop);

int tcc_tsif_p_init(unsigned char **buffer_addr, unsigned int *real_buffer_size, unsigned int buffer_size, 
		unsigned int pkt_intr_cnt, unsigned int timing_mode, unsigned int pid_mode, unsigned int serial_mode, 
		void (*callback)(void *), void *cb_data) 
{
	int error = 0;

	st_p_tsif *instance = &p_tsif_inst;
	struct tcc_tsif_handle *handle = &instance->handle;
	struct tcc_tsif_regs *reg_addr;

	if(instance->state != P_TSIF_STATE_DEINIT)
	{
		printk("[%s] Already Init : 0x%x\n", __func__, instance->state);
		return -EACCES;
	}

#if defined(CONFIG_ARCH_TCC88XX)
	reg_addr = (struct tcc_tsif_regs *)((TSIF_CH == 0) ? HwTSIF0_BASE : HwTSIF1_BASE);
#elif defined(CONFIG_ARCH_TCC892X)
	reg_addr = (struct tcc_tsif_regs *)tcc_p2v(((TSIF_CH == 0) ? HwTSIF0_BASE : ((TSIF_CH == 1) ? HwTSIF1_BASE : HwTSIF2_BASE)));
#endif

	memset(instance, 0x00, sizeof(st_p_tsif));

	instance->callback = callback;
	instance->cb_data = cb_data;
	instance->mode = serial_mode;

	handle->dma_intr_packet_cnt = TSIF_DMA_PACKET_CNT;
	handle->dma_total_packet_cnt = ((TSIF_DMA_SIZE / MPEG_PACKET_SIZE) / handle->dma_intr_packet_cnt) * handle->dma_intr_packet_cnt;
	printk("tsif > dma size:0x%x / intr_cnt:%d / total_intr_cnt:%d\n", TSIF_DMA_SIZE, handle->dma_intr_packet_cnt, handle->dma_total_packet_cnt);	

#if defined(CONFIG_ARCH_TCC88XX)
	//20110321 koo : tcc93xx와 tcc88xx의 swreset value가 뒤바뀌어져 있지만 tca_ckc_set_iobus_swreset를 동일하게 사용하고 있음
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), ON);
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), OFF);
	tca_ckc_setiobus(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), ENABLE);
#ifndef GDMA
	//20110321 koo : tcc93xx와 tcc88xx의 swreset value가 뒤바뀌어져 있지만 tca_ckc_set_iobus_swreset를 동일하게 사용하고 있음
	tca_ckc_setioswreset((TSIF_CH == 0) ? RB_GPSBCONTROLLER0 : RB_GPSBCONTROLLER1, ON);
	tca_ckc_setioswreset((TSIF_CH == 0) ? RB_GPSBCONTROLLER0 : RB_GPSBCONTROLLER1, OFF);
	tca_ckc_setiobus((TSIF_CH == 0) ? RB_GPSBCONTROLLER0 : RB_GPSBCONTROLLER1, ENABLE);
#endif	
#elif defined(CONFIG_ARCH_TCC892X)
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), ON);
	tca_ckc_setioswreset(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), OFF);
	tca_ckc_setiopwdn(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), DISABLE);

//20111220 koo : tsif interrupt가 enable 되어있으면 tsrxirq intr이 발생시 ts쪽 interrupt가 발생되어 gdma 사용시 mask 시킴.
#ifdef GDMA
	{
		static volatile PPIC pPIC;
		pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
		pPIC->IEN1.bREG.TSIF = 0;
		pPIC->INTMSK1.bREG.TSIF = 0;
	}
#endif
#else
    // reset
	tca_ckc_set_iobus_swreset(RB_TSIFCONTROLLER, OFF);
	tca_ckc_set_iobus_swreset(RB_TSIFCONTROLLER, ON);
	tca_ckc_setiobus(RB_TSIFCONTROLLER, ENABLE);
#endif

	if(tca_tsif_init(handle,
				reg_addr,
				tsif_p_alloc_dma_buffer,
				tsif_p_free_dma_buffer,
				TSIF_DMA_SIZE,
				TSIF_CH,
				TSIF_DMA_CONTROLLER,
				TSIF_DMA_CH,
				TSIF_GPIO_PORT
				)) {
		printk("[%s] SPI initialize failed\n", __func__);
		error = -EBUSY;
		goto err_tsif_p;
	}

	error = request_irq(handle->irq, tsif_p_dma_handler, IRQF_SHARED, 
			"tcc-tsif", handle);

	if(error) 
		goto err_irq; 

	*buffer_addr = handle->rx_dma.v_addr;

#ifdef GDMA
	*real_buffer_size = TSIF_DMA_SIZE;
#else
	*real_buffer_size = handle->dma_total_packet_cnt * MPEG_PACKET_SIZE;
#endif
	
	{
		int i;

		for(i=0; i<32; i++) {
			instance->pidtable[i] = 0xffff;
			
#if defined(CONFIG_ARCH_TCC88XX)
			HwGPSB_PIDT(i) = 0;
#elif defined(CONFIG_ARCH_TCC892X)
			volatile unsigned long* PIDT;

			PIDT = (volatile unsigned long *)tcc_p2v(HwTSIF_PIDT(i));
			*PIDT = 0x0;
#endif
		}
	}	

	handle->hw_init(handle);
	
	handle->prev_q_pos = (int)handle->rx_dma.v_addr;
	instance->state = P_TSIF_STATE_INIT;

	spin_lock_init(&instance->lock);

	return 0;

err_irq:
	free_irq(handle->irq, handle);

err_tsif_p:
	
	tca_tsif_clean(handle);
	
#if defined(CONFIG_ARCH_TCC88XX)
	tca_ckc_setiobus(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), DISABLE);
	tca_ckc_setiobus((TSIF_CH == 0) ? RB_GPSBCONTROLLER0 : RB_GPSBCONTROLLER1, DISABLE);
#elif defined(CONFIG_ARCH_TCC892X)
	tca_ckc_setiopwdn(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), ENABLE);
#else
	tca_ckc_setiobus(RB_TSIFCONTROLLER, DISABLE);
#endif	

	return error;
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_init);

void tcc_tsif_p_deinit(void)
{
	st_p_tsif *instance = &p_tsif_inst;
	struct tcc_tsif_handle *handle = &instance->handle;

#ifdef TSIF_DEBUG
	if(debug_thread != NULL) {
		kthread_stop(debug_thread); 
		debug_thread = NULL;
	}
#endif

	if(instance->state == P_TSIF_STATE_DEINIT)
		return;

	free_irq(handle->irq, handle);
	
	tca_tsif_clean(handle);
	
#if defined(CONFIG_ARCH_TCC88XX)
	tca_ckc_setiobus(((TSIF_CH == 0) ? RB_TSIF0CONTROLLER : RB_TSIF1CONTROLLER), DISABLE);
	tca_ckc_setiobus((TSIF_CH == 0) ? RB_GPSBCONTROLLER0 : RB_GPSBCONTROLLER1, DISABLE);
#elif defined(CONFIG_ARCH_TCC892X)
	tca_ckc_setiopwdn(((TSIF_CH == 0) ? RB_TSIF0 : ((TSIF_CH == 1) ? RB_TSIF1 : RB_TSIF2)), ENABLE);
#else
	tca_ckc_setiobus(RB_TSIFCONTROLLER, DISABLE);
#endif	

	instance->state = P_TSIF_STATE_DEINIT;
}
EXPORT_SYMBOL_GPL(tcc_tsif_p_deinit);
