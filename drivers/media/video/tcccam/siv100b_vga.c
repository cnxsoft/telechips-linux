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

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
#include <mach/bsp.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif

#ifdef CONFIG_VIDEO_CAMERA_SENSOR_SIV100B

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

/* Array of image sizes supported by OV7690.  These must be ordered from 
 * smallest image size to largest.
 */
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
struct capture_size sensor_sizes_siv100b[] = {

	{  640,  480 },	/* VGA */
	{  320,  240 },	/* QVGA */
	{  176,  144 },	/* QCIF */
};
#else
struct capture_size sensor_sizes[] = {
	{  640,  480 },	/* VGA */
	{  352,  288 },	/* CIF */
	{  320,  240 },	/* QVGA */
	{  176,  144 },	/* QCIF */
};
#endif

/* register initialization tables for sensor */
/* common sensor register initialization for all image sizes, pixel formats, 
 * and frame rates
 */
static struct sensor_reg sensor_initialize_siv100b[] = {
	 {0x04, 0x00},  // setup clock divider and HV-mirror
    {0x05, 0x07},  // setup VGA output mode

    // Vendor recommended value ### Don't change ###
    {0x11, 0x04},
    {0x12, 0x0A},
    {0x13, 0x1F},
    {0x16, 0x89},
    {0x1B, 0x90},
    {0x1F, 0x52},

    /* SIV100B  50Hz - Min. 12.5fps(100/8) 24MHz */
    {0x20, 0x10},
    {0x21, 0x90},
    {0x22, 0x63},
    {0x34, 0x64},

    {0x33, 0x08},

    /* AE */
    {0x40, 0x80},
    {0x41, 0x8A},
    {0x42, 0x7F},
    {0x43, 0xC0},
    {0x44, 0x38},	//0x48
    {0x45, 0x28},
    {0x46, 0x08},
    {0x47, 0x15},
    {0x48, 0x1E},
    {0x49, 0x13},
    {0x4A, 0x63},
    {0x4B, 0xC4},
    {0x4C, 0x3C},
    {0x4E, 0x17},
    {0x4F, 0x8A},
    {0x50, 0x94},

    {0x5A, 0x00},

    /* Auto White Balance  0105 */
    {0x60, 0xC3},	//0xC8
    {0x61, 0x08},	//0x88
    {0x62, 0x01},
    {0x63, 0x80},
    {0x64, 0x80},
    {0x65, 0xD0},
    {0x66, 0x8C},
    {0x67, 0xC8},
    {0x68, 0x8B},
    {0x69, 0x8A},
    {0x6A, 0x73},
    {0x6B, 0x90},	//0x95
    {0x6C, 0x70},
    {0x6D, 0x88},
    {0x6E, 0x77},
    {0x6F, 0x46},
    {0x70, 0xEA},
    {0x71, 0x60},
    {0x72, 0x05},
    {0x73, 0x02},
    {0x74, 0x0C},
    {0x75, 0x0F},
    {0x76, 0x20},
    {0x77, 0xB7},
    {0x78, 0x97},

    /* IDP */
    {0x80, 0xAF},
    {0x81, 0x0D},	//Select Sensor's PCLK Polarity
    {0x82, 0x1D},	//Y,Cb,Cr Order Sequence   
    {0x83, 0x00},
    {0x86, 0xA1},
    {0x87, 0x00},
    {0x88, 0x24},	//DPC Start Gain
    {0x89, 0x0F},

    {0x92, 0x44},
    {0x93, 0x10},
    {0x94, 0x20},
    {0x95, 0x18},	//0x40
    {0x96, 0x10},
    {0x97, 0x20},
    {0x98, 0x20},	//0x30
    {0x99, 0x18},	//0x29
    {0x9A, 0x50},

    /* Shading  0106 */
    {0xA4, 0xDD},	//0xFF
    {0xA5, 0xDD},	//0xFF
    {0xA6, 0xDD},	//0xFF
    {0xA7, 0xDC},	//0xED
    {0xA8, 0xBA},	//0xCB
    {0xA9, 0x44},	//0x55
    {0xAA, 0x36},	//0x55
    {0xAB, 0x11},	//0x55
    {0xAC, 0x13},	//0x55
    {0xAE, 0x00},	//0x55
    {0xAD, 0x01},	//0x55
    {0xAF, 0x98},
    {0xB0, 0x90},

    /* Gamma  0126 */
    {0xB1, 0x00},
    {0xB2, 0x0D},	//0x08
    {0xB3, 0x16},	//0x10
    {0xB4, 0x2A},	//0x23
    {0xB5, 0x48},	//0x45
    {0xB6, 0x64},	//0x62
    {0xB7, 0x79},	//0x78
    {0xB8, 0x8C},	//0x8A
    {0xB9, 0x9C},	//0x9B
    {0xBA, 0xAA},	
    {0xBB, 0xB6},	//0xB8
    {0xBC, 0xCB},	//0xCE
    {0xBD, 0xDE},	//0xE2
    {0xBE, 0xF0},	//0xF3
    {0xBF, 0xF8},	//0xFB
    {0xC0, 0xFF},

    /* Color Matrix */
    {0xC1, 0x3A},	//0x3D
    {0xC2, 0xCB},	//0xC6
    {0xC3, 0xFB},	//0xFD
    {0xC4, 0x10},
    {0xC5, 0x21},
    {0xC6, 0x10},
    {0xC7, 0xF3},
    {0xC8, 0xBD},
    {0xC9, 0x50},

    /* Edge */
    {0xCA, 0x90},
    {0xCB, 0x18},	
    {0xCC, 0x20},	
    {0xCD, 0x06},
    {0xCE, 0x06},
    {0xCF, 0x20},	//0x10
    {0xD0, 0x20},
    {0xD1, 0x28},	//0x2A
    {0xD2, 0x86},
    {0xD3, 0x00},

    /* Contrast */
    {0xD4, 0x10},
    {0xD5, 0x13},	//0x12
    {0xD6, 0x13},	//0x12
    {0xD7, 0x00},
    {0xD8, 0x00},
    {0xD9, 0x00},
    {0xDA, 0x00},
    {0xDB, 0xFF},
    {0xDC, 0x00},
    {0xDD, 0xFF},
    {0xDE, 0x00},
    {0xDF, 0xFF},
    {0xE0, 0x00},	

    /* Saturation */
    {0xE1, 0x27},	//0x24
    {0xE2, 0x8A},	//0x58
    
   /* Windowing */    
    {0xF0, 0x24},
    {0xF1, 0x00},    
    {0xF2, 0x80},
    {0xF3, 0x00},    	
    {0xF4, 0xE0}, 
    
    {0x03, 0xC5},    

    {0x7A, 0xB0},	//0x90
    {0x7B, 0xB0},
    {0x7C, 0x80},

 {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};


static struct sensor_reg sensor_set_preview_siv100b[] = {
	 {0x40, 0x80},  /* enable *NORMAL* AE */
   {0x60, 0xC3},  /* enable AWB */

	{SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_set_capture_siv100b[] = {
        
	
	{SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

 struct sensor_reg* sensor_reg_common_siv100b[3] =
{
	sensor_initialize_siv100b,
	sensor_set_preview_siv100b,
	sensor_set_capture_siv100b
};

static struct sensor_reg sensor_brightness_0_siv100b[] = {
	
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_brightness_1_siv100b[] = {
	
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_brightness_2_siv100b[] = {
	
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_brightness_3_siv100b[] = {
	
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_brightness_4_siv100b[] = {
	
	{SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

struct sensor_reg* sensor_reg_brightness_siv100b[5] =
{
	sensor_brightness_0_siv100b,
	sensor_brightness_1_siv100b,
	sensor_brightness_2_siv100b,
	sensor_brightness_3_siv100b,
	sensor_brightness_4_siv100b
};


static struct sensor_reg sensor_awb_auto_siv100b[] = {
       
	{0x60, 0xC3},
      
      
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_awb_daylight_siv100b[] = {
    {0x60, 0x00},  // disable AWB
	{0x7A, 0xC2},
	{0x7B, 0x9E},  
   
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_awb_incandescent_siv100b[] = {
  	{0x60, 0x00},  // disable AWB
	{0x7A, 0x98},
	{0x7B, 0xC8},     
                             
   {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_awb_fluorescent_siv100b[] = {
    {0x60, 0x00},  // disable AWB
	{0x7A, 0xAA},
	{0x7B, 0xBE},    
                            
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_awb_cloudy_siv100b[] = {
	{0x60, 0x00},  // disable AWB
	{0x7A, 0xD0},
	{0x7B, 0x88},
  
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_awb_tungsten_siv100b[] = {
    {0x60, 0x00},  // disable AWB
	{0x7A, 0x90},
	{0x7B, 0xC0},
   
     {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};	                                   
struct sensor_reg* sensor_reg_awb_siv100b[6] =
{
	sensor_awb_auto_siv100b,
	sensor_awb_daylight_siv100b,
	sensor_awb_incandescent_siv100b,
	sensor_awb_fluorescent_siv100b,
	sensor_awb_cloudy_siv100b,
	sensor_awb_tungsten_siv100b
	
};


static struct sensor_reg sensor_iso_auto_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_iso_100_siv100b[] = {
	
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_iso_200_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_iso_400_siv100b[] = {

	{SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

struct sensor_reg* sensor_reg_iso_siv100b[4] =
{
	sensor_iso_auto_siv100b,
	sensor_iso_100_siv100b,
	sensor_iso_200_siv100b,
	sensor_iso_400_siv100b
};


static struct sensor_reg sensor_effect_normal_siv100b[] = {
       
    {0xD8, 0x00},
     
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_effect_gray_siv100b[] = {
      
    {0xD8, 0x40},    
         
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_effect_negative_siv100b[] = {
            
       
             
    
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_effect_sepia_siv100b[] = {
    {0xD9, 0x60},
    {0xDA, 0xA0},
    {0xD8, 0x80},           
               
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_effect_sharpness_siv100b[] = {
             
        
 
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_effect_sketch_siv100b[] = {
    {0xD8, 0x04},        
        
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

struct sensor_reg* sensor_reg_effect_siv100b[6] =
{
	sensor_effect_normal_siv100b,
	sensor_effect_gray_siv100b,
	sensor_effect_negative_siv100b,
	sensor_effect_sepia_siv100b,
	sensor_effect_sharpness_siv100b,
	sensor_effect_sketch_siv100b,
};


static struct sensor_reg sensor_reg_flipnone_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_reg_hflip_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_reg_vflip_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_reg_hvflip_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

struct sensor_reg* sensor_reg_flip_siv100b[4] =
{
	sensor_reg_flipnone_siv100b,
	sensor_reg_hflip_siv100b,
	sensor_reg_vflip_siv100b,
	sensor_reg_hvflip_siv100b,
};


static struct sensor_reg sensor_secne_auto_siv100b[] = {
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_secne_night_siv100b[] = {
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_secne_landscape_siv100b[] = {
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_secne_portrait_siv100b[] = {
	{SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_secne_sport_siv100b[] = {
	{SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

struct sensor_reg* sensor_reg_scene_siv100b[5] =
{
	sensor_secne_auto_siv100b,
	sensor_secne_night_siv100b,
	sensor_secne_landscape_siv100b,
	sensor_secne_portrait_siv100b,
	sensor_secne_sport_siv100b
};

static struct sensor_reg sensor_me_mtrix_siv100b[] = {
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_me_center_weighted_siv100b[] = {
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_me_spot_siv100b[] = {
	{0x40, 0xA0},
	
    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

struct sensor_reg* sensor_reg_metering_exposure_siv100b[3] =
{
	sensor_me_mtrix_siv100b,
	sensor_me_center_weighted_siv100b,
	sensor_me_spot_siv100b,
};

static struct sensor_reg sensor_af_single_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

static struct sensor_reg sensor_af_manual_siv100b[] = {

    {SIV100B_REG_TERM, SIV100B_VAL_TERM}
};

struct sensor_reg* sensor_reg_af_siv100b[2] =
{
	sensor_af_single_siv100b,
	sensor_af_manual_siv100b,
};

static int write_regs_siv100b(const struct sensor_reg reglist[])
{
	int err;
	int err_cnt = 0;	
	unsigned char data[132];
	unsigned char bytes;
	struct sensor_reg *next = reglist;
	
	while (!((next->reg == SIV100B_REG_TERM) && (next->val == SIV100B_VAL_TERM)))
	{
		bytes = 0;
		data[bytes++] = (unsigned char)(next->reg) & 0xff; 	
		data[bytes++]   = (unsigned char)(next->val) & 0xff;
		
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

#if 0
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
#endif

static int sensor_open_siv100b(bool bChangeCamera)
{
	sensor_power_disable();
	sensor_delay(10);
	
	sensor_power_enable();
	sensor_delay(10);

	CIF_Open();
	sensor_delay(50);
	
	sensor_powerdown_disable();
	sensor_delay(100);
	
	sensor_reset_high();
	sensor_delay(15); // msleep(1);

	return write_regs_siv100b(sensor_reg_common_siv100b[0]);
}

static int sensor_close_siv100b(void)
{
	CIF_ONOFF(OFF);

	//sensor_reset_low();
	sensor_powerdown_enable();
	msleep(50); 

	CIF_Close();   
	msleep(20);  
	
	sensor_power_disable();  

	return 0;
}

static int sensor_powerdown_siv100b(bool bChangeCamera)
{
	sensor_reset_low();
	sensor_power_disable();
	sensor_powerdown_enable();

	return 0;
}

static int sensor_preview_siv100b(void)
{
	return write_regs_siv100b(sensor_reg_common_siv100b[1]);
}

static int sensor_capture_siv100b(void)
{
	return write_regs_siv100b(sensor_reg_common_siv100b[2]);
}

static int sensor_capturecfg_siv100b(int width, int height)
{
	return 0;
}

static int sensor_zoom_siv100b(int val)
{
	return 0;
}

static int sensor_autofocus_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_af_siv100b[val]);
}

static int sensor_effect_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_effect_siv100b[val]);
}

static int sensor_flip_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_flip_siv100b[val]);
}

static int sensor_iso_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_iso_siv100b[val]);
}

static int sensor_me_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_metering_exposure_siv100b[val]);
}

static int sensor_wb_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_awb_siv100b[val]);
}

static int sensor_bright_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_brightness_siv100b[val]);
}

static int sensor_scene_siv100b(int val)
{
	return write_regs_siv100b(sensor_reg_scene_siv100b[val]);
}

static int sensor_check_esd_siv100b(int val)
{
	return 0;
}

static int sensor_check_luma_siv100b(int val)
{
	return 0;
}

/**********************************************************
*    Function  
**********************************************************/
#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void sensor_info_init_siv100b(TCC_SENSOR_INFO_TYPE *sensor_info)
{
	sensor_info->i2c_addr 				= 0x66;
	
	sensor_info->reg_term 				= 0xff;
	sensor_info->val_term 				= 0xff;
	#if defined(CONFIG_ARCH_TCC92XX)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 800000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#elif defined(CONFIG_ARCH_TCC93XX)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 800000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL1;
	sensor_info->s_clock 					= 800000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#endif // CONFIG_USE_ISP
	#elif defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	#if defined(CONFIG_USE_ISP)
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 800000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#else // CONFIG_USE_ISP
	sensor_info->m_clock 				= 240000;
	sensor_info->m_clock_source 			= PCDIRECTPLL2;
	sensor_info->s_clock 					= 800000;
	sensor_info->s_clock_source 			= PCDIRECTPLL2;
	#endif // CONFIG_USE_ISP
	#endif
	sensor_info->preview_w 				= 640;
	sensor_info->preview_h 				= 480;
	sensor_info->preview_zoom_offset_x 	= 8;
	sensor_info->preview_zoom_offset_y 	= 6;
	sensor_info->capture_w 				= 640;
	sensor_info->capture_h 				= 480;
	sensor_info->capture_zoom_offset_x 	= 8;
	sensor_info->capture_zoom_offset_y 	= 6;
	sensor_info->max_zoom_step 			= 24;
	sensor_info->cam_capchg_width 		= 640;
	sensor_info->framerate				= 17;
	sensor_info->p_clock_pol 				= NEGATIVE_EDGE;
	sensor_info->v_sync_pol 				= ACT_HIGH;
	sensor_info->h_sync_pol         = ACT_HIGH;
	#if defined(CONFIG_ARCH_TCC892X)
	sensor_info->de_pol                 = ACT_LOW;
	#endif	
	sensor_info->format 					= M420_ZERO;
	sensor_info->capture_skip_frame 		= 1;
	sensor_info->sensor_sizes 			= sensor_sizes_siv100b;
}

void sensor_init_fnc_siv100b(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 					= sensor_open_siv100b;
	sensor_func->Close 					= sensor_close_siv100b;
	sensor_func->PowerDown				= sensor_powerdown_siv100b;

	sensor_func->Set_Preview 			= sensor_preview_siv100b;
	sensor_func->Set_Capture 			= sensor_capture_siv100b;
	sensor_func->Set_CaptureCfg 			= sensor_capturecfg_siv100b;

	sensor_func->Set_Zoom 				= sensor_zoom_siv100b;
	sensor_func->Set_AF 				= sensor_autofocus_siv100b;
	sensor_func->Set_Effect 				= sensor_effect_siv100b;
	sensor_func->Set_Flip 				= sensor_flip_siv100b;
	sensor_func->Set_ISO 				= sensor_iso_siv100b;
	sensor_func->Set_ME 				= sensor_me_siv100b;
	sensor_func->Set_WB 				= sensor_wb_siv100b;
	sensor_func->Set_Bright 				= sensor_bright_siv100b;
	sensor_func->Set_Scene 				= sensor_scene_siv100b;

	sensor_func->Check_ESD 				= sensor_check_esd_siv100b;
	sensor_func->Check_Luma 			= sensor_check_luma_siv100b;
}
#else
void sensor_init_fnc(SENSOR_FUNC_TYPE *sensor_func)
{
	sensor_func->Open 			= sensor_open_siv100b;
	sensor_func->Close 			= sensor_close_siv100b;
	sensor_func->PowerDown		= sensor_powerdown_siv100b;

	sensor_func->Set_Preview 	= sensor_preview_siv100b;
	sensor_func->Set_Capture 	= sensor_capture_siv100b;
	sensor_func->Set_CaptureCfg = sensor_capturecfg_siv100b;

	sensor_func->Set_Zoom		= sensor_zoom_siv100b;
	sensor_func->Set_AF 		= sensor_autofocus_siv100b;
	sensor_func->Set_Effect 	= sensor_effect_siv100b;
	sensor_func->Set_Flip 		= sensor_flip_siv100b;
	sensor_func->Set_ISO 		= sensor_iso_siv100b;
	sensor_func->Set_ME 		= sensor_me_siv100b;
	sensor_func->Set_WB 		= sensor_wb_siv100b;
	sensor_func->Set_Bright 	= sensor_bright_siv100b;
	sensor_func->Set_Scene 		= sensor_scene_siv100b;

	sensor_func->Check_ESD 		= sensor_check_esd_siv100b;
	sensor_func->Check_Luma 	= sensor_check_luma_siv100b;
}
#endif
#endif

