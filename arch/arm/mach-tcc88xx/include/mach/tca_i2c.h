/****************************************************************************
 *   FileName    : tca_i2c.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef __TCA_I2C_H__
#define __TCA_I2C_H__

/* Define
 */
#define I2C_NUM_OF_CH		2	/* master0/1 */

#define I2C_IRQSTR_OFFSET	0xC0	/* only I2C */
#define I2C_ICLK_OFFSET		0x80	/* only SMU_I2C */


struct tcc_i2c_platform_data {
    int  smu_i2c_flag;
    int  core_clk_rate;
    char core_clk_name[10];
    int  i2c_ch_clk_rate[I2C_NUM_OF_CH];
};


/* register structure
 */
//#pragma pack(push, 4)
struct tcc_i2c_regs {
	volatile unsigned long PRES, CTRL, TXR, CMD, RXR, SR, TIME;
};
//#pragma pack(pop)


/* extern functions
 */
void tca_i2c_setgpio(int core, int ch);

#endif	//__TCA_I2C_H__
