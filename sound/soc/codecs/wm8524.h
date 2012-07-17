/*
 * wm8524.h  --  WM8524 Soc Audio driver
 *
 * Copyright 2005 Openedhand Ltd.
 *
 * Author: Richard Purdie <richard@openedhand.com>
 *
 * Based on wm8731.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _WM8524_H
#define _WM8524_H

/* WM8524 register space */

#define WM8524_SYSCLK	0
#define WM8524_DAI		0

struct wm8524_setup_data {
	int            spi;
	int            i2c_bus;
	unsigned short i2c_address;
};

#endif
