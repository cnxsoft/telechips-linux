#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>
//#include <mach/tcc_used_mem.h>
#include <linux/delay.h>

#include "viqe_lib.h"
#include "tcc_viqe.h"
#include "tcc_viqe_interface.h"
#include <mach/tcc_viqe_ioctl.h>
#include "tcc_scaler.h"
#include <linux/fb.h>
#include <mach/tccfb_ioctrl.h>
#include <mach/tca_fb_output.h>
#include <plat/pmap.h>

#define VIQE_LCDC0			0
#define VIQE_LCDC1			1
#define VIQE_MSCL0			2
#define VIQE_MSCL1			3
#define MSCL0_LCDC0			(0 << 2)
#define MSCL0_LCDC1			(1 << 2)
#define MSCL0_NOTUSED		(2 << 2)
#define MSCL0_MSCL1			(3 << 2)
#define MSCL1_LCDC0			(0 << 4)
#define MSCL1_LCDC1			(1 << 4)
#define MSCL1_MSCL0			(2 << 4)
#define MSCL1_NOTUSED		(3 << 4)

#define RESTORE_NORMAL		0
#define SETTING_FOR_VIQE	1

#define LCDC_CH0		Hw0
#define LCDC_CH1		Hw1
#define LCDC_CH2		Hw2

static viqe_modes_t		viqe_modes;
static viqe_dma_t		viqe_dma;
static viqe_imgfmt_t		viqe_imgfmt;
static viqe_cfg_t		viqe_cfg;
static viqe_t			viqe;
static int				g_viqe_first = 1;
static char				g_viqe_m2m = 0;

static int g_viqe_mode = 0;
static int g_viqe_region = 0;
static int g_viqe_strength1 = 0;
static int g_viqe_strength2 = 0;
static int g_viqe_modeparam = 0;
static bool g_viqe_disp_modechange=false;

static pmap_t pmap_viqe;

#define PMEM_VIQE_BASE	pmap_viqe.base
#define PMEM_VIQE_SIZE	pmap_viqe.size

static unsigned int gPMEM_VIQE_BASE, gPMEM_VIQE_SIZE;
extern int TCCUTIL_PHYGetViqe(unsigned int *puiBase, unsigned int *puiSize);

long get_time(void)
{
	struct timeval timestamp;
	do_gettimeofday(&timestamp);

	return (long)(timestamp.tv_usec);
}

void TCC_DelayVIQE(int msec)
{
	volatile int Cnt;
	volatile int Cnt2;

	//printk ("InitWait\n");
	for (Cnt = 0; Cnt < 2000; Cnt++)
		for (Cnt2 = 0; Cnt2 < msec; Cnt2++)
			;
}

void tcc_lcdc_SetBaseAddr(int lcdController, int lcdChannel, unsigned int uiBase0, unsigned int uiBase1, unsigned int uiBase2)
{
	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_channel;

	if(lcdController == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	switch(lcdChannel)
	{
		case 0:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;

		case 1:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI1C);
			break;

		case 2:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI2C);
			break;

		default:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;
	}

	pstLCDC_channel->LIBA0 = uiBase0;
	pstLCDC_channel->LIBA1 = uiBase1;
	pstLCDC_channel->LIBA2 = uiBase2;

	BITSET(pstLCDC_channel->LIC, HwLCT_RU);
}

void tcc_lcdc_SetConfig(int lcdController, int lcdChannel, int width, int height, int x, int y, TCC_LCDC_IMG_FMT_TYPE fmt_type)
{
	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_channel;
	unsigned int buffer_width = 0, y2r_option = 0;
	unsigned int regl, lcd_width, lcd_height;
	unsigned int lcd_h_pos, lcd_w_pos;

	if(lcdController == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	switch(lcdChannel)
	{
		case 0:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;
		case 1:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI1C);
			break;
		case 2:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI2C);
			break;
		default:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;
	}

	regl = pstLCDC->LDS; // get LCD size
//	pstLCDC->LBC = 0x80202020;

	lcd_width = (regl & 0xFFFF);
	lcd_height = ((regl>>16) & 0xFFFF);

	buffer_width = (((width + 3)>>2)<<2);

	switch(fmt_type)
	{
		case TCC_LCDC_IMG_FMT_RGB444:
		case TCC_LCDC_IMG_FMT_RGB565:
		case TCC_LCDC_IMG_FMT_RGB555:
			pstLCDC_channel->LIO = buffer_width *2;
			y2r_option = HwLIC_Y2R;
			break;

		case TCC_LCDC_IMG_FMT_RGB888:
		case TCC_LCDC_IMG_FMT_RGB666:
			pstLCDC_channel->LIO = buffer_width * 4;
			y2r_option = HwLIC_Y2R;
			break;
		
		case TCC_LCDC_IMG_FMT_YUV420SP:
		case TCC_LCDC_IMG_FMT_YUV422SP:
			pstLCDC_channel->LIO = ((buffer_width / 2)<<16)|(buffer_width);
			y2r_option = HwLIC_Y2R;
			break;

		case TCC_LCDC_IMG_FMT_YUV422SQ:
			pstLCDC_channel->LIO = buffer_width * 2;
			y2r_option = HwLIC_Y2R;
			break;

		case TCC_LCDC_IMG_FMT_YUV420ITL0:
		case TCC_LCDC_IMG_FMT_YUV420ITL1:
		case TCC_LCDC_IMG_FMT_YUV422ITL0:
		case TCC_LCDC_IMG_FMT_YUV422ITL1:
			pstLCDC_channel->LIO = (buffer_width << 16)| (buffer_width);
			y2r_option = HwLIC_Y2R;
			break;

	}

	lcd_h_pos = 0;
	lcd_w_pos = 0;

//	printk("%s> Screen %d x %d, dst Size %d x %d, offset x:%d, y:%d\n", __func__, lcd_width, lcd_height, width, height, x, y);

	if(lcd_width < width + x)
		lcd_w_pos = (lcd_width - width);
	else
		lcd_w_pos = x;

	if(lcd_height < height + y)
		lcd_h_pos = (lcd_height - height);
	else
		lcd_h_pos = y;

	printk("%s> Screen %d x %d, dst Size %d x %d, offset x:%d, y:%d, fmt = %d\n", __func__, lcd_width, lcd_height, width, height, lcd_w_pos, lcd_h_pos, fmt_type);

// position
	pstLCDC_channel->LIP = ((lcd_h_pos << 16) | (lcd_w_pos));
// size
	pstLCDC_channel->LIS =((height << 16) | (width));
//channel control
	pstLCDC_channel->LIC = HwLCT_RU | y2r_option | fmt_type;
}

#if 1
int tcc_lcdc_ChOnOff(int lcdController, int lcdChannel, int onOff)
{
	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_channel;

	if(lcdController == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	switch(lcdChannel)
	{
		case 0:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;
		case 1:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI1C);
			break;
		case 2:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI2C);
			break;
		default:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;
	}

	if(onOff)
	{
		BITSET(pstLCDC_channel->LIC, HwLIC_IEN); /* channel image enable */
		BITSET(pstLCDC_channel->LIC, HwLCT_RU);
	}
	else
	{
		BITCLR(pstLCDC_channel->LIC, HwLIC_IEN); /* channel image disalbe */
		BITSET(pstLCDC_channel->LIC, HwLCT_RU);
	}

	return 1;
}
#else
static int gst_LCDController0 = 0, gst_LCDController1 = 0;
int tcc_lcdc_ChOnOff(int lcdController, int lcdChannel, int onOff)
{
	volatile PLCDC pstLCDC0, pstLCDC1;
	int channelEn = 0;

	pstLCDC0 = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	pstLCDC1 = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	if(onOff)
	{
		if(lcdController == 0)
			gst_LCDController0 |= Hw0;
		else
			gst_LCDController1 |= Hw0;
		
		if(gst_LCDController0 & Hw0)
		{
			BITSET(pstLCDC0->LI0C, HwLIC_IEN); /* channel image enable */
			BITSET(pstLCDC0->LI0C, HwLCT_RU);
		}

		if(gst_LCDController0 & Hw1)
		{
			BITSET(pstLCDC0->LI1C, HwLIC_IEN); /* channel image enable */
			BITSET(pstLCDC0->LI1C, HwLCT_RU);
		}

		if(gst_LCDController0 & Hw2)
		{
			BITSET(pstLCDC0->LI2C, HwLIC_IEN); /* channel image enable */
			BITSET(pstLCDC0->LI2C, HwLCT_RU);
		}
		gst_LCDController0 = 0;

		if(gst_LCDController1 & Hw0)
		{
			BITSET(pstLCDC1->LI0C, HwLIC_IEN); /* channel image enable */
			BITSET(pstLCDC1->LI0C, HwLCT_RU);
		}

		if(gst_LCDController1 & Hw1)
		{
			BITSET(pstLCDC1->LI1C, HwLIC_IEN); /* channel image enable */
			BITSET(pstLCDC1->LI1C, HwLCT_RU);
		}

		if(gst_LCDController1 & Hw2)
		{
			BITSET(pstLCDC1->LI2C, HwLIC_IEN); /* channel image enable */
			BITSET(pstLCDC1->LI2C, HwLCT_RU);
		}
		gst_LCDController1 = 0;
	}
	else
	{
		if(pstLCDC0->LI0C & HwLIC_IEN)
		{
			channelEn |= Hw0;
			BITCLR(pstLCDC0->LI0C, HwLIC_IEN); /* channel image disalbe */
			BITSET(pstLCDC0->LI0C, HwLCT_RU);
		}
		if(pstLCDC0->LI1C & HwLIC_IEN)
		{
			channelEn |= Hw1;
			BITCLR(pstLCDC0->LI1C, HwLIC_IEN); /* channel image disalbe */
			BITSET(pstLCDC0->LI1C, HwLCT_RU);
		}
		if(pstLCDC0->LI2C & HwLIC_IEN)
		{
			channelEn |= Hw2;
			BITCLR(pstLCDC0->LI2C, HwLIC_IEN); /* channel image disalbe */
			BITSET(pstLCDC0->LI2C, HwLCT_RU);
		}
		gst_LCDController0 = channelEn;

		channelEn = 0;

		if(pstLCDC1->LI0C & HwLIC_IEN)
		{
			channelEn |= Hw0;
			BITCLR(pstLCDC1->LI0C, HwLIC_IEN); /* channel image disalbe */
			BITSET(pstLCDC1->LI0C, HwLCT_RU);
		}
		if(pstLCDC1->LI1C & HwLIC_IEN)
		{
			channelEn |= Hw1;
			BITCLR(pstLCDC1->LI1C, HwLIC_IEN); /* channel image disalbe */
			BITSET(pstLCDC1->LI1C, HwLCT_RU);
		}
		if(pstLCDC1->LI2C & HwLIC_IEN)
		{
			channelEn |= Hw2;
			BITCLR(pstLCDC1->LI2C, HwLIC_IEN); /* channel image disalbe */
			BITSET(pstLCDC1->LI2C, HwLCT_RU);
		}
		gst_LCDController1 = channelEn;
	}

	return 1;
}
#endif
void tcc_lcdc_SetPathConfig(int lcdController, int lcdChannel, int OnTheFly)
{
	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_channel;

	if(lcdController == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	switch(lcdChannel)
	{
		case 0:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;
		case 1:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI1C);
			break;
		case 2:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI2C);
			break;
		default:
			pstLCDC_channel = (volatile PLCDC_CHANNEL)&(pstLCDC->LI0C);
			break;
	}

	if(OnTheFly == 1)
		BITSET(pstLCDC_channel->LIC, HwLIC_SRC); /* channel image enable */
	else
		BITCLR(pstLCDC_channel->LIC, HwLIC_SRC); /* channel image enable */

	BITSET(pstLCDC_channel->LIC, HwLCT_RU);
}

void tcc_DDICacheSetting(int mode)
{
	PDDICACHE pHwDDICACHE  = (volatile PDDICACHE)tcc_p2v(HwDDICACHE_BASE);

	if(mode == RESTORE_NORMAL)
	{
		#if defined(CONFIG_ARCH_TCC93XX)
			/* Cacheabe */
			pHwDDICACHE->DDIC0_CFG0 = (DDIC_LCD0_DMA_0_0)
									| (DDIC_LCD0_DMA_0_1	<< 4)
									| (DDIC_LCD0_DMA_2		<< 8)
									| (DDIC_MSCL0_DMA_0		<< 12)
									| (DDIC_MSCL0_DMA_1		<< 16)
									| (DDIC_LCD0_DMA_1		<< 20);
			/* Non-Cacheable */
			pHwDDICACHE->DDIC0_CFG1 = (DDIC_LCD0_DMA_0_2)
									| (DDIC_MSCL0_DMA_2		<< 4)
									| (DDIC_VIQE_DMA_0_0	<< 8)
									| (DDIC_VIQE_DMA_0_1	<< 12)
									| (DDIC_VIQE_DMA_0_2	<< 16);
			/* Cacheable */
			pHwDDICACHE->DDIC1_CFG0 = (DDIC_LCD1_DMA_0_0)
									| (DDIC_LCD1_DMA_0_1	<< 4)
									| (DDIC_LCD1_DMA_2		<< 8)
									| (DDIC_MSCL1_DMA_0		<< 12)
									| (DDIC_MSCL1_DMA_1		<< 16)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_LCD1_DMA_1		<< 20);
								#else
									| (DDIC_MSCL1_DMA_2		<< 20);
								#endif
			/* Non-Cacheable */
			pHwDDICACHE->DDIC1_CFG1 = (DDIC_LCD1_DMA_0_2)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_MSCL1_DMA_2		<< 4)
								#else
									| (DDIC_LCD1_DMA_1		<< 4)
								#endif
									| (DDIC_VIQE_DMA_1_0	<< 8)
									| (DDIC_VIQE_DMA_1_1	<< 12)
									| (DDIC_VIQE_DMA_1_2	<< 16)
									| (DDIC_VIQE_DMA_2_0	<< 20)
									| (DDIC_VIQE_DMA_2_1	<< 24)
									| (DDIC_VIQE_DMA_2_2	<< 28);
		#else
			/* Cacheabe */
			pHwDDICACHE->DDIC0_CFG0 = (DDIC_MSCL0_DMA_0 )
									| (DDIC_MSCL0_DMA_1 	<< 4)
									| (DDIC_LCD0_DMA_1 		<< 8)
									| (DDIC_LCD0_DMA_0_0	<< 12)
									| (DDIC_LCD0_DMA_0_1	<< 16)
									| (DDIC_LCD0_DMA_2		<< 20);
			/* Non-Cacheable */
			pHwDDICACHE->DDIC0_CFG1 = (DDIC_VIQE_DMA_0_0)
									| (DDIC_VIQE_DMA_0_1	<< 4)
									| (DDIC_LCD0_DMA_0_2	<< 8)
									| (DDIC_MSCL0_DMA_2		<< 12)
									| (DDIC_VIQE_DMA_0_2	<< 16)
									| (DDIC_LCD0_DMA_3		<< 20)
									| (DDIC_LCD1_DMA_3		<< 24)
									| (DDIC_Reserved		<< 28);
			/* Cacheable */
			pHwDDICACHE->DDIC1_CFG0 = (DDIC_MSCL1_DMA_0 )
									| (DDIC_MSCL1_DMA_1 	<< 4)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_LCD1_DMA_1 		<< 8)
								#else
									| (DDIC_MSCL1_DMA_2 	<< 8)
								#endif
									| (DDIC_LCD1_DMA_0_0	<< 12)
									| (DDIC_LCD1_DMA_0_1	<< 16)
									| (DDIC_LCD1_DMA_2		<< 20);
			/* Non-Cacheable */
			pHwDDICACHE->DDIC1_CFG1 = (DDIC_VIQE_DMA_1_0)
									| (DDIC_VIQE_DMA_1_1	<< 4)
									| (DDIC_LCD1_DMA_0_2	<< 8)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_MSCL1_DMA_2		<< 12)
								#else
									| (DDIC_LCD1_DMA_1		<< 12)
								#endif
									| (DDIC_VIQE_DMA_1_2	<< 16)
									| (DDIC_VIQE_DMA_2_0	<< 20)
									| (DDIC_VIQE_DMA_2_1	<< 24)
									| (DDIC_VIQE_DMA_2_2	<< 28);
		#endif
	}
	else	// SETTING_FOR_VIQE
	{
		#if defined(CONFIG_ARCH_TCC93XX)
			/* Cacheabe */
			pHwDDICACHE->DDIC0_CFG0 = (DDIC_MSCL0_DMA_0)
									| (DDIC_LCD0_DMA_1		<< 4)
									| (DDIC_LCD0_DMA_2		<< 8)
									| (DDIC_VIQE_DMA_0_0	<< 12)
									| (DDIC_VIQE_DMA_0_1	<< 16)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_MSCL0_DMA_1		<< 20);
								#else
									| (DDIC_VIQE_DMA_0_2	<< 20);
								#endif
			/* Non-Cacheable */
			pHwDDICACHE->DDIC0_CFG1 = (DDIC_LCD0_DMA_0_0)
									| (DDIC_LCD0_DMA_0_1	<< 4)
									| (DDIC_LCD0_DMA_0_2	<< 8)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_VIQE_DMA_0_2	<< 12)
								#else
									| (DDIC_MSCL0_DMA_1		<< 12)
								#endif
									| (DDIC_MSCL0_DMA_2		<< 16);
			/* Cacheable */
			pHwDDICACHE->DDIC1_CFG0 = (DDIC_MSCL1_DMA_0)
									| (DDIC_MSCL1_DMA_1		<< 4)
									| (DDIC_LCD1_DMA_2		<< 8)
									| (DDIC_VIQE_DMA_1_0	<< 12)
									| (DDIC_VIQE_DMA_1_1	<< 16)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_LCD1_DMA_1		<< 20);
								#else
									| (DDIC_VIQE_DMA_1_2	<< 20);
								#endif
			/* Non-Cacheable */
			pHwDDICACHE->DDIC1_CFG1 = 
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									(DDIC_VIQE_DMA_1_2)
								#else
									(DDIC_LCD1_DMA_1)
								#endif
									| (DDIC_LCD1_DMA_0_0	<< 4)
									| (DDIC_LCD1_DMA_0_1	<< 8)
									| (DDIC_LCD1_DMA_0_2	<< 12)
									| (DDIC_VIQE_DMA_2_0	<< 16)
									| (DDIC_VIQE_DMA_2_1	<< 20)
									| (DDIC_VIQE_DMA_2_2	<< 24)
									| (DDIC_MSCL1_DMA_2		<< 28);
		#else
			/* Cacheabe */
			pHwDDICACHE->DDIC0_CFG0 = (DDIC_MSCL0_DMA_0)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_MSCL0_DMA_1		<< 4)
								#else
									| (DDIC_VIQE_DMA_0_2	<< 4)
								#endif
									| (DDIC_LCD0_DMA_1		<< 8)
									| (DDIC_VIQE_DMA_0_0	<< 12)
									| (DDIC_VIQE_DMA_0_1	<< 16)
									| (DDIC_LCD0_DMA_2		<< 20);
			/* Non-Cacheable */
			pHwDDICACHE->DDIC0_CFG1 = (DDIC_LCD0_DMA_0_0)
									| (DDIC_LCD0_DMA_0_1	<< 4)
									| (DDIC_LCD0_DMA_0_2	<< 8)
									| (DDIC_MSCL0_DMA_2		<< 12)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_VIQE_DMA_0_2	<< 16)
								#else
									| (DDIC_MSCL0_DMA_1		<< 16)
								#endif
									| (DDIC_LCD0_DMA_3		<< 20)
									| (DDIC_LCD1_DMA_3		<< 24)
									| (DDIC_Reserved		<< 28);
			/* Cacheable */
			pHwDDICACHE->DDIC1_CFG0 = (DDIC_MSCL1_DMA_0)
									| (DDIC_MSCL1_DMA_1		<< 4)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_LCD1_DMA_1		<< 8)
								#else
									| (DDIC_VIQE_DMA_1_2	<< 8)
								#endif
									| (DDIC_VIQE_DMA_1_0	<< 12)
									| (DDIC_VIQE_DMA_1_1	<< 16)
									| (DDIC_LCD1_DMA_2		<< 20);
			/* Non-Cacheable */
			pHwDDICACHE->DDIC1_CFG1 = (DDIC_LCD1_DMA_0_0)
									| (DDIC_LCD1_DMA_0_1	<< 4)
									| (DDIC_LCD1_DMA_0_2	<< 8)
								#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
									| (DDIC_VIQE_DMA_1_2	<< 12)
								#else
									| (DDIC_LCD1_DMA_1		<< 12)
								#endif
									| (DDIC_MSCL1_DMA_2		<< 16)
									| (DDIC_VIQE_DMA_2_0	<< 20)
									| (DDIC_VIQE_DMA_2_1	<< 24)
									| (DDIC_VIQE_DMA_2_2	<< 28);
		#endif
	}

	pHwDDICACHE->DDIC0_CH0_AMIN = 0x00000000  ;       //  0x020   R/W     0x00000000      DDI_CACHE0 Ch0 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC0_CH0_AMAX = 0xFFFFFFFF  ;       //  0x024   R/W     0x00000000      DDI_CACHE0 Ch0 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC0_CH1_AMIN = 0x00000000  ;       //  0x028   R/W     0x00000000      DDI_CACHE0 Ch1 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC0_CH1_AMAX = 0xFFFFFFFF  ;       //  0x02C   R/W     0x00000000      DDI_CACHE0 Ch1 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC0_CH2_AMIN = 0x00000000  ;       //  0x030   R/W     0x00000000      DDI_CACHE0 Ch2 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC0_CH2_AMAX = 0xFFFFFFFF  ;       //  0x034   R/W     0x00000000      DDI_CACHE0 Ch2 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC0_CH3_AMIN = 0x00000000  ;       //  0x038   R/W     0x00000000      DDI_CACHE0 Ch3 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC0_CH3_AMAX = 0xFFFFFFFF  ;       //  0x03C   R/W     0x00000000      DDI_CACHE0 Ch3 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC0_CH4_AMIN = 0x00000000  ;       //  0x040   R/W     0x00000000      DDI_CACHE0 Ch4 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC0_CH4_AMAX = 0xFFFFFFFF  ;       //  0x044   R/W     0x00000000      DDI_CACHE0 Ch4 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC0_CH5_AMIN = 0x00000000  ;       //  0x048   R/W     0x00000000      DDI_CACHE0 Ch5 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC0_CH5_AMAX = 0xFFFFFFFF  ;       //  0x04C   R/W     0x00000000      DDI_CACHE0 Ch5 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC1_CH0_AMIN = 0x00000000  ;       //  0x050   R/W     0x00000000      DDI_CACHE1 Ch0 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC1_CH0_AMAX = 0xFFFFFFFF  ;       //  0x054   R/W     0x00000000      DDI_CACHE1 Ch0 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC1_CH1_AMIN = 0x00000000  ;       //  0x058   R/W     0x00000000      DDI_CACHE1 Ch1 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC1_CH1_AMAX = 0xFFFFFFFF  ;       //  0x05C   R/W     0x00000000      DDI_CACHE1 Ch1 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC1_CH2_AMIN = 0x00000000  ;       //  0x060   R/W     0x00000000      DDI_CACHE1 Ch2 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC1_CH2_AMAX = 0xFFFFFFFF  ;       //  0x064   R/W     0x00000000      DDI_CACHE1 Ch2 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC1_CH3_AMIN = 0x00000000  ;       //  0x068   R/W     0x00000000      DDI_CACHE1 Ch3 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC1_CH3_AMAX = 0xFFFFFFFF  ;       //  0x06C   R/W     0x00000000      DDI_CACHE1 Ch3 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC1_CH4_AMIN = 0x00000000  ;       //  0x070   R/W     0x00000000      DDI_CACHE1 Ch4 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC1_CH4_AMAX = 0xFFFFFFFF  ;       //  0x074   R/W     0x00000000      DDI_CACHE1 Ch4 Cacheable Address Range (MAX)
	pHwDDICACHE->DDIC1_CH5_AMIN = 0x00000000  ;       //  0x078   R/W     0x00000000      DDI_CACHE1 Ch5 Cacheable Address Range (MIN)
	pHwDDICACHE->DDIC1_CH5_AMAX = 0xFFFFFFFF  ;       //  0x07C   R/W     0x00000000      DDI_CACHE1 Ch5 Cacheable Address Range (MAX)

	pHwDDICACHE->DDIC_CTRL = 0x003F003F;
}

#ifdef CONFIG_ARCH_TCC88XX
void tcc_BusPrioritySetting(int mode)
{
	volatile unsigned int *pBUSPriorityRead = (volatile unsigned int *)tcc_p2v(HwBUSPRIORITY_READ);
	volatile unsigned int *pBUSPriorityWrite = (volatile unsigned int *)tcc_p2v(HwBUSPRIORITY_WRITE);

	if(mode == SETTING_FOR_VIQE)
	{
		*pBUSPriorityRead = HwBUSPRIORITY_IOBUS		| HwBUSPRIORITY_4; 	//IOBUS		: 4
		*pBUSPriorityRead = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_3; 	//GBUS	 	: 3
		*pBUSPriorityRead = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_2; 	//CPUBUS	: 2
		*pBUSPriorityRead = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
		*pBUSPriorityRead = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_1; 	//VBUS		: 1
		*pBUSPriorityRead = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_5; 	//HSIOBUS	: 5
		*pBUSPriorityRead = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_6; 	//CAMBUS	: 6

		*pBUSPriorityWrite = HwBUSPRIORITY_IOBUS	| HwBUSPRIORITY_4; 	//IOBUS		: 4
		*pBUSPriorityWrite = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_3; 	//GBUS	 	: 3
		*pBUSPriorityWrite = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_2; 	//CPUBUS	: 2
		*pBUSPriorityWrite = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
		*pBUSPriorityWrite = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_1; 	//VBUS		: 1
		*pBUSPriorityWrite = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_5; 	//HSIOBUS	: 5
		*pBUSPriorityWrite = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_6; 	//CAMBUS	: 6
	}
	else
	{
		*pBUSPriorityRead = HwBUSPRIORITY_IOBUS		| HwBUSPRIORITY_0; 	//IOBUS		: 0
		*pBUSPriorityRead = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_0; 	//GBUS	 	: 0
		*pBUSPriorityRead = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_0; 	//CPUBUS	: 0
		*pBUSPriorityRead = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
		*pBUSPriorityRead = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_0; 	//VBUS		: 0
		*pBUSPriorityRead = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_0; 	//HSIOBUS	: 0
		*pBUSPriorityRead = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_0; 	//CAMBUS	: 0

		*pBUSPriorityWrite = HwBUSPRIORITY_IOBUS	| HwBUSPRIORITY_0; 	//IOBUS		: 0
		*pBUSPriorityWrite = HwBUSPRIORITY_GBUS		| HwBUSPRIORITY_0; 	//GBUS	 	: 0
		*pBUSPriorityWrite = HwBUSPRIORITY_CPUBUS	| HwBUSPRIORITY_0; 	//CPUBUS	: 0
		*pBUSPriorityWrite = HwBUSPRIORITY_DDIBUS	| HwBUSPRIORITY_0; 	//DDIBUS	: 0
		*pBUSPriorityWrite = HwBUSPRIORITY_VBUS		| HwBUSPRIORITY_0; 	//VBUS		: 0
		*pBUSPriorityWrite = HwBUSPRIORITY_HSIOBUS	| HwBUSPRIORITY_0; 	//HSIOBUS	: 0
		*pBUSPriorityWrite = HwBUSPRIORITY_CAMBUS	| HwBUSPRIORITY_0; 	//CAMBUS	: 0
	}
}

unsigned int tcc_GetBusPriority(int ch, int rw)		// write channel = 0, read channel = 1
{
	unsigned int priority;

	*(volatile unsigned int *)(HwMULTILAYERBUSMATRIX_BASE) = (0xff << 24) | (ch << 0);

	if(rw == 0)		// rw=0 write
	{
		volatile unsigned int *pHwBusPriority = (volatile unsigned int *)tcc_p2v(HwBUSPRIORITY_WRITE);

		priority = *pHwBusPriority;
	}
	else
	{
		volatile unsigned int *pHwBusPriority = (volatile unsigned int *)tcc_p2v(HwBUSPRIORITY_READ);

		priority = *pHwBusPriority;
	}

	priority = (priority >> 8) & 0xff;

	return priority;
}

#endif
/****************************************************************************
  VIQE_SetStrength()

  Description :
  Parameter :
  Return :

****************************************************************************/
void VIQE_SetStrength(VIQEMODE eMode, int eStrengthY, int eStrengthC)
{
	viqe_strengthInfo stInfo;
	stInfo.Mode = eMode;
	stInfo.StrengthY = eStrengthY;
	stInfo.StrengthC = eStrengthC;

	//ioctl(viqe_dev_fd, IOCTL_VIQE_STRENGTH, &stInfo);
	tcc_viqe_ctrl(IOCTL_VIQE_STRENGTH, &stInfo);
}


/****************************************************************************
  VIQE_SetDIDNProcessingRegion()

  Description :
  Parameter :
  Return :

****************************************************************************/
void TCC_VIQE_SetProcessingRegion (VIQEMODE eMode)
{
	//ioctl(viqe_dev_fd, IOCTL_VIQE_REGION, &eMode);
	tcc_viqe_ctrl(IOCTL_VIQE_REGION, &eMode);
}

/****************************************************************************
  VIQE_ModeOnOff()

  Description :
  Parameter :
  Return :

****************************************************************************/
int TCC_SetModeVIQE(unsigned int eVIQEMode)
{
	if(eVIQEMode & VIQE_MODE_DI) viqe.modes->di = ON;
	else viqe.modes->di = OFF;

	if(eVIQEMode & VIQE_MODE_DISP) viqe.modes->disp = ON;
	else viqe.modes->disp = OFF;

	if(eVIQEMode & VIQE_MODE_DNTS) viqe.modes->dnts = ON;
	else viqe.modes->dnts = OFF;

	if(eVIQEMode & VIQE_MODE_LPF) viqe.modes->dnsp = ON;
	else viqe.modes->dnsp = OFF;

	if(eVIQEMode & VIQE_MODE_HPF) viqe.modes->hpf = ON;
	else viqe.modes->hpf = OFF;

	if(eVIQEMode & VIQE_MODE_HIST) viqe.modes->his = ON;
	else viqe.modes->his = OFF;

	if(eVIQEMode & VIQE_MODE_ODD_FIRST)	viqe.modes->oddfirst = ON;
	else viqe.modes->oddfirst = OFF;

	if (viqe.modes->di == OFF || (viqe.modes->di == ON && viqe.modes->disp == ON))
	{
		if (viqe.modes->hpf == ON && viqe.modes->dnsp == ON)
		{
			viqe.modes->hpf = OFF;
			viqe.modes->dnsp= OFF;
		}
	}
	else if (viqe.modes->di == ON && viqe.modes->disp == OFF)
	{
		if (viqe.modes->hpf == ON || viqe.modes->dnsp == ON)
		{
			viqe.modes->hpf = OFF;
			viqe.modes->dnsp= OFF;
		}
	}

	//printk("TCC_SetModeVIQE: DI-%d, DISP-%d, DNTS-%d, LPF-%d, HPF-%d, HIST-%d, ODDFIRST-%d",
	//	viqe.modes->di, viqe.modes->disp, viqe.modes->dnts, viqe.modes->dnsp, viqe.modes->hpf, viqe.modes->his, viqe.modes->oddfirst);

	if(tcc_viqe_ctrl(IOCTL_VIQE_SET_MODE, viqe.modes) < 0)
		return -1;

	return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////
void TCC_InitalizeVIQE(int lcdCtrlNo, int scalerCh, int OnTheFly, int DI_use, unsigned int FrmWidth, unsigned int FrmHeight,
						int crop_top, int crop_bottom, int crop_left, int crop_right)
{
	unsigned int framebufWidth; 
	unsigned int framebufHeight;
	unsigned int framebufStride;
	int compbufHeight;
	int complumaHeight;
	int compOneFrameSize;

	crop_top = (crop_top >>1)<<1;
	framebufWidth = ((FrmWidth - crop_left - crop_right) >> 3) << 3;			// 8bit align
	framebufHeight = ((FrmHeight - crop_top - crop_bottom) >> 1) << 1;		// 2bit align
	framebufStride = FrmWidth;
	
	#if defined(CONFIG_ARCH_TCC88XX)
		tcc_BusPrioritySetting(SETTING_FOR_VIQE);
	#endif
#if 0
	pmap_t pmap_viqe;

	pmap_get_info("viqe", &pmap_viqe);
	gPMEM_VIQE_BASE = pmap_viqe.base;
	gPMEM_VIQE_SIZE = pmap_viqe.size;
#else
	if(g_viqe_first == 0)
		TCC_DeInitializeVIQE(lcdCtrlNo, scalerCh, DI_use);

	pmap_get_info("viqe", &pmap_viqe);

	gPMEM_VIQE_BASE = PMEM_VIQE_BASE;
	gPMEM_VIQE_SIZE = PMEM_VIQE_SIZE;

#endif

	tcc_viqe_ctrl(IOCTL_VIQE_OFF, 0);
	//tcc_ddi_cache_setting(); it needs to viqe_sc_lcd onthefly.

	//ioctl(viqe_dev_fd, IOCTL_VIQE_ON, 0);
	tcc_viqe_ctrl(IOCTL_VIQE_ON, 0);

	memset(&viqe_modes, 0, sizeof(viqe_modes_t));
	memset(&viqe_dma, 0, sizeof(viqe_dma_t));
	memset(&viqe_imgfmt, 0, sizeof(viqe_imgfmt_t));
	memset(&viqe_cfg, 0, sizeof(viqe_cfg_t));

	viqe.modes = &viqe_modes;
	viqe.dma = &viqe_dma;
	viqe.imgfmt = &viqe_imgfmt;
	viqe.cfg = &viqe_cfg;

	viqe.cfg->comp_en = OFF;

	if(OnTheFly == 1)
		viqe.cfg->frm_manual_restart = OFF;
	else
		viqe.cfg->frm_manual_restart = ON;
	
	if(OnTheFly == 1)
	{
		viqe.cfg->directpath_en = ON;

		if(lcdCtrlNo)
		{
			if(scalerCh == 0)
				viqe.cfg->directpath_op = VIQE_MSCL0 + MSCL0_NOTUSED + MSCL1_NOTUSED;		// M2M Scaler ch 0
			else
				viqe.cfg->directpath_op = VIQE_MSCL1 + MSCL1_NOTUSED + MSCL0_NOTUSED;		// M2M Scaler ch 1
		}
		else
		{
			if(scalerCh == 0)
				viqe.cfg->directpath_op = VIQE_MSCL0 + MSCL0_NOTUSED + MSCL1_NOTUSED;		// M2M Scaler ch 1	viqe.cfg->overlap_disable = ON;
			else
				viqe.cfg->directpath_op = VIQE_MSCL1 + MSCL1_NOTUSED + MSCL0_NOTUSED;		// M2M Scaler ch 1	viqe.cfg->overlap_disable = ON;
		}
	}
	else
	{
		viqe.cfg->directpath_en = OFF;
		viqe.cfg->directpath_op = MSCL0_NOTUSED + MSCL1_NOTUSED;
	}

//	printk("[%s] Crop Info top:%d, bottom:%d, left:%d, right:%d, CropW:%d, CropH:%d, SrcW:%d\n", __func__, crop_top, crop_bottom, crop_left, crop_right, framebufWidth, framebufHeight, framebufStride);
	printk("[%s] viqe.cfg->directpath_en %d, viqe.cfg->directpath_op %x\n", __func__, viqe.cfg->directpath_en, viqe.cfg->directpath_op);

	viqe.cfg->overlap_disable = ON;

	viqe.width = framebufWidth;
	viqe.height = framebufHeight;

	viqe.imgfmt->i_fmt = FMT_YUV420IL0;		//FMT_YUV420IL0

	if ( viqe.cfg->directpath_en )
		viqe.imgfmt->o_fmt = OD_YUV422SEQ0;
	else
		viqe.imgfmt->o_fmt = OD_YUV420SEP;
	
	if(DI_use == 0)
		viqe.imgfmt->o_fmt = OD_YUV420INT;
	
	viqe.imgfmt->fmt_conv_disable = viqe.imgfmt->force_420 = OFF;
	viqe.imgfmt->first_frm_init = ON;

	viqe.dma->conti = ON;
	viqe.dma->opt = ON;
	viqe.dma->comp_pingpong = ON;

	viqe.dma->i_offs[0] = framebufStride;
	if ( viqe.imgfmt->i_fmt == FMT_YUV420SEP ) 
		viqe.dma->i_offs[1] = framebufStride>>1;
	else if ( (viqe.imgfmt->i_fmt == FMT_YUV420IL0) || (viqe.imgfmt->i_fmt == FMT_YUV420IL1) )
		viqe.dma->i_offs[1] = framebufStride;
	
	viqe.dma->o_offs[0] = framebufWidth;
	if( (viqe.imgfmt->o_fmt == OD_YUV422SEQ0) || (viqe.imgfmt->o_fmt == OD_YUV422INT) || (viqe.imgfmt->o_fmt == OD_YUV420INT) )
		viqe.dma->o_offs[1] = framebufWidth;
	else
		viqe.dma->o_offs[1] = framebufWidth>>1;

	complumaHeight = framebufHeight;
	if (viqe.imgfmt->force_420 == ON)
		compbufHeight = 2*framebufHeight;
	else 
		compbufHeight = framebufHeight;
	
	if ( viqe.dma->comp_pingpong == ON )
	{
		viqe.dma->addr_comp0[0] = gPMEM_VIQE_BASE;
		viqe.dma->addr_comp0[1] = viqe.dma->addr_comp0[0] + ( framebufStride*complumaHeight) / 2;
		viqe.dma->addr_comp0[2] = viqe.dma->addr_comp0[1] + ( (framebufStride*(compbufHeight>>2)) ) /2;
		compOneFrameSize = framebufStride*((compbufHeight >> 1) + complumaHeight);
		viqe.dma->addr_comp1[0] = viqe.dma->addr_comp0[0] + (compOneFrameSize) / 2;
		viqe.dma->addr_comp1[1] = viqe.dma->addr_comp0[1] + (compOneFrameSize) / 2;
		viqe.dma->addr_comp1[2] = viqe.dma->addr_comp0[2] + (compOneFrameSize) / 2;
	}
	else
	{
		viqe.dma->addr_comp0[0] = gPMEM_VIQE_BASE;
		viqe.dma->addr_comp0[1] = viqe.dma->addr_comp0[0] + (( framebufStride*complumaHeight )/2);
		viqe.dma->addr_comp0[2] = viqe.dma->addr_comp0[1] + (( (framebufStride*(compbufHeight>>2)) )/2);
	}

	//ioctl(viqe_dev_fd, IOCTL_VIQE_SET_INIT, &viqe);
	tcc_viqe_ctrl(IOCTL_VIQE_SET_INIT, &viqe);

	g_viqe_first = 1;
	g_viqe_m2m = 1;
	g_viqe_disp_modechange = false;
	
	return;
}

void TCC_SetInitVIQE_60Hz(int lcdCtrlNo, int scalerCh, int deinterlaceM2M, int DI_use, unsigned int FrmWidth, unsigned int FrmHeight, unsigned int FrmStride)
{
	unsigned int framebufWidth = FrmWidth;
	unsigned int framebufHeight = FrmHeight;
	unsigned int framebufStride = FrmStride;
	int compbufHeight;
	int complumaHeight;
	int compOneFrameSize;

	if(g_viqe_first == 0)
		TCC_DeInitializeVIQE(lcdCtrlNo, scalerCh, 1);
	
	pmap_get_info("viqe", &pmap_viqe);

	gPMEM_VIQE_BASE = PMEM_VIQE_BASE;		// This buffer should be allocated for De-noise filter processing by VIQE.
	gPMEM_VIQE_SIZE = PMEM_VIQE_SIZE;

	tcc_viqe_ctrl(IOCTL_VIQE_OFF, 0);
	tcc_viqe_ctrl(IOCTL_VIQE_ON, 0);
	
	memset(&viqe_modes, 0, sizeof(viqe_modes_t));
	memset(&viqe_dma, 0, sizeof(viqe_dma_t));
	memset(&viqe_imgfmt, 0, sizeof(viqe_imgfmt_t));
	memset(&viqe_cfg, 0, sizeof(viqe_cfg_t));
	
	viqe.modes = &viqe_modes;
	viqe.dma = &viqe_dma;
	viqe.imgfmt = &viqe_imgfmt;
	viqe.cfg = &viqe_cfg;

	if(deinterlaceM2M == 0)
	{
		viqe.cfg->comp_en = OFF;
		viqe.cfg->frm_manual_restart = OFF;
		viqe.cfg->directpath_en = ON;

		if(lcdCtrlNo)
		{
			if(scalerCh == 0)
				viqe.cfg->directpath_op = VIQE_MSCL0 + MSCL0_LCDC1 + MSCL1_NOTUSED;		// M2M Scaler ch 0
			else
				viqe.cfg->directpath_op = VIQE_MSCL1 + MSCL1_LCDC1 + MSCL0_NOTUSED;		// M2M Scaler ch 1
		}
		else
		{
			if(scalerCh == 0)
				viqe.cfg->directpath_op = VIQE_MSCL0 + MSCL0_LCDC0 + MSCL1_NOTUSED;		// M2M Scaler ch 1	viqe.cfg->overlap_disable = ON;
			else
				viqe.cfg->directpath_op = VIQE_MSCL1 + MSCL1_LCDC0 + MSCL0_NOTUSED;		// M2M Scaler ch 1	viqe.cfg->overlap_disable = ON;
		}

		viqe.cfg->overlap_disable = ON;

		viqe.width = framebufWidth;
		viqe.height = framebufHeight;

		viqe.imgfmt->i_fmt = FMT_YUV420IL0;		//FMT_YUV420IL0

		if ( viqe.cfg->directpath_en )
			viqe.imgfmt->o_fmt = OD_YUV422SEQ0;
		else
			viqe.imgfmt->o_fmt = OD_YUV420SEP;
		if(DI_use == 0)
			viqe.imgfmt->o_fmt = OD_YUV420INT;

		viqe.imgfmt->fmt_conv_disable = viqe.imgfmt->force_420 = OFF;
		viqe.imgfmt->first_frm_init = ON;

		viqe.dma->conti = ON;
		viqe.dma->opt = ON;
		viqe.dma->comp_pingpong = ON;

		viqe.dma->i_offs[0] = framebufStride;
		if ( viqe.imgfmt->i_fmt == FMT_YUV420SEP )
			viqe.dma->i_offs[1] = framebufStride>>1;
		else if ( (viqe.imgfmt->i_fmt == FMT_YUV420IL0) || (viqe.imgfmt->i_fmt == FMT_YUV420IL1) )
			viqe.dma->i_offs[1] = framebufStride;

		viqe.dma->o_offs[0] = framebufWidth;
		if( (viqe.imgfmt->o_fmt == OD_YUV422SEQ0) || (viqe.imgfmt->o_fmt == OD_YUV422INT) || (viqe.imgfmt->o_fmt == OD_YUV420INT) )
			viqe.dma->o_offs[1] = framebufWidth;
		else
			viqe.dma->o_offs[1] = framebufWidth>>1;

		complumaHeight = framebufHeight;
		if (viqe.imgfmt->force_420 == ON)
			compbufHeight = 2*framebufHeight;
		else
			compbufHeight = framebufHeight;

		if ( viqe.dma->comp_pingpong == ON )
		{
			viqe.dma->addr_comp0[0] = gPMEM_VIQE_BASE;
			viqe.dma->addr_comp0[1] = viqe.dma->addr_comp0[0] + ( framebufStride*complumaHeight) / 2;
			viqe.dma->addr_comp0[2] = viqe.dma->addr_comp0[1] + ( (framebufStride*(compbufHeight>>2)) ) /2;
			compOneFrameSize = framebufStride*((compbufHeight >> 1) + complumaHeight);
			viqe.dma->addr_comp1[0] = viqe.dma->addr_comp0[0] + (compOneFrameSize) / 2;
			viqe.dma->addr_comp1[1] = viqe.dma->addr_comp0[1] + (compOneFrameSize) / 2;
			viqe.dma->addr_comp1[2] = viqe.dma->addr_comp0[2] + (compOneFrameSize) / 2;
		}
		else
		{
			viqe.dma->addr_comp0[0] = gPMEM_VIQE_BASE;
			viqe.dma->addr_comp0[1] = viqe.dma->addr_comp0[0] + (( framebufStride*complumaHeight )/2);
			viqe.dma->addr_comp0[2] = viqe.dma->addr_comp0[1] + (( (framebufStride*(compbufHeight>>2)) )/2);
		}

		tcc_viqe_ctrl(IOCTL_VIQE_SET_INIT, &viqe);
	}
	else
	{
		viqe.cfg->directpath_en = ON;

		if(lcdCtrlNo)
		{
			if(scalerCh == 0)
				viqe.cfg->directpath_op = VIQE_MSCL0 + MSCL0_LCDC1 + MSCL1_NOTUSED;		// M2M Scaler ch 0
			else
				viqe.cfg->directpath_op = VIQE_MSCL1 + MSCL1_LCDC1 + MSCL0_NOTUSED;		// M2M Scaler ch 1
		}
		else
		{
			if(scalerCh == 0)
				viqe.cfg->directpath_op = VIQE_MSCL0 + MSCL0_LCDC0 + MSCL1_NOTUSED;		// M2M Scaler ch 1	viqe.cfg->overlap_disable = ON;
			else
				viqe.cfg->directpath_op = VIQE_MSCL1 + MSCL1_LCDC0 + MSCL0_NOTUSED;		// M2M Scaler ch 1	viqe.cfg->overlap_disable = ON;
		}


		if ( viqe.cfg->directpath_en )
			viqe.imgfmt->o_fmt = OD_YUV422SEQ0;
		else
			viqe.imgfmt->o_fmt = OD_YUV420SEP;


		viqe.dma->o_offs[0] = framebufWidth;
		if( (viqe.imgfmt->o_fmt == OD_YUV422SEQ0) || (viqe.imgfmt->o_fmt == OD_YUV422INT) || (viqe.imgfmt->o_fmt == OD_YUV420INT) )
			viqe.dma->o_offs[1] = framebufWidth;
		else
			viqe.dma->o_offs[1] = framebufWidth>>1;


		tcc_viqe_ctrl(IOCTL_VIQE_SET_FRMINFO, &viqe);

	}
	
	g_viqe_first = 1;
	g_viqe_m2m = 0;
	g_viqe_disp_modechange = false;
	return;
}

void TCC_DeInitializeVIQE(int lcdCtrlNo, int scalerCh, int DI_use)
{
	viqe_state_t viqe_state = VIQE_STATE_IDLE;
	if (!g_viqe_first)
	{
		printk("[%s] , lcdc(%d), scaler(%d), mode(%d)\n", __func__, lcdCtrlNo, scalerCh, g_viqe_m2m);

		if(g_viqe_m2m)	// g_viqe_m2m == 1, it means viqe will be store to memory and it will be run with scaler on OutputInterface_Overlay function.
		{
			tcc_lcdc_ChOnOff(lcdCtrlNo, 0, 0);		// lcd controller , ch0, disable
//			mdelay(16);// delay for lcdc off before scaler reset
			tcc_scaler_SWReset(scalerCh);
			//mdelay(50);
		#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
			tcc_viqe_ctrl(IOCTL_VIQE_SWRESET, 0);
		#else
			if(DI_use)
				tcc_viqe_ctrl(IOCTL_VIQE_FINALIZE, 0);
			tcc_viqe_ctrl(IOCTL_VIQE_SWRESET, 0);
		#endif

			ODMAFlush ();
			ODMARST();

			tcc_viqe_ctrl(IOCTL_VIQE_OFF, 0);
		}
		else	// g_viqe_m2m == 0, it means viqe will be run on fb driver in kernel.
		{
			tcc_lcdc_ChOnOff(lcdCtrlNo, 0, 0);		// lcd controller , ch0, disable
			tcc_lcdc_SetPathConfig(lcdCtrlNo, 0, 0);
//			mdelay(16);// delay for lcdc off before scaler reset
			tcc_scaler_SWReset(scalerCh);
			//mdelay(30);
			tcc_scaler_close(scalerCh);

			//tcc_viqe_ctrl(IOCTL_VIQE_FINALIZE, 0);
			tcc_viqe_ctrl(IOCTL_VIQE_SWRESET, 0);

			ODMAFlush ();
			ODMARST();

			tcc_viqe_ctrl(IOCTL_VIQE_OFF, 0);
			tcc_DDICacheSetting(RESTORE_NORMAL);
		}

    #ifdef CONFIG_ARCH_TCC88XX
		tcc_BusPrioritySetting(RESTORE_NORMAL);
    #endif

		tcc_viqe_ctrl(IOCTL_VIQE_SET_STATE, &viqe_state);

		#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
			if(TCC_OUTPUT_EXCLUSIVE_UI_GetEnable())
				TCC_OUTPUT_EXCLUSIVE_UI_SetInterlace(0);
		#endif
	}
	
	g_viqe_first = 1;
	g_viqe_m2m = 0;
	g_viqe_disp_modechange = false;
	
}
EXPORT_SYMBOL(TCC_DeInitializeVIQE);

int TCC_ConfigVIQEParamSet(int mode, int region, int strength1, int strength2, int modeparam)
{
	if(mode > 0 && mode < 10)
	{
		g_viqe_mode = mode;
	}
	else
	{
		printk("mode value spec over.\n");
		return -1;
	}

	if(region > 2)
	{
		printk("region value should be 0 or 1\n");
		return -1;
	}
	else
		g_viqe_region = region;

	if(strength1 > 0 && strength1 < 6)
		g_viqe_strength1 = strength1;
	else
	{
		printk("strength value should be less than 6\n");
		return -1;
	}

	if(strength2 > 0 && strength2 < 6)
	g_viqe_strength2 = strength2;
	else
	{
		printk("strength value should be less than 6\n");
		return -1;
	}

	g_viqe_modeparam = modeparam;
}

int TCC_ConfigVIQE(int mode, int region, int strength1, int strength2, int modeparam)
{
	int eMode, eStrength1, eStrength2, bHalfRegion;

	printk("[%s] %d %d %d %d %d\n", __func__, mode, region, strength1, strength2, modeparam);

	eMode = mode;

	if(eMode > 0 && eMode < 10)
	{
		printk("TCC_ConfigVIQEMain - Setmode_eMode : %d\n", eMode);
		if(TCC_SetModeVIQE(1 << eMode) != 1)
			return -1;
	}

	if(eMode > 0 && eMode < 10)
	{
		printk("TCC_ConfigVIQEMain - Region/STREN_eMode : %d\n", eMode);
		//////// set processing region
		bHalfRegion = region;
		if(bHalfRegion == 1)
		{
			printk("TCC_ConfigVIQEMain - Region_bHalfRegion : %d\n", bHalfRegion);
			TCC_VIQE_SetProcessingRegion((VIQEMODE)(1 << eMode));
		}

		/////// set strength
		eStrength1 = strength1;
		if(eStrength1 > 0 && eStrength1 < 6)
		{
			if(eMode == 3 || eMode == 5 || eMode == 8)
			{
				eStrength2 = strength2;
				if(eStrength2 > 0 && eStrength2 < 6)
				{
					printk("TCC_ConfigVIQEMain - STRENGTH_eStrength : %d, %d\n", eStrength1, eStrength2);
					VIQE_SetStrength((VIQEMODE)(1 << eMode), eStrength1, eStrength2);
				}

			}
			else
			{
				printk("TCC_ConfigVIQEMain - STRENGTH_eStrength : %d\n", eStrength1);
				VIQE_SetStrength((VIQEMODE)(1 << eMode), eStrength1, 0);
			}
		}
	}

	return 1;
}

int TCC_RunVIQE( unsigned int CurYBase, unsigned int CurUBase, unsigned int CurVBase,
		unsigned int PrevYBase, unsigned int PrevUBase, unsigned int PrevVBase, 
		unsigned int OutputMEMBase, unsigned int M2MMode, int InputFormat, int OutputFormat,
		int OddFirst, int DI_use)
{
	int ret = 1;
	unsigned int viqeModes = 0;

	if (M2MMode == 1)
		viqe.cfg->directpath_en = ON;
	else
		viqe.cfg->directpath_en = OFF;

	//input info
	viqe.imgfmt->i_fmt = InputFormat;
	if(viqe.imgfmt->i_fmt == FMT_YUV420SEP)
		viqe.dma->i_offs[1] = viqe.width>>1;
	else if((viqe.imgfmt->i_fmt == FMT_YUV420IL0) || (viqe.imgfmt->i_fmt == FMT_YUV420IL1))
		viqe.dma->i_offs[1] = viqe.width;
	else //422seq
		viqe.dma->i_offs[1] = viqe.width;

	//output info
	viqe.imgfmt->o_fmt = OutputFormat;
	if ( viqe.cfg->directpath_en )
		viqe.imgfmt->o_fmt = OD_YUV422SEQ0;

	if(DI_use == 0)
		viqe.imgfmt->o_fmt = OD_YUV420INT;

	viqe.dma->o_offs[0] = viqe.width;
	if(viqe.imgfmt->o_fmt == OD_YUV422SEQ0)
	{
		viqe.dma->o_offs[0] = viqe.width * 2;
		viqe.dma->o_offs[1] = viqe.width * 2;
	}
	else if( (viqe.imgfmt->o_fmt == OD_YUV422INT) || (viqe.imgfmt->o_fmt == OD_YUV420INT) )
		viqe.dma->o_offs[1] = viqe.width;
	else
		viqe.dma->o_offs[1] = viqe.width>>1;

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
	if(g_viqe_first == 1)
	{
		tcc_viqe_ctrl(IOCTL_VIQE_SET_FRMINFO, &viqe);
	}
#endif

	//set dma
	if(DI_use == 0)
	{
		viqe.dma->addr_cur[0] = viqe.dma->addr_pre[0] = CurYBase;
		viqe.dma->addr_cur[1] = viqe.dma->addr_pre[1] = CurUBase;
		viqe.dma->addr_cur[2] = viqe.dma->addr_pre[2] = CurVBase;
	}
	else
	{
		viqe.dma->addr_cur[0] = CurYBase;
		viqe.dma->addr_cur[1] = CurUBase;
		viqe.dma->addr_cur[2] = CurVBase;

		viqe.dma->addr_pre[0] = PrevYBase;
		viqe.dma->addr_pre[1] = PrevUBase;
		viqe.dma->addr_pre[2] = PrevVBase;
	}

	if ( !viqe.cfg->directpath_en )
	{
		viqe.dma->addr_out[0] = (unsigned int)OutputMEMBase;
		viqe.dma->addr_out[1] = (unsigned int)OutputMEMBase + (viqe.width*viqe.height);
		viqe.dma->addr_out[2] = (unsigned int)OutputMEMBase + (viqe.width*viqe.height) + (viqe.width*viqe.height>>2);
	}

	tcc_viqe_ctrl(IOCTL_VIQE_SET_DMA, viqe.dma);

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
	//set mode
	if ( g_viqe_first )
	{
		int viqe_state = VIQE_STATE_RUN;

		viqeModes |= DI_use ? VIQE_MODE_DI : 0;
		viqeModes |= OddFirst ? VIQE_MODE_ODD_FIRST : 0;
		if(TCC_SetModeVIQE(viqeModes) != 1)
			ret = -1;

		TCC_ConfigVIQE(g_viqe_mode, g_viqe_region, g_viqe_strength1, g_viqe_strength2, g_viqe_modeparam);

		tcc_viqe_ctrl(IOCTL_VIQE_SET_STATE, &viqe_state);
		printk("[%s] process first frm\n", __func__);
	}
	else
	{
		tcc_viqe_ctrl(IOCTL_VIQE_RESTART, 0);
	}
#else
	//set deinterlace mode
	viqeModes |= OddFirst ? VIQE_MODE_ODD_FIRST : 0;
	if((g_viqe_disp_modechange) && (viqe.dma->addr_cur[0] != viqe.dma->addr_pre[0]))		
	{
		g_viqe_mode = 1;
		g_viqe_disp_modechange=false;
		viqeModes = VIQE_MODE_DI;

		if(TCC_SetModeVIQE(viqeModes) != 1)
			ret = -1;
		//printk("[VIQE] De-interlaced DI ON\n");	
	}

	if((DI_use==1 && g_viqe_mode==8) ||(!g_viqe_disp_modechange) && (viqe.dma->addr_cur[0] == viqe.dma->addr_pre[0]))
	{
		g_viqe_mode = 2;
		g_viqe_disp_modechange=true;
		viqeModes = VIQE_MODE_DI | VIQE_MODE_DISP;

		if(TCC_SetModeVIQE(viqeModes) != 1)
			ret = -1;
		//printk("[VIQE] De-interlaced DISP ON\n");	
	}
		
	//set gamut mode for bypass
	if((DI_use== 0) && (g_viqe_mode!=8))
	{
		g_viqe_mode = 8;
		viqeModes = VIQE_MODE_GAMUT_A1;

		if(TCC_SetModeVIQE(viqeModes) != 1)
			ret = -1;
		//printk("[VIQE] De-interlaced OFF\n");	
	}

	//set status & run
	if ( g_viqe_first )
	{
		int viqe_state = VIQE_STATE_RUN;

		tcc_viqe_ctrl(IOCTL_VIQE_SET_STATE, &viqe_state);
		printk("[%s] process first frm\n", __func__);
	}
	else
	{
		tcc_viqe_ctrl(IOCTL_VIQE_RESTART, 0);
	}
#endif

	if (M2MMode == 0)
	{
		tcc_viqe_ctrl(IOCTL_VIQE_FINALIZE, 0);
	}

	return ret;
}

// If call this ExcuteVIQE function. You must be set to LCD size and position setting.
void TCC_ExcuteVIQE(int scalerCh, unsigned int *address,			// address[0:3] : previous, address[4:6] : current
						int srcWidth, int srcHeight,			// srcWidth, srcHeight
						int crop_top, int crop_bottom, int crop_left, int crop_right,	// offset top, offset bottom, offset left, offset right
						int lcdCtrlNo, unsigned int dstAddr,
						int deinterlaceM2M, int OddFirst, int DI_use)
{
	unsigned int pBase0, pBase1, pBase2;
	unsigned int pBase3, pBase4, pBase5;
	// This description's are represent to YUV420 Interleaved format.
	// If screen mode will be used, cropping information have to apply on viqe source image.
	crop_top = (crop_top >>1)<<1;
	pBase0 = address[0] + (crop_top * srcWidth + crop_left);
	pBase1 = address[1] + (crop_top / 2 * srcWidth + crop_left);
	pBase2 = address[2] + (crop_top / 2 * srcWidth + crop_left);

	pBase3 = address[3] + (crop_top * srcWidth + crop_left);
	pBase4 = address[4] + (crop_top / 2 * srcWidth + crop_left);
	pBase5 = address[5] + (crop_top / 2 * srcWidth + crop_left);

	if(g_viqe_first)
	{
		//tcc_DDICacheSetting(SETTING_FOR_VIQE);
		//TCC_InitalizeVIQE(lcdCtrlNo, scalerCh, deinterlaceM2M, srcWidth, srcHeight);

		printk("[VIQE] For Deinterlacing %s block will be used.\n", "VIQE-Scaler-Memory");
		printk("[VIQE] Src Size %d x %d, DI_use %d, OddFirst %d, M2MMode %d\n", srcWidth, srcHeight, DI_use, OddFirst, deinterlaceM2M);
		printk("[VIQE] Crop info top %d, bottom %d, left %d, right %d\n", crop_top, crop_bottom, crop_left, crop_right);

		
		TCC_RunVIQE(pBase0, pBase1, pBase2, pBase3, pBase4, pBase5,
					dstAddr, deinterlaceM2M, FMT_YUV420IL0, OD_YUV422SEQ0, OddFirst, DI_use);
		g_viqe_first = 0 ;

	}
	else
	{
		TCC_RunVIQE(pBase0, pBase1, pBase2, pBase3, pBase4, pBase5,
					dstAddr, deinterlaceM2M, FMT_YUV420IL0, OD_YUV422SEQ0, OddFirst, DI_use);
	}
}



/****************************************************************************************
* VIQE 60Hz mode - video image write to LCDC directly
* Condition : DecodedSize <= (TargetSize * 8)
* And 
   if (decodedSize > 2000000)		rate = 0.64;
   else if(decodedSize > 1500000)	rate = 0.44;
   else							rate = 0.25;
* deinterlaceM2M = (decodedSize *rate <= displaySize)? 0 : 1
* if deinterlaceM2M = 0, then VIQE-Scaler-LCD on-the-fly will be used for de-interlace.
* if deinterlaceM2M = 1, then Scaler-LCD on-the-fly will be used for de-interlace.
* when the viqe block start by on-the-fly to lcdc. init/deinit sequence must be follow likes below.
 1. lcdc off
 2. scaler disable
 3. delay about 1 frame output time.
 4. lcdc, scaler, viqe, data path setting.
 5. scaler start.
 6. lcdc channel on.
*****************************************************************************************/
void TCC_InitalizeVIQE_60Hz(int scalerCh, unsigned int *address,			// address[0:3] : previous, address[4:6] : current
						int srcWidth, int srcHeight,			// srcWidth, srcHeight
						int crop_top, int crop_bottom, int crop_left, int crop_right,	// offset top, offset bottom, offset left, offset right
						int dstWidth, int dstHeight, 			// dstWidth, dstHeight
						int offset_x, int offset_y,
						int lcdCtrlNo, int deinterlaceM2M, int renderCnt,
						int OddFirst, int DI_use)
{
	unsigned int pBase0, pBase1, pBase2;
	unsigned int pBase3, pBase4, pBase5;
	int Width, Height;
	// This description's are represent to YUV420 Interleaved format.
	// If screen mode will be used, cropping information have to apply on viqe source image.
	crop_top = (crop_top >>1)<<1;
	pBase0 = address[0] + (crop_top * srcWidth + crop_left);
	pBase1 = address[1] + (crop_top / 2 * srcWidth + crop_left);
	pBase2 = address[2] + (crop_top / 2 * srcWidth + crop_left);

	pBase3 = address[3] + (crop_top * srcWidth + crop_left);
	pBase4 = address[4] + (crop_top / 2 * srcWidth + crop_left);
	pBase5 = address[5] + (crop_top / 2 * srcWidth + crop_left);

	Width = ((srcWidth - crop_left - crop_right) >> 3) << 3;			// 8bit align
	Height = ((srcHeight - crop_top - crop_bottom) >> 1) << 1;		// 2bit align

	tcc_lcdc_ChOnOff(lcdCtrlNo, 0, 0);		// lcd controller , ch0, disable
//	mdelay(16);
	tcc_scaler_Disable(scalerCh);
	//mdelay(50);

#if defined(CONFIG_ARCH_TCC88XX)
	tcc_BusPrioritySetting(SETTING_FOR_VIQE);
#endif
	if(deinterlaceM2M == 0)
		tcc_DDICacheSetting(SETTING_FOR_VIQE);		// it needs to viqe_sc_lcd onthefly.
	
	tcc_scaler_open(scalerCh);
	TCC_SetInitVIQE_60Hz(lcdCtrlNo, scalerCh, deinterlaceM2M, DI_use, Width, Height, srcWidth);
	tcc_lcdc_SetConfig(lcdCtrlNo, 0, dstWidth, dstHeight, offset_x, offset_y, TCC_LCDC_IMG_FMT_YUV422SQ);
	tcc_lcdc_SetPathConfig(lcdCtrlNo, 0, 1);	// Ch0 set to use for direct path.

	printk("[VIQE60Hz] For Deinterlacing %s block will be used.\n", (deinterlaceM2M==1)? "Scaler-LCD" : "VIQE-Scaler-LCD");		//(deinterlaceM2M==0 && DI_use==0)|| 
	printk("[VIQE60Hz] Src Size %d x %d, Dst Size %d x %d, offset %d %d, DI_use %d\n", srcWidth, srcHeight, dstWidth, dstHeight, offset_x, offset_y, DI_use);
	printk("[VIQE60Hz] Crop info top %d, bottom %d, left %d, right %d\n", crop_top, crop_bottom, crop_left, crop_right);

	if(deinterlaceM2M == 0)		// deinterlace method : VIQE-Scaler-LCD.
	{
		tcc_scaler_Control60HzWithVIQE(	// Scaler Setting
			scalerCh,
			address[0], address[1], address[2],
			srcWidth, srcHeight, dstWidth, dstHeight,
			crop_top, crop_bottom, crop_left, crop_right
		);
		TCC_RunVIQE60 ( // VIQE RDMA Setting
			pBase0, pBase1, pBase2,
			pBase3, pBase4, pBase5,
			OddFirst, DI_use
		);
	}
	else						//  deinterlace method : Scaler-LCD.
	{
		tcc_scaler_Control60HzScalerOnly(	// Scaler Setting
			scalerCh, DI_use,
			address[0], address[1], address[2], 
			srcWidth, srcHeight, dstWidth, dstHeight, 
			crop_top, crop_bottom, crop_left, crop_right );
	}

	//set mode
	int viqe_state = VIQE_STATE_RUN;
	tcc_viqe_ctrl(IOCTL_VIQE_SET_STATE, &viqe_state);
	printk("[%s] process first frm\n", __func__);

	tcc_scaler_Start(scalerCh); // Scaler Start
	tcc_lcdc_ChOnOff(lcdCtrlNo, 0, 1); // LCD controller , ch0, enable
	g_viqe_first = 0 ;

}


int TCC_RunVIQE60( unsigned int CurYBase, unsigned int CurUBase, unsigned int CurVBase,
		unsigned int PrevYBase, unsigned int PrevUBase, unsigned int PrevVBase,
		int OddFirst, int DI_use)
{
	int ret = 1;
	unsigned int viqeModes = 0;

	//set dma
	if(DI_use == 0)
	{
		viqe.dma->addr_cur[0] = viqe.dma->addr_pre[0] = CurYBase;
		viqe.dma->addr_cur[1] = viqe.dma->addr_pre[1] = CurUBase;
		viqe.dma->addr_cur[2] = viqe.dma->addr_pre[2] = CurVBase;
	}
	else
	{
		viqe.dma->addr_cur[0] = CurYBase;
		viqe.dma->addr_cur[1] = CurUBase;
		viqe.dma->addr_cur[2] = CurVBase;

		viqe.dma->addr_pre[0] = PrevYBase;
		viqe.dma->addr_pre[1] = PrevUBase;
		viqe.dma->addr_pre[2] = PrevVBase;
	}
	tcc_viqe_ctrl(IOCTL_VIQE_SET_DMA, viqe.dma);

	//set gamut mode for bypass
	if(DI_use== 0)
	{
		viqeModes = VIQE_MODE_GAMUT_A1;
		viqeModes |= OddFirst ? VIQE_MODE_ODD_FIRST : 0;

		if(TCC_SetModeVIQE(viqeModes) != 1)
			ret = -1;
		//printk("[VIQE] De-interlaced OFF\n");	
	}
	else
	{
		//set deinterlace mode
		if(viqe.dma->addr_cur[0] != viqe.dma->addr_pre[0])	
		{
			viqeModes = VIQE_MODE_DI;
			viqeModes |= OddFirst ? VIQE_MODE_ODD_FIRST : 0;

			if(TCC_SetModeVIQE(viqeModes) != 1)
				ret = -1;
			//printk("[VIQE] De-interlaced DI ON\n");	
		}

		else
		{
			viqeModes = VIQE_MODE_DI | VIQE_MODE_DISP;
			viqeModes |= OddFirst ? VIQE_MODE_ODD_FIRST : 0;
			
			if(TCC_SetModeVIQE(viqeModes) != 1)
				ret = -1;
			//printk("[VIQE] De-interlaced DISP ON\n");	
		}
	}

	return ret;
}

// If call this ExcuteVIQE function. You must be set to LCD size and position setting.
void TCC_ExcuteVIQE_60Hz(int scalerCh, unsigned int *address,			// address[0:3] : previous, address[4:6] : current
						int srcWidth, int srcHeight,			// srcWidth, srcHeight
						int crop_top, int crop_bottom, int crop_left, int crop_right,	// offset top, offset bottom, offset left, offset right
						int dstWidth, int dstHeight, 			// dstWidth, dstHeight
						int offset_x, int offset_y,
						int lcdCtrlNo, int deinterlaceM2M, int renderCnt,
						int OddFirst, int DI_use)
{
	unsigned int pBase0, pBase1, pBase2;
	unsigned int pBase3, pBase4, pBase5;
	int Width, Height;
	// This description's are represent to YUV420 Interleaved format.
	// If screen mode will be used, cropping information have to apply on viqe source image.
	crop_top = (crop_top >>1)<<1;
	pBase0 = address[0] + (crop_top * srcWidth + crop_left);
	pBase1 = address[1] + (crop_top / 2 * srcWidth + crop_left);
	pBase2 = address[2] + (crop_top / 2 * srcWidth + crop_left);

	pBase3 = address[3] + (crop_top * srcWidth + crop_left);
	pBase4 = address[4] + (crop_top / 2 * srcWidth + crop_left);
	pBase5 = address[5] + (crop_top / 2 * srcWidth + crop_left);

	Width = ((srcWidth - crop_left - crop_right) >> 3) << 3;			// 8bit align
	Height = ((srcHeight - crop_top - crop_bottom) >> 1) << 1;		// 2bit align

	if (deinterlaceM2M == 0)
	{
		TCC_RunVIQE60 ( // VIQE RDMA Setting
			pBase0, pBase1, pBase2,
			pBase3, pBase4, pBase5,
			OddFirst, DI_use
		);
	}
	else
	{
		if(DI_use == 0)
		{
			tcc_scaler_ChangeBaseAddress (scalerCh, pBase0, pBase1, pBase2);
		}
		else
		{
			if (OddFirst)
			{
				if (renderCnt == 1)
				{
					tcc_scaler_ChangeBaseAddress (scalerCh, pBase0, pBase1, pBase2);
				}
				else if (renderCnt == 0)
				{
					tcc_scaler_ChangeBaseAddress (scalerCh, pBase0 + srcWidth, pBase1 + srcWidth, pBase2 + srcWidth);
				}
			}
			else
			{
				if (renderCnt == 0)
				{
					tcc_scaler_ChangeBaseAddress (scalerCh, pBase0, pBase1, pBase2);
				}
				else if (renderCnt == 1)
				{
					tcc_scaler_ChangeBaseAddress (scalerCh, pBase0 + srcWidth, pBase1 + srcWidth, pBase2 + srcWidth);
				}
			}
		}
	}
}


