
/*
 * drivers/media/video/tcccam/sensor_if.c
 *
 * COMMON Sensor driver for  camera sensor interface
 *
 *
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 */

#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/videodev2.h>
//#include <media/video-buf.h>
#include <linux/delay.h>
#include <asm/mach-types.h>
#include <linux/regulator/consumer.h>
#include <linux/spinlock.h>

#include <asm/io.h>

#if defined(CONFIG_ARCH_TCC92XX) || defined(CONFIG_ARCH_TCC93XX) || defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#include <mach/bsp.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#elif defined(CONFIG_ARCH_TCC79X)
#include <mach/tcc79x.h>
#endif
#include "tcc_cam_i2c.h"  
#include <mach/tcc_cam_ioctrl.h>
#include "sensor_if.h"
#include "tdd_cif.h"
#include "cam.h"

#if defined(CONFIG_REGULATOR) && (defined(CONFIG_MACH_TCC9200S_SPACEY) || defined(CONFIG_MACH_TCC9200S_M340EVB))
#if defined(CONFIG_MACH_TCC9200S_M340EVB)
#define VDD_AF_NAME "vdd_cam_af28"
#define VDD_IO_NAME "vdd_cam_io27"
#define VDD_CORE_NAME "vdd_cam_core12"
#else
#define VDD_AF_NAME "vdd_isp_ram18"
#define VDD_IO_NAME "vdd_isp_io28"
#define VDD_CORE_NAME "vdd_isp_core12"
#endif

struct regulator *vdd_cam_core;
struct regulator *vdd_cam_af;
struct regulator *vdd_cam_io;
#endif


static int debug	   = 0;
#define dprintk(msg...)	if (debug) { printk( "Sensor_if: " msg); }

static int enabled = 0;

/* list of image formats supported by sensor sensor */
const static struct v4l2_fmtdesc sensor_formats[] = {
	{
		/* Note:  V4L2 defines RGB565 as:
		 *
		 *	Byte 0			  Byte 1
		 *	g2 g1 g0 r4 r3 r2 r1 r0	  b4 b3 b2 b1 b0 g5 g4 g3
		 *
		 * We interpret RGB565 as:
		 *
		 *	Byte 0			  Byte 1
		 *	g2 g1 g0 b4 b3 b2 b1 b0	  r4 r3 r2 r1 r0 g5 g4 g3
		 */
		.description	= "RGB565, le",
		.pixelformat	= V4L2_PIX_FMT_RGB565,
	},{
		/* Note:  V4L2 defines RGB565X as:
		 *
		 *	Byte 0			  Byte 1
		 *	b4 b3 b2 b1 b0 g5 g4 g3	  g2 g1 g0 r4 r3 r2 r1 r0
		 *
		 * We interpret RGB565X as:
		 *
		 *	Byte 0			  Byte 1
		 *	r4 r3 r2 r1 r0 g5 g4 g3	  g2 g1 g0 b4 b3 b2 b1 b0
		 */
		.description	= "RGB565, be",
		.pixelformat	= V4L2_PIX_FMT_RGB565X,
	},
	{
		.description	= "YUYV (YUV 4:2:2), packed",
		.pixelformat	= V4L2_PIX_FMT_YUYV,
	},{
		.description	= "UYVY, packed",
		.pixelformat	= V4L2_PIX_FMT_UYVY,
	},
	{
		/* Note:  V4L2 defines RGB555 as:
		 *
		 *	Byte 0			  Byte 1
		 *	g2 g1 g0 r4 r3 r2 r1 r0	  x  b4 b3 b2 b1 b0 g4 g3
		 *
		 * We interpret RGB555 as:
		 *
		 *	Byte 0			  Byte 1
		 *	g2 g1 g0 b4 b3 b2 b1 b0	  x  r4 r3 r2 r1 r0 g4 g3
		 */
		.description	= "RGB555, le",
		.pixelformat	= V4L2_PIX_FMT_RGB555,
	},{
		/* Note:  V4L2 defines RGB555X as:
		 *
		 *	Byte 0			  Byte 1
		 *	x  b4 b3 b2 b1 b0 g4 g3	  g2 g1 g0 r4 r3 r2 r1 r0
		 *
		 * We interpret RGB555X as:
		 *
		 *	Byte 0			  Byte 1
		 *	x  r4 r3 r2 r1 r0 g4 g3	  g2 g1 g0 b4 b3 b2 b1 b0
		 */
		.description	= "RGB555, be",
		.pixelformat	= V4L2_PIX_FMT_RGB555X,
	}
};

#define NUM_CAPTURE_FORMATS ARRAY_SIZE(sensor_formats)

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
enum image_size { QQXGA, QXGA, UXGA, SXGA, XGA, SVGA, VGA, QVGA, QCIF };
#define NUM_IMAGE_SIZES 9
#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
#if defined(SENSOR_2M)
enum image_size { UXGA, SXGA, XGA, SVGA, VGA, QVGA, QCIF };
#define NUM_IMAGE_SIZES 7
#elif defined(SENSOR_1_3M)
enum image_size { SXGA, XGA, SVGA, VGA, QVGA, QCIF };
#define NUM_IMAGE_SIZES 6
#elif defined(SENSOR_3M)
enum image_size { QXGA, UXGA, SXGA, XGA, SVGA, VGA, QVGA, QCIF };
#define NUM_IMAGE_SIZES 8
#elif defined(SENSOR_5M)
enum image_size { QQXGA, QXGA, UXGA, SXGA, XGA, SVGA, VGA, QVGA, QCIF };
#define NUM_IMAGE_SIZES 9
#elif defined(SENSOR_VGA)
enum image_size { VGA, sCIF, QVGA, QCIF };
#define NUM_IMAGE_SIZES 4
#elif defined(SENSOR_TVP5150) || defined(SENSOR_RDA5888)
enum image_size { XGA, VGA};
#define NUM_IMAGE_SIZES 2
#endif
#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

enum pixel_format { YUV, RGB565, RGB555 };
#define NUM_PIXEL_FORMATS 3

SENSOR_FUNC_TYPE sensor_func;

//need to revised  MCC
#ifdef CONFIG_ARCH_OMAP24XX
#define NUM_OVERLAY_FORMATS 4
#else
#define NUM_OVERLAY_FORMATS 2
#endif


#define DEF_BRIGHTNESS      	3
#define DEF_ISO      			0
#define DEF_AWB           		0
#define DEF_EFFECT				0
#define DEF_ZOOM				0
#define DEF_FLIP				0
#define DEF_SCENE				0
#define DEF_METERING_EXPOSURE	0
#define DEF_EXPOSURE			0
#define DEF_FOCUSMODE			0
#define DEF_FLASH				0

/*  Video controls  */
static unsigned int need_new_set = 0;
static struct vcontrol {
        struct v4l2_queryctrl qc;
        int current_value;
		int	need_set;
} control[] = {
        { { V4L2_CID_BRIGHTNESS, V4L2_CTRL_TYPE_INTEGER, "Brightness", 0, 4, 1,
            DEF_BRIGHTNESS },
          0, 0},
          
        { { V4L2_CID_AUTO_WHITE_BALANCE, V4L2_CTRL_TYPE_INTEGER, "Auto White Balance", 0,5,1,
            DEF_AWB },
          0, 0},

        { { V4L2_CID_ISO, V4L2_CTRL_TYPE_INTEGER, "Iso", 0, 3, 1,
            DEF_ISO },
          0, 0},
          
        { { V4L2_CID_EFFECT, V4L2_CTRL_TYPE_INTEGER, "Special Effect", 0, 6, 1,
            DEF_EFFECT },
          0, 0},
          
        { { V4L2_CID_ZOOM, V4L2_CTRL_TYPE_INTEGER, "Zoom", 0, 24, 1,
            DEF_ZOOM },
          0, 0},    
                    
        { { V4L2_CID_FLIP, V4L2_CTRL_TYPE_INTEGER, "Flip", 0, 3, 1,
            DEF_FLIP },
          0, 0},
          
        { { V4L2_CID_SCENE, V4L2_CTRL_TYPE_INTEGER, "Scene", 0, 5, 1,
            DEF_SCENE},
          0, 0},  
          
        { { V4L2_CID_METERING_EXPOSURE, V4L2_CTRL_TYPE_INTEGER, "Metering Exposure", 0, 3, 1,
            DEF_METERING_EXPOSURE},
          0, 0},

		 { { V4L2_CID_EXPOSURE, V4L2_CTRL_TYPE_INTEGER, "Exposure", -20, 20, 5,
            DEF_EXPOSURE},
          0, 0},

		{ { V4L2_CID_FOCUS_MODE, V4L2_CTRL_TYPE_INTEGER, "AutoFocus", 0, 1, 2,
            DEF_FOCUSMODE},
          0, 0},
          
        { { V4L2_CID_FLASH, V4L2_CTRL_TYPE_INTEGER, "Flash", 0, 4, 1,
            DEF_FLASH},
          0, 0},	
};


unsigned int sensor_control[] = 
{
	V4L2_CID_BRIGHTNESS,
	V4L2_CID_AUTO_WHITE_BALANCE,
	V4L2_CID_ISO,
	V4L2_CID_EFFECT,
	V4L2_CID_ZOOM,
	V4L2_CID_FLIP,
	V4L2_CID_SCENE,
	V4L2_CID_METERING_EXPOSURE,
	V4L2_CID_EXPOSURE,
	V4L2_CID_FOCUS_MODE
	//V4L2_CID_FLASH
};

#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
extern TCC_SENSOR_INFO_TYPE tcc_sensor_info;
extern int CameraID;

spinlock_t sensor_lock = __SPIN_LOCK_UNLOCKED(sensor_lock);
#endif

#ifndef USE_SENSOR_EFFECT_IF
extern int tccxxx_cif_set_effect (u8 nCameraEffect);
#endif
#ifndef USE_SENSOR_ZOOM_IF
extern int tccxxx_cif_set_zoom(unsigned char arg);
#endif

int sensor_if_change_mode(unsigned char capture_mode)
{
    int ret=0;

	if(capture_mode)
		ret = sensor_func.Set_Capture();
	else
		ret = sensor_func.Set_Preview();
	
    return ret;
}

int sensor_if_adjust_autofocus(void)
{
	return sensor_func.Set_AF(0);
}

/* Returns the index of the requested ID from the control structure array */
static int find_vctrl(int id)
{
	int i;
	int ctrl_cnt = ARRAY_SIZE(control);

	if (id < V4L2_CID_BASE)
		return -EDOM;

	for (i = ctrl_cnt - 1; i >= 0; i--)
	{
		if (control[i].qc.id == id)
			break;
	}

	if (i < 0)
		i = -EINVAL;
	
	return i;
}

/* initialize Sensor after module was reset. */
int sensor_init_module(enum image_size isize, enum pixel_format pfmt,unsigned long xclk)
{
	return 0;
}


/* Find the best match for a requested image capture size.  The best match 
 * is chosen as the nearest match that has the same number or fewer pixels 
 * as the requested size, or the smallest image size if the requested size 
 * has fewer pixels than the smallest image.
 */
static enum image_size sensor_find_size(unsigned int width, unsigned int height)
{
	enum image_size isize;
	unsigned long pixels = width*height;

	for(isize=0; isize < (NUM_IMAGE_SIZES-1); isize++)
	{
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		if((tcc_sensor_info.sensor_sizes[isize + 1].height * tcc_sensor_info.sensor_sizes[isize + 1].width) > pixels)		
		#else
		if((sensor_sizes[isize + 1].height * sensor_sizes[isize + 1].width) > pixels)
		#endif
		{
			return isize;
		}
	}
	#if defined(CONFIG_VIDEO_ATV_SENSOR_TVP5150) || defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888) \
	|| defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690) || defined(CONFIG_VIDEO_CAMERA_SENSOR_SIV100B)
	return VGA;
	#else
	return SXGA;
	#endif
}

/* following are sensor interface functions implemented by 
 * sensor driver.
 */
int sensor_if_query_control(struct v4l2_queryctrl *qc)
{
	int i;

	i = find_vctrl (qc->id);
	if(i == -EINVAL) {
		qc->flags = V4L2_CTRL_FLAG_DISABLED;
		return 0;
	}
	
	if(i < 0) 		return -EINVAL;
	*qc = control[i].qc;
	
	return 0;
}

int sensor_if_get_control(struct v4l2_control *vc)
{
	int i;
	struct vcontrol * lvc;
	
	if((i = find_vctrl(vc->id)) < 0)
		return -EINVAL;

	lvc = &control[i];	
	vc->value = lvc->current_value;
	
	return 0;
}

int sensor_if_set_control(struct v4l2_control *vc, unsigned char init)
{	
	struct vcontrol *lvc;
	int val = vc->value;
	int i;
	
	if((i = find_vctrl(vc->id)) < 0) 	return -EINVAL;
	lvc = &control[i];
	if(lvc->qc.maximum < val) 			return val;
	if(lvc->current_value != val || init) {
		#if defined(CONFIG_ARCH_TCC892X)
			if(lvc->qc.id == V4L2_CID_ZOOM) {
				tccxxx_cif_set_zoom(val);
			} else {
				lvc->current_value = val;
				lvc->need_set = 1;
				need_new_set = 1;
			}
		#else
		lvc->current_value = val;
		lvc->need_set = 1;
		need_new_set = 1;
		#endif
	}

	return 0;
}

int sensor_if_check_control(void)
{
	int i, err;
	struct vcontrol *lvc;
	int ctrl_cnt = ARRAY_SIZE(control);

	if(!need_new_set) 	return 0;
	for(i=(ctrl_cnt - 1); i >= 0; i--)
	{
		lvc = &control[i];
		if(lvc->need_set)
		{
			switch(lvc->qc.id)
			{
				case V4L2_CID_BRIGHTNESS:
					err = sensor_func.Set_Bright(lvc->current_value);
					break;

				case V4L2_CID_AUTO_WHITE_BALANCE:
					err = sensor_func.Set_WB(lvc->current_value);
					break;

				case V4L2_CID_ISO:
					err = sensor_func.Set_ISO(lvc->current_value);
					break;

				case V4L2_CID_EFFECT:
					#ifdef USE_SENSOR_EFFECT_IF
					err = sensor_func.Set_Effect(lvc->current_value);
					#else
					err = tccxxx_cif_set_effect(lvc->current_value);
					#endif			
					break;

				case V4L2_CID_ZOOM:
					#ifdef USE_SENSOR_ZOOM_IF
					err = sensor_func.Set_Zoom(lvc->current_value);
					#else
					err = tccxxx_cif_set_zoom(lvc->current_value);
					#endif
					break;

				case V4L2_CID_FLIP:
					err = sensor_func.Set_Flip(lvc->current_value);
					break;

				case V4L2_CID_SCENE:
					err = sensor_func.Set_Scene(lvc->current_value);
					break;

				case V4L2_CID_METERING_EXPOSURE:
					err = sensor_func.Set_ME(lvc->current_value);
					break;

				case V4L2_CID_FLASH:
					// todo: 
					break;

				case V4L2_CID_EXPOSURE:
					err = sensor_func.Set_Exposure(lvc->current_value);
					break;

				case V4L2_CID_FOCUS_MODE:
					err = sensor_func.Set_FocusMode(lvc->current_value);
					break;

				default:
					break;
			}
			lvc->need_set = 0;
		}
	}

	return 0;
}

/* In case of ESD-detection, to set current value in sensor after module was resetted. */
int sensor_if_set_current_control(void)
{
	struct v4l2_control vc;
	int i;
	int ctrl_cnt = ARRAY_SIZE(sensor_control);

	printk("Setting Sensor-ctrl!! %d /n", ctrl_cnt);
	
	for (i = ctrl_cnt - 1; i >= 0; i--)
	{
		vc.id = sensor_control[i];
		sensor_if_get_control(&vc);
		sensor_if_set_control(&vc, 1);
	}

	return 0;
}

/* Implement the VIDIOC_ENUM_FMT ioctl for the CAPTURE buffer type.
 */
int sensor_if_enum_pixformat(struct v4l2_fmtdesc *fmt)
{
	int index = fmt->index;
	enum v4l2_buf_type type = fmt->type;

	memset(fmt, 0, sizeof(*fmt));
	fmt->index = index;
	fmt->type = type;

	switch (fmt->type) {
		case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		if (index >= NUM_CAPTURE_FORMATS)
			return -EINVAL;
		break;

		case V4L2_BUF_TYPE_VIDEO_OVERLAY:
		if (index >= NUM_OVERLAY_FORMATS)
			return -EINVAL;
		break;

		default:
			return -EINVAL;
	}

	fmt->flags = sensor_formats[index].flags;
	strlcpy(fmt->description, sensor_formats[index].description, sizeof(fmt->description));
	fmt->pixelformat = sensor_formats[index].pixelformat;

	return 0;
}

/* Implement the VIDIOC_TRY_FMT ioctl for the CAPTURE buffer type.  This 
 * ioctl is used to negotiate the image capture size and pixel format 
 * without actually making it take effect.
 */
static int sensor_try_format(struct v4l2_pix_format *pix)
{
	enum image_size isize;
	int ifmt;

	isize = sensor_find_size(pix->width, pix->height);
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	pix->width  = tcc_sensor_info.sensor_sizes[isize].width;
	pix->height = tcc_sensor_info.sensor_sizes[isize].height;
	#else
	pix->width  = sensor_sizes[isize].width;
	pix->height = sensor_sizes[isize].height;
	#endif

	for(ifmt=0; ifmt < NUM_CAPTURE_FORMATS; ifmt++) 
	{
		if(pix->pixelformat == sensor_formats[ifmt].pixelformat)
			break;
	}
	
	if(ifmt == NUM_CAPTURE_FORMATS)
		ifmt = 0;
	
	pix->pixelformat = sensor_formats[ifmt].pixelformat;
	pix->field = V4L2_FIELD_NONE;
	pix->bytesperline = pix->width*2;
	pix->sizeimage = pix->bytesperline*pix->height;
	pix->priv = 0;
	
	switch(pix->pixelformat) {
		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_UYVY:
		default:
			pix->colorspace = V4L2_COLORSPACE_JPEG;
			break;
			
		case V4L2_PIX_FMT_RGB565:
		case V4L2_PIX_FMT_RGB565X:
		case V4L2_PIX_FMT_RGB555:
		case V4L2_PIX_FMT_RGB555X:
			pix->colorspace = V4L2_COLORSPACE_SRGB;
			break;
	}
	
	return 0;
}



/* Given a capture format in pix, the frame period in timeperframe, and
 * the xclk frequency, set the capture format of the sensor.
 * The actual frame period will be returned in timeperframe.
 */
int sensor_if_configure(struct v4l2_pix_format *pix, unsigned long xclk)
{
  	enum pixel_format pfmt = YUV;

	switch (pix->pixelformat) {
		case V4L2_PIX_FMT_RGB565:
		case V4L2_PIX_FMT_RGB565X:
			pfmt = RGB565;
			break;
		case V4L2_PIX_FMT_RGB555:
		case V4L2_PIX_FMT_RGB555X:
			pfmt = RGB555;
			break;
		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_UYVY:
		default:
			pfmt = YUV;
 	}

	return sensor_init_module(sensor_find_size(pix->width, pix->height), pfmt, xclk);
}


/* Initialize the sensor.
 * This routine allocates and initializes the data structure for the sensor, 
 * powers up the sensor, registers the I2C driver, and sets a default image 
 * capture format in pix.  The capture format is not actually programmed 
 * into the sensor by this routine.
 * This function must return a non-NULL value to indicate that 
 * initialization is successful.
 */
int sensor_if_init(struct v4l2_pix_format *pix)
{
 	int ret, gpio_ret;

	// set to initialize normal gpio that power, power_down(or stand-by) and reset of sensor.
	#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900())
	{
		// set power.
		gpio_request(TCC_GPEXT2(3), NULL);
		gpio_direction_output(TCC_GPEXT2(3), 1);

		// set power_down(or stand-by).
		gpio_request(TCC_GPF(26), NULL);
		gpio_direction_output(TCC_GPF(26), 1);

		// set reset.
		gpio_request(TCC_GPE(26), NULL);
		gpio_direction_output(TCC_GPE(26), 0);
	}
	else if(machine_is_m801())
	{
		// customer coding part.
		// if you want to control the power, power_down and reset, this part should be coding.
		
		#if defined(PRODUCT_IS_COBY)
		// set power.
		// todo : 

		// set power_down(or stand-by).
		// todo :
		
		// set reset.
		// todo : 
		#elif defined(PRODUCT_IS_EMDOOR)
		// set power.
		// todo : 

		// set power_down(or stand-by).
		// todo :
		
		// set reset.
		// todo : 
		#else // original m801 by tcc.
		// set power.
		gpio_request(TCC_GPD(10), NULL);
		gpio_direction_output(TCC_GPD(10), 1);

		// set power_down(or stand-by).
		gpio_request(TCC_GPF(21), NULL);
		gpio_direction_output(TCC_GPF(21), 1);

		// set reset.
		gpio_request(TCC_GPE(2), NULL);
		gpio_direction_output(TCC_GPE(2), 0);
		#endif
	}
	else
	{
		// todo : etc product.
	}
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	// set power.
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			gpio_request(TCC_GPG(4), NULL);
			gpio_direction_output(TCC_GPG(4), 1);
		}
		else
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				gpio_request(TCC_GPEXT1(12), NULL);
				gpio_direction_output(TCC_GPEXT1(12), 1);
			#endif		
			gpio_request(TCC_GPEXT1(13), NULL);
			gpio_direction_output(TCC_GPEXT1(13), 1);
		}
	}
	else if(machine_is_m801_88() || (system_rev==0x0003 || system_rev==0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		// customer coding part.
		// if you want to control the power, power_down and reset, this part should be coding.
		
		#if defined(PRODUCT_IS_COBY)
		// set power.
		// todo : 
		#elif defined(PRODUCT_IS_EMDOOR)
		// set power.
		// todo : 
		#else // original m801 by tcc.
			#if defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
			gpio_request(TCC_GPF(10), NULL);
			gpio_direction_output(TCC_GPF(10), 1);

			gpio_request(TCC_GPF(14), NULL);
			gpio_direction_output(TCC_GPF(14), 1);
			#else
			gpio_request(TCC_GPF(10), NULL);
			gpio_direction_output(TCC_GPF(10), 1);
			#endif
		#endif
	}
	else
	{
		// todo : etc product.
	}

	// set power_down(or stand-by).
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPG(3), NULL);
					gpio_direction_output(TCC_GPG(3), 1);				
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPE(3), NULL);
					gpio_direction_output(TCC_GPE(3), 1);				
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPE(3), NULL);
					gpio_direction_output(TCC_GPE(3), 1);
				#else
					gpio_request(TCC_GPG(3), NULL);
					gpio_direction_output(TCC_GPG(3), 1);	
				#endif
			#endif			
	
		}
		else if(machine_is_tcc8800() || (system_rev==0x0003 || system_rev==0x0004 || system_rev==0x0005 || system_rev == 0x0006))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
				gpio_request(TCC_GPG(31), NULL);
				gpio_direction_output(TCC_GPG(31), 1);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
				gpio_request(TCC_GPEXT3(6), NULL);
				gpio_direction_output(TCC_GPEXT3(6), 1);
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPEXT3(6), NULL);
					gpio_direction_output(TCC_GPEXT3(6), 1);
				#else					
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P113)
						gpio_request(TCC_GPG(31), NULL);
						gpio_direction_output(TCC_GPG(31), 1);
					#else
						gpio_request(TCC_GPE(2), NULL);
						gpio_direction_output(TCC_GPE(2), 1);
					#endif
				#endif
			#endif
		}
		else if(machine_is_tcc9300())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
				gpio_request(TCC_GPE(2), NULL);
				gpio_direction_output(TCC_GPE(2), 1);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
				gpio_request(TCC_GPEXT3(6), NULL);
				gpio_direction_output(TCC_GPEXT3(6), 1);
				#endif
			}
			#else
			# 	if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPEXT3(6), NULL);
					gpio_direction_output(TCC_GPEXT3(6), 1);
			# 	else
					gpio_request(TCC_GPE(24), NULL);
					gpio_direction_output(TCC_GPE(24), 1);
			# 	endif
			#endif
		}
		else
		{
			// todo : 
		}
	}
	else if(machine_is_m801_88() || (system_rev==0x0003 || system_rev==0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		// customer coding part.
		// if you want to control the power, power_down and reset, this part should be coding.
		
		#if defined(PRODUCT_IS_COBY)
		// set power_down(or stand-by).
		// todo : 
		#elif defined(PRODUCT_IS_EMDOOR)
		// set power_down(or stand-by).
		// todo : 
		#else // original m801 by tcc.
		gpio_request(TCC_GPF(11), NULL);
		gpio_direction_output(TCC_GPF(11), 1);
		#endif
	}
	else if(machine_is_m803())
	{
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
				gpio_request(TCC_GPF(10), NULL);
				gpio_direction_output(TCC_GPF(10), 1);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9D112)
				gpio_request(TCC_GPF(11), NULL);
				gpio_direction_output(TCC_GPF(11), 1);
				#endif
			}
		#endif
		
	}
	else
	{
		// todo : etc product.
	}

	// set reset.
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPG(2), NULL);
					gpio_direction_output(TCC_GPG(2), 0);			
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPE(2), NULL);
					gpio_direction_output(TCC_GPE(2), 0);				
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPE(2), NULL);
					gpio_direction_output(TCC_GPE(2), 0);					
				#else
					gpio_request(TCC_GPG(2), NULL);
					gpio_direction_output(TCC_GPG(2), 0);	
				#endif
			#endif			
	
		}
		else if(machine_is_tcc8800() || (system_rev==0x0003 || system_rev==0x0004 || system_rev==0x0005 || system_rev == 0x0006))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
				gpio_request(TCC_GPG(30), NULL);
			gpio_direction_output(TCC_GPG(30), 0);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
				gpio_request(TCC_GPEXT1(12), NULL);
				gpio_direction_output(TCC_GPEXT1(12), 0);
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPEXT1(12), NULL);
					gpio_direction_output(TCC_GPEXT1(12), 0);
				#else
					gpio_request(TCC_GPG(30), NULL);
					gpio_direction_output(TCC_GPG(30), 0);
				#endif
			#endif
		}
		else if(machine_is_tcc9300())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
				gpio_request(TCC_GPE(25), NULL);
			gpio_direction_output(TCC_GPE(25), 0);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
				gpio_request(TCC_GPEXT1(12), NULL);
				gpio_direction_output(TCC_GPEXT1(12), 0);
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPEXT1(12), NULL);
					gpio_direction_output(TCC_GPEXT1(12), 0);
				#else
					gpio_request(TCC_GPE(25), NULL);
					gpio_direction_output(TCC_GPE(25), 0);
				#endif
			#endif
		}	
		else
		{
			// todo : 
		}
	}
	else if(machine_is_m801_88() && (system_rev != 0x0003 && system_rev != 0x0004 && system_rev != 0x0005 && system_rev != 0x0006))
	{
		// customer coding part.
		// if you want to control the power, power_down and reset, this part should be coding.
		
		#if defined(PRODUCT_IS_COBY)
		// set reset.
		// todo : 
		#elif defined(PRODUCT_IS_EMDOOR)
		// set reset.
		// todo : 
		#else // original m801 by tcc.
		gpio_request(TCC_GPG(30), NULL);
		gpio_direction_output(TCC_GPG(30), 0);
		#endif
	}
	else if(machine_is_m803() || (system_rev == 0x0003 || system_rev == 0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		//gpio_request(TCC_GPD(12), NULL);
		//gpio_direction_output(TCC_GPD(12), 0);
		gpio_request(TCC_GPE(15), NULL);
		gpio_direction_output(TCC_GPE(15), 0);
	}
	else
	{
		// todo : etc product.
	}
	#elif	defined(CONFIG_ARCH_TCC892X)
	{
		dprintk("[TCC892x] gpio init start!\n");
		#if defined(CONFIG_MACH_M805_892X)
			// Power doesn't need it.

			#if	defined(CONFIG_M805S_8925_0XX)
				// Powerdown
				gpio_request(TCC_GPD(1), NULL);
				gpio_direction_output(TCC_GPD(1), 1);
				
				// Reset
				gpio_request(TCC_GPF(15), NULL);
				gpio_direction_output(TCC_GPF(15), 0);

			#else
				if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005) {
					// Powerdown
					gpio_request(TCC_GPD(1), NULL);
					gpio_direction_output(TCC_GPD(1), 1);
					
					// Reset
					gpio_request(TCC_GPF(15), NULL);
					gpio_direction_output(TCC_GPF(15), 0);
				}
				else {
					#ifdef CONFIG_M805S_8923_0XA
						gpio_request(TCC_GPE(3), NULL);
						gpio_direction_output(TCC_GPE(3), 1);
					#endif
					
					// Powerdown
					gpio_request(TCC_GPE(22), NULL);
					gpio_direction_output(TCC_GPE(22), 1);

					// Reset
					gpio_request(TCC_GPE(10), NULL);
					gpio_direction_output(TCC_GPE(10), 0);
				}
			#endif
		#else

			if(system_rev == 0x1005 || system_rev == 0x1007){
				// In case of CAM0, Power
				gpio_request(TCC_GPF(24), NULL);				
				gpio_direction_output(TCC_GPF(24), 1);

				// In case of CAM4, PowerDown -> CameraID[0] = CAM0_STDBY, CameraID[1] = CAM0_FL_EN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPF(15), NULL);
					gpio_direction_output(TCC_GPF(15), 1);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPF(30), NULL);			
					gpio_direction_output(TCC_GPF(30), 1);		
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPF(30), NULL);
						gpio_direction_output(TCC_GPF(30), 1);					
					#else
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_request(TCC_GPF(15), NULL);
							gpio_direction_output(TCC_GPF(15), 1);						
						#else
							gpio_request(TCC_GPF(13), NULL);
							gpio_direction_output(TCC_GPF(13), 1);
						#endif
					#endif

				#endif

				// In case of CAM0, Reset -> CameraID[0] = CAM0_RST, CameraID[1] = CAM0_PWDN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPF(14), NULL);
					gpio_direction_output(TCC_GPF(14), 0);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPF(13), NULL);
					gpio_direction_output(TCC_GPF(13), 0);
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPF(13), NULL);
						gpio_direction_output(TCC_GPF(13), 0);
					#else
						gpio_request(TCC_GPF(14), NULL);
						gpio_direction_output(TCC_GPF(14), 0);
					#endif

				#endif
			}
			else if(system_rev == 0x1006){
				// In case of CAM0, Power
				gpio_request(TCC_GPD(3), NULL);				
				gpio_direction_output(TCC_GPD(3), 1);

				// In case of CAM4, PowerDown -> CameraID[0] = CAM0_STDBY, CameraID[1] = CAM0_FL_EN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPD(2), NULL);
					gpio_direction_output(TCC_GPD(2), 1);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPG(1), NULL);			
					gpio_direction_output(TCC_GPG(1), 1);		
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPG(1), NULL);
						gpio_direction_output(TCC_GPG(1), 1);					
					#else
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_request(TCC_GPD(2), NULL);
							gpio_direction_output(TCC_GPD(2), 1);						
						#else
							gpio_request(TCC_GPD(1), NULL);
							gpio_direction_output(TCC_GPD(1), 1);
						#endif
					#endif

				#endif

				// In case of CAM0, Reset -> CameraID[0] = CAM0_RST, CameraID[1] = CAM0_PWDN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPF(15), NULL);
					gpio_direction_output(TCC_GPF(15), 0);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPD(1), NULL);
					gpio_direction_output(TCC_GPD(1), 0);
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPD(1), NULL);
						gpio_direction_output(TCC_GPD(1), 0);
					#else
						gpio_request(TCC_GPF(15), NULL);
						gpio_direction_output(TCC_GPF(15), 0);
					#endif

				#endif
				
			}
			else if(system_rev == 0x1008){
				// In case of CAM0, Power
				gpio_request(TCC_GPC(6), NULL);				
				gpio_direction_output(TCC_GPC(6), 1);

				// In case of CAM4, PowerDown -> CameraID[0] = CAM0_STDBY, CameraID[1] = CAM0_FL_EN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPC(5), NULL);
					gpio_direction_output(TCC_GPC(5), 1);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPC(28), NULL);			
					gpio_direction_output(TCC_GPC(28), 1);		
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPC(28), NULL);
						gpio_direction_output(TCC_GPC(28), 1);					
					#else
						//#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_request(TCC_GPC(5), NULL);
							gpio_direction_output(TCC_GPC(5), 1);						
						//#else
							//gpio_request(TCC_GPD(1), NULL);
							//gpio_direction_output(TCC_GPD(1), 1);
						//#endif
					#endif

				#endif

				// In case of CAM0, Reset -> CameraID[0] = CAM0_RST, CameraID[1] = CAM0_PWDN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPC(4), NULL);
					gpio_direction_output(TCC_GPD(4), 0);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPC(8), NULL);
					gpio_direction_output(TCC_GPC(8), 0);
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPC(8), NULL);
						gpio_direction_output(TCC_GPC(8), 0);
					#else
						gpio_request(TCC_GPC(4), NULL);
						gpio_direction_output(TCC_GPC(4), 0);
					#endif

				#endif
				
			}
			else{
				// In case of CAM0, Power
				gpio_request(TCC_GPD(21), NULL);
				gpio_direction_output(TCC_GPD(21), 1);

				// In case of CAM4, PowerDown -> CameraID[0] = CAM0_STDBY, CameraID[1] = CAM0_FL_EN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPD(16), NULL);
					gpio_direction_output(TCC_GPD(16), 1);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPC(29), NULL);
					gpio_direction_output(TCC_GPC(29), 1);		
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPC(29), NULL);
						gpio_direction_output(TCC_GPC(29), 1);					
					#else
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_request(TCC_GPD(16), NULL);
							gpio_direction_output(TCC_GPD(16), 1);						
						#else
							gpio_request(TCC_GPF(13), NULL);
							gpio_direction_output(TCC_GPF(13), 1);
						#endif
					#endif

				#endif

				// In case of CAM0, Reset -> CameraID[0] = CAM0_RST, CameraID[1] = CAM0_PWDN
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_request(TCC_GPF(14), NULL);
					gpio_direction_output(TCC_GPF(14), 0);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
					gpio_request(TCC_GPF(13), NULL);
					gpio_direction_output(TCC_GPF(13), 0);
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_request(TCC_GPF(13), NULL);
						gpio_direction_output(TCC_GPF(13), 0);
					#else
						gpio_request(TCC_GPF(14), NULL);
						gpio_direction_output(TCC_GPF(14), 0);
					#endif

				#endif
			}
		#endif
		dprintk("[TCC892x] gpio init end!\n");
	}	
	#endif

	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	if(CameraID){
			spin_lock(&sensor_lock);
			CameraID = 0;

			if(!sensor_get_powerdown()){
				dprintk("First mt9p111 powerdown! \n");
				sensor_init_func_set_facing_back(&sensor_func);
		
				
				/* Go To PowerDown Mode */
				if(sensor_func.PowerDown(false) < 0)
				{
					printk("mt9p111 close error! \n");
					return -1;
				}
				
				
			}	
			CameraID = 1;
			spin_unlock(&sensor_lock);
	
			sensor_init_func_set_facing_front(&sensor_func);
					
	}
	else{
			spin_lock(&sensor_lock);
			CameraID = 1;
		
			if(!sensor_get_powerdown()){
				dprintk("First mt9m113 powerdown! \n");
				sensor_init_func_set_facing_front(&sensor_func);

				
				/* Go To PowerDown Mode */
				if(sensor_func.PowerDown(false) < 0)
				{
					printk("mt9m113 close error! \n");
					return -1;
				}				
				
			}
			
			CameraID = 0;
			spin_unlock(&sensor_lock);
		

			sensor_init_func_set_facing_back(&sensor_func);
	
	}
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	sensor_init_fnc(&sensor_func);
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT

	TDD_CIF_Initialize();

	if(!enabled)
	{
		#if !defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
		if((ret = DDI_I2C_Initialize()) < 0)
		{
			return -1;
		}
		enabled = 1;
		#endif
	}
	
	/* common register initialization */
	if(sensor_func.Open(false) < 0)
	{
		return -1;
	}
	
	/* Make the default capture format QVGA YUV422 */
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	pix->width = 640; // tcc_sensor_info.sensor_sizes[0].width;
	pix->height = 480; // tcc_sensor_info.sensor_sizes[0].height;
	#else
	pix->width = sensor_sizes[VGA].width;
	pix->height = sensor_sizes[VGA].height;
	#endif

	pix->pixelformat = V4L2_PIX_FMT_YUYV;
	sensor_try_format(pix);

	need_new_set = 0;
	
	return 0;
}

int sensor_if_deinit(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	// todo : 
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	// todo : 
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					
		#endif
	#elif defined(CONFIG_ARCH_TCC892X)
		// In case of 892X, we have to add
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					
		#endif		
	#else
	// todo : 
	#endif

	return 0;
}

void sensor_pwr_control(int name, int enable)
{
#if defined(CONFIG_REGULATOR) && (defined(CONFIG_MACH_TCC9200S_SPACEY) || defined(CONFIG_MACH_TCC9200S_M340EVB))
	if(enable)
	{
		switch(name)
		{
			case CAM_CORE:
				if (vdd_cam_core)
					regulator_enable(vdd_cam_core);
				break;

			case CAM_ETC:
				if (vdd_cam_af)
					regulator_enable(vdd_cam_af);
				break;

			case CAM_IO:
				if (vdd_cam_io)
					regulator_enable(vdd_cam_io);
				break;

			default:
				break;
		}
	}
	else
	{
		switch(name)
		{
			case CAM_CORE:
				if (vdd_cam_core)
					regulator_disable(vdd_cam_core);
				break;

			case CAM_ETC:
				if (vdd_cam_af)
					regulator_disable(vdd_cam_af);
				break;

			case CAM_IO:
				if (vdd_cam_io)
					regulator_disable(vdd_cam_io);
				break;

			default:
				break;
		}
		
	}	
#endif
}

#if defined(CONFIG_VIDEO_CAMERA_SENSOR_AIT848_ISP)
void sensor_if_set_enable(void)
{
	DDI_I2C_Initialize();
	enabled = 1;
}

int sensor_if_get_enable(void)
{
	return enabled;
}
#endif

int sensor_if_get_max_resolution(int index)
{
	// parameter : index는 나중에 사용할 수도 있음.
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
	switch(tcc_sensor_info.sensor_sizes[0].width) {
		case 2560: return QQXGA; 	// 5M size
		case 2048: return QXGA; 		// 3M size
		case 1600: return UXGA; 		// 2M size
		case 1280: return SXGA;		//1.3M size
		case 720: return XGA; 		// analog tv
		case 640: return VGA; 		// VGA
	}
	#else // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
	#if defined(SENSOR_5M)
	return 0; // QQXGA
	#elif defined(SENSOR_3M)
	return 1; // QXGA
	#elif defined(SENSOR_2M)
	return 2; // UXGA
	#elif defined(SENSOR_1_3M)
	return 3; // SXGA
	#elif defined(SENSOR_TVP5150) || defined(SENSOR_RDA5888)
	return 4; // XGA
	#elif defined(SENSOR_VGA)
	return 6; // VGA
	#endif
	#endif // CONFIG_VIDEO_DUAL_CAMERA_SUPPORT
}

int sensor_if_get_sensor_framerate(int *nFrameRate)
{
	#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
		*nFrameRate = tcc_sensor_info.framerate;
	#else
		*nFrameRate = SENSOR_FRAMERATE;
	#endif

	if(*nFrameRate)
		return 0;
	else{
		printk("Sensor Driver dosen't have frame rate information!!\n");
		return -1;
	}
}


#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
void sensor_if_set_facing_front(void)
{
	#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
		sensor_info_init_mt9m113(&tcc_sensor_info);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_SR130PC10)
		sensor_info_init_sr130pc10(&tcc_sensor_info);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
		sensor_info_init_ov7690(&tcc_sensor_info);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_SIV100B)
		sensor_info_init_siv100b(&tcc_sensor_info);
	#endif
}

void sensor_if_set_facing_back(void)
{
	#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
		sensor_info_init_mt9p111(&tcc_sensor_info);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_S5K5CAGA)
		sensor_info_init_s5k5caga(&tcc_sensor_info);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
		sensor_info_init_mt9t111(&tcc_sensor_info);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9D112)
		sensor_info_init_mt9d112(&tcc_sensor_info);
	#endif
}

void sensor_init_func_set_facing_front(SENSOR_FUNC_TYPE *sensor_func)
{
	#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
		sensor_init_fnc_mt9m113(sensor_func);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_SR130PC10)
		sensor_init_fnc_sr130pc10(sensor_func);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
		sensor_init_fnc_ov7690(sensor_func);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_SIV100B)
		sensor_init_fnc_siv100b(sensor_func);	
	#endif
}

void sensor_init_func_set_facing_back(SENSOR_FUNC_TYPE *sensor_func)
{
	#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111)
		sensor_init_fnc_mt9p111(sensor_func);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_S5K5CAGA)
		sensor_init_fnc_s5k5caga(sensor_func);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
		sensor_init_fnc_mt9t111(sensor_func);
	#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9D112)
		sensor_init_fnc_mt9d112(sensor_func);
	#endif
}
#endif

int sensor_if_restart(void)
{
	sensor_func.Open(false);
	return 0;
}

int sensor_if_capture_config(int width, int height)
{
	return sensor_func.Set_CaptureCfg(width, height);
}

int sensor_if_isESD(void)
{
	return sensor_func.Check_ESD(0);
}
/* Prepare for the driver to exit.
 * Balances sensor_if_init().
 * This function must de-initialize the sensor and its associated data 
 * structures.
 */
int sensor_if_cleanup(void)
{
	dprintk("enabled = [%d]\n", enabled);
	
	if(enabled)
	{
		sensor_func.Close();
		TDD_CIF_Termination();
		DDI_I2C_Terminate();
		enabled = 0;
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

void sensor_turn_on_camera_flash(void)
{
	#if	defined(CONFIG_ARCH_TCC892X)
	//	In case of 892X, we have to add.

	#else
		if(machine_is_m803()){
			gpio_request(TCC_GPA(6), NULL);
			gpio_direction_output(TCC_GPA(6), 1);

			gpio_set_value(TCC_GPA(6), 1);
		}
	#endif
}

void sensor_turn_off_camera_flash(void)
{
	#if	defined(CONFIG_ARCH_TCC892X)
		//	In case of 892X, we have to add.

	#else
		if(machine_is_m803())
			gpio_set_value(TCC_GPA(6), 0);
	#endif
}

void sensor_power_enable(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900())
	{
		//gpio_set_value(TCC_GPEXT2(3), 1);
	}
	else if(machine_is_m801())
	{
		//gpio_set_value(TCC_GPD(10), 1);
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			//gpio_set_value(TCC_GPG(4), 1);
		}
		else
		{
			#if defined(TCC_POWER_CONTROL)
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					gpio_set_value(TCC_GPEXT1(12), 1);
				#endif
				gpio_set_value(TCC_GPEXT1(13), 1);
			#endif
		}
	}
	else if(machine_is_m801_88() || (system_rev == 0x0003 || system_rev == 0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		#if defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
		//gpio_set_value(TCC_GPF(10), 0);
		//gpio_set_value(TCC_GPF(14), 1);
		#else
		//gpio_set_value(TCC_GPF(10), 1);
		#endif
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC892X)
		#if defined(CONFIG_MACH_M805_892X)
			#ifdef CONFIG_M805S_8923_0XA
				gpio_set_value(TCC_GPE(3), 1);
			#endif	
		#else
			if(system_rev == 0x1005 || system_rev == 0x1007){
				// In Case of CAM0
				gpio_set_value(TCC_GPF(24), 1);
			}
			else if(system_rev == 0x1006){
				// In Case of CAM0
				gpio_set_value(TCC_GPD(3), 1);
			}
			else if(system_rev == 0x1008){
				// In Case of CAM0
				gpio_set_value(TCC_GPC(6), 1);
			}
			else{
				// In Case of CAM0
				gpio_set_value(TCC_GPD(21), 1);
			}
		#endif
	#endif
}

void sensor_power_disable(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900())
	{
		//gpio_set_value(TCC_GPEXT2(3), 0);
	}
	else if(machine_is_m801())
	{
		//gpio_set_value(TCC_GPD(10), 0);
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			//gpio_set_value(TCC_GPG(4), 0);
		}
		else
		{
			#if defined(TCC_POWER_CONTROL)
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					gpio_set_value(TCC_GPEXT1(12), 0);
				#endif
				gpio_set_value(TCC_GPEXT1(13), 0);
			#endif
		}
	}
	else if(machine_is_m801_88() || (system_rev == 0x0003 || system_rev == 0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		#if defined(CONFIG_VIDEO_ATV_SENSOR_RDA5888)
		//gpio_set_value(TCC_GPF(14), 0);
		#else
		//gpio_set_value(TCC_GPF(10), 0);
		#endif
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC892X)
		#if defined(CONFIG_MACH_M805_892X)
			#ifdef CONFIG_M805S_8923_0XA
				gpio_set_value(TCC_GPE(3), 0);
			#endif		
		#else
			if(system_rev == 0x1005 || system_rev == 0x1007){
				// In Case of CAM0
				gpio_set_value(TCC_GPF(24), 0);
			}
			else if(system_rev == 0x1006){
				// In Case of CAM0
				gpio_set_value(TCC_GPD(3), 0);
			}
			else if(system_rev == 0x1008){
				// In Case of CAM0
				gpio_set_value(TCC_GPC(6), 0);
			}
			else{
				// In Case of CAM0
				gpio_set_value(TCC_GPD(21), 0);
			}
		#endif
	#endif
}

int sensor_get_powerdown(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
						return gpio_get_value(TCC_GPG(3));
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						return gpio_get_value(TCC_GPE(3)); // must enable to use 5M(mt9p111)
					#endif
				}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					return gpio_get_value(TCC_GPE(3)); // must enable to use 5M(mt9p111)
				#else
					return gpio_get_value(TCC_GPG(3));
				#endif
			#endif
		}
		else if(machine_is_tcc8800())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID){
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
						return gpio_get_value(TCC_GPG(31));						
					#endif
				}
				else{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						return gpio_get_value(TCC_GPEXT3(6));		// must enable to use 5M(mt9p111)
						
					#endif
				}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					return gpio_get_value(TCC_GPEXT3(6));		// must enable to use 5M(mt9p111)
				#else
					return gpio_get_value(TCC_GPE(2));
				#endif
			#endif
		}
		else if(machine_is_m803())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID){
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_OV7690)
						return gpio_get_value(TCC_GPF(10));
					#elif defined(CONFIG_VIDEO_CAMERA_SENSOR_SIV100B)
						return gpio_get_value(TCC_GPF(10));
					#endif
				}
				else{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9D112) 
						return gpio_get_value(TCC_GPF(11));		
					#endif
				}
			#endif
		}
		else if(machine_is_tcc9300()) 
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID){
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
						return gpio_get_value(TCC_GPE(2));
					#endif
				}
				else{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						return gpio_get_value(TCC_GPEXT3(6));		// must enable to use 5M(mt9p111)
					#endif
				}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					return gpio_get_value(TCC_GPEXT3(6));		// must enable to use 5M(mt9p111)
				#else
					return gpio_get_value(TCC_GPE(24));
				#endif
			#endif			
		}
		else
		{
			return gpio_get_value(TCC_GPE(24));
		}
	#elif defined(CONFIG_ARCH_TCC892X)
		#if defined(CONFIG_MACH_M805_892X)
		
		#else
			if(system_rev == 0x1005 || system_rev == 0x1007){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID)
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							return gpio_get_value(TCC_GPF(15));
						#endif
					}
					else
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							return gpio_get_value(TCC_GPF(30)); // must enable to use 5M(mt9p111)						
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)					
						return gpio_get_value(TCC_GPF(30)); // must enable to use 5M(mt9p111)
					#else
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							return gpio_get_value(TCC_GPF(15));					
						#else
							return gpio_get_value(TCC_GPF(13));	
						#endif						
					#endif
				#endif
				
			}
			else if(system_rev == 0x1006){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID)
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							return gpio_get_value(TCC_GPD(2));
						#endif
					}
					else
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							return gpio_get_value(TCC_GPG(1)); // must enable to use 5M(mt9p111)						
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)					
						return gpio_get_value(TCC_GPG(1)); // must enable to use 5M(mt9p111)
					#else
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							return gpio_get_value(TCC_GPD(2));					
						#else
							return gpio_get_value(TCC_GPD(1));	
						#endif						
					#endif
				#endif
				
			}
			else if(system_rev == 0x1008){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID)
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							return gpio_get_value(TCC_GPC(5));
						#endif
					}
					else
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							return gpio_get_value(TCC_GPC(28)); // must enable to use 5M(mt9p111)						
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)					
						return gpio_get_value(TCC_GPC(28)); // must enable to use 5M(mt9p111)
					#else
						//#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							return gpio_get_value(TCC_GPC(5));					
						//#else
						//	return gpio_get_value(TCC_GPD(1));	
						//#endif						
					#endif
				#endif
				
			}
			else{
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID)
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							return gpio_get_value(TCC_GPD(16));
						#endif
					}
					else
					{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							return gpio_get_value(TCC_GPC(29)); // must enable to use 5M(mt9p111)						
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)					
						return gpio_get_value(TCC_GPC(29)); // must enable to use 5M(mt9p111)
					#else
						return gpio_get_value(TCC_GPD(16));
					#endif
				#endif
			}
		#endif
	
	#endif
}

void sensor_powerdown_enable(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900())
	{
		gpio_set_value(TCC_GPF(26), 1); 
	}
	else if(machine_is_m801())
	{
		gpio_set_value(TCC_GPF(21), 1); 
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
				gpio_set_value(TCC_GPG(3), 1);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
				gpio_set_value(TCC_GPE(3), 1); // must enable to use 5M(mt9p111)
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPE(3), 1); // must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPG(3), 1);
				#endif
			#endif
		}
		else if(machine_is_tcc8800())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID){
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
						gpio_set_value(TCC_GPG(31), 1);
					#endif
				}
				else{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPEXT3(6), 1);		// must enable to use 5M(mt9p111)
					#endif
				}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT3(6), 1);		// must enable to use 5M(mt9p111)
				#else					
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
						gpio_set_value(TCC_GPG(31), 1);
					#else
						gpio_set_value(TCC_GPE(2), 1);
					#endif
				#endif
			#endif
		}
		else if(machine_is_tcc9300()) 
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID){
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
						gpio_set_value(TCC_GPE(2), 1);
					#endif
				}
				else{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPEXT3(6), 1);		// must enable to use 5M(mt9p111)
					#endif
				}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT3(6), 1);		// must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPE(24), 1);
				#endif
			#endif			
		}
		else
		{
			// todo : 
		}
	}
	else if(machine_is_m801_88() || machine_is_m803() || (system_rev == 0x0003) || system_rev == 0x0004 || system_rev==0x0005 || system_rev == 0x0006)
	{
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID){
				gpio_set_value(TCC_GPF(10), 1);
			}
			else{
				printk("mt9d112 powerdown high!!\n");
				gpio_set_value(TCC_GPF(11), 1);
			}
		#else
			gpio_set_value(TCC_GPF(11), 1);			
		#endif
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC892X)
		#if defined(CONFIG_MACH_M805_892X)
			#if defined(CONFIG_M805S_8925_0XX)
				gpio_set_value(TCC_GPD(1), 1); 
			#else
				if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
					gpio_set_value(TCC_GPD(1), 1); 
				else
					dprintk("M805S PWDN Enable!!\n");
					gpio_set_value(TCC_GPE(22), 1);	
			#endif
		#else
			if(system_rev == 0x1005 || system_rev == 0x1007){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("system_rev = 0x1005, mt9m113 powerdown Enable!!\n");
							gpio_set_value(TCC_GPF(15), 1);						
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("system_rev = 0x1005, mt9p111 powerdown Enable!!\n");
							gpio_set_value(TCC_GPF(30), 1);		// must enable to use 5M(mt9p111)							
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPF(30), 1);		// must enable to use 5M(mt9p111)					
					#else					
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPF(15), 1);
						#else
							gpio_set_value(TCC_GPF(13), 1);
						#endif
					#endif
				#endif
			}
			else if(system_rev == 0x1006){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("system_rev = 0x1006, mt9m113 powerdown Enable!!\n");
							gpio_set_value(TCC_GPD(2), 1);						
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("system_rev = 0x1006, mt9p111 powerdown Enable!!\n");
							gpio_set_value(TCC_GPG(1), 1);		// must enable to use 5M(mt9p111)							
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPG(1), 1);		// must enable to use 5M(mt9p111)					
					#else					
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPD(2), 1);
						#else
							gpio_set_value(TCC_GPD(1), 1);
						#endif
					#endif
				#endif
			}
			else if(system_rev == 0x1008){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("system_rev = 0x1008, mt9m113 powerdown Enable!!\n");
							gpio_set_value(TCC_GPC(5), 1);						
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("system_rev = 0x1008, mt9p111 powerdown Enable!!\n");
							gpio_set_value(TCC_GPC(28), 1);		// must enable to use 5M(mt9p111)							
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPC(28), 1);		// must enable to use 5M(mt9p111)					
					#else					
						//#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPC(5), 1);
						//#else
						//	gpio_set_value(TCC_GPD(1), 1);
						//#endif
					#endif
				#endif
			}			
			else{
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("mt9m113 powerdown Enable!!\n");
							gpio_set_value(TCC_GPD(16), 1);
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("mt9p111 powerdown Enable!!\n");
							gpio_set_value(TCC_GPC(29), 1);		// must enable to use 5M(mt9p111)							
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPC(29), 1);		// must enable to use 5M(mt9p111)					
					#else					
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPD(16), 1);
						#else
							gpio_set_value(TCC_GPF(13), 1);
						#endif
					#endif
				#endif
			}
		#endif
	
	#endif
}

void sensor_powerdown_disable(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900())
	{
		gpio_set_value(TCC_GPF(26), 0); 
	}
	else if(machine_is_m801())
	{
		gpio_set_value(TCC_GPF(21), 0); 
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
				gpio_set_value(TCC_GPG(3), 0);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
				gpio_set_value(TCC_GPE(3), 0); // must enable to use 5M(mt9p111)
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPE(3), 0); // must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPG(3), 0);
				#endif
			#endif
		}
		else if(machine_is_tcc8800())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID){
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
						gpio_set_value(TCC_GPG(31), 0);
					#endif
				}
				else{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPEXT3(6), 0);		// must enable to use 5M(mt9p111)
					#endif
				}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT3(6), 0);		// must enable to use 5M(mt9p111)
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
						gpio_set_value(TCC_GPG(31), 0);
					#else
						gpio_set_value(TCC_GPE(2), 0);
					#endif					
				#endif
			#endif
		}
		else if(machine_is_tcc9300())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID){
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
						gpio_set_value(TCC_GPE(2), 0);
					#endif
				}
				else{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPEXT3(6), 0);		// must enable to use 5M(mt9p111)
					#endif
				}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT3(6), 0);		// must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPE(24), 0);
				#endif
			#endif		
		}
		else
		{
			gpio_set_value(TCC_GPE(24), 0);
		}
	}
	else if(machine_is_m801_88() || machine_is_m803() || (system_rev == 0x0003 || system_rev == 0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID){
				gpio_set_value(TCC_GPF(10), 0);
			}
			else{
				printk("mt9d112 powerdown low!!\n");
				gpio_set_value(TCC_GPF(11), 0);
			}
		#else
			gpio_set_value(TCC_GPF(11), 0);			
		#endif
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC892X)
		#if defined(CONFIG_MACH_M805_892X)
			#if defined(CONFIG_M805S_8925_0XX)
				gpio_set_value(TCC_GPD(1), 0); 
			#else
				if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
					gpio_set_value(TCC_GPD(1), 0);
				else
					dprintk("M805S PWDN Disable!!\n");
					gpio_set_value(TCC_GPE(22), 0);
			#endif
		#else
			if(system_rev == 0x1005 || system_rev == 0x1007){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("mt9m113 powerdown Disable!!\n");
							gpio_set_value(TCC_GPF(15), 0);						
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("mt9p111 powerdown Disable!!\n");
							gpio_set_value(TCC_GPF(30), 0);		// must enable to use 5M(mt9p111)						
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPF(30), 0);		// must enable to use 5M(mt9p111)						
					#else					
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPF(15), 0);
						#else
							gpio_set_value(TCC_GPF(13), 0);
						#endif
					#endif
				#endif
				
			}
			else if(system_rev == 0x1006){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("system_rev = 0x1006, mt9m113 powerdown Disable!!\n");
							gpio_set_value(TCC_GPD(2), 0);						
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("system_rev = 0x1006, mt9p111 powerdown Disable!!\n");
							gpio_set_value(TCC_GPG(1), 0);		// must enable to use 5M(mt9p111)							
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPG(1), 0);		// must enable to use 5M(mt9p111)					
					#else					
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPD(2), 0);
						#else
							gpio_set_value(TCC_GPD(1), 0);
						#endif
					#endif
				#endif
			}
			else if(system_rev == 0x1008){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("system_rev = 0x1008, mt9m113 powerdown Disable!!\n");
							gpio_set_value(TCC_GPC(5), 0);						
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("system_rev = 0x1008, mt9p111 powerdown Disable!!\n");
							gpio_set_value(TCC_GPC(28), 0);		// must enable to use 5M(mt9p111)							
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPC(28), 0);		// must enable to use 5M(mt9p111)					
					#else					
						//#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPC(5), 0);
						//#else
						//	gpio_set_value(TCC_GPD(1), 0);
						//#endif
					#endif
				#endif
			}
			else{
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
					if(CameraID){
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
							dprintk("mt9m113 powerdown Disable!!\n");
							gpio_set_value(TCC_GPD(16), 0);
						#endif
					}
					else{
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
							dprintk("mt9p111 powerdown Disable!!\n");
							gpio_set_value(TCC_GPC(29), 0);		// must enable to use 5M(mt9p111)							
						#endif
					}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPC(29), 0);		// must enable to use 5M(mt9p111)						
					#else					
						#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T113)
							gpio_set_value(TCC_GPD(16), 0);
						#else
							gpio_set_value(TCC_GPF(13), 0);
						#endif
					#endif
				#endif
			}
		#endif
	
	#endif
}

void sensor_reset_high(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900())
	{
		gpio_set_value(TCC_GPE(26), 1);
	}
	else if(machine_is_m801())
	{
		gpio_set_value(TCC_GPE(2), 1);
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
				gpio_set_value(TCC_GPG(2), 1);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
				gpio_set_value(TCC_GPE(2), 1); // must enable to use 5M(mt9p111)
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPE(2), 1); // must enable to use 5M(mt9p111)
				#else
				gpio_set_value(TCC_GPG(2), 1);
				#endif
			#endif
		}
		else if(machine_is_tcc8800())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID){
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_set_value(TCC_GPG(30), 1);
				#endif
			}
			else{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 1);	// must enable to use 5M(mt9p111)
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 1);	// must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPG(30), 1);
				#endif
			#endif
		}
		else if(machine_is_tcc9300())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID){
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_set_value(TCC_GPE(25), 1);
				#endif
			}
			else{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 1);	// must enable to use 5M(mt9p111)
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 1);	// must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPE(25), 1);
				#endif
			#endif
			
		}		
		else 
		{
			gpio_set_value(TCC_GPE(25), 1);
		}
	}
	else if(machine_is_m801_88() && (system_rev != 0x0003 && system_rev != 0x0004 && system_rev != 0x0005 && system_rev != 0x0006))
	{
		printk("machine_is_m801_88 reset high!!\n");
		gpio_set_value(TCC_GPG(30), 1);
	}
	else if(machine_is_m803() || (system_rev == 0x0003 || system_rev == 0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		printk("mt9d112 reset high!!\n");
//		gpio_set_value(TCC_GPA(15), 1);
//		gpio_set_value(TCC_GPD(12), 1);
		gpio_set_value(TCC_GPE(15), 1);

	}	
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC892X)
		#if defined(CONFIG_MACH_M805_892X)
			#if defined(CONFIG_M805S_8925_0XX)
				gpio_set_value(TCC_GPF(15), 1);
			#else
				if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
					gpio_set_value(TCC_GPF(15), 1);
				else
					dprintk("M805S Reset High!!\n");
					gpio_set_value(TCC_GPE(10), 1);
			#endif
		#else
			if(system_rev == 0x1006){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					dprintk("mt9m113 reset high!!\n");
					gpio_set_value(TCC_GPF(15), 1);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					dprintk("mt9p111 reset high!!\n");
					gpio_set_value(TCC_GPD(1), 1); // must enable to use 5M(mt9p111)
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPD(1), 1); // must enable to use 5M(mt9p111)
					#else
					gpio_set_value(TCC_GPF(15), 1);
					#endif
				#endif
			}
			else if(system_rev == 0x1008){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					dprintk("mt9m113 reset high!!\n");
					gpio_set_value(TCC_GPC(4), 1);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					dprintk("mt9p111 reset high!!\n");
					gpio_set_value(TCC_GPC(8), 1); // must enable to use 5M(mt9p111)
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPC(8), 1); // must enable to use 5M(mt9p111)
					#else
					gpio_set_value(TCC_GPC(4), 1);
					#endif
				#endif
			}
			else{
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					dprintk("mt9m113 reset high!!\n");
					gpio_set_value(TCC_GPF(14), 1);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					dprintk("mt9p111 reset high!!\n");
					gpio_set_value(TCC_GPF(13), 1); // must enable to use 5M(mt9p111)
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPF(13), 1); // must enable to use 5M(mt9p111)
					#else
					gpio_set_value(TCC_GPF(14), 1);
					#endif
				#endif
			}
		#endif
		
	#endif
}

void sensor_reset_low(void)
{
	#if defined(CONFIG_ARCH_TCC92XX)
	if(machine_is_tcc8900())
	{
		gpio_set_value(TCC_GPE(26), 0);
	}
	else if(machine_is_m801())
	{
		gpio_set_value(TCC_GPE(2), 0);
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC93XX)  || defined(CONFIG_ARCH_TCC88XX)
	if(machine_is_tcc9300() || machine_is_tcc9300st() || machine_is_tcc9300cm() || machine_is_tcc8800())
	{
		if(machine_is_tcc8800() && (system_rev == 0x0614 || system_rev == 0x0622 || system_rev == 0x0624))
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID)
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)				
				gpio_set_value(TCC_GPG(2), 0);
				#endif
			}
			else
			{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)				
				gpio_set_value(TCC_GPE(2), 0); // must enable to use 5M(mt9p111)
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPE(2), 0); // must enable to use 5M(mt9p111)
				#else
				gpio_set_value(TCC_GPG(2), 0);
				#endif
			#endif
		}
		else if(machine_is_tcc8800())
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID){
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_set_value(TCC_GPG(30), 0);
				#endif
			}
			else{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 0);	// must enable to use 5M(mt9p111)
				#endif
			}
			#else 
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 0);	// must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPG(30), 0);
				#endif
			#endif
		}
		else if(machine_is_tcc9300() )
		{
			#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
			if(CameraID){
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					gpio_set_value(TCC_GPE(25), 0);
				#endif
			}
			else{
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 0);	// must enable to use 5M(mt9p111)
				#endif
			}
			#else
				#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					gpio_set_value(TCC_GPEXT1(12), 0);	// must enable to use 5M(mt9p111)
				#else
					gpio_set_value(TCC_GPE(25), 0);
				#endif
			#endif
		}
		else 
		{
			gpio_set_value(TCC_GPE(25), 0);
		}
	}
	else if(machine_is_m801_88() && (system_rev != 0x0003 && system_rev != 0x0004 && system_rev != 0x0005 && system_rev != 0x0006))
	{
		gpio_set_value(TCC_GPG(30), 0);
	}
	else if(machine_is_m803() || (system_rev == 0x0003 || system_rev == 0x0004 || system_rev==0x0005 || system_rev == 0x0006))
	{
		printk("mt9d112 reset low!!\n");
		//gpio_set_value(TCC_GPD(12), 0);
		gpio_set_value(TCC_GPE(15), 0);
	}
	else
	{
		// todo : customer coding part.
		// if you want to control the power, this part should be coding.
	}
	#elif defined(CONFIG_ARCH_TCC892X)
		#if defined(CONFIG_MACH_M805_892X)
			#if defined(CONFIG_M805S_8925_0XX)
				gpio_set_value(TCC_GPF(15), 0);
			#else
				if(system_rev == 0x2002 || system_rev == 0x2003 || system_rev == 0x2004 || system_rev == 0x2005)
					gpio_set_value(TCC_GPF(15), 0);
				else
					dprintk("M805S Reset Low!!\n");
					gpio_set_value(TCC_GPE(10), 0);
			#endif
		#else
			if(system_rev == 0x1006){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					dprintk("mt9m113 reset Low!!\n");
					gpio_set_value(TCC_GPF(15), 0);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					dprintk("mt9p111 reset Low!!\n");
					gpio_set_value(TCC_GPD(1), 0); // must enable to use 5M(mt9p111)
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPD(1), 0); // must enable to use 5M(mt9p111)
					#else
					gpio_set_value(TCC_GPF(15), 0);
					#endif
				#endif
			}
			else if(system_rev == 0x1008){
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					dprintk("mt9m113 reset Low!!\n");
					gpio_set_value(TCC_GPC(4), 0);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					dprintk("mt9p111 reset Low!!\n");
					gpio_set_value(TCC_GPC(8), 0); // must enable to use 5M(mt9p111)
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPC(8), 0); // must enable to use 5M(mt9p111)
					#else
					gpio_set_value(TCC_GPC(4), 0);
					#endif
				#endif
			}
			else{
				#if defined(CONFIG_VIDEO_DUAL_CAMERA_SUPPORT)
				if(CameraID)
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9M113)
					dprintk("mt9m113 reset Low!!\n");
					gpio_set_value(TCC_GPF(14), 0);
					#endif
				}
				else
				{
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
					dprintk("mt9p111 reset Low!!\n");
					gpio_set_value(TCC_GPF(13), 0); // must enable to use 5M(mt9p111)
					#endif
				}
				#else
					#if defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9P111) || defined(CONFIG_VIDEO_CAMERA_SENSOR_MT9T111)
						gpio_set_value(TCC_GPF(13), 0); // must enable to use 5M(mt9p111)
					#else
					gpio_set_value(TCC_GPF(14), 0);
					#endif
				#endif
			}
		#endif

	#endif
}

/* ----------------------------------------------------------------------- */
MODULE_DESCRIPTION("Camera SENSOR  driver");
MODULE_AUTHOR("Telechips");
MODULE_LICENSE("GPL");

