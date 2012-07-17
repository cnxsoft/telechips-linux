/* arch/arm/mach-tcc92xx/tcc_display.c
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

#include <asm/mach/flash.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <asm/setup.h>

#include <mach/tccfb_ioctrl.h>

#define ATAG_TCC_DISPLAY 0x54434380 /* TCCp*/

struct tag_tcc_display {
	__u32 output;
	__u32 resolution;
	__u32 hdmi_resolution;
	__u32 composite_resolution;
	__u32 component_resolution;
	__u32 hdmi_mode;
};

extern struct display_platform_data tcc_display_data;

static int __init parse_tag_tcc_display(const struct tag *tag)
{
	struct tag_tcc_display *entry = (void *) &tag->u;

	tcc_display_data.output = entry->output;
	tcc_display_data.resolution = entry->resolution;
	tcc_display_data.hdmi_resolution = entry->hdmi_resolution;
	tcc_display_data.composite_resolution = entry->composite_resolution;
	tcc_display_data.component_resolution = entry->component_resolution;
	tcc_display_data.hdmi_mode = entry->hdmi_mode;

	printk("kernel start display option [output:%d] [resolution:%d] [hdmi_r:%d] [composite_r:%d] [component_r:%d] [hdmi_m:%d]\n", 
		tcc_display_data.output, tcc_display_data.resolution, tcc_display_data.hdmi_resolution, tcc_display_data.composite_resolution, tcc_display_data.component_resolution, tcc_display_data.hdmi_mode);

	return 0;
}

__tagtable(ATAG_TCC_DISPLAY, parse_tag_tcc_display);
