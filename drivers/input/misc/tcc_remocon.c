/*
 * IR driver for remote controller : tcc_remocon.c
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/capability.h>
#include <linux/uio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/cpufreq.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <mach/bsp.h>
#include <mach/pm.h>
#include <mach/gpio.h>

#include <linux/time.h>

#include "tcc_remocon.h"

extern struct clk *clk_get(struct device *dev, const char *id);
extern int clk_enable(struct clk *clk);
extern void clk_disable(struct clk *clk);

#define DEVICE_NAME	"tcc-remote"

#define TCC_IRVERSION	0.001

#define IR_IRQ			INT_RMT

#define KEY_RELEASED	0
#define KEY_PRESSED		1

#if 0
#define dbg printk
#else /* no debug */
#define dbg(x...) do {} while(0)
#endif

/*****************************************************************************
*
* structures
*
******************************************************************************/
struct tcc_remote
{
	char old_key;		//Previous KeyCode
	int status;		//Key Status

	struct input_dev *dev;  
	struct work_struct work_q;
};

typedef struct
{
	unsigned int repeatCount;
	unsigned int BitCnt;	//Data Bit Count
	unsigned long long Buf;	//Data Buffer
	unsigned int Id;	//Remocon ID
	unsigned int Code;	//Parsing Return Value
	unsigned int Stat;	//Remocon Status
}REM_DATA;

static int remote_state = 0;

static int startbit_time;
static int prev_startbit_time;

static struct tcc_remote *rem;
static REM_DATA Rem;

static struct timer_list remocon_timer;

static int clock_set_enable=0;

extern struct tcc_freq_table_t gtRemoconClockLimitTable;

extern char fb_power_state;

unsigned int tcc_remocon_GetTickCount(void)
{
	struct timeval tTimeVal;

	do_gettimeofday(&tTimeVal);

	dbg("%d sec %d usec\n", tTimeVal.tv_sec, tTimeVal.tv_usec);

	return tTimeVal.tv_sec * 1000 + tTimeVal.tv_usec / 1000;
}

/*****************************************************************************
* Function Name : static int tcc_remocon_set_clock_table(int enable);
* Description : remocon clock limit is enable or disable.
* Arguments :
******************************************************************************/
static int tcc_remocon_set_clock_table(int enable)
{
	if(clock_set_enable != enable)
	{
		tcc_cpufreq_set_limit_table(&gtRemoconClockLimitTable, TCC_FREQ_LIMIT_REMOCON, enable);
	}

	clock_set_enable = enable;
	ndelay(1);
	return 0;
}

/*****************************************************************************
* Function Name : static void Init_IR_Port(void);
* Description : IR port register init
* Arguments :
******************************************************************************/
static void Init_IR_Port(void)
{
#if defined(CONFIG_ARCH_TCC892X)
	tcc_gpio_config(TCC_GPG(17), GPIO_FN7|GPIO_OUTPUT|GPIO_LOW);
#else
	PGPIO pGpioA = (volatile PGPIO)tcc_p2v(HwGPIOA_BASE);
	BITCSET(pGpioA->GPAFN0, (Hw20 | Hw21 | Hw22 | Hw23), Hw20);	//GPIO_A5
#endif
}

/*****************************************************************************
* Function Name : static void remocon_irq_init(void);
* Description : IRQ register init
* Arguments :
******************************************************************************/
static void remocon_irq_init(void)
{
	volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

	#if defined(CONFIG_ARCH_TCC892X)
		BITSET(pPIC->IEN1.nREG, Hw10);
		BITSET(pPIC->SEL1.nREG, Hw10);
		BITSET(pPIC->INTMSK1.nREG, Hw10);
		BITCLR(pPIC->POL1.nREG, Hw10);
		BITSET(pPIC->MODE1.nREG, Hw10);
		BITCLR(pPIC->MODEA1.nREG, Hw10);
	#else
	BITSET(pPIC->IEN1, HwINT1_RMT);
	BITSET(pPIC->SEL1, HwINT1_RMT);
	BITSET(pPIC->INTMSK1, HwINT1_RMT);
	BITCLR(pPIC->POL1, HwINT1_RMT);
	BITSET(pPIC->MODE1, HwINT1_RMT);
	BITCLR(pPIC->MODEA1, HwINT1_RMT);
	#endif
}

/*****************************************************************************
* Function Name : static void remocon_timer_handler(unsigned long data);
* Description : timer event handler for key release.
* Arguments :
******************************************************************************/
static void remocon_timer_handler(unsigned long data)
{
	struct input_dev *dev = rem->dev;

	if(rem->status==KEY_PRESSED)
	{
		input_report_key(dev, rem->old_key, 0);
		input_sync(dev);
		dbg("############### nRem=%d released(timer)\n",rem->old_key);
		rem->status = KEY_RELEASED;
		rem->old_key = NO_KEY;
		Rem.Code = NO_KEY;
	}
}

/*****************************************************************************
* Function Name : static int tca_rem_getkeycodebyscancode(unsigned short kc);
* Description : transform ScanCode into KeyCode.
* Arguments : kc - scancode
******************************************************************************/
static int tca_rem_getkeycodebyscancode(unsigned short kc)
{
	int i;
	for (i = 0;i < sizeof(key_mapping)/sizeof(SCANCODE_MAPPING);i++)
		if (kc == key_mapping[i].rcode) 
			return key_mapping[i].vkcode;
	return -1;
}

/*****************************************************************************
* Function Name : static void tca_rem_sendremocondata(unsigned int key_type);
* Description : key event is generated.
* Arguments : Data - key type(new or repeat)
******************************************************************************/
static void tca_rem_sendremocondata(unsigned int key_type)
{
	struct input_dev *dev = rem->dev;
	int nRem, repeatCheck = 1;

	switch(key_type) {
	case NEW_KEY:
		nRem = tca_rem_getkeycodebyscancode(Rem.Code);
		if(nRem == -1 || (fb_power_state==0 && nRem!=REM_POWER))
			return;
		del_timer(&remocon_timer);
		if(rem->status==KEY_PRESSED) {
			dbg("############### nRem=%d released\n",rem->old_key);
			input_report_key(dev, rem->old_key, 0);
			input_sync(dev);
			rem->status=KEY_RELEASED;
		}
		dbg("############### nRem=%d pressed\n", nRem);
		rem->status = KEY_PRESSED;
		input_report_key(dev, nRem, 1);
		input_sync(dev);
		rem->old_key = nRem;
		Rem.repeatCount = 0;
		break;
	case REPEAT_KEY:
		if(fb_power_state==0 || !isRepeatableKey(rem->old_key))
			return;
		if(rem->old_key == REM_POWER)
            repeatCheck = 15;

		del_timer(&remocon_timer);
		if(rem->status==KEY_PRESSED && Rem.repeatCount>repeatCheck) {
			dbg("############### nRem=%d repeat=%d\n",rem->old_key, Rem.repeatCount);
			input_event(dev, EV_KEY, rem->old_key, 2);    // repeat event
			input_sync(dev);
		}
		Rem.repeatCount++;
		break;
	case REPEAT_START:
		if(fb_power_state==0 || !isRepeatableKey(rem->old_key))
			return;
		Rem.repeatCount = 0;
		del_timer(&remocon_timer);
	}
	remocon_timer.expires = jiffies + msecs_to_jiffies(150);//msecs_to_jiffies(125);
	add_timer(&remocon_timer);
}

/*****************************************************************************
* Function Name : static unsigned int tca_rem_readcode(char ch);
* Description : Decode readCode.
* Arguments : ch - readcode, return value - key type(new or repeat)
******************************************************************************/
static unsigned int tca_rem_readcode(char ch)
{
	switch (Rem.Stat)
	{
	/* Initialize */
	case STATUS0:
		Rem.BitCnt = Rem.Buf = 0;
		Rem.Stat = STATUS1;

		if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800() || machine_is_tcc8800st() || machine_is_tcc8920() || machine_is_tcc8920st())
		{
			if (ch == 'S')
			{
				Rem.Stat = STATUS2;
			}
			else if (ch == 'R')
			{
				//tcnt=12.5ms
				return REPEAT_KEY;
			}
		}
		break;

	/*Start Bit */
	case  STATUS1:
		if (ch == 'S') 	//tcnt = 15ms 
		{
			//it appears to be HW bug that the 1st repeat pule is always 
			//recognized wrongly as start bit 
			if (startbit_time != prev_startbit_time)
			{
				prev_startbit_time = startbit_time;
				Rem.Code = 0;	//reset the memorized code if we get a start bit
			}
			Rem.Stat = STATUS2;
		}

		//Repeat
		else if (ch == 'R')
		{
			dbg("R: %llx\n", Rem.Code);
			//tcnt=12.5ms
			return REPEAT_KEY;
		}
		break;

	/* Data Code Check */
	case STATUS2:
		if (ch == '0')			//Bit '0' = 3.5ms
		{
			Rem.BitCnt++;
		}
		else if (ch == '1')		//Bit '1' = 5.5ms
		{
			Rem.Buf |= (((unsigned long long)1) << Rem.BitCnt);
			Rem.BitCnt++;
		}
		else if (ch == 'S')		//If Receive Start Bit, Return 0;
		{
			return 0;
		}
		else
		{
			Rem.Stat =STATUS0;
		}

		if(Rem.BitCnt == IR_SIGNAL_MAXIMUN_BIT_COUNT)
		{
			dbg("############### BUF=%llx\n", Rem.Buf);

			Rem.Id = (Rem.Buf & IR_ID_MASK);
			dbg("############### ID=%x\n", Rem.Id);

			if (Rem.Buf == REMOCON_REPEAT)
			{
				Rem.Stat = STATUS0;
				return REPEAT_START;
			}
			else if (Rem.Id == REMOCON_ID)
			{
				Rem.Code = (Rem.Buf  & IR_CODE_MASK) >> IR_BUFFER_BIT_SHIFT;
				dbg("############### CODE=%llx\n", Rem.Code);
				Rem.Stat = STATUS0;
				return NEW_KEY;
			}
			else
			{
				Rem.Stat = STATUS0;
			}
		}
		break;
	}

	return NO_KEY;
}

/*****************************************************************************
* Function Name : static int tca_rem_process(char ch);
* Description : data processing
* Arguments : ch - rawdata, return value(0: success, -1: failed)
******************************************************************************/
static int tca_rem_process(char ch)
{
	int key_type = tca_rem_readcode(ch);

	if (key_type==NO_KEY)
		return -1;

	tca_rem_sendremocondata(key_type);
	return 0;
}

/*****************************************************************************
* Function Name : static void rem_getrawdata(int *rd_data);
* Description : transform IR input value into rawdata
* Arguments : rd_data - IR input value
******************************************************************************/
static void rem_getrawdata(int *rd_data)
{
	int i=0;
	int low, high;
	char low_bit='x', high_bit='x';

#if defined(CONFIG_ARCH_TCC892X)
	#define LOW_MIN_VALUE		LOW_MIN_TCC892X
	#define LOW_MAX_VALUE		LOW_MAX_TCC892X
	#define HIGH_MIN_VALUE		HIGH_MIN_TCC892X
	#define HIGH_MAX_VALUE		HIGH_MAX_TCC892X
	#define REPEAT_MIN_VALUE	REPEAT_MIN_TCC892X
	#define REPEAT_MAX_VALUE	REPEAT_MAX_TCC892X
	#define START_MIN_VALUE		START_MIN_TCC892X
	#define START_MAX_VALUE		START_MAX_TCC892X
#else
	#define LOW_MIN_VALUE		LOW_MIN
	#define LOW_MAX_VALUE		LOW_MAX
	#define HIGH_MIN_VALUE		HIGH_MIN
	#define HIGH_MAX_VALUE		HIGH_MAX
	#define REPEAT_MIN_VALUE	REPEAT_MIN
	#define REPEAT_MAX_VALUE	REPEAT_MAX
	#define START_MIN_VALUE		START_MIN
	#define START_MAX_VALUE		START_MAX
#endif

	do
	{
		low = rd_data[i] & 0xffff;
		high = (rd_data[i] >> 16) & 0xffff;

		if ((low > LOW_MIN_VALUE) && (low < LOW_MAX_VALUE))
		{		  
			low_bit='0';
		}
		else if ((low > HIGH_MIN_VALUE) && (low < HIGH_MAX_VALUE))
		{
			low_bit='1';
		}
		else if ((low > REPEAT_MIN_VALUE) && (low < REPEAT_MAX_VALUE))
		{
			low_bit='R';
		}
		else if ((low > START_MIN_VALUE) && (low < START_MAX_VALUE))
		{
			low_bit='S';
			startbit_time= rd_data[i];  //get Start bit timing  and the 1st data bit timing
		}
		else
		{
			low_bit='E';
		}

		if ((high > LOW_MIN_VALUE) && (high < LOW_MAX_VALUE))
		{
			high_bit='0';
		}
		else if ((high > HIGH_MIN_VALUE) && (high < HIGH_MAX_VALUE))
		{
			high_bit='1';
		}
		else if ((high > REPEAT_MIN_VALUE) && (high < REPEAT_MAX_VALUE))
		{
			high_bit='R';
		}
		else if ((high > START_MIN_VALUE) && (high < START_MAX_VALUE))
		{
			high_bit='S';
			startbit_time= rd_data[i];  //get Start bit timing  and the 1st data bit timing
		}
		else
		{
			high_bit='E';
		}

		if(low_bit == 'S' || high_bit=='S') {
			dbg("\n############### start\n");
		}
		//dbg("%04x|%04x => %c%c\n", low, high, low_bit, high_bit);
		dbg("%04x|%04x => %c%c\n", low, high, low_bit, high_bit);

		if (tca_rem_process(low_bit) == 0 || tca_rem_process(high_bit) == 0)
		{
			break;
		}
	} while(++i < IR_FIFO_READ_COUNT);
}

/*****************************************************************************
* Function Name : static irqreturn_t remocon_handler(int irq, void *dev);
* Description : IR interrupt event handler
* Arguments :
******************************************************************************/
static irqreturn_t remocon_handler(int irq, void *dev)
{	
	int i=0;
	PREMOTECON pRcu = (volatile PREMOTECON)tcc_p2v(HwREMOTE_BASE);
	int rd_data[IR_FIFO_READ_COUNT];

	//tcc_remocon_GetTickCount();

	Rem.Stat = STATUS0;

	#if defined(CONFIG_ARCH_TCC892X)
		pRcu->CLKDIV.nREG = BITCLR(pRcu->CLKDIV.nREG, 0xFFFFFF00);	// Set CLK_DIV as 0
	#else
	pRcu->CLKDIV = BITCLR(pRcu->CLKDIV, 0xFFFFFF00);	// Set CLK_DIV as 0
	#endif

	udelay(150); //delay for fifo clear

	do
	{
		#if defined(CONFIG_ARCH_TCC892X)
		rd_data[i] = pRcu->RDATA.nREG;
		#else
		rd_data[i] = pRcu->RDATA;
		#endif
	}
	while (++i < IR_FIFO_READ_COUNT);

	rem_getrawdata(rd_data);

	udelay(100); //Remocon can't be so faster!!!

	RemoconInit(remote_state);

	//tcc_remocon_GetTickCount();

	return IRQ_HANDLED;
}

/*****************************************************************************
* Name :  Device register
* Description : This functions register device
* Arguments :
******************************************************************************/
static int remocon_probe(struct platform_device *pdev)
{
	int ret = -1;
	int err = -ENOMEM, i;
	struct input_dev *input_dev;

	if(machine_is_tcc8900() || machine_is_tcc9200() || machine_is_tcc9201() || machine_is_tcc9200s()) {
		tcc_remocon_set_clock_table(1);
	}
	else{
		struct clk *remote_clk = clk_get(NULL, "remocon");
		if(remote_clk>0)	clk_enable(remote_clk);
		else				printk("can't find remocon clk driver!");
	}

	rem = kzalloc(sizeof(struct tcc_remote), GFP_KERNEL);
	input_dev = input_allocate_device();

	if (!rem || !input_dev)
	{
		err = -ENOMEM;
		goto error_alloc;
	}
	platform_set_drvdata(pdev, rem);

	rem->dev = input_dev;

	rem->dev->name = "telechips_remote_controller";
	rem->dev->phys = DEVICE_NAME;
	rem->dev->evbit[0] = BIT(EV_KEY);
	rem->dev->id.version = TCC_IRVERSION;
	input_dev->dev.parent = &pdev->dev;
	for (i = 0; i < ARRAY_SIZE(key_mapping); i++)
	{
		set_bit(key_mapping[i].vkcode & KEY_MAX, rem->dev->keybit);
	}

	Init_IR_Port();
	RemoconConfigure ();
	RemoconStatus();
	RemoconDivide(remote_state);		//remocon clk divide and end_cout
	RemoconCommandOpen();				
	RemoconIntClear();
	remocon_irq_init();

	//Init IR variable
	rem->old_key = -1;
	rem->status = KEY_RELEASED;

	init_timer(&remocon_timer);
	remocon_timer.data = (unsigned long)NULL;
	remocon_timer.function = remocon_timer_handler;

#if defined(TCC_PM_SLEEP_WFI_USED) && defined(CONFIG_SLEEP_MODE)
	printk("IR request_irq with WFI mode\n");
	ret =  request_irq(IR_IRQ, remocon_handler, IRQF_DISABLED | IRQF_NO_SUSPEND, 
	DEVICE_NAME, rem);
#else
	printk("IR request_irq\n");
	ret =  request_irq(IR_IRQ, remocon_handler, IRQF_DISABLED, 
	DEVICE_NAME, rem);
#endif

	if (ret)
	{
		printk("IR remote request_irq error\n");
		goto error_irq;
	}

	ret = input_register_device(rem->dev);
	if (ret) 
		goto error_register;

	return 0;

error_alloc:
	input_free_device(input_dev);

error_irq:
	free_irq(IR_IRQ, rem);

error_register:
	input_unregister_device(rem->dev);

	return ret;
}

static int remocon_remove(struct platform_device *pdev)
{
	disable_irq(IR_IRQ);
	del_timer(&remocon_timer);
	free_irq(IR_IRQ, rem->dev);
	input_unregister_device(rem->dev);
	kfree(rem);

	if(machine_is_tcc8900() || machine_is_tcc9200() || machine_is_tcc9201() || machine_is_tcc9200s()) {
		tcc_remocon_set_clock_table(0);
	}
	else{
		struct clk *remote_clk = clk_get(NULL, "remocon");
		if(remote_clk>0)	clk_disable(remote_clk);
		else				printk("can't find remocon clk driver!");
	}

	return 0;
}

#ifdef CONFIG_PM
static int remocon_suspend(struct platform_device *pdev, pm_message_t state)
{
#if !defined(TCC_PM_SLEEP_WFI_USED) || !defined(CONFIG_SLEEP_MODE)
	DisableRemocon();
#else
	remote_state = 1;
	RemoconInit(remote_state);
#endif

	return 0;
}

static int remocon_resume(struct platform_device *pdev)
{
#if !defined(TCC_PM_SLEEP_WFI_USED) || !defined(CONFIG_SLEEP_MODE)
	Init_IR_Port();
	RemoconConfigure ();
	RemoconStatus();
	RemoconDivide(remote_state);		//remocon clk divide and end_cout
	RemoconCommandOpen();	
	RemoconIntClear();
	remocon_irq_init();
#endif

	remote_state = 0;
	RemoconInit(remote_state);

	return 0;
}
#else
#define remocon_suspend NULL
#define remocon_resume NULL
#endif

static struct platform_driver remocon_driver =
{
	.driver	=
	{
		.name	= DEVICE_NAME,
		.owner 	= THIS_MODULE,
	},
	.probe		= remocon_probe,
	.remove		= remocon_remove,
#ifdef CONFIG_PM
	.suspend		= remocon_suspend,
	.resume		= remocon_resume,
#endif
};

static int __init remocon_init(void)
{
	printk("Telechips Remote Controller Driver Init\n");
	return platform_driver_register(&remocon_driver);
}

static void __exit remocon_exit(void)
{
	printk("Telechips Remote Controller Driver Exit \n");
	platform_driver_unregister(&remocon_driver);
}

module_init(remocon_init);
module_exit(remocon_exit);

MODULE_AUTHOR("Linux Team<linux@telechips.com>");
MODULE_DESCRIPTION("IR remote control driver");
MODULE_LICENSE("GPL");
