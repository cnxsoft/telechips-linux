#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <linux/delay.h>

#include <mach/vioc_rdma.h>
#include <mach/vioc_viqe.h>
#include <mach/vioc_scaler.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_disp.h>
#include <mach/vioc_config.h>
#include <mach/vioc_api.h>
#include <mach/vioc_plugin_tcc892x.h>
#include "tcc_vioc_viqe.h"
#include "tcc_vioc_viqe_interface.h"
#include <mach/tcc_viqe_ioctl.h>

#include <linux/fb.h>
#include <mach/tccfb_ioctrl.h>
#include <mach/tca_fb_output.h>
#include <plat/pmap.h>

#define USE_DEINTERLACE_S_IN30Hz
//#define USE_DEINTERLACE_S_IN60Hz
static pmap_t pmap_viqe;
#define PA_VIQE_BASE_ADDR	pmap_viqe.base
#define PA_VIQE_BASE_SIZE	pmap_viqe.size
static unsigned int gPMEM_VIQE_BASE;
static unsigned int gPMEM_VIQE_SIZE;

static VIQE *pVIQE_30Hz;
static VIOC_RDMA *pRDMABase_30Hz;
static VIOC_SC *pSCALERBase_30Hz;
static VIOC_DISP *pDISPBase_30Hz;
static VIOC_WMIX *pWMIXBase_30Hz;
static VIOC_IREQ_CONFIG *pIREQConfig_30Hz;
static int gRDMA_reg_30Hz = 0;
static int gSCALER_reg_30Hz = 0;
static int gVIQE_RDMA_num_30Hz = 0;
static int gSC_RDMA_num_30Hz = 0;
static int gSCALER_num_30Hz = 0;
static int gFrmCnt_30Hz = 0;
static int gusingScale_30Hz = 0;

static VIQE *pVIQE_60Hz;
static VIOC_RDMA *pRDMABase_60Hz;
static VIOC_SC *pSCALERBase_60Hz;
static VIOC_DISP *pDISPBase_60Hz;
static VIOC_WMIX *pWMIXBase_60Hz;
static VIOC_IREQ_CONFIG *pIREQConfig_60Hz;
static int gRDMA_reg_60Hz = 0;
static int gSCALER_reg_60Hz = 0;
static int gVIQE_RDMA_num_60Hz = 0;
static int gSC_RDMA_num_60Hz = 0;
static int gSCALER_num_60Hz = 0;
static int gFrmCnt_60Hz = 0;
static int gusingScale_60Hz = 0;

static int gbfield_30Hz =0;
static int gLcdc_layer_60Hz = -1;
static VIOC_VIQE_FMT_TYPE gViqe_fmt_60Hz;
static int gImg_fmt_60Hz = -1;
static int gpreCrop_left_60Hz = 0;
static int gpreCrop_right_60Hz = 0;
static int gpreCrop_top_60Hz = 0;
static int gpreCrop_bottom_60Hz = 0;

#ifdef USE_DEINTERLACE_S_IN30Hz
static VIOC_VIQE_DEINTL_MODE gDI_mode_30Hz = VIOC_VIQE_DEINTL_S;
#else
static VIOC_VIQE_DEINTL_MODE gDI_mode_30Hz = VIOC_VIQE_DEINTL_MODE_2D;
#endif

#ifdef USE_DEINTERLACE_S_IN60Hz
static int gusingDI_S = 0;
static VIOC_VIQE_DEINTL_MODE gDI_mode_60Hz = VIOC_VIQE_DEINTL_S;
#else
static VIOC_VIQE_DEINTL_MODE gDI_mode_60Hz = VIOC_VIQE_DEINTL_MODE_2D;
#endif

#if 0
#define dprintk(msg...)	 { printk( "tca_hdmi: " msg); }
#else
#define dprintk(msg...)	 
#endif

extern void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
					unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U,unsigned int* V);

/* VIQE Set */
//////////////////////////////////////////////////////////////////////////////////////////
void TCC_VIQE_DI_Init(int scalerCh, int useWMIXER, unsigned int srcWidth, unsigned int srcHeight,
						int crop_top, int crop_bottom, int crop_left, int crop_right, int OddFirst)
{
#ifndef USE_DEINTERLACE_S_IN30Hz
	unsigned int deintl_dma_base0, deintl_dma_base1, deintl_dma_base2, deintl_dma_base3;
	int imgSize;
	int top_size_dont_use = OFF;		//If this value is OFF, The size information is get from VIOC modules.
#endif
	unsigned int framebufWidth, framebufHeight;
	VIOC_VIQE_FMT_TYPE img_fmt = VIOC_VIQE_FMT_YUV420;

	pmap_get_info("viqe", &pmap_viqe);

	if(useWMIXER)
	{
		gRDMA_reg_30Hz = HwVIOC_RDMA14;
		gVIQE_RDMA_num_30Hz = VIOC_VIQE_RDMA_14;	
	}
	else
	{
		if(scalerCh == 0)
		{
			gRDMA_reg_30Hz = HwVIOC_RDMA12;
			gVIQE_RDMA_num_30Hz = VIOC_VIQE_RDMA_12;
		}
		else if(scalerCh == 1)
	{
			gRDMA_reg_30Hz = HwVIOC_RDMA02;//HwVIOC_RDMA14;
			gVIQE_RDMA_num_30Hz = VIOC_VIQE_RDMA_02;//VIOC_VIQE_RDMA_14;
		}
		else
	{
			gRDMA_reg_30Hz = HwVIOC_RDMA06;
			gVIQE_RDMA_num_30Hz = VIOC_VIQE_RDMA_06;
		}
	}
		
	gPMEM_VIQE_BASE = PA_VIQE_BASE_ADDR;
	gPMEM_VIQE_SIZE = PA_VIQE_BASE_SIZE;

	pVIQE_30Hz = (VIQE *)tcc_p2v(HwVIOC_VIQE0);
	pRDMABase_30Hz = (VIOC_RDMA *)tcc_p2v(gRDMA_reg_30Hz);
	
	crop_top = (crop_top >>1)<<1;
	framebufWidth = ((srcWidth - crop_left - crop_right) >> 3) << 3;			// 8bit align
	framebufHeight = ((srcHeight - crop_top - crop_bottom) >> 1) << 1;		// 2bit align

	printk("TCC_VIQE_DI_Init, W:%d, H:%d, FMT:%s, OddFirst:%d, RDMA:%d\n", framebufWidth, framebufHeight, (img_fmt?"YUV422":"YUV420"), OddFirst, ((gRDMA_reg_30Hz-HwVIOC_RDMA00)/256));
	
	VIOC_RDMA_SetImageY2REnable(pRDMABase_30Hz, FALSE);
	VIOC_RDMA_SetImageY2RMode(pRDMABase_30Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
	VIOC_RDMA_SetImageIntl(pRDMABase_30Hz, 1);
	VIOC_RDMA_SetImageBfield(pRDMABase_30Hz, OddFirst);
	
#ifdef USE_DEINTERLACE_S_IN30Hz
	VIOC_CONFIG_PlugIn(VIOC_DEINTLS, gVIQE_RDMA_num_30Hz);
#else
	// If you use 3D(temporal) De-interlace mode, you have to set physical address for using DMA register.
	//If 2D(spatial) mode, these registers are ignored
	imgSize = (framebufWidth * framebufHeight * 2);
	deintl_dma_base0	= gPMEM_VIQE_BASE;
	deintl_dma_base1	= deintl_dma_base0 + imgSize;
	deintl_dma_base2	= deintl_dma_base1 + imgSize;
	deintl_dma_base3	= deintl_dma_base2 + imgSize;	
	if (top_size_dont_use == OFF)
	{
		framebufWidth  = 0;
		framebufHeight = 0;
	}
	
	VIOC_VIQE_SetControlRegister(pVIQE_30Hz, framebufWidth, framebufHeight, img_fmt);
	VIOC_VIQE_SetDeintlRegister(pVIQE_30Hz, img_fmt, top_size_dont_use, framebufWidth, framebufHeight, gDI_mode_30Hz, deintl_dma_base0, deintl_dma_base1, deintl_dma_base2, deintl_dma_base3);
	VIOC_VIQE_SetControlEnable(pVIQE_30Hz, OFF, OFF, OFF, OFF, ON);
	if(gVIQE_RDMA_num_30Hz== VIOC_VIQE_RDMA_02)
	{
		VIOC_VIQE_SetImageY2REnable(pVIQE_30Hz, TRUE);
		VIOC_VIQE_SetImageY2RMode(pVIQE_30Hz, 0x02);
	}
	VIOC_CONFIG_PlugIn(VIOC_VIQE, gVIQE_RDMA_num_30Hz);
	if(OddFirst)
		gbfield_30Hz =1;
	else
		gbfield_30Hz =0;
#endif

	gFrmCnt_30Hz = 0;
}


void TCC_VIQE_DI_Run(unsigned int srcWidth, unsigned int srcHeight,	
						int crop_top, int crop_bottom, int crop_left, int crop_right, int OddFirst)
{
	if(gFrmCnt_30Hz == 0)
		printk("TCC_VIQE_DI_Run\n");

#ifndef USE_DEINTERLACE_S_IN30Hz
	if(gFrmCnt_30Hz == 3)
		VIOC_VIQE_SetDeintlMode(pVIQE_30Hz, VIOC_VIQE_DEINTL_MODE_3D);

	if (gbfield_30Hz) 					// end fied of bottom field
	{
		VIOC_RDMA_SetImageBfield(pRDMABase_30Hz, 0);				// change the bottom to top field
		// if you want to change the base address, you call the RDMA SetImageBase function in this line.
		gbfield_30Hz= 0;
	} 
	else 
	{
		VIOC_RDMA_SetImageBfield(pRDMABase_30Hz, 1);				// change the top to bottom field
		gbfield_30Hz = 1;
	}
#else
	VIOC_RDMA_SetImageY2REnable(pRDMABase_30Hz, FALSE);
	VIOC_RDMA_SetImageY2RMode(pRDMABase_30Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
#endif
	gFrmCnt_30Hz++;	
}

void TCC_VIQE_DI_DeInit(void)
{
	volatile PVIOC_IREQ_CONFIG pIREQConfig;
	pIREQConfig = (volatile PVIOC_IREQ_CONFIG)tcc_p2v((unsigned int)HwVIOC_IREQ);

	printk("TCC_VIQE_DI_DeInit\n");
#ifdef USE_DEINTERLACE_S_IN30Hz	
	VIOC_CONFIG_PlugOut(VIOC_DEINTLS);
	BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<17), (0x01<<17)); // DEINTLS reset
	BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<17), (0x00<<17)); // DEINTLS reset
#else
	VIOC_CONFIG_PlugOut(VIOC_VIQE);
	BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<16), (0x01<<16)); // VIQE reset
	BITCSET(pIREQConfig->uSOFTRESET.nREG[1], (0x1<<16), (0x00<<16)); // VIQE reset
#endif
}

/* 
	//img_fmt
	TCC_LCDC_IMG_FMT_YUV420SP = 24,	
	TCC_LCDC_IMG_FMT_YUV422SP = 25, 
	TCC_LCDC_IMG_FMT_YUYV = 26, 
	TCC_LCDC_IMG_FMT_YVYU = 27,
	
	TCC_LCDC_IMG_FMT_YUV420ITL0 = 28, 
	TCC_LCDC_IMG_FMT_YUV420ITL1 = 29, 
	TCC_LCDC_IMG_FMT_YUV422ITL0 = 30, 
	TCC_LCDC_IMG_FMT_YUV422ITL1 = 31, 
*/

//////////////////////////////////////////////////////////////////////////////////////////
void TCC_VIQE_DI_Init60Hz(int lcdCtrlNum, int Lcdc_layer, int useSCALER, unsigned int img_fmt, 
						unsigned int srcWidth, unsigned int srcHeight,
						unsigned int destWidth, unsigned int destHeight,
						unsigned int offset_x, unsigned int offset_y, int OddFirst)
{
	unsigned int deintl_dma_base0, deintl_dma_base1, deintl_dma_base2, deintl_dma_base3;
	unsigned int framebufWidth, framebufHeight;
	unsigned int lcd_width = 0, lcd_height = 0, scale_x = 0, scale_y = 0;
	int imgSize;
	int top_size_dont_use = OFF;		//If this value is OFF, The size information is get from VIOC modules.

	gLcdc_layer_60Hz = Lcdc_layer;
	
	if(img_fmt == 24 || img_fmt == 28 || img_fmt==29)
		gViqe_fmt_60Hz = VIOC_VIQE_FMT_YUV420;
	else
		gViqe_fmt_60Hz = VIOC_VIQE_FMT_YUV422;
	gImg_fmt_60Hz = img_fmt;
		
	pmap_get_info("viqe", &pmap_viqe);
	gPMEM_VIQE_BASE = PA_VIQE_BASE_ADDR;
	gPMEM_VIQE_SIZE = PA_VIQE_BASE_SIZE;

	if(lcdCtrlNum)
	{
		gRDMA_reg_60Hz = HwVIOC_RDMA06;
		gVIQE_RDMA_num_60Hz = VIOC_VIQE_RDMA_06;
		gSC_RDMA_num_60Hz = VIOC_SC_RDMA_06;
		gSCALER_reg_60Hz = HwVIOC_SC1;
		gSCALER_num_60Hz = VIOC_SC1;

		pDISPBase_60Hz = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pWMIXBase_60Hz = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);		
	}
	else
	{
		gRDMA_reg_60Hz = HwVIOC_RDMA02;
		gVIQE_RDMA_num_60Hz = VIOC_VIQE_RDMA_02;
		gSC_RDMA_num_60Hz = VIOC_SC_RDMA_02;
		gSCALER_reg_60Hz = HwVIOC_SC1;
		gSCALER_num_60Hz = VIOC_SC1;

		pDISPBase_60Hz = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pWMIXBase_60Hz = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);
	}

	pVIQE_60Hz = (VIQE *)tcc_p2v(HwVIOC_VIQE0);
	pRDMABase_60Hz = (VIOC_RDMA *)tcc_p2v(gRDMA_reg_60Hz);
	pSCALERBase_60Hz = (VIOC_SC *)tcc_p2v(gSCALER_reg_60Hz );
	pIREQConfig_60Hz = (VIOC_IREQ_CONFIG *)tcc_p2v(HwVIOC_IREQ);

	framebufWidth = ((srcWidth) >> 3) << 3;			// 8bit align
	framebufHeight = ((srcHeight) >> 1) << 1;		// 2bit align
	printk("TCC_VIQE_DI_Init60Hz, W:%d, H:%d, DW:%d, DH:%d, FMT:%d, VFMT:%s, OddFirst:%d, RDMA:%d\n", framebufWidth, framebufHeight, destWidth, destHeight, img_fmt, (gViqe_fmt_60Hz?"YUV422":"YUV420"), OddFirst, ((gRDMA_reg_60Hz-HwVIOC_RDMA00)/256));

	VIOC_DISP_GetSize(pDISPBase_60Hz, &lcd_width, &lcd_height);
	if((!lcd_width) || (!lcd_height))
	{
		printk("%s invalid lcd size\n", __func__);
		return;
	}

	//RDMA SETTING
#ifdef USE_DEINTERLACE_S_IN60Hz
	VIOC_RDMA_SetImageY2REnable(pRDMABase_60Hz, TRUE);
	VIOC_RDMA_SetImageY2RMode(pRDMABase_60Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
#else
#if 0
	if( gDI_mode == VIOC_VIQE_DEINTL_MODE_BYPASS)
	{
		VIOC_RDMA_SetImageY2REnable(pRDMABase_60Hz, TRUE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase_60Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
	}
	else
#endif		
	{
		VIOC_RDMA_SetImageY2REnable(pRDMABase_60Hz, FALSE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase_60Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
	}
#endif		
	VIOC_RDMA_SetImageOffset(pRDMABase_60Hz, img_fmt, framebufWidth);
	VIOC_RDMA_SetImageFormat(pRDMABase_60Hz, img_fmt);
	VIOC_RDMA_SetImageScale(pRDMABase_60Hz, scale_x, scale_y);
	VIOC_RDMA_SetImageSize(pRDMABase_60Hz, framebufWidth, framebufHeight);
	VIOC_RDMA_SetImageIntl(pRDMABase_60Hz, 1);
	VIOC_RDMA_SetImageBfield(pRDMABase_60Hz, OddFirst);


#ifdef USE_DEINTERLACE_S_IN60Hz
	deintl_dma_base0	= NULL;
	deintl_dma_base1	= NULL;
	deintl_dma_base2	= NULL;
	deintl_dma_base3	= NULL;
	VIOC_CONFIG_PlugIn(VIOC_DEINTLS, gVIQE_RDMA_num_60Hz);
	gusingDI_S = 1;
#else
	// If you use 3D(temporal) De-interlace mode, you have to set physical address for using DMA register.
	//If 2D(spatial) mode, these registers are ignored
	imgSize = (framebufWidth * framebufHeight * 2);
	deintl_dma_base0	= gPMEM_VIQE_BASE;
	deintl_dma_base1	= deintl_dma_base0 + imgSize;
	deintl_dma_base2	= deintl_dma_base1 + imgSize;
	deintl_dma_base3	= deintl_dma_base2 + imgSize;	

	VIOC_VIQE_SetControlRegister(pVIQE_60Hz, framebufWidth, framebufHeight, gViqe_fmt_60Hz);
	VIOC_VIQE_SetDeintlRegister(pVIQE_60Hz, gViqe_fmt_60Hz, top_size_dont_use, framebufWidth, framebufHeight, gDI_mode_60Hz, deintl_dma_base0, deintl_dma_base1, deintl_dma_base2, deintl_dma_base3);
//	VIOC_VIQE_SetDenoise(pVIQE_60Hz, gViqe_fmt_60Hz, framebufWidth, framebufHeight, 1, 0, deintl_dma_base0, deintl_dma_base1); 	//for bypass path on progressive frame
	VIOC_VIQE_SetControlEnable(pVIQE_60Hz, OFF, OFF, OFF, OFF, ON);
//		if(gDI_mode != VIOC_VIQE_DEINTL_MODE_BYPASS)
	{
		VIOC_VIQE_SetImageY2REnable(pVIQE_60Hz, TRUE);
		VIOC_VIQE_SetImageY2RMode(pVIQE_60Hz, 0x02);
	}
	VIOC_CONFIG_PlugIn(VIOC_VIQE, gVIQE_RDMA_num_60Hz);
#endif		

	//SCALER SETTING
	if(useSCALER)
	{
		VIOC_CONFIG_PlugIn (gSCALER_num_60Hz, gSC_RDMA_num_60Hz);			
		VIOC_SC_SetBypass (pSCALERBase_60Hz, OFF);
		
		VIOC_SC_SetDstSize (pSCALERBase_60Hz, destWidth, destHeight);			// set destination size in scaler
		VIOC_SC_SetOutSize (pSCALERBase_60Hz, destWidth, destHeight);			// set output size in scaer
		VIOC_SC_SetUpdate (pSCALERBase_60Hz);
		gusingScale_60Hz = 1;
	}
	
	VIOC_WMIX_SetPosition(pWMIXBase_60Hz, Lcdc_layer,  offset_x, offset_y);
	VIOC_WMIX_SetUpdate(pWMIXBase_60Hz);

	gFrmCnt_60Hz= 0;		
}


void TCC_VIQE_DI_Swap60Hz(int mode)
{
	VIOC_VIQE_SwapDeintlBase(pVIQE_60Hz, mode);
}
void TCC_VIQE_DI_SetFMT60Hz(int enable)
{
	VIOC_VIQE_SetDeintlFMT(pVIQE_60Hz, enable);
}

void TCC_VIQE_DI_Run60Hz(int useSCALER, unsigned int addr0, unsigned int addr1, unsigned int addr2,
						unsigned int srcWidth, unsigned int srcHeight,	
						int crop_top, int crop_bottom, int crop_left, int crop_right, 
						unsigned int destWidth, unsigned int destHeight, 
						unsigned int offset_x, unsigned int offset_y, int OddFirst, int FrameInfo_Interlace)
{
	unsigned int lcd_width = 0, lcd_height = 0;
	int cropWidth, cropHeight;

	VIOC_DISP_GetSize(pDISPBase_60Hz, &lcd_width, &lcd_height);
	if((!lcd_width) || (!lcd_height))
	{
		printk("%s invalid lcd size\n", __func__);
		return;
	}
	dprintk("%s lcd_width:%d, lcd_height:%d\n", __func__, lcd_width, lcd_height);

	crop_top = (crop_top >>1)<<1;
	cropWidth = crop_right - crop_left;
	cropHeight = crop_bottom - crop_top;
	{
		int addr_Y = (unsigned int)addr0;
		int addr_U = (unsigned int)addr1;
		int addr_V = (unsigned int)addr2;
#ifndef USE_DEINTERLACE_S_IN60Hz
		if((gFrmCnt_60Hz!=0) && ((gpreCrop_left_60Hz != crop_left) || (gpreCrop_right_60Hz !=crop_right) || (gpreCrop_top_60Hz != crop_top) || (gpreCrop_bottom_60Hz !=crop_bottom)))
		{
			unsigned int deintl_dma_base0, deintl_dma_base1, deintl_dma_base2, deintl_dma_base3;
			int imgSize;
			int top_size_dont_use = OFF;		//If this value is OFF, The size information is get from VIOC modules.
			VIOC_RDMA_SetImageDisable(pRDMABase_60Hz);	
			VIOC_CONFIG_PlugOut(VIOC_VIQE);

			BITCSET(pIREQConfig_60Hz->uSOFTRESET.nREG[1], (0x1<<16), (0x01<<16)); // VIQE reset
			BITCSET(pIREQConfig_60Hz->uSOFTRESET.nREG[1], (0x1<<16), (0x00<<16)); // VIQE reset

			imgSize = (srcWidth * srcHeight * 2);
			deintl_dma_base0	= gPMEM_VIQE_BASE;
			deintl_dma_base1	= deintl_dma_base0 + imgSize;
			deintl_dma_base2	= deintl_dma_base1 + imgSize;
			deintl_dma_base3	= deintl_dma_base2 + imgSize;	

			VIOC_VIQE_SetControlRegister(pVIQE_60Hz, srcWidth, srcHeight, gViqe_fmt_60Hz);
			VIOC_VIQE_SetDeintlRegister(pVIQE_60Hz, gViqe_fmt_60Hz, top_size_dont_use, srcWidth, srcHeight, gDI_mode_60Hz, deintl_dma_base0, deintl_dma_base1, deintl_dma_base2, deintl_dma_base3);
			VIOC_VIQE_SetDenoise(pVIQE_60Hz, gViqe_fmt_60Hz, srcWidth, srcHeight, 1, 0, deintl_dma_base0, deintl_dma_base1); 	//for bypass path on progressive frame
			VIOC_VIQE_SetImageY2REnable(pVIQE_60Hz, TRUE);
			VIOC_VIQE_SetImageY2RMode(pVIQE_60Hz, 0x02);
			VIOC_CONFIG_PlugIn(VIOC_VIQE, gVIQE_RDMA_num_60Hz);
		}
#endif			

		tccxxx_GetAddress(gImg_fmt_60Hz, (unsigned int)addr0, srcWidth, srcHeight, crop_left, crop_top, &addr_Y, &addr_U, &addr_V);
		
		VIOC_RDMA_SetImageSize(pRDMABase_60Hz, cropWidth, cropHeight);
		VIOC_RDMA_SetImageBase(pRDMABase_60Hz, addr_Y, addr_U, addr_V);
		gpreCrop_left_60Hz = crop_left;
		gpreCrop_right_60Hz = crop_right;
		gpreCrop_top_60Hz = crop_top;
		gpreCrop_bottom_60Hz = crop_bottom;
	}
	VIOC_RDMA_SetImageBfield(pRDMABase_60Hz, OddFirst);
	VIOC_RDMA_SetImageEnable(pRDMABase_60Hz);
	dprintk(" Image Crop left=[%d], right=[%d], top=[%d], bottom=[%d], W:%d, H:%d odd:%d\n", crop_left, crop_right, crop_top, crop_bottom, cropWidth, cropHeight, OddFirst);


#ifdef USE_DEINTERLACE_S_IN60Hz
	if(FrameInfo_Interlace)
	{
		VIOC_RDMA_SetImageIntl(pRDMABase_60Hz, 1);
		if(!gusingDI_S)
		{
			VIOC_CONFIG_PlugIn(VIOC_DEINTLS, gVIQE_RDMA_num_60Hz);
			gusingDI_S = 1;
		}
	}
	else
	{
		VIOC_RDMA_SetImageIntl(pRDMABase_60Hz, 0);
		if(gusingDI_S)
		{
			VIOC_CONFIG_PlugOut(VIOC_DEINTLS);
			gusingDI_S = 0;
		}
	}
#else
	if(FrameInfo_Interlace)
	{
		VIOC_RDMA_SetImageY2REnable(pRDMABase_60Hz, FALSE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase_60Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
		if(gFrmCnt_60Hz >= 3)
		{
			VIOC_VIQE_SetDeintlMode(pVIQE_60Hz, VIOC_VIQE_DEINTL_MODE_3D);
			gDI_mode_60Hz = VIOC_VIQE_DEINTL_MODE_3D;			
		}
		else
		{
			VIOC_VIQE_SetDeintlMode(pVIQE_60Hz, VIOC_VIQE_DEINTL_MODE_2D);
			gDI_mode_60Hz = VIOC_VIQE_DEINTL_MODE_2D;			
		}
		VIOC_VIQE_SetControlMode(pVIQE_60Hz, OFF, OFF, OFF, OFF, ON);
		VIOC_RDMA_SetImageIntl(pRDMABase_60Hz, 1);
	}
	else
	{	
		VIOC_RDMA_SetImageY2REnable(pRDMABase_60Hz, TRUE);
		VIOC_RDMA_SetImageY2RMode(pRDMABase_60Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
		VIOC_VIQE_SetControlMode(pVIQE_60Hz, OFF, OFF, OFF, OFF, OFF);
		VIOC_RDMA_SetImageIntl(pRDMABase_60Hz, 0);
		gFrmCnt_60Hz = 0;
	}	
#endif


	if(useSCALER)
	{
		if(!gusingScale_60Hz) {
			gusingScale_60Hz = 1;
			VIOC_CONFIG_PlugIn (gSCALER_num_60Hz, gSC_RDMA_num_60Hz);			
			VIOC_SC_SetBypass (pSCALERBase_60Hz, OFF);
		}
		
		VIOC_SC_SetDstSize (pSCALERBase_60Hz, destWidth, destHeight);			// set destination size in scaler
		VIOC_SC_SetOutSize (pSCALERBase_60Hz, destWidth, destHeight);			// set output size in scaer
		VIOC_SC_SetUpdate (pSCALERBase_60Hz);
	}
	else
	{
		if(gusingScale_60Hz == 1)	{
			VIOC_RDMA_SetImageDisable(pRDMABase_60Hz);
			VIOC_CONFIG_PlugOut(gSCALER_num_60Hz);
			gusingScale_60Hz = 0;
		}
	}

	// position
	VIOC_WMIX_SetPosition(pWMIXBase_60Hz, gLcdc_layer_60Hz,  offset_x, offset_y);
	VIOC_WMIX_SetUpdate(pWMIXBase_60Hz);

	gFrmCnt_60Hz++;	
}

void TCC_VIQE_DI_DeInit60Hz(void)
{
	printk("TCC_VIQE_DI_DeInit60Hz\n");
	VIOC_RDMA_SetImageDisable(pRDMABase_60Hz);	
	VIOC_RDMA_SetImageY2REnable(pRDMABase_60Hz, TRUE);
	VIOC_RDMA_SetImageY2RMode(pRDMABase_60Hz, 0x02); /* Y2RMode Default 0 (Studio Color) */
	VIOC_CONFIG_PlugOut(gSCALER_num_60Hz);
	gusingScale_60Hz = 0;
	
#ifdef USE_DEINTERLACE_S_IN60Hz
	VIOC_CONFIG_PlugOut(VIOC_DEINTLS);
	BITCSET(pIREQConfig_60Hz->uSOFTRESET.nREG[1], (0x1<<17), (0x01<<17)); // DEINTLS reset
	BITCSET(pIREQConfig_60Hz->uSOFTRESET.nREG[1], (0x1<<17), (0x00<<17)); // DEINTLS reset
	gusingDI_S = 0;
#else

	VIOC_CONFIG_PlugOut(VIOC_VIQE);
	BITCSET(pIREQConfig_60Hz->uSOFTRESET.nREG[1], (0x1<<16), (0x01<<16)); // VIQE reset
	BITCSET(pIREQConfig_60Hz->uSOFTRESET.nREG[1], (0x1<<16), (0x00<<16)); // VIQE reset
#endif
}

