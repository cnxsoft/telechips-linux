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

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#ifdef CONFIG_VIDEO_CAMERA_NEXTCHIP_TEST
#if 0
/* Array of image sizes supported by MT9D112.  These must be ordered from 
 * smallest image size to largest.
 */
struct capture_size sensor_sizes[] = {
	{ PRV_W, PRV_H },	//{ 1920, 1080 },
};


/* register initialization tables for sensor */
/* common sensor register initialization for all image sizes, pixel formats, 
 * and frame rates
 */
static struct sensor_reg sensor_common[] = {
    {REG_TERM, VAL_TERM}
};


static struct sensor_reg sensor_preview[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_capture[] = {
    {REG_TERM, VAL_TERM}
};

 struct sensor_reg* sensor_reg_common[3] =
{
	sensor_common,
	sensor_preview,
	sensor_capture
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
#endif

static int write_regs(const struct sensor_reg reglist[])
{
	#if 0
	int err;
	int err_cnt = 0;	
	unsigned char data[132];
	unsigned char bytes;
	const struct sensor_reg *next = reglist;
	
	while (!((next->reg == REG_TERM) && (next->val == VAL_TERM)))
	{
		bytes = 0;
		data[bytes]= next->reg&0xff; 	bytes++;
		data[bytes]= next->val&0xff;
		
        err = DDI_I2C_Write(data, 1, 1);

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
#endif
	return 0;
}
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

static struct sensor_reg sensor_initialize[] = {
    {REG_TERM, VAL_TERM}
};


static struct sensor_reg sensor_set_preview[] = {
    {REG_TERM, VAL_TERM}
};

static struct sensor_reg sensor_set_capture[] = {
    {REG_TERM, VAL_TERM}
};

 struct sensor_reg* sensor_reg_common[3] =
{
	sensor_initialize,
	sensor_set_preview,
	sensor_set_capture
};
/*
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
*/
static int sensor_open(void)
{
#if 0
	sensor_power_disable();
	sensor_delay(10);
	
	sensor_power_enable();
	sensor_delay(10);

	sensor_powerdown_disable();
	sensor_delay(10);
	
	sensor_reset_low();
	sensor_delay(10);

	sensor_powerdown_enable();
	sensor_delay(5);

	CIF_Open();
	sensor_delay(40);
	
	sensor_reset_high();
	sensor_delay(15); // msleep(1);

	return write_regs(sensor_reg_common[0]);
	#endif
	CIF_Open();
	sensor_delay(40);
	return 0;
}

static int sensor_close(void)
{
	CIF_ONOFF(OFF);

	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_disable();

	CIF_Close();   
	msleep(5);    

	return 0;
}

static int sensor_preview(void)
{
	//return write_regs(sensor_reg_common[1]);
	return 0;
}

static int sensor_capture(void)
{
	//return write_regs(sensor_reg_common[2]);
	return 0;
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
	//return write_regs(sensor_reg_af[val]);
		return 0;
}

static int sensor_effect(int val)
{
	//return write_regs(sensor_reg_effect[val]);
		return 0;
}

static int sensor_flip(int val)
{
	//return write_regs(sensor_reg_flip[val]);
		return 0;
}

static int sensor_iso(int val)
{
	//return write_regs(sensor_reg_iso[val]);
		return 0;
}

static int sensor_me(int val)
{
	//return write_regs(sensor_reg_metering_exposure[val]);
		return 0;
}

static int sensor_wb(int val)
{
	//return write_regs(sensor_reg_awb[val]);
		return 0;
}

static int sensor_bright(int val)
{
	//return write_regs(sensor_reg_brightness[val]);
		return 0;
}

static int sensor_scene(int val)
{
	//return write_regs(sensor_reg_scene[val]);
		return 0;
}

static int sensor_check_esd(int val)
{
	//return 0;
		return 0;
}

static int sensor_check_luma(int val)
{
	//return 0;
		return 0;
}


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

