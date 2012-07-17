/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : CAMERA INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCCXXX
*    SOURCE        : tdd_cif.c
*
*    START DATE    : 2008. 4. 17.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3-2 TEAM
*                : TELECHIPS, INC.
************************************************************************/
#include <linux/delay.h>
#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/io.h>

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#include "sensor_if.h"
#include "cam.h"
#include "cam_reg.h"
#include "tdd_cif.h"
#include <asm/mach-types.h>


void TCC_CIF_CLOCK_Get(void)
{
	CIF_Clock_Get();
}

void TCC_CIF_CLOCK_Put(void)
{
	CIF_Clock_Put();
}

void TDD_CIF_Initialize()
{
#if defined(CONFIG_ARCH_TCC92XX)
	//GPIO Functional setting!!
	BITCSET(HwGPIOE->GPFN1, 0xFFFF0000, 0x11110000);
	BITCSET(HwGPIOE->GPFN2, 0xFFFFFFFF, 0x11111111);

	// Set Pull-Up/Down Control
	//BITSET( HwGPIOE->GPPD0, 0xAA000000); // D0~D3
	//BITSET( HwGPIOE->GPPD1, 0x000028AA); // D4~D7, VS, HS

	// Change to Driving Strength that GPIO_E
	BITSET(HwGPIOE->GPCD0, 0xFF000000);
	BITSET(HwGPIOE->GPCD1, 0x0000FFFF);
#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PGPION pGPIO_E = (PGPION)tcc_p2v(HwGPIOE_BASE);
	volatile PGPION pGPIO_G = (PGPION)tcc_p2v(HwGPIOG_BASE);

	//GPIO Functional setting!!
	#if defined(CONFIG_USE_ISP)
		#if defined(CONFIG_ARCH_TCC93XX)
			BITCSET(pGPIO_E->GPFN1, 0xFFFFFFFF, 0x11113333);
		#else //CONFIG_ARCH_TCC93XX
			BITCSET(pGPIO_E->GPFN1, 0xFFFFFFFF, 0x00005555);
		#endif // CONFIG_ARCH_TCC93XX
	#else // CONFIG_USE_ISP
		BITCSET(pGPIO_E->GPFN1, 0xFFFF0000, 0x11110000);
	#endif // CONFIG_USE_ISP
	BITCSET(pGPIO_E->GPFN2, 0xFFFFFFFF, 0x11111111);

	if(machine_is_tcc8800() && (system_rev == 0x0614  || system_rev == 0x0622 || system_rev == 0x0624)){
		//printk("for tcc8803 to General GPIO \n");
		BITCSET(pGPIO_E->GPFN0, 0x0000FF00, 0x00000000);  // E2(5m reset), E3(5m powerdown)
		BITCSET(pGPIO_G->GPFN0, 0x0000FF00, 0x00000000);  // G2(1.3m reset), G3(1.3m powerdown) 
	}

	// Disable Pull Up
	BITSET( pGPIO_E->GPPD0, 0x00000000); // D0~D3+			
	BITSET( pGPIO_E->GPPD1, 0x00000000); // D4~D7, VS, HS

	// Change to Driving Strength that GPIO_E
	BITSET(pGPIO_E->GPCD0, 0xFF000000);
	BITSET(pGPIO_E->GPCD1, 0x000000FF);
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of CAM4
	volatile PGPION pGPIO_F = (PGPION)tcc_p2v(HwGPIOF_BASE);
	volatile PGPION pGPIO_B = (PGPION)tcc_p2v(HwGPIOB_BASE);
	volatile PGPION pGPIO_C = (PGPION)tcc_p2v(HwGPIOC_BASE);
	volatile PGPION pGPIO_D = (PGPION)tcc_p2v(HwGPIOD_BASE);
	volatile PGPION pGPIO_G = (PGPION)tcc_p2v(HwGPIOG_BASE);

	#if defined(CONFIG_MACH_M805_892X)
		printk("m805_892x port configuration!!\n");
		// Change to Functional GPIO that GPIO_D
		BITCSET(pGPIO_F->GPFN0.nREG, 0xFFFFFFFF, 0x11111111); 
		#if defined(CONFIG_M805S_8925_0XX)
			BITCSET(pGPIO_F->GPFN1.nREG, 0x000FFFFF, 0x0001B111);
		#else
			BITCSET(pGPIO_F->GPFN1.nREG, 0x0000FFFF, 0x0000B111);
		#endif
	#else
		if(system_rev == 0x1005 || system_rev == 0x1007){
			BITCSET(pGPIO_F->GPEN.nREG, 0x4100FFFF, 0x4100FFFF);
			
			// Change to Functional GPIO that GPIO_D
			BITCSET(pGPIO_F->GPFN0.nREG, 0xFFFFFFFF, 0x11111111);  
			BITCSET(pGPIO_F->GPFN1.nREG, 0xFFFFFFFF, 0x0001B111);

			BITCSET(pGPIO_F->GPFN3.nREG, 0x0F00000F, 0x00000000);	// FL_EN(30), Power(24)
			printk("0x1005 tcc892x port configuration!!\n");
		}
		else if(system_rev == 0x1006){

			// Change to Functional GPIO that GPIO_D
			BITCSET(pGPIO_F->GPFN0.nREG, 0xFFFFFFFF, 0x11111111);  
			BITCSET(pGPIO_F->GPFN1.nREG, 0xF00FFFFF, 0x0001B111);

			BITCSET(pGPIO_D->GPFN0.nREG, 0x00000FF0, 0x00000000);	// PWDN, 5M Reset, D[1] , STDBY, 1.3M PWDN, D[2]
			BITCSET(pGPIO_G->GPFN0.nREG, 0x000000F0, 0x00000000);	// FL_EN, 5M PWDN, G[1] 
		}else if(system_rev == 0x1008){

			// Change to Functional GPIO that GPIO_D
			BITCSET(pGPIO_F->GPFN0.nREG, 0xFFFFFFFF, 0x11111111);  
			BITCSET(pGPIO_F->GPFN1.nREG, 0x000FFFFF, 0x00011111);

			BITCSET(pGPIO_B->GPFN3.nREG, 0x00F00000, 0x00C00000);	// CKO B[29]

			BITCSET(pGPIO_C->GPFN0.nREG, 0x00FF0000, 0x00000000);	// 1.3M Reset, C[4] , 1.3M STDBY C[5]
			BITCSET(pGPIO_C->GPFN1.nREG, 0x0000000F, 0x00000000);	// 5M RST, C[8] 
			BITCSET(pGPIO_C->GPFN3.nREG, 0x000F0000, 0x00000000);	// 5M STDBY c[28]			
		}
		else{
			// Change to Functional GPIO that GPIO_D
			BITCSET(pGPIO_F->GPFN0.nREG, 0xFFFFFFFF, 0x11111111);  
			BITCSET(pGPIO_F->GPFN1.nREG, 0x0000FFFF, 0x0001B111);

			BITCSET(pGPIO_C->GPFN3.nREG, 0x00F00000, 0x00000000); // Temporary, C29 to GPIO (FL_EN), 5M PWDN
			BITCSET(pGPIO_D->GPFN2.nREG, 0x0000000F, 0x00000000); // Temporary, D16 to GPIO (STDBY), 1.3M PWDN
		}
		
	#endif
		// Change to Driving Strength that GPIO_D
		BITCSET(pGPIO_F->GPCD0.nREG, 0x0FFFFFFF, 0x03FFFFFF);
	
	
#else // CONFIG_ARCH_TCC79X
	unsigned int	hpcsn= 0x03; // PORTCFG13 HPCSN Field. value 3 is SD_D1(1)
	
	HwPORTCFG11 &= ~0x01;
	HwPORTCFG12 = 0x00;
	//HwPORTCFG13 &= ~0x11100000;//HwPORTCFG13 = 0x00;
	HwPORTCFG13 = 0x00000000 | hpcsn;//HwPORTCFG13 = 0x00;	//MCC  //PORTCFG 13  4bit(LSB) is Write Only.
	//HwPORTCFG3 |= 0x100; //pwdn
	//HwPORTCFG10 |= (0x01<<8); //Reset
#endif
}

void TDD_CIF_Termination(void)
{
#if	defined(CONFIG_ARCH_TCC892X)
	//	In case of CAM4
	volatile PGPION pGPIO_F = (PGPION)tcc_p2v(HwGPIOF_BASE);

	// Change to Functional GPIO that GPIO_D
	BITCSET(pGPIO_F->GPFN0.nREG, 0xFFFFFFFF, 0x00000000);  
	BITCSET(pGPIO_F->GPFN1.nREG, 0x0000FFFF, 0x00000000);
	
#else
	volatile PGPION pGPIO_E = (PGPION)tcc_p2v(HwGPIOE_BASE);

	// Change GPIO Setting From Functional Camera To General GPIO
	BITCSET(pGPIO_E->GPFN1, 0xFFFF0000, 0x00000000);
	BITCSET(pGPIO_E->GPFN2, 0xFFFFFFFF, 0x00000000);

	// GPIO Input High Setting!
	BITCSET(pGPIO_E->GPEN, 0x00FFFF00, 0x00000000);

	// Enable Pull Down in Camera 
	BITSET( pGPIO_E->GPPD0, 0xAA000000); // D0~D3+			
	BITSET( pGPIO_E->GPPD1, 0x0000AAAA); // D4~D7, VS, HS
#endif

}

void TDD_CIF_ONOFF(unsigned int uiOnOff)
{
	CIF_ONOFF(uiOnOff);
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setInfo
//
//	DESCRIPTION
//    		set CIF register  : 	HwICPCR1
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetInfo(unsigned int uiFlag, unsigned int uiBypass, unsigned int uiBypassBusSel,
                        unsigned int uiColorPattern, unsigned int uiPatternFormat, unsigned int uiRGBMode,
                        unsigned int uiRGBBitMode, unsigned int uiColorSequence, unsigned int uiBusOrder)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);

	// 1.    HwICPCR1_BP        Hw15         // Bypass 
	if(uiFlag & SET_CIF_BYPASS_MODE)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_BP, (uiBypass << 15));
	}
	
	// 2.   HwICPCR1_BBS_LSB8      Hw14         // When bypass 16bits mode, LSB 8bits are stored in first
	if(uiFlag & SET_CIF_BYPASS_BUS)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_BBS_LSB8, (uiBypassBusSel << 14));
	}
	
	// 3.   HwICPCR1_CP_RGB        Hw12         // RGB(555,565,bayer) color pattern
	if(uiFlag & SET_CIF_COLOR_PATTERN)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_CP_RGB, (uiColorPattern << 12));
	}
	
	// 4.
	// HwICPCR1_PF_444        HwZERO       // 4:4:4 format
	// HwICPCR1_PF_422        Hw10         // 4:2:2 format
	// HwICPCR1_PF_420        Hw11         // 4:2:0 format or RGB(555,565,bayer) mode
	if(uiFlag & SET_CIF_PATTERN_FORMAT)
	{
		BITCSET(pCIF->ICPCR1, (HwICPCR1_PF_420|HwICPCR1_PF_422), (uiPatternFormat << 10));
	}
	
	// 5.
	// HwICPCR1_RGBM_BAYER      HwZERO       // Bayer RGB Mode
	// HwICPCR1_RGBM_RGB555            Hw8          // RGB555 Mode
	// HwICPCR1_RGBM_RGB565            Hw9          // RGB565 Mode
	if(uiFlag & SET_CIF_RGB_MODE)
	{
		BITCSET(pCIF->ICPCR1, (HwICPCR1_RGBM_RGB555|HwICPCR1_RGBM_RGB565), (uiRGBMode << 8));
	}

	// 6.
	// HwICPCR1_RGBBM_16      HwZERO       // 16bit mode
	// HwICPCR1_RGBBM_8DISYNC                  Hw6          // 8bit disable sync
	// HwICPCR1_RGBBM_8      Hw7          // 8bit mode
	if(uiFlag & SET_CIF_RGBBIT_MODE)
	{
		BITCSET(pCIF->ICPCR1, (HwICPCR1_RGBBM_8DISYNC|HwICPCR1_RGBBM_8), (uiRGBBitMode << 6));
	}
	
	// 7.  CS
	// HwICPCR1_CS_RGBMG      HwZERO       // 555RGB:RGB(MG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:BG->GR, CCIR656:YCbYCr
	// HwICPCR1_CS_RGBLG      Hw4          // 555RGB:RGB(LG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:GR->BG, CCIR656:YCrYCb
	// HwICPCR1_CS_BGRMG      Hw5          // 555RGB:BGR(MG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:RG->GB, CCIR656:CbYCrY
	// HwICPCR1_CS_BGRLG      (Hw5|Hw4)    // 555RGB:BGR(LG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:GB->RG, CCIR656:CrYCbY
	if(uiFlag & SET_CIF_COLOR_SEQUENCE)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_CS_BGRLG, (uiColorSequence << 4));
	}
	
	// 8.   HwICPCR1_BO_SW        Hw2          // Switch the MSB/LSB 8bit Bus
	if(uiFlag & SET_CIF_BUS_ORDER)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_BO_SW, (uiBusOrder << 2));
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else  // (CONFIG_ARCH_TCC79X)
	// 1.    HwICPCR1_BP        Hw15         // Bypass 
	if(uiFlag & SET_CIF_BYPASS_MODE)
	{
		BITCSET(HwICPCR1, HwICPCR1_BP, (uiBypass << 15));
	}
	
	// 2.   HwICPCR1_BBS_LSB8      Hw14         // When bypass 16bits mode, LSB 8bits are stored in first
	if(uiFlag & SET_CIF_BYPASS_BUS)
	{
		BITCSET(HwICPCR1, HwICPCR1_BBS_LSB8, (uiBypassBusSel << 14));
	}
	
	// 3.   HwICPCR1_CP_RGB        Hw12         // RGB(555,565,bayer) color pattern
	if(uiFlag & SET_CIF_COLOR_PATTERN)
	{
		BITCSET(HwICPCR1, HwICPCR1_CP_RGB, (uiColorPattern << 12));
	}
	
	// 4.
	// HwICPCR1_PF_444        HwZERO       // 4:4:4 format
	// HwICPCR1_PF_422        Hw10         // 4:2:2 format
	// HwICPCR1_PF_420        Hw11         // 4:2:0 format or RGB(555,565,bayer) mode
	if(uiFlag & SET_CIF_PATTERN_FORMAT)
	{
		BITCSET(HwICPCR1, (HwICPCR1_PF_420|HwICPCR1_PF_422), (uiPatternFormat << 10));
	}
	
	// 5.
	// HwICPCR1_RGBM_BAYER      HwZERO       // Bayer RGB Mode
	// HwICPCR1_RGBM_RGB555            Hw8          // RGB555 Mode
	// HwICPCR1_RGBM_RGB565            Hw9          // RGB565 Mode
	if(uiFlag & SET_CIF_RGB_MODE)
	{
		BITCSET(HwICPCR1, (HwICPCR1_RGBM_RGB555|HwICPCR1_RGBM_RGB565), (uiRGBMode << 8));
	}
	
	// 6.
	// HwICPCR1_RGBBM_16      HwZERO       // 16bit mode
	// HwICPCR1_RGBBM_8DISYNC                  Hw6          // 8bit disable sync
	// HwICPCR1_RGBBM_8      Hw7          // 8bit mode
	if(uiFlag & SET_CIF_RGBBIT_MODE)
	{
		BITCSET(HwICPCR1, (HwICPCR1_RGBBM_8DISYNC|HwICPCR1_RGBBM_8), (uiRGBBitMode << 6));
	}
	
	// 7.  CS
	// HwICPCR1_CS_RGBMG      HwZERO       // 555RGB:RGB(MG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:BG->GR, CCIR656:YCbYCr
	// HwICPCR1_CS_RGBLG      Hw4          // 555RGB:RGB(LG), 565RGB:RGB, 444/422/420:R/Cb/U first, Bayer RGB:GR->BG, CCIR656:YCrYCb
	// HwICPCR1_CS_BGRMG      Hw5          // 555RGB:BGR(MG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:RG->GB, CCIR656:CbYCrY
	// HwICPCR1_CS_BGRLG      (Hw5|Hw4)    // 555RGB:BGR(LG), 565RGB:BGR, 444/422/420:B/Cr/V first, Bayer RGB:GB->RG, CCIR656:CrYCbY
	if(uiFlag & SET_CIF_COLOR_SEQUENCE)
	{
		BITCSET(HwICPCR1, HwICPCR1_CS_BGRLG, (uiColorSequence << 4));
	}
	
	// 8.   HwICPCR1_BO_SW        Hw2          // Switch the MSB/LSB 8bit Bus
	if(uiFlag & SET_CIF_BUS_ORDER)
	{
		BITCSET(HwICPCR1, HwICPCR1_BO_SW, (uiBusOrder << 2));
	}

#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setCtrl
//
//	DESCRIPTION
//    		set CIF register  : 	HwICPCR1
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetCtrl(unsigned int uiFlag, unsigned int uiPWDN, unsigned int uiBypass_Scaler,
						unsigned int uiPXCLK_POL, unsigned int uiSKPF, unsigned int uiM420_FC, unsigned int uiC656,
						bool bEnableOfATV)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	// 1.  HwICPCR1_PWD      Power down mode in camera >> 0:Disable, 1:Power down mode , This power down mode is connected the PWDN of camera sensor  
	if(uiFlag & SET_CIF_PWDN)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_PWD, (uiPWDN << 30));
	}	

	// 2.  HwICPCR1_BPS                            Hw23         // Bypass Scaler >> 0:Non, 1:Bypass    //BP_SCA
	if(uiFlag & SET_CIF_BYPASS_SCALER)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_BPS, (uiBypass_Scaler << 23));
	}
	
	// 3.  HwICPCR1_POL                            Hw21         // PXCLK Polarity >> 0:Positive edge, 1:Negative edge
	if(uiFlag & SET_CIF_PXCLK_POL)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_POL, (uiPXCLK_POL << 21));
	}
	
	// 4.  HwICPCR1_SKPF                                        // Skip Frame >> 0~7 #Frames skips   [20:18]
	if(uiFlag & SET_CIF_SKPF)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_SKPF, (uiSKPF << 18));
	}
	
	// 5.
	//  HwICPCR1_M420_ZERO                      HwZERO        // Format Convert (YUV422->YUV420) , Not-Convert
	//  HwICPCR1_M420_ODD                       Hw17             // converted in odd line skip   // 10
	//  HwICPCR1_M420_EVEN                      (Hw17|Hw16)  // converted in even line skip  //11
	if(uiFlag & SET_CIF_M42_FC)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_M420_EVEN, (uiM420_FC << 16));
	}
	
	// 7.  #define HwICPCR1_C656                           Hw13         // Convert 656 format 0:Disable, 1:Enable  
	if(uiFlag & SET_CIF_C656)
	{
		BITCSET(pCIF->ICPCR1, HwICPCR1_C656, (uiC656 << 13)); // PCIF->ICPCR1
	}

	// 8.  #define SET_CIF_ATV				                Hw13         // Convert 656 format 0:Disable, 1:Enable
	if(uiFlag & SET_CIF_ATV)
	{
			if(bEnableOfATV)
				BITCSET(pCIF->ICPCR1, 0xFFFFFFFF, HwICPCR1_ON  | HwICPCR1_TV| HwICPCR1_TI | HwICPCR1_UF // 
				| HwICPCR1_BPS | HwICPCR1_C656 | HwICPCR1_PF_422 | HwICPCR1_BO_SW | HwICPCR1_HSP_HIGH); // 0xB8802406 //
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)	
	// 1.  HwICPCR1_PWD      Power down mode in camera >> 0:Disable, 1:Power down mode , This power down mode is connected the PWDN of camera sensor  
	if(uiFlag & SET_CIF_PWDN)
	{
		BITCSET(HwICPCR1, HwICPCR1_PWD, (uiPWDN << 30));
	}
	
	// 2.  HwICPCR1_BPS                            Hw23         // Bypass Scaler >> 0:Non, 1:Bypass    //BP_SCA
	if(uiFlag & SET_CIF_BYPASS_SCALER)
	{
		BITCSET(HwICPCR1, HwICPCR1_BPS, (uiBypass_Scaler << 23));
	}
	
	// 3.  HwICPCR1_POL                            Hw21         // PXCLK Polarity >> 0:Positive edge, 1:Negative edge
	if(uiFlag & SET_CIF_PXCLK_POL)
	{
		BITCSET(HwICPCR1, HwICPCR1_POL, (uiPXCLK_POL << 21));
	}
	
	// 4.  HwICPCR1_SKPF                                        // Skip Frame >> 0~7 #Frames skips   [20:18]
	if(uiFlag & SET_CIF_SKPF)
	{
		BITCSET(HwICPCR1, HwICPCR1_SKPF, (uiSKPF << 18));
	}
	
	// 5.
	//  HwICPCR1_M420_ZERO                      HwZERO        // Format Convert (YUV422->YUV420) , Not-Convert
	//  HwICPCR1_M420_ODD                       Hw17             // converted in odd line skip   // 10
	//  HwICPCR1_M420_EVEN                      (Hw17|Hw16)  // converted in even line skip  //11
	if(uiFlag & SET_CIF_M42_FC)
	{
	    BITCSET(HwICPCR1, HwICPCR1_M420_EVEN_SKIP, (uiM420_FC << 16));
	}
	
	// 7.  #define HwICPCR1_C656                           Hw13         // Convert 656 format 0:Disable, 1:Enable  
	if(uiFlag & SET_CIF_C656)
	{
			BITCSET(HwICPCR1, HwICPCR1_C656, (uiC656 << 13)); // PCIF->ICPCR1
	}
#endif
}	

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setTransfer
//
//	DESCRIPTION
//			CIF transef mode setting
//    		set CIF register  : 	HwCDCR1
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetTransfer(unsigned int uiFlag, unsigned int uiBurst, unsigned int uiLock, unsigned int uiTransMode)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	//HwCDCR1_TM_INC        Hw3                    // INC Transfer
	if(uiFlag & SET_CIF_TRANSFER_MODE)
	{
		BITCSET(pCIF->CDCR1, HwCDCR1_TM_INC, (uiTransMode << 3));
	}

	// HwCDCR1_LOCK_ON        Hw2                    // Lock Transfer
	if(uiFlag & SET_CIF_TRANSFER_LOCK)
	{
		BITCSET(pCIF->CDCR1, HwCDCR1_LOCK_ON, (uiLock << 2));
	}
	
	// HwCDCR1_BS_1    HwZERO      // The DMA transfers the image data as 1 word to memory
	// HwCDCR1_BS_2    Hw0            // The DMA transfers the image data as 2 word to memory
	// HwCDCR1_BS_4    Hw1            // The DMA transfers the image data as 4 word to memory
	// HwCDCR1_BS_8    (Hw1|Hw0)  // The DMA transfers the image data as 8 word to memory (default)
	if(uiFlag & SET_CIF_TRANSFER_BURST)
	{
		BITCSET(pCIF->CDCR1, HwCDCR1_BS_8, uiBurst); // PCIF->CDCR1
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.

#else // (CONFIG_ARCH_TCC79X)
	//HwCDCR1_TM_INC        Hw3                    // INC Transfer
	if(uiFlag & SET_CIF_TRANSFER_MODE)
	{
		BITCSET(HwCDCR1, HwCDCR1_TM_INC, (uiTransMode << 3));
	}
	
	// HwCDCR1_LOCK_ON        Hw2                    // Lock Transfer
	if(uiFlag & SET_CIF_TRANSFER_LOCK)
	{
		BITCSET(HwCDCR1, HwCDCR1_LOCK_ON, (uiLock << 2));
	}
	
	// HwCDCR1_BS_1    HwZERO      // The DMA transfers the image data as 1 word to memory
	// HwCDCR1_BS_2    Hw0            // The DMA transfers the image data as 2 word to memory
	// HwCDCR1_BS_4    Hw1            // The DMA transfers the image data as 4 word to memory
	// HwCDCR1_BS_8    (Hw1|Hw0)  // The DMA transfers the image data as 8 word to memory (default)
	if(uiFlag & SET_CIF_TRANSFER_BURST)
	{
		BITCSET(HwCDCR1, HwCDCR1_BS_8, uiBurst); // HwCIF->CDCR1
	}
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_convertR2Y
//
//	DESCRIPTION
//    		set CIF register  : 	HwCR2Y
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_R2Y_Convert(unsigned int uiFlag,  unsigned int uiFMT, unsigned int uiEN)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	//HwCR2Y_FMT                                  (Hw4|Hw3|Hw2|Hw1)  // FMT[4:1]  0000 -> Input format 16bit 565RGB(RGB sequence) 자세한 사항 750A CIF SPEC. 1-22
	if(uiFlag & SET_CIF_CR2Y_FMT)
	{
		BITCSET(pCIF->CR2Y, HwCR2Y_FMT, (uiFMT<<1));
	}

	//HwCR2Y_EN                                    Hw0          // R2Y Enable,   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CR2Y_EN)
	{
		BITCSET(pCIF->CR2Y, HwCR2Y_EN, (uiEN)); // PCIF->CR2Y
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	//HwCR2Y_FMT                                  (Hw4|Hw3|Hw2|Hw1)  // FMT[4:1]  0000 -> Input format 16bit 565RGB(RGB sequence) 자세한 사항 750A CIF SPEC. 1-22
	if(uiFlag & SET_CIF_CR2Y_FMT)
	{
	    BITCSET(HwCR2Y, HwCR2Y_555GAR_BGR8, (uiFMT<<1));
	}
	
	//HwCR2Y_EN                                    Hw0          // R2Y Enable,   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CR2Y_EN)
	{
			BITCSET(HwCR2Y, HwCR2Y_EN, (uiEN)); // HwCIF->CR2Y
	}

#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_overlayCtrl
//
//	DESCRIPTION
//    		set CIF register  : 	HwOCTRL1, HwOCTRL2
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_OverlayCtrl(unsigned int uiFlag, unsigned int uiRgb)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if(uiFlag & SET_CIF_ALPHA_ENABLE)
	{
		BITSET(pCIF->OCTRL1, HwOCTRL1_AEN_EN);
	}
	
	if(uiFlag & SET_CIF_ALPHA_DISABLE)
	{
		BITCLR(pCIF->OCTRL1, HwOCTRL1_AEN_EN);
	} 
	if(uiFlag & SET_CIF_CHROMA_ENABLE)
	{
		BITSET(pCIF->OCTRL1, HwOCTRL1_CEN_EN);
	}
	
	if(uiFlag & SET_CIF_CHROMA_DISABLE)
	{
		BITCLR(pCIF->OCTRL1, HwOCTRL1_CEN_EN);
	}

	if(uiFlag & SET_CIF_OVERLAY_ENABLE)
	{    
		BITSET(pCIF->OCTRL1, HwOCTRL1_OE_EN);
	}
	
	if(uiFlag & SET_CIF_OVERLAY_DISABLE)
	{    
		BITCLR(pCIF->OCTRL1, HwOCTRL1_OE_EN);
	}
	
	if(uiFlag & SET_CIF_COLOR_CONV_ENABLE)
	{
		BITSET(pCIF->OCTRL2, HwOCTRL2_CONV);
	}
	
	if(uiFlag & SET_CIF_COLOR_CONV_DISABLE)
	{
		BITCLR(pCIF->OCTRL2, HwOCTRL2_CONV);
	}
	
	if(uiFlag & SET_CIF_COLOR_MODE_RGB)
	{
		BITSET(pCIF->OCTRL2, HwOCTRL2_MD);
		BITCSET(pCIF->OCTRL2, 0x00000006, (uiRgb << 1));        
	}
	
	if(uiFlag & SET_CIF_COLOR_MODE_YUV)
	{
		BITCLR(pCIF->OCTRL2, HwOCTRL2_MD);
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)	
	if(uiFlag & SET_CIF_ALPHA_ENABLE)
	{
		BITSET(HwOCTRL1, HwOCTRL1_AEN_EN);
	}
	
	if(uiFlag & SET_CIF_ALPHA_DISABLE)
	{
		BITCLR(HwOCTRL1, HwOCTRL1_AEN_EN);
	} 
	if(uiFlag & SET_CIF_CHROMA_ENABLE)
	{
		BITSET(HwOCTRL1, HwOCTRL1_CEN_EN);
	}
	
	if(uiFlag & SET_CIF_CHROMA_DISABLE)
	{
		BITCLR(HwOCTRL1, HwOCTRL1_CEN_EN);
	}
	
	if(uiFlag & SET_CIF_OVERLAY_ENABLE)
	{    
		BITSET(HwOCTRL1, HwOCTRL1_OE_EN);
	}
	
	if(uiFlag & SET_CIF_OVERLAY_DISABLE)
	{    
		BITCLR(HwOCTRL1, HwOCTRL1_OE_EN);
	}
	
	if(uiFlag & SET_CIF_COLOR_CONV_ENABLE)
	{
		BITSET(HwOCTRL2, HwOCTRL2_CONV);
	}
	
	if(uiFlag & SET_CIF_COLOR_CONV_DISABLE)
	{
		BITCLR(HwOCTRL2, HwOCTRL2_CONV);
	}
	
	if(uiFlag & SET_CIF_COLOR_MODE_RGB)
	{
		BITSET(HwOCTRL2, HwOCTRL2_MD);
		BITCSET(HwOCTRL2, 0x00000006, (uiRgb << 1));        
	}
	
	if(uiFlag & SET_CIF_COLOR_MODE_YUV)
	{
		BITCLR(HwOCTRL2, HwOCTRL2_MD);
	}

#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_overlaySet
//
//	DESCRIPTION
//    		set CIF register  : 	HwOCTRL1
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetOverlay(unsigned int uiFlag, unsigned int uiOverlayCNT, unsigned int uiOverlayMethod, 
							 unsigned int uiXOR1, unsigned int uiXOR0, unsigned int uiAlpha1, unsigned int uiAlpha0)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if(uiFlag & SET_CIF_OVERLAY_COUNT)
	{
		BITCSET(pCIF->OCTRL1, HwOCTRL1_OCNT_MAX, (uiOverlayCNT << 24));
	}

		if(uiFlag & SET_CIF_OVERLAY_METHOD)
	{
		BITCSET(pCIF->OCTRL1, HwOCTRL1_OM_BLOCK, (uiOverlayMethod << 16));
	} 

		if(uiFlag & SET_CIF_OVERLAY_XOR0)
	{
		BITCSET(pCIF->OCTRL1, HwOCTRL1_XR0_100, (uiXOR0 << 9));
	} 

	if(uiFlag & SET_CIF_OVERLAY_XOR1)
	{
		BITCSET(pCIF->OCTRL1, HwOCTRL1_XR1_100, (uiXOR1 << 10));
	}	

	if(uiFlag & SET_CIF_OVERLAY_ALPHA0)
	{
		BITCSET(pCIF->OCTRL1, HwOCTRL1_AP0_100, (uiAlpha0 << 4));
	}

	if(uiFlag & SET_CIF_OVERLAY_ALPHA1)
	{
		BITCSET(pCIF->OCTRL1, HwOCTRL1_AP1_100, (uiAlpha1 << 6));
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	if(uiFlag & SET_CIF_OVERLAY_COUNT)
	{
		BITCSET(HwOCTRL1, HwOCTRL1_OCNT_MAX, (uiOverlayCNT << 24));
	}

	if(uiFlag & SET_CIF_OVERLAY_METHOD)
	{
		BITCSET(HwOCTRL1, HwOCTRL1_OM_BLOCK, (uiOverlayMethod << 16));
	} 

	if(uiFlag & SET_CIF_OVERLAY_XOR0)
	{
		BITCSET(HwOCTRL1, HwOCTRL1_XR0_100, (uiXOR0 << 9));
	} 

	if(uiFlag & SET_CIF_OVERLAY_XOR1)
	{
		BITCSET(HwOCTRL1, HwOCTRL1_XR1_100, (uiXOR1 << 10));
	} 

	if(uiFlag & SET_CIF_OVERLAY_ALPHA0)
	{
		BITCSET(HwOCTRL1, HwOCTRL1_AP0_100, (uiAlpha0 << 4));
	}

	if(uiFlag & SET_CIF_OVERLAY_ALPHA1)
	{
		BITCSET(HwOCTRL1, HwOCTRL1_AP1_100, (uiAlpha1 << 6));
	} 

#endif
}	

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_overlaySetKey
//
//	DESCRIPTION
//    		set CIF register  : 	HwOCTRL3, HwOCTRL4
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetOverlayKey(unsigned int uiFlag, unsigned int uiKEYR, unsigned int uiKEYG, unsigned int uiKEYB, 
								 unsigned int uiMKEYR, unsigned int uiMKEYG, unsigned int uiMKEYB)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if(uiFlag & SET_CIF_OVERLAY_KEY)
	{
		BITCSET(pCIF->OCTRL3, (HwOCTRL3_KEYR_MAX|HwOCTRL3_KEYG_MAX|HwOCTRL3_KEYB_MAX), ((uiKEYR << 16)|(uiKEYG << 8)|uiKEYB));
	} // pCIF->OCTRL3

	if(uiFlag & SET_CIF_OVERLAY_MASKKEY)
	{
		BITCSET(pCIF->OCTRL4, (HwOCTRL4_MKEYR_MAX|HwOCTRL4_MKEYG_MAX|HwOCTRL4_MKEYB_MAX), ((uiMKEYR << 16)|(uiMKEYG << 8)|uiMKEYB));
	} // pCIF->OCTRL4
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	if(uiFlag & SET_CIF_OVERLAY_KEY)
	{
		BITCSET(HwOCTRL3, (HwOCTRL3_KEYR_MAX|HwOCTRL3_KEYG_MAX|HwOCTRL3_KEYB_MAX), ((uiKEYR << 16)|(uiKEYG << 8)|uiKEYB));
	} // HwCIF->OCTRL3

	if(uiFlag & SET_CIF_OVERLAY_MASKKEY)
	{
		BITCSET(HwOCTRL4, (HwOCTRL4_MKEYR_MAX|HwOCTRL4_MKEYG_MAX|HwOCTRL4_MKEYB_MAX), ((uiMKEYR << 16)|(uiMKEYG << 8)|uiMKEYB));
	} // HwCIF->OCTRL4
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setSyncPol
//
//	DESCRIPTION
//    		set CIF register  : 	H and V sync polarity
//
//	Parameters
//    	
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetSyncPol(unsigned int uiHPolarity, unsigned int uiVpolarity)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	BITCSET(pCIF->ICPCR1, HwICPCR1_HSP_HIGH, (uiHPolarity << 1));
	BITCSET(pCIF->ICPCR1, HwICPCR1_VSP_HIGH, uiVpolarity); // pCIF->ICPCR1
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else
	BITCSET(HwICPCR1, HwICPCR1_HSP_HIGH, (uiHPolarity << 1));
	BITCSET(HwICPCR1, HwICPCR1_VSP_HIGH, uiVpolarity); // HwCIF->ICPCR1
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setSyncPol
//
//	DESCRIPTION
//    		set CIF register  : 	HwIIS, HwIIW1, HwIIW2, 
// 							HwOIS, HwOIW1, HwOIW2
//
//	Parameters
//                CIF_SETIMG (Camera I/F setting image)
//                uiType                 : Input imge(INPUT_IMG), overlay image (OVER_IMG)
//                uiHsize                : Horizontal size
//                uiVSize                : vertical size
//                uiHorWindowingStart    : start X position of windowing image
//                uiHorWindowingEnd    : end X position of windowing image
//                uiVerWindowingStart    : start Y position of windowing image
//                uiVerWindowingEnd    : end Y position of windowing image
//               BaseAddress0            : Y channel base address In Overlay, overlay image adress
//               BaseAddress1            : U channel base address (don't use overlay image)
//                BaseAddress2            : V channel base address (don't use overlay image)
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void    TDD_CIF_SetImage(unsigned int uiType, unsigned int uiHsize, unsigned int uiVsize,
                              unsigned int uiHorWindowingStart, unsigned int uiHorWindowingEnd,
                              unsigned int uiVerWindowingStart, unsigned int uiVerWindowingEnd)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if (uiType & INPUT_IMG)
	{
		BITCSET(pCIF->IIS, 0xFFFFFFFF, ((uiHsize << 16) | uiVsize));
		BITCSET(pCIF->IIW1, 0xFFFFFFFF, ((uiHorWindowingStart<< 16) | uiHorWindowingEnd));
		BITCSET(pCIF->IIW2, 0xFFFFFFFF, ((uiVerWindowingStart<< 16) | uiVerWindowingEnd));
	}

	if(uiType & OVERLAY_IMG)
	{
		BITCSET(pCIF->OIS, 0xFFFFFFFF, ((uiHsize << 16) | uiVsize));
		BITCSET(pCIF->OIW1, 0xFFFFFFFF, ((uiHorWindowingStart<< 16) | uiHorWindowingEnd));
		BITCSET(pCIF->OIW2, 0xFFFFFFFF, ((uiVerWindowingStart<< 16) | uiVerWindowingEnd));
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	if (uiType & INPUT_IMG)
	{
		BITCSET(HwIIS, 0xFFFFFFFF, ((uiHsize << 16) | uiVsize));
		BITCSET(HwIIW1, 0xFFFFFFFF, ((uiHorWindowingStart<< 16) | uiHorWindowingEnd));
		BITCSET(HwIIW2, 0xFFFFFFFF, ((uiVerWindowingStart<< 16) | uiVerWindowingEnd));
	}

	if(uiType & OVERLAY_IMG)
	{
		BITCSET(HwOIS, 0xFFFFFFFF, ((uiHsize << 16) | uiVsize));
		BITCSET(HwOIW1, 0xFFFFFFFF, ((uiHorWindowingStart<< 16) | uiHorWindowingEnd));
		BITCSET(HwOIW2, 0xFFFFFFFF, ((uiVerWindowingStart<< 16) | uiVerWindowingEnd));
	}
#endif	
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setSensorOutImgSize
//
//	DESCRIPTION
//    		set CIF register  : 	HwCEIS
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetSensorOutImgSize(unsigned int uiHsize, unsigned int uiVsize)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CEIS, HwCEIS_HSIZE, (uiHsize<<16));
	BITCSET(pCIFEffect->CEIS, HwCEIS_VSIZE, (uiVsize));    // HwCEM->CEIS
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	BITCSET(HwCEIS, HwCEIS_HSIZE, (uiHsize<<16));
	BITCSET(HwCEIS, HwCEIS_VSIZE, (uiVsize));    // HwCEM->CEIS

#endif
}	


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setBaseAddr
//
//	DESCRIPTION
//    		set CIF register  : 	HwCDCR2, HwCDCR3, HwCDCR4
//									HwCOBA
//									HwCDCR5, 
//									HwCDCR6, HwCDCR7, HwCDCR8
//									HwCESA
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetBaseAddr (unsigned int uiType, unsigned int uiBaseAddr0, 
                                                   unsigned int uiBaseAddr1, unsigned int uiBaseAddr2)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if (uiType & INPUT_IMG)
	{
		BITCSET(pCIF->CDCR2, 0xFFFFFFFF, uiBaseAddr0);
		BITCSET(pCIF->CDCR3, 0xFFFFFFFF, uiBaseAddr1);
		BITCSET(pCIF->CDCR4, 0xFFFFFFFF, uiBaseAddr2);
	}

	if(uiType & OVERLAY_IMG)
	{
		BITCSET(pCIF->COBA, 0xFFFFFFFF, uiBaseAddr0);
	}

	if (uiType & IN_IMG_ROLLING)
	{
		BITCSET(pCIF->CDCR6, 0xFFFFFFFF, uiBaseAddr0);
		BITCSET(pCIF->CDCR7, 0xFFFFFFFF, uiBaseAddr1);
		BITCSET(pCIF->CDCR8, 0xFFFFFFFF, uiBaseAddr2);
	}

	if(uiType & IN_ENC_START_ADDR)
	{
		BITCSET(pCIF->CESA, 0xFFFFFFFF, uiBaseAddr0);
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)

	if (uiType & INPUT_IMG)
	{
		BITCSET(HwCDCR2, 0xFFFFFFFF, uiBaseAddr0);
		BITCSET(HwCDCR3, 0xFFFFFFFF, uiBaseAddr1);
		BITCSET(HwCDCR4, 0xFFFFFFFF, uiBaseAddr2);
	}

	if(uiType & OVERLAY_IMG)
	{
		BITCSET(HwCOBA, 0xFFFFFFFF, uiBaseAddr0);
	}

	if (uiType & IN_IMG_ROLLING)
	{
		BITCSET(HwCDCR5, 0xFFFFFFFF, uiBaseAddr0);
		BITCSET(HwCDCR6, 0xFFFFFFFF, uiBaseAddr1);
		BITCSET(HwCDCR7, 0xFFFFFFFF, uiBaseAddr2);
	}

	if(uiType & IN_ENC_START_ADDR)
	{
		BITCSET(HwCESA, 0xFFFFFFFF, uiBaseAddr0);
	}
#endif	
}	


void TDD_CIF_SetBaseAddr_offset(unsigned int uiType, unsigned int uiOFFSET_Y, unsigned int uiOFFSET_C)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if (uiType & INPUT_IMG)
	{
		BITCSET(pCIF->CDCR5, 0xFFFFFFFF, ((uiOFFSET_C << 16) | uiOFFSET_Y));
	}

	if(uiType & OVERLAY_IMG)
	{
		BITCSET(pCIF->COBO, 0xFFFFFFFF, (uiOFFSET_Y));
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setInterrupt
//
//	DESCRIPTION
//    		set CIF register  : 	HwIEN, HwSEL, HwCIRQ
//							HwIEN, HwIRQSEL, HwCIRQ
//							HwCESA
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetInterrupt(unsigned int uiFlag)
{
	// Interrupt Enable  0:interrupt disable, 1:interrupt enable    //Hw31
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	volatile PPIC pPIC = (PPIC)tcc_p2v(HwPIC_BASE);

	if(uiFlag & SET_CIF_INT_ENABLE)
	{
		BITSET(pPIC->IEN0, HwINT0_CAM);    
		BITSET(pPIC->SEL0, HwINT0_CAM);
		BITSET(pCIF->CIRQ, HwCIRQ_IEN);    // Camera Interrupt enable
	}  
	if(uiFlag & SET_CIF_INT_DISABLE)
	{
		BITCLR(pPIC->IEN0, HwINT0_CAM);    
		//BITCLR(HwPIC->SEL0, HwINT0_CAM);
		BITCLR(pCIF->CIRQ, HwCIRQ_IEN);    // Camera Interrupt enable
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	if(uiFlag & SET_CIF_INT_ENABLE)
	{
		BITSET(HwIEN, HwINT_CAM);    
		BITSET(HwSEL, HwINT_CAM);
		BITSET(HwCIRQ, HwCIRQ_IEN_EN);    // Camera Interrupt enable
	}  
	if(uiFlag & SET_CIF_INT_DISABLE)
	{
		BITCLR(HwIEN, HwINT_CAM);    
		BITCLR(HwSEL, HwINT_CAM);
		BITCLR(HwCIRQ, HwCIRQ_IEN_EN);    // Camera Interrupt enable
	}  
#endif
	

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	// Update Register in VSYNC   0:Register is update without VSYNC , 1:When VSYNC is posedge, register is updated.  //Hw30
	if(uiFlag & SET_CIF_UPDATE_IN_VSYNC)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_URV);
	}	

	if(uiFlag & SET_CIF_UPDATE_WITHOUT_VSYNC)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_URV);
	}
		
	// Interrupt Type  0:Pulse type, 1:Hold-up type when respond signal(ICR) is high  //Hw29
	if(uiFlag & SET_CIF_INT_TYPE_HOLDUP)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_ITY);
	}
		
	if(uiFlag & SET_CIF_INT_TYPE_PULSE)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_ITY);
	}
		
	// Interrupt Clear 0:.... , 1:Interrupt Clear (using ITY is Hold-up type)  //Hw28
	if(uiFlag & SET_CIF_INT_HOLD_CLEAR)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_ICR);
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	// Update Register in VSYNC   0:Register is update without VSYNC , 1:When VSYNC is posedge, register is updated.  //Hw30
	if(uiFlag & SET_CIF_UPDATE_IN_VSYNC)
	{
		BITSET(HwCIRQ, HwCIRQ_URV);
	}
	
	if(uiFlag & SET_CIF_UPDATE_WITHOUT_VSYNC)
	{
		BITCLR(HwCIRQ, HwCIRQ_URV);
	}
	
	// Interrupt Type  0:Pulse type, 1:Hold-up type when respond signal(ICR) is high  //Hw29
	if(uiFlag & SET_CIF_INT_TYPE_HOLDUP)
	{
		BITSET(HwCIRQ, HwCIRQ_ITY);
	}
	
	if(uiFlag & SET_CIF_INT_TYPE_PULSE)
	{
		BITCLR(HwCIRQ, HwCIRQ_ITY);
	}
	
	// Interrupt Clear 0:.... , 1:Interrupt Clear (using ITY is Hold-up type)  //Hw28
	if(uiFlag & SET_CIF_INT_HOLD_CLEAR)
	{
		BITSET(HwCIRQ, HwCIRQ_ICR);
	}
#endif
}	


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setMaskIntStatus
//
//	DESCRIPTION
// 			CIF interrupt status masking
//    		set CIF register  : 	HwCIRQ
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetMaskIntStatus(unsigned int uiFlag)
{

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	// Hw29 should be written "1"!!
	BITSET(pCIF->CIRQ, HwCIRQ_ITY);

	// Hw26           // Mask interrupt of VS negative edge,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_VS_NEGA_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MVN);
	}
	
	if(uiFlag & SET_CIF_INT_VS_NEGA_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MVN);
	}
	
	// Hw25           // Mask interrupt of VS negative edge,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_VS_POSI_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MVP);
	}
	
	if(uiFlag & SET_CIF_INT_VS_POSI_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MVP);
	}
	
	//        Hw24           // Mask interrupt of VCNT Interrupt,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_VCNT_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MVIT);
	}
	
	if(uiFlag & SET_CIF_INT_VCNT_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MVIT);
	}
	
	//          Hw23           // Mask interrupt of Scaler Error,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_SCALER_ERR_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MSE);
	}
	
	if(uiFlag & SET_CIF_INT_SCALER_ERR_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MSE);
	}
	
	//          Hw22           // Mask interrupt of Scaler finish,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_SCALER_FINISH_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MSF);
	}
	
	if(uiFlag & SET_CIF_INT_SCALER_FINISH_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MSF);
	}
	
	//          Hw21           // Mask interrupt of Encoding start,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ENC_STRT_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MENS);
	}
	
	if(uiFlag & SET_CIF_INT_ENC_STRT_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MENS);
	}
	
	//          Hw20           // Mask interrupt of Rolling V address,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ROLL_VADDR_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MRLV);
	}
	
	if(uiFlag & SET_CIF_INT_ROLL_VADDR_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MRLV);
	}
	
	//          Hw19           // Mask interrupt of Rolling U address,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ROLL_UADDR_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MRLU);
	}
	
	if(uiFlag & SET_CIF_INT_ROLL_UADDR_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MRLU);
	}
	
	//          Hw18           // Mask interrupt of Rolling Y address,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ROLL_YADDR_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MRLY);
	}
	
	if(uiFlag & SET_CIF_INT_ROLL_YADDR_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MRLY);
	}
	
	//          Hw17           // Mask interrupt of Capture frame,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_CAPTURE_FRM_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MSCF);
	}
	
	if(uiFlag & SET_CIF_INT_CAPTURE_FRM_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MSCF);
	}
	
	//          Hw16           // Mask interrupt of Stored one frame,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_STORE_1FRM_MASK)
	{
		BITSET(pCIF->CIRQ, HwCIRQ_MSOF);
	}
	
	if(uiFlag & SET_CIF_INT_STORE_1FRM_NOT_MASK)
	{
		BITCLR(pCIF->CIRQ, HwCIRQ_MSOF);
	}

	if(uiFlag & SET_CIF_INT_ALL_MASK)
	{
		//BITCSET(HwCIRQ, 0x07FF0000, 0x7FF<<16);
		BITSET(pCIF->CIRQ, 0x7FF<<16);
	}

	if(uiFlag & SET_CIF_INT_ALL_CLEAR_MASK)
	{
		BITCLR(pCIF->CIRQ, 0x5FF<<16);// Hw29 should be written "1"!!
	} // pCIF->CIRQ
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC79X)
	// Hw26           // Mask interrupt of VS negative edge,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_VS_NEGA_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MVN);
	}
	
	if(uiFlag & SET_CIF_INT_VS_NEGA_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MVN);
	}
	
	// Hw25           // Mask interrupt of VS negative edge,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_VS_POSI_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MVP);
	}
	
	if(uiFlag & SET_CIF_INT_VS_POSI_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MVP);
	}
	
	//        Hw24           // Mask interrupt of VCNT Interrupt,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_VCNT_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MVIT);
	}
	
	if(uiFlag & SET_CIF_INT_VCNT_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MVIT);
	}
	
	//          Hw23           // Mask interrupt of Scaler Error,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_SCALER_ERR_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MSE);
	}
	
	if(uiFlag & SET_CIF_INT_SCALER_ERR_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MSE);
	}
	
	//          Hw22           // Mask interrupt of Scaler finish,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_SCALER_FINISH_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MSF);
	}
	
	if(uiFlag & SET_CIF_INT_SCALER_FINISH_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MSF);
	}
	
	//          Hw21           // Mask interrupt of Encoding start,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ENC_STRT_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MENS);
	}
	
	if(uiFlag & SET_CIF_INT_ENC_STRT_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MENS);
	}
	
	//          Hw20           // Mask interrupt of Rolling V address,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ROLL_VADDR_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MRLV);
	}
	
	if(uiFlag & SET_CIF_INT_ROLL_VADDR_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MRLV);
	}
	
	//          Hw19           // Mask interrupt of Rolling U address,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ROLL_UADDR_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MRLU);
	}
	
	if(uiFlag & SET_CIF_INT_ROLL_UADDR_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MRLU);
	}
	
	//          Hw18           // Mask interrupt of Rolling Y address,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_ROLL_YADDR_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MRLY);
	}
	
	if(uiFlag & SET_CIF_INT_ROLL_YADDR_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MRLY);
	}
	
	//          Hw17           // Mask interrupt of Capture frame,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_CAPTURE_FRM_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MSCF);
	}
	
	if(uiFlag & SET_CIF_INT_CAPTURE_FRM_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MSCF);
	}
	
	//          Hw16           // Mask interrupt of Stored one frame,  0:Don't mask, 1:Mask
	if(uiFlag & SET_CIF_INT_STORE_1FRM_MASK)
	{
		BITSET(HwCIRQ, HwCIRQ_MSOF);
	}
	
	if(uiFlag & SET_CIF_INT_STORE_1FRM_NOT_MASK)
	{
		BITCLR(HwCIRQ, HwCIRQ_MSOF);
	}

	if(uiFlag & SET_CIF_INT_ALL_MASK)
	{
		//BITCSET(HwCIRQ, 0x07FF0000, 0x7FF<<16);
		BITSET(HwCIRQ, 0x7FF<<16);
	}
	
	if(uiFlag & SET_CIF_INT_ALL_CLEAR_MASK)
	{
		BITCLR(HwCIRQ, 0x7FF<<16);
	} // HwCIF->CIRQ

#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_getIntStatus
//
//	DESCRIPTION
//			get CIF status
//    		get CIF register  : 	HwCIRQ
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
unsigned int TDD_CIF_GetIntStatus(unsigned int uiFlag)
{
    unsigned long uiRegStatus = 0;

#if defined(CONFIG_ARCH_TCC79X)
	// 인터럽트 발생 과 동시에 status setting이 change된 status value를  READ(GET)
	if(uiFlag & GET_CIF_INT_VS_STATUS)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_VSS;
	}
	
	//#define  HwCIRQ_VN                                       Hw10            // VS positive, 0:-, 1:When VS is generated if Negative edge
	if(uiFlag & GET_CIF_INT_NEGA_VS_GEN)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_VN;  
		//BITSET(HwCIRQ, HwCIRQ_VN);
	}
	
	//#define  HwCIRQ_VP                                       Hw9            // VS positive, 0:-, 1:When VS is generated if positive edge
	if(uiFlag & GET_CIF_INT_POSI_VS_GEN)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_VP;  
		//BITSET(HwCIRQ, HwCIRQ_VP);
	}
	
	//#define  HwCIRQ_VIT                                      Hw8            // VCNT Interrupt, 0:-, 1:When VCNT is generated....
	if(uiFlag & GET_CIF_INT_VCNT_GEN)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_VIT;  
		//BITSET(HwCIRQ, HwCIRQ_VIT);
	}
	
	//#define  HwCIRQ_SE                                       Hw7            // Scaler Error, 0:-, 1:When Scale operation is not correct.
	if(uiFlag & GET_CIF_INT_SCALER_ERR)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_SE;  
		//BITSET(HwCIRQ, HwCIRQ_SE);
	}
	
	//#define  HwCIRQ_SF                                       Hw6            // Scaler Finish, 0:-, 1:When Scale operation is finished
	if(uiFlag & GET_CIF_INT_SCALER_FINISH)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_SF;  
		//BITSET(HwCIRQ, HwCIRQ_SF);
	}
	
	//#define  HwCIRQ_ENS                                      Hw5            // Encoding start status, 0:-, 1:When Y address is bigger than encoding start address, this bit is high
	if(uiFlag & GET_CIF_INT_ENC_STRT)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_ENS;  
		//BITSET(HwCIRQ, HwCIRQ_ENS);
	}
	
	//#define  HwCIRQ_ROLV                                     Hw4            // Rolling V address status, 0:-, 1:If V address is move to start address, this bit is high
	if(uiFlag & GET_CIF_INT_ROLL_VADDR_STRT)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_ROLV;
		//BITSET(HwCIRQ, HwCIRQ_ROLV);
	}
	
	//#define  HwCIRQ_ROLU                                     Hw3            // Rolling U address starus, 0:-, 1:If U address is move to start address, this bit is high 
	if(uiFlag & GET_CIF_INT_ROLL_UADDR_STRT)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_ROLU;  
		//    BITSET(HwCIRQ, HwCIRQ_ROLU);
	}
	
	//#define  HwCIRQ_ROLY                                     Hw2            // Rolling Y address starus, 0:-, 1:If Y address is move to start address, this bit is high 
	if(uiFlag & GET_CIF_INT_ROLL_YADDR_STRT)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_ROLY;  
		//BITSET(HwCIRQ, HwCIRQ_ROLY);
	}
	
	//#define  HwCIRQ_SCF                                      Hw1            // Stored captured frame,  0:-, 1:If Captured frame is stored, this bit is high
	if(uiFlag & GET_CIF_INT_CAPTURE_FRM_STORE)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_SCF;  
		//BITSET(HwCIRQ, HwCIRQ_SCF);
	}
	
	//#define  HwCIRQ_SOF                                      Hw0            // Stored One frame, 0-, 1:If one frame if stored, this bit is high.
	if(uiFlag & GET_CIF_INT_ONEFRAME_STORE)
	{
		uiRegStatus = HwCIRQ & HwCIRQ_SOF;  // HwCIF->CIRQ
		//BITSET(HwCIRQ, HwCIRQ_SOF);
	}
#elif defined(CONFIG_ARCH_TCC92XX)  || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	// 인터럽트 발생 과 동시에 status setting이 change된 status value를  READ(GET)
	if(uiFlag & GET_CIF_INT_VS_STATUS)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_VSS;
	}
	
	//#define  HwCIRQ_VN                                       Hw10            // VS positive, 0:-, 1:When VS is generated if Negative edge
	if(uiFlag & GET_CIF_INT_NEGA_VS_GEN)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_VN;  
		//BITSET(HwCIRQ, HwCIRQ_VN);
	}
	
	//#define  HwCIRQ_VP                                       Hw9            // VS positive, 0:-, 1:When VS is generated if positive edge
	if(uiFlag & GET_CIF_INT_POSI_VS_GEN)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_VP;  
		//BITSET(HwCIRQ, HwCIRQ_VP);
	}
	
	//#define  HwCIRQ_VIT                                      Hw8            // VCNT Interrupt, 0:-, 1:When VCNT is generated....
	if(uiFlag & GET_CIF_INT_VCNT_GEN)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_VIT;  
		//BITSET(HwCIRQ, HwCIRQ_VIT);
	}
	
	//#define  HwCIRQ_SE                                       Hw7            // Scaler Error, 0:-, 1:When Scale operation is not correct.
	if(uiFlag & GET_CIF_INT_SCALER_ERR)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_SE;  
		//BITSET(HwCIRQ, HwCIRQ_SE);
	}
	
	//#define  HwCIRQ_SF                                       Hw6            // Scaler Finish, 0:-, 1:When Scale operation is finished
	if(uiFlag & GET_CIF_INT_SCALER_FINISH)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_SF;  
		//BITSET(HwCIRQ, HwCIRQ_SF);
	}
	
	//#define  HwCIRQ_ENS                                      Hw5            // Encoding start status, 0:-, 1:When Y address is bigger than encoding start address, this bit is high
	if(uiFlag & GET_CIF_INT_ENC_STRT)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_ENS;  
		//BITSET(HwCIRQ, HwCIRQ_ENS);
	}
	
	//#define  HwCIRQ_ROLV                                     Hw4            // Rolling V address status, 0:-, 1:If V address is move to start address, this bit is high
	if(uiFlag & GET_CIF_INT_ROLL_VADDR_STRT)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_ROLV;
		//BITSET(HwCIRQ, HwCIRQ_ROLV);
	}
	
	//#define  HwCIRQ_ROLU                                     Hw3            // Rolling U address starus, 0:-, 1:If U address is move to start address, this bit is high 
	if(uiFlag & GET_CIF_INT_ROLL_UADDR_STRT)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_ROLU;  
		//    BITSET(HwCIRQ, HwCIRQ_ROLU);
	}
	
	//#define  HwCIRQ_ROLY                                     Hw2            // Rolling Y address starus, 0:-, 1:If Y address is move to start address, this bit is high 
	if(uiFlag & GET_CIF_INT_ROLL_YADDR_STRT)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_ROLY;  
		//BITSET(HwCIRQ, HwCIRQ_ROLY);
	}
	
	//#define  HwCIRQ_SCF                                      Hw1            // Stored captured frame,  0:-, 1:If Captured frame is stored, this bit is high
	if(uiFlag & GET_CIF_INT_CAPTURE_FRM_STORE)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_SCF;  
		//BITSET(HwCIRQ, HwCIRQ_SCF);
	}
	
	//#define  HwCIRQ_SOF                                      Hw0            // Stored One frame, 0-, 1:If one frame if stored, this bit is high.
	if(uiFlag & GET_CIF_INT_ONEFRAME_STORE)
	{
		uiRegStatus = pCIF->CIRQ & HwCIRQ_SOF;  // PCIF->CIRQ
		//BITSET(HwCIRQ, HwCIRQ_SOF);
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.

#endif
	
    return (unsigned int)uiRegStatus;
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_set656FormatConfig
//
//	DESCRIPTION
//    		set CIF register  : 	Hw656FCR1, Hw656FCR2
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_Set656FormatConfig(unsigned int uiType, unsigned int uiPsl, unsigned int uiFpv,
										unsigned int uiSpv, unsigned int uiTpv, 
										unsigned int uiFb, unsigned int uiHb, unsigned int uiVb)
{
#if defined(CONFIG_ARCH_TCC79X)
	if(uiType & SET_CIF_656_PSL)
	{
		BITCSET(Hw656FCR1, 0x06000000, (uiPsl << 25));
	}
	
	if(uiType & SET_CIF_656_FPV)
	{
		BITCSET(Hw656FCR1, 0x00FF0000, (uiFpv << 16));
	}
	
	if(uiType & SET_CIF_656_SPV)
	{
		BITCSET(Hw656FCR1, 0x0000FF00, (uiSpv << 8));
	}
	
	if(uiType & SET_CIF_656_FIELD_INFO)
	{
		BITCSET(pCIF->CCIR656FCR2, 0x00003C00, (uiFb << 10));
	}
	
	if(uiType & SET_CIF_656_H_BLANK)
	{
		BITCSET(Hw656FCR2, 0x000001E0, (uiHb << 5));
	}
	
	if(uiType & SET_CIF_656_V_BLANK)
	{
			BITCSET(Hw656FCR2, 0x0000000F, uiVb); // HwCIF->CCIR656FCR2
	}
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if(uiType & SET_CIF_656_PSL)
	{
		BITCSET(pCIF->CCIR656FCR1, 0x06000000, (uiPsl << 25));
	}
		
	if(uiType & SET_CIF_656_FPV)
	{
		BITCSET(pCIF->CCIR656FCR1, 0x00FF0000, (uiFpv << 16));
	}
		
	if(uiType & SET_CIF_656_SPV)
	{
		BITCSET(pCIF->CCIR656FCR1, 0x0000FF00, (uiSpv << 8));
	}
		
	if(uiType & SET_CIF_656_FIELD_INFO)
	{
		BITCSET(pCIF->CCIR656FCR2, 0x00003C00, (uiFb << 10));
	}
		
	if(uiType & SET_CIF_656_H_BLANK)
	{
		BITCSET(pCIF->CCIR656FCR2, 0x000001E0, (uiHb << 5));
	}

	if(uiType & SET_CIF_656_V_BLANK)
	{
		BITCSET(pCIF->CCIR656FCR2, 0x0000000F, uiVb); // PCIF->CCIR656FCR2
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setFIFOStatusClear
//
//	DESCRIPTION
//			clear FIFO Status
//    		set CIF register  : 	HwFIFOSTATE
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetFIFOStatusClear(void)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	BITSET(pCIF->FIFOSTATE, HwFIFOSTATE_CLR); // HwCIF->FIFOSTATE
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITSET(HwFIFOSTATE, HwFIFOSTATE_CLR); // HwCIF->FIFOSTATE
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setFIFOStatusClear
//
//	DESCRIPTION
//    		get CIF register  : 	HwFIFOSTATE
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
unsigned int TDD_CIF_GetFIFOStatus(unsigned int uiType)
{
    unsigned long uiRet = 0;

#if defined(CONFIG_ARCH_TCC79X)
	if(uiType & GET_CIF_OVERLAY_READ_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_REO);
	}
	
	if(uiType & GET_CIF_VCH_READ_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_REV);
	}
	
	if(uiType & GET_CIF_UCH_READ_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_REU);
	}
	
	if(uiType & GET_CIF_YCH_READ_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_REY);
	}
	
	if(uiType & GET_CIF_OVERLAY_WRITE_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_WEO);
	}
	
	if(uiType & GET_CIF_VCH_WRITE_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_WEV);
	}
	
	if(uiType & GET_CIF_UCH_WRITE_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_WEU);
	}
	
	if(uiType & GET_CIF_YCH_WRITE_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_WEY);
	}
	
	if(uiType & GET_CIF_OVERLAY_EMPTY_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_EO);
	}
	
	if(uiType & GET_CIF_VCH_EMPTY_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_EV);
	}
	
	if(uiType & GET_CIF_UCH_EMPTY_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_EU);
	}
	
	if(uiType & GET_CIF_YCH_EMPTY_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_EY);
	}
	
	if(uiType & GET_CIF_OVERLAY_FULL_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_FO);
	}
	
	if(uiType & GET_CIF_VCH_FULL_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_FV);
	}
	
	if(uiType & GET_CIF_UCH_FULL_ERR)
	{
		uiRet |= (HwFIFOSTATE & HwFIFOSTATE_FU);
	}
	
	if(uiType & GET_CIF_YCH_FULL_ERR)
	{
			uiRet |= (HwFIFOSTATE & HwFIFOSTATE_FY); // HwCIF->FIFOSTATE
	}
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if(uiType & GET_CIF_OVERLAY_READ_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_REO);
	}
	
	if(uiType & GET_CIF_VCH_READ_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_REV);
	}
	
	if(uiType & GET_CIF_UCH_READ_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_REU);
	}
	
	if(uiType & GET_CIF_YCH_READ_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_REY);
	}
	
	if(uiType & GET_CIF_OVERLAY_WRITE_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_WEO);
	}
	
	if(uiType & GET_CIF_VCH_WRITE_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_WEV);
	}
	
	if(uiType & GET_CIF_UCH_WRITE_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_WEU);
	}
	
	if(uiType & GET_CIF_YCH_WRITE_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_WEY);
	}
	
	if(uiType & GET_CIF_OVERLAY_EMPTY_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_EO);
	}
	
	if(uiType & GET_CIF_VCH_EMPTY_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_EV);
	}
	
	if(uiType & GET_CIF_UCH_EMPTY_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_EU);
	}
	
	if(uiType & GET_CIF_YCH_EMPTY_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_EY);
	}
	
	if(uiType & GET_CIF_OVERLAY_FULL_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_FO);
	}
	
	if(uiType & GET_CIF_VCH_FULL_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_FV);
	}
	
	if(uiType & GET_CIF_UCH_FULL_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_FU);
	}
	
	if(uiType & GET_CIF_YCH_FULL_ERR)
	{
		uiRet |= (pCIF->FIFOSTATE & HwFIFOSTATE_FY); // PCIF->FIFOSTATE
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif
    return (unsigned int)uiRet;
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setCaptureCtrl
//
//	DESCRIPTION
//			CIF Capture Control
//    		set CIF register  : 	HwCCM1, HwCCM2
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetCaptureCtrl(unsigned int uiType,  unsigned int uiSkipNum, unsigned int uiVCnt, unsigned int uiFlag)
{
#if defined(CONFIG_ARCH_TCC79X)
	if(uiType & SET_CIF_SKIP_NUM)
	{
		BITCSET(HwCCM1, HwCCM1_SKIPNUM, (uiSkipNum << 4));
	}
	
	if(uiType & SET_CIF_VCNT_NUM)
	{
		BITCSET(HwCCM2, HwCCM2_VCNT, (uiVCnt << 4));
	}

	if(uiFlag & SET_CIF_EIT_ENC_INT)
	{
		BITSET(HwCCM1, HwCCM1_EIT);
	}
	
	if(uiFlag & SET_CIF_EIT_ALWAYS_1_PULSE)
	{
		BITCLR(HwCCM1, HwCCM1_EIT);
	}
	
	if(uiFlag & SET_CIF_UES_ENABLE)
	{
		BITSET(HwCCM1, HwCCM1_UES);
	}
	
	if(uiFlag & SET_CIF_UES_DISABLE)
	{
		BITCLR(HwCCM1, HwCCM1_UES);
	}
	
	if(uiFlag & SET_CIF_RLV_ENABLE)
	{
		BITSET(HwCCM1, HwCCM1_RLV);
	}
	
	if(uiFlag & SET_CIF_RLV_DISABLE)
	{
		BITCLR(HwCCM1, HwCCM1_RLV);
	}
	
	if(uiFlag & SET_CIF_RLU_ENABLE)
	{
		BITSET(HwCCM1, HwCCM1_RLU);
	}
	
	if(uiFlag & SET_CIF_RLU_DISABLE)
	{
		BITCLR(HwCCM1, HwCCM1_RLU);
	}
	
	if(uiFlag & SET_CIF_RLY_ENABLE)
	{
		BITSET(HwCCM1, HwCCM1_RLY);
	}
	
	if(uiFlag & SET_CIF_RLY_DISABLE)
	{
		BITCLR(HwCCM1, HwCCM1_RLY);
	}
	
	if(uiFlag & SET_CIF_CAP_ENABLE)
	{
		BITSET(HwCCM1, HwCCM1_CAP);
	}
	
	if(uiFlag & SET_CIF_CAP_DISABLE)
	{
		BITCLR(HwCCM1, HwCCM1_CAP);
	}
	
	if(uiFlag & SET_CIF_VEN_ENABLE)
	{
		BITSET(HwCCM2, HwCCM2_VEN);
	}
	
	if(uiFlag & SET_CIF_VEN_DISABLE)
	{
		BITCLR(HwCCM2, HwCCM2_VEN);
	}
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	if(uiType & SET_CIF_SKIP_NUM)
	{
		BITCSET(pCIF->CCM1, HwCCM1_SKIPNUM, (uiSkipNum << 4));
	}

	if(uiType & SET_CIF_VCNT_NUM)
	{
		BITCSET(pCIF->CCM2, HwCCM2_VCNT, (uiVCnt << 4));
	}

	if(uiFlag & SET_CIF_EIT_ENC_INT)
	{
		BITSET(pCIF->CCM1, HwCCM1_EIT);
	}
	
	if(uiFlag & SET_CIF_EIT_ALWAYS_1_PULSE)
	{
		BITCLR(pCIF->CCM1, HwCCM1_EIT);
	}
	
	if(uiFlag & SET_CIF_UES_ENABLE)
	{
		BITSET(pCIF->CCM1, HwCCM1_UES);
	}
	
	if(uiFlag & SET_CIF_UES_DISABLE)
	{
		BITCLR(pCIF->CCM1, HwCCM1_UES);
	}
	
	if(uiFlag & SET_CIF_RLV_ENABLE)
	{
		BITSET(pCIF->CCM1, HwCCM1_RLV);
	}
	
	if(uiFlag & SET_CIF_RLV_DISABLE)
	{
		BITCLR(pCIF->CCM1, HwCCM1_RLV);
	}
	
	if(uiFlag & SET_CIF_RLU_ENABLE)
	{
		BITSET(pCIF->CCM1, HwCCM1_RLU);
	}
	
	if(uiFlag & SET_CIF_RLU_DISABLE)
	{
		BITCLR(pCIF->CCM1, HwCCM1_RLU);
	}
	
	if(uiFlag & SET_CIF_RLY_ENABLE)
	{
		BITSET(pCIF->CCM1, HwCCM1_RLY);
	}
	
	if(uiFlag & SET_CIF_RLY_DISABLE)
	{
		BITCLR(pCIF->CCM1, HwCCM1_RLY);
	}
	
	if(uiFlag & SET_CIF_CAP_ENABLE)
	{
		BITSET(pCIF->CCM1, HwCCM1_CAP);
	}
	
	if(uiFlag & SET_CIF_CAP_DISABLE)
	{
		BITCLR(pCIF->CCM1, HwCCM1_CAP); // PCIF->CCM1
	}
	
	if(uiFlag & SET_CIF_VEN_ENABLE)
	{
		BITSET(pCIF->CCM2, HwCCM2_VEN);
	}
	
	if(uiFlag & SET_CIF_VEN_DISABLE)
	{
		BITCLR(pCIF->CCM2, HwCCM2_VEN); // PCIF->CCM2
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_getCaptureStatus
//
//	DESCRIPTION
//    		get CIF register  : 	HwCCM1
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
unsigned int TDD_CIF_GetCaptureStatus(unsigned int uiType)
{
  unsigned long uiCaptureStatus = 0;

#if defined(CONFIG_ARCH_TCC79X)
	//HwCCM1_ENCNUM                           0xF0000000              // Encode INT number (using CAP mode) [31:28], value area (0~15), Encode interrupt number
	if(uiType & SET_CIF_CCM1_ENCNUM)
	{
		uiCaptureStatus = HwCCM1 & HwCCM1_ENCNUM;
	}
	
	//HwCCM1_ROLNUMV                         0x0F000000               // Rolling number in V (using CAP mode) [27:24], value area (0~15), Rolling number
	if(uiType & SET_CIF_CCM1_ROLNUMV)
	{
		uiCaptureStatus = HwCCM1 & HwCCM1_ROLNUMV;  
	}
	
	//HwCCM1_ROLNUMU                         0x00F00000               // Rolling number in U (using CAP mode) [23:20], value area (0~15), Rolling number
	if(uiType & SET_CIF_CCM1_ROLNUMU)
	{
		uiCaptureStatus = HwCCM1 & HwCCM1_ROLNUMU;  
	}
	
	//HwCCM1_ROLNUMY                         0x000F0000               // Rolling number in Y (using CAP mode) [19:16], value area (0~15), Rolling number
	if(uiType & SET_CIF_CCM1_ROLNUMY)
	{
		uiCaptureStatus = HwCCM1 & HwCCM1_ROLNUMY;  
	}
	
	//HwCCM1_CB                                    Hw10          // Capture Busy,   0:-, 1:Capture busy
	if(uiType & SET_CIF_CCM1_CB)
	{
		uiCaptureStatus = (HwCCM1 & HwCCM1_CB);  
	} // HwCIF->CCM1
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	//HwCCM1_ENCNUM                           0xF0000000              // Encode INT number (using CAP mode) [31:28], value area (0~15), Encode interrupt number
	if(uiType & SET_CIF_CCM1_ENCNUM)
	{
		uiCaptureStatus = pCIF->CCM1 & HwCCM1_ENCNUM;
	}
	
	//HwCCM1_ROLNUMV                         0x0F000000               // Rolling number in V (using CAP mode) [27:24], value area (0~15), Rolling number
	if(uiType & SET_CIF_CCM1_ROLNUMV)
	{
		uiCaptureStatus = pCIF->CCM1 & HwCCM1_ROLNUMV;  
	}
	
	//HwCCM1_ROLNUMU                         0x00F00000               // Rolling number in U (using CAP mode) [23:20], value area (0~15), Rolling number
	if(uiType & SET_CIF_CCM1_ROLNUMU)
	{
		uiCaptureStatus = pCIF->CCM1 & HwCCM1_ROLNUMU;  
	}
	
	//HwCCM1_ROLNUMY                         0x000F0000               // Rolling number in Y (using CAP mode) [19:16], value area (0~15), Rolling number
	if(uiType & SET_CIF_CCM1_ROLNUMY)
	{
		uiCaptureStatus = pCIF->CCM1 & HwCCM1_ROLNUMY;  
	}
	
	//HwCCM1_CB                                    Hw10          // Capture Busy,   0:-, 1:Capture busy
	if(uiType & SET_CIF_CCM1_CB)
	{
		uiCaptureStatus = (pCIF->CCM1 & HwCCM1_CB);  
	} // HwCIF->CCM1
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif
  return (unsigned int)uiCaptureStatus;	
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setEffectMode
//
//	DESCRIPTION
//			CIF effect setting
//    		set CIF register  : 	HwCEM
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetEffectMode(unsigned int uiFlag)
{
	unsigned int  uiValue = 1;
	
#if defined(CONFIG_ARCH_TCC79X)
	BITCSET(HwCEM, 0xFFFFFFFF, 0x00000000);  // 모두 0 시킴	
	
	// HwCEM_UVS                                   Hw15   // UV Swap  0:u-v-u-v sequence, 1:v-u-v-u sequence
	if(uiFlag & SET_CIF_CEM_UVS)
	{
		BITCSET(HwCEM, HwCEM_UVS, (uiValue<<15));   // (uiValue<<15)); only u-v-u-v sequence
	}
	
	// HwCEM_VB                                    Hw14   // V Bias (V channel value offset),   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_VB)
	{
		BITCSET(HwCEM, HwCEM_VB, (uiValue<<14));
	}
	
	// HwCEM_UB                                    Hw13   // U Bias (U channel value offset),   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CEM_UB)
	{
		BITCSET(HwCEM, HwCEM_UB, (uiValue<<13));
	}
	
	// HwCEM_YB                                    Hw12   // Y Bias (Y channel value offset),   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CEM_YB)
	{
		BITCSET(HwCEM, HwCEM_YB, (uiValue<<12));
	}
	
	// HwCEM_YCS                                   Hw11   // YC Swap  0:u-y-v-y sequence, 1:y-u-y-v sequence
	if(uiFlag & SET_CIF_CEM_YCS)
	{
		BITCSET(HwCEM, HwCEM_YCS, (uiValue<<11));    // (uiValue<<11));  only y-u-y-v sequence  
	}
	
	// HwCEM_IVY                                   Hw10   // Invert Y,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_IVY)
	{
		BITCSET(HwCEM, HwCEM_IVY, (uiValue<<10));
	}
	
	// HwCEM_STC                                   Hw9    // Strong C,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_STC)
	{
		BITCSET(HwCEM, HwCEM_STC, (uiValue<<9));
	}
	
	// HwCEM_YCL                                   Hw8    // Y Clamp (Y value clipping),   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_YCL)
	{
		BITCSET(HwCEM, HwCEM_YCL, (uiValue<<8));
	}
	
	// HwCEM_CS                                    Hw7    // C Select (Color filter),   0:disable, 1:Enable(Color filter)  
	if(uiFlag & SET_CIF_CEM_CS )
	{
		BITCSET(HwCEM, HwCEM_CS , (uiValue<<7));
	}

	// HwCEM_SKT                                   Hw6    // Sketch Enable,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_SKT)
	{
		BITCSET(HwCEM, HwCEM_SKT, (uiValue<<6));
	}
	
	// HwCEM_EMM                                   Hw5    // Emboss mode,   0:Positive emboss, 1:Negative emboss
	if(uiFlag & SET_CIF_CEM_EMM)
	{
		BITCSET(HwCEM, HwCEM_EMM, (uiValue<<5));
	}
	
	// HwCEM_EMB                                   Hw4    // Emboss,   0:disable, 1:Enable  
	if(uiFlag & SET_CIF_CEM_EMB)
	{
		BITCSET(HwCEM, HwCEM_EMB, (uiValue<<4));
	}
	
	// HwCEM_NEGA                                  Hw3    // Negative mode,   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CEM_NEGA)
	{
		BITCSET(HwCEM, HwCEM_NEGA, (uiValue<<3));
	}
	
	// HwCEM_GRAY                                  Hw2    // Gray mode,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_GRAY)
	{
		BITCSET(HwCEM, HwCEM_GRAY, (uiValue<<2));
	}
	
	// HwCEM_SEPI                                  Hw1    // Sepia mode,   0:disable, 1:Enable    
	if(uiFlag & SET_CIF_CEM_SEPI)
	{
		BITCSET(HwCEM, HwCEM_SEPI, (uiValue<<1));
	}
	
	// HwCEM_NOR                                   Hw0    // Normal mode,   0:Effect mode, 1:Normal mode 
	if(uiFlag & SET_CIF_CEM_NOR)
	{
		BITCSET(HwCEM, HwCEM_NOR, (uiValue));  // 모두 0 시키고 normal 만 1
	}
	
	//  ALL_CLEAR
	if(uiFlag & SET_CIF_CEM_ALL_CLEAR)
	{
		BITCSET(HwCEM, 0xFFFFFFFF, 0x00000000);  // 모두 0 시킴
	} // HwCEM->CEM
#elif defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);

	BITCSET(pCIFEffect->CEM, 0xFFFFFFFF, 0x00000000);  // 모두 0 시킴	
	
	// HwCEM_UVS                                   Hw15   // UV Swap  0:u-v-u-v sequence, 1:v-u-v-u sequence
	if(uiFlag & SET_CIF_CEM_UVS)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_UVS, (uiValue<<15));   // (uiValue<<15)); only u-v-u-v sequence
	}
	
	// HwCEM_VB                                    Hw14   // V Bias (V channel value offset),   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_VB)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_VB, (uiValue<<14));
	}
	
	// HwCEM_UB                                    Hw13   // U Bias (U channel value offset),   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CEM_UB)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_UB, (uiValue<<13));
	}
	
	// HwCEM_YB                                    Hw12   // Y Bias (Y channel value offset),   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CEM_YB)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_YB, (uiValue<<12));
	}
	
	// HwCEM_YCS                                   Hw11   // YC Swap  0:u-y-v-y sequence, 1:y-u-y-v sequence
	if(uiFlag & SET_CIF_CEM_YCS)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_YCS, (uiValue<<11));    // (uiValue<<11));  only y-u-y-v sequence  
	}
	
	// HwCEM_IVY                                   Hw10   // Invert Y,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_IVY)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_IVY, (uiValue<<10));
	}
	
	// HwCEM_STC                                   Hw9    // Strong C,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_STC)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_STC, (uiValue<<9));
	}
	
	// HwCEM_YCL                                   Hw8    // Y Clamp (Y value clipping),   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_YCL)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_YCL, (uiValue<<8));
	}
	
	// HwCEM_CS                                    Hw7    // C Select (Color filter),   0:disable, 1:Enable(Color filter)  
	if(uiFlag & SET_CIF_CEM_CS )
	{
		BITCSET(pCIFEffect->CEM, HwCEM_CS , (uiValue<<7));
	}

	// HwCEM_SKT                                   Hw6    // Sketch Enable,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_SKT)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_SKT, (uiValue<<6));
	}
	
	// HwCEM_EMM                                   Hw5    // Emboss mode,   0:Positive emboss, 1:Negative emboss
	if(uiFlag & SET_CIF_CEM_EMM)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_EMM, (uiValue<<5));
	}
	
	// HwCEM_EMB                                   Hw4    // Emboss,   0:disable, 1:Enable  
	if(uiFlag & SET_CIF_CEM_EMB)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_EMB, (uiValue<<4));
	}
	
	// HwCEM_NEGA                                  Hw3    // Negative mode,   0:disable, 1:Enable
	if(uiFlag & SET_CIF_CEM_NEGA)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_NEGA, (uiValue<<3));
	}
	
	// HwCEM_GRAY                                  Hw2    // Gray mode,   0:disable, 1:Enable 
	if(uiFlag & SET_CIF_CEM_GRAY)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_GRAY, (uiValue<<2));
	}
	
	// HwCEM_SEPI                                  Hw1    // Sepia mode,   0:disable, 1:Enable    
	if(uiFlag & SET_CIF_CEM_SEPI)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_SEPI, (uiValue<<1));
	}
	
	// HwCEM_NOR                                   Hw0    // Normal mode,   0:Effect mode, 1:Normal mode 
	if(uiFlag & SET_CIF_CEM_NOR)
	{
		BITCSET(pCIFEffect->CEM, HwCEM_NOR, (uiValue));  // 모두 0 시키고 normal 만 1
	}

	//  ALL_CLEAR
	if(uiFlag & SET_CIF_CEM_ALL_CLEAR)
	{
		BITCSET(pCIFEffect->CEM, 0xFFFFFFFF, 0x00000000);  // 모두 0 시킴
	} // HwCEM->CEM
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setEffectSepiaUV
//
//	DESCRIPTION
//			CIF effect setting
//    		set CIF register  : 	HwCSUV
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetEffectSepiaUV(unsigned int uiSepiaU, unsigned int uiSepiaV)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CSUV, HwHwCSUV_SEPIA_U, (uiSepiaU<<8));
	BITCSET(pCIFEffect->CSUV, HwHwCSUV_SEPIA_V, (uiSepiaV));    // HwCEM->CSUV
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCSUV, HwHwCSUV_SEPIA_U, (uiSepiaU<<8));
	BITCSET(HwCSUV, HwHwCSUV_SEPIA_V, (uiSepiaV));    // HwCEM->CSUV
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setEffectColorSelect
//
//	DESCRIPTION
//			CIF effect setting
//    		set CIF register  : 	HwCCS
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetEffectColorSelect(unsigned int uiUStart, unsigned int uiUEnd, 
										unsigned int uiVStart, unsigned int uiVEnd)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CCS, HwCCS_USTART, (uiUStart<<24));
	BITCSET(pCIFEffect->CCS, HwCCS_UEND, (uiUEnd<<16));
	BITCSET(pCIFEffect->CCS, HwCCS_VSTART, (uiVStart<<8));
	BITCSET(pCIFEffect->CCS, HwCCS_VEND, (uiVEnd)); // HwCEM->CCS
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCCS, HwCCS_USTART, (uiUStart<<24));
	BITCSET(HwCCS, HwCCS_UEND, (uiUEnd<<16));
	BITCSET(HwCCS, HwCCS_VSTART, (uiVStart<<8));
	BITCSET(HwCCS, HwCCS_VEND, (uiVEnd)); // HwCEM->CCS
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setEffectHFilterCoeff
//
//	DESCRIPTION
//			CIF effect setting
//    		set CIF register  : 	HwCHFC_COEF
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetEffectHFilterCoeff(unsigned int uiCoeff0, unsigned int uiCoeff1, unsigned int uiCoeff2)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CHFC, HwCHFC_COEF0, (uiCoeff0<<16));
	BITCSET(pCIFEffect->CHFC, HwCHFC_COEF1, (uiCoeff1<<8));
	BITCSET(pCIFEffect->CHFC, HwCHFC_COEF2, (uiCoeff2)); // HwCEM->CHFC
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCHFC, HwCHFC_COEF0, (uiCoeff0<<16));
	BITCSET(HwCHFC, HwCHFC_COEF1, (uiCoeff1<<8));
	BITCSET(HwCHFC, HwCHFC_COEF2, (uiCoeff2));
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setEffectSketchTh
//
//	DESCRIPTION
//			CIF effect setting
//    		set CIF register  : 	HwCST
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetEffectSketchTh(unsigned int uithreshold)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CST, HwCST_THRESHOLD, (uithreshold)); // HwCEM->CST
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCST, HwCST_THRESHOLD, (uithreshold)); // HwCEM->CST
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setEffectClampTh
//
//	DESCRIPTION
//			CIF effect setting
//    		set CIF register  : 	HwCCT
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetEffectClampTh(unsigned int uithreshold)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CCT, HwCCT_THRESHOLD, (uithreshold));         //HwCEM->CCT
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCCT, HwCCT_THRESHOLD, (uithreshold));         //HwCEM->CCT
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setEffectBias
//
//	DESCRIPTION
//			CIF effect setting
//    		set CIF register  : 	HwCBR
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetEffectBias(unsigned int uiYBias, unsigned int uiUBias, unsigned int uiVBias)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CBR, HwCBR_YBIAS, (uiYBias<<16));
	BITCSET(pCIFEffect->CBR, HwCBR_UBIAS, (uiUBias<<8));
	BITCSET(pCIFEffect->CBR, HwCBR_VBIAS, (uiVBias));        //PEFFECT->CBR
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCBR, HwCBR_YBIAS, (uiYBias<<16));
	BITCSET(HwCBR, HwCBR_UBIAS, (uiUBias<<8));
	BITCSET(HwCBR, HwCBR_VBIAS, (uiVBias));        //PEFFECT->CBR
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setInpathCtrl
//
//	DESCRIPTION
//    		get CIF register  : 	HwCIC
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetInpathCtrl(unsigned int uiType, unsigned int uiHWait,
								unsigned int uiStrobeCycle, unsigned int uiInpathWait)
{
#if defined(CONFIG_ARCH_TCC92XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	if(uiType & SET_CIF_INPATH_H_WAIT)
	{
		BITCSET(pCIFEffect->INPATH_CTRL, HwCIC_H2H_WAIT, (uiHWait << 16));
	}
	
	if(uiType & SET_CIF_INPATH_S_CYCLE)
	{
		BITCSET(pCIFEffect->INPATH_CTRL, HwCIC_STB_CYCLE, (uiStrobeCycle << 8));
	}
	
	if(uiType & SET_CIF_INPATH_I_WAIT)
	{
		BITCSET(pCIFEffect->INPATH_CTRL, HwCIC_INP_WAIT, (uiInpathWait << 4));
	}
	
	if(uiType & SET_CIF_INPATH_R_ENABLE)
	{
		BITSET(pCIFEffect->INPATH_CTRL, HwCIC_INPR);
	}
	
	if(uiType & SET_CIF_INPATH_R_DISABLE)
	{
		BITCLR(pCIFEffect->INPATH_CTRL, HwCIC_INPR);
	}
	
	if(uiType & SET_CIF_INPATH_FLUSH_ENABLE)
	{
		BITSET(pCIFEffect->INPATH_CTRL, HwCIC_FA);
	}
	
	if(uiType & SET_CIF_INPATH_FLUSH_DISABLE)
	{
		BITCLR(pCIFEffect->INPATH_CTRL, HwCIC_FA);
	}
	
	if(uiType & SET_CIF_INPATH_ENABLE)
	{
		BITSET(pCIFEffect->INPATH_CTRL, HwCIC_INE);
	}
	
	if(uiType & SET_CIF_INPATH_DISABLE)
	{
		BITCLR(pCIFEffect->INPATH_CTRL, HwCIC_INE);
	}
	
	if(uiType & SET_CIF_INPATH_MEM)
	{
		BITSET(pCIFEffect->INPATH_CTRL, HwCIC_INP);
	}
	
	if(uiType & SET_CIF_INPATH_CAM)
	{
		BITCLR(pCIFEffect->INPATH_CTRL, HwCIC_INP);
	}
#elif defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	//
	//20100628 ysseung   tcc9300에선 INPATH_CTRL register가 없어졌음.
	//
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	if(uiType & SET_CIF_INPATH_H_WAIT)
	{
		BITCSET(HwCIC, HwCIC_H2H_WAIT, (uiHWait << 16));
	}
	
	if(uiType & SET_CIF_INPATH_S_CYCLE)
	{
		BITCSET(HwCIC, HwCIC_STB_CYCLE, (uiStrobeCycle << 8));
	}
	
	if(uiType & SET_CIF_INPATH_I_WAIT)
	{
		BITCSET(HwCIC, HwCIC_INP_WAIT, (uiInpathWait << 4));
	}
	
	if(uiType & SET_CIF_INPATH_R_ENABLE)
	{
		BITSET(HwCIC, HwCIC_INPR);
	}
	
	if(uiType & SET_CIF_INPATH_R_DISABLE)
	{
		BITCLR(HwCIC, HwCIC_INPR);
	}
	
	if(uiType & SET_CIF_INPATH_FLUSH_ENABLE)
	{
		BITSET(HwCIC, HwCIC_FA);
	}
	
	if(uiType & SET_CIF_INPATH_FLUSH_DISABLE)
	{
		BITCLR(HwCIC, HwCIC_FA);
	}
	
	if(uiType & SET_CIF_INPATH_ENABLE)
	{
		BITSET(HwCIC, HwCIC_INE);
	}
	
	if(uiType & SET_CIF_INPATH_DISABLE)
	{
		BITCLR(HwCIC, HwCIC_INE);
	}
	
	if(uiType & SET_CIF_INPATH_MEM)
	{
		BITSET(HwCIC, HwCIC_INP);
	}
	
	if(uiType & SET_CIF_INPATH_CAM)
	{
		BITCLR(HwCIC, HwCIC_INP);
	}

#endif	
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setInpathAddr
//
//	DESCRIPTION
//    		set CIF register  : 	HwCISA1,HwCISA2, HwCISA3
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetInpathAddr(unsigned int uiSrcType, unsigned int uiSrcBase, unsigned int uiSrcBaseY, 
								 unsigned int uiSrcBaseU, unsigned int uiSrcBaseV)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	BITCSET(pCIFEffect->CISA2, HwCISA2_SRC_TYPE_420SEPA, (uiSrcType<<28)); // pCIFEffect->CISA2
	BITCSET(pCIFEffect->CISA1, HwCISA1_SRC_BASE, (uiSrcBase << 28));  // HwCEM->CISA1
	//HwCISA1_SRC_BASE_Y         0x0FFFFFFF             // SRC_BASE_Y [27:0] Source base address in Y channel (27 down to 0 bit assign in bass address)
	BITCSET(pCIFEffect->CISA1, HwCISA1_SRC_BASE_Y, (uiSrcBaseY)); // HwCEM->CISA1
	//HwCISA2_SRC_BASE_U         0x0FFFFFFF              // SRC_BASE_U [27:0] Source base address in U channal (27 down to 0 bit assign in base address)
	BITCSET(pCIFEffect->CISA2, HwCISA2_SRC_BASE_U, (uiSrcBaseU)); // HwCEM->CISA2
	//HwCISA3_SRC_BASE_V           0x0FFFFFFF           // SRC_BASE_V [27:0] Source base address in V channal (27 down to 0 bit assign in base address)
	BITCSET(pCIFEffect->CISA3, HwCISA3_SRC_BASE_V, (uiSrcBaseV)); // HwCEM->CISA3
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCISA2, HwCISA2_SRCTYPE_420SEP, (uiSrcType<<28));
	BITCSET(HwCISA1, HwCISA1_SRC_BASE, (uiSrcBase << 28));  // HwCEM->CISA1
	//HwCISA1_SRC_BASE_Y         0x0FFFFFFF             // SRC_BASE_Y [27:0] Source base address in Y channel (27 down to 0 bit assign in bass address)
	BITCSET(HwCISA1, HwCISA1_SRC_BASE_Y, (uiSrcBaseY)); // HwCEM->CISA1
	//HwCISA2_SRC_BASE_U         0x0FFFFFFF              // SRC_BASE_U [27:0] Source base address in U channal (27 down to 0 bit assign in base address)
	BITCSET(HwCISA2, HwCISA2_SRC_BASE_U, (uiSrcBaseU)); // HwCEM->CISA2
	//HwCISA3_SRC_BASE_V           0x0FFFFFFF           // SRC_BASE_V [27:0] Source base address in V channal (27 down to 0 bit assign in base address)
	BITCSET(HwCISA3, HwCISA3_SRC_BASE_V, (uiSrcBaseV)); // HwCEM->CISA3
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setInpathScale
//
//	DESCRIPTION
//    		set CIF register  : 	HwCISS, HwCISO, HwCIDS, HwCIS
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetInpathScale(unsigned int uiType, unsigned int uiSrcHSize, unsigned int uiSrcVSize, 
								  unsigned int uiOffY, unsigned int uiOffC, unsigned int uiDstHSize,
								  unsigned int uiDstVSize, unsigned int uiHScale, unsigned int uiVScale)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PEFFECT pCIFEffect = (PEFFECT)tcc_p2v(HwCEM_BASE);
	
	if(uiType & SET_CIF_INPATH_SRC_SIZE)
	{
		BITCSET(pCIFEffect->CISS, HwCISS_SRC_HSIZE, (uiSrcHSize<<16)); // PEFFECT->CISS
		BITCSET(pCIFEffect->CISS, HwCISS_SRC_VSIZE, (uiSrcVSize));    
	}
	
	if(uiType & SET_CIF_INPATH_SRC_OFFSET)
	{
		BITCSET(pCIFEffect->CISO, HwCISO_SRC_OFFSET_Y, (uiOffY<<16)); //pCIFEffect->CISO
		BITCSET(pCIFEffect->CISO, HwCISO_SRC_OFFSET_C, (uiOffC));    
	}
	
	if(uiType & SET_CIF_INPATH_DST_SIZE)
	{
		BITCSET(pCIFEffect->CIDS, HwCIDS_DST_HSIZE, (uiDstHSize<<16)); // pCIFEffect->CIDS
		BITCSET(pCIFEffect->CIDS, HwCIDS_DST_VSIZE, (uiDstVSize));    
	}

	if(uiType & SET_CIF_INPATH_SCALE)
	{
		BITCSET(pCIFEffect->CIS, HwCIS_HSCALE, (uiHScale<<16)); // pCIFEffect->CIS
		BITCSET(pCIFEffect->CIS, HwCIS_VSCALE, (uiVScale));    
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	if(uiType & SET_CIF_INPATH_SRC_SIZE)
	{
		BITCSET(HwCISS, HwCISS_SRC_HSIZE, (uiSrcHSize<<16)); // HwCEM->CISS
		BITCSET(HwCISS, HwCISS_SRC_VSIZE, (uiSrcVSize));    
	}
	
	if(uiType & SET_CIF_INPATH_SRC_OFFSET)
	{
		BITCSET(HwCISO, HwCISO_SRC_OFFSET_Y, (uiOffY<<16)); //HwCEM->CISO
		BITCSET(HwCISO, HwCISO_SRC_OFFSET_C, (uiOffC));    
	}
	
	if(uiType & SET_CIF_INPATH_DST_SIZE)
	{
		BITCSET(HwCIDS, HwCIDS_DST_HSIZE, (uiDstHSize<<16)); // HwCEM->CIDS
		BITCSET(HwCIDS, HwCIDS_DST_VSIZE, (uiDstVSize));    
	}
	
	if(uiType & SET_CIF_INPATH_SCALE)
	{
		BITCSET(HwCIS, HwCIS_HSCALE, (uiHScale<<16)); // HwCEM->CIS
		BITCSET(HwCIS, HwCIS_VSCALE, (uiVScale));    
	}
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setDownScale
//
//	DESCRIPTION
//    		set CIF register  : 	HwCDS
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetDownScale(unsigned int uiScaleEnable, unsigned int uiXScale, unsigned int uiYScale)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIF pCIF = (PCIF)tcc_p2v(HwCIF_BASE);
	
	BITCSET(pCIF->CDS, HwCDS_SEN_EN, uiScaleEnable); // pCIF->CDS
	BITCSET(pCIF->CDS, HwCDS_SFH_8, (uiXScale << 4));
	BITCSET(pCIF->CDS, HwCDS_SFV_8, (uiYScale << 2));
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	BITCSET(HwCDS, HwCDS_SEN_EN, uiScaleEnable); // HwCIF->CDS
	BITCSET(HwCDS, HwCDS_SFH_8, (uiXScale << 4));
	BITCSET(HwCDS, HwCDS_SFV_8, (uiYScale << 2));
#endif
}


/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setScalerCtrl
//
//	DESCRIPTION
// 			CIF Scaler control
//    		set CIF register  : 	HwSCC
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetScalerCtrl(unsigned int uiEN)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIFSCALER pCIFScaler = (PCIFSCALER)tcc_p2v(HwCSC_BASE);
	
	if(uiEN & SET_CIF_SCALER_ENABLE) // HwCSC->CSC
	{
		BITSET(pCIFScaler->CSC, HwSCC_EN); // PCIFSACLER 바로 연결해도 될것 같음.
	}

	if(uiEN & SET_CIF_SCALER_DISABLE)
	{
		BITCLR(pCIFScaler->CSC, HwSCC_EN);
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	if(uiEN & SET_CIF_SCALER_ENABLE) // HwCSC->CSC
	{
	        BITSET(HwCSC, HwCSC_EN);
	}
	
	if(uiEN & SET_CIF_SCALER_DISABLE)
	{
	        BITCLR(HwCSC, HwCSC_EN);
	}
#endif
}

/*------------------------------------------------------------------------*/
//	NAME : DEV_CIF_setFreeScale
//
//	DESCRIPTION
//			CIF scaler setting
//    		set CIF register  : 	HwSCSS, HwSCSO, HwSCDS, HwSCSF
//
//	Parameters
//
//	Returns
//    	
/* -----------------------------------------------------------------------*/
void TDD_CIF_SetFreeScale(unsigned int uiType, unsigned int uiSrcHSize, unsigned int uiSrcVSize,
								unsigned int uiOffH, unsigned int uiOffV, unsigned int uiDstHSize,
								unsigned int uiDstVSize, unsigned int uiHFactor, unsigned int uiVFactor)
{
#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX)
	volatile PCIFSCALER pCIFScaler = (PCIFSCALER)tcc_p2v(HwCSC_BASE);
	
	if(uiType & SET_CIF_SCALER_SRC_SIZE)
	{
		BITCSET(pCIFScaler->CSSS, HwSCSS_HSIZE, (uiSrcHSize<<16)); // HwCSC->CSSS
		BITCSET(pCIFScaler->CSSS, HwSCSS_VSIZE, (uiSrcVSize));    
	}
	
	if(uiType & SET_CIF_SCALER_SRC_OFFSET)
	{
		BITCSET(pCIFScaler->CSSO, HwSCSO_OFFSET_H, (uiOffH<<16)); // HwCSC->CSSO
		BITCSET(pCIFScaler->CSSO, HwSCSO_OFFSET_V, (uiOffV));
	}
	
	if(uiType & SET_CIF_SCALER_DST_SIZE)
	{
		BITCSET(pCIFScaler->CSDS, HwSCDS_HSIZE, (uiDstHSize<<16)); //  HwCSC->CSDS
		BITCSET(pCIFScaler->CSDS, HwSCDS_VSIZE, (uiDstVSize));    
	}
	
	if(uiType & SET_CIF_SCALER_FACTOR)
	{
		BITCSET(pCIFScaler->CSSF, HwSCSF_HSCALE, (uiHFactor<<16)); // HwCSC->CSSF
		BITCSET(pCIFScaler->CSSF, HwSCSF_VSCALE, (uiVFactor));    
	}
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else //(CONFIG_ARCH_TCC92XX)
	if(uiType & SET_CIF_SCALER_SRC_SIZE)
	{
              BITCSET(HwCSSS, HwCSSS_HSIZE, (uiSrcHSize<<16));
              BITCSET(HwCSSS, HwCSSS_VSIZE, (uiSrcVSize));    
	}
	
	if(uiType & SET_CIF_SCALER_SRC_OFFSET)
	{
              BITCSET(HwCSSO, HwCSSO_OFFSET_H, (uiOffH<<16));
              BITCSET(HwCSSO, HwCSSO_OFFSET_V, (uiOffV));
	}
	
	if(uiType & SET_CIF_SCALER_DST_SIZE)
	{
              BITCSET(HwCSDS, HwCSDS_HSIZE, (uiDstHSize<<16));
              BITCSET(HwCSDS, HwCSDS_VSIZE, (uiDstVSize));    
	}
	
	if(uiType & SET_CIF_SCALER_FACTOR)
	{
              BITCSET(HwCSSF, HwCSSF_HSCALE, (uiHFactor<<16));
              BITCSET(HwCSSF, HwCSSF_VSCALE, (uiVFactor));    
	}
#endif
}

#if defined(CONFIG_USE_ISP)
void ISP_INTERRUPT_SET(void)
{
	volatile PPIC pPIC = (PPIC)tcc_p2v(HwPIC_BASE);

 	//BITSET(pPIC->IEN0, HwINT0_ISP0);
	//BITSET(pPIC->SEL0, HwINT0_ISP0);

#ifndef	CONFIG_ARCH_TCC93XX
	BITSET(pPIC->IEN0, HwINT0_ISP1);
	BITSET(pPIC->SEL0, HwINT0_ISP1);

	BITSET(pPIC->IEN0, HwINT0_ISP2);
	BITSET(pPIC->SEL0, HwINT0_ISP2);
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else
	BITSET(pPIC->IEN0, HwINT1_ISP1);
	BITSET(pPIC->SEL0, HwINT1_ISP1);

	BITSET(pPIC->IEN0, HwINT0_ISP2);
	BITSET(pPIC->SEL0, HwINT0_ISP2);
#endif
	//BITSET(pPIC->IEN0, HwINT0_ISP3);
	//BITSET(pPIC->SEL0, HwINT0_ISP3);
}

void ISP_INTERRUPT_CLEAR(void)
{
	volatile PPIC pPIC = (PPIC)tcc_p2v(HwPIC_BASE);

 	//BITCLR(pPIC->IEN0, HwINT0_ISP0);
#ifndef	CONFIG_ARCH_TCC93XX
	BITCLR(pPIC->IEN0, HwINT0_ISP1);
	BITCLR(pPIC->IEN0, HwINT0_ISP2);
#elif	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.
	
#else
	BITCLR(pPIC->IEN0, HwINT1_ISP1);
	BITCLR(pPIC->IEN0, HwINT0_ISP2);
#endif
	//BITCLR(pPIC->IEN0, HwINT0_ISP3);
}
#endif

void TDD_CIF_SWReset(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	volatile PDDICONFIG pDDICfg 	= (PDDICONFIG)tcc_p2v(HwDDI_CONFIG_BASE);
	BITSET(pDDICfg->SWRESET, HwDDIC_SWRESET_CIF);   // Reset
	BITCLR(pDDICfg->SWRESET, HwDDIC_SWRESET_CIF);  // Normal
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	volatile PCAMBUSCFG pCamBusCfg = (PCAMBUSCFG)tcc_p2v(HwCAMBUSCFG_BASE);
	#if defined(CONFIG_USE_ISP)
			BITSET(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_ISP);   // Reset
			BITCLR(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_ISP);  // Normal
		#else
			BITSET(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_CIF);   // Reset
			BITCLR(pCamBusCfg->SoftResetRegister, HwCAMBUS_SWRESET_CIF);  // Normal
		#endif
	#elif	defined(CONFIG_ARCH_TCC892X)
		//	In case of 892X, we have to add.
	
	#endif
}

/* end of file */

