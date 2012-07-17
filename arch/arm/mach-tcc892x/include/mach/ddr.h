/***************************************************************************************
*	FileName    : ddr2.h
*	Description : TCBOOT DDR Configuration File
****************************************************************************************
*
*	TCC Board Support Package
*	Copyright (c) Telechips, Inc.
*	ALL RIGHTS RESERVED
*
****************************************************************************************/


#ifndef __DDR_CONFIG_H__
#define __DDR_CONFIG_H__

#define DDR3_SETTING
//#define DDR2_SETTING

#if defined(DDR3_SETTING)
/* define */
#define DRAM_DDR3

#define DDR3_800	(800)
#define DDR3_1066	(1066)
#define DDR3_1333	(1333)
#define DDR3_1600	(1600)
#define DDR3_1866	(1866)
#define DDR3_2133	(2133)

enum
{
	AL_DISABLED = 0,
	AL_CL_MINUS_ONE,
	AL_CL_MINUS_TWO
};

enum
{
	SIZE_512MBIT = 0,
	SIZE_1GBIT,
	SIZE_2GBIT,
	SIZE_4GBIT,
	SIZE_8GBIT,
};

enum 
{
	PAGE_1KB = 0,
	PAGE_2KB,
};

	enum
	{
		BL_8 = 0,
		BL_BC4_OR_8,//Not supported by DDR2/DDR3 Controller
		BL_BC4,
	};

	enum
	{
		BT_NIBBLE_SEQUENTIAL = 0,
		BT_INTERLEAVE,
	};

	//Write recovery for autoprecharge in MR0
	enum{
		WR_5 = 1,
		WR_6,
		WR_7,
		WR_8,
		WR_10,
		WR_12
	};

	//DLL Control for Precharge PD
	enum
	{
		SLOW_EXIT = 0,
		FAST_EXIT
	};

/* function */
typedef struct _ddr3_setting_value
{
	unsigned int max_speed;
	unsigned int size;
	unsigned int pagesize;
	unsigned int al;
	unsigned int cl;
	unsigned int clk;
}ddr3_setting_value;

/************ ddr3 setting value **************/
#define CUR_PAGE_SIZE	PAGE_2KB
#define CUR_MAX_SPEED	DDR3_1333
#define CUR_CL			9
#define CUR_AL			AL_DISABLED
#define CUR_SIZE		SIZE_1GBIT

/************ ddr3 controller setting **************/
#define CHIP_LOGICAL_NUM    1 /* 1 or 2 chip */
#define CHIP_LOGICAL_SIZE   256 /* unit is MB */
#define CHIP_COLBITS        10 /* 7~12 bits */
#define CHIP_ROWBITS        13 /* 12~16 bits */
#define CHIP_BANK           8 /* 4 or 8 banks */

/************ ddr3 mrs setting value **************/
#define BURST_LEN			BL_8				
#define READ_BURST_TYPE 	BT_NIBBLE_SEQUENTIAL

/************ ddr3 setting end **************/

#elif defined(DDR2_SETTING)
#define DDR2CONTROLLER_LPCON
/***********************************************************
*  DRAM config
*              CS  BANK   CAS_Latency  CAS/RAS            Size                 MaxClock    PartNumber                       Vendor
*  [DDR2]
*  DRAM_TYPE1 : 1,    2,      5,         10/13      (16bit x  64 x 2) 128MB,     330Mhz     K4T51163QG-HCE6(EVB 0.1)         SAMSUNG
*  DRAM_TYPE2 : 1,    2,      6,         10/13      (16bit x  64 x 4) 128MB,     400Mhz     H5PS5162FPR S2C- 415A (EVB 1.0), HYNIX
*                                                                                           H5PS5162FFR S6C-915A             HYNIX
*  DRAM_TYPE3 : 1,    3,      5,         10/13      (16bit x 128 x 2) 256MB,     400Mhz     K4T1G164QE-HCE7                  SAMSUNG
*                                                                                330Mhz     K4T1G164QQ-HCE6                  SAMSUNG
*                                                                                330Mhz     MT47H64M16-3E                    MICRON
*                                                                                400Mhz     NT5TU64M16DG-AC                  NANYA
*  DRAM_TYPE4 : 1,    3,      6,         10/13      (16bit x 128 x 2) 256MB,     400Mhz     K4T1G1643QG-HCF7                 SAMSUNG
*                                                                                400Mhz     HY5PS1G1631C-FP                  HYNIX
*                                                                                400Mhz     H5PS1G63EFR-S6                   HYNIX
*  DRAM_TYPE10: 1,    3,      6,         10/14      ( 8bit x 128 x 4) 512MB,     330Mhz     EDE1108AEBG                      ELPIDA
*
*  [TEST Only]
*  DRAM_TYPE5 : 1,    2,      5,         10/14      ( 8bit x  64 x 4) 256MB,     330Mhz     E5108AG-6E-E                     ELPIDA
*  DRAM_TYPE6 : 2,    2,      6,         10/13      (16bit x  64 x 4) 256MB,     400Mhz     H5PS5162FFR S6C-915A             HYNIX
*  [MDDR]
*  DRAM_TYPE7 : 1,    2,      3,         10/13      (16bit x  64 x 2) 128MB,     166Mhz     H5MS516DFR J3M-919A              HYNIX
*  DRAM_TYPE8 : 1,    2,      3,         10/14      (16bit x 128 x 2) 256MB,     166Mhz     H5MS1G62MFP J3M-902A             HYNIX
*  DRAM_TYPE9 : 1,    2,      3,          9/13      (32bit x  64 x 1)  64MB,     180Mhz     ECK540ACDCN                      ELPIDA
************************************************************/
/*--------------*
 * DDR Type     *
 *--------------*/
//#define DDR2SCSemicon
#if defined(DDR2SCSemicon)

#define DRAM_DDR2
#define DRAM_SIZE_512
#define DRAM_PHY_MAX_FREQ_400
#define DRAM_PHY_CAS_5
#define DRAM_PHY_BANKBIT_3
#define DRAM_PHY_ROWBITS_14
#define DRAM_PHY_COLBITS_10
#define DRAM_PHY_DATABITS_16
#define DRAM_PHY_SIZE_2048
#define BOARD_DRAM_PHYSICAL_NUM_2
#define BOARD_DRAM_LOGICAL_NUM_1
#define BOARD_DRAM_DATABITS_32

#else

#define DRAM_DDR2
#define DRAM_SIZE_256
#define DRAM_PHY_MAX_FREQ_400
#define DRAM_PHY_CAS_6
#define DRAM_PHY_BANKBIT_3
#define DRAM_PHY_ROWBITS_13
#define DRAM_PHY_COLBITS_10
#define DRAM_PHY_DATABITS_16
#define DRAM_PHY_SIZE_1024
#define BOARD_DRAM_PHYSICAL_NUM_2
#define BOARD_DRAM_LOGICAL_NUM_1
#define BOARD_DRAM_DATABITS_32

#endif

/***************Memory Configuration*************************
*
*************** PHYSICAL ***********************************
*
* //type
* DRAM_DDR2
* DRAM_MDDR
* 
* //max freq.
* DRAM_PHY_MAX_FREQ_400
* DRAM_PHY_MAX_FREQ_330
* DRAM_PHY_MAX_FREQ_200
* DRAM_PHY_MAX_FREQ_185
* DRAM_PHY_MAX_FREQ_166
* 
* //cas latency.
* DRAM_PHY_CAS_3
* DRAM_PHY_CAS_4
* DRAM_PHY_CAS_5
* DRAM_PHY_CAS_6
* 
* //bank bit
* DRAM_PHY_BANKBIT_2
* DRAM_PHY_BANKBIT_3
* 
* //row,col bit
* DRAM_PHY_ROWBITS_13
* DRAM_PHY_ROWBITS_14
* DRAM_PHY_COLBITS_9
* DRAM_PHY_COLBITS_10
* 
* //data adress bit
* DRAM_PHY_DATABITS_16
* DRAM_PHY_DATABITS_32
* 
* //total size - 1EA 
* DRAM_PHY_SIZE_256
* DRAM_PHY_SIZE_512
* DRAM_PHY_SIZE_1024
* 
*************** LOGICAL ***********************************
*
* //# of DRAM
* BOARD_DRAM_PHYSICAL_NUM_1
* BOARD_DRAM_PHYSICAL_NUM_2
* 
* //Data bits using BSP
* BOARD_DRAM_DATABITS_16
* BOARD_DRAM_DATABITS_32
* 
* //#of DRAM memory controller 
* BOARD_DRAM_LOGICAL_NUM_1
* BOARD_DRAM_LOGICAL_NUM_2
*
* //Board DRAM Total size 
* DRAM_SIZE_64
* DRAM_SIZE_128
* DRAM_SIZE_256
* DRAM_SIZE_512
*********************************************************/


/*************************** 
 * TYPE3
 ***************************
 * DRAM_DDR2
 * DRAM_SIZE_256
 * DRAM_PHY_MAX_FREQ_330
 * DRAM_PHY_CAS_5
 * DRAM_PHY_BANKBIT_3
 * DRAM_PHY_ROWBITS_13
 * DRAM_PHY_COLBITS_10
 * DRAM_PHY_DATABITS_16
 * DRAM_PHY_SIZE_1024
 * BOARD_DRAM_PHYSICAL_NUM_2
 * BOARD_DRAM_LOGICAL_NUM_1
 * BOARD_DRAM_DATABITS_32
 **************************/


/*************************** 
 * TYPE4
 ***************************
 * DRAM_DDR2
 * DRAM_SIZE_256
 * DRAM_PHY_MAX_FREQ_400
 * DRAM_PHY_CAS_6
 * DRAM_PHY_BANKBIT_3
 * DRAM_PHY_ROWBITS_13
 * DRAM_PHY_COLBITS_10
 * DRAM_PHY_DATABITS_16
 * DRAM_PHY_SIZE_1024
 * BOARD_DRAM_PHYSICAL_NUM_2
 * BOARD_DRAM_LOGICAL_NUM_1
 * BOARD_DRAM_DATABITS_32
 **************************/


/*************************** 
 * TYPE8
 ***************************
 * DRAM_MDDR
 * DRAM_SIZE_256
 * DRAM_PHY_MAX_FREQ_166
 * DRAM_PHY_CAS_3
 * DRAM_PHY_BANKBIT_2
 * DRAM_PHY_ROWBITS_14
 * DRAM_PHY_COLBITS_10
 * DRAM_PHY_DATABITS_16
 * DRAM_PHY_SIZE_512
 * BOARD_DRAM_PHYSICAL_NUM_2
 * BOARD_DRAM_LOGICAL_NUM_1
 * BOARD_DRAM_DATABITS_32
 **************************/

/*************************** 
 * TYPE2
 ***************************
 * DRAM_DDR2
 * DRAM_SIZE_128
 * DRAM_PHY_MAX_FREQ_400
 * DRAM_PHY_CAS_6
 * DRAM_PHY_BANKBIT_2
 * DRAM_PHY_ROWBITS_13
 * DRAM_PHY_COLBITS_10
 * DRAM_PHY_DATABITS_16
 * DRAM_PHY_SIZE_512
 * BOARD_DRAM_PHYSICAL_NUM_2
 * BOARD_DRAM_LOGICAL_NUM_1
 * BOARD_DRAM_DATABITS_32
 **************************/

/*************************** 
 *16bit
 ***************************
 * DRAM_DDR2
 * DRAM_SIZE_128
 * DRAM_PHY_MAX_FREQ_400
 * DRAM_PHY_CAS_5
 * DRAM_PHY_BANKBIT_3
 * DRAM_PHY_ROWBITS_13
 * DRAM_PHY_COLBITS_10
 * DRAM_PHY_DATABITS_16
 * DRAM_PHY_SIZE_1024
 * BOARD_DRAM_PHYSICAL_NUM_1
 * BOARD_DRAM_LOGICAL_NUM_1
 * BOARD_DRAM_DATABITS_16
 **************************/

/*************************** 
 * TYPE1 zentel
 ***************************
 * DRAM_DDR2
 * DRAM_SIZE_128
 * DRAM_PHY_MAX_FREQ_400
 * DRAM_PHY_CAS_5
 * DRAM_PHY_BANKBIT_2
 * DRAM_PHY_ROWBITS_13
 * DRAM_PHY_COLBITS_10
 * DRAM_PHY_DATABITS_16
 * DRAM_PHY_SIZE_512
 * BOARD_DRAM_PHYSICAL_NUM_2
 * BOARD_DRAM_LOGICAL_NUM_1
 * BOARD_DRAM_DATABITS_32
 **************************/


/*************************** 
 *micron(MT4HTF6464AY-667E1)
 ***************************
 * DRAM_DDR2=1
 * DRAM_SIZE_256=1
 * DRAM_PHY_MAX_FREQ_330=1
 * DRAM_PHY_CAS_5=1
 * DRAM_PHY_BANKBIT_3=1
 * DRAM_PHY_ROWBITS_13=1
 * DRAM_PHY_COLBITS_10=1
 * DRAM_PHY_DATABITS_16=1
 * DRAM_PHY_SIZE_1024=1
 * BOARD_DRAM_PHYSICAL_NUM_2=1
 * BOARD_DRAM_LOGICAL_NUM_1=1
 * BOARD_DRAM_DATABITS_32=1
 **************************/
#else
ddr type is not selected!!!!!!
#endif
#endif	/* __DDR_CONFIG_H__ */
/************* end of file *************************************************************/
