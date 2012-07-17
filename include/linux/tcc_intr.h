/*
 * include/linux/tcc_intr.c 
 *
 * Author:  <linux@telechips.com>
 * Created: 31th March, 2009 
 * Description: User-level interrupt Driver 
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *-----------------------------------------------
 * 1. TCC_INTR_DEV_VIDEO_CODEC
 *    - dev node: tcc-intr-vc
 *    - Video Codec interrupt handling
 * 2. TCC_INTR_DEV_M2M_SCALER
 *    - dev node: tcc-intr-sc
 *    - Mem-to-Mem scaler 0 interrupt handling
 *
 */
#ifndef __TCC_INTR_H__
#define __TCC_INTR_H__

#define TCC_INTR_DEV_VIDEO_CODEC	"tcc-intr-vc"
#define TCC_INTR_DEV_M2M_SCALER		"tcc-intr-sc"
#define TCC_INTR_DEV_MAJOR			246

#define IOCTL_INTR_SET		_IO(TCC_INTR_DEV_MAJOR, 1)
#define IOCTL_INTR_GET		_IO(TCC_INTR_DEV_MAJOR, 2)
#define IOCTL_INTR_TEST		_IO(TCC_INTR_DEV_MAJOR, 3)

#endif	/*  __TCC_INTR_H__ */
