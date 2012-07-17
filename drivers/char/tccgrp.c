/*
 * drivers/char/Tccgrp.c
 *
 * Copyright (C) 2004 Texas Instruments, Inc. 
 *
 * Video-for-Linux (Version 2) graphic capture driver for
 * the OMAP H2 and H3 camera controller.
 *
 * Adapted from omap24xx driver written by Andy Lowe (source@mvista.com)
 * Copyright (C) 2003-2004 MontaVista Software, Inc.
 * 
 * This package is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
 * 
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * History:
 *   27/03/05   Vladimir Barinov - Added support for power management
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>

#include <mach/bsp.h>

//#include <asm/plat-tcc/tcc_ckc.h>
#include <linux/poll.h>
#include <linux/cpufreq.h>


#include "tccgrp.h"
#include "tcc_Gre2d.h"
#include "tcc_Gre2d_api.h"
#include <mach/tcc_Gre2d_type.h>
#include <mach/tcc_grp_ioctrl.h>
#include <mach/tca_fb_output.h>

extern G2D_DITHERING_TYPE gG2D_Dithering_type;
extern unsigned char gG2D_Dithering_en;

static int debug  = 0;
#define dprintk(msg...)	if (debug) { printk( "tcc_grp: " msg); }

#if 0
#define GRP_DBG(msg...)		printk(msg)
#else
#define GRP_DBG(msg...)
#endif

#define DEVICE_NAME			"graphic"
#define MAJOR_ID			199
#define MINOR_ID			1

typedef struct _intr_data_t {
	unsigned int  dev_opened;

	wait_queue_head_t poll_wq;
	wait_queue_head_t cmd_wq;

	spinlock_t g2d_spin_lock;
	struct mutex io_mutex;

	unsigned char block_waiting;
	unsigned char block_operating;
}intr_data_t;

static intr_data_t g2d_struct;
static struct clk *overlay_clk;

/* YUV FORMAT의 U, V 의 ADDRESS를 구함 */
void grp_get_address(G2D_DATA_FM fmt, unsigned int add, short  src_imgx, short  src_imgy,
							unsigned int* U,unsigned int* V)
{
	if(fmt == GE_YUV420_sp)
	{
		*U = GET_ADDR_YUV42X_spU(add, src_imgx, src_imgy);
		*V = GET_ADDR_YUV420_spV(*U, src_imgx, src_imgy);
	}
	else
	{
		*U = GET_ADDR_YUV42X_spU(add, src_imgx, src_imgy);
		*V = GET_ADDR_YUV422_spV(*U, src_imgx, src_imgy);
	}
}


void tccxxx_grp_response_check(unsigned int	responsetype)
{
	int ret;

	if(responsetype == G2D_INTERRUPT) {
		dprintk("tccxxx_grp_response_check:[%d][%d]\n", responsetype, g2d_struct.block_waiting);
			// call poll function 
	}
	else if(responsetype == G2D_POLLING) {
		dprintk("tccxxx_grp_response_check:[%d][%d]\n", responsetype, g2d_struct.block_waiting);

		ret = wait_event_interruptible_timeout(g2d_struct.poll_wq,  g2d_struct.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0) {
			printk("G2D time out :%d Line :%d \n", __LINE__, ret);
		}
	}
}

int grp_common_ctrl(G2D_COMMON_TYPE *g2d_p)
{
	unsigned int srcY = 0, srcU = 0, srcV = 0;
	unsigned int tgtY = 0, tgtU = 0, tgtV = 0;
	unsigned int result = 0;

	srcY = g2d_p->src0;
	
	if( g2d_p->src1 == 0 || g2d_p->src2  == 0 )
		grp_get_address(g2d_p->srcfm.format, srcY , g2d_p->src_imgx ,g2d_p->src_imgy, &srcU, &srcV);
	else
	{
		srcU = g2d_p->src1;
		srcV = g2d_p->src2;
	}
	
	if(g2d_p->DefaultBuffer)
	{
		return -EFAULT;
	}
	else
	{
		tgtY = g2d_p->tgt0;

		if( g2d_p->tgt1 == 0 || g2d_p->tgt2  == 0 )
			grp_get_address(g2d_p->tgtfm.format, tgtY, g2d_p->dst_imgx, g2d_p->dst_imgy, &tgtU, &tgtV);	
		else
		{
			tgtU = g2d_p->tgt1;
			tgtV = g2d_p->tgt2;
		}
	}		


	if((g2d_p->ch_mode == ROTATE_90) || (g2d_p->ch_mode == ROTATE_270))
	{
		if(g2d_p->crop_imgy > (g2d_p->dst_imgx - g2d_p->dst_off_x))
		{
			dprintk("rg2d_p->crop_imgx[%d][%d][%d]!!!\n", g2d_p->crop_imgx, g2d_p->dst_imgx, g2d_p->dst_off_x);
			g2d_p->crop_imgy = g2d_p->dst_imgx - g2d_p->dst_off_x;
		}

		if(g2d_p->crop_imgx > (g2d_p->dst_imgy - g2d_p->dst_off_y))
		{
			dprintk("rg2d_p->crop_imgy[%d][%d][%d]!!!\n", g2d_p->crop_imgy, g2d_p->dst_imgy, g2d_p->dst_off_y);
			g2d_p->crop_imgx = g2d_p->dst_imgy - g2d_p->dst_off_y;
		}
	}
	else
	{
		if(g2d_p->crop_imgx > (g2d_p->dst_imgx - g2d_p->dst_off_x))
		{
			dprintk("g2d_p->crop_imgx[%d][%d][%d]!!!\n", g2d_p->crop_imgx, g2d_p->dst_imgx, g2d_p->dst_off_x);
			g2d_p->crop_imgx = g2d_p->dst_imgx - g2d_p->dst_off_x;
		}
		
		if(g2d_p->crop_imgy > (g2d_p->dst_imgy - g2d_p->dst_off_y))
		{
			dprintk("g2d_p->crop_imgy[%d][%d][%d]!!!\n", g2d_p->crop_imgy, g2d_p->dst_imgy, g2d_p->dst_off_y);
			g2d_p->crop_imgy = g2d_p->dst_imgy - g2d_p->dst_off_y;
		}
	}
	
	gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);

	if((g2d_p->srcfm.format <= GE_YUV422_sq) && (g2d_p->tgtfm.format == GE_RGB565))	
	{
		gG2D_Dithering_en = 1;
		gG2D_Dithering_type = ADD_1_OP;
	}
	else
	{
		gG2D_Dithering_en = 0;
	}

	spin_lock_irq(&(g2d_struct.g2d_spin_lock));

	gre2d_ImgRotate_Ex(srcY , srcU, srcV, 
							g2d_p->srcfm , g2d_p->src_imgx ,g2d_p->src_imgy,
							g2d_p->crop_offx, g2d_p->crop_offy, g2d_p->crop_imgx, g2d_p->crop_imgy, 
							tgtY,tgtU, tgtV,
							g2d_p->tgtfm, g2d_p->dst_imgx, g2d_p->dst_imgy, 
							g2d_p->dst_off_x, g2d_p->dst_off_y, g2d_p->ch_mode, g2d_p->parallel_ch_mode);


	result = gre2d_interrupt_ctrl(0, (G2D_INT_TYPE)0, 0, 0);
	spin_unlock_irq(&(g2d_struct.g2d_spin_lock));

	if(gG2D_Dithering_en)	{
		gG2D_Dithering_en = 0;
	}
	
	tccxxx_grp_response_check(g2d_p->responsetype);
	
	return 0;
}


int grp_overlaymixer_ctrl(G2D_BITBLIT_TYPE_1 *g2d_p)
{

	G2D_ROP_FUNC rop;
	G2D_EN	chEn = GRP_DISABLE;
	unsigned int result = 0;
		
	unsigned int src_addr_comp0 = 0, src_addr_comp1 = 0, src_addr_comp2 = 0;
	unsigned int dest_addr_comp = 0;
	unsigned int src_addr0, src_addr1, src_addr2;
	unsigned int dest_addr;

	src_addr0 = g2d_p->src0;
	src_addr1 = g2d_p->src1;
	src_addr2 = g2d_p->src2;
	
	dest_addr = g2d_p->tgt0;

	GRP_DBG("src_addr0:0x%X, src_img(x:%d, y:%d), crop_img(%d:%d:%d:%d) fmt:%d, dev_opened:%d !!!\n",  
				src_addr0, 
				g2d_p->src0_imgx, g2d_p->src0_imgy, 
				g2d_p->src0_crop_offx, g2d_p->src0_crop_offy,  
				g2d_p->src0_crop_imgx, g2d_p->src0_crop_imgy, 
				g2d_p->src0_fmt, g2d_struct.dev_opened);

	GRP_DBG("src_addr1:0x%X, src_img(x:%d, y:%d), crop_img(%d:%d:%d:%d) fmt:%d, block_waiting:%d !!!\n",  
				src_addr1, 
				g2d_p->src1_imgx, g2d_p->src1_imgy, 
				g2d_p->src1_crop_offx, g2d_p->src1_crop_offy,  
				g2d_p->src1_crop_imgx, g2d_p->src1_crop_imgy, 
				g2d_p->src1_fmt, g2d_struct.block_waiting);

	GRP_DBG("src_addr2:0x%X, src_img(x:%d, y:%d), crop_img(%d:%d:%d:%d) fmt:%d, block_operating:%d !!!\n",  
				src_addr2, 
				g2d_p->src2_imgx, g2d_p->src2_imgy, 
				g2d_p->src2_crop_offx, g2d_p->src2_crop_offy,  
				g2d_p->src2_crop_imgx, g2d_p->src2_crop_imgy, 
				g2d_p->src2_fmt, g2d_struct.block_operating);

	GRP_DBG("dst_addr:0x%X, dst_img(x:%d, y:%d), dst_offset(x:%d, y:%d)\n",  
				dest_addr, 
				g2d_p->dst_imgx, g2d_p->dst_imgy,				
				g2d_p->dst_off_x,  g2d_p->dst_off_y);

	GRP_DBG("ch_mode:%d, tgtfm:%d\n", g2d_p->ch_mode, g2d_p->tgtfm);

	src_addr_comp0 = (src_addr0 % 8)/2;
	src_addr_comp1 = (src_addr1 % 8)/2;
	src_addr_comp2 = (src_addr2 % 8)/2;
	
	dest_addr_comp = (dest_addr % 8)/2;
	GRP_DBG("src_addr_comp0:%d, src_addr_comp1:%d, src_addr_comp2:%d, dest_addr_comp:%d\n", \
		src_addr_comp0, src_addr_comp1, src_addr_comp2, dest_addr_comp);

	src_addr0 = (src_addr0>>3)<<3;
	src_addr1 = (src_addr1>>3)<<3;
	src_addr2 = (src_addr2>>3)<<3;
	
	dest_addr = (dest_addr>>3)<<3;
	GRP_DBG("src_addr0:0x%X, src_addr1:0x%X, src_addr2:0x%X, dest_addr:0x%X\n", \
				src_addr0, src_addr1, src_addr2, dest_addr);


	memset(&rop, 0x0, sizeof(G2D_ROP_FUNC));

	rop.src0.add0			= src_addr0;
	rop.src0.frame_pix_sx	= g2d_p->src0_imgx;
	rop.src0.frame_pix_sy	= g2d_p->src0_imgy;
	rop.src0.src_off_sx 	= g2d_p->src0_crop_offx + src_addr_comp0;
	rop.src0.src_off_sy 	= g2d_p->src0_crop_offy;  
	rop.src0.img_pix_sx 	= g2d_p->src0_crop_imgx;
	rop.src0.img_pix_sy 	= g2d_p->src0_crop_imgy;  
	rop.src0.win_off_sx 	= 0;
	rop.src0.win_off_sy 	= 0;  
	rop.src0.op_mode		= NOOP;//g2d_p->ch_mode;
	rop.src0.src_form		= g2d_p->src0_fmt;
	rop.src0.arith_mode 	= AR_NOOP;		// rotation mode setting
	rop.src0.arith_RY		= 0x0;
	rop.src0.arith_GU		= 0x0;
	rop.src0.arith_BV		= 0x0;
#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
	rop.src0.clut_en		= g2d_p->src0_clut_en;
#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */

	if(rop.src0.add0) chEn = GRP_F0;

	rop.src1.add0			= src_addr1;
	rop.src1.frame_pix_sx	= g2d_p->src1_imgx;
	rop.src1.frame_pix_sy	= g2d_p->src1_imgy;
	rop.src1.src_off_sx 	= g2d_p->src1_crop_offx + dest_addr_comp;
	rop.src1.src_off_sy 	= g2d_p->src1_crop_offy;  
	rop.src1.img_pix_sx 	= g2d_p->src1_crop_imgx;
	rop.src1.img_pix_sy 	= g2d_p->src1_crop_imgy;	
	rop.src1.win_off_sx 	= 0;
	rop.src1.win_off_sy 	= 0;  
	rop.src1.op_mode		= NOOP;
	rop.src1.src_form		= g2d_p->src1_fmt;
	rop.src1.arith_mode 	= AR_NOOP;
	rop.src1.arith_RY		= 0x0;
	rop.src1.arith_GU		= 0x0;
	rop.src1.arith_BV		= 0x0;
#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
	rop.src1.clut_en		= g2d_p->src1_clut_en;
#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */

	if(rop.src1.add0) chEn = GRP_F0F1;

	rop.src2.add0			= src_addr2;
	rop.src2.frame_pix_sx	= g2d_p->src2_imgx;
	rop.src2.frame_pix_sy	= g2d_p->src2_imgy;
	rop.src2.src_off_sx 	= g2d_p->src2_crop_offx + dest_addr_comp;
	rop.src2.src_off_sy 	= g2d_p->src2_crop_offy;  
	rop.src2.img_pix_sx 	= g2d_p->src2_crop_imgx;
	rop.src2.img_pix_sy 	= g2d_p->src2_crop_imgy;	
	rop.src2.win_off_sx 	= 0;
	rop.src2.win_off_sy 	= 0;  
	rop.src2.op_mode		= NOOP;
	rop.src2.src_form		= g2d_p->src2_fmt;
	rop.src2.arith_mode 	= AR_NOOP;
	rop.src2.arith_RY		= 0x0;
	rop.src2.arith_GU		= 0x0;
	rop.src2.arith_BV		= 0x0;
#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
	rop.src2.clut_en		= g2d_p->src2_clut_en;
#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */

	if(rop.src2.add0) chEn = GRP_F0F1F2;

	rop.dest.add0			= dest_addr;
	rop.dest.frame_pix_sx	= g2d_p->dst_imgx;
	rop.dest.frame_pix_sy	= g2d_p->dst_imgy;
	rop.dest.dest_off_sx	= g2d_p->dst_off_x + dest_addr_comp;
	rop.dest.dest_off_sy	= g2d_p->dst_off_y;  
	rop.dest.op_mode		= NOOP;
	rop.dest.dest_form		= g2d_p->tgtfm;

	rop.op_ctrl.csel1		= CHROMA_OP1_NOOP;	//operator 1 chroma-key source select 0:disable 1: op0 & S0_chroam 2:op0 & S1_chroma 3:src2 & S2_chroma 
	rop.op_ctrl.csel0		= CHROMA_OP0_NOOP;	//operator 0 chroma-key source select 0:disable 1: src0 & S0_chroam 2:src1 & S1_chroma 3:reseved

	rop.op_ctrl.op_mode0 = (g2d_p->alpha_type == G2d_ALPHA_VALUE)?GE_ROP_ALPHA_0:GE_ROP_ALPHA_1;
	rop.op_ctrl.op_mode1 = (g2d_p->alpha_type == G2d_ALPHA_VALUE)?GE_ROP_ALPHA_0:GE_ROP_ALPHA_1;		
	
	rop.op_pat_0.op_alpha = g2d_p->alpha_value;
	rop.op_pat_1.op_alpha = g2d_p->alpha_value;

	if(chEn == GRP_F0){
		rop.op_ctrl.op_mode0 = GE_ROP_SRC_COPY;
		rop.op_ctrl.op_mode1 = GE_ROP_SRC_COPY;
	}
	else if(chEn == GRP_F0F1){
		/* In case of 2Ch mix(=GRP_F0F1), op1 must be set as GE_ROP_SRC_COPY */
		rop.op_ctrl.op_mode1 = GE_ROP_SRC_COPY;
	}

	GRP_DBG("op_mode0:%d, op_mode1:%d\n", rop.op_ctrl.op_mode0, rop.op_ctrl.op_mode1);
	GRP_DBG("en_channel:%d\n",chEn);
	
	spin_lock_irq(&(g2d_struct.g2d_spin_lock));
	gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);
	
	gre2d_ImgROP(rop, chEn);
	
	result = gre2d_interrupt_ctrl(0,(G2D_INT_TYPE)0,0,0);

	spin_unlock_irq(&(g2d_struct.g2d_spin_lock));

	tccxxx_grp_response_check(g2d_p->responsetype);
	
	return result;
}


#define G2D_MAX_SIZE	4080
extern void Gre2d_SetBCh_address(G2D_CHANNEL ch, unsigned int add0, unsigned int add1, unsigned int add2);
int grp_alphablending_value_set(G2D_ARITH_OP_TYPE *g2d_p)
{

	unsigned int i = 0, loop_cnt = 0, remainder = 0;

	dprintk("%s!!!\n", __func__);
	dprintk("SRC:[0x%x][0x%x][0x%x]- DEST:[0x%x][0x%x][0x%x]\n",
		g2d_p->src0, g2d_p->src1, g2d_p->src2, g2d_p->tgt0, g2d_p->tgt1, g2d_p->tgt2);

	dprintk("SRC:[%d][%d]- DEST:[%d][%d] OFFSET:[%d][%d]!!!\n",
		g2d_p->src_imgx, g2d_p->src_imgy, g2d_p->dst_imgx, g2d_p->dst_imgy, g2d_p->dst_off_x, g2d_p->dst_off_y);

	loop_cnt = (g2d_p->dst_imgy / G2D_MAX_SIZE);
	
	remainder = g2d_p->dst_imgy % G2D_MAX_SIZE; 
	
	dprintk("loop_cnt:%d  remainder:%d!!!\n", loop_cnt, remainder);

	if(g2d_p->src_imgy > G2D_MAX_SIZE)
		g2d_p->src_imgy = G2D_MAX_SIZE;
	
	for(i =0; i < loop_cnt; i++)
	{

		gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);
		if(i == 0)
		{
			gre2d_ImgArithmetic(g2d_p->src0 ,g2d_p->src1, g2d_p->src2, 
								g2d_p->srcfm, g2d_p->src_imgx, g2d_p->src_imgy,
								g2d_p->tgt0 + (G2D_MAX_SIZE * 4 * i), g2d_p->tgt1, g2d_p->tgt2, g2d_p->tgtfm,
								g2d_p->dst_imgx, g2d_p->dst_imgy, g2d_p->dst_off_x, g2d_p->dst_off_y,
								g2d_p->arith, g2d_p->R, g2d_p->G, g2d_p->B);

		}
		else
		{
			//destination address setting
			Gre2d_SetBCh_address(DEST_CH,  g2d_p->tgt0 + (G2D_MAX_SIZE* 4 * i), 0, 0);

			gre2d_waiting_result(GRP_F0);
		}

		tccxxx_grp_response_check(g2d_p->responsetype);

	}

	if(remainder)
	{
		dprintk("remainder:%d  addr:0x%x!!!\n", remainder, g2d_p->tgt0 + (G2D_MAX_SIZE * 4 * loop_cnt));

		gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);

		gre2d_ImgArithmetic(g2d_p->src0 ,g2d_p->src1, g2d_p->src2, 
							g2d_p->srcfm, g2d_p->src_imgx, g2d_p->src_imgy,
							g2d_p->tgt0 + (G2D_MAX_SIZE * 4 * loop_cnt), g2d_p->tgt1, g2d_p->tgt2, g2d_p->tgtfm,
							g2d_p->dst_imgx, g2d_p->dst_imgy, g2d_p->dst_off_x, g2d_p->dst_off_y,
							g2d_p->arith, g2d_p->R, g2d_p->G, g2d_p->B);

		tccxxx_grp_response_check(g2d_p->responsetype);
	}
	return 0;
}

int grp_arithmeic_operation_ctrl(G2D_ARITH_OP_TYPE *g2d_p)
{
	unsigned int result = 0;
	

	dprintk("%s!!!\n", __func__);
	dprintk("SRC:[0x%x][0x%x][0x%x]- DEST:[0x%x][0x%x][0x%x]\n",
		g2d_p->src0, g2d_p->src1, g2d_p->src2, g2d_p->tgt0, g2d_p->tgt1, g2d_p->tgt2);

	dprintk("SRC:[%d][%d]- DEST:[%d][%d] OFFSET:[%d][%d]!!!\n",
		g2d_p->src_imgx, g2d_p->src_imgy, g2d_p->dst_imgx, g2d_p->dst_imgy, g2d_p->dst_off_x, g2d_p->dst_off_y);

	spin_lock_irq(&(g2d_struct.g2d_spin_lock));

	gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);
	gre2d_ImgArithmetic(g2d_p->src0 ,g2d_p->src1, g2d_p->src2, 
						g2d_p->srcfm, g2d_p->src_imgx, g2d_p->src_imgy,
						g2d_p->tgt0 , g2d_p->tgt1, g2d_p->tgt2, g2d_p->tgtfm,
						g2d_p->dst_imgx, g2d_p->dst_imgy, g2d_p->dst_off_x, g2d_p->dst_off_y,
						g2d_p->arith, g2d_p->R, g2d_p->G, g2d_p->B);


	result = gre2d_interrupt_ctrl(0,(G2D_INT_TYPE)0,0,0);
	spin_unlock_irq(&(g2d_struct.g2d_spin_lock));
	
	tccxxx_grp_response_check(g2d_p->responsetype);

	return 0;
}


#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
void grp_overlaymixer_clut_ctrl(G2D_CLUT_TYPE *pClut)
{
	int i;

	if(pClut->clut_type == CLUT_DATA_ALL)
	{
		for(i=0; i<0xFF; i++)
		{
			Gre2d_Grp_Clut_Ctrl(pClut->clut.all.ch, i, pClut->clut.all.data[i]);
		}
	}
	else{
		Gre2d_Grp_Clut_Ctrl(pClut->clut.one.ch, pClut->clut.one.index, pClut->clut.one.data);
	}
}
#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */

void grp_rotate_ctrl(G2D_BITBLIT_TYPE *g2d_p)
{
	int result;
	unsigned int src_addr_comp = 0, dest_addr_comp = 0;
	unsigned int src_addr, dest_addr;

	src_addr = g2d_p->src0;
	dest_addr = g2d_p->tgt0;

	src_addr_comp = (src_addr % 8)/2;
	dest_addr_comp = (dest_addr % 8)/2;

	src_addr = (src_addr>>3)<<3;
	dest_addr = (dest_addr>>3)<<3;

	dprintk("%s  add:0x%x w:%d h:%d sx:%d sy:%d using:%d !!!\n",  __func__,src_addr, g2d_p->src_imgx, g2d_p->src_imgy, g2d_p->crop_imgx, g2d_p->crop_imgy, g2d_struct.block_waiting);
	dprintk("%s  0x%x %d %d	%d %d !! %d %d  !\n", __func__,dest_addr, g2d_p->dst_imgx, g2d_p->dst_imgy, g2d_p->crop_offx, g2d_p->crop_offy,  g2d_p->dst_off_x,  g2d_p->dst_off_y);

#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
	g2d_struct.block_operating++;
	if(g2d_struct.block_operating > 1) {
		wait_event_interruptible_timeout(g2d_struct.cmd_wq, g2d_struct.block_operating == 0, msecs_to_jiffies(500));
	}

	gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);	
	gre2d_ImgRotate_Ex(src_addr, 0, 0, 
					g2d_p->srcfm ,
					g2d_p->src_imgx ,g2d_p->src_imgy,
					g2d_p->crop_offx + src_addr_comp, g2d_p->crop_offy, 
					g2d_p->crop_imgx, g2d_p->crop_imgy, 
					dest_addr,0, 0,
					g2d_p->tgtfm,
					g2d_p->dst_imgx, g2d_p->dst_imgy, 
					g2d_p->dst_off_x + dest_addr_comp, g2d_p->dst_off_y, 
					g2d_p->ch_mode, 0);

	if((g2d_p->responsetype == G2D_INTERRUPT) || (g2d_p->responsetype == G2D_POLLING))
	{
		wait_event_interruptible_timeout(g2d_struct.poll_wq,  g2d_struct.block_operating == 0, msecs_to_jiffies(500));
	}
#else
	spin_lock_irq(&(g2d_struct.g2d_spin_lock));

	gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);	
	gre2d_ImgRotate_Ex(src_addr, 0, 0, 
					g2d_p->srcfm ,
					g2d_p->src_imgx ,g2d_p->src_imgy,
					g2d_p->crop_offx + src_addr_comp, g2d_p->crop_offy, 
					g2d_p->crop_imgx, g2d_p->crop_imgy, 
					dest_addr,0, 0,
					g2d_p->tgtfm,
					g2d_p->dst_imgx, g2d_p->dst_imgy, 
					g2d_p->dst_off_x + dest_addr_comp, g2d_p->dst_off_y, 
					g2d_p->ch_mode, 0);
	
	result = gre2d_interrupt_ctrl(0,(G2D_INT_TYPE)0,0,0);
	spin_unlock_irq(&(g2d_struct.g2d_spin_lock));
	
	tccxxx_grp_response_check(g2d_p->responsetype);
#endif
}

int grp_overlay_ctrl(G2D_OVERY_FUNC *overlay)
{
	int ret;
	
#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
	#if 1
	if(g2d_struct.block_operating)
	{
		dprintk("%s G2D Err:%d\n", __func__, g2d_struct.block_operating);
		return -1;
	}
	#endif

	g2d_struct.block_operating++;
	if(g2d_struct.block_operating > 1) {
		wait_event_interruptible_timeout(g2d_struct.cmd_wq, g2d_struct.block_operating == 0, msecs_to_jiffies(500));
	}
#else
	mutex_lock(&g2d_struct.io_mutex);

	#if 1
	if(g2d_struct.block_operating)
	{
		dprintk("%s G2D Err:%d\n", __func__, g2d_struct.block_operating);
		mutex_unlock(&g2d_struct.io_mutex);
		return -1;
	}
	#endif

	if(g2d_struct.block_operating >= 1)			{
		g2d_struct.block_waiting = 1;
		ret = wait_event_interruptible_timeout(g2d_struct.cmd_wq, g2d_struct.block_operating == 0, msecs_to_jiffies(500));
		if(ret <= 0){
			printk("G2D time out :%d Line :%d \n", __LINE__, ret);
			g2d_struct.block_operating = 0;
	
		}
	}

	mutex_unlock(&g2d_struct.io_mutex);
#endif

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(TCC_OUTPUT_EXCLUSIVE_UI_GetEnable())
		{
			if(TCC_OUTPUT_EXCLUSIVE_UI_GetInterlace())
				TCC_OUTPUT_EXCLUSIVE_UI_SetUpdate(1);
		}
	#endif
			
	gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);	
	gre2d_ImgOverlay(overlay);

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(TCC_OUTPUT_EXCLUSIVE_UI_GetEnable())
		{
			if(!TCC_OUTPUT_EXCLUSIVE_UI_GetInterlace())
			{
				if((overlay->responsetype == G2D_INTERRUPT) || (overlay->responsetype == G2D_POLLING))	{
					ret = wait_event_interruptible_timeout(g2d_struct.poll_wq,  g2d_struct.block_operating == 0, msecs_to_jiffies(500));
					if(ret <= 0){
						printk("G2D time out :%d Line :%d \n", __LINE__, ret);
					}
					
				}
			}
		}
		else
		{
			if((overlay->responsetype == G2D_INTERRUPT) || (overlay->responsetype == G2D_POLLING))	{
				ret = wait_event_interruptible_timeout(g2d_struct.poll_wq,  g2d_struct.block_operating == 0, msecs_to_jiffies(500));
				if(ret <= 0){
					printk("G2D time out :%d Line :%d \n", __LINE__, ret);
				}
			}
		}
	#else
		if((overlay->responsetype == G2D_INTERRUPT) || (overlay->responsetype == G2D_POLLING))	{
			ret = wait_event_interruptible_timeout(g2d_struct.poll_wq,  g2d_struct.block_operating == 0, msecs_to_jiffies(500));
			if(ret <= 0){
				printk("G2D time out :%d Line :%d \n", __LINE__, ret);
			}
			
		}
	#endif

	return 0;
}

static ssize_t 
tccxxx_grp_write(struct file *filp, const char __user *buffer, size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t
tccxxx_grp_read(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	return 0;  
}

extern int range_is_allowed(unsigned long pfn, unsigned long size);
static int tccxxx_grp_mmap(struct file *file, struct vm_area_struct *vma)
{
	if(range_is_allowed(vma->vm_pgoff, vma->vm_end - vma->vm_start) < 0){
		printk(KERN_ERR  "grp: this address is not allowed \n");
		return -EAGAIN;
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if(remap_pfn_range(vma,vma->vm_start, vma->vm_pgoff , vma->vm_end - vma->vm_start, vma->vm_page_prot))
	{
		return -EAGAIN;
	}

	vma->vm_ops		= NULL;
	vma->vm_flags 	|= VM_IO;
	vma->vm_flags 	|= VM_RESERVED;
	
	return 0;
}


static unsigned int tccxxx_grp_poll(struct file *file, struct poll_table_struct *wait)
{
	unsigned int ret = 0;
	intr_data_t *g2d_file = (intr_data_t *)file->private_data;
	
	dprintk(" tccxxx_grp_poll !!!\n");

	poll_wait(file, &(g2d_file->poll_wq), wait);
	
	spin_lock_irq(&(g2d_file->g2d_spin_lock));

	if (g2d_file->block_operating == 0) 	{
		ret =  (POLLIN | POLLRDNORM);
	}
	spin_unlock_irq(&(g2d_file->g2d_spin_lock));

	return ret;
}

// interrupt handler
static irqreturn_t tccxxx_grp_handler(int irq, void *dev)
{  	

	G2D_INT_TYPE IFlag;

	IFlag = gre2d_interrupt_ctrl(0,(G2D_INT_TYPE)0,0,0);	//GE_IREQ

	dprintk(" tccxxx_grp_handler irq[%d] %d block[%d]!!!\n", IFlag, g2d_struct.block_waiting, g2d_struct.block_operating);
	
	if(IFlag & G2D_INT_R_FLG)
	{
		IFlag = gre2d_interrupt_ctrl(1,(G2D_INT_TYPE)(G2D_INT_R_IRQ|G2D_INT_R_FLG), 0, TRUE);
		G2D_DMA_SET_INTERRUPT(SET_G2D_DMA_INT_DISABLE);
		gre2d_disable();

	#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(g2d_struct.block_operating >= 1)
		{
			g2d_struct.block_operating--;
			wake_up_interruptible(&g2d_struct.cmd_wq);
		}

		wake_up_interruptible(&g2d_struct.poll_wq);
	#else
		wake_up_interruptible(&g2d_struct.poll_wq);

		if(g2d_struct.block_operating >= 1) {
			g2d_struct.block_operating = 0;
		}

		if(g2d_struct.block_waiting)
			wake_up_interruptible(&g2d_struct.cmd_wq);
	#endif

		#if defined(CONFIG_TCC_EXCLUSIVE_UI_LAYER)
		if(TCC_OUTPUT_EXCLUSIVE_UI_GetUpdate())
		{
			TCC_OUTPUT_EXCLUSIVE_UI_Update_Mixer();
			TCC_OUTPUT_EXCLUSIVE_UI_SetUpdate(0);
		}
		#endif
	}	

	return IRQ_HANDLED;
}


int tccxxx_grp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int iRet = 0, ret = 0;
	intr_data_t *g2d_data = (intr_data_t *)file->private_data;

	dprintk(" tccxxx_grp_ioctl IOCTL [%d] [%d]!!!\n", cmd, g2d_struct.block_operating);

	switch(cmd)
	{

		case TCC_GRP_COMMON_IOCTRL:
		case TCC_GRP_COMMON_IOCTRL_KERNEL:
			{
				G2D_COMMON_TYPE 	g2d_p;

				if(cmd == TCC_GRP_COMMON_IOCTRL) {
					if(copy_from_user((void *)(&g2d_p), (void __user *)arg, sizeof(g2d_p)))
						return -EFAULT;
				}
				else {
					memcpy(&g2d_p, (G2D_COMMON_TYPE*)arg ,sizeof(g2d_p));
				}

				mutex_lock(&g2d_data->io_mutex);

				if(g2d_data->block_operating >= 1)	 {
					g2d_data->block_waiting = 1;
					ret = wait_event_interruptible_timeout(g2d_data->cmd_wq, g2d_data->block_operating == 0, msecs_to_jiffies(200));
					if(ret <= 0){
						printk("G2D time out :%d Line ret:%d block:%d \n", __LINE__, ret, g2d_data->block_operating);
						g2d_data->block_operating = 0;
					}
				}

				spin_lock_irq(&(g2d_data->g2d_spin_lock));
				g2d_data->block_waiting = 0;
				g2d_data->block_operating++;
				spin_unlock_irq(&(g2d_data->g2d_spin_lock));
				
				grp_common_ctrl((G2D_COMMON_TYPE *)&g2d_p);
				
				mutex_unlock(&g2d_data->io_mutex);
			}
			break;

		case TCC_GRP_ARITH_IOCTRL:
		case TCC_GRP_ARITH_IOCTRL_KERNEL:
			
			{
				G2D_ARITH_OP_TYPE g2d_arith;

				if(cmd == TCC_GRP_ARITH_IOCTRL) {
					if(copy_from_user((void *)(&g2d_arith), (void __user *)arg, sizeof(g2d_arith)))
						return -EFAULT;
				}
				else {
					memcpy(&g2d_arith, (G2D_ARITH_OP_TYPE*)arg ,sizeof(g2d_arith));
				}

				mutex_lock(&g2d_data->io_mutex);

				if(g2d_data->block_operating >= 1)			{
					g2d_data->block_waiting = 1;					
					ret = wait_event_interruptible_timeout(g2d_data->cmd_wq, g2d_struct.block_operating == 0, msecs_to_jiffies(200));
					if(ret <= 0){
						printk("G2D time out :%d Line :%d block:%d \n", __LINE__, ret, g2d_data->block_operating);
						g2d_data->block_operating = 0;
					}
				}

				spin_lock_irq(&(g2d_data->g2d_spin_lock));
				g2d_data->block_waiting = 0;				
				g2d_data->block_operating++;
				spin_unlock_irq(&(g2d_data->g2d_spin_lock));
				
				grp_arithmeic_operation_ctrl((G2D_ARITH_OP_TYPE *)&g2d_arith);

				mutex_unlock(&g2d_data->io_mutex);
			}
			break;

		case TCC_GRP_ALPHA_VALUE_SET_IOCTRL:
		case TCC_GRP_ALPHA_VALUE_SET_IOCTRL_KERNEL:
			
			{
				G2D_ARITH_OP_TYPE g2d_arith;


				if(cmd == TCC_GRP_ALPHA_VALUE_SET_IOCTRL) {
					if(copy_from_user((void *)(&g2d_arith), (void __user *)arg, sizeof(g2d_arith)))
						return -EFAULT;
				}
				else {
					memcpy(&g2d_arith, (G2D_ARITH_OP_TYPE*)arg ,sizeof(g2d_arith));
				}

				mutex_lock(&g2d_data->io_mutex);

				if(g2d_data->block_operating >= 1)			{
					g2d_data->block_waiting = 1;
					ret = wait_event_interruptible_timeout(g2d_data->cmd_wq, g2d_data->block_operating == 0, msecs_to_jiffies(200));
					if(ret <= 0){
						printk("G2D time out :%d Line :%d block:%d \n", __LINE__, ret, g2d_data->block_operating);
						g2d_data->block_operating = 0;
					}
				}

				spin_lock_irq(&(g2d_data->g2d_spin_lock));
				g2d_data->block_waiting = 0;
				g2d_data->block_operating++;
				spin_unlock_irq(&(g2d_data->g2d_spin_lock));

				grp_alphablending_value_set((G2D_ARITH_OP_TYPE *)&g2d_arith);

				mutex_unlock(&g2d_data->io_mutex);
				
			}
			break;

		case TCC_OVERLAY_MIXER_IOCTRL:
		case TCC_OVERLAY_MIXER_IOCTRL_KERNEL:

			{
				G2D_BITBLIT_TYPE_1	g2d_p1;


				if(cmd == TCC_OVERLAY_MIXER_IOCTRL) {
					if(copy_from_user((void *)(&g2d_p1), (void __user *)arg, sizeof(g2d_p1)))
						return -EFAULT;
				}
				else {
					memcpy(&g2d_p1, (G2D_BITBLIT_TYPE_1*)arg ,sizeof(g2d_p1));
				}
				
				mutex_lock(&g2d_data->io_mutex);

				if(g2d_data->block_operating >= 1)	{
					g2d_data->block_waiting = 1;					
					ret = wait_event_interruptible_timeout(g2d_data->cmd_wq, g2d_data->block_operating == 0, msecs_to_jiffies(200));
					if(ret <= 0){
						printk("G2D time out :%d Line :%d block:%d \n", __LINE__, ret, g2d_data->block_operating);
						g2d_data->block_operating = 0;
					}
				}
				
				spin_lock_irq(&(g2d_data->g2d_spin_lock));
				g2d_data->block_waiting = 0;
				g2d_data->block_operating++;
				spin_unlock_irq(&(g2d_data->g2d_spin_lock));

				grp_overlaymixer_ctrl(&g2d_p1);
				
				mutex_unlock(&g2d_data->io_mutex);

			}
			break;
			
		case TCC_GRP_ROTATE_IOCTRL:
		case TCC_GRP_ROTATE_IOCTRL_KERNEL:

			{
				G2D_BITBLIT_TYPE	g2d_p1;

				if(cmd == TCC_GRP_ROTATE_IOCTRL) {
					if(copy_from_user((void *)(&g2d_p1), (void __user *)arg, sizeof(g2d_p1)))
						return -EFAULT;
				}
				else {
					memcpy(&g2d_p1, (G2D_BITBLIT_TYPE*)arg ,sizeof(g2d_p1));
				}

				 
				mutex_lock(&g2d_data->io_mutex);

				if(g2d_data->block_operating >= 1)	{
					g2d_data->block_waiting = 1;					
					ret = wait_event_interruptible_timeout(g2d_data->cmd_wq, g2d_data->block_operating == 0, msecs_to_jiffies(200));
					if(ret <= 0){
						printk("G2D time out :%d Line :%d block:%d \n", __LINE__, ret, g2d_data->block_operating);
						g2d_data->block_operating = 0;
					}
				}
				
				spin_lock_irq(&(g2d_data->g2d_spin_lock));
				g2d_data->block_waiting = 0;
				g2d_data->block_operating++;
				spin_unlock_irq(&(g2d_data->g2d_spin_lock));

				grp_rotate_ctrl(&g2d_p1);
				
				mutex_unlock(&g2d_data->io_mutex);

			}
			break;



	#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
		case TCC_OVERLAY_MIXER_CLUT_IOCTRL:
			{
				G2D_CLUT_TYPE		g2d_clut;

				if(copy_from_user((void *)(&g2d_clut), (void __user *)arg, sizeof(g2d_clut)))
					return -EFAULT;	
				
				grp_overlaymixer_clut_ctrl(&g2d_clut);

			}
			break;
	#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */

		case TCC_OVERLAY_IOCTRL:
		case TCC_OVERLAY_IOCTRL_KERNEL:
			{
				G2D_OVERY_FUNC	g2d_p1;

				if(cmd == TCC_OVERLAY_IOCTRL) {
					if(copy_from_user((void *)(&g2d_p1), (void __user *)arg, sizeof(g2d_p1)))
						return -EFAULT;
				}
				else {
					memcpy(&g2d_p1, (G2D_OVERY_FUNC*)arg ,sizeof(g2d_p1));
				}

				mutex_lock(&g2d_data->io_mutex);

				if(g2d_data->block_operating >= 1)	{
					g2d_data->block_waiting = 1;
					ret = wait_event_interruptible_timeout(g2d_data->cmd_wq, g2d_data->block_operating == 0, msecs_to_jiffies(200));
					if(ret <= 0){
						printk("G2D time out :%d Line :%d block:%d \n", __LINE__, ret, g2d_data->block_operating);
						g2d_data->block_operating = 0;
					}
				}

				spin_lock_irq(&(g2d_data->g2d_spin_lock));
				g2d_data->block_waiting = 0;
				g2d_data->block_operating++;
				spin_unlock_irq(&(g2d_data->g2d_spin_lock));

				spin_lock_irq(&(g2d_struct.g2d_spin_lock));
				gre2d_rsp_interrupt(G2D_INTERRUPT_TYPE);
				gre2d_ImgOverlay(&g2d_p1);
				spin_unlock_irq(&(g2d_struct.g2d_spin_lock));

				if((g2d_p1.responsetype == G2D_INTERRUPT) || (g2d_p1.responsetype == G2D_POLLING))	{
					ret = wait_event_interruptible_timeout(g2d_struct.poll_wq,  g2d_struct.block_operating == 0, msecs_to_jiffies(500));
					if(ret <= 0){
						printk("G2D time out :%d Line :%d \n", __LINE__, ret);
					}
				}

				mutex_unlock(&g2d_data->io_mutex);
			}
			break;

		default:
			dprintk(" Unsupported IOCTL!!!\n");      
			return -EFAULT;

	}

	return iRet;
}

EXPORT_SYMBOL(tccxxx_grp_ioctl);


#if defined(CONFIG_CPU_FREQ)
extern struct tcc_freq_table_t gtOverlayClockLimitTable;
#endif//

int tccxxx_grp_release(struct inode *inode, struct file *file)
{

	mutex_lock(&g2d_struct.io_mutex);
	g2d_struct.dev_opened--;



	if(g2d_struct.dev_opened == 0)
	{
		#if defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtOverlayClockLimitTable, TCC_FREQ_LIMIT_G2D, 0);
#endif//

		#if defined(CONFIG_ARCH_TCC892X)
		disable_irq(INT_G2D);
		#endif//
	}
	clk_disable(overlay_clk);
	mutex_unlock(&g2d_struct.io_mutex);

	
	dprintk("%s open_cnt :%d \n",__func__, g2d_struct.dev_opened);

	return 0;
}
EXPORT_SYMBOL(tccxxx_grp_release);

int tccxxx_grp_open(struct inode *inode, struct file *file)
{
	mutex_lock(&g2d_struct.io_mutex);
	
	g2d_struct.dev_opened++;
	file->private_data = (void *)&g2d_struct;


	if(g2d_struct.dev_opened == 1)
	{
		#if defined(CONFIG_CPU_FREQ)
		tcc_cpufreq_set_limit_table(&gtOverlayClockLimitTable, TCC_FREQ_LIMIT_G2D, 1);
		#endif//

		#if defined(CONFIG_ARCH_TCC892X)
		enable_irq(INT_G2D);
		#endif//
	}
	clk_enable(overlay_clk);

	mutex_unlock(&g2d_struct.io_mutex);
	
	dprintk("%s open_cnt :%d \n",__func__, g2d_struct.dev_opened);
	
	return 0;
}
EXPORT_SYMBOL(tccxxx_grp_open);

static struct file_operations tcc_grp_fops = 
{
	.owner			= THIS_MODULE,
	.write			= tccxxx_grp_write,
	.read			= tccxxx_grp_read,
	.poll 			= tccxxx_grp_poll,
	.unlocked_ioctl	= tccxxx_grp_ioctl,
	.mmap			= tccxxx_grp_mmap,
	.open			= tccxxx_grp_open,
	.release			= tccxxx_grp_release,
};

void __exit
tccxxx_grp_cleanup(void)
{
	free_irq(INT_G2D, &g2d_struct);	
	clk_put(overlay_clk);

	unregister_chrdev(MAJOR_ID, DEVICE_NAME);
	dprintk(" ===========> tccxxx_grp_cleanup \n");
	return;
}

static char banner[] __initdata = KERN_INFO "TCC rotation driver initializing\n";
static struct class *grp_class;

int __init 
tccxxx_grp_init(void)
{
	int ret;

	printk(banner);

	overlay_clk = clk_get(0, "overlay");
	if (overlay_clk == NULL) {
		printk(KERN_ERR "Unable to get GPU clock source\n");
		return -EFAULT;
	}

	register_chrdev(MAJOR_ID, DEVICE_NAME, &tcc_grp_fops);

	grp_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(grp_class,NULL,MKDEV(MAJOR_ID, MINOR_ID), NULL, DEVICE_NAME);
	if ((ret = request_irq(INT_G2D, tccxxx_grp_handler, IRQF_SHARED, "grp", &g2d_struct))< 0) 	{
		dprintk("FAILED to aquire irq\n");
		return -EFAULT;
	}

	#if defined(CONFIG_ARCH_TCC892X)
	disable_irq(INT_G2D);
	#endif//	

	spin_lock_init(&(g2d_struct.g2d_spin_lock));
	mutex_init(&g2d_struct.io_mutex);

	init_waitqueue_head(&g2d_struct.poll_wq);
	init_waitqueue_head(&g2d_struct.cmd_wq);

	g2d_struct.block_waiting = 0;
	g2d_struct.block_operating = 0;

	return 0;
}


MODULE_AUTHOR("Telechips.");
MODULE_DESCRIPTION("TCC Video for Linux graphic driver");
MODULE_LICENSE("GPL");


arch_initcall(tccxxx_grp_init);
module_exit(tccxxx_grp_cleanup);

