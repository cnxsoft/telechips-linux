/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : CAMERA INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCCXXX
*    SOURCE        : cam.c
*
*    START DATE    : 2006. 4. 17.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3-2 TEAM
*                : TELECHIPS, INC.
************************************************************************/
#include <linux/delay.h> 
#include <mach/hardware.h>
#include <linux/clk.h>


#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#include <asm/plat-tcc/tcc_ckc.h>
#endif

#include "cam.h"
#include "cam_reg.h"
#include "sensor_if.h"


static struct clk *cifmc_clk;
#if defined(CONFIG_USE_ISP)
static struct clk *isps_clk;
static struct clk *ispj_clk;
#elif	defined(CONFIG_ARCH_TCC892X)
		// Already, VIOC Block enable.	
#else
static struct clk *cifsc_clk;
#endif

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern TCC_SENSOR_INFO_TYPE tcc_sensor_info;
#endif
/**********************************************************
*
*    Function of    
*
*    
*
*    Input    : 
*    Output    : 
*    Return    : 
*
*    Description    : 
**********************************************************/
void CIF_Clock_Get(void)
{
	#if	defined(CONFIG_ARCH_TCC892X)
		//	In case of 892X, we have to add.
		if(system_rev == 0x1008)
			cifmc_clk = clk_get(NULL, "out1");	
		else
			cifmc_clk = clk_get(NULL, "out0");	
		
		BUG_ON(cifmc_clk == NULL);
	#else
		cifmc_clk = clk_get(NULL, "cifmc");	
		BUG_ON(cifmc_clk == NULL);
		
		#if defined(CONFIG_USE_ISP)
		isps_clk = clk_get(NULL, "isps");
		BUG_ON(isps_clk == NULL);

		ispj_clk = clk_get(NULL, "ispj");
		BUG_ON(ispj_clk == NULL);
		#else
		cifsc_clk = clk_get(NULL, "cifsc");	
		BUG_ON(cifsc_clk == NULL);
		#endif
	#endif
}

void CIF_Clock_Put(void)
{
	#if	defined(CONFIG_ARCH_TCC892X)
		//	In case of 892X, we have to add.
		clk_put(cifmc_clk);
	#else
		clk_put(cifmc_clk);
		#if defined(CONFIG_USE_ISP)
		clk_put(isps_clk);
		clk_put(ispj_clk);
		#else
		clk_put(cifsc_clk);
		#endif
	#endif
}

/**********************************************************
*
*    Function of    
*
*    
*
*    Input    : 
*    Output    : 
*    Return    : 
*
*    Description    : 
**********************************************************/
void CIF_Open(void)
{
#if defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCAMBUSCFG pCamBusCfg = (PCAMBUSCFG)tcc_p2v(HwCAMBUSCFG_BASE);
#elif	defined(CONFIG_ARCH_TCC892X)	
	volatile PVIOC_IREQ_CONFIG pVIOCCfg = (PVIOC_IREQ_CONFIG)tcc_p2v(HwVIOC_CONFIG);
#endif

#if defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	#if defined(CONFIG_USE_ISP)
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	#ifndef CONFIG_VIDEO_CAMERA_NEXTCHIP_TEST
	clk_set_rate(cifmc_clk, tcc_sensor_info.m_clock*100);
	printk("CLK :: MCLK = %d, SCLK = %d. \n", tcc_sensor_info.m_clock/10000, tcc_sensor_info.s_clock/10000);
	#endif
	#else
	#ifndef CONFIG_VIDEO_CAMERA_NEXTCHIP_TEST
	clk_set_rate(cifmc_clk, CKC_CAMERA_MCLK*100);
	#endif
	printk("ISP CLK :: MCLK = %d, ISPS = %d, ISPJ = %d. \n", CKC_CAMERA_MCLK/10000, 1620000/10000, 1620000*2/10000);
	#endif //CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	#if defined(CONFIG_ARCH_TCC88XX)
	clk_set_rate(isps_clk, 168*1000*1000);
	clk_set_rate(ispj_clk, 168*1000*1000*2);
	#else
	clk_set_rate(isps_clk, 162*1000*1000);
	clk_set_rate(ispj_clk, 162*1000*1000*2);
	#endif

	#ifndef CONFIG_VIDEO_CAMERA_NEXTCHIP_TEST
	clk_enable(cifmc_clk);
	#endif
	clk_enable(isps_clk);
	clk_enable(ispj_clk);
	#else //use cif
	//PLL2 : 400Mhz
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	clk_set_rate(cifmc_clk, tcc_sensor_info.m_clock*100);
	clk_set_rate(cifsc_clk, tcc_sensor_info.s_clock*100);
	printk("CLK :: MCLK = %d, SCLK = %d. \n", tcc_sensor_info.m_clock/10000, tcc_sensor_info.s_clock/10000);
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	clk_set_rate(cifmc_clk, CKC_CAMERA_MCLK*100);
	clk_set_rate(cifsc_clk, CKC_CAMERA_SCLK*100);
	printk("CLK :: MCLK = %d, SCLK = %d. \n", CKC_CAMERA_MCLK/10000, CKC_CAMERA_SCLK/10000);
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

	clk_enable(cifmc_clk);
	clk_enable(cifsc_clk);
	#endif
#elif	defined(CONFIG_ARCH_TCC892X)
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		clk_set_rate(cifmc_clk, tcc_sensor_info.m_clock*100);
		printk("VIDEO IN Dual Camera CLK :: MCLK = %d \n", tcc_sensor_info.m_clock/10000);
	#else	
		clk_set_rate(cifmc_clk, CKC_CAMERA_MCLK*100);	
		printk("VIDEO IN Single Camera CLK :: MCLK = %d \n", CKC_CAMERA_MCLK/10000);
	#endif //CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

	clk_enable(cifmc_clk);
#else
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	clk_set_rate(cifmc_clk, tcc_sensor_info.m_clock*100);
	clk_set_rate(cifsc_clk, tcc_sensor_info.s_clock*100);
	printk("CLK :: MCLK = %d, SCLK = %d. \n", tcc_sensor_info.m_clock/10000, tcc_sensor_info.s_clock/10000);
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	#if !(defined(CONFIG_VIDEO_CAMERA_SENSOR_TVP5150) || defined(CONFIG_VIDEO_CAMERA_SENSOR_GT2005))
	//PLL2 : 400Mhz
	clk_set_rate(cifmc_clk, CKC_CAMERA_MCLK*100);
	#endif
	clk_set_rate(cifsc_clk, CKC_CAMERA_SCLK*100);
	printk("CLK :: MCLK = %d, SCLK = %d. \n", CKC_CAMERA_MCLK/10000, CKC_CAMERA_SCLK/10000);
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	clk_enable(cifmc_clk);
	clk_enable(cifsc_clk);
#endif

#if defined(CONFIG_ARCH_TCC79X)
	BITSET(HwSWRESET, HwSWRESET_CIC_ON);
	BITCLR(HwSWRESET, HwSWRESET_CIC_ON);
#elif defined(CONFIG_ARCH_TCC92XX)
	BITSET(HwDDI_CONFIG->SWRESET, HwDDIC_SWRESET_CIF);   // Reset
	BITCLR(HwDDI_CONFIG->SWRESET, HwDDIC_SWRESET_CIF);  // Normal
#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	//BITCLR(pCamBusCfg->CAMBusClk0Sel, HwCAMBUS_CLKO_SEL);
	BITCSET(pCamBusCfg->CAMBusClk0Sel, 0x0000000F, Hw1); //20101202 ysseung   posted write option enable.
	#if defined(CONFIG_USE_ISP)
	BITSET(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_ISP);   // Reset
	BITCLR(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_ISP);  // Normal
	#else
	BITSET(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_CIF);   // Reset
	BITCLR(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_CIF);  // Normal
	#endif
#elif	defined(CONFIG_ARCH_TCC892X)

	// VIN SW Reset
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[0], 0x03000000, (1<<24));
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[0], 0x03000000, (0<<24));
	// Scaler Reset
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[0], 0x10000000, (1<<28));
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[0], 0x10000000, (0<<28));
	// WMIX SW Reset
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[1], 0x00004000, (1<<14));
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[1], 0x00004000, (0<<14));
	// WDMA Reset
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[1], 0x00000020, (1<<5));
	BITCSET(pVIOCCfg->uSOFTRESET.nREG[1], 0x00000020, (0<<5));

#endif
}

/**********************************************************
*
*    Function of    
*
*    
*
*    Input    : 
*    Output    : 
*    Return    : 
*
*    Description    : 
**********************************************************/
void CIF_Close(void)
{
#if defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCAMBUSCFG pCamBusCfg = (PCAMBUSCFG)tcc_p2v(HwCAMBUSCFG_BASE);
	#ifndef CONFIG_VIDEO_CAMERA_NEXTCHIP_TEST
	clk_disable(cifmc_clk);
	#endif
	#if defined(CONFIG_USE_ISP)
	clk_disable(isps_clk);
	clk_disable(ispj_clk);
	#else
	clk_disable(cifsc_clk);
	#endif

	BITSET(pCamBusCfg->CAMBusClk0Sel, HwCAMBUS_CLKO_SEL);
#elif	defined(CONFIG_ARCH_TCC892X)
	clk_disable(cifmc_clk);
#else
	clk_disable(cifmc_clk);
	clk_disable(cifsc_clk);
#endif
}

/**********************************************************
*
*    Function of    
*
*    
*
*    Input    : 
*    Output    : 
*    Return    : 
*
*    Description    : 
**********************************************************/
void CIF_ONOFF(unsigned int uiOnOff)
{
#ifdef CONFIG_ARCH_TCC79X
	if(uiOnOff == ON)
	{
		BITCSET(HwICPCR1, HwICPCR1_ON, (uiOnOff << 31));
	}
	else if(uiOnOff == OFF)
	{
		BITCSET(HwICPCR1, HwICPCR1_ON, (uiOnOff << 31));
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else  //CONFIG_ARCH_TCC92XX
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);

	if(uiOnOff == ON)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_ON, (uiOnOff << 31));
	}
	else if(uiOnOff == OFF)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_ON, (uiOnOff << 31));
	}
#endif
}


/**********************************************************
*
*    Function of    
*
*
*    Input    : 
*    Output    : 
*    Return    : 
*
*    Description    : 
**********************************************************/
void CIF_WaitFrameSync(unsigned int exp_timer)
{
	unsigned int cnt=0;
	
#ifdef CONFIG_ARCH_TCC79X
	while((HwCIRQ & HwCIRQ_SOF) != HwCIRQ_SOF)
	{
		mdelay(1);
		cnt++;

		if(cnt>exp_timer)
			return;

	}
#elif	defined(CONFIG_ARCH_TCC892X)
	// In case of 892X, we have to add

#else  //CONFIG_ARCH_TCC92XX
	PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);

	while((pCIF->CIRQ & HwCIRQ_SOF) != HwCIRQ_SOF)
	{
		mdelay(1);
		cnt++;

		if(cnt>exp_timer)
			return;

	}
#endif
}

void CIF_OpStop(char wait_SOF, char sw_reset )
{
#if defined(CONFIG_ARCH_TCC79X)	
	if(wait_SOF)
	{
		BITSET(HwCIF->CIRQ, HwCIRQ_SOF);
		CIF_WaitFrameSync(400);
		BITSET(HwCIF->CIRQ, HwCIRQ_SOF);
	}
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF 		pCIF 		= (PCIF)tcc_p2v(HwCIF_BASE);
	#if defined(CONFIG_ARCH_TCC92XX)
	volatile PDDICONFIG pDDICfg 	= (PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCAMBUSCFG pCamBusCfg 	= (PCAMBUSCFG)tcc_p2v(HwCAMBUSCFG_BASE);
	#endif
	
	if(wait_SOF)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_SOF);
		CIF_WaitFrameSync(400);
		BITSET(pCIF->CIRQ, HwCIRQ_SOF);
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif

	CIF_ONOFF(OFF);

	if(sw_reset)
	{
#if defined(CONFIG_ARCH_TCC79X)
		BITSET(HwSWRESET, HwSWRESET_CIC_ON);
		BITCLR(HwSWRESET, HwSWRESET_CIC_ON);
#elif defined(CONFIG_ARCH_TCC92XX)
		BITSET(pDDICfg->SWRESET, HwDDIC_SWRESET_CIF);   // Reset
		BITCLR(pDDICfg->SWRESET, HwDDIC_SWRESET_CIF);  // Normal
#elif defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
		BITSET(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_CIF);   // Reset
		BITCLR(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_CIF);  // Normal
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif
	}
}
/* end of file */
