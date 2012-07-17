
/****************************************************************************
 *   FileName    : viqe_lib.h
 *   Description : 
 ****************************************************************************
 *
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
#ifndef	_VIQE_LIB_H_
#define	_VIQE_LIB_H_

#define		ON	1
#define		OFF	0


enum	viqe_const_RDFMT_t	{
	FMT_YUV420SEP = 0,
	FMT_YUV422SEP = 1,
	FMT_YUV422SEQ = 2,
	FMT_YUV420IL0 = 4,
	FMT_YUV420IL1 = 5,
	FMT_YUV422IL0 = 6,
	FMT_YUV422IL1 = 7
};

enum	viqe_const_ODFMT_t	{
	OD_YUV422SEQ0 = 0,
	OD_YUV422SEQ1 = 1,
	OD_YUV422SEP  = 2,
	OD_YUV420SEP  = 3,
	OD_YUV422INT  = 10,
	OD_YUV420INT  = 11
};

typedef enum
{
	VIQE_STATE_IDLE = 0,
	VIQE_STATE_RUN,
	VIQE_STATE_MAX
} viqe_state_t;


#ifdef CLIP3
#undef CLIP3
#endif
#define	CLIP3( x, min, max )	( ( (x) < (min) )? (min) : \
								  ( (x) > (max) )? (max) : \
								  (x) )
#ifdef MIN3
#undef MIN3
#endif
#define	MIN3( x, y, z )			( ( (x) > (y) )? \
								  ( ( (y) > (z) )?  (z) : (y) ) : \
								  ( ( (x) > (z) )?  (z) : (x) ) )
#ifdef MAX3
#undef MAX3
#endif
#define	MAX3( x, y, z )			( ( (x) > (y) )? \
								  ( ( (x) > (z) )?  (x) : (z) ) : \
								  ( ( (y) > (z) )?  (y) : (z) ) )


typedef struct	{
	int				di;
	int				disp;
	int				dnts;
	int				dnsp;
	int				hpf;
	int				his;
	int				gamut;
	int				oddfirst;
}	viqe_modes_t;

typedef struct	{
	int				i_fmt;
	int				o_fmt;
	int				force_420;
	int				first_frm_init;
	int				fmt_conv_disable;
}	viqe_imgfmt_t;


typedef struct	{
	unsigned int		addr_cur[3];
	unsigned int		addr_pre[3];
	// compressor frame address
	unsigned int		addr_comp0[3];
	unsigned int		addr_comp1[3];
	int				comp_pingpong;

	unsigned int		addr_out[3];

	// input image width offsets
	unsigned int		i_offs[3];
	// output image width offsets
	unsigned int		o_offs[3];

	int				conti;
	int				opt;

	// interleaved
	int				interleaved;
}	viqe_dma_t;


typedef struct	{
	// enable of denoise recursive path compressor
	int				comp_en;
	// manually frame-restarting
	int				frm_manual_restart;
	// enable of outdma-to-lcd/mscl directpath
	int				directpath_en;
	//lclc0 : 0, lcdc1 : 1, mscl0 : 2, mscl1 : 3
	int				directpath_op;
	// disable of block-overlapping
	int				overlap_disable;
}	viqe_cfg_t;


typedef struct	{
	int					width;
	int					height;

	viqe_modes_t		*modes;
	viqe_imgfmt_t		*imgfmt;
	viqe_dma_t			*dma;
	viqe_cfg_t			*cfg;
}	viqe_t;



// -------------------------------------------------------
// Error codes
// -------------------------------------------------------
enum	viqe_const_err_t	{
	RET_VIQE_EN_DI_DNSP		= -101,
	RET_VIQE_EN_DN_TSSP		= -102,
	RET_VIQE_EN_ERRREG		= -109
};

void viqe_set_state(int *arg);
viqe_state_t viqe_get_state(void);

// -----------------------------------------------------------------
// viqe viqe_clk_onoff
//
// 		input : 	-
// 		output :	-
// -----------------------------------------------------------------
void			viqe_clk_onoff( int onOff );

// -----------------------------------------------------------------
// viqe VIQE_SWReset
//
// 		input : 	-
// 		output :	-
// -----------------------------------------------------------------
void 			viqe_swreset( void );

// -----------------------------------------------------------------
// clearing internal states and internal registers in all blocks
//
// 		input : 	-
// 		output :	return success or fail(-1)
// -----------------------------------------------------------------
int				viqe_clear_internal( void );

// -----------------------------------------------------------------
// register writing of viqe_dma_t structure
//
// 		input : 	pointer of viqe_dma_t structure
// 		output :	base0 luminance address
// -----------------------------------------------------------------
unsigned int 	viqe_set_dma( viqe_dma_t *dma );

// -----------------------------------------------------------------
// register writing of varying dma addresses per frame processing
//
// 		input : 	pointer of viqe_dma_t structure
// 		output :	base0 luminance address
// -----------------------------------------------------------------
unsigned int 	viqe_set_dma_addr( viqe_dma_t *dma );

// -----------------------------------------------------------------
// register writing of viqe_imgfmt_t structure
//
// 		input : 	pointer of viqe_imgfmt_t structure
// 		output :	-
// -----------------------------------------------------------------
int 			viqe_set_imgfmt( viqe_imgfmt_t *imgfmt );

// -----------------------------------------------------------------
// register writing of viqe_cfg_t structure
//
// 		input : 	pointer of viqe_cfg_t structure
// 		output :	-
// -----------------------------------------------------------------
int 			viqe_set_cfg( viqe_cfg_t *cfg );

// -----------------------------------------------------------------
// disable all blocks in viqe
//
// 		input : 	-
// 		output :	return success or fail(-1)
// -----------------------------------------------------------------
int 			viqe_set_disable_all( void );

// -----------------------------------------------------------------
// viqe initialization
// this function is an example, and there can be other initialization sequences
//
// 		input : 	viqe_t structure
// 		output :	return success or fail(-1)
// -----------------------------------------------------------------
int 			viqe_init( viqe_t *viqe );

// -----------------------------------------------------------------
// return whether current frame processing is done or not
//
// 		input : 	-
// 		output :	return done(1) or not(0)
// -----------------------------------------------------------------
int 			viqe_get_frame_end( void );

// -----------------------------------------------------------------
// return whether current frame processing is done or not
//
// 		input : 	-
// 		output :	return done(1) or not(0)
// -----------------------------------------------------------------
void 			viqe_set_frame_end( void );

// -----------------------------------------------------------------
// enable/disable compressor in temporal-spatial denoiser( dnts )
//
// 		input : 	enable(1) or disable(0)
// 		output :	-
// -----------------------------------------------------------------
void 			viqe_set_denoise_comp( int comp_en );

// ------------------------------------------------------------------------------
// viqe blocks enable settings
// register writing of viqe_modes_t structure
//
// 		input : 	viqe_modes_t structure
// 		output :	return success or fail( FAIL CODES )
// ------------------------------------------------------------------------------
int 			viqe_set_modes( viqe_modes_t *modes );

// ------------------------------------------------------------------------------
// return current modes values from reading enable registers
//
// 		input : 	pointer of viqe_modes_t structure to be written
// 		output :	-
// ------------------------------------------------------------------------------
int 			viqe_get_modes( viqe_modes_t *modes );

// ------------------------------------------------------------------------------
// set restart command
// 	valid only when viqe is running at frame manually control mode
//
// 		input : 	-
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_frm_restart( void );

// ------------------------------------------------------------------------------
// interrupt setting
//
// 		input : 	-
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_interrupts( void );

void 			viqe_clear_interrupts( void );

int 			viqe_get_rdma_done (void);

void 			viqe_set_output (viqe_cfg_t	*cfg, int OutputFormat, viqe_dma_t *dma);

void 			viqe_finalize (void);

// -----------------------------------------------------------------
// bypassing de-interlacer block
//
// 		input : 	bypass on/off
// 		output :	-
// -----------------------------------------------------------------
void 			viqe_set_deintl_bypass( int bypass );

// ------------------------------------------------------------------------------
// default setting for de-interlacer
//
// 		input : 	odd_first parameter
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_deintl_default_conf( int odd_first );

// ------------------------------------------------------------------------------
// disable region setting for de-interlacer
// 	In specified image region, the output of de-interlacer is bypassed as original pixel value
//
// 		input : 	region_enable, left-top position and right-bottom position
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_deintl_region( int region_enable,
						int region_idx_x_start, int region_idx_x_end,
						int region_idx_y_start, int region_idx_y_end );

// ------------------------------------------------------------------------------
// simple configuration for de-interlacer
// 	pulldown( film-mode ) detector on/off, odd_first parameter
//
// 		input : 	pulldown on/off, odd_first parameter
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_deintl_basic_conf( int pd_on, int odd_first );

// ------------------------------------------------------------------------------
// alternative software implementation for film mode detector of de-interlacer
// 	useful when another statical anaylsis is needed.
// 	this function is an example.
// 	it uses the register values of film mode detector internal variables
//	viqe_set_pd_user_checkout( int numfrm, int width, int height )
//
// 		input : 	frame number, width, height
// 		output :	check_out value
// ------------------------------------------------------------------------------
int 			viqe_set_pd_user_checkout( int numfrm, int width, int height );

// ------------------------------------------------------------------------------
// disable region setting for spatial-temporal de-noiser
// 	In specified image region, the output of de-noiser is bypassed as original pixel value
//
// 		input : 	region_enable - on/off
//				region_swap - 0 : left region is original and right region is processed, 1 : the opposite
//				region_hor_center : horizontal region boundary
// 		output :	-
// ------------------------------------------------------------------------------
void			viqe_set_dnts_region( int region_enable, int region_swap, int region_y_center );

// ------------------------------------------------------------------------------
// disable region setting for spatial de-noiser
// 	In specified image region, the output of de-noiser is bypassed as original pixel value
//
// 		input : 	region_enable - on/off
//				region_swap - 0 : left region is original and right region is processed, 1 : the opposite
//				region_hor_center : horizontal region boundary
// 		output :	-
// ------------------------------------------------------------------------------
void			viqe_set_dnsp_region( int region_enable, int region_swap, int region_y_center );

// ------------------------------------------------------------------------------
// default setting for histgram hardware
//
// 		input : 	width, height, CDF on/off, LUT use on/off
// 		output :	success of fail(-1)
// ------------------------------------------------------------------------------
int 			viqe_set_histogram_basic_conf( int width, int height, int cdf_en, int lut_use );

// ------------------------------------------------------------------------------
// enable function for histogram
//
// 		input : 	block enable, CDF generation enable, LUT use on/off
// 		output :	success or fail(-1)
// ------------------------------------------------------------------------------
int 			viqe_set_histogram_enable( int enable, int cdf_en, int lut_use );

// ------------------------------------------------------------------------------
// detail configuration function for histogram
//
// 		input : 	width, height, CDF on/off, LUT use on/off, pixel counter offset,
// 					automode on/off, number of multi frames, segments, scales
// 		output :	success or fail(-1)
// ------------------------------------------------------------------------------
int 			viqe_set_histogram_detail_conf( int width, int height, int cdf_en, int lut_use,
	   					   		int hoffset, int voffset, int automode, int multi_frames,
						   		unsigned char segs[16], unsigned char scales[16] );

// ------------------------------------------------------------------------------
// segments configuration function for histogram
//
// 		input : 	segments values having 16 unsigned char array
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_histogram_segments( unsigned char segs[16] );

// ------------------------------------------------------------------------------
// return segments values in hardware register
//
// 		input : 	pointer of 16 elements for segments writing
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_get_histogram_segments( unsigned char segs[16] );

// ------------------------------------------------------------------------------
// scale configuration function for histogram
// if global scale value is not zero, scales array values are ignored.
//
// 		input : 	global scale value, scale values having 16 unsigned char array
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_histogram_scales( int gscale, unsigned char scales[16] );

// ------------------------------------------------------------------------------
// return scales values in hardware register
//
// 		input : 	pointer of 16 elements for scales writing
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_get_histogram_scales( unsigned char scales[16] );

// ------------------------------------------------------------------------------
// LUT configuration function for histogram
//
// 		input : 	LUT values having 256 unsigned char array
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_histogram_luts( unsigned char luts[256] );

// ------------------------------------------------------------------------------
// return LUT values stored in hardware
//
// 		input : 	pointer of 256 elements for LUT writing
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_get_histogram_luts( unsigned char luts[256] );

// ------------------------------------------------------------------------------
// region setting for histogram
//
// 		input : 	region_enable on/off, top-left point and bottom-right point
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_his_region( int region_enable, int region_idx_x_start, int region_idx_x_end, int region_idx_y_start, int region_idx_y_end );

// ------------------------------------------------------------------------------
// gamut map hardware enable function
//
// 		input : 	enable on/off
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_gamut_enable( int enable );

// ------------------------------------------------------------------------------
// gamut mapping hardware initialization function
//
// 		input : 	-
// 		output :	success of fail(-1)
// ------------------------------------------------------------------------------
int 			viqe_set_gamut_map_init( void );


// ------------------------------------------------------------------------------
// disable region setting for spatial-temporal de-noiser
// 	In specified image region, the output of de-noiser is bypassed as original pixel value
//
// 		input : 	region_enable - on/off
//				region_swap - 0 : left region is original and right region is processed, 1 : the opposite
//				region_hor_center : horizontal region boundary
// 		output :	-
// ------------------------------------------------------------------------------
void			viqe_set_dnts_region( int region_enable, int region_swap, int region_y_center );

// ------------------------------------------------------------------------------
// disable region setting for spatial de-noiser
// 	In specified image region, the output of de-noiser is bypassed as original pixel value
//
// 		input : 	region_enable - on/off
//				region_swap - 0 : left region is original and right region is processed, 1 : the opposite
//				region_hor_center : horizontal region boundary
// 		output :	-
// ------------------------------------------------------------------------------
void			viqe_set_dnsp_region( int region_enable, int region_swap, int region_y_center );

// ------------------------------------------------------------------------------
// region setting for gamut-map
//
// 		input : 	region_enable on/off, top-left point and bottom-right point
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_set_gamut_region( int region_enable, int region_idx_x_start, int region_idx_x_end, int region_idx_y_start, int region_idx_y_end );

// ------------------------------------------------------------------------------
// bypass setting for gamut-map
//
// 		input : 	bypassing gamut-map hardware when bypass valuable is 1
// 		output :	-
// ------------------------------------------------------------------------------
void 			viqe_bypass_gamut( int bypass );



void 			cal_d2d_lpf_coeff_simple( int strength );	
void 			cal_d2d_hpf_coeff_simple( int strengthY, int strengthC );
void 			cal_dnts_coeff_simple( int strengthY, int strengthC );	
void 			cal_dnts_coeff_tmp_simple( int strengthY, int strengthC );	


void 			viqe_OnTheFly_OnOff(int OnOff, int option);

void 			TurnOnOffVIQEClk (int OnOff);
void 			TurnOnOffScaler0Clk (int OnOff);
void 			TurnOnOffScaler1Clk (int OnOff);


void			ODMARST (void);
void 			ODMAFlush (void);

void			ShowVIQE(void);

#endif		// _VIQE_LIB_H_

