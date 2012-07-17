/*
 * linux/drivers/rtc/rtc-tcc8800.c
 *
 * Author: <linux@telechips.com>
 * Created: Dec 7, 2011
 * Description: RTC driver for Telechips TCC Series
 *
 * Copyright (C) 2011 Telechips
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


#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/clk.h>
#include <linux/log2.h>

#include <mach/bsp.h>
#include <mach/hardware.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/mach/time.h>
#include "tcc/tca_alarm.h"

#define DRV_NAME "tcc-rtc"

static struct clk *_rtc_clk;

#if 1
#define RTCCON      0x00
#define INTCON      0x04
#define RTCALM      0x08
#define ALMSEC      0x0C
#define ALMMIN      0x10
#define ALMHOUR     0x14
#define ALMDATE     0x18
#define ALMDAY      0x1C
#define ALMMON      0x20
#define ALMYEAR     0x24
#define BCDSEC      0x28
#define BCDMIN      0x2C
#define BCDHOUR     0x30
#define BCDDATE     0x34
#define BCDDAY      0x38
#define BCDMON      0x3C
#define BCDYEAR     0x40
#define RTCIM       0x44
#define RTCPEND     0x48
#define RTCSTR		0x4C
#endif

#undef tcc_readb
#undef tcc_readw
#undef tcc_readl
#undef tcc_writeb
#undef tcc_writew
#undef tcc_writel

#define tcc_readb(a)	(*(volatile unsigned char  *)(a))
#define tcc_readw(a)	(*(volatile unsigned short *)(a))
#define tcc_readl(a)	(*(volatile unsigned int   *)(a))

#define tcc_writeb(v,a)	(*(volatile unsigned char  *)(a) = (v))
#define tcc_writew(v,a)	(*(volatile unsigned short *)(a) = (v))
#define tcc_writel(v,a)	(*(volatile unsigned int   *)(a) = (v))

static void __iomem *rtc_base;
static int tcc_rtc_alarmno = NO_IRQ;

/* IRQ Handlers */

static irqreturn_t tcc_rtc_alarmirq(int irq, void *id)
{
	printk("[tcc_rtc_alarmirq][%u]\n", irq);

	//local_irq_disable();

	tcc_writel( tcc_readl(rtc_base + RTCCON) | Hw1, rtc_base + RTCCON);	// RTC Register write enabled
	tcc_writel( tcc_readl(rtc_base + INTCON) | Hw0, rtc_base + INTCON);	// Interrupt Block Write Enable

	//tcc_writel( tcc_readl(rtc_base + RTCIM) & ~(Hw3|Hw1|Hw0), rtc_base + RTCIM); // INTMODE-DISABLE ALARM INTRRUPT & Operation mode - normal
	tcc_writel( 0x04, rtc_base + RTCIM); // INTMODE - Reset Value - 110106, hjbae

	tcc_writel( 0, rtc_base + RTCPEND); // PEND bit is cleared.

	#if 1	//for One-Shot write to RTCSTR - 110106, hjbae
	tcc_writel( tcc_readl(rtc_base + RTCSTR) | Hw6 | Hw7 , rtc_base + RTCSTR); // RTC Alarm interrupt pending clear
	#else
	tcc_writel( tcc_readl(rtc_base + RTCSTR) | Hw6 , rtc_base + RTCSTR); // RTC Alarm interrupt pending clear
	tcc_writel( tcc_readl(rtc_base + RTCSTR) | Hw7 , rtc_base + RTCSTR); // RTC wake-up interrupt pending clear
	#endif

	printk("RTCSTR[0x%x]", tcc_readl(rtc_base + RTCSTR));
	printk("RTCPEND[0x%x]", tcc_readl(rtc_base + RTCPEND));

	tcc_writel( tcc_readl(rtc_base + INTCON) & ~Hw0, rtc_base + INTCON); // Interrupt Block Write disable
	tcc_writel( tcc_readl(rtc_base + RTCCON) & ~Hw1, rtc_base + RTCCON); // RTC Register write disable

	//local_irq_enable();

	//tca_alarm_setint((unsigned int)rtc_base);	//forTEST

	rtc_update_irq(id, 1, RTC_AF | RTC_IRQF);

	return IRQ_HANDLED;
}

/* Update control registers */
static void tcc_rtc_setaie(int to)
{
	unsigned int tmp;

	printk("%s: aie=%d\n", __func__, to);

    tcc_writel( tcc_readl(rtc_base + RTCCON) | Hw1, rtc_base + RTCCON);
    tcc_writel( tcc_readl(rtc_base + INTCON) | Hw0, rtc_base + INTCON);

	tmp = tcc_readl(rtc_base + RTCALM) & ~Hw7;

	if (to)
		tmp |= Hw7;

	tcc_writel(tmp, rtc_base + RTCALM);

    //tcc_writel( tcc_readl(rtc_base + INTCON) & ~Hw0, rtc_base + INTCON);
    tcc_writel( tcc_readl(rtc_base + RTCCON) & ~Hw1, rtc_base + RTCCON);
}


/* Time read/write */
static int tcc_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
{
    rtctime pTime;

    local_irq_disable();
    tca_rtc_gettime((unsigned int)rtc_base, &pTime);

    rtc_tm->tm_sec  = pTime.wSecond;
    rtc_tm->tm_min  = pTime.wMinute;
    rtc_tm->tm_hour = pTime.wHour;
    rtc_tm->tm_mday = pTime.wDay;
		rtc_tm->tm_wday = pTime.wDayOfWeek;
    rtc_tm->tm_mon  = pTime.wMonth - 1;
    rtc_tm->tm_year = pTime.wYear - 1900;

    printk("read time %04d/%02d/%02d %02d:%02d:%02d\n",
         pTime.wYear, pTime.wMonth, pTime.wDay,
         pTime.wHour, pTime.wMinute, pTime.wSecond);

    local_irq_enable();

	return 0;
}

static int tcc_rtc_settime(struct device *dev, struct rtc_time *tm)
{
    rtctime pTime;

    local_irq_disable();

    pTime.wSecond       = tm->tm_sec;
    pTime.wMinute       = tm->tm_min;
    pTime.wHour         = tm->tm_hour;
    pTime.wDay          = tm->tm_mday;
    pTime.wDayOfWeek    = tm->tm_wday;
    pTime.wMonth        = tm->tm_mon + 1;
    pTime.wYear         = tm->tm_year + 1900;

	printk("set time %02d/%02d/%02d %02d:%02d:%02d\n",
		  pTime.wYear, pTime.wMonth, pTime.wDay,
		 pTime.wHour, pTime.wMinute,  pTime.wSecond);


    tca_rtc_settime((unsigned int)rtc_base, &pTime);

    local_irq_enable();

	return 0;
}

static int tcc_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_time *alm_tm = &alrm->time;
	unsigned int alm_en, alm_pnd;
    rtctime pTime;
    printk("%s\n", __func__);

    local_irq_disable();

    tcc_writel( tcc_readl(rtc_base + RTCCON) | Hw1, rtc_base + RTCCON);
    tcc_writel( tcc_readl(rtc_base + INTCON) | Hw0, rtc_base + INTCON);

	alm_en = tcc_readl(rtc_base + RTCALM);
	alm_pnd = tcc_readl(rtc_base + RTCPEND);

	alrm->enabled = (alm_en & Hw7) ? 1 : 0;
	alrm->pending = (alm_pnd & Hw0) ? 1 : 0;

    printk(" alrm->enabled = %d, alm_en = %d\n", alrm->enabled, alm_en);

    tcc_writel( tcc_readl(rtc_base + INTCON) & ~Hw0, rtc_base + INTCON);
    tcc_writel( tcc_readl(rtc_base + RTCCON) & ~Hw1, rtc_base + RTCCON);

    tca_alarm_gettime((unsigned int)rtc_base, &pTime);

    alm_tm->tm_sec  = pTime.wSecond;
    alm_tm->tm_min  = pTime.wMinute;
    alm_tm->tm_hour = pTime.wHour;
    alm_tm->tm_mday = pTime.wDay ;
    alm_tm->tm_mon  = pTime.wMonth - 1;
    alm_tm->tm_year = pTime.wYear - 1900;

	printk("read alarm %02x %02x/%02x/%02x %02x:%02x:%02x\n",
		 alm_en,
		 pTime.wYear, pTime.wMonth, pTime.wDay,
		 pTime.wHour, pTime.wMinute, pTime.wSecond);

    local_irq_enable();

	return 0;
}

static int tcc_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	//struct rtc_time rtc_tm;
    rtctime pTime;

	struct rtc_time *tm = &alrm->time;
    printk("%s\n", __func__);

    alrm->enabled = 1;

    #if 0 // alarm test code
	tcc_rtc_gettime(dev, tm);

	tm->tm_sec += 5;

	if( tm->tm_sec >= 60 )
	{
		tm->tm_sec %= 60;
		tm->tm_min++;
	}

	if( tm->tm_min >= 60)
	{
		tm->tm_min %= 60;
		tm->tm_hour++;
	}

	if( tm->tm_hour >= 24)
	{
		tm->tm_hour %= 24;
		tm->tm_mday++;
	}
    #endif

    pTime.wSecond   = tm->tm_sec;
    pTime.wMinute   = tm->tm_min;
    pTime.wHour     = tm->tm_hour;
    pTime.wDay      = tm->tm_mday;
    pTime.wMonth    = tm->tm_mon + 1;
    pTime.wYear     = tm->tm_year + 1900;

	printk("set alarm %02d/%02d/%02d %02d:%02d:%02d \n",
		pTime.wYear, pTime.wMonth, pTime.wDay,
		 pTime.wHour, pTime.wMinute, pTime.wSecond);

    local_irq_disable();

    tca_alarm_settime((unsigned int)rtc_base, &pTime);

    //tcc_writel( tcc_readl(rtc_base + RTCCON) & ~Hw0, rtc_base + RTCCON);
    tcc_writel( tcc_readl(rtc_base + RTCCON) | Hw1, rtc_base + RTCCON);
    tcc_writel( tcc_readl(rtc_base + INTCON) | Hw0, rtc_base + INTCON);
    //tcc_writel( tcc_readl(rtc_base + INTCON) & ~Hw15, rtc_base + INTCON);

	if (alrm->enabled) {
	    tcc_writel(tcc_readl(rtc_base + RTCALM)| Hw7, rtc_base + RTCALM);
		enable_irq_wake(tcc_rtc_alarmno);
    } else {
	    tcc_writel(tcc_readl(rtc_base + RTCALM)& ~Hw7, rtc_base + RTCALM);
		disable_irq_wake(tcc_rtc_alarmno);
    }

    tcc_writel( tcc_readl(rtc_base + INTCON) & ~Hw0, rtc_base + INTCON);
    tcc_writel( tcc_readl(rtc_base + RTCCON) & ~Hw1, rtc_base + RTCCON);

    local_irq_enable();

	return 0;
}

static int tcc_rtc_proc(struct device *dev, struct seq_file *seq)
{
	return 0;
}


static int tcc_rtc_ioctl(struct device *dev,
             unsigned int cmd, unsigned long arg)
{
    unsigned int ret = -ENOIOCTLCMD;

    switch (cmd) {
    case RTC_AIE_OFF:
        tcc_rtc_setaie(0);
        ret = 0;
        break;
    case RTC_AIE_ON:
		tcc_rtc_setaie(1);
        ret = 0;
        break;
    case RTC_PIE_OFF:
        break;
    case RTC_PIE_ON:
        break;
    case RTC_IRQP_READ:
        break;
    case RTC_IRQP_SET:
        break;
    case RTC_UIE_ON:
        break;
    case RTC_UIE_OFF:
        break;
        ret = -EINVAL;
    }

    return ret;
}


static const struct rtc_class_ops tcc_rtcops = {
	.read_time	= tcc_rtc_gettime,
	.set_time	= tcc_rtc_settime,
	.read_alarm	= tcc_rtc_getalarm,
	.set_alarm	= tcc_rtc_setalarm,
	.proc	    = tcc_rtc_proc,
    .ioctl      = tcc_rtc_ioctl,
};


static int tcc_rtc_remove(struct platform_device *dev)
{
	struct rtc_device *rtc = platform_get_drvdata(dev);

	platform_set_drvdata(dev, NULL);
	rtc_device_unregister(rtc);

	tcc_rtc_setaie(0);

    free_irq(tcc_rtc_alarmno, rtc);

	//clk_disable(_rtc_clk);

	return 0;
}

static int tcc_rtc_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;
	volatile PRTC pRTC = (volatile PRTC)tcc_p2v(HwRTC_BASE);
	int ret;
	unsigned int ulINTCON;
	int iLoop = 1000;

	rtc_base = (void __iomem *)tcc_p2v(HwRTC_BASE);

	if (rtc_base == NULL) {
		printk("failed RTC ioremap()\n");
		return -ENOMEM;
	}

	clk_enable(_rtc_clk);

	tcc_writel( tcc_readl(rtc_base + RTCCON) | Hw1, rtc_base + RTCCON); // Write Enable
	tcc_writel( tcc_readl(rtc_base + INTCON) | Hw0, rtc_base + INTCON); // Interrupt Write Enable

	//printk("FIRST INTCON[0x%x]RTCCON[0x%x]", tcc_readl(rtc_base + INTCON), tcc_readl(rtc_base + RTCCON));

	ulINTCON = tcc_readl(rtc_base + INTCON);

	while( (ulINTCON & 0x0000B700) != 0x00009000 && iLoop > 0 )	// 32.768KHz(3.3V)
//	while( (ulINTCON & 0x0000B700) != 0x00008000 && iLoop > 0 )	// 32.768KHz (1.8V)
	{
		// to clear PROT
		//   - RTCWR == 1 && INTWREN == 1 && STARTB == 1
		tcc_writel( tcc_readl(rtc_base + RTCCON) | Hw0, rtc_base + RTCCON); // RTC Start bit - Halt
		tcc_writel( tcc_readl(rtc_base + INTCON) & ~Hw15, rtc_base + INTCON); // protect bit - disable

		printk("OLD INTCON[0x%x] iLoop [%d] RTCCON[0x%x]=================================\n", ulINTCON, iLoop, tcc_readl(rtc_base + RTCCON));

		ulINTCON = tcc_readl(rtc_base + INTCON);
		ulINTCON &= ~0x00003700;  //  START XDRV[13:12] 00 - 32.768KHz (1.8V)  /  FSEL[10:8] 32.768KHz - 000
		ulINTCON |= 0x00001000;   //  START XDRV[13:12] 01 - 32.768KHz(3.3V)  /  FSEL[10:8] 32.768KHz - 000

		tcc_writel( ulINTCON, rtc_base + INTCON); // Write XDRV
		tcc_writel( tcc_readl(rtc_base + INTCON) | Hw15, rtc_base + INTCON); // protect bit - enable

		ulINTCON = tcc_readl(rtc_base + INTCON);
		printk("SET INTCON[0x%x] iLoop [%d] RTCCON[0x%x]=================================\n", ulINTCON, iLoop, tcc_readl(rtc_base + RTCCON));

		iLoop--;
	}

	if( !(tcc_readl(rtc_base + INTCON) & Hw15) )
	{
		// to clear PROT
		//   - RTCWR == 1 && INTWREN == 1 && STARTB == 1

		tcc_writel( tcc_readl(rtc_base + RTCCON) | Hw0, rtc_base + RTCCON); // RTC Start bit - Halt
		tcc_writel( tcc_readl(rtc_base + INTCON) | Hw15, rtc_base + INTCON); // protect bit - enable
	}

	//Start : Disable the RTC Alarm - 120125, hjbae
	// Disable - Wake Up Interrupt Output(Hw7), Alarm Interrupt Output(Hw6)
	tcc_writel( tcc_readl(rtc_base + RTCCON) & ~(Hw7|Hw6), rtc_base + RTCCON);
	// Disable - Alarm Control
	tcc_writel(0x00, rtc_base + RTCALM);
	// Power down mode, Active HIGH, Disable alarm interrupt
	tcc_writel(Hw3|Hw2, rtc_base + RTCIM);
	//End

	// printk("CUR INTCON[0x%x]\n", tcc_readl(rtc_base + INTCON));

	tcc_writel( tcc_readl(rtc_base + RTCCON) & ~Hw0, rtc_base + RTCCON); // RTC Start Bit - RUN
	tcc_writel( tcc_readl(rtc_base + INTCON) & ~Hw0, rtc_base + INTCON); // Interrupt Block Write Enable Bit - Disable
	tcc_writel( tcc_readl(rtc_base + RTCCON) & ~Hw1, rtc_base + RTCCON); // RTC Write Enable bit - Disable

	// Check Valid Time
	if(tca_rtc_checkvalidtime((unsigned int)pRTC))
	{
		rtctime pTime;

		// Invalied Time
		printk("RTC Invalied Time, Set 2012.01.26\n");

		// temp init;
		pTime.wDay      = 26;
		pTime.wMonth    = 1;
		pTime.wDayOfWeek= 4;
		pTime.wHour     = 2;
		pTime.wMinute   = 30;
		pTime.wSecond   = 0;
		pTime.wYear     = 2012;

		tca_rtc_settime((unsigned int)rtc_base, &pTime);
	}

	/* find the IRQs */
	tcc_rtc_alarmno = platform_get_irq(pdev, 0);
    printk("tcc_rtc: alarm irq %d\n", tcc_rtc_alarmno);

	if (tcc_rtc_alarmno < 0) {
		dev_err(&pdev->dev, "no irq for alarm\n");
		return -ENOENT;
	}

	/* register RTC and exit */
	rtc = rtc_device_register(pdev->name, &pdev->dev, &tcc_rtcops, THIS_MODULE);

	if (IS_ERR(rtc)) {
		dev_err(&pdev->dev, "cannot attach rtc\n");
		ret = PTR_ERR(rtc);
		goto err_nortc;
	}

	platform_set_drvdata(pdev, rtc);

	//Use threaded IRQ, remove IRQ enable in interrupt handler - 120126, hjbae
//    if(request_irq(tcc_rtc_alarmno, tcc_rtc_alarmirq, IRQF_DISABLED, DRV_NAME, rtc)) {
	if(request_threaded_irq(tcc_rtc_alarmno, NULL, tcc_rtc_alarmirq, IRQF_ONESHOT, DRV_NAME, rtc)) {
		printk("%s: RTC timer interrupt IRQ%d already claimed\n", pdev->name, tcc_rtc_alarmno);
		return 0;
	}

	return 0;

 err_nortc:
    rtc_device_unregister(rtc);
    return ret;
}

#ifdef CONFIG_PM

/* RTC Power management control */

static int tcc_rtc_suspend(struct platform_device *pdev, pm_message_t state)
{
    if (device_may_wakeup(&pdev->dev))
        enable_irq_wake(tcc_rtc_alarmno);
	return 0;
}

static int tcc_rtc_resume(struct platform_device *pdev)
{
    if (device_may_wakeup(&pdev->dev))
        disable_irq_wake(tcc_rtc_alarmno);
	return 0;
}
#else
#define tcc_rtc_suspend NULL
#define tcc_rtc_resume  NULL
#endif

static struct platform_driver tcc_rtc_driver = {
	.probe		= tcc_rtc_probe,
	.remove		= tcc_rtc_remove,
	.suspend	= tcc_rtc_suspend,
	.resume		= tcc_rtc_resume,
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static char __initdata banner[] = "TCC RTC, (c) 2012, Telechips \n";

static int __init tcc_rtc_init(void)
{
	printk(banner);

	_rtc_clk = clk_get(NULL, "rtc");
	BUG_ON(_rtc_clk == NULL);

	return platform_driver_register(&tcc_rtc_driver);
}

static void __exit tcc_rtc_exit(void)
{
	platform_driver_unregister(&tcc_rtc_driver);

	//clk_put(_rtc_clk);
}

module_init(tcc_rtc_init);
module_exit(tcc_rtc_exit);

MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("Telechips RTC Driver");
MODULE_LICENSE("GPL");
