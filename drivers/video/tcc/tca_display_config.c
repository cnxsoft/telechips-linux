/*
 * linux/drivers/video/tcc/tca_disaplay_config.c
 *
 * Based on:    
 * Author:  <linux@telechips.com>
 * Created: Jan 11, 2012
 * Description: TCC LCD Controller Number define
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/init.h>

unsigned int tca_get_lcd_lcdc_num(viod)
{
	#ifdef CONFIG_LCD_LCDC0_USE
		return 0;
	#else
		#ifdef CONFIG_ARCH_TCC892X
			#ifdef CONFIG_MACH_TCC8920ST
				return 0;
			#else
				if(system_rev == 0x1006 || system_rev == 0x1008)
					return 0;
				else
					return 1;			
			#endif
		#endif
	#endif

	//default LCDC number
	return 1;
}
EXPORT_SYMBOL(tca_get_lcd_lcdc_num);

unsigned int tca_get_output_lcdc_num(viod)
{
	#ifdef CONFIG_LCD_LCDC0_USE
		return 1;
	#else
		#ifdef CONFIG_ARCH_TCC892X
			#ifdef CONFIG_MACH_TCC8920ST
				return 1;
			#else
				if(system_rev == 0x1006 || system_rev == 0x1008)
					return 1;
				else
					return 0;			
			#endif
		#endif
	#endif

	//default HDMI LCDC number
	return 0;
}
EXPORT_SYMBOL(tca_get_output_lcdc_num);

