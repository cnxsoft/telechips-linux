/*
 * linux/drivers/spi/tcc_spi.c
 *
 * Author:  <linux@telechips.com>
 * Created: 1st April, 2009
 * Description: Driver for Telechips SPI Controllers
 *              SPI master mode
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>

#include <asm/io.h>
#include <asm/dma.h>
#include <mach/bsp.h>
#include <mach/tca_spi.h>


#define SPI_DMA_SIZE 2048
struct tca_spi_pri_handle {
    struct platform_device *pdev;
    int rx_len, rx_cnt;
    int tx_len, tx_cnt;
    struct {
        int xfer_pos;
        int value;
    } done;

    unsigned char stopping;
    wait_queue_head_t wait_q;
    struct list_head queue;
    struct spi_message *current_message;
    spinlock_t lock;
};

struct tca_spi_global_setting{
    int open;
    int irq_no;
    int reg_base;
    int gpsb_port;
    struct clk *gpsb_clk;
    const char *gpsb_clk_name;
    struct spi_device *current_spi;
};
static struct tca_spi_global_setting tcc_spi_info;

static int tcc_spi_open(struct spi_device *spi);
static void tcc_spi_close(struct spi_device *spi);

static ssize_t show_port(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "gpsb port : %d\n", tcc_spi_info.gpsb_port);
}

static ssize_t store_port(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct platform_device *pdev = to_platform_device(dev);
	u32 port;

	port = simple_strtoul(buf, (char **)NULL, 16);
	/* valid port: 0xB, 0xC, 0xD, 0xE */
	if (port > 20 ) {
		printk("%s%d: invalid port! (use 0xb/c/d/e)\n", pdev->name, pdev->id);
		return -EINVAL;
	}

	tcc_spi_info.gpsb_port = port;
	return count;
}
static DEVICE_ATTR(tcc_port, S_IRUSR|S_IWUSR, show_port, store_port);

static void tea_free_dma_linux(struct tea_dma_buf *tdma)
{
    if (tdma) {
        if (tdma->v_addr != 0) {
            dma_free_writecombine(0, tdma->buf_size, tdma->v_addr, tdma->dma_addr);
        }
        memset(tdma, 0, sizeof(struct tea_dma_buf));
    }
}

static int tea_alloc_dma_linux(struct tea_dma_buf *tdma, unsigned int size)
{
    int ret = -1;
    if (tdma) {
        tea_free_dma_linux(tdma);
        tdma->buf_size = size;
        tdma->v_addr = dma_alloc_writecombine(0, tdma->buf_size, &tdma->dma_addr, GFP_KERNEL);
        //printk("tcc_spi: alloc DMA buffer @0x%X(Phy=0x%X), size:%d\n",
        //       (unsigned int)tdma->v_addr,
        //       (unsigned int)tdma->dma_addr,
        //       tdma->buf_size);
        ret = tdma->v_addr ? 0 : 1;
    }
    return ret;
}

static inline void tcc_spi_set_clk(u32 en, u32 clk)
{
	if(en == ENABLE)
    	clk_set_rate(tcc_spi_info.gpsb_clk, clk*2);
}
static int tcc_spi_start_rxtx(struct spi_master *master, unsigned int flen)
{
    struct tca_spi_handle *tspi = spi_master_get_devdata(master);
    struct tca_spi_pri_handle *tpri = (struct tca_spi_pri_handle *)tspi->private_data;

    tpri->done.value = -EIO;
    //mutex_lock(&(tpri->pm_mutex));

    if ((flen > 0) && (flen <= tspi->rx_dma.buf_size)) {
        tpri->rx_len = flen;
        tpri->tx_len = flen;
        tpri->rx_cnt = 0;
        tpri->tx_cnt = 0;

        tspi->clear_fifo_packet(tspi);
        tspi->set_packet_cnt(tspi, flen);
        tspi->dma_start(tspi);

        if (wait_event_interruptible_timeout((tpri->wait_q),
                                             (tpri->rx_cnt == tpri->rx_len),
                                             msecs_to_jiffies(WAIT_TIME_FOR_DMA_DONE)) == 0) {
            tspi->dma_stop(tspi);
            printk("[%s] wait_event timeout  (%dms) !!!\n",
                   __func__, WAIT_TIME_FOR_DMA_DONE);
        } else {
            if (tpri->rx_cnt == tpri->rx_len) {
                tpri->done.value = 0;
            }
        }
    }

    tpri->current_message = NULL;
    //mutex_unlock(&(tpri->pm_mutex));

    return tpri->done.value;
}

static void tcc_spi_next_message(struct spi_master *master)
{
    struct tca_spi_handle *tspi = NULL;
    struct spi_message *msg = NULL;
    struct spi_device *spi = NULL;
    struct spi_transfer *xfer = NULL;
    unsigned int flen = 0;
    unsigned int copy_len = 0, total_len = 0;
    struct tca_spi_pri_handle *tpri = NULL;
    unsigned int bit_width = 0;

    unsigned int bits= 0;
	unsigned int clk_hz;

    tspi = spi_master_get_devdata(master);
    tpri = (struct tca_spi_pri_handle *)tspi->private_data;

    BUG_ON(tpri->current_message);

    msg = list_entry(tpri->queue.next, struct spi_message, queue);
    list_del_init(&(msg->queue));
    tpri->current_message = msg;

    spi = msg->spi;

    list_for_each_entry(xfer, &(msg->transfers), transfer_list) {
        tpri->done.xfer_pos = 0;
        tpri->done.value = 0;
        total_len = xfer->len;
		tspi->ctf = !xfer->cs_change;	/* verify continuous transfer mode */
	
		if (xfer->speed_hz != spi->max_speed_hz) {
			clk_hz = xfer->speed_hz ? xfer->speed_hz : spi->max_speed_hz;
			tcc_spi_set_clk(ENABLE, clk_hz);
		}
		
		if (xfer->bits_per_word != spi->bits_per_word) {
			bits = xfer->bits_per_word ? xfer->bits_per_word : spi->bits_per_word;
			if (bits != 8 && bits != 16 && bits != 32) {
				dev_dbg(&spi->dev, "setup: invalid bits_per_word %u\n", bits);
				return;
			}
			tspi->set_bit_width(tspi, bits);
		}

        do {
           copy_len = (total_len > tspi->rx_dma.buf_size) ? tspi->rx_dma.buf_size : total_len;
           tspi->tx_pkt_remain = (total_len > tspi->rx_dma.buf_size) ? 1 : 0 ;
           if (copy_len & 0x3) {
               copy_len %= 4;
               bit_width = tspi->regs->MODE;
               bit_width = ((bit_width >> 8) & 0x1F) + 1;
               tspi->set_bit_width(tspi, 8);
           }
 
           if (xfer->tx_buf) {
				if (tspi->flag) {
					tspi->regs->TXBASE = tspi->tx_dma.dma_addr;
					memcpy(tspi->tx_dma.v_addr, xfer->tx_buf + tpri->done.xfer_pos, copy_len);
					tspi->flag = 0;
				} else {
					tspi->regs->TXBASE = tspi->tx_dma_1.dma_addr;
					memcpy(tspi->tx_dma_1.v_addr, xfer->tx_buf + tpri->done.xfer_pos, copy_len);
					tspi->flag = 1;
				}
            }

            if (tcc_spi_start_rxtx(master, copy_len)) {
                tspi->clear_fifo_packet(tspi);
                goto lb_return;
            }

            if (xfer->rx_buf) {
                memcpy(xfer->rx_buf + tpri->done.xfer_pos, tspi->rx_dma.v_addr, copy_len);
            }

            if (bit_width) {
                tspi->set_bit_width(tspi, bit_width);
                bit_width = 0;
            }

            total_len -= copy_len;
            tpri->done.xfer_pos += copy_len;
        } while (total_len);
        flen += xfer->len;
    }

lb_return:
    msg->status = tpri->done.value;
    if (msg->status == 0) {
        msg->actual_length = flen;
    }
    msg->complete(msg->context);

    if (list_empty(&(tpri->queue)) || tpri->stopping) {
        ;
    } else {
        tcc_spi_next_message(master);
    }
}

static irqreturn_t tcc_spi_isr(int irq, void *dev_id)
{
    struct spi_master *master = dev_id;
    struct tca_spi_handle *tspi = spi_master_get_devdata(master);
    //unsigned long flags = 0;
    unsigned long dma_done_reg = 0;
    struct tca_spi_pri_handle *tpri = (struct tca_spi_pri_handle *)tspi->private_data;

    //local_save_flags(flags);
    //local_irq_disable();

    dma_done_reg = tspi->regs->DMAICR;

    if (dma_done_reg & Hw29) {
        tspi->dma_stop(tspi);
        tpri->tx_cnt = tpri->tx_len;
        tpri->rx_cnt = tpri->rx_len;
        wake_up(&(tpri->wait_q));
    }

    //local_irq_restore(flags);
    return IRQ_HANDLED;
}

#define MODEBITS (SPI_CPOL | SPI_CPHA | SPI_CS_HIGH | SPI_LSB_FIRST)
static int tcc_spi_setup(struct spi_device *spi)
{
    struct tca_spi_handle *tspi = NULL;
    unsigned int bits = spi->bits_per_word;
    struct tca_spi_pri_handle *tpri = NULL;
    if(tcc_spi_info.open == 0){
        if(tcc_spi_open(spi) != 0)
            return -EINVAL;
        tcc_spi_info.open = 1;
    }

    tspi = spi_master_get_devdata(spi->master);
    tpri = (struct tca_spi_pri_handle *)tspi->private_data;

    if (tpri->stopping)
        return -ESHUTDOWN;
    if (spi->chip_select > spi->master->num_chipselect) {
        dev_dbg(&spi->dev,
                "setup: invalid chipselect %u (%u defined)\n",
                spi->chip_select, spi->master->num_chipselect);
        return -EINVAL;
    }

    if (bits == 0) {
        spi->bits_per_word = bits = 32;
	}
    if (bits != 8 && bits != 16 && bits != 32) {
        dev_dbg(&spi->dev, "setup: invalid bits_per_word %u\n", bits);
        return -EINVAL;
    }
    tspi->set_bit_width(tspi, bits);

    if (spi->mode & ~MODEBITS) {
        dev_dbg(&spi->dev, "[%s]setup: unsupported mode bits %x\n",
                __func__, spi->mode & ~MODEBITS);
        return -EINVAL;
    }

    tca_spi_setCPOL(tspi->regs, (spi->mode & SPI_CPOL));
    tca_spi_setCPHA(tspi->regs, (spi->mode & SPI_CPHA));
    tca_spi_setCS_HIGH(tspi->regs, (spi->mode & SPI_CS_HIGH));
    tca_spi_setLSB_FIRST(tspi->regs, (spi->mode & SPI_LSB_FIRST));

	if (spi->max_speed_hz) {
		tcc_spi_set_clk(ENABLE, spi->max_speed_hz);
	} else {
		return -EINVAL;
	}

	//printk("%s: spi->mode(0x%x), spi->max_speed_hz(%d)\n", __func__, 
    return 0;
}

static int tcc_spi_transfer(struct spi_device *spi, struct spi_message *msg)
{
    struct tca_spi_handle *tspi = NULL;
    //unsigned long flags = 0;
    struct tca_spi_pri_handle *tpri = NULL;
	
    tspi = (struct tca_spi_handle *)spi_master_get_devdata(spi->master);
    tpri = (struct tca_spi_pri_handle *)tspi->private_data;

    if (unlikely(list_empty(&(msg->transfers)))) {
        return -EINVAL;
    }

    if (tpri->stopping) {
        return -ESHUTDOWN;
    }

    msg->status = -EINPROGRESS;
    msg->actual_length = 0;

    //spin_lock_irqsave(&(tpri->lock), flags);
    list_add_tail(&(msg->queue), &(tpri->queue));
    if (!tpri->current_message) {
        tcc_spi_next_message(spi->master);
    }
    //spin_unlock_irqrestore(&(tpri->lock), flags);
    return 0;
}

static void tcc_spi_cleanup(struct spi_device *spi)
{
    if (!spi->controller_state)
        return;
    if(tcc_spi_info.open){
        tcc_spi_info.open = 0;
        tcc_spi_close(spi);
    }
    dev_dbg(&spi->dev, "tcc_spi_cleanup\n");
}

static void tcc_spi_close(struct spi_device *spi)
{
	struct tca_spi_handle *tspi = NULL;
	struct tca_spi_pri_handle *tpri = NULL;
	struct spi_message *msg = NULL;

	tspi = spi_master_get_devdata(spi->master);

	tpri = (struct tca_spi_pri_handle *)tspi->private_data;
	spin_lock_irq(&(tpri->lock));
    tpri->stopping = 1;
    spin_unlock_irq(&tpri->lock);
    list_for_each_entry(msg, &(tpri->queue), queue) {
        msg->status = -ESHUTDOWN;
        msg->complete(msg->context);
    }
	if (tpri) { kfree(tpri); }
	
	free_irq(tspi->irq, spi->master);

	tca_spi_clean(tspi);
	tcc_spi_set_clk(DISABLE, spi->max_speed_hz);
	if(tcc_spi_info.gpsb_clk)
		clk_disable(tcc_spi_info.gpsb_clk);
}

static int tcc_spi_open(struct spi_device *spi)
{
	int ret = 0;
	struct spi_master *master = NULL;
	struct platform_device *pdev = NULL;
	struct tca_spi_handle *tspi = NULL;
	struct tca_spi_pri_handle *tpri = NULL;
	int port;

	master = spi->master;
    tcc_spi_info.gpsb_clk = clk_get(NULL, tcc_spi_info.gpsb_clk_name);
    tcc_spi_info.current_spi =  spi;
    if(tcc_spi_info.gpsb_clk == NULL)
        goto err;

    clk_enable(tcc_spi_info.gpsb_clk); 
   	clk_set_rate(tcc_spi_info.gpsb_clk, spi->max_speed_hz);

	tspi = spi_master_get_devdata(spi->master);
	tpri = (struct tca_spi_pri_handle *)kmalloc(sizeof(struct tca_spi_pri_handle), GFP_KERNEL);
    if (tpri == NULL) {
       return -ENOMEM; 
    }
    memset(tpri, 0, sizeof(struct tca_spi_pri_handle));

	/* GPSB port */
	port = tcc_spi_info.gpsb_port;

	if (tca_spi_init(tspi,
					(volatile struct tca_spi_regs *)tcc_spi_info.reg_base,
					tcc_spi_info.irq_no,
					tea_alloc_dma_linux,
					tea_free_dma_linux,
					SPI_DMA_SIZE,
					master->bus_num,
					0,
					port,
					tcc_spi_info.gpsb_clk_name)) {
		goto err;
	}

	tspi->private_data = (void *)tpri;
	tpri->pdev = pdev;

	spin_lock_init(&(tpri->lock));
	INIT_LIST_HEAD(&(tpri->queue));

	tspi->clear_fifo_packet(tspi);
	tspi->dma_stop(tspi);
	init_waitqueue_head(&(tpri->wait_q));

	tspi->hw_init(tspi);

	ret = request_irq(tspi->irq, tcc_spi_isr, IRQF_SHARED, dev_name(&(master->dev)), master);
	if (ret) { goto err; }

	return 0;

err:
	printk("%s: error!!!\n", __func__);
	tca_spi_clean(tspi);
     if(tcc_spi_info.gpsb_clk)     {
    	clk_disable(tcc_spi_info.gpsb_clk);	
        tcc_spi_info.gpsb_clk =  NULL;
     }
	if (tpri) { kfree(tpri); }
	return ret;
}

static int __init tcc_spi_probe(struct platform_device *pdev)
{
	int ret = 0;
	int irq = -1;
    struct resource *regs = NULL;
    struct resource *port = NULL;
    struct spi_master *master = NULL;

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

    master = spi_alloc_master(&(pdev->dev), sizeof(struct tca_spi_handle));
    if (!master) {
        ret = -ENOMEM;
        goto err;
    }

    master->bus_num = pdev->id;
    master->num_chipselect = 1;
    master->mode_bits = MODEBITS;
    master->setup = tcc_spi_setup;
    master->transfer = tcc_spi_transfer;
    master->cleanup = tcc_spi_cleanup;

    memset(&tcc_spi_info, 0x0, sizeof(struct tca_spi_global_setting));

    tcc_spi_info.irq_no = irq;
    tcc_spi_info.reg_base = regs->start;
    tcc_spi_info.gpsb_port = port->start; //gpsb port

    tcc_spi_info.gpsb_clk_name = port->name;
	printk("%s:[%s]\n",__func__, tcc_spi_info.gpsb_clk_name);

    platform_set_drvdata(pdev, master);

    ret = spi_register_master(master);
    if (ret) { goto err; }

	ret = device_create_file(&pdev->dev, &dev_attr_tcc_port);

    printk("%s%d: init[%d]\n", pdev->name, pdev->id, ret);
    return 0;

err:
    spi_master_put(master);

    return ret;
}

static int tcc_spi_remove(struct platform_device *pdev)
{
    struct spi_master *master = platform_get_drvdata(pdev);
    spi_unregister_master(master);
	device_remove_file(&pdev->dev, &dev_attr_tcc_port);
    return 0;
}

static int tcc_spi_suspend(struct platform_device *pdev, pm_message_t state)
{
    if(tcc_spi_info.open){
        tcc_spi_info.open = 0;
        tcc_spi_close(tcc_spi_info.current_spi);
    }
	return 0;
}
static int tcc_spi_resume(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver tcc_spidrv = {
	.remove		= tcc_spi_remove,
	.suspend	= tcc_spi_suspend,
	.resume		= tcc_spi_resume,
    .driver		= {
        .name	= "tcc-spi",
        .owner	= THIS_MODULE,
    },
};

static int __init tcc_spi_init(void)
{
	printk("%s\n", __func__);
	return platform_driver_probe(&tcc_spidrv, tcc_spi_probe);
}

static void __exit tcc_spi_exit(void)
{
    platform_driver_unregister(&tcc_spidrv);
}
module_init(tcc_spi_init);
module_exit(tcc_spi_exit);

MODULE_AUTHOR("Telechips Inc. linux@telechips.com");
MODULE_DESCRIPTION("Telechips GPSB Master (SPI) Driver");
MODULE_LICENSE("GPL");
