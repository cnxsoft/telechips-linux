/*
 * linux/drivers/serial/tcc_ts_tsc.c
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
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/device.h>

#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/clk.h>

#include <linux/slab.h>
#include <linux/irq.h>

#include <linux/spinlock.h>

#include <asm/mach-types.h>

#include <mach/hardware.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <asm/io.h>
#include <mach/bsp.h>



#include "tcc_ts_tsc.h"


#ifdef CONFIG_I2C
#include <linux/i2c.h>
#endif

// =============================================================
// tca_tchhwctl.c
#if defined(CONFIG_ARCH_TCC92XX)
#define TSC_EINT_PIN		Hw4
#define TSC_EINT_PORT_EN	HwGPIOA->GPEN
#define TSC_EINT_PORT_DAT	HwGPIOA->GPDAT
#define TSC_EINT_SEL		HwEINTSEL2_EINT11(SEL_GPIOA4)

#define TSC_EINT_SEL_MASK	HwEINTSEL2_EINT11_MASK
#define TSC_EINT_SEL_ID 	HwINT0_EI11
#endif

#if defined(CONFIG_MACH_TCC9300) 
#define TSC_EINT_PIN		Hw5
#define TSC_EINT_PORT_EN		((PGPION)tcc_p2v(HwGPIOA_BASE))->GPEN
#define TSC_EINT_PORT_DAT	((PGPION)tcc_p2v(HwGPIOA_BASE))->GPDAT
#define TSC_EINT_SEL		HwEINTSEL2_EINT11(SEL_GPIOA5)

#define TSC_EINT_SEL_MASK	HwEINTSEL2_EINT11_MASK
#define TSC_EINT_SEL_ID 	HwINT0_EI11
#elif defined(CONFIG_MACH_TCC9300CM)
#define TSC_EINT_PIN		Hw4
#define TSC_EINT_PORT_EN		((PGPION)tcc_p2v(HwGPIOA_BASE))->GPEN
#define TSC_EINT_PORT_DAT	((PGPION)tcc_p2v(HwGPIOA_BASE))->GPDAT
#define TSC_EINT_SEL		HwEINTSEL2_EINT11(SEL_GPIOA4)

#define TSC_EINT_SEL_MASK	HwEINTSEL2_EINT11_MASK
#define TSC_EINT_SEL_ID 	HwINT0_EI11
#endif
// =============================================================





// =============================================================
// tca_tsc2003tchctl.c
#define SC2003_CMD_MEASURE_X	(12<<4)
#define SC2003_CMD_MEASURE_Y	(13<<4)

#define SC2003_CMD_MEASURE_Z1	(14<<4)
#define SC2003_CMD_MEASURE_Z2	(15<<4)

#define SC2003_CMD_XY_OFF		(0<<4)

#define SC2003_CMD_POWER_0		(0<<2)
#define SC2003_CMD_POWER_1		(1<<2)
#define SC2003_CMD_POWER_2		(2<<2)
#define SC2003_CMD_POWER_3		(3<<2)
// =============================================================



// =============================================================
// tca_tchcontrol.c


#define MAX_X  	3870
#define MIN_X  	170
#define MAX_Y 	3800
#define MIN_Y  	290


//for the digi_int_handle
#if defined(_TCC80x_)
#define TOUCH_COLLECT_NR		 6  // (TOUCH_COLLECT_NR-TOUCH_VALID_VALUE) must be even
#define TOUCH_VALID_VALUE		 2  // among COLLECT_NR samples, OK should be over 
#define TOUCH_PRESSURE_LIMIT  4000//	8000
#else
#define TOUCH_COLLECT_NR		6  // 4 // (TOUCH_COLLECT_NR-TOUCH_VALID_VALUE) must be even
#define TOUCH_VALID_VALUE		2  // among COLLECT_NR samples, OK should be over 
#define TOUCH_PRESSURE_LIMIT	8000
#endif


#define TCA_CAL_TOUCH_POS
#if defined(TCA_CAL_TOUCH_POS)
#define	TCA_ABS(x,y)	(((x)>(y))?(x)-(y):(y)-(x))
unsigned int tca_old_pos_x[3], tca_old_pos_y[3];
unsigned int tca_old_pos_idx;
#endif
// =============================================================



//==============================================================
#define DEVICE_NAME	"tcc-tsc2003-ts"

#define CMD_SLEEP_NO		0x80
#define CMD_ADC_ON		0x04

#define PEN_DOWN 1
#define PEN_RELEASE 0

#define READ_PENIRQ() (HwGPIOA->GPDAT & Hw4)

#if defined(CONFIG_FB_TCC_HVGA_LMS350DF01)
#define X_AXIS_MIN	10
#define X_AXIS_MAX	2000
#define Y_AXIS_MIN	10
#define Y_AXIS_MAX	2000
#elif defined(CONFIG_FB_TCC_WVGA_LMS480KF)|| defined(CONFIG_FB_TCC_WVGA_LB070WV6)||defined(CONFIG_FB_TCC_WVGA_AT070TN)
#define X_AXIS_MIN	10
#define X_AXIS_MAX	2000
#define Y_AXIS_MIN	140
#define Y_AXIS_MAX	1887
#elif defined(CONFIG_FB_TCC_XGA_CLAA104XA01CW)
#define X_AXIS_MIN	10
#define X_AXIS_MAX	2000
#define Y_AXIS_MIN	140
#define Y_AXIS_MAX	1887
#elif defined(CONFIG_FB_TCC_WXGA_HT121WX2)
#define X_AXIS_MIN	10
#define X_AXIS_MAX	2000
#define Y_AXIS_MIN	140
#define Y_AXIS_MAX	1887
#else
#define X_AXIS_MIN	10
#define X_AXIS_MAX	2000
#define Y_AXIS_MIN	140
#define Y_AXIS_MAX	1887
#endif



struct ts_event {
	unsigned short pressure;
	unsigned short x;
	unsigned short y;
};



struct tsc2003_ts {
		uint16_t addr;
		struct i2c_client *client;
		struct input_dev *input;
		int use_irq;
    unsigned int x, y, pressure, valid;
    int pen_status, opened, running;
    struct mutex mutex;
		struct work_struct work_q;
#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
#endif
		struct clk *clk;
};		


struct tsc2003_ts *tsc2003_ts;
static struct timer_list ts_timer;
static int irq_no;




static struct i2c_client *tsc2003_i2c_client;



static void tcc92xx_tsc_early_suspend(struct early_suspend *h);
static void tcc92xx_tsc_late_resume(struct early_suspend *h);


#if 0
static void TOUCH_SEND_CMD(unsigned char reg, unsigned char val)
{
	unsigned char cmd[1];
	cmd[0] = val;
	i2c_master_send(tsc2003_i2c_client, cmd, 1);
}
#endif

static void TOUCH_READ_DAT(unsigned char reg, unsigned int val)
{
	unsigned char *buf;
	buf = (unsigned char *)val;
	i2c_master_recv(tsc2003_i2c_client, buf, 2);
}


void tsc2003_enableirq(void)
{
  if(machine_is_tcc8900() || machine_is_tcc9200())
		HwPIC->INTMSK0 |= TSC_EINT_SEL_ID;

  if(machine_is_tcc9300())
		((PPIC)tcc_p2v(HwPIC_BASE))->INTMSK0 |= TSC_EINT_SEL_ID;
}

void tsc2003_disableirq(void)
{
  if(machine_is_tcc8900() || machine_is_tcc9200())
		HwPIC->INTMSK0 &= ~TSC_EINT_SEL_ID;

  if(machine_is_tcc9300())
		((PPIC)tcc_p2v(HwPIC_BASE))->INTMSK0 &= ~TSC_EINT_SEL_ID;
}


unsigned int tca_tchhw_getirqnum(void)
{
  if(machine_is_tcc8900() || machine_is_tcc9200())
		return INT_EI11;

  if(machine_is_tcc9300())
		return INT_EI11;

}


void tsc2003_port_init(void)
{
	if(machine_is_tcc8900() || machine_is_tcc9200() || machine_is_tcc9200s()) 
	{
	//power control
	BITCSET(HwGPIOE->GPFN3, HwPORTCFG_GPFN7_MASK, HwPORTCFG_GPFN7(0));
	HwGPIOE->GPEN	|= Hw31;
	HwGPIOE->GPDAT	|= Hw31;	

	// Interrupt Control
	BITCLR(TSC_EINT_PORT_EN, TSC_EINT_PIN);  //Input mode
	BITCSET(HwEINTSEL->EINTSEL2, TSC_EINT_SEL_MASK, TSC_EINT_SEL);

	HwPIC->INTMSK0 |= TSC_EINT_SEL_ID;	   

	
	HwPIC->IEN0 	&= ~(TSC_EINT_SEL_ID);						/* disable I2C */
	HwPIC->MODEA0	&= ~(TSC_EINT_SEL_ID) ; 					/* signle edge */

	HwPIC->MODE0	&= ~(TSC_EINT_SEL_ID);						/* set edge trigger mode */	

	HwPIC->POL0 	|= (TSC_EINT_SEL_ID);						/* active-high */
	HwPIC->CLR0 	=  TSC_EINT_SEL_ID; 						/* clear pending status */
	HwPIC->IEN0 	|= (TSC_EINT_SEL_ID);						/* Enable Interrupt */
	}

  if(machine_is_tcc9300() /*|| machine_is_tcc9300cm()*/)
  {
	BITCLR((TSC_EINT_PORT_EN) , TSC_EINT_PIN);  //Input mode
	BITCSET(((PGPIOINT)tcc_p2v(HwEINTSEL_BASE))->EINTSEL2, TSC_EINT_SEL_MASK, TSC_EINT_SEL);

	((PPIC)tcc_p2v(HwPIC_BASE))->INTMSK0 |= TSC_EINT_SEL_ID;	   

	
	((PPIC)tcc_p2v(HwPIC_BASE))->IEN0 	&= ~(TSC_EINT_SEL_ID);						/* disable I2C */
	((PPIC)tcc_p2v(HwPIC_BASE))->MODEA0	&= ~(TSC_EINT_SEL_ID) ; 					/* signle edge */

	((PPIC)tcc_p2v(HwPIC_BASE))->MODE0	&= ~(TSC_EINT_SEL_ID);						/* set edge trigger mode */	

	((PPIC)tcc_p2v(HwPIC_BASE))->POL0 	|= (TSC_EINT_SEL_ID);						/* active-high */
	((PPIC)tcc_p2v(HwPIC_BASE))->CLR0 	=  TSC_EINT_SEL_ID; 						/* clear pending status */
	((PPIC)tcc_p2v(HwPIC_BASE))->IEN0 	|= (TSC_EINT_SEL_ID);						/* Enable Interrupt */
   }

	tsc2003_enableirq();		

}






void tsc2003_touchbublesort(unsigned int Number[],unsigned int num) 
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

int tsc2003_getrawdata2(int * x, int * y)
{
	int i;
	unsigned int nDatax, nDatay,nDataz1,nDataz2;
	for(i=0; i < 1; i++)
	{
		nDatax = tsc2003_read_i2c(READ_X);
		nDatay = tsc2003_read_i2c(READ_Y);
		nDataz1 = tsc2003_read_i2c(READ_Z1);

		if(nDataz1!=0)
		{
			nDataz2 = tsc2003_read_i2c(READ_Z2);
			if((nDatax*nDataz2/nDataz1-nDatax)<TOUCH_PRESSURE_LIMIT)
			{
				*x = nDatax;
				*y = nDatay;
				return 0;
			}
		}
		else
		{
		}
			
	}	
	return -1;
}

unsigned int tca_touch_getvaliddata(unsigned int Number[], unsigned int num)
{
	unsigned int CurrNum, CurrCnt, OldNum, OldCnt;
	int i, InvalidCnt;

	OldNum=OldCnt=0;
	InvalidCnt = 0;
	CurrNum = Number[0];
	CurrCnt = 1;
	
	for (i=1 ; i<num ; i++)
	{
		if (CurrNum != Number[i])
		{
			if (OldCnt < CurrCnt)
			{
				OldCnt = CurrCnt;
				OldNum = CurrNum;
				InvalidCnt = 0;
			}
			else if (OldCnt == CurrCnt)
			{
				InvalidCnt++;
			}
			CurrNum = Number[i];
			CurrCnt = 1;
		}
		else
			CurrCnt++;
	}

	if (OldCnt < CurrCnt)
	{
		OldCnt = CurrCnt;
		OldNum = CurrNum;
		InvalidCnt = 0;
	}
	else if (OldCnt == CurrCnt)
	{
		InvalidCnt++;
	}

	if (InvalidCnt)
		return 0;

	return OldNum;
}


int tsc2003_getrawdata(int * x, int * y)
{
	unsigned int  i, ValidNum;
	unsigned int r_x[TOUCH_COLLECT_NR], r_y[TOUCH_COLLECT_NR];
	unsigned long  x_tol, y_tol;
	unsigned int m_pos_x, m_pos_y;
	unsigned int z1,z2;
	unsigned int index;
	
	m_pos_x = m_pos_y = 0;
	ValidNum = 0; 
	for(i = 0; i < TOUCH_COLLECT_NR; i ++)
	{
		r_x[ValidNum] = tsc2003_read_i2c(READ_X);
		r_y[ValidNum] = tsc2003_read_i2c(READ_Y);

//		printk("x = %d , y = %d  in getrawdata\n", r_x[ValidNum] , r_y[ValidNum]);

		if((r_x[ValidNum] != 0)&&(r_y[ValidNum] != 0)) 
		{		
			z1 = tsc2003_read_i2c(READ_Z1);
			if(z1!=0)
			{
				z2 = tsc2003_read_i2c(READ_Z2);
				if((r_x[ValidNum]*z2/z1-r_x[ValidNum])<TOUCH_PRESSURE_LIMIT)
					ValidNum++;
			}				
		}
		
	}
	if(ValidNum<TOUCH_VALID_VALUE)
	{
		goto digi_int_exit;
	}
    
	tsc2003_touchbublesort(r_x,ValidNum);	
	tsc2003_touchbublesort(r_y,ValidNum);

	x_tol = y_tol = 0; //sum the coordinate values
	m_pos_x = tca_touch_getvaliddata(r_x, ValidNum);
	m_pos_y = tca_touch_getvaliddata(r_y, ValidNum);
	if (!m_pos_x || !m_pos_y)
	{
		index = (ValidNum-TOUCH_VALID_VALUE)>>1;
		
		for(i=index;i<(index+TOUCH_VALID_VALUE);++i)
		{
			x_tol += r_x[i];
			y_tol += r_y[i];		
		}
		m_pos_x = x_tol/TOUCH_VALID_VALUE;
		m_pos_y = y_tol/TOUCH_VALID_VALUE;
	}

#if defined(TCA_CAL_TOUCH_POS)
	// 1. 현재 읽은 좌표가 1st_Ago 좌표와 다르다.
	if ((tca_old_pos_x[tca_old_pos_idx]!= m_pos_x) || (tca_old_pos_y[tca_old_pos_idx]!= m_pos_y))
	{
		// 2. 현재 읽은 좌표가 1st_Ago 좌표와의 차이가 16보다 작다.
		if ((TCA_ABS(tca_old_pos_x[tca_old_pos_idx], m_pos_x) <= 16) && (TCA_ABS(tca_old_pos_y[tca_old_pos_idx], m_pos_y) <= 16))
		{
			// 3. 1st_Ago 와 2nd_Ago 좌표가 같다.
			if ((tca_old_pos_x[tca_old_pos_idx] == tca_old_pos_x[(tca_old_pos_idx+2)%3]) &&
				(tca_old_pos_y[tca_old_pos_idx] == tca_old_pos_y[(tca_old_pos_idx+2)%3]))
			{
				*x = tca_old_pos_x[(tca_old_pos_idx+1)%3];
				*y = tca_old_pos_y[(tca_old_pos_idx+1)%3];

				tca_old_pos_idx++;
				if (tca_old_pos_idx > 2)
					tca_old_pos_idx = 0;

				tca_old_pos_x[tca_old_pos_idx] = m_pos_x;
				tca_old_pos_y[tca_old_pos_idx] = m_pos_y;
	
				if(*x && *y)
				{
					return 0;
				}
				else
				{
					return -1;
				}
			}
		}
	}

	tca_old_pos_idx++;
	if (tca_old_pos_idx > 2)
		tca_old_pos_idx = 0;

	tca_old_pos_x[tca_old_pos_idx] = m_pos_x;
	tca_old_pos_y[tca_old_pos_idx] = m_pos_y;
#endif

	//printk("ValidNum : %d, StartIdx : %d\n", ValidNum, index);
	//printk("(%d): %04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d\n", m_pos_x, r_x[0],r_x[1],r_x[2],r_x[3],r_x[4],r_x[5],r_x[6],r_x[7]);
	//printk("(%d): %04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d\n", m_pos_y, r_y[0],r_y[1],r_y[2],r_y[3],r_y[4],r_y[5],r_y[6],r_y[7]);
    
digi_int_exit:
	*x = m_pos_x;
	*y = m_pos_y;
	if(m_pos_x && m_pos_y)
	{
		return 0;
	}
	else
	{
		return -1;
	}


 }

void tsc2003_setreadcmd(int channel, unsigned char *nBfrTXD)
{	
	if(channel == READ_X)		
		nBfrTXD[0] = SC2003_CMD_MEASURE_X | SC2003_CMD_POWER_3;	
	else if(channel == READ_Y)		
		nBfrTXD[0] = SC2003_CMD_MEASURE_Y | SC2003_CMD_POWER_3;	
	else if(channel == READ_Z1)		
		nBfrTXD[0] = SC2003_CMD_MEASURE_Z1 | SC2003_CMD_POWER_3;	
	else if(channel == READ_Z2)		
		nBfrTXD[0] = SC2003_CMD_MEASURE_Z2 | SC2003_CMD_POWER_3;
}




void tca_rawtoscreen(int *x, int *y, unsigned int iScreenWidth, unsigned int iScreenHeight)
{
    unsigned int posX, posY;
    

	if(*x>MAX_X)
		*x=MAX_X;
	else if(*x<MIN_X)
		*x=MIN_X;

	if(*y>MAX_Y)
		*y=MAX_Y;
	else if(*y<MIN_Y)
		*y=MIN_Y;

	posX = (MAX_X-*x)*iScreenWidth/(MAX_X-MIN_X);
	posY = (*y-MIN_Y)*iScreenHeight/(MAX_Y-MIN_Y);

    *x = posX<<2;
    *y = posY<<2;
}



void tsc2003_poweroff(void)
{	
	 i2c_smbus_read_byte_data(tsc2003_i2c_client , SC2003_CMD_XY_OFF | SC2003_CMD_POWER_0);
}

short tsc2003_read_i2c(int channel)
{
	short value;
	unsigned char cmd_buf[2],data[]={0,0};
	unsigned char *buf;
	int cmd;
	
	tsc2003_setreadcmd(channel,cmd_buf);

	cmd = (int)cmd_buf[0];
	cmd |= (CMD_SLEEP_NO | CMD_ADC_ON);

	data[0] = i2c_smbus_read_byte_data(tsc2003_i2c_client , cmd);

	value = (data[0] & 0x00ff) << 4;
	value |= ((data[1] & 0x00ff) >> 4);
	value &= 0xfff;

#ifndef CONFIG_I2C
	udelay(1);
#endif
	return(value>>1);

}

	


static inline void tsc_pen_release(struct tsc2003_ts *ts_data, struct input_dev *dev)
{
    if (ts_data->pen_status != PEN_RELEASE) {
//        printk("PENUP\n");
        ts_data->pen_status = PEN_RELEASE;

        input_report_key(dev, BTN_TOUCH, PEN_RELEASE);
        input_sync(dev);


    }
}

static inline void tsc_pen_pressure(struct tsc2003_ts *ts_data, struct input_dev *dev)
{
    ts_data->pen_status = PEN_DOWN;

    input_report_key(dev, BTN_TOUCH, PEN_DOWN);

#if defined(CONFIG_FB_TCC7901_DEMOBOARD)
        input_report_abs(dev, ABS_X, X_AXIS_MAX - ts_data->x);
        input_report_abs(dev, ABS_Y, ts_data->y);
#elif defined(CONFIG_FB_TCC7901_LMS350DF01)
        input_report_abs(dev, ABS_X, Y_AXIS_MAX - ts_data->y);
        input_report_abs(dev, ABS_Y, X_AXIS_MAX - ts_data->x);
#elif defined(CONFIG_ARCH_TCC92XX)
#if defined(CONFIG_FB_TCC_HVGA_LMS350DF01)		
		input_report_abs(dev, ABS_X, Y_AXIS_MAX - ts_data->y);
		input_report_abs(dev, ABS_Y, X_AXIS_MAX - ts_data->x);
#elif defined(CONFIG_FB_TCC_WVGA_LMS480KF) || defined(CONFIG_FB_TCC_WVGA_LB070WV6)|| defined(CONFIG_FB_TCC_WVGA_AT070TN)
		input_report_abs(dev, ABS_X, X_AXIS_MAX - ts_data->x);
		input_report_abs(dev, ABS_Y, ts_data->y);	
#elif defined(CONFIG_FB_TCC_XGA_CLAA104XA01CW)		
		input_report_abs(dev, ABS_X, X_AXIS_MAX - ts_data->x);
		input_report_abs(dev, ABS_Y, ts_data->y);	
#elif defined(CONFIG_FB_TCC_WXGA_HT121WX2)		
		input_report_abs(dev, ABS_X, X_AXIS_MAX - ts_data->x);
		input_report_abs(dev, ABS_Y, ts_data->y);	
#else
		input_report_abs(dev, ABS_X, X_AXIS_MAX - ts_data->x);
		input_report_abs(dev, ABS_Y, ts_data->y);	
#endif		
#endif 
    input_report_key(dev, ABS_PRESSURE, 1);
    input_sync(dev);
	msleep_interruptible(1);   //MCC
	//printk("PEN DOWN %u, %u, %u\n", ts_data->x, ts_data->y, ts_data->pressure);
}


static void ts_fetch_thread(struct work_struct *work)
{
    struct tsc2003_ts* ts_data = container_of(work, struct tsc2003_ts, work_q);
    struct input_dev *dev = ts_data->input;
    int flag, valid;
   
    ts_data->running = 1;

    tsc2003_disableirq();
    if(READ_PENIRQ()){
        tsc_pen_release(ts_data, dev);
        ts_data->running = 0;
        tsc2003_enableirq();
    } else {

        flag = tsc2003_getrawdata(&(ts_data->x), &(ts_data->y));
        tsc2003_poweroff();

        valid = ts_data->x | ts_data->y;
        if ((flag == 0) && valid) {
            tsc_pen_pressure(ts_data, dev);
            ts_timer.expires = jiffies + msecs_to_jiffies(50);	  // It is adjustable. //MCC
            add_timer(&ts_timer);
        } else {
            tsc_pen_release(ts_data, dev);
            ts_data->running = 0;
            tsc2003_enableirq();
        }
    }

}


static irqreturn_t tsc2003_irq_handler(int irq, void *dev_id)
{
    struct tsc2003_ts *ts_data = (struct tsc2003_ts *)dev_id;

    if (ts_data->running == 0) {
        ts_data->running = 1;
        ts_timer.expires = jiffies; //+ msecs_to_jiffies(50);		 // It is adjustable. //MCC
        add_timer(&ts_timer);
    }

	return IRQ_HANDLED;
}

static void ts_timer_handler(unsigned long data)
{
    struct tsc2003_ts *ts = (struct tsc2003_ts *) data;
	if(ts->opened){
        if (schedule_work(&(ts->work_q)) == 0) {
            printk("cannot schedule work !!!\n");
            ts->running = 0;
        }
    }else {
        tsc2003_disableirq();
    }
}


static int tsc2003_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct tsc2003_ts *ts;
	struct input_dev *input_dev;
	struct clk *clk;
	int err = -ENOMEM;
	int ret;

	
	printk("%s\n", __func__);
	
	tsc2003_port_init();
	
	ts = kzalloc(sizeof(struct tsc2003_ts), GFP_KERNEL);
	if(ts == NULL)
		return -ENOMEM;
	
	
	ts->client = client;
	tsc2003_i2c_client = client;
	i2c_set_clientdata(client, ts);
	
	
	input_dev = input_allocate_device();
	if (input_dev == NULL) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate input device\n", __func__);
		goto fail1;
	}
			
	clk = clk_get(NULL, "i2c");
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
	input_dev->id.version = 0x0001;
	
	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input_set_abs_params(input_dev, ABS_X, X_AXIS_MIN, X_AXIS_MAX, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, Y_AXIS_MIN, Y_AXIS_MAX, 0, 0);
	/* TODO */
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 0, 0, 0);

	INIT_WORK(&(ts->work_q), ts_fetch_thread);
	//init_waitqueue_head(&(tsc2003_ts->wait_q));
	mutex_init(&(ts->mutex));

	init_timer(&ts_timer);
	ts_timer.data = (unsigned long) ts;
	ts_timer.function = ts_timer_handler;
	
	ret = input_register_device(ts->input);
		if (ret) {
		printk(KERN_ERR "%s: Unable to register %s input device\n", __func__, ts->input->name);
		goto fail1;
	}

	
	if((err = request_irq(tca_tchhw_getirqnum(), tsc2003_irq_handler, IRQF_DISABLED , DEVICE_NAME, ts))){
			err = -EBUSY;
			goto fail2;
	} else {
			ts->use_irq = 1;
	}

	tsc2003_poweroff();
	
#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.suspend = tcc92xx_tsc_early_suspend;
	ts->early_suspend.resume = tcc92xx_tsc_late_resume;
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	register_early_suspend(&ts->early_suspend);
#endif	
			
   printk("tsc2003 driver is successfully loaded\n");
	return 0;

fail2: free_irq(irq_no, tsc2003_ts);
fail1: input_free_device(input_dev);
       kfree(tsc2003_ts);
       return err;	
	
	

}


static int tsc2003ts_remove(struct i2c_client *client)
{
    struct tsc2003_ts *ts;

    ts = i2c_get_clientdata(client);

    unregister_early_suspend(&ts->early_suspend);

    if(ts->use_irq)
    		free_irq(client->irq, ts);

    input_unregister_device(ts->input);
    clk_disable(ts->clk);
    clk_put(ts->clk);
    kfree(ts);
    return 0;
}

#ifdef CONFIG_PM
static int tsc2003ts_suspend(struct platform_device *dev, pm_message_t state)
{
#if 0
	tsc2003_disableirq();	
	tsc2003_poweroff();
#endif
    return 0;
}

static int tsc2003ts_resume(struct platform_device *dev)
{
#if 0
	tsc2003_enableirq();	
	tsc2003_poweroff();
#endif
    return 0;
}
#else
#define tsc2003ts_suspend     NULL
#define tsc2003ts_resume      NULL
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void tcc92xx_tsc_early_suspend(struct early_suspend *h)
{
	tsc2003_disableirq();	
	tsc2003_poweroff();
	//tsc2003_driver.suspend;

}
static void tcc92xx_tsc_late_resume(struct early_suspend *h)
{
	tsc2003_enableirq();	
	tsc2003_poweroff();
	//tsc2003_driver.resume;
}

#endif



static const struct i2c_device_id tsc2003_ts_id[] = {
		{ DEVICE_NAME, 0 },
		{ }
};


static struct i2c_driver tsc2003_ts_driver = {
		.probe			= tsc2003_ts_probe,
		.remove			= tsc2003ts_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
		.suspend			=	tsc2003ts_suspend,
		.resume			=	tsc2003ts_resume,
#endif
		.id_table 	= tsc2003_ts_id,
		.driver	=	{
				.name		=	DEVICE_NAME,
		},
};

static int __init tsc2003ts_init(void)
{

    int res;
    printk("Telechips TSC2003 Touch Driver\n");
    res = i2c_add_driver(&tsc2003_ts_driver);
    if(res)
    {
        printk("fail : platform driver %s (%d) \n", tsc2003_ts_driver.driver.name, res);
        return res;
    }

    return 0;
}


void __exit tsc2003ts_exit(void)
{
    i2c_del_driver(&tsc2003_ts_driver);
}

module_init(tsc2003ts_init);
module_exit(tsc2003ts_exit);

MODULE_AUTHOR("linux@telechips.com");
MODULE_DESCRIPTION("Telechips tsc2003 touch driver");
MODULE_LICENSE("GPL");

