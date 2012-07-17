/*
 * linux/arch/arm/mach-tcc88xx/board-m803.h
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

#ifndef __ARCH_ARM_MACH_TCC88XX_BOARD_M803_H
#define __ARCH_ARM_MACH_TCC88XX_BOARD_M803_H

#define GPIO_PWR_KEY		TCC_GPA(3)
#define GPIO_LCD_BL			TCC_GPA(4)

#define GPIO_LCD_DISPLAY	0xFFFFFFFF  //TCC_GPE(1) // todo: modify
#define GPIO_LCD_RESET		TCC_GPC(29)
#define GPIO_LCD_ON			TCC_GPG(2)

#define GPIO_LVDS_EN		TCC_GPG(2)
#define GPIO_LVDSIVT_ON		TCC_GPA(4)
#define GPIO_LVDS_LP_CTRL1	TCC_GPC(29)
#define GPIO_LVDS_LP_CTRL2	TCC_GPC(24)


#define USB_IRQ				INT_EI0
#define COMPASS_IRQ			INT_EI1
#define TOUCH_IRQ			INT_EI2
#define SDMMC_IRQ			INT_EI4
#define PMIC_IRQ			INT_EI5

// Nand
#define GPIO_NAND_RDY		TCC_GPB(29)
#define GPIO_NAND_WP		TCC_GPB(27)



#endif
