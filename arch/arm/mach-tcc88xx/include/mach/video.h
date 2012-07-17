//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file video.h
 * @brief Definition for video
 *
 */
#ifndef _VIDEO_H_
#define _VIDEO_H_

#ifndef __HDMI_VIDEO_VIDEOFORMAT__
#define __HDMI_VIDEO_VIDEOFORMAT__
/**
 * @enum VideoFormat
 * Video format
 */
enum VideoFormat
{
    /** 640x480p\@60Hz */
    v640x480p_60Hz = 0,
    /** 720x480p\@60Hz */
    v720x480p_60Hz,
    /** 1280x700p\@60Hz */
    v1280x720p_60Hz,
    /** 1920x1080i\@60Hz */
    v1920x1080i_60Hz,
    /** 720x480i\@60Hz */
    v720x480i_60Hz,
    /** 720x240p\@60Hz */
    v720x240p_60Hz,
    /** 2880x480i\@60Hz */
    v2880x480i_60Hz,
    /** 2880x240p\@60Hz */
    v2880x240p_60Hz,
    /** 1440x480p\@60Hz */
    v1440x480p_60Hz,
    /** 1920x1080p\@60Hz */
    v1920x1080p_60Hz,
    /** 720x576p\@60Hz */
    v720x576p_50Hz,
    /** 1280x720p\@50Hz */
    v1280x720p_50Hz,
    /** 1920x1080i\@50Hz (V total = 1125) */
    v1920x1080i_50Hz,
    /** 720x576i\@50Hz */
    v720x576i_50Hz,
    /** 720x288p\@50Hz */
    v720x288p_50Hz,
    /** 2880x576i\@50Hz */
    v2880x576i_50Hz,
    /** 2880x288p\@50Hz */
    v2880x288p_50Hz,
    /** 1440x576p\@50Hz */
    v1440x576p_50Hz,
    /** 1920x1080p\@50Hz */
    v1920x1080p_50Hz,
    /** 1920x1080p\@23.976Hz */
    v1920x1080p_23_976Hz,
    /** 1920x1080p\@24Hz */
    v1920x1080p_24Hz,
    /** 1920x1080p\@25Hz */
    v1920x1080p_25Hz,
    /** 1920x1080p\@30Hz */
    v1920x1080p_30Hz,

	/** 1280x720p\@60Hz 3D Frame packing */
    v1280x720p_60Hz_3D,

    /** 2880x480p\@60Hz */
    v2880x480p_60Hz,
    /** 2880x576p\@60Hz */
    v2880x576p_50Hz,
    /** 1920x1080i\@50Hz (V total = 1250) */
    v1920x1080i_50Hz_1250,
    /** 1920x1080i\@100Hz */
    v1920x1080i_100Hz,
    /** 1280x720p\@100Hz */
    v1280x720p_100Hz,
    /** 720x576p\@100Hz */
    v720x576p_100Hz,
    /** 720x576i\@100Hz */
    v720x576i_100Hz,
    /** 1920x1080i\@120Hz */
    v1920x1080i_120Hz,
    /** 1280x720p\@120Hz */
    v1280x720p_120Hz,
    /** 720x480p\@120Hz */
    v720x480p_120Hz,
    /** 720x480i\@120Hz */
    v720x480i_120Hz,
    /** 720x576p\@200Hz */
    v720x576p_200Hz,
    /** 720x576i\@200Hz */
    v720x576i_200Hz,
    /** 720x480p\@240Hz */
    v720x480p_240Hz,
    /** 720x480i\@240Hz */
    v720x480i_240Hz,

	/** 1360x768p\@60Hz */
    v1360x768p_60Hz,
    /** 1366x768p\@60Hz */
    v1366x768p_60Hz,
    /** 1024x768p\@60Hz */
    v1024x768p_60Hz,
	/** 854x480p\@60Hz */
    v854x480p_60Hz,

	/** 1920x1080p\@29.97Hz */
    v1920x1080p_29_97Hz,
    
#if defined(TELECHIPS)
	max_video_formats,
#endif
};
#endif /* __HDMI_VIDEO_VIDEOFORMAT__ */
#ifndef __HDMI_VIDEO_COLORSPACE__
#define __HDMI_VIDEO_COLORSPACE__
/**
 * @enum ColorSpace
 * Color space of video stream.
 */
enum ColorSpace
{
    /** RGB color space */
    HDMI_CS_RGB,
    /** YCbCr 4:4:4 color space */
    HDMI_CS_YCBCR444,
    /** YCbCr 4:2:2 color space */
    HDMI_CS_YCBCR422
};
#endif /* __HDMI_VIDEO_COLORSPACE__ */

#ifndef __HDMI_VIDEO_COLORDEPTH__
#define __HDMI_VIDEO_COLORDEPTH__
/**
 * @enum ColorDepth
 * Color depth per pixel of video stream
 */
enum ColorDepth
{
    /** 36 bit color depth per pixel */
    HDMI_CD_36,
    /** 30 bit color depth per pixel */
    HDMI_CD_30,
    /** 24 bit color depth per pixel */
    HDMI_CD_24
};
#endif /* __HDMI_VIDEO_COLORDEPTH__ */

#ifndef __HDMI_VIDEO_HDMIMODE__
#define __HDMI_VIDEO_HDMIMODE__
/**
 * @enum HDMIMode
 * System mode
 */
enum HDMIMode
{
    /** DVI mode */
    DVI = 0,
    /** HDMI mode */
    HDMI
};
#endif /* __HDMI_VIDEO_HDMIMODE__ */

#ifndef __HDMI_VIDEO_PIXELLIMIT__
#define __HDMI_VIDEO_PIXELLIMIT__
/**
 * @enum PixelLimit
 * Pixel limitation of video stream
 */
enum PixelLimit
{
    /** Full range */
    HDMI_FULL_RANGE,
    /** Limit range for RGB color space */
    HDMI_RGB_LIMIT_RANGE,
    /** Limit range for YCbCr color space */
    HDMI_YCBCR_LIMIT_RANGE
};
#endif /* __HDMI_VIDEO_PIXELLIMIT__ */

#ifndef __HDMI_VIDEO_COLORIMETRY__
#define __HDMI_VIDEO_COLORIMETRY__
/**
 * @enum HDMIColorimetry
 * Colorimetry of video stream
 */
enum HDMIColorimetry
{
    /** Colorimetry is not defined */
    HDMI_COLORIMETRY_NO_DATA,
    /** ITU601 colorimetry */
    HDMI_COLORIMETRY_ITU601,
    /** ITU709 colorimetry */
    HDMI_COLORIMETRY_ITU709,
    /** Extended ITU601 colorimetry */
    HDMI_COLORIMETRY_EXTENDED_xvYCC601,
    /** Extended ITU709 colorimetry */
    HDMI_COLORIMETRY_EXTENDED_xvYCC709
};
#endif /* __HDMI_VIDEO_COLORIMETRY__ */

#ifndef __HDMI_VIDEO_PIXELASPECTRATIO__
#define __HDMI_VIDEO_PIXELASPECTRATIO__
/**
 * @enum PixelAspectRatio
 * Pixel aspect ratio of video stream
 */
enum PixelAspectRatio
{
    /** 4:3 pixel ratio */
    HDMI_PIXEL_RATIO_4_3,
    /** 16:9 pixel ratio */
    HDMI_PIXEL_RATIO_16_9
};
#endif /* __HDMI_VIDEO_PIXELASPECTRATIO__ */

#ifndef __HDMI_VIDEO_PIXELFREQUENCY__
#define __HDMI_VIDEO_PIXELFREQUENCY__
/**
 * @enum PixelFreq
 * Pixel Frequency
 */
enum PixelFreq {
    /** 25.2 MHz pixel frequency */
    PIXEL_FREQ_25_200 = 2520,
    /** 25.175 MHz pixel frequency */
    PIXEL_FREQ_25_175 = 2517,
    /** 27 MHz pixel frequency */
    PIXEL_FREQ_27 = 2700,
    /** 27.027 MHz pixel frequency */
    PIXEL_FREQ_27_027 = 2702,
    /** 54 MHz pixel frequency */
    PIXEL_FREQ_54 = 5400,
    /** 54.054 MHz pixel frequency */
    PIXEL_FREQ_54_054 = 5405,
    /** 74.25 MHz pixel frequency */
    PIXEL_FREQ_74_250 = 7425,
    /** 74.176 MHz pixel frequency */
    PIXEL_FREQ_74_176 = 7417,
    /** 148.5 MHz pixel frequency */
    PIXEL_FREQ_148_500 = 14850,
    /** 148.352 MHz pixel frequency */
    PIXEL_FREQ_148_352 = 14835,
    /** 108.108 MHz pixel frequency */
    PIXEL_FREQ_108_108 = 10810,
    /** 72 MHz pixel frequency */
    PIXEL_FREQ_72 = 7200,
    /** 25 MHz pixel frequency */
    PIXEL_FREQ_25 = 2500,
    /** 65 MHz pixel frequency */
    PIXEL_FREQ_65 = 6500,
    /** 108 MHz pixel frequency */
    PIXEL_FREQ_108 = 10800,
    /** 162 MHz pixel frequency */
    PIXEL_FREQ_162 = 16200,
	/** 84.75 MHz pixel frequency */
    PIXEL_FREQ_84_75 = 8475,
    /** 37.293 MHz pixel frequency */
    PIXEL_FREQ_37_293 = 3729,
};
#endif /* __HDMI_VIDEO_PIXELFREQUENCY__ */


#ifndef __HDMI_VIDEO_PARAMETERS__
#define __HDMI_VIDEO_PARAMETERS__

//! Structure for HDMI video
struct HDMIVideoParameter {
    /** Video interface */
    enum HDMIMode mode;
    /** Video format */
    enum VideoFormat resolution;
    /** Color space */
    enum ColorSpace colorSpace;
    /** Color depth */
    enum ColorDepth colorDepth;
    /** Colorimetry */
    enum HDMIColorimetry colorimetry;
    /** Pixel aspect ratio */
    enum PixelAspectRatio pixelAspectRatio;
};
#endif /* __HDMI_VIDEO_PARAMETERS__ */

#endif /* _VIDEO_H_ */
