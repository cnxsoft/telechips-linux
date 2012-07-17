/*
 * Copyright (c) 2011 Telechips, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _STRUCTURES_DISPLAY_H_
#define _STRUCTURES_DISPLAY_H_

/*
	Not finished block "VIOC, NTSC/PAL, HDMI"
*/

/*******************************************************************************
*
*    TCC892x DataSheet PART 7 DISPLAY BUS
*
********************************************************************************/

/************************************************************************
*   3. VIOC (Video Input/Output Controller       (Base Addr = 0x72000000)
*************************************************************************/

typedef	struct
{
	unsigned		LEN		: 1;
	unsigned		SWAP	: 3;
	unsigned		RESERVE0	: 2;
	unsigned		SRST	: 1;
	unsigned		TV		: 1;
	unsigned		NI		: 1;
	unsigned		DP		: 1;
	unsigned		R2Y		: 1;
	unsigned		CLEN	: 1;
	unsigned		IP		: 1;
	unsigned		IH		: 1;
	unsigned		IV		: 1;
	unsigned		ID		: 1;
	unsigned		PXDW	: 4;
	unsigned		RESERVE1	: 2;
	unsigned		SREQ	: 1;
	unsigned		CKG		: 1;
	unsigned		C656	: 1;
	unsigned		RESERVE2	: 1;
	unsigned		ADVI	: 1;
	unsigned		RESERVE3	: 1;
	unsigned		R2YMD	: 2;
	unsigned		EVS		: 1;
	unsigned		EVP		: 1;
}	VIOC_DISP_LCTRL;

typedef	union
{
	unsigned long			nREG;
	VIOC_DISP_LCTRL		bREG;
}	VIOC_DISP_LCTRL_u;

typedef	struct
{
	unsigned 		BG0		: 8;
	unsigned 		BG1		: 8;
	unsigned 		BG2		: 8;
	unsigned 		BG3		: 8;
}	VIOC_DISP_BG;

typedef	union
{
	unsigned long			nREG;
	VIOC_DISP_BG		bREG;
}	VIOC_DISP_BG_u;

typedef	struct
{
	unsigned 		PXCLKDIV	: 8; /* PXCLK = LCLK/(2*PXCLKDIV) LCLK : Display Device Pixel clock form CKC Block, PXCLK : Pixel Clock to the External Display Device */
	unsigned 		RESERVE0	: 8;
	unsigned 		ACDIV   		: 8;
	unsigned 		RESERVE1	: 8;
}	VIOC_DISP_LCLKDIV;

typedef	union
{
	unsigned long			nREG;
	VIOC_DISP_LCLKDIV	bREG;
}	VIOC_DISP_LCLKDIV_u;

typedef	struct
{
	unsigned 		LPC		:14;
	unsigned 		RESERVE0	: 2;
	unsigned 		LPW		: 9;
	unsigned 		RESERVE1	: 7;
}	VIOC_DISP_LHTIME1;

typedef	union
{
	unsigned long			nREG;
	VIOC_DISP_LHTIME1	bREG;
}	VIOC_DISP_LHTIME1_u;

typedef	struct
{
	unsigned		LEWC	: 9;
	unsigned		RESERVE0	: 7;
	unsigned		LSWC	: 9;
	unsigned		RESERVE1	: 7;
}	VIOC_DISP_LHTIME2;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_LHTIME2	bREG;
}	VIOC_DISP_LHTIME2_u;

typedef	struct
{
	unsigned		FLC		:14;
	unsigned		RESERVE0	: 2;
	unsigned		FPW	: 6;
	unsigned		VDF		: 4;
	unsigned		RESERVE1	: 1;
	unsigned		VDB		: 5;
}	VIOC_DISP_LVTIME1;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_LVTIME1	bREG;
}	VIOC_DISP_LVTIME1_u;

typedef	struct
{
	unsigned		FEWC	: 9;
	unsigned		RESERVE0		: 7;
	unsigned		FSWC	: 9;
	unsigned		RESERVE1		: 7;
}	VIOC_DISP_LVTIME2;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_LVTIME2	bREG;
}	VIOC_DISP_LVTIME2_u;

typedef	struct
{
	unsigned		XPOS	:13;
	unsigned		RESERVE0   	: 3;
	unsigned		YPOS	:13;
	unsigned		RESERVE1   	: 3;
}	VIOC_DISP_LPOS;

typedef union
{
	unsigned	long		nREG;
	VIOC_DISP_LPOS		bREG;
}	VIOC_DISP_LPOS_u;

typedef	struct
{
	unsigned		RESERVE0		:30;
	unsigned		DSEL	: 1;
	unsigned		DEN 	: 1;
}	VIOC_DISP_DITHCTRL;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_DITHCTRL	bREG;
}	VIOC_DISP_DITHCTRL_u;

typedef	struct
{
	unsigned		CLPH	: 8;
	unsigned		RESERVE0		: 8;
	unsigned		CLPL	: 8;
	unsigned		RESERVE1		: 8;
}	VIOC_DISP_CLIP;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_CLIP		bREG;
}	VIOC_DISP_CLIP_u;

typedef	struct
{
	unsigned		HSIZE	: 13;
	unsigned		RESERVE0    	:  3;
	unsigned		VSIZE	: 13;
	unsigned		RESERVE1     	:  3;
}	VIOC_DISP_SIZE;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_SIZE		bREG;
}	VIOC_DISP_SIZE_u;

typedef	struct
{
	unsigned		CONRAST		: 8;
	unsigned		BRIGHTNESS	: 8;
	unsigned		HUE			: 8;
	unsigned		RESERVE0    	: 8;
}	VIOC_DISP_ENH;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_ENH		bREG;
}	VIOC_DISP_ENH_u;

typedef	struct
{
	unsigned		C0	:  4;
	unsigned		C1	:  4;
	unsigned		RESERVE0  	: 24;
}	VIOC_DISP_ADVI;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_ADVI		bREG;
}	VIOC_DISP_ADVI_u;

typedef	struct
{
	#if 0
	unsigned		MAT0	: 16;
	unsigned		MAT1	: 16;
	unsigned		MAT2	: 16;
	unsigned		MAT3	: 16;
	#else
	unsigned	DITH00	: 3;
	unsigned	RESERVE0	: 1;
	unsigned	DITH01	: 3;
	unsigned	RESERVE1	: 1;
	unsigned	DITH02	: 3;
	unsigned	RESERVE2	: 1;
	unsigned	DITH03	: 3;
	unsigned	RESERVE3	: 1;
	unsigned	DITH10	: 3;
	unsigned	RESERVE4	: 1;
	unsigned	DITH11	: 3;
	unsigned	RESERVE5	: 1;
	unsigned	DITH12	: 3;
	unsigned	RESERVE6	: 1;
	unsigned	DITH13	: 3;
	unsigned	RESERVE7	: 1;	

	unsigned	DITH20	: 3;
	unsigned	RESERVE8	: 1;
	unsigned	DITH21	: 3;
	unsigned	RESERVE9	: 1;
	unsigned	DITH22	: 3;
	unsigned	RESERVE10	: 1;
	unsigned	DITH23	: 3;
	unsigned	RESERVE11	: 1;
	unsigned	DITH30	: 3;
	unsigned	RESERVE12	: 1;
	unsigned	DITH31	: 3;
	unsigned	RESERVE13	: 1;
	unsigned	DITH32	: 3;
	unsigned	RESERVE14	: 1;
	unsigned	DITH33	: 3;
	unsigned	RESERVE15	: 1;	
	#endif
}	VIOC_DISP_DMAT;

typedef	union
{
	unsigned	long		nREG[2];
	VIOC_DISP_DMAT		bREG;
}	VIOC_DISP_DMAT_u;

typedef	struct
{
	unsigned		FU		:  1;	// fifo underrun
	unsigned		VSR		:  1;	// VSYNC rising
	unsigned		VSF		:  1;	// VSYNC falling
	unsigned		RU		:  1;	// Register Update
	unsigned		DD		:  1;	// Disable Done
	unsigned		SREQ	:  1;	// Stop Request
	unsigned		RESERVE0	: 10;
	unsigned		RESERVE1	: 12;
	unsigned		DEOF	:  1;	// Device EOF status
	unsigned		TFIELD	:  1;	// Top field indicator
	unsigned		BUSY	:  1;	// busy status
	unsigned		VS		:  1;	// VSYNC status
}	VIOC_DISP_STATUS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_STATUS	bREG;
}	VIOC_DISP_STATUS_u;

typedef	struct
{
	unsigned		FU		:  1;	// fifo underrun
	unsigned		VSR		:  1;	// VSYNC rising
	unsigned		VSF		:  1;	// VSYNC falling
	unsigned		RU		:  1;	// Register Update
	unsigned		DD		:  1;	// Disable Done
	unsigned		SREQ	:  1;	// Stop Request
	unsigned		RESERVE0	: 10;
	unsigned		RESERVE1	: 16;
}	VIOC_DISP_IRQMASK;

typedef	union
{
	unsigned	long		nREG;
	VIOC_DISP_IRQMASK	bREG;
}	VIOC_DISP_IRQMASK_u;

#define VIOC_DISP_IREQ_FU_MASK		0x00000001UL /*  fifo underrun */
#define VIOC_DISP_IREQ_VSR_MASK		0x00000002UL /*  VSYNC rising */
#define VIOC_DISP_IREQ_VSF_MASK 		0x00000004UL /* VSYNC falling */
#define VIOC_DISP_IREQ_RU_MASK 		0x00000008UL /* Register Update */
#define VIOC_DISP_IREQ_DD_MASK 		0x00000010UL /* Disable Done */
#define VIOC_DISP_IREQ_SREQ_MASK 		0x00000020UL /* Stop Request */

#define VIOC_DISP_IREQ_DEOF_MASK 		0x10000000UL
#define VIOC_DISP_IREQ_TFIELD_MASK 	0x20000000UL
#define VIOC_DISP_IREQ_BUSY_MASK 		0x40000000UL
#define VIOC_DISP_IREQ_VS_MASK 		0x80000000UL

typedef	struct _VIOC_DISP
{
	volatile VIOC_DISP_LCTRL_u		uCTRL;			// 0x000  R/W  0x30400000  Display Device Register
	volatile VIOC_DISP_BG_u		uBG;				// 0x004  R/W  0x00000000  Display Device Background Register
	volatile VIOC_DISP_LCLKDIV_u	uCLKDIV;			// 0x008  R/W  0x00000000  Display Device Clock Control Register
	volatile VIOC_DISP_LHTIME1_u	uLHTIME1;		// 0x00c  R/W  0x00000000  Display Device Horizontal Timing Register 1
	volatile VIOC_DISP_LHTIME2_u	uLHTIME2;		// 0x010  R/W  0x00000000  Display Device Horizontal Timing Register 2
	volatile VIOC_DISP_LVTIME1_u	uLVTIME1;		// 0x014	 R/W  0x00000000  Display Device Vertical Timing Register 1
	volatile VIOC_DISP_LVTIME2_u	uLVTIME2;		// 0x018	 R/W  0x00000000  Display Device Vertical Timing Register 2
	volatile VIOC_DISP_LVTIME1_u	uLVTIME3;		// 0x01C	 R/W  0x00000000  Display Device Vertical Timing Register 3
	volatile VIOC_DISP_LVTIME2_u	uLVTIME4;		// 0x020	 R/W  0x00000000  Display Device Vertical Timing Register 4
	volatile unsigned			reserved0[3];		// reserved
	volatile VIOC_DISP_LPOS_u		uLPOS;			// 0x030  R/W  0x00000000  Display Device Position Register 
	volatile unsigned			reserved1[3];		// reserved
	volatile VIOC_DISP_DITHCTRL_u	uDITHCTRL;			// 0x040  R/W  0x00000000  Display Device Dithering Register  
	volatile VIOC_DISP_CLIP_u		uCLIPY;			// 0x044  R/W  0x000000FF  Display Device Luma clipping Register 
	volatile VIOC_DISP_CLIP_u		uCLIPC;			// 0x048  R/W  0x000000FF  Display Device chroma Clipping Register
	volatile VIOC_DISP_SIZE_u		uLSIZE;			// 0x04c  R/W  0x00000000  Display Device Size Register
	volatile VIOC_DISP_STATUS_u	uLSTATUS;		// 0x050  R/W  0x00000000  Display Device Status Register
	volatile VIOC_DISP_IRQMASK_u	uLIRQMASK;				// 0x054  R/W  0x00000000  Display Device Interrupt Masking Register
	volatile unsigned			reserved2[6];		// reserved
	volatile VIOC_DISP_ENH_u		uLENH;			// 0x070  R/W  0x00000020  Display Device Color Enhancement Register
	volatile VIOC_DISP_ADVI_u		uADVI;			// 0x074  R/W  0x00000000  Display Device Interlacer Coefficient Register
	volatile VIOC_DISP_DMAT_u		uDMAT;			// 0x078  R/W  0x00000000  Display Device Dithering Matrix Register 0, 1
} VIOC_DISP,*PVIOC_DISP;

typedef struct LCDCDEFAULT
{
	unsigned   evp;    // External VSYNC Polarity,		   [0:Direct Input 1:Inverted Input]
	unsigned   evs;    // External VSYNC Enable,		   [0:Disabled	   1:Enabled]
	unsigned   r2ymd;  // RGB to YCbCr Conversion Option,  [0: 1: 2: 3:]
	unsigned   advi;   // Advanced Interlaced Mode
	unsigned   dtype;  // LCD DMA Type					   [0: AHB DDIC on, 1:AXI DDIC Off]
	unsigned   gen;    // Gamma Correction Enable Bit,	   [0:Disabled 1:Enabled]
	unsigned   ccir656;// CCIR 656 Mode,				   [0:Disable	1:Enable]
	unsigned   ckg;    // Clock Gating Enable for Timing   [0:cannot  1:can]
	unsigned   bpp;    // Bit Per Pixel for STN-LCD 	   [0:1bpp 1:2bpp 2:4bpp 3:RGB332 4:RGB444 5~7:reserved]
	unsigned   pxdw;   // Pixel Data Width, Refer to Datasheet
	unsigned   id;	   // Inverted Data Enable, 		   [0:Active High	1:Active Low]
	unsigned   iv;	   // Inverted Vertical Sync,		   [0:Active High	1:Active Low]
	unsigned   ih;	   // Inverted Horizontal Sync		   [0:Active High	1:Active Low]
	unsigned   ip;	   // Inverted pixel Clock, 		   [0:Rising Edge	1:Falling Edge]
	unsigned   clen;   // clipping Enable				   [0:Disable	1:Enable]
	unsigned   r2y;    // RGB to YCbCr Converter Enable    [0:Disable 1:Converted]
	unsigned   dp;	   // Double Pixel Data,			   [0: 1:]
	unsigned   ni;	   // Non-Interlaced.				   [0:Interlaced	1:non-interlaced]
	unsigned   tv;	   // TV Mode,						   [0: Normal mode	1:TV Mode]
	unsigned   opt;    // LCD DMA Operation mode		   [0:8 burst	1:16 burst]
	unsigned   stn;    // STN LCD mode, 				   [0:Normal	1:STN LCD]
	unsigned   evsel;  // External VSync Select,		   [0: External VSync	1:Internal Generated VSync] Removed
	unsigned   ovp;    // Overlay 0~5
} stLCDCTR;



typedef struct LDCTIMING
{
	// LHTIME1
	unsigned	lpw;	// Line Pulse Width, HSync width
	unsigned	lpc;	// Line Pulse Count, HActive width
	// LHTIME2
	unsigned	lswc;	// Line Start Wait Clock, HFront porch
	unsigned	lewc;	// Line End wait clock, HBack porch
	// LVTIME1
	unsigned	vdb;	// Back Porch Delay
	unsigned	vdf;	// Front Porch Delay
	unsigned	fpw;	// Frame Pulse Width, VSync Width
	unsigned	flc;	// Frame Line Count, VActive width
	// LVTIME2
	unsigned	fswc;	// Frame Start Wait Cycle
	unsigned	fewc;	// Frame End Wait Cycle
	// LVTIME3 [in Interlaced, even field timing, otherwise should be same with LVTIME1]
	unsigned	fpw2;	// Frame Pulse Width,
	unsigned	flc2;	// Frame Line count,
	// LVTIME4 [in Interlaced, even field timing, otherwise should be same with LVTIME2]
	unsigned	fswc2;	 // Frame Start Wait Cycle
	unsigned	fewc2;	 // Frame End Wait Cycle
} stLTIMING;

typedef struct LCDC_PARAM
{
	stLCDCTR	 LCDCTRL;
	stLTIMING	 LCDCTIMING;
} stLCDCPARAM;





/************************************************************************
*   3.6.3   VIDEO IN                         (Base Addr = 0x72004000)
*************************************************************************/

typedef struct
{
	unsigned		enable				:	 1;
	unsigned		conv_en				:	 1;
	unsigned		intl_en				:	 1;
	unsigned		intpl_en				: 	 1;
	unsigned		hsde_connect_en		:	 1;
	unsigned		flush					:	 1;
	unsigned		vs_mask				:	 1;
	unsigned		RESERVE0		:	 1;
	unsigned		pxclk_pol				:	 1;
	unsigned		hs_active_low			:	 1;
	unsigned		vs_active_low			:	 1;
	unsigned		field_bfield_low		:	 1;
	unsigned		de_active_low			:	 1;
	unsigned		gen_field_en			:	 1;
	unsigned		stream_en			:	 1;
	unsigned		RESERVE1		:	 1;
	unsigned		fmt					:	 4;
	unsigned		data_order			:	 3;
	unsigned		RESERVE2		:	 1;
	unsigned		skip					:	 4;
	unsigned		RESERVE3		:	 3;
	unsigned		capture				:	 1;
}	VIN_CTRL;

typedef union
{
	unsigned	long		nREG;
	VIN_CTRL				bVIN_CTRL;
}	VIN_CTRL_u;

typedef struct
{
	unsigned	lut0_en				:	 1;
	unsigned	lut1_en				:	 1;
	unsigned	lut2_en				:	 1;
	unsigned	lut_apbif_en		:	 1;
	unsigned	y2r_en				:	 1;
	unsigned	y2r_mode			:	 2;
	unsigned	RESERVE0			:   25;
}	VIN_MISC1;

typedef union
{
	unsigned	long		nREG;
	VIN_MISC1				bVIN_MISC1;
}	VIN_MISC1_u;

typedef struct
{
	unsigned	hblank				:	 4;
	unsigned	vblank				:	 4;
	unsigned	field_pos				:	 4;
	unsigned	RESERVE0			:	 4;
	unsigned	pre_sta_loc			:	 2;
	unsigned	sync_byte			:	 2;
	unsigned	RESERVE1			:	12;
}	VIN_SYNC_INFO0;

typedef union
{
	unsigned	long		nREG;
	VIN_SYNC_INFO0		bREG;
}	VIN_SYNC_INFO0_u;

typedef struct
{
	unsigned	pre_first			:	 8;
	unsigned	pre_second			:	 8;
	unsigned	pre_third			:	 8;
	unsigned	RESERVE0			:	 8;
}	VIN_SYNC_INFO1;

typedef union
{
	unsigned	long		nREG;
	VIN_SYNC_INFO1		bREG;
}	VIN_SYNC_INFO1_u;

typedef	struct
{
	unsigned	width				:	16;
	unsigned	height				:	16;
}	VIN_SIZE;

typedef union
{
	unsigned	long			nREG;
	VIN_SIZE				bVIN_SIZE;
}	VIN_SIZE_u;

typedef	struct
{
	unsigned	offs_width			:	16;
	unsigned	offs_height			:	16;
}	VIN_OFFS;

typedef union
{
	unsigned	long		nREG;
	VIN_OFFS				bVIN_OFFS;
}	VIN_OFFS_u;

typedef	struct
{
	unsigned	update_int				:	 1;
	unsigned	eof_int					:	 1;
	unsigned	vs_int					:	 1;
	unsigned	invs_int					:	 1;
	unsigned	RESERVE0			:	 7;	
	unsigned	frm_sq					:	 1;	
	unsigned RESERVE1			:	4;
	unsigned	update_int_mask			:	 1;
	unsigned	eof_int_mask				:	 1;
	unsigned	vs_int_mask				: 	 1;
	unsigned	invs_int_mask				: 	 1;
	unsigned	RESERVE2			: 	 11;	
	unsigned	int_en					:	 1;
}	VIN_INT;

typedef	union
{
	unsigned	long		nREG;
	VIN_INT				bVIN_INT;
}	VIN_INT_u;

typedef	struct
{
	unsigned	width_cnt			:	16;
	unsigned	height_cnt			:	16;
}	VIN_STAT_CNT;

typedef union
{
	unsigned	long		nREG;
	VIN_STAT_CNT			bREG;
}	VIN_STAT_CNT_u;


typedef struct _VIOC_VIN
{
	volatile VIN_CTRL_u			uVIN_CTRL;			// 	0x000  R/W  0x00000000 VIN Control Register
	volatile VIN_MISC1_u			uVIN_MISC1;			//	0x004  R/W  0x00000000 VIN Misc Register
	volatile VIN_SYNC_INFO0_u		uVIN_SYNC_INFO0;	//	0x008  R/W  0x00030CB9 VIN Sync Misc. 0 Register
	volatile VIN_SYNC_INFO1_u		uVIN_SYNC_INFO1;	//	0x00c  R/W  0x000000FF VIN Sync misc. 1 Register
	volatile VIN_SIZE_u			uVIN_SIZE;			//	0x010  R/W  0x00000000 VIN Size Register
	volatile VIN_OFFS_u			uVIN_OFFS;			//	0x014  R/W  0x00000000 VIN Offset Register
	volatile VIN_OFFS_u			uVIN_OFFS_INTL;		//	0x018  R/W  0x00000000 VIN Offset Interlaced Register
	unsigned	int				undef[17];			//	0x01c ~ 0x5c
	volatile VIN_INT_u				uVIN_INT;			//	0x060  R/W  0x00000000 VIN Iterrupt Register	
	//need to add  VIN LUT !!!
}VIOC_VIN,*PVIOC_VIN;


/************************************************************************
*   3.6.4   GRDMA  and VRDMA				(Base Addr = 0x72000400)
*************************************************************************/
typedef	struct
{
	unsigned		FMT		:  5;
	unsigned		RESERVE0   		:  2;
	unsigned		BR  	:  1;
	unsigned		Y2R 	:  1;
	unsigned		Y2RMD	:  2;
	unsigned		AEN  	:  1;
	unsigned		SWAP 	:  3;
	unsigned		PD   	:  1;
	unsigned		UPD  	:  1;
	unsigned		RESERVE1    :  5;
	unsigned		NUVIH	:  1;
	unsigned		UVI  	:  1;
	unsigned		ASEL 	:  1;
	unsigned		RESERVE2  :  2;
	unsigned		STRM 	:  1;
	unsigned		IEN  	:  1;
	unsigned		BFIELD	:  1;
	unsigned		BFMD	:  1;
	unsigned		INTL	:  1;
}	VIOC_RDMA_CTRL;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_CTRL		bREG;
} VIOC_RDMA_CTRL_u;

typedef	struct
{
	unsigned	TOP		: 16;
	unsigned	BOT		: 16;
} VIOC_RDMA_PTS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_PTS		bREG;
} VIOC_RDMA_PTS_u;

typedef	struct
{
	unsigned	WIDTH   	: 13;
	unsigned	RESERVE0	:  3;
	unsigned	HEIGHT   	: 13;
	unsigned	RESERVE1	:  3;
} VIOC_RDMA_SIZE;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_SIZE		bREG;
} VIOC_RDMA_SIZE_u;

typedef	struct
{
	unsigned	OFFSET0   	: 16;
	unsigned	OFFSET1   	: 16;
} VIOC_RDMA_OFFS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_OFFS		bREG;
} VIOC_RDMA_OFFS_u;

typedef	struct
{
	unsigned	ALPHA0   	:  8;
	unsigned	RESERVE0     :  8;
	unsigned	ALPHA1   	:  8;
	unsigned	RESERVE1     :  8;
} VIOC_RDMA_ALPHA;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_ALPHA		bREG;
} VIOC_RDMA_ALPHA_u;

typedef	struct
{
	unsigned	CFGDONE	:  1;	// update
	unsigned	EOFRISE		:  1;	// Device EOF Rising
	unsigned	EOFFALL	:  1;	// Device EOF Falling
	unsigned	UPDDONE	:  1;	// Register Update Done
	unsigned	EOFWAITR	:  1;	// EOF Wait Rising - Frame End
	unsigned	TOPRDY		:  1;	// Top Ready
	unsigned	BOTRDY		:  1;	// Bottom Ready
	unsigned	RESERVE0	:  9;
	unsigned	STS_TOPRDY :  1;	// TOP field Ready
	unsigned	STS_BOTRDY :  1;	// BOTTOM field Ready
	unsigned	STS_EOFWAIT:  1;	// RDMA eof-wait status
	unsigned	STS_DEVEOF	:  1;	// Device EOF
	unsigned	STS_BFIELD	:  1;	// Bottom field indicator
	unsigned	RESERVE1	:  7;
	unsigned	STS_FDLY	:  4;	// frame delay
} VIOC_RDMA_STATUS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_STATUS	bREG;
} VIOC_RDMA_STATUS_u;

typedef	struct
{
	unsigned	CFGDONE	:  1;	// update
	unsigned	DEOFR	:  1;	// Device EOF Rising
	unsigned	DEOFF	:  1;	// Device EOF Falling
	unsigned	UPDDONE	:  1;	// Register Update Done
	unsigned	EOFWAITR:  1;	// EOF Wait Rising - Frame End
	unsigned	TOPRDY	:  1;	// Top Ready
	unsigned	BOTRDY	:  1;	// Bottom Ready
	unsigned	RESERVE0		:  9;
	unsigned	RESERVE1		: 16;
} VIOC_RDMA_IRQMSK;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_IRQMSK	bREG;
} VIOC_RDMA_IRQMSK_u;

typedef	struct
{
	unsigned	XSCALE	: 3;
	unsigned	RESERVE0		:13;
	unsigned	YSCALE	: 3;
	unsigned	RESERVE1		:13;
} VIOC_RDMA_SCALE;

typedef	union
{
	unsigned	long		nREG;
	VIOC_RDMA_SCALE		bREG;
} VIOC_RDMA_SCALE_u;

typedef	struct _VIOC_RDMA
{
	volatile VIOC_RDMA_CTRL_u	uCTRL;				// 0x00  R/W  0x00000000  RDMA Control Register
	volatile VIOC_RDMA_PTS_u	uPTS;				// 0x04  R/W  0x00000000  RDMA Image PTS Register
	volatile VIOC_RDMA_SIZE_u	uSIZE;				// 0x08  R/W  0x00000000  RDMA Image Size information Register 
	volatile unsigned  int          	nBASE0;			// 0x0C  R/W  0x00000000  RDMA Base0 Address for Each Images Register
	volatile unsigned  int 		nCBASE;			// 0x10  R/W	 0x00000000  RDMA Current Address for Each Images Register
	volatile unsigned  int		nBASE1;			// 0x14  R/W  0x00000000  RDMA Base1 Address for Each Images Register
	volatile unsigned  int		nBASE2;			// 0x18  R/W  0x00000000  RDMA Base2 Address for Each Images Register
	volatile VIOC_RDMA_OFFS_u  	uOFFSET;		// 0x1C  R/W  0x00000000  RDMA Offset Information for Each Images Register
	volatile VIOC_RDMA_SCALE_u	uSCALE;			// 0x20  R/W  0x00000000  RDMA Scale for Each images Register
	volatile VIOC_RDMA_ALPHA_u 	uALPHA;			// 0x24  R/W  0x00000000  RDMA Alpha Information for Each Imgaes Register
	volatile VIOC_RDMA_STATUS_u	uSTATUS;		// 0x28  R/W	 0x00000000  RDMA Status Register
	volatile VIOC_RDMA_IRQMSK_u	uIRQMSK;		// 0x2C  R/W  0x0000007F  RDMA interrupt mask Register
	volatile unsigned  int			nSBASE0;		// 0x30  R/W  0x00000000  RDMA Sync base address
	volatile unsigned  int			reserved1;		// 
} VIOC_RDMA, *PVIOC_RDMA;


/************************************************************************
*   3.6.5   GWDMA  and VWDMA				(Base Addr = 0x72002800)
*************************************************************************/

typedef	struct
{
	unsigned	FMT		:  5;
	unsigned	RESERVE0  :  2;
	unsigned	BR  	:  1;
	unsigned	R2Y 	:  1;
	unsigned	R2YMD	:  2;
	unsigned	RESERVE1  :  1;
	unsigned	SWAP 	:  3;
	unsigned	RESERVE2     :  1;
	unsigned	UPD  	:  1;
	unsigned	RESERVE3     :  5;
	unsigned	SREQ  	:  1;
	unsigned	CONT 	:  1;
	unsigned	DITHEN	:  1;
	unsigned	RESERVE4  :  2;
	unsigned	DITHSEL	:  1;
	unsigned	IEN  	:  1;
	unsigned	FU		:  1;
	unsigned	RESERVE5 :  1;
	unsigned	INTL	:  1;
}	VIOC_WDMA_CTRL;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_CTRL		bREG;
}	VIOC_WDMA_CTRL_u;

typedef	struct
{
	unsigned	SYNCSEL :  8;
	unsigned	SEN		:  1;
	unsigned	SYNCMD_ADDR	:  1;
	unsigned	RESERVE0		:  1;
	unsigned	SYNCMD_SENS	:  1;
	unsigned	RESERVE1		:  4;
	unsigned	MAXRATE		:  8;
	unsigned	RESERVE2		:  7;
	unsigned	REN			:  1;
}	VIOC_WDMA_RATE;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_RATE		bREG;
}	VIOC_WDMA_RATE_u;

typedef	struct
{
	unsigned	WIDTH   	: 13;
	unsigned	RESERVE0	:  3;
	unsigned	HEIGHT   	: 13;
	unsigned	RESERVE1	:  3;
}	VIOC_WDMA_SIZE;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_SIZE		bREG;
}	VIOC_WDMA_SIZE_u;

typedef	struct
{
	unsigned	OFFSET0   	: 16;
	unsigned	OFFSET1   	: 16;
}	VIOC_WDMA_OFFS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_OFFS		bREG;
}VIOC_WDMA_OFFS_u;

typedef	struct
{
	unsigned	BG0		: 8;
	unsigned	BG1		: 8;
	unsigned	BG2		: 8;
	unsigned	BG3		: 8;
}VIOC_WDMA_BG;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_BG		bREG;
}VIOC_WDMA_BG_u;

typedef	struct
{
	unsigned	PTS		: 16;
	unsigned	RESERVE0    	: 16;
}VIOC_WDMA_PTS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_PTS		bREG;
}VIOC_WDMA_PTS_u;

typedef	struct
{
	#if 0
	unsigned		MAT0	: 16;
	unsigned		MAT1	: 16;
	unsigned		MAT2	: 16;
	unsigned		MAT3	: 16;
	#else
	unsigned	DITH00	: 3;
	unsigned	RESERVE0	: 1;
	unsigned	DITH01	: 3;
	unsigned	RESERVE1	: 1;
	unsigned	DITH02	: 3;
	unsigned	RESERVE2	: 1;
	unsigned	DITH03	: 3;
	unsigned	RESERVE3	: 1;
	unsigned	DITH10	: 3;
	unsigned	RESERVE4	: 1;
	unsigned	DITH11	: 3;
	unsigned	RESERVE5	: 1;
	unsigned	DITH12	: 3;
	unsigned	RESERVE6	: 1;
	unsigned	DITH13	: 3;
	unsigned	RESERVE7	: 1;	

	unsigned	DITH20	: 3;
	unsigned	RESERVE8	: 1;
	unsigned	DITH21	: 3;
	unsigned	RESERVE9	: 1;
	unsigned	DITH22	: 3;
	unsigned	RESERVE10	: 1;
	unsigned	DITH23	: 3;
	unsigned	RESERVE11	: 1;
	unsigned	DITH30	: 3;
	unsigned	RESERVE12	: 1;
	unsigned	DITH31	: 3;
	unsigned	RESERVE13	: 1;
	unsigned	DITH32	: 3;
	unsigned	RESERVE14	: 1;
	unsigned	DITH33	: 3;
	unsigned	RESERVE15	: 1;	
	#endif

}VIOC_WDMA_DMAT;

typedef	union
{
	unsigned	long		nREG[2];
	VIOC_WDMA_DMAT	bREG;
}VIOC_WDMA_DMAT_u;

typedef	struct
{
	unsigned	CONTRAST: 8;
	unsigned	BRIGHT	: 8;
	unsigned	HUE		: 8;
	unsigned	RESERVE0  : 8;
}VIOC_WDMA_ENH;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_ENH		bREG;
}VIOC_WDMA_ENH_u;

typedef	struct
{
	unsigned	ROLLCNT   	: 16;
	unsigned	RESERVE0     : 15;
	unsigned	ROLL     	:  1;
}VIOC_WDMA_ROLL;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_ROLL		bREG;
}VIOC_WDMA_ROLL_u;

typedef	struct
{
	unsigned	UPDDONE	:  1;	// Register Update Done
	unsigned	STOPREQ	:  1;	// Stop Request
	unsigned	ROLL 		:  1;	// Roll Interrupt
	unsigned	ENRISE		:  1;	// Frame Synchronized Enable Rising
	unsigned	ENFALL		:  1;	// Frame Synchronized Enable Falling (Disable-Done)
	unsigned	EOFRISE		:  1;	// EOF Rising
	unsigned	EOFFALL	:  1;	// EOF Falling
	unsigned	SEOFRISE	:  1;	// SYNC EOF Rising
	unsigned	SEOFFALL	:  1;	// SYNC EOF Falling
	unsigned	RESERVE0	:  7;
	unsigned	RESERVE1	: 13;
	unsigned	STS_SEN		:  1;	// Synchronized Enable
	unsigned	STS_BFIELD	:  1;	// Bottom field indicator
	unsigned	STS_EOF	:  1;	// EOF
}VIOC_WDMA_STATUS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_STATUS	bREG;
}VIOC_WDMA_STATUS_u;

typedef	struct
{
	unsigned	UPDDONE	:  1;	// Register Update Done
	unsigned	STOPREQ	:  1;	// Stop Request
	unsigned	ROLL 		:  1;	// Roll Interrupt
	unsigned	ENRISE		:  1;	// Synchronized Enable Rising
	unsigned	ENFALL		:  1;	// Synchronized Enable Falling (Disable-Done)
	unsigned	EOFRISE		:  1;	// EOF Rising
	unsigned	EOFFALL	:  1;	// EOF Falling
	unsigned	SEOFRISE	:  1;	// SYNC EOF Rising
	unsigned	SEOFFALL	:  1;	// SYNC EOF Falling
	unsigned	RESERVE0	:  7;
	unsigned	RESERVE1	: 16;
}VIOC_WDMA_IRQMSK;

typedef	union
{
	unsigned	long		nREG;
	VIOC_WDMA_IRQMSK	bREG;
}VIOC_WDMA_IRQMSK_u;

typedef	struct _VIOC_WDMA
{
	volatile VIOC_WDMA_CTRL_u	uCTRL;				// 0x00  R/W  0x00000400 WMDA Control Register
	volatile VIOC_WDMA_RATE_u	uRATE;				// 0x04  R/W  0x00000000 WDMA Rate Control Register
	volatile VIOC_WDMA_SIZE_u		uSIZE;				// 0x08  R/W  0x00000000 WDMA Size Register
	volatile unsigned  int           	uBASE0;				// 0x0C  R/W  0x00000000 WDMA Base Address 0 Register
	volatile unsigned  int		uCBASE;				// 0x10  R/W  0x00000000 WDMA Current Address Register
	volatile unsigned  int			uBASE1;				// 0x14  R/W  0x00000000 WDMA Base Address 1 Register
	volatile unsigned  int			uBASE2;				// 0x18  R/W  0x00000000 WDMA Base Address 2 Register
	volatile VIOC_WDMA_OFFS_u  	uOFFS;				// 0x1C  R/W  0x00000000 WDMA Offset Register
	volatile unsigned  int			reserved0;	// 0x20
	volatile VIOC_WDMA_BG_u		uBG;					// 0x24  R/W  0x00000000 WDMA Back Ground Color Register
	volatile VIOC_WDMA_PTS_u		uPTS;				// 0x28  R/W  0x00000000 WDMA PTS Register
	volatile VIOC_WDMA_DMAT_u	uDMAT;				// 0x2C  R/W  0x00000000 WDMA Dither Matrix 0,1 Register
	volatile VIOC_WDMA_ENH_u		uENH;				// 0x34  R/W  0x00000000 WDMA Color Enhancement Register
	volatile VIOC_WDMA_ROLL_u 	uROLL;				// 0x38  R/W  0x00000000 WDMA Rolling Control Register
	volatile unsigned  int			uSBASE;				// 0x3C  R/W  0x00000000 WMA Synchronized Base Address Register
	volatile VIOC_WDMA_STATUS_u	uIRQSTS;			// 0x40  R/W  0x00000000 WDMA Interrupt Status Register
	volatile VIOC_WDMA_IRQMSK_u	uIRQMSK;			// 0x44  R/W  0x000001FF WDMA Interrupt Mask Register
}VIOC_WDMA, *PVIOC_WDMA;


/************************************************************************
*   3.6.6   Scaler				(Base Addr = 0x72002000)
*************************************************************************/

typedef	struct
{
	unsigned	BP		:  1;
	unsigned	RESERVE0: 15;
	unsigned	UPD 	:  1;
	unsigned	RESERVE1     : 15;
} VIOC_SC_CTRL;

typedef	union
{
	unsigned	long			nREG;
	VIOC_SC_CTRL		bREG;
} VIOC_SC_CTRL_u;

typedef	struct
{
	unsigned	WIDTH   	: 13;
	unsigned	RESERVE0	:  3;
	unsigned	HEIGHT   	: 13;
	unsigned	RESERVE1	:  3;
} VIOC_SC_SIZE;

typedef	union
{
	unsigned	long			nREG;
	VIOC_SC_SIZE		bREG;
} VIOC_SC_SIZE_u;

typedef	struct
{
	unsigned	XPOS    	: 13;
	unsigned	RESERVE0:  3;
	unsigned	YPOS     	: 13;
	unsigned	RESERVE1:  3;
} VIOC_SC_POS;

typedef	union
{
	unsigned	long			nREG;
	VIOC_SC_POS			bREG;
} VIOC_SC_POS_u;

typedef	struct
{
	unsigned	UPDDONE	:  1;	// Register Update Done
	unsigned	EOFRISE	:  1;	// EOF Rising
	unsigned	EOFFALL	:  1;	// EOF Falling
	unsigned	ERR		:  1;		// Error
	unsigned	RESERVE0		: 12;
	unsigned	RESERVE1		: 16;
} VIOC_SC_STATUS;

typedef	union
{
	unsigned	long		nREG;
	VIOC_SC_STATUS	bREG;
} VIOC_SC_STATUS_u;

typedef	struct
{
	unsigned	UPDDONE	:  1;	// Register Update Done
	unsigned	EOFRISE	:  1;	// EOF Rising
	unsigned	EOFFALL	:  1;	// EOF Falling
	unsigned	ERR	:  1;	// Error
	unsigned	RESERVE0		: 12;
	unsigned	RESERVE1		: 16;
} VIOC_SC_IRQMSK;

typedef	union
{
	unsigned	long			nREG;
	VIOC_SC_IRQMSK	bREG;
} VIOC_SC_IRQMSK_u;

#define VIOC_SC_INT_MASK_UPDDONE 		0x00000001UL /*Status of Register Updated*/
#define VIOC_SC_INT_MASK_EOFRISE 		0x00000002UL /*Rising EOF Interrupt Masked*/
#define VIOC_SC_INT_MASK_EOFFALL 		0x00000004UL /*Falling EOF Interrupt Masked*/
#define VIOC_SC_INT_MASK_ERROR  		0x00000008UL /*Scaler Error Interrupt Masked*/
#define VIOC_SC_INT_MASK_ALL 			0x0000000FUL /*ALL*/

#define VIOC_SC_IREQ_UPDDONE_MASK 		0x00000001UL
#define VIOC_SC_IREQ_EOFRISE_MASK 		0x00000002UL
#define VIOC_SC_IREQ_EOFFALL_MASK  		0x00000004UL
#define VIOC_SC_IREQ_ERROR_MASK  		0x00000008UL

typedef	struct _VIOC_SC
{
	volatile VIOC_SC_CTRL_u		uCTRL;				// 0x00  R/W  0x00000000 Scaler Control Register
	volatile unsigned int			reserved0;			// 0x04
	volatile VIOC_SC_SIZE_u		uSRCSIZE;			// 0x08  R       -              Scaler Source Size Register
	volatile VIOC_SC_SIZE_u		uDSTSIZE;			// 0x0C  R/W  0x00000000 Scaler Destination Size Register
	volatile VIOC_SC_POS_u 		uOUTPOS;			// 0x10  R/W  0x00000000 Scaler Output Position Register
	volatile VIOC_SC_SIZE_u		uOUTSIZE;			// 0x14  R/W  0x00000000 Scaler Output Size Register
	volatile VIOC_SC_STATUS_u		uSTATUS;			// 0x18  R/W  0x00000000 Scaler Interrupt Status Register
	volatile VIOC_SC_IRQMSK_u		uIRQMSK;			// 0x1C  R/W  0x0000000F Scaler Interrupt Mask Register
} VIOC_SC,*PVIOC_SC;



/************************************************************************
*   3.6.7   Frame Delay				(Base Addr = 0x72003A00)
*************************************************************************/

typedef	struct
{
	unsigned				FMT		:  1;
	unsigned 				   		: 15;
	unsigned 				     	: 16;
}	VIOC_FDLY_CTRL;

typedef	union
{
	unsigned	long			nREG;
	VIOC_FDLY_CTRL		bREG;
}	VIOC_FDLY_CTRL_u;

typedef	struct
{
	unsigned 						: 16;
	unsigned				MAXRATE	:  8;
	unsigned 						:  7;
	unsigned				REN		:  1;
}	VIOC_FDLY_RATE;

typedef	union
{
	unsigned	long			nREG;
	VIOC_FDLY_RATE		bREG;
}	VIOC_FDLY_RATE_u;

typedef	struct
{
	unsigned				BG0		: 8;
	unsigned				BG1		: 8;
	unsigned				BG2		: 8;
	unsigned				BG3		: 8;
}	VIOC_FDLY_BG;

typedef	union
{
	unsigned	long			nREG;
	VIOC_FDLY_BG		bREG;
}	VIOC_FDLY_BG_u;

typedef	struct _VIOC_FDLY
{
	VIOC_FDLY_CTRL_u	uCTRL;			// 0x00  R/W 
	VIOC_FDLY_RATE_u	uRATE;			// 0x04
	unsigned				uBASE0;			// 0x08
	unsigned				uBASE1;			// 0x0C
	VIOC_FDLY_BG_u		uBG;				// 0x10
	unsigned	int			reserved0[3];		// 5,6,7
}	VIOC_FDLY,*PVIOC_FDLY;



/************************************************************************
*   3.6.8   WMIX				(Base Addr = 0x72001800)
*************************************************************************/


typedef	struct
{
	unsigned		OVP		:  5;
	unsigned    		:  3;
	unsigned 			:  2; /*AOPT*/
	unsigned 			:  1;/*AOPT1*/
	unsigned    		:  5;
	unsigned		UPD 	:  1;
	unsigned 		     	: 15;
} VIOC_WMIX_CTRL;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_CTRL		bREG;
} VIOC_WMIX_CTRL_u;

typedef	struct
{
	unsigned	BG0		: 8;
	unsigned	BG1		: 8;
	unsigned	BG2		: 8;
	unsigned	BG3		: 8;
} VIOC_WMIX_BG;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_BG		bREG;
} VIOC_WMIX_BG_u;

typedef	struct
{
	unsigned	WIDTH    	: 13;
	unsigned 				:  3;
	unsigned	HEIGHT     	: 13;
	unsigned 				:  3;
} VIOC_WMIX_SIZE;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_SIZE		bREG;
} VIOC_WMIX_SIZE_u;

typedef	struct
{
	unsigned	XPOS    	: 13;
	unsigned 			:  3;
	unsigned	YPOS     	: 13;
	unsigned 			:  3;
} VIOC_WMIX_POS;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_POS		bREG;
} VIOC_WMIX_POS_u;

typedef	struct
{
	unsigned	KEYB    	:  8;
	unsigned	KEYG		:  8;
	unsigned	KEYR     	:  8;
	unsigned 				:  7;
	unsigned	KEYEN    	:  1;
	unsigned	KEYMB    	:  8;
	unsigned	KEYMG		:  8;
	unsigned	KEYMR     	:  8;
	unsigned 				:  8;
} VIOC_WMIX_KEY;

typedef	union
{
	unsigned	long			nREG[2];
	VIOC_WMIX_KEY		bREG;
} VIOC_WMIX_KEY_u;

typedef	struct
{
	unsigned	UPDDONE		:  1;	// Register Update Done
	unsigned	EOFWAITFALL	:  1;	// EOF Rising
	unsigned	EOFWAITRISE	:  1;	// EOF Falling
	unsigned	EOFRISE		:  1;	// Error
	unsigned	EOFFALL		:  1;	// Error
	unsigned	RESERVE0			: 11;
	unsigned	RESERVE1			:  8;
	unsigned	UP_EOF    	:  1;	// Source
	unsigned	UP_ENABLE 	:  1;	// Source
	unsigned	UP_UPDATE 	:  1;	// Source
	unsigned	UP_INTL   	:  1;	// Source
	unsigned	DN_EOFWAIT	:  1;	// Source
	unsigned	DN_ENABLE	:  1;	// Source
	unsigned	DN_BFIELD	:  1;	// Source
	unsigned	DN_INTL		:  1;	// Source
} VIOC_WMIX_STATUS;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_STATUS	bREG;
} VIOC_WMIX_STATUS_u;

typedef	struct
{
	unsigned	UPDDONE		:  1;	// Register Update Done
	unsigned	EOFWAITFALL	:  1;	// EOF Rising
	unsigned	EOFWAITRISE	:  1;	// EOF Falling
	unsigned	EOFRISE		:  1;	// Error
	unsigned	EOFFALL		:  1;	// Error
	unsigned	RESERVE0		: 11;
	unsigned	RESERVE1		: 16;
} VIOC_WMIX_IRQMSK;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_IRQMSK	bREG;
} VIOC_WMIX_IRQMSK_u;

/*WMIX ALPHA CONTROL*/
typedef	struct
{
	unsigned	ACON0_00		:  3;
	unsigned	RESERVE0   		:  1;
	unsigned	ACON1_00		:  3;
	unsigned	RESERVE1   		:  1;
	unsigned	ACON0_01		:  3;
	unsigned	RESERVE2   		:  1;
	unsigned	ACON1_01		:  3;
	unsigned	RESERVE3   		:  1;
	unsigned	ACON0_10		:  3;
	unsigned	RESERVE4   		:  1;
	unsigned	ACON1_10		:  3;
	unsigned	RESERVE5   		:  1;
	unsigned	ACON0_11		:  3;
	unsigned	RESERVE6   		:  1;
	unsigned	ACON1_11		:  3;
	unsigned	RESERVE7   		:  1;
} VIOC_WMIX_MACON;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_MACON		bREG;
} VIOC_WMIX_MACON_u;

typedef	struct
{
	unsigned	CCON0_00		:  4;
	unsigned	CCON1_00		:  4;
	unsigned	CCON0_01		:  4;
	unsigned	CCON1_01		:  4;
	unsigned	CCON0_10		:  4;
	unsigned	CCON1_10		:  4;
	unsigned	CCON0_11		:  4;
	unsigned	CCON1_11		:  4;
} VIOC_WMIX_MCCON;

typedef	union
{
	unsigned	long				nREG;
	VIOC_WMIX_MCCON		bREG;
} VIOC_WMIX_MCCON_u;

typedef	struct
{
	unsigned	RESERVE0		:  3;
	unsigned	ROPMODE		:  2;
	unsigned	RESERVE1		:  9;
	unsigned	ASEL			:  2;
	unsigned	ALPHA0			:  8;
	unsigned	ALPHA1			:  8;
} VIOC_WMIX_MROPC;

typedef	union
{
	unsigned	long				nREG;
	VIOC_WMIX_MROPC		bREG;
} VIOC_WMIX_MROPC_u;

typedef	struct
{
	unsigned	BLUE		:  8;
	unsigned	GREEN		:  8;
	unsigned	RED			:  8;
	unsigned	RESERVE0	:  8;
} VIOC_WMIX_MPAT;

typedef	union
{
	unsigned	long			nREG;
	VIOC_WMIX_MPAT		bREG;
} VIOC_WMIX_MPAT_u;

typedef	struct _VIOC_WMIX
{
	volatile VIOC_WMIX_CTRL_u		uCTRL;			// 0x00  R/W  0x00000005 WMIX Control Register
	volatile VIOC_WMIX_BG_u		uBG;			// 0x04  R/W  0x00000000 WMIX Background Color Register
	volatile VIOC_WMIX_SIZE_u		uSIZE;			// 0x08  R/W  0x00000000 WMIx Size Register
	volatile unsigned  int	reserved0;			// 0x0C
	volatile VIOC_WMIX_POS_u 		uPOS0;			// 0x10  R/W  0x00000000 WMIX Position 0 Register
	volatile VIOC_WMIX_POS_u 		uPOS1;			// 0x14  R/W  0x00000000 WMIX Position 1 Register
	volatile VIOC_WMIX_POS_u 		uPOS2;			// 0x18  R/W  0x00000000 WMIX Position 2 Register
	volatile VIOC_WMIX_POS_u 		uPOS3;			// 0x1C  R/W  0x00000000 WMIX Position 3 Register
	volatile VIOC_WMIX_KEY_u	uKEY0;			// 0x20  R/W  0x00000000 WMIX Chroma-Key 0,1 Image 0 Register
	volatile VIOC_WMIX_KEY_u	uKEY1;			// 0x28  R/W  0x00000000 WMIX Chroma-key 0,1 Image 1 Register
	volatile VIOC_WMIX_KEY_u	uKEY2;			// 0x30  R/W  0x00000000 WMIX Chroma-key 0,1 Image 2 Register
	volatile VIOC_WMIX_STATUS_u	uSTATUS;			// 0x38  R/W  0x00000000 WMIX Status Register
	volatile VIOC_WMIX_IRQMSK_u	uIRQMSK;		// 0x3C  R/W  0x0000001F WMIX Interrupt Masked Register

	// need to check
	volatile VIOC_WMIX_MACON_u	uACON0;			// 0x40  R/W  0x10622662 WMIX Alpha Control Register for 0'th Layer
	volatile VIOC_WMIX_MCCON_u	uCCON0;			// 0x44  R/W  0x20C44CC4 WMIX Color Control Register for 0'th Layer
	volatile VIOC_WMIX_MROPC_u	uROPC0;			// 0x48  R/W  0x0000C018 WMIX ROP Control Register for 0'th Layer
	volatile VIOC_WMIX_MPAT_u 	uPAT0;			// 0x4C  R/W  0x00000000 WMIX Patterm Register for 0'th Layer
	
	volatile VIOC_WMIX_MACON_u	uACON1;			// 0x50  R/W  0x10622662 WMIX Alpha Control Register for 1'th Layer
	volatile VIOC_WMIX_MCCON_u	uCCON1;			// 0x54  R/W  0x20C44CC4 WMIX Color Control Register for 1'th Layer
	volatile VIOC_WMIX_MROPC_u	uROPC1;			// 0x58  R/W  0x0000C018 WMIX ROP Control Register for 1'th Layer
	volatile VIOC_WMIX_MPAT_u 	uPAT1;			// 0x5C  R/W  0x00000000 WMIX Patterm Register for 1'th Layer
	
	volatile VIOC_WMIX_MACON_u	uACON2;			// 0x60  R/W  0x10622662 WMIX Alpha Control Register for 2'th Layer
	volatile VIOC_WMIX_MCCON_u	uCCON2;			// 0x64  R/W  0x20C44CC4 WMIX Color Control Register for 2'th Layer
	volatile VIOC_WMIX_MROPC_u	uROPC2;			// 0x68  R/W  0x0000C018 WMIX ROP Control Register for 2'th Layer
	volatile VIOC_WMIX_MPAT_u 	uPAT2;			// 0x6C  R/W  0x00000000 WMIX Patterm Register for 2'th Layer
	//need to check
	
} VIOC_WMIX,*PVIOC_WMIX;


// MACON0 address is not change in spec. !!!! need to check



/************************************************************************
*   3.6.10   VIQE (Video Quality Enhancemnet)			(Base Addr = 0x7200D000)
*************************************************************************/

typedef	struct
{
	unsigned	his_cdf_or_lut_en			:	 1;
	unsigned	his_en						:	 1;
	unsigned	gamut_en					:	 1;
	unsigned	denoise3d_en				:	 1;
	unsigned	deintl_en					: 	 1;
	unsigned	RESERVE0					:	 3; // bit  7
	unsigned	no_hor_intpl				:	 1;
	unsigned	RESERVE1					:	 7; // bit 15
	unsigned	fmt_conv_disable			:	 1;
	unsigned	fmt_conv_disable_using_fmt	:	 1;
	unsigned	fmt_conv_flush				:	 1;
	unsigned	RESERVE2					:	 1;
	unsigned	update_disable				:	 1;
	unsigned	cfgupd						:	 1;
	unsigned	RESERVE3					:	 2;	// bit 23
	unsigned	clkgate_deintl_disable		:	 1;
	unsigned	clkgate_d3d_disable			:	 1;
	unsigned	clkgate_pm_disable			:	 1;
	unsigned	RESERVE4					:	 5; // bit 31
}	VIQE_CTRL;

typedef	union
{
	unsigned	long			nREG;
	VIQE_CTRL			bREG;
}	VIQE_CTRL_u;

typedef	struct
{
	unsigned	width					:	16;
	unsigned	height					:	16;
}	VIQE_SIZE;

typedef	union
{
	unsigned	long			nREG;
	VIQE_SIZE			bREG;
}	VIQE_SIZE_u;

typedef	struct
{
	unsigned	h2h						:	 8;
	unsigned	y2r_en					:	 1;
	unsigned	y2r_mode				:	 2;
	unsigned	RESERVE0				:	21;
}	VIQE_CTRL_Y2R;

typedef	union
{
	unsigned	long			nREG;
	VIQE_CTRL_Y2R		bREG;
}	VIQE_CTRL_Y2R_u;

typedef	struct
{
	unsigned	deintl_IRQ				:	1;
	unsigned	denoise_IRQ				:	1;
	unsigned	pm_IRQ					:	1;
	unsigned	RESERVE0				:	5;
	unsigned	deintl_IRQ_mask			:	1;
	unsigned	denoise_IRQ_mask		:	1;
	unsigned	pm_IRQ_mask				:	1;
	unsigned							:  21;
}	VIQE_IRQ;

typedef	union
{
	unsigned	long			nREG;
	VIQE_IRQ			bREG;
}	VIQE_IRQ_u;

typedef	struct
{
	unsigned	global_en_dont_use			:	 1;
	unsigned	top_size_dont_use			:	 1;
	unsigned	stream_deintl_info_dont_use :	 1;
	unsigned	RESERVE0					:	29;
}	VIQE_CTRL_MISC;

typedef union
{
	unsigned	long			nREG;
	VIQE_CTRL_MISC	bREG;
}	VIQE_CTRL_MISC_u;

typedef	struct
{
	unsigned	base					:	32;
}	VIQE_BASE;

typedef	union
{
	unsigned	long			nREG;
	VIQE_BASE			bREG;
}	VIQE_BASE_u;

typedef	struct
{
	unsigned	offs0					:	16;
	unsigned	offs1					:	16;
}	VIQE_OFFS;

typedef	union
{
	unsigned	long			nREG;
	VIQE_OFFS			bREG;
}	VIQE_OFFS_u;

typedef	struct
{
	unsigned	top_size_dont_use			:	 1;
	unsigned	stream_deintl_info_dont_use	:	 1;
	unsigned	uvintpl						:	 1;
	//unsigned								:	13;
	unsigned	RESERVE0					:	 5;
	unsigned 	frmnum						: 	 2;
	unsigned	RESERVE1					:	 6;
	unsigned	enable						:	 1;
	unsigned	cfgupd						:	 1;
	unsigned	frame_rate_disable			:	 1;
	unsigned	RESERVE2					:	 5;
	unsigned	h2h							:	 8;
}	DEINTL_CTRL;

typedef	union
{
	unsigned	long			nREG;
	DEINTL_CTRL		bREG;
}	DEINTL_CTRL_u;

typedef	struct
{
	unsigned	detect_end				:	 1;
	unsigned	flush					:	 1;
	unsigned	eof_control_ready		:	 1;
	unsigned	size_fix				:	 1;
	unsigned	RESERVE0				:	 4;
	unsigned	divisor					:	 2;
	unsigned	RESERVE1				:	 2;
	unsigned	fmt						:	 5;
	unsigned	RESERVE2				:	15;
}	VIQE_FC_MISC;

typedef	union
{
	unsigned	long			nREG;
	VIQE_FC_MISC		bREG;
}	VIQE_FC_MISC_u;

typedef struct
{
	unsigned	width_dwalign			:	16;
	unsigned	full					:	 4;
	unsigned	empty					:	 4;
	unsigned	eof_in					:	 1;
	unsigned	eof_out					:	 1;
	unsigned	RESERVE0				:	 1;
	unsigned	decoder_err				:	 1;
	unsigned	decoder_state			:	 4;
}	VIQE_FC_STATUS;

typedef union
{
	unsigned	long			nREG;
	VIQE_FC_STATUS	bREG;
}	VIQE_FC_STATUS_u;

typedef	struct
{
	unsigned	fc_select				:	 8;
	unsigned	err_check				:	 1;
//   need to add here_en
	unsigned	RESERVE0				:	 6;
	unsigned	fc_stat					:	 8;
	unsigned	RESERVE1				:	 7;
	unsigned	fc_enable				:	 1;
}	VIQE_FC_CTRL;

typedef union
{
	unsigned	long			nREG;
	VIQE_FC_CTRL		bREG;
}	VIQE_FC_CTRL_u;

typedef	struct
{
	unsigned	k0_ac_length_limit		:	 6;
	unsigned	RESERVE0				:	 2;
	unsigned	k1_ac_length_limit		:	 6;
	unsigned	RESERVE1				:	 2;
	unsigned	k2_ac_length_limit		:	 6;
	unsigned	RESERVE2				:	 2;
	unsigned	RESERVE3				:	 8;
}	VIQE_FC_LIMIT;

typedef	union
{
	unsigned	long			nREG;
	VIQE_FC_LIMIT		bREG;
}	VIQE_FC_LIMIT_u;

typedef	struct
{
	unsigned	top_size_dont_use		:	 1;
	unsigned	RESERVE0				: 	 1;
	unsigned	uvintpl					:	 1;
//	unsigned	RESERVE0				:	13;
	unsigned	RESERVE1				:	 5;
	unsigned	frmnum					:	 2;
	unsigned	RESERVE2				:	 6;
	unsigned	enable					:	 1;
	unsigned	cfgupd					:	 1;
	unsigned	RESERVE3				:	 1;
	unsigned	RESERVE4				:	 5;
	unsigned	h2h						:	 8;
}	D3D_CTRL;

typedef	union
{
	unsigned	long			nREG;
	D3D_CTRL			bREG;
}	D3D_CTRL_u;

typedef	struct
{
	unsigned	pcnt_y					:	11;
	unsigned	RESERVE0				:	 5;
	unsigned	lcnt_y					:	11;
	unsigned	RESERVE1				:	 2;
	unsigned	int_mask				:	 1;
	unsigned	int_busy				:	 1;
	unsigned	busy_stat				:	 1;
}	D3D_COUNT;

typedef union
{
	unsigned	long				nREG;
	D3D_COUNT			bREG;
}	D3D_COUNT_u;

typedef struct
{
	unsigned	h2h						:	 8;
	unsigned	d3in					:	 1;
	unsigned	cfgupd					:	 1;
	unsigned	frmupd_disable			:	 1;
	unsigned	lut_init				:	 1;
	unsigned	RESERVE0				:	 4;
	unsigned	top_size_dont_use		:	 1;
	//unsigned							:	15;
	unsigned	RESERVE1				:	 7;
	unsigned	frmnum					:	 2;
	unsigned	RESERVE2				:	 6;
}	D3D_MISC;

typedef union
{
	unsigned	long			nREG;
	D3D_MISC			bREG;
}	D3D_MISC_u;

typedef struct
{
	unsigned	div_pos					:	16;
	unsigned	div_toggle				:	 1;
	unsigned	div_en					:	 1;
	unsigned	RESERVE0				:	14;
}	D3D_DIV;

typedef union
{
	unsigned	long			nREG;
	D3D_DIV			bREG;
}	D3D_DIV_u;

typedef	struct
{
	unsigned	bypass					:	 1;
	unsigned	RESERVE0				:	29;
	unsigned	flush					:	 2;
}	D3D_MISC2;

typedef	union
{
	unsigned	long			nREG;
	D3D_MISC2			bREG;
}	D3D_MISC2_u;

typedef	struct _VIOC_VIQE_CTRL
{
	volatile VIQE_CTRL_u				nVIQE_CTRL;				//  0x000
	volatile VIQE_SIZE_u				nVIQE_CTRL_SIZE;		//	0x004
	volatile VIQE_CTRL_Y2R_u			nVIQE_CTRL_Y2R;			//	0x008
	volatile VIQE_IRQ_u				nVIQE_IRQ;				//	0x00c
	volatile VIQE_CTRL_MISC_u		nVIQE_CTRL_MISC;		//	0x010
	unsigned	int		nHIDDEN;				//	0x014
	unsigned	int		undef_0x018[26];		// 	0x018 ~ 0x07C
}	VIOC_VIQE_CTRL,*PVIOC_VIQE_CTRL;

typedef	struct _VIQE_DEINTL_DMA
{
	volatile VIQE_BASE_u				nDEINTL_BASE0;			//	0x080
	volatile VIQE_BASE_u				nDEINTL_BASE1;			//	0x084
	volatile VIQE_BASE_u				nDEINTL_BASE2;			//	0x088
	volatile VIQE_BASE_u				nDEINTL_BASE3;			//	0x08c
	volatile VIQE_SIZE_u				nDEINTL_SIZE;			//  0x090
	volatile VIQE_OFFS_u				nDEINTL_OFFS;			//	0x094
	volatile DEINTL_CTRL_u			nDEINTL_CTRL;			//	0x098
	unsigned	int			undef_0x09c[1];			//  0x09c
	volatile VIQE_BASE_u				nDEINTL_BASE0A;			//  0x0A0
	volatile VIQE_BASE_u				nDEINTL_BASE1A;			//  0x0A4
	volatile VIQE_BASE_u				nDEINTL_BASE2A;			//  0x0A8
	volatile VIQE_BASE_u				nDEINTL_BASE3A;			//	0x0AC
	volatile VIQE_BASE_u				nDEINTL_BASE0B;			//  0x0B0
	volatile VIQE_BASE_u				nDEINTL_BASE1B;			//  0x0B4
	volatile VIQE_BASE_u				nDEINTL_BASE2B;			//  0x0B8
	volatile VIQE_BASE_u				nDEINTL_BASE3B;			//	0x0BC
	volatile VIQE_BASE_u				nDEINTL_BASE0C;			//  0x0C0
	volatile VIQE_BASE_u				nDEINTL_BASE1C;			//  0x0C4
	volatile VIQE_BASE_u				nDEINTL_BASE2C;			//  0x0C8
	volatile VIQE_BASE_u				nDEINTL_BASE3C;			//	0x0CC
	unsigned	int		nDEINTL_CUR_BASE0;		//	0x0D0
	unsigned	int		nDEINTL_CUR_BASE1;		//	0x0D4
	unsigned	int		nDEINTL_CUR_BASE2;		//	0x0D8
	unsigned	int		nDEINTL_CUR_BASE3;		//	0x0Dc
	unsigned	int		nDEINTL_CUR_WDMA;		//	0x0E0
	unsigned	int		nDEINTL_CUR_RDMA;		//	0x0E4
	unsigned	int		undef_0x0b8[6];			//	0x0E8 ~ 0x0FC
}	VIQE_DEINTL_DMA,*PVIQE_DEINTL_DMA;

typedef	struct
{
	volatile VIQE_FC_MISC_u			nVIQE_FC_MISC;			// 0x100
	volatile VIQE_SIZE_u				nVIQE_FC_SIZE;			// 0x104
	volatile VIQE_FC_STATUS_u			nVIQE_FC_STATUS;		// 0x108
	volatile VIQE_FC_CTRL_u			nVIQE_FC_CTRL;			// 0x10c
	unsigned	int		undef0[4];		// 0x110~0x11C
}	VIQE_FC;

typedef	struct
{
	volatile VIQE_FC_MISC_u			nVIQE_FC_MISC;			// 0x160
	unsigned int undef_0x164;			// 0x164
	volatile VIQE_FC_STATUS_u			nVIQE_FC_STATUS;		// 0x168
	volatile VIQE_FC_CTRL_u			nVIQE_FC_CTRL;			// 0x16c
	volatile VIQE_FC_LIMIT_u			nVIQE_FC_LIMIT;			// 0x170
	unsigned	int		undef0[3];		// 0x174~0x17C
}	VIQE_FC_COMP;


typedef struct
{
	volatile VIQE_BASE_u				nD3D_BASE0;				// 0x180;
	volatile VIQE_BASE_u				nD3D_BASE1;				// 0x184;
	volatile VIQE_SIZE_u				nD3D_SIZE;				// 0x188;
	volatile VIQE_OFFS_u				nD3D_OFFS;				// 0x18c;
	volatile D3D_CTRL_u				nD3D_CTRL;				// 0x190;
	unsigned	int		undef0[3];				// 0x194~0x19c
	unsigned int 		nD3D_CUR_BASE0;		// 0x1A0
	unsigned int 		nD3D_CUR_BASE1;			// 0x1A4
	unsigned	int		nD3D_CUR_WDMA;			// 0x1A8
	unsigned	int		nD3D_CUR_RDMA;			// 0x1AC	
	unsigned	int		undef2[20];				// 0x1B0 ~ 0x1FC
}	VIQE_D3D_DMA;


typedef	struct
{
	volatile unsigned int 		nDI_CTRL;			//0x280 
	volatile unsigned int 		nDI_ENGINE0;		//0x284 
	volatile unsigned int 		nDI_ENGINE1;		//0x288 
	volatile unsigned int 		nDI_ENGINE2;		//0x28C 
	volatile unsigned int 		nDI_ENGINE3;		//0x290 
	volatile unsigned int 		nDI_ENGINE4;		//0x294 
	volatile unsigned int 		nPD_THRES0;			//0x298 
	volatile unsigned int 		nPD_THRES1;			//0x29C 
	volatile unsigned int 		nPD_JUDDER;			//0x2A0 
	volatile unsigned int 		nPD_JUDDER_M;		//0x2A4 
	volatile unsigned int 		nDI_MISCC;			//0x2A8 
	volatile unsigned int 		nDI_STATUS;			//0x2AC 
	volatile unsigned int 		nPD_STATUS;			//0x2B0 
	volatile unsigned int 		nDI_REGION0;		//0x2B4 
	volatile unsigned int 		nDI_REGION1;		//0x2B8 
	volatile unsigned int 		nDI_INT;				//0x2BC 
	volatile unsigned			undef_0[8];			//  0x2C0 ~ 0x2DC (deintl)    	
	volatile unsigned int 		nPD_SAW;			//0x2E0 
	volatile unsigned int 		nDI_CSIZE;			//0x2E4 
	volatile unsigned int 		nDI_FMT;			//0x2E8 
	volatile unsigned			undef_1[5];			//  0x2EC ~ 0x2FC (deintl)    
	
}	VIQE_DEINTL;

typedef	struct
{
	unsigned	int		nCOEFF_HOR_LUMA_0;		//	0x300
	unsigned	int		nCOEFF_HOR_LUMA_1;		//	0x304
	unsigned	int		nCOEFF_VER_LUMA_0;		//	0x308
	unsigned	int		nCOEFF_VER_LUMA_1;		//	0x30c
	unsigned	int		nCOEFF_TEM_LUMA_0;		//	0x310
	unsigned	int		nCOEFF_TEM_LUMA_1;		//	0x314
	unsigned	int		nCOEFF_HOR_CHROMA_0;	//	0x318
	unsigned int		nCOEFF_HOR_CHROMA_1;	//	0x31c
	unsigned	int		nCOEFF_VER_CHROMA_0;	//	0x320
	unsigned	int		nCOEFF_VER_CHROMA_1;	//	0x324
	unsigned	int		nCOEFF_TEM_CHROMA_0;	//	0x328
	unsigned	int		nCOEFF_TEM_CHROMA_1;	//	0x32c	
	volatile D3D_COUNT_u		nD3D_COUNT_LUMA;		//	0x330	
	volatile VIQE_SIZE_u		nD3D_SIZE;				//	0x334
	volatile D3D_MISC_u		nD3D_MISC;				//	0x338	
	unsigned	int		nFIFO_STAT1;				//	0x348  R
	volatile D3D_DIV_u			nD3D_DIV;				//	0x340
	volatile D3D_MISC2_u		nD3D_MISC2;				//	0x344	
	unsigned	int		nFIFO_STAT2;				//	0x348  R	
	unsigned	int		undef[17];				//	0x34c ~ 0x37c
}	VIQE_D3D;

typedef	struct _VIOC_VIQE
{
	volatile VIOC_VIQE_CTRL				cVIQE_CTRL;				//	0x000 ~ 0x07c
	volatile VIQE_DEINTL_DMA			cDEINTL_DMA;			//	0x080 ~ 0x0fc
	volatile VIQE_FC					cDEINTL_DECOMP0;		//  0x100 ~ 0x11C
	volatile VIQE_FC					cDEINTL_DECOMP1;		//	0x120 ~ 0x13c
	volatile VIQE_FC					cDEINTL_DECOMP2;		//	0x140 ~ 0x15c	
	volatile VIQE_FC_COMP			cDEINTL_COMP;			//	0x160 ~ 0x17c
	volatile VIQE_D3D_DMA			cD3D_DMA;				//	0x180 ~ 0x1fc
	volatile VIQE_FC					cD3D_DECOMP;			//  0x200 ~ 0x21c
	volatile VIQE_FC_COMP			cD3D_COMP;				//  0x220 ~ 0x23c	
	unsigned	int		undef_0[16];			//	0x240 ~ 0x27c
	volatile VIQE_DEINTL				cDEINTL;				//  0x280 ~ 0x2fc (deintl)	
	volatile VIQE_D3D				cD3D;				//	0x300 ~ 0x37c
}	VIQE,*PVIQE;


/************************************************************************
*   3.6.11   DEBLOCK				(Base Addr = 0x72003C00)
*************************************************************************/

typedef	struct	{
	unsigned	EN				:	 1;
	unsigned					:	 3;
	unsigned	flush				:	 1;
	unsigned	bypass0_en		:	 1;
	unsigned	bypass1_en		:	 1;
	unsigned	bypass2_en		:	 1;
	unsigned 					: 	 8;
	unsigned 					:	 16;	
}	DBLK_CTRL;

typedef union	{
	unsigned	long			nREG;
	DBLK_CTRL			bREG;
}	DBLK_CTRL_u;

typedef	struct	{
	unsigned	div_en				:	 1;
	unsigned	div_tog				:	 1;
	unsigned 					:	14;
	unsigned	div_pos				:	16;
}	DBLK_DIV;

typedef union {
	unsigned	long			nREG;
	DBLK_DIV			bREG;
}	DBLK_DIV_u;

typedef	struct	{
	unsigned	width				:	16;
	unsigned	height				:	16;
}	DBLK_SIZE;

typedef union	{
	unsigned	long			nREG;
	DBLK_SIZE			bREG;
}	DBLK_SIZE_u;

typedef	struct	{
	unsigned	coeff_fx0			:	 8;
	unsigned	coeff_tx0			:	 8;	
	unsigned	coeff_fx1			:	 8;
	unsigned	coeff_fx2			:	 8;	//
}	DBLK_COEFF_0;

typedef union	{
	unsigned	long			nREG;
	DBLK_COEFF_0			bREG;
}	DBLK_COEFF_0_u;

typedef	struct	{
	unsigned	coeff_fx2			:	 8;
	unsigned	coeff_tx2			:	 8;	
	unsigned	coeff_fx3			:	 8;
	unsigned	coeff_fx4			:	 8;	//
}	DBLK_COEFF_1;

typedef union	{
	unsigned	long			nREG;
	DBLK_COEFF_1			bREG;
}	DBLK_COEFF_1_u;

typedef	struct	_VIOC_DBLK
{
	volatile DBLK_CTRL_u			uDBLK_CTRL;			// 	0x000
	volatile DBLK_DIV_u			uDBLK_DIV;			// 	0x004	
	volatile DBLK_SIZE_u			uDBLK_SIZE;			// 	0x008  R
	unsigned	int				undef1;				//	0x00c	
	volatile DBLK_COEFF_0_u		uDBLK_COEFF_Y_0;	//	0x010
	volatile DBLK_COEFF_1_u		uDBLK_COEFF_Y_1;	//	0x014
	volatile DBLK_COEFF_0_u		uDBLK_COEFF_U_0;	//	0x018
	volatile DBLK_COEFF_1_u		uDBLK_COEFF_U_1;	//	0x01C
	volatile DBLK_COEFF_0_u		uDBLK_COEFF_V_0;	//	0x020
	volatile DBLK_COEFF_1_u		uDBLK_COEFF_V_1;	//	0x024	
	unsigned int				undef_0x028;			//	0x028
	unsigned int				undef_0x02C;			// 	0x02C
}	VIOC_DBLK,*PVIOC_DBLK;

/************************************************************************
*   3.6.12   Filter 2D				(Base Addr = 0x72008000)
*************************************************************************/

typedef	struct	{
	unsigned					:	 1;
	unsigned	hpf0_en				:	 1;
	unsigned	hpf1_en				:	 1;
	unsigned	hpf2_en				:	 1;
	unsigned	flush				:	 1;
	unsigned	bypass0_en			:	 1;
	unsigned	bypass1_en			:	 1;
	unsigned	bypass2_en			:	 1;
	unsigned sel_ch0				:	 1;
	unsigned sel_ch1				:	 1;
	unsigned sel_ch2				:	 1;
	unsigned 					:	13;
	unsigned 	alpha_val				:	 8;
	
}	F2D_CTRL;

typedef union	{
	unsigned	long			nREG;
	F2D_CTRL			bREG;
}	F2D_CTRL_u;

typedef	struct	{
	unsigned	div_en				:	 1;
	unsigned	div_tog				:	 1;
	unsigned 					:	14;
	unsigned	div_pos				:	16;
}	F2D_DIV;

typedef union {
	unsigned	long			nREG;
	F2D_DIV			bREG;
}	F2D_DIV_u;

typedef	struct	{
	unsigned	width				:	16;
	unsigned	height				:	16;
}	F2D_SIZE;

typedef union	{
	unsigned	long			nREG;
	F2D_SIZE			bREG;
}	F2D_SIZE_u;

typedef	struct	{
	unsigned	coeff_a				:	 8;
	unsigned	coeff_ba			:	 8;
	unsigned	coeff_ca			:	 8;
	unsigned	coeff_slope			:	 8;	//
	unsigned	coeff_y_plane		:	 8;
	unsigned	coeff_d				:	 8;
	unsigned	coeff_e				:	 8;
	unsigned	coeff_f				:	 8;	//
	unsigned	coeff_g				:	 8;
	unsigned	coeff_m1			:	 8;
	unsigned	coeff_m2			:	 8;
	unsigned	coeff_l				:	 8;	//
	unsigned	coeff_y_plane_h1	:	 9;
	unsigned 					:	 7;
	unsigned	coeff_y_plane_h2	:	 9;
	unsigned 					:	 7;	//
}	F2D_COEFF;

typedef union	{
	unsigned	long			nREG[4];
	F2D_COEFF			bREG;
}	F2D_COEFF_u;

typedef struct{
	unsigned 	para_00 		:	9;
	unsigned 			:	7;
	unsigned 	para_01 		:	9;
	unsigned 			:	7;
} F2D_COEFF_WF;

typedef union{
	unsigned long		nREG;
	F2D_COEFF_WF		bREG;
} F2D_COEFF_WF_u;

typedef struct{
	unsigned para_8		:	9;
	unsigned 			:	7;
	unsigned CB0			:	4;
	unsigned	CB1			:	4;
	unsigned	CB2			:	4;
	unsigned			:	4;
} F2D_COEFF_WF_CH2_4;

typedef union{
	unsigned long		nREG;
	F2D_COEFF_WF_CH2_4	bREG;
}F2D_COEFF_WF_CH_2_4_u;



typedef	struct	{
	unsigned	para_cut0			:	 4;
	unsigned	para_cut1			:	 4;
	unsigned	para_cut2			:	 4;
	unsigned 			:	 4;	//must 16 bits
}	F2D_SCOEFF_CUT;

typedef union {
	unsigned short				nREG;
	F2D_SCOEFF_CUT			bREG;
}	F2D_SCOEFF_CUT_u;

typedef	struct	_VIOC_FILT2D
{
	volatile F2D_CTRL_u				uF2D_CTRL;				// 	0x000
	volatile F2D_DIV_u					uF2D_DIV;				// 	0x004
	volatile F2D_COEFF_u				uF2D_COEFF0;			//	0x010 ~ 0x01c
	volatile F2D_COEFF_u				uF2D_COEFF1;			//	0x020 ~ 0x02c
	volatile F2D_COEFF_u				uF2D_COEFF2;			//	0x030 ~ 0x03c	
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF0_0;			//	0x40
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF0_1;			//	0x44
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF0_2;			//	0x48
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF0_3;			//	0x4C	
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF01_40;		//	0x50
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF1_1;			//	0x54
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF1_2;			//	0x58
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF1_3;			//	0x5C
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF1_4;			//	0x60
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF2_0;			//	0x64
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF2_1;			//	0x68
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF2_2;			//	0x6C
	volatile F2D_COEFF_WF_u			uF2D_SCOEFF2_3;			//	0x70
	volatile F2D_COEFF_WF_CH_2_4_u	uF2D_SCOEFF2_4;			//	0x74
	unsigned int undef_0x78[2];			//
}	VIOC_FILT2D, *PVIOC_FILT2D;



/************************************************************************
*   3.6.13   Asynchronous Frame FIFO 			(Base Addr = 0x72003900)
*************************************************************************/

typedef	struct {
	unsigned				WEN			:  1;
	unsigned				REN0		:  1;
	unsigned				REN1		:  1;
	unsigned				REN2		:  1;
	unsigned				RMODE_TIME	:  1;
	unsigned 				   			:  3;
	unsigned				NENTRY    	:  6;
	unsigned				WMODE_TIME	:  1;
	unsigned				WMODE_FULL	:  1;
	unsigned				EFULL    	:  2;
	unsigned				EEMPTY   	:  2;
	unsigned 				   			:  4;
	unsigned				IE_FULL		:  1;
	unsigned				IE_EFULL	:  1;
	unsigned				IE_EMPTY	:  1;
	unsigned				IE_EEMPTY	:  1;
	unsigned				IE_WEOF		:  1;
	unsigned				IE_REOF0	:  1;
	unsigned				IE_REOF1	:  1;
	unsigned				IE_REOF2	:  1;
}	VIOC_FIFO_CTRL0;

typedef	union {
	unsigned	long			nREG;
	VIOC_FIFO_CTRL0		bREG;
}	VIOC_FIFO_CTRL0_u;

typedef	struct {
	unsigned				WDMA		:  4;
	unsigned				RDMA0		:  4;
	unsigned				RDMA1		:  4;
	unsigned				RDMA2		:  4;
	unsigned				FAST_RDMA	:  2;
	unsigned 				   			: 14;
}	VIOC_FIFO_CTRL1;

typedef	union {
	unsigned	long			nREG;
	VIOC_FIFO_CTRL1		bREG;
}	VIOC_FIFO_CTRL1_u;

typedef	struct {
	unsigned				IS_FULL		:  1;
	unsigned				IS_EFULL	:  1;
	unsigned				IS_EMPTY	:  1;
	unsigned				IS_EEMPTY	:  1;
	unsigned				IS_WEOF		:  1;
	unsigned				IS_REOF0	:  1;
	unsigned				IS_REOF1	:  1;
	unsigned				IS_REOF2	:  1;
	unsigned 								: 24;
}	VIOC_FIFO_IRQSTAT;

typedef	union {
	unsigned	long			nREG;
	VIOC_FIFO_IRQSTAT	bREG;
}	VIOC_FIFO_IRQSTAT_u;

typedef	struct {
	unsigned				FULL		:  1;
	unsigned				EFULL		:  1;
	unsigned				EMPTY		:  1;
	unsigned				EEMPTY		:  1;
	unsigned				WEOF		:  1;
	unsigned				REOF0		:  1;
	unsigned				REOF1		:  1;
	unsigned				REOF2		:  1;
	unsigned 							: 16;
	unsigned				FILLED		:  8;
}	VIOC_FIFO_FIFOSTAT;

typedef	union {
	unsigned	long		nREG;
	VIOC_FIFO_FIFOSTAT	bREG;
}	VIOC_FIFO_FIFOSTAT_u;

typedef	struct	_VIOC_FIFO
{
	volatile VIOC_FIFO_CTRL0_u			uCH0_CTRL0;				// 0x000
	volatile VIOC_FIFO_CTRL1_u			uCH0_CTRL1;				// 0x004
	volatile VIOC_FIFO_IRQSTAT_u		uCH0_IRQSTAT;			// 0x008
	volatile VIOC_FIFO_FIFOSTAT_u		uCH0_FIFOSTAT;			// 0x00C
	volatile VIOC_FIFO_CTRL0_u			uCH1_CTRL0;				// 0x010
	volatile VIOC_FIFO_CTRL1_u			uCH1_CTRL1;				// 0x014
	volatile VIOC_FIFO_IRQSTAT_u		uCH1_IRQSTAT;			// 0x018
	volatile VIOC_FIFO_FIFOSTAT_u		uCH1_FIFOSTAT;			// 0x01C
	unsigned	int				nCH0_BASE[16];		// 0x020~0x05C
	unsigned	int				nCH1_BASE[16];		// 0x060~0x09C
}	VIOC_FIFO,*PVIOC_FIFO;

/************************************************************************
*   3.6.14   CPU Interface 			(Base Addr = 0x72100200)
*************************************************************************/

typedef	struct {
	unsigned				RD_HLD 	:  3;
	unsigned				RD_PW  	:  9;
	unsigned				RD_STP 	:  3;
	unsigned				RD_B16 	:  1;
	unsigned				WR_HLD 	:  3;
	unsigned				WR_PW  	:  9;
	unsigned				WR_STP 	:  3;
	unsigned				WR_B16 	:  1;
}	VIOC_CPUIF_CTRL;

typedef	union {
	unsigned	long			nREG;
	VIOC_CPUIF_CTRL		bREG;
}	VIOC_CPUIF_CTRL_u;

typedef	struct {
	unsigned				XA00_RDBS	: 8;
	unsigned				XA00_WRBS	: 8;
	unsigned				XA01_RDBS	: 8;
	unsigned				XA01_WRBS	: 8;
	unsigned				XA10_RDBS	: 8;
	unsigned				XA10_WRBS	: 8;
	unsigned				XA11_RDBS	: 8;
	unsigned				XA11_WRBS	: 8;
}	VIOC_CPUIF_BSWAP;

typedef	union {
	unsigned	long			nREG[2];
	VIOC_CPUIF_BSWAP	bREG;
}	VIOC_CPUIF_BSWAP_u;

typedef	struct {
	unsigned				MODE68	: 1;
	unsigned 						:31;
}	VIOC_CPUIF_TYPE;

typedef	union {
	unsigned	long			nREG;
	VIOC_CPUIF_TYPE		bREG;
}	VIOC_CPUIF_TYPE_u;

typedef	union {
	unsigned char		b08[16];
	unsigned short	b16[ 8];
	unsigned	int		b32[ 4];
}	VIOC_CPUIF_AREA_u;

typedef	struct _VIOC_CPUIF_CHANNEL
{
	volatile VIOC_CPUIF_CTRL_u	uCS0_CMD0_CTRL;			// 0		: XA[1:0] == 2'b00		0x00
	volatile VIOC_CPUIF_CTRL_u	uCS0_CMD1_CTRL;			// 1		: XA[1:0] == 2'b01		0x04
	volatile VIOC_CPUIF_CTRL_u	uCS0_DAT0_CTRL;			// 2		: XA[1:0] == 2'b10		0x08
	volatile VIOC_CPUIF_CTRL_u	uCS0_DAT1_CTRL;			// 3		: XA[1:0] == 2'b11		0x0C
	volatile VIOC_CPUIF_CTRL_u	uCS1_CMD0_CTRL;			// 4		: XA[1:0] == 2'b00		0x10
	volatile VIOC_CPUIF_CTRL_u	uCS1_CMD1_CTRL;			// 5		: XA[1:0] == 2'b01		0x14
	volatile VIOC_CPUIF_CTRL_u	uCS1_DAT0_CTRL;			// 6		: XA[1:0] == 2'b10		0x18
	volatile VIOC_CPUIF_CTRL_u	uCS1_DAT1_CTRL;			// 7		: XA[1:0] == 2'b11		0x1C
	volatile VIOC_CPUIF_BSWAP_u	uCS0_BSWAP;				// 8 ~ 9						0x20~0x24
	volatile VIOC_CPUIF_BSWAP_u	uCS1_BSWAP;				// 10 ~ 11					0x28~0x2C
	unsigned	int			reserved0[2];			// 12 ~ 13						0x30~0x34
	volatile VIOC_CPUIF_TYPE_u	uTYPE;					// 14						0x38
	unsigned	int			reserved1;				// 15						0x3C
	volatile VIOC_CPUIF_AREA_u	uCS0_CMD0;				// 16 ~ 19	: XA[1:0] = 2'b00	0x40~0x4C
	volatile VIOC_CPUIF_AREA_u	uCS0_CMD1;				// 20 ~ 23	: XA[1:0] = 2'b01	0x50~0x5C
	volatile VIOC_CPUIF_AREA_u	uCS0_DAT0;				// 24 ~ 27	: XA[1:0] = 2'b10 	0x60~0x6C
	volatile VIOC_CPUIF_AREA_u	uCS0_DAT1;				// 28 ~ 31	: XA[1:0] = 2'b11	0x70~0x7C
	volatile VIOC_CPUIF_AREA_u	uCS1_CMD0;				// 32 ~ 35	: XA[1:0] = 2'b00	0x80~0x8C
	volatile VIOC_CPUIF_AREA_u	uCS1_CMD1;				// 36 ~ 39	: XA[1:0] = 2'b01	0x90~0x9C
	volatile VIOC_CPUIF_AREA_u	uCS1_DAT0;				// 40 ~ 43	: XA[1:0] = 2'b10	0xA0~0xAC
	volatile VIOC_CPUIF_AREA_u	uCS1_DAT1;				// 44 ~ 47	: XA[1:0] = 2'b11	0xB0~0xBC
	unsigned	int			reserved2[16];			// 48 ~ 63  

}	VIOC_CPUIF_CHANNEL,*PVIOC_CPUIF_CHANNEL;


/************************************************************************
*   3.6.15   Look-Up-Table				(Base Addr = 0x72009000)
*************************************************************************/


typedef	struct {
	unsigned				SELECT		:  4;		// 
	unsigned 							: 28;
}	VIOC_LUT_SEL;

typedef	union {
	unsigned	long			nREG;
	VIOC_LUT_SEL		bREG;
}	VIOC_LUT_SEL_u;

typedef	struct {
	unsigned				SEL	    	:  8;		// no meaning for device case 
	unsigned 						: 23;
	unsigned				EN	     	:  1;
}	VIOC_LUT_CONFIG;

typedef	union {
	unsigned	long			nREG;
	VIOC_LUT_CONFIG		bREG;
}	VIOC_LUT_CONFIG_u;

typedef	struct _VIOC_LUT
{
	volatile VIOC_LUT_SEL_u		uSEL;					// 0x00
	volatile VIOC_LUT_CONFIG_u	uDEV0CFG;				// 0x04
	volatile VIOC_LUT_CONFIG_u	uDEV1CFG;				// 0x08
	volatile VIOC_LUT_CONFIG_u	uCOMP0CFG;				// 0x0C
	volatile VIOC_LUT_CONFIG_u	uCOMP1CFG;				// 0x10
	volatile VIOC_LUT_CONFIG_u	uCOMP2CFG;				// 0x14
	volatile VIOC_LUT_CONFIG_u	uCOMP3CFG;				// 0x18
}	VIOC_LUT,*PVIOC_LUT;

typedef	struct {
	unsigned	int nEntry[256];
}	VIOC_LUT_TABLE;


/************************************************************************
*   3.6.16   VIN_DEMUX				(Base Addr = 0x7200A800)
*************************************************************************/

typedef struct
{
	unsigned	enable				:	 1;
	unsigned					:	 3;
	unsigned	byte_select			:	 2;
	unsigned					:	 2;
	unsigned	clk_mode			:	 3;
	unsigned					:	 5;
	unsigned	sel0					:	 3;
	unsigned					:	 1;
	unsigned	sel1					:	 3;
	unsigned 					:	 1;
	unsigned	sel2					: 	 3;
	unsigned 					:	 1;
	unsigned	sel3					:	 3;
	unsigned 					:	 1;
}	VIN_DEMUX_CTRL;

typedef	union
{
	unsigned	long			nREG;
	VIN_DEMUX_CTRL		bVIN_DEMUX_CTRL;
}	VIN_DEMUX_CTRL_u;

typedef	struct
{
	unsigned	hb					:	 4;
	unsigned	vb					:	 4;
	unsigned	fp					:	 4;
	unsigned					:	 4;
	unsigned	psl					:	 2;
	unsigned	sync_byte			:	 2;
	unsigned					:	12;
}	VIN_DEMUX_BLANK0;

typedef union
{
	unsigned	long		nREG;
	VIN_DEMUX_BLANK0		bREG;
}	VIN_DEMUX_BLANK0_u;

typedef struct
{
	unsigned	pf					:	 8;
	unsigned	ps					:	 8;
	unsigned	pt					:	 8;
	unsigned	RESERVE0			:	 8;
}	VIN_DEMUX_BLANK1;

typedef union
{
	unsigned	long		nREG;
	VIN_DEMUX_BLANK1		bREG;
}	VIN_DEMUX_BLANK1_u;

typedef struct
{
	unsigned	dsel0				:	 4;
	unsigned	dsel1				:	 4;
	unsigned	dsel2				:	 4;
	unsigned	dsel3				:	 4;
	unsigned	RESERVE0			:	16;
}	VIN_DEMUX_MISC;

typedef union
{
	unsigned	long		nREG;
	VIN_DEMUX_MISC		bREG;
}	VIN_DEMUX_MISC_u;

typedef struct
{
	unsigned	idx0				:	 4;
	unsigned	idx1				:	 4;
	unsigned	idx2				:	 4;
	unsigned	idx3				:	 4;
	unsigned	RESERVE0			:	16;
}	VIN_DEMUX_STS;

typedef	union
{
	unsigned	long		nREG;
	VIN_DEMUX_STS			bREG;
}	VIN_DEMUX_STS_u;


typedef	struct _VOIC_VIN_DEMUX
{
	volatile VIN_DEMUX_CTRL_u		uVIN_DEMUX_CTRL;	//	0x000 R/W 0x32100000 VIN DEMUX Control register
	volatile VIN_DEMUX_BLANK0_u		uVIN_DEMUX_BLANK0;	//	0x004 R/W 0x00030CB9 VIN DEMUX Blank Set 0 Register
	volatile VIN_DEMUX_BLANK1_u		uVIN_DEMUX_BLANK1;	//	0x008 R/W 0x000000FF VIN DEMUX Black Set 1 Register
	volatile VIN_DEMUX_MISC_u		uVIN_DEMUX_MISC;	//	0x00C R/W 0x00004444 VIN DEMUX Miscellaneous Register
	volatile VIN_DEMUX_STS_u		uVIN_DEMUX_STS;		//	0x010 R     -                VIN DEMUX Status Register
}	VIOC_VIN_DEMUX,*PVIOC_VIN_DEMUX;



/************************************************************************
*   3.6.17   Output Configuration				(Base Addr = 0x72100200)
*************************************************************************/

typedef	struct {
	unsigned				FMT		:  4;
	unsigned				reserved0		:  1;
	unsigned				XA		:  2;
	unsigned				CS		:  1;
	unsigned				SI		:  1;
	unsigned				EN		:  1;
	unsigned				reserved1		:  4;
	unsigned				IV		:  1;
	unsigned				ID		:  1;
	unsigned				reserved2		: 16;
}	VIOC_OUTCFG_CPUIF;

typedef	union {
	unsigned	long			nREG;
	VIOC_OUTCFG_CPUIF	bREG;
}	VIOC_OUTCFG_CPUIF_u;

typedef	struct {
	unsigned				HDMISEL	:  2;	// HDMI OUTPUT
	unsigned 						:  2;
	unsigned				SDVESEL	:  2;	// SDVENC OUTPUT
	unsigned 						:  2;
	unsigned				HDVESEL	:  2;	// HDVENC OUTPUT
	unsigned 						:  2;
	unsigned				M80SEL	:  2;	// MIPI I80 OUTPUT
	unsigned 						:  2;
	unsigned				MRGBSEL :  2;	// MIPI RGB OUTPUT
	unsigned 						:  2;
	unsigned 						: 12;
}	VIOC_OUTCFG_MISC;

typedef	union {
	unsigned	long			nREG;
	VIOC_OUTCFG_MISC	bREG;
}	VIOC_OUTCFG_MISC_u;

typedef	struct	{
	volatile VIOC_OUTCFG_CPUIF_u		uD0CPUIF;		//0x00
	volatile VIOC_OUTCFG_CPUIF_u		uD1CPUIF;		//0x04
	volatile VIOC_OUTCFG_CPUIF_u		uD2CPUIF;		//0x08
	volatile VIOC_OUTCFG_MISC_u		uMISCCFG;		//0x10
}	VIOC_OUTCFG;


/************************************************************************
*   3.6.18   Configuration & Interrupt			(Base Addr = 0x7200A000)
*************************************************************************/

typedef	struct 
{
	unsigned				DEV0	: 1;
	unsigned				DEV1	: 1;
	unsigned				DEV2	: 1;
	unsigned				TIMER	: 1;
	unsigned				RDMA00	: 1;
	unsigned				RDMA01	: 1;
	unsigned				RDMA02	: 1;
	unsigned				RDMA03	: 1;
	unsigned				RDMA04	: 1;
	unsigned				RDMA05	: 1;
	unsigned				RDMA06	: 1;
	unsigned				RDMA07	: 1;
	unsigned				RDMA08	: 1;
	unsigned				RDMA09	: 1;
	unsigned				RDMA10	: 1;
	unsigned				RDMA11	: 1;
	unsigned				RDMA12	: 1;
	unsigned				RDMA13	: 1;
	unsigned				RDMA14	: 1;
	unsigned				RDMA15	: 1;
	unsigned				RDMA16	: 1;
	unsigned				RDMA17	: 1;
	unsigned 						: 2;
	unsigned				MMU		: 1;
	unsigned 						: 3;
	unsigned				FIFO0	: 1;
	unsigned				FIFO1	: 1;
	unsigned 						: 2;	
	unsigned				WDMA00	: 1;
	unsigned				WDMA01	: 1;
	unsigned				WDMA02	: 1;
	unsigned				WDMA03	: 1;
	unsigned				WDMA04	: 1;
	unsigned				WDMA05	: 1;
	unsigned				WDMA06	: 1;
	unsigned				WDMA07	: 1;
	unsigned				WDMA08	: 1;
	unsigned 						: 7;
	unsigned				WMIX0	: 1;
	unsigned				WMIX1	: 1;
	unsigned				WMIX2	: 1;
	unsigned				WMIX3	: 1;
	unsigned				WMIX4	: 1;
	unsigned				WMIX5	: 1;
	unsigned 						: 5;
	unsigned				VIQE	: 1;
	unsigned				SC0		: 1;
	unsigned				SC1		: 1;
	unsigned				SC2		: 1;
	unsigned				SC3		: 1;
}	VIOC_IREQ_IREQ;

typedef	union {
	unsigned	long			nREG[2];
	VIOC_IREQ_IREQ	bREG;
}	VIOC_IREQ_IREQ_u;

typedef struct {
	unsigned 				INDEX	: 6;
	unsigned 		 			: 10;
	unsigned 					: 15;
	unsigned 				IVALID : 1 ;
}VIOC_IREQ_VECTORID;

typedef union
{
	unsigned long			nREG;
	VIOC_IREQ_VECTORID	bREG;
}VIOC_IREQ_VECTORID_u;

typedef struct 
{
	unsigned 				L0EVSEL	:  3;				// TCC8910 | TCC8010
	unsigned 						:  1;
	unsigned 				L1EVSEL	:  3;				// TCC8910
	unsigned 						:  1;
	unsigned 				L2EVSEL	:  3;				// TCC8910
	unsigned 						:  1;
	unsigned 						:  4;
	unsigned 				WMIX0_0	:  1;	// 16		// TCC8910 | TCC8010
	unsigned 				WMIX0_1	:  1;	// 17		// TCC8910 | TCC8010
	unsigned 				WMIX1_0	:  1;	// 18		// TCC8910 | TCC8010
	unsigned 				WMIX1_1	:  1;	// 19		// TCC8910 | TCC8010
	unsigned 				WMIX2_0	:  1;	// 20		// TCC8910 | TCC8010
	unsigned 				WMIX2_1	:  1;	// 21		// TCC8910 | TCC8010
	unsigned 				WMIX3_0	:  1;	// 22		// TCC8910
	unsigned 				WMIX3_1	:  1;	// 23		// TCC8910
	unsigned 				WMIX4_0	:  1;	// 24		// TCC8910
	unsigned 				WMIX4_1	:  1;	// 25		// TCC8910
	unsigned 				WMIX5_0	:  1;	// 26		// TCC8910
	unsigned 				WMIX5_1	:  1;	// 27		// TCC8910
	unsigned 				WMIX6_0	:  1;	// 28		// TCC8910
	unsigned 				WMIX6_1	:  1;	// 29		// TCC8910
	unsigned 				RDMA12	:  1;	// 30
	unsigned 				RDMA14	:  1;	// 31
}VIOC_CONFIG_ETC;

typedef	union {
	unsigned long				nREG;
	VIOC_CONFIG_ETC		bREG;
}	VIOC_CONFIG_ETC_u;

typedef struct {
	unsigned 				SELECT    	:  8;
	unsigned 							:  8;
	unsigned 				STATUS     	:  2;
	unsigned 				ERR      	:  1;
	unsigned 							:  5;
	unsigned 							:  7;
	unsigned 				EN		     	:  1;
}VIOC_CONFIG_PATH;

typedef	union {
	unsigned long				nREG;
	VIOC_CONFIG_PATH		bREG;
}VIOC_CONFIG_PATH_u;


typedef	struct {
	unsigned				RDMA 	: 1;
	unsigned				SCALER	: 1;
	unsigned				MIXER	: 1;
	unsigned				WDMA	: 1;
	unsigned				FDLY	: 1;
	unsigned				VIQE	: 1;
	unsigned				DEINTLS	: 1;
	unsigned				FILT2D	: 1;
	unsigned				FCENC	: 1;
	unsigned				FCDEC	: 1;
	unsigned				CPUIF	: 1;
	unsigned				LCD2AHB	: 1;
	unsigned				VIDEOIN	: 1;
	unsigned				DEBLOCK	: 1;
	unsigned						: 18;
}	VIOC_POWER_AUTOPD;

typedef	union {
	unsigned	long			nREG;
	VIOC_POWER_AUTOPD	bREG;
}	VIOC_POWER_AUTOPD_u;

typedef	struct {
	unsigned				MIN			: 4;
	unsigned							: 4;
	unsigned				EN			: 1;
	unsigned				PROF_EN		: 1;
	unsigned							: 5;
	unsigned							: 1;
	unsigned							: 16;
}	VIOC_POWER_CLKCTRL;

typedef	union {
	unsigned	long			nREG;
	VIOC_POWER_CLKCTRL	bREG;
}	VIOC_POWER_CLKCTRL_u;

typedef	struct {
	unsigned				RDMA00		: 1;
	unsigned				RDMA01		: 1;
	unsigned				RDMA02		: 1;
	unsigned				RDMA03		: 1;
	unsigned				RDMA04		: 1;
	unsigned				RDMA05		: 1;
	unsigned				RDMA06		: 1;
	unsigned				RDMA07		: 1;
	unsigned				RDMA08		: 1;
	unsigned				RDMA09		: 1;
	unsigned				RDMA10		: 1;
	unsigned				RDMA11		: 1;
	unsigned				RDMA12		: 1;
	unsigned				RDMA13		: 1;
	unsigned				RDMA14		: 1;
	unsigned				RDMA15		: 1;
	unsigned				RDMA16		: 1;
	unsigned				RDMA17		: 1;
	unsigned				FCDEC0		: 1;
	unsigned				FCDEC1		: 1;
	unsigned				FCDEC2		: 1;
	unsigned				FCDEC3		: 1;
	unsigned				FCENC0		: 1;
	unsigned				FCENC1		: 1;
	unsigned				VIDEOIN0	: 1;
	unsigned				VIDEOIN1	: 1;
	unsigned							: 1;
	unsigned				FRAMEFIFO	: 1;
	unsigned				SCALER0		: 1;
	unsigned				SCALER1		: 1;
	unsigned				SCALER2		: 1;
	unsigned				SCALER3		: 1;		// 32	
	unsigned				WDMA00		: 1;
	unsigned				WDMA01		: 1;
	unsigned				WDMA02		: 1;
	unsigned				WDMA03		: 1;
	unsigned				WDMA04		: 1;
	unsigned				WDMA05		: 1;
	unsigned				WDMA06		: 1;
	unsigned				WDMA07		: 1;
	unsigned				WDMA08		: 1;
	unsigned							: 1;
	unsigned				WMIX0 		: 1;
	unsigned				WMIX1 		: 1;
	unsigned				WMIX2 		: 1;
	unsigned				WMIX3 		: 1;
	unsigned				WMIX4 		: 1;
	unsigned				WMIX5 		: 1;
	unsigned				VIQE 		: 1;
	unsigned				DEINTLS		: 1;
	unsigned							: 2;
	unsigned				DEV0 		: 1;
	unsigned				DEV1 		: 1;
	unsigned				DEV2 		: 1;
	unsigned				MMU  		: 1;
	unsigned				FILT2D 		: 1;
	unsigned				DEBLOCK		: 1;
	unsigned				FDLY0 		: 1;
	unsigned				FDLY1 		: 1;
	unsigned							: 2;
	unsigned							: 2;
}	VIOC_POWER_BLOCKS;

typedef	union {
	unsigned	long			nREG[2];
	VIOC_POWER_BLOCKS	bREG;
}	VIOC_POWER_BLOCKS_u;

typedef	struct	_VIOC_IREQ_CONFIG
{
	volatile VIOC_IREQ_IREQ_u		uRAWSTATUS;		// 0x00~0x04
	volatile VIOC_IREQ_IREQ_u		uSYNCSTATUS;		// 0x08~0x0C
	volatile VIOC_IREQ_IREQ_u		uIREQSELECT;			// 0x10~0x14	
	volatile VIOC_IREQ_IREQ_u		nIRQMASKSET;		// 0x18~0x1C
	volatile VIOC_IREQ_IREQ_u		nIRQMASKCLR;		// 0x20~0x24
	volatile VIOC_IREQ_VECTORID_u	nVECTORID;			// 0x28
	unsigned	int			reserved0[5];			// 11 ~ 15
	volatile VIOC_CONFIG_ETC_u	uMISC;				// 0x40
	volatile VIOC_CONFIG_PATH_u	uSC0;				// 0x44
	volatile VIOC_CONFIG_PATH_u	uSC1;				// 0x48
	volatile VIOC_CONFIG_PATH_u	uSC2;				// 0x5C
	volatile VIOC_CONFIG_PATH_u	uSC3;				// 0x50
	volatile VIOC_CONFIG_PATH_u	uVIQE;				// 0x54
	volatile VIOC_CONFIG_PATH_u	uDEINTLS;			// 0x58
	volatile VIOC_CONFIG_PATH_u	uFILT2D;				// 0x5C
	volatile VIOC_CONFIG_PATH_u	uFCDEC0;			// 0x60
	volatile VIOC_CONFIG_PATH_u	uFCDEC1;			// 0x64
	volatile VIOC_CONFIG_PATH_u	uFCDEC2;			// 0x68
	volatile VIOC_CONFIG_PATH_u	uFCDEC3;			// 0x6C
	volatile VIOC_CONFIG_PATH_u	uFCENC0;			// 0x70
	volatile VIOC_CONFIG_PATH_u	uFCENC1;			// 0x74
	volatile VIOC_CONFIG_PATH_u	uFDELAY0;			// 0x78
	volatile VIOC_CONFIG_PATH_u	uFDELAY1;			// 0x7C
	volatile VIOC_CONFIG_PATH_u	uDEBLOCK;			// 0x80
	unsigned int			reserved1[15];				// 0x84~0xBC
	unsigned int			nARID;						// 0xC0
	unsigned int			nAWID;						// 0xC4
	volatile VIOC_POWER_AUTOPD_u	uAUTOPD;				// 0xC8
	volatile VIOC_POWER_CLKCTRL_u	uCLKCTRL;				// 0xCC
	volatile VIOC_POWER_BLOCKS_u	uPOWERDOWN;			// 0xD0~0xD4
	volatile VIOC_POWER_BLOCKS_u	uSOFTRESET;				// 0xD8~0xDC	
}	VIOC_IREQ_CONFIG,*PVIOC_IREQ_CONFIG;




/************************************************************************
*   3.6.19   Timer				(Base Addr = 0x7200C000)
*************************************************************************/

typedef	struct {
	unsigned				USECDIV		:  8;		// to make usec pulse from TCLK (freq(TCLK) / (USECDIV+1))
	unsigned				UNITDIV		:  8;		// to make 100usec pulse from usec (normally 99)
	unsigned 							: 15;
	unsigned				EN			:  1;
}	VIOC_TIMER_USEC;

typedef	union {
	unsigned	long			nREG;
	VIOC_TIMER_USEC		bREG;
}	VIOC_TIMER_USEC_u;

typedef	struct {
	unsigned				COUNTER    	: 16;
	unsigned 							: 15;
	unsigned				EN	     	:  1;
}	VIOC_TIMER_CORE;

typedef	union {
	unsigned	long			nREG;
	VIOC_TIMER_CORE		bREG;
}	VIOC_TIMER_CORE_u;

typedef	struct {
	unsigned				TIME    	: 16;
	unsigned 							: 15;
	unsigned				EN	     	:  1;
}	VIOC_TIMER_TIREQ;

typedef	union {
	unsigned	long			nREG;
	VIOC_TIMER_TIREQ	bREG;
}	VIOC_TIMER_TIREQ_u;

typedef	struct {
	unsigned				TIMER0    	:  1;
	unsigned				TIMER1    	:  1;
	unsigned				TIREQ0    	:  1;
	unsigned				TIREQ1    	:  1;
	unsigned 							: 28;
}	VIOC_TIMER_IREQ;

typedef	union {
	unsigned	long			nREG;
	VIOC_TIMER_IREQ		bREG;
}	VIOC_TIMER_IREQ_u;

typedef	struct	_VIOC_TIMER
{
	volatile VIOC_TIMER_USEC_u	uUSEC;				// 				0x00
	unsigned	int			nCURTIME;			// 1	: 100us unit 	0x04  R
	volatile VIOC_TIMER_CORE_u	uTIMER0;				// 2				0x08
	volatile VIOC_TIMER_CORE_u	uTIMER1;				// 3				0x0C
	volatile VIOC_TIMER_TIREQ_u	uTIREQ0;				// 4				0x10
	volatile VIOC_TIMER_TIREQ_u	uTIREQ1;				// 5				0x14
	volatile VIOC_TIMER_IREQ_u	uIRQMASK;			// 6				0x18
	volatile VIOC_TIMER_IREQ_u	uIRQSTAT;			// 7				0x1C
}	VIOC_TIMER,*PVIOC_TIMER;



/************************************************************************
*   3.6.20   MMU				(Base Addr = 0x7200F000)
*************************************************************************/


typedef	struct {
	unsigned				EN			:  1;		// MMU Enable
	unsigned 							: 31;
}	VIOC_MMU_CTRL;

typedef	union {
	unsigned	long			nREG;
	VIOC_MMU_CTRL		bREG;
}	VIOC_MMU_CTRL_u;

typedef	struct {
	unsigned				SIZE    	:  3;		// 0, 1, 2, 3, 4, 5
	unsigned				BASE    	: 29;
}	VIOC_MMU_TTB;

typedef	union {
	unsigned	long			nREG;
	VIOC_MMU_TTB		bREG;
}	VIOC_MMU_TTB_u;

typedef struct{
	unsigned				FAULT 	:  1;
	unsigned 							: 31;
}	VIOC_MMU_IRQSTAT;

typedef union{
	unsigned	long			nREG;
	VIOC_MMU_IRQSTAT		bREG;
}	VIOC_MMU_IRQSTAT_u;

typedef	struct _VIOC_MMU
{
	volatile VIOC_MMU_CTRL_u		uCTRL;			// 0x00
	volatile VIOC_MMU_TTB_u			uTTB;			// 0x04
	unsigned	int				nFAULTADDR;		// 0x08  R
	volatile VIOC_MMU_IRQSTAT_u		nIRQSTAT;		// 0x0C  R
	volatile VIOC_MMU_IRQSTAT_u		nIRQMASK;		// 0x10
}	VIOC_MMU,*PVIOC_MMU;







/************************************************************************
*   4. NTSC / PAL Encoder Composite Output       (Base Addr = 0x72200000)
*************************************************************************/

typedef struct _NTSCPAL {
	volatile TCC_DEF32BIT_TYPE	STATA;			// 0x000  R    -			Status register A
	volatile TCC_DEF32BIT_TYPE	ECMDA;			// 0x004  R/W  0x00000000	Encoder control register A
	volatile TCC_DEF32BIT_TYPE	ECMDB;			// 0x008  R/W  0x00000000	Encoder control register B
	volatile TCC_DEF32BIT_TYPE	GLK;			// 0x00C  R/W  0x00000000	Chroma gunlock control register
	volatile TCC_DEF32BIT_TYPE	SCH;			// 0x010  R/W  0x00000000	SCH phase control
	volatile TCC_DEF32BIT_TYPE	HUE;			// 0x014  R/W  0x00000000	HUE phase control
	volatile TCC_DEF32BIT_TYPE	SAT;			// 0x018  R/W  0x00000000	Chroma saturation control
	volatile TCC_DEF32BIT_TYPE	CONT;			// 0x01C  R/W  0x00000000	Luma gain control
	volatile TCC_DEF32BIT_TYPE	BRIGHT;			// 0x020  R/W  0x00000000	Luma offset control
	volatile TCC_DEF32BIT_TYPE	FSC_ADJM;		// 0x024  R/W  0x00000000	Color burst frequency adjustment A
	volatile TCC_DEF32BIT_TYPE	FSC_ADJL;		// 0x028  R/W  0x00000000	Color burst frequency adjustment B
	volatile TCC_DEF32BIT_TYPE	ECMDC;			// 0x02C  R/W  0x00000000	Encoder control register C
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	DACSEL;			// 0x040  R/W  0x00000000	DAC output select
	unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	DACPD;			// 0x050  R/W  0x00000000	DAC power down /enable control
	unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ICNTL;			// 0x080  R/W  0x00000000	Timing configuration register
	volatile TCC_DEF32BIT_TYPE	HVOFFST;		// 0x084  R/W  0x00000000	Horizontal active video location A
	volatile TCC_DEF32BIT_TYPE	HOFFST;			// 0x088  R/W  0x00000000	Horizontal active video location B
	volatile TCC_DEF32BIT_TYPE	VOFFST;			// 0x08C  R/W  0x00000000	Channel P vertical active location
	volatile TCC_DEF32BIT_TYPE	HSVSO;			// 0x090  R/W  0x00000000	Interlaced horizontal sync trailing location A
	volatile TCC_DEF32BIT_TYPE	HSOE;			// 0x094  R/W  0x00000000	Interlaced horizontal sync trailing location B
	volatile TCC_DEF32BIT_TYPE	HSOB;			// 0x098  R/W  0x00000000	Interlaced horizontal sync beginning location
	volatile TCC_DEF32BIT_TYPE	VSOB;			// 0x09C  R/W  0x00000000	Vertical sync beginning location
	volatile TCC_DEF32BIT_TYPE	VSOE;			// 0x0A0  R/W  0x00000000	Vertical sync ending location
} NTSCPAL, *PNTSCPAL;

typedef struct _NTSCPAL_ENCODER_CTRL {
	volatile TCC_DEF32BIT_TYPE	VENCON;			// 0x800  R/W  0x00000000	VENC control register
	volatile TCC_DEF32BIT_TYPE	VENCIF;			// 0x804  R/W  0x00000000	VENC interface register
} NTSCPAL_ENCODER_CTRL, *PNTSCPAL_ENCODER_CTRL;


/************************************************************************
*   5. LVDS                                      (Base Addr = 0x)
*************************************************************************/


/************************************************************************
*   6. HDMI                                      (Base Addr = 0x72300000)
*************************************************************************/

typedef struct _HDMICTRL{
	volatile TCC_DEF32BIT_TYPE	INTC_CON;		// 0x000  R/W  0x00000000	Interrupt Control Register
	volatile TCC_DEF32BIT_TYPE	INTC_FLAG;		// 0x004  R/W  0x00000000	Interrupt Flag Register
	volatile TCC_DEF32BIT_TYPE	AESKEY_VALID;	// 0x008  R    0x00000000	aeskey_valid Register
	volatile TCC_DEF32BIT_TYPE	HPD;			// 0x00C  R    0x00000000	HPD signal
}HDMICTRL, *PHDMICTRL;

typedef struct _HDMICORE{
	/* Control Registers */
	volatile TCC_DEF32BIT_TYPE	HDMI_CON_0;			// 0x000  R/W  HDMI system control register 0  0x00
	volatile TCC_DEF32BIT_TYPE	HDMI_CON_1;			// 0x004  R/W  HDMI system control register 1  0x00
	volatile TCC_DEF32BIT_TYPE	HDMI_CON_2;			// 0x008  R/W  HDMI system control register 2  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE_;			// 0x00C
	volatile TCC_DEF32BIT_TYPE	STATUS;				// 0x010  R/W  HDMI system status register  0x00
	volatile TCC_DEF32BIT_TYPE	PHY_STATUS;			// 0x014  R  PHY status register  0x00
	unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	STATUS_EN;			// 0x020  R/W  HDMI system status enable register  0x00
	unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	HPD;				// 0x030  R/W  HPD control register  0x00
	unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	MODE_SEL;			// 0x040  R/W  HDMI/DVI mode selection  0x00
	volatile TCC_DEF32BIT_TYPE	ENC_EN;				// 0x044  R/W  HDCP encryption enable register  0x00
	unsigned :32; unsigned :32;
	/* Video Related Registers */
	volatile TCC_DEF32BIT_TYPE	BLUE_SCREEN_0;		// 0x050  R/W  Pixel values for blue screen  0x00
	volatile TCC_DEF32BIT_TYPE	BLUE_SCREEN_1;		// 0x054  R/W  Pixel values for blue screen 0x00
	volatile TCC_DEF32BIT_TYPE	BLUE_SCREEN_2;		// 0x058  R/W  Pixel values for blue screen 0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE4;			// 0x05C
	volatile TCC_DEF32BIT_TYPE	HDMI_YMAX;			// 0x060  R/W  Maximum Y (or "R,G,B)" pixel value 0x00
	volatile TCC_DEF32BIT_TYPE	HDMI_YMIN;			// 0x064  R/W  Minimum Y (or "R,G,B)" pixel value 0x00
	volatile TCC_DEF32BIT_TYPE	HDMI_CMAX;			// 0x068  R/W  Maximum Cb/Cr pixel value  0x00
	volatile TCC_DEF32BIT_TYPE	HDMI_CMIN;			// 0x06C  R/W  Minimum Cb/Cr pixel value  0x00
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	H_BLANK[2];			// 0x0A0 ,0x00A4 R/W  Horizontal blanking setting  0x00
	unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	V_BLANK[3];			// 0x0B0,0x00B4, 0x00B8 R/W  Vertical blanking setting  0x00
	unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	H_V_LINE[3];		// 0x0C0,0x00C4, 0x00C8 R/W  Horizontal line & vertical line setting  0x00
	unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	VSYNC_POL;			// 0x0E4  R/W  Vertical sync polarity control register  0x00
	volatile TCC_DEF32BIT_TYPE	INT_PRO_MODE ;		// 0x00E8  R/W  Interlace/Progressive control register  0x00
	unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	V_BLANK_F[3];		// 0x110,0x0114, 0x0118 R/W  Vertical blanking setting for bottom field  0x00
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	H_SYNC_GEN[3] ;		// 0x0120,0x0124 ,0x0128 R/W  Horizontal sync generation setting  0x00
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	V_SYNC_GEN1[3];		// 0x130,0x0134,0x0138  R/W  Vertical sync generation for top field or frame  0x01
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	V_SYNC_GEN2[3];		// 0x140,0x0144,0x0148  R/W Vertical sync generation for bottom field   - vertical position 0x01
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	V_SYNC_GEN3[3];		// 0x150,0x0154,0x0158  R/W Vertical sync generation for bottom field   - horizontal position 0x01
	unsigned :32;
	//Audio Related Registers
	volatile TCC_DEF32BIT_TYPE	ASP_CON;			// 0x160  R/W  ASP packet control register  0x00
	volatile TCC_DEF32BIT_TYPE	ASP_SP_FLAT;		// 0x164  R/W  ASP packet sp_flat bit control  0x00
	unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ASP_CHCFG[4];		// 0x170,0x0174,0x0178,0x017C  R/W  ASP audio channel configuration  0x04
	volatile TCC_DEF32BIT_TYPE	ACR_CON;			// 0x180  R/W  ACR packet control register  0x00
	volatile TCC_DEF32BIT_TYPE	ACR_MCTS[3];		// 0x184,0x0188,0x018C  R/W  Measured CTS value  0x01
	volatile TCC_DEF32BIT_TYPE	ACR_CTS[3];			// 0x190,0x0194,0x0198  R/W  CTS value for fixed CTS transmission mode.  0xe8
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ACR_N[3];			// 0x1A0,0x01A4,0x01A8  R/W  N value for ACR packet  0xe8
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ACR_LSB2;			// 0x1B0  R/W  Alternate LSB for fixed CTS transmission mode  0x00
	volatile TCC_DEF32BIT_TYPE	ACR_TXCNT;			// 0x1B4  R/W  Number of ACR packet transmission per frame  0x1f
	volatile TCC_DEF32BIT_TYPE	ACR_TXINTERVAL;		// 0x1B8  R/W  Interval for ACR packet transmission  0x63
	volatile TCC_DEF32BIT_TYPE	ACR_CTS_OFFSET;		// 0x1BC  R/W  CTS offset for measured CTS mode  0x00
	//Packet Related Registers
	volatile TCC_DEF32BIT_TYPE	GCP_CON;			// 0x01C0  R/W  ACR packet control register  0x00
	unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	GCP_BYTE[3];            // 0x1D0,0x01D4,0x01D8  R/W  GCP packet body  0x00
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ACP_CON;                // 0x1E0  R/W  ACP packet control register  0x00
	unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ACP_TYPE;               // 0x1F0  R/W  ACP packet header  0x00
	unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ACP_DATA[17];           // 0x0200~0x0240  R/W ACP packet body 0x00
	unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ISRC_CON;               // 0x250  R/W  ACR packet control register  0x00
	unsigned :32; unsigned :32; unsigned :32;
	unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ISRC1_HEADER1;          // 0x264  R/W  ISCR1 packet header  0x00
	unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	ISRC1_DATA[16];         // 0x270~0x02AC R/W ISRC1 packet body 0x00
	volatile TCC_DEF32BIT_TYPE	ISRC2_DATA[16];         // 0x2B0~0x02EC R/W ISRC2 packet body 0x00
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;

	volatile TCC_DEF32BIT_TYPE	AVI_CON;                // 0x300  R/W  AVI packet control register  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE26[3];         // 0x304,0x0308,0x030c
	volatile TCC_DEF32BIT_TYPE	AVI_CHECK_SUM;          // 0x310  R/W  AVI packet checksum  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE27[3];         // 0x314,0x0318,0x031c
	volatile TCC_DEF32BIT_TYPE	AVI_BYTE[13];           // 0x320~0x0350 R/W AVI packet body 0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE28[3];         // 0x354,0x0358,0x035c
	volatile TCC_DEF32BIT_TYPE	AUI_CON;                // 0x360  R/W  AUI packet control register  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE29[3];         // 0x364,0x0368,0x036c
	volatile TCC_DEF32BIT_TYPE	AUI_CHECK_SUM;          // 0x370  R/W  AUI packet checksum  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE30[3];         // 0x374,0x0378,0x037c
	volatile TCC_DEF32BIT_TYPE	AUI_BYTE[5];            // 0x0380~0x0390  R/W AUI packet body 0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE31[3];         // 0x394,0x0398,0x039c

	volatile TCC_DEF32BIT_TYPE	MPG_CON;                // 0x3A0  R/W  ACR packet control register  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE32[3];         // 0x3A4,0x03A8,0x03Ac
	volatile TCC_DEF32BIT_TYPE	MPG_CHECK_SUM;          // 0x3B0  R/W  MPG packet checksum  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE33[3];         // 0x3B4,0x03B8,0x03Bc

	volatile TCC_DEF32BIT_TYPE	MPG_BYTE[5];            // 0x03C0~0x03D0  R/W MPG packet body 0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE34[11];        // 0x3D4,0x03D8,0x03Dc
	                                                // 0x3E0,
	                                                // 0x3F
	volatile TCC_DEF32BIT_TYPE	SPD_CON;                // 0x400  R/W  SPD packet control register  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE35[3];         // 0x344,0x0348,0x034c
	volatile TCC_DEF32BIT_TYPE	SPD_HEADER0;            // 0x410~  R/W  SPD packet header  0x00
	volatile TCC_DEF32BIT_TYPE	SPD_HEADER1;            // 0x414  R/W  SPD packet header  0x00
	volatile TCC_DEF32BIT_TYPE	SPD_HEADER2;            // 0x418  R/W  SPD packet header  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE36;            // 0x41C
	volatile TCC_DEF32BIT_TYPE	SPD_DATA[28];           //   0x0420~0x048C R/W SPD packet body 0x00

	//HDCP Related Registes
	volatile TCC_DEF32BIT_TYPE	GAMUT_CON;              // 0x500  R/W  GAMUT packet control register  0x00
	volatile TCC_DEF32BIT_TYPE	GAMUT_HEADER0;          // 0x504  R/W  GAMUT packet header  0x00
	volatile TCC_DEF32BIT_TYPE	GAMUT_HEADER1;          // 0x508  R/W  GAMUT packet header  0x00
	volatile TCC_DEF32BIT_TYPE	GAMUT_HEADER2;          // 0x50C  R/W  GAMUT packet header  0x00
	volatile TCC_DEF32BIT_TYPE	GAMUT_DATA[28];         // 0x0510~0x057C R/W  GAMUT packet body  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE37[16];        // 0x580~
	                                                // 0x590,~
	                                                // 0x5A0~
	                                                // 0x5B0~
	volatile TCC_DEF32BIT_TYPE	DC_CONTROL;             // 0x5C0  R/W  Deep Color Control Register  0x00
	volatile TCC_DEF32BIT_TYPE	VIDEO_PATTERN_GEN;      // 0x5C4  R/W  Video Pattern Generation Register  0x00
	volatile TCC_DEF32BIT_TYPE	HPD_GEN ;               // 0x05C8  R/W  HPD Duration value register  0x01
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE38[113];       // 0x5CC
	                                                // 0x5D0,
	                                                // 0x5E0
	                                                // 0x5F0

	volatile TCC_DEF32BIT_TYPE	HDCP_SHA1[20];          // 0x600~0x064C R/W  SHA-1 value from repeater  0x00
	volatile TCC_DEF32BIT_TYPE	HDCP_KSV_LIST[5];       // 0x0650~0x0660 R/W  KSV list from repeater  0x00

	volatile TCC_DEF32BIT_TYPE	HDCP_KSV_LIST_CON;      // 0x664  R/W  KSV list control  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE39[2];         // 0x668,0x066C
	volatile TCC_DEF32BIT_TYPE	HDCP_SHA_RESULT;        // 0x670  R/W  SHA-1 checking result register  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE40[3];         // 0x674,0x0678,0x067c
	volatile TCC_DEF32BIT_TYPE	HDCP_CTRL1;             // 0x680  R/W  HDCP control register1  0x00
	volatile TCC_DEF32BIT_TYPE	HDCP_CTRL2;             // 0x684  R/W  HDCP control register2  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE41[2];         // 0x688,0x068c
	volatile TCC_DEF32BIT_TYPE	HDCP_CHECK_RESULT;      // 0x690  R/W  Ri and Pj value checking result  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE42[3];         // 0x394,0x0398,0x039c
	volatile TCC_DEF32BIT_TYPE	HDCP_BKSV[5];           // 0x06A0~0x06B0 R/W  KSV of Rx  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE43[3];         // 0x6B4,0x06B8,0x06Bc

	volatile TCC_DEF32BIT_TYPE	HDCP_AKSV[5];           // 0x006C0~ 0x06D0 R/W  KSV of Tx  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE44[3];         // 0x6D4,0x06D8,0x06Dc
	volatile TCC_DEF32BIT_TYPE	HDCP_An[8];             // 0x06E0~ 0x06FC R/W  An value  0x00
	volatile TCC_DEF32BIT_TYPE	HDCP_BCAPS;             // 0x700  R/W  BCAPS from Rx  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE45[3];         // 0x704,0x0708,0x070c
	volatile TCC_DEF32BIT_TYPE	HDCP_BSTATUS_0;         // 0x710  R/W  BSTATUS from Rx  0x00
	volatile TCC_DEF32BIT_TYPE	HDCP_BSTATUS_1;         // 0x714  R/W  BSTATUS from Rx  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE46[10];        // 0x718,0x071c
	                                                // 0x720,
	                                                // 0x730
	volatile TCC_DEF32BIT_TYPE	HDCP_Ri_0;              // 0x740  R/W  Ri value of Tx  0x00
	volatile TCC_DEF32BIT_TYPE	HDCP_Ri_1;              // 0x744  R/W  Ri value of Tx  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE47[13];        // 0x748
	                                                // 0x750,
	                                                // 0x760,
	                                                // 0x770
	volatile TCC_DEF32BIT_TYPE	HDCP_I2C_INT;           // 0x780  R/W  I2C interrupt flag  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE48[3];         // 0x784,0x0788,0x078c

	volatile TCC_DEF32BIT_TYPE	HDCP_AN_INT;            // 0x790  R/W  An value ready interrupt flag  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE49[3];         // 0x794,0x0798,0x079c

	volatile TCC_DEF32BIT_TYPE	HDCP_WATCGDOG_INT;      // 0x7A0  R/W  Wachdog interrupt flag  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE50[3];         // 0x7A4,0x07A8,0x07Ac

	volatile TCC_DEF32BIT_TYPE	HDCP_Ri_INT;            // 0x7B0  R/W  Ri value update interrupt flag  0x00
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE51[7];         // 0x7B4,0x07B8,0x07Bc
	                                                // 0x7C0,
	volatile TCC_DEF32BIT_TYPE	HDCP_Ri_Compare_0;      // 0x7D0  R/W  HDCP Ri Interrupt Frame number index register 0  0x80
	volatile TCC_DEF32BIT_TYPE	HDCP_Ri_Compare_1;      // 0x7D4  R/W  HDCP Ri Interrupt Frame number index register 1  0x7f
	volatile TCC_DEF32BIT_TYPE	NOTDEFINE52[2];         // 0x7D8,0x07Dc

	volatile TCC_DEF32BIT_TYPE	HDCP_Frame_Count;       // 0x07E0  R  Current value of the frame count index in the hardware 0x00
}HDMICORE, *PHDMICORE;

typedef struct _HDMIAES{
	volatile TCC_DEF32BIT_TYPE	AES_START;			// 0x000  R/W  0x00000000	AES_START
	unsigned :32; unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	AES_DATA_SIZE_L;	// 0x020  R/W  0x00000020	AES_DATA_SIZE_L
	volatile TCC_DEF32BIT_TYPE	AES_DATA_SIZE_H;	// 0x024  R/W  0x00000001	AES_DATA_SIZE_H
	unsigned :32; unsigned :32;
	unsigned :32; unsigned :32; unsigned :32; unsigned :32;
	volatile TCC_DEF32BIT_TYPE	AES_DATA;			// 0x040  W    -			AES_DATA
}HDMIAES, *PHDMIAES;

typedef struct _HDMISPDIF{
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CLK_CTRL;		//  0x0000  R/W  SPDIFIN Clock Control Register  0x02
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_OP_CTRL;		//  0x0004  R/W  SPDIFIN Operation Control Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_IRQ_MASK;		//  0x0008  R/W  SPDIFIN Interrupt Request Mask Register  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_IRQ_STATUS;		//  0x000C  R/W  SPDIFIN Interrupt Request Status Register  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CONFIG_1;		//  0x0010  R/W  SPDIFIN Configuration Register 1  0x02
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CONFIG_2;		//  0x0014  R/W  SPDIFIN Configuration Register 2  0x00
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE0[2];              //  0x0018 0x001C -  Reserved  -
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_USER_VALUE_1;       //  0x0020  R  SPDIFIN User Value Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_USER_VALUE_2;       //  0x0024  R  SPDIFIN User Value Register 2  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_USER_VALUE_3;       //  0x0028  R  SPDIFIN User Value Register 3  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_USER_VALUE_4;       //  0x002C  R  SPDIFIN User Value Register 4  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CH_STATUS_0_1;      //  0x0030  R  SPDIFIN Channel Status Register 0-1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CH_STATUS_0_2;      //  0x0034  R  SPDIFIN Channel Status Register 0-2  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CH_STATUS_0_3;      //  0x0038  R  SPDIFIN Channel Status Register 0-3  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CH_STATUS_0_4;      //  0x003C  R  SPDIFIN Channel Status Register 0-4  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_CH_STATUS_1;        //  0x0040  R  SPDIFIN Channel Status Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE1;                 //  0x0044 -  Reserved  -
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_FRAME_PERIOD_1;     //  0x0048  R  SPDIFIN Frame Period Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_FRAME_PERIOD_2;     //  0x004C  R  SPDIFIN Frame Period Register 2  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_Pc_INFO_1;          //  0x0050  R  SPDIFIN Pc Info Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_Pc_INFO_2;          //  0x0054  R  SPDIFIN Pc Info Register 2  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_Pd_INFO_1;          //  0x0058  R  SPDIFIN Pd Info Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_Pd_INFO_2;          //  0x005C  R  SPDIFIN Pd Info Register 2  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_DATA_BUF_0_1;       //  0x0060  R  SPDIFIN Data Buffer Register 0_1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_DATA_BUF_0_2;       //  0x0064  R  SPDIFIN Data Buffer Register 0_2  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_DATA_BUF_0_3;       //  0x0068  R  SPDIFIN Data Buffer Register 0_3  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_USER_BUF_0;         //  0x006C  R  SPDIFIN User Buffer Register 0  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_DATA_BUF_1_1;       //  0x0070  R  SPDIFIN Data Buffer Register 1_1  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_DATA_BUF_1_2;       //  0x0074  R  SPDIFIN Data Buffer Register 1_2  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_DATA_BUF_1_3;       //  0x0078  R  SPDIFIN Data Buffer Register 1_3  0x00
    volatile TCC_DEF32BIT_TYPE	SPDIFIN_USER_BUF_1;         //  0x007C  R  SPDIFIN User Buffer Register 1  0x00
}HDMISPDIF, *PHDMISPDIF;

typedef struct _HDMII2S{
    volatile TCC_DEF32BIT_TYPE	I2S_CLK_CON;                //  0x0000  R/W  I2S Clock Enable Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CON_1;                  //  0x0004  R/W  I2S Control Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CON_2;                  //  0x0008  R/W  I2S Control Register 2  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_PIN_SEL_0;              //  0x000C  R/W  I2S Input Pin Selection Register 0  0x77
    volatile TCC_DEF32BIT_TYPE	I2S_PIN_SEL_1;              //  0x0010  R/W  I2S Input Pin Selection Register 1  0x77
    volatile TCC_DEF32BIT_TYPE	I2S_PIN_SEL_2;              //  0x0014  R/W  I2S Input Pin Selection Register 2  0x77
    volatile TCC_DEF32BIT_TYPE	I2S_PIN_SEL_3;              //  0x0018  R/W  I2S Input Pin Selection Register 3  0x07
    volatile TCC_DEF32BIT_TYPE	I2S_DSD_CON;                //  0x001C  R/W  I2S DSD Control Register  0x02
    volatile TCC_DEF32BIT_TYPE	I2S_MUX_CON;                //  0x0020  R/W  I2S In/Mux Control Register  0x60
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_CON;              //  0x0024  R/W  I2S Channel Status Control Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_0;                //  0x0028  R/W  I2S Channel Status Block 0  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_1;                //  0x002C  R/W  I2S Channel Status Block 1  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_2;                //  0x0030  R/W  I2S Channel Status Block 2  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_3;                //  0x0034  R/W  I2S Channel Status Block 3  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_4;                //  0x0038  R/W  I2S Channel Status Block 4  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_SH_0;             //  0x003C  R  I2S Channel Status Block Shadow Register 0  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_SH_1;             //  0x0040  R  I2S Channel Status Block Shadow Register 1  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_SH_2;             //  0x0044  R  I2S Channel Status Block Shadow Register 2  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_SH_3;             //  0x0048  R  I2S Channel Status Block Shadow Register 3  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH_ST_SH_4;             //  0x004C  R  I2S Channel Status Block Shadow Register 4  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_VD_DATA;                //  0x0050  R/W  I2S Audio Sample Validity Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_MUX_CH;                 //  0x0054  R/W  I2S Channel Enable Register  0x03
    volatile TCC_DEF32BIT_TYPE	I2S_MUX_CUV;                //  0x0058  R/W  I2S CUV Enable Register  0x03
    volatile TCC_DEF32BIT_TYPE	I2S_IRQ_MASK;               //  0x005C  R/W  I2S Interrupt Request Mask Register  0x03
    volatile TCC_DEF32BIT_TYPE	I2S_IRQ_STATUS;             //  0x0060  R/W  I2S Interrupt Request Status Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_L_0;                //  0x0064  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_L_1;                //  0x0068  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_L_2;                //  0x006C  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_L_3;                //  0x0070  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_R_0;                //  0x0074  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_R_1;                //  0x0078  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_R_2;                //  0x007C  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH0_R_3;                //  0x0080  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_L_0;                //  0x0084  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_L_1;                //  0x0088  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_L_2;                //  0x008C  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_L_3;                //  0x0090  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_R_0;                //  0x0094  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_R_1;                //  0x0098  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_R_2;                //  0x009C  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH1_R_3;                //  0x00A0  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH2_L_0;                //  0x00A4  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH2_L_1;                //  0x00A8  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH2_L_2;                //  0x00AC  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH2_L_3;                //  0x00B0  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH2_R_0;                //  0x00B4  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH2_R_1;                //  0x00B8  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH2_R_2;                //  0x00BC  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_Ch2_R_3;                //  0x00C0  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH3_L_0;                //  0x00C4  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH3_L_1;                //  0x00C8  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH3_L_2;                //  0x00CC  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH3_R_0;                //  0x00D0  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH3_R_1;                //  0x00D4  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CH3_R_2;                //  0x00D8  R  I2S PCM Output Data Register  0x00
    volatile TCC_DEF32BIT_TYPE	I2S_CUV_L_R;                //  0x00DC  R  I2S CUV Output Data Register  0x00
}HDMII2S, *PHDMII2S;

typedef struct _HDMICEC{
    volatile TCC_DEF32BIT_TYPE	CEC_TX_STATUS_0;            //  0x0000  R  CEC Tx status register 0.  0x00
    volatile TCC_DEF32BIT_TYPE	CEC_TX_STATUS_1;            //  0x0004  R  CEC Tx status register 1. Number of blocks transferred. 0x00
    volatile TCC_DEF32BIT_TYPE	CEC_RX_STATUS_0;            //  0x0008  R  CEC Rx status register 0.  0x00
    volatile TCC_DEF32BIT_TYPE	CEC_RX_STATUS_1;            //  0x000C  R  CEC Rx status register 1. Number of blocks received. 0x00
    volatile TCC_DEF32BIT_TYPE	CEC_INTR_MASK;              //  0x0010  R/W  CEC interrupt mask register  0x00
    volatile TCC_DEF32BIT_TYPE	CEC_INTR_CLEAR;             //  0x0014  R/W  CEC interrupt clear register  0x00
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE0[2];              //  0x0018 0x001C -  Reserved  -
    volatile TCC_DEF32BIT_TYPE	CEC_LOGIC_ADDR;             //  0x0020  R/W  HDMI Tx logical address register  0x0F
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE1[3];              //  0x0024 0x0028, 0x002C -  Reserved  -
    volatile TCC_DEF32BIT_TYPE	CEC_DIVISOR_0;              //  0x0030  R/W  Clock divisor for 0.05ms period count ([7:0] of 32-bit) 0x00
    volatile TCC_DEF32BIT_TYPE	CEC_DIVISOR_1;              //  0x0034  R/W  Clock divisor for 0.05ms period count ([15:8] of 32-bit) 0x00
    volatile TCC_DEF32BIT_TYPE	CEC_DIVISOR_2;              //  0x0038  R/W  Clock divisor for 0.05ms period count ([23:16] of 32-bit) 0x00
    volatile TCC_DEF32BIT_TYPE	CEC_DIVISOR_3;              //  0x003C  R/W  Clock divisor for 0.05ms period count ([31:24] of 32-bit) 0x00
//CEC Tx related Registers
    volatile TCC_DEF32BIT_TYPE	CEC_TX_CTRL;                //  0x0040  R/W  CEC Tx control register  0x10
    volatile TCC_DEF32BIT_TYPE	CEC_TX_BYTE_NUM;            //  0x0044  R/W  Number of blocks in a message to be transferred  0x00
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE2[6];              //  0x0048 0x004C -  Reserved  -
                                                        //  0x0050
    volatile TCC_DEF32BIT_TYPE	CEC_TX_STATUS_2;            //  0x0060  R  CEC Tx status register 2  0x00
    volatile TCC_DEF32BIT_TYPE	CEC_TX_STATUS_3;            //  0x0064  R  CEC Tx status register 3  0x00
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE3[6];              //  0x0068 0x006C -  Reserved  -
                                                        //  0x0070
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE4[3];              //  0x0074 0x0078, 0x007C -  Reserved  -
    volatile TCC_DEF32BIT_TYPE	CEC_TX_BUFFER[16];          // 0x0080 ~ 0x00BC R/W  Byte #0 ~ #15 of CEC message to be transferred. (#0 is transferred 1st ) 0x00
//CEC Rx related Registers
    volatile TCC_DEF32BIT_TYPE	CEC_RX_CTRL;                //  0x00C0  R/W  CEC Rx control register  0x00
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE5[7];              //  0x00C4 0x00C8, 0x00CC -  Reserved  -
                                                        //  0x00D0
    volatile TCC_DEF32BIT_TYPE	CEC_RX_STATUS_2;            //  0x00E0  R  CEC Rx status register 2  0x00
    volatile TCC_DEF32BIT_TYPE	CEC_RX_STATUS_3;            //  0x00E4  R  CEC Rx status register 3eived 1st )  0x00
    volatile TCC_DEF32BIT_TYPE	NOTDEFINE6[2];              //  0x00E8, 0x00EC -  Reserved  -
    volatile TCC_DEF32BIT_TYPE	CEC_RX_BUFFER[16];          // 0x0100 ~ 0x013C R  Byte #0 ~ #15 of CEC message received (#0 is received 1st ) 0x00
}HDMICEC, *PHDMICEC;


/************************************************************************
*   7. NTSC / PAL Encoder Composite Output       (Base Addr = 0x00000000)
*************************************************************************/


/************************************************************************
*   8. LVDS                                      (Base Addr = 0x00000000)
*************************************************************************/


/************************************************************************
*   9. DDI_CONFIG                                (Base Addr = 0x72380000)
*************************************************************************/

typedef struct {
	unsigned LCDC			:1;
	unsigned NTSCPAL		:1;
	unsigned HDMI			:1;
	unsigned				:29;
} DDI_CONFIG_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_CONFIG_IDX_TYPE		bREG;
} DDI_CONFIG_TYPE;

typedef struct {
	unsigned CIF0			:3;
	unsigned				:1;
	unsigned CIF1			:3;
	unsigned				:1;
	unsigned CIF2			:3;
	unsigned				:1;
	unsigned CIF3			:3;
	unsigned				:1;
	unsigned DEMUX			:3;
	unsigned				:13;
} DDI_CIFPORT_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_CIFPORT_IDX_TYPE	bREG;
} DDI_CIFPORT_TYPE;

typedef struct {
	unsigned RESET			:4;
	unsigned				:11;
	unsigned EN				:1;
	unsigned				:16;
} DDI_HDMICTRL_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_HDMICTRL_IDX_TYPE	bREG;
} DDI_HDMICTRL_TYPE;

typedef struct {
	unsigned VLD			:1;
	unsigned				:31;
} DDI_HDMIAES_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_HDMIAES_IDX_TYPE	bREG;
} DDI_HDMIAES_TYPE;

typedef struct {
	unsigned DATA			:32;
} DDI_HDMIAESD0_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_HDMIAESD0_IDX_TYPE	bREG;
} DDI_HDMIAESD0_TYPE;

typedef struct {
	unsigned DATA			:1;
	unsigned				:31;
} DDI_HDMIAESD1_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_HDMIAESD1_IDX_TYPE	bREG;
} DDI_HDMIAESD1_TYPE;

typedef struct {
	unsigned HW				:32;
} DDI_HDMIAESHW_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_HDMIAESHW_IDX_TYPE	bREG;
} DDI_HDMIAESHW_TYPE;

typedef struct {
	unsigned HW				:31;
	unsigned				:1;
} DDI_HDMIAESHW2_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_HDMIAESHW2_IDX_TYPE	bREG;
} DDI_HDMIAESHW2_TYPE;

typedef struct {
	unsigned SEL			:1;
	unsigned				:31;
} DDI_NTSCPALEN_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_NTSCPALEN_IDX_TYPE	bREG;
} DDI_NTSCPALEN_TYPE;

typedef struct {
	unsigned 					:1;
	unsigned RST				:1;
	unsigned EN				:1;
	unsigned OC				:1;
	unsigned VSEL			:1;
	unsigned S				:3;
	unsigned M				:7;
	unsigned P				:6;
	unsigned TC				:3;
	unsigned					:6;
	unsigned	SEL				:2;
} DDI_LVDSCTRL_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSCTRL_IDX_TYPE	bREG;
} DDI_LVDSCTRL_TYPE;

typedef struct {
	unsigned SEL_00			:8;
	unsigned SEL_01			:8;
	unsigned SEL_02			:8;
	unsigned SEL_03			:8;
} DDI_LVDSTXSEL0_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL0_IDX_TYPE	bREG;
} DDI_LVDSTXSEL0_TYPE;

typedef struct {
	unsigned SEL_04			:8;
	unsigned SEL_05			:8;
	unsigned SEL_06			:8;
	unsigned SEL_07			:8;
} DDI_LVDSTXSEL1_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL1_IDX_TYPE	bREG;
} DDI_LVDSTXSEL1_TYPE;

typedef struct {
	unsigned SEL_08			:8;
	unsigned SEL_09			:8;
	unsigned SEL_10			:8;
	unsigned SEL_11			:8;
} DDI_LVDSTXSEL2_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL2_IDX_TYPE	bREG;
} DDI_LVDSTXSEL2_TYPE;

typedef struct {
	unsigned SEL_12			:8;
	unsigned SEL_13			:8;
	unsigned SEL_14			:8;
	unsigned SEL_15			:8;
} DDI_LVDSTXSEL3_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL3_IDX_TYPE	bREG;
} DDI_LVDSTXSEL3_TYPE;

typedef struct {
	unsigned SEL_16			:8;
	unsigned SEL_17			:8;
	unsigned SEL_18			:8;
	unsigned SEL_19			:8;
} DDI_LVDSTXSEL4_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL4_IDX_TYPE	bREG;
} DDI_LVDSTXSEL4_TYPE;

typedef struct {
	unsigned SEL_20			:8;
	unsigned SEL_21			:8;
	unsigned SEL_22			:8;
	unsigned SEL_23			:8;
} DDI_LVDSTXSEL5_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL5_IDX_TYPE	bREG;
} DDI_LVDSTXSEL5_TYPE;

typedef struct {
	unsigned SEL_24			:8;
	unsigned SEL_25			:8;
	unsigned SEL_26			:8;
	unsigned SEL_27			:8;
} DDI_LVDSTXSEL6_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL6_IDX_TYPE	bREG;
} DDI_LVDSTXSEL6_TYPE;

typedef struct {
	unsigned SEL_28			:8;
	unsigned SEL_29			:8;
	unsigned SEL_30			:8;
	unsigned SEL_31			:8;
} DDI_LVDSTXSEL7_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL7_IDX_TYPE	bREG;
} DDI_LVDSTXSEL7_TYPE;

typedef struct {
	unsigned SEL_32			:8;
	unsigned SEL_33			:8;
	unsigned SEL_34			:8;
	unsigned				:8;
} DDI_LVDSTXSEL8_IDX_TYPE;

typedef union {
	unsigned long			nREG;
	DDI_LVDSTXSEL8_IDX_TYPE	bREG;
} DDI_LVDSTXSEL8_TYPE;

typedef struct _DDICONFIG{
	volatile DDI_CONFIG_TYPE		PWDN;			// 0x000  R/W  0x00000000	Power Down
	volatile DDI_CONFIG_TYPE		SWRESET;		// 0x004  R/W  0x00000000	Soft Reset
	unsigned :32;
	volatile DDI_CIFPORT_TYPE		CIFPORT;		// 0x00C  R/W  0x00043210	CIF select
	volatile DDI_HDMICTRL_TYPE		HDMI_CTRL;		// 0x010  R/W  0x00000000	HDMI Control
	volatile DDI_HDMIAES_TYPE		HDMI_AES;		// 0x014  R/W  0x00000000	HDMI AES
	volatile DDI_HDMIAESD0_TYPE		HDMI_AES_DATA0;	// 0x018  R/W  0x00000000	HDMI AES DATA #0
	volatile DDI_HDMIAESD1_TYPE		HDMI_AES_DATA1;	// 0x01C  R/W  0x00000000	HDMI AES DATA #1
	volatile DDI_HDMIAESHW_TYPE		HDMI_AES_HW0;	// 0x020  R/W  0x00000000	HDMI AES HW #0
	volatile DDI_HDMIAESHW_TYPE		HDMI_AES_HW1;	// 0x024  R/W  0x00000000	HDMI AES HW #1
	volatile DDI_HDMIAESHW2_TYPE	HDMI_AES_HW2;	// 0x028  R/W  0x00000000	HDMI AES HW #2
	unsigned :32;
	volatile DDI_NTSCPALEN_TYPE		NTSCPAL_EN;		// 0x030  R/W  0x00000001	NTSCPAL Encoder Enable
	unsigned :32; unsigned :32; unsigned :32;
	volatile DDI_LVDSCTRL_TYPE		LVDS_CTRL;		// 0x040  R/W  0x00850A01	LVDS Control register
	volatile DDI_LVDSTXSEL0_TYPE	LVDS_TXO_SEL0;	// 0x044  R/W  0x03020100	LVDS TXOUT select #0
	volatile DDI_LVDSTXSEL1_TYPE	LVDS_TXO_SEL1;	// 0x048  R/W  0x09080504	LVDS TXOUT select #1
	volatile DDI_LVDSTXSEL2_TYPE	LVDS_TXO_SEL2;	// 0x04C  R/W  0x0D0C0B0A	LVDS TXOUT select #2
	volatile DDI_LVDSTXSEL3_TYPE	LVDS_TXO_SEL3;	// 0x050  R/W  0x13121110	LVDS TXOUT select #3
	volatile DDI_LVDSTXSEL4_TYPE	LVDS_TXO_SEL4;	// 0x054  R/W  0x1A191514	LVDS TXOUT select #4
	volatile DDI_LVDSTXSEL5_TYPE	LVDS_TXO_SEL5;	// 0x058  R/W  0x0E070618	LVDS TXOUT select #5
	volatile DDI_LVDSTXSEL6_TYPE	LVDS_TXO_SEL6;	// 0x05C  R/W  0x1B17160F	LVDS TXOUT select #6
	volatile DDI_LVDSTXSEL7_TYPE	LVDS_TXO_SEL7;	// 0x060  R/W  0x1F1E1F1E	LVDS TXOUT select #7
	volatile DDI_LVDSTXSEL8_TYPE	LVDS_TXO_SEL8;	// 0x064  R/W  0x001E1F1E	LVDS TXOUT select #8
}DDICONFIG, *PDDICONFIG;

#endif /* _STRUCTURES_DISPLAY_H_ */
