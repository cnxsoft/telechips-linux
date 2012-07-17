

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
#include <mach/tcc_fb.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>
#include <mach/tca_lcdc.h>
#include <mach/tccfb_ioctrl.h>

//#define LCDC_AXI_BUS

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tca lcdc: " msg); }
	
#if defined(CONFIG_FB_TCC_COMPONENT)
extern int tcc_component_lcdc_handler_check(char lcdc_num);
#endif

void tcc_ddi_cache_setting(void)
{
#if defined(CONFIG_ARCH_TCC92XX)
	HwDDI_CACHE->DDIC_CFG0 = HwDDIC_CFG27(DDIC_LCD0_DMA0_0) | HwDDIC_CFG26(DDIC_LCD1_DMA0_0);
	HwDDI_CACHE->DDIC_CFG1 = HwDDIC_CFG31(DDIC_LCD1_DMA0_1) | HwDDIC_CFG30(DDIC_LCD1_DMA2)
							| HwDDIC_CFG29(DDIC_MSC0_DMA_0) | HwDDIC_CFG28(DDIC_MSC1_DMA_0);
		
	HwDDI_CACHE->DDIC_CTRL = (1<<DDIC_LCD0_DMA0_0)|(1<<DDIC_LCD1_DMA0_0)
							|(1<<DDIC_LCD1_DMA0_1)|(1<<DDIC_LCD1_DMA2)
							|(1<<DDIC_MSC0_DMA_0) |(1<<DDIC_MSC1_DMA_0)
								|(1<< DDIC_BW);
	dprintk("DDIC_CFG0 : 0x%08X, DDIC_CFG1 : 0x%08X DDIC_CTRL : %08X\n", HwDDI_CACHE->DDIC_CFG0, HwDDI_CACHE->DDIC_CFG1, HwDDI_CACHE->DDIC_CTRL);

#elif defined(CONFIG_ARCH_TCC93XX)
	PDDICACHE pHwDDICACHE  = (volatile PDDICACHE)tcc_p2v(HwDDICACHE_BASE);

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
	/* Cacheabe */
	pHwDDICACHE->DDIC0_CFG0 = (DDIC_LCD0_DMA_0_0) |(DDIC_LCD0_DMA_0_1 << 4) |(DDIC_LCD0_DMA_2<<8) |(DDIC_MSCL0_DMA_0<<12) |(DDIC_MSCL0_DMA_1<<16)	|(DDIC_LCD0_DMA_1<<20);
	/* Non-Cacheable */
	pHwDDICACHE->DDIC0_CFG1 = (DDIC_LCD0_DMA_0_2) |(DDIC_MSCL0_DMA_2 << 4) |(DDIC_VIQE_DMA_0_0<<8) |(DDIC_VIQE_DMA_0_1<<12) |(DDIC_VIQE_DMA_0_2<<16);

	/* Cacheable */
	pHwDDICACHE->DDIC1_CFG0 = (DDIC_LCD1_DMA_0_0) |(DDIC_LCD1_DMA_0_1 << 4) |(DDIC_LCD1_DMA_2<<8) |(DDIC_MSCL1_DMA_0<<12) |(DDIC_MSCL1_DMA_1<<16)  |(DDIC_LCD1_DMA_1<<20);
	/* Non-Cacheable */
	pHwDDICACHE->DDIC1_CFG1 = (DDIC_LCD1_DMA_0_2) |(DDIC_MSCL1_DMA_2 << 4) |(DDIC_VIQE_DMA_1_0<<8) |(DDIC_VIQE_DMA_1_1<<12) |(DDIC_VIQE_DMA_1_2<<16)  |(DDIC_VIQE_DMA_2_0<<20)
							|(DDIC_VIQE_DMA_2_1<<24) |(DDIC_VIQE_DMA_2_2<<28);
	#else
	/* Cacheabe */
	pHwDDICACHE->DDIC0_CFG0 = (DDIC_LCD0_DMA_0_0) |(DDIC_LCD0_DMA_0_1 << 4) |(DDIC_LCD0_DMA_2<<8) |(DDIC_MSCL0_DMA_0<<12) |(DDIC_MSCL0_DMA_1<<16)  |(DDIC_MSCL0_DMA_2<<20);
	/* Non-Cacheable */
	pHwDDICACHE->DDIC0_CFG1 = (DDIC_LCD0_DMA_0_2) |(DDIC_LCD0_DMA_1 << 4) |(DDIC_VIQE_DMA_0_0<<8) |(DDIC_VIQE_DMA_0_1<<12) |(DDIC_VIQE_DMA_0_2<<16);
	
	/* Cacheable */
	pHwDDICACHE->DDIC1_CFG0 = (DDIC_LCD1_DMA_0_0) |(DDIC_LCD1_DMA_0_1 << 4) |(DDIC_LCD1_DMA_2<<8) |(DDIC_MSCL1_DMA_0<<12) |(DDIC_MSCL1_DMA_1<<16)  |(DDIC_MSCL1_DMA_2<<20);
	/* Non-Cacheable */
	pHwDDICACHE->DDIC1_CFG1 = (DDIC_LCD1_DMA_0_2) |(DDIC_LCD1_DMA_1 << 4) |(DDIC_VIQE_DMA_1_0<<8) |(DDIC_VIQE_DMA_1_1<<12) |(DDIC_VIQE_DMA_1_2<<16)  |(DDIC_VIQE_DMA_2_0<<20)
								|(DDIC_VIQE_DMA_2_1<<24) |(DDIC_VIQE_DMA_2_2<<28);
	#endif
	
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

#elif defined(CONFIG_ARCH_TCC88XX)
	PSDDICACHE pCache =  (PSDDICACHE)tcc_p2v(HwDDICACHE_BASE);
	PDDICACHE pCsh = (PDDICACHE)pCache;
	SDDICACHE Cache;
	
//	memset( &Cache, 0, sizeof(Cache));

	#ifdef LCDC_AXI_BUS
    Cache.uDDICCFG00.B.sel00 = DDIC_MSCL0_DMA_0;
    Cache.uDDICCFG00.B.sel01 = DDIC_MSCL0_DMA_1;
    Cache.uDDICCFG00.B.sel02 = DDIC_MSCL0_DMA_2;
    Cache.uDDICCFG00.B.sel03 = DDIC_LCD0_DMA_0_0;
    Cache.uDDICCFG00.B.sel04 = DDIC_LCD0_DMA_0_1;
    Cache.uDDICCFG00.B.sel05 = DDIC_LCD0_DMA_2;
    
    Cache.uDDICCFG01.B.sel06 = DDIC_VIQE_DMA_0_0;
    Cache.uDDICCFG01.B.sel07 = DDIC_VIQE_DMA_0_1;
    Cache.uDDICCFG01.B.sel08 = DDIC_LCD0_DMA_0_2;
    Cache.uDDICCFG01.B.sel09 = DDIC_LCD0_DMA_1;
    Cache.uDDICCFG01.B.sel10 = DDIC_VIQE_DMA_0_2;
    Cache.uDDICCFG01.B.sel11 = DDIC_LCD0_DMA_3;
    Cache.uDDICCFG01.B.sel12 = DDIC_LCD1_DMA_3;
    Cache.uDDICCFG01.B.sel13 = DDIC_Reserved;

	Cache.uDDICCFG10.B.sel00 = DDIC_MSCL1_DMA_0;
	Cache.uDDICCFG10.B.sel01 = DDIC_MSCL1_DMA_1;
	Cache.uDDICCFG10.B.sel02 = DDIC_MSCL1_DMA_2;
	Cache.uDDICCFG10.B.sel03 = DDIC_VIQE_DMA_1_0;
	Cache.uDDICCFG10.B.sel04 = DDIC_VIQE_DMA_1_1;
	Cache.uDDICCFG10.B.sel05 = DDIC_VIQE_DMA_1_2;
	
	Cache.uDDICCFG11.B.sel06 = DDIC_LCD1_DMA_0_0;
	Cache.uDDICCFG11.B.sel07 = DDIC_LCD1_DMA_0_1;
	Cache.uDDICCFG11.B.sel08 = DDIC_LCD1_DMA_0_2;
	Cache.uDDICCFG11.B.sel09 = DDIC_LCD1_DMA_1;
	Cache.uDDICCFG11.B.sel10 = DDIC_LCD1_DMA_2;
	Cache.uDDICCFG11.B.sel11 = DDIC_VIQE_DMA_2_0;
	Cache.uDDICCFG11.B.sel12 = DDIC_VIQE_DMA_2_1;
	Cache.uDDICCFG11.B.sel13 = DDIC_VIQE_DMA_2_2;
	#else
	Cache.uDDICCFG00.B.sel00 = DDIC_MSCL0_DMA_0;
	Cache.uDDICCFG00.B.sel01 = DDIC_MSCL0_DMA_1;
	Cache.uDDICCFG00.B.sel02 = DDIC_LCD0_DMA_1;
	Cache.uDDICCFG00.B.sel03 = DDIC_LCD0_DMA_0_0;
	Cache.uDDICCFG00.B.sel04 = DDIC_LCD0_DMA_0_1;
	Cache.uDDICCFG00.B.sel05 = DDIC_LCD0_DMA_2;

	Cache.uDDICCFG01.B.sel06 = DDIC_VIQE_DMA_0_0;
	Cache.uDDICCFG01.B.sel07 = DDIC_VIQE_DMA_0_1;
	Cache.uDDICCFG01.B.sel08 = DDIC_LCD0_DMA_0_2;
	Cache.uDDICCFG01.B.sel09 = DDIC_MSCL0_DMA_2;
	Cache.uDDICCFG01.B.sel10 = DDIC_VIQE_DMA_0_2;
	Cache.uDDICCFG01.B.sel11 = DDIC_LCD0_DMA_3;
	Cache.uDDICCFG01.B.sel12 = DDIC_LCD1_DMA_3;
	Cache.uDDICCFG01.B.sel13 = DDIC_Reserved;

	Cache.uDDICCFG10.B.sel00 = DDIC_MSCL1_DMA_0;
	Cache.uDDICCFG10.B.sel01 = DDIC_MSCL1_DMA_1;
	Cache.uDDICCFG10.B.sel02 = DDIC_MSCL1_DMA_2;
	Cache.uDDICCFG10.B.sel03 = DDIC_LCD1_DMA_0_0;
	Cache.uDDICCFG10.B.sel04 = DDIC_LCD1_DMA_0_1;
	Cache.uDDICCFG10.B.sel05 = DDIC_LCD1_DMA_2;

	Cache.uDDICCFG11.B.sel06 = DDIC_VIQE_DMA_1_0;
	Cache.uDDICCFG11.B.sel07 = DDIC_VIQE_DMA_1_1;
	Cache.uDDICCFG11.B.sel08 = DDIC_LCD1_DMA_0_2;
	Cache.uDDICCFG11.B.sel09 = DDIC_LCD1_DMA_1;
	Cache.uDDICCFG11.B.sel10 = DDIC_VIQE_DMA_1_2;
	Cache.uDDICCFG11.B.sel11 = DDIC_VIQE_DMA_2_0;
	Cache.uDDICCFG11.B.sel12 = DDIC_VIQE_DMA_2_1;
	Cache.uDDICCFG11.B.sel13 = DDIC_VIQE_DMA_2_2;
	#endif//

	pCsh->DDIC0_CH0_AMAX = 0xFFFFFFFF;
	pCsh->DDIC0_CH1_AMAX = 0xFFFFFFFF;
	pCsh->DDIC0_CH2_AMAX = 0xFFFFFFFF;
	pCsh->DDIC0_CH3_AMAX = 0xFFFFFFFF;
	pCsh->DDIC0_CH4_AMAX = 0xFFFFFFFF;
	pCsh->DDIC0_CH5_AMAX = 0xFFFFFFFF;
	pCsh->DDIC1_CH0_AMAX = 0xFFFFFFFF;
	pCsh->DDIC1_CH1_AMAX = 0xFFFFFFFF;
	pCsh->DDIC1_CH2_AMAX = 0xFFFFFFFF;
	pCsh->DDIC1_CH3_AMAX = 0xFFFFFFFF;
	pCsh->DDIC1_CH4_AMAX = 0xFFFFFFFF;
	pCsh->DDIC1_CH5_AMAX = 0xFFFFFFFF;
	
	Cache.uDDICCTRL.ddicctrl = 0x003F003F; // All Cache Enable

	*pCache = Cache;
#endif//
	dprintk("%s  \n",__func__);

}
EXPORT_SYMBOL(tcc_ddi_cache_setting);

void tcc_onthefly_init(void)
{
#if defined(CONFIG_ARCH_TCC88XX)
	volatile PDDICONFIG pDDICONFIG;

	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	BITCSET(pDDICONFIG->ON_THE_FLY, Hw2|Hw3|Hw4|Hw5, Hw3|Hw4|Hw5 );
#endif//
}
EXPORT_SYMBOL(tcc_onthefly_init);


void tca_lcdc_interrupt_onoff(char onoff, char lcdc)
{
	unsigned int LCDC_INTbit;
	PPIC pHwPIC_reg = (volatile PPIC)tcc_p2v(HwPIC_BASE);
	PLCDC pLCDC_reg;

	#if defined(CONFIG_FB_TCC_COMPONENT)
		if((onoff == 0) && tcc_component_lcdc_handler_check(lcdc))
		{
			dprintk("%s can't disable lcdc interrupt(component_1080i)!!\n", __func__);
			return;
		}
	#endif

	if(lcdc)	{
		LCDC_INTbit = HwINT0_LCD1;
		pLCDC_reg = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	}
	else	{
		LCDC_INTbit = HwINT0_LCD0;
		pLCDC_reg = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	}
	
	#ifdef CONFIG_USE_EXT_INTERRUPT
	    if(onoff == 1)  {
	        BITSET(pHwPIC_reg->CLR0, HwINT0_EI6);
	        BITSET(pHwPIC_reg->INTMSK0, HwINT0_EI6);
	        BITSET(pHwPIC_reg->IEN0, HwINT0_EI6);
	    }
	    else    {
	        BITCLR(pHwPIC_reg->INTMSK0, HwINT0_EI6);
	        BITCLR(pHwPIC_reg->IEN0, HwINT0_EI6);
	        BITSET(pHwPIC_reg->CLR0, HwINT0_EI6);
	    }
	#else
		if(onoff == 1)	{
			// interrupt enable 
			BITSET(pHwPIC_reg->CLR0, LCDC_INTbit);			
			BITSET(pLCDC_reg->LSTATUS, lcdc ? HwLSTATUS_RU : HwLSTATUS_DEF);
			BITSET(pLCDC_reg->LIM, 0xFFFFFFFF);
			BITCLR(pLCDC_reg->LIM, lcdc ? HwLIM_RU : HwLIM_DEF);
			
			BITCLR(pHwPIC_reg->POL0, LCDC_INTbit);	// 0 : acitve -high
			BITSET(pHwPIC_reg->SEL0, LCDC_INTbit);	// 1 : IRQ interrupt
			BITSET(pHwPIC_reg->MODE0, LCDC_INTbit); //0 : edge-triggered mode  1: level

			BITSET(pHwPIC_reg->IEN0, LCDC_INTbit);
			BITSET(pHwPIC_reg->INTMSK0, LCDC_INTbit);	
		}
		else	{
			// interrupt disable
			BITSET(pLCDC_reg->LIM, 0xFFFFFFFF);
			BITSET(pLCDC_reg->LSTATUS, lcdc ? HwLSTATUS_RU : HwLSTATUS_DEF);
			BITSET(pHwPIC_reg->CLR0, LCDC_INTbit);
			BITCLR(pHwPIC_reg->IEN0, LCDC_INTbit);
			BITCLR(pHwPIC_reg->INTMSK0, LCDC_INTbit);	
		}
	#endif//CONFIG_USE_EXT_INTERRUPT
}
EXPORT_SYMBOL(tca_lcdc_interrupt_onoff);



void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num)
{
	unsigned int lcd_reg;

	PLCDC pLCDC;

#if 0	
	// Set the address for LCDC0
	pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
#endif//

	// Set the address for LCDC1
	pLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);


	lcd_reg = (pLCDC->LCTRL & HwLCTRL_GEN);

	if(lcd_spec->bus_width == 24)
		lcd_reg |= (0xC<<16);
	else if(lcd_spec->bus_width == 18)
		lcd_reg |= (0x5<<16);
	else
		lcd_reg |= (0x3<<16);

	if(lcd_spec->sync_invert & ID_INVERT)
		lcd_reg |= Hw15;

	if(lcd_spec->sync_invert & IV_INVERT)
		lcd_reg |= Hw14;

	if(lcd_spec->sync_invert & IH_INVERT)
		lcd_reg |= Hw13;

	if(lcd_spec->sync_invert & IP_INVERT)
		lcd_reg |= Hw12;

#ifdef CONFIG_LCD_CPU_INTERFACE
	lcd_reg |= Hw23; // clock gating enable 
#endif//

#if defined(LCDC_AXI_BUS) && defined(CONFIG_ARCH_TCC88XX)
	lcd_reg |= Hw27; // AXI bus
#endif//

	lcd_reg |=	Hw8 |			// Non-interlace
				(0x2<<5)|		// TFT-LCD mode: STN(Hw5), TFT(Hw6), TV(Hw7)
				//Hw4 | 		// Master Select for IMG0
				(0x5<<1)|		// OVP[3:1] - 5 : Image2 > Image1 > Image0
				//Hw0 | 		// LCD Controller Enable
				0;			// End Of Value

	pLCDC->LCTRL = lcd_reg;

// Set LCD clock
	pLCDC->LCLKDIV = ( (1 << 16) | lcd_spec->clk_div/2); //((PCK_LCD+1)/2)/((LCD_DEVIDE)*2)

// Background color
	pLCDC->LBC = 0x80000000;

//	Horizontal timing
	pLCDC->LHTIME1 = ((lcd_spec->lpw) << 16) | (lcd_spec->xres - 1);
	pLCDC->LHTIME2 = ((lcd_spec->lswc) << 16) | (lcd_spec->lewc);

//	Vertical timing
	pLCDC->LVTIME1 = ((lcd_spec->fpw1) << 16) | (lcd_spec->yres- 1);
	pLCDC->LVTIME2 = ((lcd_spec->fswc1) << 16) | (lcd_spec->fewc1);
	pLCDC->LVTIME3 = ((lcd_spec->fpw2) << 16) | (lcd_spec->yres - 1);
	pLCDC->LVTIME4 = ((lcd_spec->fswc2) << 16) | (lcd_spec->fewc2);

//	Display Size
	pLCDC->LDS = (lcd_spec->yres << 16) | lcd_spec->xres;

//	Interrupt mask
	pLCDC->LIM &= 0xFFFFFFE6;
	pLCDC->LIM |= ((1 << 4) | 1);
	
	pLCDC->LCTRL |= HwLCTRL_LEN;
}

EXPORT_SYMBOL(lcdc_initialize);

/*===========================================================================
FUNCTION
 - index : 0 = lcdc0, 1 = lcdc1
===========================================================================*/
unsigned int DEV_LCDC_Wait_signal(char lcdc)
{
	#define MAX_LCDC_WAIT_TIEM 		0x70000000
	unsigned int loop = 0;
	volatile PLCDC	uiPLCD;
	PPMU ppmu; 
	PCKC pckc;
	PDDICONFIG pddiconfig;

	ppmu = (PPMU)(tcc_p2v(HwPMU_BASE));
	pckc = (PCKC)(tcc_p2v(HwCKC_BASE)); 
	pddiconfig = (PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE));

	if((tca_ckc_getpmupwroff(PMU_DDIBUS))||(!(pckc->CLK1CTRL & 0x00200000)))
		return FALSE;

	if(lcdc == 0)	{

		uiPLCD = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

		if(ISSET(pddiconfig->PWDN, HwDDIC_PWDN_LCDC0))
			return FALSE;
	}
	else	{

		uiPLCD = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

		if(ISSET(pddiconfig->PWDN, HwDDIC_PWDN_LCDC1))
			return FALSE;
	}

	if(ISZERO(uiPLCD->LCTRL, HwLCTRL_LEN))
		return FALSE;

	while(TRUE && (loop < MAX_LCDC_WAIT_TIEM)) //check wheather is lcd on
	{
		if((ISSET(uiPLCD->LSTATUS, HwLSTATUS_DEOF))
/*			&& (ISSET(uiPLCD->LSTATUS, HwLSTATUS_DD)) */
		)
			break;
		loop++;
	}
	
 	return TRUE;
}
EXPORT_SYMBOL(DEV_LCDC_Wait_signal);


unsigned int DEV_LCDC_Status(char lcdc)
{
	volatile PLCDC	uiPLCD;
	PPMU ppmu; 
	PCKC pckc;
	PDDICONFIG pddiconfig;

	ppmu = (PPMU)(tcc_p2v(HwPMU_BASE));
	pckc = (PCKC)(tcc_p2v(HwCKC_BASE)); 
	pddiconfig = (PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE));

	if((tca_ckc_getpmupwroff(PMU_DDIBUS))||(!(pckc->CLK1CTRL & 0x00200000)))
		return FALSE;

	if(lcdc == 0)	{

		uiPLCD = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

		if(ISSET(pddiconfig->PWDN, HwDDIC_PWDN_LCDC0))
			return FALSE;
	}
	else	{

		uiPLCD = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

		if(ISSET(pddiconfig->PWDN, HwDDIC_PWDN_LCDC1))
			return FALSE;
	}

	if(ISZERO(uiPLCD->LCTRL, HwLCTRL_LEN))
		return FALSE;

	return uiPLCD->LSTATUS;
		
}
EXPORT_SYMBOL(DEV_LCDC_Status);

unsigned char tcc_LCDC_Wait_signal_disable(char lcdc)
{
	unsigned int loop = 0;
	volatile PLCDC	uiPLCD;
	PDDICONFIG pHwDDICACHE_CONFIG  = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if(lcdc == 0)	{
		uiPLCD = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

		if(ISSET(pHwDDICACHE_CONFIG->PWDN, HwDDIC_PWDN_LCDC0))
			return FALSE;
	}
	else	{
		uiPLCD = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

		if(ISSET(pHwDDICACHE_CONFIG->PWDN, HwDDIC_PWDN_LCDC1))
			return FALSE;
	}

	if( ISSET(uiPLCD->LCTRL, HwLCTRL_LEN) )
	{
		BITCLR(uiPLCD->LCTRL, HwLCTRL_LEN);

		while(TRUE && (loop < 500000)) //check wheather is lcd on
		{
			if((ISZERO(uiPLCD->LSTATUS, HwLSTATUS_BUSY)) && (ISSET(uiPLCD->LSTATUS, HwLSTATUS_DD)))
				break;

			loop++;
		}
	}

	return TRUE;
}



unsigned int tcc_LCDC_ctrl_layer(unsigned lcdc, unsigned layer, unsigned onoff)
{
	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_layer;

	if(lcdc == 0)
	{
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
		pstLCDC_layer = (volatile PLCDC_CHANNEL)tcc_p2v(pstLCDC->LI0C);
	}
	else
	{
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
		switch(layer)
		{
			case 0:
				pstLCDC_layer = (volatile PLCDC_CHANNEL)&pstLCDC->LI0C;
				break;
				
			case 1:
				pstLCDC_layer = (volatile PLCDC_CHANNEL)&pstLCDC->LI1C;
				break;
				
			case 2:
				pstLCDC_layer = (volatile PLCDC_CHANNEL)&pstLCDC->LI2C;
				break;
		}
	}

	if(onoff)
	{
		BITSET(pstLCDC_layer->LIC, (1<<28)); /* channel image enable */
	}
	else
	{
		BITCLR(pstLCDC_layer->LIC, (1<<28)); /* channel image disalbe */
	}

	#ifndef CONFIG_ARCH_TCC92XX
		BITSET(pstLCDC_layer->LIC, HwLCT_RU); /* channel image update enable */
	#endif //

	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_ctrl_layer);


unsigned int tcc_LCDC_set_alpha(unsigned lcdc, unsigned layer, unsigned alpha)
{

	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_CH;

	if(lcdc)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	switch(layer)
	{
		case 0:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI0C;
			break;
		case 1:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI1C;
			break;
		case 2:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI2C;
			break;
	}

	alpha &= 0xff;

	BITSET(pstLCDC_CH->LIC, Hw30);						/* AEN bit */
	BITCLR(pstLCDC_CH->LIC, Hw24);						/* ASEL bit */

	BITCSET(pstLCDC_CH->LIA, 0x000000ff, (alpha));		/* A0 bits */
	BITCSET(pstLCDC_CH->LIA, 0x00ff0000, (alpha)<<16);	/* A1 bits */
	
	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_set_alpha);

unsigned int tcc_LCDC_set_chromakey(lcdc_chromakey_params *lcdc_chromakey)
{

	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_CH;

	if(lcdc_chromakey->lcdc_num)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	switch(lcdc_chromakey->lcdc_num)
	{
		case 0:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI0C;
			break;
		case 1:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI1C;
			break;
		case 2:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI2C;
			break;
	}

	BITCSET (pstLCDC_CH->LIC, 0x1<< 29, (lcdc_chromakey->enable) << 29); // chroma-keying
	BITCSET (pstLCDC_CH->LIKR, 0xFFFFFFFF, lcdc_chromakey->chromaR); // key
	BITCSET (pstLCDC_CH->LIKG, 0xFFFFFFFF, lcdc_chromakey->chromaG); // key
	BITCSET (pstLCDC_CH->LIKB, 0xFFFFFFFF, lcdc_chromakey->chromaB); // key

#ifndef CONFIG_ARCH_TCC92XX
	BITCSET (pstLCDC_CH->LIC, 	HwLCT_RU, 	HwLCT_RU); //Image update
#endif//
	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_set_chromakey);

unsigned int tcc_LCDC_set_bgcolor(unsigned lcdc, unsigned red, unsigned green, unsigned blue)
{
	volatile PLCDC pstLCDC;
	unsigned uRegValue;

	if(lcdc == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	uRegValue = ((blue & 0xff)<<16) | ((green & 0xff)<<8) | (red & 0xff);

	pstLCDC->LBC = uRegValue;
	return 0;	
}
EXPORT_SYMBOL(tcc_LCDC_set_bgcolor);

unsigned int tcc_LCDC_set_colorenhance(unsigned lcdc, unsigned hue, unsigned brightness, unsigned contrast)
{
	volatile PLCDC pstLCDC;
	unsigned uRegValue;

	if(lcdc == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	uRegValue = ((hue & 0xff)<<16) | ((brightness & 0xff)<<8) | (contrast & 0xff);

	pstLCDC->LENH = uRegValue;

	return 0;	
}
EXPORT_SYMBOL(tcc_LCDC_set_colorenhance);

unsigned int tcc_LCDC_set_gamma(lcdc_gamma_params *gamma)
{
	volatile PLCDC pstLCDC;
	unsigned uRegValue[2];

	if(gamma->lcdc_num == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	if(gamma->onoff)
		BITSET(pstLCDC->LCTRL, HwLCTRL_GEN);
	else
		BITCLR(pstLCDC->LCTRL, HwLCTRL_GEN);

	uRegValue[0] = gamma->GammaRed[0] | (gamma->GammaRed[1]<<4) | (gamma->GammaRed[2]<<8) | (gamma->GammaRed[3]<<12)
				  | (gamma->GammaRed[4]<<16) |(gamma->GammaRed[5]<<20) |( gamma->GammaRed[6]<<24) | (gamma->GammaRed[7]<<28) ;
	uRegValue[1] = gamma->GammaRed[8] | (gamma->GammaRed[9]<<4) | (gamma->GammaRed[10]<<8) | (gamma->GammaRed[11]<<12)
				  | (gamma->GammaRed[12]<<16) |(gamma->GammaRed[13]<<20) |( gamma->GammaRed[14]<<24) | (gamma->GammaRed[15]<<28) ;
	pstLCDC->LGR0 = uRegValue[0];
	pstLCDC->LGR1 = uRegValue[1];
	
	uRegValue[0] = gamma->GammaGreen[0] | (gamma->GammaGreen[1]<<4) | (gamma->GammaGreen[2]<<8) | (gamma->GammaGreen[3]<<12)
				  | (gamma->GammaGreen[4]<<16) |(gamma->GammaGreen[5]<<20) |( gamma->GammaGreen[6]<<24) | (gamma->GammaGreen[7]<<28) ;
	uRegValue[1] = gamma->GammaGreen[8] | (gamma->GammaGreen[9]<<4) | (gamma->GammaGreen[10]<<8) | (gamma->GammaGreen[11]<<12)
				  | (gamma->GammaGreen[12]<<16) |(gamma->GammaGreen[13]<<20) |( gamma->GammaGreen[14]<<24) | (gamma->GammaGreen[15]<<28) ;
	pstLCDC->LGG0 = uRegValue[0];
	pstLCDC->LGG1 = uRegValue[1];

	uRegValue[0] = gamma->GammaBlue[0] | (gamma->GammaBlue[1]<<4) | (gamma->GammaBlue[2]<<8) | (gamma->GammaBlue[3]<<12)
				  | (gamma->GammaBlue[4]<<16) |(gamma->GammaBlue[5]<<20) |( gamma->GammaBlue[6]<<24) | (gamma->GammaBlue[7]<<28) ;
	uRegValue[1] = gamma->GammaBlue[8] | (gamma->GammaBlue[9]<<4) | (gamma->GammaBlue[10]<<8) | (gamma->GammaBlue[11]<<12)
				  | (gamma->GammaBlue[12]<<16) |(gamma->GammaBlue[13]<<20) |( gamma->GammaBlue[14]<<24) | (gamma->GammaBlue[15]<<28) ;
	pstLCDC->LGB0 = uRegValue[0];
	pstLCDC->LGB1 = uRegValue[1];
	
	dprintk("%s : %d  %d  0x%x 0x%x : 0x%x 0x%x : 0x%x 0x%x \n",__func__, gamma->onoff, gamma->lcdc_num,
									pstLCDC->LGR0, pstLCDC->LGR1, pstLCDC->LGG0, pstLCDC->LGG1, pstLCDC->LGB0, pstLCDC->LGB1 );
	return 0;	
}
EXPORT_SYMBOL(tcc_LCDC_set_gamma);

unsigned int tcc_LCDC_get_gamma(lcdc_gamma_params *gamma)
{
	volatile PLCDC pstLCDC;
	unsigned uRegGreen[2], uRegRed[2], uRegBlue[2];
	int i;
	
	if(gamma->lcdc_num == 0)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);

	if(ISSET(pstLCDC->LCTRL, HwLCTRL_GEN))
		gamma->onoff = 1;
	else
		gamma->onoff = 0;

	uRegRed[0] = pstLCDC->LGR0;
	uRegRed[1] = pstLCDC->LGR1;
	uRegGreen[0] = pstLCDC->LGG0;
	uRegGreen[1] = pstLCDC->LGG1;
	uRegBlue[0] = pstLCDC->LGB0;
	uRegBlue[1] = pstLCDC->LGB1;

	for(i = 0; i < 8 ; i++)	{
		gamma->GammaRed[i] = ((uRegRed[0]>>(i * 4)) & (Hw4 - Hw0));
		gamma->GammaGreen[i] = ((uRegGreen[0]>>(i * 4)) & (Hw4 - Hw0));
		gamma->GammaBlue[i] = ((uRegBlue[0]>>(i * 4)) & (Hw4 - Hw0));
	}
	
	for(i = 0; i < 8 ; i++)	{
		gamma->GammaRed[i + 8] = ((uRegRed[1]>>(i * 4)) & (Hw4 - Hw0));
		gamma->GammaGreen[i + 8] = ((uRegGreen[1]>>(i * 4)) & (Hw4 - Hw0));
		gamma->GammaBlue[i + 8] = ((uRegBlue[1]>>(i * 4)) & (Hw4 - Hw0));
	}
	dprintk("%s : %d  %d  0x%x 0x%x : 0x%x 0x%x : 0x%x 0x%x \n",__func__, gamma->onoff, gamma->lcdc_num,
									uRegRed[0], uRegRed[1], uRegGreen[0],uRegGreen[1], uRegBlue[0], uRegRed[1] );

	return 0;	
}
EXPORT_SYMBOL(tcc_LCDC_get_gamma);


void TCC_LCDC_Change_CtrlReg(PLCDC SrcLCDC, PLCDC DestLCDC)
{
	dprintk("%s  \n",__func__);

	DestLCDC->LBC = SrcLCDC->LBC;				//   0x04  R/W  0x00000000  LCD Background Color Register 
	DestLCDC->LCLKDIV = SrcLCDC->LCLKDIV;			//   0x08  R/W  0x00000000  LCD Clock Divider Register 
	DestLCDC->LHTIME1 = SrcLCDC->LHTIME1;			//   0x0C  R/W  0x00000000  LCD Horizontal Timing Register 1 
	DestLCDC->LHTIME2 = SrcLCDC->LHTIME2;			//   0x10  R/W  0x00000000  LCD Horizontal Timing Register 2 
	DestLCDC->LVTIME1 = SrcLCDC->LVTIME1;			//   0x14  R/W  0x00000000  LCD Vertical Timing Register 1 
	DestLCDC->LVTIME2 = SrcLCDC->LVTIME2;			//   0x18  R/W  0x00000000  LCD Vertical Timing Register 2 
	DestLCDC->LVTIME3 = SrcLCDC->LVTIME3;			//   0x1C  R/W  0x00000000  LCD Vertical Timing Register 3 
	DestLCDC->LVTIME4 = SrcLCDC->LVTIME4;			//   0x20  R/W  0x00000000  LCD Vertical Timing Register 4 
	DestLCDC->LLUTR = SrcLCDC->LLUTR;				//   0x24  R/W  0x00000000  LCD Lookup Register for Red 
	DestLCDC->LLUTG = SrcLCDC->LLUTG;				//   0x28  R/W  0x00000000  LCD Lookup Register for Green 
	DestLCDC->LLUTB = SrcLCDC->LLUTB;				//   0x2C  R/W  0x00000000  LCD Lookup Register for Blue 
	DestLCDC->LDP7L = SrcLCDC->LDP7L;				//   0x30  R/W  0x4d2b3401  LCD Modulo 7 Dithering Pattern (low) 
	DestLCDC->LDP7H = SrcLCDC->LDP7H;				//   0x34  R/W  0x0000003f  LCD Modulo 7 Dithering Pattern (high) 
	DestLCDC->LDP5 = SrcLCDC->LDP5;				//   0x38  R/W  0x1d0b0610  LCD Modulo 5 Dithering Pattern Register 
	DestLCDC->LDP4 = SrcLCDC->LDP4;				//   0x3C  R/W  0x00000768  LCD Modulo 4 Dithering Pattern Register 
	DestLCDC->LDP3 = SrcLCDC->LDP3;				//   0x40  R/W  0x00000034  LCD 3-bit Dithering Pattern Register 
	DestLCDC->LCP1 = SrcLCDC->LCP1;				//   0x44  R/W  0x000000ff  LCD Clipping Register1 
	DestLCDC->LCP2 = SrcLCDC->LCP2;				//   0x48  R/W  0x000000ff  LCD Clipping Register2 
	DestLCDC->LDS = SrcLCDC->LDS;				//   0x4C  R/W  0x00000000  LCD Display Size Register 
	DestLCDC->LSTATUS = SrcLCDC->LSTATUS;			//   0x50  R/CLR  0x00000000  LCD Status Register 
	DestLCDC->LIM = SrcLCDC->LIM;				//   0x54  R/W  0x0000001f  LCD Interrupt Register. 
	DestLCDC->LGR0 = SrcLCDC->LGR0;				//   0x58  R/W  0x00000000  LCD Gamma Correction Register 0 for Red Color 
	DestLCDC->LGR1 = SrcLCDC->LGR1;				//   0x5C  R/W  0x00000000  LCD Gamma Correction Register 1 for Red Color 
	DestLCDC->LGG0 = SrcLCDC->LGG0;				//   0x60  R/W  0x00000000  LCD Gamma Correction Register 0 for Green Color 
	DestLCDC->LGG1 = SrcLCDC->LGG1;				//   0x64  R/W  0x00000000  LCD Gamma Correction Register 1 for Green Color 
	DestLCDC->LGB0 = SrcLCDC->LGB0;				//   0x68  R/W  0x00000000  LCD Gamma Correction Register 0 for Blue Color 
	DestLCDC->LGB1 = SrcLCDC->LGB1;				//   0x6C  R/W  0x00000000  LCD Gamma Correction Register 1 for Blue Color 
	DestLCDC->LENH = SrcLCDC->LENH;				//   0x70  R/W  0x00000020  LCD Color Enhancement Register 
	

	DestLCDC->LCTRL = SrcLCDC->LCTRL;

}
EXPORT_SYMBOL(TCC_LCDC_Change_CtrlReg);

void TCC_LCDC_Change_ChannelReg(PLCDC_CHANNEL SrcLCDC_CH, PLCDC_CHANNEL DestLCDC_CH)
{
	dprintk("%s   \n",__func__);

	DestLCDC_CH->LIP = SrcLCDC_CH->LIP;	
	DestLCDC_CH->LIS = SrcLCDC_CH->LIS;	
	DestLCDC_CH->LIBA0 = SrcLCDC_CH->LIBA0;
	DestLCDC_CH->LICA = SrcLCDC_CH->LICA;
	DestLCDC_CH->LIBA1 = SrcLCDC_CH->LIBA1;
	DestLCDC_CH->LIBA2 = SrcLCDC_CH->LIBA2;
	DestLCDC_CH->LIO = SrcLCDC_CH->LIO;	
	DestLCDC_CH->LISR = SrcLCDC_CH->LISR;
	DestLCDC_CH->LIA = SrcLCDC_CH->LIA;	
	DestLCDC_CH->LIKR = SrcLCDC_CH->LIKR;
	DestLCDC_CH->LIKG = SrcLCDC_CH->LIKG;
	DestLCDC_CH->LIKB = SrcLCDC_CH->LIKB;
	DestLCDC_CH->LIEN = SrcLCDC_CH->LIEN;

	DestLCDC_CH->LIC = SrcLCDC_CH->LIC;	
}
EXPORT_SYMBOL(TCC_LCDC_Change_ChannelReg);



unsigned int tcc_LCDC_get_info(struct tcc_lcdc_get_image_info *lcd_info)
{
	volatile PLCDC pstLCDC;
	volatile PLCDC_CHANNEL pstLCDC_CH;

	if(lcd_info->lcdc_num)
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC1_BASE);
	else
		pstLCDC = (volatile PLCDC)tcc_p2v(HwLCDC0_BASE);

	switch(lcd_info->layer)
	{
		case 0:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI0C;
			break;
		case 1:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI1C;
			break;
		case 2:
			pstLCDC_CH = (volatile PLCDC_CHANNEL)&pstLCDC->LI2C;
			break;
	}
	
	lcd_info->lcd_width = (pstLCDC->LDS & 0xFFFF);
	lcd_info->lcd_height = (pstLCDC->LDS >> 16) & 0xFFFF; 

	lcd_info->enable = (pstLCDC_CH->LIC & HwLIC_IEN)?1:0;

	lcd_info->image_width = (pstLCDC_CH->LIS & 0xFFFF);
	lcd_info->image_height =(pstLCDC_CH->LIS >> 16) & 0xFFFF; 

	lcd_info->offset_x = (pstLCDC_CH->LIP & 0xFFFF);
	lcd_info->offset_y =(pstLCDC_CH->LIP >> 16) & 0xFFFF; 

	lcd_info->addr0 = pstLCDC_CH->LIBA0;
	lcd_info->addr1 = pstLCDC_CH->LIBA1; 
	lcd_info->addr2 = pstLCDC_CH->LIBA2; 
	lcd_info->fmt = pstLCDC_CH->LIC & 0x1F; 

	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_get_info);

