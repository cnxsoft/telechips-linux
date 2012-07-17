
#include <linux/kernel.h>
#include <mach/vioc_outcfg.h>
#include <mach/globals.h>
#include <mach/io.h>

/* -------------------------------
2¡¯b00 : Display Device 0 Component
2¡¯b01 : Display Device 1 Component
2¡¯b10 : Display Device 2 Component
2¡¯b11 : NOT USED
---------------------------------*/
void VIOC_OUTCFG_SetOutConfig (unsigned nType, unsigned nDisp)
{
	VIOC_OUTCFG *gpOutConfig;
	gpOutConfig = (VIOC_OUTCFG *)tcc_p2v(HwVIOC_OUTCFG);

	printk ("%s : addr:%p, nType:%d nDisp:%d 0x%x \n", __func__, gpOutConfig, nType, nDisp, tcc_p2v(10));

	switch (nType)
	{
		case VIOC_OUTCFG_HDMI :
			//gpOutConfig->uMISCCFG.bREG.HDMISEL   = nDisp;
			BITCSET(gpOutConfig->uMISCCFG.nREG, 0x3, nDisp) ;
			break;
		case VIOC_OUTCFG_SDVENC :
			//gpOutConfig->uMISCCFG.bREG.SDVESEL   = nDisp;
			BITCSET(gpOutConfig->uMISCCFG.nREG, 0x3 << 4, nDisp << 4) ;
			break;
		case VIOC_OUTCFG_HDVENC :
			//gpOutConfig->uMISCCFG.bREG.HDVESEL   = nDisp;
			BITCSET(gpOutConfig->uMISCCFG.nREG, 0x3 << 8, nDisp << 8) ;
			break;
		case VIOC_OUTCFG_M80 :
			//gpOutConfig->uMISCCFG.bREG.M80SEL    = nDisp;
			BITCSET(gpOutConfig->uMISCCFG.nREG, 0x3 << 12, nDisp << 12) ;
			break;
		case VIOC_OUTCFG_MRGB :
			//gpOutConfig->uMISCCFG.bREG.MRGBSEL   = nDisp;
			BITCSET(gpOutConfig->uMISCCFG.nREG, 0x3 << 16, nDisp << 16) ;
			break;
		default :
			printk ("Not supported type ...");
			break;
	}
}

// vim:ts=4:et:sw=4:sts=4
