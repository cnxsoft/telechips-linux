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

#include <mach/tcc_sc_ioctl.h>
#include <mach/tcc_pca953x.h>
#include <mach/gpio.h>

#include<mach/tca_sc.h>

#define DEVICE_NAME		"smartcard"
#define MAJOR_ID		235
#define MINOR_ID		0

static int debug = 0;
#define dprintk(msg...)	if(debug) { printk( "tcc_sc: " msg); }

void tcc_sc_init_port(void)
{
	tca_sc_init_port();
}

void tcc_sc_make_reset(unsigned uReset)
{
	dprintk("%s uReset= %d \n", __func__, uReset);
	tca_sc_make_reset(uReset);
}

void tcc_sc_make_activate(unsigned uActivate)
{
	dprintk("%s, uActivate=%d\n", __func__, uActivate);
	tca_sc_make_activate(uActivate);
}

void tcc_sc_select_voltage(unsigned uVoloate_3V)
{
	dprintk("%s\n", __func__);
	tca_sc_select_voltage(uVoloate_3V);
}

unsigned tcc_sc_detect_card(void)
{
	int ret=0;
	ret = tca_sc_detect_card();
	return ret;
}

int tcc_sc_enable(void)
{
	tca_sc_enable();	
	return 0;
}

int tcc_sc_disable(void)
{
	tca_sc_disable();
	return 0;
}

int tcc_sc_reset(unsigned char *pATR, unsigned *pATRLength)
{
	int iRet = -1;
	iRet = tca_sc_reset(pATR, pATRLength);

	return iRet;
}

int	tcc_sc_send_receive_len(char *pSend, unsigned uSendLength, char *pRcv, unsigned *pRcvLength)
{
	int	iRet = -1;
	iRet = tca_sc_send_receive_len(pSend, uSendLength, pRcv, pRcvLength);
	return iRet;
}

static int tcc_sc_open(struct inode *inode, struct file *filp)
{
	tca_sc_open(inode, filp);
	return 0;
}

static int tcc_sc_read(struct file *filp, unsigned int *buf, size_t count, loff_t *f_pos)
{
	dprintk("%s\n", __func__);
	return 0;
}

static int tcc_sc_write(struct file *filp, unsigned int *buf, size_t count, loff_t *f_pos)
{
	dprintk("%s\n", __func__);
	return 0;
}

static int tcc_sc_ioctl(struct file *file, unsigned int cmd, void *arg)
{
	dprintk("%s, cmd=%d\n", __func__, cmd);
	
	switch(cmd)
	{
		case TCC_SC_IOCTL_ENABLE:
			tcc_sc_enable();
			break;

		case TCC_SC_IOCTL_DISABLE:
			tcc_sc_disable();
			break;
			
		case TCC_SC_IOCTL_ACTIVATE:
			{
				unsigned uActivate;	
				copy_from_user((void *)&uActivate, (const void *)arg, sizeof(unsigned));
				if(uActivate)
				{
					/* Make Reset Low */
					tcc_sc_make_reset(0);
					msleep(60);
					/* Make Activation */
					tcc_sc_make_activate(1);
					msleep(60);
				}
				else
				{
					/* Make Deactivation */
					tcc_sc_make_activate(0);
					/* Make Reset Low */
					tcc_sc_make_reset(0);
				}
			}
			break;

		case TCC_SC_IOCTL_RESET:
			{
				sTCC_SC_BUFFER stScBuffer;	
				copy_from_user((void *)&stScBuffer, (const void *)arg, sizeof(sTCC_SC_BUFFER));
				tcc_sc_reset(stScBuffer.pucReadBuf, stScBuffer.puiReadLength);
			}
			break;
			
		case TCC_SC_IOCTL_SEND_RCV:
			{
				sTCC_SC_BUFFER stScBuffer;	
				copy_from_user((void *)&stScBuffer, (const void *)arg, sizeof(sTCC_SC_BUFFER));
				tcc_sc_send_receive_len(stScBuffer.pucWriteBuf, stScBuffer.uiWriteLength, 
										stScBuffer.pucReadBuf, stScBuffer.puiReadLength);
			}
			break;
			
		case TCC_SC_IOCTL_SET_PARAMS:
			break;

		case TCC_SC_IOCTL_GET_PARAMS:
			{
				sTCC_SC_PARAMS stScParams;	
				copy_from_user((void *)&stScParams, (const void *)arg, sizeof(sTCC_SC_PARAMS));
				memcpy(&stScParams, &stScParams, sizeof(sTCC_SC_PARAMS));
			}
			break;

		case TCC_SC_IOCTL_SELECT_VOL:
			{
				unsigned uVoltage;	
				copy_from_user((void *)&uVoltage, (const void *)arg, sizeof(unsigned));
				tcc_sc_select_voltage(uVoltage);
			}
			break;

		case TCC_SC_IOCTL_DETECT_CARD:
			{
				unsigned uCardDetect = tcc_sc_detect_card(); 
				copy_to_user((unsigned *)arg, &uCardDetect, sizeof(unsigned));
			}
			break;

		default:
			printk("err: unkown command(%d)\n", cmd);
			return -1;
	}

	return 0;
}

static int tcc_sc_close(struct inode *inode, struct file *file)
{
	tca_sc_close(inode, file);
	return 0;
}

static struct file_operations tcc_sc_fops = 
{
	.owner		= THIS_MODULE,
	.open		= tcc_sc_open,
	.read		= tcc_sc_read,
	.write		= tcc_sc_write,
	.unlocked_ioctl	= tcc_sc_ioctl,
	.release	= tcc_sc_close,	
};

static struct class *tcc_sc_class;

static int __init tcc_sc_init(void)
{
	dprintk("%s\n", __func__);

	tca_sc_init();
	
	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_sc_fops);	
	tcc_sc_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(tcc_sc_class, NULL, MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);

	return 0;
}

static void __exit tcc_sc_exit(void)
{
	dprintk("%s\n", __func__);

	tca_sc_exit();
	
	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
	
	return;
}

module_init(tcc_sc_init);
module_exit(tcc_sc_exit);

MODULE_AUTHOR("linux <linux@telechips.com>");
MODULE_DESCRIPTION("Telechips TCC SmartCard driver");
MODULE_LICENSE("GPL");
