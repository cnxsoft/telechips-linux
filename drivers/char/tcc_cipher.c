/*
 * linux/drivers/serial/tcc_sc.c
 *
 * Author:  <linux@telechips.com>
 * Created: March 18, 2010
 * Description: TCC SmartCard driver
 *
 * Copyright (C) 20010-2011 Telechips 
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
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>

#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/globals.h>

#include <mach/tcc_pca953x.h>
#include <mach/gpio.h>
#include <mach/tcc_cipher_ioctl.h>

#include<mach/tca_cipher.h>


#define DEVICE_NAME		"cipher"
#define MAJOR_ID		250
#define MINOR_ID		0

static int debug = 0;
#define dprintk(msg...)	if(debug) { printk( "tcc_cipher: " msg); }

void tcc_cipher_set_opmode(unsigned uOpMode)
{
	dprintk("%s\n", __func__);
	tca_cipher_set_opmode(uOpMode);
}

void tcc_cipher_set_algorithm(unsigned uAlgorithm, unsigned uArg1, unsigned uArg2)
{
	dprintk("%s\n", __func__);
	tca_cipher_set_algorithm(uAlgorithm, uArg1, uArg2);
}

void tcc_cipher_set_key(unsigned char *pucData, unsigned uLength, unsigned uOption)
{
	dprintk("%s\n", __func__);
	tca_cipher_set_key(pucData, uLength, uOption);
}

void tcc_cipher_set_vector(unsigned char *pucData, unsigned uLength)
{
	dprintk("%s\n", __func__);
	tca_cipher_set_vector(pucData, uLength);
}

int tcc_cipher_encrypt(unsigned char *pucSrcAddr, unsigned char *pucDstAddr, unsigned uLength)
{
	dprintk("%s\n", __func__);
	tca_cipher_encrypt(pucSrcAddr, pucDstAddr, uLength);
	return 0;
}

int tcc_cipher_decrypt(unsigned char *pucSrcAddr, unsigned char *pucDstAddr, unsigned uLength)
{
	dprintk("%s\n", __func__);
	tca_cipher_decrypt(pucSrcAddr, pucDstAddr, uLength);
	return 0;
}

static int tcc_cipher_read(struct file *filp, unsigned int *buf, size_t count, loff_t *f_pos)
{
	dprintk("%s\n", __func__);
	return 0;
}

static int tcc_cipher_write(struct file *filp, unsigned int *buf, size_t count, loff_t *f_pos)
{
	dprintk("%s\n", __func__);
	return 0;
}

static int tcc_cipher_ioctl(struct file *file, unsigned int cmd, void *arg)
{
	dprintk("%s, cmd=%d\n", __func__, cmd);
	
	switch(cmd)
	{
		case TCC_CIPHER_IOCTL_SET_ALGORITHM:
			{
				stCIPHER_ALGORITHM stAlgorithm;

				copy_from_user((void *)&stAlgorithm, (const void *)arg, sizeof(stCIPHER_ALGORITHM));
				tcc_cipher_set_opmode(stAlgorithm.uOperationMode);
				tcc_cipher_set_algorithm(stAlgorithm.uAlgorithm, stAlgorithm.uArgument1, stAlgorithm.uArgument2);
			}
			break;

		case TCC_CIPHER_IOCTL_SET_KEY:
			{
				stCIPHER_KEY stKeyInfo;

				copy_from_user((void *)&stKeyInfo, (const void *)arg, sizeof(stCIPHER_KEY));
				tcc_cipher_set_key(stKeyInfo.pucData, stKeyInfo.uLength, stKeyInfo.uOption);
			}
			break;
			
		case TCC_CIPHER_IOCTL_SET_VECTOR:
			{
				stCIPHER_VECTOR stVectorInfo;

				copy_from_user((void *)&stVectorInfo, (const void *)arg, sizeof(stCIPHER_VECTOR));
				tcc_cipher_set_vector(stVectorInfo.pucData, stVectorInfo.uLength);
			}
			break;
			
		case TCC_CIPHER_IOCTL_ENCRYPT:
			{
				stCIPHER_ENCRYPTION stEncryptInfo;

				copy_from_user((void *)&stEncryptInfo, (const void *)arg, sizeof(stCIPHER_ENCRYPTION));
				tcc_cipher_encrypt(stEncryptInfo.pucSrcAddr, stEncryptInfo.pucDstAddr, stEncryptInfo.uLength);
			}
			break;
			
		case TCC_CIPHER_IOCTL_DECRYPT:
			{
				stCIPHER_DECRYPTION stDecryptInfo;

				copy_from_user((void *)&stDecryptInfo, (const void *)arg, sizeof(stCIPHER_DECRYPTION));
				tcc_cipher_decrypt(stDecryptInfo.pucSrcAddr, stDecryptInfo.pucDstAddr, stDecryptInfo.uLength);
			}
			break;
			
		default:
			printk("err: unkown command(%d)\n", cmd);
			return -1;
	}

	return 0;
}

static int tcc_cipher_open(struct inode *inode, struct file *filp)
{
	tca_cipher_open(inode, filp);
	return 0;
}

static int tcc_cipher_release(struct inode *inode, struct file *file)
{
	tca_cipher_release(inode, file);
	return 0;
}

static struct file_operations tcc_cipher_fops = 
{
	.owner		= THIS_MODULE,
	.open		= tcc_cipher_open,
	.read		= tcc_cipher_read,
	.write		= tcc_cipher_write,
	.unlocked_ioctl	= tcc_cipher_ioctl,
	.release	= tcc_cipher_release,	
};

static struct class *tcc_cipher_class;

static int __init tcc_cipher_init(void)
{
	dprintk("%s\n", __func__);
	
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_cipher_fops);	
	tcc_cipher_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(tcc_cipher_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);

	return 0;
}

static void __exit tcc_cipher_exit(void)
{
	dprintk("%s\n", __func__);
	
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
	
	return;
}

module_init(tcc_cipher_init);
module_exit(tcc_cipher_exit);

MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("Telechips TCC CIPHER driver");
MODULE_LICENSE("GPL");
