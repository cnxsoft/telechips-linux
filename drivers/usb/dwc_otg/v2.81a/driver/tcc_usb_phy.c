/*
 *
 */
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <mach/bsp.h>
#include <asm/io.h>
#include <mach/gpio.h>
#include <asm/mach-types.h>
#include <linux/i2c/pca953x.h>
#include <mach/tcc_board_power.h>

#include "tcc_usb_def.h"
#include "tcc_usb_phy.h"

/* For Signature */
#define TCC_USB_PHY_SIGNATURE			'T','C','C','_','U','S','B','_','P','H','Y','_'
#define TCC_USB_PHY_VERSION				'V','2','.','0','0','0'
static const unsigned char TCC_USB_PHY_C_Version[] =
    {SIGBYAHONG, TCC_USB_PHY_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, TCC_USB_PHY_VERSION, 0};

#define	UPCR0_PR						Hw14					// (1:enable)/(0:disable) Port Reset
#define	UPCR0_CM_EN						Hw13					// Common Block Power Down Enable
#define	UPCR0_CM_DIS					~Hw13					// Common Block Power Down Disable
#define	UPCR0_RCS_11					(Hw12+Hw11)				// Reference Clock Select for PLL Block ; The PLL uses CLKCORE as reference
#define	UPCR0_RCS_10					Hw12					// Reference Clock Select for PLL Block ; The PLL uses CLKCORE as reference
#define	UPCR0_RCS_01					Hw11					// Reference Clock Select for PLL Block ; The XO block uses an external clock supplied on the XO pin
#define	UPCR0_RCS_00					((~Hw12)&(~Hw11))		// Reference Clock Select for PLL Block ; The XO block uses the clock from a crystal
#define	UPCR0_RCD_48					Hw10					// Reference Clock Frequency Select ; 48MHz
#define	UPCR0_RCD_24					Hw9						// Reference Clock Frequency Select ; 24MHz
#define	UPCR0_RCD_12					((~Hw10)&(~Hw9))		// Reference Clock Frequency Select ; 12MHz
#define	UPCR0_SDI_EN					Hw8						// IDDQ Test Enable ; The analog blocks are powered down
#define	UPCR0_SDI_DIS					~Hw8					// IDDQ Test Disable ; The analog blocks are not powered down
#define	UPCR0_FO_SI						Hw7						// UTMI/Serial Interface Select ; Serial Interface
#define	UPCR0_FO_UTMI					~Hw7					// UTMI/Serial Interface Select ; UTMI
#define UPCR0_VBDS						Hw6
#define UPCR0_DMPD						Hw5						// 1:enable / 0:disable the pull-down resistance on D-
#define UPCR0_DPPD						Hw4						// 1: enable, 0:disable the pull-down resistance on D+
#define UPCR0_VBD						Hw1						// The VBUS signal is (1:valid)/(0:invalid), and the pull-up resistor on D+ is (1:enabled)/(0:disabled)

#define UPCR1_TXFSLST(x)				((x&0xF)<<12)
#define UPCR1_SQRXT(x)					((x&0x7)<<8)
#define UPCR1_OTGT(x)					((x&0x7)<<4)
#define UPCR1_CDT(x)					(x&0x7)

#define UPCR2_TM_FS						Hw14
#define UPCR2_TM_HS						0
#define UPCR2_XCVRSEL_LS_ON_FS			(Hw13|Hw12)
#define UPCR2_XCVRSEL_LS				Hw13
#define UPCR2_XCVRSEL_FS				Hw12
#define UPCR2_XCVRSEL_HS				0
#define UPCR2_OPMODE_MASK				(Hw10|Hw9)
#define UPCR2_OPMODE_SYNC_EOP			(Hw10|Hw9)
#define UPCR2_OPMODE_DIS_BITS_NRZI		Hw10
#define UPCR2_OPMODE_NON_DRVING			Hw9
#define UPCR2_OPMODE_NORMAL				0
#define UPCR2_TXVRT(x)					((x&0xF)<<5)
#define UPCR2_TXRT(x)					((x&0x3)<<2)
#define UPCR2_TP_EN						Hw0
#define UPCR2_TP_DIS					0

#define UPCR3_SOFTRESET_DIS				Hw15
#define UPCR3_OTG_DIS					Hw12

const unsigned char * USBPHY_GetVersion(void)
{
	return TCC_USB_PHY_C_Version;
}

//void USBPHY_EnableClock(void)
//{
//	/* MODE=0 for USBOTG, SDI=0 for turn on all analog blocks */
//	BITCLR(pUSBPHYCFG->UPCR0, Hw15|Hw8);
//}

USBPHY_MODE_T USBPHY_SetMode(dwc_otg_core_if_t *_core_if, USBPHY_MODE_T mode)
{
	static USBPHY_MODE_T sUSBPHY_Mode = USBPHY_MODE_RESET;
	USBPHY_MODE_T prevUSBPHY_Mode;
	PUSBPHYCFG pUSBPHYCFG;

#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_MACH_TCC9300ST)
	if(_core_if->port_index!=0)
	{
		pUSBPHYCFG = (PUSBPHYCFG)tcc_p2v(HwUSBPHY1CFG_BASE);
	}
	else
#endif
	{
		pUSBPHYCFG = (PUSBPHYCFG)tcc_p2v(HwUSBPHYCFG_BASE);
	}

	prevUSBPHY_Mode = sUSBPHY_Mode;
	sUSBPHY_Mode = mode;
	//printk("USBPHY_SetMode %d\n",mode);
	switch (sUSBPHY_Mode) {
		//case USB_MODE_NONE:
		//{
		//	pUSBPHYCFG->UPCR0	= 0x2940;	// float DP,PM pins
		//	break;
		//}
		case USBPHY_MODE_RESET:
		{
			pUSBPHYCFG->UPCR0	= 0x4870;
			break;
		}
		case USBPHY_MODE_OTG:
		{
			pUSBPHYCFG->UPCR0	= 0x2800;
			pUSBPHYCFG->UPCR1	=
			    UPCR1_TXFSLST(7)	// FS/LS Pull-Up Resistance Adjustment = Design default
			    | UPCR1_SQRXT(3)	// Squelch Threshold Tune = -5%
			    | UPCR1_OTGT(0)		// VBUS Valid Threshold Adjustment = -12%
			    | UPCR1_CDT(7);		// Disconnect Threshold Adjustment = +6%
			pUSBPHYCFG->UPCR2	=
			    UPCR2_TM_HS
			    | UPCR2_XCVRSEL_HS
			    | UPCR2_OPMODE_NORMAL
			    | UPCR2_TXVRT(12)	// HS DC voltage level adjustment = Design default
			    | UPCR2_TXRT(0)		// HS Transmitter Rise/Fall Time Adjustment = Design default
			    | UPCR2_TP_EN;
			//pUSBPHYCFG->UPCR3	= 0x9000;	// OTG disable
		#if 0//defined(CONFIG_MACH_TCC9300ST) && defined(DWC_HOST_ONLY)
			BITSET(pUSBPHYCFG->UPCR3,Hw5);
		#endif
			break;
		}
		case USBPHY_MODE_ON:
		{
			BITCSET(pUSBPHYCFG->UPCR2, Hw10|Hw9/*UPCR2_OPMODE_MASK*/, 0/*UPCR2_OPMODE_NORMAL*/);
			//pUSBPHYCFG->UPCR0 = 0x2842;
			pUSBPHYCFG->UPCR0 = 0x2800;
			break;
		}
		case USBPHY_MODE_OFF:
		{
			/*
			 * For using Connector ID Status (gotgctl.b.conidsts), PHY is should be always ON.
			 * So TCC93X don't use USBPHY_MODE_OFF except suspend and power control.
			 */
			BITCSET(pUSBPHYCFG->UPCR2, Hw10|Hw9/*UPCR2_OPMODE_MASK*/, Hw9/*UPCR2_OPMODE_NON_DRVING*/);
			pUSBPHYCFG->UPCR0 = 0x4840;
			pUSBPHYCFG->UPCR0 = 0x6940;
			break;
		}
		case USBPHY_MODE_DEVICE_DETACH:
		{
			pUSBPHYCFG->UPCR0 = 0x2840;
			break;
		}
		default:
		{
			printk(KERN_ERR "%s error", __func__);
			break;
		}
	}

	BITSET(pUSBPHYCFG->UPCR3,UPCR3_SOFTRESET_DIS);

	return prevUSBPHY_Mode;
}

#if defined(CONFIG_TCC_DWC_OTG_V281A)
extern int dwc_otg_driver_force_init(void);
#endif
int USBPHY_Power(dwc_otg_core_if_t *_core_if, int power)
{
	if (power)
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC892X)
		if (dwc_otg_is_device_mode(_core_if)) 
			return dwc_otg_driver_force_init();
		else
			return 0;
#else
		return USBPHY_SetMode(_core_if, USBPHY_MODE_ON);
#endif
	else
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC892X)
		return 0;
#else
		return USBPHY_SetMode(_core_if, USBPHY_MODE_OFF);
#endif		
}

EXPORT_SYMBOL(USBPHY_Power);

#ifndef OTGID_ID_HOST
#define OTGID_ID_HOST						(~Hw9)
#endif
#ifndef OTGID_ID_DEVICE
#define OTGID_ID_DEVICE						Hw9
#endif
#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)
void USBPHY_SetID(unsigned int ID)
{
	PUSBOTGCFG pUSBOTGCFG = (PUSBOTGCFG)tcc_p2v(HwUSBHOSTCFG_BASE);

	if ( ID ) {
		pUSBOTGCFG->OTGID |= OTGID_ID_DEVICE;
	} else {
		pUSBOTGCFG->OTGID &= OTGID_ID_HOST;
	}
}
#endif

extern void SetForceFlagMode(int mode);
int urb_dequeue_flag = 0;
void USBPHY_IDCFG(unsigned int idcfg, unsigned int port_index, unsigned int mode)
{
#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)
	;//USBPHY_SetID(0);
#else
	PUSBPHYCFG pUSBPHYCFG;
#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_MACH_TCC9300ST)		
	if(port_index != 0){
		pUSBPHYCFG = (PUSBPHYCFG)tcc_p2v(HwUSBPHY1CFG_BASE);
	}
	else
#endif
	{
		pUSBPHYCFG = (PUSBPHYCFG)tcc_p2v(HwUSBPHYCFG_BASE);
	}

	if (idcfg == 1) {
		if (mode == 1) {
			BITCLR(pUSBPHYCFG->UPCR3, Hw4);	// set host mode
			BITSET(pUSBPHYCFG->UPCR3, Hw5);	// IDCFG enable
		} else {
			BITSET(pUSBPHYCFG->UPCR3, Hw4);	// set device mode
			BITSET(pUSBPHYCFG->UPCR3, Hw5);	// IDCFG enable
		}
	} else {
		BITCLR(pUSBPHYCFG->UPCR3, Hw5);		// IDCFG disable
	}
#endif
}

void w_idcfg_disable(struct work_struct *work)
{
#if defined(CONFIG_MACH_TCC8900) || defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201)	
	;//USBPHY_SetID(1);
#else
	dwc_otg_core_if_t *_core_if = container_of(work, dwc_otg_core_if_t, idcfg_work);
	printk("%s - port_index:%d\n", __func__, _core_if->port_index);
	USBPHY_IDCFG(0, _core_if->port_index, 0);
#endif
}

#if defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_MACH_TCC9300ST)
/*
 * USB Nano-Phy Power Control
 * --------------------------
 * PMU PWROFF Register 0xB0503018
 *   bit3 - UP0 - USB0 Nano Phy Power-Off Control Register
 *   bit2 - UP1 - USB1 Nano Phy Power-Off Control Register
 * 0: Power-Off
 * 1: Power-On
 */
void USBPHY_PWR_Control(int port_index, int on)
{
	unsigned int phy;

	if (port_index != 0)
		phy = PMU_USB1NANOPHY;
	else
		phy = PMU_USBNANOPHY;

	if (on) {
		tca_ckc_setpmupwroff(phy, 1);	//power on
	} else {
		tca_ckc_setpmupwroff(phy, 0);	//power off
	}	
}
#endif

#if defined(CONFIG_MACH_M801_88) || defined(CONFIG_MACH_M803)
void TCC_OTG_PWR_M801(int on)
{
	/* bugfixed: for VBUS detect (IsUSBDET())*/
    if (on) {
	    tca_ckc_setperi(PERI_USB11H, ENABLE, 480000);
    } else {
#if !defined(CONFIG_USB_OHCI_HCD) && !defined(CONFIG_USB_OHCI_HCD_MODULE)
        tca_ckc_setperi(PERI_USB11H, DISABLE, 480000);
#endif
    }
}
EXPORT_SYMBOL(TCC_OTG_PWR_M801);
#endif

#include "tcc_otg_regs.h"
void OTGCORE_Init(dwc_otg_core_if_t *_core_if)
{
	PUSB20OTG pUSB20OTG;

#if defined(CONFIG_ARCH_TCC93XX)
	if(_core_if->port_index!=0)
	{
		pUSB20OTG = (PUSB20OTG)tcc_p2v(HwUSB20OTG1_BASE);
	}
	else
#endif
	{
	#if defined(CONFIG_ARCH_TCC892X)
		pUSB20OTG = (PUSB20OTG)tcc_p2v(HwUSBOTG_BASE);
	#else
		pUSB20OTG = (PUSB20OTG)tcc_p2v(HwUSB20OTG_BASE);
	#endif
	}
	while ( !(pUSB20OTG->GRSTCTL & GRSTCTL_AHBIdle) );

	pUSB20OTG->GUSBCFG |= GUSBCFG_PHYIf_16BITS|GUSBCFG_TOutCal(7);						// set interface, timeout
	pUSB20OTG->GOTGCTL |= GOTGCTL_SesReq;												// session request
	pUSB20OTG->GINTSTS = 0xFFFFFFFF;													// clear interrupts
	pUSB20OTG->GAHBCFG = GAHBCFG_NPTXFEmpLvl_COMPLETELY|GAHBCFG_GlblIntrMsk_UNMASK;	// enable global interrupt

	/* Core Initialization */
	pUSB20OTG->GINTSTS = 0xFFFFFFFF;													// clear interrupts
	pUSB20OTG->GAHBCFG = GAHBCFG_DMAEn|GAHBCFG_HBstLen_SINGLE|GAHBCFG_GlblIntrMsk_UNMASK;
	pUSB20OTG->GUSBCFG |= GUSBCFG_PHYIf_16BITS|GUSBCFG_TOutCal(7);						// set interface, timeout
	//pUSB20OTG->GUSBCFG = GUSBCFG_USBTrdTim_16BIT_UTMIP|GUSBCFG_PHYIf_16BITS|GUSBCFG_TOutCal(7);
	pUSB20OTG->GOTGCTL |= GOTGCTL_SesReq;												// session request
	//pUSB20OTG->GINTMSK = GINTMSK_OTGIntMsk_UNMASK|GINTMSK_ModeMisMsk_UNMASK;
}

/* end of file */
