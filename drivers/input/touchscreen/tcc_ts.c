/* linux/drivers/input/touchscreen/tcc-ts.c
 *
 * $Id: tcc-ts.c,v 1.3 2007/06/20 04:02:28 ihlee215 Exp $
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
 */
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/irq.h>

#include <linux/gpio.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>

#include <mach/bsp.h>
#include <mach/irqs.h>

#include <plat/tcc_ts.h>
#include <mach/tca_adconv.h>

#if defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
#define TCC_ADCE_12BIT_RES
#else
#define TCC_ADCE_10BIT_RES
#endif

/* For ts->dev.id.version */
#define TCC_TSVERSION           0x0001
#define IRQ_NO                  INT_EI2
#define TOUCH_CH                ADC_TOUCHSCREEN

#define ECFLG_END               (1<<15)
#define ENABLE_START_EN         (1<<0)

// ADCTSC
#define UD_SEN_DOWN             (0<<8)
#define YM_SEN_EN               (1<<7)
#define YP_SEN_DIS              (1<<6)
#define XM_SEN_DIS              (0<<5)
#define XP_SEN_DIS              (1<<4)
#define PULL_UP_EN              (0<<3)
#define AUTO_PST_DIS            (0<<2)
#define AUTO_PST_EN             (1<<2)
#define XY_PST_WAITINT          (3<<0)
#define ADCTSC_AUTO_ADC4        AUTO_PST_EN
#define ADCTSC_AUTO_ADC5        AUTO_PST_DIS

#define TOUCH_COLLECT_NR        8  // (TOUCH_COLLECT_NR-TOUCH_VALID_VALUE) must be even
#define TOUCH_VALID_VALUE       2  // among COLLECT_NR samples, OK should be over
#define TOUCH_TIMER_DELAY       10 // ms
#define TOUCH_RELEASE_TIME      1500 // ms
#if defined(CONFIG_ARCH_TCC92XX)
#define TOUCH_MAX_CONTACT       3000
#elif defined(CONFIG_ARCH_TCC88XX)
#define TOUCH_MAX_CONTACT       15000
#elif defined(CONFIG_ARCH_TCC892X)	
#define TOUCH_MAX_CONTACT       10000	
#else
#define TOUCH_MAX_CONTACT       15000
#endif      
#define TOUCH_MAX_PRESSURE      255

#define PEN_DOWN    1
#define PEN_RELEASE 0

#define ADCTSC_WAIT_PENDOWN (UD_SEN_DOWN |YM_SEN_EN|YP_SEN_DIS|XM_SEN_DIS|XP_SEN_DIS|PULL_UP_EN|AUTO_PST_DIS|XY_PST_WAITINT)

#define ADCCON      (0x00)
#define ADCTSC      (0x04)
#define ADCDLY      (0x08)
#define ADCDAT0     (0x0C)
#define ADCDAT1     (0x10)
#define ADCUPDN     (0x14)
#define ADCCLRINT   (0x18)
#define ADCCLRUPDN  (0x20)

#if 0
//#define dbg(x...) printk(KERN_DEBUG "tcc uart: ");
#define dbg printk
#else /* no debug */
#define dbg(x...) do {} while(0)
#endif


/*
 * Definitions & global arrays.
 */
static char *tcc_ts_name = "tcc-ts";

/*
 * Per-touchscreen data.
 */
struct tcc_ts {
	struct input_dev *dev;
	long x;
	long y;
	unsigned int pressure;
	char phys[32];
	int pen_status, opened, running, valid;
	struct mutex mutex;
	struct work_struct work_q;
	struct tcc_adc_client *client;
	struct timer_list ts_timer;
	struct timer_list ts_timer_for_cpu;
	struct tcc_adc_ts_platform_data *pdata;
#ifdef CONFIG_HAS_EARLYSUSPEND
        struct early_suspend early_suspend;
        struct early_suspend earlier_suspend;
#endif
};

static void __iomem *ts_base;
static int early_check = 0;
static unsigned int tcc_ts_release_time_cnt = 0;
static unsigned long extIntSEL;

extern  struct tcc_adc_client *tcc_adc_register(struct platform_device *pdev,
                                         void (*select)(unsigned int selected),
                                         void (*conv)(unsigned d0, unsigned d1),
                                         unsigned int is_ts);
extern  unsigned long tcc_adc_start(struct tcc_adc_client *client,
                   unsigned int channel, unsigned int nr_samples);
extern  void tcc_adc_release(struct tcc_adc_client *client);

#define ENABLE_START_EN (1<<0)

void tca_touchbubblesort(unsigned int Number[],unsigned int num)
{
	int i,j;
	unsigned int temp;
	for(i=0 ; i<(int)(num-1) ; i++)
	{
		for(j=i+1;j<(int)num;j++)
		{
			if(Number[j] != 0 &&  Number[i]>Number[j])
			{
				temp   = Number[i];
				Number[i] = Number[j];
				Number[j] = temp;
			}
		}
	}
}


static int tca_getrawdata(int * x, int * y, unsigned int * contact, struct tcc_ts *ts_data)
{
	unsigned int  i;
	unsigned int r_x[TOUCH_COLLECT_NR], r_y[TOUCH_COLLECT_NR], r_contact[TOUCH_COLLECT_NR];
	unsigned long  x_tol, y_tol, contact_tol;
	unsigned int m_pos_x, m_pos_y, m_contact;
	unsigned int validcnt=0;
	unsigned int index;

	memset(r_x, 0x00, sizeof(int) * TOUCH_COLLECT_NR);
	memset(r_y, 0x00, sizeof(int) * TOUCH_COLLECT_NR);

	m_pos_x = m_pos_y = m_contact = 0;

	for(i = 0; i < TOUCH_COLLECT_NR; i ++) {
		int xp, yp, xm, ym;

		if (ts_data->client) {
			tcc_adc_start(ts_data->client, TOUCH_CH, 0);
		}

		preempt_disable();
		//tca_adc_tsautoread(&xp, &yp, &xm, &ym);

		        tca_adc_tsread(&xp, &yp, &xm, &ym);
		r_contact[validcnt] = (xp*ym/(xm+1)-xp);

		dbg("x = %d , y = %d , r = %d\n", xp, yp, (xp*ym/(xm+1)-xp));

		preempt_enable();

#if defined(CONFIG_ARCH_TCC92XX)
		if (r_contact[validcnt] < TOUCH_MAX_CONTACT) {
			r_x[validcnt]=xp;
			r_y[validcnt]=yp;
			validcnt++;
		}
#else
		if ((xp < ts_data->pdata->max_x && xp > ts_data->pdata->min_x) &&
		    (yp < ts_data->pdata->max_y && yp > ts_data->pdata->min_y) &&
		    r_contact[validcnt] < TOUCH_MAX_CONTACT) {
			r_x[validcnt]=xp;
			r_y[validcnt]=yp;
			validcnt++;
		} else {
			//printk("#### (%d,%d), %d, %d  ==> %d ####\n", xp, yp, xm, ym, (xp*ym/(xm+1)-xp));
			//break;
		}
#endif
	}

	//printk("%s : %d\n", __func__, __LINE__);
	tca_touchbubblesort(r_x,validcnt);
	tca_touchbubblesort(r_y,validcnt);
	tca_touchbubblesort(r_contact, validcnt);
	x_tol = y_tol = contact_tol = 0; //sum the coordinate values
	index = (validcnt-TOUCH_VALID_VALUE)>>1;

	if(validcnt < (TOUCH_VALID_VALUE*2)) return -1;

	for(i=index;i<(index+TOUCH_VALID_VALUE);++i) {
		x_tol += r_x[i];
		y_tol += r_y[i];
		contact_tol += r_contact[i];
	}

	if (ts_data->pdata->swap_xy_pos) {
		m_pos_x = y_tol/(TOUCH_VALID_VALUE);
		m_pos_y = x_tol/(TOUCH_VALID_VALUE);
	} else {
		m_pos_x = x_tol/(TOUCH_VALID_VALUE);
		m_pos_y = y_tol/(TOUCH_VALID_VALUE);
	}
	m_contact = contact_tol/(TOUCH_VALID_VALUE);
	if (ts_data->pdata->reverse_x_pos) {
		*x = ts_data->pdata->max_x - (m_pos_x - ts_data->pdata->min_x);
	} else {
		*x = m_pos_x;
	}
	if (ts_data->pdata->reverse_y_pos) {
		*y = ts_data->pdata->max_y - (m_pos_y - ts_data->pdata->min_y);
	} else {
		*y = m_pos_y;
	}
	*contact = (TOUCH_MAX_CONTACT - m_contact)/((TOUCH_MAX_CONTACT + TOUCH_MAX_PRESSURE - 1)/TOUCH_MAX_PRESSURE);

	dbg("m_pos_x=%d, m_pos_y=%d\n", *x, *y);

	if ((*x <= ts_data->pdata->max_x && *x >= ts_data->pdata->min_x) &&
	    (*y <= ts_data->pdata->max_y && *y >= ts_data->pdata->min_y)) {
		return 0;
	} else {
		return -1;
	}
}

void tca_tch_poweroff(void)
{
	writel(Hw0, ts_base + ADCCLRINT);
	writel(Hw0, ts_base + ADCCLRUPDN);
	writel(ADCTSC_WAIT_PENDOWN, ts_base + ADCTSC);
}

static int pen_cnt = 0;
static unsigned long t_jiffies = 0;

static int tcc_adc_touch_status_f;
int tcc_adc_touch_status(void)
{
	return tcc_adc_touch_status_f;
}
EXPORT_SYMBOL(tcc_adc_touch_status);

static inline void tcc_pen_release(struct tcc_ts* ts_data)
{
	if (pen_cnt == 0) return;
	if (ts_data->pen_status != PEN_RELEASE) {
		ts_data->pen_status = PEN_RELEASE;

		tcc_adc_touch_status_f = 0;

		preempt_disable();
		input_report_key(ts_data->dev, BTN_TOUCH, PEN_RELEASE);
		input_report_abs(ts_data->dev, ABS_PRESSURE, 0);
		input_sync(ts_data->dev);
		preempt_enable();
		/* wake_up_interruptible(&ts->wait_q); */
	}
	dbg("PEN UP\n");
	pen_cnt = 0;
	t_jiffies = 0;
}

static inline void tcc_pen_pressure(struct tcc_ts* ts_data)
{
	if (pen_cnt == 0) t_jiffies = jiffies;
	pen_cnt ++;
	ts_data->pen_status = PEN_DOWN;

	tcc_adc_touch_status_f = 1;


	preempt_disable();
	input_report_key(ts_data->dev, BTN_TOUCH, PEN_DOWN);
	input_report_abs(ts_data->dev, ABS_X, ts_data->x);
	input_report_abs(ts_data->dev, ABS_Y, ts_data->y);
	input_report_abs(ts_data->dev, ABS_PRESSURE, ts_data->pressure);
	input_sync(ts_data->dev);
	preempt_enable();

	dbg("PEN DOWN (%ld : %ld)\n", ts_data->x, ts_data->y);

	/* wake_up_interruptible(&ts->wait_q); */
}

static void ts_fetch_thread(struct work_struct *work)
{
	struct tcc_ts* ts_data = container_of(work, struct tcc_ts, work_q);
	int flag, valid;
	int updown;
	unsigned long data0;
	unsigned long data1;
	//dbg("%s\n", __func__);

	ts_data->running = 1;

	//dbg("(work_q)disable_irq_nosync\n");
	//disable_irq_nosync(IRQ_NO);

	data0 = readl(ts_base + ADCDAT0);
	data1 = readl(ts_base + ADCDAT1);

	updown = (!(data0 & Hw15)) && (!(data1 & Hw15));

	if (!updown) {
		tcc_pen_release(ts_data);
		ts_data->running = 0;
		dbg("(work_q_1)enable_irq\n");
		enable_irq(IRQ_NO);
	} else {
		flag = tca_getrawdata((int *)&(ts_data->x), (int *)&(ts_data->y), (unsigned int *)&(ts_data->pressure), ts_data);
		dbg(" (%d, %d), %d\n", ts_data->x, ts_data->y, ts_data->pressure);
		tca_tch_poweroff();

		ndelay(1);

		valid = ts_data->x | ts_data->y;

		if ((flag == 0) && valid && (!early_check)) {
			tcc_pen_pressure(ts_data);
			ts_data->ts_timer.expires = jiffies + msecs_to_jiffies(TOUCH_TIMER_DELAY);
			add_timer(&ts_data->ts_timer);
			tcc_ts_release_time_cnt = 0;
		} else {
			if (tcc_ts_release_time_cnt >= TOUCH_RELEASE_TIME) {
				ts_data->running = 0;
				tcc_pen_release(ts_data);
				if(!early_check){
				    enable_irq(IRQ_NO);
				}
			}
			else {
				ts_data->ts_timer.expires = jiffies + msecs_to_jiffies(TOUCH_TIMER_DELAY);
				add_timer(&ts_data->ts_timer);
			}
			tcc_ts_release_time_cnt += TOUCH_TIMER_DELAY;
		}
	}
}


static irqreturn_t tcc_ts_interrupt(int irqno, void *param)
{
	struct tcc_ts *ts_data = (struct tcc_ts *)param;
	//dbg("%s\n", __func__);

	if (ts_data->running == 0) {
		ts_data->running = 1;
		//dbg("(irq)disable_irq_nosync\n");
		disable_irq_nosync(IRQ_NO);
		ts_data->ts_timer.expires = jiffies;
		add_timer(&ts_data->ts_timer);
	}

	return IRQ_HANDLED;
}

static void ts_timer_handler(unsigned long data)
{
	struct tcc_ts *ts_data = (struct tcc_ts *)data;
	//dbg("%s\n", __func__);
	if(ts_data->opened){
		if (schedule_work(&(ts_data->work_q)) == 0 ) {
			//dbg("cannot schedule work !!!\n");
			ts_data->running = 0;
			//dbg("(timer)enable_irq\n");
			enable_irq(IRQ_NO);
		}
	} else {
		//dbg("(timer)disable_irq_nosync\n");
		disable_irq_nosync(IRQ_NO);
	}
}

static void tcc_touch_select(unsigned int selected)
{
}

static void tcc_touch_convert(unsigned int selected)
{
}

static void tcc_ts_early_suspend(struct early_suspend *h)
{
        int updown;
        unsigned long data0;
        unsigned long data1;

	early_check = 1;

        data0 = readl(ts_base + ADCDAT0);
        data1 = readl(ts_base + ADCDAT1);

        updown = (!(data0 & Hw15)) && (!(data1 & Hw15));

	if(!updown)
	     disable_irq_nosync(IRQ_NO);


}

static void tcc_ts_late_resume(struct early_suspend *h)
{
	early_check = 0;
	tcc_adc_touch_status_f = 0;

	enable_irq(IRQ_NO);
}
/*
 * The functions for inserting/removing us as a module.
 */
static int __devinit tcc_ts_probe(struct platform_device *pdev)
{
	struct input_dev *input_dev;
	static struct tcc_ts *ts;
	//struct resource *res;
	struct tcc_adc_ts_platform_data *pdata;
	int err = -ENOMEM;

	if (!pdev || !pdev->dev.platform_data) {
		printk(KERN_ALERT "tcc_ts: missing platform data\n");
		return -1;
	}
	pdata = pdev->dev.platform_data;

	ts_base = (void __iomem *)tcc_p2v(HwTSADC_BASE);

	if (ts_base == NULL) {
		printk(KERN_ERR "%s : failed ioremap()\n", __func__);
		return -ENOMEM;
	}

	tca_adc_portinitialize((unsigned int)tcc_p2v(HwGPIO_BASE), NULL);
	tca_adc_adcinitialize((unsigned int)ts_base, NULL);

	ts = kzalloc(sizeof(struct tcc_ts), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!ts || !input_dev) {
		err = -ENOMEM;
		goto fail1;
	}

	platform_set_drvdata(pdev, ts);
	ts->client = tcc_adc_register(pdev, tcc_touch_select, (void *)tcc_touch_convert, 0);

	if(IS_ERR(ts->client)) {
		err = -ENOMEM;
		printk("%s, ERROR!\n", __func__);
		goto fail1;
	}

	ts->running = 0;
	ts->opened = 1;
	ts->pen_status = PEN_RELEASE;

	ts->dev = input_dev;
	ts->pdata = pdata;

	sprintf(ts->phys, "touchscreen");

	ts->dev->name = tcc_ts_name;
	ts->dev->phys = ts->phys;
	ts->dev->id.bustype = BUS_RS232;
	ts->dev->id.vendor = 0xDEAD;
	ts->dev->id.product = 0xBEEF;
	ts->dev->id.version = TCC_TSVERSION;
	input_dev->dev.parent = &pdev->dev;

	//tcc_adc_start(client, TOUCH_CH, 0);

	ts->dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
	ts->dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	ts->dev->absbit[0] = BIT(ABS_X) | BIT(ABS_Y) | BIT(ABS_PRESSURE); 						// absolute coor (x,y)

	input_set_abs_params(ts->dev, ABS_X, pdata->min_x, pdata->max_x, 0, 0);
	input_set_abs_params(ts->dev, ABS_Y, pdata->min_y, pdata->max_y, 0, 0);
	input_set_abs_params(ts->dev, ABS_PRESSURE, 0, TOUCH_MAX_PRESSURE, 0, 0);

	INIT_WORK(&(ts->work_q), ts_fetch_thread);
	mutex_init(&(ts->mutex));

	init_timer(&ts->ts_timer);
	ts->ts_timer.data = (unsigned long)ts;
	ts->ts_timer.function = ts_timer_handler;

	/* Get irqs */
	tcc_gpio_config_ext_intr(IRQ_NO, EXTINT_TSUPDOWN);
	if (request_irq(IRQ_NO, tcc_ts_interrupt, IRQ_TYPE_EDGE_FALLING|IRQF_DISABLED, "tcc_ts", ts)) {
		printk(KERN_ERR "tcc_ts.c: Could not allocate ts IRQ_NO !\n");
		err = -EBUSY;
		goto fail1;
	}

	tca_tch_poweroff();

	printk(KERN_INFO "%s got loaded successfully.\n", tcc_ts_name);

	ts->early_suspend.suspend = tcc_ts_early_suspend;
	ts->early_suspend.resume = tcc_ts_late_resume;
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_STOP_DRAWING;
	register_early_suspend(&ts->early_suspend);
	/* All went ok, so register to the input system */
	err = input_register_device(ts->dev);
	if (err)
		goto fail2;

	return 0;

fail2:
	free_irq(IRQ_NO, ts);
fail1:
	input_free_device(input_dev);
	if (ts) {
		tcc_adc_release(ts->client);
		kfree(ts);
	}
	ts = NULL;
	return err;
}

static int tcc_ts_remove(struct platform_device *pdev)
{
	struct tcc_ts *ts_data = platform_get_drvdata(pdev);
	printk(KERN_INFO "tcc_ts_remove() of TS called !\n");

	dbg("(remove)disable_irq_nosync\n");
	disable_irq_nosync(IRQ_NO);
	del_timer_sync(&ts_data->ts_timer);
	flush_scheduled_work();

	if (ts_data) {
		ts_data->opened = 0;
		tcc_adc_release(ts_data->client);
		free_irq(IRQ_NO, ts_data->dev);
		input_unregister_device(ts_data->dev);
		kfree(ts_data);
	}
	platform_set_drvdata(pdev, NULL);
	return 0;
}

#ifdef CONFIG_PM
static int tcc_ts_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct tcc_ts *ts_data = platform_get_drvdata(pdev);
#if defined(CONFIG_ARCH_TCC892X) 
    	extIntSEL = *(volatile unsigned long *)tcc_p2v(0x74200240);
#endif
	if (ts_data) {
		del_timer_sync(&ts_data->ts_timer);
	   	tcc_adc_release(ts_data->client);
		ts_data->client = NULL;
	}
	return 0;
}

static int tcc_ts_resume(struct platform_device *pdev)
{
	struct tcc_ts *ts_data = platform_get_drvdata(pdev);
#if defined(CONFIG_ARCH_TCC892X) 
    	*(volatile unsigned long *)tcc_p2v(0x74200240) &= 0xff00ffff;
    	*(volatile unsigned long *)tcc_p2v(0x74200240) |= (extIntSEL & 0x00ff0000);
#endif
	early_check = 0;
	ts_data->running = 0;
	tcc_adc_touch_status_f = 0;
	ts_data->pen_status = PEN_RELEASE;

	if (ts_data) {
    	ts_data->client = tcc_adc_register(pdev, tcc_touch_select, (void *)tcc_touch_convert, 0);
	}
	return 0;
}
#else
#define tcc_ts_suspend NULL
#define tcc_ts_resume  NULL
#endif

static struct platform_driver tcc_ts_driver = {
       .probe          = tcc_ts_probe,
       .remove         = tcc_ts_remove,
       .suspend        = tcc_ts_suspend,
       .resume         = tcc_ts_resume,
       .driver		= {
		.owner	= THIS_MODULE,
		.name	= "tcc-ts",
	},
};

static char banner[] __initdata = KERN_INFO "Telechips ADC Touchscreen driver, (c) 2009 Telechips\n";

static int __init tcc_ts_init(void)
{
	printk(banner);
	return platform_driver_register(&tcc_ts_driver);
}

static void __exit tcc_ts_exit(void)
{
	platform_driver_unregister(&tcc_ts_driver);
}

module_init(tcc_ts_init);
module_exit(tcc_ts_exit);

MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("Telechips ADC Touchscreen driver");
MODULE_LICENSE("GPL");
