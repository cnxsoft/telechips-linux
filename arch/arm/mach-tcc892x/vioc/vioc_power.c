#include <linux/kernel.h>
#include <mach/vioc_outcfg.h>


void VIOC_OUTCFG_SetOutConfig (unsigned nType, unsigned nDisp)
{
	static VIOC_OUTCFG *gpOutConfig = (VIOC_OUTCFG *)tcc_p2v(HwVIOC_OUTCFG);

	switch (nType)
	{
		case VIOC_OUTCFG_HDMI :
			gpOutConfig->uMISCCFG.bREG.HDMISEL   = nDisp;
			break;
		case VIOC_OUTCFG_SDVENC :
			gpOutConfig->uMISCCFG.bREG.SDVESEL   = nDisp;
			break;
		case VIOC_OUTCFG_HDVENC :
			gpOutConfig->uMISCCFG.bREG.HDVESEL   = nDisp;
			break;
		case VIOC_OUTCFG_M80 :
			gpOutConfig->uMISCCFG.bREG.M80SEL    = nDisp;
			break;
		case VIOC_OUTCFG_MRGB :
			gpOutConfig->uMISCCFG.bREG.MRGBSEL   = nDisp;
			break;
		default :
			printk("Not supported type ...");
			break;
	}
}

// vim:ts=4:et:sw=4:sts=4
