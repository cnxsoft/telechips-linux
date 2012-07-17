/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/dwc_otg_driver.c $
 * $Revision: #71 $
 * $Date: 2009/02/04 $
 * $Change: 1179630 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

/** @file
 * The dwc_otg_driver module provides the initialization and cleanup entry
 * points for the DWC_otg driver. This module will be dynamically installed
 * after Linux is booted using the insmod command. When the module is
 * installed, the dwc_otg_driver_init function is called. When the module is
 * removed (using rmmod), the dwc_otg_driver_cleanup function is called.
 *
 * This module also defines a data structure for the dwc_otg_driver, which is
 * used in conjunction with the standard ARM lm_device structure. These
 * structures allow the OTG driver to comply with the standard Linux driver
 * model in which devices and drivers are registered with a bus driver. This
 * has the benefit that Linux can expose attributes of the driver and device
 * in its special sysfs file system. Users can then read or write files in
 * this file system to perform diagnostics on the driver components or the
 * device.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/stat.h>		/* permission constants */
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/usb.h>
#if defined(CONFIG_REGULATOR)
#include <linux/regulator/consumer.h>
#endif

#include <asm/io.h>
#include <asm/sizes.h>
#include <asm/mach-types.h>
#include <asm/gpio.h>

#include "dwc_os.h"
#include "dwc_otg_dbg.h"
#include "dwc_otg_attr.h"
#include "dwc_otg_driver.h"
#include "dwc_otg_core_if.h"
#include "dwc_otg_pcd_if.h"
#include "dwc_otg_hcd_if.h"
#include "dwc_otg_regs.h"
#include "dwc_otg_cil.h"

#include "tcc_usb_phy.h"
#include "tcc_usb_def.h"

#include <mach/tcc_board_power.h>
#include <mach/tcc_pca953x.h>
#include <linux/tcc_pwm.h>
#include <linux/cpufreq.h>
#include <linux/wakelock.h>



#define DWC_DRIVER_VERSION	"2.81a 04-FEB-2009"
#define DWC_DRIVER_DESC		"HS OTG USB Controller driver"


static const char dwc_driver_name[] = "dwc_otg";

extern void trace_usb_flow(int on);
extern void tcc_otg_vbus_init(void);
extern void tcc_otg_vbus_exit(void);
extern void tcc_ohci_clock_control(int id, int on);
extern int pcd_init(struct platform_device *_dev);
extern int hcd_init(struct platform_device *_dev);
extern int pcd_remove(struct platform_device *_dev);
extern void hcd_remove(struct platform_device *_dev);
extern struct tcc_freq_table_t gtUSBClockLimitTable[];
#if !defined(CONFIG_ARCH_TCC92XX)
extern const struct tcc_freq_table_t gtHSIOClockLimitTable;
#endif

#define OTG_DBG(msg...) do{ printk( KERN_ERR "DWC_OTG : " msg ); }while(0)

#if defined(CONFIG_STB_BOARD_HDB892S)
	#define OTG0_ID		TCC_GPF(13)
	#define LED_F_PN	TCC_GPB(24)
#elif defined(CONFIG_STB_BOARD_HDB892F)
	#define OTG0_ID		TCC_GPD(13)
	#define LED_F_PN	TCC_GPF(03)	
#endif

#if defined(CONFIG_TCC_USB_TO_SATA)
#define USB30_EN		TCC_GPF(10)
#define USB30_VBUS_DET	TCC_GPD(13)
#endif

/*
 * For using gadget_wrapper of dwc_otg_pcd_linux.c
 */
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
struct gadget_wrapper {
#ifndef DWC_HOST_ONLY
	dwc_otg_pcd_t *pcd;
#endif
	struct usb_gadget gadget;
	struct usb_gadget_driver *driver;

	struct usb_ep ep0;
	struct usb_ep in_ep[16];
	struct usb_ep out_ep[16];
};
extern struct gadget_wrapper *gadget_wrapper;
#if defined(CONFIG_TCC_DWC_OTG_V281A)
dwc_otg_device_t *dwc_otg_devices;
#endif
/* For Signature */
#define DWC_OTG_DRIVER_SIGNATURE			'D','W','C','_','O','T','G','_','D','R','I','V','E','R','_'
#define DWC_OTG_DRIVER_VERSION				'V','2','.','0','0','2'
static const unsigned char DWC_OTG_DRIVER_C_Version[] =
    {SIGBYAHONG, DWC_OTG_DRIVER_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, DWC_OTG_DRIVER_VERSION, 0};

/*-------------------------------------------------------------------------*/
/* Encapsulate the module parameter settings */

struct dwc_otg_driver_module_params {
	int32_t opt;
	int32_t otg_cap;
	int32_t dma_enable;
	int32_t dma_desc_enable;
	int32_t dma_burst_size;
	int32_t speed;
	int32_t host_support_fs_ls_low_power;
	int32_t host_ls_low_power_phy_clk;
	int32_t enable_dynamic_fifo;
	int32_t data_fifo_size;
	int32_t dev_rx_fifo_size;
	int32_t dev_nperio_tx_fifo_size;
	uint32_t dev_perio_tx_fifo_size[MAX_PERIO_FIFOS];
	int32_t host_rx_fifo_size;
	int32_t host_nperio_tx_fifo_size;
	int32_t host_perio_tx_fifo_size;
	int32_t max_transfer_size;
	int32_t max_packet_count;
	int32_t host_channels;
	int32_t dev_endpoints;
	int32_t phy_type;
	int32_t phy_utmi_width;
	int32_t phy_ulpi_ddr;
	int32_t phy_ulpi_ext_vbus;
	int32_t i2c_enable;
	int32_t ulpi_fs_ls;
	int32_t ts_dline;
	int32_t en_multiple_tx_fifo;
	uint32_t dev_tx_fifo_size[MAX_TX_FIFOS];
	uint32_t thr_ctl;
	uint32_t tx_thr_length;
	uint32_t rx_thr_length;
	int32_t lpm_enable;			//alan.K
	int32_t pti_enable;
	int32_t mpi_enable;
	/*int32_t lpm_enable;*/		//alan.K
	int32_t ic_usb_cap;
	int32_t ahb_thr_ratio;
};

static struct dwc_otg_driver_module_params dwc_otg_module_params = {
	.opt = -1,
	.otg_cap = -1,
	.dma_enable = -1,
	.dma_desc_enable = -1,
	.dma_burst_size = -1,
	.speed = -1,
	.host_support_fs_ls_low_power = -1,
	.host_ls_low_power_phy_clk = -1,
	.enable_dynamic_fifo = -1,
	.data_fifo_size = -1,
	.dev_rx_fifo_size = -1,
	.dev_nperio_tx_fifo_size = -1,
	.dev_perio_tx_fifo_size = {
				   /* dev_perio_tx_fifo_size_1 */
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1
				   /* 15 */
				   },
	.host_rx_fifo_size = -1,
	.host_nperio_tx_fifo_size = -1,
	.host_perio_tx_fifo_size = -1,
	.max_transfer_size = -1,
	.max_packet_count = -1,
	.host_channels = -1,
	.dev_endpoints = -1,
	.phy_type = -1,
	.phy_utmi_width = -1,
	.phy_ulpi_ddr = -1,
	.phy_ulpi_ext_vbus = -1,
	.i2c_enable = -1,
	.ulpi_fs_ls = -1,
	.ts_dline = -1,
	.en_multiple_tx_fifo = -1,
	.dev_tx_fifo_size = {
			     /* dev_tx_fifo_size */
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1
			     /* 15 */
			     },
	.thr_ctl = -1,
	.tx_thr_length = -1,
	.rx_thr_length = -1,
	.pti_enable = -1,
	.mpi_enable = -1,
	.lpm_enable = -1,
	.ic_usb_cap = -1,
	.ahb_thr_ratio = -1,
};

#ifndef DWC_HOST_ONLY
extern const unsigned char* dwc_otg_cil_get_version(void);
extern const unsigned char* dwc_otg_cil_intr_get_version(void);
extern const unsigned char* dwc_otg_pcd_get_version(void);
extern const unsigned char* dwc_otg_pcd_intr_get_version(void);
#endif


#define TCC_OTG_WAKE_LOCK
#define TCC_OTG_WORK_DELAY_TIME		20000	//20 second

#if defined(TCC_OTG_WAKE_LOCK)
#include <linux/mutex.h>
DEFINE_MUTEX( otg_mutex );

#define MAX_OTG_NUM 2
typedef void (*pOtgDelayWork)(struct work_struct *work);
static void otg0_delayed_work(struct work_struct *work);
static void otg1_delayed_work(struct work_struct *work);

static struct wake_lock otg_wake_lock[MAX_OTG_NUM];
static struct delayed_work otg_wakelock_delayed_work[MAX_OTG_NUM];
static pOtgDelayWork otg_delayed_work[MAX_OTG_NUM] = {otg0_delayed_work, otg1_delayed_work};

static int otg_wakelock_init(void)
{
	int i;
	printk("wakelock init for otg\n");
	for(i=0 ; i<MAX_OTG_NUM ; i++){
		wake_lock_init(&otg_wake_lock[i], WAKE_LOCK_SUSPEND, "otg_wake_lock");
		INIT_DELAYED_WORK(&otg_wakelock_delayed_work[i], otg_delayed_work[i]);
	}
	return 0;
}

static int otg_wakelock_exit(void)
{
	int i;
	printk("wakelock exit for otg\n");
	for(i=0 ; i<MAX_OTG_NUM ; i++)
	{
		flush_delayed_work(&otg_wakelock_delayed_work[i]);
		wake_lock_destroy(&otg_wake_lock[i]);
	}
	return 0;
}

static int otg_wakelock_set(int port_index)
{
	printk("set wakelock for otg%d\n", port_index);
	if(port_index >= MAX_OTG_NUM){
		printk("[%s] Invalid port num (%d)\n", __func__, port_index);
		return -1;
	}
	
	wake_lock(&otg_wake_lock[port_index]);
	schedule_delayed_work(&otg_wakelock_delayed_work[port_index], \
		msecs_to_jiffies(TCC_OTG_WORK_DELAY_TIME));
	
	return 0;
}

static void otg0_delayed_work(struct work_struct *work)
{
	printk("unset wakelock for otg0\n");
	cancel_delayed_work(&otg_wakelock_delayed_work[0]);
	wake_unlock(&otg_wake_lock[0]);
}

static void otg1_delayed_work(struct work_struct *work)
{
	printk("unset wakelock for otg1\n");	
	cancel_delayed_work(&otg_wakelock_delayed_work[1]);
	wake_unlock(&otg_wake_lock[1]);
}

#endif /* TCC_OTG_WAKE_LOCK */

int USB_FLAG;
static struct notifier_block *notifier = NULL;

void usb_register_notifier(struct notifier_block *nb)
{
	//	blocking_notifier_chain_register(&usb_notifier_list, nb);
	notifier = nb;
}
EXPORT_SYMBOL_GPL(usb_register_notifier);

/**
 * This function shows the Driver Version.
 */
static ssize_t version_show(struct device_driver *dev, char *buf)
{
#ifdef DWC_HOST_ONLY
	return snprintf(buf, sizeof(DWC_DRIVER_VERSION)+2,"%s\n",
	                DWC_DRIVER_VERSION);
#else
	return snprintf(buf, PAGE_SIZE,"%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n"
	                ,DWC_DRIVER_VERSION
	                ,DWC_OTG_DRIVER_C_Version
	                ,dwc_otg_cil_get_version()
	                ,dwc_otg_cil_intr_get_version()
	                ,dwc_otg_pcd_get_version()
	                ,dwc_otg_pcd_intr_get_version()
	                ,USBPHY_GetVersion());
#endif
}

static DRIVER_ATTR(version, S_IRUGO, version_show, NULL);

/**
 * Global Debug Level Mask.
 */
uint32_t g_dbg_lvl = DBG_OFF;		/* OFF */
//uint32_t g_dbg_lvl = DBG_ANY | DBG_HCD_URB;

/**
 * This function shows the driver Debug Level.
 */
static ssize_t dbg_level_show(struct device_driver *drv, char *buf)
{
	return sprintf(buf, "0x%0x\n", g_dbg_lvl);
}

/**
 * This function stores the driver Debug Level.
 */
static ssize_t dbg_level_store(struct device_driver *drv, const char *buf,
			       size_t count)
{
	g_dbg_lvl = simple_strtoul(buf, NULL, 16);
	return count;
}

static DRIVER_ATTR(debuglevel, S_IRUGO | S_IWUSR, dbg_level_show,
		   dbg_level_store);

static void tcc_usb_link_reset(dwc_otg_core_if_t *_core_if)
{
#if defined(CONFIG_ARCH_TCC93XX)
	if(_core_if->port_index!=0)
	{
		tca_ckc_sethsiobusswreset(HSIO_USBOTG, ON);
	}
	else
#endif
	{
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
		tca_ckc_setioswreset(RB_USB20OTG, ON);
#else
		tca_ckc_set_iobus_swreset(RB_USB20OTG, OFF);
#endif
	}

	{
		volatile unsigned int t=1000;
		while (t-->0);
	}

#if defined(CONFIG_ARCH_TCC93XX)
	if(_core_if->port_index!=0)
	{
		tca_ckc_sethsiobusswreset(HSIO_USBOTG, OFF);
	}
	else
#endif
	{
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
		tca_ckc_setioswreset(RB_USB20OTG, OFF);
#else
		tca_ckc_set_iobus_swreset(RB_USB20OTG, ON);
#endif
	}
}

static void tcc_set_vbus(dwc_otg_core_if_t *_core_if)
{
#if 1
	hprt0_data_t hprt0;
	OTG_DBG("[%s] vbus_state:%d\n", __func__, _core_if->vbus_state);
	
	if(_core_if->vbus_state)
	{
		tcc_otg_vbus_ctrl(_core_if->port_index,1);
		// wait the voltage to be stable

		msleep_interruptible(100);

		/* Control the port power bit. */
		hprt0.d32 = dwc_otg_read_hprt0( _core_if );
		hprt0.b.prtpwr = 1;
		dwc_write_reg32(_core_if->host_if->hprt0, hprt0.d32);
	}
	else
	{
		gusbcfg_data_t gusbcfg = { .d32 = 0 };
		gusbcfg_data_t gusbcfg_backup = { .d32 = 0 };
		gusbcfg_backup.d32 = dwc_read_reg32(&_core_if->core_global_regs->gusbcfg);
		if(_core_if->vbus_off_force)
		{
			gusbcfg.b.srpcap = 1;
			// disable SRP during VBUS off. (OTG compliance test)
			dwc_modify_reg32(&_core_if->core_global_regs->gusbcfg,gusbcfg.d32,0);
		}

		/* Control the port power bit. */
		hprt0.d32 = dwc_otg_read_hprt0( _core_if );
		hprt0.b.prtpwr = 0;
		dwc_write_reg32(_core_if->host_if->hprt0, hprt0.d32);

		tcc_otg_vbus_ctrl(_core_if->port_index, 0);

		if(_core_if->vbus_off_force)
		{
			msleep_interruptible(500);
			// enable SRP during VBUS off. (OTG compliance test)
			if(gusbcfg_backup.b.srpcap)
				dwc_modify_reg32(&_core_if->core_global_regs->gusbcfg,0,gusbcfg.d32);
		}
		_core_if->vbus_off_force = 0;
	}
#else
	hprt0_data_t hprt0;

	tcc_otg_vbus_ctrl(_core_if->port_index, (int)_core_if->vbus_state);
	// wait the voltage to be stable

	msleep_interruptible(100);

	/* Control the port power bit. */
	hprt0.d32 = dwc_otg_read_hprt0( _core_if );
	hprt0.b.prtpwr = (unsigned)_core_if->vbus_state;
	dwc_write_reg32(_core_if->host_if->hprt0, hprt0.d32);
#endif
}

static void tcc_vbus_work(struct work_struct *work)
{
	dwc_otg_core_if_t *_core_if = container_of(work, dwc_otg_core_if_t, vbus_work);
	tcc_set_vbus(_core_if);
}

/**
 * This function shows the file_storage gadget attach/detach status
 */
static ssize_t fsg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct platform_device *pdev = to_platform_device(dev);
	dwc_otg_device_t *dwc_otg_device = platform_get_otgdata(pdev);
	return sprintf(buf, "%d\n", dwc_otg_device->flagDeviceAttach);
}
static DEVICE_ATTR(fsg, S_IRUGO, fsg_show, NULL);

#ifdef CONFIG_MACH_TCC8900
#define DWC_SET_MODE
#ifdef DWC_SET_MODE
static ssize_t read_mode(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct platform_device *pdev = to_platform_device(dev);
	dwc_otg_device_t *dwc_otg_device = platform_get_otgdata(pdev);
	return sprintf(buf, "%d\n", dwc_otg_device->flagMode);
}
static ssize_t set_mode(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct platform_device *pdev = to_platform_device(dev);
	dwc_otg_device_t *dwc_otg_device = platform_get_otgdata(pdev);
	unsigned long mode;
	mode = simple_strtoul(buf, (char **)NULL, 10);
	dwc_otg_device->flagMode = (uint8_t)mode;
	return count;
}
static DEVICE_ATTR(setmode, S_IRUSR|S_IWUSR, read_mode, set_mode);
#endif

#if 0
/*
 * OTG Power on/off control
 * TODO: Handling DWC_HOST_ONLY/DWC_DEVICE_ONLY, 
 *       this code only control DWC_DUAL_ROLE.
 */
static stpwrinfo pwrinfo = {PWR_STATUS_ON};
static int otg_pwr_ctl(void *h_private, int cmd, void *p_out)
{
	dwc_otg_device_t *dwc_otg_device = (dwc_otg_device_t *)h_private;

	switch (cmd) {
	case PWR_CMD_OFF:
		printk(KERN_DEBUG "PWR_CMD_OFF command ==> [%d]\n", cmd);
		if (pwrinfo.status == PWR_STATUS_OFF) {
			return 0;
		}
		pwrinfo.status = PWR_STATUS_OFF;

		dwc_otg_device->flagMode_backup = dwc_otg_device->flagMode;
		
		dwc_otg_device->flagMode = 1;
		/* wait for change mode */
		while (dwc_otg_device->flagID == 0) {
			msleep_interruptible(200);
		}
		msleep_interruptible(200);

		down(&dwc_otg_device->vbus_usb_task_lock);
		
		USBPHY_SetMode(dwc_otg_device->core_if, USBPHY_MODE_OFF);

		break;
	case PWR_CMD_ON:
		printk(KERN_DEBUG "PWR_CMD_ON command ==> [%d]\n", cmd);
		if (pwrinfo.status == PWR_STATUS_ON) {
			return 0;
		}
		pwrinfo.status = PWR_STATUS_ON;

		USBPHY_SetMode(dwc_otg_device->core_if, USBPHY_MODE_ON);

		dwc_otg_device->flagMode = 1;

		up(&dwc_otg_device->vbus_usb_task_lock);
		
		/* wait for change mode */
		while (dwc_otg_device->flagID == 0) {
			msleep_interruptible(200);
		}
		msleep_interruptible(200);

		dwc_otg_device->flagMode = dwc_otg_device->flagMode_backup;

		break;
	case PWR_CMD_GETSTATUS:
		printk(KERN_DEBUG "PWR_CMD_GETSTATUS command ==> [%d], status:[%d]\n", cmd, pwrinfo.status);
		memcpy(p_out, &pwrinfo, sizeof(stpwrinfo));
		break;
	default:
		printk(KERN_DEBUG "unknown pwr command !!! ==> [%d]\n", cmd);
		return -EINVAL;
		break;
	}

    return 0;
}
#endif /* 0 */
#endif	/*CONFIG_MACH_TCC8900*/

static void InitID(void)
{
#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900() || machine_is_m57te() )
	{
		//*********************************
		//		GPIO_A13 for ID pin
		//*********************************
		// Port Configuration : GPIO_A13 -> GPIO
		BITCSET(HwGPIO->GPAFN1, (0xF/*mask*/)<<20, (0/*GPIO*/)<<20);
		// Direction : input mode
		BITCLR(HwGPIO->GPAEN ,Hw13);
		// Pull UP/DOWN : pull-up only
		BITCSET(HwGPIO->GPAPD0, 0x3/*mask*/<<26, 0x1/*pull-up only*/<<26);
	}
#endif
}

static int IsID(dwc_otg_device_t *dwc_otg_device)
{
#ifdef CONFIG_MACH_TCC8900
#ifdef DWC_SET_MODE
	int ret;
	if (dwc_otg_device->flagMode == 0) {
		ret = 0;
	} else if (dwc_otg_device->flagMode == 1) {
		ret = 1;
	} else {
		ret = (HwGPIO->GPADAT & Hw13);
	}
	return ret;
#else //DWC_SET_MODE
	return ( HwGPIO->GPADAT & Hw13 );
#endif
#else
	gotgctl_data_t gotgctrl;
	gotgctrl.d32 = dwc_read_reg32(&dwc_otg_device->core_if->core_global_regs->gotgctl);
	return  (int)gotgctrl.b.conidsts;
#endif
}

static inline unsigned int GetConnIdStatus(dwc_otg_core_if_t *_core_if)
{
	volatile unsigned int status = 0;

#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900() || machine_is_m57te())
	{
		status = (( HwGPIO->GPADAT & Hw13 )>>13);	/* 0:host, 1:device */
	}
	else
#endif
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	{
		gintsts_data_t gintsts;
		gintsts.d32 = dwc_read_reg32(&_core_if->core_global_regs->gintsts);
		status = (gintsts.b.curmode==1)?0:1;		/* 0:host, 1:device */
	}
#endif
	return status;
}

static void InitUSBDET(dwc_otg_core_if_t *_core_if)
{
#if !defined(CONFIG_ARCH_TCC892X)
	PGPIO pGPIO = (PGPIO)tcc_p2v(HwGPIO_BASE);
#endif

#if defined(CONFIG_ARCH_TCC88XX)
	if (machine_is_tcc8800() || machine_is_tcc8800st()) {
		BITCSET(pGPIO->EINTSEL0, 0x7F, EXT_INTR_SRC_USB0_VBON);
	}
#endif
#if defined(CONFIG_ARCH_TCC93XX)
	if (machine_is_tcc9300() || machine_is_tcc9300cm() || machine_is_tcc9300st())
		if(_core_if->port_index == 0){
			BITCSET(pGPIO->EINTSEL0, 0x7F, EXT_INTR_SRC_USB0_VBON);
		}
		else{
			BITCSET(pGPIO->EINTSEL0, 0x7F<<8, EXT_INTR_SRC_USB1_VBON<<8);
		}
	}
#endif
#if defined(CONFIG_ARCH_TCC92XX)
	if (machine_is_tcc8900()) {
		BITCSET(pGPIO->EINTSEL0, (Hw6-Hw0), EXT_INTR_SRC_USB_VBON);
	}
#endif
}

static int IsUSBDET(dwc_otg_core_if_t *_core_if)
{
	PPIC pPIC = (PPIC)tcc_p2v(HwPIC_BASE);

#if defined(CONFIG_ARCH_TCC88XX)
	if (machine_is_tcc8800() || machine_is_tcc8800st() || machine_is_m801_88())
		return (pPIC->STS0 & (1<<INT_EI0));
#endif
#if defined(CONFIG_ARCH_TCC93XX)
	if (machine_is_tcc9300() || machine_is_tcc9300cm() || machine_is_tcc9300st())
		return (pPIC->STS0 & (1<<((_core_if->port_index == 0)?INT_EI0:INT_EI1)));
#endif
#if defined(CONFIG_ARCH_TCC892X)
	if (machine_is_tcc8920() || machine_is_tcc8920st() || machine_is_m805_892x())
		return (pPIC->STS1.bREG.USBVBON);
#endif
#if defined(CONFIG_ARCH_TCC92XX)
	return (pPIC->STS0 & (1<<INT_EI0));
#endif
}

static int usb_state_notification(dwc_otg_core_if_t *_core_if, unsigned int state)
{
	if(notifier && notifier->notifier_call)
	{
	#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)		
		if(GetConnIdStatus(_core_if) == 1)
	#endif
		{
			notifier->notifier_call(0, state, 0);
			printk("usb %s.\n", (state == USB_BUS_ADD)?"connected":"disconnected");
			return 0;
		}
	}

	/* waiting until notification callback function registered. */
	return -1;
}

static int tcc_usb_thread(void* _dwc_otg_device)
{
	int ret = -1;
	dwc_otg_device_t *dwc_otg_device = _dwc_otg_device;

	DWC_DEBUGPL(DBG_ANY, "%s starts...\n", __func__);

	InitID();
	InitUSBDET(dwc_otg_device->core_if);

	dwc_otg_device->flagDeviceVBUS = -1;
	dwc_otg_device->flagID = -1;
	dwc_otg_device->flagDeviceAttach = 0;

	#if defined(CONFIG_STB_BOARD_HDB892S) || defined(CONFIG_STB_BOARD_HDB892F)
		tcc_gpio_config(OTG0_ID, GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);
		tcc_gpio_config(LED_F_PN, GPIO_FN(0)|GPIO_OUTPUT|GPIO_HIGH);
		gpio_set_value(OTG0_ID, 0);
		gpio_set_value(LED_F_PN, 1);
	#endif
	
	#if defined(CONFIG_TCC_USB_TO_SATA)
		tcc_gpio_config(USB30_EN, GPIO_FN(0)|GPIO_OUTPUT|GPIO_LOW);
		tcc_gpio_config(USB30_VBUS_DET, GPIO_FN(0)|GPIO_INPUT);
	#endif
		
	while (!kthread_should_stop())
	{
		down(&dwc_otg_device->vbus_usb_task_lock);
		msleep_interruptible(200);

		if ( IsID(dwc_otg_device) == 0 )
		{
			// ID = host mode
			if ( dwc_otg_device->flagID != 0 )
			{
				dwc_otg_device->flagID = 0;
				printk("Set ID to host mode(%d)\n", dwc_otg_device->core_if->port_index);
				
			#if defined(TCC_OTG_WAKE_LOCK)
				otg_wakelock_set(dwc_otg_device->core_if->port_index);
			#endif /* TCC_OTG_WAKE_LOCK */
				
#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)
				USBPHY_SetID(0);
#endif
			}
			dwc_otg_device->flagDeviceVBUS = -1;
			dwc_otg_device->flagDeviceAttach = 0;
		}
		else
		{
			// ID = device mode
			if ( dwc_otg_device->flagID != 1 )
			{
				dwc_otg_device->flagID = 1;
				printk("Set ID to device mode(%d)\n", dwc_otg_device->core_if->port_index);
#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)
#if !defined(DWC_HOST_ONLY)
				USBPHY_SetID(1);
#endif
#endif
			}
#ifndef DWC_HOST_ONLY
			/* if g_file_storage is loaded */
			if ( (dwc_otg_device->pcd) ? gadget_wrapper->driver : 0 )
			{
				/* if USB cable is connected */ 
				if (IsUSBDET(dwc_otg_device->core_if))
				{
					if (dwc_otg_device->flagDeviceAttach != 1) 
					{
						ret = usb_state_notification(dwc_otg_device->core_if, USB_BUS_ADD);
						if(ret == 0) dwc_otg_device->flagDeviceAttach = 1;
						tcc_cpufreq_set_limit_table(&gtUSBClockLimitTable[1], TCC_FREQ_LIMIT_USB, 1);						
					}
				} 
				/* else if USB cable is not connected */ 
				else
				{
					if (dwc_otg_device->flagDeviceAttach == 1) 
					{
						ret = usb_state_notification(dwc_otg_device->core_if, USB_BUS_REMOVE);
						if(ret == 0) dwc_otg_device->flagDeviceAttach = 0;
						tcc_cpufreq_set_limit_table(&gtUSBClockLimitTable[1], TCC_FREQ_LIMIT_USB, 0);						
					}
				}
			}
			/* else if g_file_storage is not loaded */
			else
			{
				dwc_otg_device->flagDeviceAttach = 0;
				dwc_otg_device->flagDeviceVBUS = -1;
			}
#endif
		}

		#if defined(CONFIG_TCC_USB_TO_SATA)
			if(gpio_get_value(USB30_VBUS_DET)){
				/* USB3.0 is detected */
				if(!gpio_get_value(USB30_EN)){
					gpio_set_value(USB30_EN, 1);
					printk("USB3.0 is detected, PC <--> USBtoSATA\n");
				}
			}else{
				/* USB3.0 is disconnected */
				if(gpio_get_value(USB30_EN)){
					gpio_set_value(USB30_EN, 0);
					printk("USB3.0 is disconnected, TCC <--> USBtoSATA\n");
				}
			}
 		#endif
		
		up(&dwc_otg_device->vbus_usb_task_lock);
	}

	DWC_DEBUGPL(DBG_ANY, "%s stopped!!!\n", __func__);

	return 0;
}

/**
 * This function is called during module intialization
 * to pass module parameters to the DWC_OTG CORE.
 */
static int set_parameters(dwc_otg_core_if_t * core_if)
{
	int retval = 0;
	int i;

	if (dwc_otg_module_params.otg_cap != -1) {
		retval +=
		    dwc_otg_set_param_otg_cap(core_if,
					      dwc_otg_module_params.otg_cap);
	}
	if (dwc_otg_module_params.dma_enable != -1) {
		retval +=
		    dwc_otg_set_param_dma_enable(core_if,
						 dwc_otg_module_params.
						 dma_enable);
	}
	if (dwc_otg_module_params.dma_desc_enable != -1) {
		retval +=
		    dwc_otg_set_param_dma_desc_enable(core_if,
						      dwc_otg_module_params.
						      dma_desc_enable);
	}
	if (dwc_otg_module_params.opt != -1) {
		retval +=
		    dwc_otg_set_param_opt(core_if, dwc_otg_module_params.opt);
	}
	if (dwc_otg_module_params.dma_burst_size != -1) {
		retval +=
		    dwc_otg_set_param_dma_burst_size(core_if,
						     dwc_otg_module_params.
						     dma_burst_size);
	}
	if (dwc_otg_module_params.host_support_fs_ls_low_power != -1) {
		retval +=
		    dwc_otg_set_param_host_support_fs_ls_low_power(core_if,
								   dwc_otg_module_params.
								   host_support_fs_ls_low_power);
	}
	if (dwc_otg_module_params.enable_dynamic_fifo != -1) {
		retval +=
		    dwc_otg_set_param_enable_dynamic_fifo(core_if,
							  dwc_otg_module_params.
							  enable_dynamic_fifo);
	}
	if (dwc_otg_module_params.data_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_data_fifo_size(core_if,
						     dwc_otg_module_params.
						     data_fifo_size);
	}
	if (dwc_otg_module_params.dev_rx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_dev_rx_fifo_size(core_if,
						       dwc_otg_module_params.
						       dev_rx_fifo_size);
	}
	if (dwc_otg_module_params.dev_nperio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_dev_nperio_tx_fifo_size(core_if,
							      dwc_otg_module_params.
							      dev_nperio_tx_fifo_size);
	}
	if (dwc_otg_module_params.host_rx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_rx_fifo_size(core_if,
							dwc_otg_module_params.host_rx_fifo_size);
	}
	if (dwc_otg_module_params.host_nperio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_nperio_tx_fifo_size(core_if,
							       dwc_otg_module_params.
							       host_nperio_tx_fifo_size);
	}
	if (dwc_otg_module_params.host_perio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_perio_tx_fifo_size(core_if,
							      dwc_otg_module_params.
							      host_perio_tx_fifo_size);
	}
	if (dwc_otg_module_params.max_transfer_size != -1) {
		retval +=
		    dwc_otg_set_param_max_transfer_size(core_if,
							dwc_otg_module_params.
							max_transfer_size);
	}
	if (dwc_otg_module_params.max_packet_count != -1) {
		retval +=
		    dwc_otg_set_param_max_packet_count(core_if,
						       dwc_otg_module_params.
						       max_packet_count);
	}
	if (dwc_otg_module_params.host_channels != -1) {
		retval +=
		    dwc_otg_set_param_host_channels(core_if,
						    dwc_otg_module_params.
						    host_channels);
	}
	if (dwc_otg_module_params.dev_endpoints != -1) {
		retval +=
		    dwc_otg_set_param_dev_endpoints(core_if,
						    dwc_otg_module_params.
						    dev_endpoints);
	}
	if (dwc_otg_module_params.phy_type != -1) {
		retval +=
		    dwc_otg_set_param_phy_type(core_if,
					       dwc_otg_module_params.phy_type);
	}
	if (dwc_otg_module_params.speed != -1) {
		retval +=
		    dwc_otg_set_param_speed(core_if,
					    dwc_otg_module_params.speed);
	}
	if (dwc_otg_module_params.host_ls_low_power_phy_clk != -1) {
		retval +=
		    dwc_otg_set_param_host_ls_low_power_phy_clk(core_if,
								dwc_otg_module_params.
								host_ls_low_power_phy_clk);
	}
	if (dwc_otg_module_params.phy_ulpi_ddr != -1) {
		retval +=
		    dwc_otg_set_param_phy_ulpi_ddr(core_if,
						   dwc_otg_module_params.
						   phy_ulpi_ddr);
	}
	if (dwc_otg_module_params.phy_ulpi_ext_vbus != -1) {
		retval +=
		    dwc_otg_set_param_phy_ulpi_ext_vbus(core_if,
							dwc_otg_module_params.
							phy_ulpi_ext_vbus);
	}
	if (dwc_otg_module_params.phy_utmi_width != -1) {
		retval +=
		    dwc_otg_set_param_phy_utmi_width(core_if,
						     dwc_otg_module_params.
						     phy_utmi_width);
	}
	if (dwc_otg_module_params.ts_dline != -1) {
		retval +=
		    dwc_otg_set_param_ts_dline(core_if,
					       dwc_otg_module_params.ts_dline);
	}
	if (dwc_otg_module_params.i2c_enable != -1) {
		retval +=
		    dwc_otg_set_param_i2c_enable(core_if,
						 dwc_otg_module_params.
						 i2c_enable);
	}
	if (dwc_otg_module_params.en_multiple_tx_fifo != -1) {
		retval +=
		    dwc_otg_set_param_en_multiple_tx_fifo(core_if,
							  dwc_otg_module_params.
							  en_multiple_tx_fifo);
	}
	for (i = 0; i < 15; i++) {
		if (dwc_otg_module_params.dev_perio_tx_fifo_size[i] != -1) {
			retval +=
			    dwc_otg_set_param_dev_perio_tx_fifo_size(core_if,
								     dwc_otg_module_params.
								     dev_perio_tx_fifo_size
								     [i], i);
		}
	}

	for (i = 0; i < 15; i++) {
		if (dwc_otg_module_params.dev_tx_fifo_size[i] != -1) {
			retval += dwc_otg_set_param_dev_tx_fifo_size(core_if,
								     dwc_otg_module_params.
								     dev_tx_fifo_size
								     [i], i);
		}
	}
	if (dwc_otg_module_params.thr_ctl != -1) {
		retval +=
		    dwc_otg_set_param_thr_ctl(core_if,
					      dwc_otg_module_params.thr_ctl);
	}
	if (dwc_otg_module_params.mpi_enable != -1) {
		retval +=
		    dwc_otg_set_param_mpi_enable(core_if,
						 dwc_otg_module_params.
						 mpi_enable);
	}
	if (dwc_otg_module_params.pti_enable != -1) {
		retval +=
		    dwc_otg_set_param_pti_enable(core_if,
						 dwc_otg_module_params.
						 pti_enable);
	}
	if (dwc_otg_module_params.lpm_enable != -1) {
		retval +=
		    dwc_otg_set_param_lpm_enable(core_if,
						 dwc_otg_module_params.
						 lpm_enable);
	}
	if (dwc_otg_module_params.ic_usb_cap != -1) {
		retval +=
		    dwc_otg_set_param_ic_usb_cap(core_if,
						 dwc_otg_module_params.
						 ic_usb_cap);
	}
	if (dwc_otg_module_params.tx_thr_length != -1) {
		retval +=
		    dwc_otg_set_param_tx_thr_length(core_if,
						    dwc_otg_module_params.tx_thr_length);
	}
	if (dwc_otg_module_params.rx_thr_length != -1) {
		retval +=
		    dwc_otg_set_param_rx_thr_length(core_if,
						    dwc_otg_module_params.
						    rx_thr_length);
	}
	if(dwc_otg_module_params.ahb_thr_ratio != -1) {
		retval +=
		    dwc_otg_set_param_ahb_thr_ratio(core_if, dwc_otg_module_params.ahb_thr_ratio);
	}
	return retval;
}

/**
 * This function is the top level interrupt handler for the Common
 * (Device and host modes) interrupts.
 */
static irqreturn_t dwc_otg_common_irq(int irq, void *dev)
{
	dwc_otg_device_t *otg_dev = dev;
	int32_t retval = IRQ_NONE;

	retval = dwc_otg_handle_common_intr(otg_dev->core_if);
	return IRQ_RETVAL(retval);
}

/**
 * This function is called when a lm_device is unregistered with the
 * dwc_otg_driver. This happens, for example, when the rmmod command is
 * executed. The device may or may not be electrically present. If it is
 * present, the driver stops device processing. Any resources used on behalf
 * of this device are freed.
 *
 * @param[in] _dev
 */
static int dwc_otg_driver_remove(struct platform_device *_dev)
{
	dwc_otg_device_t *otg_dev = platform_get_otgdata(_dev);

	DWC_DEBUGPL(DBG_ANY, "%s(%p)\n", __func__, _dev);

	if (!otg_dev) {
		/* Memory allocation for the dwc_otg_device failed. */
		DWC_ERROR("%s: otg_dev NULL!\n", __func__);
		return 0;
	}

	/* stop tcc_usb_thread */
	if (!IS_ERR(otg_dev->vbus_usb_task)) {
		printk("vbus_usb_task stops...\n");
		kthread_stop(otg_dev->vbus_usb_task);
	}

	/* flush work_queue */
	flush_scheduled_work();
	cancel_work_sync(&otg_dev->core_if->vbus_work);
	otg_dev->core_if->vbus_state = -1;
	
#ifndef DWC_DEVICE_ONLY
	if (otg_dev->hcd) {
		hcd_remove(_dev);
	} else {
		DWC_ERROR("%s: otg_dev->hcd NULL!\n", __func__);
		return 0;
	}
#endif

#ifndef DWC_HOST_ONLY
	if (otg_dev->pcd) {
		pcd_remove(_dev);
	}
#endif
	/*
	 * Free the IRQ
	 */
	if (otg_dev->common_irq_installed) {
		int irq = platform_get_irq(_dev, 0);
		if(irq >= 0)
			free_irq(irq, otg_dev);
	}

	if (otg_dev->core_if) {
		dwc_otg_cil_remove(otg_dev->core_if);
	}

	/*
	 * Remove the device attributes
	 */
	dwc_otg_attr_remove(_dev);
#ifdef DWC_SET_MODE
	device_remove_file(&_dev->dev, &dev_attr_setmode);
	device_remove_file(&_dev->dev, &dev_attr_fsg);
#endif

	/*
	 * Return the memory.
	 */
	if (otg_dev->base) {
		otg_dev->base = NULL;
	}
	dwc_free(otg_dev);

	/*
	 * Clear the drvdata pointer.
	 */
	platform_set_otgdata(_dev, 0);

#if defined(CONFIG_ARCH_TCC8800)
	TCC_OTG_PWR_Control(0);
#endif

	return 0;
}

#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_MACH_TCC9300ST)
static void dwc_otg_disable_unused_port(int port_index)
{
	dwc_otg_core_if_t	core_if;
	core_if.port_index = port_index;

	USBPHY_PWR_Control(1, 1);	// USB1 nano-phy power on
	USBPHY_SetMode(&core_if, USBPHY_MODE_OFF);
	USBPHY_PWR_Control(1, 0);	// USB1 nano-phy power off
}
#endif

/**
 * This function is called when an lm_device is bound to a
 * dwc_otg_driver. It creates the driver components required to
 * control the device (CIL, HCD, and PCD) and it initializes the
 * device. The driver components are stored in a dwc_otg_device
 * structure. A reference to the dwc_otg_device is saved in the
 * lm_device. This allows the driver to access the dwc_otg_device
 * structure on subsequent calls to driver methods for this device.
 *
 * @param[in] _dev Bus device
 */
static int dwc_otg_driver_probe(struct platform_device *_dev)
{
	int retval = 0;
	dwc_otg_device_t *dwc_otg_device;
	struct resource *resource;
	int irq;
	uint8_t port_index = 0;

	dev_dbg(&_dev->dev, "dwc_otg_driver_probe(%p)\n", _dev);

	dwc_otg_device = dwc_alloc(sizeof(dwc_otg_device_t));

	if (!dwc_otg_device) {
		dev_err(&_dev->dev, "kmalloc of dwc_otg_device failed\n");
		retval = -ENOMEM;
		goto fail;
	}

	memset(dwc_otg_device, 0, sizeof(*dwc_otg_device));
	dwc_otg_device->reg_offset = 0xFFFFFFFF;
	dwc_otg_device->vbus_usb_task = (struct task_struct *)(-EINVAL);

#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_MACH_TCC9300ST)
	/*
	 * Map the DWC_otg Port
	 */
	resource = platform_get_resource(_dev, IORESOURCE_IO, 0);
	if (!resource) {
		dev_err(&_dev->dev, "no resource for DWC_otg Port?\n");
		retval = -ENODEV;
		goto fail;
	}
	port_index = (uint8_t)resource->start;
#endif
	
	/*
	 * Map the DWC_otg Core memory into virtual address space.
	 */
	resource = platform_get_resource(_dev, IORESOURCE_MEM, 0);
	if (!resource) {
		dev_err(&_dev->dev, "no resource for DWC_otg Core Memory?\n");
		retval = -ENODEV;
		goto fail;
	}
	dwc_otg_device->base = (void *)resource->start;
	if (!dwc_otg_device->base) {
		dev_err(&_dev->dev, "ioremap() failed\n");
		retval = -ENOMEM;
		goto fail;
	}
	dev_dbg(&_dev->dev, "base=0x%08x\n", (unsigned)dwc_otg_device->base);

	/* clock control register */
	dwc_otg_device->clk[0] = clk_get(NULL, "usb_otg");
	if (IS_ERR(dwc_otg_device->clk[0])){
		printk("ERR - usb_otg clk_get fail.\n");
		goto fail;
	}
	clk_enable(dwc_otg_device->clk[0]);

#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_MACH_TCC9300ST)		
{
	struct clk *pHSGDMAClk;
	pHSGDMAClk = clk_get(NULL, "hs_gdma");
	if (IS_ERR(pHSGDMAClk)){
		printk("ERR - hs_gdma clk_get fail.\n");	
		goto fail;
	}
	clk_enable(pHSGDMAClk);

	tcc_cpufreq_set_limit_table(&gtHSIOClockLimitTable, TCC_FREQ_LIMIT_OTG, 1);

	dwc_otg_device->clk[1] = clk_get(NULL, "hs_usb_otg");
	if (IS_ERR(dwc_otg_device->clk[1])){
		printk("ERR - hs_usb_otg clk_get fail.\n");
		goto fail;
	}
	clk_enable(dwc_otg_device->clk[1]);
}
#endif	

	/*
	 * Turn on DWC_otg core.
	 */
#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_MACH_TCC9300ST)
	if(port_index != 0)
	{
		USBPHY_PWR_Control(port_index, 1);	// USB1 nano-phy power on
		tca_ckc_setcommonhsiobus(HSIOBUS_USB20OTG1, ENABLE);
	}
	else
#endif
	{
#if defined(CONFIG_ARCH_TCC88XX)
		if (machine_is_tcc8800() || machine_is_tcc8800st())
			tcc_ohci_clock_control(-1, 1);
		if (machine_is_m801_88() || machine_is_m803())
			TCC_OTG_PWR_M801(1);
#endif
		tca_ckc_setiopwdn(RB_USB20OTG, 0);	// Turn on the USB clock from IO BUS
	}

#if defined(CONFIG_ARCH_TCC93XX)
	/* Reduce leakage current */
	#if defined(CONFIG_TCC_DWC_OTG0) && !defined(CONFIG_TCC_DWC_OTG1)
		dwc_otg_disable_unused_port(1);
	#elif !defined(CONFIG_TCC_DWC_OTG0) && defined(CONFIG_TCC_DWC_OTG1)
		dwc_otg_disable_unused_port(0);
	#endif
#endif

	/*
	 * Initialize driver data to point to the global DWC_otg
	 * Device structure.
	 */
	platform_set_otgdata(_dev, dwc_otg_device);
	dev_dbg(&_dev->dev, "dwc_otg_device=0x%p\n", dwc_otg_device);

	dwc_otg_device->core_if = dwc_otg_cil_init(dwc_otg_device->base);
	if (!dwc_otg_device->core_if) {
		dev_err(&_dev->dev, "CIL initialization failed!\n");
		retval = -ENOMEM;
		goto fail;
	}
	dwc_otg_device->core_if->port_index = port_index;

#if defined(CONFIG_REGULATOR)
	dwc_otg_device->vdd_usb33 = NULL;
	dwc_otg_device->vdd_usb33 = regulator_get(NULL, "vdd_usb33");
	if (IS_ERR(dwc_otg_device->vdd_usb33)) {
		pr_warning("%s: failed to get USB 3.3V regulator\n", dwc_driver_name);
		dwc_otg_device->vdd_usb33 = NULL;
	} else {
		regulator_enable(dwc_otg_device->vdd_usb33);
	}

	dwc_otg_device->vdd_usb12 = NULL;
	dwc_otg_device->vdd_usb12 = regulator_get(NULL, "vdd_usb12");
	if (IS_ERR(dwc_otg_device->vdd_usb12)) {
		pr_warning("%s: failed to get USB 1.2V regulator\n", dwc_driver_name);
		dwc_otg_device->vdd_usb12 = NULL;
	} else {
		regulator_enable(dwc_otg_device->vdd_usb12);
	}
#endif

	/*
	 * Attempt to ensure this device is really a DWC_otg Controller.
	 * Read and verify the SNPSID register contents. The value should be
	 * 0x45F42XXX, which corresponds to "OT2", as in "OTG version 2.XX".
	 */
	if ((dwc_otg_get_gsnpsid(dwc_otg_device->core_if) & 0xFFFFF000) !=
	    0x4F542000) {
		dev_err(&_dev->dev, "Bad value for SNPSID: 0x%08x\n",
			dwc_otg_get_gsnpsid(dwc_otg_device->core_if));
		dwc_otg_cil_remove(dwc_otg_device->core_if);
		dwc_free(dwc_otg_device);
		retval = -EINVAL;
		goto fail;
	}

	/*
	 * Validate parameter values.
	 */
	if (set_parameters(dwc_otg_device->core_if)) {
		dwc_otg_cil_remove(dwc_otg_device->core_if);
		retval = -EINVAL;
		goto fail;
	}

	/*
	 * Create Device Attributes in sysfs
	 */
	dwc_otg_attr_create(_dev);
#ifdef DWC_SET_MODE
	device_create_file(&_dev->dev, &dev_attr_setmode);
	dwc_otg_device->flagMode = 2;
	device_create_file(&_dev->dev, &dev_attr_fsg);
	dwc_otg_device->flagDeviceAttach = 0;
#endif

	/*
	 * VBUS control workQ
	 */
	INIT_WORK(&dwc_otg_device->core_if->vbus_work, tcc_vbus_work);
	INIT_WORK(&dwc_otg_device->core_if->idcfg_work, w_idcfg_disable);

	/*
	 * Disable the global interrupt until all the interrupt
	 * handlers are installed.
	 */
	dwc_otg_disable_global_interrupts(dwc_otg_device->core_if);

	/*
	 * Install the interrupt handler for the common interrupts before
	 * enabling common interrupts in core_init below.
	 */
	irq = platform_get_irq(_dev, 0);
	if (irq < 0) {
		dev_err(&_dev->dev, "no irq? (irq=%d)\n", irq);
		retval = -ENODEV;
		goto fail;
	}
	DWC_DEBUGPL(DBG_CIL, "registering (common) handler for irq%d\n", irq);
	retval = request_irq(irq, dwc_otg_common_irq,
			     IRQF_SHARED, "dwc_otg", dwc_otg_device);
	if (retval) {
		DWC_ERROR("request of irq%d failed\n", irq);
		retval = -EBUSY;
		goto fail;
	} else {
		dwc_otg_device->common_irq_installed = 1;
	}

	tcc_otg_vbus_init();	

	/*
	 * Initialize the DWC_otg core.
	 */
	tcc_otg_vbus_ctrl(dwc_otg_device->core_if->port_index, 0);
	USBPHY_SetMode(dwc_otg_device->core_if, USBPHY_MODE_RESET);

#if defined(CONFIG_ARCH_TCC92XX)
	USBPHY_SetID(1);	// device
#endif
	USBPHY_SetMode(dwc_otg_device->core_if, USBPHY_MODE_OTG);

	tcc_usb_link_reset(dwc_otg_device->core_if);	//Reset OTG LINK - AlenOh
	mdelay(5);
	dwc_otg_core_init(dwc_otg_device->core_if);

	if(machine_is_tcc8800())
	{
		if(GetConnIdStatus(dwc_otg_device->core_if) == 0)
		{
			/* host */
			dwc_otg_enable_global_interrupts(dwc_otg_device->core_if);
		}
	}

#ifndef DWC_HOST_ONLY
	/*
	 * Initialize the PCD
	 */
	retval = pcd_init(_dev);
	if (retval != 0) {
		DWC_ERROR("pcd_init failed\n");
		dwc_otg_device->pcd = NULL;
		goto fail;
	}
#endif
#ifndef DWC_DEVICE_ONLY
	/*
	 * Initialize the HCD
	 */
	retval = hcd_init(_dev);
	if (retval != 0) {
		DWC_ERROR("hcd_init failed\n");
		dwc_otg_device->hcd = NULL;
		goto fail;
	}
#endif

	sema_init(&dwc_otg_device->vbus_usb_task_lock, 1);
	dwc_otg_device->vbus_usb_task = kthread_create(tcc_usb_thread, dwc_otg_device, "tcc-usb-thread");
	if (IS_ERR(dwc_otg_device->vbus_usb_task)) {
		printk("\nPTR ERR %p", dwc_otg_device->vbus_usb_task);
		retval = -EINVAL;
		goto fail;
	}

	/* Tell the thread to start working */
	wake_up_process(dwc_otg_device->vbus_usb_task);

	/*
	 * Enable the global interrupt after all the interrupt
	 * handlers are installed.
	 */
	dwc_otg_enable_global_interrupts(dwc_otg_device->core_if);

#if defined(CONFIG_TCC_DWC_OTG_V281A)
	dwc_otg_devices	= dwc_otg_device;
#endif	
	return 0;

fail:
	dwc_otg_driver_remove(_dev);
	return retval;
}

#ifdef CONFIG_PM
static int dwc_otg_driver_suspend(struct platform_device *pdev, pm_message_t state)
{
	dwc_otg_device_t *dwc_otg_device = platform_get_otgdata(pdev);

	USBPHY_IDCFG(1, dwc_otg_device->core_if->port_index, 0);
	tcc_otg_vbus_ctrl(dwc_otg_device->core_if->port_index, 0);

#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)	
	/* device mode change & wait */
	dwc_otg_device->flagMode_backup = dwc_otg_device->flagMode;
	dwc_otg_device->flagMode = 1;

	while (dwc_otg_device->flagID == 0) {
		msleep_interruptible(200);
	}
#else
	msleep_interruptible(500);
#endif
	dwc_otg_disable_global_interrupts(dwc_otg_device->core_if);

	/* stop vbus thread */
	down(&dwc_otg_device->vbus_usb_task_lock);

#if defined(CONFIG_REGULATOR)
	if (dwc_otg_device->vdd_usb33)
		regulator_disable(dwc_otg_device->vdd_usb33);
	if (dwc_otg_device->vdd_usb12)
		regulator_disable(dwc_otg_device->vdd_usb12);
#endif

	/* USB Phy off */
	USBPHY_SetMode(dwc_otg_device->core_if, USBPHY_MODE_OFF);
	USBPHY_IDCFG(0, dwc_otg_device->core_if->port_index, 0);

#if defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8800ST)
	tcc_ohci_clock_control(-1, 0);
#endif
#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
	TCC_OTG_PWR_M801(0);
#endif

#if defined(CONFIG_ARCH_TCC892X)
	clk_disable(dwc_otg_device->clk[0]);
#endif
	
	tcc_otg_vbus_exit();
	
	return 0;
}

static int dwc_otg_driver_resume(struct platform_device *pdev)
{
	int id = -1;
	dwc_otg_device_t *dwc_otg_device = platform_get_otgdata(pdev);

	tcc_otg_vbus_init();
	
#if defined(CONFIG_ARCH_TCC892X)
	clk_enable(dwc_otg_device->clk[0]);
#endif
	
#if defined(CONFIG_REGULATOR)
	if (dwc_otg_device->vdd_usb33)
		regulator_enable(dwc_otg_device->vdd_usb33);
	if (dwc_otg_device->vdd_usb12)
		regulator_enable(dwc_otg_device->vdd_usb12);
#endif

#if defined(CONFIG_MACH_TCC8800) || defined(CONFIG_MACH_TCC8800ST)
	tcc_ohci_clock_control(-1, 1);
#endif
#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
	TCC_OTG_PWR_M801(1);
#endif

	USBPHY_SetMode(dwc_otg_device->core_if, USBPHY_MODE_RESET);
	USBPHY_SetMode(dwc_otg_device->core_if, USBPHY_MODE_OTG);

	tcc_usb_link_reset(dwc_otg_device->core_if);
	dwc_otg_cil_reinit(dwc_otg_device->core_if, dwc_otg_device->base, (dwc_otg_core_params_t *)&dwc_otg_module_params);
	dwc_otg_core_init(dwc_otg_device->core_if);
	dwc_otg_enable_global_interrupts(dwc_otg_device->core_if);

	/* GetConnIdStatus will be updated after OTG powerup. */
	id = GetConnIdStatus(dwc_otg_device->core_if);

	dwc_otg_device->flagDeviceVBUS = -1;
	dwc_otg_device->flagID = -1;
	dwc_otg_device->flagDeviceAttach = 0;

#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)	
	dwc_otg_core_dev_init(dwc_otg_device->core_if);
	if(id == 0){
		tcc_otg_vbus_ctrl(dwc_otg_device->core_if->port_index, 1);
	}
#else
	if(id){
		dwc_otg_core_dev_init(dwc_otg_device->core_if);
	}
	else{
		dwc_otg_core_host_init(dwc_otg_device->core_if);
	}
#endif

#ifndef DWC_HOST_ONLY
	/* disconnect Gadget driver (only tested FSG) */
	if (dwc_otg_device->flagDeviceAttach && !IsUSBDET(dwc_otg_device->core_if)) {
		gadget_wrapper->driver->disconnect(&gadget_wrapper->gadget);
	}
#endif

#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)	
	/* device mode change & wait */
	dwc_otg_device->flagMode = 1;

	/* device re-attach */
	dwc_otg_device->flagDeviceAttach = 0;
	dwc_otg_device->flagDeviceVBUS = -1;
#endif	

	/* start vbus thread */
	up(&dwc_otg_device->vbus_usb_task_lock);

#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)
	/* device mode change & wait */
	while (dwc_otg_device->flagID == 0) {
	msleep_interruptible(200);
	}

	/* resotre mode */
	dwc_otg_device->flagMode = dwc_otg_device->flagMode_backup;		
#endif	
	dwc_otg_enable_global_interrupts(dwc_otg_device->core_if);

	return 0;
}
#else
#define dwc_otg_driver_suspend	NULL
#define dwc_otg_driver_resume	NULL
#endif

#if defined(CONFIG_TCC_DWC_OTG_V281A)
int dwc_otg_driver_force_init(void)
{
	printk("DWC_OTG Force Init!!(%d)\n", dwc_otg_devices->core_if->port_index); 
	
	USBPHY_SetMode(dwc_otg_devices->core_if, USBPHY_MODE_ON);
	OTGCORE_Init(dwc_otg_devices->core_if);
	tcc_usb_link_reset(dwc_otg_devices->core_if);
	dwc_otg_cil_reinit(dwc_otg_devices->core_if, dwc_otg_devices->base, (dwc_otg_core_params_t *)&dwc_otg_module_params);

	dwc_otg_core_init(dwc_otg_devices->core_if);
	dwc_otg_enable_global_interrupts(dwc_otg_devices->core_if);
	dwc_otg_core_dev_init(dwc_otg_devices->core_if);
	return 0;
}
EXPORT_SYMBOL(dwc_otg_driver_force_init);
#endif
/**
 * This structure defines the methods to be called by a bus driver
 * during the lifecycle of a device on that bus. Both drivers and
 * devices are registered with a bus driver. The bus driver matches
 * devices to drivers based on information in the device and driver
 * structures.
 *
 * The probe function is called when the bus driver matches a device
 * to this driver. The remove function is called when a device is
 * unregistered with the bus driver.
 */
static struct platform_driver dwc_otg_driver = {
	.probe		= dwc_otg_driver_probe,
	.remove		= dwc_otg_driver_remove,
	.suspend	= dwc_otg_driver_suspend,
	.resume		= dwc_otg_driver_resume,
	.driver		= {
		.name	= (char *)dwc_driver_name,
		.owner	= THIS_MODULE,
	},
};


/**
 * This function is called when the dwc_otg_driver is installed with the
 * insmod command. It registers the dwc_otg_driver structure with the
 * appropriate bus driver. This will cause the dwc_otg_driver_probe function
 * to be called. In addition, the bus driver will automatically expose
 * attributes defined for the device and driver in the special sysfs file
 * system.
 *
 * @return
 */
static int __init dwc_otg_driver_init(void)
{
	int retval = 0, ret;

#if defined(DWC_HOST_ONLY)
	char *p = "HostOnly";
#elif defined(DWC_DEVICE_ONLY)
	char *p = "DeviceOnly";
#elif defined(DWC_DUAL_ROLE)
	char *p = "DualRole";
#elif defined(CONFIG_TCC_DWC_OTG_2PORT_0OTG_1HOST)
	char *p = "0OTG_1HOST";
#elif defined(CONFIG_TCC_DWC_OTG_2PORT_0HOST_1OTG)
	char *p = "0HOST_1OTG";
#else
	char *p = "Unknown";
#endif

#if defined(TCC_OTG_WAKE_LOCK)
	otg_wakelock_init();
#endif /* TCC_OTG_WAKE_LOCK */

	printk(KERN_INFO "%s: version %s (%s)\n", dwc_driver_name,
	       DWC_DRIVER_VERSION, p);
	printk(KERN_INFO "Working version %s\n", "No 007 - 10/24/2007");

	retval = platform_driver_register(&dwc_otg_driver);
	if (retval < 0) {
		printk(KERN_ERR "%s retval=%d\n", __func__, retval);
	
	#if defined(TCC_OTG_WAKE_LOCK)
		otg_wakelock_exit();
	#endif /* TCC_OTG_WAKE_LOCK */	
		return retval;
	}

	ret = driver_create_file(&dwc_otg_driver.driver, &driver_attr_version);
	ret = driver_create_file(&dwc_otg_driver.driver, &driver_attr_debuglevel);

	return retval;
}

module_init(dwc_otg_driver_init);

/**
 * This function is called when the driver is removed from the kernel
 * with the rmmod command. The driver unregisters itself with its bus
 * driver.
 *
 */
static void __exit dwc_otg_driver_cleanup(void)
{
	printk(KERN_DEBUG "dwc_otg_driver_cleanup()\n");

	driver_remove_file(&dwc_otg_driver.driver, &driver_attr_debuglevel);
	driver_remove_file(&dwc_otg_driver.driver, &driver_attr_version);
	
	platform_driver_unregister(&dwc_otg_driver);

	printk(KERN_INFO "%s module removed\n", dwc_driver_name);

#if defined(TCC_OTG_WAKE_LOCK)
	otg_wakelock_exit();
#endif /* TCC_OTG_WAKE_LOCK */	
}
module_exit(dwc_otg_driver_cleanup);

MODULE_DESCRIPTION(DWC_DRIVER_DESC);
MODULE_AUTHOR("Synopsys Inc.");
MODULE_LICENSE("GPL");

module_param_named(otg_cap, dwc_otg_module_params.otg_cap, int, 0444);
MODULE_PARM_DESC(otg_cap, "OTG Capabilities 0=HNP&SRP 1=SRP Only 2=None");
module_param_named(opt, dwc_otg_module_params.opt, int, 0444);
MODULE_PARM_DESC(opt, "OPT Mode");
module_param_named(dma_enable, dwc_otg_module_params.dma_enable, int, 0444);
MODULE_PARM_DESC(dma_enable, "DMA Mode 0=Slave 1=DMA enabled");

module_param_named(dma_desc_enable, dwc_otg_module_params.dma_desc_enable, int,
		   0444);
MODULE_PARM_DESC(dma_desc_enable,
		 "DMA Desc Mode 0=Address DMA 1=DMA Descriptor enabled");

module_param_named(dma_burst_size, dwc_otg_module_params.dma_burst_size, int,
		   0444);
MODULE_PARM_DESC(dma_burst_size,
		 "DMA Burst Size 1, 4, 8, 16, 32, 64, 128, 256");
module_param_named(speed, dwc_otg_module_params.speed, int, 0444);
MODULE_PARM_DESC(speed, "Speed 0=High Speed 1=Full Speed");
module_param_named(host_support_fs_ls_low_power,
		   dwc_otg_module_params.host_support_fs_ls_low_power, int,
		   0444);
MODULE_PARM_DESC(host_support_fs_ls_low_power,
		 "Support Low Power w/FS or LS 0=Support 1=Don't Support");
module_param_named(host_ls_low_power_phy_clk,
		   dwc_otg_module_params.host_ls_low_power_phy_clk, int, 0444);
MODULE_PARM_DESC(host_ls_low_power_phy_clk,
		 "Low Speed Low Power Clock 0=48Mhz 1=6Mhz");
module_param_named(enable_dynamic_fifo,
		   dwc_otg_module_params.enable_dynamic_fifo, int, 0444);
MODULE_PARM_DESC(enable_dynamic_fifo, "0=cC Setting 1=Allow Dynamic Sizing");
module_param_named(data_fifo_size, dwc_otg_module_params.data_fifo_size, int,
		   0444);
MODULE_PARM_DESC(data_fifo_size,
		 "Total number of words in the data FIFO memory 32-32768");
module_param_named(dev_rx_fifo_size, dwc_otg_module_params.dev_rx_fifo_size,
		   int, 0444);
MODULE_PARM_DESC(dev_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(dev_nperio_tx_fifo_size,
		   dwc_otg_module_params.dev_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(dev_nperio_tx_fifo_size,
		 "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(dev_perio_tx_fifo_size_1,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_1,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_2,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_2,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_3,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_3,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_4,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_4,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_5,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_5,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_6,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_6,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_7,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_7,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_8,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_8,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_9,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_9,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_10,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_10,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_11,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_11,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_12,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_12,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_13,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_13,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_14,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_14,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_15,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_15,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(host_rx_fifo_size, dwc_otg_module_params.host_rx_fifo_size,
		   int, 0444);
MODULE_PARM_DESC(host_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(host_nperio_tx_fifo_size,
		   dwc_otg_module_params.host_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_nperio_tx_fifo_size,
		 "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(host_perio_tx_fifo_size,
		   dwc_otg_module_params.host_perio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_perio_tx_fifo_size,
		 "Number of words in the host periodic Tx FIFO 16-32768");
module_param_named(max_transfer_size, dwc_otg_module_params.max_transfer_size,
		   int, 0444);
/** @todo Set the max to 512K, modify checks */
MODULE_PARM_DESC(max_transfer_size,
		 "The maximum transfer size supported in bytes 2047-65535");
module_param_named(max_packet_count, dwc_otg_module_params.max_packet_count,
		   int, 0444);
MODULE_PARM_DESC(max_packet_count,
		 "The maximum number of packets in a transfer 15-511");
module_param_named(host_channels, dwc_otg_module_params.host_channels, int,
		   0444);
MODULE_PARM_DESC(host_channels,
		 "The number of host channel registers to use 1-16");
module_param_named(dev_endpoints, dwc_otg_module_params.dev_endpoints, int,
		   0444);
MODULE_PARM_DESC(dev_endpoints,
		 "The number of endpoints in addition to EP0 available for device mode 1-15");
module_param_named(phy_type, dwc_otg_module_params.phy_type, int, 0444);
MODULE_PARM_DESC(phy_type, "0=Reserved 1=UTMI+ 2=ULPI");
module_param_named(phy_utmi_width, dwc_otg_module_params.phy_utmi_width, int,
		   0444);
MODULE_PARM_DESC(phy_utmi_width, "Specifies the UTMI+ Data Width 8 or 16 bits");
module_param_named(phy_ulpi_ddr, dwc_otg_module_params.phy_ulpi_ddr, int, 0444);
MODULE_PARM_DESC(phy_ulpi_ddr,
		 "ULPI at double or single data rate 0=Single 1=Double");
module_param_named(phy_ulpi_ext_vbus, dwc_otg_module_params.phy_ulpi_ext_vbus,
		   int, 0444);
MODULE_PARM_DESC(phy_ulpi_ext_vbus,
		 "ULPI PHY using internal or external vbus 0=Internal");
module_param_named(i2c_enable, dwc_otg_module_params.i2c_enable, int, 0444);
MODULE_PARM_DESC(i2c_enable, "FS PHY Interface");
module_param_named(ulpi_fs_ls, dwc_otg_module_params.ulpi_fs_ls, int, 0444);
MODULE_PARM_DESC(ulpi_fs_ls, "ULPI PHY FS/LS mode only");
module_param_named(ts_dline, dwc_otg_module_params.ts_dline, int, 0444);
MODULE_PARM_DESC(ts_dline, "Term select Dline pulsing for all PHYs");
module_param_named(debug, g_dbg_lvl, int, 0444);
MODULE_PARM_DESC(debug, "");

module_param_named(en_multiple_tx_fifo,
		   dwc_otg_module_params.en_multiple_tx_fifo, int, 0444);
MODULE_PARM_DESC(en_multiple_tx_fifo,
		 "Dedicated Non Periodic Tx FIFOs 0=disabled 1=enabled");
module_param_named(dev_tx_fifo_size_1,
		   dwc_otg_module_params.dev_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_1, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_2,
		   dwc_otg_module_params.dev_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_2, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_3,
		   dwc_otg_module_params.dev_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_3, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_4,
		   dwc_otg_module_params.dev_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_4, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_5,
		   dwc_otg_module_params.dev_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_5, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_6,
		   dwc_otg_module_params.dev_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_6, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_7,
		   dwc_otg_module_params.dev_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_7, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_8,
		   dwc_otg_module_params.dev_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_8, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_9,
		   dwc_otg_module_params.dev_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_9, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_10,
		   dwc_otg_module_params.dev_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_10, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_11,
		   dwc_otg_module_params.dev_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_11, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_12,
		   dwc_otg_module_params.dev_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_12, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_13,
		   dwc_otg_module_params.dev_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_13, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_14,
		   dwc_otg_module_params.dev_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_14, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_15,
		   dwc_otg_module_params.dev_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_15, "Number of words in the Tx FIFO 4-768");

module_param_named(thr_ctl, dwc_otg_module_params.thr_ctl, int, 0444);
MODULE_PARM_DESC(thr_ctl,
		 "Thresholding enable flag bit 0 - non ISO Tx thr., 1 - ISO Tx thr., 2 - Rx thr.- bit 0=disabled 1=enabled");
module_param_named(tx_thr_length, dwc_otg_module_params.tx_thr_length, int,
		   0444);
MODULE_PARM_DESC(tx_thr_length, "Tx Threshold length in 32 bit DWORDs");
module_param_named(rx_thr_length, dwc_otg_module_params.rx_thr_length, int,
		   0444);
MODULE_PARM_DESC(rx_thr_length, "Rx Threshold length in 32 bit DWORDs");

module_param_named(pti_enable, dwc_otg_module_params.pti_enable, int, 0444);
module_param_named(mpi_enable, dwc_otg_module_params.mpi_enable, int, 0444);
module_param_named(lpm_enable, dwc_otg_module_params.lpm_enable, int, 0444);
MODULE_PARM_DESC(lpm_enable, "LPM Enable 0=LPM Disabled 1=LPM Enabled");
module_param_named(ic_usb_cap, dwc_otg_module_params.ic_usb_cap, int, 0444);
MODULE_PARM_DESC(ic_usb_cap,
		 "IC_USB Capability 0=IC_USB Disabled 1=IC_USB Enabled");
module_param_named(ahb_thr_ratio, dwc_otg_module_params.ahb_thr_ratio, int, 0444);
MODULE_PARM_DESC(ahb_thr_ratio, "AHB Threshold Ratio");

/** @page "Module Parameters"
 *
 * The following parameters may be specified when starting the module.
 * These parameters define how the DWC_otg controller should be
 * configured. Parameter values are passed to the CIL initialization
 * function dwc_otg_cil_init
 *
 * Example: <code>modprobe dwc_otg speed=1 otg_cap=1</code>
 *

 <table>
 <tr><td>Parameter Name</td><td>Meaning</td></tr>

 <tr>
 <td>otg_cap</td>
 <td>Specifies the OTG capabilities. The driver will automatically detect the
 value for this parameter if none is specified.
 - 0: HNP and SRP capable (default, if available)
 - 1: SRP Only capable
 - 2: No HNP/SRP capable
 </td></tr>

 <tr>
 <td>dma_enable</td>
 <td>Specifies whether to use slave or DMA mode for accessing the data FIFOs.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Slave
 - 1: DMA (default, if available)
 </td></tr>

 <tr>
 <td>dma_burst_size</td>
 <td>The DMA Burst size (applicable only for External DMA Mode).
 - Values: 1, 4, 8 16, 32, 64, 128, 256 (default 32)
 </td></tr>

 <tr>
 <td>speed</td>
 <td>Specifies the maximum speed of operation in host and device mode. The
 actual speed depends on the speed of the attached device and the value of
 phy_type.
 - 0: High Speed (default)
 - 1: Full Speed
 </td></tr>

 <tr>
 <td>host_support_fs_ls_low_power</td>
 <td>Specifies whether low power mode is supported when attached to a Full
 Speed or Low Speed device in host mode.
 - 0: Don't support low power mode (default)
 - 1: Support low power mode
 </td></tr>

 <tr>
 <td>host_ls_low_power_phy_clk</td>
 <td>Specifies the PHY clock rate in low power mode when connected to a Low
 Speed device in host mode. This parameter is applicable only if
 HOST_SUPPORT_FS_LS_LOW_POWER is enabled.
 - 0: 48 MHz (default)
 - 1: 6 MHz
 </td></tr>

 <tr>
 <td>enable_dynamic_fifo</td>
 <td> Specifies whether FIFOs may be resized by the driver software.
 - 0: Use cC FIFO size parameters
 - 1: Allow dynamic FIFO sizing (default)
 </td></tr>

 <tr>
 <td>data_fifo_size</td>
 <td>Total number of 4-byte words in the data FIFO memory. This memory
 includes the Rx FIFO, non-periodic Tx FIFO, and periodic Tx FIFOs.
 - Values: 32 to 32768 (default 8192)

 Note: The total FIFO memory depth in the FPGA configuration is 8192.
 </td></tr>

 <tr>
 <td>dev_rx_fifo_size</td>
 <td>Number of 4-byte words in the Rx FIFO in device mode when dynamic
 FIFO sizing is enabled.
 - Values: 16 to 32768 (default 1064)
 </td></tr>

 <tr>
 <td>dev_nperio_tx_fifo_size</td>
 <td>Number of 4-byte words in the non-periodic Tx FIFO in device mode when
 dynamic FIFO sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>dev_perio_tx_fifo_size_n (n = 1 to 15)</td>
 <td>Number of 4-byte words in each of the periodic Tx FIFOs in device mode
 when dynamic FIFO sizing is enabled.
 - Values: 4 to 768 (default 256)
 </td></tr>

 <tr>
 <td>host_rx_fifo_size</td>
 <td>Number of 4-byte words in the Rx FIFO in host mode when dynamic FIFO
 sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>host_nperio_tx_fifo_size</td>
 <td>Number of 4-byte words in the non-periodic Tx FIFO in host mode when
 dynamic FIFO sizing is enabled in the core.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>host_perio_tx_fifo_size</td>
 <td>Number of 4-byte words in the host periodic Tx FIFO when dynamic FIFO
 sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>max_transfer_size</td>
 <td>The maximum transfer size supported in bytes.
 - Values: 2047 to 65,535 (default 65,535)
 </td></tr>

 <tr>
 <td>max_packet_count</td>
 <td>The maximum number of packets in a transfer.
 - Values: 15 to 511 (default 511)
 </td></tr>

 <tr>
 <td>host_channels</td>
 <td>The number of host channel registers to use.
 - Values: 1 to 16 (default 12)

 Note: The FPGA configuration supports a maximum of 12 host channels.
 </td></tr>

 <tr>
 <td>dev_endpoints</td>
 <td>The number of endpoints in addition to EP0 available for device mode
 operations.
 - Values: 1 to 15 (default 6 IN and OUT)

 Note: The FPGA configuration supports a maximum of 6 IN and OUT endpoints in
 addition to EP0.
 </td></tr>

 <tr>
 <td>phy_type</td>
 <td>Specifies the type of PHY interface to use. By default, the driver will
 automatically detect the phy_type.
 - 0: Full Speed
 - 1: UTMI+ (default, if available)
 - 2: ULPI
 </td></tr>

 <tr>
 <td>phy_utmi_width</td>
 <td>Specifies the UTMI+ Data Width. This parameter is applicable for a
 phy_type of UTMI+. Also, this parameter is applicable only if the
 OTG_HSPHY_WIDTH cC parameter was set to "8 and 16 bits", meaning that the
 core has been configured to work at either data path width.
 - Values: 8 or 16 bits (default 16)
 </td></tr>

 <tr>
 <td>phy_ulpi_ddr</td>
 <td>Specifies whether the ULPI operates at double or single data rate. This
 parameter is only applicable if phy_type is ULPI.
 - 0: single data rate ULPI interface with 8 bit wide data bus (default)
 - 1: double data rate ULPI interface with 4 bit wide data bus
 </td></tr>

 <tr>
 <td>i2c_enable</td>
 <td>Specifies whether to use the I2C interface for full speed PHY. This
 parameter is only applicable if PHY_TYPE is FS.
 - 0: Disabled (default)
 - 1: Enabled
 </td></tr>

 <tr>
 <td>otg_en_multiple_tx_fifo</td>
 <td>Specifies whether dedicatedto tx fifos are enabled for non periodic IN EPs.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Disabled
 - 1: Enabled (default, if available)
 </td></tr>

 <tr>
 <td>dev_tx_fifo_size_n (n = 1 to 15)</td>
 <td>Number of 4-byte words in each of the Tx FIFOs in device mode
 when dynamic FIFO sizing is enabled.
 - Values: 4 to 768 (default 256)
 </td></tr>

 <tr>
 <td>tx_thr_length</td>
 <td>Transmit Threshold length in 32 bit double words
 - Values: 8 to 128 (default 64)
 </td></tr>

 <tr>
 <td>rx_thr_length</td>
 <td>Receive Threshold length in 32 bit double words
 - Values: 8 to 128 (default 64)
 </td></tr>

<tr>
 <td>thr_ctl</td>
 <td>Specifies whether to enable Thresholding for Device mode. Bits 0, 1, 2 of this
 parmater specifies if thresholding is enabled for non-Iso Tx, Iso Tx and Rx
 transfers accordingly.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - Values: 0 to 7 (default 0)
 Bit values indicate:
 - 0: Thresholding disabled
 - 1: Thresholding enabled
 </td></tr>

<tr>
 <td>dma_desc_enable</td>
 <td>Specifies whether to enable Descriptor DMA mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Descriptor DMA disabled
 - 1: Descriptor DMA (default, if available)
 </td></tr>

<tr>
 <td>mpi_enable</td>
 <td>Specifies whether to enable MPI enhancement mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: MPI disabled (default)
 - 1: MPI enable
 </td></tr>

<tr>
 <td>pti_enable</td>
 <td>Specifies whether to enable PTI enhancement support.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: PTI disabled (default)
 - 1: PTI enable
 </td></tr>

<tr>
 <td>lpm_enable</td>
 <td>Specifies whether to enable LPM support.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: LPM disabled
 - 1: LPM enable (default, if available)
 </td></tr>

 <tr>
 <td>ahb_thr_ratio</td>
 <td>Specifies AHB Threshold ratio.
 - Values: 0 to 3 (default 0)
 </td></tr>

*/
