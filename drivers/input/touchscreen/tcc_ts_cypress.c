/*
 * linux/drivers/serial/tcc_ts_cypress.c
 *
 * Author: <linux@telechips.com>
 * Created: June 10, 2008
 * Description: Touchscreen driver for TSC2003 on Telechips TCC Series
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
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/i2c.h>
#include <linux/tcc_ts_cypress.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include <asm/io.h>
#include <asm/irq.h>
#include <mach/hardware.h>

#include <mach/bsp.h>
#include <mach/irqs.h>


//#define TSC_SUPPORT_MULTI_TOUCH
//#define TSC_USE_HRTIMER

#define dbg(msg...)	if (0) { printk( "TCC_TSC: " msg); }

#define TSC_EINT_PIN			Hw14
#define TSC_EINT_PORT_EN		HwGPIOD->GPEN
#define TSC_EINT_PORT_PD		HwGPIOD->GPPD0
#define TSC_EINT_PORT_PD_MASK	(Hw28|Hw29)
#define TSC_EINT_PORT_PD_UP		(Hw28)
#define TSC_EINT_PORT_PD_DOWN	(Hw29)
#define TSC_EINT_PORT_DAT		HwGPIOD->GPDAT
#define TSC_EINT_SEL			HwEINTSEL2_EINT11(SEL_GPIOD14)

#define TSC_EINT_SEL_MASK		HwEINTSEL2_EINT11_MASK
#define TSC_EINT_SEL_ID			HwINT0_EI11

#define TS_WAKEUP_PORT_EN		HwGPIOD->GPEN
#define TS_WAKEUP_PORT_DAT		HwGPIOD->GPDAT
#define TS_WAKEUP_PIN			Hw12

#ifdef TSC_USE_HRTIMER
#define CYPRESS_TS_TIMER_PERIOD	(10*1000*1000/2)	/* touch polling period : 10 ms. */
#else
#define CYPRESS_TS_TIMER_PERIOD	10	/* touch polling period : 10 ms. */
#endif


//==============================================================

#define CYPRESS_TS_STATUS_REG	0x00
#define CYPRESS_TS_MODE_REG		0x06
#define CYPRESS_TS_VER_REG		0x07

//==============================================================
#define DEVICE_NAME		"cypress_ts"
#define PEN_RELEASE		0x0
#define PEN_DOWN		0x1

#define X_AXIS_MIN		0
#define X_AXIS_MAX		480
#define Y_AXIS_MIN		0
#define Y_AXIS_MAX		320
#ifdef TSC_SUPPORT_MULTI_TOUCH
#define DISTANCE_MAX	20
#endif

struct cypress_ts {
	struct i2c_client *client;
	struct input_dev *input_dev;
	int irq;
#ifdef TSC_USE_HRTIMER	
	struct hrtimer timer;
#else
	struct timer_list timer;
#endif
	struct work_struct work;
	struct clk *clk;
#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
	int	in_suspend, running;
#endif
};

static struct workqueue_struct *cypress_ts_wq;
static s32 cypress_ts_version;

#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc92xx_tsc_early_suspend(struct early_suspend *h);
static void tcc92xx_tsc_late_resume(struct early_suspend *h);
#endif

static void cypress_ts_irq_init(void)
{
	// Interrupt Pin
	BITCSET(HwGPIOD->GPFN1, 0x0F000000, 0x00000000);
	HwGPIOD->GPEN	&= ~Hw14;
	HwGPIOD->GPDAT	&= ~Hw14;

	BITCLR(TSC_EINT_PORT_EN, TSC_EINT_PIN);						/* PENIRQ Port direction set (input) */
	BITCSET(TSC_EINT_PORT_PD, TSC_EINT_PORT_PD_MASK, TSC_EINT_PORT_PD_UP);
	BITCSET(HwEINTSEL->EINTSEL2, TSC_EINT_SEL_MASK, TSC_EINT_SEL);

	HwPIC->INTMSK0 |= TSC_EINT_SEL_ID;	   						/* interrupt mask enabke */

	HwPIC->IEN0		&= ~TSC_EINT_SEL_ID;						/* disable Isr */
	HwPIC->MODEA0	|= TSC_EINT_SEL_ID; 						/* both edge */
	HwPIC->MODE0	&= ~TSC_EINT_SEL_ID;						/* set edge trigger mode */
	HwPIC->POL0		|= TSC_EINT_SEL_ID;							/* active-low */
	HwPIC->CLR0		|= TSC_EINT_SEL_ID;							/* clear pending status */
	HwPIC->SEL0		|= TSC_EINT_SEL_ID;							/* IRQ Select */
	HwPIC->IEN0		|= TSC_EINT_SEL_ID;							/* Enable Interrupt */
}

static void cypress_ts_irq_enable(void)
{
	HwPIC->INTMSK0 |= TSC_EINT_SEL_ID;	   
	HwPIC->CLR0    |= TSC_EINT_SEL_ID;
	HwPIC->IEN0    |= TSC_EINT_SEL_ID;
}

static void cypress_ts_irq_disable(void)
{
	HwPIC->INTMSK0 &= ~TSC_EINT_SEL_ID;
	HwPIC->IEN0    &= ~TSC_EINT_SEL_ID; 
}

static void cypress_io_init(struct i2c_client *client)
{
	BITCSET(TS_WAKEUP_PORT_EN, TS_WAKEUP_PIN, TS_WAKEUP_PIN);  //output mode
	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, 0);//HIGH to enter sleep mode
	mdelay(100);
	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, TS_WAKEUP_PIN);//HIGH to enter sleep mode
	udelay(100);
	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, 0);//set low to wakeup goodix device
	mdelay(100);

#ifdef TSC_SUPPORT_MULTI_TOUCH
	i2c_smbus_write_byte_data(client, 0x06, 0x02);
#else
	i2c_smbus_write_byte_data(client, 0x06, 0x00);
#endif
}

unsigned int cypress_getirqnum(void)
{
	return INT_EI11;
}

static void cypress_ts_work_func(struct work_struct *work)
{
	struct cypress_ts* ts = container_of(work, struct cypress_ts, work);
	struct input_dev *dev = ts->input_dev;
	unsigned char readData[6];
	unsigned int fingers, x_pos, y_pos, dist;
	int res, retry_cnt;

	struct i2c_msg msg[2];
	uint8_t start_reg;
	int buf_len = 6;
	msg[0].addr = ts->client->addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &start_reg;
	start_reg = 0x00;
	msg[1].addr = ts->client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = buf_len;
	msg[1].buf = readData;


    ts->running = 1;
	cypress_ts_irq_disable();

	/* read x,y posision */
	retry_cnt = 3;
	do {
		res = i2c_transfer(ts->client->adapter, msg, 2);
		if (res < 0) {
			printk(KERN_ERR "%s: i2c_transfer failed\n", __func__);
		} else {
			break;
		}
		retry_cnt--;
	} while (retry_cnt && res);


	x_pos = (((u16)readData[1]) <<8)|(u16)readData[2];
	y_pos = (((u16)readData[3]) <<8)|(u16)readData[4];
	fingers = readData[0] & 0x3;
#if defined(TSC_SUPPORT_MULTI_TOUCH)
	dist = (readData[5] & 0x3F);
	if (dist > DISTANCE_MAX)
		dist = DISTANCE_MAX;
#endif
	/* send input events */
	if (fingers) {
		input_report_abs(dev, ABS_X, x_pos);
		input_report_abs(dev, ABS_Y, y_pos);
	}
    input_report_key(dev, BTN_TOUCH, fingers);
    input_report_key(dev, ABS_PRESSURE, fingers);
#ifdef TSC_SUPPORT_MULTI_TOUCH
	if (fingers > 1)	/* two fingers touch */
		input_report_key(dev, ABS_DISTANCE, dist);
#endif
    input_sync(dev);

	dbg("%s: finger:%d, x:%d, y:%d, dist:%d\n", __func__, fingers, x_pos, y_pos, dist);

	if (fingers) {
#ifdef TSC_USE_HRTIMER
		hrtimer_start(&ts->timer, ktime_set(0, CYPRESS_TS_TIMER_PERIOD), HRTIMER_MODE_REL);
#else
        ts->timer.expires = jiffies + msecs_to_jiffies(CYPRESS_TS_TIMER_PERIOD);
        add_timer(&ts->timer);
#endif
	}
	else {
		ts->running = 0;
		cypress_ts_irq_enable();
	}
}

static irqreturn_t cypress_ts_irq_handler(int irq, void *dev_id)
{
	struct cypress_ts *ts = (struct cypress_ts *)dev_id;
	dbg("\n %s\n",__func__);
	if ((ts->in_suspend == 0) && (ts->running == 0)) {
        ts->running = 1;
		queue_work(cypress_ts_wq, &ts->work);
	}
	return IRQ_HANDLED;
}

#ifdef TSC_USE_HRTIMER
static enum hrtimer_restart cypress_ts_timer_func(struct hrtimer *timer)
{
	struct cypress_ts *ts = container_of(timer, struct cypress_ts, timer);
	if (ts->in_suspend)
		ts->running = 0;
	else
		queue_work(cypress_ts_wq, &ts->work);
	return HRTIMER_NORESTART;
}
#else
static void cypress_ts_timer_func(unsigned long data)
{
	struct cypress_ts *ts = (struct cypress_ts *)data;
	if (ts->in_suspend)
		ts->running = 0;
	else
		queue_work(cypress_ts_wq, &ts->work);
}
#endif

static int __devinit cypress_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct cypress_ts *ts;
	int ret = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk(KERN_ERR "%s: need I2C_FUNC_I2C\n", __func__);
		ret = -ENODEV;
		goto err_check_functionality_failed;
	}

	ts = kzalloc(sizeof(struct cypress_ts), GFP_KERNEL);
	if (ts == NULL) {
		ret = -ENOMEM;
		goto err_alloc_data_failed;
	}

	INIT_WORK(&ts->work, cypress_ts_work_func);
	ts->client = client;
	i2c_set_clientdata(client, ts);

#if 0
	pdata = client->dev.platform_data;
	if (pdata)
		ts->power = pdata->power;
	if (ts->power) {
		ret = ts->power(1);
		if (ret < 0) {
			printk(KERN_ERR "synaptics_ts_probe power on failed\n");
			goto err_power_failed;
		}
	}
#endif

	ret = i2c_smbus_read_byte_data(client, CYPRESS_TS_VER_REG);
	if (ret < 0 || (ret&0xFF == 0XFF)) {
		printk(KERN_ERR "%s: check version failed\n", __func__);
		ret = -ENODEV;
		goto err_detect_failed;
	}
	printk(KERN_INFO "%s: Version %x\n", __func__, ret);

	ts->input_dev = input_allocate_device();
	if (ts->input_dev == NULL) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate input device\n", __func__);
		goto err_input_dev_alloc_failed;
	}

	ts->input_dev->name = "tcc-cypress-ts";
	set_bit(EV_SYN, ts->input_dev->evbit);
	set_bit(EV_KEY, ts->input_dev->evbit);
	set_bit(BTN_TOUCH, ts->input_dev->keybit);
	set_bit(EV_ABS, ts->input_dev->evbit);
	input_set_abs_params(ts->input_dev, ABS_X, X_AXIS_MIN, X_AXIS_MAX, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_Y, Y_AXIS_MIN, Y_AXIS_MAX, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_PRESSURE, 0, 1, 0, 0);
#ifdef TSC_SUPPORT_MULTI_TOUCH
	input_set_abs_params(ts->input_dev, ABS_DISTANCE, 0, DISTANCE_MAX, 0, 0);
#endif

	ret = input_register_device(ts->input_dev);
	if (ret) {
		printk(KERN_ERR "%s: Unable to register %s input device\n", __func__, ts->input_dev->name);
		goto err_input_register_device_failed;
	}

#ifdef TSC_USE_HRTIMER
	hrtimer_init(&ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ts->timer.function = cypress_ts_timer_func;
#else
    init_timer(&ts->timer);
    ts->timer.data = (unsigned long)ts;
    ts->timer.function = cypress_ts_timer_func;
#endif

	cypress_ts_irq_init();

	ts->irq = cypress_getirqnum();
	/* Enable touch interrupt */
	if (request_irq(ts->irq, cypress_ts_irq_handler, IRQF_DISABLED, DEVICE_NAME,ts)) {
		ret = -EBUSY;
		goto fail1;
	}

	cypress_io_init(ts->client);	

#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.suspend 	= tcc92xx_tsc_early_suspend;
	ts->early_suspend.resume 	= tcc92xx_tsc_late_resume;
	ts->early_suspend.level 	= EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&ts->early_suspend);
	ts->in_suspend = 0;
#endif

	printk("cypress driver is successfully loaded\n");
	return 0;

	free_irq(ts->irq, ts);
fail1:
err_input_register_device_failed:
	input_free_device(ts->input_dev);
err_detect_failed:
err_input_dev_alloc_failed:
	kfree(ts);
err_alloc_data_failed:
err_check_functionality_failed:
	return ret;
}

static int __devexit cypress_ts_remove(struct i2c_client *client)
{
	struct cypress_ts *ts = i2c_get_clientdata(client);
	unregister_early_suspend(&ts->early_suspend);
	free_irq(ts->irq, ts);
#ifdef TSC_USE_HRTIMER
	hrtimer_cancel(&ts->timer);
#else
	del_timer(&ts->timer);
#endif
	input_unregister_device(ts->input_dev);
	kfree(ts);
	return 0;
}

static int cypress_ts_suspend(struct i2c_client *client, pm_message_t state)
{
//	i2c_smbus_write_byte_data(client, 0x06, 0x01);
//	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, TS_WAKEUP_PIN);
	return 0;
}

static int cypress_ts_resume(struct i2c_client *client)
{
#ifndef CONFIG_HAS_EARLYSUSPEND
	cypress_io_init(client);
#endif
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc92xx_tsc_early_suspend(struct early_suspend *h)
{
	struct cypress_ts *ts = container_of(h, struct cypress_ts, early_suspend);
	ts->in_suspend = 1;
	dbg("%s\n", __func__);
	cypress_ts_irq_disable();
//	i2c_smbus_write_byte_data(ts->client, 0x06, 0x01);
//	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, TS_WAKEUP_PIN);//HIGH to enter sleep mode
}
static void tcc92xx_tsc_late_resume(struct early_suspend *h)
{
	struct cypress_ts *ts = container_of(h, struct cypress_ts, early_suspend);
	char readData[6];
	struct i2c_msg msg[2];
	uint8_t start_reg;
	int buf_len = 6;
	msg[0].addr = ts->client->addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &start_reg;
	start_reg = 0x00;
	msg[1].addr = ts->client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = buf_len;
	msg[1].buf = readData;

	dbg("%s\n", __func__);
	ts->in_suspend = 0;
	i2c_transfer(ts->client->adapter, msg, 2);
	cypress_ts_irq_enable();
}
#endif

static const struct i2c_device_id cypress_ts_id[] = {
	{ TCC_TS_CYPRESS_NAME, 0 },
	{ }
};

static struct i2c_driver cypress_driver = {
	.probe      = cypress_ts_probe,
	.remove     = cypress_ts_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend    = cypress_ts_suspend,
	.resume     = cypress_ts_resume,
#endif
	.id_table	= cypress_ts_id,
	.driver = {
		.name	= TCC_TS_CYPRESS_NAME,
	},
};

static int __devinit cypress_ts_init(void)
{
	cypress_ts_wq = create_singlethread_workqueue("cypress_ts_wq");
	if (!cypress_ts_wq)
		return -ENOMEM;
	return i2c_add_driver(&cypress_driver);
}

void __exit cypress_ts_exit(void)
{
	i2c_del_driver(&cypress_driver);
	if (cypress_ts_wq)
		destroy_workqueue(cypress_ts_wq);
}

module_init(cypress_ts_init);
module_exit(cypress_ts_exit);

MODULE_AUTHOR("linux@telechips.com");
MODULE_DESCRIPTION("Telechips cypress touch driver");
MODULE_LICENSE("GPL");
