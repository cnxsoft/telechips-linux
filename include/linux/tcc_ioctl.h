/*
 * File:        include/linux/tcc_ioctl.h
 *
 * Created:     June 10, 2008
 * Copyright (C) 2008-2009 Telechips <linux@telechips.com>
 * Description: header file of driver/char/tcc_ioctl.c
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
#ifndef __TCC_IOCTL_H__
#define __TCC_IOCTL_H__

#include <mach/ioctl_ckcstr.h>

#define MAJOR_NUM 247

#define IOCTL_CKC_SET_PERI          _IO(MAJOR_NUM, 0)
#define IOCTL_CKC_GET_PERI          _IO(MAJOR_NUM, 1)
#define IOCTL_CKC_SET_PERIBUS       _IO(MAJOR_NUM, 2)
#define IOCTL_CKC_GET_PERIBUS       _IO(MAJOR_NUM, 3)
#define IOCTL_CKC_SET_PERISWRESET   _IO(MAJOR_NUM, 4)
#define IOCTL_CKC_SET_FBUSSWRESET   _IO(MAJOR_NUM, 5)
#define IOCTL_CKC_SET_CPU           _IO(MAJOR_NUM, 6) 
#define IOCTL_CKC_GET_CPU           _IO(MAJOR_NUM, 7) 
#define IOCTL_CKC_SET_SMUI2C        _IO(MAJOR_NUM, 8) 
#define IOCTL_CKC_GET_BUS           _IO(MAJOR_NUM, 9) 
#define IOCTL_CKC_GET_VALIDPLLINFO  _IO(MAJOR_NUM, 10)
#define IOCTL_CKC_SET_FBUS          _IO(MAJOR_NUM, 11)
#define IOCTL_CKC_GET_FBUS          _IO(MAJOR_NUM, 12)
#define IOCTL_CKC_SET_PMUPOWER      _IO(MAJOR_NUM, 13)
#define IOCTL_CKC_GET_PMUPOWER      _IO(MAJOR_NUM, 14)
#define IOCTL_CKC_GET_CLOCKINFO     _IO(MAJOR_NUM, 15)
#define IOCTL_CKC_SET_CHANGEFBUS    _IO(MAJOR_NUM, 16)
#define IOCTL_CKC_SET_CHANGEMEM     _IO(MAJOR_NUM, 17)
#define IOCTL_CKC_SET_CHANGECPU     _IO(MAJOR_NUM, 18)

#if  defined(CONFIG_MACH_TCC8920)  || defined(CONFIG_MACH_TCC8800) || \
     defined(CONFIG_MACH_TCC8800ST)|| defined(CONFIG_MACH_TCC8900) || \
     defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9201) || \
     defined(CONFIG_MACH_TCC8923)  || defined(CONFIG_MACH_TCC8924) || \
     defined(CONFIG_MACH_TCC8920ST) || defined(CONFIG_MACH_M805_892X)
struct ckc_ioctl{
	stckcioctl  in_ckc;
	stckcinfo   out_ckc;
};

//AlenOh
struct storage_direct{
	void *buf;
	size_t count;
	loff_t pos;
	ssize_t actual;
	unsigned int user_space;
};
 
#define IOCTL_STORAGE_DIRECT_READ   _IO(MAJOR_NUM, 100)
#define IOCTL_STORAGE_DIRECT_WRITE  _IO(MAJOR_NUM, 101)
#define IOCTL_STORAGE_PING          _IO(MAJOR_NUM, 102)

#endif
#endif	/* __TCC_IOCTL_H__ */
