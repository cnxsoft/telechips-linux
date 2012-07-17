/*
 * linux/drivers/video/ths8200.c
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
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <mach/bsp.h>
#include <linux/i2c/ths8200.h>
#include "tcc_component.h"
#include "tcc_component_ths8200.h"

#include <linux/gpio.h>
#include <mach/gpio.h>

#if defined(CONFIG_ARCH_TCC88XX)
#define THS8200_RST		TCC_GPC(29)
#else
#define THS8200_RST		TCC_GPEXT3(10)
#endif
 
/* Debugging stuff */
static int debug = 0;

#define dprintk(msg...)	if (debug) { printk( "ths8200: " msg); }

static unsigned short probe[] = {0, THS8200_I2C_DEVICE, I2C_CLIENT_END};

static unsigned short dummy[] = {I2C_CLIENT_END};

static struct i2c_client_address_data addr_data = {
	.normal_i2c = dummy,
	.probe      = probe,
	.ignore     = dummy,
};

static const struct i2c_device_id ths8200_i2c_id[] = {
	{ "component-ths8200", 8, },
	{ }
};

struct ths8200_i2c_chip_info {
	unsigned gpio_start;
	uint16_t reg_output;
	uint16_t reg_direction;

	struct i2c_client *client;
	struct gpio_chip gpio_chip;
};

static struct i2c_driver ths8200_i2c_driver;
static struct i2c_client *ths8200_i2c_client = NULL;

static int ths8200_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ths8200_i2c_platform_data 	*pdata;
	struct ths8200_i2c_chip_info 		*chip;

	dprintk("%s\n", __func__);
	
	pdata = client->dev.platform_data;
	if(pdata == NULL)
	{
		dev_dbg(&client->dev, "no platform data\n");
		dprintk("\n tcc_ths8200_ii2c  :  no platform data. \n");
		return -EINVAL;
	}

	chip = kzalloc(sizeof(struct ths8200_i2c_chip_info), GFP_KERNEL);
	if(chip == NULL)
	{
		dprintk("\n tcc_ths8200_ii2c  :  no chip info. \n");
		return -ENOMEM;
	}

	chip->client = client;
	i2c_set_clientdata(client, chip);
	ths8200_i2c_client = client;

	return 0;
}

static int ths8200_i2c_remove(struct i2c_client *client)
{
	struct ths8200_i2c_platform_data 	*pdata = client->dev.platform_data;
	struct ths8200_i2c_chip_info 		*chip  = i2c_get_clientdata(client);
	int ret = 0;

	if(pdata->teardown)
	{
		ret = pdata->teardown(client, chip->gpio_chip.base, chip->gpio_chip.ngpio, pdata->context);
		if(ret < 0)
		{
			dev_err(&client->dev, "%s failed, %d\n", "teardown", ret);
			dprintk("\n tcc_ths8200_ii2c  :  teardown error. \n");
			return ret;
		}
	}

	kfree(chip);
	ths8200_i2c_client = NULL;
	
	return 0;
}

static struct i2c_driver ths8200_i2c_driver = {
	.driver = {
		.name	= "component-ths8200",
	},
	.probe		= ths8200_i2c_probe,
	.remove		= ths8200_i2c_remove,
	.id_table	= ths8200_i2c_id,
};

int ths8200_i2c_write(unsigned char reg, unsigned char val)
{
	unsigned char data[2];
	unsigned char bytes;

	data[0]= reg;
	data[1]= val;

	bytes = 2;

	if (ths8200_i2c_client == NULL)
		return ENODEV;

	if (i2c_master_send(ths8200_i2c_client, data, bytes) != bytes)
	{
		dprintk("ths8200_i2c_write error!!!! \n");
		return -EIO;    
	}

	//dprintk("ths8200_i2c_write success!!!! \n");
	return 0;
}

int ths8200_i2c_read(unsigned char reg, unsigned char *val)
{
	unsigned char bytes;
	
	if (ths8200_i2c_client == NULL)
		return ENODEV;

	bytes = 1;
	if (i2c_master_send(ths8200_i2c_client, &reg, bytes) != bytes)
	{
		dprintk("read(w) error!!!! \n");
		return -EIO;
	}

	bytes = 1;
	if (i2c_master_recv(ths8200_i2c_client, val, bytes) != bytes)
	{
		dprintk("read error!!!! \n");
		return -EIO;    
	}

	dprintk("ths8200_i2c_read success!!!! \n");
	return 0;
}

void ths8200_power(int onoff)
{
	PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	
	#if defined(CONFIG_MACH_TCC9300ST)
		/* THS8200 Power Control - GPIO_D6 */
		BITCLR(pGPIO->GPDFN0, Hw28-Hw24);
		BITSET(pGPIO->GPDEN, Hw6);
		if(onoff)
			BITSET(pGPIO->GPDDAT, Hw6);
		else
			BITCLR(pGPIO->GPDDAT, Hw6);
	#elif defined(CONFIG_MACH_TCC8800ST)
	#elif defined(CONFIG_MACH_TCC8920ST)
	#else
		/* THS8200 Power Control - GPIO_A5 */
		BITCLR(pGPIO->GPAFN0, Hw24-Hw20);
		BITSET(pGPIO->GPAEN, Hw5);
		if(onoff)
			BITSET(pGPIO->GPADAT, Hw5);
		else
			BITCLR(pGPIO->GPADAT, Hw5);
	#endif

	udelay(100);
}

void ths8200_reset(void)
{
	dprintk("ths8200_reset \n");

#if defined(CONFIG_ARCH_TCC93XX)
	#if defined(CONFIG_MACH_TCC9300ST)
		PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
		/* Reset: GPIO_D5 */
		BITSET(pGPIO->GPDDAT, Hw5);
		BITSET(pGPIO->GPDEN, Hw5);
		BITCLR(pGPIO->GPDFN0, Hw24-Hw20);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		BITCLR(pGPIO->GPDDAT, Hw5);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		BITSET(pGPIO->GPDDAT, Hw5);
		udelay(100);
	#else
		/* Reset: EXTEND_GPIO_CHIP */
		gpio_set_value(THS8200_RST, 1);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		gpio_set_value(THS8200_RST, 0);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		gpio_set_value(THS8200_RST, 1);
	#endif
#elif defined(CONFIG_ARCH_TCC88XX)
	#if defined (CONFIG_MACH_TCC8800ST)
		PGPIO pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
		/* Reset: GPIO_C29 */
		BITSET(pGPIO->GPCDAT, Hw29);
		BITSET(pGPIO->GPCEN, Hw29);
		BITCLR(pGPIO->GPCFN3, Hw24-Hw20);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		BITCLR(pGPIO->GPCDAT, Hw29);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		BITSET(pGPIO->GPCDAT, Hw29);
		udelay(100);
	#else	
		/* Reset: EXTEND_GPIO_CHIP */
		gpio_set_value(TCC_GPEXT3(10), 1);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		gpio_set_value(TCC_GPEXT3(10), 0);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		gpio_set_value(TCC_GPEXT3(10), 1);
		udelay(100);
	#endif		
#elif defined(CONFIG_ARCH_TCC892X)
	#if defined (CONFIG_MACH_TCC8920ST)
		gpio_set_value(TCC_GPB(28), 1);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		gpio_set_value(TCC_GPB(28), 0);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
		gpio_set_value(TCC_GPB(28), 1);
		{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	#else
	#endif
#else
	GPIO *pGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
	/* Reset: GPIO_D22 */
	BITSET(pGPIO->GPDDAT, Hw22);
	BITSET(pGPIO->GPDEN, Hw22);
	BITCLR(pGPIO->GPDFN2, Hw28-Hw24);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITCLR(pGPIO->GPDDAT, Hw22);
	{volatile int ttt;for(ttt=0;ttt<500;ttt++);}
	BITSET(pGPIO->GPDDAT, Hw22);
	udelay(100);
#endif

	return;
}

int ths8200_set_ready(void)
{
	if (i2c_add_driver(&ths8200_i2c_driver) != 0)
	{
		dprintk(KERN_ERR "Failed to register ths8200 I2C client.\n");
		return ENODEV;
	}

	//ths8200_i2c_client->addr = 0;
	//ths8200_i2c_write(0x0f, THS8200_I2C_ADDR);
	//ths8200_i2c_client->addr = THS8200_I2C_ADDR;

	return 0;
}

void ths8200_set_end(void)
{
	i2c_del_driver(&ths8200_i2c_driver);
	if(ths8200_i2c_client)
	{
		kfree(ths8200_i2c_client);
		ths8200_i2c_client = NULL;
	}
}

static struct ths8200_std_info ths8200_video_720p_info[] = 
{   /*720p*/
	 {THS8200_DTG2_CNTL, THS8200_DTG2_CNTL_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_A, THS8200_DTG1_SPEC_A_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_B, THS8200_DTG1_SPEC_B_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_C, THS8200_DTG1_SPEC_C_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_D_LSB, THS8200_DTG1_SPEC_D_LSB_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_E_LSB, THS8200_DTG1_SPEC_E_LSB_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_DEH_MSB, THS8200_DTG1_SPEC_DEH_MSB_720P_DEFAULT},
	 {THS8200_DTG1_SPEC_K_LSB, THS8200_DTG1_SPEC_K_LSB_720P_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_MSB, THS8200_DTG1_TOT_PIXELS_MSB_720P_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_LSB, THS8200_DTG1_TOT_PIXELS_LSB_720P_DEFAULT},
	 {THS8200_DTG1_MODE, THS8200_DTG1_MODE_720P_DEFAULT},
	 {THS8200_DTG1_FRAME_FIELD_SZ_MSB, THS8200_DTG1_FRAME_FIELD_SZ_MSB_720P_DEFAULT},
	 {THS8200_DTG1_FRAME_SZ_LSB, THS8200_DTG1_FRAME_SZ_LSB_720P_DEFAULT},
	 {THS8200_DTG1_FIELD_SZ_LSB, THS8200_DTG1_FIELD_SZ_LSB_720P_DEFAULT},
	 {THS8200_DTG2_HS_IN_DLY_LSB, THS8200_DTG2_HS_IN_DLY_LSB_720P_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_MSB, THS8200_DTG2_VS_IN_DLY_MSB_720P_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_LSB, THS8200_DTG2_VS_IN_DLY_LSB_720P_DEFAULT},
	 {0, 0},
};
	 
static struct ths8200_std_info ths8200_video_1080i_info[] =
{        /*1080I*/
	 {THS8200_TST_CNTL1, THS8200_TST_CNTL1_1080I_DEFAULT},
	 {THS8200_TST_CNTL2, THS8200_TST_CNTL2_1080I_DEFAULT},
	 {THS8200_CSM_GY_CNTL_MULT_MSB, THS8200_CSM_GY_CNTL_MULT_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_CNTL, THS8200_DTG2_CNTL_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_A, THS8200_DTG1_SPEC_A_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_B, THS8200_DTG1_SPEC_B_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_C, THS8200_DTG1_SPEC_C_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_D1, THS8200_DTG1_SPEC_D1_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_D_LSB, THS8200_DTG1_SPEC_D_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_E_LSB, THS8200_DTG1_SPEC_E_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_DEH_MSB, THS8200_DTG1_SPEC_DEH_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_K_LSB, THS8200_DTG1_SPEC_K_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_G_LSB, THS8200_DTG1_SPEC_G_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_SPEC_G_MSB, THS8200_DTG1_SPEC_G_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_MSB, THS8200_DTG1_TOT_PIXELS_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_TOT_PIXELS_LSB, THS8200_DTG1_TOT_PIXELS_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_MODE, THS8200_DTG1_MODE_1080I_DEFAULT},
	 {THS8200_DTG1_FRAME_FIELD_SZ_MSB, THS8200_DTG1_FRAME_FIELD_SZ_MSB_1080I_DEFAULT},
	 {THS8200_DTG1_FRAME_SZ_LSB, THS8200_DTG1_FRAME_SZ_LSB_1080I_DEFAULT},
	 {THS8200_DTG1_FIELD_SZ_LSB, THS8200_DTG1_FIELD_SZ_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_HLENGTH_LSB, THS8200_DTG2_HLENGTH_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_HLENGTH_LSB_HDLY_MSB, THS8200_DTG2_HLENGTH_LSB_HDLY_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_HLENGTH_HDLY_LSB, THS8200_DTG2_HLENGTH_HDLY_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH1_LSB, THS8200_DTG2_VLENGTH1_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH1_MSB_VDLY1_MSB, THS8200_DTG2_VLENGTH1_MSB_VDLY1_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_VDLY1_LSB, THS8200_DTG2_VDLY1_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH2_LSB, THS8200_DTG2_VLENGTH2_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VDLY2_LSB, THS8200_DTG2_VDLY2_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VLENGTH2_MSB_VDLY2_MSB, THS8200_DTG2_VLENGTH2_MSB_VDLY2_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_VDLY1_LSB, THS8200_DTG2_VDLY1_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_HS_IN_DLY_LSB, THS8200_DTG2_HS_IN_DLY_LSB_1080I_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_MSB, THS8200_DTG2_VS_IN_DLY_MSB_1080I_DEFAULT},
	 {THS8200_DTG2_VS_IN_DLY_LSB, THS8200_DTG2_VS_IN_DLY_LSB_1080I_DEFAULT},
	 {0, 0}
}; 	 

static int ths8200_soft_reset(int mode)
{
	int err = 0;
	unsigned char val;

	err |= ths8200_i2c_read(THS8200_CHIP_CTL, &val);
	
	dprintk("%s\n", __func__);
	
	/* reset consists of toggling the reset bit from low to high */

	val &= 0xfe;
	err |= ths8200_i2c_write(THS8200_CHIP_CTL, val);
	
	#if 0
	if(mode == COMPONENT_MODE_1080I)
		val |= 0x11;
	else
	#endif
		val |= 0x1;
		
	err |= ths8200_i2c_write(THS8200_CHIP_CTL, val);

	return err;
}

void ths8200_set_mode(int mode)
{
	dprintk("%s : mode=%d\n", __func__, mode);
	
	int err = 0,i;
	unsigned char reg, val;
	
    static struct ths8200_std_info *std_info;
	    
	switch(mode)
	{
	    case COMPONENT_MODE_1080I:
	        std_info = ths8200_video_1080i_info;
	        break;
	    case COMPONENT_MODE_720P:
	        std_info = ths8200_video_720p_info;
	        break;
	    default:
	        std_info = ths8200_video_720p_info;
	        break;
	}   
    
    ths8200_soft_reset(mode);

	#if 0
    for (i = THS8200_CSC_R11; i <= THS8200_CSC_OFFS3; i++) {
	    /* reset color space conversion registers */
	    err |= ths8200_i2c_write(i, 0x0);
    }

    /* CSC bypassed and Under overflow protection ON */
    err |= ths8200_i2c_write(THS8200_CSC_OFFS3, ((THS8200_CSC_BYPASS << THS8200_CSC_BYPASS_SHIFT) | THS8200_CSC_UOF_CNTL));

	/* 20bit YCbCr 4:2:2 input data format */
    err |= ths8200_i2c_write(THS8200_DATA_CNTL,THS8200_DATA_CNTL_MODE_20BIT_YCBCR);
	#else
    /* set color space conversion registers */
    err |= ths8200_i2c_write(THS8200_CSC_R11, 0x00);
    err |= ths8200_i2c_write(THS8200_CSC_R12, 0xDA);
    err |= ths8200_i2c_write(THS8200_CSC_R21, 0x80);
    err |= ths8200_i2c_write(THS8200_CSC_R22, 0x78);
    err |= ths8200_i2c_write(THS8200_CSC_R31, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_R32, 0x0C);
    err |= ths8200_i2c_write(THS8200_CSC_G11, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_G12, 0xDC);
    err |= ths8200_i2c_write(THS8200_CSC_G21, 0x81);
    err |= ths8200_i2c_write(THS8200_CSC_G22, 0x94);
    err |= ths8200_i2c_write(THS8200_CSC_G31, 0x81);
    err |= ths8200_i2c_write(THS8200_CSC_G32, 0xDC);
    err |= ths8200_i2c_write(THS8200_CSC_B11, 0x00);
    err |= ths8200_i2c_write(THS8200_CSC_B12, 0x4A);
    err |= ths8200_i2c_write(THS8200_CSC_B21, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_B22, 0x0C);
    err |= ths8200_i2c_write(THS8200_CSC_B31, 0x80);
    err |= ths8200_i2c_write(THS8200_CSC_B32, 0x30);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS1, 0x00);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS12, 0x08);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS23, 0x02);
    err |= ths8200_i2c_write(THS8200_CSC_OFFS3, 0x00);

	/* 30bit RGB 4:4:4 input data format */
    err |= ths8200_i2c_write(THS8200_DATA_CNTL,THS8200_DATA_CNTL_MODE_30BIT_YCBCR);
	#endif

    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC1_LSB,THS8200_DTG1_CBCR_SYNC1_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC2_LSB,THS8200_DTG1_Y_SYNC2_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC3_LSB,THS8200_DTG1_Y_SYNC3_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC1_LSB,THS8200_DTG1_CBCR_SYNC1_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC2_LSB,THS8200_DTG1_CBCR_SYNC2_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC3_LSB,THS8200_DTG1_CBCR_SYNC3_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_Y_SYNC_MSB,THS8200_DTG1_Y_SYNC_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_CBCR_SYNC_MSB,THS8200_DTG1_CBCR_SYNC_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_SPEC_H_LSB,THS8200_DTG1_SPEC_H_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_SPEC_K_MSB,THS8200_DTG1_SPEC_K_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_FLD_FLIP_LINECNT_MSB,THS8200_DTG1_FLD_FLIP_LINECNT_MSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG1_LINECNT_LSB,THS8200_DTG1_LINECNT_LSB_DEFAULT);
    err |= ths8200_i2c_write(THS8200_DTG2_HS_IN_DLY_MSB,THS8200_DTG2_HS_IN_DLY_MSB_DEFAULT);

    do {
	    reg = std_info->reg;
	    val = std_info->val;

	    err |= ths8200_i2c_write(reg, val);
	    
		if (err < 0) 
		{
		    dprintk("Set mode i2c write error\n");
			break;
	    }
	    
        std_info++;
	}while (std_info->reg);
    
    if (err < 0) 
    {
		dprintk("Set standard failed\n");
		return err;
	}
		
	ths8200_soft_reset(mode); 
	
	#if 0 /* this is for testing */
	{
		for(i=0; i<0x90; i++)
		{
			err = ths8200_i2c_read(i, &val);

			if(i%8 == 0)
				dprintk("\n0x%02x : ", i);
			dprintk("0x%02x ", val);
		}
		dprintk("\n\n");
	}
	#endif
	
	return;
}

void ths8200_enable(int mode)
{
	dprintk("ths8200_enable() : type=%d\n", mode);
	
	ths8200_power(1);
	ths8200_reset();
	ths8200_set_ready();
	ths8200_set_mode(mode);
	ths8200_set_end();
}

