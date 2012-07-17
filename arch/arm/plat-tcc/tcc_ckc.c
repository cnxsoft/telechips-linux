/*
 * linux/arch/arm/plat-tcc/tcc_ckc.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: tcc clock control functions.
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include "tca_ckc.h"

EXPORT_SYMBOL(tca_ckc_setsystemsource);
EXPORT_SYMBOL(tca_ckc_getsystemsource);
EXPORT_SYMBOL(tca_ckc_setperibus);
#ifdef CONFIG_ARCH_TCC79X
EXPORT_SYMBOL(tca_ckc_setperibusmask);
#endif
EXPORT_SYMBOL(tca_ckc_resetperi);
EXPORT_SYMBOL(tca_ckc_currentpll);
EXPORT_SYMBOL(tca_ckc_peri);
EXPORT_SYMBOL(tca_ckc_setperi);
EXPORT_SYMBOL(tca_ckc_getperi);
EXPORT_SYMBOL(tca_ckc_getcurrentsys);
EXPORT_SYMBOL(tca_ckc_getcurrentbus);
EXPORT_SYMBOL(tca_ckc_getcurrentcpu);
EXPORT_SYMBOL(tca_ckc_getcurrentsubcpu);
EXPORT_SYMBOL(tca_ckc_validpll);
EXPORT_SYMBOL(tca_ckc_setpll);
EXPORT_SYMBOL(tca_ckc_setmaincpu);
EXPORT_SYMBOL(tca_ckc_setsubcpu);
EXPORT_SYMBOL(tca_ckc_setcpu);
EXPORT_SYMBOL(tca_ckc_setbus );
#ifdef CONFIG_ARCH_TCC79X
EXPORT_SYMBOL(tca_ckc_minsubcpu);
EXPORT_SYMBOL(tca_ckc_closesubcpu);
EXPORT_SYMBOL(tca_ckc_opensubcpu );
#endif
EXPORT_SYMBOL(tca_ckc_setclock );

