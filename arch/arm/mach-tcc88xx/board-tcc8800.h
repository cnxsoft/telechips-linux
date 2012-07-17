/*
 * linux/arch/arm/mach-tcc88xx/board-tcc8800.h
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

#ifndef __ARCH_ARM_MACH_TCC88XX_BOARD_TCC8800_H
#define __ARCH_ARM_MACH_TCC88XX_BOARD_TCC8800_H


/**************************************************************
	GPIO Port
**************************************************************/

// Nand
#define GPIO_NAND_RDY0		TCC_GPB(29)
#define GPIO_NAND_RDY1		TCC_GPB(28)
#define GPIO_NAND_WP		TCC_GPB(27)

// ETC/PCIE
#define GPIO_PWR_SHDN		TCC_GPA(2)
#define GPIO_PWR_KEY		TCC_GPA(3)
#define GPIO_IR_SENSOR		TCC_GPA(5)

//#define INT_EDI_IRQ			TCC_GPB(31)
#define INT_GPEPD_INT		TCC_GPB(31)

#define GPIO_SATA0_ON		TCC_GPF(15)
#define GPIO_SATA1_ON		TCC_GPF(16)

// LCD
#define GPIO_LCD_BL			TCC_GPA(4)
#define GPIO_LCD_DISPLAY	TCC_GPC(29)

// Touch
#define INT_PENIRQ			TCC_GPA(6)

// GPS
#define GPIO_GPS_PWDN		TCC_GPD(21)
#define INT_GPS_IRQ			TCC_GPE(24)

// GMAC
#define GPIO_PHY_RST		TCC_GPG(28)
#define GPIO_PHY_ON			TCC_GPG(29)
#define INT_PHY_IRQ			TCC_GPF(17)

// CAM
#define GPIO_CAM_RST		TCC_GPG(30)
#define GPIO_CAM_STDBY		TCC_GPG(31)

// G-Sensor
#define INT_GSEN_IRQ		TCC_GPF(25)
#define INT_COMPASS_IRQ		TCC_GPF(26)

// DxB
#define GPIO_DXB0_SFRM		TCC_GPE(2)
#define GPIO_DXB0_SCLK		TCC_GPE(3)
#define GPIO_DXB0_RST		TCC_GPE(4)
#define INT_DXB0_IRQ		TCC_GPE(5)
#define GPIO_DXB0_SDI		TCC_GPE(6)
#define GPIO_DXB0_SDO		TCC_GPE(7)
#define GPIO_DXB1_SFRM		TCC_GPD(5)
#define GPIO_DXB1_SCLK		TCC_GPD(6)
#define GPIO_DXB1_SDI		TCC_GPD(7)
#define GPIO_DXB1_SDO		TCC_GPD(8)
#define GPIO_DXB1_RST		TCC_GPD(9)
#define INT_DXB1_IRQ		TCC_GPD(10)
#define GPIO_DXB_UARTTX		TCC_GPF(26)
#define GPIO_DXB_UARTRX		TCC_GPF(27)

// MIPI
#define GPIO_MIPIS_PWRON	TCC_GPE(8)
#define GPIO_MIPIS_FL_EN	TCC_GPE(9)
#define GPIO_MIPIM_PWRON	TCC_GPE(10)
#define GPIO_MIPIM_RST		TCC_GPE(11)


/**************************************************************
	GPIO Expander
**************************************************************/

// GPEXT1
#define GPIO_LCD_ON			TCC_GPEXT1(0)
#define GPIO_CODEC_ON		TCC_GPEXT1(1)
#define GPIO_SD0_ON			TCC_GPEXT1(2)
#define GPIO_SD1_ON			TCC_GPEXT1(3)
#define GPIO_SD2_ON			TCC_GPEXT1(4)
#define GPIO_EXT_CODEC_ON	TCC_GPEXT1(5)
#define GPIO_GPS_PWREN		TCC_GPEXT1(6)
#define GPIO_BT_ON			TCC_GPEXT1(7)

#define GPIO_DXB_ON			TCC_GPEXT1(8)
#define GPIO_IPOD_ON		TCC_GPEXT1(9)
#define GPIO_CAS_ON			TCC_GPEXT1(10)
#define GPIO_FMTC_ON		TCC_GPEXT1(11)
#define GPIO_PCAM_PWR_ON	TCC_GPEXT1(12)
#define GPIO_CAM1_ON		TCC_GPEXT1(13)
#define GPIO_CAM2_ON		TCC_GPEXT1(14)
#define GPIO_ATAPI_ON		TCC_GPEXT1(15)

// GPEXT2
#define GPIO_MUTE_CTL		TCC_GPEXT2(0)
#define GPIO_LVDSIVT_ON		TCC_GPEXT2(1)
#define GPIO_LVDS_LP_CTRL	TCC_GPEXT2(2)
#define GPIO_AUTH_RST		TCC_GPEXT2(3)
#define GPIO_BT_RST			TCC_GPEXT2(4)
#define GPIO_SDWF_RST		TCC_GPEXT2(5)
#define GPIO_CAS_RST		TCC_GPEXT2(6)
#define GPIO_CAS_GP			TCC_GPEXT2(7)

#define GPIO_DXB1_PD		TCC_GPEXT2(8)
#define GPIO_DXB2_PD		TCC_GPEXT2(9)
#define GPIO_PWR_CONTROL0	TCC_GPEXT2(10)
#define GPIO_PWR_CONTROL1	TCC_GPEXT2(11)
#define GPIO_HDD_RST		TCC_GPEXT2(12)
#define GPIO_OTG0_VBUS_EN	TCC_GPEXT2(13)
#define GPIO_OTG1_VBUS_EN	TCC_GPEXT2(14)
#define GPIO_HOST_VBUS_EN	TCC_GPEXT2(15)

// GPEXT3
#define GPIO_FMT_RST		TCC_GPEXT3(0)
#define GPIO_FMT_IRQ		TCC_GPEXT3(1)
#define GPIO_BT_WAKE		TCC_GPEXT3(2)
#define GPIO_BT_HWAKE		TCC_GPEXT3(3)
#define GPIO_PHY2_ON		TCC_GPEXT3(4)
#define GPIO_COMPASS_RST	TCC_GPEXT3(5)
#define GPIO_CAM_FL_EN		TCC_GPEXT3(6)
#define GPIO_CAM2_FL_EN		TCC_GPEXT3(7)

#define GPIO_CAM2_RST		TCC_GPEXT3(8)
#define GPIO_CAM2_PWDN		TCC_GPEXT3(9)
#define GPIO_LCD_RESET		TCC_GPEXT3(10)
#define GPIO_TV_SLEEP		TCC_GPEXT3(11)
#define GPIO_ETH_ON			TCC_GPEXT3(12)
#define GPIO_ETH_RST		TCC_GPEXT3(13)
#define GPIO_SMART_AUX1		TCC_GPEXT3(14)
#define GPIO_SMART_AUX2		TCC_GPEXT3(15)

// GPEXT5	(SV60 Power Sub Board)
#define GPIO_V_5P0_EN		TCC_GPEXT5(0)
#define GPIO_OTG_EN			TCC_GPEXT5(1)
#define GPIO_HS_HOST_EN		TCC_GPEXT5(2)
#define GPIO_FS_HOST_EN		TCC_GPEXT5(3)
#define GPIO_HDMI_EN		TCC_GPEXT5(4)
#define GPIO_MIPI_EN		TCC_GPEXT5(5)
#define GPIO_SATA_EN		TCC_GPEXT5(6)
#define GPIO_LVDS_EN		TCC_GPEXT5(7)

#define GPIO_ATV_EN			TCC_GPEXT5(8)
#define GPIO_ATAPI_IPOD_EN	TCC_GPEXT5(9)
#define GPIO_USB_CHARGE_SEL	TCC_GPEXT5(10)
#define GPIO_USB_SUSPEND	TCC_GPEXT5(11)
#define GPIO_CHARGE_EN		TCC_GPEXT5(12)
#define GPIO_SD30_PWR_EN	TCC_GPEXT5(13)
#define GPIO_SD30_PWR_CTL	TCC_GPEXT5(14)
#define INT_CHARGE_FINISH	TCC_GPEXT5(15)


/**************************************************************
	Externel Interrupt
**************************************************************/

#define EINT_USB			HwINT0_EI0
#define EINT_COMPASS		HwINT0_EI1
#define EINT_TOUCH			HwINT0_EI2
#define EINT_TDMB			HwINT0_EI3

#define EINT_SDMMC0			HwINT1_EI4
#define EINT_SDMMC1			HwINT1_EI5
#define PMIC_IRQ			INT_EI6
// #define EINT_LCDC			HwINT1_EI6
#define EINT_GPS			HwINT1_EI7

//#define INT_NAND			



#endif
