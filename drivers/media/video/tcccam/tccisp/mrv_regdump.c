/*****************************************************************************/
/*!
 *  \file        mrv_regdump.c \n
 *  \version     1.0 \n
 *  \author      Vogt \n
 *  \brief       Implementation of some routines to access 
 *               MARVINs registers directly
 *
 *  \revision    $Revision: 432 $ \n
 *               $Author: neugebaa $ \n
 *               $Date: 2009-06-30 11:48:59 +0200 (Di, 30 Jun 2009) $ \n
 *               $Id: mrv_regdump.c 432 2009-06-30 09:48:59Z neugebaa $ \n
 */
/*  This is an unpublished work, the copyright in which vests in Silicon Image
 *  GmbH. The information contained herein is the property of Silicon Image GmbH
 *  and is supplied without liability for errors or omissions. No part may be
 *  reproduced or used expect as authorized by contract or other written
 *  permission. Copyright(c) Silicon Image GmbH, 2009, all rights reserved.
 */
/*****************************************************************************/

#include "os.h"
#include "stdinc.h"

#ifdef MRV_SUPPORT_REGDUMP

#include "reg_access.h"
#include "mrv_priv.h"

// structure for debugging MARVIN status
typedef struct tsMrvDbgStatus_
{
    UINT32 ulIspImsc;
    UINT32 ulMiImsc;
    UINT32 ulJpeErrorImsc;
    UINT32 ulJpeStatusImsc;
} tsMrvDbgStatus;

// (SIG generated) structure for register names (incl. reset values)
typedef struct tsRegDescrReset_
{
    const UINT32 ulOffset;
    const char  *pszName;
} tsRegDescrReset;

#define REGIDX_LAST 0xFFFFFFFF


static tsMrvDbgStatus s_tMrvDbgStatus = {0};

// from SIG generated file(s) 'mrv_all_regs_values_reset.h':
static const tsRegDescrReset g_MrvResetValuesTab[]= {
// offset  reset-mask   reset-value  name
  {0x0000, "vi_ccl"},
  {0x0008,  "vi_id"},
  {0x0010,  "vi_iccl"},
  {0x0014,  "vi_ircl"},
  {0x0018,  "vi_dpcl"},
#if (MARVIN_FEATURE_CLOCK_DOMAINS == MARVIN_CLOCK_DOMAINS_2)
  {0x001C,  "vi_toc"},
#endif  // MARVIN_FEATURE_CLOCK_DOMAINS
#if ((MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V1) || \
     (MARVIN_FEATURE_DMA_READ == MARVIN_FEATURE_DMA_READ_V2))
  {0x0020,  "vi_spmcl"},
#endif  // MARVIN_FEATURE_DMA_READ
#if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_EXIST)
#if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V2)
  {0x0200,  "img_eff_ctrl"},
#else // (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V3)
  {0x0200,  "img_eff_ctrl"},
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V3)
  {0x0204,  "img_eff_color_sel"},
  {0x0208,  "img_eff_mat_1"},
  {0x020C,  "img_eff_mat_2"},
  {0x0210,  "img_eff_mat_3"},
  {0x0214,  "img_eff_mat_4"},
  {0x0218,  "img_eff_mat_5"},
  {0x021C,  "img_eff_tint"},
  {0x0220,  "img_eff_ctrl_shd"},
#if (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V3)
  {0x0224,  "img_eff_sharpen"},
#endif // (MARVIN_FEATURE_IMAGE_EFFECTS >= MARVIN_FEATURE_IE_V3)
#endif  // MARVIN_FEATURE_IMAGE_EFFECTS
#if (MARVIN_FEATURE_SUPERIMPOSE == MARVIN_FEATURE_EXIST)
  {0x0300,  "super_imp_ctrl"},
  {0x0304,  "super_imp_offset_x"},
  {0x0308,  "super_imp_offset_y"},
  {0x030C,  "super_imp_color_y"},
  {0x0310,  "super_imp_color_cb"},
  {0x0314,  "super_imp_color_cr"},
#endif  // MARVIN_FEATURE_SUPERIMPOSE
  {0x0400,  "isp_ctrl"},
  {0x0404,  "isp_acq_prop"},
  {0x0408,  "isp_acq_h_offs"},
  {0x040C,  "isp_acq_v_offs"},
  {0x0410,  "isp_acq_h_size"},
  {0x0414,  "isp_acq_v_size"},
  {0x0418,  "isp_acq_nr_frames"},
  {0x041C,  "isp_gamma_dx_lo"},
  {0x0420,  "isp_gamma_dx_hi"},
  {0x0424,  "isp_gamma_r_y[0]"},
  {0x0428,  "isp_gamma_r_y[1]"},
  {0x042c,  "isp_gamma_r_y[2]"},
  {0x0430,  "isp_gamma_r_y[3]"},
  {0x0434,  "isp_gamma_r_y[4]"},
  {0x0438,  "isp_gamma_r_y[5]"},
  {0x043c,  "isp_gamma_r_y[6]"},
  {0x0440,  "isp_gamma_r_y[7]"},
  {0x0444,  "isp_gamma_r_y[8]"},
  {0x0448,  "isp_gamma_r_y[9]"},
  {0x044c,  "isp_gamma_r_y[10]"},
  {0x0450,  "isp_gamma_r_y[11]"},
  {0x0454,  "isp_gamma_r_y[12]"},
  {0x0458,  "isp_gamma_r_y[13]"},
  {0x045c,  "isp_gamma_r_y[14]"},
  {0x0460,  "isp_gamma_r_y[15]"},
  {0x0464,  "isp_gamma_r_y[16]"},
  {0x0468,  "isp_gamma_g_y[0]"},
  {0x046c,  "isp_gamma_g_y[1]"},
  {0x0470,  "isp_gamma_g_y[2]"},
  {0x0474,  "isp_gamma_g_y[3]"},
  {0x0478,  "isp_gamma_g_y[4]"},
  {0x047c,  "isp_gamma_g_y[5]"},
  {0x0480,  "isp_gamma_g_y[6]"},
  {0x0484,  "isp_gamma_g_y[7]"},
  {0x0488,  "isp_gamma_g_y[8]"},
  {0x048c,  "isp_gamma_g_y[9]"},
  {0x0490,  "isp_gamma_g_y[10]"},
  {0x0494,  "isp_gamma_g_y[11]"},
  {0x0498,  "isp_gamma_g_y[12]"},
  {0x049c,  "isp_gamma_g_y[13]"},
  {0x04a0,  "isp_gamma_g_y[14]"},
  {0x04a4,  "isp_gamma_g_y[15]"},
  {0x04a8,  "isp_gamma_g_y[16]"},
  {0x04ac,  "isp_gamma_b_y[0]"},
  {0x04b0,  "isp_gamma_b_y[1]"},
  {0x04b4,  "isp_gamma_b_y[2]"},
  {0x04b8,  "isp_gamma_b_y[3]"},
  {0x04bc,  "isp_gamma_b_y[4]"},
  {0x04c0,  "isp_gamma_b_y[5]"},
  {0x04c4,  "isp_gamma_b_y[6]"},
  {0x04c8,  "isp_gamma_b_y[7]"},
  {0x04cc,  "isp_gamma_b_y[8]"},
  {0x04d0,  "isp_gamma_b_y[9]"},
  {0x04d4,  "isp_gamma_b_y[10]"},
  {0x04d8,  "isp_gamma_b_y[11]"},
  {0x04dc,  "isp_gamma_b_y[12]"},
  {0x04e0,  "isp_gamma_b_y[13]"},
  {0x04e4,  "isp_gamma_b_y[14]"},
  {0x04e8,  "isp_gamma_b_y[15]"},
  {0x04ec,  "isp_gamma_b_y[16]"},
#if ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V1) && \
     (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V3))
  {0x04f0,  "isp_bls_prop"},
  {0x04f4,  "isp_bls_h_window"},
  {0x04f8,  "isp_bls_v_window"},
  {0x04fc,  "isp_bls_samples"},
  {0x0500,  "isp_bls_ref_value_ab"},
  {0x0504,  "isp_bls_ref_value_cd"},
  {0x0508,  "isp_bls_mean_value_ab"},
  {0x050c,  "isp_bls_mean_value_cd"},
#endif  // MARVIN_FEATURE_BLACK_LEVEL
#if (MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V5)
  {0x0510,  "isp_awb_prop"},
#else // (MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V5)
  {0x0510,  "isp_awb_prop"},
#endif // (MARVIN_FEATURE_WB == MARVIN_FEATURE_WB_V5)
  {0x0514,  "isp_awb_h_offs"},
  {0x0518,  "isp_awb_v_offs"},
  {0x051C,  "isp_awb_h_size"},
  {0x0520,  "isp_awb_v_size"},
  {0x0524,  "isp_awb_frames"},
  {0x0528,  "isp_awb_ref"},
  {0x052C,  "isp_awb_thresh"},
  {0x0538,  "isp_awb_gain_g"},
  {0x053C,  "isp_awb_gain_rb"},
  {0x0540,  "isp_awb_white_cnt"},
  {0x0544,  "isp_awb_mean"},
#if (MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V1)
  {0x0548,  "isp_ae_h_offs"},
  {0x054C,  "isp_ae_v_offs"},
  {0x0550,  "isp_ae_h_size"},
  {0x0554,  "isp_ae_v_size"},
  {0x0558,  "isp_ae_frames"},
  {0x055C,  "isp_ae_hist_range"},
  {0x0560,  "isp_ae_occ_y"},
  {0x0564,  "isp_ae_mean_y"},
  {0x0568,  "isp_ae_max_y"},
  {0x056C,  "isp_ae_min_y"},
#endif  // MARVIN_FEATURE_AUTO_EXPOSURE
  {0x0570,  "isp_cc_coeff_0"},
  {0x0574,  "isp_cc_coeff_1"},
  {0x0578,  "isp_cc_coeff_2"},
  {0x057C,  "isp_cc_coeff_3"},
  {0x0580,  "isp_cc_coeff_4"},
  {0x0584,  "isp_cc_coeff_5"},
  {0x0588,  "isp_cc_coeff_6"},
  {0x058C,  "isp_cc_coeff_7"},
  {0x0590,  "isp_cc_coeff_8"},
#if ((MARVIN_FEATURE_OUTPUT_FORMATTER >= MARVIN_FEATURE_OUTPUT_FORMATTER_V1) && \
     (MARVIN_FEATURE_OUTPUT_FORMATTER <= MARVIN_FEATURE_OUTPUT_FORMATTER_V2))
  {0x0594,  "isp_out_h_offs"},
  {0x0598,  "isp_out_v_offs"},
  {0x059C,  "isp_out_h_size"},
  {0x05A0,  "isp_out_v_size"},
#endif  // MARVIN_FEATURE_OUTPUT_FORMATTER
  {0x05A4,  "isp_demosaic"},
  {0x05A8,  "isp_flags_shd"},
#if ((MARVIN_FEATURE_OUTPUT_FORMATTER >= MARVIN_FEATURE_OUTPUT_FORMATTER_V1) && \
     (MARVIN_FEATURE_OUTPUT_FORMATTER <= MARVIN_FEATURE_OUTPUT_FORMATTER_V2))
  {0x05AC,  "isp_out_h_offs_shd"},
  {0x05B0,  "isp_out_v_offs_shd"},
  {0x05B4,  "isp_out_h_size_shd"},
  {0x05B8,  "isp_out_v_size_shd"},
#endif  // MARVIN_FEATURE_OUTPUT_FORMATTER
  {0x05BC,  "isp_imsc"},
  {0x05C0,  "isp_ris"},
  {0x05C4,  "isp_mis"},
  {0x05C8,  "isp_icr"},
  {0x05CC,  "isp_isr"},
#if ((MARVIN_FEATURE_XTALK >= MARVIN_FEATURE_XTALK_10BITS) && \
     (MARVIN_FEATURE_XTALK <= MARVIN_FEATURE_XTALK_11BITS))
  {0x05D0,  "isp_ct_coeff[0]"},
  {0x05D4,  "isp_ct_coeff[1]"},
  {0x05D8,  "isp_ct_coeff[2]"},
  {0x05DC,  "isp_ct_coeff[3]"},
  {0x05E0,  "isp_ct_coeff[4]"},
  {0x05E4,  "isp_ct_coeff[5]"},
  {0x05E8,  "isp_ct_coeff[6]"},
  {0x05EC,  "isp_ct_coeff[7]"},
  {0x05F0,  "isp_ct_coeff[8]"},
#endif  // MARVIN_FEATURE_XTALK
#if (MARVIN_FEATURE_GAMMAOUT == MARVIN_FEATURE_EXIST)
  {0x05F4,  "isp_gamma_out_mode"},
  {0x05F8,  "isp_gamma_out_y[0]"},
  {0x05FC,  "isp_gamma_out_y[1]"},
  {0x0600,  "isp_gamma_out_y[2]"},
  {0x0604,  "isp_gamma_out_y[3]"},
  {0x0608,  "isp_gamma_out_y[4]"},
  {0x060C,  "isp_gamma_out_y[5]"},
  {0x0610,  "isp_gamma_out_y[6]"},
  {0x0614,  "isp_gamma_out_y[7]"},
  {0x0618,  "isp_gamma_out_y[8]"},
  {0x061C,  "isp_gamma_out_y[9]"},
  {0x0620,  "isp_gamma_out_y[10]"},
  {0x0624,  "isp_gamma_out_y[11]"},
  {0x0628,  "isp_gamma_out_y[12]"},
  {0x062C,  "isp_gamma_out_y[13]"},
  {0x0630,  "isp_gamma_out_y[14]"},
  {0x0634,  "isp_gamma_out_y[15]"},
  {0x0638,  "isp_gamma_out_y[16]"},
#endif  // MARVIN_FEATURE_GAMMAOUT
#if (MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)
  {0x063C,  "isp_err"},
  {0x0640,  "isp_err_clr"},
#endif  // MARVIN_FEATURE_ISP_ERRORFLAGS
#if (MARVIN_FEATURE_FRAMECOUNTER == MARVIN_FEATURE_EXIST)
  {0x0644,  "isp_frame_count"},
#endif  // MARVIN_FEATURE_FRAMECOUNTER
#if (MARVIN_FEATURE_XTALK_OFFSET == MARVIN_FEATURE_EXIST)
  {0x0648,  "isp_ct_offset_r"},
  {0x064C,  "isp_ct_offset_g"},
  {0x0650,  "isp_ct_offset_b"},
#endif  // MARVIN_FEATURE_XTALK_OFFSET
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
  {0x0660,  "isp_flash_cmd"},
  {0x0664,  "isp_flash_config"},
  {0x0668,  "isp_flash_prediv"},
  {0x066C,  "isp_flash_delay"},
  {0x0670,  "isp_flash_time"},
  {0x0674,  "isp_flash_maxp"},
#endif  // MARVIN_FEATURE_FLASH_LIGHT
#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
  {0x0680,  "isp_sh_ctrl"},
  {0x0684,  "isp_sh_prediv"},
  {0x0688,  "isp_sh_delay"},
  {0x068C,  "isp_sh_time"},
#endif  // MARVIN_FEATURE_SHUTTER
  {0x0800,  "c_proc_ctrl"},
  {0x0804,  "c_proc_contrast"},
  {0x0808,  "c_proc_brightness"},
  {0x080C,  "c_proc_saturation"},
  {0x0810,  "c_proc_hue"},
#if (MARVIN_FEATURE_MSCALE_UP == MARVIN_FEATURE_EXIST)
  {0x0C00,  "mrsz_ctrl"},
  {0x0C04,  "mrsz_scale_hy"},
  {0x0C08,  "mrsz_scale_hcb"},
  {0x0C0C,  "mrsz_scale_hcr"},
  {0x0C10,  "mrsz_scale_vy"},
  {0x0C14,  "mrsz_scale_vc"},
  {0x0C18,  "mrsz_phase_hy"},
  {0x0C1C,  "mrsz_phase_hc"},
  {0x0C20,  "mrsz_phase_vy"},
  {0x0C24,  "mrsz_phase_vc"},
  {0x0C28,  "mrsz_scale_lut_addr"},
  {0x0C2C,  "mrsz_scale_lut"},
  {0x0C30,  "mrsz_ctrl_shd"},
  {0x0C34,  "mrsz_scale_hy_shd"},
  {0x0C38,  "mrsz_scale_hcb_shd"},
  {0x0C3C,  "mrsz_scale_hcr_shd"},
  {0x0C40,  "mrsz_scale_vy_shd"},
  {0x0C44,  "mrsz_scale_vc_shd"},
  {0x0C48,  "mrsz_phase_hy_shd"},
  {0x0C4C,  "mrsz_phase_hc_shd"},
  {0x0C50,  "mrsz_phase_vy_shd"},
  {0x0C54,  "mrsz_phase_vc_shd"},
#else  // MARVIN_FEATURE_MSCALE_UP
  {0x0C00,  "mrsz_ctrl"},
  {0x0C04,  "mrsz_scale_hy"},
  {0x0C08,  "mrsz_scale_hc"},
  {0x0C0C,  "mrsz_scale_vy"},
  {0x0C10,  "mrsz_scale_vc"},
  {0x0C14,  "mrsz_phase_hy"},
  {0x0C18,  "mrsz_phase_hc"},
  {0x0C1C,  "mrsz_phase_vy"},
  {0x0C20,  "mrsz_phase_vc"},
  {0x0C24,  "mrsz_ctrl_shd"},
  {0x0C28,  "mrsz_scale_hy_shd"},
  {0x0C2C,  "mrsz_scale_hc_shd"},
  {0x0C30,  "mrsz_scale_vy_shd"},
  {0x0C34,  "mrsz_scale_vc_shd"},
  {0x0C38,  "mrsz_phase_hy_shd"},
  {0x0C3C,  "mrsz_phase_hc_shd"},
  {0x0C40,  "mrsz_phase_vy_shd"},
  {0x0C44,  "mrsz_phase_vc_shd"},
#endif  // MARVIN_FEATURE_MSCALE_UP
#if (MARVIN_FEATURE_SSCALE == MARVIN_FEATURE_EXIST)
  #if (MARVIN_FEATURE_SSCALE_UP == MARVIN_FEATURE_EXIST)
  {0x1000,  "srsz_ctrl"},
  {0x1004,  "srsz_scale_hy"},
  {0x1008,  "srsz_scale_hcb"},
  {0x100C,  "srsz_scale_hcr"},
  {0x1010,  "srsz_scale_vy"},
  {0x1014,  "srsz_scale_vc"},
  {0x1018,  "srsz_phase_hy"},
  {0x101C,  "srsz_phase_hc"},
  {0x1020,  "srsz_phase_vy"},
  {0x1024,  "srsz_phase_vc"},
  {0x1028,  "srsz_scale_lut_addr"},
  {0x102C,  "srsz_scale_lut"},
  {0x1030,  "srsz_ctrl_shd"},
  {0x1034,  "srsz_scale_hy_shd"},
  {0x1038,  "srsz_scale_hcb_shd"},
  {0x103C,  "srsz_scale_hcr_shd"},
  {0x1040,  "srsz_scale_vy_shd"},
  {0x1044,  "srsz_scale_vc_shd"},
  {0x1048,  "srsz_phase_hy_shd"},
  {0x104C,  "srsz_phase_hc_shd"},
  {0x1050,  "srsz_phase_vy_shd"},
  {0x1054,  "srsz_phase_vc_shd"},
  #else   // MARVIN_FEATURE_SSCALE_UP
  {0x1000,  "srsz_ctrl"},
  {0x1004,  "srsz_scale_hy"},
  {0x1008,  "srsz_scale_hc"},
  {0x100C,  "srsz_scale_vy"},
  {0x1010,  "srsz_scale_vc"},
  {0x1014,  "srsz_phase_hy"},
  {0x1018,  "srsz_phase_hc"},
  {0x101C,  "srsz_phase_vy"},
  {0x1020,  "srsz_phase_vc"},
  {0x1024,  "srsz_ctrl_shd"},
  {0x1028,  "srsz_scale_hy_shd"},
  {0x102C,  "srsz_scale_hc_shd"},
  {0x1030,  "srsz_scale_vy_shd"},
  {0x1034,  "srsz_scale_vc_shd"},
  {0x1038,  "srsz_phase_hy_shd"},
  {0x103C,  "srsz_phase_hc_shd"},
  {0x1040,  "srsz_phase_vy_shd"},
  {0x1044,  "srsz_phase_vc_shd"},
  #endif  // MARVIN_FEATURE_SSCALE_UP
#endif // MARVIN_FEATURE_SSCALE
#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
  {0x1400,  "emp_out_ctrl"},
  {0x14C8,  "emp_out_imsc"},
  {0x14CC,  "emp_out_ris"},
  {0x14D0,  "emp_out_mis"},
  {0x14D4,  "emp_out_icr"},
  {0x14D8,  "emp_out_isr"},
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V1)
  {0x1600,  "mi_ctrl"},
  {0x1604,  "mi_init"},
  {0x1608,  "mi_mp_y_base_ad_init"},
  {0x160C,  "mi_mp_y_size_init"},
  {0x1610,  "mi_mp_y_offs_cnt_init"},
  {0x1614,  "mi_mp_y_offs_cnt_start"},
  {0x1618,  "mi_mp_y_irq_offs_init"},
  {0x161C,  "mi_mp_cb_base_ad_init"},
  {0x1620,  "mi_mp_cb_size_init"},
  {0x1624,  "mi_mp_cb_offs_cnt_init"},
  {0x1628,  "mi_mp_cb_offs_cnt_start"},
  {0x162C,  "mi_mp_cr_base_ad_init"},
  {0x1630,  "mi_mp_cr_size_init"},
  {0x1634,  "mi_mp_cr_offs_cnt_init"},
  {0x1638,  "mi_mp_cr_offs_cnt_start"},
  {0x163C,  "mi_sp_y_base_ad_init"},
  {0x1640,  "mi_sp_y_size_init"},
  {0x1644,  "mi_sp_y_offs_cnt_init"},
  {0x1648,  "mi_sp_y_offs_cnt_start"},
  {0x164C,  "mi_sp_y_llength"},
  {0x1650,  "mi_sp_cb_base_ad_init"},
  {0x1654,  "mi_sp_cb_size_init"},
  {0x1658,  "mi_sp_cb_offs_cnt_init"},
  {0x165C,  "mi_sp_cb_offs_cnt_start"},
  {0x1660,  "mi_sp_cr_base_ad_init"},
  {0x1664,  "mi_sp_cr_size_init"},
  {0x1668,  "mi_sp_cr_offs_cnt_init"},
  {0x166C,  "mi_sp_cr_offs_cnt_start"},
  {0x1670,  "mi_byte_cnt"},
  {0x1674,  "mi_ctrl_shd"},
  {0x1678,  "mi_mp_y_base_ad_shd"},
  {0x167C,  "mi_mp_y_size_shd"},
  {0x1680,  "mi_mp_y_offs_cnt_shd"},
  {0x1684,  "mi_mp_y_irq_offs_shd"},
  {0x1688,  "mi_mp_cb_base_ad_shd"},
  {0x168C,  "mi_mp_cb_size_shd"},
  {0x1690,  "mi_mp_cb_offs_cnt_shd"},
  {0x1694,  "mi_mp_cr_base_ad_shd"},
  {0x1698,  "mi_mp_cr_size_shd"},
  {0x169C,  "mi_mp_cr_offs_cnt_shd"},
  {0x16A0,  "mi_sp_y_base_ad_shd"},
  {0x16A4,  "mi_sp_y_size_shd"},
  {0x16A8,  "mi_sp_y_offs_cnt_shd"},
  {0x16B0,  "mi_sp_cb_base_ad_shd"},
  {0x16B4,  "mi_sp_cb_size_shd"},
  {0x16B8,  "mi_sp_cb_offs_cnt_shd"},
  {0x16BC,  "mi_sp_cr_base_ad_shd"},
  {0x16C0,  "mi_sp_cr_size_shd"},
  {0x16C4,  "mi_sp_cr_offs_cnt_shd"},
  {0x16C8,  "mi_imsc"},
  {0x16CC,  "mi_ris"},
  {0x16D0,  "mi_mis"},
  {0x16D4,  "mi_icr"},
  {0x16D8,  "mi_isr"},
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
  {0x1600,  "mi_ctrl"},
  {0x1604,  "mi_init"},
  {0x1608,  "mi_mp_y_base_ad_init"},
  {0x160C,  "mi_mp_y_size_init"},
  {0x1610,  "mi_mp_y_offs_cnt_init"},
  {0x1614,  "mi_mp_y_offs_cnt_start"},
  {0x1618,  "mi_mp_y_irq_offs_init"},
  {0x161C,  "mi_mp_cb_base_ad_init"},
  {0x1620,  "mi_mp_cb_size_init"},
  {0x1624,  "mi_mp_cb_offs_cnt_init"},
  {0x1628,  "mi_mp_cb_offs_cnt_start"},
  {0x162C,  "mi_mp_cr_base_ad_init"},
  {0x1630,  "mi_mp_cr_size_init"},
  {0x1634,  "mi_mp_cr_offs_cnt_init"},
  {0x1638,  "mi_mp_cr_offs_cnt_start"},
  {0x163C,  "mi_sp_y_base_ad_init"},
  {0x1640,  "mi_sp_y_size_init"},
  {0x1644,  "mi_sp_y_offs_cnt_init"},
  {0x1648,  "mi_sp_y_offs_cnt_start"},
  {0x164C,  "mi_sp_y_llength"},
  {0x1650,  "mi_sp_cb_base_ad_init"},
  {0x1654,  "mi_sp_cb_size_init"},
  {0x1658,  "mi_sp_cb_offs_cnt_init"},
  {0x165C,  "mi_sp_cb_offs_cnt_start"},
  {0x1660,  "mi_sp_cr_base_ad_init"},
  {0x1664,  "mi_sp_cr_size_init"},
  {0x1668,  "mi_sp_cr_offs_cnt_init"},
  {0x166C,  "mi_sp_cr_offs_cnt_start"},
  {0x1670,  "mi_byte_cnt"},
  {0x1674,  "mi_ctrl_shd"},
  {0x1678,  "mi_mp_y_base_ad_shd"},
  {0x167C,  "mi_mp_y_size_shd"},
  {0x1680,  "mi_mp_y_offs_cnt_shd"},
  {0x1684,  "mi_mp_y_irq_offs_shd"},
  {0x1688,  "mi_mp_cb_base_ad_shd"},
  {0x168C,  "mi_mp_cb_size_shd"},
  {0x1690,  "mi_mp_cb_offs_cnt_shd"},
  {0x1694,  "mi_mp_cr_base_ad_shd"},
  {0x1698,  "mi_mp_cr_size_shd"},
  {0x169C,  "mi_mp_cr_offs_cnt_shd"},
  {0x16A0,  "mi_sp_y_base_ad_shd"},
  {0x16A4,  "mi_sp_y_size_shd"},
  {0x16A8,  "mi_sp_y_offs_cnt_shd"},
  {0x16B0,  "mi_sp_cb_base_ad_shd"},
  {0x16B4,  "mi_sp_cb_size_shd"},
  {0x16B8,  "mi_sp_cb_offs_cnt_shd"},
  {0x16BC,  "mi_sp_cr_base_ad_shd"},
  {0x16C0,  "mi_sp_cr_size_shd"},
  {0x16C4,  "mi_sp_cr_offs_cnt_shd"},
  {0x16C8,  "mi_dma_y_pic_start_ad"},
  {0x16CC,  "mi_dma_y_pic_width"},
  {0x16D0,  "mi_dma_y_llength"},
  {0x16D4,  "mi_dma_y_pic_size"},
  {0x16D8,  "mi_dma_cb_pic_start_ad"},
  {0x16E8,  "mi_dma_cr_pic_start_ad"},
  {0x16F8,  "mi_imsc"},
  {0x16FC,  "mi_ris"},
  {0x1700,  "mi_mis"},
  {0x1704,  "mi_icr"},
  {0x1708,  "mi_isr"},
  #if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
  {0x170C,  "mi_status"},
  {0x1710,  "mi_status_clr"},
  #endif  // MARVIN_FEATURE_MI_STATUSFLAGS
  {0x1714,  "mi_sp_y_pic_width"},
  {0x1718,  "mi_sp_y_pic_height"},
  {0x171C,  "mi_sp_y_pic_size"},
  {0x1720,  "mi_dma_ctrl"},
  {0x1724,  "mi_dma_start"},
  {0x1728,  "mi_dma_status"},
#endif //MARVIN_FEATURE_MI V4
#else  // MARVIN_FEATURE_SMALL_OUTUNIT
#if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
  {0x1400,  "mi_ctrl"},
#else // (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
  {0x1400,  "mi_ctrl"},
#endif // (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V4)
  {0x1404,  "mi_init"},
  {0x1408,  "mi_mp_y_base_ad_init"},
  {0x140C,  "mi_mp_y_size_init"},
  {0x1410,  "mi_mp_y_offs_cnt_init"},
  {0x1414,  "mi_mp_y_offs_cnt_start"},
  {0x1418,  "mi_mp_y_irq_offs_init"},
  {0x141C,  "mi_mp_cb_base_ad_init"},
  {0x1420,  "mi_mp_cb_size_init"},
  {0x1424,  "mi_mp_cb_offs_cnt_init"},
  {0x1428,  "mi_mp_cb_offs_cnt_start"},
  {0x142C,  "mi_mp_cr_base_ad_init"},
  {0x1430,  "mi_mp_cr_size_init"},
  {0x1434,  "mi_mp_cr_offs_cnt_init"},
  {0x1438,  "mi_mp_cr_offs_cnt_start"},
  {0x143C,  "mi_sp_y_base_ad_init"},
  {0x1440,  "mi_sp_y_size_init"},
  {0x1444,  "mi_sp_y_offs_cnt_init"},
  {0x1448,  "mi_sp_y_offs_cnt_start"},
  {0x144C,  "mi_sp_y_llength"},
  {0x1450,  "mi_sp_cb_base_ad_init"},
  {0x1454,  "mi_sp_cb_size_init"},
  {0x1458,  "mi_sp_cb_offs_cnt_init"},
  {0x145C,  "mi_sp_cb_offs_cnt_start"},
  {0x1460,  "mi_sp_cr_base_ad_init"},
  {0x1464,  "mi_sp_cr_size_init"},
  {0x1468,  "mi_sp_cr_offs_cnt_init"},
  {0x146C,  "mi_sp_cr_offs_cnt_start"},
  {0x1470,  "mi_byte_cnt"},
  {0x1474,  "mi_ctrl_shd"},
  {0x1478,  "mi_mp_y_base_ad_shd"},
  {0x147C,  "mi_mp_y_size_shd"},
  {0x1480,  "mi_mp_y_offs_cnt_shd"},
  {0x1484,  "mi_mp_y_irq_offs_shd"},
  {0x1488,  "mi_mp_cb_base_ad_shd"},
  {0x148C,  "mi_mp_cb_size_shd"},
  {0x1490,  "mi_mp_cb_offs_cnt_shd"},
  {0x1494,  "mi_mp_cr_base_ad_shd"},
  {0x1498,  "mi_mp_cr_size_shd"},
  {0x149C,  "mi_mp_cr_offs_cnt_shd"},
  {0x14A0,  "mi_sp_y_base_ad_shd"},
  {0x14A4,  "mi_sp_y_size_shd"},
  {0x14A8,  "mi_sp_y_offs_cnt_shd"},
  #if (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V3) //older versions of MI
  {0x144C,  "mi_sp_llength_shd"},
  #endif  // MARVIN_FEATURE_MI
  {0x14B0,  "mi_sp_cb_base_ad_shd"},
  {0x14B4,  "mi_sp_cb_size_shd"},
  {0x14B8,  "mi_sp_cb_offs_cnt_shd"},
  {0x14BC,  "mi_sp_cr_base_ad_shd"},
  {0x14C0,  "mi_sp_cr_size_shd"},
  {0x14C4,  "mi_sp_cr_offs_cnt_shd"},
  #if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2) && \
       (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
  {0x14C8,  "mi_dma_y_pic_start_ad"},
  {0x14CC,  "mi_dma_y_pic_width"},
  {0x14D0,  "mi_dma_y_llength"},
  {0x14D4,  "mi_dma_y_pic_size"},
  {0x14D8,  "mi_dma_cb_pic_start_ad"},
    #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
  {0x14DC,  "mi_dma_cb_pic_width"},
  {0x14E0,  "mi_dma_cb_llength"},
  {0x14E4,  "mi_dma_cb_pic_size"},
    #endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
  {0x14E8,  "mi_dma_cr_pic_start_ad"},
    #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
  {0x14EC,  "mi_dma_cr_pic_width"},
  {0x14F0,  "mi_dma_cr_llength"},
  {0x14F4,  "mi_dma_cr_pic_size"},
    #endif  // #if (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
  {0x14F8,  "mi_imsc"},
  {0x14FC,  "mi_ris"},
  {0x1500,  "mi_mis"},
  {0x1504,  "mi_icr"},
  {0x1508,  "mi_isr"},
  #else   // #if (MARVIN_FEATURE_MI >= MI_V2 && <= MI_V4)
  {0x14C8,  "mi_imsc"},
  {0x14CC,  "mi_ris"},
  {0x14D0,  "mi_mis"},
  {0x14D4,  "mi_icr"},
  {0x14D8,  "mi_isr"},
  #endif  // #if (MARVIN_FEATURE_MI >= MI_V2 && <= MI_V4)
  #if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
  {0x150C,  "mi_status"},
  {0x1510,  "mi_status_clr"},
  #endif  // MARVIN_FEATURE_MI_STATUSFLAGS
  #if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
  {0x1514,  "mi_sp_y_pic_width"},
  {0x1518,  "mi_sp_y_pic_height"},
  {0x151C,  "mi_sp_y_pic_size"},
  {0x1520,  "mi_dma_ctrl"},
  {0x1524,  "mi_dma_start"},
  {0x1528,  "mi_dma_status"},
  {0x152C,  "mi_pixel_cnt"},
  #endif  // #if (MARVIN_FEATURE_MI >= MI_V3 && <= MI_V4)
#endif  // MARVIN_FEATURE_SMALL_OUTUNIT
  {0x1800,  "jpe_gen_header"},
#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V1))
  {0x1804,  "jpe_encode"},
#else  
  {0x1804,  "jpe_encode"},
#endif
#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V2) && \
     (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3))
  {0x1808,  "jpe_init"},
#endif // MARVIN_FEATURE_JPE
  {0x180C,  "jpe_y_scale_en"},
  {0x1810,  "jpe_cbcr_scale_en"},
  {0x1814,  "jpe_table_flush"},
  {0x1818,  "jpe_enc_hsize"},
  {0x181C,  "jpe_enc_vsize"},
  {0x1820,  "jpe_pic_format"},
  {0x1824,  "jpe_restart_interval"},
  {0x1828,  "jpe_tq_y_select"},
  {0x182C,  "jpe_tq_u_select"},
  {0x1830,  "jpe_tq_v_select"},
  {0x1834,  "jpe_dc_table_select"},
  {0x1838,  "jpe_ac_table_select"},
  {0x183C,  "jpe_table_data"},
  {0x1840,  "jpe_table_id"},
  {0x1844,  "jpe_tac0_len"},
  {0x1848,  "jpe_tdc0_len"},
  {0x184C,  "jpe_tac1_len"},
  {0x1850,  "jpe_tdc1_len"},
  {0x1858,  "jpe_encoder_busy"},
  {0x185C,  "jpe_header_mode"},
  {0x1860,  "jpe_encode_mode"},
  {0x1864,  "jpe_debug"},
  {0x1868,  "jpe_error_imr"},
  {0x186C,  "jpe_error_ris"},
  {0x1870,  "jpe_error_mis"},
  {0x1874,  "jpe_error_icr"},
  {0x1878,  "jpe_error_isr"},
  {0x187C,  "jpe_status_imr"},
  {0x1880,  "jpe_status_ris"},
  {0x1884,  "jpe_status_mis"},
  {0x1888,  "jpe_status_icr"},
  {0x188C,  "jpe_status_isr"},
#if ((MARVIN_FEATURE_JPE >= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE <= MARVIN_FEATURE_JPE_V3) && \
     (MARVIN_FEATURE_JPE_CFG == MARVIN_FEATURE_JPE_CFG_V2))
  {0x1890,  "jpe_config"},
#endif
#if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP)      || \
     (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
  {0x1A00,  "smia_ctrl"},
  {0x1A04,  "smia_status"},
  {0x1A08,  "smia_imsc"},
  {0x1A0C,  "smia_ris"},
  {0x1A10,  "smia_mis"},
  {0x1A14,  "smia_icr"},
  {0x1A18,  "smia_isr"},
  {0x1A1C,  "smia_data_format_sel"},
  {0x1A20,  "smia_sof_emb_data_lines"},
  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
       (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_EMP))
  {0x1A24,  "smia_emb_hstart"},
  {0x1A28,  "smia_emb_hsize"},
  {0x1A2C,  "smia_emb_vstart"},
  #endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP))
  {0x1A30,  "smia_num_lines"},
  {0x1A34,  "smia_emb_data_fifo"},
  #if ((MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_COMPLETE) || \
       (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP))
  {0x1A38,  "smia_fifo_fill_level"},
  #endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_MIPI_EMP))
  #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
  {0x1A40,  "smia_emb_hstart1"},
  {0x1A44,  "smia_emb_hsize1"},
  {0x1A48,  "smia_emb_vstart1"},
  {0x1A4C,  "smia_emb_vsize1"},
  {0x1A50,  "smia_emb_hstart2"},
  {0x1A54,  "smia_emb_hsize2"},
  {0x1A58,  "smia_emb_vstart2"},
  {0x1A5C,  "smia_emb_vsize2"},
  #endif // #if (MARVIN_FEATURE_SMIA == MARVIN_FEATURE_SMIA_MIPI_EMP)
#endif // #if ((MARVIN_FEATURE_SMIA_COMPLETE || MARVIN_FEATURE_SMIA_EMP || MARVIN_FEATURE_SMIA_MIPI_EMP))
#if ( (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V1) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V2) || \
        (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V3) || (MARVIN_FEATURE_MIPI == MARVIN_FEATURE_MIPI_V4))
  {0x1C00,  "mipi_ctrl"},
  {0x1C04,  "mipi_status"},
  {0x1C08,  "mipi_imsc"},
  {0x1C0C,  "mipi_ris"},
  {0x1C10,  "mipi_mis"},
  {0x1C14,  "mipi_icr"},
  {0x1C18,  "mipi_isr"},
  {0x1C1C,  "mipi_cur_data_id"},
  {0x1C20,  "mipi_img_data_sel"},
  {0x1C24,  "mipi_add_data_sel_1"},
  {0x1C28,  "mipi_add_data_sel_2"},
  {0x1C2C,  "mipi_add_data_sel_3"},
  {0x1C30,  "mipi_add_data_sel_4"},
  {0x1C34,  "mipi_add_data_fifo"},
  {0x1C38,  "mipi_add_data_fill_level"},
#endif
#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
  {0x2000,  "isp_afm_ctrl"},
  {0x2004,  "isp_afm_lt_a"},
  {0x2008,  "isp_afm_rb_a"},
  {0x200C,  "isp_afm_lt_b"},
  {0x2010,  "isp_afm_rb_b"},
  {0x2014,  "isp_afm_lt_c"},
  {0x2018,  "isp_afm_rb_c"},
  {0x201C,  "isp_afm_thres"},
  {0x2020,  "isp_afm_var_shift"},
  {0x2024,  "isp_afm_sum_a"},
  {0x2028,  "isp_afm_sum_b"},
  {0x202C,  "isp_afm_sum_c"},
  {0x2030,  "isp_afm_lum_a"},
  {0x2034,  "isp_afm_lum_b"},
  {0x2038,  "isp_afm_lum_c"},
#endif  // MARVIN_FEATURE_AF_MEASURE
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
  {0x2100,  "isp_bp_ctrl"},
  {0x2104,  "isp_bp_cfg1"},
  {0x2108,  "isp_bp_cfg2"},
  {0x210C,  "isp_bp_number"},
  {0x2110,  "isp_bp_table_addr"},
  {0x2114,  "isp_bp_table_data"},
  {0x2118,  "isp_bp_new_number"},
  {0x211c,  "isp_bp_new_table[0]"},
  {0x2120,  "isp_bp_new_table[1]"},
  {0x2124,  "isp_bp_new_table[2]"},
  {0x2128,  "isp_bp_new_table[3]"},
  {0x212c,  "isp_bp_new_table[4]"},
  {0x2130,  "isp_bp_new_table[5]"},
  {0x2134,  "isp_bp_new_table[6]"},
  {0x2138,  "isp_bp_new_table[7]"},
#endif  // MARVIN_FEATURE_BAD_PIXEL
#if (MARVIN_FEATURE_LENS_SHADING >= MARVIN_FEATURE_LSC_V1)
  {0x2200,  "isp_lsc_ctrl"},
  {0x2204,  "isp_lsc_r_table_addr"},
  {0x2208,  "isp_lsc_g_table_addr"},
  {0x220C,  "isp_lsc_b_table_addr"},
  {0x2210,  "isp_lsc_r_table_data"},
  {0x2214,  "isp_lsc_g_table_data"},
  {0x2218,  "isp_lsc_b_table_data"},
  {0x2220,  "isp_lsc_xgrad_01"},
  {0x2224,  "isp_lsc_xgrad_23"},
  {0x2228,  "isp_lsc_xgrad_45"},
  {0x222C,  "isp_lsc_xgrad_67"},
  {0x2230,  "isp_lsc_ygrad_01"},
  {0x2234,  "isp_lsc_ygrad_23"},
  {0x2238,  "isp_lsc_ygrad_45"},
  {0x223C,  "isp_lsc_ygrad_67"},
  {0x2240,  "isp_lsc_xsize_01"},
  {0x2244,  "isp_lsc_xsize_23"},
  {0x2248,  "isp_lsc_xsize_45"},
  {0x224C,  "isp_lsc_xsize_67"},
  {0x2250,  "isp_lsc_ysize_01"},
  {0x2254,  "isp_lsc_ysize_23"},
  {0x2258,  "isp_lsc_ysize_45"},
  {0x225C,  "isp_lsc_ysize_67"},
#endif  // MARVIN_FEATURE_LENS_SHADING
#if ((MARVIN_FEATURE_IMG_STABILIZATION >= MARVIN_FEATURE_IMG_STABILIZATION_V1) || \
     (MARVIN_FEATURE_IMG_STABILIZATION <= MARVIN_FEATURE_IMG_STABILIZATION_V2))
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
  {0x2300,  "isp_is_ctrl"},
  {0x2304,  "isp_is_recenter"},
  #endif  // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
  {0x2308,  "isp_is_h_offs"},
  {0x230C,  "isp_is_v_offs"},
  {0x2310,  "isp_is_h_size"},
  {0x2314,  "isp_is_v_size"},
  #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
  {0x2318,  "isp_is_max_dx"},
  {0x231C,  "isp_is_max_dy"},
  #endif  // #if (MARVIN_FEATURE_IMG_STABILIZATION == MARVIN_FEATURE_IMG_STABILIZATION_V2)
  {0x2320,  "isp_is_displace"},
  {0x2324,  "isp_is_h_offs_shd"},
  {0x2328,  "isp_is_v_offs_shd"},
  {0x232C,  "isp_is_h_size_shd"},
  {0x2330,  "isp_is_v_size_shd"},
#endif  // MARVIN_FEATURE_IMG_STABILIZATION
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V1)
  {0x2400,  "isp_hist_prop"},
  {0x2404,  "isp_hist_h_offs"},
  {0x2408,  "isp_hist_v_offs"},
  {0x240C,  "isp_hist_h_size"},
  {0x2410,  "isp_hist_v_size"},
  {0x2414,  "isp_hist_bin[0]"},
  {0x2418,  "isp_hist_bin[1]"},
  {0x241c,  "isp_hist_bin[2]"},
  {0x2420,  "isp_hist_bin[3]"},
  {0x2424,  "isp_hist_bin[4]"},
  {0x2428,  "isp_hist_bin[5]"},
  {0x242c,  "isp_hist_bin[6]"},
  {0x2430,  "isp_hist_bin[7]"},
  {0x2434,  "isp_hist_bin[8]"},
  {0x2438,  "isp_hist_bin[9]"},
  {0x243c,  "isp_hist_bin[10]"},
  {0x2440,  "isp_hist_bin[11]"},
  {0x2444,  "isp_hist_bin[12]"},
  {0x2448,  "isp_hist_bin[13]"},
  {0x244c,  "isp_hist_bin[14]"},
  {0x2450,  "isp_hist_bin[15]"},
#endif  // MARVIN_FEATURE_ISP_HISTOGRAM
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V2)
  {0x2400,  "isp_hist_prop"},
  {0x2404,  "isp_hist_h_offs"},
  {0x2408,  "isp_hist_v_offs"},
  {0x240C,  "isp_hist_h_size"},
  {0x2410,  "isp_hist_v_size"},
  {0x2414,  "isp_hist_bin[0]"},
  {0x2418,  "isp_hist_bin[1]"},
  {0x241c,  "isp_hist_bin[2]"},
  {0x2420,  "isp_hist_bin[3]"},
  {0x2424,  "isp_hist_bin[4]"},
  {0x2428,  "isp_hist_bin[5]"},
  {0x242c,  "isp_hist_bin[6]"},
  {0x2430,  "isp_hist_bin[7]"},
  {0x2434,  "isp_hist_bin[8]"},
  {0x2438,  "isp_hist_bin[9]"},
  {0x243c,  "isp_hist_bin[10]"},
  {0x2440,  "isp_hist_bin[11]"},
  {0x2444,  "isp_hist_bin[12]"},
  {0x2448,  "isp_hist_bin[13]"},
  {0x244c,  "isp_hist_bin[14]"},
  {0x2450,  "isp_hist_bin[15]"},
#endif  // MARVIN_FEATURE_ISP_HISTOGRAM
#if (MARVIN_FEATURE_ISP_HISTOGRAM == MARVIN_FEATURE_HIST_V3)
  {0x2400,  "isp_hist_prop"},
  {0x2404,  "isp_hist_h_offs"},
  {0x2408,  "isp_hist_v_offs"},
  {0x240C,  "isp_hist_h_size"},
  {0x2410,  "isp_hist_v_size"},
  {0x2414,  "isp_hist_bin[0]"},
  {0x2418,  "isp_hist_bin[1]"},
  {0x241c,  "isp_hist_bin[2]"},
  {0x2420,  "isp_hist_bin[3]"},
  {0x2424,  "isp_hist_bin[4]"},
  {0x2428,  "isp_hist_bin[5]"},
  {0x242c,  "isp_hist_bin[6]"},
  {0x2430,  "isp_hist_bin[7]"},
  {0x2434,  "isp_hist_bin[8]"},
  {0x2438,  "isp_hist_bin[9]"},
  {0x243c,  "isp_hist_bin[10]"},
  {0x2440,  "isp_hist_bin[11]"},
  {0x2444,  "isp_hist_bin[12]"},
  {0x2448,  "isp_hist_bin[13]"},
  {0x244c,  "isp_hist_bin[14]"},
  {0x2450,  "isp_hist_bin[15]"},
  {0x2454,  "isp_hist_weight_00to30"},
  {0x2458,  "isp_hist_weight_40to21"},
  {0x245c,  "isp_hist_weight_31to12"},
  {0x2460,  "isp_hist_weight_22to03"},
  {0x2464,  "isp_hist_weight_13to43"},
  {0x2468,  "isp_hist_weight_04to34"},
  {0x246c,  "isp_hist_weight_44"},
#endif  // MARVIN_FEATURE_ISP_HISTOGRAM
#if (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V1)
  {0x2500,  "isp_filt_mode"},
  {0x2504,  "isp_filt_mask_sharp_0"},
  {0x2508,  "isp_filt_mask_sharp_1"},
  {0x250C,  "isp_filt_mask_sharp_diag"},
  {0x2510,  "isp_filt_mask_blur_max"},
  {0x2514,  "isp_filt_mask_blur"},
  {0x2518,  "isp_filt_mask_lin"},
  {0x251C,  "isp_filt_mask_orth"},
  {0x2520,  "isp_filt_mask_diag"},
  {0x2524,  "isp_filt_mask_h_diag"},
  {0x2528,  "isp_filt_blur_th0"},
  {0x252C,  "isp_filt_blur_th1"},
  {0x2530,  "isp_filt_sharp0_th"},
  {0x2534,  "isp_filt_sharp1_th"},
  {0x2538,  "isp_filt_lum_weight"},
#elif (MARVIN_FEATURE_ISP_FILTER == MARVIN_FEATURE_ISP_FILTER_V2)
  {0x2500,  "isp_filt_mode"},
  {0x2528,  "isp_filt_thresh_bl0"},
  {0x252C,  "isp_filt_thresh_bl1"},
  {0x2530,  "isp_filt_thresh_sh0"},
  {0x2534,  "isp_filt_thresh_sh1"},
  {0x2538,  "isp_filt_lum_weight"},
  {0x253C,  "isp_filt_fac_sh1"},
  {0x2540,  "isp_filt_fac_sh0"},
  {0x2544,  "isp_filt_fac_mid"},
  {0x2548,  "isp_filt_fac_bl0"},
  {0x254C,  "isp_filt_fac_bl1"},
#endif  // MARVIN_FEATURE_ISP_FILTER
#if (MARVIN_FEATURE_CHROM_ABERRATION == MARVIN_FEATURE_EXIST)
  {0x2580,  "isp_cac_ctrl"},
  {0x2584,  "isp_cac_count_start"},
  {0x2588,  "isp_cac_a"},
  {0x258C,  "isp_cac_b"},
  {0x2590,  "isp_cac_c"},
  {0x2594,  "isp_cac_x_norm"},
  {0x2598,  "isp_cac_y_norm"},
#endif // MARVIN_FEATURE_CHROM_ABERRATION
#if( ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V2 ) ||\
     ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4 ) )

#if (MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V2)
  {0x2600,  "isp_exp_ctrl"},
#else
  {0x2600,  "isp_exp_ctrl"},
#endif // MARVIN_FEATURE_AUTO_EXPOSURE: V2
  {0x2604,  "isp_exp_h_offset"},
  {0x2608,  "isp_exp_v_offset"},
  {0x260C,  "isp_exp_h_size"},
  {0x2610,  "isp_exp_v_size"},
  {0x2614,  "isp_exp_mean_00"},
  {0x2618,  "isp_exp_mean_10"},
  {0x261C,  "isp_exp_mean_20"},
  {0x2620,  "isp_exp_mean_30"},
  {0x2624,  "isp_exp_mean_40"},
  {0x2628,  "isp_exp_mean_01"},
  {0x262C,  "isp_exp_mean_11"},
  {0x2630,  "isp_exp_mean_21"},
  {0x2634,  "isp_exp_mean_31"},
  {0x2638,  "isp_exp_mean_41"},
  {0x263C,  "isp_exp_mean_02"},
  {0x2640,  "isp_exp_mean_12"},
  {0x2644,  "isp_exp_mean_22"},
  {0x2648,  "isp_exp_mean_32"},
  {0x264C,  "isp_exp_mean_42"},
  {0x2650,  "isp_exp_mean_03"},
  {0x2654,  "isp_exp_mean_13"},
  {0x2658,  "isp_exp_mean_23"},
  {0x265C,  "isp_exp_mean_33"},
  {0x2660,  "isp_exp_mean_43"},
  {0x2664,  "isp_exp_mean_04"},
  {0x2668,  "isp_exp_mean_14"},
  {0x266C,  "isp_exp_mean_24"},
  {0x2670,  "isp_exp_mean_34"},
  {0x2674,  "isp_exp_mean_44"},
#endif  // MARVIN_FEATURE_AUTO_EXPOSURE: V2,V4
#if ((MARVIN_FEATURE_BLACK_LEVEL >= MARVIN_FEATURE_BLACK_LEVEL_V4) && \
     (MARVIN_FEATURE_BLACK_LEVEL <= MARVIN_FEATURE_BLACK_LEVEL_V5))
  {0x2700,  "isp_bls_ctrl"},
  {0x2704,  "isp_bls_samples"},
  {0x2708,  "isp_bls_h1_start"},
  {0x270C,  "isp_bls_h1_stop"},
  {0x2710,  "isp_bls_v1_start"},
  {0x2714,  "isp_bls_v1_stop"},
  {0x2718,  "isp_bls_h2_start"},
  {0x271C,  "isp_bls_h2_stop"},
  {0x2720,  "isp_bls_v2_start"},
  {0x2724,  "isp_bls_v2_stop"},
  {0x2728,  "isp_bls_a_fixed"},
  {0x272C,  "isp_bls_b_fixed"},
  {0x2730,  "isp_bls_c_fixed"},
  {0x2734,  "isp_bls_d_fixed"},
  {0x2738,  "isp_bls_a_measured"},
  {0x273C,  "isp_bls_b_measured"},
  {0x2740,  "isp_bls_c_measured"},
  {0x2744,  "isp_bls_d_measured"},
#endif  // MARVIN_FEATURE_BLACK_LEVEL
#if (MARVIN_FEATURE_DPF == MARVIN_FEATURE_DPF_V1)
  {0x2800,  "isp_dpf_mode"},          
  {0x2804,  "dpf_strength_r"},   
  {0x2808,  "dpf_strength_g"},       
  {0x280C,  "dpf_strength_b"},  
  {0x2810,  "dpf_s_weight_g_1_4"},  
  {0x2814,  "dpf_s_weight_g_5_6"},  
  {0x2818,  "dpf_s_weight_rb_1_4"}, 
  {0x281C,  "dpf_s_weight_rb_5_6"},
  {0x2820,  "isp_dpf_nll_coeff_0"},      
  {0x2824,  "isp_dpf_nll_coeff_1"},  
  {0x2828,  "isp_dpf_nll_coeff_2"},      
  {0x282C,  "isp_dpf_nll_coeff_3"}, 
  {0x2830,  "isp_dpf_nll_coeff_4"},
  {0x2834,  "isp_dpf_nll_coeff_5"},      
  {0x2838,  "isp_dpf_nll_coeff_6"},  
  {0x283C,  "isp_dpf_nll_coeff_7"},      
  {0x2840,  "isp_dpf_nll_coeff_8"}, 
  {0x2844,  "isp_dpf_nll_coeff_9"},
  {0x2848,  "isp_dpf_nll_coeff_10"},      
  {0x284C,  "isp_dpf_nll_coeff_11"},  
  {0x2850,  "isp_dpf_nll_coeff_12"},      
  {0x2854,  "isp_dpf_nll_coeff_13"},     
  {0x2858,  "isp_dpf_nll_coeff_14"},      
  {0x285C,  "isp_dpf_nll_coeff_15"},      
  {0x2860,  "isp_dpf_nll_coeff_16"},    
  {0x2864,  "isp_dpf_nl_r_gain"},    
  {0x2868,  "isp_dpf_nl_gr_gain"},    
  {0x286C,  "isp_dpf_nl_gb_gain"},    
  {0x2870,  "isp_dpf_nl_b_gain"},    
#endif
#if (MARVIN_FEATURE_DPCC == MARVIN_FEATURE_DPCC_V1)
  {0x2900,  "isp_dpcc_mode"},          
  {0x2904,  "isp_dpcc_output_mode"},   
  {0x2908,  "isp_dpcc_set_use"},       
  {0x290C,  "isp_dpcc_methods_set1"},  
  {0x2910,  "isp_dpcc_methods_set2"},  
  {0x2914,  "isp_dpcc_methods_set3"},  
  {0x2918,  "isp_dpcc_line_thresh_1"}, 
  {0x291C,  "isp_dpcc_line_mad_fac_1"},
  {0x2920,  "isp_dpcc_pg_fac_1"},      
  {0x2924,  "isp_dpcc_rnd_thresh_1"},  
  {0x2928,  "isp_dpcc_rg_fac_1"},      
  {0x292C,  "isp_dpcc_line_thresh_2"}, 
  {0x2930,  "isp_dpcc_line_mad_fac_2"},
  {0x2934,  "isp_dpcc_pg_fac_2"},      
  {0x2938,  "isp_dpcc_rnd_thresh_2"},  
  {0x293C,  "isp_dpcc_rg_fac_2"},      
  {0x2940,  "isp_dpcc_line_thresh_3"}, 
  {0x2944,  "isp_dpcc_line_mad_fac_3"},
  {0x2948,  "isp_dpcc_pg_fac_3"},      
  {0x294C,  "isp_dpcc_rnd_thresh_3"},  
  {0x2950,  "isp_dpcc_rg_fac_3"},      
  {0x2954,  "isp_dpcc_ro_limits"},     
  {0x2958,  "isp_dpcc_rnd_offs"},      
  {0x295C,  "isp_dpcc_bpt_ctrl"},      
  {0x2960,  "isp_dpcc_bpt_number"},    
  {0x2964,  "isp_dpcc_bpt_addr"},      
  {0x2968,  "isp_dpcc_bpt_data"},      
#endif  // MARVIN_FEATURE_DPCC_V1
#if( MARVIN_FEATURE_WDR == MARVIN_FEATURE_WDR_V1 )   
  {0x2A00, "isp_wdr_ctrl"},   
  {0x2A04, "isp_wdr_tonecurve_dyn_1"},
  {0x2A08, "isp_wdr_tonecurve_dyn_2"},
  {0x2A0C, "isp_wdr_tonecurve_dyn_3"},
  {0x2A10, "isp_wdr_tonecurve_dyn_4"},
  {0x2A14, "isp_wdr_tonecurve_ym[0]"},
  {0x2A18, "isp_wdr_tonecurve_ym[1]"},
  {0x2A1C, "isp_wdr_tonecurve_ym[2]"},
  {0x2A20, "isp_wdr_tonecurve_ym[3]"},
  {0x2A24, "isp_wdr_tonecurve_ym[4]"},
  {0x2A28, "isp_wdr_tonecurve_ym[5]"},
  {0x2A2C, "isp_wdr_tonecurve_ym[6]"},
  {0x2A30, "isp_wdr_tonecurve_ym[7]"},
  {0x2A34, "isp_wdr_tonecurve_ym[8]"},
  {0x2A38, "isp_wdr_tonecurve_ym[9]"},
  {0x2A3C, "isp_wdr_tonecurve_ym[10]"},
  {0x2A40, "isp_wdr_tonecurve_ym[11]"},
  {0x2A44, "isp_wdr_tonecurve_ym[12]"},
  {0x2A48, "isp_wdr_tonecurve_ym[13]"},
  {0x2A4C, "isp_wdr_tonecurve_ym[14]"},
  {0x2A50, "isp_wdr_tonecurve_ym[15]"},
  {0x2A54, "isp_wdr_tonecurve_ym[16]"},
  {0x2A58, "isp_wdr_tonecurve_ym[17]"},
  {0x2A5C, "isp_wdr_tonecurve_ym[18]"},
  {0x2A60, "isp_wdr_tonecurve_ym[19]"},
  {0x2A64, "isp_wdr_tonecurve_ym[20]"},
  {0x2A68, "isp_wdr_tonecurve_ym[21]"},
  {0x2A6C, "isp_wdr_tonecurve_ym[22]"},
  {0x2A70, "isp_wdr_tonecurve_ym[23]"},
  {0x2A74, "isp_wdr_tonecurve_ym[24]"},
  {0x2A78, "isp_wdr_tonecurve_ym[25]"},
  {0x2A7C, "isp_wdr_tonecurve_ym[26]"},
  {0x2A80, "isp_wdr_tonecurve_ym[27]"},
  {0x2A84, "isp_wdr_tonecurve_ym[28]"},
  {0x2A88, "isp_wdr_tonecurve_ym[29]"},
  {0x2A8C, "isp_wdr_tonecurve_ym[30]"},
  {0x2A90, "isp_wdr_tonecurve_ym[31]"},
  {0x2A94, "isp_wdr_tonecurve_ym[32]"},
  {0x2A98, "isp_wdr_offset"},
  {0x2A9C, "isp_wdr_dmin_threshold"},
  {0x2AA0, "isp_wdr_tonecurve_dyn_1_shd"},
  {0x2AA4, "isp_wdr_tonecurve_dyn_2_shd"},
  {0x2AA8, "isp_wdr_tonecurve_dyn_3_shd"},
  {0x2AAC, "isp_wdr_tonecurve_dyn_4_shd"},
  {0x2AB0, "isp_wdr_tonecurve_ym_shd[0]"},
  {0x2AB4, "isp_wdr_tonecurve_ym_shd[1]"},
  {0x2AB8, "isp_wdr_tonecurve_ym_shd[2]"},
  {0x2ABC, "isp_wdr_tonecurve_ym_shd[3]"},
  {0x2AC0, "isp_wdr_tonecurve_ym_shd[4]"},
  {0x2AC4, "isp_wdr_tonecurve_ym_shd[5]"},
  {0x2AC8, "isp_wdr_tonecurve_ym_shd[6]"},
  {0x2ACC, "isp_wdr_tonecurve_ym_shd[7]"},
  {0x2AD0, "isp_wdr_tonecurve_ym_shd[8]"},
  {0x2AD4, "isp_wdr_tonecurve_ym_shd[9]"},
  {0x2AD8, "isp_wdr_tonecurve_ym_shd[10]"},
  {0x2ADC, "isp_wdr_tonecurve_ym_shd[11]"},
  {0x2AE0, "isp_wdr_tonecurve_ym_shd[12]"},
  {0x2AE4, "isp_wdr_tonecurve_ym_shd[13]"},
  {0x2AE8, "isp_wdr_tonecurve_ym_shd[14]"},
  {0x2AEC, "isp_wdr_tonecurve_ym_shd[15]"},
  {0x2AF0, "isp_wdr_tonecurve_ym_shd[16]"},
  {0x2AF4, "isp_wdr_tonecurve_ym_shd[17]"},
  {0x2AF8, "isp_wdr_tonecurve_ym_shd[18]"},
  {0x2AFC, "isp_wdr_tonecurve_ym_shd[19]"},
  {0x2B00, "isp_wdr_tonecurve_ym_shd[20]"},
  {0x2B04, "isp_wdr_tonecurve_ym_shd[21]"},
  {0x2B08, "isp_wdr_tonecurve_ym_shd[22]"},
  {0x2B0C, "isp_wdr_tonecurve_ym_shd[23]"},
  {0x2B10, "isp_wdr_tonecurve_ym_shd[24]"},
  {0x2B14, "isp_wdr_tonecurve_ym_shd[25]"},
  {0x2B18, "isp_wdr_tonecurve_ym_shd[26]"},
  {0x2B1C, "isp_wdr_tonecurve_ym_shd[27]"},
  {0x2B20, "isp_wdr_tonecurve_ym_shd[28]"},
  {0x2B24, "isp_wdr_tonecurve_ym_shd[29]"},
  {0x2B28, "isp_wdr_tonecurve_ym_shd[30]"},
  {0x2B2C, "isp_wdr_tonecurve_ym_shd[31]"},
  {0x2B30, "isp_wdr_tonecurve_ym_shd[32]"},
#endif // MARVIN_FEATURE_WDR_V1  
  {REGIDX_LAST,  "OUT OF RANGE"},
};


/*****************************************************************************/
/*!
 *  \if EMPTY_HEADER  <!-- Delete complete line, if Function Header is used -->
 *
 *  \FUNCTION     (empty) \n
 *
 *  \RETURNVALUE  Return Value 1  [Description] \n
 *                Return Value 2  [Description] \n
 *                Return Value n  [Description] \n
 *
 *  \PARAMETERS   Parameter 1     [Description] \n
 *                Parameter 2     [Description] \n
 *                Parameter n     [Description] \n
 *
 *  \NOTES        (empty) \n
 *                (empty new line) \n
 *
 *  \DESCRIPTION  (empty) \n
 *                (empty new line) \n
 *
 *  \endif  <!-- Delete complete line, if Function Header is used -->
 */
/*****************************************************************************/
UINT32 MrvReg_Read(UINT32 ulRegAddr)
{
    volatile UINT32* pulReg = (UINT32*)(MEM_MRV_REG_BASE + ulRegAddr);
    return REG_READ(*pulReg);
}


/*****************************************************************************/
/*!
 *  \if EMPTY_HEADER  <!-- Delete complete line, if Function Header is used -->
 *
 *  \FUNCTION     (empty) \n
 *
 *  \RETURNVALUE  Return Value 1  [Description] \n
 *                Return Value 2  [Description] \n
 *                Return Value n  [Description] \n
 *
 *  \PARAMETERS   Parameter 1     [Description] \n
 *                Parameter 2     [Description] \n
 *                Parameter n     [Description] \n
 *
 *  \NOTES        (empty) \n
 *                (empty new line) \n
 *
 *  \DESCRIPTION  (empty) \n
 *                (empty new line) \n
 *
 *  \endif  <!-- Delete complete line, if Function Header is used -->
 */
/*****************************************************************************/
void MrvReg_Write(UINT32 ulRegAddr, UINT32 ulData)
{
    volatile UINT32* pulReg = (UINT32*)(MEM_MRV_REG_BASE + ulRegAddr);
    REG_WRITE(*pulReg, ulData);
}


/*****************************************************************************/
/*!
 *  \if EMPTY_HEADER  <!-- Delete complete line, if Function Header is used -->
 *
 *  \FUNCTION     (empty) \n
 *
 *  \RETURNVALUE  Return Value 1  [Description] \n
 *                Return Value 2  [Description] \n
 *                Return Value n  [Description] \n
 *
 *  \PARAMETERS   Parameter 1     [Description] \n
 *                Parameter 2     [Description] \n
 *                Parameter n     [Description] \n
 *
 *  \NOTES        (empty) \n
 *                (empty new line) \n
 *
 *  \DESCRIPTION  (empty) \n
 *                (empty new line) \n
 *
 *  \endif  <!-- Delete complete line, if Function Header is used -->
 */
/*****************************************************************************/
const char* MrvReg_GetName(UINT32 ulRegAddr)
{
    const tsRegDescrReset* ptReg = g_MrvResetValuesTab;

    while (ptReg->ulOffset != REGIDX_LAST)
    {
        if (ptReg->ulOffset == ulRegAddr)
        {
            return ptReg->pszName;
        }

        ++ptReg;
    }

    return NULL;
}


/*****************************************************************************/
/*!
 *  \if EMPTY_HEADER  <!-- Delete complete line, if Function Header is used -->
 *
 *  \FUNCTION     (empty) \n
 *
 *  \RETURNVALUE  Return Value 1  [Description] \n
 *                Return Value 2  [Description] \n
 *                Return Value n  [Description] \n
 *
 *  \PARAMETERS   Parameter 1     [Description] \n
 *                Parameter 2     [Description] \n
 *                Parameter n     [Description] \n
 *
 *  \NOTES        (empty) \n
 *                (empty new line) \n
 *
 *  \DESCRIPTION  (empty) \n
 *                (empty new line) \n
 *
 *  \endif  <!-- Delete complete line, if Function Header is used -->
 */
/*****************************************************************************/
void MrvReg_DumpAll(FILE* hFile)
{
    const tsRegDescrReset* ptReg = g_MrvResetValuesTab;

    (void)fprintf(hFile,"*************************************************************\n");
    (void)fprintf(hFile,"* MARVIN REGISTERS                                          *\n");
    (void)fprintf(hFile,"*************************************************************\n");

    while (ptReg->ulOffset != REGIDX_LAST)
    {
        (void)fprintf(hFile, "%-25s @ 0x%04lX = 0x%08lX\n",
                      ptReg->pszName, ptReg->ulOffset, MrvReg_Read(ptReg->ulOffset));
        ++ptReg;
    }

}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvReg_DumpBPTable \n
 *  \RETURNVALUE NONE \n
 *
 *  \PARAMETERS  hFile file handle,
 *               ulBpNumber  number of members from the table \n
 *
 *               just a printout the bad pixel table into file \n
 *
 *  \NOTES         \n
 *
 *
 *  \DESCRIPTION just add MrvReg_DumpBPTable(); to your code to get \n
 *               the MARVINs BpTable printed into a file \n
 */
/*****************************************************************************/
void MrvReg_DumpBPTable(FILE* hFile, UINT32 ulBpNumber)
{
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_bp_ctrl = REG_READ(ptMrvReg->isp_bp_ctrl);
    UINT32 ulBpCtrlSave = isp_bp_ctrl;
    UINT32 uli = 0;

    ASSERT(ulBpNumber <= MRV_BP_BP_TABLE_ADDR_MAX);

    // disable BP
    REG_SET_SLICE(isp_bp_ctrl, MRV_BP_HOT_COR_EN,  OFF);
    REG_SET_SLICE(isp_bp_ctrl, MRV_BP_DEAD_COR_EN, OFF);
    REG_SET_SLICE(isp_bp_ctrl, MRV_BP_BP_DET_EN,   OFF);
    REG_WRITE(ptMrvReg->isp_bp_ctrl, isp_bp_ctrl);

    (void)fprintf(hFile,"*************************************************************\n");
    (void)fprintf(hFile,"* MARVIN Bp Table                                           *\n");
    (void)fprintf(hFile,"*************************************************************\n");

    REG_SET_SLICE(ptMrvReg->isp_bp_table_addr, MRV_BP_BP_TABLE_ADDR, 0);
    for (uli = 0; uli < ulBpNumber; uli++)
    { // use increment read out
        UINT32 isp_bp_table_data = REG_READ(ptMrvReg->isp_bp_table_data);
        (void)fprintf(hFile, "BP_table[%ld] : 0x%.8lX (%s, v %.4ld h %.4ld)\n", uli, isp_bp_table_data,
                      (REG_GET_SLICE(isp_bp_table_data, MRV_BP_PIX_TYPE) == MRV_BP_PIX_TYPE_HOT) ? "hot " : "dead",
                      REG_GET_SLICE(isp_bp_table_data, MRV_BP_V_ADDR),
                      REG_GET_SLICE(isp_bp_table_data, MRV_BP_H_ADDR));
    }

    // re-enable BP
    REG_WRITE(ptMrvReg->isp_bp_ctrl, ulBpCtrlSave);

    return;
#else // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    UNUSED_PARAM(hFile);
    UNUSED_PARAM(ulBpNumber);
    return;
#endif // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvReg_DumpIrqStatus \n
 *
 *  \RETURNVALUE NONE \n
 *
 *  \PARAMETERS  file handle to dump to \n
 *
 *  \NOTES       This is only a printout for debugging \n
 *
 *  \DESCRIPTION Printout all Interrupts Raw Status \n
 */
/*****************************************************************************/
void MrvReg_DumpIrqStatus(FILE* hFile)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 isp_ris        = REG_READ(ptMrvReg->isp_ris);
#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
    UINT32 emp_out_ris    = REG_READ(ptMrvReg->emp_out_ris);
#endif // MARVIN_FEATURE_SMALL_OUTUNIT
    UINT32 mi_ris         = REG_READ(ptMrvReg->mi_ris);
    UINT32 jpe_error_ris  = REG_READ(ptMrvReg->jpe_error_ris);
    UINT32 jpe_status_ris = REG_READ(ptMrvReg->jpe_status_ris);

    (void)fprintf(hFile, "#########IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ#########\n"); // changed to avoid LINT warnings (Warning 534)
#if( ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V2 ) ||\
     ( MARVIN_FEATURE_AUTO_EXPOSURE == MARVIN_FEATURE_AUTO_EXPOSURE_V4 ) )
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_EXP_END))        { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_EXP_END        //[18] Exposure measurement complete\n"); };
#endif // MARVIN_FEATURE_AUTO_EXPOSURE
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_FLASH_CAP))      { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_FLASH_CAP      //[17] Signaling captured frame\n"); };
#endif // MARVIN_FEATURE_FLASH_LIGHT
#if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_BP_DET))         { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_BP_DET         //[16] Bad pixel detected\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_BP_NEW_TAB_FUL)) { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_BP_NEW_TAB_FUL //[15] New Bad Pixel table is full\n"); };
#endif // #if (MARVIN_FEATURE_BAD_PIXEL == MARVIN_FEATURE_EXIST)
#if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_AFM_FIN))        { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_AFM_FIN        //[14] AF measurement finished: this interrupt is set when the first complete frame is calculated after enabling the AF measurement\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_AFM_LUM_OF))     { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_AFM_LUM_OF     //[13] Auto focus luminance overflow\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_AFM_SUM_OF))     { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_AFM_SUM_OF     //[12] Auto focus sum overflow\n"); };
#endif // #if (MARVIN_FEATURE_AF_MEASURE == MARVIN_FEATURE_EXIST)
#if (MARVIN_FEATURE_SHUTTER == MARVIN_FEATURE_EXIST)
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_SHUTTER_OFF))    { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_SHUTTER_OFF    //[11] Mechanical shutter is switched off\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_SHUTTER_ON))     { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_SHUTTER_ON     //[10] Mechanical shutter is switched on\n"); };
#endif // MARVIN_FEATURE_SHUTTER
#if (MARVIN_FEATURE_FLASH_LIGHT == MARVIN_FEATURE_EXIST)
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_FLASH_OFF))      { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_FLASH_OFF      // [9] Flash light is switched off\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_FLASH_ON))       { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_FLASH_ON       // [8] Flash light is switched on\n"); };
#endif // MARVIN_FEATURE_FLASH_LIGHT
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_V_START))        { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_V_START        // [7] Start edge of h_sync\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_V_START))        { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_V_START        // [6] Start edge of v_sync\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_FRAME_IN))       { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_FRAME_IN       // [5] sampled input frame is complete\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_AWB_DONE))       { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_AWB_DONE       // [4] White balancing measurement cycle is complete, results can be read out\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_PIC_SIZE_ERR))   { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_PIC_SIZE_ERR   // [3] pic size violation occurred, programming seems wrong\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_DATA_LOSS))      { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_DATA_LOSS      // [2] loss of data occurred within a line, processing failure\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_FRAME))          { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_FRAME          // [1] frame was completely put out\n"); };
    if(REG_GET_SLICE(isp_ris, MRV_ISP_RIS_ISP_OFF))        { (void)fprintf(hFile, "ISP: MRV_ISP_RIS_ISP_OFF        // [0] isp was turned off (vsynced) due to f_cnt reached or manual\n"); };

#if (MARVIN_FEATURE_SMALL_OUTUNIT == MARVIN_FEATURE_EXIST)
    if(REG_GET_SLICE(emp_out_ris, MRV_EMPOUT_FRAME_END))   { (void)fprintf(hFile, "EMP: MRV_EMPOUT_FRAME_END // [0] Raw status of CIF end of frame interrupt\n"); };
#endif // MARVIN_FEATURE_SMALL_OUTUNIT

// Note: most MI interrupt bits depend on MI version so use of _SHIFT defines is better solution to show bit number
#if( MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2 )
    if(REG_GET_SLICE(mi_ris, MRV_MI_DMA_READY))     { (void)fprintf(hFile, "MI : MRV_MI_DMA_READY           //[%2d] dma ready interrupt\n", MRV_MI_DMA_READY_SHIFT); };
#endif // ( MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2 )
    if(REG_GET_SLICE(mi_ris, MRV_MI_AHB_ERROR))     { (void)fprintf(hFile, "MI : MRV_MI_AHB_ERROR           //[%2d] AHB write or read error interrupt (from BVCI target interfaces)\n", MRV_MI_AHB_ERROR_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_WRAP_SP_CR))    { (void)fprintf(hFile, "MI : MRV_MI_WRAP_SP_CR          //[%2d] self picture Cr address wrap interrupt\n", MRV_MI_WRAP_SP_CR_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_WRAP_SP_CB))    { (void)fprintf(hFile, "MI : MRV_MI_WRAP_SP_CB          //[%2d] self picture Cb address wrap interrupt\n", MRV_MI_WRAP_SP_CB_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_WRAP_SP_Y))     { (void)fprintf(hFile, "MI : MRV_MI_WRAP_SP_Y           //[%2d] self picture Y address wrap interrupt\n", MRV_MI_WRAP_SP_Y_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_WRAP_MP_CR))    { (void)fprintf(hFile, "MI : MRV_MI_WRAP_MP_CR          //[%2d] main picture Cr address wrap interrupt\n", MRV_MI_WRAP_MP_CR_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_WRAP_MP_CB))    { (void)fprintf(hFile, "MI : MRV_MI_WRAP_MP_CB          //[%2d] main picture Cb address wrap interrupt\n", MRV_MI_WRAP_MP_CB_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_WRAP_MP_Y))     { (void)fprintf(hFile, "MI : MRV_MI_WRAP_MP_Y           //[%2d] main picture Y address wrap interrupt\n", MRV_MI_WRAP_MP_Y_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_FILL_MP_Y))     { (void)fprintf(hFile, "MI : MRV_MI_FILL_MP_Y           //[%2d] fill level interrupt of main picture Y, JPEG or raw data\n", MRV_MI_FILL_MP_Y_SHIFT); };
    if(REG_GET_SLICE(mi_ris, MRV_MI_MBLK_LINE))     { (void)fprintf(hFile, "MI : MRV_MI_MBLK_LINE           //[%2d] makroblock line interrupt of main picture (16 lines of Y, 8 lines of Cb and 8 lines of Cr are written into RAM)\n", MRV_MI_MBLK_LINE_SHIFT); };
#if ((MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V3) && (MARVIN_FEATURE_MI <= MARVIN_FEATURE_MI_V4))
    if(REG_GET_SLICE(mi_ris, MRV_MI_SP_FRAME_END))  { (void)fprintf(hFile, "MI : MRV_MI_SP_FRAME_END        //[%2d] self picture end of frame interrupt\n", MRV_MI_SP_FRAME_END_SHIFT); };
#elif (MARVIN_FEATURE_MI == MARVIN_FEATURE_MI_V2)
    if(REG_GET_SLICE(mi_ris, MRV_MI_DMA_FRAME_END)) { (void)fprintf(hFile, "MI : MRV_MI_DMA_FRAME_END       //[%2d] DMA end of frame interrupt\n", MRV_MI_DMA_FRAME_END_SHIFT); };
#endif // (MARVIN_FEATURE_MI >= MARVIN_FEATURE_MI_V2)
    if(REG_GET_SLICE(mi_ris, MRV_MI_MP_FRAME_END))  { (void)fprintf(hFile, "MI : MRV_MI_MP_FRAME_END        //[%2d] main picture end of frame interrupt\n", MRV_MI_MP_FRAME_END_SHIFT); };

    if(REG_GET_SLICE(jpe_error_ris,  MRV_JPE_VLC_TABLE_ERR))    { (void)fprintf(hFile, "JPE: MRV_JPE_VLC_TABLE_ERR      //[10] illegal table detected\n"); };
    if(REG_GET_SLICE(jpe_error_ris,  MRV_JPE_R2B_IMG_SIZE_ERR)) { (void)fprintf(hFile, "JPE: MRV_JPE_R2B_IMG_SIZE_ERR   // [9] mismatch of predefined h_size and v_size values with calculated values (encode mode)\n"); };
    if(REG_GET_SLICE(jpe_error_ris,  MRV_JPE_DCT_ERR))          { (void)fprintf(hFile, "JPE: MRV_JPE_DCT_ERR            // [7] block start mismatch\n"); };
    if(REG_GET_SLICE(jpe_error_ris,  MRV_JPE_VLC_SYMBOL_ERR))   { (void)fprintf(hFile, "JPE: MRV_JPE_VLC_SYMBOL_ERR     // [4] illegal symbol detected (encoding)\n"); };

    if(REG_GET_SLICE(jpe_status_ris, MRV_JPE_GEN_HEADER_DONE))  { (void)fprintf(hFile, "JPE: MRV_JPGINT_GEN_HEADER_DONE // [5] Stream header generation finished\n"); };
    if(REG_GET_SLICE(jpe_status_ris, MRV_JPE_ENCODE_DONE))      { (void)fprintf(hFile, "JPE: MRV_JPE_ENCODE_DONE        // [4] Encode processing finished\n"); };

    (void)fprintf(hFile, "#########IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ##################IRQ#########\n"); // changed to avoid LINT warnings (Warning 534)
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvReg_DumpDebugInfoInit \n
 *
 *  \RETURNVALUE none \n
 *
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Initializes the IMSC registers for dumping MARVIN debug
 *               status. \n
 *               Note: This will result in interrupts at the IRQ controller
 *                     of the complete system! \n
 */
/*****************************************************************************/
void MrvReg_DumpDebugInfoInit(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // store all current imsc states
    s_tMrvDbgStatus.ulIspImsc       = REG_READ(ptMrvReg->isp_imsc);
    s_tMrvDbgStatus.ulMiImsc        = REG_READ(ptMrvReg->mi_imsc);
    s_tMrvDbgStatus.ulJpeErrorImsc  = REG_READ(ptMrvReg->jpe_error_imr);
    s_tMrvDbgStatus.ulJpeStatusImsc = REG_READ(ptMrvReg->jpe_status_imr);

    // activate all interrupts
    REG_SET_SLICE(ptMrvReg->isp_imsc,       MRV_ISP_IMSC_ALL, ON);
    REG_SET_SLICE(ptMrvReg->mi_imsc,        MRV_MI_ALLIRQS,   ON);
    REG_SET_SLICE(ptMrvReg->jpe_error_imr,  MRV_JPE_ALL_ERR,  ON);
    REG_SET_SLICE(ptMrvReg->jpe_status_imr, MRV_JPE_ALL_STAT, ON);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvReg_DumpDebugInfoExit \n
 *
 *  \RETURNVALUE none \n
 *
 *  \PARAMETERS  none \n
 *
 *  \DESCRIPTION Restores the IMSC registers after dumping MARVIN debug
 *               status. \n
 */
/*****************************************************************************/
void MrvReg_DumpDebugInfoExit(void)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    // restore all current imsc states
    REG_WRITE(ptMrvReg->isp_imsc,       s_tMrvDbgStatus.ulIspImsc);
    REG_WRITE(ptMrvReg->mi_imsc,        s_tMrvDbgStatus.ulMiImsc);
    REG_WRITE(ptMrvReg->jpe_error_imr,  s_tMrvDbgStatus.ulJpeErrorImsc);
    REG_WRITE(ptMrvReg->jpe_status_imr, s_tMrvDbgStatus.ulJpeStatusImsc);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvReg_DumpDebugInfoNow \n
 *
 *  \RETURNVALUE none \n
 *
 *  \PARAMETERS  file handle to dump to \n
 *
 *  \DESCRIPTION Dumps the MARVIN debug status and clears the interrupts. \n
 */
/*****************************************************************************/
void MrvReg_DumpDebugInfoNow(FILE *ptFile)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;

    (void)fprintf(ptFile, "######### MrvReg_DumpDebugInfoNow ################################\n");
    (void)fprintf(ptFile, "ISP_MIS        = 0x%08lX\n", ptMrvReg->isp_mis);
#if (MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)
    (void)fprintf(ptFile, "ISP_ERR        = 0x%08lX\n", ptMrvReg->isp_err);
#endif // #if (MARVIN_FEATURE_ISP_ERRORFLAGS)
    (void)fprintf(ptFile, "MI_MIS         = 0x%08lX\n", ptMrvReg->mi_mis);
#if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
    (void)fprintf(ptFile, "MI_STATUS      = 0x%08lX\n", ptMrvReg->mi_status);
#endif // #if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
    (void)fprintf(ptFile, "JPE_DEBUG      = 0x%08lX\n", ptMrvReg->jpe_debug);
    (void)fprintf(ptFile, "JPE_ERROR_MIS  = 0x%08lX\n", ptMrvReg->jpe_error_mis);
    (void)fprintf(ptFile, "JPE_STATUS_MIS = 0x%08lX\n", ptMrvReg->jpe_status_mis);
    (void)fprintf(ptFile, "#########\n");

    // clear all status registers
    REG_SET_SLICE(ptMrvReg->isp_icr,        MRV_ISP_ICR_ALL,  ON);
#if (MARVIN_FEATURE_ISP_ERRORFLAGS == MARVIN_FEATURE_EXIST)
    REG_SET_SLICE(ptMrvReg->isp_err_clr,    MRV_ISP_ALL_ERR,  ON);
#endif // #if (MARVIN_FEATURE_ISP_ERRORFLAGS)
    REG_SET_SLICE(ptMrvReg->mi_icr,         MRV_MI_ALLIRQS,   ON);
#if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
    REG_SET_SLICE(ptMrvReg->mi_status_clr,  MRV_MI_ALL_STAT,  ON);
#endif // #if (MARVIN_FEATURE_MI_STATUSFLAGS == MARVIN_FEATURE_EXIST)
    REG_SET_SLICE(ptMrvReg->jpe_error_icr,  MRV_JPE_ALL_ERR,  ON);
    REG_SET_SLICE(ptMrvReg->jpe_status_icr, MRV_JPE_ALL_STAT, ON);
}


/*****************************************************************************/
/*!
 *  \FUNCTION    MrvReg_MonitorMrvInputStatus \n
 *
 *  \RETURNVALUE none \n
 *
 *  \PARAMETERS  file handle to dump to \n
 *               amount of microseconds to monitor \n
 *
 *  \DESCRIPTION Monitors the ISP_FLAGS_SHD register while the given amount
 *               of microseconds. \n
 */
/*****************************************************************************/
void MrvReg_MonitorMrvInputStatus(FILE  *ptFile, UINT32 ulMicroSec)
{
    volatile tsMrvRegister *ptMrvReg = (tsMrvRegister*)MEM_MRV_REG_BASE;
    UINT32 ulStartTick,ulTickFreq;
    ulTickFreq  = OS_GetFrequency();

    (void)fprintf(ptFile, "######### MrvReg_MonitorMrvInputStatus ###########################\n");

    ulStartTick = OS_GetTick();

    while (((OS_GetTick() - ulStartTick) / ulTickFreq*1000000) < ulMicroSec)
    {
        // debugging MARVIN ISP input status
        (void)fprintf(ptFile, "ISP_FLAGS_SHD = 0x%08lX\n", ptMrvReg->isp_flags_shd);
    }


    (void)fprintf(ptFile, "#########\n");
}


#else // MRV_SUPPORT_REGDUMP

// This is just to avoid the boring compiler warning "C3040E: no external declaration
// in translation unit" at minimum costs. Sadly, we can not temporarily disable warnings in
// ADS1.2, but we want to keep our code warning-free.
// Chances are good that this will be optimized away by the linker...
//static const char s_cDummy = 0;

#endif // MRV_SUPPORT_REGDUMP
