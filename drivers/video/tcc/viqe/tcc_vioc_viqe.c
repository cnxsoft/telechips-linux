/* 
 * linux/drivers/char/tcc_viqe.c
 *
 * Author:  <linux@telechips.com>
 * Created: 11th JAN, 2010
 * Description: Telechips Linux VIDEO & IMAGE QUALITY ENHANCER(VIQE) DRIVER
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

#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/fs.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>


#include <mach/tcc_viqe_ioctl.h>
#include <mach/vioc_viqe.h>
#include "tcc_vioc_viqe.h"
#include "tcc_vioc_viqe_interface.h"

#if 0
#define dprintk(msg...)	 { printk( "tcc_viqe: " msg); }
#else
#define dprintk(msg...)	 
#endif


/*****************************************************************************
*
* Functions
*
******************************************************************************/
static int tcc_viqe_DI_init_wrapper(VIQE_DI_TYPE *arg)
{
	VIQE_DI_TYPE viqe_arg;
	
	if(copy_from_user(&viqe_arg, arg, sizeof(VIQE_DI_TYPE)))
		return -EINVAL;

	dprintk("[VIQE Initiaize] LCDC %d, Scaler %d, OnTheFly %d, W:%d, H:%d\n",
		viqe_arg.lcdCtrlNo, viqe_arg.scalerCh, viqe_arg.onTheFly, viqe_arg.srcWidth, viqe_arg.srcHeight);

	TCC_VIQE_DI_Init(viqe_arg.scalerCh, viqe_arg.useWMIXER, viqe_arg.srcWidth, viqe_arg.srcHeight, 
				viqe_arg.crop_top, viqe_arg.crop_bottom, viqe_arg.crop_left, viqe_arg.crop_right, viqe_arg.OddFirst);

	return 0;
}

static int tcc_viqe_DI_excute_wrapper(VIQE_DI_TYPE *arg)
{
	VIQE_DI_TYPE viqe_arg;
	
	if(copy_from_user(&viqe_arg, arg, sizeof(VIQE_DI_TYPE)))
		return -EINVAL;
/*
	dprintk("[VIQE_EXCUTE]scalerCH %d, Address %x %x %x %x %x %x, srcWidth %d, srcHeight %d\nlcdcCH %d, dstAddr %x, M2MMode %d, OddFirst %d, DI %d\n", 
			viqe_arg.scalerCh, viqe_arg.address[0], viqe_arg.address[1], viqe_arg.address[2], viqe_arg.address[3], viqe_arg.address[4], viqe_arg.address[5],
			viqe_arg.srcWidth, viqe_arg.srcHeight, viqe_arg.lcdCtrlNo, viqe_arg.dstAddr, viqe_arg.deinterlaceM2M, viqe_arg.OddFirst, viqe_arg.DI_use);
*/
	TCC_VIQE_DI_Run(viqe_arg.srcWidth, viqe_arg.srcHeight, 
				viqe_arg.crop_top, viqe_arg.crop_bottom, viqe_arg.crop_left, viqe_arg.crop_right, viqe_arg.OddFirst);
	return 0;
}

static int tcc_viqe_DI_deinit_wrapper()
{
	VIQE_DI_TYPE viqe_arg;
	
	TCC_VIQE_DI_DeInit();
	
	return 0;
}

static long tcc_viqe_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	dprintk(KERN_INFO "%s  (0x%x)  \n", __FUNCTION__, cmd);

	switch (cmd) {
		case IOCTL_VIQE_INITIALIZE:
			tcc_viqe_DI_init_wrapper((VIQE_DI_TYPE *)arg);
			break;

		case IOCTL_VIQE_EXCUTE:
			tcc_viqe_DI_excute_wrapper((VIQE_DI_TYPE *)arg);
			break;

		case IOCTL_VIQE_DEINITIALIZE:
			tcc_viqe_DI_deinit_wrapper();
			break;

		default:
			printk("viqe: unrecognized ioctl (0x%x)\n", cmd); 
			ret = -EINVAL;
			break;
	}

	return ret;
}

static int tcc_viqe_release(struct inode *inode, struct file *filp)
{
	printk("%s\n", __func__);
	return 0;
}

static int tcc_viqe_open(struct inode *inode, struct file *filp)
{
	printk("%s\n", __func__);
	return 0;
}


struct file_operations tcc_viqe_fops =
{
	.owner		= THIS_MODULE,
	.open		= tcc_viqe_open,
	.release	= tcc_viqe_release,
	.unlocked_ioctl = tcc_viqe_ioctl,
};

static struct class *viqe_class;

static int __init tcc_viqe_init(void)
{
	int res;

	res = register_chrdev(VIQE_DEV_MAJOR, VIQE_DEV_NAME, &tcc_viqe_fops);
	if (res < 0)
		return res;
	viqe_class = class_create(THIS_MODULE, VIQE_DEV_NAME);
	device_create(viqe_class, NULL, MKDEV(VIQE_DEV_MAJOR, VIQE_DEV_MINOR), NULL, VIQE_DEV_NAME);

	printk(KERN_INFO "%s\n", __FUNCTION__);
	return 0;
}

static void __exit tcc_viqe_exit(void)
{
	unregister_chrdev(VIQE_DEV_MAJOR, VIQE_DEV_NAME);
	printk(KERN_INFO "%s\n", __FUNCTION__);
}

module_init(tcc_viqe_init);
module_exit(tcc_viqe_exit);

MODULE_AUTHOR("Telechips Inc.");
MODULE_DESCRIPTION("TCCxxx viqe driver");
MODULE_LICENSE("GPL");
