/*
 * linux/arch/arm/mach-tcc8800/include/mach/tcc_pca953x.h
 *
 * Author:  <linux@telechips.com>
 * Created: 17th September, 2010 
 * Description: GPIO Expand Driver 
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * ===============================================================================================
 *  USAGE
 * =======
 *
 * Use "tcc_pca953x_setup()" function for management port status.
 *
 * 1. read operation
 *
 *   int tcc_pca953x_setup(int slave, int name, int direction, int value, int mode);
 *
 *   parameters
 *   ----------
 *     int slave     - PCA953x I2C slave address
 *     int name      - fixed 0
 *     int direction - direction of port [OUTPUT/INPUT]
 *     int value     - fixed 0
 *     int mode      - fixed GET_VALUE
 *
 *   return value
 *   ------------
 *     int rd_buf    - 16bit read data returned, -1 is fail
 *
 *   example
 *   -------
 *     // read PCA9539_U2_SLAVE_ADDR port0 and port1
 *     int rd_buf = tcc_pca953x_setup(PCA9539_U2_SLAVE_ADDR, 0, OUTPUT, 0, GET_VALUE);
 *
 * 2. write operation
 *
 *   int tcc_pca953x_setup(int slave, int name, int direction, int value, int mode);
 *
 *   parameters
 *   ----------
 *     int slave     - PCA953x I2C slave address
 *     int name      - control port name, see "Expanded GPIO port map"
 *     int direction - direction of port [OUTPUT/INPUT]
 *     int value     - output mode only. set port value [HIGH/LOW]
 *     int mode      - [SET_DIRECTION/SET_VALUE] or [SET_DIRECTION|SET_VALUE]
 *
 *   return value
 *   ------------
 *     int rd_buf    - 1 is success, -1 is fail
 *
 *   example
 *   -------
 *     + set port direction
 *       // PCA9539_U2_SLAVE_ADDR, ETH_RST, Output mode
 *       tcc_pca953x_setup(PCA9539_U2_SLAVE_ADDR, ETH_RST, OUTPUT, 0, SET_DIRECTION);
 *
 *     + set port value (output mode only)
 *       // PCA9539_U2_SLAVE_ADDR, ETH_RST, Output High
 *       tcc_pca953x_setup(PCA9539_U2_SLAVE_ADDR, ETH_RST, OUTPUT, HIGH, SET_VALUE);
 *
 *     + set both (direction & value)
 *       // PCA9539_U2_SLAVE_ADDR, ETH_RST, Output mode, Output Low
 *       tcc_pca953x_setup(PCA9539_U2_SLAVE_ADDR, ETH_RST, OUTPUT, LOW, SET_DIRECTION|SET_VALUE);
 *
 * 3. direct read/write
 *   + read
 *     tcc_pca953x_read(PCA9539_U2_SLAVE_ADDR, PCA9539_OUTPUT_0, &buf[0]);
 *     tcc_pca953x_read(PCA9539_U2_SLAVE_ADDR, PCA9539_OUTPUT_1, &buf[1]);
 *     printk("PCA9539_U2_SLAVE_ADDR port0(%x), port1(%x)", buf[0], buf[1]);
 *   
 *   + write each port
 *     buf[0] = PCA9539_OUTPUT_0;
 *     buf[1] = 0x12;
 *     tcc_pca953x_write(PCA9539_U2_SLAVE_ADDR, &buf[0], 2);
 *     buf[0] = PCA9539_OUTPUT_1;
 *     buf[1] = 0x34;
 *     tcc_pca953x_write(PCA9539_U2_SLAVE_ADDR, &buf[0], 2);
 *   
 *   + write all port
 *     buf[0] = PCA9539_OUTPUT_0;
 *     buf[1] = 0x12;
 *     buf[2] = 0x34;
 *     tcc_pca953x_write(PCA9539_U3_SLAVE_ADDR, &buf[0], 3);
 *
 * ===============================================================================================
 */
#ifndef __PCA953X_H__
#define __PCA953X_H__

#include <mach/bsp.h>


/*
 * EXPORT_SYMBOL
 */
#ifdef CONFIG_TCC_I2C_PCA953X
extern int tcc_pca953x_setup(int slave, int name, int direction, int value, int mode);
extern int tcc_pca953x_read(int slave, unsigned char cmd, unsigned char *rd_buf);
extern int tcc_pca953x_write(int slave, const unsigned char *wr_buf, int count);
#else
static int tcc_pca953x_dummy(int format, ...) 
{
	return 0;
}
#define tcc_pca953x_setup(format, args...)	tcc_pca953x_dummy(format, ## args)
#define tcc_pca953x_read(format, args...)	tcc_pca953x_dummy(format, ## args)
#define tcc_pca953x_write(format, args...)	tcc_pca953x_dummy(format, ## args)
#endif

/*
 * PCA953x I2C slave address
 */
#define PCA9539_U2_SLAVE_ADDR	0x77
#define PCA9539_U3_SLAVE_ADDR	0x74
////#define PCA9537_U4_SLAVE_ADDR	0x49
#define PCA9539_U5_SLAVE_ADDR	0x75

/* 
 * PCA9539 U11 - TCC88XX_PWR_DISCRETE_SV5.0
 */
#define PCA9539_U11_SLAVE_ADDR	0x76

/*
 * Port setup mode
 */
#define GET_VALUE		0x0001
#define SET_DIRECTION	0x0010
#define SET_VALUE		0x0100

/*
 * Port direction & value
 */
#define INPUT	1
#define OUTPUT	0
#define HIGH	1
#define LOW		0

/*
 * Expanded GPIO port map
 */
/* PCA9539 U2 */
//PORT0
#define MUTE_CTL   				Hw0   // Audio Mute Control
#define LVDSIVT_ON     			Hw1   // LVDS Inverter
#define LVDS_LP_CTRL      		Hw2   // LVDS LCD Controller
#define AUTH_RST      			Hw3   // iPod Auth Reset
#define BT_RST     				Hw4   // 
#define SDWF_RST       			Hw5   // 
#define CAS_RST      			Hw6   // CAS Reset
#define CAS_GP      			Hw7	  // CASGP

//PORT1
#define DXB1_PD				    Hw8   // 
#define DXB2_PD     			Hw9   // 
#define PWR_CONTROL0       		Hw10  //  
#define PWR_CONTROL1     		Hw11  //  
#define HDD_RST      			Hw12  // HDD Reset
#define OTG0_VBUS_EN    		Hw13  //  
#define OTG1_VBUS_EN     		Hw14  //  
#define HOST_VBUS_EN     		Hw15  //  


/* PCA9539 U3 */
//PORT0
#define LCD_ON     				Hw0   // LCD Power 
#define CODEC_ON       			Hw1   // External Audio CODEC 
#define SD0_ON   				Hw2   //  
#define SD1_ON       			Hw3   //  
#define SD2_ON     				Hw4   //  
#define EXT_CODEC_ON      		Hw5   //  
#define GPS_PWREN       		Hw6   //  
#define BT_ON       			Hw7   // Bluetooth 
//PORT1
#define DXB_ON        			Hw8   // DMB, DAB Power 
#define IPOD_ON       			Hw9   // iPOD Connection Power 
#define CAS_ON       			Hw10  // CAS 
#define FMTC_ON       			Hw11  // FM Transceiver 
#define P_CAM_PWR_ON       		Hw12  //  
#define CAM1_ON      			Hw13  //  
#define CAM2_ON      			Hw14  //  
#define ATAPI_ON 				Hw15  // IDE Disk Interface 

/* PCA9537 U4 */
//PORT0
#define SD3_0_POW_CTL_0     	Hw0   //  
#define SD3_0_POW_CTL_1     	Hw1   //  
#define SD3_0_POW_CTL_2 		Hw2   //  
#define SD3_0_POW_CTL_3      	Hw3   //  

/* PCA9539 U5 */
//PORT0
#define FMT_RST					Hw0   // FM Transceiver Reset 
#define FMT_IRQ       			Hw1   // INPUT : FM Receiver IRQ 
#define BT_WAKE   				Hw2   // Bluetooth Wakeup 
#define BT_HWAKE       			Hw3   // INPUT : Bluetooth Host Wakeup 
#define PHY2_ON     			Hw4   //  
#define COMPASS_RST      		Hw5   //  
#define CAM_FL_EN       		Hw6   // Camera Flash Light En/Disable 
#define CAM2_FL_EN       		Hw7   // Camera Flash Light En/Disable 
//PORT1
#define CAM2_RST        		Hw8   //  
#define CAM2_PWDN       		Hw9   //  
#define LCD_RST       			Hw10  //  
#define TV_SLEEP       			Hw11  //  
#define ETH_ON       			Hw12  //  Ethernet Controller
#define ETH_RST      			Hw13  //  Ethernet Controller Reset
#define SMART_AUX1      		Hw14  //  
#define SMART_AUX2 				Hw15  //  

/* 
 * PCA9539 U11 - TCC88XX_PWR_DISCRETE_SV5.0
 */
//PORT0
#define PLL1_EN     			Hw0   // PLL1 Power Enable										Active H
#define OTG_EN       			Hw1   // OTG Power [1.2V, 3.3V, 5V] Enable						Active H
#define HS_HOST_EN   			Hw2   // High Speed Host Power [1.2V, 3.3V, 5V] Enable			Active H
#define FS_HOST_EN 		    	Hw3   // Full Speed Host Power [3.3V, 5V] Enable				Active H
#define HDMI_EN				    Hw4   // HDMI Power [1.2V, 3.3V, 5V] Enable						Active H
#define MIPI_EN      			Hw5   // MIPI Power [1.2V, 1.8V] Enable							Active H
#define SATA_EN       			Hw6   // SATA Power [3.3V] Enable								Active H
#define LVDS_EN       			Hw7   // LVDS Power [1.2V, 3.3V] Enable							Active H
//PORT1
#define ATV_V5P0_EN  		    Hw8   // Analog TV Power [5V] Enable							Active H
#define ATAPI_IPOD_V5P0_EN      Hw9   // ATAPI, iPod Power [5V] Enable							Active H
#define USB_CHARGE_CURRENT_SEL  Hw10  // USB Charger Current Selection [H : 500mA, L : 100mA]
#define USB_SUSPEND       		Hw11  // USB Suspend Mode Selection [H : Suspend, L : Normal]
#define CHARGE_EN       		Hw12  // Charger IC Enable										Active L
#define SD_3_0_PWR_EN      		Hw13  // SD3.0 Power Enable										Active H
#define SD_3_0_PWR_CTL      	Hw14  // SD3.0 Power Selection [H : 1.8V, L : 3.3V]			
#define CHARGE_FINISH 			Hw15  // Charge Finish											Active L


/*
 * PCA953x command
 */
enum pca9539_cmd
{
	PCA9539_INPUT_0		= 0,
	PCA9539_INPUT_1		= 1,
	PCA9539_OUTPUT_0	= 2,
	PCA9539_OUTPUT_1	= 3,
	PCA9539_INVERT_0	= 4,
	PCA9539_INVERT_1	= 5,
	PCA9539_DIRECTION_0	= 6,
	PCA9539_DIRECTION_1	= 7,
};

enum pca9538_cmd
{
	PCA9538_INPUT_0		= 0,
	PCA9538_OUTPUT_0	= 1,
	PCA9538_INVERT_0	= 2,
	PCA9538_DIRECTION_0	= 3,
};

enum pca9537_cmd
{
	PCA9537_INPUT_0		= 0,
	PCA9537_OUTPUT_0	= 1,
	PCA9537_INVERT_0	= 2,
	PCA9537_DIRECTION_0	= 3,
};


#endif	/*__PCA953X_H__*/

