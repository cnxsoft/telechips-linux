/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               CAMERA    API    M O D U L E

                        EDIT HISTORY FOR MODULE

when        who       what, where, why
--------    ---       -------------------------------------------------------
10/xx/08  Telechips   Created file.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include <linux/delay.h>
#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include "sensor_if.h"
#include "cam.h"
#include "tcc_cam_i2c.h"

#if defined(CONFIG_ARCH_TCC92XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_OV3640

#if defined(CONFIG_MACH_TCC9200S)
#define CAM_PWR_ENABLE()		(HwGPIOD->GPEN |= Hw10);	(HwGPIOD->GPDAT |= Hw10);
#define CAM_PWR_DISABLE()		(HwGPIOD->GPEN |= Hw10);	(HwGPIOD->GPDAT &= ~Hw10);
#define CAM_PWRDN_ENABLE()		(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT |= Hw21);
#define CAM_PWRDN_DISABLE()		(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT &= ~Hw21);
#define CAM_RESET_HIGH()		(HwGPIOE->GPEN |= Hw2);		(HwGPIOE->GPDAT |= Hw2);
#define CAM_RESET_LOW()			(HwGPIOE->GPEN |= Hw2);		(HwGPIOE->GPDAT &= ~Hw2);
#elif defined(CONFIG_MACH_TCC9200) || defined(CONFIG_MACH_TCC9201) || defined(CONFIG_MACH_TCC8900)
#define CAM_PWR_ENABLE()
#define CAM_PWR_DISABLE()
#define CAM_PWRDN_ENABLE()		//(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT |= Hw21);
#define CAM_PWRDN_DISABLE()		//(HwGPIOF->GPEN |= Hw21);	(HwGPIOF->GPDAT &= ~Hw21);
#define CAM_RESET_HIGH()		(HwGPIOE->GPEN |= Hw26);	(HwGPIOE->GPDAT |= Hw26);
#define CAM_RESET_LOW()			(HwGPIOE->GPEN |= Hw26);	(HwGPIOE->GPDAT &= ~Hw26);
#else
#define CAM_PWR_ENABLE()
#define CAM_PWR_DISABLE()
#define CAM_PWRDN_ENABLE()		(HwGPFEN |= Hw1); 	(HwGPFDAT |= Hw1);
#define CAM_PWRDN_DISABLE()		(HwGPFEN |= Hw1); 	(HwGPFDAT &= ~Hw1);
#define CAM_RESET_HIGH()		(HwGPEEN |= Hw9); 	(HwGPEDAT |= Hw9);
#define CAM_RESET_LOW()			(HwGPEEN |= Hw9); 	(HwGPEDAT &= ~Hw9);
#endif

/* Array of image sizes supported by OV3640.  These must be ordered from 
 * smallest image size to largest.
 */
struct capture_size sensor_sizes[] = {
	{ 2048, 1536 },	/* QXGA */
	{ 1600, 1200 },	/* UXGA */
	{ 1280,  960 },	/* SXGA */
	{ 1024,  768 },	/* XGA */
	{  800,  600 },	/* SVGA */
	{  640,  480 },	/* VGA */
	{  320,  240 },	/* QVGA */
	{  176,  144 },	/* QCIF */
};


/* register initialization tables for sensor */
/* common sensor register initialization for all image sizes, pixel formats, 
 * and frame rates
 */
static struct sensor_reg sensor_initialize[] = {

	/* Reset */
	{ 0x3012, 0x80 },
	{ REG_TERM, 0x05 }, // delay

	/* XGA Common */
	{ 0x3002, 0x03 },	//	AEC = 0x030F
	{ 0x3003, 0x0F },	//	AEC = 0x030F
	{ 0x3001, 0x07 },	//	AGC = 0x0007 { REG[0x3000]=0x00}
	{ 0x304D, 0x45 },	//	??, default 0x79
	{ 0x30AA, 0x45 },	//	??, default 0x48

	{ 0x30B0, 0xFF },	//	output port, CY[7:0]
	{ 0x30B1, 0xFF },	//	output port, GP[1:0], VSYNC, STROBE, PCLK, HREF, CY[9:8]
	{ 0x30B2, 0x10 },	//	GPO_monitor, FREX, R_PAD[3:0]
						//	bit[1:0] - drive capability

	{ 0x3018, 0x38 },	//	WPT/HisH
	{ 0x3019, 0x30 },	//	BPT/HisL
	{ 0x301A, 0x61 },	//	VPT

	{ 0x3082, 0x20 },	//	??, default 0x00
	{ 0x3015, 0x14 },	//	alow 1 dummy frame, 32x AGC gain ceiling
	{ 0x3013, 0xF8 },	//	faster AEC, big steps, banding filter on, auto disable banding filter under strong light
						//	extreme bright exposure control enable

	{ 0x303C, 0x08 },	//	Average Window Horizotal Width ( AHW ) = 0x0818
	{ 0x303D, 0x18 },
	{ 0x303E, 0x06 },	//	Average Window Vertical Height ( AVH ) = 0x060c
	{ 0x303F, 0x0C },
	{ 0x3030, 0x62 },	//	Average Section Weight, Sect 2 = 6, Sect 1 = 2
	{ 0x3031, 0x26 },	//	Average Section Weight, Sect 4 = 2, Sect 3 = 6
	{ 0x3032, 0xE6 },
	{ 0x3033, 0x6E },
	{ 0x3034, 0xEA },
	{ 0x3035, 0xAE },
	{ 0x3036, 0xA6 },
	{ 0x3037, 0x6A },	//	Average Section Weight, Sect 16 = 6, Sect 15 = 10

	{ 0x3104, 0x02 },	//	MC top reset, others enable ( ISP_pad, DSP, CIF_ptn, MIPI_top, Format, MIPI_rst, CIF_top )
	{ 0x3105, 0xFD },	//	MC clock disable, others clock enable ( ISP_pad, DSP, CIF_ptn, MIPI_top, Format, MIPI_rst, CIF_top )
	{ 0x3106, 0x00 },	//	enable module of CIF pad, RGB dither, YUV2RGB, CIF ext, SCCB arb, Compression top
	{ 0x3107, 0xFF },	//	enable clock of RGB dither, YUV2RGB, CIF ext, SCCB arb, Compression top

	{ 0x3300, 0x13 },	//	enable AFC, LENC, other disable OVLY, YUV422 in, bar en, 
	{ 0x3301, 0xDE },	//	enable UV arg, CMX, WC, BC
	{ 0x3302, 0xEF },	//	enable SDE, UV adj, Scale, CIP, GMA, AWB gain, AWB
	{ 0x3316, 0xFF },	//	??
	{ 0x3317, 0x00 },	//	??
	{ 0x3312, 0x26 },	//	??
	{ 0x3314, 0x42 },	//	??
	{ 0x3313, 0x2B },	//	??
	{ 0x3315, 0x42 },	//	??
	{ 0x3310, 0xD0 },	//	??
	{ 0x3311, 0xBD },	//	??
	{ 0x330C, 0x18 },	//	??
	{ 0x330D, 0x18 },	//	??
	{ 0x330E, 0x56 },	//	??
	{ 0x330F, 0x5C },	//	??
	{ 0x330B, 0x1C },	//	??
	{ 0x3306, 0x5C },	//	??
	{ 0x3307, 0x11 },	//	??
	{ 0x336A, 0x52 },	//	R_A1
	{ 0x3370, 0x46 },	//	G_A1
	{ 0x3376, 0x38 },	//	B_A1
	{ 0x30B8, 0x20 },	//	uvadj_k[7:0]
	{ 0x30B9, 0x17 },	//	uvadj_offs[4:0]
	{ 0x30BA, 0x04 },	//	uvadj_gth1[3:0]
	{ 0x30BB, 0x08 },	//	uvadj_gth2[4:0]
	{ 0x3507, 0x06 },	//	??
	{ 0x350A, 0x4F },	//	??
	{ 0x3100, 0x02 },	//	MC power down
	{ 0x3301, 0xDE },	//	enable UV avg, CMX, WC, BC
	{ 0x3304, 0xFC },	//	CMX, GMA, LENC on/plus
	{ 0x3012, 0x10 },	//	xga mode
	{ 0x3023, 0x07 },	//	vertical start = 0x0007 ( 0x3022 = 0x00 )
	{ 0x3026, 0x03 },	//	vertical height = 0x0424
	{ 0x3027, 0x04 },	//	vertical height = 0x0424
	{ 0x3075, 0x24 },	//	VS start point = 2, VS pulse width = 4
	{ 0x300D, 0x01 },	//	PCLK div 2
	{ 0x30D7, 0x90 },	//	??

	//	zoom
	{ 0x335F, 0x34 },	//	Hsize_in = 0x40C, Vsize_in = 0x304
	{ 0x3360, 0x0C },
	{ 0x3361, 0x04 },
	{ 0x3362, 0x34 },	//	Hsize_out = 0x408, Vsize_out = 0x304
	{ 0x3363, 0x08 },
	{ 0x3364, 0x04 },

	//	output
	{ 0x3403, 0x42 },	//	DVP X start = 4, DVP Y start = 2
	{ 0x3088, 0x04 },	//	ISP XOUT = 0x0400
	{ 0x3089, 0x00 },
	{ 0x308A, 0x03 },	//	ISP YOUT = 0x0300
	{ 0x308B, 0x00 },
	{ 0x308D, 0x04 },	//	reset block sleep enable
	{ 0x3600, 0xC4 },	//	VS mode 3, VS gate PCLK, VS pol

	{ 0x332B, 0x00 },	//	AWB gain auto
	{ 0x332D, 0x60 },	//	DNS manual, Edge manual
	{ 0x332F, 0x03 },	//	base1 = 3

    {REG_TERM, VAL_TERM}

};


static struct sensor_reg sensor_set_preview[] = {
	
	/*	XGA YUV422 2	*/
	{ 0x3100, 0x02 },
	{ 0x3304, 0xFC },
	{ 0x3400, 0x07 },	//	not use
	{ 0x3404, 0x02 },	//	uyvy
	{ 0x3601, 0x01 },
	{ 0x302A, 0x03 },
	{ 0x302B, 0x10 },
	
	/*	frame rate 30 fps	*/
	{ 0x3011, 0x00 },
	{ 0x300E, 0x32 },
	{ 0x300F, 0x21 },
	{ 0x304C, 0x82 },

    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_set_capture[] = {

	/*	XGA YUV422 2	*/
	{ 0x3100, 0x02 },
	{ 0x3304, 0xFC },
	{ 0x3400, 0x07 },	//	not use
	{ 0x3404, 0x02 },	//	uyvy
	{ 0x3601, 0x01 },
	{ 0x302A, 0x03 },
	{ 0x302B, 0x10 },
	
	/*	frame rate 30 fps	*/
	{ 0x3011, 0x00 },
	{ 0x300E, 0x32 },
	{ 0x300F, 0x21 },
	{ 0x304C, 0x82 },

    {REG_TERM, VAL_TERM}
};

 struct sensor_reg* sensor_reg_common[3] =
{
	sensor_initialize,
	sensor_set_preview,
	sensor_set_capture
};

static struct sensor_reg sensor_brightness_0[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_brightness_1[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_brightness_2[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_brightness_3[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_brightness_4[] = {
    {REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_brightness[5] =
{
	sensor_brightness_0,
	sensor_brightness_1,
	sensor_brightness_2,
	sensor_brightness_3,
	sensor_brightness_4
};


static struct sensor_reg sensor_awb_auto[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_awb_daylight[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_awb_incandescent[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_awb_fluorescent[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_awb_cloudy[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_awb_sunset[] = {
    {REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_awb[6] =
{
	sensor_awb_auto,
	sensor_awb_daylight,
	sensor_awb_incandescent,
	sensor_awb_fluorescent,
	sensor_awb_cloudy,
	sensor_awb_sunset
	
};


static struct sensor_reg sensor_iso_auto[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_iso_100[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_iso_200[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_iso_400[] = {
	{REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_iso[4] =
{
	sensor_iso_auto,
	sensor_iso_100,
	sensor_iso_200,
	sensor_iso_400
};


static struct sensor_reg sensor_effect_normal[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_effect_gray[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_effect_negative[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_effect_sepia[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_effect_sharpness[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_effect_sketch[] = {
    {REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_effect[6] =
{
	sensor_effect_normal,
	sensor_effect_gray,
	sensor_effect_negative,
	sensor_effect_sepia,
	sensor_effect_sharpness,
	sensor_effect_sketch,
};


static struct sensor_reg sensor_reg_flipnone[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_reg_hflip[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_reg_vflip[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_reg_hvflip[] = {
    {REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_flip[4] =
{
	sensor_reg_flipnone,
	sensor_reg_hflip,
	sensor_reg_vflip,
	sensor_reg_hvflip,
};


static struct sensor_reg sensor_secne_auto[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_secne_night[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_secne_landscape[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_secne_portrait[] = {
	{REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_secne_sport[] = {
	{REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_scene[5] =
{
	sensor_secne_auto,
	sensor_secne_night,
	sensor_secne_landscape,
	sensor_secne_portrait,
	sensor_secne_sport
};

static struct sensor_reg sensor_me_mtrix[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_me_center_weighted[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_me_spot[] = {
    {REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_metering_exposure[3] =
{
	sensor_me_mtrix,
	sensor_me_center_weighted,
	sensor_me_spot,
};

static struct sensor_reg sensor_af_single[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_af_manual[] = {
    {REG_TERM, VAL_TERM}
};

struct sensor_reg* sensor_reg_af[2] =
{
	sensor_af_single,
	sensor_af_manual,
};

static int write_regs(const struct sensor_reg reglist[])
{
	int err;
	int err_cnt = 0;	
	unsigned char data[132];
	unsigned char bytes;
	const struct sensor_reg *next = reglist;
	
	while (!((next->reg == REG_TERM) && (next->val == VAL_TERM)))
	{
		
		if(next->reg == REG_TERM && next->val != VAL_TERM)
		{
			mdelay(next->val);
			printk("Sensor init Delay[%d]!!!! \n", next->val);
			next++;
		}
		else
		{
			bytes = 0;
			data[bytes]= next->reg>>8;		bytes++;		
			data[bytes]= (u8)next->reg&0xff; 	bytes++;

			data[bytes]= next->val;		bytes++;
			
	        err = DDI_I2C_Write(data, 2, bytes-2);
	        
	        if (err)
	        {
				err_cnt++;
				if(err_cnt >= 3)
				{
					printk("ERROR: Sensor I2C !!!! \n"); 
					return err;
				}
	        }
	        else
	        {
	        	err_cnt = 0;
	            next++;
	        }
		}
	}

	return 0;
}

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

static int sensor_open(void)
{
	CAM_PWR_DISABLE();		sensor_delay(10);
	CAM_PWR_ENABLE();		sensor_delay(10);

	CAM_PWRDN_DISABLE();	sensor_delay(10);
	CAM_RESET_LOW();		sensor_delay(10);

/**/	CAM_PWRDN_ENABLE();		sensor_delay(5);

	CIF_Open();				sensor_delay(40);
	
	CAM_RESET_HIGH();		sensor_delay(15);

	return write_regs(sensor_reg_common[0]);
}

static int sensor_close(void)
{
	CIF_ONOFF(OFF);

	CAM_RESET_LOW();
	CAM_PWR_DISABLE();
	CAM_PWRDN_DISABLE();

	CIF_Close();   

	msleep(5);    

	return 0;
}

static int sensor_preview(void)
{
	return write_regs(sensor_reg_common[1]);
}

static int sensor_capture(void)
{
	return write_regs(sensor_reg_common[2]);
}

static int sensor_capturecfg(int width, int height)
{
	return 0;
}

static int sensor_zoom(int val)
{
	return 0;
}

static int sensor_autofocus(int val)
{
	return write_regs(sensor_reg_af[val]);
}

static int sensor_effect(int val)
{
	return write_regs(sensor_reg_effect[val]);
}

static int sensor_flip(int val)
{
	return write_regs(sensor_reg_flip[val]);
}

static int sensor_iso(int val)
{
	return write_regs(sensor_reg_iso[val]);
}

static int sensor_me(int val)
{
	return write_regs(sensor_reg_metering_exposure[val]);
}

static int sensor_wb(int val)
{
	return write_regs(sensor_reg_awb[val]);
}

static int sensor_bright(int val)
{
	return write_regs(sensor_reg_brightness[val]);
}

static int sensor_scene(int val)
{
	return write_regs(sensor_reg_scene[val]);
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
*    Function  
**********************************************************/
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 			= sensor_open;
	sensor_func->Close 			= sensor_close;

	sensor_func->Set_Preview 	= sensor_preview;
	sensor_func->Set_Capture 	= sensor_capture;
	sensor_func->Set_CaptureCfg = sensor_capturecfg;

	sensor_func->Set_Zoom		= sensor_zoom;
	sensor_func->Set_AF 		= sensor_autofocus;
	sensor_func->Set_Effect 	= sensor_effect;
	sensor_func->Set_Flip 		= sensor_flip;
	sensor_func->Set_ISO 		= sensor_iso;
	sensor_func->Set_ME 		= sensor_me;
	sensor_func->Set_WB 		= sensor_wb;
	sensor_func->Set_Bright 	= sensor_bright;
	sensor_func->Set_Scene 		= sensor_scene;

	sensor_func->Check_ESD 		= sensor_check_esd;
	sensor_func->Check_Luma 	= sensor_check_luma;
}

#endif

