/*
 * linux/arch/arm/mach-tcc88xx/include/mach/gpio.h
 *
 * Copyright (C) 2010 Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ASM_ARCH_TCC88XX_GPIO_H
#define __ASM_ARCH_TCC88XX_GPIO_H

#define GPIO_PORTA	(0 << 5)
#define GPIO_PORTB	(1 << 5)
#define GPIO_PORTC	(2 << 5)
#define GPIO_PORTD	(3 << 5)
#define GPIO_PORTE	(4 << 5)
#define GPIO_PORTF	(5 << 5)
#define GPIO_PORTG	(6 << 5)
#define GPIO_PORTEXT1	(7 << 5)
#define GPIO_PORTEXT2	(8 << 5)
#define GPIO_PORTEXT3	(9 << 5)
#define GPIO_PORTEXT4	(10 << 5)
#define GPIO_PORTEXT5	(11 << 5)

#define TCC_GPA(x)	(GPIO_PORTA | (x))
#define TCC_GPB(x)	(GPIO_PORTB | (x))
#define TCC_GPC(x)	(GPIO_PORTC | (x))
#define TCC_GPD(x)	(GPIO_PORTD | (x))
#define TCC_GPE(x)	(GPIO_PORTE | (x))
#define TCC_GPF(x)	(GPIO_PORTF | (x))
#define TCC_GPG(x)	(GPIO_PORTG | (x))
#define TCC_GPEXT1(x)	(GPIO_PORTEXT1 | (x))
#define TCC_GPEXT2(x)	(GPIO_PORTEXT2 | (x))
#define TCC_GPEXT3(x)	(GPIO_PORTEXT3 | (x))
#define TCC_GPEXT4(x)	(GPIO_PORTEXT4 | (x))
#define TCC_GPEXT5(x)	(GPIO_PORTEXT5 | (x))

#define GPIO_PULLUP	        0x0100
#define GPIO_PULLDOWN	    0x0200
#define GPIO_PULL_DISABLE   0x0400
#define GPIO_CD_BITMASK	0x0F000000
#define GPIO_CD_SHIFT	24
#define GPIO_CD(x)	(((x) + 1) << GPIO_CD_SHIFT)
#define GPIO_FN_BITMASK	0xF0000000
#define GPIO_FN_SHIFT	28
#define GPIO_FN(x)	(((x) + 1) << GPIO_FN_SHIFT)

#define gpio_get_value	__gpio_get_value
#define gpio_set_value	__gpio_set_value
#define gpio_cansleep	__gpio_cansleep

enum {
    EXINT_EI0 = 0,
    EXINT_EI1,
    EXINT_EI2,
    EXINT_EI3,
    EXINT_EI4,
    EXINT_EI5,
    EXINT_EI6,
    EXINT_EI7,
};

enum {
    EXTINT_GPIOA_0 = 0,
    EXTINT_GPIOA_01,
    EXTINT_GPIOA_02,
    EXTINT_GPIOA_03,
    EXTINT_GPIOA_04,
    EXTINT_GPIOA_05,
    EXTINT_GPIOA_06,
    EXTINT_GPIOA_07,

    EXTINT_GPIOA_08,
    EXTINT_GPIOA_09,
    EXTINT_GPIOA_10,
    EXTINT_GPIOA_11,
    EXTINT_GPIOA_12,
    EXTINT_GPIOA_13,
    EXTINT_GPIOA_14,
    EXTINT_GPIOA_15,

    EXTINT_GPIOD_05,
    EXTINT_GPIOD_06,
    EXTINT_GPIOD_07,
    EXTINT_GPIOD_08,
    EXTINT_GPIOD_09,
    EXTINT_GPIOD_10,
    EXTINT_GPIOD_13,
    EXTINT_GPIOD_14,

    EXTINT_GPIOD_15,
    EXTINT_GPIOD_16,
    EXTINT_GPIOD_17,
    EXTINT_GPIOD_18,
    EXTINT_GPIOD_19,
    EXTINT_GPIOD_20,
    EXTINT_GPIOF_23,
    EXTINT_GPIOF_24 = 31,

    EXTINT_GPIOF_25,
    EXTINT_GPIOF_26,
    EXTINT_GPIOF_27,
    EXTINT_GPIOF_20,
    EXTINT_GPIOF_17,
    EXTINT_GPIOF_13,
    EXTINT_GPIOF_10,
    EXTINT_GPIOF_08,

    EXTINT_GPIOC_28,
    EXTINT_GPIOC_29,
    EXTINT_GPIOC_30,
    EXTINT_GPIOC_31,
    EXTINT_GPIOC_09,
    EXTINT_GPIOC_13,
    EXTINT_GPIOB_28,
    EXTINT_GPIOB_29,

    EXTINT_GPIOB_30,
    EXTINT_GPIOB_31,
    EXTINT_GPIOB_08,
    EXTINT_GPIOB_12,
    EXTINT_GPIOE_04,
    EXTINT_GPIOE_05,
    EXTINT_GPIOE_24,
    EXTINT_GPIOE_25,

    EXTINT_TSWKU,
    EXTINT_TSSTOP,
    EXTINT_TSUPDOWN,
    EXTINT_RESERVED0,
    EXTINT_RESERVED1,
    EXTINT_RTC_WAKEUP,
    EXTINT_USB0_VBON,
    EXTINT_USB0_VBOFF,
};
        
int gpio_to_irq(unsigned gpio);
int tcc_gpio_config_ext_intr(unsigned intr, unsigned source);
int tcc_gpio_config(unsigned gpio, unsigned flags);
int tcc_gpio_direction_input(unsigned gpio);

#define ARCH_NR_GPIOS	TCC_GPEXT5(16)

struct board_gpio_irq_config {
	unsigned gpio;
	unsigned irq;
};

extern struct board_gpio_irq_config *board_gpio_irqs;

#include <asm-generic/gpio.h>

#endif
