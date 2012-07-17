#ifndef __drv_isp_h__
#define __drv_isp_h__

#define DRVISP_HANDLER_SIZE   208
//#define DRVISP_HPROFILE_SIZE  328
#define DRVISP_HPROFILE_SIZE  12628
#define DRVISP_HEVENT_SIZE     40
//#define DRVISP_PRESET_MAX     40
enum {
	DRVISPPRESET_INIT = 0,
	DRVISPPRESET_EXIT,
	DRVISPPRESET_PREVIEW,
	DRVISPPRESET_CAPTURE,
	DRVISPPRESET_VIDEO,
	DRVISPPRESET_DEBUG,
	DRVISPPRESET_STREAMOUT,
	DRVISPPRESET_STOP,
	DRVISPPRESET_RESET,
	
	DRVISPPRESET_DPF0,
	DRVISPPRESET_DPF1,
	DRVISPPRESET_DPF2,
	DRVISPPRESET_DPF3,
	DRVISPPRESET_DPF4,
	DRVISPPRESET_DPF5,
	DRVISPPRESET_DPF6,
	DRVISPPRESET_DPF7,
	DRVISPPRESET_DPF8,
	
	DRVISPPRESET_EFT0,
	DRVISPPRESET_EFT1,
	DRVISPPRESET_EFT2,
	DRVISPPRESET_EFT3,
	DRVISPPRESET_EFT4,
	DRVISPPRESET_EFT5,
	DRVISPPRESET_EFT6,
	DRVISPPRESET_EFT7,
	DRVISPPRESET_EFT8,
	DRVISPPRESET_EFT9,
	
	DRVISPPRESET_PRESET_MAX
};

#ifndef DRV_ISP_INTERNAL
typedef int HIsp;
typedef int HProfile;
typedef int HEvent;
#endif 

typedef enum {
	DRVISPST_EMPTY = 0,
	DRVISPST_START ,
	DRVISPST_POWERDOWN,
	DRVISPST_IDLE,
	DRVISPST_STREAMING,
	DRVISPST_DEBUG,
	DRVISPST_MAX
} DRVISPST;
#define DRVISPST_MASK		0xff

/*
#define DRVISPST_INTERNAL	0x80000000
#define DRVISPSTIN_MASK	0xff00
typedef enum {
	DRVISPSTIN_EMPTY = 0,
	DRVISPST_START ,
	DRVISPST_POWERDOWN,
	DRVISPST_IDLE,
	DRVISPST_STREAMING,
	DRVISPST_DEBUG,
	DRVISPST_MAX
} DRVISPST_INTERNAL;
*/

#define DRVISPSTBIT_START  		1
#define DRVISPSTBIT_POWERDOWN  	2
#define DRVISPSTBIT_IDLE  		4
#define DRVISPSTBIT_STREAMING  	8
#define DRVISPSTBIT_DEBUG  		0x10


typedef enum
{
	DRVISPPSWITCH_DISABLE = 0,
	DRVISPPSWITCH_IE,
	DRVISPPSWITCH_SI,
	DRVISPPSWITCH_SELF,
	DRVISPPSWITCH_MAX
} DRVISPPSWITCH;	
/*
typedef enum
{
	DRVISPIE_BYPASS = 0,
	DRVISPIE_GRAYSCALE,			
	DRVISPIE_NEGATIVE,
	DRVISPIE_SEPIA,				//cr[7:0],			cb[7:0]
	DRVISPIE_COLORSELECTION,	//threshold[7:0], 	selection[2:0]
	DRVISPIE_EMBOSS,			//enable[8:0],    	coeff[9][2:0]
	DRVISPIE_SKETCH,			//enable[8:0],		coeff[9][2:0]
	DRVISPIE_SHARPEN,			//factor[3:0],		coring[7:0]
	DRVISPIE_MAX
} DRVISPIE;	
*/
typedef enum
{
	DRVISPIE_BYPASS = 0,
	DRVISPIE_GRAYSCALE,
	DRVISPIE_NEGATIVE,
	DRVISPIE_SEPIA,
	DRVISPIE_COLORSELECTION,
	DRVISPIE_EMBOSS,
	DRVISPIE_SKETCH,
	DRVISPIE_SHARPEN,
	DRVISPIE_MAX
} DRVISPIE;	

typedef enum
{	DRVISPMIMPIN_DIABLE = 0,
	DRVISPMIMPIN_BYPASS,
	DRVISPMIMPIN_ISP,
	DRVISPMIMPIN_DMA,		// reserved
	DRVISPMIMPIN_MAX
} DRVISPMIMPIN;

typedef enum
{	DRVISPMISPIN_DIABLE = 0,
	DRVISPMISPIN_BYPASS,
	DRVISPMISPIN_ISP,
	DRVISPMISPIN_DMA,		// reserved
	DRVISPMISPIN_MAX
} DRVISPMISPIN;

typedef struct
{
	unsigned colorsel;	// 2bit:R, 1bit:G, 0bit:B
	unsigned effmat1;
	unsigned effmat2;
	unsigned effmat3;
	unsigned effmat4;
	unsigned effmat5;
	unsigned sepia;
	unsigned sharpen;

} DRVISPIEDetail;

/*
#define DRVISPDTYPE_NONE			0
#define DRVISPDTYPE_RAW8			1
#define DRVISPDTYPE_RAW12			2
#define DRVISPDTYPE_JPEG			3
#define DRVISPDTYPE_RGB888			4
#define DRVISPDTYPE_RGB666			5
#define DRVISPDTYPE_RGB565			6
#define DRVISPDTYPE_YCbCr444		7
#define DRVISPDTYPE_YCbCr422		8
#define DRVISPDTYPE_YCbCr420		9
#define DRVISPDTYPE_YCbCr400		10
#define DRVISPDTYPE_DP				15
#define DRVISPDTYPE_MAX			   0xf
*/
#define DRVISPDTYPE_RAW8			0
#define DRVISPDTYPE_RAW12			1
#define DRVISPDTYPE_JPEG			2
#define DRVISPDTYPE_RGB888			3
#define DRVISPDTYPE_RGB666			4
#define DRVISPDTYPE_RGB565			5
#define DRVISPDTYPE_YCbCr444		6
#define DRVISPDTYPE_YCbCr422		7
#define DRVISPDTYPE_YCbCr420		8
#define DRVISPDTYPE_YCbCr400		9
#define DRVISPDTYPE_DP				15
#define DRVISPDTYPE_MAX			   0xf


#define DRVISPDTYPE_PLANAR			0x00	// for YCbCr type 
#define DRVISPDTYPE_SEMIPLANAR		0x10	// for YCbCr 4:2:x type
#define DRVISPDTYPE_INTERLEAVED		0x20	// for YCbCr 4:2:2 type only
#define DRVISPDTYPE_YUVTYPE_MAX			0x30	

#define DRVISPDTYPE_NONCOSITED		0x40	// for YCbCr type.(Cb and Cr samples are centered between Y samples)
#define DRVISPDTYPE_COSITED			0x00	// for YCbCr type.(Y0 Cb0 and Cr0 are sampled at the same position)9

#define DRVISPDTYPE_FULLRANGE		0x80	// (0..255)
#define DRVISPDTYPE_COMPRESSEDRANGE	0x00	// Y(16..235), CbCr(16..240)

#define DRVISPDTYPE_NORMAL			0x000
#define DRVISPDTYPE_ROT90			0x100
#define DRVISPDTYPE_ROT180			0x600
#define DRVISPDTYPE_ROT270			0x700
#define DRVISPDTYPE_VFLIP			0x200
#define DRVISPDTYPE_HFLIP			0x400

#define DRVISPDTYPE_MIPI			0x000
#define DRVISPDTYPE_PARALLEL		0x800

#define DRVISPORIENTATION_ROT		1		// Rotation 90 depree counter clockwise
#define DRVISPORIENTATION_VFLIP		2		// Vertical flipping
#define DRVISPORIENTATION_HFLIP		4		// Horizontal flipping

#define DRVISPAPPLY_IMMEDIATELY			0		// Update now.
#define DRVISPAPPLY_ENDOFFRAME			1		// Update at the end of frame.
#define DRVISPAPPLY_MODECHANGE			2		// Update at mode change.
#define DRVISPAPPLY_LATER				3		//
#define DRVISPAPPLY_UPDATEALL			4		//
#define DRVISPAPPLY_UPDATE_MAX			0xff	
#define DRVISPAPPLY_NORMAL				0x000
#define DRVISPAPPLY_SMART				0x100

#define DRVISPMODE_MIPI_RAW12			0
#define DRVISPMODE_PARALLEL_RAW12		0x10
#define DRVISPMODE_PARALLEL_RAW8		0x11
#define DRVISPMODE_PARALLEL_YUV			0x12
#define DRVISPMODE_PARALLEL_DATA		0x13

#define DRVISPACQ_REF2PRESET			0xffffffff

#define DRVISPACQ_MODE_MASK				0x3
#define DRVISPACQ_MODE_MIPI				0x1	
#define DRVISPACQ_MODE_PARALLEL			0x2

#define DRVISPACQ_SYNC_MASK				0xC
#define DRVISPACQ_SYNC_BT601			0x4
#define DRVISPACQ_SYNC_BT656			0x8

#define DRVISPACQ_PCLKPOL_MASK			0x30
#define DRVISPACQ_PCLKPOL_NEGA			0x10
#define DRVISPACQ_PCLKPOL_POSI			0x20

#define DRVISPACQ_HSYNCPOL_MASK			0xC0
#define DRVISPACQ_HSYNCPOL_HIGH			0x40
#define DRVISPACQ_HSYNCPOL_LOW			0x80

#define DRVISPACQ_VSYNCPOL_MASK			0x300
#define DRVISPACQ_VSYNCPOL_HIGH			0x100
#define DRVISPACQ_VSYNCPOL_LOW			0x200

#define DRVISPACQ_BIT_MASK				0x1C00
#define DRVISPACQ_BIT_12				0x0400
#define DRVISPACQ_BIT_10ZERO			0x0800
#define DRVISPACQ_BIT_10MSB				0x0c00
#define DRVISPACQ_BIT_8ZERO				0x1000
#define DRVISPACQ_BIT_8MSB				0x1400

#define DRVISPACQ_CCIRSEQ_MASK			0xe000
#define DRVISPACQ_CCIRSEQ_YCBYCR		0x2000
#define DRVISPACQ_CCIRSEQ_YCRYCB		0x4000
#define DRVISPACQ_CCIRSEQ_CBYCRY		0x6000
#define DRVISPACQ_CCIRSEQ_CRYCBY		0x8000

#define DRVISPACQ_BAYERPAT_MASK			0x70000
#define DRVISPACQ_BAYERPAT_RGGB			0x10000
#define DRVISPACQ_BAYERPAT_GRBG			0x20000
#define DRVISPACQ_BAYERPAT_GBRG			0x30000
#define DRVISPACQ_BAYERPAT_BGGR			0x40000

#define DRVISPACQ_FIELD_MASK			0x300000
#define DRVISPACQ_FIELD_ALL				0x100000
#define DRVISPACQ_FIELD_EVEN			0x200000
#define DRVISPACQ_FIELD_ODD				0x300000

#define DRVISPACQ_CONV422_MASK			0xc00000
#define DRVISPACQ_CONV422_COSITED		0x400000
#define DRVISPACQ_CONV422_INTERLEAVED	0x800000
#define DRVISPACQ_CONV422_NONCOSITED	0xc00000

// for reset
#define DRVISPRST_MIPI 	0x00000800
#define DRVISPRST_SMIA	0x00000400
#define DRVISPRST_SIMP	0x00000200
#define DRVISPRST_IE		0x00000100
#define DRVISPRST_ALL	0x00000080
#define DRVISPRST_MI		0x00000040
#define DRVISPRST_JPEG	0x00000020
#define DRVISPRST_SRSZ	0x00000010
#define DRVISPRST_MRSZ	0x00000008
#define DRVISPRST_YCS	0x00000004
#define DRVISPRST_CP	0x00000002
#define DRVISPRST_ISP	0x00000001

///////////////////////////////////////////////////
// Event handler
///////////////////////////////////////////////////

// for event1
#define DRVISPINTMIPI_GEN_SHORT_PACK			0x08000000
#define DRVISPINTMIPI_ADD_DATA_FILL_LEVEL		0x04000000
#define DRVISPINTMIPI_ADD_DATA_OVFLW			0x02000000
#define DRVISPINTMIPI_FRAME_END					0x01000000
#define DRVISPINTMIPI_ERR_CS						0x00800000
#define DRVISPINTMIPI_ERR_ECC1					0x00400000
#define DRVISPINTMIPI_ERR_ECC2					0x00200000
#define DRVISPINTMIPI_ERR_PROTOCOL				0x00100000
#define DRVISPINTMIPI_ERR_CONTROL_4LANE		0x00080000
#define DRVISPINTMIPI_ERR_CONTROL_3LANE		0x00040000
#define DRVISPINTMIPI_ERR_CONTROL_2LANE		0x00020000
#define DRVISPINTMIPI_ERR_CONTROL_1LANE		0x00010000
#define DRVISPINTMIPI_ERR_EOT_SYNC_4LANE		0x00008000
#define DRVISPINTMIPI_ERR_EOT_SYNC_3LANE		0x00004000
#define DRVISPINTMIPI_ERR_EOT_SYNC_2LANE		0x00002000
#define DRVISPINTMIPI_ERR_EOT_SYNC_1LANE		0x00001000
#define DRVISPINTMIPI_ERR_SOT_SYNC_4LANE		0x00000800
#define DRVISPINTMIPI_ERR_SOT_SYNC_3LANE		0x00000400
#define DRVISPINTMIPI_ERR_SOT_SYNC_2LANE		0x00000200
#define DRVISPINTMIPI_ERR_SOT_SYNC_1LANE		0x00000100
#define DRVISPINTMIPI_ERR_SOT_4LANE				0x00000080
#define DRVISPINTMIPI_ERR_SOT_3LANE				0x00000040
#define DRVISPINTMIPI_ERR_SOT_2LANE				0x00000020
#define DRVISPINTMIPI_ERR_SOT_1LANE				0x00000010
#define DRVISPINTMIPI_SYNC_FIFO_OVFLW_4LANE	0x00000008
#define DRVISPINTMIPI_SYNC_FIFO_OVFLW_3LANE	0x00000004
#define DRVISPINTMIPI_SYNC_FIFO_OVFLW_2LANE	0x00000002
#define DRVISPINTMIPI_SYNC_FIFO_OVFLW_1LANE	0x00000001
           
// for event2
#define DRVISPINTMI_DMA_READY 	0x00000800
#define DRVISPINTMI_AHB_ERROR		0x00000400
#define DRVISPINTMI_WRAP_SP_CR	0x00000200
#define DRVISPINTMI_WRAP_SP_CB	0x00000100
#define DRVISPINTMI_WRAP_SP_Y		0x00000080
#define DRVISPINTMI_WRAP_MP_CR	0x00000040
#define DRVISPINTMI_WRAP_MP_CB	0x00000020
#define DRVISPINTMI_WRAP_MP_Y		0x00000010
#define DRVISPINTMI_FILL_MP_Y		0x00000008
#define DRVISPINTMI_MBLK_LINE		0x00000004
#define DRVISPINTMI_SP_FRAME_END		0x00000002
#define DRVISPINTMI_MP_FRAME_END		0x00000001

// for event3
#define DRVISPINT_EXP_END        			0x00040000 //[18] Exposure measurement complete
#define DRVISPINT_FLASH_CAP        		0x00020000 //[17] Signaling captured frame
#define DRVISPINT_BP_DET				0x00010000 //[16] Bad pixel detected
#define DRVISPINT_BP_NEW_TAB_FUL		0x00008000 //[15] New Bad Pixel table is full
#define DRVISPINT_AFM_FIN          		0x00004000 //[14] AF measurement completely
#define DRVISPINT_AFM_LUM_OF       		0x00002000 //[13] overflow luminance
#define DRVISPINT_AFM_SUM_OF       		0x00001000 //[12] overflow summation of sharpness
#define DRVISPINT_SHUTTER_OFF      	0x00000800 //[11] Mechanical shutter is switched off
#define DRVISPINT_SHUTTER_ON       		0x00000400 //[10] Mechanical shutter is switched on
#define DRVISPINT_FLASH_OFF        		0x00000200 //[ 9] Flash light is switched off
#define DRVISPINT_FLASH_ON         		0x00000100 //[ 8] Flash light is switched on
#define DRVISPINT_HSTART           		0x00000080 //[7] hsync from sensor was detected
#define DRVISPINT_VSTART           		0x00000040 //[6] vsync from sensor was detected
#define DRVISPINT_IN_FRAME_END     		0x00000020 //[5] input image was sampled completely
#define DRVISPINT_AWB_DONE         		0x00000010 //[4] white balancing measurement cycle complete,results can be read out
#define DRVISPINT_PIC_ERR          		0x00000008 //[3] input picture size violation occurred,programming seems to be wrong
#define DRVISPINT_DATA_LOSS        		0x00000004 //[2] loss of data occurred within an active line,processing failure
#define DRVISPINT_OUT_FRAME_END    	0x00000002 //[1] current frame was completely put out
#define DRVISPINT_ISP_OFF    			0x00000001 //[0] Isp output was disabled due to f_cnt reached

// for event4
#define DRVISPINTJPE_VLC_TABLE_ERR 		0x04000000
#define DRVISPINTJPE_R2B_IMG_SIZE_ERR 	0x02000000
#define DRVISPINTJPE_DCT_ERR 				0x00800000
#define DRVISPINTJPE_VLC_SYMBOL_ERR 	0x00100000
#define DRVISPINTJPE_GEN_HEADER_DONE 	0x00000020
#define DRVISPINTJPE_ENCODE_DONE 		0x00000010

typedef struct {
	int (*I2cInit) ();
	int (*I2cExit) ();
	int (*I2cRead) ( unsigned slaveaddr, unsigned subaddr, unsigned *data, unsigned mode);
	int (*I2cWrite) (unsigned slaveaddr, unsigned subaddr, unsigned data, unsigned mode);

	int (*TransInit) (HIsp hisp );
	int (*TransExit) (HIsp hisp );
	int (*TransTurnOn) (HIsp hisp );
	int (*TransTurnOff) (HIsp hisp );
	int (*TransWakeUp) (HIsp hisp );
	int (*TransSleep) (HIsp hisp );
	int (*TransStream) (HIsp hisp );
	int (*TransStop) (HIsp hisp );
	int (*TransDebugOn) (HIsp hisp );		// 3A lock, adaptivefunc lock 
	int (*TransDebugOff) (HIsp hisp );

	int (*TransHalfShutter) (HIsp hisp );	// AW lock, AF lock, 외부에서 처리해야할경우 사용
	int (*TransRelease) (HIsp hisp );
	
	//int (*Flesh) (HIsp hisp, unsigned onoff);
	int (*Flesh) (unsigned onoff);
} DRVISPCBFunc;

typedef struct
{
	unsigned short devid;
	unsigned short addr;
	unsigned       data;

} DRVISPPreset;

typedef struct DRVISPJpegInfo {
	unsigned 	size;
	unsigned char *addr;
	unsigned 	tickcount;
	unsigned 	framenum;
	unsigned 	flags;
	unsigned 	idx;
	unsigned	iso;
	unsigned	exposure;
	unsigned	wb;
} DRVISPJpegInfo;

typedef struct DRVISPImgInfo {
	unsigned 	mp_type;
	unsigned char *mp_addr1;
	unsigned char *mp_addr2;
	unsigned char *mp_addr3;
	unsigned	mp_size1;
	unsigned	mp_size2;
	unsigned	mp_size3;
	
	unsigned 	sp_type;
	unsigned char *sp_addr1;
	unsigned char *sp_addr2;
	unsigned char *sp_addr3;
	unsigned	sp_size1;
	unsigned	sp_size2;
	unsigned	sp_size3;
	
	unsigned 	tickcount;
	unsigned 	framenum;
	unsigned 	flags;
	unsigned 	idx;
	unsigned	iso;
	unsigned	exposure;
	unsigned	wb;
} DRVISPImgInfo;

#define DRVISP_PRESET_MAX 		1000	// 

typedef struct {
	int	offx;
	int	offy;
	int	width;
	int	height;
	int	xpitch;
	int	ypitch;	
} DRVISPRegion;

typedef struct {
	int	offx;
	int	offy;
	int	width;
	int	height;
} DRVISPRect;

typedef struct {
	int	width;
	int	height;
} DRVISPRectSize;

typedef struct {
	int acquisition;
	int isp;
	int mi;
	int jpeg;
} DRVISPProperties;

/**
 *  load preset 
 * @param hisp	 	[in] isp handler
 * @param idx    	[in] preset index
 * @param str    	[in] preset string
 * @param reserved  [in] default 0 value
 * @return error code. 
 */
int drvisp_loadpreset(HIsp hisp, int idx, char* str, int reserved);

/**
 *  make profile 
 * @param hisp	 	[in] isp handler
 * @param hevent 	[in] event handler
 * @param reserved  [in] default 0 value
 * @return error code. 
 */
int drvisp_makeevent(HIsp hisp, HEvent hevent, int reserved);

/**
 *  make profile 
 * @param hisp	 	[in] isp handler
 * @param hprofile 	[in] profile handler
 * @param reserved  [in] default 0 value
 * @return error code. 
 */
int drvisp_makeprofile(HIsp hisp, HProfile hprofile, DRVISPPreset *preset, int reserved);

/**
 *  set mode
 * @param hisp	 	[in] isp handler
 * @param mode		[in] 
 * @param reserved  [in] default 0 value
 * @return error code. 
 */
int drvisp_setmode(HIsp hisp, int mode, int reserved );

int	drvispprofile_setacq(HProfile hprofile, unsigned prop, unsigned hoffs, unsigned voffs, unsigned hsize, unsigned vsize );

/**
 *  set properties 
 * @param hprofile 		[in] profile handler
 * @param properties	[in] refer to DRVISPProperties struct
 * @param apply			[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return error code. 
 */
int drvispprofile_setproperties(HProfile hprofile, DRVISPProperties* properties , int apply );

/**
 *  color processing  
 * @param hprofile 	[in] profile handler
 * @param enable	[in] 1: color processing is active(contrast, brightness, saturation, hue)
 * @param range		[in] [2] c out range        1 : full uv, 0 : 16..240
 *                       [1] y in range			1 : full y,  0 : 64..940
 *                       [0] y out range		1 : full y,  0 : 16..235
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return error code. 
 */
int drvispprofile_setcproc(HProfile hprofile, int enable, int range, int apply );

/**
 *  color processing - contrast control
 * @param hprofile 	[in] profile handler
 * @param contrast  [in] [7:0] 0.0 - 1.922
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return error code. 
 */
int drvispprofile_setcontrast(HProfile hprofile, int contrast, int apply );

/**
 *  color processing - brightness control
 * @param hprofile 	[in] profile handler
 * @param brightness[in] [7:0] -128 - +127
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return error code. 
 */
int drvispprofile_setbrightness(HProfile hprofile, int brightness, int apply );

/**
 *  color processing - saturation control
 * @param hprofile 	[in] profile handler
 * @param saturation[in] [7:0] 0.0 - 1.922
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return error code. 
 */
int drvispprofile_setsaturation(HProfile hprofile, int saturation, int apply );

/**
 *  color processing - hue control  
 * @param hprofile 	[in] profile handler
 * @param brightness[in] [7:0] -90 deg - +87.188 deg
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return error code. 
 */
int drvispprofile_sethue(HProfile hprofile, int hue, int apply );


int drvispprofile_setie(HProfile hprofile, int mode, int detail1, int detail2, int apply);

/**
 *  Super Impose
 * @param hprofile 		[in] profile handler
 * @param overlay		[in] set area of overlay image
 * @param chromakey_y	[in] chroma key y value
 * @param chromakey_cb	[in] chroma key cb value
 * @param chromakey_cr	[in] chroma key cr value
 * @param properties 	[in] 0 : bypass, not 0 : active
 * @return error code.
 * @remark camera에서 들어오는 이미지는 resize적용되기 전 이미지.
 *                   
 */
int drvispprofile_setsi(HProfile hprofile, DRVISPRect* overlay, int chromakey_y, int chromakey_cb, int chromakey_cr, int properties );

/**
 *  Jpeg
 * @param hprofile 	[in] profile handler
 * @param quality   [in] quality factor
 * @param reserved  [in] default 0 value
 * @return error code.
 *                   
 */
int drvispprofile_setjpeg(HProfile hprofile, int quality, int reserved );

/**
 *  set window
 * @param hprofile 	[in] profile handler
 * @param window 	[in] input size and offset
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return 			error code. 
 * @remark 			output size는 drvisp_setprofile_mpath()의 region parameter에서 정해짐
 *					4배까지 확대 가능 
 */
int drvispprofile_setwindow(HProfile hprofile, DRVISPRect* window , int apply);
int drvispprofile_setzoom(HProfile hprofile, int level, int apply);
int drvispprofile_setzoomtbl(HProfile hprofile, int level, DRVISPRect* zoom, int reserved);
//int drvispprofile_zoom_apply(HProfile hprofile, int reserved);
int drvisp_zoom_apply(HIsp hisp, int reserved);

int drvisp_reset_apply (HIsp hisp, int mask);

#if 0
/**
 *  Resizing function for mainpath
 * @param hprofile 	[in] profile handler
 * @param window 	[in] input size and offset
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return 			error code. 
 * @remark 			output size는 drvisp_setprofile_mpath()의 region parameter에서 정해짐
 *					4배까지 확대 가능 
 */
int drvispprofile_setmrsz(HProfile hprofile, DRVISPRect* window , int apply);

/**
 *  Resizing function for selfpath
 * @param hprofile 	[in] profile handler
 * @param window 	[in] input size and offset
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return 			error code. 
 * @remark 			output size는 drvisp_setprofile_spath()의 region parameter에서 정해짐
 *					output size는 1280*960 size를 초과할 수 없다.
 *					4배까지 확대 가능,  
 */
int drvispprofile_setsrsz(HProfile hprofile, DRVISPRect* window , int apply);
#endif

/**
 *  Set main path 
 * @param hprofile 	[in] profile handler
 * @param num	 	[in] frame buffer에 저장할 frame의 개수.  
 *                       0 : disabled 
 *						 1 : single	frame (1개의 frame)
 *						 n : multi frames (n개의 frame)
 *						0x80000000 : continuous frames - frame buffer에 무한 반복 저장을 한다. 
 * @param skip		[in] skip frame. 
 * @param addr	 	[in] memory address of frame buffer.
 * @param size	 	[in] size of one frame. (byte) 
 * @param rectsize 	[in] size of image. (width, height)
 * @param dtype	 	[in] image data type. refer to DRVISPDTYPE_XXX define.
 * @param reserved  [in] default 0 value.
 * @return 			error code. 
 * @remark 			frame buffer의 크기는 size에 의해 제한 된다.
 *					dtype으로 DRVISPDTYPE_RGBxxx type은 사용 할 수 없다.
 */
int drvispprofile_setmpath (HProfile hprofile, int num, int skip, void *addr, int size, DRVISPRectSize* rectsize, int dtype, int reserved); 
int drvispprofile_setmpath_number (HProfile hprofile, int num, int reserved);  
int drvispprofile_setmpath_size (HProfile hprofile, DRVISPRectSize* rectsize, int reserved); 
int drvispprofile_setmpath_type (HProfile hprofile, int dtype, int reserved); 

/**
 *  Set self path 
 * @param hprofile 	[in] profile handler
 * @param num	 	[in] frame buffer에 저장할 frame의 개수.  
 *                       0 : disabled 
 *						 1 : single	frame (1개의 frame)
 *						 n : multi frames (n개의 frame)
 *						0x80000000 : continuous frames - frame buffer에 무한 반복 저장을 한다. 
 * @param skip		[in] skip frame. 
 * @param addr	 	[in] memory address of frame buffer.
 * @param size	 	[in] size of one frame. (byte) 
 * @param region 	[in] region of image. 
 * @param dtype	 	[in] image data type. refer to DRVISPDTYPE_XXX define.
 * @param orientation [in] image orientation ( rotate 0, 90, 180, 270, mirror, flip)
 * @param reserved  [in] default 0 value.
 * @return 			error code. 
 * @remark 			frame buffer의 크기는 size에 의해 제한 된다.
 *					dtype으로 DRVISPDTYPE_RAWxxx, DRVISPDTYPE_JPEG type은 사용 할 수 없다.
 *                  output size는 1280*960 size를 초과할 수 없다.
 *                  ( rotate를 적용할 경우 추가적인 size 제약이 있을 수 있다.)
 */
int drvispprofile_setspath (HProfile hprofile, int num, int skip, void *addr, int size, DRVISPRegion* region, int dtype, int orientation, int reserved);
int drvispprofile_setspath_number (HProfile hprofile, int num, int reserved);  
int drvispprofile_setspath_region (HProfile hprofile, DRVISPRegion* region, int reserved); 
int drvispprofile_setspath_type (HProfile hprofile, int dtype, int reserved); 
/**
 *  Set read DMA path - 메모리에 저장된 영상을 읽어 IE,SI,JPEG,RSZ 등을 선택적으로 적용할 수 있도록 한다.
 * @param hprofile 	[in] profile handler
 * @param pswitch 	[in] select data path. refer to DRVISPPSWITCH_XXX 
 * @param num	 	[in] frame buffer에 저장할 frame의 개수.  
 *                       0 : disabled 
 *						 1 : single	frame (1개의 frame)
 *						 n : multi frames (n개의 frame)
 *						0x80000000 : continuous frames - frame buffer에 무한 반복 저장을 한다. 
 * @param addr	 	[in] memory address of frame buffer.
 * @param region 	[in] region of image. 
 * @param dtype	 	[in] image data type. refer to DRVISPDTYPE_XXX define.
 * @param reserved  [in] default 0 value.
 * @return 			error code. 
 * @remark 			frame buffer의 크기는 size에 의해 제한 된다.
 *					SI를 사용하기 위해 DRVISPPSWITCH_SI 설정.      
 */
int drvispprofile_setrpath (HProfile hprofile, int pswitch, int num, void *addr, DRVISPRegion* region, int dtype, int reserved);
int drvispprofile_setrpath_number (HProfile hprofile, int num, int reserved);  
int drvispprofile_setrpath_region (HProfile hprofile, DRVISPRegion* region, int reserved); 
int drvispprofile_setrpath_type (HProfile hprofile, int dtype, int reserved); 
/**
 *  Set profile - hprofile을 일괄적으로 isp에 적용한다. 
 * @param hisp		[in] isp handler
 * @param hprofile 	[in] profile handler
 * @param apply		[in] 적용시점을 정한다. refer to DRVISPAPPLY_XXX 
 * @return 			error code. 
 */
int drvisp_setprofile(HIsp hisp, HProfile hprofile, int apply);

/**
 *  set address of main frame buffer. 
 * @param hprofile 	[in] profile handler
 * @param addr1		[in] address1 (dtype : Y, RGB, raw, jpeg) 
 * @param addr2		[in] address2 (dtype : U, UV)
 * @param addr3		[in] address3 (dtype : V)
 * @param size1		[in] size1
 * @param size2		[in] size2
 * @param size3		[in] size3
 * @param uid		[out] user define id. (0 - 255) uid must be unique of the frame.
 * @return 			error code. 
 */
int drvispprofile_setmfb(HProfile hprofile, void *addr1, void *addr2, void *addr3, int size1, int size2, int size3, int uid, int apply );
int drvispprofile_setmfb_addr(HProfile hprofile, void *addr1, void *addr2, void *addr3, int apply);
int drvispprofile_setmfb_size(HProfile hprofile, unsigned size1, unsigned size2, unsigned size3, int reserved);


/**
 *  set address of self frame buffer. 
 * @param hprofile 	[in] profile handler
 * @param addr1		[in] address1 (dtype : Y, RGB, raw, jpeg) 
 * @param addr2		[in] address2 (dtype : U, UV)
 * @param addr3		[in] address3 (dtype : V)
 * @param size1		[in] size1
 * @param size2		[in] size2
 * @param size3		[in] size3
 * @param uid		[out] user define id. (0 - 255) uid must be unique of the frame.
 * @return 			error code. 
 */
int drvispprofile_setsfb(HProfile hprofile, void *addr1, void *addr2, void *addr3, int size1, int size2, int size3, int uid, int apply );
int drvispprofile_setsfb_addr(HProfile hprofile, void *addr1, void *addr2, void *addr3, int apply);
int drvispprofile_setsfb_size(HProfile hprofile, unsigned size1, unsigned size2, unsigned size3, int reserved);


/**
 *  get address of main frame buffer. 
 * @param hprofile 	[in] profile handler
 * @param addr1		[out] address1 (dtype : Y, RGB, raw, jpeg) 
 * @param addr2		[out] address2 (dtype : U, UV)
 * @param addr3		[out] address3 (dtype : V)
 * @param size1		[out] size1
 * @param size2		[out] size2
 * @param size3		[out] size3
 * @param hfb		[out] id of frame buffer.
 * @return 			error code. 
 */
int drvispprofile_getmfb(HProfile hprofile, void **addr1, void **addr2, void **addr3, int *size1, int *size2, int *size3, int *hfb );

/**
 *  get address of self frame buffer. 
 * @param hprofile 	[in] profile handler
 * @param addr1		[out] address1 (dtype : Y, RGB, raw, jpeg) 
 * @param addr2		[out] address2 (dtype : U, UV)
 * @param addr3		[out] address3 (dtype : V)
 * @param size1		[out] size1
 * @param size2		[out] size2
 * @param size3		[out] size3
 * @param hfb	  	[out] id of frame buffer.
 * @return 			error code. 
 */
int drvispprofile_getsfb(HProfile hprofile, void **addr1, void **addr2, void **addr3, int *size1, int *size2, int *size3, int *hfb );

/**
 *  release main frame buffer. 
 * @param hprofile 	[in] profile handler
 * @param hfb		[in] id of frame buffer
 * @return 			error code. 
 */
int drvispprofile_releasemfb(HProfile hprofile, int hfb);

/**
 *  release self frame buffer. 
 * @param hprofile 	[in] profile handler
 * @param hfb		[in] id of frame buffer
 * @return 			error code. 
 */
int drvispprofile_releasesfb(HProfile hprofile, int hfb);

/**
 *  initialize drvisp. drvisp 초기화.
 * @param hprofile 		[in] profile handler
 * @param sensorinfo	[in] 해당 센서의 init value와 preset value들
 * @param cbfunc		[in] call back functions
 * @return 			error code. 
 */
int drvisp_init(HIsp hisp, DRVISPPreset **preset, DRVISPCBFunc *cbfunc );
                           
/**
 *  exit drvisp. drvisp 종료.
 * @param hisp	 	[in] isp handler
 * @return 			error code. 
 */
int drvisp_exit(HIsp hisp);

/**
 *  set status. 
 * @param hisp	 	[in] isp handler
 * @param state	 	[in] set isp status
 * @return 			error code. 
 */
int drvisp_setstatus (HIsp hisp, int state);

/**
 *  get status.  현재의 isp status를 얻어온다.
 * @param hisp	 	[in] isp handler
 * @return 			[out] status. 
 */
//int drvisp_getstatus (HIsp hisp);
int drvisp_getstatus (HIsp hisp, int *state);	
/**
 *  get sensor clock. 
 * @param hisp	 	[in] isp handler
 * @param state	 	[in] isp handler
 * @return 			[out] sensor clock. 
 */
int drvisp_getsensorclk(HIsp hisp);

/**
 *  get isp clock. 
 * @param hisp	 	[in] isp handler
 * @param state	 	[in] isp handler
 * @return 			[out] isp clock. 
 */
int drvisp_getclk (HIsp hisp);

/**
 *  set image orientation. 
 * @param hisp	 		[in] isp handler
 * @param orientation 	[in] image orientation ( normal, mirror, flip, rotate 180 degree )
 * @return 				[out] error code. 
 */
int drvisp_setfilp (HIsp hisp, int orientation, int apply);

int drvisp_sensoridx(HIsp hisp);

int drvisp_getjpeginfo(HIsp hisp, int idx, int reserved, DRVISPJpegInfo *jpeginfo);

// for MIPI Interrupt
unsigned	drvispevent_waitmipi(HEvent hevent, unsigned events, unsigned timeout);
unsigned	drvispevent_ctrlmipi(HEvent hevent, unsigned events);
unsigned	drvispevent_maskmipi(HEvent hevent, unsigned events);
unsigned	drvispevent_unmaskmipi(HEvent hevent, unsigned events);

// for MI Interrupt
unsigned	drvispevent_waitmi(HEvent hevent, unsigned events, unsigned timeout);
unsigned	drvispevent_ctrlmi(HEvent hevent, unsigned events);
unsigned	drvispevent_maskmi(HEvent hevent, unsigned events);
unsigned	drvispevent_unmaskmi(HEvent hevent, unsigned events);

// for ISP Interrupt
unsigned	drvispevent_waitisp(HEvent hevent, unsigned events, unsigned timeout);
unsigned	drvispevent_ctrlisp(HEvent hevent, unsigned events);
unsigned	drvispevent_maskisp(HEvent hevent, unsigned events);
unsigned	drvispevent_unmaskisp(HEvent hevent, unsigned events);

// for JPEG Interrupt
unsigned	drvispevent_waitjpe(HEvent hevent, unsigned events, unsigned timeout);
unsigned	drvispevent_ctrljpe(HEvent hevent, unsigned events);
unsigned	drvispevent_maskjpe(HEvent hevent, unsigned events);
unsigned	drvispevent_unmaskjpe(HEvent hevent, unsigned events);

unsigned drvisp_irq1_noti( HIsp hisp );
unsigned drvisp_irq2_noti( HIsp hisp );
unsigned drvisp_irq3_noti( HIsp hisp );
unsigned drvisp_irq4_noti( HIsp hisp );

unsigned drvisp_chkirq( HIsp hisp );
void drvisp_clrirq(HIsp hisp );

// 모드 전환시 처리해야 할 내용 
int drvispmode_init	(HIsp hisp);
int drvispmode_process(HIsp hisp);
int drvispmode_exit	(HIsp hisp);

int drvisp3A_init	(HIsp hisp);
int drvisp3A_process(HIsp hisp, unsigned param1, unsigned param2, unsigned param3) ;
int drvisp3A_exit	(HIsp hisp);
int drvisp3A_enable(HIsp hisp);
int drvisp3A_disable(HIsp hisp);

int drvispAdptvFunc_init	(HIsp hisp);
int drvispAdptvFunc_process(HIsp hisp, unsigned param1, unsigned param2, unsigned param3) ;
int drvispAdptvFunc_exit	(HIsp hisp);

int drvisp_stop_irq_apply(HIsp hisp);

#endif // __drv_isp_h__
