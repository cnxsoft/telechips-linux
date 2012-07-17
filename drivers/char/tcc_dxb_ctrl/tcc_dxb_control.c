/* 
 * linux/drivers/char/tcc_dxb/ctrl/tcc_dxb_control.c
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

#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include "tcc_dxb_control.h"

static struct device *pdev_dxb = NULL;

static long tcc_dxb_ctrl_ioctl(struct file *filp, 
							unsigned int cmd, unsigned long arg)
{
	int ret;
	unsigned int deviceIdx;
	unsigned int uiboardtype;	
	unsigned int uirfpathctrl;
	ST_CTRLINFO_ARG stCtrlInfo;

	struct tcc_dxb_platform_data *dxb_dev;
	dxb_dev = (struct tcc_dxb_platform_data *)pdev_dxb->platform_data;

	switch (cmd) {
	case IOCTL_DXB_CTRL_OFF:
		if(arg!=0)
		{
			deviceIdx = *(unsigned int *)arg;
			dxb_dev->power_off(deviceIdx);
		}
		else
		{
			dxb_dev->power_off(0);
		}
		break;
	case IOCTL_DXB_CTRL_ON:
		if(arg!=0)
		{
			deviceIdx = *(unsigned int *)arg;
			dxb_dev->power_on(deviceIdx);
		}
		else
		{
			dxb_dev->power_on(0);
		}
		break;
	case IOCTL_DXB_CTRL_RF_PATH:
		ret = copy_from_user((void*)&uirfpathctrl, (const void*)arg, sizeof(unsigned long));
		dxb_dev->rf_path(uirfpathctrl);
		break;
	case IOCTL_DXB_CTRL_RESET:
		if(arg!=0)
		{
			deviceIdx = *(unsigned int *)arg;
			dxb_dev->power_reset(deviceIdx);
		}
		else
		{
			dxb_dev->power_reset(0);
		}
		break;
	case IOCTL_DXB_CTRL_SET_BOARD:
		ret = copy_from_user((void *)&uiboardtype, (const void *)arg, sizeof(unsigned long));			
		if(uiboardtype < BOARD_MAX)
		{
			dxb_dev->set_board(uiboardtype);
		}
		break;
	case IOCTL_DXB_CTRL_GET_CTLINFO:
		dxb_dev->get_info(&stCtrlInfo);
		ret = copy_to_user((ST_CTRLINFO_ARG *)arg, &stCtrlInfo, sizeof(ST_CTRLINFO_ARG));
		break;
	default:
		printk("bl: unrecognized ioctl (0x%x)\n", cmd); 
		return -EINVAL;
		break;
	}

	return 0;
}

static int tcc_dxb_ctrl_release(struct inode *inode, struct file *filp)
{
//	printk(KERN_INFO "%s::%d\n", __FUNCTION__, __LINE__);
	return 0;
}

static int tcc_dxb_ctrl_open(struct inode *inode, struct file *filp)
{
//	printk(KERN_INFO "%s::%d\n", __FUNCTION__, __LINE__);
	return 0;
}

struct file_operations tcc_dxb_ctrl_fops =
{
	.owner          = THIS_MODULE,
	.open           = tcc_dxb_ctrl_open,
	.release        = tcc_dxb_ctrl_release,
	.unlocked_ioctl = tcc_dxb_ctrl_ioctl,
};

static struct class *tcc_dxb_ctrl_class;

static int tcc_dxb_probe(struct platform_device *pdev)
{
	int res;
	struct tcc_dxb_platform_data *dxb_dev;

	res = register_chrdev(DXB_CTRL_DEV_MAJOR, DXB_CTRL_DEV_NAME, &tcc_dxb_ctrl_fops);
	if (res < 0)
		return res;
	tcc_dxb_ctrl_class = class_create(THIS_MODULE, DXB_CTRL_DEV_NAME);
	if(NULL == device_create(tcc_dxb_ctrl_class, NULL, MKDEV(DXB_CTRL_DEV_MAJOR, DXB_CTRL_DEV_MINOR), NULL, DXB_CTRL_DEV_NAME))
		printk(KERN_INFO "%s device_create failed\n", __FUNCTION__);	

	pdev_dxb = &pdev->dev;

	dxb_dev = (struct tcc_dxb_platform_data *)pdev_dxb->platform_data;

	if(dxb_dev->init != NULL)
		dxb_dev->init();
	
	return 0;
}

static int tcc_dxb_remove(struct platform_device *pdev)
{
	device_destroy(tcc_dxb_ctrl_class, MKDEV(DXB_CTRL_DEV_MAJOR, DXB_CTRL_DEV_MINOR));
	class_destroy(tcc_dxb_ctrl_class);
	unregister_chrdev(DXB_CTRL_DEV_MAJOR, DXB_CTRL_DEV_NAME);

	return 0;
}

static struct platform_driver tcc_dxb_ctrl = {
	.probe	= tcc_dxb_probe,
	.remove	= tcc_dxb_remove,
	.driver	= {
		.name	= DXB_CTRL_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init tcc_dxb_ctrl_init(void)
{
	return platform_driver_register(&tcc_dxb_ctrl);
}

static void __exit tcc_dxb_ctrl_exit(void)
{
	platform_driver_register(&tcc_dxb_ctrl);
}

module_init(tcc_dxb_ctrl_init);
module_exit(tcc_dxb_ctrl_exit);

MODULE_AUTHOR("Telechips Inc.");
MODULE_DESCRIPTION("TCC Broadcasting Control(Power, Reset..)");
MODULE_LICENSE("GPL");
