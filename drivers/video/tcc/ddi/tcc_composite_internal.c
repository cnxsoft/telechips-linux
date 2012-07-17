/*
 * linux/drivers/video/tcc_composite_internal.c
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
#include <asm/io.h>
#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <mach/bsp.h>
#include "tcc_composite.h"
#include "tcc_composite_internal.h"

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tcc_composite_internal: " msg); }

void internal_tve_set_mode(unsigned int type)
{
	volatile PPMU pHwPMU = (volatile PPMU)tcc_p2v(HwPMU_BASE);
	volatile PNTSCPAL pHwTVE = (volatile PNTSCPAL)tcc_p2v(HwTVE_BASE);
	
#if defined(CONFIG_ARCH_TCC92XX)
	pHwPMU->PWROFF &= ~Hw0; /* Video DAC */
#else
	pHwPMU->PWROFF |= Hw0;  /* Video DAC */
#endif

	switch(type)
	{
		case NTSC_M:
			pHwTVE->ECMDA  = 
				HwTVECMDA_PWDENC_PD 			|	// [7]	 Power down mode for entire digital logic of TV encoder
				HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_M_J:
			pHwTVE->ECMDA  = 
				HwTVECMDA_PWDENC_PD 			|	// [7]	 Power down mode for entire digital logic of TV encoder
				HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_NO_PEDESTAL			|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_N:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_N_J:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_NO_PEDESTAL			|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_443:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALX			|	// [5:4] Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case PAL_M:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALM			|	// [5:4] Color subcarrier frequency is 3.57561149 MHz for PAL-M
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PAL_N:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALCN			|	// [5:4] Color subcarrier frequency is 3.58205625 MHz for PAL-combination N
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PAL_B:
		case PAL_G:
		case PAL_H:
		case PAL_I:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
				HwTVECMDA_FSCSEL_PALX			|	// [5:4] Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PSEUDO_PAL:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALX			|	// [5:4] Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PSEUDO_NTSC:
			pHwTVE->ECMDA  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;
	}
}

void internal_tve_set_config(unsigned int type)
{
	volatile PNTSCPAL pHwTVE = (volatile PNTSCPAL)tcc_p2v(HwTVE_BASE);
	volatile PNTSCPALOP pHwTVE_VEN = (volatile PNTSCPALOP)tcc_p2v(HwTVE_VEN_BASE);
	
	//Disconnect LCDC with NTSC/PAL encoder
	BITCLR(pHwTVE_VEN->VENCON, HwTVEVENCON_EN_EN);
		
	//Set ECMDA Register
	internal_tve_set_mode(type);

	//Set DACSEL Register
	BITSET(pHwTVE->DACSEL, HwTVEDACSEL_DACSEL_CVBS);
	//Set DACPD Register
	BITSET(pHwTVE->DACPD, HwTVEDACPD_PD_EN);

	BITSET(pHwTVE->ICNTL, HwTVEICNTL_VSIP_HIGH);
	BITSET(pHwTVE->ICNTL, HwTVEICNTL_HSVSP_RISING);
#ifdef TCC_COMPOSITE_CCIR656
	BITCSET(pHwTVE->ICNTL, HwTVEICNTL_ISYNC_MASK, HwTVEICNTL_ISYNC_ESAV_F);
#else
	BITCSET(pHwTVE->ICNTL, HwTVEICNTL_ISYNC_MASK, HwTVEICNTL_ISYNC_HVSI);
#endif
		
	//Set the Vertical Offset
	BITCSET(pHwTVE->HVOFFST, 0x07, ((0 & 0x700)>>8));
	pHwTVE->HOFFST = (0 & 0xFF);
			
	//Set the Horizontal Offset
	BITCSET(pHwTVE->HVOFFST, 0x08, ((1 & 0x100)>>5));
	pHwTVE->VOFFST = (1 & 0xFF);
			
	//Set the Digital Output Format
	BITCSET(pHwTVE->HVOFFST, HwTVEHVOFFST_INSEL_MASK, HwTVEHVOFFST_INSEL(2));
			
	//Set HSVSO Register
	BITCSET(pHwTVE->HSVSO, 0x07, ((0 & 0x700)>>8));
	pHwTVE->HSOE = (0 & 0xFF);
	BITCSET(pHwTVE->HSVSO, 0x38, ((0 & 0x700)>>5));
	pHwTVE->HSOB = (0 & 0xFF);
	BITCSET(pHwTVE->HSVSO, 0x40, ((0 & 0x100)>>2));
	pHwTVE->VSOB = (0 & 0xFF);

	//Set VSOE Register
	BITCSET(pHwTVE->VSOE, 0x1F, (0 & 0x1F));
	BITCSET(pHwTVE->VSOE, 0xC0, (0 & 0x03)<<6);
	BITCSET(pHwTVE->VSOE, 0x20, (0 & 0x01)<<5);
			
	//Set the Connection Type
	BITSET(pHwTVE_VEN->VENCIF, HwTVEVENCIF_FMT_1);
}

void internal_tve_enable(unsigned int type, unsigned int onoff)
{
	volatile PNTSCPAL pHwTVE = (volatile PNTSCPAL)tcc_p2v(HwTVE_BASE);
	volatile PNTSCPALOP pHwTVE_VEN = (volatile PNTSCPALOP)tcc_p2v(HwTVE_VEN_BASE);
	
	if(onoff)
	{
		internal_tve_set_config(type);
		BITSET(pHwTVE_VEN->VENCON, HwTVEVENCON_EN_EN);
		BITCLR(pHwTVE->DACPD, HwTVEDACPD_PD_EN);
		BITCLR(pHwTVE->ECMDA, HwTVECMDA_PWDENC_PD);
	}
	else
	{
		BITCLR(pHwTVE_VEN->VENCON, HwTVEVENCON_EN_EN);
		BITSET(pHwTVE->DACPD, HwTVEDACPD_PD_EN);
		BITSET(pHwTVE->ECMDA, HwTVECMDA_PWDENC_PD);
	}
}

