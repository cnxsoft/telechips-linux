/*
 * linux/arch/arm/mach-tcc88xx/devices.
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
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

#include <asm/setup.h>
#include <asm/io.h>
#include <asm/mach/flash.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <mach/bsp.h>
#include <plat/pmap.h>
#include <mach/tcc_fb.h>
#include <mach/ohci.h>
#include <mach/tcc_cam_ioctrl.h>
#include <mach/tccfb_ioctrl.h>

/*----------------------------------------------------------------------
 * Device	  : SD/MMC resource
 * Description: tcc8800_mmc_core0_slot0_resource
 *				tcc8800_mmc_core0_slot1_resource
 *				tcc8800_mmc_core1_slot2_resource
 *				tcc8800_mmc_core1_slot3_resource
 *----------------------------------------------------------------------*/
#if defined(CONFIG_MMC_TCC_SDHC0) || defined(CONFIG_MMC_TCC_SDHC0_MODULE)
static u64 tcc_device_sdhc0_dmamask = 0xffffffffUL;
static struct resource tcc_sdhc0_resource[] = {
	#if defined(CONFIG_MMC_TCC_PORT3)
	[0] = {
		.start	= HwSDMMC3_BASE,
		.end	= HwSDMMC3_BASE + 0x1ff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INT_SD3,
		.end	= INT_SD3,
		.flags	= IORESOURCE_IRQ,
	},
	#else	//if defined(CONFIG_MMC_TCC_PORT7)
	[0] = {
		.start	= HwSDMMC1_BASE,
		.end	= HwSDMMC1_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INT_SD1,
		.end	= INT_SD1,
		.flags	= IORESOURCE_IRQ,
	},
	#endif
};
struct platform_device tcc_sdhc0_device = {
	.name			= "tcc-sdhc0",
	.id 			= 0,
	.num_resources	= ARRAY_SIZE(tcc_sdhc0_resource),
	.resource		= tcc_sdhc0_resource,
	.dev			= {
		.dma_mask			= &tcc_device_sdhc0_dmamask,
		.coherent_dma_mask	= 0xffffffffUL
	}
};
#endif

#if defined(CONFIG_MMC_TCC_SDHC1) || defined(CONFIG_MMC_TCC_SDHC1_MODULE)
static u64 tcc_device_sdhc1_dmamask = 0xffffffffUL;
static struct resource tcc_sdhc1_resource[] = {
	#if defined(CONFIG_MMC_TCC_PORT3)
	[0] = {
		.start	= HwSDMMC1_BASE,
		.end	= HwSDMMC1_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INT_SD1,
		.end	= INT_SD1,
		.flags	= IORESOURCE_IRQ,
	},
	#else	//if defined(CONFIG_MMC_TCC_PORT7)
	[0] = {
		.start	= HwSDMMC3_BASE,
		.end	= HwSDMMC3_BASE + 0x1ff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INT_SD3,
		.end	= INT_SD3,
		.flags	= IORESOURCE_IRQ,
	},
	#endif
};
struct platform_device tcc_sdhc1_device = {
	.name			= "tcc-sdhc1",
	.id 			= 1,
	.num_resources	= ARRAY_SIZE(tcc_sdhc1_resource),
	.resource		= tcc_sdhc1_resource,
	.dev			= {
		.dma_mask			= &tcc_device_sdhc1_dmamask,
		.coherent_dma_mask	= 0xffffffffUL
	}
};
#endif

#if defined(CONFIG_MMC_TCC_SDHC2) || defined(CONFIG_MMC_TCC_SDHC2_MODULE)
static u64 tcc_device_sdhc2_dmamask = 0xffffffffUL;
static struct resource tcc_sdhc2_resource[] = {
	[0] = {
		.start	= HwSDMMC2_BASE,
		.end	= HwSDMMC2_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INT_SD2,
		.end	= INT_SD2,
		.flags	= IORESOURCE_IRQ,
	},
};
struct platform_device tcc_sdhc2_device = {
	.name			= "tcc-sdhc2",
	.id 			= 2,
	.num_resources	= ARRAY_SIZE(tcc_sdhc2_resource),
	.resource		= tcc_sdhc2_resource,
	.dev			= {
		.dma_mask			= &tcc_device_sdhc2_dmamask,
		.coherent_dma_mask	= 0xffffffffUL
	}
};
#endif

#if defined(CONFIG_MMC_TCC_SDHC3) || defined(CONFIG_MMC_TCC_SDHC3_MODULE)
static u64 tcc_device_sdhc3_dmamask = 0xffffffffUL;
static struct resource tcc_sdhc3_resource[] = {
	[0] = {
		.start	= HwSDMMC0_BASE,
		.end	= HwSDMMC0_BASE + 0x1ff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INT_SD0,
		.end	= INT_SD0,
		.flags	= IORESOURCE_IRQ,
	},
};
struct platform_device tcc_sdhc3_device = {
	.name			= "tcc-sdhc3",
	.id 			= 3,
	.num_resources	= ARRAY_SIZE(tcc_sdhc3_resource),
	.resource		= tcc_sdhc3_resource,
	.dev			= {
		.dma_mask			= &tcc_device_sdhc3_dmamask,
		.coherent_dma_mask	= 0xffffffffUL
	}
};
#endif

#if defined(CONFIG_I2C_TCC)
#define I2C_IRQSTR_OFFSET	0xC0	/* only I2C */
#define I2C_ICLK_OFFSET		0x80	/* only SMU_I2C */

/*----------------------------------------------------------------------
 * Device     : I2C resource
 * Description: tcc8800_i2c_core0_resources
 *				tcc8800_i2c_core1_resources
 *				tcc8800_i2c_smu_resources
 *
 * [0] = {										// channel
 *		.name	= "master0"						// channel name
 *		.start	= 100,							// speed (KHz)
 *		.end	= tcc_p2v(HwI2C_CORE0_BASE),	// base address
 *		.flags	= IORESOURCE_IRQ,				// resource type
 * },
 *----------------------------------------------------------------------*/
#if defined(CONFIG_I2C_TCC_CORE0)
static struct resource tcc8800_i2c_core0_resources[] = {
	[0] = {
		.name	= "master0",					/* [GPEPD,CODEC,FMT,EEP,PMIC,CAS,IPOD]_[SCL, SDA] */
		.start  = tcc_p2v(HwI2C_CORE0_BASE),
		.end    = HwI2C_CORE0_BASE + I2C_IRQSTR_OFFSET,
		.flags	= IORESOURCE_IO,
	},
	[1] = {
		.name	= "master1",
		.start  = tcc_p2v(HwI2C_CORE0_BASE) + 0x40,
		.end    = HwI2C_CORE0_BASE + I2C_IRQSTR_OFFSET,
		.flags	= IORESOURCE_IO,
	},
};
struct platform_device tcc8800_i2c_core0_device = {
    .name           = "tcc-i2c",
    .id             = 0,
    .resource       = tcc8800_i2c_core0_resources,
    .num_resources  = ARRAY_SIZE(tcc8800_i2c_core0_resources),
};
#endif
#if defined(CONFIG_I2C_TCC_CORE1)
static struct resource tcc8800_i2c_core1_resources[] = {
	[0] = {
		.name	= "master0",					/* No use in TCC8800 Demo Board_V5.0 */
        .start  = tcc_p2v(HwI2C_CORE1_BASE),
        .end    = HwI2C_CORE1_BASE + I2C_IRQSTR_OFFSET,
		.flags	= IORESOURCE_IO,
    },
	[1] = {
		.name	= "master1",					/* HDMI_[SCL,SDA] */
        .start  = tcc_p2v(HwI2C_CORE1_BASE) + 0x40,
		.end    = HwI2C_CORE1_BASE + I2C_IRQSTR_OFFSET,
		.flags	= IORESOURCE_IO,
    },
};
struct platform_device tcc8800_i2c_core1_device = {
    .name           = "tcc-i2c",
    .id             = 1,
    .resource       = tcc8800_i2c_core1_resources,
    .num_resources  = ARRAY_SIZE(tcc8800_i2c_core1_resources),
};
#endif
#if defined(CONFIG_I2C_TCC_SMU)
static struct resource tcc8800_i2c_smu_resources[] = {
	[0] = {
		.name	= "master0",							/* SATA PHY */
		.start  = tcc_p2v(HwSMUI2C_BASE),
        .end    = HwSMUI2C_BASE + I2C_ICLK_OFFSET,
		.flags	= IORESOURCE_IO,
    },
	[1] = {
		.name	= "master1",							/* HDMI PHY */
        .start  = tcc_p2v(HwSMUI2C_BASE) + 0x40,
        .end    = HwSMUI2C_BASE + I2C_ICLK_OFFSET,
		.flags	= IORESOURCE_IO,
    },
};
struct platform_device tcc8800_i2c_smu_device = {
    .name           = "tcc-i2c",
    .id             = 2,
    .resource       = tcc8800_i2c_smu_resources,
    .num_resources  = ARRAY_SIZE(tcc8800_i2c_smu_resources),
};
#endif
#endif  /* #if defined(CONFIG_I2C_TCC) */

#if defined(CONFIG_TCC_GMAC) || defined(CONFIG_TCC_GMAC_MODULE)

static u64 tcc_gmac_dma_mask = ~(u32)0;
static struct resource tcc_gmac_resources[] = {
	[0] = {
		.start	= tcc_p2v(HwGMAC_BASE),
		.end	= tcc_p2v(HwGMAC_BASE) + 0x2000,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INT_GMAC,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device tcc_gmac_device = {
	.name			= "tcc-gmac",
	.id 			= -1, 
	.dev			= {
		.dma_mask = &tcc_gmac_dma_mask,
		.coherent_dma_mask = 0xffffffffUL,
	},
	.num_resources	= ARRAY_SIZE(tcc_gmac_resources),
	.resource		= tcc_gmac_resources,
};
#endif

#if defined(CONFIG_SERIAL_TCC) || defined(CONFIG_SERIAL_TCC_MODULE)
/*----------------------------------------------------------------------
 * Device     : UART resource
 * Description: uart0_resources
 *              uart1_resources
 *              uart2_resources (not used)
 *              uart3_resources (not used)
 *              uart4_resources (not used)
 *              uart5_resources (not used)
 *----------------------------------------------------------------------*/
static struct resource uart0_resources[] = {
    /* PA -> VA */
    [0] = {
        .start  = tcc_p2v(HwUART0_BASE),
        .end    = tcc_p2v(HwUART0_BASE) + 0xff,
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start  = INT_UART0,
        .end    = INT_UART0,
        .flags  = IORESOURCE_IRQ,
    },
};

struct platform_device tcc8800_uart0_device = {
    .name           = "tcc-uart",
    .id             = 0,
    .resource       = uart0_resources,
    .num_resources  = ARRAY_SIZE(uart0_resources),
};

static struct resource uart1_resources[] = {
    [0] = {
        .start  = tcc_p2v(HwUART1_BASE),
        .end    = tcc_p2v(HwUART1_BASE) + 0xff,
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start  = INT_UART1,
        .end    = INT_UART1,
        .flags  = IORESOURCE_IRQ,
    },
};

struct platform_device tcc8800_uart1_device = {
    .name           = "tcc-uart",
    .id             = 1,
    .resource       = uart1_resources,
    .num_resources  = ARRAY_SIZE(uart1_resources),
};

static struct resource uart2_resources[] = {
    [0] = {
        .start  = tcc_p2v(HwUART2_BASE),
        .end    = tcc_p2v(HwUART2_BASE) + 0xff,
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start  = INT_UART2,
        .end    = INT_UART2,
        .flags  = IORESOURCE_IRQ,
    },
};

struct platform_device tcc8800_uart2_device = {
    .name           = "tcc-uart",
    .id             = 2,
    .resource       = uart2_resources,
    .num_resources  = ARRAY_SIZE(uart2_resources),
};

static struct resource uart3_resources[] = {
    [0] = {
        .start  = tcc_p2v(HwUART3_BASE),
        .end    = tcc_p2v(HwUART3_BASE) + 0xff,
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start  = INT_UART3,
        .end    = INT_UART3,
        .flags  = IORESOURCE_IRQ,
    },
};

struct platform_device tcc8800_uart3_device = {
    .name           = "tcc-uart",
    .id             = 3,
    .resource       = uart3_resources,
    .num_resources  = ARRAY_SIZE(uart3_resources),
};

static struct resource uart4_resources[] = {
    [0] = {
        .start  = tcc_p2v(HwUART4_BASE),
        .end    = tcc_p2v(HwUART4_BASE) + 0xff,
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start  = INT_UART4,
        .end    = INT_UART4,
        .flags  = IORESOURCE_IRQ,
    },
};

struct platform_device tcc8800_uart4_device = {
    .name           = "tcc-uart",
    .id             = 4,
    .resource       = uart4_resources,
    .num_resources  = ARRAY_SIZE(uart4_resources),
};

static struct resource uart5_resources[] = {
    [0] = {
        .start  = tcc_p2v(HwUART5_BASE),
        .end    = tcc_p2v(HwUART5_BASE) + 0xff,
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start  = INT_UART5,
        .end    = INT_UART5,
        .flags  = IORESOURCE_IRQ,
    },
};

struct platform_device tcc8800_uart5_device = {
    .name           = "tcc-uart",
    .id             = 5,
    .resource       = uart5_resources,
    .num_resources  = ARRAY_SIZE(uart5_resources),
};
#endif


/*----------------------------------------------------------------------
 * Device     : USB HOST2.0 EHCI resource
 * Description: tcc8800_ehci_resources
 *----------------------------------------------------------------------*/
#if defined(CONFIG_USB_EHCI_TCC) || defined(CONFIG_USB_EHCI_TCC_MODULE)
static u64 tcc8800_device_ehci_dmamask = 0xffffffffUL;
 
static struct resource tcc8800_ehci_hs_resources[] = {
	[0] = {
		.start = tcc_p2v(HwUSB20HOST_EHCI_BASE),
		.end   = tcc_p2v(HwUSB20HOST_EHCI_BASE) + 0x0fff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_USB20H,
		.end   = INT_USB20H,
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device ehci_hs_device = {
	.name			= "tcc-ehci",
	.id				= -1,
	.num_resources  = ARRAY_SIZE(tcc8800_ehci_hs_resources),
	.resource       = tcc8800_ehci_hs_resources,
	.dev			= {
		.dma_mask 			= &tcc8800_device_ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
		//.platform_data	= tcc8800_ehci_hs_data,
	},
};

static int tcc8800_echi_fs_init(struct device *dev)
{
	return 0;
}

static struct tccohci_platform_data tcc8800_ehci_fs_platform_data = {
	.port_mode	= USBOHCI_PPM_PERPORT,
	.init		= tcc8800_echi_fs_init,
};

static struct resource tcc8800_ehci_fs_resources[] = {
	[0] = {
		.start = tcc_p2v(HwUSB20HOST_OHCI_BASE),
		.end   = tcc_p2v(HwUSB20HOST_OHCI_BASE) + 0x0fff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_USB20H,
		.end   = INT_USB20H,
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device ehci_fs_device = {
	.name			= "tcc-ohci",
	.id				= 1,
	.num_resources  = ARRAY_SIZE(tcc8800_ehci_fs_resources),
	.resource       = tcc8800_ehci_fs_resources,
	.dev			= {
		.dma_mask 			= &tcc8800_device_ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &tcc8800_ehci_fs_platform_data,
	},
};
#else
static inline void tcc8800_init_ehci(void) {}
#endif /*CONFIG_USB_EHCI_TCC  || CONFIG_USB_EHCI_TCC_MODULE*/

/*----------------------------------------------------------------------
 * Device     : USB DWC OTG resource
 * Description: dwc_otg_resources
 *----------------------------------------------------------------------*/
#if defined(CONFIG_TCC_DWC_OTG) || defined(CONFIG_TCC_DWC_OTG_MODULE)
static u64 tcc8800_dwc_otg_dmamask = 0xffffffffUL;
struct resource dwc_otg_resources[] = {
	[0] = {
		.start	= tcc_p2v(HwUSB20OTG_BASE),
		.end	= tcc_p2v(HwUSB20OTG_BASE) + 0x100,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_UOTG,
		.end   = INT_UOTG,
		.flags = IORESOURCE_IRQ,
	},
};

struct platform_device tcc8800_dwc_otg_device = {
	.name			= "dwc_otg",
	.id				= 0,
	.resource		= dwc_otg_resources,
	.num_resources	= ARRAY_SIZE(dwc_otg_resources),
	.dev			= {
		.dma_mask 			= &tcc8800_dwc_otg_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
};
#endif

#if defined(CONFIG_SATA_AHCI) || defined(CONFIG_SATA_AHCI_MODULE)
static struct resource tcc_ahci_resources[] = {
#if defined (CONFIG_TCC_SATA_PHY_0)
		[0] = {
				.start 	= 	tcc_p2v(HwSATAHOST0_BASE),
				.end 	= 	tcc_p2v(HwSATAHOST0_BASE) + 0x1FFF,
				.flags  = 	IORESOURCE_MEM,
		},
		[1] = {
				.start 	= 	INT_SATA0,
				.flags 	= 	IORESOURCE_IRQ,
		}
#elif defined (CONFIG_TCC_SATA_PHY_1)
		[0] = {
				.start	=	tcc_p2v(HwSATAHOST1_BASE),
				.end		=	tcc_p2v(HwSATAHOST1_BASE) + 0x1FFF,
				.flags	=	IORESOURCE_MEM,
		},
		[1] = {
				.start	=	INT_SATA1,
				.flags	=	IORESOURCE_IRQ,
		}
#endif 
};

static int tcc_ahci_data = 0;
struct platform_device tcc_ahci_device = {
		.name 	= 	"ahci",
		.id 	= 	-1,
		.dev 	= 	{
					.platform_data = & tcc_ahci_data,
					.coherent_dma_mask = 0xffffffff,
				},
		.num_resources = ARRAY_SIZE(tcc_ahci_resources),
		.resource 	= tcc_ahci_resources,
};
#endif


#if defined(CONFIG_FB_TCC)
/*----------------------------------------------------------------------
 * Device     : LCD Frame Buffer resource
 * Description: 
 *----------------------------------------------------------------------*/

static u64 tcc_device_lcd_dmamask = 0xffffffffUL;

struct platform_device tcc_lcd_device = {
	.name	  = "tccxxx-lcd",
	.id		  = -1,
	.dev      = {
		.dma_mask		    = &tcc_device_lcd_dmamask,
//		.coherent_dma_mask	= 0xffffffffUL
	}
};
#endif//

int tcc_panel_id = -1;
static int __init parse_tag_tcc_panel(const struct tag *tag)
{
	int *id = (int *) &tag->u;

	tcc_panel_id = *id;

	return 0;
}
__tagtable(ATAG_TCC_PANEL, parse_tag_tcc_panel);

int tcc_is_camera_enable = -1;

static int __init parse_tag_tcc_is_camera_enable(const struct tag *tag)
{
        int *is_camera_enable = (int *) &tag->u;

        tcc_is_camera_enable = *is_camera_enable;

        return 0;
}
__tagtable(ATAG_CAMERA, parse_tag_tcc_is_camera_enable);

struct display_platform_data tcc_display_data = {
	.resolution = 0,
	.output = 0,
	.hdmi_resolution = 0,
	.composite_resolution = 0,
	.component_resolution = 0,
};

//#if(1) // defined(CONFIG_TCC_ADC) || defined(CONFIG_TCC_ADC_MODULE)
/*----------------------------------------------------------------------
 * Device	  : ADC resource
 * Description: tcc8800_adc_resource
 *----------------------------------------------------------------------*/

struct resource tcc8800_adc_resources[] = {
	[0] = {
		.start	= TCC_TSADC_BASE,		
		.end	= TCC_TSADC_BASE + 0x40,	
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device tcc8800_adc_device = {
	.name		= "tcc-adc",
	.id		= -1,
	.resource	= tcc8800_adc_resources,
	.num_resources	= ARRAY_SIZE(tcc8800_adc_resources),
};
//#endif 

#ifdef CONFIG_BATTERY_TCC
struct platform_device tcc_battery_device = {
    .name           = "tcc-battery",
    .id             = -1,
};
#endif /* CONFIG_BATTERY_TCC */

/*----------------------------------------------------------------------
 * Device     : SPI(GPSB) Master resource
 * Description: 
 *----------------------------------------------------------------------*/
#if defined(CONFIG_SPI_TCCXXXX_MASTER)
static struct resource spi0_resources[] = {
	[0] = {
		.start = tcc_p2v(HwGPSBCH0_BASE),
		.end   = tcc_p2v(HwGPSBCH0_BASE + 0x38),
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_GPSB0_DMA,
		.end   = INT_GPSB0_DMA,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {	    
	    .name   = "gpsb0",
#if defined(CONFIG_MACH_TCC8800ST)	
		.start	= 5,/* Port5 GPIO_E[2:3, 6:7] */
		.end	= 5,
#else
		.start	= 11,/* Port11 GPIO_D[5:8] */
		.end	= 11,
#endif
		.flags	= IORESOURCE_IO,
	},
};

struct platform_device tcc8800_spi0_device = {
	.name		= "tcc-spi",
	.id		= 0,
	.resource	= spi0_resources,
	.num_resources	= ARRAY_SIZE(spi0_resources),
};
#endif


/*----------------------------------------------------------------------
 * Device     : RTC resource
 * Description: tcc8800_rtc_resources
 *----------------------------------------------------------------------*/
#if defined(CONFIG_RTC_DRV_TCC8800)
struct resource tcc8800_rtc_resource[] = {
    [0] = {
        .start = 0xF05F2000,
        .end   = 0xF05F20FF,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start  = INT_RTC,
        .end    = INT_RTC,
        .flags  = IORESOURCE_IRQ,
    }
};

struct platform_device tcc8800_rtc_device = {
    .name           = "tcc-rtc",
    .id             = -1,
    .resource       = tcc8800_rtc_resource,
    .num_resources  = ARRAY_SIZE(tcc8800_rtc_resource),
};
#endif  /* CONFIG_RTC_DRV_TCC */


/*----------------------------------------------------------------------
 * Device     : remote control resource
 * Description:
 *----------------------------------------------------------------------*/
#if defined(CONFIG_INPUT_TCC_REMOTE) 
struct resource tcc8800_remote_resources[] = {
#if 1
	[0] = {
		.start	= 0xF05F3000,		
		.end	= 0xF05F3000,	
		.flags	= IORESOURCE_MEM,
	},
    [1] = {
        .start = INT_RMT,
        .end   = INT_RMT,
        .flags = IORESOURCE_IRQ,
    },
#endif
};

struct platform_device tcc8800_remote_device = {
	.name			= "tcc-remote",
	.id				= -1,
	.resource		= tcc8800_remote_resources,
	.num_resources	= ARRAY_SIZE(tcc8800_remote_resources),
};
#if 0
static inline void tcc8800_init_remote(void)
{
	platform_device_register(&tcc8800_remote_device);
}
#endif
#endif /* CONFIG_INPUT_TCC_REMOTE */


/*----------------------------------------------------------------------
 * Device     : SPI(TSIF) Slave resource
 * Description:
 *----------------------------------------------------------------------*/
#if defined(CONFIG_SPI_TCCXXXX_TSIF_SLAVE)
static struct resource tsif_resources[] = {
	[0] = {
		.start = tcc_p2v(HwGPSBCH1_BASE),
		.end   = tcc_p2v(HwGPSBCH1_BASE + 0x38),
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_GPSB1_DMA,
		.end   = INT_GPSB1_DMA,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
	    .name   = "gpsb1",
#if defined(CONFIG_MACH_TCC8800ST)	
		.start	= 11,/* Port11 GPIO_D[5:8] */
		.end	= 11,
#else
		.start	= 5,/* Port5 GPIO_E[2:3, 6:7] */
		.end	= 5,
#endif        

		.flags	= IORESOURCE_IO,
	}
};

struct platform_device tcc_tsif_device = {
	.name		= "tcc-tsif",
	.id		= -1,
	.resource	= tsif_resources,
	.num_resources	= ARRAY_SIZE(tsif_resources),
};

/*----------------------------------------------------------------------
 * Device     : TSIF Block
 * Description:
 *----------------------------------------------------------------------*/
static struct resource tsif_ex_resources[] = {
    [0] = {
		.start = tcc_p2v(HwTSIF0_BASE),
		.end   = tcc_p2v(HwTSIF0_BASE + 0x10000),
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_TSIF0,
		.end   = INT_TSIF0,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
	    .name   = "tsif0",
   		.start	= 5,/* PortX GPIO_X[X:X] */
		.end	= 5,
		.flags	= IORESOURCE_IO,
	}
};

struct platform_device tcc_tsif_ex_device = {
	.name		= "tcc-tsif_ex",
	.id			= -1,
	.resource	= tsif_ex_resources,
	.num_resources	= ARRAY_SIZE(tsif_ex_resources),
};
#endif

#if defined(CONFIG_TCC_ECID_SUPPORT)
struct platform_device tcc_cpu_id_device = {
	.name	= "cpu-id",
	.id		= -1,
};
#endif

#if defined(CONFIG_SND_SOC) || defined(CONFIG_SND_SOC_MODULE)
static struct platform_device tcc_pcm = {
	.name	= "tcc-pcm-audio",
	.id	= -1,
};

static struct platform_device tcc_dai = {
	.name	= "tcc-dai",
	.id	= -1,
};

static struct platform_device tcc_iec958 = {
	.name	= "tcc-iec958",
	.id	= -1,
};

static void tcc_init_audio(void)
{
	platform_device_register(&tcc_pcm);
	platform_device_register(&tcc_dai);
	platform_device_register(&tcc_iec958);
}
#else
static void tcc_init_audio(void){;}
#endif

static int __init tcc88xx_init_devices(void)
{
    tcc_init_audio();
	return 0;
}
arch_initcall(tcc88xx_init_devices);
