/*
 * linux/drivers/usb/host/ohci-tcc.c
 *
 * Description: OHCI HCD (Host Controller Driver) for USB.
 *              Bus Glue for Telechips TCCXXX
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2002 David Brownell <dbrownell@users.sourceforge.net>
 * (C) Copyright 2002 Hewlett-Packard Company
 *
 * Bus Glue for Telechips SoC
 *
 * Written by Christopher Hoover <ch@hpl.hp.com>
 * Based on fragments of previous driver by Russell King et al.
 *
 * Modified for LH7A404 from ohci-sa1111.c
 *  by Durgesh Pattamatta <pattamattad@sharpsec.com>
 *
 * Modified for pxa27x from ohci-lh7a404.c
 *  by Nick Bane <nick@cecomputing.co.uk> 26-8-2004
 *
 * Modified for Telechips SoC from ohci-pxa27x.c
 *  by Telechips Linux Team <linux@telechips.com> 03-9-2008
 *
 * This file is licenced under the GPL.
 */

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/signal.h>
#include <linux/platform_device.h>
#include <linux/usb/hcd.h>

#include <asm/mach-types.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <mach/bsp.h>
#include <mach/ohci.h>

extern int tcc_ohci_vbus_init(void);
extern int tcc_ohci_vbus_exit(void);
extern int tcc_ohci_vbus_ctrl(int id, int on);
extern void tcc_ohci_clock_control(int id, int on);

static int tcc_ohci_select_pmm(unsigned long reg_base, int mode, int num_port, int *port_mode)
{
#if !defined(CONFIG_ARCH_TCC892X)
	volatile struct _USBHOST11 *ohci_reg;
	ohci_reg = (volatile struct _USBHOST11 *)reg_base;

	switch (mode)
	{
	case USBOHCI_PPM_NPS:
		/* set NO Power Switching mode */
		ohci_reg->HcRhDescriptorA |= USBOHCI_UHCRHDA_NPS;
		break;
	case USBOHCI_PPM_GLOBAL:
		/* make sure the NO Power Switching mode bit is OFF so Power Switching can occur
		 * make sure the PSM bit is CLEAR, which allows all ports to be controlled with
		 * the GLOBAL set and clear power commands
		 */
		ohci_reg->HcRhDescriptorA &= ~(USBOHCI_UHCRHDA_NPS | USBOHCI_UHCRHDA_PSM_PERPORT);
		break;
	case USBOHCI_PPM_PERPORT:
		/* make sure the NO Power Switching mode bit is OFF so Power Switching can occur
		 * make sure the PSM bit is SET, which allows all ports to be controlled with
		 * the PER PORT set and clear power commands
		 */
		ohci_reg->HcRhDescriptorA &= ~USBOHCI_UHCRHDA_NPS;
		ohci_reg->HcRhDescriptorA |=  USBOHCI_UHCRHDA_PSM_PERPORT;

		/* set the power management mode for each individual port to Per Port. */
		{
			int p;
			for (p = 0; p < num_port; p++)
			{
				ohci_reg->HcRhDescriptorB |= (unsigned int)(1u << (p + 17));   // port 1 begins at bit 17
			}
		}
		break;
	case USBOHCI_PPM_MIXED:
		/* make sure the NO Power Switching mode bit is OFF so Power Switching can occur
		 * make sure the PSM bit is SET, which allows all ports to be controlled with
		 * the PER PORT set and clear power commands
		 */
		ohci_reg->HcRhDescriptorA &= ~USBOHCI_UHCRHDA_NPS;
		ohci_reg->HcRhDescriptorA |=  USBOHCI_UHCRHDA_PSM_PERPORT;

		/* set the power management mode for each individual port to Per Port.
		 * if the value in the PortMode array is non-zero, set Per Port mode for the port.
		 * if the value in the PortMode array is zero, set Global mode for the port
		 */
		{
			int p;
			for (p = 0; p < num_port; p++)
			{
				if (port_mode[p])
				{
					ohci_reg->HcRhDescriptorB |= (unsigned int)(1u << (p + 17));   // port 1 begins at bit 17
				}
				else
				{
					ohci_reg->HcRhDescriptorB &= ~(unsigned int)(1u << (p + 17));  // port 1 begins at bit 17
				}
			}
		}
		break;
	default:
		printk(KERN_ERR "Invalid mode %d, set to non-power switch mode.\n", mode);
		ohci_reg->HcRhDescriptorA |= USBOHCI_UHCRHDA_NPS;
	}

	return 0;

#else
	volatile struct _USB_OHCI *ohci_reg;
	ohci_reg = (volatile struct _USB_OHCI *)reg_base;

	switch (mode)
	{
	case USBOHCI_PPM_NPS:
		/* set NO Power Switching mode */
		ohci_reg->HcRhDescriptorA.nREG |= USBOHCI_UHCRHDA_NPS;
		break;
	case USBOHCI_PPM_GLOBAL:
		/* make sure the NO Power Switching mode bit is OFF so Power Switching can occur
		 * make sure the PSM bit is CLEAR, which allows all ports to be controlled with
		 * the GLOBAL set and clear power commands
		 */
		ohci_reg->HcRhDescriptorA.nREG &= ~(USBOHCI_UHCRHDA_NPS | USBOHCI_UHCRHDA_PSM_PERPORT);
		break;
	case USBOHCI_PPM_PERPORT:
		/* make sure the NO Power Switching mode bit is OFF so Power Switching can occur
		 * make sure the PSM bit is SET, which allows all ports to be controlled with
		 * the PER PORT set and clear power commands
		 */
		ohci_reg->HcRhDescriptorA.nREG &= ~USBOHCI_UHCRHDA_NPS;
		ohci_reg->HcRhDescriptorA.nREG |=  USBOHCI_UHCRHDA_PSM_PERPORT;

		/* set the power management mode for each individual port to Per Port. */
		{
			int p;
			for (p = 0; p < num_port; p++)
			{
				ohci_reg->HcRhDescriptorB.nREG |= (unsigned int)(1u << (p + 17));   // port 1 begins at bit 17
			}
		}
		break;
	case USBOHCI_PPM_MIXED:
		/* make sure the NO Power Switching mode bit is OFF so Power Switching can occur
		 * make sure the PSM bit is SET, which allows all ports to be controlled with
		 * the PER PORT set and clear power commands
		 */
		ohci_reg->HcRhDescriptorA.nREG &= ~USBOHCI_UHCRHDA_NPS;
		ohci_reg->HcRhDescriptorA.nREG |=  USBOHCI_UHCRHDA_PSM_PERPORT;

		/* set the power management mode for each individual port to Per Port.
		 * if the value in the PortMode array is non-zero, set Per Port mode for the port.
		 * if the value in the PortMode array is zero, set Global mode for the port
		 */
		{
			int p;
			for (p = 0; p < num_port; p++)
			{
				if (port_mode[p])
				{
					ohci_reg->HcRhDescriptorB.nREG |= (unsigned int)(1u << (p + 17));   // port 1 begins at bit 17
				}
				else
				{
					ohci_reg->HcRhDescriptorB.nREG &= ~(unsigned int)(1u << (p + 17));  // port 1 begins at bit 17
				}
			}
		}
		break;
	default:
		printk(KERN_ERR "Invalid mode %d, set to non-power switch mode.\n", mode);
		ohci_reg->HcRhDescriptorA.nREG |= USBOHCI_UHCRHDA_NPS;
	}

	return 0;
#endif
}

extern int usb_disabled(void);

/*-------------------------------------------------------------------------*/

/*
 * TCC89/91/92 has two down stream ports.
 * Down stream port1 interfaces with USB1.1 transceiver.
 * Down stream port2 interfaces with USB2.0 OTG PHY.
 * port1 should be power off, because OTG use USB2.0 OTG PHY.
 */
void ohci_port_power_control(int id)
{
	if (id == 1)
	{
		;
	}
	#if !defined(CONFIG_ARCH_TCC892X)
	else
	{
		PUSBHOST11 ohci = (PUSBHOST11)tcc_p2v(HwUSBHOST_BASE);

		/* Port power switch mode & each port is powered individually
		 */
		BITCSET(ohci->HcRhDescriptorA, Hw9 | Hw8, Hw8);	// NPS 0, PSM 1

		/* port2 power state is only affected by per-port power control (Set/ClearPortPower)
		 * port1 is controlled by the global power switch (Set/ClearGlobalPower)
		 */
		BITSET(ohci->HcRhDescriptorB, Hw18);	// PPCM bit2 1

		/* Clear the PortPowerStatus (port2 power off)
		 */
		ohci->HcRhPortStatus2 = Hw9;	// port2 LSDA 1
	}
	#endif
}

static int tcc_start_hc(int id, struct device *dev)
{
	int retval = 0;
	struct tccohci_platform_data *inf;
	inf = dev->platform_data;
	
	if (id == 1)
	{
		;
	}
	#if !defined(CONFIG_ARCH_TCC892X)
	else
	{
		/* USB 1.1 Host Configuration Register
		 * DP, DN pull-down enable
		 */
		PIOBUSCFG pIOBUSCfg;
		pIOBUSCfg = (PIOBUSCFG)tcc_p2v(HwIOBUSCFG_BASE);
		BITSET(pIOBUSCfg->USB11H, Hw4 | Hw3);
	}
	#endif

	tcc_ohci_clock_control(id, 1);

	if (inf->init)
		retval = inf->init(dev);
	if (retval < 0)
		return retval;

	return 0;
}

static void tcc_stop_hc(int id, struct device *dev)
{
	struct tccohci_platform_data *inf;
	
	inf = dev->platform_data;
	if (inf->exit)
		inf->exit(dev);

	tcc_ohci_clock_control(id, 0);
}

/*-------------------------------------------------------------------------*/

/* configure so an HC device and id are always provided */
/* always called with process context; sleeping is OK */


/**
 * usb_hcd_tcc_probe - initialize tcc-based HCDs
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 *
 */
static int usb_hcd_tcc_probe(const struct hc_driver *driver, struct platform_device *pdev)
{
	int retval = 0;
	struct usb_hcd *hcd;
	struct tccohci_platform_data *inf;

	inf = pdev->dev.platform_data;
	if (!inf)
	{
		return -ENODEV;
	}

	if (pdev->resource[1].flags != IORESOURCE_IRQ)
	{
		printk(KERN_ERR "resource[1] is not IORESOURCE_IRQ");
		return -ENOMEM;
	}
	
	hcd = usb_create_hcd(driver, &pdev->dev, "tcc");
	if (!hcd)
		return -ENOMEM;

	hcd->rsrc_start = pdev->resource[0].start;
	hcd->rsrc_len   = pdev->resource[0].end - pdev->resource[0].start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name))
	{
		printk(KERN_ERR "request_mem_region failed");
		retval = -EBUSY;
		goto err1;
	}
	hcd->regs = (void __iomem *)(int)(hcd->rsrc_start);

	tcc_ohci_vbus_init();

	/* USB HOST Power Enable */
	if (tcc_ohci_vbus_ctrl(pdev->id, 1) != 1)
	{
		printk(KERN_ERR "ohci-tcc: USB HOST VBUS failed\n");
		retval = -EIO;
		goto err2;
	}

	if ((retval = tcc_start_hc(pdev->id, &pdev->dev)) < 0)
	{
		printk(KERN_ERR "tcc_start_hc failed");
		goto err2;
	}

	/* Select Power Management Mode */
	tcc_ohci_select_pmm(hcd->rsrc_start, inf->port_mode, 0, 0);

	if (inf->power_budget)
		hcd->power_budget = inf->power_budget;

	ohci_hcd_init(hcd_to_ohci(hcd));

	retval = usb_add_hcd(hcd, pdev->resource[1].start, IRQF_SHARED/*IRQF_DISABLED*/);
	if (retval == 0){
		return retval;
	}
	
	tcc_stop_hc(pdev->id, &pdev->dev);
err2:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err1:
	usb_put_hcd(hcd);
	
	return retval;
}


/* may be called without controller electrically present */
/* may be called with controller, bus, and devices active */

/**
 * usb_hcd_tcc_remove - shutdown processing for tcc-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_hcd_tcc_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 */
static void usb_hcd_tcc_remove(struct usb_hcd *hcd, struct platform_device *pdev)
{
	usb_remove_hcd(hcd);

	tcc_stop_hc(pdev->id, &pdev->dev);

	/* GPIO_EXPAND HVBUS & PWR_GP1 Power-off */
	tcc_ohci_vbus_ctrl(pdev->id, 0);

	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

/*-------------------------------------------------------------------------*/

static int __devinit ohci_tcc_start(struct usb_hcd *hcd)
{
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);
	int ret;
	struct device *dev = hcd->self.controller;
	struct platform_device *pdev = to_platform_device(dev);
		
	/* ignore this - rudals */
	//tcc_set_cken(pdev->id, 1);

	/* The value of NDP in roothub_a is incorrect on this hardware */
	ohci->num_ports = 1;

	if ((ret = ohci_init(ohci)) < 0)
	{
		return ret;
	}
	if ((ret = ohci_run(ohci)) < 0)
	{
		err("can't start %s", hcd->self.bus_name);
		ohci_stop(hcd);
		return ret;
	}

	ohci_port_power_control(pdev->id);

	return 0;
}

/*-------------------------------------------------------------------------*/

static const struct hc_driver ohci_tcc_hc_driver =
{
	.description =			hcd_name,
	.product_desc =		OHCI_PRODUCT_DESC,
	.hcd_priv_size =		sizeof(struct ohci_hcd),

	/*
	* generic hardware linkage
	*/
	.irq =				ohci_irq,
	.flags =				HCD_USB11 | HCD_MEMORY,

	/*
	* basic lifecycle operations
	*/
	.start =				ohci_tcc_start,
	.stop =				ohci_stop,
	.shutdown =			ohci_shutdown,

	/*
	* managing i/o requests and associated device resources
	*/
	.urb_enqueue =		ohci_urb_enqueue,
	.urb_dequeue =		ohci_urb_dequeue,
	.endpoint_disable =	ohci_endpoint_disable,

	/*
	* scheduling support
	*/
	.get_frame_number =	ohci_get_frame,

	/*
	* root hub support
	*/
	.hub_status_data =		ohci_hub_status_data,
	.hub_control =		ohci_hub_control,
#ifdef  CONFIG_PM
	.bus_suspend = 		ohci_bus_suspend,
	.bus_resume =		ohci_bus_resume,
#endif
	.start_port_reset =		ohci_start_port_reset,
};


/*-------------------------------------------------------------------------*/

#ifdef	CONFIG_PM
int ohci_hcd_tcc_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);

	if (time_before(jiffies, ohci->next_statechange))
		msleep(5);
	ohci->next_statechange = jiffies;

	tcc_stop_hc(pdev->id, &pdev->dev);
	hcd->state = HC_STATE_SUSPENDED;

	tcc_ohci_vbus_ctrl(pdev->id, 0);
	tcc_ohci_vbus_exit();

	return 0;
}

static int ohci_hcd_tcc_drv_resume(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);
	int status;
	
	tcc_ohci_vbus_init();

	if(pdev->id == 1) {
		return 0;
	}

	tcc_ohci_vbus_ctrl(pdev->id, 1);

	if (time_before(jiffies, ohci->next_statechange))
		msleep(5);
	ohci->next_statechange = jiffies;

	if ((status = tcc_start_hc(pdev->id, &pdev->dev)) < 0)
	{
		printk(KERN_ERR "[%s] tcc_start_hc error(%d)\n", __func__, status);
		return status;
	}

	ohci_finish_controller_resume(hcd);
	ohci_port_power_control(pdev->id);
	
	return 0;
}
#else
#define ohci_hcd_tcc_drv_suspend		NULL
#define ohci_hcd_tcc_drv_resume			NULL
#endif

static int ohci_hcd_tcc_drv_probe(struct platform_device *pdev)
{
	if (usb_disabled()) {
		return -ENODEV;
	}
	return usb_hcd_tcc_probe(&ohci_tcc_hc_driver, pdev);
}

static int ohci_hcd_tcc_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	usb_hcd_tcc_remove(hcd, pdev);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver ohci_hcd_tcc_driver =
{
	.probe			= ohci_hcd_tcc_drv_probe,
	.remove			= ohci_hcd_tcc_drv_remove,
	.suspend		= ohci_hcd_tcc_drv_suspend,
	.resume			= ohci_hcd_tcc_drv_resume,
	.driver			=
	{
	.name	= "tcc-ohci",
	.owner	= THIS_MODULE,
	},
};


/* work with hotplug and coldplug */
MODULE_ALIAS("platform:tcc-ohci");
