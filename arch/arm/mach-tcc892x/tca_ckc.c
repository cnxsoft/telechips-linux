/****************************************************************************
 *  FileName    : tca_ckc.c
 *  Description :
 ****************************************************************************
 *
 *  TCC Version 1.0
 *  Copyright (c) Telechips, Inc.
 *  ALL RIGHTS RESERVED
 *
 ****************************************************************************/

#include <mach/bsp.h>
#include <asm/io.h>
#include <linux/mm.h>       // for PAGE_ALIGN
#include <linux/kernel.h>
#include <linux/module.h>

#define CPU_SRC_CH  5
#define CPU_SRC_PLL DIRECTPLL5

#define MEM_SRC_CH  4
#define MEM_SRC_PLL DIRECTPLL4

#if defined(CONFIG_GPU_BUS_SCALING)
#define USE_GPU_SCALING
#endif
#if defined(USE_GPU_SCALING)
#define GPU_SRC_CH  3
#define GPU_SRC_PLL DIRECTPLL3
#endif

#define MAX_PERI_DIV    4096
#define MAX_FBUS_DIV    16

typedef struct {
    unsigned int    uFpll;
    unsigned int    P;
    unsigned int    M;
    unsigned int    S;
    unsigned int    VSEL;
} sfPLL;

#define PLLFREQ(P, M, S)        (( 120000 * (M) )  / (P) ) >> (S) // 100Hz Unit..
#define FPLL_t(P, M, S, VSEL)   PLLFREQ(P,M,S), P, M, S, VSEL

static sfPLL pIO_CKC_PLL[] = {
    {FPLL_t(3, 300,  0,  0)},       // 1200 MHz
    {FPLL_t(3, 275,  0,  0)},       // 1100 MHz
    //{FPLL_t(3, 251,  0,  0)},       // 1004 MHz
    {FPLL_t(3, 502,  1,  1)},       // 1004 MHz
    {FPLL_t(3, 498,  1,  1)},       // 996 MHz
    {FPLL_t(3, 494,  1,  1)},       // 988 MHz
    {FPLL_t(3, 490,  1,  1)},       // 980 MHz
    {FPLL_t(3, 486,  1,  1)},       // 972 MHz
    {FPLL_t(3, 482,  1,  1)},       // 964 MHz
    {FPLL_t(3, 478,  1,  1)},       // 956 MHz
    {FPLL_t(3, 474,  1,  1)},       // 948 MHz
    {FPLL_t(3, 470,  1,  1)},       // 940 MHz
    {FPLL_t(3, 466,  1,  1)},       // 932 MHz
    {FPLL_t(3, 462,  1,  1)},       // 924 MHz
    {FPLL_t(3, 458,  1,  1)},       // 916 MHz
    {FPLL_t(3, 454,  1,  1)},       // 908 MHz
    {FPLL_t(3, 450,  1,  1)},       // 900 MHz
    {FPLL_t(3, 446,  1,  1)},       // 892 MHz
    {FPLL_t(3, 442,  1,  1)},       // 884 MHz
    {FPLL_t(3, 438,  1,  1)},       // 876 MHz
    {FPLL_t(3, 434,  1,  1)},       // 868 MHz
    {FPLL_t(3, 432,  1,  1)},       // 864 MHz
    {FPLL_t(3, 428,  1,  1)},       // 856 MHz
    {FPLL_t(3, 424,  1,  1)},       // 848 MHz
    {FPLL_t(3, 420,  1,  1)},       // 840 MHz
    {FPLL_t(3, 416,  1,  1)},       // 832 MHz
    {FPLL_t(3, 412,  1,  1)},       // 824 MHz
    {FPLL_t(3, 408,  1,  1)},       // 816 MHz
    {FPLL_t(3, 404,  1,  1)},       // 808 MHz
    {FPLL_t(3, 400,  1,  1)},       // 800 MHz
    {FPLL_t(3, 396,  1,  1)},       // 792 MHz
    {FPLL_t(3, 392,  1,  1)},       // 784 MHz
    {FPLL_t(3, 388,  1,  1)},       // 776 MHz
    {FPLL_t(3, 384,  1,  1)},       // 768 MHz
    {FPLL_t(3, 380,  1,  1)},       // 760 MHz
    {FPLL_t(3, 376,  1,  1)},       // 752 MHz
    {FPLL_t(3, 372,  1,  1)},       // 744 MHz
    {FPLL_t(3, 370,  1,  1)},       // 740 MHz
    {FPLL_t(3, 368,  1,  1)},       // 736 MHz
    {FPLL_t(3, 364,  1,  1)},       // 728 MHz
    {FPLL_t(3, 360,  1,  1)},       // 720 MHz
    {FPLL_t(3, 356,  1,  1)},       // 712 MHz
    {FPLL_t(3, 352,  1,  1)},       // 704 MHz
    {FPLL_t(3, 350,  1,  0)},       // 700 MHz
    {FPLL_t(3, 348,  1,  0)},       // 696 MHz
    {FPLL_t(3, 344,  1,  0)},       // 688 MHz
    {FPLL_t(3, 340,  1,  0)},       // 680 MHz
    {FPLL_t(3, 336,  1,  0)},       // 672 MHz
    {FPLL_t(3, 332,  1,  0)},       // 664 MHz
    {FPLL_t(3, 328,  1,  0)},       // 656 MHz
    {FPLL_t(3, 324,  1,  0)},       // 648 MHz
    {FPLL_t(3, 320,  1,  0)},       // 640 MHz
    {FPLL_t(3, 316,  1,  0)},       // 632 MHz
    {FPLL_t(3, 312,  1,  0)},       // 624 MHz
    {FPLL_t(3, 308,  1,  0)},       // 616 MHz
    {FPLL_t(3, 304,  1,  0)},       // 608 MHz
    {FPLL_t(3, 300,  1,  0)},       // 600 MHz
    {FPLL_t(3, 297,  1,  0)},       // 594 MHz
    {FPLL_t(3, 296,  1,  0)},       // 592 MHz
    {FPLL_t(3, 292,  1,  0)},       // 584 MHz
    {FPLL_t(3, 288,  1,  0)},       // 576 MHz
    {FPLL_t(3, 284,  1,  0)},       // 568 MHz
    {FPLL_t(3, 280,  1,  0)},       // 560 MHz
    {FPLL_t(3, 276,  1,  0)},       // 552 MHz
    {FPLL_t(3, 272,  1,  0)},       // 544 MHz
    {FPLL_t(3, 268,  1,  0)},       // 536 MHz
    {FPLL_t(3, 264,  1,  0)},       // 528 MHz
    {FPLL_t(3, 260,  1,  0)},       // 520 MHz
    {FPLL_t(3, 256,  1,  0)},       // 512 MHz
    {FPLL_t(3, 252,  1,  0)},       // 504 MHz
    {FPLL_t(3, 500,  2,  1)},       // 500 MHz
    {FPLL_t(3, 496,  2,  1)},       // 496 MHz
    {FPLL_t(3, 488,  2,  1)},       // 488 MHz
    {FPLL_t(3, 480,  2,  1)},       // 480 MHz
    {FPLL_t(3, 472,  2,  1)},       // 472 MHz
    {FPLL_t(3, 464,  2,  1)},       // 464 MHz
    {FPLL_t(3, 456,  2,  1)},       // 456 MHz
    {FPLL_t(3, 448,  2,  1)},       // 448 MHz
    {FPLL_t(3, 440,  2,  1)},       // 440 MHz
    {FPLL_t(3, 432,  2,  1)},       // 432 MHz
    {FPLL_t(3, 424,  2,  1)},       // 424 MHz
    {FPLL_t(3, 416,  2,  1)},       // 416 MHz
    {FPLL_t(3, 408,  2,  1)},       // 408 MHz
    {FPLL_t(3, 400,  2,  1)},       // 400 MHz
    {FPLL_t(3, 304,  2,  0)},       // 304 MHz
    {FPLL_t(3, 294,  2,  0)},       // 294 MHz
    {FPLL_t(3, 408,  3,  1)},       // 204 MHz
    {FPLL_t(3, 384,  3,  1)},       // 192 MHz
    {FPLL_t(3, 404,  4,  1)},       // 101 MHz
    {FPLL_t(3, 344,  4,  0)},       //  86 MHz
};

#define NUM_PLL                 (sizeof(pIO_CKC_PLL)/sizeof(sfPLL))

#define tca_wait()              { volatile int i; for (i=0; i<100; i++); }

#define tca_pll_lock_wait(addr) { \
                                    volatile unsigned int i; \
                                    for (i=100; i ; i--) \
                                        while((*addr & 0x00200000) == 0); \
                                    for (i=0x200; i ; i--); \
                                }

#define MAX_TCC_PLL             6
#define MAX_CLK_SRC             (MAX_TCC_PLL*2 + 1)     // XIN


/****************************************************************************************
* External Functions
* ***************************************************************************************/
extern void tcc_ddr_set_clock(unsigned int freq);


/****************************************************************************************
* Local Variables
* ***************************************************************************************/
static volatile PCKC            pCKC ;
static volatile PPMU            pPMU ;
static volatile PIOBUSCFG       pIOBUSCFG;
static volatile PDDICONFIG      pDDIBUSCFG;
static volatile PGRPBUSCFG      pGPUBUSCFG;
static volatile PVIDEOBUSCFG    pVIDEOBUSCFG;
static volatile PHSIOBUSCFG     pHSIOBUSCFG;
static volatile PMEMBUSCFG      pMEMBUSCFG;
static volatile PCLK_XXX_TYPE   stHDMIReg;
static volatile PCLK_YYY_TYPE   stHDMIAudReg;

static unsigned int stClockSource[MAX_CLK_SRC];
static int initialized = 0;
static int hdml_lcdc_flag = 0;
static int hdmi_lcdc_source = 0;


/****************************************************************************************
* FUNCTION :void tca_ckc_init(void)
* DESCRIPTION :
* ***************************************************************************************/
void tca_ckc_init(void)
{
    int i;
    unsigned int mem_pll;
    pCKC = (CKC *)io_p2v(HwCKC_BASE);
    pPMU = (PMU *)io_p2v(HwPMU_BASE);
    pIOBUSCFG = (IOBUSCFG *)io_p2v(HwIOBUSCFG_BASE);
    pDDIBUSCFG = (DDICONFIG *)io_p2v(HwDDI_CONFIG_BASE);
    pGPUBUSCFG = (GRPBUSCFG *)io_p2v(HwGRPBUSCONFIG_BASE);
    pVIDEOBUSCFG = (VIDEOBUSCFG *)io_p2v(HwVIDEOBUSCONFIG_BASE);
    pHSIOBUSCFG = (HSIOBUSCFG *)io_p2v(HwHSIOBUSCFG_BASE);
    pMEMBUSCFG = (MEMBUSCFG *)io_p2v(HwMBUSCFG_BASE);

    /* IOBUS AHB2AXI: flushs prefetch buffer when bus state is IDLE or WRITE
       enable:  A2XMOD1 (Audio DMA, GPSB, DMA2/3, EHI1)
       disable: A2XMOD0 (USB1.1Host, USB OTG, SD/MMC, IDE, DMA0/1, MPEFEC, EHI0)
    */
#if 0
    pIOBUSCFG->IO_A2X.bREG.A2XMOD2 = 1;
    pIOBUSCFG->IO_A2X.bREG.A2XMOD1 = 1;
    pIOBUSCFG->IO_A2X.bREG.A2XMOD0 = 1;
    pHSIOBUSCFG->HSIO_CFG.bREG.A2X_USB20H = 1;

    pCKC->CLKDIVC0.nREG    = 0x01010101;    // PLL0,PLL1,PLL2,PLL3
    pCKC->CLKDIVC1.nREG    = 0x01010101;    // PLL4,PLL5,XIN,XTIN
#endif

    if (initialized)
        return;

    stHDMIReg.nREG = 0x24000000;
    stHDMIAudReg.nREG = 0x24000000;

    for (i=0 ; i<MAX_TCC_PLL ; i++) {
        if (i == CPU_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("    pll_%d:  cpu clock source\n", i);
        }
        else if (i == MEM_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("    pll_%d:  memory clock source\n", i);
        }
#if defined(USE_GPU_SCALING)
        else if (i == GPU_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("    pll_%d:  graphic clock source\n", i);
        }
#endif
        else {
            stClockSource[i] = tca_ckc_getpll(i);
            pr_info("    pll_%d:  %d kHz (Fixed)\n", i, stClockSource[i]/10);
        }
    }
    for ( ; i<(MAX_TCC_PLL*2) ; i++) {
        if ((i-MAX_TCC_PLL) == CPU_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("div_pll_%d:  cpu clock source\n", i-MAX_TCC_PLL);
        }
        else if ((i-MAX_TCC_PLL) == MEM_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("div_pll_%d:  memory clock source\n", i-MAX_TCC_PLL);
        }
#if defined(USE_GPU_SCALING)
        else if ((i-MAX_TCC_PLL) == GPU_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("div_pll_%d:  graphic clock source\n", i-MAX_TCC_PLL);
        }
#endif
        else {
            stClockSource[i] = tca_ckc_getdividpll(i-MAX_TCC_PLL);
            pr_info("div_pll_%d:  %d kHz (Fixed)\n", i-MAX_TCC_PLL, stClockSource[i]/10);
        }
    }

    stClockSource[i] = 120000;    // XIN
    pr_info("      xin:  %d kHz (Fixed)\n", stClockSource[i]/10);

    mem_pll = tca_ckc_getpll(MEM_SRC_CH);
    tcc_ddr_set_clock(mem_pll/20);

    initialized = 1;
    hdml_lcdc_flag = 0;
}

/****************************************************************************************
* FUNCTION :int tca_ckc_setpll(unsigned int pll, unsigned int ch)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setpll(unsigned int pll, unsigned int ch)
{
    unsigned uCnt;
    sfPLL    *pPLL;
    volatile PLLCFG_TYPE *pPLLCFG = (PLLCFG_TYPE *)((&pCKC->PLL0CFG)+ch);

    if (ch >= MAX_TCC_PLL)
        return -1;

    if (ch == MEM_SRC_CH) {
        stClockSource[ch] = 0;
        stClockSource[MAX_TCC_PLL+ch] = 0;
        return -1;
    }


     if(pll != 0) {
        pPLL = &pIO_CKC_PLL[0];
        for (uCnt = 0; uCnt < NUM_PLL; uCnt ++, pPLL++) {
            if (pPLL->uFpll <= pll)
                break;
        }

        if (uCnt >= NUM_PLL) {
            uCnt = NUM_PLL - 1;
            pPLL = &pIO_CKC_PLL[uCnt];
        }

        pPLLCFG->nREG = ((pPLL->VSEL&0x1) << 30) | ((pPLL->S&0x7) << 24) | (1 << 20) | ((pPLL->M&0x3FF) << 8) | (pPLL->P&0x3F);
        pPLLCFG->nREG |= (1 << 31);
        tca_pll_lock_wait(&(pPLLCFG->nREG));

        if (ch == CPU_SRC_CH) {
            stClockSource[ch] = 0;
            stClockSource[MAX_TCC_PLL+ch] = 0;
            return 0;
        }
        else {
            stClockSource[ch] = pPLL->uFpll;
            stClockSource[MAX_TCC_PLL+ch] = tca_ckc_getdividpll(ch);
        }
    }
    else {
        pPLLCFG->bREG.EN = 0;
        stClockSource[ch] = 0;
        stClockSource[MAX_TCC_PLL+ch] = 0;
    }
    return 0;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_getpll(unsigned int ch)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_getpll(unsigned int ch)
{
    unsigned int tPLL;
    unsigned int tPCO;
    unsigned    iP=0, iM=0, iS=0, iEN = 0;

    switch(ch) {
        case 0:
            iEN = pCKC->PLL0CFG.bREG.EN;
            iP = pCKC->PLL0CFG.bREG.P;
            iM = pCKC->PLL0CFG.bREG.M;
            iS = pCKC->PLL0CFG.bREG.S;
            break;
        case 1:
            iEN = pCKC->PLL1CFG.bREG.EN;
            iP = pCKC->PLL1CFG.bREG.P;
            iM = pCKC->PLL1CFG.bREG.M;
            iS = pCKC->PLL1CFG.bREG.S;
            break;
        case 2:
            iEN = pCKC->PLL2CFG.bREG.EN;
            iP = pCKC->PLL2CFG.bREG.P;
            iM = pCKC->PLL2CFG.bREG.M;
            iS = pCKC->PLL2CFG.bREG.S;
            break;
        case 3:
            iEN = pCKC->PLL3CFG.bREG.EN;
            iP = pCKC->PLL3CFG.bREG.P;
            iM = pCKC->PLL3CFG.bREG.M;
            iS = pCKC->PLL3CFG.bREG.S;
            break;
        case 4:
            iEN = pCKC->PLL4CFG.bREG.EN;
            iP = pCKC->PLL4CFG.bREG.P;
            iM = pCKC->PLL4CFG.bREG.M;
            iS = pCKC->PLL4CFG.bREG.S;
            break;
        case 5:
            iEN = pCKC->PLL5CFG.bREG.EN;
            iP = pCKC->PLL5CFG.bREG.P;
            iM = pCKC->PLL5CFG.bREG.M;
            iS = pCKC->PLL5CFG.bREG.S;
            break;
    }

    if (iEN == 0)
        return 0;

    tPCO = (120000 * iM ) / iP;
    tPLL= ((tPCO) >> (iS));

    return tPLL;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_getdividpll(unsigned int ch)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_getdividpll(unsigned int ch)
{
    unsigned int tDIVPLL;
    unsigned int tPLL = tca_ckc_getpll(ch);
    unsigned int uiPDIV = 0;

    if (tPLL == 0)
        return 0;

    switch(ch) {
        case 0:
            if (pCKC->CLKDIVC0.bREG.P0TE == 0)
                return 0;
            uiPDIV = pCKC->CLKDIVC0.bREG.P0DIV;
            break;
        case 1:
            if (pCKC->CLKDIVC0.bREG.P1TE == 0)
                return 0;
            uiPDIV = pCKC->CLKDIVC0.bREG.P1DIV;
            break;
        case 2:
            if (pCKC->CLKDIVC0.bREG.P2TE == 0)
                return 0;
            uiPDIV = pCKC->CLKDIVC0.bREG.P2DIV;
            break;
        case 3:
            if (pCKC->CLKDIVC0.bREG.P3TE == 0)
                return 0;
            uiPDIV = pCKC->CLKDIVC0.bREG.P3DIV;
            break;
        case 4:
            if (pCKC->CLKDIVC1.bREG.P4TE == 0)
                return 0;
            uiPDIV = pCKC->CLKDIVC1.bREG.P4DIV;
            break;
        case 5:
            if (pCKC->CLKDIVC1.bREG.P5TE == 0)
                return 0;
            uiPDIV = pCKC->CLKDIVC1.bREG.P5DIV;
            break;
    }

    //Fdivpll Clock
    tDIVPLL = (unsigned int)tPLL/(uiPDIV+1);

    return tDIVPLL;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setcpu(unsigned int n)
* DESCRIPTION :  n is n/16
* example : CPU == PLL : n=16 - CPU == PLL/2 : n=8
* ***************************************************************************************/
static unsigned int tca_ckc_setcpu(unsigned int freq)
{
    unsigned uCnt;
    sfPLL    *pPLL;
    volatile PLLCFG_TYPE *pPLLCFG = (PLLCFG_TYPE *)((&pCKC->PLL0CFG)+CPU_SRC_CH);

    // 1. temporally change the cpu clock source.(XIN)
    pCKC->CLKCTRL0.nREG = (pCKC->CLKCTRL0.nREG & 0xFFF00000) | 0xFFFF4;

    // 2. change pll(for cpu) clock.
#if (0)
    tca_ckc_setpll(freq, CPU_SRC_CH);
#else
    pPLL = &pIO_CKC_PLL[0];
    for (uCnt = 0; uCnt < NUM_PLL; uCnt ++, pPLL++) {
        if (pPLL->uFpll <= freq)
            break;
    }

    if (uCnt >= NUM_PLL) {
        uCnt = NUM_PLL - 1;
        pPLL = &pIO_CKC_PLL[uCnt];
    }

    pPLLCFG->nREG = ((pPLL->VSEL&0x1) << 30) | ((pPLL->S&0x7) << 24) | (1 << 20) | ((pPLL->M&0x3FF) << 8) | (pPLL->P&0x3F);
    pPLLCFG->nREG |= (1 << 31);
    tca_pll_lock_wait(&(pPLLCFG->nREG));
#endif

    // 3. change th cpu clock source.
    pCKC->CLKCTRL0.nREG = (pCKC->CLKCTRL0.nREG & 0xFFF00000) | 0xFFFF0 | CPU_SRC_PLL;

    tca_wait();

    return tca_ckc_getpll(CPU_SRC_CH);
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_getcpu(void)
* DESCRIPTION :
* ***************************************************************************************/
static unsigned int tca_ckc_getcpu(void)
{
    unsigned int lcpu = 0;
    unsigned int lconfig = 0;
    unsigned int lcnt = 0;
    unsigned int li = 0;
    unsigned int lclksource = 0;

    lconfig = pCKC->CLKCTRL0.bREG.CONFIG;

    for(li = 0; li < 16; li++) {
        if((lconfig & Hw0) == 1)
            lcnt++;
        lconfig = (lconfig >> 1);
    }

    switch(pCKC->CLKCTRL0.bREG.SEL) {
        case DIRECTPLL0 :
            lclksource =  tca_ckc_getpll(0);
            break;
        case DIRECTPLL1 :
            lclksource =  tca_ckc_getpll(1);
            break;
        case DIRECTPLL2 :
            lclksource =  tca_ckc_getpll(2);
            break;
        case DIRECTPLL3 :
            lclksource =  tca_ckc_getpll(3);
            break;
        case DIRECTXIN:
            lclksource =  120000;
            break;
        case DIVIDPLL0:
            lclksource = tca_ckc_getdividpll(0);
            break;
        case DIVIDPLL1:
            lclksource = tca_ckc_getdividpll(1);
            break;
        /*
        case DIRECTXTIN:
            lclksource =  327;
            break;
        */
        case DIRECTPLL4:
            lclksource =  tca_ckc_getpll(4);
            break;
        case DIRECTPLL5:
            lclksource =  tca_ckc_getpll(5);
            break;
        case DIVIDPLL2:
            lclksource = tca_ckc_getdividpll(2);
            break;
        case DIVIDPLL3:
            lclksource = tca_ckc_getdividpll(3);
            break;
        case DIVIDPLL4:
            lclksource = tca_ckc_getdividpll(4);
            break;
        case DIVIDPLL5:
            lclksource = tca_ckc_getdividpll(5);
            break;
        /*
        case DIVIDXIN:
            break;
        case DIVIDXTIN:
            break;
        */
        default :
            lclksource =  tca_ckc_getpll(5);
            break;
    }

    lcpu = (lclksource * lcnt)/16;

    return lcpu;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setfpll(unsigned int pll, unsigned int fbus, unsigned int freq)
* ***************************************************************************************/
static unsigned int tca_ckc_setfpll(unsigned int pll_ch, unsigned int pll_src, unsigned int fbus_ch, unsigned int freq)
{
    unsigned uCnt;
    sfPLL    *pPLL;
    volatile PLLCFG_TYPE *pPLLCFG = (PLLCFG_TYPE *)((&pCKC->PLL0CFG)+pll_ch);
    volatile CLKCTRL_TYPE *pCLKCTRL = (CLKCTRL_TYPE *)((&pCKC->CLKCTRL0)+fbus_ch);

    if (freq < 480000)
        freq = 480000;

    // 1. temporally change the fbus clock source.(XIN)
    pCLKCTRL->nREG = (pCLKCTRL->nREG & 0xFFF00000) | 0x00014;

    // 2. change pll(for fbus) clock.
#if (0)
    tca_ckc_setpll(freq*2, CPU_SRC_CH);
#else
    pPLL = &pIO_CKC_PLL[0];
    for (uCnt = 0; uCnt < NUM_PLL; uCnt ++, pPLL++) {
        if (pPLL->uFpll <= freq*2)
            break;
    }

    if (uCnt >= NUM_PLL) {
        uCnt = NUM_PLL - 1;
        pPLL = &pIO_CKC_PLL[uCnt];
    }

    pPLLCFG->nREG = ((pPLL->VSEL&0x1) << 30) | ((pPLL->S&0x7) << 24) | (1 << 20) | ((pPLL->M&0x3FF) << 8) | (pPLL->P&0x3F);
    pPLLCFG->nREG |= (1 << 31);
    tca_pll_lock_wait(&(pPLLCFG->nREG));
#endif

    // 3. change the fbus clock source.
    pCLKCTRL->nREG = (pCLKCTRL->nREG & 0xFFF00000) | 0x00010 | pll_src;

    tca_wait();

    return tca_ckc_getpll(pll_ch)/2;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setfbusctrl(unsigned int clkname,unsigned int isenable,unsigned int freq)
* DESCRIPTION :
*   return: real clock rate. ( unit: 100 Hz)
* ***************************************************************************************/
unsigned int tca_ckc_setfbusctrl(unsigned int clkname, unsigned int isenable, unsigned int freq)
{
    volatile CLKCTRL_TYPE *pCLKCTRL = (CLKCTRL_TYPE *)((&pCKC->CLKCTRL0)+clkname);
    unsigned int div[MAX_CLK_SRC], div_100[MAX_CLK_SRC], i, clksrc, searchsrc, overclksrc;
    unsigned int clkrate=0, clkdiv=0;

    searchsrc = 0xFFFFFFFF;
    overclksrc = 0xFFFFFFFF;

    if (clkname == FBUS_CPU)        // CPU
        return tca_ckc_setcpu(freq);
    else if (clkname == FBUS_MEM) {    // Memory
        tcc_ddr_set_clock(freq/10);
        return freq;
    }
#if defined(USE_GPU_SCALING)
    else if (clkname == FBUS_GPU) {
        return tca_ckc_setfpll(GPU_SRC_CH, GPU_SRC_PLL, FBUS_GPU, freq);
    }
#endif
    if (freq < 480000)
        freq = 480000;

    if (freq <= 60000) {
        clksrc = DIRECTXIN;
        clkrate = 60000;
        clkdiv = 1;
    }
    else {
        for (i=0 ; i<MAX_CLK_SRC ; i++) {
            if (stClockSource[i] == 0)
                continue;
        if (stClockSource[i] < freq)
            continue;
            div_100[i] = stClockSource[i]/(freq/100);

            if (div_100[i] > MAX_FBUS_DIV*100)
                div_100[i] = MAX_FBUS_DIV*100;

            /* find maximum frequency pll source */
            if (div_100[i] <= 100) {
                if (overclksrc == 0xFFFFFFFF)
                    overclksrc = i;
                else if (stClockSource[i] > stClockSource[overclksrc])
                    overclksrc = i;
                continue;
            }

            div[i]= div_100[i]/100;
            if (div_100[i]%100)
                div[i] += 1;

            if (div[i] < 2)
                div[i] = 2;

            div_100[i] = freq - stClockSource[i]/div[i];

            if (searchsrc == 0xFFFFFFFF)
                searchsrc = i;
            else {
                /* find similar clock */
                if (div_100[i] < div_100[searchsrc])
                    searchsrc = i;
                /* find even division vlaue */
                else if(div_100[i] == div_100[searchsrc]) {
                    if (div[searchsrc]%2)
                        searchsrc = i;
                    else if (div[searchsrc] > div[i])
                        searchsrc = i;
                }
            }
        }
        if (searchsrc == 0xFFFFFFFF) {
            if (overclksrc == 0xFFFFFFFF) {
                overclksrc = 0;
                for (i=1 ; i<MAX_CLK_SRC ; i++) {
                    if (stClockSource[i] > stClockSource[overclksrc])
                        overclksrc = i;
                }
            }
            searchsrc = overclksrc;
            div[searchsrc] = 2;
        }        
        switch(searchsrc) {
            case 0: clksrc = DIRECTPLL0; break;
            case 1: clksrc = DIRECTPLL1; break;
            case 2: clksrc = DIRECTPLL2; break;
            case 3: clksrc = DIRECTPLL3; break;
            case 4: clksrc = DIRECTPLL4; break;
            case 5: clksrc = DIRECTPLL5; break;
            case 6: clksrc = DIVIDPLL0; break;
            case 7: clksrc = DIVIDPLL1; break;
            case 8: clksrc = DIVIDPLL2; break;
            case 9: clksrc = DIVIDPLL3; break;
            case 10: clksrc = DIVIDPLL4; break;
            case 11: clksrc = DIVIDPLL5; break;
            case 12: clksrc = DIRECTXIN; break;
            default: return 0;
        }
        clkrate = stClockSource[searchsrc]/((div[searchsrc]>16)?16:div[searchsrc]);

        if (div[searchsrc] > MAX_FBUS_DIV)
            clkdiv = MAX_FBUS_DIV - 1;
        else
            clkdiv = div[searchsrc] - 1;
    }
    
    if(clkdiv == CLKDIV0)
        clkdiv = 1;

#if (0)
    pCLKCTRL->nREG = (pCLKCTRL->nREG & (1<<21)) | ((clkdiv&0xF) << 4) | (clksrc&0xF);
    if(isenable == ENABLE)
        pCLKCTRL->nREG |= 1<<21;
    else if (isenable == DISABLE)
        pCLKCTRL->nREG &= ~(1<<21);
#else
    pCLKCTRL->nREG = (pCLKCTRL->nREG & 0xFFF0000F) | 0xF0;
    pCLKCTRL->bREG.SEL = clksrc;
    while(pCLKCTRL->bREG.CHGREQ);
    pCLKCTRL->bREG.CONFIG = clkdiv;
    while(pCLKCTRL->bREG.CFGREQ);
    if(isenable == ENABLE)
        pCLKCTRL->bREG.EN = 1;
    else if (isenable == DISABLE)
        pCLKCTRL->bREG.EN = 0;
    while(pCLKCTRL->bREG.CFGREQ);
#endif
    tca_wait();
    return clkrate;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_getfbusctrl(unsigned int clkname)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_getfbusctrl(unsigned int clkname)
{
    volatile CLKCTRL_TYPE *pCLKCTRL = (CLKCTRL_TYPE *)((&pCKC->CLKCTRL0)+clkname);
    unsigned int clksource = 0;

    if (pCLKCTRL->bREG.EN == 0)
        return 0;

    if(clkname == FBUS_CPU)
        return tca_ckc_getcpu();

    switch(pCLKCTRL->bREG.SEL) {
        case DIRECTPLL0:
            clksource =  tca_ckc_getpll(0);
            break;
        case DIRECTPLL1:
            clksource =  tca_ckc_getpll(1);
            break;
        case DIRECTPLL2:
            clksource =  tca_ckc_getpll(2);
            break;
        case DIRECTPLL3:
            clksource =  tca_ckc_getpll(3);
            break;
        case DIRECTXIN:
            clksource =  120000;
            break;
        case DIVIDPLL0:
            clksource =  tca_ckc_getdividpll(0);
            break;
        case DIVIDPLL1:
            clksource =  tca_ckc_getdividpll(1);
            break;
        /*
        case DIRECTXTIN:
            clksource =  327;
            break;
        */
        case DIRECTPLL4:
            clksource =  tca_ckc_getpll(4);
            break;
        case DIRECTPLL5:
            clksource =  tca_ckc_getpll(5);
            break;
        case DIVIDPLL2:
            clksource =  tca_ckc_getdividpll(2);
            break;
        case DIVIDPLL3:
            clksource =  tca_ckc_getdividpll(3);
            break;
        case DIVIDPLL4:
            clksource =  tca_ckc_getdividpll(4);
            break;
        case DIVIDPLL5:
            clksource =  tca_ckc_getdividpll(5);
            break;
        /*
        case DIVIDXIN:
            clksource =  60000;
            break;
        case DIVIDXTIN:
            clksource =  163;
            break;
        */
        default: return 0;
    }

    return (clksource / (pCLKCTRL->bREG.CONFIG+1));
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setperi(unsigned int periname,unsigned int isenable, unsigned int freq, unsigned int sor)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_setperi(unsigned int periname,unsigned int isenable, unsigned int freq)
{
    volatile PCLK_XXX_TYPE *pPCLKCTRL_XXX = (PCLK_XXX_TYPE *)((&pCKC->PCLKCTRL00)+periname);
    volatile PCLK_YYY_TYPE *pPCLKCTRL_YYY = (PCLK_YYY_TYPE *)((&pCKC->PCLKCTRL00)+periname);
    unsigned int div[MAX_CLK_SRC], div_100[MAX_CLK_SRC], i, searchsrc, overclksrc, dco_shift;
    unsigned int clkmd, clksrc, clkdiv, clkrate;

    if (hdmi_lcdc_source == periname)
        return 0;

    searchsrc = 0xFFFFFFFF;
    overclksrc = 0xFFFFFFFF;

    /* PCK_YYY (n=0~55, n== 7,27,28,31,36,37,38) */
    if (periname == PERI_HDMIA || periname == PERI_ADAI1 || periname == PERI_ADAM1 || periname == PERI_SPDIF1 ||
        periname == PERI_ADAI0 || periname == PERI_ADAM0 || periname == PERI_SPDIF0 || periname == PERI_ADC)
    {
        if (periname == PERI_ADC) {
            clkmd = 1;        // Divider mode
            clksrc = PCDIRECTXIN;

            searchsrc = 0;
            div[searchsrc] = (120000 + freq - 1)/freq;
            clkdiv = div[searchsrc];
            clkrate = 120000/clkdiv;
            if (clkdiv > 0)
                clkdiv -= 1;
        }
        else {
            clkmd = 0;        // DCO mode
            if (freq < 65536)
                dco_shift = 0;
            else if (freq < 65536*2)  //  13.1072 MHz
                dco_shift = 1;
            else if (freq < 65536*4)  //  26.2144 MHz
                dco_shift = 2;
            else if (freq < 65536*8)  //  52.4288 MHz
                dco_shift = 3;
            else if (freq < 65536*16) // 104.8596 MHz
                dco_shift = 4;
            else                         // 209.7152 MHz
                dco_shift = 5;

            for (i=0 ; i<MAX_CLK_SRC ; i++) {
                if (stClockSource[i] == 0)
                    continue;
                if (stClockSource[i] < freq)
                    continue;
                div_100[i] = ((freq*(65536>>dco_shift))/(stClockSource[i]/100))<<dco_shift;
                if ((div_100[i]%100) > 50) {
                    div[i] = div_100[i]/100 + 1;
                    div_100[i] = 100 - (div_100[i]%100);
                }
                else {
                    div[i] = div_100[i]/100;
                    div_100[i] %= 100;
                }
                if (searchsrc == 0xFFFFFFFF)
                    searchsrc = i;
                else {
                    /* find similar clock */
                    if (div_100[i] < div_100[searchsrc])
                        searchsrc = i;
                }
            }
            if (searchsrc == 0xFFFFFFFF) {
                if (overclksrc == 0xFFFFFFFF) {
                    overclksrc = 0;
                    for (i=1 ; i<MAX_CLK_SRC ; i++) {
                        if (stClockSource[i] > stClockSource[overclksrc])
                            overclksrc = i;
                    }
                }
                searchsrc = overclksrc;
                div[searchsrc] = 1;
            }

            switch(searchsrc) {
                case 0: clksrc = PCDIRECTPLL0; break;
                case 1: clksrc = PCDIRECTPLL1; break;
                case 2: clksrc = PCDIRECTPLL2; break;
                case 3: clksrc = PCDIRECTPLL3; break;
                case 4: clksrc = PCDIRECTPLL4; break;
                case 5: clksrc = PCDIRECTPLL5; break;
                case 6: clksrc = PCDIVIDPLL0; break;
                case 7: clksrc = PCDIVIDPLL1; break;
                case 8: clksrc = PCDIVIDPLL2; break;
                case 9: clksrc = PCDIVIDPLL3; break;
                case 10: clksrc = PCDIVIDPLL4; break;
                case 11: clksrc = PCDIVIDPLL5; break;
                case 12: clksrc = PCDIRECTXIN; break;
                default: return 0;
            }

            clkdiv = div[searchsrc];
            if (clkdiv > 32768)
                clkrate = ((stClockSource[searchsrc]>>dco_shift)*(65536-div[searchsrc]))/(65536>>dco_shift);
            else
                clkrate = ((stClockSource[searchsrc]>>dco_shift)*div[searchsrc])/(65536>>dco_shift);
        }

#if 1
        if (periname == PERI_HDMIA) {
            stHDMIAudReg.nREG = ((clkmd&0x1)<<31) | ((clksrc&0x1F)<<24) | (clkdiv&0xFFFF);
            if (isenable)
                stHDMIAudReg.nREG |= (1<<29);
            pPCLKCTRL_YYY->nREG = stHDMIAudReg.nREG;
        }
        else {
            pPCLKCTRL_YYY->nREG = ((clkmd&0x1)<<31) | ((clksrc&0x1F)<<24) | (clkdiv&0xFFFF);
            if (isenable)
                pPCLKCTRL_YYY->nREG |= (1<<29);
        }
#else
        pPCLKCTRL_YYY->bREG.MD = clkmd;
        pPCLKCTRL_YYY->bREG.EN = 0;
        pPCLKCTRL_YYY->bREG.DIV = clkdiv;
        pPCLKCTRL_YYY->bREG.SEL = clksrc;

        if (isenable)
            pPCLKCTRL_YYY->bREG.EN = 1;
#endif
    }
    else {
        if (periname == PERI_I2C0 || periname == PERI_I2C1 || periname == PERI_I2C2 || periname == PERI_I2C3) {
            clksrc = PCDIRECTXIN;
            searchsrc = 0;
            div[searchsrc] = (120000 + freq - 1)/freq;
            clkdiv = div[searchsrc];
            clkrate = 120000/clkdiv;
            if (clkdiv > 0)
                clkdiv -= 1;
        }
        else {
            for (i=0 ; i<MAX_CLK_SRC ; i++) {
                if (stClockSource[i] == 0)
                    continue;
                if (stClockSource[i] < freq)
                    continue;
                div_100[i] = stClockSource[i]/(freq/100);
                if (div_100[i] > MAX_PERI_DIV*100)
                    div_100[i] = MAX_PERI_DIV*100;
                if ((div_100[i]%100) > 50) {
                    div[i] = div_100[i]/100 + 1;
                    div_100[i] = 100 - (div_100[i]%100);
                }
                else {
                    div[i] = div_100[i]/100;
                    div_100[i] %= 100;
                }
                if (searchsrc == 0xFFFFFFFF)
                    searchsrc = i;
                else {
                    /* find similar clock */
                    if (div_100[i] < div_100[searchsrc])
                        searchsrc = i;
                    /* find even division vlaue */
                    else if(div_100[i] == div_100[searchsrc]) {
                        if (div[searchsrc]%2)
                            searchsrc = i;
                        else if (div[searchsrc] > div[i])
                            searchsrc = i;
                    }
                }
            }
            if (searchsrc == 0xFFFFFFFF) {
                if (overclksrc == 0xFFFFFFFF) {
                    overclksrc = 0;
                    for (i=1 ; i<MAX_CLK_SRC ; i++) {
                        if (stClockSource[i] > stClockSource[overclksrc])
                            overclksrc = i;
                    }
                }
                searchsrc = overclksrc;
                div[searchsrc] = 1;
            }

            switch(searchsrc) {
                case 0: clksrc = PCDIRECTPLL0; break;
                case 1: clksrc = PCDIRECTPLL1; break;
                case 2: clksrc = PCDIRECTPLL2; break;
                case 3: clksrc = PCDIRECTPLL3; break;
                case 4: clksrc = PCDIRECTPLL4; break;
                case 5: clksrc = PCDIRECTPLL5; break;
                case 6: clksrc = PCDIVIDPLL0; break;
                case 7: clksrc = PCDIVIDPLL1; break;
                case 8: clksrc = PCDIVIDPLL2; break;
                case 9: clksrc = PCDIVIDPLL3; break;
                case 10: clksrc = PCDIVIDPLL4; break;
                case 11: clksrc = PCDIVIDPLL5; break;
                case 12: clksrc = PCDIRECTXIN; break;
                default: return 0;
            }

            clkdiv = div[searchsrc];
            clkrate = stClockSource[searchsrc]/clkdiv;
            if (clkdiv > 0)
                clkdiv -= 1;
        }

        if (periname == PERI_HDMI) {
            stHDMIReg.bREG.EN = 0;
            stHDMIReg.bREG.DIV = 0;
            stHDMIReg.bREG.SEL = PCHDMI;
            if (isenable)
                stHDMIReg.bREG.EN = 1;
            pPCLKCTRL_XXX->nREG = stHDMIReg.nREG;
        }
        else {
            pPCLKCTRL_XXX->bREG.EN = 0;
            pPCLKCTRL_XXX->bREG.DIV = clkdiv;
            pPCLKCTRL_XXX->bREG.SEL = clksrc;
            if (isenable)
                pPCLKCTRL_XXX->bREG.EN = 1;
        }
    }

    return clkrate;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_getperi(unsigned int periname)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_getperi(unsigned int periname)
{
    volatile PCLK_XXX_TYPE *pPCLKCTRL_XXX = (PCLK_XXX_TYPE *)((&pCKC->PCLKCTRL00)+periname);
    volatile PCLK_YYY_TYPE *pPCLKCTRL_YYY = (PCLK_YYY_TYPE *)((&pCKC->PCLKCTRL00)+periname);
    unsigned int lclksource = 0;

    if (hdmi_lcdc_source == periname)
        return 0;

    if (pPCLKCTRL_XXX->bREG.EN == 0)
        return 0;

    switch(pPCLKCTRL_XXX->bREG.SEL) {
        case PCDIRECTPLL0 :
            lclksource =  tca_ckc_getpll(0);
            break;
        case PCDIRECTPLL1 :
            lclksource =  tca_ckc_getpll(1);
            break;
        case PCDIRECTPLL2 :
            lclksource =  tca_ckc_getpll(2);
            break;
        case PCDIRECTPLL3 :
            lclksource =  tca_ckc_getpll(3);
            break;
        case PCDIRECTXIN :
            lclksource =  120000;
            break;
        case PCDIVIDPLL0:
            lclksource =  tca_ckc_getdividpll(0);
            break;
        case PCDIVIDPLL1:
            lclksource =  tca_ckc_getdividpll(1);
            break;
        case PCDIVIDPLL2:
            lclksource =  tca_ckc_getdividpll(2);
            break;
        case PCDIVIDPLL3:
            lclksource =  tca_ckc_getdividpll(3);
            break;
        /*
        case PCDIRECTXTIN:
            lclksource =  327;
            break;
        case PCEXITERNAL:
            lclksource =  tca_ckc_getpll(3);
            break;
        case PCDIVIDXIN_HDMITMDS:
            lclksource =  tca_ckc_getpll(3);
            break;
        case PCDIVIDXTIN_HDMIPCLK:
            lclksource =  tca_ckc_getpll(3);
            break;
        case PCHDMI:
            lclksource =  tca_ckc_getpll(3);
            break;
        case PCSATA:
            lclksource =  tca_ckc_getpll(3);
            break;
        case PCUSBPHY:
            lclksource =  tca_ckc_getpll(3);
            break;
        case PCDIVIDXIN:
            lclksource =  60000;
            break;
        case PCDIVIDXTIN:
            lclksource =  163;
            break;
        */
        case PCDIRECTPLL4:
            lclksource =  tca_ckc_getpll(4);
            break;
        case PCDIRECTPLL5:
            lclksource =  tca_ckc_getpll(5);
            break;
        case PCDIVIDPLL4:
            lclksource =  tca_ckc_getdividpll(4);
            break;
        case PCDIVIDPLL5:
            lclksource =  tca_ckc_getdividpll(5);
            break;
        default :
            return 0;
    }

    if (periname == PERI_HDMIA || periname == PERI_ADAI1 || periname == PERI_ADAM1 || periname == PERI_SPDIF1 ||
        periname == PERI_ADAI0 || periname == PERI_ADAM0 || periname == PERI_SPDIF0 || periname == PERI_ADC)
    {
        if (pPCLKCTRL_YYY->nREG & (1<<31)) {
            return (lclksource/((pPCLKCTRL_YYY->nREG&0xFFFF) +1));
        }
        else {
            if (pPCLKCTRL_YYY->bREG.DIV > 32768)
                return ((lclksource * (65536 - (pPCLKCTRL_YYY->nREG&0xFFFF))) / 65536);
            else
                return ((lclksource * (pPCLKCTRL_YYY->nREG&0xFFFF)) / 65536);
        }
    }
    else {
        return (lclksource/(pPCLKCTRL_XXX->bREG.DIV+1));
    }

    return 0;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setpmuippwdn( unsigned int sel , unsigned int ispwdn)
* DESCRIPTION :
*   - fbusname : IP Isolation index
*   - ispwdn : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_setippwdn( unsigned int sel, unsigned int ispwdn)
{
#if (1)
    static unsigned int isol = 0x0;
    
    if( sel == PMU_ISOL_HDMI || sel == PMU_ISOL_LVDS ||
        sel == PMU_ISOL_VDAC || sel == PMU_ISOL_TSADC ||
        //Bruce, These IP powers are controlled by PM.
        //sel == PMU_ISOL_RTC || sel == PMU_ISOL_PLL || sel == PMU_ISOL_OTP || sel == PMU_ISOL_ECID ||
        sel == PMU_ISOL_USBHP || sel == PMU_ISOL_USBOP )
    {
        if (ispwdn)
            isol |= (1<<sel);
        else
            isol &= ~(1<<sel);

        pPMU->PMU_ISOL.nREG = isol;
        return 0;
    }
    return -1;
#else
    unsigned int ctrl_value;

    return 0;

    if (ispwdn)
        ctrl_value = 1;
    else
        ctrl_value = 0;
    
    switch (sel) {
        case PMU_ISOL_OTP:
            pPMU->PMU_ISOL.bREG.OTP = ctrl_value;
            break;
        case PMU_ISOL_RTC:
            pPMU->PMU_ISOL.bREG.RTC = ctrl_value;
            break;
        case PMU_ISOL_PLL:
            pPMU->PMU_ISOL.bREG.PLL = ctrl_value;
            break;
        case PMU_ISOL_ECID:
            pPMU->PMU_ISOL.bREG.ECID = ctrl_value;
            break;
        case PMU_ISOL_HDMI:
            pPMU->PMU_ISOL.bREG.HDMI = ctrl_value;
            break;
        case PMU_ISOL_VDAC:
            pPMU->PMU_ISOL.bREG.VDAC = ctrl_value;
            break;
        case PMU_ISOL_TSADC:
            pPMU->PMU_ISOL.bREG.TSADC = ctrl_value;
            break;
        case PMU_ISOL_USBHP:
            pPMU->PMU_ISOL.bREG.USBHP = ctrl_value;
            break;
        case PMU_ISOL_USBOP:
            pPMU->PMU_ISOL.bREG.USBOP = ctrl_value;
            break;
        case PMU_ISOL_LVDS:
            pPMU->PMU_ISOL.bREG.LVDS = ctrl_value;
            break;
        default:
            return -1;
    }
    return 0;
#endif
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setfbuspwdn( unsigned int fbusname , unsigned int ispwdn)
* DESCRIPTION :
*   - fbusname : CLKCTRL(n) index
*   - ispwdn : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_setfbuspwdn( unsigned int fbusname, unsigned int ispwdn)
{
    while(pPMU->PMU_PWRSTS.bREG.MAIN_STATE);

    switch (fbusname) {
        case FBUS_MEM:
            if (ispwdn) {
                do {
                    pPMU->PWRDN_MBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PD_MB == 0);
            }
            else {
                do {
                    pPMU->PWRUP_MBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PU_MB == 0);
            }
            break;
        case FBUS_DDI:
            if (ispwdn) {
                do {
                    pMEMBUSCFG->HCLKMASK.bREG.DBUS = 0;
                } while ((pMEMBUSCFG->MBUSSTS.bREG.DBUS0 | pMEMBUSCFG->MBUSSTS.bREG.DBUS1) == 1);

                pPMU->PMU_SYSRST.bREG.DB = 0;
                do {
                    pPMU->PWRDN_DBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PD_DB == 0);
            }
            else {
                do {
                    pPMU->PWRUP_DBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PU_DB == 0);
                pPMU->PMU_SYSRST.bREG.DB = 1;

                do {
                    pMEMBUSCFG->HCLKMASK.bREG.DBUS = 1;
                } while ((pMEMBUSCFG->MBUSSTS.bREG.DBUS0 & pMEMBUSCFG->MBUSSTS.bREG.DBUS1) == 0);
            }
            break;
        case FBUS_GPU:
            if (ispwdn) {
                do {
                    pMEMBUSCFG->HCLKMASK.bREG.GBUS = 0;
                } while (pMEMBUSCFG->MBUSSTS.bREG.GBUS == 1);

                pPMU->PMU_SYSRST.bREG.GB = 0;
                do {
                    pPMU->PWRDN_GBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PD_GB == 0);
            }
            else {
                do {
                    pPMU->PWRUP_GBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PU_GB == 0);
                pPMU->PMU_SYSRST.bREG.GB = 1;

                do {
                    pMEMBUSCFG->HCLKMASK.bREG.GBUS = 1;
                } while (pMEMBUSCFG->MBUSSTS.bREG.GBUS == 0);
            }
            break;
        case FBUS_VBUS:
            if (ispwdn) {
                do {pMEMBUSCFG->HCLKMASK.bREG.VBUS = 0;
                } while ((pMEMBUSCFG->MBUSSTS.bREG.VBUS0 | pMEMBUSCFG->MBUSSTS.bREG.VBUS1) == 1);

                pPMU->PMU_SYSRST.bREG.VB = 0;
                do {
                    pPMU->PWRDN_VBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PD_VB == 0);
            }
            else {
                do {
                    pPMU->PWRUP_VBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PU_VB == 0);
                pPMU->PMU_SYSRST.bREG.VB = 1;

                do {
                    pMEMBUSCFG->HCLKMASK.bREG.VBUS = 1;
                } while ((pMEMBUSCFG->MBUSSTS.bREG.VBUS0 & pMEMBUSCFG->MBUSSTS.bREG.VBUS1) == 0);
            }
            break;
        case FBUS_HSIO:
            if (ispwdn) {
                do {
                    pMEMBUSCFG->HCLKMASK.bREG.HSIOBUS = 0;
                } while (pMEMBUSCFG->MBUSSTS.bREG.HSIOBUS == 1);

                pPMU->PMU_SYSRST.bREG.HSB = 0;
                do {
                    pPMU->PWRDN_HSBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PD_HSB == 0);
            }
            else {
                do {
                    pPMU->PWRUP_HSBUS.bREG.DATA = 1;
                } while (pPMU->PMU_PWRSTS.bREG.PU_HSB == 0);
                pPMU->PMU_SYSRST.bREG.HSB = 1;

                do {
                    pMEMBUSCFG->HCLKMASK.bREG.HSIOBUS = 1;
                } while (pMEMBUSCFG->MBUSSTS.bREG.HSIOBUS == 0);
            }
            break;
        default:
            return -1;
    }

    return 0;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_getfbuspwdn( unsigned int fbusname)
* DESCRIPTION :
*   - fbusname : CLKCTRL(n) index
*   - return : 1:pwdn, 0:wkup
* ***************************************************************************************/
int tca_ckc_getfbuspwdn( unsigned int fbusname)
{
    int retVal = 0;

    switch (fbusname) {
        case FBUS_MEM:
            if (pPMU->PMU_PWRSTS.bREG.PU_MB)
                retVal = 1;
            break;
        case FBUS_DDI:
            if (pPMU->PMU_PWRSTS.bREG.PU_DB)
                retVal = 1;
            break;
        case FBUS_GPU:
            if (pPMU->PMU_PWRSTS.bREG.PU_GB)
                retVal = 1;
            break;
//        case FBUS_VCORE:
        case FBUS_VBUS:
            if (pPMU->PMU_PWRSTS.bREG.PU_VB)
                retVal = 1;
            break;
        case FBUS_HSIO:
            if (pPMU->PMU_PWRSTS.bREG.PU_HSB)
                retVal = 1;
            break;
        default :
            retVal = -1;
            break;
    }

     return retVal;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setswreset(unsigned int fbus_name, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setfbusswreset(unsigned int fbusname, unsigned int isreset)
{
    unsigned long old_value, ctrl_value;

    return 0;

    old_value = (0x1FF | ~(pCKC->SWRESET.nREG));

    switch (fbusname) {
        case FBUS_DDI:
            ctrl_value = 0x00000004;
            break;
        case FBUS_GPU:
            ctrl_value = 0x00000008;
            break;
        case FBUS_VBUS:
//        case FBUS_VCORE:  
            ctrl_value = 0x00000060;
            break;
        case FBUS_HSIO:
            ctrl_value = 0x00000080;
            break;
        default:
            return -1;
    }

    if (isreset)
        pCKC->SWRESET.nREG = old_value & ~ctrl_value;
    else
        pCKC->SWRESET.nREG = old_value | ctrl_value;

    return 0;
}

/****************************************************************************************
* FUNCTION :  int tca_ckc_setiopwdn(unsigned int sel, unsigned int ispwdn)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setiopwdn(unsigned int sel, unsigned int ispwdn)
{
    if (sel >= RB_MAX)
        return -1;

    if (pCKC->CLKCTRL4.bREG.EN == 0)
        return -2;

    if (sel >= 32) {
        if (ispwdn)
            pIOBUSCFG->HCLKEN1.nREG &= ~(0x1 << (sel-32));
        else
            pIOBUSCFG->HCLKEN1.nREG |= (0x1 << (sel-32));
    }
    else {
        if (ispwdn)
            pIOBUSCFG->HCLKEN0.nREG &= ~(0x1 << sel);
        else
            pIOBUSCFG->HCLKEN0.nREG |= (0x1 << sel);
    }

    return 0;
}

/****************************************************************************************
* FUNCTION :  int tca_ckc_getiobus(unsigned int sel)
* DESCRIPTION :
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_getiopwdn(unsigned int sel)
{
    int retVal = 0;

    if (sel >= RB_MAX)
        return -1;

    if (pCKC->CLKCTRL4.bREG.EN == 0)
        return -2;

    if (sel >= 32) {
        if (pIOBUSCFG->HCLKEN1.nREG & (0x1 << (sel-32)))
            retVal = 0;
        else
            retVal = 1;
    }
    else {
        if (pIOBUSCFG->HCLKEN0.nREG & (0x1 << sel))
            retVal = 0;
        else
            retVal = 1;
    }

    return retVal;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_setioswreset(unsigned int sel, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setioswreset(unsigned int sel, unsigned int isreset)
{
    if (sel >= RB_MAX)
        return -1;

    if (pCKC->CLKCTRL4.bREG.EN == 0)
        return -2;

    if (sel >= 32) {
        if (isreset)
            pIOBUSCFG->HRSTEN1.nREG &= ~(0x1 << (sel-32));
        else
            pIOBUSCFG->HRSTEN1.nREG |= (0x1 << (sel-32));
    }
    else {
        if (isreset)
            pIOBUSCFG->HRSTEN0.nREG &= ~(0x1 << sel);
        else
            pIOBUSCFG->HRSTEN0.nREG |= (0x1 << sel);
    }

    return 0;
}

/****************************************************************************************
* FUNCTION : void tca_ckc_setddipwdn(unsigned int sel , unsigned int ispwdn)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setddipwdn(unsigned int sel , unsigned int ispwdn)
{
    if (sel >= DDIBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL2.bREG.EN == 0)
        return -2;

    if (ispwdn)
        pDDIBUSCFG->PWDN.nREG &= ~(0x1 << sel);
    else
        pDDIBUSCFG->PWDN.nREG |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_getddipwdn(unsigned int sel)
* DESCRIPTION :
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_getddipwdn(unsigned int sel)
{
    int retVal = 0;

    if (sel >= DDIBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL2.bREG.EN == 0)
        return -2;

    if (pDDIBUSCFG->PWDN.nREG & (0x1 << sel))
        retVal = 0;
    else
        retVal = 1;

    return retVal;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_setddiswreset(unsigned int sel, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setddiswreset(unsigned int sel, unsigned int isreset)
{
    if (sel >= DDIBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL2.bREG.EN == 0)
        return -2;

    if (isreset)
        pDDIBUSCFG->SWRESET.nREG &= ~(0x1 << sel);
    else
        pDDIBUSCFG->SWRESET.nREG |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : void tca_ckc_setgpupwdn(unsigned int sel , unsigned int ispwdn)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setgpupwdn(unsigned int sel , unsigned int ispwdn)
{
    if (sel >= GPUBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL3.bREG.EN == 0)
        return -2;

    if (ispwdn)
        pGPUBUSCFG->PWDN.nREG &= ~(0x1 << sel);
    else
        pGPUBUSCFG->PWDN.nREG |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_getgpupwdn(unsigned int sel)
* DESCRIPTION :
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_getgpupwdn(unsigned int sel)
{
    int retVal = 0;

    if (sel >= GPUBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL3.bREG.EN == 0)
        return -2;

    if (pGPUBUSCFG->PWDN.nREG & (0x1 << sel))
        retVal = 0;
    else
        retVal = 1;

    return retVal;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_setgpuswreset(unsigned int sel, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setgpuswreset(unsigned int sel, unsigned int isreset)
{
    if (sel >= GPUBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL3.bREG.EN == 0)
        return -2;

    if (isreset)
        pGPUBUSCFG->SWRESET.nREG &= ~(0x1 << sel);
    else
        pGPUBUSCFG->SWRESET.nREG |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : void tca_ckc_setvideopwdn(unsigned int sel , unsigned int ispwdn)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setvideopwdn(unsigned int sel , unsigned int ispwdn)
{
    if (sel >= VIDEOBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL5.bREG.EN == 0)
        return -2;

    if (ispwdn)
        pVIDEOBUSCFG->PWDN.nREG &= ~(0x1 << sel);
    else
        pVIDEOBUSCFG->PWDN.nREG |= (0x1 << sel);
    
    return 0;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_getvideopwdn(unsigned int sel)
* DESCRIPTION :
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_getvideopwdn(unsigned int sel)
{
    int retVal = 0;

    if (sel >= VIDEOBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL5.bREG.EN == 0)
        return -2;

    if (pVIDEOBUSCFG->PWDN.nREG & (0x1 << sel))
        retVal = 0;
    else
        retVal = 1;

    return retVal;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_setvideoswreset(unsigned int sel, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setvideoswreset(unsigned int sel, unsigned int isreset)
{
    if (sel >= VIDEOBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL5.bREG.EN == 0)
        return -2;

    if (isreset)
        pVIDEOBUSCFG->SWRESET.nREG &= ~(0x1 << sel);
    else
        pVIDEOBUSCFG->SWRESET.nREG |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : void tca_ckc_sethsiopwdn(unsigned int sel , unsigned int ispwdn)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_sethsiopwdn(unsigned int sel , unsigned int ispwdn)
{
    if (sel >= HSIOBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL7.bREG.EN == 0)
        return -2;

    if (ispwdn)
        pHSIOBUSCFG->PWDN.nREG &= ~(0x1 << sel);
    else
        pHSIOBUSCFG->PWDN.nREG |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_gethsiopwdn(unsigned int sel)
* DESCRIPTION : Power Down Register of DDI_CONFIG
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_gethsiopwdn(unsigned int sel)
{
    int retVal = 0;

    if (sel >= HSIOBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL7.bREG.EN == 0)
        return -2;

    if (pHSIOBUSCFG->PWDN.nREG & (0x1 << sel))
        retVal = 0;
    else
        retVal = 1;

    return retVal;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_sethsioswreset(unsigned int sel, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_sethsioswreset(unsigned int sel, unsigned int isreset)
{
    if (sel >= HSIOBUS_MAX)
        return -1;

    if (pCKC->CLKCTRL7.bREG.EN == 0)
        return -2;

    if (isreset)
        pHSIOBUSCFG->SWRESET.nREG &= ~(0x1 << sel);
    else
        pHSIOBUSCFG->SWRESET.nREG |= (0x1 << sel);

    return 0;
}

int tca_ckc_fclk_enable(unsigned int fclk, unsigned int enable)
{
    volatile CLKCTRL_TYPE *pCLKCTRL;
    pCLKCTRL = (volatile CLKCTRL_TYPE *)((&pCKC->CLKCTRL0)+fclk);

    if (enable)
        pCLKCTRL->bREG.EN = 1;
    else
        pCLKCTRL->bREG.EN = 0;
    while(pCLKCTRL->bREG.CFGREQ);

    return 0;
}

int tca_ckc_pclk_enable(unsigned int pclk, unsigned int enable)
{
    volatile PCLK_XXX_TYPE *pPERI;
    pPERI = (volatile PCLK_XXX_TYPE *)((&pCKC->PCLKCTRL00)+pclk);

    if (pclk == PERI_HDMI) {
        if (enable)
            stHDMIReg.bREG.EN = 1;
        else
            stHDMIReg.bREG.EN = 0;
        pPERI->nREG = stHDMIReg.nREG;
    }
    else if (pclk == PERI_HDMIA) {
        if (enable)
            stHDMIAudReg.bREG.EN = 1;
        else
            stHDMIAudReg.bREG.EN = 0;
        pPERI->nREG = stHDMIAudReg.nREG;
    }
    else {
        if (enable)
            pPERI->bREG.EN = 1;
        else
            pPERI->bREG.EN = 0;
    }
    return 0;
}

int tca_ckc_hdmipclk_enable(unsigned int pclk, unsigned int enable)
{
    volatile PCLK_XXX_TYPE *pPERI;
    pPERI = (volatile PCLK_XXX_TYPE *)((&pCKC->PCLKCTRL00)+pclk);

    if ((pclk != PERI_LCD0) && (pclk != PERI_LCD1))
        return -1;

    if (enable) {
        hdml_lcdc_flag = 1;
        hdmi_lcdc_source = pclk;
        pPERI->bREG.SEL = PCDIVIDXTIN_HDMIPCLK;
        pPERI->bREG.DIV = 0;
    }
    else {
        hdml_lcdc_flag = 0;
        pPERI->bREG.SEL = PCDIRECTXIN;
        pPERI->bREG.DIV = 11;
    }

    return 0;
}

/****************************************************************************************
* EXPORT_SYMBOL clock functions for Linux
* ***************************************************************************************/
#if defined(_LINUX_)

EXPORT_SYMBOL(tca_ckc_init);
EXPORT_SYMBOL(tca_ckc_setpll);
EXPORT_SYMBOL(tca_ckc_getpll);
EXPORT_SYMBOL(tca_ckc_getdividpll);
EXPORT_SYMBOL(tca_ckc_setfbusctrl);
EXPORT_SYMBOL(tca_ckc_getfbusctrl);
EXPORT_SYMBOL(tca_ckc_setperi);
EXPORT_SYMBOL(tca_ckc_getperi);
EXPORT_SYMBOL(tca_ckc_setippwdn);
EXPORT_SYMBOL(tca_ckc_setfbuspwdn);
EXPORT_SYMBOL(tca_ckc_getfbuspwdn);
EXPORT_SYMBOL(tca_ckc_setfbusswreset);
EXPORT_SYMBOL(tca_ckc_setiopwdn);
EXPORT_SYMBOL(tca_ckc_getiopwdn);
EXPORT_SYMBOL(tca_ckc_setioswreset);
EXPORT_SYMBOL(tca_ckc_setddipwdn);
EXPORT_SYMBOL(tca_ckc_getddipwdn);
EXPORT_SYMBOL(tca_ckc_setddiswreset);
EXPORT_SYMBOL(tca_ckc_setgpupwdn);
EXPORT_SYMBOL(tca_ckc_getgpupwdn);
EXPORT_SYMBOL(tca_ckc_setgpuswreset);
EXPORT_SYMBOL(tca_ckc_setvideopwdn);
EXPORT_SYMBOL(tca_ckc_getvideopwdn);
EXPORT_SYMBOL(tca_ckc_setvideoswreset);
EXPORT_SYMBOL(tca_ckc_sethsiopwdn);
EXPORT_SYMBOL(tca_ckc_gethsiopwdn);
EXPORT_SYMBOL(tca_ckc_sethsioswreset);
EXPORT_SYMBOL(tca_ckc_fclk_enable);
EXPORT_SYMBOL(tca_ckc_pclk_enable);
EXPORT_SYMBOL(tca_ckc_hdmipclk_enable);

#endif

/* end of file */
