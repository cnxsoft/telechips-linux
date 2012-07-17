/* 
 * linux/drivers/i2c/busses/tcc93xx/i2c-tcc.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: Telechips I2C Controller
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * ----------------------------
 * for TCC DIBCOM DVB-T module 
 * [M] i2c-core.c, i2c-dev.c
 * [M] include/linux/i2c-dev.h, include/linux/i2c.h
 *
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/tca_i2c.h>
//#include "tca_i2c.h"


enum tcc_i2c_state {
	STATE_IDLE,
	STATE_START,
	STATE_READ,
	STATE_WRITE,
	STATE_STOP
};

struct tcc_i2c {
	spinlock_t                      lock;
	wait_queue_head_t               wait;
	volatile struct tcc_i2c_regs    *regs;
    volatile unsigned long          IRQSTR;

	int                 core;
    int                 ch;
    int                 smu_i2c_flag;
    unsigned int        i2c_clk_rate;
	unsigned int        core_clk_rate;
    char                core_clk_name[10];
	struct clk          *pclk;
	struct i2c_msg      *msg;
	unsigned int        msg_num;
	unsigned int        msg_idx;
	unsigned int        msg_ptr;

	enum tcc_i2c_state  state;
	struct device       *dev;
	struct i2c_adapter  adap;
};

/* tcc_i2c_message_start
 *
 * put the start of a message onto the bus 
*/
static void tcc_i2c_message_start(struct tcc_i2c *i2c, struct i2c_msg *msg)
{
	unsigned int addr = (msg->addr & 0x7f) << 1;

	if (msg->flags & I2C_M_RD)
		addr |= 1;
	if (msg->flags & I2C_M_REV_DIR_ADDR)
		addr ^= 1;

	i2c->regs->TXR = addr;
	i2c->regs->CMD = Hw7 | Hw4;
}

static int wait_intr(struct tcc_i2c *i2c)
{
	unsigned long cnt = 0;

	if (!i2c->smu_i2c_flag) {
#if defined (CONFIG_ARCH_TCC892X)
		while (!(tcc_readl(i2c->IRQSTR) & (1<<i2c->core))) {
#else
		while (!(tcc_readl(i2c->IRQSTR) & (i2c->ch?Hw1:Hw0))) {
#endif
			cnt++;
			if (cnt > 100000) {
				printk("i2c-tcc: time out!  core%d ch%d\n", i2c->core, i2c->ch);
				return -1;
			}
		}
	} else {
		/* SMU_I2C wait */
		while (1) {
			cnt = i2c->regs->SR;
			if (!(cnt & Hw1)) break;
		}
		for (cnt = 0; cnt <15; cnt++);
	}

	return 0;
}

static int recv_i2c(struct tcc_i2c *i2c)
{
	int ret, i;
	dev_dbg(&i2c->adap.dev, "READ [%x][%d]\n", i2c->msg->addr, i2c->msg->len);

	tcc_i2c_message_start(i2c, i2c->msg);

	ret = wait_intr(i2c);
	if (ret != 0)
		return ret;
	BITSET(i2c->regs->CMD, Hw0);	/* Clear a pending interrupt */

	for (i = 0; i < i2c->msg->len; i++) {
        if (i2c->msg->flags & I2C_M_WM899x_CODEC) { /* B090183 */
            if (i == 0)
                i2c->regs->CMD = Hw5;
            else
                i2c->regs->CMD = Hw5 | Hw3;
        } else {
    		if (i == (i2c->msg->len - 1)) 
    			i2c->regs->CMD = Hw5 | Hw3;
    		else
    			i2c->regs->CMD = Hw5;
        }

		ret = wait_intr(i2c);
		if (ret != 0)
			return ret;
		BITSET(i2c->regs->CMD, Hw0);

		i2c->msg->buf[i] = (unsigned char) i2c->regs->RXR;
	}

	i2c->regs->CMD = Hw6;

	ret = wait_intr(i2c);
	if (ret != 0)
		return ret;
	BITSET(i2c->regs->CMD, Hw0);

	return 1;
}

static int send_i2c(struct tcc_i2c *i2c)
{
	int ret, i, no_stop = 0;
	dev_dbg(&i2c->adap.dev, "SEND [%x][%d]", i2c->msg->addr, i2c->msg->len);

	tcc_i2c_message_start(i2c, i2c->msg);

	ret = wait_intr(i2c);
	if (ret != 0)
		return ret;
	BITSET(i2c->regs->CMD, Hw0);	/* Clear a pending interrupt */

	for (i = 0; i < i2c->msg->len; i++) {
		i2c->regs->TXR = i2c->msg->buf[i];

        if(i2c->msg->flags == I2C_M_WM899x_CODEC)   /* B090183 */
            i2c->regs->CMD = Hw4 | Hw3;
        else
    		i2c->regs->CMD = Hw4;

		ret = wait_intr(i2c);
		if (ret != 0)
			return ret;
		BITSET(i2c->regs->CMD, Hw0);

		//i2c->msg->buf[i] = (unsigned char) i2c->regs->RXR;	//XXX
	}

	/*
	 *	Check no-stop operation condition (write -> read operation)
	 *	1. DxB
	 * 	2. msg_num == 2
	 */
	if (i2c->msg->flags & I2C_M_WR_RD)
		no_stop = 1;
	if (i2c->msg_num > 1)
		no_stop = 1;
	
	if (no_stop == 0) {
		i2c->regs->CMD = Hw6;
		ret = wait_intr(i2c);
		if (ret != 0)
			return ret;
	}
	BITSET(i2c->regs->CMD, Hw0);	/* Clear a pending interrupt */

	return 1;
}

/* tcc_i2c_doxfer
 *
 * this starts an i2c transfer
*/
static int tcc_i2c_doxfer(struct tcc_i2c *i2c, struct i2c_msg *msgs, int num)
{
	int ret, i;
	
	for (i = 0; i < num; i++) {
		spin_lock_irq(&i2c->lock);
		i2c->msg 		= &msgs[i];
		i2c->msg->flags = msgs[i].flags;
		i2c->msg_num 	= num;
		i2c->msg_ptr 	= 0;
		i2c->msg_idx 	= 0;
		i2c->state = STATE_START;
		spin_unlock_irq(&i2c->lock);

		if (i2c->msg->flags & I2C_M_RD) {
			ret = recv_i2c(i2c);
			if (ret != 1)
				printk("recv_i2c failed! - addr(0x%02X)\n", i2c->msg->addr);
		} else {
			ret = send_i2c(i2c);
			if (ret != 1)
				printk("send_i2c failed! - addr(0x%02X)\n", i2c->msg->addr);
		}
	}
	
	return ret;
}

/* tcc_i2c_xfer
 *
 * first port of call from the i2c bus code when an message needs
 * transferring across the i2c bus.
*/
static int tcc_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	struct tcc_i2c *i2c = (struct tcc_i2c *)adap->algo_data;
	int retry;
	int ret;

	for (retry = 0; retry < adap->retries; retry++) {
		
		ret = tcc_i2c_doxfer(i2c, msgs, num);

		if (ret == 1)
			return ret;

		dev_dbg(&i2c->adap.dev, "Retrying transmission (%d)\n", retry);

		udelay(100);
	}

	return -EREMOTEIO;
}

static u32 tcc_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

/* i2c bus registration info */
static const struct i2c_algorithm tcc_i2c_algorithm = {
	.master_xfer	= tcc_i2c_xfer,
	.functionality	= tcc_i2c_func,
};

/* tcc_i2c_init
 *
 * initialize the I2C controller, set the IO lines and frequency
 */
static int tcc_i2c_init(struct tcc_i2c *i2c)
{
	unsigned int prescale;
    volatile struct tcc_i2c_regs *i2c_reg = i2c->regs;

	if (!i2c->smu_i2c_flag)
	{
		/* I2C clock Enable */
		if(i2c->pclk == NULL) {
			i2c->pclk = clk_get(NULL, i2c->core_clk_name);
			if (IS_ERR(i2c->pclk)) {
				i2c->pclk = NULL;
				dev_err(i2c->dev, "can't get i2c clock\n");
				return -1;
			}
		}
		if(clk_enable(i2c->pclk) != 0) {
			dev_err(i2c->dev, "can't do i2c clock enable\n");
			return -1;
		}

		/* Set i2c core clock to 4MHz */
		clk_set_rate(i2c->pclk, i2c->core_clk_rate);

		prescale = ((clk_get_rate(i2c->pclk) / 1000) / (i2c->i2c_clk_rate * 5)) - 1;
		writel(prescale, &i2c_reg->PRES);
		writel(Hw7 | Hw6 | HwZERO, &i2c_reg->CTRL);		// start enable, stop enable, 8bit mode
		writel(Hw0 | readl(&i2c_reg->CMD), &i2c_reg->CMD);	// clear pending interrupt

		/* I2C GPIO setting */
		tca_i2c_setgpio(i2c->core, i2c->ch);

		msleep(100);    // Why does it need ?
	} 
	else
	{
		/* SMU_I2C clock setting */
		tcc_writel(0x80000000, i2c->IRQSTR);

		i2c->regs->CTRL = 0;
		i2c->regs->PRES = 4;
		BITSET(i2c->regs->CTRL, Hw7|Hw6);
	}

	return 0;
}

/* tcc83xx_i2c_probe
 *
 * called by the bus driver when a suitable device is found
*/
static int tcc_i2c_probe(struct platform_device *pdev)
{
	int i, ret = 0;
	struct tcc_i2c *i2c;
    struct tcc_i2c_platform_data *tcc_platform_data = pdev->dev.platform_data;
	struct resource *resource[I2C_NUM_OF_CH];

	i2c = kzalloc(sizeof(struct tcc_i2c) * I2C_NUM_OF_CH, GFP_KERNEL);
	if (!i2c) {
		ret = -ENOMEM;
		goto err_nockl;
	}

	for (i = 0; i < I2C_NUM_OF_CH; i++) {
		resource[i] = platform_get_resource(pdev, IORESOURCE_IO, i);
		if (!resource[i]) {
            printk("i2c%d: no io resource? (ch%d)\n", pdev->id, i);
			dev_err(&pdev->dev, "no io resource? (ch%d)\n", i);
			ret = -ENODEV;
			goto err_io;
		}

		i2c[i].adap.owner = THIS_MODULE;
		i2c[i].adap.algo = &tcc_i2c_algorithm;
		i2c[i].adap.retries = 2;
		sprintf(i2c[i].adap.name, "%s-ch%d",pdev->name, i);

		spin_lock_init(&i2c[i].lock);
		init_waitqueue_head(&i2c[i].wait);

		i2c[i].core     = pdev->id;
		i2c[i].ch       = i;
		i2c[i].regs     = (volatile struct tcc_i2c_regs *) IO_ADDRESS(resource[i]->start);
        i2c[i].IRQSTR   = resource[i]->end;
        i2c[i].smu_i2c_flag  = tcc_platform_data->smu_i2c_flag;
        i2c[i].i2c_clk_rate  = tcc_platform_data->i2c_ch_clk_rate[i];
        i2c[i].core_clk_rate = tcc_platform_data->core_clk_rate;
        memcpy(i2c[i].core_clk_name, tcc_platform_data->core_clk_name, sizeof(tcc_platform_data->core_clk_name));

        /* Set GPIO and Enable Clock */
		if(tcc_i2c_init(i2c + i) < 0) {
            goto err_io;
        }

		i2c[i].adap.algo_data = i2c + i;
		i2c[i].adap.dev.parent = &pdev->dev;
		i2c[i].adap.class = I2C_CLASS_HWMON | I2C_CLASS_SPD;
		i2c[i].adap.nr = pdev->id * I2C_NUM_OF_CH + i;

		ret = i2c_add_numbered_adapter(&i2c[i].adap);
		if (ret < 0) {
			printk("%s: Failed to add bus\n", i2c[i].adap.name);
			for (i--; i >= 0; i--)
				i2c_del_adapter(&i2c[i].adap);
			goto err_clk;
		}
	}

	platform_set_drvdata(pdev, i2c);

	return 0;

err_clk:
    printk("~~~~~~ %s() err_clk\n", __func__);
	//TODO: I2C peri bus disable
	for(i = 0;i < I2C_NUM_OF_CH;i++) {
    	if(i2c[i].pclk != NULL) {
            clk_disable(i2c[i].pclk);
            clk_put(i2c[i].pclk);
            i2c[i].pclk = NULL;
        }
    }
err_io:
    printk("~~~~~~ %s() err_io\n", __func__);
	kfree(i2c);
err_nockl:
    printk("~~~~~~ %s() err_nock1\n", __func__);
	return ret;
}

static int tcc_i2c_remove(struct platform_device *pdev)
{
	int i;
	struct tcc_i2c *i2c = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

    /* I2C clock Disable */
	for (i = 0; i < I2C_NUM_OF_CH; i++) {
		i2c_del_adapter(&i2c[i].adap);
        /* I2C bus & clock disable */
        clk_disable(i2c[i].pclk);
        clk_put(i2c[i].pclk);
    }

	kfree(i2c);

	return 0;
}

#ifdef CONFIG_PM
static int tcc_i2c_suspend_late(struct platform_device *pdev, pm_message_t state)
{
    int i;
	struct tcc_i2c *i2c = platform_get_drvdata(pdev);

	/* I2C Clock Disable */
	for(i = 0;i < I2C_NUM_OF_CH;i++) {
        if(i2c[i].pclk != NULL) {
            clk_disable(i2c[i].pclk);
            clk_put(i2c[i].pclk);
            i2c[i].pclk = NULL;
        }
    }
	return 0;
}

static int tcc_i2c_resume_early(struct platform_device *pdev)
{
	int i;
	struct tcc_i2c *i2c = platform_get_drvdata(pdev);

	/* Set gpio and I2C Clock Enable */
	for (i = 0; i < I2C_NUM_OF_CH; i++) {
		tcc_i2c_init(i2c + i);
	}
	return 0;
}
#else
#define tcc_i2c_suspend_late	NULL
#define tcc_i2c_resume_early	NULL
#endif

/* device driver for platform bus bits */

static struct platform_driver tcc_i2c_driver = {
	.probe			= tcc_i2c_probe,
	.remove			= tcc_i2c_remove,
	.suspend		= tcc_i2c_suspend_late,
	.resume			= tcc_i2c_resume_early,
	.driver			= {
		.owner		= THIS_MODULE,
		.name		= "tcc-i2c",
	},
};

static int __init tcc_i2c_adap_init(void)
{
    return platform_driver_register(&tcc_i2c_driver);
}

static void __exit tcc_i2c_adap_exit(void)
{
    platform_driver_unregister(&tcc_i2c_driver);
}

subsys_initcall(tcc_i2c_adap_init);
module_exit(tcc_i2c_adap_exit);

MODULE_AUTHOR("Telechips Inc. SYS4-3 linux@telechips.com");
MODULE_DESCRIPTION("Telechips H/W I2C driver");
MODULE_LICENSE("GPL");
