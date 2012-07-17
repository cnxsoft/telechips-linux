/* arch/arm/mach-tcc92xx/board-tcc8800-keypad.c
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
#include "board-tcc8800.h"

/*******************************************************************
		TCC GPIO KEY
*******************************************************************/
static const struct gpio_event_direct_entry tcc88xx_gpio_keymap[] = {
	{ GPIO_PWR_KEY,	KEY_POWER/*KEY_END*/ },
};

static struct gpio_event_input_info tcc8800_gpio_key_input_info = {
	.info.func = gpio_event_input_func,
	.keymap = tcc88xx_gpio_keymap,
	.keymap_size = ARRAY_SIZE(tcc88xx_gpio_keymap),
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
	//.flags = 0 /*GPIOEDF_PRINT_KEYS*/,
	.flags = GPIOEDF_ACTIVE_HIGH ,
	.type = EV_KEY,
};

static struct gpio_event_info *tcc8800_gpio_key_info[] = {
	&tcc8800_gpio_key_input_info.info,
};

static struct gpio_event_platform_data tcc8800_gpio_key_data = {
	.name = "tcc-gpiokey",
	.info = tcc8800_gpio_key_info,
	.info_count = ARRAY_SIZE(tcc8800_gpio_key_info),
};

static struct platform_device tcc8800_gpio_key_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &tcc8800_gpio_key_data,
	},
};

static struct gpio_event_input_info tcc8801_gpio_key_input_info = {
	.info.func = gpio_event_input_func,
	.keymap = tcc88xx_gpio_keymap,
	.keymap_size = ARRAY_SIZE(tcc88xx_gpio_keymap),
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
	//.flags = 0 /*GPIOEDF_PRINT_KEYS*/,
	.type = EV_KEY,
};

static struct gpio_event_info *tcc8801_gpio_key_info[] = {
	&tcc8801_gpio_key_input_info.info,
};

static struct gpio_event_platform_data tcc8801_gpio_key_data = {
	.name = "tcc-gpiokey",
	.info = tcc8801_gpio_key_info,
	.info_count = ARRAY_SIZE(tcc8801_gpio_key_info),
};

static struct platform_device tcc8801_gpio_key_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &tcc8801_gpio_key_data,
	},
};

static const struct gpio_event_direct_entry tcc8803_2cs_gpio_keymap[] = {
	{ GPIO_PWR_KEY, KEY_POWER/*KEY_END*/ },
	{ TCC_GPF(2), KEY_MENU },
	{ TCC_GPF(3), KEY_BACK },
	{ TCC_GPF(4), KEY_UP },
	{ TCC_GPF(5), KEY_DOWN },
	{ TCC_GPF(6), KEY_HOMEPAGE },
};

static struct gpio_event_input_info tcc8803_2cs_gpio_key_input_info = {
	.info.func = gpio_event_input_func,
	.keymap = tcc8803_2cs_gpio_keymap,
	.keymap_size = ARRAY_SIZE(tcc8803_2cs_gpio_keymap),
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
	//.flags = 0 /*GPIOEDF_PRINT_KEYS*/,
	.type = EV_KEY,
};

static struct gpio_event_info *tcc8803_2cs_gpio_key_info[] = {
	&tcc8803_2cs_gpio_key_input_info.info,
};

static struct gpio_event_platform_data tcc8803_2cs_gpio_key_data = {
	.name = "tcc-gpiokey",
	.info = tcc8803_2cs_gpio_key_info,
	.info_count = ARRAY_SIZE(tcc8803_2cs_gpio_key_info),
};

static struct platform_device tcc8803_2cs_gpio_key_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &tcc8803_2cs_gpio_key_data,
	},
};
/*******************************************************************
		TCC METRIX KEY
*******************************************************************/
static unsigned int tcc8801_col_gpios[] = { GPIO_PORTG|23, GPIO_PORTG|24, GPIO_PORTG|25 };
static unsigned int tcc8801_row_gpios[] = { GPIO_PORTG|26, GPIO_PORTG|27, GPIO_PORTA|3  };

#define KEYMAP_INDEX(col, row) ((col)*ARRAY_SIZE(tcc8801_row_gpios) + (row))

static const unsigned short tcc8801_keymap[] = {
	[KEYMAP_INDEX(0, 0)] = KEY_MENU,
	[KEYMAP_INDEX(0, 1)] = KEY_RESERVED,
	[KEYMAP_INDEX(0, 2)] = KEY_POWER,	//KEY_END,

	[KEYMAP_INDEX(1, 0)] = KEY_VOLUMEUP,
	[KEYMAP_INDEX(1, 1)] = KEY_VOLUMEDOWN,
	[KEYMAP_INDEX(1, 2)] = KEY_RESERVED,
	
	[KEYMAP_INDEX(2, 0)] = KEY_HOMEPAGE,
	[KEYMAP_INDEX(2, 1)] = KEY_BACK,
	[KEYMAP_INDEX(2, 2)] = KEY_RESERVED,
};

static struct gpio_event_matrix_info tcc8801_keymap_info = {
	.info.func = gpio_event_matrix_func,
	.keymap = tcc8801_keymap,
	.output_gpios = tcc8801_col_gpios,
	.input_gpios = tcc8801_row_gpios,
	.noutputs = ARRAY_SIZE(tcc8801_col_gpios),
	.ninputs = ARRAY_SIZE(tcc8801_row_gpios),
	.settle_time.tv.nsec = 40 * NSEC_PER_USEC,
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_delay.tv.nsec = 50 * NSEC_PER_MSEC,
#if 1 // XXX: disable interrupt for now
	.flags = GPIOKPF_DRIVE_INACTIVE /*| GPIOKPF_PRINT_UNMAPPED_KEYS | GPIOKPF_PRINT_MAPPED_KEYS*/,
#else
	.flags = GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_PRINT_UNMAPPED_KEYS,
#endif
};

static struct gpio_event_info *tcc8801_keypad_matrix_info[] = {
	&tcc8801_keymap_info.info,
};

static struct gpio_event_platform_data tcc8801_keypad_matrix_data = {
	.name = "tcc-gpiokey",
	.info = tcc8801_keypad_matrix_info,
	.info_count = ARRAY_SIZE(tcc8801_keypad_matrix_info),
};

static struct platform_device tcc8801_keypad_matrix_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &tcc8801_keypad_matrix_data,
	},
};

static unsigned int tcc8803_2cs_col_gpios[] = { GPIO_PORTG|10, GPIO_PORTG|11, GPIO_PORTG|12 };
static unsigned int tcc8803_2cs_row_gpios[] = { GPIO_PORTG|13, GPIO_PORTG|14, GPIO_PORTA|3  };

#define KEYMAP_INDEX_TCC8803_2CS(col, row) ((col)*ARRAY_SIZE(tcc8803_2cs_row_gpios) + (row))

static const unsigned short tcc8803_2cs_keymap[] = {
	[KEYMAP_INDEX_TCC8803_2CS(0, 0)] = KEY_MENU,
	[KEYMAP_INDEX_TCC8803_2CS(0, 1)] = KEY_RESERVED,
	[KEYMAP_INDEX_TCC8803_2CS(0, 2)] = KEY_POWER/*KEY_END*/,

	[KEYMAP_INDEX_TCC8803_2CS(1, 0)] = KEY_VOLUMEUP,
	[KEYMAP_INDEX_TCC8803_2CS(1, 1)] = KEY_VOLUMEDOWN,
	[KEYMAP_INDEX_TCC8803_2CS(1, 2)] = KEY_RESERVED,

	[KEYMAP_INDEX_TCC8803_2CS(2, 0)] = KEY_HOMEPAGE,
	[KEYMAP_INDEX_TCC8803_2CS(2, 1)] = KEY_BACK,
	[KEYMAP_INDEX_TCC8803_2CS(2, 2)] = KEY_RESERVED,
};

static struct gpio_event_matrix_info tcc8803_2cs_keymap_info = {
	.info.func = gpio_event_matrix_func,
	.keymap = tcc8803_2cs_keymap,
	.output_gpios = tcc8803_2cs_col_gpios,
	.input_gpios = tcc8803_2cs_row_gpios,
	.noutputs = ARRAY_SIZE(tcc8803_2cs_col_gpios),
	.ninputs = ARRAY_SIZE(tcc8803_2cs_row_gpios),
	.settle_time.tv.nsec = 40 * NSEC_PER_USEC,
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_delay.tv.nsec = 50 * NSEC_PER_MSEC,
#if 1 // XXX: disable interrupt for now
	.flags = GPIOKPF_DRIVE_INACTIVE /*| GPIOKPF_PRINT_UNMAPPED_KEYS | GPIOKPF_PRINT_MAPPED_KEYS*/,
#else
	.flags = GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_PRINT_UNMAPPED_KEYS,
#endif
};

static struct gpio_event_info *tcc8803_2cs_keypad_matrix_info[] = {
	&tcc8803_2cs_keymap_info.info,
};

static struct gpio_event_platform_data tcc8803_2cs_keypad_matrix_data = {
	.name = "tcc-gpiokey",
	.info = tcc8803_2cs_keypad_matrix_info,
	.info_count = ARRAY_SIZE(tcc8803_2cs_keypad_matrix_info),
};

static struct platform_device tcc8803_2cs_keypad_matrix_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &tcc8803_2cs_keypad_matrix_data,
	},
};

#if defined(CONFIG_KEYPAD_TCC_ADC)
static struct resource tcc8800_keypad_resources[] = {
	[0] = {
		.start	= 0xF0102000,		
		.end	= 0xF0102030,	
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device tcc8800_keypad_device = {
	.name			= "tcc-keypad",
	.id 			= -1,
	.resource		= tcc8800_keypad_resources,
	.num_resources		= ARRAY_SIZE(tcc8800_keypad_resources),
};
#endif

static int __init tcc8800_keypad_init(void)
{
	if (!machine_is_tcc8800())
		return 0;

	if (system_rev == 0x0610 || system_rev == 0x0613 || system_rev == 0x0615 || system_rev == 0x0620 || system_rev == 0x0621 || system_rev == 0x0623) /* matrix key use */
		platform_device_register(&tcc8801_keypad_matrix_device);
	else if (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624) /* matrix key use */
		platform_device_register(&tcc8803_2cs_keypad_matrix_device);
	else if (system_rev == 0x0602 || system_rev == 0x0612) {
		platform_device_register(&tcc8803_2cs_gpio_key_device);
	}
	else {
		if (system_rev >= 0x0600 && system_rev != 0x0601)
			platform_device_register(&tcc8801_gpio_key_device);
		else
			platform_device_register(&tcc8800_gpio_key_device);

		#ifdef CONFIG_KEYPAD_TCC_ADC
		platform_device_register(&tcc8800_keypad_device);
		#endif
	}

	return 0;
}

device_initcall(tcc8800_keypad_init);
