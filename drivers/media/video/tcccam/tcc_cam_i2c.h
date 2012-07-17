/****************************************************************************
 *   TCC Version 0.6
 *   Copyright (c) telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
****************************************************************************/

#ifndef __TCC7901_I2C_H_char__
#define __TCC7901_I2C_H_char__

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif
#include "sensor_if.h"

#define	IO_CKC_EnableBUS_I2C()		(BITSET(HwBCLKCTR, HwBCLKCTR_I2C_ON))
#define	IO_CKC_DisableBUS_I2C()		(BITCLR(HwBCLKCTR, HwBCLKCTR_I2C_ON))

#define IO_CKC_Fxin 120000

/************************************************************************
 * I2C define
 ***********************************************************************/
#define		i2c_ack			0					// i2c acknowledgement
#define		i2c_noack		1					// i2c no acknowledgement
#define		i2c_err			0					// i2c error occurs
#define		i2c_ok			1					// i2c error does not occurs

#define i2c_full_delay 		2
#define i2c_half_delay 		1


// I2C-bus standard mode timing setting
#define		i2c_Tbuf		400		//200
#define		i2c_Tsetup		8		//4//8		// above min 250ns
#define		i2c_Thold		200		//100		// the fm_Thold is a basic time as above min 5us
#define		i2c_Tcatch		100		//50		// the half of the i2c_Thold

#define 		I2C_SUBADDR_NOUSE 	-1

// Message 
#define I2C_FREE	0
#define I2C_USED	1



enum 
{
	I2C_CODEC,
	I2C_TVOUT,
	I2C_TOUCH,
	I2C_CAMERA,
	I2C_MAX_VECTOR
} ;

enum
{
	I2C_CLK,
	I2C_DATA,
	I2C_SIGNAL_BUS
};

#define		i2c_wr			(*(volatile unsigned long int *)(I2C_Vector[I2C_DATA].tgtaddr+0x04) |= (I2C_Vector[I2C_DATA].port));
#define		i2c_rd			(*(volatile unsigned long int *)(I2C_Vector[I2C_DATA].tgtaddr+0x04) &= ~(I2C_Vector[I2C_DATA].port));
#define		i2c_clk_hi		(*(volatile unsigned long int *)(I2C_Vector[I2C_CLK].tgtaddr) |= (I2C_Vector[I2C_CLK].port));
#define		i2c_clk_lo		(*(volatile unsigned long int *)(I2C_Vector[I2C_CLK].tgtaddr) &= ~(I2C_Vector[I2C_CLK].port));	
#define		i2c_data_hi		(*(volatile unsigned long int *)(I2C_Vector[I2C_DATA].tgtaddr) |= (I2C_Vector[I2C_DATA].port)); 	   
#define		i2c_data_lo		(*(volatile unsigned long int *)(I2C_Vector[I2C_DATA].tgtaddr) &= ~(I2C_Vector[I2C_DATA].port)); 	 


//**********************************************************************
typedef struct I2C_SW_READ_s
{
	unsigned int i2ctgt;
	unsigned char SlaveAddr;
	char subaddr;
	unsigned char *ptr;
	unsigned char bytes;
	unsigned char i2c_return;
} I2C_SW_READ_t;

typedef struct I2C_SW_WRITE_s
{
	unsigned int i2ctgt;
	unsigned char SlaveAddr;
	char subaddr;
	unsigned char *ptr;
	unsigned char bytes;
	unsigned char i2c_return;
} I2C_SW_WRITE_t;


//***********************************************************************
extern int	DDI_I2C_Initialize(void);
extern int	DDI_I2C_Terminate(void);
extern int DDI_I2C_Write(unsigned char* data, unsigned short reg_bytes, unsigned short data_bytes);
extern int DDI_I2C_Read(unsigned short reg, unsigned char reg_bytes, unsigned char *val, unsigned short val_bytes);	

#endif /*__TCC7901_I2C_H_char__*/

