/*
 * linux/drivers/mmc/host/tcc_sdhc.c  
 *
 * Author:  <linux@telechips.com>
 * Created: Octo 18, 2010
 * Description: SD/MMC Host Driver for Telechips Boards.
 *
 * Copyright (C) 2008-2010 Telechips 
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/highmem.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/card.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/irq.h>

#include <linux/pci.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/scatterlist.h>
#include <asm/mach-types.h>


#include <mach/bsp.h>
#include <mach/irqs.h>
#include <mach/gpio.h>
#include <mach/tca_ckc.h>
#include <mach/mmc.h>
#include "tcc_sdhc.h"


#undef DEBUG_SD
#ifdef	DEBUG_SD
#define SD_DEBUG(x...)	printk(x)
#else
#define SD_DEBUG(x...)
#endif

#undef	DEBUG_CMD
#ifdef	DEBUG_CMD
#define SD_CMD(x...)	printk(x)
#else
#define SD_CMD(x...)
#endif

/* Clock Control */
#undef SD_CLOCK_CONTROL
#undef SD_CLOCK_CHANGE

#define DRIVER_NAME		"tcc-sdhc"
#define DRIVER_NAME_SDHC0	"tcc-sdhc0"
#define DRIVER_NAME_SDHC1	"tcc-sdhc1"
#define DRIVER_NAME_SDHC2	"tcc-sdhc2"
#define DRIVER_NAME_SDHC3	"tcc-sdhc3"

#define DETECT_TIMEOUT		(HZ/2)
#define SDMMC_FIFO_CNT		1024
#define SDMMC_TIMEOUT_TICKS	(1000*HZ/1000)	/* 1000ms */

static void tcc_mmc_start_command(struct tcc_mmc_host *host, struct mmc_command *cmd);
static void tcc_mmc_tasklet_finish(unsigned long param);
static void init_mmc_host(struct tcc_mmc_host *host);

static int tcc_sw_reset(struct tcc_mmc_host *host, uint8_t rst_bits)
{
	int timeout = 100;

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return -EHOSTDOWN;
	}

	writew(rst_bits<<8 | readw(host->iobase+TCCSDHC_TIMEOUT_CONTROL), host->iobase+TCCSDHC_TIMEOUT_CONTROL);
	
	while (--timeout) {
		if (!(readw(host->iobase+TCCSDHC_TIMEOUT_CONTROL)& rst_bits<<8)){
			break;
		}
		/* Must be mdelay not msleep, as called from interrupt context */
		mdelay(1);
	}
	if (!timeout) {
		printk(KERN_ERR "%s%d: timed out waiting for reset\n", DRIVER_NAME, host->id);
		return -ETIMEDOUT;
	}

	return 0;
}

int tcc_mmc_card_inserted(struct tcc_mmc_host *host)
{
	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return 0;//-EHOSTDOWN;
	}

	if (host->pdata->card_detect)
		return host->pdata->card_detect(host->dev, host->id);
	else
		return 0;	
}

/*
 * Card insert/remove timer handler
 */
static void tcc_mmc_poll_event(unsigned long data)
{
	struct tcc_mmc_host *host = (struct tcc_mmc_host *) data;	

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	if (!tcc_mmc_card_inserted(host)) {
		/* card removed */
		if (host->card_inserted == 1) {
			host->card_inserted = 0;
			host->card_changed = 1;
			SD_DEBUG("%s%d: card removed\n", DRIVER_NAME, host->id);

			if (host && host->card_changed) {
#ifdef SD_CLOCK_CONTROL
				if(host->id==0)
					clk_disable(host->fclk);
#endif		
				mmc_detect_change(host->mmc, msecs_to_jiffies(0));
				host->card_changed = 0;
			}
		}
	} else if (host->card_inserted == 0) {
		host->card_inserted = 1;
		host->card_changed = 1;		
		SD_DEBUG("%s%d: card inserted\n", DRIVER_NAME, host->id);

		if (host && host->card_changed) {
#ifdef SD_CLOCK_CONTROL		
			if(host->id==0)
				clk_enable(host->fclk);	// wifi always enable 
#endif
			mmc_detect_change(host->mmc, msecs_to_jiffies(100));
			host->card_changed = 0;
			SD_DEBUG("%s%d: mmc_detect_change\n", DRIVER_NAME, host->id);
		}
	}

	mod_timer(&host->detect_timer, jiffies + DETECT_TIMEOUT);
}

static char *tcc_mmc_kmap_atomic(struct scatterlist *sg, unsigned long *flags)
{
	if((sg == NULL)||(flags == NULL)) {
		printk("[mmc:NULL] %s(sg:%x, flags:%x)\n", __func__, (u32)sg, (u32)flags);
		return 0;
	}

	local_irq_save(*flags);
	return kmap_atomic(sg_page(sg), KM_BIO_SRC_IRQ) + sg->offset;
}

static void tcc_mmc_kunmap_atomic(void *buffer, unsigned long *flags)
{
	if((buffer == NULL)||(flags == NULL)) {
		printk("[mmc:NULL] %s(buffer:%x, flags:%x)\n", __func__, (u32)buffer, (u32)flags);
		return;
	}

	kunmap_atomic(buffer, KM_BIO_SRC_IRQ);
	local_irq_restore(*flags);
}

static int tcc_mmc_adma_table_pre(struct tcc_mmc_host *host, struct mmc_data *data)
{
	int direction;

	u8 *desc;
	u8 *align;
	dma_addr_t addr;
	dma_addr_t align_addr;
	int len, offset;

	struct scatterlist *sg;
	int i;
	char *buffer;
	unsigned long flags;

	if((host == NULL)||(data == NULL)) {
		printk("[mmc:NULL] %s(host:%x, data:%x)\n", __func__, (u32)host, (u32)data);
		return -EHOSTDOWN;
	}

	/*
	 * The spec does not specify endianness of descriptor table.
	 * We currently guess that it is LE.
	 */

	if (data->flags & MMC_DATA_READ)
		direction = DMA_FROM_DEVICE;
	else
		direction = DMA_TO_DEVICE;

	/*
	 * The ADMA descriptor table is mapped further down as we
	 * need to fill it with data first.
	 */

	host->align_addr = dma_map_single(mmc_dev(host->mmc),
			host->align_buffer, 128 * 4, direction);
	if (dma_mapping_error(mmc_dev(host->mmc), host->align_addr))
		goto fail;
	BUG_ON(host->align_addr & 0x3);

	host->sg_count = dma_map_sg(mmc_dev(host->mmc),
			data->sg, data->sg_len, direction);
	if (host->sg_count == 0)
		goto unmap_align;

	desc = host->adma_desc;
	align = host->align_buffer;

	align_addr = host->align_addr;

	for_each_sg(data->sg, sg, host->sg_count, i) {
		addr = sg_dma_address(sg);
		len = sg_dma_len(sg);

		/*
		 * The SDHCI specification states that ADMA
		 * addresses must be 32-bit aligned. If they
		 * aren't, then we use a bounce buffer for
		 * the (up to three) bytes that screw up the
		 * alignment.
		 */
		offset = (4 - (addr & 0x3)) & 0x3;
		if (offset) {
			if (data->flags & MMC_DATA_WRITE) {
				buffer = tcc_mmc_kmap_atomic(sg, &flags);
				WARN_ON(((long)buffer & PAGE_MASK) > (PAGE_SIZE - 3));
				memcpy(align, buffer, offset);
				tcc_mmc_kunmap_atomic(buffer, &flags);
			}

			desc[7] = (align_addr >> 24) & 0xff;
			desc[6] = (align_addr >> 16) & 0xff;
			desc[5] = (align_addr >> 8) & 0xff;
			desc[4] = (align_addr >> 0) & 0xff;

			BUG_ON(offset > 65536);

			desc[3] = (offset >> 8) & 0xff;
			desc[2] = (offset >> 0) & 0xff;

			desc[1] = 0x00;
			desc[0] = 0x21; /* tran, valid */

			align += 4;
			align_addr += 4;

			desc += 8;

			addr += offset;
			len -= offset;
		}

		desc[7] = (addr >> 24) & 0xff;
		desc[6] = (addr >> 16) & 0xff;
		desc[5] = (addr >> 8) & 0xff;
		desc[4] = (addr >> 0) & 0xff;

		BUG_ON(len > 65536);

		desc[3] = (len >> 8) & 0xff;
		desc[2] = (len >> 0) & 0xff;

		desc[1] = 0x00;
		desc[0] = 0x21; /* tran, valid */

		desc += 8;

		/*
		 * If this triggers then we have a calculation bug
		 * somewhere. :/
		 */
		WARN_ON((desc - host->adma_desc) > (128 * 2 + 1) * 4);
	}

	desc -= 8;

	/*
	 * Add a terminating entry.
	 */
//	desc[7] = 0;
//	desc[6] = 0;
//	desc[5] = 0;
//	desc[4] = 0;
//	desc[3] = 0;
//	desc[2] = 0;
//	desc[1] = 0x00;
	desc[0] = 0x27; /* nop, end, valid */

	/*
	 * Resync align buffer as we might have changed it.
	 */
	if (data->flags & MMC_DATA_WRITE) {
		dma_sync_single_for_device(mmc_dev(host->mmc),
				host->align_addr, 128 * 4, direction);
	}

	host->adma_addr = dma_map_single(mmc_dev(host->mmc),	host->adma_desc, (128 * 2 + 1) * 4, DMA_TO_DEVICE);

//	printk("ADMA address: 0x%x\n",host->adma_addr);

	if (dma_mapping_error(mmc_dev(host->mmc), host->adma_addr))
		goto unmap_entries;
	BUG_ON(host->adma_addr & 0x3);

	return 0;

unmap_entries:
	dma_unmap_sg(mmc_dev(host->mmc), data->sg,
			data->sg_len, direction);
unmap_align:
	dma_unmap_single(mmc_dev(host->mmc), host->align_addr,
			128 * 4, direction);
fail:
	return -EINVAL;
}

static void tcc_mmc_adma_table_post(struct tcc_mmc_host *host, struct mmc_data *data)
{
	int direction;

	struct scatterlist *sg;
	int i, size;
	u8 *align;
	char *buffer;
	unsigned long flags;

	if((host == NULL)||(data == NULL)) {
		printk("[mmc:NULL] %s(host:%x, data:%x)\n", __func__, (u32)host, (u32)data);
		return;// -EHOSTDOWN;
	}

	if (data->flags & MMC_DATA_READ)
		direction = DMA_FROM_DEVICE;
	else
		direction = DMA_TO_DEVICE;

	dma_unmap_single(mmc_dev(host->mmc), host->adma_addr,
			(128 * 2 + 1) * 4, DMA_TO_DEVICE);

	dma_unmap_single(mmc_dev(host->mmc), host->align_addr,
			128 * 4, direction);

	if (data->flags & MMC_DATA_READ) {
		dma_sync_sg_for_cpu(mmc_dev(host->mmc), data->sg,
				data->sg_len, direction);

		align = host->align_buffer;

		for_each_sg(data->sg, sg, host->sg_count, i) {
			if (sg_dma_address(sg) & 0x3) {
				size = 4 - (sg_dma_address(sg) & 0x3);

				buffer = tcc_mmc_kmap_atomic(sg, &flags);
				WARN_ON(((long)buffer & PAGE_MASK) > (PAGE_SIZE - 3));
				memcpy(buffer, align, size);
				tcc_mmc_kunmap_atomic(buffer, &flags);

				align += 4;
			}
		}
	}

	dma_unmap_sg(mmc_dev(host->mmc), data->sg,
			data->sg_len, direction);
}

static void tcc_mmc_finish_data(struct tcc_mmc_host *host)
{
	struct mmc_data *data;
	u16 blocks;

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	BUG_ON(!host->data);

	data = host->data;
	host->data = NULL;

	if (host->flags & TCC_MMC_REQ_USE_DMA) {
		if (!host->is_direct) {
			if (host->flags & TCC_MMC_USE_ADMA) {
				tcc_mmc_adma_table_post(host, data);
			} else {
				dma_unmap_sg(NULL, data->sg, data->sg_len,
					(data->flags & MMC_DATA_READ)?DMA_FROM_DEVICE:DMA_TO_DEVICE);
			}
		} else {
			dma_unmap_sg(NULL, data->sg, data->sg_len,
				(data->flags & MMC_DATA_READ)?DMA_FROM_DEVICE:DMA_TO_DEVICE);
		}
	}

	/*
	 * Controller doesn't count down when in single block mode.
	 */
	if (data->blocks == 1) {
		blocks = (data->error == 0) ? 0 : 1;
	} else {
		blocks = readw(host->iobase+TCCSDHC_BLOCK_COUNT);
	}
	data->bytes_xfered = data->blksz * (data->blocks - blocks);

	if (!data->error && blocks) {
		printk(KERN_ERR "%s: Controller signalled completion even "
				"though there were blocks left.\n",
				mmc_hostname(host->mmc));
		data->error = -EIO;
	}

	/* It must be enabled. When It doesn't sends STOP command */
		
	//if (host->stop_cmd) {
	//	if (data->stop) {
	//		/*
	//		 * The controller needs a reset of internal state machines
	//		 * upon error conditions.
	//		 */
	//		if (data->error) {
	//			tcc_sw_reset(host, HwSD_SRESET_RSTCMD);
	//			tcc_sw_reset(host, HwSD_SRESET_RSTDAT);
	//		}
	//		tcc_mmc_start_command(host, data->stop);
	//	} else {
	//		tasklet_schedule(&host->finish_tasklet);
	//	}
	//	host->stop_cmd = 0;
	//}

	tasklet_schedule(&host->finish_tasklet);
}

//FIXME: Not yet implemented...
static void tcc_transfer_pio(struct tcc_mmc_host *host)
{
	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	BUG_ON(!host->data);
	SD_DEBUG("enter %s  ", __func__);
}

static void tcc_mmc_start_command(struct tcc_mmc_host *host, struct mmc_command *cmd)
{
	u32 resptype;
	u32 cmdtype;
	u32 mask;
	unsigned long timeout;    
	int cmd_reg = 0x00000000;
	unsigned int uiIntStatusEn;

	if((host == NULL)||(cmd == NULL)) {
		printk("[mmc:NULL] %s(host:%x, cmd:%x)\n", __func__, (u32)host, (u32)cmd);
		return;// -EHOSTDOWN;
	}

	/* Wait max 10 ms */
#if defined(CONFIG_ATHEROS_WIFI)
	timeout = jiffies + HZ/100;
#else 
	timeout = 10;    
#endif 

	cmdtype = 0;

	mask = HwSD_STATE_NOCMD;
	if ((cmd->data != NULL) || (cmd->flags & MMC_RSP_BUSY)) {
		mask |= HwSD_STATE_NODAT;
	}

	/* We shouldn't wait for data inihibit for stop commands, even
	   though they might use busy signaling */
	if (host->mrq->data && (cmd == host->mrq->data->stop)) {
		mask &= ~HwSD_STATE_NODAT;
	}

	while (readl(host->iobase+TCCSDHC_PRESENT_STATE) & mask) {
#if defined(CONFIG_ATHEROS_WIFI)
		if (time_is_after_jiffies(timeout)) {
#else
		if (timeout == 0) {
#endif
			printk(KERN_ERR "%s: Controller never released "
					"inhibit bit(s).\n", mmc_hostname(host->mmc));
			cmd->error = -EIO;
			tasklet_schedule(&host->finish_tasklet);
			return;
		}
#ifndef CONFIG_ATHEROS_WIFI /* atheros*/
		timeout--;
		mdelay(1);
#endif 
	}

	mod_timer(&host->timer, jiffies + 10 * HZ);

	host->cmd = cmd;

	switch (mmc_resp_type(cmd)) {
		case MMC_RSP_NONE:
			resptype = 0;
			break;
		case MMC_RSP_R1:
			resptype = 2;
			break;
		case MMC_RSP_R1B:
			resptype = 3;
			break;
		case MMC_RSP_R2:
			resptype = 1;
			break;
		default:
			resptype = 2;
			break;
	}

	uiIntStatusEn = readl(host->iobase+TCCSDHC_INT_ENABLE);

	uiIntStatusEn |= HwSDINT_EN_TDONE | HwSDINT_EN_CDONE;

	if (cmd->data) {
		host->data = cmd->data;
		host->data_early = 0;

		cmd_reg |= HwSD_COM_TRANS_DATSEL | HwSD_COM_TRANS_DIR;

		if (cmd->data->blocks > 1) {			
			cmd_reg |= HwSD_COM_TRANS_MS | HwSD_COM_TRANS_BCNTEN;
			if (cmd->opcode != SD_IO_RW_EXTENDED) {
				cmd_reg |= HwSD_COM_TRANS_ACMD12;  // It is related STOP command
			}
		}

		if (cmd->data->flags & MMC_DATA_WRITE)
			cmd_reg &= ~HwSD_COM_TRANS_DIR;

		if (host->flags & TCC_MMC_USE_DMA)
			host->flags |= TCC_MMC_REQ_USE_DMA;

		if (host->flags & TCC_MMC_REQ_USE_DMA)
			cmd_reg |= HwSD_COM_TRANS_DMAEN;

		/*
		 * Always adjust the DMA selection as some controllers
		 * (e.g. JMicron) can't do PIO properly when the selection
		 * is ADMA.
		 */
		if (host->flags & TCC_MMC_USE_DMA) {
			unsigned short ctrl = readw(host->iobase+TCCSDHC_HOST_CONTROL);
			ctrl &= ~HwSD_CTRL_DMA_MASK;

			if (!host->is_direct) {
				if ((host->flags & TCC_MMC_REQ_USE_DMA) && (host->flags & TCC_MMC_USE_ADMA))
					ctrl |= HwSD_CTRL_ADMA32;
				else
					ctrl |= HwSD_CTRL_SDMA;
			} else {
				ctrl |= HwSD_CTRL_SDMA;
			}
			
			writew(ctrl, host->iobase+TCCSDHC_HOST_CONTROL);
		}

		if (host->flags & TCC_MMC_REQ_USE_DMA) {
			if (!host->is_direct) {
				if (host->flags  & TCC_MMC_USE_ADMA) {
					tcc_mmc_adma_table_pre(host,cmd->data);
					writel(host->adma_addr, host->iobase+TCCSDHC_ADMA_ADDRESS);
				} else {
					int count;
					count = dma_map_sg(NULL, cmd->data->sg, cmd->data->sg_len,
						(cmd->data->flags & MMC_DATA_READ)?DMA_FROM_DEVICE:DMA_TO_DEVICE);
					BUG_ON(count != 1);

					writel(sg_dma_address(cmd->data->sg), host->iobase+TCCSDHC_DMA_ADDRESS);
				}
			} else {
				writel(host->req->dma, host->iobase+TCCSDHC_DMA_ADDRESS);
			}
		} else {
			if (!host->is_direct) {
				host->cur_sg = cmd->data->sg;
				host->num_sg = cmd->data->sg_len;
				host->offset = 0;
				host->remain = host->cur_sg->length;
			} else {
				printk(KERN_ERR "%s: only support DMA!!!", mmc_hostname(host->mmc));
			}
		}
		/* reset is_direct */
		//host->is_direct = 0;
		
	}

	if (((cmd->opcode==SD_IO_RW_DIRECT) && ((cmd->arg & (0x03<<28)) ==0)  
				&& ((cmd->arg&(0x1ff<<9)) == (SDIO_CCCR_ABORT<<9)) && (cmd->arg&0x07)) 
			|| (cmd->opcode==MMC_STOP_TRANSMISSION)) {
		/* Use R5b For CMD52, Function 0, I/O Abort 
		 * Need to be revised for handling CMD12       
		 */
		cmdtype = HwSD_COM_TRANS_ABORT;
	}

	if (cmd->flags & MMC_RSP_CRC)
		cmd_reg |= HwSD_COM_TRANS_CRCHK;

	if (cmd->flags & MMC_RSP_OPCODE)
		cmd_reg |= HwSD_COM_TRANS_CICHK;

	cmd_reg |= (cmd->opcode << 24) |cmdtype| (resptype << 16);

	if (readl(host->iobase+TCCSDHC_INT_STATUS) & HwSDINT_STATUS_ERR) {
		writew(readw(host->iobase+TCCSDHC_TIMEOUT_CONTROL)|(HwSD_SRESET_RSTCMD<<8), host->iobase+TCCSDHC_TIMEOUT_CONTROL);
		while (readw(host->iobase+TCCSDHC_TIMEOUT_CONTROL) & HwSD_SRESET_RSTCMD<<8);
			msleep(1);

	}
	
	writel(readl(host->iobase+TCCSDHC_INT_STATUS), host->iobase+TCCSDHC_INT_STATUS);

	if (cmd->data) {
		writew((0x07<<12) | cmd->data->blksz, host->iobase+TCCSDHC_BLOCK_SIZE);
		writew(cmd->data->blocks, host->iobase+TCCSDHC_BLOCK_COUNT);
	} else {
		writel(0, host->iobase+TCCSDHC_DMA_ADDRESS);
		writew(0, host->iobase+TCCSDHC_BLOCK_SIZE);
		writew(0, host->iobase+TCCSDHC_BLOCK_COUNT);
	}

	writel(cmd->arg, host->iobase+TCCSDHC_ARGUMENT);

	/* Enable transfer interrupt sources */
	writel(uiIntStatusEn, host->iobase+TCCSDHC_INT_ENABLE);
	writel(cmd_reg, host->iobase+TCCSDHC_TMODE_COM);
	
	SD_CMD("%s: CMD%d, arg=%x\n", __func__, cmd->opcode, cmd->arg);
}

static void tcc_mmc_finish_command(struct tcc_mmc_host *host)
{
	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	BUG_ON(host->cmd == NULL);

	if (host->cmd->flags & MMC_RSP_PRESENT) {
		if (host->cmd->flags & MMC_RSP_136) {
			host->cmd->resp[0] = readl(host->iobase+TCCSDHC_RESPONSE76);
			host->cmd->resp[1] = readl(host->iobase+TCCSDHC_RESPONSE54);
			host->cmd->resp[2] = readl(host->iobase+TCCSDHC_RESPONSE32);
			host->cmd->resp[3] = readl(host->iobase+TCCSDHC_RESPONSE10);

			SD_CMD("%s: R2: resp[0]=0x%08x, resp[1]=0x%08x, resp[2]=0x%08x, resp[3]=0x%08x\n",
					__func__, host->cmd->resp[0], host->cmd->resp[1], host->cmd->resp[2], host->cmd->resp[3]);

			host->cmd->resp[0] = (host->cmd->resp[0] << 8) | ((host->cmd->resp[1] & 0xFF000000) >> 24);
			host->cmd->resp[1] = (host->cmd->resp[1] << 8) | ((host->cmd->resp[2] & 0xFF000000) >> 24);
			host->cmd->resp[2] = (host->cmd->resp[2] << 8) | ((host->cmd->resp[3] & 0xFF000000) >> 24);
			host->cmd->resp[3] <<= 8;                

		} else {
			host->cmd->resp[0] = readl(host->iobase+TCCSDHC_RESPONSE10);
			
			SD_CMD("%s: R1: resp[0]=0x%08x\n",  __func__, host->cmd->resp[0]);
		}
	}

	host->cmd->error = 0;

	if (host->data && host->data_early)
		tcc_mmc_finish_data(host);

	if (!host->cmd->data)
		tasklet_schedule(&host->finish_tasklet);

	host->cmd = NULL;
}

static void tcc_mmc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct tcc_mmc_host *host = mmc_priv(mmc);
	unsigned long flags;

	if((mmc == NULL)||(host == NULL)||(mrq == NULL)) {
		printk("[mmc:NULL] %s(mmc:%x, host:%x, mrq:%x)\n", __func__, (u32)mmc, (u32)host, (u32)mrq);
		return;// -EHOSTDOWN;
	}

	spin_lock_irqsave(&host->lock, flags);
#ifdef SD_CLOCK_CONTROL
	tcc_mmc_clock_onoff(host, 1);
#endif

	BUG_ON(host->mrq != NULL);

	host->mrq = mrq;

	if(!tcc_mmc_card_inserted(host)) {
		if (mrq->data && !(mrq->data->flags & MMC_DATA_READ)) {
			mrq->cmd->error = 0;
			mrq->data->bytes_xfered = mrq->data->blksz *
						  mrq->data->blocks;
		} else
			mrq->cmd->error = -ENOMEDIUM;	

		tasklet_schedule(&host->finish_tasklet);
		spin_unlock_irqrestore(&host->lock, flags);
	//	mmc_request_done(mmc, mrq);
		return;		
		
	} else {
		tcc_mmc_start_command(host, mrq->cmd);
	}

	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);
}

/* High speed mode threshold (Hz).
 * 
 * Although high speed mode should be suitable for all speeds not all
 * controller/card combinations are capable of meeting the higher
 * tolerances for (e.g.) clock rise/fall times.  Therefore, default
 * mode is used where possible for improved compatibility. */
 
#define SDIO_CLOCK_FREQ_HIGH_SPD 25000000

static void tcc_hw_set_high_speed(struct mmc_host *mmc, int hs)
{
	struct tcc_mmc_host *host = mmc_priv(mmc);
	unsigned long flags;
	u8 host_ctrl = 0;

	if((mmc == NULL)||(host == NULL)) {
		printk("[mmc:NULL] %s(mmc:%x, host:%x)\n", __func__, (u32)mmc, (u32)host);
		return;// -EHOSTDOWN;
	}

	spin_lock_irqsave(&host->lock, flags);

	host_ctrl= readw(host->iobase+TCCSDHC_HOST_CONTROL);
	host_ctrl &= ~HwSD_POWER_HS;
	
	if (hs) {
		host_ctrl |= HwSD_POWER_HS;
	}
	
	writew(host_ctrl, host->iobase+TCCSDHC_HOST_CONTROL);

	spin_unlock_irqrestore(&host->lock, flags);
}

static void tcc_mmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	//unsigned long flags;
	struct tcc_mmc_host *host = mmc_priv(mmc);
	uint32_t temp_reg;

	long dwMaxClockRate = host->peri_clk;
	int i = 0; /* 2^i is the divisor value */
	u32 clk_div = 0;

	if((mmc == NULL)||(host == NULL)||(ios == NULL)) {
		printk("[mmc:NULL] %s(mmc:%x, host:%x, ios:%x)\n", __func__, (u32)mmc, (u32)host, (u32)ios);
		return;// -EHOSTDOWN;
	}

	if (ios->clock != 0) {
		tcc_hw_set_high_speed(mmc, ios->clock > SDIO_CLOCK_FREQ_HIGH_SPD);
		/* shift MaxClockRate until we find the closest frequency <= target */
		while ((ios->clock < dwMaxClockRate) && (i < 8)) {
			dwMaxClockRate = dwMaxClockRate >> 1;
			i++;
		}

		if(i==0)
			clk_div = 0;
		else
			clk_div = 1<<(i-1);
		
		host->clk_div = clk_div;	/* store divider */
			
		temp_reg = readl(host->iobase+TCCSDHC_CLOCK_CONTROL);
		writew(temp_reg & ~HwSDCLKSEL_SCK_EN, host->iobase+TCCSDHC_CLOCK_CONTROL);

		udelay(10);

		writew((clk_div << 8)|HwSDCLKSEL_INCLK_EN, host->iobase+TCCSDHC_CLOCK_CONTROL);

		while (!(readl(host->iobase+TCCSDHC_CLOCK_CONTROL) & HwSDCLKSEL_INCLK_STABLE)) 	{
			udelay(100);
		}
		temp_reg = readl(host->iobase+TCCSDHC_CLOCK_CONTROL);
		//writew(temp_reg|(HwSDCLKSEL_SCK_EN|HwSDCLKSEL_INCLK_EN), host->iobase+TCCSDHC_CLOCK_CONTROL);
		writew(temp_reg|(HwSDCLKSEL_SCK_EN), host->iobase+TCCSDHC_CLOCK_CONTROL);

		udelay(100);
	}

	switch (ios->power_mode) {
		case MMC_POWER_OFF:
		{
			SD_DEBUG("%s%d: MMC_POWER_OFF\n", DRIVER_NAME, host->id);

			if (host->pdata->set_power)
				host->pdata->set_power(host->dev, host->id, 0);			
			
			break;
		}
		case MMC_POWER_UP:
		{
			SD_DEBUG("%s%d: MMC_POWER_UP\n", DRIVER_NAME, host->id);

			if (host->pdata->set_power)
				host->pdata->set_power(host->dev, host->id, 1); 		

			init_mmc_host(host);
//			msleep(2);
			break;
		}
		case MMC_POWER_ON:
		{
			SD_DEBUG("%s%d: MMC_POWER_ON\n", DRIVER_NAME, host->id);
			/*init_mmc_host(host);*/
			break;
		}
	}

	switch (ios->bus_width) {
		uint16_t cont_l, cont_h;

		case MMC_BUS_WIDTH_1:
			SD_DEBUG("%s%d: 1 bit mode\n", DRIVER_NAME, host->id);
			cont_l = readw(host->iobase+TCCSDHC_HOST_CONTROL);
			cont_l &= ~(HwSD_POWER_SD4|HwSD_POWER_SD8);
			cont_l |= HwSD_POWER_POW|HwSD_POWER_VOL33;
			writew(cont_l, host->iobase+TCCSDHC_HOST_CONTROL);

			cont_h = readw(host->iobase+TCCSDHC_BLOCK_GAP_CONTROL);
			cont_h &= ~Hw3;
			writew(cont_h, host->iobase+TCCSDHC_BLOCK_GAP_CONTROL);
			break;
		case MMC_BUS_WIDTH_4:
			SD_DEBUG("%s%d: 4 bit mode\n", DRIVER_NAME, host->id);
			cont_l = readw(host->iobase+TCCSDHC_HOST_CONTROL);
			cont_l &= ~HwSD_POWER_SD8;
			cont_l |= HwSD_POWER_POW|HwSD_POWER_VOL33|HwSD_POWER_SD4;
			writew(cont_l, host->iobase+TCCSDHC_HOST_CONTROL);

			cont_h = readw(host->iobase+TCCSDHC_BLOCK_GAP_CONTROL);
			cont_h |= Hw3;
			writew(cont_h, host->iobase+TCCSDHC_BLOCK_GAP_CONTROL);
			break;
		case MMC_BUS_WIDTH_8:
			SD_DEBUG("%s%d: 8 bit mode\n", DRIVER_NAME, host->id);
			cont_l = readw(host->iobase+TCCSDHC_HOST_CONTROL);
			cont_l |= HwSD_POWER_POW|HwSD_POWER_VOL33|HwSD_POWER_SD8;
			writew(cont_l, host->iobase+TCCSDHC_HOST_CONTROL);

			cont_h = readw(host->iobase+TCCSDHC_BLOCK_GAP_CONTROL);
			cont_h &= ~Hw3;
			writew(cont_h, host->iobase+TCCSDHC_BLOCK_GAP_CONTROL);
			break;
	}

	host->bus_mode = ios->bus_mode;

#if 0
	//need to add resetting a controller?  //MCC
	tcc_sw_reset(host, HwSD_SRESET_RSTCMD);
	tcc_sw_reset(host, HwSD_SRESET_RSTDAT); 		   
#endif

	mmiowb();	 
}

static int tcc_mmc_get_ro(struct mmc_host *mmc)
{
	struct tcc_mmc_host *host = mmc_priv(mmc);
	unsigned long flags;
	uint32_t reg = 0;

	if((mmc == NULL)||(host == NULL)) {
		printk("[mmc:NULL] %s(mmc:%x, host:%x)\n", __func__, (u32)mmc, (u32)host);
		return 0;// -EHOSTDOWN;
	}

#ifdef SD_CLOCK_CONTROL
	tcc_mmc_clock_onoff(host, 1);
#endif

	spin_lock_irqsave(&host->lock, flags);

	reg = readl(host->iobase + TCCSDHC_PRESENT_STATE);

	spin_unlock_irqrestore(&host->lock, flags);

#ifdef SD_CLOCK_CONTROL
	tcc_mmc_clock_onoff(host, 0);
#endif

	return !(reg & HwSD_STATE_SDWP);
}

//fix me, MCC
static int tcc_mmc_get_cd(struct mmc_host *mmc)
{
	struct tcc_mmc_host *host = mmc_priv(mmc);
	unsigned int status;

	if((mmc == NULL)||(host == NULL)) {
		printk("[mmc:NULL] %s(mmc:%x, host:%x)\n", __func__, (u32)mmc, (u32)host);
		return 0;// -EHOSTDOWN;
	}

	status=	tcc_mmc_card_inserted(host);
	udelay(500);	// jitter?
	
	return (tcc_mmc_card_inserted(host)); // return 1 card inserted, return 0 card not inserted
}

static void tcc_sdio_hw_enable_int(struct mmc_host *mmc, uint32_t sigs)
{
	struct tcc_mmc_host *host = mmc_priv(mmc);
	unsigned long flags;
	uint32_t stat_en;

	if((mmc == NULL)||(host == NULL)) {
		printk("[mmc:NULL] %s(mmc:%x, host:%x)\n", __func__, (u32)mmc, (u32)host);
		return;// -EHOSTDOWN;
	}

	pr_debug("%s id[%d] iobase[0x%x], sigs[%x]\n", __FUNCTION__, host->id, host->iobase, sigs);

	spin_lock_irqsave(&host->lock, flags);

	stat_en=readl(host->iobase+TCCSDHC_INT_ENABLE);	
	writel( stat_en | sigs, host->iobase+TCCSDHC_INT_ENABLE);

	spin_unlock_irqrestore(&host->lock, flags);
}

static void tcc_sdio_hw_disable_int(struct mmc_host *mmc, uint32_t sigs)
{
	struct tcc_mmc_host *host = mmc_priv(mmc);
	unsigned long flags;
	uint32_t stat_en;    

	if((mmc == NULL)||(host == NULL)) {
		printk("[mmc:NULL] %s(mmc:%x, host:%x)\n", __func__, (u32)mmc, (u32)host);
		return;// -EHOSTDOWN;
	}

	pr_debug("%s id[%d] iobase[0x%x], sigs[%x]\n", __FUNCTION__, host->id, host->iobase, sigs);
	spin_lock_irqsave(&host->lock, flags);

	stat_en=readl(host->iobase+TCCSDHC_INT_ENABLE);
	writel(stat_en & ~sigs, host->iobase+TCCSDHC_INT_ENABLE);

	spin_unlock_irqrestore(&host->lock, flags);
}

static void tcc_sdio_enable_card_int(struct mmc_host *mmc)
{
	if(mmc == NULL) {
		printk("[mmc:NULL] %s(mmc:%x)\n", __func__, (u32)mmc);
		return;// -EHOSTDOWN;
	}

	tcc_sdio_hw_enable_int(mmc, HwSDINT_EN_CDINT);
}

static void tcc_sdio_disable_card_int(struct mmc_host *mmc)
{
	if(mmc == NULL) {
		printk("[mmc:NULL] %s(mmc:%x)\n", __func__, (u32)mmc);
		return;// -EHOSTDOWN;
	}

	tcc_sdio_hw_disable_int(mmc, HwSDINT_EN_CDINT);
}

static void tcc_mmc_enable_cd_irq(int cd_irq)
{
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

#if defined(CONFIG_ARCH_TCC892X)
	if(!(cd_irq<0))
		pPIC->INTMSK0.nREG	|= 1<<cd_irq;
#else
	if(!(cd_irq<0))
		pPIC->INTMSK0	|= 1<<cd_irq;
#endif
}

static void tcc_mmc_disable_cd_irq(int cd_irq)
{
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

#if defined(CONFIG_ARCH_TCC892X)
	if(!(cd_irq<0))
		pPIC->INTMSK0.nREG &= ~(1<<cd_irq);
#else
	if(!(cd_irq<0))
		pPIC->INTMSK0	&= ~(1<<cd_irq);
#endif
}

static void tcc_mmc_check_status(struct tcc_mmc_host *host)
{
	unsigned int status;

	if((host == NULL)||(host->mmc == NULL)) {
		printk("[mmc:NULL] %s(host:%x, host->mmc:%x)\n", __func__, (u32)host, (u32)(host->mmc));
		return;// -EHOSTDOWN;
	}

	pr_debug("%s: cd detect\n",mmc_hostname(host->mmc));
	// 1: insert  0:remove 
	status=tcc_mmc_get_cd(host->mmc);

	host->card_inserted=status;

	if(status^host->oldstat)
	{
		pr_debug("%s: Slot status change detected (%d -> %d)\n", mmc_hostname(host->mmc), host->oldstat, status);
		if (status)
			mmc_detect_change(host->mmc, msecs_to_jiffies(3000));
		else
			mmc_detect_change(host->mmc, 0);
	}

#ifdef SD_CLOCK_CONTROL
	if(host->id==0)
		clk_enable(host->fclk); // wifi always enable
#endif

	host->oldstat=status;

}
/*
 * ISR for handling card insertion and removal
 */
static irqreturn_t tcc_mmc_cd_irq(int irq, void *dev_id)
{
	struct tcc_mmc_host *host = (struct tcc_mmc_host *)dev_id;
	unsigned long flags;

	if(host)
	{
		tcc_mmc_disable_cd_irq(host->cd_irq);

		spin_lock_irqsave(&host->lock, flags);

		tcc_mmc_check_status(host);

		spin_unlock_irqrestore(&host->lock, flags);

		tcc_mmc_enable_cd_irq(host->cd_irq);
	}

	return IRQ_HANDLED;
}
/*
 * Interrupt handling
 */
static void tcc_mmc_cmd_irq(struct tcc_mmc_host *host, u32 intmask)
{
	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	BUG_ON(intmask == 0);

	if (!host->cmd) {
		printk(KERN_ERR "%s: Got command interrupt 0x%08x even "
				"though no command operation was in progress.\n",
				mmc_hostname(host->mmc), (unsigned)intmask);
		return;
	}

	if (intmask & HwSDINT_STATUS_CMDTIME) {
		host->cmd->error = -ETIMEDOUT;
	} else if (intmask & (HwSDINT_STATUS_CMDCRC | HwSDINT_STATUS_CMDEND | HwSDINT_STATUS_CINDEX)) {
		host->cmd->error = -EILSEQ;
	}

	if (host->cmd->error) {
		tasklet_schedule(&host->finish_tasklet);
	} else if (intmask & HwSDINT_STATUS_CDONE) {
		tcc_mmc_finish_command(host);
	}
}

static void tcc_mmc_data_irq(struct tcc_mmc_host *host, u32 intmask)
{
	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	BUG_ON(intmask == 0);

	if (!host->data) {
		/*
		 * A data end interrupt is sent together with the response
		 * for the stop command.
		 */
		if (intmask & HwSDINT_STATUS_TDONE)
			return;

		printk(KERN_ERR "%s: Got data interrupt 0x%08x even "
				"though no data operation was in progress.\n",
				mmc_hostname(host->mmc), (unsigned)intmask);
		return;
	}

	if (intmask & HwSDINT_STATUS_DATTIME) {
		host->data->error = -ETIMEDOUT;
	} else if (intmask & (HwSDINT_STATUS_DATCRC | HwSDINT_STATUS_DATEND)) {
		host->data->error = -EILSEQ;
	} else if (intmask & HwSDINT_STATUS_ADMA) {
		host->data->error = -EIO;
	}

	if (host->data->error) {
		tcc_mmc_finish_data(host);
	} else {
		if (intmask & (HwSDINT_STATUS_RDRDY | HwSDINT_STATUS_WRRDY)) {
			printk(" \n %x \n ", intmask);
			tcc_transfer_pio(host);
		}

		/*
		 * We currently don't do anything fancy with DMA
		 * boundaries, but as we can't disable the feature
		 * we need to at least restart the transfer.
		 */
		//if (intmask & HwSDINT_STATUS_DMA){
		//	writel(readl(host->iobase+TCCSDHC_DMA_ADDRESS), host->iobase+TCCSDHC_DMA_ADDRESS);			
		//}			

		if (intmask & HwSDINT_STATUS_TDONE) {			
			if (host->cmd) {
				/*
				 * Data managed to finish before the
				 * command completed. Make sure we do
				 * things in the proper order.
				 */
				host->data_early = 1;
			} else {
				tcc_mmc_finish_data(host);
			}
		}
	}
}

static irqreturn_t tcc_mmc_interrupt_handler(int irq, void *dev_id)
{
	unsigned int IntStatus;
	irqreturn_t result;    
	int cardint = 0;    
	struct tcc_mmc_host *host = (struct tcc_mmc_host *) dev_id;
	unsigned long flags;

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return IRQ_HANDLED;// -EHOSTDOWN;
	}

	spin_lock_irqsave(&host->lock, flags);

	IntStatus = readl(host->iobase+TCCSDHC_INT_STATUS);

	if (IntStatus == 0  || IntStatus == 0xffffffff ) {
		result = IRQ_NONE;
		goto out;
	}

	if (IntStatus & HwSDINT_CMD_MASK) {
		writel(IntStatus & HwSDINT_CMD_MASK,host->iobase+TCCSDHC_INT_STATUS);
		tcc_mmc_cmd_irq(host, IntStatus & HwSDINT_CMD_MASK);
	}

	if (IntStatus & HwSDINT_DATA_MASK) {
		writel(IntStatus & HwSDINT_DATA_MASK,host->iobase+TCCSDHC_INT_STATUS);		
		tcc_mmc_data_irq(host, IntStatus & HwSDINT_DATA_MASK);
	}

	IntStatus &= ~(HwSDINT_CMD_MASK |HwSDINT_DATA_MASK);
	IntStatus &= ~HwSDINT_STATUS_ERR;

#if defined(CONFIG_ATHEROS_WIFI)
	if(IntStatus & HwSDINT_STATUS_CDINT) {
		if (readl(host->iobase + TCCSDHC_INT_ENABLE) & HwSDINT_STATUS_CDINT) {
			cardint =1;
			pr_debug("%s id[%d] cardint = 1\n", __FUNCTION__, host->id);
		}
	}
#else
	if(IntStatus & HwSDINT_STATUS_CDINT) {
		cardint =1;
	}
#endif

	IntStatus &= ~ HwSDINT_STATUS_CDINT;

	if (IntStatus) {
		SD_DEBUG(KERN_ERR "%s: Unexpected interrupt 0x%08x.\n", mmc_hostname(host->mmc), IntStatus);    
		writel(IntStatus, host->iobase+TCCSDHC_INT_STATUS);		
	}

	result = IRQ_HANDLED;

	mmiowb();

out:
	spin_unlock_irqrestore(&host->lock, flags);

	if (cardint) {
		mmc_signal_sdio_irq(host->mmc);
	}

	return result;
}

static void tcc_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	if (enable) {
		tcc_sdio_enable_card_int(mmc);
	} else {
		tcc_sdio_disable_card_int(mmc);
	}
}

static struct mmc_host_ops tcc_mmc_ops = {
	.request		 = tcc_mmc_request,
	.set_ios		 = tcc_mmc_set_ios,
	.get_ro			 = tcc_mmc_get_ro,
	.enable_sdio_irq = tcc_enable_sdio_irq,	
};

/*
 * Tasklets
 */
static void tcc_mmc_tasklet_finish(unsigned long param)
{
	struct tcc_mmc_host *host;
	unsigned long flags;
	struct mmc_request *mrq;

	host = (struct tcc_mmc_host *)param;

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	spin_lock_irqsave(&host->lock, flags);

	del_timer(&host->timer);

	mrq = host->mrq;

	/*
	 * The controller needs a reset of internal state machines
	 * upon error conditions.
	 */
	if (mrq->cmd->error ||
			(mrq->data && (mrq->data->error ||
						   (mrq->data->stop && mrq->data->stop->error)))) {

		/* Spec says we should do both at the same time, but Ricoh
		   controllers do not like that. */
		tcc_sw_reset(host, HwSD_SRESET_RSTCMD);
		tcc_sw_reset(host, HwSD_SRESET_RSTDAT);        

//		tcc_sw_reset(host, HwSD_SRESET_RSTALL);      		
	}
	

	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;

	mmiowb();

#ifdef SD_CLOCK_CONTROL
	tcc_mmc_clock_onoff(host, 0);
#endif
	/* reset is_direct */
	host->is_direct = 0;

	spin_unlock_irqrestore(&host->lock, flags);
	mmc_request_done(host->mmc, mrq);
}

static void tcc_mmc_timeout_timer(unsigned long data)
{
	struct tcc_mmc_host *host;
	unsigned long flags;

	host = (struct tcc_mmc_host*)data;

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	spin_lock_irqsave(&host->lock, flags);

	if (host->mrq) {
		printk(KERN_ERR "%s: Timeout waiting for hardware "
				"interrupt.\n", mmc_hostname(host->mmc));

		if (host->data) {
			host->data->error = -ETIMEDOUT;
			tcc_mmc_finish_data(host);
		} else {
			if (host->cmd)
				host->cmd->error = -ETIMEDOUT;
			else
				host->mrq->cmd->error = -ETIMEDOUT;

			tasklet_schedule(&host->finish_tasklet);
		}
	}    
	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);
}

static void init_mmc_host(struct tcc_mmc_host *host)
{
	unsigned int temp_val;
	PSDCHCTRL sdctrl = (PSDCHCTRL)tcc_p2v(HwSDMMC_CHCTRL_BASE); 	

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return;// -EHOSTDOWN;
	}

	clk_set_rate(host->fclk, 48*1000*1000);
	host->peri_clk = clk_get_rate(host->fclk);
	SD_DEBUG("tcc-sdhc%d: clock %lu\n", host->id, host->peri_clk);

	writel(0x03ff00ff, host->iobase+TCCSDHC_INT_ENABLE);
	writel(0x03ff01ff, host->iobase+TCCSDHC_SIGNAL_ENABLE);

	temp_val= readw(host->iobase+TCCSDHC_TIMEOUT_CONTROL)&(0xFF00);
	writew(temp_val | (0x000E),host->iobase+TCCSDHC_TIMEOUT_CONTROL);

	temp_val = readw(host->iobase+TCCSDHC_HOST_CONTROL);
	temp_val &= ~(HwSD_POWER_SD4 | HwSD_POWER_SD8);
	writew(temp_val, host->iobase+TCCSDHC_HOST_CONTROL);
	writew(temp_val|HwSD_POWER_POW, host->iobase+TCCSDHC_HOST_CONTROL);

#if defined(CONFIG_ARCH_TCC892X)
	switch (host->pdata->slot % 4) {
		case 0:
			sdctrl->SD0CMDDAT.nREG = 0x3F3F3F3F;
			sdctrl->SD0CAPREG0.nREG = 0x0CFF9870;
			break;
		case 1:
			sdctrl->SD1CMDDAT.nREG = 0x3F3F3F3F;
			sdctrl->SD1CAPREG0.nREG = 0x0CFF9870;
			break;
		case 2:
			sdctrl->SD2CMDDAT.nREG = 0x3F3F3F3F;
			sdctrl->SD2CAPREG0.nREG = 0x0CFF9870;
			break;
		case 3:
			sdctrl->SD3CMDDAT.nREG = 0x3F3F3F3F;
			sdctrl->SD3CAPREG0.nREG = 0x0CFF9870;
			break;
		default:
			break;
	}
#else
	switch (host->id) {
		case 0:
		{
#if defined(CONFIG_ARCH_TCC88XX)
			sdctrl->SD1PRESET1 = 0x0CFF9870;
#elif defined(CONFIG_ARCH_TCC93XX)
			BITCSET(sdctrl->SD0CMDDAT,0x3F3F3F3F, 0x3F3F3F3F);			
#endif
			break;
		}
		case 1:
		{			
#if defined(CONFIG_ARCH_TCC88XX)
			sdctrl->SD3PRESET1 = 0x0CFF9870;
#elif defined(CONFIG_ARCH_TCC93XX)
			BITCSET(sdctrl->SD1CMDDAT,0x3F3F3F3F, 0x3F3F3F3F);			
#endif
			break;
		}
		case 2:
		{
#if defined(CONFIG_ARCH_TCC88XX)
			sdctrl->SD2PRESET1 = 0x0CFF9870;
#elif defined(CONFIG_ARCH_TCC93XX)
			BITCSET(sdctrl->SD2CMDDAT,0x3F3F3F3F, 0x3f3F3f3F);			
#endif
			break;
		}
		case 3:
		{
#if defined(CONFIG_ARCH_TCC88XX)
			sdctrl->SD0PRESET1 = 0x0CFF9870;			
#elif defined(CONFIG_ARCH_TCC93XX)
			BITCSET(sdctrl->SD3CMDDAT,0x3F3F3F3F, 0x3F3F3F3F);
#endif
			break;
		}
	}
#endif

	if (host->pdata->init)
		host->pdata->init(host->dev, host->id);
}

static int __init tcc_mmc_probe(struct platform_device *pdev)
{
	struct tcc_mmc_platform_data *pdata = pdev->dev.platform_data;
	struct mmc_host *mmc;
	struct tcc_mmc_host *host = NULL;
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	int ret = 0;

	if (!pdata) {
		pr_err("%s: platform data is not specified\n", __func__);
		return -EINVAL;
	}

	mmc = mmc_alloc_host(sizeof(struct tcc_mmc_host), &pdev->dev);
	if (!mmc)
		return -ENOMEM;

	host = mmc_priv(mmc);

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return -ENODEV;// -EHOSTDOWN;
	}

	host->mmc = mmc;
	host->id = pdev->id;
	host->pdata = pdata;

#if defined(CONFIG_ARCH_TCC88XX)
	#if defined(CONFIG_MMC_TCC_PORT3)
	if(host->id == 0){
		host->fclk = clk_get(NULL,"sdhc3");
	} else if(host->id == 1){
		host->fclk = clk_get(NULL,"sdhc1");
	#else	//if defined(CONFIG_MMC_TCC_PORT7)
	if(host->id == 0){
		host->fclk = clk_get(NULL,"sdhc1");
	} else if(host->id == 1){
		host->fclk = clk_get(NULL,"sdhc3");
	#endif
	} else if(host->id == 2){
		host->fclk = clk_get(NULL,"sdhc2");
	} else if(host->id == 3){
		host->fclk = clk_get(NULL,"sdhc0");
	}
#elif defined(CONFIG_ARCH_TCC892X)
	switch (host->pdata->slot % 4) {
	case 0:
		host->fclk = clk_get(NULL,"sdhc0");		break;
	case 1:
		host->fclk = clk_get(NULL,"sdhc1");		break;
	case 2:
		host->fclk = clk_get(NULL,"sdhc2");		break;
	case 3:
		host->fclk = clk_get(NULL,"sdhc3");		break;
	default:
		break;
}
#else
	if(host->id == 0){
		host->fclk = clk_get(NULL,"sdhc0");		
	} else if(host->id == 1){
		host->fclk = clk_get(NULL,"sdhc1");		
	} else if(host->id == 2){
		host->fclk = clk_get(NULL,"sdhc2");		
	} else if(host->id == 3){
		host->fclk = clk_get(NULL,"sdhc3");		
	}
#endif

	if (!host->fclk) {
		printk("%s: SDHC core %d can't get mmc clock\n",DRIVER_NAME, host->id);
	} else {
		clk_enable(host->fclk);	
	}

	host->card_changed = 0;

	/* direct path from FSG */
	host->is_direct = 0;

	host->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	host->irq = platform_get_irq(pdev, 0);
//	host->cd_irq = platform_get_irq(pdev,  1);
	host->cd_irq = pdata->cd_int_num;	
	host->iobase = host->res->start;
	host->flags =0;
	//ADMA
	host->flags = TCC_MMC_USE_DMA
#if defined(CONFIG_MMC_TCC_ADMA)
				| TCC_MMC_USE_ADMA
#endif
				;

	mmc->caps |= pdata->caps;
	mmc->ops = &tcc_mmc_ops;
	mmc->f_min = pdata->f_min;
	mmc->f_max = pdata->f_max;
	mmc->ocr_avail = pdata->ocr_mask;	

	if (host->flags & TCC_MMC_USE_ADMA) {
		/*
		 * We need to allocate descriptors for all sg entries
		 * (128) and potentially one alignment transfer for
		 * each of those entries.
		 */
		host->adma_desc = kmalloc((128 * 2 + 1) * 4, GFP_KERNEL);
		host->align_buffer = kmalloc(128 * 4, GFP_KERNEL);
		if (!host->adma_desc || !host->align_buffer) {
			kfree(host->adma_desc);
			kfree(host->align_buffer);
			printk(KERN_WARNING "%s: Unable to allocate ADMA "
				"buffers. Falling back to standard DMA.\n",
				mmc_hostname(mmc));
			host->flags &= ~TCC_MMC_USE_ADMA;
		}
	}

	/*
	 * Maximum number of segments. Hardware cannot do scatter lists.
	 */
	if (host->flags & TCC_MMC_USE_DMA) {
		if(host->flags & TCC_MMC_USE_ADMA) {
			mmc->max_segs = 128;
		} else {
			mmc->max_segs = 1;
		}
	} else {
		mmc->max_segs = 128;
	}
	//mmc->max_phys_segs = 128;

	/*
	 * Maximum number of sectors in one transfer. Limited by DMA boundary
	 * size (512KiB).
	 */
	mmc->max_req_size = 524288;

	/*
	 * Maximum segment size. Could be one segment with the maximum number
	 * of bytes.
	 */
	if (host->flags & TCC_MMC_USE_ADMA) {
		mmc->max_seg_size = 65536;
	} else {
		mmc->max_seg_size = mmc->max_req_size;
	}

	/*
	 * Maximum block size. This varies from controller to controller and
	 * is specified in the capabilities register.
	 */
	mmc->max_blk_size = 512;

	/*
	 * Maximum block count.
	 */
	mmc->max_blk_count = 65535;

	host->dev = &pdev->dev;
	platform_set_drvdata(pdev, host);

	init_mmc_host(host);

// FIXME: Should move?
	mmc_add_host(mmc);

	spin_lock_init(&host->lock);

	/*
	 * Init tasklets.
	 */
	tasklet_init(&host->finish_tasklet,tcc_mmc_tasklet_finish, (unsigned long)host);

	setup_timer(&host->timer, tcc_mmc_timeout_timer, (unsigned long)host);

#if defined(CONFIG_ARCH_TCC892X)
	pPIC->SEL1.nREG		|= host->pdata->pic;
	pPIC->INTMSK1.nREG	|= host->pdata->pic;
	pPIC->MODE1.nREG 	|= host->pdata->pic;	// Level trigger
#else
	pPIC->SEL1		|= host->pdata->pic;
	pPIC->INTMSK1	|= host->pdata->pic;
	pPIC->MODE1 	|= host->pdata->pic;	// Level trigger
#endif

	snprintf(host->slot_desc, 16, "tcc-sdhc%d", host->id);
	ret = request_irq(host->irq, tcc_mmc_interrupt_handler, IRQF_DISABLED|IRQ_TYPE_LEVEL_HIGH, host->slot_desc, host);
	if (ret)
		goto error;

	//Card detect
	if(host->cd_irq<0)
	{
		/* configuration the SD card insertion detection */
		init_timer(&host->detect_timer);
		host->detect_timer.function = tcc_mmc_poll_event;
		host->detect_timer.data = (unsigned long) host;
		host->detect_timer.expires = jiffies + DETECT_TIMEOUT;
		
		add_timer(&host->detect_timer);
	}
	else
	{	
		host->oldstat= 0;

		if (host->pdata->cd_int_config)
		{
			if(host->pdata->cd_int_config(host->dev, host->id, host->cd_irq) == 0)
				ret = request_irq(pdata->cd_irq_num, tcc_mmc_cd_irq, IRQF_DISABLED|IRQ_TYPE_EDGE_BOTH, host->slot_desc, host);
		}
		
		if (ret)
			goto error;			

		tcc_mmc_check_status(host);
	}
	/* initialize the card-insert status */
	if (tcc_mmc_card_inserted(host)) {
		host->card_inserted = 1;
	} else {
		host->card_inserted = 0;
	}

	printk("%s: SDHC%d init\n", DRIVER_NAME, host->id);
	return ret;

error:
	printk("%s: SDHC%d failed\n", DRIVER_NAME, host->id);
	return ret;   	
}

static int tcc_mmc_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	if (mmc) {
		struct tcc_mmc_host *host = mmc_priv(mmc);

		free_irq(host->irq, host);

		mmc_remove_host(mmc);
		del_timer(&host->timer);
		del_timer_sync(&host->timer);
		mmc_free_host(mmc);

		tasklet_kill(&host->finish_tasklet);

		kfree(host->adma_desc);
		kfree(host->align_buffer);

		host->adma_desc = NULL;
		host->align_buffer = NULL;
	}

	return 0;
}

#ifdef CONFIG_PM
static int tcc_mmc_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	int ret = 0;
	struct tcc_mmc_host *host = platform_get_drvdata(pdev);

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return 0;// -EHOSTDOWN;
	}

	if (host && host->suspended)
		return 0;

	if (host) {
		ret = mmc_suspend_host(host->mmc);
		if (ret == 0)
			host->suspended = 1;
	}

	//disable cd interrupt
	tcc_mmc_disable_cd_irq(host->cd_irq);
 
	if (host->pdata->suspend)
		ret = host->pdata->suspend(&pdev->dev, host->id);	

	return ret;
}

static int tcc_mmc_resume(struct platform_device *pdev)
{
	int ret = 0;
	struct tcc_mmc_host *host = platform_get_drvdata(pdev);

	if(host == NULL) {
		printk("[mmc:NULL] %s(host:%x)\n", __func__, (u32)host);
		return 0;// -EHOSTDOWN;
	}

	if (host->pdata->resume)
		ret = host->pdata->resume(&pdev->dev, host->id);	

	if (host && !host->suspended)
		return 0;

	if (host) {
		ret = mmc_resume_host(host->mmc);
		if (ret == 0)
			host->suspended = 0;

		host->oldstat = tcc_mmc_get_cd(host->mmc);
	}

	//enable cd interrupt
	tcc_mmc_enable_cd_irq(host->cd_irq);

	return ret;
}
#else
#define tcc_mmc_suspend	NULL
#define tcc_mmc_resume	NULL
#endif


#if defined(CONFIG_MMC_TCC_SDHC0)
static struct platform_driver tcc_mmc_sdhc0_driver = {
	.probe		= tcc_mmc_probe,
	.remove		= tcc_mmc_remove,
	.suspend	= tcc_mmc_suspend,
	.resume		= tcc_mmc_resume,
	.driver		= {
		.name	= DRIVER_NAME_SDHC0,
	},
};
#endif
#if defined(CONFIG_MMC_TCC_SDHC1)
static struct platform_driver tcc_mmc_sdhc1_driver = {
	.probe		= tcc_mmc_probe,
	.remove 	= tcc_mmc_remove,
	.suspend	= tcc_mmc_suspend,
	.resume 	= tcc_mmc_resume,
	.driver 	= {
		.name	= DRIVER_NAME_SDHC1,
	},
};
#endif
#if defined(CONFIG_MMC_TCC_SDHC2)
static struct platform_driver tcc_mmc_sdhc2_driver = {
	.probe		= tcc_mmc_probe,
	.remove 	= tcc_mmc_remove,
	.suspend	= tcc_mmc_suspend,
	.resume 	= tcc_mmc_resume,
	.driver 	= {
		.name	= DRIVER_NAME_SDHC2,
	},
};
#endif
#if defined(CONFIG_MMC_TCC_SDHC3)
static struct platform_driver tcc_mmc_sdhc3_driver = {
	.probe		= tcc_mmc_probe,
	.remove 	= tcc_mmc_remove,
	.suspend	= tcc_mmc_suspend,
	.resume 	= tcc_mmc_resume,
	.driver 	= {
		.name	= DRIVER_NAME_SDHC3,
	},
};
#endif

static int __init tcc_mmc_init(void)
{
	int ret = -1;

#if defined(CONFIG_MMC_TCC_SDHC0)
	ret = platform_driver_register(&tcc_mmc_sdhc0_driver);
#endif
#if defined(CONFIG_MMC_TCC_SDHC1)
	ret = platform_driver_register(&tcc_mmc_sdhc1_driver);
#endif
#if defined(CONFIG_MMC_TCC_SDHC2)
	ret = platform_driver_register(&tcc_mmc_sdhc2_driver);
#endif
#if defined(CONFIG_MMC_TCC_SDHC3)
	ret = platform_driver_register(&tcc_mmc_sdhc3_driver);
#endif
	return ret;
}

static void __exit tcc_mmc_exit(void)
{
#if defined(CONFIG_MMC_TCC_SDHC0)
	platform_driver_unregister(&tcc_mmc_sdhc0_driver);
#endif
#if defined(CONFIG_MMC_TCC_SDHC1)
	platform_driver_unregister(&tcc_mmc_sdhc1_driver);
#endif
#if defined(CONFIG_MMC_TCC_SDHC2)
	platform_driver_unregister(&tcc_mmc_sdhc2_driver);
#endif
#if defined(CONFIG_MMC_TCC_SDHC3)
	platform_driver_unregister(&tcc_mmc_sdhc3_driver);
#endif
}

module_init(tcc_mmc_init);
module_exit(tcc_mmc_exit);

MODULE_DESCRIPTION("Telechips SD/MMC/SDIO Card driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS(DRIVER_NAME);
MODULE_AUTHOR("Telechips Inc. linux@telechips.com");
