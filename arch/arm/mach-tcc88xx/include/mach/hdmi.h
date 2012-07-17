//-------------------------------------------------------------------
// Copyright (c) Telechips, Inc.
// All right reserved.
//
//-------------------------------------------------------------------
/**
 * @file hdmi.h
 * @brief header file for hdmi device driver.
 *
 */
#ifndef _LINUX_HDMI_H_
#define _LINUX_HDMI_H_

#define HDMI_IOC_MAGIC      'h'


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
 * Color space.
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
 * Color Depth per pixel.
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
 * Pixel limitation.
 */
enum PixelLimit
{
    /** Full range */
    HDMI_FULL_RANGE,
    /** limit range for RGB color space */
    HDMI_RGB_LIMIT_RANGE,
    /** limit range for YCbCr color space */
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
 * Pixel aspect ratio.
 */
enum PixelAspectRatio
{
    /** 4:3 pixel ratio */
    HDMI_PIXEL_RATIO_4_3,
    /** 16:9 pixel ratio */
    HDMI_PIXEL_RATIO_16_9
};
#endif /* __HDMI_VIDEO_PIXELASPECTRATIO__ */

#ifndef __HDMI_AUDIO_AUDIOSAMPLEPACKETTYPE__
#define __HDMI_AUDIO_AUDIOSAMPLEPACKETTYPE__
/**
 * @enum HDMIASPType
 * Type of HDMI audio sample packet
 */
enum HDMIASPType
{
    /** Audio Sample Packet Type */
    HDMI_ASP,
    /** One Bit Audio Packet Type */
    HDMI_DSD,
    /** High Bit Rate Packet Type */
    HDMI_HBR,
    /** DST Packet Type */
    HDMI_DST
};
#endif /* __HDMI_AUDID_AUDIOSAMPLEPACKETTYPE__ */

#ifndef __HDMI_DEVICE_VIDEO_PARAMS__
#define __HDMI_DEVICE_VIDEO_PARAMS__
/**
 * @struct device_video_params
 * Video timing paramters to set HDMI H/W @n
 * For more information, refer to HDMI register map doc.
 */
struct device_video_params
{
    /** [H Blank] */
    unsigned int    HBlank;

    /** [V Blank] */
    unsigned int    VBlank;

    /** [H Total : V Total] */
    unsigned int    HVLine;

    /** [H Sync polarity : H Sync end point : H Sync start point] */
    unsigned int    HSYNCGEN;

    /** [V Sync start line num + V Sync end line num] */
    unsigned int    VSYNCGEN;

    /** CEA VIC */
    unsigned char   AVI_VIC;
    /** CEA VIC for 16:9 pixel ratio */
    unsigned char   AVI_VIC_16_9;

    /** 0 - progressive, 1 - interlaced */
    unsigned char   interlaced;

    /** Pixel repetition if double, set 1 */
    unsigned char   repetition;

    /** V Sync polarity */
    unsigned char   polarity;

    /**
     * If interlaced mode, @n
     * [ end point of bottom field's active region : start point of that ]
     */
    unsigned int    VBLANK_F;

    /**
     * If interlaced mode, @n
     * [ start line of bottom field's V Sync : end line of that ]
     */
    unsigned int    VSYNCGEN2;

    /**
     * If interlaced mode, @n
     * [ start transition point of bottom field's V Sync : end transition of that ]
     */
    unsigned int    VSYNCGEN3;
};

#endif /* __HDMI_DEVICE_VIDEO_PARAMS__ */

#if defined(TELECHIPS)
#ifndef __HDMI_DEVICE_LCDC_TIMING_PARAMS__
#define __HDMI_DEVICE_LCDC_TIMING_PARAMS__
/**
 * @struct device_lcdc_timing_params
 * Video timing paramters to set LCDC H/W @n
 * For more information, refer to LCDC register map doc.
 */
struct device_lcdc_timing_params
{
	unsigned int id;
	unsigned int iv;
	unsigned int ih;
	unsigned int ip;
	unsigned int dp;
	unsigned int ni;
	unsigned int tv;
	unsigned int tft;
	unsigned int stn;

	//LHTIME1
	unsigned int lpw;
	unsigned int lpc;
	//LHTIME2
	unsigned int lswc;
	unsigned int lewc;
	//LVTIME1
	unsigned int vdb;
	unsigned int vdf;
	unsigned int fpw;
	unsigned int flc;
	//LVTIME2
	unsigned int fswc;
	unsigned int fewc;
	//LVTIME3
	unsigned int fpw2;
	unsigned int flc2;
	//LVTIME4
	unsigned int fswc2;
	unsigned int fewc2;

};

#endif /* __HDMI_DEVICE_VIDEO_PARAMS__ */

#if 0 //not use sys3
typedef enum{
	LCDC_IMG_FMT_1BPP,
	LCDC_IMG_FMT_2BPP,
	LCDC_IMG_FMT_4BPP,
	LCDC_IMG_FMT_8BPP,
	LCDC_IMG_FMT_RGB332 = 8,
	LCDC_IMG_FMT_RGB444 = 9,
	LCDC_IMG_FMT_RGB565 = 10,
	LCDC_IMG_FMT_RGB555 = 11,
	LCDC_IMG_FMT_RGB888 = 12,
	LCDC_IMG_FMT_RGB666 = 13,
	LCDC_IMG_FMT_YUV420SP = 24,	
	LCDC_IMG_FMT_YUV422SP = 25, 
	LCDC_IMG_FMT_YUV422SQ = 26, 
	LCDC_IMG_FMT_YUV420ITL0 = 28, 
	LCDC_IMG_FMT_YUV420ITL1 = 29, 
	LCDC_IMG_FMT_YUV422ITL0 = 30, 
	LCDC_IMG_FMT_YUV422ITL1 = 31, 
	LCDC_IMG_FMT_MAX
}LCDC_IMG_FMT_TYPE;

typedef enum{
	LCDC_LAYER_0, 
	LCDC_LAYER_1, 
	LCDC_LAYER_2, 
	LCDC_LAYER_MAX
}LCD_IMG_LAYER_TYPE;

struct device_lcdc_image_params
{
	unsigned int LCD_layer;
	
	unsigned int Frame_width;
	unsigned int Frame_height;

	unsigned int Image_width;
	unsigned int Image_height;

	unsigned int addr0;
	unsigned int addr1;
	unsigned int addr2;
	unsigned int fmt;	//LCDC_IMG_FMT_TYPE
};
#endif//
#endif /* TELECHIPS */

/**
 * @enum hdcp_event
 * HDCP event to be used for HDCP processing
 */
enum hdcp_event
{
    /** Stop HDCP  */
    HDCP_EVENT_STOP,
    /** Start HDCP */
    HDCP_EVENT_START,
    /** Start to read Bksv,Bcaps */
    HDCP_EVENT_READ_BKSV_START,
    /** Start to write Aksv,An */
    HDCP_EVENT_WRITE_AKSV_START,
    /** Start to check if Ri is equal to Rj */
    HDCP_EVENT_CHECK_RI_START,
    /** Start 2nd authentication process */
    HDCP_EVENT_SECOND_AUTH_START,
};
/** Size of KSV */
#define HDCP_KSV_SIZE       5
/** Size of Bstatus */
#define HDCP_STATUS_SIZE    2
/** Size of Ri */
#define HDCP_RI_SIZE        2
/** Size of An */
#define HDCP_AN_SIZE        8
/** Size of SHA1 result */
#define HDCP_SHA1_SIZE      20

/**
 * @struct hdcp_ksv
 * Contains KSV(Key Selection Vector).
 */
struct hdcp_ksv{
    /** KSV */
    unsigned char ksv[HDCP_KSV_SIZE];
};

/**
 * @struct hdcp_status
 * Contains Bstatus.
 */
struct hdcp_status{
    /** Status */
    unsigned char status[HDCP_STATUS_SIZE];
};

/**
 * @struct hdcp_an
 * Contains An(64-bit pseudo-random value).
 */
struct hdcp_an{
    /** An */
    unsigned char an[HDCP_AN_SIZE];
};

/**
 * @struct hdcp_ksv_list
 * Contains one KSV and flag that indicates whether this is the last KSV @n
 * in KSV list or not
 */
struct hdcp_ksv_list{
    /**
     * Flag that indicates structure contains the last KSV in KSV list. @n
     * If so, 1; otherwise, 0.
     */
    unsigned char end;
    /** KSV */
    unsigned char ksv[HDCP_KSV_SIZE];
};

/**
 * @struct hdcp_sha1
 * Contains SHA1 calculated result.
 */
struct hdcp_sha1{
    /** SHA1 calculated result */
    unsigned char sha1[HDCP_SHA1_SIZE];
};


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

enum hdmi_video_format
{
	HDMI_2D,
	HDMI_VIC,
	HDMI_3D,
};

enum hdmi_3d_type
{
	FRAME_PACKING,
	TOP_AND_BOTTOM,
	SIDE_BY_SIDE,
};

struct HDMIVideoFormatCtrl{
    unsigned char video_format;
	unsigned char structure_3D;
	unsigned char ext_data_3D;
};


// IOW
/** Device request code to set color space. */
#define HDMI_IOC_SET_COLORSPACE             _IOW(HDMI_IOC_MAGIC,0,enum ColorSpace)

/** Device request code to set color depth */
#define HDMI_IOC_SET_COLORDEPTH             _IOW(HDMI_IOC_MAGIC,1,enum ColorDepth)

/** Device request code to set video system */
#define HDMI_IOC_SET_HDMIMODE               _IOW(HDMI_IOC_MAGIC,2,enum HDMIMode)

/** Device request code to set video timing parameters */
#define HDMI_IOC_SET_VIDEOMODE              _IOW(HDMI_IOC_MAGIC,3,struct device_video_params)

/** Device request code to set pixel aspect ratio */
#define HDMI_IOC_SET_PIXEL_ASPECT_RATIO       _IOW(HDMI_IOC_MAGIC,20,enum PixelAspectRatio)

/** Device request code to set video format information */
#define HDMI_IOC_SET_VIDEOFORMAT_INFO       _IOW(HDMI_IOC_MAGIC,21,enum VideoFormat)



/** Device request code to get video configuration information (video parameters) */
#define HDMI_IOC_GET_VIDEOCONFIG           _IOR(HDMI_IOC_MAGIC,8,struct HDMIVideoParameter)


/** Device request code to get hdmi start/stop status */
#define HDMI_IOC_GET_HDMISTART_STATUS      _IOR(HDMI_IOC_MAGIC,9,unsigned int)


/**
 * Device requset code to set/clear blue screen mode @n
 * 1 to set, 0 to clear
 */
#define HDMI_IOC_SET_BLUESCREEN             _IOW(HDMI_IOC_MAGIC,4,unsigned char)

/** Device requset code to set pixel limitation */
#define HDMI_IOC_SET_PIXEL_LIMIT            _IOW(HDMI_IOC_MAGIC,5,enum PixelLimit)

/** Device requset code to get pixel limitation */
#define HDMI_IOC_GET_PIXEL_LIMIT            _IOR(HDMI_IOC_MAGIC,10,enum PixelLimit)


/**
 * Device requset code to set/clear AVMute @n
 * 1 to set, 0 to clear
 */
#define HDMI_IOC_SET_AVMUTE                 _IOW(HDMI_IOC_MAGIC,6,unsigned char)

/** Device requset code to set packet type of HDMI audio output */
#define HDMI_IOC_SET_AUDIOPACKETTYPE        _IOW(HDMI_IOC_MAGIC,7,enum HDMIASPType)
#define HDMI_IOC_GET_AUDIOPACKETTYPE        _IOW(HDMI_IOC_MAGIC,14,enum HDMIASPType)

/** Device requset code to set audio sample freqency */
#define HDMI_IOC_SET_AUDIOSAMPLEFREQ        _IOW(HDMI_IOC_MAGIC,8,enum SamplingFreq)
#define HDMI_IOC_GET_AUDIOSAMPLEFREQ        _IOR(HDMI_IOC_MAGIC,13,enum SamplingFreq)

/** Device requset code to set number of channels */
#define HDMI_IOC_SET_AUDIOCHANNEL           _IOW(HDMI_IOC_MAGIC,9,enum ChannelNum)
#define HDMI_IOC_GET_AUDIOCHANNEL           _IOR(HDMI_IOC_MAGIC,12,enum ChannelNum)


/** Device requset code to set audio speaker allocation information */
#define HDMI_IOC_SET_SPEAKER_ALLOCATION     _IOW(HDMI_IOC_MAGIC,10,unsigned int)

/** Device requset code to get audio speaker allocation information */
#define HDMI_IOC_GET_SPEAKER_ALLOCATION     _IOR(HDMI_IOC_MAGIC,11,unsigned int)

/**
 * Device requset code to enable/disable HDCP H/W @n
 * 1 to enable, 0 to disable
 */
#define HDMI_IOC_ENABLE_HDCP                _IOW(HDMI_IOC_MAGIC,11,unsigned char)

/** Device requset code to set Bksv  */
#define HDMI_IOC_SET_BKSV                   _IOW(HDMI_IOC_MAGIC,12,struct hdcp_ksv)

/** Device requset code to set Bcaps  */
#define HDMI_IOC_SET_BCAPS                  _IOW(HDMI_IOC_MAGIC,13,unsigned char)

/**
 * Device requset code to set the result whether Ri and Ri' are match or not@n
 * set 1 if match; otherwise set 0
 */
#define HDMI_IOC_SET_HDCP_CHECK_RESULT      _IOW(HDMI_IOC_MAGIC,14,unsigned char)

/** Device requset code to set BStatus  */
#define HDMI_IOC_SET_BSTATUS                _IOW(HDMI_IOC_MAGIC,15,struct hdcp_status)

/** Device requset code to set KSV list  */
#define HDMI_IOC_SET_KSV_LIST               _IOW(HDMI_IOC_MAGIC,16,struct hdcp_ksv_list)

/** Device requset code to set Rx SHA1 calculated result  */
#define HDMI_IOC_SET_SHA1                   _IOW(HDMI_IOC_MAGIC,17,struct hdcp_sha1)

/**
 * Device requset code to start/stop sending EESS @n
 * 1 to start, 0 to stop
 */
#define HDMI_IOC_SET_ENCRYPTION             _IOW(HDMI_IOC_MAGIC,18,unsigned char)

/**
 * Device requset code to start/stop sending audio packet @n
 * 1 to start, 0 to stop
 */
#define HDMI_IOC_SET_AUDIO_ENABLE           _IOW(HDMI_IOC_MAGIC,19,unsigned char)

#if defined(TELECHIPS)
/** Device request code to set lcdc timing parameters */
#define HDMI_IOC_SET_LCDC_TIMING           _IOW(HDMI_IOC_MAGIC,30,struct device_lcdc_timing_params)

/** Device request code to set/get Hdmi Device Power */
#define HDMI_IOC_SET_PWR_CONTROL			_IOW(HDMI_IOC_MAGIC,31, unsigned int)
#define HDMI_IOC_GET_PWR_STATUS				_IOR(HDMI_IOC_MAGIC,31, unsigned int)

#endif /*TELECHIPS*/

#define HDMI_IOC_VIDEO_FORMAT_CONTROL		_IOW(HDMI_IOC_MAGIC,40, struct HDMIVideoFormatCtrl)

/**
 * Device requset code to get status of HDMI PHY H/W @n
 * if 1, PHY is ready; if 0, PHY is not ready.
 */
#define HDMI_IOC_GET_PHYREADY               _IOR(HDMI_IOC_MAGIC,0,unsigned char)

/** Device requset code to get hdcp_event */
#define HDMI_IOC_GET_HDCP_EVENT             _IOR(HDMI_IOC_MAGIC,1,enum hdcp_event)

/** Device requset code to get Aksv */
#define HDMI_IOC_GET_AKSV                   _IOR(HDMI_IOC_MAGIC,2,struct hdcp_ksv)

/** Device requset code to get An */
#define HDMI_IOC_GET_AN                     _IOR(HDMI_IOC_MAGIC,3,struct hdcp_an)

/**
 * Device requset code to get Ri @n
 * The length of Ri is 2 bytes. Ri is contained in LSB 2 bytes.
 */
#define HDMI_IOC_GET_RI                     _IOR(HDMI_IOC_MAGIC,4,int)

#define HDMI_IOC_GET_RI_REG                 _IOR(HDMI_IOC_MAGIC,100,int)

/**
 * Device request code to get SHA1 result
 * which is the comparison between Rx and Tx. @n
 * 1 if match; 0 if not matched
 */
#define HDMI_IOC_GET_SHA1_RESULT            _IOR(HDMI_IOC_MAGIC,5,int)

/**
 * Device request code to get the state of 1st part of HDCP authentication @n
 * 1 if authenticated; 0 if not authenticated
 */
#define HDMI_IOC_GET_AUTH_STATE             _IOR(HDMI_IOC_MAGIC,6,int)

/**
 * Device request code to get whether HDCP H/W finishes reading KSV or not @n
 * 1 if H/W finishes reading KSV; otherwise 0
 */
#define HDMI_IOC_GET_KSV_LIST_READ_DONE     _IOR(HDMI_IOC_MAGIC,7,unsigned char)

/** Device requset code to start sending HDMI output */
#define HDMI_IOC_START_HDMI                 _IO(HDMI_IOC_MAGIC,0)

/** Device requset code to stop sending HDMI output */
#define HDMI_IOC_STOP_HDMI                  _IO(HDMI_IOC_MAGIC,1)

/** Device requset code to start processing HDCP */
#define HDMI_IOC_START_HDCP                 _IO(HDMI_IOC_MAGIC,2)

/** Device requset code to stop processing HDCP */
#define HDMI_IOC_STOP_HDCP                  _IO(HDMI_IOC_MAGIC,3)

/** Device requset code to notify that one of downstream receivers is illegal */
#define HDMI_IOC_SET_ILLEGAL_DEVICE         _IO(HDMI_IOC_MAGIC,4)

/** Device requset code to notify that repeater is not ready within 5 secs */
#define HDMI_IOC_SET_REPEATER_TIMEOUT       _IO(HDMI_IOC_MAGIC,5)

/** Device requset code to reset HDCP H/W  */
#define HDMI_IOC_RESET_HDCP                 _IO(HDMI_IOC_MAGIC,6)

/**
 * Device requset code to notify if Tx is repeater, @n
 * But it has no downstream receiver.
 */
#define HDMI_IOC_SET_KSV_LIST_EMPTY         _IO(HDMI_IOC_MAGIC,7)

/** Device requset code to reset AUI Sampling Freq Fields  */
#define HDMI_IOC_RESET_AUISAMPLEFREQ        _IO(HDMI_IOC_MAGIC,8)

#define HDMI_IOC_SET_PHYRESET               _IO(HDMI_IOC_MAGIC,9)

#endif /* _LINUX_HDMI_H_ */
