/*
 * TCC CPU ID
 *
 * (C) 2007-2009 by smit Inc.
 * Author: jianjun jiang <jerryjianjun@gmail.com>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <asm/gpio.h>
#include <asm/delay.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <mach/bsp.h>
#include <linux/io.h>


#define ENABLE_FLAG		(0x80000000)
#define	MODE	Hw31
#define	CS		Hw30
#define	FSET	Hw29
#define	PRCHG	Hw27
#define	PROG	Hw26
#define	SCK		Hw25
#define	SDI		Hw24
#define	SIGDEV	Hw23
#define	A2		Hw19
#define	A1		Hw18
#define	A0		Hw17
#define	SEC		Hw16
#define	USER	(~Hw16)

#define DELAY_LOOP		100

static unsigned gECID[4] = {0,0,0,0};
// ECID Code
// -------- 31 ------------- 15 ----------- 0 --------
// [0]     |****************|****************|    : '*' is valid
// [1]     |0000000000000000|****************|    : 
//

static void IO_UTIL_ReadECID (void)
{
	volatile PGPIO	pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
  	unsigned i;

	gECID[0] = gECID[1] = gECID[2] = gECID[3] = 0;

	pGPIO->ECID0 = MODE;
	pGPIO->ECID0 = MODE | CS | SEC;

	for (i=0;i<8;i++) {
		pGPIO->ECID0 = MODE | CS | (i<<17) | SEC;
		pGPIO->ECID0 = MODE | CS | (i<<17) | SIGDEV | SEC ;
		pGPIO->ECID0 = MODE | CS | (i<<17) | SIGDEV | PRCHG | SEC ;
		pGPIO->ECID0 = MODE | CS | (i<<17) | SIGDEV | PRCHG | FSET | SEC;
		pGPIO->ECID0 = MODE | CS | (i<<17) | PRCHG  | FSET  | SEC;
		pGPIO->ECID0 = MODE | CS | (i<<17) | FSET   | SEC;
		pGPIO->ECID0 = MODE | CS | (i<<17) | SEC;
	}

	gECID[0] = pGPIO->ECID2; 	// Low  32 Bit
	gECID[1] = pGPIO->ECID3;	// High 16 Bit

	pGPIO->ECID0 = 0x00000000;  // ECID Closed

	pGPIO->ECID0 = MODE;
	pGPIO->ECID0 = MODE | CS;

	for (i=0;i<8;i++) {
		pGPIO->ECID0 = MODE | CS | (i<<17);
		pGPIO->ECID0 = MODE | CS | (i<<17) | SIGDEV  ;
		pGPIO->ECID0 = MODE | CS | (i<<17) | SIGDEV | PRCHG ;
		pGPIO->ECID0 = MODE | CS | (i<<17) | SIGDEV | PRCHG | FSET;
		pGPIO->ECID0 = MODE | CS | (i<<17) | PRCHG  | FSET;
		pGPIO->ECID0 = MODE | CS | (i<<17) | FSET;
		pGPIO->ECID0 = MODE | CS | (i<<17);
	}

	gECID[2] = pGPIO->ECID2; 	// Low  32 Bit
	gECID[3] = pGPIO->ECID3;	// High 16 Bit

	pGPIO->ECID0 = 0x00000000;  // ECID Closed
}

static ssize_t cpu_id_read(struct device *dev, struct device_attribute *attr, char *buf)
{
	IO_UTIL_ReadECID();
	return sprintf(buf, "%08X%08X", gECID[0], gECID[1]);	// chip id
//	return sprintf(buf, "%08X%08X", gECID[2], gECID[3]);	// mac address
}

static ssize_t cpu_id_write(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	return 1;
}

static DEVICE_ATTR(chip_id, 0644, cpu_id_read, cpu_id_write);

static struct attribute * cpu_id_sysfs_entries[] = {
	&dev_attr_chip_id.attr,
	NULL,
};

static struct attribute_group cpu_id_attr_group = {
	.name	= NULL,
	.attrs	= cpu_id_sysfs_entries,
};

static int cpu_id_probe(struct platform_device *pdev)
{
	return sysfs_create_group(&pdev->dev.kobj, &cpu_id_attr_group);
	return 0;
}

static int cpu_id_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &cpu_id_attr_group);
	return 0;
}

#ifdef CONFIG_PM
static int cpu_id_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int cpu_id_resume(struct platform_device *pdev)
{
	return 0;
}
#else
#define cpu_id_suspend	NULL
#define cpu_id_resume	NULL
#endif

static struct platform_driver cpu_id_driver = {
	.probe		= cpu_id_probe,
	.remove		= cpu_id_remove,
	.suspend	= cpu_id_suspend,
	.resume		= cpu_id_resume,
	.driver		= {
		.name	= "cpu-id",
	},
};

static int __devinit cpu_id_init(void)
{
	return platform_driver_register(&cpu_id_driver);
}

static void cpu_id_exit(void)
{
	platform_driver_unregister(&cpu_id_driver);
}

module_init(cpu_id_init);
module_exit(cpu_id_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jianjun jiang <jerryjianjun@gmail.com>");
MODULE_DESCRIPTION("Get CPU ID");
