/************************************************************************
*    Telechips Multi Media Player
*    ------------------------------------------------
*
*    FUNCTION    : CAMERA INTERFACE API
*    MODEL        : DMP
*    CPU NAME    : TCC92XX
*    SOURCE        : CE14X_isp.c
*
*    START DATE    : 2009. 8. 14.
*    MODIFY DATE :
*    DEVISION    : DEPT. SYSTEM BT 3 TEAM
*                : TELECHIPS, INC.
* 
************************************************************************/


#if defined(CONFIG_ARCH_TCC92XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif
#include "tcc_cam_i2c.h"
#include <asm/delay.h>
#include <linux/delay.h>
#include "sensor_if.h"
#include "cam.h"

static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "venus_5mp_isp: " msg); }

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP
#include "venus_api.h"
#include "venusv3_api.h"
#include "venus_hostcmd.h"
#include "venusv3_api.h"

/* Array of image sizes supported by MV9317.  These must be ordered from 
 * smallest image size to largest.
 */
struct capture_size sensor_sizes[] = {
	{ 2560, 1920 }, // CAM_5M	
	{ 2048, 1536 },	//CAM_QXGA
	{ 1600, 1200 },	//CAM_UXGA
	{ 1280,  960 },	//CAM_SXGA
	{ 1024,  768 },	//CAM_XGA
	{  800,  600 },	//CAM_SVGA
	{  800,  480 },	//CAM_Wallpaper
	{  640,  480 },	//CAM_VGA
//	{  480,  480 },	//CAM_Contacts
	{  320,  240 },	//CAM_QVGA	
};

#ifdef CONFIG_MACH_TCC9200S_SPACEY
#define SENSOR_CORE_PWR_ON()			sensor_pwr_control(CAM_CORE, 1);			
#define SENSOR_CORE_PWR_OFF()			sensor_pwr_control(CAM_CORE, 0);

//For VGA_ Camera
#define VGA_SENSOR_PWRDN_ENABLE() 		(HwGPIOA->GPEN |= Hw13);	(HwGPIOA->GPDAT |= Hw13);
#define VGA_SENSOR_PWRDN_DISABLE() 		(HwGPIOA->GPEN |= Hw13);	(HwGPIOA->GPDAT &= ~Hw13);
#define VGA_SENSOR_RESET_HIGH()			(HwGPIOE->GPEN |= Hw0);		(HwGPIOE->GPDAT |= Hw0);
#define VGA_SENSOR_RESET_LOW()			(HwGPIOE->GPEN |= Hw0);		(HwGPIOE->GPDAT &= ~Hw0);

// Added ISP Ctrl Register for Rev.A Board, because Rev.A B/D is ISP and VGA Cam with together. 
#define ISP_SENSOR_PWRDN_ENABLE()		(HwGPIOE->GPEN |= Hw4);		(HwGPIOE->GPDAT |= Hw4);  // ISP Core Power Enable
#define ISP_SENSOR_PWRDN_DISABLE()		(HwGPIOE->GPEN |= Hw4);		(HwGPIOE->GPDAT &= ~Hw4); //
#define ISP_SENSOR_RESET_HIGH()			(HwGPIOE->GPEN |= Hw3);		(HwGPIOE->GPDAT |= Hw3);
#define ISP_SENSOR_RESET_LOW()			(HwGPIOE->GPEN |= Hw3);		(HwGPIOE->GPDAT &= ~Hw3);

//put the power using pmic.
#define SENSOR_RAM_PWR_ON()		 		sensor_pwr_control(CAM_ETC, 1); //RAM
#define SENSOR_RAM_PWR_OFF()		 	sensor_pwr_control(CAM_ETC, 0); //RAM
#define SENSOR_IO_PWR_ON()			 	sensor_pwr_control(CAM_IO, 1);
#define SENSOR_IO_PWR_OFF()			 	sensor_pwr_control(CAM_IO, 0);

#else
#define SENSOR_CORE_PWR_ON()			(HwGPIOD->GPEN |= Hw10);	(HwGPIOD->GPDAT |= Hw10);
#define SENSOR_CORE_PWR_OFF()			(HwGPIOD->GPEN |= Hw10);	(HwGPIOD->GPDAT &= ~Hw10);

//For VGA_ Camera
#define VGA_SENSOR_PWRDN_ENABLE() 		
#define VGA_SENSOR_PWRDN_DISABLE() 		
#define VGA_SENSOR_RESET_HIGH()			
#define VGA_SENSOR_RESET_LOW()			

// Added ISP Ctrl Register for Rev.A Board, because Rev.A B/D is ISP and VGA Cam with together. 
#define ISP_SENSOR_PWRDN_ENABLE()		(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT |= Hw21);  
#define ISP_SENSOR_PWRDN_DISABLE()		(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT &= ~Hw21); 
#define ISP_SENSOR_RESET_HIGH()			(HwGPIOE->GPEN |= Hw2);		(HwGPIOE->GPDAT |= Hw2);
#define ISP_SENSOR_RESET_LOW()			(HwGPIOE->GPEN |= Hw2);		(HwGPIOE->GPDAT &= ~Hw2);

//put the power using gpio.
#define SENSOR_RAM_PWR_ON()				(HwGPIOD->GPEN |= Hw20);	(HwGPIOD->GPDAT |= Hw20);
#define SENSOR_RAM_PWR_OFF()			(HwGPIOD->GPEN |= Hw20);	(HwGPIOD->GPDAT &= ~Hw20);
#define SENSOR_IO_PWR_ON()				(HwGPIOD->GPEN |= Hw24);	(HwGPIOD->GPDAT |= Hw24);
#define SENSOR_IO_PWR_OFF()				(HwGPIOD->GPEN |= Hw24);	(HwGPIOD->GPDAT &= ~Hw24);
#endif

#define SENSOR_5M_AIT848_ISP_I2C_ADDR 	(0x78)
#define WRITE_RETRY_COUNT 3 //  Write 실패시 재시도 횟수 

unsigned int gnSizeOfAITALLJPEG=0;
unsigned int gnSizeOfAITMainJPEG =0;
unsigned int gnSizeOfAITThumbnail=0;
unsigned int gnoffsetOfAITThumbnail=0;
unsigned short gnWidthOfAITMainJPEG=0;
unsigned short gnHeightOfAITMainJPEG=0;


/**********************************************************
*    Function  
**********************************************************/
void sensor_delay(int ms)
{
#if 1 //ZzaU
	unsigned int msec;

	msec = ms / 10; //10msec unit

	if(!msec)
		msleep(1);
	else
		msleep(msec);
#else
	int totCnt, iCnt, temp = 0;

	totCnt = ms*20000;
	for(iCnt = 0; iCnt < totCnt; iCnt++)
	{
		temp++;
	}
#endif
}

int sensor_venus_chip_enable(void)
{
#ifndef CONFIG_MACH_TCC9200S_SPACEY
	SENSOR_RAM_PWR_OFF();
	SENSOR_IO_PWR_OFF();	sensor_delay(50);
#endif

	// VGA CAM Power Down!!
	VGA_SENSOR_RESET_LOW();		sensor_delay(50);
	VGA_SENSOR_PWRDN_DISABLE();	sensor_delay(50);

	//ISP
	ISP_SENSOR_PWRDN_DISABLE();	sensor_delay(50);
	ISP_SENSOR_RESET_LOW();		sensor_delay(50);

	//put the power using pmic.
	SENSOR_CORE_PWR_ON(); 			sensor_delay(10);		
	SENSOR_RAM_PWR_ON();	sensor_delay(10);
	SENSOR_IO_PWR_ON();		sensor_delay(50);
	
	CIF_Open();					sensor_delay(50);
	
	ISP_SENSOR_PWRDN_ENABLE();

	if(!sensor_if_get_enable())
	{
		sensor_if_set_enable();	sensor_delay(50);
	}

	//sensor_delay(1);
	//SENSOR_RESET_HIGH();

	dprintk("sensor_venus_chip_enable \n");
	
	return 0;//sensor_8M_ce14x_isp_cmd(CAM_RESOLUTION_MODE_XGA, 0);
}


int sensor_venus_chip_reset(void)
{
		
	ISP_SENSOR_RESET_LOW();	sensor_delay(50);		
	ISP_SENSOR_RESET_HIGH();
	
	dprintk("sensor_venus_chip_reset \n");
	
	return 0;//sensor_8M_ce14x_isp_cmd(CAM_RESOLUTION_MODE_XGA, 0);
	
}

extern short RegisterAccessTest();
static int sensor_open(void)
{
	dprintk("sensor_open In \n");

	if(0)
	{
		RegisterAccessTest();
		AccessTest();
	}
	else
	{
		//VA_HIGH("VenusV3_Initialization : start\n");

		// load firmware binary
		VenusV3_ReadFirmwareFile();

		// power on and check firmware version
		V3A_CheckFirmwareVersion();

		// if the firmware is not updated, do nothing
		V3A_PowerUp();

		// free firmware memory
		VenusV3_FreeFirmwareMemory();

		//VA_HIGH("VenusV3_Initialization : end\n");	
	}

	dprintk("sensor_open Out \n");
		
	return 0;//sensor_8M_ce14x_isp_cmd(CAM_RESOLUTION_MODE_XGA, 0);
}


static int sensor_close(void)
{
	dprintk("sensor_close In \n");

	CIF_ONOFF(OFF);

	ISP_SENSOR_RESET_LOW();		sensor_delay(1);
	ISP_SENSOR_PWRDN_DISABLE();	sensor_delay(1);

	CIF_Close();				sensor_delay(1);

	SENSOR_IO_PWR_OFF();
	SENSOR_RAM_PWR_OFF();
	SENSOR_CORE_PWR_OFF();

	return 0;
}


static int sensor_preview(void)
{
	uint8 chValOfReturn = 0;

	dprintk("sensor_preview In \n");
	

//	VA_SetISO(AE_ISO_AUTO);
//	VA_SetAEWnd(VENUS_AE_WINTYPE_AVERAGE);
//	VA_SetAETarget(4);
//	VA_SetSaturation(2);
//	VA_SetSharpness(2);
//	VA_SetContrast(2);
//	VA_SetAWB(AWB_MODE_AUTO);
	VA_SetPreviewFPS(VENUS_AE_FPS_30);
	VA_SetImgEffect(VENUS_IMAGE_EFFECT_NORMAL);
	VA_SetFlickerMode(VENUS_FLICKER_MODE_60HZ);
	VA_SetISC(VENUS_ISC_MODE_OFF);
	VA_SetPreviewResolution(1280, 720); // 0x0280, 0x01E0
//	VA_SetPreviewResolution(0x8002, 0xE001);
//	VA_SetSensorResolutionAsPreview(SENSOR_RESOL_SXGA_1280x960);
	VA_SetPreviewFormat(VENUS_PREVIEW_FORMAT_YUV422);
	VA_InitializeSensor();
	
	VA_SetDumpDataSpeedViaCCIR(2);//PREVIEW_DUMP_SPEED);
	// Enable the Preview, Host shall receive preview data via CCIR interface
	VA_SetPreviewControl(VA_PREVIEW_STATUS_START);
	VA_SetISPSceneMode(VENUS_SCENE_MODE_NONE);
	
	VA_SetZoom(100);

	dprintk("sensor_preview Out \n");

	return 0;
}

static int sensor_capture(void)
{
	dprintk("sensor_capture In \n");

	//VA_DumpDataViaCCIR(0x136f900,gnSizeOfAITJPEG); //VA_GetSnapshotThumbJPEGStartAddress()
	VA_DumpDataViaCCIR(VA_GetSnapshotMainJPEGStartAddress(), gnSizeOfAITALLJPEG); //VA_GetSnapshotMainJPEGStartAddress() 0x10CF900
	//	sensor_preview();

	return 0;
}

static int sensor_capturecfg(int width, int height)
{
	uint8   ret = 0;

	dprintk("sensor_capturecfg In \n");
	
	gnWidthOfAITMainJPEG =  width;
	gnHeightOfAITMainJPEG = height;

	VA_SetSnapshotMode(VENUS_SNAPSHOT_MODE_JPEG);		
	VA_SetSnapshotThumbFormat(VENUS_SNAPSHOT_FORMAT_JPEG);

	VA_SetSnapshotMainResolution(gnWidthOfAITMainJPEG,gnHeightOfAITMainJPEG); //VenusConfig[VENUS_MODE_CAMERA].Main_Width, VenusConfig[VENUS_MODE_CAMERA].Main_Height);
	VA_SetSnapshotThumbnailResolution(320,240); //VenusConfig[VENUS_MODE_CAMERA].Thumb_Width, VenusConfig[VENUS_MODE_CAMERA].Thumb_Height);

 	VA_SetJPEGQuality(VENUS_SNAPSHOT_QUALITY_SUPERFINE);
	
	ret = VA_TriggerSnapshot();
    if(ret == 1) 
	{
        VA_MSG("Ln:%d VA_TriggerSnapshot timeout!!\n", __LINE__);
		return FALSE;
    }
	
  	VA_MSG("  Main(%d),Thumb(%d)\n", VA_GetSnapshotMainJPEGCompressedSize(),VA_GetSnapshotThumbJPEGCompressedSize());

	gnSizeOfAITMainJPEG = VA_GetSnapshotMainJPEGCompressedSize();
	gnSizeOfAITThumbnail = VA_GetSnapshotThumbJPEGCompressedSize();

	gnoffsetOfAITThumbnail = 0x2A0000;

	gnSizeOfAITALLJPEG = 0x2EE000;//0x2A0000 + VA_GetSnapshotThumbJPEGCompressedSize(); //0x2EE000;//

	//gnAlignSizeOfAITALLJPEG = ((gnSizeOfAITALLJPEG+1535)/1536)*1536;

	return 0;
}

static int sensor_zoom(int val)
{
	uint16 nZoomOfAIT = 0;
	dprintk("sensor_zoom In \n");

	nZoomOfAIT = 100 + val*10;

	if(nZoomOfAIT > 400) // Maximum Zoom Value is 400!!
		nZoomOfAIT = 400;

	VA_SetZoom(nZoomOfAIT);
	
	return 0;
}

static int sensor_autofocus(int val)
{
	dprintk("sensor_autofocus In \n");

	return 0;
}

static int sensor_effect(int val)
{
	int value;
	dprintk("sensor_effect In %d \n", val);

	switch(val)
	{
		case 1/*TCC_EFFECT_GRAY*/:			value = VENUS_IMAGE_EFFECT_GREY;	break;
		case 2/*TCC_EFFECT_NEGATIVE*/:		value = VENUS_IMAGE_EFFECT_NEGATIVE;break;
		case 3/*TCC_EFFECT_SEPHIA*/:		value = VENUS_IMAGE_EFFECT_SEPIA;	break;
		case 4/*TCC_EFFECT_AQUA*/:			value = VENUS_IMAGE_EFFECT_BLUE;	break;
		case 5/*TCC_EFFECT_GREEN*/: 		value = VENUS_IMAGE_EFFECT_WATERCOLOR; break;
		default:							value = VENUS_IMAGE_EFFECT_NORMAL;	break;
	}

	VA_SetImgEffect(value);
	
	return 0;
}


static int sensor_flip(int val)
{
	dprintk("sensor_flip In \n");

	VA_SetSensorMirrorFlip(val);

	return 0;
}

static int sensor_iso(int val)
{
	dprintk("sensor_iso In \n");

	VA_SetISO(val);

	return 0;
}

static int sensor_me(int val)
{
	dprintk("sensor_me In \n");

	VA_SetAEWnd(val);

	return 0;
}

static int sensor_wb(int val)
{
	uint8 WBOfAIT = 0;
	WBOfAIT = val + 1;

	dprintk("sensor_wb In \n");

	VA_SetAWB(WBOfAIT);

	return 0;
}

static int sensor_bright(int val)
{
	dprintk("sensor_bright In \n");

	VA_SetAETarget(val);

	return 0;
}

static int sensor_scene(int val)
{
	dprintk("sensor_scene In \n");

	VA_SetISPSceneMode(val);

	return 0;
}

static int sensor_check_esd(int val)
{
	return 0;
}

static int sensor_check_luma(int val)
{
	return 0;
}

/**********************************************************
*	 Function  
**********************************************************/
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open			= sensor_open;
	sensor_func->Close			= sensor_close;

	sensor_func->Set_Preview	= sensor_preview;
	sensor_func->Set_Capture	= sensor_capture;
	sensor_func->Set_CaptureCfg = sensor_capturecfg;

	sensor_func->Set_Zoom		= sensor_zoom;
	sensor_func->Set_AF 		= sensor_autofocus;
	sensor_func->Set_Effect 	= sensor_effect;
	sensor_func->Set_Flip		= sensor_flip;
	sensor_func->Set_ISO		= sensor_iso;
	sensor_func->Set_ME 		= sensor_me;
	sensor_func->Set_WB 		= sensor_wb;
	sensor_func->Set_Bright 	= sensor_bright;
	sensor_func->Set_Scene		= sensor_scene;

	sensor_func->Check_ESD		= sensor_check_esd;
	sensor_func->Check_Luma 	= sensor_check_luma;
}
#endif 
