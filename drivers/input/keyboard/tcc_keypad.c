/*
 * linux/drivers/input/keyboard/tcc-keys.c
 *
 * Based on:     	drivers/input/keyboard/bf54x-keys.c
 * Author: <linux@telechips.com>
 * Created: June 10, 2008
 * Description: Keypad ADC driver on Telechips TCC Series
 *
 * Copyright (C) 2008-2009 Telechips 
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
//#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/platform_device.h>
#include <asm/mach-types.h>

#include <mach/bsp.h>

#include "tca_keypad.h"

/* For id.version */
#define TCCKEYVERSION        0x0001
#define DRV_NAME             "tcc-keypad"


//#define BUTTON_DELAY    ((20 * HZ) / 1000) // 20ms
#define BUTTON_DELAY   msecs_to_jiffies(20) 

#define KEY_RELEASED    0
#define KEY_PRESSED     1

struct tcc_button
{
    int             s_scancode;
    int             e_scancode;
    int             vkcode;
};


static struct tcc_button tcc8900_buttons[] = {
#if 0
	{0x14A, 0x168, KEY_ESC},
	{0x1A4, 0x1C2, KEY_1 },
	{0x0  , 0x28 , KEY_2 },
	{0x50 , 0x6E , KEY_3 },
	{0x10E, 0x12C, KEY_4 },
	{0x3A2, 0x3B6, KEY_5 },
	{0xE1 , 0xF5 , KEY_6 },
	{0x32F, 0x343, KEY_7 },
	{0x2B2, 0x2C6, KEY_8 },
	{0x91 , 0xA5 , KEY_9 },
	{0x235, 0x249, KEY_0 },
#else
#if 1
	{0x23F, 0x243, KEY_BACK},	// VK_LWIN
	{0x11E, 0x121, KEY_7},		// VK_MENU
	{0x9A , 0x9F , KEY_DOWN},	// VK_SPACE
	{0x5A , 0x5F , KEY_UP},		// VK_UP
	{0x2BA, 0x2C1, KEY_RIGHT},	// VK_TAB
	{0x8  , 0x1E , KEY_ENTER},	// VK_ESCAPE
	{0x338, 0x340, KEY_LEFT},	// VK_RBUTTON
	{0x1B4, 0x1B9, KEY_MENU},	// VK_LEFT
	{0xEA , 0xEF , KEY_8},		// VK_RIGHT
	{0x158, 0x15C, KEY_HOME},	// VK_DOWN
//	{0x384, 0x3CB, KEY_CAMERA},	// VK_RETURN   
#else
	// Grater than 4050 -> IDLE
	// {ADC MinValue, MaxValue, Virtual Key Value}
	{0x55A, 0x582,  KEY_ESC},	// VK_LWIN
	{0x6C2, 0x6F4,  KEY_1},		// VK_MENU
	{0x5A , 0x82 ,  KEY_2},		// VK_SPACE
	{0x15E, 0x186,  KEY_3},		// VK_UP
	{0x460, 0x492,  KEY_4},		// VK_TAB
	{0xE92, 0x1004, KEY_5},		// VK_ESCAPE
	{0x398, 0x3CA,  KEY_6},		// VK_RBUTTON
	{0xCD0, 0xD0C,  KEY_7},		// VK_LEFT
	{0xADC, 0xB18,  KEY_8},		// VK_RIGHT
	{0x258, 0x294,  KEY_9},		// VK_DOWN
	{0x8E8, 0x92E,  KEY_0},		// VK_RETURN
#endif
#endif
};

static struct tcc_button tcc9300cm_buttons[] = {
	{0xEC0, 0xECF, KEY_MENU},
	{0xAF0, 0xAFF, KEY_CLEAR},
	{0x6C0, 0x6CF, KEY_UP},
	{0x460, 0x46F, KEY_HOME},
	{0x250, 0x25F, KEY_BACK},
	{0x50 , 0x5F , KEY_DOWN},
};

static struct tcc_button tcc8800_buttons[] = {
	{0x650, 0x7FF, KEY_MENU},
	{0x200, 0x2FF, KEY_DOWN},	
	{0x500, 0x5FF, KEY_HOME},
	{0x100, 0x1FF, KEY_UP},
	{0xC00, 0xE8F, KEY_BACK},
};

static struct tcc_button tcc8801_buttons[] = {
	{0x650, 0x7FF, KEY_HOME},
	{0x200, 0x2FF, KEY_DOWN},	
	{0x500, 0x5FF, KEY_MENU},
	{0x100, 0x1FF, KEY_UP},
	{0xC00, 0xE8F, KEY_BACK},
};

struct tcc_private
{
    struct input_dev *input_dev;
    struct input_polled_dev *poll_dev;
   	struct tcc_adc_client *client;
	struct platform_device *pdev;
    int key_pressed;
    int old_key;
    short status;
};

struct tcc_private        *tcc_private;
static struct tcc_adc_client *client;

extern  struct tcc_adc_client *tcc_adc_register(struct platform_device *pdev,
                                         void (*select)(unsigned int selected),
                                         void (*conv)(unsigned d0, unsigned d1),
                                         unsigned int is_ts);
extern  int tcc_adc_start(struct tcc_adc_client *client,
                   unsigned int channel, unsigned int nr_samples);        

extern  void tcc_adc_release(struct tcc_adc_client *client);                   
extern void tcc92x_sleep_mode(void);

static inline void tcc_key_convert(void)
{
   
}



static void tcc_key_poll_callback(struct input_polled_dev *dev)
{
    int     key = -1;
	unsigned int value;
	struct tcc_adc_client *client = tcc_private->client;
	
	if (client) {
		if (machine_is_tcc8900() || machine_is_tcc9200() || machine_is_tcc9201())
			value = (unsigned int)tcc_adc_start(client, 0, 7);
		else if (machine_is_tcc8800())
			value = (unsigned int)tcc_adc_start(client, 4, 7);
		else if (machine_is_tcc9300cm())
			value = (unsigned int)tcc_adc_start(client, 4, 7);
	}	

	key = tca_keypad_getkeycodebyscancode(value);

//    printk("key=%d/ status=%d/ old_key=%d, value=%d\n", key, tcc_private->status, tcc_private->old_key, value);

    if(key >= 0){
        if(tcc_private->old_key == key){
            tcc_private->key_pressed = key;
            input_report_key(tcc_private->poll_dev->input, tcc_private->key_pressed, KEY_PRESSED);
            //input_sync(tcc_private->poll_dev->input);
            tcc_private->status = KEY_PRESSED;
        } else {
            input_report_key(tcc_private->poll_dev->input, tcc_private->key_pressed, KEY_RELEASED);
            //input_sync(tcc_private->poll_dev->input);
            tcc_private->status = KEY_RELEASED;
        }
    }else{ 
        if (tcc_private->key_pressed >= 0)
        {
            input_report_key(tcc_private->poll_dev->input, tcc_private->key_pressed, KEY_RELEASED);
            //input_sync(tcc_private->poll_dev->input);
            tcc_private->key_pressed =  -1;
            tcc_private->status = KEY_RELEASED;
        }
    }

    input_sync(tcc_private->poll_dev->input);
    tcc_private->old_key = key;
}

static void tcc_keypad_select(unsigned int selected)
{
}

static void tcc_keypad_convert(unsigned int d0, unsigned int d1)
{
	
}
static int __devinit tcc_key_probe(struct platform_device *pdev)
{
    struct input_polled_dev *poll_dev; 
    struct input_dev *input_dev;
	struct tcc_adc_client *client;
	
    int error;
    int  i;
	int rc;
	
    tcc_private = kzalloc(sizeof(struct tcc_private), GFP_KERNEL);

    poll_dev = input_allocate_polled_device();
    
    if (!tcc_private || !poll_dev) {
        error = -ENOMEM;
        goto fail;
    }
        

    platform_set_drvdata(pdev, tcc_private);

	client = tcc_adc_register(pdev, tcc_keypad_select, tcc_keypad_convert, 0);
	if (IS_ERR(client)) {
		rc = PTR_ERR(client);
		goto fail;
	}
    
    poll_dev->private = tcc_private;
    poll_dev->poll = tcc_key_poll_callback;
    poll_dev->poll_interval = BUTTON_DELAY;

    input_dev = poll_dev->input;
    input_dev->evbit[0] = BIT(EV_KEY);
    input_dev->name = "telechips keypad";
    input_dev->phys = "tcc-keypad";
    input_dev->id.version = TCCKEYVERSION;

	if (machine_is_tcc8900() || machine_is_tcc9200() || machine_is_tcc9201()) {
	    for (i = 0; i < ARRAY_SIZE(tcc8900_buttons); i++)
	        set_bit(tcc8900_buttons[i].vkcode & KEY_MAX, input_dev->keybit);
	}
	else if (machine_is_tcc8800()) {
		if (system_rev >= 0x600) {	// for SV6.0 Board
		    for (i = 0; i < ARRAY_SIZE(tcc8801_buttons); i++)
		        set_bit(tcc8801_buttons[i].vkcode & KEY_MAX, input_dev->keybit);
		}
		else {
		    for (i = 0; i < ARRAY_SIZE(tcc8800_buttons); i++)
		        set_bit(tcc8800_buttons[i].vkcode & KEY_MAX, input_dev->keybit);
		}
	}
	else if (machine_is_tcc9300cm()) {
	    for (i = 0; i < ARRAY_SIZE(tcc9300cm_buttons); i++)
	        set_bit(tcc9300cm_buttons[i].vkcode & KEY_MAX, input_dev->keybit);
	}

    tcc_private->poll_dev    = poll_dev;
    tcc_private->key_pressed = -1;
    tcc_private->input_dev   = input_dev;
	tcc_private->client      = client;
	tcc_private->pdev        = pdev;
	
   	input_register_polled_device(tcc_private->poll_dev);

	if (machine_is_tcc8900())
		tcc_adc_start(client, 0, 7);
	else if (machine_is_tcc8800())
		tcc_adc_start(client, 4, 7);
	else if (machine_is_tcc9300cm())
		tcc_adc_start(client, 4, 7);
	
    return 0;
    
fail:        
    kfree(tcc_private);
   	tcc_adc_release(client);
    input_free_polled_device(poll_dev);
    return 0;
}

static int __devexit tcc_key_remove(struct platform_device *pdev)
{
    input_unregister_polled_device(tcc_private->poll_dev);
    kfree(tcc_private);
   	tcc_adc_release(client);
    return 0;
}

#ifdef CONFIG_PM
static int tcc_key_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int tcc_key_resume(struct platform_device *pdev)
{
	return 0;
}

#else
#define tcc_key_suspend NULL
#define tcc_key_resume  NULL
#endif



static struct platform_driver tcc_key_driver = {
       .driver         = {
	       .name   = "tcc-keypad",
	       .owner  = THIS_MODULE,
       },
       .probe          = tcc_key_probe,
       .remove         = tcc_key_remove,
       .suspend        = tcc_key_suspend,
       .resume         = tcc_key_resume,

};

int __init tcc_key_init(void)
{
	int res;

	if (!machine_is_tcc8900() && !machine_is_tcc9200() && !machine_is_tcc9201() && !machine_is_tcc8800() && !machine_is_tcc9300cm() )
		return -ENODEV;

    printk(KERN_INFO "Telechips ADC Keypad Driver\n");

	res =  platform_driver_register(&tcc_key_driver);
	if(res)
	{
		printk("fail : platrom driver %s (%d) \n", tcc_key_driver.driver.name, res);
		return res;
	}

	return 0;
}

void __exit tcc_key_exit(void)
{
	platform_driver_unregister(&tcc_key_driver);
}

module_init(tcc_key_init);
module_exit(tcc_key_exit);


MODULE_AUTHOR("linux@telechips.com");
MODULE_DESCRIPTION("Telechips keypad driver");
MODULE_LICENSE("GPL");

