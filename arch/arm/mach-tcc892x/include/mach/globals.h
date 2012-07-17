/****************************************************************************
*   FileName    : globals.h
*   Description : 
****************************************************************************
*
*   TCC Version : 1.0
*   Copyright (c) Telechips, Inc.
*   ALL RIGHTS RESERVED
*
****************************************************************************/

//using only global defines, macros.. etc - If you want using this file contact to RYU

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#ifdef __cplusplus
extern "C" {
#endif

//Log Level
#define TC_ERROR 	0x00000001
#define TC_LOG		0x00000002
#define TC_TRACE	0x00000004
#define	TC_DEBUG	0x00000008

	//system info
#define IOCTL_PLATFORM_TYPE                 (L"PLATFORM_TYPE")
#define IOCTL_PLATFORM_OEM                  (L"PLATFORM_OEM")

//------------------------------------------------------------------------------
//  Define:  IOCTL_PROCESSOR_VENDOR/NAME/CORE
//
//  Defines the processor information
//

#define IOCTL_PROCESSOR_VENDOR              (L"Telechips")
#define IOCTL_PROCESSOR_NAME                (L"TCC892x")
#define IOCTL_PROCESSOR_CORE                (L"Cortex-A5")

//------------------------------------------------------------------------------
//
//  Define:  IOCTL_PROCESSOR_INSTRUCTION_SET
//
//  Defines the processor instruction set information
//
#define IOCTL_PROCESSOR_INSTRUCTION_SET     (0)
#define IOCTL_PROCESSOR_CLOCK_SPEED	    266*1000

//macro defines
/************************************************************************************************
*										 MACRO												   *
************************************************************************************************/
#ifndef BITSET
#define BITSET(X, MASK) 			( (X) |= (unsigned int)(MASK) )
#endif
#ifndef BITSCLR
#define BITSCLR(X, SMASK, CMASK)	( (X) = ((((unsigned int)(X)) | ((unsigned int)(SMASK))) & ~((unsigned int)(CMASK))) )
#endif
#ifndef BITCSET
#define BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned int)(X)) & ~((unsigned int)(CMASK))) | ((unsigned int)(SMASK))) )
#endif
#ifndef BITCLR
#define BITCLR(X, MASK) 			( (X) &= ~((unsigned int)(MASK)) )
#endif
#ifndef BITXOR
#define BITXOR(X, MASK) 			( (X) ^= (unsigned int)(MASK) )
#endif
#ifndef ISZERO
#define ISZERO(X, MASK) 			(  ! (((unsigned int)(X)) & ((unsigned int)(MASK))) )
#endif

#ifndef ISSET
#define	ISSET(X, MASK)				( (unsigned long)(X) & ((unsigned long)(MASK)) )
#endif


#ifndef ENABLE
#define ENABLE 1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef NOCHANGE
#define NOCHANGE 2
#endif

#ifndef ON
#define ON		1
#endif
#ifndef OFF
#define OFF 	0
#endif

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif

#define HwVMT_SZ(X) 							(((X)-1)*Hw12)
	#define SIZE_4GB								32
	#define SIZE_2GB								31
	#define SIZE_1GB								30
	#define SIZE_512MB								29
	#define SIZE_256MB								28
	#define SIZE_128MB								27
	#define SIZE_64MB								26
	#define SIZE_32MB								25
	#define SIZE_16MB								24
	#define SIZE_8MB								23
	#define SIZE_4MB								22
	#define SIZE_2MB								21
	#define SIZE_1MB								20
	#define HwVMT_REGION_AP_ALL 				(Hw11+Hw10)
	#define HwVMT_DOMAIN(X) 					((X)*Hw5)
	#define HwVMT_REGION_EN 					Hw9 							// Region Enable Register
	#define HwVMT_CACHE_ON						Hw3 							// Cacheable Register
	#define HwVMT_CACHE_OFF 					HwZERO
	#define HwVMT_BUFF_ON							Hw2 							// Bufferable Register
	#define HwVMT_BUFF_OFF							HwZERO

	#define HwVMT_REGION0_EN						Hw9 							// Region Enable Register
	#define HwVMT_REGION0_CA						Hw3 							// Cacheable Register
	#define HwVMT_REGION0_BU						Hw2 							// Bufferable Register

/************************************************************************************************
*										 ENUM												   *
************************************************************************************************/
/***************************************CLOCK****************************************************/
	enum
	{
		IDLE_PRIORITY = 0, // Don't Return IDLE_PRIORITY
		LOW_PRIORITY,
		MID_PRIORITY,
		HIGH_PRIORITY,
		MAX_PRIORITY,
	
		CLOCK_PRIORITY_NUM,
	//}stCKC_PRIORITY;
	};

//CKC Enum
	enum{ /* CLKCTRL Clock Source */
		DIRECTPLL0=0,
		DIRECTPLL1,
		DIRECTPLL2,
		DIRECTPLL3,
		DIRECTXIN,
		DIVIDPLL0,
		DIVIDPLL1,
		DIRECTXTIN,
		DIRECTPLL4,
		DIRECTPLL5,
		DIVIDPLL2,
		DIVIDPLL3,
		DIVIDPLL4,
		DIVIDPLL5,
		DIVIDXIN,
		DIVIDXTIN,
	};

	enum {
		FBUS_CPU = 0,	// CLKCTRL0
		FBUS_MEM,		// CLKCTRL1
		FBUS_DDI,		// CLKCTRL2
		FBUS_GPU,		// CLKCTRL3
		FBUS_IO,		// CLKCTRL4
		FBUS_VBUS,		// CLKCTRL5
		FBUS_VCORE,		// CLKCTRL6
		FBUS_HSIO,		// CLKCTRL7
		FBUS_SMU,		// CLKCTRL8
	};

	enum{ /* Peri. Clock Source */
		PCDIRECTPLL0=0,
		PCDIRECTPLL1,
		PCDIRECTPLL2,
		PCDIRECTPLL3,
		PCDIRECTXIN,
		PCDIVIDPLL0,
		PCDIVIDPLL1,
		PCDIVIDPLL2,
		PCDIVIDPLL3,
		PCDIRECTXTIN,
		PCEXITERNAL, // 10
		PCDIVIDXIN_HDMITMDS,
		PCDIVIDXTIN_HDMIPCLK,
		PCHDMI, 	// 27Mhz
		PCSATA, 	// 25Mhz
		PCUSBPHY,	// 48Mhz
		PCDIVIDXIN,
		PCDIVIDXTIN,
		PCDIRECTPLL4,
		PCDIRECTPLL5,
		PCDIVIDPLL4,
		PCDIVIDPLL5,
	};
	
	enum{ /* Peri. Clock Source */
		PDCO = 0,
		PDIVIDER,
	};
	
	enum {/* Peri. Name */
		PERI_TCX = 0,		// 0	// 0x80
		PERI_TCT,
		PERI_TCZ,
		PERI_LCD0,
		PERI_LCDSI0,
		PERI_LCD1,			// 5
		PERI_LCDSI1,
		PERI_RESERVED0,
		PERI_LCDTIMER,
		PERI_JPEG,
		PERI_RESERVED1,		// 10
		PERI_RESERVED2,
		PERI_GMAC,
		PERI_USBOTG,
		PERI_RESERVED3,
		PERI_OUT0,			// 15
		PERI_USB20H,
		PERI_HDMI,
		PERI_HDMIA,
		PERI_OUT1,
		PERI_EHI,			// 20
		PERI_SDMMC0,
		PERI_SDMMC1,
		PERI_SDMMC2,
		PERI_SDMMC3,
		PERI_ADAI1,			// 25
		PERI_ADAM1,
		PERI_SPDIF1,
		PERI_ADAI0,
		PERI_ADAM0,
		PERI_SPDIF0,		// 30
		PERI_PDM,
		PERI_RESERVED4,
		PERI_ADC,
		PERI_I2C0,
		PERI_I2C1,			// 35
		PERI_I2C2,
		PERI_I2C3,
		PERI_UART0,
		PERI_UART1,
		PERI_UART2,			// 40
		PERI_UART3,
		PERI_UART4,
		PERI_UART5,
		PERI_UART6,
		PERI_UART7,			// 45
		PERI_GPSB0,
		PERI_GPSB1,
		PERI_GPSB2,
		PERI_GPSB3,
		PERI_GPSB4,			// 50
		PERI_GPSB5,
		PERI_MAX,
	};
	
	enum {/* clock divider (div+1) */
		CLKDIV0 = 0,
		CLKDIV2 ,
		CLKDIV3 ,
		CLKDIV4 ,
		CLKDIVNONCHANGE,
	};

	enum { /* iobus pwdn/swreset */
		RB_EHI=0,                   //  0
		RB_MPEFEC,                  //  1
		RB_SDMMC0CONTROLLER,        //  2
		RB_SDMMC1CONTROLLER,        //  3
		RB_SDMMC2CONTROLLER,        //  4
		RB_SDMMC3CONTROLLER,        //  5
		RB_SDMMCCHANNELCONTROLLER,  //  6
		RB_DMA0,                    //  7
		RB_DMA1,                    //  8
		RB_DMA2,                    //  9
		RB_DMACONTROLLER,           // 10
		RB_OVERAYMIXER,             // 11
		RB_PWM,                     // 12
		RB_SMC,                     // 13
		RB_RTC=15,                  // 15
		RB_REMOTECONTROLLER,        // 16
		RB_TSADC,                   // 17
		RB_EDICONFIGURATION,        // 18
		RB_EDICONTROLLER,           // 19
		RB_SHOULDBE1,               // 20
		RB_AUDIO0_MCADMA,           // 21
		RB_AUDIO0_MCDAI,            // 22
		RB_AUDIO0_MCSPDIF,          // 23
		RB_AUDIO0_CONTROLLER,       // 24
		RB_AUDIO1_ADMA,             // 25
		RB_AUDIO1_DAI,              // 26
		RB_AUDIO1_SPDIF,            // 27
		RB_AUDIO1_CONTROLLER,       // 28
		RB_I2C_MASTER0,             // 29
		RB_I2C_MASTER1,             // 30
		RB_I2C_MASTER2,             // 31
		RB_I2C_MASTER3,             //  0 32
		RB_I2C_SLAVE0,              //  1 33
		RB_I2C_SLAVE1,              //  2 34
		RB_I2C_CONTROLLER,          //  3 35
		RB_UART0,                   //  4 36
		RB_UART1,                   //  5 37
		RB_UART2,                   //  6 38
		RB_UART3,                   //  7 39
		RB_UART4,                   //  8 40
		RB_UART5,                   //  9 41
		RB_UART6,                   // 10 42
		RB_UART7,                   // 11 43
		RB_UART_CONTROLLER,         // 12 44
		RB_IDE,                     // 13 45
		RB_NFC,                     // 14 46
		RB_TSIF0,                   // 15 47
		RB_TSIF1,                   // 16 48
		RB_TSIF2,                   // 17 49
		RB_TSIF_CONTROLLER,         // 18 50
		RB_GPSB0,                   // 19 51
		RB_GPSB1,                   // 20 52
		RB_GPSB2,                   // 21 53
		RB_GPSB3,                   // 22 54
		RB_GPSB4,                   // 23 55
		RB_GPSB5,                   // 24 56
		RB_GPSB_CONTROLLER,         // 25 57
		RB_USB20OTG,                // 26 58
		RB_MAX,
	};
	
	enum{ /* display bus pwdn/swreset */
		DDIBUS_LCDC = 0,
		DDIBUS_NTSCPAL,
		DDIBUS_HDMI,
		DDIBUS_MAX,
	};

	enum{ /* graphic bus pwdn/swreset */
		GPUBUS_GRP = 0,
		GPUBUS_MAX,
	};

	enum{ /* video bus pwdn/swreset */
		VIDEOBUS_JENC = 0,
		VIDEOBUS_COD = 2,
		VIDEOBUS_BUS,
		VIDEOBUS_MAX,
	};

	enum{ /* high-speed io bus pwdn/swreset */
		HSIOBUS_DMAX = 0,
		HSIOBUS_GMAC_GLOBAL,
		HSIOBUS_GMAC,
		HSIOBUS_USB20H = 6,
		HSIOBUS_SEC_CTRL = 12,
		HSIOBUS_HSIOCFG = 16,
		HSIOBUS_CIPHER = 18,
		HSIOBUS_MAX,
	};

	/*
	 * IP Isolation Control Register (PMU_ISOL: 0x7440005C)
	 */
	enum{                           // bit Name
		PMU_ISOL_OTP = 0,
		PMU_ISOL_RTC,
		PMU_ISOL_PLL,
		PMU_ISOL_ECID,
		PMU_ISOL_HDMI,
		PMU_ISOL_VDAC = 6,
		PMU_ISOL_TSADC,
		PMU_ISOL_USBHP,
		PMU_ISOL_USBOP,
		PMU_ISOL_LVDS = 11,
		PMU_ISOL_MAX
	};

#ifdef __cplusplus
}
#endif

#endif // __GLOBALS_H__
