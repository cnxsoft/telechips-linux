#ifndef _VIOC_VIN_H_
#define	_VIOC_VIN_H_

#include  <mach/reg_physical.h>

#define     ORDER_RGB	   0
#define     ORDER_RBG	   1
#define     ORDER_GRB	   2
#define     ORDER_GBR	   3
#define     ORDER_BRG	   4
#define     ORDER_BGR	   5

#define     FMT_YUV422_16BIT        0
#define     FMT_YUV422_8BIT         1
#define     FMT_YUVK4444_16BIT      2
#define     FMT_YUVK4224_24BIT      3
#define     FMT_RGBK4444_16BIT      4
#define     FMT_RGB444_24BIT        9
#define     FMT_SD_PROG             12  //NOT USED

#define		EXT_PORT0		0
#define		EXT_PORT1		1
#define		EXT_PORT2		2
#define		EXT_PORT3		3
#define		MUL_PORT0		4
#define		MUL_PORT1		5
#define		MUL_PORT2		6
#define		MUL_PORT3		7

#define		CLK_DOUBLE_EDGE			0
#define		CLK_DOUBLE_FREQ			1
#define		CLK_DOUBLE_EDGE_FREQ	2
#define		CLK_DOUBLE_4TIME_FREQ	3

#define		ON				1
#define		OFF				0

/* Interface APIs. */
extern void VIOC_VIN_SetSyncPolarity (VIOC_VIN *pVIN, unsigned int hs_active_low, unsigned int vs_active_low, unsigned int field_bfield_low, unsigned int de_active_low, unsigned int gen_field_en, unsigned int pxclk_pol);
extern void VIOC_VIN_SetCtrl (VIOC_VIN *pVIN, unsigned int conv_en, unsigned int hsde_connect_en, unsigned int vs_mask, unsigned int fmt, unsigned int data_order);
extern void VIOC_VIN_SetInterlaceMode (VIOC_VIN *pVIN, unsigned int intl_en, unsigned int intpl_en);
extern void VIOC_VIN_SetCaptureModeEnable(VIOC_VIN *pVIN, unsigned int cap_en);
extern void VIOC_VIN_SetEnable (VIOC_VIN *pVIN, unsigned int vin_en);
extern void VIOC_VIN_SetImageSize (VIOC_VIN *pVIN, unsigned int width, unsigned int height);
extern void VIOC_VIN_SetImageOffset (VIOC_VIN *pVIN, unsigned int offs_width, unsigned int offs_height, unsigned int offs_height_intl);
extern void VIOC_VIN_SetY2RMode (VIOC_VIN *pVIN, unsigned int y2r_mode);
extern void VIOC_VIN_SetY2REnable (VIOC_VIN *pVIN, unsigned int y2r_en);
extern void VIOC_VIN_SetLUT(VIOC_VIN *pVIN, unsigned int *pLUT);
extern void VIOC_VIN_SetLUTEnable(VIOC_VIN *pVIN, unsigned int lut0_en, unsigned int lut1_en, unsigned int lut2_en);
extern void VIOC_VIN_SetDemuxPort(VIOC_VIN_DEMUX *pVINDEMUX, unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3);
extern void VIOC_VIN_SetDemuxClock(VIOC_VIN_DEMUX *pVINDEMUX, unsigned int mode);
extern void VIOC_VIN_SetDemuxEnable(VIOC_VIN_DEMUX *pVINDEMUX, unsigned int enable);
extern void VIOC_VIN_IreqHandler(unsigned int vectorID);

#endif
