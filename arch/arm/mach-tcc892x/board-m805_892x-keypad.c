/* arch/arm/mach-tcc892x/board-m805_892x-keypad.c
 *
 * Copyright (C) 2011 Telechips, Inc.
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

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_event.h>
#include <asm/gpio.h>
#include <asm/mach-types.h>
#include <mach/bsp.h>
#include <mach/io.h>

#include "board-m805_892x.h"

/*******************************************************************
		TCC GPIO KEY
*******************************************************************/
static const struct gpio_event_direct_entry m805_892x_gpio_keymap[] = {
#if !defined(CONFIG_REGULATOR_AXP192_PEK)
	{ TCC_GPE(15),	KEY_POWER },
#endif
#if (CONFIG_M805S_8925_0XX)
	{ TCC_GPE(21),	KEY_VOLUMEUP },   // volume +
	{ TCC_GPE(22),	KEY_VOLUMEDOWN }, // volume -
	{ TCC_GPE(23),	KEY_HOMEPAGE },   // home
	{ TCC_GPE(19),	KEY_MENU },       // menu		// not used system_rev 0x2003 0x2004 0x2005
	{ TCC_GPE(20),	KEY_BACK },       // back		// not used system_rev 0x2003 0x2004 0x2005
#else
	{ TCC_GPE(12),	KEY_VOLUMEUP },   // volume +
	{ TCC_GPE(26),	KEY_VOLUMEDOWN }, // volume -
	{ TCC_GPE(27),	KEY_HOMEPAGE },   // home
	{ TCC_GPD(12),	KEY_MENU },       // menu
	{ TCC_GPD(13),	KEY_BACK },       // back
#endif
};

static struct gpio_event_input_info m805_892x_gpio_key_input_info = {
	.info.func = gpio_event_input_func,
	.keymap = m805_892x_gpio_keymap,
	.keymap_size = ARRAY_SIZE(m805_892x_gpio_keymap),
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
	//.flags = 0 /*GPIOEDF_PRINT_KEYS*/,
	.type = EV_KEY,
};

static struct gpio_event_info *m805_892x_gpio_key_info[] = {
	&m805_892x_gpio_key_input_info.info,
};

static struct gpio_event_platform_data m805_892x_gpio_key_data = {
	.name = "tcc-gpiokey",
	.info = m805_892x_gpio_key_info,
	.info_count = ARRAY_SIZE(m805_892x_gpio_key_info),
};

static struct platform_device m805_892x_gpio_key_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &m805_892x_gpio_key_data,
	},
};

static int __init m805_892x_keypad_init(void)
{
	if (!machine_is_m805_892x())
		return 0;

	if (system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
		m805_892x_gpio_key_input_info.keymap_size = ARRAY_SIZE(m805_892x_gpio_keymap) - 2;
	}

	platform_device_register(&m805_892x_gpio_key_device);

	return 0;
}

device_initcall(m805_892x_keypad_init);
