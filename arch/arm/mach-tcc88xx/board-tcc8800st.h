/*
 * linux/arch/arm/mach-tcc88xx/board-tcc8800st.h
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

#ifndef __ARCH_ARM_MACH_TCC88XX_BOARD_TCC8800ST_H
#define __ARCH_ARM_MACH_TCC88XX_BOARD_TCC8800ST_H

#define GPIO_PWR_KEY		TCC_GPA(3)
#define GPIO_LCD_BL			0xFFFFFFFF

#define GPIO_LCD_DISPLAY	0xFFFFFFFF
#define GPIO_LCD_RESET		0xFFFFFFFF
#define GPIO_LCD_ON			0xFFFFFFFF

#define GPIO_SD1_ON			0xFFFFFFFF
#define GPIO_SDWF_RST		TCC_GPE(25)

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
#define GPIO_DXB0_PD		TCC_GPE(9)
#define GPIO_DXB1_PD		TCC_GPD(16)
#define GPIO_DXB_ON		    TCC_GPF(14) //In case of TCC9300ST, GPIO_TSIF_ON

#endif
