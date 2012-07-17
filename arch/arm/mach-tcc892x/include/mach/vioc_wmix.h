/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : vioc_wmix.h
*
*  Description : VIOC wmix components control module header file.
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/10     0.1            created                      hskim
*******************************************************************************/

#ifndef __VIOC_WMIX_H__
#define __VIOC_WMIX_H__
#include <mach/reg_physical.h>

typedef	struct
{
	volatile VIOC_WMIX_MACON_u	uACON;				// 0
	volatile VIOC_WMIX_MCCON_u	uCCON;				// 1
	volatile VIOC_WMIX_MROPC_u	uROPC;				// 2
	volatile VIOC_WMIX_MPAT_u 	uPAT;				// 3
} VIOC_WMIX_ALPHA;


/* Interface APIs */
extern void VIOC_WMIX_SetOverlayPriority(VIOC_WMIX *pWMIX, unsigned int nOverlayPriority);
extern void VIOC_WMIX_GetOverlayPriority(VIOC_WMIX *pWMIX, unsigned int *nOverlayPriority);
extern void VIOC_WMIX_SetUpdate(VIOC_WMIX *pWMIX);
#if 0 //[[ hskim_20110920_BEGIN -- Not Support
extern void VIOC_WMIX_SetAlphOpt(VIOC_WMIX *pWMIX, unsigned int opt);
#endif //]] hskim_20110920_END -- Not Support
extern void VIOC_WMIX_SetSize(VIOC_WMIX *pWMIX, unsigned int nWidth, unsigned int nHeight);
extern void VIOC_WMIX_SetBGColor(VIOC_WMIX *pWMIX, unsigned int nBG0, unsigned int nBG1, unsigned int nBG2, unsigned int nBG3);
extern void VIOC_WMIX_SetPosition(VIOC_WMIX *pWMIX, unsigned int nChannel, unsigned int nX, unsigned int nY);
extern void VIOC_WMIX_SetChromaKey(VIOC_WMIX *pWMIX, unsigned int nLayer, unsigned int nKeyEn, unsigned int nKeyR, unsigned int nKeyG, unsigned int nKeyB, unsigned int nKeyMaskR, unsigned int nKeyMaskG, unsigned int nKeyMaskB);
extern void VIOC_WMIX_GetChromaKey(VIOC_WMIX *pWMIX, unsigned int nLayer, unsigned int *nKeyEn, unsigned int *nKeyR, unsigned int *nKeyG, unsigned int *nKeyB, unsigned int *nKeyMaskR, unsigned int *nKeyMaskG, unsigned int *nKeyMaskB);
extern void VIOC_WMIX_ALPHA_SetAlphaValueControl( VIOC_WMIX_ALPHA * pWALPHA, unsigned int region, unsigned int acon0, unsigned int acon1);
extern void VIOC_WMIX_ALPHA_SetColorControl( VIOC_WMIX_ALPHA * pWALPHA, unsigned int region, unsigned int ccon0, unsigned int ccon1);
extern void VIOC_WMIX_ALPHA_SetROPMode( VIOC_WMIX_ALPHA * pWALPHA, unsigned int mode);
extern void VIOC_WMIX_ALPHA_SetAlphaSelection( VIOC_WMIX_ALPHA * pWALPHA, unsigned int asel);
extern void VIOC_WMIX_ALPHA_SetAlphaValue( VIOC_WMIX_ALPHA * pWALPHA, unsigned int alpha0, unsigned int alpha1);
extern void VIOC_WMIX_ALPHA_SetROPPattern( VIOC_WMIX_ALPHA * pWALPHA, unsigned int patR, unsigned int patG, unsigned int patB);
extern void VIOC_WMIX_SetInterruptMask(VIOC_WMIX *pWMIX, unsigned int nMask, unsigned int set);
extern unsigned int VIOC_WMIX_GetStatus(VIOC_WMIX *pWMIX );
extern void VIOC_WMIX_IreqHandler( unsigned int vectorID );




// VIOC API FUNCTION  //

/* 4X2 */
typedef enum
{
	VIOC_WMIX0_ALPHA_L0 =0,
	VIOC_WMIX0_ALPHA_L1,
	VIOC_WMIX0_ALPHA_L2,
	VIOC_WMIX0_ALPHA_LAYER_MAX	
}VIOC_WMIX0_ALPHA_LAYER_Type;

/* 2X2 #1*/
typedef enum
{
	VIOC_WMIX1_ALPHA_L0 =0,
	VIOC_WMIX1_ALPHA_LAYER_MAX	
}VIOC_WMIX1_ALPHA_LAYER_Type;

/* 2X2 #2*/
typedef enum
{
	VIOC_WMIX2_ALPHA_L0 =0,
	VIOC_WMIX2_ALPHA_LAYER_MAX	
}VIOC_WMIX2_ALPHA_LAYER_Type;

typedef enum
{
	VIOC_WMIX_ALPHA_SEL0 = 0, 	/* image alpha 0 ~ 255 ( 0% ~ 99.6% )*/
	VIOC_WMIX_ALPHA_SEL1, 		/*not defined*/
	VIOC_WMIX_ALPHA_SEL2, 		/*image alpha 1 ~ 256 ( 0.39% ~ 100% )*/
	VIOC_WMIX_ALPHA_SEL3, 		/*image alpha 0 ~ 127, 129 ~ 256 ( 0% ~ 49.6%, 50.3% ~ 100% )*/
	VIOC_WMIX_ALPHA_SEL_MAX
}VIOC_WMIX_ALPHA_SELECT_Type;

typedef enum
{
	VIOC_WMIX_ALPHA_ROP_NOTDEFINE = 0, 	/*not defined*/
	VIOC_WMIX_ALPHA_ROP_GLOBAL = 2, 	/*Global Alpha*/
	VIOC_WMIX_ALPHA_ROP_PIXEL, 	/*Pixel Alpha*/
	VIOC_WMIX_ALPHA_ROP_MAX
}VIOC_WMIX_ALPHA_ROPMODE_Type;

typedef enum
{
	VIOC_WMIX_ALPHA_ACON0_0 = 0, 	/* Result_A = ALPHA0 * SEL0_Out, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_1, 		/* Result_A = ALPHA0 * SEL0_Out, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_2, 		/* Result_A = ALPHA0 * 256, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_3, 		/* Result_A = ALPHA0 * 256, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_4, 		/* Result_A = ALPHA0 * 128, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_5, 		/* Result_A = ALPHA0 * 128, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_6, 		/* Result_A = 0, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_7, 		/* Result_A = 0, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_ACON0_MAX		/* Result : AlphaDataOut = (Result_A + Result_B + 128) / 256*/
}VIOC_WMIX_ALPHA_ACON0_Type;

typedef enum
{
	VIOC_WMIX_ALPHA_ACON1_0 = 0, 	/* Result_B = ALPHA1 * SEL1_Out, SEL1_Out = ALPHA0*/            
	VIOC_WMIX_ALPHA_ACON1_1, 		/* Result_B = ALPHA1 * SEL1_Out, SEL1_Out = 256 - ALPHA0*/      
	VIOC_WMIX_ALPHA_ACON1_2, 		/* Result_B = ALPHA1 * 256, SEL1_Out = ALPHA0*/                 
	VIOC_WMIX_ALPHA_ACON1_3, 		/* Result_B = ALPHA1 * 256, SEL1_Out = 256 - ALPHA0*/           
	VIOC_WMIX_ALPHA_ACON1_4, 		/* Result_B = ALPHA1 * 128, SEL1_Out = ALPHA0*/                 
	VIOC_WMIX_ALPHA_ACON1_5, 		/* Result_B = ALPHA1 * 128, SEL1_Out = 256 - ALPHA0*/           
	VIOC_WMIX_ALPHA_ACON1_6, 		/* Result_B = 0, SEL1_Out = ALPHA0*/                            
	VIOC_WMIX_ALPHA_ACON1_7, 		/* Result_B = 0, SEL1_Out = 256 - ALPHA0*/                      
	VIOC_WMIX_ALPHA_ACON1_MAX		/* Result : AlphaDataOut = (Result_A + Result_B + 128) / 256*/  
}VIOC_WMIX_ALPHA_ACON1_Type;

typedef enum
{
	VIOC_WMIX_ALPHA_CCON0_0 = 0, 	/* Result_A = PixelDataA * SEL0_Out, SEL0_Out = ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_1, 		/* Result_A = PixelDataA * SEL0_Out, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_2, 		/* Result_A = PixelDataA * SEL0_Out, SEL0_Out = 256 - ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_3, 		/* Result_A = PixelDataA * SEL0_Out, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_4, 		/* Result_A = PixelDataA * 256, SEL0_Out = ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_5, 		/* Result_A = PixelDataA * 256, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_6, 		/* Result_A = PixelDataA * 256, SEL0_Out = 256 - ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_7, 		/* Result_A = PixelDataA * 256, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_8, 		/* Result_A = PixelDataA * 128, SEL0_Out = ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_9, 		/* Result_A = PixelDataA * 128, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_10, 		/* Result_A = PixelDataA * 128, SEL0_Out = 256 - ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_11, 		/* Result_A = PixelDataA * 128, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_12, 		/* Result_A = 0, SEL0_Out = ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_13, 		/* Result_A = 0, SEL0_Out = ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_14, 		/* Result_A = 0, SEL0_Out = 256 - ALPHA0*/
	VIOC_WMIX_ALPHA_CCON0_15, 		/* Result_A = 0, SEL0_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_CCON0_MAX		/* Result : PixelDataOut = (Result_A + Result_B + 128) / 256*/
}VIOC_WMIX_ALPHA_CCON0_Type;

typedef enum
{
	VIOC_WMIX_ALPHA_CCON1_0 = 0,		/* Result_B = PixelDataB * SEL1_Out, SEL1_Out = ALPHA0*/      
	VIOC_WMIX_ALPHA_CCON1_1, 		/* Result_B = PixelDataB * SEL1_Out, SEL1_Out = ALPHA1*/      
	VIOC_WMIX_ALPHA_CCON1_2, 		/* Result_B = PixelDataB * SEL1_Out, SEL1_Out = 256 - ALPHA0*/
	VIOC_WMIX_ALPHA_CCON1_3, 		/* Result_B = PixelDataB * SEL1_Out, SEL1_Out = 256 - ALPHA1*/
	VIOC_WMIX_ALPHA_CCON1_4, 		/* Result_B = PixelDataB * 256, SEL1_Out = ALPHA0*/           
	VIOC_WMIX_ALPHA_CCON1_5, 		/* Result_B = PixelDataB * 256, SEL1_Out = ALPHA1*/           
	VIOC_WMIX_ALPHA_CCON1_6, 		/* Result_B = PixelDataB * 256, SEL1_Out = 256 - ALPHA0*/     
	VIOC_WMIX_ALPHA_CCON1_7, 		/* Result_B = PixelDataB * 256, SEL1_Out = 256 - ALPHA1*/     
	VIOC_WMIX_ALPHA_CCON1_8, 		/* Result_B = PixelDataB * 128, SEL1_Out = ALPHA0*/           
	VIOC_WMIX_ALPHA_CCON1_9, 		/* Result_B = PixelDataB * 128, SEL1_Out = ALPHA1*/           
	VIOC_WMIX_ALPHA_CCON1_10, 		/* Result_B = PixelDataB * 128, SEL1_Out = 256 - ALPHA0*/     
	VIOC_WMIX_ALPHA_CCON1_11, 		/* Result_B = PixelDataB * 128, SEL1_Out = 256 - ALPHA1*/     
	VIOC_WMIX_ALPHA_CCON1_12, 		/* Result_B = 0, SEL1_Out = ALPHA0*/                          
	VIOC_WMIX_ALPHA_CCON1_13, 		/* Result_B = 0, SEL1_Out = ALPHA1*/                          
	VIOC_WMIX_ALPHA_CCON1_14, 		/* Result_B = 0, SEL1_Out = 256 - ALPHA0*/                    
	VIOC_WMIX_ALPHA_CCON1_15, 		/* Result_B = 0, SEL1_Out = 256 - ALPHA1*/                    
	VIOC_WMIX_ALPHA_CCON1_MAX		/* Result : PixelDataOut = (Result_A + Result_B + 128) / 256*/
}VIOC_WMIX_ALPHA_CCON1_Type;

#define VIOC_WMIX_ALPHA_REGION_A		0x0
#define VIOC_WMIX_ALPHA_REGION_B		0x1
#define VIOC_WMIX_ALPHA_REGION_C		0x2
#define VIOC_WMIX_ALPHA_REGION_D		0x3


extern void VIOC_API_WMIX_SetOverlayAlphaROPMode( VIOC_WMIX *pWMIX, unsigned int layer, unsigned int opmode );
extern void VIOC_API_WMIX_SetOverlayAlphaValue( VIOC_WMIX *pWMIX, unsigned int layer, unsigned int alpha0, unsigned int alpha1 );
extern void VIOC_API_WMIX_SetOverlayAlphaSelection(VIOC_WMIX *pWMIX, unsigned int layer,unsigned int asel );
extern void VIOC_API_WMIX_SetOverlayAlphaValueControl(VIOC_WMIX *pWMIX, unsigned int layer, unsigned int region, unsigned int acon0, unsigned int acon1 );
extern void VIOC_API_WMIX_SetOverlayAlphaColorControl(VIOC_WMIX *pWMIX, unsigned int layer, unsigned int region, unsigned int ccon0, unsigned int ccon1 );
extern void VIOC_WMIX_SetSWReset(unsigned int WMIX, unsigned int RDMA, unsigned int WDMA);
#endif


