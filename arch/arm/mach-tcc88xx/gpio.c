/*
 * linux/arch/arm/mach-tcc88xx/gpio.c
 *
 * Copyright (C) 2009, 2010 Telechips, Inc.
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

#include <mach/bsp.h>
#include <mach/globals.h>
//#include <mach/reg_physical.h>

#define RMWREG32(addr, startbit, width, val) writel((readl(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit)), addr)

#define GPIO_REG(reg)	IO_ADDRESS(reg)

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
EXPORT_SYMBOL(gpio_to_irq);

int tcc_gpio_config_ext_intr(unsigned intr, unsigned source)	/* intr: irq num, source: external interrupt source */
{
	int extint;
    GPIO *reg = (GPIO *) GPIO_REG(HwGPIO_BASE);
	PPIC pPIC = (PPIC)GPIO_REG(HwPIC_BASE);

	switch (intr) {
		case INT_EI0:	extint = EXINT_EI0;	break;
		case INT_EI1:	extint = EXINT_EI1;	break;
		case INT_EI2:	extint = EXINT_EI2;	break;
		case INT_TSD:	extint = EXINT_EI3;	break;
		case INT_EI4:	extint = EXINT_EI4;	break;
		case INT_EI5:	extint = EXINT_EI5;	break;
		case INT_EI6:	extint = EXINT_EI6;	break;
		case INT_EI7:	extint = EXINT_EI7;	break;
		default:		extint = -1;			break;
	}

	if (extint < 0)
		return -1;

	/* External Interrupt 3~7 Selection */
	switch (extint) {
		case 3:	BITCSET(pPIC->EI37SEL, 0x00000008, 0x00000008);	break;
		case 4:	BITCSET(pPIC->EI37SEL, 0x00000010, 0x00000010);	break;
		case 5:	BITCSET(pPIC->EI37SEL, 0x00000020, 0x00000020);	break;
		case 6:	BITCSET(pPIC->EI37SEL, 0x00000040, 0x00000040);	break;
		case 7:	BITCSET(pPIC->EI37SEL, 0x00000080, 0x00000080);	break;
	}

    if(extint < 4) {
        RMWREG32(&reg->EINTSEL0, extint*8, 7, source);
    }
    else if(extint < 8) {
        RMWREG32(&reg->EINTSEL1, (extint-4)*8, 7, source);
    }

	return 0;
}
EXPORT_SYMBOL(tcc_gpio_config_ext_intr);


int tcc_gpio_config(unsigned gpio, unsigned flags)
{
	GPION *reg;
	unsigned bit = gpio & 0x1f;

	if (gpio < GPIO_PORTB)
		reg = (GPION *) GPIO_REG(HwGPIOA_BASE);
	else if (gpio < GPIO_PORTC)
		reg = (GPION *) GPIO_REG(HwGPIOB_BASE);
	else if (gpio < GPIO_PORTD)
		reg = (GPION *) GPIO_REG(HwGPIOC_BASE);
	else if (gpio < GPIO_PORTE)
		reg = (GPION *) GPIO_REG(HwGPIOD_BASE);
	else if (gpio < GPIO_PORTF)
		reg = (GPION *) GPIO_REG(HwGPIOE_BASE);
	else if (gpio < GPIO_PORTG)
		reg = (GPION *) GPIO_REG(HwGPIOF_BASE);
	else if (gpio < GPIO_PORTEXT1)
		reg = (GPION *) GPIO_REG(HwGPIOG_BASE);
	else
		return -EINVAL;

	if (flags & GPIO_FN_BITMASK) {
		unsigned fn = ((flags & GPIO_FN_BITMASK) >> GPIO_FN_SHIFT) - 1;

		if (bit < 8)
			RMWREG32(&reg->GPFN0, bit*4, 4, fn);
		else if (bit < 16)
			RMWREG32(&reg->GPFN1, (bit-8)*4, 4, fn);
		else if (bit < 24)
			RMWREG32(&reg->GPFN2, (bit-16)*4, 4, fn);
		else
			RMWREG32(&reg->GPFN3, (bit-24)*4, 4, fn);
	}

	if (flags & GPIO_PULLUP) {
		if (bit < 16)
			RMWREG32(&reg->GPPD0, bit*2, 2, 0x1);
		else
			RMWREG32(&reg->GPPD1, (bit-16)*2, 2, 0x1);
	} else if (flags & GPIO_PULLDOWN) {
		if (bit < 16)
			RMWREG32(&reg->GPPD0, bit*2, 2, 0x2);
		else
			RMWREG32(&reg->GPPD1, (bit-16)*2, 2, 0x2);
	} else if (flags & GPIO_PULL_DISABLE) {
		if (bit < 16)
			RMWREG32(&reg->GPPD0, bit*2, 2, 0);
		else
			RMWREG32(&reg->GPPD1, (bit-16)*2, 2, 0);
    }
    

	if (flags & GPIO_CD_BITMASK) {
		unsigned cd = ((flags & GPIO_CD_BITMASK) >> GPIO_CD_SHIFT) - 1;

		if (bit < 16)
			RMWREG32(&reg->GPCD0, bit*2, 2, cd);
		else
			RMWREG32(&reg->GPCD1, (bit-16)*2, 2, cd);
	}
	return 0;
}
EXPORT_SYMBOL(tcc_gpio_config);

int tcc_gpio_direction_input(unsigned gpio)
{
	GPION *reg;
	unsigned bit = gpio & 0x1f;

	if (gpio < GPIO_PORTB)
		reg = (GPION *) GPIO_REG(HwGPIOA_BASE);
	else if (gpio < GPIO_PORTC)
		reg = (GPION *) GPIO_REG(HwGPIOB_BASE);
	else if (gpio < GPIO_PORTD)
		reg = (GPION *) GPIO_REG(HwGPIOC_BASE);
	else if (gpio < GPIO_PORTE)
		reg = (GPION *) GPIO_REG(HwGPIOD_BASE);
	else if (gpio < GPIO_PORTF)
		reg = (GPION *) GPIO_REG(HwGPIOE_BASE);
	else if (gpio < GPIO_PORTG)
		reg = (GPION *) GPIO_REG(HwGPIOF_BASE);
	else if (gpio < GPIO_PORTEXT1)
		reg = (GPION *) GPIO_REG(HwGPIOG_BASE);
	else
		return -EINVAL;

	writel(readl(&reg->GPEN) & ~(1 << bit), &reg->GPEN);
	return 0;
}
EXPORT_SYMBOL(tcc_gpio_direction_input);


static inline GPION *tcc88xx_gpio_to_reg(struct gpio_chip *chip)
{
	if (chip->base == GPIO_PORTA)
		return (GPION *) GPIO_REG(HwGPIOA_BASE);
	if (chip->base == GPIO_PORTB)
		return (GPION *) GPIO_REG(HwGPIOB_BASE);
	if (chip->base == GPIO_PORTC)
		return (GPION *) GPIO_REG(HwGPIOC_BASE);
	if (chip->base == GPIO_PORTD)
		return (GPION *) GPIO_REG(HwGPIOD_BASE);
	if (chip->base == GPIO_PORTE)
		return (GPION *) GPIO_REG(HwGPIOE_BASE);
	if (chip->base == GPIO_PORTF)
		return (GPION *) GPIO_REG(HwGPIOF_BASE);
	if (chip->base == GPIO_PORTG)
		return (GPION *) GPIO_REG(HwGPIOG_BASE);
	return 0;
}


static int tcc88xx_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	GPION *reg = tcc88xx_gpio_to_reg(chip);

	return (readl(&reg->GPDAT) >> offset) & 1;
}

static void tcc88xx_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	GPION *reg = tcc88xx_gpio_to_reg(chip);

	if (value)
		writel(readl(&reg->GPDAT) | (1 << offset), &reg->GPDAT);
	else
		writel(readl(&reg->GPDAT) & ~(1 << offset), &reg->GPDAT);
}

static int tcc88xx_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	GPION *reg = tcc88xx_gpio_to_reg(chip);

	writel(readl(&reg->GPEN) & ~(1 << offset), &reg->GPEN);
	return 0;
}

static int tcc88xx_gpio_direction_output(struct gpio_chip *chip, unsigned offset, int value)
{
	GPION *reg = tcc88xx_gpio_to_reg(chip);

	tcc88xx_gpio_set(chip, offset, value);
	writel(readl(&reg->GPEN) | (1 << offset), &reg->GPEN);
	return 0;
}

static struct gpio_chip tcc88xx_gpio_chips[] = {
	{
		.label = "gpio_a",
		.base = GPIO_PORTA,
		.ngpio = 32,
	},
	{
		.label = "gpio_b",
		.base = GPIO_PORTB,
		.ngpio = 32,
	},
	{
		.label = "gpio_c",
		.base = GPIO_PORTC,
		.ngpio = 32,
	},
	{
		.label = "gpio_d",
		.base = GPIO_PORTD,
		.ngpio = 32,
	},
	{
		.label = "gpio_e",
		.base = GPIO_PORTE,
		.ngpio = 32,
	},
	{
		.label = "gpio_f",
		.base = GPIO_PORTF,
		.ngpio = 32,
	},
	{
		.label = "gpio_g",
		.base = GPIO_PORTG,
		.ngpio = 32,
	},
};

int __init tcc88xx_gpio_init(void)
{
	struct gpio_chip *chip;
	int i;

	for (i = 0; i < ARRAY_SIZE(tcc88xx_gpio_chips); i++) {
		chip = &tcc88xx_gpio_chips[i];

		chip->direction_input = tcc88xx_gpio_direction_input;
		chip->direction_output = tcc88xx_gpio_direction_output;
		chip->set = tcc88xx_gpio_set;
		chip->get = tcc88xx_gpio_get;

		gpiochip_add(chip);
	}
	return 0;
}

postcore_initcall(tcc88xx_gpio_init);
