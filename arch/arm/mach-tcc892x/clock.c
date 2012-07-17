/*
 * linux/arch/arm/mach-tcc892x/clock.c
 *
 * Clock driver for TCC892x
 *
 * Copyright (C) 2011 Telechips, Inc.
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

#define TCC_HDMI_LCDC1_USE

static LIST_HEAD(clock_list);
static DEFINE_SPINLOCK(clock_lock);
static DEFINE_MUTEX(clock_mutex);


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
    if (clk->pwdn) {
        clk->pwdn(clk->pwdn_idx, 0);
        ndelay(50);
    }
    if (clk->swreset)
        clk->swreset(clk->swreset_idx, 0);
    if (clk->clock)
        clk->clock(clk->clock_idx, 1);
    local_irq_restore(flags);
    return 0;
}

static void fclk_disable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);
    if (clk->clock)
        clk->clock(clk->clock_idx, 0);
    if (clk->swreset)
        clk->swreset(clk->swreset_idx, 1);
    if (clk->pwdn)
        clk->pwdn(clk->pwdn_idx, 1);
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
    tca_ckc_setippwdn(clk->pwdn_idx, 0);
    local_irq_restore(flags);
    return 0;
}

static void phy_disable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);     
    tca_ckc_setippwdn(clk->pwdn_idx, 1);
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
        clk->swreset(clk->swreset_idx, 0);
    local_irq_restore(flags);
    return 0;
}

static void pclk_disable(struct clk *clk)
{
    unsigned long flags;
    dbg("%s: clk:%s\n", __func__, clk->name);
    local_irq_save(flags);     
    if (clk->swreset)
        clk->swreset(clk->swreset_idx, 1);
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
    .pwdn_idx       = FBUS_DDI,
    .pwdn           = tca_ckc_setfbuspwdn,
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
    .pwdn_idx       = FBUS_GPU,
    .pwdn           = tca_ckc_setfbuspwdn,
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
    .pwdn_idx       = FBUS_VBUS,
    .pwdn           = tca_ckc_setfbuspwdn,
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
    .pwdn_idx       = FBUS_HSIO,
    .pwdn           = tca_ckc_setfbuspwdn,
    .swreset_idx    = FBUS_HSIO,
    .swreset        = tca_ckc_setfbusswreset,
    .clock_idx      = FBUS_HSIO,
    .clock          = tca_ckc_fclk_enable,
};


/**************************************/
/* IP isolation Control */
static struct clk clk_otp_phy = {
    .name           = "otp_phy",
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_OTP,
};

static struct clk clk_rtc_phy = {
    .name           = "rtc_phy",
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_RTC,
};

static struct clk clk_pll_phy = {
    .name           = "pll_phy",
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_PLL,
};

static struct clk clk_ecid_phy = {
    .name           = "ecid_phy",
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_ECID,
};

static struct clk clk_hdmi_phy = {
    .name           = "hdmi_phy",
	.parent         = &clk_ddi,
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_HDMI,
};

static struct clk clk_vdac_phy = {
    .name           = "vdac_phy",
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_VDAC,
};

static struct clk clk_tsadc_phy = {
    .name           = "tsadc_phy",
	.parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_TSADC,
};

static struct clk clk_usbhp_phy = {
    .name           = "usbhp_phy",
	.parent         = &clk_hsio,
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_USBHP,
};

static struct clk clk_usbop_phy = {
    .name           = "usbop_phy",
	.parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_USBOP,
};

static struct clk clk_lvds_phy = {
    .name           = "lvds_phy",
    .flags          = CLK_AUTO_OFF,
    .enable         = phy_enable,
    .disable        = phy_disable,
    .pwdn_idx       = PMU_ISOL_LVDS,
};


/**************************************/
/* Peripheral Bus Clock */

static struct clk clk_timerx = {
    .name           = "timerx",
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_TCX,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_timert = {
    .name           = "timert",
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_TCT,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_timerz = {
    .name           = "timerz",
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_TCZ,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_out0 = {
    .name           = "out0",
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_OUT0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_out1 = {
    .name           = "out1",
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_OUT1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_jpeg = {
    .name           = "jpeg",
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_JPEG,
    .clock          = tca_ckc_pclk_enable,
};


/**************************************/
/* Display Bus Clock */

static struct clk clk_lcdc = {
    .name           = "lcdc",
    .parent         = &clk_ddi,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = DDIBUS_LCDC,
    .pwdn           = tca_ckc_setddipwdn,
    .swreset_idx    = DDIBUS_LCDC,
    .swreset        = tca_ckc_setddiswreset,
};

static struct clk clk_ntscpal = {
    .name           = "ntscpal",
    .parent         = &clk_ddi,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = DDIBUS_NTSCPAL,
    .pwdn           = tca_ckc_setddipwdn,
    .swreset_idx    = DDIBUS_NTSCPAL,
    .swreset        = tca_ckc_setddiswreset,
};

static struct clk clk_hdmi = {
    .name           = "hdmi",
    .parent         = &clk_hdmi_phy,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = DDIBUS_HDMI,
    .pwdn           = tca_ckc_setddipwdn,
    .swreset_idx    = DDIBUS_HDMI,
    .swreset        = tca_ckc_setddiswreset,
    .clock_idx      = PERI_HDMI,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_lcdc0 = {
    .name           = "lcdc0",
    .parent         = &clk_lcdc,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_LCD0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_lcdsi0 = {
    .name           = "lcdsi0",
    .parent         = &clk_ddi,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_LCDSI0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_lcdc1 = {
    .name           = "lcdc1",
    .parent         = &clk_lcdc,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_LCD1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_lcdsi1 = {
    .name           = "lcdsi1",
    .parent         = &clk_ddi,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_LCDSI1,
    .clock          = tca_ckc_pclk_enable,
};


#if defined(TCC_HDMI_LCDC0_USE)
static struct clk clk_hdmi_lcdc = {
    .name           = "hdmi_lcdc",
    .parent         = &clk_lcdc0,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .clock_idx      = PERI_LCD0,
    .clock          = tca_ckc_hdmipclk_enable,
};
#elif defined(TCC_HDMI_LCDC1_USE)
static struct clk clk_hdmi_lcdc = {
    .name           = "hdmi_lcdc",
    .parent         = &clk_lcdc1,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .clock_idx      = PERI_LCD1,
    .clock          = tca_ckc_hdmipclk_enable,
};
#else
    #error Not defined HDMI_LCDC channel
#endif

static struct clk clk_lcd_timer = {
    .name           = "lcd_timer",
    .parent         = &clk_ddi,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_LCDTIMER,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_hdmi_audio = {
    .name           = "hdmi_audio",
    .parent         = &clk_ddi,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_HDMIA,
    .clock          = tca_ckc_pclk_enable,
};


/**************************************/
/* Graphic Bus Clock */

static struct clk clk_mali = {
    .name           = "mali_clk",
    .parent         = &clk_gpu,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = GPUBUS_GRP,
    .pwdn           = tca_ckc_setgpupwdn,
    .swreset_idx    = GPUBUS_GRP,
    .swreset        = tca_ckc_setgpuswreset,
};

/**************************************/
/* Video Bus Clock */

static struct clk clk_vcache = {
    .name           = "vcache",
    .parent         = &clk_vbus,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = VIDEOBUS_BUS,
    .pwdn           = tca_ckc_setvideopwdn,
    .swreset_idx    = VIDEOBUS_BUS,
    .swreset        = tca_ckc_setvideoswreset,
};

static struct clk clk_vcodec = {
    .name           = "vcodec",
    .parent         = &clk_vbus,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = VIDEOBUS_COD,
    .pwdn           = tca_ckc_setvideopwdn,
    .swreset_idx    = VIDEOBUS_COD,
    .swreset        = tca_ckc_setvideoswreset,
};

static struct clk clk_jpege = {
    .name           = "jpege",
    .parent         = &clk_vbus,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = VIDEOBUS_JENC,
    .pwdn           = tca_ckc_setvideopwdn,
    .swreset_idx    = VIDEOBUS_JENC,
    .swreset        = tca_ckc_setvideoswreset,
};

/**************************************/
/* I/O Bus Clock */

static struct clk clk_ehi = {
    .name           = "ehi",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_EHI,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_EHI,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_EHI,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_mpefec = {
    .name           = "mpefec",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_MPEFEC,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_MPEFEC,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_sdmmc = {
    .name           = "sdhc",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_SDMMCCHANNELCONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_SDMMCCHANNELCONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_sdmmc0 = {
    .name           = "sdhc0",
    .parent         = &clk_sdmmc,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_SDMMC0CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_SDMMC0CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_SDMMC0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_sdmmc1 = {
    .name           = "sdhc1",
    .parent         = &clk_sdmmc,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_SDMMC1CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_SDMMC1CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_SDMMC1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_sdmmc2 = {
    .name           = "sdhc2",
    .parent         = &clk_sdmmc,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_SDMMC2CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_SDMMC2CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_SDMMC2,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_sdmmc3 = {
    .name           = "sdhc3",
    .parent         = &clk_sdmmc,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_SDMMC3CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_SDMMC3CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_SDMMC3,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_dmac = {
    .name           = "dma",
    .parent         = &clk_io,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_DMACONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_DMACONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_dmac0 = {
    .name           = "dma0",
    .parent         = &clk_dmac,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_DMA0,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_DMA0,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_dmac1 = {
    .name           = "dma1",
    .parent         = &clk_dmac,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_DMA1,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_DMA1,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_dmac2 = {
    .name           = "dma2",
    .parent         = &clk_dmac,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_DMA2,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_DMA2,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_overlay = {
    .name           = "overlay",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_OVERAYMIXER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_OVERAYMIXER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_pwm = {
    .name           = "pdm",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_PWM,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_PWM,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_PDM,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_smc = {
    .name           = "smc",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_SMC,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_SMC,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_rtc = {
    .name           = "rtc",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_RTC,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_RTC,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_remote = {
    .name           = "remocon",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_REMOTECONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_REMOTECONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_tsadc = {
    .name           = "adc",
    .parent         = &clk_tsadc_phy,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_TSADC,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_TSADC,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_ADC,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_edi = {
    .name           = "edi_parent",
    .parent         = &clk_io,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_EDICONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_EDICONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_ediconf = {
    .name           = "edi",
    .parent         = &clk_edi,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_EDICONFIGURATION,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_EDICONFIGURATION,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_shuldbe1 = {
    .name           = "io_shuldbe1",
    .parent         = &clk_io,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_SHOULDBE1,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_SHOULDBE1,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_audio0 = {
    .name           = "audio0",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_AUDIO0_CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO0_CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_adma0 = {
    .name           = "adma0",
    .parent         = &clk_audio0,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_AUDIO0_MCADMA,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO0_MCADMA,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_dam0 = {
    .name           = "dam0",
    .parent         = &clk_audio0,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_ADAM0,
    .clock          = tca_ckc_pclk_enable,
};


static struct clk clk_dai0 = {
    .name           = "dai0",
    .parent         = &clk_adma0,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_AUDIO0_MCDAI,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO0_MCDAI,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_ADAI0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_spdif0 = {
    .name           = "spdif0",
    .parent         = &clk_audio0,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_AUDIO0_MCSPDIF,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO0_MCSPDIF,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_SPDIF0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_audio1 = {
    .name           = "audio1",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_AUDIO1_CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO1_CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_adma1 = {
    .name           = "adma1",
    .parent         = &clk_audio1,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_AUDIO1_ADMA,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO1_ADMA,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_dam1 = {
    .name           = "dam1",
    .parent         = &clk_audio1,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .clock_idx      = PERI_ADAM1,
    .clock          = tca_ckc_pclk_enable,
};


static struct clk clk_dai1 = {
    .name           = "dai1",
    .parent         = &clk_adma1,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_AUDIO1_DAI,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO1_DAI,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_ADAI1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_spdif1 = {
    .name           = "spdif1",
    .parent         = &clk_audio1,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_AUDIO1_SPDIF,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_AUDIO1_SPDIF,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_SPDIF1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_i2c = {
    .name           = "i2c",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_I2C_CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_I2C_CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_i2cm0 = {
    .name           = "i2c0",
    .parent         = &clk_i2c,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_I2C_MASTER0,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_I2C_MASTER0,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_I2C0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_i2cm1 = {
    .name           = "i2c1",
    .parent         = &clk_i2c,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_I2C_MASTER1,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_I2C_MASTER1,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_I2C1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_i2cm2 = {
    .name           = "i2c2",
    .parent         = &clk_i2c,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_I2C_MASTER2,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_I2C_MASTER2,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_I2C2,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_i2cm3 = {
    .name           = "i2c3",
    .parent         = &clk_i2c,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_I2C_MASTER3,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_I2C_MASTER3,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_I2C3,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_i2cs0 = {
    .name           = "i2cs0",
    .parent         = &clk_i2c,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_I2C_SLAVE0,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_I2C_SLAVE0,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_i2cs1 = {
    .name           = "i2cs1",
    .parent         = &clk_i2c,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_I2C_SLAVE1,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_I2C_SLAVE1,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_uart = {
    .name           = "uart",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_UART_CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART_CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_uart0 = {
    .name           = "uart0",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART0,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART0,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_uart1 = {
    .name           = "uart1",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART1,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART1,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_uart2 = {
    .name           = "uart2",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART2,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART2,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART2,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_uart3 = {
    .name           = "uart3",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART3,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART3,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART3,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_uart4 = {
    .name           = "uart4",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART4,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART4,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART4,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_uart5 = {
    .name           = "uart5",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART5,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART5,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART5,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_uart6 = {
    .name           = "uart6",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART6,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART6,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART6,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_uart7 = {
    .name           = "uart7",
    .parent         = &clk_uart,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_UART7,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_UART7,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_UART7,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_ide = {
    .name           = "ide",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_IDE,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_IDE,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_nfc = {
    .name           = "nfc",
    .parent         = &clk_io,
    .flags          = CLK_ALWAYS_ENABLED,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_NFC,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_NFC,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_tsif = {
    .name           = "tsif",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_TSIF_CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_TSIF_CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_tsif0 = {
    .name           = "tsif0",
    .parent         = &clk_tsif,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_TSIF0,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_TSIF0,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_tsif1 = {
    .name           = "tsif1",
    .parent         = &clk_tsif,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_TSIF1,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_TSIF1,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_tsif2 = {
    .name           = "tsif2",
    .parent         = &clk_tsif,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_TSIF2,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_TSIF2,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_gpsb = {
    .name           = "gpsb",
    .parent         = &clk_io,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = RB_GPSB_CONTROLLER,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_GPSB_CONTROLLER,
    .swreset        = tca_ckc_setioswreset,
};

static struct clk clk_gpsb0 = {
    .name           = "gpsb0",
    .parent         = &clk_gpsb,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_GPSB0,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_GPSB0,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_GPSB0,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb1 = {
    .name           = "gpsb1",
    .parent         = &clk_gpsb,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_GPSB1,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_GPSB1,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_GPSB1,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb2 = {
    .name           = "gpsb2",
    .parent         = &clk_gpsb,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_GPSB2,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_GPSB2,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_GPSB2,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb3 = {
    .name           = "gpsb3",
    .parent         = &clk_gpsb,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_GPSB3,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_GPSB3,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_GPSB3,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb4 = {
    .name           = "gpsb4",
    .parent         = &clk_gpsb,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_GPSB4,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx        = RB_GPSB4,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_GPSB4,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_gpsb5 = {
    .name           = "gpsb5",
    .parent         = &clk_gpsb,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_GPSB5,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_GPSB5,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_GPSB5,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_otg = {
    .name           = "usb_otg",
    .parent         = &clk_usbop_phy,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = RB_USB20OTG,
    .pwdn           = tca_ckc_setiopwdn,
    .swreset_idx    = RB_USB20OTG,
    .swreset        = tca_ckc_setioswreset,
    .clock_idx      = PERI_USBOTG,
    .clock          = tca_ckc_pclk_enable,
};


/**************************************/
/* High Speed I/O Bus Clock */

static struct clk clk_dmax = {
    .name           = "dmax",
    .parent         = &clk_hsio,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = HSIOBUS_DMAX,
    .pwdn           = tca_ckc_sethsiopwdn,
    .swreset_idx    = HSIOBUS_DMAX,
    .swreset        = tca_ckc_sethsioswreset,
};

static struct clk clk_gmac_global = {
    .name           = "gmac_global",
    .parent         = &clk_hsio,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = HSIOBUS_GMAC_GLOBAL,
    .pwdn           = tca_ckc_sethsiopwdn,
    .swreset_idx    = HSIOBUS_GMAC_GLOBAL,
    .swreset        = tca_ckc_sethsioswreset,
};

static struct clk clk_gmac = {
    .name           = "gmac",
    .parent         = &clk_gmac_global,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = HSIOBUS_GMAC,
    .pwdn           = tca_ckc_sethsiopwdn,
    .swreset_idx    = HSIOBUS_GMAC,
    .swreset        = tca_ckc_sethsioswreset,
    .clock_idx      = PERI_GMAC,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_usb20host = {
    .name           = "usb20h",
    .parent         = &clk_usbhp_phy,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .set_rate       = pclk_set_rate,
    .get_rate       = pclk_get_rate,
    .pwdn_idx       = HSIOBUS_USB20H,
    .pwdn           = tca_ckc_sethsiopwdn,
    .swreset_idx    = HSIOBUS_USB20H,
    .swreset        = tca_ckc_sethsioswreset,
    .clock_idx      = PERI_USB20H,
    .clock          = tca_ckc_pclk_enable,
};

static struct clk clk_security = {
    .name           = "security",
    .parent         = &clk_hsio,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = HSIOBUS_SEC_CTRL,
    .pwdn           = tca_ckc_sethsiopwdn,
    .swreset_idx    = HSIOBUS_SEC_CTRL,
    .swreset        = tca_ckc_sethsioswreset,
};

static struct clk clk_hsiocfg = {
    .name           = "hsiocfg",
    .parent         = &clk_hsio,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = HSIOBUS_HSIOCFG,
    .pwdn           = tca_ckc_sethsiopwdn,
    .swreset_idx    = HSIOBUS_HSIOCFG,
    .swreset        = tca_ckc_sethsioswreset,
};

static struct clk clk_cipher = {
    .name           = "cipher",
    .parent         = &clk_security,
    .flags          = CLK_AUTO_OFF,
    .enable         = pclk_enable,
    .disable        = pclk_disable,
    .pwdn_idx       = HSIOBUS_CIPHER,
    .pwdn           = tca_ckc_sethsiopwdn,
    .swreset_idx    = HSIOBUS_CIPHER,
    .swreset        = tca_ckc_sethsioswreset,
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

    // IP isolation Control
    &clk_otp_phy,
    &clk_rtc_phy,
    &clk_pll_phy,
    &clk_ecid_phy,
    &clk_hdmi_phy,
    &clk_vdac_phy,
    &clk_tsadc_phy,
    &clk_usbhp_phy,
    &clk_usbop_phy,
    &clk_lvds_phy,

    // Peripheral clocks
    &clk_timerx,
    &clk_timert,
    &clk_timerz,
    &clk_jpeg,
    &clk_out0,
    &clk_out1,

    // Display Bus
    &clk_lcdc,          // bit 1    with lcd0 peri
    &clk_ntscpal,       // bit 2    with lcd1 peri
    &clk_hdmi,          // bit 3    with lcdsi0 peri
    &clk_lcdc0,
    &clk_lcdsi0,
    &clk_lcdc1,
    &clk_lcdsi1,
//    &clk_hdmi_lcdc,
    &clk_lcd_timer,
    &clk_hdmi_audio,
    &clk_ddi,

    // Graphic Bus
    &clk_mali,          // bit 0
    &clk_gpu,

    // IO Bus
    &clk_ehi,           // bit 0    EHI
    &clk_mpefec,        // bit 1    MPEFEC
    &clk_sdmmc0,        // bit 2    SD/SDIO/MMC #0
    &clk_sdmmc1,        // bit 3    SD/SDIO/MMC #1
    &clk_sdmmc2,        // bit 4    SD/SDIO/MMC #2
    &clk_sdmmc3,        // bit 5    SD/SDIO/MMC #3
    &clk_sdmmc,         // bit 6    if SD/SDIO/MMC is used, it should be set
    &clk_dmac0,         // bit 7    DMAC #0
    &clk_dmac1,         // bit 8    DMAC #1
    &clk_dmac2,         // bit 9    DMAC #2
    &clk_dmac,          // bit 10   if DMAC is used, it should be set
    &clk_overlay,       // bit 11   Overlay Mixer
    &clk_pwm,           // bit 12   PWM
    &clk_smc,           // bit 13   SMC
    &clk_rtc,           // bit 15   RTC
    &clk_remote,        // bit 16   Remote Controller
    &clk_tsadc,         // bit 17   TSADC
    &clk_ediconf,       // bit 18   EDI configuration
    &clk_edi,           // bit 19   if EDI is used, it should be set
    &clk_shuldbe1,      // bit 20   should be set
    &clk_adma0,         // bit 21   Audio 0 (MC_ADMA)
    &clk_dam0,
    &clk_dai0,          // bit 22   Audio 0 (MC_DAI)
    &clk_spdif0,        // bit 23   Audio 0 (MC_SPDIF)
    &clk_audio0,        // bit 24   Audio 0. if Audio 0 is used, it should be set
    &clk_adma1,         // bit 25   Audio 1 (ADMA)
    &clk_dam1,
    &clk_dai1,          // bit 26   Audio 1 (DAI)
    &clk_spdif1,        // bit 27   Audio 1 (SPDIF TX)
    &clk_audio1,        // bit 28   if Audio 1 is used, it should be set
    &clk_i2cm0,         // bit 29   I2C Master #0
    &clk_i2cm1,         // bit 30   I2C Master #1
    &clk_i2cm2,         // bit 31   I2C Master #2
    &clk_i2cm3,         // bit 0    I2C Master #3
    &clk_i2cs0,         // bit 1    I2C Slave #0
    &clk_i2cs1,         // bit 2    I2C Slave #1
    &clk_i2c,           // bit 3    if I2C is used, it should be set
    &clk_uart0,         // bit 4    UART #0
    &clk_uart1,         // bit 5    UART #1
    &clk_uart2,         // bit 6    UART #2
    &clk_uart3,         // bit 7    UART #3
    &clk_uart4,         // bit 8    UART #4
    &clk_uart5,         // bit 9    UART #5
    &clk_uart6,         // bit 10   UART #6
    &clk_uart7,         // bit 11   UART #7
    &clk_uart,          // bit 12   if UART is used, it should be set
    &clk_ide,           // bit 13   IDE
    &clk_nfc,           // bit 14   Nand Flash Controller
    &clk_tsif0,         // bit 15   TS I/F #0
    &clk_tsif1,         // bit 16   TS I/F #1
    &clk_tsif2,         // bit 17   TS I/F #2
    &clk_gpsb0,         // bit 18   GPSB #0
    &clk_gpsb1,         // bit 19   GPSB #1
    &clk_gpsb2,         // bit 20   GPSB #2
    &clk_gpsb3,         // bit 21   GPSB #3
    &clk_gpsb4,         // bit 22   GPSB #4
    &clk_gpsb5,         // bit 23   GPSB #5
    &clk_gpsb,          // bit 24   if GPSB is used, it should be set
    &clk_otg,           // bit 25   OTG
    &clk_io,

    // HSIO Bus
    &clk_dmax,          // bit 0    DMAX
    &clk_gmac_global,   // bit 1    Ethernet MAC (GMAC) Global
    &clk_gmac,          // bit 2    Ethernet MAC (GMAC)
    &clk_usb20host,     // bit 6    USB2.0 Host
    &clk_security,      // bit 12   SECURITY Controller (CIPHER)
    &clk_hsiocfg,       // bit 16   HSIO CFG
    &clk_cipher,        // bit 18   Cipher
    &clk_hsio,

    // Video Bus
    &clk_jpege,         // bit 0
    &clk_vcodec,        // bit 2
    &clk_vcache,        // bit 3
    &clk_vbus,
    &clk_vcore,
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
        pr_warning("clock: unimplemented function 'clk_set_rate' called\n");
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
