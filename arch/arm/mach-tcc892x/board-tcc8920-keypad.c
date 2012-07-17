/* arch/arm/mach-tcc892x/board-tcc8920-keypad.c
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

#include "board-tcc8920.h"

/*******************************************************************
		TCC GPIO KEY
*******************************************************************/
static const struct gpio_event_direct_entry tcc8920_gpio_keymap[] = {
	{ GPIO_PWR_KEY,	KEY_END },
};

static struct gpio_event_input_info tcc8920_gpio_key_input_info = {
	.info.func = gpio_event_input_func,
	.keymap = tcc8920_gpio_keymap,
	.keymap_size = ARRAY_SIZE(tcc8920_gpio_keymap),
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
	//.flags = 0 /*GPIOEDF_PRINT_KEYS*/,
	.type = EV_KEY,
};

static struct gpio_event_info *tcc8920_gpio_key_info[] = {
	&tcc8920_gpio_key_input_info.info,
};

static struct gpio_event_platform_data tcc8920_gpio_key_data = {
	.name = "tcc-gpiokey",
	.info = tcc8920_gpio_key_info,
	.info_count = ARRAY_SIZE(tcc8920_gpio_key_info),
};

static struct platform_device tcc8920_gpio_key_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &tcc8920_gpio_key_data,
	},
};

/*******************************************************************
		TCC METRIX KEY
*******************************************************************/
#if	1	// For Camera uses, you have to use below key map. 
static unsigned int tcc8920_col_gpios[] = { TCC_GPF(3), TCC_GPF(4), TCC_GPF(5) };
static unsigned int tcc8920_row_gpios[] = { TCC_GPF(6), TCC_GPF(7), TCC_GPG(16) };
#else
static unsigned int tcc8920_col_gpios[] = { TCC_GPF(30), TCC_GPF(28), TCC_GPF(16) }; 
static unsigned int tcc8920_row_gpios[] = { TCC_GPF(15), TCC_GPB(7), TCC_GPG(16) };
#endif

//Bruce, for system_rev 0x1005
static unsigned int tcc8920_2cs_col_gpios[] = { TCC_GPB(24), TCC_GPB(25), TCC_GPB(26) };
static unsigned int tcc8920_2cs_row_gpios[] = { TCC_GPB(27), TCC_GPB(29), TCC_GPE(30) };

//Bruce, for system_rev 0x1006
static unsigned int tcc8925_2cs_col_gpios[] = { TCC_GPE(19), TCC_GPE(20), TCC_GPE(21) };
static unsigned int tcc8925_2cs_row_gpios[] = { TCC_GPE(22), TCC_GPE(23), TCC_GPE(24) };

//Bruce, for system_rev 0x1008
static unsigned int tcc8923_col_gpios[] = { TCC_GPE(20), TCC_GPE(21), TCC_GPE(22) };
static unsigned int tcc8923_row_gpios[] = { TCC_GPE(23), TCC_GPE(24), TCC_GPE(30) };

#define KEYMAP_INDEX(col, row) ((col)*ARRAY_SIZE(tcc8920_row_gpios) + (row))

static const unsigned short tcc8920_keymap[] = {
	[KEYMAP_INDEX(0, 0)] = KEY_MENU,
	[KEYMAP_INDEX(0, 1)] = KEY_RESERVED,
	[KEYMAP_INDEX(0, 2)] = KEY_POWER,	// KEY_END

	[KEYMAP_INDEX(1, 0)] = KEY_VOLUMEUP,
	[KEYMAP_INDEX(1, 1)] = KEY_VOLUMEDOWN,
	[KEYMAP_INDEX(1, 2)] = KEY_RESERVED,
	
	[KEYMAP_INDEX(2, 0)] = KEY_HOMEPAGE,	// KEY_HOME
	[KEYMAP_INDEX(2, 1)] = KEY_BACK,
	[KEYMAP_INDEX(2, 2)] = KEY_RESERVED,
};

static struct gpio_event_matrix_info tcc8921_keymap_info = {
	.info.func = gpio_event_matrix_func,
	.keymap = tcc8920_keymap,
	.output_gpios = tcc8920_col_gpios,
	.input_gpios = tcc8920_row_gpios,
	.noutputs = ARRAY_SIZE(tcc8920_col_gpios),
	.ninputs = ARRAY_SIZE(tcc8920_row_gpios),
	.settle_time.tv.nsec = 40 * NSEC_PER_USEC,
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.debounce_delay.tv.nsec = 50 * NSEC_PER_MSEC,
#if 1 // XXX: disable interrupt for now
	.flags = GPIOKPF_DRIVE_INACTIVE /*| GPIOKPF_PRINT_UNMAPPED_KEYS | GPIOKPF_PRINT_MAPPED_KEYS*/,
#else
	.flags = GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_PRINT_UNMAPPED_KEYS,
#endif
};

static struct gpio_event_info *tcc8920_matrix_key_info[] = {
	&tcc8921_keymap_info.info,
};

static struct gpio_event_platform_data tcc8920_matrix_key_data = {
	.name = "tcc-gpiokey",
	.info = tcc8920_matrix_key_info,
	.info_count = ARRAY_SIZE(tcc8920_matrix_key_info),
};

static struct platform_device tcc8920_matrix_key_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &tcc8920_matrix_key_data,
	},
};

#if defined(CONFIG_KEYPAD_TCC_ADC)
static struct resource tcc8920_keypad_resources[] = {
	[0] = {
		.start	= io_p2v(HwTSADC_BASE),		
		.end	= io_p2v(HwTSADC_BASE + 0x30),	
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device tcc8920_keypad_device = {
	.name			= "tcc-keypad",
	.id 			= -1,
	.resource		= tcc8920_keypad_resources,
	.num_resources		= ARRAY_SIZE(tcc8920_keypad_resources),
};
#endif

static int __init tcc8920_keypad_init(void)
{
	if (!machine_is_tcc8920())
		return 0;

	if(system_rev == 0x1005 || system_rev == 0x1007)
	{
		tcc8921_keymap_info.input_gpios = tcc8920_2cs_row_gpios;
		tcc8921_keymap_info.output_gpios = tcc8920_2cs_col_gpios;
	}
	else if(system_rev == 0x1006)
	{
		tcc8921_keymap_info.input_gpios = tcc8925_2cs_row_gpios;
		tcc8921_keymap_info.output_gpios = tcc8925_2cs_col_gpios;
	}
	else if(system_rev == 0x1008)
	{
		tcc8921_keymap_info.input_gpios = tcc8923_row_gpios;
		tcc8921_keymap_info.output_gpios = tcc8923_col_gpios;
	}

	platform_device_register(&tcc8920_matrix_key_device);
	//platform_device_register(&tcc8920_gpio_key_device);

	#ifdef CONFIG_KEYPAD_TCC_ADC
	platform_device_register(&tcc8920_keypad_device);
	#endif

	return 0;
}

device_initcall(tcc8920_keypad_init);
