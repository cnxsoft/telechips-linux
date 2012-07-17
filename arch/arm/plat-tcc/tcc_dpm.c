/*
 * arch/arm/plat-tcc/tcc_dpm.c  DPM support for Telecips Chips 
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: tcc clock control functions.
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
 * Copyright (C) 2002, 2004 MontaVista Software <source@mvista.com>.
 * Copyright (C) 2008-2009 Telechips 
 *
 * Based on code by Matthew Locke, Dmitry Chigirev, and Bishop Brock.
 */

#include <linux/autoconf.h>
//#include <linux/config.h>  //MCC
#include <linux/dpm.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/pm.h>
#include <linux/delay.h>
#include <linux/err.h>

#include <asm/hardirq.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/system.h>
#include <asm/mach/time.h>

#include <asm/plat-tcc/tcc_ckc.h>
#include <asm/plat-tcc/tca_backlight.h>


#if 0
#define dbg printk
#else /* no debug */
#define dbg(x...) do {} while(0)
#endif

#if 0
#define DEBUG
#endif

static void 
set_sys_clock(int sys_cpu, int sys_bus)
{
    tca_ckc_setclock(0, sys_cpu, sys_bus, 1);
#ifdef DEBUG
    int cpu, bus;
    cpu = tca_ckc_getcurrentsys();
    bus = tca_ckc_getcurrentbus();
    dbg("[%s]cpu/bus = %d/%d\n", __func__, cpu/10000, bus/10000);
#endif
}

static int
get_cpu_clock(void)
{
#ifdef DEBUG
    int cpu = 0;
    cpu = tca_ckc_getcurrentsys();
    dbg("[%s]cpu = %d\n",__func__, cpu);
    return cpu/10000;
#endif
    return tca_ckc_getcurrentsys();
}

static int
get_bus_clock(void)
{
#ifdef DEBUG
    int bus = 0;
    bus = tca_ckc_getcurrentbus();
    dbg("[%s]bus = %d\n", __func__, bus);
    return bus/10000;
#endif
    return tca_ckc_getcurrentbus();
}

static void 
set_lcd_clock(int peri_lcd)
{
    tca_ckc_setperi(PERI_LCD, MHz, peri_lcd, PCDIRECTPLL2);
#ifdef DEBUG
    int lcd = 0;
    printk("input lcd freq = %d\n", peri_lcd);
    lcd = tca_ckc_getperi(PERI_LCD);
    dbg("[%s]lcd = %d\n", __func__, lcd/10000);
#endif
}

/* Get Peri Clock */
static int 
get_lcd_clock(void)
{
#ifdef DEBUG
    int lcd_ckc = 0;
    lcd_ckc = tca_ckc_getperi(PERI_LCD);
    dbg("[%s]lcd = %d\n", __func__, lcd_ckc);
    return lcd_ckc/10000;
#else
    return tca_ckc_getperi(PERI_LCD);
#endif
}


static void 
set_bl_clock(int peri_bl)
{
    tca_bkl_setpowerval(peri_bl);
#ifdef DEBUG
    int bl = 0;
    printk("input bl freq = %d\n", peri_bl);
    bl = tca_bkl_getpowerval();
    dbg("[%s]bl = %d\n", __func__, bl/10000);
#endif
}

/* Get Peri Clock */
static int 
get_bl_clock(void)
{
#ifdef DEBUG
    int bl_ckc = 0;
    bl_ckc  = tca_bkl_getpowerval();
    dbg("[%s]bl = %d\n", __func__, bl_ckc);

    return bl_ckc/10000;
#else
    return tca_bkl_getpowerval();
#endif
}


static void
set_i2c_clock(int peri_i2c)
{
    tca_ckc_setperi(PERI_I2C, MHz, peri_i2c, PCDIRECTXIN);
#ifdef DEBUG
    int i2c = 0;
    i2c = tca_ckc_getperi(PERI_I2C);
    dbg("[%s]i2c = %d\n", __func__, i2c/10000);
#endif
}

static int 
get_i2c_clock(void)
{
#ifdef DEBUG
    int i2c = 0;
    i2c = tca_ckc_getperi(PERI_I2C);
    dbg("[%s]i2c = %d\n", __func__, i2c);

    return i2c/10000;
#else
    return tca_ckc_getperi(PERI_I2C);
#endif
}




static void
set_uart0_clock(int peri_uart0)
{
    tca_ckc_setperi(PERI_UART0, MHz, peri_uart0, PCDIRECTPLL1);
#ifdef DEBUG
    int uart0 = 0;
    uart0 = tca_ckc_getperi(PERI_UART0);
    dbg("[%s]uart0 = %d\n", __func__, uart0/1000);
#endif
}


static void 
set_uart1_clock(int peri_uart1)
{
    tca_ckc_setperi(PERI_UART1, MHz, peri_uart1, PCDIRECTPLL1);
#ifdef DEBUG
    int uart1 = 0;
    uart1 = tca_ckc_getperi(PERI_UART1);
    dbg("[%s]uart1 = %d\n", __func__, uart1/10000);
#endif
}


static void 
set_uart2_clock(int peri_uart2)
{
    tca_ckc_setperi(PERI_UART2, MHz, peri_uart2, PCDIRECTPLL1);
#ifdef DEBUG
    int uart2 = 0;
    uart2 = tca_ckc_getperi(PERI_UART2);
    dbg("[%s]uart2 = %d\n", __func__, uart2/10000);
#endif
}


static void 
set_uart3_clock(int peri_uart3)
{
    tca_ckc_setperi(PERI_UART3, MHz, peri_uart3, PCDIRECTPLL1);
#ifdef DEBUG
    int uart3 = 0;
    uart3 = tca_ckc_getperi(PERI_UART3);
    dbg("[%s]uart3 = %d\n", __func__, uart3/10000);
#endif
}


static int 
get_uart0_clock(void)
{
#ifdef DEBUG
    int uart0 = 0;
    uart0 = tca_ckc_getperi(PERI_UART0);
    dbg("[%s]uart0 = %d\n", __func__, uart0);

    return uart0/10000;
#else
    return tca_ckc_getperi(PERI_UART0);
#endif
}



static int 
get_uart1_clock(void)
{
#ifdef DEBUG
    int uart1 = 0;
    uart1 = tca_ckc_getperi(PERI_UART1);
    dbg("[%s]uart1 = %d\n", __func__, uart1);

    return uart1/10000;
#else
    return tca_ckc_getperi(PERI_UART1);
#endif
}


static int 
get_uart2_clock(void)
{
#ifdef DEBUG
    int uart2 = 0;
    uart2 = tca_ckc_getperi(PERI_UART2);
    dbg("[%s]uart2 = %d\n", __func__, uart2);

    return uart2/10000;
#else
    return tca_ckc_getperi(PERI_UART2);
#endif
}

static int 
get_uart3_clock(void)
{
#ifdef DEBUG
    int uart3 = 0;
    uart3 = tca_ckc_getperi(PERI_UART3);
    dbg("[%s]uart3 = %d\n", __func__, uart3);

    return uart3/10000;
#else
    return tca_ckc_getperi(PERI_UART3);
#endif
}



static void 
set_usb_clock(int peri_usb)
{
    tca_ckc_setperi(PERI_USB11H, MHz, peri_usb, PCCLKFROM48M);
#ifdef DEBUG
    int usb = 0;
    usb = tca_ckc_getperi(PERI_USB11H);
    dbg("[%s]usb = %d\n", __func__, usb/10000);
#endif
}

static int 
get_usb_clock(void)
{
#ifdef DEBUG
    int usb = 0;
    usb = tca_ckc_getperi(PERI_USB11H);
    dbg("[%s]usb = %d\n", __func__, usb);

    return usb/10000;
#else
    return tca_ckc_getperi(PERI_USB11H);
#endif
}

static void 
set_sdmmc_clock(int peri_sdmmc)
{
    tca_ckc_setperi(PERI_SDMMC, MHz, peri_sdmmc, PCDIRECTPLL1);
#ifdef DEBUG
    int sdmmc = 0;

    sdmmc = tca_ckc_getperi(PERI_SDMMC);
    dbg("[%s]sdmmc = %d\n", __func__, sdmmc/10000);
#endif
}

static int 
get_sdmmc_clock(void)
{
#ifdef DEBUG
    int sdmmc = 0;
    sdmmc = tca_ckc_getperi(PERI_SDMMC);
    dbg("[%s]sdmmc = %d\n", __func__, sdmmc);

    return sdmmc/10000;
#else
    return tca_ckc_getperi(PERI_SDMMC);
#endif
}

static void 
set_cam_clock(int peri_cam)
{
    tca_ckc_setperi(PERI_CAM, MHz, peri_cam, PCDIRECTPLL2);
#ifdef DEBUG
    int cam = 0;

    cam = tca_ckc_getperi(PERI_CAM);
    dbg("[%s]cam = %d\n", __func__, cam/10000);
#endif
}


static int 
get_cam_clock(void)
{
#ifdef DEBUG
    int cam = 0;
    cam = tca_ckc_getperi(PERI_CAM);
    dbg("[%s]cam = %d\n", __func__, cam);

    return cam/10000;
#else
    return tca_ckc_getperi(PERI_CAM);
#endif
}


static void 
set_scaler_clock(int peri_scaler)
{
    tca_ckc_setperi(PERI_SCALER, MHz, peri_scaler, PCDIRECTPLL2);
#ifdef DEBUG
    int scaler = 0;

    scaler = tca_ckc_getperi(PERI_SCALER);
    dbg("[%s]scaler = %d\n", __func__, scaler/10000);
#endif
}

static int 
get_scaler_clock(void)
{
#ifdef DEBUG
    int scaler = 0;
    scaler = tca_ckc_getperi(PERI_SCALER);
    dbg("[%s]scaler = %d\n", __func__, scaler);

    return scaler/10000;
#else
    return tca_ckc_getperi(PERI_SCALER);
#endif
}


#if 0
static void 
set_adc_clock(int peri_adc)
{
    int adc = 0;
    tca_ckc_setperi(PERI_ADC, MHz, peri_adc, PCDIRECTXIN);

    adc = tca_ckc_getperi(PERI_ADC);
    dbg("[%s]adc = %d\n", __func__, adc/10000);
}

static int 
get_adc_clock(void)
{
#if 1
    int adc = 0;
    adc = tca_ckc_getperi(PERI_ADC);
    dbg("[%s]adc = %d\n", __func__, adc);

    return adc/10000;
#else
    return tca_ckc_getperi(PERI_ADC);
#endif
}
#endif

#if defined(_TCC79x_)
static void 
set_gpsb0_clock(int peri_gpsb0)
{
    tca_ckc_setperi(PERI_GPSB0, KHz, peri_gpsb0, PCDIRECTPLL0);
#ifdef DEBUG
    int gpsb0 = 0;

    gpsb0 = tca_ckc_getperi(PERI_GPSB0);
    dbg("[%s]gpsb0 = %d\n", __func__, gpsb0/10);
#endif
}

static void 
set_gpsb1_clock(int peri_gpsb1)
{
    tca_ckc_setperi(PERI_GPSB1, KHz, peri_gpsb1, PCDIRECTPLL0);
#ifdef DEBUG
    int gpsb1 = 0;

    gpsb1 = tca_ckc_getperi(PERI_GPSB1);
    dbg("[%s]gpsb1 = %d\n", __func__, gpsb1/10);
#endif
}

#if 0
static int 
set_gpsb2_clock(int peri_gpsb2)
{
    int gpsb2 = 0;
    tca_ckc_setperi(PERI_GPSB2, KHz, peri_gpsb2, PCDIRECTPLL0);

    gpsb2 = tca_ckc_getperi(PERI_GPSB2);
    dbg("[%s]gpsb2 = %d\n", __func__, gpsb2/10);
}

static int 
set_gpsb3_clock(int peri_gpsb3)
{
    int gpsb3 = 0;
    tca_ckc_setperi(PERI_GPSB3, KHz, peri_gpsb3, PCDIRECTPLL0);

    gpsb3 = tca_ckc_getperi(PERI_GPSB3);
    dbg("[%s]gpsb3 = %d\n", __func__, gpsb3/10);
}
#endif

#elif defined(_TCC83x_)
static void 
set_spi0_clock(int peri_spi0)
{
    tca_ckc_setperi(PERI_SPIMS0, KHz, peri_spi0, PCDIRECTPLL0);
#ifdef DEBUG
    int spi0 = 0;

    spi0 = tca_ckc_getperi(PERI_SPIMS0);
    dbg("[%s]spi0 = %d\n", __func__, spi0/10);
#endif
}

static void 
set_spi1_clock(int peri_spi1)
{
    tca_ckc_setperi(PERI_SPIMS1, KHz, peri_spi1, PCDIRECTPLL0);
#ifdef DEBUG
    int spi1 = 0;

    spi1 = tca_ckc_getperi(PERI_SPIMS1);
    dbg("[%s]spi1 = %d\n", __func__, spi1/10);
#endif
}
#endif

#if defined(_TCC79x_)
static int 
get_gpsb0_clock(void)
{
#ifdef DEBUG
    int gpsb0 = 0;
    gpsb0 = tca_ckc_getperi(PERI_GPSB0);
    dbg("[%s]gpsb0 = %d\n", __func__, gpsb0);

    return gpsb0/10000;
#else
    return tca_ckc_getperi(PERI_GPSB0);
#endif
}

static int 
get_gpsb1_clock(void)
{
#ifdef DEBUG
    int gpsb1 = 0;
    gpsb1 = tca_ckc_getperi(PERI_GPSB1);
    dbg("[%s]gpsb1 = %d\n", __func__, gpsb1);

    return gpsb1/10000;
#else
    return tca_ckc_getperi(PERI_GPSB1);
#endif
}

#if 0
static int 
get_gpsb2_clock(void)
{
#if 1
    int gpsb2 = 0;
    gpsb2 = tca_ckc_getperi(PERI_GPSB2);
    dbg("[%s]gpsb2 = %d\n", __func__, gpsb2);

    return gpsb2/10000;
#else
    return tca_ckc_getperi(PERI_GPSB2);
#endif
}

static int 
get_gpsb3_clock(void)
{
#if 1
    int gpsb3 = 0;
    gpsb3 = tca_ckc_getperi(PERI_GPSB3);
    dbg("[%s]gpsb3 = %d\n", __func__, gpsb3);

    return gpsb3/10000;
#else
    return tca_ckc_getperi(PERI_GPSB3);
#endif
}
#endif
#elif defined(_TCC83x_)

static int 
get_spi0_clock(void)
{
#ifdef DEBUG
    int spi0 = 0;
    spi0 = tca_ckc_getperi(PERI_SPIMS0);
    dbg("[%s]spi0 = %d\n", __func__, spi0);

    return spi0/10000;
#else
    return tca_ckc_getperi(PERI_SPIMS0);
#endif
}

static int 
get_spi1_clock(void)
{
#ifdef DEBUG
    int spi1 = 0;
    spi1 = tca_ckc_getperi(PERI_SPIMS1);
    dbg("[%s]spi1 = %d\n", __func__, spi1);

    return spi1/10000;
#else
    return tca_ckc_getperi(PERI_SPIMS1);
#endif
}
#endif


int
dpm_tcc_init_opt(struct dpm_opt *opt)
{
    int sys_cpu  = opt->pp[DPM_MD_SYS_CPU];
    int sys_bus  = opt->pp[DPM_MD_SYS_BUS];
    int peri_lcd = opt->pp[DPM_MD_PERI_LCD];
    int peri_bl = opt->pp[DPM_MD_PERI_BACKLIGHT];
    int peri_i2c = opt->pp[DPM_MD_PERI_I2C];
    int peri_uart0 = opt->pp[DPM_MD_PERI_UART0];
    int peri_uart1 = opt->pp[DPM_MD_PERI_UART1];
    int peri_uart2 = opt->pp[DPM_MD_PERI_UART2];
    int peri_uart3 = opt->pp[DPM_MD_PERI_UART3];
    int peri_usb = opt->pp[DPM_MD_PERI_USB];
    int peri_sdmmc = opt->pp[DPM_MD_PERI_SDMMC];
    int peri_cam = opt->pp[DPM_MD_PERI_CAM];
    int peri_scaler = opt->pp[DPM_MD_PERI_SCALER];
    //int peri_adc = opt->pp[DPM_MD_PERI_ADC];
#if defined(_TCC79x_)
    int peri_gpsb0 = opt->pp[DPM_MD_PERI_GPSB0];
    int peri_gpsb1 = opt->pp[DPM_MD_PERI_GPSB1];
#if 0
    int peri_gpsb2 = opt->pp[DPM_MD_PERI_GPSB2];
    int peri_gpsb3 = opt->pp[DPM_MD_PERI_GPSB3];
#endif
#elif defined(_TCC83x_)
    int peri_spi0 = opt->pp[DPM_MD_PERI_SPIMS0];
    int peri_spi1 = opt->pp[DPM_MD_PERI_SPIMS1];
#endif

    struct dpm_md_opt *md_opt = &opt->md_opt;

    md_opt->sys_cpu = sys_cpu;
    md_opt->sys_bus = sys_bus;
    md_opt->peri_lcd = peri_lcd;
    md_opt->peri_bl = peri_bl;
    md_opt->peri_i2c = peri_i2c;
    md_opt->peri_uart0 = peri_uart0;
    md_opt->peri_uart1 = peri_uart1;
    md_opt->peri_uart2 = peri_uart2;
    md_opt->peri_uart3 = peri_uart3;
    md_opt->peri_usb = peri_usb;
    md_opt->peri_sdmmc = peri_sdmmc;
    md_opt->peri_cam = peri_cam;
    md_opt->peri_scaler = peri_scaler;
    //md_opt->peri_adc = peri_adc;
#if defined(_TCC79x_)
    md_opt->peri_gpsb0 = peri_gpsb0;
    md_opt->peri_gpsb1 = peri_gpsb1;
#if 0
    md_opt->peri_gpsb2 = peri_gpsb2;
    md_opt->peri_gpsb3 = peri_gpsb3;
#endif
#elif defined(_TCC83x_)
    md_opt->peri_spi0 = peri_spi0;
    md_opt->peri_spi1 = peri_spi1;
#endif

    return 0;
}

int
dpm_tcc_set_opt(struct dpm_opt *cur, struct dpm_opt *new)
{

    struct dpm_md_opt *md_cur, *md_new;
    md_cur = &cur->md_opt;
    md_new = &new->md_opt;

    // system cpu and bus clock 
    dbg("[%d]: md_cur->sys_cpu = %d",__LINE__, md_cur->sys_cpu);
    dbg("[%d]: md_new->sys_cpu = %d\n",__LINE__, md_new->sys_cpu);
    dbg("[%d]: md_cur->sys_bus = %d",__LINE__, md_cur->sys_bus);
    dbg("[%d]: md_new->sys_bus = %d\n",__LINE__, md_new->sys_bus);
    if( (md_cur->sys_cpu != md_new->sys_cpu) 
        || (md_cur->sys_bus != md_new->sys_bus))
    {
        set_sys_clock(md_new->sys_cpu, md_new->sys_bus);
        mdelay(500);
    }
    // peri lcd clock 
    dbg("[%d]: md_cur->peri_lcd = %d",__LINE__, md_cur->peri_lcd);
    dbg("[%d]: md_new->peri_lcd = %d\n",__LINE__, md_new->peri_lcd);
    if(md_cur->peri_lcd != md_new->peri_lcd)
    {
        set_lcd_clock(md_new->peri_lcd);
    }

    // peri bl clock 
    dbg("[%d]: md_cur->peri_bl = %d",__LINE__, md_cur->peri_bl);
    dbg("[%d]: md_new->peri_bl = %d\n",__LINE__, md_new->peri_bl);
    if(md_cur->peri_bl != md_new->peri_bl)
    {
        set_bl_clock(md_new->peri_bl);
    }


    // peri i2c clock 
    dbg("[%d]: md_cur->peri_i2c = %d",__LINE__, md_cur->peri_i2c);
    dbg("[%d]: md_new->peri_i2c = %d\n",__LINE__, md_new->peri_i2c);
    if(md_cur->peri_i2c != md_new->peri_i2c)
    {
        set_i2c_clock(md_new->peri_i2c);
    }

    // peri uart0 clock 
    dbg("[%d]: md_cur->peri_uart0 = %d",__LINE__, md_cur->peri_uart0);
    dbg("[%d]: md_new->peri_uart0 = %d\n",__LINE__, md_new->peri_uart0);
    if(md_cur->peri_uart0 != md_new->peri_uart0)
    {
        set_uart0_clock(md_new->peri_uart0);
    }

    // peri uart1 clock 
    dbg("[%d]: md_cur->peri_uart1 = %d",__LINE__, md_cur->peri_uart1);
    dbg("[%d]: md_new->peri_uart1 = %d\n",__LINE__, md_new->peri_uart1);
    if(md_cur->peri_uart1 != md_new->peri_uart1)
    {
        set_uart1_clock(md_new->peri_uart1);
    }

    // peri uart2 clock 
    dbg("[%d]: md_cur->peri_uart2 = %d",__LINE__, md_cur->peri_uart2);
    dbg("[%d]: md_new->peri_uart2 = %d\n",__LINE__, md_new->peri_uart2);
    if(md_cur->peri_uart2 != md_new->peri_uart2)
    {
        set_uart2_clock(md_new->peri_uart2);
    }

    // peri uart3 clock 
    dbg("[%d]: md_cur->peri_uart3 = %d",__LINE__, md_cur->peri_uart3);
    dbg("[%d]: md_new->peri_uart3 = %d\n",__LINE__, md_new->peri_uart3);
    if(md_cur->peri_uart3 != md_new->peri_uart3)
    {
        set_uart3_clock(md_new->peri_uart3);
    }

    // peri usb clock 
    dbg("[%d]: md_cur->peri_usb = %d",__LINE__, md_cur->peri_usb);
    dbg("[%d]: md_new->peri_usb = %d\n",__LINE__, md_new->peri_usb);
    if(md_cur->peri_usb != md_new->peri_usb)
    {
        set_usb_clock(md_new->peri_usb);
    }

    // peri sdmmc clock 
    dbg("[%d]: md_cur->peri_sdmmc = %d",__LINE__, md_cur->peri_sdmmc);
    dbg("[%d]: md_new->peri_sdmmc = %d\n",__LINE__, md_new->peri_sdmmc);
    if(md_cur->peri_sdmmc != md_new->peri_sdmmc)
    {
        set_sdmmc_clock(md_new->peri_sdmmc);
    }

    // peri cam clock 
    dbg("[%d]: md_cur->peri_cam = %d",__LINE__, md_cur->peri_cam);
    dbg("[%d]: md_new->peri_cam = %d\n",__LINE__, md_new->peri_cam);
    if(md_cur->peri_cam != md_new->peri_cam)
    {
        set_cam_clock(md_new->peri_cam);
    }

    // peri scaler clock 
    dbg("[%d]: md_cur->peri_scaler = %d",__LINE__, md_cur->peri_scaler);
    dbg("[%d]: md_new->peri_scaler = %d\n",__LINE__, md_new->peri_scaler);
    if(md_cur->peri_scaler != md_new->peri_scaler)
    {
        set_scaler_clock(md_new->peri_scaler);
    }

#if 0
    // peri adc clock 
    dbg("[%d]: md_cur->peri_adc = %d",__LINE__, md_cur->peri_adc);
    dbg("[%d]: md_new->peri_adc = %d\n",__LINE__, md_new->peri_adc);
    if(md_cur->peri_adc != md_new->peri_adc)
    {
        set_adc_clock(md_new->peri_adc);
    }
#endif

#if defined(_TCC79x_)
    dbg("[%d]: md_cur->peri_gpsb0 = %d",__LINE__, md_cur->peri_gpsb0);
    dbg("[%d]: md_new->peri_gpsb0 = %d\n",__LINE__, md_new->peri_gpsb0);
    if(md_cur->peri_gpsb0 != md_new->peri_gpsb0)
    {
        set_gpsb0_clock(md_new->peri_gpsb0);
    }

    dbg("[%d]: md_cur->peri_gpsb1 = %d",__LINE__, md_cur->peri_gpsb1);
    dbg("[%d]: md_new->peri_gpsb1 = %d\n",__LINE__, md_new->peri_gpsb1);
    if(md_cur->peri_gpsb1 != md_new->peri_gpsb1)
    {
        set_gpsb1_clock(md_new->peri_gpsb1);
    }

#if 0
    dbg("[%d]: md_cur->peri_gpsb2 = %d",__LINE__, md_cur->peri_gpsb2);
    dbg("[%d]: md_new->peri_gpsb2 = %d\n",__LINE__, md_new->peri_gpsb2);
    if(md_cur->peri_gpsb2 != md_new->peri_gpsb2)
    {
        set_gpsb2_clock(md_new->peri_gpsb2);
    }

    dbg("[%d]: md_cur->peri_gpsb3 = %d",__LINE__, md_cur->peri_gpsb3);
    dbg("[%d]: md_new->peri_gpsb3 = %d\n",__LINE__, md_new->peri_gpsb3);
    if(md_cur->peri_gpsb3 != md_new->peri_gpsb3)
    {
        set_gpsb3_clock(md_new->peri_gpsb3);
    }
#endif
#elif defined(_TCC83x_)
    dbg("[%d]: md_cur->peri_spi0 = %d",__LINE__, md_cur->peri_spi0);
    dbg("[%d]: md_new->peri_spi0 = %d\n",__LINE__, md_new->peri_spi0);
    if(md_cur->peri_spi0 != md_new->peri_spi0)
    {
        set_spi0_clock(md_new->peri_spi0);
    }

    dbg("[%d]: md_cur->peri_spi1 = %d",__LINE__, md_cur->peri_spi1);
    dbg("[%d]: md_new->peri_spi1 = %d\n",__LINE__, md_new->peri_spi1);
    if(md_cur->peri_spi1 != md_new->peri_spi1)
    {
        set_spi1_clock(md_new->peri_spi1);
    }
#endif

    return 0;
}

/* Fully determine the current machine-dependent operating point, and fill in a
   structure presented by the caller. This should only be called when the
   dpm_sem is held. This call can return an error if the system is currently at
   an operating point that could not be constructed by dpm_md_init_opt(). */

int
dpm_tcc_get_opt(struct dpm_opt *opt)
{
	struct dpm_md_opt *md_opt = &opt->md_opt;

	md_opt->sys_cpu = get_cpu_clock();
	md_opt->sys_bus = get_bus_clock();
	md_opt->peri_lcd = get_lcd_clock();
	md_opt->peri_bl = get_bl_clock();
	md_opt->peri_i2c = get_i2c_clock();
	md_opt->peri_uart0 = get_uart0_clock();
	md_opt->peri_uart1 = get_uart1_clock();
	md_opt->peri_uart2 = get_uart2_clock();
	md_opt->peri_uart3 = get_uart3_clock();
	md_opt->peri_usb = get_usb_clock();
	md_opt->peri_sdmmc = get_sdmmc_clock();
	md_opt->peri_cam = get_cam_clock();
	md_opt->peri_scaler = get_scaler_clock();
	//md_opt->peri_adc = get_adc_clock();
#if defined(_TCC79x_)
	md_opt->peri_gpsb0 = get_gpsb0_clock();
	md_opt->peri_gpsb1 = get_gpsb1_clock();
#if 0
	md_opt->peri_gpsb2 = get_gpsb2_clock();
	md_opt->peri_gpsb3 = get_gpsb3_clock();
#endif
#elif defined(_TCC83x_)
	md_opt->peri_spi0 = get_spi0_clock();
	md_opt->peri_spi1 = get_spi1_clock();
#endif

	return 0;
}


/****************************************************************************
 *  DPM Idle Handler
 ****************************************************************************/

static void (*orig_idle)(void);

void dpm_tcc_idle(void)
{
	extern void default_idle(void);

	if (orig_idle)
		orig_idle();
	else {
		local_irq_disable();
		if (!need_resched()) {
//			timer_dyn_reprogram();  //MCC
			arch_idle();
		}
		local_irq_enable();
	}
}

/****************************************************************************
 * Initialization/Exit
 ****************************************************************************/

extern void (*pm_idle)(void);

static void
dpm_tcc_startup(void)
{
    dbg("(%s)%s:%d\n", __FILE__, __func__, __LINE__);
    /*
	if (pm_idle != dpm_idle) {
		orig_idle = pm_idle;
		pm_idle = dpm_idle;
	}
    */
    dbg("(%s)%s:%d\n", __FILE__, __func__, __LINE__);
}

static void
dpm_tcc_cleanup(void)
{
    dbg("%s:%d\n", __func__, __LINE__);
	pm_idle = orig_idle;
}

int __init
dpm_tcc_init(void)
{
	printk("Telechips Dynamic Power Management.\n");
	dpm_md.init_opt		= dpm_tcc_init_opt;
	dpm_md.set_opt		= dpm_tcc_set_opt;
	dpm_md.get_opt		= dpm_tcc_get_opt;
	dpm_md.check_constraint	= dpm_default_check_constraint;
	dpm_md.idle		= dpm_tcc_idle;
	dpm_md.startup		= dpm_tcc_startup;
	dpm_md.cleanup		= dpm_tcc_cleanup;

	return 0;
}

__initcall(dpm_tcc_init);

