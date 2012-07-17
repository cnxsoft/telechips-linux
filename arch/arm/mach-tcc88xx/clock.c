/*
 * linux/arch/arm/mach-tcc88xx/clock.c
 *
 * Clock driver for TCC88xx
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/spinlock.h>

#include <mach/clock.h>
#include <mach/globals.h>
#include <mach/tca_ckc.h>
#include <linux/delay.h>

#include <mach/bsp.h>
#include <asm/io.h> 

/*===========================================================================

                 DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#define dbg(msg...)    if (0) { printk( "CLOCK: " msg); }

static LIST_HEAD(clock_list);
static DEFINE_SPINLOCK(clock_lock);
static DEFINE_MUTEX(clock_mutex);

extern int tcc_change_pmu(unsigned int src, unsigned int onoff);


/*===========================================================================

                                Clock Sources

===========================================================================*/

/*------------------------------ xin ------------------------------*/
static struct clk clk_xin = {
    .name = "xin",
    .rate = 12000000,
};

/*----------------------------- PLL0 ------------------------------*/
static struct clk clk_pll0 = {
    .name = "pll0",
    .flags = CLK_ALWAYS_ENABLED,
};

/*----------------------------- PLL1 ------------------------------*/
static struct clk clk_pll1 = {
    .name = "pll1",
    .flags = CLK_ALWAYS_ENABLED,
};

/*----------------------------- PLL2 ------------------------------*/
static struct clk clk_pll2 = {
    .name = "pll2",
    .flags = CLK_ALWAYS_ENABLED,
};

/*----------------------------- PLL3 ------------------------------*/
static struct clk clk_pll3 = {
    .name = "pll3",
    .flags = CLK_ALWAYS_ENABLED,
};

/*----------------------------- PLL4 ------------------------------*/
static struct clk clk_pll4 = {
    .name = "pll4",
    .flags = CLK_ALWAYS_ENABLED,
};

/*----------------------------- PLL5 ------------------------------*/
static struct clk clk_pll5 = {
    .name = "pll5",
    .flags = CLK_ALWAYS_ENABLED,
};


/*===========================================================================

                               System Clocks

===========================================================================*/

/*===========================================================================
          Clock Control Functions
===========================================================================*/

static int fclk_enable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);
    if (tcc_change_pmu(clk->clock_idx,1)) {
        if((tca_ckc_getpmupwroff(clk->pwdn_idx+2) == 1) && clk->pwdn) {
            if (clk->pwdn) {
                clk->pwdn(clk->pwdn_idx+1, 0);    // BUSPRE
                clk->pwdn(clk->pwdn_idx+2, 0);    // BUS
                clk->pwdn(clk->pwdn_idx, 1);    // UBSISOLATION
                ndelay(50);
            }
            if (clk->swreset) {
                clk->swreset(clk->swreset_idx, 0);
                if (clk->swreset_idx == FBUS_VBUS)
                    clk->swreset(FBUS_VCORE, 0);
            }
        }
        if (clk->clock)
            clk->clock(clk->clock_idx, 1);
    }
    local_irq_restore(flags);
    return 0;
}

static void fclk_disable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);
    if (tcc_change_pmu(clk->clock_idx,0)) {
        if (clk->clock)
            clk->clock(clk->clock_idx, 0);
    }
    local_irq_restore(flags);
}

static int fclk_set_rate(struct clk *clk, unsigned long rate)
{
    unsigned long flags = 0;
    if (clk->clock_idx != FBUS_MEM)
        local_irq_save(flags);
    clk->real_rate = tca_ckc_setfbusctrl(clk->clock_idx, NOCHANGE, rate/100) * 100;
    dbg("%s: clkname:%s, old_rate:%ld, new_rate:%ld(%ld)\n", __func__, clk->name, clk->rate, rate, clk->real_rate);
    if (clk->clock_idx != FBUS_MEM)
        local_irq_restore(flags);
    return 0;
}

static unsigned long fclk_get_rate(struct clk *clk)
{
    unsigned long flags;
    if (clk->real_rate == 0xFFFFFFFF) {
        local_irq_save(flags);     
        clk->real_rate = (unsigned long)tca_ckc_getfbusctrl(clk->clock_idx) * 100;
        local_irq_restore(flags);
    }
    return (unsigned long)(clk->real_rate);
}

static int phy_enable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);     
    tca_ckc_setpmupwroff(clk->pwdn_idx, 0);
    local_irq_restore(flags);
    return 0;
}

static void phy_disable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);     
    tca_ckc_setpmupwroff(clk->pwdn_idx, 1);
    local_irq_restore(flags);
}

static int pclk_enable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);     
    if (clk->pwdn)
        clk->pwdn(clk->pwdn_idx, 0);
    if (clk->clock)
        clk->clock(clk->clock_idx, 1);
    if (clk->swreset)
        clk->swreset(clk->pwdn_idx, 0);
    local_irq_restore(flags);
    return 0;
}

static void pclk_disable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);     
    if (clk->swreset)
        clk->swreset(clk->pwdn_idx, 1);
    if (clk->clock)
        clk->clock(clk->clock_idx, 0);
    if (clk->pwdn)
        clk->pwdn(clk->pwdn_idx, 1);
    local_irq_restore(flags);
}

static int pclk_set_rate(struct clk *clk, unsigned long rate)
{
    unsigned long flags;
    local_irq_save(flags);     
    clk->real_rate = tca_ckc_setperi(clk->clock_idx, NOCHANGE, rate/100) * 100;
    local_irq_restore(flags);
    dbg("%s: clkname:%s, old_rate:%ld, new_rate:%ld(%ld)\n", __func__, clk->name, clk->rate, rate, clk->real_rate);
    return 0;
}

static unsigned long pclk_get_rate(struct clk *clk)
{
    unsigned long flags;
    if (clk->real_rate == 0xFFFFFFFF) {
        local_irq_save(flags);     
        clk->real_rate = (unsigned long)tca_ckc_getperi(clk->clock_idx) * 100;
        local_irq_restore(flags);
    }
    return (unsigned long)(clk->real_rate);
}

/**************************************/
/* System Clock */

static struct clk clk_cpu = {
    .name           = "cpu",
    .flags          = CLK_ALWAYS_ENABLED,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .clock_idx      = FBUS_CPU,
};

static struct clk clk_membus = {
    .name           = "membus",
    .flags          = CLK_ALWAYS_ENABLED,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .clock_idx      = FBUS_MEM,
};

static struct clk clk_smu = {
    .name           = "smu",
    .flags          = CLK_ALWAYS_ENABLED,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .clock_idx      = FBUS_SMU,
};


/**************************************/
/* Main Bus Clock */

static struct clk clk_ddi = {
    .name           = "ddi",
    .flags          = CLK_AUTO_OFF,
    .enable         = fclk_enable,
    .disable        = fclk_disable,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .pwdn_idx       = PMU_DDIBUSISOLATION,
    .pwdn           = tca_ckc_setpmupwroff,
    .swreset_idx    = FBUS_DDI,
    .swreset        = tca_ckc_setfbusswreset,
    .clock_idx      = FBUS_DDI,
    .clock          = tca_ckc_fclk_enable
};

static struct clk clk_gpu = {
    .name           = "gpu",
    .flags          = CLK_AUTO_OFF,
    .enable         = fclk_enable,
    .disable        = fclk_disable,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .pwdn_idx       = PMU_GRAPHICBUSISOLATION,
    .pwdn           = tca_ckc_setpmupwroff,
    .swreset_idx    = FBUS_GPU,
    .swreset        = tca_ckc_setfbusswreset,
    .clock_idx      = FBUS_GPU,
    .clock          = tca_ckc_fclk_enable,
};

static struct clk clk_io = {
    .name           = "iobus",
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = fclk_enable,
    .disable        = fclk_disable,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .clock_idx      = FBUS_IO,
};

static struct clk clk_vbus = {
    .name           = "vbus",
    .flags          = CLK_AUTO_OFF,
    .enable         = fclk_enable,
    .disable        = fclk_disable,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .pwdn_idx       = PMU_VIDEOBUSISOLATION,
    .pwdn           = tca_ckc_setpmupwroff,
    .swreset_idx    = FBUS_VBUS,
    .swreset        = tca_ckc_setfbusswreset,
    .clock_idx      = FBUS_VBUS,
    .clock          = tca_ckc_fclk_enable,
};

static struct clk clk_vcore = {
    .name           = "vcore",
    .flags          = CLK_AUTO_OFF,
    .enable         = fclk_enable,
    .disable        = fclk_disable,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .clock_idx      = FBUS_VCORE,
    .clock          = tca_ckc_fclk_enable,
};

static struct clk clk_hsio = {
    .name           = "hsio",
    .flags          = CLK_AUTO_OFF,
    .enable         = fclk_enable,
    .disable        = fclk_disable,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .pwdn_idx       = PMU_HSBUSISOLATION,
    .pwdn           = tca_ckc_setpmupwroff,
    .swreset_idx    = FBUS_HSIO,
    .swreset        = tca_ckc_setfbusswreset,
    .clock_idx      = FBUS_HSIO,
    .clock          = tca_ckc_fclk_enable,
};

static struct clk clk_cam = {
    .name           = "cam",
    .flags          = CLK_AUTO_OFF,
    .enable         = fclk_enable,
    .disable        = fclk_disable,
    .set_rate       = fclk_set_rate,
    .get_rate       = fclk_get_rate,
    .pwdn_idx       = PMU_CAMBUSISOLATION,
    .pwdn           = tca_ckc_setpmupwroff,
    .swreset_idx    = FBUS_CAM,
    .swreset        = tca_ckc_setfbusswreset,
    .clock_idx      = FBUS_CAM,
    .clock          = tca_ckc_fclk_enable,
};


/**************************************/
/* Display Bus Clock */

static struct clk clk_viqe = {
    .name      = "viqe",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = DDIBUS_VIQE,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
};

static struct clk clk_lcdc0 = {
    .name      = "lcdc0",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = DDIBUS_LCDC0,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
    .clock_idx = PERI_LCD0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_lcdc1 = {
    .name      = "lcdc1",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = DDIBUS_LCDC1,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
    .clock_idx = PERI_LCD1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_lcdsi0 = {
    .name      = "lcdsi0",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = DDIBUS_LCDSI0,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
    .clock_idx = PERI_LCDSI0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_lcdsi1 = {
    .name      = "lcdsi1",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = DDIBUS_LCDSI1,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
    .clock_idx = PERI_LCDSI1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_m2m0 = {
    .name      = "m2m0",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = DDIBUS_MSCL0,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
};

static struct clk clk_m2m1 = {
    .name      = "m2m1",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = DDIBUS_MSCL1,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
};

static struct clk clk_ddi_cache = {
    .name      = "ddi_cache",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = DDIBUS_DDIC,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
};

static struct clk clk_hdmi_phy = {
    .name      = "hdmi_phy",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = phy_enable,
    .disable   = phy_disable,
    .pwdn_idx  = PMU_HDMIPHY,
};

static struct clk clk_hdmi = {
    .name      = "hdmi",
    .parent    = &clk_hdmi_phy,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = DDIBUS_HDMI,
    .pwdn      = tca_ckc_setddipwdn,
    .swreset   = tca_ckc_setddiswreset,
};

static struct clk clk_lvds = {
    .name      = "lvds",
    .parent    = &clk_ddi,
    .flags     = CLK_AUTO_OFF,
    .enable    = phy_enable,
    .disable   = phy_disable,
    .pwdn_idx  = PMU_LVDSPHY,
};


/**************************************/
/* Graphic Bus Clock */

static struct clk clk_mali = {
    .name      = "mali_clk",
    .parent    = &clk_gpu,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = GPUBUS_GRP,
    .pwdn      = tca_ckc_setgpupwdn,
    .swreset   = tca_ckc_setgpuswreset,
};


/**************************************/
/* I/O Bus Clock */

static struct clk clk_usb11h = {
    .name      = "usb11h",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_USB11HOST,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_USB11H,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_usb20otg = {
    .name      = "usb_otg",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_USB20OTG,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_ide = {
    .name      = "ide",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,     
    .pwdn_idx  = RB_IDECONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_dma = {
    .name      = "dma",
    .parent    = &clk_io,
    .flags     = CLK_ALWAYS_ENABLED,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_DMACONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_sdmmcch = {
    .name      = "sdhc",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_SDMMCCHANNELCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_sdmmc0 = {
    .name      = "sdhc0",
    .parent    = &clk_sdmmcch,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_SDMMC0CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_SDMMC0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_mstick = {
    .name      = "mstick_clk",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_MSTICKCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_MSTICK,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_i2c0 = {
    .name      = "i2c0",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_I2CCONTROLLER0,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_I2C0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_nfc = {
    .name      = "nfc",
    .parent    = &clk_io,
    .flags     = CLK_ALWAYS_ENABLED,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_NFCCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_ehi0 = {
    .name      = "ehi0",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_EXTHCONTROLLER0,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_EHI0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_ehi1 = {
    .name      = "ehi1",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_EXTHCONTROLLER1,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_EHI1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_uart0  = {
    .name      = "uart0",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_UARTCONTROLLER0,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_UART0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_uart1 = {
    .name      = "uart1",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_UARTCONTROLLER1,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_UART1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_uart2 = {
    .name      = "uart2",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_UARTCONTROLLER2,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_UART2,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_uart3 = {
    .name      = "uart3",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_UARTCONTROLLER3,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_UART3,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_uart4 = {
    .name      = "uart4",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_UARTCONTROLLER4,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_UART4,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_uart5 = {
    .name      = "uart5",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_UARTCONTROLLER5,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_UART5,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb0 = {
    .name      = "gpsb0",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_GPSBCONTROLLER0,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_GPSB0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb1 = {
    .name      = "gpsb1",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_GPSBCONTROLLER1,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_GPSB1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb2 = {
    .name      = "gpsb2",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_GPSBCONTROLLER2,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_GPSB2,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb3 = {
    .name      = "gpsb3",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_GPSBCONTROLLER3,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_GPSB3,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb4 = {
    .name      = "gpsb4",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_GPSBCONTROLLER4,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_GPSB4,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb5 = {
    .name      = "gpsb5",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_GPSBCONTROLLER5,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_GPSB5,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_adma = {
    .name      = "adma",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_ADMACONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_dai0 = {
    .name      = "dai0",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_DAI0CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_DAI0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_dai1 = {
    .name      = "dai1",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_DAI1CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_DAI1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_spdif = {
    .name      = "spdif",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_SPDIFTXCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_SPDIF,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_rtc = {
    .name      = "rtc",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_RTCCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_adc = {
    .name      = "adc",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_TSADCCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_ADC,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_overlay = {
    .name      = "overlay",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_OVERAYMIXERCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_mpefec = {
    .name      = "mpefec",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_MPE_FECCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_tsif0 = {
    .name      = "tsif0",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_TSIF0CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_sram = {
    .name      = "sram",
    .parent    = &clk_io,
    .flags     = CLK_ALWAYS_ENABLED,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_SRAMCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_sdmmc1 = {
    .name      = "sdhc1",
    .parent    = &clk_sdmmcch,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_SDMMC1CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_SDMMC1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_sdmmc2 = {
    .name      = "sdhc2",
    .parent    = &clk_sdmmcch,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_SDMMC2CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_SDMMC2,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_sdmmc3 = {
    .name      = "sdhc3",
    .parent    = &clk_sdmmcch,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_SDMMC3CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_SDMMC3,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_i2c1 = {
    .name      = "i2c1",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_I2CCONTROLLER1,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_I2C1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_dai2 = {
    .name      = "dai2",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = RB_DAI2CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
    .clock_idx = PERI_DAI2,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_pdm = {
    .name      = "pdm",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_PDMCONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_tsif1 = {
    .name      = "tsif1",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_TSIF1CONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_remocon = {
    .name      = "remocon",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = RB_REMOTECONTROLLER,
    .pwdn      = tca_ckc_setiopwdn,
    .swreset   = tca_ckc_setioswreset,
};

static struct clk clk_hdmi_audio = {
    .name      = "hdmi_audio",
    .parent    = &clk_io,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .clock_idx = PERI_HDMIA,
    .clock     = tca_ckc_pclk_enable,
};


/**************************************/
/* Video Bus Clock */

static struct clk clk_vcache = {
    .name      = "vcache",
    .parent    = &clk_vbus,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = VIDEOBUS_CACHE,
    .pwdn      = tca_ckc_setvideopwdn,
    .swreset   = tca_ckc_setvideoswreset,
};

static struct clk clk_vcodec = {
    .name      = "vcodec",
    .parent    = &clk_vbus,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = VIDEOBUS_CODEC,
    .pwdn      = tca_ckc_setvideopwdn,
    .swreset   = tca_ckc_setvideoswreset,
};

static struct clk clk_jpege = {
    .name      = "jpege",
    .parent    = &clk_vbus,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = VIDEOBUS_JENC,
    .pwdn      = tca_ckc_setvideopwdn,
    .swreset   = tca_ckc_setvideoswreset,
};

static struct clk clk_videodac = {
    .name      = "videodac",
    .flags     = CLK_AUTO_OFF,
    .enable    = phy_enable,
    .disable   = phy_disable,
    .pwdn_idx  = PMU_VIDEODAC,
};


/**************************************/
/* High speed I/O Bus Clock */

static struct clk clk_pcie = {
    .name      = "pcie",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = HSIOBUS_PCIE,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
};

static struct clk clk_sata_ref0 = {
    .name      = "sata_ref0",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .clock_idx = PERI_SATA_REF0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_sata_ref1 = {
    .name      = "sata_ref1",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .clock_idx = PERI_SATA_REF1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_satah0_phy = {
    .name      = "satah0_phy",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = phy_enable,
    .disable   = phy_disable,
    .pwdn_idx  = PMU_SATAPHY,
};

static struct clk clk_satah0 = {
    .name      = "satah0",
    .parent    = &clk_satah0_phy,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = HSIOBUS_SATAH0,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
    .clock_idx = PERI_SATAH0,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_satah1_phy = {
    .name      = "satah1_phy",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = phy_enable,
    .disable   = phy_disable,
    .pwdn_idx  = PMU_SATA1PHY,
};

static struct clk clk_satah1 = {
    .name      = "satah1",
    .parent    = &clk_satah1_phy,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = HSIOBUS_SATAH1,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
    .clock_idx = PERI_SATAH1,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_satad = {
    .name      = "satad",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
#if 0  // SangWon, Feb 21 2011, Do not use sata device.
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
#endif
    .pwdn_idx  = HSIOBUS_SATAD,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
#if 0  // SangWon, Feb 21 2011, Do not use sata device.
    .clock_idx = PERI_SATAD,
    .clock     = tca_ckc_pclk_enable,
#endif
};

static struct clk clk_usb20h = {
    .name      = "usb20h",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = HSIOBUS_USB20H,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
    .clock_idx = PERI_USB20H,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_gmac = {
    .name      = "gmac",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .pwdn_idx  = HSIOBUS_GMAC,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
    .clock_idx = PERI_GMAC,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_tsdemux = {
    .name      = "tsdemux",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = HSIOBUS_TSDEMUX,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
};

static struct clk clk_hsdma = {
    .name      = "hsdma",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = HSIOBUS_HSDMA,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
};

static struct clk clk_cipher = {
    .name      = "cipher",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = HSIOBUS_CIPHER,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
};

static struct clk clk_dmax = {
    .name      = "dmax",
    .parent    = &clk_hsio,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = HSIOBUS_DMAX,
    .pwdn      = tca_ckc_sethsiopwdn,
    .swreset   = tca_ckc_sethsioswreset,
};


/**************************************/
/* Camera Bus Clock */

static struct clk clk_cif = {
    .name      = "cif",
    .parent    = &clk_cam,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = CAMBUS_CIF,
    .pwdn      = tca_ckc_setcampwdn,
    .swreset   = tca_ckc_setcamswreset,
};

static struct clk clk_isp = {
    .name      = "isp",
    .parent    = &clk_cam,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .pwdn_idx  = CAMBUS_ISP,
    .pwdn      = tca_ckc_setcampwdn,
    .swreset   = tca_ckc_setcamswreset,
};

static struct clk clk_cifmc = {
    .name      = "cifmc",
    .parent    = &clk_cif,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .clock_idx = PERI_CIFMC,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_cifsc = {
    .name      = "cifsc",
    .parent    = &clk_cif,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .clock_idx = PERI_CIFSC,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_isps = {
    .name      = "isps",
    .parent    = &clk_isp,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .clock_idx = PERI_ISPS,
    .clock     = tca_ckc_pclk_enable,
};

static struct clk clk_ispj = {
    .name      = "ispj",
    .parent    = &clk_isp,
    .flags     = CLK_AUTO_OFF,
    .enable    = pclk_enable,
    .disable   = pclk_disable,
    .set_rate  = pclk_set_rate,
    .get_rate  = pclk_get_rate,
    .clock_idx = PERI_ISPJ,
    .clock     = tca_ckc_pclk_enable,
};


/*===========================================================================

                          Register Clock Driver

===========================================================================*/

static struct clk *onchip_clocks[] = {
    // clock source
    &clk_xin,
//    &clk_xtin,
    &clk_pll0,
    &clk_pll1,
    &clk_pll2,
    &clk_pll3,
    &clk_pll4,
    &clk_pll5,

    // System clocks
    &clk_cpu,
    &clk_membus,
    &clk_smu,

    // Display Bus
    &clk_viqe,            // bit 1
    &clk_lcdc0,            // bit 2    with lcd0 peri
    &clk_lcdc1,            // bit 3    with lcd1 peri
    &clk_lcdsi0,        // bit 4    with lcdsi0 peri
    &clk_m2m0,            // bit 5
    &clk_m2m1,            // bit 6
    &clk_ddi_cache,        // bit 7
    &clk_hdmi,            // bit 8    with hdmi&hdmia peri
    &clk_lcdsi1,        // bit 9    with lcdsi1 peri
    &clk_lvds,
    &clk_hdmi_phy,
    &clk_ddi,

    // Graphic Bus
    &clk_mali,            // bit 0
    &clk_gpu,

    // IO Bus
    &clk_usb11h,        // bit 0
    &clk_usb20otg,        // bit 1
    &clk_ide,            // bit 2
//    &clk_dma,            // bit 3
    &clk_sdmmcch,        // bit 4
    &clk_sdmmc0,        // bit 5
    &clk_mstick,        // bit 6
    &clk_i2c0,            // bit 7
    &clk_nfc,            // bit 8
    &clk_ehi0,            // bit 9
    &clk_ehi1,            // bit 10
    &clk_uart0,            // bit 11
    &clk_uart1,            // bit 12
    &clk_uart2,            // bit 13
    &clk_uart3,            // bit 14
    &clk_uart4,            // bit 15
    &clk_uart5,            // bit 16
    &clk_gpsb0,            // bit 17
    &clk_gpsb1,            // bit 18
    &clk_gpsb2,            // bit 19
    &clk_gpsb3,            // bit 20
    &clk_gpsb4,            // bit 21
    &clk_gpsb5,            // bit 22
    &clk_dai0,            // bit 23
    &clk_dai1,            // bit 24
    &clk_spdif,            // bit 25
    &clk_rtc,            // bit 26
    &clk_adc,            // bit 27
    &clk_overlay,        // bit 28
    &clk_adma,            // bit 31    // reserved
    &clk_mpefec,        // bit 0
    &clk_tsif0,            // bit 1
//    &clk_sram,            // bit 2
    &clk_sdmmc1,        // bit 3
    &clk_sdmmc2,        // bit 4
    &clk_sdmmc3,        // bit 5
    &clk_i2c1,            // bit 6
    &clk_dai2,            // bit 7
    &clk_pdm,            // bit 8
    &clk_tsif1,            // bit 9
    &clk_remocon,        // bit 10
    &clk_hdmi_audio,
    &clk_io,

    // HSIO Bus
    &clk_pcie,            // bit 0    with pcie peri
    &clk_sata_ref0,
    &clk_sata_ref1,
    &clk_satah0,        // bit 1    with satah0 peri
    &clk_satah1,        // bit 2    with satah1 peri
    &clk_satad,            // bit 3    with satad peri
    &clk_usb20h,        // bit 4    with usb20h peri
    &clk_gmac,            // bit 5    with gmac peri
    &clk_tsdemux,        // bit 6
//    &clk_hsdma,            // bit 7
    &clk_cipher,        // bit 8
    &clk_dmax,            // bit 9
    &clk_satah1_phy,
    &clk_satah0_phy,
    &clk_hsio,

    // Video Bus
    &clk_jpege,            // bit 0
    &clk_vcodec,        // bit 2
    &clk_vcache,        // bit 3
    &clk_videodac,
    &clk_vbus,
    &clk_vcore,

    // Camera Bus
    &clk_cif,            // bit 0
    &clk_isp,            // bit 1
    &clk_cifmc,            // peri
    &clk_cifsc,            // peri
    &clk_isps,            // peri
    &clk_ispj,            // peri
    &clk_cam,
};

struct clk *clk_get(struct device *dev, const char *id)
{
    struct clk *clk;
    mutex_lock(&clock_mutex);
    list_for_each_entry(clk, &clock_list, list) {
        if (!strcmp(id, clk->name) && clk->dev == dev)
            goto found;
        if (!strcmp(id, clk->name) && clk->dev == NULL)
            goto found;
    }
    clk = ERR_PTR(-ENOENT);
found:
    mutex_unlock(&clock_mutex);
    return clk;
}
EXPORT_SYMBOL(clk_get);

void clk_put(struct clk *clk)
{
    /* do nothing */
}
EXPORT_SYMBOL(clk_put);

static int _clk_enable(struct clk *clk)
{
    int ret = 0;
    clk->usecount++;
    if (clk->usecount == 1) {
        if (clk->parent)
            ret = _clk_enable(clk->parent);
        if (clk->enable)
            ret = clk->enable(clk);
        else
            pr_warning("clock: %s: unimplemented function 'clk_enable' called\n", clk->name);
    }
    return ret;
}

int clk_enable(struct clk *clk)
{
    int ret = 0;
    BUG_ON(clk == 0);
    spin_lock(&clock_lock);
    ret = _clk_enable(clk);
    spin_unlock(&clock_lock);
    return ret;
}
EXPORT_SYMBOL(clk_enable);

static void _clk_disable(struct clk *clk)
{
    if (clk->flags & CLK_ALWAYS_ENABLED)
        return;
    clk->usecount--;
    if (clk->usecount == 0) {
        if (clk->disable)
            (*clk->disable)(clk);
        else
            pr_warning("clock: %s: unimplemented function 'clk_disable' called\n", clk->name);
        if (clk->parent)
            _clk_disable(clk->parent);
    }
}

void clk_disable(struct clk *clk)
{
    BUG_ON(clk->usecount == 0);
    spin_lock(&clock_lock);
    _clk_disable(clk);
    spin_unlock(&clock_lock);
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
    unsigned long rate;
    if (clk->get_rate)
        rate = (*clk->get_rate)(clk);
    else
        rate = clk->real_rate;
    return rate;
}
EXPORT_SYMBOL(clk_get_rate);

/*
 * Set the clock rate for a clock source
 */
static int _clk_set_rate(struct clk *clk, unsigned long rate)
{
    int ret = 0;
    if (clk->flags & CLK_FIXED_RATE)
        return 0;
    if (clk->set_rate)
        ret = (*clk->set_rate)(clk, rate);
    else
        pr_warning("clock: %s: unimplemented function 'clk_set_rate' called\n", clk->name);
    return ret;
}

int clk_set_rate(struct clk *clk, unsigned long rate)
{
    int ret = 0;
    if (clk->rate == rate)
        return 0;
    spin_lock(&clock_lock); 
    ret = _clk_set_rate(clk, rate);
    spin_unlock(&clock_lock); 
    if (ret == 0)
        clk->rate = rate;
    return ret;
}
EXPORT_SYMBOL(clk_set_rate);

int clk_forced_set_rate(struct clk *clk, unsigned long rate)
{
    int ret = 0;
    spin_lock(&clock_lock); 
    ret = _clk_set_rate(clk, rate);
    spin_unlock(&clock_lock); 
    if (ret == 0)
        clk->rate = rate;
    return ret;
}
EXPORT_SYMBOL(clk_forced_set_rate);

/*
 * Adjust a rate to the exact rate a clock can provide
 */
long clk_round_rate(struct clk *clk, unsigned long rate)
{
    //TODO
    return rate;
}
EXPORT_SYMBOL(clk_round_rate);

/*
 * Set the parent clock source for the clock
 */
int clk_set_parent(struct clk *clk, struct clk *parent)
{
    spin_lock(&clock_lock);
    clk->parent = parent;
    spin_unlock(&clock_lock);
    return 0;
}
EXPORT_SYMBOL(clk_set_parent);

/*
 * Get the parent clock source for the clock
 */
struct clk *clk_get_parent(struct clk *clk)
{
    BUG_ON(clk == 0);
    return clk->parent;
}
EXPORT_SYMBOL(clk_get_parent);

int __init clk_init(void)
{
    struct clk **clk;
    spin_lock_init(&clock_lock);
    mutex_lock(&clock_mutex);
    tca_ckc_init();
    for (clk = onchip_clocks; clk < onchip_clocks + ARRAY_SIZE(onchip_clocks); clk++) {
        (*clk)->rate = 0xFFFFFFFF;
        (*clk)->real_rate = 0xFFFFFFFF;
        list_add_tail(&(*clk)->list, &clock_list);
    }
    mutex_unlock(&clock_mutex);
    pr_info("TCC clock driver initialized\n");
    return 0;
}
arch_initcall(clk_init);

void clk_disable_unused(void)
{
    struct clk *clk;
    int count = 0;
    mutex_lock(&clock_mutex);
    list_for_each_entry(clk, &clock_list, list) {
        if (clk->flags & CLK_AUTO_OFF && !clk->usecount && clk->disable) {
            clk->disable(clk);
            count++;
            pr_debug("clock: clock '%s' disabled\n", clk->name);
        }
    }
    mutex_unlock(&clock_mutex);
    pr_debug("clock: disabled %d unused clocks\n", count);
}
EXPORT_SYMBOL(clk_disable_unused);

static int __init clock_late_init(void)
{
    clk_disable_unused();
    return 0;
}
late_initcall(clock_late_init);


#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static void *tcc_clk_start(struct seq_file *m, loff_t *pos)
{
    return *pos < 1 ? (void *)1 : NULL;
}

static void *tcc_clk_next(struct seq_file *m, void *v, loff_t *pos)
{
    ++*pos;
    return NULL;
}

static void tcc_clk_stop(struct seq_file *m, void *v)
{
}

static int tcc_clk_show(struct seq_file *m, void *v)
{
    unsigned long flags;
    spin_lock_irqsave(&clock_lock, flags);
    seq_printf(m, "cpu         : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_CPU)/10000);
    seq_printf(m, "memory bus  : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_MEM)/10000);
    seq_printf(m, "display bus : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_DDI)/10000);
    seq_printf(m, "graphic bus : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_GPU)/10000);
    seq_printf(m, "i/o bus     : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_IO)/10000);
    seq_printf(m, "video bus   : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_VBUS)/10000);
    seq_printf(m, "video codec : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_VCORE)/10000);
    seq_printf(m, "hsio bus    : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_HSIO)/10000);
    seq_printf(m, "camera bus  : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_CAM)/10000);
    seq_printf(m, "smu bus     : %4d MHz\n", tca_ckc_getfbusctrl(FBUS_SMU)/10000);
    spin_unlock_irqrestore(&clock_lock, flags);
    return 0;
}

static const struct seq_operations tcc_clk_op = {
    .start  = tcc_clk_start,
    .next   = tcc_clk_next,
    .stop   = tcc_clk_stop,
    .show   = tcc_clk_show
};

static int tcc_clk_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &tcc_clk_op);
}

static const struct file_operations proc_tcc_clk_operations = {
    .open       = tcc_clk_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release,
};

static int __init tcc_clk_proc_init(void)
{
    proc_create("clocks", 0, NULL, &proc_tcc_clk_operations);
    return 0;
}

__initcall(tcc_clk_proc_init);
