/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of GalaxyCore Inc. (C) 2003
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   image_sensor.c
 *
 * Version:
 * --------
 *   GT2005 MTK 6235 DRV V1.1.0
 *
 * Release Date:
 * ------------
 *   2010-10-25
 *
*****************************************************************************/

#include <linux/delay.h>
#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/io.h>
//#include "sensor_if.h"
//#include "cam.h"
#include "tcc_cam_i2c.h"
#include "linux/kernel.h"


#if defined(CONFIG_ARCH_TCC92X) || defined(CONFIG_ARCH_TCC93XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

//#include "drv_comm.h"
//#include "IntrCtrl.h"
//#include "reg_base.h"
//#include "gpio_sw.h"
//#include "sccb.h"
//#include "isp_if.h"
#include "gt2005_drv.h"
//#include "camera_para.h"
//#include "upll_ctrl.h"
//#include "med_api.h"

#if 0
static int debug	   = 1;
#else
static int debug	   = 0;
#endif

#define dprintk(msg...)	if (debug) { printk( "GT2005_2mp: " msg); }


/* Global Valuable */

//#define A_LIGHT_CORRECTION
//#define INTERPOLATION_3M

bool sensor_night_mode= FALSE;
bool gPVmode = FALSE;
bool VEDIO_encode_mode = FALSE; 
bool sensor_cap_state = FALSE;
bool gc_effect_on = FALSE;


void GT2005_delay(int ms)
{
#if 1 //ZzaU
	unsigned int msec;

	msec = ms / 10; //10msec unit

	if(!msec)
		msleep(1);
	else
		msleep(msec);
#else
	int totCnt, iCnt, temp = 0;

	totCnt = ms*20000;
	for(iCnt = 0; iCnt < totCnt; iCnt++)
	{
		temp++;
	}
#endif
}

#ifndef HW_SCCB

void Delayms_GC(kal_uint32 ms)
{
	kal_uint32 delay;
	for (delay =0;delay <5000*ms;delay++) {}
}

void SCCB_send_byte(kal_uint8 send_byte)
{
	volatile signed char i;
	volatile kal_uint32 j;
	for(j=0;j<SENSOR_I2C_DELAY;j++);
	for (i=7;i>=0;i--)
	{	/* data bit 7~0 */
		if (send_byte & (1<<i))
		{
			SET_SCCB_DATA_HIGH;
		}
		else
		{
			SET_SCCB_DATA_LOW;
		}
		for(j=0;j<SENSOR_I2C_DELAY;j++);
		SET_SCCB_CLK_HIGH;
		for(j=0;j<SENSOR_I2C_DELAY;j++);
		SET_SCCB_CLK_LOW;
		for(j=0;j<SENSOR_I2C_DELAY;j++);
	}
	/* don't care bit, 9th bit */
     
	SET_SCCB_DATA_INPUT;
	for(j=0;j<SENSOR_I2C_DELAY;j++);
	SET_SCCB_CLK_HIGH;
	for(j=0;j<SENSOR_I2C_DELAY;j++);
	SET_SCCB_CLK_LOW;
	SET_SCCB_DATA_OUTPUT;
	SET_SCCB_DATA_LOW;

}	/* SCCB_send_byte() */

kal_uint8 SCCB_get_byte(void)
{
	volatile signed char i;
	volatile kal_uint32 j;
	kal_uint8 get_byte=0;

	SET_SCCB_DATA_INPUT;

	for (i=7;i>=0;i--)
	{	/* data bit 7~0 */
		SET_SCCB_CLK_HIGH;
		if (GET_SCCB_DATA_BIT)
			get_byte |= (1<<i);
		SET_SCCB_CLK_LOW;
		for(j=0;j<SENSOR_I2C_DELAY;j++);
		for(j=0;j<SENSOR_I2C_DELAY;j++);
	}
	/* don't care bit, 9th bit */
	SET_SCCB_DATA_OUTPUT;

	//send NACK to slave
	SET_SCCB_DATA_HIGH;          //SET_SCCB_DATA_HIGH
	for(j=0;j<SENSOR_I2C_DELAY;j++);
	SET_SCCB_CLK_HIGH;
	for(j=0;j<SENSOR_I2C_DELAY;j++);
	SET_SCCB_CLK_LOW;
  	for(j=0;j<SENSOR_I2C_DELAY;j++);
	return get_byte;
}	/* SCCB_get_byte() */

#endif


static void GT2005_write_cmos_sensor(const unsigned short iAddr, const unsigned char iPara)
{
	volatile unsigned char iI;

  #ifdef HW_SCCB
		unsigned char arrData[128];
		unsigned char bytes;

		bytes =0;

		arrData[bytes++] = (iAddr>>8)&0xff;
		arrData[bytes++] = (unsigned char)iAddr&0xff;
		arrData[bytes] = iPara;

		if(DDI_I2C_Write(arrData, 1, 2))
		{
			printk("[GT2005_2MP]IIC_DATAL_ERR!!!!!!!!!!!!!!!!!!");	
			//return IIC_ERR;
		}
  #else
	I2C_START_TRANSMISSION;
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(GT2005_WRITE_ID);
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(iAddr >> 8); // send high-byte reg. address first
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(iAddr & 0x00FF); // send low-byte reg. address
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(iPara);
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	I2C_STOP_TRANSMISSION;
  #endif  /*  HW_SCCB */
}   /*  GT2005_write_cmos_sensor()  */


static unsigned char GT2005_read_cmos_sensor(const unsigned char iAddr)
{
	volatile unsigned char iI;
	unsigned char iGetByte = 0;
	unsigned char tmpDataForHWI2C[1];

  #ifdef HW_SCCB
	if(DDI_I2C_Read(iAddr, 2, tmpDataForHWI2C, 1))
	{
		printk("[GT2005_2MP]IIC_dadd_ERR!!!!!!!!!!!!");
		return -1;
	}

	//*pData = (((uint16)*(tmpDataForHWI2C+1)) + ((uint16)*(tmpDataForHWI2C)<<8));

	dprintk("TCC Hw-IIC Read Data : %x %x\n", iAddr, *tmpDataForHWI2C);

	iGetByte = *tmpDataForHWI2C;
  #else
	I2C_START_TRANSMISSION;
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(GT2005_WRITE_ID);
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(iAddr >> 8); // send high-byte reg. address first
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(iAddr & 0x00FF); // send low-byte reg. address
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	I2C_STOP_TRANSMISSION;
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	I2C_START_TRANSMISSION;
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	SCCB_send_byte(GT2005_READ_ID);
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	iGetByte = SCCB_get_byte();
	for (iI = 0; iI < SENSOR_I2C_DELAY; iI++);
	I2C_STOP_TRANSMISSION;
  #endif
  
	return iGetByte;
}   /*  GT2005_read_cmos_sensor()  */



static void GT2005_write_shutter()
{
	unsigned short shutter,AGain_shutter,DGain_shutter;

	GT2005_write_cmos_sensor(0x0300, 0xc1);	
	shutter= (GT2005_read_cmos_sensor(0x0012)<<8 )|( GT2005_read_cmos_sensor(0x0013));    
	AGain_shutter= (GT2005_read_cmos_sensor(0x0014)<<8 )|( GT2005_read_cmos_sensor(0x0015));
	DGain_shutter= (GT2005_read_cmos_sensor(0x0016)<<8 )|( GT2005_read_cmos_sensor(0x0017));
	GT2005_write_cmos_sensor(0x0300, 0x41); //close ALC

#ifndef INTERPOLATION_3M
	shutter = shutter / 2; 
#else
	shutter = shutter / 4; 
#endif

	GT2005_write_cmos_sensor(0x0305,  shutter&0xff);           
	GT2005_write_cmos_sensor(0x0304, (shutter >>8)&0xff); 

	GT2005_write_cmos_sensor(0x0307,  AGain_shutter&0xff);      
	GT2005_write_cmos_sensor(0x0306, (AGain_shutter >>8)&0xff); //AG

	GT2005_write_cmos_sensor(0x0308,  DGain_shutter&0xff);   //DG

}	/* GT2005_write_shutter */

unsigned short GT2005_read_shutter(void)
{  

}	/* GT2005_read_shutter */

void GT2005_set_dummy(unsigned short pixels, unsigned short lines)
{

}	/* GT2005_set_dummy */

static void GT2005_YUV_sensor_initial_setting(void)
{
	dprintk("%s Start!! \n", __FUNCTION__);

	GT2005_write_cmos_sensor(0x0101 , 0x00);
	GT2005_write_cmos_sensor(0x0103 , 0x00);

	//Hcount&Vcount
	GT2005_write_cmos_sensor(0x0105 , 0x00);
	GT2005_write_cmos_sensor(0x0106 , 0xF0);
	GT2005_write_cmos_sensor(0x0107 , 0x00);
	GT2005_write_cmos_sensor(0x0108 , 0x1C);

	//Binning&Resoultion
	GT2005_write_cmos_sensor(0x0109 , 0x01);
	GT2005_write_cmos_sensor(0x010A , 0x00);
	GT2005_write_cmos_sensor(0x010B , 0x00);
	GT2005_write_cmos_sensor(0x010C , 0x00);
	GT2005_write_cmos_sensor(0x010D , 0x08);
	GT2005_write_cmos_sensor(0x010E , 0x00);
	GT2005_write_cmos_sensor(0x010F , 0x08);
	GT2005_write_cmos_sensor(0x0110 , 0x06);
	GT2005_write_cmos_sensor(0x0111 , 0x40);
	GT2005_write_cmos_sensor(0x0112 , 0x04);
	GT2005_write_cmos_sensor(0x0113 , 0xB0);

	//YUV Mode
	GT2005_write_cmos_sensor(0x0114 , 0x00);

	//Picture Effect
	GT2005_write_cmos_sensor(0x0115 , 0x00);

	//PLL&Frame Rate
	GT2005_write_cmos_sensor(0x0116 , 0x02);
	GT2005_write_cmos_sensor(0x0117 , 0x00);
	GT2005_write_cmos_sensor(0x0118 , 0x67);
	GT2005_write_cmos_sensor(0x0119 , 0x02);
	GT2005_write_cmos_sensor(0x011A , 0x04);
	GT2005_write_cmos_sensor(0x011B , 0x01);

	//DCLK Polarity
	GT2005_write_cmos_sensor(0x011C , 0x00);

	//Do not change
	GT2005_write_cmos_sensor(0x011D , 0x02);
	GT2005_write_cmos_sensor(0x011E , 0x00);
	
	GT2005_write_cmos_sensor(0x011F , 0x00);
	GT2005_write_cmos_sensor(0x0120 , 0x1C);
	GT2005_write_cmos_sensor(0x0121 , 0x00);
	GT2005_write_cmos_sensor(0x0122 , 0x04);
	GT2005_write_cmos_sensor(0x0123 , 0x00);
	GT2005_write_cmos_sensor(0x0124 , 0x00);
	GT2005_write_cmos_sensor(0x0125 , 0x00);
	GT2005_write_cmos_sensor(0x0126 , 0x00);
	GT2005_write_cmos_sensor(0x0127 , 0x00);
	GT2005_write_cmos_sensor(0x0128 , 0x00);

	//Contrast
	GT2005_write_cmos_sensor(0x0200 , 0x00);

	//Brightness
	GT2005_write_cmos_sensor(0x0201 , 0x00);

	//Saturation
	GT2005_write_cmos_sensor(0x0202 , 0x40);

	//Do not change
	GT2005_write_cmos_sensor(0x0203 , 0x00);
	GT2005_write_cmos_sensor(0x0204 , 0x03);
	GT2005_write_cmos_sensor(0x0205 , 0x1F);
	GT2005_write_cmos_sensor(0x0206 , 0x0B);
	GT2005_write_cmos_sensor(0x0207 , 0x20);
	GT2005_write_cmos_sensor(0x0208 , 0x00);
	GT2005_write_cmos_sensor(0x0209 , 0x2A);
	GT2005_write_cmos_sensor(0x020A , 0x01);

	//Sharpness
	GT2005_write_cmos_sensor(0x020B , 0x48);
	GT2005_write_cmos_sensor(0x020C , 0x64);

	//Do not change
	GT2005_write_cmos_sensor(0x020D , 0xC8);
	GT2005_write_cmos_sensor(0x020E , 0xBC);
	GT2005_write_cmos_sensor(0x020F , 0x08);
	GT2005_write_cmos_sensor(0x0210 , 0xD6);
	GT2005_write_cmos_sensor(0x0211 , 0x00);
	GT2005_write_cmos_sensor(0x0212 , 0x20);
	GT2005_write_cmos_sensor(0x0213 , 0x81);
	GT2005_write_cmos_sensor(0x0214 , 0x15);
	GT2005_write_cmos_sensor(0x0215 , 0x00);
	GT2005_write_cmos_sensor(0x0216 , 0x00);
	GT2005_write_cmos_sensor(0x0217 , 0x00);
	GT2005_write_cmos_sensor(0x0218 , 0x46);
	GT2005_write_cmos_sensor(0x0219 , 0x30);
	GT2005_write_cmos_sensor(0x021A , 0x03);
	GT2005_write_cmos_sensor(0x021B , 0x28);
	GT2005_write_cmos_sensor(0x021C , 0x02);
	GT2005_write_cmos_sensor(0x021D , 0x60);
	GT2005_write_cmos_sensor(0x021E , 0x00);
	GT2005_write_cmos_sensor(0x021F , 0x00);
	GT2005_write_cmos_sensor(0x0220 , 0x08);
	GT2005_write_cmos_sensor(0x0221 , 0x08);
	GT2005_write_cmos_sensor(0x0222 , 0x04);
	GT2005_write_cmos_sensor(0x0223 , 0x00);
	GT2005_write_cmos_sensor(0x0224 , 0x1F);
	GT2005_write_cmos_sensor(0x0225 , 0x1E);
	GT2005_write_cmos_sensor(0x0226 , 0x18);
	GT2005_write_cmos_sensor(0x0227 , 0x1D);
	GT2005_write_cmos_sensor(0x0228 , 0x1F);
	GT2005_write_cmos_sensor(0x0229 , 0x1F);
	GT2005_write_cmos_sensor(0x022A , 0x01);
	GT2005_write_cmos_sensor(0x022B , 0x04);
	GT2005_write_cmos_sensor(0x022C , 0x05);
	GT2005_write_cmos_sensor(0x022D , 0x05);
	GT2005_write_cmos_sensor(0x022E , 0x04);
	GT2005_write_cmos_sensor(0x022F , 0x03);
	GT2005_write_cmos_sensor(0x0230 , 0x02);
	GT2005_write_cmos_sensor(0x0231 , 0x1F);
	GT2005_write_cmos_sensor(0x0232 , 0x1A);
	GT2005_write_cmos_sensor(0x0233 , 0x19);
	GT2005_write_cmos_sensor(0x0234 , 0x19);
	GT2005_write_cmos_sensor(0x0235 , 0x1B);
	GT2005_write_cmos_sensor(0x0236 , 0x1F);
	GT2005_write_cmos_sensor(0x0237 , 0x04);
	GT2005_write_cmos_sensor(0x0238 , 0xEE);
	GT2005_write_cmos_sensor(0x0239 , 0xFF);
	GT2005_write_cmos_sensor(0x023A , 0x00);
	GT2005_write_cmos_sensor(0x023B , 0x00);
	GT2005_write_cmos_sensor(0x023C , 0x00);
	GT2005_write_cmos_sensor(0x023D , 0x00);
	GT2005_write_cmos_sensor(0x023E , 0x00);
	GT2005_write_cmos_sensor(0x023F , 0x00);
	GT2005_write_cmos_sensor(0x0240 , 0x00);
	GT2005_write_cmos_sensor(0x0241 , 0x00);
	GT2005_write_cmos_sensor(0x0242 , 0x00);
	GT2005_write_cmos_sensor(0x0243 , 0x21);
	GT2005_write_cmos_sensor(0x0244 , 0x42);
	GT2005_write_cmos_sensor(0x0245 , 0x53);
	GT2005_write_cmos_sensor(0x0246 , 0x54);
	GT2005_write_cmos_sensor(0x0247 , 0x54);
	GT2005_write_cmos_sensor(0x0248 , 0x54);
	GT2005_write_cmos_sensor(0x0249 , 0x33);
	GT2005_write_cmos_sensor(0x024A , 0x11);
	GT2005_write_cmos_sensor(0x024B , 0x00);
	GT2005_write_cmos_sensor(0x024C , 0x00);
	GT2005_write_cmos_sensor(0x024D , 0xFF);
	GT2005_write_cmos_sensor(0x024E , 0xEE);
	GT2005_write_cmos_sensor(0x024F , 0xDD);
	GT2005_write_cmos_sensor(0x0250 , 0x00);
	GT2005_write_cmos_sensor(0x0251 , 0x00);
	GT2005_write_cmos_sensor(0x0252 , 0x00);
	GT2005_write_cmos_sensor(0x0253 , 0x00);
	GT2005_write_cmos_sensor(0x0254 , 0x00);
	GT2005_write_cmos_sensor(0x0255 , 0x00);
	GT2005_write_cmos_sensor(0x0256 , 0x00);
	GT2005_write_cmos_sensor(0x0257 , 0x00);
	GT2005_write_cmos_sensor(0x0258 , 0x00);
	GT2005_write_cmos_sensor(0x0259 , 0x00);
	GT2005_write_cmos_sensor(0x025A , 0x00);
	GT2005_write_cmos_sensor(0x025B , 0x00);
	GT2005_write_cmos_sensor(0x025C , 0x00);
	GT2005_write_cmos_sensor(0x025D , 0x00);
	GT2005_write_cmos_sensor(0x025E , 0x00);
	GT2005_write_cmos_sensor(0x025F , 0x00);
	GT2005_write_cmos_sensor(0x0260 , 0x00);
	GT2005_write_cmos_sensor(0x0261 , 0x00);
	GT2005_write_cmos_sensor(0x0262 , 0x00);
	GT2005_write_cmos_sensor(0x0263 , 0x00);
	GT2005_write_cmos_sensor(0x0264 , 0x00);
	GT2005_write_cmos_sensor(0x0265 , 0x00);
	GT2005_write_cmos_sensor(0x0266 , 0x00);
	GT2005_write_cmos_sensor(0x0267 , 0x00);
	GT2005_write_cmos_sensor(0x0268 , 0x8F);
	GT2005_write_cmos_sensor(0x0269 , 0xA3);
	GT2005_write_cmos_sensor(0x026A , 0xB4);
	GT2005_write_cmos_sensor(0x026B , 0x90);
	GT2005_write_cmos_sensor(0x026C , 0x00);
	GT2005_write_cmos_sensor(0x026D , 0xD0);
	GT2005_write_cmos_sensor(0x026E , 0x60);
	GT2005_write_cmos_sensor(0x026F , 0xA0);
	GT2005_write_cmos_sensor(0x0270 , 0x40);
	GT2005_write_cmos_sensor(0x0300 , 0x81);
	GT2005_write_cmos_sensor(0x0301 , 0x80);
	GT2005_write_cmos_sensor(0x0302 , 0x22);
	GT2005_write_cmos_sensor(0x0303 , 0x06);
	GT2005_write_cmos_sensor(0x0304 , 0x03);
	GT2005_write_cmos_sensor(0x0305 , 0x83);
	GT2005_write_cmos_sensor(0x0306 , 0x00);
	GT2005_write_cmos_sensor(0x0307 , 0x22);
	GT2005_write_cmos_sensor(0x0308 , 0x00);
	GT2005_write_cmos_sensor(0x0309 , 0x55);
	GT2005_write_cmos_sensor(0x030A , 0x55);
	GT2005_write_cmos_sensor(0x030B , 0x55);
	GT2005_write_cmos_sensor(0x030C , 0x54);
	GT2005_write_cmos_sensor(0x030D , 0x1F);
	GT2005_write_cmos_sensor(0x030E , 0x0A);
	GT2005_write_cmos_sensor(0x030F , 0x10);
	GT2005_write_cmos_sensor(0x0310 , 0x04);
	GT2005_write_cmos_sensor(0x0311 , 0xFF);
	GT2005_write_cmos_sensor(0x0312 , 0x08);
	GT2005_write_cmos_sensor(0x0313 , 0x28);
	GT2005_write_cmos_sensor(0x0314 , 0x66);
	GT2005_write_cmos_sensor(0x0315 , 0x96);
	GT2005_write_cmos_sensor(0x0316 , 0x26);
	GT2005_write_cmos_sensor(0x0317 , 0x02);
	GT2005_write_cmos_sensor(0x0318 , 0x08);
	GT2005_write_cmos_sensor(0x0319 , 0x0C);

#ifndef A_LIGHT_CORRECTION
	//Normal AWB Setting
	GT2005_write_cmos_sensor(0x031A , 0x81);
	GT2005_write_cmos_sensor(0x031B , 0x00);
	GT2005_write_cmos_sensor(0x031C , 0x3D);
	GT2005_write_cmos_sensor(0x031D , 0x00);
	GT2005_write_cmos_sensor(0x031E , 0xF9);
	GT2005_write_cmos_sensor(0x031F , 0x00);
	GT2005_write_cmos_sensor(0x0320 , 0x24);
	GT2005_write_cmos_sensor(0x0321 , 0x14);
	GT2005_write_cmos_sensor(0x0322 , 0x1A);
	GT2005_write_cmos_sensor(0x0323 , 0x24);
	GT2005_write_cmos_sensor(0x0324 , 0x08);
	GT2005_write_cmos_sensor(0x0325 , 0xF0);
	GT2005_write_cmos_sensor(0x0326 , 0x30);
	GT2005_write_cmos_sensor(0x0327 , 0x17);
	GT2005_write_cmos_sensor(0x0328 , 0x11);
	GT2005_write_cmos_sensor(0x0329 , 0x22);
	GT2005_write_cmos_sensor(0x032A , 0x2F);
	GT2005_write_cmos_sensor(0x032B , 0x21);
	GT2005_write_cmos_sensor(0x032C , 0xDA);
	GT2005_write_cmos_sensor(0x032D , 0x10);
	GT2005_write_cmos_sensor(0x032E , 0xEA);
	GT2005_write_cmos_sensor(0x032F , 0x18);
	GT2005_write_cmos_sensor(0x0330 , 0x29);
	GT2005_write_cmos_sensor(0x0331 , 0x25);
	GT2005_write_cmos_sensor(0x0332 , 0x12);
	GT2005_write_cmos_sensor(0x0333 , 0x0F);
	GT2005_write_cmos_sensor(0x0334 , 0xE0);
	GT2005_write_cmos_sensor(0x0335 , 0x13);
	GT2005_write_cmos_sensor(0x0336 , 0xFF);
	GT2005_write_cmos_sensor(0x0337 , 0x20);
	GT2005_write_cmos_sensor(0x0338 , 0x46);
	GT2005_write_cmos_sensor(0x0339 , 0x04);
	GT2005_write_cmos_sensor(0x033A , 0x04);
	GT2005_write_cmos_sensor(0x033B , 0xFF);
	GT2005_write_cmos_sensor(0x033C , 0x01);
	GT2005_write_cmos_sensor(0x033D , 0x00);

#else
	//A LIGHT CORRECTION
	GT2005_write_cmos_sensor(0x031A , 0x81);
	GT2005_write_cmos_sensor(0x031B , 0x00);
	GT2005_write_cmos_sensor(0x031C , 0x1D);
	GT2005_write_cmos_sensor(0x031D , 0x00);
	GT2005_write_cmos_sensor(0x031E , 0xFD);
	GT2005_write_cmos_sensor(0x031F , 0x00);
	GT2005_write_cmos_sensor(0x0320 , 0xE1);
	GT2005_write_cmos_sensor(0x0321 , 0x1A);
	GT2005_write_cmos_sensor(0x0322 , 0xDE);
	GT2005_write_cmos_sensor(0x0323 , 0x11);
	GT2005_write_cmos_sensor(0x0324 , 0x1A);
	GT2005_write_cmos_sensor(0x0325 , 0xEE);
	GT2005_write_cmos_sensor(0x0326 , 0x50);
	GT2005_write_cmos_sensor(0x0327 , 0x18);
	GT2005_write_cmos_sensor(0x0328 , 0x25);
	GT2005_write_cmos_sensor(0x0329 , 0x37);
	GT2005_write_cmos_sensor(0x032A , 0x24);
	GT2005_write_cmos_sensor(0x032B , 0x32);
	GT2005_write_cmos_sensor(0x032C , 0xA9);
	GT2005_write_cmos_sensor(0x032D , 0x32);
	GT2005_write_cmos_sensor(0x032E , 0xFF);
	GT2005_write_cmos_sensor(0x032F , 0x7F);
	GT2005_write_cmos_sensor(0x0330 , 0xBA);
	GT2005_write_cmos_sensor(0x0331 , 0x7F);
	GT2005_write_cmos_sensor(0x0332 , 0x7F);
	GT2005_write_cmos_sensor(0x0333 , 0x14);
	GT2005_write_cmos_sensor(0x0334 , 0x81);
	GT2005_write_cmos_sensor(0x0335 , 0x14);
	GT2005_write_cmos_sensor(0x0336 , 0xFF);
	GT2005_write_cmos_sensor(0x0337 , 0x20);
	GT2005_write_cmos_sensor(0x0338 , 0x46);
	GT2005_write_cmos_sensor(0x0339 , 0x04);
	GT2005_write_cmos_sensor(0x033A , 0x04);
	GT2005_write_cmos_sensor(0x033B , 0x00);
	GT2005_write_cmos_sensor(0x033C , 0x00);
	GT2005_write_cmos_sensor(0x033D , 0x00);
#endif

	//Do not change
	GT2005_write_cmos_sensor(0x033E , 0x03);
	GT2005_write_cmos_sensor(0x033F , 0x28);
	GT2005_write_cmos_sensor(0x0340 , 0x02);
	GT2005_write_cmos_sensor(0x0341 , 0x60);
	GT2005_write_cmos_sensor(0x0342 , 0xAC);
	GT2005_write_cmos_sensor(0x0343 , 0x97);
	GT2005_write_cmos_sensor(0x0344 , 0x7F);
	GT2005_write_cmos_sensor(0x0400 , 0xE8);
	GT2005_write_cmos_sensor(0x0401 , 0x40);
	GT2005_write_cmos_sensor(0x0402 , 0x00);
	GT2005_write_cmos_sensor(0x0403 , 0x00);
	GT2005_write_cmos_sensor(0x0404 , 0xF8);
	GT2005_write_cmos_sensor(0x0405 , 0x03);
	GT2005_write_cmos_sensor(0x0406 , 0x03);
	GT2005_write_cmos_sensor(0x0407 , 0x85);
	GT2005_write_cmos_sensor(0x0408 , 0x44);
	GT2005_write_cmos_sensor(0x0409 , 0x1F);
	GT2005_write_cmos_sensor(0x040A , 0x40);
	GT2005_write_cmos_sensor(0x040B , 0x33);

	//Lens Shading Correction
	GT2005_write_cmos_sensor(0x040C , 0xA0);
	GT2005_write_cmos_sensor(0x040D , 0x00);
	GT2005_write_cmos_sensor(0x040E , 0x00);
	GT2005_write_cmos_sensor(0x040F , 0x00);
	GT2005_write_cmos_sensor(0x0410 , 0x0D);
	GT2005_write_cmos_sensor(0x0411 , 0x0D);
	GT2005_write_cmos_sensor(0x0412 , 0x0C);
	GT2005_write_cmos_sensor(0x0413 , 0x04);
	GT2005_write_cmos_sensor(0x0414 , 0x00);
	GT2005_write_cmos_sensor(0x0415 , 0x00);
	GT2005_write_cmos_sensor(0x0416 , 0x07);
	GT2005_write_cmos_sensor(0x0417 , 0x09);
	GT2005_write_cmos_sensor(0x0418 , 0x16);
	GT2005_write_cmos_sensor(0x0419 , 0x14);
	GT2005_write_cmos_sensor(0x041A , 0x11);
	GT2005_write_cmos_sensor(0x041B , 0x14);
	GT2005_write_cmos_sensor(0x041C , 0x07);
	GT2005_write_cmos_sensor(0x041D , 0x07);
	GT2005_write_cmos_sensor(0x041E , 0x06);
	GT2005_write_cmos_sensor(0x041F , 0x02);
	GT2005_write_cmos_sensor(0x0420 , 0x42);
	GT2005_write_cmos_sensor(0x0421 , 0x42);
	GT2005_write_cmos_sensor(0x0422 , 0x47);
	GT2005_write_cmos_sensor(0x0423 , 0x39);
	GT2005_write_cmos_sensor(0x0424 , 0x3E);
	GT2005_write_cmos_sensor(0x0425 , 0x4D);
	GT2005_write_cmos_sensor(0x0426 , 0x46);
	GT2005_write_cmos_sensor(0x0427 , 0x3A);
	GT2005_write_cmos_sensor(0x0428 , 0x21);
	GT2005_write_cmos_sensor(0x0429 , 0x21);
	GT2005_write_cmos_sensor(0x042A , 0x26);
	GT2005_write_cmos_sensor(0x042B , 0x1C);
	GT2005_write_cmos_sensor(0x042C , 0x25);
	GT2005_write_cmos_sensor(0x042D , 0x25);
	GT2005_write_cmos_sensor(0x042E , 0x28);
	GT2005_write_cmos_sensor(0x042F , 0x20);
	GT2005_write_cmos_sensor(0x0430 , 0x3E);
	GT2005_write_cmos_sensor(0x0431 , 0x3E);
	GT2005_write_cmos_sensor(0x0432 , 0x33);
	GT2005_write_cmos_sensor(0x0433 , 0x2E);
	GT2005_write_cmos_sensor(0x0434 , 0x54);
	GT2005_write_cmos_sensor(0x0435 , 0x53);
	GT2005_write_cmos_sensor(0x0436 , 0x3C);
	GT2005_write_cmos_sensor(0x0437 , 0x51);
	GT2005_write_cmos_sensor(0x0438 , 0x2B);
	GT2005_write_cmos_sensor(0x0439 , 0x2B);
	GT2005_write_cmos_sensor(0x043A , 0x38);
	GT2005_write_cmos_sensor(0x043B , 0x22);
	GT2005_write_cmos_sensor(0x043C , 0x3B);
	GT2005_write_cmos_sensor(0x043D , 0x3B);
	GT2005_write_cmos_sensor(0x043E , 0x31);
	GT2005_write_cmos_sensor(0x043F , 0x37);

	//PWB Gain
	GT2005_write_cmos_sensor(0x0440 , 0x00);
	GT2005_write_cmos_sensor(0x0441 , 0x4B);
	GT2005_write_cmos_sensor(0x0442 , 0x00);
	GT2005_write_cmos_sensor(0x0443 , 0x00);
	GT2005_write_cmos_sensor(0x0444 , 0x31);

	GT2005_write_cmos_sensor(0x0445 , 0x00);
	GT2005_write_cmos_sensor(0x0446 , 0x00);
	GT2005_write_cmos_sensor(0x0447 , 0x00);
	GT2005_write_cmos_sensor(0x0448 , 0x00);
	GT2005_write_cmos_sensor(0x0449 , 0x00);
	GT2005_write_cmos_sensor(0x044A , 0x00);
	GT2005_write_cmos_sensor(0x044D , 0xE0);
	GT2005_write_cmos_sensor(0x044E , 0x05);
	GT2005_write_cmos_sensor(0x044F , 0x07);
	GT2005_write_cmos_sensor(0x0450 , 0x00);
	GT2005_write_cmos_sensor(0x0451 , 0x00);
	GT2005_write_cmos_sensor(0x0452 , 0x00);
	GT2005_write_cmos_sensor(0x0453 , 0x00);
	GT2005_write_cmos_sensor(0x0454 , 0x00);
	GT2005_write_cmos_sensor(0x0455 , 0x00);
	GT2005_write_cmos_sensor(0x0456 , 0x00);
	GT2005_write_cmos_sensor(0x0457 , 0x00);
	GT2005_write_cmos_sensor(0x0458 , 0x00);
	GT2005_write_cmos_sensor(0x0459 , 0x00);
	GT2005_write_cmos_sensor(0x045A , 0x00);
	GT2005_write_cmos_sensor(0x045B , 0x00);
	GT2005_write_cmos_sensor(0x045C , 0x00);
	GT2005_write_cmos_sensor(0x045D , 0x00);
	GT2005_write_cmos_sensor(0x045E , 0x00);
	GT2005_write_cmos_sensor(0x045F , 0x00);

	//GAMMA Correction
	GT2005_write_cmos_sensor(0x0460 , 0x80);
	GT2005_write_cmos_sensor(0x0461 , 0x10);
	GT2005_write_cmos_sensor(0x0462 , 0x10);
	GT2005_write_cmos_sensor(0x0463 , 0x10);
	GT2005_write_cmos_sensor(0x0464 , 0x08);
	GT2005_write_cmos_sensor(0x0465 , 0x08);
	GT2005_write_cmos_sensor(0x0466 , 0x11);
	GT2005_write_cmos_sensor(0x0467 , 0x09);
	GT2005_write_cmos_sensor(0x0468 , 0x23);
	GT2005_write_cmos_sensor(0x0469 , 0x2A);
	GT2005_write_cmos_sensor(0x046A , 0x2A);
	GT2005_write_cmos_sensor(0x046B , 0x47);
	GT2005_write_cmos_sensor(0x046C , 0x52);
	GT2005_write_cmos_sensor(0x046D , 0x42);
	GT2005_write_cmos_sensor(0x046E , 0x36);
	GT2005_write_cmos_sensor(0x046F , 0x46);
	GT2005_write_cmos_sensor(0x0470 , 0x3A);
	GT2005_write_cmos_sensor(0x0471 , 0x32);
	GT2005_write_cmos_sensor(0x0472 , 0x32);
	GT2005_write_cmos_sensor(0x0473 , 0x38);
	GT2005_write_cmos_sensor(0x0474 , 0x3D);
	GT2005_write_cmos_sensor(0x0475 , 0x2F);
	GT2005_write_cmos_sensor(0x0476 , 0x29);
	GT2005_write_cmos_sensor(0x0477 , 0x48);

	//Do not change
	GT2005_write_cmos_sensor(0x0600 , 0x00);
	GT2005_write_cmos_sensor(0x0601 , 0x24);
	GT2005_write_cmos_sensor(0x0602 , 0x45);
	GT2005_write_cmos_sensor(0x0603 , 0x0E);
	GT2005_write_cmos_sensor(0x0604 , 0x14);
	GT2005_write_cmos_sensor(0x0605 , 0x2F);
	GT2005_write_cmos_sensor(0x0606 , 0x01);
	GT2005_write_cmos_sensor(0x0607 , 0x0E);
	GT2005_write_cmos_sensor(0x0608 , 0x0E);
	GT2005_write_cmos_sensor(0x0609 , 0x37);
	GT2005_write_cmos_sensor(0x060A , 0x18);
	GT2005_write_cmos_sensor(0x060B , 0xA0);
	GT2005_write_cmos_sensor(0x060C , 0x20);
	GT2005_write_cmos_sensor(0x060D , 0x07);
	GT2005_write_cmos_sensor(0x060E , 0x47);
	GT2005_write_cmos_sensor(0x060F , 0x90);
	GT2005_write_cmos_sensor(0x0610 , 0x06);
	GT2005_write_cmos_sensor(0x0611 , 0x0C);
	GT2005_write_cmos_sensor(0x0612 , 0x28);
	GT2005_write_cmos_sensor(0x0613 , 0x13);
	GT2005_write_cmos_sensor(0x0614 , 0x0B);
	GT2005_write_cmos_sensor(0x0615 , 0x10);
	GT2005_write_cmos_sensor(0x0616 , 0x14);
	GT2005_write_cmos_sensor(0x0617 , 0x19);
	GT2005_write_cmos_sensor(0x0618 , 0x52);
	GT2005_write_cmos_sensor(0x0619 , 0xA0);
	GT2005_write_cmos_sensor(0x061A , 0x11);
	GT2005_write_cmos_sensor(0x061B , 0x33);
	GT2005_write_cmos_sensor(0x061C , 0x56);
	GT2005_write_cmos_sensor(0x061D , 0x20);
	GT2005_write_cmos_sensor(0x061E , 0x28);
	GT2005_write_cmos_sensor(0x061F , 0x2B);
	GT2005_write_cmos_sensor(0x0620 , 0x22);
	GT2005_write_cmos_sensor(0x0621 , 0x11);
	GT2005_write_cmos_sensor(0x0622 , 0x75);
	GT2005_write_cmos_sensor(0x0623 , 0x49);
	GT2005_write_cmos_sensor(0x0624 , 0x6E);
	GT2005_write_cmos_sensor(0x0625 , 0x80);
	GT2005_write_cmos_sensor(0x0626 , 0x02);
	GT2005_write_cmos_sensor(0x0627 , 0x0C);
	GT2005_write_cmos_sensor(0x0628 , 0x51);
	GT2005_write_cmos_sensor(0x0629 , 0x25);
	GT2005_write_cmos_sensor(0x062A , 0x01);
	GT2005_write_cmos_sensor(0x062B , 0x3D);
	GT2005_write_cmos_sensor(0x062C , 0x04);
	GT2005_write_cmos_sensor(0x062D , 0x01);
	GT2005_write_cmos_sensor(0x062E , 0x0C);
	GT2005_write_cmos_sensor(0x062F , 0x2C);
	GT2005_write_cmos_sensor(0x0630 , 0x0D);
	GT2005_write_cmos_sensor(0x0631 , 0x14);
	GT2005_write_cmos_sensor(0x0632 , 0x12);
	GT2005_write_cmos_sensor(0x0633 , 0x34);
	GT2005_write_cmos_sensor(0x0634 , 0x00);
	GT2005_write_cmos_sensor(0x0635 , 0x00);
	GT2005_write_cmos_sensor(0x0636 , 0x00);
	GT2005_write_cmos_sensor(0x0637 , 0xB1);
	GT2005_write_cmos_sensor(0x0638 , 0x22);
	GT2005_write_cmos_sensor(0x0639 , 0x32);
	GT2005_write_cmos_sensor(0x063A , 0x0E);
	GT2005_write_cmos_sensor(0x063B , 0x18);
	GT2005_write_cmos_sensor(0x063C , 0x88);
	GT2005_write_cmos_sensor(0x0640 , 0xB2);
	GT2005_write_cmos_sensor(0x0641 , 0xC0);
	GT2005_write_cmos_sensor(0x0642 , 0x01);
	GT2005_write_cmos_sensor(0x0643 , 0x26);
	GT2005_write_cmos_sensor(0x0644 , 0x13);
	GT2005_write_cmos_sensor(0x0645 , 0x88);
	GT2005_write_cmos_sensor(0x0646 , 0x64);
	GT2005_write_cmos_sensor(0x0647 , 0x00);
	GT2005_write_cmos_sensor(0x0681 , 0x1B);
	GT2005_write_cmos_sensor(0x0682 , 0xA0);
	GT2005_write_cmos_sensor(0x0683 , 0x28);
	GT2005_write_cmos_sensor(0x0684 , 0x00);
	GT2005_write_cmos_sensor(0x0685 , 0xB0);
	GT2005_write_cmos_sensor(0x0686 , 0x6F);
	GT2005_write_cmos_sensor(0x0687 , 0x33);
	GT2005_write_cmos_sensor(0x0688 , 0x1F);
	GT2005_write_cmos_sensor(0x0689 , 0x44);
	GT2005_write_cmos_sensor(0x068A , 0xA8);
	GT2005_write_cmos_sensor(0x068B , 0x44);
	GT2005_write_cmos_sensor(0x068C , 0x08);
	GT2005_write_cmos_sensor(0x068D , 0x08);
	GT2005_write_cmos_sensor(0x068E , 0x00);
	GT2005_write_cmos_sensor(0x068F , 0x00);
	GT2005_write_cmos_sensor(0x0690 , 0x01);
	GT2005_write_cmos_sensor(0x0691 , 0x00);
	GT2005_write_cmos_sensor(0x0692 , 0x01);
	GT2005_write_cmos_sensor(0x0693 , 0x00);
	GT2005_write_cmos_sensor(0x0694 , 0x00);
	GT2005_write_cmos_sensor(0x0695 , 0x00);
	GT2005_write_cmos_sensor(0x0696 , 0x00);
	GT2005_write_cmos_sensor(0x0697 , 0x00);
	GT2005_write_cmos_sensor(0x0698 , 0x2A);
	GT2005_write_cmos_sensor(0x0699 , 0x80);
	GT2005_write_cmos_sensor(0x069A , 0x1F);
	GT2005_write_cmos_sensor(0x069B , 0x00);
	GT2005_write_cmos_sensor(0x069C , 0x02);
	GT2005_write_cmos_sensor(0x069D , 0xF5);
	GT2005_write_cmos_sensor(0x069E , 0x03);
	GT2005_write_cmos_sensor(0x069F , 0x6D);
	GT2005_write_cmos_sensor(0x06A0 , 0x0C);
	GT2005_write_cmos_sensor(0x06A1 , 0xB8);
	GT2005_write_cmos_sensor(0x06A2 , 0x0D);
	GT2005_write_cmos_sensor(0x06A3 , 0x74);
	GT2005_write_cmos_sensor(0x06A4 , 0x00);
	GT2005_write_cmos_sensor(0x06A5 , 0x2F);
	GT2005_write_cmos_sensor(0x06A6 , 0x00);
	GT2005_write_cmos_sensor(0x06A7 , 0x2F);
	GT2005_write_cmos_sensor(0x0F00 , 0x00);
	GT2005_write_cmos_sensor(0x0F01 , 0x00);

	//Output Enable
	GT2005_write_cmos_sensor(0x0100 , 0x01);
	GT2005_write_cmos_sensor(0x0102 , 0x02);
	GT2005_write_cmos_sensor(0x0104 , 0x03);

	dprintk("%s End!! \n", __FUNCTION__);
 
} /* GT2005_YUV_sensor_initial_setting */


void GT2005_GAMMA_Select(unsigned char level)
{
	switch(level)
	{
		case 1: 							//smallest gamma curve
			GT2005_write_cmos_sensor(0x0461 , 0x00);
			GT2005_write_cmos_sensor(0x0462 , 0x00);
			GT2005_write_cmos_sensor(0x0463 , 0x00);
			GT2005_write_cmos_sensor(0x0464 , 0x00);
			GT2005_write_cmos_sensor(0x0465 , 0x00);
			GT2005_write_cmos_sensor(0x0466 , 0x12);
			GT2005_write_cmos_sensor(0x0467 , 0x3B);
			GT2005_write_cmos_sensor(0x0468 , 0x34);
			GT2005_write_cmos_sensor(0x0469 , 0x26);
			GT2005_write_cmos_sensor(0x046A , 0x1E);
			GT2005_write_cmos_sensor(0x046B , 0x33);
			GT2005_write_cmos_sensor(0x046C , 0x2E);
			GT2005_write_cmos_sensor(0x046D , 0x2C);
			GT2005_write_cmos_sensor(0x046E , 0x28);
			GT2005_write_cmos_sensor(0x046F , 0x42);
			GT2005_write_cmos_sensor(0x0470 , 0x42);
			GT2005_write_cmos_sensor(0x0471 , 0x38);
			GT2005_write_cmos_sensor(0x0472 , 0x37);
			GT2005_write_cmos_sensor(0x0473 , 0x4D);
			GT2005_write_cmos_sensor(0x0474 , 0x48);
			GT2005_write_cmos_sensor(0x0475 , 0x44);
			GT2005_write_cmos_sensor(0x0476 , 0x40);
			GT2005_write_cmos_sensor(0x0477 , 0x56);
			break;
                 
		case 2:
			GT2005_write_cmos_sensor(0x0461 , 0x00);
			GT2005_write_cmos_sensor(0x0462 , 0x00);
			GT2005_write_cmos_sensor(0x0463 , 0x00);
			GT2005_write_cmos_sensor(0x0464 , 0x00);
			GT2005_write_cmos_sensor(0x0465 , 0x00);
			GT2005_write_cmos_sensor(0x0466 , 0x29);
			GT2005_write_cmos_sensor(0x0467 , 0x37);
			GT2005_write_cmos_sensor(0x0468 , 0x3A);
			GT2005_write_cmos_sensor(0x0469 , 0x26);
			GT2005_write_cmos_sensor(0x046A , 0x21);
			GT2005_write_cmos_sensor(0x046B , 0x34);
			GT2005_write_cmos_sensor(0x046C , 0x34);
			GT2005_write_cmos_sensor(0x046D , 0x2B);
			GT2005_write_cmos_sensor(0x046E , 0x28);
			GT2005_write_cmos_sensor(0x046F , 0x41);
			GT2005_write_cmos_sensor(0x0470 , 0x3F);
			GT2005_write_cmos_sensor(0x0471 , 0x3A);
			GT2005_write_cmos_sensor(0x0472 , 0x36);
			GT2005_write_cmos_sensor(0x0473 , 0x47);
			GT2005_write_cmos_sensor(0x0474 , 0x44);
			GT2005_write_cmos_sensor(0x0475 , 0x3B);
			GT2005_write_cmos_sensor(0x0476 , 0x3B);
			GT2005_write_cmos_sensor(0x0477 , 0x4D);
			break;                  
			
		case 3:            
			GT2005_write_cmos_sensor(0x0461 , 0x00);
			GT2005_write_cmos_sensor(0x0462 , 0x00);
			GT2005_write_cmos_sensor(0x0463 , 0x00);
			GT2005_write_cmos_sensor(0x0464 , 0x00);
			GT2005_write_cmos_sensor(0x0465 , 0x00);
			GT2005_write_cmos_sensor(0x0466 , 0x29);
			GT2005_write_cmos_sensor(0x0467 , 0x4B);
			GT2005_write_cmos_sensor(0x0468 , 0x41);
			GT2005_write_cmos_sensor(0x0469 , 0x2A);
			GT2005_write_cmos_sensor(0x046A , 0x25);
			GT2005_write_cmos_sensor(0x046B , 0x3A);
			GT2005_write_cmos_sensor(0x046C , 0x2C);
			GT2005_write_cmos_sensor(0x046D , 0x2B);
			GT2005_write_cmos_sensor(0x046E , 0x28);
			GT2005_write_cmos_sensor(0x046F , 0x40);
			GT2005_write_cmos_sensor(0x0470 , 0x3D);
			GT2005_write_cmos_sensor(0x0471 , 0x38);
			GT2005_write_cmos_sensor(0x0472 , 0x31);
			GT2005_write_cmos_sensor(0x0473 , 0x44);
			GT2005_write_cmos_sensor(0x0474 , 0x3E);
			GT2005_write_cmos_sensor(0x0475 , 0x3E);
			GT2005_write_cmos_sensor(0x0476 , 0x37);
			GT2005_write_cmos_sensor(0x0477 , 0x43);
			break;                 
			
		case 4:	    
			GT2005_write_cmos_sensor(0x0461 , 0x00);
			GT2005_write_cmos_sensor(0x0462 , 0x00);
			GT2005_write_cmos_sensor(0x0463 , 0x00);
			GT2005_write_cmos_sensor(0x0464 , 0x00);
			GT2005_write_cmos_sensor(0x0465 , 0x00);
			GT2005_write_cmos_sensor(0x0466 , 0x2F);
			GT2005_write_cmos_sensor(0x0467 , 0x4E);
			GT2005_write_cmos_sensor(0x0468 , 0x50);
			GT2005_write_cmos_sensor(0x0469 , 0x31);
			GT2005_write_cmos_sensor(0x046A , 0x27);
			GT2005_write_cmos_sensor(0x046B , 0x3C);
			GT2005_write_cmos_sensor(0x046C , 0x35);
			GT2005_write_cmos_sensor(0x046D , 0x27);
			GT2005_write_cmos_sensor(0x046E , 0x23);
			GT2005_write_cmos_sensor(0x046F , 0x46);
			GT2005_write_cmos_sensor(0x0470 , 0x3A);
			GT2005_write_cmos_sensor(0x0471 , 0x32);
			GT2005_write_cmos_sensor(0x0472 , 0x32);
			GT2005_write_cmos_sensor(0x0473 , 0x38);
			GT2005_write_cmos_sensor(0x0474 , 0x3E);
			GT2005_write_cmos_sensor(0x0475 , 0x36);
			GT2005_write_cmos_sensor(0x0476 , 0x33);
			GT2005_write_cmos_sensor(0x0477 , 0x41);
			break;

		case 5:							// largest gamma curve
			GT2005_write_cmos_sensor(0x0461 , 0x00);
			GT2005_write_cmos_sensor(0x0462 , 0x00);
			GT2005_write_cmos_sensor(0x0463 , 0x00);
			GT2005_write_cmos_sensor(0x0464 , 0x00);
			GT2005_write_cmos_sensor(0x0465 , 0x15);
			GT2005_write_cmos_sensor(0x0466 , 0x33);
			GT2005_write_cmos_sensor(0x0467 , 0x61);
			GT2005_write_cmos_sensor(0x0468 , 0x56);
			GT2005_write_cmos_sensor(0x0469 , 0x30);
			GT2005_write_cmos_sensor(0x046A , 0x22);
			GT2005_write_cmos_sensor(0x046B , 0x3E);
			GT2005_write_cmos_sensor(0x046C , 0x2E);
			GT2005_write_cmos_sensor(0x046D , 0x2B);
			GT2005_write_cmos_sensor(0x046E , 0x28);
			GT2005_write_cmos_sensor(0x046F , 0x3C);
			GT2005_write_cmos_sensor(0x0470 , 0x38);
			GT2005_write_cmos_sensor(0x0471 , 0x2F);
			GT2005_write_cmos_sensor(0x0472 , 0x2A);
			GT2005_write_cmos_sensor(0x0473 , 0x3C);
			GT2005_write_cmos_sensor(0x0474 , 0x34);
			GT2005_write_cmos_sensor(0x0475 , 0x31);
			GT2005_write_cmos_sensor(0x0476 , 0x31);
			GT2005_write_cmos_sensor(0x0477 , 0x39);
			break;

		default:
			break;
	}
}


void GT2005_H_V_Switch(unsigned char direction)
{
	switch(direction) 
	{
		case 1:  // Normal
	    		GT2005_write_cmos_sensor(0x0101 , 0x00);
		  	break;
		case 2:  // IMAGE_H_MIRROR
	    		GT2005_write_cmos_sensor(0x0101 , 0x01);
		  	break;		  
		case 3:  // IMAGE_V_MIRROR
	    		GT2005_write_cmos_sensor(0x0101 , 0x02);
  	  		break;	
		case 4:  // IMAGE_HV_MIRROR
	   		 GT2005_write_cmos_sensor(0x0101 , 0x03);
		 	break;		    
	} 
}


static void GT2005_write_more_registers(void)
{

	GT2005_GAMMA_Select(0);
	GT2005_H_V_Switch(1);

}


/*************************************************************************
* FUNCTION
*	GT2005_Init
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor and ISP control register.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned char GT2005_Init(void)
{
    volatile unsigned int i;
    unsigned short sensor_id = 0;
	gc_effect_on = FALSE;
    
    /*	cis_module_power_on(KAL_TRUE);      // Power On CIS Power
    	kal_sleep_task(2);					     // To wait for Stable Power
    	GPIO_ModeSetup(SCCB_SERIAL_CLK_PIN,0);
	GPIO_ModeSetup(SCCB_SERIAL_DATA_PIN,0);		
	GPIO_WriteIO(1, SCCB_SERIAL_CLK_PIN);		
 	GPIO_WriteIO(1, SCCB_SERIAL_DATA_PIN);		      
	GPIO_InitIO(OUTPUT, SCCB_SERIAL_CLK_PIN);		
	GPIO_InitIO(OUTPUT, SCCB_SERIAL_DATA_PIN);	
    	kal_sleep_task(2);

	PWRDN_PIN_HIGH;	//Must Pulled To High For Normal Work

	SET_RESET_CMOS_SENSOR_HIGH;
	kal_sleep_task(2);	
	SET_RESET_CMOS_SENSOR_LOW;
	kal_sleep_task(2);
	SET_RESET_CMOS_SENSOR_HIGH;

   
    	SET_CMOS_CLOCK_POLARITY_LOW;
    	SET_VSYNC_POLARITY_HIGH;
    	SET_HSYNC_POLARITY_LOW;
 
	ENABLE_CAMERA_INDATA_FORMAT;
	SET_CAMERA_INPUT_TYPE(INPUT_YUV422);
	SET_CAMERA_INPUT_ORDER(INPUT_ORDER_YCrY1Cb);		
 
    	ENABLE_CAMERA_TG_CLK_48M;
    	UPLL_Enable(UPLL_OWNER_ISP);    
	set_isp_driving_current(ISP_DRIVING_8MA);

    	SET_TG_OUTPUT_CLK_DIVIDER(1);	
    	SET_CMOS_RISING_EDGE(0);
    	SET_CMOS_FALLING_EDGE(1);
    	ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;
    	SET_TG_PIXEL_CLK_DIVIDER(1);
    	SET_CMOS_DATA_LATCH(1);	*/

   	sensor_id = (GT2005_read_cmos_sensor(0x0000) << 8) | GT2005_read_cmos_sensor(0x0001);

   	if (sensor_id != GT2005_SENSOR_ID )
	{
		printk("[GT2005_2mp] TCC_HW_I2C Read Fail!!");
		return -1;
   	}
   
   	GT2005_YUV_sensor_initial_setting();

	GT2005_write_more_registers();
	
    return 1;
}	/* GT2005_Init */



/*************************************************************************
* FUNCTION
*	GT2005_PowerOff
*
* DESCRIPTION
*	This function is to turn off sensor module power.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GT2005_PowerOff(void)
{
	#if 0
	cis_module_power_on(KAL_FALSE);      // Power Off CIS Power
	UPLL_Disable(UPLL_OWNER_ISP);
	#ifndef HW_SCCB
	   	SET_SCCB_CLK_LOW;
	  	SET_SCCB_DATA_LOW;
	#endif
	#endif
}	/* GT2005_PowerOff */



/*************************************************************************
* FUNCTION
*	GT2005_GetID
*
* DESCRIPTION
*	This function return the sensor read/write id of SCCB interface.
*
* PARAMETERS
*	*sensor_write_id : address pointer of sensor write id
*  *sensor_read_id  : address pointer of sensor read id
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GT2005_GetID(unsigned char *sensor_write_id, unsigned char *sensor_read_id)
{
	*sensor_write_id=GT2005_WRITE_ID;
	*sensor_read_id=GT2005_READ_ID;
}	/* GT2005_GetID */



/*************************************************************************
* FUNCTION
*	GT2005_GetSize
*
* DESCRIPTION
*	This function return the image width and height of image sensor.
*
* PARAMETERS
*	*sensor_width : address pointer of horizontal effect pixels of image sensor
*  *sensor_height : address pointer of vertical effect pixels of image sensor
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GT2005_GetSize(unsigned short *sensor_width, unsigned short *sensor_height)
{
	*sensor_width=IMAGE_SENSOR_FULL_WIDTH;		/* pixel numbers actually used in one frame */
	*sensor_height=IMAGE_SENSOR_FULL_HEIGHT;		/* line numbers actually used in one frame */
}	/* GT2005_GetSize */



/*************************************************************************
* FUNCTION
*	GT2005_GetPeriod
*
* DESCRIPTION
*	This function return the image width and height of image sensor.
*
* PARAMETERS
*	*pixel_number : address pointer of pixel numbers in one period of HSYNC
*  *line_number : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GT2005_GetPeriod(unsigned short *pixel_number, unsigned short *line_number)
{
	*pixel_number=PV_PERIOD_PIXEL_NUMS;	/* pixel numbers in one period of HSYNC */
	*line_number=PV_PERIOD_LINE_NUMS;		/* line numbers in one period of VSYNC */
}	/* GT2005_GetPeriod */


void GT2005_Preview(void)
{
	dprintk("%s Start!! \n", __FUNCTION__);
	 //Preview Setting Start//

	 //Resolution Setting
	 GT2005_write_cmos_sensor(0x0109 , 0x00);
	 GT2005_write_cmos_sensor(0x010A , 0x04);	
	 GT2005_write_cmos_sensor(0x010B , 0x03);					
	 GT2005_write_cmos_sensor(0x0110 , 0x03);					
	 GT2005_write_cmos_sensor(0x0111 , 0x20);					
	 GT2005_write_cmos_sensor(0x0112 , 0x02);					
	 GT2005_write_cmos_sensor(0x0113 , 0x58);

	 //PLL Setting 18.6FPS Under 48MHz PCLK
	 GT2005_write_cmos_sensor(0x0116 , 0x02);
	 GT2005_write_cmos_sensor(0x0118 , 0x40);
	 GT2005_write_cmos_sensor(0x0119 , 0x01);
	 GT2005_write_cmos_sensor(0x011a , 0x04);	
	 GT2005_write_cmos_sensor(0x011B , 0x00);					
	//Preview Setting End//

	GT2005_delay(150);

	dprintk("%s End!! \n", __FUNCTION__);
		
}

void GT2005_Capture(void)
{
	dprintk("%s Start!! \n", __FUNCTION__);

	/* 2M FULL Mode */
	gPVmode = FALSE;

	//GT2005_write_shutter();

	//Resolution Setting
	GT2005_write_cmos_sensor(0x0109 , 0x01);
	GT2005_write_cmos_sensor(0x010A , 0x00);	
	GT2005_write_cmos_sensor(0x010B , 0x00);					
	GT2005_write_cmos_sensor(0x0110 , 0x06);					
	GT2005_write_cmos_sensor(0x0111 , 0x40);					
	GT2005_write_cmos_sensor(0x0112 , 0x04);					
	GT2005_write_cmos_sensor(0x0113 , 0xb0);

#ifndef INTERPOLATION_3M
	//PLL Setting 4.65FPS Under 24MHz PCLK
	GT2005_write_cmos_sensor(0x0119 , 0x02);	
	GT2005_write_cmos_sensor(0x011B , 0x01);	
#else
	//PLL Setting 2.33FPS Under 12MHz PCLK
	GT2005_write_cmos_sensor(0x0119 , 0x05);	
	GT2005_write_cmos_sensor(0x011B , 0x02);	
#endif

	GT2005_delay(500);

	dprintk("%s End!! \n", __FUNCTION__);
		
}

#if 0
void GT2005_Preview(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{
	 sensor_cap_state = KAL_FALSE;

	 //Preview Setting Start//

	 //Resolution Setting
	 GT2005_write_cmos_sensor(0x0109 , 0x00);
	 GT2005_write_cmos_sensor(0x010A , 0x04);	
	 GT2005_write_cmos_sensor(0x010B , 0x03);					
	 GT2005_write_cmos_sensor(0x0110 , 0x03);					
	 GT2005_write_cmos_sensor(0x0111 , 0x20);					
	 GT2005_write_cmos_sensor(0x0112 , 0x02);					
	 GT2005_write_cmos_sensor(0x0113 , 0x58);

	 //PLL Setting 18.6FPS Under 48MHz PCLK
	 GT2005_write_cmos_sensor(0x0116 , 0x02);
	 GT2005_write_cmos_sensor(0x0118 , 0x40);
	 GT2005_write_cmos_sensor(0x0119 , 0x01);
	 GT2005_write_cmos_sensor(0x011a , 0x04);	
	 GT2005_write_cmos_sensor(0x011B , 0x00);					
	//Preview Setting End//

	kal_sleep_task(150);


	gPVmode = KAL_TRUE;

	if (sensor_config_data->isp_op_mode != ISP_PREVIEW_MODE)
	{
		if(sensor_config_data->frame_rate==0x0F)
		{		
			// MPEG4 Encode Mode
			VEDIO_encode_mode = KAL_TRUE;
		}
		else 
		{
			VEDIO_encode_mode = KAL_FALSE;
		}

	        /* config TG of ISP to match the setting of image sensor*/
	        SET_TG_OUTPUT_CLK_DIVIDER(1);   // MCLK = 24MHz
	        SET_CMOS_RISING_EDGE(0);
	        SET_CMOS_FALLING_EDGE(1);
	        ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;
	        SET_TG_PIXEL_CLK_DIVIDER(1);    // PCLK = 24MHz
	        SET_CMOS_DATA_LATCH(1);
	}
	else 
	{					
		 VEDIO_encode_mode = KAL_FALSE;
			
		 SET_TG_OUTPUT_CLK_DIVIDER(1);	// MCLK = 24MHz
		 SET_CMOS_RISING_EDGE(0);
		 SET_CMOS_FALLING_EDGE(1);
		 ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;
		 SET_TG_PIXEL_CLK_DIVIDER(1);	// PCLK = 24MHz
		 SET_CMOS_DATA_LATCH(1);					
	}
	   
	image_window->grab_start_x = 1;
	image_window->grab_start_y = 1;
	image_window->exposure_window_width = PV_PERIOD_PIXEL_NUMS - 3;
	image_window->exposure_window_height = PV_PERIOD_LINE_NUMS - 3;

}	/* GT2005_Preview */

void GT2005_Capture(image_sensor_exposure_window_struct *image_window, image_sensor_config_struct *sensor_config_data)
{

	sensor_cap_state = KAL_TRUE;

	if(VEDIO_encode_mode)ASSERT(0);

	if ((image_window->image_target_width<=PV_PERIOD_PIXEL_NUMS)&&(image_window->image_target_height<=PV_PERIOD_LINE_NUMS))
	{ 
		/* Less than PV Mode */
		gPVmode=KAL_TRUE;

		image_window->grab_start_x = 1;
		image_window->grab_start_y = 1;
		image_window->exposure_window_width = PV_PERIOD_PIXEL_NUMS - 3;
		image_window->exposure_window_height = PV_PERIOD_LINE_NUMS - 3;

	}
	else
	{
		/* 2M FULL Mode */
		gPVmode = KAL_FALSE;

		GT2005_write_shutter();

		//Resolution Setting
		GT2005_write_cmos_sensor(0x0109 , 0x01);
		GT2005_write_cmos_sensor(0x010A , 0x00);	
		GT2005_write_cmos_sensor(0x010B , 0x00);					
		GT2005_write_cmos_sensor(0x0110 , 0x06);					
		GT2005_write_cmos_sensor(0x0111 , 0x40);					
		GT2005_write_cmos_sensor(0x0112 , 0x04);					
		GT2005_write_cmos_sensor(0x0113 , 0xb0);

	#ifndef INTERPOLATION_3M
		//PLL Setting 4.65FPS Under 24MHz PCLK
		GT2005_write_cmos_sensor(0x0119 , 0x02);	
		GT2005_write_cmos_sensor(0x011B , 0x01);	
	#else
		//PLL Setting 2.33FPS Under 12MHz PCLK
		GT2005_write_cmos_sensor(0x0119 , 0x05);	
		GT2005_write_cmos_sensor(0x011B , 0x02);	
	#endif
	
		Delayms_GC(1000);

		image_window->grab_start_x=1;
		image_window->grab_start_y=2;
		image_window->exposure_window_width=IMAGE_SENSOR_FULL_WIDTH -3;
		image_window->exposure_window_height=IMAGE_SENSOR_FULL_HEIGHT -3;
	}

}	/* GT2005_Capture */

#endif

/*************************************************************************
* FUNCTION
*	GT2005_WriteReg
*
* DESCRIPTION
*	This function set the register of GT2005.
*
* PARAMETERS
*	addr : the register index of GT2005
*     para : setting parameter of the specified register of GT2005
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GT2005_WriteReg(unsigned int addr, unsigned int para)
{
	GT2005_write_cmos_sensor(addr,para);
}	/* GT2005_WriteReg */



/*************************************************************************
* FUNCTION
*	GT2005_ReadReg
*
* DESCRIPTION
*	This function read parameter of specified register from GT2005.
*
* PARAMETERS
*	addr : the register index of GT2005
*
* RETURNS
*	the data that read from GT2005
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_ReadReg(unsigned int addr)
{
	return (GT2005_read_cmos_sensor(addr));
}	/* GT2005_ReadReg */



/*************************************************************************
* FUNCTION
*	GT2005_SetShutter
*
* DESCRIPTION
*	This function set e-shutter of GT2005 to change exposure time.
*
* PARAMETERS
*	shutter : exposured lines
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GT2005_SetShutter(unsigned short shutter)
{} /* GT2005_SetShutter */



/*************************************************************************
* FUNCTION
*	GT2005_SetGain
*
* DESCRIPTION
*	This function is to set global gain to sensor.
*
* PARAMETERS
*	gain : sensor global gain(base: 0x40)
*
* RETURNS
*	the actually gain set to sensor.
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned short GT2005_SetGain(unsigned short gain)
{
    /* YUV : setGain would not be called */
    return gain;
} /* GT2005_SetGain */



/*************************************************************************
* FUNCTION
*	GT2005_NightMode
*
* DESCRIPTION
*	This function night mode of GT2005.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GT2005_NightMode(bool enable)
{     
	if (sensor_cap_state == FALSE)
	{
		if (enable)
		{
			sensor_night_mode = TRUE;
			
			if (VEDIO_encode_mode == TRUE)
			{
				/* MJPEG or MPEG4 Apps */
				GT2005_write_cmos_sensor(0x0312 , 0x98); //VIDEO enable night mode  1/2 Frame rate
			}
			else 
			{
				/* Camera mode only */                
				GT2005_write_cmos_sensor(0x0312 , 0xc8); //Camera Enable night mode  1/5 Frame rate
			}      
		}
		else 
		{
			sensor_night_mode = FALSE;
			
			if (VEDIO_encode_mode == TRUE) 
			{
				/* MJPEG or MPEG4 Apps */
		       	GT2005_write_cmos_sensor(0x0312 , 0x08); //Disable night mode  Frame rate do not change
			}
			else {
				/* Camera mode only */      
				GT2005_write_cmos_sensor(0x0312 , 0x98); //Disable night mode  1/2 Frame rate
			}
			
		}	
	}

}	/* GT2005_NightMode */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_zoom
*
* DESCRIPTION
*	GT2005 zoom setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_zoom(unsigned int para)
{
	return FALSE;
} /* GT2005_set_param_zoom */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_contrast
*
* DESCRIPTION
*	GT2005 contrast setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_contrast(unsigned int para)
{
	return FALSE;
} /* GT2005_set_param_contrast */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_brightness
*
* DESCRIPTION
*	brightness setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_brightness(unsigned int para)
{
	return FALSE;
} /* GT2005_set_param_brightness */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_hue
*
* DESCRIPTION
*	hue setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_hue(unsigned int para)
{
	return FALSE;
} /* GT2005_set_param_hue */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_gamma
*
* DESCRIPTION
*	gamma setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_gamma(unsigned int para)
{
    return FALSE;
} /* GT2005_set_param_gamma */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_wb
*
* DESCRIPTION
*	wb setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_wb(unsigned int para)
{
	switch (para)
	{
		
		case CAM_WB_AUTO:
			GT2005_write_cmos_sensor(0x031a,0x81);   // select Auto WB
			GT2005_write_cmos_sensor(0x0320,0x24);//  WBGRMAX[7:0];
			GT2005_write_cmos_sensor(0x0321,0x14);//  WBGRMIN[7:0];
			GT2005_write_cmos_sensor(0x0322,0x24);//  WBGBMAX[7:0];
			GT2005_write_cmos_sensor(0x0323,0x1a);//  WBGBMIN[7:0];
			GT2005_write_cmos_sensor(0x0441,0x4B);//  PWBGAINR[7:0];
			GT2005_write_cmos_sensor(0x0442,0x00);//  PWBGAINGR[7:0];
			GT2005_write_cmos_sensor(0x0443,0x00);//  PWBGAINGB[7:0];
			GT2005_write_cmos_sensor(0x0444,0x31);//  PWBGAINB[7:0];
			break;

		case CAM_WB_CLOUD: //cloudy
			GT2005_write_cmos_sensor(0x0320 ,0x02);//  WBGRMAX[7:0];
			GT2005_write_cmos_sensor(0x0321 ,0x02);//  WBGRMIN[7:0];
			GT2005_write_cmos_sensor(0x0322 ,0x02);//  WBGBMAX[7:0];
			GT2005_write_cmos_sensor(0x0323 ,0x02);//  WBGBMIN[7:0];
			GT2005_write_cmos_sensor(0x0441 ,0x72);//  PWBGAINR[7:0];
			GT2005_write_cmos_sensor(0x0442 ,0x00);//  PWBGAINGR[7:0];
			GT2005_write_cmos_sensor(0x0443 ,0x00);//  PWBGAINGB[7:0];
			GT2005_write_cmos_sensor(0x0444 ,0x0D);//  PWBGAINB[7:0];
			break;

		case CAM_WB_DAYLIGHT: //sunny
			GT2005_write_cmos_sensor(0x0320 ,0x02);//  WBGRMAX[7:0];
			GT2005_write_cmos_sensor(0x0321 ,0x02);//  WBGRMIN[7:0];
			GT2005_write_cmos_sensor(0x0322 ,0x02);//  WBGBMAX[7:0];
			GT2005_write_cmos_sensor(0x0323 ,0x02);//  WBGBMIN[7:0];
			GT2005_write_cmos_sensor(0x0441 ,0x60);//  PWBGAINR[7:0];
			GT2005_write_cmos_sensor(0x0442 ,0x00);//  PWBGAINGR[7:0];
			GT2005_write_cmos_sensor(0x0443 ,0x00);//  PWBGAINGB[7:0];
			GT2005_write_cmos_sensor(0x0444 ,0x14);//  PWBGAINB[7:0];
			break;

		case CAM_WB_INCANDESCENCE: //office
		case CAM_WB_TUNGSTEN: //home
			GT2005_write_cmos_sensor(0x0320 ,0x02);//  WBGRMAX[7:0];
			GT2005_write_cmos_sensor(0x0321 ,0x02);//  WBGRMIN[7:0];
			GT2005_write_cmos_sensor(0x0322 ,0x02);//  WBGBMAX[7:0];
			GT2005_write_cmos_sensor(0x0323 ,0x02);//  WBGBMIN[7:0];
			GT2005_write_cmos_sensor(0x0441 ,0x0B);//  PWBGAINR[7:0];
			GT2005_write_cmos_sensor(0x0442 ,0x00);//  PWBGAINGR[7:0];
			GT2005_write_cmos_sensor(0x0443 ,0x00);//  PWBGAINGB[7:0];
			GT2005_write_cmos_sensor(0x0444 ,0x5E);//  PWBGAINB[7:0];
			break;

      		case CAM_WB_FLUORESCENT:
      			GT2005_write_cmos_sensor(0x0320 ,0x02);//  WBGRMAX[7:0];
			GT2005_write_cmos_sensor(0x0321 ,0x02);//  WBGRMIN[7:0];
			GT2005_write_cmos_sensor(0x0322 ,0x02);//  WBGBMAX[7:0];
			GT2005_write_cmos_sensor(0x0323 ,0x02);//  WBGBMIN[7:0];
			GT2005_write_cmos_sensor(0x0441 ,0x43);//  PWBGAINR[7:0];
			GT2005_write_cmos_sensor(0x0442 ,0x00);//  PWBGAINGR[7:0];
			GT2005_write_cmos_sensor(0x0443 ,0x00);//  PWBGAINGB[7:0];
			GT2005_write_cmos_sensor(0x0444 ,0x4B);//  PWBGAINB[7:0];
			break;

		case CAM_WB_MANUAL:
		    	// TODO
			break;
	}

	
	return TRUE;
} /* GT2005_set_param_wb */

/*************************************************************************
* FUNCTION
*	GT2005_set_param_exposure
*
* DESCRIPTION
*	exposure setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_exposure(unsigned int para)
{
	switch (para)
	{
		case CAM_EV_NEG_4_3:    
            		GT2005_write_cmos_sensor(0x0300, 0x80);
			GT2005_write_cmos_sensor(0x0301, 0x90);
			GT2005_write_cmos_sensor(0x0201, 0x85);
			break;
			
		case CAM_EV_NEG_3_3:
            		GT2005_write_cmos_sensor(0x0300, 0x80);
			GT2005_write_cmos_sensor(0x0301, 0xa5);
			GT2005_write_cmos_sensor(0x0201, 0xa4);
			break;
			
		case CAM_EV_NEG_2_3:
            		GT2005_write_cmos_sensor(0x0300, 0x81);
			GT2005_write_cmos_sensor(0x0301, 0x10);
			GT2005_write_cmos_sensor(0x0201, 0xca);
			break;
			
		case CAM_EV_NEG_1_3:
            		GT2005_write_cmos_sensor(0x0300, 0x81);
			GT2005_write_cmos_sensor(0x0301, 0x40);
			GT2005_write_cmos_sensor(0x0201, 0xf0);
			break;
			
		case CAM_EV_ZERO:
           		GT2005_write_cmos_sensor(0x0300, 0x81);
			GT2005_write_cmos_sensor(0x0301, 0x80);
			GT2005_write_cmos_sensor(0x0201, 0x0c);
			break;
			
		case CAM_EV_POS_1_3:
            		GT2005_write_cmos_sensor(0x0300, 0x81);
			GT2005_write_cmos_sensor(0x0301, 0xc0);
			GT2005_write_cmos_sensor(0x0201, 0x40);
			break;
			
		case CAM_EV_POS_2_3:
            		GT2005_write_cmos_sensor(0x0300, 0x81);
			GT2005_write_cmos_sensor(0x0301, 0xf0);
			GT2005_write_cmos_sensor(0x0201, 0x50);
			break;
			
		case CAM_EV_POS_3_3:
            		GT2005_write_cmos_sensor(0x0300, 0x82);
			GT2005_write_cmos_sensor(0x0301, 0x20);
			GT2005_write_cmos_sensor(0x0201, 0x60);
			break;
			
		case CAM_EV_POS_4_3:	
            		GT2005_write_cmos_sensor(0x0300, 0x82);
			GT2005_write_cmos_sensor(0x0301, 0x60);
			GT2005_write_cmos_sensor(0x0201, 0x70);
			break;
			
		default:
			return FALSE;
	}


	return TRUE;
} /* GT2005_set_param_exposure */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_effect
*
* DESCRIPTION
*	effect setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_effect(unsigned int para)
{
	switch (para)
	{
		case CAM_EFFECT_ENC_NORMAL:
			GT2005_write_cmos_sensor(0x0115,0x00);
	      	gc_effect_on = FALSE;
			break;		

		case CAM_EFFECT_ENC_GRAYSCALE:
			GT2005_write_cmos_sensor(0x0115,0x06);
			gc_effect_on = TRUE;
			break;

		case CAM_EFFECT_ENC_SEPIA:
		     	GT2005_write_cmos_sensor(0x0115,0x0a);
			GT2005_write_cmos_sensor(0x026e,0x60);
			GT2005_write_cmos_sensor(0x026f,0xa0);
			gc_effect_on = TRUE;
			break;		
				
		case CAM_EFFECT_ENC_SEPIAGREEN:
			GT2005_write_cmos_sensor(0x0115,0x0a);
			GT2005_write_cmos_sensor(0x026e,0x20);
			GT2005_write_cmos_sensor(0x026f,0x00);
			gc_effect_on = TRUE;
			break;					

		case CAM_EFFECT_ENC_SEPIABLUE:
			GT2005_write_cmos_sensor(0x0115,0x0a);
			GT2005_write_cmos_sensor(0x026e,0xfb);
			GT2005_write_cmos_sensor(0x026f,0x00);
			gc_effect_on = TRUE;
			break;								

		case CAM_EFFECT_ENC_COLORINV:
			GT2005_write_cmos_sensor(0x0115,0x09);
			gc_effect_on = TRUE;
			break;		

		default:
			return  FALSE;
	}
	

	return TRUE;

} /* GT2005_set_param_effect */

/*************************************************************************
* FUNCTION
*	GT2005_set_param_banding
*
* DESCRIPTION
*	banding setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_banding(unsigned int para)
{
	switch (para)
	{
		case CAM_BANDING_50HZ:
			GT2005_write_cmos_sensor(0x0315, 0x16);                  			
			GT2005_write_cmos_sensor(0x0313, 0x35); 
			GT2005_write_cmos_sensor(0x0314, 0x36); 
	
			break;

		case CAM_BANDING_60HZ:
			GT2005_write_cmos_sensor(0x0315, 0x56);                  			
			GT2005_write_cmos_sensor(0x0313, 0x35); 
			GT2005_write_cmos_sensor(0x0314, 0x36); 
			break;

	      default:
	          return FALSE;
	}

	return TRUE;
} /* GT2005_set_param_banding */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_saturation
*
* DESCRIPTION
*	SATURATION setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_saturation(unsigned int para)
{
    // Not Support
	return FALSE;
} /* GT2005_set_param_saturation */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_nightmode
*
* DESCRIPTION
*	night mode setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_nightmode(unsigned int para)
{
	GT2005_NightMode((bool)para);
	return TRUE;
} /* GT2005_set_param_nightmode */



/*************************************************************************
* FUNCTION
*	GT2005_set_param_ev
*
* DESCRIPTION
*	ev setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_set_param_ev(unsigned int para)
{
	return GT2005_set_param_exposure(para);
} /* GT2005_set_param_ev */



/*************************************************************************
* FUNCTION
*	GT2005_YUV_SensorSetting
*
* DESCRIPTION
*	This function send command and parameter to yuv sensor module GT2005
*  to configure it
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
unsigned int GT2005_YUV_SensorSetting(unsigned int cmd, unsigned int para)
{
	unsigned int ret = TRUE;
	
	if (sensor_cap_state == TRUE) {
		return TRUE;
	}

	switch (cmd)
	{
		case CAM_PARAM_ZOOM_FACTOR:
			ret = GT2005_set_param_zoom(para);
			break;
		case CAM_PARAM_CONTRAST:
			ret = GT2005_set_param_contrast(para);
			break;
		case CAM_PARAM_BRIGHTNESS:
			ret = GT2005_set_param_brightness(para);
			break;
		case CAM_PARAM_HUE:
			ret = GT2005_set_param_hue(para);
			break;
		case CAM_PARAM_GAMMA:
			ret = GT2005_set_param_gamma(para);
			break;
		case CAM_PARAM_WB:
			ret = GT2005_set_param_wb(para);
			break;
		case CAM_PARAM_EXPOSURE:
			ret = GT2005_set_param_exposure(para);
			break;
		case CAM_PARAM_EFFECT:
			ret = GT2005_set_param_effect(para);
			break;
		case CAM_PARAM_BANDING:
			ret = GT2005_set_param_banding(para);
			break;
		case CAM_PARAM_SATURATION:
			ret = GT2005_set_param_saturation(para);
			break;
		case CAM_PARAM_NIGHT_MODE:
			ret = GT2005_set_param_nightmode(para);
			break;
		case CAM_PARAM_EV_VALUE:
			ret = GT2005_set_param_ev(para);
			break;
		default:
			ret = FALSE;
	}
	return ret;
} /* GT2005_YUV_SensorSetting */




#if 0

/*************************************************************************
* FUNCTION
*	image_sensor_func_GT2005
*
* DESCRIPTION
*	GT2005 Image Sensor functions struct.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
image_sensor_func_struct image_sensor_func_GT2005=
{
	GT2005_Init,
	GT2005_GetID,
	GT2005_GetSize,
	GT2005_GetPeriod,
	GT2005_Preview,
	GT2005_Capture,
	GT2005_WriteReg,
	GT2005_ReadReg,
	GT2005_SetShutter,
	GT2005_NightMode,
	GT2005_PowerOff,
	GT2005_SetGain,
	NULL,
    	GT2005_YUV_SensorSetting
};  /* image_sensor_func_GT2005 */



/*************************************************************************
* FUNCTION
*	image_sensor_func_config
*
* DESCRIPTION
*	This function maps the external camera module function API structure.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void image_sensor_func_config(void)
{
	image_sensor_func = &image_sensor_func_GT2005;
}	/* image_sensor_func_config */

#endif

