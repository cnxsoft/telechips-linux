/* linux/arch/arm/mach-tcc88xx/include/mach/mmc.h
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

#ifndef __TCC_MMC_H
#define __TCC_MMC_H

#include <linux/types.h>
#include <linux/device.h>
#include <linux/mmc/host.h>

struct tcc_mmc_platform_data {
	int slot;
	unsigned int caps;
	unsigned int f_min;
	unsigned int f_max;
	u32 ocr_mask;

	int (*init)(struct device *dev, int id);

	int (*card_detect)(struct device *dev, int id);

	int (*cd_int_config)(struct device *dev, int id, unsigned int cd_irq);	

	int (*suspend)(struct device *dev, int id);
	int (*resume)(struct device *dev, int id);

	int (*set_power)(struct device *dev, int id, int on);

	int (*set_bus_width)(struct device *dev, int id, int width);

	unsigned int cd_int_num;
	unsigned int cd_irq_num;
	unsigned int peri_name;			/* sd core name */
	unsigned int io_name;			/* sd core name */	
	unsigned int pic;			/* Priority Interrupt Controller register name */
	
};

struct mmc_port_config {
	int data0;
	int data1;
	int data2;
	int data3;
	int data4;
	int data5;
	int data6;
	int data7;
	int clk;
	int cmd;
	int cd;

	int pwr;
};


#endif
