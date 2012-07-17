/*
 * linux/include/asm-arm/arch-tcc79x/ohci.h 
 *
 * Author:  <linux@telechips.com>
 * Created: August, 2010 
 * Description: ohci definition for TCC79x
 *
 * Copyright (C) 2010 Telechips 
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
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef ASMARM_ARCH_OHCI_H
#define ASMARM_ARCH_OHCI_H


// fields and bits for uhcrev:      HCI Spec Revision
#define USBOHCI_UHCREV_OHCISPEC1_0_A        0x10

// fields and bits for uhchcon:     Control register
#define USBOHCI_UHCCON_CBSR_MASK            ( 0x3u << 0 )

#define USBOHCI_UHCCON_CBSR_PLE             ( 2u   << 2 )
#define USBOHCI_UHCCON_CBSR_IE              ( 3u   << 2 )
#define USBOHCI_UHCCON_CBSR_CLE             ( 4u   << 2 )
#define USBOHCI_UHCCON_CBSR_BLE             ( 5u   << 2 )

#define USBOHCI_UHCCON_HCFS_MASK            ( 0x3u << 6 )

#define USBOHCI_UHCCON_CBSR_IR              ( 8u   << 2 )
#define USBOHCI_UHCCON_CBSR_RWC             ( 9u   << 2 )
#define USBOHCI_UHCCON_CBSR_RWE             ( 10u  << 2 )

// for the HCFS field
#define USBOHCI_UHCCON_HCFS_USBRESET        ( 0u << 6 )
#define USBOHCI_UHCCON_HCFS_USBRESUME       ( 1u << 6 )
#define USBOHCI_UHCCON_HCFS_USBOPERATIONAL  ( 2u << 6 )
#define USBOHCI_UHCCON_HCFS_USBSUSPEND      ( 3u << 6 )


// fields and bits for uhccoms:     Command Status
#define USBOHCI_UHCCOMS_HCR                 ( 1u   << 0)
#define USBOHCI_UHCCOMS_CLF                 ( 1u   << 1 )
#define USBOHCI_UHCCOMS_BLF                 ( 1u   << 2 )
#define USBOHCI_UHCCOMS_OCR                 ( 1u   << 3 )

#define USBOHCI_UHCCOMS_SOC_MASK            ( 0x3u << 16 )



// fields and bits for uhcints:     Interrupt Status
// fields and bits for uhcinte:     Interrupt Enable Control register
// fields and bits for uhcintd:     Interrupt Disable Control register
#define USBOHCI_UHCINT_SO                   ( 1u   << 0 )
#define USBOHCI_UHCINT_WDH                  ( 1u   << 1 )
#define USBOHCI_UHCINT_SF                   ( 1u   << 2 )
#define USBOHCI_UHCINT_RD                   ( 1u   << 3 )
#define USBOHCI_UHCINT_UE                   ( 1u   << 4 )
#define USBOHCI_UHCINT_FNO                  ( 1u   << 5 )
#define USBOHCI_UHCINT_RHSC                 ( 1u   << 6 )
#define USBOHCI_UHCINT_OC                   ( 1u   << 30 )
#define USBOHCI_UHCINT_MIE                  ( 1u   << 31 )

// fields and bits for uhchcca:     Host controller Communication Area
#define USBOHCI_UHCHCCA_MASK                ( 0xfffffffu << 8 )

// fields and bits for uhcpced:     Period Current Endpoint Descriptor
#define USBOHCI_UHCPCED_MASK                ( 0xffffffffu << 4 )

// fields and bits for uhcched:     Control Head Endpoint Descriptor register
#define USBOHCI_UHCCHED_MASK                ( 0xffffffffu << 4 )

// fields and bits for uhccced:     Control Current Endpoint Descriptor register
#define USBOHCI_UHCCCED_MASK                ( 0xffffffffu << 4 )

// fields and bits for uhcbhed:     Bulk Head Endpoint Descriptor register
#define USBOHCI_UHCBHED_MASK                ( 0xffffffffu << 4 )

// fields and bits for uhcbced:     Bulk Current Endpoint Descriptor register
#define USBOHCI_UHCBCED_MASK                ( 0xffffffffu << 4 )

// fields and bits for uhcdhead:    Done head register
#define USBOHCI_UHCDHED_MASK                ( 0xffffffffu << 4 )    // should be "DHTD" because its a transfer descriptor

// fields and bits for uhcfmi:      Frame Interval register
#define USBOHCI_UHCFMI_FI_MASK              ( 0x3fffu << 0 )
#define USBOHCI_UHCFMI_FSMPS_MASK           ( 0x7fffu << 16 )
#define USBOHCI_UHCFMI_FIT                  ( 1u   << 31 )

// fields and bits for uhcfmr:      Frame Remaining register
#define USBOHCI_UHCFMR_FR_MASK              ( 0x3fffu << 0 )
#define USBOHCI_UHCFMI_FRT                  ( 1u   << 31 )

// fields and bits for uhcfmn:      Frame Number register
#define USBOHCI_UHCFMN_FN_MASK              ( 0xffffu << 0 )

// fields and bits for uhcpers:     Periodic Start register
#define USBOHCI_UHCPERS_PS_MASK             ( 0x3fffu << 0 )

// fields and bits for uhclst:      Low Speed Threshold register
#define USBOHCI_UHCPLST_LST_MASK            ( 0xfffu << 0 )

// fields and bits for uhcrhda:     Root Hub Descriptor A register
#define USBOHCI_UHCRHDA_NDP_MASK            ( 0xffu << 0 )

#define USBOHCI_UHCRHDA_PSM                 ( 1u   << 8 )
#define USBOHCI_UHCRHDA_NPS                 ( 1u   << 9 )
#define USBOHCI_UHCRHDA_DT                  ( 1u   << 10 )
#define USBOHCI_UHCRHDA_OCPM                ( 1u   << 11 )
#define USBOHCI_UHCRHDA_NOCP                ( 1u   << 12 )

#define USBOHCI_UHCRHDA_POTPGT_MASK         ( 0xffu << 24 )

// fields and bits for uhcrhdb:     Root Hub Descriptor B register
#define USBOHCI_UHCRHDB_DR_MASK             ( 0xffffu << 0 )
#define USBOHCI_UHCRHDB_PPCM_MASK           ( 0xffffu << 16 )

// fields and bits for uhcrhs:      Root Hub Status register
#define USBOHCI_UHCRHS_LPS                  ( 1u   << 0 )       // meaning on read
#define USBOHCI_UHCRHS_CGP                  ( 1u   << 0 )       // meaining on write
#define USBOHCI_UHCRHS_OCI                  ( 1u   << 1 )
#define USBOHCI_UHCRHS_DRWE                 ( 1u   << 15 )      // meaning on read
#define USBOHCI_UHCRHS_SRWE                 ( 1u   << 15 )      // meaning on write
#define USBOHCI_UHCRHS_LPSC                 ( 1u   << 16 )      // meaning on read
#define USBOHCI_UHCRHS_SGP                  ( 1u   << 16 )      // meaning on write
#define USBOHCI_UHCRHS_OCIC                 ( 1u   << 17 )
#define USBOHCI_UHCRHS_CRWE                 ( 1u   << 31 )

// fields and bits for uhcrhps1:    Root Hub Port 1 Status register
// fields and bits for uhcrhps2:    Root Hub Port 2 Status register
#define USBOHCI_UHCRHPS_CCS                 ( 1u   << 0 )
#define USBOHCI_UHCRHPS_PES                 ( 1u   << 1 )
#define USBOHCI_UHCRHPS_PSS                 ( 1u   << 2 )
#define USBOHCI_UHCRHPS_POCI                ( 1u   << 3 )
#define USBOHCI_UHCRHPS_PRS                 ( 1u   << 4 )
#define USBOHCI_UHCRHPS_PPS                 ( 1u   << 8 )
#define USBOHCI_UHCRHPS_LSDA                ( 1u   << 9 )       // meaning on read
#define USBOHCI_UHCRHPS_CPP                 ( 1u   << 9 )       // meaning on write
#define USBOHCI_UHCRHPS_CSC                 ( 1u   << 16 )
#define USBOHCI_UHCRHPS_PESC                ( 1u   << 17 )
#define USBOHCI_UHCRHPS_PSSC                ( 1u   << 18 )
#define USBOHCI_UHCRHPS_POCIC               ( 1u   << 19 )
#define USBOHCI_UHCRHPS_PRSC                ( 1u   << 20 )

// fields and bits for uhcstat:     USB Host Status
#define USBOHCI_UHCSTAT_RWUE                ( 1u   << 7 )
#define USBOHCI_UHCSTAT_HBA                 ( 1u   << 8 )
#define USBOHCI_UHCSTAT_HTA                 ( 1u   << 10 )
#define USBOHCI_UHCSTAT_UPS1                ( 1u   << 11 )
#define USBOHCI_UHCSTAT_UPS2                ( 1u   << 12 )
#define USBOHCI_UHCSTAT_UPRI                ( 1u   << 13 )
#define USBOHCI_UHCSTAT_SBTAI               ( 1u   << 14 )
#define USBOHCI_UHCSTAT_SBMAI               ( 1u   << 15 )

// fields and bits for uhchr:       USB Host Reset
// UHCHR host reset register bit positions
#define USBOHCI_UHCHR_SSEP1                 ( 1u << 10) // Sleep Standby Enable: enable/disable port1 SE receivers & power  supply
#define USBOHCI_UHCHR_SSEP0                 ( 1u <<  9) // Sleep Standby Enable: enable/disable port1 SE receivers & power  supply
#define USBOHCI_UHCHR_PCPL                  ( 1u <<  7) // Power CONTROL Polarity: Control polarity of Power Enable signals output to the MAX1693EUB USB Power Switch
#define USBOHCI_UHCHR_PSPL                  ( 1u <<  6) // Power SENSE Polarity: Control polarity of Over-current Indicator signals input from the MAX1693EUB USB Power Switch
#define USBOHCI_UHCHR_SSE                   ( 1u <<  5) // Sleep Standby Enable: enable/disable both ports SE receivers & power  supply
#define USBOHCI_UHCHR_UIT                   ( 1u <<  4) // USB Interrupt Test: Enable Interrupt Test Mode and UHCHIT register
#define USBOHCI_UHCHR_SSDC                  ( 1u <<  3) // Simulation Scale Down Clock: When 1, internal 1 mSec timer changes to 1 uSec to speed up simulations
#define USBOHCI_UHCHR_CGR                   ( 1u <<  2) // Clock Generation Reset: When 0, resets the OHCI Clock Generation block (DPLL). Used only in simulation
#define USBOHCI_UHCHR_FHR                   ( 1u <<  1) // Force Host controller Reset: When 1, resets OHCI core. Must be held high for 10 uSeconds, then cleared
#define USBOHCI_UHCHR_FSBIR                 ( 1u <<  0) // Force System Bus Interface Reset: When 1, resets the logic that interfaces to the system bus, DMA, etc. Auto clears after three system bus clocks.

// fields and bits for uhchie:      USB Host Interrupt Enable
#define USBOHCI_UHCIE_RWIE                 ( 1u   << 7 )
#define USBOHCI_UHCIE_HBAIE                ( 1u   << 8 )
#define USBOHCI_UHCIE_TAIE                 ( 1u   << 10 )
#define USBOHCI_UHCIE_UPS1IE               ( 1u   << 11 )
#define USBOHCI_UHCIE_UPS2IE               ( 1u   << 12 )
#define USBOHCI_UHCIE_UPRIE                ( 1u   << 13 )


// fields and bits for uhchit:      USB Host Interrupt Test
#define USBOHCI_UHCIT_RWIT                 ( 1u   << 7 )
#define USBOHCI_UHCIT_BAT                  ( 1u   << 8 )
#define USBOHCI_UHCIT_IRQT                 ( 1u   << 9 )
#define USBOHCI_UHCIT_TAT                  ( 1u   << 10 )
#define USBOHCI_UHCIT_UPS1T                ( 1u   << 11 )
#define USBOHCI_UHCIT_UPS2T                ( 1u   << 12 )
#define USBOHCI_UHCIT_UPRT                 ( 1u   << 13 )
#define USBOHCI_UHCIT_STAT                 ( 1u   << 14 )
#define USBOHCI_UHCIT_SMAT                 ( 1u   << 15 )



// root hub descriptor A information
#define USBOHCI_UHCRHDA_PSM_PERPORT        1


#define INT_CTRL_BASE                           0xF3001000
#define IEN_REG_OFFSET                          0x0
#define CLR_REG_OFFSET                          0x4
#define SEL_REG_OFFSET                          0xC
#define POL_REG_OFFSET                          0x1C

#define CLK_CTRL_BASE                           0xF3000000
#define BCLK_CTRL_OFFSET                        0x18
#define SWRESET_CTRL_OFFSET                     0x1C

#define HwBCLK                                  *(volatile unsigned long *)(CLK_CTRL_BASE+BCLK_CTRL_OFFSET)

#define IO_CKC_BUS_UBH                          0x00000001
#define IO_CKC_BUS_UBD                          0x00000002



struct device;

struct tccohci_platform_data {
	int (*init)(struct device *);
	void (*exit)(struct device *);

	int port_mode;
#define USBOHCI_PPM_NPS     1
#define USBOHCI_PPM_GLOBAL  2
#define USBOHCI_PPM_PERPORT 3
#define USBOHCI_PPM_MIXED   4

	int power_budget;
};

extern void tcc_set_ohci_info(struct tccohci_platform_data *info);

#endif
