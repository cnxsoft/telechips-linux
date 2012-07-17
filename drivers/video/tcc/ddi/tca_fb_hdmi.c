
/****************************************************************************
 *   FileName    : tca_fb_hdmi.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
/*****************************************************************************
*
* Header Files Include
*
******************************************************************************/
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

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#include <linux/cpufreq.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>

#include <mach/tccfb_ioctrl.h>
#include <mach/tcc_grp_ioctrl.h>
#include <mach/tcc_scaler_ctrl.h>
#include <mach/tca_fb_hdmi.h>
#include <mach/tca_fb_output.h>

#if 0
#define dprintk(msg...)	 { printk( "tca_hdmi: " msg); }
#else
#define dprintk(msg...)	 
#endif

/*****************************************************************************
*
* Defines
*
******************************************************************************/
//#define TCC_FB_UPSCALE

/*****************************************************************************
*
* structures
*
******************************************************************************/


/*****************************************************************************
*
* Variables
*
******************************************************************************/

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
#define HDMI_EXCLUSIVE_UI_SCALER0		0
#define HDMI_EXCLUSIVE_UI_SCALER1		1
#define HDMI_EXCLUSIVE_UI_DVD_MAX_WD	720
#define HDMI_EXCLUSIVE_UI_DVD_MAX_HT	576
static unsigned int hdmi_exclusive_ui_idx = 0;
static unsigned int hdmi_exclusive_ui_onthefly = FALSE;
static exclusive_ui_params hdmi_exclusive_ui_param;
#endif

extern void tccxxx_overlay_start(void);
char M2M_Scaler1_Ctrl_Detail(SCALER_TYPE *scale_img);
char M2M_Scaler_Ctrl_Detail(SCALER_TYPE *scale_img);

/*****************************************************************************
*
* Functions
*
******************************************************************************/
void TCC_HDMI_LCDC_Timing(char hdmi_lcdc, struct lcdc_timimg_parms_t *mode)
{
    volatile unsigned int regl;
	unsigned int lcdc_id;
	volatile PLCDC	pLCDC;
	PDDICONFIG pHwDDICACHE_CONFIG  = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	PCKC	pCKC = (volatile PCKC *)tcc_p2v(HwCKC_BASE);

	dprintk(" %s lcdc:[%d]  %d %d %d %d \n", __func__, hdmi_lcdc, mode->id, mode->iv, mode->ih, mode->lpw);

	if (hdmi_lcdc)	// lcdc1
	{
		lcdc_id = 1;
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
		pLCDC->LCTRL = 0;

		BITSET(pHwDDICACHE_CONFIG->HDMI_CTRL, HwDDIC_HDMI_CTRL_SEL_LCDC1);

		// disable lcdc
		regl = readl(&pLCDC->LCTRL);
		writel(regl & ~(0x00000001), &pLCDC->LCTRL);
		
		#if defined(CONFIG_ARCH_TCC92XX)
		regl = readl(&pCKC->PCLK_LCD1);
		writel(regl & ~(0x10000000), &pCKC->PCLK_LCD1);	// EN: 0

		// set lcdc1 clock
		regl = readl(&pCKC->PCLK_LCD1);
		regl &= ~(0x1f000fff);
		writel(regl, &pCKC->PCLK_LCD1);	// EN : 0, init.
		regl |= 12<<24;					// SEL: HDMI PCLK(Variable)
		regl |= 0;						// DIV: 0
		writel(regl, &pCKC->PCLK_LCD1);
		regl |= 1 <<28;					// EN : 1
		writel(regl, &pCKC->PCLK_LCD1);
		#else


		regl = readl(&pCKC->PCLK_LCD1);
        writel(regl & ~(0x20000000), &pCKC->PCLK_LCD1);	// EN: 0

		// set lcdc1 clock
		regl = readl(&pCKC->PCLK_LCD1);
        regl &= ~(0x3f000fff);

		writel(regl, &pCKC->PCLK_LCD1);	// EN : 0, init.
		regl |= 12<<24;					// SEL: HDMI PCLK(Variable)
		regl |= 0;						// DIV: 0
		writel(regl, &pCKC->PCLK_LCD1);
        regl |= 1 <<29;					// EN : 1
		writel(regl, &pCKC->PCLK_LCD1);	
		#endif//
	}
	else		// lcdc0
	{
		lcdc_id = 0;
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

		pLCDC->LCTRL = 0;

		BITCLR(pHwDDICACHE_CONFIG->HDMI_CTRL, HwDDIC_HDMI_CTRL_SEL_LCDC1);
		#ifdef CONFIG_ARCH_TCC92XX
		// disable lcdc
		regl = readl(&pLCDC->LCTRL);
		writel(regl & ~(0x00000001), &pLCDC->LCTRL);

		// set lcdc0 clock
		regl = readl(&pCKC->PCLK_LCD0);
		regl &= ~(0x1f000fff);
		writel(regl, &pCKC->PCLK_LCD0);	// EN : 0, init.
		regl |= 12<<24;					// SEL: HDMI PCLK(Variable)
		regl |= 0;						// DIV: 0
		writel(regl, &pCKC->PCLK_LCD0);
		regl |= 1 <<28;					// EN : 1
		writel(regl, &pCKC->PCLK_LCD0);
		#else
		// set lcdc0 clock
		regl = readl(&pCKC->PCLK_LCD0);
        regl &= ~(0x3f000fff);
		writel(regl, &pCKC->PCLK_LCD0);	// EN : 0, init.
		regl |= 12<<24;					// SEL: HDMI PCLK(Variable)
		regl |= 0;						// DIV: 0
		writel(regl, &pCKC->PCLK_LCD0);
        regl |= 1 <<29;					// EN : 1
		writel(regl, &pCKC->PCLK_LCD0);		
		#endif//

	}

	//channel disable
	regl = readl(&pLCDC->LI0C);
	writel(regl & (~(0x10000000)), &pLCDC->LI0C);

	// set lctrl
	regl = readl(&pLCDC->LCTRL);
	regl &= ~(0x0000f3e0);
	regl |=((mode->id  & 0x1) <<15)
		 | ((mode->iv  & 0x1) <<14)
		 | ((mode->ih  & 0x1) <<13)
		 | ((mode->ip  & 0x1) <<12)
		 | ((mode->dp  & 0x1) << 9)
		 | ((mode->ni  & 0x1) << 8)
		 | ((mode->tv  & 0x1) << 7)
		 | ((mode->tft & 0x1) << 6)
		 | ((mode->stn & 0x1) << 5);
	regl = (regl & ~(0xf<<16)) | ((12)<<16);	//pxdw: 12 - rgb888
	
#if 0//defined(LCDC_AXI_BUS) && defined(CONFIG_ARCH_TCC88XX)
	regl |= Hw27; // AXI bus
#endif//

	writel(regl, &pLCDC->LCTRL);

	// set clkdiv
	writel(0x80000000, &pLCDC->LCLKDIV);
	
	// set timing
	writel((mode->lpw  <<16) | (mode->lpc  ), &pLCDC->LHTIME1);
	writel((mode->lswc <<16) | (mode->lewc ), &pLCDC->LHTIME2);
	writel((mode->fpw  <<16) | (mode->flc  ), &pLCDC->LVTIME1);
	writel((mode->fswc <<16) | (mode->fewc ), &pLCDC->LVTIME2);
	writel((mode->fpw2 <<16) | (mode->flc2 ), &pLCDC->LVTIME3);
	writel((mode->fswc2<<16) | (mode->fewc2), &pLCDC->LVTIME4);

	regl = readl(&pLCDC->LVTIME1);
	writel(regl | (mode->vdb  <<27) | (mode->vdf  <<22), &pLCDC->LVTIME1);

	// set lcdc.lds
	if( mode->dp == 1)
	{
		regl = ((mode->flc+1 )<<16) | ((mode->lpc+1) >> 1);
	}
	else
	{
		regl = ((mode->flc+1)<<16) | (mode->lpc+1);
	}
	writel(regl, &pLCDC->LDS);

	// set lcdc.img dma interlace mode
	if (mode->ni)	// non-interlace mode (progressive mode)
	{
		regl = readl(&pLCDC->LI0C);
		writel(regl & ~(0x80000000), &pLCDC->LI0C);
		if(lcdc_id)
		{
			regl = readl(&pLCDC->LI1C);
			writel(regl & ~(0x80000000), &pLCDC->LI1C);
			regl = readl(&pLCDC->LI2C);
			writel(regl & ~(0x80000000), &pLCDC->LI2C);
		}
	}
	else			// interlace mode
	{
		regl = readl(&pLCDC->LI0C);
		writel(regl | (0x80000000), &pLCDC->LI0C);
		if(lcdc_id)
		{
			regl = readl(&pLCDC->LI1C);
			writel(regl | (0x80000000), &pLCDC->LI1C);
			regl = readl(&pLCDC->LI2C);
			writel(regl | (0x80000000), &pLCDC->LI2C);
		}
	}
	writel(0x00000000, &pLCDC->LBC);
	
	// enable lcdc
	regl = readl(&pLCDC->LCTRL);
	writel(regl | 0x00000001, &pLCDC->LCTRL);

    return;
}


char TCC_OnTheFly_setting(char OnOff, char hdmi_lcdc,  char scaler)
{
	volatile PDDICONFIG pDDICONFIG;

	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if(OnOff)
	{		
	 	if(scaler)
			M2M_Scaler_SW_Reset(M2M_SCALER1); 
		else
			M2M_Scaler_SW_Reset(M2M_SCALER0); 

		dprintk("%s, lcdc_num=%d, scaler_num=%d on\n", __func__, hdmi_lcdc, scaler);
	}		
	
	if(scaler)
	{
		if(hdmi_lcdc)
		{
			BITCSET(pDDICONFIG->ON_THE_FLY , Hw0+Hw1, 0);			// VIQE + LCDC0
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);			// SCALER0 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4);		// SCALER1 + LCDC1
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);	// SCALER1 Not Used
		}
		else
		{
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw0);			// VIQE + LCDC1
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);			// SCALER0 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, 0);		// SCALER1 + LCDC0
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);	// SCALER1 Not Used
		}
	}
	else
	{
		if(hdmi_lcdc)
		{
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, 0);			// VIQE + LCDC0
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);		// SCALER1 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw2);		// SCALER0 + LCDC1
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);		// SCALER0 Not Used
		}
		else
		{
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw0+Hw1, Hw0);			// VIQE + LCDC1
			BITCSET(pDDICONFIG->ON_THE_FLY, Hw4+Hw5, Hw4+Hw5);		// SCALER1 Not Used

			if(OnOff)	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, 0);		// SCALER0 + LCDC0
			else	
				BITCSET(pDDICONFIG->ON_THE_FLY, Hw2+Hw3, Hw3);		// SCALER0 Not Used
		}
	}

	return 1;
}

void TCC_HDMI_DISPLAY_UPDATE_OnTheFly(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo)
{
	unsigned int lcd_width, lcd_height;
	unsigned int img_width, img_height, img_offset_x, img_offset_y;
	unsigned int output_width, output_height;
	unsigned int scaler_num;
	unsigned int scale_limit_wd, scale_limit_ht, scale_limit_total;
	SCALER_TYPE fbscaler;
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_channel;

	scaler_num = 0;

	dprintk(" %s onoff:%d :fmt:%d Fw:%d Fh:%d Iw:%d Ih:%d \n", __func__,ImageInfo->enable,
		ImageInfo->fmt,ImageInfo->Frame_width, ImageInfo->Frame_height, ImageInfo->Image_width, ImageInfo->Image_height);
	
	img_width	= ImageInfo->Frame_width;
	img_height	= ImageInfo->Frame_height;

	img_offset_x = 0;
	img_offset_y = 0;

 	if (hdmi_lcdc)
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	
	lcd_width	= ((pLCDC->LDS>>0 ) & 0xFFFF);
	lcd_height	= ((pLCDC->LDS>>16) & 0xFFFF);

	pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI0C;

	output_width	= ImageInfo->Image_width;
	output_height	= ImageInfo->Image_height;

	 /* Set the scaler information */
	memset(&fbscaler, 0x00, sizeof(SCALER_TYPE));

	//fbscaler.responsetype = SCALER_INTERRUPT;
	fbscaler.responsetype = SCALER_NOWAIT;

	if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SP)
		fbscaler.src_fmt = SCALER_YUV422_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420SP)
		fbscaler.src_fmt = SCALER_YUV420_sp;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422SQ)
		fbscaler.src_fmt = SCALER_YUV422_sq0;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV420ITL0)
		fbscaler.src_fmt = SCALER_YUV420_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_YUV422ITL0)
		fbscaler.src_fmt = SCALER_YUV422_inter;
	else if(ImageInfo->fmt == TCC_LCDC_IMG_FMT_RGB565)
		fbscaler.src_fmt = SCALER_RGB565;
	else
		printk("%s, Err: M2M Scaler Format(%d) is not supported!!\n", __func__, ImageInfo->fmt);
		
	fbscaler.src_Yaddr = (char *)ImageInfo->addr0;
	fbscaler.src_Uaddr = (char *)ImageInfo->addr1;
	fbscaler.src_Vaddr = (char *)ImageInfo->addr2;
	fbscaler.src_ImgWidth = img_width;
	fbscaler.src_ImgHeight = img_height;
	fbscaler.src_winLeft = 0;
	fbscaler.src_winTop = 0;
	fbscaler.src_winRight = img_width;
	fbscaler.src_winBottom = img_height;

	fbscaler.dest_fmt = SCALER_YUV422_sq0;		// destination image format
	fbscaler.dest_ImgWidth = output_width;		// destination image width
	fbscaler.dest_ImgHeight = output_height; 		// destination image height
	fbscaler.dest_winLeft = 0;
	fbscaler.dest_winTop = 0;
	fbscaler.dest_winRight = output_width;
	fbscaler.dest_winBottom = output_height;

 	/* Set the output path for ON_THE_FLY */
	fbscaler.viqe_onthefly = 0x02;

 	/* Execute the scaler */
	if(scaler_num)
		M2M_Scaler1_Ctrl_Detail(&fbscaler);
	else
		M2M_Scaler_Ctrl_Detail(&fbscaler);
	
	dprintk("%s, src_format=%d, src_w=%d, src_h=%d, src_addr=0x%08x, dst_format=%d, dst_w=%d, dst_h=%d, offset_x=%d, offset_y=%d\n", 
			__func__, fbscaler.src_fmt, fbscaler.src_ImgWidth, fbscaler.src_ImgHeight, fbscaler.src_Yaddr,
			fbscaler.dest_fmt, fbscaler.dest_ImgWidth, fbscaler.dest_ImgHeight, img_offset_x, img_offset_y);
		
 	/* Update new image information */
	/*
	ImageInfo->Frame_width = fbscaler.dest_ImgWidth;
	ImageInfo->Frame_height = fbscaler.dest_ImgHeight;
	ImageInfo->Image_width = fbscaler.dest_ImgWidth;
	ImageInfo->Image_height = fbscaler.dest_ImgHeight;
	ImageInfo->offset_x = img_offset_x;
	ImageInfo->offset_y = img_offset_y;	

	ImageInfo->fmt = TCC_LCDC_IMG_FMT_YUV422SQ;	
	ImageInfo->addr0 = (unsigned int)fbscaler.dest_Yaddr;
	ImageInfo->addr1 = (unsigned int)fbscaler.dest_Uaddr;
	ImageInfo->addr2 = (unsigned int)fbscaler.dest_Vaddr;
	*/

	return ;
}

static unsigned int onthefly_using = 0;
void TCC_HDMI_DISPLAY_UPDATE(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo)
{
	unsigned int regl;
	unsigned int lcd_width = 0, lcd_height = 0, lcd_h_pos = 0, lcd_w_pos = 0, scale_x = 0, scale_y = 0;
	unsigned int buffer_width = 0, y2r_option = 0, onthefly = 0;
	unsigned int Image_fmt;
	int loop=0;

	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_channel;

	dprintk(" %s onoff:%d :fmt:%d Fw:%d Fh:%d Iw:%d Ih:%d onthefly:%d \n", __func__,ImageInfo->enable,
		ImageInfo->fmt,ImageInfo->Frame_width, ImageInfo->Frame_height, ImageInfo->Image_width, ImageInfo->Image_height,ImageInfo->on_the_fly);

	if((ImageInfo->Lcdc_layer >= 3) || (ImageInfo->fmt >TCC_LCDC_IMG_FMT_MAX))
		return;

	if (hdmi_lcdc)	// lcdc1
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	regl = pLCDC->LDS; // get LCD size 

	lcd_width = (regl & 0xFFFF);
	lcd_height = ((regl>>16) & 0xFFFF);
	
	if((!lcd_width) || (!lcd_height))
		return;

	switch(ImageInfo->Lcdc_layer )
	{
		case 0:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI0C;
			break;
		case 1:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI1C;
			break;
		case 2:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI2C;
			break;
	}
	
	if(!ImageInfo->enable)	{
		
		if(ISZERO(pLCDC->LCTRL, HwLCTRL_LEN))
			return ;			

		if(ISZERO(pLCDC_channel, HwLIC_IEN))
			return ;			

		while(TRUE && (loop < 0x70000000)) //check wheather is lcd video channel on
		{
			if(ISSET(pLCDC->LSTATUS, HwLSTATUS_I0EOF))
				break;			
			loop++;
		}
		
		pLCDC_channel->LIS =0;
		
		#if defined(CONFIG_ARCH_TCC92XX)
		pLCDC_channel->LIC = 0;
		#else		
		pLCDC_channel->LIC= HwLCT_RU | !HwLIC_IEN;
		onthefly_using = 0;		
		
		#endif//

		
		return;
	}

	if( ImageInfo->on_the_fly) {
		if(!onthefly_using)
		{
			TCC_OnTheFly_setting(1, hdmi_lcdc ,ImageInfo->on_the_fly - 1);
			onthefly =  HwLIC_SRC;						
			onthefly_using = 1;
		}		
		TCC_HDMI_DISPLAY_UPDATE_OnTheFly( hdmi_lcdc, ImageInfo);
		BITCSET(pLCDC_channel->LIC, HwLIC_SRC, HwLIC_SRC);
		Image_fmt = TCC_LCDC_IMG_FMT_YUV422SQ;
		
	} else {
		if(onthefly_using)
		{
			onthefly_using = 0;		
		}
		BITCSET(pLCDC_channel->LIC, HwLIC_SRC, 0);
		Image_fmt = ImageInfo->fmt;		
	}				
	
	{
		buffer_width = (((ImageInfo->Frame_width + 3)>>2)<<2);

		switch(Image_fmt)
		{
			case TCC_LCDC_IMG_FMT_RGB444:
			case TCC_LCDC_IMG_FMT_RGB565:
			case TCC_LCDC_IMG_FMT_RGB555:
				pLCDC_channel->LIO = buffer_width *2;
				y2r_option = 0;
				break;

			case TCC_LCDC_IMG_FMT_RGB888:
			case TCC_LCDC_IMG_FMT_RGB666:
				pLCDC_channel->LIO = buffer_width * 4;
				y2r_option = 0;
				break;
			
			case TCC_LCDC_IMG_FMT_YUV420SP:
			case TCC_LCDC_IMG_FMT_YUV422SP:
				pLCDC_channel->LIO = ((buffer_width / 2)<<16)|(buffer_width);
				y2r_option = HwLIC_Y2R;
				break;

			case TCC_LCDC_IMG_FMT_YUV422SQ:
				pLCDC_channel->LIO = buffer_width * 2;
				y2r_option = HwLIC_Y2R;
				break;

			case TCC_LCDC_IMG_FMT_YUV420ITL0:
			case TCC_LCDC_IMG_FMT_YUV420ITL1:
			case TCC_LCDC_IMG_FMT_YUV422ITL0:
			case TCC_LCDC_IMG_FMT_YUV422ITL1:
				pLCDC_channel->LIO = (buffer_width << 16)| (buffer_width);
				y2r_option = HwLIC_Y2R;
				break;

		}
		
		scale_x = 0;
		scale_y = 0;
		lcd_h_pos = 0;
		lcd_w_pos = 0;

		if(lcd_width > ImageInfo->Image_width)
			lcd_w_pos = (lcd_width - ImageInfo->Image_width)/2;
		
		if(lcd_height > ImageInfo->Image_height)
			lcd_h_pos = (lcd_height - ImageInfo->Image_height)/2;

	// position
		pLCDC_channel->LIP = ((lcd_h_pos << 16) | (lcd_w_pos));
	// scale
		pLCDC_channel->LISR =((scale_y<<4) | scale_x);
	// size
		pLCDC_channel->LIS =((ImageInfo->Image_height << 16) | (ImageInfo->Image_width));

	// position
		if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
			pLCDC_channel->LIP =(((ImageInfo->offset_y/2) << 16) | (ImageInfo->offset_x));
		else
			pLCDC_channel->LIP =((ImageInfo->offset_y << 16) | (ImageInfo->offset_x));

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		/* Get the parameters for exclusive ui */
		TCC_OUTPUT_EXCLUSIVE_UI_GetParam(&hdmi_exclusive_ui_param);

		if(hdmi_exclusive_ui_param.enable && hdmi_exclusive_ui_onthefly)
		{
			//channel control
			BITCSET(pLCDC_channel->LIC, HwLIC_SRC | HwLIC_Y2R | HwLIC_FMT, HwLIC_SRC | y2r_option | (Image_fmt));
		}
		else
		{
			// image address
			pLCDC_channel->LIBA0 = ImageInfo->addr0;
			pLCDC_channel->LIBA1 = ImageInfo->addr1;
			pLCDC_channel->LIBA2 = ImageInfo->addr2;

			//channel control
			BITCSET(pLCDC_channel->LIC, HwLIC_SRC | HwLIC_Y2R | HwLIC_FMT, y2r_option | (Image_fmt));
		}

		hdmi_exclusive_ui_onthefly = FALSE;
		
		if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
		{
			if(hdmi_exclusive_ui_param.interlace && !TCC_OUTPUT_EXCLUSIVE_UI_GetBypass())
				BITCSET(pLCDC_channel->LIC, HwLIC_INTL, 0);
			else
				BITCSET(pLCDC_channel->LIC, HwLIC_INTL, HwLIC_INTL);
		}
		else
			BITCSET(pLCDC_channel->LIC, HwLIC_INTL, 0);
	#else
		// image address
		pLCDC_channel->LIBA0 = ImageInfo->addr0;
		pLCDC_channel->LIBA1 = ImageInfo->addr1;
		pLCDC_channel->LIBA2 = ImageInfo->addr2;

		//channel contro
		BITCSET(pLCDC_channel->LIC, HwLIC_Y2R | HwLIC_FMT, y2r_option | (Image_fmt));

		if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
			BITCSET(pLCDC_channel->LIC, HwLIC_INTL, HwLIC_INTL);
		else
			BITCSET(pLCDC_channel->LIC, HwLIC_INTL, 0);

	#endif

		#if defined(CONFIG_ARCH_TCC92XX)
		BITSET(pLCDC_channel->LIC, HwLIC_IEN);
		#else
		BITSET(pLCDC_channel->LIC, HwLCT_RU | HwLIC_IEN);
		#endif//

		tccxxx_overlay_start(); // to inform that overlay is started.
	}
}

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
void TCC_HDMI_DISPLAY_PROCESS(char hdmi_lcdc, struct tcc_lcdc_image_update *ImageInfo, char force_update)
{
	unsigned int regl;
	unsigned int lcd_width = 0, lcd_height = 0, lcd_h_pos = 0, lcd_w_pos = 0, scale_x = 0, scale_y = 0;
	unsigned int img_width = 0, img_height = 0, output_width = 0, output_height = 0;
	unsigned int buffer_width = 0, y2r_option = 0, mixer_interrupt_use = 0;
	exclusive_ui_ar_params aspect_ratio;
	PLCDC pLCDC;
	PLCDC_CHANNEL pLCDC_channel;

	dprintk(" %s onoff:%d :fmt:%d Fw:%d Fh:%d Iw:%d Ih:%d \n", __func__,ImageInfo->enable,
		ImageInfo->fmt,ImageInfo->Frame_width, ImageInfo->Frame_height, ImageInfo->Image_width, ImageInfo->Image_height);

	if((ImageInfo->Lcdc_layer >= 3) || (ImageInfo->fmt >TCC_LCDC_IMG_FMT_MAX))
		return;

	if (hdmi_lcdc)	// lcdc1
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	regl = pLCDC->LDS; // get LCD size 

	lcd_width = (regl & 0xFFFF);
	lcd_height = ((regl>>16) & 0xFFFF);
	
	if((!lcd_width) || (!lcd_height))
		return;

	output_width = lcd_width;
	output_height = lcd_height;

	switch(ImageInfo->Lcdc_layer)
	{
		case 0:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI0C;
			break;
		case 1:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI1C;
			break;
		case 2:
			pLCDC_channel = (volatile PLCDC_CHANNEL)&pLCDC->LI2C;
			break;
	}
	
	img_width = ImageInfo->Image_width;
	img_height= ImageInfo->Image_height;

	/* Aspect Ratio Conversion for DVD Contents */
	//if((img_width <= HDMI_EXCLUSIVE_UI_DVD_MAX_WD) && (img_height <= HDMI_EXCLUSIVE_UI_DVD_MAX_HT))
	{
		memset((void *)&aspect_ratio, 0x00, sizeof(exclusive_ui_ar_params));
		TCC_OUTPUT_EXCLUSIVE_UI_CfgAR(hdmi_lcdc, img_width, img_height, &aspect_ratio);
	}

	if(!ImageInfo->enable)	{
		#if defined(CONFIG_ARCH_TCC92XX)
		pLCDC_channel->LIC = 0;
		#else
		pLCDC_channel->LIC= HwLCT_RU;
		#endif//
		return;
	}

	/* Get the parameters for exclusive ui */
	TCC_OUTPUT_EXCLUSIVE_UI_GetParam(&hdmi_exclusive_ui_param);

	if(hdmi_exclusive_ui_param.enable)
	{
		/* Clear the on_the_flay for video */
		hdmi_exclusive_ui_onthefly = FALSE;

		/* Clear update flag and backup an information of the last video */
		TCC_OUTPUT_EXCLUSIVE_UI_SetImage(ImageInfo);

		/*-------------------------------------*/
		/*           INTERLACE VIDEO           */
		/* Update video data with exclusive ui */
		/*-------------------------------------*/
		if(hdmi_exclusive_ui_param.interlace)
		{
			/* Interlace BD Contents with 1920x1080 graphic plane */
			if(!TCC_OUTPUT_EXCLUSIVE_UI_ViqeMtoM(hdmi_lcdc, img_width, img_height))
			{
				if((img_width == lcd_width) && (img_height == lcd_height))
				{
					/* 1920x1080 HDMI Output */
				}
				else
				{
					/* 1280x720 HDMI Output */
				}
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(hdmi_lcdc, FALSE);

				/* Check the output format */
				if(ISZERO(pLCDC->LCTRL, HwLCTRL_NI))
					lcd_height = lcd_height >> 1;
			
				if(hdmi_exclusive_ui_param.clear == TRUE)
				{
					/* M2M Scaler without On_the_fly */
					if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(ImageInfo, hdmi_lcdc) < 0)
						hdmi_exclusive_ui_onthefly = FALSE;
					else
						hdmi_exclusive_ui_onthefly = TRUE;

					/* Update Screen */
					TCC_HDMI_DISPLAY_UPDATE(hdmi_lcdc, ImageInfo);
				}
				else
				{
					/* VIQE and M2M Scaler with On_the_fly */
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(ImageInfo, HDMI_EXCLUSIVE_UI_SCALER1, hdmi_exclusive_ui_param.plane_width, hdmi_exclusive_ui_param.plane_height, hdmi_exclusive_ui_idx, 0);

					if(TCC_OUTPUT_EXCLUSIVE_UI_GetUpdate())
					{
						dprintk("E\n");
					}
					else
					{
						/* Overlay Mixer */
						TCC_OUTPUT_EXCLUSIVE_UI_Mixer(ImageInfo, hdmi_exclusive_ui_idx, 0);
						/* Update overlay mixer image */
						TCC_OUTPUT_EXCLUSIVE_UI_Restore_Mixer(ImageInfo, hdmi_lcdc, output_width, output_height, hdmi_exclusive_ui_idx);
						/* Set the interrupt flag */
						mixer_interrupt_use = 1;
	 				}
				}
			}
		}
		else
		/*-------------------------------------*/
		/*          PROGRESSIVE VIDEO          */
		/* Update video data with exclusive ui */
		/*-------------------------------------*/
		{
			/* 1920x1080 Progressive BD Contents */
			if( (img_width == hdmi_exclusive_ui_param.plane_width) && (img_width == lcd_width) && (img_width == TCC_FB_OUT_MAX_WIDTH) &&
				(img_height == hdmi_exclusive_ui_param.plane_height) && (img_height == lcd_height) && (img_height == TCC_FB_OUT_MAX_HEIGHT) )
			{
				/* 1920x1080 HDMI Output */
			}
			else
			{
				/* Disable the lcdc channel_1 */
				TCC_OUTPUT_EXCLUSIVE_UI_Direct(hdmi_lcdc, FALSE);

				if(hdmi_exclusive_ui_param.clear == FALSE)
				{
					/* M2M Scaler without On_the_fly */
					TCC_OUTPUT_EXCLUSIVE_UI_Scaler(ImageInfo, HDMI_EXCLUSIVE_UI_SCALER0, hdmi_exclusive_ui_param.plane_width, hdmi_exclusive_ui_param.plane_height, hdmi_exclusive_ui_idx, 0);
					/* Overlay Mixer */
					TCC_OUTPUT_EXCLUSIVE_UI_Mixer(ImageInfo, hdmi_exclusive_ui_idx, 0);
				}

			#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
				/* M2M Scaler without On_the_fly */
				TCC_OUTPUT_EXCLUSIVE_UI_Scaler(ImageInfo, HDMI_EXCLUSIVE_UI_SCALER0, output_width, output_height, hdmi_exclusive_ui_idx, 1);
				//hdmi_exclusive_ui_onthefly = FALSE;
			#else
				/* M2M Scaler with On_the_fly */
				if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(ImageInfo, hdmi_lcdc) < 0)
					hdmi_exclusive_ui_onthefly = FALSE;
				else
					hdmi_exclusive_ui_onthefly = TRUE;
			#endif
			}

			#if 0 //shkim - on_the_fly
			/* Set the on_the_flay for android UI */
			if(hdmi_exclusive_ui_onthefly == FALSE)
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(hdmi_lcdc, 1, TRUE);
			else
				TCC_OUTPUT_EXCLUSIVE_UI_SetOnthefly(hdmi_lcdc, 1, FALSE);
			#endif
		}

		hdmi_exclusive_ui_idx = !hdmi_exclusive_ui_idx;
	}

	if(force_update && !mixer_interrupt_use)
	{
		TCC_HDMI_DISPLAY_UPDATE(hdmi_lcdc, ImageInfo);
	}
}

void TCC_HDMI_DISPLAY_IMAGE(exclusive_ui_update UpdateImage)
{
	/* M2M Scaler with On_the_fly */
	if(TCC_OUTPUT_EXCLUSIVE_UI_Scaler_OnTheFly(&UpdateImage.image, UpdateImage.lcdc) < 0)
		hdmi_exclusive_ui_onthefly = FALSE;
	else
		hdmi_exclusive_ui_onthefly = TRUE;

	dprintk("%s, hdmi_exclusive_ui_onthefly=%d\n", __func__, hdmi_exclusive_ui_onthefly);
			
	/* Update Screen */
	TCC_HDMI_DISPLAY_UPDATE(UpdateImage.lcdc, &UpdateImage.image);
}
#endif

