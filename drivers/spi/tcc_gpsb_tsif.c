/*
 * linux/drivers/spi/tcc9200_tsif.c
 *
 * Author:  <linux@telechips.com>
 * Created: 1st April, 2009
 * Description: Driver for Telechips SPI (GPSB) Controllers
 *              SPI slave mode for DXB TSIF (Transport Stream Interface) modules
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
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
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <asm/mach-types.h>

#include <mach/bsp.h>
#include <mach/gpio.h>
#include <linux/spi/tcc_tsif.h>
#include <mach/tca_spi.h>
#include "tsdemux/TSDEMUX_sys.h"
#define      SUPPORT_TSIF_BLOCK

#define USE_STATIC_DMA_BUFFER
#undef  TSIF_DMA_SIZE
#define TSIF_DMA_SIZE 0x200000

extern int tsif_ex_init(void);
extern void tsif_ex_exit(void);
extern int tcc_ex_tsif_open(struct inode *inode, struct file *filp);
extern int tcc_ex_tsif_release(struct inode *inode, struct file *filp);
extern int tcc_ex_tsif_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
extern int tcc_ex_tsif_set_external_tsdemux(int (*decoder)(char *p1, int p1_size, char *p2, int p2_size), int max_dec_packet);

static struct tea_dma_buf *g_static_dma;
static int g_use_tsif_block = 0;
static char g_use_tsif_export_ioctl = 0;



static struct clk *gpsb_clk;


struct tca_spi_pri_handle {
    wait_queue_head_t wait_q;
    struct mutex mutex;
    int open_cnt;
	u32 gpsb_port;
	u32 gpsb_channel;
	u32 reg_base;
	u32 drv_major_num;
	u32 pcr_pid;
	u32 bus_num;
	u32 irq_no;
	u32 is_suspend;  //0:not in suspend, 1:in suspend
	u32 packet_read_count;
	const char *name;
};

static tca_spi_handle_t tsif_handle;
static struct tca_spi_pri_handle tsif_pri;
static int tcc_tsif_init(void);
static void tcc_tsif_deinit(void);
//External Decoder :: Send packet to external kernel ts demuxer
typedef struct
{
    int is_active; //0:don't use external demuxer, 1:use external decoder
    int index;
    int call_decoder_index;
    int (*tsdemux_decoder)(char *p1, int p1_size, char *p2, int p2_size)
}tsdemux_extern_t;
static tsdemux_extern_t tsdemux_extern_handle;
static int tsif_get_readable_cnt(tca_spi_handle_t *H);

static int tcc_tsif_set_external_tsdemux(int (*decoder)(char *p1, int p1_size, char *p2, int p2_size), int max_dec_packet)
{
    if(g_use_tsif_block)
	{
		tcc_ex_tsif_set_external_tsdemux(decoder, max_dec_packet);
	}
	else
	{
		if(max_dec_packet == 0)
		{
			//turn off external decoding
			memset(&tsdemux_extern_handle, 0x0, sizeof(tsdemux_extern_t));
			return 0;
		}

		if(tsdemux_extern_handle.call_decoder_index != max_dec_packet)
		{
			tsdemux_extern_handle.is_active = 1;
			tsdemux_extern_handle.tsdemux_decoder = decoder;
			tsdemux_extern_handle.index = 0;
			if(tsif_handle.dma_intr_packet_cnt < max_dec_packet)
				tsdemux_extern_handle.call_decoder_index = max_dec_packet; //every max_dec_packet calling isr, call decoder
			else
				tsdemux_extern_handle.call_decoder_index = 1;
			printk("%s::%d::max_dec_packet[%d]int_packet[%d]\n", __func__, __LINE__, tsdemux_extern_handle.call_decoder_index, tsif_handle.dma_intr_packet_cnt);
		}
	}
    return 0;
}
EXPORT_SYMBOL(tcc_tsif_set_external_tsdemux);

static char is_use_tsif_export_ioctl(void)
{
	return g_use_tsif_export_ioctl;
}

static ssize_t show_port(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "gpsb port : %d\n", tsif_pri.gpsb_port);

}
static ssize_t store_port(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	u32 port;

	port = simple_strtoul(buf, (char **)NULL, 16);
	/* valid port: 0xC, 0xD, 0xE */
	if (port > 20 ) {
		printk("tcc-tsif: invalid port! (use 0xc/d/e)\n");
		return -EINVAL;
	}

	tsif_pri.gpsb_port = port;
	return count;

}
static DEVICE_ATTR(tcc_port, S_IRUSR|S_IWUSR, show_port, store_port);

static int __init tsif_drv_probe(struct platform_device *pdev)
{

	int ret = 0;
	int irq = -1;
    struct resource *regs = NULL;
    struct resource *port = NULL;

    regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!regs) {
        return -ENXIO;
    }
    irq = platform_get_irq(pdev, 0);
    if (irq < 0) {
        return -ENXIO;
    }
    port = platform_get_resource(pdev, IORESOURCE_IO, 0);
    if (!port) {
        return -ENXIO;
    }

	tsif_pri.bus_num =  pdev->id;
	tsif_pri.irq_no = irq;
	tsif_pri.reg_base = regs->start;
	tsif_pri.gpsb_port = port->start;
	tsif_pri.name = port->name;

	printk("%s:[%s]\n",__func__, tsif_pri.name);
    gpsb_clk = clk_get(NULL, tsif_pri.name);

	platform_set_drvdata(pdev, &tsif_handle);

	/* TODO: device_remove_file(&pdev->dev, &dev_attr_tcc_port); */
	ret = device_create_file(&pdev->dev, &dev_attr_tcc_port);

	//printk("[%s]%d: init port:%d re:%d\n", pdev->name, gTSIFCH, tsif_pri.gpio_port, ret);
	return 0;
}

static int tsif_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
    if(tsif_pri.is_suspend == 0)
    {
        tsif_pri.is_suspend = 1;
    }
	return 0;
}
static int tsif_drv_resume(struct platform_device *pdev)
{
    if(tsif_pri.is_suspend == 1)
    {
        tsif_pri.is_suspend = 0;
    }

	return 0;
}

static struct platform_driver tsif_platform_driver = {
	.driver = {
		.name	= "tcc-tsif",
		.owner	= THIS_MODULE,
	},
	.suspend = tsif_drv_suspend,
	.resume  = tsif_drv_resume,
};

static void tea_free_dma_linux(struct tea_dma_buf *tdma)
{
#ifdef      SUPORT_USE_SRAM
#else    
    if(g_static_dma){        
        return;
    }
    
    if (tdma) {
        if (tdma->v_addr != 0) {
            dma_free_writecombine(0, tdma->buf_size, tdma->v_addr, tdma->dma_addr);
        }
        memset(tdma, 0, sizeof(struct tea_dma_buf));
    }
#endif    
}

static int tea_alloc_dma_linux(struct tea_dma_buf *tdma, unsigned int size)
{
#ifdef      SUPORT_USE_SRAM
    tdma->buf_size = SRAM_TOT_PACKET*MPEG_PACKET_SIZE;
    tdma->v_addr = (void *)SRAM_VIR_ADDR;
    tdma->dma_addr = (unsigned int)SRAM_PHY_ADDR;        
    printk("tcc_tsif: alloc DMA buffer @0x%X(Phy=0x%X), size:%d\n",
               (unsigned int)tdma->v_addr,
               (unsigned int)tdma->dma_addr,
               tdma->buf_size);
    return 0;
#else    
    int ret = -1;
     if(g_static_dma){        
        tdma->buf_size = g_static_dma->buf_size;
        tdma->v_addr = g_static_dma->v_addr;
        tdma->dma_addr = g_static_dma->dma_addr;
        return 0;
    }

    if (tdma) {
        tea_free_dma_linux(tdma);
        tdma->buf_size = size;
        tdma->v_addr = dma_alloc_writecombine(0, tdma->buf_size, &tdma->dma_addr, GFP_KERNEL);
        printk("tcc_tsif: alloc DMA buffer @0x%X(Phy=0x%X), size:%d\n",
               (unsigned int)tdma->v_addr,
               (unsigned int)tdma->dma_addr,
               tdma->buf_size);
        ret = tdma->v_addr ? 0 : 1;
    }
    return ret;
#endif    
}

static irqreturn_t tcc_tsif_dma_handler(int irq, void *dev_id)
{
    struct tca_spi_handle *tspi = (struct tca_spi_handle *)dev_id;
    struct tca_spi_pri_handle *tpri = (struct tca_spi_pri_handle *)tspi->private_data;
    unsigned long dma_done_reg = 0;

    dma_done_reg = tspi->regs->DMAICR;    
    if (dma_done_reg & (Hw28 | Hw29)) {
        BITSET(tspi->regs->DMAICR, Hw29 | Hw28);
        if(tsdemux_extern_handle.is_active)
        {
            if (tpri->open_cnt > 0) 
            {
                if(tsdemux_extern_handle.tsdemux_decoder)
                {
                    if(++tsdemux_extern_handle.index >= tsdemux_extern_handle.call_decoder_index)
                    {
                        char *p1 = NULL, *p2 = NULL;
                        int p1_size = 0, p2_size = 0;                        
                        tspi->cur_q_pos = (int)(tspi->regs->DMASTR >> 17);
                        if( tspi->cur_q_pos > tsif_handle.q_pos )
                        {
                            p1 = (char *)tsif_handle.rx_dma.v_addr + tsif_handle.q_pos*TSIF_PACKET_SIZE;
                            p1_size = (tspi->cur_q_pos - tsif_handle.q_pos)*TSIF_PACKET_SIZE;
                        }
                        else
                        {
                            p1 = (char *)tsif_handle.rx_dma.v_addr + tsif_handle.q_pos*TSIF_PACKET_SIZE;
                            p1_size = (tspi->dma_total_packet_cnt - tsif_handle.q_pos)*TSIF_PACKET_SIZE;

                            p2 = (char *)tsif_handle.rx_dma.v_addr;
                            p2_size = tspi->cur_q_pos*TSIF_PACKET_SIZE;
                        }
                        if(tsdemux_extern_handle.tsdemux_decoder(p1, p1_size, p2, p2_size) == 0)
                        {
                            tsif_handle.q_pos = tspi->cur_q_pos;
                            tsdemux_extern_handle.index = 0;
                        }
                    }
                }
            }
        }

        if (tpri->open_cnt > 0) {
            tspi->cur_q_pos = (int)(tspi->regs->DMASTR >> 17);
            //Check PCR & Make STC
            if(tpri->pcr_pid < 0x1FFF ) {	
                int start_pos, search_pos, search_size;
                start_pos = tspi->cur_q_pos - tspi->dma_intr_packet_cnt;
                if(start_pos > 0 ) {
                    search_pos = start_pos;
                    search_size = tspi->dma_intr_packet_cnt;
                } else {
                    search_pos = 0;
                    search_size = tspi->cur_q_pos;
                }	
                TSDEMUX_MakeSTC((unsigned char *)tsif_handle.rx_dma.v_addr + search_pos*TSIF_PACKET_SIZE, search_size*TSIF_PACKET_SIZE, tpri->pcr_pid );				
            }			
            //check read count & wake_up wait_q
            if( tsif_get_readable_cnt(tspi) >= tpri->packet_read_count)
                wake_up(&(tpri->wait_q));
        }
    }
    return IRQ_HANDLED;
}

static int tsif_get_readable_cnt(tca_spi_handle_t *H)
{
    if (H) {
        int dma_pos = H->cur_q_pos;
        int q_pos = H->q_pos;
        int readable_cnt = 0;

        if (dma_pos > q_pos) {
            readable_cnt = dma_pos - q_pos;
        } else if (dma_pos < q_pos) {
            readable_cnt = H->dma_total_packet_cnt - q_pos;
            readable_cnt += dma_pos;
        } 
        return readable_cnt;
    }
    return 0;
}

static ssize_t tcc_tsif_read(struct file *filp, char *buf, size_t len, loff_t *ppos)
{
    int readable_cnt = 0, copy_cnt = 0;
    int copy_byte = 0;

    readable_cnt = tsif_get_readable_cnt(&tsif_handle);
    if (readable_cnt > 0) {
        
        if(tsif_pri.packet_read_count != len/TSIF_PACKET_SIZE)
            printk("set packet_read_count=%d\n", len/TSIF_PACKET_SIZE);

        tsif_pri.packet_read_count = len/TSIF_PACKET_SIZE;
        copy_byte = readable_cnt * TSIF_PACKET_SIZE;
        if (copy_byte > len) {
            copy_byte = len;
        }

        copy_byte -= copy_byte % TSIF_PACKET_SIZE;
        copy_cnt = copy_byte / TSIF_PACKET_SIZE;
        copy_cnt -= copy_cnt % tsif_handle.dma_intr_packet_cnt;
        copy_byte = copy_cnt * TSIF_PACKET_SIZE;

        if (copy_cnt >= tsif_handle.dma_intr_packet_cnt) {
            int offset = tsif_handle.q_pos * TSIF_PACKET_SIZE;
            if (copy_cnt > tsif_handle.dma_total_packet_cnt - tsif_handle.q_pos) {
                int first_copy_byte = (tsif_handle.dma_total_packet_cnt - tsif_handle.q_pos) * TSIF_PACKET_SIZE;
                int first_copy_cnt = first_copy_byte / TSIF_PACKET_SIZE;
                int second_copy_byte = (copy_cnt - first_copy_cnt) * TSIF_PACKET_SIZE;

                if (copy_to_user(buf, tsif_handle.rx_dma.v_addr + offset, first_copy_byte)) {
                    return -EFAULT;
                }
                if (copy_to_user(buf + first_copy_byte, tsif_handle.rx_dma.v_addr, second_copy_byte)) {
                    return -EFAULT;
                }

                tsif_handle.q_pos = copy_cnt - first_copy_cnt;
            } else {
                if (copy_to_user(buf, tsif_handle.rx_dma.v_addr + offset, copy_byte)) {
                    return -EFAULT;
                }

                tsif_handle.q_pos += copy_cnt;
                if (tsif_handle.q_pos >= tsif_handle.dma_total_packet_cnt) {
                    tsif_handle.q_pos = 0;
                }
            }
            return copy_byte;
        }
    }
    return 0;
}

static ssize_t tcc_tsif_write(struct file *filp, const char *buf, size_t len, loff_t *ppos)
{
    return 0;
}

static unsigned int tcc_tsif_poll(struct file *filp, struct poll_table_struct *wait)
{
    if (tsif_get_readable_cnt(&tsif_handle) >= tsif_pri.packet_read_count) {
		return  (POLLIN | POLLRDNORM);
    }

    poll_wait(filp, &(tsif_pri.wait_q), wait);
    if (tsif_get_readable_cnt(&tsif_handle) >= tsif_pri.packet_read_count) {
		return  (POLLIN | POLLRDNORM);
    }
    return 0;
}

static ssize_t tcc_tsif_copy_from_user(void *dest, void *src, size_t copy_size)
{
	int ret = 0;
	if(is_use_tsif_export_ioctl() == 1) {
		memcpy(dest, src, copy_size);
	} else {
		ret = copy_from_user(dest, src, copy_size);
	}
	return ret;
}

static ssize_t tcc_tsif_copy_to_user(void *dest, void *src, size_t copy_size)
{
	int ret = 0;
	if(is_use_tsif_export_ioctl() == 1) {
		memcpy(dest, src, copy_size);
	} else {
		ret = copy_to_user(dest, src, copy_size);
	}
	return ret;
}


static int tcc_tsif_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
//	printk("%s::%d::0x%X\n", __func__, __LINE__, cmd);	
    switch (cmd) {
    case IOCTL_TSIF_DMA_START:
        {
            struct tcc_tsif_param param;
            if (tcc_tsif_copy_from_user(&param, (void *)arg, sizeof(struct tcc_tsif_param))) {
                printk("cannot copy from user tcc_tsif_param in IOCTL_TSIF_DMA_START !!! \n");
                return -EFAULT;
            }

            if (((TSIF_PACKET_SIZE * param.ts_total_packet_cnt) > tsif_handle.dma_total_size)
                || (param.ts_total_packet_cnt <= 0)) {
                printk("so big ts_total_packet_cnt !!! \n");
                return -EFAULT;
            }

            tsif_handle.dma_stop(&tsif_handle);

            tca_spi_setCPHA(tsif_handle.regs, param.mode & SPI_CPHA);
            tca_spi_setCPOL(tsif_handle.regs, param.mode & SPI_CPOL);
            tca_spi_setCS_HIGH(tsif_handle.regs, param.mode & SPI_CS_HIGH);
            tca_spi_setLSB_FIRST(tsif_handle.regs, param.mode & SPI_LSB_FIRST);

			tsif_handle.dma_mode = param.dma_mode;
			if (tsif_handle.dma_mode == 0) {
				tsif_handle.set_mpegts_pidmode(&tsif_handle, 0);
			}

            tsif_handle.dma_total_packet_cnt = param.ts_total_packet_cnt;
            tsif_handle.dma_intr_packet_cnt = param.ts_intr_packet_cnt;
            tsif_pri.packet_read_count =  tsif_handle.dma_intr_packet_cnt;

            #ifdef      SUPORT_USE_SRAM
            tsif_handle.dma_total_packet_cnt = SRAM_TOT_PACKET;
            tsif_handle.dma_intr_packet_cnt = SRAM_INT_PACKET;
            #endif
            tsif_handle.clear_fifo_packet(&tsif_handle);
            tsif_handle.q_pos = tsif_handle.cur_q_pos = 0;

            tsif_handle.set_packet_cnt(&tsif_handle, MPEG_PACKET_SIZE);
            printk("interrupt packet count [%u]\n", tsif_handle.dma_intr_packet_cnt);
            tsif_handle.dma_start(&tsif_handle);
        }
        break;
		
    case IOCTL_TSIF_DMA_STOP:
            tsif_handle.dma_stop(&tsif_handle);
        break;
		
    case IOCTL_TSIF_GET_MAX_DMA_SIZE:
        {
            struct tcc_tsif_param param;
            param.ts_total_packet_cnt = tsif_handle.dma_total_size / TSIF_PACKET_SIZE;
            param.ts_intr_packet_cnt = 1;
            if (tcc_tsif_copy_from_user((void *)arg, (void *)&param, sizeof(struct tcc_tsif_param))) {
                printk("cannot copy to user tcc_tsif_param in IOCTL_TSIF_GET_MAX_DMA_SIZE !!! \n");
                return -EFAULT;
            }
        }
        break;
		
    case IOCTL_TSIF_SET_PID:
        {
            struct tcc_tsif_pid_param param;
            if (tcc_tsif_copy_from_user(&param, (void *)arg, sizeof(struct tcc_tsif_pid_param))) {
                printk("cannot copy from user tcc_tsif_pid_param in IOCTL_TSIF_SET_PID !!! \n");
                return -EFAULT;
            }
            ret = tca_spi_register_pids(&tsif_handle, param.pid_data, param.valid_data_cnt);
        } 
        break;
		
	case IOCTL_TSIF_DXB_POWER:
		{
			// the power control moves to tcc_dxb_control driver.
		}
		break;
	case IOCTL_TSIF_SET_PCRPID:		
		if (tcc_tsif_copy_from_user((void *)&tsif_pri.pcr_pid, (const void *)arg, sizeof(int))) {
			return -EFAULT;
		}		
		printk("Set PCR PID[0x%X]\n", tsif_pri.pcr_pid);
		if( tsif_pri.pcr_pid < 0x1FFF)
			TSDEMUX_Open();
		break;
	case IOCTL_TSIF_GET_STC:	
		{
			unsigned int uiSTC;
			uiSTC = TSDEMUX_GetSTC();
			//printk("STC %d\n", uiSTC);
			if (tcc_tsif_copy_to_user((void *)arg, (void *)&uiSTC, sizeof(int))) {
                printk("cannot copy to user tcc_tsif_param in IOCTL_TSIF_GET_PCR !!! \n");
                return -EFAULT;
            }
		}
		break;	
    case IOCTL_TSIF_RESET:
        break;
    default:
        printk("tsif: unrecognized ioctl (0x%X)\n", cmd);
        ret = -EINVAL;
        break;
    }
    return ret;
}

static int tcc_export_tsif_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
	//	printk("%s::%d::0x%X\n", __func__, __LINE__, cmd);	
    if(g_use_tsif_block)
	{
		ret = tcc_ex_tsif_ioctl(filp, cmd, arg);
	}
	else
	{
		g_use_tsif_export_ioctl = 1;
		ret = tcc_tsif_ioctl(filp, cmd, arg);
		g_use_tsif_export_ioctl = 0;
	}
    return ret;
}
EXPORT_SYMBOL(tcc_export_tsif_ioctl);
	
static int tcc_tsif_init(void)
{
	int ret = 0;
    memset(&tsif_handle, 0, sizeof(tca_spi_handle_t));
	if(gpsb_clk == NULL)
        return -1;

    clk_enable(gpsb_clk); 
	if (tca_spi_init(&tsif_handle,
					 (volatile struct tca_spi_regs *)tsif_pri.reg_base,
                     tsif_pri.irq_no,
                     tea_alloc_dma_linux,
                     tea_free_dma_linux,
                     TSIF_DMA_SIZE,
                     tsif_pri.bus_num,
                     1,
					 tsif_pri.gpsb_port,
					 tsif_pri.name)) {
        printk("%s: tca_spi_init error !!!!!\n", __func__);
		ret = -EBUSY;
		goto err_spi;
    }
    
	tsif_handle.private_data = (void *)&tsif_pri;
    init_waitqueue_head(&(tsif_pri.wait_q));
    tsif_handle.clear_fifo_packet(&tsif_handle);
    tsif_handle.dma_stop(&tsif_handle);

    //init_waitqueue_head(&(tsif_pri.wait_q));
    //mutex_init(&(tsif_pri.mutex));

    tsif_handle.dma_total_packet_cnt = tsif_handle.dma_total_size / TSIF_PACKET_SIZE;
    tsif_handle.dma_intr_packet_cnt = 1;

    tsif_handle.hw_init(&tsif_handle);
	
    ret = request_irq(tsif_handle.irq, tcc_tsif_dma_handler, IRQF_SHARED, TSIF_DEV_NAME, &tsif_handle);
	if (ret) { 
		goto err_irq; 
	}

    tsif_handle.set_packet_cnt(&tsif_handle, MPEG_PACKET_SIZE);

	return 0;

err_irq:
	free_irq(tsif_handle.irq, &tsif_handle);
	
err_spi:
	tca_spi_clean(&tsif_handle);
	
	
	return ret;
}

static void tcc_tsif_deinit(void)
{
	free_irq(tsif_handle.irq, &tsif_handle);
    tca_spi_clean(&tsif_handle);
}

static int tcc_tsif_open(struct inode *inode, struct file *filp)
{
	int ret = 0;	

    if(g_use_tsif_block)
	{
		ret = tcc_ex_tsif_open(inode, filp);
	}
	else
	{
		if (tsif_pri.open_cnt == 0) {
	        tsif_pri.open_cnt++;
	    }
	    else {
	        return -EBUSY;
	    }
	   	if(gpsb_clk == NULL)
	        return -EBUSY;

	    clk_enable(gpsb_clk);
	    mutex_lock(&(tsif_pri.mutex));
		tsif_pri.pcr_pid = 0xFFFF;
	    tcc_tsif_init();
	    mutex_unlock(&(tsif_pri.mutex));
	}
	return ret;
}
EXPORT_SYMBOL(tcc_tsif_open);

static int tcc_tsif_release(struct inode *inode, struct file *filp)
{
	int ret = 0;	

    if(g_use_tsif_block)
	{
		ret = tcc_ex_tsif_release(inode, filp);
	}
	else
	{
		if (tsif_pri.open_cnt > 0) {
	    	tsif_pri.open_cnt--;
	    }	

	    if (tsif_pri.open_cnt == 0)
	    {
	    	mutex_lock(&(tsif_pri.mutex));
	        tsif_handle.dma_stop(&tsif_handle);
	        tcc_tsif_deinit();
		    TSDEMUX_Close();
	        mutex_unlock(&(tsif_pri.mutex));

	        clk_disable(gpsb_clk);
	    }
	}
	return ret;
}
EXPORT_SYMBOL(tcc_tsif_release);

struct file_operations tcc_tsif_fops =
{
    .owner          = THIS_MODULE,
    .read           = tcc_tsif_read,
    .write          = tcc_tsif_write,
    .unlocked_ioctl = tcc_tsif_ioctl,
    .open           = tcc_tsif_open,
    .release        = tcc_tsif_release,
    .poll           = tcc_tsif_poll,
};

static struct class *tsif_class;

static int __init tsif_init(void)
{
    int ret = 0;
#ifdef      SUPPORT_TSIF_BLOCK
	if(tca_tsif_can_support())
    {
        g_use_tsif_block = 1;
        tsif_ex_init();
        return 0;
    }
#endif
    memset(&tsif_pri, 0, sizeof(struct tca_spi_pri_handle));
    mutex_init(&(tsif_pri.mutex));
    ret = register_chrdev(0, TSIF_DEV_NAME, &tcc_tsif_fops);
    if (ret < 0) {
        printk("[%s:%d] register_chrdev error !!!!!\n", __func__, __LINE__); 
        return ret;
    }
    tsif_pri.drv_major_num = ret;
    printk("[%s:%d] major number = %d\n", __func__, __LINE__, tsif_pri.drv_major_num);
    g_static_dma = NULL;
#ifdef      USE_STATIC_DMA_BUFFER
     g_static_dma = kmalloc(sizeof(struct tea_dma_buf), GFP_KERNEL);
     if(g_static_dma)
     {
        g_static_dma->buf_size = TSIF_DMA_SIZE;
        g_static_dma->v_addr = dma_alloc_writecombine(0, g_static_dma->buf_size, &g_static_dma->dma_addr, GFP_KERNEL);
		printk("tcc-tsif : dma buffer alloc @0x%X(Phy=0x%X), size:%d\n", (unsigned int)g_static_dma->v_addr, (unsigned int)g_static_dma->dma_addr, g_static_dma->buf_size);
        if(g_static_dma->v_addr == NULL)
        {
            kfree(g_static_dma);
            g_static_dma = NULL;
        }
     }
#endif       

    ret = platform_driver_probe(&tsif_platform_driver, tsif_drv_probe);

    tsif_class = class_create(THIS_MODULE, TSIF_DEV_NAME);
    device_create(tsif_class, NULL, MKDEV(tsif_pri.drv_major_num, 1), NULL, TSIF_DEV_NAME);

	tsif_pri.pcr_pid = 0xFFFF;
	memset(&tsdemux_extern_handle, 0x0, sizeof(tsdemux_extern_t));
    return ret;
}

static void __exit tsif_exit(void)
{
    if(g_use_tsif_block)
    {
        g_use_tsif_block = 0;
        tsif_ex_exit();
        return;
    }

	unregister_chrdev(tsif_pri.drv_major_num, TSIF_DEV_NAME);
    platform_driver_unregister(&tsif_platform_driver);
	if(gpsb_clk)
	{
	    clk_disable(gpsb_clk);
    	clk_put(gpsb_clk);
	}
	if(g_static_dma)
    {
        dma_free_writecombine(0, g_static_dma->buf_size, g_static_dma->v_addr, g_static_dma->dma_addr);
        kfree(g_static_dma);
        g_static_dma = NULL;
    }
}
module_init(tsif_init);
module_exit(tsif_exit);

MODULE_AUTHOR("Telechips Inc. linux@telechips.com");
MODULE_DESCRIPTION("Telechips GPSB Slave (TSIF) driver");
MODULE_LICENSE("GPL");
