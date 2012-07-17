/*
 * linux/arch/arm/mach-tccxxxx/tcc_ddr.c
 *
 * Author:  <linux@telechips.com>
 * Created: November, 2010
 * Description: to change memory bus clock for Telechips chipset
 *
 * Copyright (C) Telechips, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <mach/hardware.h>
#include <asm/tlbflush.h>
#include <asm/cacheflush.h>
#include <mach/bsp.h>
#include <mach/tcc_ddr.h>
#include <mach/tca_lcdc.h>



/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

extern unsigned int IO_ARM_ChangeStackSRAM(void);
extern void IO_ARM_RestoreStackSRAM(unsigned int);
static unsigned int get_cycle(unsigned int tCK, unsigned int ps, unsigned int ck);

#define time2cycle(time, tCK)		((int)((time + tCK - 1) / tCK))

#define addr(x) (0xF0000000+x)

typedef void (*FuncPtr)(unsigned int type);

#if defined(CONFIG_DRAM_DDR3)
#define DDR_MIN_CLK      200
#elif defined(CONFIG_DRAM_DDR2)
#define DDR_MIN_CLK      180
#elif defined(CONFIG_DRAM_LPDDR2)
#define DDR_MIN_CLK      180
#endif

#if defined (CONFIG_MACH_TCC8800ST)
#define GPU_400MHz_USED
#endif

/*===========================================================================

                                 MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) TCC93xx
     << sram1 >>
     0x10000000(0xF0A00000) ------------------
                           | ckc change func  | 0x700
                     0x700  ------------------
                           | ckc change arg   | 0x80
                     0x780  ------------------
                           |      Stack       | 0x80
                     0x800  ------------------

---------------------------------------------------------------------------*/

#define CKC_CHANGE_FUNC_ADDR       0xF0A00000

#define CKC_CHANGE_FUNC_SIZE       0x700

#define CKC_CHANGE_ARG_BASE        0xF0A00700



/*===========================================================================

                          Clock Register Value

===========================================================================*/

typedef struct {
	unsigned int	pll;
	unsigned int	clkctrl;   
	unsigned int	freq;
} MEMCLK;

MEMCLK pIO_CKC_PLL_H[]	=
{
#if defined(CONFIG_DRAM_LPDDR2)
	{0x43019803, 0x00200018, 1020000},
	{0x4301BC03, 0x00200018, 1110000},
	{0x4301E803, 0x00200018, 1220000},
#endif
	{0x0200FC03, 0x00200018, 1260000},
	{0x02010403, 0x00200018, 1300000},
	{0x02011403, 0x00200018, 1380000},
	{0x02011A03, 0x00200018, 1410000},
	{0x02012203, 0x00200018, 1450000},
	{0x02013003, 0x00200018, 1520000},
	{0x02014003, 0x00200018, 1600000},
	{0x02015403, 0x00200018, 1700000},
	{0x42016803, 0x00200018, 1800000},
	{0x42017C03, 0x00200018, 1900000},
	{0x42019003, 0x00200018, 2000000},
	{0x4201A403, 0x00200018, 2100000},
	{0x4201B803, 0x00200018, 2200000},
	{0x4201CC03, 0x00200018, 2300000},
	{0x4201E003, 0x00200018, 2400000},
	{0x4201F403, 0x00200018, 2500000},
	{0x01010403, 0x00200018, 2600000},
	{0x01010E03, 0x00200018, 2700000},
	{0x01011803, 0x00200018, 2800000},
	{0x01012203, 0x00200018, 2900000},
	{0x01012C03, 0x00200018, 3000000},
	{0x01013603, 0x00200018, 3100000},
	{0x01014003, 0x00200018, 3200000},
#if !defined(CONFIG_DRAM_LPDDR2)
	{0x01014A03, 0x00200018, 3300000},
	{0x01015403, 0x00200018, 3400000},
	{0x01015E03, 0x00200018, 3500000},
	{0x41016803, 0x00200018, 3600000},
	{0x41017203, 0x00200018, 3700000},
	{0x41017C03, 0x00200018, 3800000},
	{0x41018603, 0x00200018, 3900000},
	{0x41019003, 0x00200018, 4000000},
#if defined(CONFIG_DRAM_DDR3)
	{0x4101E203, 0x00200018, 4820000},
	{0x0000FB03, 0x00200018, 5020000},
	{0x00010903, 0x00200018, 5300000},
#endif
#endif
};

MEMCLK pIO_CKC_PLL_L[]	=
{
#if defined(CONFIG_DRAM_LPDDR2)
	{0x01006603, 0x00200018, 1020000},
	{0x01006F03, 0x00200018, 1110000},
	{0x41007A03, 0x00200018, 1220000},
#endif
	{0x41007E03, 0x00200013, 1260000},
	{0x41008203, 0x00200013, 1300000},
	{0x41008A03, 0x00200013, 1380000},
	{0x41008D03, 0x00200013, 1410000},
	{0x41009103, 0x00200013, 1450000},
	{0x41009803, 0x00200013, 1520000},
	{0x4100A003, 0x00200013, 1600000},
	{0x00005503, 0x00200013, 1700000},
	{0x00005A03, 0x00200013, 1800000},
	{0x00005F03, 0x00200013, 1900000},
	{0x00006403, 0x00200013, 2000000},
	{0x00006903, 0x00200013, 2100000},
	{0x00006E03, 0x00200013, 2200000},
	{0x00007303, 0x00200013, 2300000},
	{0x40007803, 0x00200013, 2400000},
	{0x40007D03, 0x00200013, 2500000},
	{0x40008203, 0x00200013, 2600000},
	{0x40008703, 0x00200013, 2700000},
	{0x40008C03, 0x00200013, 2800000},
	{0x40009103, 0x00200013, 2900000},
	{0x40009603, 0x00200013, 3000000},
	{0x40009B03, 0x00200013, 3100000},
	{0x4000A003, 0x00200013, 3200000},
#if !defined(CONFIG_DRAM_LPDDR2)
	{0x4000A503, 0x00200013, 3300000},
	{0x4000AA03, 0x00200013, 3400000},
	{0x4000AF03, 0x00200013, 3500000},
	{0x4000B403, 0x00200013, 3600000},
	{0x4000B903, 0x00200013, 3700000},
	{0x4000BE03, 0x00200013, 3800000},
	{0x4000C303, 0x00200013, 3900000},
	{0x4000C803, 0x00200013, 4000000},
#endif
};


/*===========================================================================

                          Clock Change Argument

===========================================================================*/

#define CKC_CHANGE_ARG(x)   (*(volatile unsigned long *)(CKC_CHANGE_ARG_BASE + (4*(x))))

#if defined(CONFIG_DRAM_DDR3)
	enum {
		//Clock
		PLL_VALUE = 0,
		CKC_CTRL_VALUE,
		CLK_RATE,
		RESET,
		PMU_IDX,
		SOURCE,
		ONOFF,
		T_REFI, /* 0 <= t_refi < 2^17 */
		T_RFC, /* 0 <= t_rfc < 2^8 */
		T_RRD, /* 0 <= t_rrd < 2^4 */
		T_RP, /* 0 <= t_rp < 2^4 */
		T_RCD, /* 0 <= t_rcd < 2^4 */ 
		T_RC, /* 0 <= t_rc < 2^6 */ 
		T_RAS, /* 0 <= t_ras < 2^6 */ 
		T_WTR, /* 0 <= t_wtr < 2^4 */ 
		T_WR, /* 0 <= t_wr < 2^4 */ 
		T_RTP, /* 0 <= t_rtp < 2^4 */ 
		CL, /* 0 <= cl < 2^4 */ 
		WL, /* 0 <= wl < 2^4 */ 	
		RL, /* 0 <= rl < 2^4 */ 	
		T_FAW, /* 0 <= t_faw < 2^6 */ 	 
		T_XSR, /* 0 <= t_xsr < 2^11 */ 	 
		T_XP, /* 0 <= t_xp < 2^11 */ 	 
		T_CKE, /* 0 <= t_cke < 2^4 */
		T_MRD, /* 0 <= t_mrd < 2^4 */
		MR0_DDR3,
		MR1_DDR3,
		MR2_DDR3,
		MR3_DDR3,
		#ifndef MRS_ALWAYS_SETTING
		MRS_SET,
		#endif		
	};
#elif defined(CONFIG_DRAM_DDR2) || defined(CONFIG_DRAM_LPDDR2)
	enum {
		PLL_VALUE = 0,
		CKC_CTRL_VALUE,
		CLK_RATE,
		RESET,
		PMU_IDX,
		SOURCE,
		ONOFF,
		T_REFI,
		TROW,
		TDATA,
		TPOWER,
		T_WR,
		CL,
	};
#endif



#if defined(CONFIG_DRAM_DDR2)
/*===========================================================================

                             DDR2 Setting

===========================================================================*/

/*===========================================================================
FUNCTION
 - mem_freq : MHz unit
===========================================================================*/
static void get_ddr_param(unsigned int mem_freq)
{
	int tCK = 0;
	unsigned int nRFC, nRRD, nRP, nRCD, nRC, nRAS;
	unsigned int nWTR, nWR, nRTP, nCL, nWL, nRL;
	unsigned int nFAW, nXSR, nXP, nCKE, nMRD;


	tCK = 1000000/mem_freq; // tCK is pico sec unit

	//nCL = nRCD = nRP
	nCL = DDR2_CL;//((DDR2_CL > 5) ? 5 : DDR2_CL);

	CKC_CHANGE_ARG(CL) = nCL;
	CKC_CHANGE_ARG(T_REFI) = get_cycle(tCK, DDR2_tREFI_ps, 1);

	nRFC = get_cycle(tCK, DDR2_tRFC_ps, 1);

	nRRD = get_cycle(tCK, DDR2_tRRD_ps, DDR2_tRRD_ck);
	nRP  = nCL; //get_cycle(tCK, DDR2_tRP_ps,  DDR2_tRP_ck);
	nRCD = nCL; //get_cycle(tCK, DDR2_tRCD_ps, DDR2_tRCD_ck);
	nRC  = get_cycle(tCK, DDR2_tRC_ps,  DDR2_tRC_ck);
	nRAS = get_cycle(tCK, DDR2_tRAS_ps, DDR2_tRAS_ck);

	CKC_CHANGE_ARG(TROW) = (nRFC<<24)|(nRRD<<20)|(nRP<<16)|(nRCD<<12)|(nRC<<6)|(nRAS);

	nWTR = get_cycle(tCK, DDR2_tWTR_ps, DDR2_tWTR_ck);
	nWR  = get_cycle(tCK, DDR2_tWR_ps, DDR2_tWR_ck);
	nRTP = get_cycle(tCK, DDR2_tRTP_ps, DDR2_tRTP_ck);
	nWL  = nCL -1;
	nRL  = nCL;
	CKC_CHANGE_ARG(TDATA) = (nWTR<<28)|(nWR<<24)|(nRTP<<20)|(nCL<<16)|(nWL<<8)|(nRL);

	nFAW = get_cycle(tCK, DDR2_tFAW_ps, DDR2_tFAW_ck);
	nXSR = DDR2_tXSR_ck;
	nXP  = DDR2_tXP_ck;
	nCKE = DDR2_tCKE_ck;
	nMRD = DDR2_tMRD_ck;
	CKC_CHANGE_ARG(TPOWER) = (nFAW<<24)|(nXSR<<16)|(nXP<<8)|(nCKE<<4)|(nMRD);

	CKC_CHANGE_ARG(T_WR) = nWR;

	CKC_CHANGE_ARG(CLK_RATE) = mem_freq;

	//printk("FREQ:%d, T_REFI:0x%x, TROW:0x%x, TDATA:0x%x, TPOWER:0x%x, T_WR:0x%x\n",mem_freq,CKC_CHANGE_ARG(T_REFI),CKC_CHANGE_ARG(TROW),CKC_CHANGE_ARG(TDATA),CKC_CHANGE_ARG(TPOWER),CKC_CHANGE_ARG(T_WR));

}

/*===========================================================================
FUNCTION
===========================================================================*/
static void change_clock(unsigned int type)
{
	volatile unsigned int i = 0;			

//--------------------------------------------------------------------------
// Change to config mode

	*(volatile unsigned long *)addr(0x303020 ) &= ~Hw17;

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}

//--------------------------------------------------------------------------
// Clock setting..

	if(type == 0)
	{
		//mem bus - DirectPLL2/3(It is for asynchronous clock mode)
		*(volatile unsigned long *)addr(0x400008)  = 0x00200014;

#if defined(GPU_400MHz_USED) // GPU_400MHz
		if (*(volatile unsigned long*)addr(0x40000C) & 0x0020008)
			*(volatile unsigned long *)addr(0x40000C)  = 0x00200014;
#endif

		//set pll1 value
		*(volatile unsigned long *)addr(0x400050)  = CKC_CHANGE_ARG(PLL_VALUE); // PLL_PWR_OFF & SET PMS
		*(volatile unsigned long *)addr(0x400050) |= 0x80000000;			// PLL_PWR_ON
		i = 20; while(i--);

		//*(volatile unsigned long *)addr(0x400008)  = 0x00200013;
		*(volatile unsigned long *)addr(0x400008) = CKC_CHANGE_ARG(CKC_CTRL_VALUE);

#if defined(GPU_400MHz_USED) // GPU_400MHz
		if (*(volatile unsigned long*)addr(0x40000C) & 0x0020004)
			*(volatile unsigned long *)addr(0x40000C) = 0x00200018;
#endif
	}
	else //---------------------------------------------------------------------------
	{
		*(volatile unsigned long *)addr(0x400008)  = 0x00200014;	// mem
		*(volatile unsigned long *)addr(0x40001C)  = 0x00200014;	// smu

		if (CKC_CHANGE_ARG(ONOFF)) {
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << (CKC_CHANGE_ARG(PMU_IDX)+1));
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << (CKC_CHANGE_ARG(PMU_IDX)+2));
			*(volatile unsigned long *)addr(0x404018) |= (1 << CKC_CHANGE_ARG(PMU_IDX));
			*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (1<<CKC_CHANGE_ARG(RESET));
			if (CKC_CHANGE_ARG(RESET) == 5)
				*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (1<<(CKC_CHANGE_ARG(RESET)+1));
			*(volatile unsigned long *)addr(0x400000 + (4*CKC_CHANGE_ARG(SOURCE))) |= (1<<21);
	//		if (reset == 5)
	//			*(volatile unsigned long *)addr(0x400000 + (4*(src+1))) |= (1<<21);
		}
		else {
	//		if (reset == 5)
	//			*(volatile unsigned long *)addr(0x400000 + (4*(src+1))) &= ~(1<<21);
			*(volatile unsigned long *)addr(0x400000 + (4*CKC_CHANGE_ARG(SOURCE))) &= ~(1<<21);
			if (CKC_CHANGE_ARG(RESET) == 5)
				*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(1<<(CKC_CHANGE_ARG(RESET)+1));
			*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(1<<CKC_CHANGE_ARG(RESET));
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << CKC_CHANGE_ARG(PMU_IDX));
			*(volatile unsigned long *)addr(0x404018) |= (1 << (CKC_CHANGE_ARG(PMU_IDX)+1));
			*(volatile unsigned long *)addr(0x404018) |= (1 << (CKC_CHANGE_ARG(PMU_IDX)+2));
		}

		*(volatile unsigned long *)addr(0x400008)  = CKC_CHANGE_ARG(CKC_CTRL_VALUE);	// mem
		*(volatile unsigned long *)addr(0x40001C)  = 0x00200039;	// smu
	}

//--------------------------------------------------------------------------
// Controller setting

	//phy configuration
	*(volatile unsigned long *)addr(0x303024 ) = 0x200;//PHYCFG

	//PhyZQControl
	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1 ;//zq start
	} else {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw0;
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1 | Hw0 ;//zq start
	}
	while (((*(volatile unsigned long *)addr(0x306040)) & (0x10000)) != 0x10000);	// Wait until ZQ End

	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;
	} else {
		*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw0;
	}

	*(volatile unsigned long *)addr(0x306018 ) = 0x0010100A; //PHY Control0
	*(volatile unsigned long *)addr(0x30601C ) = 0xE0000086; //PHY Control1 // modify by crony : [31:30] : ODT Enable for Write and Read
	*(volatile unsigned long *)addr(0x306020 ) = 0x00000000; //PHY Control2
	*(volatile unsigned long *)addr(0x306024 ) = 0x00000000; //PHY Control3
	*(volatile unsigned long *)addr(0x306018 ) = 0x0010100B; //PHY Control0

	while (((*(volatile unsigned long *)addr(0x306040)) & (0x7)) != 0x7);// Wait until FLOCK == 1

	//PHY Control1
	*(volatile unsigned long *)addr(0x30601C) = 0xE000008E; //resync = 1
	*(volatile unsigned long *)addr(0x30601C) = 0xE0000086; //resync = 0

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr(0x306000 ) = 0x30FF2018;

	//MEMCTRL
	*(volatile unsigned long *)addr(0x306004 ) = (0x2 << 20) |
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
	*(volatile unsigned long *)addr(0x306008 ) = (0x40<<24) |
	                                             ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((DDR2_COLBITS - 7)<<8) |
	                                             ((DDR2_ROWBITS - 12)<<4) |
	                                             DDR2_BANK_BITS;

	//MEMCHIP1
	if(DDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr(0x30600C ) = ((0x40 + DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (DDR2_TOTAL_MB_SIZE/(DDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((DDR2_COLBITS - 7)<<8) |
		                                         ((DDR2_ROWBITS - 12)<<4) |
		                                         DDR2_BANK_BITS;

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x306014 ) = 0x0; //PRECONFIG
	*(volatile unsigned long *)addr(0x306028 ) = 0xFFFF00FF; //PRECONFIG

//--------------------------------------------------------------------------
// Timing parameter setting.

	*(volatile unsigned long *)addr(0x306030 ) = CKC_CHANGE_ARG(T_REFI);
	*(volatile unsigned long *)addr(0x306034 ) = CKC_CHANGE_ARG(TROW);
	*(volatile unsigned long *)addr(0x306038 ) = CKC_CHANGE_ARG(TDATA);
	*(volatile unsigned long *)addr(0x30603C ) = CKC_CHANGE_ARG(TPOWER);

//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr(0x306010 ) = 0x07000000;//NOP
	*(volatile unsigned long *)addr(0x306010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr(0x306010 ) = 0x00020000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00030000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00010000;
	*(volatile unsigned long *)addr(0x306010 ) = 0x00000100;
	*(volatile unsigned long *)addr(0x306010 ) = 0x01000000;//precharge all
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x05000000;//AREF
	*(volatile unsigned long *)addr(0x306010 ) = 0x00000000;	// DLL reset release.
	*(volatile unsigned long *)addr(0x306010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(CKC_CHANGE_ARG(CL)<<4)|((CKC_CHANGE_ARG(T_WR)-1)<<9));	// BurstLength 4, CL , WR 
	i = 20; while(i--);
	*(volatile unsigned long *)addr(0x306010 ) = 0x00010380; // OCD Calibration default
	i = 20; while(i--);
	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ)
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1);
	else
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_DIC<<1);

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		*(volatile unsigned long *)addr(0x306010 ) = 0x07000000 | Hw20;//NOP
		*(volatile unsigned long *)addr(0x306010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr(0x306010 ) = 0x00020000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00030000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x00000100 | Hw20;
		*(volatile unsigned long *)addr(0x306010 ) = 0x01000000 | Hw20;//precharge all
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x05000000 | Hw20;//AREF
		*(volatile unsigned long *)addr(0x306010 ) = 0x00000000 | Hw20;	// DLL reset release.
		*(volatile unsigned long *)addr(0x306010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(CKC_CHANGE_ARG(CL)<<4)|((CKC_CHANGE_ARG(T_WR)-1)<<9)) | Hw20;	// BurstLength 4, CL , WR 
		i = 20; while(i--);
		*(volatile unsigned long *)addr(0x306010 ) = 0x00010380 | Hw20; // OCD Calibration default
		i = 20; while(i--);
		if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ)
			*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1) | Hw20;
		else
			*(volatile unsigned long *)addr(0x306010 ) = 0x00010000 | (DDR2_DIC<<1) | Hw20;
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x303020 ) =  0x0007010A;//EMCCFG
	*(volatile unsigned long *)addr(0x306000 ) |= 0x20;

	if(DDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//DDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR2_LOGICAL_CHIP_NUM == 2){
		if(DDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//DDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}
}

#elif defined(CONFIG_DRAM_LPDDR2)

/*===========================================================================

                             LPDDR2 Setting

===========================================================================*/

/*===========================================================================
FUNCTION
 - mem_freq : MHz unit
===========================================================================*/
static void get_ddr_param(unsigned int mem_freq)
{
	int tCK = 0;
	unsigned int nRFC, nRRD, nRP, nRCD, nRC, nRAS;
	unsigned int nWTR, nWR, nRTP;
	unsigned int nFAW, nXSR, nXP, nCKE;


	tCK = 1000000/mem_freq; // tCK is pico sec unit

	CKC_CHANGE_ARG(T_REFI) = get_cycle(tCK, LPDDR2_tREFI_ps, 1);

	nRFC = get_cycle(tCK, LPDDR2_tRFC_ps, 1);

	nRRD = get_cycle(tCK, LPDDR2_tRRD_ps, LPDDR2_tRRD_ck);
	nRP  = get_cycle(tCK, LPDDR2_tRP_ps,  LPDDR2_tRP_ck);
	nRCD = get_cycle(tCK, LPDDR2_tRCD_ps, LPDDR2_tRCD_ck);
	nRC  = get_cycle(tCK, LPDDR2_tRC_ps,  LPDDR2_tRC_ck);
	nRAS = get_cycle(tCK, LPDDR2_tRAS_ps, LPDDR2_tRAS_ck);

	CKC_CHANGE_ARG(TROW) = (nRFC<<24)|(nRRD<<20)|(nRP<<16)|(nRCD<<12)|(nRC<<6)|(nRAS);

	nWTR = get_cycle(tCK, LPDDR2_tWTR_ps, LPDDR2_tWTR_ck);
	nWR  = get_cycle(tCK, LPDDR2_tWR_ps, LPDDR2_tWR_ck);
	nRTP = get_cycle(tCK, LPDDR2_tRTP_ps, LPDDR2_tRTP_ck);
	CKC_CHANGE_ARG(TDATA) = (nWTR<<28)|(nWR<<24)|(nRTP<<20)|(LPDDR2_tWL_ck<<8)|(LPDDR2_tRL_ck);

	nFAW = get_cycle(tCK, LPDDR2_tFAW_ps, LPDDR2_tFAW_ck);
	nXSR = get_cycle(tCK, LPDDR2_tXSR_ps, LPDDR2_tXSR_ck);
	nXP  = get_cycle(tCK, LPDDR2_tXP_ps,  LPDDR2_tXP_ck);
	nCKE = get_cycle(tCK, LPDDR2_tCKE_ps, LPDDR2_tCKE_ck);
	CKC_CHANGE_ARG(TPOWER) = (nFAW<<24)|(nXSR<<16)|(nXP<<8)|(nCKE<<4)|(LPDDR2_tMRD_ck);

	CKC_CHANGE_ARG(T_WR) = nWR;

	CKC_CHANGE_ARG(CLK_RATE) = mem_freq;

	//printk("FREQ:%d, T_REFI:0x%x, TROW:0x%x, TDATA:0x%x, TPOWER:0x%x, T_WR:0x%x\n",mem_freq,CKC_CHANGE_ARG(T_REFI),CKC_CHANGE_ARG(TROW),CKC_CHANGE_ARG(TDATA),CKC_CHANGE_ARG(TPOWER),CKC_CHANGE_ARG(T_WR));

}

/*===========================================================================
FUNCTION
===========================================================================*/
static void change_clock(unsigned int type)
{
	volatile unsigned int i = 0;			

//--------------------------------------------------------------------------
// Change to config mode

	*(volatile unsigned long *)addr(0x303020 ) &= ~Hw17;

	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

	if(LPDDR2_LOGICAL_CHIP_NUM == 2){
		if(LPDDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//LPDDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}

//--------------------------------------------------------------------------
// Clock setting..

	if(type == 0)
	{
		//mem bus - DirectPLL2/3(It is for asynchronous clock mode)
		*(volatile unsigned long *)addr(0x400008)  = 0x00200014;

		//set pll1 value
		*(volatile unsigned long *)addr(0x400050)  = CKC_CHANGE_ARG(PLL_VALUE); // PLL_PWR_OFF & SET PMS
		*(volatile unsigned long *)addr(0x400050) |= 0x80000000;			// PLL_PWR_ON
		i = 20; while(i--);

		//*(volatile unsigned long *)addr(0x400008)  = 0x00200013;
		*(volatile unsigned long *)addr(0x400008) = CKC_CHANGE_ARG(CKC_CTRL_VALUE);
	}
	else //---------------------------------------------------------------------------
	{
		*(volatile unsigned long *)addr(0x400008)  = 0x00200014;	// mem
		*(volatile unsigned long *)addr(0x40001C)  = 0x00200014;	// smu

		if (CKC_CHANGE_ARG(ONOFF)) {
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << (CKC_CHANGE_ARG(PMU_IDX)+1));
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << (CKC_CHANGE_ARG(PMU_IDX)+2));
			*(volatile unsigned long *)addr(0x404018) |= (1 << CKC_CHANGE_ARG(PMU_IDX));
			*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (1<<CKC_CHANGE_ARG(RESET));
			if (CKC_CHANGE_ARG(RESET) == 5)
				*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (1<<(CKC_CHANGE_ARG(RESET)+1));
			*(volatile unsigned long *)addr(0x400000 + (4*CKC_CHANGE_ARG(SOURCE))) |= (1<<21);
	//		if (reset == 5)
	//			*(volatile unsigned long *)addr(0x400000 + (4*(src+1))) |= (1<<21);
		}
		else {
	//		if (reset == 5)
	//			*(volatile unsigned long *)addr(0x400000 + (4*(src+1))) &= ~(1<<21);
			*(volatile unsigned long *)addr(0x400000 + (4*CKC_CHANGE_ARG(SOURCE))) &= ~(1<<21);
			if (CKC_CHANGE_ARG(RESET) == 5)
				*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(1<<(CKC_CHANGE_ARG(RESET)+1));
			*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(1<<CKC_CHANGE_ARG(RESET));
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << CKC_CHANGE_ARG(PMU_IDX));
			*(volatile unsigned long *)addr(0x404018) |= (1 << (CKC_CHANGE_ARG(PMU_IDX)+1));
			*(volatile unsigned long *)addr(0x404018) |= (1 << (CKC_CHANGE_ARG(PMU_IDX)+2));
		}

		*(volatile unsigned long *)addr(0x400008)  = CKC_CHANGE_ARG(CKC_CTRL_VALUE);	// mem
		*(volatile unsigned long *)addr(0x40001C)  = 0x00200039;	// smu
	}

//--------------------------------------------------------------------------
// Controller setting

	//phy configuration
	*(volatile unsigned long *)addr(0x303024) = 0x200;//PHYCFG
	//*(volatile unsigned long *)addr(0x304400) = 0x00000509; //PHYMODE , LPDDR2 , diff DQS , under 333MHz

//--------------------------------------------------------------------------
// ZQ Calibration

	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL;
	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL | Hw1;//zq start
	while (((*(volatile unsigned long *)addr(0x306040)) & (0x10000)) != 0x10000);	// Wait until ZQ End
	*(volatile unsigned long *)addr(0x306044 ) = PHYZQCTRL ;

//--------------------------------------------------------------------------
// Phy config

	if(CKC_CHANGE_ARG(CLK_RATE) < LPDDR2_DLL_LOW_FREQ_MODE_LIMIT)
		*(volatile unsigned long *)addr(0x306018) = (1<<3)|(1<<2); // ctrl_dfdqs, ctrl_half
	else
		*(volatile unsigned long *)addr(0x306018) = (1<<3); // ctrl_dfdqs

	*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(4<<0); // ctrl_cmosrcv, ctrl_ref, ctrl_shiftc
	*(volatile unsigned long *)addr(0x306020) = 0;

	if(CKC_CHANGE_ARG(CLK_RATE) < LPDDR2_DLL_LOW_FREQ_MODE_LIMIT){
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<2)|(1<<1); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_half, ctrl_dll_on
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<2)|(1<<1)|(1<<0); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_half, ctrl_dll_on, ctrl_start
	}else{
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<1); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_dll_on
		*(volatile unsigned long *)addr(0x306018) = (20<<16)|(20<<8)|(1<<3)|(1<<1)|(1<<0); // ctrl_inc, ctrl_start_point, ctrl_dfdqs, ctrl_dll_on, ctrl_start
	}

//--------------------------------------------------------------------------
// Memory config

	//Enable Out of order scheduling
	*(volatile unsigned long *)addr(0x306000) =
		  ( 3 << 28) // phy_type, DDR3PHY
		| (0xff << 16) // timeout_cnt
		| ( 2 << 12) // rd_fetch
		| ( 0 << 11) // qos_fast_en
		| ( 0 << 10) // dq_swap
		| ( 0 <<  9) // chip1_empty
		| ( 0 <<  8) // chip0_empty
		| ( 0 <<  7) // drv_en
		| ( 1 <<  6) // ctc_trt_gap_en
		| ( 1 <<  5) // aref_en   // auto refresh should be disabled at this moment
		| ( 1 <<  4) // out_of
		| ( 1 <<  3) // div_pipe
		| ( 0 <<  1) // clk_ratio
		| ( 0 <<  0) // async
		;

	//MEMCTRL
	*(volatile unsigned long *)addr(0x306004 ) = (0x2 << 20) |
	                                             ((LPDDR2_LOGICAL_CHIP_NUM-1)<<16) |
	                                             ((LPDDR2_LOGICAL_DATA_BITS/16)<<12) |
	                                             (0x2 << 8) |
	                                             (0x1 << 6) |
	                                             (0x0 << 5) |
	                                             (0x0 << 4) |
	                                             (0x0 << 2) |
	                                             (0x0 << 1) |
	                                             (0x0);
	//MEMCHIP0
	*(volatile unsigned long *)addr(0x306008 ) = (0x40<<24) |
	                                             ((0xFF - (LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
	                                             (0x1 << 12) |
	                                             ((LPDDR2_COLBITS - 7)<<8) |
	                                             ((LPDDR2_ROWBITS - 12)<<4) |
	                                             LPDDR2_BANK_BITS;
	//MEMCHIP1
	if(LPDDR2_LOGICAL_CHIP_NUM == 2)
	*(volatile unsigned long *)addr(0x30600C ) = ((0x40 + LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10))<<24) |
		                                         ((0xFF - (LPDDR2_TOTAL_MB_SIZE/(LPDDR2_LOGICAL_CHIP_NUM*0x10)-1))<<16) |
		                                         (0x1 << 12) |
		                                         ((LPDDR2_COLBITS - 7)<<8) |
		                                         ((LPDDR2_ROWBITS - 12)<<4) |
		                                         LPDDR2_BANK_BITS;


//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x306014 ) = 0x0; //PRECONFIG
	//*(volatile unsigned long *)addr(0x306028 ) = 0xFFFF00FF; //PWRDNCONFIG


//--------------------------------------------------------------------------
// Timing parameter setting.

	*(volatile unsigned long *)addr(0x306030 ) = CKC_CHANGE_ARG(T_REFI);
	*(volatile unsigned long *)addr(0x306034 ) = CKC_CHANGE_ARG(TROW);
	*(volatile unsigned long *)addr(0x306038 ) = CKC_CHANGE_ARG(TDATA);
	*(volatile unsigned long *)addr(0x30603C ) = CKC_CHANGE_ARG(TPOWER);


//--------------------------------------------------------------------------
// If QoS scheme is required, set the QosControl0~15 and QosConfig0~15 registers.
    //0xf0306060

//--------------------------------------------------------------------------
// Check whether PHY DLL is locked.

	while (((*(volatile unsigned long *)addr(0x306040)) & (0x4)) != 0x4);

	// Update DLL information
	*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(1<<3)|(4<<0); // ctrl_cmosrcv, ctrl_ref, fp_resync, ctrl_shiftc
	//*(volatile unsigned long *)addr(0x30601C) = (1<<23)|(8<<4)|(4<<0); // ctrl_cmosrcv, ctrl_ref, ctrl_shiftc

	i = 50; while(i--); // Wait 100 ns


//--------------------------------------------------------------------------
// MRS Setting

	//Direct Command
	*(volatile unsigned long *)addr(0x306010 ) = 0x07000000;//NOP
	// Wait 200 us 
	i = 50; while(i--);

	// MRW (Reset)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x3f&0xc0)>>6)|(((0x3f&0x38)>>3)<<16)|((0x3f&0x7)<<10)| // mr_addr
		((0x00)<<2); // mr_value
	i = 10; while(i--); // Wait 1us

	// MRW (Device Feature1)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x01&0xc0)>>6)|(((0x01&0x38)>>3)<<16)|((0x01&0x7)<<10)| // mr_addr
		((((CKC_CHANGE_ARG(T_WR)-2)<<5)|0x2)<<2); // mr_value

	// MRW (Device Feature2)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x02&0xc0)>>6)|(((0x02&0x38)>>3)<<16)|((0x02&0x7)<<10)| // mr_addr
		((LPDDR2_tRL_ck-2)<<2); // mr_value

	// MRW (I/O Config-1)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x03&0xc0)>>6)|(((0x03&0x38)>>3)<<16)|((0x03&0x7)<<10)| // mr_addr
		((LPDDR2_DS)<<2); // mr_value

	// MRW (ZQ Calibration)
	//-- add ZQ calibration MRW here if LPDDR2 memmory supports ZQ calibration
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x0A&0xc0)>>6)|(((0x0A&0x38)>>3)<<16)|((0x0A&0x7)<<10)| // mr_addr
		((0xFF)<<2); // mr_value

	// MRR (Device Auto-Initialization, DAI, polling)
	*(volatile unsigned long *)addr(0x306010) = (0<<24)|(0<<20)| // cmd_type|chip_sel
		((0x00&0xc0)>>6)|(((0x00&0x38)>>3)<<16)|((0x00&0x7)<<10)| // mr_addr
		((0x00)<<2); // mr_value

	while((((*(volatile unsigned long *)addr(0x306010))&0x3FC)>>2)&0x01);
	i = 10; while(i--); // Wait 1us

	if(LPDDR2_LOGICAL_CHIP_NUM == 2)
	{
		//Direct Command
		*(volatile unsigned long *)addr(0x306010 ) = 0x07000000|Hw20;//NOP
		// Wait 200 us 
		i = 50; while(i--);

		// MRW (Reset)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x3f&0xc0)>>6)|(((0x3f&0x38)>>3)<<16)|((0x3f&0x7)<<10)| // mr_addr
			((0x00)<<2); // mr_value
		i = 10; while(i--); // Wait 1us

		// MRW (Device Feature1)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x01&0xc0)>>6)|(((0x01&0x38)>>3)<<16)|((0x01&0x7)<<10)| // mr_addr
			((((CKC_CHANGE_ARG(T_WR)-2)<<5)|0x2)<<2); // mr_value

		// MRW (Device Feature2)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x02&0xc0)>>6)|(((0x02&0x38)>>3)<<16)|((0x02&0x7)<<10)| // mr_addr
			((LPDDR2_tRL_ck-2)<<2); // mr_value

		// MRW (I/O Config-1)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x03&0xc0)>>6)|(((0x03&0x38)>>3)<<16)|((0x03&0x7)<<10)| // mr_addr
			((LPDDR2_DS)<<2); // mr_value

		// MRW (ZQ Calibration)
		//-- add ZQ calibration MRW here if LPDDR2 memmory supports ZQ calibration
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x0A&0xc0)>>6)|(((0x0A&0x38)>>3)<<16)|((0x0A&0x7)<<10)| // mr_addr
			((0xFF)<<2); // mr_value

		// MRR (Device Auto-Initialization, DAI, polling)
		*(volatile unsigned long *)addr(0x306010) = (0<<24)|(1<<20)| // cmd_type|chip_sel
			((0x00&0xc0)>>6)|(((0x00&0x38)>>3)<<16)|((0x00&0x7)<<10)| // mr_addr
			((0x00)<<2); // mr_value

		while((((*(volatile unsigned long *)addr(0x306010))&0x3FC)>>2)&0x01);
		i = 10; while(i--); // Wait 1us
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x303020 ) =  0x0007010A;//EMCCFG
	*(volatile unsigned long *)addr(0x306000 ) |= 0x20; // Auto Refresh Enable 

	if(LPDDR2_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x306048)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else//LPDDR2_BANK_NUM == 4
		while (((*(volatile unsigned long *)addr(0x306048)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(LPDDR2_LOGICAL_CHIP_NUM == 2){
		if(LPDDR2_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else//LPDDR2_BANK_NUM == 4
			while (((*(volatile unsigned long *)addr(0x30604C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}
}

#elif defined(CONFIG_DRAM_DDR3)

/*===========================================================================

                             DDR3 Setting

===========================================================================*/

/*===========================================================================
VARIABLE
===========================================================================*/

#ifndef MRS_ALWAYS_SETTING
static unsigned int gWR = 0;
static unsigned int gCL = 0;
#endif

/*===========================================================================
FUNCTION
 - mem_freq : MHz unit
===========================================================================*/
static void get_ddr_param(unsigned int mem_freq)
{
	unsigned int tCK = 0, nCL=0, nWR=0, nCWL=0, nAL=0;
	unsigned int arrMRS[4];

//--------------------------------------------------------------------------
// Calculate timing parameters.

	tCK = 1000000/mem_freq; // tCK is pico sec unit

	/* tRP = tRCD = CL */
	//CL, RP, RCD
	if(mem_freq < 400)
		nCL = DDR3_CL*400*2/DDR3_MAX_SPEED;
	else
		nCL = DDR3_CL*mem_freq*2/DDR3_MAX_SPEED;
	if(DDR3_CL*mem_freq%DDR3_MAX_SPEED != 0)
		nCL++;

	nWR = get_cycle(tCK, DDR3_tWR_ps, 1);

	if(tCK >= 2500 /* 2.5 ns */)
		nCWL = 5;
	else if(tCK >= 1875 /* 1.875 ns */)
		nCWL = 6;
	else if(tCK >= 1500 /* 1.5 ns */)
		nCWL = 7;
	else if(tCK >= 1250 /* 1.25 ns */)
		nCWL = 8;
	else if(tCK >= 1070 /* 1.07 ns */)
		nCWL = 9;
	else if(tCK >= 935 /* 0.935 ns */)
		nCWL = 10;
	else if(tCK >= 833 /* 0.833 ns */)
		nCWL = 11;
	else if(tCK >= 750 /* 0.75 ns */)
		nCWL = 12;

	if(DDR3_AL == AL_DISABLED)
		nAL = 0;
	else if(DDR3_AL == AL_CL_MINUS_ONE)
		nAL = nCL - 1;
	else if(DDR3_AL == AL_CL_MINUS_TWO)
		nAL = nCL - 2;

	// Set timing parameters.
	CKC_CHANGE_ARG(T_REFI) = get_cycle(tCK, DDR3_tREFI_ps, 1);
	CKC_CHANGE_ARG(T_RFC) = get_cycle(tCK, DDR3_tRFC_ps, 1);
	CKC_CHANGE_ARG(T_RRD) = get_cycle(tCK, DDR3_tRRD_ps, DDR3_tRRD_ck);
	CKC_CHANGE_ARG(T_RP) = nCL;//get_cycle(tCK, DDR3_tRP_ps, DDR3_tRP_ck);
	CKC_CHANGE_ARG(T_RCD) = nCL;//get_cycle(tCK, DDR3_tRCD_ps, DDR3_tRCD_ck);
	CKC_CHANGE_ARG(T_RC) = get_cycle(tCK, DDR3_tRC_ps, DDR3_tRC_ck);
	CKC_CHANGE_ARG(T_RAS) = get_cycle(tCK, DDR3_tRAS_ps, DDR3_tRAS_ck);
	CKC_CHANGE_ARG(T_WTR) = get_cycle(tCK, DDR3_tWTR_ps, DDR3_tWTR_ck);
	CKC_CHANGE_ARG(T_WR) = nWR;
	CKC_CHANGE_ARG(T_RTP) = get_cycle(tCK, DDR3_tRTP_ps, DDR3_tRTP_ck);
	CKC_CHANGE_ARG(CL) = nCL; 
	CKC_CHANGE_ARG(WL) = nAL + nCWL; 
	CKC_CHANGE_ARG(RL) = nAL + nCL;
	CKC_CHANGE_ARG(T_FAW) = get_cycle(tCK, DDR3_tFAW_ps, DDR3_tFAW_ck); 
	CKC_CHANGE_ARG(T_XSR) = get_cycle(tCK, DDR3_tXS_ps, DDR3_tXS_ck);
	CKC_CHANGE_ARG(T_XP) = get_cycle(tCK, DDR3_tXP_ps, DDR3_tXP_ck);
	CKC_CHANGE_ARG(T_CKE) = get_cycle(tCK, DDR3_tCKE_ps, DDR3_tCKE_ck);
	CKC_CHANGE_ARG(T_MRD) = DDR3_tMRD_ck;

#ifndef MRS_ALWAYS_SETTING
	if((nWR == gWR) && (nCL == gCL))
		CKC_CHANGE_ARG(MRS_SET) = 0;
	else
		CKC_CHANGE_ARG(MRS_SET) = 1;
	gWR = nWR; gCL = nCL;
#endif

//--------------------------------------------------------------------------
// Set mode registers

	/* MR0 */
	arrMRS[0] = 0;
	arrMRS[0] |= DDR3_BURST_LEN; // Burst Len
	arrMRS[0] |= DDR3_READ_BURST_TYPE*Hw3; // Read Burst Type

	if(nCL < 5) // CAS Latency
		arrMRS[0] |= (5-4)<<4;
	else if(nCL > 11)
		arrMRS[0] |= (11-4)<<4;
	else
		arrMRS[0] |= (nCL-4)<<4;

	if(nWR <= 5) // Write Recovery for autoprecharge
		arrMRS[0] |= WR_5<<9;
	else if(nWR == 6)
		arrMRS[0] |= WR_6<<9;
	else if(nWR == 7)
		arrMRS[0] |= WR_7<<9;
	else if(nWR == 8)
		arrMRS[0] |= WR_8<<9;
	else if(nWR == 9 || nWR == 10)
		arrMRS[0] |= WR_10<<9;
	else if(nWR >= 11)
		arrMRS[0] |= WR_12<<9;

	arrMRS[0] |= FAST_EXIT<<12; // DLL On

	/* MR1 */
	arrMRS[1] = Hw16;//MR1
	
	arrMRS[1] |= (DDR3_AL<<3);
	//arrMRS[1] |= (Hw12 | Hw11 | Hw6); //Rtt_Nom is RZQ/2, Don't enable "Write leveling enable", tDQS enable
	arrMRS[1] |= ((DDR3_ODT<<2) | (DDR3_DIC<<1));

	/* MR2 */
	arrMRS[2] = Hw17;//MR2
	if(mem_freq*2 <= DDR3_800)
		arrMRS[2] |= 0;
	else if(mem_freq*2 <= DDR3_1066)
		arrMRS[2] |= 1<<3;
	else if(mem_freq*2 <= DDR3_1333)
		arrMRS[2] |= 2<<3;
	else if(mem_freq*2 <= DDR3_1600)
		arrMRS[2] |= 3<<3;	

	//Bruce, 101029, modify according to soc guide 
	//arrMRS[2] |= Hw10; //Rtt_WR is RZQ/2

	/* MR3 */
	arrMRS[3] = Hw17|Hw16;//MR3

	// Set Mode Registers
	CKC_CHANGE_ARG(MR0_DDR3) = arrMRS[0];
	CKC_CHANGE_ARG(MR1_DDR3) = arrMRS[1];
	CKC_CHANGE_ARG(MR2_DDR3) = arrMRS[2];
	CKC_CHANGE_ARG(MR3_DDR3) = arrMRS[3];

	CKC_CHANGE_ARG(CLK_RATE) = mem_freq;

//	printk("T_REFI:0x%x, T_RFC:0x%x, T_RRD:0x%x, T_RP:0x%x, T_RCD:0x%x\n",CKC_CHANGE_ARG(T_REFI),CKC_CHANGE_ARG(T_RFC),CKC_CHANGE_ARG(T_RRD),CKC_CHANGE_ARG(T_RP),CKC_CHANGE_ARG(T_RCD));
//	printk("T_RC:0x%x, T_RAS:0x%x, T_WTR:0x%x, T_WR:0x%x, T_RTP:0x%x\n",CKC_CHANGE_ARG(T_RC),CKC_CHANGE_ARG(T_RAS),CKC_CHANGE_ARG(T_WTR),CKC_CHANGE_ARG(T_WR),CKC_CHANGE_ARG(T_RTP));
//	printk("CL:0x%x, WL:0x%x, RL:0x%x\n",CKC_CHANGE_ARG(CL),CKC_CHANGE_ARG(WL),CKC_CHANGE_ARG(RL));
//	printk("T_FAW:0x%x, T_XSR:0x%x, T_XP:0x%x, T_CKE:0x%x, T_MRD:0x%x\n",CKC_CHANGE_ARG(T_FAW),CKC_CHANGE_ARG(T_XSR),CKC_CHANGE_ARG(T_XP),CKC_CHANGE_ARG(T_CKE),CKC_CHANGE_ARG(T_MRD));
//	printk("MR0:0x%x, MR1:0x%x, MR2:0x%x, MR3:0x%x\n", CKC_CHANGE_ARG(MR0_DDR3), CKC_CHANGE_ARG(MR1_DDR3), CKC_CHANGE_ARG(MR2_DDR3), CKC_CHANGE_ARG(MR3_DDR3));
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void change_clock(unsigned int type)
{
	volatile int i;

//--------------------------------------------------------------------------
// Change to config mode

	*(volatile unsigned long *)addr(0x303020 ) = 0x0003010b ;//EMCCFG

	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
	else//DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED

	if(DDR3_LOGICAL_CHIP_NUM == 2){
		if(DDR3_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0xFFFFFFFF)!= 0x33333333); //Wait PAUSED
		else//DDR3_BANK_NUM is 4
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0x0000FFFF)!= 0x00003333); //Wait PAUSED
	}

//--------------------------------------------------------------------------
// Clock setting..

	if(type == 0)
	{
		//mem bus - DirectPLL2/3(It is for asynchronous clock mode)
		*(volatile unsigned long *)addr(0x400008)  = 0x00200014;

#if defined(GPU_400MHz_USED) // GPU_400MHz
		if (*(volatile unsigned long*)addr(0x40000C) & 0x0020008)
			*(volatile unsigned long *)addr(0x40000C)  = 0x00200014;
#endif

		//set pll1 value
		*(volatile unsigned long *)addr(0x400050)  = CKC_CHANGE_ARG(PLL_VALUE); // PLL_PWR_OFF & SET PMS
		*(volatile unsigned long *)addr(0x400050) |= 0x80000000;			// PLL_PWR_ON
		i = 20; while(i--);

		//*(volatile unsigned long *)addr(0x400008)  = 0x00200013;
		*(volatile unsigned long *)addr(0x400008) = CKC_CHANGE_ARG(CKC_CTRL_VALUE);

#if defined(GPU_400MHz_USED) // GPU_400MHz
		if (*(volatile unsigned long*)addr(0x40000C) & 0x0020004)
			*(volatile unsigned long *)addr(0x40000C) = 0x00200018;
#endif
	}
	else //---------------------------------------------------------------------------
	{
		*(volatile unsigned long *)addr(0x400008)  = 0x00200014;	// mem
		*(volatile unsigned long *)addr(0x40001C)  = 0x00200014;	// smu

		if (CKC_CHANGE_ARG(ONOFF)) {
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << (CKC_CHANGE_ARG(PMU_IDX)+1));
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << (CKC_CHANGE_ARG(PMU_IDX)+2));
			*(volatile unsigned long *)addr(0x404018) |= (1 << CKC_CHANGE_ARG(PMU_IDX));
			*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (1<<CKC_CHANGE_ARG(RESET));
			if (CKC_CHANGE_ARG(RESET) == 5)
				*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) | (1<<(CKC_CHANGE_ARG(RESET)+1));
			*(volatile unsigned long *)addr(0x400000 + (4*CKC_CHANGE_ARG(SOURCE))) |= (1<<21);
	//		if (reset == 5)
	//			*(volatile unsigned long *)addr(0x400000 + (4*(src+1))) |= (1<<21);
		}
		else {
	//		if (reset == 5)
	//			*(volatile unsigned long *)addr(0x400000 + (4*(src+1))) &= ~(1<<21);
			*(volatile unsigned long *)addr(0x400000 + (4*CKC_CHANGE_ARG(SOURCE))) &= ~(1<<21);
			if (CKC_CHANGE_ARG(RESET) == 5)
				*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(1<<(CKC_CHANGE_ARG(RESET)+1));
			*(volatile unsigned long *)addr(0x400044) = ~(*(volatile unsigned long *)addr(0x400044)) & ~(1<<CKC_CHANGE_ARG(RESET));
			*(volatile unsigned long *)addr(0x404018) &= ~(1 << CKC_CHANGE_ARG(PMU_IDX));
			*(volatile unsigned long *)addr(0x404018) |= (1 << (CKC_CHANGE_ARG(PMU_IDX)+1));
			*(volatile unsigned long *)addr(0x404018) |= (1 << (CKC_CHANGE_ARG(PMU_IDX)+2));
		}

		*(volatile unsigned long *)addr(0x400008)  = CKC_CHANGE_ARG(CKC_CTRL_VALUE);	// mem
		*(volatile unsigned long *)addr(0x40001C)  = 0x00200039;	// smu
	}

//--------------------------------------------------------------------------
// Controller setting

	*(volatile unsigned long *)addr(0x303024 ) = 0x00000300 ;//PHYCFG
	*(volatile unsigned long *)addr(0x304400 ) = 0x0000000A ;//PHYMODE

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309400 ) = 0x00101708  ;//PhyControl0
	*(volatile unsigned long *)addr(0x309400 ) = 0x0110140A  ;//PhyControl0

	*(volatile unsigned long *)addr(0x309404 ) = 0x00000086  ;//PhyControl1
	*(volatile unsigned long *)addr(0x309408 ) = 0x00000000  ;//PhyControl2
	*(volatile unsigned long *)addr(0x30940c ) = 0x00000000  ;//PhyControl3
	*(volatile unsigned long *)addr(0x309410 ) = 0x201c7004  ;//PhyControl4

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309400 ) = 0x0110170B  ;//PhyControl0
	//while (((*(volatile unsigned long *)addr(0x309418)) & (0x04)) != 4);	// dll locked
	if(CKC_CHANGE_ARG(CLK_RATE) >= 333)
		*(volatile unsigned long *)addr(0x309400 ) = 0x0110140B  ;//PhyControl0
	else
		*(volatile unsigned long *)addr(0x309400 ) = 0x0110140F  ;//PhyControl0
	while (((*(volatile unsigned long *)addr(0x309418)) & (0x04)) != 4);	// dll locked

	*(volatile unsigned long *)addr(0x309404 ) = 0x0000008e  ;//PhyControl1
	*(volatile unsigned long *)addr(0x309404 ) = 0x00000086  ;//PhyControl1

	//Bruce, 101029, modify according to soc guide
	//*(volatile unsigned long *)addr(0x309414 ) = 0x00030003  ;//PhyControl5
	*(volatile unsigned long *)addr(0x309414 ) = 0x00020003  ;//PhyControl5

	*(volatile unsigned long *)addr(0x309414 ) = 0x0000000b | (MEMCTRL_DDS<<15) | (MEMCTRL_TERM<<11);//PhyControl5

	while (((*(volatile unsigned long *)addr(0x309420)) & (0x01)) != 1);	// zq end

//--------------------------------------------------------------------------
// Memory config

	*(volatile unsigned long *)addr(0x309004 ) = 0x0000018A ; //MemControl

	if(DDR3_BURST_LEN == BL_8) // BL
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x7<<7, 0x3<<7);
	else
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x7<<7, 0x2<<7);

	if(DDR3_LOGICAL_CHIP_NUM == 1) // num_chip
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x3<<5, 0x0);
	else
		BITCSET(*(volatile unsigned long *)addr(0x309004), 0x3<<5, 0x1<<5);

    // Chip 0 Configuration ------------------------------------------------
    {
		*(volatile unsigned long *)addr(0x309008) = 0x40F01313; //MemConfig0 //address mapping method - interleaved
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xFF<<16, (0xFF - ((DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10)-1))<<16);//set chip mask
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF<<8, (DDR3_COLBITS - 7)<<8);//set column bits
		BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF<<4, (DDR3_ROWBITS - 12)<<4);//set row bits
		if(DDR3_BANK_NUM == 8)//8 banks
			BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF, 0x3);
		else // 4 banks
			BITCSET(*(volatile unsigned long *)addr(0x309008), 0xF, 0x2);
    }

    // Chip 1 Configuration ------------------------------------------------
	if(DDR3_LOGICAL_CHIP_NUM == 2)
	{
		*(volatile unsigned long *)addr(0x30900C) = 0x50E01313; //MemConfig1 //address mapping method - interleaved
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xFF<<24, (0x40 + (DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10))<<24);//set chip base
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xFF<<16, (0xFF - ((DDR3_TOTAL_MB_SIZE)/(DDR3_LOGICAL_CHIP_NUM*0x10)-1))<<16);//set chip mask
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF<<8, (DDR3_COLBITS - 7)<<8);//set column bits
		BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF<<4, (DDR3_ROWBITS - 12)<<4);//set row bits
		if(DDR3_BANK_NUM == 8)// 8 banks
			BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF, 0x3);
		else // 4 banks
			BITCSET(*(volatile unsigned long *)addr(0x30900C), 0xF, 0x2);
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x309000) = 0x40FF3010  ;//ConControl
	*(volatile unsigned long *)addr(0x309014) = 0x01000000  ;//PrechConfig

//--------------------------------------------------------------------------
// Timing parameter setting.

	*(volatile unsigned long *)addr(0x309100) = CKC_CHANGE_ARG(T_REFI);
	*(volatile unsigned long *)addr(0x309104) = CKC_CHANGE_ARG(T_RFC);
	*(volatile unsigned long *)addr(0x309108) = CKC_CHANGE_ARG(T_RRD);
	*(volatile unsigned long *)addr(0x30910c) = CKC_CHANGE_ARG(T_RP);
	*(volatile unsigned long *)addr(0x309110) = CKC_CHANGE_ARG(T_RCD);
	*(volatile unsigned long *)addr(0x309114) = CKC_CHANGE_ARG(T_RC);
	*(volatile unsigned long *)addr(0x309118) = CKC_CHANGE_ARG(T_RAS);
	*(volatile unsigned long *)addr(0x30911c) = CKC_CHANGE_ARG(T_WTR);
	*(volatile unsigned long *)addr(0x309120) = CKC_CHANGE_ARG(T_WR);
	*(volatile unsigned long *)addr(0x309124) = CKC_CHANGE_ARG(T_RTP);
	*(volatile unsigned long *)addr(0x309128) = CKC_CHANGE_ARG(CL);
	*(volatile unsigned long *)addr(0x30912c) = CKC_CHANGE_ARG(WL);
	*(volatile unsigned long *)addr(0x309130) = CKC_CHANGE_ARG(RL);
	*(volatile unsigned long *)addr(0x309134) = CKC_CHANGE_ARG(T_FAW);
	*(volatile unsigned long *)addr(0x309138) = CKC_CHANGE_ARG(T_XSR);
	*(volatile unsigned long *)addr(0x30913c) = CKC_CHANGE_ARG(T_XP);
	*(volatile unsigned long *)addr(0x309140) = CKC_CHANGE_ARG(T_CKE);
	*(volatile unsigned long *)addr(0x309144) = CKC_CHANGE_ARG(T_MRD);

//--------------------------------------------------------------------------
// MRS Setting

	*(volatile unsigned long *)addr(0x309010) = 0x08000000;//DirectCmd - XSR

	if(DDR3_LOGICAL_CHIP_NUM == 2)
		*(volatile unsigned long *)addr(0x309010) = 0x08000000 | Hw20;//DirectCmd - XSR

	i = 50; while(i--);

#ifndef MRS_ALWAYS_SETTING
	if(CKC_CHANGE_ARG(MRS_SET))
#endif
	{
		*(volatile unsigned long *)addr(0x309010) = 0x07000000 ;//DirectCmd - NOP

		*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR2_DDR3);//DirectCmd - MRS : MR2
		*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR3_DDR3);//DirectCmd - MRS : MR3
		*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR1_DDR3);//DirectCmd - MRS : MR1 : DLL(enable)

		//Bruce, 101102, for DLL Reset.
		//*(volatile unsigned long *)addr(0x309010 ) = CKC_CHANGE_ARG(MR0_DDR3) ;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(5), DLL Reset(Yes), MODE(0), CL(6), BL(8)
		{
			*(volatile unsigned long *)addr(0x309010) = (CKC_CHANGE_ARG(MR0_DDR3))|0x100;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(), DLL Reset(Yes), MODE(0), CL(), BL(8)
			*(volatile unsigned long *)addr(0x309010) = 0x01000000;//precharge all
			*(volatile unsigned long *)addr(0x309010) = 0x05000000;//AREF
			*(volatile unsigned long *)addr(0x309010) = 0x05000000;//AREF
			*(volatile unsigned long *)addr(0x309010) = 0x05000000;//AREF
			*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR0_DDR3);	// DLL reset release.
			i = 50;	while(i--);
		}
		
		*(volatile unsigned long *)addr(0x309010) = 0x0a000400 ;//DirectCmd - ZQCL

		if(DDR3_LOGICAL_CHIP_NUM == 2){
			*(volatile unsigned long *)addr(0x309010) = 0x07000000 | Hw20;//DirectCmd - NOP

			*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR2_DDR3) | Hw20;//DirectCmd - MRS : MR2
			*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR3_DDR3) | Hw20;//DirectCmd - MRS : MR3
			*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR1_DDR3) | Hw20;//DirectCmd - MRS : MR1 : DLL(enable)

			//Bruce, 101102, for DLL Reset.
			//*(volatile unsigned long *)addr(0x309010 ) = CKC_CHANGE_ARG(MR0_DDR3) | Hw20;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(5), DLL Reset(Yes), MODE(0), CL(6), BL(8)
			{
				*(volatile unsigned long *)addr(0x309010) = (CKC_CHANGE_ARG(MR0_DDR3))|0x100 | Hw20;//DirectCmd - MRS : MR0 : DLLPRE(off), WR(), DLL Reset(Yes), MODE(0), CL(), BL(8)
				*(volatile unsigned long *)addr(0x309010) = 0x01000000 | Hw20;//precharge all
				*(volatile unsigned long *)addr(0x309010) = 0x05000000 | Hw20;//AREF
				*(volatile unsigned long *)addr(0x309010) = 0x05000000 | Hw20;//AREF
				*(volatile unsigned long *)addr(0x309010) = 0x05000000 | Hw20;//AREF
				*(volatile unsigned long *)addr(0x309010) = CKC_CHANGE_ARG(MR0_DDR3) | Hw20;	// DLL reset release.
				i = 50; while(i--);
			}
			
			*(volatile unsigned long *)addr(0x309010) = 0x0a000400 | Hw20;//DirectCmd - ZQCL
		}
	}

//--------------------------------------------------------------------------

	*(volatile unsigned long *)addr(0x309000) = 0x60FF3030;//ConControl
	*(volatile unsigned long *)addr(0x303020) = 0x0007010b;//EMCCFG

	if(DDR3_BANK_NUM == 8)
		while (((*(volatile unsigned long *)addr(0x309208)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
	else //DDR3_BANK_NUM is 4
		while (((*(volatile unsigned long *)addr(0x309208)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED

	if(DDR3_LOGICAL_CHIP_NUM == 2){
		if(DDR3_BANK_NUM == 8)
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0xFFFFFFFF)!= 0x44444444); //Wait PAUSED
		else //DDR3_BANK_NUM is 4
			while (((*(volatile unsigned long *)addr(0x30920C)) & 0x0000FFFF)!= 0x00004444); //Wait PAUSED
	}
}

#else
	#error Not Selected ddr type
#endif



/*===========================================================================

                            Common Functions

===========================================================================*/

/*===========================================================================
FUNCTION
===========================================================================*/
static unsigned int get_cycle(unsigned int tCK, unsigned int ps, unsigned int ck)
{
	unsigned int ret;

	ret = time2cycle(ps, tCK);

	if(ret > ck)
		return ret;
	else
		return ck;
}



/*===========================================================================
FUNCTION
 - return value : MHz unit
===========================================================================*/
static unsigned int get_membus_ckc(unsigned int mem_freq)
{
	int i = 0;

	for (i=0 ; i<ARRAY_SIZE(pIO_CKC_PLL_H) ; i++) {
		if (pIO_CKC_PLL_H[i].freq >= mem_freq)
			break;
	}

	if (i >= ARRAY_SIZE(pIO_CKC_PLL_H))
		i = (ARRAY_SIZE(pIO_CKC_PLL_H) - 1);;

	CKC_CHANGE_ARG(PLL_VALUE) = pIO_CKC_PLL_H[i].pll;
	CKC_CHANGE_ARG(CKC_CTRL_VALUE) = pIO_CKC_PLL_H[i].clkctrl;

	return (pIO_CKC_PLL_H[i].freq/10000);
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void copy_change_clock(unsigned int type)
{
	unsigned long  flags;
	unsigned int   stack;
	unsigned int   lcdc0_on = 0, lcdc1_on = 0;
	FuncPtr pFunc = (FuncPtr)(CKC_CHANGE_FUNC_ADDR);

#if defined(CONFIG_GENERIC_TIME)
	volatile PPIC	pPIC	= (volatile PPIC)tcc_p2v(HwPIC_BASE);
#else
	volatile PTIMER	pTIMER	= (volatile PTIMER)tcc_p2v(HwTMR_BASE);
#endif

	lcdc0_on = DEV_LCDC_Status(0);
	lcdc1_on = DEV_LCDC_Status(1);

	if((lcdc0_on & HwLSTATUS_DEOF) || (lcdc1_on & HwLSTATUS_DEOF))	{
		mdelay(1);
	}

	lcdc0_on = DEV_LCDC_Wait_signal(0);
	lcdc1_on = DEV_LCDC_Wait_signal(1);

	memcpy((void *)CKC_CHANGE_FUNC_ADDR, (void*)change_clock, CKC_CHANGE_FUNC_SIZE);

	local_irq_save(flags);
	local_irq_disable();
	local_flush_tlb_all();
//	flush_cache_all();	// it spend 1~2milisec. time
#if defined(CONFIG_GENERIC_TIME)
	pPIC->IEN0 &= ~Hw1;		/* Disable Timer0 interrupt */
#else
	pTIMER->TC32EN &= ~Hw24;
#endif

	stack = IO_ARM_ChangeStackSRAM();

	pFunc(type);

	IO_ARM_RestoreStackSRAM(stack);

#if !defined(CONFIG_GENERIC_TIME)
	pTIMER->TC32EN |= Hw24;
#endif
	local_irq_restore(flags);
#if defined(CONFIG_GENERIC_TIME)
	pPIC->IEN0 |= Hw1;		/* Enable Timer0 interrupt */
#endif
}

/*===========================================================================
VARIABLE
===========================================================================*/
unsigned int mem_clk_initialized = 0;

/*===========================================================================
FUNCTION
===========================================================================*/
void tcc_ddr_set_clock(unsigned int freq)
{
	unsigned int   mem_freq;

	mem_freq = get_membus_ckc(freq*10);
	get_ddr_param(mem_freq);

#if defined(CONFIG_MACH_TCC8800ST)
	// Do not set ddr memory clock
#else
	copy_change_clock(0);
#endif /* CONFIG_MACH_TCC8800ST */


	mem_clk_initialized = 1;
}

/*===========================================================================
FUNCTION
===========================================================================*/
int tcc_change_pmu(unsigned int src, unsigned int onoff)
{
	unsigned int   pmu, reset;
//	int            ret;

//	printk("%s: src:%d, onoff:%d ==> ", __func__, src, onoff);

	return -1;	// 20110627, Do not into the self-refresh mode when device control the PMU PWROFF register.

#if defined(CONFIG_MACH_TCC8800ST)
	goto err_chgpmu_chiprev;
#endif /* CONFIG_MACH_TCC8800ST */

	if (mem_clk_initialized == 0)
	{
		printk("tcc_change_pmu() : Warning : BUSes of PMU is controlled, before memory clock is initialized!!");
		tcc_ddr_set_clock(DDR_MIN_CLK*1000);
	}
//	goto err_chgpmu_beforeinit;

	switch (src) {
		case FBUS_DDI: // Display Bus
			pmu = 5;
			reset = 1;
			break;
		case FBUS_GPU: // Graphic Bus
			pmu = 14;
			reset = 3;
			break;
		case FBUS_VBUS: // Video Bus
			pmu = 11;
			reset = 5;
			break;
//		case FBUS_VCORE: // Video Codec
//			break;
		case FBUS_HSIO: // High Speed IO
			pmu = 20;
			reset = 8;
			break;
		case FBUS_CAM: // Camera
			pmu = 17;
			reset = 9;
			break;
		default:
			goto err_chgpmu_nosrc;
	}

	CKC_CHANGE_ARG(RESET) = reset;
	CKC_CHANGE_ARG(PMU_IDX) = pmu;
	CKC_CHANGE_ARG(SOURCE) = src;
	CKC_CHANGE_ARG(ONOFF) = onoff;

	copy_change_clock(1);

//	printk("OK\n");
	return 0;

err_chgpmu_chiprev:
//	printk(".3.");
//err_chgpmu_beforeinit:
//	printk(".2.");
err_chgpmu_nosrc:
//	printk(".1.");

//	printk("Failed!!\n");

	return -1;
}
/*=========================================================================*/

EXPORT_SYMBOL(tcc_ddr_set_clock);
EXPORT_SYMBOL(tcc_change_pmu);


