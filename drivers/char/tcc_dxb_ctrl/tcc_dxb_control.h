/* 
 * linux/drivers/char/tcc_dxb/ctrl/tcc_dxb_control.h
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2008 
 * Description: Telechips Linux BACK-LIGHT DRIVER
 *
 * Copyright (c) Telechips, Inc.
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
 */

#ifndef     _TCC_DXB_CONTROL_H_
#define     _TCC_DXB_CONTROL_H_
#include	<mach/tcc_dxb_ctrl.h>

#define DXB_CTRL_DEV_FILE		"/dev/tcc_dxb_ctrl"
#define DXB_CTRL_DEV_NAME		"tcc_dxb_ctrl"
#define DXB_CTRL_DEV_MAJOR		251
#define DXB_CTRL_DEV_MINOR		0

#define IOCTL_DXB_CTRL_OFF		    _IO(DXB_CTRL_DEV_MAJOR, 1)
#define IOCTL_DXB_CTRL_ON			_IO(DXB_CTRL_DEV_MAJOR, 2)
#define IOCTL_DXB_CTRL_RESET    	_IO(DXB_CTRL_DEV_MAJOR, 3)
#define IOCTL_DXB_CTRL_SET_BOARD    _IO(DXB_CTRL_DEV_MAJOR, 4)
#define IOCTL_DXB_CTRL_GET_CTLINFO  _IO(DXB_CTRL_DEV_MAJOR, 5)
#define IOCTL_DXB_CTRL_RF_PATH      _IO(DXB_CTRL_DEV_MAJOR, 6)

#endif
