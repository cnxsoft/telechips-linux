/*
 * Touch Screen driver for Sitronix I2C Interface.
 *   Copyright (c) 2010 CT Chen <ct_chen@sitronix.com.tw>
 *
 * Please refer to Sitronix I2C Interface Protocol for the protocol specification.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/timer.h>
#include <linux/gpio.h>

#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/mutex.h>
//#include <mach/gpio.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/string.h>
#include <linux/gpio.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <linux/earlysuspend.h>

#include <asm/mach-types.h>
#include <mach/bsp.h>
#include <mach/io.h>

#define DEVICE_NAME "tcc-ts-sitronix"


#define TS_TIMER_SUPPORT 1
#define TS_TIMER_PERIOD 10
#define MULTI_TOUCH 1

#define MAX_SAME_COUNT 10
#define MAX_SYNC_COUNT 5

#define REMOVE_SAME_POINT 1

#define TS_X_REVERSE 0
#define TS_Y_REVERSE 0
#define TS_XY_SWAP 0


struct sitronix_ts_priv {
	struct i2c_client *client;
	struct input_dev *input;
	struct work_struct work;
	struct mutex mutex;
      struct early_suspend early_suspend;
      struct early_suspend earlier_suspend;	
#if TS_TIMER_SUPPORT	
	struct timer_list ts_timer;	
 	int running;	
	int opened;	
	int pen_status;
#endif	
	int irq;
	bool isp_enabled;
	bool autotune_result;
	bool always_update;
	char I2C_Offset;
	char I2C_Length;
	char I2C_RepeatTime;
	int  fw_revision;
	int	 struct_version;
};


#define MAX_X 1280
#define MAX_Y 800
#define MAX_FINGERS 5
#define CHECK_FINGERS 1


typedef struct {
	u8	y_h		: 3,
		reserved	: 1,
		x_h		: 3,
		valid		: 1;
	u8	x_l;
	u8	y_l;
	u8	z;
} xy_data_t;

typedef struct {
	xy_data_t	xy_data[5];
} stx_report_data_t;



static int gp_penirq = 0;

static int temp_x;
static int same_count=0;
static int sync_count = 0;
static u16 x_res = 0, y_res = 0;

static int i2c_read_bytes(struct i2c_client *client, uint8_t *buf, int len)
{
	struct i2c_msg msgs[2];
	int ret=-1;
	int retries = 0;

	msgs[0].flags=!I2C_M_RD;
	msgs[0].addr=client->addr;
	msgs[0].len=1;
	msgs[0].buf=&buf[0];

	msgs[1].flags=I2C_M_RD;
	msgs[1].addr=client->addr;
	msgs[1].len=len-1;
	msgs[1].buf=&buf[1];

	while(retries<5)
	{
		ret=i2c_transfer(client->adapter,msgs, 2);
		if(ret < 0) printk("i2c read fail\n");
		//if(ret == 2)break;
		if(ret > 0)break;
		retries++;
	}
	return ret;
}


static int i2c_write_bytes(struct i2c_client *client,uint8_t *data,int len)
{
	struct i2c_msg msg;
	int ret=-1;
	int retries = 0;

	msg.flags=!I2C_M_RD;
	msg.addr=client->addr;
	msg.len=len;
	msg.buf=data;		
	
	while(retries<5)
	{
		ret=i2c_transfer(client->adapter,&msg, 1);
		if(ret == 1)break;
		retries++;
	}
	return ret;
}


static int sitronix_ts_get_sc(struct i2c_client *client, u32 *var)
{
	char buf[2];
	int ret;

	buf[0] = 0x7;
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
	return -EIO;

	//Read 1 byte FW version from Reg. 0x0 set previously.
	if ((ret = i2c_master_recv(client, buf, 2)) != 2)
		return -EIO;

	*var = (u32)(buf[0]<<8 + buf[1]);

	return 0;	
}



static int sitronix_ts_get_fingers(struct i2c_client *client, u32 *var)
{
	char buf[1];
	int i,ret, temp=0;


	for(i=0 ; i<CHECK_FINGERS; i++)
	{
		buf[0] = 0x10; // finger register
		if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;
	
		if ((ret = i2c_master_recv(client, buf, 1)) != 1)
			return -EIO;
		temp += (u32)buf[0];
	}

	*var = (temp/CHECK_FINGERS);

#if 0
	buf[0] = 0x10;	//Set Reg. address back to 0x10 for coordinates.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;
#endif

	return 0;	


}


static int sitronix_ts_get_device_control(struct i2c_client *client, u32 *var)
{
	char buf[1];
	int ret;

	buf[0] = 0x2;
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
	return -EIO;

	//Read 1 byte FW version from Reg. 0x0 set previously.
	if ((ret = i2c_master_recv(client, buf, 1)) != 1)
		return -EIO;

	*var = (u32) buf[0];

	buf[0] = 0x10;	//Set Reg. address back to 0x10 for coordinates.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	return 0;
	
}

static int sitronix_ts_get_device_status(struct i2c_client *client, u32 *var)
{
	char buf[1];
	int ret;

	buf[0] = 0x1;
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
	return -EIO;

	//Read 1 byte FW version from Reg. 0x0 set previously.
	if ((ret = i2c_master_recv(client, buf, 1)) != 1)
		return -EIO;

	*var = (u32) buf[0];

	buf[0] = 0x10;	//Set Reg. address back to 0x10 for coordinates.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	return 0;
	
}

static int sitronix_ts_get_fw_version(struct i2c_client *client, u32 *ver)
{
	char buf[1];
	int ret;

	buf[0] = 0x0;	//Set Reg. address to 0x0 for reading FW version.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	//Read 1 byte FW version from Reg. 0x0 set previously.
	if ((ret = i2c_master_recv(client, buf, 1)) != 1)
		return -EIO;

	*ver = (u32) buf[0];

	buf[0] = 0x10;	//Set Reg. address back to 0x10 for coordinates.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	return 0;
}

static int sitronix_ts_get_fw_revision(struct i2c_client *client, u32 *rev)
{
	char buf[4];
	int ret;

	buf[0] = 0xC;	//Set Reg. address to 0x0 for reading FW version.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	//Read 1 byte FW version from Reg. 0x0 set previously.
	if ((ret = i2c_master_recv(client, buf, 4)) != 4)
		return -EIO;

	*rev = ((u32)buf[3]);
	*rev |= (((u32)buf[2]) << 8);
	*rev |= (((u32)buf[1]) << 16);
	*rev |= (((u32)buf[0]) << 24);

	buf[0] = 0x10;	//Set Reg. address back to 0x10 for coordinates.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	return 0;

}


static int sitronix_ts_enter_powerdown(struct i2c_client *client)
{
	char buf[4];
	
	buf[0] = 0x02;
	buf[1] = 0x0a;
	if(i2c_master_send(client, buf, 2) != 2)
		return -EIO;
	return 0;
}

static int sitronix_ts_exit_powerdown(struct i2c_client *client)
{
	char buf[4];
	
	buf[0] = 0x02;
	buf[1] = 0x01;
	if(i2c_master_send(client, buf, 2) != 2)
		return -EIO;
	return 0;
}

static int sitronix_ts_get_resolution(struct i2c_client *client, u16 *x_res, u16 *y_res)
{
	char buf[3];
	int ret;

	buf[0] = 0x4;	//Set Reg. address to 0x4 for reading XY resolution.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	//Read 3 byte XY resolution from Reg. 0x4 set previously.
	if ((ret = i2c_master_recv(client, buf, 3)) != 3)
		return -EIO;

	*x_res = ((buf[0] & 0xF0) << 4) | buf[1];
	*y_res = ((buf[0] & 0x0F) << 8) | buf[2];

	buf[0] = 0x10;	//Set Reg. address back to 0x10 for coordinates.
	if ((ret = i2c_master_send(client, buf, 1)) != 1)
		return -EIO;

	return 0;

}


static int sitronix_ts_pendown(void)
{
	int i = 0;
	int temp = 0;
	
	for(i=0 ; i<5; i++)
	{
		if(gpio_get_value(gp_penirq))
			temp++;
	}
	
	if(temp == 5){
		printk("pen up\n");
		return 0;
	}else{
		printk("pen down\n");
		return 1;
	}
}

static void sitronix_ts_work(struct work_struct *work)
{
#if 1
	struct sitronix_ts_priv *priv = container_of(work, struct sitronix_ts_priv, work);
	stx_report_data_t *pt_data;
	int i,err,ret,valid;
	char buf[8], fingers;
	char point_data[1 + (MAX_FINGERS*4)];
	unsigned int x, swap_x, y, z;	
	char sc;
	//printk("%s\n", __func__);
	mutex_lock(&priv->mutex);

	
#if 1
	if ((err = sitronix_ts_get_fingers(priv->client, &fingers))) {
		printk("Unable to get fingers!\n");
	} else {
		//printk("%s(%u): fingers=%X\n", __FUNCTION__, __LINE__, fingers);
	}
#endif
	

#if MULTI_TOUCH
		if(fingers && (fingers != 0xff))
		{
			point_data[0] = 0x12;
		ret = i2c_read_bytes(priv->client, point_data, (MAX_FINGERS*4)+1);
			pt_data = (stx_report_data_t*)(point_data+1);
	
			for(i=0;i<fingers;i++)
			{
				valid = pt_data->xy_data[i].valid;
				x = pt_data->xy_data[i].x_h << 8 | pt_data->xy_data[i].x_l;
#if TS_X_REVERSE
			x = x_res - x;
#endif
			y = pt_data->xy_data[i].y_h << 8 | pt_data->xy_data[i].y_l;
#if TS_Y_REVERSE
			y = y_res -y;
#endif

#if TS_XY_SWAP
			swap_x = x;
			x = y;
			y = swap_x;
#endif
				z = pt_data->xy_data[i].z;

#if REMOVE_SAME_POINT
				if(i == 0 && temp_x == x && same_count<MAX_SAME_COUNT && sync_count>MAX_SYNC_COUNT)
				{
					//printk("same count = %d\n", same_count);
					same_count++;
				}else{
#endif
				if(valid && x > 0 && x < MAX_X && y > 0 && y < MAX_Y)
				{
					sync_count++;
					if(sync_count>0x7FFFFFFF) sync_count = 0;
					if(fingers > 1 && x > 600 && y < 50){  // temporary tuning
								
					}
					else{
					input_report_key(priv->input, BTN_TOUCH, 1);
					input_report_abs(priv->input, ABS_MT_POSITION_X, x);
					input_report_abs(priv->input, ABS_MT_POSITION_Y, y);
					input_report_abs(priv->input, ABS_MT_TOUCH_MAJOR, 5);
					input_report_abs(priv->input, ABS_MT_WIDTH_MAJOR, 5);
//					input_report_abs(priv->input, ABS_PRESSURE, 1);			
					input_report_abs(priv->input, ABS_MT_TRACKING_ID, i);
					input_mt_sync(priv->input);
					//printk("pt[%d] x = %x, y = %d, z = %d\n", i, x, y, z);
					}
				}
#if REMOVE_SAME_POINT				
				}
#endif
				temp_x = x;
				//printk("temp_x = %d\n", temp_x);
				
				
			}
			input_sync(priv->input);
#if TS_TIMER_SUPPORT		
			priv->ts_timer.expires = jiffies + msecs_to_jiffies(TS_TIMER_PERIOD);
			add_timer(&priv->ts_timer);
#endif
			}
	
	else
	{
		same_count = 0;
		sync_count = 0;
		//printk("pen release\n");
		input_report_key(priv->input, BTN_TOUCH, 0);
		//input_report_abs(priv->input, ABS_MT_TOUCH_MAJOR, 0);
		//input_report_abs(priv->input, ABS_MT_WIDTH_MAJOR, 0);
		input_mt_sync(priv->input);
		input_sync(priv->input);
#if TS_TIMER_SUPPORT		
		priv->running = 0;
		enable_irq(priv->irq);
#endif

	}



#else
	if(fingers && (fingers != 0xff))
	{
		point_data[0] = 0x12;
		ret = i2c_read_bytes(priv->client, point_data, (fingers*4)+1);
		pt_data = (stx_report_data_t*)(point_data+1);

		valid = pt_data->xy_data[0].valid;
		x = pt_data->xy_data[0].x_h << 8 | pt_data->xy_data[0].x_l;
		y = pt_data->xy_data[0].y_h << 8 | pt_data->xy_data[0].y_l;
		z = pt_data->xy_data[0].z;

		if(valid)
		{
			input_report_key(priv->input, BTN_TOUCH, 1);
			input_report_abs(priv->input, ABS_X, x);
			input_report_abs(priv->input, ABS_Y, y);
			input_report_abs(priv->input, ABS_PRESSURE, 1);		
			input_sync(priv->input);
			//printk("pt x = %x, y = %d, z = %d\n", x, y, z);			
		}
#if TS_TIMER_SUPPORT		
		priv->ts_timer.expires = jiffies + msecs_to_jiffies(TS_TIMER_PERIOD);
		add_timer(&priv->ts_timer);
#endif		
	}
	else{
		input_report_key(priv->input, BTN_TOUCH, 0);
		input_report_abs(priv->input, ABS_PRESSURE, 0);
		input_sync(priv->input);
#if TS_TIMER_SUPPORT		
		priv->running = 0;
		enable_irq(priv->irq);
#endif		
	}
		
#endif

out:
	mutex_unlock(&priv->mutex);
#endif
}

static irqreturn_t sitronix_ts_isr(int irq, void *dev_data)
{
#if TS_TIMER_SUPPORT
	struct sitronix_ts_priv *priv = dev_data;
	//printk("%s\n", __func__);
	if(priv->running == 0)
	{
		priv->running = 1;
		disable_irq_nosync(priv->irq);
		priv->ts_timer.expires = jiffies;
		add_timer(&priv->ts_timer);
	}
	return IRQ_HANDLED;
#else
	struct sitronix_ts_priv *priv = dev_data;

	//printk("%s\n", __func__);
	schedule_work(&priv->work);

	return IRQ_HANDLED;
#endif
}


#if TS_TIMER_SUPPORT
static void sitronix_timer_handler(unsigned long data)
{
	struct sitronix_ts_priv *priv = (struct sitronix_ts_priv *) data;
	//printk(KERN_INFO "sitronix_timer_handle in\n");
	if (priv->opened) {
		if (schedule_work(&(priv->work)) == 0) {
			printk("cannot schedule work !!!\n");
			priv->running = 0;
		}
	} else {
		disable_irq_nosync(priv->irq);
	}
	//printk(KERN_INFO "sitronix_timer_handle exit\n");
}
#endif


static int sitronix_ts_open(struct input_dev *dev)
{
	struct sitronix_ts_priv *priv = input_get_drvdata(dev);

	enable_irq(priv->irq);

	return 0;
}

static void sitronix_ts_close(struct input_dev *dev)
{
	struct sitronix_ts_priv *priv = input_get_drvdata(dev);

	disable_irq(priv->irq);
	cancel_work_sync(&priv->work);
}

static void sitronix_ts_port_init(void)
{
	volatile PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	//int gp_penirq;

#if defined(CONFIG_ARCH_TCC88XX)
	if (machine_is_tcc8800()) {
		gp_penirq = TCC_GPA(6);
	}
#elif defined(CONFIG_ARCH_TCC892X)
	if (machine_is_tcc8920()) {
		if(system_rev == 0x1005 || system_rev == 0x1007)
			gp_penirq = TCC_GPB(11);
		else if(system_rev == 0x1006)
			gp_penirq = TCC_GPE(17);
		else if(system_rev == 0x1008)
			gp_penirq = TCC_GPE(5);
		else
			gp_penirq = TCC_GPG(18);
	}
#endif

      gpio_request(gp_penirq, "tsc_int");
      tcc_gpio_config(gp_penirq, GPIO_FN(0));
      gpio_direction_input(gp_penirq);

#if defined(CONFIG_ARCH_TCC88XX)
      if (machine_is_tcc8800()) {
		BITCSET(pGPIO->EINTSEL0 , HwEINTSEL0_EINT2_MASK, HwEINTSEL0_EINT2(SEL_GPIOA6));

		HwPIC->INTMSK0	|= Hw5;
		HwPIC->IEN0	&= ~Hw5;	/* disable Isr */
		HwPIC->MODEA0	&= ~Hw5; 	/* single edge */
		HwPIC->MODE0	&= ~Hw5;	/* set edge trigger mode */
		HwPIC->POL0	|= Hw5;		/* active-low */
		HwPIC->CLR0	|= Hw5;		/* clear pending status */
		HwPIC->SEL0	|= Hw5;		/* IRQ Select */
		HwPIC->IEN0	|= Hw5;
	}
#elif defined(CONFIG_ARCH_TCC892X)
	if (machine_is_tcc8920()) {
		if(system_rev == 0x1005 || system_rev == 0x1007)
			tcc_gpio_config_ext_intr(INT_EI2, EXTINT_GPIOB_11);
		else if(system_rev == 0x1006)
			tcc_gpio_config_ext_intr(INT_EI2, EXTINT_GPIOE_17);
		else if(system_rev == 0x1008)
			tcc_gpio_config_ext_intr(INT_EI2, EXTINT_GPIOE_05);
		else
			tcc_gpio_config_ext_intr(INT_EI2, EXTINT_GPIOG_18);
	}
#endif
}


static int st15xx_jump_to_isp(struct i2c_client *client)
{
	int i;
	u8 signature[] = "STX_FWUP";
	u8 buf[2];

	for (i = 0; i < strlen(signature); i++) {
		buf[0] = 0x0;
		buf[1] = signature[i];
		if (i2c_master_send(client, buf, 2) != 2) {
			dev_err(&client->dev, "%s(%u): Unable to write ISP STX_FWUP!\n", __FUNCTION__, __LINE__);
			return -EIO;
		}
		msleep(100);
	}

	//printk("*************************************** Jump to ISP ok! ***********************************************\n");

	return 0;
}

static ssize_t sitronix_ts_isp_ctrl_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct sitronix_ts_priv *priv = i2c_get_clientdata(client);
	
	return sprintf(buf, "%d\n", priv->isp_enabled);
}

static ssize_t sitronix_ts_isp_ctrl_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct sitronix_ts_priv *priv = i2c_get_clientdata(client);
	int enabled = 0;

	sscanf(buf, "%x", &enabled);
	if (priv->isp_enabled && !enabled) {
		//ISP Reset.
		priv->isp_enabled = false;
	} else if (!priv->isp_enabled && enabled) {
		//Jump to ISP.
		priv->isp_enabled = true;
		st15xx_jump_to_isp(client);
	}

	return count;
}

static DEVICE_ATTR(isp_ctrl, 0644, sitronix_ts_isp_ctrl_show, sitronix_ts_isp_ctrl_store);

static ssize_t sitronix_ts_revision_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	u32 rev;
	int err;
	
	if ((err = sitronix_ts_get_fw_revision(client, &rev))) {
		dev_err(&client->dev, "Unable to get FW revision!\n");
		return 0;
	}

	return sprintf(buf, "%u\n", rev);
}

static DEVICE_ATTR(revision, 0644, sitronix_ts_revision_show, NULL);

static struct attribute *sitronix_ts_attrs_v0[] = {
	&dev_attr_isp_ctrl.attr,
	&dev_attr_revision.attr,
	NULL,
};



static struct attribute_group sitronix_ts_attr_group_v0 = {
	.name = "sitronix_ts_attrs",
	.attrs = sitronix_ts_attrs_v0,
};

static int sitronix_ts_create_sysfs_entry(struct i2c_client *client)
{
	//struct sitronix_ts_priv *priv = i2c_get_clientdata(client);
	int err;

	err = sysfs_create_group(&(client->dev.kobj), &sitronix_ts_attr_group_v0);
	if (err) {
		dev_dbg(&client->dev, "%s(%u): sysfs_create_group() failed!\n", __FUNCTION__, __LINE__);
	}
	return err;
}

static void sitronix_ts_destroy_sysfs_entry(struct i2c_client *client)
{
	//struct sitronix_ts_priv *priv = i2c_get_clientdata(client);
	sysfs_remove_group(&(client->dev.kobj), &sitronix_ts_attr_group_v0);

	return;
}

void sitronix_ts_early_suspend(struct early_suspend *h)
{
	struct sitronix_ts_priv *priv = container_of(h, struct sitronix_ts_priv, early_suspend);
	printk("%s in\n", __func__);
	//when enter suspend, disable irq
	disable_irq_nosync(priv->irq);
	//cancel_work_sync(&priv->work);
#if TS_TIMER_SUPPORT	
	del_timer_sync(&priv->ts_timer);
#endif
	sitronix_ts_enter_powerdown(priv->client);
	printk("%s out\n", __func__);
}

void sitronix_ts_late_resume(struct early_suspend *h)
{
	struct sitronix_ts_priv *priv = container_of(h, struct sitronix_ts_priv, early_suspend);
	printk("%s in\n", __func__);
	//when enter suspend, disable irq
	enable_irq(priv->irq);
	sitronix_ts_exit_powerdown(priv->client);
	printk("%s out\n", __func__);	
}

static int __devinit sitronix_ts_probe(struct i2c_client *client, const struct i2c_device_id *idp)
{
	struct sitronix_ts_priv *priv;
	struct input_dev *input;
	int err = -ENOMEM;
	u32 ver, rev, var;
	char buf[2];
	//u8 struct_ver;
	

	printk("%s\n", __func__);

	sitronix_ts_port_init();

	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		printk("Failed to allocate driver data\n");
		goto err0;
	}

	mutex_init(&priv->mutex);
	input = input_allocate_device();

	if (!input) {
		dev_err(&client->dev, "Failed to allocate input device.\n");
		goto err1;
	}

#if 1   // reset
      buf[0] = 0x02;
	buf[1] = 0x01;
	i2c_write_bytes(client, buf, 2);
#endif

	mdelay(30);

#if AUTO_TUNE // auto tune
	buf[0] = 0x02;
	buf[1] = 0x00;
	i2c_write_bytes(client, buf, 2);

	buf[0] = 0x02;
	buf[1] = 0x80;
	i2c_write_bytes(client, buf, 2);
	
	buf[0] = 0x01;
	while(1){
		i2c_read_bytes(client, buf, 2);
		printk("i2c read device status = %X\n", buf[1]);		
		if(buf[1] == 0) break; // auto tune done
	}

	buf[0] = 0x02;
	buf[1] = 0x40;
	i2c_write_bytes(client, buf, 2);
#endif

#if PROXIMITY  // proximity on
	buf[0] = 0x02;
	buf[1] = 0x04;
	i2c_write_bytes(client, buf, 2);
#endif



	if ((err = sitronix_ts_get_device_status(client, &var))) {
		printk("Unable to get device control!\n");
		goto err1;
	} else {
		printk("%s(%u): device status=%X\n", __FUNCTION__, __LINE__, var);
	}


	if ((err = sitronix_ts_get_device_control(client, &var))) {
		printk("Unable to get device control!\n");
		goto err1;
	} else {
		printk("%s(%u): device control=%X\n", __FUNCTION__, __LINE__, var);
	}


	if ((err = sitronix_ts_get_fw_version(client, &ver))) {
		printk("Unable to get FW version!\n");
		goto err1;
	} else {
		printk("%s(%u): FW version=%X\n", __FUNCTION__, __LINE__, ver);
	}
	
	if ((err = sitronix_ts_get_resolution(client, &x_res, &y_res))) {
		printk("Unable to get resolution!\n");
		goto err1;
	}
	printk("%s: sitronix resolution x = %d, y = %d\n", __FUNCTION__, (int)x_res, (int)y_res);

	input->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
	input->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input->absbit[0] = BIT(ABS_X) | BIT(ABS_Y) | BIT(ABS_PRESSURE); 		

//	set_bit(EV_ABS, input->evbit);
//	set_bit(EV_SYN, input->evbit);
#if 0
	set_bit(ABS_MT_TRACKING_ID, input->absbit);
	set_bit(ABS_MT_POSITION_X, input->absbit);
	set_bit(ABS_MT_POSITION_Y, input->absbit);
#endif	// 0
//	set_bit(BTN_TOUCH, input->keybit);
#if MULTI_TOUCH
	input_set_abs_params(input, ABS_MT_WIDTH_MAJOR, 0, 50, 0, 0);
	input_set_abs_params(input, ABS_MT_TOUCH_MAJOR, 0, 50, 0, 0);
	input_set_abs_params(input, ABS_MT_POSITION_X, 0, (int)x_res, 0, 0);
	input_set_abs_params(input, ABS_MT_POSITION_Y, 0, (int)y_res, 0, 0);
//	input_set_abs_params(input, ABS_PRESSURE, 0, 0, 0, 0);	
	input_set_abs_params(input, ABS_MT_TRACKING_ID, 0, MAX_FINGERS, 0, 0);	
#else
	input_set_abs_params(input, ABS_X, 0, (int)x_res, 0, 0);
	input_set_abs_params(input, ABS_Y, 0, (int)y_res, 0, 0);
	input_set_abs_params(input, ABS_PRESSURE, 0, 1, 0, 0);
#endif	// 0
	
	input->name = DEVICE_NAME;//client->name;
	input->id.bustype = BUS_I2C;
	input->dev.parent = &client->dev;
//	input->open = sitronix_ts_open;
//	input->close = sitronix_ts_close;

	priv->client = client;
	priv->input = input;
	priv->irq = INT_EI2;
	priv->isp_enabled = false;
	priv->autotune_result = false;
	priv->always_update = false;

	INIT_WORK(&priv->work, sitronix_ts_work);
#if TS_TIMER_SUPPORT
	priv->running = 0;
	priv->opened = 1;
	//priv->pen_status = PEN_RELEASE;
	init_timer(&priv->ts_timer);
	priv->ts_timer.data = (unsigned long) priv;
	priv->ts_timer.function = sitronix_timer_handler;
#endif
	
	i2c_set_clientdata(client, priv);
	input_set_drvdata(input, priv);


	if ((err = sitronix_ts_get_fw_revision(client, &rev))) {
		dev_err(&client->dev, "Unable to get FW revision!\n");
		goto err1;
	} else {
		dev_dbg(&client->dev, "%s(%u): FW revision=%X\n", __FUNCTION__, __LINE__, rev);
		priv->fw_revision = rev;
	}
	
	// ToDo
	// Check Version
	// Check Power 
	//sitronix_ts_internal_update(client);
#if 1
	priv->early_suspend.suspend = sitronix_ts_early_suspend;
	priv->early_suspend.resume = sitronix_ts_late_resume;
	priv->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB - 3;
	register_early_suspend(&priv->early_suspend);	
#endif

	err = input_register_device(input);
	if (err)
		goto err1;

	err = request_irq(INT_EI2, sitronix_ts_isr, IRQF_TRIGGER_FALLING | IRQF_DISABLED,
			  DEVICE_NAME, priv);
	if (err) {
		printk("Unable to request touchscreen IRQ.\n");
		goto err2;
	}

	/* Disable irq. The irq will be enabled once the input device is opened. */
	//disable_irq(priv->irq);

	//device_init_wakeup(&client->dev, 0);

	err = sitronix_ts_create_sysfs_entry(client);
	if (err)
		goto err2;

	printk("%s done\n", __func__);

	return 0;

err2:
	input_unregister_device(input);
	input = NULL; /* so we dont try to free it below */
err1:
	input_free_device(input);
	i2c_set_clientdata(client, NULL);
	kfree(priv);
err0:
	return err;
}

static int __devexit sitronix_ts_remove(struct i2c_client *client)
{
	struct sitronix_ts_priv *priv = i2c_get_clientdata(client);

	sitronix_ts_destroy_sysfs_entry(client);
	free_irq(priv->irq, priv);
	input_unregister_device(priv->input);
	i2c_set_clientdata(client, NULL);
	kfree(priv);

	return 0;
}

#ifdef CONFIG_PM
static int sitronix_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct sitronix_ts_priv *priv = i2c_get_clientdata(client);
	
	printk("%s in\n", __func__);

	printk("%s out\n", __func__);
	return 0;
}

static int sitronix_ts_resume(struct i2c_client *client)
{
	struct sitronix_ts_priv *priv = i2c_get_clientdata(client);
		
	printk("%s in\n", __func__);

	printk("%s out\n", __func__);
	return 0;
}
#else
#define sitronix_ts_suspend NULL
#define sitronix_ts_resume NULL
#endif

static const struct i2c_device_id sitronix_ts_id[] = {
	{ "tcc-ts-sitronix", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, sitronix_ts_id);

static struct i2c_driver sitronix_ts_driver = {
	.driver = {
		.name = "tcc-ts-sitronix",
	},
	.probe = sitronix_ts_probe,
	.remove = __devexit_p(sitronix_ts_remove),
	.suspend = sitronix_ts_suspend,
	.resume = sitronix_ts_resume,
	.id_table = sitronix_ts_id,
};

static int __init sitronix_ts_init(void)
{
	printk("%s\n", __func__);
	return i2c_add_driver(&sitronix_ts_driver);
}

static void __exit sitronix_ts_exit(void)
{
	i2c_del_driver(&sitronix_ts_driver);
}

MODULE_DESCRIPTION("Sitronix Touch Screen Driver");
MODULE_AUTHOR("CT Chen <ct_chen@sitronix.com.tw>");
MODULE_LICENSE("GPL");

module_init(sitronix_ts_init);
module_exit(sitronix_ts_exit);

