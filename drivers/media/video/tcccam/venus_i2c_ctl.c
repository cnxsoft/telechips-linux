/*************************************************************************************************/
/* AIT ISP Host API                                                                              */
/* All rights reserved by Alpah Image Technology Corp                                            */
/*-----------------------------------------------------------------------------------------------*/
/* File name: Venus_i2c_ctl.c                                                                    */
/* Description: ISP Host API i2c routines abstraction layer                                      */
/*                                                                                               */
/* Version 0.0A   20090326                                                                       */
/*************************************************************************************************/

#include "venus_i2c_ctl.h"
#include "venus_project.h"
#include "tcc_cam_i2c.h"
#include "venusv3_api.h"

//#define AIT848_DEVICE_ID	(0x06)
#define SENSOR_5M_AIT848_ISP_I2C_ADDR	(0x06)


/*************************************************************************************************/
/* I2C Implementation                                                                            */
/*************************************************************************************************/

#if VA_PROJECT_AIT705DEMOKIT

	extern FILE *gOutfp;

	extern void SetVenusRegB(uint16 Addr, uint8 Val);
	extern void SetVenusRegW(uint16 Addr, uint16 Val);
	extern void SetVenusMultiBytes(uint8* dataPtr, uint16 startAddr, uint16 length);
	extern uint8 GetVenusRegB(uint16 Addr);
	extern uint16 GetVenusRegW(uint16 Addr);
	extern void GetVenusMultiBytes(uint8* dataPtr, uint16 startAddr, uint16 length);

void transmit_byte_via_i2c(uint16 addr, uint8 data)
{
		SetVenusRegB(addr, data);
	#if TRACE_I2C_CMD_EN
		fprintf(gOutfp, "	W 0x%04X, 0x%02X,\n", addr, data);
	#endif
}
    
void transmit_word_via_i2c(uint16 addr, uint16 data)
{
		SetVenusRegW(addr, data);
	#if TRACE_I2C_CMD_EN
		fprintf(gOutfp, "	W 0x%04X, 0x%02X,\n", addr, data & 0xff);
		fprintf(gOutfp, "	W 0x%04X, 0x%02X,\n", addr+1, (data >>8) & 0xff);
	#endif
}
    
void transmit_multibytes_via_i2c(uint8* ptr, uint16 addr, uint16 length)
{
	#if TRACE_I2C_CMD_EN
		uint32 i;
    for (i = 0; i < length; i++) {
			fprintf(gOutfp, "	W 0x%04X, 0x%02X,\n", addr + i, ptr[i]);
    }
	#endif
		SetVenusMultiBytes(ptr, addr, length);
}
     
uint8 receive_byte_via_i2c(uint16 addr)
{
	#if TRACE_I2C_CMD_EN
		if(addr == 0x6713 || addr== 0x65fe)
			fprintf(gOutfp, "	Polling: 0x%04X, = 0x%02X,\n", addr,GetVenusRegB(addr));
		else
			fprintf(gOutfp, "	R 0x%04X, = 0x%02X,\n", addr,GetVenusRegB(addr));
	#endif

		return GetVenusRegB(addr);
}

uint16 receive_word_via_i2c(uint16 addr)
{
	#if TRACE_I2C_CMD_EN
		fprintf(gOutfp, "	R 0x%04X, = 0x%02X,\n", addr,GetVenusRegB(addr));
		fprintf(gOutfp, "	R 0x%04X, = 0x%02X,\n", addr+1,GetVenusRegB(addr+1));
	#endif
    
		return GetVenusRegW(addr);
}

void receive_multibytes_via_i2c(uint8* ptr, uint16 addr, uint16 length)
{
		GetVenusMultiBytes(ptr, addr, length);
    }
    
#elif VA_PROJECT_SPEACY

int transmit_i2c(unsigned short reg, unsigned char *val, unsigned short len)
{
	unsigned char data[132];
	unsigned int cnt;
	unsigned char bytes;

	data[0]= reg>>8;
	data[1]= (u8)reg&0xff;
	bytes = 2;
	
	for( cnt = 0; cnt<len ; cnt++)
	{
		data[bytes + cnt] = val[cnt];
	}

	DDI_I2C_Write(data, 2, len);

	return 0;
}


void transmit_byte_via_i2c(uint16 addr, uint8 data)
{
	uint8 tData[10];

	tData[0] = data;
	
	transmit_i2c(addr, tData, 1);
}
    
void transmit_word_via_i2c(uint16 addr, uint16 data)
{
	uint8 tData[10];

	tData[0]= (uint8)data&0xff;
	tData[1]= data>>8;
	
	transmit_i2c(addr, tData, 2);
}
    
void transmit_multibytes_via_i2c(uint8* ptr, uint16 addr, uint16 length)
{
	transmit_i2c(addr, ptr, length);
}
     
uint8 receive_byte_via_i2c(uint16 addr)
{
	unsigned char ucSzDataBuff;

//	printk("receive_byte_via_i2c: addr = 0x%x, data_buff = 0x%x \n", addr, ucSzDataBuff);
	DDI_I2C_Read(addr, 2, &ucSzDataBuff, 1);

	return ucSzDataBuff;
}

uint16 receive_word_via_i2c(uint16 addr)
{
	unsigned char ucSzDataBuff[2];
	uint16 WordOfRecive;
	
//	printk("receive_word_via_i2c \n");
	DDI_I2C_Read(addr, 2, ucSzDataBuff, 2);

	WordOfRecive = (((uint16)*(ucSzDataBuff)) + ((uint16)*(ucSzDataBuff+1)<<8));
	
	return WordOfRecive;
}

void receive_multibytes_via_i2c(uint8* ptr, uint16 addr, uint16 length)
{
//	printk("receive_multibytes_via_i2c \n");

	DDI_I2C_Read(addr, 2, ptr, length);
}

uint8 receive_byte_via_i2c_ISP(uint16 addr) // This api will use for CIF camera interface.
{
    uint8 data8;

    //CAM_ReadI2c_Ait848(addr,1,&data8);
    
    return data8;
}

void transmit_byte_via_i2c_ISP(uint16 addr, uint8 data) // This api will use for CIF camera interface.
{
    //CAM_WriteI2c_Ait848(addr,1,&data);
}

uint16 receive_word_via_i2c_ISP(uint16 addr)
{
    uint8 data8[2];

    //CAM_ReadI2c_Ait848(addr,2,data8);

    return (data8[1]<<8)|data8[0];
}

#endif


short RegisterAccessTest() /* Perform the register access test */
{
	unsigned short Addr, ValWord, i;
	unsigned char ValByte;
	short ErrorCount = 0;
	
	V3A_ChipEnable();
	V3A_ChipReset();
	
	for ( i = 0; i<16; i++ )
	{
		Addr = 0x65F0 + i;
		transmit_byte_via_i2c( Addr, i);
	}

	for ( i = 0; i<16; i++ )
	{
		Addr = 0x65F0 + i;
		ValByte = receive_byte_via_i2c( Addr );
		if ( ValByte != i )
		{
			printk("Register Byte Access Failure\n");
			ErrorCount++;
		}
	}
	for ( i = 0; i<8; i++ )
	{
		Addr = 0x65F0 + i*2;
		transmit_word_via_i2c ( Addr, i);
	}
	for ( i = 0; i<8; i++ )
	{
		Addr = 0x65F0 + i*2;
		ValWord = receive_word_via_i2c ( Addr );
		if ( ValWord != i )
		{
			printk("Register Word Access Failure\n");
			ErrorCount++;
		}
	}
	return ErrorCount;
}
	

/*short RegisterAccessTest()  
{
   unsigned short Addr, ValWord, i;
   unsigned char ValByte;
   short ErrorCount = 0;
   
   for ( i = 0; i<16; i++ )
   {
      Addr = 0x6540 + i;
      transmit_byte_via_i2c( Addr, i);
   }
   for ( i = 0; i<16; i++ )
   {
      Addr = 0x6540 + i;
      ValByte = receive_byte_via_i2c( Addr );
      if ( ValByte != i )
      {
         printf("Register Byte Access Failure\n");
         ErrorCount++;
      }
   }
   for ( i = 0; i<8; i++ )
   {
      Addr = 0x6540 + i*2;
      transmit_word_via_i2c( Addr, i);
   }
   for ( i = 0; i<8; i++ )
   {
      Addr = 0x6540 + i*2;
      ValWord = receive_word_via_i2c( Addr );
      if ( ValWord != i )
      {
         printf("Register Word Access Failure\n");
         ErrorCount++;
      }
   }
   return ErrorCount;
}*/

short MemoryAccessTest()    /* Perform the memory access test */
{
   unsigned char ValBuf[16];
   unsigned short i;
   short ErrorCount = 0;
   
   for ( i = 0; i<16; i++ )
   {
      ValBuf[i] = i;
   }
   transmit_multibytes_via_i2c( ValBuf, 0x8000, 16 );
   for ( i = 0; i<16; i++ )
   {
      ValBuf[i] = 0;
   }
   receive_multibytes_via_i2c( ValBuf, 0x8000, 16 );
   
   for ( i = 0; i<16; i++ )
   {
      if ( ValBuf[i] != i )
      {
         printk("Memory Access Failure\n");
         ErrorCount++;
      }
   }
   return ErrorCount;
}

