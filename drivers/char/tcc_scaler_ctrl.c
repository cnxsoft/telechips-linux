
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <mach/bsp.h>

#include <mach/tcc_scaler_ctrl.h>

extern volatile PM2MSCALER pM2MSCALER;
extern volatile PM2MSCALER pM2MSCALER1;

/****************************************************************************/
/*																			*/	
/*					M2M Scaler												*/ 
/*																			*/
/****************************************************************************/
void IO_CKC_EnableBUS_MSCL(M2M_TYPE type)
{
/*
	if(type == M2M_SCALER0)
		BITCLR(HwDDI_CONFIG->PWDN, HwDDIC_PWDN_MSCL0);
	else
		BITCLR(HwDDI_CONFIG->PWDN, HwDDIC_PWDN_MSCL1);
*/
}

void IO_CKC_DisableBUS_MSCL(M2M_TYPE type)
{
/*
	if(type == M2M_SCALER0)
		BITSET(HwDDI_CONFIG->PWDN, HwDDIC_PWDN_MSCL0);
	else
		BITSET(HwDDI_CONFIG->PWDN, HwDDIC_PWDN_MSCL1);
*/
}

void M2M_Scaler_SW_Reset(M2M_TYPE type)
{
	volatile PDDICONFIG pDDICONFIG;

	pDDICONFIG = (volatile PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);

	if(type == M2M_SCALER0)
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL0);
	}
	else
	{
		BITSET(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
		BITCLR(pDDICONFIG->SWRESET, HwDDIC_SWRESET_MSCL1);
	}
}

void MEMSCL_SET_INTERRUPT_INIT(M2M_TYPE type)
{
	volatile PPIC pPIC;
	pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

	if(type == M2M_SCALER0)
	{
		BITCLR(pPIC->MODE0, HwINT0_SC0);	// edge-triggered
		BITCLR(pPIC->MODEA0, HwINT0_SC0);	// single-edge
		BITSET(pPIC->POL0, HwINT0_SC0);		// active-low
	}
	else
	{
		BITCLR(pPIC->MODE0, HwINT0_SC1);	// edge-triggered
		BITCLR(pPIC->MODEA0, HwINT0_SC1);	// single-edge
		BITSET(pPIC->POL0, HwINT0_SC1);		// active-low
	}
}

void MEMSCL_SET_INTERRUPT(M2M_TYPE type, unsigned int uiFlag)
{
	volatile PPIC pPIC;

	pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);

	if(type == M2M_SCALER0)
	{
		if(uiFlag & SET_M2MSCL_INT_ENABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC0);	
			BITSET(pPIC->INTMSK0, HwINT0_SC0);	
		}  
		if(uiFlag & SET_M2MSCL_INT_DISABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC0);	
			BITCLR(pPIC->INTMSK0, HwINT0_SC0);
		}  
	}
	else
	{
		if(uiFlag & SET_M2MSCL_INT_ENABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC1);	
			BITSET(pPIC->INTMSK0, HwINT0_SC1);	
		}  
		if(uiFlag & SET_M2MSCL_INT_DISABLE)
		{
			BITSET(pPIC->CLR0, HwINT0_SC1);	
			BITCLR(pPIC->INTMSK0, HwINT0_SC1);
		}  
	}
}

void MEM_SCALER_SRC_ADDR(unsigned int uiMSC_SYaddr, unsigned int uiMSC_SUaddr, unsigned int uiMSC_SVaddr)
{  
	BITCSET(pM2MSCALER->SRCBASEY, HwMSC_SRC_Y_BASE_ADDR, (uiMSC_SYaddr));
	BITCSET(pM2MSCALER->SRCBASEU, HwMSC_SRC_U_BASE_ADDR, (uiMSC_SUaddr));
	BITCSET(pM2MSCALER->SRCBASEV, HwMSC_SRC_V_BASE_ADDR, (uiMSC_SVaddr));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER_DST_ADDR(unsigned int uiMSC_DYaddr, unsigned int uiMSC_DUaddr, unsigned int uiMSC_DVaddr)
{  
	BITCSET(pM2MSCALER->DSTBASEY, HwMSC_DST_Y_BASE_ADDR, (uiMSC_DYaddr));
	BITCSET(pM2MSCALER->DSTBASEU, HwMSC_DST_U_BASE_ADDR, (uiMSC_DUaddr));
	BITCSET(pM2MSCALER->DSTBASEV, HwMSC_DST_V_BASE_ADDR, (uiMSC_DVaddr));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER_SRC_SIZE(unsigned int uiSRC_HSize,unsigned int uiSRC_VSize)
{
	BITCSET(pM2MSCALER->SRCSIZE, HwMSC_SRC_SIZE_H, (uiSRC_HSize));
	BITCSET(pM2MSCALER->SRCSIZE, HwMSC_SRC_SIZE_V, (uiSRC_VSize<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER_DST_SIZE(unsigned int uiDST_HSize,unsigned int uiDST_VSize)
{
	BITCSET(pM2MSCALER->DSTSIZE, HwMSC_DST_SIZE_H, (uiDST_HSize));
	BITCSET(pM2MSCALER->DSTSIZE, HwMSC_DST_SIZE_V, (uiDST_VSize<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER_SRC_OFFSET_YC(unsigned int uiOFFSET_Y,unsigned int uiOFFSET_C )
{
	BITCSET(pM2MSCALER->SRCOFF, HwMSC_SRC_OFFSET_Y, (uiOFFSET_Y));
	BITCSET(pM2MSCALER->SRCOFF, HwMSC_SRC_OFFSET_C, (uiOFFSET_C<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER_DST_OFFSET_YC(unsigned int uiOFFSET_Y, unsigned int uiOFFSET_C )
{
	BITCSET(pM2MSCALER->DSTOFF, HwMSC_DST_OFFSET_Y, (uiOFFSET_Y));
	BITCSET(pM2MSCALER->DSTOFF, HwMSC_DST_OFFSET_C, (uiOFFSET_C<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER_SRC_CONFIG(unsigned int uiFlag, unsigned int uiInterleaved_mode, unsigned int uiInterleaved_type, unsigned int uiType)
{
	if(uiFlag & MSC_CFG_INTM)
	{
		//uiInterleaved_mode 0 : cr/cb,  1 : cb/cr                 
		BITCSET(pM2MSCALER->SRCCFG, HwMSC_SRC_CFG_INTM, (uiInterleaved_mode<<5));
	}  

	if(uiFlag & MSC_CFG_INTLV)
	{
		//uiInterleaved_type 0 : interleaved,  1 : seperate                
		BITCSET(pM2MSCALER->SRCCFG, HwMSC_SRC_CFG_INTLV, (uiInterleaved_type<<3));
	}

	if(uiFlag & MSC_CFG_TYPE)
	{
		BITCSET(pM2MSCALER->SRCCFG, HwMSC_SRC_CFG_RGB454, (uiType));   
	}
}

void MEM_SCALER_DST_CONFIG(unsigned int uiFlag, unsigned int uiCbCrWriting_mode, unsigned int uiWait, unsigned int uiReady, 
												unsigned int uiInterleaved_mode, unsigned int uiPath, unsigned int uiInterleaved_type, unsigned int uiType)
{
	if(uiFlag & MSC_CFG_COP)
	{
		//uiCbCrWriting_mode for yuv420sep => 0 : cr/cb,  1 : cb/cr                 
		BITCSET(pM2MSCALER->DSTCFG, HwMSC_DST_CFG_COP, (uiCbCrWriting_mode<<11));
	}  

	if(uiFlag & MSC_CFG_WAIT)
	{
		BITCSET(pM2MSCALER->DSTCFG, HwMSC_DST_CFG_WAIT, (uiWait<<8));
	}  

	if(uiFlag & MSC_CFG_RDY)
	{
		BITCSET(pM2MSCALER->DSTCFG, HwMSC_DST_CFG_RDY, (uiReady<<6));
	}

	if(uiFlag & MSC_CFG_INTM)
	{
		//uiInterleaved_mode 0 : cr/cb,  1 : cb/cr                 
		BITCSET(pM2MSCALER->DSTCFG, HwMSC_DST_CFG_INTM, (uiInterleaved_mode<<5));
	}  

	if(uiFlag & MSC_CFG_PATH)
	{
		BITCSET(pM2MSCALER->DSTCFG, HwMSC_DST_CFG_PATH, (uiPath<<4));
	}

	if(uiFlag & MSC_CFG_INTLV)
	{
		//uiInterleaved_type 0 : interleaved,  1 : seperate                
		BITCSET(pM2MSCALER->DSTCFG, HwMSC_DST_CFG_INTLV, (uiInterleaved_type<<3));
	}

	if(uiFlag & MSC_CFG_TYPE)
	{
		BITCSET(pM2MSCALER->DSTCFG, HwMSC_SRC_CFG_RGB454, (uiType));   
	}
}


/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER_RATIO(unsigned int uiHRatio,unsigned int uiVRatio )
{
	BITCSET(pM2MSCALER->MSCINF, HwMSC_HRATIO, (uiHRatio));
	BITCSET(pM2MSCALER->MSCINF, HwMSC_VRATIO, (uiVRatio<<16));
}
/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/
void MEM_SCALER_CTRL(unsigned int uiType)
{
	unsigned uMSCCTR = pM2MSCALER->MSCCTR;

	// 23. Rolling Operation mode Enable
	if(uiType & MSC_CTRL_ROLL_EN)
	{
		BITSET(uMSCCTR, HwMSC_REN);
	}
	if(uiType & MSC_CTRL_ROLL_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_REN);
	}

	// 22. Middle Operation mode Enable
	if(uiType & MSC_CTRL_MIDDLE_EN)
	{					
		BITSET(uMSCCTR, HwMSC_MEN);
	}
	if(uiType & MSC_CTRL_MIDDLE_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_MEN);
	}

	// 19. Release Stop mode
	// Rolling count or Middle count에서  stop된 후,  Rolling restart를 위해  set
	// 자동 value clear 된다.
	if(uiType & MSC_CTRL_ROLL_RESTR_SET)
	{
		BITSET(uMSCCTR, HwMSC_RLS);
	}

	// 17. Rolling go stop mode 
	// 사이즈가 너무 커서 여러번 rolling을 반복해야 하는 경우 bit setting한다.
	// REN 만 bit set 하면 자동으로 처음으로 돌아가지만 
	// REN + RGSM이 동시에 setting 된면 DST_ROLL_CNT 만큼 만 operation되고 
	// stop된다. RLS를 setting하면 그위치에서 다시 시작 한다.
	if(uiType & MSC_CTRL_ROLL_GOSTOP_EN)
	{
		BITSET(uMSCCTR, HwMSC_RGSM);
	}
	if(uiType & MSC_CTRL_ROLL_GOSTOP_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_RGSM);
	}

	// 16. Middle go stop mode
	if(uiType & MSC_CTRL_MIDDLE_GOSTOP_EN)
	{
		BITSET(uMSCCTR, HwMSC_MGSM);
	}
	if(uiType & MSC_CTRL_MIDDLE_GOSTOP_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_MGSM);
	}

	// 7. Rolling interrupt enable
	if(uiType & MSC_CTRL_ROLL_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_RIRQEN);
	}
	if(uiType & MSC_CTRL_ROLL_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_RIRQEN);
	}

	// 6. Middle interrupt enable
	if(uiType & MSC_CTRL_MIDDLE_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_MIRQEN);
	}
	if(uiType & MSC_CTRL_MIDDLE_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_MIRQEN);
	}

        // 5. Contiuous enable
	if(uiType & MSC_CTRL_CON_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_CON_EN);
	}
	
	if(uiType & MSC_CTRL_CON_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_CON_EN);
	}
	
	if(uiType & MSC_CTRL_RDY_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_RDY_EN);
	}
	if(uiType & MSC_CTRL_RDY_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_BUSY_EN);
	}
	if(uiType & MSC_CTRL_BUSY_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_BUSY_EN);
	}
	if(uiType & MSC_CTRL_BUSY_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_BUSY_EN);
	}
	if(uiType & MSC_CTRL_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_EN);
	}
	if(uiType & MSC_CTRL_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_EN);
	}

	pM2MSCALER->MSCCTR = uMSCCTR;
}

// 070901_hjb	TCC8222->TCC832x 

void MEM_SCALER_RM_CNT(unsigned int uiRollAddr, unsigned int uiMidAddr )
{
#if defined(CONFIG_ARCH_TCC93XX)
//
#elif defined(CONFIG_ARCH_TCC92XX)
	BITCSET(pM2MSCALER->DSTSLINE, HwMSC_DST_ROLL_CNT, (uiRollAddr<<16));
	BITCSET(pM2MSCALER->DSTSLINE, HwMSC_DST_MID_CNT, (uiMidAddr));
#else
	//
#endif

}


void MEM_SCALER_CURRENT_ROLL_CNT(unsigned int uiCRollAddr )
{
#if defined(CONFIG_ARCH_TCC93XX)
//
#elif defined(CONFIG_ARCH_TCC92XX)
	BITCSET(pM2MSCALER->DSTCLINE, HwMSC_C_R_CNT, (uiCRollAddr<<16));
#endif
}

unsigned int scaler_ended = 0;
int MEM_SCALER_POLLING_CHECK(void)
{
	unsigned int timeout_cnt = 100;
	
	scaler_ended = 0;

	while(!scaler_ended)
	{
		timeout_cnt--;
		msleep(1);
	};

	if(scaler_ended != 1)
	{
		printk("polling:: scaler busy!!!\n");
		return -1;
	}

	return 0;
}

int MEM_SCALER_POLLING_CHECK_External(void)
{
	unsigned int timeout_cnt = 50;

	while((pM2MSCALER->MSCSTR & HwMSC_STATUS_IRDY) != HwMSC_STATUS_IRDY)
	{ 
		if(timeout_cnt == 0)
		{
			printk("Scaler Timeout!!");
			return -1;
		}
		timeout_cnt--;
		msleep(2);
	};

	return 0;
}

int MEM_SCALER1_POLLING_CHECK_External(void)
{
	unsigned int timeout_cnt = 50;

	while((pM2MSCALER1->MSCSTR & HwMSC_STATUS_IRDY) != HwMSC_STATUS_IRDY)
	{ 
		if(timeout_cnt == 0)
		{
			printk("Scaler 1Timeout!!");
			return -1;
		}
		timeout_cnt--;
		msleep(2);
	};

	return 0;
}

void MEM_SCALER1_SRC_ADDR(unsigned int uiMSC_SYaddr, unsigned int uiMSC_SUaddr, unsigned int uiMSC_SVaddr)
{  
	BITCSET(pM2MSCALER1->SRCBASEY, HwMSC_SRC_Y_BASE_ADDR, (uiMSC_SYaddr));
	BITCSET(pM2MSCALER1->SRCBASEU, HwMSC_SRC_U_BASE_ADDR, (uiMSC_SUaddr));
	BITCSET(pM2MSCALER1->SRCBASEV, HwMSC_SRC_V_BASE_ADDR, (uiMSC_SVaddr));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER1_DST_ADDR(unsigned int uiMSC_DYaddr, unsigned int uiMSC_DUaddr, unsigned int uiMSC_DVaddr)
{  
	BITCSET(pM2MSCALER1->DSTBASEY, HwMSC_DST_Y_BASE_ADDR, (uiMSC_DYaddr));
	BITCSET(pM2MSCALER1->DSTBASEU, HwMSC_DST_U_BASE_ADDR, (uiMSC_DUaddr));
	BITCSET(pM2MSCALER1->DSTBASEV, HwMSC_DST_V_BASE_ADDR, (uiMSC_DVaddr));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER1_SRC_SIZE(unsigned int uiSRC_HSize,unsigned int uiSRC_VSize)
{
	BITCSET(pM2MSCALER1->SRCSIZE, HwMSC_SRC_SIZE_H, (uiSRC_HSize));
	BITCSET(pM2MSCALER1->SRCSIZE, HwMSC_SRC_SIZE_V, (uiSRC_VSize<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER1_DST_SIZE(unsigned int uiDST_HSize,unsigned int uiDST_VSize)
{
	BITCSET(pM2MSCALER1->DSTSIZE, HwMSC_DST_SIZE_H, (uiDST_HSize));
	BITCSET(pM2MSCALER1->DSTSIZE, HwMSC_DST_SIZE_V, (uiDST_VSize<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER1_SRC_OFFSET_YC(unsigned int uiOFFSET_Y,unsigned int uiOFFSET_C )
{
	BITCSET(pM2MSCALER1->SRCOFF, HwMSC_SRC_OFFSET_Y, (uiOFFSET_Y));
	BITCSET(pM2MSCALER1->SRCOFF, HwMSC_SRC_OFFSET_C, (uiOFFSET_C<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER1_DST_OFFSET_YC(unsigned int uiOFFSET_Y, unsigned int uiOFFSET_C )
{
	BITCSET(pM2MSCALER1->DSTOFF, HwMSC_DST_OFFSET_Y, (uiOFFSET_Y));
	BITCSET(pM2MSCALER1->DSTOFF, HwMSC_DST_OFFSET_C, (uiOFFSET_C<<16));
}

/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER1_SRC_CONFIG(unsigned int uiFlag, unsigned int uiInterleaved_mode, unsigned int uiInterleaved_type, unsigned int uiType)
{
	if(uiFlag & MSC_CFG_INTM)
	{
		//uiInterleaved_mode 0 : cr/cb,  1 : cb/cr                 
		BITCSET(pM2MSCALER1->SRCCFG, HwMSC_SRC_CFG_INTM, (uiInterleaved_mode<<5));
	}  

	if(uiFlag & MSC_CFG_INTLV)
	{
		//uiInterleaved_type 0 : interleaved,  1 : seperate                
		BITCSET(pM2MSCALER1->SRCCFG, HwMSC_SRC_CFG_INTLV, (uiInterleaved_type<<3));
	}

	if(uiFlag & MSC_CFG_TYPE)
	{
		BITCSET(pM2MSCALER1->SRCCFG, HwMSC_SRC_CFG_RGB454, (uiType));   
	}
}

void MEM_SCALER1_SRC_CONFIG1
(
	unsigned int uiFlag, 
	unsigned int uiARGB, 
	unsigned int uiImode, 
	unsigned int uiOmode, 
	unsigned int uiMSBF
)
{
	PM2MSCALER pM2MScaler = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);

	if(uiFlag & MSC_CFG_ARGB){
		BITCSET(pM2MScaler->SRCCFG, Hw16, (uiARGB<<16));
	}
	if(uiFlag & MSC_CFG_IMODE){
		BITCSET(pM2MScaler->SRCCFG, Hw17|Hw18, (uiImode<<17));
	}
	if(uiFlag & MSC_CFG_OMODE){
		BITCSET(pM2MScaler->SRCCFG, Hw19|Hw20, (uiOmode<<19));
	}
	if(uiFlag & MSC_CFG_MSBF){
		BITCSET(pM2MScaler->SRCCFG, Hw31, (uiMSBF<<31));
	}
}

void MEM_SCALER1_DST_CONFIG(unsigned int uiFlag, unsigned int uiCbCrWriting_mode, unsigned int uiWait, unsigned int uiReady, 
												unsigned int uiInterleaved_mode, unsigned int uiPath, unsigned int uiInterleaved_type, unsigned int uiType)
{
	if(uiFlag & MSC_CFG_COP)
	{
		//uiCbCrWriting_mode for yuv420sep => 0 : cr/cb,  1 : cb/cr                 
		BITCSET(pM2MSCALER1->DSTCFG, HwMSC_DST_CFG_COP, (uiCbCrWriting_mode<<11));
	}  

	if(uiFlag & MSC_CFG_WAIT)
	{
		BITCSET(pM2MSCALER1->DSTCFG, HwMSC_DST_CFG_WAIT, (uiWait<<8));
	}  

	if(uiFlag & MSC_CFG_RDY)
	{
		BITCSET(pM2MSCALER1->DSTCFG, HwMSC_DST_CFG_RDY, (uiReady<<6));
	}

	if(uiFlag & MSC_CFG_INTM)
	{
		//uiInterleaved_mode 0 : cr/cb,  1 : cb/cr                 
		BITCSET(pM2MSCALER1->DSTCFG, HwMSC_DST_CFG_INTM, (uiInterleaved_mode<<5));
	}  

	if(uiFlag & MSC_CFG_PATH)
	{
		BITCSET(pM2MSCALER1->DSTCFG, HwMSC_DST_CFG_PATH, (uiPath<<4));
	}

	if(uiFlag & MSC_CFG_INTLV)
	{
		//uiInterleaved_type 0 : interleaved,  1 : seperate                
		BITCSET(pM2MSCALER1->DSTCFG, HwMSC_DST_CFG_INTLV, (uiInterleaved_type<<3));
	}

	if(uiFlag & MSC_CFG_TYPE)
	{
		BITCSET(pM2MSCALER1->DSTCFG, HwMSC_SRC_CFG_RGB454, (uiType));   
	}
}


/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/

void MEM_SCALER1_RATIO(unsigned int uiHRatio,unsigned int uiVRatio )
{
	BITCSET(pM2MSCALER1->MSCINF, HwMSC_HRATIO, (uiHRatio));
	BITCSET(pM2MSCALER1->MSCINF, HwMSC_VRATIO, (uiVRatio<<16));
}
/**********************************************************
*
*  Function of
*
*
*
*  Input
*  Output
*  Return
*
*  Description
**********************************************************/
void MEM_SCALER1_CTRL(unsigned int uiType)
{
	unsigned uMSCCTR = pM2MSCALER1->MSCCTR;


	// 24. HwMSC_INPTH Operation mode Enable
	if(uiType & MSC_CTRL_INPAHT_EN)
	{
		BITSET(uMSCCTR, HwMSC_INPTH);
	}
	if(uiType & MSC_CTRL_INPAHT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_INPTH);
	}
	
	// 23. Rolling Operation mode Enable
	if(uiType & MSC_CTRL_ROLL_EN)
	{
		BITSET(uMSCCTR, HwMSC_REN);
	}
	if(uiType & MSC_CTRL_ROLL_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_REN);
	}

	// 22. Middle Operation mode Enable
	if(uiType & MSC_CTRL_MIDDLE_EN)
	{					
		BITSET(uMSCCTR, HwMSC_MEN);
	}
	if(uiType & MSC_CTRL_MIDDLE_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_MEN);
	}

	// 19. Release Stop mode
	// Rolling count or Middle count에서  stop된 후,  Rolling restart를 위해  set
	// 자동 value clear 된다.
	if(uiType & MSC_CTRL_ROLL_RESTR_SET)
	{
		BITSET(uMSCCTR, HwMSC_RLS);
	}

	// 17. Rolling go stop mode 
	// 사이즈가 너무 커서 여러번 rolling을 반복해야 하는 경우 bit setting한다.
	// REN 만 bit set 하면 자동으로 처음으로 돌아가지만 
	// REN + RGSM이 동시에 setting 된면 DST_ROLL_CNT 만큼 만 operation되고 
	// stop된다. RLS를 setting하면 그위치에서 다시 시작 한다.
	if(uiType & MSC_CTRL_ROLL_GOSTOP_EN)
	{
		BITSET(uMSCCTR, HwMSC_RGSM);
	}
	if(uiType & MSC_CTRL_ROLL_GOSTOP_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_RGSM);
	}

	// 16. Middle go stop mode
	if(uiType & MSC_CTRL_MIDDLE_GOSTOP_EN)
	{
		BITSET(uMSCCTR, HwMSC_MGSM);
	}
	if(uiType & MSC_CTRL_MIDDLE_GOSTOP_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_MGSM);
	}

	// 7. Rolling interrupt enable
	if(uiType & MSC_CTRL_ROLL_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_RIRQEN);
	}
	if(uiType & MSC_CTRL_ROLL_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_RIRQEN);
	}

	// 6. Middle interrupt enable
	if(uiType & MSC_CTRL_MIDDLE_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_MIRQEN);
	}
	if(uiType & MSC_CTRL_MIDDLE_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_MIRQEN);
	}
        // 5. Contiuous enable
	if(uiType & MSC_CTRL_CON_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_CON_EN);
	}
	
	if(uiType & MSC_CTRL_CON_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_CON_EN);
	}

	
	if(uiType & MSC_CTRL_RDY_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_RDY_EN);
	}
	if(uiType & MSC_CTRL_RDY_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_BUSY_EN);
	}
	if(uiType & MSC_CTRL_BUSY_INT_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_BUSY_EN);
	}
	if(uiType & MSC_CTRL_BUSY_INT_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_BUSY_EN);
	}
	if(uiType & MSC_CTRL_EN)
	{
		BITSET(uMSCCTR, HwMSC_CTRL_EN);
	}
	if(uiType & MSC_CTRL_DIS)
	{
		BITCLR(uMSCCTR, HwMSC_CTRL_EN);
	}

	pM2MSCALER1->MSCCTR = uMSCCTR;

}


void MEM_SCALER1_RM_CNT(unsigned int uiRollAddr, unsigned int uiMidAddr )
{
#if defined(CONFIG_ARCH_TCC93XX)
//
#else
	BITCSET(pM2MSCALER1->DSTSLINE, HwMSC_DST_ROLL_CNT, (uiRollAddr<<16));
	BITCSET(pM2MSCALER1->DSTSLINE, HwMSC_DST_MID_CNT, (uiMidAddr));
#endif
}


void MEM_SCALER1_CURRENT_ROLL_CNT(unsigned int uiCRollAddr )
{
#if defined(CONFIG_ARCH_TCC93XX)
//
#else
	BITCSET(pM2MSCALER1->DSTCLINE, HwMSC_C_R_CNT, (uiCRollAddr<<16));
#endif
}

unsigned int scaler1_ended = 0;
int MEM_SCALER1_POLLING_CHECK(void)
{
	unsigned int timeout_cnt = 100;

	scaler1_ended = 0;

	while(!scaler1_ended)
	{
		timeout_cnt--;
		msleep(1);
	};

	if(scaler1_ended != 1)
	{
		printk("polling:: scaler busy!!!\n");
		return -1;
	}

	return 0;
}

unsigned int DEV_M2M_Wait_signal_disable(char scaler)
{
	unsigned int loop = 0, reg = 0;
	PM2MSCALER pM2MSCALER;
	PPMU pPMU = (PPMU)tcc_p2v(HwPMU_BASE); 
	PDDICONFIG pDDI_CONFIG = (PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE); 


	#ifdef CONFIG_ARCH_TCC93XX
	if(tca_ckc_getpmupwroff(PMU_DDIBUS) == 1)
		return FALSE;
	#elif CONFIG_ARCH_TCC88XX
		// to do 
	#else
	if(ISSET(pPMU->PWROFF, HwPMU_PWROFF_DB))
		return FALSE;
	#endif//

	if(scaler == 0)
	{

		if(ISSET(pDDI_CONFIG->PWDN, HwDDIC_PWDN_MSCL0))
			return FALSE;

		pM2MSCALER = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER0_BASE);
	}
	else
	{
		if(ISSET(pDDI_CONFIG->PWDN, HwDDIC_PWDN_MSCL1))
			return FALSE;

		pM2MSCALER = (volatile PM2MSCALER)tcc_p2v(HwM2MSCALER1_BASE);
	}

	loop = 1;

	#if defined(CONFIG_MACH_TCC9300ST)
	while(ISSET(pM2MSCALER->MSCSTR, HwMSC_STATUS_BUSY) || ISZERO(pM2MSCALER->MSCSTR, HwMSC_STATUS_RDY))
	#else
	while(ISSET(pM2MSCALER->MSCSTR, HwMSC_STATUS_BUSY))
	#endif
	{
		loop++;
		if(loop > M2M_WAIT_MAX)
			break;
	}
	
	return loop;
}
void scale_get_address(SCALER_DATA_FM format, unsigned int base_Yaddr, unsigned int  src_imgx, unsigned int  src_imgy, unsigned int start_x, unsigned int start_y,
							unsigned int* Y, unsigned int* U,unsigned int* V)
{
	unsigned int Uaddr, Vaddr, Yoffset, UVoffset, start_yPos;

	start_yPos = (start_y>>1)<<1;
	Yoffset = (src_imgx * start_yPos) + start_x;

	if((format == SCALER_YUV422_sq0) || (format == SCALER_YUV422_sq1))
		Yoffset = 2*Yoffset;

	*Y = base_Yaddr + Yoffset;

	if(*U == 0 && *V == 0)
	{
		Uaddr = GET_ADDR_YUV42X_spU(base_Yaddr, src_imgx, src_imgy);

		if(format == SCALER_YUV420_sp)
		{
			Vaddr = GET_ADDR_YUV420_spV(Uaddr, src_imgx, src_imgy);
		}
		else
		{
			Vaddr = GET_ADDR_YUV422_spV(Uaddr, src_imgx, src_imgy);
		}
	}
	else
	{
		Uaddr = *U;
		Vaddr = *V;
	}

	if(format == SCALER_YUV420_sp || format == SCALER_YUV420_inter)
	{
		if(format == SCALER_YUV420_inter)
			UVoffset = ((src_imgx * start_yPos)/2 + start_x);
		else
			UVoffset = ((src_imgx * start_yPos)/4 + start_x/2);
	}
	else
	{
		if(format == SCALER_YUV422_inter)
			UVoffset = ((src_imgx * start_yPos) + start_x);
		else
			UVoffset = ((src_imgx * start_yPos)/2 + start_x/2);
	}
	
	*U = Uaddr + UVoffset;
	*V = Vaddr + UVoffset;
}

char scaler_image_converter(SCALER_DATA_FM UiFormat, unsigned int *DrvierForamt, int *bpp)
{
	switch(UiFormat)
	{
		case SCALER_YUV422_sq0:
			*DrvierForamt = HwMSC_SRC_CFG_YUV422_SEQ0;
			*bpp = 1;
			break;
		case SCALER_YUV422_sq1:
			*DrvierForamt = HwMSC_SRC_CFG_YUV422_SEQ1;
			*bpp = 1;
			break;
		case SCALER_YUV422_inter:
		case SCALER_YUV422_sp:
			*DrvierForamt = HwMSC_SRC_CFG_YUV422_SEP;
			*bpp = 1;
			break;
		case SCALER_YUV420_inter:
		case SCALER_YUV420_sp:
			*DrvierForamt = HwMSC_SRC_CFG_YUV420_SEP;
			*bpp = 1;
			break;
		case SCALER_RGB565:
			*DrvierForamt = HwMSC_SRC_CFG_RGB565;
			*bpp = 2;
			break;
		case SCALER_RGB555:
			*DrvierForamt = HwMSC_SRC_CFG_RGB555;
			*bpp = 2;
			break;
		case SCALER_RGB454:
			*DrvierForamt = HwMSC_SRC_CFG_RGB454;
			*bpp = 2;
			break;
		case SCALER_RGB444:		
			*DrvierForamt = HwMSC_SRC_CFG_RGB444;
			*bpp = 2;
			break;

		default:
			return 0;
	}

	return 1;
}

