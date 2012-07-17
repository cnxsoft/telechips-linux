  
/**** ********************************************************************************************************
 * drivers/input/touchscreen/wisky_ts_goodix.c 
 
 * Author        :    liuxianghui@wisky.com.cn
 * Date          :    2009-12-08
 * Description   :    goodix  Driver for Telechips TCC8902
 
 * Copyright (c) ShenZhen Wisky, Ltd.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * LOG:
 * 1. 20100603 CD Huang^Migrate to Tcc8902 platform
 *************************************************************************************************************/
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <asm/types.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/ioport.h>
#include <linux/i2c.h>	 
#include <linux/clk.h>
#include <linux/workqueue.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include <mach/bsp.h>

#define DEVICE_NAME		"goodix_ts"
/*Hardware setting*/
//IRQ
#define TS_EINT_PORT_EN		HwGPIOD->GPEN
#define TS_EINT_PORT_DAT	HwGPIOD->GPDAT
#define TS_EINT_PIN			Hw14
#define TS_EINT_IRQ_NUM		INT_EI11
#define TS_EINT_SEL			HwEINTSEL2_EINT11(SEL_GPIOD14)
#define TS_EINT_SEL_MASK	HwEINTSEL2_EINT11_MASK
#define TS_EINT_SEL_ID 		HwINT0_EI11
//slave device wake up control
#define TS_WAKEUP_PORT_EN		HwGPIOD->GPEN
#define TS_WAKEUP_PORT_DAT	HwGPIOD->GPDAT
#define TS_WAKEUP_PIN			Hw12
	#define TS_WAKEUP_DISABLE	0
	#define TS_WAKEUP_ENABLE	1
	
#define 	RESOLUTION_X	(800)
#define 	RESOLUTION_Y	(480)
#define LCD_MAX_LENGTH			RESOLUTION_X
#define LCD_MAX_WIDTH			RESOLUTION_Y

#define GOODIX_TOUCH__LEFT		0
#define GOODIX_TOUCH__RIGHT		7680
#define GOODIX_TOUCH__TOP		    	5120
#define GOODIX_TOUCH__BOTTOM	    	0
#define GOODIX_TOUCH_WEIGHT_MAX 		150

#define AD_TO_Y(ady)	(LCD_MAX_WIDTH * (GOODIX_TOUCH__TOP - ady)) / (GOODIX_TOUCH__TOP - GOODIX_TOUCH__BOTTOM)
#define AD_TO_X(adx)	(LCD_MAX_LENGTH * (adx - GOODIX_TOUCH__LEFT)) / (GOODIX_TOUCH__RIGHT - GOODIX_TOUCH__LEFT)

#define NO_TOUCH	(0xffff)



#define GOODIX_IRQ 		   7
#define GOODIX_ID_ADDR 	(0x69)
#define GOODIX_ID_LEN	 (34)

//finger count
#define FINGER5	(1u<<4)
#define FINGER4	(1u<<3)
#define FINGER3	(1u<<2)
#define FINGER2	(1u<<1)
#define FINGER1	(1u<<0)

#define PEN_DOWN 1
#define PEN_RELEASE 0

#define TS_POLL_DELAY	  (1*1000*1000)	/* ns delay before the first sample */
#define TS_POLL_PERIOD	  (30*1000*1000)	/* ns delay between samples */

#define HW_ADDR_INIT (0x30)

#define MULTI_TOUCH_SUPPORT

static u8 i2c_buf[54];
#if 0
static u8 HW_INIT[] = {25,5,6,40,2,20,20,16,80,184,20,0,30,0,1,35,69,103,137,171,205,224,
					0,0,0,0,13,207,32,11,11,139,80,60,30,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
#else
static u8 HW_INIT[] = {0x19,0x05,0x06,0x28,0x02,0x14,0x14,0x10,0x50,0xb8,0x14,0,0x1e,0,1,0x23,0x45,0x67,0x89,0xab,0xcd,0xe1,
					0,0,0,0,0x05,0xcf,0x20,0x05,0x9,0x8b,0x50,0x3c,0x1e,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
#endif
#if 0
#define DPRINTK printk
#else
#define DPRINTK(x...) (void)(0)
#endif

//for debug
#define debug 0

struct goodix_ts {		
	struct input_dev *input_dev;		
    	u16 	touch_x1;
	u16 	touch_y1;
    	u16	touch_x2;
	u16 	touch_y2;
	int reported_finger_count;
	int pen_state;
	u8	touch_pop;
	u8	ts_dist;
	int 	irq;  	
	struct i2c_client *client;
	struct work_struct scan_work;
#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
#endif
};

/*
*	return 0:success
*	return negative:fail
*/
static int goodix_read_regs(struct i2c_client *client, u8 reg, u8 buf[], unsigned len)
{
	int ret;
	struct i2c_msg msgs[2] = {
		{ client->addr, 0, 1, buf },//send
		{ client->addr, I2C_M_RD, len, buf },//recv
	};

	buf[0] = reg;
	msgs[1].flags |= (client->flags & I2C_M_RD);
	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret < 0)
	{
		pr_err(" read regs error \n");
	}
	return ret;
}

/*
*	return 0:success
*	return negative:fail
*/
static int goodix_set_regs(struct i2c_client *client, u8 reg, u8 const buf[], __u16 len)
{
	int ret;
	struct i2c_msg msgs[1] = {
		{ client->addr, 0, len+1, i2c_buf }
	};

	i2c_buf[0] = reg;
	memcpy(&i2c_buf[1], &buf[0], len);

	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret< 0)
	{
		pr_err(" write regs error \n");
	}
	return ret;
}

#ifndef MULTI_TOUCH_SUPPORT
static int read_position(goodix_ts *ts)
{
	u8 read_buf[33];
	int err = 0;
	uint coorlen = 0;
	uint len = 0;
	u16 xpos1 = 0, ypos1 = 0;
	int i;
	uint checksum = 0;

	
	memset(&read_buf,0,sizeof(read_buf));

	//read coorlen
	err = goodix_read_regs(client,0x00,read_buf,1);
	if (err < 0)
	{
		pr_err("read coorlen failed\n");
		return err;
	}

	coorlen = read_buf[0];
	DPRINTK("coorlen:0x%x\n",coorlen);

	if (coorlen == 0)
	{
		return NO_TOUCH;
	}
	//judge how many finger press down
	if (coorlen & FINGER5)
		len = 33;
	else if (coorlen & FINGER4)
		len = 28;
	else if (coorlen & FINGER3)
		len = 18;
	else if (coorlen & FINGER2)
		len = 12;
	else if (coorlen & FINGER1)
		len = 7;

	if (len > 18)
	{
		pr_err("not support up to 4 fingers multiple touch\n");
		return -EIO;
	}
	err = goodix_read_regs(client,0x00,read_buf,len + 1);//read one more Byte for date checksum
	if (err < 0)
	{
		pr_err("read coor xy failed\n");
		return err;
	}
	for (i=0;i<len;i++)
	{
		//DPRINTK("buf[%d]=%d\n",i,read_buf[i]);
		checksum += read_buf[i];
	}
	checksum &= 0xff;
	
	if (checksum != read_buf[len])
	{
		pr_err("checksum failed:checksum=%d,checksum_buf=%d\n",checksum,read_buf[len]);
		return -EIO;
	}

	DPRINTK("checksum:%d,checksum_buf:%d\n",checksum,read_buf[len]);
	
	ypos1 = (((u16)read_buf[2]) << 8 ) |((u16)read_buf[3]);
	xpos1 = (((u16)read_buf[4]) << 8 ) |((u16)read_buf[5]);
	
	ts->touch_x1 = AD_TO_X(xpos1);
	ts->touch_y1 = AD_TO_Y(ypos1);
	
	DPRINTK("pos (%d, %d)\n", ts->touch_x1, ts->touch_y1); 
	
	return err;
}
#endif


#ifdef MULTI_TOUCH_SUPPORT
void goodix_ts_scan_work(struct work_struct *work)
{
	struct goodix_ts *ts= container_of(work, struct goodix_ts, scan_work);
	int err = 0, i;
	u8 read_buf[35];
	uint coorlen = 0;
	uint len = 0;
	uint checksum = 0;
	u16 xpos1 = 0, ypos1 = 0, xpos2 = 0, ypos2 = 0;
	int finger, finger2;
	
	memset(&read_buf,0,sizeof(read_buf));
	//read coorlen
	err = goodix_read_regs(ts->client, 0x00, read_buf, 13);
	if (err < 0){
		pr_err("read coorlen failed\n");
		return;
	}

	coorlen = read_buf[0];
//	DPRINTK("coorlen:0x%x\n",coorlen);
	if (coorlen == 0){
		goto no_touch;
	}
#if 0
	//judge how many finger press down
	if (coorlen & FINGER5)
		len = 33;
	else if (coorlen & FINGER4)
		len = 28;
	else if (coorlen & FINGER3)
		len = 17;
	else if (coorlen & FINGER2)
		len = 12;
	else if (coorlen & FINGER1)
		len = 7;
	
	err = goodix_read_regs(ts->client, 0x00, read_buf, len + 1);//read one more Byte for date checksum
	if (err < 0)
	{
		pr_err("read coor xy failed\n");
		return;
	}
#endif
#if 0//test
	pr_info("print read data buffer[tatal %d]:\n", len+1);
	for(i = 0; i <= len; i++){
		printk("%d, ", read_buf[i]);
	}
	printk("\n");
#endif
#if 0
	for (i=0; i<len; i++)
	{
		//DPRINTK("buf[%d]=%d\n",i,read_buf[i]);
		checksum += read_buf[i];
	}
	checksum &= 0xff;
	
	if (checksum != read_buf[len])
	{
		pr_err("checksum failed:checksum=%d,checksum_buf=%d\n",checksum,read_buf[len]);
		return;// -EIO;
	}
//	DPRINTK("checksum:%d,checksum_buf:%d\n",checksum,read_buf[len]);
#endif

	ypos1 = (((u16)read_buf[2]) << 8 ) |((u16)read_buf[3]);
	xpos1 = (((u16)read_buf[4]) << 8 ) |((u16)read_buf[5]);
	
	ts->touch_x1 = AD_TO_X(xpos1);
	ts->touch_y1 = AD_TO_Y(ypos1);
	finger = read_buf[0] & 0x1F;
	//pr_info("Finger*1 (%d, %d) w[%d]\n", ts->touch_x1, ts->touch_y1, read_buf[6]); 
	input_report_abs(ts->input_dev, ABS_X, ts->touch_x1 );
	input_report_abs(ts->input_dev, ABS_Y, ts->touch_y1 );
	input_report_abs(ts->input_dev, ABS_PRESSURE, read_buf[6]);
	input_report_abs(ts->input_dev, ABS_TOOL_WIDTH, read_buf[6]);
	input_report_key(ts->input_dev, BTN_TOUCH, finger);
	
	finger2 = coorlen > 1 && coorlen != 7;
	if (finger2){
		ypos2 = (((u16)read_buf[7]) << 8 ) |((u16)read_buf[8]);
		xpos2 = (((u16)read_buf[9]) << 8 ) |((u16)read_buf[10]);		
		ts->touch_x2 = AD_TO_X(xpos2);
		ts->touch_y2 = AD_TO_Y(ypos2);
		//pr_info("Finger*2 (%d, %d) w[%d]\n", ts->touch_x2, ts->touch_y2, read_buf[11]); 
		input_report_key(ts->input_dev, BTN_2, finger2);
		input_report_abs(ts->input_dev, ABS_HAT0X, ts->touch_x2);
		input_report_abs(ts->input_dev, ABS_HAT0Y, ts->touch_y2);
	}
	input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, read_buf[6]);
	input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, read_buf[6]);
	input_report_abs(ts->input_dev, ABS_MT_POSITION_X, ts->touch_x1);
	input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, ts->touch_y1);
	input_mt_sync(ts->input_dev);
	if(finger2){
		input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, read_buf[11]);
		input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, read_buf[11]);
		input_report_abs(ts->input_dev, ABS_MT_POSITION_X, ts->touch_x2);
		input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, ts->touch_y2);
		input_mt_sync(ts->input_dev);
	}else if(ts->reported_finger_count > 1){
		input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0);
		input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0);
		input_mt_sync(ts->input_dev);
	}
	ts->reported_finger_count = finger;
	input_sync(ts->input_dev);
	ts->pen_state = PEN_DOWN;

	return;
	
no_touch:
	if(PEN_DOWN == ts->pen_state){
		ts->pen_state = PEN_RELEASE;
    		input_report_key(ts->input_dev, BTN_TOUCH, 0);
		input_report_key(ts->input_dev, BTN_2, 0);
		input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0);
		input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0);
		input_report_abs(ts->input_dev, ABS_MT_POSITION_X, 0);
		input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, 0);
		input_mt_sync(ts->input_dev);
		input_sync(ts->input_dev);
	}
}
#else
void goodix_ts_scan_work(struct work_struct *work)
{
	struct goodix_ts *ts= container_of(work, struct goodix_ts, scan_work);
	int err = 0;

    	err = read_position(ts);
		
	//finger up
	if (err == NO_TOUCH)
    	{
    		DPRINTK("finger up\n");

    		input_report_key(ts->input_dev,BTN_TOUCH,0);
	   	input_sync(ts->input_dev);
//		gpio_irq_enable(TS_INT_PIN);
		
    	}
	//finger down.
	else
	{
   		DPRINTK("finger down\n");
		//pr_info("touch pos:(%d, %d)\n", ts->touch_x, ts->touch_y);
		input_report_abs(ts->input_dev,ABS_X,ts->touch_x );
		input_report_abs(ts->input_dev,ABS_Y,ts->touch_y );
		input_report_key(ts->input_dev,BTN_TOUCH,1);
		input_sync(ts->input_dev);
		//hrtimer_start(&ts->timer,ktime_set(0,TS_POLL_PERIOD),HRTIMER_MODE_REL);	
//		gpio_irq_enable(TS_INT_PIN);	
	}
	
}
#endif

static irqreturn_t goodix_ts_interrupt(int irq, void *dev_id)
{
	struct goodix_ts *ts = (struct goodix_ts *)dev_id;	

	DPRINTK("goodix_ts_interrupt.\n");

	schedule_work(&ts->scan_work);
//	mod_timer(&ts->timer, jiffies + 20);
	
	return IRQ_HANDLED;
}

static int goodix_init(struct i2c_client *client)
{
	u8 buf[53];
	int err = 0;
	int i;
	int len = 0;
	uint HW_INIT_CRC = 0;
	uint tmp;
	
	len = sizeof(HW_INIT)/sizeof(HW_INIT[0]);

	for (i=0;i<len;i++)
	{
		buf[i] = HW_INIT[i];	
		HW_INIT_CRC += buf[i];
	}

	err = goodix_set_regs(client,HW_ADDR_INIT,buf,len);
	if (err < 0) 
	{
		DPRINTK("hw init failed\n");
		return -1;
	}
	HW_INIT_CRC -= buf[len -1];
	//change direction 
	#if 0
	buf[0] = 1;
	err = goodix_set_regs(client,0x68,buf,1);
	if (err < 0) 
	{
		DPRINTK("change direction failed\n");
		return -1;
	}
	#endif

	mdelay(100);
	err = goodix_read_regs(client, 0x65, buf, 2);
	if (err < 0){
		DPRINTK("read config crc error\n");
	}
	tmp = ((uint)buf[0] << 8) | buf[1];
	if (tmp != HW_INIT_CRC){
		pr_err("set goodix hw config reg failed:HW_INIT_CRC:%d,0x6566:%d\n",HW_INIT_CRC,tmp);
		return -1;
	}
	DPRINTK("set goodix hw config success:HW_INIT_CRC:%d,0x6566:%d\n",HW_INIT_CRC,tmp);

	#if 0//debug
	memset(buf,0,len);
	err = goodix_read_regs(client,HW_ADDR_INIT,buf,len);
	if (err < 0) 
	{
		DPRINTK("hw init failed\n");
		return -1;
	}
	
	DPRINTK("read config len=%d:\n",len);
	for(i=0;i<len;i++)
	DPRINTK("%d",buf[i]);
	
	//read id
	mdelay(600);
	buf[0] = 0xff;
	err = goodix_set_regs(client,GOODIX_ID_ADDR,buf,1);
	if (err < 0)
	{	
		
		DPRINTK("read id error\n");
		return -1;
	}
	mdelay(25);
	err = goodix_read_regs(client, GOODIX_ID_ADDR + 1, buf, GOODIX_ID_LEN);
	if (err < 0)
	{
		DPRINTK("read id error\n");
	}

	DPRINTK("GOODIX ID:\n");
	for (i=0;i<GOODIX_ID_LEN;i++)
		DPRINTK("%c",buf[i]);
	DPRINTK("\n");
	#endif
	
	//for interrupt pulse control
	buf[0] = 0;
	err = goodix_set_regs(client,GOODIX_ID_ADDR,buf,1);
	if (err < 0)
	{
		
		DPRINTK("read id error\n");
		return -1;
	}
	
	DPRINTK("\n init goodix success\n");
	
	return 0;
}


static void tcc8902_irq_hw_init(void)
{
	// Interrupt Control
	BITCLR(TS_EINT_PORT_EN, TS_EINT_PIN);  //Input mode
	BITCSET(HwEINTSEL->EINTSEL2, TS_EINT_SEL_MASK, TS_EINT_SEL);

	HwPIC->INTMSK0 |= TS_EINT_SEL_ID;	//enable irq
	
	HwPIC->IEN0 	&= ~(TS_EINT_SEL_ID);						/* disable I2C */
	HwPIC->MODEA0	&= ~(TS_EINT_SEL_ID) ; 					/* signle edge */

	HwPIC->MODE0	&= ~(TS_EINT_SEL_ID);						/* set edge trigger mode */	

	HwPIC->POL0 	|= (TS_EINT_SEL_ID);						/* active-high */
	HwPIC->CLR0 	=  TS_EINT_SEL_ID; 						/* clear pending status */
	HwPIC->IEN0 	|= (TS_EINT_SEL_ID);						/* Enable Interrupt */

//	BITSET(HwPOL, HwINT_EI2);
//	BITCLR(HwMODE, HwINT_EI2);

}

static void goodix_wakeup_init(void)
{
	BITCSET(TS_WAKEUP_PORT_EN, TS_WAKEUP_PIN, TS_WAKEUP_PIN);  //output mode

	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, TS_WAKEUP_PIN);//HIGH to enter sleep mode
	mdelay(100);
	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, 0);//set low to wakeup goodix device
	mdelay(100);
	/*if(TS_WAKEUP_ENABLE == mode){
		BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, 0);//set low to wakeup goodix device
	}
	else{//(TS_WAKEUP_DISABLE == mode)
		BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, TS_WAKEUP_PIN);//HIGH to enter sleep mode
	}*/
}


#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc8902_tsc_early_suspend(struct early_suspend *h)
{
	pr_info("%s() \n", __FUNCTION__);
	HwPIC->INTMSK0 &= ~TS_EINT_SEL_ID;	//disable irq
	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, TS_WAKEUP_PIN);//HIGH to enter sleep mode
}

static void tcc8902_tsc_late_resume(struct early_suspend *h)
{
	pr_info("%s() \n", __FUNCTION__);
	HwPIC->INTMSK0 |= TS_EINT_SEL_ID;	//enable irq
	BITCSET(TS_WAKEUP_PORT_DAT, TS_WAKEUP_PIN, 0);//set low to wakeup goodix device
}

#endif

static int goodix_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct goodix_ts *ts;
	int ret = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk(KERN_ERR "%s: need I2C_FUNC_I2C\n", __func__);
		ret = -ENODEV;
		goto err_check_functionality_failed;
	}
	
	ts = kzalloc(sizeof(struct goodix_ts), GFP_KERNEL);
	if (ts == NULL) {
		ret = -ENOMEM;
		goto err_alloc_data_failed;
	}

	INIT_WORK(&ts->scan_work, goodix_ts_scan_work);
	ts->client = client;
	i2c_set_clientdata(client, ts);

	// TODO: add device check routine
	
	ts->input_dev = input_allocate_device();
	if (ts->input_dev == NULL) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate input device\n", __func__);
		goto err_input_dev_alloc_failed;
	}

	ts->input_dev->name = "tcc-goodix-ts";

#ifdef MULTI_TOUCH_SUPPORT
	set_bit(EV_SYN, ts->input_dev->evbit);
	set_bit(EV_KEY, ts->input_dev->evbit);
	set_bit(EV_ABS, ts->input_dev->evbit);
	set_bit(BTN_TOUCH, ts->input_dev->keybit);
	set_bit(BTN_2, ts->input_dev->keybit);
	input_set_abs_params(ts->input_dev, ABS_X, 0, RESOLUTION_X, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_Y, 0, RESOLUTION_Y, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_PRESSURE, 0, GOODIX_TOUCH_WEIGHT_MAX, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_TOOL_WIDTH, GOODIX_TOUCH_WEIGHT_MAX, 15, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_HAT0X, 0, RESOLUTION_X, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_HAT0Y, 0, RESOLUTION_Y, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, RESOLUTION_X, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, RESOLUTION_Y, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, GOODIX_TOUCH_WEIGHT_MAX, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, GOODIX_TOUCH_WEIGHT_MAX, 0, 0);	
#else
	ts->input_dev->evbit[0] = BIT_MASK(EV_ABS)|BIT_MASK(EV_KEY);
	ts->input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input_set_abs_params(ts->input_dev,ABS_X,0,RESOLUTION_X,0,0);
	input_set_abs_params(ts->input_dev,ABS_Y,0,RESOLUTION_Y,0,0);
#endif

	tcc8902_irq_hw_init();
	//Low to wakeup goodix
	goodix_wakeup_init();

	ret = goodix_init(ts->client);
	if (ret < 0)
	{
		ret = -ENXIO;
		goto exit_attach;
	}

	ret = input_register_device(ts->input_dev);
	if (ret) {
		printk(KERN_ERR "%s: Unable to register %s input device\n", __func__, ts->input_dev->name);
		goto err_input_register_device_failed;
	}

	ts->irq = TS_EINT_IRQ_NUM;
	if (request_irq(ts->irq, goodix_ts_interrupt, IRQF_DISABLED, DEVICE_NAME, ts)){
		ret = -EBUSY;
		goto exit_irq;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.suspend 	= tcc8902_tsc_early_suspend;
	ts->early_suspend.resume 	= tcc8902_tsc_late_resume;
	ts->early_suspend.level 	= EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&ts->early_suspend);
#endif

	pr_info("Goodix i2c touchscreen device probe done.\n");	
	return 0;

	free_irq(ts->irq, ts);
exit_irq:
err_input_register_device_failed:
	input_free_device(ts->input_dev);
exit_attach:
err_detect_failed:
err_input_dev_alloc_failed:
	kfree(ts);
err_alloc_data_failed:
err_check_functionality_failed:
	return ret;
}

static int __devexit goodix_i2c_remove(struct i2c_client *client)
{
	struct goodix_ts *ts = i2c_get_clientdata(client);
	unregister_early_suspend(&ts->early_suspend);
	free_irq(ts->irq, ts);
	input_unregister_device(ts->input_dev);
	kfree(ts);
	return 0;
}

static int goodix_i2c_suspend(struct i2c_client *client, pm_message_t state)
{
	return 0;
}

static int goodix_i2c_resume(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id goodix_i2c_id[] = {
	{ "goodix_ts", 0 },
	{ }
};

static struct i2c_driver goodix_i2c_driver = {
	.probe		= goodix_i2c_probe,
	.remove		= goodix_i2c_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend    = goodix_i2c_suspend,
	.resume     = goodix_i2c_resume,
#endif
	.driver = {
		.name = "goodix_ts",
		.owner = THIS_MODULE,
	},
	.id_table = goodix_i2c_id,
};



static int __init goodix_i2c_init(void)
{
	printk("Goodix i2c touch panel device driver module init\n");
	return i2c_add_driver(&goodix_i2c_driver);
}


static void __exit goodix_i2c_exit(void)
{
	i2c_del_driver(&goodix_i2c_driver);
}

module_init(goodix_i2c_init);
module_exit(goodix_i2c_exit);

MODULE_AUTHOR("liuxianghui@wisky.com.cn");
MODULE_DESCRIPTION("goodix I2C driver");
MODULE_LICENSE("GPL");


