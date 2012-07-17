/*
 * linux/arch/arm/mach-tcc88xx/board-m801_88.h
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

#ifndef __ARCH_ARM_MACH_TCC88XX_BOARD_M801_88_H
#define __ARCH_ARM_MACH_TCC88XX_BOARD_M801_88_H

#define		M805_SUPPORT //This is upgrade version of M801

#define GPIO_NAND_RDY0		TCC_GPB(29)
#define GPIO_NAND_RDY1		TCC_GPB(28)
#define GPIO_NAND_WP		TCC_GPB(27)

#define GPIO_PWR_KEY		TCC_GPA(3)
#define GPIO_LCD_BL			TCC_GPA(4)

#define GPIO_LCD_DISPLAY	0xFFFFFFFF  //TCC_GPE(1) // todo: modify
#define GPIO_LCD_RESET		TCC_GPC(29)
#define GPIO_LCD_ON			TCC_GPG(2)

#define USB_IRQ				INT_EI0
#define COMPASS_IRQ			INT_EI1
#define TOUCH_IRQ			INT_EI2
#define SDMMC_IRQ			INT_EI4
#define PMIC_IRQ			INT_EI5

// Nand
#define GPIO_NAND_RDY		TCC_GPB(29)
#define GPIO_NAND_WP		TCC_GPB(27)

// DxB
#define GPIO_DXB0_SFRM		TCC_GPE(2)
#define GPIO_DXB0_SCLK		TCC_GPE(3)

#define INT_DXB0_IRQ		TCC_GPE(5)
#define GPIO_DXB0_SDI		TCC_GPE(6)
#define GPIO_DXB0_SDO		TCC_GPE(7)
#define GPIO_DXB1_SFRM		TCC_GPD(5)
#define GPIO_DXB1_SCLK		TCC_GPD(6)
#define GPIO_DXB1_SDI		TCC_GPD(7)
#define GPIO_DXB1_SDO		TCC_GPD(8)
#define GPIO_DXB1_RST		//TCC_GPD(9)
#define INT_DXB1_IRQ		//TCC_GPD(10)
#ifdef		M805_SUPPORT
#define GPIO_DXB_PWDN		TCC_GPE(12)
#define GPIO_DXB_PWREN		TCC_GPG(5)
#define GPIO_DXB0_RST		TCC_GPE(13)
#define GPIO_RFSW_CTL0		TCC_GPE(14)
#else
#define GPIO_DXB_PWDN		TCC_GPE(8)
#define GPIO_DXB_PWREN		TCC_GPG(5)
#define GPIO_DXB0_RST		TCC_GPE(9)
#define GPIO_RFSW_CTL0		TCC_GPE(1)
#endif

#define GPIO_RFSW_CTL3		TCC_GPF(13)
#define GPIO_RFSW_CTL2		TCC_GPE(24)
#define GPIO_RFSW_CTL1		TCC_GPE(25)



#endif
