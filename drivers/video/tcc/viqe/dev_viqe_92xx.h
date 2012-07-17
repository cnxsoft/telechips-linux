// vim:ts=4:sw=4

#ifndef _DEV_VIPET_H_
#define	_DEV_VIPET_H_

// ---------------------------------------------------------------------
// top
// ---------------------------------------------------------------------
typedef struct {
	uint	rdma_gen						:	 1;				// 00
	uint	rdma0_en						:	 1;				// 01
	uint	rdma1_en						:	 1;				// 02
	uint	rdma2_en						:	 1;				// 03

	uint	di_en							:	 1;				// 04
	uint	dnts_en							: 	 1;				// 05
	uint	dnsp_en							: 	 1;				// 06
	uint	cdma_en							:	 1;				// 07

	uint	denoise_mode					: 	 2;				// 08:09
	uint	hpf_mode						:	 1;				// 10
	uint									: 	 1;

	uint	di_spatial_only_en				: 	 1;				// 12
	uint	first_deintl_en					:	 1;				// 13
	uint									:	 2;

	uint	outdma_en						:	 1;				// 16
	uint	gamut_en						:	 1;				// 17
	uint	his_en							:	 1;				// 18
	uint	his_cdf_or_lut_en				:	 1;				// 19
	uint	fmt_conv_disable				:	 1;				// 20
	uint									:	11;
}	TOP_CTRL;

typedef	union {
	uint			nTOP_CTRL;
	TOP_CTRL		bTOP_CTRL;
}	TOP_CTRL_U;


typedef struct {
	uint	width							:	11;
	uint									:	 5;
	uint	height							:	11;
	uint									: 	 5;
}	TOP_SIZE;

typedef union {
	uint			nTOP_SIZE;
	TOP_SIZE		bTOP_SIZE;
}	TOP_SIZE_U;


typedef	struct {
	uint	p2p								:	 4;
	uint	v2h								:	 4;

	uint	h2v								:	 4;
	uint									:	 4;

	uint	h2h								:	 8;
	uint									:	 8;
}	TOP_TIMEGEN;

typedef union	{
	uint			nTOP_TIMEGEN;
	TOP_TIMEGEN		bTOP_TIMEGEN;
}	TOP_TIMEGEN_U;


typedef struct {
	uint	rdma_md_dly						:	 2;
	uint	dn_md_dly						:	 2;
	uint									:	28;
}	TOP_LUMADLY;

typedef union	{
	uint			nTOP_LUMADLY;
	TOP_LUMADLY		bTOP_LUMADLY;
}	TOP_LUMADLY_U;


typedef struct {
	uint	odd_first						:	 1;
	uint									:	31;
}	TOP_IMGCONF;

typedef	union	{
	uint			nTOP_IMGCONF;
	TOP_IMGCONF		bTOP_IMGCONF;
}	TOP_IMGCONF_U;


typedef struct	{
	uint	imgfmt							:	 3;
	uint									:	 1;
	uint	chroma_not_first_line			:	 1;
	uint	chroma_read_twice				:	 1;
	uint	first_prev_frame				:	 1;
	uint									:	25;
}	TOP_IMGFMT;

typedef union	{
	uint			nTOP_IMGFMT;
	TOP_IMGFMT		bTOP_IMGFMT;
}	TOP_IMGFMT_U;

typedef struct 	{
	uint	frmupdate_manual				:	 1;
	uint									:	22;

	uint	overlap_blocks_disable			:	 1;					// 23
	uint	frmupdate_ctrl_disable			:	 1;					// 24
	uint	frmupdate_md_dly_disable		:	 1;					// 25
	uint	frmupdatemux_disable			:	 1;					// 26
	uint	frmupdatereg_disable			:	 1;					// 27
	uint	scaler_direct					:	 1;					// 28
	uint	fmt_conv_flush					: 	 1;					// 29
	uint	lumadly_flush					:	 1;					// 30
	uint	tmpfifo_flush					:	 1;					// 31
}	TOP_MISC_CTRL;

typedef	union	{
	uint			nTOP_MISC_CTRL;
	TOP_MISC_CTRL	bTOP_MISC_CTRL;
}	TOP_MISC_CTRL_U; 


typedef struct 	{
	uint	frmupdate_restart				:	 1;
	uint									:	31; // 30 ->31 // 20090427
}	TOP_FRM_CTRL;

typedef	union	{
	uint			nTOP_FRM_CTRL;
	TOP_FRM_CTRL	bTOP_FRM_CTRL;
}	TOP_FRM_CTRL_U; 


typedef struct 	{
	uint	check_frm_done_cnts				:	16;
	uint	check_frm_done_disable			:	 1;					// 16
	uint									:	15;
}	TOP_EXCEPT0;

typedef	union	{
	uint			nTOP_EXCEPT0;
	TOP_EXCEPT0		bTOP_EXCEPT0;
}	TOP_EXCEPT0_U; 


typedef struct 	{
	uint	intmask							:	16;
	uint									:	16;
}	TOP_INTMASK;

typedef	union	{
	uint			nTOP_INTMASK;
	TOP_INTMASK		bTOP_INTMASK;
}	TOP_INTMASK_U; 


typedef struct 	{
	//uint	interrupt						:	16;
	uint	rdma							:	 1;					// 00
	uint	dn								:	 1;					// 01
	uint	di								:	 1;					// 02
	uint	whdma							:	 1;					// 03
	uint	outdma							:	 1;					// 04
	uint	pm								:	 1;					// 05
	uint									:	 2;
	uint	except_frmdone					:	 1;					// 08
	uint									:	 3;
	uint	err_regset						:	 4;					// 12:15
	uint									:	16;
}	TOP_INT;

typedef	union	{
	uint			nTOP_INT;
	TOP_INT			bTOP_INT;
}	TOP_INT_U; 


typedef struct 	{
	uint	clkgate_rdma					:	 1;					// 00
	uint	clkgate_denoise					:	 1;					// 01
	uint	clkgate_deintl					:	 1;					// 02
	uint	clkgate_whdma					:	 1;					// 03
	uint	clkgate_outdma					:	 1;					// 04
	uint	clkgate_pm						:	 1;					// 05
	uint									:	26;
}	TOP_CLKGATE;

typedef	union	{
	uint			nTOP_CLKGATE;
	TOP_CLKGATE		bTOP_CLKGATE;
}	TOP_CLKGATE_U; 


typedef struct 	{
	uint	version							:	32;
}	TOP_VERSION;

typedef	union	{
	uint			nTOP_VERSION;
	TOP_VERSION		bTOP_VERSION;
}	TOP_VERSION_U; 





// ---------------------------------------------------------------------
// di
// ---------------------------------------------------------------------
typedef	struct	{
	uint									:	 3;
	uint	dm_stati_thres_disable			:	 1;
	uint	maxrange_tmp_on					:	 1;					// 04
	uint	maxrange_spa_on					:	 1;					// 05
	uint	gen_thres_jaggy_en				:	 1;					// 06
	uint	spatial_jaggycheck				:	 1;					// 07

	uint	pulldown_on						:	 1;					// 08
	uint	pd_judder_on					:	 1;					// 09
	uint	pd_gen_thres_judder_en			:	 1;					// 10
	uint	pd_use_prevent_flag_on			:	 1;					// 11

	uint	pd_user_checkout_enable			:	 1;					// 12
	uint									:	 2;
	uint	pd_clear_internal_frm_idx 		: 	 1;					// 15

	uint									:	13;
	uint	bypass							:	 1;					// 29
	uint	flush_synch_fifo				:	 1;					// 30
	uint	internal_reg_clear				:	 1;					// 31
}	DI_CTRL;

typedef	union	{
	uint			nDI_CTRL;
	DI_CTRL			bDI_CTRL;
}	DI_CTRL_U;


typedef struct 	{
	uint	edi_thres_dir_hor				:	 8;
	uint	dm_thres_sad					:	 8;
	uint	dm_thres_pixel					:	 8;
	uint	edi_thres_dir_adap				:	 8;
}	DI_ENGINE0;

typedef union	{
	uint				nDI_ENGINE0;
	DI_ENGINE0		bDI_ENGINE0;
}	DI_ENGINE0_U;


typedef struct	{
	uint	maxrange_spa_type				:	 2;					// 00:01
	uint									:	 2;
	uint	edi_dir_adap_disable			:	 1;					// 04
	uint									:	 3;
	uint	dm_stati_thres_mul				:	 4;					// 08:11
	uint	dm_stati_thres_weight			:	 4;					// 12:15
	uint	gen_thres_jaggy_minthres		:	 8;					// 16:23
	uint	gen_thres_jaggy_maxthres		:	 8;					// 24:31
}	DI_ENGINE1;

typedef union	{
	uint			nDI_ENGINE1;
	DI_ENGINE1		bDI_ENGINE1;
}	DI_ENGINE1_U;


typedef struct	{
	uint	pd_thres_pulldown_cnts			:	10; // 00:09
	uint	pd_thres_weight					:	 3; // 09:11
	uint									:	 2; // 12:13
	uint	pd_thres_user_val_disable		:	 1; // 13
	uint	pd_user_checkout				:	 2; // 14:15
	uint									:	 2; // 16:17
	uint	pd_cnts_thres_pulldown_checkout	:	 4; // 18:21
	uint									:	 8; //22:31
}	DI_PD_THRES0;

typedef union	{
	uint			nDI_PD_THRES0;
	DI_PD_THRES0	bDI_PD_THRES0;
}	DI_PD_THRES0_U;


typedef struct	{
	uint	pd_thres_pulldown_0				:	 8;
	uint	pd_thres_pulldown_1				:	 8;
	uint	pd_thres_pulldown_2				:	 8;
	uint									:	 8;
}	DI_PD_THRES1;

typedef	union	{
	uint			nDI_PD_THRES1;
	DI_PD_THRES1	bDI_PD_THRES1;
}	DI_PD_THRES1_U;


typedef struct	{
	uint	pd_thres_cnt_judder				: 	 8;
	uint	pd_judder_downline_margin		:	 4;
	uint	pd_judder_horline_margin		:	 4;
	uint	pd_gen_thres_judder_minthres	:	 8;
	uint	pd_gen_thres_judder_maxthres	:	 8;
}	DI_PD_JUDDER;

typedef	union	{
	uint			nDI_PD_JUDDER;
	DI_PD_JUDDER	bDI_PD_JUDDER;
}	DI_PD_JUDDER_U;


typedef struct	{
	uint	initval_idx_x_mst				: 	 8;
	uint	initval_idx_y_mst				:	 8;

	uint	initval_x_mst_end				:	 6;
	uint	initval_y_mst_jump				:	 2;

	uint	initval_last_two_line			:	 4;
	uint									:	 4;
}	DI_MISC_CTRL;

typedef	union	{
	uint			nDI_MISC_CTRL;
	DI_MISC_CTRL	bDI_MISC_CTRL;
}	DI_MISC_CTRL_U;


typedef struct	{
	uint	busy							: 	 1;
	uint									:	 7;

	uint	idx_x_proc						:	11;
	uint	idx_y_proc						:	11;
	uint									:	 2;
}	DI_STATUS;

typedef	union	{
	uint			nDI_STATUS;
	DI_STATUS		bDI_STATUS;
}	DI_STATUS_U;


typedef struct	{
	uint	pulldown_cur_state				: 	 4;
	uint	check_out						: 	 2;
	uint									:	 2;

	uint	minpos_offset					:	 3;
	uint									:	 5;

	uint									:	16;
}	DI_PD_STATUS;

typedef	union	{
	uint			nDI_PD_STATUS;
	DI_PD_STATUS	bDI_PD_STATUS;
}	DI_PD_STATUS_U;


typedef struct	{
	uint	region_idx_x_start				: 	11;
	uint									:	 5;
	uint	region_idx_x_end				: 	11;
	uint									:	 4;
	uint	region_enable					: 	 1;
}	DI_REGION_0;

typedef	union	{
	uint			nDI_REGION_0;
	DI_REGION_0		bDI_REGION_0;
}	DI_REGION_0_U;


typedef struct	{
	uint	region_idx_y_start				: 	11;
	uint									:	 5;
	uint	region_idx_y_end				: 	11;
	uint									:	 5;
}	DI_REGION_1;

typedef	union	{
	uint			nDI_REGION_1;
	DI_REGION_1		bDI_REGION_1;
}	DI_REGION_1_U;


typedef struct	{
	uint	interrupt						: 	16;
	uint	intmask							: 	16;
}	DI_INT;

typedef	union	{
	uint			nDI_INT;
	DI_INT			bDI_INT;
}	DI_INT_U;


typedef struct	{
	uint	cnts							: 	20;
	uint									: 	12;
}	DI_PD_CNT0_0;

typedef	union	{
	uint			nDI_PD_CNT0_0;
	DI_PD_CNT0_0	bDI_PD_CNT0_0;
}	DI_PD_CNT0_0_U;


typedef struct	{
	uint	cnts							: 	20;
	uint									: 	12;
}	DI_PD_CNT0_1;

typedef	union	{
	uint			nDI_PD_CNT0_1;
	DI_PD_CNT0_1	bDI_PD_CNT0_1;
}	DI_PD_CNT0_1_U;


typedef struct	{
	uint	cnts							: 	20;
	uint									: 	12;
}	DI_PD_CNT0_2;

typedef	union	{
	uint			nDI_PD_CNT0_2;
	DI_PD_CNT0_2	bDI_PD_CNT0_2;
}	DI_PD_CNT0_2_U;


typedef struct	{
	uint	cnts							: 	20;
	uint									: 	12;
}	DI_PD_CNT1_0;

typedef	union	{
	uint			nDI_PD_CNT1_0;
	DI_PD_CNT1_0	bDI_PD_CNT1_0;
}	DI_PD_CNT1_0_U;


typedef struct	{
	uint	cnts							: 	20;
	uint									: 	12;
}	DI_PD_CNT1_1;

typedef	union	{
	uint			nDI_PD_CNT1_1;
	DI_PD_CNT1_1	bDI_PD_CNT1_1;
}	DI_PD_CNT1_1_U;


typedef struct	{
	uint	cnts							: 	20;
	uint									: 	12;
}	DI_PD_CNT1_2;

typedef	union	{
	uint			nDI_PD_CNT1_2;
	DI_PD_CNT1_2	bDI_PD_CNT1_2;
}	DI_PD_CNT1_2_U;



// ---------------------------------------------------------------------
// dn
// ---------------------------------------------------------------------
typedef	struct	{
	uint		coeff						:	32;
}	DN_COEFF;

typedef union {
	uint			nDN_COEFF;
	DN_COEFF		bDN_COEFF;
}	DN_COEFF_U;


typedef	struct	{
	uint	pcnt							:	11;
	uint									:	 4;
	uint	update_reg						:	 1;
	uint	lcnt							:	11;
	uint									:	 2;
	uint	int_mask						:	 1;
	uint	interrupt						:	 1;
	uint	busy							:	 1;
}	DN_LINE_STATE;

typedef union	{
	uint			nDN_LINE_STATE;
	DN_LINE_STATE	bDN_LINE_STATE;	
}	DN_LINE_STATE_U;


typedef struct {
	uint	para_a							:	 8;	
	uint	para_ba							:	 8;
	uint	para_ca							:	 8;
	uint	para_slope						:	 8;
}	DN_COEFF0_2D;

typedef union	{
	uint			nDN_COEFF0_2D;
	DN_COEFF0_2D	bDN_COEFF0_2D;
}	DN_COEFF0_2D_U;


typedef struct {
	uint	para_y_plane					:	 8;	
	uint	para_d							:	 8;
	uint	para_e							:	 8;
	uint	para_f							:	 8;
}	DN_COEFF1_2D;

typedef union	{
	uint			nDN_COEFF1_2D;
	DN_COEFF1_2D	bDN_COEFF1_2D;
}	DN_COEFF1_2D_U;


typedef struct {
	uint	para_g							:	 8;	
	uint	para_m1							:	 8;
	uint	para_m2							:	 8;
	uint	para_l							:	 8;
}	DN_COEFF2_2D;

typedef union	{
	uint			nDN_COEFF2_2D;
	DN_COEFF2_2D	bDN_COEFF2_2D;
}	DN_COEFF2_2D_U;


typedef struct {
	uint	para_y_plane_h1					:	 9;	
	uint									:	 7;
	uint	para_y_plane_h2					:	 9;
	uint									:	 7;
}	DN_COEFF3_2D;

typedef union	{
	uint			nDN_COEFF3_2D;
	DN_COEFF3_2D	bDN_COEFF3_2D;
}	DN_COEFF3_2D_U;


typedef struct	{
	uint	bypass_dnsp						:	 1;
	uint	bypass_dnts						:	 1;
	uint									:	28;
	uint	fifo_flush						: 	 2;
}	DN_CTRL;

typedef union	{
	uint			nDN_CTRL;
	DN_CTRL			bDN_CTRL;
}	DN_CTRL_U;


typedef struct	{
	uint	state_0							:	16;
	uint	state_1							:	16;
}	DN_FIFOSTATE;

typedef union	{
	uint			nDN_FIFOSTATE;
	DN_FIFOSTATE	bDN_FIFOSTATE;
}	DN_FIFOSTATE_U;


typedef struct	{
	uint	pos								:	11;
	uint									:	3;
	uint	dtog							:	 1;	
	uint	den								:	 1;
	uint									:	16;
}	DN_DIV_IMG;

typedef union	{
	uint			nDN_DIV_IMG;
	DN_DIV_IMG		bDN_DIV_IMG;
}	DN_DIV_IMG_U;


// ---------------------------------------------------------------------
// rdma
// ---------------------------------------------------------------------
typedef	struct	{
	uint	pcnt							:	11;
	uint									:	 4;
	uint	update_reg						:	 1;
	uint	lcnt							:	11;
	uint									:	 2;
	uint	busy							:	 3;
}	RD_LINE_STATE;

typedef union	{
	uint			nRD_LINE_STATE;
	RD_LINE_STATE	bRD_LINE_STATE;	
}	RD_LINE_STATE_U;


typedef	struct	{
	uint	pcnt							:	11;
	uint									:	 3;
	uint	frm_done						:	 1;
	uint	update_reg						:	 1;
	uint	lcnt							:	11;
	uint									:	 1;
	uint	int_mask						:	 2;
	uint	interrupt						:	 1;
	uint									:	 1;
}	RD_LINE_STATE1;

typedef union	{
	uint			nRD_LINE_STATE1;
	RD_LINE_STATE1	bRD_LINE_STATE1;	
}	RD_LINE_STATE1_U;


typedef	struct {
	uint	baseaddr						:	32;
}	RD_BASEADDR;

typedef union	{
	uint			nRD_BASEADDR;
	RD_BASEADDR		bRD_BASEADDR;
}	RD_BASEADDR_U;


typedef	struct {
	uint	base_ofs0						:16;
	uint	base_ofs1						:16;
}	RD_BASEOFS;

typedef union	{
	uint			nRD_BASEOFS;
	RD_BASEOFS		bRD_BASEOFS;
}	RD_BASEOFS_U;


typedef	struct	{
	uint									:	 4;
	uint	rdopt							:	 1;
	uint	sc								:	 1;
	uint	toggle							:	 1;
	uint	bypass_comp						:	 1;
	uint	rdma_cont						:	 1;
	uint									:	 3;
	uint	comp_table_mode					:	 4;
	uint	clear_decomp_buff				:	 1;
	uint									:	15;
}	RD_CTRL;

typedef union	{
	uint			nRD_CTRL;
	RD_CTRL			bRD_CTRL;
}	RD_CTRL_U;


typedef	struct	{
	uint		comp_pixel_limit			:	32;
}	RD_COMP_PL;

typedef union 	{
	uint			nRD_COMP_PL;
	RD_COMP_PL		bRD_COMP_PL;
}	RD_COMP_PL_U;



// ---------------------------------------------------------------------
// whdma
// ---------------------------------------------------------------------
typedef struct 	{
	uint	coded_data_m1					:	 8;
	uint	coded_data_m2					:	 8;
	uint	coded_data_m3					:	 8;
	uint	coded_data_m4					:	 8;
}	COMP_COEFF0;

typedef union 	{
	uint			uCOMP_COEFF0;
	COMP_COEFF0		bCOMP_COEFF0;
}	COMP_COEFF0_U;


typedef struct {
	uint	coded_data_m1				:	 8;
	uint	coded_data_m2				:	 8;
	uint	coded_data_m3				:	 8;
	uint								:	 8;
}	COMP_COEFF1;

typedef union 	{
	uint			uCOMP_COEFF1;
	COMP_COEFF1		bCOMP_COEFF1;
}	COMP_COEFF1_U;


typedef struct {
	uint	coded_data_0					:	 8;
	uint	coded_data_1					:	 8;
	uint	coded_data_2					:	 8;
	uint	coded_data_3					:	 8;
}	COMP_COEFF2;

typedef union 	{
	uint			uCOMP_COEFF2;
	COMP_COEFF2		bCOMP_COEFF2;
}	COMP_COEFF2_U;


typedef struct {
	uint	coded_data_4					:	 8;
	uint	coded_data_5					:	 8;
	uint	coded_data_6					:	 8;
	uint	coded_data_7					:	 8;
}	COMP_COEFF3;

typedef union 	{
	uint			uCOMP_COEFF3;
	COMP_COEFF3		bCOMP_COEFF3;
}	COMP_COEFF3_U;


typedef struct	{
	uint	code_length_m1					:	 4;
	uint	code_length_m2					:	 4;
	uint	code_length_m3					:	 4;
	uint	code_length_m4					:	 4;
	uint	code_length_m5					:	 4;
	uint	code_length_m6					:	 4;
	uint	code_length_m7					:	 4;
	uint	undef_0							:	 4;
}	COMP_COEFF4;

typedef union	{
	uint			uCOMP_COEFF4;
	COMP_COEFF4		bCOMP_COEFF4;
}	COMP_COEFF4_U;


typedef struct	{
	uint	code_length_0					:	 4;
	uint	code_length_1					:	 4;
	uint	code_length_2					:	 4;
	uint	code_length_3					:	 4;
	uint	code_length_4					:	 4;
	uint	code_length_5					:	 4;
	uint	code_length_6					:	 4;
	uint	code_length_7					:	 4;
}	COMP_COEFF5;

typedef union	{
	uint			uCOMP_COEFF5;
	COMP_COEFF5		bCOMP_COEFF5;
}	COMP_COEFF5_U;


typedef	struct {
	uint	baseaddr						:	32;
}	CD_BASEADDR;

typedef union	{
	uint			nCD_BASEADDR;
	CD_BASEADDR		bCD_BASEADDR;
}	CD_BASEADDR_U;

typedef struct	{
	uint	cdma_state_1					:	 3;
	uint	ou_1							:	 1;
	uint									:	 9;
	uint	frm_done						:	 1;
	uint	int_mask						:	 1;
	uint	interrupt						:	 1;
	uint	cdma_state_0					:	 3;
	uint	ou_0							:	 1;
	uint									:	11;
	uint	busy							:	 1;
}	CD_STATE;

typedef union	{
	uint			nCD_STATE;
	CD_STATE		bCD_STATE;
}	CD_STATE_U;


typedef struct	{
	uint									:	 4;
	uint	cdopt							:	 1;
	uint	toggle							:	 1;
	uint	bypass_comp						:	 1;
	uint									:	 1;
	uint	comp_table_mode					:	 4;
	uint									:	 3;
	uint	clear_comp_buff					:	 1;
	uint									:	16;
}	CD_CTRL;

typedef union	{
	uint			nCD_CTRL;
	CD_CTRL			bCD_CTRL;
}	CD_CTRL_U;



// ---------------------------------------------------------------------
// outdma
// ---------------------------------------------------------------------
typedef	struct {
	uint	baseaddr						:	32;
}	OD_BASEADDR;

typedef union	{
	uint			nOD_BASEADDR;
	OD_BASEADDR		bOD_BASEADDR;
}	OD_BASEADDR_U;


typedef struct {
	uint	width							:	11;
	uint									:	 5;
	uint	height							:	11;
	uint									: 	 5;
}	OD_SIZE;

typedef union {
	uint			nOD_SIZE;
	OD_SIZE			bOD_SIZE;
}	OD_SIZE_U;


typedef	struct {
	uint	base_ofs0						:	16;
	uint	base_ofs1						:	16;
}	OD_BASEOFS;

typedef union	{
	uint			nOD_BASEOFS;
	OD_BASEOFS		bOD_BASEOFS;
}	OD_BASEOFS_U;


typedef struct {
	uint	dsttype							:	 4;
	uint	outpath							:	 1;
	uint	dstintm							:	 1;
	uint	outp_ready						:    1;
	uint									:	 1;
	uint	outp_waitcnt					:	 3;
	uint	dst420ctype						:	 1;
	uint									:	20;
}	OD_CFG;

typedef union {
	uint			nOD_CFG;
	OD_CFG			bOD_CFG;
}	OD_CFG_U;


typedef struct 	{
	uint									:	 1;
	uint	ien_ready						:	 1;
	uint	ien_busy						:	 1;
	uint	ien_done						:	 1;
	uint	bypass							:	 1;
	uint	cont							:	 1;
	uint	mid_irq_en						:	 1;
	uint	roll_irq_en						:	 1;
	uint	reg_reset						:	 1;
	uint	oup_underrun_irq_en				:	 1;
	uint									:	 6;
	uint	mid_gs_en						:	 1;
	uint	roll_gs_en						:	 1;
	uint									:	 4;
	uint	mid_en							:	 1;
	uint	roll_en							:	 1;
	uint									:	 6;
	uint	opt_testen						:	 1;
	uint	opt_clkgate						: 	 1;
}	OD_CONTROL;

typedef union {
	uint			nOD_CONTROL;
	OD_CONTROL		bOD_CONTRL;
}	OD_CONTROL_U;


typedef struct	{
	uint	mid_vcnt						:	12;
	uint									:	 4;
	uint	roll_vcnt						:	12;
	uint									:	 4;
}	OD_ROLLVCNT;

typedef union	{
	uint			nOD_ROLLVCNT;
	OD_ROLLVCNT		bOD_ROLLVCNT;
}	OD_ROLLVCNT_U;



// ---------------------------------------------------------------------
// gamut_map
// ---------------------------------------------------------------------
typedef struct	{
	uint	init							:	 1;			// 00
	uint	round							:	 1;			// 01
	uint									:	29;
	uint	bypass_mux						:	 1;			// 31
}	GM_CTRL;

typedef union	{
	uint			nGM_CTRL;
	GM_CTRL			bGM_CTRL;
}	GM_CTRL_U;


typedef struct	{
	uint	done_init						:	 1;			// 00
	uint	busy_init						:	 1;			// 01
	uint									:	30;
}	GM_STATUS;

typedef union	{
	uint			nGM_STATUS;
	GM_STATUS		bGM_STATUS;
}	GM_STATUS_U;


typedef struct	{
	uint	region_x_start					: 	11;
	uint									:	 5;
	uint	region_x_end					: 	11;
	uint									:	 4;
	uint	region_enable					: 	 1;
}	GM_REGION_0;

typedef	union	{
	uint			nGM_REGION_0;
	GM_REGION_0		bGM_REGION_0;
}	GM_REGION_0_U;


typedef struct	{
	uint	region_y_start					: 	11;
	uint									:	 5;
	uint	region_y_end					: 	11;
	uint									:	 5;
}	GM_REGION_1;

typedef	union	{
	uint			nGM_REGION_1;
	GM_REGION_1		bGM_REGION_1;
}	GM_REGION_1_U;


typedef	struct	{
	uint	interrupts						:	16;
	uint									:	16;
}	GM_INT;

typedef	union	{
	uint			nGM_INT;
	GM_INT			bGM_INT;
}	GM_INT_U;


// ---------------------------------------------------------------------
// his
// ---------------------------------------------------------------------
typedef struct	{
	uint	measure_start					:	 1;			// 00
	uint	measure_finish					:	 1;			// 01
	uint									:	 1;
	uint	norm_inter_start				:	 1;			// 03

	uint	cdf_en							:	 1;			// 04
	uint	lut_user_update_start			:	 1;			// 05
	uint	lut_init						:	 1;			// 06
	uint	lut_use							:	 1;			// 07

	uint	lut_direct_update				:	 1;			// 08
	uint	lut_direct_init					:	 1;			// 09
	uint									:	 5;
	uint	measure_direct_finish			:	 1;			// 15

	uint	measure_auto					:	 1;			// 16
	uint	multi_frames					:	 3;			// 17:19
	uint									:	10;
	uint	busy_ctrl_disable				:	 1;			// 30
	uint	clear_internal_vars				:	 1;			// 31
}	HI_CTRL;

typedef union	{
	uint			nHI_CTRL;
	HI_CTRL			bHI_CTRL;
}	HI_CTRL_U;


typedef struct	{
	uint	busy							:	 1;			// 00
	uint	norm_bank_rdy					:	 1;			// 01
	uint									:	 2;
	uint	lut_user_update_busy			:	 1;			// 04
	uint									:	 3;

	uint									:	 8;
	uint	measure_ctrl_state				:	 1;			// 16
	uint	norm_inter_ctrl_state			:	 3;			// 17:19
	uint									:	12;
}	HI_STATUS;

typedef union	{
	uint			nHI_STATUS;
	HI_STATUS		bHI_STATUS;
}	HI_STATUS_U;


typedef struct	{
	uint	h_offset_size					:	 5;			// 00:04
	uint									:	 3;
	uint	v_offset_size					:	 5;			// 08:12
	uint									:	 2;
	uint	round							:	 1;			// 15

	uint	sample_size						:	15;			// 16:30
	uint									:	 1;
}	HI_CONFIG;

typedef union	{
	uint			nHI_CONFIG;
	HI_CONFIG		bHI_CONFIG;
}	HI_CONFIG_U;


typedef struct	{
	uint	region_x_start					: 	11;
	uint									:	 5;
	uint	region_x_end					: 	11;
	uint									:	 4;
	uint	region_enable					: 	 1;
}	HI_REGION_0;

typedef	union	{
	uint			nHI_REGION_0;
	HI_REGION_0		bHI_REGION_0;
}	HI_REGION_0_U;


typedef struct	{
	uint	region_y_start					: 	11;
	uint									:	 5;
	uint	region_y_end					: 	11;
	uint									:	 5;
}	HI_REGION_1;

typedef	union	{
	uint			nHI_REGION_1;
	HI_REGION_1		bHI_REGION_1;
}	HI_REGION_1_U;


typedef	struct	{
	uint	interrupts						:	16;
	uint									:	16;
}	HI_INT;

typedef	union	{
	uint			nHI_INT;
	HI_INT			bHI_INT;
}	HI_INT_U;

typedef	struct	{
	uint	segs[4];
}	HI_SEGS;


typedef	struct	{
	uint	cdfs[4];
}	HI_CDFS;

typedef	struct	{
	uint	scales[4];
}	HI_SCALES;

typedef	struct	{
	uint	pdfs[8];
}	HI_PDFS;


// HIS LUT
typedef	struct	{
	uint 	luts[256/4];
}	HI_LUTS;




// ---------------------------------------------------------------------
// instance
// ---------------------------------------------------------------------
typedef	volatile struct {
	TOP_CTRL_U				uTOP_CTRL ;				// 0x000
	TOP_SIZE_U				uTOP_SIZE;				// 0x004
	TOP_TIMEGEN_U			uTOP_TIMEGEN;			// 0x008
	TOP_LUMADLY_U			uTOP_LUMADLY;			// 0x00c
	TOP_IMGCONF_U			uTOP_IMGCONF;			// 0x010
	TOP_IMGFMT_U			uTOP_IMGFMT;			// 0x014
	TOP_MISC_CTRL_U			uTOP_MISC_CTRL;			// 0x018
	TOP_FRM_CTRL_U			uTOP_FRM_CTRL;			// 0x01c
	TOP_INT_U				uTOP_INT;				// 0x020
	TOP_INTMASK_U			uTOP_INTMASK;			// 0x024
	TOP_EXCEPT0_U			uTOP_EXCEPT0;			// 0x028
	uint					undef_0_1[3];			// 0x02c ~ 0x034
	TOP_CLKGATE_U			uTOP_CLKGATE;			// 0x038
	TOP_VERSION_U			uTOP_VERSION;			// 0x03c
	uint					undef_0[16];			// 0x040 ~ 0x07c

	DI_CTRL_U				uDI_CTRL;				// 0x080
	DI_ENGINE0_U			uDI_ENGINE0;			// 0x084
	DI_ENGINE1_U			uDI_ENGINE1;			// 0x088
	DI_PD_THRES0_U			uDI_PD_THRES0;			// 0x08c
	DI_PD_THRES1_U			uDI_PD_THRES1;			// 0x090
	DI_PD_JUDDER_U			uDI_PD_JUDDER; 			// 0x094
	DI_MISC_CTRL_U			uDI_MISC_CTRL; 			// 0x098
	uint					undef_1_1;				// 0x09c
	DI_STATUS_U				uDI_STATUS; 			// 0x0a0
	DI_PD_STATUS_U			uDI_PD_STATUS; 			// 0x0a4
	DI_REGION_0_U			uDI_REGION_0;			// 0x0a8
	DI_REGION_1_U			uDI_REGION_1;			// 0x0ac
	uint					undef_1_2[3];			// 0x0b0 ~ 0x0b8
	DI_INT_U				uDI_INT;		 		// 0x0bc
	DI_PD_CNT0_0_U			uDI_PD_CNT0_0;			// 0x0c0
	DI_PD_CNT0_1_U			uDI_PD_CNT0_1;			// 0x0c4
	DI_PD_CNT0_2_U			uDI_PD_CNT0_2;			// 0x0c8
	DI_PD_CNT1_0_U			uDI_PD_CNT1_0;			// 0x0cc
	DI_PD_CNT1_1_U			uDI_PD_CNT1_1;			// 0x0d0
	DI_PD_CNT1_2_U			uDI_PD_CNT1_2;			// 0x0d4
	uint					undef_1[10];			// 0x0d8 ~ 0x0fc

	DN_COEFF_U				uDN_COEFF_HOR_Y0;		// 0x100
	DN_COEFF_U				uDN_COEFF_HOR_Y1;		// 0x104
	DN_COEFF_U				uDN_COEFF_VER_Y0;		// 0x108
	DN_COEFF_U				uDN_COEFF_VER_Y1;		// 0x10c
	DN_COEFF_U				uDN_COEFF_TEM_Y0;		// 0x110
	DN_COEFF_U				uDN_COEFF_TEM_Y1;		// 0x114
	DN_COEFF_U				uDN_COEFF_HOR_C0;		// 0x118
	DN_COEFF_U				uDN_COEFF_HOR_C1;		// 0x11c
	DN_COEFF_U				uDN_COEFF_VER_C0;		// 0x120
	DN_COEFF_U				uDN_COEFF_VER_C1;		// 0x124
	DN_COEFF_U				uDN_COEFF_TEM_C0;		// 0x128
	DN_COEFF_U				uDN_COEFF_TEM_C1;		// 0x12c
	DN_LINE_STATE_U			uDN_LINE_STATE0_3D;		// 0x130
	DN_LINE_STATE_U			uDN_LINE_STATE1_3D;		// 0x134
	DN_DIV_IMG_U			uDN_DIV_IMG_3D;
	uint					undef_2[1];				// 0x13c	
	DN_COEFF0_2D_U			uDN_COEFF0_2D_Y;		// 0x140
	DN_COEFF1_2D_U			uDN_COEFF1_2D_Y;		// 0x144
	DN_COEFF2_2D_U			uDN_COEFF2_2D_Y;		// 0x148
	DN_COEFF3_2D_U			uDN_COEFF3_2D_Y;		// 0x14c
	DN_COEFF0_2D_U			uDN_COEFF0_2D_C;		// 0x150
	DN_COEFF1_2D_U			uDN_COEFF1_2D_C;		// 0x154
	DN_COEFF2_2D_U			uDN_COEFF2_2D_C;		// 0x158
	DN_COEFF3_2D_U			uDN_COEFF3_2D_C;		// 0x15c
	DN_FIFOSTATE_U			uDN_FIFOSTATE;			// 0x160
	DN_LINE_STATE_U			uDN_LINE_STATE0_2D;		// 0x164
	DN_LINE_STATE_U			uDN_LINE_STATE1_2D;		// 0x168
	DN_CTRL_U				uDN_CTRL;				// 0x16c
	DN_DIV_IMG_U			uDN_DIV_IMG_2D;
	uint					undef_3[3];				// 0x174 ~ 0x17c

	RD_BASEADDR_U			uRD_IMG0_BASE0;			// 0x180
	RD_BASEADDR_U			uRD_IMG0_BASE1;			// 0x184
	RD_BASEADDR_U			uRD_IMG0_BASE2;			// 0x188
	RD_BASEOFS_U			uRD_IMG0_OFS;			// 0x18c
	RD_BASEADDR_U			uRD_IMG1_BASE0;			// 0x190
	RD_BASEADDR_U			uRD_IMG1_BASE1;			// 0x194
	RD_BASEADDR_U			uRD_IMG1_BASE2;			// 0x198
	RD_BASEOFS_U			uRD_IMG1_OFS;			// 0x19c
	RD_BASEADDR_U			uRD_IMG2_BASE0_0;		// 0x1a0
	RD_BASEADDR_U			uRD_IMG2_BASE1_0;		// 0x1a4
	RD_BASEADDR_U			uRD_IMG2_BASE2_0;		// 0x1a8
	RD_BASEADDR_U			uRD_IMG2_BASE0_1;		// 0x1ac
	RD_BASEADDR_U			uRD_IMG2_BASE1_1;		// 0x1b0
	RD_BASEADDR_U			uRD_IMG2_BASE2_1;		// 0x1b4
	RD_BASEADDR_U			uRD_CURR_ADDR0;			// 0x1b8
	RD_BASEADDR_U			uRD_CURR_ADDR1;			// 0x1bc
	RD_BASEADDR_U			uRD_CURR_ADDR2;			// 0x1c0
	DN_FIFOSTATE_U			uRD_FIFOSTATE;			// 0x1c4
	RD_LINE_STATE_U			uRD_LINE_STATE0;		// 0x1c8
	RD_LINE_STATE1_U		uRD_LINE_STATE1;		// 0x1cc
	RD_CTRL_U				uRD_CTRL;				// 0x1d0
	RD_COMP_PL_U			uRD_COMP_PL0;			// 0x1d4
	RD_COMP_PL_U			uRD_COMP_PL1;			// 0x1d8
	uint					undef_4[1];				// 0x1dc
	COMP_COEFF0_U			uRD_COMP_COEFF0;		// 0x1e0
	COMP_COEFF1_U			uRD_COMP_COEFF1;		// 0x1e4
	COMP_COEFF2_U			uRD_COMP_COEFF2;		// 0x1e8
	COMP_COEFF3_U			uRD_COMP_COEFF3;		// 0x1ec
	COMP_COEFF4_U			uRD_COMP_COEFF4;		// 0x1f0
	COMP_COEFF5_U			uRD_COMP_COEFF5;		// 0x1f4
	uint					undef_4_1[2];			// 0x1f8 ~ 0x1fc

	CD_BASEADDR_U			uCD_BASE0_0;			// 0x200
	CD_BASEADDR_U			uCD_BASE1_0;			// 0x204
	CD_BASEADDR_U			uCD_BASE2_0;			// 0x208
	CD_BASEADDR_U			uCD_BASE0_1;			// 0x20c
	CD_BASEADDR_U			uCD_BASE1_1;			// 0x210
	CD_BASEADDR_U			uCD_BASE2_1;			// 0x214
	CD_BASEADDR_U			uCD_CURR_ADDR;			// 0x218
	CD_STATE_U				uCD_STATE;				// 0x21c	
	CD_CTRL_U				uCD_CTRL;				// 0x220
	uint					undef_5[3];				// 0x224 ~ 0x22c
	COMP_COEFF0_U			uCD_COMP_COEFF0;		// 0x230
	COMP_COEFF1_U			uCD_COMP_COEFF1;		// 0x234
	COMP_COEFF2_U			uCD_COMP_COEFF2;		// 0x238
	COMP_COEFF3_U			uCD_COMP_COEFF3;		// 0x23c
	COMP_COEFF4_U			uCD_COMP_COEFF4;		// 0x240
	COMP_COEFF5_U			uCD_COMP_COEFF5;		// 0x244
	uint					undef_5_1[14];			// 0x248 ~ 0x27c

	OD_BASEADDR_U			uOD_BASE0;				// 0x280
	OD_BASEADDR_U			uOD_BASE1;				// 0x284
	OD_BASEADDR_U			uOD_BASE2;				// 0x288
	OD_SIZE_U				uOD_SIZE;				// 0x28c
	OD_BASEOFS_U			uOD_OFS;				// 0x290
	OD_CFG_U				uOD_CFG;				// 0x294
	OD_BASEADDR_U			uOD_OUTBUFBASE;			// 0x298
	OD_BASEADDR_U			uOD_OUTBUFLIMIT;		// 0x29c
	uint					undef_6;				// 0x2a0
	OD_CONTROL_U			uOD_CONTROL;			// 0x2a4
	uint					uOD_STATE;				// 0x2a8
	uint					undef_7[5];				// 0x2ac ~ 0x2bc
	OD_ROLLVCNT_U			uOD_ROLLVCNT;			// 0x2c0

	uint					undef_7_1[79];			// 0x2c4 ~ 0x3fc

	GM_CTRL_U				uGM_CTRL;				// 0x400
	GM_STATUS_U				uGM_STATUS;				// 0x404
	GM_REGION_0_U			uGM_REGION_0;			// 0x408
	GM_REGION_1_U			uGM_REGION_1;			// 0x40c
	uint					undef_8[3];				// 0x410 ~ 0x418
	GM_INT_U				uGM_INT;				// 0x41c
	uint					undef_8_1[120];			// 0x420 ~ 0x5fc

	HI_CTRL_U				uHI_CTRL;				// 0x600
	HI_STATUS_U				uHI_STATUS;				// 0x604
	HI_CONFIG_U				uHI_CONFIG;				// 0x608
	HI_REGION_0_U			uHI_REGION_0;			// 0x60c
	HI_REGION_1_U			uHI_REGION_1;			// 0x610
	uint					undef_9_1[2];			// 0x614 ~ 0x618
	HI_INT_U				uHI_INT;				// 0x61c
	HI_SEGS					bHI_SEGS;				// 0x620 ~ 0x62c
	HI_CDFS					bHI_CDFS;				// 0x630 ~ 0x63c
	HI_PDFS					bHI_PDFS;				// 0x640 ~ 0x65c
	HI_SCALES				bHI_SCALES;				// 0x660 ~ 0x66c
	uint					undef_10[36];			// 0x670 ~ 0x6fc
	HI_LUTS					bHI_LUTS;				// 0x700 ~ 0x7fc
}	VIPET, *PVIPET;



#endif		// _DEV_VIPET_H_
