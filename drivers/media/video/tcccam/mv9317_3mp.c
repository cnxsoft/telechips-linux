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
#include "cam_reg.h"
#include "tcc_cam_i2c.h"

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_MV9317

#if (0)
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
#endif

/* Array of image sizes supported by MV9317.  These must be ordered from 
 * smallest image size to largest.
 */
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
struct capture_size sensor_sizes_mv9317[] = {
	{ 2048, 1536 },	/* QXGA */
	{ 1600, 1200 },	/* UXGA */
	{ 1280,  960 },	/* SXGA */
	{ 1024,  768 },	/* XGA */
	{  800,  600 },	/* SVGA */
	{  640,  480 },	/* VGA */
	{  320,  240 },	/* QVGA */
	{  176,  144 },	/* QCIF */
};
#else
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
#endif

/* register initialization tables for sensor */
/* common sensor register initialization for all image sizes, pixel formats, 
 * and frame rates
 */
static struct sensor_reg sensor_initialize_mv9317[] = {
	{0x07, 0x07},			//XGA
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};


static struct sensor_reg sensor_set_preview_mv9317[] = {
	{0x07, 0x07},			//XGA
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_set_capture_mv9317[] = {
	{0x07, 0x0c},			//QXGA
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

 struct sensor_reg* sensor_reg_common_mv9317[3] =
{
	sensor_initialize_mv9317,
	sensor_set_preview_mv9317,
	sensor_set_capture_mv9317
};

static struct sensor_reg sensor_brightness_0_mv9317[] = {
	{0x10, 0x83},			//M2
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_brightness_1_mv9317[] = {
	{0x10, 0x81},			//M1
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_brightness_2_mv9317[] = {
	{0x10, 0x00},			//DEFAULT
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_brightness_3_mv9317[] = {
	{0x10, 0x01},			//P1
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_brightness_4_mv9317[] = {
	{0x10, 0x03},			//P2
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_brightness_mv9317[5] =
{
	sensor_brightness_0_mv9317,
	sensor_brightness_1_mv9317,
	sensor_brightness_2_mv9317,
	sensor_brightness_3_mv9317,
	sensor_brightness_4_mv9317
};


static struct sensor_reg sensor_awb_auto_mv9317[] = {
	{0x14, 0x80},
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_awb_daylight_mv9317[] = {
	{0x14, 0x01},			//Daylight
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_awb_incandescent_mv9317[] = {
	{0x14, 0x04},			//A(Incandescent)
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_awb_fluorescent_mv9317[] = {
	{0x14, 0x03},			//Cool White Fluorescent
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_awb_cloudy_mv9317[] = {
	{0x14, 0x06},			//Cloudy
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_awb_sunset_mv9317[] = {
	{0x14, 0x05},			//Horizon(sunset)
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_awb_mv9317[6] =
{
	sensor_awb_auto_mv9317,
	sensor_awb_daylight_mv9317,
	sensor_awb_incandescent_mv9317,
	sensor_awb_fluorescent_mv9317,
	sensor_awb_cloudy_mv9317,
	sensor_awb_sunset_mv9317
	
};


static struct sensor_reg sensor_iso_auto_mv9317[] = {
	{0x0d, 0x00},			//auto
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_iso_100_mv9317[] = {
	{0x0d, 0x01},			//100
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_iso_200_mv9317[] = {
	{0x0d, 0x02},			//200
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_iso_400_mv9317[] = {
	{0x0d, 0x03},			//400
	{MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_iso_mv9317[4] =
{
	sensor_iso_auto_mv9317,
	sensor_iso_100_mv9317,
	sensor_iso_200_mv9317,
	sensor_iso_400_mv9317
};


static struct sensor_reg sensor_effect_normal_mv9317[] = {
	{0x1f, 0x00},			//NORMAL
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_effect_gray_mv9317[] = {
	{0x1f, 0x40},			//BW
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_effect_negative_mv9317[] = {
	{0x1f, 0x20},			//NEG_POSITIVE
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_effect_sepia_mv9317[] = {
	{0x1f, 0x10},			//SEPHIA
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_effect_sharpness_mv9317[] = {
	{0x1f, 0x02},			//SHARPNESS
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_effect_sketch_mv9317[] = {
	{0x1f, 0x04},			//SKETCH
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_effect_mv9317[6] =
{
	sensor_effect_normal_mv9317,
	sensor_effect_gray_mv9317,
	sensor_effect_negative_mv9317,
	sensor_effect_sepia_mv9317,
	sensor_effect_sharpness_mv9317,
	sensor_effect_sketch_mv9317
};


static struct sensor_reg sensor_reg_flipnone_mv9317[] = {
	{0x09, 0x00},			//NORMAL
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_reg_hflip_mv9317[] = {
	{0x09, 0x01},			//MIRROR
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_reg_vflip_mv9317[] = {
	{0x09, 0x02},			//FLIP
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_reg_hvflip_mv9317[] = {
	{0x09, 0x03},			//MIRROR_FLIP
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_flip_mv9317[4] =
{
	sensor_reg_flipnone_mv9317,
	sensor_reg_hflip_mv9317,
	sensor_reg_vflip_mv9317,
	sensor_reg_hvflip_mv9317
};


static struct sensor_reg sensor_secne_auto_mv9317[] = {
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_secne_night_mv9317[] = {
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_secne_landscape_mv9317[] = {
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_secne_portrait_mv9317[] = {
	{MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_secne_sport_mv9317[] = {
	{MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_scene_mv9317[5] =
{
	sensor_secne_auto_mv9317,
	sensor_secne_night_mv9317,
	sensor_secne_landscape_mv9317,
	sensor_secne_portrait_mv9317,
	sensor_secne_sport_mv9317
};

static struct sensor_reg sensor_me_mtrix_mv9317[] = {
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_me_center_weighted_mv9317[] = {
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_me_spot_mv9317[] = {
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_metering_exposure_mv9317[3] =
{
	sensor_me_mtrix_mv9317,
	sensor_me_center_weighted_mv9317,
	sensor_me_spot_mv9317
};

static struct sensor_reg sensor_af_single_mv9317[] = {
    {0x19, 0x00},	
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

static struct sensor_reg sensor_af_manual_mv9317[] = {
    {0x1A, 0x20},
    {MV9317_REG_TERM, MV9317_VAL_TERM}
};

struct sensor_reg* sensor_reg_af_mv9317[2] =
{
	sensor_af_single_mv9317,
	sensor_af_manual_mv9317
};

static int write_regs_mv9317(const struct sensor_reg reglist[])
{
	int err;
	int err_cnt = 0;	
	unsigned char data[132];
	unsigned char bytes;
	const struct sensor_reg *next = reglist;
	
	while (!((next->reg == MV9317_REG_TERM) && (next->val == MV9317_VAL_TERM)))
	{
		bytes = 0;
		data[bytes]= (unsigned char)next->reg&0xff; 	bytes++;
		data[bytes]= (unsigned char)next->val&0xff;

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

	return 0;
}

static int sensor_open_mv9317(bool bChangeCamera)
{
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

	return write_regs_mv9317(sensor_reg_common_mv9317[0]);
}

static int sensor_close_mv9317(void)
{
	CIF_ONOFF(OFF);

	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_disable();

	CIF_Close();   
	msleep(5);    

	return 0;
}

static int sensor_powerdown_mv9317(bool bChangeCamera)
{
	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_disable();

	return 0;
}

static int sensor_preview_mv9317(void)
{
	return write_regs_mv9317(sensor_reg_common_mv9317[1]);
}

static int sensor_capture_mv9317(void)
{
	return write_regs_mv9317(sensor_reg_common_mv9317[2]);
}

static int sensor_capturecfg_mv9317(int width, int height)
{
	return 0;
}

static int sensor_zoom_mv9317(int val)
{
	return 0;
}

static int sensor_autofocus_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_af_mv9317[val]);
}

static int sensor_effect_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_effect_mv9317[val]);
}

static int sensor_flip_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_flip_mv9317[val]);
}

static int sensor_iso_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_iso_mv9317[val]);
}

static int sensor_me_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_metering_exposure_mv9317[val]);
}

static int sensor_wb_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_awb_mv9317[val]);
}

static int sensor_bright_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_brightness_mv9317[val]);
}

static int sensor_scene_mv9317(int val)
{
	return write_regs_mv9317(sensor_reg_scene_mv9317[val]);
}

static int sensor_check_esd_mv9317(int val)
{
	return 0;
}

static int sensor_check_luma_mv9317(int val)
{
	return 0;
}

/**********************************************************
*    Function  
**********************************************************/
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void sensor_info_init_mv9317(TCC_SENSOR_INFO_TYPE *sensor_info)
{
	sensor_info->i2c_addr 				= 0x50;
	sensor_info->reg_term 				= 0xFF;
	sensor_info->val_term 				= 0xFF;
	sensor_info->m_clock 				= 720000;
	sensor_info->s_clock 					= 720000;
	#if defined(CONFIG_ARCH_TCC92XX)
	sensor_info->m_clock 				= 72000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 720000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#elif defined(CONFIG_ARCH_TCC93XX)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 720000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 720000;
	sensor_info->s_clock_source 			= PCDIRECTPLL1;
	#else // CONFIG_USE_ISP
	sensor_info->m_clock 				= 720000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 720000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#endif // CONFIG_USE_ISP
	#elif defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 720000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 720000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else // CONFIG_USE_ISP
	sensor_info->m_clock 				= 720000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 720000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#endif // CONFIG_USE_ISP
	#endif
	sensor_info->preview_w 				= 1024;
	sensor_info->preview_h 				= 768;
	sensor_info->preview_zoom_offset_x 	= 16;
	sensor_info->preview_zoom_offset_y 	= 12;
	sensor_info->capture_w 				= 2048;
	sensor_info->capture_h 				= 1536;
	sensor_info->capture_zoom_offset_x 	= 32;
	sensor_info->capture_zoom_offset_y 	= 24;
	sensor_info->max_zoom_step 			= 15;
	sensor_info->cam_capchg_width 		= 1024;
	sensor_info->framerate				= 20;
	sensor_info->p_clock_pol 				= NEGATIVE_EDGE;
	sensor_info->v_sync_pol 				= ACT_LOW;
	sensor_info->h_sync_pol                 = ACT_HIGH;
	#if defined(CONFIG_ARCH_TCC892X)
	sensor_info->de_pol                 = ACT_LOW;
	#endif
	sensor_info->format 					= M420_ZERO;
	sensor_info->capture_skip_frame 		= 3;
	sensor_info->sensor_sizes 			= sensor_sizes_mv9317;
}

void sensor_init_fnc_mv9317(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_mv9317;
	sensor_func->Close 					= sensor_close_mv9317;
	sensor_func->PowerDown				= sensor_powerdown_mv9317;

	sensor_func->Set_Preview 			= sensor_preview_mv9317;
	sensor_func->Set_Capture 			= sensor_capture_mv9317;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_mv9317;

	sensor_func->Set_Zoom 				= sensor_zoom_mv9317;
	sensor_func->Set_AF 				= sensor_autofocus_mv9317;
	sensor_func->Set_Effect 				= sensor_effect_mv9317;
	sensor_func->Set_Flip 				= sensor_flip_mv9317;
	sensor_func->Set_ISO 				= sensor_iso_mv9317;
	sensor_func->Set_ME 				= sensor_me_mv9317;
	sensor_func->Set_WB 				= sensor_wb_mv9317;
	sensor_func->Set_Bright 				= sensor_bright_mv9317;
	sensor_func->Set_Scene 				= sensor_scene_mv9317;

	sensor_func->Check_ESD 				= sensor_check_esd_mv9317;
	sensor_func->Check_Luma 			= sensor_check_luma_mv9317;
}
#else
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_mv9317;
	sensor_func->Close 					= sensor_close_mv9317;
	sensor_func->PowerDown				= sensor_powerdown_mv9317;

	sensor_func->Set_Preview 			= sensor_preview_mv9317;
	sensor_func->Set_Capture 			= sensor_capture_mv9317;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_mv9317;

	sensor_func->Set_Zoom 				= sensor_zoom_mv9317;
	sensor_func->Set_AF 				= sensor_autofocus_mv9317;
	sensor_func->Set_Effect 				= sensor_effect_mv9317;
	sensor_func->Set_Flip 				= sensor_flip_mv9317;
	sensor_func->Set_ISO 				= sensor_iso_mv9317;
	sensor_func->Set_ME 				= sensor_me_mv9317;
	sensor_func->Set_WB 				= sensor_wb_mv9317;
	sensor_func->Set_Bright 				= sensor_bright_mv9317;
	sensor_func->Set_Scene 				= sensor_scene_mv9317;

	sensor_func->Check_ESD 				= sensor_check_esd_mv9317;
	sensor_func->Check_Luma 			= sensor_check_luma_mv9317;
}
#endif
#endif

