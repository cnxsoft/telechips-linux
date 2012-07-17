#include "isp_interface.h"
//#include "TCC_ISP_API.h"
#include "simdef.h"
#include "tcc9300_dev.h"
#include "tcc93x.h"
#include "io_address.h" // for debugging
//#include "t3a.h" // for dump

#define PREVIEW_BUF 0x40200000
#define THUMB_BUF   0x40200000
#define CAPTURE_BUF ISP_BUF_ADDR_MP_Y
#define JPEG_BUF 	0x43300000

#define DISPLAY_BUF 0x40200000

#define SENSOR_W 800
#define SENSOR_H 600

#define SENSOR2_W 1600
#define SENSOR2_H 1200


#define PREV_W	640
#define PREV_H	480
//#define PREVH_W	1920
//#define PREVH_H	1080
//#define PREVH_W	1600
//#define PREVH_H	1080
//#define PREVH_W	640
//#define PREVH_H	480

//#define PREVH_W	1648

//#define PREVH_W	1640
//#define PREVH_H	1230
 
//#define PREVH_W	1600
//#define PREVH_H	1200

//#define PREVH_W	1152
//#define PREVH_H	864

//#define PREVH_H		2462

#ifdef PREV8M
	#define PREVH_W		3280	// 3280
	#define PREVH_H		2462	// 2464
#else
	#define PREVH_W	1632	// 1640
	#define PREVH_H	1224	// 1232
#endif



#define PREV_NBUF	2

#define CAP_W		3280
#define CAP_H		2462
#define CAP_NBUF	2

extern unsigned g_hisp;

///////////////////////extern variables for debug /////////////////////////
extern unsigned short avg_bufRgain;
extern unsigned short avg_bufBgain;
extern unsigned int	fout_hitCnt[8];
extern signed short globalLV;
extern signed short DiffLum;
extern signed short INDEX;
extern signed short AGC_INDEX;
extern unsigned int avg256Y;
extern unsigned int avg256G;
extern unsigned char hout_sortedMap[8];
extern unsigned char hys_LV[3];
extern unsigned char fLSCRatio;
extern unsigned char sLSCRatio;
extern unsigned char fLsLSC;
extern unsigned char sLsLSC;
extern unsigned char fCCMRatio;
extern unsigned char sCCMRatio;
extern unsigned char fLsCCM;
extern unsigned char sLsCCM;
extern signed short CCtbl[9];

///////////////////////extern variables for debug /////////////////////////

#if 0
void test_camera_capture(void)
{
	static oneshot = 1;
	if (oneshot)
	{
		HwCKC->uCLK_TCZ.bReg.DIV = 0;
		HwCKC->uCLK_TCZ.bReg.ENABLE = 0;
		HwCKC->uCLK_TCZ.bReg.DIV = 0;
		HwCKC->uCLK_TCZ.bReg.SEL = PCLK_SRC_XIN;
		HwCKC->uCLK_TCZ.bReg.ENABLE = 1;

		HwCKC->uCLK_TCT.bReg.DIV = 0;
		HwCKC->uCLK_TCT.bReg.ENABLE = 0;
		HwCKC->uCLK_TCT.bReg.DIV = 0;
		HwCKC->uCLK_TCT.bReg.SEL = PCLK_SRC_XIN;
		HwCKC->uCLK_TCT.bReg.ENABLE = 1;

		IO_TMR_SetTIMER(4, (4<<4) | HwTCFG0_CC_ZERO | HwTCFG0_IEN_EN | HwTCFG0_CON_ON, 0xFFFFF, 0);

		SDMMC_DRV_Initialize();

		HwCKC->uCLK_SDMMC0.bReg.DIV = 0;
		HwCKC->uCLK_SDMMC0.bReg.ENABLE = 0;
		HwCKC->uCLK_SDMMC0.bReg.DIV = 0;
		HwCKC->uCLK_SDMMC0.bReg.SEL = PCLK_SRC_XIN;
		HwCKC->uCLK_SDMMC0.bReg.ENABLE = 1;

#ifdef TCC9300S
		    HwGPIO->pGPA.uFNC.uBIT4.P0 = 1;
		    HwGPIO->pGPA.uFNC.uBIT4.P1 = 1;
#else
		    HwGPIO->pGPA.uFNC.uBIT4.P2 = 2;
		    HwGPIO->pGPA.uFNC.uBIT4.P3 = 2;
#endif

		Initialize_FileSystem();
		oneshot = 0;
	}

	//IO_ARM_FlushDCACHE();
	ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_STOP);
	{
		// before_reg0xb0210000 - 0x2b00
		int	iFile, len, ret;

		iFile = TC_Open("before.bin",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, 0xb0210000, 0x2b00);
		ret = TC_Close(iFile);

	}
	{
		// last preview image
		DRVISPImgInfo imginfo;
		int	iFile, len, ret;
#if 1
		drvisp_getimginfo(g_hisp, 0, 0, &imginfo); 
		dbg_printf( "PreviewLastImage: Y  = 0x%08X (0x%08X) \n", imginfo.sp_addr1, imginfo.sp_size1 );
		dbg_printf( "PreviewLastImage: U  = 0x%08X (0x%08X) \n", imginfo.sp_addr2, imginfo.sp_size2 );
		dbg_printf( "PreviewLastImage: V  = 0x%08X (0x%08X) \n", imginfo.sp_addr3, imginfo.sp_size3 );
		
		iFile = TC_Open("previmg.yuv",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, imginfo.sp_addr1, imginfo.sp_size1);
		len = TC_Write(iFile, imginfo.sp_addr2, imginfo.sp_size2);
		len = TC_Write(iFile, imginfo.sp_addr3, imginfo.sp_size3);
		ret = TC_Close(iFile);
#else
		drvisp_getimginfo(g_hisp, 0, 0, &imginfo); 
		dbg_printf( "PreviewLastImage: Y  = 0x%08X (0x%08X) \n", imginfo.mp_addr1, imginfo.mp_size1 );
		dbg_printf( "PreviewLastImage: U  = 0x%08X (0x%08X) \n", imginfo.mp_addr2, imginfo.mp_size2 );
		dbg_printf( "PreviewLastImage: V  = 0x%08X (0x%08X) \n", imginfo.mp_addr3, imginfo.mp_size3 );
		
		iFile = TC_Open("previmg.yuv",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, imginfo.mp_addr1, imginfo.mp_size1);
		len = TC_Write(iFile, imginfo.mp_addr2, imginfo.mp_size2);
		len = TC_Write(iFile, imginfo.mp_addr3, imginfo.mp_size3);
		ret = TC_Close(iFile);

#endif
	}

	memset( ISP_BUF_ADDR_MP_Y, 0, 0x100000 );		// buffer clear temporary
	ISP_Capture_Shooting();	
	memcpy(ISP_BUF_ADDR_JPEG, ISP_BUF_ADDR_MP_Y, 0x300000 );		// buffer clear temporary
	{

		dbg_printf( "MrvJpeWaitForEncodeDone: Register ISP_RIS        = 0x%08X\n", HwMRVISP_RIS );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register MI_RIS         = 0x%08X\n", HwMRVMI_RIS );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_DEBUG      = 0x%08X\n", HwMRVISP_BASE(0x1864) );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_ERROR_RIS  = 0x%08X\n", HwMRVISP_BASE(0x186c) );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_STATUS_RIS = 0x%08X\n", HwMRVISP_BASE(0x1880) );
		HwMRVISP_BASE(0x1874) = 0xffffffff;
		HwMRVISP_BASE(0x1888) = 0xffffffff;
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_ERROR_RIS  = 0x%08X\n", HwMRVISP_BASE(0x186c) );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_STATUS_RIS = 0x%08X\n", HwMRVISP_BASE(0x1880) );
		dbg_printf( "Done:\n");
	}
	
	ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_STOP);
	{
		// last preview image
		DRVISPImgInfo imginfo;
		int	iFile, len, ret;

		drvisp_getimginfo(g_hisp, 0, 0, &imginfo); 
		dbg_printf( "PreviewLastImage: Y  = 0x%08X (0x%08X) \n", imginfo.sp_addr1, imginfo.sp_size1 );
		dbg_printf( "PreviewLastImage: U  = 0x%08X (0x%08X) \n", imginfo.sp_addr2, imginfo.sp_size2 );
		dbg_printf( "PreviewLastImage: V  = 0x%08X (0x%08X) \n", imginfo.sp_addr3, imginfo.sp_size3 );
		
		iFile = TC_Open("capture.yuv",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, imginfo.sp_addr1, imginfo.sp_size1);
		len = TC_Write(iFile, imginfo.sp_addr2, imginfo.sp_size2);
		len = TC_Write(iFile, imginfo.sp_addr3, imginfo.sp_size3);
		ret = TC_Close(iFile);
	}

	//ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_START);
	#if 1
	{
		int	iFile;
		int len;
		int ret;
//		unsigned char  pBuf[1024] = "test file 1234567890 /n";
		ISPJpegInfo jpeginfo;



/*
		iFile = TC_Open("test.txt",0, 0000400, 1);
		if (iFile<0)
		{
			// Error;
			TC_Close(iFile);
		}
		len = TC_Read(iFile, pBuf,1024);
		ret = TC_Close(iFile);
*/
		iFile = TC_Open("test.jpg",0x0302, 0000200, 1);

		if (iFile<0)
		{
			// Error;
			TC_Close(iFile);
		}
		
		ISP_getJpeg_ExifData( &jpeginfo )	;
//		len = TC_Write(iFile, jpeginfo.addr,jpeginfo.size);
		len = TC_Write(iFile, ISP_BUF_ADDR_JPEG,jpeginfo.size);
		ret = TC_Close(iFile);

		//TC_Unmount_All_Drive();	
	}
	{
		int	iFile, len, ret;
		// after_reg0xb0210000 - 0x2b00
		iFile = TC_Open("after.bin",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, 0xb0210000, 0x2b00);
		ret = TC_Close(iFile);
	}		

	#endif 
}
#else


//STEVE
void SeperateRaw(short *pdst, short *psrc, int szx, int szy)

{
	int i,j;

	for (i = 0; i < (szy>>1); i++)
	{
		for (j=0; j < (szx>>1); j++)
		{
			*(pdst+szx*i+j)							= *(psrc+szx*(i<<1)+(j<<1) ); 
			*(pdst+szx*i+j+(szx>>1))				= *(psrc+szx*(i<<1)+(j<<1)+1 ); 
			*(pdst+szx*i+j+(szx*(szy>>1)))			= *(psrc+szx*(i<<1)+(j<<1)+szx ); 
			*(pdst+szx*i+j+(szx*(szy>>1))+(szx>>1))	= *(psrc+szx*(i<<1)+(j<<1)+szx+1 ); 
		}
	}
}

void SeperateRaw2(short *pdst, short *psrc, int szx, int szy)

{
	int i,j;

	for (i = 0; i < (szy>>1); i++)
	{
		for (j=0; j < (szx>>1); j++)
		{
			*(pdst+(szx>>1)*i+j)				= *(psrc+szx*(i<<1)+(j<<1) ); 
			*(pdst+(szx>>1)*i+j+(szx*szy>>2)*1)	= *(psrc+szx*(i<<1)+(j<<1)+1 ); 
			*(pdst+(szx>>1)*i+j+(szx*szy>>2)*2)	= *(psrc+szx*(i<<1)+(j<<1)+szx ); 
			*(pdst+(szx>>1)*i+j+(szx*szy>>2)*3)	= *(psrc+szx*(i<<1)+(j<<1)+szx+1 ); 
		}
	}
}

unsigned SaveImageToPgmFile(char *lpstrFile, char *pRawBuffer, int sizeX, int sizeY)
{

	//FILE *fp = NULL;
	unsigned iFile;
	char str[256];
	int len;
	unsigned ret;

	iFile = TC_Open(lpstrFile,0x0302, 0000200, 1);
	if (iFile<0)  	{	TC_Close(iFile);	return 0;	}  // Error occured;

	len = sprintf (&str[0], "P5 %d %d 65535 ",sizeX, sizeY);

	TC_Write(iFile, &str[0], len);	
	TC_Write(iFile, pRawBuffer, sizeX*sizeY*2);		
	ret = TC_Close(iFile);
	return ret;
}


void test_camera_capture(void)
{
	static oneshot = 1;
	if (oneshot)  // initialize for SDCARD
	{
		HwCKC->uCLK_TCZ.bReg.DIV = 0;
		HwCKC->uCLK_TCZ.bReg.ENABLE = 0;
		HwCKC->uCLK_TCZ.bReg.DIV = 0;
		HwCKC->uCLK_TCZ.bReg.SEL = PCLK_SRC_XIN;
		HwCKC->uCLK_TCZ.bReg.ENABLE = 1;

		HwCKC->uCLK_TCT.bReg.DIV = 0;
		HwCKC->uCLK_TCT.bReg.ENABLE = 0;
		HwCKC->uCLK_TCT.bReg.DIV = 0;
		HwCKC->uCLK_TCT.bReg.SEL = PCLK_SRC_XIN;
		HwCKC->uCLK_TCT.bReg.ENABLE = 1;

		IO_TMR_SetTIMER(4, (4<<4) | HwTCFG0_CC_ZERO | HwTCFG0_IEN_EN | HwTCFG0_CON_ON, 0xFFFFF, 0);

		SDMMC_DRV_Initialize();

		HwCKC->uCLK_SDMMC0.bReg.DIV = 0;
		HwCKC->uCLK_SDMMC0.bReg.ENABLE = 0;
		HwCKC->uCLK_SDMMC0.bReg.DIV = 0;
		HwCKC->uCLK_SDMMC0.bReg.SEL = PCLK_SRC_XIN;
		HwCKC->uCLK_SDMMC0.bReg.ENABLE = 1;

#ifdef TCC9300S
		    HwGPIO->pGPA.uFNC.uBIT4.P0 = 1;
		    HwGPIO->pGPA.uFNC.uBIT4.P1 = 1;
#else
		    HwGPIO->pGPA.uFNC.uBIT4.P2 = 2;
		    HwGPIO->pGPA.uFNC.uBIT4.P3 = 2;
#endif

		Initialize_FileSystem();
		oneshot = 0;
	}

	//IO_ARM_FlushDCACHE();
	if (0)
	{
		ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_STOP); // preview STOP 
		{
			// before_reg0xb0210000 - 0x2b00
			int	iFile, len, ret;

			iFile = TC_Open("before.bin",0x0302, 0000200, 1);  // ISP fregister dump file OPEN
			if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
			len = TC_Write(iFile, 0xb0210000, 0x2b00);
			ret = TC_Close(iFile);

		}
	}
	
	#if 0
	{
		// before_reg0xb0210000 - 0x2b00
		int iFile, len, ret;
		unsigned char str[100]={0,};	
		
		
		iFile = TC_Open("var_dump.txt",0x0302, 0000200, 1);
		if (iFile<0)	{	TC_Close(iFile);		}  // Error occured;
		len=sprintf(str," AE info\n");		TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," globalLV= %d DiffLum= %d\n",globalLV,DiffLum); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," avg256Y= %d avg256G= %d\n",avg256Y,avg256G); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," INDEX= %d AGC_INDEX= %d\n",INDEX,AGC_INDEX); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," hys_LV1= %d hys_LV2= %d hys_LV3= %d\n",hys_LV[0],hys_LV[1],hys_LV[2]);	TC_Write(iFile, str, len); memset( str, 0, 100 );			
		
		len=sprintf(str," \nAWB info\n");	TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," avg_bufRgain= %d avg_bufBgain= %d\n",avg_bufRgain,avg_bufBgain); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," top1= %d top2= %d top3= %d\n",fout_hitCnt[0],fout_hitCnt[1],fout_hitCnt[2]); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," LS1= %d LS2= %d LS3= %d\n",hout_sortedMap[0],hout_sortedMap[1],hout_sortedMap[2]); TC_Write(iFile, str, len); memset( str, 0, 100 );	
		len=sprintf(str," fLsLSC= %d fLSCRatio= %d\n",fLsLSC,fLSCRatio); TC_Write(iFile, str, len); memset( str, 0, 100 );	
		len=sprintf(str," sLsLSC= %d sLSCRatio= %d\n",sLsLSC,sLSCRatio); TC_Write(iFile, str, len); memset( str, 0, 100 );

		len=sprintf(str," fLsCCM= %d fCCMRatio= %d\n",fLsCCM,fCCMRatio); TC_Write(iFile, str, len); memset( str, 0, 100 );	
		len=sprintf(str," sLsCCM= %d sCCMRatio= %d\n",sLsCCM,sCCMRatio); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," coef1= %d coef2= %d coef3= %d\n",CCtbl[0],CCtbl[1],CCtbl[2]); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," coef4= %d coef5= %d coef6= %d\n",CCtbl[3],CCtbl[4],CCtbl[5]); TC_Write(iFile, str, len); memset( str, 0, 100 );
		len=sprintf(str," coef7= %d coef8= %d coef9= %d\n",CCtbl[6],CCtbl[7],CCtbl[8]); TC_Write(iFile, str, len); memset( str, 0, 100 );		

		//len = TC_Write(iFile, str, 1000);
		ret = TC_Close(iFile);

	}
	#endif
	
	
if(0) // STEVE disable : preivew LCD size YUV
	{
		// last preview image
		DRVISPImgInfo imginfo;
		int	iFile, len, ret;
#if 0  // LAST preview Image at self path  640x480 same with LCD image
		drvisp_getimginfo(g_hisp, 0, 0, &imginfo); 
		dbg_printf( "PreviewLastImage: Y  = 0x%08X (0x%08X) \n", imginfo.sp_addr1, imginfo.sp_size1 );
		dbg_printf( "PreviewLastImage: U  = 0x%08X (0x%08X) \n", imginfo.sp_addr2, imginfo.sp_size2 );
		dbg_printf( "PreviewLastImage: V  = 0x%08X (0x%08X) \n", imginfo.sp_addr3, imginfo.sp_size3 );
		
		iFile = TC_Open("previmg.yuv",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, imginfo.sp_addr1, imginfo.sp_size1);
		len = TC_Write(iFile, imginfo.sp_addr2, imginfo.sp_size2);
		len = TC_Write(iFile, imginfo.sp_addr3, imginfo.sp_size3);
		ret = TC_Close(iFile);
#else  // LAST preview Image 2M at Main  path   Main path (Hyvision size) same with LCD image
		drvisp_getimginfo(g_hisp, 0, 0, &imginfo); 
		dbg_printf( "PreviewLastImage: Y  = 0x%08X (0x%08X) \n", imginfo.mp_addr1, imginfo.mp_size1 );
		dbg_printf( "PreviewLastImage: U  = 0x%08X (0x%08X) \n", imginfo.mp_addr2, imginfo.mp_size2 );
		dbg_printf( "PreviewLastImage: V  = 0x%08X (0x%08X) \n", imginfo.mp_addr3, imginfo.mp_size3 );
		
		iFile = TC_Open("previmg.yuv",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, imginfo.mp_addr1, imginfo.mp_size1);
		len = TC_Write(iFile, imginfo.mp_addr2, imginfo.mp_size2);
		len = TC_Write(iFile, imginfo.mp_addr3, imginfo.mp_size3);
		ret = TC_Close(iFile);

#endif
	}

#if  1  // JPEG capture after Mode change (8M)
	memset( ISP_BUF_ADDR_MP_Y, 0, 0x100000 );		// buffer clear temporary
	ISP_Capture_Shooting();	// 1st Image (each image )
	//ISP_Capture_Shooting();	// 2nd Image (each image )
	memcpy(ISP_BUF_ADDR_JPEG, ISP_BUF_ADDR_MP_Y, 0x300000 );		// buffer clear temporary
	{

		dbg_printf( "MrvJpeWaitForEncodeDone: Register ISP_RIS        = 0x%08X\n", HwMRVISP_RIS );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register MI_RIS         = 0x%08X\n", HwMRVMI_RIS );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_DEBUG      = 0x%08X\n", HwMRVISP_BASE(0x1864) );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_ERROR_RIS  = 0x%08X\n", HwMRVISP_BASE(0x186c) );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_STATUS_RIS = 0x%08X\n", HwMRVISP_BASE(0x1880) );
		HwMRVISP_BASE(0x1874) = 0xffffffff;
		HwMRVISP_BASE(0x1888) = 0xffffffff;
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_ERROR_RIS  = 0x%08X\n", HwMRVISP_BASE(0x186c) );
		dbg_printf( "MrvJpeWaitForEncodeDone: Register JPE_STATUS_RIS = 0x%08X\n", HwMRVISP_BASE(0x1880) );
		dbg_printf( "Done:\n");
	}
	
	//ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_START);


	#ifndef CAPRAW  // STEVE enable :  JPEG save to file
	{
		int	iFile;
		int len;
		int ret;
//		unsigned char  pBuf[1024] = "test file 1234567890 /n";
		ISPJpegInfo jpeginfo;



/*
		iFile = TC_Open("test.txt",0, 0000400, 1);
		if (iFile<0)
		{
			// Error;
			TC_Close(iFile);
		}
		len = TC_Read(iFile, pBuf,1024);
		ret = TC_Close(iFile);
*/
		iFile = TC_Open("test.jpg",0x0302, 0000200, 1);

		if (iFile<0)
		{
			// Error;
			TC_Close(iFile);
		}
		
		ISP_getJpeg_ExifData( &jpeginfo )	;
//		len = TC_Write(iFile, jpeginfo.addr,jpeginfo.size);
		len = TC_Write(iFile, ISP_BUF_ADDR_JPEG,jpeginfo.size);
		ret = TC_Close(iFile);

		//TC_Unmount_All_Drive();	
	}

	if (0)
	{ // ISP Register dump on Capture state
		int	iFile, len, ret;
		// after_reg0xb0210000 - 0x2b00
		iFile = TC_Open("after.bin",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, 0xb0210000, 0x2b00);
		ret = TC_Close(iFile);
	}		


	if (0) // STEVE disable : LCD size(640x480) Capture Image(YUV)  
		{ 
		// last preview image
		DRVISPImgInfo imginfo;
		int	iFile, len, ret;

		ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_STOP); // ISP output STOP (no more memory access)

		drvisp_getimginfo(g_hisp, 0, 0, &imginfo); 
		dbg_printf( "CaptureImage: Y  = 0x%08X (0x%08X) \n", imginfo.sp_addr1, imginfo.sp_size1 );
		dbg_printf( "CaptureImage: U  = 0x%08X (0x%08X) \n", imginfo.sp_addr2, imginfo.sp_size2 );
		dbg_printf( "CaptureImage: V  = 0x%08X (0x%08X) \n", imginfo.sp_addr3, imginfo.sp_size3 );
		
		iFile = TC_Open("capture.yuv",0x0302, 0000200, 1);
		if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
		len = TC_Write(iFile, imginfo.sp_addr1, imginfo.sp_size1);
		len = TC_Write(iFile, imginfo.sp_addr2, imginfo.sp_size2);
		len = TC_Write(iFile, imginfo.sp_addr3, imginfo.sp_size3);
		ret = TC_Close(iFile);

		ISP_Capture_Shooting();	// 1st Image (each image )	
		//ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_START); // ISP OUTPUT START( START MEMORY ACCESS)
	}

	
	#endif 

	#ifdef CAPRAW
	if (1) // STEVE disable : Capture Raw  
		{ 
		// last preview image
		DRVISPImgInfo imginfo;
		int	iFile, len, ret;
		int i, j;

		for (i = 0; i < CAPRAW; i++)
		{
			ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_STOP); // ISP output STOP (no more memory access)
			//for (j=0; j<100000; j++) ;
			//ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_STOP); // ISP output STOP (no more memory access)
		
			drvisp_getimginfo(g_hisp, 0, 0, &imginfo); 
			dbg_printf( "CaptureRaw: Y  = 0x%08X (0x%08X) \n", imginfo.mp_addr1, imginfo.mp_size1 );
			dbg_printf( "CaptureRaw: U  = 0x%08X (0x%08X) \n", imginfo.mp_addr2, imginfo.mp_size2 );
			dbg_printf( "CaptureRaw: V  = 0x%08X (0x%08X) \n", imginfo.mp_addr3, imginfo.mp_size3 );

			/*
			iFile = TC_Open("capture.raw",0x0302, 0000200, 1);
			if (iFile<0)  	{	TC_Close(iFile);		}  // Error occured;
			len = TC_Write(iFile, imginfo.mp_addr1, imginfo.mp_size1);
			ret = TC_Close(iFile);
			*/

		#ifdef SEPRAW
			SeperateRaw(ISP_BUF_ADDR_JPEG, (char *)imginfo.mp_addr1, CAP_W, CAP_H)
			SaveImageToPgmFile("capture.pgm",(char *)ISP_BUF_ADDR_JPEG, CAP_W, CAP_H);
		#else
			SaveImageToPgmFile("capture.pgm",(char *)imginfo.mp_addr1, CAP_W, CAP_H);		
		#endif


			dbg_printf( "Capture complete [%d] \n", i );

			ISP_Capture_Shooting();	// 2nd Image (each image )
			//for (j=0; j<100000; j++) ;
		}
		
	}
	#endif

#endif  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

}

#endif
void test_camera_api(void)
{
#if 1
	
	ISP_Init();

#if 0
#ifdef PREV8M
	ISP_SetPreview_Window(0 , 0, 3280, 2462);	// for preview
#else
	ISP_SetPreview_Window(4 , 3, 1632, 1224);	// STEVE for Hyvsion preview size
#endif
#else
	ISP_SetPreview_Window(0, 0, SENSOR_W, SENSOR_H);
#endif

	//ISP_SetPreview_Window(0 , 0, 3280, 2462);	// for preview
	//ISP_SetPreview_Window(0 , 0, 1640, 1230);	// for preview
	//ISP_SetPreview_Window(0 , 0, 640,480);	// for preview

	#ifndef HV_YUV422SEQ
	ISP_SetPreview_StartAddress((unsigned *)ISP_BUF_ADDR_SP_Y,(unsigned *)( ISP_BUF_ADDR_SP_CB), (unsigned *) (ISP_BUF_ADDR_SP_CR) );
	//ISP_SetPreview_Size(640*480, 640*480/2, 640*480/2);
	ISP_SetPreview_Size(PREV_W*PREV_H*PREV_NBUF, PREV_W*PREV_H/2*PREV_NBUF, PREV_W*PREV_H/2*PREV_NBUF);	// double buffer
	ISP_SetPreview_Resolution( PREV_W, PREV_H );
	ISP_SetPreview_Format(ISP_FORMAT_YUV422);
	#else
	ISP_SetPreview_StartAddress((unsigned *)ISP_BUF_ADDR_SP_Y,(unsigned *)( ISP_BUF_ADDR_SP_CB), (unsigned *) (ISP_BUF_ADDR_SP_CR) );
	//ISP_SetPreview_Size(640*480, 640*480/2, 640*480/2);
	ISP_SetPreview_Size(PREV_W*PREV_H*PREV_NBUF*2, 0, 0);	// double buffer
	ISP_SetPreview_Resolution( PREV_W, PREV_H );
	ISP_SetPreview_Format(ISP_FORMAT_YUV422);
	#endif
	//ISP_SetPreview_Format(ISP_FORMAT_YUV420);
	//#ifndef CAPRAW
	// STEVE Hyvision Enable
	#ifdef HYVISION

	#ifndef HV_YUV422SEQ
	ISP_SetPreviewH_StartAddress((unsigned *)ISP_BUF_ADDR_MP_Y,(unsigned *)( ISP_BUF_ADDR_MP_CB), (unsigned *) (ISP_BUF_ADDR_MP_CR) );
	ISP_SetPreviewH_Size(PREVH_W*PREVH_H*PREV_NBUF, PREVH_W*PREVH_H/2*PREV_NBUF, PREVH_W*PREVH_H/2*PREV_NBUF);	// double buffer
	ISP_SetPreviewH_Resolution( PREVH_W, PREVH_H );
	ISP_SetPreviewH_Format(ISP_FORMAT_YUV422);
	#else
	ISP_SetPreviewH_StartAddress((unsigned *)ISP_BUF_ADDR_MP_Y,(unsigned *)( ISP_BUF_ADDR_MP_CB), (unsigned *) (ISP_BUF_ADDR_MP_CR) );
	ISP_SetPreviewH_Size(PREVH_W*PREVH_H*PREV_NBUF*2,0,0);	// double buffer
	ISP_SetPreviewH_Resolution( PREVH_W, PREVH_H );
	ISP_SetPreviewH_Format(ISP_FORMAT_YUV422);
	#endif

	//ISP_SetPreview_Format(ISP_FORMAT_YUV420);
	#endif		// #ifdef HYVISION

	//#endif		// #ifndef CAPRAW


	//ISP_SetPreview_Overlay(uint8 onoff, FORMAT type, uint32 chroma_value, uint32 sx, uint32 sy, uint32 widht, uint32 height, uint8 ext_type);

	//ISP_SetImg_Effect(IMAGE_EFFECT effectID);
	//ISP_SetSharpness(uint8 level);
	//ISP_SetSaturation(uint8 level);
	//ISP_SetContrast(uint8 level);

	//ISP_SetSensorMirrorFlip(sensor_mirrorflip_type sensor_mirrorflip);
	//ISP_SetZoom(uint16 zoom);  // 입력사이즈에 자동으로 설정

/////////////////////////////////////////////////////////////////
#ifndef  CAPRAW		// jpeg capture

#if 0
	ISP_SetCapture_Window(0 , 0, 3280,2462);	// for capture
#else
	ISP_SetCapture_Window(0 , 0, SENSOR_W,SENSOR_H);	// for capture
#endif

	//ISP_SetWindow(0 , 0, 800,480);	// 1 : 1 
    ISP_SetCapture_MainImageStartAddress((unsigned *)ISP_BUF_ADDR_MP_Y,(unsigned *)( ISP_BUF_ADDR_MP_CB ), (unsigned *) (ISP_BUF_ADDR_MP_CR) );
	ISP_SetCapture_MainImageSize(CAP_W*CAP_H, CAP_W*CAP_H/2, CAP_W*CAP_H/2);
    ISP_SetCapture_ThumbImageStartAddress((unsigned *)ISP_BUF_ADDR_SP_Y,(unsigned *)( ISP_BUF_ADDR_SP_CB), (unsigned *) (ISP_BUF_ADDR_SP_CR) );
//	ISP_SetCapture_ThumbImageSize(PREV_W*PREV_H, PREV_W*PREV_H/2, PREV_W*PREV_H/2);
	ISP_SetCapture_ThumbImageSize(PREV_W*PREV_H*PREV_NBUF, PREV_W*PREV_H/2*PREV_NBUF, PREV_W*PREV_H/2*PREV_NBUF);

	
	ISP_SetCapture_MainResolution( CAP_W, CAP_H );
	ISP_SetCapture_ThumbResolution( PREV_W, PREV_H );
	//ISP_SetCapture_MainFormat(ISP_FORMAT_YUV422);
	ISP_SetCapture_MainFormat(ISP_FORMAT_JPEG);
	ISP_SetCapture_ThumbFormat(ISP_FORMAT_YUV422);


#else		// raw capture
	ISP_SetCapture_Window(0 , 0, 3280,2462);	// for capture
	//ISP_SetWindow(0 , 0, 800,480);	// 1 : 1 
	ISP_SetCapture_MainImageStartAddress((unsigned *)ISP_BUF_ADDR_MP_Y,(unsigned *)( ISP_BUF_ADDR_MP_CB ), (unsigned *) (ISP_BUF_ADDR_MP_CR) );
	ISP_SetCapture_MainImageSize(CAP_W*CAP_H*2*PREV_NBUF,0,0);
	//ISP_SetCapture_ThumbImageStartAddress((unsigned *)ISP_BUF_ADDR_SP_Y,(unsigned *)( ISP_BUF_ADDR_SP_CB), (unsigned *) (ISP_BUF_ADDR_SP_CR) );
	//ISP_SetCapture_ThumbImageSize(PREV_W*PREV_H*PREV_NBUF, PREV_W*PREV_H/2*PREV_NBUF, PREV_W*PREV_H/2*PREV_NBUF);

	
	ISP_SetCapture_MainResolution( CAP_W, CAP_H );
	//ISP_SetCapture_MainFormat(ISP_FORMAT_YUV422);
	//ISP_SetCapture_MainFormat(ISP_FORMAT_JPEG);
	ISP_SetCapture_MainFormat(ISP_FORMAT_BAYER);

	//ISP_SetCapture_ThumbResolution( PREV_W, PREV_H );
	//ISP_SetCapture_ThumbFormat(ISP_FORMAT_YUV422);
#endif

	ISP_SetPreview_Control(TCC_ISP_PREVIEW_STATUS_START);
/////////////////////////////////////////////////////////////////


#else
	
	ISP_Init();
	ISP_SetCapture_Window(0 , 0, 3280,2462);	// for capture
	//ISP_SetWindow(0 , 0, 800,480);	// 1 : 1 
    ISP_SetCapture_MainImageStartAddress((unsigned *)CAPTURE_BUF,(unsigned *)( CAPTURE_BUF+3280*2462 ), (unsigned *) (CAPTURE_BUF+3280*2462+3280*2462/2) );
	ISP_SetCapture_MainImageSize(3280*2462, 3280*2462/2, 3280*2462/2);
    ISP_SetCapture_ThumbImageStartAddress((unsigned *)THUMB_BUF,(unsigned *)( THUMB_BUF+800*480 ), (unsigned *) (THUMB_BUF+800*480+800*480/2) );
	ISP_SetCapture_ThumbImageSize(800*480, 800*480/2, 800*480/2);

	
	ISP_SetCapture_MainResolution( 3280, 2462 );
	ISP_SetCapture_ThumbResolution( 800, 480 );
	ISP_SetCapture_MainFormat(ISP_FORMAT_YUV422);
	ISP_SetCapture_ThumbFormat(ISP_FORMAT_YUV422);

	//ISP_SetPreview_Overlay(uint8 onoff, FORMAT type, uint32 chroma_value, uint32 sx, uint32 sy, uint32 widht, uint32 height, uint8 ext_type);

	//ISP_SetImg_Effect(IMAGE_EFFECT effectID);
	//ISP_SetSharpness(uint8 level);
	//ISP_SetSaturation(uint8 level);
	//ISP_SetContrast(uint8 level);

	//ISP_SetSensorMirrorFlip(sensor_mirrorflip_type sensor_mirrorflip);
	//ISP_SetZoom(uint16 zoom);  // 입력사이즈에 자동으로 설정

	//ISP_SetCapture_MultiShotCount( 0 ) ;
	ISP_Capture_Shooting();	

#endif	
}
