/* linux/arch/arm/mach-tcc88xx/include/mach/tcc_fb.h
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

#ifndef __TCC_FB_H
#define __TCC_FB_H

#define TCC_LCDC1_USE

#define ATAG_TCC_PANEL	0x54434364 /* TCCd */

enum {
	PANEL_ID_LMS350DF01,
	PANEL_ID_LMS480KF01,
	PANEL_ID_DX08D11VM0AAA,
	PANEL_ID_LB070WV6,
	PANEL_ID_CLAA104XA01CW,
	PANEL_ID_HT121WX2,
	PANEL_ID_TD043MGEB1,
	PANEL_ID_AT070TN93,
	PANEL_ID_TD070RDH11,
	PANEL_ID_N101L6,
	PANEL_ID_TW8816,
	PANEL_ID_CLAA102NA0DCW,
	PANEL_ID_ED090NA,	
	PANEL_ID_KR080PA2S,	
	PANEL_ID_HDMI	
};

struct lcd_platform_data {
	unsigned power_on;
	unsigned display_on;
	unsigned bl_on;
	unsigned reset;
};

struct lcd_panel {
	const char *name;
	const char *manufacturer;

	struct device *dev;

	int id;			/* panel ID */
	int xres;		/* x resolution in pixels */
	int yres;		/* y resolution in pixels */
	int width;		/* display width in mm */
	int height;		/* display height in mm */
	int bpp;		/* bits per pixels */

	int clk_freq;
	int clk_div;
	int bus_width;
	int lpw;		/* line pulse width */
	int lpc;		/* line pulse count */
	int lswc;		/* line start wait clock */
	int lewc;		/* line end wait clock */
	int vdb;		/* back porch vsync delay */
	int vdf;		/* front porch vsync delay */
	int fpw1;		/* frame pulse width 1 */
	int flc1;		/* frame line count 1 */
	int fswc1;		/* frame start wait cycle 1 */
	int fewc1;		/* frame end wait cycle 1 */
	int fpw2;		/* frame pulse width 2 */
	int flc2;		/* frame line count 2 */
	int fswc2;		/* frame start wait cycle 2 */
	int fewc2;		/* frame end wait cycle 2 */
	int sync_invert;

	int (*init)(struct lcd_panel *panel);
	int (*set_power)(struct lcd_panel *panel, int on ,unsigned int lcd_num);
	int (*set_backlight_level)(struct lcd_panel *panel, int level);

	int state; //current state 0 off , 0: on
	int bl_level; //current backlight level
};

int tccfb_register_panel(struct lcd_panel *panel);
struct lcd_panel *tccfb_get_panel(void);

#endif
