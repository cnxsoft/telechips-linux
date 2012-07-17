/*
 * linux/drivers/video/tcc_component_cs4954.c
 *
 * Author:  <linux@telechips.com>
 * Created: March 18, 2010
 * Description: TCC Component TV-Out Driver
 *
 * Copyright (C) 20010-2011 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/gpio.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <mach/bsp.h>
#include <linux/i2c/cs4954.h>
#include "tcc_component.h"
#include "tcc_component_cs4954.h"

#ifdef CONFIG_MACH_TCC9300
#include <linux/gpio.h>
#include <mach/gpio.h>
#define CS4954_RST		TCC_GPEXT3(10)
#endif
 
/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tcc_component_cs4954: " msg); }

static unsigned short probe[] = {0, CS4954_I2C_DEVICE, I2C_CLIENT_END};
static unsigned short dummy[] = {I2C_CLIENT_END};

static struct i2c_client_address_data addr_data = {
	.normal_i2c = dummy,
	.probe      = probe,
	.ignore     = dummy,
};

static const struct i2c_device_id cs4954_i2c_id[] = {
	{ "component-cs4954", 8, },
	{ }
};

struct cs4954_i2c_chip_info {
	unsigned gpio_start;
	uint16_t reg_output;
	uint16_t reg_direction;

	struct i2c_client *client;
	struct gpio_chip gpio_chip;
};

static struct i2c_driver cs4954_i2c_driver;
static struct i2c_client *cs4954_i2c_client = NULL;

static int cs4954_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct cs4954_i2c_platform_data 	*pdata;
	struct cs4954_i2c_chip_info 		*chip;

	dprintk("%s\n", __func__);
	
	pdata = client->dev.platform_data;
	if(pdata == NULL)
	{
		dev_dbg(&client->dev, "no platform data\n");
		printk("\n tcc_cs_4954_ii2c  :  no platform data. \n");
		return -EINVAL;
	}

	chip = kzalloc(sizeof(struct cs4954_i2c_chip_info), GFP_KERNEL);
	if(chip == NULL)
	{
		printk("\n tcc_cs_4954_ii2c  :  no chip info. \n");
		return -ENOMEM;
	}

	chip->client = client;
	i2c_set_clientdata(client, chip);
	cs4954_i2c_client = client;

	return 0;
}

static int cs4954_i2c_remove(struct i2c_client *client)
{
	struct cs4954_i2c_platform_data 	*pdata = client->dev.platform_data;
	struct cs4954_i2c_chip_info 		*chip  = i2c_get_clientdata(client);
	int ret = 0;

	if(pdata->teardown)
	{
		ret = pdata->teardown(client, chip->gpio_chip.base, chip->gpio_chip.ngpio, pdata->context);
		if(ret < 0)
		{
			dev_err(&client->dev, "%s failed, %d\n", "teardown", ret);
			printk("\n tcc_cs_4954_ii2c  :  teardown error. \n");
			return ret;
		}
	}

	kfree(chip);
	cs4954_i2c_client = NULL;
	
	return 0;
}

static struct i2c_driver cs4954_i2c_driver = {
	.driver = {
		.name	= "component-cs4954",
	},
	.probe		= cs4954_i2c_probe,
	.remove		= cs4954_i2c_remove,
	.id_table	= cs4954_i2c_id,
};

int cs4954_i2c_write(unsigned char reg, unsigned char val)
{
	unsigned char data[2];
	unsigned char bytes;

	data[0]= reg;
	data[1]= val;

	bytes = 2;

	if (cs4954_i2c_client == NULL)
	{
		printk("%s, Err: No i2c client\n", __func__);
		return ENODEV;
	}

	if (i2c_master_send(cs4954_i2c_client, data, bytes) != bytes)
	{
		printk("cs4954_i2c_write error!!!! \n");
		return -EIO;    
	}

	//dprintk("cs4954_i2c_write success!!!! \n");
	return 0;
}

int cs4954_i2c_read(unsigned char reg, unsigned char *val)
{
	unsigned char bytes;
	
	if (cs4954_i2c_client == NULL)
	{
		printk("%s, Err: No i2c client\n", __func__);
		return ENODEV;
	}

	bytes = 1;
	if (i2c_master_send(cs4954_i2c_client, &reg, bytes) != bytes)
	{
		printk("read(w) error!!!! \n");
		return -EIO;    
	}

	bytes = 1;
	if (i2c_master_recv(cs4954_i2c_client, val, bytes) != bytes)
	{
		printk("read error!!!! \n");
		return -EIO;    
	}

	//dprintk("cs4954_i2c_read success!!!! \n");
	return 0;
}

void cs4954_reset(void)
{
	dprintk("%s\n", __func__);
	
#ifdef CONFIG_MACH_TCC9300
	gpio_set_value(CS4954_RST, 1);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	gpio_set_value(CS4954_RST, 0);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	gpio_set_value(CS4954_RST, 1);
#else
	GPIO *pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);

	//#reset
	BITSET(pGPIO->GPDDAT, Hw22);
	BITSET(pGPIO->GPDEN, Hw22);
	BITCLR(pGPIO->GPDFN2, Hw28-Hw24);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITCLR(pGPIO->GPDDAT, Hw22);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITSET(pGPIO->GPDDAT, Hw22);
#endif

	udelay(100);

}

int cs4954_set_ready(void)
{
	dprintk("%s\n", __func__);
	
	if (i2c_add_driver(&cs4954_i2c_driver) != 0)
	{
		printk(KERN_ERR "Failed to register cs4954 I2C client.\n");
		return ENODEV;
	}

	if (cs4954_i2c_client == NULL)
	{
		printk("%s, Err: No i2c client\n", __func__);
		return ENODEV;
	}

	cs4954_i2c_client->addr = 0;
	cs4954_i2c_write(0x0f, CS4954_I2C_ADDR);
	cs4954_i2c_client->addr = CS4954_I2C_ADDR;

	return 0;
}

void cs4954_set_end(void)
{
	dprintk("%s\n", __func__);
	
	i2c_del_driver(&cs4954_i2c_driver);
	if(cs4954_i2c_client)
	{
		kfree(cs4954_i2c_client);
		cs4954_i2c_client = NULL;
	}
}

void cs4954_set_mode(int type)
{
	unsigned char ucRegValue;

	dprintk("%s : type=%d\n", __func__, type);
	
	switch(type) 
	{
	case COMPONENT_MODE_NTSC_M:
		cs4954_i2c_write(0x00, 0x03); // NTSC-M ITU R.BT601
		cs4954_i2c_write(0x01, 0x12);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x1C);
		cs4954_i2c_write(0x11, 0x3E);
		cs4954_i2c_write(0x12, 0xF8);
		cs4954_i2c_write(0x13, 0xE0);
		cs4954_i2c_write(0x14, 0x43);
//		cs4954_i2c_write(0x28, 0xF8); // Brightness Control: -128 ~ 127
		break;

	case COMPONENT_MODE_NTSC_M_J:
		cs4954_i2c_write(0x00, 0x03); // NTSC-J ITU R.BT601
		cs4954_i2c_write(0x01, 0x10);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x1C);
		cs4954_i2c_write(0x11, 0x3E);
		cs4954_i2c_write(0x12, 0xF8);
		cs4954_i2c_write(0x13, 0xE0);
		cs4954_i2c_write(0x14, 0x43);
		break;

	case COMPONENT_MODE_NTSC_N:
	case COMPONENT_MODE_NTSC_N_J:
		break;
	case COMPONENT_MODE_NTSC_443:
		break;
		
	case COMPONENT_MODE_PAL_M:
		cs4954_i2c_write(0x00, 0x63); 
		cs4954_i2c_write(0x01, 0x12);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x15);
		cs4954_i2c_write(0x11, 0xC7);
		cs4954_i2c_write(0x12, 0xDF);
		cs4954_i2c_write(0x13, 0xCD);
		cs4954_i2c_write(0x14, 0x43);
		break;

	case COMPONENT_MODE_PAL_N:
		cs4954_i2c_write(0x00, 0xa3);
		cs4954_i2c_write(0x01, 0x30);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x15);
		cs4954_i2c_write(0x11, 0x96);
		cs4954_i2c_write(0x12, 0x15);
		cs4954_i2c_write(0x13, 0x13);
		cs4954_i2c_write(0x14, 0x54);
		break;

	case COMPONENT_MODE_PAL_B:
	case COMPONENT_MODE_PAL_G:
	case COMPONENT_MODE_PAL_H:
	case COMPONENT_MODE_PAL_I:
		cs4954_i2c_write(0x00, 0x43);
		cs4954_i2c_write(0x01, 0x30);
		cs4954_i2c_write(0x02, 0x20);
		cs4954_i2c_write(0x03, 0x00);
		cs4954_i2c_write(0x04, 0x00);
		cs4954_i2c_write(0x05, 0x1F);
		cs4954_i2c_write(0x10, 0x15);
		cs4954_i2c_write(0x11, 0x96);
		cs4954_i2c_write(0x12, 0x15);
		cs4954_i2c_write(0x13, 0x13);
		cs4954_i2c_write(0x14, 0x54);
		break;

	case COMPONENT_MODE_PSEUDO_PAL:
		break;
	case COMPONENT_MODE_PSEUDO_NTSC:
		break;
	default:
		break;
	}

	#ifdef TCC_COMPONENT_CCIR656
	cs4954_i2c_read(0x00, &ucRegValue);
	ucRegValue |= Hw3; // CCIR656
	cs4954_i2c_write(0x00, ucRegValue);
	#endif
	
	#if 0 // This is for testing
	{
		unsigned char val;
		int i;

		printk("\n");
		for (i=0; i<0x80; i++) {

			cs4954_i2c_read(i, &val);

			if (i%8 == 0)
				printk("\n[%04d|0x%02x]  %02x", i, i, val);
			else
				printk("  %02x", val);
		}
		
		printk("\n");
		printk("\n");
	}
	#endif
}

void cs4954_enable(int type)
{
	dprintk("%s : type=%d\n", __func__, type);
	
	cs4954_reset();
	cs4954_set_ready();
	cs4954_set_mode(type);
	cs4954_set_end();
}

