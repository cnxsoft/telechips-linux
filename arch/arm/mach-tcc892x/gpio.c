/*
 * linux/arch/arm/mach-tcc892x/gpio.c
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
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sysdev.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <mach/irqs.h>
#include <mach/gpio.h>
#include <asm/mach/irq.h>

#include <mach/reg_physical.h>

#define RMWREG32(addr, startbit, width, val) writel((readl(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit)), addr)

#define GPIO_REG(reg)	IO_ADDRESS(reg)

typedef struct gpioregs gpioregs;

struct gpioregs
{
	unsigned data;         /* data */
	unsigned out_en;       /* output enable */
	unsigned out_or;       /* OR fnction on output data */
	unsigned out_bic;      /* BIC function on output data */
	unsigned out_xor;      /* XOR function on output data */
	unsigned strength0;    /* driver strength control 0 */
	unsigned strength1;    /* driver strength control 1 */
	unsigned pull_enable;  /* pull-up/down enable */
	unsigned pull_select;  /* pull-up/down select */
	unsigned in_en;        /* input enable */
	unsigned in_type;      /* input type (Shmitt / CMOS) */
	unsigned slew_rate;    /* slew rate */
	unsigned func_select0; /* port configuration 0 */
	unsigned func_select1; /* port configuration 1 */
	unsigned func_select2; /* port configuration 2 */
	unsigned func_select3; /* port configuration 3 */
};

struct board_gpio_irq_config *board_gpio_irqs;

int gpio_to_irq(unsigned gpio)
{
	struct board_gpio_irq_config *gpio_irq = board_gpio_irqs;

	if (gpio_irq) {
		while (gpio_irq->irq != -1) {
			if (gpio_irq->gpio == gpio)
				return gpio_irq->irq;
			++gpio_irq;
		}
	}
	return -EINVAL;
}

int tcc_gpio_config_ext_intr(unsigned intr, unsigned source) /* intr: irq num, source: external interrupt source */
{
	int extint;
    GPIO *reg = (GPIO *) GPIO_REG(HwGPIO_BASE);

	switch (intr) {
		case INT_EI0:  extint = EXINT_EI0;  break;
		case INT_EI1:  extint = EXINT_EI1;  break;
		case INT_EI2:  extint = EXINT_EI2;  break;
		case INT_EI3:  extint = EXINT_EI3;  break;
		case INT_EI4:  extint = EXINT_EI4;  break;
		case INT_EI5:  extint = EXINT_EI5;  break;
		case INT_EI6:  extint = EXINT_EI6;  break;
		case INT_EI7:  extint = EXINT_EI7;  break;
		case INT_EI8:  extint = EXINT_EI8;  break;
		case INT_EI9:  extint = EXINT_EI9;  break;
		case INT_EI10: extint = EXINT_EI10; break;
		case INT_EI11: extint = EXINT_EI11; break;
		default:
			extint = -1;
			break;
	}

	if (extint < 0)
		return -1;

    if(extint < 4) {
        RMWREG32(&reg->EINTSEL0.nREG, extint*8, 7, source);
    }
    else if(extint < 8) {
        RMWREG32(&reg->EINTSEL1.nREG, (extint-4)*8, 7, source);
    }
    else if(extint < 12) {
        RMWREG32(&reg->EINTSEL2.nREG, (extint-8)*8, 7, source);
    }

	return 0;
}

static inline gpioregs *tcc892x_gpio_to_reg(unsigned gpio_port)
{
	switch(gpio_port)
	{
		case GPIO_PORTA:
			return (gpioregs *) GPIO_REG(HwGPIOA_BASE);
		case GPIO_PORTB:
			return (gpioregs *) GPIO_REG(HwGPIOB_BASE);
		case GPIO_PORTC:
			return (gpioregs *) GPIO_REG(HwGPIOC_BASE);
		case GPIO_PORTD:
			return (gpioregs *) GPIO_REG(HwGPIOD_BASE);
		case GPIO_PORTE:
			return (gpioregs *) GPIO_REG(HwGPIOE_BASE);
		case GPIO_PORTF:
			return (gpioregs *) GPIO_REG(HwGPIOF_BASE);
		case GPIO_PORTG:
			return (gpioregs *) GPIO_REG(HwGPIOG_BASE);
		case GPIO_PORTHDMI:
			return (gpioregs *) GPIO_REG(HwGPIOHDMI_BASE);
		case GPIO_PORTADC:
			return (gpioregs *) GPIO_REG(HwGPIOADC_BASE);
		default:
			break;
	}
	return NULL;
}

int tcc_gpio_config(unsigned gpio, unsigned flags)
{
	gpioregs *r = tcc892x_gpio_to_reg((gpio&GPIO_REGMASK));
	unsigned num = gpio & GPIO_BITMASK;;
	unsigned bit = (1<<num);

	if(r == NULL)
		return -EINVAL;

	if (flags & GPIO_FN_BITMASK) {
		unsigned fn = ((flags & GPIO_FN_BITMASK) >> GPIO_FN_SHIFT) - 1;

		if (num < 8)
			RMWREG32(&r->func_select0, num*4, 4, fn);
		else if (num < 16)
			RMWREG32(&r->func_select1, (num-8)*4, 4, fn);
		else if (num < 24)
			RMWREG32(&r->func_select2, (num-16)*4, 4, fn);
		else
			RMWREG32(&r->func_select3, (num-24)*4, 4, fn);
	}

	if (flags & GPIO_CD_BITMASK) {
		unsigned cd = ((flags & GPIO_CD_BITMASK) >> GPIO_CD_SHIFT) - 1;

		if (num < 16)
			RMWREG32(&r->strength0, num*2, 2, cd);
		else
			RMWREG32(&r->strength1, (num-16)*2, 2, cd);
	}

	if (flags & GPIO_OUTPUT) {
		if (flags & GPIO_HIGH)
			writel(readl(&r->data) | bit, &r->data);
		if (flags & GPIO_LOW)
			writel(readl(&r->data) & (~bit),& r->data);
		writel(readl(&r->out_en) | bit, &r->out_en);
	} else if (flags & GPIO_INPUT) {
		writel(readl(&r->out_en) & (~bit), &r->out_en);
	}

	if (flags & GPIO_PULLUP){
		writel(readl(&r->pull_select) | bit, &r->pull_select);
		writel(readl(&r->pull_enable) | bit, &r->pull_enable);
	} else if (flags & GPIO_PULLDOWN) {
		writel(readl(&r->pull_select) & (~bit), &r->pull_select);
		writel(readl(&r->pull_enable) | bit, &r->pull_enable);
	} else if (flags & GPIO_PULL_DISABLE) {
		writel(readl(&r->pull_enable) & (~bit), &r->pull_enable);	
  	} else if (flags & GPIO_SCHMITT_INPUT) {
		writel(readl(&r->in_type) | bit, &r->in_type);
	} else if (flags & GPIO_CMOS_INPUT) {
		writel(readl(&r->in_type) & (~bit), &r->in_type);
	}

	return 0;
}

static int tcc892x_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	gpioregs *reg = tcc892x_gpio_to_reg(chip->base);

	if(reg) {
		return (readl(&reg->data) >> offset) & 1;
	}
	return 0;
}

static void tcc892x_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	gpioregs *reg = tcc892x_gpio_to_reg(chip->base);

	if(reg)	{
		if (value)
			writel(readl(&reg->data) | (1 << offset), &reg->data);
		else
			writel(readl(&reg->data) & ~(1 << offset), &reg->data);
	}
}

static int tcc892x_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	gpioregs *reg = tcc892x_gpio_to_reg(chip->base);

	if(reg)	{
		writel(readl(&reg->out_en) & ~(1 << offset), &reg->out_en);
	}
	return 0;
}

static int tcc892x_gpio_direction_output(struct gpio_chip *chip, unsigned offset, int value)
{
	gpioregs *reg = tcc892x_gpio_to_reg(chip->base);

	if(reg)	{
		if (value)
			writel(readl(&reg->data) | (1 << offset), &reg->data);
		else
			writel(readl(&reg->data) & ~(1 << offset), &reg->data);
		writel(readl(&reg->out_en) | (1 << offset), &reg->out_en);
	}
	return 0;
}

static struct gpio_chip tcc892x_gpio_chips[] = {
	{
		.label = "gpio_a",
		.base = GPIO_PORTA,
		.ngpio = 17,
	},
	{
		.label = "gpio_b",
		.base = GPIO_PORTB,
		.ngpio = 30,
	},
	{
		.label = "gpio_c",
		.base = GPIO_PORTC,
		.ngpio = 30,
	},
	{
		.label = "gpio_d",
		.base = GPIO_PORTD,
		.ngpio = 22,
	},
	{
		.label = "gpio_e",
		.base = GPIO_PORTE,
		.ngpio = 32,
	},
	{
		.label = "gpio_f",
		.base = GPIO_PORTF,
		.ngpio = 31,
	},
	{
		.label = "gpio_g",
		.base = GPIO_PORTG,
		.ngpio = 20,
	},
	{
		.label = "gpio_hdmi",
		.base = GPIO_PORTHDMI,
		.ngpio = 4,
	},
	{
		.label = "gpio_adc",
		.base = GPIO_PORTADC,
		.ngpio = 6,
	},
};

int __init tcc892x_gpio_init(void)
{
	struct gpio_chip *chip;
	int i;

	for (i = 0; i < ARRAY_SIZE(tcc892x_gpio_chips); i++) {
		chip = &tcc892x_gpio_chips[i];

		chip->direction_input = tcc892x_gpio_direction_input;
		chip->direction_output = tcc892x_gpio_direction_output;
		chip->set = tcc892x_gpio_set;
		chip->get = tcc892x_gpio_get;

		gpiochip_add(chip);
	}
	return 0;
}

EXPORT_SYMBOL(gpio_to_irq);
EXPORT_SYMBOL(tcc_gpio_config_ext_intr);
EXPORT_SYMBOL(tcc_gpio_config);

postcore_initcall(tcc892x_gpio_init);
