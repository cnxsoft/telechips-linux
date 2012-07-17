/****************************************************************************
 *     FileName     : tca_ckc.c
 *     Description :
 ****************************************************************************
 *
 *     TCC Version 1.0
 *     Copyright (c) Telechips, Inc.
 *     ALL RIGHTS RESERVED
 *
 ****************************************************************************/


#include <mach/bsp.h>
#include <asm/io.h>
#include <linux/mm.h>    // for PAGE_ALIGN
#include <linux/kernel.h>
#include <linux/module.h>

#define CPU_SRC_CH      5
#define CPU_SRC_PLL     DIRECTPLL5

#define MEM_SRC_CH      4
#define MEM_SRC_PLL     DIRECTPLL4

#define MAX_PERI_DIV    4096
#define MAX_FBUS_DIV    16

#define MAX_TCC_PLL     6
#define MAX_CLK_SRC     (MAX_TCC_PLL*2 + 1)     // XIN


typedef struct {
    unsigned int        uFpll;
    unsigned int        P;
    unsigned int        M;
    unsigned int        S;
    unsigned int        VSEL;
} sfPLL;

#define PLLFREQ(P, M, S)        (( 120000 * (M) )  / (P) ) >> (S) // 100Hz Unit..
#define FPLL_t(P, M, S, VSEL)   PLLFREQ(P,M,S), P, M, S, VSEL

static sfPLL pIO_CKC_PLL045[] = {
    {FPLL_t(3, 279,  0,  0)},       // 1116 MHz
    {FPLL_t(3, 277,  0,  0)},       // 1108 MHz
    {FPLL_t(3, 275,  0,  0)},       // 1100 MHz
    {FPLL_t(3, 273,  0,  0)},       // 1092 MHz
    {FPLL_t(3, 271,  0,  0)},       // 1084 MHz
    {FPLL_t(3, 269,  0,  0)},       // 1076 MHz
    {FPLL_t(3, 267,  0,  0)},       // 1068 MHz
    {FPLL_t(3, 265,  0,  0)},       // 1060 MHz
    {FPLL_t(3, 263,  0,  0)},       // 1052 MHz
    {FPLL_t(3, 261,  0,  0)},       // 1044 MHz
    {FPLL_t(3, 259,  0,  0)},       // 1036 MHz
    {FPLL_t(3, 257,  0,  0)},       // 1028 MHz
    {FPLL_t(3, 255,  0,  0)},       // 1020 MHz
    {FPLL_t(3, 253,  0,  0)},       // 1012 MHz
    {FPLL_t(3, 251,  0,  0)},       // 1004 MHz
    {FPLL_t(3, 498,  1,  1)},       // 996 MHz
    {FPLL_t(3, 490,  1,  1)},       // 980 MHz
    {FPLL_t(3, 470,  1,  1)},       // 940 MHz
    {FPLL_t(3, 450,  1,  1)},       // 900 MHz
    {FPLL_t(3, 440,  1,  1)},       // 880 MHz
    {FPLL_t(3, 430,  1,  1)},       // 860 MHz
    {FPLL_t(3, 420,  1,  1)},       // 840 MHz
    {FPLL_t(3, 410,  1,  1)},       // 820 MHz
    {FPLL_t(3, 400,  1,  1)},       // 800 MHz
    {FPLL_t(3, 384,  1,  1)},       // 768 MHz
    {FPLL_t(3, 380,  1,  1)},       // 760 MHz
    {FPLL_t(3, 360,  1,  1)},       // 720 MHz
    {FPLL_t(3, 348,  1,  0)},       // 696 MHz
    {FPLL_t(3, 336,  1,  0)},       // 672 MHz
    {FPLL_t(3, 330,  1,  0)},       // 660 MHz
    {FPLL_t(3, 324,  1,  0)},       // 648 MHz
    {FPLL_t(3, 312,  1,  0)},       // 624 MHz
    {FPLL_t(3, 300,  1,  0)},       // 600 MHz
    {FPLL_t(3, 297,  1,  0)},       // 594 MHz
    {FPLL_t(3, 270,  1,  0)},       // 540 MHz
    {FPLL_t(3, 264,  1,  0)},       // 528 MHz
    {FPLL_t(3, 500,  2,  1)},       // 500 MHz
    {FPLL_t(3, 486,  2,  1)},       // 486 MHz
    {FPLL_t(3, 480,  2,  1)},       // 480 MHz
    {FPLL_t(3, 468,  2,  1)},       // 468 MHz
    {FPLL_t(3, 432,  2,  1)},       // 432 MHz
    {FPLL_t(3, 380,  2,  1)},       // 380 MHz
    {FPLL_t(3, 324,  2,  0)},       // 324 MHz
    {FPLL_t(3, 312,  2,  0)},       // 312 MHz
    {FPLL_t(3, 290,  2,  0)},       // 290 MHz
    {FPLL_t(3, 260,  2,  0)},       // 260 MHz
    {FPLL_t(3, 460,  3,  1)},       // 230 MHz
    {FPLL_t(3, 444,  3,  1)},       // 222 MHz
    {FPLL_t(3, 432,  3,  2)},       // 216 MHz
    {FPLL_t(3, 408,  3,  1)},       // 204MHz
    {FPLL_t(3, 384,  3,  1)},       // 192 MHz
    {FPLL_t(3, 380,  3,  1)},       // 190 MHz
    {FPLL_t(3, 360,  3,  1)},       // 180 MHz
    {FPLL_t(3, 292,  3,  0)},       // 146MHz
    {FPLL_t(3, 264,  3,  0)},       // 132 MHz
    {FPLL_t(3, 280,  4,  0)},       // 70MHz
};

static sfPLL pIO_CKC_PLL123[] = {
    {FPLL_t(3, 168,  0,  1)},       // 672 MHz
    {FPLL_t(3, 165,  0,  1)},       // 660 MHz
    {FPLL_t(3, 156,  0,  1)},       // 624 MHz
    {FPLL_t(3, 150,  0,  1)},       // 600 MHz
    {FPLL_t(3, 145,  0,  1)},       // 580 MHz
    {FPLL_t(3, 135,  0,  1)},       // 540 MHz
    {FPLL_t(3, 132,  0,  1)},       // 528 MHz
    {FPLL_t(3, 125,  0,  1)},       // 500 MHz
    {FPLL_t(3, 120,  0,  1)},       // 480 MHz
    {FPLL_t(3, 117,  0,  1)},       // 468 MHz
    {FPLL_t(3, 108,  0,  0)},       // 432 MHz
    {FPLL_t(3, 95,   0,  0)},       // 380 MHz
    {FPLL_t(3, 156,  1,  1)},       // 312 MHz
    {FPLL_t(3, 145,  1,  1)},       // 290 MHz
    {FPLL_t(3, 130,  1,  1)},       // 260 MHz
    {FPLL_t(3, 115,  1,  0)},       // 230 MHz
    {FPLL_t(3, 102,  1,  0)},       // 204MHz
    {FPLL_t(3, 146,  2,  1)},       // 146MHz
    {FPLL_t(3, 140,  3,  1)},       // 70MHz
};

#define NUM_PLL045              (sizeof(pIO_CKC_PLL045)/sizeof(sfPLL))
#define NUM_PLL123              (sizeof(pIO_CKC_PLL123)/sizeof(sfPLL))

#define tca_wait()              { volatile int i; for (i=0; i<0x2000; i++); }

#define TCC_PLL_LOCK_USE
#if defined(TCC_PLL_LOCK_USE)
#define tca_pll_lock_wait(addr) { volatile unsigned int i; \
                                  for (i=100; i ; i--) \
                                      while((*addr & 0x00200000) == 0); \
                                  for (i=0x200; i ; i--); \
                                }
#endif


/****************************************************************************************
* External Functions
* ***************************************************************************************/
extern void tcc_ddr_set_clock(unsigned int freq);


/****************************************************************************************
* Local Variable
* ***************************************************************************************/
static volatile PCKC            pCKC ;
static volatile PPMU            pPMU ;
static volatile PIOBUSCFG       pIOBUSCFG;
static volatile PDDICONFIG      pDDIBUSCFG;
static volatile PGRPBUSCONFIG   pGPUBUSCFG;
static volatile PVIDEOBUSCFG    pVIDEOBUSCFG;
static volatile PHSIOBUSCFG     pHSIOBUSCFG;
static volatile PCAMBUSCFG      pCAMBUSCFG;

static unsigned int             stClockSource[MAX_CLK_SRC];
static int                      initialized = 0;


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
    pGPUBUSCFG = (GRPBUSCONFIG *)io_p2v(HwGRPBUSCONFIG_BASE);
    pVIDEOBUSCFG = (VIDEOBUSCFG *)io_p2v(HwVIDEOBUSCFG_BASE);
    pHSIOBUSCFG = (HSIOBUSCFG *)io_p2v(HwHSIOBUSCFG_BASE);
    pCAMBUSCFG = (CAMBUSCFG *)io_p2v(HwCAMBUSCFG_BASE);

#if 0
    /* IOBUS AHB2AXI: flushs prefetch buffer when bus state is IDLE or WRITE
       enable:  A2XMOD1 (Audio DMA, GPSB, DMA2/3, EHI1)
       disable: A2XMOD0 (USB1.1Host, USB OTG, SD/MMC, IDE, DMA0/1, MPEFEC, EHI0)
    */
    pIOBUSCFG->IO_A2X = 0x11;

    //Mali package port init : PMU Config0 register(5)
    //Video codec size limit option off : PMU Config0 register(4)
    pPMU->CONFIG0 |= Hw5|Hw4;

    //CLKDIVC0/1
    pCKC->CLKDIVC0    = 0x01010101;
    pCKC->CLKDIVC1    = 0x01010101;
#endif

    if (initialized)
        return;

    for (i=0 ; i<MAX_TCC_PLL ; i++) {
        if (i == CPU_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("    pll_%d:  cpu clcok source\n", i);
        }
        else if (i == MEM_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("    pll_%d:  memory clcok source\n", i);
        }
        else {
            stClockSource[i] = tca_ckc_getpll(i);
            pr_info("    pll_%d:  %d kHz (Fixed)\n", i, stClockSource[i]/10);
        }
    }
    for ( ; i<(MAX_TCC_PLL*2) ; i++) {
        if ((i-MAX_TCC_PLL) == CPU_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("div_pll_%d:  cpu clcok source\n", i-MAX_TCC_PLL);
        }
        else if ((i-MAX_TCC_PLL) == MEM_SRC_CH) {
            stClockSource[i] = 0;
            pr_info("div_pll_%d:  memory clcok source\n", i-MAX_TCC_PLL);
        }
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
}

/****************************************************************************************
* FUNCTION :int tca_ckc_setpll(unsigned int pll, unsigned int ch)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setpll(unsigned int pll, unsigned int ch)
{
    unsigned    uCnt;
    sfPLL       *pPLL;
    volatile unsigned int *pPLLCFG;

    if (ch >= MAX_TCC_PLL)
        return -1;

    if (ch == MEM_SRC_CH) {
        stClockSource[ch] = 0;
        stClockSource[MAX_TCC_PLL+ch] = 0;
        return -1;
    }

    if (ch >= 4)
        pPLLCFG = (unsigned int *)((&pCKC->PLL4CFG)+(ch-4));
    else
        pPLLCFG = (unsigned int *)((&pCKC->PLL0CFG)+ch);

    if(pll != 0) {
        if(ch == 0 || ch == 4 || ch == 5) { // High Frequency PLL
            pPLL = &pIO_CKC_PLL045[0];
            for (uCnt = 0; uCnt < NUM_PLL045; uCnt ++, pPLL++) {
                if (pPLL->uFpll <= pll)
                    break;
            }

            if (uCnt >= NUM_PLL045) {
                uCnt = NUM_PLL045 - 1;
                pPLL = &pIO_CKC_PLL045[uCnt];
            }
        }
        else {  // Low Frequency PLL
            pPLL = &pIO_CKC_PLL123[0];
            for (uCnt = 0; uCnt < NUM_PLL123; uCnt ++, pPLL++) {
                if (pPLL->uFpll <= pll)
                    break;
            }

            if (uCnt >= NUM_PLL123) {
                uCnt = NUM_PLL123 - 1;
                pPLL = &pIO_CKC_PLL123[uCnt];
            }
        }

#if defined(TCC_PLL_LOCK_USE)
        *pPLLCFG = ((pPLL->VSEL&0x1) << 30) | ((pPLL->S&0x7) << 24) | (1 << 20) | ((pPLL->M&0x3FF) << 8) | (pPLL->P&0x3F);
        *pPLLCFG |= (1 << 31);
        tca_pll_lock_wait(pPLLCFG);
#else
        *pPLLCFG = ((pPLL->VSEL&0x1) << 30) | ((pPLL->S&0x7) << 24) | ((pPLL->M&0x3FF) << 8) | (pPLL->P&0x3F);
        *pPLLCFG |= (1 << 31);
        tca_wait();
#endif

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
        //Disable PLL
        *pPLLCFG &= ~(1 << 31);
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
    volatile unsigned int   tPLLCFG;
    unsigned int            tPLL;
    unsigned int            tPCO;

    switch(ch) {
        case 0:
            tPLLCFG = pCKC->PLL0CFG;
            break;
        case 1:
            tPLLCFG = pCKC->PLL1CFG;
            break;
        case 2:
            tPLLCFG = pCKC->PLL2CFG;
            break;
        case 3:
            tPLLCFG = pCKC->PLL3CFG;
            break;
        case 4:
            tPLLCFG = pCKC->PLL4CFG;
            break;
        case 5:
            tPLLCFG = pCKC->PLL5CFG;
            break;
        default:
            return 0;
    }

    if ((tPLLCFG & (1<<31)) == 0)
        return 0;

    tPCO = (120000 * ((tPLLCFG & 0xFFF00) >> 8)) / (tPLLCFG & 0x0003F);
    tPLL= ((tPCO) >> ((tPLLCFG & 0x7000000) >> 24));

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

    if (ch >= MAX_TCC_PLL)
        return 0;

    switch(ch)
    {
        case 0:
            if ((pCKC->CLKDIVC0 & Hw31) == 0)
                return 0;
            uiPDIV = (pCKC->CLKDIVC0 & (Hw30-Hw24))>>24;
            break;
        case 1:
            if ((pCKC->CLKDIVC0 & Hw23) == 0)
                return 0;
            uiPDIV = (pCKC->CLKDIVC0 & (Hw22-Hw16))>>16;
            break;
        case 2:
            if ((pCKC->CLKDIVC0 & Hw15) == 0)
                return 0;
            uiPDIV = (pCKC->CLKDIVC0 & (Hw14-Hw8))>>8;
            break;
        case 3:
            if ((pCKC->CLKDIVC0 & Hw7) == 0)
                return 0;
            uiPDIV = (pCKC->CLKDIVC0 & (Hw6-Hw0));
            break;
        case 4:
            if ((pCKC->CLKDIVC1 & Hw31) == 0)
                return 0;
            uiPDIV = (pCKC->CLKDIVC1 & (Hw30-Hw24))>>24;
            break;
        case 5:
            if ((pCKC->CLKDIVC1 & Hw23) == 0)
                return 0;
            uiPDIV = (pCKC->CLKDIVC1 & (Hw22-Hw16))>>16;
            break;
    }

    //Fdivpll Clock
    tDIVPLL = (unsigned int)tPLL/(uiPDIV+1);

    return tDIVPLL;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setcpu(unsigned int freq)
* DESCRIPTION :
* ***************************************************************************************/
static unsigned int tca_ckc_setcpu(unsigned int freq)
{
    unsigned uCnt;
    sfPLL    *pPLL;

    // 1. temporally change the cpu clock source.(XIN)
    pCKC->CLK0CTRL = (pCKC->CLK0CTRL & 0xFFF00000) | 0xFFFF4;
//    pCKC->CLK7CTRL = (pCKC->CLK7CTRL & 0xFFF00000) | 0x00014;

    tca_wait();

    // 2. change pll(for cpu) clock.
#if (0)
    tca_ckc_setpll(freq, CPU_SRC_CH);
#else
    pPLL = &pIO_CKC_PLL045[0];
    for (uCnt = 0; uCnt < NUM_PLL045; uCnt ++, pPLL++) {
        if (pPLL->uFpll <= freq)
            break;
    }

    if (uCnt >= NUM_PLL045) {
        uCnt = NUM_PLL045 - 1;
        pPLL = &pIO_CKC_PLL045[uCnt];
    }

#if defined(TCC_PLL_LOCK_USE)
    pCKC->PLL5CFG = ((pPLL->VSEL&0x1) << 30) | ((pPLL->S&0x7) << 24) | (1 << 20) | ((pPLL->M&0x3FF) << 8) | (pPLL->P&0x3F);
    pCKC->PLL5CFG |= (1 << 31);
    tca_pll_lock_wait(&(pCKC->PLL5CFG));
#else
    pCKC->PLL5CFG = ((pPLL->VSEL&0x1) << 30) | ((pPLL->S&0x7) << 24) | ((pPLL->M&0x3FF) << 8) | (pPLL->P&0x3F);
    pCKC->PLL5CFG |= (1 << 31);
    tca_wait();
#endif
#endif

    // 3. change the cpu clock source.
    pCKC->CLK0CTRL = (pCKC->CLK0CTRL & 0xFFF00000) | 0xFFFF0 | CPU_SRC_PLL;
//    pCKC->CLK7CTRL = (pCKC->CLK7CTRL & 0xFFF00000) | 0x00030 | CPU_SRC_PLL;

    tca_wait();

    return tca_ckc_getpll(CPU_SRC_CH);
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_getcpu(void)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_getcpu(void)
{
    unsigned int lcpu = 0;
    unsigned int lconfig = 0;
    unsigned int lcnt = 0;
    unsigned int li = 0;
    unsigned int lclksource = 0;

    lconfig = ((pCKC->CLK0CTRL & (Hw20-Hw4))>>4);

    for(li = 0; li < 16; li++)
    {
        if((lconfig & Hw0) == 1)
            lcnt++;
        lconfig = (lconfig >> 1);
    }

    switch(pCKC->CLK0CTRL & (Hw4-Hw0)) // Check CPU Source
    {
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
        case DIRECTXTIN:
            lclksource =  120000;
            break;
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
            lclksource =  tca_ckc_getpll(1);
            break;
    }

    lcpu = (lclksource * lcnt)/16;

    return lcpu;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setfbusctrl(unsigned int clkname,unsigned int isenable,unsigned int freq)
* DESCRIPTION :
*   return: real clock rate. ( unit: 100 Hz)
* ***************************************************************************************/
unsigned int tca_ckc_setfbusctrl(unsigned int clkname,unsigned int isenable,unsigned int freq)
{
    volatile unsigned int *pCLKCTRL;
    unsigned int div[MAX_CLK_SRC], div_100[MAX_CLK_SRC], i, clksrc, searchsrc, overclksrc;
    unsigned int clkrate=0, clkdiv=0;

    if (clkname >= FBUS_MAX)
        return 0;

    if (clkname >= FBUS_HSIO)
        pCLKCTRL = (unsigned int *)((&pCKC->CLK8CTRL)+(clkname-FBUS_HSIO));
    else
        pCLKCTRL = (unsigned int *)((&pCKC->CLK0CTRL)+clkname);

    searchsrc = 0xFFFFFFFF;
    overclksrc = 0xFFFFFFFF;

    if (clkname == FBUS_CPU)
        return tca_ckc_setcpu(freq);
    else if (clkname == FBUS_MEM) {
        tcc_ddr_set_clock(freq/10);
        return freq;
    }

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
    *pCLKCTRL = (*pCLKCTRL & (1<<21)) | ((clkdiv&0xF) << 4) | (clksrc&0xF);
    if(isenable == ENABLE)
        *pCLKCTRL |= 1<<21;
    else if (isenable == DISABLE)
        *pCLKCTRL &= ~(1<<21);
#else
    *pCLKCTRL = (*pCLKCTRL&0xFFF0000F) | 0xF0;
    *pCLKCTRL = (*pCLKCTRL&0xFFFFFFF0) | (clksrc&0xF);
    while(*pCLKCTRL & Hw31);    // CHGREQ
    *pCLKCTRL = (*pCLKCTRL&0xFFFFFF0F) | ((clkdiv&0xF)<<4);
    while(*pCLKCTRL & Hw29);
    if(isenable == ENABLE)
        *pCLKCTRL |= Hw21;
    else if (isenable == DISABLE)
        *pCLKCTRL &= ~Hw21;
    while(*pCLKCTRL & Hw29);
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
    volatile unsigned int *pCLKCTRL;
    unsigned int clksource = 0;

    if (clkname >= FBUS_MAX)
        return 0;

    if (clkname >= FBUS_HSIO)
        pCLKCTRL = (unsigned int *)((&pCKC->CLK8CTRL)+(clkname-FBUS_HSIO));
    else
        pCLKCTRL = (unsigned int *)((&pCKC->CLK0CTRL)+clkname);

    if ((*pCLKCTRL & (1<<21)) == 0)
        return 0;

    if(clkname == FBUS_CPU)
        return tca_ckc_getcpu();

    switch(*pCLKCTRL & 0xF) {
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

    return (clksource / (((*pCLKCTRL >> 4) & 0xF) + 1));
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setperi(unsigned int periname,unsigned int isenable, unsigned int freq)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_setperi(unsigned int periname,unsigned int isenable, unsigned int freq)
{
    volatile unsigned int *pPCLKCTRL = (unsigned int *)((&pCKC->PCLK_TCX)+periname);
    unsigned int div[MAX_CLK_SRC], div_100[MAX_CLK_SRC], i, searchsrc, overclksrc, dco_shift;
    unsigned int clkmd, clksrc, clkdiv, clkrate;

    searchsrc = 0xFFFFFFFF;
    overclksrc = 0xFFFFFFFF;

    /* PCK_YYY (n=0~55, n== 7,27,28,31,36,37,38) */
    if (periname == PERI_HDMIA || periname == PERI_ADC || periname == PERI_SPDIF || periname == PERI_AUD ||
        periname == PERI_DAI0 || periname == PERI_DAI1 || periname == PERI_DAI2)
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

        *pPCLKCTRL = ((clkmd&0x1)<<31) | ((clksrc&0x1F)<<24) | (clkdiv&0xFFFF);
        if (isenable)
            *pPCLKCTRL |= (1<<29);
    }
    else {
        if (periname == PERI_I2C0 || periname == PERI_I2C1) {
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

        *pPCLKCTRL = ((clksrc&0x1F)<<24) | (clkdiv&0xFFF);
        if (isenable)
            *pPCLKCTRL |= (1<<29);
    }

    return clkrate;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_getperi(unsigned int periname)
* DESCRIPTION :
* ***************************************************************************************/
unsigned int tca_ckc_getperi(unsigned int periname)
{
    volatile unsigned int *pPCLKCTRL = (unsigned int *)((&pCKC->PCLK_TCX)+periname);
    unsigned int lclksource = 0;

    if ((*pPCLKCTRL & (1<<29)) == 0)
        return 0;

    switch((*pPCLKCTRL >> 24) & 0x1F) {
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

    if (periname == PERI_HDMIA || periname == PERI_ADC || periname == PERI_SPDIF || periname == PERI_AUD ||
        periname == PERI_DAI0 || periname == PERI_DAI1 || periname == PERI_DAI2)
    {
        if (*pPCLKCTRL & (1<<31)) {
            return (lclksource/((*pPCLKCTRL&0xFFFF)+1));
        }
        else {
            if ((*pPCLKCTRL&0xFFFF) > 32768)
                return ((lclksource * (65536 - (*pPCLKCTRL&0xFFFF))) / 65536);
            else
                return ((lclksource * (*pPCLKCTRL&0xFFFF)) / 65536);
        }
    }
    else {
        return (lclksource/((*pPCLKCTRL&0xFFF)+1));
    }

    return 0;
}

/****************************************************************************************
* FUNCTION :void tca_ckc_setpmupwroff( unsigned int periname , unsigned int isenable)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setpmupwroff( unsigned int periname , unsigned int isenable)
{
    unsigned int retVal = 0;
     retVal = Hw0<<periname;
     if(isenable)
        pPMU->PWROFF |= (retVal);
    else
        pPMU->PWROFF &= ~(retVal);
}
/****************************************************************************************
* FUNCTION :void tca_ckc_getpmupwroff( unsigned int pmuoffname)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_getpmupwroff( unsigned int pmuoffname)
{
    unsigned int retVal = 0;
     retVal =  (pPMU->PWROFF >> pmuoffname)  & Hw0;
     return retVal;
}

/****************************************************************************************
* FUNCTION :int tca_ckc_setfbusswreset(unsigned int fbusname, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setfbusswreset(unsigned int fbusname, unsigned int isreset)
{
    if (fbusname >= FBUS_MAX)
        return -1;

    if(!isreset)
        pCKC->SWRESET = ~(pCKC->SWRESET) | (0x1<<fbusname);
    else
        pCKC->SWRESET = ~(pCKC->SWRESET) & ~(0x1<<fbusname);

    return 0;
}

/****************************************************************************************
* FUNCTION :  int tca_ckc_setiopwdn(unsigned int sel, unsigned int ispwdn)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setiopwdn(unsigned int sel, unsigned int ispwdn)
{
    if(sel >= RB_MAX)
        return -1;

    if ((pCKC->CLK4CTRL & Hw21) == 0)
        return -2;

    if(sel >= 32) {
        if(ispwdn)
            pIOBUSCFG->HCLKEN1 &= ~(0x1<<(sel-32));
        else
            pIOBUSCFG->HCLKEN1 |= (0x1<<(sel-32));
    }
    else {
        if(ispwdn)
            pIOBUSCFG->HCLKEN0 &= ~(0x1<<sel);
        else
            pIOBUSCFG->HCLKEN0 |= (0x1<<sel);
    }

    return 0;
}

/****************************************************************************************
* FUNCTION :  int tca_ckc_getiopwdn(unsigned int sel)
* DESCRIPTION :
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_getiopwdn(unsigned int sel)
{
    int retVal = 0;

    if(sel >= RB_MAX)
        return -1;

    if ((pCKC->CLK4CTRL & Hw21) == 0)
        return -2;

    if(sel >= 32) {
        retVal = (pIOBUSCFG->HCLKEN1  & (0x1<<(sel-32))) ;
    }
    else {
        retVal = (pIOBUSCFG->HCLKEN0  & (0x1<<sel)) ;
    }
    if(retVal)
        retVal = 0;
    else
        retVal = 1;

    return retVal;
}

/****************************************************************************************
* FUNCTION :int tca_ckc_setioswreset(unsigned int sel, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setioswreset(unsigned int sel, unsigned int isreset)
{
    if(sel >= RB_MAX)
        return -1;

    if ((pCKC->CLK4CTRL & Hw21) == 0)
        return -2;

    if(sel >= 32) {
        if(isreset)
            pIOBUSCFG->HRSTEN1 &= ~(0x1<<(sel-32));
        else
            pIOBUSCFG->HRSTEN1 |= (0x1<<(sel-32));
    }
    else {
        if(isreset)
            pIOBUSCFG->HRSTEN0 &= ~(0x1<<sel);
        else
            pIOBUSCFG->HRSTEN0 |= (0x1<<sel);
    }

    return 0;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_setddipwdn(unsigned int sel, unsigned int ispwdn)
* DESCRIPTION : Power Down Register of DDI_CONFIG
* ***************************************************************************************/
int tca_ckc_setddipwdn(unsigned int sel, unsigned int ispwdn)
{
    if(sel >= DDIBUS_MAX)
        return -1;

    if ((pCKC->CLK1CTRL & Hw21) == 0)
        return -2;
    
    if(ispwdn) // Power Down
        pDDIBUSCFG->PWDN |= (0x1 << sel);
    else // Normal
        pDDIBUSCFG->PWDN &= ~(0x1 << sel);

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

    if(sel >= DDIBUS_MAX)
        return -1;

    if ((pCKC->CLK1CTRL & Hw21) == 0)
        return -2;
    
    if (pDDIBUSCFG->PWDN & (0x1 << sel))
        retVal = 1;
    else // Normal
        retVal = 0;

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

    if ((pCKC->CLK1CTRL & Hw21) == 0)
        return -2;

    if (isreset)
        pDDIBUSCFG->SWRESET |= (0x1 << sel);
    else
        pDDIBUSCFG->SWRESET &= ~(0x1 << sel);

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

    if ((pCKC->CLK3CTRL & Hw21) == 0)
        return -2;

    if (ispwdn)
        pGPUBUSCFG->GRPBUS_PWRDOWN |= (0x1 << sel);
    else
        pGPUBUSCFG->GRPBUS_PWRDOWN &= ~(0x1 << sel);

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

    if ((pCKC->CLK3CTRL & Hw21) == 0)
        return -2;

    if (pGPUBUSCFG->GRPBUS_PWRDOWN & (0x1 << sel))
        retVal = 1;
    else
        retVal = 0;

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

    if ((pCKC->CLK3CTRL & Hw21) == 0)
        return -2;

    if (isreset)
        pGPUBUSCFG->GRPBUS_SWRESET |= (0x1 << sel);
    else
        pGPUBUSCFG->GRPBUS_SWRESET &= ~(0x1 << sel);

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

    if ((pCKC->CLK5CTRL & Hw21) == 0)
        return -2;

    if (ispwdn)
        pVIDEOBUSCFG->PWDN |= (0x1 << sel);
    else
        pVIDEOBUSCFG->PWDN &= ~(0x1 << sel);
    
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

    if ((pCKC->CLK5CTRL & Hw21) == 0)
        return -2;

    if (pVIDEOBUSCFG->PWDN & (0x1 << sel))
        retVal = 1;
    else
        retVal = 0;

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

    if ((pCKC->CLK5CTRL & Hw21) == 0)
        return -2;

    if (isreset)
        pVIDEOBUSCFG->SWRESET |= (0x1 << sel);
    else
        pVIDEOBUSCFG->SWRESET &= ~(0x1 << sel);

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

    if ((pCKC->CLK8CTRL & Hw21) == 0)
        return -2;

    if (ispwdn)
        pHSIOBUSCFG->HCLKMASK &= ~(0x1 << sel);
    else
        pHSIOBUSCFG->HCLKMASK |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_gethsiopwdn(unsigned int sel)
* DESCRIPTION :
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_gethsiopwdn(unsigned int sel)
{
    int retVal = 0;

    if (sel >= HSIOBUS_MAX)
        return -1;

    if ((pCKC->CLK8CTRL & Hw21) == 0)
        return -2;

    if (pHSIOBUSCFG->HCLKMASK & (0x1 << sel))
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

    if ((pCKC->CLK8CTRL & Hw21) == 0)
        return -2;

    if (isreset)
        pHSIOBUSCFG->SWRESET &= ~(0x1 << sel);
    else
        pHSIOBUSCFG->SWRESET |= (0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : void tca_ckc_setcampwdn(unsigned int sel , unsigned int ispwdn)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setcampwdn(unsigned int sel , unsigned int ispwdn)
{
    if (sel >= CAMBUS_MAX)
        return -1;

    if ((pCKC->CLK9CTRL & Hw21) == 0)
        return -2;

    if (ispwdn)
        pCAMBUSCFG->HCLKMASK |= (0x1 << sel);
    else
        pCAMBUSCFG->HCLKMASK &= ~(0x1 << sel);

    return 0;
}

/****************************************************************************************
* FUNCTION : int tca_ckc_getcampwdn(unsigned int sel)
* DESCRIPTION :
*   - return : (1:pwdn, 0:wkup)
* ***************************************************************************************/
int tca_ckc_getcampwdn(unsigned int sel)
{
    int retVal = 0;

    if (sel >= HSIOBUS_MAX)
        return -1;

    if ((pCKC->CLK8CTRL & Hw21) == 0)
        return -2;

    if (pCAMBUSCFG->HCLKMASK & (0x1 << sel))
        retVal = 1;
    else
        retVal = 0;

    return retVal;
}

/****************************************************************************************
* FUNCTION :unsigned int tca_ckc_sethsioswreset(unsigned int sel, unsigned int isreset)
* DESCRIPTION :
* ***************************************************************************************/
int tca_ckc_setcamswreset(unsigned int sel, unsigned int isreset)
{
    if (sel >= HSIOBUS_MAX)
        return -1;

    if ((pCKC->CLK8CTRL & Hw21) == 0)
        return -2;

    if (isreset)
        pCAMBUSCFG->SoftResetRegister |= (0x1 << sel);
    else
        pCAMBUSCFG->SoftResetRegister &= ~(0x1 << sel);

    return 0;
}

int tca_ckc_fclk_enable(unsigned int fclk, unsigned int enable)
{
    volatile unsigned int *pCLKCTRL;

    if (fclk >= FBUS_HSIO)
        pCLKCTRL = (volatile unsigned int *)((&pCKC->CLK8CTRL)+(fclk-FBUS_HSIO));
    else
        pCLKCTRL = (volatile unsigned int *)((&pCKC->CLK0CTRL)+fclk);

    if (enable)
        *pCLKCTRL |= Hw21;
    else
        *pCLKCTRL &= ~Hw21;
    while(*pCLKCTRL & Hw29);

    return 0;
}

int tca_ckc_pclk_enable(unsigned int pclk, unsigned int enable)
{
    volatile unsigned int *pPERI = (volatile unsigned int *)((&pCKC->PCLK_TCX)+pclk);

    if (enable)
        *pPERI |= Hw29;
    else
        *pPERI &= ~Hw29;

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
EXPORT_SYMBOL(tca_ckc_setcpu);
EXPORT_SYMBOL(tca_ckc_getcpu);
EXPORT_SYMBOL(tca_ckc_setfbusctrl);
EXPORT_SYMBOL(tca_ckc_getfbusctrl);
EXPORT_SYMBOL(tca_ckc_setperi);
EXPORT_SYMBOL(tca_ckc_getperi);
EXPORT_SYMBOL(tca_ckc_setpmupwroff);
EXPORT_SYMBOL(tca_ckc_getpmupwroff);
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

#endif

/* end of file */
