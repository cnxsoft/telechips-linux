/* 
 * linux/drivers/char/sensor_ak8975c.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: Telechips Linux BACK-LIGHT DRIVER
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/mach-types.h>
#include <linux/delay.h>


#include <asm/mach-types.h>
//#include <mach/tca_backlight.h>

//#if defined(CONFIG_ARCH_TCC92X) || defined (CONFIG_ARCH_TCC93XX)
#include <mach/bsp.h>
//#endif

#ifdef CONFIG_I2C
#include <linux/i2c.h>
#endif
#include <asm/gpio.h>
#include <linux/i2c/sensor_i2c.h>
#include <linux/input.h>

#include <mach/sensor_ioctl.h>


#define AK8975C_DEBUG    0
#define AK8975C_DEBUG_D  0

#if AK8975C_DEBUG
#define msensor_dbg(fmt, arg...)     printk(fmt, ##arg)
#else
#define msensor_dbg(arg...)
#endif

#if AK8975C_DEBUG_D
#define msensor_dbg_d(fmt, arg...)     printk(fmt, ##arg)
#else
#define msensor_dbg_d(arg...)
#endif


#define MSENSOR_DEV_NAME		"msensor"
#define MSENSOR_DEV_MAJOR		248
#define MSENSOR_DEV_MINOR		1

typedef struct {
	int x;
	int y;
	int z;
} tcc_sensor_magne_t;


#ifdef CONFIG_I2C
#define AK8975C_I2C_ADDRESS      0x0F

static struct i2c_driver msensor_i2c_driver;
static struct i2c_client *msensor_i2c_client;

static struct timer_list *msensor_timer;
static int msensor_used_count=0;
volatile static unsigned int msensor_duration=200;
volatile static unsigned int msensor_state_flag=0;
volatile static tcc_sensor_magne_t tcc_msensor_magne;
static struct work_struct msensor_work_q;

static const struct i2c_device_id msensor_i2c_id[] = {
	{ "tcc-magne-sensor", 0, },
	{ }
};

struct msensor_i2c_chip_info {
	unsigned gpio_start;
	uint16_t reg_output;
	uint16_t reg_direction;

	struct i2c_client *client;
	struct gpio_chip gpio_chip;
};


static void msensor_timer_handler(unsigned long data)
{
	msensor_dbg("%s\n", __func__);
	if (schedule_work(&msensor_work_q) == 0) {
		msensor_dbg("cannot schedule work !!!\n");
	}
}

static void msensor_timer_registertimer(struct timer_list* ptimer, unsigned int timeover )
{
	msensor_dbg("%s\n", __func__);
	//del_timer_sync(ptimer);
	init_timer(ptimer);
	ptimer->expires = jiffies+msecs_to_jiffies(timeover);
	ptimer->data = (unsigned long)NULL;
	ptimer->function = msensor_timer_handler;

	add_timer(ptimer);
}

static void MSENSOR_SEND_CMD(unsigned char reg, unsigned char val)
{
	unsigned char cmd[2];

	msensor_dbg_d("%s\n", __func__);
	cmd[0] = reg;
	cmd[1] = val;	
//	msensor_dbg("\n MSENSOR_SEND_CMD  : reg= %x val = %x\n", reg, val);

	i2c_master_send(msensor_i2c_client, cmd, 2);
}


static unsigned char MSENSOR_READ_DAT(unsigned char reg)
{
	unsigned char buf;
	unsigned char tmp;

	msensor_dbg_d("%s\n", __func__);	
	tmp = reg;
//	msensor_dbg("\n MSENSOR_READ_DAT  : reg= %x \n", reg);	
	i2c_master_send(msensor_i2c_client, &tmp, 1);
	i2c_master_recv(msensor_i2c_client, &buf, 1);

	return 	buf;
}


static int msensor_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
//	struct sensor_i2c_platform_data 	*pdata;
	struct msensor_i2c_chip_info 	*chip;

	msensor_dbg("\n msensor_i2c_probe  :  %s \n", client->name);

	chip = kzalloc(sizeof(struct msensor_i2c_chip_info), GFP_KERNEL);
	if(chip == NULL)
	{
		msensor_dbg("\n tcc_msensor_i2c  :  no chip info. \n");
		return -ENOMEM;
	}

	chip->client = client;
	i2c_set_clientdata(client, chip);
	//msensor_i2c_client = client;

	return 0;
}

static int msensor_i2c_remove(struct i2c_client *client)
{
//	struct sensor_i2c_platform_data 	*pdata = client->dev.platform_data;
	struct msensor_i2c_chip_info 		*chip  = i2c_get_clientdata(client);
//	int ret = 0;

	kfree(chip);
	//msensor_i2c_client = NULL;
	
	return 0;
}

/* ak8975c i2c codec control layer */
static struct i2c_driver msensor_i2c_driver = {
	.driver = {
		.name	= "tcc-magne-sensor",
		.owner  = THIS_MODULE,
	},
	.probe		= msensor_i2c_probe,
	.remove		= msensor_i2c_remove,
	.id_table	= msensor_i2c_id,
};

static int ak8975c_i2c_register(void)
{
    struct i2c_board_info info;
    struct i2c_adapter *adapter;
    struct i2c_client *client;

    memset(&info, 0, sizeof(struct i2c_board_info));
    info.addr = AK8975C_I2C_ADDRESS;
    strlcpy(info.type, "tcc-magne-sensor", I2C_NAME_SIZE);
#if defined(CONFIG_ARCH_TCC93XX)
    adapter = i2c_get_adapter(3);
#else
    if(machine_is_m57te()){
      msensor_dbg(KERN_INFO "%s : i2c_get_adapter(1), m57te_89\n", __FUNCTION__);
    	adapter = i2c_get_adapter(1);
    }
    else if(machine_is_m801())  // 89_froyo
    {
        msensor_dbg(KERN_INFO "%s : i2c_get_adapter(0), M801_89\n", __FUNCTION__);    	
    	   adapter = i2c_get_adapter(0);
    }
    else{  // 88_froyo (88_93 demo black board)  // M801_8803
        msensor_dbg(KERN_INFO "%s : i2c_get_adapter(0), M801_88 or other\n", __FUNCTION__);
    adapter = i2c_get_adapter(0);
    }
#endif
    if (!adapter) 
    {
        msensor_dbg(KERN_ERR "can't get i2c adapter 0 for tcc-magne-sensor\n");
        return -ENODEV;
    }

    client = i2c_new_device(adapter, &info);
    i2c_put_adapter(adapter);
    if (!client) 
    {
        msensor_dbg(KERN_ERR "can't add i2c device at 0x%x\n", (unsigned int)info.addr);
        return -ENODEV;
    }

    msensor_i2c_client = client;

    return 0;
}

#endif  /* #ifdef CONFIG_I2C */


static int tcc_msensor_get_magne(void)
{
//AK8975C
	signed char		rdy_count = 0, st2 = 0, drdy=0;
	signed char		xm, ym, zm;
	unsigned char   	xl, yl, zl;

	// 1) CNTL wirite (mode = 0001)
	msensor_dbg("%s\n", __func__);
	MSENSOR_SEND_CMD(0x0A, 0x01);   // to Single Measurement Mode

	while((rdy_count<=100)||(drdy==0))
	{
		drdy = MSENSOR_READ_DAT(0x02);  // when data read ready : 1
		// when measurement data register read or ST2 read : 0
		udelay(10);
		rdy_count++; 
	}

	st2 = MSENSOR_READ_DAT(0x09);
	
	// 0x04 DERR : Data Error , out of data readable period, When read : 1,  // When ST2 register read : 0
	// 0x08 HOFL : Magnetic sensor Overflow, When |X|+|Y|+|Z|>2400uT : 1  // when Next Measurement starts :  0
	// 0xC : DERR | HOFL read fail
	if(st2 == 0x4 || st2 == 0x8 || st2 == 0x0C)   
	{
		MSENSOR_SEND_CMD(0x0A, 0x00);   // to to power-down
		msensor_dbg("%s Read Fail\n", __func__);		
		return 1;   // read fail
	}

	xl = MSENSOR_READ_DAT(0x03);	xm = (char) MSENSOR_READ_DAT(0x04);	// X -axis
	yl = MSENSOR_READ_DAT(0x05);	ym = (char) MSENSOR_READ_DAT(0x06);	// Y - axis
	zl = MSENSOR_READ_DAT(0x07);	zm = (char) MSENSOR_READ_DAT(0x08);	// Z - axis

	msensor_dbg("%s: XM: %d, YM: %d, ZM: %d\n", __func__, xm, ym, zm);
	msensor_dbg("%s: XL:%d, YL: %d, ZL: %d\n", __func__, xl, yl, zl);
	tcc_msensor_magne.x = (((int) xm) ) | (xl);
	tcc_msensor_magne.y = ((((int) ym) ) | (yl));
	tcc_msensor_magne.z = (((int) zm) ) | (zl);

	msensor_dbg("%s: %d, %d, %d\n", __func__, tcc_msensor_magne.x, tcc_msensor_magne.y, tcc_msensor_magne.z);
	
	MSENSOR_SEND_CMD(0x0A, 0x00);   // to to power-down
	return 0;
}

static void msensor_fetch_thread(struct work_struct *work)
{
	msensor_dbg("%s: msensor_duration =%d \n", __func__, msensor_duration);
	tcc_msensor_get_magne();
	msensor_timer_registertimer( msensor_timer, msensor_duration );            // test
}

static ssize_t tcc_msensor_write(struct file *file, const char __user *user, size_t size, loff_t *o)
 {
	msensor_dbg("%s\n", __func__);
	return 0;
 }

static ssize_t tcc_msensor_read(struct file *file, char __user *user, size_t size, loff_t *o)
{
	msensor_dbg("%s\n", __func__);	
	msensor_dbg("%s: READ MSENSOR, GET DATA  %d, %d, %d\n", __func__, tcc_msensor_magne.x, tcc_msensor_magne.y, tcc_msensor_magne.z);

	msensor_dbg("%s: msensor_duration =%d \n", __func__, msensor_duration);

	if(copy_to_user(( tcc_sensor_magne_t*) user, (const void *)&tcc_msensor_magne, sizeof( tcc_sensor_magne_t))!=0)
	{
			msensor_dbg("tcc_msensor_read error\n");
	}
	return 0;
}

static long tcc_msensor_ioctl(struct file *filp, unsigned int cmd, void *arg)
{
       int ret = 0;

	msensor_dbg(KERN_INFO "%s  (0x%x)  \n", __FUNCTION__, cmd);

	switch (cmd) {
		case IOCTL_MSENSOR_GET_DATA_MAGNE:    // not used  --> tcc_msensor_read() used
			if(copy_to_user((tcc_sensor_magne_t*)arg, (const void *)&tcc_msensor_magne, sizeof(tcc_sensor_magne_t))!=0)
			{
					msensor_dbg("copy_to error\n");
			}
			msensor_dbg("%s: IOCTL_MSENSOR_GET_DATA_MAGNE x =%d, y=%d, z=%d\n", __func__, tcc_msensor_magne.x, tcc_msensor_magne.y, tcc_msensor_magne.z);
			break;

		case IOCTL_MSENSOR_SET_DELAY_MAGNE:

			if(copy_from_user((void *)&msensor_duration, (unsigned int*) arg, sizeof(unsigned int))!=0)
			{
					msensor_dbg("copy_from error\n");
			}			
			msensor_duration = 200;
			msensor_dbg(KERN_INFO "%s:  IOCTL_MSENSOR_SET_DELAY_MAGNE (0x%x) %d \n", __FUNCTION__, cmd, msensor_duration);			
			break;

		case IOCTL_MSENSOR_GET_STATE_MAGNE:
			msensor_state_flag = 1;
			if(copy_to_user((unsigned int*) arg, (const void *)&msensor_state_flag, sizeof(unsigned int))!=0)
			{
					msensor_dbg("copy_to error\n");
			}			
			msensor_dbg(KERN_INFO "%s: IOCTL_MSENSOR_GET_STATE_MAGNE  (0x%x) %d \n", __FUNCTION__, cmd, msensor_state_flag);	
			break;
			
		case IOCTL_MSENSOR_SET_STATE_MAGNE:
			if(copy_from_user((void *)&msensor_state_flag, (unsigned int*) &arg, sizeof(unsigned int))!=0)
			{
					msensor_dbg("copy_from error\n");
			}
			msensor_dbg(KERN_INFO "%s: IOCTL_MSENSOR_SET_STATE_MAGNE  (0x%x) %d \n", __FUNCTION__, cmd, msensor_state_flag);			
			break;

		default:
			msensor_dbg("msensor: unrecognized ioctl (0x%x)\n", cmd); 
			ret = -EINVAL;
			break;
	}

	return ret;
}


static int tcc_msensor_release(struct inode *inode, struct file *filp)
{
	msensor_dbg(KERN_INFO "%s (%d) tcc_msensor_release\n", __FUNCTION__, msensor_used_count);
	msensor_used_count--;
	if (msensor_used_count < 0) {
		msensor_dbg("msensor: release error (over)\n"); 
		msensor_used_count = 0;
	}

	if (msensor_used_count <= 0) {
		flush_scheduled_work();     
		del_timer_sync( msensor_timer ); 
		//del_timer( msensor_timer);
		kfree( msensor_timer);   
#ifdef CONFIG_I2C
		i2c_unregister_device(msensor_i2c_client);
		i2c_del_driver(&msensor_i2c_driver);
		msensor_i2c_client = NULL;
#endif		
	}
	return 0;
}

static int tcc_msensor_open(struct inode *inode, struct file *filp)
{
    int ret;
    unsigned char old_ctrl;
//    int num = MINOR(inode->i_rdev);

    msensor_dbg("tcc_msensor_open\n");
//    msensor_dbg(KERN_INFO "%s -> minor : %d, (cnt:%d) \n", __FUNCTION__, num, msensor_used_count+1);
    if (msensor_used_count == 0) {

    if(machine_is_m801_88()){  // M801_88 board
        gpio_request(TCC_GPF(8), NULL);
        tcc_gpio_config(TCC_GPF(8), GPIO_FN(0));
        gpio_direction_input(TCC_GPF(8));    
    }

#ifdef CONFIG_I2C
        // Initialize I2C driver for AK8975C
        ret = i2c_add_driver(&msensor_i2c_driver);
        if(ret < 0) 
        {
            msensor_dbg("%s() [Error] failed i2c_add_driver() = %d\n", __func__, ret);
            return ret;
        }
        ret = ak8975c_i2c_register();
        if(ret < 0) 
        {
            msensor_dbg("%s() [Error] Failed register i2c client driver for ak8975c, return is %d\n", __func__, ret);
            return ret;
        }
#endif

        old_ctrl = MSENSOR_READ_DAT(0x00);
        if (old_ctrl == 0xFF)
        {
            msensor_dbg("%s: No such device or address\n", __func__);
#ifdef CONFIG_I2C
            i2c_unregister_device(msensor_i2c_client);
            i2c_del_driver(&msensor_i2c_driver);
            msensor_i2c_client = NULL;
#endif				
            return -ENXIO;
        }
        msensor_dbg("%s: identification : %x\n", __func__, old_ctrl);

        old_ctrl = MSENSOR_READ_DAT(0x01);
        msensor_dbg("%s: ASIC revition ID : %x\n", __func__, old_ctrl);

        msensor_timer= kmalloc( sizeof( struct timer_list ), GFP_KERNEL ); 
        if (msensor_timer == NULL)
        {
            msensor_dbg("%s: mem alloc fail\n", __func__);
#ifdef CONFIG_I2C
            i2c_unregister_device(msensor_i2c_client);
            i2c_del_driver(&msensor_i2c_driver);
            msensor_i2c_client = NULL;
#endif				
            return -ENOMEM;
        }
        memset(msensor_timer, 0, sizeof(struct timer_list));
        msensor_dbg("%s: msensor_duration = %d\n", __func__, msensor_duration);
        msensor_timer_registertimer( msensor_timer, msensor_duration );
    }
    msensor_used_count++;
    return 0;
}

struct file_operations tcc_msensor_fops =
{
	.owner          = THIS_MODULE,
	.open           = tcc_msensor_open,
	.release        = tcc_msensor_release,
	.unlocked_ioctl = tcc_msensor_ioctl,
	.read           = tcc_msensor_read,
	.write          = tcc_msensor_write,	
};

static struct class *msensor_class;

int __init tcc_msensor_init(void)
{
	int res;
	msensor_dbg(KERN_INFO "%s 1 \n", __FUNCTION__);
	res = register_chrdev(MSENSOR_DEV_MAJOR, MSENSOR_DEV_NAME, &tcc_msensor_fops);
	if (res < 0)
		return res;
	
	msensor_dbg(KERN_INFO "%s 2 \n", __FUNCTION__);
	msensor_class = class_create(THIS_MODULE, MSENSOR_DEV_NAME);
	device_create(msensor_class,NULL,MKDEV(MSENSOR_DEV_MAJOR,MSENSOR_DEV_MINOR),NULL,MSENSOR_DEV_NAME);

//	msensor_dbg(KERN_INFO "%s 3 \n", __FUNCTION__);
	INIT_WORK(&msensor_work_q, msensor_fetch_thread);

	msensor_dbg(KERN_INFO "%s\n", __FUNCTION__);
	return 0;
}

void __exit tcc_msensor_exit(void)
{
	msensor_dbg(KERN_INFO "%s\n", __FUNCTION__);
	unregister_chrdev(MSENSOR_DEV_MAJOR, MSENSOR_DEV_NAME);	
}

module_init(tcc_msensor_init);
module_exit(tcc_msensor_exit);

MODULE_AUTHOR("Telechips Inc. c2-g2-2 linux@telechips.com");
MODULE_DESCRIPTION("TCCxxx magne-msensor driver");
MODULE_LICENSE("GPL");

