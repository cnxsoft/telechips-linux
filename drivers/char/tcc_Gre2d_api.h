/************************************************************************
*    Telechips Graphic Engine application head
*    ------------------------------------------------
*
*    FUNCTION    : Graphic Engine application header 
*    MODEL       :
*    CPU NAME    : TCC83XX
*
*    START DATE    : 2007. 7. 30.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3 TEAM
*                : TELECHIPS, INC.
************************************************************************/
#ifndef __GRE2D_API_H__
#define __GRE2D_API_H__

#include <mach/tcc_Gre2d_type.h>

/*-------------- Graphice engine special application function -------------------------*/
#define SET_G2D_DMA_INT_ENABLE         0x00000001
#define SET_G2D_DMA_INT_DISABLE        0x00000002
extern void G2D_DMA_SET_INTERRUPT(unsigned int uiFlag);

typedef enum{
	G2D_POLLING_TYPE 	= 0,
	G2D_INTERRUPT_TYPE 	= 0x1,
	G2D_CHECK_TYPE 		= 0x2,
	G2D_RSP_MAX
}G2D_RSP_TYPE;

extern void gre2d_rsp_interrupt(G2D_RSP_TYPE rsp_type);

 /*------------------------------------------------------------------
gre2d_enabe
 graphic engine S/W reset & clock enalbe 
-------------------------------------------------------------------*/
extern void gre2d_enabe(void);

/*------------------------------------------------------------------
gre2d_disable
 graphic engine  clock disable 
-------------------------------------------------------------------*/
extern void gre2d_disable(void);

/*------------------------------------------------------------------
gre2d_interrupt_ctrl
 graphic engine interrupt control 
 wr : 1 : write   0 : read
 int_irq : interrupt request 
 int_flg : flag bit
-------------------------------------------------------------------*/
G2D_INT_TYPE gre2d_interrupt_ctrl(unsigned char wr, G2D_INT_TYPE flag, unsigned char int_irq, unsigned char int_flg);

/*-----------------------------------------------------------------------------
gre2d_ChImgSize
 graphic engine image SIZE converter
 
Graphic engine 의 image SIZE 를 변경 해 준다.
-----------------------------------------------------------------------------*/
extern unsigned char gre2d_ChImgSize(unsigned int src0,unsigned int src1, unsigned int src2,
						unsigned int src_w, unsigned int src_h, unsigned int str_x, unsigned int str_y, G2D_FMT_CTRL src_fmt,
						unsigned int tgt0, unsigned int tgt1, unsigned int tgt2, 
						unsigned int tgt_w, unsigned int tgt_h, G2D_FMT_CTRL fmt);

/*-----------------------------------------------------------------------------
gre2d_ChImgFmt
 graphic engine image format converter
 
Graphic engine 의 image format를 변경 해 준다.
------------------------------------------------------------------------------*/
extern unsigned char gre2d_ChImgFmt(unsigned int src0, unsigned int src1, unsigned int src2, G2D_FMT_CTRL srcfm,
		        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2, G2D_FMT_CTRL tgtfm,
			unsigned int imgh, unsigned int imgv);

/*------------------------------------------------------------------------------
gre2d_ImgArith
 graphic engine image arithmetic operation
 
Graphic engine 의 image arithmetic 연산 해 준다.
RGB888 type 로 연산됨
------------------------------------------------------------------------------*/
extern unsigned char gre2d_ImgArithmetic(unsigned int src0, unsigned int src1, unsigned int src2, 
								G2D_FMT_CTRL srcfm, unsigned int  src_w, unsigned int  src_h ,
						        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2, G2D_FMT_CTRL tgtfm,
								unsigned int  dest_w, unsigned int  dest_h , unsigned int dest_off_x, unsigned int  dest_off_y, 
								G2D_ARITH_TYPE arith, unsigned char R, unsigned char G, unsigned char B);



/*------------------------------------------------------------------------------
gre2d_ImgOverlay
 graphic engine overlay function
 
Graphic engine 의 2 channel alpha-blending 및 chroam-key 기능을 수행 한다.

alpha_en 이 disable 일때는 2 channel를 add 연산 한다. 
------------------------------------------------------------------------------*/
extern unsigned char gre2d_ImgOverlay(G2D_OVERY_FUNC *overlay);



/*------------------------------------------------------------------------------
gre2d_ImgROP
 graphic engine ROP(Raster operation) function
 
Graphic engine 의 1~3개의 Input image의 image size, offset, window offset 및
rotate, flip, format change 등의 처리 후에 Rop 연산 기능을 수행함

RGB888 type 로 연산됨
------------------------------------------------------------------------------*/
extern unsigned char gre2d_ImgROP(G2D_ROP_FUNC rop , G2D_EN en_channel);



/*-----------------------------------------------------------------------------
gre2d_ImgRotate
 graphic engine image rotate function
 
Graphic engine 의 image rotate 및 Flip 기능을 수행 한다.
-----------------------------------------------------------------------------*/
extern unsigned char gre2d_ImgRotate(unsigned int src0, unsigned int src1, unsigned int src2,
                        G2D_FMT_CTRL srcfm, unsigned int  src_imgx, unsigned int  src_imgy,
		        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2,
		        G2D_FMT_CTRL tgtfm, unsigned int  des_imgx, unsigned int  des_imgy, G2D_OP_MODE ch_mode);


/*-----------------------------------------------------------------------------
gre2d_ImgRotate_Ex
 graphic engine image rotate function
 
Graphic engine 의 image rotate 및 Flip 기능을 수행 한다.
extension 내용 : source offset 및 destination offset 추가 
-----------------------------------------------------------------------------*/
extern unsigned char gre2d_ImgRotate_Ex(unsigned int src0, unsigned int src1, unsigned int src2,
					                        G2D_FMT_CTRL srcfm, unsigned int  src_imgx, unsigned int  src_imgy,
					                        unsigned int img_off_x, unsigned int img_off_y, unsigned int Rimg_x, unsigned int Rimg_y,
							        unsigned int tgt0, unsigned int tgt1, unsigned int tgt2,
							        G2D_FMT_CTRL tgtfm, unsigned int  des_imgx, unsigned int  des_imgy, 
							        unsigned int dest_off_x, unsigned int dest_off_y, G2D_OP_MODE ch_mode, G2D_OP_MODE parallel_ch_mode);



extern void gre2d_waiting_result(G2D_EN grp_enalbe);


/* ------------------ SOFTWARE APPLICATION  FUNCTION  ----------------------- */


/*------------------------------------------------------------------------------
gre2d_RGBxxx2RGB888
 
Graphic engine에서 사용할 RGB888 변환을 해준다. 
ZF 의 값을 참조 해서 하위 비트를 채운다.
------------------------------------------------------------------------------*/
//extern unsigned char  gre2d_RGBxxx2RGB888(G2D_DATA_FM form, unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char *r_Red, unsigned char *r_Green, unsigned char *r_Blue);


/*------------------------------------------------------------------------------
gre2d_YUVtoRGB888
 
Graphic engine에서 사용할 YUV to RGB888로 변환을 해준다. 
gre2d_Y2R_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
extern void gre2d_YUVtoRGB888(unsigned char Y, unsigned char U, unsigned char V, unsigned char *R, unsigned char *G, unsigned char *B);


/*------------------------------------------------------------------------------
gre2d_RGB888toYUV_y
 
Graphic engine에서 사용할 RGB888 to YUV의 Y 값을 계산 한다. 변환을 해준다. 
gre2d_R2Y_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
extern unsigned int  gre2d_RGB888toYUV_y(  unsigned int  rgb );


/*------------------------------------------------------------------------------
gre2d_RGB888toYUV_cb
 
Graphic engine에서 사용할 RGB888 to YUV의 U 값을 계산 한다. 변환을 해준다. 
gre2d_R2Y_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
extern unsigned int  gre2d_RGB888toYUV_cb(  unsigned int rgb );

/*------------------------------------------------------------------------------
gre2d_RGB888toYUV_cr
 
Graphic engine에서 사용할 RGB888 to YUV의 V 값을 계산 한다. 변환을 해준다. 
gre2d_R2Y_type 의 값을 참조 변환 식이 선택된다.
------------------------------------------------------------------------------*/
extern unsigned int  gre2d_RGB888toYUV_cr(  unsigned int  rgb );

/*------------------------------------------------------------------------------
gre2d_RGBxxx2RGB888
 
Graphic engine에서 사용할 RGB888 변환을 해준다. 
ZF 의 값을 참조 해서 하위 비트를 채운다.
------------------------------------------------------------------------------*/
extern unsigned char  gre2d_RGBxxx2RGB888(G2D_DATA_FM form, unsigned char Red, unsigned char Green, unsigned char Blue,
                        								unsigned char *r_Red, unsigned char *r_Green, unsigned char *r_Blue);

#endif// __GRE2D_API_H__
