#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <mach/bsp.h>
#include <mach/vioc_viqe.h>
#include <mach/globals.h>

#if 0
#define dprintk(msg...)	 { printk( "Tcc892x_viqe: " msg); }
#else
#define dprintk(msg...)	 
#endif

/******************************* VIQE Control *******************************/
void VIOC_VIQE_SetImageSize(VIQE *pVIQE, unsigned int width, unsigned int height)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cVIQE_CTRL.nVIQE_CTRL_SIZE.bREG.width	= width;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL_SIZE.bREG.height	= height;
	*/
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL_SIZE.nREG, 0x07FF07FF, (height << 16)|(width << 0));

}

void VIOC_VIQE_SetImageY2RMode(VIQE *pVIQE, unsigned int y2r_mode)
{
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL_Y2R.nREG, 0x3<<9, y2r_mode << 9);
}
void VIOC_VIQE_SetImageY2REnable(VIQE *pVIQE, unsigned int enable)
{
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL_Y2R.nREG, 1<<8, enable << 8);
}

void VIOC_VIQE_SetControlMisc(VIQE *pVIQE, unsigned int no_hor_intpl, unsigned int fmt_conv_disable, unsigned int fmt_conv_disable_using_fmt, unsigned int update_disable, unsigned int cfgupd, unsigned int h2h)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.no_hor_intpl				= no_hor_intpl;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.fmt_conv_disable			= fmt_conv_disable;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.fmt_conv_disable_using_fmt 	= fmt_conv_disable_using_fmt;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.update_disable			= update_disable;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.cfgupd					= cfgupd;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL_Y2R.bREG.h2h					= h2h;
	*/
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL.nREG, 0x00330100, 
								(cfgupd << 21)
								|(update_disable << 20) 
								|(fmt_conv_disable_using_fmt << 17) 
								|(fmt_conv_disable << 16) 
								|(no_hor_intpl << 8));
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL_Y2R.nREG, 0xFF, h2h);

}

void VIOC_VIQE_SetControlDontUse(VIQE *pVIQE, unsigned int global_en_dont_use, unsigned int top_size_dont_use, unsigned int stream_deintl_info_dont_use)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cVIQE_CTRL.nVIQE_CTRL_MISC.bREG.global_en_dont_use			= global_en_dont_use;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL_MISC.bREG.top_size_dont_use			= top_size_dont_use;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL_MISC.bREG.stream_deintl_info_dont_use	= stream_deintl_info_dont_use;
	*/
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL_MISC.nREG, 0x7, 
								(stream_deintl_info_dont_use << 2) 
								|(top_size_dont_use << 1) 
								|(global_en_dont_use << 0));	
}

void VIOC_VIQE_SetControlClockGate(VIQE *pVIQE, unsigned int deintl_dis, unsigned int d3d_dis, unsigned int pm_dis)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.clkgate_deintl_disable	= deintl_dis;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.clkgate_d3d_disable	= d3d_dis;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.clkgate_pm_disable		= pm_dis;
	*/
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL.nREG, 0x07000000, 
								(pm_dis << 26) 
								|(d3d_dis << 25) 
								|(deintl_dis << 24));	
}

void VIOC_VIQE_SetControlEnable(VIQE *pVIQE, unsigned int his_cdf_or_lut_en, unsigned int his_en, unsigned int gamut_en, unsigned int denoise3d_en, unsigned int deintl_en)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cDEINTL_DMA.nDEINTL_CTRL.bREG.enable 			= deintl_en;
	pVIQE->cDEINTL_DECOMP0.nVIQE_FC_CTRL.bREG.fc_enable	= deintl_en;
	pVIQE->cDEINTL_DECOMP1.nVIQE_FC_CTRL.bREG.fc_enable	= deintl_en;
	pVIQE->cDEINTL_DECOMP2.nVIQE_FC_CTRL.bREG.fc_enable	= deintl_en;
	pVIQE->cDEINTL_COMP.nVIQE_FC_CTRL.bREG.fc_enable 		= deintl_en;

	pVIQE->cD3D_DMA.nD3D_CTRL.bREG.enable					= denoise3d_en;
	pVIQE->cD3D_DECOMP.nVIQE_FC_CTRL.bREG.fc_enable		= denoise3d_en;
	pVIQE->cD3D_COMP.nVIQE_FC_CTRL.bREG.fc_enable 			= denoise3d_en;

	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.his_cdf_or_lut_en		= his_cdf_or_lut_en;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.his_en 				= his_en;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.gamut_en			= gamut_en;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.denoise3d_en			= denoise3d_en;
	pVIQE->cVIQE_CTRL.nVIQE_CTRL.bREG.deintl_en				= deintl_en;
	*/
	BITCSET(pVIQE->cDEINTL_DECOMP0.nVIQE_FC_CTRL.nREG, (0x1<<31), (deintl_en << 31));	
	BITCSET(pVIQE->cDEINTL_DECOMP1.nVIQE_FC_CTRL.nREG, (0x1<<31), (deintl_en << 31));	
	BITCSET(pVIQE->cDEINTL_DECOMP2.nVIQE_FC_CTRL.nREG, (0x1<<31), (deintl_en << 31));	
	BITCSET(pVIQE->cDEINTL_COMP.nVIQE_FC_CTRL.nREG, (0x1<<31), (deintl_en << 31));	

	BITCSET(pVIQE->cD3D_DMA.nD3D_CTRL.nREG, (0x1<<16), (denoise3d_en<<16));
	BITCSET(pVIQE->cD3D_DECOMP.nVIQE_FC_CTRL.nREG, (0x1<<31), (denoise3d_en << 31));	
	BITCSET(pVIQE->cD3D_COMP.nVIQE_FC_CTRL.nREG, (0x1<<31), (denoise3d_en << 31));	

	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL.nREG, 0x1F, 
								(deintl_en << 4)
								|(denoise3d_en << 3) 
								|(gamut_en << 2) 
								|(his_en << 1) 
								|(his_cdf_or_lut_en << 0));
	
}

void VIOC_VIQE_SetControlMode(VIQE *pVIQE, unsigned int his_cdf_or_lut_en, unsigned int his_en, unsigned int gamut_en, unsigned int denoise3d_en, unsigned int deintl_en)
{
	BITCSET(pVIQE->cVIQE_CTRL.nVIQE_CTRL.nREG, 0x1F, 
								(deintl_en << 4)
								|(denoise3d_en << 3) 
								|(gamut_en << 2) 
								|(his_en << 1) 
								|(his_cdf_or_lut_en << 0));
}

void VIOC_VIQE_SetControlRegister(VIQE *pVIQE, unsigned int width, unsigned int height, unsigned int fmt)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	VIOC_VIQE_SetImageSize(pVIQE, width, height);
	VIOC_VIQE_SetControlMisc(pVIQE, OFF, OFF, ON, OFF, ON, 0x16);	/* All of variables are the recommended value */
	VIOC_VIQE_SetControlDontUse(pVIQE, OFF, OFF, OFF);				/* All of variables are the recommended value */
	VIOC_VIQE_SetControlClockGate(pVIQE, OFF, OFF, OFF);			/* All of variables are the recommended value */
}

/******************************* DI Control *******************************/
void VIOC_VIQE_SetDeintlBase(VIQE *pVIQE, unsigned int frmnum, unsigned int base0, unsigned int base1, unsigned int base2, unsigned int base3)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	if (frmnum == 0)
	{
	/*
		pVIQE->cDEINTL_DMA.nDEINTL_BASE0.nREG	= base0;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE1.nREG	= base1;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE2.nREG	= base2;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE3.nREG	= base3;
	*/
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE0.nREG, 0xFFFFFFFF, base0);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE1.nREG, 0xFFFFFFFF, base1);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE2.nREG, 0xFFFFFFFF, base2);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE3.nREG, 0xFFFFFFFF, base3);	
	}
	else if (frmnum == 1)
	{
	/*
		pVIQE->cDEINTL_DMA.nDEINTL_BASE0A.nREG	= base0;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE1A.nREG	= base1;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE2A.nREG	= base2;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE3A.nREG	= base3;
	*/
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE0A.nREG, 0xFFFFFFFF, base0);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE1A.nREG, 0xFFFFFFFF, base1);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE2A.nREG, 0xFFFFFFFF, base2);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE3A.nREG, 0xFFFFFFFF, base3);	
	}
	else if (frmnum == 2)
	{
	/*
		pVIQE->cDEINTL_DMA.nDEINTL_BASE0B.nREG	= base0;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE1B.nREG	= base1;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE2B.nREG	= base2;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE3B.nREG	= base3;
	*/
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE0B.nREG, 0xFFFFFFFF, base0);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE1B.nREG, 0xFFFFFFFF, base1);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE2B.nREG, 0xFFFFFFFF, base2);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE3B.nREG, 0xFFFFFFFF, base3);	
	}
	else
	{
	/*
		pVIQE->cDEINTL_DMA.nDEINTL_BASE0C.nREG	= base0;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE1C.nREG	= base1;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE2C.nREG	= base2;
		pVIQE->cDEINTL_DMA.nDEINTL_BASE3C.nREG	= base3;
	*/
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE0C.nREG, 0xFFFFFFFF, base0);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE1C.nREG, 0xFFFFFFFF, base1);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE2C.nREG, 0xFFFFFFFF, base2);
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE3C.nREG, 0xFFFFFFFF, base3);	
	}
	
}

void VIOC_VIQE_SwapDeintlBase(VIQE *pVIQE, int mode)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	unsigned int	curr_viqe_base[4];
	unsigned int	next_viqe_base[4];

	curr_viqe_base[3] = pVIQE->cDEINTL_DMA.nDEINTL_BASE3.nREG ;
	curr_viqe_base[2] = pVIQE->cDEINTL_DMA.nDEINTL_BASE2.nREG ;
	curr_viqe_base[1] = pVIQE->cDEINTL_DMA.nDEINTL_BASE1.nREG ;
	curr_viqe_base[0] = pVIQE->cDEINTL_DMA.nDEINTL_BASE0.nREG ;

	switch (mode) {
		case	DUPLI_MODE :
			next_viqe_base[3] = curr_viqe_base[2];
			next_viqe_base[2] = curr_viqe_base[1];
			next_viqe_base[1] = curr_viqe_base[0];
			next_viqe_base[0] = curr_viqe_base[3];
			break;
		case	SKIP_MODE :
			next_viqe_base[3] = curr_viqe_base[2];//[0];
			next_viqe_base[2] = curr_viqe_base[1];//[3];
			next_viqe_base[1] = curr_viqe_base[0];//[2];
			next_viqe_base[0] = curr_viqe_base[3];//[1];
			break;
		default :
			next_viqe_base[3] = curr_viqe_base[3];
			next_viqe_base[2] = curr_viqe_base[2];
			next_viqe_base[1] = curr_viqe_base[1];
			next_viqe_base[0] = curr_viqe_base[0];
			break;
	}

	BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE3.nREG, 0xFFFFFFFF, next_viqe_base[3]) ;
	BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE2.nREG, 0xFFFFFFFF, next_viqe_base[2]) ;
	BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE1.nREG, 0xFFFFFFFF, next_viqe_base[1]) ;
	BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_BASE0.nREG, 0xFFFFFFFF, next_viqe_base[0]);
}

void VIOC_VIQE_SetDeintlSize(VIQE *pVIQE, unsigned int width, unsigned int height)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cDEINTL_DMA.nDEINTL_SIZE.bREG.width		= width;
	pVIQE->cDEINTL_DMA.nDEINTL_SIZE.bREG.height	= height >> 1;
	*/
	BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_SIZE.nREG, 0x07FF07FF, ((height>>1) << 16)|(width << 0));
}

void VIOC_VIQE_SetDeintlMisc(VIQE *pVIQE, unsigned int uvintpl, unsigned int cfgupd, unsigned int dma_enable, unsigned int h2h, unsigned int top_size_dont_use)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cDEINTL_DMA.nDEINTL_CTRL.bREG.uvintpl			= uvintpl;
	pVIQE->cDEINTL_DMA.nDEINTL_CTRL.bREG.cfgupd			= cfgupd;
	pVIQE->cDEINTL_DMA.nDEINTL_CTRL.bREG.h2h				= h2h;
	pVIQE->cDEINTL_DMA.nDEINTL_CTRL.bREG.top_size_dont_use	= top_size_dont_use;
	*/
	BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_CTRL.nREG, 0xFF030003, 
								(h2h << 24) 
								|(cfgupd << 17) 
								|(dma_enable << 16) 			//DI DMA enable
								|(uvintpl << 2) 
								|(top_size_dont_use << 0));
}

void VIOC_VIQE_SetDeintlControl(VIQE *pVIQE, unsigned int fmt, unsigned int eof_control_ready, unsigned int dec_divisor, unsigned int ac_k0_limit, unsigned int ac_k1_limit, unsigned int ac_k2_limit)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cDEINTL_DECOMP0.nVIQE_FC_MISC.bREG.eof_control_ready	= eof_control_ready;
	pVIQE->cDEINTL_DECOMP0.nVIQE_FC_MISC.bREG.divisor			= dec_divisor;
	pVIQE->cDEINTL_DECOMP1.nVIQE_FC_MISC.bREG.eof_control_ready	= eof_control_ready;
	pVIQE->cDEINTL_DECOMP1.nVIQE_FC_MISC.bREG.divisor			= dec_divisor;
	pVIQE->cDEINTL_DECOMP2.nVIQE_FC_MISC.bREG.eof_control_ready	= eof_control_ready;
	pVIQE->cDEINTL_DECOMP2.nVIQE_FC_MISC.bREG.divisor			= dec_divisor;

	pVIQE->cDEINTL_COMP.nVIQE_FC_MISC.bREG.fmt				= fmt;
	pVIQE->cDEINTL_COMP.nVIQE_FC_LIMIT.bREG.k0_ac_length_limit	= ac_k0_limit;
	pVIQE->cDEINTL_COMP.nVIQE_FC_LIMIT.bREG.k1_ac_length_limit	= ac_k1_limit;
	pVIQE->cDEINTL_COMP.nVIQE_FC_LIMIT.bREG.k2_ac_length_limit	= ac_k2_limit;
	*/
	BITCSET(pVIQE->cDEINTL_DECOMP0.nVIQE_FC_MISC.nREG, 0x304, (dec_divisor << 8) |(eof_control_ready << 2));
	BITCSET(pVIQE->cDEINTL_DECOMP1.nVIQE_FC_MISC.nREG, 0x304, (dec_divisor << 8) |(eof_control_ready << 2));
	BITCSET(pVIQE->cDEINTL_DECOMP2.nVIQE_FC_MISC.nREG, 0x304, (dec_divisor << 8) |(eof_control_ready << 2));

	BITCSET(pVIQE->cDEINTL_COMP.nVIQE_FC_MISC.nREG, 0xF000, (fmt << 12));
	BITCSET(pVIQE->cDEINTL_COMP.nVIQE_FC_LIMIT.nREG, 0x003F3F3F, 
								(ac_k2_limit << 16) 
								|(ac_k1_limit << 8) 
								|(ac_k0_limit << 0));

}

/******************************* DI Core Control *******************************/
void VIOC_VIQE_InitDeintlCoreBypass(VIQE *pVIQE)
{	/* Bypass Through Deinterlacer Core */
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cDEINTL.nDI_CTRL 		= 0x02010a31;			//0x280  
	pVIQE->cDEINTL.nDI_ENGINE0 	= 0x02040408;			//0x284  
	pVIQE->cDEINTL.nDI_ENGINE1 	= 0x7f32040f;			//0x288  
	pVIQE->cDEINTL.nDI_ENGINE2 	= 0x00800410;			//0x28C  
	pVIQE->cDEINTL.nDI_ENGINE3 	= 0x01002000;			//0x290  
	pVIQE->cDEINTL.nDI_ENGINE4 	= 0x12462582;			//0x294  
	pVIQE->cDEINTL.nPD_THRES0 	= 0x010085f4;			//0x298  
	pVIQE->cDEINTL.nPD_THRES1 	= 0x001e140f;			//0x29C  
	pVIQE->cDEINTL.nPD_JUDDER 	= 0x6f40881e;			//0x2A0  
	pVIQE->cDEINTL.nPD_JUDDER_M 	= 0x00095800;			//0x2A4  
	pVIQE->cDEINTL.nDI_MISCC	 	= 0x00000000;			//0x2A8  
	
	pVIQE->cDEINTL.nDI_REGION0 	= 0x00000000;			//0x2B4  
	pVIQE->cDEINTL.nDI_REGION1 	= 0x00000000;			//0x2B8  
	pVIQE->cDEINTL.nDI_INT		= 0x00000000;			//0x2BC  
	
	pVIQE->cDEINTL.nPD_SAW 		= 0x0008050a;			//0x2E0  
//	pVIQE->cDEINTL.nDI_CSIZE 		= 0x04380780;			//0x2E4  
//	pVIQE->cDEINTL.nDI_FMT 		= 0x00000001;			//0x2E8  
	*/
	BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0xFFFFFFFF, 0x00010b31);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE0, 0xFFFFFFFF, 0x02040408);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE1, 0xFFFFFFFF, 0x7f32040f);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE2, 0xFFFFFFFF, 0x00800410);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE3, 0xFFFFFFFF, 0x01002000);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE4, 0xFFFFFFFF, 0x12462582);
	BITCSET(pVIQE->cDEINTL.nPD_THRES0, 0xFFFFFFFF, 0x010085f4);
	BITCSET(pVIQE->cDEINTL.nPD_THRES1, 0xFFFFFFFF, 0x001e140f);
	BITCSET(pVIQE->cDEINTL.nPD_JUDDER, 0xFFFFFFFF, 0x6f40881e);
	BITCSET(pVIQE->cDEINTL.nPD_JUDDER_M, 0xFFFFFFFF, 0x00095800);
	BITCSET(pVIQE->cDEINTL.nDI_MISCC, 0xFFFFFFFF, 0x00000000);
	
	BITCSET(pVIQE->cDEINTL.nDI_REGION0, 0xFFFFFFFF, 0x00000000);
	BITCSET(pVIQE->cDEINTL.nDI_REGION1, 0xFFFFFFFF, 0x00000000);
	BITCSET(pVIQE->cDEINTL.nDI_INT, 0xFFFFFFFF, 0x00000000);
	
	BITCSET(pVIQE->cDEINTL.nPD_SAW, 0xFFFFFFFF, 0x0008050a);

}

void VIOC_VIQE_InitDeintlCoreSpatial(VIQE *pVIQE)
{	/* Deinterlacing By Using Just Input Frame */
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*	
	pVIQE->cDEINTL.nDI_CTRL 		= 0x00030a31;			//0x280  
	pVIQE->cDEINTL.nDI_ENGINE0 	= 0x02040408;			//0x284  
	pVIQE->cDEINTL.nDI_ENGINE1 	= 0x2812050f;			//0x288  
	pVIQE->cDEINTL.nDI_ENGINE2 	= 0x00800410;			//0x28C  
	pVIQE->cDEINTL.nDI_ENGINE3 	= 0x01002000;			//0x290  
	pVIQE->cDEINTL.nDI_ENGINE4 	= 0x12462582;			//0x294  
	pVIQE->cDEINTL.nPD_THRES0 	= 0x010085f4;			//0x298  
	pVIQE->cDEINTL.nPD_THRES1 	= 0x001e140f;			//0x29C  
	pVIQE->cDEINTL.nPD_JUDDER 	= 0x6f408805;			//0x2A0  
	pVIQE->cDEINTL.nPD_JUDDER_M 	= 0x00095800;			//0x2A4  
	pVIQE->cDEINTL.nDI_MISCC	 	= 0x00000000;			//0x2A8  
	
	pVIQE->cDEINTL.nDI_REGION0 	= 0x00000000;			//0x2B4  
	pVIQE->cDEINTL.nDI_REGION1 	= 0x00000000;			//0x2B8  
	pVIQE->cDEINTL.nDI_INT		= 0x00000000;			//0x2BC  
	
	pVIQE->cDEINTL.nPD_SAW 		= 0x0008050a;			//0x2E0  
//	pVIQE->cDEINTL.nDI_CSIZE 		= 0x01200160;			//0x2E4  
//	pVIQE->cDEINTL.nDI_FMT 		= 0x00000000;			//0x2E8  
	*/
	BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0xFFFFFFFF, 0x00030a31);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE0, 0xFFFFFFFF, 0x02040408);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE1, 0xFFFFFFFF, 0x2812050f);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE2, 0xFFFFFFFF, 0x00800410);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE3, 0xFFFFFFFF, 0x01002000);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE4, 0xFFFFFFFF, 0x12462582);
	BITCSET(pVIQE->cDEINTL.nPD_THRES0, 0xFFFFFFFF, 0x010085f4);
	BITCSET(pVIQE->cDEINTL.nPD_THRES1, 0xFFFFFFFF, 0x001e140f);
	BITCSET(pVIQE->cDEINTL.nPD_JUDDER, 0xFFFFFFFF, 0x6f408805);
	BITCSET(pVIQE->cDEINTL.nPD_JUDDER_M, 0xFFFFFFFF, 0x00095800);
	BITCSET(pVIQE->cDEINTL.nDI_MISCC, 0xFFFFFFFF, 0x00000000);
	
	BITCSET(pVIQE->cDEINTL.nDI_REGION0, 0xFFFFFFFF, 0x00000000);
	BITCSET(pVIQE->cDEINTL.nDI_REGION1, 0xFFFFFFFF, 0x00000000);
	BITCSET(pVIQE->cDEINTL.nDI_INT, 0xFFFFFFFF, 0x00000000);
	
	BITCSET(pVIQE->cDEINTL.nPD_SAW, 0xFFFFFFFF, 0x0008050a);
}

void VIOC_VIQE_InitDeintlCoreTemporal(VIQE *pVIQE)
{	/* Deinterlacing By Using 4-Field Reference Frame */
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*	
	pVIQE->cDEINTL.nDI_CTRL 		= 0x00010a31;			//0x280  
	pVIQE->cDEINTL.nDI_ENGINE0 	= 0x02040408;			//0x284  
	pVIQE->cDEINTL.nDI_ENGINE1 	= 0x7f32050f;			//0x288  
	pVIQE->cDEINTL.nDI_ENGINE2 	= 0x00800410;			//0x28C  
	pVIQE->cDEINTL.nDI_ENGINE3 	= 0x01002000;			//0x290  
	pVIQE->cDEINTL.nDI_ENGINE4 	= 0x12462582;			//0x294  
	pVIQE->cDEINTL.nPD_THRES0 	= 0x010085f4;			//0x298  
	pVIQE->cDEINTL.nPD_THRES1 	= 0x001e140f;			//0x29C  
	pVIQE->cDEINTL.nPD_JUDDER 	= 0x6f408805;			//0x2A0  
	pVIQE->cDEINTL.nPD_JUDDER_M 	= 0x00095800;			//0x2A4  
	pVIQE->cDEINTL.nDI_MISCC	 	= 0x00000000;			//0x2A8  
	
	pVIQE->cDEINTL.nDI_REGION0 	= 0x00000000;			//0x2B4  
	pVIQE->cDEINTL.nDI_REGION1 	= 0x00000000;			//0x2B8  
	pVIQE->cDEINTL.nDI_INT		= 0x00000000;			//0x2BC  
	
	pVIQE->cDEINTL.nPD_SAW 		= 0x0008050a;			//0x2E0  
//	pVIQE->cDEINTL.nDI_CSIZE 		= 0x01200160;			//0x2E4  
//	pVIQE->cDEINTL.nDI_FMT 		= 0x00000000;			//0x2E8  
	*/
	BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0xFFFFFFFF, 0x00010a31);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE0, 0xFFFFFFFF, 0x02040408);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE1, 0xFFFFFFFF, 0x7f32050f);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE2, 0xFFFFFFFF, 0x00800410);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE3, 0xFFFFFFFF, 0x01002000);
	BITCSET(pVIQE->cDEINTL.nDI_ENGINE4, 0xFFFFFFFF, 0x12462582);
	BITCSET(pVIQE->cDEINTL.nPD_THRES0, 0xFFFFFFFF, 0x010085f4);
	BITCSET(pVIQE->cDEINTL.nPD_THRES1, 0xFFFFFFFF, 0x001e140f);
	BITCSET(pVIQE->cDEINTL.nPD_JUDDER, 0xFFFFFFFF, 0x6f408805);
	BITCSET(pVIQE->cDEINTL.nPD_JUDDER_M, 0xFFFFFFFF, 0x00095800);
	BITCSET(pVIQE->cDEINTL.nDI_MISCC, 0xFFFFFFFF, 0x00000000);
	
	BITCSET(pVIQE->cDEINTL.nDI_REGION0, 0xFFFFFFFF, 0x00000000);
	BITCSET(pVIQE->cDEINTL.nDI_REGION1, 0xFFFFFFFF, 0x00000000);
	BITCSET(pVIQE->cDEINTL.nDI_INT, 0xFFFFFFFF, 0x00000000);
	
	BITCSET(pVIQE->cDEINTL.nPD_SAW, 0xFFFFFFFF, 0x0008050a);
}


void VIOC_VIQE_SetDeintlFMT(VIQE *pVIQE, int enable)
{
	BITCSET(pVIQE->cDEINTL.nDI_FMT, 1<<31, enable << 31);

}

void VIOC_VIQE_SetDeintlMode(VIQE *pVIQE, VIOC_VIQE_DEINTL_MODE mode)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	
	if(mode == VIOC_VIQE_DEINTL_MODE_BYPASS)
	{
		BITCSET(pVIQE->cDEINTL.nDI_CTRL , 0xFFFFFFFF, 0x20010b31);
		BITCSET(pVIQE->cDEINTL.nDI_ENGINE1, 0xFFFFFFFF, 0x7f32040f);
		BITCSET(pVIQE->cDEINTL.nPD_JUDDER, 0xFFFFFFFF, 0x6f40881e);
		
		BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0x20000000, (1<<29));						//bypass
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_CTRL.nREG, (0x1 << 16), (1 << 16)); 		//DI DMA enable
	}
	else if(mode == VIOC_VIQE_DEINTL_MODE_2D)
	{
		BITCSET(pVIQE->cDEINTL.nDI_CTRL , 0xFFFFFFFF, 0x00020a31);
		BITCSET(pVIQE->cDEINTL.nDI_ENGINE1, 0xFFFFFFFF, 0x2812050f);
		BITCSET(pVIQE->cDEINTL.nPD_JUDDER, 0xFFFFFFFF, 0x6f408805);

		BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0x20000000, (0<<29));						//bypass
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_CTRL.nREG, (0x1 << 16), (1 << 16)); 		//DI DMA enable
	}
	else if(mode == VIOC_VIQE_DEINTL_MODE_3D)	//Temporal Mode - using 4-field frames.
	{
		BITCSET(pVIQE->cDEINTL.nDI_CTRL , 0xFFFFFFFF, 0x00010a31);
		BITCSET(pVIQE->cDEINTL.nDI_ENGINE1, 0xFFFFFFFF, 0x7f32050f);
		BITCSET(pVIQE->cDEINTL.nPD_JUDDER, 0xFFFFFFFF, 0x6f408805);

		BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0x20000000, (0<<29));						//bypass
		BITCSET(pVIQE->cDEINTL_DMA.nDEINTL_CTRL.nREG, (0x1 << 16), (1 << 16)); 		//DI DMA enable
	}
	
}	

void VIOC_VIQE_SetDeintlRegion(VIQE *pVIQE, int region_enable, int region_idx_x_start, int region_idx_x_end, int region_idx_y_start, int region_idx_y_end)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*	
	// region0;
	pVIQE->cDEINTL.nDI_REGION0 = 0;
	pVIQE->cDEINTL.nDI_REGION0  |= (region_idx_x_start);
	pVIQE->cDEINTL.nDI_REGION0  |= (region_idx_x_end << 16);
	pVIQE->cDEINTL.nDI_REGION0  |= (region_enable << 31);
	// region1;
	pVIQE->cDEINTL.nDI_REGION1  = 0;
	pVIQE->cDEINTL.nDI_REGION1 |= (region_idx_y_start);
	pVIQE->cDEINTL.nDI_REGION1 |= (region_idx_y_end << 16);
	*/
	BITCSET(pVIQE->cDEINTL.nDI_REGION0, 0xFFFFFFFF, 
								(region_enable << 31)
								|(region_idx_x_end << 16)
								|(region_idx_x_start << 0));
	
	BITCSET(pVIQE->cDEINTL.nDI_REGION1, 0xFFFFFFFF, 
								(region_idx_y_end << 16)
								|(region_idx_y_start << 0));
	
}


void VIOC_VIQE_SetDeintlCore(VIQE *pVIQE, unsigned int width, unsigned int height, VIOC_VIQE_FMT_TYPE fmt, unsigned int bypass, unsigned int top_size_dont_use)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	if(bypass)
		BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0x00000100, (0<<8));						//0x280

	BITCSET(pVIQE->cDEINTL.nDI_CTRL, 0x20000000, (bypass<<29));						//0x280
	BITCSET(pVIQE->cDEINTL.nDI_CSIZE, 0x07FF07FF, (height << 16) | (width << 0));			//0x2E4  
	BITCSET(pVIQE->cDEINTL.nDI_FMT, 0x00010001, (top_size_dont_use << 16) | (fmt << 0));	//0x2E8
}

void VIOC_VIQE_SetDeintlRegister(VIQE *pVIQE, unsigned int fmt, unsigned int top_size_dont_use, unsigned int width, unsigned int height, VIOC_VIQE_DEINTL_MODE mode, unsigned int base0, unsigned int base1, unsigned int base2, unsigned int base3)
{
	int bypass =0;
	int dma_enable =1;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	
	if(mode == VIOC_VIQE_DEINTL_MODE_BYPASS)
	{
		bypass =1;
		VIOC_VIQE_InitDeintlCoreBypass(pVIQE);
	}	
	else if(mode == VIOC_VIQE_DEINTL_MODE_2D)
	{
		VIOC_VIQE_InitDeintlCoreSpatial(pVIQE);
	}
	else			//VIOC_VIQE_DEINTL_MODE_3D
	{
		dma_enable =1;
		VIOC_VIQE_InitDeintlCoreTemporal(pVIQE);
	}

	VIOC_VIQE_SetDeintlBase(pVIQE, 0, base0, base1, base2, base3);
	VIOC_VIQE_SetDeintlSize(pVIQE, width, height);
	VIOC_VIQE_SetDeintlMisc(pVIQE, OFF, ON, dma_enable, 0x16, OFF);					/* All of variables are the recommended value */
	VIOC_VIQE_SetDeintlControl(pVIQE, fmt, ON, 0x3, 0x31, 0x2a, 0x23);	/* All of variables are the recommended value */
	VIOC_VIQE_SetDeintlCore(pVIQE, width, height, fmt, bypass, OFF);
}

/******************************* DN Control *******************************/
void VIOC_VIQE_SetDenoiseBase(VIQE *pVIQE, unsigned int  frmnum, unsigned int  base0, unsigned int  base1, unsigned int  offs0, unsigned int  offs1)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cD3D_DMA.nD3D_CTRL.bREG.frmnum = frmnum;
	pVIQE->cD3D.nD3D_MISC.bREG.frmnum = frmnum;
	*/
	if (frmnum == 0) 
	{
	/*
		pVIQE->cD3D_DMA.nD3D_BASE0.nREG	=	base0;
		pVIQE->cD3D_DMA.nD3D_BASE1.nREG		=	base1;
	*/
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE0.nREG, 0x0FFFFFFFF, base0);						
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE1.nREG, 0x0FFFFFFFF, base1);						
	} 
#if 0
	else if (frmnum == 1) 
	{
	/*
		pVIQE->cD3D_DMA.nD3D_BASE0A.nREG		=	base0;
		pVIQE->cD3D_DMA.nD3D_BASE1A.nREG		=	base1;
	*/
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE0A.nREG, 0x0FFFFFFFF, base0);						
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE1A.nREG, 0x0FFFFFFFF, base1);						
	} 
	else if (frmnum == 2) 
	{
	/*
		pVIQE->cD3D_DMA.nD3D_BASE0B.nREG		=	base0;
		pVIQE->cD3D_DMA.nD3D_BASE1B.nREG		=	base1;
	*/
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE0B.nREG, 0x0FFFFFFFF, base0);						
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE1B.nREG, 0x0FFFFFFFF, base1);						
	} 
	else if (frmnum == 3) 
	{
	/*
		pVIQE->cD3D_DMA.nD3D_BASE0C.nREG		=	base0;
		pVIQE->cD3D_DMA.nD3D_BASE1C.nREG		=	base1;
	*/
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE0C.nREG, 0x0FFFFFFFF, base0);						
		BITCSET(pVIQE->cD3D_DMA.nD3D_BASE1C.nREG, 0x0FFFFFFFF, base1);						
	}
#endif
	/*
	pVIQE->cD3D_DMA.nD3D_OFFS.bREG.offs0	=	offs0;
	pVIQE->cD3D_DMA.nD3D_OFFS.bREG.offs1	=	offs1;
	*/
	BITCSET(pVIQE->cD3D_DMA.nD3D_OFFS.nREG, 0x0FFFFFFFF, (offs1 << 16)|(offs0 << 0));						
}

void VIOC_VIQE_SetDenoiseSize(VIQE *pVIQE, unsigned int  width, unsigned int  height)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cD3D_DMA.nD3D_SIZE.bREG.width	=	width;
	pVIQE->cD3D_DMA.nD3D_SIZE.bREG.height	=	height;

	pVIQE->cD3D.nD3D_SIZE.bREG.width	=	width;
	pVIQE->cD3D.nD3D_SIZE.bREG.height	=	height;
	*/
	BITCSET(pVIQE->cD3D_DMA.nD3D_SIZE.nREG, 0x07FF07FF, (height << 16)|(width << 0));						
	BITCSET(pVIQE->cD3D.nD3D_SIZE.nREG, 0x07FF07FF, (height << 16)|(width << 0));						
}	

void VIOC_VIQE_SetDenoiseMisc(VIQE *pVIQE, unsigned int  uvintpl, unsigned int  cfgupd, unsigned int  h2h, unsigned int  top_size_dont_use)
{
	/*
	pVIQE->cD3D_DMA.nD3D_CTRL.bREG.uvintpl	=	uvintpl;
	pVIQE->cD3D_DMA.nD3D_CTRL.bREG.cfgupd	=	cfgupd;
	pVIQE->cD3D_DMA.nD3D_CTRL.bREG.h2h		=	h2h;
	pVIQE->cD3D_DMA.nD3D_CTRL.bREG.top_size_dont_use	=	top_size_dont_use;
	*/
	BITCSET(pVIQE->cD3D_DMA.nD3D_CTRL.nREG, 0xFF020005, 
								(h2h << 24) 
								|(cfgupd << 17) 
								|(uvintpl << 2) 
								|(top_size_dont_use << 0));
}

/******************************* DN Core Control *******************************/
void VIOC_VIQE_SetDenoiseCoeff	(VIQE *pVIQE, int luma_2d_strength, int luma_tem_strength, int chroma_2d_strength, int chroma_tem_strength)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
#if 0
	/*
	pVIQE->cD3D.nCOEFF_HOR_LUMA_0 = d3d_luma_coeff_2d[luma_2d_strength][0];
	pVIQE->cD3D.nCOEFF_HOR_LUMA_1 = d3d_luma_coeff_2d[luma_2d_strength][1];
	pVIQE->cD3D.nCOEFF_VER_LUMA_0 = d3d_luma_coeff_2d[luma_2d_strength][2];
	pVIQE->cD3D.nCOEFF_VER_LUMA_1 = d3d_luma_coeff_2d[luma_2d_strength][3];
	pVIQE->cD3D.nCOEFF_TEM_LUMA_0 = d3d_luma_coeff_tem[luma_tem_strength][0];
	pVIQE->cD3D.nCOEFF_TEM_LUMA_1 = d3d_luma_coeff_tem[luma_tem_strength][1];

	pVIQE->cD3D.nCOEFF_HOR_CHROMA_0 = d3d_chroma_coeff_2d[chroma_2d_strength][0];
	pVIQE->cD3D.nCOEFF_HOR_CHROMA_1 = d3d_chroma_coeff_2d[chroma_2d_strength][1];
	pVIQE->cD3D.nCOEFF_VER_CHROMA_0 = d3d_chroma_coeff_2d[chroma_2d_strength][2];
	pVIQE->cD3D.nCOEFF_VER_CHROMA_1 = d3d_chroma_coeff_2d[chroma_2d_strength][3];
	pVIQE->cD3D.nCOEFF_TEM_CHROMA_0 = d3d_chroma_coeff_tem[chroma_tem_strength][0];
	pVIQE->cD3D.nCOEFF_TEM_CHROMA_1 = d3d_chroma_coeff_tem[chroma_tem_strength][1];
	*/
#endif	
}

void VIOC_VIQE_SetDenoiseCoreMisc(VIQE *pVIQE, unsigned int  top_size_dont_use, unsigned int  d3in, unsigned int  lut_init, unsigned int  cfgupd)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cD3D.nD3D_MISC.bREG.top_size_dont_use	=	top_size_dont_use;
	pVIQE->cD3D.nD3D_MISC.bREG.d3in		=	d3in;
	pVIQE->cD3D.nD3D_MISC.bREG.lut_init	=	lut_init;
	pVIQE->cD3D.nD3D_MISC.bREG.cfgupd	= 	cfgupd;
	*/
	BITCSET(pVIQE->cD3D.nD3D_MISC.nREG, 0x00010B00, 
								(top_size_dont_use << 16) 
								|(lut_init << 11) 
								|(cfgupd << 9) 
								|(d3in << 8));
}

void VIOC_VIQE_SetDenoiseCoreBypass(VIQE *pVIQE, unsigned int  bypass_en)
{
	/*
	pVIQE->cD3D.nD3D_MISC2.bREG.bypass = bypass_en;
	*/
	BITCSET(pVIQE->cD3D.nD3D_MISC2.nREG, 0x1, bypass_en);						
}

void VIOC_VIQE_SetDenoiseFCCtrl(VIQE *pVIQE, unsigned int  fmt, unsigned int  eof_control_ready, unsigned int  dec_divisor, unsigned int  ac_k0_limit, unsigned int  ac_k1_limit, unsigned int  ac_k2_limit)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	/*
	pVIQE->cD3D_DECOMP.nVIQE_FC_MISC.bREG.eof_control_ready = eof_control_ready;
	pVIQE->cD3D_DECOMP.nVIQE_FC_MISC.bREG.divisor = dec_divisor;
	pVIQE->cD3D_COMP.nVIQE_FC_MISC.bREG.fmt = fmt;
	pVIQE->cD3D_COMP.nVIQE_FC_LIMIT.bREG.k0_ac_length_limit = ac_k0_limit;
	pVIQE->cD3D_COMP.nVIQE_FC_LIMIT.bREG.k1_ac_length_limit = ac_k1_limit;
	pVIQE->cD3D_COMP.nVIQE_FC_LIMIT.bREG.k2_ac_length_limit = ac_k2_limit;
	*/
	BITCSET(pVIQE->cD3D_DECOMP.nVIQE_FC_MISC.nREG, 0x304, (dec_divisor << 8) |(eof_control_ready << 2));
	BITCSET(pVIQE->cD3D_COMP.nVIQE_FC_MISC.nREG, 0xF000, (fmt << 12));
	BITCSET(pVIQE->cD3D_COMP.nVIQE_FC_LIMIT.nREG, 0x003F3F3F, 
								(ac_k2_limit << 16) 
								|(ac_k1_limit << 8) 
								|(ac_k0_limit << 0));
}
							
void VIOC_VIQE_SetDenoise(VIQE *pVIQE, unsigned int  fmt, unsigned int  width, unsigned int  height, unsigned int  bypass, unsigned int  frmnum, unsigned int  base0, unsigned int  base1)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
//	VIOC_VIQE_SetDenoiseCoeff(pVIQE, STRENGTH_3,STRENGTH_3,STRENGTH_3,STRENGTH_3);
	VIOC_VIQE_SetDenoiseBase(pVIQE, frmnum, base0, base1, 0x0, 0x0);
	VIOC_VIQE_SetDenoiseSize(pVIQE, width, height);	
	VIOC_VIQE_SetDenoiseMisc (pVIQE, OFF, ON, 0x16, OFF);
	VIOC_VIQE_SetDenoiseCoreMisc(pVIQE, OFF, OFF, ON, ON);
	VIOC_VIQE_SetDenoiseCoreBypass(pVIQE, bypass);
	VIOC_VIQE_SetDenoiseFCCtrl(pVIQE, fmt, OFF, 0x3, 0x31, 0x2a, 0x23);
}

/******************************* GM Control *******************************/
/******************************* HI Control *******************************/


#if 0
#define HwVIOC_VIQE_BASE	((volatile int *)(HwVIOC_VIQE0))
#define VIOC_VIQE_IREQ_DII_MASK		0x00000001UL

void VIOC_VIQE_IreqHandler(unsigned int vectorID)
{

	VIQE *pVIQE;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	
	switch(vectorID)
	{
		case VIOC_IREQ_VIQE:
		{
			pVIQE = (VIQE *)HwVIOC_VIQE_BASE;
			break;
		}
		default:
		{
			return;
		}	
	}

	if(pVIQE->cVIQE_CTRL.nVIQE_IRQ.nREG& VIOC_VIQE_IREQ_DII_MASK)
	{
		/* Interrput Source Clear */
		/* Status Clear */
		pVIQE->cVIQE_CTRL.nVIQE_IRQ.bREG.deintl_IRQ = 1;
		/* DO Action for Interrupt */

		/* Interrput Re-Setting */
	}	
}
#endif

