/************************************************************************
*    Telechips Graphic Engine application
*    ------------------------------------------------
*
*    FUNCTION    : Graphic Engine application
*    MODEL       :
*    CPU NAME    : TCC83XX
*
*    START DATE    : 2007. 7. 30.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3 TEAM
*                : TELECHIPS, INC.
************************************************************************/

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
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include "tcc_Gre2d_api.h"
#include "tcc_Gre2d.h"


#define TRUE 1
#define FALSE 0


#define ALPHA_RGB_DIABLE    FALSE

// Front channel YUV to RGB converter type
static G2D_Y2R_TYPE gre2d_Y2R_type = Y2R_TYP0;

// Back channel YUV to RGB converter type
static G2D_Y2R_TYPE gre2d_R2Y_type = R2Y_TYP0;

// ZF : 0:MSB Fill Mode Enable  1:Zero Fill Mode Enable  
G2D_ZF_TYPE gZF = ZF_HOB_FILL;

G2D_ASEL_TYPE gG2D_ASEL0 = ASEL_2;
G2D_ASEL_TYPE gG2D_ASEL1 = ASEL_2;

#if defined(CONFIG_ARCH_TCC892X)
// alpha-value control 1,0
G2D_OP_ACON gG2D_ACON1 = ACON_2;
G2D_OP_ACON gG2D_ACON0 = ACON_2;

// color control 1,0
G2D_OP_CCON gG2D_CCON1 = CCON_4;
G2D_OP_CCON gG2D_CCON0 = CCON_4;

static G2D_OP_ATUNE gG2D_ATUNE = Alpha_11;

#endif

// AXI Bus Master Address Boundary
G2D_MABC_TYPE gG2D_SRC0_MABC = MABC_4KBYTE;
G2D_MABC_TYPE gG2D_SRC1_MABC = MABC_4KBYTE;
G2D_MABC_TYPE gG2D_SRC2_MABC = MABC_4KBYTE;
G2D_MABC_TYPE gG2D_DEST_MABC = MABC_4KBYTE;

// Swap U for V, 0: keep on U and V, 1: Swap U for V
unsigned char gG2D_SRC0_SUV = 0;
unsigned char gG2D_SRC1_SUV = 0;
unsigned char gG2D_SRC2_SUV = 0;
unsigned char gG2D_DEST_SUV = 0;

// lookup table enalbe
unsigned char gG2D_SRC0_LUTE = 0;
unsigned char gG2D_SRC1_LUTE = 0;
unsigned char gG2D_SRC2_LUTE = 0;

G2D_DITHERING_TYPE gG2D_Dithering_type = BIT_TOGGLE_OP;
unsigned char gG2D_Dithering_en = 0;
unsigned short DitheringMatrix[4 * 4] = {0, 8, 2, 10,
						                12, 4, 14, 6,
						                3, 11, 1, 9,
						                15, 7, 13, 5};



static G2D_RSP_TYPE gre2d_rsp_type;

char g2d_working;

void gre2d_rsp_interrupt(G2D_RSP_TYPE rsp_type)
{
	gre2d_rsp_type = rsp_type;
}

G2D_INT_TYPE gre2d_Grp_int_ctrl(unsigned char wr, G2D_INT_TYPE flag, unsigned char int_irq, unsigned char int_flg)
{
	return Gre2d_Grp_int_ctrl(wr, flag, int_irq, int_flg);
}

void G2D_DMA_SET_INTERRUPT(unsigned int uiFlag)
{
	if(uiFlag & SET_G2D_DMA_INT_ENABLE)
	{
		Gre2d_Set_interrupt(1);
	}  
	
	if(uiFlag & SET_G2D_DMA_INT_DISABLE)
	{
		Gre2d_Set_interrupt(0);	
	}  
}

void gre2d_Grp_enable(G2D_EN grp_enalbe, unsigned char int_en)
{
	Gre2d_Grp_enable(grp_enalbe, int_en);
}

void gre2d_waiting_result(G2D_EN grp_enalbe)
{
	G2D_INT_TYPE grp_isr = 0;
	if(gre2d_rsp_type == G2D_INTERRUPT_TYPE)
	{
		gre2d_Grp_int_ctrl(TRUE,(G2D_INT_TYPE)(G2D_INT_R_FLG|G2D_INT_R_IRQ),FALSE, TRUE);//    regw(GE_IREQ, 0x10000)  // IREQ FLG Clear

		G2D_DMA_SET_INTERRUPT(SET_G2D_DMA_INT_ENABLE);	
		g2d_working = 1;
		/*-- interrupt type --*/	
		gre2d_Grp_enable(grp_enalbe, TRUE);	//channel enable : Front End Channel 0, 1, 2 enable
	}
	else if(gre2d_rsp_type == G2D_CHECK_TYPE)
	{
		/*-- check  type --*/
		G2D_DMA_SET_INTERRUPT(SET_G2D_DMA_INT_ENABLE);	
		gre2d_Grp_enable(grp_enalbe, 0); 	//channel enable : Front End Channel 0, 1, 2 enable	
	}
	else
	{	
		/*-- polling type --*/
		G2D_DMA_SET_INTERRUPT(SET_G2D_DMA_INT_DISABLE);	
		gre2d_Grp_enable(grp_enalbe, 0); 	//channel enable : Front End Channel 0, 1, 2 enable

		// waiting for transfer
		while (!(grp_isr & G2D_INT_R_FLG)) {
		    grp_isr = gre2d_Grp_int_ctrl(0,(G2D_INT_TYPE)0,0,0);	//GE_IREQ
		}

		gre2d_Grp_int_ctrl(TRUE,(G2D_INT_TYPE)(G2D_INT_R_FLG|G2D_INT_R_IRQ),FALSE, TRUE);//    regw(GE_IREQ, 0x10000)  // IREQ FLG Clear

		Gre2d_disable();		
	}
	gre2d_rsp_type = G2D_POLLING_TYPE;
}

/*-----------------------------------------------------------------------------
gre2d_3ch_dma_main_func
 graphic engine main function
 
Graphic engine 관련 모든 기능을 사용할수 있음
-----------------------------------------------------------------------------*/
unsigned char gre2d_3ch_dma_main_func(G2D_FUNC_TYPE gre2d_value, G2D_EN grp_enalbe)
{
    unsigned char ret = TRUE;
	G2D_BCH_CTRL_TYPE dest_ctrl;

    G2D_SRC_CTRL src_ctrl;
    
    int Window_size_x = 0, Window_size_y = 0; // 내부에서 사용되는 window size

    if((gre2d_value.src0.op_mode == ROTATE_90) || (gre2d_value.src0.op_mode == ROTATE_270))
    {
        Window_size_x = gre2d_value.src0.img_pix_sy;
        Window_size_y = gre2d_value.src0.img_pix_sx;
    }
    else
    {
        Window_size_x = gre2d_value.src0.img_pix_sx;
        Window_size_y = gre2d_value.src0.img_pix_sy;
    }

    if((Window_size_y < gre2d_value.src1.win_off_sy) || (Window_size_x < gre2d_value.src1.win_off_sx)) 
            return FALSE;

    if((Window_size_y < gre2d_value.src2.win_off_sy) || (Window_size_x < gre2d_value.src2.win_off_sx)) 
            return FALSE;

    if((gre2d_value.src1.op_mode == ROTATE_90) |(gre2d_value.src1.op_mode == ROTATE_270))
    {
        if((Window_size_y < gre2d_value.src1.win_off_sy) || (Window_size_x < gre2d_value.src1.win_off_sx)) 
                return FALSE;
        
        if((gre2d_value.src1.img_pix_sx + gre2d_value.src1.win_off_sy) > Window_size_y)
                gre2d_value.src1.img_pix_sx = Window_size_y - gre2d_value.src1.win_off_sy;

        if((gre2d_value.src1.img_pix_sy + gre2d_value.src1.win_off_sx) > Window_size_x)
                gre2d_value.src1.img_pix_sy = Window_size_x - gre2d_value.src1.win_off_sx;
    }
    else
    {
        if((gre2d_value.src1.img_pix_sx + gre2d_value.src1.win_off_sx) > Window_size_x)
                gre2d_value.src1.img_pix_sx = Window_size_x - gre2d_value.src1.win_off_sx;

        if((gre2d_value.src1.img_pix_sy + gre2d_value.src1.win_off_sy) > Window_size_y)
                gre2d_value.src1.img_pix_sy = Window_size_y - gre2d_value.src1.win_off_sy;
    }

    if((gre2d_value.src2.op_mode == ROTATE_90) ||(gre2d_value.src2.op_mode == ROTATE_270))
    {
        if((gre2d_value.src2.img_pix_sx + gre2d_value.src2.win_off_sy) > Window_size_y)
                gre2d_value.src2.img_pix_sx = Window_size_y - gre2d_value.src2.win_off_sy;

        if((gre2d_value.src2.img_pix_sy + gre2d_value.src2.win_off_sx) > Window_size_x)
                gre2d_value.src2.img_pix_sy = Window_size_x - gre2d_value.src2.win_off_sx;
    }
    else
    {
        if((gre2d_value.src2.img_pix_sx + gre2d_value.src2.win_off_sx) > Window_size_x)
                gre2d_value.src2.img_pix_sx = Window_size_x - gre2d_value.src2.win_off_sx;

        if((gre2d_value.src2.img_pix_sy + gre2d_value.src2.win_off_sy) > Window_size_y)
                gre2d_value.src2.img_pix_sy = Window_size_y - gre2d_value.src2.win_off_sy;
    }    

	Gre2d_enable();

// front end channel address setting.

//////////////// 
// channel 0
//////////////// 

	Gre2d_SetFCh_address(FCH0_CH, 
				gre2d_value.src0.add0, 
				gre2d_value.src0.add1, 
				gre2d_value.src0.add2);

	Gre2d_SetFCh_position(FCH0_CH, 
				gre2d_value.src0.frame_pix_sx, 
				gre2d_value.src0.frame_pix_sy, 
				gre2d_value.src0.src_off_sx, 
				gre2d_value.src0.src_off_sy,
	                        gre2d_value.src0.img_pix_sx, 
				gre2d_value.src0.img_pix_sy, 
				gre2d_value.src0.win_off_sx, 
				gre2d_value.src0.win_off_sy);

	Gre2d_SetFCh_control(FCH0_CH, 
				gG2D_SRC0_MABC, 
			#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
				(gre2d_value.src0.clut_en)?1:0,
			#else
				gG2D_SRC0_LUTE, 				
			#endif /* CLUE_ENABLE */
				gre2d_value.src0.src_form.uv_order, 
				gre2d_value.src0.op_mode,  
				gZF, 
				gre2d_value.src0.src_form);

	Gre2d_SetFCh_Chroma_key(FCH0_CH, gre2d_value.src0.chroma_RY, gre2d_value.src0.chroma_GU, gre2d_value.src0.chroma_BV);

	Gre2d_SetFCh_Arithmetic_par(FCH0_CH, gre2d_value.src0.arith_RY, gre2d_value.src0.arith_GU, gre2d_value.src0.arith_BV);

//////////////// 
// channel 1
//////////////// 

	Gre2d_SetFCh_address(FCH1_CH, 
				gre2d_value.src1.add0, 
				gre2d_value.src1.add1, 
				gre2d_value.src1.add2);

	Gre2d_SetFCh_position(FCH1_CH, 
				gre2d_value.src1.frame_pix_sx, 
				gre2d_value.src1.frame_pix_sy, 
				gre2d_value.src1.src_off_sx, 
				gre2d_value.src1.src_off_sy,
	                        gre2d_value.src1.img_pix_sx, 
				gre2d_value.src1.img_pix_sy, 
				gre2d_value.src1.win_off_sx, 
				gre2d_value.src1.win_off_sy);
	Gre2d_SetFCh_control(FCH1_CH, 
				gG2D_SRC1_MABC, 
			#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
				(gre2d_value.src1.clut_en)?1:0,
			#else
				gG2D_SRC1_LUTE, 				
			#endif /* CLUE_ENABLE */
				gre2d_value.src1.src_form.uv_order, 
				gre2d_value.src1.op_mode, gZF, gre2d_value.src1.src_form);


	Gre2d_SetFCh_Chroma_key(FCH1_CH, 
				gre2d_value.src1.chroma_RY, 
				gre2d_value.src1.chroma_GU, 
				gre2d_value.src1.chroma_BV);

	Gre2d_SetFCh_Arithmetic_par(FCH1_CH, 
				gre2d_value.src1.arith_RY, 
				gre2d_value.src1.arith_GU, 
				gre2d_value.src1.arith_BV);

//////////////// 
// channel 2
//////////////// 

	Gre2d_SetFCh_address(FCH2_CH, 
				gre2d_value.src2.add0, 
				gre2d_value.src2.add1, 
				gre2d_value.src2.add2);

	Gre2d_SetFCh_position(FCH2_CH, 
				gre2d_value.src2.frame_pix_sx, 
				gre2d_value.src2.frame_pix_sy, 
				gre2d_value.src2.src_off_sx, 
				gre2d_value.src2.src_off_sy,
	                        gre2d_value.src2.img_pix_sx, 
				gre2d_value.src2.img_pix_sy, 
				gre2d_value.src2.win_off_sx, 
				gre2d_value.src2.win_off_sy);

	Gre2d_SetFCh_control(FCH2_CH, 
				gG2D_SRC2_MABC, 
			#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
				(gre2d_value.src2.clut_en)?1:0,
			#else
				gG2D_SRC2_LUTE, 				
			#endif /* CLUE_ENABLE */
				gre2d_value.src2.src_form.uv_order, 
				gre2d_value.src2.op_mode, gZF, gre2d_value.src2.src_form);


	Gre2d_SetFCh_Chroma_key(FCH2_CH, 
				gre2d_value.src2.chroma_RY, 
				gre2d_value.src2.chroma_GU, 
				gre2d_value.src2.chroma_BV);

	Gre2d_SetFCh_Arithmetic_par(FCH2_CH, 
				gre2d_value.src1.arith_RY, 
				gre2d_value.src2.arith_GU, 
				gre2d_value.src2.arith_BV);

	//channel control 
	src_ctrl.src0_arith = gre2d_value.src0.arith_mode;
	src_ctrl.src0_chroma_en = gre2d_value.src0.src_chroma_en;
	src_ctrl.src_sel_0 = FCH0_CH;
	src_ctrl.src0_y2r.src_y2r = gre2d_value.src0.src_y2r;
	src_ctrl.src0_y2r.src_y2r_type = gre2d_value.src0.src_y2r_type;

	src_ctrl.src1_arith = gre2d_value.src1.arith_mode;
	src_ctrl.src1_chroma_en = gre2d_value.src1.src_chroma_en;
	src_ctrl.src_sel_1 = FCH1_CH;
	src_ctrl.src1_y2r.src_y2r = gre2d_value.src1.src_y2r;
	src_ctrl.src1_y2r.src_y2r_type = gre2d_value.src1.src_y2r_type;

	src_ctrl.src2_arith = gre2d_value.src2.arith_mode;
	src_ctrl.src2_chroma_en = gre2d_value.src2.src_chroma_en;
	src_ctrl.src_sel_2 = FCH2_CH;
	src_ctrl.src2_y2r.src_y2r = gre2d_value.src2.src_y2r;
	src_ctrl.src2_y2r.src_y2r_type = gre2d_value.src2.src_y2r_type;

	Gre2d_src_ctrl(src_ctrl);


	// operator 0 pattern setting
	Gre2d_operator_set(OP_0, 
				gre2d_value.op_pat_0.op_alpha, 
				gre2d_value.op_pat_0.op_pat_RY, 
				gre2d_value.op_pat_0.op_pat_GU, 
				gre2d_value.op_pat_0.op_pat_BV);
	// operator 1 pattern setting
	Gre2d_operator_set(OP_1, 
				gre2d_value.op_pat_1.op_alpha, 
				gre2d_value.op_pat_1.op_pat_RY, 
				gre2d_value.op_pat_1.op_pat_GU, 
				gre2d_value.op_pat_1.op_pat_BV);


	

	#if defined(CONFIG_ARCH_TCC892X)
	Gre2d_operator_ctrl(OP_0, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1,gG2D_CCON0,gG2D_ATUNE, 
							gre2d_value.op_ctrl.csel0, gre2d_value.op_ctrl.op_mode0);

	Gre2d_operator_ctrl(OP_1, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1,gG2D_CCON0,gG2D_ATUNE, 
							gre2d_value.op_ctrl.csel1, gre2d_value.op_ctrl.op_mode1);

	#else

	Gre2d_operator_ctrl(gG2D_ASEL1, 
				gre2d_value.op_ctrl.csel1, 
				gre2d_value.op_ctrl.op_mode1, 
				gG2D_ASEL0, 
				gre2d_value.op_ctrl.csel0, 
				gre2d_value.op_ctrl.op_mode0);
	#endif	

	//back end channel 
	Gre2d_SetBCh_address(DEST_CH,  gre2d_value.dest.add0, gre2d_value.dest.add1, gre2d_value.dest.add2);

	Gre2d_SetBCh_position(DEST_CH, 
				gre2d_value.dest.frame_pix_sx, 
				gre2d_value.dest.frame_pix_sy, 
				gre2d_value.dest.dest_off_sx, 
				gre2d_value.dest.dest_off_sy);

	dest_ctrl.MABC = gG2D_DEST_MABC;
	dest_ctrl.ysel = gre2d_value.dest.ysel;
	dest_ctrl.xsel = gre2d_value.dest.xsel;
	dest_ctrl.converter_en = gre2d_value.dest.converter_en;
	dest_ctrl.converter_mode = gre2d_value.dest.converter_mode;
	dest_ctrl.DSUV = gre2d_value.dest.dest_form.uv_order;
	dest_ctrl.opmode =  gre2d_value.dest.op_mode;
	dest_ctrl.dithering_type = gG2D_Dithering_type;
	dest_ctrl.dithering_en = gG2D_Dithering_en;
	dest_ctrl.data_form = gre2d_value.dest.dest_form;
	Gre2d_SetBCh_control(&dest_ctrl);

	if(gG2D_Dithering_en)
		Gre2d_SetDitheringMatrix(DitheringMatrix);

	gre2d_waiting_result(grp_enalbe);


    return ret;
}

/*-----------------------------------------------------------------------------
gre2d_2ch_dma_main_func
 graphic engine 2 channel main function
 
 ch_0, ch_1, dest, operator 0 사용 시 Graphic engine 관련 기능을 사용할수 있음

------------------------------------------------------------------------------*/
unsigned char gre2d_2ch_dma_main_func(G2d_2CH_FUNC gre2d_value)
{
    unsigned char ret = TRUE;
    G2D_SRC_CTRL src_ctrl;
	G2D_BCH_CTRL_TYPE dest_ctrl;

    int Window_size_x = 0, Window_size_y = 0; // 내부에서 사용되는 window size

    if((gre2d_value.src0.op_mode == ROTATE_90) ||(gre2d_value.src0.op_mode == ROTATE_270))
    {
        Window_size_x = gre2d_value.src0.img_pix_sy;
        Window_size_y = gre2d_value.src0.img_pix_sx;
    }
    else
    {
        Window_size_x = gre2d_value.src0.img_pix_sx;
        Window_size_y = gre2d_value.src0.img_pix_sy;
    }
    
    if((Window_size_y < gre2d_value.src1.win_off_sy) || (Window_size_x < gre2d_value.src1.win_off_sx)) 
        return FALSE;


   if((gre2d_value.src1.op_mode == ROTATE_90) ||(gre2d_value.src1.op_mode == ROTATE_270))
    {
        if((gre2d_value.src1.img_pix_sx + gre2d_value.src1.win_off_sy) > Window_size_y)
                gre2d_value.src1.img_pix_sx = Window_size_y - gre2d_value.src1.win_off_sy;

        if((gre2d_value.src1.img_pix_sy + gre2d_value.src1.win_off_sx) > Window_size_x)
                gre2d_value.src1.img_pix_sy = Window_size_x - gre2d_value.src1.win_off_sx;
    }
    else
    {
        if((gre2d_value.src1.img_pix_sx + gre2d_value.src1.win_off_sx) > Window_size_x)
                gre2d_value.src1.img_pix_sx = Window_size_x - gre2d_value.src1.win_off_sx;

        if((gre2d_value.src1.img_pix_sy + gre2d_value.src1.win_off_sy) > Window_size_y)
                gre2d_value.src1.img_pix_sy = Window_size_y - gre2d_value.src1.win_off_sy;
    }

	Gre2d_enable();

// front end channel address setting.

// channel 0
    Gre2d_SetFCh_address(FCH0_CH, gre2d_value.src0.add0, gre2d_value.src0.add1, gre2d_value.src0.add2);

    Gre2d_SetFCh_position(FCH0_CH, gre2d_value.src0.frame_pix_sx, gre2d_value.src0.frame_pix_sy, gre2d_value.src0.src_off_sx, gre2d_value.src0.src_off_sy,
                                    gre2d_value.src0.img_pix_sx, gre2d_value.src0.img_pix_sy, gre2d_value.src0.win_off_sx, gre2d_value.src0.win_off_sy);

	Gre2d_SetFCh_control(FCH0_CH, gG2D_SRC0_MABC, gG2D_SRC0_LUTE, gre2d_value.src0.src_form.uv_order, gre2d_value.src0.op_mode,	gZF, gre2d_value.src0.src_form);

    Gre2d_SetFCh_Chroma_key(FCH0_CH, gre2d_value.src0.chroma_RY, gre2d_value.src0.chroma_GU, gre2d_value.src0.chroma_BV);

    Gre2d_SetFCh_Arithmetic_par(FCH0_CH, gre2d_value.src0.arith_RY, gre2d_value.src0.arith_GU, gre2d_value.src0.arith_BV);

//channel 1
    Gre2d_SetFCh_address(FCH1_CH, gre2d_value.src1.add0, gre2d_value.src1.add1, gre2d_value.src1.add2);

    Gre2d_SetFCh_position(FCH1_CH, gre2d_value.src1.frame_pix_sx, gre2d_value.src1.frame_pix_sy, gre2d_value.src1.src_off_sx, gre2d_value.src1.src_off_sy,
                                    gre2d_value.src1.img_pix_sx, gre2d_value.src1.img_pix_sy, gre2d_value.src1.win_off_sx, gre2d_value.src1.win_off_sy);
	Gre2d_SetFCh_control(FCH1_CH, gG2D_SRC1_MABC, gG2D_SRC1_LUTE, gre2d_value.src1.src_form.uv_order, gre2d_value.src1.op_mode, gZF, gre2d_value.src1.src_form);

    Gre2d_SetFCh_Chroma_key(FCH1_CH, gre2d_value.src1.chroma_RY, gre2d_value.src1.chroma_GU, gre2d_value.src1.chroma_BV);

    Gre2d_SetFCh_Arithmetic_par(FCH1_CH, gre2d_value.src1.arith_RY, gre2d_value.src1.arith_GU, gre2d_value.src1.arith_BV);

#if 0
//channel 2
    Gre2d_SetFCh_address(FCH2_CH, gre2d_value.src2.add0, gre2d_value.src2.add1, gre2d_value.src2.add2);

    Gre2d_SetFCh_position(FCH2_CH, gre2d_value.src2.frame_pix_sx, gre2d_value.src2.frame_pix_sy, gre2d_value.src2.src_off_sx, gre2d_value.src2.src_off_sy,
                                    gre2d_value.src2.img_pix_sx, gre2d_value.src2.img_pix_sy, gre2d_value.src2.win_off_sx, gre2d_value.src2.win_off_sy);

    Gre2d_SetFCh_control(FCH2_CH, gre2d_value.src2.op_mode, ZF, gre2d_value.src2.src_form);

    Gre2d_SetFCh_Chroma_key(FCH2_CH, gre2d_value.src2.chroma_RY, gre2d_value.src2.chroma_GU, gre2d_value.src2.chroma_BV);

    Gre2d_SetFCh_Arithmetic_par(FCH2_CH, gre2d_value.src1.arith_RY, gre2d_value.src2.arith_GU, gre2d_value.src2.arith_BV);
#endif//

//channel control 
    src_ctrl.src0_arith = gre2d_value.src0.arith_mode;
    src_ctrl.src0_chroma_en = gre2d_value.src0.src_chroma_en;
    src_ctrl.src_sel_0 = FCH0_CH;
    src_ctrl.src0_y2r.src_y2r = gre2d_value.src0.src_y2r;
    src_ctrl.src0_y2r.src_y2r_type = gre2d_value.src0.src_y2r_type;
    
    src_ctrl.src1_arith = gre2d_value.src1.arith_mode;
    src_ctrl.src1_chroma_en = gre2d_value.src1.src_chroma_en;
    src_ctrl.src_sel_1 = FCH1_CH;
    src_ctrl.src1_y2r.src_y2r = gre2d_value.src1.src_y2r;
    src_ctrl.src1_y2r.src_y2r_type = gre2d_value.src1.src_y2r_type;

    src_ctrl.src2_arith = AR_NOOP;
    src_ctrl.src2_chroma_en = 0;
    src_ctrl.src_sel_2 = FCH2_CH;
    src_ctrl.src2_y2r.src_y2r = 0;
    src_ctrl.src2_y2r.src_y2r_type = 0;
#if defined(CONFIG_ARCH_TCC892X)
    src_ctrl.src_sel_3 = FCH3_CH;
#endif//

    Gre2d_src_ctrl(src_ctrl);


// operator 0 pattern setting
    Gre2d_operator_set(OP_0, gre2d_value.op_pat_0.op_alpha, gre2d_value.op_pat_0.op_pat_RY, gre2d_value.op_pat_0.op_pat_GU, gre2d_value.op_pat_0.op_pat_BV);
// operator 1 pattern setting
    Gre2d_operator_set(OP_1, 0, 0, 0, 0);
	

	#if defined(CONFIG_ARCH_TCC892X)
	Gre2d_operator_ctrl(OP_0, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1,gG2D_CCON0,gG2D_ATUNE, 
							gre2d_value.csel0, gre2d_value.op_mode0);


	Gre2d_operator_ctrl(OP_1, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1,gG2D_CCON0,gG2D_ATUNE, 
							CHROMA_OP1_NOOP,GE_ROP_SRC_COPY);
	Gre2d_operator_ctrl(OP_2, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1, gG2D_CCON0,gG2D_ATUNE, 
							CHROMA_OP1_NOOP,GE_ROP_SRC_COPY);	
	#else
	Gre2d_operator_ctrl(gG2D_ASEL1, CHROMA_OP1_NOOP, GE_ROP_SRC_COPY, gG2D_ASEL0,  gre2d_value.csel0, gre2d_value.op_mode0);
		
	#endif

//back end channel 
    Gre2d_SetBCh_address(DEST_CH,  gre2d_value.dest.add0, gre2d_value.dest.add1, gre2d_value.dest.add2);

    Gre2d_SetBCh_position(DEST_CH, gre2d_value.dest.frame_pix_sx, gre2d_value.dest.frame_pix_sy, gre2d_value.dest.dest_off_sx, gre2d_value.dest.dest_off_sy);


	dest_ctrl.MABC = gG2D_DEST_MABC;
	dest_ctrl.ysel =  gre2d_value.dest.ysel;
	dest_ctrl.xsel = gre2d_value.dest.xsel;
	dest_ctrl.converter_en = gre2d_value.dest.converter_en;
	dest_ctrl.converter_mode = gre2d_value.dest.converter_mode;
	dest_ctrl.DSUV = gre2d_value.dest.dest_form.uv_order;
	dest_ctrl.opmode = gre2d_value.dest.op_mode;
	dest_ctrl.dithering_type = gG2D_Dithering_type;
	dest_ctrl.dithering_en = gG2D_Dithering_en;
	dest_ctrl.data_form = gre2d_value.dest.dest_form;
	Gre2d_SetBCh_control(&dest_ctrl);

	if(gG2D_Dithering_en)
		Gre2d_SetDitheringMatrix(DitheringMatrix);

	//channel enable : Front End Channel 0 ,1 enable
	gre2d_waiting_result(GRP_F0F1);
 
    return ret;
}


/*-----------------------------------------------------------------------------
gre2d_1ch_dma_main_func
 graphic engine 1 channel main function
 
  ch_0,  dest 사용 시 Graphic engine 관련 기능을 사용할수 있음

-----------------------------------------------------------------------------*/
unsigned char gre2d_1ch_dma_main_func(G2d_1CH_FUNC gre2d_value)
{

    unsigned char ret = TRUE;
	G2D_SRC_CTRL src_ctrl;
	G2D_BCH_CTRL_TYPE dest_ctrl;
	Gre2d_enable();

// front end channel address setting.

// channel 0
    Gre2d_SetFCh_address(FCH0_CH, gre2d_value.src0.add0, gre2d_value.src0.add1, gre2d_value.src0.add2);

    Gre2d_SetFCh_position(FCH0_CH, gre2d_value.src0.frame_pix_sx, gre2d_value.src0.frame_pix_sy, gre2d_value.src0.src_off_sx, gre2d_value.src0.src_off_sy,
                                    gre2d_value.src0.img_pix_sx, gre2d_value.src0.img_pix_sy, gre2d_value.src0.win_off_sx, gre2d_value.src0.win_off_sy);

	Gre2d_SetFCh_control(FCH0_CH, gG2D_SRC0_MABC, gG2D_SRC0_LUTE, gre2d_value.src0.src_form.uv_order, gre2d_value.src0.op_mode,	gZF, gre2d_value.src0.src_form);

    Gre2d_SetFCh_Chroma_key(FCH0_CH, gre2d_value.src0.chroma_RY, gre2d_value.src0.chroma_GU, gre2d_value.src0.chroma_BV);

    Gre2d_SetFCh_Arithmetic_par(FCH0_CH, gre2d_value.src0.arith_RY, gre2d_value.src0.arith_GU, gre2d_value.src0.arith_BV);

#if 0
//channel 1
    Gre2d_SetFCh_address(FCH1_CH, gre2d_value.src1.add0, gre2d_value.src1.add1, gre2d_value.src1.add2);

    Gre2d_SetFCh_position(FCH1_CH, gre2d_value.src1.frame_pix_sx, gre2d_value.src1.frame_pix_sy, gre2d_value.src1.src_off_sx, gre2d_value.src1.src_off_sy,
                                    gre2d_value.src1.img_pix_sx, gre2d_value.src1.img_pix_sy, gre2d_value.src1.win_off_sx, gre2d_value.src1.win_off_sy);

    Gre2d_SetFCh_control(FCH1_CH, gre2d_value.src1.op_mode, ZF, gre2d_value.src1.src_form);

    Gre2d_SetFCh_Chroma_key(FCH1_CH, gre2d_value.src1.chroma_RY, gre2d_value.src1.chroma_GU, gre2d_value.src1.chroma_BV);

    Gre2d_SetFCh_Arithmetic_par(FCH1_CH, gre2d_value.src1.arith_RY, gre2d_value.src1.arith_GU, gre2d_value.src1.arith_BV);


//channel 2
    Gre2d_SetFCh_address(FCH2_CH, gre2d_value.src2.add0, gre2d_value.src2.add1, gre2d_value.src2.add2);

    Gre2d_SetFCh_position(FCH2_CH, gre2d_value.src2.frame_pix_sx, gre2d_value.src2.frame_pix_sy, gre2d_value.src2.src_off_sx, gre2d_value.src2.src_off_sy,
                                    gre2d_value.src2.img_pix_sx, gre2d_value.src2.img_pix_sy, gre2d_value.src2.win_off_sx, gre2d_value.src2.win_off_sy);

    Gre2d_SetFCh_control(FCH2_CH, gre2d_value.src2.op_mode, ZF, gre2d_value.src2.src_form);

    Gre2d_SetFCh_Chroma_key(FCH2_CH, gre2d_value.src2.chroma_RY, gre2d_value.src2.chroma_GU, gre2d_value.src2.chroma_BV);

    Gre2d_SetFCh_Arithmetic_par(FCH2_CH, gre2d_value.src1.arith_RY, gre2d_value.src2.arith_GU, gre2d_value.src2.arith_BV);
#endif//

//channel control 
    src_ctrl.src0_arith = gre2d_value.src0.arith_mode;
    src_ctrl.src0_chroma_en = gre2d_value.src0.src_chroma_en;
    src_ctrl.src_sel_0 = FCH0_CH;
    src_ctrl.src0_y2r.src_y2r = gre2d_value.src0.src_y2r;
    src_ctrl.src0_y2r.src_y2r_type = gre2d_value.src0.src_y2r_type;
  
    src_ctrl.src1_arith = AR_NOOP;
    src_ctrl.src1_chroma_en = 0;
    src_ctrl.src_sel_1 = FCH1_CH;
    src_ctrl.src1_y2r.src_y2r = 0;
    src_ctrl.src1_y2r.src_y2r_type = 0;

    src_ctrl.src2_arith = AR_NOOP;
    src_ctrl.src2_chroma_en = 0;
    src_ctrl.src_sel_2 = FCH2_CH;
    src_ctrl.src2_y2r.src_y2r = 0;
    src_ctrl.src2_y2r.src_y2r_type = 0;
#if defined(CONFIG_ARCH_TCC892X)
    src_ctrl.src_sel_3 = FCH3_CH;
#endif//
    Gre2d_src_ctrl(src_ctrl);


// operator 0 pattern setting
    Gre2d_operator_set(OP_0, 0, 0, 0, 0);
// operator 1 pattern setting
    Gre2d_operator_set(OP_1, 0, 0, 0, 0);


	
	
	#if defined(CONFIG_ARCH_TCC892X)
	// operator 1 pattern setting
	Gre2d_operator_set(OP_2, 0, 0, 0, 0);
#endif//
	#if defined(CONFIG_ARCH_TCC892X)
	Gre2d_operator_ctrl(OP_0, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1,gG2D_CCON0,gG2D_ATUNE, 
							CHROMA_OP0_NOOP, GE_ROP_SRC_COPY);

	Gre2d_operator_ctrl(OP_1, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1,gG2D_CCON0,gG2D_ATUNE, 
							CHROMA_OP1_NOOP,GE_ROP_SRC_COPY);
	Gre2d_operator_ctrl(OP_2, gG2D_ACON1,gG2D_ACON0,
							gG2D_CCON1,gG2D_CCON0,gG2D_ATUNE, 
							CHROMA_OP1_NOOP,GE_ROP_SRC_COPY);

	#else 
	Gre2d_operator_ctrl(gG2D_ASEL1, CHROMA_OP1_NOOP, GE_ROP_SRC_COPY, gG2D_ASEL0, CHROMA_OP0_NOOP, GE_ROP_SRC_COPY);	
	#endif

//back end channel 
    Gre2d_SetBCh_address(DEST_CH,  gre2d_value.dest.add0, gre2d_value.dest.add1, gre2d_value.dest.add2);

    Gre2d_SetBCh_position(DEST_CH, gre2d_value.dest.frame_pix_sx, gre2d_value.dest.frame_pix_sy, gre2d_value.dest.dest_off_sx, gre2d_value.dest.dest_off_sy);


	dest_ctrl.MABC = gG2D_DEST_MABC;
	dest_ctrl.ysel =  gre2d_value.dest.ysel;
	dest_ctrl.xsel = gre2d_value.dest.xsel;
	dest_ctrl.converter_en = gre2d_value.dest.converter_en;
	dest_ctrl.converter_mode = gre2d_value.dest.converter_mode;
	dest_ctrl.DSUV = gre2d_value.dest.dest_form.uv_order;
	dest_ctrl.opmode = gre2d_value.dest.op_mode;
	dest_ctrl.dithering_type = gG2D_Dithering_type;
	dest_ctrl.dithering_en = gG2D_Dithering_en;
	dest_ctrl.data_form = gre2d_value.dest.dest_form;
	Gre2d_SetBCh_control(&dest_ctrl);
	
	if(gG2D_Dithering_en)
		Gre2d_SetDitheringMatrix(DitheringMatrix);
	
	//channel enable : Front End Channel 0 enable
	gre2d_waiting_result(GRP_F0);

    return ret;
}





/* -------------- GRAPHIC ENGINE APPLICATION  FUNCTION  ------------ */

 /*------------------------------------------------------------------
gre2d_enabe
 graphic engine S/W reset & clock enalbe 
-------------------------------------------------------------------*/
void gre2d_enabe(void)
{
	Gre2d_enable();
}

/*------------------------------------------------------------------
gre2d_disable
 graphic engine  clock disable 
-------------------------------------------------------------------*/
void gre2d_disable(void)
{
	Gre2d_disable();
}

/*------------------------------------------------------------------
gre2d_interrupt_ctrl
 graphic engine interrupt control 
 wr : 1 : write   0 : read
 int_irq : interrupt request 
 int_flg : flag bit
-------------------------------------------------------------------*/
G2D_INT_TYPE gre2d_interrupt_ctrl(unsigned char wr, G2D_INT_TYPE flag, unsigned char int_irq, unsigned char int_flg)
{
	G2D_INT_TYPE ret_v = 0;
	
	ret_v = Gre2d_Grp_int_ctrl(wr, flag, int_irq, int_flg);

	return ret_v;
}



/*-----------------------------------------------------------------------------
gre2d_ChImgSize
 graphic engine image SIZE converter
 
Graphic engine 의 image SIZE 를 변경 해 준다.
-----------------------------------------------------------------------------*/
unsigned char gre2d_ChImgSize(unsigned int src0,unsigned int src1, unsigned int src2,
						unsigned int src_w, unsigned int src_h, unsigned int str_x, unsigned int str_y, G2D_FMT_CTRL src_fmt,
						unsigned int tgt0, unsigned int tgt1, unsigned int tgt2, 
						unsigned int tgt_w, unsigned int tgt_h, G2D_FMT_CTRL dest_fmt)
{
    unsigned char ret = FALSE;
    G2d_1CH_FUNC gre2d_value;
    IMGFMT_CONV_TYPE change_type = NONE;

#if ALPHA_RGB_DIABLE        
// alpha - RGB 는 결과 값으로 설정 할수 없다.
	if((fmt == GE_RGB444_A) || (fmt == GE_RGB454_A) || (fmt == GE_RGB555_A) || 
		(fmt == GE_RGB666_A) || (fmt == GE_RGB888_A))
	{
	        return FALSE;
	}
#endif//
        
	memset((char*)&gre2d_value, (int)NULL, sizeof(G2d_1CH_FUNC));

    if((src_fmt.format >= GE_RGB444) && (dest_fmt.format < GE_RGB444)) 
        change_type = R2Y_TYPE;
    else if((src_fmt.format < GE_RGB444) && (dest_fmt.format >= GE_RGB444)) 
        change_type = Y2R_TYPE;

// front channel setting
    gre2d_value.src0.add0 = src0;
    gre2d_value.src0.add1 = src1;
    gre2d_value.src0.add2 = src2;
    gre2d_value.src0.frame_pix_sx = src_w;
    gre2d_value.src0.frame_pix_sy = src_h;        
    gre2d_value.src0.src_off_sx = str_x;
    gre2d_value.src0.src_off_sy = str_y;        
    gre2d_value.src0.img_pix_sx = tgt_w;
    gre2d_value.src0.img_pix_sy = tgt_h;  
    gre2d_value.src0.win_off_sx = 0;
    gre2d_value.src0.win_off_sy = 0;  

    gre2d_value.src0.op_mode = NOOP;
    gre2d_value.src0.src_form = src_fmt;

    gre2d_value.src0.chroma_RY = 0;
    gre2d_value.src0.chroma_GU = 0;        
    gre2d_value.src0.chroma_BV = 0;

    gre2d_value.src0.src_chroma_en = FALSE;
    gre2d_value.src0.arith_mode = AR_NOOP;
    gre2d_value.src0.arith_RY = 0;
    gre2d_value.src0.arith_GU = 0;        
    gre2d_value.src0.arith_BV = 0;

// YUV to RGB 는 front image format coverter 
    if(change_type == Y2R_TYPE){
            gre2d_value.src0.src_y2r = TRUE;        
            gre2d_value.src0.src_y2r_type = gre2d_Y2R_type;
    }


// back end channel setting
    gre2d_value.dest.add0 = tgt0;
    gre2d_value.dest.add1 = tgt1;
    gre2d_value.dest.add2 = tgt2;
    gre2d_value.dest.frame_pix_sx = tgt_w;
    gre2d_value.dest.frame_pix_sy = tgt_h;
    gre2d_value.dest.dest_off_sx = 0;
    gre2d_value.dest.dest_off_sy = 0;
    gre2d_value.dest.ysel = 0;
    gre2d_value.dest.xsel = 0;


// RGB to YUV 는 back end image format coverter 
    if(change_type == R2Y_TYPE){
            gre2d_value.dest.converter_en = TRUE;
            gre2d_value.dest.converter_mode = gre2d_R2Y_type;
    }

    
    gre2d_value.dest.op_mode = NOOP;
    gre2d_value.dest.dest_form = dest_fmt;
   
    ret = gre2d_1ch_dma_main_func(gre2d_value);

	return ret;
}

/*-----------------------------------------------------------------------------
gre2d_ChImgFmt
 graphic engine image format converter
 
Graphic engine 의 image format를 변경 해 준다.
-----------------------------------------------------------------------------*/
unsigned char gre2d_ChImgFmt(unsigned int src0,unsigned int src1, unsigned int src2, G2D_FMT_CTRL srcfm,
		        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2, G2D_FMT_CTRL tgtfm,
			unsigned int imgx, unsigned int imgy)

{
        unsigned char ret = FALSE;
        G2d_1CH_FUNC gre2d_value;
        IMGFMT_CONV_TYPE change_type = NONE;

#if ALPHA_RGB_DIABLE        
// alpha - RGB 는 결과 값으로 설정 할수 없다.
	if((srcfm.format == tgtfm) || (tgtfm.format == GE_RGB444_A) || (tgtfm.format == GE_RGB454_A) || (tgtfm.format == GE_RGB555_A) || 
	(tgtfm.format == GE_RGB666_A) || (tgtfm.format == GE_RGB888_A))
	{
	        return FALSE;
	}
#endif//

        if((srcfm.format >= GE_RGB444) && (tgtfm.format < GE_RGB444)) 
                change_type = R2Y_TYPE;
        else if((srcfm.format < GE_RGB444) && (tgtfm.format >= GE_RGB444)) 
                change_type = Y2R_TYPE;

        
	memset((char *)&gre2d_value,(int) NULL, sizeof(G2d_1CH_FUNC));


// front channel setting
        gre2d_value.src0.add0 = src0;
        gre2d_value.src0.add1 = src1;
        gre2d_value.src0.add2 = src2;
        gre2d_value.src0.frame_pix_sx = imgx;
        gre2d_value.src0.frame_pix_sy = imgy;        
        gre2d_value.src0.src_off_sx = 0;
        gre2d_value.src0.src_off_sy = 0;        
        gre2d_value.src0.img_pix_sx = imgx;
        gre2d_value.src0.img_pix_sy = imgy;  
        gre2d_value.src0.win_off_sx = 0;
        gre2d_value.src0.win_off_sy = 0;  

        gre2d_value.src0.op_mode = NOOP;
        gre2d_value.src0.src_form = srcfm;

        gre2d_value.src0.chroma_RY = 0;
        gre2d_value.src0.chroma_GU = 0;        
        gre2d_value.src0.chroma_BV = 0;

        gre2d_value.src0.src_chroma_en = FALSE;
        gre2d_value.src0.arith_mode = AR_NOOP;
        gre2d_value.src0.arith_RY = 0;
        gre2d_value.src0.arith_GU = 0;        
        gre2d_value.src0.arith_BV = 0;


// YUV to RGB 는 front image format coverter 
        if(change_type == Y2R_TYPE){
                gre2d_value.src0.src_y2r = TRUE;        
                gre2d_value.src0.src_y2r_type = gre2d_Y2R_type;
        }


// back end channel setting
        gre2d_value.dest.add0 = tgt0;
        gre2d_value.dest.add1 = tgt1;
        gre2d_value.dest.add2 = tgt2;
        gre2d_value.dest.frame_pix_sx = imgx;
        gre2d_value.dest.frame_pix_sy = imgy;
        gre2d_value.dest.dest_off_sx = 0;
        gre2d_value.dest.dest_off_sy = 0;
        gre2d_value.dest.ysel = 0;
        gre2d_value.dest.xsel = 0;


// RGB to YUV 는 back end image format coverter 
        if(change_type == R2Y_TYPE){
                gre2d_value.dest.converter_en = TRUE;
                gre2d_value.dest.converter_mode = gre2d_R2Y_type;
        }
        
        gre2d_value.dest.op_mode = NOOP;
        gre2d_value.dest.dest_form = tgtfm;
       
        ret = gre2d_1ch_dma_main_func(gre2d_value);

	return ret;
}

/*-----------------------------------------------------------------------------
gre2d_ImgRotate
 graphic engine image rotate function
 
Graphic engine 의 image rotate 및 Flip 기능을 수행 한다.
-----------------------------------------------------------------------------*/
unsigned char gre2d_ImgRotate(unsigned int src0, unsigned int src1, unsigned int src2,
                        G2D_FMT_CTRL srcfm, unsigned int  src_imgx, unsigned int  src_imgy,
		        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2,
		        G2D_FMT_CTRL tgtfm, unsigned int  des_imgx, unsigned int  des_imgy, G2D_OP_MODE ch_mode)
{
    unsigned char ret = FALSE;
    G2d_1CH_FUNC gre2d_value;
    IMGFMT_CONV_TYPE change_type = NONE;

#if ALPHA_RGB_DIABLE        
// alpha - RGB 는 결과 값으로 설정 할수 없다.
	if((tgtfm == GE_RGB444_A) || (tgtfm == GE_RGB454_A) || (tgtfm == GE_RGB555_A) || 
	(tgtfm == GE_RGB666_A) || (tgtfm == GE_RGB888_A))
	{
	        return FALSE;
	}
#endif//

	if((srcfm.format >= GE_RGB444) && (tgtfm.format < GE_RGB444)) 
	    change_type = R2Y_TYPE;
	else if((srcfm.format < GE_RGB444) && (tgtfm.format >= GE_RGB444)) 
	    change_type = Y2R_TYPE;
        
	memset(&gre2d_value, (int)NULL, sizeof(G2d_1CH_FUNC));

	// front channel setting
	gre2d_value.src0.add0 = src0;
	gre2d_value.src0.add1 = src1;
	gre2d_value.src0.add2 = src2;
	gre2d_value.src0.frame_pix_sx = src_imgx;
	gre2d_value.src0.frame_pix_sy = src_imgy;        
	gre2d_value.src0.src_off_sx = 0;
	gre2d_value.src0.src_off_sy = 0;        
	gre2d_value.src0.img_pix_sx = src_imgx;
	gre2d_value.src0.img_pix_sy = src_imgy;  
	gre2d_value.src0.win_off_sx = 0;
	gre2d_value.src0.win_off_sy = 0;  

	// rotate option 적용
	gre2d_value.src0.op_mode = ch_mode;

	gre2d_value.src0.src_form = srcfm;

	gre2d_value.src0.chroma_RY = 0;
	gre2d_value.src0.chroma_GU = 0;        
	gre2d_value.src0.chroma_BV = 0;

	gre2d_value.src0.src_chroma_en = FALSE;
	gre2d_value.src0.arith_mode = AR_NOOP;
	gre2d_value.src0.arith_RY = 0;
	gre2d_value.src0.arith_GU = 0;        
	gre2d_value.src0.arith_BV = 0;


	// YUV to RGB 는 front image format coverter 
	if(change_type == Y2R_TYPE)
	{
	        gre2d_value.src0.src_y2r = TRUE;        
	        gre2d_value.src0.src_y2r_type = gre2d_Y2R_type;
	}

	// back end channel setting
	gre2d_value.dest.add0 = tgt0;
	gre2d_value.dest.add1 = tgt1;
	gre2d_value.dest.add2 = tgt2;
	gre2d_value.dest.frame_pix_sx = des_imgx;
	gre2d_value.dest.frame_pix_sy = des_imgy;
	gre2d_value.dest.dest_off_sx = 0;
	gre2d_value.dest.dest_off_sy = 0;
	gre2d_value.dest.ysel = 0;
	gre2d_value.dest.xsel = 0;


	// RGB to YUV 는 back end image format coverter 
	if(change_type == R2Y_TYPE)
	{
        gre2d_value.dest.converter_en = TRUE;
        gre2d_value.dest.converter_mode = gre2d_R2Y_type;
	}

    gre2d_value.dest.op_mode = NOOP;
    gre2d_value.dest.dest_form = tgtfm;
   
    ret = gre2d_1ch_dma_main_func(gre2d_value);

	return ret;
}

unsigned char gre2d_ImgRotate_Ex(unsigned int src0, unsigned int src1, unsigned int src2,
                        G2D_FMT_CTRL srcfm, unsigned int  src_imgx, unsigned int  src_imgy,
                        unsigned int img_off_x, unsigned int img_off_y, unsigned int Rimg_x, unsigned int Rimg_y,
		        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2,
		        G2D_FMT_CTRL tgtfm, unsigned int  des_imgx, unsigned int  des_imgy, 
		        unsigned int dest_off_x, unsigned int dest_off_y, G2D_OP_MODE ch_mode, G2D_OP_MODE parallel_ch_mode)
{
	unsigned char ret = FALSE;
	G2d_1CH_FUNC gre2d_value;
	IMGFMT_CONV_TYPE change_type = NONE;

#if ALPHA_RGB_DIABLE        
// alpha - RGB 는 결과 값으로 설정 할수 없다.
	if((tgtfm == GE_RGB444_A) || (tgtfm == GE_RGB454_A) || (tgtfm == GE_RGB555_A) || 
	(tgtfm == GE_RGB666_A) || (tgtfm == GE_RGB888_A))
	{
	        return FALSE;
	}
#endif//

	if((srcfm.format >= GE_RGB444) && (tgtfm.format < GE_RGB444)) 
		change_type = R2Y_TYPE;
	else if((srcfm.format < GE_RGB444) && (tgtfm.format >= GE_RGB444)) 
		change_type = Y2R_TYPE;
        
	memset(&gre2d_value, (int)NULL, sizeof(G2d_1CH_FUNC));

// front channel setting
        gre2d_value.src0.add0 = src0;
        gre2d_value.src0.add1 = src1;
        gre2d_value.src0.add2 = src2;
        gre2d_value.src0.frame_pix_sx = src_imgx;
        gre2d_value.src0.frame_pix_sy = src_imgy;        
        gre2d_value.src0.src_off_sx = img_off_x;
        gre2d_value.src0.src_off_sy = img_off_y;        
        gre2d_value.src0.img_pix_sx = Rimg_x;
        gre2d_value.src0.img_pix_sy = Rimg_y;  
        gre2d_value.src0.win_off_sx = 0;
        gre2d_value.src0.win_off_sy = 0;  

// rotate option 적용
        gre2d_value.src0.op_mode = ch_mode;
        
        gre2d_value.src0.src_form = srcfm;

        gre2d_value.src0.chroma_RY = 0;
        gre2d_value.src0.chroma_GU = 0;        
        gre2d_value.src0.chroma_BV = 0;

        gre2d_value.src0.src_chroma_en = FALSE;
        gre2d_value.src0.arith_mode = AR_NOOP;
        gre2d_value.src0.arith_RY = 0;
        gre2d_value.src0.arith_GU = 0;        
        gre2d_value.src0.arith_BV = 0;


// YUV to RGB 는 front image format coverter 
        if(change_type == Y2R_TYPE)
	{
                gre2d_value.src0.src_y2r = TRUE;        
                gre2d_value.src0.src_y2r_type = gre2d_Y2R_type;
        }
        
// back end channel setting
        gre2d_value.dest.add0 = tgt0;
        gre2d_value.dest.add1 = tgt1;
        gre2d_value.dest.add2 = tgt2;
        gre2d_value.dest.frame_pix_sx = des_imgx;
        gre2d_value.dest.frame_pix_sy = des_imgy;
        gre2d_value.dest.dest_off_sx = dest_off_x;
        gre2d_value.dest.dest_off_sy = dest_off_y;
        gre2d_value.dest.ysel = 0;
        gre2d_value.dest.xsel = 0;


// RGB to YUV 는 back end image format coverter 
        if(change_type == R2Y_TYPE)
		{
             gre2d_value.dest.converter_en = TRUE;
             gre2d_value.dest.converter_mode = gre2d_R2Y_type;
        }

		// Exception Handling of parallel_ch_mode
		if(parallel_ch_mode < NOOP || parallel_ch_mode > ROTATE_270)
			parallel_ch_mode = NOOP;
        
       	if(parallel_ch_mode)
			gre2d_value.dest.op_mode = parallel_ch_mode;
		else
			gre2d_value.dest.op_mode = NOOP;

		gre2d_value.dest.dest_form = tgtfm;
       
        ret = gre2d_1ch_dma_main_func(gre2d_value);

	return ret;
}

/*------------------------------------------------------------------------------
gre2d_ImgArith
 graphic engine image arithmetic operation
 
Graphic engine 의 image arithmetic 연산 해 준다.
RGB888 type 로 연산됨
------------------------------------------------------------------------------*/
unsigned char gre2d_ImgArithmetic(unsigned int src0, unsigned int src1, unsigned int src2, 
								G2D_FMT_CTRL srcfm, unsigned int  src_w, unsigned int  src_h ,
						        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2, G2D_FMT_CTRL tgtfm,
								unsigned int  dest_w, unsigned int  dest_h , unsigned int dest_off_x, unsigned int  dest_off_y, 
								G2D_ARITH_TYPE arith, unsigned char R, unsigned char G, unsigned char B)
{
        unsigned char ret = FALSE;
        G2d_1CH_FUNC gre2d_value;
        IMGFMT_CONV_TYPE src0_fmt_ch = NONE; //format change option
        IMGFMT_CONV_TYPE dst_fmt_ch = NONE; //format change option

#if ALPHA_RGB_DIABLE        
// alpha - RGB 는 결과 값으로 설정 할수 없다.
	if((tgtfm == GE_RGB444_A) || (tgtfm == GE_RGB454_A) || (tgtfm == GE_RGB555_A) || 
	(tgtfm == GE_RGB666_A) || (tgtfm == GE_RGB888_A))
	{
	        return FALSE;
	}
#endif//

        if(srcfm.format < GE_RGB332) 
                src0_fmt_ch = Y2R_TYPE;

        if(tgtfm.format < GE_RGB332) 
                dst_fmt_ch = R2Y_TYPE;

        
	memset(&gre2d_value, (int)NULL, sizeof(G2d_1CH_FUNC));

// front channel setting
        gre2d_value.src0.add0 = src0;
        gre2d_value.src0.add1 = src1;
        gre2d_value.src0.add2 = src2;
        gre2d_value.src0.frame_pix_sx = src_w;
        gre2d_value.src0.frame_pix_sy = src_h;        
        gre2d_value.src0.src_off_sx = 0;
        gre2d_value.src0.src_off_sy = 0;        
        gre2d_value.src0.img_pix_sx = src_w;
        gre2d_value.src0.img_pix_sy = src_h;  
        gre2d_value.src0.win_off_sx = 0;
        gre2d_value.src0.win_off_sy = 0;  

        gre2d_value.src0.op_mode = NOOP;
        gre2d_value.src0.src_form = srcfm;


        gre2d_value.src0.src_chroma_en = FALSE;
        gre2d_value.src0.chroma_RY = 0;
        gre2d_value.src0.chroma_GU = 0;
        gre2d_value.src0.chroma_BV = 0;

        gre2d_value.src0.arith_mode = arith;
        gre2d_value.src0.arith_RY = R;
        gre2d_value.src0.arith_GU = G;        
        gre2d_value.src0.arith_BV = B;


// YUV to RGB 는 front image format coverter 
        if(src0_fmt_ch == Y2R_TYPE){
                gre2d_value.src0.src_y2r = TRUE;        
                gre2d_value.src0.src_y2r_type = gre2d_Y2R_type;
        }


// back end channel setting
        gre2d_value.dest.add0 = tgt0;
        gre2d_value.dest.add1 = tgt1;
        gre2d_value.dest.add2 = tgt2;
        gre2d_value.dest.frame_pix_sx = dest_w;
        gre2d_value.dest.frame_pix_sy = dest_h;
        gre2d_value.dest.dest_off_sx = dest_off_x;
        gre2d_value.dest.dest_off_sy = dest_off_y;
        gre2d_value.dest.ysel = 0;
        gre2d_value.dest.xsel = 0;


// RGB to YUV 는 back end image format coverter 
        if(dst_fmt_ch == R2Y_TYPE){
                gre2d_value.dest.converter_en = TRUE;
                gre2d_value.dest.converter_mode = gre2d_R2Y_type;
        }
        
        gre2d_value.dest.op_mode = NOOP;
        gre2d_value.dest.dest_form = tgtfm;
       
        ret = gre2d_1ch_dma_main_func(gre2d_value);

	return ret;
}


/*------------------------------------------------------------------------------
gre2d_ImgOverlay
 graphic engine overlay function
 
Graphic engine 의 2 channel alpha-blending 및 chroam-key 기능을 수행 한다.

alpha_en 이 disable 일때는 2 channel를 add 연산 한다. 
------------------------------------------------------------------------------*/
unsigned char gre2d_ImgOverlay(G2D_OVERY_FUNC *overlay)
{
        unsigned char ret = FALSE;
        G2d_2CH_FUNC gre2d_value;
        IMGFMT_CONV_TYPE src0_fmt_ch = NONE, src1_fmt_ch = NONE, dest_fmt_ch = NONE; // format change option


#if ALPHA_RGB_DIABLE                
// alpha - RGB 는 결과 값으로 설정 할수 없다.
	if((overlay->dest.dest_form== GE_RGB444_A) || (overlay->dest.dest_form == GE_RGB454_A) || (overlay->dest.dest_form == GE_RGB555_A) || 
	(overlay->dest.dest_form == GE_RGB666_A) || (overlay->dest.dest_form == GE_RGB888_A))
	{
	        return FALSE;
	}


        if(overlay->alpha_en == G2d_ALAPH_RGB)  
        {
                if ((overlay->dest.dest_form== GE_RGB444_A) || (overlay->dest.dest_form == GE_RGB454_A) 
                || (overlay->dest.dest_form == GE_RGB555_A) || (overlay->dest.dest_form == GE_RGB666_A) || (overlay->dest.dest_form == GE_RGB888_A))
                        return FALSE;
        }
#endif//

// format change 
        if(!(((overlay->dest.dest_form.format >= GE_RGB444) && (overlay->src0.src_form.format >= GE_RGB444) && (overlay->src1.src_form.format >= GE_RGB444))
                || ((overlay->dest.dest_form.format < GE_RGB444) && (overlay->src0.src_form.format < GE_RGB444) && (overlay->src1.src_form.format < GE_RGB444))))
        {
                if(overlay->src0.src_form.format < GE_RGB444)      src0_fmt_ch = Y2R_TYPE;
                
                if(overlay->src1.src_form.format < GE_RGB444)      src1_fmt_ch = Y2R_TYPE;

                if(overlay->dest.dest_form.format < GE_RGB444)     dest_fmt_ch = R2Y_TYPE;
        }
       
		memset(&gre2d_value, (int)NULL, sizeof(G2d_2CH_FUNC));


        
/*         front channel setting          */
// FRONT CHANNEL 0 SETTING
        gre2d_value.src0.add0 = overlay->src0.add0;
        gre2d_value.src0.add1 = overlay->src0.add1;
        gre2d_value.src0.add2 = overlay->src0.add2;
        gre2d_value.src0.frame_pix_sx = overlay->src0.frame_pix_sx;
        gre2d_value.src0.frame_pix_sy = overlay->src0.frame_pix_sy;        
        gre2d_value.src0.src_off_sx = overlay->src0.src_off_sx;
        gre2d_value.src0.src_off_sy = overlay->src0.src_off_sy;        
        gre2d_value.src0.img_pix_sx = overlay->src0.img_pix_sx;
        gre2d_value.src0.img_pix_sy = overlay->src0.img_pix_sy;  
        gre2d_value.src0.win_off_sx = 0; 
        gre2d_value.src0.win_off_sy = 0;  

// rotate option 적용
        gre2d_value.src0.op_mode = overlay->src0.op_mode;
        
        gre2d_value.src0.src_form = overlay->src0.src_form;


        gre2d_value.src0.src_chroma_en = FALSE;
        gre2d_value.src0.arith_mode = AR_NOOP;


// YUV to RGB 는 front image format coverter 
        if(src0_fmt_ch == Y2R_TYPE){
                gre2d_value.src0.src_y2r = TRUE;        
                gre2d_value.src0.src_y2r_type = gre2d_Y2R_type;
        }

// FRONT CHANNEL 1 SETTING        
        gre2d_value.src1.add0 = overlay->src1.add0;
        gre2d_value.src1.add1 = overlay->src1.add1;
        gre2d_value.src1.add2 = overlay->src1.add2;
        gre2d_value.src1.frame_pix_sx = overlay->src1.frame_pix_sx;
        gre2d_value.src1.frame_pix_sy = overlay->src1.frame_pix_sy;        
        gre2d_value.src1.src_off_sx = overlay->src1.src_off_sx;
        gre2d_value.src1.src_off_sy = overlay->src1.src_off_sy;        
        gre2d_value.src1.img_pix_sx = overlay->src1.img_pix_sx;
        gre2d_value.src1.img_pix_sy = overlay->src1.img_pix_sy;  
        gre2d_value.src1.win_off_sx = overlay->src1.win_off_sx; 
        gre2d_value.src1.win_off_sy = overlay->src1.win_off_sy;  

// rotate option 적용
        gre2d_value.src1.op_mode = overlay->src1.op_mode;
        
        gre2d_value.src1.src_form = overlay->src1.src_form;

        gre2d_value.src1.src_chroma_en = FALSE;
        gre2d_value.src1.arith_mode = AR_NOOP;



// YUV to RGB 는 front image format coverter 
        if(src1_fmt_ch == Y2R_TYPE){
                gre2d_value.src1.src_y2r = TRUE;        
                gre2d_value.src1.src_y2r_type = gre2d_Y2R_type;
        }


// chroma-key 적용   
        if(overlay->op_ctrl_scel0 == CHROMA_S0)
        {
                // S0_CHROMA        
                gre2d_value.src0.chroma_RY = overlay->chroma_RY;
                gre2d_value.src0.chroma_GU = overlay->chroma_GU;        
                gre2d_value.src0.chroma_BV = overlay->chroma_BV;

        }
        else if(overlay->op_ctrl_scel0 == CHROMA_S1)
        {
                // S1_CHROMA        
                gre2d_value.src1.chroma_RY = overlay->chroma_RY;
                gre2d_value.src1.chroma_GU = overlay->chroma_GU;        
                gre2d_value.src1.chroma_BV = overlay->chroma_BV;
        }
        gre2d_value.csel0 = overlay->op_ctrl_scel0;


//operator setting
        if(overlay->alpha_en == G2d_ALPHA_NONE)
        {
                gre2d_value.op_mode0 = GE_ROP_SRC_AND;
        }
        else if(overlay->alpha_en == G2d_ALPHA_VALUE)
        {
                gre2d_value.op_pat_0.op_alpha = overlay->alpha_value;
                gre2d_value.op_mode0 = GE_ROP_ALPHA_0;
        }
        else
        {
                gre2d_value.op_mode0 = GE_ROP_ALPHA_1;
        }


// back end channel setting
        gre2d_value.dest.add0 = overlay->dest.add0;
        gre2d_value.dest.add1 = overlay->dest.add1;
        gre2d_value.dest.add2 = overlay->dest.add2;
        gre2d_value.dest.frame_pix_sx = overlay->dest.frame_pix_sx;
        gre2d_value.dest.frame_pix_sy = overlay->dest.frame_pix_sy;
        gre2d_value.dest.dest_off_sx = overlay->dest.dest_off_sx;
        gre2d_value.dest.dest_off_sy = overlay->dest.dest_off_sy;
        gre2d_value.dest.ysel = 0;
        gre2d_value.dest.xsel = 0;

        gre2d_value.dest.op_mode = overlay->dest.op_mode;
        gre2d_value.dest.dest_form = overlay->dest.dest_form;
        
// RGB to YUV 는 back end image format coverter 
        if(dest_fmt_ch == R2Y_TYPE){
                gre2d_value.dest.converter_en = TRUE;
                gre2d_value.dest.converter_mode = gre2d_R2Y_type;
        }
       
        ret = gre2d_2ch_dma_main_func(gre2d_value);

	return ret;
}



/*------------------------------------------------------------------------------
gre2d_ImgROP
 graphic engine ROP(Raster operation) function
 
Graphic engine 의 1~3개의 Input image의 image size, offset, window offset 및
rotate, flip, format change 등의 처리 후에 Rop 연산 기능을 수행함

RGB888 type 로 연산됨
------------------------------------------------------------------------------*/
unsigned char gre2d_ImgROP(G2D_ROP_FUNC rop , G2D_EN en_channel)
{
        unsigned char ret = FALSE;
        G2D_FUNC_TYPE gre2d_value;
        IMGFMT_CONV_TYPE src0_fmt_ch = NONE, src1_fmt_ch = NONE, src2_fmt_ch = NONE, dest_fmt_ch = NONE; // format change option

#if ALPHA_RGB_DIABLE        
        // alpha - RGB 는 결과 값으로 설정 할수 없다.
	if((rop.dest.dest_form== GE_RGB444_A) || (rop.dest.dest_form == GE_RGB454_A) || (rop.dest.dest_form == GE_RGB555_A) || 
	(rop.dest.dest_form == GE_RGB666_A) || (rop.dest.dest_form == GE_RGB888_A))
	{
	        return FALSE;
	}
#endif//

// YUV image를 rop 연산 하면 color가 이상해 진다.
//       format change 
//        if(!(((rop.dest.dest_form >= GE_RGB444) && (rop.src0.src_form >= GE_RGB444) && (rop.src1.src_form >= GE_RGB444) && (rop.src2.src_form >= GE_RGB444))
//        || ((rop.dest.dest_form < GE_RGB444) && (rop.src0.src_form < GE_RGB444) && (rop.src1.src_form < GE_RGB444) && (rop.src2.src_form < GE_RGB444)))
//        )

	if(rop.src0.src_form.format < GE_RGB332)      src0_fmt_ch = Y2R_TYPE;
	if(rop.src1.src_form.format < GE_RGB332)      src1_fmt_ch = Y2R_TYPE;
	if(rop.src2.src_form.format < GE_RGB332)      src2_fmt_ch = Y2R_TYPE;
	if(rop.dest.dest_form.format < GE_RGB332)     dest_fmt_ch = R2Y_TYPE;
       
	memset(&gre2d_value, (int)NULL, sizeof(G2D_FUNC_TYPE));

/*         Front channel setting          */

// FRONT CHANNEL 0 SETTING
        gre2d_value.src0.add0 = rop.src0.add0;
        gre2d_value.src0.add1 = rop.src0.add1;
        gre2d_value.src0.add2 = rop.src0.add2;
        gre2d_value.src0.frame_pix_sx = rop.src0.frame_pix_sx;
        gre2d_value.src0.frame_pix_sy = rop.src0.frame_pix_sy;        
        gre2d_value.src0.src_off_sx = rop.src0.src_off_sx;
        gre2d_value.src0.src_off_sy = rop.src0.src_off_sy;        
        gre2d_value.src0.img_pix_sx = rop.src0.img_pix_sx;
        gre2d_value.src0.img_pix_sy = rop.src0.img_pix_sy;  
        gre2d_value.src0.win_off_sx = 0; 
        gre2d_value.src0.win_off_sy = 0;  

        // rotate option 적용
        gre2d_value.src0.op_mode = rop.src0.op_mode;
        
        gre2d_value.src0.src_form = rop.src0.src_form;

        gre2d_value.src0.chroma_RY = rop.src0.chroma_R; 
        gre2d_value.src0.chroma_GU = rop.src0.chroma_G; 
        gre2d_value.src0.chroma_BV = rop.src0.chroma_B; 

        gre2d_value.src0.src_chroma_en = FALSE;
        gre2d_value.src0.arith_mode = rop.src0.arith_mode;
		gre2d_value.src0.arith_RY = rop.src0.arith_RY;
		gre2d_value.src0.arith_GU = rop.src0.arith_GU;
		gre2d_value.src0.arith_BV = rop.src0.arith_BV;

        // YUV to RGB 는 front image format coverter 
        if(src0_fmt_ch == Y2R_TYPE){
                gre2d_value.src0.src_y2r = TRUE;        
                gre2d_value.src0.src_y2r_type = gre2d_Y2R_type;
        }
#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
	gre2d_value.src0.clut_en = rop.src0.clut_en;
#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */


// FRONT CHANNEL 1 SETTING        
        gre2d_value.src1.add0 = rop.src1.add0;
        gre2d_value.src1.add1 = rop.src1.add1;
        gre2d_value.src1.add2 = rop.src1.add2;
        gre2d_value.src1.frame_pix_sx = rop.src1.frame_pix_sx;
        gre2d_value.src1.frame_pix_sy = rop.src1.frame_pix_sy;        
        gre2d_value.src1.src_off_sx = rop.src1.src_off_sx;
        gre2d_value.src1.src_off_sy = rop.src1.src_off_sy;        
        gre2d_value.src1.img_pix_sx = rop.src1.img_pix_sx;
        gre2d_value.src1.img_pix_sy = rop.src1.img_pix_sy;  
        gre2d_value.src1.win_off_sx = rop.src1.win_off_sx; 
        gre2d_value.src1.win_off_sy = rop.src1.win_off_sy;  

        // rotate option 적용
        gre2d_value.src1.op_mode = rop.src1.op_mode;
        
        gre2d_value.src1.src_form = rop.src1.src_form;

        gre2d_value.src1.chroma_RY = rop.src1.chroma_R; 
        gre2d_value.src1.chroma_GU = rop.src1.chroma_G; 
        gre2d_value.src1.chroma_BV = rop.src1.chroma_B; 

        gre2d_value.src1.src_chroma_en = FALSE;
        gre2d_value.src1.arith_mode = rop.src1.arith_mode;
		gre2d_value.src1.arith_RY = rop.src1.arith_RY;
		gre2d_value.src1.arith_GU = rop.src1.arith_GU;
		gre2d_value.src1.arith_BV = rop.src1.arith_BV;

        // YUV to RGB 는 front image format coverter 
        if(src1_fmt_ch == Y2R_TYPE){
                gre2d_value.src1.src_y2r = TRUE;        
                gre2d_value.src1.src_y2r_type = gre2d_Y2R_type;
        }
#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
	gre2d_value.src1.clut_en = rop.src1.clut_en;
#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */


// FRONT CHANNEL 1 SETTING        
        gre2d_value.src2.add0 = rop.src2.add0;
        gre2d_value.src2.add1 = rop.src2.add1;
        gre2d_value.src2.add2 = rop.src2.add2;
        gre2d_value.src2.frame_pix_sx = rop.src2.frame_pix_sx;
        gre2d_value.src2.frame_pix_sy = rop.src2.frame_pix_sy;        
        gre2d_value.src2.src_off_sx = rop.src2.src_off_sx;
        gre2d_value.src2.src_off_sy = rop.src2.src_off_sy;        
        gre2d_value.src2.img_pix_sx = rop.src2.img_pix_sx;
        gre2d_value.src2.img_pix_sy = rop.src2.img_pix_sy;  
        gre2d_value.src2.win_off_sx = rop.src2.win_off_sx; 
        gre2d_value.src2.win_off_sy = rop.src2.win_off_sy;  

// rotate option 적용
        gre2d_value.src2.op_mode = rop.src2.op_mode;
        
        gre2d_value.src2.src_form = rop.src2.src_form;

        gre2d_value.src2.chroma_RY = rop.src2.chroma_R; 
        gre2d_value.src2.chroma_GU = rop.src2.chroma_G; 
        gre2d_value.src2.chroma_BV = rop.src2.chroma_B; 
        gre2d_value.src2.src_chroma_en = FALSE;
		
		gre2d_value.src2.arith_mode = rop.src2.arith_mode;
		gre2d_value.src2.arith_RY = rop.src2.arith_RY;
		gre2d_value.src2.arith_GU = rop.src2.arith_GU;
		gre2d_value.src2.arith_BV = rop.src2.arith_BV;

// YUV to RGB 는 front image format coverter 
        if(src2_fmt_ch == Y2R_TYPE){
                gre2d_value.src2.src_y2r = TRUE;        
                gre2d_value.src2.src_y2r_type = gre2d_Y2R_type;
        }
#if defined(TCC_OVERLAY_MIXER_CLUT_SUPPORT)
	gre2d_value.src2.clut_en = rop.src2.clut_en;
#endif /* TCC_OVERLAY_MIXER_CLUT_SUPPORT */
        
// operator 정의
        gre2d_value.op_ctrl.csel0 = rop.op_ctrl.csel0;
        gre2d_value.op_ctrl.op_mode0 = rop.op_ctrl.op_mode0;
        gre2d_value.op_ctrl.csel1 = rop.op_ctrl.csel1;
        gre2d_value.op_ctrl.op_mode1 = rop.op_ctrl.op_mode1;
        
        memcpy(&gre2d_value.op_pat_0, &rop.op_pat_0, sizeof(G2D_OPERATOR_PATT));
        memcpy(&gre2d_value.op_pat_1, &rop.op_pat_1, sizeof(G2D_OPERATOR_PATT));
        

// Back end channel setting
        gre2d_value.dest.add0 = rop.dest.add0;
        gre2d_value.dest.add1 = rop.dest.add1;
        gre2d_value.dest.add2 = rop.dest.add2;
        gre2d_value.dest.frame_pix_sx = rop.dest.frame_pix_sx;
        gre2d_value.dest.frame_pix_sy = rop.dest.frame_pix_sy;
        gre2d_value.dest.dest_off_sx = rop.dest.dest_off_sx;
        gre2d_value.dest.dest_off_sy = rop.dest.dest_off_sy;
        gre2d_value.dest.ysel = 0;
        gre2d_value.dest.xsel = 0;

        gre2d_value.dest.op_mode = rop.dest.op_mode;
        gre2d_value.dest.dest_form = rop.dest.dest_form;
        
        // RGB to YUV 는 back end image format coverter 
        if(dest_fmt_ch == R2Y_TYPE){
                gre2d_value.dest.converter_en = TRUE;
                gre2d_value.dest.converter_mode = gre2d_R2Y_type;
        }

        ret = gre2d_3ch_dma_main_func(gre2d_value, en_channel);

	return ret;
}

#if 0


/* ------------------ SOFTWARE APPLICATION  FUNCTION  ----------------------- */

#define MSB_8BIT(x)     (x & 0x80) ? TRUE : FALSE
#define MSB_6BIT(x)     (x & 0x20) ? TRUE : FALSE
#define MSB_5BIT(x)     (x & 0x10) ? TRUE : FALSE
#define MSB_4BIT(x)     (x & 0x8) ? TRUE : FALSE

/*------------------------------------------------------------------------------
gre2d_RGBxxx2RGB888
 
Graphic engine에서 사용할 RGB888 변환을 해준다. 
ZF 의 값을 참조 해서 하위 비트를 채운다.
------------------------------------------------------------------------------*/
unsigned char  gre2d_RGBxxx2RGB888(G2D_DATA_FM form, unsigned char Red, unsigned char Green, unsigned char Blue,
                        unsigned char *r_Red, unsigned char *r_Green, unsigned char *r_Blue)
{
        unsigned char ret = TRUE;

        if(form  == GE_RGB444 || form == GE_RGB444_A)
        {
                if(ZF | (!MSB_4BIT(Red))) *r_Red = ((Red<<4) & 0xF0);
                else *r_Red = (((Red<<4) & 0xF0) | 0x0F);

                if(ZF | (!MSB_4BIT(Green))) *r_Green = ((Green<<4) & 0xF0);
                else *r_Green = (((Green<<4) & 0xF0) | 0x0F);

                if(ZF | (!MSB_4BIT(Blue))) *r_Blue = ((Blue<<4) & 0xF0);
                else *r_Blue = (((Blue<<4) & 0xF0) | 0x0F);
        }
        else if(form  == GE_RGB454 || form == GE_RGB454_A)
        {
                if(ZF | (!MSB_4BIT(Red))) *r_Red = ((Red<<4) & 0xF0);
                else *r_Red = (((Red<<4) & 0xF0) | 0x0F);

                if(ZF | (!MSB_5BIT(Green))) *r_Green = ((Green<<3) & 0xF8);
                else *r_Green = (((Green<<3) & 0xF8) | 0x07);

                if(ZF | (!MSB_4BIT(Blue))) *r_Blue = ((Blue<<4) & 0xF0);
                else *r_Blue = (((Blue<<4) & 0xF0) | 0x0F);
        }

        else if(form  == GE_RGB555 || form == GE_RGB555_A)
        {
                if(ZF | (!MSB_5BIT(Red))) *r_Red = ((Red<<3) & 0xF8);
                else *r_Red = (((Red<<3) & 0xF8) | 0x07);

                if(ZF | (!MSB_5BIT(Green))) *r_Green = ((Green<<3) & 0xF8);
                else *r_Green = (((Green<<3) & 0xF8) | 0x07);

                if(ZF | (!MSB_5BIT(Blue))) *r_Blue = ((Blue<<3) & 0xF8);
                else *r_Blue = (((Blue<<3) & 0xF8) | 0x07);
        }
        else if(form  == GE_RGB565)
        {
                if(ZF | (!MSB_5BIT(Red))) *r_Red = ((Red<<3) & 0xF8);
                else *r_Red = (((Red<<3) & 0xF8) | 0x07);

                if(ZF | (!MSB_6BIT(Green))) *r_Green = ((Green<<2) & 0xFC);
                else *r_Green = (((Green<<2) & 0xFC) | 0x03);

                if(ZF | (!MSB_5BIT(Blue))) *r_Blue = ((Blue<<3) & 0xF8);
                else *r_Blue = (((Blue<<3) & 0xF8) | 0x07);
        }
        else if(form  == GE_RGB666 || form == GE_RGB666_A)
        {
                if(ZF | (!MSB_6BIT(Red))) *r_Red = ((Red<<2) & 0xFC);
                else *r_Red = (((Red<<2) & 0xFC) | 0x03);

                if(ZF | (!MSB_6BIT(Green))) *r_Green = ((Green<<2) & 0xFC);
                else *r_Green = (((Green<<2) & 0xFC) | 0x03);

                if(ZF | (!MSB_6BIT(Blue))) *r_Blue = ((Blue<<2) & 0xFC);
                else *r_Blue = (((Blue<<2) & 0xFC) | 0x03);
        }
        else ret = FALSE;

        return ret;
}


#define BitCLIP(x) (((x) < 0) ? 0 : (((x) > 255) ? 255 : (x)))

/*------------------------------------------------------------------------------
gre2d_YUVtoRGB888
 
Graphic engine에서 사용할 YUV to RGB888로 변환을 해준다. 
gre2d_Y2R_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
void gre2d_YUVtoRGB888(unsigned char Y, unsigned char U, unsigned char V, unsigned char *R, unsigned char *G, unsigned char *B)
{

	float	tmp_r, tmp_g, tmp_b;

        if(gre2d_Y2R_type == Y2R_TYP0)
        {
                tmp_r = Y + (1 * V) + (0.371094 * V) - 176;
                tmp_g = Y - (0.335938 * U) - (0.697266 * V) + 132;
                tmp_b = Y + (1 * U) + (0.732422 * U) - 222;
        }
        else if(gre2d_Y2R_type == Y2R_TYP1)
        {
                tmp_r =  Y + (1 * V) + (0.164063 * Y) + (0.595703 * V) - 223;
                tmp_g =  Y + (0.164063 * Y) - (0.390625 * U) - (0.814453 * V) + 136;
                tmp_b = Y + (2 * U) + (0.164063 * Y) + (0.017578 * U) - 277;
        }
        else if(gre2d_Y2R_type == Y2R_TYP2)
        {
                tmp_r = Y + (1 * V) + (0.539063 * V) - 197;
                tmp_g = Y - (0.183594 * U) - (0.458984 * V) + 82;
                tmp_b = Y + (1 * U) + (0.816406 * U) - 233;
        }
        else if(gre2d_Y2R_type == Y2R_TYP3)
        {
                tmp_r = Y + (1 * V) + (0.164063 * Y) + (0.792969 * V) - 248;
                tmp_g = Y + (0.164063 * Y) - (0.212891 * U) - (0.533203 * V) + 77;
                tmp_b = Y + (2 * U) + (0.164063 * Y) + (0.115234 * U) - 289;
        }

        *R = BitCLIP(tmp_r);
        *G = BitCLIP(tmp_g);
        *B = BitCLIP(tmp_b);
}



/*------------------------------------------------------------------------------
gre2d_RGB888toYUV_y
 
Graphic engine에서 사용할 RGB888 to YUV의 Y 값을 계산 한다. 변환을 해준다. 
gre2d_R2Y_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
unsigned int	gre2d_RGB888toYUV_y(  unsigned int rgb )
{
        uint y;
        double	tmp_y;
        ushort  r = ((rgb >> 16) & 0xFF);
        ushort  g = ((rgb >> 8) & 0xFF);
        ushort  b = (rgb & 0xFF);

        if(gre2d_R2Y_type == R2Y_TYP0)
        	tmp_y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        else if(gre2d_R2Y_type == R2Y_TYP1)
                tmp_y = (0.257 * r) + (0.504 * g) + (0.098 * b) + 16;
        else if(gre2d_R2Y_type == R2Y_TYP2)
                tmp_y = (0.213 * r) + (0.715 * g) + (0.072 * b);
        else 
                tmp_y = (0.183 * r) + (0.614 * g) + (0.062 * b) + 16;
                
        if( tmp_y > 235 )			tmp_y = 235;
        else if( tmp_y < 16 ) 	tmp_y = 16;
        else					tmp_y = tmp_y;

        y = (unsigned char)tmp_y;

        return 	y;
}


/*------------------------------------------------------------------------------
gre2d_RGB888toYUV_cb
 
Graphic engine에서 사용할 RGB888 to YUV의 U 값을 계산 한다. 변환을 해준다. 
gre2d_R2Y_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
unsigned int	gre2d_RGB888toYUV_cb(  unsigned int  rgb )
{
        unsigned char cb;
        double	tmp_cb;
        
        ushort  r = ((rgb >> 16) & 0xFF);
        ushort  g = ((rgb >> 8) & 0xFF);
        ushort  b = (rgb & 0xFF);

        if(gre2d_R2Y_type == R2Y_TYP0)
                tmp_cb = -(0.172 * r) - (0.339 * g) + (0.511 * b) + 128;
        else if(gre2d_R2Y_type == R2Y_TYP1)
                tmp_cb = -(0.148 * r) - (0.291 * g) + (0.439 * b) + 128;
        else if(gre2d_R2Y_type == R2Y_TYP2)
                tmp_cb = -(0.117 * r) - (0.394 * g) + (0.511 * b) + 128;
        else 
                tmp_cb = -(0.101 * r) - (0.338 * g) + (0.439 * b) + 128;



        if( tmp_cb > 240 )			tmp_cb = 240;
        else if( tmp_cb < 16 ) 	tmp_cb = 16;
        else					tmp_cb = tmp_cb;

        cb = (unsigned char)tmp_cb;

        return 	cb;
}

/*------------------------------------------------------------------------------
gre2d_RGB888toYUV_cr
 
Graphic engine에서 사용할 RGB888 to YUV의 V 값을 계산 한다. 변환을 해준다. 
gre2d_R2Y_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
unsigned int	gre2d_RGB888toYUV_cr(  unsigned int  rgb )
{
        unsigned char cr;
        double	tmp_cr;
        ushort  r = ((rgb >> 16) & 0xFF);
        ushort  g = ((rgb >> 8) & 0xFF);
        ushort  b = (rgb & 0xFF);

        if(gre2d_R2Y_type == R2Y_TYP0)
                tmp_cr = (0.511 * r) - (0.428 * g) - (0.083 * b) + 128;
        else if(gre2d_R2Y_type == R2Y_TYP1)
                tmp_cr = (0.439 * r) - (0.368 * g) - (0.071 * b) + 128;
        else if(gre2d_R2Y_type == R2Y_TYP2)
                tmp_cr = (0.511 * r) - (0.464 * g) - (0.047 * b) + 128;
        else 
                tmp_cr = (0.439 * r) - (0.399 * g) - (0.040 * b) + 128;


        if( tmp_cr > 240 )			tmp_cr = 240;
        else if( tmp_cr < 16 ) 	tmp_cr = 16;
        else					tmp_cr = tmp_cr;

        cr = (unsigned char)tmp_cr;

        return 	cr;
}

#endif 

