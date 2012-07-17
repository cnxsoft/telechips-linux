/* arch/arm/mach-tcc92xx/board-m801_88-keypad.c
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

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_event.h>
#include <asm/gpio.h>
#include <asm/mach-types.h>
#include "board-m801_88.h"

static const struct gpio_event_direct_entry m801_88_gpio_keymap[] = {
#if !defined(CONFIG_REGULATOR_AXP192_PEK)
	{ GPIO_PWR_KEY,	KEY_END },
#endif
	{ TCC_GPF(2),	KEY_MENU },	// menu
	{ TCC_GPF(3),	KEY_BACK },	// back
	{ TCC_GPF(4), 	KEY_VOLUMEUP }, // home
	{ TCC_GPF(5), 	KEY_VOLUMEDOWN }, // home
	{ TCC_GPF(6), 	KEY_HOMEPAGE }, // home
};

static struct gpio_event_input_info m801_88_gpio_key_input_info = {
	.info.func = gpio_event_input_func,
	.keymap = m801_88_gpio_keymap,
	.keymap_size = ARRAY_SIZE(m801_88_gpio_keymap),
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.flags = 0 /*GPIOEDF_PRINT_KEYS*/,
	.type = EV_KEY,
};

static struct gpio_event_info *m801_88_gpio_key_info[] = {
	&m801_88_gpio_key_input_info.info,
};

static struct gpio_event_platform_data m801_88_gpio_key_data = {
	.name = "tcc-gpiokey",
	.info = m801_88_gpio_key_info,
	.info_count = ARRAY_SIZE(m801_88_gpio_key_info),
};

static struct platform_device m801_88_gpio_key_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &m801_88_gpio_key_data,
	},
};

static int __init m801_88_keypad_init(void)
{
	if (!machine_is_m801_88())
		return 0;

	platform_device_register(&m801_88_gpio_key_device);


	return 0;
}

device_initcall(m801_88_keypad_init);
