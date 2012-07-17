/*************************************************************************************************/
/* AIT ISP Host API                                                                              */
/* All rights reserved by Alpah Image Technology Corp                                            */
/*-----------------------------------------------------------------------------------------------*/
/* File name: Venus_project.h                                                                    */
/* Description: ISP Host API project related definition                                          */
/*                                                                                               */
/* Version 0.0A   20090326                                                                       */
/*************************************************************************************************/

#ifndef _VENUS_PROJECT_H_
#define _VENUS_PROJECT_H_


#define VA_PROJECT_AIT705DEMOKIT		0
#define VA_PROJECT_SPEACY				1

#define AIT818_TEST_API					0
#define AIT828_TEST_API					0
#define AIT838_TEST_API					0
#define AIT848_TEST_API					1

/*************************************************************************************************/
/* Venus : Headers                                                                               */
/*************************************************************************************************/
#if VA_PROJECT_AIT705DEMOKIT

	//#include <stdio.h>
	#include "ait_utility.h"
	#include <time.h>

#elif VA_PROJECT_SPEACY

	


#endif


/*************************************************************************************************/
/* Venus : System Clock                                                                          */
/*************************************************************************************************/
#if VA_PROJECT_AIT705DEMOKIT

	#define VA_PLL_M				40
	#define VA_PLL_N				4

#elif VA_PROJECT_SPEACY


#endif

/*************************************************************************************************/
/* Venus : Typedefinition                                                                        */
/*************************************************************************************************/
#if VA_PROJECT_AIT705DEMOKIT

	typedef unsigned char			uint8;
	typedef unsigned short			uint16;
	typedef unsigned int			uint32;
	typedef char					int8;
	typedef short					int16;
	typedef int						int32;

#elif VA_PROJECT_SPEACY
typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef char                    int8;
typedef short                   int16;
typedef int                     int32;


#endif

/*************************************************************************************************/
/* Venus : Message                                                                               */
/*************************************************************************************************/
#if VA_PROJECT_AIT705DEMOKIT

	#define VA_MSG(...)				printf(""          ## __VA_ARGS__)
	#define VA_INFO(...)			printf("  INFO: "  ## __VA_ARGS__)
	#define VA_ERR(...)				printf("  ERROR: " ## __VA_ARGS__)
	#define VA_HIGH(...)			printf("-##- "     ## __VA_ARGS__)

#elif VA_PROJECT_SPEACY

	#define MM_CAMERA				(0)
	#define VA_MSG			
	#define VA_INFO			
	#define VA_ERR			
	#define VA_HIGH			


#endif



#endif // _VENUS_PROJECT_H_
