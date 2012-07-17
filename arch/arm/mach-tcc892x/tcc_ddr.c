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
#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_CACHE_L2X0)
#include <asm/hardware/cache-l2x0.h>
#endif

#include <asm/mach-types.h>


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

extern unsigned int IO_ARM_ChangeStackSRAM(void);
extern void IO_ARM_RestoreStackSRAM(unsigned int);
static unsigned int get_cycle(unsigned int tCK, unsigned int ps, unsigned int ck);

#define time2cycle(time, tCK)		((int)((time + tCK - 1) / tCK))

#if defined(CONFIG_DRAM_DDR3)
#define addr_clk(b) (0x74000000+b)
#define addr_mem(b) (0x73000000+b)
#else
#define addr_clk(b) (0xF5000000+b)
#define addr_mem(b) (0xF4000000+b)
#endif

#define denali_ctl(x) (*(volatile unsigned long *)addr_mem(0x500000+(x*4)))
#define denali_phy(x) (*(volatile unsigned long *)addr_mem(0x600000+(x)))
#define ddr_phy(x) (*(volatile unsigned long *)addr_mem(0x420400+(x)))

typedef void (*FuncPtr)(void);

#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_CACHE_L2X0)
#define L2CACHE_BASE   0xFA000000
#endif


/*===========================================================================

                                 MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) TCC93xx
     << sram1 >>
     0xF0000000(0xF0000000) ------------------
                           | ckc change func  | 0x700
                     0x700  ------------------
                           | ckc change arg   | 0x80
                     0x780  ------------------
                           |      Stack       | 0x80
                     0x800  ------------------

---------------------------------------------------------------------------*/

#define CKC_CHANGE_FUNC_ADDR       0xF0000000

#define CKC_CHANGE_FUNC_SIZE       0x700

#define CKC_CHANGE_ARG_BASE        0xF0000700



/*===========================================================================

                          Clock Register Value

===========================================================================*/

typedef struct {
	unsigned int	pll;
	unsigned int	clkctrl;   
	unsigned int	freq;
} MEMCLK;

MEMCLK mem_clk_table[]	=
{
#if defined(CONFIG_DRAM_DDR3)
//	{0x00014D03, 0x00200018, 6660000},
	{0x00012C03, 0x00200018, 6000000},
	{0x00011303, 0x00200018, 5500000},
	{0x00010B03, 0x00200018, 5340000},
	{0x00010903, 0x00200018, 5300000},
	{0x00010503, 0x00200018, 5220000},
	{0x0000FF03, 0x00200018, 5100000},
	{0x0000FB03, 0x00200018, 5020000},
	{0x4101F203, 0x00200018, 4980000},
	{0x4101C203, 0x00200018, 4500000},
	{0x41019003, 0x00200018, 4000000},
	{0x41018603, 0x00200018, 3900000},
	{0x41017C03, 0x00200018, 3800000},
	{0x41017203, 0x00200018, 3700000},
#endif
	{0x41016803, 0x00200018, 3600000},
	{0x01015E03, 0x00200018, 3500000},
	{0x01015403, 0x00200018, 3400000},
	{0x01014A03, 0x00200018, 3300000},
	{0x01014003, 0x00200018, 3200000},
	{0x01013603, 0x00200018, 3100000},
	{0x01012C03, 0x00200018, 3000000},
#if defined(CONFIG_DRAM_DDR2)
	{0x01012203, 0x00200018, 2900000},
	{0x01011803, 0x00200018, 2800000},
	{0x01010E03, 0x00200018, 2700000},
	{0x01010403, 0x00200018, 2600000},
	{0x4201F403, 0x00200018, 2500000},
	{0x4201E003, 0x00200018, 2400000},
	{0x4201CC03, 0x00200018, 2300000},
	{0x4201B803, 0x00200018, 2200000},
	{0x4201A403, 0x00200018, 2100000},
	{0x42019003, 0x00200018, 2000000},
	{0x42017C03, 0x00200018, 1900000},
	{0x42016803, 0x00200018, 1800000},
	{0x02015403, 0x00200018, 1700000},
	{0x02014003, 0x00200018, 1600000},
	{0x02013003, 0x00200018, 1520000},
	{0x02012203, 0x00200018, 1450000},
	{0x02011A03, 0x00200018, 1410000},
	{0x02011403, 0x00200018, 1380000},
	{0x02010403, 0x00200018, 1300000},
	{0x0200FC03, 0x00200018, 1260000},
#endif
};


/*===========================================================================

                          Clock Change Argument

===========================================================================*/

#define CKC_CHANGE_ARG(x)   (*(volatile unsigned long *)(CKC_CHANGE_ARG_BASE + (4*(x))))

#if defined(CONFIG_DRAM_DDR3)
enum {
	PLL_VALUE = 0,
	CKC_CTRL_VALUE,
	CLK_RATE,
	DENALI_CTL_3,
	DENALI_CTL_4,
	DENALI_CTL_5,
	DENALI_CTL_6,
	DENALI_CTL_7,
	DENALI_CTL_8,
	DENALI_CTL_9,
	DENALI_CTL_10,
	DENALI_CTL_11,
	DENALI_CTL_12,
	DENALI_CTL_13,
	tRFC,
	DENALI_CTL_15,
	DENALI_CTL_16,
	DENALI_CTL_17,
	DENALI_CTL_18,
	tCKSRX,
	tCKSRE,
	DENALI_CTL_101,
	MR0,
	MR1,
	MR2,
	MR3
};
#elif defined(CONFIG_DRAM_DDR2)
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


/*===========================================================================
FUNCTION
 - mem_freq : MHz unit
===========================================================================*/
static void get_ddr_param(unsigned int mem_freq)
{
#if defined(CONFIG_DRAM_DDR2)
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

//--------------------------------------------------------------------------
#elif defined(CONFIG_DRAM_DDR3)

	unsigned tck = 0;
	unsigned nCL, nCWL;
	unsigned tmp;

	tck = (1000000/mem_freq);

	if(tck >= 2500){ /* 2.5ns, 400MHz */
		nCL = 6;
		nCWL = 5;
	}else if(tck >= 1875){ // 1.875ns, 533..MHz
		nCL = 8;
		nCWL = 6;
	}else if(tck >= 1500){ // 1.5 ns, 666..MHz
		if(DDR3_MAX_SPEED < DDR3_1600)
			nCL = 9;
		else
			nCL = 10;
		nCWL = 7;
	}else if(tck >= 1250){ // 1.25 ns, 800MHz
		nCL = 11;
		nCWL = 8;
	}else if(tck >= 1070){ // 1.07 ns, 933..MHz
		nCL = 13;
		nCWL = 9;
	}else if(tck >= 935){ // 0.935 ns, 1066..MHz
		nCL = 14;
		nCWL = 10;
	}

	CKC_CHANGE_ARG(DENALI_CTL_3) = get_cycle(tck, 20000000, 1); //TRST_PWRON = 200us, 7 //Bruce_temp.. ns ?
	CKC_CHANGE_ARG(DENALI_CTL_4) = get_cycle(tck, 50000000, 1); //CKE_INACTIVE = 500us, 10 //Bruce_temp.. ns ?

	//TBST_INT_INTERVAL[26:24] = 0x1, WL[20:16], CASLAT_LIN[13:8], CL half-cycle increment = 0, INITAREF[3:0] = 0x8
	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		CKC_CHANGE_ARG(DENALI_CTL_5) = (0x1<<24 | (nCWL)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		CKC_CHANGE_ARG(DENALI_CTL_5) = (0x1<<24 | (nCWL+nCL-1)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		CKC_CHANGE_ARG(DENALI_CTL_5) = (0x1<<24 | (nCWL+nCL-2)<<16 | ((nCL<<1)|0)<<8 | 0x8);
	}

	CKC_CHANGE_ARG(DENALI_CTL_6) = (get_cycle(tck, DDR3_tRAS_ps, DDR3_tRAS_ck)<<24 | get_cycle(tck, DDR3_tRC_ps, DDR3_tRC_ck)<<16 | get_cycle(tck, DDR3_tRRD_ps, DDR3_tRRD_ck)<<8 | DDR3_tCCD_ck);
	CKC_CHANGE_ARG(DENALI_CTL_7) = (DDR3_tMRD_ck<<24 | get_cycle(tck, DDR3_tRTP_ps, DDR3_tRTP_ck)<<16 | get_cycle(tck, DDR3_tRP_ps, DDR3_tRP_ck)<<8 | (get_cycle(tck, DDR3_tWTR_ps, DDR3_tWTR_ck)+1));
	CKC_CHANGE_ARG(DENALI_CTL_8) = (get_cycle(tck, DDR3_tRAS_MAX_ps, 1)<<8 | get_cycle(tck, DDR3_tMOD_ps, DDR3_tMOD_ck));
	CKC_CHANGE_ARG(DENALI_CTL_9) = ((get_cycle(tck, DDR3_tCKE_ps, DDR3_tCKE_ck)+1)<<8 | get_cycle(tck, DDR3_tCKE_ps, DDR3_tCKE_ck));
	CKC_CHANGE_ARG(DENALI_CTL_10) = (get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck)<<24 | get_cycle(tck, DDR3_tRCD_ps, DDR3_tRCD_ck)<<16 | 1<<8 | 1);
	CKC_CHANGE_ARG(DENALI_CTL_11) = (1<<24 | DDR3_tDLLK_ck<<8 | (get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck)+nCL));
	CKC_CHANGE_ARG(DENALI_CTL_12) = (1<<16 | get_cycle(tck, DDR3_tFAW_ps, DDR3_tFAW_ck)<<8 | 3);
	CKC_CHANGE_ARG(DENALI_CTL_13) = get_cycle(tck, DDR3_tRP_ps, DDR3_tRP_ck)+1;
	CKC_CHANGE_ARG(tRFC) = get_cycle(tck, DDR3_tRFC_ps, 1);
	CKC_CHANGE_ARG(DENALI_CTL_15) = get_cycle(tck, DDR3_tREFI_ps, 1);
	CKC_CHANGE_ARG(DENALI_CTL_16) = (get_cycle(tck, DDR3_tXPDLL_ps, DDR3_tXPDLL_ck)<<16 | get_cycle(tck, DDR3_tXP_ps, DDR3_tXP_ck)); // DDR3 Only
	CKC_CHANGE_ARG(DENALI_CTL_17) = (6<<16 | 2); //TXARD[-0] = 0x2, TXARDS[-16] = 0x6 // DDR2 only
	CKC_CHANGE_ARG(DENALI_CTL_18) = (get_cycle(tck, DDR3_tXS_ps, DDR3_tXS_ck)<<16 | DDR3_tXSDLL_ck);
	CKC_CHANGE_ARG(tCKSRX) = get_cycle(tck, DDR3_tCKSRX_ps, DDR3_tCKSRX_ck);
	CKC_CHANGE_ARG(tCKSRE) = get_cycle(tck, DDR3_tCKSRE_ps, DDR3_tCKSRE_ck);

//--------------------------------------------------------------------------
// DFI Timing

	if(DDR3_AL == AL_DISABLED){ //nAL = 0;
		CKC_CHANGE_ARG(DENALI_CTL_101) = (0x1<<24|0x1<<16|nCWL<<8|(nCL+5));
	}
	else if(DDR3_AL == AL_CL_MINUS_ONE){ //nAL = nCL - 1;
		CKC_CHANGE_ARG(DENALI_CTL_101) = (0x1<<24|0x1<<16|(nCWL+nCL-1)<<8|(nCL+5));
	}	
	else if(DDR3_AL == AL_CL_MINUS_TWO){ //nAL = nCL - 2;
		CKC_CHANGE_ARG(DENALI_CTL_101) = (0x1<<24|0x1<<16|(nCWL+nCL-2)<<8|(nCL+5));
	}

//--------------------------------------------------------------------------
// MRS Setting

	// MRS0
#if (1) //Bruce_temp_mrs0 FAST_EXIT ¸Â³ª?
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

	if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) <= 5) // Write Recovery for autoprecharge
		tmp |= WR_5<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 6)
		tmp |= WR_6<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 7)
		tmp |= WR_7<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 8)
		tmp |= WR_8<<9;
	else if((get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 9) || (get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) == 10))
		tmp |= WR_10<<9;
	else if(get_cycle(tck, DDR3_tWR_ps, DDR3_tWR_ck) >= 11)
		tmp |= WR_12<<9;

	tmp	|= (1<<8); // DLL Reset

	CKC_CHANGE_ARG(MR0) = tmp;

	// MRS1
	CKC_CHANGE_ARG(MR1) = ((DDR3_AL<<3) | (DDR3_ODT<<2) | (DDR3_DIC<<1));

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

	CKC_CHANGE_ARG(MR2) = tmp;

//	printk("mem_freq=%dMhz, nCL=%d, nCWL=%d\n", mem_freq, nCL, nCWL);
//	printk("T_REFI:0x%x, T_RFC:0x%x, T_RRD:0x%x, T_RP:0x%x, T_RCD:0x%x\n",CKC_CHANGE_ARG(T_REFI),CKC_CHANGE_ARG(T_RFC),CKC_CHANGE_ARG(T_RRD),CKC_CHANGE_ARG(T_RP),CKC_CHANGE_ARG(T_RCD));
//	printk("T_RC:0x%x, T_RAS:0x%x, T_WTR:0x%x, T_WR:0x%x, T_RTP:0x%x\n",CKC_CHANGE_ARG(T_RC),CKC_CHANGE_ARG(T_RAS),CKC_CHANGE_ARG(T_WTR),CKC_CHANGE_ARG(T_WR),CKC_CHANGE_ARG(T_RTP));
//	printk("CL:0x%x, WL:0x%x, RL:0x%x\n",CKC_CHANGE_ARG(CL),CKC_CHANGE_ARG(WL),CKC_CHANGE_ARG(RL));
//	printk("T_FAW:0x%x, T_XSR:0x%x, T_XP:0x%x, T_CKE:0x%x, T_MRD:0x%x\n",CKC_CHANGE_ARG(T_FAW),CKC_CHANGE_ARG(T_XSR),CKC_CHANGE_ARG(T_XP),CKC_CHANGE_ARG(T_CKE),CKC_CHANGE_ARG(T_MRD));
//	printk("MR0:0x%x, MR1:0x%x, MR2:0x%x, MR3:0x%x\n", CKC_CHANGE_ARG(MR0_DDR3), CKC_CHANGE_ARG(MR1_DDR3), CKC_CHANGE_ARG(MR2_DDR3), CKC_CHANGE_ARG(MR3_DDR3));

//--------------------------------------------------------------------------
#endif
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void change_clock(void)
{
	volatile int i;

#if defined(CONFIG_DRAM_DDR2)

//--------------------------------------------------------------------------
//Change to config mode

	//Miscellaneous Configuration : COMMON
	*(volatile unsigned long *)addr_mem(0x410020) &= ~Hw17; //creq2=0 : enter low power
	while((*(volatile unsigned long *)addr_mem(0x410020))&Hw24); //cack2==0 : wait for acknowledgement to request..

	//MEMCTRL
	*(volatile unsigned long *)addr_mem(0x430004) |= 0x00000001; //clk_stop_en=1 //Bruce_temp_8920

#if (0)
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
	//*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //dclkr=0 : this is used for DDR3 only. //Bruce_temp_8920 ??

#if (1)
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=20; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=20; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFFF0) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x0000000F);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x80000000);	// CHGREQ
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFF0F) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x000000F0);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x20000000);	// CFGREQ
#else
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=20; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=100; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = CKC_CHANGE_ARG(CKC_CTRL_VALUE);  // mem bus
	i=50; while(i--);
#endif

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
	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ) {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL ;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 ;//zq start
	} else {
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw0;
		*(volatile unsigned long *)addr_mem(0x430044 ) = PHYZQCTRL | Hw1 | Hw0 ;//zq start
	}
	while (((*(volatile unsigned long *)addr_mem(0x430040)) & (0x10000)) != 0x10000);	// Wait until ZQ End

	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ) {
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

	*(volatile unsigned long *)addr_mem(0x430030 ) = CKC_CHANGE_ARG(T_REFI);
	*(volatile unsigned long *)addr_mem(0x430034 ) = CKC_CHANGE_ARG(TROW);
	*(volatile unsigned long *)addr_mem(0x430038 ) = CKC_CHANGE_ARG(TDATA);
	*(volatile unsigned long *)addr_mem(0x43003C ) = CKC_CHANGE_ARG(TPOWER);

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
	*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(CKC_CHANGE_ARG(CL)<<4)|((CKC_CHANGE_ARG(T_WR)-1)<<9));	// BurstLength 4, CL , WR 
	i = 100; while(i--);
	*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380; // OCD Calibration default
	i = 100; while(i--);
	if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ)
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1);
	else
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_DIC<<1);

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
		*(volatile unsigned long *)addr_mem(0x430010 ) = (DDR2_BURST_LEN|(DDR2_READ_BURST_TYPE<<3)|(CKC_CHANGE_ARG(CL)<<4)|((CKC_CHANGE_ARG(T_WR)-1)<<9)) | Hw20;	// BurstLength 4, CL , WR 
		i = 100; while(i--);
		*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010380 | Hw20; // OCD Calibration default
		i = 100; while(i--);
		if (CKC_CHANGE_ARG(CLK_RATE) >= DDR2_ODT_ENABLE_MIN_FREQ)
			*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_ODT<<2) | (DDR2_DIC<<1) | Hw20;
		else
			*(volatile unsigned long *)addr_mem(0x430010 ) = 0x00010000 | (DDR2_DIC<<1) | Hw20;
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

#elif defined(CONFIG_DRAM_DDR3)

	volatile int tmp;

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

//--------------------------------------------------------------------------
// holding to access to dram

	BITSET(denali_ctl(44),0x1); //inhibit_dram_cmd=1
	while(!(denali_ctl(46)&(0x1<<16)));	// wait for inhibit_dram_cmd
	BITSET(denali_ctl(47), 0x1<<16);

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

//--------------------------------------------------------------------------
//Clock setting..

	//Memory BUS Configuration : MBUSCFG
	#ifdef DDR3_AXI0_FIFO_1to2_ENABLE
	*(volatile unsigned long *)addr_mem(0x810010) |= Hw15; //dclkr=1 : this is used for DDR3 only.
	#else
	*(volatile unsigned long *)addr_mem(0x810010) &= ~Hw15; //(d)not used just for ddr3 clk div
	#endif
	i=10; while(i--);

#if (1)
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=15; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=15; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFFF0) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x0000000F);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x80000000);	// CHGREQ
	*(volatile unsigned long *)addr_clk(0x000004) = ((*(volatile unsigned long *)addr_clk(0x000004))&0xFFFFFF0F) | (CKC_CHANGE_ARG(CKC_CTRL_VALUE)&0x000000F0);
	while((*(volatile unsigned long *)addr_clk(0x000004))&0x20000000);	// CFGREQ
#else
	*(volatile unsigned long *)addr_clk(0x000004) = 0x00200014;  // mem bus
	*(volatile unsigned long *)addr_clk(0x000040) = CKC_CHANGE_ARG(PLL_VALUE) | 0x00100000; //pll4 for mem , lock_en
	*(volatile unsigned long *)addr_clk(0x000040) |= 0x80000000;
	i=20; while(i--) while((*(volatile unsigned long *)addr_clk(0x000040) & 0x00200000) == 0);
	i=100; while(i--);
	*(volatile unsigned long *)addr_clk(0x000004) = CKC_CHANGE_ARG(CKC_CTRL_VALUE);  // mem bus
	i=50; while(i--);
#endif

//--------------------------------------------------------------------------
// Timing Parameter

	denali_ctl(3) = CKC_CHANGE_ARG(DENALI_CTL_3);
	denali_ctl(4) = CKC_CHANGE_ARG(DENALI_CTL_4);
	denali_ctl(5) = CKC_CHANGE_ARG(DENALI_CTL_5);
	denali_ctl(6) = CKC_CHANGE_ARG(DENALI_CTL_6);
	denali_ctl(7) = CKC_CHANGE_ARG(DENALI_CTL_7);
	denali_ctl(8) = CKC_CHANGE_ARG(DENALI_CTL_8);
	denali_ctl(9) = CKC_CHANGE_ARG(DENALI_CTL_9);
	denali_ctl(10) = CKC_CHANGE_ARG(DENALI_CTL_10);
	denali_ctl(11) = CKC_CHANGE_ARG(DENALI_CTL_11);
	denali_ctl(12) = CKC_CHANGE_ARG(DENALI_CTL_12);
	denali_ctl(13) = CKC_CHANGE_ARG(DENALI_CTL_13);
	BITCSET(denali_ctl(14), 0x03FF0100, (CKC_CHANGE_ARG(tRFC)<<16 | 1<<8));
	denali_ctl(15) = CKC_CHANGE_ARG(DENALI_CTL_15);
	denali_ctl(16) = CKC_CHANGE_ARG(DENALI_CTL_16);
	denali_ctl(17) = CKC_CHANGE_ARG(DENALI_CTL_17);
	denali_ctl(18) = CKC_CHANGE_ARG(DENALI_CTL_18);
	BITCSET(denali_ctl(20), 0x000F0F00, CKC_CHANGE_ARG(tCKSRX)<<16 | CKC_CHANGE_ARG(tCKSRE)<<8);

//--------------------------------------------------------------------------
// DFI Timing

	denali_ctl(101) = CKC_CHANGE_ARG(DENALI_CTL_101);

//--------------------------------------------------------------------------
// MRS Setting

	// MRS0
	denali_ctl(27) = CKC_CHANGE_ARG(MR0)<<8;
	BITCSET(denali_ctl(30), 0x0000FFFF, CKC_CHANGE_ARG(MR0));

	// MRS1
	BITCSET(denali_ctl(28), 0x0000FFFF, CKC_CHANGE_ARG(MR1));
	BITCSET(denali_ctl(30), 0xFFFF0000, CKC_CHANGE_ARG(MR1)<<16);

	// MRS2
	BITCSET(denali_ctl(28), 0xFFFF0000, CKC_CHANGE_ARG(MR2)<<16);
	BITCSET(denali_ctl(31), 0x0000FFFF, CKC_CHANGE_ARG(MR2));

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

	i = 10;	while(i--) tmp = denali_ctl(46); // for reset DLL on DDR3
	BITCLR(denali_ctl(44),0x1); //inhibit_dram_cmd=0

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

//--------------------------------------------------------------------------
#else
	#error Not Selected ddr type
#endif
}



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
	unsigned i = 0;

	for (i=0 ; i<ARRAY_SIZE(mem_clk_table) ; i++) {
		if (mem_clk_table[i].freq <= mem_freq)
			break;
	}

	if (i >= ARRAY_SIZE(mem_clk_table))
		i = (ARRAY_SIZE(mem_clk_table) - 1);

	CKC_CHANGE_ARG(PLL_VALUE) = mem_clk_table[i].pll;
	CKC_CHANGE_ARG(CKC_CTRL_VALUE) = mem_clk_table[i].clkctrl;
	CKC_CHANGE_ARG(CLK_RATE) = (mem_clk_table[i].freq/10000);

	return (mem_clk_table[i].freq/10000);
}

/*===========================================================================
FUNCTION
===========================================================================*/
static void copy_change_clock(void)
{
	unsigned long  flags;
	unsigned int   stack;
	FuncPtr pFunc = (FuncPtr)(CKC_CHANGE_FUNC_ADDR);
	unsigned int	RDMA_sts = 0;
#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_CACHE_L2X0)
	unsigned way_mask;
#endif
#if defined(CONFIG_GENERIC_TIME)
	volatile PPIC	pPIC	= (volatile PPIC)tcc_p2v(HwPIC_BASE);
#else
	volatile PTIMER	pTIMER	= (volatile PTIMER)tcc_p2v(HwTMR_BASE);
#endif


//--------------------------------------------------------------
// copy clock change function to sram
	memcpy((void *)CKC_CHANGE_FUNC_ADDR, (void*)change_clock, CKC_CHANGE_FUNC_SIZE);

//--------------------------------------------------------------
// disable LCD
#if 1//defined(CONFIG_DRAM_DDR2)
	#if defined(CONFIG_MACH_TCC8920ST)
		DEV_LCDC_Wait_signal_Ext();
	#else
		DEV_LCDC_Wait_signal(0);
		DEV_LCDC_Wait_signal(1);
	#endif
#endif

//--------------------------------------------------------------
// disable irq
	local_irq_save(flags);
	local_irq_disable();

//--------------------------------------------------------------
// flush tlb
	local_flush_tlb_all();

//--------------------------------------------------------------
// flush internal cache(ICache, DCache)
//	flush_cache_all();	// it spend 1~2milisec. time

//--------------------------------------------------------------
// disable kernel tick timer
#if defined(CONFIG_GENERIC_TIME)
	pPIC->IEN0.bREG.TC1 = 0;		/* Disable Timer1 interrupt */
#else
	pTIMER->TC32EN.bREG.EN = 0;
#endif

//--------------------------------------------------------------
// L2 cache off
#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_CACHE_L2X0)
	if (*(volatile unsigned int *)(L2CACHE_BASE+L2X0_AUX_CTRL) & (1 << 16))
		way_mask = (1 << 16) - 1;
	else
		way_mask = (1 << 8) - 1;
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CLEAN_WAY) = way_mask; //clean all
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CLEAN_WAY)&way_mask); //wait for clean
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC) = 0; //sync
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC)&1); //wait for sync
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 0; //cache off
#endif

//--------------------------------------------------------------
// change stack to sram
	stack = IO_ARM_ChangeStackSRAM();

////////////////////////////////////////////////////////////////
	pFunc();
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
// restore stack
	IO_ARM_RestoreStackSRAM(stack);

//--------------------------------------------------------------
// L2 cache on
#if defined(CONFIG_DRAM_DDR3) && defined(CONFIG_CACHE_L2X0)
	if (*(volatile unsigned int *)(L2CACHE_BASE+L2X0_AUX_CTRL) & (1 << 16))
		way_mask = (1 << 16) - 1;
	else
		way_mask = (1 << 8) - 1;
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_INV_WAY) = way_mask; //invalidate all
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_INV_WAY)&way_mask); //wait for invalidate
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC) = 0; //sync
	while(*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CACHE_SYNC)&1); //wait for sync
	*(volatile unsigned int *)(L2CACHE_BASE+L2X0_CTRL) = 1; //cache on
#endif

	
//--------------------------------------------------------------
// enable kernel tick timer
#if !defined(CONFIG_GENERIC_TIME)
	pTIMER->TC32EN |= Hw24;
#endif

//--------------------------------------------------------------
// enable irq
	local_irq_restore(flags);

//--------------------------------------------------------------
// enable kernel tick timer interrupt
#if defined(CONFIG_GENERIC_TIME)
	pPIC->IEN0.bREG.TC1 = 1;
#endif

}

/*===========================================================================
FUNCTION
===========================================================================*/
void tcc_ddr_set_clock(unsigned int freq)
{
	unsigned int   mem_freq;

	mem_freq = get_membus_ckc(freq*10);
	get_ddr_param(mem_freq);

	//printk("**********mem clock change in %dMHz\n", mem_freq);
	copy_change_clock();
	//printk("**********mem clock change out\n");

}

/*=========================================================================*/

EXPORT_SYMBOL(tcc_ddr_set_clock);


