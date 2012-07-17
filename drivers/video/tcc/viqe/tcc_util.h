/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*                                   
*  This material is confidential and shall remain as such. 
*  Any unauthorized use, distribution, reproduction is strictly prohibited. 
*
*   FileName    : TCCFile.h
*   Description : 
*   TCC Version 1.0
*   Copyright (c) Telechips, Inc.
*   ALL RIGHTS RESERVED
*******************************************************************************/
#ifndef	_TCC_UTIL_H__
#define	_TCC_UTIL_H__

/******************************************************************************
* include 
******************************************************************************/
//#include <stdio.h>
//#include <stdlib.h>
//#include <libgen.h>
//#include <sys/vfs.h>
//#include <sys/stat.h>
//#include <sys/types.h>

//#include <unistd.h>

/******************************************************************************
* typedefs & structure
******************************************************************************/
struct f_size{
	long blocks;
	long avail;
};

typedef struct _mountinfo {
	FILE *fp;			// 파일 스트림 포인터
	char devname[80];	// 장치 이름
	char mountdir[80];	// 마운트 디렉토리 이름
	char fstype[12];	// 파일 시스템 타입
	struct f_size size;	// 파일 시스템의 총크기/사용율
} MOUNTP;

/******************************************************************************
* defines 
******************************************************************************/

/******************************************************************************
* declarations
******************************************************************************/
MOUNTP *dfopen();
MOUNTP *dfget(MOUNTP *MP, int deviceType);
int dfclose(MOUNTP *MP);

#endif //_TCC_UTIL_H__
