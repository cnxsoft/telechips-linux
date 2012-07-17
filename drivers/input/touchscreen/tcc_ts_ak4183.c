/*
 *  drivers/input/touchscreen/ak4183.c
 *
 *  Copyright (c) by CD Huang <huangchengdong@wisky.com.cn>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 **************************************************
 * log:
 * 1. 20100720 CD Huang
 * 	> Created for M801 project touch screen.
 *
 */

#include <linux/input.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <asm/delay.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/irqreturn.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <asm/mach-types.h>
#include <mach/bsp.h>
#include <mach/io.h>

#define AK4183_I2C_ADDR     (0x90>>1)   ///1 0 0 1 0 0 [CAD0] [R/W], CAD0 = LOW in M801 project

#define DEVICE_NAME "tcc-ts-ak4183"

/*
 *NOTE:high resolution timer not support in RK2808 platform
 *		kernel BUG at kernel/hrtimer.c:1072!
 */
//#define USE_HRTIMER	
#ifdef USE_HRTIMER
#define AK4183_POLL_INTERVAL    (10*1000*1000L)   //ns between samples
#else
#define AK4183_SAMPLE_DELAY		(jiffies+4) 		/// 5ms interval
#define AK4183_TIWTTER_DELAY		(jiffies+1)		///1 * 5ms = 5 ms
#endif

#define AK4183_I2C_SPEED	300			//standard speed 100KHz, high speed 400KHz

#define ADC_MODE_12BIT	(0<<1)
#define ADC_MODE_8BIT	(1<<1)

#define ADC_BIT_MODE		ADC_MODE_12BIT

#define ADC_READ_DATA_CNT		3
#define CTRL_CMD_X_AXIS	(0xc4|ADC_BIT_MODE)
#define CTRL_CMD_Y_AXIS	(0xd4|ADC_BIT_MODE)
#define CTRL_CMD_PWRDWN	0xc0	//auto power down mode
#define CTRL_CMD_SLEEP		0x70


#if 1
#define ADCVAL_X_MIN		150
#define ADCVAL_X_MAX		3950
#define ADCVAL_Y_MIN		280
#define ADCVAL_Y_MAX		3850
#else
#define ADCVAL_X_MIN		0
#define ADCVAL_X_MAX		4000
#define ADCVAL_Y_MIN		200
#define ADCVAL_Y_MAX		4000
#endif


#define LCD_SCREEN_X_PIXEL	1024
#define LCD_SCREEN_Y_PIXEL	600

/*Hardware setting*/
//IRQ

#if CONFIG_MACH_TCC8900
#define TS_EINT_PORT_EN		(((PGPION)tcc_p2v(HwGPIOA_BASE))->GPEN)
#define TS_EINT_PORT_DAT	(((PGPION)tcc_p2v(HwGPIOA_BASE))->GPDAT)
#define TS_EINT_PIN			Hw5
#define TS_EINT_IRQ_NUM		INT_EI11
#define TS_EINT_SEL			HwEINTSEL2_EINT11(SEL_GPIOA5)
#define TS_EINT_SEL_MASK	HwEINTSEL2_EINT11_MASK
#define TS_EINT_SEL_ID 		HwINT0_EI11
#endif

#if CONFIG_MACH_TCC8800
#define TS_EINT_PORT_EN		(((PGPION)tcc_p2v(HwGPIOA_BASE))->GPEN)
#define TS_EINT_PORT_DAT	(((PGPION)tcc_p2v(HwGPIOA_BASE))->GPDAT)
#define TS_EINT_PIN			Hw6
#define TS_EINT_IRQ_NUM		INT_EI2
#define TS_EINT_SEL			HwEINTSEL0_EINT2(SEL_GPIOA6)
#define TS_EINT_SEL_MASK	HwEINTSEL0_EINT2_MASK
#define TS_EINT_SEL_ID 		HwINT0_EI2
#endif

#define PEN_UP		0
#define PEN_DOWN	1

//#define TS_IRQ_INACT	0
//#define TS_IRQ_ACT	1

#define NO_KEY_TO_SYNC		0
#define HAVE_KEY_TO_SYNC	1

/*
 * NOTE: The Ak4183 ADC origin (X/Y axis = 0, 0) is local at LEFT/BOTTOM,
 *		but Android event handle origin at LEFT/TOP, so we reverse the
 *		Y-axis here.
 */
#define REVERSE_X_AXIS
//#define REVERSE_Y_AXIS

//#define DEBUG_AK4183
#ifdef DEBUG_AK4183
#define DPRINTK(arg...) printk(arg)
#else
#define DPRINTK(arg...)
#endif

struct phy_xy{
    u8 px[2];	//ADC 12 bit mode
    u8 py[2];	//ADC 12 bit mode
};

struct logic_xy{
    u16 lx;
    u16 ly;
};



struct ak4183_ts{
	struct input_dev *input;
	struct i2c_client *client;
	int use_irq;
#ifdef USE_HRTIMER
	struct hrtimer timer;
#else
	struct timer_list sample_timer;	//AK4183 ADC sample timer
	struct timer_list twitter_timer;	//pen down twitter delay time
#endif
	int irq;
	struct phy_xy pos_phy;	
	struct logic_xy pos_logic;
	int pen_state;	//PEN_DOWN or PEN_UP
	struct clk *clk;
#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
#endif
};

struct ak4183_ts *the_ak4183 = NULL;
//int ts_irq_flag ;

static unsigned short probe[] = {4, AK4183_I2C_ADDR, I2C_CLIENT_END};
static unsigned short ignore = I2C_CLIENT_END;
static struct i2c_client_address_data ak4183_addr_data = {
    .normal_i2c = &ignore,
    .probe = probe,
    .ignore = &ignore
};

static int ak4183_read_data(struct i2c_client *client, u8 *data)
{
	struct i2c_msg msgs[] = {
		{
		.addr = client->addr,
		.flags = 1,	//read data flag
		.len = 2,		//ADC mode in 12bit , read two bytes data
		.buf = data,
		},
	};
	if (i2c_transfer(client->adapter, msgs, 1) < 0){
		DPRINTK(KERN_ERR "%s: read data failed\n", DEVICE_NAME);
		return -EIO;
	}

	return 0;
}

static int ak4183_write_command(struct i2c_client *client, u8 cmd)
{
	u8 data[2] = {0};
	struct i2c_msg msgs[] = {
		{
		.addr = client->addr,
		.flags = 0,	//write data
		.len = 2,		//the first byte data[0] is for register address= NULL, the write data is local in data[1]
		.buf = data,
		},
	};

	data[1] = cmd;	//set cmd data to send buffer

	if (i2c_transfer(client->adapter, msgs, 1) < 0){
	    DPRINTK(KERN_ERR "%s: write command data failed\n", DEVICE_NAME);
	    return -EIO;
	}

	return 0;
}

/*
 * Get touch screen logic position
 * Read AK4183 data sequence in ADC 12-bit mode:
 * 	----------------------    ------------    -----------------------
 *	SDA		|  | Address 	| |   |			|   |				|   |   |
 *			|S|			| |   |A/D Data    |   | A/D Data	|   | P |
 *			|  |			| |   |			|   |				|   |   |
 *			---------------------------------------------    ---
 *							D11			D4	D3	D0 [0000]
 * the first byte data is B11-B4, than the second byte is B3-B0 filled with four bits '0'
 */
static int ak4183_get_logic_pos(struct ak4183_ts *ts_dev)
{
	u8 adcx[2], adcy[2];
	int px, py, last_px=0, last_py=0;
	int sumx=0, sumy=0;
	int cnt = 0;	//take average of 3 times ADC_READ_DATA_CNT

	do{
		//read Y axis
		ak4183_write_command(ts_dev->client, CTRL_CMD_Y_AXIS);
		ak4183_read_data(ts_dev->client, adcy);
		//read X axis
		ak4183_write_command(ts_dev->client, CTRL_CMD_X_AXIS);
		ak4183_read_data(ts_dev->client, adcx);

		px = (adcx[0]<<4) | (adcx[1]>>4);
		py = (adcy[0]<<4) | (adcy[1]>>4);
		DPRINTK(KERN_INFO "%s: read ADC data X[H, L]: %d, %d\n", __FUNCTION__, adcx[0], adcx[1]);
		DPRINTK(KERN_INFO "%s: read ADC data Y[H, L]: %d, %d\n", __FUNCTION__, adcy[0], adcy[1]);
		DPRINTK(KERN_INFO "%s: read ADC data X, Y axis: %d, %d\n", __FUNCTION__, px, py);
		
		
		if(px < ADCVAL_X_MIN || px > ADCVAL_X_MAX|| py < ADCVAL_Y_MIN|| py > ADCVAL_Y_MAX)
			break;	//adc data not in valid range, break;
		if((cnt > 0) && ((abs(last_px-px) > 100) ||(abs(last_py-py) > 100)))
			break;

		last_px = px;
		last_py = py;
		
		sumx += px;
		sumy += py;
	}while(++cnt != ADC_READ_DATA_CNT);

	ak4183_write_command(ts_dev->client, CTRL_CMD_PWRDWN);
	if(cnt < 2){	//ignore invalid data
		ts_dev->pos_logic.lx = 0;
		ts_dev->pos_logic.ly = 0;
	}
	else{
		sumx /=cnt;
		sumy /=cnt;
#if 0
		//Calculate formula: sumx/(ADCVAL_X_MAX-ADCVAL_X_MIN) = (logic_x/LCD_SCREEN_X_PIXEL), so...		
		ts_dev->pos_logic.lx = LCD_SCREEN_X_PIXEL*(sumx-ADCVAL_X_MIN)/(ADCVAL_X_MAX-ADCVAL_X_MIN);
		ts_dev->pos_logic.ly = LCD_SCREEN_Y_PIXEL*(sumy-ADCVAL_Y_MIN)/(ADCVAL_Y_MAX-ADCVAL_Y_MIN);
#ifdef REVERSE_X_AXIS
		ts_dev->pos_logic.lx = LCD_SCREEN_X_PIXEL - ts_dev->pos_logic.lx;
#endif
#ifdef REVERSE_Y_AXIS
		ts_dev->pos_logic.ly = LCD_SCREEN_Y_PIXEL - ts_dev->pos_logic.ly;
#endif	
#endif
		ts_dev->pos_logic.lx = sumx; 
		ts_dev->pos_logic.ly = sumy;

		input_report_abs(ts_dev->input, ABS_X, ts_dev->pos_logic.lx);
#if 0 
		input_report_abs(ts_dev->input, ABS_Y, ts_dev->pos_logic.ly); // for LVDS
#else
		input_report_abs(ts_dev->input, ABS_Y, ADCVAL_Y_MAX - (ts_dev->pos_logic.ly - ADCVAL_Y_MIN));
#endif
		input_report_key(ts_dev->input, BTN_TOUCH, 1);
		input_sync(ts_dev->input);
#if 1
		DPRINTK(KERN_INFO "%s: ADC Count %d\n", __FUNCTION__, cnt);
		DPRINTK(KERN_INFO "%s: touch pos X %d\n", __FUNCTION__, ts_dev->pos_logic.lx);
		DPRINTK(KERN_INFO "%s: touch pos Y %d\n", __FUNCTION__, ts_dev->pos_logic.ly);
#endif
	}
	
	return 0;
}

#ifdef USE_HRTIMER
static enum hrtimer_restart ak4183_hrtimer_handle(struct hrtimer *timer)
{
	s32 pin_level;
	struct ak4183_ts *ts_dev = container_of(timer, struct ak4183_ts, timer);

	if (NULL == ts_dev)
		return HRTIMER_RESTART;
	pin_level = GPIOGetPinLevel(AK4183_IRQ_GPIO);
	if(pin_level == GPIO_LOW)
	{	//pen down, 
		ak4183_get_logic_pos(ts_dev);
	}
	else{
		if(PEN_DOWN== ts_dev->pen_state){
			ts_dev->pen_state = PEN_UP;
			input_report_key(ts_dev->input, BTN_TOUCH, 0);	//report touch up event
			input_sync(ts_dev->input);
			gpio_irq_enable(AK4183_IRQ_GPIO);
			DPRINTK(KERN_INFO "%s: touch pen up!!\n", __FUNCTION__);
		}
	}

    return HRTIMER_RESTART;
}
#else
static void ak4183_key_tiwtter_hdl(unsigned long data)
{
	s32 pin_level;
	struct ak4183_ts *ts_dev = (struct ak4183_ts *)data;

	if (NULL == ts_dev)
		return;

	if(!(TS_EINT_PORT_DAT & TS_EINT_PIN)){
		if (PEN_UP == the_ak4183->pen_state){
			the_ak4183->pen_state = PEN_DOWN;
//			pr_info("%s: touch pen down!!\n", __FUNCTION__);	
		}
		//start ADC sample timer
		mod_timer(&the_ak4183->sample_timer, AK4183_SAMPLE_DELAY);
	}
	else
		mod_timer(&the_ak4183->twitter_timer, AK4183_SAMPLE_DELAY);
}

static void ak4183_sample_timer_handle(unsigned long data)
{
	s32 pin_level;
	struct ak4183_ts *ts_dev = (struct ak4183_ts *)data;

	if (NULL == ts_dev)
		return;
	
	if(!(TS_EINT_PORT_DAT & TS_EINT_PIN)){
		ak4183_get_logic_pos(ts_dev);
		mod_timer(&the_ak4183->sample_timer, AK4183_SAMPLE_DELAY);
	}
	else{
		if(PEN_DOWN == ts_dev->pen_state){
			input_report_key(ts_dev->input, BTN_TOUCH, 0);	//report touch up event
			input_sync(ts_dev->input);
			ts_dev->pen_state = PEN_UP;
//			pr_info("%s: touch pen up!!\n", __FUNCTION__);
		}

		mod_timer(&the_ak4183->twitter_timer, AK4183_SAMPLE_DELAY);
	}

    return;
}
#endif

static irqreturn_t ak4183_ts_isr(int irq, void *dev_id)
{
	s32 pin_level;

	if(NULL == the_ak4183)
		return IRQ_NONE;
	DPRINTK(KERN_INFO "%s: --------------get IRQ GPIO state:%d\n", __FUNCTION__, (TS_EINT_PORT_DAT & TS_EINT_PIN));
	if(!(TS_EINT_PORT_DAT & TS_EINT_PIN) && (PEN_UP == the_ak4183->pen_state)){
#ifdef USE_HRTIMER
		gpio_irq_disable(AK4183_IRQ_GPIO);
		if (PEN_UP == the_ak4183->pen_state){
			the_ak4183->pen_state = PEN_DOWN;
			DPRINTK(KERN_INFO "%s: touch pen down!!\n", __FUNCTION__);	
		}
		hrtimer_start(&the_ak4183->timer, ktime_set(0, AK4183_POLL_INTERVAL), HRTIMER_MODE_REL);
#else
		//start touch pen twitter timer
		mod_timer(&the_ak4183->twitter_timer, AK4183_TIWTTER_DELAY);
#endif
	}

	return IRQ_HANDLED;
}

static void tcc8800_irq_hw_init(void)
{
		volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	
	      gpio_request(TCC_GPA(6), "tsc_int");
		tcc_gpio_config(TCC_GPA(6), GPIO_FN(0));
		gpio_direction_input(TCC_GPA(6));


//		BITCSET(pGPIO->EINTSEL0 , HwEINTSEL0_EINT2_MASK, HwEINTSEL0_EINT2(SEL_GPIOB31));
		BITCSET(pGPIO->EINTSEL0 , HwEINTSEL0_EINT2_MASK, HwEINTSEL0_EINT2(SEL_GPIOA6));
//		BITCSET(pGPIO->GPBPD1, Hw31|Hw30, Hw31|Hw30);


		HwPIC->INTMSK0 |= Hw5;	   
	
		HwPIC->IEN0		&= ~Hw5;						/* disable Isr */
		HwPIC->MODEA0	&= ~Hw5; 						/* both edge */
		HwPIC->MODE0	&= ~Hw5;						/* set edge trigger mode */
		HwPIC->POL0		|= Hw5;							/* active-low */
		HwPIC->CLR0		|= Hw5;							/* clear pending status */
		HwPIC->SEL0		|= Hw5;							/* IRQ Select */
		HwPIC->IEN0		|= Hw5;			


}



static void tcc8902_irq_hw_init(void)
{
#if 0
	PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);

	// Interrupt Control
	BITCLR(TS_EINT_PORT_EN, TS_EINT_PIN);  //Input mode
	BITCSET(pGPIO->EINTSEL2, HwEINTSEL2_EINT11_MASK, TS_EINT_SEL);

	pPIC->INTMSK0	|= TS_EINT_SEL_ID;	//enable irq
	pPIC->IEN0		&= ~(TS_EINT_SEL_ID);						/* disable I2C */
	pPIC->MODEA0	&= ~(TS_EINT_SEL_ID) ; 					/* signle edge */

	pPIC->MODE0		&= ~(TS_EINT_SEL_ID);						/* set edge trigger mode */	

	pPIC->POL0		|= (TS_EINT_SEL_ID);						/* active-high */
	pPIC->CLR0		 =  TS_EINT_SEL_ID; 						/* clear pending status */
	pPIC->IEN0		|= (TS_EINT_SEL_ID);						/* Enable Interrupt */
#endif
}

static int ak4183_attach_adapter(struct i2c_adapter *adap);
static int ak4183_detach_clilent(struct i2c_client *client);






#ifdef CONFIG_HAS_EARLYSUSPEND
static void ak4183_ts_early_suspend(struct early_suspend *h)
{
	printk("%s() \n", __FUNCTION__);
	PPIC mHwPIC = (PPIC)tcc_p2v(HwVPIC_BASE);
	mHwPIC->INTMSK0 &= ~TS_EINT_SEL_ID; // disable irq
//	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, TS_WAKEUP_PIN);//HIGH to enter sleep mode
	printk("%s() out\n", __FUNCTION__);
}

static void ak4183_ts_late_resume(struct early_suspend *h)
{
	pr_info("%s() \n", __FUNCTION__);
	PPIC mHwPIC = (PPIC)tcc_p2v(HwVPIC_BASE);
	mHwPIC->INTMSK0 |= TS_EINT_SEL_ID; // enable irq	
//	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, 0);//set low to wakeup goodix device
}
#endif

static void ak4183_ts_suspend(struct early_suspend *h)
{
	return 0;
}

static void ak4183_ts_resume(struct early_suspend *h)
{
	return 0;
}


static int ak4183_ts_probe(struct i2c_client *client, const struct i2c_devive_id *id)
{
	struct ak4183_ts *ak4183_dev = NULL;
	struct input_dev *ak4183_input = NULL;
	int err = 0;

	printk(KERN_INFO "%s: probe i2c touch screen device at addr 0x%x.\n", DEVICE_NAME);

	if(NULL != the_ak4183){
		printk(KERN_ERR "%s: only one device can be probe\n", __FILE__);
		return -EEXIST;
	}
	ak4183_dev = kzalloc(sizeof(struct ak4183_ts), GFP_KERNEL);
	if (NULL == ak4183_dev){
		printk(KERN_WARNING "%s: allocate memory failed!\n", __FILE__);
		err = -ENOMEM;
		goto fail_1;
	}


	ak4183_dev->client = client;
	i2c_set_clientdata(client, ak4183_dev);

	ak4183_input = input_allocate_device();
	if (NULL == ak4183_input){
		printk(KERN_WARNING "%s: allocate input device failed.\n", __FILE__);
		err = -ENOMEM;
		goto fail_2;
	}
	ak4183_dev->input = ak4183_input;

	ak4183_input->phys = "ak4183-touch/input1";
	ak4183_input->evbit[0] = BIT(EV_SYN) | BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
	ak4183_input->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input_set_abs_params(ak4183_input, ABS_X, 0, 4095, 0, 0);//x-axis min = 0, max = 800
	input_set_abs_params(ak4183_input, ABS_Y, 0, 4095, 0, 0);//y-axis min = 0, max = 480
	input_set_abs_params(ak4183_input, ABS_PRESSURE, 0, 1, 0, 0);
	ak4183_input->name = DEVICE_NAME;



	err = input_register_device(ak4183_input);
	if(err < 0)
		goto fail_2;

	the_ak4183 = ak4183_dev;

	ak4183_dev->pen_state = PEN_UP;


	if(machine_is_tcc8900())
		tcc8902_irq_hw_init();
	else if(machine_is_tcc8800())
		tcc8800_irq_hw_init();
	
	//setup irq gpio

	err =  request_irq(TS_EINT_IRQ_NUM, ak4183_ts_isr, IRQF_DISABLED, DEVICE_NAME, ak4183_dev);

	if(err < 0){
		printk(KERN_ERR "%s, %d: could not request gpio irq\n", __FILE__, __LINE__);
		goto fail_3;
	}
	
#ifdef USE_HRTIMER
	hrtimer_init(&ak4183_dev->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ak4183_dev->timer.function = ak4183_hrtimer_handle;
//    hrtimer_start(&ak4183_dev->timer, ktime_set(0, AK4183_POLL_INTERVAL), HRTIMER_MODE_REL);
#else
//init ak4183 ADC poll timer
	init_timer(&ak4183_dev->sample_timer);
	ak4183_dev->sample_timer.data = (unsigned long)ak4183_dev;
	ak4183_dev->sample_timer.function = ak4183_sample_timer_handle;
//init touch pen twitter delay timer
	init_timer(&ak4183_dev->twitter_timer);
	ak4183_dev->twitter_timer.data = (unsigned long)ak4183_dev;
	ak4183_dev->twitter_timer.function = ak4183_key_tiwtter_hdl;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
	ak4183_dev->early_suspend.suspend 	= ak4183_ts_early_suspend;
	ak4183_dev->early_suspend.resume 	= ak4183_ts_late_resume;
	ak4183_dev->early_suspend.level 	= EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&ak4183_dev->early_suspend);
#endif


	printk(KERN_INFO "%s: touch screen device init done\n", DEVICE_NAME);

	mod_timer(&ak4183_dev->twitter_timer, AK4183_SAMPLE_DELAY);

    return 0;
	
fail_3:
	free_irq(ak4183_dev->irq, NULL);

fail_2:
	input_unregister_device(ak4183_input);
	input_free_device(ak4183_input); 
	
fail_1:
	kfree(ak4183_dev);
	the_ak4183 = NULL;
	return err;
}


static int ak4183_ts_remove(struct i2c_client *client)
{
	struct ak4183_ts *ts = i2c_get_clientdata(client);
	unregister_early_suspend(&ts->early_suspend);
	if (ts->use_irq)
		free_irq(client->irq, ts);
	input_unregister_device(ts->input);
	clk_disable(ts->clk);
	clk_put(ts->clk);
	kfree(ts);
	return 0;
}





static const struct i2c_device_id ak4183_ts_id[] = {
	{ DEVICE_NAME, 0 },
	{ }
};


static struct i2c_driver ak4183_ts_driver = {
	.probe 		= ak4183_ts_probe,
	.remove		= ak4183_ts_remove,
	.suspend		= ak4183_ts_suspend,
	.resume		= ak4183_ts_resume,
	.id_table		= ak4183_ts_id,
	.driver		= {
		.name	=	DEVICE_NAME,
	},
};


static int __init ak4183_init(void)
{
    return i2c_add_driver(&ak4183_ts_driver);
}

static void __exit ak4183_exit(void)
{
    i2c_del_driver(&ak4183_ts_driver);
}

module_init(ak4183_init);
module_exit(ak4183_exit);

MODULE_AUTHOR("CD Huang <huangchengdong@wisky.com.cn>");
MODULE_DESCRIPTION(DEVICE_NAME "driver");
MODULE_LICENSE("GPL");




