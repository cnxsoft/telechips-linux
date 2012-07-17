/************************************************************************
*    Telechips Graphic Engine header file
*    ------------------------------------------------
*
*    FUNCTION    : Graphic Engine drvier
*    MODEL       :
*    CPU NAME    : TCC83XX
*
*    START DATE    : 2007. 7. 27.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM 3 TEAM
*                : TELECHIPS, INC.
************************************************************************/
#ifndef __GRE2D_H__
#define __GRE2D_H__

#include <mach/tcc_Gre2d_type.h>

#define TCC_OVERLAY_MIXER_CLUT_SUPPORT

 /*------------------------------------------------------------------
Gre2d_Set_interrupt
 graphic engine interrupt enable/ disable
-------------------------------------------------------------------*/
extern void Gre2d_Set_interrupt(char onoff);

 /*------------------------------------------------------------------
Gre2d_enable
 graphic engine S/W reset & clock enalbe 
-------------------------------------------------------------------*/
extern void Gre2d_enable(void);

/*------------------------------------------------------------------
Gre2d_disable
 graphic engine  clock disable 
-------------------------------------------------------------------*/
extern void Gre2d_disable(void);

/*------------------------------------------------------------------
Gre2d_SetFCh_address
 graphic engine Front End channel address 0,1,2 setting
-------------------------------------------------------------------*/
extern void Gre2d_SetFCh_address(G2D_CHANNEL ch, unsigned int add0, unsigned int add1, unsigned int add2);



/*------------------------------------------------------------------
Gre2d_SetFCh_position
 graphic engine Front channel position settig
 
 frameps_x, frameps_y : frame pixsel size
 poffset_x, poffset_y : pixsel offset
 imageps_x, imageps_y : imagme pixel size
 winps_x, winps_y : window pixsel offset
-------------------------------------------------------------------*/
extern void Gre2d_SetFCh_position(G2D_CHANNEL ch, unsigned int frameps_x, unsigned int frameps_y, 
                                unsigned int poffset_x, unsigned int poffset_y, unsigned int imageps_x, unsigned int imageps_y, unsigned int winps_x, unsigned int winps_y );



/*------------------------------------------------------------------
Gre2d_SetFCh_control
 graphic engine Front channel control setting
 mode : flip, rotate
 data_form : rgb, yuv, alpha-rgb
-------------------------------------------------------------------*/
extern void Gre2d_SetFCh_control(G2D_CHANNEL ch, G2D_MABC_TYPE MABC, unsigned char LUTE, unsigned char SSUV, G2D_OP_MODE mode, G2D_ZF_TYPE ZF, G2D_FMT_CTRL data_form);


/*------------------------------------------------------------------
Gre2d_SetFCh_Chroma_key
 graphic engine Front channel chroma key Set
 
-------------------------------------------------------------------*/
extern void Gre2d_SetFCh_Chroma_key(G2D_CHANNEL ch, unsigned char RY, unsigned char GU, unsigned char BV);



/*------------------------------------------------------------------
Gre2d_SetFCh_Arithmetic_par
 graphic engine Front channel Arithmetic parameter setting
-------------------------------------------------------------------*/
extern void Gre2d_SetFCh_Arithmetic_par(G2D_CHANNEL ch, unsigned char RY, unsigned char GU, unsigned char BV);



/*------------------------------------------------------------------
Gre2d_src_ctrl
 graphic engine sorce control
-------------------------------------------------------------------*/
extern void Gre2d_src_ctrl(G2D_SRC_CTRL reg);



/*-------- Source Operator pattern set -------*/


/*------------------------------------------------------------------
Gre2d_operator_set
 graphic engine operator 0, 1 setting 
-------------------------------------------------------------------*/
extern void Gre2d_operator_set(G2D_OP_TYPE op_set, unsigned short alpha , unsigned char RY, unsigned char GU, unsigned char BV);




/*------------------------------------------------------------------
Gre2d_operator_ctrl
 graphic engine operator control register setting
-------------------------------------------------------------------*/



#if defined (CONFIG_ARCH_TCC892X)
extern void Gre2d_operator_ctrl(G2D_OP_TYPE op_set, G2D_OP_ACON ACON1, G2D_OP_ACON ACON0, G2D_OP_CCON CCON1, G2D_OP_CCON CCON0, G2D_OP_ATUNE ATUNE, G2D_OP1_CHROMA CSEL,GE_ROP_TYPE op );

#else
extern void Gre2d_operator_ctrl(G2D_ASEL_TYPE ASEL1, G2D_OP1_CHROMA CSEL1, GE_ROP_TYPE op1, G2D_ASEL_TYPE ASEL0, G2D_OP0_CHROMA CSEL0, GE_ROP_TYPE op0);

#endif



/*-------- BACK END CHANNEL DESTINATION SETTIG. -------*/

/*------------------------------------------------------------------
Gre2d_SetBCh_address
  graphic engine BACK End channel address 0,1,2 setting
-------------------------------------------------------------------*/
extern void Gre2d_SetBCh_address(G2D_CHANNEL ch, unsigned int add0, unsigned int add1, unsigned int add2);



/*------------------------------------------------------------------
Gre2d_SetBCh_position
 graphic engine BACK END channel position settig
 
 frameps_x, frameps_y : frame pixsel size
 poffset_x, poffset_y : pixsel offset
-------------------------------------------------------------------*/
extern void Gre2d_SetBCh_position(G2D_CHANNEL ch, unsigned int frameps_x, unsigned intframeps_y, unsigned int poffset_x, unsigned int poffset_y);



/*------------------------------------------------------------------
Gre2d_SetBCh_control
 graphic engine Back END channel control setting
 mode : flip, rotate
 data_form : rgb, yuv, alpha-rgb
-------------------------------------------------------------------*/
extern void Gre2d_SetBCh_control(G2D_BCH_CTRL_TYPE *reg);

/*------------------------------------------------------------------
Gre2d_SetDitheringMatrix
 graphic engine Dithering Matix Setting
-------------------------------------------------------------------*/
extern void Gre2d_SetDitheringMatrix(unsigned short *Matrix);


/*------------------------------------------------------------------
Gre2d_Grp_enable
 graphic engine channel enable control
-------------------------------------------------------------------*/
extern void Gre2d_Grp_enable(G2D_EN grp_enalbe, unsigned char int_en);


/*------------------------------------------------------------------
Gre2d_Grp_check
 graphic engine transger check
-------------------------------------------------------------------*/
extern unsigned int Gre2d_Grp_check(void);


/*------------------------------------------------------------------
Gre2d_Grp_int_en
 graphic engine interrupt enable
-------------------------------------------------------------------*/
extern void Gre2d_Grp_int_en(unsigned int int_en);




/*------------------------------------------------------------------
Gre2d_Grp_int_ctrl
 graphic engine interrupt control 
 wr : write / read
 int_irq : interrupt request 
 int_flg : flag bit
-------------------------------------------------------------------*/
extern G2D_INT_TYPE Gre2d_Grp_int_ctrl(unsigned char wr, G2D_INT_TYPE flag, unsigned char int_irq, unsigned char int_flg);


extern void Gre2d_Grp_Clut_Ctrl(unsigned int ch, unsigned int index, unsigned int data);

#endif//__GRE2D_H__
