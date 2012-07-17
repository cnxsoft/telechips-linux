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

#include "viqe_lib.h"
#include "tcc_viqe.h"
#include "tcc_viqe_interface.h"
#include <mach/tcc_viqe_ioctl.h>

#if 0
#define dprintk(msg...)	 { printk( "tcc_viqe: " msg); }
#else
#define dprintk(msg...)	 
#endif


/*****************************************************************************
*
* Variables
*
******************************************************************************/

int g_dwWidth;
int g_dwHeight;
viqe_proregion_t	g_proRegion;

/*****************************************************************************
*
* Functions
*
******************************************************************************/
static void tcc_viqe_onoff(bool flag)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	viqe_clk_onoff(flag);
}

static void tcc_viqe_InfoInit(viqe_t *arg)
{
	viqe_modes_t	viqe_modes;
	viqe_dma_t		viqe_dma;
	viqe_imgfmt_t	viqe_imgfmt;
	viqe_cfg_t		viqe_cfg;
	viqe_t 			viqe_val;
	
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	
	memset( &viqe_modes, 0, sizeof(viqe_modes_t) );
	memset( &viqe_dma, 0, sizeof(viqe_dma_t) );
	memset( &viqe_imgfmt, 0, sizeof(viqe_imgfmt_t) );				
	memset( &viqe_cfg, 0, sizeof(viqe_cfg_t) );				
	viqe_val.modes = &viqe_modes;
	viqe_val.dma = &viqe_dma;
	viqe_val.imgfmt = &viqe_imgfmt;
	viqe_val.cfg = &viqe_cfg;

	if( !memcpy(&viqe_val, arg, sizeof(viqe_t)) )
		dprintk(KERN_ERR "%s arg get fail!\n", __FUNCTION__);
	
	viqe_init(&viqe_val);

	viqe_val.modes->di = OFF;
	viqe_val.modes->disp = OFF;
	viqe_val.modes->dnts = OFF;
	viqe_val.modes->dnsp = OFF;
	viqe_val.modes->hpf = OFF;
	viqe_val.modes->his = ON;
	viqe_val.modes->gamut = ON;

	viqe_bypass_gamut( ON );

	if ( viqe_val.modes->his )
		viqe_set_histogram_enable( ON, ON, OFF );

	printk("%s viqe_val.width %d, viqe_val.height %d\n", __func__, viqe_val.width, viqe_val.height);
	g_dwWidth = viqe_val.width;
	g_dwHeight = viqe_val.height;

	if( !memcpy(arg, &viqe_val, sizeof(viqe_t)) )
		dprintk(KERN_ERR "%s arg return fail!\n", __FUNCTION__);
}

static void tcc_viqe_SetFrmInfo(viqe_t *arg)
{
	viqe_t 			viqe_val;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	if( !memcpy(&viqe_val, arg, sizeof(viqe_t)) )
		dprintk(KERN_ERR "%s arg get fail!\n", __FUNCTION__);

	ODMAFlush ();
//	printk("[VIQE %s] viqe_ofmt = %d\n", __func__, viqe_val.imgfmt->o_fmt);
	viqe_set_output(viqe_val.cfg, viqe_val.imgfmt->o_fmt, viqe_val.dma);
	ODMARST ();
}

static void tcc_viqe_SetDMA(viqe_dma_t *arg)
{
	 viqe_dma_t		viqe_dma;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	if( !memcpy(&viqe_dma,arg,sizeof(viqe_dma_t)) )
		dprintk(KERN_ERR "%s arg get fail!\n", __FUNCTION__);
	
	viqe_set_dma_addr( &viqe_dma );
	//VIQE_DeintlOnOff (OFF);
	//VIQE_OddfirstOnOff (0);

}

static void tcc_viqe_SetStrength(int *arg)
{
	char ModeName[10];
	viqe_strengthInfo stInfo;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	if( !memcpy(&stInfo,arg,sizeof(viqe_strengthInfo)) )
		dprintk(KERN_ERR "%s arg get fail!\n", __FUNCTION__);
	
	memset( &ModeName, 0, sizeof(ModeName) );
	switch((VIQEMODE)stInfo.Mode)
	{
		case VIQE_MODE_DI:
		case VIQE_MODE_DISP:
			break;
		case VIQE_MODE_DNTS:
			cal_dnts_coeff_simple(stInfo.StrengthY, stInfo.StrengthC);
			cal_dnts_coeff_tmp_simple(stInfo.StrengthY, stInfo.StrengthC);
			sprintf(ModeName, "DNTS");
			break;
		case VIQE_MODE_LPF:
			cal_d2d_lpf_coeff_simple(stInfo.StrengthY);
			sprintf(ModeName, "LPF");
			break;
		case VIQE_MODE_HPF:
			cal_d2d_hpf_coeff_simple(stInfo.StrengthY, stInfo.StrengthC);
			sprintf(ModeName, "HPF");
			break;
		case VIQE_MODE_HIST:
			viqe_set_histogram_scales((stInfo.StrengthY*50+30), NULL);
			sprintf(ModeName, "HIST");
			break;
		default :
			break;
	}

	printk("Strength : %s - strength1(%d), strength2(%d)\n", ModeName, stInfo.StrengthY, stInfo.StrengthC);

}

static void tcc_viqe_SetProcRegion(int *arg)
{
	int eMode;
	int procWidth = g_dwWidth >> 1;
	printk("%s - mode : %x\n", __FUNCTION__, eMode);

	if( !memcpy(&eMode,arg,sizeof(int)) )
		dprintk(KERN_ERR "%s arg get fail!\n", __FUNCTION__);

	switch((VIQEMODE)eMode)
	{
		case VIQE_MODE_DI:
		case VIQE_MODE_DISP:
			g_proRegion.di = g_proRegion.di? OFF : ON;
			viqe_set_deintl_region( g_proRegion.di, 0, procWidth, 0, g_dwHeight);
			break;
		case VIQE_MODE_DNTS:
			g_proRegion.dnts = g_proRegion.dnts? OFF : ON;
			viqe_set_dnts_region( g_proRegion.dnts, ON, procWidth );
			break;
		case VIQE_MODE_LPF:
		case VIQE_MODE_HPF:
			g_proRegion.pf = g_proRegion.pf? OFF : ON;
			viqe_set_dnsp_region( g_proRegion.pf, ON, procWidth );
			break;
		case VIQE_MODE_HIST:
			g_proRegion.hist= g_proRegion.hist? OFF : ON;
			viqe_set_his_region( g_proRegion.hist, 0, procWidth, 0, g_dwHeight );
			break;

		case VIQE_MODE_MAXNUM:
			g_proRegion.di = g_proRegion.di? OFF : ON;
			viqe_set_deintl_region( g_proRegion.di, 0, procWidth, 0, g_dwHeight);

			g_proRegion.dnts = g_proRegion.dnts? OFF : ON;
			viqe_set_dnts_region( g_proRegion.dnts, ON, procWidth );

			g_proRegion.pf = g_proRegion.pf? OFF : ON;
			viqe_set_dnsp_region( g_proRegion.pf, ON, procWidth );

			g_proRegion.hist= g_proRegion.hist? OFF : ON;
			viqe_set_his_region( g_proRegion.hist, 0, procWidth, 0, g_dwHeight );
			
			break;
		default :
			break;

	}
	printk("Half Region Setting : DI-%d, DNTS-%d, LPF/HPF-%d, HIST-%d\n", g_proRegion.di, g_proRegion.dnts, g_proRegion.pf, g_proRegion.hist);

}

static int tcc_viqe_SetMode(viqe_modes_t *arg)
{
	int ret;
	viqe_modes_t viqe_modes;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	memcpy(&viqe_modes, arg, sizeof(viqe_modes_t));

	if(viqe_set_modes( &viqe_modes) == RET_VIQE_EN_ERRREG)
		return RET_VIQE_EN_ERRREG;
	ret = viqe_get_modes(&viqe_modes);

	//printk("%s ret = %d\n", __func__, ret);
	return ret;
}


int tcc_viqe_ctrl(unsigned int cmd, void *arg)
{
	int ret = 0;

	dprintk(KERN_INFO "%s  (0x%x)  \n", __FUNCTION__, cmd);

	switch (cmd) {
		case IOCTL_VIQE_OFF:
			tcc_viqe_onoff(false);
			break;

		case IOCTL_VIQE_ON:
			tcc_viqe_onoff(true);
			break;

		case IOCTL_VIQE_SET_INIT:
			tcc_viqe_InfoInit((viqe_t *)arg);
			break;

		case IOCTL_VIQE_SET_FRMINFO:
			tcc_viqe_SetFrmInfo((viqe_t *)arg);
			break;

		case IOCTL_VIQE_SET_DMA:
			tcc_viqe_SetDMA((viqe_dma_t *)arg);
			break;

		case IOCTL_VIQE_SET_MODE:
			ret = tcc_viqe_SetMode((viqe_modes_t *)arg);
			break;
			
		case IOCTL_VIQE_RESTART:
			viqe_set_frm_restart();
			break;
			
		case IOCTL_VIQE_FINALIZE:
			viqe_finalize();
			break;

		case IOCTL_VIQE_REGION:
			tcc_viqe_SetProcRegion((int *)arg);
			break;

		case IOCTL_VIQE_STRENGTH:
			tcc_viqe_SetStrength((int *)arg);
			break;

		case IOCTL_VIQE_SET_STATE:
			viqe_set_state((int *)arg);
			break;

		case IOCTL_VIQE_SWRESET:
			viqe_swreset();
			break;

		case IOCTL_VIQE_DEINT_BYPASS:
			viqe_set_deintl_bypass((int*)arg);
			break;

		default:
			printk("tcc_viqe_ctrl: unrecognized ioctl (0x%x)\n", cmd); 
			ret = -EINVAL;
			break;
	}
	return ret;
}

static int tcc_viqe_initialize_wrapper(VIQE_DI_TYPE *arg)
{
	VIQE_DI_TYPE viqe_arg;
	
	if(copy_from_user(&viqe_arg, arg, sizeof(VIQE_DI_TYPE)))
		return -EINVAL;

	dprintk("[VIQE Initiaize] LCDC %d, Scaler %d, OnTheFly %d, W:%d, H:%d\n",
		viqe_arg.lcdCtrlNo, viqe_arg.scalerCh, viqe_arg.onTheFly, viqe_arg.srcWidth, viqe_arg.srcHeight);

	TCC_InitalizeVIQE(viqe_arg.lcdCtrlNo, viqe_arg.scalerCh, viqe_arg.onTheFly, viqe_arg.DI_use, viqe_arg.srcWidth, viqe_arg.srcHeight, 
				viqe_arg.crop_top, viqe_arg.crop_bottom, viqe_arg.crop_left, viqe_arg.crop_right);

	return 0;
}

static int tcc_viqe_excute_wrapper(VIQE_DI_TYPE *arg)
{
	VIQE_DI_TYPE viqe_arg;
	
	if(copy_from_user(&viqe_arg, arg, sizeof(VIQE_DI_TYPE)))
		return -EINVAL;

	dprintk("[VIQE_EXCUTE]scalerCH %d, Address %x %x %x %x %x %x, srcWidth %d, srcHeight %d\nlcdcCH %d, dstAddr %x, M2MMode %d, OddFirst %d, DI %d\n", 
			viqe_arg.scalerCh, viqe_arg.address[0], viqe_arg.address[1], viqe_arg.address[2], viqe_arg.address[3], viqe_arg.address[4], viqe_arg.address[5],
			viqe_arg.srcWidth, viqe_arg.srcHeight, viqe_arg.lcdCtrlNo, viqe_arg.dstAddr, viqe_arg.deinterlaceM2M, viqe_arg.OddFirst, viqe_arg.DI_use);

	TCC_ExcuteVIQE(viqe_arg.scalerCh, viqe_arg.address, viqe_arg.srcWidth, viqe_arg.srcHeight,
					viqe_arg.crop_top, viqe_arg.crop_bottom, viqe_arg.crop_left, viqe_arg.crop_right,
					viqe_arg.lcdCtrlNo, viqe_arg.dstAddr, viqe_arg.deinterlaceM2M, viqe_arg.OddFirst, viqe_arg.DI_use);

	return 0;
}

static int tcc_viqe_deinitialize_wrapper(VIQE_DI_TYPE *arg)
{
	VIQE_DI_TYPE viqe_arg;
	
	if(copy_from_user(&viqe_arg, arg, sizeof(VIQE_DI_TYPE)))
		return -EINVAL;

	TCC_DeInitializeVIQE(viqe_arg.lcdCtrlNo, viqe_arg.scalerCh, viqe_arg.DI_use);

	return 0;
}

static long tcc_viqe_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	dprintk(KERN_INFO "%s  (0x%x)  \n", __FUNCTION__, cmd);

	switch (cmd) {
		case IOCTL_VIQE_INITIALIZE:
			tcc_viqe_initialize_wrapper((VIQE_DI_TYPE *)arg);
			break;

		case IOCTL_VIQE_EXCUTE:
			tcc_viqe_excute_wrapper((VIQE_DI_TYPE *)arg);
			break;

		case IOCTL_VIQE_DEINITIALIZE:
			tcc_viqe_deinitialize_wrapper((VIQE_DI_TYPE *)arg);
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
	.owner          = THIS_MODULE,
	.open           = tcc_viqe_open,
	.release        = tcc_viqe_release,
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
