

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
#include <mach/vioc_outcfg.h>
#include <mach/globals.h>
#include <mach/reg_physical.h>

#include <mach/vioc_rdma.h>
#include <mach/vioc_wdma.h>
#include <mach/vioc_wmix.h>
#include <mach/vioc_disp.h>
#include <mach/vioc_ireq.h>


//#define LCDC_AXI_BUS

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tca vioc: " msg); }
	
#if defined(CONFIG_FB_TCC_COMPONENT)
extern int tcc_component_lcdc_handler_check(char lcdc_num);
#endif



void tca_lcdc_interrupt_onoff(char onoff, char lcdc)
{
	VIOC_DISP * pDISPBase;
	dprintk("%s %d ! onoff: %d  \n", __func__, lcdc, onoff);
	
	if(lcdc)	{
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);
	}
	else {
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP0);
	}
	
	#if defined(CONFIG_FB_TCC_COMPONENT)
		if((onoff == 0) && tcc_component_lcdc_handler_check(lcdc))
		{
			dprintk("%s can't disable lcdc interrupt(component_1080i)!!\n", __func__);
			return;
		}
	#endif

	// init INT mask reg
	pDISPBase->uLSTATUS.nREG = 0xFFFFFFFF;

	if(onoff)		//VIOC INT en
	{
		VIOC_DISP_SetIreqMask(pDISPBase, VIOC_DISP_IREQ_RU_MASK, 0);
	}
	else			//VIOC INT dis
	{
		VIOC_DISP_SetIreqMask(pDISPBase, VIOC_DISP_IREQ_RU_MASK, VIOC_DISP_IREQ_RU_MASK);
	}
	

}
EXPORT_SYMBOL(tca_lcdc_interrupt_onoff);



void lcdc_initialize(struct lcd_panel *lcd_spec, unsigned int lcdc_num)
{
	VIOC_DISP * pDISPBase;
	VIOC_WMIX* pWIXBase;

	if(lcdc_num) // Set the address for LCDC0 or LCDC1
	{
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);
		pWIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);
	}
	else
	{
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP0);
		pWIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);
	}

	VIOC_WMIX_SetSize(pWIXBase, lcd_spec->xres, lcd_spec->yres);	
	VIOC_WMIX_SetOverlayPriority(pWIXBase, 24);	
	VIOC_WMIX_SetBGColor(pWIXBase, 0, 0, 0, 0);	

	if(lcd_spec->bus_width == 24)
		//pDISPBase->uCTRL.bREG.PXDW = 0xC;
		BITCSET(pDISPBase->uCTRL.nREG, 0xF << 16 , 0xC << 16);
	else if(lcd_spec->bus_width == 18)
		//pDISPBase->uCTRL.bREG.PXDW = 0x5;
		BITCSET(pDISPBase->uCTRL.nREG, 0xF << 16 , 0x5 << 16) ;
	else
		//pDISPBase->uCTRL.bREG.PXDW = 0x3;
		BITCSET(pDISPBase->uCTRL.nREG, 0xF << 16 , 0x3 << 16);


	if(lcd_spec->sync_invert & ID_INVERT)
		//pDISPBase->uCTRL.bREG.ID = 1;
		BITCSET(pDISPBase->uCTRL.nREG, 1 << 15 , 1 << 15);


	if(lcd_spec->sync_invert & IV_INVERT)
		//pDISPBase->uCTRL.bREG.IV = 1;
		BITCSET(pDISPBase->uCTRL.nREG, 1 << 14 , 1 << 14);

	if(lcd_spec->sync_invert & IH_INVERT)
		//pDISPBase->uCTRL.bREG.IH = 1;
		BITCSET(pDISPBase->uCTRL.nREG, 1 << 13 , 1 << 13);

	if(lcd_spec->sync_invert & IP_INVERT)
		//pDISPBase->uCTRL.bREG.IP = 1;
		BITCSET(pDISPBase->uCTRL.nREG, 1 << 12 , 1 << 12);
	

	//pDISPBase->uCTRL.bREG.CKG = 0; // clock gating enable 
	BITCSET(pDISPBase->uCTRL.nREG, 1 << 23 , 0 << 23);

	BITCSET(pDISPBase->uCTRL.nREG, 1 << 22 , 0 << 22);
	
	//pDISPBase->uCTRL.bREG.NI = 1;
	BITCSET(pDISPBase->uCTRL.nREG, 1 << 8 , 1 << 8);

// Set LCD clock
	//pDISPBase->uCLKDIV.bREG.ACDIV= 1;
	//pDISPBase->uCLKDIV.bREG.PXCLKDIV = lcd_spec->clk_div/2;

	BITCSET(pDISPBase->uCLKDIV.nREG,0x00FF0000,  1 << 16 );
	BITCSET(pDISPBase->uCLKDIV.nREG,0x000000FF, lcd_spec->clk_div/2);

// Background color
	//pDISPBase->uBG.nREG = 0x00000000;
	BITCSET(pDISPBase->uBG.nREG,0xFFFFFFFF,0x00000000);

//	Horizontal timing
	//pDISPBase->uLHTIME1.bREG.LPC  = (lcd_spec->xres - 1);
	BITCSET(pDISPBase->uLHTIME1.nREG, 0x00003FFF, (lcd_spec->xres - 1) );

	//pDISPBase->uLHTIME1.bREG.LPW  = lcd_spec->lpw;
	BITCSET(pDISPBase->uLHTIME1.nREG, 0x01FF0000, lcd_spec->lpw<< 16 );
	
	//pDISPBase->uLHTIME2.bREG.LEWC = lcd_spec->lewc;
	BITCSET(pDISPBase->uLHTIME2.nREG, 0x01FF01FF, (lcd_spec->lswc << 16) | lcd_spec->lewc );


//	Vertical timing
	//pDISPBase->uLVTIME1.bREG.FLC = lcd_spec->yres;
	//pDISPBase->uLVTIME1.bREG.FPW = lcd_spec->fpw1;

	BITCSET(pDISPBase->uLVTIME1.nREG, 0x3F << 16 , lcd_spec->fpw1 << 16);	
	BITCSET(pDISPBase->uLVTIME1.nREG, 0x00003FFF, lcd_spec->yres);
		
	
	//pDISPBase->uLVTIME2.bREG.FEWC = lcd_spec->fewc1;
	//pDISPBase->uLVTIME2.bREG.FSWC = lcd_spec->fswc1;

	BITCSET(pDISPBase->uLVTIME2.nREG, 0x01FF01FF, (lcd_spec->fswc1 << 16) | lcd_spec->fewc1 );

	
	//pDISPBase->uLVTIME3.bREG.FLC = lcd_spec->yres;
	//pDISPBase->uLVTIME3.bREG.FPW = lcd_spec->fpw2;
	
	BITCSET(pDISPBase->uLVTIME3.nREG, 0x3F << 16 , lcd_spec->fpw2 << 16);	
	BITCSET(pDISPBase->uLVTIME3.nREG, 0x00003FFF, lcd_spec->yres);
	
	//pDISPBase->uLVTIME4.bREG.FEWC = lcd_spec->fewc2;
	//pDISPBase->uLVTIME4.bREG.FSWC = lcd_spec->fswc2;

	BITCSET(pDISPBase->uLVTIME4.nREG, 0x01FF01FF, (lcd_spec->fswc2 << 16) | lcd_spec->fewc2 );

	
	//pDISPBase->uLSIZE.bREG.HSIZE = lcd_spec->xres;
	//pDISPBase->uLSIZE.bREG.VSIZE = lcd_spec->yres;

	BITCSET(pDISPBase->uLSIZE.nREG, 0x1FFF1FFF, (lcd_spec->yres << 16) | lcd_spec->xres );

	//pDISPBase->uCTRL.bREG.LEN = 1;

	BITCSET(pDISPBase->uCTRL.nREG,1,1);

}

EXPORT_SYMBOL(lcdc_initialize);

/*===========================================================================
FUNCTION
 - index : 0 = lcdc0, 1 = lcdc1
===========================================================================*/
unsigned int DEV_LCDC_Wait_signal(char lcdc)
{

	// TO DO
	#define MAX_LCDC_WAIT_TIEM 		0x70000000

	unsigned loop = 1;
	VIOC_DISP * pDISPBase;
	VIOC_RDMA * pRDMABase;
	PPMU pPMU = (PPMU)(tcc_p2v(HwPMU_BASE));
	PCKC pCKC  = (PCKC)(tcc_p2v(HwCKC_BASE)); 
	PDDICONFIG pDDICONFIG = (PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE));

	if ( ISZERO(pCKC->CLKCTRL2.nREG, Hw21) || ISZERO(pPMU->PMU_PWRSTS.nREG, Hw2) )
		return FALSE;
	
	if(lcdc == 0 ){
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);

		if(ISZERO(pDDICONFIG->PWDN.nREG, HwDDIC_PWDN_LCDC))
			return FALSE;
	}
	else{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
		if(ISZERO(pDDICONFIG->PWDN.nREG, HwDDIC_PWDN_LCDC))
			return FALSE;
	}
	
	if ( ISZERO(pDISPBase->uCTRL.nREG, HwDISP_LEN) )
		return FALSE;

	BITSET(pRDMABase->uSTATUS.nREG, Hw4);

	while(TRUE && (loop < MAX_LCDC_WAIT_TIEM))
	{
		if(ISSET(pRDMABase->uSTATUS.nREG, Hw4))
			break;
		loop++;
	}
#if 0
	loop=1;
	while(TRUE && (loop < MAX_LCDC_WAIT_TIEM) )
	{
		if(ISSET(pDISPBase->uLSTATUS.nREG , HwLSTATUS_DEOF))
			break;
		loop++;
	}
#endif//
 	return loop;
}
EXPORT_SYMBOL(DEV_LCDC_Wait_signal);

unsigned int DEV_LCDC_Wait_signal_Ext(void)
{
	#define MAX_LCDC_WAIT_TIEM 		0x70000000

	VIOC_DISP *pDISPBase0, *pDISPBase1;
	VIOC_RDMA *pRDMABase0, *pRDMABase1;
	VIOC_OUTCFG *pOUTCFGBase = (VIOC_OUTCFG *)tcc_p2v(HwVIOC_OUTCFG);
	PDDICONFIG pDDICONFIG = (PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	PPMU pPMU = (PPMU)(tcc_p2v(HwPMU_BASE));
	PCKC pCKC = (PCKC)(tcc_p2v(HwCKC_BASE)); 
	int disp_en0=0, disp_en1=0;

	if(ISZERO(pCKC->CLKCTRL2.nREG, Hw21) || ISZERO(pPMU->PMU_PWRSTS.nREG, Hw2))
		return FALSE;
	
	if(ISZERO(pDDICONFIG->PWDN.nREG, HwDDIC_PWDN_LCDC))
		return FALSE;
			
	pDISPBase0 = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
	pRDMABase0 = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);
	if(ISSET(pDISPBase0->uCTRL.nREG, HwDISP_LEN))
		disp_en0 = 1;

	pDISPBase1 = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
	pRDMABase1 = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
	if(ISSET(pDISPBase1->uCTRL.nREG, HwDISP_LEN))
		disp_en1 = 1;

	if(disp_en0 && disp_en1)
	{
		if((pOUTCFGBase->uMISCCFG.nREG & 0x3) == 1)
		{
			DEV_LCDC_Wait_signal(0);
			DEV_LCDC_Wait_signal(1);
		}
		else
		{
			DEV_LCDC_Wait_signal(1);
			DEV_LCDC_Wait_signal(0);
		}
	}
	else
	{
		if(disp_en0)
			DEV_LCDC_Wait_signal(0);
			
		if(disp_en1)
			DEV_LCDC_Wait_signal(1);
	}

 	return 1;
}
EXPORT_SYMBOL(DEV_LCDC_Wait_signal_Ext);

unsigned int DEV_LCDC_Status(char lcdc)
{
	dprintk("%s \n",__func__);

	VIOC_DISP * pDISPBase;
	VIOC_RDMA * pRDMABase;
	PPMU pPMU = (PPMU)(tcc_p2v(HwPMU_BASE));
	PCKC pCKC  = (PCKC)(tcc_p2v(HwCKC_BASE)); 
	PDDICONFIG pDDICONFIG = (PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE));

	if ( ISZERO(pCKC->CLKCTRL2.nREG, Hw21) || ISZERO(pPMU->PMU_PWRSTS.nREG, Hw2) )
		return FALSE;

	if(lcdc == 0 ){
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);

		if(ISZERO(pDDICONFIG->PWDN.nREG, HwDDIC_PWDN_LCDC))
			return FALSE;
	}else{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
		
		if(ISZERO(pDDICONFIG->PWDN.nREG, HwDDIC_PWDN_LCDC))
			return FALSE;		
	}

	if(ISZERO(pRDMABase->uCTRL.nREG, HwDMA_IEN))
		return FALSE;

	if ( ISZERO(pDISPBase->uCTRL.nREG , HwDISP_LEN) )
		return FALSE;
	
	return pDISPBase->uLSTATUS.nREG;
		
}
EXPORT_SYMBOL(DEV_LCDC_Status);

unsigned char tcc_LCDC_Wait_signal_disable(char lcdc)
{
	// TO DO

	unsigned int loop=0;
	VIOC_DISP * pDISPBase;
	VIOC_RDMA * pRDMABase;


	PDDICONFIG pddiconfig;
	pddiconfig = (PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE));

	if(lcdc == 0 ){
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);

		if(ISZERO(pddiconfig->PWDN.nREG, HwDDIC_PWDN_HDMI))
		return FALSE;		
	}else{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);

		if(ISZERO(pddiconfig->PWDN.nREG, HwDDIC_PWDN_LCDC))
		return FALSE;
	}
	
	if(ISSET(pRDMABase->uCTRL.nREG, HwDMA_IEN) )
	{
		BITCLR(pRDMABase->uCTRL.nREG, HwDMA_IEN);

		while(TRUE && (loop < 500000))
		{
			if( ( ISZERO(pRDMABase->uSTATUS.nREG, Hw19)) )
				break;
			loop++;			
		}
	}
	
	loop = 0;

	if ( ISSET(pDISPBase->uCTRL.nREG , HwDISP_LEN) )
	{
		BITCLR(pDISPBase->uCTRL.nREG , HwDISP_LEN);

		while(TRUE && (loop < 500000) ) //check whether is lcd on
		{
			if( ( ISZERO(pDISPBase->uLSTATUS.nREG, HwLSTATUS_BUSY)) && (ISSET(pDISPBase->uLSTATUS.nREG ,HwLSTATUS_DD )) )
				break;
			loop++;
		}
	}

	return TRUE;
}


unsigned int tcc_LCDC_ctrl_layer(unsigned lcdc, unsigned layer, unsigned onoff)
{
	VIOC_DISP * pDISPBase;
	VIOC_RDMA * pRDMABase;
	VIOC_WMIX* pWMIXBase;
	
	if(lcdc){
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);		

		switch(layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA06);
				break;
		}

	}else{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);
		
		switch(layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA02);
				break;
		}		
	}

	if(onoff)
		VIOC_RDMA_SetImageEnable(pRDMABase);
	else
		VIOC_RDMA_SetImageDisable(pRDMABase);

	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_ctrl_layer);


unsigned int tcc_LCDC_set_alpha(unsigned lcdc, unsigned layer, unsigned alpha)
{
	VIOC_RDMA * pRDMABase;
	
	if(lcdc){
		switch(layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA06);
				break;
		}

	}else{
		switch(layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA02);
				break;
		}		
	}

	VIOC_RDMA_SetImageAlphaEnable(pRDMABase, 1);
	VIOC_RDMA_SetImageAlpha(pRDMABase, alpha, alpha);
	
	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_set_alpha);

unsigned int tcc_LCDC_set_chromakey(lcdc_chromakey_params *lcdc_chromakey)
{
	VIOC_WMIX *pWMIX;
	
	if(lcdc_chromakey->lcdc_num)
		pWMIX = (VIOC_WMIX *)tcc_p2v(HwVIOC_WDMA01);
	else
		pWMIX = (VIOC_WMIX *)tcc_p2v(HwVIOC_WDMA00);

	VIOC_WMIX_SetChromaKey(pWMIX, lcdc_chromakey->layer_num, 1, lcdc_chromakey->chromaR, lcdc_chromakey->chromaG, lcdc_chromakey->chromaB, 0xF8,  0xFC, 0xF8);
	
	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_set_chromakey);

unsigned int tcc_LCDC_set_bgcolor(unsigned lcdc, unsigned red, unsigned green, unsigned blue)
{
	VIOC_DISP * pDISPBase;
	if(lcdc)
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP1);
	else
		pDISPBase = (VIOC_DISP *)tcc_p2v(HwVIOC_DISP0);		

	VIOC_DISP_SetBGColor(pDISPBase, red, green, blue);
	
	return 0;	
}
EXPORT_SYMBOL(tcc_LCDC_set_bgcolor);

unsigned int tcc_LCDC_get_info(struct tcc_lcdc_get_image_info *lcd_info)
{
	VIOC_DISP * pDISPBase;
	VIOC_RDMA * pRDMABase;
	VIOC_WMIX* pWMIXBase;
	
	if(lcd_info->lcdc_num){
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX1);		

		switch(lcd_info->layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA05);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA06);
				break;
		}

	}else{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pWMIXBase = (VIOC_WMIX*)tcc_p2v(HwVIOC_WMIX0);
		
		switch(lcd_info->layer)
		{			
			case 0:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);
				break;
			case 1:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA01);
				break;
			case 2:
				pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA02);
				break;
		}		
	}

	lcd_info->lcd_width = (pDISPBase->uLSIZE.nREG & 0xFFFF);
	lcd_info->lcd_height = (pDISPBase->uLSIZE.nREG >> 16) & 0xFFFF; 

	lcd_info->enable = (pRDMABase->uCTRL.nREG & HwDMA_IEN ) ?1:0;

	lcd_info->image_width = (pRDMABase->uSIZE.nREG & 0xFFFF);
	lcd_info->image_height =(pRDMABase->uSIZE.nREG >> 16) & 0xFFFF; 

	lcd_info->offset_x = (pRDMABase->uOFFSET.nREG & 0xFFFF);
	lcd_info->offset_y = (pRDMABase->uOFFSET.nREG >> 16) & 0xFFFF; 

	lcd_info->addr0 = pRDMABase->nBASE0;
	lcd_info->addr1 = pRDMABase->nBASE1; 
	lcd_info->addr2 = pRDMABase->nBASE2; 
	lcd_info->fmt = pRDMABase->uCTRL.nREG & 0x1F; 	

	return 1;
}
EXPORT_SYMBOL(tcc_LCDC_get_info);

void tcc_LCDC_onoff_ctrl(unsigned int lcdc, unsigned int onoff)
{
	dprintk("%s \n",__func__);

	unsigned loop = 0;
	VIOC_DISP * pDISPBase;
	VIOC_RDMA * pRDMABase;
	PCKC pCKC  = (PCKC)(tcc_p2v(HwCKC_BASE)); 
	
	if(lcdc){
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
	}else{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);		
	}	

	if(onoff){
		if(lcdc)
			BITSET(pCKC->PCLKCTRL05.nREG, Hw29);
		else
			BITSET(pCKC->PCLKCTRL03.nREG, Hw29);

		VIOC_DISP_TurnOn(pDISPBase);
		VIOC_RDMA_SetImageEnable(pRDMABase);	
		
	}else{
		VIOC_RDMA_SetImageDisable(pRDMABase);
		VIOC_DISP_TurnOff(pDISPBase);

		if(lcdc)
			BITCLR(pCKC->PCLKCTRL05.nREG, Hw29);
		else
			BITCLR(pCKC->PCLKCTRL03.nREG, Hw29);
	}

}
EXPORT_SYMBOL(tcc_LCDC_onoff_ctrl);

unsigned int DEV_RDMA_Status(char lcdc)
{
	dprintk("%s \n",__func__);
	
	VIOC_RDMA * pRDMABase;
	VIOC_DISP * pDISPBase;
	PPMU pPMU = (PPMU)(tcc_p2v(HwPMU_BASE));
	PCKC pCKC  = (PCKC)(tcc_p2v(HwCKC_BASE)); 
	PDDICONFIG pDDICONFIG = (PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE));

	if ( ISZERO(pCKC->CLKCTRL2.nREG, Hw21) || ISZERO(pPMU->PMU_PWRSTS.nREG, Hw2) )
		return FALSE;		
		
	if(lcdc == 0 ){
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP0);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA00);

		if(ISZERO(pDDICONFIG->PWDN.nREG, HwDDIC_PWDN_LCDC))
			return FALSE;

		if(ISZERO(pCKC->PCLKCTRL03.nREG, Hw29))
			return FALSE;

	}else{
		pDISPBase = (VIOC_DISP*)tcc_p2v(HwVIOC_DISP1);
		pRDMABase = (VIOC_RDMA*)tcc_p2v(HwVIOC_RDMA04);
		
		if(ISZERO(pDDICONFIG->PWDN.nREG, HwDDIC_PWDN_LCDC))
			return FALSE;		

		if(ISZERO(pCKC->PCLKCTRL05.nREG, Hw29))
			return FALSE;
	}

	if ( ISZERO(pDISPBase->uCTRL.nREG, HwDISP_LEN) )
		return FALSE;	
	
	if( pRDMABase->uCTRL.nREG & HwDMA_IEN) 
		return TRUE;
	else
		return FALSE;
		
}
EXPORT_SYMBOL(DEV_RDMA_Status);
