/*
 * linux/drivers/serial/tcc_ts_synaptics.c
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

#include <mach/hardware.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <asm/io.h>

#ifdef CONFIG_I2C
#   include <linux/i2c.h>
#endif

#include <mach/bsp.h>

#if defined(CONFIG_ARCH_TCC92XX)
#define TSC_EINT_PIN			Hw4
#define TSC_EINT_PORT_EN		HwGPIOA->GPEN
#define TSC_EINT_PORT_PD		HwGPIOA->GPPD0
#define TSC_EINT_PORT_PD_MASK		(Hw8|Hw9)
#define TSC_EINT_PORT_PD_UP 		(Hw8)
#define TSC_EINT_PORT_PD_DOWN		(Hw9)
#define TSC_EINT_PORT_DAT		HwGPIOA->GPDAT
#define TSC_EINT_SEL			HwEINTSEL2_EINT11(SEL_GPIOA4)

#define TSC_EINT_SEL_MASK	HwEINTSEL2_EINT11_MASK
#define TSC_EINT_SEL_ID 	HwINT0_EI11
#endif

#define TOUCH_PWR_CTRL_USE
#define TOUCH_PWR_REPAIR_CNT	20
#define TOUCH_POS_READ_PERIOD	30


/*******************************************************
	Control Registers
*******************************************************/

#define TOUCHPAD_SENSITIVITY_REG			0x24 //   0x1044 Sensitivity Adjust

// SMI BUS Address
#define TOUCHPAD_DATA0_REG					0x00 //   Width, Gesture, Sensor Status
#define TOUCHPAD_DATA1_REG					0x01 //   Z
#define TOUCHPAD_DATA2_REG					0x02 //   X-Position(12:8)
#define TOUCHPAD_DATA3_REG					0x03 //   X-Position( 7:0)
#define TOUCHPAD_DATA4_REG					0x04 //   Y-Position(12:8)
#define TOUCHPAD_DATA5_REG					0x05 //   Y-Position( 7:0)
#define TOUCHPAD_DATA6_REG					0x06 //   X-Delta
#define TOUCHPAD_DATA7_REG					0x07 //   Y-Delta
#define TOUCHPAD_DATA8_REG					0x08 //   Confirmed, Zoom, Flick, Press, TapCode
#define TOUCHPAD_DATA9_REG					0x09 //   Y Flick, X Flick
#define TOUCHPAD_DATA10_REG 				0x0A // button data
#define TOUCHPAD_DEVICE_STATUS_REG			0x0B // 0x040C Device Status Register

#define TOUCHPAD_2D_CONTROL_REG 			0x21 // 0x1041 - | - | No Filter | - | Reduced Reporting | No Clip Z | - | - |
#define TOUCHPAD_BUTTON_CONTROL2_REG		0x22 // 0x1842 Func18: Button Control Reg 1 Button Sensitivity A djust
#define TOUCHPAD_LED_ENABLE_REG 			0x42 // 0x2242 Func22: LED Enable Enable 7 Enable 6 Enable 5 Enable 4 Enable 3 Enable 2 Enable 1 Enable 0
#define TOUCHPAD_RAMP1_PERIOD_REG			0x44 // 0x2244 Func22: Ramp Period A R amp Period A
#define TOUCHPAD_RAMP2_PERIOD_REG			0x45 // 0x2245 Func22: Ramp Period B R amp Period B
#define TOUCHPAD_LED0_REG					0x50 // 0x2250 Func22: LED#0 Pattern Brightness
#define TOUCHPAD_LED1_REG					0x51 // 0x2251 Func22: LED#1 Pattern Brightness
#define TOUCHPAD_LED2_REG					0x52 // 0x2252 Func22: LED#2 Pattern Brightness
#define TOUCHPAD_LED3_REG					0x53 // 0x2253 Func22: LED#3 Pattern Brightness
#define TOUCHPAD_LED4_REG					0x54 // 0x2254 Func22: LED#4 Pattern Brightness
#define TOUCHPAD_LED5_REG					0x55 // 0x2255 Func22: LED#5 Pattern Brightness
#define TOUCHPAD_LED6_REG					0x56 // 0x2256 Func22: LED#6 Pattern Brightness
#define TOUCHPAD_LED7_REG					0x57 // 0x2257 Func22: LED#7 Pattern Brightness

#define TOUCHPAD_RMI_VER_REG				0xE0 // 0x0200 RMI Protocol Version ( RMI Protocol Major Version | RMI ProtocolMinor Version ) Default $01
#define TOUCHPAD_MANUFACTURE_ID_REG 		0xE1 // 0X0201 Manufacturer ID
#define TOUCHPAD_INTERFACE_VER_REG			0xE2 // 0x0202 Physical Interface ( Version Physical Interface Major Version Physical Interface Minor Version $01
#define TOUCHPAD_MAJOR_VER_REG				0xE4 // 0x0204 Product Major Version Major Ver sion (Produc t Family)
#define TOUCHPAD_MINOR_VER_REG				0xE5 // 0x0205 Product Minor Version Minor Vers ion (Product Number)
#define TOUCHPAD_PRODUCT_INFO2_REG			0xE6 // 0x0206 Product Info 2
#define TOUCHPAD_PRODUCT_INFO3_REG			0xE7 // 0x0207 Product Info 3

#define TOUCHPAD_DEVICE_CONTROL_REG 		0xF0 // 0x0000 Device Control Register Report Rate . . . Sleep Mode
#define TOUCHPAD_INTERRUPT_ENABLE_REG		0xF1 // 0x0001 Interrupt Enable Control 			 Register . . . . . . . F18 Button
#define TOUCHPAD_ERROR_STATUS_REG			0xF2 // 0x0002 Error Status Register Error C ode
#define TOUCHPAD_INTERRUPT_STATUS_REG		0xF3 // 0x0003 Interrupt Request Status 			 Register . . . . . . . F18 Button
#define TOUCHPAD_DEVICE_COMMAND_REG 		0xF4 // 0x0004 Device Command Register . . . . . . Rezero Reset
#define TOUCHPAD_PAGE_SELECT_REG			0xFF // 0xxxFF Page Select Register . High 7 bits of 15-Bit RMI Page Address



/*******************************************************
	Register Values
*******************************************************/

#define TOUCHPAD_DEVICE_COMMAND_RESET		0x01
#define TOUCHPAD_RESET_CAUSE				0x87


/* Only for test. sleep mode control is required */
#define TOUCHPAD_DEVICE_CONTROL_DEFAULT 	0x80 // report rate 80/sec, always awaken.	// should be proper value for Power Management.
#if defined (LGE_CRYSTAL_VIEWTY2_HW) //FW_NOHYONGGYUN_20090410 :: PINCH_TEST
#define TOUCHPAD_DEVICE_CONTROL_DEEP_SLEEP	0x84 // report rate 80/sec, deep sleep. 	// should be proper value for Power Management.
#else
#define TOUCHPAD_DEVICE_CONTROL_DEEP_SLEEP	0x85 // report rate 80/sec, deep sleep. 	// should be proper value for Power Management.
#endif
#define TOUCHPAD_2D_CONTROL_DEFAULT 		0x08  // Filter, Reduced Reporting, Clip Z

#define TOUCHPAD_X_POS_MAX					5184
#define TOUCHPAD_Y_POS_MAX					3118


typedef enum
{
	TOUCHPAD_NO_FINGER = 0,
	TOUCHPAD_ONE_FINGER = 1,
	TOUCHPAD_TWO_FINGER = 2,
	TOUCHPAD_THREE_MORE_FINGER = 3,
	TOUCHPAD_RESERVED1 = 4,
	TOUCHPAD_RESERVED2 = 5,
	TOUCHPAD_RESERVED3 = 6,
	TOUCHPAD_TRANSITIONAL_FINGER = 7
} touchpad_finger_status_type;

/*****************************************************************************
*	Variables
******************************************************************************/
static unsigned char Touchpad_Manufacture_ID;
static unsigned char Touchpad_RMI_Ver;
static unsigned char Touchpad_Interface_Ver;
static unsigned char Touchpad_Major_Product_Ver;
static unsigned char Touchpad_Minor_Product_Ver;

static unsigned char Touchpad_Width_Gesture_SensorStatus;
static unsigned char Touchpad_Z;
static unsigned char Touchpad_X_Position_High;
static unsigned char Touchpad_X_Position_Low;
static unsigned char Touchpad_Y_Position_High;
static unsigned char Touchpad_Y_Position_Low;
static unsigned char Touchpad_X_Delta;
static unsigned char Touchpad_Y_Delta;
static unsigned char Touchpad_EnhancedGesture_Data;
static unsigned char Touchpad_Y_Zoom_X_Zoom;// For now Flick
static unsigned char Touchpad_Button_Data;
static unsigned char Touchpad_DeviceStatus;
static unsigned char Touchpad_W;
static touchpad_finger_status_type Touchpad_Finger_Status;
//static touchpad_finger_status_type	Touchpad_Old_Finger_Status=0;

//static int TouchPad_Finger_X_Compensate_Offset = 0;
//static int TouchPad_Finger_Y_Compensate_Offset = 0;
//static int LCD_X_Pos=0, LCD_Y_Pos=0;
//static int LCD_X_Pos_Drag=0, LCD_Y_Pos_Drag=0;
//static int LCD_X_Pos_Start=0, LCD_Y_Pos_Start=0;
//static int Touchpad_X_Pos_Old=0, Touchpad_Y_Pos_Old=0;

#ifdef TOUCH_PWR_CTRL_USE
static unsigned char Touchpad_pwr_status = 2;
static unsigned char Touchpad_reset_count;
static unsigned int touch_reset_cnt;
#endif

//==============================================================
#define DEVICE_NAME	"tcc-synaptics-ts"

#define CMD_SLEEP_NO		0x80
#define CMD_ADC_ON		0x04

#define PEN_DOWN 1
#define PEN_RELEASE 0

#if defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9200) || defined(CONFIG_MACH_TCC9201) || defined(CONFIG_MACH_TCC8900)
#define READ_PENIRQ() (HwGPIOA->GPDAT & Hw4)
#else
#define READ_PENIRQ() (HwGPIOE->GPDAT & Hw5)
#endif
#define X_AXIS_MIN	0
#define X_AXIS_MAX	3118
#define Y_AXIS_MIN	0
#define Y_AXIS_MAX	5184

struct ts_event {
	unsigned short pressure;
	unsigned short x;
	unsigned short y;
};

struct synaptics_ts {
	uint16_t addr;
	struct i2c_client *client;
	struct input_dev *input;
	int use_irq;
	unsigned int x, y, pressure, valid;
	int pen_status, opened, running;
	struct mutex mutex;
	struct work_struct work_q;
	struct clk *clk;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif	
};

static struct timer_list ts_timer;

static void synaptics_ts_early_suspend(struct early_suspend *h);
static void synaptics_ts_late_resume(struct early_suspend *h);

#ifdef TOUCH_PWR_CTRL_USE
static void TSC_PWR_CTRL(unsigned char ctrl)
{
	if (ctrl)
	{
//		BITCSET(HwGPIOC->GPDAT, Hw30, Hw30);
	}
	else
	{
//		BITCSET(HwGPIOC->GPDAT, Hw30, 0);
	}
}
#endif



#if 0
static int TSC_I2C_write(unsigned int uiAddr, unsigned int uiData)
{
	int nResult;
	
	unsigned char uData = (unsigned char)uiData;
	nResult = (int)DEV_I2C_write(I2C_TSC, TSC_I2C_ADDR, uiAddr, &uData, 1);

	return nResult;
}


static unsigned char TSC_I2C_read(unsigned int uiAddr)
{
	unsigned char ucSzDataBuff, result;
	result = DEV_I2C_read(I2C_TSC, TSC_I2C_ADDR, uiAddr, &ucSzDataBuff, 1);

	return (ucSzDataBuff);
}

static unsigned char TSC_I2C_Multi_read(unsigned int uiAddr, unsigned char *buffer, int size)
{
	unsigned char result;
	result = DEV_I2C_read(I2C_TSC, TSC_I2C_ADDR, uiAddr, buffer, size);

	return (result);
}
#endif

static void TOUCH_READ_MULTI(struct i2c_client *client, unsigned int reg, unsigned char *buf, int size)
{
	i2c_master_send(client, (const char *) &reg, 1);
	i2c_master_recv(client, buf, size);
}

static void synaptics_io_init(struct i2c_client *client)
{
	struct synaptics_ts *ts = i2c_get_clientdata(client);
	unsigned char ucMode = 0xC0;	

//	i2c_smbus_write_byte_data(0xf4, 0x01); /* device command = reset */
//	mdelay(1);

//	i2c_smbus_write_byte_data(TOUCHPAD_DEVICE_CONTROL_REG, TOUCHPAD_DEVICE_CONTROL_DEFAULT);
//	udelay(1);
	
//	i2c_smbus_write_byte_data(TOUCHPAD_2D_CONTROL_REG, TOUCHPAD_2D_CONTROL_DEFAULT);
//	udelay(1);

//	i2c_smbus_write_byte_data(TOUCHPAD_SENSITIVITY_REG, 0x00);
	
	Touchpad_RMI_Ver = i2c_smbus_read_byte_data(client, TOUCHPAD_RMI_VER_REG);
	Touchpad_Manufacture_ID = i2c_smbus_read_byte_data(client, TOUCHPAD_MANUFACTURE_ID_REG);
	Touchpad_Interface_Ver = i2c_smbus_read_byte_data(client, TOUCHPAD_INTERFACE_VER_REG);
	Touchpad_Major_Product_Ver = i2c_smbus_read_byte_data(client, TOUCHPAD_MAJOR_VER_REG);
	Touchpad_Minor_Product_Ver = i2c_smbus_read_byte_data(client, TOUCHPAD_MINOR_VER_REG);
	ucMode = i2c_smbus_read_byte_data(client, TOUCHPAD_PRODUCT_INFO2_REG);
	ucMode = i2c_smbus_read_byte_data(client, TOUCHPAD_PRODUCT_INFO3_REG);

	if (ts->use_irq) {
		if (Touchpad_Minor_Product_Ver>=3) {
			/* Rev.8 and higher rev. */
			i2c_smbus_write_byte_data(client, TOUCHPAD_INTERRUPT_ENABLE_REG, 0x05); // Button, ABS INT Enable
		} else {
			i2c_smbus_write_byte_data(client, TOUCHPAD_INTERRUPT_ENABLE_REG, 0x09); // Button, ABS INT Enable
		}
	} else {
		i2c_smbus_write_byte_data(client, TOUCHPAD_INTERRUPT_ENABLE_REG, 0); /* disable interrupt */
	}

	printk(KERN_INFO "%s: RMI version: %d\n", __func__, Touchpad_RMI_Ver);
	printk(KERN_INFO "%s: Manufacturer ID: %d\n", __func__, Touchpad_Manufacture_ID);
	printk(KERN_INFO "%s: Product major version: %d\n", __func__, Touchpad_Major_Product_Ver);
	printk(KERN_INFO "%s: Product minor version: %d\n", __func__, Touchpad_Minor_Product_Ver);
}

int TOUCH_IO_XY_POS_READ(struct i2c_client *client, unsigned int *x_pos, unsigned int *y_pos)
{
#if 1
	unsigned int Touchpad_X_Pos;
	unsigned int Touchpad_Y_Pos;
	unsigned char readData[12];
	int i;

	int iReturn = -1;

	// 처음 초기화가 안되었을때 다시 초기화 해주는 코드 넣어줌.
#ifdef TOUCH_PWR_CTRL_USE
	if (Touchpad_reset_count < TOUCH_PWR_REPAIR_CNT)
		Touchpad_reset_count++;

	if (Touchpad_pwr_status == 0)
	{
		Touchpad_pwr_status = 1;
		Touchpad_reset_count = 0;
		TSC_PWR_CTRL(1);
		return -1;
	}
	else 
#endif
	if (Touchpad_Manufacture_ID == 0 && Touchpad_RMI_Ver == 0 && Touchpad_Interface_Ver == 0 && 
		Touchpad_Major_Product_Ver == 0 && Touchpad_Minor_Product_Ver == 0
#ifdef TOUCH_PWR_CTRL_USE
		&& Touchpad_reset_count == 1
#endif
		)
	{
#ifdef TOUCH_PWR_CTRL_USE
		Touchpad_pwr_status = 2;
#endif
		synaptics_io_init(client);

		return -1;
	}

	if(Touchpad_Minor_Product_Ver>=3) // Rev.8 and higher rev.
		TOUCH_READ_MULTI(client, TOUCHPAD_DATA0_REG, readData, 10);
	else
		TOUCH_READ_MULTI(client, TOUCHPAD_DATA0_REG, readData, 12);

	if (readData[0] == 0 && readData[1] == 0 && readData[2] == 0 && readData[3] == 0 &&
		readData[4] == 0 && readData[5] == 0 && readData[6] == 0 && readData[7] == 0 &&
		readData[8] == 0 && readData[9] == 0
#ifdef TOUCH_PWR_CTRL_USE
		&& Touchpad_reset_count >= TOUCH_PWR_REPAIR_CNT
#endif
		)
	{
#ifdef TOUCH_PWR_CTRL_USE
		Touchpad_pwr_status = 0;
		Touchpad_reset_count = 0;
#endif
		i2c_smbus_write_byte_data(client, TOUCHPAD_DEVICE_COMMAND_REG, TOUCHPAD_DEVICE_COMMAND_RESET);
		mdelay(1);
		i2c_smbus_write_byte_data(client, TOUCHPAD_DEVICE_COMMAND_REG, TOUCHPAD_DEVICE_COMMAND_RESET);
		mdelay(1);				

#ifdef TOUCH_PWR_CTRL_USE
		//uart_send_printf("touch ic reset(%d)", touch_reset_cnt++);
		TSC_PWR_CTRL(0);
#endif
		return -1;
	}

	i=0;
	
	Touchpad_Width_Gesture_SensorStatus = readData[i++];
	Touchpad_Z = readData[i++];
	Touchpad_X_Position_High = readData[i++];
	Touchpad_X_Position_Low = readData[i++];
	Touchpad_Y_Position_High = readData[i++];
	Touchpad_Y_Position_Low = readData[i++];
	if(Touchpad_Minor_Product_Ver<3) // before Rev.8
	{
		Touchpad_X_Delta = readData[i++];
		Touchpad_Y_Delta = readData[i++];
	}
	Touchpad_EnhancedGesture_Data = readData[i++];
	Touchpad_Y_Zoom_X_Zoom = readData[i++];// For now Flick
	Touchpad_Button_Data = readData[i++];
	Touchpad_DeviceStatus = readData[i++];

	Touchpad_X_Pos = (int)Touchpad_X_Position_High*0x100 + Touchpad_X_Position_Low;
	Touchpad_Y_Pos = (int)Touchpad_Y_Position_High*0x100 + Touchpad_Y_Position_Low;

	Touchpad_W = Touchpad_Width_Gesture_SensorStatus>>4;
	Touchpad_Finger_Status = Touchpad_Width_Gesture_SensorStatus&0x07;


	if((Touchpad_X_Pos != 0) && (Touchpad_Y_Pos != 0) && Touchpad_Width_Gesture_SensorStatus)
	{
		*x_pos = Touchpad_X_Pos;
		*y_pos = TOUCHPAD_X_POS_MAX - Touchpad_Y_Pos;

		#ifdef TOUCH_PWR_CTRL_USE
		touch_reset_cnt = 0;
		#endif

		iReturn = 0;
	}

	return iReturn;
#endif
}

void synaptics_enableirq(void)
{
	HwPIC->INTMSK0 |= TSC_EINT_SEL_ID;	   
}

void synaptics_disableirq(void)
{
	HwPIC->INTMSK0 &= ~TSC_EINT_SEL_ID;    
}

static inline void tsc_pen_release(struct synaptics_ts *ts_data, struct input_dev *dev)
{
	if (ts_data->pen_status != PEN_RELEASE) {
		//printk("PENUP\n");
		ts_data->pen_status = PEN_RELEASE;

		input_report_key(dev, BTN_TOUCH, PEN_RELEASE);
		input_report_key(dev, ABS_PRESSURE, 0);
		input_sync(dev);
	}
}

static inline void tsc_pen_pressure(struct synaptics_ts *ts_data, struct input_dev *dev)
{
	ts_data->pen_status = PEN_DOWN;

	input_report_abs(dev, ABS_X, ts_data->x);
	input_report_abs(dev, ABS_Y, ts_data->y);	

	input_report_key(dev, BTN_TOUCH, PEN_DOWN);
	input_report_key(dev, ABS_PRESSURE, 1);
	input_sync(dev);
	msleep_interruptible(1);   //MCC
	//printk("PEN DOWN %u, %u, %u\n", synaptics_ts->x, synaptics_ts->y, ts_data->pressure);
}

static void ts_fetch_thread(struct work_struct *work)
{
	struct synaptics_ts* ts_data = container_of(work, struct synaptics_ts, work_q);
	struct input_dev *dev = ts_data->input;
	int flag, valid;
   
	ts_data->running = 1;
	synaptics_disableirq();
	if(READ_PENIRQ()){
		tsc_pen_release(ts_data, dev);
		ts_data->running = 0;
		synaptics_enableirq();
	} else {
		flag = TOUCH_IO_XY_POS_READ(ts_data->client, &(ts_data->x), &(ts_data->y));
		//tca_tch_poweroff();

		//printk("\n X_POS:%d   Y_POS: %d",ts_data->x,ts_data->y);

		valid = ts_data->x | ts_data->y;
		if ((flag == 0) && valid) {
			tsc_pen_pressure(ts_data, dev);
			ts_timer.expires = jiffies + msecs_to_jiffies(TOUCH_POS_READ_PERIOD);
			add_timer(&ts_timer);
		} else {
			tsc_pen_release(ts_data, dev);
			ts_data->running = 0;
			synaptics_enableirq();
		}
	}
}

static irqreturn_t synaptics_irq_handler(int irq, void *dev_id)
{
	struct synaptics_ts *ts_data = (struct synaptics_ts *)dev_id;
	
	if (ts_data->running == 0) {
		ts_data->running = 1;
		ts_timer.expires = jiffies;//+msecs_to_jiffies(TOUCH_POS_READ_PERIOD);
		add_timer(&ts_timer);
	}
	return IRQ_HANDLED;
}

static void ts_timer_handler(unsigned long data)
{
	struct synaptics_ts *ts = (struct synaptics_ts *) data;
	if (ts->opened) {
		if (schedule_work(&(ts->work_q)) == 0) {
			printk("cannot schedule work !!!\n");
			ts->running = 0;
		}
	} else {
		synaptics_disableirq();
	}
}

static int synaptics_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct synaptics_ts *ts;
	struct input_dev *input_dev;
	struct clk *clk;
	int err = -ENOMEM;
	int ret;

	ts = kzalloc(sizeof(struct synaptics_ts), GFP_KERNEL);
	if (ts == NULL) {
		return -ENOMEM;
	}
	ts->client = client;
	i2c_set_clientdata(client, ts);

	ret = i2c_smbus_write_byte_data(ts->client, 0xf4, 0x01); /* device command = reset */
	if (ret < 0) {
		dev_err(&client->dev, "i2c_smbus_write_byte_data failed\n");
	}
	{
		int retry = 10;
		while (retry-- > 0) {
			ret = i2c_smbus_read_byte_data(client, 0xe4);
			if (ret >= 0) {
				if (ret&0xFF == 0xFF) {
					printk(KERN_ERR "%s: read Major version failed\n", __func__);
					err = -ENOMEM;
					goto err_detect;
				}
				printk(KERN_ERR "%s: Major version is %x\n", __func__, ret);
				break;
			}
			msleep(100);
		}
	}
	if (ret < 0) {
		err = ret;
		goto err_detect;
	}

	input_dev = input_allocate_device();
	if (input_dev == NULL) {
		goto fail_input_dev_alloc;
	}

	clk = clk_get(NULL, "i2c");
	if (clk)
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
	input_dev->id.vendor = Touchpad_Manufacture_ID;   //MCC test
	input_dev->id.product = Touchpad_Major_Product_Ver;   //MCC test

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

#if 0 //POWER CONTROL
	BITCSET(HwGPIOF->GPFN2, HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN0(0));
	HwGPIOF->GPEN	|= Hw16;
	HwGPIOF->GPDAT	|= Hw16;
#endif	

// Interrupt Pin
#if 1 //arena
	BITCSET(HwGPIOA->GPFN0, HwPORTCFG_GPFN4_MASK, HwPORTCFG_GPFN4(0));
	HwGPIOA->GPEN	&= ~Hw4;
	HwGPIOA->GPDAT	&= ~Hw4;

	BITCLR(TSC_EINT_PORT_EN, TSC_EINT_PIN);						/* PENIRQ Port direction set (input) */
	BITCSET(TSC_EINT_PORT_PD, TSC_EINT_PORT_PD_MASK, TSC_EINT_PORT_PD_UP);
	BITCSET(HwEINTSEL->EINTSEL2, TSC_EINT_SEL_MASK, TSC_EINT_SEL);

	HwPIC->INTMSK0 |= TSC_EINT_SEL_ID;	   
	
	HwPIC->IEN0		&= ~TSC_EINT_SEL_ID;						/* disable Isr */
	HwPIC->MODEA0	|= TSC_EINT_SEL_ID; 						/* both edge */
	HwPIC->MODE0	&= ~TSC_EINT_SEL_ID;						/* set edge trigger mode */
	HwPIC->POL0		|= TSC_EINT_SEL_ID;							/* active-low */
	HwPIC->CLR0		|= TSC_EINT_SEL_ID;							/* clear pending status */
	HwPIC->SEL0		|= TSC_EINT_SEL_ID;							/* IRQ Select */
	HwPIC->IEN0		|= TSC_EINT_SEL_ID;							/* Enable Interrupt */
#endif
	
	err = input_register_device(ts->input);
	if (err)
		goto fail_input_register_device;

	/* Enable touch interrupt */
	if ((err = request_irq(client->irq, synaptics_irq_handler, 0, DEVICE_NAME, ts))) {
		err = -EBUSY;
		goto fail_request_irq;
	} else {
		ts->use_irq = 1;
	}
        
//	tca_tch_poweroff();
	synaptics_io_init(client);

#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.suspend = synaptics_ts_early_suspend;
	ts->early_suspend.resume = synaptics_ts_late_resume;
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	register_early_suspend(&ts->early_suspend);
#endif

	printk(KERN_INFO "synaptics_ts_probe: synaptics driver is successfully loaded\n");
	return 0;

fail_request_irq:
fail_input_register_device:
	input_free_device(input_dev);
fail_input_dev_alloc:
err_detect:
	kfree(ts);
	return err;
}

static int synaptics_ts_remove(struct i2c_client *client)
{
	struct synaptics_ts *ts = i2c_get_clientdata(client);
	unregister_early_suspend(&ts->early_suspend);
	if (ts->use_irq)
		free_irq(client->irq, ts);
	input_unregister_device(ts->input);
	clk_disable(ts->clk);
	clk_put(ts->clk);
	kfree(ts);
	return 0;
}

#ifdef CONFIG_PM
static int synaptics_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
	synaptics_disableirq();

	return 0;
}

static int synaptics_ts_resume(struct i2c_client *client)
{
	synaptics_io_init(client);
	synaptics_enableirq();

	return 0;
}
#else
#define synaptics_ts_suspend     NULL
#define synaptics_ts_resume      NULL
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void synaptics_ts_early_suspend(struct early_suspend *h)
{
	struct synaptics_ts *ts;
	ts = container_of(h, struct synaptics_ts, early_suspend);
	synaptics_ts_suspend(ts->client, PMSG_SUSPEND);

}
static void synaptics_ts_late_resume(struct early_suspend *h)
{
	struct synaptics_ts *ts;
	ts = container_of(h, struct synaptics_ts, early_suspend);
	synaptics_ts_resume(ts->client);
}
#endif

static const struct i2c_device_id synaptics_ts_id[] = {
	{ DEVICE_NAME, 0 },
	{ }
};

static struct i2c_driver synaptics_ts_driver = {
	.probe		= synaptics_ts_probe,
	.remove		= synaptics_ts_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend	= synaptics_ts_suspend,
	.resume		= synaptics_ts_resume,
#endif
	.id_table	= synaptics_ts_id,
	.driver = {
		.name	= DEVICE_NAME,
	},
};

static int __init synaptics_ts_init(void)
{
    printk("Telechips Synaptics touch driver\n");
    return i2c_add_driver(&synaptics_ts_driver);
}


void __exit synaptics_ts_exit(void)
{
	i2c_del_driver(&synaptics_ts_driver);
}

module_init(synaptics_ts_init);
module_exit(synaptics_ts_exit);

MODULE_AUTHOR("linux@telechips.com");
MODULE_DESCRIPTION("Telechips synaptics touch driver");
MODULE_LICENSE("GPL");
