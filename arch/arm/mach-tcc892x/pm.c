/*
 * arch/arm/mach-tcc892x/pm.c
 *
 * Author:  <linux@telechips.com>
 * Created: November, 2011
 * Description: LINUX POWER MANAGEMENT FUNCTIONS
 *
 * Copyright (C) 2011 Telechips
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/suspend.h>
#include <linux/io.h>
#include <linux/reboot.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <asm/tlbflush.h>
#include <linux/syscalls.h>		// sys_sync()
#include <asm/cacheflush.h>		// local_flush_tlb_all(), flush_cache_all();
#include <mach/globals.h>
#include <mach/system.h>
#include <mach/pm.h>
#include <mach/tcc_ddr.h>
#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>
#endif

extern void SRAM_Boot(void);
extern void save_cpu_reg(int sram_addr, unsigned int pReg, void *);
extern void resore_cpu_reg(void);
extern void __cpu_early_init(void);
extern unsigned int IO_ARM_ChangeStackSRAM(void);
extern void IO_ARM_RestoreStackSRAM(unsigned int);

/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define nop_delay(x) for(cnt=0 ; cnt<x ; cnt++){ \
					__asm__ __volatile__ ("nop\n"); }

#define addr_clk(b) (0x74000000+b)
#define addr_mem(b) (0x73000000+b)
#define time2cycle(time, tCK)		((int)((time + tCK - 1) / tCK))

#define denali_ctl(x) (*(volatile unsigned long *)addr_mem(0x500000+(x*4)))
#define denali_phy(x) (*(volatile unsigned long *)addr_mem(0x600000+(x)))
#define ddr_phy(x) (*(volatile unsigned long *)addr_mem(0x420400+(x)))

typedef void (*FuncPtr)(void);

#define L2CACHE_BASE   0xFA000000

#define LOG_NDEBUG 0

#if defined(TCC_PM_REGULATOR_CTRL)
#define addr_iocfg(b) (0x76066000+b)
#define PMIC_PARAM(x)   (*(volatile unsigned long *)(COREPWR_PARAM_ADDR + (4*(x))))
#define SLEEP           0
#define WAKEUP          1

enum {
	DEV_ADDR = 0,   /* i2c device address */
	CORE_CTRL_REG,  /* coreA power control register */
	READ_SIZE,      /* read data size */
	WRITE_SIZE,     /* write data size */
	WRITE_DATA1_S,  /* write data 1 (sleep) */
	WRITE_DATA2_S,  /* write data 2 (sleep) */
	WRITE_DATA1_W,  /* write data 1 (wakeup) */
	WRITE_DATA2_W,  /* write data 2 (wakeup) */
	DEV_CH,
	MACHINE_ID,
	SYSTEM_REV,
};
typedef void (*CoreFuncPtr)(unsigned int goto_state);
#endif

//#define TCC_PM_TP_DEBUG		// Test Port debugging - 120611, hjbae
#if defined(TCC_PM_TP_DEBUG)
#include <mach/gpio.h>

//#define TP_GPIO_PORT	(TCC_GPE(30))	// 0x1007 : UT_RTS2
#define TP_GPIO_PORT	(TCC_GPG(4))	// 0x1008 : SD2_WP
//#define TP_GPIO_PORT	(TCC_GPG(5))
#endif

/*===========================================================================
FUNCTION
===========================================================================*/
#if defined(CONFIG_MACH_TCC8920ST)
static inline void tcc_stb_suspend(void)
{
#if defined(TCC_PM_MEMQ_PWR_CTRL)
#if defined(CONFIG_STB_BOARD_HDB892F)
    BITCLR(((PGPIO)HwGPIO_BASE)->GPFDAT.nREG, 1<<9); //VDDQ_MEM_ON : low
#else
    BITCLR(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<22); //VDDQ_MEM_ON : low
#endif /* CONFIG_STB_BOARD_HDB892F */
#endif    
#if defined(CONFIG_STB_BOARD_HDB892F)
   	BITCLR(((PGPIO)HwGPIO_BASE)->GPFDAT.nREG, 1<<8); //SLEEP_MODE_CTL : low
#else
   	BITCLR(((PGPIO)HwGPIO_BASE)->GPFDAT.nREG, 1<<16); //SLEEP_MODE_CTL : low
#endif /* CONFIG_STB_BOARD_HDB892F */
	BITCLR(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<19); //CORE1_ON : low
	BITCLR(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<21); //CORE2_ON : low
}

static inline void tcc_stb_resume(void)
{
#if defined(TCC_PM_MEMQ_PWR_CTRL)
#if defined(CONFIG_STB_BOARD_HDB892F)
    BITSET(((PGPIO)HwGPIO_BASE)->GPFDAT.nREG, 1<<9); //VDDQ_MEM_ON : high
#else
    BITSET(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<22); //VDDQ_MEM_ON : high
#endif /* CONFIG_STB_BOARD_HDB892F */
#endif        
#if defined(CONFIG_STB_BOARD_HDB892F)
  	BITSET(((PGPIO)HwGPIO_BASE)->GPFDAT.nREG, 1<<8); //SLEEP_MODE_CTL : high
#else
   	BITSET(((PGPIO)HwGPIO_BASE)->GPFDAT.nREG, 1<<16); //SLEEP_MODE_CTL : high
#endif /* CONFIG_STB_BOARD_HDB892F */
	BITSET(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<19); //CORE1_ON : high
	BITSET(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<21); //CORE2_ON : high	
}
#endif

static void sdram_init(void)
{
#if defined(CONFIG_DRAM_DDR3)
	volatile int i;
	register unsigned int tmp;
	uint nCL, nCWL;

	#define PLL5_VALUE      0x01012C03	// PLL5 : 600MHz for CPU
#ifdef DDR3_AXI0_FIFO_1to2_ENABLE
//	#define PLL4_VALUE      0x00012C03	// PLL4 : 1200MHz for MEM
//	#define MEM_CLK         600 // = PLL4/2
//	#define PLL4_VALUE      0x00011303	// PLL4 : 1068MHz for MEM
//	#define MEM_CLK         550 // = PLL4/2
//	#define PLL4_VALUE      0x00010B03	// PLL4 : 1068MHz for MEM
//	#define MEM_CLK         533 // = PLL4/2
	#define PLL4_VALUE      0x01012C03	// PLL4 : 600MHz for MEM
	#define MEM_CLK         300 // = PLL4/2
#else
//	#define PLL4_VALUE      0x41016803 // PLL4 : 720MHz for MEM
//	#define MEM_CLK         360// = PLL4/2
	#define PLL4_VALUE      0x01012C03	// PLL4 : 600MHz for MEM
	#define MEM_CLK         300 // = PLL4/2
#endif
	#define DDR3_CLK        MEM_CLK
	#define tCK (1000000/DDR3_CLK)

//--------------------------------------------------------------------------
//for debugging by j-tag
	*(volatile unsigned long *)0x77000018 = 0xf0000000;

//--------------------------------------------------------------------------
//Change to config mode

//--------------------------------------------------------------------------
//Clock setting..

	//Memory BUS Configuration : MBUSCFG
	#ifdef DDR3_AXI0_FIFO_1to2_ENABLE
	*(volatile unsigned long *)addr_mem(0x810010) |= Hw15; //dclkr=1 : this is used for DDR3 only.
	#else
	*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //(d)not used just for ddr3 clk div
	#endif
	i=3200; while(i--);

	*(volatile unsigned long *)addr_clk(0x000000) = 0x002ffff4; //cpu bus : XIN
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014; //mem bus : XIN/2 
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200014; //io bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200014; //smu bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000044) = PLL5_VALUE; //pll5 for cpu
	*(volatile unsigned long *)addr_clk(0x000044) = 0x80000000 | PLL5_VALUE;
	*(volatile unsigned long *)addr_clk(0x000040) = PLL4_VALUE; //pll4 for mem
	*(volatile unsigned long *)addr_clk(0x000040) = 0x80000000 | PLL4_VALUE;
	i=3200; while(i--);
	*(volatile unsigned long *)addr_clk(0x000000) = 0x002FFFF9;  // cpu
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200018;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200039;  // io bus
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200039;  // smu bus
	i=3200; while(i--);

//--------------------------------------------------------------------------
// Timing Parameter

	if(tCK >= 2500){ /* 2.5ns, 400MHz */
		nCL = 6;
		nCWL = 5;
	}else if(tCK >= 1875){ // 1.875ns, 533..MHz
		nCL = 8;
		nCWL = 6;
	}else if(tCK >= 1500){ // 1.5 ns, 666..MHz
		if(DDR3_MAX_SPEED < DDR3_1600)
			nCL = 9;
		else
			nCL = 10;
		nCWL = 7;
	}else if(tCK >= 1250){ // 1.25 ns, 800MHz
		nCL = 11;
		nCWL = 8;
	}else if(tCK >= 1070){ // 1.07 ns, 933..MHz
		nCL = 13;
		nCWL = 9;
	}else if(tCK >= 935){ // 0.935 ns, 1066..MHz
		nCL = 14;
		nCWL = 10;
	}

	denali_ctl(0) = 0x20410600; //DRAM_CLASS[11:8] = 6:DDR3, 4:DDR2
	denali_ctl(2) = 0x00000006; //TINIT[23:0] = 0x4 -==> 0x6
	denali_ctl(3) = time2cycle(20000000,tCK); //TRST_PWRON = 200us, 7 //Bruce_temp.. ns ?
	denali_ctl(4) = time2cycle(50000000,tCK); //CKE_INACTIVE = 500us, 10 //Bruce_temp.. ns ?

	//TBST_INT_INTERVAL[26:24] = 0x1, WL[20:16], CASLAT_LIN[13:8], CL half-cycle increment = 0, INITAREF[3:0] = 0x8
	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		denali_ctl(5) = (0x1<<24 | (nCWL)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		denali_ctl(5) = (0x1<<24 | (nCWL+nCL-1)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		denali_ctl(5) = (0x1<<24 | (nCWL+nCL-2)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}

	denali_ctl(6) = (time2cycle(DDR3_tRAS_ps,tCK)<<24 | time2cycle(DDR3_tRC_ps,tCK)<<16 | time2cycle(DDR3_tRRD_ps,tCK)<<8 | DDR3_tCCD_ck);
	denali_ctl(7) = (DDR3_tMRD_ck<<24 | time2cycle(DDR3_tRTP_ps,tCK)<<16 | time2cycle(DDR3_tRP_ps,tCK)<<8 | (time2cycle(DDR3_tWTR_ps,tCK)+1));
	denali_ctl(8) = (time2cycle(DDR3_tRAS_MAX_ps,tCK)<<8 | time2cycle(DDR3_tMOD_ps,tCK));
	denali_ctl(9) = ((time2cycle(DDR3_tCKE_ps,tCK)+1)<<8 | time2cycle(DDR3_tCKE_ps,tCK));
	denali_ctl(10) = (time2cycle(DDR3_tWR_ps,tCK)<<24 | time2cycle(DDR3_tRCD_ps,tCK)<<16 | 1<<8 | 1);
	denali_ctl(11) = (1<<24 | DDR3_tDLLK_ck<<8 | (time2cycle(DDR3_tWR_ps,tCK)+nCL));
	denali_ctl(12) = (1<<16 | time2cycle(DDR3_tFAW_ps,tCK)<<8 | 3);
	denali_ctl(13) = time2cycle(DDR3_tRP_ps,tCK)+1;
	BITCSET(denali_ctl(14), 0x03FF0100, (time2cycle(DDR3_tRFC_ps,tCK)<<16 | 1<<8));
	denali_ctl(15) = time2cycle(DDR3_tREFI_ps,tCK);
	denali_ctl(16) = (time2cycle(DDR3_tXPDLL_ps,tCK)<<16 | time2cycle(DDR3_tXP_ps,tCK)); // DDR3 Only
	denali_ctl(17) = (6<<16 | 2); //TXARD[-0] = 0x2, TXARDS[-16] = 0x6 // DDR2 only
	denali_ctl(18) = (time2cycle(DDR3_tXS_ps,tCK)<<16 | DDR3_tXSDLL_ck);
	denali_ctl(19) |= (1<<16); //ENABLE_QUICK_SREFRESH = 0x1
	BITCSET(denali_ctl(20), 0x000F0F00, time2cycle(DDR3_tCKSRX_ps,tCK)<<16 | time2cycle(DDR3_tCKSRE_ps,tCK)<<8);

//--------------------------------------------------------------------------
// MRS Setting

	// MRS0
#if (1) //Bruce_temp_mrs0 FAST_EXIT 맞나?
	tmp = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3);
#else
	tmp = DDR3_BURST_LEN | (DDR3_READ_BURST_TYPE<<3) | (FAST_EXIT<<12);
#endif
	if(nCL < 5)
		tmp |= ((5-4)<<4);
	else if(nCL > 11)
		tmp |= ((11-4)<<4);
	else
		tmp |= ((nCL-4)<<4);

	if(time2cycle(DDR3_tWR_ps,tCK) <= 5) // Write Recovery for autoprecharge
		tmp |= WR_5<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) == 6)
		tmp |= WR_6<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) == 7)
		tmp |= WR_7<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) == 8)
		tmp |= WR_8<<9;
	else if((time2cycle(DDR3_tWR_ps,tCK) == 9) || (time2cycle(DDR3_tWR_ps,tCK) == 10))
		tmp |= WR_10<<9;
	else if(time2cycle(DDR3_tWR_ps,tCK) >= 11)
		tmp |= WR_12<<9;

	tmp	|= (1<<8); // DLL Reset

	denali_ctl(27) = tmp<<8;
	BITCSET(denali_ctl(30), 0x0000FFFF, tmp);

	// MRS1
	BITCSET(denali_ctl(28), 0x0000FFFF, (DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1));
	BITCSET(denali_ctl(30), 0xFFFF0000, ((DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1))<<16);

	// MRS2
	tmp = 0;
	if(nCWL <= 5)
		tmp |= 0;
	else if(nCWL == 6)
		tmp |= 1<<3;
	else if(nCWL == 7)
		tmp |= 2<<3;
	else if(nCWL >= 8)
		tmp |= 3<<3;

	BITCSET(denali_ctl(28), 0xFFFF0000, tmp<<16);
	BITCSET(denali_ctl(31), 0x0000FFFF, tmp);

	// MRS3
	BITCSET(denali_ctl(29), 0xFFFF0000, 0<<16);
	BITCSET(denali_ctl(32), 0x0000FFFF, 0);

//--------------------------------------------------------------------------

	//BIST Start
	BITCLR(denali_ctl(32), 1<<16); //BIST_GO = 0x0
	denali_ctl(33) = (1<<16)|(1<<8); //BIST_ADDR_CHECK[-16] = 0x1, BIST_DATA_CHECK[-8] = 0x1
	denali_ctl(34) = 0; denali_ctl(35) = 0; denali_ctl(36) = 0; denali_ctl(37) = 0;
	//BIST End

	denali_ctl(38) = (DDR3_tZQOPER_ck<<16 | DDR3_tZQINIT_ck);
	denali_ctl(39) = (0x2<<24 | DDR3_tZQCS_ck); //ZQCS, ZQ_ON_SREF_EXIT
	denali_ctl(40) = (0x1<<16 | 0x40); //ZQCS_ROTATE=0x1, REFRESH_PER_ZQ=0x40

	// DRAM Size Setting
	if(DDR3_BANK_NUM == 8) //EIGHT_BANK_MODE
		BITSET(denali_ctl(40), 0x1<<24);
	else
		BITCLR(denali_ctl(40), 0x1<<24);
	denali_ctl(41) = 0xFF<<24|DDR3_APBIT<<16|(11-DDR3_COLBITS)<<8|(16-DDR3_ROWBITS); //ADDR_PINS, COLUMN_SIZE, APREBIT, AGE_COUNT = 0xFF
	denali_ctl(42) = 0x1<<24|0x1<<16|0x1<<8|0xFF; //COMMAND_AGE_CCOUNT = 0xff, ADDR_CMP_EN = 0x1, BANK_SPLIT_EN = 0x1 PLACEMENT_EN = 0x1
	denali_ctl(43) = 0x1<<24|0x1<<16|0x1<<8|0x1; //PRIORITY_EN = 0x1, RW_SAME_EN = 0x1,SWAP_EN = 0x1, SWAP_PORT_RW_SAME_EN = 0x1
#if defined(CONFIG_DRAM_16BIT_USED)
	if(DDR3_LOGICAL_CHIP_NUM == 2)
		denali_ctl(44) = (0x1<<24|0xc<<16|0x3<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
	else
		denali_ctl(44) = (0x1<<24|0xc<<16|0x1<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
#else
	if(DDR3_LOGICAL_CHIP_NUM == 2)
		denali_ctl(44) = (0x0<<24|0xc<<16|0x3<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
	else
		denali_ctl(44) = (0x0<<24|0xc<<16|0x1<<8); //REDUC[24] = DQ 0:32BIT, 1:16BIT
#endif
	denali_ctl(45) = 0x1<<24; //RESYNC_DLL_PER_AREF_EN = 0x1 // Automatic Controller-Initiated Update

	denali_ctl(47) = 0x00020000;

//--------------------------------------------------------------------------
// ODT Setting

	BITCSET(denali_ctl(64), 0x00030000, 0x0<<16); //ODT_RD_MAP_CS0
	if(DDR3_LOGICAL_CHIP_NUM == 2)
		BITCSET(denali_ctl(65), 0x00000003, 0x0<<0); //ODT_RD_MAP_CS1

	if(DDR3_PINMAP_TYPE == 0 || DDR3_PINMAP_TYPE == 1){
		BITCSET(denali_ctl(64), 0x03000000, 0x1<<24); //ODT_WR_MAP_CS0
		if(DDR3_LOGICAL_CHIP_NUM == 2)
			BITCSET(denali_ctl(65), 0x00000300, 0x1<<8); //ODT_WR_MAP_CS1
	} else {
		BITCSET(denali_ctl(64), 0x03000000, 0x3<<24); //ODT_WR_MAP_CS0
		if(DDR3_LOGICAL_CHIP_NUM == 2)
			BITCSET(denali_ctl(65), 0x00000300, 0x3<<8); //ODT_WR_MAP_CS1
	}
	BITCSET(denali_ctl(65), 0x000F0000, 0x2<<16); //ADD_ODT_CLK_R2W_SAMECS = 0x2
	denali_ctl(66) = 0x1<<24|0x1<<16|0x2<<8|0x2; //ADD_ODT_CLK_DIFFTYPE_DIFFCS = 0x2, ADD_ODT_CLK_SAMETYPE_DIFFCS = 0x2, R2R_DIFFCS_DLY = 0x1, R2W_DIFFCS_DLY = 0x1
	denali_ctl(67) = 0x2<<24|0x0<<16|0x1<<8|0x1; //W2R_DIFFCS_DLY = 0x1, W2W_DIFFCS_DLY = 0x1, R2R_SAMECS_DLY = 0x0, R2W_SAMECS_DLY = 0x2
	BITCSET(denali_ctl(68), 0x00000707, 0x0<<8|0x0); //W2R_SAMECS_DLY = 0x0, W2W_SAMECS_DLY = 0x0
	denali_ctl(72) = 0x0<<16|0x28<<8|0x19; //WLDQSEN = 0x19, WLMRD = 0x28, WRLVL_EN = 0x0
	denali_ctl(81) = 0x21<<16|0x21; //RDLVL_DELAY_0 = 0x21, RDLVL_GATE_DELAY_0 = 0x21
	denali_ctl(84) = 0x2121<<8; //RDLVL_DELAY_1 = 0x2121
	denali_ctl(88) = 0x2121; //RDLVL_DELAY_2 = 0x2121
	denali_ctl(91) = 0x2121<<8; //RDLVL_DELAY_3 = 0x2121
	denali_ctl(92) = 0xFFFF<<16; //AXI0_EN_SIZE_LT_WIDTH_INSTR = 0xffff
	denali_ctl(93) = 0x8<<8|0x8; //AXI0_R_PRIORITY = 0x8. AXI0_W_PRIORITY = 0x8
	#ifdef DDR3_AXI0_FIFO_1to2_ENABLE // AXI : MEM Rating Setting, 0 = Async, 1 = 2:1 ( Port : Core ), 2 = 1:2 ( Port : Core ), 3 = Sync.
		BITCSET(denali_ctl(93), 0x00030000, 0x3<<16); //AXI0_FIFO_TYPE_REG[17:16] = 0x3
	#else
		BITCSET(denali_ctl(93), 0x00030000, 0x1<<16); //AXI0_FIFO_TYPE_REG[17:16] = 0x1
	#endif

//--------------------------------------------------------------------------
// DFI Timing

	// tPHY_RDLAT     : DFI_RDDATA_EN <=> DFI_RDDATA_VALID 
	// tPHY_WRLAT(RO) : Write Command <=> DFI_WRDATA_EN    : WRLAT_ADJ(min. 1) + REG_DIMM_ENABLE - 1
	// tRDDATA_EN     : Read Command  <=> DFI_RDDATA_EN    : RDLAT_ADJ(min. 2) + REG_DIMM_ENABLE - 1
	// tPHY_WRDATA    : DFI_WRDATA    <=> DFI_WRDATA_EN
	BITCSET(denali_ctl(95), 0x3F000000, 0x8<<24); //TDFI_PHY_RDLAT = 0x8
	BITCLR(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x0
	denali_ctl(97) = 0x200<<16|0x50E; //TDFI_CTRLUPD_MAX = 0x50e, TDFI_PHYUPD_TYPE0 = 0x200
	denali_ctl(98) = 0x200<<16|0x200; //TDFI_PHYUPD_TYPE1 = 0x200, TDFI_PHYUPD_TYPE2 = 0x200
	denali_ctl(99) = 0x50E<<16|0x200; //TDFI_PHYUPD_TYPE3 = 0x200, TDFI_PHYUPD_RESP = 0x50e
	denali_ctl(100) = 0x1946; //TDFI_CTRLUPD_INTERVAL = 0x1946
	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		denali_ctl(101) = (0x1<<24|0x1<<16|nCWL<<8|(nCL+5));
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		denali_ctl(101) = (0x1<<24|0x1<<16|(nCWL+nCL-1)<<8|(nCL+5));
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		denali_ctl(101) = (0x1<<24|0x1<<16|(nCWL+nCL-2)<<8|(nCL+5));
	}
	denali_ctl(102) = 0x3<<24|0x8000<<8|0x1; //TDFI_DRAM_CLK_ENABLE = 0x1, DFI_WRLVL_MAX_DELAY = 0x8000, TDFI_WRLVL_EN = 0x3
	denali_ctl(103) = 0x4<<16|0x7<<8|0x3; //TDFI_WRLVL_DLL = 0x3, TDFI_WRLVL_LOAD = 0x7, TDFI_WRLVL_RESPLAT = 0x4
	denali_ctl(104) = 0xA; //TDFI_WRLVL_WW = 0xa
	denali_ctl(107) = 0x10<<16|0xFFFF; //RDLVL_MAX_DELAY = 0xffff, RDLVL_GATE_MAX_DELAY = 0x10
	denali_ctl(108) = 0x12<<24|0x7<<16|0x3<<8|0x3; //TDFI_RDLVL_EN = 0x3, TDFI_RDLVL_DLL = 0x3, TDFI_RDLVL_LOAD = 0x7, TDFI_RDLVL_RESPLAT = 0x12 
	denali_ctl(109) = 0xF; //TDFI_RDLVL_RR = 0xf
	denali_ctl(115) = 0x2<<8|0x4; //RDLVL_DQ_ZERO_COUNT = 0x4, RDLVL_GATE_DQ_ZERO_COUNT = 0x2
	denali_ctl(118) = 1; //ODT_ALT_EN = 0x1

//--------------------------------------------------------------------------
// Denali phy setting

	denali_phy(0x10) = 0x10680000; //Bruce_temp, 현재 ZQ Cal 동작 안되서 강제로 focing한 값을 사용함
	denali_phy(0x0C) = 0x01801010;
	denali_phy(0x1C) = 0x00316000|(MEMCTRL_TERM<<8)|(MEMCTRL_DDS<<4); //Bruce_temp, zq forcing valu (일단 이값을 사용해야함)

//--------------------------------------------------------------------------
// DDR phy setting

	//PHY Mode contrl register (PHYMDCFG0)
	ddr_phy(0x0000) = (0 << 21) |    //ctrl_lat                  //Bruce_temp_8920 ??
	                  (0x1f << 16) | //ctrl_wake_up
	                  (DDR3_PINMAP_TYPE << 14) |    //pin_map
	                  ((DDR3_LOGICAL_CHIP_NUM-1) << 13) | //cs2s
	                  (0 << 4) |     //mode                        //Bruce_temp_8920 ??
	                  (0 << 3) |     //c_type : denali=0, lpcon=1
	                  (1);           //dram_type : ddr2=0, ddr3=1, lpddr=2, lpddr2=3

//--------------------------------------------------------------------------
// Start denali MC init
	BITSET(denali_ctl(0), 1); //START = 1
	if(denali_ctl(46)&(1<<5))
	{
		BITSET(denali_ctl(47),(1<<5));
		while(denali_ctl(46)&(1<<5));
	}
	BITCLR(denali_phy(0x08),1<<1); //ctrl_dll_on_rst = 0
	for (i=0; i<5; i++);
	BITSET(denali_phy(0x08),1<<1); //ctrl_dll_on_rst = 1
	while(!(denali_phy(0x28)&(1<<12))); //ctrl_zq_end, ZQ End ...
	while(!(denali_phy(0x24)&(1<<10))); //ctrl_locked, DLL Locked ...
	while(!(denali_ctl(46)&0x20));

	//denali_ctl(47) |= 0x20;
	denali_phy(0x1C) = 0x00316000|(MEMCTRL_TERM<<8)|(MEMCTRL_DDS<<4);
	denali_phy(0x14) = 0x0A0A0A0A;		// Denali Phy Control Reg_02

//--------------------------------------------------------------------------

#ifdef TCC_PM_SSTLIO_CTRL
//--------------------------------------------------------------------------
//enter self-refresh

	BITSET(denali_phy(0x34), 0x1<<10); //lp_ext_req=1
	while(!(denali_phy(0x34)&(0x1<<27))); //until lp_ext_ack==1
	BITCSET(denali_phy(0x34), 0x000000FF, (2<<2)|(1<<1)|(0));
	BITSET(denali_phy(0x34), 0x1<<8); //lp_ext_cmd_strb=1
	while((denali_phy(0x34)&(0x007F0000)) !=0x00450000); //until lp_ext_state==0x45 : check self-refresh state
	BITCLR(denali_phy(0x34), 0x1<<8); //lp_ext_cmd_strb=0
	BITCLR(denali_phy(0x34), 0x1<<10); //lp_ext_req=0
	while(denali_phy(0x34)&(0x1<<27)); //until lp_ext_ack==0
	BITSET(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x3
	BITCLR(denali_ctl(0),0x1); //START[0] = 0
	BITSET(denali_phy(0x0C), 0x1<<22); //ctrl_cmosrcv[22] = 0x1
	BITCSET(denali_phy(0x0C), 0x001F0000, 0x1F<<16); //ctrl_pd[20:16] = 0x1f
	BITCSET(denali_phy(0x20), 0x000000FF, 0xF<<4|0xF); //ctrl_pulld_dq[7:4] = 0xf, ctrl_pulld_dqs[3:0] = 0xf

// -------------------------------------------------------------------------
// SSTL Retention Disable

	while(!(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8))){
		BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode disable=1
	}

//--------------------------------------------------------------------------
// Exit self-refresh

	BITCLR(denali_phy(0x0C), 0x1<<22); //ctrl_cmosrcv[22] = 0x0
	BITCLR(denali_phy(0x0C), 0x001F0000); //ctrl_pd[20:16] = 0x0
	BITCLR(denali_phy(0x20), 0x000000FF); //ctrl_pulld_dq[7:4] = 0x0, ctrl_pulld_dqs[3:0] = 0x0
	while(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8)){
		BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode =0
	}
	*(volatile unsigned long *)addr_mem(0x810004) &= ~(1<<2); //PHY=0
	*(volatile unsigned long *)addr_mem(0x810004) |= (1<<2); //PHY=1
	while(!(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8))){
		BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode disable=1
	}
	BITCLR(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x0
	BITSET(denali_ctl(0),0x1); //START[0] = 1
//	while(!(denali_ctl(46)&(0x20000)));
//	BITSET(denali_ctl(47), 0x20000);
	BITCSET(denali_ctl(20), 0xFF000000, ((2<<2)|(0<<1)|(1))<<24);
	while(!(denali_ctl(46)&(0x40)));
	BITSET(denali_ctl(47), 0x40);
#endif

//--------------------------------------------------------------------------

#elif defined(CONFIG_DRAM_DDR2)

	volatile int i;
	register unsigned int tmp;

	#define PLL5_VALUE      0x01012C03	// PLL5 : 600MHz for CPU
	//#define PLL4_VALUE      0x4201A403 //for 210 //0x01012C03	// PLL4 : 600MHz for MEM
	//#define MEM_CLK         210//300 // = PLL4/2
	#define PLL4_VALUE      0x01012C03	// PLL4 : 600MHz for MEM
	#define MEM_CLK         300 // = PLL4/2

	#define DDR2_CLK        MEM_CLK
	#define tCK (1000000/DDR2_CLK)

//--------------------------------------------------------------------------
//for debugging by j-tag
	*(volatile unsigned long *)0x77000018 = 0xf0000000;

//--------------------------------------------------------------------------
//Change to config mode

	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) &= ~Hw17; //creq2=0 : enter low power
	while((*(volatile unsigned long *)addr_mem(0x410020))&Hw24); //cack2==0 : wait for acknowledgement to request..

#if (0)
	//MEMCTRL
	//*(volatile unsigned long *)addr_mem(0x430004) |= 0x00000001; //clk_stop_en=1

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}
#endif

//--------------------------------------------------------------------------
//Clock setting..

	//Memory BUS Configuration : MBUSCFG
	*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //dclkr=0 : this is used for DDR3 only.
	i=3200; while(i--);

	*(volatile unsigned long *)addr_clk(0x000000) = 0x002ffff4; //cpu bus : XIN
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014; //mem bus : XIN/2 
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200014; //io bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200014; //smu bus : XIN/2
	*(volatile unsigned long *)addr_clk(0x000044) = PLL5_VALUE; //pll5 for cpu
	*(volatile unsigned long *)addr_clk(0x000044) = 0x80000000 | PLL5_VALUE;
	*(volatile unsigned long *)addr_clk(0x000040) = PLL4_VALUE; //pll4 for mem
	*(volatile unsigned long *)addr_clk(0x000040) = 0x80000000 | PLL4_VALUE;
	i=3200; while(i--);
	*(volatile unsigned long *)addr_clk(0x000000) = 0x002FFFF9;  // cpu
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200018;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000010) = 0x00200039;  // io bus
	*(volatile unsigned long *)addr_clk(0x000020) = 0x00200039;  // smu bus
	i=3200; while(i--);


//--------------------------------------------------------------------------
// Controller setting

	//PHY Mode contrl register (PHYMDCFG0)
	*(volatile unsigned long *)addr_mem(0x420400) = (0 << 21) | //ctrl_lat                  //Bruce_temp_8920 ??
	                                             (0x1f << 16) | //ctrl_wake_up
	                                             (DDR2_PINMAP_TYPE << 14) | //pin_map
	                                             ((DDR2_LOGICAL_CHIP_NUM-1) << 13) | //cs2s
	                                             (0 << 4) | //mode                        //Bruce_temp_8920 ??
	                                             (1 << 3) | //c_type : denali=0, lpcon=1
	                                             (0); //dram_type : ddr2=0, ddr3=1, lpddr=2, lpddr2=3



	//PhyZQControl
	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL ;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 ;//zq start
	} else {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw0;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 | Hw0 ;//zq start
	}
	while (((*(volatile unsigned long *)addr_mem(0x430040)) & (0x10000)) != 0x10000);	// Wait until ZQ End

	if (DDR2_CLK >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL ;
	} else {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw0;
	}

	*(volatile unsigned long *)addr_mem(0x430018 ) = 0x0010100A; //PHY Control0
	*(volatile unsigned long *)addr_mem(0x43001C ) = 0xE0000086; //PHY Control1 // modify by crony : [31:30] : ODT Enable for Write and Read
	*(volatile unsigned long *)addr_mem(0x430020 ) = 0x00000000; //PHY Control2
	*(volatile unsigned long *)addr_mem(0x430024 ) = 0x00000000; //PHY Control3
	*(volatile unsigned long *)addr_mem(0x430018 ) = 0x0010100B; //PHY Control0

	while (((*(volatile unsigned long *)addr_mem(0x430040)) & (0x7)) != 0x7);// Wait until FLOCK == 1

	//PHY Control1
	*(volatile unsigned long *)addr_mem(0x43001C) = 0xE000008E; //resync = 1
	*(volatile unsigned long *)addr_mem(0x43001C) = 0xE0000086; //resync = 0

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr_mem(0x430000 ) = 0x30FF2018;

	//MEMCTRL
	*(volatile unsigned long *)addr_mem(0x430004 ) = (0x2 << 20) |
	                                             ((DDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((DDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x4 << 8) |
	                                             (0x0 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);

	//MEMCHIP0
	*(volatile unsigned long *)addr_mem(0x430008 ) = (0x80<<24) |
	                                             ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((DDR2_COLBITS - 7)<<8) |
	                                             ((DDR2_ROWBITS - 12)<<4) |
	                                             DDR2_BANK_BITS;

	//MEMCHIP1
	if(DDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr_mem(0x43000C ) = ((0x80 + DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((DDR2_COLBITS - 7)<<8) |
		                                         ((DDR2_ROWBITS - 12)<<4) |
		                                         DDR2_BANK_BITS;

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr_mem(0x430014 ) = 0x0; //PRECONFIG
	*(volatile unsigned long *)addr_mem(0x430028 ) = 0xFFFF00FF; //PRECONFIG

//--------------------------------------------------------------------------
// Timing parameter setting.

	//T_REFI
	*(volatile unsigned long *)addr_mem(0x430030 ) = time2cycle(DDR2_tREFI_ps, tCK);

	//TROW
	*(volatile unsigned long *)addr_mem(0x430034 ) = time2cycle(DDR2_tRAS_ps, tCK); //tRAS
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (time2cycle(DDR2_tRC_ps, tCK)<<6); //tRC
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (DDR2_CL<<12); //tRCD
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (DDR2_CL<<16); //tRP
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (time2cycle(DDR2_tRRD_ps, tCK)<<20); //tRRD
	*(volatile unsigned long *)addr_mem(0x430034 ) |= (time2cycle(DDR2_tRFC_ps, tCK)<<24); //tRFC

	//TDATA
	*(volatile unsigned long *)addr_mem(0x430038 ) = DDR2_CL; //tRL
	*(volatile unsigned long *)addr_mem(0x430038 ) |= ((DDR2_CL-1)<<8); //tWL
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (DDR2_CL<<16); //tCL
	tmp = time2cycle(DDR2_tRTP_ps, tCK);
	if(tmp<DDR2_tRTP_ck) tmp=DDR2_tRTP_ck;
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (tmp<<20); //tRTP
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (time2cycle(DDR2_tWR_ps, tCK)<<24); //tWR
	tmp = time2cycle(DDR2_tWTR_ps, tCK);
	if(tmp<DDR2_tWTR_ck) tmp=DDR2_tWTR_ck;
	*(volatile unsigned long *)addr_mem(0x430038 ) |= (tmp<<28); //tWTR

	//TPOWER
	*(volatile unsigned long *)addr_mem(0x43003C ) = DDR2_tMRD_ck; //tMRD
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (DDR2_tCKE_ck<<4); //tCKE
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (DDR2_tXP_ck<<8); //tXP
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (DDR2_tXSR_ck<<16); //tXSR
	*(volatile unsigned long *)addr_mem(0x43003C ) |= (time2cycle(DDR2_tFAW_ps, tCK)<<24); //tFAW

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x07000000;//NOP
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00020000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00030000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000100;
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000000;	// DLL reset release.
	*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9));
	i = 100; while(i--);
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380; // OCD Calibration default
	i = 100; while(i--);
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1);

	if(DDR2_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x07000000 | Hw20;//NOP
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00020000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00030000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000100 | Hw20;
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00000000 | Hw20;	// DLL reset release.
		*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(DDR2_CL<<4)|((time2cycle(DDR2_tWR_ps, tCK)-1)<<9)) | Hw20;
		i = 100; while(i--);
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380 | Hw20; // OCD Calibration default
		i = 100; while(i--);
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1) | Hw20;
	}

//--------------------------------------------------------------------------

	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) |= Hw17; //creq2=1 : exit low power
	while(!((*(volatile unsigned long *)addr_mem(0x410020))&Hw24)); //cack2==1 : wait for acknowledgement to request..

	*(volatile unsigned long *)addr_mem(0x430000 ) |= 0x00000020; //CONCONTROL : aref_en=1

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr_mem(0x430048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr_mem(0x43004C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}

//--------------------------------------------------------------------------
#endif
}

#if defined(TCC_PM_REGULATOR_CTRL)
#define I2C_WR				0
#define I2C_RD				1
#define I2C_WAIT_INTR_CNT	10
#define I2C_PRES_REG		*(volatile unsigned long *)0x76300000
#define I2C_CTRL_REG		*(volatile unsigned long *)0x76300004
#define I2C_TXR_REG			*(volatile unsigned long *)0x76300008
#define I2C_CMD_REG			*(volatile unsigned long *)0x7630000C
#define I2C_RXR_REG			*(volatile unsigned long *)0x76300010
#define I2C_SR_REG			*(volatile unsigned long *)0x76300014

/*===========================================================================
FUNCTION
===========================================================================*/
static void set_core_voltage(unsigned int state)
{
	unsigned long slave_addr = PMIC_PARAM(DEV_ADDR);
	unsigned long reg_addr;
	unsigned long buf[2];
	unsigned long backup_reg[10];

	int cnt;
	unsigned int i = 0;

	if (PMIC_PARAM(DEV_ADDR) == 0)
		return;

	/* i2c_set_clock */
	backup_reg[0] = *(volatile unsigned long *)addr_clk(0x0000);	// CPU
	backup_reg[1] = *(volatile unsigned long *)addr_clk(0x0010);	// IO Bus
	backup_reg[2] = *(volatile unsigned long *)addr_clk(0x001C);	// SMU Bus
	backup_reg[3] = *(volatile unsigned long *)addr_clk(0x00b8);	// I2C Master1 Peri
	backup_reg[4] = *(volatile unsigned long *)addr_iocfg(0x10);	// IOCFG_PWR0
	backup_reg[5] = *(volatile unsigned long *)addr_iocfg(0x14);	// IOCFG_PWR1
	backup_reg[6] = *(volatile unsigned long *)addr_iocfg(0x18);	// IOCFG_RST0
	backup_reg[7] = *(volatile unsigned long *)addr_iocfg(0x1C);	// IOCFG_RST1
	backup_reg[8] = I2C_PRES_REG;
	backup_reg[9] = I2C_CTRL_REG;

	*(volatile unsigned long *)addr_clk(0x0000)  = 0x002FFFF4;	// CPU Clock
	*(volatile unsigned long *)addr_clk(0x0010)  = 0x00200014;	// io clk set to 6MHz Xin
	*(volatile unsigned long *)addr_clk(0x001C)  = 0x00200014;	// SMU Bus
//	*(volatile unsigned long *)addr_iocfg(0x1C) &= ~Hw3;		// I2C IF S/W RESET
	*(volatile unsigned long *)addr_iocfg(0x14) |= Hw3;			// I2C IF HCLK
	*(volatile unsigned long *)addr_iocfg(0x1C) |= Hw3;			// I2C IF S/W RESET
//	*(volatile unsigned long *)addr_iocfg(0x18) &= ~Hw29;		// I2C_0 S/W RESET
	*(volatile unsigned long *)addr_iocfg(0x10) |= Hw29;		// I2C_0 HCLK
	*(volatile unsigned long *)addr_clk(0x0108)  = 0x2400000B;	// PCLK 1MHz
	*(volatile unsigned long *)addr_iocfg(0x18) |= Hw29;		// I2C_0 S/W RESET

	if (PMIC_PARAM(DEV_CH) == 0) {
#if defined(CONFIG_MACH_TCC8920ST)
		//I2C[26] - GPIOG[18][19]
		//i2c_portcfg->PCFG0.bREG.MASTER0 = 26;
		BITCSET(*(volatile unsigned long *)(0x742001B8), 0x0000FF00, 0x00004400);
		BITCSET(*(volatile unsigned long *)(0x76360000), 0x000000FF, 0x0000001A);
#elif defined(CONFIG_MACH_M805_892X)
		if (PMIC_PARAM(SYSTEM_REV) == 0x2002 || PMIC_PARAM(SYSTEM_REV) == 0x2003 || PMIC_PARAM(SYSTEM_REV) == 0x2004 || PMIC_PARAM(SYSTEM_REV) == 0x2005) {
			//I2C[12] - GPIOC[2][3]
			//i2c_portcfg->PCFG0.bREG.MASTER0 = 12;
			BITCSET(*(volatile unsigned long *)(0x742000B0), 0x0000FF00, 0x00007700);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x000000FF, 0x0000000C);
		}
		else {
	#if defined(CONFIG_M805S_8923_0XA)
			//I2C[22] - GPIOG[2][3]
			//i2c_portcfg->PCFG0.bREG.MASTER0 = 22;
			BITCSET(*(volatile unsigned long *)(0x742001B0), 0x0000FF00, 0x00004400);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x000000FF, 0x00000016);
	#else
			//I2C[23] - GPIOG[6][7]
			//i2c_portcfg->PCFG0.bREG.MASTER0 = 23;
			BITCSET(*(volatile unsigned long *)(0x742001B0), 0x44000000, 0x44000000);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x000000FF, 0x00000017);
	#endif
		}
#else
		if(PMIC_PARAM(SYSTEM_REV) == 0x1006) {
			//I2C[18] - GPIOF[13][14]
			//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER0 = 18;
			BITCSET(*(volatile unsigned long *)(0x74200174), 0x0FF00000, 0x0AA00000);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x000000FF, 0x00000012);
		}
		else if(PMIC_PARAM(SYSTEM_REV) == 0x1008) {
			//I2C[22] - GPIOG[2][3]
			//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER0 = 22;
			BITCSET(*(volatile unsigned long *)(0x742001B0), 0x0000FF00, 0x00004400);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x000000FF, 0x00000016);
		}
		else {
			//I2C[8] - GPIOB[9][10]
			//i2c_portcfg->PCFG0.bREG.MASTER0 = 8;
			BITCSET(*(volatile unsigned long *)(0x74200074), 0x00000FF0, 0x00000BB0);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x000000FF, 0x00000008);
		}
#endif
	}
	else if (PMIC_PARAM(DEV_CH) == 1) {
#if defined(CONFIG_MACH_M805_892X)
		if (PMIC_PARAM(SYSTEM_REV) == 0x2002 || PMIC_PARAM(SYSTEM_REV) == 0x2003 || PMIC_PARAM(SYSTEM_REV) == 0x2004 || PMIC_PARAM(SYSTEM_REV) == 0x2005) {
			//I2C[25] - GPIOG[12][13]
			//i2c_portcfg->PCFG0.bREG.MASTER1 = 25;
			BITCSET(*(volatile unsigned long *)(0x742001B4), 0x00FF0000, 0x00440000);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x0000FF00, 0x00001900);
		}
		else {
	#if defined(CONFIG_M805S_8923_0XA)
			//I2C[21] - GPIOF[27][28]
			//i2c_portcfg->PCFG0.bREG.MASTER1 = 21;
			BITCSET(*(volatile unsigned long *)(0x7420017C), 0x000FF000, 0x000AA000);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x0000FF00, 0x00001500);
	#else
			//I2C[24] - GPIOG[10][11]
			//i2c_portcfg->PCFG0.bREG.MASTER1 = 24;
			BITCSET(*(volatile unsigned long *)(0x742001B4), 0x0000FF00, 0x00004400);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x0000FF00, 0x00001800);
	#endif
		}
#else
		if(PMIC_PARAM(SYSTEM_REV) == 0x1005 || PMIC_PARAM(SYSTEM_REV) == 0x1007) {
			//I2C[21] - GPIOF[27][28]
			//i2c_portcfg->PCFG0.bREG.MASTER1 = 21;
			BITCSET(*(volatile unsigned long *)(0x7420017C), 0x000FF000, 0x000AA000);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x0000FF00, 0x00001500);
		}
		else if(PMIC_PARAM(SYSTEM_REV) == 0x1006) {
			//I2C[28] - GPIO_ADC[2][3]
			//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER1 = 28;
			BITCSET(*(volatile unsigned long *)(0x7420023C), 0x0000FF00, 0x00003300);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x0000FF00, 0x00001C00);
		}
		else if(PMIC_PARAM(SYSTEM_REV) == 0x1008) {
			//I2C[13] - GPIOC[12][13]
			//((PI2CPORTCFG)HwI2C_PORTCFG_BASE)->PCFG0.bREG.MASTER1 = 13;
			BITCSET(*(volatile unsigned long *)(0x742000B4), 0x00FF0000, 0x00770000);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x0000FF00, 0x00000D00);
		}
		else {
			//I2C[22] - GPIOG[2][3]
			//i2c_portcfg->PCFG0.bREG.MASTER1 = 22;
			BITCSET(*(volatile unsigned long *)(0x742001B0), 0x0000FF00, 0x00004400);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x0000FF00, 0x00001600);
		}
#endif
	}
	else if (PMIC_PARAM(DEV_CH) == 2) {
#if defined(CONFIG_MACH_M805_892X)
		if (PMIC_PARAM(SYSTEM_REV) == 0x2002 || PMIC_PARAM(SYSTEM_REV) == 0x2003 || PMIC_PARAM(SYSTEM_REV) == 0x2004 || PMIC_PARAM(SYSTEM_REV) == 0x2005) {
			//I2C[18] - GPIOF[13][14]
			//i2c_portcfg->PCFG0.bREG.MASTER1 = 18;
			BITCSET(*(volatile unsigned long *)(0x74200178), 0x0FF00000, 0x0AA00000);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x00FF0000, 0x00120000);
		}
#else
		if(PMIC_PARAM(SYSTEM_REV) != 0x1006 && PMIC_PARAM(SYSTEM_REV) != 0x1008) {
			//I2C[28] - GPIO_ADC[2][3]
			//i2c_portcfg->PCFG0.bREG.MASTER2 = 28;
			BITCSET(*(volatile unsigned long *)(0x74200230), 0x0000FF00, 0x00003300);
			BITCSET(*(volatile unsigned long *)(0x76360000), 0x00FF0000, 0x001C0000);
		}
#endif
	}
	else if (PMIC_PARAM(DEV_CH) == 3) {
		//I2C[27] - GPIO_HDMI[2][3]
		//i2c_portcfg->PCFG0.bREG.MASTER3 = 27;
		BITCSET(*(volatile unsigned long *)(0x742001F0), 0x0000FF00, 0x00001100);
		BITCSET(*(volatile unsigned long *)(0x76360000), 0xFF000000, 0x1B000000);
		//Not used..
	}
	else
		goto set_core_voltage_end;

	/* i2c init */
	I2C_PRES_REG = 0x00000001;
	I2C_CTRL_REG = 0x000000C0;
	I2C_CMD_REG |= Hw0;	/* clear pending interrupt */

	/* init. values */
	reg_addr = PMIC_PARAM(CORE_CTRL_REG);
	buf[0] = 0;
	buf[1] = 0;

	/* READ */
	if (PMIC_PARAM(READ_SIZE)) {
		/* write dev_addr, reg_addr */
		I2C_TXR_REG = slave_addr | I2C_WR;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_TXR_REG = (reg_addr & 0xFF);
		I2C_CMD_REG = Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		/* read_data */

		I2C_TXR_REG = slave_addr | I2C_RD;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		for (i = 0; i < PMIC_PARAM(READ_SIZE); i++) {
			I2C_CMD_REG = Hw5 | Hw3;

			cnt = I2C_WAIT_INTR_CNT;
			while(cnt--) {
				if (I2C_SR_REG & Hw0) {
					if (!(I2C_SR_REG & Hw7))
						break;
				}
			}

			if (cnt == 0) goto set_core_voltage_end;
			BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

			buf[i] = I2C_RXR_REG;
		}

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
	}

	if (PMIC_PARAM(WRITE_SIZE)) {
		if (state == SLEEP) {
			buf[0] = (unsigned long)PMIC_PARAM(WRITE_DATA1_S);
			buf[1] = (unsigned long)PMIC_PARAM(WRITE_DATA2_S);
		}
		else {
			buf[0] = (unsigned long)PMIC_PARAM(WRITE_DATA1_W);
			buf[1] = (unsigned long)PMIC_PARAM(WRITE_DATA2_W);
		}

		/* WRITE */
		I2C_TXR_REG = slave_addr | I2C_WR;
		I2C_CMD_REG = Hw7 | Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		I2C_TXR_REG = (reg_addr & 0xFF);
		I2C_CMD_REG = Hw4;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt

		for (i = 0; i < PMIC_PARAM(WRITE_SIZE); i++) {
			I2C_TXR_REG = (buf[i] & 0xFF);
			I2C_CMD_REG = Hw4;

			cnt = I2C_WAIT_INTR_CNT;
			while(cnt--) {
				if (I2C_SR_REG & Hw0) {
					if (!(I2C_SR_REG & Hw7))
						break;
				}
			}

			if (cnt == 0) goto set_core_voltage_end;
			BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
		}

		I2C_CMD_REG = Hw6;

		cnt = I2C_WAIT_INTR_CNT;
		while(cnt--) {
			if (I2C_SR_REG & Hw0) {
				if (!(I2C_SR_REG & Hw7))
					break;
			}
		}

		if (cnt == 0) goto set_core_voltage_end;
		BITSET(I2C_CMD_REG, Hw0); //Clear a pending interrupt
	}

set_core_voltage_end:
	I2C_PRES_REG = backup_reg[9];
	I2C_CTRL_REG = backup_reg[8];
	*(volatile unsigned long *)addr_iocfg(0x1C) = backup_reg[7];
	*(volatile unsigned long *)addr_iocfg(0x18) = backup_reg[6];
	*(volatile unsigned long *)addr_iocfg(0x14) = backup_reg[5];
	*(volatile unsigned long *)addr_iocfg(0x10) = backup_reg[4];
	*(volatile unsigned long *)addr_clk(0x00b8) = backup_reg[3];
	*(volatile unsigned long *)addr_clk(0x001C) = backup_reg[2];
	*(volatile unsigned long *)addr_clk(0x0010) = backup_reg[1];
	*(volatile unsigned long *)addr_clk(0x0000) = backup_reg[0];
}
#endif

#if defined(CONFIG_SHUTDOWN_MODE)
/*===========================================================================

                                 Shut-down

===========================================================================*/
#if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
static void tcc_pm_uart_suspend(bkUART *pBackupUART)
{
	UART *pHwUART = (UART *)tcc_p2v(HwUART0_BASE);

	//pBackupUART->CFG0 = *(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE);
	//pBackupUART->CFG1 = *(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE + 0x4);

	pBackupUART->IER	= pHwUART->REG2.nREG;	//0x04 : IER
	pHwUART->REG2.nREG	&= ~Hw2;	//disable interrupt : ELSI
	pBackupUART->LCR	= pHwUART->LCR.nREG;	//0x0C : LCR
	pHwUART->LCR.nREG	|= Hw7;		// DLAB = 1
	pBackupUART->DLL	= pHwUART->REG1.nREG;	//0x00 : DLL
	pBackupUART->DLM	= pHwUART->REG2.nREG;	//0x04 : DLM
	pBackupUART->MCR	= pHwUART->MCR.nREG;	//0x10 : MCR
	pBackupUART->AFT	= pHwUART->AFT.nREG;	//0x20 : AFT
	pBackupUART->UCR	= pHwUART->UCR.nREG;	//0x24 : UCR
}

static void tcc_pm_uart_resume(bkUART *pBackupUART)
{
	UART *pHwUART = (UART *)tcc_p2v(HwUART0_BASE);

	//*(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE) = pBackupUART->CFG0;
	//*(volatile unsigned long *)tcc_p2v(HwUART_PORTCFG_BASE + 0x4) = pBackupUART->CFG1;

	pHwUART->REG2.nREG	&= ~Hw2;	//disable interrupt : ELSI
	pHwUART->LCR.nREG	|= Hw7;		// DLAB = 1
	pHwUART->REG3.nREG	= Hw2 + Hw1 + Hw0;	//0x08 : FCR
	pHwUART->REG1.nREG	= pBackupUART->DLL;	//0x00 : DLL
	pHwUART->REG2.nREG	= pBackupUART->DLM;	//0x04 : DLM
	pHwUART->MCR.nREG	= pBackupUART->MCR;	//0x10 : MCR
	pHwUART->AFT.nREG	= pBackupUART->AFT;	//0x20 : AFT
	pHwUART->UCR.nREG	= pBackupUART->UCR;	//0x24 : UCR
	pHwUART->LCR.nREG	= pBackupUART->LCR;	//0x0C : LCR
	pHwUART->REG2.nREG	= pBackupUART->IER;	//0x04 : IER
}
#endif

/*===========================================================================
VARIABLE
===========================================================================*/

/*===========================================================================
FUNCTION
===========================================================================*/
static void shutdown(void)
{
	volatile unsigned int cnt = 0;
#ifdef TCC_PM_CHECK_WAKEUP_SOURCE
	volatile unsigned int loop = 0;
#endif

// -------------------------------------------------------------------------
// mmu & cache off

	__asm__ volatile (
	"mrc p15, 0, r0, c1, c0, 0 \n"
	"bic r0, r0, #(1<<12) \n" //ICache
	"bic r0, r0, #(1<<2) \n" //DCache
	"bic r0, r0, #(1<<0) \n" //MMU
	"mcr p15, 0, r0, c1, c0, 0 \n"
	"nop \n"
	);

// -------------------------------------------------------------------------
// SDRAM Self-refresh

#if defined(CONFIG_DRAM_DDR3)
	#if (0)
		BITCSET(denali_ctl(20), 0xFF000000, ((2<<2)|(1<<1)|(0))<<24);
		BITSET(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x3
	#else
		while(denali_ctl(45)&(1<<8)); //CONTROLLER_BUSY
		BITCSET(denali_ctl(20), 0xFF000000, ((2<<2)|(1<<1)|(0))<<24);
		while(denali_ctl(45)&(1<<8)); //CONTROLLER_BUSY
		while(!(denali_ctl(46)&(0x40)));
		BITSET(denali_ctl(47), 0x40);
		BITSET(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x3
		BITCLR(denali_ctl(0),0x1); //START[0] = 0
		BITSET(denali_phy(0x0C), 0x1<<22); //ctrl_cmosrcv[22] = 0x1
		BITCSET(denali_phy(0x0C), 0x001F0000, 0x1F<<16); //ctrl_pd[20:16] = 0x1f
		BITCSET(denali_phy(0x20), 0x000000FF, 0xF<<4|0xF); //ctrl_pulld_dq[7:4] = 0xf, ctrl_pulld_dqs[3:0] = 0xf
	#endif
#elif defined(CONFIG_DRAM_DDR2)
	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) &= ~Hw17; //creq2=0 : enter low power
	while((*(volatile unsigned long *)addr_mem(0x410020))&Hw24); //cack2==0 : wait for acknowledgement to request..

	//MEMCTRL
	*(volatile unsigned long *)addr_mem(0x430004) |= 0x00000001; //clk_stop_en=1 //Bruce_temp_8920
#elif defined(CONFIG_DRAM_LPDDR2)
	#error "LPDDR2 is not implemented"
#else
	#error "not selected"
#endif

	nop_delay(1000);

// -------------------------------------------------------------------------
// Clock <- XIN, PLL <- OFF

//Bruce_temp..
	((PCKC)HwCKC_BASE)->CLKCTRL0.nREG = 0x002ffff4; //CPU
	((PCKC)HwCKC_BASE)->CLKCTRL1.nREG = 0x00200014; //Memory Bus
	((PCKC)HwCKC_BASE)->CLKCTRL2.nREG = 0x00000014; //DDI Bus
	((PCKC)HwCKC_BASE)->CLKCTRL3.nREG = 0x00000014; //Graphic Bus
	((PCKC)HwCKC_BASE)->CLKCTRL4.nREG = 0x00200014; //I/O Bus
	((PCKC)HwCKC_BASE)->CLKCTRL5.nREG = 0x00000014; //Video Bus
	((PCKC)HwCKC_BASE)->CLKCTRL6.nREG = 0x00000014; //Video core
	((PCKC)HwCKC_BASE)->CLKCTRL7.nREG = 0x00000014; //HSIO BUS
	((PCKC)HwCKC_BASE)->CLKCTRL8.nREG = 0x00200014; //SMU Bus

	((PCKC)HwCKC_BASE)->PLL0CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL1CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL2CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL3CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL4CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL5CFG.nREG &= ~0x80000000;

#if defined(TCC_PM_REGULATOR_CTRL)
	{
		CoreFuncPtr pFunc = (CoreFuncPtr)(COREPWR_FUNC_ADDR);
		pFunc(SLEEP);
	}
#endif

// -------------------------------------------------------------------------
// ZQ/VDDQ Power OFF

#if defined(TCC_PM_MEMQ_PWR_CTRL)
	#if defined(CONFIG_MACH_M805_892X)
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x2002 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2003 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2004  || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2005)
		BITCLR(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<6); //GPIO D 6
	else
		BITCLR(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<15); //GPIO D 15
	#elif defined(CONFIG_MACH_TCC8920)
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1005 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1007)
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<4); //GPIO B 4
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1006)
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<29); //GPIO B 29
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<7); //GPIO C 7
	}
	else
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<22); //GPIO C 22
	}
	#endif
#endif
#if defined(CONFIG_MACH_TCC8920ST)
    tcc_stb_suspend();
#endif

// -------------------------------------------------------------------------
// SRAM Retention

	BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<16); //PD_RETN : SRAM retention mode=0

// -------------------------------------------------------------------------
// Remap

	BITCSET(((PPMU)HwPMU_BASE)->PMU_CONFIG.nREG, 0x30000000, 0x10000000); //remap : 0x00000000 <- sram(0x10000000)
	BITSET(((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSCFG.nREG, 1<<31); //RMIR : remap for top area : 0xF0000000 <- sram(0x10000000)

// -------------------------------------------------------------------------
// BUS Power Down

	//IP isolation off
	((PPMU)HwPMU_BASE)->PMU_ISOL.nREG = 0x00000BDF;

#if defined(TCC_PM_PMU_CTRL)
	//High Speed I/O Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.HSIOBUS = 0;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.HSIOBUS == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.HSB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_HSBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_HSB == 0);

	//DDI Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.DBUS = 0;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS0 | ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS1) == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.DB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_DBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_DB == 0);

	//Graphic Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.GBUS = 0;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.GBUS == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.GB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_GBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_GB == 0);

	//Video Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.VBUS = 0;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS0 | ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS1) == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.VB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_VBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_VB == 0);
#endif

// -------------------------------------------------------------------------
// SSTL & IO Retention

	while(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8)){
		BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode =0
	}
	while(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<4)){
		BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<4); //IO_RTO : I/O retention mode =0
	}

// -------------------------------------------------------------------------
// set wake-up
//Bruce, wake-up source should be surely set here !!!!

	//set wake-up trigger mode : edge
	((PPMU)HwPMU_BASE)->PMU_WKMOD0.nREG = 0xFFFFFFFF;
	((PPMU)HwPMU_BASE)->PMU_WKMOD1.nREG = 0xFFFFFFFF;
	//set wake-up polarity : default : active high
	((PPMU)HwPMU_BASE)->PMU_WKPOL0.nREG = 0x00000000;
	((PPMU)HwPMU_BASE)->PMU_WKPOL1.nREG = 0x00000000;

	/* Power Key */
#if defined(CONFIG_MACH_M805_892X)
	if (*(volatile unsigned long *)SRAM_STACK_ADDR == 0x2002 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2003 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2004  || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2005)
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL1.bREG.GPIO_E15 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E15 = 1; //power key

		#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT) && defined(TCC_CSR_HOST_WAKE_UP)
		((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_G18 = 1; //uart-rx(BT HOST WAKE UP) - Active Low-               
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_G18 = 1; //uart-rx(BT HOST WAKE UP)
		#endif

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1) 	// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E28 = 1;	// PMU WakeUp Enable
		#endif
	}
	else
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_D09 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D09 = 1; //power key

		#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT) && defined(TCC_CSR_HOST_WAKE_UP)
		((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_G14 = 1; //uart-rx(BT HOST WAKE UP) - Active Low-               
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_G14 = 1; //uart-rx(BT HOST WAKE UP)
		#endif

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1) 	// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E13 = 1;	// PMU WakeUp Enable
		#endif
	}
#elif defined(CONFIG_MACH_TCC8920ST)
	//set wake-up polarity
	((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_D14 = 1; //power key - Active Low
	//set wake-up source
	((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D14 = 1; //power key

	#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
	if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D12 = 1;	// PMU WakeUp Enable
	#endif
#else
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1005 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1007 || \
	   *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL1.bREG.GPIO_E30 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E30 = 1; //power key
	
		#if defined (CONFIG_TCC_CSR_BC0406_MODULE_SUPPORT) && defined(TCC_CSR_HOST_WAKE_UP)
		((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_G14 = 1; //uart-rx(BT HOST WAKE UP) - Active Low-               
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_G14 = 1; //uart-rx(BT HOST WAKE UP)
		#endif

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
		{
			if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
			{
				#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
				((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E00 = 1;	// PMU WakeUp Enable
				#else
				((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E01 = 1;	// PMU WakeUp Enable
				#endif
			}
			else
				((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_B12 = 1;	// PMU WakeUp Enable
		}
		#endif
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1006)
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL1.bREG.GPIO_E24 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E24 = 1; //power key

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E28 = 1;	// PMU WakeUp Enable
		#endif
	}
	else
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_G16 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_G16 = 1; //power key

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D13 = 1;	// PMU WakeUp Enable
		#endif
	}
#endif

	/* RTC Alarm Wake Up */
	//set wake-up polarity
	((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.RTC_WAKEUP = 0; //RTC_PMWKUP - Active High
	//set wake-up source
	((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.RTC_WAKEUP = 1; //RTC_PMWKUP - PMU WakeUp Enable


// -------------------------------------------------------------------------
// Enter Shutdown !!

	while(1){
		((PPMU)HwPMU_BASE)->PWRDN_TOP.nREG = 1;
		nop_delay(100);

#ifdef TCC_PM_CHECK_WAKEUP_SOURCE
		if(loop++ > 100){
			if(((PPMU)HwPMU_BASE)->PMU_WKSTS0.nREG || ((PPMU)HwPMU_BASE)->PMU_WKSTS1.nREG){
				((PPMU)HwPMU_BASE)->PMU_WDTCTRL.nREG = (Hw31 + 0x1);
				while(1);
			}
		}
#endif
	};
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void wakeup(void)
{
	volatile unsigned int cnt;
	volatile unsigned *src;
	volatile unsigned *dest;

// -------------------------------------------------------------------------
// GPIO Restore

	src = (unsigned*)GPIO_REPOSITORY_ADDR;
	dest = (unsigned*)HwGPIO_BASE;

	for(cnt=0 ; cnt<(sizeof(GPIO)/sizeof(unsigned)) ; cnt++)
		dest[cnt] = src[cnt];

// -------------------------------------------------------------------------
// clear wake-up
	//disable all for accessing PMU Reg. !!!
	((PPMU)HwPMU_BASE)->PMU_WKUP0.nREG = 0x00000000;
	((PPMU)HwPMU_BASE)->PMU_WKUP1.nREG = 0x00000000;
#ifdef TCC_PM_CHECK_WAKEUP_SOURCE
	((PPMU)HwPMU_BASE)->PMU_WKCLR0.nREG = 0xFFFFFFFF;
	((PPMU)HwPMU_BASE)->PMU_WKCLR1.nREG = 0xFFFFFFFF;
#endif

// -------------------------------------------------------------------------
// SSTL & IO Retention Disable

#ifndef TCC_PM_SSTLIO_CTRL
	while(!(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8))){
		BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode disable=1
	}
#endif
	while(!(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<4))){
		BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<4); //IO_RTO : I/O retention mode disable=1
	}

// -------------------------------------------------------------------------
// ZQ/VDDQ Power ON

#if defined(TCC_PM_MEMQ_PWR_CTRL)
	#if defined(CONFIG_MACH_M805_892X)
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x2002 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2003 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2004  || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2005)
		BITSET(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<6); //GPIO D 6
	else
		BITSET(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<15); //GPIO D 15
	#elif defined(CONFIG_MACH_TCC8920)
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1005 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1007)
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<4); //GPIO B 4
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1006)
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<29); //GPIO B 29
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<7); //GPIO C 7
	}
	else
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<22); //GPIO C 22
	}
	#endif
#endif
#if defined(CONFIG_MACH_TCC8920ST)
    tcc_stb_resume();
#endif

#if defined(TCC_PM_REGULATOR_CTRL)
	{
		CoreFuncPtr pFunc = (CoreFuncPtr)(COREPWR_FUNC_ADDR);
		pFunc(WAKEUP);
	}
#endif

// -------------------------------------------------------------------------
// BUS Power On

#if defined(TCC_PM_PMU_CTRL)
	while(((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.MAIN_STATE);

	((PPMU)HwPMU_BASE)->PWRUP_HSBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_HSB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.HSB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.HSIOBUS = 1;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.HSIOBUS == 0);

	((PPMU)HwPMU_BASE)->PWRUP_DBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_DB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.DB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.DBUS = 1;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS0 & ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS1) == 0);

	((PPMU)HwPMU_BASE)->PWRUP_GBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_GB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.GB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.GBUS = 1;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.GBUS == 0);

	((PPMU)HwPMU_BASE)->PWRUP_VBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_VB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.VB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.VBUS = 1;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS0 & ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS1) == 0);

	//IP isolation on
	((PPMU)HwPMU_BASE)->PMU_ISOL.nREG = 0x00000000;
#else
	//IP isolation on
	((PPMU)HwPMU_BASE)->PMU_ISOL.nREG = 0x00000BD0;
#endif
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void edi_init(void)
{
	PEDI pEDI = (PEDI)tcc_p2v(HwEDI_BASE);

#if defined(CONFIG_MACH_TCC8920ST) || defined(TARGET_BOARD_STB)
	BITCSET(pEDI->EDI_RDYCFG.nREG,  0x000FFFFF, 0x00000001 );
	BITCSET(pEDI->EDI_CSNCFG0.nREG, 0x000FFFFF, 0x00008765 );
#elif defined(CONFIG_MACH_M805_892X) || defined(_M805_8923_)
	BITCSET(pEDI->EDI_RDYCFG.nREG,  0x000FFFFF, 0x00000001 );
	BITCSET(pEDI->EDI_CSNCFG0.nREG, 0x0000FFFF, 0x00008765 );
#else
	BITCSET(pEDI->EDI_RDYCFG.nREG,  0x000FFFFF, 0x00032104 );
	BITCSET(pEDI->EDI_CSNCFG0.nREG, 0x0000FFFF, 0x00008765 );
#endif
	BITCSET(pEDI->EDI_OENCFG.nREG,  0x0000000F, 0x00000001 );
	BITCSET(pEDI->EDI_WENCFG.nREG,  0x0000000F, 0x00000001 ); 
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void tcc_nfc_suspend(PNFC pBackupNFC , PNFC pHwNFC)
{
	pBackupNFC->NFC_CACYC = pHwNFC->NFC_CACYC;
	pBackupNFC->NFC_WRCYC = pHwNFC->NFC_WRCYC;
	pBackupNFC->NFC_RECYC = pHwNFC->NFC_RECYC;
	pBackupNFC->NFC_CTRL = pHwNFC->NFC_CTRL;
	pBackupNFC->NFC_IRQCFG = pHwNFC->NFC_IRQCFG;
	pBackupNFC->NFC_RFWBASE = pHwNFC->NFC_RFWBASE;
	
}

/*===========================================================================
FUNCTION
===========================================================================*/


static void tcc_nfc_resume(PNFC pHwNFC , PNFC pBackupNFC)
{
	edi_init();
	pHwNFC->NFC_CACYC = pBackupNFC->NFC_CACYC;
	pHwNFC->NFC_WRCYC = pBackupNFC->NFC_WRCYC;
	pHwNFC->NFC_RECYC = pBackupNFC->NFC_RECYC;
	pHwNFC->NFC_CTRL = pBackupNFC->NFC_CTRL;
	pHwNFC->NFC_IRQCFG = pBackupNFC->NFC_IRQCFG;
	pHwNFC->NFC_IRQ = 0xFFFFFFFF;
	pHwNFC->NFC_RFWBASE = pBackupNFC->NFC_RFWBASE;

}

/*===========================================================================
VARIABLE
===========================================================================*/
static TCC_REG RegRepo;

/*===========================================================================
FUNCTION
===========================================================================*/
static void shutdown_mode(void)
{
	volatile unsigned int cnt = 0;
	unsigned way_mask;

	/*--------------------------------------------------------------
	 replace pm functions
	--------------------------------------------------------------*/
	memcpy((void*)SRAM_BOOT_ADDR,       (void*)SRAM_Boot,  SRAM_BOOT_SIZE);
	memcpy((void*)SHUTDOWN_FUNC_ADDR,   (void*)shutdown,   SHUTDOWN_FUNC_SIZE);
	memcpy((void*)WAKEUP_FUNC_ADDR,     (void*)wakeup,     WAKEUP_FUNC_SIZE);
	memcpy((void*)SDRAM_INIT_FUNC_ADDR, (void*)sdram_init, SDRAM_INIT_FUNC_SIZE);

#if defined(TCC_PM_REGULATOR_CTRL)
	memcpy((void*)COREPWR_FUNC_ADDR,    (void*)set_core_voltage, COREPWR_FUNC_SIZE);

	PMIC_PARAM(DEV_ADDR)      = TCC_PM_REGULATOR_DEV_ADDR;
	PMIC_PARAM(CORE_CTRL_REG) = TCC_PM_REGULATOR_DEV_REG;
	PMIC_PARAM(READ_SIZE)     = TCC_PM_REGULATOR_READ_SIZE;
	PMIC_PARAM(WRITE_SIZE)    = TCC_PM_REGULATOR_WRITE_SIZE;
	PMIC_PARAM(WRITE_DATA1_S) = TCC_PM_REGULATOR_SLEEP_DATA1;
	PMIC_PARAM(WRITE_DATA2_S) = TCC_PM_REGULATOR_SLEEP_DATA2;
	PMIC_PARAM(WRITE_DATA1_W) = TCC_PM_REGULATOR_WAKEUP_DATA1;
	PMIC_PARAM(WRITE_DATA2_W) = TCC_PM_REGULATOR_WAKEUP_DATA2;
	PMIC_PARAM(DEV_CH)        = TCC_PM_REGULATOR_I2C_CH;
	PMIC_PARAM(MACHINE_ID)    = 0x00;	// machine id
	PMIC_PARAM(SYSTEM_REV)    = system_rev;
#endif

#ifdef CONFIG_PM_CONSOLE_NOT_SUSPEND
	/*--------------------------------------------------------------
	 UART block suspend
	--------------------------------------------------------------*/
	tcc_pm_uart_suspend(&RegRepo.bkuart);
#endif

	/*--------------------------------------------------------------
	 BUS Config
	--------------------------------------------------------------*/
	memcpy(&RegRepo.membuscfg, (PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE), sizeof(MEMBUSCFG));
	memcpy(&RegRepo.iobuscfg, (PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE), sizeof(IOBUSCFG));

	/* all peri io bus on */
	((PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE))->HCLKEN0.nREG = 0xFFFFFFFF;
	((PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE))->HCLKEN1.nREG = 0xFFFFFFFF;

	/*--------------------------------------------------------------
	 SMU & PMU
	--------------------------------------------------------------*/
	memcpy(&RegRepo.smuconfig, (PSMUCONFIG)tcc_p2v(HwSMUCONFIG_BASE), sizeof(SMUCONFIG));
#if defined(TCC_PM_PMU_CTRL)
	memcpy(&RegRepo.pmu, (PPMU)tcc_p2v(HwPMU_BASE), sizeof(PMU));
#endif
	memcpy(&RegRepo.timer, (PTIMER *)tcc_p2v(HwTMR_BASE), sizeof(TIMER));
	memcpy(&RegRepo.vic, (PVIC)tcc_p2v(HwVIC_BASE), sizeof(VIC));
	memcpy(&RegRepo.pic, (PPIC)tcc_p2v(HwPIC_BASE), sizeof(PIC));
	memcpy(&RegRepo.ckc, (PCKC)tcc_p2v(HwCKC_BASE), sizeof(CKC));

	/*--------------------------------------------------------------
	 GPIO
	--------------------------------------------------------------*/
	memcpy((void*)GPIO_REPOSITORY_ADDR, (PGPIO)tcc_p2v(HwGPIO_BASE), sizeof(GPIO));

	/*--------------------------------------------------------------
	 NFC suspend 
	--------------------------------------------------------------*/
	tcc_nfc_suspend(&RegRepo.nfc, (PNFC)tcc_p2v(HwNFC_BASE));

#ifdef CONFIG_CACHE_L2X0
	/*--------------------------------------------------------------
	 L2 cache
	--------------------------------------------------------------*/
	RegRepo.L2_aux = *(volatile unsigned int *)(L2CACHE_BASE+L2X0_AUX_CTRL); //save aux
	if (RegRepo.L2_aux & (1 << 16))
		way_mask = (1 << 16) - 1;
	else
		way_mask = (1 << 8) - 1;
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CLEAN_WAY) = way_mask; //clean all
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CLEAN_WAY)&way_mask); //wait for clean
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC) = 0; //sync
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC)&1); //wait for sync
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 0; //cache off
#endif

	/*--------------------------------------------------------------
	 flush tlb & cache
	--------------------------------------------------------------*/
	local_flush_tlb_all();
	flush_cache_all();

	/////////////////////////////////////////////////////////////////
	save_cpu_reg(SHUTDOWN_FUNC_ADDR, CPU_DATA_REPOSITORY_ADDR, resore_cpu_reg);
	/////////////////////////////////////////////////////////////////

	__asm__ __volatile__ ("nop\n");


#if defined(TCC_PM_TP_DEBUG)	// 1
	gpio_set_value(TP_GPIO_PORT, 0);
#endif

	/* all peri io bus on */
	((PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE))->HCLKEN0.nREG = 0xFFFFFFFF;
	((PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE))->HCLKEN1.nREG = 0xFFFFFFFF;

#if defined(TCC_PM_TP_DEBUG)	// 2
	gpio_set_value(TP_GPIO_PORT, 1);
#endif

	/*--------------------------------------------------------------
	 SMU & PMU
	--------------------------------------------------------------*/

	//memcpy((PCKC)tcc_p2v(HwCKC_BASE), &RegRepo.ckc, sizeof(CKC));
	{
		//PLL
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL0CFG.nREG = RegRepo.ckc.PLL0CFG.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL1CFG.nREG = RegRepo.ckc.PLL1CFG.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL2CFG.nREG = RegRepo.ckc.PLL2CFG.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL3CFG.nREG = RegRepo.ckc.PLL3CFG.nREG;
		//((PCKC)tcc_p2v(HwCKC_BASE))->PLL4CFG.nREG = RegRepo.ckc.PLL4CFG.nREG; //PLL4 is used as a source of memory bus clock
		//((PCKC)tcc_p2v(HwCKC_BASE))->PLL5CFG.nREG = RegRepo.ckc.PLL5CFG.nREG; //PLL5 is used as a source of CPU bus clock
		nop_delay(1000);

		//Divider
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKDIVC0.nREG = RegRepo.ckc.CLKDIVC0.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKDIVC1.nREG = RegRepo.ckc.CLKDIVC1.nREG;
		nop_delay(100);

		//((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL0.nREG = RegRepo.ckc.CLKCTRL0.nREG; //CPU Clock can't be adjusted freely.
		//((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL1.nREG = RegRepo.ckc.CLKCTRL1.nREG; //Memory Clock can't be adjusted freely.
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL2.nREG = RegRepo.ckc.CLKCTRL2.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL3.nREG = RegRepo.ckc.CLKCTRL3.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL4.nREG = RegRepo.ckc.CLKCTRL4.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL5.nREG = RegRepo.ckc.CLKCTRL5.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL6.nREG = RegRepo.ckc.CLKCTRL6.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL7.nREG = RegRepo.ckc.CLKCTRL7.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL8.nREG = RegRepo.ckc.CLKCTRL8.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->SWRESET.nREG = ~(RegRepo.ckc.SWRESET.nREG);

		//Peripheral clock
		memcpy((void*)&(((PCKC)tcc_p2v(HwCKC_BASE))->PCLKCTRL00.nREG), (void*)&(RegRepo.ckc.PCLKCTRL00.nREG), 0x150-0x80);
	}
	nop_delay(100);

#if defined(TCC_PM_TP_DEBUG)	// 3
	gpio_set_value(TP_GPIO_PORT, 0);
#endif

#if defined(TCC_PM_PMU_CTRL)
	//memcpy((PPMU)tcc_p2v(HwPMU_BASE), &RegRepo.pmu, sizeof(PMU));
	{
		while(((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.MAIN_STATE);

		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_HSB){ //if High Speed I/O Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.HSIOBUS = 0;
			while (((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.HSIOBUS == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.HSB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_HSBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_HSB == 0);
		}
		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_DB){ //if DDI Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.DBUS = 0;
			while ((((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.DBUS0 | ((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.DBUS1) == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.DB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_DBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_DB == 0);
		}
		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_GB){ //if Graphic Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.GBUS = 0;
			while (((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.GBUS == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.GB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_GBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_GB == 0);
		}
		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_VB){ //if Video Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.VBUS = 0;
			while ((((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.VBUS0 | ((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.VBUS1) == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.VB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_VBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_VB == 0);
		}

		//IP isolation restore
		//Bruce, PMU_ISOL is only write-only.. so it can't be set to back-up data..
		//((PPMU)tcc_p2v(HwPMU_BASE))->PMU_ISOL.nREG = RegRepo.PMU_ISOL.nREG;
		((PPMU)tcc_p2v(HwPMU_BASE))->PMU_ISOL.nREG = 0x00000000;
	}
#endif

#if defined(TCC_PM_TP_DEBUG)	// 4
	gpio_set_value(TP_GPIO_PORT, 1);
#endif

	memcpy((PPIC)tcc_p2v(HwPIC_BASE), &RegRepo.pic, sizeof(PIC));
	memcpy((PVIC)tcc_p2v(HwVIC_BASE), &RegRepo.vic, sizeof(VIC));
	memcpy((PTIMER *)tcc_p2v(HwTMR_BASE), &RegRepo.timer, sizeof(TIMER));
	memcpy((PSMUCONFIG)tcc_p2v(HwSMUCONFIG_BASE), &RegRepo.smuconfig, sizeof(SMUCONFIG));

#if defined(TCC_PM_TP_DEBUG)	// 5
	gpio_set_value(TP_GPIO_PORT, 0);
#endif

	/*--------------------------------------------------------------
	 BUS Config
	--------------------------------------------------------------*/
	memcpy((PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE), &RegRepo.iobuscfg, sizeof(IOBUSCFG));
	memcpy((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE), &RegRepo.membuscfg, sizeof(MEMBUSCFG));

#if defined(TCC_PM_TP_DEBUG)	// 6
	gpio_set_value(TP_GPIO_PORT, 1);
#endif

	/*--------------------------------------------------------------
	 NFC
	--------------------------------------------------------------*/
	tcc_nfc_resume((PNFC)tcc_p2v(HwNFC_BASE), &RegRepo.nfc);

#if defined(TCC_PM_TP_DEBUG)	// 7
	gpio_set_value(TP_GPIO_PORT, 0);
#endif

#ifdef CONFIG_PM_CONSOLE_NOT_SUSPEND
	/*--------------------------------------------------------------
	 UART block resume
	--------------------------------------------------------------*/
	tcc_pm_uart_resume(&RegRepo.bkuart);
#endif

#if defined(TCC_PM_TP_DEBUG)	// 8
	gpio_set_value(TP_GPIO_PORT, 1);
#endif

	/*--------------------------------------------------------------
	 cpu re-init for VFP
	--------------------------------------------------------------*/
	__cpu_early_init();

#if defined(TCC_PM_TP_DEBUG)	// 9
	gpio_set_value(TP_GPIO_PORT, 0);
#endif

#ifdef CONFIG_CACHE_L2X0
	/*--------------------------------------------------------------
	 L2 cache enable
	--------------------------------------------------------------*/
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 0; //cache off
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_AUX_CTRL) = RegRepo.L2_aux; //restore aux
	if (RegRepo.L2_aux & (1 << 16))
		way_mask = (1 << 16) - 1;
	else
		way_mask = (1 << 8) - 1;
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_INV_WAY) = way_mask; //invalidate all
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_INV_WAY)&way_mask); //wait for invalidate
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC) = 0; //sync
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC)&1); //wait for sync
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 1; //cache on
#endif

#if defined(TCC_PM_TP_DEBUG)	// 10
	gpio_set_value(TP_GPIO_PORT, 1);
#endif
}
/*=========================================================================*/

#elif defined(CONFIG_SLEEP_MODE)
/*===========================================================================

                                 SLEEP

===========================================================================*/

/*===========================================================================
FUNCTION
===========================================================================*/
static void sleep(void)
{
	volatile unsigned int cnt = 0;

	#define PLL5_VALUE      0x01012C03	// PLL5 : 600MHz for CPU

// -------------------------------------------------------------------------
// mmu & cache off

	__asm__ volatile (
	"mrc p15, 0, r0, c1, c0, 0 \n"
	"bic r0, r0, #(1<<12) \n" //ICache
	"bic r0, r0, #(1<<2) \n" //DCache
	"bic r0, r0, #(1<<0) \n" //MMU
	"mcr p15, 0, r0, c1, c0, 0 \n"
	"nop \n"
	);

#if defined(CONFIG_DRAM_DDR3)
	#if (0)
		BITCSET(denali_ctl(20), 0xFF000000, ((2<<2)|(1<<1)|(0))<<24);
		BITSET(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x3
	#else
		while(denali_ctl(45)&(1<<8)); //CONTROLLER_BUSY
		BITCSET(denali_ctl(20), 0xFF000000, ((2<<2)|(1<<1)|(0))<<24);
		while(denali_ctl(45)&(1<<8)); //CONTROLLER_BUSY
		while(!(denali_ctl(46)&(0x40)));
		BITSET(denali_ctl(47), 0x40);
		BITSET(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x3
		BITCLR(denali_ctl(0),0x1); //START[0] = 0
		BITSET(denali_phy(0x0C), 0x1<<22); //ctrl_cmosrcv[22] = 0x1
		BITCSET(denali_phy(0x0C), 0x001F0000, 0x1F<<16); //ctrl_pd[20:16] = 0x1f
		BITCSET(denali_phy(0x20), 0x000000FF, 0xF<<4|0xF); //ctrl_pulld_dq[7:4] = 0xf, ctrl_pulld_dqs[3:0] = 0xf
	#endif
#elif defined(CONFIG_DRAM_DDR2)
// -------------------------------------------------------------------------
// SDRAM Self-refresh
	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) &= ~Hw17; //creq2=0 : enter low power
	while((*(volatile unsigned long *)addr_mem(0x410020))&Hw24); //cack2==0 : wait for acknowledgement to request..

	//MEMCTRL
	*(volatile unsigned long *)addr_mem(0x430004) |= 0x00000001; //clk_stop_en=1 //Bruce_temp_8920
#elif defined(CONFIG_DRAM_LPDDR2)
	#error "LPDDR2 is not implemented"
#else
	#error "not selected"
#endif

	nop_delay(1000);

// -------------------------------------------------------------------------
// Clock <- XIN, PLL <- OFF

	((PCKC)HwCKC_BASE)->CLKCTRL0.nREG = 0x002ffff4; //CPU
	((PCKC)HwCKC_BASE)->CLKCTRL1.nREG = 0x00200014; //Memory Bus
	((PCKC)HwCKC_BASE)->CLKCTRL2.nREG = 0x00000014; //DDI Bus
	((PCKC)HwCKC_BASE)->CLKCTRL3.nREG = 0x00000014; //Graphic Bus
	((PCKC)HwCKC_BASE)->CLKCTRL4.nREG = 0x00200014; //I/O Bus
	((PCKC)HwCKC_BASE)->CLKCTRL5.nREG = 0x00000014; //Video Bus
	((PCKC)HwCKC_BASE)->CLKCTRL6.nREG = 0x00000014; //Video core
	((PCKC)HwCKC_BASE)->CLKCTRL7.nREG = 0x00000014; //HSIO BUS
	((PCKC)HwCKC_BASE)->CLKCTRL8.nREG = 0x00200014; //SMU Bus

	((PCKC)HwCKC_BASE)->PLL0CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL1CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL2CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL3CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL4CFG.nREG &= ~0x80000000;
	((PCKC)HwCKC_BASE)->PLL5CFG.nREG &= ~0x80000000;

// -------------------------------------------------------------------------
// ZQ/VDDQ Power OFF

#if defined(TCC_PM_MEMQ_PWR_CTRL)
	#if defined(CONFIG_MACH_M805_892X)
	if (*(volatile unsigned long *)SRAM_STACK_ADDR == 0x2002 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2003 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2004  || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2005)
		BITCLR(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<6); //GPIO D 6
	else
		BITCLR(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<15); //GPIO D 15
	#elif defined(CONFIG_MACH_TCC8920)
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1005 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1007)
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<4); //GPIO B 4
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1006)
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<29); //GPIO B 29
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<7); //GPIO C 7
	}
	else
	{
		BITCLR(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<22); //GPIO C 22
	}
	#endif
#endif
#if defined(CONFIG_MACH_TCC8920ST)
    tcc_stb_suspend();
#endif
// -------------------------------------------------------------------------
// SRAM Retention

	BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<16); //PD_RETN : SRAM retention mode=0

// -------------------------------------------------------------------------
// BUS Power Down

#if defined(TCC_PM_PMU_CTRL)
	//IP isolation off
	((PPMU)HwPMU_BASE)->PMU_ISOL.nREG = 0x00000BDF;

	//High Speed I/O Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.HSIOBUS = 0;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.HSIOBUS == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.HSB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_HSBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_HSB == 0);

	//DDI Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.DBUS = 0;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS0 | ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS1) == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.DB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_DBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_DB == 0);

	//Graphic Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.GBUS = 0;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.GBUS == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.GB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_GBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_GB == 0);

	//Video Bus power down
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.VBUS = 0;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS0 | ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS1) == 1);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.VB = 0;
	((PPMU)HwPMU_BASE)->PWRDN_VBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PD_VB == 0);
#endif

// -------------------------------------------------------------------------
// SSTL & IO Retention

	while(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8)){
		BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode =0
	}
	while(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<4)){
		BITCLR(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<4); //IO_RTO : I/O retention mode =0
	}

// -------------------------------------------------------------------------
// set wake-up
//Bruce, wake-up source should be surely set here !!!!

	//set wake-up trigger mode : edge
	((PPMU)HwPMU_BASE)->PMU_WKMOD0.nREG = 0xFFFFFFFF;
	((PPMU)HwPMU_BASE)->PMU_WKMOD1.nREG = 0xFFFFFFFF;
	//set wake-up polarity : active high
	((PPMU)HwPMU_BASE)->PMU_WKPOL0.nREG = 0x00000000;
	((PPMU)HwPMU_BASE)->PMU_WKPOL1.nREG = 0x00000000;

	/* Power Key */
#if defined(CONFIG_MACH_M805_892X)
	if (*(volatile unsigned long *)SRAM_STACK_ADDR == 0x2002 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2003 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2004  || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2005)
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL1.bREG.GPIO_E27 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E27 = 1; //power key

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1) 	// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E28 = 1;	// PMU WakeUp Enable
		#endif
	}
	else
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_D09 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D09 = 1; //power key

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1) 	// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E13 = 1;	// PMU WakeUp Enable
		#endif
	}
#elif defined(CONFIG_MACH_TCC8920ST)
	//set wake-up polarity
	((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_D14 = 1; //power key - Active Low
	//set wake-up source
	((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D14 = 1; //power key

	#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
	if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D12 = 1;	// PMU WakeUp Enable
	#endif

  #if defined(TCC_PM_SLEEP_WFI_USED)
	((PPIC)HwPIC_BASE)->IEN0.nREG = 0;
	((PPIC)HwPIC_BASE)->IEN1.nREG = 0;

    #if defined(CONFIG_STB_BOARD_HDB892S) || defined(CONFIG_STB_BOARD_HDB892F)

    #else
	((PPIC)HwPIC_BASE)->IEN0.bREG.EINT0 = 1;
	((PGPIO)HwGPIO_BASE)->EINTSEL0.bREG.EINT00SEL = 105;
	((PPIC)HwPIC_BASE)->POL0.bREG.EINT0 = 1;
	((PPIC)HwPIC_BASE)->MODE0.bREG.EINT0 = 1;
	((PPIC)HwPIC_BASE)->INTMSK0.bREG.EINT0 = 1;
    #endif
	((PPIC)HwPIC_BASE)->IEN1.bREG.REMOCON = 1;
  #endif
#else
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1005 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1007 || \
	   *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL1.bREG.GPIO_E30 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E30 = 1; //power key

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
		{
			if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
			{
				#if defined(CONFIG_MMC_TCC_SUPPORT_EMMC)
				((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E00 = 1;	// PMU WakeUp Enable
				#else
				((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E01 = 1;	// PMU WakeUp Enable
				#endif
			}
			else
				((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_B12 = 1;	// PMU WakeUp Enable
		}
		#endif
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1006)
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL1.bREG.GPIO_E24 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E24 = 1; //power key

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP1.bREG.GPIO_E28 = 1;	// PMU WakeUp Enable
		#endif
	}
	else
	{
		//set wake-up polarity
		((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.GPIO_G16 = 1; //power key - Active Low
		//set wake-up source
		((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_G16 = 1; //power key

		#if defined(CONFIG_MMC_TCC_SDHC)	// Wakeup for SD Insert->Remove in Suspend.
		if(*(volatile unsigned long *)(SRAM_STACK_ADDR+4) == 1)		// SD Insert -> Remove in suspend : Active High
			((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.GPIO_D13 = 1;	// PMU WakeUp Enable
		#endif
	}
#endif

	/* RTC Alarm Wake Up */
	//set wake-up polarity
	((PPMU)HwPMU_BASE)->PMU_WKPOL0.bREG.RTC_WAKEUP = 0; //RTC_PMWKUP - Active High
	//set wake-up source
	((PPMU)HwPMU_BASE)->PMU_WKUP0.bREG.RTC_WAKEUP = 1; //RTC_PMWKUP - PMU WakeUp Enable

// -------------------------------------------------------------------------
// Enter Sleep !!
////////////////////////////////////////////////////////////////////////////
#ifdef TCC_PM_SLEEP_WFI_USED
	asm("dsb");
	asm("wfi");
#else
	BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<0);
#endif
////////////////////////////////////////////////////////////////////////////

// -------------------------------------------------------------------------
// set wake-up
	//disable all for accessing PMU Reg. !!!
	((PPMU)HwPMU_BASE)->PMU_WKUP0.nREG = 0x00000000;
	((PPMU)HwPMU_BASE)->PMU_WKUP1.nREG = 0x00000000;

// -------------------------------------------------------------------------
// SSTL & IO Retention Disable

	while(!(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<8))){
		BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode disable=1
	}
	while(!(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG&(1<<4))){
		BITSET(((PPMU)HwPMU_BASE)->PWRDN_XIN.nREG, 1<<4); //IO_RTO : I/O retention mode disable=1
	}

// -------------------------------------------------------------------------
// ZQ/VDDQ Power ON

#if defined(TCC_PM_MEMQ_PWR_CTRL)
	#if defined(CONFIG_MACH_M805_892X)
	if (*(volatile unsigned long *)SRAM_STACK_ADDR == 0x2002 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2003 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2004  || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x2005)
		BITSET(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<6); //GPIO D 6
	else
		BITSET(((PGPIO)HwGPIO_BASE)->GPDDAT.nREG, 1<<15); //GPIO D 15
	#elif defined(CONFIG_MACH_TCC8920)
	if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1005 || *(volatile unsigned long *)SRAM_STACK_ADDR == 0x1007)
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<4); //GPIO B 4
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1006)
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPBDAT.nREG, 1<<29); //GPIO B 29
	}
	else if(*(volatile unsigned long *)SRAM_STACK_ADDR == 0x1008)
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<7); //GPIO C 7
	}
	else
	{
		BITSET(((PGPIO)HwGPIO_BASE)->GPCDAT.nREG, 1<<22); //GPIO C 22
	}
	#endif
#endif
#if defined(CONFIG_MACH_TCC8920ST)
    tcc_stb_resume();
#endif
// -------------------------------------------------------------------------
// BUS Power On

#if defined(TCC_PM_PMU_CTRL)
	while(((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.MAIN_STATE);

	((PPMU)HwPMU_BASE)->PWRUP_HSBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_HSB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.HSB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.HSIOBUS = 1;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.HSIOBUS == 0);

	((PPMU)HwPMU_BASE)->PWRUP_DBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_DB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.DB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.DBUS = 1;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS0 & ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.DBUS1) == 0);

	((PPMU)HwPMU_BASE)->PWRUP_GBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_GB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.GB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.GBUS = 1;
	while (((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.GBUS == 0);

	((PPMU)HwPMU_BASE)->PWRUP_VBUS.bREG.DATA = 1;
	while (((PPMU)HwPMU_BASE)->PMU_PWRSTS.bREG.PU_VB == 0);
	((PPMU)HwPMU_BASE)->PMU_SYSRST.bREG.VB = 1;
	((PMEMBUSCFG)HwMBUSCFG_BASE)->HCLKMASK.bREG.VBUS = 1;
	while ((((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS0 & ((PMEMBUSCFG)HwMBUSCFG_BASE)->MBUSSTS.bREG.VBUS1) == 0);

	//IP isolation on
	((PPMU)HwPMU_BASE)->PMU_ISOL.nREG = 0x00000000;
#endif

// Exit SDRAM Self-refresh ------------------------------------------------------------
#if defined(CONFIG_DRAM_DDR3)

	((PCKC)HwCKC_BASE)->PLL5CFG.nREG = PLL5_VALUE; //for CPU clock
	((PCKC)HwCKC_BASE)->PLL5CFG.nREG |= 0x80000000;
	((PCKC)HwCKC_BASE)->PLL4CFG.nREG |= 0x80000000; //for Mem clock
	cnt=3200; while(cnt--);
	((PCKC)HwCKC_BASE)->CLKCTRL0.nREG = 0x002FFFF9;
	((PCKC)HwCKC_BASE)->CLKCTRL1.nREG = 0x00200018;

	BITCLR(denali_phy(0x0C), 0x1<<22); //ctrl_cmosrcv[22] = 0x0
	BITCLR(denali_phy(0x0C), 0x001F0000); //ctrl_pd[20:16] = 0x0
	BITCLR(denali_phy(0x20), 0x000000FF); //ctrl_pulld_dq[7:4] = 0x0, ctrl_pulld_dqs[3:0] = 0x0
	while(((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_XIN.nREG&(1<<8)){
		BITCLR(((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode =0
	}
	*(volatile unsigned long *)addr_mem(0x810004) &= ~(1<<2); //PHY=0
	*(volatile unsigned long *)addr_mem(0x810004) |= (1<<2); //PHY=1
	while(!(((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_XIN.nREG&(1<<8))){
		BITSET(((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_XIN.nREG, 1<<8); //SSTL_RTO : SSTL I/O retention mode disable=1
	}
	BITCLR(denali_ctl(96), 0x3<<8); //DRAM_CLK_DISABLE[9:8] = [CS1, CS0] = 0x0
	BITSET(denali_ctl(0),0x1); //START[0] = 1
//	while(!(denali_ctl(46)&(0x20000)));
//	BITSET(denali_ctl(47), 0x20000);
	BITCSET(denali_ctl(20), 0xFF000000, ((2<<2)|(0<<1)|(1))<<24);
	while(!(denali_ctl(46)&(0x40)));
	BITSET(denali_ctl(47), 0x40);

//--------------------------------------------------------------------------
// MRS Write

	// MRS2
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(28)&0xFFFF0000)>>16);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(31)&0x0000FFFF)<<16);
	denali_ctl(26) = (2<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

	// MRS3
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(29)&0xFFFF0000)>>16);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(32)&0x0000FFFF)<<16);
	denali_ctl(26) = (3<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

	// MRS1
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(28)&0x0000FFFF)>>0);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(30)&0xFFFF0000)<<0);
	denali_ctl(26) = (1<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

	// MRS0
	BITCSET(denali_ctl(29), 0x0000FFFF, (denali_ctl(27)&0x00FFFF00)>>8);
	BITCSET(denali_ctl(31), 0xFFFF0000, (denali_ctl(30)&0x0000FFFF)<<16);
	denali_ctl(26) = (0<<0)|(1<<23)|(1<<24)|(1<<25); // MR Select[7-0], MR enable[23], All CS[24], Trigger[25]
	while(!(denali_ctl(46)&(0x8000)));
	BITSET(denali_ctl(47), 0x8000);

//--------------------------------------------------------------------------

	while(!(denali_phy(0x24)&(1<<10))); //ctrl_locked, DLL Locked ...

	BITCLR(denali_ctl(40) ,0x1<<16); //ZQCS_ROTATE=0x0
	BITCSET(denali_ctl(39) ,0x3<<16, 0x2<<16); //ZQ_REQ=2 : 0x1=short calibration, 0x2=long calibration

//--------------------------------------------------------------------------
// release holding to access to dram

	cnt=1000; while(cnt--); // for reset DLL on DDR3
//	BITCLR(denali_ctl(44),0x1); //inhibit_dram_cmd=0

#elif defined(CONFIG_DRAM_DDR2)
	{
		FuncPtr pFunc = (FuncPtr)(SDRAM_INIT_FUNC_ADDR);
		pFunc();
	}
#elif defined(CONFIG_DRAM_LPDDR2)
	#error "LPDDR2 is not implemented"
#else
	#error "not selected"
#endif

// -------------------------------------------------------------------------
// mmu & cache on

	__asm__ volatile (
	"mrc p15, 0, r0, c1, c0, 0 \n"
	"orr r0, r0, #(1<<12) \n" //ICache
	"orr r0, r0, #(1<<2) \n" //DCache
	"orr r0, r0, #(1<<0) \n" //MMU
	"mcr p15, 0, r0, c1, c0, 0 \n"
	"nop \n"
	);
}

/*===========================================================================
VARIABLE
===========================================================================*/
static TCC_REG RegRepo;

#if defined(TCC_PM_SLEEP_WFI_USED)
static unsigned int uiRegBackup0;
static unsigned int uiRegBackup1;
#endif

/*===========================================================================
FUNCTION
===========================================================================*/
static void sleep_mode(void)
{
	volatile unsigned int cnt = 0;
	unsigned stack;
	unsigned way_mask;
	FuncPtr  pFunc = (FuncPtr )SLEEP_FUNC_ADDR;

	/*--------------------------------------------------------------
	 replace pm functions
	--------------------------------------------------------------*/
	memcpy((void*)SLEEP_FUNC_ADDR,       (void*)sleep,  SLEEP_FUNC_SIZE);
	memcpy((void*)SDRAM_INIT_FUNC_ADDR, (void*)sdram_init, SDRAM_INIT_FUNC_SIZE);

	/*--------------------------------------------------------------
	 CKC & PMU
	--------------------------------------------------------------*/
	memcpy(&RegRepo.pmu, (PPMU)tcc_p2v(HwPMU_BASE), sizeof(PMU));
	memcpy(&RegRepo.ckc, (PCKC)tcc_p2v(HwCKC_BASE), sizeof(CKC));

#if defined(TCC_PM_SLEEP_WFI_USED)
	uiRegBackup0 = ((PPIC)tcc_p2v(HwPIC_BASE))->IEN0.nREG;
	uiRegBackup1 = ((PPIC)tcc_p2v(HwPIC_BASE))->IEN1.nREG;
#endif

#ifdef CONFIG_CACHE_L2X0
	/*--------------------------------------------------------------
	 L2 cache
	--------------------------------------------------------------*/
	RegRepo.L2_aux = *(volatile unsigned int *)(L2CACHE_BASE+L2X0_AUX_CTRL); //save aux
	if (RegRepo.L2_aux & (1 << 16))
		way_mask = (1 << 16) - 1;
	else
		way_mask = (1 << 8) - 1;
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CLEAN_WAY) = way_mask; //clean all
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CLEAN_WAY)&way_mask); //wait for clean
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC) = 0; //sync
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC)&1); //wait for sync
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 0; //cache off
#endif

	/*--------------------------------------------------------------
	 flush tlb & cache
	--------------------------------------------------------------*/
	local_flush_tlb_all();
	flush_cache_all();

	stack = IO_ARM_ChangeStackSRAM();
	/////////////////////////////////////////////////////////////////
	pFunc();
	/////////////////////////////////////////////////////////////////
	IO_ARM_RestoreStackSRAM(stack);

#if defined(TCC_PM_SLEEP_WFI_USED)
	((PPIC)tcc_p2v(HwPIC_BASE))->IEN0.nREG = uiRegBackup0;
	((PPIC)tcc_p2v(HwPIC_BASE))->IEN1.nREG = uiRegBackup1;
#endif

	/*--------------------------------------------------------------
	 CKC & PMU
	--------------------------------------------------------------*/

	//memcpy((PCKC)tcc_p2v(HwCKC_BASE), &RegRepo.ckc, sizeof(CKC));
	{
		//PLL
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL0CFG.nREG = RegRepo.ckc.PLL0CFG.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL1CFG.nREG = RegRepo.ckc.PLL1CFG.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL2CFG.nREG = RegRepo.ckc.PLL2CFG.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->PLL3CFG.nREG = RegRepo.ckc.PLL3CFG.nREG;
		//((PCKC)tcc_p2v(HwCKC_BASE))->PLL4CFG.nREG = RegRepo.ckc.PLL4CFG.nREG; //PLL4 is used as a source of memory bus clock
		//((PCKC)tcc_p2v(HwCKC_BASE))->PLL5CFG.nREG = RegRepo.ckc.PLL5CFG.nREG; //PLL5 is used as a source of CPU bus clock
		nop_delay(1000);

		//Divider
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKDIVC0.nREG = RegRepo.ckc.CLKDIVC0.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKDIVC1.nREG = RegRepo.ckc.CLKDIVC1.nREG;
		nop_delay(100);

		//((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL0.nREG = RegRepo.ckc.CLKCTRL0.nREG; //CPU Clock can't be adjusted freely.
		//((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL1.nREG = RegRepo.ckc.CLKCTRL1.nREG; //Memory Clock can't be adjusted freely.
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL2.nREG = RegRepo.ckc.CLKCTRL2.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL3.nREG = RegRepo.ckc.CLKCTRL3.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL4.nREG = RegRepo.ckc.CLKCTRL4.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL5.nREG = RegRepo.ckc.CLKCTRL5.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL6.nREG = RegRepo.ckc.CLKCTRL6.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL7.nREG = RegRepo.ckc.CLKCTRL7.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->CLKCTRL8.nREG = RegRepo.ckc.CLKCTRL8.nREG;
		((PCKC)tcc_p2v(HwCKC_BASE))->SWRESET.nREG = ~(RegRepo.ckc.SWRESET.nREG);

		//Peripheral clock
		memcpy((void*)&(((PCKC)tcc_p2v(HwCKC_BASE))->PCLKCTRL00.nREG), (void*)&(RegRepo.ckc.PCLKCTRL00.nREG), 0x150-0x80);
	}
	nop_delay(100);

#if defined(TCC_PM_PMU_CTRL)
	//memcpy((PPMU)tcc_p2v(HwPMU_BASE), &RegRepo.pmu, sizeof(PMU));
	{
		while(((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.MAIN_STATE);

		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_HSB){ //if High Speed I/O Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.HSIOBUS = 0;
			while (((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.HSIOBUS == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.HSB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_HSBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_HSB == 0);
		}
		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_DB){ //if DDI Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.DBUS = 0;
			while ((((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.DBUS0 | ((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.DBUS1) == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.DB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_DBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_DB == 0);
		}
		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_GB){ //if Graphic Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.GBUS = 0;
			while (((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.GBUS == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.GB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_GBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_GB == 0);
		}
		if(RegRepo.pmu.PMU_PWRSTS.bREG.PD_VB){ //if Video Bus has been turn off.
			((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->HCLKMASK.bREG.VBUS = 0;
			while ((((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.VBUS0 | ((PMEMBUSCFG)tcc_p2v(HwMBUSCFG_BASE))->MBUSSTS.bREG.VBUS1) == 1);
			((PPMU)tcc_p2v(HwPMU_BASE))->PMU_SYSRST.bREG.VB = 0;
			((PPMU)tcc_p2v(HwPMU_BASE))->PWRDN_VBUS.bREG.DATA = 1;
			while (((PPMU)tcc_p2v(HwPMU_BASE))->PMU_PWRSTS.bREG.PD_VB == 0);
		}

		//IP isolation restore
		//Bruce, PMU_ISOL is only write-only.. so it can't be set to back-up data..
		//((PPMU)tcc_p2v(HwPMU_BASE))->PMU_ISOL.nREG = RegRepo.PMU_ISOL.nREG;
		((PPMU)tcc_p2v(HwPMU_BASE))->PMU_ISOL.nREG = 0x00000000;
	}
#endif

#ifdef CONFIG_CACHE_L2X0
	/*--------------------------------------------------------------
	 L2 cache enable
	--------------------------------------------------------------*/
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 0; //cache off
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_AUX_CTRL) = RegRepo.L2_aux; //restore aux
	if (RegRepo.L2_aux & (1 << 16))
		way_mask = (1 << 16) - 1;
	else
		way_mask = (1 << 8) - 1;
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_INV_WAY) = way_mask; //invalidate all
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_INV_WAY)&way_mask); //wait for invalidate
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC) = 0; //sync
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC)&1); //wait for sync
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 1; //cache on
#endif

}
#endif




/*===========================================================================

                        Power Management Driver

===========================================================================*/
#if defined(CONFIG_MMC_TCC_SDHC)
extern int tcc892x_sd_card_detect(void);
#endif

/*===========================================================================
FUNCTION
	mode 0: sleep mode, mode 1: shut down mode
===========================================================================*/
static int tcc_pm_enter(suspend_state_t state)
{
	unsigned long flags;

#if defined(TCC_PM_SLEEP_WFI_USED)
	#if LOG_NDEBUG
		volatile PPIC pPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
		unsigned reg_backup[20];

		reg_backup[0] = pPIC->STS0.nREG;
		reg_backup[1] = pPIC->STS1.nREG;
	#endif
#endif

// -------------------------------------------------------------------------
// disable interrupt
	local_irq_save(flags);
	local_irq_disable();

// -------------------------------------------------------------------------
// set board information
	*(volatile unsigned long *)SRAM_STACK_ADDR = system_rev;

#if defined(CONFIG_MMC_TCC_SDHC)
	*(volatile unsigned long *)(SRAM_STACK_ADDR+4) = tcc892x_sd_card_detect();
#endif

#if defined(TCC_PM_TP_DEBUG)	// Start
	gpio_set_value(TP_GPIO_PORT, 1);
#endif

// -------------------------------------------------------------------------
// enter shutdown mode
#if defined(CONFIG_SHUTDOWN_MODE)
	shutdown_mode();
#elif defined(CONFIG_SLEEP_MODE)
	sleep_mode();
#endif

#if defined(TCC_PM_TP_DEBUG)	// End
	gpio_set_value(TP_GPIO_PORT, 0);
#endif

// -------------------------------------------------------------------------
// enable interrupt
	local_irq_restore(flags);

#if defined(TCC_PM_SLEEP_WFI_USED)
	#if LOG_NDEBUG
		reg_backup[2] = pPIC->STS0.nREG;
		reg_backup[3] = pPIC->STS1.nREG;
	#endif
#endif

	return 0;
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void tcc_pm_power_off(void)
{
#ifdef CONFIG_RTC_DISABLE_ALARM_FOR_PWROFF_STATE		//Disable the RTC Alarm during the power off state
	extern volatile void tca_alarm_disable(unsigned int rtcbaseaddress);

	tca_alarm_disable(tcc_p2v(HwRTC_BASE));
#endif

#if defined(CONFIG_REGULATOR_AXP192)
	extern void axp192_power_off(void);
	axp192_power_off();
#elif defined(CONFIG_REGULATOR_RN5T614)
	extern void rn5t614_power_off(void);
	rn5t614_power_off();
	if(system_rev == 0x1008)
	    gpio_set_value(TCC_GPC(9),0);
#endif

	if(machine_is_m801_88() || machine_is_m803()){
		gpio_set_value(TCC_GPA(4), 0);  // LCD_BLCTL
		gpio_set_value(TCC_GPG(2), 0);  // LCD_PWREN
		gpio_set_value(TCC_GPF(26), 0); // SHDN
	}
	else if(machine_is_m805_892x())
	{
		if (system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
			gpio_set_value(TCC_GPC(0), 0); // LCD_BLCTL
			gpio_set_value(TCC_GPE(24), 0); // LCD_PWREN
			gpio_set_value(TCC_GPC(13), 0);  // PMU_HOLD

		} else {
			gpio_set_value(TCC_GPD(10), 0); // LCD_BLCTL
			gpio_set_value(TCC_GPD(18), 0); // LCD_PWREN
			gpio_set_value(TCC_GPE(7), 0);  // SHDN
		}
	}

	while(1);
}

/*===========================================================================
VARIABLE
===========================================================================*/
static struct platform_suspend_ops tcc_pm_ops = {
	.valid   = suspend_valid_only_mem,
	.enter   = tcc_pm_enter,
};

/*===========================================================================
VARIABLE
===========================================================================*/
static uint32_t restart_reason = 0x776655AA;

/*===========================================================================
FUNCTION
===========================================================================*/
static void tcc_pm_restart(char str)
{
	/* store restart_reason to USTS register */
	if (restart_reason != 0x776655AA)
		((PPMU)tcc_p2v(HwPMU_BASE))->PMU_USSTATUS.nREG = restart_reason;

	//printk(KERN_DEBUG "reboot: reason=0x%x\n", restart_reason);

	arch_reset(str, NULL);
}

/*===========================================================================
FUNCTION
===========================================================================*/
static int tcc_reboot_call(struct notifier_block *this, unsigned long code, void *cmd)
{
	/* XXX: convert reboot mode value because USTS register
	 * hold only 8-bit value
	 */
	if (code == SYS_RESTART) {
		if (cmd) {
			if (!strcmp(cmd, "bootloader")) {
				restart_reason = 1;	/* fastboot mode */
			} else if (!strcmp(cmd, "recovery")) {
				restart_reason = 2;	/* recovery mode */
			} else {
				restart_reason = 0;
			}
		} else {
			restart_reason = 0;
		}
	}
	return NOTIFY_DONE;
}

/*===========================================================================
VARIABLE
===========================================================================*/
static struct notifier_block tcc_reboot_notifier = {
	.notifier_call = tcc_reboot_call,
};

/*===========================================================================
FUNCTION
===========================================================================*/
static int __init tcc_pm_init(void)
{
	pm_power_off = tcc_pm_power_off;
	arm_pm_restart = tcc_pm_restart;

	register_reboot_notifier(&tcc_reboot_notifier);

	suspend_set_ops(&tcc_pm_ops);

#if defined(TCC_PM_TP_DEBUG)	// Init
	tcc_gpio_config(TP_GPIO_PORT, GPIO_FN(0)|GPIO_PULLUP);
	gpio_request(TP_GPIO_PORT, "TestPort");

	gpio_direction_output(TP_GPIO_PORT, 0);
#endif

	return 0;
}

__initcall(tcc_pm_init);

