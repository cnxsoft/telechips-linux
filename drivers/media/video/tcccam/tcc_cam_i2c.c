/****************************************************************************
 *   
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
****************************************************************************/

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif
#include "tcc_cam_i2c.h"
#include <asm/delay.h>
#include <linux/delay.h>
#include <mach/tca_ckc.h>
#ifdef CONFIG_I2C
#include <linux/i2c.h>
#endif
#include <media/cam_i2c.h> //20100716 ysseung   add to i2c struct that platform data.
#include "sensor_if.h"
#include <asm/gpio.h>
#include <linux/slab.h>

static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "Tcc_cam_i2c: " msg); }


#define 		I2C_WR		0
#define 		I2C_RD		1


#ifdef USING_HW_I2C
#include <linux/i2c.h>

#if (0) //20110211   ysseung.
#define SENSOR_DEVICE_I2C (SENSOR_I2C_ADDR>>1)
	#if defined(CONFIG_MACH_TCC9200S) || defined(CONFIG_MACH_TCC9300) || defined(CONFIG_MACH_TCC9300ST) || defined(CONFIG_MACH_TCC9300CM) || defined(CONFIG_ARCH_TCC88XX)
		static unsigned short probe[] = { 1, SENSOR_DEVICE_I2C, I2C_CLIENT_END };
	#elif defined(CONFIG_MACH_TCC9200) || defined(CONFIG_MACH_TCC9201) || defined(CONFIG_MACH_TCC8900)
		static unsigned short probe[] = { 0, SENSOR_DEVICE_I2C, I2C_CLIENT_END };
	#else
		#if defined(CONFIG_FB_TCC7901_DEMOBOARD)
			static unsigned short probe[] = { 1, SENSOR_DEVICE_I2C, I2C_CLIENT_END };
		#elif defined(CONFIG_FB_TCC7901_LMS350DF01)
			static unsigned short probe[] = { 0, SENSOR_DEVICE_I2C, I2C_CLIENT_END };
		#endif
	#endif
static unsigned short dummy[] = {I2C_CLIENT_END};

static struct i2c_client_address_data addr_data = {
	.normal_i2c = dummy,
	.probe      = probe,
	.ignore     = dummy,
};
#endif

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern int CameraID;
static const struct i2c_device_id cam_i2c_id0[] = {
	{ "tcc-cam-sensor-0", 0, },
	{ }
};

static const struct i2c_device_id cam_i2c_id1[] = {
	{ "tcc-cam-sensor-1", 1, },
	{ }
};
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
static const struct i2c_device_id cam_i2c_id[] = {
	{ "tcc-cam-sensor", 0, },
	{ }
};
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

struct cam_i2c_chip_info {
	unsigned gpio_start;
	uint16_t reg_output;
	uint16_t reg_direction;

	struct i2c_client *client;
	struct gpio_chip gpio_chip;
};
#endif

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
static struct i2c_client *cam_i2c_client[] = { NULL, NULL };
#else
static struct i2c_client *cam_i2c_client = NULL;
#endif

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
static int cam_i2c_probe0(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct cam_i2c_platform_data 	*pdata;
	struct cam_i2c_chip_info 		*chip;

	pdata = client->dev.platform_data;
	if(pdata == NULL)
	{
		dev_dbg(&client->dev, "no platform data\n");
		printk("\n tcc_cam_i2c  :  no platform data. \n");
		return -EINVAL;
	}

	chip = kzalloc(sizeof(struct cam_i2c_chip_info), GFP_KERNEL);
	if(chip == NULL)
	{
		printk("\n tcc_cam_i2c  :  no chip info. \n");
		return -ENOMEM;
	}

	chip->client = client;
	i2c_set_clientdata(client, chip);
	cam_i2c_client[0] = client;
	printk(KERN_INFO "_______%s() :  cam_i2c_client = 0x%p \n", __func__, cam_i2c_client[0]);

	return 0;
}

static int cam_i2c_remove0(struct i2c_client *client)
{
	struct cam_i2c_platform_data 	*pdata = client->dev.platform_data;
	struct cam_i2c_chip_info 		*chip  = i2c_get_clientdata(client);
	int ret = 0;

	if(pdata->teardown)
	{
		ret = pdata->teardown(client, chip->gpio_chip.base, chip->gpio_chip.ngpio, pdata->context);
		if(ret < 0)
		{
			dev_err(&client->dev, "%s failed, %d\n", "teardown", ret);
			printk("\n tcc_cam_i2c  :  teardown error. \n");
			return ret;
		}
	}

	kfree(chip);
	cam_i2c_client[0] = NULL;
	
	return 0;
}

static int cam_i2c_probe1(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct cam_i2c_platform_data 	*pdata;
	struct cam_i2c_chip_info 		*chip;

	pdata = client->dev.platform_data;
	if(pdata == NULL)
	{
		dev_dbg(&client->dev, "no platform data\n");
		printk("\n tcc_cam_i2c  :  no platform data. \n");
		return -EINVAL;
	}

	chip = kzalloc(sizeof(struct cam_i2c_chip_info), GFP_KERNEL);
	if(chip == NULL)
	{
		printk("\n tcc_cam_i2c  :  no chip info. \n");
		return -ENOMEM;
	}

	chip->client = client;
	i2c_set_clientdata(client, chip);
	cam_i2c_client[1] = client;
	printk(KERN_INFO "_______%s() :  cam_i2c_client = 0x%p \n", __func__, cam_i2c_client[1]);

	return 0;
}

static int cam_i2c_remove1(struct i2c_client *client)
{
	struct cam_i2c_platform_data 	*pdata = client->dev.platform_data;
	struct cam_i2c_chip_info 		*chip  = i2c_get_clientdata(client);
	int ret = 0;
	
	if(pdata->teardown)
	{
		ret = pdata->teardown(client, chip->gpio_chip.base, chip->gpio_chip.ngpio, pdata->context);
		if(ret < 0)
		{
			dev_err(&client->dev, "%s failed, %d\n", "teardown", ret);
			printk("\n tcc_cam_i2c  :  teardown error. \n");
			return ret;
		}
	}

	kfree(chip);
	cam_i2c_client[1] = NULL;
	
	return 0;
}
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
static int cam_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct cam_i2c_platform_data 	*pdata;
	struct cam_i2c_chip_info 		*chip;

	pdata = client->dev.platform_data;
	if(pdata == NULL)
	{
		dev_dbg(&client->dev, "no platform data\n");
		printk("\n tcc_cam_i2c  :  no platform data. \n");
		return -EINVAL;
	}

	chip = kzalloc(sizeof(struct cam_i2c_chip_info), GFP_KERNEL);
	if(chip == NULL)
	{
		printk("\n tcc_cam_i2c  :  no chip info. \n");
		return -ENOMEM;
	}

	chip->client = client;
	i2c_set_clientdata(client, chip);
	cam_i2c_client = client;
	printk(KERN_INFO "_______%s() :  cam_i2c_client = 0x%p \n", __func__, cam_i2c_client);

	return 0;
}

static int cam_i2c_remove(struct i2c_client *client)
{
	struct cam_i2c_platform_data 	*pdata = client->dev.platform_data;
	struct cam_i2c_chip_info 		*chip  = i2c_get_clientdata(client);
	int ret = 0;
	
	if(pdata->teardown)
	{
		ret = pdata->teardown(client, chip->gpio_chip.base, chip->gpio_chip.ngpio, pdata->context);
		if(ret < 0)
		{
			dev_err(&client->dev, "%s failed, %d\n", "teardown", ret);
			printk("\n tcc_cam_i2c  :  teardown error. \n");
			return ret;
		}
	}

	kfree(chip);
	cam_i2c_client = NULL;
	
	return 0;
}
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#else // defined(CONFIG_ARCH_TCC93XX) 
static int cam_i2c_probe(struct i2c_adapter *adap, int addr, int kind)
{
    struct i2c_client *i2c;
    int ret;
	
	dprintk("cam_i2c_probe! \n");

    i2c = kzalloc(sizeof(*i2c), GFP_KERNEL);
    if (!i2c)
	{
		printk("i2c alloc failed!!");
        return -ENOMEM;
	}

    strcpy(i2c->name, "CAM_SENSOR");
	i2c->flags = I2C_M_MODE;
    i2c->addr = addr;
    i2c->adapter = adap;
    i2c->driver = &cam_i2c_driver;

    cam_i2c_client = i2c;

    /* attach i2c client device */
    ret = i2c_attach_client(i2c);
    if (ret < 0) {
        printk("%s: failed to attach codec at addr %x\n", __func__, addr);
        goto err;
    }

    return ret;
err:
    kfree(i2c);
    return ret;
}

static int cam_i2c_detach(struct i2c_client *client)
{
	i2c_detach_client(client);

	if(cam_i2c_client)
	{
		kfree(cam_i2c_client);
		cam_i2c_client = NULL;
	}
	return 0;
}

static int cam_i2c_attach(struct i2c_adapter *adap)
{
	dprintk("cam_i2c_attach! \n");

    return i2c_probe(adap, &addr_data, cam_i2c_probe);
}
#endif // defined(CONFIG_ARCH_TCC93XX)

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
static struct i2c_driver cam_i2c_driver0 = {	// I2C driver for Back Camera
	.driver = {
		.name	= "tcc-cam-sensor-0",
	},
	.probe		= cam_i2c_probe0,
	.remove		= cam_i2c_remove0,
	.id_table	= cam_i2c_id0,
};

static struct i2c_driver cam_i2c_driver1 = {	// I2C driver for Front Camera
	.driver = {
		.name	= "tcc-cam-sensor-1",
	},
	.probe		= cam_i2c_probe1,
	.remove		= cam_i2c_remove1,
	.id_table	= cam_i2c_id1,
};
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
static struct i2c_driver cam_i2c_driver = {
	.driver = {
		.name	= "tcc-cam-sensor",
	},
	.probe		= cam_i2c_probe,
	.remove		= cam_i2c_remove,
	.id_table	= cam_i2c_id,
};
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#else // defined(CONFIG_ARCH_TCC93XX)
static struct i2c_driver cam_i2c_driver = {
    .driver = {
        .name   = "tcc-cam-sensor",
        .owner  = THIS_MODULE,
    },
    .id             = I2C_DRIVERID_CAMERA,
    .attach_adapter = cam_i2c_attach,
    .detach_client  = cam_i2c_detach,
    .command        = NULL,
};
#endif

#else // USING_HW_I2C

typedef struct stI2C_Vector
{
	unsigned long int tgtaddr;	
	unsigned long int port;
}tI2C_Vector;

static char I2C_use_flag;
static unsigned int	guiI2CTgt;
static unsigned char gI2C_SW_Flag = 0;
#define	IS(X, MASK)					( (unsigned int)(X) & ((unsigned int)(MASK)) )

static const tI2C_Vector I2C_VectorTable[I2C_MAX_VECTOR + 1][I2C_SIGNAL_BUS] =
{
	{{&(HwGPIOA->GPDAT), Hw0}, {&(HwGPIOA->GPDAT), Hw1}}, //CODEC
	{{&(HwGPIOA->GPDAT), Hw0}, {&(HwGPIOA->GPDAT), Hw1}}, //TVOUT
	{{&(HwGPIOA->GPDAT), Hw8}, {&(HwGPIOA->GPDAT), Hw9}}, //TOUCH
	{{&(HwGPIOA->GPDAT), Hw8}, {&(HwGPIOA->GPDAT), Hw9}}, //CAMERA
	{{&(HwGPIOA->GPDAT), Hw0}, {&(HwGPIOA->GPDAT), Hw1}}, //I2C_MAX_VECTOR
};
static tI2C_Vector I2C_Vector[I2C_SIGNAL_BUS];

static unsigned int delay_cnt = 100; //500Mhz
static void delay_set(void)
{
	unsigned long cpu_clk;

	cpu_clk = tca_ckc_getcpu() / 10000;

	if(cpu_clk < 50)
		delay_cnt = 10;
	else if(cpu_clk < 100)
		delay_cnt = 20;
	else if(cpu_clk < 150)
		delay_cnt = 40;
	else if(cpu_clk < 300)
		delay_cnt = 60;
	else if(cpu_clk < 400)
		delay_cnt = 80;
	else if(cpu_clk < 500)
		delay_cnt = 100;
	else
		delay_cnt = 130;

	dprintk("delay_set: cpu %d Mhz, delay_cnt = %d \n", cpu_clk, delay_cnt);
	
}

static void delay760(unsigned int count)
{
#if 1
	for(count=delay_cnt;count>0;count--) {
		asm volatile ( "\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n");
		asm volatile ( "\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n");
	}

#else
	udelay(2*count);
#endif
}


int i2c_register(unsigned int i2ctgt)
{
	guiI2CTgt = i2ctgt;

#if defined( CONFIG_MACH_TCC9200S_SPACEY)||defined(CONFIG_MACH_TCC9200S_M340EVB)
	if(!IS(HwGPIOE->GPEN, Hw2))
	{
		BITCSET(HwGPIOE->GPEN, Hw2, Hw2);
		gI2C_SW_Flag = 1;
	}

	if(!IS(HwGPIOE->GPEN, Hw1))
	{
		BITCSET(HwGPIOE->GPEN, Hw1, Hw1);
		gI2C_SW_Flag = 3;
	}
#else
	if(!IS(HwGPIOA->GPEN, Hw8))
	{
		BITCSET(HwGPIOA->GPEN, Hw8, Hw8);
		gI2C_SW_Flag = 1;
	}

	if(!IS(HwGPIOA->GPEN, Hw9))
	{
		BITCSET(HwGPIOA->GPEN, Hw9, Hw9);
		gI2C_SW_Flag = 3;
	}

	BITCSET(HwGPIOA->GPFN1, HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN0(0));
	BITCSET(HwGPIOA->GPFN1, HwPORTCFG_GPFN1_MASK, HwPORTCFG_GPFN1(0));
#endif

	I2C_Vector[I2C_CLK]  = I2C_VectorTable[i2ctgt][I2C_CLK];
	I2C_Vector[I2C_DATA] = I2C_VectorTable[i2ctgt][I2C_DATA];

	return 1;
}


int i2c_deregister(unsigned int i2ctgt)
{
#if defined(CONFIG_MACH_TCC9200S_SPACEY)||defined(CONFIG_MACH_TCC9200S_M340EVB)
	if(gI2C_SW_Flag == 3)
	{
		BITCLR(HwGPIOE->GPEN, (Hw2|Hw1));
	}
	else if(gI2C_SW_Flag == 1)
	{
		BITCLR(HwGPIOE->GPEN, Hw2);
	}			
#else
	if(gI2C_SW_Flag == 3)
	{
		BITCLR(HwGPIOA->GPEN, (Hw8|Hw9));
	}
	else if(gI2C_SW_Flag == 1)
	{
		BITCLR(HwGPIOA->GPEN, Hw8);
	}

	BITCSET(HwGPIOA->GPFN1, HwPORTCFG_GPFN0_MASK, HwPORTCFG_GPFN0(1));
	BITCSET(HwGPIOA->GPFN1, HwPORTCFG_GPFN1_MASK, HwPORTCFG_GPFN1(1));
#endif

	gI2C_SW_Flag = 0;

	return 1;
}


// set to i2c start bit
void i2c_comm_s(void)
{	
	i2c_wr;
	i2c_clk_hi;
//	i2c_wr;
	i2c_data_hi;
	delay760(i2c_full_delay);

	i2c_wr;
	i2c_data_lo;
	delay760(i2c_full_delay);
}


// set to i2c stop bit
void i2c_comm_p(void)
{
	i2c_wr;
	i2c_clk_lo;	
//	i2c_wr;
	delay760(i2c_half_delay);
	i2c_data_lo;

	i2c_wr;		
	i2c_clk_hi;
	delay760(i2c_full_delay);
	
	i2c_data_hi;
	delay760(i2c_full_delay);

	i2c_wr;
	i2c_clk_hi;
	i2c_data_hi;
	delay760(i2c_full_delay);
}


// output data : write
void i2c_out_byte(unsigned char *ptr)
{	
	unsigned char i, temp;

	i2c_wr;	
	i2c_clk_lo;
//	i2c_wr;
	delay760(i2c_half_delay);
	
	if(*ptr & 0x80)
	{
		i2c_data_hi;
	}
	else
	{
		i2c_data_lo;
	}
	delay760 (i2c_half_delay);

	i2c_clk_hi;
	delay760 (i2c_full_delay);
	
	for(i=1; i < 8; i++)
	{
		i2c_clk_lo;
		i2c_wr;		
		delay760 (i2c_half_delay);

		temp = *ptr << i;
		temp &= 0x80;
		if(temp)
		{
			i2c_data_hi;
		}
		else
		{
			i2c_data_lo;
		}
		delay760(i2c_half_delay);
		
		i2c_clk_hi;
		if(i == 7)		
			delay760(i2c_half_delay);
		else
			delay760(i2c_full_delay);			
	}
}


// get acknowledge
unsigned char i2c_get_ack(void)
{	
	unsigned char i = 0;

	delay760(i2c_full_delay);
	i2c_clk_lo;
	i2c_rd;
	delay760(i2c_full_delay);

	i2c_clk_hi;
	i = (unsigned char)(((*(volatile unsigned long int *)I2C_Vector[I2C_DATA].tgtaddr) & (I2C_Vector[I2C_DATA].port)) ? i2c_noack : i2c_ack);
	delay760(i2c_full_delay);

	return i;
}


// input data : read
void i2c_in_byte(unsigned char *ptr)
{
	unsigned char i, temp = 0, val = 0;

	i2c_clk_lo;
	i2c_rd;

	for(i=0; i < 8; i++)
	{
		i2c_clk_lo;
		delay760(i2c_full_delay);

		i2c_clk_hi;
		temp = (unsigned char)(((*(volatile unsigned long int *)(I2C_Vector[I2C_DATA].tgtaddr)) & (I2C_Vector[I2C_DATA].port)) ? 1 : 0);
		val = (val << 1) | temp;
		delay760(i2c_full_delay);
	}

	*ptr = val;
}


// put acknowledge
void i2c_do_ack(unsigned char bit)
{
	i2c_wr; 
	i2c_clk_lo;
//	i2c_wr;
	delay760(i2c_half_delay);
	if(bit)
	{
		i2c_data_hi;
	}
	else
	{
		i2c_data_lo;
	}
	delay760(i2c_full_delay);
		
	i2c_clk_hi;
	delay760(i2c_full_delay);
}


static unsigned char i2c_write(unsigned int i2ctgt, unsigned char SlaveAddr, 
									unsigned char *subaddr, unsigned char subaddr_bytes, 
									unsigned char *data, unsigned char data_bytes )
{
	unsigned char i;

	i2c_register(i2ctgt);

	/* for prevent from access on same time */
	while(I2C_use_flag == I2C_USED) { msleep(1); }
	I2C_use_flag = I2C_USED;

	i2c_wr
	i2c_clk_hi
	i2c_data_hi
	delay760(i2c_Tbuf);
	
	i2c_comm_s();
	i2c_out_byte(&SlaveAddr);
	if(i2c_get_ack() != i2c_ack) 
	{
		i2c_comm_p();
		I2C_use_flag = I2C_FREE;
		return i2c_err;
	}

	if((subaddr_bytes == 2 && (subaddr[0]<<8 + subaddr[1]) != (short)I2C_SUBADDR_NOUSE) ||
		(subaddr_bytes == 1 && (*subaddr) != (char)I2C_SUBADDR_NOUSE))
	{
		for(i=0; i<subaddr_bytes; i++)
		{
			i2c_out_byte(subaddr);
			if(i2c_get_ack() != i2c_ack)
			{
				i2c_comm_p();
				I2C_use_flag = I2C_FREE;
				return i2c_err;
			}
			subaddr++;
		}
	}

	for(i=0; i < data_bytes; i++)
	{
		i2c_out_byte(data);
		if(i2c_get_ack() != i2c_ack)
		{
			i2c_comm_p();
			I2C_use_flag = I2C_FREE;
			return i2c_err;
		}
		data++;
	}

	i2c_comm_p();
	i2c_deregister(i2ctgt);
	I2C_use_flag = I2C_FREE;
		
	return i2c_ok;
}


static unsigned char i2c_read(unsigned int i2ctgt, unsigned char SlaveAddr, 
									unsigned char *subaddr, unsigned char subaddr_bytes, 
									unsigned char *data, unsigned char data_bytes )
{
	unsigned char i, SA = SlaveAddr;
	
	i2c_register(i2ctgt);
	
	/* for prevent from access on same time */
	while(I2C_use_flag == I2C_USED) { msleep(1); }
	I2C_use_flag = I2C_USED;

	i2c_wr
	i2c_clk_hi
	i2c_data_hi
	delay760(i2c_Tbuf);

	if((subaddr_bytes == 2 && (subaddr[0]<<8 + subaddr[1]) != (short)I2C_SUBADDR_NOUSE) ||
		(subaddr_bytes == 1 && (*subaddr) != (char)I2C_SUBADDR_NOUSE))
	{
		i2c_comm_s();
		i2c_out_byte(&SA);
		if(i2c_get_ack() != i2c_ack)
		{
			i2c_comm_p();
			I2C_use_flag = I2C_FREE;
			return i2c_err;
		}

		for(i=0; i < subaddr_bytes; i++)
		{
			i2c_out_byte(subaddr);
			if(i2c_get_ack() != i2c_ack)
			{
				i2c_comm_p();
				I2C_use_flag = I2C_FREE;
				return i2c_err;
			}
			subaddr++;
		}

		i2c_comm_p();
	}

	SA = SlaveAddr | I2C_RD;
	i2c_comm_s();
	i2c_out_byte(&SA);
	if(i2c_get_ack() != i2c_ack)
	{
		i2c_comm_p();
		I2C_use_flag = I2C_FREE;
		return i2c_err;
	}
	
	for(i=0; i < data_bytes; i++)
	{
		i2c_in_byte(data);
		if(i < (data_bytes-1))
		{
			i2c_do_ack(i2c_ack);
		}
		else
		{
			i2c_do_ack(i2c_noack);
		}
		data++;
		
	}

	i2c_comm_p();
	I2C_use_flag = I2C_FREE;
	
	return i2c_ok;
}
#endif /* USING_HW_I2C */

int	DDI_I2C_Initialize(void)
{
#if (1)
	return 0;
#else
	#ifdef USING_HW_I2C
	int ret;

	ret = i2c_add_driver(&cam_i2c_driver);
	if(ret != 0)
	{
		printk(KERN_ERR "Failed to register Sensor I2C client.\n");
		return ENODEV;
	}
	#endif 

	return 0;
#endif
}

int	DDI_I2C_Terminate(void)
{
#if (1)
	return 0;
#else
	#ifdef USING_HW_I2C
	i2c_del_driver(&cam_i2c_driver);
	
	if(cam_i2c_client)
	{
		kfree(cam_i2c_client);
		cam_i2c_client = NULL;
	}
	#endif

	dprintk("DDI_I2C_Terminate \n");

	return 0;
#endif
}

int DDI_I2C_Write(unsigned char* data, unsigned short reg_bytes, unsigned short data_bytes)
{
	unsigned short bytes = reg_bytes + data_bytes;

#if defined(USING_HW_I2C)
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(i2c_master_send(cam_i2c_client[CameraID], data, bytes) != bytes)
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(i2c_master_send(cam_i2c_client, data, bytes) != bytes)
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	{
		printk("write error!!!! \n");
		return -EIO; 
	}
#else // USING_HW_I2C
	delay_set();
	if(i2c_write(I2C_CAMERA, (unsigned char)SENSOR_I2C_ADDR, data, reg_bytes, data+reg_bytes, data_bytes) == i2c_err)
	{
		printk("write error, delay_cnt = %d !!!! \n", delay_cnt);
		return -EIO; 
	}
#endif // USING_HW_I2C

	return 0;
}

int DDI_I2C_Read(unsigned short reg, unsigned char reg_bytes, unsigned char *val, unsigned short val_bytes)
{
	unsigned char data[2];
	
	if(reg_bytes == 2)
	{
		data[0]= reg>>8;
		data[1]= (u8)reg&0xff;
	}
	else
	{
		data[0]= (u8)reg&0xff;
	}
	
	#ifdef USING_HW_I2C	
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(i2c_master_send(cam_i2c_client[CameraID], data, reg_bytes) != reg_bytes)
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(i2c_master_send(cam_i2c_client, data, reg_bytes) != reg_bytes)
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	{
	     printk("write error for read!!!! \n");			
	     return -EIO; 
	}

	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(i2c_master_recv(cam_i2c_client[CameraID], val, val_bytes) != val_bytes)
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	if(i2c_master_recv(cam_i2c_client, val, val_bytes) != val_bytes)
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	{
		printk("read error!!!! \n");
		return -EIO; 
	}
	#else
	delay_set();
	if(i2c_read(I2C_CAMERA, (unsigned char)SENSOR_I2C_ADDR, data, reg_bytes, val, val_bytes) == i2c_err)
	{
		printk("read error, delay_cnt = %d !!!! \n", delay_cnt);
		return -1; 
	}
	#endif

    return 0;
}

static int __init cam_i2c_init(void)
{
	int ret;

	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	ret = i2c_add_driver(&cam_i2c_driver0);
	ret = i2c_add_driver(&cam_i2c_driver1);
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	ret = i2c_add_driver(&cam_i2c_driver);
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	
	return ret;
}
module_init(cam_i2c_init);

static void __exit cam_i2c_exit(void)
{
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	i2c_del_driver(&cam_i2c_driver0);
	i2c_del_driver(&cam_i2c_driver1);
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	i2c_del_driver(&cam_i2c_driver);
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
}
module_exit(cam_i2c_exit);

