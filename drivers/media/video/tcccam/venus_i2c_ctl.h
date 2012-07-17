/*************************************************************************************************/
/* AIT ISP Host API                                                                              */
/* All rights reserved by Alpah Image Technology Corp                                            */
/*-----------------------------------------------------------------------------------------------*/
/* File name: Venus_i2c_ctl.h                                                                    */
/* Description: ISP Host API i2c routines abstraction layer                                      */
/*                                                                                               */
/* Version 0.0A   20090326                                                                       */
/*************************************************************************************************/

#ifndef _VENUS_I2C_CTL_H_
#define _VENUS_I2C_CTL_H_


#include "venus_project.h"


/*************************************************************************************************/
/* I2C Interface                                                                                 */
/*************************************************************************************************/
void transmit_byte_via_i2c(uint16 addr, uint8 data);
void transmit_word_via_i2c(uint16 addr, uint16 data);
void transmit_multibytes_via_i2c(uint8* ptr, uint16 addr, uint16 length);

uint8 receive_byte_via_i2c(uint16 addr);
uint16 receive_word_via_i2c(uint16 addr);
void receive_multibytes_via_i2c(uint8* ptr, uint16 addr, uint16 length);
uint8 receive_byte_via_i2c_ISP(uint16 addr); // This api will use for CIF camera interface.
void transmit_byte_via_i2c_ISP(uint16 addr, uint8 data); // This api will use for CIF camera interface.
uint16 receive_word_via_i2c_ISP(uint16 addr);


#endif // _VENUS_I2C_CTL_H_


