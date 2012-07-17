/******************************************************************************
 * *
 * *  (C)Copyright All Rights Reserved by Telechips Inc.
 * *
 * *  This material is confidential and shall remain as such.
 * *  Any unauthorized use, distribution, reproduction is strictly prohibited.
 * *
 * *******************************************************************************
 * *
 * *  File Name   : tcc_fb_address.h
 * *
 * *  Description : calculate image start position of address
 * *
 * *******************************************************************************
 * *
 * *    yyyy/mm/dd     ver            descriptions                Author
 * *	---------	--------   ---------------       -----------------
 * *    2011/03/19     0.1            created                     KC Min
 * *******************************************************************************/
 
#ifndef __TCC_FB_ADDRESS__
#define __TCC_FB_ADDRESS__ 

 #ifndef ADDRESS_ALIGNED
#define ADDRESS_ALIGNED
#define ALIGN_BIT (0x8-1)
#define BIT_0 3
#define GET_ADDR_YUV42X_spY(Base_addr) 		(((((unsigned int)Base_addr) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV42X_spU(Yaddr, x, y) 		(((((unsigned int)Yaddr+(x*y)) + ALIGN_BIT)>> BIT_0)<<BIT_0)
#define GET_ADDR_YUV422_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/2)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#define GET_ADDR_YUV420_spV(Uaddr, x, y) 		(((((unsigned int)Uaddr+(x*y/4)) + ALIGN_BIT) >> BIT_0)<<BIT_0)
#endif

									
void tccxxx_GetAddress(unsigned char format, unsigned int base_Yaddr, unsigned int src_imgx, unsigned int  src_imgy,
									unsigned int start_x, unsigned int start_y, unsigned int* Y, unsigned int* U,unsigned int* V);

#endif
