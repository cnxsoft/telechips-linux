#ifndef __IXC5000_H__
#define __IXC5000_H__

#define ITV_XC5000_TUNER_RESET		0

//#define ITV_XC5000_DEFAULT_FIRMWARE 		"itv-fe-xc5000-1.4.fw"

/* Misc Defines */
#define MAX_TV_STANDARD				23
#define XC_MAX_I2C_WRITE_LENGTH		64

/* Signal Types */
#define XC_RF_MODE_AIR			0
#define XC_RF_MODE_CABLE		1

/* Result codes */
#define XC_RESULT_SUCCESS			0
#define XC_RESULT_RESET_FAILURE		1
#define XC_RESULT_I2C_WRITE_FAILURE	2
#define XC_RESULT_I2C_READ_FAILURE	3
#define XC_RESULT_OUT_OF_RANGE		5

/* Product id */
#define XC_PRODUCT_ID_FW_NOT_LOADED	0x2000
//20110824 koo : lgdt3305 demod & xc500c tuner porting
#if defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C) || defined(CONFIG_iTV_FE_TUNER_MODULE_XC5000C_MODULE)		
#define XC_PRODUCT_ID_FW_LOADED 	0x14B4
#else
#define XC_PRODUCT_ID_FW_LOADED 	0x1388
#endif

/* Registers */
#define XREG_INIT         0x00
#define XREG_VIDEO_MODE   0x01
#define XREG_AUDIO_MODE   0x02
#define XREG_RF_FREQ      0x03
#define XREG_D_CODE       0x04
#define XREG_IF_OUT       0x05
#define XREG_SEEK_MODE    0x07
#define XREG_POWER_DOWN   0x0A
#define XREG_SIGNALSOURCE 0x0D /* 0=Air, 1=Cable */
#define XREG_SMOOTHEDCVBS 0x0E
#define XREG_XTALFREQ     0x0F
#define XREG_FINERFFREQ   0x10
#define XREG_DDIMODE      0x11

#define XREG_ADC_ENV      0x00
#define XREG_QUALITY      0x01
#define XREG_FRAME_LINES  0x02
#define XREG_HSYNC_FREQ   0x03
#define XREG_LOCK         0x04
#define XREG_FREQ_ERROR   0x05
#define XREG_SNR          0x06
#define XREG_VERSION      0x07
#define XREG_PRODUCT_ID   0x08
#define XREG_BUSY         0x09

/* Tuner standards */
#define MN_NTSC_PAL_BTSC	0
#define MN_NTSC_PAL_A2		1
#define MN_NTSC_PAL_EIAJ	2
#define MN_NTSC_PAL_Mono	3
#define BG_PAL_A2			4
#define BG_PAL_NICAM		5
#define BG_PAL_MONO			6
#define I_PAL_NICAM			7
#define I_PAL_NICAM_MONO	8
#define DK_PAL_A2			9
#define DK_PAL_NICAM		10
#define DK_PAL_MONO			11
#define DK_SECAM_A2DK1		12
#define DK_SECAM_A2LDK3 	13
#define DK_SECAM_A2MONO 	14
#define L_SECAM_NICAM		15
#define LC_SECAM_NICAM		16
#define DTV6				17
#define DTV8				18
#define DTV7_8				19
#define DTV7				20
#define FM_Radio_INPUT2 	21
#define FM_Radio_INPUT1 	22

typedef struct itv_xc5000_tv_standard_t {
	char *Name;
	unsigned short AudioMode;
	unsigned short VideoMode;
} itv_xc5000_tv_standard_t;

typedef struct itv_xc5000_config_t {
	unsigned char	i2c_address;
	unsigned int	if_khz;
} itv_xc5000_config_t;

#endif	/* __IXC5000_H__ */
