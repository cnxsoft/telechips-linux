/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*
*  This material is confidential and shall remain as such.
*  Any unauthorized use, distribution, reproduction is strictly prohibited.
*
*******************************************************************************
*
*  File Name   : Vioc_api.h
*
*  Description : VIOC user application interface Module header file
*
*******************************************************************************
*
*  yyyy/mm/dd     ver            descriptions                Author
*	---------	--------   ---------------       -----------------
*    2011/08/08     0.1            created                      hskim
*******************************************************************************/

#ifndef __VIOC_API_H__
#define	__VIOC_API_H__

#include  <mach/reg_physical.h>

/* Scaler Related*/

typedef enum
{
	VIOC_SCALER0 = 0,
	VIOC_SCALER1, 
	VIOC_SCALER2, 
	VIOC_SCALER3,
	VIOC_SCALER_MAX
} VIOC_SCALER_Type;

typedef struct
{
	unsigned short SRC_WIDTH; 	 	/* Scaler Input Source Width*/
	unsigned short SRC_HEIGHT; 		/* Scaler Input Source Height*/
	unsigned short DST_WIDTH; 		/* Scaler Output Image Dest Width in Output Window(Scaler out image real size) */
	unsigned short DST_HEIGHT; 		/* Scaler Output Image Dest Width in Output Window(Scaler out image real size) */
	unsigned short OUTPUT_POS_X; 	/* Scaler Output Image Positon x in output Window */
	unsigned short OUTPUT_POS_Y; 	/* Scaler Output Image Positon y in output Window*/
	unsigned short OUTPUT_WIDTH; 	/* Scaler Output Window Size*/
	unsigned short OUTPUT_HEIGHT; 	/* Scaler Output Window Size*/
	unsigned short BYPASS;  		/*Scaler Bypass Option*/
} VIOC_SCALER_INFO_Type;


/* 
	Interface APIs 
*/

/* Scaler Component */
extern void VIOC_API_SCALER_SetConfig(unsigned int scaler, VIOC_SCALER_INFO_Type * Info);
extern void VIOC_API_SCALER_SetUpdate(unsigned int scaler);
extern int VIOC_API_SCALER_SetInterruptEnable(unsigned int scaler, unsigned int interrupt);
extern int VIOC_API_SCALER_SetInterruptDiable(unsigned int scaler, unsigned int interrupt);
extern int VIOC_API_SCALER_SetPlugIn(unsigned int scaler, unsigned int path);
extern int VIOC_API_SCALER_SetPlugOut(unsigned int scaler);
extern void VIOC_API_IREQ_Init(void);

#endif



