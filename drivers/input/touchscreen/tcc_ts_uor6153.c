/*
 * drivers/input/touchscreen/tcc_ts_uor6153.c
 *
 * Author: <linux@telechips.com>
 * Created: May 4, 2011
 * Description: Touchscreen driver for UOR6153 r-type Dual-touch on Telechips TCC Series
 *
 * Copyright (C) 2008-2011 Telechips 
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
#include <linux/gpio.h>
//#include <linux/tcc_ts_uor6153.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include <asm/io.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>

#include <mach/bsp.h>
#include <mach/irqs.h>

#define dbg(msg...)	if (0) { printk( "TCC_TSC: " msg); }

#define DEVICE_NAME "tcc-ts-uor6153"

#define PD_SETTING 0x2
#define ADC_RESOLUTION 0 // 0 = 12bit, 1 = 8bit


#define INITIAL_X 1
#define INITIAL_Y 2
#define MEASURE_P1X 3
#define MEASURE_P1Y 4
#define MEASURE_P1Z1 5
#define MEASURE_P1Z2 6
#define MEASURE_P2X 7
#define MEASURE_P2Y 8
#define MEASURE_P2Z1 9
#define MEASURE_P2Z2 10

#define PEN_DOWN 1
#define PEN_RELEASE 0
#define TOUCH_POS_READ_PERIOD 10
#define TOUCH_COLLECT_NR 8
#define TOUCH_PRESSURE_LIMIT 15000
#define TOUCH_VALID_VALUE 2

#define X_AXIS_MIN	0
#define X_AXIS_MAX	4095
#define Y_AXIS_MIN	0
#define Y_AXIS_MAX	4095

#define READ_PENIRQ() (HwGPIOB->GPDAT & Hw31)


struct uor6153_ts{
	uint16_t addr;
	struct i2c_client *client;
	struct input_dev *input;
	int use_irq;
	unsigned int x, y, pressure, valid, irq;
	int pen_status, opened, running;
	struct mutex mutex;
	struct work_struct work_q;
	struct timer_list ts_timer;
	struct clk *clk;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif	
};


static int uor6513_set_command(struct i2c_client *client, char command1, char command2)
{
	int ret;
	u8 commandBuf;


	switch(command1)
	{
		case INITIAL_X:
			commandBuf = (0<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case INITIAL_Y:
			commandBuf = (1<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P1X:
			commandBuf = (12<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P1Y:
			commandBuf = (13<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P1Z1:
			commandBuf = (14<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P1Z2:
			commandBuf = (15<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P2X:
			commandBuf = (4<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P2Y:
			commandBuf = (5<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P2Z1:
			commandBuf = (6<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
		case MEASURE_P2Z2:
			commandBuf = (7<<4) | (command2 << 2) | (ADC_RESOLUTION << 1);
			break;
	}

		
	
	ret = i2c_smbus_write_byte(client, commandBuf);
	if(ret < 0)
	{
		pr_err("read regs error\n");
	}
	

}


static int uor6153_read_regs(struct i2c_client *client, u8 *buf, unsigned len)
{	
	int ret;
	int i;
	struct i2c_msg msgs[1] = {
		{ client->addr, I2C_M_RD, len, buf },//recv
	};
	

	msgs[0].flags |= (client->flags & I2C_M_RD);
	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret < 0)
	{
		pr_err(" read regs error \n");
	}
	

}

void uor6153_touchbublesort(unsigned int Number[],unsigned int num) 
{
	int i,j;
	unsigned int temp;
	for(i=0 ; i<(int)(num-1) ; i++)   
	{    
		for(j=i+1;j<(int)num;j++)
		{ 
			if(Number[i]>Number[j]) 
			{ 
				temp   = Number[i]; 
				Number[i] = Number[j]; 
				Number[j] = temp;
			}
		} 
	}
}

int uor6153_pos_read(struct i2c_client *client, unsigned int *x_pos, unsigned int *y_pos , struct uor6153_ts *ts_data)
{

	u8 read_buf[33];
	unsigned int  i;
	unsigned int r_x[TOUCH_COLLECT_NR], r_y[TOUCH_COLLECT_NR];
	unsigned long  x_tol, y_tol;
	unsigned int m_pos_x, m_pos_y;
	unsigned int validcnt=0;
	unsigned int index;
	unsigned int z1,z2;

	memset(r_x, 0x00, sizeof(int) * TOUCH_COLLECT_NR);
	memset(r_y, 0x00, sizeof(int) * TOUCH_COLLECT_NR);
	

	for(i=0; i < TOUCH_COLLECT_NR; i++)
	{
		uor6513_set_command(client, MEASURE_P1X, 0x2);
		uor6153_read_regs(client, read_buf, 2);	
		r_x[validcnt] = (read_buf[0] << 4) | (read_buf[1] >> 4);

		uor6513_set_command(client, MEASURE_P1Y, 0x2);
		uor6153_read_regs(client, read_buf, 2);		
		r_y[validcnt]= (read_buf[0] << 4) | (read_buf[1] >> 4);


		if((r_x[validcnt] != 0) && (r_y[validcnt] != 0))
		{
			uor6513_set_command(client, MEASURE_P1Z1, 0x2);
			uor6153_read_regs(client, read_buf, 2);			
			z1 = (read_buf[0] << 4) | (read_buf[1] >> 4);
			if(z1 != 0)
			{
				uor6513_set_command(client, MEASURE_P1Z2, 0x2);
				uor6153_read_regs(client, read_buf, 2);			
				z2 = (read_buf[0] << 4) | (read_buf[1] >> 4);

				//printk("x = %d , y = %d , z = %d\n", r_x[validcnt], r_y[validcnt], (r_x[validcnt]*z2/z1 - r_x[validcnt]));
				
				if((r_x[validcnt]*z2/z1 - r_x[validcnt]) < TOUCH_PRESSURE_LIMIT)
					validcnt++;
			}
		}


		
				
	}

	if(validcnt < TOUCH_VALID_VALUE)
	{

	}

	uor6153_touchbublesort(r_x, validcnt);
	uor6153_touchbublesort(r_y, validcnt);


	x_tol = y_tol = 0; //sum the coordinate values
	index = (validcnt-TOUCH_VALID_VALUE)>>1;

	if(validcnt < (TOUCH_VALID_VALUE*2)) return -1;

	for(i=index;i<(index+TOUCH_VALID_VALUE);++i) {
		x_tol += r_x[i];
		y_tol += r_y[i];
	}

	m_pos_x = x_tol/(TOUCH_VALID_VALUE);
	m_pos_y = y_tol/(TOUCH_VALID_VALUE);
	

	*x_pos = X_AXIS_MAX - m_pos_x;
	*y_pos = m_pos_y;

	if ((*x_pos <= X_AXIS_MAX && *x_pos >= X_AXIS_MIN) &&
	    (*y_pos <=Y_AXIS_MAX && *y_pos >= Y_AXIS_MIN)) {
		return 0;
	} else {
		return -1;
	}




}



static void uor6153_port_init(void)
{
	if(machine_is_m803())
	{
		volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	
	      gpio_request(TCC_GPB(31), "tsc_int");
		tcc_gpio_config(TCC_GPB(31), GPIO_FN(0));
		gpio_direction_input(TCC_GPB(31));


//		BITCSET(pGPIO->EINTSEL0 , HwEINTSEL0_EINT2_MASK, HwEINTSEL0_EINT2(SEL_GPIOB31));
		BITCSET(pGPIO->EINTSEL0 , HwEINTSEL0_EINT2_MASK, HwEINTSEL0_EINT2(SEL_GPIOB31));
		BITCSET(pGPIO->GPBPD1, Hw31|Hw30, Hw31|Hw30);


		HwPIC->INTMSK0 |= Hw5;	   
	
		HwPIC->IEN0		&= ~Hw5;						/* disable Isr */
		HwPIC->MODEA0	&= ~Hw5; 						/* both edge */
		HwPIC->MODE0	&= ~Hw5;						/* set edge trigger mode */
		HwPIC->POL0		|= Hw5;							/* active-low */
		HwPIC->CLR0		|= Hw5;							/* clear pending status */
		HwPIC->SEL0		|= Hw5;							/* IRQ Select */
		HwPIC->IEN0		|= Hw5;							/* Enable Interrupt */		
		


	}


}

void uor6153_enable_irq(void)
{
	if(machine_is_m803())
		HwPIC->INTMSK0 |= Hw5;
	
}


void uor6153_disable_irq(void)
{
	if(machine_is_m803())
		HwPIC->INTMSK0 &= ~Hw5;
	
}

static inline void tsc_pen_release(struct uor6153_ts *ts_data, struct input_dev *dev)
{
	if (ts_data->pen_status != PEN_RELEASE) {
		//printk("PENUP\n");
		ts_data->pen_status = PEN_RELEASE;

		input_report_key(dev, BTN_TOUCH, PEN_RELEASE);
		input_report_key(dev, ABS_PRESSURE, 0);
		input_sync(dev);
	}
}

static inline void tsc_pen_pressure(struct uor6153_ts *ts_data, struct input_dev *dev)
{
	ts_data->pen_status = PEN_DOWN;

	input_report_abs(dev, ABS_X, ts_data->x);
	input_report_abs(dev, ABS_Y, ts_data->y);	

	input_report_key(dev, BTN_TOUCH, PEN_DOWN);
	input_report_key(dev, ABS_PRESSURE, 1);
	input_sync(dev);

	//printk("PEN DOWN %u, %u, %u\n", ts_data->x, ts_data->y, ts_data->pressure);
}

#if MULTI_TOUCH_SUPPORT


#else
static void ts_fetch_thread(struct work_struct *work)
{
	struct uor6153_ts* ts_data = container_of(work, struct uor6153_ts, work_q);
	struct input_dev *dev = ts_data->input;
	int flag, valid;
   
	ts_data->running = 1;
	uor6153_disable_irq();
	if(READ_PENIRQ()){
		tsc_pen_release(ts_data, dev);
		ts_data->running = 0;
		uor6153_enable_irq();
	} else {
		flag = uor6153_pos_read(ts_data->client, &(ts_data->x), &(ts_data->y), ts_data);
		//tca_tch_poweroff();

		//printk("\n X_POS:%d   Y_POS: %d",ts_data->x,ts_data->y);

		valid = ts_data->x | ts_data->y;
		if ((flag == 0) && valid) {
			tsc_pen_pressure(ts_data, dev);
			ts_data->ts_timer.expires = jiffies + msecs_to_jiffies(TOUCH_POS_READ_PERIOD);
			add_timer(&ts_data->ts_timer);
		} else {
			tsc_pen_release(ts_data, dev);
			ts_data->running = 0;
			uor6153_enable_irq();
		}
	}
}
#endif

static irqreturn_t uor6153_irq_handler(int irq, void *dev_id)
{
	struct uor6153_ts *ts_data = (struct uor6153_ts *)dev_id;

//	printk("%s\n", __func__);
	
	if (ts_data->running == 0) {
		ts_data->running = 1;
		ts_data->ts_timer.expires = jiffies;//+msecs_to_jiffies(TOUCH_POS_READ_PERIOD);
		add_timer(&ts_data->ts_timer);
	}
	return IRQ_HANDLED;
}

static void ts_timer_handler(unsigned long data)
{
	struct uor6153_ts *ts = (struct uor6153_ts *) data;
	if (ts->opened) {
		if (schedule_work(&(ts->work_q)) == 0) {
			printk("cannot schedule work !!!\n");
			ts->running = 0;
		}
	} else {
		uor6153_disable_irq();
	}
}


#ifdef CONFIG_PM
static int uor6153_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
	uor6153_disable_irq();

	return 0;
}

static int uor6153_ts_resume(struct i2c_client *client)
{
//	uor6153_io_init(client);
	uor6153_enable_irq();

	return 0;
}
#else
#define uor6153_ts_suspend     NULL
#define uor6153_ts_resume      NULL
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void uor6153_ts_early_suspend(struct early_suspend *h)
{
	struct uor6153_ts *ts;
	ts = container_of(h, struct uor6153_ts, early_suspend);
	uor6153_ts_suspend(ts->client, PMSG_SUSPEND);

}
static void uor6153_ts_late_resume(struct early_suspend *h)
{
	struct uor6153_ts *ts;
	ts = container_of(h, struct uor6153_ts, early_suspend);
	uor6153_ts_resume(ts->client);
}
#endif


static int uor6153_ts_probe(struct i2c_client *client, const struct i2c_devive_id *id)
{
	struct uor6153_ts *ts;
	struct input_dev *input_dev;
	struct clk *clk;
	int err = -ENOMEM;
	int ret;


	//temp
	char read_buf[2];

	printk("%s\n", __func__);

	ts = kzalloc(sizeof(struct uor6153_ts), GFP_KERNEL);
	if(ts == NULL){
		return - ENOMEM;
	}

	ts->client = client;
	i2c_set_clientdata(client, ts);

	
	input_dev = input_allocate_device();
	if(input_dev == NULL){
		goto fail_input_dev_alloc;
	}

	clk = clk_get(NULL, "i2c0");
	if(clk)
		clk_enable(clk);
	else
		dev_err(&client->dev, "can't get i2c clock\n");

	ts->clk = clk;
	ts->running = 0;
	ts->opened = 1;
	ts->pen_status = PEN_RELEASE;
	ts->input = input_dev;

	input_dev->name = DEVICE_NAME;
//	input_dev->id.version = 0x0001;
//	input_dev->id.vendor = Touchpad_Manufacture_ID;   //MCC test
//	input_dev->id.product = Touchpad_Major_Product_Ver;   //MCC test

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input_set_abs_params(input_dev, ABS_X, X_AXIS_MIN, X_AXIS_MAX, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, Y_AXIS_MIN, Y_AXIS_MAX, 0, 0);
	/* TODO */
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 0, 0, 0);

	INIT_WORK(&(ts->work_q), ts_fetch_thread);
	mutex_init(&(ts->mutex));

	init_timer(&ts->ts_timer);
	ts->ts_timer.data = (unsigned long) ts;
	ts->ts_timer.function = ts_timer_handler;


	uor6153_port_init();
	uor6513_set_command(ts->client, INITIAL_X, 0x0);
	uor6513_set_command(ts->client, INITIAL_Y, 0x0);

	printk(" input register\n");

	err = input_register_device(ts->input);
	if (err)
		goto fail_input_register_device;

	printk("after input register\n");
	/* Enable touch interrupt */

	ts->irq = INT_EI2;
	if ((err = request_irq(ts->irq, uor6153_irq_handler, 0, DEVICE_NAME, ts))) {
		err = -EBUSY;
		goto fail_request_irq;
	} else {
		ts->use_irq = 1;
	}

	printk("after irq\n");		
//	tca_tch_poweroff();
//	uor6153_io_init(client);

#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.suspend = uor6153_ts_early_suspend;
	ts->early_suspend.resume = uor6153_ts_late_resume;
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	register_early_suspend(&ts->early_suspend);
#endif

	printk(KERN_INFO "uor6153_ts_probe: uor6153 driver is successfully loaded\n");
	return 0;

fail_request_irq:
fail_input_register_device:
	input_free_device(input_dev);
fail_input_dev_alloc:
err_detect:
	kfree(ts);
	return err;

}

static int uor6153_ts_remove(struct i2c_client *client)
{
	struct uor6153_ts *ts = i2c_get_clientdata(client);
	unregister_early_suspend(&ts->early_suspend);
	if (ts->use_irq)
		free_irq(client->irq, ts);
	input_unregister_device(ts->input);
	clk_disable(ts->clk);
	clk_put(ts->clk);
	kfree(ts);
	return 0;
}





static const struct i2c_device_id uor6153_ts_id[] = {
	{ DEVICE_NAME, 0 },
	{ }
};


static struct i2c_driver uor6153_ts_driver = {
	.probe 		= uor6153_ts_probe,
	.remove		= uor6153_ts_remove,
	.suspend		= uor6153_ts_suspend,
	.resume		= uor6153_ts_resume,
	.id_table		= uor6153_ts_id,
	.driver		= {
		.name	=	DEVICE_NAME,
	},
};



static int __init uor6153_ts_init(void)
{
	printk("%s\n", __func__);
	return i2c_add_driver(&uor6153_ts_driver);
}


void __exit uor6153_ts_exit(void)
{
	i2c_del_driver(&uor6153_ts_driver);
}

module_init(uor6153_ts_init);
modue_exit(uor6153_ts_exit);


MODULE_AUTHOR("linux@telechips.com");
MODULE_DESCRIPTION("Telechips uor6153 touch driver");
MODULE_LICENSE("GPL");


