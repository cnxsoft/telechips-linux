
/****************************************************************************
 *   FileName    : viqe_lib.c
 *   Description : 
 ****************************************************************************
 *
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
/*****************************************************************************
*
* Header Files Include
*
******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <mach/bsp.h>
#include <mach/tca_ckc.h>

#if defined(CONFIG_ARCH_TCC93XX)
#define HwVIPET			( (volatile PVIPET)(tcc_p2v(HwVIQE_BASE)) )
#define	HwDDICONFIG		( (volatile PDDICONFIG)(tcc_p2v(HwDDICFG_BASE)) )

#include "dev_viqe_93xx.h"

#elif defined(CONFIG_ARCH_TCC88XX)
#define HwVIPET         ( (volatile PVIPET)(tcc_p2v(HwVIQE_BASE)) )
#define HwDDICONFIG     ( (volatile PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE)) )

#include "dev_viqe_93xx.h"

#elif defined(CONFIG_ARCH_TCC92XX)
#define HwVIPET         ( (volatile PVIPET)(tcc_p2v(HwVIQE_BASE)) )
#define HwDDICONFIG     ( (volatile PDDICONFIG)(tcc_p2v(HwDDI_CONFIG_BASE)) )

#include "dev_viqe_92xx.h"
#endif

#include <mach/io.h>

#include "viqe_lib.h"

/*****************************************************************************
*
* Defines
*
******************************************************************************/
#define	VERYLOW			0
#define	LOW				1
#define	MEDIUM			2
#define	STRONG			3
#define	VERYSTRONG		4

#if 0
#define dprintk(msg...)	 { printk( "Tcc92_viqe_lib: " msg); }
#else
#define dprintk(msg...)	 
#endif

#define VIQE_SC_LCD_ONTHEFLY

/*****************************************************************************
*
* structures
*
******************************************************************************/
typedef	struct	{
	int		val;
	int		posx;
	int		posy;
}	val_idx_t;

typedef struct {
	unsigned int	coeff0;		// slope,ca,ba,a
	unsigned int	coeff1;		// Y_plane, NOT USED [31:8]
}	nD2D_LPF_COEFF;

typedef struct {
	unsigned int	coeff0;		// c, e, f , NOT USED [7:0]
	unsigned int	coeff1;		// g, m1, m2, l
	unsigned int 	coeff2;		// y_plane_h1, y_plane_h2
}	nD2D_HPF_COEFF;

typedef struct {
	unsigned int	strength;
	unsigned int	hor0;
	unsigned int	hor1;
	unsigned int	ver0;
	unsigned int	ver1;
}	nDNTS_COEFF_CH_LIST;

typedef struct {
	unsigned int	strength;
	unsigned int	tmp0;
	unsigned int	tmp1;
}	nDNTS_COEFF_TMP_CH_LIST;

typedef struct {
	unsigned int	strength;
	unsigned int	coeff0;
	unsigned int	coeff1;
	unsigned int	coeff2;
}	nD2D_HPF_CH_COEFF_LIST;

typedef struct {
	unsigned int 	strength;
	unsigned int	luma_coeff0;
	unsigned int	luma_coeff1;
	unsigned int	chroma_coeff0;
	unsigned int	chroma_coeff1;
}	nD2D_LPF_COEFF_LIST;

/*****************************************************************************
*
* Variables
*
******************************************************************************/
static int 				gDeintlOnOff = 0;
static int 				gOddFirstOnOff = 0;

static val_idx_t		cnt_val[3][5];
static int				frm_val_current[3];
static int				frm_val_predict[3];
static int				flag[5];


static nDNTS_COEFF_CH_LIST 	dnts_coeff_luma_list[] = {
	{VERYLOW,	0x154, 0x0, 0x154, 0x0},
	{LOW,		0x5554, 0x0, 0x5554, 0x0},
	{MEDIUM,	0x15556a94, 0x0, 0x15556a94, 0x0},
	{STRONG,	0x55aaaa94, 0x155, 0x55aaaa94, 0x155},
	{VERYSTRONG, 0xaabffea4, 0x55555a, 0xaaaaaaa4, 0x55555a}
	};

static nDNTS_COEFF_CH_LIST 	dnts_coeff_chroma_list[] = {
	{VERYLOW, 	0x14, 0x0, 0x14, 0x0},
	{LOW,		0x154, 0x0, 0x154, 0x0},
	{MEDIUM,	0x555554, 0x0, 0x555554, 0x0},
	{STRONG,	0x15556a94, 0x0, 0x15556a94, 0x0},
	{VERYSTRONG, 0x55aaaa94, 0x155, 0x55aaaa94, 0x155}
	};

static nDNTS_COEFF_TMP_CH_LIST	dnts_coeff_tmp_luma_list[] = {
	{VERYLOW,	0x555554, 0x0},
	{LOW,		0x15556a94, 0x0},
	{MEDIUM,	0x555aaa94, 0x5},
	{STRONG,	0x55aaaa94, 0x155},
	{VERYSTRONG, 0x6aaaaa94, 0x15555}
};

static nDNTS_COEFF_TMP_CH_LIST	dnts_coeff_tmp_chroma_list[] = {
	{VERYLOW,	0x154, 0x0},
	{LOW,		0x5554, 0x0},
	{MEDIUM,	0x555554, 0x0},
	{STRONG,	0x15556a94, 0x0},
	{VERYSTRONG, 0x555aaa94, 0x5}
};

static nD2D_HPF_CH_COEFF_LIST	d2d_hpf_luma_coeff_list[] = {
	{ VERYLOW, 	0x3a3c0200, 0x9b2040ff, 0x001c01fe},
	{ LOW, 		0x573c0200, 0xff260ac, 0x002d01fd},
	{ MEDIUM, 	0x742c0200, 0xff408081, 0x003801fc},
	{ STRONG, 	0x7f3c0200, 0xff468c75, 0x003f01fc},
	{ VERYSTRONG,0x913c0200, 0xff4ca068, 0x004901fb}
	};

static nD2D_HPF_CH_COEFF_LIST d2d_hpf_chroma_coeff_list[] = {
	{ VERYLOW, 	0x2c2d0100, 0x952040ff, 0x001501ff},
	{ LOW, 		0x422d0100, 0xff2c60ab, 0x002201ff},
	{ MEDIUM, 	0x582d0100, 0xff408080, 0x002b01fe},
	{ STRONG, 	0x602d0100, 0xff468c74, 0x002f01fe},	
	{ VERYSTRONG,0x6e2d0100, 0xff4ca067, 0x003701fe}
	};

static nD2D_LPF_COEFF_LIST		d2d_lpf_coeff_list[] = {
	{ VERYLOW, 	0x12320f0a, 0xe, 0x11190705, 0x6},
	{ LOW, 		0x12641e14, 0x1c, 0x12320f0a, 0xe},
	{ MEDIUM, 	0x12c83c28, 0x38, 0x12641e14, 0x1c},
	{ STRONG, 	0x12ff5a3c, 0x54, 0x12962d1e, 0x2a},
	{ VERYSTRONG,0x12ff7850, 0x70, 0x12c83c28, 0x38}
	};

typedef enum
{
	VIQE_DNCOEFF_Y0 = 0,
	VIQE_DNCOEFF_C0
} VIQEDNCOEFF;

viqe_state_t gVIQE_State = VIQE_STATE_IDLE;

/*****************************************************************************
*
* Functions
*
******************************************************************************/
void viqe_set_state(int *arg)
{
	int state;

	memcpy(&state, arg, sizeof(int));

	gVIQE_State = state;
}

viqe_state_t viqe_get_state(void)
{
	return gVIQE_State;
}

// -----------------------------------------------------------------
// viqe clk_onoff
//
// 		input : 	-
// 		output :	-
// -----------------------------------------------------------------
void viqe_clk_onoff( int onOff )
{
	if(onOff)
	{
		BITSET(HwDDICONFIG->SWRESET, HwDDIC_SWRESET_VIQE);
		BITCLR(HwDDICONFIG->PWDN,	HwDDIC_PWDN_VIQE);
		BITCLR(HwDDICONFIG->SWRESET, HwDDIC_SWRESET_VIQE);
	}
	else
	{
		BITSET(HwDDICONFIG->SWRESET, HwDDIC_SWRESET_VIQE);
		BITSET(HwDDICONFIG->PWDN,	HwDDIC_PWDN_VIQE);
		BITCLR(HwDDICONFIG->SWRESET, HwDDIC_SWRESET_VIQE);
	}

	if(onOff)
	{
		viqe_set_disable_all();
		viqe_clear_internal();
	}
}

// -----------------------------------------------------------------
// viqe VIQE_SWReset
//
// 		input : 	-
// 		output :	-
// -----------------------------------------------------------------
void viqe_swreset( void )	
{
	BITCSET(HwDDICONFIG->SWRESET,	HwDDIC_SWRESET_VIQE, HwDDIC_SWRESET_VIQE);
	BITCLR(HwDDICONFIG->SWRESET,	HwDDIC_SWRESET_VIQE);	
}

// -----------------------------------------------------------------
// clearing internal states and internal registers in all blocks
//
// 		input : 	-
// 		output :	return success or fail(-1)
// -----------------------------------------------------------------
int viqe_clear_internal( void )	
{
	int		ret = 0;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	
	// avoid clkgating...
	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE |= 0x3F; // 20090429

	viqe_swreset();

	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL |= 0xE0000000;	// 20090429

	// di
	HwVIPET->uDI_CTRL.nDI_CTRL |= 0xC0000000;	

	// dn
	HwVIPET->uDN_CTRL.nDN_CTRL |= 0xC0000000;	
	HwVIPET->uDN_CTRL.nDN_CTRL &= ~0xC0000000;	
	
	// rdma2
	HwVIPET->uRD_CTRL.nRD_CTRL |= 0x00010000;	
	HwVIPET->uRD_CTRL.nRD_CTRL &= ~0x00010000;	
	
	// cdma
	HwVIPET->uCD_CTRL.nCD_CTRL |= 0x00008000;		
	HwVIPET->uCD_CTRL.nCD_CTRL &= ~0x00008000;			

	// hi
	HwVIPET->uHI_CTRL.nHI_CTRL |= 0x80000000;		
	HwVIPET->uHI_CTRL.nHI_CTRL &= ~0x80000000;
	
	// outdma
	HwVIPET->uOD_CONTROL.nOD_CONTROL |= ( ((unsigned int)(1)<<31)|(1<<8) );
	HwVIPET->uOD_CONTROL.nOD_CONTROL &= 0x7ffffeff;

	ret = 	( HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL & 0xE0000000 ) |
			( HwVIPET->uDI_CTRL.nDI_CTRL & 0xC0000000 )	|
			( HwVIPET->uDN_CTRL.nDN_CTRL & 0xC0000000 ) |
			( HwVIPET->uRD_CTRL.nRD_CTRL & 0x00010000) |
			( HwVIPET->uCD_CTRL.nCD_CTRL & 0x00008000) |
			( HwVIPET->uHI_CTRL.nHI_CTRL & 0x80000000 );

	// reset clkgating...
	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE = 0;

	return ( ret == 0 )? 1 : -1;
}

// -----------------------------------------------------------------
// register writing of viqe_dma_t structure
//
// 		input : 	pointer of viqe_dma_t structure
// 		output :	base0 luminance address
// -----------------------------------------------------------------
unsigned int viqe_set_dma( viqe_dma_t *dma )
{
	uint		cdma_comp0[3];
	uint		cdma_comp1[3];
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	// control
	HwVIPET->uRD_CTRL.nRD_CTRL = (dma->opt 		? HwVIPET->uRD_CTRL.nRD_CTRL|Hw4 : HwVIPET->uRD_CTRL.nRD_CTRL&(~Hw4));
	HwVIPET->uRD_CTRL.nRD_CTRL = (dma->conti 	? HwVIPET->uRD_CTRL.nRD_CTRL|Hw8 : HwVIPET->uRD_CTRL.nRD_CTRL&(~Hw8));
	HwVIPET->uCD_CTRL.nCD_CTRL = (dma->opt		? HwVIPET->uCD_CTRL.nCD_CTRL|Hw4 : HwVIPET->uCD_CTRL.nCD_CTRL&(~Hw4));	

	// cur
	HwVIPET->uRD_IMG0_BASE0.nRD_BASEADDR = dma->addr_cur[0];
	HwVIPET->uRD_IMG0_BASE1.nRD_BASEADDR = dma->addr_cur[1];
	HwVIPET->uRD_IMG0_BASE2.nRD_BASEADDR = dma->addr_cur[2];

	// pre
	HwVIPET->uRD_IMG1_BASE0.nRD_BASEADDR = dma->addr_pre[0];
	HwVIPET->uRD_IMG1_BASE1.nRD_BASEADDR = dma->addr_pre[1];
	HwVIPET->uRD_IMG1_BASE2.nRD_BASEADDR = dma->addr_pre[2];

	// width offset for cur, pre
	HwVIPET->uRD_IMG0_OFS.nRD_BASEOFS = ((dma->i_offs[1]<<16) | (dma->i_offs[0]));
	HwVIPET->uRD_IMG1_OFS.nRD_BASEOFS = ((dma->i_offs[1]<<16) | (dma->i_offs[0]));

	// de-compressor dma
	HwVIPET->uRD_CTRL.nRD_CTRL = ((!dma->comp_pingpong) ? HwVIPET->uRD_CTRL.nRD_CTRL|Hw6 : HwVIPET->uRD_CTRL.nRD_CTRL&(~Hw6)); // 20090429

	HwVIPET->uRD_IMG2_BASE0_0.nRD_BASEADDR = dma->addr_comp0[0];
	HwVIPET->uRD_IMG2_BASE1_0.nRD_BASEADDR = dma->addr_comp0[1];
	HwVIPET->uRD_IMG2_BASE2_0.nRD_BASEADDR = dma->addr_comp0[2];

	//if ( !dma->addr_comp1[0] | !dma->addr_comp1[1] | !dma->addr_comp1[2] )	{	
	HwVIPET->uRD_IMG2_BASE0_1.nRD_BASEADDR = dma->addr_comp1[0];
	HwVIPET->uRD_IMG2_BASE1_1.nRD_BASEADDR = dma->addr_comp1[1];
	HwVIPET->uRD_IMG2_BASE2_1.nRD_BASEADDR = dma->addr_comp1[2];

	// compressor dma	
	HwVIPET->uCD_CTRL.nCD_CTRL = ((!(dma->comp_pingpong)) ? HwVIPET->uCD_CTRL.nCD_CTRL|Hw5 : HwVIPET->uCD_CTRL.nCD_CTRL&(~Hw5));

	if ( dma->comp_pingpong )	{
		cdma_comp0[0] = dma->addr_comp1[0];
		cdma_comp0[1] = dma->addr_comp1[1];
		cdma_comp0[2] = dma->addr_comp1[2];
		cdma_comp1[0] = dma->addr_comp0[0];
		cdma_comp1[1] = dma->addr_comp0[1];
		cdma_comp1[2] = dma->addr_comp0[2];		
	}
	else	{
		cdma_comp0[0] = dma->addr_comp0[0];
		cdma_comp0[1] = dma->addr_comp0[1];
		cdma_comp0[2] = dma->addr_comp0[2];
		cdma_comp1[0] = dma->addr_comp1[0];
		cdma_comp1[1] = dma->addr_comp1[1];
		cdma_comp1[2] = dma->addr_comp1[2];		
	}
	
	HwVIPET->uCD_BASE0_0.nCD_BASEADDR = cdma_comp0[0];
	HwVIPET->uCD_BASE1_0.nCD_BASEADDR = cdma_comp0[1];
	HwVIPET->uCD_BASE2_0.nCD_BASEADDR = cdma_comp0[2];
	
	HwVIPET->uCD_BASE0_1.nCD_BASEADDR = cdma_comp1[0];
	HwVIPET->uCD_BASE1_1.nCD_BASEADDR = cdma_comp1[1];
	HwVIPET->uCD_BASE2_1.nCD_BASEADDR = cdma_comp1[2];

	// outdma
	HwVIPET->uOD_BASE0.nOD_BASEADDR = dma->addr_out[0];
	HwVIPET->uOD_BASE1.nOD_BASEADDR = dma->addr_out[1];
	HwVIPET->uOD_BASE2.nOD_BASEADDR = dma->addr_out[2];

	HwVIPET->uOD_OFS.nOD_BASEOFS = (dma->o_offs[1]<<16 | dma->o_offs[0]);

	HwVIPET->uOD_CFG.nOD_CFG |= Hw6;
	
	return ( HwVIPET->uRD_IMG0_BASE0.nRD_BASEADDR);
}

// -----------------------------------------------------------------
// register writing of varying dma addresses per frame processing
//
// 		input : 	pointer of viqe_dma_t structure
// 		output :	base0 luminance address
// -----------------------------------------------------------------
unsigned int viqe_set_dma_addr( viqe_dma_t *dma )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	// cur
	HwVIPET->uRD_IMG0_BASE0.nRD_BASEADDR = dma->addr_cur[0];
	HwVIPET->uRD_IMG0_BASE1.nRD_BASEADDR = dma->addr_cur[1];
	HwVIPET->uRD_IMG0_BASE2.nRD_BASEADDR = dma->addr_cur[2];

	// pre
	HwVIPET->uRD_IMG1_BASE0.nRD_BASEADDR = dma->addr_pre[0];
	HwVIPET->uRD_IMG1_BASE1.nRD_BASEADDR = dma->addr_pre[1];
	HwVIPET->uRD_IMG1_BASE2.nRD_BASEADDR = dma->addr_pre[2];	

	// outdma
	HwVIPET->uOD_BASE0.nOD_BASEADDR = dma->addr_out[0];
	HwVIPET->uOD_BASE1.nOD_BASEADDR = dma->addr_out[1];
	HwVIPET->uOD_BASE2.nOD_BASEADDR = dma->addr_out[2];
	
	return ( HwVIPET->uRD_IMG0_BASE0.nRD_BASEADDR);	
}

// -----------------------------------------------------------------
// register writing of viqe_imgfmt_t structure
//
// 		input : 	pointer of viqe_imgfmt_t structure
// 		output :	-
// -----------------------------------------------------------------
int viqe_set_imgfmt( viqe_imgfmt_t *imgfmt )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT |= imgfmt->i_fmt;
	HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT = (imgfmt->force_420 ? HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT|Hw5 : HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT&(~Hw5));
	HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT = (imgfmt->first_frm_init ? HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT|Hw6 : HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT&(~Hw6));

	HwVIPET->uTOP_CTRL.nTOP_CTRL = (imgfmt->fmt_conv_disable ? HwVIPET->uTOP_CTRL.nTOP_CTRL|Hw20 : HwVIPET->uTOP_CTRL.nTOP_CTRL&(~Hw20));

	//HwVIPET->uOD_CFG.nOD_CFG |= imgfmt->o_fmt;
	BITCSET(HwVIPET->uOD_CFG.nOD_CFG, 0x0000000F, imgfmt->o_fmt);

//	HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT |= (1<<5); // twice read mode //srlee
//	HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT &= ~(1<<5); // twice read mode // srlee
	return 1;
}

// -----------------------------------------------------------------
// register writing of viqe_cfg_t structure
//
// 		input : 	pointer of viqe_cfg_t structure
// 		output :	-
// -----------------------------------------------------------------
int viqe_set_cfg( viqe_cfg_t *cfg )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	// overlap blocks
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL = (cfg->overlap_disable ? HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL|Hw23 : HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL&(~Hw23));

	// frame manual restart
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL |= cfg->frm_manual_restart;
	
	// directpath enable
	viqe_OnTheFly_OnOff(cfg->directpath_en, cfg->directpath_op);
	
	// de-noiser compressor enable
	HwVIPET->uRD_CTRL.nRD_CTRL = ( cfg->comp_en ? HwVIPET->uRD_CTRL.nRD_CTRL&(~Hw7) : HwVIPET->uRD_CTRL.nRD_CTRL|Hw7);
	HwVIPET->uCD_CTRL.nCD_CTRL = ( cfg->comp_en ? HwVIPET->uCD_CTRL.nCD_CTRL &(~Hw6) : HwVIPET->uCD_CTRL.nCD_CTRL |Hw6);

	return 1;
}

// -----------------------------------------------------------------
// disable all blocks in viqe
//
// 		input : 	-
// 		output :	return success or fail(-1)
// -----------------------------------------------------------------
int viqe_set_disable_all( void )	
{
	int		ret = 0;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	// frmupdate_ctrl_disable
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL |= (1<<24);

	HwVIPET->uTOP_CTRL.nTOP_CTRL &= ~(Hw6 | Hw5 | Hw4 | Hw10 | Hw12 |
									Hw2 | Hw3 | Hw7 |
									Hw18 | Hw17 |
									Hw0 | Hw1 | Hw16);

 	ret = HwVIPET->uTOP_CTRL.nTOP_CTRL &
									 (Hw6 | Hw5 | Hw4 | Hw10 | Hw12 |
									Hw2 | Hw3 | Hw7|
									Hw18 | Hw17|
									Hw0 | Hw1 | Hw16	);

	// frmupdate_ctrl_disable
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL  &= ~(1<<24);
	
	return ( ret == 0 )? 1 : -1;
}

// -----------------------------------------------------------------
// viqe initialization
// this function is an example, and there can be other initialization sequences
//
// 		input : 	viqe_t structure
// 		output :	return success or fail(-1)
// -----------------------------------------------------------------
int viqe_init( viqe_t *viqe )	
{
	viqe_state_t viqe_state = VIQE_STATE_IDLE;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	
	// package limitation
	HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT &= 0x0000FFFF;
	HwVIPET->uTOP_IMGFMT.nTOP_IMGFMT |= ((unsigned int)(0xbacb))<<16;

	// frm update : always on // in TCC9300 this is cleared and used in 60Hz mode
#ifdef VIQE_SC_LCD_ONTHEFLY
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL &= ~(1<<27);
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL &= ~(1<<24);
#else
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL |= (1<<27);
	HwVIPET->uTOP_MISC_CTRL.nTOP_MISC_CTRL |= (1<<24);
#endif

	// width, height	
	HwVIPET->uTOP_SIZE.nTOP_SIZE = ((viqe->height)<<16 | viqe->width);
	HwVIPET->uOD_SIZE.nOD_SIZE = ((viqe->height)<<16 | viqe->width);

	// de-compressor size
	if ( viqe->imgfmt->force_420 == ON )	{
		HwVIPET->uRD_COMP_PL0.nRD_COMP_PL = (viqe->width * viqe->height);
		HwVIPET->uRD_COMP_PL1.nRD_COMP_PL = (viqe->width * viqe->height >> 2);
	}
	else	{
		HwVIPET->uRD_COMP_PL0.nRD_COMP_PL = (viqe->width * viqe->height);
		HwVIPET->uRD_COMP_PL1.nRD_COMP_PL = (viqe->width * viqe->height >> 2);
	}
	viqe_set_imgfmt( viqe->imgfmt );
	
	// cfg
	viqe_set_cfg( viqe->cfg );

	// dma	
	viqe_set_dma( viqe->dma );

	// -------------------------------------------
	// default setting of blocks
	// -------------------------------------------
	// deintl
	viqe_set_deintl_default_conf(0);
	
	// denoise
	//viqe_set_denoise_comp( OFF );		// comp_en
	//viqe_set_denoise_comp( ON );		// srlee

	// gamut
	viqe_set_gamut_map_init();

	// his
	//viqe_set_histogram_basic_conf( viqe->width, viqe->height, OFF, ON );
	viqe_set_histogram_basic_conf( viqe->width, viqe->height, OFF, OFF );

	//his, gamut TopCntl enable
	HwVIPET->uGM_CTRL.nGM_CTRL = 0x0;		// bypass off

	viqe_OnTheFly_OnOff(viqe->cfg->directpath_en, viqe->cfg->directpath_op);
	
	viqe_set_state(&viqe_state);
	return 1;
}

void viqe_set_resolution(viqe_t *viqe, int width, int height)
{
	viqe->width = width;
	viqe->height = height;
	// width, height	
	HwVIPET->uTOP_SIZE.nTOP_SIZE = ((viqe->height)<<16 | viqe->width);
	HwVIPET->uOD_SIZE.nOD_SIZE = ((viqe->height)<<16 | viqe->width);
}

// -----------------------------------------------------------------
// return whether current frame processing is done or not
//
// 		input : 	-
// 		output :	return done(1) or not(0)
// -----------------------------------------------------------------
int viqe_get_frame_end( void )	
{
	//printk ("0x%x 0x%x\n", &(HwVIPET->uOD_STATE), HwVIPET->uOD_STATE);
	return ((HwVIPET->uOD_STATE & Hw2) >> 2);
}

// -----------------------------------------------------------------
// reset frame ending signal
// 	it should be called before restart new frame if viqe_get_frame_end() is high at current frame
//
// 		input : 	-
// 		output :	-
// -----------------------------------------------------------------
void viqe_set_frame_end( void )	
{
	HwVIPET->uOD_STATE |= (Hw2);
}

// -----------------------------------------------------------------
// enable/disable compressor in temporal-spatial denoiser( dnts )
//
// 		input : 	enable(1) or disable(0)
// 		output :	-
// -----------------------------------------------------------------
void viqe_set_denoise_comp( int comp_en )	
{
	// compression enables, default is enabled
	HwVIPET->uRD_CTRL.nRD_CTRL = ( comp_en ? (HwVIPET->uRD_CTRL.nRD_CTRL&(~Hw7)) : (HwVIPET->uRD_CTRL.nRD_CTRL|Hw7));
	HwVIPET->uCD_CTRL.nCD_CTRL = ( comp_en ? (HwVIPET->uCD_CTRL.nCD_CTRL&(~Hw6)) : (HwVIPET->uCD_CTRL.nCD_CTRL|Hw6));	
}

// ------------------------------------------------------------------------------
// viqe blocks enable settings
// register writing of viqe_modes_t structure
//
// 		input : 	viqe_modes_t structure
// 		output :	return success or fail( FAIL CODES )
// ------------------------------------------------------------------------------
int viqe_set_modes( viqe_modes_t *modes )
{
	int		ret;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	dprintk("DI-%d, DISP-%d, DNTS-%d, LPF-%d, HPF-%d, HIST-%d, ODDFIRST-%d"							//, GAMUT-%d\n "
		, modes->di, modes->disp, modes->dnts, modes->dnsp, modes->hpf, modes->his, modes->oddfirst);		//, modes->gamut);
	// block enables

	if ( modes->hpf == 1 && modes->dnsp == 1 )	{
//		printf ("error >> turn on only one filter on 93xx(dnsp or hpf).\n");
		modes->hpf = 0;
		modes->dnsp = 0;
	}
	HwVIPET->uHI_CTRL.nHI_CTRL = (modes->his ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw7 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw7));
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( modes->gamut ? (HwVIPET->uTOP_CTRL.nTOP_CTRL|Hw17) : (HwVIPET->uTOP_CTRL.nTOP_CTRL&(~Hw17)));
	
	HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF= modes->oddfirst ? (HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF|Hw0) : (HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF&(~Hw0));
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( (modes->dnsp || modes->hpf) ? (HwVIPET->uTOP_CTRL.nTOP_CTRL|Hw6) : (HwVIPET->uTOP_CTRL.nTOP_CTRL&(~Hw6)));
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( modes->dnts ? (HwVIPET->uTOP_CTRL.nTOP_CTRL|Hw5) : (HwVIPET->uTOP_CTRL.nTOP_CTRL&(~Hw5)));
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( modes->di ? (HwVIPET->uTOP_CTRL.nTOP_CTRL|Hw4) : (HwVIPET->uTOP_CTRL.nTOP_CTRL&(~Hw4)));
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( modes->hpf ? (HwVIPET->uTOP_CTRL.nTOP_CTRL|Hw10) : (HwVIPET->uTOP_CTRL.nTOP_CTRL&(~Hw10)));
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( modes->disp ? (HwVIPET->uTOP_CTRL.nTOP_CTRL|Hw12) : (HwVIPET->uTOP_CTRL.nTOP_CTRL&(~Hw12)));

	// en_di and en_dnsp, en_hpf
	if ( modes->di && !modes->disp )	
	{
		if ( modes->dnsp | modes->hpf )
			return RET_VIQE_EN_DI_DNSP;
	}

	// dn, hpf
	if ( modes->dnts && modes->dnsp )	
	{
		HwVIPET->uTOP_CTRL.nTOP_CTRL &=~ (Hw8|Hw9);
		HwVIPET->uTOP_CTRL.nTOP_CTRL |= Hw9;		
	}
	else if ( modes->dnts && modes->hpf )
	{
		HwVIPET->uTOP_CTRL.nTOP_CTRL &=~ (Hw8|Hw9);
		HwVIPET->uTOP_CTRL.nTOP_CTRL |= Hw9;		
	}
	else if ( modes->dnts )
	{
		HwVIPET->uTOP_CTRL.nTOP_CTRL &=~ (Hw8|Hw9);
	}
	else if ( modes->dnsp || modes->hpf )
	{
		HwVIPET->uTOP_CTRL.nTOP_CTRL &=~ (Hw8|Hw9);
		HwVIPET->uTOP_CTRL.nTOP_CTRL |= Hw8;		
	}
	
	// deinterlacer first
	if ( modes->di && !modes->disp && modes->hpf )	// only hpf
		HwVIPET->uTOP_CTRL.nTOP_CTRL |= Hw13;		
	else
		HwVIPET->uTOP_CTRL.nTOP_CTRL &= ~Hw13;

	// need previous frame data
	if ( (modes->di && !modes->disp) || modes->dnts )
		HwVIPET->uTOP_CTRL.nTOP_CTRL |= Hw2;		
	else
		HwVIPET->uTOP_CTRL.nTOP_CTRL &= ~Hw2;		

	// compressor for de-noiser recursive path
	if ( modes->dnts )
	{
		HwVIPET->uTOP_CTRL.nTOP_CTRL |= Hw3;
		HwVIPET->uTOP_CTRL.nTOP_CTRL |= Hw7;	
	}
	else
	{
		HwVIPET->uTOP_CTRL.nTOP_CTRL &= ~Hw3;
		HwVIPET->uTOP_CTRL.nTOP_CTRL &= ~Hw7;	
	}

	// main enables
	ret = 1;
	HwVIPET->uTOP_CTRL.nTOP_CTRL = HwVIPET->uTOP_CTRL.nTOP_CTRL | Hw16;
	HwVIPET->uTOP_CTRL.nTOP_CTRL = HwVIPET->uTOP_CTRL.nTOP_CTRL | Hw1;
	HwVIPET->uTOP_CTRL.nTOP_CTRL = HwVIPET->uTOP_CTRL.nTOP_CTRL | Hw0;

	ret = ( (HwVIPET->uTOP_INT.nTOP_INT&(Hw12|Hw13|Hw14|Hw15)) == 0 ) ? 1 : RET_VIQE_EN_ERRREG;
	dprintk("%s return value : %d\n",  __FUNCTION__, ret);
	return ret;
}

// ------------------------------------------------------------------------------
// return current modes values from reading enable registers
//
// 		input : 	pointer of viqe_modes_t structure to be written
// 		output :	-
// ------------------------------------------------------------------------------
int viqe_get_modes( viqe_modes_t *modes )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	modes->dnsp = (HwVIPET->uTOP_CTRL.nTOP_CTRL&Hw6)>>6;
	modes->dnts = (HwVIPET->uTOP_CTRL.nTOP_CTRL&Hw5)>>5;
	modes->di = (HwVIPET->uTOP_CTRL.nTOP_CTRL&Hw4)>>4;
	modes->hpf = (HwVIPET->uTOP_CTRL.nTOP_CTRL&Hw10)>>10;
	modes->disp = (HwVIPET->uTOP_CTRL.nTOP_CTRL&Hw12)>>12;
	modes->his = (HwVIPET->uTOP_CTRL.nTOP_CTRL&Hw18)>>18;
	modes->gamut = ((HwVIPET->uTOP_CTRL.nTOP_CTRL&Hw17)>>17) ? 0 : 1;
	modes->oddfirst = HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF&Hw0;

	if(modes->di && modes->disp)
	{
		modes->di = 0;	
	}
	if(modes->dnsp && modes->hpf)
	{
		modes->dnsp = 0;	
	}

	return (modes->dnsp |modes->dnts | modes->di |modes->hpf |modes->disp |modes->his |modes->gamut | modes->oddfirst);
}

// ------------------------------------------------------------------------------
// set restart command
// 	valid only when viqe is running at frame manually control mode
//
// 		input : 	-
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_frm_restart( void )	
{
	HwVIPET->uTOP_FRM_CTRL.nTOP_FRM_CTRL |= Hw0;	
}

// ------------------------------------------------------------------------------
// interrupt setting
//
// 		input : 	-
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_interrupts( void )	
{
	HwVIPET->uTOP_INTMASK.nTOP_INTMASK |= 0x000000ff;

	// di
	HwVIPET->uDI_INT.nDI_INT |= 0xffff0000;	
}

void viqe_clear_interrupts( void )	
{
	int		interrupts;
	interrupts = HwVIPET->uTOP_INT.nTOP_INT;


	if ( HwVIPET->uTOP_INT.nTOP_INT & Hw2)	
	{
		int		di_int;
		di_int = HwVIPET->uDI_INT.nDI_INT;
		HwVIPET->uDI_INT.nDI_INT = di_int;
	}

	if ( HwVIPET->uTOP_INT.nTOP_INT & Hw5)	
	{
		int		pm_int;
		pm_int = HwVIPET->uHI_INT.nHI_INT;
		
		if ( pm_int == 0 )	{
			pm_int = HwVIPET->uGM_INT.nGM_INT;
			HwVIPET->uGM_INT.nGM_INT = pm_int;
		}
		else	{
			pm_int = HwVIPET->uHI_INT.nHI_INT;
			HwVIPET->uHI_INT.nHI_INT = pm_int;
		}
	}

	HwVIPET->uTOP_INT.nTOP_INT = interrupts;
}


int viqe_get_rdma_done (void)
{
	return ((HwVIPET->uRD_LINE_STATE1.nRD_LINE_STATE1 & Hw31) >> 31);
}


void viqe_set_output (viqe_cfg_t	*cfg, int OutputFormat, viqe_dma_t *dma)
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	// directpath enable
	BITCSET(HwVIPET->uOD_CFG.nOD_CFG, 0x0000000F, OutputFormat);
	HwVIPET->uOD_OFS.nOD_BASEOFS = (dma->o_offs[1]<<16 | dma->o_offs[0]);

	viqe_OnTheFly_OnOff(cfg->directpath_en, cfg->directpath_op);
	return;
}

void viqe_finalize (void)
{
	volatile int dmadone;
	volatile int rdmadone;
	volatile unsigned int i = 0;

//	dprintk(KERN_INFO "%s\n", __FUNCTION__);
//	printk ("Wait!!\n");
	i = 0;
	do	{
		i++;
		
		dmadone = viqe_get_frame_end();
		rdmadone = viqe_get_rdma_done ();

	#if 1
		if(viqe_get_state() != VIQE_STATE_RUN)
		{
			printk("dmadone: %d rdmadone: %d\n", dmadone, rdmadone);
			break;
		}
	#endif
	} while(!dmadone || !rdmadone);

	viqe_set_frame_end();
}


// -----------------------------------------------------------------
// bypassing de-interlacer block
//
// 		input : 	bypass on/off
// 		output :	-
// -----------------------------------------------------------------
void viqe_set_deintl_bypass( int bypass )	
{
	HwVIPET->uDI_CTRL.nDI_CTRL = bypass ? (HwVIPET->uDI_CTRL.nDI_CTRL|Hw29) : (HwVIPET->uDI_CTRL.nDI_CTRL&(~Hw29));
}

// ------------------------------------------------------------------------------
// default setting for de-interlacer
//
// 		input : 	odd_first parameter
// 		output :	-
// ------------------------------------------------------------------------------

void viqe_set_deintl_default_conf( int odd_first)
{
	int maxrange_tmp_on = 1;
	int maxrange_spa_on = 1;
	int spatial_jaggycheck = 1;
	int spatio_temp_jaggycheck = 0;
	int temporal_jaggycheck = 0;
	int yd_mode = 0;
	int dir_length_type = 0;
	int pulldown_on = 0; // for 60Hz
//	int pulldown_on = 1; // for 30Hz
	int pd_judder_on = 1;
	int pd_use_prevent_flag_on = 1;
	int dm_thres_sad_y = 8;
	int dm_thres_pixel_y = 4;
	int dm_thres_sad_c = 4;
	int dm_thres_pixel_c = 2;
	int maxrange_spa_type = 3;
	int gen_thres_jaggy_minthres = 50;
	int gen_thres_jaggy_maxthres = 127;
	int edi_leftref_disable = 1;
	int edi_leftref_len = 2;
	int edi_leftref_type = 0;
	int edi_glmode_disable = 1;
	int gen_thres_jaggy_div_half = 1;
	int edi_dir_adap_disable = 0;
	int edi_thres_dir_adap = 8;
	int edi_thres_dir_early = 4;
	int edi_dir_early_disable = 0;
	int edi_vert_err_ratio = 2;
	int edi_opp_dir_disable = 0;
	int dm_stati_thres_disable = 0;
	int dm_stati_thres_mul = 32;
	int dm_stati_thres_weight = 16;
	int dm_mode_l = 0;
	int dm_mode_c = 1;
	int dm_hpf_frm_thres = 8;
	int dm_hpf_fld_thres = 5;
	int dm_hpf_var_d_thres = 2;
	int dm_dyn_var_d_thres = 6;
	int dm_var_stat_ratio_0 = 4;
	int dm_var_stat_ratio_1 = 2;
	int dm_var_lg_range = 1;
	int pd_thres_pulldown_cnts = 500;
	int pd_thres_weight = 1;
	int pd_thres_user_val_disable = 1;
	int pd_user_checkout = 0;
	int pd_cnts_thres_pulldown_checkout = 0;
	int pd_cnts_zero_prevent = 1;
	int pd_thres_pulldown_0 = 15;
	int pd_thres_pulldown_1 = 20;
	int pd_thres_pulldown_2 = 30;
	int pd_thres_cnt_judder = 2;
	int pd_judder_downline_margin = 8;
	int pd_judder_horline_margin = 8;
	int pd_gen_thres_judder_minthres = 64;
	int pd_gen_thres_judder_maxthres = 111;
	int pd_gen_thres_judder_div_half = 0;
	int pd_judder_thres_two_lbufs = 88;
	int pd_not_cnt_judder_stati = 1;
	int pd_mul_dm_thres = 4;
	int region_idx_x_start = 0;
	int region_idx_x_end = 0;
	int region_enable = 0;
	int region_idx_y_start = 0;
	int region_idx_y_end = 0;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	// CTRL
	HwVIPET->uDI_CTRL.nDI_CTRL = (
							 (1) << 12 |
							 (pd_use_prevent_flag_on<<11)|
							 (1<<10)|
							 (pd_judder_on<<9)|
							 (pulldown_on<<8) |
							 (yd_mode<<7) |
							 (1<<6) |
							 (maxrange_spa_on<<5) |
							 (maxrange_tmp_on<<4) |
							 (temporal_jaggycheck<<2) |
							 (spatio_temp_jaggycheck<<1) |
							 (spatial_jaggycheck<<0) );

	// ENGINE0
	HwVIPET->uDI_ENGINE0.nDI_ENGINE0 = (
							 (dm_thres_pixel_c<<24)|
							 (dm_thres_sad_c<<16)|
							 (dm_thres_pixel_y<<8)|
							 (dm_thres_sad_y<<0) );

	// ENGINE1
	HwVIPET->uDI_ENGINE1.nDI_ENGINE1 = (
							 (gen_thres_jaggy_maxthres<<24)|
							 (gen_thres_jaggy_minthres<<16)|
							 (edi_leftref_type<<14)|
							 (edi_leftref_len<<9)|
							 (edi_leftref_disable<<8)|
							 (dir_length_type<<4)|
							 (edi_glmode_disable<<3)|
							 (gen_thres_jaggy_div_half<<2)|
							 (maxrange_spa_type<<0) );

	// ENGINE2
	HwVIPET->uDI_ENGINE2.nDI_ENGINE2 = (
							 (edi_thres_dir_adap<<20)|
							 (edi_thres_dir_early<<8)|
							 (edi_vert_err_ratio<<3)|
							 (edi_opp_dir_disable<<2)|
							 (edi_dir_early_disable<<1)|
							 (edi_dir_adap_disable<<0) );

	// ENGINE3
	HwVIPET->uDI_ENGINE3.nDI_ENGINE3 = (
							 (dm_stati_thres_weight<<20) |
							 (dm_stati_thres_mul<<8) |
							 (dm_stati_thres_disable<<0) );

	// ENGINE4
	HwVIPET->uDI_ENGINE4.nDI_ENGINE4 = (
							 (dm_var_lg_range<<28) |
							 (dm_var_stat_ratio_1<<24) |
							 (dm_var_stat_ratio_0<<20) |
							 (dm_dyn_var_d_thres<<16) |
							 (dm_hpf_var_d_thres<<12) |
							 (dm_hpf_fld_thres<<8) |
							 (dm_hpf_frm_thres<<4) |
							 (dm_mode_c<<1) |
							 (dm_mode_l<<0) );

	// PD_THRES0
	HwVIPET->uDI_PD_THRES0.nDI_PD_THRES0 = (
							 (pd_cnts_zero_prevent<<24)|
							 (pd_cnts_thres_pulldown_checkout<<20)|
							 (pd_thres_user_val_disable<<15)|
							 (pd_thres_weight<<10) |
							 (pd_thres_pulldown_cnts<<0) );

	// PD_THRES1
	HwVIPET->uDI_PD_THRES1.nDI_PD_THRES1 = (
							 (pd_thres_pulldown_2<<16)|
							 (pd_thres_pulldown_1<<8)|
							 (pd_thres_pulldown_0<<0) );

	// PD_JUDDER
	HwVIPET->uDI_PD_JUDDER.nDI_PD_JUDDER = (
							 (pd_gen_thres_judder_maxthres<<24)|
							 (pd_gen_thres_judder_minthres<<16)|
							 (pd_judder_horline_margin<<12) |
							 (pd_judder_downline_margin<<8) |
							 (pd_thres_cnt_judder<<0) );

	// PD_JUDDER_M
	HwVIPET->uDI_PD_JUDDER_MISC.nDI_PD_JUDDER_MISC = (
							 (pd_mul_dm_thres<<17)|
							 (pd_not_cnt_judder_stati<<16)|
							 (pd_judder_thres_two_lbufs<<8)|
							 (pd_gen_thres_judder_div_half<<0) );

	// interrupt mask
	//apb.write( 10'h80 + 15*4, 			(16'hffff<<16) );

	// top register
	//apb.write( 10'h00 + 4*4, 			odd_first );
	HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF = odd_first;

	//init_deintl = 1;
}

// ------------------------------------------------------------------------------
// region setting for de-interlacer
//
// 		input : 	region_enable on/off, top-left point and bottom-right point
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_deintl_region( int region_enable, int region_idx_x_start, int region_idx_x_end, int region_idx_y_start, int region_idx_y_end )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	// region0;
	HwVIPET->uDI_REGION_0.nDI_REGION_0 = 0;
	HwVIPET->uDI_REGION_0.nDI_REGION_0 |= (region_idx_x_start);
	HwVIPET->uDI_REGION_0.nDI_REGION_0 |= (region_idx_x_end << 16);
	HwVIPET->uDI_REGION_0.nDI_REGION_0 |= (region_enable << 31);
	// region1;
	HwVIPET->uDI_REGION_1.nDI_REGION_1 = 0;
	HwVIPET->uDI_REGION_1.nDI_REGION_1 |= (region_idx_y_start);
	HwVIPET->uDI_REGION_1.nDI_REGION_1 |= (region_idx_y_end << 16);
}
	
// ------------------------------------------------------------------------------
// simple configuration for de-interlacer
// 	pulldown( film-mode ) detector on/off, odd_first parameter
//
// 		input : 	pulldown on/off, odd_first parameter
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_deintl_basic_conf( int pd_on, int odd_first ) 
{
	// pulldown on/off
	HwVIPET->uDI_CTRL.nDI_CTRL = ((pd_on & 0x1) == 1) ? (HwVIPET->uDI_CTRL.nDI_CTRL|Hw8) : (HwVIPET->uDI_CTRL.nDI_CTRL&(~Hw8));
	// odd first on/off
	HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF = ((odd_first & 0x1) == 1) ? (HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF|Hw0) : (HwVIPET->uTOP_IMGCONF.nTOP_IMGCONF&(~Hw0));
}


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
static int find_min( int cnt_val[5], int except_idx )	
{
	int		d_1_2_idx;
	int		d_3_4_idx;
	int		min_idx;

	if ( except_idx == 1 )
		d_1_2_idx = 2;
	else if ( except_idx == 2 )
		d_1_2_idx = 1;
	else	
		d_1_2_idx = ( cnt_val[1] > cnt_val[2] )? 2 : 1;


	if ( except_idx == 3 )
		d_3_4_idx = 4;
	else if ( except_idx == 4 )
		d_3_4_idx = 3;
	else	
		d_3_4_idx = ( cnt_val[3] > cnt_val[4] )? 4 : 3;
	

	if ( except_idx == 0 )
		min_idx = ( cnt_val[d_3_4_idx] > cnt_val[d_1_2_idx] )? d_1_2_idx : d_3_4_idx;
	else	
	{
		if ( cnt_val[d_3_4_idx] > cnt_val[d_1_2_idx] )
			min_idx = ( cnt_val[d_1_2_idx] > cnt_val[0] )? 0 : d_1_2_idx;
		else	
			min_idx = ( cnt_val[d_3_4_idx] > cnt_val[0] )? 0 : d_3_4_idx;
	}

	return min_idx;
}


int viqe_set_pd_user_checkout( int numfrm, int width, int height )	
{
	int				prevent_flag_on;
	int				num_fields;
	int				check_pulldown[2];
	int				minpos_offset;
	int				check_out = 0;
	int				n, i, j;
	int				cur_idx;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	
	num_fields = numfrm * 2;

	cnt_val[0][(num_fields)%5].val = HwVIPET->uDI_PD_CNT0_0.nDI_PD_CNT0_0;
	cnt_val[1][(num_fields)%5].val = HwVIPET->uDI_PD_CNT0_1.nDI_PD_CNT0_1;
	cnt_val[2][(num_fields)%5].val = HwVIPET->uDI_PD_CNT0_2.nDI_PD_CNT0_2;

	cnt_val[0][(num_fields+1)%5].val = HwVIPET->uDI_PD_CNT1_0.nDI_PD_CNT1_0;
	cnt_val[1][(num_fields+1)%5].val = HwVIPET->uDI_PD_CNT1_1.nDI_PD_CNT1_1;
	cnt_val[2][(num_fields+1)%5].val = HwVIPET->uDI_PD_CNT1_2.nDI_PD_CNT1_2;

	for ( n = 0; n < 2; n++ )	
	{
		cur_idx = num_fields % 5;

		prevent_flag_on = 0;
		for ( i = 0; i < 3; i++ )	
		{
			if ( 1 )	
			{
				int		min_idx, min_2nd_idx;
				int		new_cnts[5];
				for ( j = 0; j < 5; j++ )	
					new_cnts[j] = cnt_val[i][j].val;
		

				min_idx = find_min( new_cnts, -1 );
				min_2nd_idx = find_min( new_cnts, min_idx );
		
				if ( abs( new_cnts[min_idx] - new_cnts[min_2nd_idx] ) < (new_cnts[min_idx] * 2) )	
					prevent_flag_on = 0x1;
				

				frm_val_current[i] = min_idx;
			}
		}
		
		if ( prevent_flag_on )	
			flag[cur_idx] = 0;
		else	
		{
			if ( frm_val_current[0] == frm_val_predict[0] &&
				 frm_val_current[1] == frm_val_predict[1] &&
				 frm_val_current[2] == frm_val_predict[2] )

				flag[cur_idx] = 1;
		
			else	
				flag[cur_idx] = 0;
		
		}

		// check flags
		if ( num_fields > 5 )	
		{
			if ( flag[0] &  flag[1] &  flag[2] &  flag[3] & flag[4] )
				check_pulldown[n] = 1;
			else	
				check_pulldown[n] = 0;
		
		}
		else	
		{
			check_pulldown[0] = 0;
			check_pulldown[1] = 0;
		}

		frm_val_predict[0] = frm_val_current[0];
		frm_val_predict[1] = frm_val_current[1];
		frm_val_predict[2] = frm_val_current[2];

		num_fields++;
	}

	check_out = 0;
	if ( check_pulldown[0] & check_pulldown[1] )	
	{
		// num_fields : always even ( indicates the first field )
		//num_fields = imgin->numframes * 2;
		num_fields = numfrm * 2;
		minpos_offset = ( num_fields - frm_val_current[1] ) % 5;

		switch ( minpos_offset )	
		{
			case 3 :		// next 0
				check_out = 3;				// weaving current first field and second field of previous frame
				break;
			case 0 :		// next 2
				check_out = 3;				// weaving current first field and second field of previous frame
				break;
			case 2 :		// next 4
				check_out = 2;				// just weaving
				break;
			case 4 :		// next 1
				check_out = 2;				// just weaving
				break;
			case 1 :		// next 3
				check_out = 2;				// just weaving
				break;
		}
	}

	if ( check_out != HwVIPET->uDI_PD_STATUS.bDI_PD_STATUS.check_out )	{
#ifdef VSIMUL
		sim_debug( "[VIPET] Error!! pulldown detection error\0" );
		sim_fail();
#endif
		return -1;
	}
	return 1;

}


// ------------------------------------------------------------------------------
// disable region setting for spatial-temporal de-noiser
// 	In specified image region, the output of de-noiser is bypassed as original pixel value
//
// 		input : 	region_enable - on/off
//				region_swap - 0 : left region is original and right region is processed, 1 : the opposite
//				region_hor_center : horizontal region boundary
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_dnts_region( int region_enable, int region_swap, int region_hor_center )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	HwVIPET->uDN_DIV_IMG_3D.nDN_DIV_IMG = 0;
	HwVIPET->uDN_DIV_IMG_3D.nDN_DIV_IMG |= (region_enable << 15);
	HwVIPET->uDN_DIV_IMG_3D.nDN_DIV_IMG |= (!region_swap << 14);
	HwVIPET->uDN_DIV_IMG_3D.nDN_DIV_IMG |= (region_hor_center);
}

// ------------------------------------------------------------------------------
// disable region setting for spatial de-noiser
// 	In specified image region, the output of de-noiser is bypassed as original pixel value
//
// 		input : 	region_enable - on/off
//				region_swap - 0 : left region is original and right region is processed, 1 : the opposite
//				region_hor_center : horizontal region boundary
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_dnsp_region( int region_enable, int region_swap, int region_hor_center )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	HwVIPET->uDN_DIV_IMG_2D.nDN_DIV_IMG = 0;
	HwVIPET->uDN_DIV_IMG_2D.nDN_DIV_IMG |= (region_enable << 15);
	HwVIPET->uDN_DIV_IMG_2D.nDN_DIV_IMG |= (!region_swap << 14);
	HwVIPET->uDN_DIV_IMG_2D.nDN_DIV_IMG |= (region_hor_center);
}


// ------------------------------------------------------------------------------
// default setting for histgram hardware
//
// 		input : 	width, height, CDF on/off, LUT use on/off
// 		output :	success of fail(-1)
// ------------------------------------------------------------------------------
int viqe_set_histogram_basic_conf( int width, int height, int cdf_en, int lut_use )	
{
	int		i;
	int		h_offset_size;
	int		v_offset_size;
	int		sample_size;

	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	// size
	h_offset_size = 10;
	v_offset_size = 10;

	if ((height % v_offset_size) == 0)
		sample_size = (width/h_offset_size) * ((height / v_offset_size));
	else
		sample_size = (width/h_offset_size) * ((height / v_offset_size)+1);

	dprintk ("SampleSize = %d\n", sample_size);

	HwVIPET->uHI_CONFIG.nHI_CONFIG = 0;
	HwVIPET->uHI_CONFIG.nHI_CONFIG |= (h_offset_size);
	HwVIPET->uHI_CONFIG.nHI_CONFIG |= (v_offset_size << 8);
	HwVIPET->uHI_CONFIG.nHI_CONFIG |= (sample_size << 16);

	// measure auto
	HwVIPET->uHI_CTRL.nHI_CTRL |= (1 << 16);
	// multi frames
	HwVIPET->uHI_CTRL.nHI_CTRL &= ~(Hw17 | Hw18 | Hw19);
	HwVIPET->uHI_CTRL.nHI_CTRL |= (0 << 17);

	// segments init
	//for ( i = 0; i < 15; i++ )	{
	for ( i = 0; i < 16; i += 4 )	{
		HwVIPET->bHI_SEGS.segs[i/4] = (16*(i+4))<<24|(16*(i+3))<<16|(16*(i+2))<<8|(16*(i+1));
	}

	// scale init
	for ( i = 0; i < 4; i++ )	{
		//HwVIPET->bHI_SEGS.seg[i] = 16*i;
		HwVIPET->bHI_SCALES.scales[i] = 0xffffffff;
	}
	// top enable
	//HwVIPET->uTOP_CTRL.bTOP_CTRL.his_en = 1;
	HwVIPET->uTOP_CTRL.nTOP_CTRL |= (( cdf_en | lut_use ) << 19);

	// -------------------------------------
	// lut_direct_init
	// avoid clkgating...
	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE |= (1 << 5);

	// first, check histgram hardware turns off
	// TODO:
	
	HwVIPET->uHI_CTRL.nHI_CTRL |= (1 << 9);

	// wait
	if ( 1 )	{
		int		x = 0;
		while( 1 )	{
			if ( !HwVIPET->uHI_STATUS.bHI_STATUS.lut_user_update_busy )	{
				break;
			}
			x++;
			if ( x > 1000000 )		// check infinite-loop situation
				return -1;
		}
	}
	
	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE &= ~Hw5;
	// -------------------------------------

	// cdf_en
	HwVIPET->uHI_CTRL.nHI_CTRL = cdf_en ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw4 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw4);

	// lut_use
	HwVIPET->uHI_CTRL.nHI_CTRL = lut_use ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw7 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw7);

	return 1;
}

// ------------------------------------------------------------------------------
// enable function for histogram
//
// 		input : 	block enable, CDF generation enable, LUT use on/off
// 		output :	success or fail(-1)
// ------------------------------------------------------------------------------
int viqe_set_histogram_enable( int enable, int cdf_en, int lut_use )	
{
//	if ( !enable & ( cdf_en | lut_use ) )
//		return -1;
	dprintk(KERN_INFO "%s\n", __FUNCTION__);
	HwVIPET->uTOP_CTRL.nTOP_CTRL = enable ? HwVIPET->uTOP_CTRL.nTOP_CTRL | Hw18 : HwVIPET->uTOP_CTRL.nTOP_CTRL & (~Hw18);
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( cdf_en | lut_use ) ? HwVIPET->uTOP_CTRL.nTOP_CTRL | Hw19 : HwVIPET->uTOP_CTRL.nTOP_CTRL & (~Hw19);
	HwVIPET->uHI_CTRL.nHI_CTRL = cdf_en ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw4 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw4);
	HwVIPET->uHI_CTRL.nHI_CTRL = lut_use ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw7 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw7);
	return 1;
}

// ------------------------------------------------------------------------------
// detail configuration function for histogram
//
// 		input : 	width, height, CDF on/off, LUT use on/off, pixel counter offset,
// 					automode on/off, number of multi frames, segments, scales
// 		output :	success or fail(-1)
// ------------------------------------------------------------------------------
int
viqe_set_histogram_detail_conf( int width, int height, int cdf_en, int lut_use,
	   					   		int hoffset, int voffset, int automode, int multi_frames,
						   		unsigned char segs[16], unsigned char scales[16] )	{

	int		sample_size;
	int		i;

	sample_size = (width/hoffset) * ((height / voffset)+1);
	HwVIPET->uHI_CONFIG.nHI_CONFIG = 0;
	HwVIPET->uHI_CONFIG.nHI_CONFIG |= (hoffset);
	HwVIPET->uHI_CONFIG.nHI_CONFIG |= (voffset << 8);
	HwVIPET->uHI_CONFIG.nHI_CONFIG |= (sample_size << 16);

	// measure auto
	HwVIPET->uHI_CTRL.nHI_CTRL = automode ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw16 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw16);
	// multi frames
	HwVIPET->uHI_CTRL.nHI_CTRL &= ~(Hw17 | Hw18 | Hw19);
	HwVIPET->uHI_CTRL.nHI_CTRL |= (multi_frames << 17);

	// segments init
	for ( i = 0; i < 16; i += 4 )	{
		HwVIPET->bHI_SEGS.segs[i/4] = (segs[i*4+3])<<24|(segs[i*4+2])<<16|(segs[i*4+1])<<8|(segs[i*4+0]);
	}

	// scale init
	for ( i = 0; i < 4; i++ )	{
		HwVIPET->bHI_SCALES.scales[i] = (scales[i*4+3])<<24|(scales[i*4+2])<<16|(scales[i*4+1])<<8|(scales[i*4+0]);
	}

	// top enable
	//HwVIPET->uTOP_CTRL.bTOP_CTRL.his_en = 1;
	HwVIPET->uTOP_CTRL.nTOP_CTRL = ( cdf_en | lut_use ) ? HwVIPET->uTOP_CTRL.nTOP_CTRL | Hw19 : HwVIPET->uTOP_CTRL.nTOP_CTRL & (~Hw19);

	// -------------------------------------
	// lut_direct_init
	// avoid clkgating...
	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE |= (1 << 5);

	// first, check histgram hardware turns off
	// TODO:
	
	HwVIPET->uHI_CTRL.nHI_CTRL |= (1 << 9);

	// wait
	if ( 1 )	{
		int		x = 0;
		while( 1 )	{
			if ( !HwVIPET->uHI_STATUS.bHI_STATUS.lut_user_update_busy )	{
				break;
			}
			x++;
			if ( x > 1000000 )		// check infinite-loop situation
				return -1;
		}
	}

	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE &= (~Hw5);
	// -------------------------------------
	// cdf_en
	HwVIPET->uHI_CTRL.nHI_CTRL = cdf_en ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw4 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw4);

	// lut_use
	HwVIPET->uHI_CTRL.nHI_CTRL = lut_use ? HwVIPET->uHI_CTRL.nHI_CTRL | Hw7 : HwVIPET->uHI_CTRL.nHI_CTRL & (~Hw7);

	return 1;
}

// ------------------------------------------------------------------------------
// segments configuration function for histogram
//
// 		input : 	segments values having 16 unsigned char array
// 		output :	-
// ------------------------------------------------------------------------------
void
viqe_set_histogram_segments( unsigned char segs[16] )	{
	int		i;
	for ( i = 0; i < 16; i += 4 )	{
		HwVIPET->bHI_SEGS.segs[i/4] = (segs[i+3])<<24|(segs[i+2])<<16|(segs[i+1])<<8|(segs[i+0]);
	}
}

// ------------------------------------------------------------------------------
// return segments values in hardware register
//
// 		input : 	pointer of 16 elements for segments writing
// 		output :	-
// ------------------------------------------------------------------------------
void
viqe_get_histogram_segments( unsigned char segs[16] )	{
	int		i;
	for ( i = 0; i < 16; i += 4 )	{
		segs[i+3] = (HwVIPET->bHI_SEGS.segs[i/4] << 24) & 0xff;
		segs[i+2] = (HwVIPET->bHI_SEGS.segs[i/4] << 16) & 0xff;
		segs[i+1] = (HwVIPET->bHI_SEGS.segs[i/4] <<  8) & 0xff;
		segs[i+0] = (HwVIPET->bHI_SEGS.segs[i/4] <<  0) & 0xff;
	}
}

// ------------------------------------------------------------------------------
// scale configuration function for histogram
// if global scale value is not zero, scales array values are ignored.
//
// 		input : 	global scale value, scale values having 16 unsigned char array
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_histogram_scales( int gscale, unsigned char scales[16] )	
{
	int		i;
	if ( gscale != 0 )	
	{
		int		g_scale = CLIP3( gscale, 0, 0xff );
		for ( i = 0; i < 16; i += 4 )	
			HwVIPET->bHI_SCALES.scales[i/4] = (g_scale)<<24|(g_scale)<<16|(g_scale)<<8|(g_scale);
		
	}
	else	
	{
		for ( i = 0; i < 16; i += 4 )	
			HwVIPET->bHI_SCALES.scales[i/4] = (scales[i+3])<<24|(scales[i+2])<<16|(scales[i+1])<<8|(scales[i+0]);
	}
}

// ------------------------------------------------------------------------------
// return scales values in hardware register
//
// 		input : 	pointer of 16 elements for scales writing
// 		output :	-
// ------------------------------------------------------------------------------
void
viqe_get_histogram_scales( unsigned char scales[16] )	{
	int		i;
	for ( i = 0; i < 16; i += 4 )	{
		scales[i+3] = (HwVIPET->bHI_SCALES.scales[i/4] << 24) & 0xff;
		scales[i+2] = (HwVIPET->bHI_SCALES.scales[i/4] << 16) & 0xff;
		scales[i+1] = (HwVIPET->bHI_SCALES.scales[i/4] <<  8) & 0xff;
		scales[i+0] = (HwVIPET->bHI_SCALES.scales[i/4] <<  0) & 0xff;
	}
}

// ------------------------------------------------------------------------------
// LUT configuration function for histogram
//
// 		input : 	LUT values having 256 unsigned char array
// 		output :	-
// ------------------------------------------------------------------------------
void
viqe_set_histogram_luts( unsigned char luts[256] )	{
	int		i;
	for ( i = 0; i < 256; i += 4 )	{
		HwVIPET->bHI_LUTS.luts[i/4] = (luts[i+3])<<24|(luts[i+2])<<16|(luts[i+1])<<8|(luts[i+0]);
	}
}

// ------------------------------------------------------------------------------
// return LUT values stored in hardware
//
// 		input : 	pointer of 256 elements for LUT writing
// 		output :	-
// ------------------------------------------------------------------------------
void
viqe_get_histogram_luts( unsigned char luts[256] )	{
	int		i;
	for ( i = 0; i < 256; i += 4 )	{
		luts[i+3] = (HwVIPET->bHI_LUTS.luts[i/4] << 24) & 0xff;
		luts[i+2] = (HwVIPET->bHI_LUTS.luts[i/4] << 16) & 0xff;
		luts[i+1] = (HwVIPET->bHI_LUTS.luts[i/4] <<  8) & 0xff;
		luts[i+0] = (HwVIPET->bHI_LUTS.luts[i/4] <<  0) & 0xff;
	}
}

// ------------------------------------------------------------------------------
// region setting for histogram
//
// 		input : 	region_enable on/off, top-left point and bottom-right point
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_his_region( int region_enable, int region_idx_x_start, int region_idx_x_end, int region_idx_y_start, int region_idx_y_end )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	// region0;
	HwVIPET->uHI_REGION_0.nHI_REGION_0 = 0;
	HwVIPET->uHI_REGION_0.nHI_REGION_0 |= (region_idx_x_start);
	HwVIPET->uHI_REGION_0.nHI_REGION_0 |= (region_idx_x_end << 16);
	HwVIPET->uHI_REGION_0.nHI_REGION_0 |= (region_enable << 31);

	// region1;
	HwVIPET->uHI_REGION_1.nHI_REGION_1 = 0;
	HwVIPET->uHI_REGION_1.nHI_REGION_1 |= (region_idx_y_start);
	HwVIPET->uHI_REGION_1.nHI_REGION_1 |= (region_idx_y_end << 16);
}


// ------------------------------------------------------------------------------
// gamut map hardware enable function
//
// 		input : 	enable on/off
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_gamut_enable( int enable )
{
	HwVIPET->uTOP_CTRL.nTOP_CTRL = enable ? HwVIPET->uTOP_CTRL.nTOP_CTRL | Hw17 : HwVIPET->uTOP_CTRL.nTOP_CTRL & (~Hw17);
}

// ------------------------------------------------------------------------------
// gamut mapping hardware initialization function
//
// 		input : 	-
// 		output :	success of fail(-1)
// ------------------------------------------------------------------------------
int viqe_set_gamut_map_init( void )
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	// avoid clkgating...
	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE |= (1 << 5);

	HwVIPET->uGM_CTRL.nGM_CTRL |= (1);

	if ( 1 )
	{
		int		x = 0;
		while( 1 )	{
			if ( HwVIPET->uGM_STATUS.bGM_STATUS.done_init )	{
				break;
			}
			x++;
			if ( x > 1000000 )		// check infinite-loop situation
				return -1;
		}
	}
	HwVIPET->uTOP_CLKGATE.nTOP_CLKGATE &= (~Hw5);

	return 1;
}

// ------------------------------------------------------------------------------
// region setting for gamut-map
//
// 		input : 	region_enable on/off, top-left point and bottom-right point
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_set_gamut_region( int region_enable, int region_idx_x_start, int region_idx_x_end, int region_idx_y_start, int region_idx_y_end )	
{
	dprintk(KERN_INFO "%s\n", __FUNCTION__);

	// region0;
	HwVIPET->uGM_REGION_0.nGM_REGION_0 &= ~0x7fffffff;
//	HwVIPET->uGM_REGION_0.nGM_REGION_0 = 0;
	HwVIPET->uGM_REGION_0.nGM_REGION_0 |= (region_idx_x_start);
	HwVIPET->uGM_REGION_0.nGM_REGION_0 |= (region_idx_x_end << 16);
	HwVIPET->uGM_REGION_0.nGM_REGION_0 |= (region_enable << 31);

	// region1;
	HwVIPET->uGM_REGION_1.nGM_REGION_1 = 0;
	HwVIPET->uGM_REGION_1.nGM_REGION_1 |= (region_idx_y_start);
	HwVIPET->uGM_REGION_1.nGM_REGION_1 |= (region_idx_y_end << 16);
}

// ------------------------------------------------------------------------------
// bypass setting for gamut-map
//
// 		input : 	bypassing gamut-map hardware when bypass valuable is 1
// 		output :	-
// ------------------------------------------------------------------------------
void viqe_bypass_gamut( int bypass )
{
	HwVIPET->uGM_CTRL.nGM_CTRL = ( bypass )? HwVIPET->uGM_CTRL.nGM_CTRL | Hw31 : HwVIPET->uGM_CTRL.nGM_CTRL & (~Hw31);
}


// ------------------------------------------------------------------------------
// parameters for DNTS
// ------------------------------------------------------------------------------

static void* dev_viqe_GetCoeffDNSP(VIQEDNCOEFF coeffKind)
{
	if(coeffKind == VIQE_DNCOEFF_Y0)
		return  (unsigned int *)(&(HwVIPET->uDN_COEFF0_2D_Y));
	else
		return  (unsigned int *)(&(HwVIPET->uDN_COEFF0_2D_C));
}

static void* dev_viqe_GetCoeffDNTEHor(VIQEDNCOEFF coeffKind)
{
	if(coeffKind == VIQE_DNCOEFF_Y0)
		return  (unsigned int *)(&(HwVIPET->uDN_COEFF_HOR_Y0));
	else
		return  (unsigned int *)(&(HwVIPET->uDN_COEFF_HOR_C0));
}


static void* dev_viqe_GetCoeffDNTETem(VIQEDNCOEFF coeffKind)
{
	if(coeffKind == VIQE_DNCOEFF_Y0)
		return  (unsigned int *)(&(HwVIPET->uDN_COEFF_TEM_Y0));
	else
		return  (unsigned int *)(&(HwVIPET->uDN_COEFF_TEM_C0));
}

void cal_d2d_lpf_coeff_simple( int strength )	
{
	unsigned int i, cnt, nReg;
	unsigned int *nDst;
	unsigned int *nSrc;

	nDst = (unsigned int *)dev_viqe_GetCoeffDNSP(VIQE_DNCOEFF_Y0);

	for (i=0; i<5; i++)
	{
		if (d2d_lpf_coeff_list[i].strength == strength)
		{
			nSrc = &(d2d_lpf_coeff_list[i].luma_coeff0);

			for(cnt=0;cnt<4;cnt++)
			{
				if ((cnt==1) | (cnt==3))
				{
					nReg = *nDst & 0xffffff00;
					*nDst++ = nReg | *nSrc++;
					*nDst++;
					*nDst++;
				}
				else
				{
					*nDst++ = *nSrc++;
				}
			}
			break;
		}
	}
}

void cal_d2d_hpf_coeff_simple( int strengthY, int strengthC )	
{
	unsigned int i, cnt, nReg;
	unsigned int *pLUMDst;
	unsigned int *pLUMSrc;
	unsigned int *pCROMADst;
	unsigned int *pCROMASrc;

	pLUMDst = (unsigned int *)dev_viqe_GetCoeffDNSP(VIQE_DNCOEFF_Y0);
	pCROMADst = (unsigned int *)dev_viqe_GetCoeffDNSP(VIQE_DNCOEFF_C0);

	for (i=0; i<5; i++)
	{
		//LUM
		if (d2d_hpf_luma_coeff_list[i].strength == strengthY)
		{
			pLUMSrc = &(d2d_hpf_luma_coeff_list[i].coeff0);

			*pLUMDst++;
			nReg = *pLUMDst & 0xff;
			*pLUMDst++ = nReg | *pLUMSrc++;

			for(cnt=1;cnt<3;cnt++)
				*pLUMDst++ = *pLUMSrc++;
		}
		// CHROMA
		if (d2d_hpf_chroma_coeff_list[i].strength == strengthC)
		{
			pCROMASrc = &(d2d_hpf_chroma_coeff_list[i].coeff0);
			*pCROMADst++;
			nReg = *pCROMADst & 0xff;
			*pCROMADst++ = nReg | *pCROMASrc++;

			for(cnt=1;cnt<3;cnt++)
				*pCROMADst++ = *pCROMASrc++;
		}
	}
	
}


void cal_dnts_coeff_simple( int strengthY, int strengthC )	
{
	unsigned int i,cnt;
	unsigned int *pLUMDst;
	unsigned int *pLUMSrc;
	unsigned int *pCROMADst;
	unsigned int *pCROMASrc;

	pLUMDst = (unsigned int *)dev_viqe_GetCoeffDNTEHor(VIQE_DNCOEFF_Y0);
	pCROMADst = (unsigned int *)dev_viqe_GetCoeffDNTEHor(VIQE_DNCOEFF_C0);
	
	for (i=0; i<5; i++)
	{
		//LUM
		if (dnts_coeff_luma_list[i].strength == strengthY)
		{
			pLUMSrc = &(dnts_coeff_luma_list[i].hor0);

			for(cnt=0;cnt<4;cnt++)
				*pLUMDst++ = *pLUMSrc++;
		}

		// CHROMA
		if (dnts_coeff_chroma_list[i].strength == strengthC)
		{
			pCROMASrc = &(dnts_coeff_chroma_list[i].hor0);

			for(cnt=0;cnt<4;cnt++)
				*pCROMADst++ = *pCROMASrc++;
		}
	}
}


void cal_dnts_coeff_tmp_simple( int strengthY, int strengthC )	
{
	unsigned int i,cnt;
	unsigned int *pLUMDst;
	unsigned int *pLUMSrc;
	unsigned int *pCROMADst;
	unsigned int *pCROMASrc;


	pLUMDst = (unsigned int *)dev_viqe_GetCoeffDNTETem(VIQE_DNCOEFF_Y0);
	pCROMADst = (unsigned int *)dev_viqe_GetCoeffDNTETem(VIQE_DNCOEFF_C0);
	
	for (i=0; i<5; i++)
	{
		//LUM
		if (dnts_coeff_tmp_luma_list[i].strength == strengthY)
		{
			pLUMSrc = &(dnts_coeff_tmp_luma_list[i].tmp0);
			
			for(cnt=0;cnt<2;cnt++)
				*pLUMDst++ = *pLUMSrc++;
		}

		// CHROMA
		if (dnts_coeff_tmp_chroma_list[i].strength == strengthC)
		{
			pCROMASrc = &(dnts_coeff_tmp_chroma_list[i].tmp0);
			
			for(cnt=0;cnt<2;cnt++)
				*pCROMADst++ = *pCROMASrc++;
		}
	}
}



void viqe_OnTheFly_OnOff(int OnOff, int option)
{
	if(OnOff)
	{
		// directpath enable
		HwVIPET->uOD_CFG.nOD_CFG = (HwVIPET->uOD_CFG.nOD_CFG | Hw4);

		// ddi config, viqe->mscl direct path
		BITCSET(HwDDICONFIG->ON_THE_FLY, 0x000000ff, option);		//lclc0 : 0, lcdc1 : 1, mscl0 : 2, mscl1 : 3
	}
	else
	{
		// directpath disable
		HwVIPET->uOD_CFG.nOD_CFG =  (HwVIPET->uOD_CFG.nOD_CFG&(~Hw4));
			
		// ddi config, viqe->mscl direct path
		BITCSET(HwDDICONFIG->ON_THE_FLY, 0x000000ff, 0);	
	}
}


void VIQE_OddfirstOnOff (int OddFirstFlag)
{
	gOddFirstOnOff = OddFirstFlag;
	return;
}

void VIQE_DeintlOnOff (int OnOff)
{
	gDeintlOnOff = OnOff;
	return;
}

int IsFilmModeErr93 ( int FrmCnt, int width, int height ) {
	int RetVal = 0;
	static int PrevCheck = 0;
	int CurCheck;
	int JdrCnt;
	int	repeat_cond_0 = 0;
	int	repeat_cond_2 = 0;
	int	cnt_0 = 0;
	int	cnt_1 = 0;
	int	thres = width*11;

	CurCheck = HwVIPET->uDI_PD_STATUS.bDI_PD_STATUS.check_out;
	JdrCnt = HwVIPET->uDI_PD_CNT_JD_FRM.nDI_PD_CNT_JD_FRM;

	cnt_0 = HwVIPET->uDI_PD_CNT0_0.nDI_PD_CNT0_0;
	cnt_1 = HwVIPET->uDI_PD_CNT1_0.nDI_PD_CNT1_0;
	
	if ( CurCheck != 0 )
	{
		HwVIPET->uDI_CTRL.nDI_CTRL = HwVIPET->uDI_CTRL.nDI_CTRL & (~Hw5);
	}
	else
	{
		HwVIPET->uDI_CTRL.nDI_CTRL = HwVIPET->uDI_CTRL.nDI_CTRL | (Hw5);
	}
	repeat_cond_0 = ( CurCheck == 0 && PrevCheck != 0 );
	repeat_cond_2 = ( CurCheck != 0 && JdrCnt > thres);

	HwVIPET->uDI_CTRL.nDI_CTRL |= Hw31;
	HwVIPET->uDI_CTRL.nDI_CTRL &= (~Hw31);

	if ( repeat_cond_0 || repeat_cond_2 )
	{
		dprintk ("%d) Film mode err : cond_0 = %d, cond_2 = %d\n", FrmCnt, repeat_cond_0, repeat_cond_2 );
		dprintk (">>> check_out = %d, prev_check_out = %d, cnt_0 = %d, cnt_1 = %d\n", CurCheck, PrevCheck, cnt_0, cnt_1 );
		HwVIPET->uDI_CTRL.nDI_CTRL = HwVIPET->uDI_CTRL.nDI_CTRL | (Hw12);
		HwVIPET->uDI_PD_THRES0.nDI_PD_THRES0 = HwVIPET->uDI_PD_THRES0.nDI_PD_THRES0 & (~(Hw17|Hw16));
		CurCheck = 0;
		RetVal = 1;
	}
	else
	{
		HwVIPET->uDI_CTRL.nDI_CTRL = HwVIPET->uDI_CTRL.nDI_CTRL & (~Hw12);
	}
	
	PrevCheck = CurCheck;
	return RetVal;
}

void TurnOnOffVIQEClk (int OnOff)
{
	HwDDICONFIG->PWDN = (OnOff == 0) ? HwDDICONFIG->PWDN | (Hw1) : HwDDICONFIG->PWDN & (~Hw1);
	return;
}

void TurnOnOffScaler0Clk (int OnOff)
{
	HwDDICONFIG->PWDN = (OnOff == 0) ? HwDDICONFIG->PWDN | (Hw5) : HwDDICONFIG->PWDN & (~Hw5);
	return;
}

void TurnOnOffScaler1Clk (int OnOff)
{
	HwDDICONFIG->PWDN = (OnOff == 0) ? HwDDICONFIG->PWDN | (Hw6) : HwDDICONFIG->PWDN & (~Hw6);
	return;
}


void ODMARST (void)
{
	HwVIPET->uTOP_CTRL.nTOP_CTRL = (HwVIPET->uTOP_CTRL.nTOP_CTRL & (~Hw16));
	HwVIPET->uOD_CONTROL.nOD_CONTROL = (HwVIPET->uOD_CONTROL.nOD_CONTROL | Hw8);
	HwVIPET->uOD_CONTROL.nOD_CONTROL = (HwVIPET->uOD_CONTROL.nOD_CONTROL & (~Hw8));
	HwVIPET->uTOP_CTRL.nTOP_CTRL = (HwVIPET->uTOP_CTRL.nTOP_CTRL | (Hw16));
	return;
}

void ODMAFlush (void)
{
	HwVIPET->uOD_CONTROL.nOD_CONTROL = (HwVIPET->uOD_CONTROL.nOD_CONTROL | (Hw31));	
	HwVIPET->uOD_CONTROL.nOD_CONTROL = (HwVIPET->uOD_CONTROL.nOD_CONTROL & (~Hw31));	
	return;
}

void ShowVIQE(void)
{
	int i;
	unsigned int *ptr;

#if 1	// VIQE
	{
		printk ("####### VIQE ########\n");
		ptr = (unsigned int *)tcc_p2v(HwVIQE_BASE);
		for (i = 0; i < (sizeof (VIPET)/4); i++)
		{
			printk ("0x%x : 0x%x\n", 4*i, *(ptr + i));
		}
	}
#endif
#if 1	// Scaler 0
	{
		printk ("####### ScalerState0 ########\n");
		ptr = (unsigned int *)tcc_p2v(HwM2MSCALER0_BASE);
		for (i = 0; i < 21; i++)
		{
			printk ("0x%x : 0x%x\n", 4*i, *(ptr+i));
		}
	}
#endif
#if 0	// Scaler1
	{
		printk ("####### ScalerState1 ########\n");
		ptr = (unsigned int *)tcc_p2v(HwM2MSCALER1_BASE);
		for (i = 0; i < 21; i++)
		{
			printk ("0x%x : 0x%x\n", 4*i, *(ptr+i));
		}
	}
#endif
#if 1	// LCDC0
	{
		printk ("####### LCDC0 ########\n");
		ptr = (unsigned int *)tcc_p2v(HwLCDC0_BASE);
		for (i = 0; i <= (0xC0/4); i++)
		{
			printk ("0x%x : 0x%x\n", 4*i, *(ptr+i));
		}
	}
#endif
#if 0	// LCDC1
	{
		printk ("####### LCDC1 ########\n");
		ptr = (unsigned int *)tcc_p2v(HwLCDC1_BASE);
		for (i = 0; i <= (0xC0/4); i++)
		{
			printk ("0x%x : 0x%x\n", 4*i, *(ptr+i));
		}
	}
#endif
#if 0	// DDIConfig
	{
		printk ("####### DDIConfig ########\n");
		ptr = (unsigned int *)tcc_p2v(HwDDI_CONFIG_BASE);
		for (i = 0; i <= (0x5C/4); i++)
		{
			printk ("0x%x : 0x%x\n", 4*i, *(ptr+i));
		}
	}
#endif
#if 0	// DDICache
	{
		printk ("####### DDICache ########\n");
		ptr = (unsigned int *)tcc_p2v(HwDDICACHE_BASE);
		for (i = 0; i <= (0x10/4); i++)
		{
			printk ("0x%x : 0x%x\n", 4*i, *(ptr+i));
		}
	}
#endif
	return;
}


