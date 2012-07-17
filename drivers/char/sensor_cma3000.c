/* 
 * linux/drivers/char/tcc_backlight.c
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
#include <asm/uaccess.h>

//#include <asm/plat-tcc/tca_backlight.h>

//#if defined(CONFIG_ARCH_TCC92X)
#include <mach/bsp.h>
//#endif

#ifdef CONFIG_I2C
#   include <linux/i2c.h>
#endif

#define SENSOR_DEV_NAME			"sensor"
#define SENSOR_DEV_MAJOR		249
#define SENSOR_DEV_MINOR		1

#define IOCTL_SENSOR_GET_ACCEL	0
#define IOCTL_SENSOR_SET_DELAY	1

typedef struct {
	int x;
	int y;
	int z;
	int resolution;
} tcc_sensor_accel_t;

#ifdef CONFIG_I2C
static unsigned short probe[] = { 1, 0x1C, I2C_CLIENT_END };
static unsigned short dummy[] = {I2C_CLIENT_END};

static struct i2c_client_address_data addr_data = {
	.normal_i2c = dummy,
	.probe      = probe,
	.ignore     = dummy,
};

static struct i2c_driver sensor_i2c_driver;
static struct i2c_client *sensor_i2c_client;

static struct timer_list *sensor_timer;
static int sensor_used_count=0;
volatile static unsigned int sensor_duration=200;
volatile static tcc_sensor_accel_t tcc_sensor_accel;
static struct work_struct sensor_work_q;

static void sensor_timer_handler(unsigned long data)
{
	if (schedule_work(&sensor_work_q) == 0) {
		printk("cannot schedule work !!!\n");
	}
}

static void sensor_timer_registertimer(struct timer_list* ptimer, unsigned int timeover )
{
	//del_timer_sync(ptimer);
	init_timer(ptimer);
	ptimer->expires = jiffies+msecs_to_jiffies(timeover);
	ptimer->data = (unsigned long)NULL;
	ptimer->function = sensor_timer_handler;

	add_timer(ptimer);
}

static void SENSOR_SEND_CMD(unsigned char reg, unsigned char val)
{
	unsigned char cmd[2];
	cmd[0] = reg;
	cmd[1] = val;	
	i2c_master_send(sensor_i2c_client, cmd, 2);
}


static unsigned char SENSOR_READ_DAT(unsigned char reg)
{
	unsigned char buf;

	i2c_master_send(sensor_i2c_client, &reg, 1);
	i2c_master_recv(sensor_i2c_client, &buf, 1);

	return 	buf;
}

static int sensor_i2c_probe(struct i2c_adapter *adap, int addr, int kind)
{
    struct i2c_client *i2c;
    int ret;
	unsigned char old_ctrl, new_ctrl;

    i2c = kzalloc(sizeof(*i2c), GFP_KERNEL);
    if (!i2c)
   	{		
        return -ENOMEM;
   	}

    strcpy(i2c->name, "ACCEL_SENSOR");
    i2c->flags = 0;
    i2c->addr = addr;
    i2c->adapter = adap;
    i2c->driver = &sensor_i2c_driver;

    sensor_i2c_client = i2c;

    /* attach i2c client device */
    ret = i2c_attach_client(i2c);
    if (ret < 0) {
        printk("%s: failed to attach codec at addr %x\n", __func__, addr);
        goto err;
    }

	HwGPIOD->GPEN	&= ~Hw19;
	//HwGPIOD->GPDAT	|= Hw19;

	old_ctrl = SENSOR_READ_DAT(0x00);
	printk("%s: identification : %x\n", __func__, old_ctrl);
	old_ctrl = SENSOR_READ_DAT(0x01);
	printk("%s: ASIC revition ID : %x\n", __func__, old_ctrl);

	old_ctrl = SENSOR_READ_DAT(0x02);
	new_ctrl = (0x80|0x00|0x00|0x00|0x02|0x00);	// range, level, exit, , mode, int_s
	SENSOR_SEND_CMD(0x02, new_ctrl);

    return ret;
err:
    kfree(i2c);
    return ret;
}

static int sensor_i2c_detach(struct i2c_client *client)
{
    i2c_detach_client(client);
    kfree(sensor_i2c_client);
    return 0;
}

static int sensor_i2c_attach(struct i2c_adapter *adap)
{
    return i2c_probe(adap, &addr_data, sensor_i2c_probe);
}

/* synaptics i2c codec control layer */
static struct i2c_driver sensor_i2c_driver = {
    .driver = {
        .name   = "tcc-accel-sensor",
        .owner  = THIS_MODULE,
    },
    .id             = I2C_DRIVERID_ACCEL_SENSOR,
    .attach_adapter = sensor_i2c_attach,
    .detach_client  = sensor_i2c_detach,
    .command        = NULL,
};
#endif

static void sensor_openi2c(void)
{
#ifdef CONFIG_I2C
	int ret = 0;

	ret = i2c_add_driver(&sensor_i2c_driver);
	if (ret != 0)
		printk("%s: can't add i2c driver\n", __func__);
#endif
}

static int tcc_sensor_get_accel(void)
{
	int x, y, z;
	
	x = (int)SENSOR_READ_DAT(0x06);
	y = (int)SENSOR_READ_DAT(0x07);
	z = (int)SENSOR_READ_DAT(0x08);

	if (x >= 128)
		x = - (256 - x);

	if (y >= 128)
		y = - (256 - y);

	if (z >= 128)
		z = - (256 - z);

	tcc_sensor_accel.x = x/5;
	tcc_sensor_accel.y = y/5;
	tcc_sensor_accel.z = z/5;

	//printk("%s: %d, %d, %d\n", __func__, tcc_sensor_accel.x, tcc_sensor_accel.y, tcc_sensor_accel.z);

	return 0;
}

static void sensor_fetch_thread(struct work_struct *work)
{
	tcc_sensor_get_accel();
	sensor_timer_registertimer( sensor_timer, sensor_duration );
}

static long tcc_sensor_ioctl(struct file *filp, 
							unsigned int cmd, void *arg)
{
	if (!sensor_used_count)
		return -1;

	//printk(KERN_INFO "%s  (0x%x)  \n", __FUNCTION__, cmd);

	switch (cmd) {
		case IOCTL_SENSOR_GET_ACCEL:
			copy_to_user((tcc_sensor_accel_t*) arg, &tcc_sensor_accel, sizeof(tcc_sensor_accel_t));
			break;

		case IOCTL_SENSOR_SET_DELAY:
			printk("%s  (0x%x) %d \n", __FUNCTION__, cmd, sensor_duration);
			copy_from_user(&sensor_duration, (unsigned int*) arg, sizeof(unsigned int));
			break;

		default:
			printk("sensor: unrecognized ioctl (0x%x)\n", cmd); 
			return -EINVAL;
			break;
	}

	return 0;
}

static int tcc_sensor_release(struct inode *inode, struct file *filp)
{
	printk("%s (%d)\n", __FUNCTION__, sensor_used_count);
	sensor_used_count--;
	if (sensor_used_count < 0) {
		printk("sensor: release error (over)\n"); 
		sensor_used_count = 0;
	}

	if (sensor_used_count <= 0) {
		flush_scheduled_work();
		del_timer_sync( sensor_timer );
		//del_timer( sensor_timer);
		kfree( sensor_timer);
		i2c_del_driver(&sensor_i2c_driver);
	}
	return 0;
}

static int tcc_sensor_open(struct inode *inode, struct file *filp)
{
	unsigned char old_ctrl, new_ctrl;
	int num = MINOR(inode->i_rdev);
	
	printk("%s -> minor : %d, (cnt:%d)\n", __FUNCTION__, num, sensor_used_count+1);
	if (sensor_used_count == 0) {
		sensor_openi2c();

		//synaptics_io_init();
		old_ctrl = SENSOR_READ_DAT(0x00);
		if (old_ctrl == 0xFF) {
			i2c_del_driver(&sensor_i2c_driver);

			printk("%s: No such device or address\n", __func__);
			return -ENXIO;
		}
		printk("%s: identification : %x\n", __func__, old_ctrl);

		old_ctrl = SENSOR_READ_DAT(0x01);
		printk("%s: ASIC revition ID : %x\n", __func__, old_ctrl);

		old_ctrl = SENSOR_READ_DAT(0x02);
		new_ctrl = (0x80|0x00|0x00|0x00|0x02|0x00);	// range, level, exit, , mode, int_s
		SENSOR_SEND_CMD(0x02, new_ctrl);
		sensor_timer= kmalloc( sizeof( struct timer_list ), GFP_KERNEL );
		if (sensor_timer == NULL) return -ENOMEM;
		memset(sensor_timer, 0, sizeof(struct timer_list));
		sensor_timer_registertimer( sensor_timer, sensor_duration );
	}
	
	sensor_used_count++;	
	return 0;
}

struct file_operations tcc_sensor_fops =
{
	.owner		= THIS_MODULE,
	.open		= tcc_sensor_open,
	.release	= tcc_sensor_release,
	.ioctl		= tcc_sensor_ioctl,
};

static struct class *sensor_class;

int __init tcc_sensor_init(void)
{
	int res;

	res = register_chrdev(SENSOR_DEV_MAJOR, SENSOR_DEV_NAME, &tcc_sensor_fops);
	if (res < 0)
		return res;

	sensor_class = class_create(THIS_MODULE, SENSOR_DEV_NAME);
	device_create(sensor_class,NULL,MKDEV(SENSOR_DEV_MAJOR,SENSOR_DEV_MINOR),NULL,SENSOR_DEV_NAME);

	INIT_WORK(&sensor_work_q, sensor_fetch_thread);
	//init_waitqueue_head(&(tsc2003_ts->wait_q));
	//mutex_init(&(tcc_sensor->mutex));

	printk(KERN_INFO "%s\n", __FUNCTION__);
	return 0;
}

void __exit tcc_sensor_exit(void)
{
	unregister_chrdev(SENSOR_DEV_MAJOR, SENSOR_DEV_NAME);
    printk(KERN_INFO "%s\n", __FUNCTION__);
}

module_init(tcc_sensor_init);
module_exit(tcc_sensor_exit);

MODULE_AUTHOR("Telechips Inc. SYS4-3 linux@telechips.com");
MODULE_DESCRIPTION("TCCxxx 3axis-sensor driver");
MODULE_LICENSE("GPL");

