/*
 * linux/drivers/video/tcc/tccfb.h
 *
 * Author: <linux@telechips.com>
 * Created: June 10, 2008
 * Description: TCC LCD Controller Frame Buffer Driver
 *
 * Copyright (C) 2008-2009 Telechips 
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

#ifndef __TCCFB_H
#define __TCCFB_H

struct tccfb_platform_data;

struct tccfb_info {
	struct fb_info		*fb;
	struct device		*dev;

	//struct tccfb_mach_info *mach_info;

	/* raw memory addresses */
	dma_addr_t		map_dma;	/* physical */
	u_char *		map_cpu;	/* virtual */
	u_int			map_size;

	/* addresses of pieces placed in raw buffer */
	u_char *		screen_cpu;	/* virtual address of buffer */
	dma_addr_t		screen_dma;	/* physical address of buffer */

	u_int			imgch;

	struct tccfb_platform_data *pdata;
#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
    struct early_suspend earlier_suspend;
#endif


};

#define PALETTE_BUFF_CLEAR (0x80000000)	/* entry is clear/invalid */

#if defined(CONFIG_TCC_VIDEO_DISPLAY_BY_VSYNC_INT)
#define TCC_VIDEO_DISPLAY_BY_VSYNC_INT
#endif

#if defined(CONFIG_TCC_VIDEO_DISPLAY_DEINTERLACE_MODE)
#define TCC_VIDEO_DISPLAY_DEINTERLACE_MODE
#endif

//int tccfb_init(void);

#endif
