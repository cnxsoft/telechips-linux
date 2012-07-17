/****************************************************************************
 *    FileName    : tcc_fb_address.c
 *    Description :
 *    ****************************************************************************
 *  
 *    TCC Version 1.0
 *    Copyright (c) Telechips, Inc.
 *    ALL RIGHTS RESERVED
 *    *
 ****************************************************************************/
#include <linux/kernel.h>
#include <mach/tccfb_address.h>
#include <mach/tccfb_ioctrl.h> 

void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
									unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U,unsigned int* V)
{
	unsigned int Uaddr, Vaddr, Yoffset, UVoffset, start_yPos, start_xPos;
	
	start_yPos = (start_y>>1)<<1;
	start_xPos = (start_x>>1)<<1;
	Yoffset = (src_imgx * start_yPos) + start_xPos;

	//RGB format 
	if((format >= TCC_LCDC_IMG_FMT_RGB332) && (format <= TCC_LCDC_IMG_FMT_ARGB6666_3))
	{
		int Bpp;

		if(format == TCC_LCDC_IMG_FMT_RGB332)
			Bpp = 1;
		else if((format >= TCC_LCDC_IMG_FMT_RGB444) && (format <= TCC_LCDC_IMG_FMT_RGB555))
			Bpp = 2;
		else if((format >= TCC_LCDC_IMG_FMT_RGB888) && (format <= TCC_LCDC_IMG_FMT_ARGB6666_3))
			Bpp = 4;
		else 
			Bpp = 2;
		
		*Y = base_Yaddr + Yoffset * Bpp;
		
		return 0;
	}

	if((format == TCC_LCDC_IMG_FMT_UYVY) || (format == TCC_LCDC_IMG_FMT_VYUY)
		|| (format == TCC_LCDC_IMG_FMT_YUYV) || (format == TCC_LCDC_IMG_FMT_YVYU))
		Yoffset = 2*Yoffset;

	*Y = base_Yaddr + Yoffset;

	if(*U == 0 && *V == 0) {
		Uaddr = GET_ADDR_YUV42X_spU(base_Yaddr, src_imgx, src_imgy);
		if(format == TCC_LCDC_IMG_FMT_YUV420SP)
			Vaddr = GET_ADDR_YUV420_spV(Uaddr, src_imgx, src_imgy);
		else
			Vaddr = GET_ADDR_YUV422_spV(Uaddr, src_imgx, src_imgy);
	} else {
		Uaddr = *U;
		Vaddr = *V;
	}

	if((format == TCC_LCDC_IMG_FMT_YUV420SP) || (format == TCC_LCDC_IMG_FMT_YUV420ITL0) || (format == TCC_LCDC_IMG_FMT_YUV420ITL1)) {
		if(format == TCC_LCDC_IMG_FMT_YUV420SP)
			UVoffset = ((src_imgx * start_yPos)/4 + start_xPos/2);
		else
			UVoffset = ((src_imgx * start_yPos)/2 + start_xPos);
	} else {
		if(format == TCC_LCDC_IMG_FMT_YUV422ITL1)
			UVoffset = ((src_imgx * start_yPos) + start_xPos);
		else
			UVoffset = ((src_imgx * start_yPos)/2 + start_xPos/2);
	}
	
	*U = Uaddr + UVoffset;
	*V = Vaddr + UVoffset;

	//printk(" ### %s Yoffset = [%d] \n",__func__,Yoffset);
}
EXPORT_SYMBOL(tccxxx_GetAddress);
